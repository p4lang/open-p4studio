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


#include <dvm/bf_drv_intf.h>
#include <lld/bf_dma_if.h>
#include <lld/bf_dev_if.h>
#include <lld/lld_reg_if.h>
#include <lld/lld_spi_if.h>
#include <lld/lld_err.h>
#include <lld/lld_sku.h>
#include "lld_tof3_interrupt.h"
#include "lld.h"
#include "lld_dev.h"
#include "lld_dev_tof3.h"
#include "lld_map.h"
#include "lld_log.h"
#include <tof3_regs/tof3_reg_drv.h>

#ifndef __KERNEL__
static void read_core_clk_from_file(uint32_t *bps_val,
                                    uint32_t *pps_val,
                                    uint32_t *tm_val) {
  uint32_t bps = 0, pps = 0, tm = 0;
  FILE *pFile;
  pFile = fopen("/tmp/core_clk", "r");
  if (pFile != NULL) {
    if (1 != fscanf(pFile, "%x", &bps)) {
      printf("Unable to read clock value from /tmp/core_clk\n");
    } else {
      printf("Read bps_clk=%x from /tmp/core_clk\n", bps);
    }
    fclose(pFile);
  }
  pFile = fopen("/tmp/pps_clk", "r");
  if (pFile != NULL) {
    if (1 != fscanf(pFile, "%x", &pps)) {
      printf("Unable to read PPS clock from /tmp/pps_clk\n");
    } else {
      printf("Read pps_clk=%x from /tmp/pps_clk\n", pps);
    }
    fclose(pFile);
  }
  pFile = fopen("/tmp/tm_clk", "r");
  if (pFile != NULL) {
    if (1 != fscanf(pFile, "%x", &tm)) {
      printf("Unable to read TM clock from /tmp/tm_clk\n");
    } else {
      printf("Read tm_clk=%x from /tmp/tm_clk\n", tm);
    }
    fclose(pFile);
  }
  *bps_val = bps;
  *pps_val = pps;
  *tm_val = tm;
}
#endif /* __KERNEL__ */

/*************************************************
 * freq_to_pll_val
 *
 * Helper function to program pps_pll and core_pll.  Given a desired frequency
 * returns the value to set in the PLL's ctrl0 register.
 *************************************************/
static uint32_t freq_to_pll_val(int freq) {
  /* The ref_clk is 156.25MHz so using a divider of 25 gives a step of 6.25MHz
   * which allows for fine enough control on the frequency. */
  uint32_t ref_clk = 156250000;
  uint32_t ref_div = 25;
  uint32_t step = ref_clk / ref_div;
  uint32_t int_div = freq / step;
  uint32_t ret = (1u << 31) | /* Bypass */
                 (1u << 27) | /* Load */
                 ((int_div - 1) << 16) | ((ref_div - 1) << 8);
  lld_log("Target frequency %d, actual %d, encoded 0x%x",
          freq,
          int_div * step,
          ret);
  return ret;
}

/*************************************************
 * pll_val_to_freq
 *
 * Helper function to read pps_pll and core_pll.  Given the value of the ctrl0
 * register returns the frequency.
 *************************************************/
static int pll_val_to_freq(uint32_t val) {
  uint32_t ref_clk = 156250000;
  uint32_t out_div = val & 0xFF;
  uint32_t ref_div = 1 + ((val >> 8) & 0x7F);
  uint32_t int_div = 1 + ((val >> 16) & 0x3FF);
  uint32_t step = ref_clk / ref_div;

  if (out_div) {
    /* This decode supports an output divider value of 0. */
    return 0;
  }
  return int_div * step;
}

/*************************************************
 * bps_pps_freq_from_sku
 *
 * Helper function to convert the clock speed based on sku to integer values.
 *************************************************/
static bf_status_t bps_pps_freq_from_sku(bf_dev_id_t dev_id,
                                         int *bps_freq,
                                         int *pps_freq) {
  bf_sku_core_clk_freq_t bps_clk_speed, pps_clk_speed;
  lld_err_t err;

  err = lld_sku_get_core_clk_freq(dev_id, &bps_clk_speed, &pps_clk_speed);
  if (err != LLD_OK) return BF_INVALID_ARG;
  switch (pps_clk_speed) {
    case BF_SKU_CORE_CLK_1_0_GHZ:
      *pps_freq = 1000000000;
      break;
    case BF_SKU_CORE_CLK_1_05_GHZ:
      *pps_freq = 1050000000;
      break;
    case BF_SKU_CORE_CLK_1_2625_GHZ:
    default:
      *pps_freq = 1262500000;
      break;
  }
  switch (bps_clk_speed) {
    case BF_SKU_CORE_CLK_1_0_GHZ:
      *bps_freq = 1000000000;
      break;
    case BF_SKU_CORE_CLK_1_05_GHZ:
      *pps_freq = 1050000000;
      break;
    case BF_SKU_CORE_CLK_1_3_GHZ:
    default:
      *bps_freq = 1300000000;
      break;
  }
  return BF_SUCCESS;
}

