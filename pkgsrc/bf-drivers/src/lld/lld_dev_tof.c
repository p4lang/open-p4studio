/*******************************************************************************
 *  Copyright (C) 2024 Intel Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions
 *  and limitations under the License.
 *
 *
 *  SPDX-License-Identifier: Apache-2.0
 ******************************************************************************/


#ifndef __KERNEL__
#include "stdio.h"  // for fopen
#endif
#include <dvm/bf_drv_intf.h>
#include <lld/bf_dma_if.h>
#include <lld/bf_dev_if.h>
#include <lld/lld_reg_if.h>
#include <lld/lld_spi_if.h>
#include <lld/lld_err.h>
#include <lld/lld_sku.h>
#include <lld/lld_interrupt_if.h>
#include <lld/tofino_defs.h>
#include "lld.h"
#include "lld_dev.h"
#include "lld_map.h"
#include "lld_log.h"
#include <tofino_regs/tofino.h>
#include "lld_dev_tof.h"

// fwd refs
static lld_err_t lld_dev_tof_set_core_clk_1_25G(bf_dev_id_t dev_id,
                                                bool force_clk,
                                                uint32_t force_clk_val);
static lld_err_t lld_dev_tof_set_core_clk_1_1G(bf_dev_id_t dev_id);
static lld_err_t lld_dev_tof_core_pll_bypass_disable(bf_dev_id_t dev_id);
static lld_err_t lld_dev_tof_core_pll_bypass_enable(bf_dev_id_t dev_id);
static lld_err_t lld_dev_tof_mac_pll_bypass_disable(bf_dev_id_t dev_id);
static lld_err_t lld_dev_tof_mac_pll_bypass_enable(bf_dev_id_t dev_id);
static void lld_dev_tof_write_soft_reset(bf_dev_id_t dev_id, uint32_t val);

static int read_core_clk_from_file(uint32_t *clk_val) {
#ifndef BF_SLT
  (void)clk_val;
  /* Read from file is enabled only in SLT mode */
  return 1;
#else
#ifndef __KERNEL__
  uint32_t clk;
  FILE *pFile;
  pFile = fopen("/tmp/core_clk", "r");
  if (pFile != NULL) {
    fscanf(pFile, "%x", &clk);
    fclose(pFile);
    printf("Read clk=%x from /tmp/core_clk\n", clk);
    *clk_val = clk;
    return 0;
  } else {
    return 1;
  }
#else  /* __KERNEL__ */
  return 1;
#endif /* __KERNEL__ */
#endif /* BF_SLT */
}

/*************************************************
 * lld_dev_tof_set_core_clk
 *
 * If frequency_reduction is set in efuse
 * Changes the core clock speed to 1.1G
 * Returns LLD_OK on success
 *         LLD_ERR_LOCK_FAILED on failure
 *************************************************/
bf_status_t lld_dev_tof_set_core_clk(bf_dev_id_t dev_id,
                                     bool force_clk,
                                     uint32_t force_clk_val) {
  bf_sku_core_clk_freq_t bps_freq, pps_freq;
  lld_err_t err;

  err = lld_sku_get_core_clk_freq(dev_id, &bps_freq, &pps_freq);
  if (err != LLD_OK) return BF_INVALID_ARG;
  /*
   * fuse_freq[1:0]
   * 2'b00 - no constraint
   * 2'b01 - 1.1G
   * 2'b11 - 1G
   */
  switch (bps_freq) {
    case BF_SKU_CORE_CLK_1_25_GHZ:
      err = lld_dev_tof_set_core_clk_1_25G(dev_id, force_clk, force_clk_val);
      break;
    case BF_SKU_CORE_CLK_1_1_GHZ:
      err = lld_dev_tof_set_core_clk_1_1G(dev_id);
      break;
    case BF_SKU_CORE_CLK_1_0_GHZ:
      err = LLD_OK;  // default
      break;
    default:
      lld_log("Error: EFUSE frequency reduction mis-programmed: dev=%d",
              dev_id);
      err = LLD_ERR_INVALID_CFG;
  }
  return ((err == LLD_OK) ? BF_SUCCESS : BF_INVALID_ARG);
}

/*************************************************
 * lld_dev_tof_set_core_clk_1_25G
 *
 * Changes th ecore clock speed to 1.25G
 * Returns LLD_OK on success
 *         LLD_ERR_LOCK_FAILED on failure
 *************************************************/
