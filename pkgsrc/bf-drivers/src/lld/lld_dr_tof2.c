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
#include <string.h>  //for memset
#else
#include <linux/string.h>  //for memset
#define bf_sys_assert(x) \
  do {                   \
  } while (0)
#endif

#include <bf_types/bf_types.h>
#include <dvm/bf_dma_types.h>
#include <tof2_regs/tof2_reg_drv.h>
#include <lld/bf_dma_if.h>
#include <lld/lld_reg_if.h>
#include <lld/lld_dr_regs.h>
#include <lld/lld_dr_regs_tof2.h>
#include <lld/lld_err.h>
#include <lld/lld_sku.h>
#include "lld_dr.h"
#include "lld.h"
#include "lld_log.h"
#include <lld/lld_dr_if.h>
#include "lld_dr_tof2.h"

// fwd refs
void lld_dr_tof2_enable_set(bf_dev_id_t dev_id, bf_dma_dr_id_t dr_id, bool en);
void lld_dr_tof2_data_timeout_set(bf_dev_id_t dev_id,
                                  bf_dma_dr_id_t dr_id,
                                  uint32_t timeout);
bf_status_t lld_dr_tof2_pushed_ptr_mode_set(bf_dev_id_t dev_id,
                                            bf_dma_dr_id_t dr_id,
                                            bool en);

/************************************************************
 * lld_dr_tof2_dma_enable_set
 *
 ************************************************************/
void lld_dr_tof2_dma_enable_set(bf_dev_id_t dev_id, bool en_dma) {
  uint32_t glb_ctrl;
  uint32_t reg = tof2_reg_device_select_pcie_bar01_regs_dma_glb_ctrl_address;

  lld_read_register(dev_id, reg, &glb_ctrl);
  setp_tof2_dma_glb_ctrl_dma_en(&glb_ctrl, (en_dma ? 1 : 0));
  lld_write_register(dev_id, reg, glb_ctrl);
}

/************************************************************
 * lld_dr_tof2_pbus_arb_ctrl_set
 *
 ************************************************************/
void lld_dr_tof2_pbus_arb_ctrl_set(bf_dev_id_t dev_id,
                                   pbus_arb_ctrl_t *pbus_arb_ctrl) {
  lld_err_t err;
  uint32_t num_pipes;
  bf_dev_pipe_t log_pipe;
  bf_dev_pipe_t phy_pipe;
  uint32_t reg;

  err = lld_sku_get_num_active_pipes(dev_id, &num_pipes);
  bf_sys_assert(err == LLD_OK);
  reg = tof2_reg_device_select_pbc_pbc_pbus_arb_ctrl0_address;
  lld_write_register(dev_id, reg, pbus_arb_ctrl->ctrl_0);

  // init the pbus arb_ctrl settings that affect idle/stats DMAs
  // Note: Cannot set non-0 values for pipes that are not
  //       present.
  reg = tof2_reg_device_select_pbc_pbc_pbus_arb_ctrl1_address;
  for (log_pipe = 0; log_pipe < num_pipes; log_pipe++) {
    err = lld_sku_map_pipe_id_to_phy_pipe_id(dev_id, log_pipe, &phy_pipe);
    bf_sys_assert(err == LLD_OK);
    lld_write_register(
        dev_id, reg + (phy_pipe * 4), pbus_arb_ctrl->ctrl_1[log_pipe]);
  }
}

/************************************************************
 * lld_dr_tof2_cbus_arb_ctrl_set
 *
 ************************************************************/
void lld_dr_tof2_cbus_arb_ctrl_set(bf_dev_id_t dev_id,
                                   uint32_t cbus_arb_ctrl_val) {
  // enable cbus to post interrupts
  uint32_t reg = tof2_reg_device_select_cbc_cbc_cbus_arb_ctrl_address;

  lld_write_register(dev_id, reg, cbus_arb_ctrl_val);
}

/************************************************************
 * lld_dr_tof2_enable_set
 *
 ************************************************************/
void lld_dr_tof2_enable_set(bf_dev_id_t dev_id, bf_dma_dr_id_t dr_id, bool en) {
  uint32_t dru_base = lld_dr_base_get(dev_id, dr_id);
  uint32_t ctrl_wd = 0;
  uint32_t set_val = (en ? 1 : 0);

  /* get current contents */
  lld_read_register(
      dev_id, dru_base + offsetof(tof2_Dru_rspec, ctrl), &ctrl_wd);

  setp_tof2_dr_ctrl_en(&ctrl_wd, set_val);

  /* and write it out */
  lld_write_register(
      dev_id, dru_base + offsetof(tof2_Dru_rspec, ctrl), ctrl_wd);
}