/*************************************************
 * freq_to_pll_val
 *
 * Helper function to program pps_pll, core_pll, tm_pll.
 * Checks the efuse to get the correct frequency and returns the value to
 *program in the PLL's ctrl0 register.  In SLT mode this will also check for the
 * existence of "clock override" files and use the value from that file instead.
 *************************************************/
static bf_status_t bps_pps_ctrl0_val(bf_dev_id_t dev_id,
                                     uint32_t *bps_val,
                                     uint32_t *pps_val,
                                     uint32_t *tm_val) {
  /* Read the frequency in efuse and return PLL values to program, enable when
     we receive parts with efuse populated
  */
  if (0) {
    bf_status_t rc;
    int bps_freq, pps_freq;

    rc = bps_pps_freq_from_sku(dev_id, &bps_freq, &pps_freq);
    if (rc != BF_SUCCESS) return rc;

    *pps_val = freq_to_pll_val(pps_freq);
    *bps_val = freq_to_pll_val(bps_freq);
  }

#ifndef __KERNEL__
  /* Ovverride the clk values from a file if the file exists */
  uint32_t bps, pps, tm;
  read_core_clk_from_file(&bps, &pps, &tm);
  if (bps) *bps_val = 0x80000000 | bps;
  if (pps) *pps_val = 0x80000000 | pps;
  if (tm) *tm_val = 0x80000000 | tm;
#endif /* __KERNEL__ */

  return BF_SUCCESS;
}

/************************************************************
 * lld_dev_tof3_ts_inc_value_set
 *
 ************************************************************/
static void lld_dev_tof3_ts_inc_value_set(bf_dev_id_t dev_id,
                                          bf_subdev_id_t subdev_id,
                                          int core_freq,
                                          int pps_freq) {
#ifndef __KERNEL__
  if (!core_freq || !pps_freq) {
    lld_log("%s:%d Error! Either core_freq or pps_freq is 0 for device id %d",
            __func__,
            __LINE__,
            dev_id);
    return;
  }
  uint32_t ctrl_val = 0;
  uint32_t addr;
  uint32_t x;
  double clk_ratio;

  /* Program to 2^28 / core-clock in GHz
   * The top nibble is a ns value and the remaining 28 bits are a fractional ns
   * value.
   * At 1.20 GHz program 0x0D555555
   * At 1.25 GHz program 0x0CCCCCCC
   * At 1.30 GHz program 0x0C4EC4EC
   * At 1.35 GHz program 0x0BDA12F6
   */
  x = (1 << 28) / (core_freq / 1000000000.0) + 0.5;
  setp_tof3_mbus_ts_4ns_inc_value_count(&ctrl_val, x);
  addr = offsetof(tof3_reg, device_select.mbc.mbc_mbus.ts_4ns_inc_value);
  lld_subdev_write_register(dev_id, subdev_id, addr, ctrl_val);
  lld_log("Set 4ns pulse config to 0x%x", x);

  /* Program the same thing for the global_ts_inc_value. */
  addr = offsetof(tof3_reg, device_select.mbc.mbc_mbus.global_ts_inc_value);
  lld_subdev_write_register(dev_id, subdev_id, addr, x);
  lld_log("Set global ts inc config to 0x%x", x);

  /* Program a ratio between the core and pps clocks in the TM's PSC timestamp
   * increment register. */
  clk_ratio = core_freq / (double)pps_freq;
  addr =
      offsetof(tof3_reg, device_select.tm_top.tm_psc_top.psc_common.ts_offset);
  x = (1 << 28) / (core_freq / 1000000000.0) / 65536.0 * clk_ratio + 0.5;
  lld_subdev_write_register(dev_id, subdev_id, addr, x);
  lld_log("Set global PSC inc config to 0x%x", x);

  /* Program the global_ts_inc_value_tm_clk. */
  x = 3244;
  addr =
      offsetof(tof3_reg, device_select.mbc.mbc_mbus.global_ts_inc_value_clk_tm);
  lld_subdev_write_register(dev_id, subdev_id, addr, x);
  lld_log("Set global ts inc tm config to 0x%x", x);
#else
  uint32_t addr, x, y;
  if (!core_freq || !pps_freq) {
    return;
  }
  x = (1 << 28) * 1000000000ull / (uint64_t)core_freq;
  y = (uint64_t)(x >> 16) * core_freq / pps_freq;
  addr = offsetof(tof3_reg, device_select.mbc.mbc_mbus.ts_4ns_inc_value);
  lld_subdev_write_register(dev_id, subdev_id, addr, x);
  addr = offsetof(tof3_reg, device_select.mbc.mbc_mbus.global_ts_inc_value);
  lld_subdev_write_register(dev_id, subdev_id, addr, x);
  addr =
      offsetof(tof3_reg, device_select.tm_top.tm_psc_top.psc_common.ts_offset);
  lld_subdev_write_register(dev_id, subdev_id, addr, y);
  x = 3244;
  addr =
      offsetof(tof3_reg, device_select.mbc.mbc_mbus.global_ts_inc_value_clk_tm);
  lld_subdev_write_register(dev_id, subdev_id, addr, x);
#endif
}