static lld_err_t lld_dev_tof_set_core_clk_1_25G(bf_dev_id_t dev_id,
                                                bool force_clk,
                                                uint32_t force_clk_val) {
  int cnt;
  uint32_t val, rst;

  /* reset the core PLL */
  lld_read_register(
      dev_id, DEF_tofino_device_select_misc_regs_soft_reset_address, &rst);
  rst |= 0x4; /* core pll reset */
  lld_dev_tof_write_soft_reset(dev_id, rst);

  /* Set PLL bypass bit (31). Modify the clock settings */
  if (force_clk) {
    val = force_clk_val;
  } else {
    if (read_core_clk_from_file(&val)) {
      val = 0xcd44cbfe;
    }
  }
#ifndef __KERNEL__
  printf("Setting core_pll_ctrl0=%x\n", val);
#endif /* __KERNEL__ */

  lld_log("EFUSE: Set core clk to 1.22GHz, dev=%d, PLL ctrl0 value=0x%x",
          dev_id,
          val);
  lld_write_register(
      dev_id, DEF_tofino_device_select_misc_regs_core_pll_ctrl0_address, val);

  /* un-reset the core PLL */
  rst &= 0xFFFFFFFB;
  lld_dev_tof_write_soft_reset(dev_id, rst);

  /* wait for core PLL to lock; maximum wait for 100 ms */
  cnt = 0;
  do {
    bf_sys_usleep(1000); /* sleep for 1ms */
    lld_read_register(
        dev_id, DEF_tofino_device_select_misc_regs_dbg_rst1_address, &val);
  } while (((val & 1) == 0) && (cnt++ <= 100));

  if (cnt >= 100) {
    lld_log(
        "Core PLL has not locked even after 100 ms; PLL Lock chip %d dbg_rst "
        "%x",
        dev_id,
        val);
  }
  return LLD_OK;
}

/*************************************************
 * lld_dev_tof_set_core_clk_1.1G
 *
 * Changes th ecore clock speed to 1.1G
 * Returns LLD_OK on success
 *         LLD_ERR_LOCK_FAILED on failure
 *************************************************/
static lld_err_t lld_dev_tof_set_core_clk_1_1G(bf_dev_id_t dev_id) {
  int cnt;
  uint32_t val, rst;

  lld_log("EFUSE: Set core clk to 1.1Ghz, dev=%d", dev_id);

  /* reset the core PLL */
  lld_read_register(
      dev_id, DEF_tofino_device_select_misc_regs_soft_reset_address, &rst);
  rst |= 0x4; /* core pll reset */
  lld_dev_tof_write_soft_reset(dev_id, rst);

  /* Set PLL bypass bit (31). Modify the clock settings */
  val = 0xC54558FE;
  lld_write_register(
      dev_id, DEF_tofino_device_select_misc_regs_core_pll_ctrl0_address, val);

  /* un-reset the core PLL */
  rst &= 0xFFFFFFFB;
  lld_dev_tof_write_soft_reset(dev_id, rst);

  /* wait for core PLL to lock; maximum wait for 100 ms */
  cnt = 0;
  do {
    bf_sys_usleep(1000); /* sleep for 1ms */
    lld_read_register(
        dev_id, DEF_tofino_device_select_misc_regs_dbg_rst1_address, &val);
  } while (((val & 1) == 0) && (cnt++ <= 100));

  if (cnt >= 100) {
    lld_log("Error PLL Lock chip %d dbg_rst %x", dev_id, val);
    return LLD_ERR_LOCK_FAILED;
  }
  return LLD_OK;
}

/*************************************************
 * lld_dev_tof_core_pll_bypass_disable
 *
 * Disable bypass setting of the CORE PLL
 * Returns LLD_OK on success
 *************************************************/
static lld_err_t lld_dev_tof_core_pll_bypass_disable(bf_dev_id_t dev_id) {
  uint32_t core_pll_ctrl;

  /* Clear the Core PLL bypass bit (31) */
  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_core_pll_ctrl0_address,
                    &core_pll_ctrl);
  core_pll_ctrl &= ~(0x80000000);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_core_pll_ctrl0_address,
                     core_pll_ctrl);

  return LLD_OK;
}

/*************************************************
 * lld_dev_tof_mac_pll_bypass_disable
 *
 * Disable bypass setting of the MAC PLL
 * Returns LLD_OK on success
 *************************************************/