/************************************************************
 * lld_dr_tof2_write_time_mode_set
 *
 ************************************************************/
bf_status_t lld_dr_tof2_write_time_mode_set(bf_dev_id_t dev_id,
                                            bf_dma_dr_id_t dr_id,
                                            bool en) {
  uint32_t dru_base = lld_dr_base_get(dev_id, dr_id);
  uint32_t ctrl_wd = 0, enabled;
  uint32_t set_val = (en ? 1 : 0);

  /* get current contents */
  lld_read_register(
      dev_id, dru_base + offsetof(tof2_Dru_rspec, ctrl), &ctrl_wd);

  /* Can only be set before DR is enabled */
  enabled = getp_tof2_dr_ctrl_en(&ctrl_wd);
  if (enabled) {
    return BF_INVALID_ARG;
  }

  setp_tof2_dr_ctrl_write_time_mode(&ctrl_wd, set_val);

  /* and write it out */
  lld_write_register(
      dev_id, dru_base + offsetof(tof2_Dru_rspec, ctrl), ctrl_wd);
  return BF_SUCCESS;
}

/************************************************************
 * lld_dr_tof2_pushed_ptr_mode_set
 *
 ************************************************************/
bf_status_t lld_dr_tof2_pushed_ptr_mode_set(bf_dev_id_t dev_id,
                                            bf_dma_dr_id_t dr_id,
                                            bool en) {
  uint32_t dru_base = lld_dr_base_get(dev_id, dr_id);
  uint32_t ctrl_wd = 0, enabled;
  uint32_t set_val = (en ? 1 : 0);
  lld_dr_basic_cfg_t *cfg = lld_dr_basic_cfg_get(dev_id, 0, dr_id);
  if (!cfg) {
    return BF_INVALID_ARG;
  }

  /* get current contents */
  lld_read_register(
      dev_id, dru_base + offsetof(tof2_Dru_rspec, ctrl), &ctrl_wd);

  /* Can only be set before DR is enabled */
  enabled = getp_tof2_dr_ctrl_en(&ctrl_wd);
  if (enabled) {
    return BF_INVALID_ARG;
  }
  if ((cfg->type == TX) || (cfg->type == FM)) {
    setp_tof2_dr_ctrl_head_ptr_mode(&ctrl_wd, set_val);
  } else {
    setp_tof2_dr_ctrl_tail_ptr_mode(&ctrl_wd, set_val);
  }
  /* and write it out */
  lld_write_register(
      dev_id, dru_base + offsetof(tof2_Dru_rspec, ctrl), ctrl_wd);
  return BF_SUCCESS;
}

/************************************************************
 * lld_dr_tof2_ring_timeout_set
 *
 ************************************************************/
void lld_dr_tof2_ring_timeout_set(bf_dev_id_t dev_id,
                                  bf_dma_dr_id_t dr_id,
                                  uint16_t timeout) {
  uint32_t dru_base = lld_dr_base_get(dev_id, dr_id);
  uint32_t tout = (uint32_t)timeout;

  lld_write_register(
      dev_id, dru_base + offsetof(tof2_Dru_rspec, ring_timeout), tout);
}

/************************************************************
 * lld_dr_tof2_data_timeout_set
 *
 ************************************************************/
void lld_dr_tof2_data_timeout_set(bf_dev_id_t dev_id,
                                  bf_dma_dr_id_t dr_id,
                                  uint32_t timeout) {
  uint32_t dru_base = lld_dr_base_get(dev_id, dr_id);

  lld_write_register(
      dev_id, dru_base + offsetof(tof2_Dru_rspec, data_timeout), timeout);
}

/************************************************************
 * lld_dr_tof2_data_timeout_get
 *
 ************************************************************/
bf_status_t lld_dr_tof2_data_timeout_get(bf_dev_id_t dev_id,
                                         bf_dma_dr_id_t dr_id,
                                         uint32_t *timeout) {
  uint32_t dru_base = lld_dr_base_get(dev_id, dr_id);

  if (lld_read_register(dev_id,
                        dru_base + offsetof(tof2_Dru_rspec, data_timeout),
                        timeout) != 0) {
    return BF_HW_COMM_FAIL;
  }
  return BF_SUCCESS;
}

/************************************************************
 * lld_dr_tof2_flush_all
 *
 ************************************************************/