/*************************************************
 * lld_dev_tof3_un_reset
 *
 * Bring all blocks out of reset
 *************************************************/
bf_status_t lld_dev_tof3_un_reset(bf_dev_id_t dev_id,
                                  bf_subdev_id_t subdev_id) {
  bf_status_t rc;
  uint32_t rev_id;
  int i = 0, max_polls = 1000;
  int core_clk_freq = 0, pps_clk_freq = 0;

  uint32_t soft_rst =
      tof3_reg_device_select_misc_all_regs_misc_regs_soft_reset_address;
  uint32_t soft_rst_val = 0;
  uint32_t dbg_rst =
      tof3_reg_device_select_misc_all_regs_misc_regs_dbg_rst_address;
  uint32_t dbg_rst_val = 0;
  uint32_t pps_pll =
      tof3_reg_device_select_misc_all_regs_misc_regs_pps_pll_ctrl0_address;
  uint32_t bps_pll =
      tof3_reg_device_select_misc_all_regs_misc_regs_core_pll_ctrl0_address;
  uint32_t mac0_pll =
      tof3_reg_device_select_misc_all_regs_misc_regs_mac_pll_ctrl0_address;
  uint32_t tm_pll =
      tof3_reg_device_select_misc_all_regs_misc_regs_tm_pll_ctrl0_address;
  uint32_t pps_pll_val;
  uint32_t bps_pll_val;
  uint32_t tm_pll_val;
  uint32_t mac0_pll_val = 0x88FA1800;  // 1.568G
  uint32_t dummy;
  uint32_t pvt =
      tof3_reg_device_select_misc_all_regs_misc_regs_remote_pvt_ctrl_address;

  /* On the model and emulator components such as PLLs do not exist so don't
   * expected them to lock. */
  bool errors_expected = false;
  bf_drv_device_type_get(dev_id, &errors_expected);
#ifdef DEVICE_IS_EMULATOR
  errors_expected = true;
#endif
  if (dev_id < 0 || dev_id >= BF_MAX_DEV_COUNT ||
      subdev_id >= BF_MAX_SUBDEV_COUNT) {
    return BF_INVALID_ARG;
  }
  rev_id = lld_subdev_efuse_get_part_revision_number(dev_id, subdev_id);
  if (rev_id == BF_SKU_CHIP_PART_REV_A1) {
    pps_pll_val = 0x88A71800;  // 1.05G (0x889f1800 for 1.0G)
    bps_pll_val = 0x88A71800;  // 1.5G
    tm_pll_val = 0x88af1800;   // 1.1G
  } else {                     // for A0 and B0
    pps_pll_val = 0x88c91800;  // 1.2625G (0x88c71800 for 1.25G)
    bps_pll_val = 0x88cf1800;  // 1.3G
    tm_pll_val = 0x88cf1800;   // 1.3G
  }
  /* Put PPS, BPS, MAC0, and TM PLLs in bypass.  Put the PPS PLL in bypass
   * first and wait a bit.  If the chip was running at capacity this will
   * reduce the load before we assert the resets below. */
  lld_subdev_write_register(dev_id, subdev_id, pps_pll, pps_pll_val);
  lld_subdev_read_register(dev_id, subdev_id, pps_pll, &dummy);
  bf_sys_usleep(10);
  lld_subdev_write_register(dev_id, subdev_id, bps_pll, bps_pll_val);
  lld_subdev_write_register(dev_id, subdev_id, mac0_pll, mac0_pll_val);
  lld_subdev_write_register(dev_id, subdev_id, tm_pll, tm_pll_val);

  /* Dummy read to ensure posted writes to PLLs complete. */
  lld_subdev_read_register(dev_id, subdev_id, tm_pll, &dummy);

  /* Wait for PLLs to be updated. */
  bf_sys_usleep(5);

  /* Put core, MAC, MBus, TV80, tiles, and PLLs are in soft reset. */
  lld_subdev_read_register(dev_id, subdev_id, soft_rst, &soft_rst_val);
  soft_rst_val |= 0x0F3FC000;
  lld_subdev_write_register(dev_id, subdev_id, soft_rst, soft_rst_val);

  /* Program PPS and BPS PLL values based on desired clock frequency. */
  rc = bps_pps_ctrl0_val(dev_id, &bps_pll_val, &pps_pll_val, &tm_pll_val);
  if (rc != BF_SUCCESS) return rc;
  lld_subdev_write_register(dev_id, subdev_id, pps_pll, pps_pll_val);
  lld_subdev_write_register(dev_id, subdev_id, bps_pll, bps_pll_val);
  lld_subdev_write_register(dev_id, subdev_id, tm_pll, tm_pll_val);

  /* Dummy read to ensure posted writes to PLLs complete. */
  lld_subdev_read_register(dev_id, subdev_id, tm_pll, &dummy);

  /* It takes HW a little while after the register write to actually move the
   * data into the PLL, so wait a few micro-seconds to ensure the writes to the
   * PLL ctrl0 registers are fully commited. */
  bf_sys_usleep(5);

  /* Ensure the four PLLs are NOT locked. */
  for (i = 0; i < max_polls; ++i) {
    lld_subdev_read_register(dev_id, subdev_id, dbg_rst, &dbg_rst_val);
    if ((dbg_rst_val & 0x78000000) == 0) {
      lld_log("Dev %d [Unlock] Done after %d polls", dev_id, i);
      break;
    }
    bf_sys_usleep(1000); /* sleep for 1ms */
  }
  if ((dbg_rst_val & 0x78000000) != 0 && !errors_expected) {
    lld_log("Dev %d [Unlock] Failed 0x%x (note: will fail on arch model)",
            dev_id,
            dbg_rst_val);
    return BF_HW_COMM_FAIL;
  }

  /* Un-reset the PLLs. */
  soft_rst_val &= ~0x0003C000;
  lld_subdev_write_register(dev_id, subdev_id, soft_rst, soft_rst_val);

  /* Wait for them to lock.  Since they need to come out of reset to lock there
   * is no need to poll their reset status before polling their lock status. */
  for (i = 0; i < max_polls; ++i) {
    bf_sys_usleep(1000); /* sleep for 1ms */
    lld_subdev_read_register(dev_id, subdev_id, dbg_rst, &dbg_rst_val);
    if ((dbg_rst_val & 0x78000000) == 0x78000000) {
      lld_log("Dev %d [Lock] Done after %d polls", dev_id, i);
      break;
    }
    if (errors_expected) break;
  }
  if ((dbg_rst_val & 0x78000000) != 0x78000000 && !errors_expected) {
    lld_log("Dev %d [Lock] Failed 0x%x (note: will fail on arch model)",
            dev_id,
            dbg_rst_val);
    return BF_HW_COMM_FAIL;
  }

  /* Bring core, MAC, MBus, and tiles out of reset. */
  soft_rst_val &= ~0x0F1C0000;
  lld_subdev_write_register(dev_id, subdev_id, soft_rst, soft_rst_val);

  /* Wait for the core, MAC, and MBus to come out of reset. */
  for (i = 0; i < max_polls; ++i) {
    lld_subdev_read_register(dev_id, subdev_id, dbg_rst, &dbg_rst_val);
    if ((dbg_rst_val & 0x07000000) == 0x00000000) {
      lld_log("Dev %d [Unreset] Done after %d polls", dev_id, i);
      break;
    }
  }
  if ((dbg_rst_val & 0x07000000) != 0x00000000 && !errors_expected) {
    lld_log("Dev %d [Unreset] Failed 0x%x (note: will fail on arch model)",
            dev_id,
            dbg_rst_val);
    return BF_HW_COMM_FAIL;
  }

  /* Remove PLL bypass. */
  pps_pll_val &= ~0x80000000;
  bps_pll_val &= ~0x80000000;
  mac0_pll_val &= ~0x80000000;
  tm_pll_val &= ~0x80000000;
  lld_subdev_write_register(dev_id, subdev_id, pps_pll, pps_pll_val);
  lld_subdev_write_register(dev_id, subdev_id, bps_pll, bps_pll_val);
  lld_subdev_write_register(dev_id, subdev_id, mac0_pll, mac0_pll_val);
  lld_subdev_write_register(dev_id, subdev_id, tm_pll, tm_pll_val);

  /* Set timestamp increment values. */
  core_clk_freq = pll_val_to_freq(bps_pll_val);
  pps_clk_freq = pll_val_to_freq(pps_pll_val);
  lld_dev_tof3_ts_inc_value_set(dev_id, subdev_id, core_clk_freq, pps_clk_freq);

  /* Set PVT into tempature monitoring mode. */
  lld_subdev_write_register(dev_id, subdev_id, pvt, 0x008101E3);

  return BF_SUCCESS;
}