static lld_err_t lld_dev_tof_mac_pll_bypass_disable(bf_dev_id_t dev_id) {
  uint32_t mac_pll_ctrl;

  /* Clear the MAC PLL bypass bit (31) */
  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_mac_pll_ctrl0_address,
                    &mac_pll_ctrl);
  mac_pll_ctrl &= ~(0x80000000);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_mac_pll_ctrl0_address,
                     mac_pll_ctrl);

  return LLD_OK;
}

/*************************************************
 * lld_dev_tof_core_pll_bypass_enable
 *
 * Enable bypass setting of the CORE PLL
 * Returns LLD_OK on success
 *************************************************/
static lld_err_t lld_dev_tof_core_pll_bypass_enable(bf_dev_id_t dev_id) {
  uint32_t core_pll_ctrl;

  /* Set the Core PLL bypass bit (31) */
  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_core_pll_ctrl0_address,
                    &core_pll_ctrl);
  core_pll_ctrl |= 0x80000000;
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_core_pll_ctrl0_address,
                     core_pll_ctrl);

  return LLD_OK;
}

/*************************************************
 * lld_dev_tof_mac_pll_bypass_enable
 *
 * Enable bypass setting of the MAC PLL
 * Returns LLD_OK on success
 *************************************************/
static lld_err_t lld_dev_tof_mac_pll_bypass_enable(bf_dev_id_t dev_id) {
  uint32_t mac_pll_ctrl;

  /* Set the MAC PLL bypass bit (31) */
  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_mac_pll_ctrl0_address,
                    &mac_pll_ctrl);
  mac_pll_ctrl |= 0x80000000;
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_mac_pll_ctrl0_address,
                     mac_pll_ctrl);

  return LLD_OK;
}

/************************************************************
* lld_dev_tof_parde_ring_full_threshold_set
  Set parde ring full threshold.
  Set max num of outstanding transactions to 1 on Tofino, which
  fixes a HW issue where multiple DMA's causes issues in parde.
*
************************************************************/
static lld_err_t lld_dev_tof_parde_ring_full_threshold_set(bf_dev_id_t dev_id) {
  uint32_t num_pipes = 0;
  bf_dev_pipe_t pipe = 0;
  bf_dev_pipe_t phy_pipe = 0;
  lld_err_t err = LLD_OK;
  uint32_t ctrl_val = 0;

  err = lld_sku_get_num_active_pipes(dev_id, &num_pipes);
  if (err != LLD_OK) {
    return err;
  }

  /* Set all fields to 1 */
  setp_party_glue_reg_rspec_csr_ring_full_thresh_e_ind(&ctrl_val, 1);
  setp_party_glue_reg_rspec_csr_ring_full_thresh_e_any(&ctrl_val, 1);
  setp_party_glue_reg_rspec_csr_ring_full_thresh_i_ind(&ctrl_val, 1);
  setp_party_glue_reg_rspec_csr_ring_full_thresh_i_any(&ctrl_val, 1);

  for (pipe = 0; pipe < num_pipes; pipe++) {
    err = lld_sku_map_pipe_id_to_phy_pipe_id(dev_id, pipe, &phy_pipe);
    if (err != LLD_OK) {
      return err;
    }

    lld_write_register(
        dev_id,
        DEF_tofino_pipes_pmarb_party_glue_reg_csr_ring_full_thresh_address +
            (phy_pipe * DEF_tofino_pipes_array_element_size),
        ctrl_val);
  }

  return LLD_OK;
}

/*************************************************
 * lld_dev_tof_un_reset
 *
 * Bring all blocks out of reset
 *************************************************/