void lld_dr_tof2_flush_all(bf_dev_id_t dev_id) {
  uint32_t addr, ctrl;

  addr = offsetof(tof2_reg, device_select.mbc.mbc_mbus.flush);
  lld_write_register(dev_id, addr, 1);

  addr = offsetof(tof2_reg, device_select.cbc.cbc_cbus.flush);
  lld_write_register(dev_id, addr, 1);

  /* De-assert ctrl_diag_test_sel
   * De-assert ctrl_diag_cap_en
   * De-assert diag_trigger_en
   * Set the ctrl_flush bit
   */
  addr = offsetof(tof2_reg, device_select.pbc.pbc_pbus.ctrl);
  lld_read_register(dev_id, addr, &ctrl);
  setp_tof2_pbus_ctrl_diag_test_sel(&ctrl, 0);
  setp_tof2_pbus_ctrl_diag_cap_en(&ctrl, 0);
  setp_tof2_pbus_ctrl_diag_trig_en(&ctrl, 0);
  lld_write_register(dev_id, addr, ctrl);

  addr = offsetof(tof2_reg, device_select.pbc.pbc_pbus.flush);
  lld_write_register(dev_id, addr, 1);

  /* Set pfc_fm and pfc_rx to 0s (txpfc will be de-asserted, xon)
   * De-aassert rx_en (txfcu will drive out reset)
   * Set the ctrl_flush bit
   */
  addr = offsetof(tof2_reg, device_select.tbc.tbc_tbus.ctrl);
  lld_read_register(dev_id, addr, &ctrl);
  setp_tof2_tbus_ctrl_pfc_fm(&ctrl, 0);
  setp_tof2_tbus_ctrl_pfc_rx(&ctrl, 0);
  lld_write_register(dev_id, addr, ctrl);

  setp_tof2_tbus_ctrl_rx_en(&ctrl, 0);
  lld_write_register(dev_id, addr, ctrl);

  addr = offsetof(tof2_reg, device_select.tbc.tbc_tbus.flush);
  lld_write_register(dev_id, addr, 1);
}

/************************************************************
 * lld_dr_tof2_wait_for_flush_done
 *
 ************************************************************/
uint32_t lld_dr_tof2_wait_for_flush_done(bf_dev_id_t dev_id,
                                         uint32_t max_tries) {
  uint32_t addr, flush_asserted, i;
  uint32_t flush_mbus = 0, flush_cbus = 0, flush_pbus = 0, flush_tbus = 0;
  bool is_model = false;

  /* No need to poll for flush-done when running against the model. */
  bf_drv_device_type_get(dev_id, &is_model);
  if (is_model) {
    return max_tries;
  }

  for (i = 0; i < max_tries; i++) {
    flush_asserted = 0;
    addr = offsetof(tof2_reg, device_select.mbc.mbc_mbus.flush);
    lld_read_register(dev_id, addr, &flush_mbus);
    flush_asserted |= getp_tof2_mbus_flush_flush(&flush_mbus);

    addr = offsetof(tof2_reg, device_select.cbc.cbc_cbus.flush);
    lld_read_register(dev_id, addr, &flush_cbus);
    flush_asserted |= getp_tof2_mbus_flush_flush(&flush_cbus);

    addr = offsetof(tof2_reg, device_select.pbc.pbc_pbus.flush);
    lld_read_register(dev_id, addr, &flush_pbus);
    flush_asserted |= getp_tof2_mbus_flush_flush(&flush_pbus);

    addr = offsetof(tof2_reg, device_select.tbc.tbc_tbus.flush);
    lld_read_register(dev_id, addr, &flush_tbus);
    flush_asserted |= getp_tof2_mbus_flush_flush(&flush_tbus);

    if (!flush_asserted) return max_tries - i;
  }
  lld_log("DR flush still asserted: dev %d mbus %x cbus %x pbus %x tbus %x",
          dev_id,
          flush_mbus,
          flush_cbus,
          flush_pbus,
          flush_tbus);
  return 0;
}

/************************************************************
 * lld_dr_tof2_clear_link_down
 *
 ************************************************************/
void lld_dr_tof2_clear_link_down(bf_dev_id_t dev_id) {
  uint32_t addr;
  addr = offsetof(tof2_reg, device_select.mbc.mbc_mbus.link_down);
  lld_write_register(dev_id, addr, 1);

  addr = offsetof(tof2_reg, device_select.cbc.cbc_cbus.link_down);
  lld_write_register(dev_id, addr, 1);

  addr = offsetof(tof2_reg, device_select.pbc.pbc_pbus.link_down);
  lld_write_register(dev_id, addr, 1);

  addr = offsetof(tof2_reg, device_select.tbc.tbc_tbus.link_down);
  lld_write_register(dev_id, addr, 1);
}