/********************************************************************
 * lld_dev_tof3_set_pps_clock
 *
 * Updates the PPS clock frequency.  This will put the PLL in bypass and switch
 * to the ref_clk, which is slow, so traffic should not be flowing.
 * Note 0 is a magic number and will reset it based on the SKU.
 *******************************************************************/
bf_status_t lld_dev_tof3_set_pps_clock(bf_dev_id_t dev_id,
                                       bf_subdev_id_t subdev_id,
                                       int freq) {
  bf_status_t rc;
  int core_clk_freq = 0, pps_clk_freq = 0;
  int i, bps_sku_freq, pps_sku_freq = 1262500000, max_polls = 1000;
  uint32_t bps_pll =
      tof3_reg_device_select_misc_all_regs_misc_regs_core_pll_ctrl0_address;
  uint32_t pps_pll =
      tof3_reg_device_select_misc_all_regs_misc_regs_pps_pll_ctrl0_address;
  uint32_t pps_pll_val, bps_pll_val;
  uint32_t soft_rst =
      tof3_reg_device_select_misc_all_regs_misc_regs_soft_reset_address;
  uint32_t soft_rst_val = 0;
  uint32_t dbg_rst =
      tof3_reg_device_select_misc_all_regs_misc_regs_dbg_rst_address;
  uint32_t dbg_rst_val = 0;
  uint32_t dummy;
  lld_dev_t *dev_p =
      lld_map_subdev_id_to_dev_p_allow_unassigned(dev_id, subdev_id);
  if (dev_p == NULL) return BF_INVALID_ARG;

  if (!freq) {
    rc = bps_pps_freq_from_sku(dev_id, &bps_sku_freq, &pps_sku_freq);
    if (rc != BF_SUCCESS) return rc;
    freq = pps_sku_freq;
  }

  /* First put the PLL in bypass. */
  lld_subdev_read_register(dev_id, subdev_id, pps_pll, &pps_pll_val);
  pps_pll_val |= 0x80000000;
  lld_subdev_write_register(dev_id, subdev_id, pps_pll, pps_pll_val);

  /* Dummy read to ensure posted writes to PLLs complete. */
  lld_subdev_read_register(dev_id, subdev_id, bps_pll, &bps_pll_val);
  bf_sys_usleep(5);

  /* If someone passed a PLL value rather than a clock frequency try to detect
   * it and use the value directly. */
  if ((uint32_t)freq & 0x80000000) {
    pps_pll_val = freq;
  } else {
    /* Set new value, load, and bypass. */
    pps_pll_val = freq_to_pll_val(freq);
  }

  /* Put the PLL in reset before reprogramming it. */
  lld_subdev_read_register(dev_id, subdev_id, soft_rst, &soft_rst_val);
  soft_rst_val |= 1u << 15;
  lld_subdev_write_register(dev_id, subdev_id, soft_rst, soft_rst_val);

  /* Write the new configuration into the PLL. */
  lld_subdev_write_register(dev_id, subdev_id, pps_pll, pps_pll_val);

  /* Dummy read to ensure posted writes to PLLs complete. */
  lld_subdev_read_register(dev_id, subdev_id, pps_pll, &dummy);

  /* It takes HW a little while after the register write to actually move the
   * data into the PLL, so wait a few micro-seconds to ensure the writes to the
   * PLL ctrl0 registers are fully commited. */
  bf_sys_usleep(5);

  /* Take the PLL out of reset. */
  soft_rst_val &= ~(1u << 15);
  lld_subdev_write_register(dev_id, subdev_id, soft_rst, soft_rst_val);

  /* Wait for it to lock. */
  for (i = 0; i < max_polls; ++i) {
    bf_sys_usleep(1000); /* sleep for 1ms */
    lld_subdev_read_register(dev_id, subdev_id, dbg_rst, &dbg_rst_val);
    if ((dbg_rst_val & 0x10000000) == 0x10000000) {
      break;
    }
  }
  if ((dbg_rst_val & 0x10000000) != 0x10000000) {
    lld_log(
        "Dev %d Subdev %d [Lock] Failed 0x%x (note: will fail on arch model)",
        dev_id,
        subdev_id,
        dbg_rst_val);
    return BF_HW_COMM_FAIL;
  }

  /* Remove bypass. */
  pps_pll_val &= ~0x80000000;
  lld_subdev_write_register(dev_id, subdev_id, pps_pll, pps_pll_val);

  /* Set timestamp increment values. */
  core_clk_freq = pll_val_to_freq(bps_pll_val);
  pps_clk_freq = pll_val_to_freq(pps_pll_val);
  lld_dev_tof3_ts_inc_value_set(dev_id, subdev_id, core_clk_freq, pps_clk_freq);

  return BF_SUCCESS;
}