bf_status_t lld_dev_tof_un_reset(bf_dev_id_t dev_id) {
  int polls = 0, max_polls = 10, reset_done = 0;
  uint32_t resets;
  lld_err_t err;

  /* Enable CORE and MAC PLL bypass */
  lld_dev_tof_core_pll_bypass_enable(dev_id);
  lld_dev_tof_mac_pll_bypass_enable(dev_id);

  /* Put Core, MBUS and MAC in soft reset */
  lld_dev_tof_write_soft_reset(dev_id, 0x270);

  /* allow 10 usec reset  pulse period */
  bf_sys_usleep(10);

  /* Take MBUS out of reset. Keep Core and MAC in reset */
  lld_dev_tof_write_soft_reset(dev_id, 0x260);

  bf_sys_usleep(10);

  /* Enable termination on ETH_REF_CLK */
  lld_write_register(
      dev_id, DEF_tofino_ethgpiobr_gpio_common_regs_refclk_ctrl_address, 0xBB);
  lld_write_register(
      dev_id, DEF_tofino_ethgpiotl_gpio_common_regs_refclk_ctrl_address, 0xBB);

  /* Wait for ETH_REF_CLK termination to complete */
  bf_sys_usleep(100);

  /* Reset Core-PLL and wait for it to lock */
  err = lld_dev_tof_set_core_clk(dev_id, false, 0);
  if (err != LLD_OK) {
    lld_log("Warning: Core clock on dev=%d may not be set correctly!", dev_id);
  }

  /* Take Core and MAC out of reset */
  lld_dev_tof_write_soft_reset(dev_id, 0x200);

  /* Disable CORE and MAC PLL bypass */
  lld_dev_tof_core_pll_bypass_disable(dev_id);
  lld_dev_tof_mac_pll_bypass_disable(dev_id);

  /* de-assert reset */
  while (!reset_done) {
    lld_read_register(
        dev_id, DEF_tofino_device_select_misc_regs_dbg_rst1_address, &resets);
    reset_done = ((resets & 0x39) == 0x1);
    polls++;
    if (!reset_done) {
      if (polls >= max_polls) {
        lld_log("[Unreset] Failed (note: will fail on arch model)");
        return BF_INVALID_ARG;
      }
      lld_log("[Unreset] In progress: %08x", resets);
    }
  }
  lld_log("[Unreset] Done after %d polls", polls);

  /* Set parde ring full threshold */
  err = lld_dev_tof_parde_ring_full_threshold_set(dev_id);
  if (err != LLD_OK) {
    lld_log("ERROR: Failed to program parde ring full threshold on dev=%d",
            dev_id);
  }
  return BF_SUCCESS;
}

/********************************************************************
 * lld_dev_tof_reset_core
 *
 * Reset core blocks, leaving NIF untouched
 *******************************************************************/
bf_status_t lld_dev_tof_reset_core(bf_dev_id_t dev_id) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  lld_err_t err = LLD_OK;

  if (dev_p == NULL) return BF_INVALID_ARG;

  /* Enable CORE PLL bypass */
  lld_dev_tof_core_pll_bypass_enable(dev_id);

  /* Set bit 6 */
  lld_dev_tof_write_soft_reset(dev_id, 0x240);

  /* Unset bit 6 */
  lld_dev_tof_write_soft_reset(dev_id, 0x200);

  /* Disable CORE PLL bypass */
  lld_dev_tof_core_pll_bypass_disable(dev_id);

  /* Set parde ring full threshold */
  err = lld_dev_tof_parde_ring_full_threshold_set(dev_id);
  if (err != LLD_OK) {
    lld_log("ERROR: Failed to program parde ring full threshold on dev=%d",
            dev_id);
  }

  return BF_SUCCESS;
}

typedef enum {
  BF_TCU_STATE_RESET_VALUE = (int)(-(-0xbabefeed)),
  BF_TCU_STATE_FLUSH_START = (int)(-(-0xffffbbbb)),
  BF_TCU_STATE_FLUSH_DONE = (int)(-(-0xffffdddd)),
  BF_TCU_STATE_REPAIR_START = (int)(-(-0xbfbbbbbb)),
  BF_TCU_STATE_REPAIR_DONE = (int)(-(-0xbfdddddd))
} bf_tcu_state_t;

#define tcu_st_reg                                                                      \
  DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_scratch_reg_address +              \
      (DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_scratch_reg_array_index_max * \
       DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_scratch_reg_array_element_size)

/*****************************************************************************
 * lld_dev_tof_tcu_state_set
 *****************************************************************************/
static void lld_dev_tof_tcu_state_set(bf_dev_id_t dev_id, bf_tcu_state_t st) {
  lld_write_register(dev_id, tcu_st_reg, st);
}

/*****************************************************************************
 * lld_dev_tof_tcu_state_get
 *****************************************************************************/
static bf_tcu_state_t lld_dev_tof_tcu_state_get(bf_dev_id_t dev_id) {
  bf_tcu_state_t st;

  lld_read_register(dev_id, tcu_st_reg, (uint32_t *)&st);
  return st;
}