/********************************************************************
 * lld_dev_tof3_set_core_clock
 *
 * Updates the core (BPS) clock frequency. This will put the PLL in bypass and
 * switch to the ref_clk, which is slow, so traffic should not be flowing.
 * Note 0 is a magic number and will reset it based on the SKU.
 *******************************************************************/
bf_status_t lld_dev_tof3_set_core_clock(bf_dev_id_t dev_id,
                                        bf_subdev_id_t subdev_id,
                                        int freq) {
  bf_status_t rc;
  int core_clk_freq = 0, pps_clk_freq = 0;
  int i, bps_sku_freq = 1300000000, pps_sku_freq, max_polls = 1000;
  uint32_t pps_pll =
      tof3_reg_device_select_misc_all_regs_misc_regs_pps_pll_ctrl0_address;
  uint32_t bps_pll =
      tof3_reg_device_select_misc_all_regs_misc_regs_core_pll_ctrl0_address;
  uint32_t pps_pll_val, bps_pll_val;
  uint32_t soft_rst =
      tof3_reg_device_select_misc_all_regs_misc_regs_soft_reset_address;
  uint32_t soft_rst_val = 0;
  uint32_t dbg_rst =
      tof3_reg_device_select_misc_all_regs_misc_regs_dbg_rst_address;
  uint32_t dbg_rst_val = 0;
  lld_dev_t *dev_p =
      lld_map_subdev_id_to_dev_p_allow_unassigned(dev_id, subdev_id);
  if (dev_p == NULL) return BF_INVALID_ARG;

  if (!freq) {
    rc = bps_pps_freq_from_sku(dev_id, &bps_sku_freq, &pps_sku_freq);
    if (rc != BF_SUCCESS) return rc;
    freq = bps_sku_freq;
  }

  /* If someone passed a PLL value rather than a clock frequency try to detect
   * it and use the value directly. */
  if ((uint32_t)freq & 0x80000000) {
    bps_pll_val = freq;
  } else {
    /* Set new value, load, and bypass. */
    bps_pll_val = freq_to_pll_val(freq);
  }

  /* Put the PLL in reset before reprogramming it. */
  lld_subdev_read_register(dev_id, subdev_id, soft_rst, &soft_rst_val);
  soft_rst_val |= 1u << 14;
  lld_subdev_write_register(dev_id, subdev_id, soft_rst, soft_rst_val);

  /* Write the new configuration into the PLL. */
  lld_subdev_write_register(dev_id, subdev_id, bps_pll, bps_pll_val);

  /* Read to ensure posted writes to PLLs complete. */
  lld_subdev_read_register(dev_id, subdev_id, pps_pll, &pps_pll_val);

  /* It takes HW a little while after the register write to actually move the
   * data into the PLL, so wait a few micro-seconds to ensure the writes to the
   * PLL ctrl0 registers are fully commited. */
  bf_sys_usleep(5);

  /* Take the PLL out of reset. */
  soft_rst_val &= ~(1u << 14);
  lld_subdev_write_register(dev_id, subdev_id, soft_rst, soft_rst_val);

  /* Wait for it to lock. */
  for (i = 0; i < max_polls; ++i) {
    bf_sys_usleep(1000); /* sleep for 1ms */
    lld_subdev_read_register(dev_id, subdev_id, dbg_rst, &dbg_rst_val);
    if ((dbg_rst_val & 0x08000000) == 0x08000000) {
      break;
    }
  }
  if ((dbg_rst_val & 0x08000000) != 0x08000000) {
    lld_log(
        "Dev %d Subdev %d [Lock] Failed 0x%x (note: will fail on arch model)",
        dev_id,
        subdev_id,
        dbg_rst_val);
    return BF_HW_COMM_FAIL;
  }

  /* Remove bypass. */
  bps_pll_val &= ~0x80000000;
  lld_subdev_write_register(dev_id, subdev_id, bps_pll, bps_pll_val);

  /* Set timestamp increment values. */
  core_clk_freq = pll_val_to_freq(bps_pll_val);
  pps_clk_freq = pll_val_to_freq(pps_pll_val);
  lld_dev_tof3_ts_inc_value_set(dev_id, subdev_id, core_clk_freq, pps_clk_freq);

  return BF_SUCCESS;
}