/********************************************************************
 * lld_dev_tof_tcu_flush_seq
 *
 * Flush out any incomplete transaction that may be present in
 * the path,
 *
 * #Flush out any previous garbage from hold_din register
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control0 00000000
 * wr dev_0 device_select misc_regs tcu_control1 80001FF1
 *
 * rr dev_0 device_select misc_regs tcu_wrack
 * rr dev_0 device_select misc_regs tcu_status
 *******************************************************************/
static void lld_dev_tof_tcu_flush_seq(bf_dev_id_t dev_id) {
  uint32_t tcu_wrack = 0, tcu_status = 0;
  int n;

  lld_dev_tof_tcu_state_set(dev_id, BF_TCU_STATE_FLUSH_START);

  for (n = 0; n < 16; n++) {
    lld_write_register(
        dev_id, DEF_tofino_device_select_misc_regs_tcu_control0_address, 0x0);
    bf_sys_usleep(100);
  }
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control1_address,
                     0x80001FF1);
  bf_sys_usleep(200);

  lld_read_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_wrack_address, &tcu_wrack);
  lld_log("tcu_wrack register value on dev=%d. Val : 0x%x", dev_id, tcu_wrack);

  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_tcu_status_address,
                    &tcu_status);
  lld_log(
      "tcu_status register value on dev=%d. Val : 0x%x", dev_id, tcu_status);

  lld_dev_tof_tcu_state_set(dev_id, BF_TCU_STATE_FLUSH_DONE);
}

/********************************************************************
 * lld_dev_tof_tcu_seq
 *
 * Put the chip in test mode so that it can repair the memories.
 * Needs to be done only during COLD boot
 *******************************************************************/
bf_status_t lld_dev_tof_tcu_seq(bf_dev_id_t dev_id) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  uint32_t tcu_wrack = 0, tcu_status = 0;
  bf_tcu_state_t st;

  if (dev_p == NULL) return BF_INVALID_ARG;

  if ((st = lld_dev_tof_tcu_state_get(dev_id)) == BF_TCU_STATE_REPAIR_DONE) {
    lld_log("TCU: %d : Memory repair already done <%08x>", dev_id, st);
    return BF_SUCCESS;
  }
  if (st != BF_TCU_STATE_RESET_VALUE) {
    lld_log("TCU: %d : WARNING: device requires Power-on Reset: <%08x>",
            dev_id,
            st);
  }

  // start by flushing out any pre-existing transaction
  lld_log("TCU: %d : Flush..", dev_id);
  lld_dev_tof_tcu_flush_seq(dev_id);
  lld_log("TCU: %d : Flush done", dev_id);

  lld_dev_tof_tcu_state_set(dev_id, BF_TCU_STATE_REPAIR_START);
  lld_log("TCU: %d : Repair..", dev_id);

  bf_sys_usleep(100);  // 100 msec

  lld_write_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_control0_address, 0x2b4);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control1_address,
                     0x80000091);
  bf_sys_usleep(200);

  lld_read_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_wrack_address, &tcu_wrack);
  bf_sys_usleep(100);
  lld_log("tcu_wrack register value on dev=%d. Val : 0x%x", dev_id, tcu_wrack);
  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_tcu_status_address,
                    &tcu_status);
  bf_sys_usleep(100);
  lld_log(
      "tcu_status register value on dev=%d. Val : 0x%x", dev_id, tcu_status);

  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x00000001);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x00000000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x00000000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xc0000b82);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffff9401);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x01e7ffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x00000000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xb4494000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x0000002d);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x00000004);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffff0000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xfffff023);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control1_address,
                     0x80001FF8);
  bf_sys_usleep(200);
  lld_read_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_wrack_address, &tcu_wrack);
  bf_sys_usleep(100);
  lld_log("tcu_wrack register value on dev=%d. Val : 0x%x", dev_id, tcu_wrack);
  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_tcu_status_address,
                    &tcu_status);
  bf_sys_usleep(100);
  lld_log(
      "tcu_status register value on dev=%d. Val : 0x%x", dev_id, tcu_status);

  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x0000ffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x00000000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x00000000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xfff00000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x2fffffff);
  bf_sys_usleep(100);
  lld_write_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_control0_address, 0x47b06);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control1_address,
                     0x80001724);
  bf_sys_usleep(200);
  lld_read_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_wrack_address, &tcu_wrack);
  bf_sys_usleep(100);
  lld_log("tcu_wrack register value on dev=%d. Val : 0x%x", dev_id, tcu_wrack);
  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_tcu_status_address,
                    &tcu_status);
  bf_sys_usleep(100);
  lld_log(
      "tcu_status register value on dev=%d. Val : 0x%x", dev_id, tcu_status);
  bf_sys_usleep(100);

  lld_write_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_control0_address, 0x2c0);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control1_address,
                     0x80000091);
  bf_sys_usleep(200);
  lld_read_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_wrack_address, &tcu_wrack);
  bf_sys_usleep(100);
  lld_log("tcu_wrack register value on dev=%d. Val : 0x%x", dev_id, tcu_wrack);
  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_tcu_status_address,
                    &tcu_status);
  bf_sys_usleep(100);
  lld_log(
      "tcu_status register value on dev=%d. Val : 0x%x", dev_id, tcu_status);
  bf_sys_usleep(100);

  lld_write_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_control0_address, 0x42);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control1_address,
                     0x80000064);
  bf_sys_usleep(200);
  lld_read_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_wrack_address, &tcu_wrack);
  bf_sys_usleep(100);
  lld_log("tcu_wrack register value on dev=%d. Val : 0x%x", dev_id, tcu_wrack);
  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_tcu_status_address,
                    &tcu_status);
  bf_sys_usleep(100);
  lld_log(
      "tcu_status register value on dev=%d. Val : 0x%x", dev_id, tcu_status);

  lld_write_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_control0_address, 0x2c1);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control1_address,
                     0x80000091);
  bf_sys_usleep(200);
  lld_read_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_wrack_address, &tcu_wrack);
  bf_sys_usleep(100);
  lld_log("tcu_wrack register value on dev=%d. Val : 0x%x", dev_id, tcu_wrack);
  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_tcu_status_address,
                    &tcu_status);
  bf_sys_usleep(100);
  lld_log(
      "tcu_status register value on dev=%d. Val : 0x%x", dev_id, tcu_status);

  lld_write_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_control0_address, 0x2);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control1_address,
                     0x80000014);
  bf_sys_usleep(200);
  lld_read_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_wrack_address, &tcu_wrack);
  bf_sys_usleep(100);
  lld_log("tcu_wrack register value on dev=%d. Val : 0x%x", dev_id, tcu_wrack);
  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_tcu_status_address,
                    &tcu_status);
  bf_sys_usleep(100);
  lld_log(
      "tcu_status register value on dev=%d. Val : 0x%x", dev_id, tcu_status);

  lld_write_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_control0_address, 0x2c0);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control1_address,
                     0x80000091);
  bf_sys_usleep(200);
  lld_read_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_wrack_address, &tcu_wrack);
  bf_sys_usleep(100);
  lld_log("tcu_wrack register value on dev=%d. Val : 0x%x", dev_id, tcu_wrack);
  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_tcu_status_address,
                    &tcu_status);
  bf_sys_usleep(100);
  lld_log(
      "tcu_status register value on dev=%d. Val : 0x%x", dev_id, tcu_status);

  lld_write_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_control0_address, 0x66);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control1_address,
                     0x80000064);
  // Insert delay greater than the following estimate
  // DELAY 634100 TCK cycles (1 cycle = 100ns) : 63410 us
  bf_sys_usleep(100000);  // 100 msec

  lld_read_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_wrack_address, &tcu_wrack);
  bf_sys_usleep(100);
  lld_log("tcu_wrack register value on dev=%d. Val : 0x%x", dev_id, tcu_wrack);
  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_tcu_status_address,
                    &tcu_status);
  bf_sys_usleep(100);
  lld_log(
      "tcu_status register value on dev=%d. Val : 0x%x", dev_id, tcu_status);
  bf_sys_usleep(100);

  lld_write_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_control0_address, 0x2c1);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control1_address,
                     0x80000091);
  bf_sys_usleep(200);
  lld_read_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_wrack_address, &tcu_wrack);
  lld_log("tcu_wrack register value on dev=%d. Val : 0x%x", dev_id, tcu_wrack);
  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_tcu_status_address,
                    &tcu_status);
  lld_log(
      "tcu_status register value on dev=%d. Val : 0x%x", dev_id, tcu_status);

  lld_write_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_control0_address, 0x00);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control1_address,
                     0x80000064);
  bf_sys_usleep(200);

  lld_read_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_wrack_address, &tcu_wrack);
  lld_log("tcu_wrack register value on dev=%d. Val : 0x%x", dev_id, tcu_wrack);
  if (tcu_wrack != 0x1) {
    lld_log(
        "ERROR: tcu_wrack register value is not correct on dev=%d. Expected : "
        "0x01 , Actual : 0x%x",
        dev_id,
        tcu_wrack);
  }
  lld_read_register(dev_id,
                    DEF_tofino_device_select_misc_regs_tcu_status_address,
                    &tcu_status);
  lld_log(
      "tcu_status register value on dev=%d. Val : 0x%x", dev_id, tcu_status);
  if (tcu_status != 0x2a && tcu_status != 0x22) {
    lld_log(
        "ERROR: tcu_status register value is not correct on dev=%d. Expected : "
        "0x2a , Actual : 0x%x",
        dev_id,
        tcu_status);
  }

  bf_sys_usleep(100);
  lld_write_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_control0_address, 0x2b4);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control1_address,
                     0x80000091);
  bf_sys_usleep(200);

  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x00000001);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x00000000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x00000000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xc0000b82);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffff9401);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x01e7ffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x00000000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xb4494000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x0000002d);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x00000000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffff0000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xfffff023);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control1_address,
                     0x80001FF8);
  bf_sys_usleep(200);

  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x0000ffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x00000000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x00000000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xfff00000);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0xffffffff);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control0_address,
                     0x2fffffff);
  bf_sys_usleep(100);
  lld_write_register(
      dev_id, DEF_tofino_device_select_misc_regs_tcu_control0_address, 0x47b06);
  bf_sys_usleep(100);
  lld_write_register(dev_id,
                     DEF_tofino_device_select_misc_regs_tcu_control1_address,
                     0x80001724);
  bf_sys_usleep(100);

  bf_sys_usleep(10000);  // 10 msec

  lld_dev_tof_tcu_state_set(dev_id, BF_TCU_STATE_REPAIR_DONE);
  lld_log("TCU: %d : Repair done.", dev_id);

  return BF_SUCCESS;
}