/********************************************************************
 * lld_dev_tof3_set_tm_clock
 *
 * Updates the TM clock frequency.  This will put the PLL in bypass and switch
 * to the ref_clk, which is slow, so traffic should not be flowing.
 * Note 0 is a magic number and will reset it based on the SKU.
 *******************************************************************/
bf_status_t lld_dev_tof3_set_tm_clock(bf_dev_id_t dev_id,
                                      bf_subdev_id_t subdev_id,
                                      int freq) {
  int i, max_polls = 1000;
  uint32_t tm_pll =
      tof3_reg_device_select_misc_all_regs_misc_regs_tm_pll_ctrl0_address;
  uint32_t tm_pll_val;
  uint32_t soft_rst =
      tof3_reg_device_select_misc_all_regs_misc_regs_soft_reset_address;
  uint32_t soft_rst_val = 0;
  uint32_t dbg_rst =
      tof3_reg_device_select_misc_all_regs_misc_regs_dbg_rst_address;
  uint32_t dbg_rst_val = 0;
  uint32_t dummy;
  lld_dev_t *dev_p =
      lld_map_subdev_id_to_dev_p_allow_unassigned(dev_id, subdev_id);
  if (dev_p == NULL) return BF_INVALID_ARG;

  if (!freq) {
    return BF_INVALID_ARG;
  }

  /* First put the PLL in bypass. */
  lld_subdev_read_register(dev_id, subdev_id, tm_pll, &tm_pll_val);
  tm_pll_val |= 0x80000000;
  lld_subdev_write_register(dev_id, subdev_id, tm_pll, tm_pll_val);

  /* Dummy read to ensure posted writes to PLLs complete. */
  lld_subdev_read_register(dev_id, subdev_id, tm_pll, &dummy);
  bf_sys_usleep(5);

  /* If someone passed a PLL value rather than a clock frequency try to detect
   * it and use the value directly. */
  if ((uint32_t)freq & 0x80000000) {
    tm_pll_val = freq;
  } else {
    /* Set new value, load, and bypass. */
    tm_pll_val = freq_to_pll_val(freq);
  }

  /* Put the PLL in reset before reprogramming it. */
  lld_subdev_read_register(dev_id, subdev_id, soft_rst, &soft_rst_val);
  soft_rst_val |= 1u << 17;
  lld_subdev_write_register(dev_id, subdev_id, soft_rst, soft_rst_val);

  /* Write the new configuration into the PLL. */
  lld_subdev_write_register(dev_id, subdev_id, tm_pll, tm_pll_val);

  /* Dummy read to ensure posted writes to PLLs complete. */
  lld_subdev_read_register(dev_id, subdev_id, tm_pll, &dummy);

  /* It takes HW a little while after the register write to actually move the
   * data into the PLL, so wait a few micro-seconds to ensure the writes to the
   * PLL ctrl0 registers are fully commited. */
  bf_sys_usleep(5);

  /* Take the PLL out of reset. */
  soft_rst_val &= ~(1u << 17);
  lld_subdev_write_register(dev_id, subdev_id, soft_rst, soft_rst_val);

  /* Wait for it to lock. */
  for (i = 0; i < max_polls; ++i) {
    bf_sys_usleep(1000); /* sleep for 1ms */
    lld_subdev_read_register(dev_id, subdev_id, dbg_rst, &dbg_rst_val);
    if ((dbg_rst_val & 0x40000000) == 0x40000000) {
      break;
    }
  }
  if ((dbg_rst_val & 0x40000000) != 0x40000000) {
    lld_log(
        "Dev %d Subdev %d [Lock] Failed 0x%x (note: will fail on arch model)",
        dev_id,
        subdev_id,
        dbg_rst_val);
    return BF_HW_COMM_FAIL;
  }

  /* Remove bypass. */
  tm_pll_val &= ~0x80000000;
  lld_subdev_write_register(dev_id, subdev_id, tm_pll, tm_pll_val);
  return BF_SUCCESS;
}