/********************************************************************
 * lld_dev_tof_write_soft_reset
 *
 * Special handler for soft_reset register to guarantee bit 25
 * (chicken bit) is set correctly.
 *******************************************************************/
static void lld_dev_tof_write_soft_reset(bf_dev_id_t dev_id, uint32_t val) {
  // Always leave bit 25 set
  val |= (1 << 25);

  lld_write_register(
      dev_id, DEF_tofino_device_select_misc_regs_soft_reset_address, val);
}

/********************************************************************
 * lld_dev_tof_change_core_clk
 *
 * Changes Tofino core clk frequency
 *******************************************************************/
bf_status_t lld_dev_tof_change_core_clk(bf_dev_id_t dev_id, uint32_t clk_val) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  bf_status_t status = BF_SUCCESS;

  if (dev_p == NULL) return BF_INVALID_ARG;

  /* Enable CORE PLL bypass */
  lld_dev_tof_core_pll_bypass_enable(dev_id);

  /* Set the new core clk value */
  status = lld_dev_tof_set_core_clk(dev_id, true, clk_val);

  /* Disable CORE PLL bypass */
  lld_dev_tof_core_pll_bypass_disable(dev_id);

  return status;
}

/********************************************************************
 * lld_dev_tof_get_core_clk
 *
 * Get Tofino core clk frequency
 *******************************************************************/
bf_status_t lld_dev_tof_get_core_clk(bf_dev_id_t dev_id, uint32_t *clk_val) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  lld_err_t err = LLD_OK;

  if (dev_p == NULL) return BF_INVALID_ARG;

  err = lld_read_register(
      dev_id,
      DEF_tofino_device_select_misc_regs_core_pll_ctrl0_address,
      clk_val);
  if (err != LLD_OK) {
    return BF_INVALID_ARG;
  }

  return BF_SUCCESS;
}

bf_status_t lld_dev_tof_tlp_poison_set(bf_dev_id_t dev_id, bool en) {
  uint32_t offset, cpu_glb_ctrl;

  offset =
      DEF_tofino_device_select_pcie_bar01_regs_pcie_regs_cpu_glb_ctrl_address;
  lld_subdev_read_register(dev_id, 0, offset, &cpu_glb_ctrl);
  setp_cpu_glb_ctrl_ena_pois(&cpu_glb_ctrl, (en ? 1 : 0));
  setp_cpu_glb_ctrl_data_pois(&cpu_glb_ctrl, 1);
  lld_subdev_write_register(dev_id, 0, offset, cpu_glb_ctrl);
  return BF_SUCCESS;
}