/********************************************************************
 * lld_dev_tof3_get_pps_clock
 *
 * Gets the PPS clock frequency and PLL value.
 *******************************************************************/
bf_status_t lld_dev_tof3_get_pps_clock(bf_dev_id_t dev_id,
                                       bf_subdev_id_t subdev_id,
                                       int *freq,
                                       uint32_t *pll_val) {
  uint32_t pps_pll =
      tof3_reg_device_select_misc_all_regs_misc_regs_pps_pll_ctrl0_address;
  lld_dev_t *dev_p =
      lld_map_subdev_id_to_dev_p_allow_unassigned(dev_id, subdev_id);
  if (dev_p == NULL) return BF_INVALID_ARG;

  lld_subdev_read_register(dev_id, subdev_id, pps_pll, pll_val);
  *freq = pll_val_to_freq(*pll_val);
  return BF_SUCCESS;
}

/********************************************************************
 * lld_dev_tof3_get_core_clock
 *
 * Gets the core (BPS) clock frequency and PLL value.
 *******************************************************************/
bf_status_t lld_dev_tof3_get_core_clock(bf_dev_id_t dev_id,
                                        bf_subdev_id_t subdev_id,
                                        int *freq,
                                        uint32_t *pll_val) {
  uint32_t bps_pll =
      tof3_reg_device_select_misc_all_regs_misc_regs_core_pll_ctrl0_address;
  lld_dev_t *dev_p =
      lld_map_subdev_id_to_dev_p_allow_unassigned(dev_id, subdev_id);
  if (dev_p == NULL) return BF_INVALID_ARG;

  lld_subdev_read_register(dev_id, subdev_id, bps_pll, pll_val);
  *freq = pll_val_to_freq(*pll_val);
  return BF_SUCCESS;
}

/********************************************************************
 * lld_dev_tof3_get_tm_clock
 *
 * Gets the TM clock frequency and PLL value.
 *******************************************************************/
bf_status_t lld_dev_tof3_get_tm_clock(bf_dev_id_t dev_id,
                                      bf_subdev_id_t subdev_id,
                                      int *freq,
                                      uint32_t *pll_val) {
  uint32_t tm_pll =
      tof3_reg_device_select_misc_all_regs_misc_regs_tm_pll_ctrl0_address;
  lld_dev_t *dev_p =
      lld_map_subdev_id_to_dev_p_allow_unassigned(dev_id, subdev_id);
  if (dev_p == NULL) return BF_INVALID_ARG;

  lld_subdev_read_register(dev_id, subdev_id, tm_pll, pll_val);
  *freq = pll_val_to_freq(*pll_val);
  return BF_SUCCESS;
}

/********************************************************************
 * lld_dev_tof3_reset_core
 *
 * Reset core blocks, leaving NIF untouched
 *******************************************************************/
bf_status_t lld_dev_tof3_reset_core(bf_dev_id_t dev_id,
                                    bf_subdev_id_t subdev_id) {
  int core_clk_freq = 0, pps_clk_freq = 0;
  const uint32_t core_reset = 1u << 18;
  const uint32_t core_reset_status = 1u << 24;
  lld_dev_t *dev_p =
      lld_map_subdev_id_to_dev_p_allow_unassigned(dev_id, subdev_id);
  uint32_t val = 0;
  int polls = 0, max_polls = 10, reset_done = 0;
  uint32_t pps_pll =
      tof3_reg_device_select_misc_all_regs_misc_regs_pps_pll_ctrl0_address;
  uint32_t bps_pll =
      tof3_reg_device_select_misc_all_regs_misc_regs_core_pll_ctrl0_address;
  uint32_t pps_pll_val;
  uint32_t bps_pll_val;
  uint32_t dummy;
  uint32_t pvt =
      tof3_reg_device_select_misc_all_regs_misc_regs_remote_pvt_ctrl_address;
  bool errors_expected = false;
  if (dev_p == NULL) return BF_INVALID_ARG;

  /* The model may not report reset status through the dbg_rst register. */
  bf_drv_device_type_get(dev_id, &errors_expected);

  /* Read modify write the core and PPS PPLs to put them in bypass. */
  lld_subdev_read_register(dev_id, subdev_id, pps_pll, &pps_pll_val);
  lld_subdev_read_register(dev_id, subdev_id, bps_pll, &bps_pll_val);
  lld_subdev_write_register(
      dev_id, subdev_id, pps_pll, pps_pll_val | 0x80000000);
  lld_subdev_write_register(
      dev_id, subdev_id, bps_pll, bps_pll_val | 0x80000000);
  /* Dummy read to ensure posted writes to PLLs complete. */
  lld_subdev_read_register(dev_id, subdev_id, bps_pll, &dummy);
  bf_sys_usleep(5);

  /* Read register first to preserve other bits. */
  lld_subdev_read_register(
      dev_id,
      subdev_id,
      tof3_reg_device_select_misc_all_regs_misc_regs_soft_reset_address,
      &val);
  val |= core_reset;
  /* Write the register to assert the core reset bit. */
  lld_subdev_write_register(
      dev_id,
      subdev_id,
      tof3_reg_device_select_misc_all_regs_misc_regs_soft_reset_address,
      val);
  /* Write the register again to deassert the core reset bit. */
  val &= ~core_reset;
  lld_subdev_write_register(
      dev_id,
      subdev_id,
      tof3_reg_device_select_misc_all_regs_misc_regs_soft_reset_address,
      val);

  /* Poll the reset status to wait for core reset to complete. */
  while (!errors_expected && !reset_done) {
    lld_subdev_read_register(
        dev_id,
        subdev_id,
        tof3_reg_device_select_misc_all_regs_misc_regs_dbg_rst_address,
        &val);
    reset_done = !(val & core_reset_status);
    polls++;
    if (!reset_done) {
      if (polls >= max_polls) {
        lld_log(
            "Dev %d subdev %d [Core Unreset] Failed (note: will fail on arch "
            "model)",
            dev_id,
            subdev_id);
        return BF_INVALID_ARG;
      }
      lld_log("Dev %d subdev %d [Core Unreset] In progress: %08x",
              dev_id,
              subdev_id,
              val);
    }
  }
  lld_log("Dev %d subdev %d [Core Unreset] Done after %d polls",
          dev_id,
          subdev_id,
          polls);

  /* Remove the PLL bypass. */
  lld_subdev_write_register(dev_id, subdev_id, pps_pll, pps_pll_val);
  lld_subdev_write_register(dev_id, subdev_id, bps_pll, bps_pll_val);

  /* Set timestamp increment values. */
  core_clk_freq = pll_val_to_freq(bps_pll_val);
  pps_clk_freq = pll_val_to_freq(pps_pll_val);
  lld_dev_tof3_ts_inc_value_set(dev_id, subdev_id, core_clk_freq, pps_clk_freq);

  /* Set PVT into tempature monitoring mode. */
  lld_subdev_write_register(dev_id, subdev_id, pvt, 0x008101E3);

  return BF_SUCCESS;
}

bf_status_t lld_dev_tof3_tlp_poison_set(bf_dev_id_t dev_id,
                                        bf_subdev_id_t subdev_id,
                                        bool en) {
  uint32_t offset, cpu_glb_ctrl;

  offset = offsetof(tof3_reg, device_select.pcie_bar01_regs.cpu_glb_ctrl);
  lld_subdev_read_register(dev_id, subdev_id, offset, &cpu_glb_ctrl);
  setp_tof3_cpu_glb_ctrl_ena_pois(&cpu_glb_ctrl, (en ? 1 : 0));
  setp_tof3_cpu_glb_ctrl_data_pois(&cpu_glb_ctrl, 1);
  lld_subdev_write_register(dev_id, subdev_id, offset, cpu_glb_ctrl);
  return BF_SUCCESS;
}
