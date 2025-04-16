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

#ifndef _REGISTER_MAP_
#define _REGISTER_MAP_
#include "reg.h"

namespace MODEL_CHIP_TEST_NAMESPACE {

struct Pcie_bar01_regs_pcie_intr_map: public RegisterMapper {
  static constexpr PTR_Pcie_bar01_regs_pcie_intr Pcie_bar01_regs_pcie_intr_base=0;
  Pcie_bar01_regs_pcie_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_pcie_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_pcie_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_pcie_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_pcie_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_pcie_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pcie_bar01_regs_map: public RegisterMapper {
  static constexpr PTR_Pcie_bar01_regs Pcie_bar01_regs_base=0;
  Pcie_bar01_regs_map() : RegisterMapper( {
    { "scratch_reg", { const_cast<uint32_t(*)[0x4]>(&Pcie_bar01_regs_base->scratch_reg), 0, {0x4}, sizeof(uint32_t) } },
    { "freerun_cnt", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->freerun_cnt), 0, {}, sizeof(uint32_t) } },
    { "dma_glb_ctrl", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->dma_glb_ctrl), 0, {}, sizeof(uint32_t) } },
    { "wrr_table0", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->wrr_table0), 0, {}, sizeof(uint32_t) } },
    { "wrr_table1", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->wrr_table1), 0, {}, sizeof(uint32_t) } },
    { "wrr_table2", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->wrr_table2), 0, {}, sizeof(uint32_t) } },
    { "wrr_table3", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->wrr_table3), 0, {}, sizeof(uint32_t) } },
    { "dmard_thruput_ctrl", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->dmard_thruput_ctrl), 0, {}, sizeof(uint32_t) } },
    { "int_timeout_ctrl", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->int_timeout_ctrl), 0, {}, sizeof(uint32_t) } },
    { "cpu_glb_ctrl", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->cpu_glb_ctrl), 0, {}, sizeof(uint32_t) } },
    { "cpu_ind_addr_low", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->cpu_ind_addr_low), 0, {}, sizeof(uint32_t) } },
    { "cpu_ind_addr_high", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->cpu_ind_addr_high), 0, {}, sizeof(uint32_t) } },
    { "cpu_ind_data00", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->cpu_ind_data00), 0, {}, sizeof(uint32_t) } },
    { "cpu_ind_data01", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->cpu_ind_data01), 0, {}, sizeof(uint32_t) } },
    { "cpu_ind_data10", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->cpu_ind_data10), 0, {}, sizeof(uint32_t) } },
    { "cpu_ind_data11", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->cpu_ind_data11), 0, {}, sizeof(uint32_t) } },
    { "cpu_ind_rerr", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->cpu_ind_rerr), 0, {}, sizeof(uint32_t) } },
    { "dma_tag_pndg", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->dma_tag_pndg), 0, {}, sizeof(uint32_t) } },
    { "glb_shadow_int", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->glb_shadow_int), 0, {}, sizeof(uint32_t) } },
    { "shadow_int", { const_cast<uint32_t(*)[0x10]>(&Pcie_bar01_regs_base->shadow_int), 0, {0x10}, sizeof(uint32_t) } },
    { "shadow_msk", { const_cast<uint32_t(*)[0x10]>(&Pcie_bar01_regs_base->shadow_msk), 0, {0x10}, sizeof(uint32_t) } },
    { "window0_base_param", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->window0_base_param), 0, {}, sizeof(uint32_t) } },
    { "window0_base_high", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->window0_base_high), 0, {}, sizeof(uint32_t) } },
    { "window0_limit_low", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->window0_limit_low), 0, {}, sizeof(uint32_t) } },
    { "window0_limit_high", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->window0_limit_high), 0, {}, sizeof(uint32_t) } },
    { "window1_base_param", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->window1_base_param), 0, {}, sizeof(uint32_t) } },
    { "window1_base_high", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->window1_base_high), 0, {}, sizeof(uint32_t) } },
    { "window1_limit_low", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->window1_limit_low), 0, {}, sizeof(uint32_t) } },
    { "window1_limit_high", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->window1_limit_high), 0, {}, sizeof(uint32_t) } },
    { "default_pciehdr_param", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->default_pciehdr_param), 0, {}, sizeof(uint32_t) } },
    { "pcie_intr", { &Pcie_bar01_regs_base->pcie_intr, new Pcie_bar01_regs_pcie_intr_map, {}, sizeof(Pcie_bar01_regs_pcie_intr) } },
    { "sram_ecc", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->sram_ecc), 0, {}, sizeof(uint32_t) } },
    { "rxbuf_sbe_err_log", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->rxbuf_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "rxbuf_mbe_err_log", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->rxbuf_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "rxcpl_sbe_err_log", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->rxcpl_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "rxcpl_mbe_err_log", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->rxcpl_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "txbuf_sbe_err_log", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->txbuf_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "txbuf_mbe_err_log", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->txbuf_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "msix_sbe_err_log", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->msix_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "msix_mbe_err_log", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->msix_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "pcie_dev_info", { const_cast<uint32_t(*)[0x8]>(&Pcie_bar01_regs_base->pcie_dev_info), 0, {0x8}, sizeof(uint32_t) } },
    { "pcie_bus_dev", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->pcie_bus_dev), 0, {}, sizeof(uint32_t) } },
    { "pcie_dma_temp_stall", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->pcie_dma_temp_stall), 0, {}, sizeof(uint32_t) } },
    { "pcie_mst_cred", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->pcie_mst_cred), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "pcie_bw_change", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->pcie_bw_change), 0, {}, sizeof(uint32_t) } },
    { "tl_tx_proterr", { const_cast<uint32_t(*)>(&Pcie_bar01_regs_base->tl_tx_proterr), 0, {}, sizeof(uint32_t) } },
    { "msix_map", { const_cast<uint32_t(*)[0x80]>(&Pcie_bar01_regs_base->msix_map), 0, {0x80}, sizeof(uint32_t) } }
    } )
  {}
};

struct Misc_regs_misc_intr_map: public RegisterMapper {
  static constexpr PTR_Misc_regs_misc_intr Misc_regs_misc_intr_base=0;
  Misc_regs_misc_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Misc_regs_misc_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Misc_regs_misc_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Misc_regs_misc_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Misc_regs_misc_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Misc_regs_misc_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Misc_regs_tv80_intr_map: public RegisterMapper {
  static constexpr PTR_Misc_regs_tv80_intr Misc_regs_tv80_intr_base=0;
  Misc_regs_tv80_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Misc_regs_tv80_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Misc_regs_tv80_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Misc_regs_tv80_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Misc_regs_tv80_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Misc_regs_tv80_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Misc_regs_map: public RegisterMapper {
  static constexpr PTR_Misc_regs Misc_regs_base=0;
  Misc_regs_map() : RegisterMapper( {
    { "soft_reset", { const_cast<uint32_t(*)>(&Misc_regs_base->soft_reset), 0, {}, sizeof(uint32_t) } },
    { "reset_option", { const_cast<uint32_t(*)>(&Misc_regs_base->reset_option), 0, {}, sizeof(uint32_t) } },
    { "pciectl_reset_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->pciectl_reset_ctrl), 0, {}, sizeof(uint32_t) } },
    { "dbg_rst", { const_cast<uint32_t(*)>(&Misc_regs_base->dbg_rst), 0, {}, sizeof(uint32_t) } },
    { "gpio_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->gpio_ctrl), 0, {}, sizeof(uint32_t) } },
    { "clkobs_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->clkobs_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pps_pll_ctrl0", { const_cast<uint32_t(*)>(&Misc_regs_base->pps_pll_ctrl0), 0, {}, sizeof(uint32_t) } },
    { "pps_pll_ctrl1", { const_cast<uint32_t(*)>(&Misc_regs_base->pps_pll_ctrl1), 0, {}, sizeof(uint32_t) } },
    { "core_pll_ctrl0", { const_cast<uint32_t(*)>(&Misc_regs_base->core_pll_ctrl0), 0, {}, sizeof(uint32_t) } },
    { "core_pll_ctrl1", { const_cast<uint32_t(*)>(&Misc_regs_base->core_pll_ctrl1), 0, {}, sizeof(uint32_t) } },
    { "mac0_pll_ctrl0", { const_cast<uint32_t(*)>(&Misc_regs_base->mac0_pll_ctrl0), 0, {}, sizeof(uint32_t) } },
    { "mac0_pll_ctrl1", { const_cast<uint32_t(*)>(&Misc_regs_base->mac0_pll_ctrl1), 0, {}, sizeof(uint32_t) } },
    { "mac1_pll_ctrl0", { const_cast<uint32_t(*)>(&Misc_regs_base->mac1_pll_ctrl0), 0, {}, sizeof(uint32_t) } },
    { "mac1_pll_ctrl1", { const_cast<uint32_t(*)>(&Misc_regs_base->mac1_pll_ctrl1), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_lane_ctrl0", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_lane_ctrl0), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_lane_ctrl1", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_lane_ctrl1), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_lane_status0", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_lane_status0), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_lane_status1", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_lane_status1), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_sram_bypass", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_sram_bypass), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_sram_init_status", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_sram_init_status), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_pipe_config", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_pipe_config), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_rtune", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_rtune), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_mode", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_ovr_mode), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_bs_ref_tx_dco_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_ovr_bs_ref_tx_dco_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_rx_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_ovr_rx_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_mplla_ctrl0", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_ovr_mplla_ctrl0), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_mplla_ctrl1", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_ovr_mplla_ctrl1), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_mplla_ctrl2", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_ovr_mplla_ctrl2), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_mplla_ctrl3", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_ovr_mplla_ctrl3), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_mplla_ctrl4", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_ovr_mplla_ctrl4), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_mpllb_ctrl0", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_ovr_mpllb_ctrl0), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_mpllb_ctrl1", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_ovr_mpllb_ctrl1), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_mpllb_ctrl2", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_ovr_mpllb_ctrl2), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_mpllb_ctrl3", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_ovr_mpllb_ctrl3), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_mpllb_ctrl4", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_phy_ovr_mpllb_ctrl4), 0, {}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_rxeq0_ctrl_g1", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pcie_phy_ovr_rxeq0_ctrl_g1), 0, {0x4}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_rxeq0_ctrl_g2", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pcie_phy_ovr_rxeq0_ctrl_g2), 0, {0x4}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_rxeq0_ctrl_g3", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pcie_phy_ovr_rxeq0_ctrl_g3), 0, {0x4}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_rxeq1_ctrl_g1", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pcie_phy_ovr_rxeq1_ctrl_g1), 0, {0x4}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_rxeq1_ctrl_g2", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pcie_phy_ovr_rxeq1_ctrl_g2), 0, {0x4}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_rxeq1_ctrl_g3", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pcie_phy_ovr_rxeq1_ctrl_g3), 0, {0x4}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_txeq_ctrl_g1", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pcie_phy_ovr_txeq_ctrl_g1), 0, {0x4}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_txeq_ctrl_g2", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pcie_phy_ovr_txeq_ctrl_g2), 0, {0x4}, sizeof(uint32_t) } },
    { "pcie_phy_ovr_txeq_ctrl_g3", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pcie_phy_ovr_txeq_ctrl_g3), 0, {0x4}, sizeof(uint32_t) } },
    { "spi_outdata0", { const_cast<uint32_t(*)>(&Misc_regs_base->spi_outdata0), 0, {}, sizeof(uint32_t) } },
    { "spi_outdata1", { const_cast<uint32_t(*)>(&Misc_regs_base->spi_outdata1), 0, {}, sizeof(uint32_t) } },
    { "spi_command", { const_cast<uint32_t(*)>(&Misc_regs_base->spi_command), 0, {}, sizeof(uint32_t) } },
    { "spi_indata", { const_cast<uint32_t(*)>(&Misc_regs_base->spi_indata), 0, {}, sizeof(uint32_t) } },
    { "spi_idcode", { const_cast<uint32_t(*)>(&Misc_regs_base->spi_idcode), 0, {}, sizeof(uint32_t) } },
    { "pciectl_gen3_default", { const_cast<uint32_t(*)>(&Misc_regs_base->pciectl_gen3_default), 0, {}, sizeof(uint32_t) } },
    { "pcie_rxeq_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_rxeq_ctrl), 0, {}, sizeof(uint32_t) } },
    { "baresync_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->baresync_ctrl), 0, {}, sizeof(uint32_t) } },
    { "fuse_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->fuse_ctrl), 0, {}, sizeof(uint32_t) } },
    { "func_fuse", { const_cast<uint32_t(*)[0x10]>(&Misc_regs_base->func_fuse), 0, {0x10}, sizeof(uint32_t) } },
    { "fuse_status", { const_cast<uint32_t(*)>(&Misc_regs_base->fuse_status), 0, {}, sizeof(uint32_t) } },
    { "tcu_control0", { const_cast<uint32_t(*)>(&Misc_regs_base->tcu_control0), 0, {}, sizeof(uint32_t) } },
    { "tcu_control1", { const_cast<uint32_t(*)>(&Misc_regs_base->tcu_control1), 0, {}, sizeof(uint32_t) } },
    { "tcu_wrack", { const_cast<uint32_t(*)>(&Misc_regs_base->tcu_wrack), 0, {}, sizeof(uint32_t) } },
    { "tcu_status", { const_cast<uint32_t(*)>(&Misc_regs_base->tcu_status), 0, {}, sizeof(uint32_t) } },
    { "tv80_debug_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->tv80_debug_ctrl), 0, {}, sizeof(uint32_t) } },
    { "tv80_debug_head_ptr", { const_cast<uint32_t(*)>(&Misc_regs_base->tv80_debug_head_ptr), 0, {}, sizeof(uint32_t) } },
    { "tv80_debug_tail_ptr", { const_cast<uint32_t(*)>(&Misc_regs_base->tv80_debug_tail_ptr), 0, {}, sizeof(uint32_t) } },
    { "tv80_stall_on_error", { const_cast<uint32_t(*)>(&Misc_regs_base->tv80_stall_on_error), 0, {}, sizeof(uint32_t) } },
    { "tv80_halted_status", { const_cast<uint32_t(*)>(&Misc_regs_base->tv80_halted_status), 0, {}, sizeof(uint32_t) } },
    { "tv80_watchdog_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->tv80_watchdog_ctrl), 0, {}, sizeof(uint32_t) } },
    { "tv80_watchdog_count", { const_cast<uint32_t(*)>(&Misc_regs_base->tv80_watchdog_count), 0, {}, sizeof(uint32_t) } },
    { "tv80_addr_msb", { const_cast<uint32_t(*)>(&Misc_regs_base->tv80_addr_msb), 0, {}, sizeof(uint32_t) } },
    { "pvt_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->pvt_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pvt_int", { const_cast<uint32_t(*)>(&Misc_regs_base->pvt_int), 0, {}, sizeof(uint32_t) } },
    { "pvt_status", { const_cast<uint32_t(*)>(&Misc_regs_base->pvt_status), 0, {}, sizeof(uint32_t) } },
    { "misc_intr", { &Misc_regs_base->misc_intr, new Misc_regs_misc_intr_map, {}, sizeof(Misc_regs_misc_intr) } },
    { "sram_ecc", { const_cast<uint32_t(*)>(&Misc_regs_base->sram_ecc), 0, {}, sizeof(uint32_t) } },
    { "pciephy_sram_sbe_err_log", { const_cast<uint32_t(*)>(&Misc_regs_base->pciephy_sram_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "pciephy_sram_mbe_err_log", { const_cast<uint32_t(*)>(&Misc_regs_base->pciephy_sram_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "tv80mem_sbe_err_log", { const_cast<uint32_t(*)>(&Misc_regs_base->tv80mem_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "tv80mem_mbe_err_log", { const_cast<uint32_t(*)>(&Misc_regs_base->tv80mem_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "tv80_intr", { &Misc_regs_base->tv80_intr, new Misc_regs_tv80_intr_map, {}, sizeof(Misc_regs_tv80_intr) } },
    { "pcie_debug_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_debug_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pcie_debug_tail_ptr", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_debug_tail_ptr), 0, {}, sizeof(uint32_t) } },
    { "pcie_debug_head_ptr", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_debug_head_ptr), 0, {}, sizeof(uint32_t) } },
    { "fuse_vid_override", { const_cast<uint32_t(*)>(&Misc_regs_base->fuse_vid_override), 0, {}, sizeof(uint32_t) } },
    { "clkpad_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->clkpad_ctrl), 0, {}, sizeof(uint32_t) } },
    { "barealt_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->barealt_ctrl), 0, {}, sizeof(uint32_t) } },
    { "scratch", { const_cast<uint32_t(*)[0x20]>(&Misc_regs_base->scratch), 0, {0x20}, sizeof(uint32_t) } }
    } )
  {}
};

struct Misc_tv80_rspec_map: public RegisterMapper {
  static constexpr PTR_Misc_tv80_rspec Misc_tv80_rspec_base=0;
  Misc_tv80_rspec_map() : RegisterMapper( {
    { "dummy_register", { const_cast<uint32_t(*)[0x1000]>(&Misc_tv80_rspec_base->dummy_register), 0, {0x1000}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbus_baresync_ts_set_value_map: public RegisterMapper {
  static constexpr PTR_Mbus_baresync_ts_set_value Mbus_baresync_ts_set_value_base=0;
  Mbus_baresync_ts_set_value_map() : RegisterMapper( {
    { "baresync_ts_set_value_0_2", { const_cast<uint32_t(*)>(&Mbus_baresync_ts_set_value_base->baresync_ts_set_value_0_2), 0, {}, sizeof(uint32_t) } },
    { "baresync_ts_set_value_1_2", { const_cast<uint32_t(*)>(&Mbus_baresync_ts_set_value_base->baresync_ts_set_value_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbus_baresync_ts_inc_map: public RegisterMapper {
  static constexpr PTR_Mbus_baresync_ts_inc Mbus_baresync_ts_inc_base=0;
  Mbus_baresync_ts_inc_map() : RegisterMapper( {
    { "baresync_ts_inc_0_3", { const_cast<uint32_t(*)>(&Mbus_baresync_ts_inc_base->baresync_ts_inc_0_3), 0, {}, sizeof(uint32_t) } },
    { "baresync_ts_inc_1_3", { const_cast<uint32_t(*)>(&Mbus_baresync_ts_inc_base->baresync_ts_inc_1_3), 0, {}, sizeof(uint32_t) } },
    { "baresync_ts_inc_2_3", { const_cast<uint32_t(*)>(&Mbus_baresync_ts_inc_base->baresync_ts_inc_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbus_global_ts_set_map: public RegisterMapper {
  static constexpr PTR_Mbus_global_ts_set Mbus_global_ts_set_base=0;
  Mbus_global_ts_set_map() : RegisterMapper( {
    { "global_ts_set_0_2", { const_cast<uint32_t(*)>(&Mbus_global_ts_set_base->global_ts_set_0_2), 0, {}, sizeof(uint32_t) } },
    { "global_ts_set_1_2", { const_cast<uint32_t(*)>(&Mbus_global_ts_set_base->global_ts_set_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbus_global_ts_inc_map: public RegisterMapper {
  static constexpr PTR_Mbus_global_ts_inc Mbus_global_ts_inc_base=0;
  Mbus_global_ts_inc_map() : RegisterMapper( {
    { "global_ts_inc_0_2", { const_cast<uint32_t(*)>(&Mbus_global_ts_inc_base->global_ts_inc_0_2), 0, {}, sizeof(uint32_t) } },
    { "global_ts_inc_1_2", { const_cast<uint32_t(*)>(&Mbus_global_ts_inc_base->global_ts_inc_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbus_global_ts_offset_value_map: public RegisterMapper {
  static constexpr PTR_Mbus_global_ts_offset_value Mbus_global_ts_offset_value_base=0;
  Mbus_global_ts_offset_value_map() : RegisterMapper( {
    { "global_ts_offset_value_0_2", { const_cast<uint32_t(*)>(&Mbus_global_ts_offset_value_base->global_ts_offset_value_0_2), 0, {}, sizeof(uint32_t) } },
    { "global_ts_offset_value_1_2", { const_cast<uint32_t(*)>(&Mbus_global_ts_offset_value_base->global_ts_offset_value_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbus_global_ts_value_map: public RegisterMapper {
  static constexpr PTR_Mbus_global_ts_value Mbus_global_ts_value_base=0;
  Mbus_global_ts_value_map() : RegisterMapper( {
    { "global_ts_value_0_2", { const_cast<uint32_t(*)>(&Mbus_global_ts_value_base->global_ts_value_0_2), 0, {}, sizeof(uint32_t) } },
    { "global_ts_value_1_2", { const_cast<uint32_t(*)>(&Mbus_global_ts_value_base->global_ts_value_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbus_baresync_ts_value_map: public RegisterMapper {
  static constexpr PTR_Mbus_baresync_ts_value Mbus_baresync_ts_value_base=0;
  Mbus_baresync_ts_value_map() : RegisterMapper( {
    { "baresync_ts_value_0_2", { const_cast<uint32_t(*)>(&Mbus_baresync_ts_value_base->baresync_ts_value_0_2), 0, {}, sizeof(uint32_t) } },
    { "baresync_ts_value_1_2", { const_cast<uint32_t(*)>(&Mbus_baresync_ts_value_base->baresync_ts_value_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbus_baresync_alt_ts_value_map: public RegisterMapper {
  static constexpr PTR_Mbus_baresync_alt_ts_value Mbus_baresync_alt_ts_value_base=0;
  Mbus_baresync_alt_ts_value_map() : RegisterMapper( {
    { "baresync_alt_ts_value_0_2", { const_cast<uint32_t(*)>(&Mbus_baresync_alt_ts_value_base->baresync_alt_ts_value_0_2), 0, {}, sizeof(uint32_t) } },
    { "baresync_alt_ts_value_1_2", { const_cast<uint32_t(*)>(&Mbus_baresync_alt_ts_value_base->baresync_alt_ts_value_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbus_host_creq_credit_map: public RegisterMapper {
  static constexpr PTR_Mbus_host_creq_credit Mbus_host_creq_credit_base=0;
  Mbus_host_creq_credit_map() : RegisterMapper( {
    { "host_creq_credit_0_2", { const_cast<uint32_t(*)>(&Mbus_host_creq_credit_base->host_creq_credit_0_2), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_1_2", { const_cast<uint32_t(*)>(&Mbus_host_creq_credit_base->host_creq_credit_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbus_mac_0_creq_credit_map: public RegisterMapper {
  static constexpr PTR_Mbus_mac_0_creq_credit Mbus_mac_0_creq_credit_base=0;
  Mbus_mac_0_creq_credit_map() : RegisterMapper( {
    { "mac_0_creq_credit_0_2", { const_cast<uint32_t(*)>(&Mbus_mac_0_creq_credit_base->mac_0_creq_credit_0_2), 0, {}, sizeof(uint32_t) } },
    { "mac_0_creq_credit_1_2", { const_cast<uint32_t(*)>(&Mbus_mac_0_creq_credit_base->mac_0_creq_credit_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbus_wb_creq_credit_map: public RegisterMapper {
  static constexpr PTR_Mbus_wb_creq_credit Mbus_wb_creq_credit_base=0;
  Mbus_wb_creq_credit_map() : RegisterMapper( {
    { "wb_creq_credit_0_7", { const_cast<uint32_t(*)>(&Mbus_wb_creq_credit_base->wb_creq_credit_0_7), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_1_7", { const_cast<uint32_t(*)>(&Mbus_wb_creq_credit_base->wb_creq_credit_1_7), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_2_7", { const_cast<uint32_t(*)>(&Mbus_wb_creq_credit_base->wb_creq_credit_2_7), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_3_7", { const_cast<uint32_t(*)>(&Mbus_wb_creq_credit_base->wb_creq_credit_3_7), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_4_7", { const_cast<uint32_t(*)>(&Mbus_wb_creq_credit_base->wb_creq_credit_4_7), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_5_7", { const_cast<uint32_t(*)>(&Mbus_wb_creq_credit_base->wb_creq_credit_5_7), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_6_7", { const_cast<uint32_t(*)>(&Mbus_wb_creq_credit_base->wb_creq_credit_6_7), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbus_mac_wb_multicast_vec_map: public RegisterMapper {
  static constexpr PTR_Mbus_mac_wb_multicast_vec Mbus_mac_wb_multicast_vec_base=0;
  Mbus_mac_wb_multicast_vec_map() : RegisterMapper( {
    { "mac_wb_multicast_0_2", { const_cast<uint32_t(*)>(&Mbus_mac_wb_multicast_vec_base->mac_wb_multicast_0_2), 0, {}, sizeof(uint32_t) } },
    { "mac_wb_multicast_1_2", { const_cast<uint32_t(*)>(&Mbus_mac_wb_multicast_vec_base->mac_wb_multicast_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbus_baresync_ts_inc_alt_map: public RegisterMapper {
  static constexpr PTR_Mbus_baresync_ts_inc_alt Mbus_baresync_ts_inc_alt_base=0;
  Mbus_baresync_ts_inc_alt_map() : RegisterMapper( {
    { "baresync_ts_inc_alt_0_3", { const_cast<uint32_t(*)>(&Mbus_baresync_ts_inc_alt_base->baresync_ts_inc_alt_0_3), 0, {}, sizeof(uint32_t) } },
    { "baresync_ts_inc_alt_1_3", { const_cast<uint32_t(*)>(&Mbus_baresync_ts_inc_alt_base->baresync_ts_inc_alt_1_3), 0, {}, sizeof(uint32_t) } },
    { "baresync_ts_inc_alt_2_3", { const_cast<uint32_t(*)>(&Mbus_baresync_ts_inc_alt_base->baresync_ts_inc_alt_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbus_baresync_alt_cyc_status_map: public RegisterMapper {
  static constexpr PTR_Mbus_baresync_alt_cyc_status Mbus_baresync_alt_cyc_status_base=0;
  Mbus_baresync_alt_cyc_status_map() : RegisterMapper( {
    { "baresync_alt_cyc_0_2", { const_cast<uint32_t(*)>(&Mbus_baresync_alt_cyc_status_base->baresync_alt_cyc_0_2), 0, {}, sizeof(uint32_t) } },
    { "baresync_alt_cyc_1_2", { const_cast<uint32_t(*)>(&Mbus_baresync_alt_cyc_status_base->baresync_alt_cyc_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbus_rspec_map: public RegisterMapper {
  static constexpr PTR_Mbus_rspec Mbus_rspec_base=0;
  Mbus_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)[0x4]>(&Mbus_rspec_base->scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Mbus_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "ctrl", { const_cast<uint32_t(*)>(&Mbus_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "flush", { const_cast<uint32_t(*)>(&Mbus_rspec_base->flush), 0, {}, sizeof(uint32_t) } },
    { "link_down", { const_cast<uint32_t(*)>(&Mbus_rspec_base->link_down), 0, {}, sizeof(uint32_t) } },
    { "baresync_ts_set_value", { const_cast<Mbus_baresync_ts_set_value(*)>(&Mbus_rspec_base->baresync_ts_set_value), new Mbus_baresync_ts_set_value_map, {}, sizeof(Mbus_baresync_ts_set_value) } },
    { "baresync_ts_inc", { const_cast<Mbus_baresync_ts_inc(*)>(&Mbus_rspec_base->baresync_ts_inc), new Mbus_baresync_ts_inc_map, {}, sizeof(Mbus_baresync_ts_inc) } },
    { "global_ts_set", { const_cast<Mbus_global_ts_set(*)>(&Mbus_rspec_base->global_ts_set), new Mbus_global_ts_set_map, {}, sizeof(Mbus_global_ts_set) } },
    { "global_ts_inc", { const_cast<Mbus_global_ts_inc(*)>(&Mbus_rspec_base->global_ts_inc), new Mbus_global_ts_inc_map, {}, sizeof(Mbus_global_ts_inc) } },
    { "global_ts_inc_value", { const_cast<uint32_t(*)>(&Mbus_rspec_base->global_ts_inc_value), 0, {}, sizeof(uint32_t) } },
    { "global_ts_offset_value", { const_cast<Mbus_global_ts_offset_value(*)>(&Mbus_rspec_base->global_ts_offset_value), new Mbus_global_ts_offset_value_map, {}, sizeof(Mbus_global_ts_offset_value) } },
    { "ts_timer", { const_cast<uint32_t(*)>(&Mbus_rspec_base->ts_timer), 0, {}, sizeof(uint32_t) } },
    { "ts_capture", { const_cast<uint32_t(*)>(&Mbus_rspec_base->ts_capture), 0, {}, sizeof(uint32_t) } },
    { "global_ts_value", { const_cast<Mbus_global_ts_value(*)>(&Mbus_rspec_base->global_ts_value), new Mbus_global_ts_value_map, {}, sizeof(Mbus_global_ts_value) } },
    { "baresync_ts_value", { const_cast<Mbus_baresync_ts_value(*)>(&Mbus_rspec_base->baresync_ts_value), new Mbus_baresync_ts_value_map, {}, sizeof(Mbus_baresync_ts_value) } },
    { "baresync_alt_ts_value", { const_cast<Mbus_baresync_alt_ts_value(*)>(&Mbus_rspec_base->baresync_alt_ts_value), new Mbus_baresync_alt_ts_value_map, {}, sizeof(Mbus_baresync_alt_ts_value) } },
    { "intr_stat", { const_cast<uint32_t(*)>(&Mbus_rspec_base->intr_stat), 0, {}, sizeof(uint32_t) } },
    { "intr_en_0", { const_cast<uint32_t(*)>(&Mbus_rspec_base->intr_en_0), 0, {}, sizeof(uint32_t) } },
    { "intr_en_1", { const_cast<uint32_t(*)>(&Mbus_rspec_base->intr_en_1), 0, {}, sizeof(uint32_t) } },
    { "freeze_en", { const_cast<uint32_t(*)>(&Mbus_rspec_base->freeze_en), 0, {}, sizeof(uint32_t) } },
    { "intr_inj", { const_cast<uint32_t(*)>(&Mbus_rspec_base->intr_inj), 0, {}, sizeof(uint32_t) } },
    { "mac_0_tx_dr_rd_err_log", { const_cast<uint32_t(*)>(&Mbus_rspec_base->mac_0_tx_dr_rd_err_log), 0, {}, sizeof(uint32_t) } },
    { "wb_tx_dr_rd_err_log", { const_cast<uint32_t(*)>(&Mbus_rspec_base->wb_tx_dr_rd_err_log), 0, {}, sizeof(uint32_t) } },
    { "controller_mbe_log", { const_cast<uint32_t(*)>(&Mbus_rspec_base->controller_mbe_log), 0, {}, sizeof(uint32_t) } },
    { "controller_sbe_log", { const_cast<uint32_t(*)>(&Mbus_rspec_base->controller_sbe_log), 0, {}, sizeof(uint32_t) } },
    { "parity_err_log", { const_cast<uint32_t(*)[0x4]>(&Mbus_rspec_base->parity_err_log), 0, {0x4}, sizeof(uint32_t) } },
    { "host_creq_credit", { const_cast<Mbus_host_creq_credit(*)>(&Mbus_rspec_base->host_creq_credit), new Mbus_host_creq_credit_map, {}, sizeof(Mbus_host_creq_credit) } },
    { "mac_0_creq_credit", { const_cast<Mbus_mac_0_creq_credit(*)>(&Mbus_rspec_base->mac_0_creq_credit), new Mbus_mac_0_creq_credit_map, {}, sizeof(Mbus_mac_0_creq_credit) } },
    { "wb_creq_credit", { const_cast<Mbus_wb_creq_credit(*)>(&Mbus_rspec_base->wb_creq_credit), new Mbus_wb_creq_credit_map, {}, sizeof(Mbus_wb_creq_credit) } },
    { "host_slave_credit", { const_cast<uint32_t(*)>(&Mbus_rspec_base->host_slave_credit), 0, {}, sizeof(uint32_t) } },
    { "mac_0_dma_log", { const_cast<uint32_t(*)>(&Mbus_rspec_base->mac_0_dma_log), 0, {}, sizeof(uint32_t) } },
    { "wb_dma_log", { const_cast<uint32_t(*)>(&Mbus_rspec_base->wb_dma_log), 0, {}, sizeof(uint32_t) } },
    { "ts_4ns_set_value", { const_cast<uint32_t(*)>(&Mbus_rspec_base->ts_4ns_set_value), 0, {}, sizeof(uint32_t) } },
    { "ts_4ns_inc_value", { const_cast<uint32_t(*)>(&Mbus_rspec_base->ts_4ns_inc_value), 0, {}, sizeof(uint32_t) } },
    { "mac_wb_multicast", { const_cast<Mbus_mac_wb_multicast_vec(*)>(&Mbus_rspec_base->mac_wb_multicast), new Mbus_mac_wb_multicast_vec_map, {}, sizeof(Mbus_mac_wb_multicast_vec) } },
    { "baresync_ts_inc_alt", { const_cast<Mbus_baresync_ts_inc_alt(*)>(&Mbus_rspec_base->baresync_ts_inc_alt), new Mbus_baresync_ts_inc_alt_map, {}, sizeof(Mbus_baresync_ts_inc_alt) } },
    { "ts_4ns_value", { const_cast<uint32_t(*)>(&Mbus_rspec_base->ts_4ns_value), 0, {}, sizeof(uint32_t) } },
    { "ts_4ns_capture", { const_cast<uint32_t(*)>(&Mbus_rspec_base->ts_4ns_capture), 0, {}, sizeof(uint32_t) } },
    { "baresync_alt_cyc", { const_cast<Mbus_baresync_alt_cyc_status(*)>(&Mbus_rspec_base->baresync_alt_cyc), new Mbus_baresync_alt_cyc_status_map, {}, sizeof(Mbus_baresync_alt_cyc_status) } },
    { "baresync_ts_inc_alt_gap", { const_cast<uint32_t(*)>(&Mbus_rspec_base->baresync_ts_inc_alt_gap), 0, {}, sizeof(uint32_t) } },
    { "baresync_ts_inc_alt_cyc", { const_cast<uint32_t(*)>(&Mbus_rspec_base->baresync_ts_inc_alt_cyc), 0, {}, sizeof(uint32_t) } },
    { "mst_ctrl_log", { const_cast<uint32_t(*)>(&Mbus_rspec_base->mst_ctrl_log), 0, {}, sizeof(uint32_t) } },
    { "global_ts_log", { const_cast<uint32_t(*)>(&Mbus_rspec_base->global_ts_log), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dru_rspec_map: public RegisterMapper {
  static constexpr PTR_Dru_rspec Dru_rspec_base=0;
  Dru_rspec_map() : RegisterMapper( {
    { "ctrl", { const_cast<uint32_t(*)>(&Dru_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "base_addr_low", { const_cast<uint32_t(*)>(&Dru_rspec_base->base_addr_low), 0, {}, sizeof(uint32_t) } },
    { "base_addr_high", { const_cast<uint32_t(*)>(&Dru_rspec_base->base_addr_high), 0, {}, sizeof(uint32_t) } },
    { "limit_addr_low", { const_cast<uint32_t(*)>(&Dru_rspec_base->limit_addr_low), 0, {}, sizeof(uint32_t) } },
    { "limit_addr_high", { const_cast<uint32_t(*)>(&Dru_rspec_base->limit_addr_high), 0, {}, sizeof(uint32_t) } },
    { "size", { const_cast<uint32_t(*)>(&Dru_rspec_base->size), 0, {}, sizeof(uint32_t) } },
    { "head_ptr", { const_cast<uint32_t(*)>(&Dru_rspec_base->head_ptr), 0, {}, sizeof(uint32_t) } },
    { "tail_ptr", { const_cast<uint32_t(*)>(&Dru_rspec_base->tail_ptr), 0, {}, sizeof(uint32_t) } },
    { "ring_timeout", { const_cast<uint32_t(*)>(&Dru_rspec_base->ring_timeout), 0, {}, sizeof(uint32_t) } },
    { "data_timeout", { const_cast<uint32_t(*)>(&Dru_rspec_base->data_timeout), 0, {}, sizeof(uint32_t) } },
    { "status", { const_cast<uint32_t(*)>(&Dru_rspec_base->status), 0, {}, sizeof(uint32_t) } },
    { "empty_int_time", { const_cast<uint32_t(*)>(&Dru_rspec_base->empty_int_time), 0, {}, sizeof(uint32_t) } },
    { "empty_int_count", { const_cast<uint32_t(*)>(&Dru_rspec_base->empty_int_count), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbc_rspec_map: public RegisterMapper {
  static constexpr PTR_Mbc_rspec Mbc_rspec_base=0;
  Mbc_rspec_map() : RegisterMapper( {
    { "mbc_mbus", { &Mbc_rspec_base->mbc_mbus, new Mbus_rspec_map, {}, sizeof(Mbus_rspec) } },
    { "mbc_mac_0_tx_dr", { &Mbc_rspec_base->mbc_mac_0_tx_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "mbc_mac_0_cpl_dr", { &Mbc_rspec_base->mbc_mac_0_cpl_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "mbc_wb_tx_dr", { &Mbc_rspec_base->mbc_wb_tx_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "mbc_wb_cpl_dr", { &Mbc_rspec_base->mbc_wb_cpl_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } }
    } )
  {}
};

struct Pbus_host_creq_credit_map: public RegisterMapper {
  static constexpr PTR_Pbus_host_creq_credit Pbus_host_creq_credit_base=0;
  Pbus_host_creq_credit_map() : RegisterMapper( {
    { "host_creq_credit_0_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_0_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_1_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_1_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_2_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_2_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_3_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_3_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_4_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_4_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_5_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_5_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_6_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_6_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_7_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_7_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_8_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_8_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_9_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_9_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_10_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_10_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_11_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_11_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_12_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_12_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_13_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_13_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_14_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_14_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_15_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_15_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_16_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_16_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_17_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_17_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_18_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_18_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_19_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_19_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_20_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_20_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_21_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_21_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_22_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_22_24), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_23_24", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_23_24), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pbus_il_creq_credit_map: public RegisterMapper {
  static constexpr PTR_Pbus_il_creq_credit Pbus_il_creq_credit_base=0;
  Pbus_il_creq_credit_map() : RegisterMapper( {
    { "il_creq_credit_0_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_0_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_1_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_1_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_2_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_2_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_3_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_3_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_4_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_4_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_5_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_5_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_6_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_6_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_7_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_7_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_8_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_8_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_9_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_9_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_10_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_10_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_11_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_11_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_12_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_12_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_13_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_13_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_14_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_14_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_15_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_15_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_16_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_16_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_17_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_17_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_18_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_18_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_19_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_19_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_20_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_20_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_21_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_21_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_22_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_22_24), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_23_24", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_23_24), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pbus_wb_creq_credit_map: public RegisterMapper {
  static constexpr PTR_Pbus_wb_creq_credit Pbus_wb_creq_credit_base=0;
  Pbus_wb_creq_credit_map() : RegisterMapper( {
    { "wb_creq_credit_0_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_0_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_1_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_1_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_2_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_2_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_3_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_3_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_4_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_4_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_5_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_5_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_6_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_6_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_7_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_7_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_8_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_8_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_9_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_9_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_10_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_10_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_11_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_11_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_12_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_12_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_13_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_13_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_14_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_14_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_15_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_15_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_16_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_16_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_17_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_17_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_18_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_18_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_19_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_19_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_20_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_20_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_21_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_21_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_22_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_22_24), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_23_24", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_23_24), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pbus_rb_creq_credit_map: public RegisterMapper {
  static constexpr PTR_Pbus_rb_creq_credit Pbus_rb_creq_credit_base=0;
  Pbus_rb_creq_credit_map() : RegisterMapper( {
    { "rb_creq_credit_0_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_0_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_1_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_1_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_2_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_2_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_3_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_3_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_4_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_4_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_5_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_5_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_6_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_6_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_7_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_7_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_8_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_8_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_9_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_9_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_10_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_10_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_11_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_11_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_12_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_12_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_13_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_13_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_14_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_14_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_15_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_15_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_16_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_16_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_17_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_17_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_18_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_18_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_19_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_19_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_20_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_20_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_21_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_21_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_22_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_22_24), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_23_24", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_23_24), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pbus_dma_log_map: public RegisterMapper {
  static constexpr PTR_Pbus_dma_log Pbus_dma_log_base=0;
  Pbus_dma_log_map() : RegisterMapper( {
    { "dma_log_0_7", { const_cast<uint32_t(*)>(&Pbus_dma_log_base->dma_log_0_7), 0, {}, sizeof(uint32_t) } },
    { "dma_log_1_7", { const_cast<uint32_t(*)>(&Pbus_dma_log_base->dma_log_1_7), 0, {}, sizeof(uint32_t) } },
    { "dma_log_2_7", { const_cast<uint32_t(*)>(&Pbus_dma_log_base->dma_log_2_7), 0, {}, sizeof(uint32_t) } },
    { "dma_log_3_7", { const_cast<uint32_t(*)>(&Pbus_dma_log_base->dma_log_3_7), 0, {}, sizeof(uint32_t) } },
    { "dma_log_4_7", { const_cast<uint32_t(*)>(&Pbus_dma_log_base->dma_log_4_7), 0, {}, sizeof(uint32_t) } },
    { "dma_log_5_7", { const_cast<uint32_t(*)>(&Pbus_dma_log_base->dma_log_5_7), 0, {}, sizeof(uint32_t) } },
    { "dma_log_6_7", { const_cast<uint32_t(*)>(&Pbus_dma_log_base->dma_log_6_7), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pbus_rspec_map: public RegisterMapper {
  static constexpr PTR_Pbus_rspec Pbus_rspec_base=0;
  Pbus_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)[0x4]>(&Pbus_rspec_base->scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "ctrl", { const_cast<uint32_t(*)>(&Pbus_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "flush", { const_cast<uint32_t(*)>(&Pbus_rspec_base->flush), 0, {}, sizeof(uint32_t) } },
    { "link_down", { const_cast<uint32_t(*)>(&Pbus_rspec_base->link_down), 0, {}, sizeof(uint32_t) } },
    { "arb_ctrl0", { const_cast<uint32_t(*)>(&Pbus_rspec_base->arb_ctrl0), 0, {}, sizeof(uint32_t) } },
    { "arb_ctrl1", { const_cast<uint32_t(*)[0x4]>(&Pbus_rspec_base->arb_ctrl1), 0, {0x4}, sizeof(uint32_t) } },
    { "pri_ctrl", { const_cast<uint32_t(*)>(&Pbus_rspec_base->pri_ctrl), 0, {}, sizeof(uint32_t) } },
    { "intr_stat0", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_stat0), 0, {}, sizeof(uint32_t) } },
    { "intr_stat1", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_stat1), 0, {}, sizeof(uint32_t) } },
    { "intr_stat2", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_stat2), 0, {}, sizeof(uint32_t) } },
    { "intr_stat3", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_stat3), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_0", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en0_0), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_1", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en0_1), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_2", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en0_2), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_3", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en0_3), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_4", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en0_4), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_5", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en0_5), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_6", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en0_6), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_7", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en0_7), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_0", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en1_0), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_1", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en1_1), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_2", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en1_2), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_3", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en1_3), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_4", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en1_4), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_5", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en1_5), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_6", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en1_6), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_7", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en1_7), 0, {}, sizeof(uint32_t) } },
    { "intr_en2_0", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en2_0), 0, {}, sizeof(uint32_t) } },
    { "intr_en2_1", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en2_1), 0, {}, sizeof(uint32_t) } },
    { "intr_en2_2", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en2_2), 0, {}, sizeof(uint32_t) } },
    { "intr_en2_3", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en2_3), 0, {}, sizeof(uint32_t) } },
    { "intr_en2_4", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en2_4), 0, {}, sizeof(uint32_t) } },
    { "intr_en2_5", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en2_5), 0, {}, sizeof(uint32_t) } },
    { "intr_en2_6", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en2_6), 0, {}, sizeof(uint32_t) } },
    { "intr_en2_7", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en2_7), 0, {}, sizeof(uint32_t) } },
    { "intr_en3_0", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en3_0), 0, {}, sizeof(uint32_t) } },
    { "intr_en3_1", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en3_1), 0, {}, sizeof(uint32_t) } },
    { "intr_en3_2", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en3_2), 0, {}, sizeof(uint32_t) } },
    { "intr_en3_3", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en3_3), 0, {}, sizeof(uint32_t) } },
    { "intr_en3_4", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en3_4), 0, {}, sizeof(uint32_t) } },
    { "intr_en3_5", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en3_5), 0, {}, sizeof(uint32_t) } },
    { "intr_en3_6", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en3_6), 0, {}, sizeof(uint32_t) } },
    { "intr_en3_7", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_en3_7), 0, {}, sizeof(uint32_t) } },
    { "freeze_en0", { const_cast<uint32_t(*)>(&Pbus_rspec_base->freeze_en0), 0, {}, sizeof(uint32_t) } },
    { "freeze_en1", { const_cast<uint32_t(*)>(&Pbus_rspec_base->freeze_en1), 0, {}, sizeof(uint32_t) } },
    { "freeze_en2", { const_cast<uint32_t(*)>(&Pbus_rspec_base->freeze_en2), 0, {}, sizeof(uint32_t) } },
    { "freeze_en3", { const_cast<uint32_t(*)>(&Pbus_rspec_base->freeze_en3), 0, {}, sizeof(uint32_t) } },
    { "intr_inj", { const_cast<uint32_t(*)>(&Pbus_rspec_base->intr_inj), 0, {}, sizeof(uint32_t) } },
    { "diag_ts", { const_cast<uint32_t(*)>(&Pbus_rspec_base->diag_ts), 0, {}, sizeof(uint32_t) } },
    { "diag_delay", { const_cast<uint32_t(*)>(&Pbus_rspec_base->diag_delay), 0, {}, sizeof(uint32_t) } },
    { "diag_edge", { const_cast<uint32_t(*)[0x4]>(&Pbus_rspec_base->diag_edge), 0, {0x4}, sizeof(uint32_t) } },
    { "diag_value", { const_cast<uint32_t(*)[0x4]>(&Pbus_rspec_base->diag_value), 0, {0x4}, sizeof(uint32_t) } },
    { "diag_mask", { const_cast<uint32_t(*)[0x4]>(&Pbus_rspec_base->diag_mask), 0, {0x4}, sizeof(uint32_t) } },
    { "il_address", { const_cast<uint32_t(*)[0x4]>(&Pbus_rspec_base->il_address), 0, {0x4}, sizeof(uint32_t) } },
    { "il_tx_dr_rd_err_log", { const_cast<uint32_t(*)[0x4]>(&Pbus_rspec_base->il_tx_dr_rd_err_log), 0, {0x4}, sizeof(uint32_t) } },
    { "wb_tx_dr_rd_err_log", { const_cast<uint32_t(*)>(&Pbus_rspec_base->wb_tx_dr_rd_err_log), 0, {}, sizeof(uint32_t) } },
    { "rb_tx_dr_rd_err_log", { const_cast<uint32_t(*)>(&Pbus_rspec_base->rb_tx_dr_rd_err_log), 0, {}, sizeof(uint32_t) } },
    { "stat_fm_dr_rd_err_log", { const_cast<uint32_t(*)>(&Pbus_rspec_base->stat_fm_dr_rd_err_log), 0, {}, sizeof(uint32_t) } },
    { "idle_fm_dr_rd_err_log", { const_cast<uint32_t(*)>(&Pbus_rspec_base->idle_fm_dr_rd_err_log), 0, {}, sizeof(uint32_t) } },
    { "diag_fm_dr_rd_err_log", { const_cast<uint32_t(*)>(&Pbus_rspec_base->diag_fm_dr_rd_err_log), 0, {}, sizeof(uint32_t) } },
    { "controller_mbe_log", { const_cast<uint32_t(*)>(&Pbus_rspec_base->controller_mbe_log), 0, {}, sizeof(uint32_t) } },
    { "controller_sbe_log", { const_cast<uint32_t(*)>(&Pbus_rspec_base->controller_sbe_log), 0, {}, sizeof(uint32_t) } },
    { "parity_err_log", { const_cast<uint32_t(*)[0x7]>(&Pbus_rspec_base->parity_err_log), 0, {0x7}, sizeof(uint32_t) } },
    { "host_creq_credit", { const_cast<Pbus_host_creq_credit(*)>(&Pbus_rspec_base->host_creq_credit), new Pbus_host_creq_credit_map, {}, sizeof(Pbus_host_creq_credit) } },
    { "il_creq_credit", { const_cast<Pbus_il_creq_credit(*)[0x2]>(&Pbus_rspec_base->il_creq_credit), new Pbus_il_creq_credit_map, {0x2}, sizeof(Pbus_il_creq_credit) } },
    { "wb_creq_credit", { const_cast<Pbus_wb_creq_credit(*)>(&Pbus_rspec_base->wb_creq_credit), new Pbus_wb_creq_credit_map, {}, sizeof(Pbus_wb_creq_credit) } },
    { "rb_creq_credit", { const_cast<Pbus_rb_creq_credit(*)>(&Pbus_rspec_base->rb_creq_credit), new Pbus_rb_creq_credit_map, {}, sizeof(Pbus_rb_creq_credit) } },
    { "sreq_slot_credit", { const_cast<uint32_t(*)>(&Pbus_rspec_base->sreq_slot_credit), 0, {}, sizeof(uint32_t) } },
    { "host_slave_credit", { const_cast<uint32_t(*)>(&Pbus_rspec_base->host_slave_credit), 0, {}, sizeof(uint32_t) } },
    { "il_qid_map", { const_cast<uint32_t(*)>(&Pbus_rspec_base->il_qid_map), 0, {}, sizeof(uint32_t) } },
    { "stn_fairness", { const_cast<uint32_t(*)>(&Pbus_rspec_base->stn_fairness), 0, {}, sizeof(uint32_t) } },
    { "dma_log", { const_cast<Pbus_dma_log(*)>(&Pbus_rspec_base->dma_log), new Pbus_dma_log_map, {}, sizeof(Pbus_dma_log) } },
    { "mst_ctrl_log", { const_cast<uint32_t(*)>(&Pbus_rspec_base->mst_ctrl_log), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pbc_rspec_map: public RegisterMapper {
  static constexpr PTR_Pbc_rspec Pbc_rspec_base=0;
  Pbc_rspec_map() : RegisterMapper( {
    { "pbc_pbus", { &Pbc_rspec_base->pbc_pbus, new Pbus_rspec_map, {}, sizeof(Pbus_rspec) } },
    { "pbc_il_tx_dr", { &Pbc_rspec_base->pbc_il_tx_dr, new Dru_rspec_map, {0x4}, sizeof(Dru_rspec) } },
    { "pbc_il_cpl_dr", { &Pbc_rspec_base->pbc_il_cpl_dr, new Dru_rspec_map, {0x4}, sizeof(Dru_rspec) } },
    { "pbc_wb_tx_dr", { &Pbc_rspec_base->pbc_wb_tx_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "pbc_wb_cpl_dr", { &Pbc_rspec_base->pbc_wb_cpl_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "pbc_rb_tx_dr", { &Pbc_rspec_base->pbc_rb_tx_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "pbc_rb_cpl_dr", { &Pbc_rspec_base->pbc_rb_cpl_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "pbc_stat_fm_dr", { &Pbc_rspec_base->pbc_stat_fm_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "pbc_stat_rx_dr", { &Pbc_rspec_base->pbc_stat_rx_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "pbc_idle_fm_dr", { &Pbc_rspec_base->pbc_idle_fm_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "pbc_idle_rx_dr", { &Pbc_rspec_base->pbc_idle_rx_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "pbc_diag_fm_dr", { &Pbc_rspec_base->pbc_diag_fm_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "pbc_diag_rx_dr", { &Pbc_rspec_base->pbc_diag_rx_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } }
    } )
  {}
};

struct Cbus_host_creq_credit_map: public RegisterMapper {
  static constexpr PTR_Cbus_host_creq_credit Cbus_host_creq_credit_base=0;
  Cbus_host_creq_credit_map() : RegisterMapper( {
    { "host_creq_credit_0_3", { const_cast<uint32_t(*)>(&Cbus_host_creq_credit_base->host_creq_credit_0_3), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_1_3", { const_cast<uint32_t(*)>(&Cbus_host_creq_credit_base->host_creq_credit_1_3), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_2_3", { const_cast<uint32_t(*)>(&Cbus_host_creq_credit_base->host_creq_credit_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Cbus_wl_creq_credit_map: public RegisterMapper {
  static constexpr PTR_Cbus_wl_creq_credit Cbus_wl_creq_credit_base=0;
  Cbus_wl_creq_credit_map() : RegisterMapper( {
    { "wl_creq_credit_0_3", { const_cast<uint32_t(*)>(&Cbus_wl_creq_credit_base->wl_creq_credit_0_3), 0, {}, sizeof(uint32_t) } },
    { "wl_creq_credit_1_3", { const_cast<uint32_t(*)>(&Cbus_wl_creq_credit_base->wl_creq_credit_1_3), 0, {}, sizeof(uint32_t) } },
    { "wl_creq_credit_2_3", { const_cast<uint32_t(*)>(&Cbus_wl_creq_credit_base->wl_creq_credit_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Cbus_rb_creq_credit_map: public RegisterMapper {
  static constexpr PTR_Cbus_rb_creq_credit Cbus_rb_creq_credit_base=0;
  Cbus_rb_creq_credit_map() : RegisterMapper( {
    { "rb_creq_credit_0_3", { const_cast<uint32_t(*)>(&Cbus_rb_creq_credit_base->rb_creq_credit_0_3), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_1_3", { const_cast<uint32_t(*)>(&Cbus_rb_creq_credit_base->rb_creq_credit_1_3), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_2_3", { const_cast<uint32_t(*)>(&Cbus_rb_creq_credit_base->rb_creq_credit_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Cbus_dma_log_map: public RegisterMapper {
  static constexpr PTR_Cbus_dma_log Cbus_dma_log_base=0;
  Cbus_dma_log_map() : RegisterMapper( {
    { "dma_log_0_3", { const_cast<uint32_t(*)>(&Cbus_dma_log_base->dma_log_0_3), 0, {}, sizeof(uint32_t) } },
    { "dma_log_1_3", { const_cast<uint32_t(*)>(&Cbus_dma_log_base->dma_log_1_3), 0, {}, sizeof(uint32_t) } },
    { "dma_log_2_3", { const_cast<uint32_t(*)>(&Cbus_dma_log_base->dma_log_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Cbus_rspec_map: public RegisterMapper {
  static constexpr PTR_Cbus_rspec Cbus_rspec_base=0;
  Cbus_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)[0x4]>(&Cbus_rspec_base->scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "ctrl", { const_cast<uint32_t(*)>(&Cbus_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "flush", { const_cast<uint32_t(*)>(&Cbus_rspec_base->flush), 0, {}, sizeof(uint32_t) } },
    { "link_down", { const_cast<uint32_t(*)>(&Cbus_rspec_base->link_down), 0, {}, sizeof(uint32_t) } },
    { "arb_ctrl", { const_cast<uint32_t(*)>(&Cbus_rspec_base->arb_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pri_ctrl", { const_cast<uint32_t(*)>(&Cbus_rspec_base->pri_ctrl), 0, {}, sizeof(uint32_t) } },
    { "intr_stat0", { const_cast<uint32_t(*)>(&Cbus_rspec_base->intr_stat0), 0, {}, sizeof(uint32_t) } },
    { "intr_stat1", { const_cast<uint32_t(*)>(&Cbus_rspec_base->intr_stat1), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_0", { const_cast<uint32_t(*)>(&Cbus_rspec_base->intr_en0_0), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_1", { const_cast<uint32_t(*)>(&Cbus_rspec_base->intr_en0_1), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_2", { const_cast<uint32_t(*)>(&Cbus_rspec_base->intr_en0_2), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_3", { const_cast<uint32_t(*)>(&Cbus_rspec_base->intr_en0_3), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_0", { const_cast<uint32_t(*)>(&Cbus_rspec_base->intr_en1_0), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_1", { const_cast<uint32_t(*)>(&Cbus_rspec_base->intr_en1_1), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_2", { const_cast<uint32_t(*)>(&Cbus_rspec_base->intr_en1_2), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_3", { const_cast<uint32_t(*)>(&Cbus_rspec_base->intr_en1_3), 0, {}, sizeof(uint32_t) } },
    { "freeze_en0", { const_cast<uint32_t(*)>(&Cbus_rspec_base->freeze_en0), 0, {}, sizeof(uint32_t) } },
    { "freeze_en1", { const_cast<uint32_t(*)>(&Cbus_rspec_base->freeze_en1), 0, {}, sizeof(uint32_t) } },
    { "intr_inj", { const_cast<uint32_t(*)>(&Cbus_rspec_base->intr_inj), 0, {}, sizeof(uint32_t) } },
    { "wl_tx_dr_rd_err_log", { const_cast<uint32_t(*)[0x2]>(&Cbus_rspec_base->wl_tx_dr_rd_err_log), 0, {0x2}, sizeof(uint32_t) } },
    { "rb_tx_dr_rd_err_log", { const_cast<uint32_t(*)[0x2]>(&Cbus_rspec_base->rb_tx_dr_rd_err_log), 0, {0x2}, sizeof(uint32_t) } },
    { "lq_fm_dr_rd_err_log", { const_cast<uint32_t(*)>(&Cbus_rspec_base->lq_fm_dr_rd_err_log), 0, {}, sizeof(uint32_t) } },
    { "controller_mbe_log", { const_cast<uint32_t(*)>(&Cbus_rspec_base->controller_mbe_log), 0, {}, sizeof(uint32_t) } },
    { "controller_sbe_log", { const_cast<uint32_t(*)>(&Cbus_rspec_base->controller_sbe_log), 0, {}, sizeof(uint32_t) } },
    { "parity_err_log", { const_cast<uint32_t(*)[0x7]>(&Cbus_rspec_base->parity_err_log), 0, {0x7}, sizeof(uint32_t) } },
    { "host_creq_credit", { const_cast<Cbus_host_creq_credit(*)>(&Cbus_rspec_base->host_creq_credit), new Cbus_host_creq_credit_map, {}, sizeof(Cbus_host_creq_credit) } },
    { "wl_creq_credit", { const_cast<Cbus_wl_creq_credit(*)>(&Cbus_rspec_base->wl_creq_credit), new Cbus_wl_creq_credit_map, {}, sizeof(Cbus_wl_creq_credit) } },
    { "rb_creq_credit", { const_cast<Cbus_rb_creq_credit(*)>(&Cbus_rspec_base->rb_creq_credit), new Cbus_rb_creq_credit_map, {}, sizeof(Cbus_rb_creq_credit) } },
    { "lq_slot_credit", { const_cast<uint32_t(*)>(&Cbus_rspec_base->lq_slot_credit), 0, {}, sizeof(uint32_t) } },
    { "host_slave_credit", { const_cast<uint32_t(*)>(&Cbus_rspec_base->host_slave_credit), 0, {}, sizeof(uint32_t) } },
    { "dma_log", { const_cast<Cbus_dma_log(*)>(&Cbus_rspec_base->dma_log), new Cbus_dma_log_map, {}, sizeof(Cbus_dma_log) } },
    { "mst_ctrl_log", { const_cast<uint32_t(*)>(&Cbus_rspec_base->mst_ctrl_log), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Cbc_rspec_map: public RegisterMapper {
  static constexpr PTR_Cbc_rspec Cbc_rspec_base=0;
  Cbc_rspec_map() : RegisterMapper( {
    { "cbc_cbus", { &Cbc_rspec_base->cbc_cbus, new Cbus_rspec_map, {}, sizeof(Cbus_rspec) } },
    { "cbc_wl0_tx_dr", { &Cbc_rspec_base->cbc_wl0_tx_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "cbc_wl0_cpl_dr", { &Cbc_rspec_base->cbc_wl0_cpl_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "cbc_wl1_tx_dr", { &Cbc_rspec_base->cbc_wl1_tx_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "cbc_wl1_cpl_dr", { &Cbc_rspec_base->cbc_wl1_cpl_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "cbc_rb0_tx_dr", { &Cbc_rspec_base->cbc_rb0_tx_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "cbc_rb0_cpl_dr", { &Cbc_rspec_base->cbc_rb0_cpl_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "cbc_rb1_tx_dr", { &Cbc_rspec_base->cbc_rb1_tx_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "cbc_rb1_cpl_dr", { &Cbc_rspec_base->cbc_rb1_cpl_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "cbc_lq_fm_dr", { &Cbc_rspec_base->cbc_lq_fm_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "cbc_lq_rx_dr", { &Cbc_rspec_base->cbc_lq_rx_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } }
    } )
  {}
};

struct Tbus_rspec_map: public RegisterMapper {
  static constexpr PTR_Tbus_rspec Tbus_rspec_base=0;
  Tbus_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)[0x4]>(&Tbus_rspec_base->scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "ctrl", { const_cast<uint32_t(*)>(&Tbus_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "flush", { const_cast<uint32_t(*)>(&Tbus_rspec_base->flush), 0, {}, sizeof(uint32_t) } },
    { "link_down", { const_cast<uint32_t(*)>(&Tbus_rspec_base->link_down), 0, {}, sizeof(uint32_t) } },
    { "ts", { const_cast<uint32_t(*)>(&Tbus_rspec_base->ts), 0, {}, sizeof(uint32_t) } },
    { "intr_stat0", { const_cast<uint32_t(*)>(&Tbus_rspec_base->intr_stat0), 0, {}, sizeof(uint32_t) } },
    { "intr_stat1", { const_cast<uint32_t(*)>(&Tbus_rspec_base->intr_stat1), 0, {}, sizeof(uint32_t) } },
    { "intr_stat2", { const_cast<uint32_t(*)>(&Tbus_rspec_base->intr_stat2), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_0", { const_cast<uint32_t(*)>(&Tbus_rspec_base->intr_en0_0), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_1", { const_cast<uint32_t(*)>(&Tbus_rspec_base->intr_en0_1), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_0", { const_cast<uint32_t(*)>(&Tbus_rspec_base->intr_en1_0), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_1", { const_cast<uint32_t(*)>(&Tbus_rspec_base->intr_en1_1), 0, {}, sizeof(uint32_t) } },
    { "intr_en2_0", { const_cast<uint32_t(*)>(&Tbus_rspec_base->intr_en2_0), 0, {}, sizeof(uint32_t) } },
    { "intr_en2_1", { const_cast<uint32_t(*)>(&Tbus_rspec_base->intr_en2_1), 0, {}, sizeof(uint32_t) } },
    { "freeze_en0", { const_cast<uint32_t(*)>(&Tbus_rspec_base->freeze_en0), 0, {}, sizeof(uint32_t) } },
    { "freeze_en1", { const_cast<uint32_t(*)>(&Tbus_rspec_base->freeze_en1), 0, {}, sizeof(uint32_t) } },
    { "freeze_en2", { const_cast<uint32_t(*)>(&Tbus_rspec_base->freeze_en2), 0, {}, sizeof(uint32_t) } },
    { "intr_inj", { const_cast<uint32_t(*)>(&Tbus_rspec_base->intr_inj), 0, {}, sizeof(uint32_t) } },
    { "tx_dr_rd_err_log", { const_cast<uint32_t(*)[0x4]>(&Tbus_rspec_base->tx_dr_rd_err_log), 0, {0x4}, sizeof(uint32_t) } },
    { "fm_dr_rd_err_log", { const_cast<uint32_t(*)[0x8]>(&Tbus_rspec_base->fm_dr_rd_err_log), 0, {0x8}, sizeof(uint32_t) } },
    { "controller_mbe_log", { const_cast<uint32_t(*)>(&Tbus_rspec_base->controller_mbe_log), 0, {}, sizeof(uint32_t) } },
    { "controller_sbe_log", { const_cast<uint32_t(*)>(&Tbus_rspec_base->controller_sbe_log), 0, {}, sizeof(uint32_t) } },
    { "host_slave_credit", { const_cast<uint32_t(*)>(&Tbus_rspec_base->host_slave_credit), 0, {}, sizeof(uint32_t) } },
    { "dma_log", { const_cast<uint32_t(*)>(&Tbus_rspec_base->dma_log), 0, {}, sizeof(uint32_t) } },
    { "mst_ctrl_log", { const_cast<uint32_t(*)>(&Tbus_rspec_base->mst_ctrl_log), 0, {}, sizeof(uint32_t) } },
    { "credit_ctrl", { const_cast<uint32_t(*)>(&Tbus_rspec_base->credit_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tbc_rspec_map: public RegisterMapper {
  static constexpr PTR_Tbc_rspec Tbc_rspec_base=0;
  Tbc_rspec_map() : RegisterMapper( {
    { "tbc_tbus", { &Tbc_rspec_base->tbc_tbus, new Tbus_rspec_map, {}, sizeof(Tbus_rspec) } },
    { "tbc_tx_dr", { &Tbc_rspec_base->tbc_tx_dr, new Dru_rspec_map, {0x4}, sizeof(Dru_rspec) } },
    { "tbc_cpl_dr", { &Tbc_rspec_base->tbc_cpl_dr, new Dru_rspec_map, {0x4}, sizeof(Dru_rspec) } },
    { "tbc_fm_dr", { &Tbc_rspec_base->tbc_fm_dr, new Dru_rspec_map, {0x8}, sizeof(Dru_rspec) } },
    { "tbc_rx_dr", { &Tbc_rspec_base->tbc_rx_dr, new Dru_rspec_map, {0x8}, sizeof(Dru_rspec) } }
    } )
  {}
};

struct Lfltr_hash_array_map: public RegisterMapper {
  static constexpr PTR_Lfltr_hash_array Lfltr_hash_array_base=0;
  Lfltr_hash_array_map() : RegisterMapper( {
    { "hash_array_0_12", { const_cast<uint32_t(*)>(&Lfltr_hash_array_base->hash_array_0_12), 0, {}, sizeof(uint32_t) } },
    { "hash_array_1_12", { const_cast<uint32_t(*)>(&Lfltr_hash_array_base->hash_array_1_12), 0, {}, sizeof(uint32_t) } },
    { "hash_array_2_12", { const_cast<uint32_t(*)>(&Lfltr_hash_array_base->hash_array_2_12), 0, {}, sizeof(uint32_t) } },
    { "hash_array_3_12", { const_cast<uint32_t(*)>(&Lfltr_hash_array_base->hash_array_3_12), 0, {}, sizeof(uint32_t) } },
    { "hash_array_4_12", { const_cast<uint32_t(*)>(&Lfltr_hash_array_base->hash_array_4_12), 0, {}, sizeof(uint32_t) } },
    { "hash_array_5_12", { const_cast<uint32_t(*)>(&Lfltr_hash_array_base->hash_array_5_12), 0, {}, sizeof(uint32_t) } },
    { "hash_array_6_12", { const_cast<uint32_t(*)>(&Lfltr_hash_array_base->hash_array_6_12), 0, {}, sizeof(uint32_t) } },
    { "hash_array_7_12", { const_cast<uint32_t(*)>(&Lfltr_hash_array_base->hash_array_7_12), 0, {}, sizeof(uint32_t) } },
    { "hash_array_8_12", { const_cast<uint32_t(*)>(&Lfltr_hash_array_base->hash_array_8_12), 0, {}, sizeof(uint32_t) } },
    { "hash_array_9_12", { const_cast<uint32_t(*)>(&Lfltr_hash_array_base->hash_array_9_12), 0, {}, sizeof(uint32_t) } },
    { "hash_array_10_12", { const_cast<uint32_t(*)>(&Lfltr_hash_array_base->hash_array_10_12), 0, {}, sizeof(uint32_t) } },
    { "hash_array_11_12", { const_cast<uint32_t(*)>(&Lfltr_hash_array_base->hash_array_11_12), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Lfltr_hash_rspec_map: public RegisterMapper {
  static constexpr PTR_Lfltr_hash_rspec Lfltr_hash_rspec_base=0;
  Lfltr_hash_rspec_map() : RegisterMapper( {
    { "hash_array", { const_cast<Lfltr_hash_array(*)[0x10]>(&Lfltr_hash_rspec_base->hash_array), new Lfltr_hash_array_map, {0x10}, sizeof(Lfltr_hash_array) } }
    } )
  {}
};

struct Lfltr_ctr48_lq_sop_in_map: public RegisterMapper {
  static constexpr PTR_Lfltr_ctr48_lq_sop_in Lfltr_ctr48_lq_sop_in_base=0;
  Lfltr_ctr48_lq_sop_in_map() : RegisterMapper( {
    { "lq_sop_in_0_2", { const_cast<uint32_t(*)>(&Lfltr_ctr48_lq_sop_in_base->lq_sop_in_0_2), 0, {}, sizeof(uint32_t) } },
    { "lq_sop_in_1_2", { const_cast<uint32_t(*)>(&Lfltr_ctr48_lq_sop_in_base->lq_sop_in_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Lfltr_ctr48_lq_eop_in_err_map: public RegisterMapper {
  static constexpr PTR_Lfltr_ctr48_lq_eop_in_err Lfltr_ctr48_lq_eop_in_err_base=0;
  Lfltr_ctr48_lq_eop_in_err_map() : RegisterMapper( {
    { "lq_eop_in_err_0_2", { const_cast<uint32_t(*)>(&Lfltr_ctr48_lq_eop_in_err_base->lq_eop_in_err_0_2), 0, {}, sizeof(uint32_t) } },
    { "lq_eop_in_err_1_2", { const_cast<uint32_t(*)>(&Lfltr_ctr48_lq_eop_in_err_base->lq_eop_in_err_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Lfltr_ctr48_lq_in_map: public RegisterMapper {
  static constexpr PTR_Lfltr_ctr48_lq_in Lfltr_ctr48_lq_in_base=0;
  Lfltr_ctr48_lq_in_map() : RegisterMapper( {
    { "lq_in_0_2", { const_cast<uint32_t(*)>(&Lfltr_ctr48_lq_in_base->lq_in_0_2), 0, {}, sizeof(uint32_t) } },
    { "lq_in_1_2", { const_cast<uint32_t(*)>(&Lfltr_ctr48_lq_in_base->lq_in_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Lfltr_ctr48_lq_dropped_state_map: public RegisterMapper {
  static constexpr PTR_Lfltr_ctr48_lq_dropped_state Lfltr_ctr48_lq_dropped_state_base=0;
  Lfltr_ctr48_lq_dropped_state_map() : RegisterMapper( {
    { "lq_dropped_state_0_2", { const_cast<uint32_t(*)>(&Lfltr_ctr48_lq_dropped_state_base->lq_dropped_state_0_2), 0, {}, sizeof(uint32_t) } },
    { "lq_dropped_state_1_2", { const_cast<uint32_t(*)>(&Lfltr_ctr48_lq_dropped_state_base->lq_dropped_state_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Lfltr_ctr48_lq_dropped_learned_map: public RegisterMapper {
  static constexpr PTR_Lfltr_ctr48_lq_dropped_learned Lfltr_ctr48_lq_dropped_learned_base=0;
  Lfltr_ctr48_lq_dropped_learned_map() : RegisterMapper( {
    { "lq_dropped_learned_0_2", { const_cast<uint32_t(*)>(&Lfltr_ctr48_lq_dropped_learned_base->lq_dropped_learned_0_2), 0, {}, sizeof(uint32_t) } },
    { "lq_dropped_learned_1_2", { const_cast<uint32_t(*)>(&Lfltr_ctr48_lq_dropped_learned_base->lq_dropped_learned_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Lfltr_ctr48_lq_learned_map: public RegisterMapper {
  static constexpr PTR_Lfltr_ctr48_lq_learned Lfltr_ctr48_lq_learned_base=0;
  Lfltr_ctr48_lq_learned_map() : RegisterMapper( {
    { "lq_learned_0_2", { const_cast<uint32_t(*)>(&Lfltr_ctr48_lq_learned_base->lq_learned_0_2), 0, {}, sizeof(uint32_t) } },
    { "lq_learned_1_2", { const_cast<uint32_t(*)>(&Lfltr_ctr48_lq_learned_base->lq_learned_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Lfltr_ctrl_rspec_map: public RegisterMapper {
  static constexpr PTR_Lfltr_ctrl_rspec Lfltr_ctrl_rspec_base=0;
  Lfltr_ctrl_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)[0x4]>(&Lfltr_ctrl_rspec_base->scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "common_ctrl", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->common_ctrl), 0, {}, sizeof(uint32_t) } },
    { "lqt_timeout", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->lqt_timeout), 0, {}, sizeof(uint32_t) } },
    { "bft_ctrl", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->bft_ctrl), 0, {}, sizeof(uint32_t) } },
    { "hash_seed", { const_cast<uint32_t(*)[0x4]>(&Lfltr_ctrl_rspec_base->hash_seed), 0, {0x4}, sizeof(uint32_t) } },
    { "intr_stat", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->intr_stat), 0, {}, sizeof(uint32_t) } },
    { "intr_en0", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->intr_en0), 0, {}, sizeof(uint32_t) } },
    { "intr_en1", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->intr_en1), 0, {}, sizeof(uint32_t) } },
    { "freeze_en", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->freeze_en), 0, {}, sizeof(uint32_t) } },
    { "intr_inj", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->intr_inj), 0, {}, sizeof(uint32_t) } },
    { "lq_sop_in", { const_cast<Lfltr_ctr48_lq_sop_in(*)>(&Lfltr_ctrl_rspec_base->lq_sop_in), new Lfltr_ctr48_lq_sop_in_map, {}, sizeof(Lfltr_ctr48_lq_sop_in) } },
    { "lq_eop_in_err", { const_cast<Lfltr_ctr48_lq_eop_in_err(*)>(&Lfltr_ctrl_rspec_base->lq_eop_in_err), new Lfltr_ctr48_lq_eop_in_err_map, {}, sizeof(Lfltr_ctr48_lq_eop_in_err) } },
    { "lq_in", { const_cast<Lfltr_ctr48_lq_in(*)>(&Lfltr_ctrl_rspec_base->lq_in), new Lfltr_ctr48_lq_in_map, {}, sizeof(Lfltr_ctr48_lq_in) } },
    { "lq_dropped_state", { const_cast<Lfltr_ctr48_lq_dropped_state(*)>(&Lfltr_ctrl_rspec_base->lq_dropped_state), new Lfltr_ctr48_lq_dropped_state_map, {}, sizeof(Lfltr_ctr48_lq_dropped_state) } },
    { "lq_dropped_learned", { const_cast<Lfltr_ctr48_lq_dropped_learned(*)>(&Lfltr_ctrl_rspec_base->lq_dropped_learned), new Lfltr_ctr48_lq_dropped_learned_map, {}, sizeof(Lfltr_ctr48_lq_dropped_learned) } },
    { "lq_learned", { const_cast<Lfltr_ctr48_lq_learned(*)>(&Lfltr_ctrl_rspec_base->lq_learned), new Lfltr_ctr48_lq_learned_map, {}, sizeof(Lfltr_ctr48_lq_learned) } },
    { "pbe_log", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->pbe_log), 0, {}, sizeof(uint32_t) } },
    { "sbe_log", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->sbe_log), 0, {}, sizeof(uint32_t) } },
    { "mbe_log", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->mbe_log), 0, {}, sizeof(uint32_t) } },
    { "bft_state", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->bft_state), 0, {}, sizeof(uint32_t) } },
    { "lqt_state", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->lqt_state), 0, {}, sizeof(uint32_t) } },
    { "creq_state", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->creq_state), 0, {}, sizeof(uint32_t) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Lfltr_rspec_map: public RegisterMapper {
  static constexpr PTR_Lfltr_rspec Lfltr_rspec_base=0;
  Lfltr_rspec_map() : RegisterMapper( {
    { "hash", { &Lfltr_rspec_base->hash, new Lfltr_hash_rspec_map, {0x4}, sizeof(Lfltr_hash_rspec) } },
    { "ctrl", { &Lfltr_rspec_base->ctrl, new Lfltr_ctrl_rspec_map, {}, sizeof(Lfltr_ctrl_rspec) } }
    } )
  {}
};

struct Cnt48_inc_map: public RegisterMapper {
  static constexpr PTR_Cnt48_inc Cnt48_inc_base=0;
  Cnt48_inc_map() : RegisterMapper( {
    { "ctr_ctrl_pkt_0_2", { const_cast<uint32_t(*)>(&Cnt48_inc_base->ctr_ctrl_pkt_0_2), 0, {}, sizeof(uint32_t) } },
    { "ctr_ctrl_pkt_1_2", { const_cast<uint32_t(*)>(&Cnt48_inc_base->ctr_ctrl_pkt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_port_ct_dis_map: public RegisterMapper {
  static constexpr PTR_Wac_port_ct_dis Wac_port_ct_dis_base=0;
  Wac_port_ct_dis_map() : RegisterMapper( {
    { "wac_port_ct_dis_0_3", { const_cast<uint32_t(*)>(&Wac_port_ct_dis_base->wac_port_ct_dis_0_3), 0, {}, sizeof(uint32_t) } },
    { "wac_port_ct_dis_1_3", { const_cast<uint32_t(*)>(&Wac_port_ct_dis_base->wac_port_ct_dis_1_3), 0, {}, sizeof(uint32_t) } },
    { "wac_port_ct_dis_2_3", { const_cast<uint32_t(*)>(&Wac_port_ct_dis_base->wac_port_ct_dis_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pipe_reg_intr_map: public RegisterMapper {
  static constexpr PTR_Pipe_reg_intr Pipe_reg_intr_base=0;
  Pipe_reg_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Pipe_reg_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Pipe_reg_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Pipe_reg_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Pipe_reg_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Pipe_reg_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pipe_reg_map: public RegisterMapper {
  static constexpr PTR_Pipe_reg Pipe_reg_base=0;
  Pipe_reg_map() : RegisterMapper( {
    { "wac_pipe_ctrl", { const_cast<uint32_t(*)>(&Pipe_reg_base->wac_pipe_ctrl), 0, {}, sizeof(uint32_t) } },
    { "wac_pipe_mac_spd", { const_cast<uint32_t(*)[0x9]>(&Pipe_reg_base->wac_pipe_mac_spd), 0, {0x9}, sizeof(uint32_t) } },
    { "wac_bypass_config", { const_cast<uint32_t(*)>(&Pipe_reg_base->wac_bypass_config), 0, {}, sizeof(uint32_t) } },
    { "ctr_drop_in_err", { const_cast<uint32_t(*)>(&Pipe_reg_base->ctr_drop_in_err), 0, {}, sizeof(uint32_t) } },
    { "ctr_drop_fuse_pipe", { const_cast<uint32_t(*)>(&Pipe_reg_base->ctr_drop_fuse_pipe), 0, {}, sizeof(uint32_t) } },
    { "ctr_drop_no_dst", { const_cast<uint32_t(*)>(&Pipe_reg_base->ctr_drop_no_dst), 0, {}, sizeof(uint32_t) } },
    { "ap_red_drop", { const_cast<uint32_t(*)[0x4]>(&Pipe_reg_base->ap_red_drop), 0, {0x4}, sizeof(uint32_t) } },
    { "ap_yel_drop", { const_cast<uint32_t(*)[0x4]>(&Pipe_reg_base->ap_yel_drop), 0, {0x4}, sizeof(uint32_t) } },
    { "ap_green_drop", { const_cast<uint32_t(*)[0x4]>(&Pipe_reg_base->ap_green_drop), 0, {0x4}, sizeof(uint32_t) } },
    { "ctr_drop_sop_by_sop", { const_cast<uint32_t(*)>(&Pipe_reg_base->ctr_drop_sop_by_sop), 0, {}, sizeof(uint32_t) } },
    { "ctr_caa_full", { const_cast<uint32_t(*)>(&Pipe_reg_base->ctr_caa_full), 0, {}, sizeof(uint32_t) } },
    { "ctr_ctrl_pkt", { const_cast<Cnt48_inc(*)>(&Pipe_reg_base->ctr_ctrl_pkt), new Cnt48_inc_map, {}, sizeof(Cnt48_inc) } },
    { "ctr_vld_sop", { const_cast<Cnt48_inc(*)>(&Pipe_reg_base->ctr_vld_sop), new Cnt48_inc_map, {}, sizeof(Cnt48_inc) } },
    { "wac_drop_buf_full", { const_cast<Cnt48_inc(*)>(&Pipe_reg_base->wac_drop_buf_full), new Cnt48_inc_map, {}, sizeof(Cnt48_inc) } },
    { "wac_drop_psc_full", { const_cast<Cnt48_inc(*)>(&Pipe_reg_base->wac_drop_psc_full), new Cnt48_inc_map, {}, sizeof(Cnt48_inc) } },
    { "wac_inerror_drop", { const_cast<Cnt48_inc(*)>(&Pipe_reg_base->wac_inerror_drop), new Cnt48_inc_map, {}, sizeof(Cnt48_inc) } },
    { "wac_inerror_skip_drop", { const_cast<Cnt48_inc(*)>(&Pipe_reg_base->wac_inerror_skip_drop), new Cnt48_inc_map, {}, sizeof(Cnt48_inc) } },
    { "wac_iport_sop_cnt", { const_cast<uint32_t(*)[0x49]>(&Pipe_reg_base->wac_iport_sop_cnt), 0, {0x49}, sizeof(uint32_t) } },
    { "wac_debug_register", { const_cast<uint32_t(*)>(&Pipe_reg_base->wac_debug_register), 0, {}, sizeof(uint32_t) } },
    { "offset_profile", { const_cast<uint32_t(*)[0x20]>(&Pipe_reg_base->offset_profile), 0, {0x20}, sizeof(uint32_t) } },
    { "port_config", { const_cast<uint32_t(*)[0x49]>(&Pipe_reg_base->port_config), 0, {0x49}, sizeof(uint32_t) } },
    { "wac_rm_pfc_state", { const_cast<uint32_t(*)[0x48]>(&Pipe_reg_base->wac_rm_pfc_state), 0, {0x48}, sizeof(uint32_t) } },
    { "wac_port_ct_dis", { const_cast<Wac_port_ct_dis(*)>(&Pipe_reg_base->wac_port_ct_dis), new Wac_port_ct_dis_map, {}, sizeof(Wac_port_ct_dis) } },
    { "wac_ecc", { const_cast<uint32_t(*)>(&Pipe_reg_base->wac_ecc), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Pipe_reg_base->intr, new Pipe_reg_intr_map, {}, sizeof(Pipe_reg_intr) } },
    { "ppg_mapping_table_sbe_err_log", { const_cast<uint32_t(*)>(&Pipe_reg_base->ppg_mapping_table_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ppg_mapping_table_mbe_err_log", { const_cast<uint32_t(*)>(&Pipe_reg_base->ppg_mapping_table_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "drop_cnt_table_sbe_err_log", { const_cast<uint32_t(*)>(&Pipe_reg_base->drop_cnt_table_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "drop_cnt_table_mbe_err_log", { const_cast<uint32_t(*)>(&Pipe_reg_base->drop_cnt_table_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "qid_map_sbe_err_log", { const_cast<uint32_t(*)>(&Pipe_reg_base->qid_map_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "qid_map_mbe_err_log", { const_cast<uint32_t(*)>(&Pipe_reg_base->qid_map_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "wac_pre_fifo_mapping", { const_cast<uint32_t(*)[0x8]>(&Pipe_reg_base->wac_pre_fifo_mapping), 0, {0x8}, sizeof(uint32_t) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Pipe_reg_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_wac_pipe_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_wac_pipe_rspec Tm_wac_pipe_rspec_base=0;
  Tm_wac_pipe_rspec_map() : RegisterMapper( {
    { "wac_reg", { &Tm_wac_pipe_rspec_base->wac_reg, new Pipe_reg_map, {}, sizeof(Pipe_reg) } }
    } )
  {}
};

struct Wac_cnt48_map: public RegisterMapper {
  static constexpr PTR_Wac_cnt48 Wac_cnt48_base=0;
  Wac_cnt48_map() : RegisterMapper( {
    { "wac_dod_cnt48_0_2", { const_cast<uint32_t(*)>(&Wac_cnt48_base->wac_dod_cnt48_0_2), 0, {}, sizeof(uint32_t) } },
    { "wac_dod_cnt48_1_2", { const_cast<uint32_t(*)>(&Wac_cnt48_base->wac_dod_cnt48_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_common_block_map: public RegisterMapper {
  static constexpr PTR_Wac_common_block Wac_common_block_base=0;
  Wac_common_block_map() : RegisterMapper( {
    { "wac_scratch", { const_cast<uint32_t(*)[0x4]>(&Wac_common_block_base->wac_scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Wac_common_block_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "wac_glb_config", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_glb_config), 0, {}, sizeof(uint32_t) } },
    { "wac_qac_afull_th", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_qac_afull_th), 0, {}, sizeof(uint32_t) } },
    { "wac_mem_init_done", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_mem_init_done), 0, {}, sizeof(uint32_t) } },
    { "wac_ap_limit_cell", { const_cast<uint32_t(*)[0x4]>(&Wac_common_block_base->wac_ap_limit_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "wac_ap_offset_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_ap_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_ap_red_limit_cell", { const_cast<uint32_t(*)[0x4]>(&Wac_common_block_base->wac_ap_red_limit_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "wac_ap_red_offset_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_ap_red_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_ap_yel_limit_cell", { const_cast<uint32_t(*)[0x4]>(&Wac_common_block_base->wac_ap_yel_limit_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "wac_ap_yel_offset_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_ap_yel_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_dod_limit_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_dod_limit_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_glb_min_limit_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_glb_min_limit_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_caa_block_hdr_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_caa_block_hdr_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_caa_rsvd_blocks", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_caa_rsvd_blocks), 0, {}, sizeof(uint32_t) } },
    { "wac_glb_cell_limit", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_glb_cell_limit), 0, {}, sizeof(uint32_t) } },
    { "wac_ap_cnt_cell", { const_cast<uint32_t(*)[0x4]>(&Wac_common_block_base->wac_ap_cnt_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "wac_wm_ap_cnt_cell", { const_cast<uint32_t(*)[0x4]>(&Wac_common_block_base->wac_wm_ap_cnt_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "wac_wm_tot_cnt_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_wm_tot_cnt_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_hdr_cnt_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_hdr_cnt_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_wm_hdr_cnt_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_wm_hdr_cnt_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_hdr_limit_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_hdr_limit_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_hdr_offset_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_hdr_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_dod_cnt_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_dod_cnt_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_pfc_pool_0_limit_cell", { const_cast<uint32_t(*)[0x8]>(&Wac_common_block_base->wac_pfc_pool_0_limit_cell), 0, {0x8}, sizeof(uint32_t) } },
    { "wac_pfc_pool_1_limit_cell", { const_cast<uint32_t(*)[0x8]>(&Wac_common_block_base->wac_pfc_pool_1_limit_cell), 0, {0x8}, sizeof(uint32_t) } },
    { "wac_pfc_pool_2_limit_cell", { const_cast<uint32_t(*)[0x8]>(&Wac_common_block_base->wac_pfc_pool_2_limit_cell), 0, {0x8}, sizeof(uint32_t) } },
    { "wac_pfc_pool_3_limit_cell", { const_cast<uint32_t(*)[0x8]>(&Wac_common_block_base->wac_pfc_pool_3_limit_cell), 0, {0x8}, sizeof(uint32_t) } },
    { "wac_dod_cnt48", { const_cast<Wac_cnt48(*)>(&Wac_common_block_base->wac_dod_cnt48), new Wac_cnt48_map, {}, sizeof(Wac_cnt48) } },
    { "wac_dod_drop_cnt48", { const_cast<Wac_cnt48(*)>(&Wac_common_block_base->wac_dod_drop_cnt48), new Wac_cnt48_map, {}, sizeof(Wac_cnt48) } },
    { "wac_sop_cred_rtn_cnt", { const_cast<Wac_cnt48(*)[0x10]>(&Wac_common_block_base->wac_sop_cred_rtn_cnt), new Wac_cnt48_map, {0x10}, sizeof(Wac_cnt48) } },
    { "wac_pfc_pool_step_timer", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_pfc_pool_step_timer), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_common_block_drop_st_map: public RegisterMapper {
  static constexpr PTR_Wac_common_block_drop_st Wac_common_block_drop_st_base=0;
  Wac_common_block_drop_st_map() : RegisterMapper( {
    { "drop_state_cell", { const_cast<uint32_t(*)[0x4]>(&Wac_common_block_drop_st_base->drop_state_cell), 0, {0x4}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_wac_common_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_wac_common_rspec Tm_wac_common_rspec_base=0;
  Tm_wac_common_rspec_map() : RegisterMapper( {
    { "wac_common", { &Tm_wac_common_rspec_base->wac_common, new Wac_common_block_map, {}, sizeof(Wac_common_block) } },
    { "wac_common_block_drop_st", { &Tm_wac_common_rspec_base->wac_common_block_drop_st, new Wac_common_block_drop_st_map, {}, sizeof(Wac_common_block_drop_st) } }
    } )
  {}
};

struct Tm_wac_top_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_wac_top_rspec Tm_wac_top_rspec_base=0;
  Tm_wac_top_rspec_map() : RegisterMapper( {
    { "wac_pipe", { &Tm_wac_top_rspec_base->wac_pipe, new Tm_wac_pipe_rspec_map, {0x4}, sizeof(Tm_wac_pipe_rspec) } },
    { "wac_common", { &Tm_wac_top_rspec_base->wac_common, new Tm_wac_common_rspec_map, {}, sizeof(Tm_wac_common_rspec) } }
    } )
  {}
};

struct Caa_block_valid_r_map: public RegisterMapper {
  static constexpr PTR_Caa_block_valid_r Caa_block_valid_r_base=0;
  Caa_block_valid_r_map() : RegisterMapper( {
    { "block_valid_0_6", { const_cast<uint32_t(*)>(&Caa_block_valid_r_base->block_valid_0_6), 0, {}, sizeof(uint32_t) } },
    { "block_valid_1_6", { const_cast<uint32_t(*)>(&Caa_block_valid_r_base->block_valid_1_6), 0, {}, sizeof(uint32_t) } },
    { "block_valid_2_6", { const_cast<uint32_t(*)>(&Caa_block_valid_r_base->block_valid_2_6), 0, {}, sizeof(uint32_t) } },
    { "block_valid_3_6", { const_cast<uint32_t(*)>(&Caa_block_valid_r_base->block_valid_3_6), 0, {}, sizeof(uint32_t) } },
    { "block_valid_4_6", { const_cast<uint32_t(*)>(&Caa_block_valid_r_base->block_valid_4_6), 0, {}, sizeof(uint32_t) } },
    { "block_valid_5_6", { const_cast<uint32_t(*)>(&Caa_block_valid_r_base->block_valid_5_6), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Caa_block_reset_r_map: public RegisterMapper {
  static constexpr PTR_Caa_block_reset_r Caa_block_reset_r_base=0;
  Caa_block_reset_r_map() : RegisterMapper( {
    { "block_reset_0_6", { const_cast<uint32_t(*)>(&Caa_block_reset_r_base->block_reset_0_6), 0, {}, sizeof(uint32_t) } },
    { "block_reset_1_6", { const_cast<uint32_t(*)>(&Caa_block_reset_r_base->block_reset_1_6), 0, {}, sizeof(uint32_t) } },
    { "block_reset_2_6", { const_cast<uint32_t(*)>(&Caa_block_reset_r_base->block_reset_2_6), 0, {}, sizeof(uint32_t) } },
    { "block_reset_3_6", { const_cast<uint32_t(*)>(&Caa_block_reset_r_base->block_reset_3_6), 0, {}, sizeof(uint32_t) } },
    { "block_reset_4_6", { const_cast<uint32_t(*)>(&Caa_block_reset_r_base->block_reset_4_6), 0, {}, sizeof(uint32_t) } },
    { "block_reset_5_6", { const_cast<uint32_t(*)>(&Caa_block_reset_r_base->block_reset_5_6), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Caa_block_enable_r_map: public RegisterMapper {
  static constexpr PTR_Caa_block_enable_r Caa_block_enable_r_base=0;
  Caa_block_enable_r_map() : RegisterMapper( {
    { "block_enable_0_6", { const_cast<uint32_t(*)>(&Caa_block_enable_r_base->block_enable_0_6), 0, {}, sizeof(uint32_t) } },
    { "block_enable_1_6", { const_cast<uint32_t(*)>(&Caa_block_enable_r_base->block_enable_1_6), 0, {}, sizeof(uint32_t) } },
    { "block_enable_2_6", { const_cast<uint32_t(*)>(&Caa_block_enable_r_base->block_enable_2_6), 0, {}, sizeof(uint32_t) } },
    { "block_enable_3_6", { const_cast<uint32_t(*)>(&Caa_block_enable_r_base->block_enable_3_6), 0, {}, sizeof(uint32_t) } },
    { "block_enable_4_6", { const_cast<uint32_t(*)>(&Caa_block_enable_r_base->block_enable_4_6), 0, {}, sizeof(uint32_t) } },
    { "block_enable_5_6", { const_cast<uint32_t(*)>(&Caa_block_enable_r_base->block_enable_5_6), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Caa_block_ready_r_map: public RegisterMapper {
  static constexpr PTR_Caa_block_ready_r Caa_block_ready_r_base=0;
  Caa_block_ready_r_map() : RegisterMapper( {
    { "block_ready_0_6", { const_cast<uint32_t(*)>(&Caa_block_ready_r_base->block_ready_0_6), 0, {}, sizeof(uint32_t) } },
    { "block_ready_1_6", { const_cast<uint32_t(*)>(&Caa_block_ready_r_base->block_ready_1_6), 0, {}, sizeof(uint32_t) } },
    { "block_ready_2_6", { const_cast<uint32_t(*)>(&Caa_block_ready_r_base->block_ready_2_6), 0, {}, sizeof(uint32_t) } },
    { "block_ready_3_6", { const_cast<uint32_t(*)>(&Caa_block_ready_r_base->block_ready_3_6), 0, {}, sizeof(uint32_t) } },
    { "block_ready_4_6", { const_cast<uint32_t(*)>(&Caa_block_ready_r_base->block_ready_4_6), 0, {}, sizeof(uint32_t) } },
    { "block_ready_5_6", { const_cast<uint32_t(*)>(&Caa_block_ready_r_base->block_ready_5_6), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Caa_epipe_pkt_dropcnt_r_map: public RegisterMapper {
  static constexpr PTR_Caa_epipe_pkt_dropcnt_r Caa_epipe_pkt_dropcnt_r_base=0;
  Caa_epipe_pkt_dropcnt_r_map() : RegisterMapper( {
    { "pkt_dropcnt_0_2", { const_cast<uint32_t(*)>(&Caa_epipe_pkt_dropcnt_r_base->pkt_dropcnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "pkt_dropcnt_1_2", { const_cast<uint32_t(*)>(&Caa_epipe_pkt_dropcnt_r_base->pkt_dropcnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Caa_epipe_g_map: public RegisterMapper {
  static constexpr PTR_Caa_epipe_g Caa_epipe_g_base=0;
  Caa_epipe_g_map() : RegisterMapper( {
    { "enable", { const_cast<uint32_t(*)>(&Caa_epipe_g_base->enable), 0, {}, sizeof(uint32_t) } },
    { "blks_usecnt", { const_cast<uint32_t(*)>(&Caa_epipe_g_base->blks_usecnt), 0, {}, sizeof(uint32_t) } },
    { "blks_max_usecnt", { const_cast<uint32_t(*)>(&Caa_epipe_g_base->blks_max_usecnt), 0, {}, sizeof(uint32_t) } },
    { "pkt_dropcnt", { const_cast<Caa_epipe_pkt_dropcnt_r(*)>(&Caa_epipe_g_base->pkt_dropcnt), new Caa_epipe_pkt_dropcnt_r_map, {}, sizeof(Caa_epipe_pkt_dropcnt_r) } }
    } )
  {}
};

struct Caa_block_g_map: public RegisterMapper {
  static constexpr PTR_Caa_block_g Caa_block_g_base=0;
  Caa_block_g_map() : RegisterMapper( {
    { "state", { const_cast<uint32_t(*)>(&Caa_block_g_base->state), 0, {}, sizeof(uint32_t) } },
    { "addr_usecnt", { const_cast<uint32_t(*)>(&Caa_block_g_base->addr_usecnt), 0, {}, sizeof(uint32_t) } },
    { "addr_max_usecnt", { const_cast<uint32_t(*)>(&Caa_block_g_base->addr_max_usecnt), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_caa_top_rspec_ecc_map: public RegisterMapper {
  static constexpr PTR_Tm_caa_top_rspec_ecc Tm_caa_top_rspec_ecc_base=0;
  Tm_caa_top_rspec_ecc_map() : RegisterMapper( {
    { "ecc_0_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_0_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_1_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_1_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_2_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_2_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_3_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_3_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_4_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_4_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_5_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_5_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_6_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_6_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_7_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_7_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_8_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_8_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_9_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_9_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_10_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_10_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_11_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_11_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_12_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_12_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_13_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_13_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_14_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_14_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_15_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_15_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_16_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_16_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_17_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_17_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_18_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_18_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_19_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_19_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_20_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_20_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_21_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_21_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_22_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_22_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_23_24", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_ecc_base->ecc_23_24), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_caa_top_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_caa_top_rspec_intr Tm_caa_top_rspec_intr_base=0;
  Tm_caa_top_rspec_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Caa_cdm_indir_access_data_r_map: public RegisterMapper {
  static constexpr PTR_Caa_cdm_indir_access_data_r Caa_cdm_indir_access_data_r_base=0;
  Caa_cdm_indir_access_data_r_map() : RegisterMapper( {
    { "cdm_indir_access_data_0_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_0_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_1_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_1_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_2_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_2_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_3_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_3_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_4_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_4_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_5_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_5_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_6_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_6_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_7_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_7_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_8_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_8_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_9_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_9_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_10_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_10_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_11_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_11_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_12_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_12_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_13_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_13_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_14_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_14_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_15_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_15_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_16_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_16_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_17_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_17_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_18_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_18_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_19_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_19_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_20_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_20_22), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_21_22", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_21_22), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_caa_cdm_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_caa_cdm_rspec Tm_caa_cdm_rspec_base=0;
  Tm_caa_cdm_rspec_map() : RegisterMapper( {
    { "dft_csr", { const_cast<uint32_t(*)>(&Tm_caa_cdm_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_addr", { const_cast<uint32_t(*)>(&Tm_caa_cdm_rspec_base->cdm_indir_access_addr), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data", { const_cast<Caa_cdm_indir_access_data_r(*)>(&Tm_caa_cdm_rspec_base->cdm_indir_access_data), new Caa_cdm_indir_access_data_r_map, {}, sizeof(Caa_cdm_indir_access_data_r) } }
    } )
  {}
};

struct Tm_caa_top_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_caa_top_rspec Tm_caa_top_rspec_base=0;
  Tm_caa_top_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)[0x4]>(&Tm_caa_top_rspec_base->scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "cdm_org", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_base->cdm_org), 0, {}, sizeof(uint32_t) } },
    { "ctrl", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "full_threshold", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_base->full_threshold), 0, {}, sizeof(uint32_t) } },
    { "hyst_threshold", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_base->hyst_threshold), 0, {}, sizeof(uint32_t) } },
    { "almost_full_threshold", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_base->almost_full_threshold), 0, {}, sizeof(uint32_t) } },
    { "block_valid", { const_cast<Caa_block_valid_r(*)>(&Tm_caa_top_rspec_base->block_valid), new Caa_block_valid_r_map, {}, sizeof(Caa_block_valid_r) } },
    { "block_reset", { const_cast<Caa_block_reset_r(*)>(&Tm_caa_top_rspec_base->block_reset), new Caa_block_reset_r_map, {}, sizeof(Caa_block_reset_r) } },
    { "block_enable", { const_cast<Caa_block_enable_r(*)>(&Tm_caa_top_rspec_base->block_enable), new Caa_block_enable_r_map, {}, sizeof(Caa_block_enable_r) } },
    { "block_ready", { const_cast<Caa_block_ready_r(*)>(&Tm_caa_top_rspec_base->block_ready), new Caa_block_ready_r_map, {}, sizeof(Caa_block_ready_r) } },
    { "blocks_freecnt", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_base->blocks_freecnt), 0, {}, sizeof(uint32_t) } },
    { "epipe", { &Tm_caa_top_rspec_base->epipe, new Caa_epipe_g_map, {0x10}, sizeof(Caa_epipe_g) } },
    { "block", { &Tm_caa_top_rspec_base->block, new Caa_block_g_map, {0xc0}, sizeof(Caa_block_g) } },
    { "caa_bank_ctr", { const_cast<uint32_t(*)[0x180]>(&Tm_caa_top_rspec_base->caa_bank_ctr), 0, {0x180}, sizeof(uint32_t) } },
    { "lmem_indir_access_addr", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_base->lmem_indir_access_addr), 0, {}, sizeof(uint32_t) } },
    { "lmem_indir_access_data", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_base->lmem_indir_access_data), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<Tm_caa_top_rspec_ecc(*)>(&Tm_caa_top_rspec_base->ecc), new Tm_caa_top_rspec_ecc_map, {}, sizeof(Tm_caa_top_rspec_ecc) } },
    { "intr", { &Tm_caa_top_rspec_base->intr, new Tm_caa_top_rspec_intr_map, {}, sizeof(Tm_caa_top_rspec_intr) } },
    { "linkmem_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_base->linkmem_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "linkmem_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_base->linkmem_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "overflow_err_log", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_base->overflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "underflow_err_log", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_base->underflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Tm_caa_top_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } },
    { "cdm_lo", { &Tm_caa_top_rspec_base->cdm_lo, new Tm_caa_cdm_rspec_map, {}, sizeof(Tm_caa_cdm_rspec) } },
    { "cdm_hi", { &Tm_caa_top_rspec_base->cdm_hi, new Tm_caa_cdm_rspec_map, {}, sizeof(Tm_caa_cdm_rspec) } }
    } )
  {}
};

struct Qac_common_block_map: public RegisterMapper {
  static constexpr PTR_Qac_common_block Qac_common_block_base=0;
  Qac_common_block_map() : RegisterMapper( {
    { "dft_csr", { const_cast<uint32_t(*)>(&Qac_common_block_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "qac_glb_config", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_glb_config), 0, {}, sizeof(uint32_t) } },
    { "qac_dod_limit_cell", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_dod_limit_cell), 0, {}, sizeof(uint32_t) } },
    { "qac_mem_init_done", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_mem_init_done), 0, {}, sizeof(uint32_t) } },
    { "qac_mem_init_en", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_mem_init_en), 0, {}, sizeof(uint32_t) } },
    { "qac_dod_cnt_cell", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_dod_cnt_cell), 0, {}, sizeof(uint32_t) } },
    { "qac_ep_limit_cell", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_ep_limit_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_ep_resume_offset_cell", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_ep_resume_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "qac_discard_queue_throttleer", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_discard_queue_throttleer), 0, {}, sizeof(uint32_t) } },
    { "qac_uc_disc_disable", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_uc_disc_disable), 0, {}, sizeof(uint32_t) } },
    { "qac_intr_stat", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_intr_stat), 0, {}, sizeof(uint32_t) } },
    { "qac_glb_ap_gre_limit_cell", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_glb_ap_gre_limit_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_glb_ap_gre_resume_offset_cell", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_glb_ap_gre_resume_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "qac_glb_ap_red_limit_cell", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_glb_ap_red_limit_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_glb_ap_red_resume_offset_cell", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_glb_ap_red_resume_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "qac_glb_ap_yel_limit_cell", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_glb_ap_yel_limit_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_glb_ap_yel_resume_offset_cell", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_glb_ap_yel_resume_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "qac_ep_cnt_cell", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_ep_cnt_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_wm_ep_cnt_cell", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_wm_ep_cnt_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_ep_cnt_ph", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_ep_cnt_ph), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_glb_ap_cnt_ph", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_glb_ap_cnt_ph), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_wm_glb_ap_cnt_ph", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_wm_glb_ap_cnt_ph), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_glb_ap_cnt_cell", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_glb_ap_cnt_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_wm_glb_ap_cnt_cell", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_wm_glb_ap_cnt_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_cnt_pkt_pipe0", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_cnt_pkt_pipe0), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_cnt_cell_pipe0", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_cnt_cell_pipe0), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_cnt_pkt_pipe1", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_cnt_pkt_pipe1), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_cnt_cell_pipe1", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_cnt_cell_pipe1), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_cnt_pkt_pipe2", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_cnt_pkt_pipe2), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_cnt_cell_pipe2", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_cnt_cell_pipe2), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_cnt_pkt_pipe3", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_cnt_pkt_pipe3), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_cnt_cell_pipe3", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_cnt_cell_pipe3), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_offset_cell", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_pre_fifo_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "qac_pre_fifo_limit_shr_cell_pipe0", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_limit_shr_cell_pipe0), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_limit_shr_cell_pipe1", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_limit_shr_cell_pipe1), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_limit_shr_cell_pipe2", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_limit_shr_cell_pipe2), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_limit_shr_cell_pipe3", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_limit_shr_cell_pipe3), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_limit_min_cell_pipe0", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_limit_min_cell_pipe0), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_limit_min_cell_pipe1", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_limit_min_cell_pipe1), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_limit_min_cell_pipe2", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_limit_min_cell_pipe2), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_limit_min_cell_pipe3", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_limit_min_cell_pipe3), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_limit_pkt_pipe0", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_limit_pkt_pipe0), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_limit_pkt_pipe1", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_limit_pkt_pipe1), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_limit_pkt_pipe2", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_limit_pkt_pipe2), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_pre_fifo_limit_pkt_pipe3", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_pre_fifo_limit_pkt_pipe3), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_mcct_pkt_limit", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_mcct_pkt_limit), 0, {}, sizeof(uint32_t) } },
    { "qac_mcct_dis_pkt_hys", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_mcct_dis_pkt_hys), 0, {}, sizeof(uint32_t) } },
    { "qac_mcct_cnt_pkt", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_mcct_cnt_pkt), 0, {}, sizeof(uint32_t) } },
    { "qac_queue_parser_fifo_cred", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_queue_parser_fifo_cred), 0, {}, sizeof(uint32_t) } },
    { "qac_pipe_disable", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_pipe_disable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qac_common_block_drop_st_map: public RegisterMapper {
  static constexpr PTR_Qac_common_block_drop_st Qac_common_block_drop_st_base=0;
  Qac_common_block_drop_st_map() : RegisterMapper( {
    { "qac_glb_ap_drop_state_cell", { const_cast<uint32_t(*)[0x8]>(&Qac_common_block_drop_st_base->qac_glb_ap_drop_state_cell), 0, {0x8}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_qac_common_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_qac_common_rspec Tm_qac_common_rspec_base=0;
  Tm_qac_common_rspec_map() : RegisterMapper( {
    { "qac_common", { &Tm_qac_common_rspec_base->qac_common, new Qac_common_block_map, {}, sizeof(Qac_common_block) } },
    { "qac_common_block_drop_st", { &Tm_qac_common_rspec_base->qac_common_block_drop_st, new Qac_common_block_drop_st_map, {}, sizeof(Qac_common_block_drop_st) } }
    } )
  {}
};

struct Pipe_block_reg_qac_ctr48_drop_pre_fifo_map: public RegisterMapper {
  static constexpr PTR_Pipe_block_reg_qac_ctr48_drop_pre_fifo Pipe_block_reg_qac_ctr48_drop_pre_fifo_base=0;
  Pipe_block_reg_qac_ctr48_drop_pre_fifo_map() : RegisterMapper( {
    { "qac_ctr48_drop_pre_fifo_0_2", { const_cast<uint32_t(*)>(&Pipe_block_reg_qac_ctr48_drop_pre_fifo_base->qac_ctr48_drop_pre_fifo_0_2), 0, {}, sizeof(uint32_t) } },
    { "qac_ctr48_drop_pre_fifo_1_2", { const_cast<uint32_t(*)>(&Pipe_block_reg_qac_ctr48_drop_pre_fifo_base->qac_ctr48_drop_pre_fifo_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qac_port_rx_disable_map: public RegisterMapper {
  static constexpr PTR_Qac_port_rx_disable Qac_port_rx_disable_base=0;
  Qac_port_rx_disable_map() : RegisterMapper( {
    { "qac_port_rx_disable_0_3", { const_cast<uint32_t(*)>(&Qac_port_rx_disable_base->qac_port_rx_disable_0_3), 0, {}, sizeof(uint32_t) } },
    { "qac_port_rx_disable_1_3", { const_cast<uint32_t(*)>(&Qac_port_rx_disable_base->qac_port_rx_disable_1_3), 0, {}, sizeof(uint32_t) } },
    { "qac_port_rx_disable_2_3", { const_cast<uint32_t(*)>(&Qac_port_rx_disable_base->qac_port_rx_disable_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pipe_block_reg_intr_map: public RegisterMapper {
  static constexpr PTR_Pipe_block_reg_intr Pipe_block_reg_intr_base=0;
  Pipe_block_reg_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Pipe_block_reg_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Pipe_block_reg_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Pipe_block_reg_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Pipe_block_reg_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Pipe_block_reg_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qac_queue_state_visible_bmp_map: public RegisterMapper {
  static constexpr PTR_Qac_queue_state_visible_bmp Qac_queue_state_visible_bmp_base=0;
  Qac_queue_state_visible_bmp_map() : RegisterMapper( {
    { "qac_queue_state_visible_bmp_0_4", { const_cast<uint32_t(*)>(&Qac_queue_state_visible_bmp_base->qac_queue_state_visible_bmp_0_4), 0, {}, sizeof(uint32_t) } },
    { "qac_queue_state_visible_bmp_1_4", { const_cast<uint32_t(*)>(&Qac_queue_state_visible_bmp_base->qac_queue_state_visible_bmp_1_4), 0, {}, sizeof(uint32_t) } },
    { "qac_queue_state_visible_bmp_2_4", { const_cast<uint32_t(*)>(&Qac_queue_state_visible_bmp_base->qac_queue_state_visible_bmp_2_4), 0, {}, sizeof(uint32_t) } },
    { "qac_queue_state_visible_bmp_3_4", { const_cast<uint32_t(*)>(&Qac_queue_state_visible_bmp_base->qac_queue_state_visible_bmp_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pipe_block_reg_map: public RegisterMapper {
  static constexpr PTR_Pipe_block_reg Pipe_block_reg_base=0;
  Pipe_block_reg_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "qac_ctr48_drop_pre_fifo", { const_cast<Pipe_block_reg_qac_ctr48_drop_pre_fifo(*)[0x4]>(&Pipe_block_reg_base->qac_ctr48_drop_pre_fifo), new Pipe_block_reg_qac_ctr48_drop_pre_fifo_map, {0x4}, sizeof(Pipe_block_reg_qac_ctr48_drop_pre_fifo) } },
    { "qac_ctr32_drop_no_dst", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac_ctr32_drop_no_dst), 0, {}, sizeof(uint32_t) } },
    { "qac_ctr32_pre_mc_drop", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac_ctr32_pre_mc_drop), 0, {}, sizeof(uint32_t) } },
    { "qac_ctr32_pre_mc_enq", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac_ctr32_pre_mc_enq), 0, {}, sizeof(uint32_t) } },
    { "qac_ctr32_clc_intf", { const_cast<uint32_t(*)[0x4]>(&Pipe_block_reg_base->qac_ctr32_clc_intf), 0, {0x4}, sizeof(uint32_t) } },
    { "offset_profile", { const_cast<uint32_t(*)[0x20]>(&Pipe_block_reg_base->offset_profile), 0, {0x20}, sizeof(uint32_t) } },
    { "mc_apid", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->mc_apid), 0, {}, sizeof(uint32_t) } },
    { "pipe_config", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->pipe_config), 0, {}, sizeof(uint32_t) } },
    { "discard_queue_cnt_cell", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->discard_queue_cnt_cell), 0, {}, sizeof(uint32_t) } },
    { "discard_queue_wm_cnt_cell", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->discard_queue_wm_cnt_cell), 0, {}, sizeof(uint32_t) } },
    { "qac_port_rx_disable", { const_cast<Qac_port_rx_disable(*)>(&Pipe_block_reg_base->qac_port_rx_disable), new Qac_port_rx_disable_map, {}, sizeof(Qac_port_rx_disable) } },
    { "qac_debug_register", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac_debug_register), 0, {}, sizeof(uint32_t) } },
    { "qac_ecc", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac_ecc), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Pipe_block_reg_base->intr, new Pipe_block_reg_intr_map, {}, sizeof(Pipe_block_reg_intr) } },
    { "queue_drop_table_sbe_err_log", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->queue_drop_table_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "queue_drop_table_mbe_err_log", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->queue_drop_table_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "port_drop_cnt_table_sbe_err_log", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->port_drop_cnt_table_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "port_drop_cnt_table_mbe_err_log", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->port_drop_cnt_table_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "qid_mapping_table_sbe_err_log", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qid_mapping_table_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "qid_mapping_table_mbe_err_log", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qid_mapping_table_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "qac2prc_fifo_sbe_err_log", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac2prc_fifo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "qac2prc_fifo_mbe_err_log", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac2prc_fifo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "prc2psc_fifo_sbe_err_log", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->prc2psc_fifo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "prc2psc_fifo_mbe_err_log", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->prc2psc_fifo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } },
    { "queue_drop_state", { const_cast<uint32_t(*)[0x24]>(&Pipe_block_reg_base->queue_drop_state), 0, {0x24}, sizeof(uint32_t) } },
    { "queue_drop_yel_state", { const_cast<uint32_t(*)[0x24]>(&Pipe_block_reg_base->queue_drop_yel_state), 0, {0x24}, sizeof(uint32_t) } },
    { "queue_drop_red_state", { const_cast<uint32_t(*)[0x24]>(&Pipe_block_reg_base->queue_drop_red_state), 0, {0x24}, sizeof(uint32_t) } },
    { "qac_queue_state_mask", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac_queue_state_mask), 0, {}, sizeof(uint32_t) } },
    { "port_drop_state", { const_cast<uint32_t(*)[0x48]>(&Pipe_block_reg_base->port_drop_state), 0, {0x48}, sizeof(uint32_t) } },
    { "qac_queue_state_visible_bmp", { const_cast<Qac_queue_state_visible_bmp(*)[0x9]>(&Pipe_block_reg_base->qac_queue_state_visible_bmp), new Qac_queue_state_visible_bmp_map, {0x9}, sizeof(Qac_queue_state_visible_bmp) } },
    { "qac_drop_reason_bmp", { const_cast<uint32_t(*)[0x48]>(&Pipe_block_reg_base->qac_drop_reason_bmp), 0, {0x48}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_qac_pipe_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_qac_pipe_rspec Tm_qac_pipe_rspec_base=0;
  Tm_qac_pipe_rspec_map() : RegisterMapper( {
    { "qac_reg", { &Tm_qac_pipe_rspec_base->qac_reg, new Pipe_block_reg_map, {}, sizeof(Pipe_block_reg) } }
    } )
  {}
};

struct Tm_qac_top_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_qac_top_rspec Tm_qac_top_rspec_base=0;
  Tm_qac_top_rspec_map() : RegisterMapper( {
    { "qac_common", { &Tm_qac_top_rspec_base->qac_common, new Tm_qac_common_rspec_map, {}, sizeof(Tm_qac_common_rspec) } },
    { "qac_pipe", { &Tm_qac_top_rspec_base->qac_pipe, new Tm_qac_pipe_rspec_map, {0x4}, sizeof(Tm_qac_pipe_rspec) } }
    } )
  {}
};

struct Tm_sch_pipe_rspec_ecc_map: public RegisterMapper {
  static constexpr PTR_Tm_sch_pipe_rspec_ecc Tm_sch_pipe_rspec_ecc_base=0;
  Tm_sch_pipe_rspec_ecc_map() : RegisterMapper( {
    { "ecc_0_5", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_ecc_base->ecc_0_5), 0, {}, sizeof(uint32_t) } },
    { "ecc_1_5", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_ecc_base->ecc_1_5), 0, {}, sizeof(uint32_t) } },
    { "ecc_2_5", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_ecc_base->ecc_2_5), 0, {}, sizeof(uint32_t) } },
    { "ecc_3_5", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_ecc_base->ecc_3_5), 0, {}, sizeof(uint32_t) } },
    { "ecc_4_5", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_ecc_base->ecc_4_5), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_sch_pipe_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_sch_pipe_rspec_intr Tm_sch_pipe_rspec_intr_base=0;
  Tm_sch_pipe_rspec_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Sch_queue_flush_mask_r_map: public RegisterMapper {
  static constexpr PTR_Sch_queue_flush_mask_r Sch_queue_flush_mask_r_base=0;
  Sch_queue_flush_mask_r_map() : RegisterMapper( {
    { "queue_flush_mask_0_4", { const_cast<uint32_t(*)>(&Sch_queue_flush_mask_r_base->queue_flush_mask_0_4), 0, {}, sizeof(uint32_t) } },
    { "queue_flush_mask_1_4", { const_cast<uint32_t(*)>(&Sch_queue_flush_mask_r_base->queue_flush_mask_1_4), 0, {}, sizeof(uint32_t) } },
    { "queue_flush_mask_2_4", { const_cast<uint32_t(*)>(&Sch_queue_flush_mask_r_base->queue_flush_mask_2_4), 0, {}, sizeof(uint32_t) } },
    { "queue_flush_mask_3_4", { const_cast<uint32_t(*)>(&Sch_queue_flush_mask_r_base->queue_flush_mask_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Sch_watchdog_timer_r_map: public RegisterMapper {
  static constexpr PTR_Sch_watchdog_timer_r Sch_watchdog_timer_r_base=0;
  Sch_watchdog_timer_r_map() : RegisterMapper( {
    { "watchdog_timer_0_2", { const_cast<uint32_t(*)>(&Sch_watchdog_timer_r_base->watchdog_timer_0_2), 0, {}, sizeof(uint32_t) } },
    { "watchdog_timer_1_2", { const_cast<uint32_t(*)>(&Sch_watchdog_timer_r_base->watchdog_timer_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Sch_pq_port_en_r_map: public RegisterMapper {
  static constexpr PTR_Sch_pq_port_en_r Sch_pq_port_en_r_base=0;
  Sch_pq_port_en_r_map() : RegisterMapper( {
    { "pq_port_en_0_3", { const_cast<uint32_t(*)>(&Sch_pq_port_en_r_base->pq_port_en_0_3), 0, {}, sizeof(uint32_t) } },
    { "pq_port_en_1_3", { const_cast<uint32_t(*)>(&Sch_pq_port_en_r_base->pq_port_en_1_3), 0, {}, sizeof(uint32_t) } },
    { "pq_port_en_2_3", { const_cast<uint32_t(*)>(&Sch_pq_port_en_r_base->pq_port_en_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Sch_pq_port_profile_sel_r_map: public RegisterMapper {
  static constexpr PTR_Sch_pq_port_profile_sel_r Sch_pq_port_profile_sel_r_base=0;
  Sch_pq_port_profile_sel_r_map() : RegisterMapper( {
    { "pq_port_profile_sel_0_3", { const_cast<uint32_t(*)>(&Sch_pq_port_profile_sel_r_base->pq_port_profile_sel_0_3), 0, {}, sizeof(uint32_t) } },
    { "pq_port_profile_sel_1_3", { const_cast<uint32_t(*)>(&Sch_pq_port_profile_sel_r_base->pq_port_profile_sel_1_3), 0, {}, sizeof(uint32_t) } },
    { "pq_port_profile_sel_2_3", { const_cast<uint32_t(*)>(&Sch_pq_port_profile_sel_r_base->pq_port_profile_sel_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_sch_pipe_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_sch_pipe_rspec Tm_sch_pipe_rspec_base=0;
  Tm_sch_pipe_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<Tm_sch_pipe_rspec_ecc(*)>(&Tm_sch_pipe_rspec_base->ecc), new Tm_sch_pipe_rspec_ecc_map, {}, sizeof(Tm_sch_pipe_rspec_ecc) } },
    { "intr", { &Tm_sch_pipe_rspec_base->intr, new Tm_sch_pipe_rspec_intr_map, {}, sizeof(Tm_sch_pipe_rspec_intr) } },
    { "tdm_table_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->tdm_table_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "tdm_table_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->tdm_table_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "upd_wac_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_wac_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "upd_wac_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_wac_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "upd_edprsr_advfc_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_edprsr_advfc_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "upd_edprsr_advfc_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_edprsr_advfc_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "q_minrate_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->q_minrate_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "q_minrate_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->q_minrate_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "q_excrate_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->q_excrate_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "q_excrate_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->q_excrate_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "q_maxrate_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->q_maxrate_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "q_maxrate_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->q_maxrate_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "l1_minrate_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->l1_minrate_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "l1_minrate_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->l1_minrate_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "l1_excrate_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->l1_excrate_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "l1_excrate_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->l1_excrate_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "l1_maxrate_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->l1_maxrate_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "l1_maxrate_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->l1_maxrate_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "p_maxrate_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->p_maxrate_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "p_maxrate_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->p_maxrate_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "upd_pex0_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_pex0_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "upd_pex0_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_pex0_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "upd_pex1_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_pex1_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "upd_pex1_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_pex1_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "upd_edprsr_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_edprsr_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "upd_edprsr_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_edprsr_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "pex_credit_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->pex_credit_err_log), 0, {}, sizeof(uint32_t) } },
    { "pex_mac_credit_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->pex_mac_credit_err_log), 0, {}, sizeof(uint32_t) } },
    { "q_watchdog_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->q_watchdog_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "q_watchdog_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->q_watchdog_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ctrl", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "ready", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->ready), 0, {}, sizeof(uint32_t) } },
    { "global_bytecnt_adj", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->global_bytecnt_adj), 0, {}, sizeof(uint32_t) } },
    { "tdm_config", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->tdm_config), 0, {}, sizeof(uint32_t) } },
    { "tdm_table", { const_cast<uint32_t(*)[0x80]>(&Tm_sch_pipe_rspec_base->tdm_table), 0, {0x80}, sizeof(uint32_t) } },
    { "port_arb_ctrl", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->port_arb_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pex_credit_ctrl", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->pex_credit_ctrl), 0, {}, sizeof(uint32_t) } },
    { "port_config", { const_cast<uint32_t(*)[0x48]>(&Tm_sch_pipe_rspec_base->port_config), 0, {0x48}, sizeof(uint32_t) } },
    { "port_pfc_status_mem", { const_cast<uint32_t(*)[0x48]>(&Tm_sch_pipe_rspec_base->port_pfc_status_mem), 0, {0x48}, sizeof(uint32_t) } },
    { "port_pex_status_mem", { const_cast<uint32_t(*)[0x240]>(&Tm_sch_pipe_rspec_base->port_pex_status_mem), 0, {0x240}, sizeof(uint32_t) } },
    { "mac_pex_status_mem", { const_cast<uint32_t(*)[0x5]>(&Tm_sch_pipe_rspec_base->mac_pex_status_mem), 0, {0x5}, sizeof(uint32_t) } },
    { "l1_config", { const_cast<uint32_t(*)[0x120]>(&Tm_sch_pipe_rspec_base->l1_config), 0, {0x120}, sizeof(uint32_t) } },
    { "queue_config", { const_cast<uint32_t(*)[0x480]>(&Tm_sch_pipe_rspec_base->queue_config), 0, {0x480}, sizeof(uint32_t) } },
    { "queue_flush_ctrl", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->queue_flush_ctrl), 0, {}, sizeof(uint32_t) } },
    { "queue_flush_mask", { const_cast<Sch_queue_flush_mask_r(*)>(&Tm_sch_pipe_rspec_base->queue_flush_mask), new Sch_queue_flush_mask_r_map, {}, sizeof(Sch_queue_flush_mask_r) } },
    { "watchdog_config", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->watchdog_config), 0, {}, sizeof(uint32_t) } },
    { "watchdog_status", { const_cast<uint32_t(*)[0x480]>(&Tm_sch_pipe_rspec_base->watchdog_status), 0, {0x480}, sizeof(uint32_t) } },
    { "watchdog_timer", { const_cast<Sch_watchdog_timer_r(*)>(&Tm_sch_pipe_rspec_base->watchdog_timer), new Sch_watchdog_timer_r_map, {}, sizeof(Sch_watchdog_timer_r) } },
    { "iadvfc_config", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->iadvfc_config), 0, {}, sizeof(uint32_t) } },
    { "iadvfc_status", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->iadvfc_status), 0, {}, sizeof(uint32_t) } },
    { "iadvfc_ctr", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->iadvfc_ctr), 0, {}, sizeof(uint32_t) } },
    { "eadvfc_config", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->eadvfc_config), 0, {}, sizeof(uint32_t) } },
    { "eadvfc_status", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->eadvfc_status), 0, {}, sizeof(uint32_t) } },
    { "eadvfc_ctr", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->eadvfc_ctr), 0, {}, sizeof(uint32_t) } },
    { "pq_ctrl", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->pq_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pq_port_en", { const_cast<Sch_pq_port_en_r(*)>(&Tm_sch_pipe_rspec_base->pq_port_en), new Sch_pq_port_en_r_map, {}, sizeof(Sch_pq_port_en_r) } },
    { "pq_port_profile_sel", { const_cast<Sch_pq_port_profile_sel_r(*)>(&Tm_sch_pipe_rspec_base->pq_port_profile_sel), new Sch_pq_port_profile_sel_r_map, {}, sizeof(Sch_pq_port_profile_sel_r) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_sch_top_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_sch_top_rspec Tm_sch_top_rspec_base=0;
  Tm_sch_top_rspec_map() : RegisterMapper( {
    { "sch", { &Tm_sch_top_rspec_base->sch, new Tm_sch_pipe_rspec_map, {0x2}, sizeof(Tm_sch_pipe_rspec) } }
    } )
  {}
};

struct Tm_clc_pipe_rspec_ecc_map: public RegisterMapper {
  static constexpr PTR_Tm_clc_pipe_rspec_ecc Tm_clc_pipe_rspec_ecc_base=0;
  Tm_clc_pipe_rspec_ecc_map() : RegisterMapper( {
    { "ecc_0_2", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_ecc_base->ecc_0_2), 0, {}, sizeof(uint32_t) } },
    { "ecc_1_2", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_ecc_base->ecc_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_clc_pipe_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_clc_pipe_rspec_intr Tm_clc_pipe_rspec_intr_base=0;
  Tm_clc_pipe_rspec_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_clc_pipe_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_clc_pipe_rspec Tm_clc_pipe_rspec_base=0;
  Tm_clc_pipe_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "pipe_ctrl", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->pipe_ctrl), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<Tm_clc_pipe_rspec_ecc(*)>(&Tm_clc_pipe_rspec_base->ecc), new Tm_clc_pipe_rspec_ecc_map, {}, sizeof(Tm_clc_pipe_rspec_ecc) } },
    { "intr", { &Tm_clc_pipe_rspec_base->intr, new Tm_clc_pipe_rspec_intr_map, {}, sizeof(Tm_clc_pipe_rspec_intr) } },
    { "enfifo_serr_ep0_log", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->enfifo_serr_ep0_log), 0, {}, sizeof(uint32_t) } },
    { "enfifo_serr_ep1_log", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->enfifo_serr_ep1_log), 0, {}, sizeof(uint32_t) } },
    { "enfifo_serr_ep2_log", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->enfifo_serr_ep2_log), 0, {}, sizeof(uint32_t) } },
    { "enfifo_serr_ep3_log", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->enfifo_serr_ep3_log), 0, {}, sizeof(uint32_t) } },
    { "enfifo_merr_ep0_log", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->enfifo_merr_ep0_log), 0, {}, sizeof(uint32_t) } },
    { "enfifo_merr_ep1_log", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->enfifo_merr_ep1_log), 0, {}, sizeof(uint32_t) } },
    { "enfifo_merr_ep2_log", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->enfifo_merr_ep2_log), 0, {}, sizeof(uint32_t) } },
    { "enfifo_merr_ep3_log", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->enfifo_merr_ep3_log), 0, {}, sizeof(uint32_t) } },
    { "clc_qac_serr_log", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->clc_qac_serr_log), 0, {}, sizeof(uint32_t) } },
    { "clc_qac_merr_log", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->clc_qac_merr_log), 0, {}, sizeof(uint32_t) } },
    { "fifo_threshold", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->fifo_threshold), 0, {}, sizeof(uint32_t) } },
    { "qclc_pt_spd", { const_cast<uint32_t(*)[0x48]>(&Tm_clc_pipe_rspec_base->qclc_pt_spd), 0, {0x48}, sizeof(uint32_t) } },
    { "clc_debug_reg", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->clc_debug_reg), 0, {}, sizeof(uint32_t) } },
    { "ingress_port_ct_state", { const_cast<uint32_t(*)[0x3]>(&Tm_clc_pipe_rspec_base->ingress_port_ct_state), 0, {0x3}, sizeof(uint32_t) } },
    { "egress_port_ct_state", { const_cast<uint32_t(*)[0x3]>(&Tm_clc_pipe_rspec_base->egress_port_ct_state), 0, {0x3}, sizeof(uint32_t) } },
    { "mc_ct_cnt", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->mc_ct_cnt), 0, {}, sizeof(uint32_t) } },
    { "uc_ct_cnt", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->uc_ct_cnt), 0, {}, sizeof(uint32_t) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_clc_common_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_clc_common_rspec Tm_clc_common_rspec_base=0;
  Tm_clc_common_rspec_map() : RegisterMapper( {
    { "dft_csr", { const_cast<uint32_t(*)>(&Tm_clc_common_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "top_ctrl", { const_cast<uint32_t(*)>(&Tm_clc_common_rspec_base->top_ctrl), 0, {}, sizeof(uint32_t) } },
    { "tot_th", { const_cast<uint32_t(*)>(&Tm_clc_common_rspec_base->tot_th), 0, {}, sizeof(uint32_t) } },
    { "qclc_pt_ct_cnt_pipe0", { const_cast<uint32_t(*)[0x12]>(&Tm_clc_common_rspec_base->qclc_pt_ct_cnt_pipe0), 0, {0x12}, sizeof(uint32_t) } },
    { "qclc_pt_ct_cnt_pipe1", { const_cast<uint32_t(*)[0x12]>(&Tm_clc_common_rspec_base->qclc_pt_ct_cnt_pipe1), 0, {0x12}, sizeof(uint32_t) } },
    { "qclc_pt_ct_cnt_pipe2", { const_cast<uint32_t(*)[0x12]>(&Tm_clc_common_rspec_base->qclc_pt_ct_cnt_pipe2), 0, {0x12}, sizeof(uint32_t) } },
    { "qclc_pt_ct_cnt_pipe3", { const_cast<uint32_t(*)[0x12]>(&Tm_clc_common_rspec_base->qclc_pt_ct_cnt_pipe3), 0, {0x12}, sizeof(uint32_t) } },
    { "ct_tot_cnt", { const_cast<uint32_t(*)>(&Tm_clc_common_rspec_base->ct_tot_cnt), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_clc_top_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_clc_top_rspec Tm_clc_top_rspec_base=0;
  Tm_clc_top_rspec_map() : RegisterMapper( {
    { "clc", { &Tm_clc_top_rspec_base->clc, new Tm_clc_pipe_rspec_map, {0x4}, sizeof(Tm_clc_pipe_rspec) } },
    { "clc_common", { &Tm_clc_top_rspec_base->clc_common, new Tm_clc_common_rspec_map, {}, sizeof(Tm_clc_common_rspec) } }
    } )
  {}
};

struct Tm_pex_pipe_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_pex_pipe_rspec_intr Tm_pex_pipe_rspec_intr_base=0;
  Tm_pex_pipe_rspec_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qpex_port_tdm_enb_map_map: public RegisterMapper {
  static constexpr PTR_Qpex_port_tdm_enb_map Qpex_port_tdm_enb_map_base=0;
  Qpex_port_tdm_enb_map_map() : RegisterMapper( {
    { "port_tdm_enb_map_0_3", { const_cast<uint32_t(*)>(&Qpex_port_tdm_enb_map_base->port_tdm_enb_map_0_3), 0, {}, sizeof(uint32_t) } },
    { "port_tdm_enb_map_1_3", { const_cast<uint32_t(*)>(&Qpex_port_tdm_enb_map_base->port_tdm_enb_map_1_3), 0, {}, sizeof(uint32_t) } },
    { "port_tdm_enb_map_2_3", { const_cast<uint32_t(*)>(&Qpex_port_tdm_enb_map_base->port_tdm_enb_map_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qpex_epb_cred_cnt_map: public RegisterMapper {
  static constexpr PTR_Qpex_epb_cred_cnt Qpex_epb_cred_cnt_base=0;
  Qpex_epb_cred_cnt_map() : RegisterMapper( {
    { "pt_epb_cred_0_2", { const_cast<uint32_t(*)>(&Qpex_epb_cred_cnt_base->pt_epb_cred_0_2), 0, {}, sizeof(uint32_t) } },
    { "pt_epb_cred_1_2", { const_cast<uint32_t(*)>(&Qpex_epb_cred_cnt_base->pt_epb_cred_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pex_eport_tx_cnt_map: public RegisterMapper {
  static constexpr PTR_Pex_eport_tx_cnt Pex_eport_tx_cnt_base=0;
  Pex_eport_tx_cnt_map() : RegisterMapper( {
    { "pex_eport_tx_cnt_0_2", { const_cast<uint32_t(*)>(&Pex_eport_tx_cnt_base->pex_eport_tx_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "pex_eport_tx_cnt_1_2", { const_cast<uint32_t(*)>(&Pex_eport_tx_cnt_base->pex_eport_tx_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pex_tx_cnt_timer_map: public RegisterMapper {
  static constexpr PTR_Pex_tx_cnt_timer Pex_tx_cnt_timer_base=0;
  Pex_tx_cnt_timer_map() : RegisterMapper( {
    { "pex_tx_cnt_timer_0_2", { const_cast<uint32_t(*)>(&Pex_tx_cnt_timer_base->pex_tx_cnt_timer_0_2), 0, {}, sizeof(uint32_t) } },
    { "pex_tx_cnt_timer_1_2", { const_cast<uint32_t(*)>(&Pex_tx_cnt_timer_base->pex_tx_cnt_timer_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pex_cnt_map: public RegisterMapper {
  static constexpr PTR_Pex_cnt Pex_cnt_base=0;
  Pex_cnt_map() : RegisterMapper( {
    { "tot_byte_cnt_0_2", { const_cast<uint32_t(*)>(&Pex_cnt_base->tot_byte_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "tot_byte_cnt_1_2", { const_cast<uint32_t(*)>(&Pex_cnt_base->tot_byte_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_pex_pipe_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_pex_pipe_rspec Tm_pex_pipe_rspec_base=0;
  Tm_pex_pipe_rspec_map() : RegisterMapper( {
    { "pipe_ctrl", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->pipe_ctrl), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->ecc), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Tm_pex_pipe_rspec_base->intr, new Tm_pex_pipe_rspec_intr_map, {}, sizeof(Tm_pex_pipe_rspec_intr) } },
    { "linkmem_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->linkmem_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "linkmem_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->linkmem_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "dq_ph_fifo_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->dq_ph_fifo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "dq_ph_fifo_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->dq_ph_fifo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "dq_meta_fifo_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->dq_meta_fifo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "dq_meta_fifo_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->dq_meta_fifo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ph_afifo_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->ph_afifo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ph_afifo_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->ph_afifo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "discard_ph_fifo0_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->discard_ph_fifo0_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "discard_ph_fifo0_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->discard_ph_fifo0_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "discard_ph_fifo1_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->discard_ph_fifo1_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "discard_ph_fifo1_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->discard_ph_fifo1_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "port_tdm_enb_map", { const_cast<Qpex_port_tdm_enb_map(*)>(&Tm_pex_pipe_rspec_base->port_tdm_enb_map), new Qpex_port_tdm_enb_map_map, {}, sizeof(Qpex_port_tdm_enb_map) } },
    { "pt_state", { const_cast<uint32_t(*)[0xb]>(&Tm_pex_pipe_rspec_base->pt_state), 0, {0xb}, sizeof(uint32_t) } },
    { "pt_epb_cred", { const_cast<Qpex_epb_cred_cnt(*)[0x9]>(&Tm_pex_pipe_rspec_base->pt_epb_cred), new Qpex_epb_cred_cnt_map, {0x9}, sizeof(Qpex_epb_cred_cnt) } },
    { "q_empty", { const_cast<uint32_t(*)[0x18]>(&Tm_pex_pipe_rspec_base->q_empty), 0, {0x18}, sizeof(uint32_t) } },
    { "pt_gap_wm", { const_cast<uint32_t(*)[0x48]>(&Tm_pex_pipe_rspec_base->pt_gap_wm), 0, {0x48}, sizeof(uint32_t) } },
    { "cfg_ct_timer", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->cfg_ct_timer), 0, {}, sizeof(uint32_t) } },
    { "pex_pfc_map_table", { const_cast<uint32_t(*)[0x240]>(&Tm_pex_pipe_rspec_base->pex_pfc_map_table), 0, {0x240}, sizeof(uint32_t) } },
    { "pex_eport_tx_cnt", { const_cast<Pex_eport_tx_cnt(*)[0x48]>(&Tm_pex_pipe_rspec_base->pex_eport_tx_cnt), new Pex_eport_tx_cnt_map, {0x48}, sizeof(Pex_eport_tx_cnt) } },
    { "pex_debug_register", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->pex_debug_register), 0, {}, sizeof(uint32_t) } },
    { "pex_tx_cnt_timer", { const_cast<Pex_tx_cnt_timer(*)>(&Tm_pex_pipe_rspec_base->pex_tx_cnt_timer), new Pex_tx_cnt_timer_map, {}, sizeof(Pex_tx_cnt_timer) } },
    { "pex_epb_credit_probe", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->pex_epb_credit_probe), 0, {}, sizeof(uint32_t) } },
    { "tot_byte_cnt", { const_cast<Pex_cnt(*)>(&Tm_pex_pipe_rspec_base->tot_byte_cnt), new Pex_cnt_map, {}, sizeof(Pex_cnt) } },
    { "tot_pkt_cnt", { const_cast<Pex_cnt(*)>(&Tm_pex_pipe_rspec_base->tot_pkt_cnt), new Pex_cnt_map, {}, sizeof(Pex_cnt) } },
    { "dis_cell_cnt", { const_cast<Pex_cnt(*)>(&Tm_pex_pipe_rspec_base->dis_cell_cnt), new Pex_cnt_map, {}, sizeof(Pex_cnt) } },
    { "scratch", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_pex_common_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_pex_common_rspec Tm_pex_common_rspec_base=0;
  Tm_pex_common_rspec_map() : RegisterMapper( {
    { "dft_csr", { const_cast<uint32_t(*)>(&Tm_pex_common_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "clm_dft_csr", { const_cast<uint32_t(*)>(&Tm_pex_common_rspec_base->clm_dft_csr), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_pex_top_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_pex_top_rspec Tm_pex_top_rspec_base=0;
  Tm_pex_top_rspec_map() : RegisterMapper( {
    { "pex", { &Tm_pex_top_rspec_base->pex, new Tm_pex_pipe_rspec_map, {0x4}, sizeof(Tm_pex_pipe_rspec) } },
    { "pex_common", { &Tm_pex_top_rspec_base->pex_common, new Tm_pex_common_rspec_map, {}, sizeof(Tm_pex_common_rspec) } }
    } )
  {}
};

struct Tm_qlc_pipe_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_qlc_pipe_rspec_intr Tm_qlc_pipe_rspec_intr_base=0;
  Tm_qlc_pipe_rspec_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qlc_pex_dis_cnt_map: public RegisterMapper {
  static constexpr PTR_Qlc_pex_dis_cnt Qlc_pex_dis_cnt_base=0;
  Qlc_pex_dis_cnt_map() : RegisterMapper( {
    { "pex_dis_cnt_0_2", { const_cast<uint32_t(*)>(&Qlc_pex_dis_cnt_base->pex_dis_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "pex_dis_cnt_1_2", { const_cast<uint32_t(*)>(&Qlc_pex_dis_cnt_base->pex_dis_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qlc_qac_dis_cnt_map: public RegisterMapper {
  static constexpr PTR_Qlc_qac_dis_cnt Qlc_qac_dis_cnt_base=0;
  Qlc_qac_dis_cnt_map() : RegisterMapper( {
    { "qac_dis_cnt_0_2", { const_cast<uint32_t(*)>(&Qlc_qac_dis_cnt_base->qac_dis_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "qac_dis_cnt_1_2", { const_cast<uint32_t(*)>(&Qlc_qac_dis_cnt_base->qac_dis_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qlc_qid_eq_cnt_map: public RegisterMapper {
  static constexpr PTR_Qlc_qid_eq_cnt Qlc_qid_eq_cnt_base=0;
  Qlc_qid_eq_cnt_map() : RegisterMapper( {
    { "qid_eq_cnt_0_2", { const_cast<uint32_t(*)>(&Qlc_qid_eq_cnt_base->qid_eq_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "qid_eq_cnt_1_2", { const_cast<uint32_t(*)>(&Qlc_qid_eq_cnt_base->qid_eq_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qlc_qid_deq_cnt_map: public RegisterMapper {
  static constexpr PTR_Qlc_qid_deq_cnt Qlc_qid_deq_cnt_base=0;
  Qlc_qid_deq_cnt_map() : RegisterMapper( {
    { "qid_deq_cnt_0_2", { const_cast<uint32_t(*)>(&Qlc_qid_deq_cnt_base->qid_deq_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "qid_deq_cnt_1_2", { const_cast<uint32_t(*)>(&Qlc_qid_deq_cnt_base->qid_deq_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qlc_tot_eq_cnt_map: public RegisterMapper {
  static constexpr PTR_Qlc_tot_eq_cnt Qlc_tot_eq_cnt_base=0;
  Qlc_tot_eq_cnt_map() : RegisterMapper( {
    { "tot_eq_cnt_0_2", { const_cast<uint32_t(*)>(&Qlc_tot_eq_cnt_base->tot_eq_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "tot_eq_cnt_1_2", { const_cast<uint32_t(*)>(&Qlc_tot_eq_cnt_base->tot_eq_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qlc_tot_dq_cnt_map: public RegisterMapper {
  static constexpr PTR_Qlc_tot_dq_cnt Qlc_tot_dq_cnt_base=0;
  Qlc_tot_dq_cnt_map() : RegisterMapper( {
    { "tot_dq_cnt_0_2", { const_cast<uint32_t(*)>(&Qlc_tot_dq_cnt_base->tot_dq_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "tot_dq_cnt_1_2", { const_cast<uint32_t(*)>(&Qlc_tot_dq_cnt_base->tot_dq_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qlc_tot_dis_dq_cnt_map: public RegisterMapper {
  static constexpr PTR_Qlc_tot_dis_dq_cnt Qlc_tot_dis_dq_cnt_base=0;
  Qlc_tot_dis_dq_cnt_map() : RegisterMapper( {
    { "tot_dis_dq_cnt_0_2", { const_cast<uint32_t(*)>(&Qlc_tot_dis_dq_cnt_base->tot_dis_dq_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "tot_dis_dq_cnt_1_2", { const_cast<uint32_t(*)>(&Qlc_tot_dis_dq_cnt_base->tot_dis_dq_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_qlc_pipe_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_qlc_pipe_rspec Tm_qlc_pipe_rspec_base=0;
  Tm_qlc_pipe_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "control", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_base->control), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_base->ecc), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Tm_qlc_pipe_rspec_base->intr, new Tm_qlc_pipe_rspec_intr_map, {}, sizeof(Tm_qlc_pipe_rspec_intr) } },
    { "linkmem_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_base->linkmem_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "linkmem_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_base->linkmem_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "schdeq_err_log", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_base->schdeq_err_log), 0, {}, sizeof(uint32_t) } },
    { "dis_qlen", { const_cast<uint32_t(*)[0x20]>(&Tm_qlc_pipe_rspec_base->dis_qlen), 0, {0x20}, sizeof(uint32_t) } },
    { "dis_cred", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_base->dis_cred), 0, {}, sizeof(uint32_t) } },
    { "fifo_threshold", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_base->fifo_threshold), 0, {}, sizeof(uint32_t) } },
    { "pex_dis_cnt", { const_cast<Qlc_pex_dis_cnt(*)>(&Tm_qlc_pipe_rspec_base->pex_dis_cnt), new Qlc_pex_dis_cnt_map, {}, sizeof(Qlc_pex_dis_cnt) } },
    { "qac_dis_cnt", { const_cast<Qlc_qac_dis_cnt(*)>(&Tm_qlc_pipe_rspec_base->qac_dis_cnt), new Qlc_qac_dis_cnt_map, {}, sizeof(Qlc_qac_dis_cnt) } },
    { "qid_eq_cnt", { const_cast<Qlc_qid_eq_cnt(*)>(&Tm_qlc_pipe_rspec_base->qid_eq_cnt), new Qlc_qid_eq_cnt_map, {}, sizeof(Qlc_qid_eq_cnt) } },
    { "qid_deq_cnt", { const_cast<Qlc_qid_deq_cnt(*)>(&Tm_qlc_pipe_rspec_base->qid_deq_cnt), new Qlc_qid_deq_cnt_map, {}, sizeof(Qlc_qid_deq_cnt) } },
    { "tot_eq_cnt", { const_cast<Qlc_tot_eq_cnt(*)>(&Tm_qlc_pipe_rspec_base->tot_eq_cnt), new Qlc_tot_eq_cnt_map, {}, sizeof(Qlc_tot_eq_cnt) } },
    { "tot_dq_cnt", { const_cast<Qlc_tot_dq_cnt(*)>(&Tm_qlc_pipe_rspec_base->tot_dq_cnt), new Qlc_tot_dq_cnt_map, {}, sizeof(Qlc_tot_dq_cnt) } },
    { "tot_dis_dq_cnt", { const_cast<Qlc_tot_dis_dq_cnt(*)>(&Tm_qlc_pipe_rspec_base->tot_dis_dq_cnt), new Qlc_tot_dis_dq_cnt_map, {}, sizeof(Qlc_tot_dis_dq_cnt) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qlm_blk_rdy_map: public RegisterMapper {
  static constexpr PTR_Qlm_blk_rdy Qlm_blk_rdy_base=0;
  Qlm_blk_rdy_map() : RegisterMapper( {
    { "blk_rdy_0_3", { const_cast<uint32_t(*)>(&Qlm_blk_rdy_base->blk_rdy_0_3), 0, {}, sizeof(uint32_t) } },
    { "blk_rdy_1_3", { const_cast<uint32_t(*)>(&Qlm_blk_rdy_base->blk_rdy_1_3), 0, {}, sizeof(uint32_t) } },
    { "blk_rdy_2_3", { const_cast<uint32_t(*)>(&Qlm_blk_rdy_base->blk_rdy_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qlm_blk_reset_map: public RegisterMapper {
  static constexpr PTR_Qlm_blk_reset Qlm_blk_reset_base=0;
  Qlm_blk_reset_map() : RegisterMapper( {
    { "qlm_blk_reset_0_3", { const_cast<uint32_t(*)>(&Qlm_blk_reset_base->qlm_blk_reset_0_3), 0, {}, sizeof(uint32_t) } },
    { "qlm_blk_reset_1_3", { const_cast<uint32_t(*)>(&Qlm_blk_reset_base->qlm_blk_reset_1_3), 0, {}, sizeof(uint32_t) } },
    { "qlm_blk_reset_2_3", { const_cast<uint32_t(*)>(&Qlm_blk_reset_base->qlm_blk_reset_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_qlc_common_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_qlc_common_rspec Tm_qlc_common_rspec_base=0;
  Tm_qlc_common_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Tm_qlc_common_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Tm_qlc_common_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "qlm_dft_csr", { const_cast<uint32_t(*)>(&Tm_qlc_common_rspec_base->qlm_dft_csr), 0, {}, sizeof(uint32_t) } },
    { "blk_rdy", { const_cast<Qlm_blk_rdy(*)>(&Tm_qlc_common_rspec_base->blk_rdy), new Qlm_blk_rdy_map, {}, sizeof(Qlm_blk_rdy) } },
    { "qlink_rdy", { const_cast<uint32_t(*)>(&Tm_qlc_common_rspec_base->qlink_rdy), 0, {}, sizeof(uint32_t) } },
    { "qlm_blk_reset", { const_cast<Qlm_blk_reset(*)>(&Tm_qlc_common_rspec_base->qlm_blk_reset), new Qlm_blk_reset_map, {}, sizeof(Qlm_blk_reset) } }
    } )
  {}
};

struct Tm_qlc_top_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_qlc_top_rspec Tm_qlc_top_rspec_base=0;
  Tm_qlc_top_rspec_map() : RegisterMapper( {
    { "qlc", { &Tm_qlc_top_rspec_base->qlc, new Tm_qlc_pipe_rspec_map, {0x4}, sizeof(Tm_qlc_pipe_rspec) } },
    { "qlc_common", { &Tm_qlc_top_rspec_base->qlc_common, new Tm_qlc_common_rspec_map, {}, sizeof(Tm_qlc_common_rspec) } }
    } )
  {}
};

struct Tm_prc_pipe_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_prc_pipe_rspec_intr Tm_prc_pipe_rspec_intr_base=0;
  Tm_prc_pipe_rspec_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_prc_pipe_rspec_cache_mbe_err_log_map: public RegisterMapper {
  static constexpr PTR_Tm_prc_pipe_rspec_cache_mbe_err_log Tm_prc_pipe_rspec_cache_mbe_err_log_base=0;
  Tm_prc_pipe_rspec_cache_mbe_err_log_map() : RegisterMapper( {
    { "cache_mbe_err_log_0_2", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_cache_mbe_err_log_base->cache_mbe_err_log_0_2), 0, {}, sizeof(uint32_t) } },
    { "cache_mbe_err_log_1_2", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_cache_mbe_err_log_base->cache_mbe_err_log_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_prc_pipe_rspec_cache_sbe_err_log_map: public RegisterMapper {
  static constexpr PTR_Tm_prc_pipe_rspec_cache_sbe_err_log Tm_prc_pipe_rspec_cache_sbe_err_log_base=0;
  Tm_prc_pipe_rspec_cache_sbe_err_log_map() : RegisterMapper( {
    { "cache_sbe_err_log_0_2", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_cache_sbe_err_log_base->cache_sbe_err_log_0_2), 0, {}, sizeof(uint32_t) } },
    { "cache_sbe_err_log_1_2", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_cache_sbe_err_log_base->cache_sbe_err_log_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prc_qac_cnt_map: public RegisterMapper {
  static constexpr PTR_Prc_qac_cnt Prc_qac_cnt_base=0;
  Prc_qac_cnt_map() : RegisterMapper( {
    { "qac_cnt_0_2", { const_cast<uint32_t(*)>(&Prc_qac_cnt_base->qac_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "qac_cnt_1_2", { const_cast<uint32_t(*)>(&Prc_qac_cnt_base->qac_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prc_qac_zero_cnt_map: public RegisterMapper {
  static constexpr PTR_Prc_qac_zero_cnt Prc_qac_zero_cnt_base=0;
  Prc_qac_zero_cnt_map() : RegisterMapper( {
    { "qac_zero_cnt_0_2", { const_cast<uint32_t(*)>(&Prc_qac_zero_cnt_base->qac_zero_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "qac_zero_cnt_1_2", { const_cast<uint32_t(*)>(&Prc_qac_zero_cnt_base->qac_zero_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prc_pex_cnt_map: public RegisterMapper {
  static constexpr PTR_Prc_pex_cnt Prc_pex_cnt_base=0;
  Prc_pex_cnt_map() : RegisterMapper( {
    { "pex_cnt_0_2", { const_cast<uint32_t(*)>(&Prc_pex_cnt_base->pex_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "pex_cnt_1_2", { const_cast<uint32_t(*)>(&Prc_pex_cnt_base->pex_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prc_pex_zero_cnt_map: public RegisterMapper {
  static constexpr PTR_Prc_pex_zero_cnt Prc_pex_zero_cnt_base=0;
  Prc_pex_zero_cnt_map() : RegisterMapper( {
    { "pex_zero_cnt_0_2", { const_cast<uint32_t(*)>(&Prc_pex_zero_cnt_base->pex_zero_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "pex_zero_cnt_1_2", { const_cast<uint32_t(*)>(&Prc_pex_zero_cnt_base->pex_zero_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_prc_pipe_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_prc_pipe_rspec Tm_prc_pipe_rspec_base=0;
  Tm_prc_pipe_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "control", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_base->control), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_base->ecc), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Tm_prc_pipe_rspec_base->intr, new Tm_prc_pipe_rspec_intr_map, {}, sizeof(Tm_prc_pipe_rspec_intr) } },
    { "cache_mbe_err_log", { const_cast<Tm_prc_pipe_rspec_cache_mbe_err_log(*)>(&Tm_prc_pipe_rspec_base->cache_mbe_err_log), new Tm_prc_pipe_rspec_cache_mbe_err_log_map, {}, sizeof(Tm_prc_pipe_rspec_cache_mbe_err_log) } },
    { "cache_sbe_err_log", { const_cast<Tm_prc_pipe_rspec_cache_sbe_err_log(*)>(&Tm_prc_pipe_rspec_base->cache_sbe_err_log), new Tm_prc_pipe_rspec_cache_sbe_err_log_map, {}, sizeof(Tm_prc_pipe_rspec_cache_sbe_err_log) } },
    { "qac_cnt", { const_cast<Prc_qac_cnt(*)>(&Tm_prc_pipe_rspec_base->qac_cnt), new Prc_qac_cnt_map, {}, sizeof(Prc_qac_cnt) } },
    { "qac_zero_cnt", { const_cast<Prc_qac_zero_cnt(*)>(&Tm_prc_pipe_rspec_base->qac_zero_cnt), new Prc_qac_zero_cnt_map, {}, sizeof(Prc_qac_zero_cnt) } },
    { "pex_cnt", { const_cast<Prc_pex_cnt(*)>(&Tm_prc_pipe_rspec_base->pex_cnt), new Prc_pex_cnt_map, {}, sizeof(Prc_pex_cnt) } },
    { "pex_zero_cnt", { const_cast<Prc_pex_zero_cnt(*)>(&Tm_prc_pipe_rspec_base->pex_zero_cnt), new Prc_pex_zero_cnt_map, {}, sizeof(Prc_pex_zero_cnt) } },
    { "cache_wm", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_base->cache_wm), 0, {}, sizeof(uint32_t) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_prc_common_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_prc_common_rspec Tm_prc_common_rspec_base=0;
  Tm_prc_common_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Tm_prc_common_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Tm_prc_common_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "prm_dft_csr", { const_cast<uint32_t(*)>(&Tm_prc_common_rspec_base->prm_dft_csr), 0, {}, sizeof(uint32_t) } },
    { "control", { const_cast<uint32_t(*)>(&Tm_prc_common_rspec_base->control), 0, {}, sizeof(uint32_t) } },
    { "status", { const_cast<uint32_t(*)>(&Tm_prc_common_rspec_base->status), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_prc_top_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_prc_top_rspec Tm_prc_top_rspec_base=0;
  Tm_prc_top_rspec_map() : RegisterMapper( {
    { "prc", { &Tm_prc_top_rspec_base->prc, new Tm_prc_pipe_rspec_map, {0x4}, sizeof(Tm_prc_pipe_rspec) } },
    { "prc_common", { &Tm_prc_top_rspec_base->prc_common, new Tm_prc_common_rspec_map, {}, sizeof(Tm_prc_common_rspec) } }
    } )
  {}
};

struct Pre_filter_ctrl_map: public RegisterMapper {
  static constexpr PTR_Pre_filter_ctrl Pre_filter_ctrl_base=0;
  Pre_filter_ctrl_map() : RegisterMapper( {
    { "filter_ctrl_0_3", { const_cast<uint32_t(*)>(&Pre_filter_ctrl_base->filter_ctrl_0_3), 0, {}, sizeof(uint32_t) } },
    { "filter_ctrl_1_3", { const_cast<uint32_t(*)>(&Pre_filter_ctrl_base->filter_ctrl_1_3), 0, {}, sizeof(uint32_t) } },
    { "filter_ctrl_2_3", { const_cast<uint32_t(*)>(&Pre_filter_ctrl_base->filter_ctrl_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_filter_mask_map: public RegisterMapper {
  static constexpr PTR_Pre_filter_mask Pre_filter_mask_base=0;
  Pre_filter_mask_map() : RegisterMapper( {
    { "filter_mask_0_3", { const_cast<uint32_t(*)>(&Pre_filter_mask_base->filter_mask_0_3), 0, {}, sizeof(uint32_t) } },
    { "filter_mask_1_3", { const_cast<uint32_t(*)>(&Pre_filter_mask_base->filter_mask_1_3), 0, {}, sizeof(uint32_t) } },
    { "filter_mask_2_3", { const_cast<uint32_t(*)>(&Pre_filter_mask_base->filter_mask_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_pre_pipe_rspec_ecc_map: public RegisterMapper {
  static constexpr PTR_Tm_pre_pipe_rspec_ecc Tm_pre_pipe_rspec_ecc_base=0;
  Tm_pre_pipe_rspec_ecc_map() : RegisterMapper( {
    { "ecc_0_2", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_ecc_base->ecc_0_2), 0, {}, sizeof(uint32_t) } },
    { "ecc_1_2", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_ecc_base->ecc_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_pre_pipe_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_pre_pipe_rspec_intr Tm_pre_pipe_rspec_intr_base=0;
  Tm_pre_pipe_rspec_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_ctr48_cpu_copies_map: public RegisterMapper {
  static constexpr PTR_Pre_ctr48_cpu_copies Pre_ctr48_cpu_copies_base=0;
  Pre_ctr48_cpu_copies_map() : RegisterMapper( {
    { "cpu_copies_0_2", { const_cast<uint32_t(*)>(&Pre_ctr48_cpu_copies_base->cpu_copies_0_2), 0, {}, sizeof(uint32_t) } },
    { "cpu_copies_1_2", { const_cast<uint32_t(*)>(&Pre_ctr48_cpu_copies_base->cpu_copies_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_ctr48_ph_processed_map: public RegisterMapper {
  static constexpr PTR_Pre_ctr48_ph_processed Pre_ctr48_ph_processed_base=0;
  Pre_ctr48_ph_processed_map() : RegisterMapper( {
    { "ph_processed_0_2", { const_cast<uint32_t(*)>(&Pre_ctr48_ph_processed_base->ph_processed_0_2), 0, {}, sizeof(uint32_t) } },
    { "ph_processed_1_2", { const_cast<uint32_t(*)>(&Pre_ctr48_ph_processed_base->ph_processed_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_ctr48_total_copies_map: public RegisterMapper {
  static constexpr PTR_Pre_ctr48_total_copies Pre_ctr48_total_copies_base=0;
  Pre_ctr48_total_copies_map() : RegisterMapper( {
    { "total_copies_0_2", { const_cast<uint32_t(*)>(&Pre_ctr48_total_copies_base->total_copies_0_2), 0, {}, sizeof(uint32_t) } },
    { "total_copies_1_2", { const_cast<uint32_t(*)>(&Pre_ctr48_total_copies_base->total_copies_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_ctr48_xid_prunes_map: public RegisterMapper {
  static constexpr PTR_Pre_ctr48_xid_prunes Pre_ctr48_xid_prunes_base=0;
  Pre_ctr48_xid_prunes_map() : RegisterMapper( {
    { "xid_prunes_0_2", { const_cast<uint32_t(*)>(&Pre_ctr48_xid_prunes_base->xid_prunes_0_2), 0, {}, sizeof(uint32_t) } },
    { "xid_prunes_1_2", { const_cast<uint32_t(*)>(&Pre_ctr48_xid_prunes_base->xid_prunes_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_ctr48_yid_prunes_map: public RegisterMapper {
  static constexpr PTR_Pre_ctr48_yid_prunes Pre_ctr48_yid_prunes_base=0;
  Pre_ctr48_yid_prunes_map() : RegisterMapper( {
    { "yid_prunes_0_2", { const_cast<uint32_t(*)>(&Pre_ctr48_yid_prunes_base->yid_prunes_0_2), 0, {}, sizeof(uint32_t) } },
    { "yid_prunes_1_2", { const_cast<uint32_t(*)>(&Pre_ctr48_yid_prunes_base->yid_prunes_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_ctr48_first_map: public RegisterMapper {
  static constexpr PTR_Pre_ctr48_first Pre_ctr48_first_base=0;
  Pre_ctr48_first_map() : RegisterMapper( {
    { "first_copies_0_2", { const_cast<uint32_t(*)>(&Pre_ctr48_first_base->first_copies_0_2), 0, {}, sizeof(uint32_t) } },
    { "first_copies_1_2", { const_cast<uint32_t(*)>(&Pre_ctr48_first_base->first_copies_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_port_vector_map: public RegisterMapper {
  static constexpr PTR_Pre_port_vector Pre_port_vector_base=0;
  Pre_port_vector_map() : RegisterMapper( {
    { "filtered_port_vector_0_3", { const_cast<uint32_t(*)>(&Pre_port_vector_base->filtered_port_vector_0_3), 0, {}, sizeof(uint32_t) } },
    { "filtered_port_vector_1_3", { const_cast<uint32_t(*)>(&Pre_port_vector_base->filtered_port_vector_1_3), 0, {}, sizeof(uint32_t) } },
    { "filtered_port_vector_2_3", { const_cast<uint32_t(*)>(&Pre_port_vector_base->filtered_port_vector_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_ctr48_ph_lost_map: public RegisterMapper {
  static constexpr PTR_Pre_ctr48_ph_lost Pre_ctr48_ph_lost_base=0;
  Pre_ctr48_ph_lost_map() : RegisterMapper( {
    { "ph_lost_0_2", { const_cast<uint32_t(*)>(&Pre_ctr48_ph_lost_base->ph_lost_0_2), 0, {}, sizeof(uint32_t) } },
    { "ph_lost_1_2", { const_cast<uint32_t(*)>(&Pre_ctr48_ph_lost_base->ph_lost_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_ctr48_packet_drop_map: public RegisterMapper {
  static constexpr PTR_Pre_ctr48_packet_drop Pre_ctr48_packet_drop_base=0;
  Pre_ctr48_packet_drop_map() : RegisterMapper( {
    { "packet_drop_0_2", { const_cast<uint32_t(*)>(&Pre_ctr48_packet_drop_base->packet_drop_0_2), 0, {}, sizeof(uint32_t) } },
    { "packet_drop_1_2", { const_cast<uint32_t(*)>(&Pre_ctr48_packet_drop_base->packet_drop_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_max_l1_node_log_map: public RegisterMapper {
  static constexpr PTR_Pre_max_l1_node_log Pre_max_l1_node_log_base=0;
  Pre_max_l1_node_log_map() : RegisterMapper( {
    { "max_l1_node_log_0_2", { const_cast<uint32_t(*)>(&Pre_max_l1_node_log_base->max_l1_node_log_0_2), 0, {}, sizeof(uint32_t) } },
    { "max_l1_node_log_1_2", { const_cast<uint32_t(*)>(&Pre_max_l1_node_log_base->max_l1_node_log_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_max_l2_node_log_map: public RegisterMapper {
  static constexpr PTR_Pre_max_l2_node_log Pre_max_l2_node_log_base=0;
  Pre_max_l2_node_log_map() : RegisterMapper( {
    { "max_l2_node_log_0_2", { const_cast<uint32_t(*)>(&Pre_max_l2_node_log_base->max_l2_node_log_0_2), 0, {}, sizeof(uint32_t) } },
    { "max_l2_node_log_1_2", { const_cast<uint32_t(*)>(&Pre_max_l2_node_log_base->max_l2_node_log_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_illegal_l1_node_log_map: public RegisterMapper {
  static constexpr PTR_Pre_illegal_l1_node_log Pre_illegal_l1_node_log_base=0;
  Pre_illegal_l1_node_log_map() : RegisterMapper( {
    { "illegal_l1_node_log_0_2", { const_cast<uint32_t(*)>(&Pre_illegal_l1_node_log_base->illegal_l1_node_log_0_2), 0, {}, sizeof(uint32_t) } },
    { "illegal_l1_node_log_1_2", { const_cast<uint32_t(*)>(&Pre_illegal_l1_node_log_base->illegal_l1_node_log_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_illegal_l2_node_log_map: public RegisterMapper {
  static constexpr PTR_Pre_illegal_l2_node_log Pre_illegal_l2_node_log_base=0;
  Pre_illegal_l2_node_log_map() : RegisterMapper( {
    { "illegal_l2_node_log_0_2", { const_cast<uint32_t(*)>(&Pre_illegal_l2_node_log_base->illegal_l2_node_log_0_2), 0, {}, sizeof(uint32_t) } },
    { "illegal_l2_node_log_1_2", { const_cast<uint32_t(*)>(&Pre_illegal_l2_node_log_base->illegal_l2_node_log_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_pre_pipe_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_pre_pipe_rspec Tm_pre_pipe_rspec_base=0;
  Tm_pre_pipe_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "ctrl", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "arb_ctrl", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->arb_ctrl), 0, {}, sizeof(uint32_t) } },
    { "wrr_ctrl", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->wrr_ctrl), 0, {}, sizeof(uint32_t) } },
    { "fifo_depth", { const_cast<uint32_t(*)[0x4]>(&Tm_pre_pipe_rspec_base->fifo_depth), 0, {0x4}, sizeof(uint32_t) } },
    { "max_l1_node_ctrl", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->max_l1_node_ctrl), 0, {}, sizeof(uint32_t) } },
    { "max_l2_node_ctrl", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->max_l2_node_ctrl), 0, {}, sizeof(uint32_t) } },
    { "rdm_ctrl", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->rdm_ctrl), 0, {}, sizeof(uint32_t) } },
    { "filter_ctrl", { const_cast<Pre_filter_ctrl(*)>(&Tm_pre_pipe_rspec_base->filter_ctrl), new Pre_filter_ctrl_map, {}, sizeof(Pre_filter_ctrl) } },
    { "filter_mask", { const_cast<Pre_filter_mask(*)>(&Tm_pre_pipe_rspec_base->filter_mask), new Pre_filter_mask_map, {}, sizeof(Pre_filter_mask) } },
    { "rdm_addr_ctrl", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->rdm_addr_ctrl), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<Tm_pre_pipe_rspec_ecc(*)>(&Tm_pre_pipe_rspec_base->ecc), new Tm_pre_pipe_rspec_ecc_map, {}, sizeof(Tm_pre_pipe_rspec_ecc) } },
    { "intr", { &Tm_pre_pipe_rspec_base->intr, new Tm_pre_pipe_rspec_intr_map, {}, sizeof(Tm_pre_pipe_rspec_intr) } },
    { "fifo_ph_count", { const_cast<uint32_t(*)[0x4]>(&Tm_pre_pipe_rspec_base->fifo_ph_count), 0, {0x4}, sizeof(uint32_t) } },
    { "table_ph_count", { const_cast<uint32_t(*)[0x2]>(&Tm_pre_pipe_rspec_base->table_ph_count), 0, {0x2}, sizeof(uint32_t) } },
    { "cpu_copies", { const_cast<Pre_ctr48_cpu_copies(*)>(&Tm_pre_pipe_rspec_base->cpu_copies), new Pre_ctr48_cpu_copies_map, {}, sizeof(Pre_ctr48_cpu_copies) } },
    { "ph_processed", { const_cast<Pre_ctr48_ph_processed(*)>(&Tm_pre_pipe_rspec_base->ph_processed), new Pre_ctr48_ph_processed_map, {}, sizeof(Pre_ctr48_ph_processed) } },
    { "total_copies", { const_cast<Pre_ctr48_total_copies(*)>(&Tm_pre_pipe_rspec_base->total_copies), new Pre_ctr48_total_copies_map, {}, sizeof(Pre_ctr48_total_copies) } },
    { "xid_prunes", { const_cast<Pre_ctr48_xid_prunes(*)>(&Tm_pre_pipe_rspec_base->xid_prunes), new Pre_ctr48_xid_prunes_map, {}, sizeof(Pre_ctr48_xid_prunes) } },
    { "yid_prunes", { const_cast<Pre_ctr48_yid_prunes(*)>(&Tm_pre_pipe_rspec_base->yid_prunes), new Pre_ctr48_yid_prunes_map, {}, sizeof(Pre_ctr48_yid_prunes) } },
    { "first_copies", { const_cast<Pre_ctr48_first(*)>(&Tm_pre_pipe_rspec_base->first_copies), new Pre_ctr48_first_map, {}, sizeof(Pre_ctr48_first) } },
    { "filtered_ph_processed", { const_cast<Pre_ctr48_ph_processed(*)>(&Tm_pre_pipe_rspec_base->filtered_ph_processed), new Pre_ctr48_ph_processed_map, {}, sizeof(Pre_ctr48_ph_processed) } },
    { "filtered_total_copies", { const_cast<Pre_ctr48_total_copies(*)>(&Tm_pre_pipe_rspec_base->filtered_total_copies), new Pre_ctr48_total_copies_map, {}, sizeof(Pre_ctr48_total_copies) } },
    { "filtered_xid_prunes", { const_cast<Pre_ctr48_xid_prunes(*)>(&Tm_pre_pipe_rspec_base->filtered_xid_prunes), new Pre_ctr48_xid_prunes_map, {}, sizeof(Pre_ctr48_xid_prunes) } },
    { "filtered_yid_prunes", { const_cast<Pre_ctr48_yid_prunes(*)>(&Tm_pre_pipe_rspec_base->filtered_yid_prunes), new Pre_ctr48_yid_prunes_map, {}, sizeof(Pre_ctr48_yid_prunes) } },
    { "filtered_port_vector", { const_cast<Pre_port_vector(*)>(&Tm_pre_pipe_rspec_base->filtered_port_vector), new Pre_port_vector_map, {}, sizeof(Pre_port_vector) } },
    { "rdm_ph_log", { const_cast<uint32_t(*)[0xa]>(&Tm_pre_pipe_rspec_base->rdm_ph_log), 0, {0xa}, sizeof(uint32_t) } },
    { "ph_lost", { const_cast<Pre_ctr48_ph_lost(*)>(&Tm_pre_pipe_rspec_base->ph_lost), new Pre_ctr48_ph_lost_map, {}, sizeof(Pre_ctr48_ph_lost) } },
    { "packet_drop", { const_cast<Pre_ctr48_packet_drop(*)>(&Tm_pre_pipe_rspec_base->packet_drop), new Pre_ctr48_packet_drop_map, {}, sizeof(Pre_ctr48_packet_drop) } },
    { "max_l1_node_log", { const_cast<Pre_max_l1_node_log(*)>(&Tm_pre_pipe_rspec_base->max_l1_node_log), new Pre_max_l1_node_log_map, {}, sizeof(Pre_max_l1_node_log) } },
    { "max_l2_node_log", { const_cast<Pre_max_l2_node_log(*)>(&Tm_pre_pipe_rspec_base->max_l2_node_log), new Pre_max_l2_node_log_map, {}, sizeof(Pre_max_l2_node_log) } },
    { "illegal_l1_node_log", { const_cast<Pre_illegal_l1_node_log(*)>(&Tm_pre_pipe_rspec_base->illegal_l1_node_log), new Pre_illegal_l1_node_log_map, {}, sizeof(Pre_illegal_l1_node_log) } },
    { "illegal_l2_node_log", { const_cast<Pre_illegal_l2_node_log(*)>(&Tm_pre_pipe_rspec_base->illegal_l2_node_log), new Pre_illegal_l2_node_log_map, {}, sizeof(Pre_illegal_l2_node_log) } },
    { "sbe_log", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->sbe_log), 0, {}, sizeof(uint32_t) } },
    { "mbe_log", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->mbe_log), 0, {}, sizeof(uint32_t) } },
    { "pre_ififo_ctrl0", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->pre_ififo_ctrl0), 0, {}, sizeof(uint32_t) } },
    { "pre_ififo_ctrl1", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->pre_ififo_ctrl1), 0, {}, sizeof(uint32_t) } },
    { "pre_ififo_ctrl2", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->pre_ififo_ctrl2), 0, {}, sizeof(uint32_t) } },
    { "pre_ififo_ctrl3", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->pre_ififo_ctrl3), 0, {}, sizeof(uint32_t) } },
    { "pre_ififo_stats", { const_cast<uint32_t(*)[0x4]>(&Tm_pre_pipe_rspec_base->pre_ififo_stats), 0, {0x4}, sizeof(uint32_t) } },
    { "credit_log", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->credit_log), 0, {}, sizeof(uint32_t) } },
    { "debug", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->debug), 0, {}, sizeof(uint32_t) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_rdm_blk_id_map: public RegisterMapper {
  static constexpr PTR_Pre_rdm_blk_id Pre_rdm_blk_id_base=0;
  Pre_rdm_blk_id_map() : RegisterMapper( {
    { "blk_id_0_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_0_16), 0, {}, sizeof(uint32_t) } },
    { "blk_id_1_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_1_16), 0, {}, sizeof(uint32_t) } },
    { "blk_id_2_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_2_16), 0, {}, sizeof(uint32_t) } },
    { "blk_id_3_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_3_16), 0, {}, sizeof(uint32_t) } },
    { "blk_id_4_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_4_16), 0, {}, sizeof(uint32_t) } },
    { "blk_id_5_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_5_16), 0, {}, sizeof(uint32_t) } },
    { "blk_id_6_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_6_16), 0, {}, sizeof(uint32_t) } },
    { "blk_id_7_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_7_16), 0, {}, sizeof(uint32_t) } },
    { "blk_id_8_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_8_16), 0, {}, sizeof(uint32_t) } },
    { "blk_id_9_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_9_16), 0, {}, sizeof(uint32_t) } },
    { "blk_id_10_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_10_16), 0, {}, sizeof(uint32_t) } },
    { "blk_id_11_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_11_16), 0, {}, sizeof(uint32_t) } },
    { "blk_id_12_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_12_16), 0, {}, sizeof(uint32_t) } },
    { "blk_id_13_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_13_16), 0, {}, sizeof(uint32_t) } },
    { "blk_id_14_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_14_16), 0, {}, sizeof(uint32_t) } },
    { "blk_id_15_16", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_15_16), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_port_mask_map: public RegisterMapper {
  static constexpr PTR_Pre_port_mask Pre_port_mask_base=0;
  Pre_port_mask_map() : RegisterMapper( {
    { "port_mask_0_9", { const_cast<uint32_t(*)>(&Pre_port_mask_base->port_mask_0_9), 0, {}, sizeof(uint32_t) } },
    { "port_mask_1_9", { const_cast<uint32_t(*)>(&Pre_port_mask_base->port_mask_1_9), 0, {}, sizeof(uint32_t) } },
    { "port_mask_2_9", { const_cast<uint32_t(*)>(&Pre_port_mask_base->port_mask_2_9), 0, {}, sizeof(uint32_t) } },
    { "port_mask_3_9", { const_cast<uint32_t(*)>(&Pre_port_mask_base->port_mask_3_9), 0, {}, sizeof(uint32_t) } },
    { "port_mask_4_9", { const_cast<uint32_t(*)>(&Pre_port_mask_base->port_mask_4_9), 0, {}, sizeof(uint32_t) } },
    { "port_mask_5_9", { const_cast<uint32_t(*)>(&Pre_port_mask_base->port_mask_5_9), 0, {}, sizeof(uint32_t) } },
    { "port_mask_6_9", { const_cast<uint32_t(*)>(&Pre_port_mask_base->port_mask_6_9), 0, {}, sizeof(uint32_t) } },
    { "port_mask_7_9", { const_cast<uint32_t(*)>(&Pre_port_mask_base->port_mask_7_9), 0, {}, sizeof(uint32_t) } },
    { "port_mask_8_9", { const_cast<uint32_t(*)>(&Pre_port_mask_base->port_mask_8_9), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_port_down_map: public RegisterMapper {
  static constexpr PTR_Pre_port_down Pre_port_down_base=0;
  Pre_port_down_map() : RegisterMapper( {
    { "port_down_0_9", { const_cast<uint32_t(*)>(&Pre_port_down_base->port_down_0_9), 0, {}, sizeof(uint32_t) } },
    { "port_down_1_9", { const_cast<uint32_t(*)>(&Pre_port_down_base->port_down_1_9), 0, {}, sizeof(uint32_t) } },
    { "port_down_2_9", { const_cast<uint32_t(*)>(&Pre_port_down_base->port_down_2_9), 0, {}, sizeof(uint32_t) } },
    { "port_down_3_9", { const_cast<uint32_t(*)>(&Pre_port_down_base->port_down_3_9), 0, {}, sizeof(uint32_t) } },
    { "port_down_4_9", { const_cast<uint32_t(*)>(&Pre_port_down_base->port_down_4_9), 0, {}, sizeof(uint32_t) } },
    { "port_down_5_9", { const_cast<uint32_t(*)>(&Pre_port_down_base->port_down_5_9), 0, {}, sizeof(uint32_t) } },
    { "port_down_6_9", { const_cast<uint32_t(*)>(&Pre_port_down_base->port_down_6_9), 0, {}, sizeof(uint32_t) } },
    { "port_down_7_9", { const_cast<uint32_t(*)>(&Pre_port_down_base->port_down_7_9), 0, {}, sizeof(uint32_t) } },
    { "port_down_8_9", { const_cast<uint32_t(*)>(&Pre_port_down_base->port_down_8_9), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_pre_common_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_pre_common_rspec Tm_pre_common_rspec_base=0;
  Tm_pre_common_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Tm_pre_common_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Tm_pre_common_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "rdm_dft_csr", { const_cast<uint32_t(*)>(&Tm_pre_common_rspec_base->rdm_dft_csr), 0, {}, sizeof(uint32_t) } },
    { "common_ctrl", { const_cast<uint32_t(*)>(&Tm_pre_common_rspec_base->common_ctrl), 0, {}, sizeof(uint32_t) } },
    { "prune_rid", { const_cast<uint32_t(*)>(&Tm_pre_common_rspec_base->prune_rid), 0, {}, sizeof(uint32_t) } },
    { "blk_id", { const_cast<Pre_rdm_blk_id(*)>(&Tm_pre_common_rspec_base->blk_id), new Pre_rdm_blk_id_map, {}, sizeof(Pre_rdm_blk_id) } },
    { "port_mask", { const_cast<Pre_port_mask(*)[0x2]>(&Tm_pre_common_rspec_base->port_mask), new Pre_port_mask_map, {0x2}, sizeof(Pre_port_mask) } },
    { "port_down", { const_cast<Pre_port_down(*)>(&Tm_pre_common_rspec_base->port_down), new Pre_port_down_map, {}, sizeof(Pre_port_down) } },
    { "pipe_int_status", { const_cast<uint32_t(*)>(&Tm_pre_common_rspec_base->pipe_int_status), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_pre_top_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_pre_top_rspec Tm_pre_top_rspec_base=0;
  Tm_pre_top_rspec_map() : RegisterMapper( {
    { "pre", { &Tm_pre_top_rspec_base->pre, new Tm_pre_pipe_rspec_map, {0x4}, sizeof(Tm_pre_pipe_rspec) } },
    { "pre_common", { &Tm_pre_top_rspec_base->pre_common, new Tm_pre_common_rspec_map, {}, sizeof(Tm_pre_common_rspec) } }
    } )
  {}
};

struct Tm_psc_pipe_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_psc_pipe_rspec_intr Tm_psc_pipe_rspec_intr_base=0;
  Tm_psc_pipe_rspec_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Psc_psm_indir_access_data_r_map: public RegisterMapper {
  static constexpr PTR_Psc_psm_indir_access_data_r Psc_psm_indir_access_data_r_base=0;
  Psc_psm_indir_access_data_r_map() : RegisterMapper( {
    { "psm_indir_access_data_0_8", { const_cast<uint32_t(*)>(&Psc_psm_indir_access_data_r_base->psm_indir_access_data_0_8), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_data_1_8", { const_cast<uint32_t(*)>(&Psc_psm_indir_access_data_r_base->psm_indir_access_data_1_8), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_data_2_8", { const_cast<uint32_t(*)>(&Psc_psm_indir_access_data_r_base->psm_indir_access_data_2_8), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_data_3_8", { const_cast<uint32_t(*)>(&Psc_psm_indir_access_data_r_base->psm_indir_access_data_3_8), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_data_4_8", { const_cast<uint32_t(*)>(&Psc_psm_indir_access_data_r_base->psm_indir_access_data_4_8), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_data_5_8", { const_cast<uint32_t(*)>(&Psc_psm_indir_access_data_r_base->psm_indir_access_data_5_8), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_data_6_8", { const_cast<uint32_t(*)>(&Psc_psm_indir_access_data_r_base->psm_indir_access_data_6_8), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_data_7_8", { const_cast<uint32_t(*)>(&Psc_psm_indir_access_data_r_base->psm_indir_access_data_7_8), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_psc_pipe_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_psc_pipe_rspec Tm_psc_pipe_rspec_base=0;
  Tm_psc_pipe_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "psc_ph_used", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_base->psc_ph_used), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_base->ecc), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Tm_psc_pipe_rspec_base->intr, new Tm_psc_pipe_rspec_intr_map, {}, sizeof(Tm_psc_pipe_rspec_intr) } },
    { "psm_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_base->psm_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "psm_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_base->psm_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_addr", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_base->psm_indir_access_addr), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_data", { const_cast<Psc_psm_indir_access_data_r(*)>(&Tm_psc_pipe_rspec_base->psm_indir_access_data), new Psc_psm_indir_access_data_r_map, {}, sizeof(Psc_psm_indir_access_data_r) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Psc_block_valid_r_map: public RegisterMapper {
  static constexpr PTR_Psc_block_valid_r Psc_block_valid_r_base=0;
  Psc_block_valid_r_map() : RegisterMapper( {
    { "block_valid_0_3", { const_cast<uint32_t(*)>(&Psc_block_valid_r_base->block_valid_0_3), 0, {}, sizeof(uint32_t) } },
    { "block_valid_1_3", { const_cast<uint32_t(*)>(&Psc_block_valid_r_base->block_valid_1_3), 0, {}, sizeof(uint32_t) } },
    { "block_valid_2_3", { const_cast<uint32_t(*)>(&Psc_block_valid_r_base->block_valid_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Psc_block_reset_r_map: public RegisterMapper {
  static constexpr PTR_Psc_block_reset_r Psc_block_reset_r_base=0;
  Psc_block_reset_r_map() : RegisterMapper( {
    { "block_reset_0_3", { const_cast<uint32_t(*)>(&Psc_block_reset_r_base->block_reset_0_3), 0, {}, sizeof(uint32_t) } },
    { "block_reset_1_3", { const_cast<uint32_t(*)>(&Psc_block_reset_r_base->block_reset_1_3), 0, {}, sizeof(uint32_t) } },
    { "block_reset_2_3", { const_cast<uint32_t(*)>(&Psc_block_reset_r_base->block_reset_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Psc_block_enable_r_map: public RegisterMapper {
  static constexpr PTR_Psc_block_enable_r Psc_block_enable_r_base=0;
  Psc_block_enable_r_map() : RegisterMapper( {
    { "block_enable_0_3", { const_cast<uint32_t(*)>(&Psc_block_enable_r_base->block_enable_0_3), 0, {}, sizeof(uint32_t) } },
    { "block_enable_1_3", { const_cast<uint32_t(*)>(&Psc_block_enable_r_base->block_enable_1_3), 0, {}, sizeof(uint32_t) } },
    { "block_enable_2_3", { const_cast<uint32_t(*)>(&Psc_block_enable_r_base->block_enable_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Psc_block_ready_r_map: public RegisterMapper {
  static constexpr PTR_Psc_block_ready_r Psc_block_ready_r_base=0;
  Psc_block_ready_r_map() : RegisterMapper( {
    { "block_ready_0_3", { const_cast<uint32_t(*)>(&Psc_block_ready_r_base->block_ready_0_3), 0, {}, sizeof(uint32_t) } },
    { "block_ready_1_3", { const_cast<uint32_t(*)>(&Psc_block_ready_r_base->block_ready_1_3), 0, {}, sizeof(uint32_t) } },
    { "block_ready_2_3", { const_cast<uint32_t(*)>(&Psc_block_ready_r_base->block_ready_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Psc_epipe_pkt_dropcnt_r_map: public RegisterMapper {
  static constexpr PTR_Psc_epipe_pkt_dropcnt_r Psc_epipe_pkt_dropcnt_r_base=0;
  Psc_epipe_pkt_dropcnt_r_map() : RegisterMapper( {
    { "pkt_dropcnt_0_2", { const_cast<uint32_t(*)>(&Psc_epipe_pkt_dropcnt_r_base->pkt_dropcnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "pkt_dropcnt_1_2", { const_cast<uint32_t(*)>(&Psc_epipe_pkt_dropcnt_r_base->pkt_dropcnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Psc_epipe_g_map: public RegisterMapper {
  static constexpr PTR_Psc_epipe_g Psc_epipe_g_base=0;
  Psc_epipe_g_map() : RegisterMapper( {
    { "enable", { const_cast<uint32_t(*)>(&Psc_epipe_g_base->enable), 0, {}, sizeof(uint32_t) } },
    { "blks_usecnt", { const_cast<uint32_t(*)>(&Psc_epipe_g_base->blks_usecnt), 0, {}, sizeof(uint32_t) } },
    { "blks_max_usecnt", { const_cast<uint32_t(*)>(&Psc_epipe_g_base->blks_max_usecnt), 0, {}, sizeof(uint32_t) } },
    { "pkt_dropcnt", { const_cast<Psc_epipe_pkt_dropcnt_r(*)>(&Psc_epipe_g_base->pkt_dropcnt), new Psc_epipe_pkt_dropcnt_r_map, {}, sizeof(Psc_epipe_pkt_dropcnt_r) } }
    } )
  {}
};

struct Psc_block_g_map: public RegisterMapper {
  static constexpr PTR_Psc_block_g Psc_block_g_base=0;
  Psc_block_g_map() : RegisterMapper( {
    { "state", { const_cast<uint32_t(*)>(&Psc_block_g_base->state), 0, {}, sizeof(uint32_t) } },
    { "addr_usecnt", { const_cast<uint32_t(*)>(&Psc_block_g_base->addr_usecnt), 0, {}, sizeof(uint32_t) } },
    { "addr_max_usecnt", { const_cast<uint32_t(*)>(&Psc_block_g_base->addr_max_usecnt), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_psc_common_rspec_ecc_map: public RegisterMapper {
  static constexpr PTR_Tm_psc_common_rspec_ecc Tm_psc_common_rspec_ecc_base=0;
  Tm_psc_common_rspec_ecc_map() : RegisterMapper( {
    { "ecc_0_12", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_0_12), 0, {}, sizeof(uint32_t) } },
    { "ecc_1_12", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_1_12), 0, {}, sizeof(uint32_t) } },
    { "ecc_2_12", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_2_12), 0, {}, sizeof(uint32_t) } },
    { "ecc_3_12", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_3_12), 0, {}, sizeof(uint32_t) } },
    { "ecc_4_12", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_4_12), 0, {}, sizeof(uint32_t) } },
    { "ecc_5_12", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_5_12), 0, {}, sizeof(uint32_t) } },
    { "ecc_6_12", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_6_12), 0, {}, sizeof(uint32_t) } },
    { "ecc_7_12", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_7_12), 0, {}, sizeof(uint32_t) } },
    { "ecc_8_12", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_8_12), 0, {}, sizeof(uint32_t) } },
    { "ecc_9_12", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_9_12), 0, {}, sizeof(uint32_t) } },
    { "ecc_10_12", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_10_12), 0, {}, sizeof(uint32_t) } },
    { "ecc_11_12", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_11_12), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_psc_common_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_psc_common_rspec_intr Tm_psc_common_rspec_intr_base=0;
  Tm_psc_common_rspec_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_psc_common_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_psc_common_rspec Tm_psc_common_rspec_base=0;
  Tm_psc_common_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "psm_dft_csr", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->psm_dft_csr), 0, {}, sizeof(uint32_t) } },
    { "psm_org", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->psm_org), 0, {}, sizeof(uint32_t) } },
    { "ctrl", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "timestamp_shift", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->timestamp_shift), 0, {}, sizeof(uint32_t) } },
    { "full_threshold", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->full_threshold), 0, {}, sizeof(uint32_t) } },
    { "hyst_threshold", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->hyst_threshold), 0, {}, sizeof(uint32_t) } },
    { "ts_offset", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->ts_offset), 0, {}, sizeof(uint32_t) } },
    { "block_valid", { const_cast<Psc_block_valid_r(*)>(&Tm_psc_common_rspec_base->block_valid), new Psc_block_valid_r_map, {}, sizeof(Psc_block_valid_r) } },
    { "block_reset", { const_cast<Psc_block_reset_r(*)>(&Tm_psc_common_rspec_base->block_reset), new Psc_block_reset_r_map, {}, sizeof(Psc_block_reset_r) } },
    { "block_enable", { const_cast<Psc_block_enable_r(*)>(&Tm_psc_common_rspec_base->block_enable), new Psc_block_enable_r_map, {}, sizeof(Psc_block_enable_r) } },
    { "block_ready", { const_cast<Psc_block_ready_r(*)>(&Tm_psc_common_rspec_base->block_ready), new Psc_block_ready_r_map, {}, sizeof(Psc_block_ready_r) } },
    { "blocks_freecnt", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->blocks_freecnt), 0, {}, sizeof(uint32_t) } },
    { "epipe", { &Tm_psc_common_rspec_base->epipe, new Psc_epipe_g_map, {0x4}, sizeof(Psc_epipe_g) } },
    { "block", { &Tm_psc_common_rspec_base->block, new Psc_block_g_map, {0x60}, sizeof(Psc_block_g) } },
    { "psc_bank_ctrl_r", { const_cast<uint32_t(*)[0x180]>(&Tm_psc_common_rspec_base->psc_bank_ctrl_r), 0, {0x180}, sizeof(uint32_t) } },
    { "lmem_indir_access_addr", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->lmem_indir_access_addr), 0, {}, sizeof(uint32_t) } },
    { "lmem_indir_access_data", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->lmem_indir_access_data), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<Tm_psc_common_rspec_ecc(*)>(&Tm_psc_common_rspec_base->ecc), new Tm_psc_common_rspec_ecc_map, {}, sizeof(Tm_psc_common_rspec_ecc) } },
    { "intr", { &Tm_psc_common_rspec_base->intr, new Tm_psc_common_rspec_intr_map, {}, sizeof(Tm_psc_common_rspec_intr) } },
    { "linkmem_sbe_err_log", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->linkmem_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "linkmem_mbe_err_log", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->linkmem_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "overflow_err_log", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->overflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "underflow_err_log", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->underflow_err_log), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_psc_top_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_psc_top_rspec Tm_psc_top_rspec_base=0;
  Tm_psc_top_rspec_map() : RegisterMapper( {
    { "psc", { &Tm_psc_top_rspec_base->psc, new Tm_psc_pipe_rspec_map, {0x4}, sizeof(Tm_psc_pipe_rspec) } },
    { "psc_common", { &Tm_psc_top_rspec_base->psc_common, new Tm_psc_common_rspec_map, {}, sizeof(Tm_psc_common_rspec) } }
    } )
  {}
};

struct Tm_top_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_top_rspec Tm_top_rspec_base=0;
  Tm_top_rspec_map() : RegisterMapper( {
    { "tm_wac_top", { &Tm_top_rspec_base->tm_wac_top, new Tm_wac_top_rspec_map, {}, sizeof(Tm_wac_top_rspec) } },
    { "tm_caa_top", { &Tm_top_rspec_base->tm_caa_top, new Tm_caa_top_rspec_map, {}, sizeof(Tm_caa_top_rspec) } },
    { "tm_qac_top", { &Tm_top_rspec_base->tm_qac_top, new Tm_qac_top_rspec_map, {}, sizeof(Tm_qac_top_rspec) } },
    { "tm_scha_top", { &Tm_top_rspec_base->tm_scha_top, new Tm_sch_top_rspec_map, {}, sizeof(Tm_sch_top_rspec) } },
    { "tm_schb_top", { &Tm_top_rspec_base->tm_schb_top, new Tm_sch_top_rspec_map, {}, sizeof(Tm_sch_top_rspec) } },
    { "tm_clc_top", { &Tm_top_rspec_base->tm_clc_top, new Tm_clc_top_rspec_map, {}, sizeof(Tm_clc_top_rspec) } },
    { "tm_pex_top", { &Tm_top_rspec_base->tm_pex_top, new Tm_pex_top_rspec_map, {}, sizeof(Tm_pex_top_rspec) } },
    { "tm_qlc_top", { &Tm_top_rspec_base->tm_qlc_top, new Tm_qlc_top_rspec_map, {}, sizeof(Tm_qlc_top_rspec) } },
    { "tm_prc_top", { &Tm_top_rspec_base->tm_prc_top, new Tm_prc_top_rspec_map, {}, sizeof(Tm_prc_top_rspec) } },
    { "tm_pre_top", { &Tm_top_rspec_base->tm_pre_top, new Tm_pre_top_rspec_map, {}, sizeof(Tm_pre_top_rspec) } },
    { "tm_psc_top", { &Tm_top_rspec_base->tm_psc_top, new Tm_psc_top_rspec_map, {}, sizeof(Tm_psc_top_rspec) } }
    } )
  {}
};

struct Dvsl_addrmap_map: public RegisterMapper {
  static constexpr PTR_Dvsl_addrmap Dvsl_addrmap_base=0;
  Dvsl_addrmap_map() : RegisterMapper( {
    { "pcie_bar01_regs", { &Dvsl_addrmap_base->pcie_bar01_regs, new Pcie_bar01_regs_map, {}, sizeof(Pcie_bar01_regs) } },
    { "misc_regs", { &Dvsl_addrmap_base->misc_regs, new Misc_regs_map, {}, sizeof(Misc_regs) } },
    { "misc_tv80_regs", { &Dvsl_addrmap_base->misc_tv80_regs, new Misc_tv80_rspec_map, {}, sizeof(Misc_tv80_rspec) } },
    { "mbc", { &Dvsl_addrmap_base->mbc, new Mbc_rspec_map, {}, sizeof(Mbc_rspec) } },
    { "pbc", { &Dvsl_addrmap_base->pbc, new Pbc_rspec_map, {}, sizeof(Pbc_rspec) } },
    { "cbc", { &Dvsl_addrmap_base->cbc, new Cbc_rspec_map, {}, sizeof(Cbc_rspec) } },
    { "tbc", { &Dvsl_addrmap_base->tbc, new Tbc_rspec_map, {}, sizeof(Tbc_rspec) } },
    { "lfltr", { &Dvsl_addrmap_base->lfltr, new Lfltr_rspec_map, {0x4}, sizeof(Lfltr_rspec) } },
    { "tm_top", { &Dvsl_addrmap_base->tm_top, new Tm_top_rspec_map, {}, sizeof(Tm_top_rspec) } }
    } )
  {}
};

struct Umac3_addrmap_map: public RegisterMapper {
  static constexpr PTR_Umac3_addrmap Umac3_addrmap_base=0;
  Umac3_addrmap_map() : RegisterMapper( {
    { "dummy_register", { const_cast<uint32_t(*)[0x7f00]>(&Umac3_addrmap_base->dummy_register), 0, {0x7f00}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth100g_reg_rspec_eth_onestep_ets_offset_ctrl_map: public RegisterMapper {
  static constexpr PTR_Eth100g_reg_rspec_eth_onestep_ets_offset_ctrl Eth100g_reg_rspec_eth_onestep_ets_offset_ctrl_base=0;
  Eth100g_reg_rspec_eth_onestep_ets_offset_ctrl_map() : RegisterMapper( {
    { "eth_onestep_ets_offset_ctrl_0_2", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_eth_onestep_ets_offset_ctrl_base->eth_onestep_ets_offset_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "eth_onestep_ets_offset_ctrl_1_2", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_eth_onestep_ets_offset_ctrl_base->eth_onestep_ets_offset_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth100g_reg_rspec_eth_mac_ts_offset_ctrl_map: public RegisterMapper {
  static constexpr PTR_Eth100g_reg_rspec_eth_mac_ts_offset_ctrl Eth100g_reg_rspec_eth_mac_ts_offset_ctrl_base=0;
  Eth100g_reg_rspec_eth_mac_ts_offset_ctrl_map() : RegisterMapper( {
    { "eth_mac_ts_offset_ctrl_0_2", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_eth_mac_ts_offset_ctrl_base->eth_mac_ts_offset_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "eth_mac_ts_offset_ctrl_1_2", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_eth_mac_ts_offset_ctrl_base->eth_mac_ts_offset_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth100g_reg_rspec_chnl_intr_map: public RegisterMapper {
  static constexpr PTR_Eth100g_reg_rspec_chnl_intr Eth100g_reg_rspec_chnl_intr_base=0;
  Eth100g_reg_rspec_chnl_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_chnl_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_chnl_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_chnl_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_chnl_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_chnl_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth100g_reg_rspec_mem_intr_map: public RegisterMapper {
  static constexpr PTR_Eth100g_reg_rspec_mem_intr Eth100g_reg_rspec_mem_intr_base=0;
  Eth100g_reg_rspec_mem_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_mem_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_mem_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_mem_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_mem_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_mem_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth100g_reg_rspec_uctrl_intr_map: public RegisterMapper {
  static constexpr PTR_Eth100g_reg_rspec_uctrl_intr Eth100g_reg_rspec_uctrl_intr_base=0;
  Eth100g_reg_rspec_uctrl_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_uctrl_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_uctrl_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_uctrl_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_uctrl_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_uctrl_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth100g_reg_rspec_tv80_intr_map: public RegisterMapper {
  static constexpr PTR_Eth100g_reg_rspec_tv80_intr Eth100g_reg_rspec_tv80_intr_base=0;
  Eth100g_reg_rspec_tv80_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_tv80_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_tv80_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_tv80_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_tv80_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_tv80_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth100g_reg_rspec_map: public RegisterMapper {
  static constexpr PTR_Eth100g_reg_rspec Eth100g_reg_rspec_base=0;
  Eth100g_reg_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)[0x4]>(&Eth100g_reg_rspec_base->scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "eth_soft_reset", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_soft_reset), 0, {}, sizeof(uint32_t) } },
    { "chnl_seq", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->chnl_seq), 0, {}, sizeof(uint32_t) } },
    { "port_alive_lut", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->port_alive_lut), 0, {}, sizeof(uint32_t) } },
    { "eth_clkobs_ctrl", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_clkobs_ctrl), 0, {}, sizeof(uint32_t) } },
    { "eth_onestep_ets_offset_ctrl", { const_cast<Eth100g_reg_rspec_eth_onestep_ets_offset_ctrl(*)[0x4]>(&Eth100g_reg_rspec_base->eth_onestep_ets_offset_ctrl), new Eth100g_reg_rspec_eth_onestep_ets_offset_ctrl_map, {0x4}, sizeof(Eth100g_reg_rspec_eth_onestep_ets_offset_ctrl) } },
    { "eth_mac_ts_offset_ctrl", { const_cast<Eth100g_reg_rspec_eth_mac_ts_offset_ctrl(*)>(&Eth100g_reg_rspec_base->eth_mac_ts_offset_ctrl), new Eth100g_reg_rspec_eth_mac_ts_offset_ctrl_map, {}, sizeof(Eth100g_reg_rspec_eth_mac_ts_offset_ctrl) } },
    { "eth_status", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_status), 0, {}, sizeof(uint32_t) } },
    { "global_intr_stat", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->global_intr_stat), 0, {}, sizeof(uint32_t) } },
    { "txff_ctrl", { const_cast<uint32_t(*)[0x4]>(&Eth100g_reg_rspec_base->txff_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "txff_status", { const_cast<uint32_t(*)[0x4]>(&Eth100g_reg_rspec_base->txff_status), 0, {0x4}, sizeof(uint32_t) } },
    { "txcrc_trunc_ctrl", { const_cast<uint32_t(*)[0x4]>(&Eth100g_reg_rspec_base->txcrc_trunc_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "rxff_ctrl", { const_cast<uint32_t(*)[0x4]>(&Eth100g_reg_rspec_base->rxff_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "rxpkt_err_sts", { const_cast<uint32_t(*)[0x4]>(&Eth100g_reg_rspec_base->rxpkt_err_sts), 0, {0x4}, sizeof(uint32_t) } },
    { "eth_ring_addr", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_ring_addr), 0, {}, sizeof(uint32_t) } },
    { "eth_ring_wdata", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_ring_wdata), 0, {}, sizeof(uint32_t) } },
    { "eth_ring_rdata", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_ring_rdata), 0, {}, sizeof(uint32_t) } },
    { "eth_ring_ctrl", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_ring_ctrl), 0, {}, sizeof(uint32_t) } },
    { "eth_ring_setup", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_ring_setup), 0, {}, sizeof(uint32_t) } },
    { "soft_port_alive", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->soft_port_alive), 0, {}, sizeof(uint32_t) } },
    { "chnl_intr", { &Eth100g_reg_rspec_base->chnl_intr, new Eth100g_reg_rspec_chnl_intr_map, {}, sizeof(Eth100g_reg_rspec_chnl_intr) } },
    { "mem_intr", { &Eth100g_reg_rspec_base->mem_intr, new Eth100g_reg_rspec_mem_intr_map, {}, sizeof(Eth100g_reg_rspec_mem_intr) } },
    { "uctrl_intr", { &Eth100g_reg_rspec_base->uctrl_intr, new Eth100g_reg_rspec_uctrl_intr_map, {}, sizeof(Eth100g_reg_rspec_uctrl_intr) } },
    { "mem_ecc", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->mem_ecc), 0, {}, sizeof(uint32_t) } },
    { "txfifo_sbe_err_log", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->txfifo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "txfifo_mbe_err_log", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->txfifo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "statsmem_sbe_err_log", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->statsmem_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "statsmem_mbe_err_log", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->statsmem_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "tv80mem_sbe_err_log", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->tv80mem_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "tv80mem_mbe_err_log", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->tv80mem_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "mac_en0", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->mac_en0), 0, {}, sizeof(uint32_t) } },
    { "mac_en1", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->mac_en1), 0, {}, sizeof(uint32_t) } },
    { "mac_freeze_enable", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->mac_freeze_enable), 0, {}, sizeof(uint32_t) } },
    { "crcerr_inj", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->crcerr_inj), 0, {}, sizeof(uint32_t) } },
    { "tv80_intr", { &Eth100g_reg_rspec_base->tv80_intr, new Eth100g_reg_rspec_tv80_intr_map, {}, sizeof(Eth100g_reg_rspec_tv80_intr) } },
    { "tv80_debug_ctrl", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->tv80_debug_ctrl), 0, {}, sizeof(uint32_t) } },
    { "tv80_debug_head_ptr", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->tv80_debug_head_ptr), 0, {}, sizeof(uint32_t) } },
    { "tv80_debug_tail_ptr", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->tv80_debug_tail_ptr), 0, {}, sizeof(uint32_t) } },
    { "tv80_stall_on_error", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->tv80_stall_on_error), 0, {}, sizeof(uint32_t) } },
    { "tv80_halted_status", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->tv80_halted_status), 0, {}, sizeof(uint32_t) } },
    { "tv80_watchdog_ctrl", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->tv80_watchdog_ctrl), 0, {}, sizeof(uint32_t) } },
    { "tv80_watchdog_count", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->tv80_watchdog_count), 0, {}, sizeof(uint32_t) } },
    { "eth_rxsigok_ctrl", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_rxsigok_ctrl), 0, {}, sizeof(uint32_t) } },
    { "eth_mdioci_ctrl", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_mdioci_ctrl), 0, {}, sizeof(uint32_t) } },
    { "eth_mdioci_poll_ctrl", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_mdioci_poll_ctrl), 0, {}, sizeof(uint32_t) } },
    { "eth_mdioci_poll_time", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_mdioci_poll_time), 0, {}, sizeof(uint32_t) } },
    { "eth_rxsigok_bitsel", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_rxsigok_bitsel), 0, {}, sizeof(uint32_t) } },
    { "mdioci_intr_stat", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->mdioci_intr_stat), 0, {}, sizeof(uint32_t) } },
    { "mdioci_en0", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->mdioci_en0), 0, {}, sizeof(uint32_t) } },
    { "mdioci_en1", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->mdioci_en1), 0, {}, sizeof(uint32_t) } },
    { "mdioci_freeze_enable", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->mdioci_freeze_enable), 0, {}, sizeof(uint32_t) } },
    { "mac_ctrl", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->mac_ctrl), 0, {}, sizeof(uint32_t) } },
    { "txff_pream0", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->txff_pream0), 0, {}, sizeof(uint32_t) } },
    { "txff_pream1", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->txff_pream1), 0, {}, sizeof(uint32_t) } },
    { "serdes_config", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->serdes_config), 0, {}, sizeof(uint32_t) } },
    { "eth_ppm_sel", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_ppm_sel), 0, {}, sizeof(uint32_t) } },
    { "eth_ppm_ctrl", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_ppm_ctrl), 0, {}, sizeof(uint32_t) } },
    { "eth_ppm_stat", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_ppm_stat), 0, {}, sizeof(uint32_t) } },
    { "eth_mdioci_addr", { const_cast<uint32_t(*)>(&Eth100g_reg_rspec_base->eth_mdioci_addr), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth100g_tv80_reg_map: public RegisterMapper {
  static constexpr PTR_Eth100g_tv80_reg Eth100g_tv80_reg_base=0;
  Eth100g_tv80_reg_map() : RegisterMapper( {
    { "dummy_register", { const_cast<uint32_t(*)[0x1000]>(&Eth100g_tv80_reg_base->dummy_register), 0, {0x1000}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth100g_addrmap_map: public RegisterMapper {
  static constexpr PTR_Eth100g_addrmap Eth100g_addrmap_base=0;
  Eth100g_addrmap_map() : RegisterMapper( {
    { "eth100g_umac3", { &Eth100g_addrmap_base->eth100g_umac3, new Umac3_addrmap_map, {}, sizeof(Umac3_addrmap) } },
    { "eth100g_reg", { &Eth100g_addrmap_base->eth100g_reg, new Eth100g_reg_rspec_map, {}, sizeof(Eth100g_reg_rspec) } },
    { "eth100g_tv80", { &Eth100g_addrmap_base->eth100g_tv80, new Eth100g_tv80_reg_map, {}, sizeof(Eth100g_tv80_reg) } }
    } )
  {}
};

struct Umac4_addrmap_map: public RegisterMapper {
  static constexpr PTR_Umac4_addrmap Umac4_addrmap_base=0;
  Umac4_addrmap_map() : RegisterMapper( {
    { "dummy_register", { const_cast<uint32_t(*)[0x4000]>(&Umac4_addrmap_base->dummy_register), 0, {0x4000}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth400g_mac_rspec_eth_onestep_ets_offset_ctrl_map: public RegisterMapper {
  static constexpr PTR_Eth400g_mac_rspec_eth_onestep_ets_offset_ctrl Eth400g_mac_rspec_eth_onestep_ets_offset_ctrl_base=0;
  Eth400g_mac_rspec_eth_onestep_ets_offset_ctrl_map() : RegisterMapper( {
    { "eth_onestep_ets_offset_ctrl_0_2", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_eth_onestep_ets_offset_ctrl_base->eth_onestep_ets_offset_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "eth_onestep_ets_offset_ctrl_1_2", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_eth_onestep_ets_offset_ctrl_base->eth_onestep_ets_offset_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth400g_mac_rspec_eth_mac_ts_offset_ctrl_map: public RegisterMapper {
  static constexpr PTR_Eth400g_mac_rspec_eth_mac_ts_offset_ctrl Eth400g_mac_rspec_eth_mac_ts_offset_ctrl_base=0;
  Eth400g_mac_rspec_eth_mac_ts_offset_ctrl_map() : RegisterMapper( {
    { "eth_mac_ts_offset_ctrl_0_2", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_eth_mac_ts_offset_ctrl_base->eth_mac_ts_offset_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "eth_mac_ts_offset_ctrl_1_2", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_eth_mac_ts_offset_ctrl_base->eth_mac_ts_offset_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth400g_mac_rspec_cts_fifo_out_map: public RegisterMapper {
  static constexpr PTR_Eth400g_mac_rspec_cts_fifo_out Eth400g_mac_rspec_cts_fifo_out_base=0;
  Eth400g_mac_rspec_cts_fifo_out_map() : RegisterMapper( {
    { "cts_fifo_out_0_2", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_cts_fifo_out_base->cts_fifo_out_0_2), 0, {}, sizeof(uint32_t) } },
    { "cts_fifo_out_1_2", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_cts_fifo_out_base->cts_fifo_out_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth400g_mac_rspec_chnl_intr_map: public RegisterMapper {
  static constexpr PTR_Eth400g_mac_rspec_chnl_intr Eth400g_mac_rspec_chnl_intr_base=0;
  Eth400g_mac_rspec_chnl_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_chnl_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_chnl_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_chnl_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_chnl_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_chnl_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth400g_mac_rspec_cts_intr_map: public RegisterMapper {
  static constexpr PTR_Eth400g_mac_rspec_cts_intr Eth400g_mac_rspec_cts_intr_base=0;
  Eth400g_mac_rspec_cts_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_cts_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_cts_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_cts_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_cts_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_cts_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth400g_mac_rspec_mem_intr_map: public RegisterMapper {
  static constexpr PTR_Eth400g_mac_rspec_mem_intr Eth400g_mac_rspec_mem_intr_base=0;
  Eth400g_mac_rspec_mem_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_mem_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_mem_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_mem_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_mem_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_mem_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth400g_mac_rspec_uctrl_intr_map: public RegisterMapper {
  static constexpr PTR_Eth400g_mac_rspec_uctrl_intr Eth400g_mac_rspec_uctrl_intr_base=0;
  Eth400g_mac_rspec_uctrl_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_uctrl_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_uctrl_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_uctrl_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_uctrl_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_uctrl_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth400g_mac_rspec_txff_ecc_map: public RegisterMapper {
  static constexpr PTR_Eth400g_mac_rspec_txff_ecc Eth400g_mac_rspec_txff_ecc_base=0;
  Eth400g_mac_rspec_txff_ecc_map() : RegisterMapper( {
    { "txff_ecc_0_2", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_txff_ecc_base->txff_ecc_0_2), 0, {}, sizeof(uint32_t) } },
    { "txff_ecc_1_2", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_txff_ecc_base->txff_ecc_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth400g_mac_rspec_tv80_intr_map: public RegisterMapper {
  static constexpr PTR_Eth400g_mac_rspec_tv80_intr Eth400g_mac_rspec_tv80_intr_base=0;
  Eth400g_mac_rspec_tv80_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_tv80_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_tv80_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_tv80_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_tv80_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_tv80_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth400g_mac_rspec_map: public RegisterMapper {
  static constexpr PTR_Eth400g_mac_rspec Eth400g_mac_rspec_base=0;
  Eth400g_mac_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)[0x4]>(&Eth400g_mac_rspec_base->scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "eth_soft_reset", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->eth_soft_reset), 0, {}, sizeof(uint32_t) } },
    { "chnl_seq", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->chnl_seq), 0, {}, sizeof(uint32_t) } },
    { "port_alive_lut", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->port_alive_lut), 0, {}, sizeof(uint32_t) } },
    { "eth_clkobs_ctrl", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->eth_clkobs_ctrl), 0, {}, sizeof(uint32_t) } },
    { "eth_onestep_ets_offset_ctrl", { const_cast<Eth400g_mac_rspec_eth_onestep_ets_offset_ctrl(*)[0x8]>(&Eth400g_mac_rspec_base->eth_onestep_ets_offset_ctrl), new Eth400g_mac_rspec_eth_onestep_ets_offset_ctrl_map, {0x8}, sizeof(Eth400g_mac_rspec_eth_onestep_ets_offset_ctrl) } },
    { "eth_mac_ts_offset_ctrl", { const_cast<Eth400g_mac_rspec_eth_mac_ts_offset_ctrl(*)>(&Eth400g_mac_rspec_base->eth_mac_ts_offset_ctrl), new Eth400g_mac_rspec_eth_mac_ts_offset_ctrl_map, {}, sizeof(Eth400g_mac_rspec_eth_mac_ts_offset_ctrl) } },
    { "eth_status0", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->eth_status0), 0, {}, sizeof(uint32_t) } },
    { "eth_status1", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->eth_status1), 0, {}, sizeof(uint32_t) } },
    { "global_intr_stat", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->global_intr_stat), 0, {}, sizeof(uint32_t) } },
    { "cts_fifo_stat", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->cts_fifo_stat), 0, {}, sizeof(uint32_t) } },
    { "txff_ctrl", { const_cast<uint32_t(*)[0x8]>(&Eth400g_mac_rspec_base->txff_ctrl), 0, {0x8}, sizeof(uint32_t) } },
    { "txff_status", { const_cast<uint32_t(*)[0x8]>(&Eth400g_mac_rspec_base->txff_status), 0, {0x8}, sizeof(uint32_t) } },
    { "txcrc_trunc_ctrl", { const_cast<uint32_t(*)[0x8]>(&Eth400g_mac_rspec_base->txcrc_trunc_ctrl), 0, {0x8}, sizeof(uint32_t) } },
    { "rxff_ctrl", { const_cast<uint32_t(*)[0x8]>(&Eth400g_mac_rspec_base->rxff_ctrl), 0, {0x8}, sizeof(uint32_t) } },
    { "rxpkt_err_sts", { const_cast<uint32_t(*)[0x8]>(&Eth400g_mac_rspec_base->rxpkt_err_sts), 0, {0x8}, sizeof(uint32_t) } },
    { "cts_fifo_out", { const_cast<Eth400g_mac_rspec_cts_fifo_out(*)[0x8]>(&Eth400g_mac_rspec_base->cts_fifo_out), new Eth400g_mac_rspec_cts_fifo_out_map, {0x8}, sizeof(Eth400g_mac_rspec_cts_fifo_out) } },
    { "eth_ring_addr", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->eth_ring_addr), 0, {}, sizeof(uint32_t) } },
    { "eth_ring_wdata", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->eth_ring_wdata), 0, {}, sizeof(uint32_t) } },
    { "eth_ring_rdata", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->eth_ring_rdata), 0, {}, sizeof(uint32_t) } },
    { "eth_ring_ctrl", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->eth_ring_ctrl), 0, {}, sizeof(uint32_t) } },
    { "eth_ring_setup", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->eth_ring_setup), 0, {}, sizeof(uint32_t) } },
    { "soft_port_alive", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->soft_port_alive), 0, {}, sizeof(uint32_t) } },
    { "chnl_intr", { &Eth400g_mac_rspec_base->chnl_intr, new Eth400g_mac_rspec_chnl_intr_map, {}, sizeof(Eth400g_mac_rspec_chnl_intr) } },
    { "cts_intr", { &Eth400g_mac_rspec_base->cts_intr, new Eth400g_mac_rspec_cts_intr_map, {}, sizeof(Eth400g_mac_rspec_cts_intr) } },
    { "mem_intr", { &Eth400g_mac_rspec_base->mem_intr, new Eth400g_mac_rspec_mem_intr_map, {}, sizeof(Eth400g_mac_rspec_mem_intr) } },
    { "uctrl_intr", { &Eth400g_mac_rspec_base->uctrl_intr, new Eth400g_mac_rspec_uctrl_intr_map, {}, sizeof(Eth400g_mac_rspec_uctrl_intr) } },
    { "txff_ecc", { const_cast<Eth400g_mac_rspec_txff_ecc(*)>(&Eth400g_mac_rspec_base->txff_ecc), new Eth400g_mac_rspec_txff_ecc_map, {}, sizeof(Eth400g_mac_rspec_txff_ecc) } },
    { "rxff_stat_ecc", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->rxff_stat_ecc), 0, {}, sizeof(uint32_t) } },
    { "txfifo_sbe_err_log", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->txfifo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "txfifo_mbe_err_log", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->txfifo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "txappfifo_sbe_err_log", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->txappfifo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "txappfifo_mbe_err_log", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->txappfifo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "rxappfifo_sbe_err_log", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->rxappfifo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "rxappfifo_mbe_err_log", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->rxappfifo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "statsmem_sbe_err_log", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->statsmem_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "statsmem_mbe_err_log", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->statsmem_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "tv80mem_sbe_err_log", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->tv80mem_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "tv80mem_mbe_err_log", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->tv80mem_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "mac_en0", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->mac_en0), 0, {}, sizeof(uint32_t) } },
    { "mac_en1", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->mac_en1), 0, {}, sizeof(uint32_t) } },
    { "mac_freeze_enable", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->mac_freeze_enable), 0, {}, sizeof(uint32_t) } },
    { "crcerr_inj", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->crcerr_inj), 0, {}, sizeof(uint32_t) } },
    { "tv80_intr", { &Eth400g_mac_rspec_base->tv80_intr, new Eth400g_mac_rspec_tv80_intr_map, {}, sizeof(Eth400g_mac_rspec_tv80_intr) } },
    { "tv80_debug_ctrl", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->tv80_debug_ctrl), 0, {}, sizeof(uint32_t) } },
    { "tv80_debug_head_ptr", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->tv80_debug_head_ptr), 0, {}, sizeof(uint32_t) } },
    { "tv80_debug_tail_ptr", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->tv80_debug_tail_ptr), 0, {}, sizeof(uint32_t) } },
    { "tv80_stall_on_error", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->tv80_stall_on_error), 0, {}, sizeof(uint32_t) } },
    { "tv80_halted_status", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->tv80_halted_status), 0, {}, sizeof(uint32_t) } },
    { "tv80_watchdog_ctrl", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->tv80_watchdog_ctrl), 0, {}, sizeof(uint32_t) } },
    { "tv80_watchdog_count", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->tv80_watchdog_count), 0, {}, sizeof(uint32_t) } },
    { "eth_mdioci_addr", { const_cast<uint32_t(*)>(&Eth400g_mac_rspec_base->eth_mdioci_addr), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth400g_pcs_rspec_mem_intr_map: public RegisterMapper {
  static constexpr PTR_Eth400g_pcs_rspec_mem_intr Eth400g_pcs_rspec_mem_intr_base=0;
  Eth400g_pcs_rspec_mem_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_mem_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_mem_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_mem_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_mem_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_mem_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth400g_pcs_rspec_map: public RegisterMapper {
  static constexpr PTR_Eth400g_pcs_rspec Eth400g_pcs_rspec_base=0;
  Eth400g_pcs_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)[0x4]>(&Eth400g_pcs_rspec_base->scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "eth_soft_reset", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->eth_soft_reset), 0, {}, sizeof(uint32_t) } },
    { "eth_clkobs_ctrl", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->eth_clkobs_ctrl), 0, {}, sizeof(uint32_t) } },
    { "eth_rxsigok_ctrl", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->eth_rxsigok_ctrl), 0, {}, sizeof(uint32_t) } },
    { "eth_mdioci_ctrl", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->eth_mdioci_ctrl), 0, {}, sizeof(uint32_t) } },
    { "eth_mdioci_poll_ctrl", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->eth_mdioci_poll_ctrl), 0, {}, sizeof(uint32_t) } },
    { "eth_mdioci_poll_time", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->eth_mdioci_poll_time), 0, {}, sizeof(uint32_t) } },
    { "eth_rxsigok_bitsel", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->eth_rxsigok_bitsel), 0, {}, sizeof(uint32_t) } },
    { "mem_intr", { &Eth400g_pcs_rspec_base->mem_intr, new Eth400g_pcs_rspec_mem_intr_map, {}, sizeof(Eth400g_pcs_rspec_mem_intr) } },
    { "deskew_ecc", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->deskew_ecc), 0, {}, sizeof(uint32_t) } },
    { "rsfec01_ecc", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->rsfec01_ecc), 0, {}, sizeof(uint32_t) } },
    { "rsfec23_ecc", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->rsfec23_ecc), 0, {}, sizeof(uint32_t) } },
    { "deskew_sbe_err_log", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->deskew_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "deskew_mbe_err_log", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->deskew_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "rsfec_sbe_err_log", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->rsfec_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "rsfec_mbe_err_log", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->rsfec_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "mdioci_intr_stat", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->mdioci_intr_stat), 0, {}, sizeof(uint32_t) } },
    { "mdioci_en0", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->mdioci_en0), 0, {}, sizeof(uint32_t) } },
    { "mdioci_en1", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->mdioci_en1), 0, {}, sizeof(uint32_t) } },
    { "mdioci_freeze_enable", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->mdioci_freeze_enable), 0, {}, sizeof(uint32_t) } },
    { "txsds_mode", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->txsds_mode), 0, {}, sizeof(uint32_t) } },
    { "rxsds_mode", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->rxsds_mode), 0, {}, sizeof(uint32_t) } },
    { "sds_112g", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->sds_112g), 0, {}, sizeof(uint32_t) } },
    { "eth_ppm_sel", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->eth_ppm_sel), 0, {}, sizeof(uint32_t) } },
    { "eth_ppm_ctrl", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->eth_ppm_ctrl), 0, {}, sizeof(uint32_t) } },
    { "eth_ppm_stat", { const_cast<uint32_t(*)>(&Eth400g_pcs_rspec_base->eth_ppm_stat), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tv80_rspec_map: public RegisterMapper {
  static constexpr PTR_Tv80_rspec Tv80_rspec_base=0;
  Tv80_rspec_map() : RegisterMapper( {
    { "dummy_register", { const_cast<uint32_t(*)[0x1000]>(&Tv80_rspec_base->dummy_register), 0, {0x1000}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth400g_addrmap_map: public RegisterMapper {
  static constexpr PTR_Eth400g_addrmap Eth400g_addrmap_base=0;
  Eth400g_addrmap_map() : RegisterMapper( {
    { "eth400g_umac4", { &Eth400g_addrmap_base->eth400g_umac4, new Umac4_addrmap_map, {}, sizeof(Umac4_addrmap) } },
    { "eth400g_mac", { &Eth400g_addrmap_base->eth400g_mac, new Eth400g_mac_rspec_map, {}, sizeof(Eth400g_mac_rspec) } },
    { "eth400g_pcs", { &Eth400g_addrmap_base->eth400g_pcs, new Eth400g_pcs_rspec_map, {}, sizeof(Eth400g_pcs_rspec) } },
    { "eth400g_tv80", { &Eth400g_addrmap_base->eth400g_tv80, new Tv80_rspec_map, {}, sizeof(Tv80_rspec) } }
    } )
  {}
};

struct Gpio_pair_regs_map: public RegisterMapper {
  static constexpr PTR_Gpio_pair_regs Gpio_pair_regs_base=0;
  Gpio_pair_regs_map() : RegisterMapper( {
    { "gpio_config", { const_cast<uint32_t(*)>(&Gpio_pair_regs_base->gpio_config), 0, {}, sizeof(uint32_t) } },
    { "gpio_settings", { const_cast<uint32_t(*)>(&Gpio_pair_regs_base->gpio_settings), 0, {}, sizeof(uint32_t) } },
    { "i2c_addr", { const_cast<uint32_t(*)>(&Gpio_pair_regs_base->i2c_addr), 0, {}, sizeof(uint32_t) } },
    { "i2c_wdata", { const_cast<uint32_t(*)>(&Gpio_pair_regs_base->i2c_wdata), 0, {}, sizeof(uint32_t) } },
    { "i2c_ctrl", { const_cast<uint32_t(*)>(&Gpio_pair_regs_base->i2c_ctrl), 0, {}, sizeof(uint32_t) } },
    { "i2c_statein", { const_cast<uint32_t(*)>(&Gpio_pair_regs_base->i2c_statein), 0, {}, sizeof(uint32_t) } },
    { "i2c_scl_freq", { const_cast<uint32_t(*)>(&Gpio_pair_regs_base->i2c_scl_freq), 0, {}, sizeof(uint32_t) } },
    { "i2c_rdata", { const_cast<uint32_t(*)>(&Gpio_pair_regs_base->i2c_rdata), 0, {}, sizeof(uint32_t) } },
    { "mdio_ctrl", { const_cast<uint32_t(*)>(&Gpio_pair_regs_base->mdio_ctrl), 0, {}, sizeof(uint32_t) } },
    { "mdio_addrdata", { const_cast<uint32_t(*)>(&Gpio_pair_regs_base->mdio_addrdata), 0, {}, sizeof(uint32_t) } },
    { "mdio_clkdiv", { const_cast<uint32_t(*)>(&Gpio_pair_regs_base->mdio_clkdiv), 0, {}, sizeof(uint32_t) } },
    { "pair_mac_ctrl", { const_cast<uint32_t(*)>(&Gpio_pair_regs_base->pair_mac_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Gpio_common_regs_eth_gpio_intr_map: public RegisterMapper {
  static constexpr PTR_Gpio_common_regs_eth_gpio_intr Gpio_common_regs_eth_gpio_intr_base=0;
  Gpio_common_regs_eth_gpio_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Gpio_common_regs_eth_gpio_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Gpio_common_regs_eth_gpio_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Gpio_common_regs_eth_gpio_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Gpio_common_regs_eth_gpio_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Gpio_common_regs_eth_gpio_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Gpio_common_regs_map: public RegisterMapper {
  static constexpr PTR_Gpio_common_regs Gpio_common_regs_base=0;
  Gpio_common_regs_map() : RegisterMapper( {
    { "stateout", { const_cast<uint32_t(*)[0x40]>(&Gpio_common_regs_base->stateout), 0, {0x40}, sizeof(uint32_t) } },
    { "statein", { const_cast<uint32_t(*)[0x8]>(&Gpio_common_regs_base->statein), 0, {0x8}, sizeof(uint32_t) } },
    { "validin", { const_cast<uint32_t(*)[0x8]>(&Gpio_common_regs_base->validin), 0, {0x8}, sizeof(uint32_t) } },
    { "i2c_basetime", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->i2c_basetime), 0, {}, sizeof(uint32_t) } },
    { "eth_gpio_intr", { &Gpio_common_regs_base->eth_gpio_intr, new Gpio_common_regs_eth_gpio_intr_map, {}, sizeof(Gpio_common_regs_eth_gpio_intr) } },
    { "shld_ctrl", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->shld_ctrl), 0, {}, sizeof(uint32_t) } },
    { "gpio_status", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->gpio_status), 0, {}, sizeof(uint32_t) } },
    { "stateout_mask", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->stateout_mask), 0, {}, sizeof(uint32_t) } },
    { "scratch_r", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->scratch_r), 0, {}, sizeof(uint32_t) } },
    { "ring_lock_timeout", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->ring_lock_timeout), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Gpio_regs_map: public RegisterMapper {
  static constexpr PTR_Gpio_regs Gpio_regs_base=0;
  Gpio_regs_map() : RegisterMapper( {
    { "gpio_pair_regs", { &Gpio_regs_base->gpio_pair_regs, new Gpio_pair_regs_map, {0x6}, sizeof(Gpio_pair_regs) } },
    { "gpio_common_regs", { &Gpio_regs_base->gpio_common_regs, new Gpio_common_regs_map, {}, sizeof(Gpio_common_regs) } }
    } )
  {}
};

struct Eth_gpio_regs_map: public RegisterMapper {
  static constexpr PTR_Eth_gpio_regs Eth_gpio_regs_base=0;
  Eth_gpio_regs_map() : RegisterMapper( {
    { "gpio_regs", { &Eth_gpio_regs_base->gpio_regs, new Gpio_regs_map, {}, sizeof(Gpio_regs) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Eth_gpio_regs_base->dft_csr), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Gpio_iotile_rspec_map: public RegisterMapper {
  static constexpr PTR_Gpio_iotile_rspec Gpio_iotile_rspec_base=0;
  Gpio_iotile_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)[0x4]>(&Gpio_iotile_rspec_base->scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "clkobs_ctrl", { const_cast<uint32_t(*)>(&Gpio_iotile_rspec_base->clkobs_ctrl), 0, {}, sizeof(uint32_t) } },
    { "mdioci_ctrl", { const_cast<uint32_t(*)>(&Gpio_iotile_rspec_base->mdioci_ctrl), 0, {}, sizeof(uint32_t) } },
    { "mdioci_int_stat", { const_cast<uint32_t(*)>(&Gpio_iotile_rspec_base->mdioci_int_stat), 0, {}, sizeof(uint32_t) } },
    { "mdioci_en0", { const_cast<uint32_t(*)>(&Gpio_iotile_rspec_base->mdioci_en0), 0, {}, sizeof(uint32_t) } },
    { "mdioci_en1", { const_cast<uint32_t(*)>(&Gpio_iotile_rspec_base->mdioci_en1), 0, {}, sizeof(uint32_t) } },
    { "mdioci_freeze_enable", { const_cast<uint32_t(*)>(&Gpio_iotile_rspec_base->mdioci_freeze_enable), 0, {}, sizeof(uint32_t) } },
    { "tile_softreset", { const_cast<uint32_t(*)>(&Gpio_iotile_rspec_base->tile_softreset), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Serdes_addrmap_map: public RegisterMapper {
  static constexpr PTR_Serdes_addrmap Serdes_addrmap_base=0;
  Serdes_addrmap_map() : RegisterMapper( {
    { "dummy_register", { const_cast<uint32_t(*)>(&Serdes_addrmap_base->dummy_register), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_imem_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_imem_addrmap Mau_imem_addrmap_base=0;
  Mau_imem_addrmap_map() : RegisterMapper( {
    { "imem_mocha_subword16", { const_cast<uint32_t(*)[0x2][0x4][0x4][0x20]>(&Mau_imem_addrmap_base->imem_mocha_subword16), 0, {0x2,0x4,0x4,0x20}, sizeof(uint32_t) } },
    { "imem_dark_subword16", { const_cast<uint32_t(*)[0x2][0x4][0x4][0x20]>(&Mau_imem_addrmap_base->imem_dark_subword16), 0, {0x2,0x4,0x4,0x20}, sizeof(uint32_t) } },
    { "imem_mocha_subword32", { const_cast<uint32_t(*)[0x2][0x2][0x4][0x20]>(&Mau_imem_addrmap_base->imem_mocha_subword32), 0, {0x2,0x2,0x4,0x20}, sizeof(uint32_t) } },
    { "imem_mocha_subword8", { const_cast<uint32_t(*)[0x2][0x2][0x4][0x20]>(&Mau_imem_addrmap_base->imem_mocha_subword8), 0, {0x2,0x2,0x4,0x20}, sizeof(uint32_t) } },
    { "imem_dark_subword32", { const_cast<uint32_t(*)[0x2][0x2][0x4][0x20]>(&Mau_imem_addrmap_base->imem_dark_subword32), 0, {0x2,0x2,0x4,0x20}, sizeof(uint32_t) } },
    { "imem_dark_subword8", { const_cast<uint32_t(*)[0x2][0x2][0x4][0x20]>(&Mau_imem_addrmap_base->imem_dark_subword8), 0, {0x2,0x2,0x4,0x20}, sizeof(uint32_t) } },
    { "imem_subword16", { const_cast<uint32_t(*)[0x2][0x4][0x10][0x20]>(&Mau_imem_addrmap_base->imem_subword16), 0, {0x2,0x4,0x10,0x20}, sizeof(uint32_t) } },
    { "imem_subword32", { const_cast<uint32_t(*)[0x2][0x2][0x10][0x20]>(&Mau_imem_addrmap_base->imem_subword32), 0, {0x2,0x2,0x10,0x20}, sizeof(uint32_t) } },
    { "imem_subword8", { const_cast<uint32_t(*)[0x2][0x2][0x10][0x20]>(&Mau_imem_addrmap_base->imem_subword8), 0, {0x2,0x2,0x10,0x20}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_snapshot_control_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_snapshot_control_addrmap Mau_snapshot_control_addrmap_base=0;
  Mau_snapshot_control_addrmap_map() : RegisterMapper( {
    { "mau_fsm_snapshot_cur_stateq", { const_cast<uint32_t(*)[0x2]>(&Mau_snapshot_control_addrmap_base->mau_fsm_snapshot_cur_stateq), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_snapshot_config", { const_cast<uint32_t(*)>(&Mau_snapshot_control_addrmap_base->mau_snapshot_config), 0, {}, sizeof(uint32_t) } },
    { "mau_snapshot_timestamp_trigger_hi", { const_cast<uint32_t(*)>(&Mau_snapshot_control_addrmap_base->mau_snapshot_timestamp_trigger_hi), 0, {}, sizeof(uint32_t) } },
    { "mau_snapshot_timestamp_trigger_lo", { const_cast<uint32_t(*)>(&Mau_snapshot_control_addrmap_base->mau_snapshot_timestamp_trigger_lo), 0, {}, sizeof(uint32_t) } },
    { "mau_snapshot_timestamp_hi", { const_cast<uint32_t(*)>(&Mau_snapshot_control_addrmap_base->mau_snapshot_timestamp_hi), 0, {}, sizeof(uint32_t) } },
    { "mau_snapshot_timestamp_lo", { const_cast<uint32_t(*)>(&Mau_snapshot_control_addrmap_base->mau_snapshot_timestamp_lo), 0, {}, sizeof(uint32_t) } },
    { "mau_snapshot_datapath_capture", { const_cast<uint32_t(*)[0x2]>(&Mau_snapshot_control_addrmap_base->mau_snapshot_datapath_capture), 0, {0x2}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_snapshot_match_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_snapshot_match_addrmap Mau_snapshot_match_addrmap_base=0;
  Mau_snapshot_match_addrmap_map() : RegisterMapper( {
    { "mau_snapshot_match_subword32b_lo", { const_cast<uint32_t(*)[0x40][0x2]>(&Mau_snapshot_match_addrmap_base->mau_snapshot_match_subword32b_lo), 0, {0x40,0x2}, sizeof(uint32_t) } },
    { "mau_snapshot_match_subword32b_hi", { const_cast<uint32_t(*)[0x40][0x2]>(&Mau_snapshot_match_addrmap_base->mau_snapshot_match_subword32b_hi), 0, {0x40,0x2}, sizeof(uint32_t) } },
    { "mau_snapshot_match_subword8b", { const_cast<uint32_t(*)[0x40][0x2]>(&Mau_snapshot_match_addrmap_base->mau_snapshot_match_subword8b), 0, {0x40,0x2}, sizeof(uint32_t) } },
    { "mau_snapshot_match_subword16b", { const_cast<uint32_t(*)[0x60][0x2]>(&Mau_snapshot_match_addrmap_base->mau_snapshot_match_subword16b), 0, {0x60,0x2}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_snapshot_capture_half_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_snapshot_capture_half_addrmap Mau_snapshot_capture_half_addrmap_base=0;
  Mau_snapshot_capture_half_addrmap_map() : RegisterMapper( {
    { "mau_snapshot_capture_subword32b_lo", { const_cast<uint32_t(*)[0x14][0x2]>(&Mau_snapshot_capture_half_addrmap_base->mau_snapshot_capture_subword32b_lo), 0, {0x14,0x2}, sizeof(uint32_t) } },
    { "mau_snapshot_capture_subword32b_hi", { const_cast<uint32_t(*)[0x14][0x2]>(&Mau_snapshot_capture_half_addrmap_base->mau_snapshot_capture_subword32b_hi), 0, {0x14,0x2}, sizeof(uint32_t) } },
    { "mau_snapshot_capture_subword8b", { const_cast<uint32_t(*)[0x14][0x2]>(&Mau_snapshot_capture_half_addrmap_base->mau_snapshot_capture_subword8b), 0, {0x14,0x2}, sizeof(uint32_t) } },
    { "mau_snapshot_capture_subword16b", { const_cast<uint32_t(*)[0x14][0x4]>(&Mau_snapshot_capture_half_addrmap_base->mau_snapshot_capture_subword16b), 0, {0x14,0x4}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_snapshot_datapath_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_snapshot_datapath_addrmap Mau_snapshot_datapath_addrmap_base=0;
  Mau_snapshot_datapath_addrmap_map() : RegisterMapper( {
    { "snapshot_match", { &Mau_snapshot_datapath_addrmap_base->snapshot_match, new Mau_snapshot_match_addrmap_map, {}, sizeof(Mau_snapshot_match_addrmap) } },
    { "snapshot_capture", { &Mau_snapshot_datapath_addrmap_base->snapshot_capture, new Mau_snapshot_capture_half_addrmap_map, {0x2}, sizeof(Mau_snapshot_capture_half_addrmap) } }
    } )
  {}
};

struct Tcam_byte_swizzle_xbar_addrmap_map: public RegisterMapper {
  static constexpr PTR_Tcam_byte_swizzle_xbar_addrmap Tcam_byte_swizzle_xbar_addrmap_base=0;
  Tcam_byte_swizzle_xbar_addrmap_map() : RegisterMapper( {
    { "tcam_byte_swizzle_ctl", { const_cast<uint32_t(*)[0x11]>(&Tcam_byte_swizzle_xbar_addrmap_base->tcam_byte_swizzle_ctl), 0, {0x11}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_match_input_xbar_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_match_input_xbar_addrmap Mau_match_input_xbar_addrmap_base=0;
  Mau_match_input_xbar_addrmap_map() : RegisterMapper( {
    { "match_input_xbar_816b_ctl", { const_cast<uint32_t(*)[0x4][0x100]>(&Mau_match_input_xbar_addrmap_base->match_input_xbar_816b_ctl), 0, {0x4,0x100}, sizeof(uint32_t) } },
    { "mau_match_input_xbar_ternary_match_enable", { const_cast<uint32_t(*)[0x2]>(&Mau_match_input_xbar_addrmap_base->mau_match_input_xbar_ternary_match_enable), 0, {0x2}, sizeof(uint32_t) } },
    { "tswizzle", { &Mau_match_input_xbar_addrmap_base->tswizzle, new Tcam_byte_swizzle_xbar_addrmap_map, {}, sizeof(Tcam_byte_swizzle_xbar_addrmap) } },
    { "match_input_xbar_32b_ctl", { const_cast<uint32_t(*)[0x4][0x100]>(&Mau_match_input_xbar_addrmap_base->match_input_xbar_32b_ctl), 0, {0x4,0x100}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_match_input_hash_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_match_input_hash_addrmap Mau_match_input_hash_addrmap_base=0;
  Mau_match_input_hash_addrmap_map() : RegisterMapper( {
    { "parity_group_mask", { const_cast<uint32_t(*)[0x8][0x2]>(&Mau_match_input_hash_addrmap_base->parity_group_mask), 0, {0x8,0x2}, sizeof(uint32_t) } },
    { "hash_seed", { const_cast<uint32_t(*)[0x34]>(&Mau_match_input_hash_addrmap_base->hash_seed), 0, {0x34}, sizeof(uint32_t) } },
    { "galois_field_matrix", { const_cast<uint32_t(*)[0x40][0x40]>(&Mau_match_input_hash_addrmap_base->galois_field_matrix), 0, {0x40,0x40}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_match_input_xbar_hash_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_match_input_xbar_hash_addrmap Mau_match_input_xbar_hash_addrmap_base=0;
  Mau_match_input_xbar_hash_addrmap_map() : RegisterMapper( {
    { "xbar", { &Mau_match_input_xbar_hash_addrmap_base->xbar, new Mau_match_input_xbar_addrmap_map, {}, sizeof(Mau_match_input_xbar_addrmap) } },
    { "hash", { &Mau_match_input_xbar_hash_addrmap_base->hash, new Mau_match_input_hash_addrmap_map, {}, sizeof(Mau_match_input_hash_addrmap) } }
    } )
  {}
};

struct Mau_datapath_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_datapath_addrmap Mau_datapath_addrmap_base=0;
  Mau_datapath_addrmap_map() : RegisterMapper( {
    { "imem", { &Mau_datapath_addrmap_base->imem, new Mau_imem_addrmap_map, {}, sizeof(Mau_imem_addrmap) } },
    { "phv_egress_thread_imem", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_datapath_addrmap_base->phv_egress_thread_imem), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "actionmux_din_power_ctl", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_datapath_addrmap_base->actionmux_din_power_ctl), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "match_input_xbar_din_power_ctl", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_datapath_addrmap_base->match_input_xbar_din_power_ctl), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "snapshot_ctl", { &Mau_datapath_addrmap_base->snapshot_ctl, new Mau_snapshot_control_addrmap_map, {}, sizeof(Mau_snapshot_control_addrmap) } },
    { "imem_table_addr_format", { const_cast<uint32_t(*)[0x10]>(&Mau_datapath_addrmap_base->imem_table_addr_format), 0, {0x10}, sizeof(uint32_t) } },
    { "imem_table_selector_fallback_addr", { const_cast<uint32_t(*)[0x10]>(&Mau_datapath_addrmap_base->imem_table_selector_fallback_addr), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_snapshot_imem_logical_read_adr", { const_cast<uint32_t(*)[0x10]>(&Mau_datapath_addrmap_base->mau_snapshot_imem_logical_read_adr), 0, {0x10}, sizeof(uint32_t) } },
    { "intr_enable0_mau_gfm_hash", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_enable0_mau_gfm_hash), 0, {}, sizeof(uint32_t) } },
    { "intr_enable1_mau_gfm_hash", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_enable1_mau_gfm_hash), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_gfm_hash", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_inject_mau_gfm_hash), 0, {}, sizeof(uint32_t) } },
    { "intr_freeze_enable_mau_gfm_hash", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_freeze_enable_mau_gfm_hash), 0, {}, sizeof(uint32_t) } },
    { "imem_word_read_override", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->imem_word_read_override), 0, {}, sizeof(uint32_t) } },
    { "mau_snapshot_imem_logical_selector_fallback", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->mau_snapshot_imem_logical_selector_fallback), 0, {}, sizeof(uint32_t) } },
    { "imem_table_selector_fallback_icxbar_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_datapath_addrmap_base->imem_table_selector_fallback_icxbar_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "dft_csr_memctrl_dp", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->dft_csr_memctrl_dp), 0, {}, sizeof(uint32_t) } },
    { "action_output_delay", { const_cast<uint32_t(*)[0x2]>(&Mau_datapath_addrmap_base->action_output_delay), 0, {0x2}, sizeof(uint32_t) } },
    { "cur_stage_dependency_on_prev", { const_cast<uint32_t(*)[0x2]>(&Mau_datapath_addrmap_base->cur_stage_dependency_on_prev), 0, {0x2}, sizeof(uint32_t) } },
    { "next_stage_dependency_on_cur", { const_cast<uint32_t(*)[0x2]>(&Mau_datapath_addrmap_base->next_stage_dependency_on_cur), 0, {0x2}, sizeof(uint32_t) } },
    { "pipelength_added_stages", { const_cast<uint32_t(*)[0x2]>(&Mau_datapath_addrmap_base->pipelength_added_stages), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_match_input_xbar_exact_match_enable", { const_cast<uint32_t(*)[0x2]>(&Mau_datapath_addrmap_base->mau_match_input_xbar_exact_match_enable), 0, {0x2}, sizeof(uint32_t) } },
    { "match_ie_input_mux_sel", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->match_ie_input_mux_sel), 0, {}, sizeof(uint32_t) } },
    { "imem_parity_ctl", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->imem_parity_ctl), 0, {}, sizeof(uint32_t) } },
    { "phv_fifo_enable", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->phv_fifo_enable), 0, {}, sizeof(uint32_t) } },
    { "stage_concurrent_with_prev", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->stage_concurrent_with_prev), 0, {}, sizeof(uint32_t) } },
    { "imem_table_addr_egress", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->imem_table_addr_egress), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_adb_ctl", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->mau_diag_adb_ctl), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_imem", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_status_mau_imem), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_imem", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_enable0_mau_imem), 0, {}, sizeof(uint32_t) } },
    { "intr_enable1_mau_imem", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_enable1_mau_imem), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_imem", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_inject_mau_imem), 0, {}, sizeof(uint32_t) } },
    { "intr_freeze_enable_mau_imem", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_freeze_enable_mau_imem), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_snapshot", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_status_mau_snapshot), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_snapshot", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_enable0_mau_snapshot), 0, {}, sizeof(uint32_t) } },
    { "intr_enable1_mau_snapshot", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_enable1_mau_snapshot), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_snapshot", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_inject_mau_snapshot), 0, {}, sizeof(uint32_t) } },
    { "intr_freeze_enable_mau_snapshot", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_freeze_enable_mau_snapshot), 0, {}, sizeof(uint32_t) } },
    { "mau_scratch", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->mau_scratch), 0, {}, sizeof(uint32_t) } },
    { "imem_sbe_errlog", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->imem_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "hashout_ctl", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->hashout_ctl), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_gfm_hash", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_status_mau_gfm_hash), 0, {}, sizeof(uint32_t) } },
    { "phv_ingress_thread", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_datapath_addrmap_base->phv_ingress_thread), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "phv_egress_thread", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_datapath_addrmap_base->phv_egress_thread), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "phv_ingress_thread_imem", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_datapath_addrmap_base->phv_ingress_thread_imem), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "snapshot_dp", { &Mau_datapath_addrmap_base->snapshot_dp, new Mau_snapshot_datapath_addrmap_map, {}, sizeof(Mau_snapshot_datapath_addrmap) } },
    { "xbar_hash", { &Mau_datapath_addrmap_base->xbar_hash, new Mau_match_input_xbar_hash_addrmap_map, {}, sizeof(Mau_match_input_xbar_hash_addrmap) } }
    } )
  {}
};

struct Mau_cfg_regs_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_cfg_regs_addrmap Mau_cfg_regs_addrmap_base=0;
  Mau_cfg_regs_addrmap_map() : RegisterMapper( {
    { "pbs_cresp_ecc", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->pbs_cresp_ecc), 0, {}, sizeof(uint32_t) } },
    { "pbs_sreq_ecc", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->pbs_sreq_ecc), 0, {}, sizeof(uint32_t) } },
    { "amod_ecc", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->amod_ecc), 0, {}, sizeof(uint32_t) } },
    { "amod_ecc_sbe_errlog", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->amod_ecc_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "amod_ecc_mbe_errlog", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->amod_ecc_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "amod_req_interval", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->amod_req_interval), 0, {}, sizeof(uint32_t) } },
    { "amod_req_limit", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->amod_req_limit), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_cfg", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->intr_status_mau_cfg), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_cfg", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->intr_enable0_mau_cfg), 0, {}, sizeof(uint32_t) } },
    { "intr_enable1_mau_cfg", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->intr_enable1_mau_cfg), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_cfg", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->intr_inject_mau_cfg), 0, {}, sizeof(uint32_t) } },
    { "intr_freeze_enable_mau_cfg", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->intr_freeze_enable_mau_cfg), 0, {}, sizeof(uint32_t) } },
    { "intr_decode_top", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->intr_decode_top), 0, {}, sizeof(uint32_t) } },
    { "q_hole_acc_errlog_hi", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->q_hole_acc_errlog_hi), 0, {}, sizeof(uint32_t) } },
    { "q_hole_acc_errlog_lo", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->q_hole_acc_errlog_lo), 0, {}, sizeof(uint32_t) } },
    { "sreq_stats_timeout_errlog", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->sreq_stats_timeout_errlog), 0, {}, sizeof(uint32_t) } },
    { "sreq_idle_timeout_errlog", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->sreq_idle_timeout_errlog), 0, {}, sizeof(uint32_t) } },
    { "mau_cfg_movereg_tcam_only", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_cfg_movereg_tcam_only), 0, {}, sizeof(uint32_t) } },
    { "mau_cfg_mem_slow_mode", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_cfg_mem_slow_mode), 0, {}, sizeof(uint32_t) } },
    { "mau_cfg_lt_thread", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_cfg_lt_thread), 0, {}, sizeof(uint32_t) } },
    { "mau_cfg_dram_thread", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_cfg_dram_thread), 0, {}, sizeof(uint32_t) } },
    { "mau_cfg_imem_bubble_req", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_cfg_imem_bubble_req), 0, {}, sizeof(uint32_t) } },
    { "mau_cfg_cmd_queue_timeout", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_cfg_cmd_queue_timeout), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_pbus_enable", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_diag_pbus_enable), 0, {}, sizeof(uint32_t) } },
    { "stats_dump_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_cfg_regs_addrmap_base->stats_dump_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "idle_dump_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_cfg_regs_addrmap_base->idle_dump_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_diag_valid_ctl", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_diag_valid_ctl), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_cfg_ctl", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_diag_cfg_ctl), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_tcam_hit_xbar_ctl", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_diag_tcam_hit_xbar_ctl), 0, {}, sizeof(uint32_t) } },
    { "amod_protocol_errlog", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->amod_protocol_errlog), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_cfg_oxbar_ctl", { const_cast<uint32_t(*)[0x8]>(&Mau_cfg_regs_addrmap_base->mau_diag_cfg_oxbar_ctl), 0, {0x8}, sizeof(uint32_t) } },
    { "pbs_creq_errlog", { const_cast<uint32_t(*)[0x4]>(&Mau_cfg_regs_addrmap_base->pbs_creq_errlog), 0, {0x4}, sizeof(uint32_t) } },
    { "pbs_cresp_errlog", { const_cast<uint32_t(*)[0x4]>(&Mau_cfg_regs_addrmap_base->pbs_cresp_errlog), 0, {0x4}, sizeof(uint32_t) } },
    { "pbs_sreq_errlog", { const_cast<uint32_t(*)[0x4]>(&Mau_cfg_regs_addrmap_base->pbs_sreq_errlog), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_cfg_stats_alu_lt", { const_cast<uint32_t(*)[0x4]>(&Mau_cfg_regs_addrmap_base->mau_cfg_stats_alu_lt), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_cfg_uram_thread", { const_cast<uint32_t(*)[0x3]>(&Mau_cfg_regs_addrmap_base->mau_cfg_uram_thread), 0, {0x3}, sizeof(uint32_t) } },
    { "amod_pre_drain_delay", { const_cast<uint32_t(*)[0x2]>(&Mau_cfg_regs_addrmap_base->amod_pre_drain_delay), 0, {0x2}, sizeof(uint32_t) } },
    { "amod_wide_bubble_rsp_delay", { const_cast<uint32_t(*)[0x2]>(&Mau_cfg_regs_addrmap_base->amod_wide_bubble_rsp_delay), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_cfg_mram_thread", { const_cast<uint32_t(*)[0x2]>(&Mau_cfg_regs_addrmap_base->mau_cfg_mram_thread), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_cfg_sreq_timeout", { const_cast<uint32_t(*)[0x2]>(&Mau_cfg_regs_addrmap_base->mau_cfg_sreq_timeout), 0, {0x2}, sizeof(uint32_t) } },
    { "dft_csr_memctrl_glue", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->dft_csr_memctrl_glue), 0, {}, sizeof(uint32_t) } },
    { "tcam_scrub_ctl", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->tcam_scrub_ctl), 0, {}, sizeof(uint32_t) } },
    { "stage_dump_ctl", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->stage_dump_ctl), 0, {}, sizeof(uint32_t) } },
    { "pbs_creq_ecc", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->pbs_creq_ecc), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_tcam_row_vh_xbar_array_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_tcam_row_vh_xbar_array_addrmap Mau_tcam_row_vh_xbar_array_addrmap_base=0;
  Mau_tcam_row_vh_xbar_array_addrmap_map() : RegisterMapper( {
    { "tcam_validbit_xbar_ctl", { const_cast<uint32_t(*)[0x2][0x8][0x8]>(&Mau_tcam_row_vh_xbar_array_addrmap_base->tcam_validbit_xbar_ctl), 0, {0x2,0x8,0x8}, sizeof(uint32_t) } },
    { "tcam_extra_byte_ctl", { const_cast<uint32_t(*)[0x2][0x8]>(&Mau_tcam_row_vh_xbar_array_addrmap_base->tcam_extra_byte_ctl), 0, {0x2,0x8}, sizeof(uint32_t) } },
    { "tcam_row_output_ctl", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_tcam_row_vh_xbar_array_addrmap_base->tcam_row_output_ctl), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "tcam_extra_bit_mux_ctl", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_tcam_row_vh_xbar_array_addrmap_base->tcam_extra_bit_mux_ctl), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "tcam_row_halfbyte_mux_ctl", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_tcam_row_vh_xbar_array_addrmap_base->tcam_row_halfbyte_mux_ctl), 0, {0x2,0x10}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_tcam_column_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_tcam_column_addrmap Mau_tcam_column_addrmap_base=0;
  Mau_tcam_column_addrmap_map() : RegisterMapper( {
    { "tcam_ghost_thread_en", { const_cast<uint32_t(*)[0xc]>(&Mau_tcam_column_addrmap_base->tcam_ghost_thread_en), 0, {0xc}, sizeof(uint32_t) } },
    { "tcam_table_map", { const_cast<uint32_t(*)[0x8]>(&Mau_tcam_column_addrmap_base->tcam_table_map), 0, {0x8}, sizeof(uint32_t) } },
    { "tcam_mode", { const_cast<uint32_t(*)[0x10]>(&Mau_tcam_column_addrmap_base->tcam_mode), 0, {0x10}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_tcam_array_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_tcam_array_addrmap Mau_tcam_array_addrmap_base=0;
  Mau_tcam_array_addrmap_map() : RegisterMapper( {
    { "vh_data_xbar", { &Mau_tcam_array_addrmap_base->vh_data_xbar, new Mau_tcam_row_vh_xbar_array_addrmap_map, {}, sizeof(Mau_tcam_row_vh_xbar_array_addrmap) } },
    { "col", { &Mau_tcam_array_addrmap_base->col, new Mau_tcam_column_addrmap_map, {0x2}, sizeof(Mau_tcam_column_addrmap) } },
    { "tcam_sbe_errlog", { const_cast<uint32_t(*)[0xc]>(&Mau_tcam_array_addrmap_base->tcam_sbe_errlog), 0, {0xc}, sizeof(uint32_t) } },
    { "mau_diag_tcam_clk_en", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->mau_diag_tcam_clk_en), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_tcam_array", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->intr_status_mau_tcam_array), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_tcam_array", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->intr_enable0_mau_tcam_array), 0, {}, sizeof(uint32_t) } },
    { "intr_enable1_mau_tcam_array", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->intr_enable1_mau_tcam_array), 0, {}, sizeof(uint32_t) } },
    { "tcam_match_adr_shift", { const_cast<uint32_t(*)[0x8]>(&Mau_tcam_array_addrmap_base->tcam_match_adr_shift), 0, {0x8}, sizeof(uint32_t) } },
    { "tcam_output_table_thread", { const_cast<uint32_t(*)[0x8]>(&Mau_tcam_array_addrmap_base->tcam_output_table_thread), 0, {0x8}, sizeof(uint32_t) } },
    { "intr_inject_mau_tcam_array", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->intr_inject_mau_tcam_array), 0, {}, sizeof(uint32_t) } },
    { "intr_freeze_enable_mau_tcam_array", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->intr_freeze_enable_mau_tcam_array), 0, {}, sizeof(uint32_t) } },
    { "tcam_logical_channel_errlog_lo", { const_cast<uint32_t(*)[0x4]>(&Mau_tcam_array_addrmap_base->tcam_logical_channel_errlog_lo), 0, {0x4}, sizeof(uint32_t) } },
    { "tcam_logical_channel_errlog_hi", { const_cast<uint32_t(*)[0x4]>(&Mau_tcam_array_addrmap_base->tcam_logical_channel_errlog_hi), 0, {0x4}, sizeof(uint32_t) } },
    { "atomic_mod_tcam_go", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->atomic_mod_tcam_go), 0, {}, sizeof(uint32_t) } },
    { "tcam_piped", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->tcam_piped), 0, {}, sizeof(uint32_t) } },
    { "tcam_error_detect_enable", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->tcam_error_detect_enable), 0, {}, sizeof(uint32_t) } },
    { "tcam_parity_control", { const_cast<uint32_t(*)[0xc]>(&Mau_tcam_array_addrmap_base->tcam_parity_control), 0, {0xc}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_memory_data_hv_switchbox_row_control_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_memory_data_hv_switchbox_row_control_addrmap Mau_memory_data_hv_switchbox_row_control_addrmap_base=0;
  Mau_memory_data_hv_switchbox_row_control_addrmap_map() : RegisterMapper( {
    { "r_l_action_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->r_l_action_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "l_stats_alu_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->l_stats_alu_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "l_meter_alu_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->l_meter_alu_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "l_oflo_wr_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->l_oflo_wr_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "l_oflo2_wr_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->l_oflo2_wr_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "r_action_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->r_action_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "r_stats_alu_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->r_stats_alu_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "r_meter_alu_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->r_meter_alu_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "r_oflo_wr_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->r_oflo_wr_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "r_oflo2_wr_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->r_oflo2_wr_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "b_oflo_wr_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->b_oflo_wr_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "b_oflo2dn_wr_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->b_oflo2dn_wr_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "b_oflo2up_rd_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->b_oflo2up_rd_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "t_oflo_rd_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->t_oflo_rd_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "t_oflo2dn_rd_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->t_oflo2dn_rd_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "t_oflo2up_wr_o_mux_select", { const_cast<uint32_t(*)>(&Mau_memory_data_hv_switchbox_row_control_addrmap_base->t_oflo2up_wr_o_mux_select), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_memory_data_hv_switchbox_row_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_memory_data_hv_switchbox_row_addrmap Mau_memory_data_hv_switchbox_row_addrmap_base=0;
  Mau_memory_data_hv_switchbox_row_addrmap_map() : RegisterMapper( {
    { "ctl", { &Mau_memory_data_hv_switchbox_row_addrmap_base->ctl, new Mau_memory_data_hv_switchbox_row_control_addrmap_map, {}, sizeof(Mau_memory_data_hv_switchbox_row_control_addrmap) } }
    } )
  {}
};

struct Mau_memory_data_hv_switchbox_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_memory_data_hv_switchbox_addrmap Mau_memory_data_hv_switchbox_addrmap_base=0;
  Mau_memory_data_hv_switchbox_addrmap_map() : RegisterMapper( {
    { "row", { &Mau_memory_data_hv_switchbox_addrmap_base->row, new Mau_memory_data_hv_switchbox_row_addrmap_map, {0x8}, sizeof(Mau_memory_data_hv_switchbox_row_addrmap) } }
    } )
  {}
};

struct Mau_unit_ram_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_unit_ram_addrmap Mau_unit_ram_addrmap_base=0;
  Mau_unit_ram_addrmap_map() : RegisterMapper( {
    { "match_mask", { const_cast<uint32_t(*)[0x4]>(&Mau_unit_ram_addrmap_base->match_mask), 0, {0x4}, sizeof(uint32_t) } },
    { "unit_ram_ctl", { const_cast<uint32_t(*)>(&Mau_unit_ram_addrmap_base->unit_ram_ctl), 0, {}, sizeof(uint32_t) } },
    { "match_ram_vpn", { const_cast<uint32_t(*)>(&Mau_unit_ram_addrmap_base->match_ram_vpn), 0, {}, sizeof(uint32_t) } },
    { "match_nibble_s0q1_enable", { const_cast<uint32_t(*)>(&Mau_unit_ram_addrmap_base->match_nibble_s0q1_enable), 0, {}, sizeof(uint32_t) } },
    { "match_nibble_s1q0_enable", { const_cast<uint32_t(*)>(&Mau_unit_ram_addrmap_base->match_nibble_s1q0_enable), 0, {}, sizeof(uint32_t) } },
    { "match_next_table_bitpos", { const_cast<uint32_t(*)>(&Mau_unit_ram_addrmap_base->match_next_table_bitpos), 0, {}, sizeof(uint32_t) } },
    { "unit_ram_ecc", { const_cast<uint32_t(*)>(&Mau_unit_ram_addrmap_base->unit_ram_ecc), 0, {}, sizeof(uint32_t) } },
    { "unit_ram_sbe_errlog", { const_cast<uint32_t(*)>(&Mau_unit_ram_addrmap_base->unit_ram_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "unit_ram_mbe_errlog", { const_cast<uint32_t(*)>(&Mau_unit_ram_addrmap_base->unit_ram_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "match_bytemask", { const_cast<uint32_t(*)[0x5]>(&Mau_unit_ram_addrmap_base->match_bytemask), 0, {0x5}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_exactmatch_row_adr_vh_xbar_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_exactmatch_row_adr_vh_xbar_addrmap Mau_exactmatch_row_adr_vh_xbar_addrmap_base=0;
  Mau_exactmatch_row_adr_vh_xbar_addrmap_map() : RegisterMapper( {
    { "exactmatch_bank_enable", { const_cast<uint32_t(*)[0xc]>(&Mau_exactmatch_row_adr_vh_xbar_addrmap_base->exactmatch_bank_enable), 0, {0xc}, sizeof(uint32_t) } },
    { "alu_hashdata_bytemask", { const_cast<uint32_t(*)>(&Mau_exactmatch_row_adr_vh_xbar_addrmap_base->alu_hashdata_bytemask), 0, {}, sizeof(uint32_t) } },
    { "exactmatch_row_hashadr_xbar_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_exactmatch_row_adr_vh_xbar_addrmap_base->exactmatch_row_hashadr_xbar_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "exactmatch_mem_hashadr_xbar_ctl", { const_cast<uint32_t(*)[0xc]>(&Mau_exactmatch_row_adr_vh_xbar_addrmap_base->exactmatch_mem_hashadr_xbar_ctl), 0, {0xc}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_stash_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_stash_addrmap Mau_stash_addrmap_base=0;
  Mau_stash_addrmap_map() : RegisterMapper( {
    { "stash_bank_enable", { const_cast<uint32_t(*)[0x4]>(&Mau_stash_addrmap_base->stash_bank_enable), 0, {0x4}, sizeof(uint32_t) } },
    { "stash_match_address", { const_cast<uint32_t(*)[0x4]>(&Mau_stash_addrmap_base->stash_match_address), 0, {0x4}, sizeof(uint32_t) } },
    { "stash_version_valid", { const_cast<uint32_t(*)[0x4]>(&Mau_stash_addrmap_base->stash_version_valid), 0, {0x4}, sizeof(uint32_t) } },
    { "stash_match_input_data_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_stash_addrmap_base->stash_match_input_data_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "stash_match_result_bus_select", { const_cast<uint32_t(*)[0x2]>(&Mau_stash_addrmap_base->stash_match_result_bus_select), 0, {0x2}, sizeof(uint32_t) } },
    { "stash_data", { const_cast<uint32_t(*)[0x4][0x4]>(&Mau_stash_addrmap_base->stash_data), 0, {0x4,0x4}, sizeof(uint32_t) } },
    { "stash_match_mask", { const_cast<uint32_t(*)[0x2][0x4]>(&Mau_stash_addrmap_base->stash_match_mask), 0, {0x2,0x4}, sizeof(uint32_t) } },
    { "stash_bus_overload_bytemask", { const_cast<uint32_t(*)[0x2]>(&Mau_stash_addrmap_base->stash_bus_overload_bytemask), 0, {0x2}, sizeof(uint32_t) } },
    { "stash_hashkey_data", { const_cast<uint32_t(*)[0x4]>(&Mau_stash_addrmap_base->stash_hashkey_data), 0, {0x4}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_gateway_table_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_gateway_table_addrmap Mau_gateway_table_addrmap_base=0;
  Mau_gateway_table_addrmap_map() : RegisterMapper( {
    { "gateway_table_ctl", { const_cast<uint32_t(*)>(&Mau_gateway_table_addrmap_base->gateway_table_ctl), 0, {}, sizeof(uint32_t) } },
    { "gateway_table_matchdata_xor_en", { const_cast<uint32_t(*)>(&Mau_gateway_table_addrmap_base->gateway_table_matchdata_xor_en), 0, {}, sizeof(uint32_t) } },
    { "gateway_table_vv_entry", { const_cast<uint32_t(*)[0x4]>(&Mau_gateway_table_addrmap_base->gateway_table_vv_entry), 0, {0x4}, sizeof(uint32_t) } },
    { "gateway_table_entry_matchdata", { const_cast<uint32_t(*)[0x4][0x2]>(&Mau_gateway_table_addrmap_base->gateway_table_entry_matchdata), 0, {0x4,0x2}, sizeof(uint32_t) } },
    { "gateway_table_data_entry", { const_cast<uint32_t(*)[0x4][0x2]>(&Mau_gateway_table_addrmap_base->gateway_table_data_entry), 0, {0x4,0x2}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_exactmatch_row_vh_xbar_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_exactmatch_row_vh_xbar_addrmap Mau_exactmatch_row_vh_xbar_addrmap_base=0;
  Mau_exactmatch_row_vh_xbar_addrmap_map() : RegisterMapper( {
    { "exactmatch_row_vh_xbar_ctl", { const_cast<uint32_t(*)>(&Mau_exactmatch_row_vh_xbar_addrmap_base->exactmatch_row_vh_xbar_ctl), 0, {}, sizeof(uint32_t) } },
    { "stateful_meter_alu_data_ctl", { const_cast<uint32_t(*)>(&Mau_exactmatch_row_vh_xbar_addrmap_base->stateful_meter_alu_data_ctl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_action_output_hv_xbar_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_action_output_hv_xbar_addrmap Mau_action_output_hv_xbar_addrmap_base=0;
  Mau_action_output_hv_xbar_addrmap_map() : RegisterMapper( {
    { "mau_diag_row_adb_clk_enable", { const_cast<uint32_t(*)>(&Mau_action_output_hv_xbar_addrmap_base->mau_diag_row_adb_clk_enable), 0, {}, sizeof(uint32_t) } },
    { "action_hv_xbar_disable_ram_adr", { const_cast<uint32_t(*)>(&Mau_action_output_hv_xbar_addrmap_base->action_hv_xbar_disable_ram_adr), 0, {}, sizeof(uint32_t) } },
    { "action_hv_ixbar_ctl_byte", { const_cast<uint32_t(*)[0x2]>(&Mau_action_output_hv_xbar_addrmap_base->action_hv_ixbar_ctl_byte), 0, {0x2}, sizeof(uint32_t) } },
    { "action_hv_ixbar_input_bytemask", { const_cast<uint32_t(*)[0x2]>(&Mau_action_output_hv_xbar_addrmap_base->action_hv_ixbar_input_bytemask), 0, {0x2}, sizeof(uint32_t) } },
    { "action_hv_ixbar_ctl_halfword", { const_cast<uint32_t(*)[0x3][0x2]>(&Mau_action_output_hv_xbar_addrmap_base->action_hv_ixbar_ctl_halfword), 0, {0x3,0x2}, sizeof(uint32_t) } },
    { "action_hv_ixbar_ctl_word", { const_cast<uint32_t(*)[0x4][0x2]>(&Mau_action_output_hv_xbar_addrmap_base->action_hv_ixbar_ctl_word), 0, {0x4,0x2}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_unit_ram_row_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_unit_ram_row_addrmap Mau_unit_ram_row_addrmap_base=0;
  Mau_unit_ram_row_addrmap_map() : RegisterMapper( {
    { "ram", { &Mau_unit_ram_row_addrmap_base->ram, new Mau_unit_ram_addrmap_map, {0xc}, sizeof(Mau_unit_ram_addrmap) } },
    { "exactmatch_row_vh_xbar_byteswizzle_ctl", { const_cast<uint32_t(*)[0x2][0x10][0x8]>(&Mau_unit_ram_row_addrmap_base->exactmatch_row_vh_xbar_byteswizzle_ctl), 0, {0x2,0x10,0x8}, sizeof(uint32_t) } },
    { "vh_adr_xbar", { &Mau_unit_ram_row_addrmap_base->vh_adr_xbar, new Mau_exactmatch_row_adr_vh_xbar_addrmap_map, {}, sizeof(Mau_exactmatch_row_adr_vh_xbar_addrmap) } },
    { "stash", { &Mau_unit_ram_row_addrmap_base->stash, new Mau_stash_addrmap_map, {}, sizeof(Mau_stash_addrmap) } },
    { "gateway_table", { &Mau_unit_ram_row_addrmap_base->gateway_table, new Mau_gateway_table_addrmap_map, {0x2}, sizeof(Mau_gateway_table_addrmap) } },
    { "vh_xbar", { &Mau_unit_ram_row_addrmap_base->vh_xbar, new Mau_exactmatch_row_vh_xbar_addrmap_map, {0x2}, sizeof(Mau_exactmatch_row_vh_xbar_addrmap) } },
    { "intr_freeze_enable_mau_unit_ram_row", { const_cast<uint32_t(*)>(&Mau_unit_ram_row_addrmap_base->intr_freeze_enable_mau_unit_ram_row), 0, {}, sizeof(uint32_t) } },
    { "tind_ecc_error_uram_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_unit_ram_row_addrmap_base->tind_ecc_error_uram_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "emm_ecc_error_uram_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_unit_ram_row_addrmap_base->emm_ecc_error_uram_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "actiondata_error_uram_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_unit_ram_row_addrmap_base->actiondata_error_uram_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "intr_status_mau_unit_ram_row", { const_cast<uint32_t(*)>(&Mau_unit_ram_row_addrmap_base->intr_status_mau_unit_ram_row), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_unit_ram_row", { const_cast<uint32_t(*)>(&Mau_unit_ram_row_addrmap_base->intr_enable0_mau_unit_ram_row), 0, {}, sizeof(uint32_t) } },
    { "intr_enable1_mau_unit_ram_row", { const_cast<uint32_t(*)>(&Mau_unit_ram_row_addrmap_base->intr_enable1_mau_unit_ram_row), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_unit_ram_row", { const_cast<uint32_t(*)>(&Mau_unit_ram_row_addrmap_base->intr_inject_mau_unit_ram_row), 0, {}, sizeof(uint32_t) } },
    { "action_hv_xbar", { &Mau_unit_ram_row_addrmap_base->action_hv_xbar, new Mau_action_output_hv_xbar_addrmap_map, {}, sizeof(Mau_action_output_hv_xbar_addrmap) } }
    } )
  {}
};

struct Mau_unit_ram_array_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_unit_ram_array_addrmap Mau_unit_ram_array_addrmap_base=0;
  Mau_unit_ram_array_addrmap_map() : RegisterMapper( {
    { "switchbox", { &Mau_unit_ram_array_addrmap_base->switchbox, new Mau_memory_data_hv_switchbox_addrmap_map, {}, sizeof(Mau_memory_data_hv_switchbox_addrmap) } },
    { "row", { &Mau_unit_ram_array_addrmap_base->row, new Mau_unit_ram_row_addrmap_map, {0x8}, sizeof(Mau_unit_ram_row_addrmap) } }
    } )
  {}
};

struct Mau_read_adr_vh_switchbox_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_read_adr_vh_switchbox_addrmap Mau_read_adr_vh_switchbox_addrmap_base=0;
  Mau_read_adr_vh_switchbox_addrmap_map() : RegisterMapper( {
    { "adr_dist_idletime_adr_xbar_ctl", { const_cast<uint32_t(*)[0x6]>(&Mau_read_adr_vh_switchbox_addrmap_base->adr_dist_idletime_adr_xbar_ctl), 0, {0x6}, sizeof(uint32_t) } },
    { "adr_dist_oflo2_adr_xbar_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_read_adr_vh_switchbox_addrmap_base->adr_dist_oflo2_adr_xbar_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "adr_dist_tind_adr_xbar_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_read_adr_vh_switchbox_addrmap_base->adr_dist_tind_adr_xbar_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "adr_dist_oflo_adr_xbar_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_read_adr_vh_switchbox_addrmap_base->adr_dist_oflo_adr_xbar_ctl), 0, {0x2}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_dpram_array_row_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_dpram_array_row_addrmap Mau_dpram_array_row_addrmap_base=0;
  Mau_dpram_array_row_addrmap_map() : RegisterMapper( {
    { "synth2port_vpn_ctl", { const_cast<uint32_t(*)>(&Mau_dpram_array_row_addrmap_base->synth2port_vpn_ctl), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_synth2port", { const_cast<uint32_t(*)>(&Mau_dpram_array_row_addrmap_base->intr_status_mau_synth2port), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_synth2port", { const_cast<uint32_t(*)>(&Mau_dpram_array_row_addrmap_base->intr_enable0_mau_synth2port), 0, {}, sizeof(uint32_t) } },
    { "intr_enable1_mau_synth2port", { const_cast<uint32_t(*)>(&Mau_dpram_array_row_addrmap_base->intr_enable1_mau_synth2port), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_synth2port", { const_cast<uint32_t(*)>(&Mau_dpram_array_row_addrmap_base->intr_inject_mau_synth2port), 0, {}, sizeof(uint32_t) } },
    { "mau_synth2port_errlog", { const_cast<uint32_t(*)>(&Mau_dpram_array_row_addrmap_base->mau_synth2port_errlog), 0, {}, sizeof(uint32_t) } },
    { "mau_synth2port_error_ctl", { const_cast<uint32_t(*)>(&Mau_dpram_array_row_addrmap_base->mau_synth2port_error_ctl), 0, {}, sizeof(uint32_t) } },
    { "synth2port_ctl", { const_cast<uint32_t(*)>(&Mau_dpram_array_row_addrmap_base->synth2port_ctl), 0, {}, sizeof(uint32_t) } },
    { "synth2port_fabric_ctl", { const_cast<uint32_t(*)[0x3][0x2]>(&Mau_dpram_array_row_addrmap_base->synth2port_fabric_ctl), 0, {0x3,0x2}, sizeof(uint32_t) } },
    { "synth2port_hbus_members", { const_cast<uint32_t(*)[0x3][0x2]>(&Mau_dpram_array_row_addrmap_base->synth2port_hbus_members), 0, {0x3,0x2}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_adrmux_row_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_adrmux_row_addrmap Mau_adrmux_row_addrmap_base=0;
  Mau_adrmux_row_addrmap_map() : RegisterMapper( {
    { "atomic_mod_shadow_ram_status_left", { const_cast<uint32_t(*)[0x6]>(&Mau_adrmux_row_addrmap_base->atomic_mod_shadow_ram_status_left), 0, {0x6}, sizeof(uint32_t) } },
    { "atomic_mod_shadow_ram_status_right", { const_cast<uint32_t(*)[0x6]>(&Mau_adrmux_row_addrmap_base->atomic_mod_shadow_ram_status_right), 0, {0x6}, sizeof(uint32_t) } },
    { "mapram_sbe_inj", { const_cast<uint32_t(*)>(&Mau_adrmux_row_addrmap_base->mapram_sbe_inj), 0, {}, sizeof(uint32_t) } },
    { "mapram_mbe_inj", { const_cast<uint32_t(*)>(&Mau_adrmux_row_addrmap_base->mapram_mbe_inj), 0, {}, sizeof(uint32_t) } },
    { "adrmux_row_mem_slow_mode", { const_cast<uint32_t(*)>(&Mau_adrmux_row_addrmap_base->adrmux_row_mem_slow_mode), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_adrmux_row", { const_cast<uint32_t(*)>(&Mau_adrmux_row_addrmap_base->intr_status_mau_adrmux_row), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_adrmux_row", { const_cast<uint32_t(*)>(&Mau_adrmux_row_addrmap_base->intr_enable0_mau_adrmux_row), 0, {}, sizeof(uint32_t) } },
    { "intr_enable1_mau_adrmux_row", { const_cast<uint32_t(*)>(&Mau_adrmux_row_addrmap_base->intr_enable1_mau_adrmux_row), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_adrmux_row", { const_cast<uint32_t(*)>(&Mau_adrmux_row_addrmap_base->intr_inject_mau_adrmux_row), 0, {}, sizeof(uint32_t) } },
    { "intr_freeze_enable_mau_adrmux_row", { const_cast<uint32_t(*)>(&Mau_adrmux_row_addrmap_base->intr_freeze_enable_mau_adrmux_row), 0, {}, sizeof(uint32_t) } },
    { "ram_address_mux_ctl", { const_cast<uint32_t(*)[0x2][0x8]>(&Mau_adrmux_row_addrmap_base->ram_address_mux_ctl), 0, {0x2,0x8}, sizeof(uint32_t) } },
    { "unitram_config", { const_cast<uint32_t(*)[0x2][0x8]>(&Mau_adrmux_row_addrmap_base->unitram_config), 0, {0x2,0x8}, sizeof(uint32_t) } },
    { "mapram_config", { const_cast<uint32_t(*)[0x6]>(&Mau_adrmux_row_addrmap_base->mapram_config), 0, {0x6}, sizeof(uint32_t) } },
    { "mapram_sbe_errlog", { const_cast<uint32_t(*)[0x6]>(&Mau_adrmux_row_addrmap_base->mapram_sbe_errlog), 0, {0x6}, sizeof(uint32_t) } },
    { "mapram_mbe_errlog", { const_cast<uint32_t(*)[0x6]>(&Mau_adrmux_row_addrmap_base->mapram_mbe_errlog), 0, {0x6}, sizeof(uint32_t) } },
    { "idletime_logical_to_physical_sweep_grant_ctl", { const_cast<uint32_t(*)[0x6]>(&Mau_adrmux_row_addrmap_base->idletime_logical_to_physical_sweep_grant_ctl), 0, {0x6}, sizeof(uint32_t) } },
    { "idletime_physical_to_logical_req_inc_ctl", { const_cast<uint32_t(*)[0x6]>(&Mau_adrmux_row_addrmap_base->idletime_physical_to_logical_req_inc_ctl), 0, {0x6}, sizeof(uint32_t) } },
    { "idletime_cfg_rd_clear_val", { const_cast<uint32_t(*)[0x6]>(&Mau_adrmux_row_addrmap_base->idletime_cfg_rd_clear_val), 0, {0x6}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_map_and_alu_row_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_map_and_alu_row_addrmap Mau_map_and_alu_row_addrmap_base=0;
  Mau_map_and_alu_row_addrmap_map() : RegisterMapper( {
    { "vh_xbars", { &Mau_map_and_alu_row_addrmap_base->vh_xbars, new Mau_read_adr_vh_switchbox_addrmap_map, {}, sizeof(Mau_read_adr_vh_switchbox_addrmap) } },
    { "i2portctl", { &Mau_map_and_alu_row_addrmap_base->i2portctl, new Mau_dpram_array_row_addrmap_map, {}, sizeof(Mau_dpram_array_row_addrmap) } },
    { "adrmux", { &Mau_map_and_alu_row_addrmap_base->adrmux, new Mau_adrmux_row_addrmap_map, {}, sizeof(Mau_adrmux_row_addrmap) } }
    } )
  {}
};

struct Mau_mapram_color_write_switchbox_control_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_mapram_color_write_switchbox_control_addrmap Mau_mapram_color_write_switchbox_control_addrmap_base=0;
  Mau_mapram_color_write_switchbox_control_addrmap_map() : RegisterMapper( {
    { "r_oflo_color_write_o_mux_select", { const_cast<uint32_t(*)>(&Mau_mapram_color_write_switchbox_control_addrmap_base->r_oflo_color_write_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "b_oflo_color_write_o_mux_select", { const_cast<uint32_t(*)>(&Mau_mapram_color_write_switchbox_control_addrmap_base->b_oflo_color_write_o_mux_select), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_mapram_color_write_switchbox_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_mapram_color_write_switchbox_addrmap Mau_mapram_color_write_switchbox_addrmap_base=0;
  Mau_mapram_color_write_switchbox_addrmap_map() : RegisterMapper( {
    { "ctl", { &Mau_mapram_color_write_switchbox_addrmap_base->ctl, new Mau_mapram_color_write_switchbox_control_addrmap_map, {}, sizeof(Mau_mapram_color_write_switchbox_control_addrmap) } }
    } )
  {}
};

struct Mau_selector_action_adr_switchbox_row_control_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_selector_action_adr_switchbox_row_control_addrmap Mau_selector_action_adr_switchbox_row_control_addrmap_base=0;
  Mau_selector_action_adr_switchbox_row_control_addrmap_map() : RegisterMapper( {
    { "t_oflo2up_adr_o_mux_select", { const_cast<uint32_t(*)>(&Mau_selector_action_adr_switchbox_row_control_addrmap_base->t_oflo2up_adr_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "l_oflo_adr_o_mux_select", { const_cast<uint32_t(*)>(&Mau_selector_action_adr_switchbox_row_control_addrmap_base->l_oflo_adr_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "l_oflo2_adr_o_mux_select", { const_cast<uint32_t(*)>(&Mau_selector_action_adr_switchbox_row_control_addrmap_base->l_oflo2_adr_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "r_oflo_adr_o_mux_select", { const_cast<uint32_t(*)>(&Mau_selector_action_adr_switchbox_row_control_addrmap_base->r_oflo_adr_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "r_oflo2_adr_o_mux_select", { const_cast<uint32_t(*)>(&Mau_selector_action_adr_switchbox_row_control_addrmap_base->r_oflo2_adr_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "b_oflo_adr_o_mux_select", { const_cast<uint32_t(*)>(&Mau_selector_action_adr_switchbox_row_control_addrmap_base->b_oflo_adr_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "b_oflo2dn_adr_o_mux_select", { const_cast<uint32_t(*)>(&Mau_selector_action_adr_switchbox_row_control_addrmap_base->b_oflo2dn_adr_o_mux_select), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_selector_action_adr_switchbox_row_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_selector_action_adr_switchbox_row_addrmap Mau_selector_action_adr_switchbox_row_addrmap_base=0;
  Mau_selector_action_adr_switchbox_row_addrmap_map() : RegisterMapper( {
    { "ctl", { &Mau_selector_action_adr_switchbox_row_addrmap_base->ctl, new Mau_selector_action_adr_switchbox_row_control_addrmap_map, {}, sizeof(Mau_selector_action_adr_switchbox_row_control_addrmap) } }
    } )
  {}
};

struct Mau_selector_action_adr_switchbox_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_selector_action_adr_switchbox_addrmap Mau_selector_action_adr_switchbox_addrmap_base=0;
  Mau_selector_action_adr_switchbox_addrmap_map() : RegisterMapper( {
    { "row", { &Mau_selector_action_adr_switchbox_addrmap_base->row, new Mau_selector_action_adr_switchbox_row_addrmap_map, {0x4}, sizeof(Mau_selector_action_adr_switchbox_row_addrmap) } }
    } )
  {}
};

struct Mau_mapram_color_switchbox_row_control_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_mapram_color_switchbox_row_control_addrmap Mau_mapram_color_switchbox_row_control_addrmap_base=0;
  Mau_mapram_color_switchbox_row_control_addrmap_map() : RegisterMapper( {
    { "t_oflo_color_o_mux_select", { const_cast<uint32_t(*)>(&Mau_mapram_color_switchbox_row_control_addrmap_base->t_oflo_color_o_mux_select), 0, {}, sizeof(uint32_t) } },
    { "r_color0_mux_select", { const_cast<uint32_t(*)>(&Mau_mapram_color_switchbox_row_control_addrmap_base->r_color0_mux_select), 0, {}, sizeof(uint32_t) } },
    { "r_color1_mux_select", { const_cast<uint32_t(*)>(&Mau_mapram_color_switchbox_row_control_addrmap_base->r_color1_mux_select), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_mapram_color_switchbox_row_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_mapram_color_switchbox_row_addrmap Mau_mapram_color_switchbox_row_addrmap_base=0;
  Mau_mapram_color_switchbox_row_addrmap_map() : RegisterMapper( {
    { "ctl", { &Mau_mapram_color_switchbox_row_addrmap_base->ctl, new Mau_mapram_color_switchbox_row_control_addrmap_map, {}, sizeof(Mau_mapram_color_switchbox_row_control_addrmap) } }
    } )
  {}
};

struct Mau_mapram_color_switchbox_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_mapram_color_switchbox_addrmap Mau_mapram_color_switchbox_addrmap_base=0;
  Mau_mapram_color_switchbox_addrmap_map() : RegisterMapper( {
    { "row", { &Mau_mapram_color_switchbox_addrmap_base->row, new Mau_mapram_color_switchbox_row_addrmap_map, {0x8}, sizeof(Mau_mapram_color_switchbox_row_addrmap) } }
    } )
  {}
};

struct Mau_stats_alu_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_stats_alu_addrmap Mau_stats_alu_addrmap_base=0;
  Mau_stats_alu_addrmap_map() : RegisterMapper( {
    { "lrt_update_interval", { const_cast<uint32_t(*)[0x3]>(&Mau_stats_alu_addrmap_base->lrt_update_interval), 0, {0x3}, sizeof(uint32_t) } },
    { "mau_stats_alu_status_ctl", { const_cast<uint32_t(*)>(&Mau_stats_alu_addrmap_base->mau_stats_alu_status_ctl), 0, {}, sizeof(uint32_t) } },
    { "statistics_ctl", { const_cast<uint32_t(*)>(&Mau_stats_alu_addrmap_base->statistics_ctl), 0, {}, sizeof(uint32_t) } },
    { "statistics_ctl_teop_en", { const_cast<uint32_t(*)>(&Mau_stats_alu_addrmap_base->statistics_ctl_teop_en), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_stats_alu", { const_cast<uint32_t(*)>(&Mau_stats_alu_addrmap_base->mau_diag_stats_alu), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_stats_alu", { const_cast<uint32_t(*)>(&Mau_stats_alu_addrmap_base->intr_status_mau_stats_alu), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_stats_alu", { const_cast<uint32_t(*)>(&Mau_stats_alu_addrmap_base->intr_enable0_mau_stats_alu), 0, {}, sizeof(uint32_t) } },
    { "intr_enable1_mau_stats_alu", { const_cast<uint32_t(*)>(&Mau_stats_alu_addrmap_base->intr_enable1_mau_stats_alu), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_stats_alu", { const_cast<uint32_t(*)>(&Mau_stats_alu_addrmap_base->intr_inject_mau_stats_alu), 0, {}, sizeof(uint32_t) } },
    { "mau_stats_alu_status", { const_cast<uint32_t(*)>(&Mau_stats_alu_addrmap_base->mau_stats_alu_status), 0, {}, sizeof(uint32_t) } },
    { "lrt_threshold", { const_cast<uint32_t(*)[0x3]>(&Mau_stats_alu_addrmap_base->lrt_threshold), 0, {0x3}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_stats_alu_wrap_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_stats_alu_wrap_addrmap Mau_stats_alu_wrap_addrmap_base=0;
  Mau_stats_alu_wrap_addrmap_map() : RegisterMapper( {
    { "stats", { &Mau_stats_alu_wrap_addrmap_base->stats, new Mau_stats_alu_addrmap_map, {}, sizeof(Mau_stats_alu_addrmap) } }
    } )
  {}
};

struct Mau_meter_alu_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_meter_alu_addrmap Mau_meter_alu_addrmap_base=0;
  Mau_meter_alu_addrmap_map() : RegisterMapper( {
    { "meter_ctl_teop_en", { const_cast<uint32_t(*)>(&Mau_meter_alu_addrmap_base->meter_ctl_teop_en), 0, {}, sizeof(uint32_t) } },
    { "red_value_ctl", { const_cast<uint32_t(*)>(&Mau_meter_alu_addrmap_base->red_value_ctl), 0, {}, sizeof(uint32_t) } },
    { "meter_ctl", { const_cast<uint32_t(*)>(&Mau_meter_alu_addrmap_base->meter_ctl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_selector_alu_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_selector_alu_addrmap Mau_selector_alu_addrmap_base=0;
  Mau_selector_alu_addrmap_map() : RegisterMapper( {
    { "selector_alu_ctl", { const_cast<uint32_t(*)>(&Mau_selector_alu_addrmap_base->selector_alu_ctl), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_selector_alu", { const_cast<uint32_t(*)>(&Mau_selector_alu_addrmap_base->intr_status_mau_selector_alu), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_selector_alu", { const_cast<uint32_t(*)>(&Mau_selector_alu_addrmap_base->intr_enable0_mau_selector_alu), 0, {}, sizeof(uint32_t) } },
    { "intr_enable1_mau_selector_alu", { const_cast<uint32_t(*)>(&Mau_selector_alu_addrmap_base->intr_enable1_mau_selector_alu), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_selector_alu", { const_cast<uint32_t(*)>(&Mau_selector_alu_addrmap_base->intr_inject_mau_selector_alu), 0, {}, sizeof(uint32_t) } },
    { "mau_selector_alu_errlog", { const_cast<uint32_t(*)>(&Mau_selector_alu_addrmap_base->mau_selector_alu_errlog), 0, {}, sizeof(uint32_t) } },
    { "mau_meter_alu_group_status", { const_cast<uint32_t(*)>(&Mau_selector_alu_addrmap_base->mau_meter_alu_group_status), 0, {}, sizeof(uint32_t) } },
    { "mau_meter_alu_group_status_ctl", { const_cast<uint32_t(*)>(&Mau_selector_alu_addrmap_base->mau_meter_alu_group_status_ctl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_stateful_alu_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_stateful_alu_addrmap Mau_stateful_alu_addrmap_base=0;
  Mau_stateful_alu_addrmap_map() : RegisterMapper( {
    { "mau_diag_meter_alu_group", { const_cast<uint32_t(*)>(&Mau_stateful_alu_addrmap_base->mau_diag_meter_alu_group), 0, {}, sizeof(uint32_t) } },
    { "stateful_clear_action_output", { const_cast<uint32_t(*)>(&Mau_stateful_alu_addrmap_base->stateful_clear_action_output), 0, {}, sizeof(uint32_t) } },
    { "salu_const_regfile", { const_cast<uint32_t(*)[0x4]>(&Mau_stateful_alu_addrmap_base->salu_const_regfile), 0, {0x4}, sizeof(uint32_t) } },
    { "salu_const_regfile_msbs", { const_cast<uint32_t(*)[0x4]>(&Mau_stateful_alu_addrmap_base->salu_const_regfile_msbs), 0, {0x4}, sizeof(uint32_t) } },
    { "salu_mathtable", { const_cast<uint32_t(*)[0x4]>(&Mau_stateful_alu_addrmap_base->salu_mathtable), 0, {0x4}, sizeof(uint32_t) } },
    { "salu_instr_common", { const_cast<uint32_t(*)[0x4]>(&Mau_stateful_alu_addrmap_base->salu_instr_common), 0, {0x4}, sizeof(uint32_t) } },
    { "tmatch_mask", { const_cast<uint32_t(*)[0x2][0x2]>(&Mau_stateful_alu_addrmap_base->tmatch_mask), 0, {0x2,0x2}, sizeof(uint32_t) } },
    { "salu_instr_state_alu", { const_cast<uint32_t(*)[0x4][0x4]>(&Mau_stateful_alu_addrmap_base->salu_instr_state_alu), 0, {0x4,0x4}, sizeof(uint32_t) } },
    { "salu_instr2_state_alu", { const_cast<uint32_t(*)[0x4][0x4]>(&Mau_stateful_alu_addrmap_base->salu_instr2_state_alu), 0, {0x4,0x4}, sizeof(uint32_t) } },
    { "salu_instr_cmp_alu", { const_cast<uint32_t(*)[0x4][0x4]>(&Mau_stateful_alu_addrmap_base->salu_instr_cmp_alu), 0, {0x4,0x4}, sizeof(uint32_t) } },
    { "salu_instr_output_alu", { const_cast<uint32_t(*)[0x4][0x4]>(&Mau_stateful_alu_addrmap_base->salu_instr_output_alu), 0, {0x4,0x4}, sizeof(uint32_t) } },
    { "stateful_pred_intr", { const_cast<uint32_t(*)>(&Mau_stateful_alu_addrmap_base->stateful_pred_intr), 0, {}, sizeof(uint32_t) } },
    { "salu_mathunit_ctl", { const_cast<uint32_t(*)>(&Mau_stateful_alu_addrmap_base->salu_mathunit_ctl), 0, {}, sizeof(uint32_t) } },
    { "stateful_ctl", { const_cast<uint32_t(*)>(&Mau_stateful_alu_addrmap_base->stateful_ctl), 0, {}, sizeof(uint32_t) } },
    { "salu_instr_tmatch_alu", { const_cast<uint32_t(*)[0x4][0x2]>(&Mau_stateful_alu_addrmap_base->salu_instr_tmatch_alu), 0, {0x4,0x2}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_meter_alu_group_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_meter_alu_group_addrmap Mau_meter_alu_group_addrmap_base=0;
  Mau_meter_alu_group_addrmap_map() : RegisterMapper( {
    { "meter", { &Mau_meter_alu_group_addrmap_base->meter, new Mau_meter_alu_addrmap_map, {}, sizeof(Mau_meter_alu_addrmap) } },
    { "selector", { &Mau_meter_alu_group_addrmap_base->selector, new Mau_selector_alu_addrmap_map, {}, sizeof(Mau_selector_alu_addrmap) } },
    { "stateful", { &Mau_meter_alu_group_addrmap_base->stateful, new Mau_stateful_alu_addrmap_map, {}, sizeof(Mau_stateful_alu_addrmap) } }
    } )
  {}
};

struct Mau_map_and_alu_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_map_and_alu_addrmap Mau_map_and_alu_addrmap_base=0;
  Mau_map_and_alu_addrmap_map() : RegisterMapper( {
    { "row", { &Mau_map_and_alu_addrmap_base->row, new Mau_map_and_alu_row_addrmap_map, {0x8}, sizeof(Mau_map_and_alu_row_addrmap) } },
    { "meter_alu_group_phv_hash_mask", { const_cast<uint32_t(*)[0x4][0x4]>(&Mau_map_and_alu_addrmap_base->meter_alu_group_phv_hash_mask), 0, {0x4,0x4}, sizeof(uint32_t) } },
    { "meter_alu_group_phv_hash_shift", { const_cast<uint32_t(*)[0x4]>(&Mau_map_and_alu_addrmap_base->meter_alu_group_phv_hash_shift), 0, {0x4}, sizeof(uint32_t) } },
    { "mapram_color_write_switchbox", { &Mau_map_and_alu_addrmap_base->mapram_color_write_switchbox, new Mau_mapram_color_write_switchbox_addrmap_map, {0x4}, sizeof(Mau_mapram_color_write_switchbox_addrmap) } },
    { "mau_selector_action_adr_shift", { const_cast<uint32_t(*)[0x8]>(&Mau_map_and_alu_addrmap_base->mau_selector_action_adr_shift), 0, {0x8}, sizeof(uint32_t) } },
    { "intr_mau_decode_memory_core", { const_cast<uint32_t(*)[0x2]>(&Mau_map_and_alu_addrmap_base->intr_mau_decode_memory_core), 0, {0x2}, sizeof(uint32_t) } },
    { "meter_alu_group_data_delay_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_map_and_alu_addrmap_base->meter_alu_group_data_delay_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "selector_adr_switchbox", { &Mau_map_and_alu_addrmap_base->selector_adr_switchbox, new Mau_selector_action_adr_switchbox_addrmap_map, {}, sizeof(Mau_selector_action_adr_switchbox_addrmap) } },
    { "mapram_color_switchbox", { &Mau_map_and_alu_addrmap_base->mapram_color_switchbox, new Mau_mapram_color_switchbox_addrmap_map, {}, sizeof(Mau_mapram_color_switchbox_addrmap) } },
    { "meter_alu_group_action_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_map_and_alu_addrmap_base->meter_alu_group_action_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "meter_alu_group_error_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_map_and_alu_addrmap_base->meter_alu_group_error_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "selector_action_adr_fallback", { const_cast<uint32_t(*)[0x8][0x2]>(&Mau_map_and_alu_addrmap_base->selector_action_adr_fallback), 0, {0x8,0x2}, sizeof(uint32_t) } },
    { "stats_wrap", { &Mau_map_and_alu_addrmap_base->stats_wrap, new Mau_stats_alu_wrap_addrmap_map, {0x4}, sizeof(Mau_stats_alu_wrap_addrmap) } },
    { "meter_group", { &Mau_map_and_alu_addrmap_base->meter_group, new Mau_meter_alu_group_addrmap_map, {0x4}, sizeof(Mau_meter_alu_group_addrmap) } }
    } )
  {}
};

struct Mau_address_distribution_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_address_distribution_addrmap Mau_address_distribution_addrmap_base=0;
  Mau_address_distribution_addrmap_map() : RegisterMapper( {
    { "meter_to_teop_adr_oxbar_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->meter_to_teop_adr_oxbar_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "teop_bus_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->teop_bus_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_snapshot_meter_adr", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->mau_snapshot_meter_adr), 0, {0x4}, sizeof(uint32_t) } },
    { "immediate_data_8b_enable", { const_cast<uint32_t(*)[0x2]>(&Mau_address_distribution_addrmap_base->immediate_data_8b_enable), 0, {0x2}, sizeof(uint32_t) } },
    { "deferred_eop_bus_delay", { const_cast<uint32_t(*)[0x2]>(&Mau_address_distribution_addrmap_base->deferred_eop_bus_delay), 0, {0x2}, sizeof(uint32_t) } },
    { "movereg_ad_stats_alu_to_logical_xbar_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_address_distribution_addrmap_base->movereg_ad_stats_alu_to_logical_xbar_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "movereg_ad_meter_alu_to_logical_xbar_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_address_distribution_addrmap_base->movereg_ad_meter_alu_to_logical_xbar_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "movereg_idle_pop_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_address_distribution_addrmap_base->movereg_idle_pop_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "stats_bubble_req", { const_cast<uint32_t(*)[0x2]>(&Mau_address_distribution_addrmap_base->stats_bubble_req), 0, {0x2}, sizeof(uint32_t) } },
    { "meter_bubble_req", { const_cast<uint32_t(*)[0x2]>(&Mau_address_distribution_addrmap_base->meter_bubble_req), 0, {0x2}, sizeof(uint32_t) } },
    { "idle_bubble_req", { const_cast<uint32_t(*)[0x2]>(&Mau_address_distribution_addrmap_base->idle_bubble_req), 0, {0x2}, sizeof(uint32_t) } },
    { "bubble_req_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_address_distribution_addrmap_base->bubble_req_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "oflo_adr_user", { const_cast<uint32_t(*)[0x2]>(&Mau_address_distribution_addrmap_base->oflo_adr_user), 0, {0x2}, sizeof(uint32_t) } },
    { "atomic_mod_sram_go_pending", { const_cast<uint32_t(*)[0x2]>(&Mau_address_distribution_addrmap_base->atomic_mod_sram_go_pending), 0, {0x2}, sizeof(uint32_t) } },
    { "meter_enable", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->meter_enable), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_adr_dist_idletime_adr_oxbar_ctl", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->mau_diag_adr_dist_idletime_adr_oxbar_ctl), 0, {}, sizeof(uint32_t) } },
    { "deferred_oflo_ctl", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->deferred_oflo_ctl), 0, {}, sizeof(uint32_t) } },
    { "adr_dist_mem_slow_mode", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->adr_dist_mem_slow_mode), 0, {}, sizeof(uint32_t) } },
    { "immediate_data_rng_enable", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->immediate_data_rng_enable), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_32b_oxbar_ctl", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->mau_diag_32b_oxbar_ctl), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_32b_oxbar_premux_ctl", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->mau_diag_32b_oxbar_premux_ctl), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_8b_oxbar_ctl", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->mau_diag_8b_oxbar_ctl), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_adb_map", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->mau_diag_adb_map), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_stats_adr_sel", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->mau_diag_stats_adr_sel), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_meter_adr_sel", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->mau_diag_meter_adr_sel), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_eop_vld_xport", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->mau_diag_eop_vld_xport), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_ad", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->intr_status_mau_ad), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_ad", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->intr_enable0_mau_ad), 0, {}, sizeof(uint32_t) } },
    { "immediate_data_8b_ixbar_ctl", { const_cast<uint32_t(*)[0x20]>(&Mau_address_distribution_addrmap_base->immediate_data_8b_ixbar_ctl), 0, {0x20}, sizeof(uint32_t) } },
    { "immediate_data_16b_ixbar_ctl", { const_cast<uint32_t(*)[0x20]>(&Mau_address_distribution_addrmap_base->immediate_data_16b_ixbar_ctl), 0, {0x20}, sizeof(uint32_t) } },
    { "immediate_data_32b_ixbar_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->immediate_data_32b_ixbar_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "meter_color_output_map", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->meter_color_output_map), 0, {0x10}, sizeof(uint32_t) } },
    { "meter_color_logical_to_phys_icxbar_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->meter_color_logical_to_phys_icxbar_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "adr_dist_action_data_adr_icxbar_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->adr_dist_action_data_adr_icxbar_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "adr_dist_stats_adr_icxbar_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->adr_dist_stats_adr_icxbar_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "adr_dist_meter_adr_icxbar_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->adr_dist_meter_adr_icxbar_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "idletime_sweep_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->idletime_sweep_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "idletime_slip", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->idletime_slip), 0, {0x10}, sizeof(uint32_t) } },
    { "idletime_slip_intr_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->idletime_slip_intr_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "movereg_idle_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->movereg_idle_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_stateful_log_counter_logical_map", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->mau_stateful_log_counter_logical_map), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_stateful_log_stage_vpn_offset", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->mau_stateful_log_stage_vpn_offset), 0, {0x10}, sizeof(uint32_t) } },
    { "stateful_instr_width_logical", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->stateful_instr_width_logical), 0, {0x10}, sizeof(uint32_t) } },
    { "intr_enable1_mau_ad", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->intr_enable1_mau_ad), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_ad", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->intr_inject_mau_ad), 0, {}, sizeof(uint32_t) } },
    { "intr_freeze_enable_mau_ad", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->intr_freeze_enable_mau_ad), 0, {}, sizeof(uint32_t) } },
    { "idletime_slip_errlog", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->idletime_slip_errlog), 0, {}, sizeof(uint32_t) } },
    { "adr_dist_idletime_adr_oxbar_ctl", { const_cast<uint32_t(*)[0x5]>(&Mau_address_distribution_addrmap_base->adr_dist_idletime_adr_oxbar_ctl), 0, {0x5}, sizeof(uint32_t) } },
    { "packet_action_at_headertime", { const_cast<uint32_t(*)[0x2][0x4]>(&Mau_address_distribution_addrmap_base->packet_action_at_headertime), 0, {0x2,0x4}, sizeof(uint32_t) } },
    { "deferred_ram_ctl", { const_cast<uint32_t(*)[0x2][0x4]>(&Mau_address_distribution_addrmap_base->deferred_ram_ctl), 0, {0x2,0x4}, sizeof(uint32_t) } },
    { "meter_sweep_errlog", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->meter_sweep_errlog), 0, {}, sizeof(uint32_t) } },
    { "meter_adr_shift", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->meter_adr_shift), 0, {}, sizeof(uint32_t) } },
    { "meter_sweep_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->meter_sweep_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "meter_sweep_cmd_ovr_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->meter_sweep_cmd_ovr_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "meter_sweep_num_subwords", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->meter_sweep_num_subwords), 0, {0x4}, sizeof(uint32_t) } },
    { "adr_dist_pipe_delay", { const_cast<uint32_t(*)[0x2][0x2]>(&Mau_address_distribution_addrmap_base->adr_dist_pipe_delay), 0, {0x2,0x2}, sizeof(uint32_t) } },
    { "stats_lrt_fsm_sweep_size", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->stats_lrt_fsm_sweep_size), 0, {0x4}, sizeof(uint32_t) } },
    { "stats_lrt_fsm_sweep_offset", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->stats_lrt_fsm_sweep_offset), 0, {0x4}, sizeof(uint32_t) } },
    { "stats_lrt_sweep_adr", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->stats_lrt_sweep_adr), 0, {0x4}, sizeof(uint32_t) } },
    { "immediate_data_rng_logical_map_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->immediate_data_rng_logical_map_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "movereg_ad_direct", { const_cast<uint32_t(*)[0x3]>(&Mau_address_distribution_addrmap_base->movereg_ad_direct), 0, {0x3}, sizeof(uint32_t) } },
    { "movereg_stats_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->movereg_stats_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "movereg_meter_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->movereg_meter_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_ad_meter_virt_lt", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->mau_ad_meter_virt_lt), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_ad_stats_virt_lt", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->mau_ad_stats_virt_lt), 0, {0x4}, sizeof(uint32_t) } },
    { "deferred_stats_parity_control", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->deferred_stats_parity_control), 0, {0x4}, sizeof(uint32_t) } },
    { "deferred_stats_parity_errlog", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->deferred_stats_parity_errlog), 0, {0x4}, sizeof(uint32_t) } },
    { "deferred_meter_parity_control", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->deferred_meter_parity_control), 0, {0x4}, sizeof(uint32_t) } },
    { "def_meter_sbe_errlog", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->def_meter_sbe_errlog), 0, {0x4}, sizeof(uint32_t) } },
    { "adr_dist_table_thread", { const_cast<uint32_t(*)[0x2][0x2]>(&Mau_address_distribution_addrmap_base->adr_dist_table_thread), 0, {0x2,0x2}, sizeof(uint32_t) } },
    { "mau_stats_alu_vpn_range", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->mau_stats_alu_vpn_range), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_meter_alu_vpn_range", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->mau_meter_alu_vpn_range), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_stateful_log_counter_oxbar_map", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->mau_stateful_log_counter_oxbar_map), 0, {0x4}, sizeof(uint32_t) } },
    { "meter_alu_adr_range_check_icxbar_map", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->meter_alu_adr_range_check_icxbar_map), 0, {0x4}, sizeof(uint32_t) } },
    { "dp_teop_stats_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->dp_teop_stats_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "dp_teop_meter_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->dp_teop_meter_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "teop_to_stats_adr_oxbar_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->teop_to_stats_adr_oxbar_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "teop_to_meter_adr_oxbar_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->teop_to_meter_adr_oxbar_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "stats_to_teop_adr_oxbar_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->stats_to_teop_adr_oxbar_ctl), 0, {0x4}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_match_merge_col_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_match_merge_col_addrmap Mau_match_merge_col_addrmap_base=0;
  Mau_match_merge_col_addrmap_map() : RegisterMapper( {
    { "row_action_nxtable_bus_drive", { const_cast<uint32_t(*)[0x8]>(&Mau_match_merge_col_addrmap_base->row_action_nxtable_bus_drive), 0, {0x8}, sizeof(uint32_t) } },
    { "hitmap_output_map", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_col_addrmap_base->hitmap_output_map), 0, {0x10}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_match_merge_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_match_merge_addrmap Mau_match_merge_addrmap_base=0;
  Mau_match_merge_addrmap_map() : RegisterMapper( {
    { "mau_hash_group_xbar_ctl", { const_cast<uint32_t(*)[0x6][0x2]>(&Mau_match_merge_addrmap_base->mau_hash_group_xbar_ctl), 0, {0x6,0x2}, sizeof(uint32_t) } },
    { "mpr_glob_exec_lut", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mpr_glob_exec_lut), 0, {0x10}, sizeof(uint32_t) } },
    { "stash_hitmap_output_map", { const_cast<uint32_t(*)[0x2][0x8]>(&Mau_match_merge_addrmap_base->stash_hitmap_output_map), 0, {0x2,0x8}, sizeof(uint32_t) } },
    { "stash_next_table_lut", { const_cast<uint32_t(*)[0x2][0x8]>(&Mau_match_merge_addrmap_base->stash_next_table_lut), 0, {0x2,0x8}, sizeof(uint32_t) } },
    { "stash_row_nxtable_bus_drive", { const_cast<uint32_t(*)[0x2][0x8]>(&Mau_match_merge_addrmap_base->stash_row_nxtable_bus_drive), 0, {0x2,0x8}, sizeof(uint32_t) } },
    { "gateway_to_logicaltable_xbar_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->gateway_to_logicaltable_xbar_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "gateway_inhibit_lut", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->gateway_inhibit_lut), 0, {0x10}, sizeof(uint32_t) } },
    { "gateway_to_pbus_xbar_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->gateway_to_pbus_xbar_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "exit_gateway_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->exit_gateway_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "tind_bus_prop", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->tind_bus_prop), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_snapshot_physical_tcam_hit_address", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_snapshot_physical_tcam_hit_address), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_snapshot_physical_exact_match_hit_address", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_snapshot_physical_exact_match_hit_address), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_table_counter", { const_cast<uint32_t(*)[0x10][0x1]>(&Mau_match_merge_addrmap_base->mau_table_counter), 0, {0x10,0x1}, sizeof(uint32_t) } },
    { "mau_table_counter_clear", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_table_counter_clear), 0, {}, sizeof(uint32_t) } },
    { "mau_stateful_log_counter_clear", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_stateful_log_counter_clear), 0, {}, sizeof(uint32_t) } },
    { "actiondata_error_ctl", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->actiondata_error_ctl), 0, {}, sizeof(uint32_t) } },
    { "imem_parity_error_ctl", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->imem_parity_error_ctl), 0, {}, sizeof(uint32_t) } },
    { "tcam_hit_to_logical_table_ixbar_outputmap", { const_cast<uint32_t(*)[0x8]>(&Mau_match_merge_addrmap_base->tcam_hit_to_logical_table_ixbar_outputmap), 0, {0x8}, sizeof(uint32_t) } },
    { "mau_hash_group_mask", { const_cast<uint32_t(*)[0x6]>(&Mau_match_merge_addrmap_base->mau_hash_group_mask), 0, {0x6}, sizeof(uint32_t) } },
    { "mpr_long_brch_lut", { const_cast<uint32_t(*)[0x8]>(&Mau_match_merge_addrmap_base->mpr_long_brch_lut), 0, {0x8}, sizeof(uint32_t) } },
    { "gateway_payload_tind_pbus", { const_cast<uint32_t(*)[0x8]>(&Mau_match_merge_addrmap_base->gateway_payload_tind_pbus), 0, {0x8}, sizeof(uint32_t) } },
    { "gateway_payload_exact_pbus", { const_cast<uint32_t(*)[0x8]>(&Mau_match_merge_addrmap_base->gateway_payload_exact_pbus), 0, {0x8}, sizeof(uint32_t) } },
    { "col", { &Mau_match_merge_addrmap_base->col, new Mau_match_merge_col_addrmap_map, {0xc}, sizeof(Mau_match_merge_col_addrmap) } },
    { "gateway_payload_exact_disable", { const_cast<uint32_t(*)[0x8]>(&Mau_match_merge_addrmap_base->gateway_payload_exact_disable), 0, {0x8}, sizeof(uint32_t) } },
    { "gateway_payload_tind_disable", { const_cast<uint32_t(*)[0x8]>(&Mau_match_merge_addrmap_base->gateway_payload_tind_disable), 0, {0x8}, sizeof(uint32_t) } },
    { "tcam_table_prop", { const_cast<uint32_t(*)[0x8]>(&Mau_match_merge_addrmap_base->tcam_table_prop), 0, {0x8}, sizeof(uint32_t) } },
    { "mau_stateful_log_counter", { const_cast<uint32_t(*)[0x4][0x2]>(&Mau_match_merge_addrmap_base->mau_stateful_log_counter), 0, {0x4,0x2}, sizeof(uint32_t) } },
    { "mau_stateful_log_ctl_ixbar_map", { const_cast<uint32_t(*)[0x2][0x2][0x2]>(&Mau_match_merge_addrmap_base->mau_stateful_log_ctl_ixbar_map), 0, {0x2,0x2,0x2}, sizeof(uint32_t) } },
    { "exact_match_delay_thread", { const_cast<uint32_t(*)[0x3]>(&Mau_match_merge_addrmap_base->exact_match_delay_thread), 0, {0x3}, sizeof(uint32_t) } },
    { "logical_table_thread", { const_cast<uint32_t(*)[0x3]>(&Mau_match_merge_addrmap_base->logical_table_thread), 0, {0x3}, sizeof(uint32_t) } },
    { "pred_glob_exec_thread", { const_cast<uint32_t(*)[0x3]>(&Mau_match_merge_addrmap_base->pred_glob_exec_thread), 0, {0x3}, sizeof(uint32_t) } },
    { "pred_long_brch_thread", { const_cast<uint32_t(*)[0x3]>(&Mau_match_merge_addrmap_base->pred_long_brch_thread), 0, {0x3}, sizeof(uint32_t) } },
    { "pred_always_run", { const_cast<uint32_t(*)[0x3]>(&Mau_match_merge_addrmap_base->pred_always_run), 0, {0x3}, sizeof(uint32_t) } },
    { "mau_mapram_color_map_to_logical_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_match_merge_addrmap_base->mau_mapram_color_map_to_logical_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_physical_to_meter_alu_icxbar_map", { const_cast<uint32_t(*)[0x2][0x2]>(&Mau_match_merge_addrmap_base->mau_physical_to_meter_alu_icxbar_map), 0, {0x2,0x2}, sizeof(uint32_t) } },
    { "mau_meter_precolor_hash_map_to_logical_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_match_merge_addrmap_base->mau_meter_precolor_hash_map_to_logical_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_selector_action_entry_size", { const_cast<uint32_t(*)[0x4]>(&Mau_match_merge_addrmap_base->mau_selector_action_entry_size), 0, {0x4}, sizeof(uint32_t) } },
    { "selector_action_adr_shift", { const_cast<uint32_t(*)[0x4]>(&Mau_match_merge_addrmap_base->selector_action_adr_shift), 0, {0x4}, sizeof(uint32_t) } },
    { "mpr_stage_id", { const_cast<uint32_t(*)[0x3]>(&Mau_match_merge_addrmap_base->mpr_stage_id), 0, {0x3}, sizeof(uint32_t) } },
    { "mau_snapshot_next_table_out", { const_cast<uint32_t(*)[0x3]>(&Mau_match_merge_addrmap_base->mau_snapshot_next_table_out), 0, {0x3}, sizeof(uint32_t) } },
    { "mau_snapshot_mpr_next_table_out", { const_cast<uint32_t(*)[0x3]>(&Mau_match_merge_addrmap_base->mau_snapshot_mpr_next_table_out), 0, {0x3}, sizeof(uint32_t) } },
    { "mau_stateful_log_counter_ctl", { const_cast<uint32_t(*)[0x2][0x2]>(&Mau_match_merge_addrmap_base->mau_stateful_log_counter_ctl), 0, {0x2,0x2}, sizeof(uint32_t) } },
    { "mau_stateful_log_counter_ctl2", { const_cast<uint32_t(*)[0x4]>(&Mau_match_merge_addrmap_base->mau_stateful_log_counter_ctl2), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_stateful_log_counter_ctl3", { const_cast<uint32_t(*)[0x4]>(&Mau_match_merge_addrmap_base->mau_stateful_log_counter_ctl3), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_stateful_log_fifo_level", { const_cast<uint32_t(*)[0x4]>(&Mau_match_merge_addrmap_base->mau_stateful_log_fifo_level), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_stateful_log_watermark_threshold", { const_cast<uint32_t(*)[0x4]>(&Mau_match_merge_addrmap_base->mau_stateful_log_watermark_threshold), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_stateful_log_cur_watermark", { const_cast<uint32_t(*)[0x4]>(&Mau_match_merge_addrmap_base->mau_stateful_log_cur_watermark), 0, {0x4}, sizeof(uint32_t) } },
    { "dft_csr_memctrl_mmc", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->dft_csr_memctrl_mmc), 0, {}, sizeof(uint32_t) } },
    { "meter_alu_thread", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->meter_alu_thread), 0, {0x2}, sizeof(uint32_t) } },
    { "exact_match_phys_result_delay", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->exact_match_phys_result_delay), 0, {0x2}, sizeof(uint32_t) } },
    { "exact_match_phys_result_en", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->exact_match_phys_result_en), 0, {0x2}, sizeof(uint32_t) } },
    { "exact_match_phys_result_thread", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->exact_match_phys_result_thread), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_action_instruction_adr_map_en", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_action_instruction_adr_map_en), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_action_instruction_adr_mode", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_action_instruction_adr_mode), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_hash_group_expand", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_hash_group_expand), 0, {0x2}, sizeof(uint32_t) } },
    { "meter_group_table_vpn_mod_enable", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->meter_group_table_vpn_mod_enable), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_meter_alu_to_logical_map", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_meter_alu_to_logical_map), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_logical_to_meter_alu_map", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_logical_to_meter_alu_map), 0, {0x2}, sizeof(uint32_t) } },
    { "action_adr_vpn_mod_enable", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->action_adr_vpn_mod_enable), 0, {0x2}, sizeof(uint32_t) } },
    { "predication_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->predication_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "mpr_thread_delay", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mpr_thread_delay), 0, {0x2}, sizeof(uint32_t) } },
    { "gateway_payload_exact_shift_ovr", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->gateway_payload_exact_shift_ovr), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_snapshot_table_active", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_snapshot_table_active), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_snapshot_capture_datapath_error", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_snapshot_capture_datapath_error), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_snapshot_global_exec_out", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_snapshot_global_exec_out), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_snapshot_long_branch_out", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_snapshot_long_branch_out), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_snapshot_mpr_global_exec_out", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_snapshot_mpr_global_exec_out), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_snapshot_mpr_long_branch_out", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_snapshot_mpr_long_branch_out), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_table_counter_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_table_counter_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "tcam_match_error_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->tcam_match_error_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "tind_ecc_error_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->tind_ecc_error_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "gfm_parity_error_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->gfm_parity_error_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "emm_ecc_error_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->emm_ecc_error_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "prev_error_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->prev_error_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "err_idata_ovr_fifo_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->err_idata_ovr_fifo_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "err_idata_ovr_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->err_idata_ovr_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "o_error_fifo_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->o_error_fifo_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "s2p_stats_error_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->s2p_stats_error_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "s2p_meter_error_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->s2p_meter_error_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "exact_match_logical_result_delay", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->exact_match_logical_result_delay), 0, {}, sizeof(uint32_t) } },
    { "exact_match_logical_result_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->exact_match_logical_result_en), 0, {}, sizeof(uint32_t) } },
    { "next_table_map_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->next_table_map_en), 0, {}, sizeof(uint32_t) } },
    { "next_table_map_en_gateway", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->next_table_map_en_gateway), 0, {}, sizeof(uint32_t) } },
    { "next_table_tcam_actionbit_map_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->next_table_tcam_actionbit_map_en), 0, {}, sizeof(uint32_t) } },
    { "pred_long_brch_terminate", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->pred_long_brch_terminate), 0, {}, sizeof(uint32_t) } },
    { "pred_is_a_brch", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->pred_is_a_brch), 0, {}, sizeof(uint32_t) } },
    { "pred_ghost_thread", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->pred_ghost_thread), 0, {}, sizeof(uint32_t) } },
    { "pred_stage_id", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->pred_stage_id), 0, {}, sizeof(uint32_t) } },
    { "mau_immediate_data_tcam_actionbit_map_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_immediate_data_tcam_actionbit_map_en), 0, {}, sizeof(uint32_t) } },
    { "mau_action_instruction_adr_tcam_actionbit_map_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_action_instruction_adr_tcam_actionbit_map_en), 0, {}, sizeof(uint32_t) } },
    { "mau_actiondata_adr_tcam_actionbit_map_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_actiondata_adr_tcam_actionbit_map_en), 0, {}, sizeof(uint32_t) } },
    { "pred_map_loca", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->pred_map_loca), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "pred_map_glob", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->pred_map_glob), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "mau_immediate_data_exact_shiftcount", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->mau_immediate_data_exact_shiftcount), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "mau_action_instruction_adr_exact_shiftcount", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->mau_action_instruction_adr_exact_shiftcount), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "mau_actiondata_adr_exact_shiftcount", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->mau_actiondata_adr_exact_shiftcount), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "mau_stats_adr_exact_shiftcount", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->mau_stats_adr_exact_shiftcount), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "mau_meter_adr_exact_shiftcount", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->mau_meter_adr_exact_shiftcount), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "mau_idletime_adr_exact_shiftcount", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->mau_idletime_adr_exact_shiftcount), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "gateway_next_table_lut", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->gateway_next_table_lut), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "match_to_logical_table_ixbar_outputmap", { const_cast<uint32_t(*)[0x4][0x10]>(&Mau_match_merge_addrmap_base->match_to_logical_table_ixbar_outputmap), 0, {0x4,0x10}, sizeof(uint32_t) } },
    { "mau_action_instruction_adr_map_data", { const_cast<uint32_t(*)[0x2][0x10][0x2]>(&Mau_match_merge_addrmap_base->mau_action_instruction_adr_map_data), 0, {0x2,0x10,0x2}, sizeof(uint32_t) } },
    { "mpr_next_table_lut", { const_cast<uint32_t(*)[0x3][0x10]>(&Mau_match_merge_addrmap_base->mpr_next_table_lut), 0, {0x3,0x10}, sizeof(uint32_t) } },
    { "gateway_payload_data", { const_cast<uint32_t(*)[0x8][0x2][0x2][0x2]>(&Mau_match_merge_addrmap_base->gateway_payload_data), 0, {0x8,0x2,0x2,0x2}, sizeof(uint32_t) } },
    { "mau_stats_adr_tcam_actionbit_map_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_stats_adr_tcam_actionbit_map_en), 0, {}, sizeof(uint32_t) } },
    { "mau_meter_adr_tcam_actionbit_map_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_meter_adr_tcam_actionbit_map_en), 0, {}, sizeof(uint32_t) } },
    { "mau_match_central_mapram_read_color_oflo_ctl", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_match_central_mapram_read_color_oflo_ctl), 0, {}, sizeof(uint32_t) } },
    { "mau_idletime_adr_tcam_actionbit_map_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_idletime_adr_tcam_actionbit_map_en), 0, {}, sizeof(uint32_t) } },
    { "mau_selector_hash_sps_enable", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_selector_hash_sps_enable), 0, {}, sizeof(uint32_t) } },
    { "mau_hash_group_config", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_hash_group_config), 0, {}, sizeof(uint32_t) } },
    { "mau_hash_group_shiftcount", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_hash_group_shiftcount), 0, {}, sizeof(uint32_t) } },
    { "meter_group_table_vpn_max", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->meter_group_table_vpn_max), 0, {}, sizeof(uint32_t) } },
    { "mau_meter_precolor_hash_sel", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_meter_precolor_hash_sel), 0, {}, sizeof(uint32_t) } },
    { "mpr_bus_dep", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mpr_bus_dep), 0, {}, sizeof(uint32_t) } },
    { "mpr_glob_exec_thread", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mpr_glob_exec_thread), 0, {}, sizeof(uint32_t) } },
    { "mpr_long_brch_thread", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mpr_long_brch_thread), 0, {}, sizeof(uint32_t) } },
    { "mpr_always_run", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mpr_always_run), 0, {}, sizeof(uint32_t) } },
    { "gateway_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->gateway_en), 0, {}, sizeof(uint32_t) } },
    { "mau_snapshot_logical_table_hit", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_snapshot_logical_table_hit), 0, {}, sizeof(uint32_t) } },
    { "mau_snapshot_gateway_table_inhibit_logical", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_snapshot_gateway_table_inhibit_logical), 0, {}, sizeof(uint32_t) } },
    { "mau_payload_shifter_enable", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_payload_shifter_enable), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_immediate_data_mask", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_immediate_data_mask), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_immediate_data_default", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_immediate_data_default), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_immediate_data_tcam_actionbit_map_data", { const_cast<uint32_t(*)[0x10][0x2]>(&Mau_match_merge_addrmap_base->mau_immediate_data_tcam_actionbit_map_data), 0, {0x10,0x2}, sizeof(uint32_t) } },
    { "mau_action_instruction_adr_mask", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_action_instruction_adr_mask), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_action_instruction_adr_default", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_action_instruction_adr_default), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_action_instruction_adr_per_entry_en_mux_ctl", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_action_instruction_adr_per_entry_en_mux_ctl), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_actiondata_adr_vpn_shiftcount", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_actiondata_adr_vpn_shiftcount), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_actiondata_adr_mask", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_actiondata_adr_mask), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_actiondata_adr_default", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_actiondata_adr_default), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_actiondata_adr_per_entry_en_mux_ctl", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_actiondata_adr_per_entry_en_mux_ctl), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_actiondata_adr_tcam_actionbit_map_data", { const_cast<uint32_t(*)[0x10][0x2]>(&Mau_match_merge_addrmap_base->mau_actiondata_adr_tcam_actionbit_map_data), 0, {0x10,0x2}, sizeof(uint32_t) } },
    { "mau_stats_adr_mask", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_stats_adr_mask), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_stats_adr_default", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_stats_adr_default), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_stats_adr_per_entry_en_mux_ctl", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_stats_adr_per_entry_en_mux_ctl), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_stats_adr_hole_swizzle_mode", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_stats_adr_hole_swizzle_mode), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_stats_adr_tcam_actionbit_map_data", { const_cast<uint32_t(*)[0x10][0x2]>(&Mau_match_merge_addrmap_base->mau_stats_adr_tcam_actionbit_map_data), 0, {0x10,0x2}, sizeof(uint32_t) } },
    { "mau_meter_adr_mask", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_meter_adr_mask), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_meter_adr_default", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_meter_adr_default), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_meter_adr_per_entry_en_mux_ctl", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_meter_adr_per_entry_en_mux_ctl), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_meter_adr_tcam_actionbit_map_data", { const_cast<uint32_t(*)[0x10][0x2]>(&Mau_match_merge_addrmap_base->mau_meter_adr_tcam_actionbit_map_data), 0, {0x10,0x2}, sizeof(uint32_t) } },
    { "mau_idletime_adr_mask", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_idletime_adr_mask), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_idletime_adr_default", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_idletime_adr_default), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_idletime_adr_tcam_actionbit_map_data", { const_cast<uint32_t(*)[0x10][0x2]>(&Mau_match_merge_addrmap_base->mau_idletime_adr_tcam_actionbit_map_data), 0, {0x10,0x2}, sizeof(uint32_t) } },
    { "mau_idletime_adr_per_entry_en_mux_ctl", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_idletime_adr_per_entry_en_mux_ctl), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_selectorlength_shiftcount", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_selectorlength_shiftcount), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_selectorlength_mask", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_selectorlength_mask), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_selectorlength_default", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_selectorlength_default), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "mau_meter_adr_type_position", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_meter_adr_type_position), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "gateway_payload_match_adr", { const_cast<uint32_t(*)[0x8][0x2][0x2]>(&Mau_match_merge_addrmap_base->gateway_payload_match_adr), 0, {0x8,0x2,0x2}, sizeof(uint32_t) } },
    { "tcam_match_adr_to_physical_oxbar_outputmap", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->tcam_match_adr_to_physical_oxbar_outputmap), 0, {0x10}, sizeof(uint32_t) } },
    { "tind_ram_data_size", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->tind_ram_data_size), 0, {0x10}, sizeof(uint32_t) } },
    { "next_table_format_data", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->next_table_format_data), 0, {0x10}, sizeof(uint32_t) } },
    { "pred_miss_exec", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->pred_miss_exec), 0, {0x10}, sizeof(uint32_t) } },
    { "pred_miss_long_brch", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->pred_miss_long_brch), 0, {0x10}, sizeof(uint32_t) } },
    { "pred_long_brch_lt_src", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->pred_long_brch_lt_src), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_immediate_data_tcam_shiftcount", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_immediate_data_tcam_shiftcount), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_immediate_data_miss_value", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_immediate_data_miss_value), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_action_instruction_adr_tcam_shiftcount", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_action_instruction_adr_tcam_shiftcount), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_action_instruction_adr_miss_value", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_action_instruction_adr_miss_value), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_actiondata_adr_tcam_shiftcount", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_actiondata_adr_tcam_shiftcount), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_actiondata_adr_miss_value", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_actiondata_adr_miss_value), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_stats_adr_tcam_shiftcount", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_stats_adr_tcam_shiftcount), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_stats_adr_miss_value", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_stats_adr_miss_value), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_meter_adr_tcam_shiftcount", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_meter_adr_tcam_shiftcount), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_meter_adr_miss_value", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_meter_adr_miss_value), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_idletime_adr_tcam_shiftcount", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_idletime_adr_tcam_shiftcount), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_idletime_adr_miss_value", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_idletime_adr_miss_value), 0, {0x10}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_match_central_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_match_central_addrmap Mau_match_central_addrmap_base=0;
  Mau_match_central_addrmap_map() : RegisterMapper( {
    { "adrdist", { &Mau_match_central_addrmap_base->adrdist, new Mau_address_distribution_addrmap_map, {}, sizeof(Mau_address_distribution_addrmap) } },
    { "merge", { &Mau_match_central_addrmap_base->merge, new Mau_match_merge_addrmap_map, {}, sizeof(Mau_match_merge_addrmap) } }
    } )
  {}
};

struct Mau_memory_core_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_memory_core_addrmap Mau_memory_core_addrmap_base=0;
  Mau_memory_core_addrmap_map() : RegisterMapper( {
    { "array", { &Mau_memory_core_addrmap_base->array, new Mau_unit_ram_array_addrmap_map, {}, sizeof(Mau_unit_ram_array_addrmap) } },
    { "map_alu", { &Mau_memory_core_addrmap_base->map_alu, new Mau_map_and_alu_addrmap_map, {}, sizeof(Mau_map_and_alu_addrmap) } },
    { "match", { &Mau_memory_core_addrmap_base->match, new Mau_match_central_addrmap_map, {}, sizeof(Mau_match_central_addrmap) } }
    } )
  {}
};

struct Mau_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_addrmap Mau_addrmap_base=0;
  Mau_addrmap_map() : RegisterMapper( {
    { "dp", { &Mau_addrmap_base->dp, new Mau_datapath_addrmap_map, {}, sizeof(Mau_datapath_addrmap) } },
    { "cfg_regs", { &Mau_addrmap_base->cfg_regs, new Mau_cfg_regs_addrmap_map, {}, sizeof(Mau_cfg_regs_addrmap) } },
    { "tcams", { &Mau_addrmap_base->tcams, new Mau_tcam_array_addrmap_map, {}, sizeof(Mau_tcam_array_addrmap) } },
    { "rams", { &Mau_addrmap_base->rams, new Mau_memory_core_addrmap_map, {}, sizeof(Mau_memory_core_addrmap) } }
    } )
  {}
};

struct Glb_status_map: public RegisterMapper {
  static constexpr PTR_Glb_status Glb_status_base=0;
  Glb_status_map() : RegisterMapper( {
    { "glb_status_0_2", { const_cast<uint32_t(*)>(&Glb_status_base->glb_status_0_2), 0, {}, sizeof(uint32_t) } },
    { "glb_status_1_2", { const_cast<uint32_t(*)>(&Glb_status_base->glb_status_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Glb_ctrl_map: public RegisterMapper {
  static constexpr PTR_Glb_ctrl Glb_ctrl_base=0;
  Glb_ctrl_map() : RegisterMapper( {
    { "glb_ctrl_0_2", { const_cast<uint32_t(*)>(&Glb_ctrl_base->glb_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "glb_ctrl_1_2", { const_cast<uint32_t(*)>(&Glb_ctrl_base->glb_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Glb_group_intr_stat_stat_map: public RegisterMapper {
  static constexpr PTR_Glb_group_intr_stat_stat Glb_group_intr_stat_stat_base=0;
  Glb_group_intr_stat_stat_map() : RegisterMapper( {
    { "stat_0_2", { const_cast<uint32_t(*)>(&Glb_group_intr_stat_stat_base->stat_0_2), 0, {}, sizeof(uint32_t) } },
    { "stat_1_2", { const_cast<uint32_t(*)>(&Glb_group_intr_stat_stat_base->stat_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Glb_group_intr_stat_en0_map: public RegisterMapper {
  static constexpr PTR_Glb_group_intr_stat_en0 Glb_group_intr_stat_en0_base=0;
  Glb_group_intr_stat_en0_map() : RegisterMapper( {
    { "en0_0_2", { const_cast<uint32_t(*)>(&Glb_group_intr_stat_en0_base->en0_0_2), 0, {}, sizeof(uint32_t) } },
    { "en0_1_2", { const_cast<uint32_t(*)>(&Glb_group_intr_stat_en0_base->en0_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Glb_group_intr_stat_en1_map: public RegisterMapper {
  static constexpr PTR_Glb_group_intr_stat_en1 Glb_group_intr_stat_en1_base=0;
  Glb_group_intr_stat_en1_map() : RegisterMapper( {
    { "en1_0_2", { const_cast<uint32_t(*)>(&Glb_group_intr_stat_en1_base->en1_0_2), 0, {}, sizeof(uint32_t) } },
    { "en1_1_2", { const_cast<uint32_t(*)>(&Glb_group_intr_stat_en1_base->en1_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Glb_group_intr_stat_inj_map: public RegisterMapper {
  static constexpr PTR_Glb_group_intr_stat_inj Glb_group_intr_stat_inj_base=0;
  Glb_group_intr_stat_inj_map() : RegisterMapper( {
    { "inj_0_2", { const_cast<uint32_t(*)>(&Glb_group_intr_stat_inj_base->inj_0_2), 0, {}, sizeof(uint32_t) } },
    { "inj_1_2", { const_cast<uint32_t(*)>(&Glb_group_intr_stat_inj_base->inj_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Glb_group_intr_stat_freeze_enable_map: public RegisterMapper {
  static constexpr PTR_Glb_group_intr_stat_freeze_enable Glb_group_intr_stat_freeze_enable_base=0;
  Glb_group_intr_stat_freeze_enable_map() : RegisterMapper( {
    { "freeze_enable_0_2", { const_cast<uint32_t(*)>(&Glb_group_intr_stat_freeze_enable_base->freeze_enable_0_2), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable_1_2", { const_cast<uint32_t(*)>(&Glb_group_intr_stat_freeze_enable_base->freeze_enable_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Glb_group_intr_stat_map: public RegisterMapper {
  static constexpr PTR_Glb_group_intr_stat Glb_group_intr_stat_base=0;
  Glb_group_intr_stat_map() : RegisterMapper( {
    { "stat", { const_cast<Glb_group_intr_stat_stat(*)>(&Glb_group_intr_stat_base->stat), new Glb_group_intr_stat_stat_map, {}, sizeof(Glb_group_intr_stat_stat) } },
    { "en0", { const_cast<Glb_group_intr_stat_en0(*)>(&Glb_group_intr_stat_base->en0), new Glb_group_intr_stat_en0_map, {}, sizeof(Glb_group_intr_stat_en0) } },
    { "en1", { const_cast<Glb_group_intr_stat_en1(*)>(&Glb_group_intr_stat_base->en1), new Glb_group_intr_stat_en1_map, {}, sizeof(Glb_group_intr_stat_en1) } },
    { "inj", { const_cast<Glb_group_intr_stat_inj(*)>(&Glb_group_intr_stat_base->inj), new Glb_group_intr_stat_inj_map, {}, sizeof(Glb_group_intr_stat_inj) } },
    { "freeze_enable", { const_cast<Glb_group_intr_stat_freeze_enable(*)>(&Glb_group_intr_stat_base->freeze_enable), new Glb_group_intr_stat_freeze_enable_map, {}, sizeof(Glb_group_intr_stat_freeze_enable) } }
    } )
  {}
};

struct Intr_log_group_map: public RegisterMapper {
  static constexpr PTR_Intr_log_group Intr_log_group_base=0;
  Intr_log_group_map() : RegisterMapper( {
    { "pc_fatal_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->pc_fatal_err), 0, {}, sizeof(uint32_t) } },
    { "mac_protocol_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->mac_protocol_err), 0, {}, sizeof(uint32_t) } },
    { "mac_runt_pkt_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->mac_runt_pkt_err), 0, {}, sizeof(uint32_t) } },
    { "mac_oversz_pkt_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->mac_oversz_pkt_err), 0, {}, sizeof(uint32_t) } },
    { "prsr_pkt_ctl_fatal0_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->prsr_pkt_ctl_fatal0_err), 0, {}, sizeof(uint32_t) } },
    { "prsr_pkt_ctl_fatal1_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->prsr_pkt_ctl_fatal1_err), 0, {}, sizeof(uint32_t) } },
    { "prsr_pkt_ctl_fatal2_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->prsr_pkt_ctl_fatal2_err), 0, {}, sizeof(uint32_t) } },
    { "prsr_pkt_ctl_fatal3_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->prsr_pkt_ctl_fatal3_err), 0, {}, sizeof(uint32_t) } },
    { "dprsr_pkt_ctl_fatal0_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->dprsr_pkt_ctl_fatal0_err), 0, {}, sizeof(uint32_t) } },
    { "dprsr_pkt_ctl_fatal1_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->dprsr_pkt_ctl_fatal1_err), 0, {}, sizeof(uint32_t) } },
    { "dprsr_pkt_ctl_fatal2_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->dprsr_pkt_ctl_fatal2_err), 0, {}, sizeof(uint32_t) } },
    { "dprsr_pkt_ctl_fatal3_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->dprsr_pkt_ctl_fatal3_err), 0, {}, sizeof(uint32_t) } },
    { "meta_fifo_fatal0_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->meta_fifo_fatal0_err), 0, {}, sizeof(uint32_t) } },
    { "meta_fifo_fatal1_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->meta_fifo_fatal1_err), 0, {}, sizeof(uint32_t) } },
    { "meta_fifo_fatal2_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->meta_fifo_fatal2_err), 0, {}, sizeof(uint32_t) } },
    { "meta_fifo_fatal3_err", { const_cast<uint32_t(*)>(&Intr_log_group_base->meta_fifo_fatal3_err), 0, {}, sizeof(uint32_t) } },
    { "pbc_mem0_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->pbc_mem0_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "pbc_mem0_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->pbc_mem0_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "pbc_mem1_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->pbc_mem1_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "pbc_mem1_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->pbc_mem1_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "mbc_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->mbc_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "mbc_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->mbc_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "mbc_meta_fifo_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->mbc_meta_fifo_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "mbc_meta_fifo_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->mbc_meta_fifo_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "llc_mem0_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->llc_mem0_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "llc_mem0_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->llc_mem0_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "llc_mem1_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->llc_mem1_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "llc_mem1_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->llc_mem1_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "llc_mem2_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->llc_mem2_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "llc_mem2_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->llc_mem2_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "drop_mem0_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->drop_mem0_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "drop_mem0_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->drop_mem0_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "drop_mem1_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->drop_mem1_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "drop_mem1_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->drop_mem1_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "papc_merge_mem0_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->papc_merge_mem0_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "papc_merge_mem0_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->papc_merge_mem0_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "papc_merge_mem1_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->papc_merge_mem1_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "papc_merge_mem1_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->papc_merge_mem1_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "papc_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->papc_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "papc_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->papc_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "dppc_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->dppc_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "dppc_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->dppc_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "gacc_mem0_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->gacc_mem0_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "gacc_mem0_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->gacc_mem0_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "gacc_mem1_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->gacc_mem1_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "gacc_mem1_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->gacc_mem1_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "gacc_mem2_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->gacc_mem2_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "gacc_mem2_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->gacc_mem2_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "gacc_mem3_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->gacc_mem3_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "gacc_mem3_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->gacc_mem3_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "bcc_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->bcc_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "bcc_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Intr_log_group_base->bcc_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Glb_group_map: public RegisterMapper {
  static constexpr PTR_Glb_group Glb_group_base=0;
  Glb_group_map() : RegisterMapper( {
    { "sw_reset", { const_cast<uint32_t(*)>(&Glb_group_base->sw_reset), 0, {}, sizeof(uint32_t) } },
    { "glb_status", { const_cast<Glb_status(*)>(&Glb_group_base->glb_status), new Glb_status_map, {}, sizeof(Glb_status) } },
    { "glb_prsr_crd_stat", { const_cast<uint32_t(*)>(&Glb_group_base->glb_prsr_crd_stat), 0, {}, sizeof(uint32_t) } },
    { "port_rates", { const_cast<uint32_t(*)>(&Glb_group_base->port_rates), 0, {}, sizeof(uint32_t) } },
    { "port_en", { const_cast<uint32_t(*)>(&Glb_group_base->port_en), 0, {}, sizeof(uint32_t) } },
    { "glb_ctrl", { const_cast<Glb_ctrl(*)>(&Glb_group_base->glb_ctrl), new Glb_ctrl_map, {}, sizeof(Glb_ctrl) } },
    { "acc_ctrl", { const_cast<uint32_t(*)>(&Glb_group_base->acc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "acc_stat", { const_cast<uint32_t(*)>(&Glb_group_base->acc_stat), 0, {}, sizeof(uint32_t) } },
    { "glb_parser_maxbyte", { const_cast<uint32_t(*)>(&Glb_group_base->glb_parser_maxbyte), 0, {}, sizeof(uint32_t) } },
    { "intr_stat", { &Glb_group_base->intr_stat, new Glb_group_intr_stat_map, {}, sizeof(Glb_group_intr_stat) } },
    { "intr_log_group", { &Glb_group_base->intr_log_group, new Intr_log_group_map, {}, sizeof(Intr_log_group) } },
    { "ecc_inj", { const_cast<uint32_t(*)>(&Glb_group_base->ecc_inj), 0, {}, sizeof(uint32_t) } },
    { "ecc_dis", { const_cast<uint32_t(*)>(&Glb_group_base->ecc_dis), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Glb_group_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "glb_ipb_mtu", { const_cast<uint32_t(*)>(&Glb_group_base->glb_ipb_mtu), 0, {}, sizeof(uint32_t) } },
    { "glb_ipb_tim_off", { const_cast<uint32_t(*)>(&Glb_group_base->glb_ipb_tim_off), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_ctrl_map: public RegisterMapper {
  static constexpr PTR_Chnl_ctrl Chnl_ctrl_base=0;
  Chnl_ctrl_map() : RegisterMapper( {
    { "chnl_ctrl_0_2", { const_cast<uint32_t(*)>(&Chnl_ctrl_base->chnl_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_ctrl_1_2", { const_cast<uint32_t(*)>(&Chnl_ctrl_base->chnl_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_meta_map: public RegisterMapper {
  static constexpr PTR_Chnl_meta Chnl_meta_base=0;
  Chnl_meta_map() : RegisterMapper( {
    { "chnl_meta_0_4", { const_cast<uint32_t(*)>(&Chnl_meta_base->chnl_meta_0_4), 0, {}, sizeof(uint32_t) } },
    { "chnl_meta_1_4", { const_cast<uint32_t(*)>(&Chnl_meta_base->chnl_meta_1_4), 0, {}, sizeof(uint32_t) } },
    { "chnl_meta_2_4", { const_cast<uint32_t(*)>(&Chnl_meta_base->chnl_meta_2_4), 0, {}, sizeof(uint32_t) } },
    { "chnl_meta_3_4", { const_cast<uint32_t(*)>(&Chnl_meta_base->chnl_meta_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_fifo_ctrl_map: public RegisterMapper {
  static constexpr PTR_Chnl_fifo_ctrl Chnl_fifo_ctrl_base=0;
  Chnl_fifo_ctrl_map() : RegisterMapper( {
    { "chnl_fifo_ctrl_0_3", { const_cast<uint32_t(*)>(&Chnl_fifo_ctrl_base->chnl_fifo_ctrl_0_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_ctrl_1_3", { const_cast<uint32_t(*)>(&Chnl_fifo_ctrl_base->chnl_fifo_ctrl_1_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_ctrl_2_3", { const_cast<uint32_t(*)>(&Chnl_fifo_ctrl_base->chnl_fifo_ctrl_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_acc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Chnl_acc_ctrl Chnl_acc_ctrl_base=0;
  Chnl_acc_ctrl_map() : RegisterMapper( {
    { "chnl_acc_ctrl_0_3", { const_cast<uint32_t(*)>(&Chnl_acc_ctrl_base->chnl_acc_ctrl_0_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_acc_ctrl_1_3", { const_cast<uint32_t(*)>(&Chnl_acc_ctrl_base->chnl_acc_ctrl_1_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_acc_ctrl_2_3", { const_cast<uint32_t(*)>(&Chnl_acc_ctrl_base->chnl_acc_ctrl_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_deparser_drop_pkt_map: public RegisterMapper {
  static constexpr PTR_Chnl_deparser_drop_pkt Chnl_deparser_drop_pkt_base=0;
  Chnl_deparser_drop_pkt_map() : RegisterMapper( {
    { "chnl_deparser_drop_pkt_0_2", { const_cast<uint32_t(*)>(&Chnl_deparser_drop_pkt_base->chnl_deparser_drop_pkt_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_drop_pkt_1_2", { const_cast<uint32_t(*)>(&Chnl_deparser_drop_pkt_base->chnl_deparser_drop_pkt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_wsch_discard_pkt_map: public RegisterMapper {
  static constexpr PTR_Chnl_wsch_discard_pkt Chnl_wsch_discard_pkt_base=0;
  Chnl_wsch_discard_pkt_map() : RegisterMapper( {
    { "chnl_wsch_discard_pkt_0_2", { const_cast<uint32_t(*)>(&Chnl_wsch_discard_pkt_base->chnl_wsch_discard_pkt_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_wsch_discard_pkt_1_2", { const_cast<uint32_t(*)>(&Chnl_wsch_discard_pkt_base->chnl_wsch_discard_pkt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_wsch_trunc_pkt_map: public RegisterMapper {
  static constexpr PTR_Chnl_wsch_trunc_pkt Chnl_wsch_trunc_pkt_base=0;
  Chnl_wsch_trunc_pkt_map() : RegisterMapper( {
    { "chnl_wsch_trunc_pkt_0_2", { const_cast<uint32_t(*)>(&Chnl_wsch_trunc_pkt_base->chnl_wsch_trunc_pkt_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_wsch_trunc_pkt_1_2", { const_cast<uint32_t(*)>(&Chnl_wsch_trunc_pkt_base->chnl_wsch_trunc_pkt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_drop_trunc_pkt_map: public RegisterMapper {
  static constexpr PTR_Chnl_drop_trunc_pkt Chnl_drop_trunc_pkt_base=0;
  Chnl_drop_trunc_pkt_map() : RegisterMapper( {
    { "chnl_drop_trunc_pkt_0_2", { const_cast<uint32_t(*)>(&Chnl_drop_trunc_pkt_base->chnl_drop_trunc_pkt_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_drop_trunc_pkt_1_2", { const_cast<uint32_t(*)>(&Chnl_drop_trunc_pkt_base->chnl_drop_trunc_pkt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_resubmit_discard_pkt_map: public RegisterMapper {
  static constexpr PTR_Chnl_resubmit_discard_pkt Chnl_resubmit_discard_pkt_base=0;
  Chnl_resubmit_discard_pkt_map() : RegisterMapper( {
    { "chnl_resubmit_discard_pkt_0_2", { const_cast<uint32_t(*)>(&Chnl_resubmit_discard_pkt_base->chnl_resubmit_discard_pkt_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_resubmit_discard_pkt_1_2", { const_cast<uint32_t(*)>(&Chnl_resubmit_discard_pkt_base->chnl_resubmit_discard_pkt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_parser_discard_pkt_map: public RegisterMapper {
  static constexpr PTR_Chnl_parser_discard_pkt Chnl_parser_discard_pkt_base=0;
  Chnl_parser_discard_pkt_map() : RegisterMapper( {
    { "chnl_parser_discard_pkt_0_2", { const_cast<uint32_t(*)>(&Chnl_parser_discard_pkt_base->chnl_parser_discard_pkt_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_parser_discard_pkt_1_2", { const_cast<uint32_t(*)>(&Chnl_parser_discard_pkt_base->chnl_parser_discard_pkt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_parser_send_pkt_map: public RegisterMapper {
  static constexpr PTR_Chnl_parser_send_pkt Chnl_parser_send_pkt_base=0;
  Chnl_parser_send_pkt_map() : RegisterMapper( {
    { "chnl_parser_send_pkt_0_3", { const_cast<uint32_t(*)>(&Chnl_parser_send_pkt_base->chnl_parser_send_pkt_0_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_parser_send_pkt_1_3", { const_cast<uint32_t(*)>(&Chnl_parser_send_pkt_base->chnl_parser_send_pkt_1_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_parser_send_pkt_2_3", { const_cast<uint32_t(*)>(&Chnl_parser_send_pkt_base->chnl_parser_send_pkt_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_deparser_send_pkt_map: public RegisterMapper {
  static constexpr PTR_Chnl_deparser_send_pkt Chnl_deparser_send_pkt_base=0;
  Chnl_deparser_send_pkt_map() : RegisterMapper( {
    { "chnl_deparser_send_pkt_0_3", { const_cast<uint32_t(*)>(&Chnl_deparser_send_pkt_base->chnl_deparser_send_pkt_0_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_send_pkt_1_3", { const_cast<uint32_t(*)>(&Chnl_deparser_send_pkt_base->chnl_deparser_send_pkt_1_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_send_pkt_2_3", { const_cast<uint32_t(*)>(&Chnl_deparser_send_pkt_base->chnl_deparser_send_pkt_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_macs_received_pkt_map: public RegisterMapper {
  static constexpr PTR_Chnl_macs_received_pkt Chnl_macs_received_pkt_base=0;
  Chnl_macs_received_pkt_map() : RegisterMapper( {
    { "chnl_macs_received_pkt_0_3", { const_cast<uint32_t(*)>(&Chnl_macs_received_pkt_base->chnl_macs_received_pkt_0_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_macs_received_pkt_1_3", { const_cast<uint32_t(*)>(&Chnl_macs_received_pkt_base->chnl_macs_received_pkt_1_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_macs_received_pkt_2_3", { const_cast<uint32_t(*)>(&Chnl_macs_received_pkt_base->chnl_macs_received_pkt_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_resubmit_received_pkt_map: public RegisterMapper {
  static constexpr PTR_Chnl_resubmit_received_pkt Chnl_resubmit_received_pkt_base=0;
  Chnl_resubmit_received_pkt_map() : RegisterMapper( {
    { "chnl_resubmit_received_pkt_0_2", { const_cast<uint32_t(*)>(&Chnl_resubmit_received_pkt_base->chnl_resubmit_received_pkt_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_resubmit_received_pkt_1_2", { const_cast<uint32_t(*)>(&Chnl_resubmit_received_pkt_base->chnl_resubmit_received_pkt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ipb_rspec_chan0_group_map: public RegisterMapper {
  static constexpr PTR_Ipb_rspec_chan0_group Ipb_rspec_chan0_group_base=0;
  Ipb_rspec_chan0_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Chnl_ctrl(*)>(&Ipb_rspec_chan0_group_base->chnl_ctrl), new Chnl_ctrl_map, {}, sizeof(Chnl_ctrl) } },
    { "chnl_meta", { const_cast<Chnl_meta(*)>(&Ipb_rspec_chan0_group_base->chnl_meta), new Chnl_meta_map, {}, sizeof(Chnl_meta) } },
    { "meta_fifo_ctrl", { const_cast<uint32_t(*)>(&Ipb_rspec_chan0_group_base->meta_fifo_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_ctrl", { const_cast<Chnl_fifo_ctrl(*)>(&Ipb_rspec_chan0_group_base->chnl_fifo_ctrl), new Chnl_fifo_ctrl_map, {}, sizeof(Chnl_fifo_ctrl) } },
    { "chnl_acc_ctrl", { const_cast<Chnl_acc_ctrl(*)>(&Ipb_rspec_chan0_group_base->chnl_acc_ctrl), new Chnl_acc_ctrl_map, {}, sizeof(Chnl_acc_ctrl) } },
    { "chnl_acc_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan0_group_base->chnl_acc_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan0_group_base->chnl_pktnum0_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan0_group_base->chnl_pktnum1_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan0_group_base->chnl_metanum_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan0_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Ipb_rspec_chan0_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Ipb_rspec_chan0_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum", { const_cast<uint32_t(*)>(&Ipb_rspec_chan0_group_base->chnl_metanum), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_drop_pkt", { const_cast<Chnl_deparser_drop_pkt(*)>(&Ipb_rspec_chan0_group_base->chnl_deparser_drop_pkt), new Chnl_deparser_drop_pkt_map, {}, sizeof(Chnl_deparser_drop_pkt) } },
    { "chnl_wsch_discard_pkt", { const_cast<Chnl_wsch_discard_pkt(*)>(&Ipb_rspec_chan0_group_base->chnl_wsch_discard_pkt), new Chnl_wsch_discard_pkt_map, {}, sizeof(Chnl_wsch_discard_pkt) } },
    { "chnl_wsch_trunc_pkt", { const_cast<Chnl_wsch_trunc_pkt(*)>(&Ipb_rspec_chan0_group_base->chnl_wsch_trunc_pkt), new Chnl_wsch_trunc_pkt_map, {}, sizeof(Chnl_wsch_trunc_pkt) } },
    { "chnl_drop_trunc_pkt", { const_cast<Chnl_drop_trunc_pkt(*)>(&Ipb_rspec_chan0_group_base->chnl_drop_trunc_pkt), new Chnl_drop_trunc_pkt_map, {}, sizeof(Chnl_drop_trunc_pkt) } },
    { "chnl_resubmit_discard_pkt", { const_cast<Chnl_resubmit_discard_pkt(*)>(&Ipb_rspec_chan0_group_base->chnl_resubmit_discard_pkt), new Chnl_resubmit_discard_pkt_map, {}, sizeof(Chnl_resubmit_discard_pkt) } },
    { "chnl_parser_discard_pkt", { const_cast<Chnl_parser_discard_pkt(*)>(&Ipb_rspec_chan0_group_base->chnl_parser_discard_pkt), new Chnl_parser_discard_pkt_map, {}, sizeof(Chnl_parser_discard_pkt) } },
    { "chnl_parser_send_pkt", { const_cast<Chnl_parser_send_pkt(*)>(&Ipb_rspec_chan0_group_base->chnl_parser_send_pkt), new Chnl_parser_send_pkt_map, {}, sizeof(Chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Chnl_deparser_send_pkt(*)>(&Ipb_rspec_chan0_group_base->chnl_deparser_send_pkt), new Chnl_deparser_send_pkt_map, {}, sizeof(Chnl_deparser_send_pkt) } },
    { "chnl_macs_received_pkt", { const_cast<Chnl_macs_received_pkt(*)>(&Ipb_rspec_chan0_group_base->chnl_macs_received_pkt), new Chnl_macs_received_pkt_map, {}, sizeof(Chnl_macs_received_pkt) } },
    { "chnl_resubmit_received_pkt", { const_cast<Chnl_resubmit_received_pkt(*)>(&Ipb_rspec_chan0_group_base->chnl_resubmit_received_pkt), new Chnl_resubmit_received_pkt_map, {}, sizeof(Chnl_resubmit_received_pkt) } }
    } )
  {}
};

struct Ipb_rspec_chan1_group_map: public RegisterMapper {
  static constexpr PTR_Ipb_rspec_chan1_group Ipb_rspec_chan1_group_base=0;
  Ipb_rspec_chan1_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Chnl_ctrl(*)>(&Ipb_rspec_chan1_group_base->chnl_ctrl), new Chnl_ctrl_map, {}, sizeof(Chnl_ctrl) } },
    { "chnl_meta", { const_cast<Chnl_meta(*)>(&Ipb_rspec_chan1_group_base->chnl_meta), new Chnl_meta_map, {}, sizeof(Chnl_meta) } },
    { "meta_fifo_ctrl", { const_cast<uint32_t(*)>(&Ipb_rspec_chan1_group_base->meta_fifo_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_ctrl", { const_cast<Chnl_fifo_ctrl(*)>(&Ipb_rspec_chan1_group_base->chnl_fifo_ctrl), new Chnl_fifo_ctrl_map, {}, sizeof(Chnl_fifo_ctrl) } },
    { "chnl_acc_ctrl", { const_cast<Chnl_acc_ctrl(*)>(&Ipb_rspec_chan1_group_base->chnl_acc_ctrl), new Chnl_acc_ctrl_map, {}, sizeof(Chnl_acc_ctrl) } },
    { "chnl_acc_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan1_group_base->chnl_acc_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan1_group_base->chnl_pktnum0_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan1_group_base->chnl_pktnum1_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan1_group_base->chnl_metanum_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan1_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Ipb_rspec_chan1_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Ipb_rspec_chan1_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum", { const_cast<uint32_t(*)>(&Ipb_rspec_chan1_group_base->chnl_metanum), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_drop_pkt", { const_cast<Chnl_deparser_drop_pkt(*)>(&Ipb_rspec_chan1_group_base->chnl_deparser_drop_pkt), new Chnl_deparser_drop_pkt_map, {}, sizeof(Chnl_deparser_drop_pkt) } },
    { "chnl_wsch_discard_pkt", { const_cast<Chnl_wsch_discard_pkt(*)>(&Ipb_rspec_chan1_group_base->chnl_wsch_discard_pkt), new Chnl_wsch_discard_pkt_map, {}, sizeof(Chnl_wsch_discard_pkt) } },
    { "chnl_wsch_trunc_pkt", { const_cast<Chnl_wsch_trunc_pkt(*)>(&Ipb_rspec_chan1_group_base->chnl_wsch_trunc_pkt), new Chnl_wsch_trunc_pkt_map, {}, sizeof(Chnl_wsch_trunc_pkt) } },
    { "chnl_drop_trunc_pkt", { const_cast<Chnl_drop_trunc_pkt(*)>(&Ipb_rspec_chan1_group_base->chnl_drop_trunc_pkt), new Chnl_drop_trunc_pkt_map, {}, sizeof(Chnl_drop_trunc_pkt) } },
    { "chnl_resubmit_discard_pkt", { const_cast<Chnl_resubmit_discard_pkt(*)>(&Ipb_rspec_chan1_group_base->chnl_resubmit_discard_pkt), new Chnl_resubmit_discard_pkt_map, {}, sizeof(Chnl_resubmit_discard_pkt) } },
    { "chnl_parser_discard_pkt", { const_cast<Chnl_parser_discard_pkt(*)>(&Ipb_rspec_chan1_group_base->chnl_parser_discard_pkt), new Chnl_parser_discard_pkt_map, {}, sizeof(Chnl_parser_discard_pkt) } },
    { "chnl_parser_send_pkt", { const_cast<Chnl_parser_send_pkt(*)>(&Ipb_rspec_chan1_group_base->chnl_parser_send_pkt), new Chnl_parser_send_pkt_map, {}, sizeof(Chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Chnl_deparser_send_pkt(*)>(&Ipb_rspec_chan1_group_base->chnl_deparser_send_pkt), new Chnl_deparser_send_pkt_map, {}, sizeof(Chnl_deparser_send_pkt) } },
    { "chnl_macs_received_pkt", { const_cast<Chnl_macs_received_pkt(*)>(&Ipb_rspec_chan1_group_base->chnl_macs_received_pkt), new Chnl_macs_received_pkt_map, {}, sizeof(Chnl_macs_received_pkt) } },
    { "chnl_resubmit_received_pkt", { const_cast<Chnl_resubmit_received_pkt(*)>(&Ipb_rspec_chan1_group_base->chnl_resubmit_received_pkt), new Chnl_resubmit_received_pkt_map, {}, sizeof(Chnl_resubmit_received_pkt) } }
    } )
  {}
};

struct Ipb_rspec_chan2_group_map: public RegisterMapper {
  static constexpr PTR_Ipb_rspec_chan2_group Ipb_rspec_chan2_group_base=0;
  Ipb_rspec_chan2_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Chnl_ctrl(*)>(&Ipb_rspec_chan2_group_base->chnl_ctrl), new Chnl_ctrl_map, {}, sizeof(Chnl_ctrl) } },
    { "chnl_meta", { const_cast<Chnl_meta(*)>(&Ipb_rspec_chan2_group_base->chnl_meta), new Chnl_meta_map, {}, sizeof(Chnl_meta) } },
    { "meta_fifo_ctrl", { const_cast<uint32_t(*)>(&Ipb_rspec_chan2_group_base->meta_fifo_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_ctrl", { const_cast<Chnl_fifo_ctrl(*)>(&Ipb_rspec_chan2_group_base->chnl_fifo_ctrl), new Chnl_fifo_ctrl_map, {}, sizeof(Chnl_fifo_ctrl) } },
    { "chnl_acc_ctrl", { const_cast<Chnl_acc_ctrl(*)>(&Ipb_rspec_chan2_group_base->chnl_acc_ctrl), new Chnl_acc_ctrl_map, {}, sizeof(Chnl_acc_ctrl) } },
    { "chnl_acc_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan2_group_base->chnl_acc_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan2_group_base->chnl_pktnum0_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan2_group_base->chnl_pktnum1_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan2_group_base->chnl_metanum_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan2_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Ipb_rspec_chan2_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Ipb_rspec_chan2_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum", { const_cast<uint32_t(*)>(&Ipb_rspec_chan2_group_base->chnl_metanum), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_drop_pkt", { const_cast<Chnl_deparser_drop_pkt(*)>(&Ipb_rspec_chan2_group_base->chnl_deparser_drop_pkt), new Chnl_deparser_drop_pkt_map, {}, sizeof(Chnl_deparser_drop_pkt) } },
    { "chnl_wsch_discard_pkt", { const_cast<Chnl_wsch_discard_pkt(*)>(&Ipb_rspec_chan2_group_base->chnl_wsch_discard_pkt), new Chnl_wsch_discard_pkt_map, {}, sizeof(Chnl_wsch_discard_pkt) } },
    { "chnl_wsch_trunc_pkt", { const_cast<Chnl_wsch_trunc_pkt(*)>(&Ipb_rspec_chan2_group_base->chnl_wsch_trunc_pkt), new Chnl_wsch_trunc_pkt_map, {}, sizeof(Chnl_wsch_trunc_pkt) } },
    { "chnl_drop_trunc_pkt", { const_cast<Chnl_drop_trunc_pkt(*)>(&Ipb_rspec_chan2_group_base->chnl_drop_trunc_pkt), new Chnl_drop_trunc_pkt_map, {}, sizeof(Chnl_drop_trunc_pkt) } },
    { "chnl_resubmit_discard_pkt", { const_cast<Chnl_resubmit_discard_pkt(*)>(&Ipb_rspec_chan2_group_base->chnl_resubmit_discard_pkt), new Chnl_resubmit_discard_pkt_map, {}, sizeof(Chnl_resubmit_discard_pkt) } },
    { "chnl_parser_discard_pkt", { const_cast<Chnl_parser_discard_pkt(*)>(&Ipb_rspec_chan2_group_base->chnl_parser_discard_pkt), new Chnl_parser_discard_pkt_map, {}, sizeof(Chnl_parser_discard_pkt) } },
    { "chnl_parser_send_pkt", { const_cast<Chnl_parser_send_pkt(*)>(&Ipb_rspec_chan2_group_base->chnl_parser_send_pkt), new Chnl_parser_send_pkt_map, {}, sizeof(Chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Chnl_deparser_send_pkt(*)>(&Ipb_rspec_chan2_group_base->chnl_deparser_send_pkt), new Chnl_deparser_send_pkt_map, {}, sizeof(Chnl_deparser_send_pkt) } },
    { "chnl_macs_received_pkt", { const_cast<Chnl_macs_received_pkt(*)>(&Ipb_rspec_chan2_group_base->chnl_macs_received_pkt), new Chnl_macs_received_pkt_map, {}, sizeof(Chnl_macs_received_pkt) } },
    { "chnl_resubmit_received_pkt", { const_cast<Chnl_resubmit_received_pkt(*)>(&Ipb_rspec_chan2_group_base->chnl_resubmit_received_pkt), new Chnl_resubmit_received_pkt_map, {}, sizeof(Chnl_resubmit_received_pkt) } }
    } )
  {}
};

struct Ipb_rspec_chan3_group_map: public RegisterMapper {
  static constexpr PTR_Ipb_rspec_chan3_group Ipb_rspec_chan3_group_base=0;
  Ipb_rspec_chan3_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Chnl_ctrl(*)>(&Ipb_rspec_chan3_group_base->chnl_ctrl), new Chnl_ctrl_map, {}, sizeof(Chnl_ctrl) } },
    { "chnl_meta", { const_cast<Chnl_meta(*)>(&Ipb_rspec_chan3_group_base->chnl_meta), new Chnl_meta_map, {}, sizeof(Chnl_meta) } },
    { "meta_fifo_ctrl", { const_cast<uint32_t(*)>(&Ipb_rspec_chan3_group_base->meta_fifo_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_ctrl", { const_cast<Chnl_fifo_ctrl(*)>(&Ipb_rspec_chan3_group_base->chnl_fifo_ctrl), new Chnl_fifo_ctrl_map, {}, sizeof(Chnl_fifo_ctrl) } },
    { "chnl_acc_ctrl", { const_cast<Chnl_acc_ctrl(*)>(&Ipb_rspec_chan3_group_base->chnl_acc_ctrl), new Chnl_acc_ctrl_map, {}, sizeof(Chnl_acc_ctrl) } },
    { "chnl_acc_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan3_group_base->chnl_acc_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan3_group_base->chnl_pktnum0_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan3_group_base->chnl_pktnum1_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan3_group_base->chnl_metanum_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan3_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Ipb_rspec_chan3_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Ipb_rspec_chan3_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum", { const_cast<uint32_t(*)>(&Ipb_rspec_chan3_group_base->chnl_metanum), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_drop_pkt", { const_cast<Chnl_deparser_drop_pkt(*)>(&Ipb_rspec_chan3_group_base->chnl_deparser_drop_pkt), new Chnl_deparser_drop_pkt_map, {}, sizeof(Chnl_deparser_drop_pkt) } },
    { "chnl_wsch_discard_pkt", { const_cast<Chnl_wsch_discard_pkt(*)>(&Ipb_rspec_chan3_group_base->chnl_wsch_discard_pkt), new Chnl_wsch_discard_pkt_map, {}, sizeof(Chnl_wsch_discard_pkt) } },
    { "chnl_wsch_trunc_pkt", { const_cast<Chnl_wsch_trunc_pkt(*)>(&Ipb_rspec_chan3_group_base->chnl_wsch_trunc_pkt), new Chnl_wsch_trunc_pkt_map, {}, sizeof(Chnl_wsch_trunc_pkt) } },
    { "chnl_drop_trunc_pkt", { const_cast<Chnl_drop_trunc_pkt(*)>(&Ipb_rspec_chan3_group_base->chnl_drop_trunc_pkt), new Chnl_drop_trunc_pkt_map, {}, sizeof(Chnl_drop_trunc_pkt) } },
    { "chnl_resubmit_discard_pkt", { const_cast<Chnl_resubmit_discard_pkt(*)>(&Ipb_rspec_chan3_group_base->chnl_resubmit_discard_pkt), new Chnl_resubmit_discard_pkt_map, {}, sizeof(Chnl_resubmit_discard_pkt) } },
    { "chnl_parser_discard_pkt", { const_cast<Chnl_parser_discard_pkt(*)>(&Ipb_rspec_chan3_group_base->chnl_parser_discard_pkt), new Chnl_parser_discard_pkt_map, {}, sizeof(Chnl_parser_discard_pkt) } },
    { "chnl_parser_send_pkt", { const_cast<Chnl_parser_send_pkt(*)>(&Ipb_rspec_chan3_group_base->chnl_parser_send_pkt), new Chnl_parser_send_pkt_map, {}, sizeof(Chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Chnl_deparser_send_pkt(*)>(&Ipb_rspec_chan3_group_base->chnl_deparser_send_pkt), new Chnl_deparser_send_pkt_map, {}, sizeof(Chnl_deparser_send_pkt) } },
    { "chnl_macs_received_pkt", { const_cast<Chnl_macs_received_pkt(*)>(&Ipb_rspec_chan3_group_base->chnl_macs_received_pkt), new Chnl_macs_received_pkt_map, {}, sizeof(Chnl_macs_received_pkt) } },
    { "chnl_resubmit_received_pkt", { const_cast<Chnl_resubmit_received_pkt(*)>(&Ipb_rspec_chan3_group_base->chnl_resubmit_received_pkt), new Chnl_resubmit_received_pkt_map, {}, sizeof(Chnl_resubmit_received_pkt) } }
    } )
  {}
};

struct Ipb_rspec_chan4_group_map: public RegisterMapper {
  static constexpr PTR_Ipb_rspec_chan4_group Ipb_rspec_chan4_group_base=0;
  Ipb_rspec_chan4_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Chnl_ctrl(*)>(&Ipb_rspec_chan4_group_base->chnl_ctrl), new Chnl_ctrl_map, {}, sizeof(Chnl_ctrl) } },
    { "chnl_meta", { const_cast<Chnl_meta(*)>(&Ipb_rspec_chan4_group_base->chnl_meta), new Chnl_meta_map, {}, sizeof(Chnl_meta) } },
    { "meta_fifo_ctrl", { const_cast<uint32_t(*)>(&Ipb_rspec_chan4_group_base->meta_fifo_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_ctrl", { const_cast<Chnl_fifo_ctrl(*)>(&Ipb_rspec_chan4_group_base->chnl_fifo_ctrl), new Chnl_fifo_ctrl_map, {}, sizeof(Chnl_fifo_ctrl) } },
    { "chnl_acc_ctrl", { const_cast<Chnl_acc_ctrl(*)>(&Ipb_rspec_chan4_group_base->chnl_acc_ctrl), new Chnl_acc_ctrl_map, {}, sizeof(Chnl_acc_ctrl) } },
    { "chnl_acc_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan4_group_base->chnl_acc_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan4_group_base->chnl_pktnum0_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan4_group_base->chnl_pktnum1_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan4_group_base->chnl_metanum_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan4_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Ipb_rspec_chan4_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Ipb_rspec_chan4_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum", { const_cast<uint32_t(*)>(&Ipb_rspec_chan4_group_base->chnl_metanum), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_drop_pkt", { const_cast<Chnl_deparser_drop_pkt(*)>(&Ipb_rspec_chan4_group_base->chnl_deparser_drop_pkt), new Chnl_deparser_drop_pkt_map, {}, sizeof(Chnl_deparser_drop_pkt) } },
    { "chnl_wsch_discard_pkt", { const_cast<Chnl_wsch_discard_pkt(*)>(&Ipb_rspec_chan4_group_base->chnl_wsch_discard_pkt), new Chnl_wsch_discard_pkt_map, {}, sizeof(Chnl_wsch_discard_pkt) } },
    { "chnl_wsch_trunc_pkt", { const_cast<Chnl_wsch_trunc_pkt(*)>(&Ipb_rspec_chan4_group_base->chnl_wsch_trunc_pkt), new Chnl_wsch_trunc_pkt_map, {}, sizeof(Chnl_wsch_trunc_pkt) } },
    { "chnl_drop_trunc_pkt", { const_cast<Chnl_drop_trunc_pkt(*)>(&Ipb_rspec_chan4_group_base->chnl_drop_trunc_pkt), new Chnl_drop_trunc_pkt_map, {}, sizeof(Chnl_drop_trunc_pkt) } },
    { "chnl_resubmit_discard_pkt", { const_cast<Chnl_resubmit_discard_pkt(*)>(&Ipb_rspec_chan4_group_base->chnl_resubmit_discard_pkt), new Chnl_resubmit_discard_pkt_map, {}, sizeof(Chnl_resubmit_discard_pkt) } },
    { "chnl_parser_discard_pkt", { const_cast<Chnl_parser_discard_pkt(*)>(&Ipb_rspec_chan4_group_base->chnl_parser_discard_pkt), new Chnl_parser_discard_pkt_map, {}, sizeof(Chnl_parser_discard_pkt) } },
    { "chnl_parser_send_pkt", { const_cast<Chnl_parser_send_pkt(*)>(&Ipb_rspec_chan4_group_base->chnl_parser_send_pkt), new Chnl_parser_send_pkt_map, {}, sizeof(Chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Chnl_deparser_send_pkt(*)>(&Ipb_rspec_chan4_group_base->chnl_deparser_send_pkt), new Chnl_deparser_send_pkt_map, {}, sizeof(Chnl_deparser_send_pkt) } },
    { "chnl_macs_received_pkt", { const_cast<Chnl_macs_received_pkt(*)>(&Ipb_rspec_chan4_group_base->chnl_macs_received_pkt), new Chnl_macs_received_pkt_map, {}, sizeof(Chnl_macs_received_pkt) } },
    { "chnl_resubmit_received_pkt", { const_cast<Chnl_resubmit_received_pkt(*)>(&Ipb_rspec_chan4_group_base->chnl_resubmit_received_pkt), new Chnl_resubmit_received_pkt_map, {}, sizeof(Chnl_resubmit_received_pkt) } }
    } )
  {}
};

struct Ipb_rspec_chan5_group_map: public RegisterMapper {
  static constexpr PTR_Ipb_rspec_chan5_group Ipb_rspec_chan5_group_base=0;
  Ipb_rspec_chan5_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Chnl_ctrl(*)>(&Ipb_rspec_chan5_group_base->chnl_ctrl), new Chnl_ctrl_map, {}, sizeof(Chnl_ctrl) } },
    { "chnl_meta", { const_cast<Chnl_meta(*)>(&Ipb_rspec_chan5_group_base->chnl_meta), new Chnl_meta_map, {}, sizeof(Chnl_meta) } },
    { "meta_fifo_ctrl", { const_cast<uint32_t(*)>(&Ipb_rspec_chan5_group_base->meta_fifo_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_ctrl", { const_cast<Chnl_fifo_ctrl(*)>(&Ipb_rspec_chan5_group_base->chnl_fifo_ctrl), new Chnl_fifo_ctrl_map, {}, sizeof(Chnl_fifo_ctrl) } },
    { "chnl_acc_ctrl", { const_cast<Chnl_acc_ctrl(*)>(&Ipb_rspec_chan5_group_base->chnl_acc_ctrl), new Chnl_acc_ctrl_map, {}, sizeof(Chnl_acc_ctrl) } },
    { "chnl_acc_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan5_group_base->chnl_acc_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan5_group_base->chnl_pktnum0_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan5_group_base->chnl_pktnum1_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan5_group_base->chnl_metanum_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan5_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Ipb_rspec_chan5_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Ipb_rspec_chan5_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum", { const_cast<uint32_t(*)>(&Ipb_rspec_chan5_group_base->chnl_metanum), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_drop_pkt", { const_cast<Chnl_deparser_drop_pkt(*)>(&Ipb_rspec_chan5_group_base->chnl_deparser_drop_pkt), new Chnl_deparser_drop_pkt_map, {}, sizeof(Chnl_deparser_drop_pkt) } },
    { "chnl_wsch_discard_pkt", { const_cast<Chnl_wsch_discard_pkt(*)>(&Ipb_rspec_chan5_group_base->chnl_wsch_discard_pkt), new Chnl_wsch_discard_pkt_map, {}, sizeof(Chnl_wsch_discard_pkt) } },
    { "chnl_wsch_trunc_pkt", { const_cast<Chnl_wsch_trunc_pkt(*)>(&Ipb_rspec_chan5_group_base->chnl_wsch_trunc_pkt), new Chnl_wsch_trunc_pkt_map, {}, sizeof(Chnl_wsch_trunc_pkt) } },
    { "chnl_drop_trunc_pkt", { const_cast<Chnl_drop_trunc_pkt(*)>(&Ipb_rspec_chan5_group_base->chnl_drop_trunc_pkt), new Chnl_drop_trunc_pkt_map, {}, sizeof(Chnl_drop_trunc_pkt) } },
    { "chnl_resubmit_discard_pkt", { const_cast<Chnl_resubmit_discard_pkt(*)>(&Ipb_rspec_chan5_group_base->chnl_resubmit_discard_pkt), new Chnl_resubmit_discard_pkt_map, {}, sizeof(Chnl_resubmit_discard_pkt) } },
    { "chnl_parser_discard_pkt", { const_cast<Chnl_parser_discard_pkt(*)>(&Ipb_rspec_chan5_group_base->chnl_parser_discard_pkt), new Chnl_parser_discard_pkt_map, {}, sizeof(Chnl_parser_discard_pkt) } },
    { "chnl_parser_send_pkt", { const_cast<Chnl_parser_send_pkt(*)>(&Ipb_rspec_chan5_group_base->chnl_parser_send_pkt), new Chnl_parser_send_pkt_map, {}, sizeof(Chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Chnl_deparser_send_pkt(*)>(&Ipb_rspec_chan5_group_base->chnl_deparser_send_pkt), new Chnl_deparser_send_pkt_map, {}, sizeof(Chnl_deparser_send_pkt) } },
    { "chnl_macs_received_pkt", { const_cast<Chnl_macs_received_pkt(*)>(&Ipb_rspec_chan5_group_base->chnl_macs_received_pkt), new Chnl_macs_received_pkt_map, {}, sizeof(Chnl_macs_received_pkt) } },
    { "chnl_resubmit_received_pkt", { const_cast<Chnl_resubmit_received_pkt(*)>(&Ipb_rspec_chan5_group_base->chnl_resubmit_received_pkt), new Chnl_resubmit_received_pkt_map, {}, sizeof(Chnl_resubmit_received_pkt) } }
    } )
  {}
};

struct Ipb_rspec_chan6_group_map: public RegisterMapper {
  static constexpr PTR_Ipb_rspec_chan6_group Ipb_rspec_chan6_group_base=0;
  Ipb_rspec_chan6_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Chnl_ctrl(*)>(&Ipb_rspec_chan6_group_base->chnl_ctrl), new Chnl_ctrl_map, {}, sizeof(Chnl_ctrl) } },
    { "chnl_meta", { const_cast<Chnl_meta(*)>(&Ipb_rspec_chan6_group_base->chnl_meta), new Chnl_meta_map, {}, sizeof(Chnl_meta) } },
    { "meta_fifo_ctrl", { const_cast<uint32_t(*)>(&Ipb_rspec_chan6_group_base->meta_fifo_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_ctrl", { const_cast<Chnl_fifo_ctrl(*)>(&Ipb_rspec_chan6_group_base->chnl_fifo_ctrl), new Chnl_fifo_ctrl_map, {}, sizeof(Chnl_fifo_ctrl) } },
    { "chnl_acc_ctrl", { const_cast<Chnl_acc_ctrl(*)>(&Ipb_rspec_chan6_group_base->chnl_acc_ctrl), new Chnl_acc_ctrl_map, {}, sizeof(Chnl_acc_ctrl) } },
    { "chnl_acc_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan6_group_base->chnl_acc_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan6_group_base->chnl_pktnum0_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan6_group_base->chnl_pktnum1_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan6_group_base->chnl_metanum_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan6_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Ipb_rspec_chan6_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Ipb_rspec_chan6_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum", { const_cast<uint32_t(*)>(&Ipb_rspec_chan6_group_base->chnl_metanum), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_drop_pkt", { const_cast<Chnl_deparser_drop_pkt(*)>(&Ipb_rspec_chan6_group_base->chnl_deparser_drop_pkt), new Chnl_deparser_drop_pkt_map, {}, sizeof(Chnl_deparser_drop_pkt) } },
    { "chnl_wsch_discard_pkt", { const_cast<Chnl_wsch_discard_pkt(*)>(&Ipb_rspec_chan6_group_base->chnl_wsch_discard_pkt), new Chnl_wsch_discard_pkt_map, {}, sizeof(Chnl_wsch_discard_pkt) } },
    { "chnl_wsch_trunc_pkt", { const_cast<Chnl_wsch_trunc_pkt(*)>(&Ipb_rspec_chan6_group_base->chnl_wsch_trunc_pkt), new Chnl_wsch_trunc_pkt_map, {}, sizeof(Chnl_wsch_trunc_pkt) } },
    { "chnl_drop_trunc_pkt", { const_cast<Chnl_drop_trunc_pkt(*)>(&Ipb_rspec_chan6_group_base->chnl_drop_trunc_pkt), new Chnl_drop_trunc_pkt_map, {}, sizeof(Chnl_drop_trunc_pkt) } },
    { "chnl_resubmit_discard_pkt", { const_cast<Chnl_resubmit_discard_pkt(*)>(&Ipb_rspec_chan6_group_base->chnl_resubmit_discard_pkt), new Chnl_resubmit_discard_pkt_map, {}, sizeof(Chnl_resubmit_discard_pkt) } },
    { "chnl_parser_discard_pkt", { const_cast<Chnl_parser_discard_pkt(*)>(&Ipb_rspec_chan6_group_base->chnl_parser_discard_pkt), new Chnl_parser_discard_pkt_map, {}, sizeof(Chnl_parser_discard_pkt) } },
    { "chnl_parser_send_pkt", { const_cast<Chnl_parser_send_pkt(*)>(&Ipb_rspec_chan6_group_base->chnl_parser_send_pkt), new Chnl_parser_send_pkt_map, {}, sizeof(Chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Chnl_deparser_send_pkt(*)>(&Ipb_rspec_chan6_group_base->chnl_deparser_send_pkt), new Chnl_deparser_send_pkt_map, {}, sizeof(Chnl_deparser_send_pkt) } },
    { "chnl_macs_received_pkt", { const_cast<Chnl_macs_received_pkt(*)>(&Ipb_rspec_chan6_group_base->chnl_macs_received_pkt), new Chnl_macs_received_pkt_map, {}, sizeof(Chnl_macs_received_pkt) } },
    { "chnl_resubmit_received_pkt", { const_cast<Chnl_resubmit_received_pkt(*)>(&Ipb_rspec_chan6_group_base->chnl_resubmit_received_pkt), new Chnl_resubmit_received_pkt_map, {}, sizeof(Chnl_resubmit_received_pkt) } }
    } )
  {}
};

struct Ipb_rspec_chan7_group_map: public RegisterMapper {
  static constexpr PTR_Ipb_rspec_chan7_group Ipb_rspec_chan7_group_base=0;
  Ipb_rspec_chan7_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Chnl_ctrl(*)>(&Ipb_rspec_chan7_group_base->chnl_ctrl), new Chnl_ctrl_map, {}, sizeof(Chnl_ctrl) } },
    { "chnl_meta", { const_cast<Chnl_meta(*)>(&Ipb_rspec_chan7_group_base->chnl_meta), new Chnl_meta_map, {}, sizeof(Chnl_meta) } },
    { "meta_fifo_ctrl", { const_cast<uint32_t(*)>(&Ipb_rspec_chan7_group_base->meta_fifo_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_ctrl", { const_cast<Chnl_fifo_ctrl(*)>(&Ipb_rspec_chan7_group_base->chnl_fifo_ctrl), new Chnl_fifo_ctrl_map, {}, sizeof(Chnl_fifo_ctrl) } },
    { "chnl_acc_ctrl", { const_cast<Chnl_acc_ctrl(*)>(&Ipb_rspec_chan7_group_base->chnl_acc_ctrl), new Chnl_acc_ctrl_map, {}, sizeof(Chnl_acc_ctrl) } },
    { "chnl_acc_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan7_group_base->chnl_acc_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan7_group_base->chnl_pktnum0_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan7_group_base->chnl_pktnum1_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum_cfg", { const_cast<uint32_t(*)>(&Ipb_rspec_chan7_group_base->chnl_metanum_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Ipb_rspec_chan7_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Ipb_rspec_chan7_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Ipb_rspec_chan7_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_metanum", { const_cast<uint32_t(*)>(&Ipb_rspec_chan7_group_base->chnl_metanum), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_drop_pkt", { const_cast<Chnl_deparser_drop_pkt(*)>(&Ipb_rspec_chan7_group_base->chnl_deparser_drop_pkt), new Chnl_deparser_drop_pkt_map, {}, sizeof(Chnl_deparser_drop_pkt) } },
    { "chnl_wsch_discard_pkt", { const_cast<Chnl_wsch_discard_pkt(*)>(&Ipb_rspec_chan7_group_base->chnl_wsch_discard_pkt), new Chnl_wsch_discard_pkt_map, {}, sizeof(Chnl_wsch_discard_pkt) } },
    { "chnl_wsch_trunc_pkt", { const_cast<Chnl_wsch_trunc_pkt(*)>(&Ipb_rspec_chan7_group_base->chnl_wsch_trunc_pkt), new Chnl_wsch_trunc_pkt_map, {}, sizeof(Chnl_wsch_trunc_pkt) } },
    { "chnl_drop_trunc_pkt", { const_cast<Chnl_drop_trunc_pkt(*)>(&Ipb_rspec_chan7_group_base->chnl_drop_trunc_pkt), new Chnl_drop_trunc_pkt_map, {}, sizeof(Chnl_drop_trunc_pkt) } },
    { "chnl_resubmit_discard_pkt", { const_cast<Chnl_resubmit_discard_pkt(*)>(&Ipb_rspec_chan7_group_base->chnl_resubmit_discard_pkt), new Chnl_resubmit_discard_pkt_map, {}, sizeof(Chnl_resubmit_discard_pkt) } },
    { "chnl_parser_discard_pkt", { const_cast<Chnl_parser_discard_pkt(*)>(&Ipb_rspec_chan7_group_base->chnl_parser_discard_pkt), new Chnl_parser_discard_pkt_map, {}, sizeof(Chnl_parser_discard_pkt) } },
    { "chnl_parser_send_pkt", { const_cast<Chnl_parser_send_pkt(*)>(&Ipb_rspec_chan7_group_base->chnl_parser_send_pkt), new Chnl_parser_send_pkt_map, {}, sizeof(Chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Chnl_deparser_send_pkt(*)>(&Ipb_rspec_chan7_group_base->chnl_deparser_send_pkt), new Chnl_deparser_send_pkt_map, {}, sizeof(Chnl_deparser_send_pkt) } },
    { "chnl_macs_received_pkt", { const_cast<Chnl_macs_received_pkt(*)>(&Ipb_rspec_chan7_group_base->chnl_macs_received_pkt), new Chnl_macs_received_pkt_map, {}, sizeof(Chnl_macs_received_pkt) } },
    { "chnl_resubmit_received_pkt", { const_cast<Chnl_resubmit_received_pkt(*)>(&Ipb_rspec_chan7_group_base->chnl_resubmit_received_pkt), new Chnl_resubmit_received_pkt_map, {}, sizeof(Chnl_resubmit_received_pkt) } }
    } )
  {}
};

struct Ipb_rspec_map: public RegisterMapper {
  static constexpr PTR_Ipb_rspec Ipb_rspec_base=0;
  Ipb_rspec_map() : RegisterMapper( {
    { "glb_group", { &Ipb_rspec_base->glb_group, new Glb_group_map, {}, sizeof(Glb_group) } },
    { "chan0_group", { &Ipb_rspec_base->chan0_group, new Ipb_rspec_chan0_group_map, {}, sizeof(Ipb_rspec_chan0_group) } },
    { "chan1_group", { &Ipb_rspec_base->chan1_group, new Ipb_rspec_chan1_group_map, {}, sizeof(Ipb_rspec_chan1_group) } },
    { "chan2_group", { &Ipb_rspec_base->chan2_group, new Ipb_rspec_chan2_group_map, {}, sizeof(Ipb_rspec_chan2_group) } },
    { "chan3_group", { &Ipb_rspec_base->chan3_group, new Ipb_rspec_chan3_group_map, {}, sizeof(Ipb_rspec_chan3_group) } },
    { "chan4_group", { &Ipb_rspec_base->chan4_group, new Ipb_rspec_chan4_group_map, {}, sizeof(Ipb_rspec_chan4_group) } },
    { "chan5_group", { &Ipb_rspec_base->chan5_group, new Ipb_rspec_chan5_group_map, {}, sizeof(Ipb_rspec_chan5_group) } },
    { "chan6_group", { &Ipb_rspec_base->chan6_group, new Ipb_rspec_chan6_group_map, {}, sizeof(Ipb_rspec_chan6_group) } },
    { "chan7_group", { &Ipb_rspec_base->chan7_group, new Ipb_rspec_chan7_group_map, {}, sizeof(Ipb_rspec_chan7_group) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Ipb_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } },
    { "scratch", { const_cast<uint32_t(*)>(&Ipb_rspec_base->scratch), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_phv_owner_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_phv_owner Prsr_reg_main_rspec_phv_owner_base=0;
  Prsr_reg_main_rspec_phv_owner_map() : RegisterMapper( {
    { "phv_owner_0_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_0_8), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_1_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_1_8), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_2_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_2_8), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_3_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_3_8), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_4_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_4_8), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_5_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_5_8), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_6_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_6_8), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_7_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_7_8), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_no_multi_wr_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_no_multi_wr Prsr_reg_main_rspec_no_multi_wr_base=0;
  Prsr_reg_main_rspec_no_multi_wr_map() : RegisterMapper( {
    { "no_multi_wr_0_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_0_8), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_1_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_1_8), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_2_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_2_8), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_3_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_3_8), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_4_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_4_8), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_5_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_5_8), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_6_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_6_8), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_7_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_7_8), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_hdr_byte_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_hdr_byte_cnt Prsr_reg_main_rspec_hdr_byte_cnt_base=0;
  Prsr_reg_main_rspec_hdr_byte_cnt_map() : RegisterMapper( {
    { "hdr_byte_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_hdr_byte_cnt_base->hdr_byte_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "hdr_byte_cnt_1_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_hdr_byte_cnt_base->hdr_byte_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_idle_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_idle_cnt Prsr_reg_main_rspec_idle_cnt_base=0;
  Prsr_reg_main_rspec_idle_cnt_map() : RegisterMapper( {
    { "idle_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_idle_cnt_base->idle_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "idle_cnt_1_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_idle_cnt_base->idle_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_pkt_rx_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_pkt_rx_cnt Prsr_reg_main_rspec_pkt_rx_cnt_base=0;
  Prsr_reg_main_rspec_pkt_rx_cnt_map() : RegisterMapper( {
    { "pkt_rx_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_pkt_rx_cnt_base->pkt_rx_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "pkt_rx_cnt_1_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_pkt_rx_cnt_base->pkt_rx_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_pkt_tx_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_pkt_tx_cnt Prsr_reg_main_rspec_pkt_tx_cnt_base=0;
  Prsr_reg_main_rspec_pkt_tx_cnt_map() : RegisterMapper( {
    { "pkt_tx_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_pkt_tx_cnt_base->pkt_tx_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "pkt_tx_cnt_1_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_pkt_tx_cnt_base->pkt_tx_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_pkt_drop_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_pkt_drop_cnt Prsr_reg_main_rspec_pkt_drop_cnt_base=0;
  Prsr_reg_main_rspec_pkt_drop_cnt_map() : RegisterMapper( {
    { "pkt_drop_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_pkt_drop_cnt_base->pkt_drop_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "pkt_drop_cnt_1_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_pkt_drop_cnt_base->pkt_drop_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_op_fifo_full_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_op_fifo_full_cnt Prsr_reg_main_rspec_op_fifo_full_cnt_base=0;
  Prsr_reg_main_rspec_op_fifo_full_cnt_map() : RegisterMapper( {
    { "op_fifo_full_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_op_fifo_full_cnt_base->op_fifo_full_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "op_fifo_full_cnt_1_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_op_fifo_full_cnt_base->op_fifo_full_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_op_fifo_full_stall_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_op_fifo_full_stall_cnt Prsr_reg_main_rspec_op_fifo_full_stall_cnt_base=0;
  Prsr_reg_main_rspec_op_fifo_full_stall_cnt_map() : RegisterMapper( {
    { "op_fifo_full_stall_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_op_fifo_full_stall_cnt_base->op_fifo_full_stall_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "op_fifo_full_stall_cnt_1_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_op_fifo_full_stall_cnt_base->op_fifo_full_stall_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_no_tcam_match_err_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_no_tcam_match_err_cnt Prsr_reg_main_rspec_no_tcam_match_err_cnt_base=0;
  Prsr_reg_main_rspec_no_tcam_match_err_cnt_map() : RegisterMapper( {
    { "no_tcam_match_err_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_tcam_match_err_cnt_base->no_tcam_match_err_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "no_tcam_match_err_cnt_1_2", { &Prsr_reg_main_rspec_no_tcam_match_err_cnt_base->no_tcam_match_err_cnt_1_2, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_partial_hdr_err_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_partial_hdr_err_cnt Prsr_reg_main_rspec_partial_hdr_err_cnt_base=0;
  Prsr_reg_main_rspec_partial_hdr_err_cnt_map() : RegisterMapper( {
    { "partial_hdr_err_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_partial_hdr_err_cnt_base->partial_hdr_err_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "partial_hdr_err_cnt_1_2", { &Prsr_reg_main_rspec_partial_hdr_err_cnt_base->partial_hdr_err_cnt_1_2, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_ctr_range_err_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_ctr_range_err_cnt Prsr_reg_main_rspec_ctr_range_err_cnt_base=0;
  Prsr_reg_main_rspec_ctr_range_err_cnt_map() : RegisterMapper( {
    { "ctr_range_err_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_ctr_range_err_cnt_base->ctr_range_err_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "ctr_range_err_cnt_1_2", { &Prsr_reg_main_rspec_ctr_range_err_cnt_base->ctr_range_err_cnt_1_2, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_timeout_iter_err_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_timeout_iter_err_cnt Prsr_reg_main_rspec_timeout_iter_err_cnt_base=0;
  Prsr_reg_main_rspec_timeout_iter_err_cnt_map() : RegisterMapper( {
    { "timeout_iter_err_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_timeout_iter_err_cnt_base->timeout_iter_err_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "timeout_iter_err_cnt_1_2", { &Prsr_reg_main_rspec_timeout_iter_err_cnt_base->timeout_iter_err_cnt_1_2, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_timeout_cycle_err_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_timeout_cycle_err_cnt Prsr_reg_main_rspec_timeout_cycle_err_cnt_base=0;
  Prsr_reg_main_rspec_timeout_cycle_err_cnt_map() : RegisterMapper( {
    { "timeout_cycle_err_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_timeout_cycle_err_cnt_base->timeout_cycle_err_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "timeout_cycle_err_cnt_1_2", { &Prsr_reg_main_rspec_timeout_cycle_err_cnt_base->timeout_cycle_err_cnt_1_2, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_src_ext_err_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_src_ext_err_cnt Prsr_reg_main_rspec_src_ext_err_cnt_base=0;
  Prsr_reg_main_rspec_src_ext_err_cnt_map() : RegisterMapper( {
    { "src_ext_err_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_src_ext_err_cnt_base->src_ext_err_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "src_ext_err_cnt_1_2", { &Prsr_reg_main_rspec_src_ext_err_cnt_base->src_ext_err_cnt_1_2, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_phv_owner_err_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_phv_owner_err_cnt Prsr_reg_main_rspec_phv_owner_err_cnt_base=0;
  Prsr_reg_main_rspec_phv_owner_err_cnt_map() : RegisterMapper( {
    { "phv_owner_err_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_err_cnt_base->phv_owner_err_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_err_cnt_1_2", { &Prsr_reg_main_rspec_phv_owner_err_cnt_base->phv_owner_err_cnt_1_2, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_multi_wr_err_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_multi_wr_err_cnt Prsr_reg_main_rspec_multi_wr_err_cnt_base=0;
  Prsr_reg_main_rspec_multi_wr_err_cnt_map() : RegisterMapper( {
    { "multi_wr_err_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_multi_wr_err_cnt_base->multi_wr_err_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "multi_wr_err_cnt_1_2", { &Prsr_reg_main_rspec_multi_wr_err_cnt_base->multi_wr_err_cnt_1_2, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_aram_sbe_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_aram_sbe_cnt Prsr_reg_main_rspec_aram_sbe_cnt_base=0;
  Prsr_reg_main_rspec_aram_sbe_cnt_map() : RegisterMapper( {
    { "aram_sbe_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_aram_sbe_cnt_base->aram_sbe_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "aram_sbe_cnt_1_2", { &Prsr_reg_main_rspec_aram_sbe_cnt_base->aram_sbe_cnt_1_2, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_aram_mbe_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_aram_mbe_cnt Prsr_reg_main_rspec_aram_mbe_cnt_base=0;
  Prsr_reg_main_rspec_aram_mbe_cnt_map() : RegisterMapper( {
    { "aram_mbe_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_aram_mbe_cnt_base->aram_mbe_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "aram_mbe_cnt_1_2", { &Prsr_reg_main_rspec_aram_mbe_cnt_base->aram_mbe_cnt_1_2, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_fcs_err_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_fcs_err_cnt Prsr_reg_main_rspec_fcs_err_cnt_base=0;
  Prsr_reg_main_rspec_fcs_err_cnt_map() : RegisterMapper( {
    { "fcs_err_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_fcs_err_cnt_base->fcs_err_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "fcs_err_cnt_1_2", { &Prsr_reg_main_rspec_fcs_err_cnt_base->fcs_err_cnt_1_2, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_csum_err_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_csum_err_cnt Prsr_reg_main_rspec_csum_err_cnt_base=0;
  Prsr_reg_main_rspec_csum_err_cnt_map() : RegisterMapper( {
    { "csum_err_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_csum_err_cnt_base->csum_err_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "csum_err_cnt_1_2", { &Prsr_reg_main_rspec_csum_err_cnt_base->csum_err_cnt_1_2, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_tcam_par_err_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_tcam_par_err_cnt Prsr_reg_main_rspec_tcam_par_err_cnt_base=0;
  Prsr_reg_main_rspec_tcam_par_err_cnt_map() : RegisterMapper( {
    { "tcam_par_err_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_tcam_par_err_cnt_base->tcam_par_err_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "tcam_par_err_cnt_1_2", { &Prsr_reg_main_rspec_tcam_par_err_cnt_base->tcam_par_err_cnt_1_2, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_csum_sbe_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_csum_sbe_cnt Prsr_reg_main_rspec_csum_sbe_cnt_base=0;
  Prsr_reg_main_rspec_csum_sbe_cnt_map() : RegisterMapper( {
    { "csum_sbe_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_csum_sbe_cnt_base->csum_sbe_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "csum_sbe_cnt_1_2", { &Prsr_reg_main_rspec_csum_sbe_cnt_base->csum_sbe_cnt_1_2, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_csum_mbe_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_csum_mbe_cnt Prsr_reg_main_rspec_csum_mbe_cnt_base=0;
  Prsr_reg_main_rspec_csum_mbe_cnt_map() : RegisterMapper( {
    { "csum_mbe_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_csum_mbe_cnt_base->csum_mbe_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "csum_mbe_cnt_1_2", { &Prsr_reg_main_rspec_csum_mbe_cnt_base->csum_mbe_cnt_1_2, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_intr Prsr_reg_main_rspec_intr_base=0;
  Prsr_reg_main_rspec_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_ecc_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_ecc Prsr_reg_main_rspec_ecc_base=0;
  Prsr_reg_main_rspec_ecc_map() : RegisterMapper( {
    { "ecc_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_ecc_base->ecc_0_2), 0, {}, sizeof(uint32_t) } },
    { "ecc_1_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_ecc_base->ecc_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_phv_clr_on_wr_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_phv_clr_on_wr Prsr_reg_main_rspec_phv_clr_on_wr_base=0;
  Prsr_reg_main_rspec_phv_clr_on_wr_map() : RegisterMapper( {
    { "phv_clr_on_wr_0_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_clr_on_wr_base->phv_clr_on_wr_0_8), 0, {}, sizeof(uint32_t) } },
    { "phv_clr_on_wr_1_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_clr_on_wr_base->phv_clr_on_wr_1_8), 0, {}, sizeof(uint32_t) } },
    { "phv_clr_on_wr_2_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_clr_on_wr_base->phv_clr_on_wr_2_8), 0, {}, sizeof(uint32_t) } },
    { "phv_clr_on_wr_3_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_clr_on_wr_base->phv_clr_on_wr_3_8), 0, {}, sizeof(uint32_t) } },
    { "phv_clr_on_wr_4_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_clr_on_wr_base->phv_clr_on_wr_4_8), 0, {}, sizeof(uint32_t) } },
    { "phv_clr_on_wr_5_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_clr_on_wr_base->phv_clr_on_wr_5_8), 0, {}, sizeof(uint32_t) } },
    { "phv_clr_on_wr_6_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_clr_on_wr_base->phv_clr_on_wr_6_8), 0, {}, sizeof(uint32_t) } },
    { "phv_clr_on_wr_7_8", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_clr_on_wr_base->phv_clr_on_wr_7_8), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec Prsr_reg_main_rspec_base=0;
  Prsr_reg_main_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "port_rate_cfg", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->port_rate_cfg), 0, {}, sizeof(uint32_t) } },
    { "port_chnl_en", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->port_chnl_en), 0, {}, sizeof(uint32_t) } },
    { "start_state", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->start_state), 0, {}, sizeof(uint32_t) } },
    { "start_lookup_offsets", { const_cast<uint32_t(*)[0x2]>(&Prsr_reg_main_rspec_base->start_lookup_offsets), 0, {0x2}, sizeof(uint32_t) } },
    { "max_iter", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->max_iter), 0, {}, sizeof(uint32_t) } },
    { "max_cycle", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->max_cycle), 0, {}, sizeof(uint32_t) } },
    { "pri_start", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->pri_start), 0, {}, sizeof(uint32_t) } },
    { "pri_thresh", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->pri_thresh), 0, {}, sizeof(uint32_t) } },
    { "hdr_len_adj", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->hdr_len_adj), 0, {}, sizeof(uint32_t) } },
    { "pri_map", { const_cast<uint32_t(*)[0x2]>(&Prsr_reg_main_rspec_base->pri_map), 0, {0x2}, sizeof(uint32_t) } },
    { "seq_reset", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->seq_reset), 0, {}, sizeof(uint32_t) } },
    { "out_arb_ctrl", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->out_arb_ctrl), 0, {}, sizeof(uint32_t) } },
    { "phv_owner", { const_cast<Prsr_reg_main_rspec_phv_owner(*)>(&Prsr_reg_main_rspec_base->phv_owner), new Prsr_reg_main_rspec_phv_owner_map, {}, sizeof(Prsr_reg_main_rspec_phv_owner) } },
    { "no_multi_wr", { const_cast<Prsr_reg_main_rspec_no_multi_wr(*)>(&Prsr_reg_main_rspec_base->no_multi_wr), new Prsr_reg_main_rspec_no_multi_wr_map, {}, sizeof(Prsr_reg_main_rspec_no_multi_wr) } },
    { "err_phv_cfg", { const_cast<uint32_t(*)[0x2]>(&Prsr_reg_main_rspec_base->err_phv_cfg), 0, {0x2}, sizeof(uint32_t) } },
    { "hdr_byte_cnt", { const_cast<Prsr_reg_main_rspec_hdr_byte_cnt(*)[0x2]>(&Prsr_reg_main_rspec_base->hdr_byte_cnt), new Prsr_reg_main_rspec_hdr_byte_cnt_map, {0x2}, sizeof(Prsr_reg_main_rspec_hdr_byte_cnt) } },
    { "idle_cnt", { const_cast<Prsr_reg_main_rspec_idle_cnt(*)[0x2]>(&Prsr_reg_main_rspec_base->idle_cnt), new Prsr_reg_main_rspec_idle_cnt_map, {0x2}, sizeof(Prsr_reg_main_rspec_idle_cnt) } },
    { "pkt_rx_cnt", { const_cast<Prsr_reg_main_rspec_pkt_rx_cnt(*)[0x2]>(&Prsr_reg_main_rspec_base->pkt_rx_cnt), new Prsr_reg_main_rspec_pkt_rx_cnt_map, {0x2}, sizeof(Prsr_reg_main_rspec_pkt_rx_cnt) } },
    { "pkt_tx_cnt", { const_cast<Prsr_reg_main_rspec_pkt_tx_cnt(*)[0x2]>(&Prsr_reg_main_rspec_base->pkt_tx_cnt), new Prsr_reg_main_rspec_pkt_tx_cnt_map, {0x2}, sizeof(Prsr_reg_main_rspec_pkt_tx_cnt) } },
    { "pkt_drop_cnt", { const_cast<Prsr_reg_main_rspec_pkt_drop_cnt(*)[0x2]>(&Prsr_reg_main_rspec_base->pkt_drop_cnt), new Prsr_reg_main_rspec_pkt_drop_cnt_map, {0x2}, sizeof(Prsr_reg_main_rspec_pkt_drop_cnt) } },
    { "op_fifo_full_cnt", { const_cast<Prsr_reg_main_rspec_op_fifo_full_cnt(*)>(&Prsr_reg_main_rspec_base->op_fifo_full_cnt), new Prsr_reg_main_rspec_op_fifo_full_cnt_map, {}, sizeof(Prsr_reg_main_rspec_op_fifo_full_cnt) } },
    { "op_fifo_full_stall_cnt", { const_cast<Prsr_reg_main_rspec_op_fifo_full_stall_cnt(*)>(&Prsr_reg_main_rspec_base->op_fifo_full_stall_cnt), new Prsr_reg_main_rspec_op_fifo_full_stall_cnt_map, {}, sizeof(Prsr_reg_main_rspec_op_fifo_full_stall_cnt) } },
    { "no_tcam_match_err_cnt", { const_cast<Prsr_reg_main_rspec_no_tcam_match_err_cnt(*)>(&Prsr_reg_main_rspec_base->no_tcam_match_err_cnt), new Prsr_reg_main_rspec_no_tcam_match_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_no_tcam_match_err_cnt) } },
    { "partial_hdr_err_cnt", { const_cast<Prsr_reg_main_rspec_partial_hdr_err_cnt(*)>(&Prsr_reg_main_rspec_base->partial_hdr_err_cnt), new Prsr_reg_main_rspec_partial_hdr_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_partial_hdr_err_cnt) } },
    { "ctr_range_err_cnt", { const_cast<Prsr_reg_main_rspec_ctr_range_err_cnt(*)>(&Prsr_reg_main_rspec_base->ctr_range_err_cnt), new Prsr_reg_main_rspec_ctr_range_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_ctr_range_err_cnt) } },
    { "timeout_iter_err_cnt", { const_cast<Prsr_reg_main_rspec_timeout_iter_err_cnt(*)>(&Prsr_reg_main_rspec_base->timeout_iter_err_cnt), new Prsr_reg_main_rspec_timeout_iter_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_timeout_iter_err_cnt) } },
    { "timeout_cycle_err_cnt", { const_cast<Prsr_reg_main_rspec_timeout_cycle_err_cnt(*)>(&Prsr_reg_main_rspec_base->timeout_cycle_err_cnt), new Prsr_reg_main_rspec_timeout_cycle_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_timeout_cycle_err_cnt) } },
    { "src_ext_err_cnt", { const_cast<Prsr_reg_main_rspec_src_ext_err_cnt(*)>(&Prsr_reg_main_rspec_base->src_ext_err_cnt), new Prsr_reg_main_rspec_src_ext_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_src_ext_err_cnt) } },
    { "phv_owner_err_cnt", { const_cast<Prsr_reg_main_rspec_phv_owner_err_cnt(*)>(&Prsr_reg_main_rspec_base->phv_owner_err_cnt), new Prsr_reg_main_rspec_phv_owner_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_phv_owner_err_cnt) } },
    { "multi_wr_err_cnt", { const_cast<Prsr_reg_main_rspec_multi_wr_err_cnt(*)>(&Prsr_reg_main_rspec_base->multi_wr_err_cnt), new Prsr_reg_main_rspec_multi_wr_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_multi_wr_err_cnt) } },
    { "aram_sbe_cnt", { const_cast<Prsr_reg_main_rspec_aram_sbe_cnt(*)>(&Prsr_reg_main_rspec_base->aram_sbe_cnt), new Prsr_reg_main_rspec_aram_sbe_cnt_map, {}, sizeof(Prsr_reg_main_rspec_aram_sbe_cnt) } },
    { "aram_mbe_cnt", { const_cast<Prsr_reg_main_rspec_aram_mbe_cnt(*)>(&Prsr_reg_main_rspec_base->aram_mbe_cnt), new Prsr_reg_main_rspec_aram_mbe_cnt_map, {}, sizeof(Prsr_reg_main_rspec_aram_mbe_cnt) } },
    { "fcs_err_cnt", { const_cast<Prsr_reg_main_rspec_fcs_err_cnt(*)>(&Prsr_reg_main_rspec_base->fcs_err_cnt), new Prsr_reg_main_rspec_fcs_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_fcs_err_cnt) } },
    { "csum_err_cnt", { const_cast<Prsr_reg_main_rspec_csum_err_cnt(*)>(&Prsr_reg_main_rspec_base->csum_err_cnt), new Prsr_reg_main_rspec_csum_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_csum_err_cnt) } },
    { "tcam_par_err_cnt", { const_cast<Prsr_reg_main_rspec_tcam_par_err_cnt(*)>(&Prsr_reg_main_rspec_base->tcam_par_err_cnt), new Prsr_reg_main_rspec_tcam_par_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_tcam_par_err_cnt) } },
    { "csum_sbe_cnt", { const_cast<Prsr_reg_main_rspec_csum_sbe_cnt(*)>(&Prsr_reg_main_rspec_base->csum_sbe_cnt), new Prsr_reg_main_rspec_csum_sbe_cnt_map, {}, sizeof(Prsr_reg_main_rspec_csum_sbe_cnt) } },
    { "csum_mbe_cnt", { const_cast<Prsr_reg_main_rspec_csum_mbe_cnt(*)>(&Prsr_reg_main_rspec_base->csum_mbe_cnt), new Prsr_reg_main_rspec_csum_mbe_cnt_map, {}, sizeof(Prsr_reg_main_rspec_csum_mbe_cnt) } },
    { "intr", { &Prsr_reg_main_rspec_base->intr, new Prsr_reg_main_rspec_intr_map, {}, sizeof(Prsr_reg_main_rspec_intr) } },
    { "no_tcam_match_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->no_tcam_match_err_log), 0, {}, sizeof(uint32_t) } },
    { "timeout_iter_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->timeout_iter_err_log), 0, {}, sizeof(uint32_t) } },
    { "timeout_cycle_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->timeout_cycle_err_log), 0, {}, sizeof(uint32_t) } },
    { "partial_hdr_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->partial_hdr_err_log), 0, {}, sizeof(uint32_t) } },
    { "ctr_range_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->ctr_range_err_log), 0, {}, sizeof(uint32_t) } },
    { "multi_wr_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->multi_wr_err_log), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->phv_owner_err_log), 0, {}, sizeof(uint32_t) } },
    { "src_ext_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->src_ext_err_log), 0, {}, sizeof(uint32_t) } },
    { "aram_sbe_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->aram_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "aram_mbe_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->aram_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "tcam_par_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->tcam_par_err_log), 0, {}, sizeof(uint32_t) } },
    { "ibuf_oflow_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->ibuf_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "ibuf_uflow_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->ibuf_uflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "op_fifo_oflow_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->op_fifo_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "op_fifo_uflow_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->op_fifo_uflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "csum_sbe_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->csum_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "csum_mbe_err_log", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->csum_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<Prsr_reg_main_rspec_ecc(*)>(&Prsr_reg_main_rspec_base->ecc), new Prsr_reg_main_rspec_ecc_map, {}, sizeof(Prsr_reg_main_rspec_ecc) } },
    { "parity", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->parity), 0, {}, sizeof(uint32_t) } },
    { "debug_ctrl", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->debug_ctrl), 0, {}, sizeof(uint32_t) } },
    { "mem_ctrl", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->mem_ctrl), 0, {}, sizeof(uint32_t) } },
    { "op_fifo_state", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->op_fifo_state), 0, {}, sizeof(uint32_t) } },
    { "ver_upd", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->ver_upd), 0, {}, sizeof(uint32_t) } },
    { "iq_state", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->iq_state), 0, {}, sizeof(uint32_t) } },
    { "phv_clr_on_wr", { const_cast<Prsr_reg_main_rspec_phv_clr_on_wr(*)>(&Prsr_reg_main_rspec_base->phv_clr_on_wr), new Prsr_reg_main_rspec_phv_clr_on_wr_map, {}, sizeof(Prsr_reg_main_rspec_phv_clr_on_wr) } }
    } )
  {}
};

struct Ipb_prsr4_reg_map: public RegisterMapper {
  static constexpr PTR_Ipb_prsr4_reg Ipb_prsr4_reg_base=0;
  Ipb_prsr4_reg_map() : RegisterMapper( {
    { "ipbreg", { &Ipb_prsr4_reg_base->ipbreg, new Ipb_rspec_map, {}, sizeof(Ipb_rspec) } },
    { "prsr", { &Ipb_prsr4_reg_base->prsr, new Prsr_reg_main_rspec_map, {0x4}, sizeof(Prsr_reg_main_rspec) } }
    } )
  {}
};

struct Pmerge_lower_left_reg_intr_map: public RegisterMapper {
  static constexpr PTR_Pmerge_lower_left_reg_intr Pmerge_lower_left_reg_intr_base=0;
  Pmerge_lower_left_reg_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Pmerge_lower_left_reg_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Pmerge_lower_left_reg_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Pmerge_lower_left_reg_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Pmerge_lower_left_reg_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Pmerge_lower_left_reg_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_lower_left_reg_map: public RegisterMapper {
  static constexpr PTR_Pmerge_lower_left_reg Pmerge_lower_left_reg_base=0;
  Pmerge_lower_left_reg_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Pmerge_lower_left_reg_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Pmerge_lower_left_reg_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "port_rate_cfg_4_0", { const_cast<uint32_t(*)[0x5]>(&Pmerge_lower_left_reg_base->port_rate_cfg_4_0), 0, {0x5}, sizeof(uint32_t) } },
    { "i_mac_empty_4_0", { const_cast<uint32_t(*)[0x5]>(&Pmerge_lower_left_reg_base->i_mac_empty_4_0), 0, {0x5}, sizeof(uint32_t) } },
    { "debug_ctrl", { const_cast<uint32_t(*)>(&Pmerge_lower_left_reg_base->debug_ctrl), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Pmerge_lower_left_reg_base->intr, new Pmerge_lower_left_reg_intr_map, {}, sizeof(Pmerge_lower_left_reg_intr) } },
    { "i_occ_oflow_err_log", { const_cast<uint32_t(*)>(&Pmerge_lower_left_reg_base->i_occ_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_slot_ou_flow_err_log", { const_cast<uint32_t(*)>(&Pmerge_lower_left_reg_base->i_slot_ou_flow_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_pre_wr_addr", { const_cast<uint32_t(*)>(&Pmerge_lower_left_reg_base->i_pre_wr_addr), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_upper_left_reg_phv_owner_127_0_map: public RegisterMapper {
  static constexpr PTR_Pmerge_upper_left_reg_phv_owner_127_0 Pmerge_upper_left_reg_phv_owner_127_0_base=0;
  Pmerge_upper_left_reg_phv_owner_127_0_map() : RegisterMapper( {
    { "phv_owner_127_0_0_4", { const_cast<uint32_t(*)>(&Pmerge_upper_left_reg_phv_owner_127_0_base->phv_owner_127_0_0_4), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_127_0_1_4", { const_cast<uint32_t(*)>(&Pmerge_upper_left_reg_phv_owner_127_0_base->phv_owner_127_0_1_4), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_127_0_2_4", { const_cast<uint32_t(*)>(&Pmerge_upper_left_reg_phv_owner_127_0_base->phv_owner_127_0_2_4), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_127_0_3_4", { const_cast<uint32_t(*)>(&Pmerge_upper_left_reg_phv_owner_127_0_base->phv_owner_127_0_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_upper_left_reg_phv_clr_on_wr_127_0_map: public RegisterMapper {
  static constexpr PTR_Pmerge_upper_left_reg_phv_clr_on_wr_127_0 Pmerge_upper_left_reg_phv_clr_on_wr_127_0_base=0;
  Pmerge_upper_left_reg_phv_clr_on_wr_127_0_map() : RegisterMapper( {
    { "phv_clr_on_wr_127_0_0_4", { const_cast<uint32_t(*)>(&Pmerge_upper_left_reg_phv_clr_on_wr_127_0_base->phv_clr_on_wr_127_0_0_4), 0, {}, sizeof(uint32_t) } },
    { "phv_clr_on_wr_127_0_1_4", { const_cast<uint32_t(*)>(&Pmerge_upper_left_reg_phv_clr_on_wr_127_0_base->phv_clr_on_wr_127_0_1_4), 0, {}, sizeof(uint32_t) } },
    { "phv_clr_on_wr_127_0_2_4", { const_cast<uint32_t(*)>(&Pmerge_upper_left_reg_phv_clr_on_wr_127_0_base->phv_clr_on_wr_127_0_2_4), 0, {}, sizeof(uint32_t) } },
    { "phv_clr_on_wr_127_0_3_4", { const_cast<uint32_t(*)>(&Pmerge_upper_left_reg_phv_clr_on_wr_127_0_base->phv_clr_on_wr_127_0_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_upper_left_reg_map: public RegisterMapper {
  static constexpr PTR_Pmerge_upper_left_reg Pmerge_upper_left_reg_base=0;
  Pmerge_upper_left_reg_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Pmerge_upper_left_reg_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "port_rate_cfg_8_5", { const_cast<uint32_t(*)[0x4]>(&Pmerge_upper_left_reg_base->port_rate_cfg_8_5), 0, {0x4}, sizeof(uint32_t) } },
    { "phv_owner_127_0", { const_cast<Pmerge_upper_left_reg_phv_owner_127_0(*)>(&Pmerge_upper_left_reg_base->phv_owner_127_0), new Pmerge_upper_left_reg_phv_owner_127_0_map, {}, sizeof(Pmerge_upper_left_reg_phv_owner_127_0) } },
    { "phv_clr_on_wr_127_0", { const_cast<Pmerge_upper_left_reg_phv_clr_on_wr_127_0(*)>(&Pmerge_upper_left_reg_base->phv_clr_on_wr_127_0), new Pmerge_upper_left_reg_phv_clr_on_wr_127_0_map, {}, sizeof(Pmerge_upper_left_reg_phv_clr_on_wr_127_0) } },
    { "i_mac_empty_8_5", { const_cast<uint32_t(*)[0x4]>(&Pmerge_upper_left_reg_base->i_mac_empty_8_5), 0, {0x4}, sizeof(uint32_t) } },
    { "i_pre_wr_addr", { const_cast<uint32_t(*)>(&Pmerge_upper_left_reg_base->i_pre_wr_addr), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_lower_left_pps_reg_i_pkt_ctr_map: public RegisterMapper {
  static constexpr PTR_Pmerge_lower_left_pps_reg_i_pkt_ctr Pmerge_lower_left_pps_reg_i_pkt_ctr_base=0;
  Pmerge_lower_left_pps_reg_i_pkt_ctr_map() : RegisterMapper( {
    { "i_pkt_ctr_0_2", { const_cast<uint32_t(*)>(&Pmerge_lower_left_pps_reg_i_pkt_ctr_base->i_pkt_ctr_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_pkt_ctr_1_2", { const_cast<uint32_t(*)>(&Pmerge_lower_left_pps_reg_i_pkt_ctr_base->i_pkt_ctr_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_lower_left_pps_reg_i_ctr_time_map: public RegisterMapper {
  static constexpr PTR_Pmerge_lower_left_pps_reg_i_ctr_time Pmerge_lower_left_pps_reg_i_ctr_time_base=0;
  Pmerge_lower_left_pps_reg_i_ctr_time_map() : RegisterMapper( {
    { "i_ctr_time_0_2", { const_cast<uint32_t(*)>(&Pmerge_lower_left_pps_reg_i_ctr_time_base->i_ctr_time_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_ctr_time_1_2", { const_cast<uint32_t(*)>(&Pmerge_lower_left_pps_reg_i_ctr_time_base->i_ctr_time_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_lower_left_pps_reg_map: public RegisterMapper {
  static constexpr PTR_Pmerge_lower_left_pps_reg Pmerge_lower_left_pps_reg_base=0;
  Pmerge_lower_left_pps_reg_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Pmerge_lower_left_pps_reg_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "i_start_table", { const_cast<uint32_t(*)[0x48]>(&Pmerge_lower_left_pps_reg_base->i_start_table), 0, {0x48}, sizeof(uint32_t) } },
    { "i_pkt_ctr", { const_cast<Pmerge_lower_left_pps_reg_i_pkt_ctr(*)[0x48]>(&Pmerge_lower_left_pps_reg_base->i_pkt_ctr), new Pmerge_lower_left_pps_reg_i_pkt_ctr_map, {0x48}, sizeof(Pmerge_lower_left_pps_reg_i_pkt_ctr) } },
    { "i_ctr_time", { const_cast<Pmerge_lower_left_pps_reg_i_ctr_time(*)>(&Pmerge_lower_left_pps_reg_base->i_ctr_time), new Pmerge_lower_left_pps_reg_i_ctr_time_map, {}, sizeof(Pmerge_lower_left_pps_reg_i_ctr_time) } },
    { "i_ctr_sample", { const_cast<uint32_t(*)>(&Pmerge_lower_left_pps_reg_base->i_ctr_sample), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_lower_right_reg_intr_map: public RegisterMapper {
  static constexpr PTR_Pmerge_lower_right_reg_intr Pmerge_lower_right_reg_intr_base=0;
  Pmerge_lower_right_reg_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Pmerge_lower_right_reg_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Pmerge_lower_right_reg_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Pmerge_lower_right_reg_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Pmerge_lower_right_reg_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Pmerge_lower_right_reg_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_lower_right_reg_map: public RegisterMapper {
  static constexpr PTR_Pmerge_lower_right_reg Pmerge_lower_right_reg_base=0;
  Pmerge_lower_right_reg_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Pmerge_lower_right_reg_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "port_rate_cfg_4_0", { const_cast<uint32_t(*)[0x5]>(&Pmerge_lower_right_reg_base->port_rate_cfg_4_0), 0, {0x5}, sizeof(uint32_t) } },
    { "e_mac_empty_4_0", { const_cast<uint32_t(*)[0x5]>(&Pmerge_lower_right_reg_base->e_mac_empty_4_0), 0, {0x5}, sizeof(uint32_t) } },
    { "intr", { &Pmerge_lower_right_reg_base->intr, new Pmerge_lower_right_reg_intr_map, {}, sizeof(Pmerge_lower_right_reg_intr) } },
    { "e_occ_oflow_err_log", { const_cast<uint32_t(*)>(&Pmerge_lower_right_reg_base->e_occ_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_slot_ou_flow_err_log", { const_cast<uint32_t(*)>(&Pmerge_lower_right_reg_base->e_slot_ou_flow_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_pre_wr_addr", { const_cast<uint32_t(*)>(&Pmerge_lower_right_reg_base->e_pre_wr_addr), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_upper_right_reg_phv_owner_255_128_map: public RegisterMapper {
  static constexpr PTR_Pmerge_upper_right_reg_phv_owner_255_128 Pmerge_upper_right_reg_phv_owner_255_128_base=0;
  Pmerge_upper_right_reg_phv_owner_255_128_map() : RegisterMapper( {
    { "phv_owner_255_128_0_4", { const_cast<uint32_t(*)>(&Pmerge_upper_right_reg_phv_owner_255_128_base->phv_owner_255_128_0_4), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_255_128_1_4", { const_cast<uint32_t(*)>(&Pmerge_upper_right_reg_phv_owner_255_128_base->phv_owner_255_128_1_4), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_255_128_2_4", { const_cast<uint32_t(*)>(&Pmerge_upper_right_reg_phv_owner_255_128_base->phv_owner_255_128_2_4), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_255_128_3_4", { const_cast<uint32_t(*)>(&Pmerge_upper_right_reg_phv_owner_255_128_base->phv_owner_255_128_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_upper_right_reg_phv_clr_on_wr_255_128_map: public RegisterMapper {
  static constexpr PTR_Pmerge_upper_right_reg_phv_clr_on_wr_255_128 Pmerge_upper_right_reg_phv_clr_on_wr_255_128_base=0;
  Pmerge_upper_right_reg_phv_clr_on_wr_255_128_map() : RegisterMapper( {
    { "phv_clr_on_wr_255_128_0_4", { const_cast<uint32_t(*)>(&Pmerge_upper_right_reg_phv_clr_on_wr_255_128_base->phv_clr_on_wr_255_128_0_4), 0, {}, sizeof(uint32_t) } },
    { "phv_clr_on_wr_255_128_1_4", { const_cast<uint32_t(*)>(&Pmerge_upper_right_reg_phv_clr_on_wr_255_128_base->phv_clr_on_wr_255_128_1_4), 0, {}, sizeof(uint32_t) } },
    { "phv_clr_on_wr_255_128_2_4", { const_cast<uint32_t(*)>(&Pmerge_upper_right_reg_phv_clr_on_wr_255_128_base->phv_clr_on_wr_255_128_2_4), 0, {}, sizeof(uint32_t) } },
    { "phv_clr_on_wr_255_128_3_4", { const_cast<uint32_t(*)>(&Pmerge_upper_right_reg_phv_clr_on_wr_255_128_base->phv_clr_on_wr_255_128_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_upper_right_reg_map: public RegisterMapper {
  static constexpr PTR_Pmerge_upper_right_reg Pmerge_upper_right_reg_base=0;
  Pmerge_upper_right_reg_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Pmerge_upper_right_reg_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "port_rate_cfg_8_5", { const_cast<uint32_t(*)[0x4]>(&Pmerge_upper_right_reg_base->port_rate_cfg_8_5), 0, {0x4}, sizeof(uint32_t) } },
    { "phv_owner_255_128", { const_cast<Pmerge_upper_right_reg_phv_owner_255_128(*)>(&Pmerge_upper_right_reg_base->phv_owner_255_128), new Pmerge_upper_right_reg_phv_owner_255_128_map, {}, sizeof(Pmerge_upper_right_reg_phv_owner_255_128) } },
    { "phv_clr_on_wr_255_128", { const_cast<Pmerge_upper_right_reg_phv_clr_on_wr_255_128(*)>(&Pmerge_upper_right_reg_base->phv_clr_on_wr_255_128), new Pmerge_upper_right_reg_phv_clr_on_wr_255_128_map, {}, sizeof(Pmerge_upper_right_reg_phv_clr_on_wr_255_128) } },
    { "e_mac_empty_8_5", { const_cast<uint32_t(*)[0x4]>(&Pmerge_upper_right_reg_base->e_mac_empty_8_5), 0, {0x4}, sizeof(uint32_t) } },
    { "e_pre_wr_addr", { const_cast<uint32_t(*)>(&Pmerge_upper_right_reg_base->e_pre_wr_addr), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_lower_right_pps_reg_intr_map: public RegisterMapper {
  static constexpr PTR_Pmerge_lower_right_pps_reg_intr Pmerge_lower_right_pps_reg_intr_base=0;
  Pmerge_lower_right_pps_reg_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Pmerge_lower_right_pps_reg_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Pmerge_lower_right_pps_reg_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Pmerge_lower_right_pps_reg_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Pmerge_lower_right_pps_reg_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Pmerge_lower_right_pps_reg_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_lower_right_pps_reg_e_pkt_ctr_map: public RegisterMapper {
  static constexpr PTR_Pmerge_lower_right_pps_reg_e_pkt_ctr Pmerge_lower_right_pps_reg_e_pkt_ctr_base=0;
  Pmerge_lower_right_pps_reg_e_pkt_ctr_map() : RegisterMapper( {
    { "e_pkt_ctr_0_2", { const_cast<uint32_t(*)>(&Pmerge_lower_right_pps_reg_e_pkt_ctr_base->e_pkt_ctr_0_2), 0, {}, sizeof(uint32_t) } },
    { "e_pkt_ctr_1_2", { const_cast<uint32_t(*)>(&Pmerge_lower_right_pps_reg_e_pkt_ctr_base->e_pkt_ctr_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_lower_right_pps_reg_e_ctr_time_map: public RegisterMapper {
  static constexpr PTR_Pmerge_lower_right_pps_reg_e_ctr_time Pmerge_lower_right_pps_reg_e_ctr_time_base=0;
  Pmerge_lower_right_pps_reg_e_ctr_time_map() : RegisterMapper( {
    { "e_ctr_time_0_2", { const_cast<uint32_t(*)>(&Pmerge_lower_right_pps_reg_e_ctr_time_base->e_ctr_time_0_2), 0, {}, sizeof(uint32_t) } },
    { "e_ctr_time_1_2", { const_cast<uint32_t(*)>(&Pmerge_lower_right_pps_reg_e_ctr_time_base->e_ctr_time_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_lower_right_pps_reg_map: public RegisterMapper {
  static constexpr PTR_Pmerge_lower_right_pps_reg Pmerge_lower_right_pps_reg_base=0;
  Pmerge_lower_right_pps_reg_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Pmerge_lower_right_pps_reg_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "tm_status_phv", { const_cast<uint32_t(*)>(&Pmerge_lower_right_pps_reg_base->tm_status_phv), 0, {}, sizeof(uint32_t) } },
    { "e_start_table", { const_cast<uint32_t(*)[0x48]>(&Pmerge_lower_right_pps_reg_base->e_start_table), 0, {0x48}, sizeof(uint32_t) } },
    { "g_start_table", { const_cast<uint32_t(*)>(&Pmerge_lower_right_pps_reg_base->g_start_table), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Pmerge_lower_right_pps_reg_base->intr, new Pmerge_lower_right_pps_reg_intr_map, {}, sizeof(Pmerge_lower_right_pps_reg_intr) } },
    { "pipe_map", { const_cast<uint32_t(*)>(&Pmerge_lower_right_pps_reg_base->pipe_map), 0, {}, sizeof(uint32_t) } },
    { "e_pkt_ctr", { const_cast<Pmerge_lower_right_pps_reg_e_pkt_ctr(*)[0x48]>(&Pmerge_lower_right_pps_reg_base->e_pkt_ctr), new Pmerge_lower_right_pps_reg_e_pkt_ctr_map, {0x48}, sizeof(Pmerge_lower_right_pps_reg_e_pkt_ctr) } },
    { "e_ctr_time", { const_cast<Pmerge_lower_right_pps_reg_e_ctr_time(*)>(&Pmerge_lower_right_pps_reg_base->e_ctr_time), new Pmerge_lower_right_pps_reg_e_ctr_time_map, {}, sizeof(Pmerge_lower_right_pps_reg_e_ctr_time) } },
    { "e_ctr_sample", { const_cast<uint32_t(*)>(&Pmerge_lower_right_pps_reg_base->e_ctr_sample), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pmerge_reg_map: public RegisterMapper {
  static constexpr PTR_Pmerge_reg Pmerge_reg_base=0;
  Pmerge_reg_map() : RegisterMapper( {
    { "ll0", { &Pmerge_reg_base->ll0, new Pmerge_lower_left_reg_map, {}, sizeof(Pmerge_lower_left_reg) } },
    { "ul", { &Pmerge_reg_base->ul, new Pmerge_upper_left_reg_map, {}, sizeof(Pmerge_upper_left_reg) } },
    { "ll1", { &Pmerge_reg_base->ll1, new Pmerge_lower_left_pps_reg_map, {}, sizeof(Pmerge_lower_left_pps_reg) } },
    { "lr0", { &Pmerge_reg_base->lr0, new Pmerge_lower_right_reg_map, {}, sizeof(Pmerge_lower_right_reg) } },
    { "ur", { &Pmerge_reg_base->ur, new Pmerge_upper_right_reg_map, {}, sizeof(Pmerge_upper_right_reg) } },
    { "lr1", { &Pmerge_reg_base->lr1, new Pmerge_lower_right_pps_reg_map, {}, sizeof(Pmerge_lower_right_pps_reg) } }
    } )
  {}
};

struct Parb_bubble_count_map: public RegisterMapper {
  static constexpr PTR_Parb_bubble_count Parb_bubble_count_base=0;
  Parb_bubble_count_map() : RegisterMapper( {
    { "i_bubble_count_0_2", { const_cast<uint32_t(*)>(&Parb_bubble_count_base->i_bubble_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_bubble_count_1_2", { const_cast<uint32_t(*)>(&Parb_bubble_count_base->i_bubble_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_phv_count_map: public RegisterMapper {
  static constexpr PTR_Parb_phv_count Parb_phv_count_base=0;
  Parb_phv_count_map() : RegisterMapper( {
    { "i_phv_count_0_2", { const_cast<uint32_t(*)>(&Parb_phv_count_base->i_phv_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_phv_count_1_2", { const_cast<uint32_t(*)>(&Parb_phv_count_base->i_phv_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_left_reg_intr_map: public RegisterMapper {
  static constexpr PTR_Parb_left_reg_intr Parb_left_reg_intr_base=0;
  Parb_left_reg_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Parb_left_reg_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Parb_left_reg_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Parb_left_reg_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Parb_left_reg_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Parb_left_reg_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_left_reg_map: public RegisterMapper {
  static constexpr PTR_Parb_left_reg Parb_left_reg_base=0;
  Parb_left_reg_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Parb_left_reg_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "port_rate_cfg", { const_cast<uint32_t(*)[0x9]>(&Parb_left_reg_base->port_rate_cfg), 0, {0x9}, sizeof(uint32_t) } },
    { "i_phv_rate_ctrl", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_phv_rate_ctrl), 0, {}, sizeof(uint32_t) } },
    { "i_chnl_ctrl", { const_cast<uint32_t(*)[0x48]>(&Parb_left_reg_base->i_chnl_ctrl), 0, {0x48}, sizeof(uint32_t) } },
    { "i_dprsr_cred_status", { const_cast<uint32_t(*)[0x48]>(&Parb_left_reg_base->i_dprsr_cred_status), 0, {0x48}, sizeof(uint32_t) } },
    { "i_bubble_count", { const_cast<Parb_bubble_count(*)>(&Parb_left_reg_base->i_bubble_count), new Parb_bubble_count_map, {}, sizeof(Parb_bubble_count) } },
    { "i_phv_count", { const_cast<Parb_phv_count(*)>(&Parb_left_reg_base->i_phv_count), new Parb_phv_count_map, {}, sizeof(Parb_phv_count) } },
    { "i_eop_count", { const_cast<Parb_phv_count(*)>(&Parb_left_reg_base->i_eop_count), new Parb_phv_count_map, {}, sizeof(Parb_phv_count) } },
    { "i_norm_phv_count", { const_cast<Parb_phv_count(*)>(&Parb_left_reg_base->i_norm_phv_count), new Parb_phv_count_map, {}, sizeof(Parb_phv_count) } },
    { "i_norm_eop_count", { const_cast<Parb_phv_count(*)>(&Parb_left_reg_base->i_norm_eop_count), new Parb_phv_count_map, {}, sizeof(Parb_phv_count) } },
    { "i_resub_phv_count", { const_cast<Parb_phv_count(*)>(&Parb_left_reg_base->i_resub_phv_count), new Parb_phv_count_map, {}, sizeof(Parb_phv_count) } },
    { "i_resub_eop_count", { const_cast<Parb_phv_count(*)>(&Parb_left_reg_base->i_resub_eop_count), new Parb_phv_count_map, {}, sizeof(Parb_phv_count) } },
    { "i_port_dbg", { const_cast<uint32_t(*)[0x12]>(&Parb_left_reg_base->i_port_dbg), 0, {0x12}, sizeof(uint32_t) } },
    { "i_tdm_last_entry", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_tdm_last_entry), 0, {}, sizeof(uint32_t) } },
    { "i_tdm_table", { const_cast<uint32_t(*)[0x40]>(&Parb_left_reg_base->i_tdm_table), 0, {0x40}, sizeof(uint32_t) } },
    { "i_slot_ctrl", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_slot_ctrl), 0, {}, sizeof(uint32_t) } },
    { "i_slot_status", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_slot_status), 0, {}, sizeof(uint32_t) } },
    { "i_pri_inc_ctrl_400g", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_pri_inc_ctrl_400g), 0, {}, sizeof(uint32_t) } },
    { "i_pri_inc_ctrl_200g", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_pri_inc_ctrl_200g), 0, {}, sizeof(uint32_t) } },
    { "i_pri_inc_ctrl_100g", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_pri_inc_ctrl_100g), 0, {}, sizeof(uint32_t) } },
    { "i_pri_inc_ctrl_50g", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_pri_inc_ctrl_50g), 0, {}, sizeof(uint32_t) } },
    { "i_pri_inc_ctrl_40g", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_pri_inc_ctrl_40g), 0, {}, sizeof(uint32_t) } },
    { "i_pri_inc_ctrl_25g", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_pri_inc_ctrl_25g), 0, {}, sizeof(uint32_t) } },
    { "i_pri_inc_ctrl_10g", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_pri_inc_ctrl_10g), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Parb_left_reg_base->intr, new Parb_left_reg_intr_map, {}, sizeof(Parb_left_reg_intr) } },
    { "i_avail_oflow_err_log", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_avail_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_bcnt_oflow_err_log", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_bcnt_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_afifo_oflow_err_log", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_afifo_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_arb_cred_uflow_err_log", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_arb_cred_uflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_arb_cred_oflow_err_log", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_arb_cred_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_port_dbg_en", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_port_dbg_en), 0, {}, sizeof(uint32_t) } },
    { "i_wb_ctrl", { const_cast<uint32_t(*)>(&Parb_left_reg_base->i_wb_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_right_reg_intr_map: public RegisterMapper {
  static constexpr PTR_Parb_right_reg_intr Parb_right_reg_intr_base=0;
  Parb_right_reg_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Parb_right_reg_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Parb_right_reg_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Parb_right_reg_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Parb_right_reg_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Parb_right_reg_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_right_reg_map: public RegisterMapper {
  static constexpr PTR_Parb_right_reg Parb_right_reg_base=0;
  Parb_right_reg_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Parb_right_reg_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "port_rate_cfg", { const_cast<uint32_t(*)[0x9]>(&Parb_right_reg_base->port_rate_cfg), 0, {0x9}, sizeof(uint32_t) } },
    { "e_phv_rate_ctrl", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_phv_rate_ctrl), 0, {}, sizeof(uint32_t) } },
    { "g_phv_rate_ctrl", { const_cast<uint32_t(*)>(&Parb_right_reg_base->g_phv_rate_ctrl), 0, {}, sizeof(uint32_t) } },
    { "gbl_rate_ctrl", { const_cast<uint32_t(*)>(&Parb_right_reg_base->gbl_rate_ctrl), 0, {}, sizeof(uint32_t) } },
    { "e_chnl_ctrl", { const_cast<uint32_t(*)[0x48]>(&Parb_right_reg_base->e_chnl_ctrl), 0, {0x48}, sizeof(uint32_t) } },
    { "e_dprsr_cred_status", { const_cast<uint32_t(*)[0x48]>(&Parb_right_reg_base->e_dprsr_cred_status), 0, {0x48}, sizeof(uint32_t) } },
    { "e_bubble_count", { const_cast<Parb_bubble_count(*)>(&Parb_right_reg_base->e_bubble_count), new Parb_bubble_count_map, {}, sizeof(Parb_bubble_count) } },
    { "e_phv_count", { const_cast<Parb_phv_count(*)>(&Parb_right_reg_base->e_phv_count), new Parb_phv_count_map, {}, sizeof(Parb_phv_count) } },
    { "e_eop_count", { const_cast<Parb_phv_count(*)>(&Parb_right_reg_base->e_eop_count), new Parb_phv_count_map, {}, sizeof(Parb_phv_count) } },
    { "e_port_dbg", { const_cast<uint32_t(*)[0x12]>(&Parb_right_reg_base->e_port_dbg), 0, {0x12}, sizeof(uint32_t) } },
    { "e_tdm_last_entry", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_tdm_last_entry), 0, {}, sizeof(uint32_t) } },
    { "e_tdm_table", { const_cast<uint32_t(*)[0x40]>(&Parb_right_reg_base->e_tdm_table), 0, {0x40}, sizeof(uint32_t) } },
    { "e_slot_ctrl", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_slot_ctrl), 0, {}, sizeof(uint32_t) } },
    { "e_slot_status", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_slot_status), 0, {}, sizeof(uint32_t) } },
    { "e_pri_inc_ctrl_400g", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_pri_inc_ctrl_400g), 0, {}, sizeof(uint32_t) } },
    { "e_pri_inc_ctrl_200g", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_pri_inc_ctrl_200g), 0, {}, sizeof(uint32_t) } },
    { "e_pri_inc_ctrl_100g", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_pri_inc_ctrl_100g), 0, {}, sizeof(uint32_t) } },
    { "e_pri_inc_ctrl_50g", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_pri_inc_ctrl_50g), 0, {}, sizeof(uint32_t) } },
    { "e_pri_inc_ctrl_40g", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_pri_inc_ctrl_40g), 0, {}, sizeof(uint32_t) } },
    { "e_pri_inc_ctrl_25g", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_pri_inc_ctrl_25g), 0, {}, sizeof(uint32_t) } },
    { "e_pri_inc_ctrl_10g", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_pri_inc_ctrl_10g), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Parb_right_reg_base->intr, new Parb_right_reg_intr_map, {}, sizeof(Parb_right_reg_intr) } },
    { "e_avail_oflow_err_log", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_avail_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_bcnt_oflow_err_log", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_bcnt_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_afifo_oflow_err_log", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_afifo_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_arb_cred_uflow_err_log", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_arb_cred_uflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_arb_cred_oflow_err_log", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_arb_cred_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "debug_ctrl", { const_cast<uint32_t(*)>(&Parb_right_reg_base->debug_ctrl), 0, {}, sizeof(uint32_t) } },
    { "e_port_dbg_en", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_port_dbg_en), 0, {}, sizeof(uint32_t) } },
    { "e_wb_ctrl", { const_cast<uint32_t(*)>(&Parb_right_reg_base->e_wb_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_reg_map: public RegisterMapper {
  static constexpr PTR_Parb_reg Parb_reg_base=0;
  Parb_reg_map() : RegisterMapper( {
    { "left", { &Parb_reg_base->left, new Parb_left_reg_map, {}, sizeof(Parb_left_reg) } },
    { "right", { &Parb_reg_base->right, new Parb_right_reg_map, {}, sizeof(Parb_right_reg) } }
    } )
  {}
};

struct S2p_reg_pkt_ctr_map: public RegisterMapper {
  static constexpr PTR_S2p_reg_pkt_ctr S2p_reg_pkt_ctr_base=0;
  S2p_reg_pkt_ctr_map() : RegisterMapper( {
    { "pkt_ctr_0_2", { const_cast<uint32_t(*)>(&S2p_reg_pkt_ctr_base->pkt_ctr_0_2), 0, {}, sizeof(uint32_t) } },
    { "pkt_ctr_1_2", { const_cast<uint32_t(*)>(&S2p_reg_pkt_ctr_base->pkt_ctr_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct S2p_reg_byte_ctr_map: public RegisterMapper {
  static constexpr PTR_S2p_reg_byte_ctr S2p_reg_byte_ctr_base=0;
  S2p_reg_byte_ctr_map() : RegisterMapper( {
    { "byte_ctr_0_2", { const_cast<uint32_t(*)>(&S2p_reg_byte_ctr_base->byte_ctr_0_2), 0, {}, sizeof(uint32_t) } },
    { "byte_ctr_1_2", { const_cast<uint32_t(*)>(&S2p_reg_byte_ctr_base->byte_ctr_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct S2p_reg_ctr_time_map: public RegisterMapper {
  static constexpr PTR_S2p_reg_ctr_time S2p_reg_ctr_time_base=0;
  S2p_reg_ctr_time_map() : RegisterMapper( {
    { "ctr_time_0_2", { const_cast<uint32_t(*)>(&S2p_reg_ctr_time_base->ctr_time_0_2), 0, {}, sizeof(uint32_t) } },
    { "ctr_time_1_2", { const_cast<uint32_t(*)>(&S2p_reg_ctr_time_base->ctr_time_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct S2p_reg_ecc_map: public RegisterMapper {
  static constexpr PTR_S2p_reg_ecc S2p_reg_ecc_base=0;
  S2p_reg_ecc_map() : RegisterMapper( {
    { "ecc_0_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_0_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_1_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_1_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_2_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_2_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_3_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_3_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_4_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_4_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_5_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_5_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_6_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_6_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_7_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_7_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_8_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_8_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_9_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_9_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_10_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_10_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_11_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_11_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_12_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_12_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_13_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_13_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_14_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_14_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_15_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_15_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_16_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_16_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_17_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_17_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_18_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_18_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_19_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_19_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_20_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_20_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_21_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_21_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_22_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_22_24), 0, {}, sizeof(uint32_t) } },
    { "ecc_23_24", { const_cast<uint32_t(*)>(&S2p_reg_ecc_base->ecc_23_24), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct S2p_reg_intr_map: public RegisterMapper {
  static constexpr PTR_S2p_reg_intr S2p_reg_intr_base=0;
  S2p_reg_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&S2p_reg_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&S2p_reg_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&S2p_reg_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&S2p_reg_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&S2p_reg_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct S2p_reg_map: public RegisterMapper {
  static constexpr PTR_S2p_reg S2p_reg_base=0;
  S2p_reg_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&S2p_reg_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&S2p_reg_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "port_rate_cfg", { const_cast<uint32_t(*)[0x9]>(&S2p_reg_base->port_rate_cfg), 0, {0x9}, sizeof(uint32_t) } },
    { "chan_en", { const_cast<uint32_t(*)[0x9]>(&S2p_reg_base->chan_en), 0, {0x9}, sizeof(uint32_t) } },
    { "copy2cpu", { const_cast<uint32_t(*)>(&S2p_reg_base->copy2cpu), 0, {}, sizeof(uint32_t) } },
    { "tm_cred_wr", { const_cast<uint32_t(*)[0x4]>(&S2p_reg_base->tm_cred_wr), 0, {0x4}, sizeof(uint32_t) } },
    { "tm_cred_rd", { const_cast<uint32_t(*)>(&S2p_reg_base->tm_cred_rd), 0, {}, sizeof(uint32_t) } },
    { "pkt_ctr", { const_cast<S2p_reg_pkt_ctr(*)[0x48]>(&S2p_reg_base->pkt_ctr), new S2p_reg_pkt_ctr_map, {0x48}, sizeof(S2p_reg_pkt_ctr) } },
    { "byte_ctr", { const_cast<S2p_reg_byte_ctr(*)[0x48]>(&S2p_reg_base->byte_ctr), new S2p_reg_byte_ctr_map, {0x48}, sizeof(S2p_reg_byte_ctr) } },
    { "ctr_time", { const_cast<S2p_reg_ctr_time(*)>(&S2p_reg_base->ctr_time), new S2p_reg_ctr_time_map, {}, sizeof(S2p_reg_ctr_time) } },
    { "ctr_sample", { const_cast<uint32_t(*)>(&S2p_reg_base->ctr_sample), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<S2p_reg_ecc(*)>(&S2p_reg_base->ecc), new S2p_reg_ecc_map, {}, sizeof(S2p_reg_ecc) } },
    { "intr", { &S2p_reg_base->intr, new S2p_reg_intr_map, {}, sizeof(S2p_reg_intr) } },
    { "sb_0_sbe_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->sb_0_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_0_mbe_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->sb_0_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_1_sbe_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->sb_1_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_1_mbe_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->sb_1_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_2_sbe_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->sb_2_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_2_mbe_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->sb_2_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_3_sbe_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->sb_3_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_3_mbe_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->sb_3_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_oflow_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->sb_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_uflow_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->sb_uflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "mirr_sbe_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->mirr_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "mirr_mbe_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->mirr_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "byte_adj_sbe_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->byte_adj_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "byte_adj_mbe_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->byte_adj_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "tm_cred_oflow_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->tm_cred_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "tm_cred_uflow_err_log", { const_cast<uint32_t(*)>(&S2p_reg_base->tm_cred_uflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "debug_ctrl", { const_cast<uint32_t(*)>(&S2p_reg_base->debug_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pipe_map", { const_cast<uint32_t(*)>(&S2p_reg_base->pipe_map), 0, {}, sizeof(uint32_t) } },
    { "hold_es_400g", { const_cast<uint32_t(*)>(&S2p_reg_base->hold_es_400g), 0, {}, sizeof(uint32_t) } },
    { "hold_es_200g", { const_cast<uint32_t(*)>(&S2p_reg_base->hold_es_200g), 0, {}, sizeof(uint32_t) } },
    { "hold_es_100g", { const_cast<uint32_t(*)>(&S2p_reg_base->hold_es_100g), 0, {}, sizeof(uint32_t) } },
    { "hold_es_50g", { const_cast<uint32_t(*)>(&S2p_reg_base->hold_es_50g), 0, {}, sizeof(uint32_t) } },
    { "hold_es_40g", { const_cast<uint32_t(*)>(&S2p_reg_base->hold_es_40g), 0, {}, sizeof(uint32_t) } },
    { "hold_es_25g", { const_cast<uint32_t(*)>(&S2p_reg_base->hold_es_25g), 0, {}, sizeof(uint32_t) } },
    { "hold_es_10g", { const_cast<uint32_t(*)>(&S2p_reg_base->hold_es_10g), 0, {}, sizeof(uint32_t) } },
    { "hold_sm_400g", { const_cast<uint32_t(*)>(&S2p_reg_base->hold_sm_400g), 0, {}, sizeof(uint32_t) } },
    { "hold_sm_200g", { const_cast<uint32_t(*)>(&S2p_reg_base->hold_sm_200g), 0, {}, sizeof(uint32_t) } },
    { "hold_sm_100g", { const_cast<uint32_t(*)>(&S2p_reg_base->hold_sm_100g), 0, {}, sizeof(uint32_t) } },
    { "hold_sm_50g", { const_cast<uint32_t(*)>(&S2p_reg_base->hold_sm_50g), 0, {}, sizeof(uint32_t) } },
    { "hold_sm_40g", { const_cast<uint32_t(*)>(&S2p_reg_base->hold_sm_40g), 0, {}, sizeof(uint32_t) } },
    { "hold_sm_25g", { const_cast<uint32_t(*)>(&S2p_reg_base->hold_sm_25g), 0, {}, sizeof(uint32_t) } },
    { "hold_sm_10g", { const_cast<uint32_t(*)>(&S2p_reg_base->hold_sm_10g), 0, {}, sizeof(uint32_t) } },
    { "tm_cred_low_wmark", { const_cast<uint32_t(*)>(&S2p_reg_base->tm_cred_low_wmark), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct P2s_reg_pkt_ctr_map: public RegisterMapper {
  static constexpr PTR_P2s_reg_pkt_ctr P2s_reg_pkt_ctr_base=0;
  P2s_reg_pkt_ctr_map() : RegisterMapper( {
    { "pkt_ctr_0_2", { const_cast<uint32_t(*)>(&P2s_reg_pkt_ctr_base->pkt_ctr_0_2), 0, {}, sizeof(uint32_t) } },
    { "pkt_ctr_1_2", { const_cast<uint32_t(*)>(&P2s_reg_pkt_ctr_base->pkt_ctr_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct P2s_reg_byte_ctr_map: public RegisterMapper {
  static constexpr PTR_P2s_reg_byte_ctr P2s_reg_byte_ctr_base=0;
  P2s_reg_byte_ctr_map() : RegisterMapper( {
    { "byte_ctr_0_2", { const_cast<uint32_t(*)>(&P2s_reg_byte_ctr_base->byte_ctr_0_2), 0, {}, sizeof(uint32_t) } },
    { "byte_ctr_1_2", { const_cast<uint32_t(*)>(&P2s_reg_byte_ctr_base->byte_ctr_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct P2s_reg_ctr_time_map: public RegisterMapper {
  static constexpr PTR_P2s_reg_ctr_time P2s_reg_ctr_time_base=0;
  P2s_reg_ctr_time_map() : RegisterMapper( {
    { "ctr_time_0_2", { const_cast<uint32_t(*)>(&P2s_reg_ctr_time_base->ctr_time_0_2), 0, {}, sizeof(uint32_t) } },
    { "ctr_time_1_2", { const_cast<uint32_t(*)>(&P2s_reg_ctr_time_base->ctr_time_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct P2s_reg_ecc_map: public RegisterMapper {
  static constexpr PTR_P2s_reg_ecc P2s_reg_ecc_base=0;
  P2s_reg_ecc_map() : RegisterMapper( {
    { "ecc_0_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_0_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_1_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_1_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_2_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_2_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_3_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_3_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_4_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_4_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_5_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_5_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_6_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_6_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_7_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_7_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_8_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_8_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_9_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_9_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_10_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_10_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_11_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_11_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_12_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_12_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_13_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_13_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_14_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_14_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_15_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_15_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_16_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_16_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_17_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_17_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_18_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_18_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_19_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_19_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_20_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_20_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_21_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_21_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_22_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_22_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_23_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_23_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_24_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_24_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_25_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_25_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_26_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_26_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_27_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_27_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_28_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_28_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_29_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_29_31), 0, {}, sizeof(uint32_t) } },
    { "ecc_30_31", { const_cast<uint32_t(*)>(&P2s_reg_ecc_base->ecc_30_31), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct P2s_reg_intr_map: public RegisterMapper {
  static constexpr PTR_P2s_reg_intr P2s_reg_intr_base=0;
  P2s_reg_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&P2s_reg_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&P2s_reg_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&P2s_reg_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&P2s_reg_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&P2s_reg_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct P2s_reg_map: public RegisterMapper {
  static constexpr PTR_P2s_reg P2s_reg_base=0;
  P2s_reg_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&P2s_reg_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&P2s_reg_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "port_rate_cfg", { const_cast<uint32_t(*)[0x9]>(&P2s_reg_base->port_rate_cfg), 0, {0x9}, sizeof(uint32_t) } },
    { "epb_cred_wr", { const_cast<uint32_t(*)[0x12]>(&P2s_reg_base->epb_cred_wr), 0, {0x12}, sizeof(uint32_t) } },
    { "epb_cred_rd", { const_cast<uint32_t(*)[0x12]>(&P2s_reg_base->epb_cred_rd), 0, {0x12}, sizeof(uint32_t) } },
    { "pkt_ctr", { const_cast<P2s_reg_pkt_ctr(*)[0x48]>(&P2s_reg_base->pkt_ctr), new P2s_reg_pkt_ctr_map, {0x48}, sizeof(P2s_reg_pkt_ctr) } },
    { "byte_ctr", { const_cast<P2s_reg_byte_ctr(*)[0x48]>(&P2s_reg_base->byte_ctr), new P2s_reg_byte_ctr_map, {0x48}, sizeof(P2s_reg_byte_ctr) } },
    { "ctr_time", { const_cast<P2s_reg_ctr_time(*)>(&P2s_reg_base->ctr_time), new P2s_reg_ctr_time_map, {}, sizeof(P2s_reg_ctr_time) } },
    { "ctr_sample", { const_cast<uint32_t(*)>(&P2s_reg_base->ctr_sample), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<P2s_reg_ecc(*)>(&P2s_reg_base->ecc), new P2s_reg_ecc_map, {}, sizeof(P2s_reg_ecc) } },
    { "intr", { &P2s_reg_base->intr, new P2s_reg_intr_map, {}, sizeof(P2s_reg_intr) } },
    { "tm_sbe_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->tm_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "tm_mbe_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->tm_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_0_sbe_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->sb_0_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_0_mbe_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->sb_0_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_1_sbe_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->sb_1_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_1_mbe_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->sb_1_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_2_sbe_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->sb_2_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_2_mbe_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->sb_2_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_3_sbe_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->sb_3_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_3_mbe_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->sb_3_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_4_sbe_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->sb_4_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_4_mbe_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->sb_4_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_oflow_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->sb_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "sb_uflow_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->sb_uflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "epb_cred_oflow_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->epb_cred_oflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "epb_cred_uflow_err_log", { const_cast<uint32_t(*)>(&P2s_reg_base->epb_cred_uflow_err_log), 0, {}, sizeof(uint32_t) } },
    { "pipe_map", { const_cast<uint32_t(*)>(&P2s_reg_base->pipe_map), 0, {}, sizeof(uint32_t) } },
    { "tm_cred", { const_cast<uint32_t(*)[0x12]>(&P2s_reg_base->tm_cred), 0, {0x12}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parde_glue_reg_rspec_csr_ring0intr_map: public RegisterMapper {
  static constexpr PTR_Parde_glue_reg_rspec_csr_ring0intr Parde_glue_reg_rspec_csr_ring0intr_base=0;
  Parde_glue_reg_rspec_csr_ring0intr_map() : RegisterMapper( {
    { "status0", { const_cast<uint32_t(*)>(&Parde_glue_reg_rspec_csr_ring0intr_base->status0), 0, {}, sizeof(uint32_t) } },
    { "status1", { const_cast<uint32_t(*)>(&Parde_glue_reg_rspec_csr_ring0intr_base->status1), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parde_glue_reg_rspec_csr_ring1intr_map: public RegisterMapper {
  static constexpr PTR_Parde_glue_reg_rspec_csr_ring1intr Parde_glue_reg_rspec_csr_ring1intr_base=0;
  Parde_glue_reg_rspec_csr_ring1intr_map() : RegisterMapper( {
    { "status0", { const_cast<uint32_t(*)>(&Parde_glue_reg_rspec_csr_ring1intr_base->status0), 0, {}, sizeof(uint32_t) } },
    { "status1", { const_cast<uint32_t(*)>(&Parde_glue_reg_rspec_csr_ring1intr_base->status1), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parde_glue_reg_rspec_csr_ring2intr_map: public RegisterMapper {
  static constexpr PTR_Parde_glue_reg_rspec_csr_ring2intr Parde_glue_reg_rspec_csr_ring2intr_base=0;
  Parde_glue_reg_rspec_csr_ring2intr_map() : RegisterMapper( {
    { "status0", { const_cast<uint32_t(*)>(&Parde_glue_reg_rspec_csr_ring2intr_base->status0), 0, {}, sizeof(uint32_t) } },
    { "status1", { const_cast<uint32_t(*)>(&Parde_glue_reg_rspec_csr_ring2intr_base->status1), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parde_glue_reg_rspec_parde_intr_map: public RegisterMapper {
  static constexpr PTR_Parde_glue_reg_rspec_parde_intr Parde_glue_reg_rspec_parde_intr_base=0;
  Parde_glue_reg_rspec_parde_intr_map() : RegisterMapper( {
    { "status0", { const_cast<uint32_t(*)>(&Parde_glue_reg_rspec_parde_intr_base->status0), 0, {}, sizeof(uint32_t) } },
    { "status1", { const_cast<uint32_t(*)>(&Parde_glue_reg_rspec_parde_intr_base->status1), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parde_glue_reg_rspec_map: public RegisterMapper {
  static constexpr PTR_Parde_glue_reg_rspec Parde_glue_reg_rspec_base=0;
  Parde_glue_reg_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Parde_glue_reg_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "debug_ctrl", { const_cast<uint32_t(*)>(&Parde_glue_reg_rspec_base->debug_ctrl), 0, {}, sizeof(uint32_t) } },
    { "csr_ring_full_thresh", { const_cast<uint32_t(*)>(&Parde_glue_reg_rspec_base->csr_ring_full_thresh), 0, {}, sizeof(uint32_t) } },
    { "csr_ring_fifo_err", { const_cast<uint32_t(*)>(&Parde_glue_reg_rspec_base->csr_ring_fifo_err), 0, {}, sizeof(uint32_t) } },
    { "csr_ring0intr", { &Parde_glue_reg_rspec_base->csr_ring0intr, new Parde_glue_reg_rspec_csr_ring0intr_map, {}, sizeof(Parde_glue_reg_rspec_csr_ring0intr) } },
    { "csr_ring1intr", { &Parde_glue_reg_rspec_base->csr_ring1intr, new Parde_glue_reg_rspec_csr_ring1intr_map, {}, sizeof(Parde_glue_reg_rspec_csr_ring1intr) } },
    { "csr_ring2intr", { &Parde_glue_reg_rspec_base->csr_ring2intr, new Parde_glue_reg_rspec_csr_ring2intr_map, {}, sizeof(Parde_glue_reg_rspec_csr_ring2intr) } },
    { "parde_intr", { &Parde_glue_reg_rspec_base->parde_intr, new Parde_glue_reg_rspec_parde_intr_map, {}, sizeof(Parde_glue_reg_rspec_parde_intr) } },
    { "intr_period", { const_cast<uint32_t(*)>(&Parde_glue_reg_rspec_base->intr_period), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_prsr_stat_map: public RegisterMapper {
  static constexpr PTR_Epb_prsr_stat Epb_prsr_stat_base=0;
  Epb_prsr_stat_map() : RegisterMapper( {
    { "glb_prsr_stat_0_2", { const_cast<uint32_t(*)>(&Epb_prsr_stat_base->glb_prsr_stat_0_2), 0, {}, sizeof(uint32_t) } },
    { "glb_prsr_stat_1_2", { const_cast<uint32_t(*)>(&Epb_prsr_stat_base->glb_prsr_stat_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_glb_group_intr_stat_stat_map: public RegisterMapper {
  static constexpr PTR_Epb_glb_group_intr_stat_stat Epb_glb_group_intr_stat_stat_base=0;
  Epb_glb_group_intr_stat_stat_map() : RegisterMapper( {
    { "stat_0_2", { const_cast<uint32_t(*)>(&Epb_glb_group_intr_stat_stat_base->stat_0_2), 0, {}, sizeof(uint32_t) } },
    { "stat_1_2", { const_cast<uint32_t(*)>(&Epb_glb_group_intr_stat_stat_base->stat_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_glb_group_intr_stat_en0_map: public RegisterMapper {
  static constexpr PTR_Epb_glb_group_intr_stat_en0 Epb_glb_group_intr_stat_en0_base=0;
  Epb_glb_group_intr_stat_en0_map() : RegisterMapper( {
    { "en0_0_2", { const_cast<uint32_t(*)>(&Epb_glb_group_intr_stat_en0_base->en0_0_2), 0, {}, sizeof(uint32_t) } },
    { "en0_1_2", { const_cast<uint32_t(*)>(&Epb_glb_group_intr_stat_en0_base->en0_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_glb_group_intr_stat_en1_map: public RegisterMapper {
  static constexpr PTR_Epb_glb_group_intr_stat_en1 Epb_glb_group_intr_stat_en1_base=0;
  Epb_glb_group_intr_stat_en1_map() : RegisterMapper( {
    { "en1_0_2", { const_cast<uint32_t(*)>(&Epb_glb_group_intr_stat_en1_base->en1_0_2), 0, {}, sizeof(uint32_t) } },
    { "en1_1_2", { const_cast<uint32_t(*)>(&Epb_glb_group_intr_stat_en1_base->en1_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_glb_group_intr_stat_inj_map: public RegisterMapper {
  static constexpr PTR_Epb_glb_group_intr_stat_inj Epb_glb_group_intr_stat_inj_base=0;
  Epb_glb_group_intr_stat_inj_map() : RegisterMapper( {
    { "inj_0_2", { const_cast<uint32_t(*)>(&Epb_glb_group_intr_stat_inj_base->inj_0_2), 0, {}, sizeof(uint32_t) } },
    { "inj_1_2", { const_cast<uint32_t(*)>(&Epb_glb_group_intr_stat_inj_base->inj_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_glb_group_intr_stat_freeze_enable_map: public RegisterMapper {
  static constexpr PTR_Epb_glb_group_intr_stat_freeze_enable Epb_glb_group_intr_stat_freeze_enable_base=0;
  Epb_glb_group_intr_stat_freeze_enable_map() : RegisterMapper( {
    { "freeze_enable_0_2", { const_cast<uint32_t(*)>(&Epb_glb_group_intr_stat_freeze_enable_base->freeze_enable_0_2), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable_1_2", { const_cast<uint32_t(*)>(&Epb_glb_group_intr_stat_freeze_enable_base->freeze_enable_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_glb_group_intr_stat_map: public RegisterMapper {
  static constexpr PTR_Epb_glb_group_intr_stat Epb_glb_group_intr_stat_base=0;
  Epb_glb_group_intr_stat_map() : RegisterMapper( {
    { "stat", { const_cast<Epb_glb_group_intr_stat_stat(*)>(&Epb_glb_group_intr_stat_base->stat), new Epb_glb_group_intr_stat_stat_map, {}, sizeof(Epb_glb_group_intr_stat_stat) } },
    { "en0", { const_cast<Epb_glb_group_intr_stat_en0(*)>(&Epb_glb_group_intr_stat_base->en0), new Epb_glb_group_intr_stat_en0_map, {}, sizeof(Epb_glb_group_intr_stat_en0) } },
    { "en1", { const_cast<Epb_glb_group_intr_stat_en1(*)>(&Epb_glb_group_intr_stat_base->en1), new Epb_glb_group_intr_stat_en1_map, {}, sizeof(Epb_glb_group_intr_stat_en1) } },
    { "inj", { const_cast<Epb_glb_group_intr_stat_inj(*)>(&Epb_glb_group_intr_stat_base->inj), new Epb_glb_group_intr_stat_inj_map, {}, sizeof(Epb_glb_group_intr_stat_inj) } },
    { "freeze_enable", { const_cast<Epb_glb_group_intr_stat_freeze_enable(*)>(&Epb_glb_group_intr_stat_base->freeze_enable), new Epb_glb_group_intr_stat_freeze_enable_map, {}, sizeof(Epb_glb_group_intr_stat_freeze_enable) } }
    } )
  {}
};

struct Epb_intr_log_group_map: public RegisterMapper {
  static constexpr PTR_Epb_intr_log_group Epb_intr_log_group_base=0;
  Epb_intr_log_group_map() : RegisterMapper( {
    { "wpc_fifo_fatal0_err", { const_cast<uint32_t(*)>(&Epb_intr_log_group_base->wpc_fifo_fatal0_err), 0, {}, sizeof(uint32_t) } },
    { "wpc_fifo_fatal1_err", { const_cast<uint32_t(*)>(&Epb_intr_log_group_base->wpc_fifo_fatal1_err), 0, {}, sizeof(uint32_t) } },
    { "wpc_fifo_fatal2_err", { const_cast<uint32_t(*)>(&Epb_intr_log_group_base->wpc_fifo_fatal2_err), 0, {}, sizeof(uint32_t) } },
    { "wpc_fifo_fatal3_err", { const_cast<uint32_t(*)>(&Epb_intr_log_group_base->wpc_fifo_fatal3_err), 0, {}, sizeof(uint32_t) } },
    { "wpc_overflow_err", { const_cast<uint32_t(*)>(&Epb_intr_log_group_base->wpc_overflow_err), 0, {}, sizeof(uint32_t) } },
    { "wpc_ff_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Epb_intr_log_group_base->wpc_ff_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "wpc_ff_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Epb_intr_log_group_base->wpc_ff_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "pbc_overflow_chnl", { const_cast<uint32_t(*)>(&Epb_intr_log_group_base->pbc_overflow_chnl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_glb_group_map: public RegisterMapper {
  static constexpr PTR_Epb_glb_group Epb_glb_group_base=0;
  Epb_glb_group_map() : RegisterMapper( {
    { "sw_reset", { const_cast<uint32_t(*)>(&Epb_glb_group_base->sw_reset), 0, {}, sizeof(uint32_t) } },
    { "glb_status", { const_cast<uint32_t(*)>(&Epb_glb_group_base->glb_status), 0, {}, sizeof(uint32_t) } },
    { "port_rates", { const_cast<uint32_t(*)>(&Epb_glb_group_base->port_rates), 0, {}, sizeof(uint32_t) } },
    { "port_en", { const_cast<uint32_t(*)>(&Epb_glb_group_base->port_en), 0, {}, sizeof(uint32_t) } },
    { "glb_ctrl", { const_cast<uint32_t(*)>(&Epb_glb_group_base->glb_ctrl), 0, {}, sizeof(uint32_t) } },
    { "glb_prsr_crd", { const_cast<uint32_t(*)>(&Epb_glb_group_base->glb_prsr_crd), 0, {}, sizeof(uint32_t) } },
    { "glb_prsr_crd_stat", { const_cast<uint32_t(*)>(&Epb_glb_group_base->glb_prsr_crd_stat), 0, {}, sizeof(uint32_t) } },
    { "glb_prsr_stat", { const_cast<Epb_prsr_stat(*)>(&Epb_glb_group_base->glb_prsr_stat), new Epb_prsr_stat_map, {}, sizeof(Epb_prsr_stat) } },
    { "glb_parser_maxbyte", { const_cast<uint32_t(*)>(&Epb_glb_group_base->glb_parser_maxbyte), 0, {}, sizeof(uint32_t) } },
    { "intr_stat", { &Epb_glb_group_base->intr_stat, new Epb_glb_group_intr_stat_map, {}, sizeof(Epb_glb_group_intr_stat) } },
    { "intr_log_group", { &Epb_glb_group_base->intr_log_group, new Epb_intr_log_group_map, {}, sizeof(Epb_intr_log_group) } },
    { "ecc_inj", { const_cast<uint32_t(*)>(&Epb_glb_group_base->ecc_inj), 0, {}, sizeof(uint32_t) } },
    { "ecc_dis", { const_cast<uint32_t(*)>(&Epb_glb_group_base->ecc_dis), 0, {}, sizeof(uint32_t) } },
    { "time_offset", { const_cast<uint32_t(*)>(&Epb_glb_group_base->time_offset), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Epb_glb_group_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "glb_epb_tim_off", { const_cast<uint32_t(*)>(&Epb_glb_group_base->glb_epb_tim_off), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_chnl_ctrl_map: public RegisterMapper {
  static constexpr PTR_Epb_chnl_ctrl Epb_chnl_ctrl_base=0;
  Epb_chnl_ctrl_map() : RegisterMapper( {
    { "chnl_ctrl_0_2", { const_cast<uint32_t(*)>(&Epb_chnl_ctrl_base->chnl_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_ctrl_1_2", { const_cast<uint32_t(*)>(&Epb_chnl_ctrl_base->chnl_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_chnl_fifo_ctrl_map: public RegisterMapper {
  static constexpr PTR_Epb_chnl_fifo_ctrl Epb_chnl_fifo_ctrl_base=0;
  Epb_chnl_fifo_ctrl_map() : RegisterMapper( {
    { "chnl_fifo_ctrl_0_2", { const_cast<uint32_t(*)>(&Epb_chnl_fifo_ctrl_base->chnl_fifo_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_ctrl_1_2", { const_cast<uint32_t(*)>(&Epb_chnl_fifo_ctrl_base->chnl_fifo_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_chnl_pktnum3_map: public RegisterMapper {
  static constexpr PTR_Epb_chnl_pktnum3 Epb_chnl_pktnum3_base=0;
  Epb_chnl_pktnum3_map() : RegisterMapper( {
    { "chnl_pktnum3_0_2", { const_cast<uint32_t(*)>(&Epb_chnl_pktnum3_base->chnl_pktnum3_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum3_1_2", { const_cast<uint32_t(*)>(&Epb_chnl_pktnum3_base->chnl_pktnum3_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_chnl_parser_send_pkt_map: public RegisterMapper {
  static constexpr PTR_Epb_chnl_parser_send_pkt Epb_chnl_parser_send_pkt_base=0;
  Epb_chnl_parser_send_pkt_map() : RegisterMapper( {
    { "chnl_parser_send_pkt_0_3", { const_cast<uint32_t(*)>(&Epb_chnl_parser_send_pkt_base->chnl_parser_send_pkt_0_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_parser_send_pkt_1_3", { const_cast<uint32_t(*)>(&Epb_chnl_parser_send_pkt_base->chnl_parser_send_pkt_1_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_parser_send_pkt_2_3", { const_cast<uint32_t(*)>(&Epb_chnl_parser_send_pkt_base->chnl_parser_send_pkt_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_chnl_deparser_send_pkt_map: public RegisterMapper {
  static constexpr PTR_Epb_chnl_deparser_send_pkt Epb_chnl_deparser_send_pkt_base=0;
  Epb_chnl_deparser_send_pkt_map() : RegisterMapper( {
    { "chnl_deparser_send_pkt_0_3", { const_cast<uint32_t(*)>(&Epb_chnl_deparser_send_pkt_base->chnl_deparser_send_pkt_0_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_send_pkt_1_3", { const_cast<uint32_t(*)>(&Epb_chnl_deparser_send_pkt_base->chnl_deparser_send_pkt_1_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_send_pkt_2_3", { const_cast<uint32_t(*)>(&Epb_chnl_deparser_send_pkt_base->chnl_deparser_send_pkt_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_chnl_warp_send_pkt_map: public RegisterMapper {
  static constexpr PTR_Epb_chnl_warp_send_pkt Epb_chnl_warp_send_pkt_base=0;
  Epb_chnl_warp_send_pkt_map() : RegisterMapper( {
    { "chnl_warp_send_pkt_0_3", { const_cast<uint32_t(*)>(&Epb_chnl_warp_send_pkt_base->chnl_warp_send_pkt_0_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_warp_send_pkt_1_3", { const_cast<uint32_t(*)>(&Epb_chnl_warp_send_pkt_base->chnl_warp_send_pkt_1_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_warp_send_pkt_2_3", { const_cast<uint32_t(*)>(&Epb_chnl_warp_send_pkt_base->chnl_warp_send_pkt_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_chnl_p2s_received_pkt_map: public RegisterMapper {
  static constexpr PTR_Epb_chnl_p2s_received_pkt Epb_chnl_p2s_received_pkt_base=0;
  Epb_chnl_p2s_received_pkt_map() : RegisterMapper( {
    { "chnl_p2s_received_pkt_0_3", { const_cast<uint32_t(*)>(&Epb_chnl_p2s_received_pkt_base->chnl_p2s_received_pkt_0_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_p2s_received_pkt_1_3", { const_cast<uint32_t(*)>(&Epb_chnl_p2s_received_pkt_base->chnl_p2s_received_pkt_1_3), 0, {}, sizeof(uint32_t) } },
    { "chnl_p2s_received_pkt_2_3", { const_cast<uint32_t(*)>(&Epb_chnl_p2s_received_pkt_base->chnl_p2s_received_pkt_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_rspec_chan0_group_map: public RegisterMapper {
  static constexpr PTR_Epb_rspec_chan0_group Epb_rspec_chan0_group_base=0;
  Epb_rspec_chan0_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Epb_chnl_ctrl(*)>(&Epb_rspec_chan0_group_base->chnl_ctrl), new Epb_chnl_ctrl_map, {}, sizeof(Epb_chnl_ctrl) } },
    { "chnl_fifo_ctrl", { const_cast<Epb_chnl_fifo_ctrl(*)>(&Epb_rspec_chan0_group_base->chnl_fifo_ctrl), new Epb_chnl_fifo_ctrl_map, {}, sizeof(Epb_chnl_fifo_ctrl) } },
    { "chnl_fifo_cfg", { const_cast<uint32_t(*)>(&Epb_rspec_chan0_group_base->chnl_fifo_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Epb_rspec_chan0_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Epb_rspec_chan0_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Epb_rspec_chan0_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum2", { const_cast<uint32_t(*)>(&Epb_rspec_chan0_group_base->chnl_pktnum2), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum3", { const_cast<Epb_chnl_pktnum3(*)>(&Epb_rspec_chan0_group_base->chnl_pktnum3), new Epb_chnl_pktnum3_map, {}, sizeof(Epb_chnl_pktnum3) } },
    { "chnl_parser_send_pkt", { const_cast<Epb_chnl_parser_send_pkt(*)>(&Epb_rspec_chan0_group_base->chnl_parser_send_pkt), new Epb_chnl_parser_send_pkt_map, {}, sizeof(Epb_chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Epb_chnl_deparser_send_pkt(*)>(&Epb_rspec_chan0_group_base->chnl_deparser_send_pkt), new Epb_chnl_deparser_send_pkt_map, {}, sizeof(Epb_chnl_deparser_send_pkt) } },
    { "chnl_warp_send_pkt", { const_cast<Epb_chnl_warp_send_pkt(*)>(&Epb_rspec_chan0_group_base->chnl_warp_send_pkt), new Epb_chnl_warp_send_pkt_map, {}, sizeof(Epb_chnl_warp_send_pkt) } },
    { "chnl_p2s_received_pkt", { const_cast<Epb_chnl_p2s_received_pkt(*)>(&Epb_rspec_chan0_group_base->chnl_p2s_received_pkt), new Epb_chnl_p2s_received_pkt_map, {}, sizeof(Epb_chnl_p2s_received_pkt) } }
    } )
  {}
};

struct Epb_rspec_chan1_group_map: public RegisterMapper {
  static constexpr PTR_Epb_rspec_chan1_group Epb_rspec_chan1_group_base=0;
  Epb_rspec_chan1_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Epb_chnl_ctrl(*)>(&Epb_rspec_chan1_group_base->chnl_ctrl), new Epb_chnl_ctrl_map, {}, sizeof(Epb_chnl_ctrl) } },
    { "chnl_fifo_ctrl", { const_cast<Epb_chnl_fifo_ctrl(*)>(&Epb_rspec_chan1_group_base->chnl_fifo_ctrl), new Epb_chnl_fifo_ctrl_map, {}, sizeof(Epb_chnl_fifo_ctrl) } },
    { "chnl_fifo_cfg", { const_cast<uint32_t(*)>(&Epb_rspec_chan1_group_base->chnl_fifo_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Epb_rspec_chan1_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Epb_rspec_chan1_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Epb_rspec_chan1_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum2", { const_cast<uint32_t(*)>(&Epb_rspec_chan1_group_base->chnl_pktnum2), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum3", { const_cast<Epb_chnl_pktnum3(*)>(&Epb_rspec_chan1_group_base->chnl_pktnum3), new Epb_chnl_pktnum3_map, {}, sizeof(Epb_chnl_pktnum3) } },
    { "chnl_parser_send_pkt", { const_cast<Epb_chnl_parser_send_pkt(*)>(&Epb_rspec_chan1_group_base->chnl_parser_send_pkt), new Epb_chnl_parser_send_pkt_map, {}, sizeof(Epb_chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Epb_chnl_deparser_send_pkt(*)>(&Epb_rspec_chan1_group_base->chnl_deparser_send_pkt), new Epb_chnl_deparser_send_pkt_map, {}, sizeof(Epb_chnl_deparser_send_pkt) } },
    { "chnl_warp_send_pkt", { const_cast<Epb_chnl_warp_send_pkt(*)>(&Epb_rspec_chan1_group_base->chnl_warp_send_pkt), new Epb_chnl_warp_send_pkt_map, {}, sizeof(Epb_chnl_warp_send_pkt) } },
    { "chnl_p2s_received_pkt", { const_cast<Epb_chnl_p2s_received_pkt(*)>(&Epb_rspec_chan1_group_base->chnl_p2s_received_pkt), new Epb_chnl_p2s_received_pkt_map, {}, sizeof(Epb_chnl_p2s_received_pkt) } }
    } )
  {}
};

struct Epb_rspec_chan2_group_map: public RegisterMapper {
  static constexpr PTR_Epb_rspec_chan2_group Epb_rspec_chan2_group_base=0;
  Epb_rspec_chan2_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Epb_chnl_ctrl(*)>(&Epb_rspec_chan2_group_base->chnl_ctrl), new Epb_chnl_ctrl_map, {}, sizeof(Epb_chnl_ctrl) } },
    { "chnl_fifo_ctrl", { const_cast<Epb_chnl_fifo_ctrl(*)>(&Epb_rspec_chan2_group_base->chnl_fifo_ctrl), new Epb_chnl_fifo_ctrl_map, {}, sizeof(Epb_chnl_fifo_ctrl) } },
    { "chnl_fifo_cfg", { const_cast<uint32_t(*)>(&Epb_rspec_chan2_group_base->chnl_fifo_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Epb_rspec_chan2_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Epb_rspec_chan2_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Epb_rspec_chan2_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum2", { const_cast<uint32_t(*)>(&Epb_rspec_chan2_group_base->chnl_pktnum2), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum3", { const_cast<Epb_chnl_pktnum3(*)>(&Epb_rspec_chan2_group_base->chnl_pktnum3), new Epb_chnl_pktnum3_map, {}, sizeof(Epb_chnl_pktnum3) } },
    { "chnl_parser_send_pkt", { const_cast<Epb_chnl_parser_send_pkt(*)>(&Epb_rspec_chan2_group_base->chnl_parser_send_pkt), new Epb_chnl_parser_send_pkt_map, {}, sizeof(Epb_chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Epb_chnl_deparser_send_pkt(*)>(&Epb_rspec_chan2_group_base->chnl_deparser_send_pkt), new Epb_chnl_deparser_send_pkt_map, {}, sizeof(Epb_chnl_deparser_send_pkt) } },
    { "chnl_warp_send_pkt", { const_cast<Epb_chnl_warp_send_pkt(*)>(&Epb_rspec_chan2_group_base->chnl_warp_send_pkt), new Epb_chnl_warp_send_pkt_map, {}, sizeof(Epb_chnl_warp_send_pkt) } },
    { "chnl_p2s_received_pkt", { const_cast<Epb_chnl_p2s_received_pkt(*)>(&Epb_rspec_chan2_group_base->chnl_p2s_received_pkt), new Epb_chnl_p2s_received_pkt_map, {}, sizeof(Epb_chnl_p2s_received_pkt) } }
    } )
  {}
};

struct Epb_rspec_chan3_group_map: public RegisterMapper {
  static constexpr PTR_Epb_rspec_chan3_group Epb_rspec_chan3_group_base=0;
  Epb_rspec_chan3_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Epb_chnl_ctrl(*)>(&Epb_rspec_chan3_group_base->chnl_ctrl), new Epb_chnl_ctrl_map, {}, sizeof(Epb_chnl_ctrl) } },
    { "chnl_fifo_ctrl", { const_cast<Epb_chnl_fifo_ctrl(*)>(&Epb_rspec_chan3_group_base->chnl_fifo_ctrl), new Epb_chnl_fifo_ctrl_map, {}, sizeof(Epb_chnl_fifo_ctrl) } },
    { "chnl_fifo_cfg", { const_cast<uint32_t(*)>(&Epb_rspec_chan3_group_base->chnl_fifo_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Epb_rspec_chan3_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Epb_rspec_chan3_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Epb_rspec_chan3_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum2", { const_cast<uint32_t(*)>(&Epb_rspec_chan3_group_base->chnl_pktnum2), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum3", { const_cast<Epb_chnl_pktnum3(*)>(&Epb_rspec_chan3_group_base->chnl_pktnum3), new Epb_chnl_pktnum3_map, {}, sizeof(Epb_chnl_pktnum3) } },
    { "chnl_parser_send_pkt", { const_cast<Epb_chnl_parser_send_pkt(*)>(&Epb_rspec_chan3_group_base->chnl_parser_send_pkt), new Epb_chnl_parser_send_pkt_map, {}, sizeof(Epb_chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Epb_chnl_deparser_send_pkt(*)>(&Epb_rspec_chan3_group_base->chnl_deparser_send_pkt), new Epb_chnl_deparser_send_pkt_map, {}, sizeof(Epb_chnl_deparser_send_pkt) } },
    { "chnl_warp_send_pkt", { const_cast<Epb_chnl_warp_send_pkt(*)>(&Epb_rspec_chan3_group_base->chnl_warp_send_pkt), new Epb_chnl_warp_send_pkt_map, {}, sizeof(Epb_chnl_warp_send_pkt) } },
    { "chnl_p2s_received_pkt", { const_cast<Epb_chnl_p2s_received_pkt(*)>(&Epb_rspec_chan3_group_base->chnl_p2s_received_pkt), new Epb_chnl_p2s_received_pkt_map, {}, sizeof(Epb_chnl_p2s_received_pkt) } }
    } )
  {}
};

struct Epb_rspec_chan4_group_map: public RegisterMapper {
  static constexpr PTR_Epb_rspec_chan4_group Epb_rspec_chan4_group_base=0;
  Epb_rspec_chan4_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Epb_chnl_ctrl(*)>(&Epb_rspec_chan4_group_base->chnl_ctrl), new Epb_chnl_ctrl_map, {}, sizeof(Epb_chnl_ctrl) } },
    { "chnl_fifo_ctrl", { const_cast<Epb_chnl_fifo_ctrl(*)>(&Epb_rspec_chan4_group_base->chnl_fifo_ctrl), new Epb_chnl_fifo_ctrl_map, {}, sizeof(Epb_chnl_fifo_ctrl) } },
    { "chnl_fifo_cfg", { const_cast<uint32_t(*)>(&Epb_rspec_chan4_group_base->chnl_fifo_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Epb_rspec_chan4_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Epb_rspec_chan4_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Epb_rspec_chan4_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum2", { const_cast<uint32_t(*)>(&Epb_rspec_chan4_group_base->chnl_pktnum2), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum3", { const_cast<Epb_chnl_pktnum3(*)>(&Epb_rspec_chan4_group_base->chnl_pktnum3), new Epb_chnl_pktnum3_map, {}, sizeof(Epb_chnl_pktnum3) } },
    { "chnl_parser_send_pkt", { const_cast<Epb_chnl_parser_send_pkt(*)>(&Epb_rspec_chan4_group_base->chnl_parser_send_pkt), new Epb_chnl_parser_send_pkt_map, {}, sizeof(Epb_chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Epb_chnl_deparser_send_pkt(*)>(&Epb_rspec_chan4_group_base->chnl_deparser_send_pkt), new Epb_chnl_deparser_send_pkt_map, {}, sizeof(Epb_chnl_deparser_send_pkt) } },
    { "chnl_warp_send_pkt", { const_cast<Epb_chnl_warp_send_pkt(*)>(&Epb_rspec_chan4_group_base->chnl_warp_send_pkt), new Epb_chnl_warp_send_pkt_map, {}, sizeof(Epb_chnl_warp_send_pkt) } },
    { "chnl_p2s_received_pkt", { const_cast<Epb_chnl_p2s_received_pkt(*)>(&Epb_rspec_chan4_group_base->chnl_p2s_received_pkt), new Epb_chnl_p2s_received_pkt_map, {}, sizeof(Epb_chnl_p2s_received_pkt) } }
    } )
  {}
};

struct Epb_rspec_chan5_group_map: public RegisterMapper {
  static constexpr PTR_Epb_rspec_chan5_group Epb_rspec_chan5_group_base=0;
  Epb_rspec_chan5_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Epb_chnl_ctrl(*)>(&Epb_rspec_chan5_group_base->chnl_ctrl), new Epb_chnl_ctrl_map, {}, sizeof(Epb_chnl_ctrl) } },
    { "chnl_fifo_ctrl", { const_cast<Epb_chnl_fifo_ctrl(*)>(&Epb_rspec_chan5_group_base->chnl_fifo_ctrl), new Epb_chnl_fifo_ctrl_map, {}, sizeof(Epb_chnl_fifo_ctrl) } },
    { "chnl_fifo_cfg", { const_cast<uint32_t(*)>(&Epb_rspec_chan5_group_base->chnl_fifo_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Epb_rspec_chan5_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Epb_rspec_chan5_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Epb_rspec_chan5_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum2", { const_cast<uint32_t(*)>(&Epb_rspec_chan5_group_base->chnl_pktnum2), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum3", { const_cast<Epb_chnl_pktnum3(*)>(&Epb_rspec_chan5_group_base->chnl_pktnum3), new Epb_chnl_pktnum3_map, {}, sizeof(Epb_chnl_pktnum3) } },
    { "chnl_parser_send_pkt", { const_cast<Epb_chnl_parser_send_pkt(*)>(&Epb_rspec_chan5_group_base->chnl_parser_send_pkt), new Epb_chnl_parser_send_pkt_map, {}, sizeof(Epb_chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Epb_chnl_deparser_send_pkt(*)>(&Epb_rspec_chan5_group_base->chnl_deparser_send_pkt), new Epb_chnl_deparser_send_pkt_map, {}, sizeof(Epb_chnl_deparser_send_pkt) } },
    { "chnl_warp_send_pkt", { const_cast<Epb_chnl_warp_send_pkt(*)>(&Epb_rspec_chan5_group_base->chnl_warp_send_pkt), new Epb_chnl_warp_send_pkt_map, {}, sizeof(Epb_chnl_warp_send_pkt) } },
    { "chnl_p2s_received_pkt", { const_cast<Epb_chnl_p2s_received_pkt(*)>(&Epb_rspec_chan5_group_base->chnl_p2s_received_pkt), new Epb_chnl_p2s_received_pkt_map, {}, sizeof(Epb_chnl_p2s_received_pkt) } }
    } )
  {}
};

struct Epb_rspec_chan6_group_map: public RegisterMapper {
  static constexpr PTR_Epb_rspec_chan6_group Epb_rspec_chan6_group_base=0;
  Epb_rspec_chan6_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Epb_chnl_ctrl(*)>(&Epb_rspec_chan6_group_base->chnl_ctrl), new Epb_chnl_ctrl_map, {}, sizeof(Epb_chnl_ctrl) } },
    { "chnl_fifo_ctrl", { const_cast<Epb_chnl_fifo_ctrl(*)>(&Epb_rspec_chan6_group_base->chnl_fifo_ctrl), new Epb_chnl_fifo_ctrl_map, {}, sizeof(Epb_chnl_fifo_ctrl) } },
    { "chnl_fifo_cfg", { const_cast<uint32_t(*)>(&Epb_rspec_chan6_group_base->chnl_fifo_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Epb_rspec_chan6_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Epb_rspec_chan6_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Epb_rspec_chan6_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum2", { const_cast<uint32_t(*)>(&Epb_rspec_chan6_group_base->chnl_pktnum2), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum3", { const_cast<Epb_chnl_pktnum3(*)>(&Epb_rspec_chan6_group_base->chnl_pktnum3), new Epb_chnl_pktnum3_map, {}, sizeof(Epb_chnl_pktnum3) } },
    { "chnl_parser_send_pkt", { const_cast<Epb_chnl_parser_send_pkt(*)>(&Epb_rspec_chan6_group_base->chnl_parser_send_pkt), new Epb_chnl_parser_send_pkt_map, {}, sizeof(Epb_chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Epb_chnl_deparser_send_pkt(*)>(&Epb_rspec_chan6_group_base->chnl_deparser_send_pkt), new Epb_chnl_deparser_send_pkt_map, {}, sizeof(Epb_chnl_deparser_send_pkt) } },
    { "chnl_warp_send_pkt", { const_cast<Epb_chnl_warp_send_pkt(*)>(&Epb_rspec_chan6_group_base->chnl_warp_send_pkt), new Epb_chnl_warp_send_pkt_map, {}, sizeof(Epb_chnl_warp_send_pkt) } },
    { "chnl_p2s_received_pkt", { const_cast<Epb_chnl_p2s_received_pkt(*)>(&Epb_rspec_chan6_group_base->chnl_p2s_received_pkt), new Epb_chnl_p2s_received_pkt_map, {}, sizeof(Epb_chnl_p2s_received_pkt) } }
    } )
  {}
};

struct Epb_rspec_chan7_group_map: public RegisterMapper {
  static constexpr PTR_Epb_rspec_chan7_group Epb_rspec_chan7_group_base=0;
  Epb_rspec_chan7_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<Epb_chnl_ctrl(*)>(&Epb_rspec_chan7_group_base->chnl_ctrl), new Epb_chnl_ctrl_map, {}, sizeof(Epb_chnl_ctrl) } },
    { "chnl_fifo_ctrl", { const_cast<Epb_chnl_fifo_ctrl(*)>(&Epb_rspec_chan7_group_base->chnl_fifo_ctrl), new Epb_chnl_fifo_ctrl_map, {}, sizeof(Epb_chnl_fifo_ctrl) } },
    { "chnl_fifo_cfg", { const_cast<uint32_t(*)>(&Epb_rspec_chan7_group_base->chnl_fifo_cfg), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Epb_rspec_chan7_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Epb_rspec_chan7_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Epb_rspec_chan7_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum2", { const_cast<uint32_t(*)>(&Epb_rspec_chan7_group_base->chnl_pktnum2), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum3", { const_cast<Epb_chnl_pktnum3(*)>(&Epb_rspec_chan7_group_base->chnl_pktnum3), new Epb_chnl_pktnum3_map, {}, sizeof(Epb_chnl_pktnum3) } },
    { "chnl_parser_send_pkt", { const_cast<Epb_chnl_parser_send_pkt(*)>(&Epb_rspec_chan7_group_base->chnl_parser_send_pkt), new Epb_chnl_parser_send_pkt_map, {}, sizeof(Epb_chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Epb_chnl_deparser_send_pkt(*)>(&Epb_rspec_chan7_group_base->chnl_deparser_send_pkt), new Epb_chnl_deparser_send_pkt_map, {}, sizeof(Epb_chnl_deparser_send_pkt) } },
    { "chnl_warp_send_pkt", { const_cast<Epb_chnl_warp_send_pkt(*)>(&Epb_rspec_chan7_group_base->chnl_warp_send_pkt), new Epb_chnl_warp_send_pkt_map, {}, sizeof(Epb_chnl_warp_send_pkt) } },
    { "chnl_p2s_received_pkt", { const_cast<Epb_chnl_p2s_received_pkt(*)>(&Epb_rspec_chan7_group_base->chnl_p2s_received_pkt), new Epb_chnl_p2s_received_pkt_map, {}, sizeof(Epb_chnl_p2s_received_pkt) } }
    } )
  {}
};

struct Epb_rspec_map: public RegisterMapper {
  static constexpr PTR_Epb_rspec Epb_rspec_base=0;
  Epb_rspec_map() : RegisterMapper( {
    { "glb_group", { &Epb_rspec_base->glb_group, new Epb_glb_group_map, {}, sizeof(Epb_glb_group) } },
    { "chan0_group", { &Epb_rspec_base->chan0_group, new Epb_rspec_chan0_group_map, {}, sizeof(Epb_rspec_chan0_group) } },
    { "chan1_group", { &Epb_rspec_base->chan1_group, new Epb_rspec_chan1_group_map, {}, sizeof(Epb_rspec_chan1_group) } },
    { "chan2_group", { &Epb_rspec_base->chan2_group, new Epb_rspec_chan2_group_map, {}, sizeof(Epb_rspec_chan2_group) } },
    { "chan3_group", { &Epb_rspec_base->chan3_group, new Epb_rspec_chan3_group_map, {}, sizeof(Epb_rspec_chan3_group) } },
    { "chan4_group", { &Epb_rspec_base->chan4_group, new Epb_rspec_chan4_group_map, {}, sizeof(Epb_rspec_chan4_group) } },
    { "chan5_group", { &Epb_rspec_base->chan5_group, new Epb_rspec_chan5_group_map, {}, sizeof(Epb_rspec_chan5_group) } },
    { "chan6_group", { &Epb_rspec_base->chan6_group, new Epb_rspec_chan6_group_map, {}, sizeof(Epb_rspec_chan6_group) } },
    { "chan7_group", { &Epb_rspec_base->chan7_group, new Epb_rspec_chan7_group_map, {}, sizeof(Epb_rspec_chan7_group) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Epb_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } },
    { "scratch", { const_cast<uint32_t(*)>(&Epb_rspec_base->scratch), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_prsr4_reg_map: public RegisterMapper {
  static constexpr PTR_Epb_prsr4_reg Epb_prsr4_reg_base=0;
  Epb_prsr4_reg_map() : RegisterMapper( {
    { "epbreg", { &Epb_prsr4_reg_base->epbreg, new Epb_rspec_map, {}, sizeof(Epb_rspec) } },
    { "prsr", { &Epb_prsr4_reg_base->prsr, new Prsr_reg_main_rspec_map, {0x4}, sizeof(Prsr_reg_main_rspec) } }
    } )
  {}
};

struct Pbus_station_regs_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Pbus_station_regs_rspec_intr Pbus_station_regs_rspec_intr_base=0;
  Pbus_station_regs_rspec_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Pbus_station_regs_rspec_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Pbus_station_regs_rspec_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Pbus_station_regs_rspec_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Pbus_station_regs_rspec_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Pbus_station_regs_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pbus_station_regs_rspec_map: public RegisterMapper {
  static constexpr PTR_Pbus_station_regs_rspec Pbus_station_regs_rspec_base=0;
  Pbus_station_regs_rspec_map() : RegisterMapper( {
    { "intr", { &Pbus_station_regs_rspec_base->intr, new Pbus_station_regs_rspec_intr_map, {}, sizeof(Pbus_station_regs_rspec_intr) } },
    { "ecc", { const_cast<uint32_t(*)>(&Pbus_station_regs_rspec_base->ecc), 0, {}, sizeof(uint32_t) } },
    { "ecc_dis", { const_cast<uint32_t(*)>(&Pbus_station_regs_rspec_base->ecc_dis), 0, {}, sizeof(uint32_t) } },
    { "capt_dual_ecc_addr", { const_cast<uint32_t(*)>(&Pbus_station_regs_rspec_base->capt_dual_ecc_addr), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Pbus_station_regs_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "scratch", { const_cast<uint32_t(*)>(&Pbus_station_regs_rspec_base->scratch), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_app_recir_match_value_map: public RegisterMapper {
  static constexpr PTR_Pgr_app_recir_match_value Pgr_app_recir_match_value_base=0;
  Pgr_app_recir_match_value_map() : RegisterMapper( {
    { "recir_match_value_0_4", { const_cast<uint32_t(*)>(&Pgr_app_recir_match_value_base->recir_match_value_0_4), 0, {}, sizeof(uint32_t) } },
    { "recir_match_value_1_4", { const_cast<uint32_t(*)>(&Pgr_app_recir_match_value_base->recir_match_value_1_4), 0, {}, sizeof(uint32_t) } },
    { "recir_match_value_2_4", { const_cast<uint32_t(*)>(&Pgr_app_recir_match_value_base->recir_match_value_2_4), 0, {}, sizeof(uint32_t) } },
    { "recir_match_value_3_4", { const_cast<uint32_t(*)>(&Pgr_app_recir_match_value_base->recir_match_value_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_app_recir_match_mask_map: public RegisterMapper {
  static constexpr PTR_Pgr_app_recir_match_mask Pgr_app_recir_match_mask_base=0;
  Pgr_app_recir_match_mask_map() : RegisterMapper( {
    { "recir_match_mask_0_4", { const_cast<uint32_t(*)>(&Pgr_app_recir_match_mask_base->recir_match_mask_0_4), 0, {}, sizeof(uint32_t) } },
    { "recir_match_mask_1_4", { const_cast<uint32_t(*)>(&Pgr_app_recir_match_mask_base->recir_match_mask_1_4), 0, {}, sizeof(uint32_t) } },
    { "recir_match_mask_2_4", { const_cast<uint32_t(*)>(&Pgr_app_recir_match_mask_base->recir_match_mask_2_4), 0, {}, sizeof(uint32_t) } },
    { "recir_match_mask_3_4", { const_cast<uint32_t(*)>(&Pgr_app_recir_match_mask_base->recir_match_mask_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_app_ctr48_trigger_map: public RegisterMapper {
  static constexpr PTR_Pgr_app_ctr48_trigger Pgr_app_ctr48_trigger_base=0;
  Pgr_app_ctr48_trigger_map() : RegisterMapper( {
    { "ctr48_trigger_0_2", { const_cast<uint32_t(*)>(&Pgr_app_ctr48_trigger_base->ctr48_trigger_0_2), 0, {}, sizeof(uint32_t) } },
    { "ctr48_trigger_1_2", { const_cast<uint32_t(*)>(&Pgr_app_ctr48_trigger_base->ctr48_trigger_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_app_ctr48_batch_map: public RegisterMapper {
  static constexpr PTR_Pgr_app_ctr48_batch Pgr_app_ctr48_batch_base=0;
  Pgr_app_ctr48_batch_map() : RegisterMapper( {
    { "ctr48_batch_0_2", { const_cast<uint32_t(*)>(&Pgr_app_ctr48_batch_base->ctr48_batch_0_2), 0, {}, sizeof(uint32_t) } },
    { "ctr48_batch_1_2", { const_cast<uint32_t(*)>(&Pgr_app_ctr48_batch_base->ctr48_batch_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_app_ctr48_packet_map: public RegisterMapper {
  static constexpr PTR_Pgr_app_ctr48_packet Pgr_app_ctr48_packet_base=0;
  Pgr_app_ctr48_packet_map() : RegisterMapper( {
    { "ctr48_packet_0_2", { const_cast<uint32_t(*)>(&Pgr_app_ctr48_packet_base->ctr48_packet_0_2), 0, {}, sizeof(uint32_t) } },
    { "ctr48_packet_1_2", { const_cast<uint32_t(*)>(&Pgr_app_ctr48_packet_base->ctr48_packet_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_app_reg_rspec_map: public RegisterMapper {
  static constexpr PTR_Pgr_app_reg_rspec Pgr_app_reg_rspec_base=0;
  Pgr_app_reg_rspec_map() : RegisterMapper( {
    { "ctrl", { const_cast<uint32_t(*)>(&Pgr_app_reg_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "payload_ctrl", { const_cast<uint32_t(*)>(&Pgr_app_reg_rspec_base->payload_ctrl), 0, {}, sizeof(uint32_t) } },
    { "ingr_port_ctrl", { const_cast<uint32_t(*)>(&Pgr_app_reg_rspec_base->ingr_port_ctrl), 0, {}, sizeof(uint32_t) } },
    { "recir_match_value", { const_cast<Pgr_app_recir_match_value(*)>(&Pgr_app_reg_rspec_base->recir_match_value), new Pgr_app_recir_match_value_map, {}, sizeof(Pgr_app_recir_match_value) } },
    { "recir_match_mask", { const_cast<Pgr_app_recir_match_mask(*)>(&Pgr_app_reg_rspec_base->recir_match_mask), new Pgr_app_recir_match_mask_map, {}, sizeof(Pgr_app_recir_match_mask) } },
    { "event_number", { const_cast<uint32_t(*)>(&Pgr_app_reg_rspec_base->event_number), 0, {}, sizeof(uint32_t) } },
    { "event_ibg_jitter_base_value", { const_cast<uint32_t(*)>(&Pgr_app_reg_rspec_base->event_ibg_jitter_base_value), 0, {}, sizeof(uint32_t) } },
    { "event_max_ibg_jitter", { const_cast<uint32_t(*)>(&Pgr_app_reg_rspec_base->event_max_ibg_jitter), 0, {}, sizeof(uint32_t) } },
    { "event_ibg_jitter_scale", { const_cast<uint32_t(*)>(&Pgr_app_reg_rspec_base->event_ibg_jitter_scale), 0, {}, sizeof(uint32_t) } },
    { "event_ipg_jitter_base_value", { const_cast<uint32_t(*)>(&Pgr_app_reg_rspec_base->event_ipg_jitter_base_value), 0, {}, sizeof(uint32_t) } },
    { "event_max_ipg_jitter", { const_cast<uint32_t(*)>(&Pgr_app_reg_rspec_base->event_max_ipg_jitter), 0, {}, sizeof(uint32_t) } },
    { "event_ipg_jitter_scale", { const_cast<uint32_t(*)>(&Pgr_app_reg_rspec_base->event_ipg_jitter_scale), 0, {}, sizeof(uint32_t) } },
    { "event_timer", { const_cast<uint32_t(*)>(&Pgr_app_reg_rspec_base->event_timer), 0, {}, sizeof(uint32_t) } },
    { "ctr48_trigger", { const_cast<Pgr_app_ctr48_trigger(*)>(&Pgr_app_reg_rspec_base->ctr48_trigger), new Pgr_app_ctr48_trigger_map, {}, sizeof(Pgr_app_ctr48_trigger) } },
    { "ctr48_batch", { const_cast<Pgr_app_ctr48_batch(*)>(&Pgr_app_reg_rspec_base->ctr48_batch), new Pgr_app_ctr48_batch_map, {}, sizeof(Pgr_app_ctr48_batch) } },
    { "ctr48_packet", { const_cast<Pgr_app_ctr48_packet(*)>(&Pgr_app_reg_rspec_base->ctr48_packet), new Pgr_app_ctr48_packet_map, {}, sizeof(Pgr_app_ctr48_packet) } },
    { "log", { const_cast<uint32_t(*)>(&Pgr_app_reg_rspec_base->log), 0, {}, sizeof(uint32_t) } },
    { "cnt_evt_ovf", { const_cast<uint32_t(*)>(&Pgr_app_reg_rspec_base->cnt_evt_ovf), 0, {}, sizeof(uint32_t) } },
    { "scratch", { const_cast<uint32_t(*)>(&Pgr_app_reg_rspec_base->scratch), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_ipb_port_ctrl_map: public RegisterMapper {
  static constexpr PTR_Pgr_ipb_port_ctrl Pgr_ipb_port_ctrl_base=0;
  Pgr_ipb_port_ctrl_map() : RegisterMapper( {
    { "ipb_port_ctrl_0_2", { const_cast<uint32_t(*)>(&Pgr_ipb_port_ctrl_base->ipb_port_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "ipb_port_ctrl_1_2", { const_cast<uint32_t(*)>(&Pgr_ipb_port_ctrl_base->ipb_port_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_pgen_port_down_event_mask_map: public RegisterMapper {
  static constexpr PTR_Pgr_pgen_port_down_event_mask Pgr_pgen_port_down_event_mask_base=0;
  Pgr_pgen_port_down_event_mask_map() : RegisterMapper( {
    { "pgen_port_down_mask_0_3", { const_cast<uint32_t(*)>(&Pgr_pgen_port_down_event_mask_base->pgen_port_down_mask_0_3), 0, {}, sizeof(uint32_t) } },
    { "pgen_port_down_mask_1_3", { const_cast<uint32_t(*)>(&Pgr_pgen_port_down_event_mask_base->pgen_port_down_mask_1_3), 0, {}, sizeof(uint32_t) } },
    { "pgen_port_down_mask_2_3", { const_cast<uint32_t(*)>(&Pgr_pgen_port_down_event_mask_base->pgen_port_down_mask_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_port_down_dis_map: public RegisterMapper {
  static constexpr PTR_Pgr_port_down_dis Pgr_port_down_dis_base=0;
  Pgr_port_down_dis_map() : RegisterMapper( {
    { "port_down_dis_0_3", { const_cast<uint32_t(*)>(&Pgr_port_down_dis_base->port_down_dis_0_3), 0, {}, sizeof(uint32_t) } },
    { "port_down_dis_1_3", { const_cast<uint32_t(*)>(&Pgr_port_down_dis_base->port_down_dis_1_3), 0, {}, sizeof(uint32_t) } },
    { "port_down_dis_2_3", { const_cast<uint32_t(*)>(&Pgr_port_down_dis_base->port_down_dis_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_intr_stat_map: public RegisterMapper {
  static constexpr PTR_Pgr_intr_stat Pgr_intr_stat_base=0;
  Pgr_intr_stat_map() : RegisterMapper( {
    { "intr_stat_0_4", { const_cast<uint32_t(*)>(&Pgr_intr_stat_base->intr_stat_0_4), 0, {}, sizeof(uint32_t) } },
    { "intr_stat_1_4", { const_cast<uint32_t(*)>(&Pgr_intr_stat_base->intr_stat_1_4), 0, {}, sizeof(uint32_t) } },
    { "intr_stat_2_4", { const_cast<uint32_t(*)>(&Pgr_intr_stat_base->intr_stat_2_4), 0, {}, sizeof(uint32_t) } },
    { "intr_stat_3_4", { const_cast<uint32_t(*)>(&Pgr_intr_stat_base->intr_stat_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_intr_en0_map: public RegisterMapper {
  static constexpr PTR_Pgr_intr_en0 Pgr_intr_en0_base=0;
  Pgr_intr_en0_map() : RegisterMapper( {
    { "intr_en0_0_4", { const_cast<uint32_t(*)>(&Pgr_intr_en0_base->intr_en0_0_4), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_1_4", { const_cast<uint32_t(*)>(&Pgr_intr_en0_base->intr_en0_1_4), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_2_4", { const_cast<uint32_t(*)>(&Pgr_intr_en0_base->intr_en0_2_4), 0, {}, sizeof(uint32_t) } },
    { "intr_en0_3_4", { const_cast<uint32_t(*)>(&Pgr_intr_en0_base->intr_en0_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_intr_en1_map: public RegisterMapper {
  static constexpr PTR_Pgr_intr_en1 Pgr_intr_en1_base=0;
  Pgr_intr_en1_map() : RegisterMapper( {
    { "intr_en1_0_4", { const_cast<uint32_t(*)>(&Pgr_intr_en1_base->intr_en1_0_4), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_1_4", { const_cast<uint32_t(*)>(&Pgr_intr_en1_base->intr_en1_1_4), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_2_4", { const_cast<uint32_t(*)>(&Pgr_intr_en1_base->intr_en1_2_4), 0, {}, sizeof(uint32_t) } },
    { "intr_en1_3_4", { const_cast<uint32_t(*)>(&Pgr_intr_en1_base->intr_en1_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_freeze_en_map: public RegisterMapper {
  static constexpr PTR_Pgr_freeze_en Pgr_freeze_en_base=0;
  Pgr_freeze_en_map() : RegisterMapper( {
    { "freeze_en_0_4", { const_cast<uint32_t(*)>(&Pgr_freeze_en_base->freeze_en_0_4), 0, {}, sizeof(uint32_t) } },
    { "freeze_en_1_4", { const_cast<uint32_t(*)>(&Pgr_freeze_en_base->freeze_en_1_4), 0, {}, sizeof(uint32_t) } },
    { "freeze_en_2_4", { const_cast<uint32_t(*)>(&Pgr_freeze_en_base->freeze_en_2_4), 0, {}, sizeof(uint32_t) } },
    { "freeze_en_3_4", { const_cast<uint32_t(*)>(&Pgr_freeze_en_base->freeze_en_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_intr_inj_map: public RegisterMapper {
  static constexpr PTR_Pgr_intr_inj Pgr_intr_inj_base=0;
  Pgr_intr_inj_map() : RegisterMapper( {
    { "intr_inj_0_4", { const_cast<uint32_t(*)>(&Pgr_intr_inj_base->intr_inj_0_4), 0, {}, sizeof(uint32_t) } },
    { "intr_inj_1_4", { const_cast<uint32_t(*)>(&Pgr_intr_inj_base->intr_inj_1_4), 0, {}, sizeof(uint32_t) } },
    { "intr_inj_2_4", { const_cast<uint32_t(*)>(&Pgr_intr_inj_base->intr_inj_2_4), 0, {}, sizeof(uint32_t) } },
    { "intr_inj_3_4", { const_cast<uint32_t(*)>(&Pgr_intr_inj_base->intr_inj_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_sbe_log_map: public RegisterMapper {
  static constexpr PTR_Pgr_sbe_log Pgr_sbe_log_base=0;
  Pgr_sbe_log_map() : RegisterMapper( {
    { "sbe_log_0_2", { const_cast<uint32_t(*)>(&Pgr_sbe_log_base->sbe_log_0_2), 0, {}, sizeof(uint32_t) } },
    { "sbe_log_1_2", { const_cast<uint32_t(*)>(&Pgr_sbe_log_base->sbe_log_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_mbe_log_map: public RegisterMapper {
  static constexpr PTR_Pgr_mbe_log Pgr_mbe_log_base=0;
  Pgr_mbe_log_map() : RegisterMapper( {
    { "mbe_log_0_2", { const_cast<uint32_t(*)>(&Pgr_mbe_log_base->mbe_log_0_2), 0, {}, sizeof(uint32_t) } },
    { "mbe_log_1_2", { const_cast<uint32_t(*)>(&Pgr_mbe_log_base->mbe_log_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_pgen_chnl_log_map: public RegisterMapper {
  static constexpr PTR_Pgr_pgen_chnl_log Pgr_pgen_chnl_log_base=0;
  Pgr_pgen_chnl_log_map() : RegisterMapper( {
    { "pgen_chnl_log_0_3", { const_cast<uint32_t(*)>(&Pgr_pgen_chnl_log_base->pgen_chnl_log_0_3), 0, {}, sizeof(uint32_t) } },
    { "pgen_chnl_log_1_3", { const_cast<uint32_t(*)>(&Pgr_pgen_chnl_log_base->pgen_chnl_log_1_3), 0, {}, sizeof(uint32_t) } },
    { "pgen_chnl_log_2_3", { const_cast<uint32_t(*)>(&Pgr_pgen_chnl_log_base->pgen_chnl_log_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_pgen_pfc_log_map: public RegisterMapper {
  static constexpr PTR_Pgr_pgen_pfc_log Pgr_pgen_pfc_log_base=0;
  Pgr_pgen_pfc_log_map() : RegisterMapper( {
    { "pgen_pfc_log_0_2", { const_cast<uint32_t(*)>(&Pgr_pgen_pfc_log_base->pgen_pfc_log_0_2), 0, {}, sizeof(uint32_t) } },
    { "pgen_pfc_log_1_2", { const_cast<uint32_t(*)>(&Pgr_pgen_pfc_log_base->pgen_pfc_log_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_pgen_port_down_log_map: public RegisterMapper {
  static constexpr PTR_Pgr_pgen_port_down_log Pgr_pgen_port_down_log_base=0;
  Pgr_pgen_port_down_log_map() : RegisterMapper( {
    { "pgen_port_down_log_0_3", { const_cast<uint32_t(*)>(&Pgr_pgen_port_down_log_base->pgen_port_down_log_0_3), 0, {}, sizeof(uint32_t) } },
    { "pgen_port_down_log_1_3", { const_cast<uint32_t(*)>(&Pgr_pgen_port_down_log_base->pgen_port_down_log_1_3), 0, {}, sizeof(uint32_t) } },
    { "pgen_port_down_log_2_3", { const_cast<uint32_t(*)>(&Pgr_pgen_port_down_log_base->pgen_port_down_log_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_recir_log_map: public RegisterMapper {
  static constexpr PTR_Pgr_recir_log Pgr_recir_log_base=0;
  Pgr_recir_log_map() : RegisterMapper( {
    { "recir_log_0_4", { const_cast<uint32_t(*)>(&Pgr_recir_log_base->recir_log_0_4), 0, {}, sizeof(uint32_t) } },
    { "recir_log_1_4", { const_cast<uint32_t(*)>(&Pgr_recir_log_base->recir_log_1_4), 0, {}, sizeof(uint32_t) } },
    { "recir_log_2_4", { const_cast<uint32_t(*)>(&Pgr_recir_log_base->recir_log_2_4), 0, {}, sizeof(uint32_t) } },
    { "recir_log_3_4", { const_cast<uint32_t(*)>(&Pgr_recir_log_base->recir_log_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_ipb_chnl_pfc_cfg_map: public RegisterMapper {
  static constexpr PTR_Pgr_ipb_chnl_pfc_cfg Pgr_ipb_chnl_pfc_cfg_base=0;
  Pgr_ipb_chnl_pfc_cfg_map() : RegisterMapper( {
    { "ipb_chnl_pfc_cfg_0_3", { const_cast<uint32_t(*)>(&Pgr_ipb_chnl_pfc_cfg_base->ipb_chnl_pfc_cfg_0_3), 0, {}, sizeof(uint32_t) } },
    { "ipb_chnl_pfc_cfg_1_3", { const_cast<uint32_t(*)>(&Pgr_ipb_chnl_pfc_cfg_base->ipb_chnl_pfc_cfg_1_3), 0, {}, sizeof(uint32_t) } },
    { "ipb_chnl_pfc_cfg_2_3", { const_cast<uint32_t(*)>(&Pgr_ipb_chnl_pfc_cfg_base->ipb_chnl_pfc_cfg_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_eth_cpu_chnl_pfc_cfg_map: public RegisterMapper {
  static constexpr PTR_Pgr_eth_cpu_chnl_pfc_cfg Pgr_eth_cpu_chnl_pfc_cfg_base=0;
  Pgr_eth_cpu_chnl_pfc_cfg_map() : RegisterMapper( {
    { "eth_cpu_chnl_pfc_cfg_0_2", { const_cast<uint32_t(*)>(&Pgr_eth_cpu_chnl_pfc_cfg_base->eth_cpu_chnl_pfc_cfg_0_2), 0, {}, sizeof(uint32_t) } },
    { "eth_cpu_chnl_pfc_cfg_1_2", { const_cast<uint32_t(*)>(&Pgr_eth_cpu_chnl_pfc_cfg_base->eth_cpu_chnl_pfc_cfg_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_cfg_pfc_hdr_map: public RegisterMapper {
  static constexpr PTR_Pgr_cfg_pfc_hdr Pgr_cfg_pfc_hdr_base=0;
  Pgr_cfg_pfc_hdr_map() : RegisterMapper( {
    { "cfg_pfc_hdr_0_4", { const_cast<uint32_t(*)>(&Pgr_cfg_pfc_hdr_base->cfg_pfc_hdr_0_4), 0, {}, sizeof(uint32_t) } },
    { "cfg_pfc_hdr_1_4", { const_cast<uint32_t(*)>(&Pgr_cfg_pfc_hdr_base->cfg_pfc_hdr_1_4), 0, {}, sizeof(uint32_t) } },
    { "cfg_pfc_hdr_2_4", { const_cast<uint32_t(*)>(&Pgr_cfg_pfc_hdr_base->cfg_pfc_hdr_2_4), 0, {}, sizeof(uint32_t) } },
    { "cfg_pfc_hdr_3_4", { const_cast<uint32_t(*)>(&Pgr_cfg_pfc_hdr_base->cfg_pfc_hdr_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_pgen_ctrl_map: public RegisterMapper {
  static constexpr PTR_Pgr_pgen_ctrl Pgr_pgen_ctrl_base=0;
  Pgr_pgen_ctrl_map() : RegisterMapper( {
    { "pgen_ctrl_0_2", { const_cast<uint32_t(*)>(&Pgr_pgen_ctrl_base->pgen_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "pgen_ctrl_1_2", { const_cast<uint32_t(*)>(&Pgr_pgen_ctrl_base->pgen_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_data_fifo0_sbe_log_map: public RegisterMapper {
  static constexpr PTR_Pgr_data_fifo0_sbe_log Pgr_data_fifo0_sbe_log_base=0;
  Pgr_data_fifo0_sbe_log_map() : RegisterMapper( {
    { "pgr_data_fifo0_sbe_log_0_2", { const_cast<uint32_t(*)>(&Pgr_data_fifo0_sbe_log_base->pgr_data_fifo0_sbe_log_0_2), 0, {}, sizeof(uint32_t) } },
    { "pgr_data_fifo0_sbe_log_1_2", { const_cast<uint32_t(*)>(&Pgr_data_fifo0_sbe_log_base->pgr_data_fifo0_sbe_log_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_data_fifo0_mbe_log_map: public RegisterMapper {
  static constexpr PTR_Pgr_data_fifo0_mbe_log Pgr_data_fifo0_mbe_log_base=0;
  Pgr_data_fifo0_mbe_log_map() : RegisterMapper( {
    { "pgr_data_fifo0_mbe_log_0_2", { const_cast<uint32_t(*)>(&Pgr_data_fifo0_mbe_log_base->pgr_data_fifo0_mbe_log_0_2), 0, {}, sizeof(uint32_t) } },
    { "pgr_data_fifo0_mbe_log_1_2", { const_cast<uint32_t(*)>(&Pgr_data_fifo0_mbe_log_base->pgr_data_fifo0_mbe_log_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_data_fifo1_sbe_log_map: public RegisterMapper {
  static constexpr PTR_Pgr_data_fifo1_sbe_log Pgr_data_fifo1_sbe_log_base=0;
  Pgr_data_fifo1_sbe_log_map() : RegisterMapper( {
    { "pgr_data_fifo1_sbe_log_0_2", { const_cast<uint32_t(*)>(&Pgr_data_fifo1_sbe_log_base->pgr_data_fifo1_sbe_log_0_2), 0, {}, sizeof(uint32_t) } },
    { "pgr_data_fifo1_sbe_log_1_2", { const_cast<uint32_t(*)>(&Pgr_data_fifo1_sbe_log_base->pgr_data_fifo1_sbe_log_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_data_fifo1_mbe_log_map: public RegisterMapper {
  static constexpr PTR_Pgr_data_fifo1_mbe_log Pgr_data_fifo1_mbe_log_base=0;
  Pgr_data_fifo1_mbe_log_map() : RegisterMapper( {
    { "pgr_data_fifo1_mbe_log_0_2", { const_cast<uint32_t(*)>(&Pgr_data_fifo1_mbe_log_base->pgr_data_fifo1_mbe_log_0_2), 0, {}, sizeof(uint32_t) } },
    { "pgr_data_fifo1_mbe_log_1_2", { const_cast<uint32_t(*)>(&Pgr_data_fifo1_mbe_log_base->pgr_data_fifo1_mbe_log_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_data_fifo2_sbe_log_map: public RegisterMapper {
  static constexpr PTR_Pgr_data_fifo2_sbe_log Pgr_data_fifo2_sbe_log_base=0;
  Pgr_data_fifo2_sbe_log_map() : RegisterMapper( {
    { "pgr_data_fifo2_sbe_log_0_2", { const_cast<uint32_t(*)>(&Pgr_data_fifo2_sbe_log_base->pgr_data_fifo2_sbe_log_0_2), 0, {}, sizeof(uint32_t) } },
    { "pgr_data_fifo2_sbe_log_1_2", { const_cast<uint32_t(*)>(&Pgr_data_fifo2_sbe_log_base->pgr_data_fifo2_sbe_log_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_data_fifo2_mbe_log_map: public RegisterMapper {
  static constexpr PTR_Pgr_data_fifo2_mbe_log Pgr_data_fifo2_mbe_log_base=0;
  Pgr_data_fifo2_mbe_log_map() : RegisterMapper( {
    { "pgr_data_fifo2_mbe_log_0_2", { const_cast<uint32_t(*)>(&Pgr_data_fifo2_mbe_log_base->pgr_data_fifo2_mbe_log_0_2), 0, {}, sizeof(uint32_t) } },
    { "pgr_data_fifo2_mbe_log_1_2", { const_cast<uint32_t(*)>(&Pgr_data_fifo2_mbe_log_base->pgr_data_fifo2_mbe_log_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_ipb_pkt_err_ctr48_trigger_map: public RegisterMapper {
  static constexpr PTR_Pgr_ipb_pkt_err_ctr48_trigger Pgr_ipb_pkt_err_ctr48_trigger_base=0;
  Pgr_ipb_pkt_err_ctr48_trigger_map() : RegisterMapper( {
    { "ctr48_ipb_pkt_err_0_2", { const_cast<uint32_t(*)>(&Pgr_ipb_pkt_err_ctr48_trigger_base->ctr48_ipb_pkt_err_0_2), 0, {}, sizeof(uint32_t) } },
    { "ctr48_ipb_pkt_err_1_2", { const_cast<uint32_t(*)>(&Pgr_ipb_pkt_err_ctr48_trigger_base->ctr48_ipb_pkt_err_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_common_reg_rspec_map: public RegisterMapper {
  static constexpr PTR_Pgr_common_reg_rspec Pgr_common_reg_rspec_base=0;
  Pgr_common_reg_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)[0x4]>(&Pgr_common_reg_rspec_base->scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "recirc_ts", { const_cast<uint32_t(*)[0x4]>(&Pgr_common_reg_rspec_base->recirc_ts), 0, {0x4}, sizeof(uint32_t) } },
    { "csr_ts_offset", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->csr_ts_offset), 0, {}, sizeof(uint32_t) } },
    { "tbc_port_ctrl", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->tbc_port_ctrl), 0, {}, sizeof(uint32_t) } },
    { "ebuf_port_ctrl", { const_cast<uint32_t(*)[0x4]>(&Pgr_common_reg_rspec_base->ebuf_port_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "eth_cpu_port_ctrl", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->eth_cpu_port_ctrl), 0, {}, sizeof(uint32_t) } },
    { "ipb_port_ctrl", { const_cast<Pgr_ipb_port_ctrl(*)>(&Pgr_common_reg_rspec_base->ipb_port_ctrl), new Pgr_ipb_port_ctrl_map, {}, sizeof(Pgr_ipb_port_ctrl) } },
    { "pgen_port_down_mask", { const_cast<Pgr_pgen_port_down_event_mask(*)[0x2]>(&Pgr_common_reg_rspec_base->pgen_port_down_mask), new Pgr_pgen_port_down_event_mask_map, {0x2}, sizeof(Pgr_pgen_port_down_event_mask) } },
    { "port_down_dis", { const_cast<Pgr_port_down_dis(*)>(&Pgr_common_reg_rspec_base->port_down_dis), new Pgr_port_down_dis_map, {}, sizeof(Pgr_port_down_dis) } },
    { "pgen_retrigger_port_down", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->pgen_retrigger_port_down), 0, {}, sizeof(uint32_t) } },
    { "pgen_port_down_vec_clr", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->pgen_port_down_vec_clr), 0, {}, sizeof(uint32_t) } },
    { "pgen_port_down_ctrl", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->pgen_port_down_ctrl), 0, {}, sizeof(uint32_t) } },
    { "intr_stat", { const_cast<Pgr_intr_stat(*)>(&Pgr_common_reg_rspec_base->intr_stat), new Pgr_intr_stat_map, {}, sizeof(Pgr_intr_stat) } },
    { "intr_en0", { const_cast<Pgr_intr_en0(*)>(&Pgr_common_reg_rspec_base->intr_en0), new Pgr_intr_en0_map, {}, sizeof(Pgr_intr_en0) } },
    { "intr_en1", { const_cast<Pgr_intr_en1(*)>(&Pgr_common_reg_rspec_base->intr_en1), new Pgr_intr_en1_map, {}, sizeof(Pgr_intr_en1) } },
    { "freeze_en", { const_cast<Pgr_freeze_en(*)>(&Pgr_common_reg_rspec_base->freeze_en), new Pgr_freeze_en_map, {}, sizeof(Pgr_freeze_en) } },
    { "intr_inj", { const_cast<Pgr_intr_inj(*)>(&Pgr_common_reg_rspec_base->intr_inj), new Pgr_intr_inj_map, {}, sizeof(Pgr_intr_inj) } },
    { "sbe_log", { const_cast<Pgr_sbe_log(*)>(&Pgr_common_reg_rspec_base->sbe_log), new Pgr_sbe_log_map, {}, sizeof(Pgr_sbe_log) } },
    { "mbe_log", { const_cast<Pgr_mbe_log(*)>(&Pgr_common_reg_rspec_base->mbe_log), new Pgr_mbe_log_map, {}, sizeof(Pgr_mbe_log) } },
    { "pgen_chnl_log", { const_cast<Pgr_pgen_chnl_log(*)>(&Pgr_common_reg_rspec_base->pgen_chnl_log), new Pgr_pgen_chnl_log_map, {}, sizeof(Pgr_pgen_chnl_log) } },
    { "pgen_pfc_log", { const_cast<Pgr_pgen_pfc_log(*)>(&Pgr_common_reg_rspec_base->pgen_pfc_log), new Pgr_pgen_pfc_log_map, {}, sizeof(Pgr_pgen_pfc_log) } },
    { "pgen_port_down_log", { const_cast<Pgr_pgen_port_down_log(*)[0x2]>(&Pgr_common_reg_rspec_base->pgen_port_down_log), new Pgr_pgen_port_down_log_map, {0x2}, sizeof(Pgr_pgen_port_down_log) } },
    { "recir_log", { const_cast<Pgr_recir_log(*)>(&Pgr_common_reg_rspec_base->recir_log), new Pgr_recir_log_map, {}, sizeof(Pgr_recir_log) } },
    { "cfg_pgen_dwrr_weight", { const_cast<uint32_t(*)[0x10]>(&Pgr_common_reg_rspec_base->cfg_pgen_dwrr_weight), 0, {0x10}, sizeof(uint32_t) } },
    { "ebuf_credit_ctrl", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->ebuf_credit_ctrl), 0, {}, sizeof(uint32_t) } },
    { "ipb_chnl_pfc_cfg", { const_cast<Pgr_ipb_chnl_pfc_cfg(*)>(&Pgr_common_reg_rspec_base->ipb_chnl_pfc_cfg), new Pgr_ipb_chnl_pfc_cfg_map, {}, sizeof(Pgr_ipb_chnl_pfc_cfg) } },
    { "ipb_chnl_xoff_cfg", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->ipb_chnl_xoff_cfg), 0, {}, sizeof(uint32_t) } },
    { "eth_cpu_chnl_pfc_cfg", { const_cast<Pgr_eth_cpu_chnl_pfc_cfg(*)>(&Pgr_common_reg_rspec_base->eth_cpu_chnl_pfc_cfg), new Pgr_eth_cpu_chnl_pfc_cfg_map, {}, sizeof(Pgr_eth_cpu_chnl_pfc_cfg) } },
    { "tbc_chnl_pfc_cfg", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->tbc_chnl_pfc_cfg), 0, {}, sizeof(uint32_t) } },
    { "cfg_pfc_hdr", { const_cast<Pgr_cfg_pfc_hdr(*)>(&Pgr_common_reg_rspec_base->cfg_pfc_hdr), new Pgr_cfg_pfc_hdr_map, {}, sizeof(Pgr_cfg_pfc_hdr) } },
    { "cfg_pfc_timer", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->cfg_pfc_timer), 0, {}, sizeof(uint32_t) } },
    { "cfg_pfc_max_pkt_size", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->cfg_pfc_max_pkt_size), 0, {}, sizeof(uint32_t) } },
    { "cfg_init_tbc_credit", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->cfg_init_tbc_credit), 0, {}, sizeof(uint32_t) } },
    { "cfg_init_eth_cpu_credit", { const_cast<uint32_t(*)[0x4]>(&Pgr_common_reg_rspec_base->cfg_init_eth_cpu_credit), 0, {0x4}, sizeof(uint32_t) } },
    { "cfg_pgen_dwrr", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->cfg_pgen_dwrr), 0, {}, sizeof(uint32_t) } },
    { "cfg_pgen_chnl_ts", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->cfg_pgen_chnl_ts), 0, {}, sizeof(uint32_t) } },
    { "cfg_pgen_tdm_ts", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->cfg_pgen_tdm_ts), 0, {}, sizeof(uint32_t) } },
    { "cfg_app_recirc_src", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->cfg_app_recirc_src), 0, {}, sizeof(uint32_t) } },
    { "cfg_hi_prio", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->cfg_hi_prio), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "pgen_ctrl", { const_cast<Pgr_pgen_ctrl(*)>(&Pgr_common_reg_rspec_base->pgen_ctrl), new Pgr_pgen_ctrl_map, {}, sizeof(Pgr_pgen_ctrl) } },
    { "pgr_data_fifo0_sbe_log", { const_cast<Pgr_data_fifo0_sbe_log(*)>(&Pgr_common_reg_rspec_base->pgr_data_fifo0_sbe_log), new Pgr_data_fifo0_sbe_log_map, {}, sizeof(Pgr_data_fifo0_sbe_log) } },
    { "pgr_data_fifo0_mbe_log", { const_cast<Pgr_data_fifo0_mbe_log(*)>(&Pgr_common_reg_rspec_base->pgr_data_fifo0_mbe_log), new Pgr_data_fifo0_mbe_log_map, {}, sizeof(Pgr_data_fifo0_mbe_log) } },
    { "pgr_data_fifo1_sbe_log", { const_cast<Pgr_data_fifo1_sbe_log(*)>(&Pgr_common_reg_rspec_base->pgr_data_fifo1_sbe_log), new Pgr_data_fifo1_sbe_log_map, {}, sizeof(Pgr_data_fifo1_sbe_log) } },
    { "pgr_data_fifo1_mbe_log", { const_cast<Pgr_data_fifo1_mbe_log(*)>(&Pgr_common_reg_rspec_base->pgr_data_fifo1_mbe_log), new Pgr_data_fifo1_mbe_log_map, {}, sizeof(Pgr_data_fifo1_mbe_log) } },
    { "pgr_data_fifo2_sbe_log", { const_cast<Pgr_data_fifo2_sbe_log(*)>(&Pgr_common_reg_rspec_base->pgr_data_fifo2_sbe_log), new Pgr_data_fifo2_sbe_log_map, {}, sizeof(Pgr_data_fifo2_sbe_log) } },
    { "pgr_data_fifo2_mbe_log", { const_cast<Pgr_data_fifo2_mbe_log(*)>(&Pgr_common_reg_rspec_base->pgr_data_fifo2_mbe_log), new Pgr_data_fifo2_mbe_log_map, {}, sizeof(Pgr_data_fifo2_mbe_log) } },
    { "ctr48_ipb_pkt_err", { const_cast<Pgr_ipb_pkt_err_ctr48_trigger(*)>(&Pgr_common_reg_rspec_base->ctr48_ipb_pkt_err), new Pgr_ipb_pkt_err_ctr48_trigger_map, {}, sizeof(Pgr_ipb_pkt_err_ctr48_trigger) } },
    { "ebuf_pfc_cfg", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->ebuf_pfc_cfg), 0, {}, sizeof(uint32_t) } },
    { "tbc_credit_log", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->tbc_credit_log), 0, {}, sizeof(uint32_t) } },
    { "eth_cpu_credit_log", { const_cast<uint32_t(*)[0x4]>(&Pgr_common_reg_rspec_base->eth_cpu_credit_log), 0, {0x4}, sizeof(uint32_t) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Pgr_common_reg_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_reg_rspec_map: public RegisterMapper {
  static constexpr PTR_Pgr_reg_rspec Pgr_reg_rspec_base=0;
  Pgr_reg_rspec_map() : RegisterMapper( {
    { "pgr_app", { &Pgr_reg_rspec_base->pgr_app, new Pgr_app_reg_rspec_map, {0x10}, sizeof(Pgr_app_reg_rspec) } },
    { "pgr_common", { &Pgr_reg_rspec_base->pgr_common, new Pgr_common_reg_rspec_map, {}, sizeof(Pgr_common_reg_rspec) } }
    } )
  {}
};

struct Ebuf400_glb_ctrl_map: public RegisterMapper {
  static constexpr PTR_Ebuf400_glb_ctrl Ebuf400_glb_ctrl_base=0;
  Ebuf400_glb_ctrl_map() : RegisterMapper( {
    { "glb_ctrl_0_2", { const_cast<uint32_t(*)>(&Ebuf400_glb_ctrl_base->glb_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "glb_ctrl_1_2", { const_cast<uint32_t(*)>(&Ebuf400_glb_ctrl_base->glb_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebuf400_int_log_group_map: public RegisterMapper {
  static constexpr PTR_Ebuf400_int_log_group Ebuf400_int_log_group_base=0;
  Ebuf400_int_log_group_map() : RegisterMapper( {
    { "warp_mem_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->warp_mem_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "warp_mem_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->warp_mem_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "dprsr_mem_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->dprsr_mem_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "dprsr_mem_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->dprsr_mem_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "mac_mem_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->mac_mem_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "mac_mem_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->mac_mem_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "warp_fifo_overflow_err", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->warp_fifo_overflow_err), 0, {}, sizeof(uint32_t) } },
    { "dprsr_fifo_overflow_err", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->dprsr_fifo_overflow_err), 0, {}, sizeof(uint32_t) } },
    { "warp_protocol_err", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->warp_protocol_err), 0, {}, sizeof(uint32_t) } },
    { "dprsr_protocol_err", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->dprsr_protocol_err), 0, {}, sizeof(uint32_t) } },
    { "dprsr_cfifo_overflow_err", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->dprsr_cfifo_overflow_err), 0, {}, sizeof(uint32_t) } },
    { "dprsr_cfifo_underflow_err", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->dprsr_cfifo_underflow_err), 0, {}, sizeof(uint32_t) } },
    { "warp_cfifo_overflow_err", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->warp_cfifo_overflow_err), 0, {}, sizeof(uint32_t) } },
    { "warp_cfifo_underflow_err", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->warp_cfifo_underflow_err), 0, {}, sizeof(uint32_t) } },
    { "mac_cfifo_overflow_err", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->mac_cfifo_overflow_err), 0, {}, sizeof(uint32_t) } },
    { "mac_cfifo_underflow_err", { const_cast<uint32_t(*)>(&Ebuf400_int_log_group_base->mac_cfifo_underflow_err), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebuf400_glb_group_map: public RegisterMapper {
  static constexpr PTR_Ebuf400_glb_group Ebuf400_glb_group_base=0;
  Ebuf400_glb_group_map() : RegisterMapper( {
    { "port_en_dprsr", { const_cast<uint32_t(*)>(&Ebuf400_glb_group_base->port_en_dprsr), 0, {}, sizeof(uint32_t) } },
    { "port_en_warp", { const_cast<uint32_t(*)>(&Ebuf400_glb_group_base->port_en_warp), 0, {}, sizeof(uint32_t) } },
    { "glb_ctrl", { const_cast<Ebuf400_glb_ctrl(*)>(&Ebuf400_glb_group_base->glb_ctrl), new Ebuf400_glb_ctrl_map, {}, sizeof(Ebuf400_glb_ctrl) } },
    { "intr_stat", { const_cast<uint32_t(*)>(&Ebuf400_glb_group_base->intr_stat), 0, {}, sizeof(uint32_t) } },
    { "intr_log_group", { &Ebuf400_glb_group_base->intr_log_group, new Ebuf400_int_log_group_map, {}, sizeof(Ebuf400_int_log_group) } },
    { "intr_en0", { const_cast<uint32_t(*)>(&Ebuf400_glb_group_base->intr_en0), 0, {}, sizeof(uint32_t) } },
    { "intr_en1", { const_cast<uint32_t(*)>(&Ebuf400_glb_group_base->intr_en1), 0, {}, sizeof(uint32_t) } },
    { "freeze_en", { const_cast<uint32_t(*)>(&Ebuf400_glb_group_base->freeze_en), 0, {}, sizeof(uint32_t) } },
    { "ecc_inj", { const_cast<uint32_t(*)>(&Ebuf400_glb_group_base->ecc_inj), 0, {}, sizeof(uint32_t) } },
    { "ecc_dis", { const_cast<uint32_t(*)>(&Ebuf400_glb_group_base->ecc_dis), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Ebuf400_glb_group_base->dft_csr), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebuf400_chnl_fifo_ctrl_map: public RegisterMapper {
  static constexpr PTR_Ebuf400_chnl_fifo_ctrl Ebuf400_chnl_fifo_ctrl_base=0;
  Ebuf400_chnl_fifo_ctrl_map() : RegisterMapper( {
    { "chnl_fifo_ctrl_0_2", { const_cast<uint32_t(*)>(&Ebuf400_chnl_fifo_ctrl_base->chnl_fifo_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_ctrl_1_2", { const_cast<uint32_t(*)>(&Ebuf400_chnl_fifo_ctrl_base->chnl_fifo_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebuf400_chnl_fifo_stat_map: public RegisterMapper {
  static constexpr PTR_Ebuf400_chnl_fifo_stat Ebuf400_chnl_fifo_stat_base=0;
  Ebuf400_chnl_fifo_stat_map() : RegisterMapper( {
    { "chnl_fifo_stat_0_2", { const_cast<uint32_t(*)>(&Ebuf400_chnl_fifo_stat_base->chnl_fifo_stat_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_stat_1_2", { const_cast<uint32_t(*)>(&Ebuf400_chnl_fifo_stat_base->chnl_fifo_stat_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebuf400_chnl_pktnum_map: public RegisterMapper {
  static constexpr PTR_Ebuf400_chnl_pktnum Ebuf400_chnl_pktnum_base=0;
  Ebuf400_chnl_pktnum_map() : RegisterMapper( {
    { "chnl_pktnum_0_14", { const_cast<uint32_t(*)>(&Ebuf400_chnl_pktnum_base->chnl_pktnum_0_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_1_14", { const_cast<uint32_t(*)>(&Ebuf400_chnl_pktnum_base->chnl_pktnum_1_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_2_14", { const_cast<uint32_t(*)>(&Ebuf400_chnl_pktnum_base->chnl_pktnum_2_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_3_14", { const_cast<uint32_t(*)>(&Ebuf400_chnl_pktnum_base->chnl_pktnum_3_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_4_14", { const_cast<uint32_t(*)>(&Ebuf400_chnl_pktnum_base->chnl_pktnum_4_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_5_14", { const_cast<uint32_t(*)>(&Ebuf400_chnl_pktnum_base->chnl_pktnum_5_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_6_14", { const_cast<uint32_t(*)>(&Ebuf400_chnl_pktnum_base->chnl_pktnum_6_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_7_14", { const_cast<uint32_t(*)>(&Ebuf400_chnl_pktnum_base->chnl_pktnum_7_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_8_14", { const_cast<uint32_t(*)>(&Ebuf400_chnl_pktnum_base->chnl_pktnum_8_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_9_14", { const_cast<uint32_t(*)>(&Ebuf400_chnl_pktnum_base->chnl_pktnum_9_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_10_14", { const_cast<uint32_t(*)>(&Ebuf400_chnl_pktnum_base->chnl_pktnum_10_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_11_14", { const_cast<uint32_t(*)>(&Ebuf400_chnl_pktnum_base->chnl_pktnum_11_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_12_14", { const_cast<uint32_t(*)>(&Ebuf400_chnl_pktnum_base->chnl_pktnum_12_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_13_14", { const_cast<uint32_t(*)>(&Ebuf400_chnl_pktnum_base->chnl_pktnum_13_14), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebuf400_chan_group_map: public RegisterMapper {
  static constexpr PTR_Ebuf400_chan_group Ebuf400_chan_group_base=0;
  Ebuf400_chan_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<uint32_t(*)>(&Ebuf400_chan_group_base->chnl_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Ebuf400_chan_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_ctrl", { const_cast<Ebuf400_chnl_fifo_ctrl(*)>(&Ebuf400_chan_group_base->chnl_fifo_ctrl), new Ebuf400_chnl_fifo_ctrl_map, {}, sizeof(Ebuf400_chnl_fifo_ctrl) } },
    { "chnl_fifo_stat", { const_cast<Ebuf400_chnl_fifo_stat(*)>(&Ebuf400_chan_group_base->chnl_fifo_stat), new Ebuf400_chnl_fifo_stat_map, {}, sizeof(Ebuf400_chnl_fifo_stat) } },
    { "chnl_pktnum", { const_cast<Ebuf400_chnl_pktnum(*)>(&Ebuf400_chan_group_base->chnl_pktnum), new Ebuf400_chnl_pktnum_map, {}, sizeof(Ebuf400_chnl_pktnum) } }
    } )
  {}
};

struct Ebuf400_rspec_map: public RegisterMapper {
  static constexpr PTR_Ebuf400_rspec Ebuf400_rspec_base=0;
  Ebuf400_rspec_map() : RegisterMapper( {
    { "glb_group", { &Ebuf400_rspec_base->glb_group, new Ebuf400_glb_group_map, {}, sizeof(Ebuf400_glb_group) } },
    { "chan_group", { &Ebuf400_rspec_base->chan_group, new Ebuf400_chan_group_map, {0x8}, sizeof(Ebuf400_chan_group) } },
    { "scratch", { const_cast<uint32_t(*)>(&Ebuf400_rspec_base->scratch), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebuf100_glb_ctrl_map: public RegisterMapper {
  static constexpr PTR_Ebuf100_glb_ctrl Ebuf100_glb_ctrl_base=0;
  Ebuf100_glb_ctrl_map() : RegisterMapper( {
    { "glb_ctrl_0_2", { const_cast<uint32_t(*)>(&Ebuf100_glb_ctrl_base->glb_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "glb_ctrl_1_2", { const_cast<uint32_t(*)>(&Ebuf100_glb_ctrl_base->glb_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebuf100_int_log_group_map: public RegisterMapper {
  static constexpr PTR_Ebuf100_int_log_group Ebuf100_int_log_group_base=0;
  Ebuf100_int_log_group_map() : RegisterMapper( {
    { "warp_mem_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->warp_mem_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "warp_mem_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->warp_mem_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "dprsr_mem_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->dprsr_mem_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "dprsr_mem_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->dprsr_mem_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "mac_mem_ecc_dual_err_addr", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->mac_mem_ecc_dual_err_addr), 0, {}, sizeof(uint32_t) } },
    { "mac_mem_ecc_sngl_err_addr", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->mac_mem_ecc_sngl_err_addr), 0, {}, sizeof(uint32_t) } },
    { "warp_fifo_overflow_err", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->warp_fifo_overflow_err), 0, {}, sizeof(uint32_t) } },
    { "dprsr_fifo_overflow_err", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->dprsr_fifo_overflow_err), 0, {}, sizeof(uint32_t) } },
    { "warp_protocol_err", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->warp_protocol_err), 0, {}, sizeof(uint32_t) } },
    { "dprsr_protocol_err", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->dprsr_protocol_err), 0, {}, sizeof(uint32_t) } },
    { "dprsr_cfifo_overflow_err", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->dprsr_cfifo_overflow_err), 0, {}, sizeof(uint32_t) } },
    { "dprsr_cfifo_underflow_err", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->dprsr_cfifo_underflow_err), 0, {}, sizeof(uint32_t) } },
    { "warp_cfifo_overflow_err", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->warp_cfifo_overflow_err), 0, {}, sizeof(uint32_t) } },
    { "warp_cfifo_underflow_err", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->warp_cfifo_underflow_err), 0, {}, sizeof(uint32_t) } },
    { "mac_cfifo_overflow_err", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->mac_cfifo_overflow_err), 0, {}, sizeof(uint32_t) } },
    { "mac_cfifo_underflow_err", { const_cast<uint32_t(*)>(&Ebuf100_int_log_group_base->mac_cfifo_underflow_err), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebuf100_glb_group_map: public RegisterMapper {
  static constexpr PTR_Ebuf100_glb_group Ebuf100_glb_group_base=0;
  Ebuf100_glb_group_map() : RegisterMapper( {
    { "port_en_dprsr", { const_cast<uint32_t(*)>(&Ebuf100_glb_group_base->port_en_dprsr), 0, {}, sizeof(uint32_t) } },
    { "port_en_warp", { const_cast<uint32_t(*)>(&Ebuf100_glb_group_base->port_en_warp), 0, {}, sizeof(uint32_t) } },
    { "glb_ctrl", { const_cast<Ebuf100_glb_ctrl(*)>(&Ebuf100_glb_group_base->glb_ctrl), new Ebuf100_glb_ctrl_map, {}, sizeof(Ebuf100_glb_ctrl) } },
    { "intr_stat", { const_cast<uint32_t(*)>(&Ebuf100_glb_group_base->intr_stat), 0, {}, sizeof(uint32_t) } },
    { "intr_log_group", { &Ebuf100_glb_group_base->intr_log_group, new Ebuf100_int_log_group_map, {}, sizeof(Ebuf100_int_log_group) } },
    { "intr_en0", { const_cast<uint32_t(*)>(&Ebuf100_glb_group_base->intr_en0), 0, {}, sizeof(uint32_t) } },
    { "intr_en1", { const_cast<uint32_t(*)>(&Ebuf100_glb_group_base->intr_en1), 0, {}, sizeof(uint32_t) } },
    { "freeze_en", { const_cast<uint32_t(*)>(&Ebuf100_glb_group_base->freeze_en), 0, {}, sizeof(uint32_t) } },
    { "ecc_inj", { const_cast<uint32_t(*)>(&Ebuf100_glb_group_base->ecc_inj), 0, {}, sizeof(uint32_t) } },
    { "ecc_dis", { const_cast<uint32_t(*)>(&Ebuf100_glb_group_base->ecc_dis), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Ebuf100_glb_group_base->dft_csr), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebuf100_chnl_fifo_ctrl_map: public RegisterMapper {
  static constexpr PTR_Ebuf100_chnl_fifo_ctrl Ebuf100_chnl_fifo_ctrl_base=0;
  Ebuf100_chnl_fifo_ctrl_map() : RegisterMapper( {
    { "chnl_fifo_ctrl_0_2", { const_cast<uint32_t(*)>(&Ebuf100_chnl_fifo_ctrl_base->chnl_fifo_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_ctrl_1_2", { const_cast<uint32_t(*)>(&Ebuf100_chnl_fifo_ctrl_base->chnl_fifo_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebuf100_chnl_fifo_stat_map: public RegisterMapper {
  static constexpr PTR_Ebuf100_chnl_fifo_stat Ebuf100_chnl_fifo_stat_base=0;
  Ebuf100_chnl_fifo_stat_map() : RegisterMapper( {
    { "chnl_fifo_stat_0_2", { const_cast<uint32_t(*)>(&Ebuf100_chnl_fifo_stat_base->chnl_fifo_stat_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_stat_1_2", { const_cast<uint32_t(*)>(&Ebuf100_chnl_fifo_stat_base->chnl_fifo_stat_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebuf100_chnl_pktnum_map: public RegisterMapper {
  static constexpr PTR_Ebuf100_chnl_pktnum Ebuf100_chnl_pktnum_base=0;
  Ebuf100_chnl_pktnum_map() : RegisterMapper( {
    { "chnl_pktnum_0_14", { const_cast<uint32_t(*)>(&Ebuf100_chnl_pktnum_base->chnl_pktnum_0_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_1_14", { const_cast<uint32_t(*)>(&Ebuf100_chnl_pktnum_base->chnl_pktnum_1_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_2_14", { const_cast<uint32_t(*)>(&Ebuf100_chnl_pktnum_base->chnl_pktnum_2_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_3_14", { const_cast<uint32_t(*)>(&Ebuf100_chnl_pktnum_base->chnl_pktnum_3_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_4_14", { const_cast<uint32_t(*)>(&Ebuf100_chnl_pktnum_base->chnl_pktnum_4_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_5_14", { const_cast<uint32_t(*)>(&Ebuf100_chnl_pktnum_base->chnl_pktnum_5_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_6_14", { const_cast<uint32_t(*)>(&Ebuf100_chnl_pktnum_base->chnl_pktnum_6_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_7_14", { const_cast<uint32_t(*)>(&Ebuf100_chnl_pktnum_base->chnl_pktnum_7_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_8_14", { const_cast<uint32_t(*)>(&Ebuf100_chnl_pktnum_base->chnl_pktnum_8_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_9_14", { const_cast<uint32_t(*)>(&Ebuf100_chnl_pktnum_base->chnl_pktnum_9_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_10_14", { const_cast<uint32_t(*)>(&Ebuf100_chnl_pktnum_base->chnl_pktnum_10_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_11_14", { const_cast<uint32_t(*)>(&Ebuf100_chnl_pktnum_base->chnl_pktnum_11_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_12_14", { const_cast<uint32_t(*)>(&Ebuf100_chnl_pktnum_base->chnl_pktnum_12_14), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum_13_14", { const_cast<uint32_t(*)>(&Ebuf100_chnl_pktnum_base->chnl_pktnum_13_14), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebuf100_chan_group_map: public RegisterMapper {
  static constexpr PTR_Ebuf100_chan_group Ebuf100_chan_group_base=0;
  Ebuf100_chan_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<uint32_t(*)>(&Ebuf100_chan_group_base->chnl_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_stat", { const_cast<uint32_t(*)>(&Ebuf100_chan_group_base->chnl_stat), 0, {}, sizeof(uint32_t) } },
    { "chnl_fifo_ctrl", { const_cast<Ebuf100_chnl_fifo_ctrl(*)>(&Ebuf100_chan_group_base->chnl_fifo_ctrl), new Ebuf100_chnl_fifo_ctrl_map, {}, sizeof(Ebuf100_chnl_fifo_ctrl) } },
    { "chnl_fifo_stat", { const_cast<Ebuf100_chnl_fifo_stat(*)>(&Ebuf100_chan_group_base->chnl_fifo_stat), new Ebuf100_chnl_fifo_stat_map, {}, sizeof(Ebuf100_chnl_fifo_stat) } },
    { "chnl_pktnum", { const_cast<Ebuf100_chnl_pktnum(*)>(&Ebuf100_chan_group_base->chnl_pktnum), new Ebuf100_chnl_pktnum_map, {}, sizeof(Ebuf100_chnl_pktnum) } }
    } )
  {}
};

struct Ebuf100_rspec_map: public RegisterMapper {
  static constexpr PTR_Ebuf100_rspec Ebuf100_rspec_base=0;
  Ebuf100_rspec_map() : RegisterMapper( {
    { "glb_group", { &Ebuf100_rspec_base->glb_group, new Ebuf100_glb_group_map, {}, sizeof(Ebuf100_glb_group) } },
    { "chan_group", { &Ebuf100_rspec_base->chan_group, new Ebuf100_chan_group_map, {0x2}, sizeof(Ebuf100_chan_group) } },
    { "scratch", { const_cast<uint32_t(*)>(&Ebuf100_rspec_base->scratch), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebuf900_reg_map: public RegisterMapper {
  static constexpr PTR_Ebuf900_reg Ebuf900_reg_base=0;
  Ebuf900_reg_map() : RegisterMapper( {
    { "ebuf400reg", { &Ebuf900_reg_base->ebuf400reg, new Ebuf400_rspec_map, {0x2}, sizeof(Ebuf400_rspec) } },
    { "ebuf100reg", { &Ebuf900_reg_base->ebuf100reg, new Ebuf100_rspec_map, {}, sizeof(Ebuf100_rspec) } }
    } )
  {}
};

struct Parde_glue_stn_reg_map: public RegisterMapper {
  static constexpr PTR_Parde_glue_stn_reg Parde_glue_stn_reg_base=0;
  Parde_glue_stn_reg_map() : RegisterMapper( {
    { "ipbprsr4reg", { &Parde_glue_stn_reg_base->ipbprsr4reg, new Ipb_prsr4_reg_map, {0x9}, sizeof(Ipb_prsr4_reg) } },
    { "pmergereg", { &Parde_glue_stn_reg_base->pmergereg, new Pmerge_reg_map, {}, sizeof(Pmerge_reg) } },
    { "parbreg", { &Parde_glue_stn_reg_base->parbreg, new Parb_reg_map, {}, sizeof(Parb_reg) } },
    { "s2preg", { &Parde_glue_stn_reg_base->s2preg, new S2p_reg_map, {}, sizeof(S2p_reg) } },
    { "p2sreg", { &Parde_glue_stn_reg_base->p2sreg, new P2s_reg_map, {}, sizeof(P2s_reg) } },
    { "pgluereg", { &Parde_glue_stn_reg_base->pgluereg, new Parde_glue_reg_rspec_map, {}, sizeof(Parde_glue_reg_rspec) } },
    { "epbprsr4reg", { &Parde_glue_stn_reg_base->epbprsr4reg, new Epb_prsr4_reg_map, {0x9}, sizeof(Epb_prsr4_reg) } },
    { "pbusreg", { &Parde_glue_stn_reg_base->pbusreg, new Pbus_station_regs_rspec_map, {}, sizeof(Pbus_station_regs_rspec) } },
    { "pgrreg", { &Parde_glue_stn_reg_base->pgrreg, new Pgr_reg_rspec_map, {}, sizeof(Pgr_reg_rspec) } },
    { "ebuf900reg", { &Parde_glue_stn_reg_base->ebuf900reg, new Ebuf900_reg_map, {0x4}, sizeof(Ebuf900_reg) } }
    } )
  {}
};

struct Mirror_s2p_regs_intr_map: public RegisterMapper {
  static constexpr PTR_Mirror_s2p_regs_intr Mirror_s2p_regs_intr_base=0;
  Mirror_s2p_regs_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mirror_s2p_cnt48_inc_map: public RegisterMapper {
  static constexpr PTR_Mirror_s2p_cnt48_inc Mirror_s2p_cnt48_inc_base=0;
  Mirror_s2p_cnt48_inc_map() : RegisterMapper( {
    { "ctr48_0_2", { const_cast<uint32_t(*)>(&Mirror_s2p_cnt48_inc_base->ctr48_0_2), 0, {}, sizeof(uint32_t) } },
    { "ctr48_1_2", { const_cast<uint32_t(*)>(&Mirror_s2p_cnt48_inc_base->ctr48_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mirror_s2p_regs_map: public RegisterMapper {
  static constexpr PTR_Mirror_s2p_regs Mirror_s2p_regs_base=0;
  Mirror_s2p_regs_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "s2p_credit_cfg", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_base->s2p_credit_cfg), 0, {}, sizeof(uint32_t) } },
    { "sess_entry_word0", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_base->sess_entry_word0), 0, {}, sizeof(uint32_t) } },
    { "sess_entry_word1", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_base->sess_entry_word1), 0, {}, sizeof(uint32_t) } },
    { "sess_entry_word2", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_base->sess_entry_word2), 0, {}, sizeof(uint32_t) } },
    { "sess_entry_word3", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_base->sess_entry_word3), 0, {}, sizeof(uint32_t) } },
    { "sess_entry_word4", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_base->sess_entry_word4), 0, {}, sizeof(uint32_t) } },
    { "coal_to_interval", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_base->coal_to_interval), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Mirror_s2p_regs_base->intr, new Mirror_s2p_regs_intr_map, {}, sizeof(Mirror_s2p_regs_intr) } },
    { "s2p_session_sbe_err_log", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_base->s2p_session_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "s2p_session_mbe_err_log", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_base->s2p_session_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "s2p_session_ecc_ctrl", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_base->s2p_session_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "mirr_crc_ctrl", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_base->mirr_crc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "ctr48", { const_cast<Mirror_s2p_cnt48_inc(*)>(&Mirror_s2p_regs_base->ctr48), new Mirror_s2p_cnt48_inc_map, {}, sizeof(Mirror_s2p_cnt48_inc) } },
    { "dprsr_bubble", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_base->dprsr_bubble), 0, {}, sizeof(uint32_t) } },
    { "curr_s2p", { const_cast<uint32_t(*)>(&Mirror_s2p_regs_base->curr_s2p), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct S2p_coal_hdr_r_map: public RegisterMapper {
  static constexpr PTR_S2p_coal_hdr_r S2p_coal_hdr_r_base=0;
  S2p_coal_hdr_r_map() : RegisterMapper( {
    { "coal_hdr_tbl_0_4", { const_cast<uint32_t(*)>(&S2p_coal_hdr_r_base->coal_hdr_tbl_0_4), 0, {}, sizeof(uint32_t) } },
    { "coal_hdr_tbl_1_4", { const_cast<uint32_t(*)>(&S2p_coal_hdr_r_base->coal_hdr_tbl_1_4), 0, {}, sizeof(uint32_t) } },
    { "coal_hdr_tbl_2_4", { const_cast<uint32_t(*)>(&S2p_coal_hdr_r_base->coal_hdr_tbl_2_4), 0, {}, sizeof(uint32_t) } },
    { "coal_hdr_tbl_3_4", { const_cast<uint32_t(*)>(&S2p_coal_hdr_r_base->coal_hdr_tbl_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mirror_s2p_coal_map_map: public RegisterMapper {
  static constexpr PTR_Mirror_s2p_coal_map Mirror_s2p_coal_map_base=0;
  Mirror_s2p_coal_map_map() : RegisterMapper( {
    { "coal_hdr_tbl", { const_cast<S2p_coal_hdr_r(*)[0x10]>(&Mirror_s2p_coal_map_base->coal_hdr_tbl), new S2p_coal_hdr_r_map, {0x10}, sizeof(S2p_coal_hdr_r) } }
    } )
  {}
};

struct Mirror_s2p_session_map_map: public RegisterMapper {
  static constexpr PTR_Mirror_s2p_session_map Mirror_s2p_session_map_base=0;
  Mirror_s2p_session_map_map() : RegisterMapper( {
    { "tbl0", { const_cast<uint32_t(*)[0x100]>(&Mirror_s2p_session_map_base->tbl0), 0, {0x100}, sizeof(uint32_t) } },
    { "tbl1", { const_cast<uint32_t(*)[0x100]>(&Mirror_s2p_session_map_base->tbl1), 0, {0x100}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mirror_sess_cfg_map: public RegisterMapper {
  static constexpr PTR_Mirror_sess_cfg Mirror_sess_cfg_base=0;
  Mirror_sess_cfg_map() : RegisterMapper( {
    { "entry", { const_cast<uint32_t(*)[0x100]>(&Mirror_sess_cfg_base->entry), 0, {0x100}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mirror_coal_sess_cfg_map: public RegisterMapper {
  static constexpr PTR_Mirror_coal_sess_cfg Mirror_coal_sess_cfg_base=0;
  Mirror_coal_sess_cfg_map() : RegisterMapper( {
    { "entry", { const_cast<uint32_t(*)[0x10]>(&Mirror_coal_sess_cfg_base->entry), 0, {0x10}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mirror_coal_to_cfg_map: public RegisterMapper {
  static constexpr PTR_Mirror_coal_to_cfg Mirror_coal_to_cfg_base=0;
  Mirror_coal_to_cfg_map() : RegisterMapper( {
    { "entry", { const_cast<uint32_t(*)[0x10]>(&Mirror_coal_to_cfg_base->entry), 0, {0x10}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mirror_slice_mem_map: public RegisterMapper {
  static constexpr PTR_Mirror_slice_mem Mirror_slice_mem_base=0;
  Mirror_slice_mem_map() : RegisterMapper( {
    { "sess_cfg", { &Mirror_slice_mem_base->sess_cfg, new Mirror_sess_cfg_map, {}, sizeof(Mirror_sess_cfg) } },
    { "coal_sess_cfg", { &Mirror_slice_mem_base->coal_sess_cfg, new Mirror_coal_sess_cfg_map, {}, sizeof(Mirror_coal_sess_cfg) } },
    { "coal_to_cfg", { &Mirror_slice_mem_base->coal_to_cfg, new Mirror_coal_to_cfg_map, {}, sizeof(Mirror_coal_to_cfg) } }
    } )
  {}
};

struct Mirror_cnt48_inc_map: public RegisterMapper {
  static constexpr PTR_Mirror_cnt48_inc Mirror_cnt48_inc_base=0;
  Mirror_cnt48_inc_map() : RegisterMapper( {
    { "mirror_out_pkt_cnt_0_2", { const_cast<uint32_t(*)>(&Mirror_cnt48_inc_base->mirror_out_pkt_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "mirror_out_pkt_cnt_1_2", { const_cast<uint32_t(*)>(&Mirror_cnt48_inc_base->mirror_out_pkt_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mirror_slice_regs_intr_map: public RegisterMapper {
  static constexpr PTR_Mirror_slice_regs_intr Mirror_slice_regs_intr_base=0;
  Mirror_slice_regs_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Mirror_slice_regs_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Mirror_slice_regs_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Mirror_slice_regs_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Mirror_slice_regs_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Mirror_slice_regs_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mirror_slice_regs_map: public RegisterMapper {
  static constexpr PTR_Mirror_slice_regs Mirror_slice_regs_base=0;
  Mirror_slice_regs_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Mirror_slice_regs_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "slice_ctr", { const_cast<uint32_t(*)>(&Mirror_slice_regs_base->slice_ctr), 0, {}, sizeof(uint32_t) } },
    { "dbuff_wm", { const_cast<uint32_t(*)[0x5]>(&Mirror_slice_regs_base->dbuff_wm), 0, {0x5}, sizeof(uint32_t) } },
    { "dbuff_cnt", { const_cast<uint32_t(*)[0x5]>(&Mirror_slice_regs_base->dbuff_cnt), 0, {0x5}, sizeof(uint32_t) } },
    { "dbuff_th", { const_cast<uint32_t(*)>(&Mirror_slice_regs_base->dbuff_th), 0, {}, sizeof(uint32_t) } },
    { "qdepth", { const_cast<uint32_t(*)[0x36]>(&Mirror_slice_regs_base->qdepth), 0, {0x36}, sizeof(uint32_t) } },
    { "mirror_out_pkt_cnt", { const_cast<Mirror_cnt48_inc(*)[0x5]>(&Mirror_slice_regs_base->mirror_out_pkt_cnt), new Mirror_cnt48_inc_map, {0x5}, sizeof(Mirror_cnt48_inc) } },
    { "mirror_egress_in_pk_cnt", { const_cast<Mirror_cnt48_inc(*)>(&Mirror_slice_regs_base->mirror_egress_in_pk_cnt), new Mirror_cnt48_inc_map, {}, sizeof(Mirror_cnt48_inc) } },
    { "mirror_ingress_in_pk_cnt", { const_cast<Mirror_cnt48_inc(*)>(&Mirror_slice_regs_base->mirror_ingress_in_pk_cnt), new Mirror_cnt48_inc_map, {}, sizeof(Mirror_cnt48_inc) } },
    { "mirror_ingress_dbuf_drop_pk_cnt", { const_cast<Mirror_cnt48_inc(*)>(&Mirror_slice_regs_base->mirror_ingress_dbuf_drop_pk_cnt), new Mirror_cnt48_inc_map, {}, sizeof(Mirror_cnt48_inc) } },
    { "mirror_egress_dbuf_drop_pk_cnt", { const_cast<Mirror_cnt48_inc(*)>(&Mirror_slice_regs_base->mirror_egress_dbuf_drop_pk_cnt), new Mirror_cnt48_inc_map, {}, sizeof(Mirror_cnt48_inc) } },
    { "m_slice_ecc", { const_cast<uint32_t(*)>(&Mirror_slice_regs_base->m_slice_ecc), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Mirror_slice_regs_base->intr, new Mirror_slice_regs_intr_map, {}, sizeof(Mirror_slice_regs_intr) } },
    { "session_mem_sbe_err_log", { const_cast<uint32_t(*)>(&Mirror_slice_regs_base->session_mem_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "session_mem_mbe_err_log", { const_cast<uint32_t(*)>(&Mirror_slice_regs_base->session_mem_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "data_mem_sbe_err_log", { const_cast<uint32_t(*)>(&Mirror_slice_regs_base->data_mem_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "data_mem_mbe_err_log", { const_cast<uint32_t(*)>(&Mirror_slice_regs_base->data_mem_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "meta_mem_sbe_err_log", { const_cast<uint32_t(*)>(&Mirror_slice_regs_base->meta_mem_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "meta_mem_mbe_err_log", { const_cast<uint32_t(*)>(&Mirror_slice_regs_base->meta_mem_mbe_err_log), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mirror_reg_map: public RegisterMapper {
  static constexpr PTR_Mirror_reg Mirror_reg_base=0;
  Mirror_reg_map() : RegisterMapper( {
    { "s2p_regs", { &Mirror_reg_base->s2p_regs, new Mirror_s2p_regs_map, {}, sizeof(Mirror_s2p_regs) } },
    { "s2p_coal", { &Mirror_reg_base->s2p_coal, new Mirror_s2p_coal_map_map, {}, sizeof(Mirror_s2p_coal_map) } },
    { "s2p_sess", { &Mirror_reg_base->s2p_sess, new Mirror_s2p_session_map_map, {}, sizeof(Mirror_s2p_session_map) } },
    { "slice_mem", { &Mirror_reg_base->slice_mem, new Mirror_slice_mem_map, {0x4}, sizeof(Mirror_slice_mem) } },
    { "slice_regs", { &Mirror_reg_base->slice_regs, new Mirror_slice_regs_map, {0x4}, sizeof(Mirror_slice_regs) } }
    } )
  {}
};

struct Mirr_reg_map: public RegisterMapper {
  static constexpr PTR_Mirr_reg Mirr_reg_base=0;
  Mirr_reg_map() : RegisterMapper( {
    { "mirror", { &Mirr_reg_base->mirror, new Mirror_reg_map, {}, sizeof(Mirror_reg) } }
    } )
  {}
};

struct Dprsr_pv_table_map_map: public RegisterMapper {
  static constexpr PTR_Dprsr_pv_table_map Dprsr_pv_table_map_base=0;
  Dprsr_pv_table_map_map() : RegisterMapper( {
    { "tbl0", { const_cast<uint32_t(*)[0x4000]>(&Dprsr_pv_table_map_base->tbl0), 0, {0x4000}, sizeof(uint32_t) } },
    { "tbl1", { const_cast<uint32_t(*)[0x4000]>(&Dprsr_pv_table_map_base->tbl1), 0, {0x4000}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ii_mem_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ii_mem Dprsr_ii_mem_base=0;
  Dprsr_ii_mem_map() : RegisterMapper( {
    { "pv_table", { &Dprsr_ii_mem_base->pv_table, new Dprsr_pv_table_map_map, {}, sizeof(Dprsr_pv_table_map) } }
    } )
  {}
};

struct Dprsr_pov_position_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_pov_position_r Dprsr_pov_position_r_base=0;
  Dprsr_pov_position_r_map() : RegisterMapper( {
    { "pov_0_4", { const_cast<uint32_t(*)>(&Dprsr_pov_position_r_base->pov_0_4), 0, {}, sizeof(uint32_t) } },
    { "pov_1_4", { const_cast<uint32_t(*)>(&Dprsr_pov_position_r_base->pov_1_4), 0, {}, sizeof(uint32_t) } },
    { "pov_2_4", { const_cast<uint32_t(*)>(&Dprsr_pov_position_r_base->pov_2_4), 0, {}, sizeof(uint32_t) } },
    { "pov_3_4", { const_cast<uint32_t(*)>(&Dprsr_pov_position_r_base->pov_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_cnt_vld_i_phv_map: public RegisterMapper {
  static constexpr PTR_Dprsr_cnt_vld_i_phv Dprsr_cnt_vld_i_phv_base=0;
  Dprsr_cnt_vld_i_phv_map() : RegisterMapper( {
    { "cnt_i_phv_0_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_vld_i_phv_base->cnt_i_phv_0_2), 0, {}, sizeof(uint32_t) } },
    { "cnt_i_phv_1_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_vld_i_phv_base->cnt_i_phv_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pp_ctr_cfg48_r_map: public RegisterMapper {
  static constexpr PTR_Pp_ctr_cfg48_r Pp_ctr_cfg48_r_base=0;
  Pp_ctr_cfg48_r_map() : RegisterMapper( {
    { "pp_ctr_cfg48_0_2", { const_cast<uint32_t(*)>(&Pp_ctr_cfg48_r_base->pp_ctr_cfg48_0_2), 0, {}, sizeof(uint32_t) } },
    { "pp_ctr_cfg48_1_2", { const_cast<uint32_t(*)>(&Pp_ctr_cfg48_r_base->pp_ctr_cfg48_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_input_ing_and_egr_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_input_ing_and_egr_g Dprsr_input_ing_and_egr_g_base=0;
  Dprsr_input_ing_and_egr_g_map() : RegisterMapper( {
    { "pov", { const_cast<Dprsr_pov_position_r(*)>(&Dprsr_input_ing_and_egr_g_base->pov), new Dprsr_pov_position_r_map, {}, sizeof(Dprsr_pov_position_r) } },
    { "cnt_i_phv", { const_cast<Dprsr_cnt_vld_i_phv(*)>(&Dprsr_input_ing_and_egr_g_base->cnt_i_phv), new Dprsr_cnt_vld_i_phv_map, {}, sizeof(Dprsr_cnt_vld_i_phv) } },
    { "scratch", { const_cast<uint32_t(*)>(&Dprsr_input_ing_and_egr_g_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "pp_ctr_cfg48", { const_cast<Pp_ctr_cfg48_r(*)>(&Dprsr_input_ing_and_egr_g_base->pp_ctr_cfg48), new Pp_ctr_cfg48_r_map, {}, sizeof(Pp_ctr_cfg48_r) } },
    { "pp_ctr_cfg_mask", { const_cast<uint32_t(*)>(&Dprsr_input_ing_and_egr_g_base->pp_ctr_cfg_mask), 0, {}, sizeof(uint32_t) } },
    { "pp_ctr_cfg_data", { const_cast<uint32_t(*)>(&Dprsr_input_ing_and_egr_g_base->pp_ctr_cfg_data), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_learn_table_entry_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_learn_table_entry_r Dprsr_learn_table_entry_r_base=0;
  Dprsr_learn_table_entry_r_map() : RegisterMapper( {
    { "learn_tbl_0_13", { const_cast<uint32_t(*)>(&Dprsr_learn_table_entry_r_base->learn_tbl_0_13), 0, {}, sizeof(uint32_t) } },
    { "learn_tbl_1_13", { const_cast<uint32_t(*)>(&Dprsr_learn_table_entry_r_base->learn_tbl_1_13), 0, {}, sizeof(uint32_t) } },
    { "learn_tbl_2_13", { const_cast<uint32_t(*)>(&Dprsr_learn_table_entry_r_base->learn_tbl_2_13), 0, {}, sizeof(uint32_t) } },
    { "learn_tbl_3_13", { const_cast<uint32_t(*)>(&Dprsr_learn_table_entry_r_base->learn_tbl_3_13), 0, {}, sizeof(uint32_t) } },
    { "learn_tbl_4_13", { const_cast<uint32_t(*)>(&Dprsr_learn_table_entry_r_base->learn_tbl_4_13), 0, {}, sizeof(uint32_t) } },
    { "learn_tbl_5_13", { const_cast<uint32_t(*)>(&Dprsr_learn_table_entry_r_base->learn_tbl_5_13), 0, {}, sizeof(uint32_t) } },
    { "learn_tbl_6_13", { const_cast<uint32_t(*)>(&Dprsr_learn_table_entry_r_base->learn_tbl_6_13), 0, {}, sizeof(uint32_t) } },
    { "learn_tbl_7_13", { const_cast<uint32_t(*)>(&Dprsr_learn_table_entry_r_base->learn_tbl_7_13), 0, {}, sizeof(uint32_t) } },
    { "learn_tbl_8_13", { const_cast<uint32_t(*)>(&Dprsr_learn_table_entry_r_base->learn_tbl_8_13), 0, {}, sizeof(uint32_t) } },
    { "learn_tbl_9_13", { const_cast<uint32_t(*)>(&Dprsr_learn_table_entry_r_base->learn_tbl_9_13), 0, {}, sizeof(uint32_t) } },
    { "learn_tbl_10_13", { const_cast<uint32_t(*)>(&Dprsr_learn_table_entry_r_base->learn_tbl_10_13), 0, {}, sizeof(uint32_t) } },
    { "learn_tbl_11_13", { const_cast<uint32_t(*)>(&Dprsr_learn_table_entry_r_base->learn_tbl_11_13), 0, {}, sizeof(uint32_t) } },
    { "learn_tbl_12_13", { const_cast<uint32_t(*)>(&Dprsr_learn_table_entry_r_base->learn_tbl_12_13), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_resubmit_pktgen_table_entry_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_resubmit_pktgen_table_entry_r Dprsr_resubmit_pktgen_table_entry_r_base=0;
  Dprsr_resubmit_pktgen_table_entry_r_map() : RegisterMapper( {
    { "resub_tbl_0_5", { const_cast<uint32_t(*)>(&Dprsr_resubmit_pktgen_table_entry_r_base->resub_tbl_0_5), 0, {}, sizeof(uint32_t) } },
    { "resub_tbl_1_5", { const_cast<uint32_t(*)>(&Dprsr_resubmit_pktgen_table_entry_r_base->resub_tbl_1_5), 0, {}, sizeof(uint32_t) } },
    { "resub_tbl_2_5", { const_cast<uint32_t(*)>(&Dprsr_resubmit_pktgen_table_entry_r_base->resub_tbl_2_5), 0, {}, sizeof(uint32_t) } },
    { "resub_tbl_3_5", { const_cast<uint32_t(*)>(&Dprsr_resubmit_pktgen_table_entry_r_base->resub_tbl_3_5), 0, {}, sizeof(uint32_t) } },
    { "resub_tbl_4_5", { const_cast<uint32_t(*)>(&Dprsr_resubmit_pktgen_table_entry_r_base->resub_tbl_4_5), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_input_ingress_only_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_input_ingress_only_g Dprsr_input_ingress_only_g_base=0;
  Dprsr_input_ingress_only_g_map() : RegisterMapper( {
    { "learn_tbl", { const_cast<Dprsr_learn_table_entry_r(*)[0x8]>(&Dprsr_input_ingress_only_g_base->learn_tbl), new Dprsr_learn_table_entry_r_map, {0x8}, sizeof(Dprsr_learn_table_entry_r) } },
    { "resub_tbl", { const_cast<Dprsr_resubmit_pktgen_table_entry_r(*)[0x8]>(&Dprsr_input_ingress_only_g_base->resub_tbl), new Dprsr_resubmit_pktgen_table_entry_r_map, {0x8}, sizeof(Dprsr_resubmit_pktgen_table_entry_r) } },
    { "pgen_tbl", { const_cast<Dprsr_resubmit_pktgen_table_entry_r(*)>(&Dprsr_input_ingress_only_g_base->pgen_tbl), new Dprsr_resubmit_pktgen_table_entry_r_map, {}, sizeof(Dprsr_resubmit_pktgen_table_entry_r) } },
    { "resubmit_mode", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->resubmit_mode), 0, {}, sizeof(uint32_t) } },
    { "copy_to_cpu_pv", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->copy_to_cpu_pv), 0, {}, sizeof(uint32_t) } },
    { "m_learn_sel", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->m_learn_sel), 0, {}, sizeof(uint32_t) } },
    { "m_resub_sel", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->m_resub_sel), 0, {}, sizeof(uint32_t) } },
    { "m_pgen", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->m_pgen), 0, {}, sizeof(uint32_t) } },
    { "m_pgen_len", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->m_pgen_len), 0, {}, sizeof(uint32_t) } },
    { "m_pgen_addr", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->m_pgen_addr), 0, {}, sizeof(uint32_t) } },
    { "m_egress_unicast_port", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->m_egress_unicast_port), 0, {}, sizeof(uint32_t) } },
    { "m_mgid1", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->m_mgid1), 0, {}, sizeof(uint32_t) } },
    { "m_mgid2", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->m_mgid2), 0, {}, sizeof(uint32_t) } },
    { "m_copy_to_cpu", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->m_copy_to_cpu), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_sel", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->m_mirr_sel), 0, {}, sizeof(uint32_t) } },
    { "m_drop_ctl", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->m_drop_ctl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_input_egress_only_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_input_egress_only_g Dprsr_input_egress_only_g_base=0;
  Dprsr_input_egress_only_g_map() : RegisterMapper( {
    { "m_egress_unicast_port", { const_cast<uint32_t(*)>(&Dprsr_input_egress_only_g_base->m_egress_unicast_port), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_sel", { const_cast<uint32_t(*)>(&Dprsr_input_egress_only_g_base->m_mirr_sel), 0, {}, sizeof(uint32_t) } },
    { "m_drop_ctl", { const_cast<uint32_t(*)>(&Dprsr_input_egress_only_g_base->m_drop_ctl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_csum_pov_cfg_map: public RegisterMapper {
  static constexpr PTR_Dprsr_csum_pov_cfg Dprsr_csum_pov_cfg_base=0;
  Dprsr_csum_pov_cfg_map() : RegisterMapper( {
    { "csum_pov_cfg", { const_cast<uint32_t(*)[0x8]>(&Dprsr_csum_pov_cfg_base->csum_pov_cfg), 0, {0x8}, sizeof(uint32_t) } },
    { "csum_pov_invert", { const_cast<uint32_t(*)[0x8]>(&Dprsr_csum_pov_cfg_base->csum_pov_invert), 0, {0x8}, sizeof(uint32_t) } },
    { "thread", { const_cast<uint32_t(*)>(&Dprsr_csum_pov_cfg_base->thread), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ipp_regs_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ipp_regs Dprsr_ipp_regs_base=0;
  Dprsr_ipp_regs_map() : RegisterMapper( {
    { "main_i", { &Dprsr_ipp_regs_base->main_i, new Dprsr_input_ing_and_egr_g_map, {}, sizeof(Dprsr_input_ing_and_egr_g) } },
    { "main_e", { &Dprsr_ipp_regs_base->main_e, new Dprsr_input_ing_and_egr_g_map, {}, sizeof(Dprsr_input_ing_and_egr_g) } },
    { "ingr", { &Dprsr_ipp_regs_base->ingr, new Dprsr_input_ingress_only_g_map, {}, sizeof(Dprsr_input_ingress_only_g) } },
    { "egr", { &Dprsr_ipp_regs_base->egr, new Dprsr_input_egress_only_g_map, {}, sizeof(Dprsr_input_egress_only_g) } },
    { "phv_csum_pov_cfg", { &Dprsr_ipp_regs_base->phv_csum_pov_cfg, new Dprsr_csum_pov_cfg_map, {}, sizeof(Dprsr_csum_pov_cfg) } },
    { "scratch", { const_cast<uint32_t(*)>(&Dprsr_ipp_regs_base->scratch), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_phv_csum_cfg_entry_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_phv_csum_cfg_entry_g Dprsr_phv_csum_cfg_entry_g_base=0;
  Dprsr_phv_csum_cfg_entry_g_map() : RegisterMapper( {
    { "entry", { const_cast<uint32_t(*)[0x120]>(&Dprsr_phv_csum_cfg_entry_g_base->entry), 0, {0x120}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_i_csum_map_map: public RegisterMapper {
  static constexpr PTR_Dprsr_i_csum_map Dprsr_i_csum_map_base=0;
  Dprsr_i_csum_map_map() : RegisterMapper( {
    { "engine", { &Dprsr_i_csum_map_base->engine, new Dprsr_phv_csum_cfg_entry_g_map, {0x8}, sizeof(Dprsr_phv_csum_cfg_entry_g) } }
    } )
  {}
};

struct Dprsr_ipp_mem_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ipp_mem Dprsr_ipp_mem_base=0;
  Dprsr_ipp_mem_map() : RegisterMapper( {
    { "i_csum", { &Dprsr_ipp_mem_base->i_csum, new Dprsr_i_csum_map_map, {}, sizeof(Dprsr_i_csum_map) } }
    } )
  {}
};

struct Dprsr_input_non_pp_ing_and_egr_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_input_non_pp_ing_and_egr_g Dprsr_input_non_pp_ing_and_egr_g_base=0;
  Dprsr_input_non_pp_ing_and_egr_g_map() : RegisterMapper( {
    { "chunk_info", { const_cast<uint32_t(*)[0x80]>(&Dprsr_input_non_pp_ing_and_egr_g_base->chunk_info), 0, {0x80}, sizeof(uint32_t) } },
    { "fd_tags", { const_cast<uint32_t(*)[0x10]>(&Dprsr_input_non_pp_ing_and_egr_g_base->fd_tags), 0, {0x10}, sizeof(uint32_t) } },
    { "clot_fifo_wmk", { const_cast<uint32_t(*)>(&Dprsr_input_non_pp_ing_and_egr_g_base->clot_fifo_wmk), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_i_fullcsum_engine_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_i_fullcsum_engine_g Dprsr_i_fullcsum_engine_g_base=0;
  Dprsr_i_fullcsum_engine_g_map() : RegisterMapper( {
    { "clot_entry", { const_cast<uint32_t(*)[0x10]>(&Dprsr_i_fullcsum_engine_g_base->clot_entry), 0, {0x10}, sizeof(uint32_t) } },
    { "phv_entry", { const_cast<uint32_t(*)[0x8]>(&Dprsr_i_fullcsum_engine_g_base->phv_entry), 0, {0x8}, sizeof(uint32_t) } },
    { "tags", { const_cast<uint32_t(*)[0x10]>(&Dprsr_i_fullcsum_engine_g_base->tags), 0, {0x10}, sizeof(uint32_t) } },
    { "csum_constant", { const_cast<uint32_t(*)>(&Dprsr_i_fullcsum_engine_g_base->csum_constant), 0, {}, sizeof(uint32_t) } },
    { "zeros_as_ones", { const_cast<uint32_t(*)>(&Dprsr_i_fullcsum_engine_g_base->zeros_as_ones), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ingress_hdr_meta_for_input_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ingress_hdr_meta_for_input_g Dprsr_ingress_hdr_meta_for_input_g_base=0;
  Dprsr_ingress_hdr_meta_for_input_g_map() : RegisterMapper( {
    { "m_hash1", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_hash1), 0, {}, sizeof(uint32_t) } },
    { "m_hash2", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_hash2), 0, {}, sizeof(uint32_t) } },
    { "m_copy_to_cpu_cos", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_copy_to_cpu_cos), 0, {}, sizeof(uint32_t) } },
    { "m_deflect_on_drop", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_deflect_on_drop), 0, {}, sizeof(uint32_t) } },
    { "m_icos", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_icos), 0, {}, sizeof(uint32_t) } },
    { "m_pkt_color", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_pkt_color), 0, {}, sizeof(uint32_t) } },
    { "m_qid", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_qid), 0, {}, sizeof(uint32_t) } },
    { "m_xid_l1", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_xid_l1), 0, {}, sizeof(uint32_t) } },
    { "m_xid_l2", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_xid_l2), 0, {}, sizeof(uint32_t) } },
    { "m_rid", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_rid), 0, {}, sizeof(uint32_t) } },
    { "m_bypss_egr", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_bypss_egr), 0, {}, sizeof(uint32_t) } },
    { "m_ct_disable", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_ct_disable), 0, {}, sizeof(uint32_t) } },
    { "m_ct_mcast", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_ct_mcast), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_io_sel", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_mirr_io_sel), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_hash", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_mirr_hash), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_epipe_port", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_mirr_epipe_port), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_qid", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_mirr_qid), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_dond_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_mirr_dond_ctrl), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_icos", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_mirr_icos), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_mc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_mirr_mc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_c2c_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_mirr_c2c_ctrl), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_coal_smpl_len", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_mirr_coal_smpl_len), 0, {}, sizeof(uint32_t) } },
    { "m_afc", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_afc), 0, {}, sizeof(uint32_t) } },
    { "m_mtu_trunc_len", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_mtu_trunc_len), 0, {}, sizeof(uint32_t) } },
    { "m_mtu_trunc_err_f", { const_cast<uint32_t(*)>(&Dprsr_ingress_hdr_meta_for_input_g_base->m_mtu_trunc_err_f), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_egress_hdr_meta_for_input_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_egress_hdr_meta_for_input_g Dprsr_egress_hdr_meta_for_input_g_base=0;
  Dprsr_egress_hdr_meta_for_input_g_map() : RegisterMapper( {
    { "m_force_tx_err", { const_cast<uint32_t(*)>(&Dprsr_egress_hdr_meta_for_input_g_base->m_force_tx_err), 0, {}, sizeof(uint32_t) } },
    { "m_capture_tx_ts", { const_cast<uint32_t(*)>(&Dprsr_egress_hdr_meta_for_input_g_base->m_capture_tx_ts), 0, {}, sizeof(uint32_t) } },
    { "m_tx_pkt_has_offsets", { const_cast<uint32_t(*)>(&Dprsr_egress_hdr_meta_for_input_g_base->m_tx_pkt_has_offsets), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_io_sel", { const_cast<uint32_t(*)>(&Dprsr_egress_hdr_meta_for_input_g_base->m_mirr_io_sel), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_hash", { const_cast<uint32_t(*)>(&Dprsr_egress_hdr_meta_for_input_g_base->m_mirr_hash), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_epipe_port", { const_cast<uint32_t(*)>(&Dprsr_egress_hdr_meta_for_input_g_base->m_mirr_epipe_port), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_qid", { const_cast<uint32_t(*)>(&Dprsr_egress_hdr_meta_for_input_g_base->m_mirr_qid), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_dond_ctrl", { const_cast<uint32_t(*)>(&Dprsr_egress_hdr_meta_for_input_g_base->m_mirr_dond_ctrl), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_icos", { const_cast<uint32_t(*)>(&Dprsr_egress_hdr_meta_for_input_g_base->m_mirr_icos), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_mc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_egress_hdr_meta_for_input_g_base->m_mirr_mc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_c2c_ctrl", { const_cast<uint32_t(*)>(&Dprsr_egress_hdr_meta_for_input_g_base->m_mirr_c2c_ctrl), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_coal_smpl_len", { const_cast<uint32_t(*)>(&Dprsr_egress_hdr_meta_for_input_g_base->m_mirr_coal_smpl_len), 0, {}, sizeof(uint32_t) } },
    { "m_afc", { const_cast<uint32_t(*)>(&Dprsr_egress_hdr_meta_for_input_g_base->m_afc), 0, {}, sizeof(uint32_t) } },
    { "m_mtu_trunc_len", { const_cast<uint32_t(*)>(&Dprsr_egress_hdr_meta_for_input_g_base->m_mtu_trunc_len), 0, {}, sizeof(uint32_t) } },
    { "m_mtu_trunc_err_f", { const_cast<uint32_t(*)>(&Dprsr_egress_hdr_meta_for_input_g_base->m_mtu_trunc_err_f), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_learn_mask_entry_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_learn_mask_entry_r Dprsr_learn_mask_entry_r_base=0;
  Dprsr_learn_mask_entry_r_map() : RegisterMapper( {
    { "lrnmask_0_12", { const_cast<uint32_t(*)>(&Dprsr_learn_mask_entry_r_base->lrnmask_0_12), 0, {}, sizeof(uint32_t) } },
    { "lrnmask_1_12", { const_cast<uint32_t(*)>(&Dprsr_learn_mask_entry_r_base->lrnmask_1_12), 0, {}, sizeof(uint32_t) } },
    { "lrnmask_2_12", { const_cast<uint32_t(*)>(&Dprsr_learn_mask_entry_r_base->lrnmask_2_12), 0, {}, sizeof(uint32_t) } },
    { "lrnmask_3_12", { const_cast<uint32_t(*)>(&Dprsr_learn_mask_entry_r_base->lrnmask_3_12), 0, {}, sizeof(uint32_t) } },
    { "lrnmask_4_12", { const_cast<uint32_t(*)>(&Dprsr_learn_mask_entry_r_base->lrnmask_4_12), 0, {}, sizeof(uint32_t) } },
    { "lrnmask_5_12", { const_cast<uint32_t(*)>(&Dprsr_learn_mask_entry_r_base->lrnmask_5_12), 0, {}, sizeof(uint32_t) } },
    { "lrnmask_6_12", { const_cast<uint32_t(*)>(&Dprsr_learn_mask_entry_r_base->lrnmask_6_12), 0, {}, sizeof(uint32_t) } },
    { "lrnmask_7_12", { const_cast<uint32_t(*)>(&Dprsr_learn_mask_entry_r_base->lrnmask_7_12), 0, {}, sizeof(uint32_t) } },
    { "lrnmask_8_12", { const_cast<uint32_t(*)>(&Dprsr_learn_mask_entry_r_base->lrnmask_8_12), 0, {}, sizeof(uint32_t) } },
    { "lrnmask_9_12", { const_cast<uint32_t(*)>(&Dprsr_learn_mask_entry_r_base->lrnmask_9_12), 0, {}, sizeof(uint32_t) } },
    { "lrnmask_10_12", { const_cast<uint32_t(*)>(&Dprsr_learn_mask_entry_r_base->lrnmask_10_12), 0, {}, sizeof(uint32_t) } },
    { "lrnmask_11_12", { const_cast<uint32_t(*)>(&Dprsr_learn_mask_entry_r_base->lrnmask_11_12), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ic_regs_intr_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ic_regs_intr Dprsr_ic_regs_intr_base=0;
  Dprsr_ic_regs_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ic_regs_intr_b_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ic_regs_intr_b Dprsr_ic_regs_intr_b_base=0;
  Dprsr_ic_regs_intr_b_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_intr_b_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_intr_b_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_intr_b_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_intr_b_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_intr_b_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ic_regs_phv32_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ic_regs_phv32_ecc_ctrl Dprsr_ic_regs_phv32_ecc_ctrl_base=0;
  Dprsr_ic_regs_phv32_ecc_ctrl_map() : RegisterMapper( {
    { "phv32_ecc_ctrl_0_3", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_phv32_ecc_ctrl_base->phv32_ecc_ctrl_0_3), 0, {}, sizeof(uint32_t) } },
    { "phv32_ecc_ctrl_1_3", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_phv32_ecc_ctrl_base->phv32_ecc_ctrl_1_3), 0, {}, sizeof(uint32_t) } },
    { "phv32_ecc_ctrl_2_3", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_phv32_ecc_ctrl_base->phv32_ecc_ctrl_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ic_regs_phv16_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ic_regs_phv16_ecc_ctrl Dprsr_ic_regs_phv16_ecc_ctrl_base=0;
  Dprsr_ic_regs_phv16_ecc_ctrl_map() : RegisterMapper( {
    { "phv16_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_phv16_ecc_ctrl_base->phv16_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "phv16_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_phv16_ecc_ctrl_base->phv16_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ic_regs_phv8_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ic_regs_phv8_ecc_ctrl Dprsr_ic_regs_phv8_ecc_ctrl_base=0;
  Dprsr_ic_regs_phv8_ecc_ctrl_map() : RegisterMapper( {
    { "phv8_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_phv8_ecc_ctrl_base->phv8_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "phv8_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_phv8_ecc_ctrl_base->phv8_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ic_regs_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ic_regs Dprsr_ic_regs_base=0;
  Dprsr_ic_regs_map() : RegisterMapper( {
    { "ingr", { &Dprsr_ic_regs_base->ingr, new Dprsr_input_non_pp_ing_and_egr_g_map, {}, sizeof(Dprsr_input_non_pp_ing_and_egr_g) } },
    { "egr", { &Dprsr_ic_regs_base->egr, new Dprsr_input_non_pp_ing_and_egr_g_map, {}, sizeof(Dprsr_input_non_pp_ing_and_egr_g) } },
    { "csum_engine", { &Dprsr_ic_regs_base->csum_engine, new Dprsr_i_fullcsum_engine_g_map, {0x8}, sizeof(Dprsr_i_fullcsum_engine_g) } },
    { "scratch", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "scratch2", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->scratch2), 0, {}, sizeof(uint32_t) } },
    { "i_phv8_grp", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_phv8_grp), 0, {}, sizeof(uint32_t) } },
    { "i_phv16_grp", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_phv16_grp), 0, {}, sizeof(uint32_t) } },
    { "i_phv32_grp", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_phv32_grp), 0, {}, sizeof(uint32_t) } },
    { "e_phv8_grp", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_phv8_grp), 0, {}, sizeof(uint32_t) } },
    { "e_phv16_grp", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_phv16_grp), 0, {}, sizeof(uint32_t) } },
    { "e_phv32_grp", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_phv32_grp), 0, {}, sizeof(uint32_t) } },
    { "mac0_rates", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac0_rates), 0, {}, sizeof(uint32_t) } },
    { "mac0_en", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac0_en), 0, {}, sizeof(uint32_t) } },
    { "mac1_rates", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac1_rates), 0, {}, sizeof(uint32_t) } },
    { "mac1_en", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac1_en), 0, {}, sizeof(uint32_t) } },
    { "mac2_rates", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac2_rates), 0, {}, sizeof(uint32_t) } },
    { "mac2_en", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac2_en), 0, {}, sizeof(uint32_t) } },
    { "mac3_rates", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac3_rates), 0, {}, sizeof(uint32_t) } },
    { "mac3_en", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac3_en), 0, {}, sizeof(uint32_t) } },
    { "mac4_rates", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac4_rates), 0, {}, sizeof(uint32_t) } },
    { "mac4_en", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac4_en), 0, {}, sizeof(uint32_t) } },
    { "mac5_rates", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac5_rates), 0, {}, sizeof(uint32_t) } },
    { "mac5_en", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac5_en), 0, {}, sizeof(uint32_t) } },
    { "mac6_rates", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac6_rates), 0, {}, sizeof(uint32_t) } },
    { "mac6_en", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac6_en), 0, {}, sizeof(uint32_t) } },
    { "mac7_rates", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac7_rates), 0, {}, sizeof(uint32_t) } },
    { "mac7_en", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac7_en), 0, {}, sizeof(uint32_t) } },
    { "mac8_rates", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac8_rates), 0, {}, sizeof(uint32_t) } },
    { "mac8_en", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mac8_en), 0, {}, sizeof(uint32_t) } },
    { "ingr_meta_pov", { &Dprsr_ic_regs_base->ingr_meta_pov, new Dprsr_ingress_hdr_meta_for_input_g_map, {}, sizeof(Dprsr_ingress_hdr_meta_for_input_g) } },
    { "egr_meta_pov", { &Dprsr_ic_regs_base->egr_meta_pov, new Dprsr_egress_hdr_meta_for_input_g_map, {}, sizeof(Dprsr_egress_hdr_meta_for_input_g) } },
    { "lrnmask", { const_cast<Dprsr_learn_mask_entry_r(*)[0x8]>(&Dprsr_ic_regs_base->lrnmask), new Dprsr_learn_mask_entry_r_map, {0x8}, sizeof(Dprsr_learn_mask_entry_r) } },
    { "intr", { &Dprsr_ic_regs_base->intr, new Dprsr_ic_regs_intr_map, {}, sizeof(Dprsr_ic_regs_intr) } },
    { "pv_tbl_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->pv_tbl_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "pv_tbl_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->pv_tbl_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_clot_fifo_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_clot_fifo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_clot_fifo_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_clot_fifo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_clot_fifo_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_clot_fifo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_clot_fifo_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_clot_fifo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "mau_acctg_fifo_fifo_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mau_acctg_fifo_fifo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "mau_acctg_fifo_fifo_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mau_acctg_fifo_fifo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "pvt_cfg", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->pvt_cfg), 0, {}, sizeof(uint32_t) } },
    { "pvt_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->pvt_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "i_clot_fifo_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_clot_fifo_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "e_clot_fifo_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_clot_fifo_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "mau_acctg_fifo_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mau_acctg_fifo_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "mau_acctg_fifo_wmk", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->mau_acctg_fifo_wmk), 0, {}, sizeof(uint32_t) } },
    { "max_inp_buff_entries", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->max_inp_buff_entries), 0, {}, sizeof(uint32_t) } },
    { "slice_max_credits", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->slice_max_credits), 0, {}, sizeof(uint32_t) } },
    { "intr_b", { &Dprsr_ic_regs_base->intr_b, new Dprsr_ic_regs_intr_b_map, {}, sizeof(Dprsr_ic_regs_intr_b) } },
    { "i_meta_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_meta_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_meta_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_meta_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_meta_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_meta_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_meta_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_meta_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "phv32_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv32_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "phv32_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv32_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "phv16_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv16_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "phv16_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv16_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "phv8_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv8_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "phv8_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv8_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_volts_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_volts_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_volts_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_volts_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_volts_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_volts_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_volts_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_volts_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_fdinfo_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_fdinfo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_fdinfo_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_fdinfo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_fdinfo_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_fdinfo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_fdinfo_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_fdinfo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_cmd_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_cmd_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_cmd_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_cmd_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_cmd_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_cmd_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_cmd_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_cmd_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_buff_overflow_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_buff_overflow_log), 0, {}, sizeof(uint32_t) } },
    { "e_buff_overflow_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_buff_overflow_log), 0, {}, sizeof(uint32_t) } },
    { "i_meta_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_meta_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "e_meta_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_meta_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "phv32_ecc_ctrl", { const_cast<Dprsr_ic_regs_phv32_ecc_ctrl(*)>(&Dprsr_ic_regs_base->phv32_ecc_ctrl), new Dprsr_ic_regs_phv32_ecc_ctrl_map, {}, sizeof(Dprsr_ic_regs_phv32_ecc_ctrl) } },
    { "phv16_ecc_ctrl", { const_cast<Dprsr_ic_regs_phv16_ecc_ctrl(*)>(&Dprsr_ic_regs_base->phv16_ecc_ctrl), new Dprsr_ic_regs_phv16_ecc_ctrl_map, {}, sizeof(Dprsr_ic_regs_phv16_ecc_ctrl) } },
    { "phv8_ecc_ctrl", { const_cast<Dprsr_ic_regs_phv8_ecc_ctrl(*)>(&Dprsr_ic_regs_base->phv8_ecc_ctrl), new Dprsr_ic_regs_phv8_ecc_ctrl_map, {}, sizeof(Dprsr_ic_regs_phv8_ecc_ctrl) } },
    { "i_volts_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_volts_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "e_volts_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_volts_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "i_fdinfo_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_fdinfo_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "e_fdinfo_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_fdinfo_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "i_cmd_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_cmd_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "e_cmd_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_cmd_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "i_freelist_empty_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_freelist_empty_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_freelist_empty_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_freelist_empty_err_log), 0, {}, sizeof(uint32_t) } },
    { "phv_count_sel", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv_count_sel), 0, {}, sizeof(uint32_t) } },
    { "phv_count", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv_count), 0, {}, sizeof(uint32_t) } },
    { "input_status", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->input_status), 0, {}, sizeof(uint32_t) } },
    { "fcu_latency_ctrl", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->fcu_latency_ctrl), 0, {}, sizeof(uint32_t) } },
    { "egr_unicast_check", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->egr_unicast_check), 0, {}, sizeof(uint32_t) } },
    { "lfltr_eop_delay", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->lfltr_eop_delay), 0, {}, sizeof(uint32_t) } },
    { "i_chan_mismatch_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_chan_mismatch_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_chan_mismatch_err_log", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_chan_mismatch_err_log), 0, {}, sizeof(uint32_t) } },
    { "cfg48", { const_cast<Pp_ctr_cfg48_r(*)[0x4]>(&Dprsr_ic_regs_base->cfg48), new Pp_ctr_cfg48_r_map, {0x4}, sizeof(Pp_ctr_cfg48_r) } },
    { "cfg48_mask", { const_cast<uint32_t(*)[0x4]>(&Dprsr_ic_regs_base->cfg48_mask), 0, {0x4}, sizeof(uint32_t) } },
    { "cfg48_data", { const_cast<uint32_t(*)[0x4]>(&Dprsr_ic_regs_base->cfg48_data), 0, {0x4}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_inp_map: public RegisterMapper {
  static constexpr PTR_Dprsr_inp Dprsr_inp_base=0;
  Dprsr_inp_map() : RegisterMapper( {
    { "iim", { &Dprsr_inp_base->iim, new Dprsr_ii_mem_map, {}, sizeof(Dprsr_ii_mem) } },
    { "ipp", { &Dprsr_inp_base->ipp, new Dprsr_ipp_regs_map, {}, sizeof(Dprsr_ipp_regs) } },
    { "ipp_m", { &Dprsr_inp_base->ipp_m, new Dprsr_ipp_mem_map, {}, sizeof(Dprsr_ipp_mem) } },
    { "icr", { &Dprsr_inp_base->icr, new Dprsr_ic_regs_map, {}, sizeof(Dprsr_ic_regs) } }
    } )
  {}
};

struct Dprsr_inp_slice_intr_map: public RegisterMapper {
  static constexpr PTR_Dprsr_inp_slice_intr Dprsr_inp_slice_intr_base=0;
  Dprsr_inp_slice_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_inp_slice_phv32_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_inp_slice_phv32_ecc_ctrl Dprsr_inp_slice_phv32_ecc_ctrl_base=0;
  Dprsr_inp_slice_phv32_ecc_ctrl_map() : RegisterMapper( {
    { "phv32_ecc_ctrl_0_3", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_phv32_ecc_ctrl_base->phv32_ecc_ctrl_0_3), 0, {}, sizeof(uint32_t) } },
    { "phv32_ecc_ctrl_1_3", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_phv32_ecc_ctrl_base->phv32_ecc_ctrl_1_3), 0, {}, sizeof(uint32_t) } },
    { "phv32_ecc_ctrl_2_3", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_phv32_ecc_ctrl_base->phv32_ecc_ctrl_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_inp_slice_phv16_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_inp_slice_phv16_ecc_ctrl Dprsr_inp_slice_phv16_ecc_ctrl_base=0;
  Dprsr_inp_slice_phv16_ecc_ctrl_map() : RegisterMapper( {
    { "phv16_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_phv16_ecc_ctrl_base->phv16_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "phv16_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_phv16_ecc_ctrl_base->phv16_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_inp_slice_phv8_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_inp_slice_phv8_ecc_ctrl Dprsr_inp_slice_phv8_ecc_ctrl_base=0;
  Dprsr_inp_slice_phv8_ecc_ctrl_map() : RegisterMapper( {
    { "phv8_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_phv8_ecc_ctrl_base->phv8_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "phv8_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_phv8_ecc_ctrl_base->phv8_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_inp_slice_map: public RegisterMapper {
  static constexpr PTR_Dprsr_inp_slice Dprsr_inp_slice_base=0;
  Dprsr_inp_slice_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Dprsr_inp_slice_base->intr, new Dprsr_inp_slice_intr_map, {}, sizeof(Dprsr_inp_slice_intr) } },
    { "i_meta_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->i_meta_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_meta_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->i_meta_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_meta_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->e_meta_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_meta_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->e_meta_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "phv32_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->phv32_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "phv32_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->phv32_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "phv16_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->phv16_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "phv16_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->phv16_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "phv8_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->phv8_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "phv8_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->phv8_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_volts_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->i_volts_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_volts_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->i_volts_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_volts_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->e_volts_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_volts_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->e_volts_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_fdinfo_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->i_fdinfo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_fdinfo_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->i_fdinfo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_fdinfo_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->e_fdinfo_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "e_fdinfo_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->e_fdinfo_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "i_meta_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->i_meta_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "e_meta_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->e_meta_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "phv32_ecc_ctrl", { const_cast<Dprsr_inp_slice_phv32_ecc_ctrl(*)>(&Dprsr_inp_slice_base->phv32_ecc_ctrl), new Dprsr_inp_slice_phv32_ecc_ctrl_map, {}, sizeof(Dprsr_inp_slice_phv32_ecc_ctrl) } },
    { "phv16_ecc_ctrl", { const_cast<Dprsr_inp_slice_phv16_ecc_ctrl(*)>(&Dprsr_inp_slice_base->phv16_ecc_ctrl), new Dprsr_inp_slice_phv16_ecc_ctrl_map, {}, sizeof(Dprsr_inp_slice_phv16_ecc_ctrl) } },
    { "phv8_ecc_ctrl", { const_cast<Dprsr_inp_slice_phv8_ecc_ctrl(*)>(&Dprsr_inp_slice_base->phv8_ecc_ctrl), new Dprsr_inp_slice_phv8_ecc_ctrl_map, {}, sizeof(Dprsr_inp_slice_phv8_ecc_ctrl) } },
    { "i_volts_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->i_volts_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "e_volts_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->e_volts_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "i_fdinfo_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->i_fdinfo_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "e_fdinfo_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->e_fdinfo_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "cred_fifo", { const_cast<uint32_t(*)>(&Dprsr_inp_slice_base->cred_fifo), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parde_dprsr_reg_rspec_csr_ring0intr_map: public RegisterMapper {
  static constexpr PTR_Parde_dprsr_reg_rspec_csr_ring0intr Parde_dprsr_reg_rspec_csr_ring0intr_base=0;
  Parde_dprsr_reg_rspec_csr_ring0intr_map() : RegisterMapper( {
    { "status0", { const_cast<uint32_t(*)>(&Parde_dprsr_reg_rspec_csr_ring0intr_base->status0), 0, {}, sizeof(uint32_t) } },
    { "status1", { const_cast<uint32_t(*)>(&Parde_dprsr_reg_rspec_csr_ring0intr_base->status1), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parde_dprsr_reg_rspec_csr_ring1intr_map: public RegisterMapper {
  static constexpr PTR_Parde_dprsr_reg_rspec_csr_ring1intr Parde_dprsr_reg_rspec_csr_ring1intr_base=0;
  Parde_dprsr_reg_rspec_csr_ring1intr_map() : RegisterMapper( {
    { "status0", { const_cast<uint32_t(*)>(&Parde_dprsr_reg_rspec_csr_ring1intr_base->status0), 0, {}, sizeof(uint32_t) } },
    { "status1", { const_cast<uint32_t(*)>(&Parde_dprsr_reg_rspec_csr_ring1intr_base->status1), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parde_dprsr_reg_rspec_csr_ring2intr_map: public RegisterMapper {
  static constexpr PTR_Parde_dprsr_reg_rspec_csr_ring2intr Parde_dprsr_reg_rspec_csr_ring2intr_base=0;
  Parde_dprsr_reg_rspec_csr_ring2intr_map() : RegisterMapper( {
    { "status0", { const_cast<uint32_t(*)>(&Parde_dprsr_reg_rspec_csr_ring2intr_base->status0), 0, {}, sizeof(uint32_t) } },
    { "status1", { const_cast<uint32_t(*)>(&Parde_dprsr_reg_rspec_csr_ring2intr_base->status1), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parde_dprsr_reg_rspec_csr_ring3intr_map: public RegisterMapper {
  static constexpr PTR_Parde_dprsr_reg_rspec_csr_ring3intr Parde_dprsr_reg_rspec_csr_ring3intr_base=0;
  Parde_dprsr_reg_rspec_csr_ring3intr_map() : RegisterMapper( {
    { "status0", { const_cast<uint32_t(*)>(&Parde_dprsr_reg_rspec_csr_ring3intr_base->status0), 0, {}, sizeof(uint32_t) } },
    { "status1", { const_cast<uint32_t(*)>(&Parde_dprsr_reg_rspec_csr_ring3intr_base->status1), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parde_dprsr_reg_rspec_parde_intr_map: public RegisterMapper {
  static constexpr PTR_Parde_dprsr_reg_rspec_parde_intr Parde_dprsr_reg_rspec_parde_intr_base=0;
  Parde_dprsr_reg_rspec_parde_intr_map() : RegisterMapper( {
    { "status0", { const_cast<uint32_t(*)>(&Parde_dprsr_reg_rspec_parde_intr_base->status0), 0, {}, sizeof(uint32_t) } },
    { "status1", { const_cast<uint32_t(*)>(&Parde_dprsr_reg_rspec_parde_intr_base->status1), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parde_dprsr_reg_rspec_map: public RegisterMapper {
  static constexpr PTR_Parde_dprsr_reg_rspec Parde_dprsr_reg_rspec_base=0;
  Parde_dprsr_reg_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Parde_dprsr_reg_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "debug_ctrl", { const_cast<uint32_t(*)>(&Parde_dprsr_reg_rspec_base->debug_ctrl), 0, {}, sizeof(uint32_t) } },
    { "csr_ring_full_thresh", { const_cast<uint32_t(*)>(&Parde_dprsr_reg_rspec_base->csr_ring_full_thresh), 0, {}, sizeof(uint32_t) } },
    { "csr_ring_fifo_err", { const_cast<uint32_t(*)>(&Parde_dprsr_reg_rspec_base->csr_ring_fifo_err), 0, {}, sizeof(uint32_t) } },
    { "csr_ring0intr", { &Parde_dprsr_reg_rspec_base->csr_ring0intr, new Parde_dprsr_reg_rspec_csr_ring0intr_map, {}, sizeof(Parde_dprsr_reg_rspec_csr_ring0intr) } },
    { "csr_ring1intr", { &Parde_dprsr_reg_rspec_base->csr_ring1intr, new Parde_dprsr_reg_rspec_csr_ring1intr_map, {}, sizeof(Parde_dprsr_reg_rspec_csr_ring1intr) } },
    { "csr_ring2intr", { &Parde_dprsr_reg_rspec_base->csr_ring2intr, new Parde_dprsr_reg_rspec_csr_ring2intr_map, {}, sizeof(Parde_dprsr_reg_rspec_csr_ring2intr) } },
    { "csr_ring3intr", { &Parde_dprsr_reg_rspec_base->csr_ring3intr, new Parde_dprsr_reg_rspec_csr_ring3intr_map, {}, sizeof(Parde_dprsr_reg_rspec_csr_ring3intr) } },
    { "parde_intr", { &Parde_dprsr_reg_rspec_base->parde_intr, new Parde_dprsr_reg_rspec_parde_intr_map, {}, sizeof(Parde_dprsr_reg_rspec_parde_intr) } },
    { "intr_period", { const_cast<uint32_t(*)>(&Parde_dprsr_reg_rspec_base->intr_period), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Fd_byte_off_info_r_map: public RegisterMapper {
  static constexpr PTR_Fd_byte_off_info_r Fd_byte_off_info_r_base=0;
  Fd_byte_off_info_r_map() : RegisterMapper( {
    { "byte_off_0_2", { const_cast<uint32_t(*)>(&Fd_byte_off_info_r_base->byte_off_0_2), 0, {}, sizeof(uint32_t) } },
    { "byte_off_1_2", { const_cast<uint32_t(*)>(&Fd_byte_off_info_r_base->byte_off_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_h_fd_full_chunk_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_h_fd_full_chunk_g Dprsr_h_fd_full_chunk_g_base=0;
  Dprsr_h_fd_full_chunk_g_map() : RegisterMapper( {
    { "cfg", { const_cast<uint32_t(*)>(&Dprsr_h_fd_full_chunk_g_base->cfg), 0, {}, sizeof(uint32_t) } },
    { "is_phv", { const_cast<uint32_t(*)>(&Dprsr_h_fd_full_chunk_g_base->is_phv), 0, {}, sizeof(uint32_t) } },
    { "byte_off", { const_cast<Fd_byte_off_info_r(*)>(&Dprsr_h_fd_full_chunk_g_base->byte_off), new Fd_byte_off_info_r_map, {}, sizeof(Fd_byte_off_info_r) } }
    } )
  {}
};

struct Dprsr_h_fd_compress_map_map: public RegisterMapper {
  static constexpr PTR_Dprsr_h_fd_compress_map Dprsr_h_fd_compress_map_base=0;
  Dprsr_h_fd_compress_map_map() : RegisterMapper( {
    { "chunk", { &Dprsr_h_fd_compress_map_base->chunk, new Dprsr_h_fd_full_chunk_g_map, {0x80}, sizeof(Dprsr_h_fd_full_chunk_g) } }
    } )
  {}
};

struct Dprsr_mirror_table_entry_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_mirror_table_entry_r Dprsr_mirror_table_entry_r_base=0;
  Dprsr_mirror_table_entry_r_map() : RegisterMapper( {
    { "entry_0_16", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->entry_0_16), 0, {}, sizeof(uint32_t) } },
    { "entry_1_16", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->entry_1_16), 0, {}, sizeof(uint32_t) } },
    { "entry_2_16", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->entry_2_16), 0, {}, sizeof(uint32_t) } },
    { "entry_3_16", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->entry_3_16), 0, {}, sizeof(uint32_t) } },
    { "entry_4_16", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->entry_4_16), 0, {}, sizeof(uint32_t) } },
    { "entry_5_16", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->entry_5_16), 0, {}, sizeof(uint32_t) } },
    { "entry_6_16", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->entry_6_16), 0, {}, sizeof(uint32_t) } },
    { "entry_7_16", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->entry_7_16), 0, {}, sizeof(uint32_t) } },
    { "entry_8_16", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->entry_8_16), 0, {}, sizeof(uint32_t) } },
    { "entry_9_16", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->entry_9_16), 0, {}, sizeof(uint32_t) } },
    { "entry_10_16", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->entry_10_16), 0, {}, sizeof(uint32_t) } },
    { "entry_11_16", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->entry_11_16), 0, {}, sizeof(uint32_t) } },
    { "entry_12_16", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->entry_12_16), 0, {}, sizeof(uint32_t) } },
    { "entry_13_16", { &Dprsr_mirror_table_entry_r_base->entry_13_16, 0, {}, sizeof(uint32_t) } },
    { "entry_14_16", { &Dprsr_mirror_table_entry_r_base->entry_14_16, 0, {}, sizeof(uint32_t) } },
    { "entry_15_16", { &Dprsr_mirror_table_entry_r_base->entry_15_16, 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_h_mirror_tbl_map_map: public RegisterMapper {
  static constexpr PTR_Dprsr_h_mirror_tbl_map Dprsr_h_mirror_tbl_map_base=0;
  Dprsr_h_mirror_tbl_map_map() : RegisterMapper( {
    { "entry", { const_cast<Dprsr_mirror_table_entry_r(*)[0x10]>(&Dprsr_h_mirror_tbl_map_base->entry), new Dprsr_mirror_table_entry_r_map, {0x10}, sizeof(Dprsr_mirror_table_entry_r) } }
    } )
  {}
};

struct Dprsr_hi_mem_map: public RegisterMapper {
  static constexpr PTR_Dprsr_hi_mem Dprsr_hi_mem_base=0;
  Dprsr_hi_mem_map() : RegisterMapper( {
    { "fd_compress", { &Dprsr_hi_mem_base->fd_compress, new Dprsr_h_fd_compress_map_map, {}, sizeof(Dprsr_h_fd_compress_map) } },
    { "mirr_hdr_tbl", { &Dprsr_hi_mem_base->mirr_hdr_tbl, new Dprsr_h_mirror_tbl_map_map, {}, sizeof(Dprsr_h_mirror_tbl_map) } }
    } )
  {}
};

struct Dprsr_hdr_xbar_const_defs_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_hdr_xbar_const_defs_r Dprsr_hdr_xbar_const_defs_r_base=0;
  Dprsr_hdr_xbar_const_defs_r_map() : RegisterMapper( {
    { "hdr_xbar_const_0_2", { const_cast<uint32_t(*)>(&Dprsr_hdr_xbar_const_defs_r_base->hdr_xbar_const_0_2), 0, {}, sizeof(uint32_t) } },
    { "hdr_xbar_const_1_2", { const_cast<uint32_t(*)>(&Dprsr_hdr_xbar_const_defs_r_base->hdr_xbar_const_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_hi_regs_h_intr_map: public RegisterMapper {
  static constexpr PTR_Dprsr_hi_regs_h_intr Dprsr_hi_regs_h_intr_base=0;
  Dprsr_hi_regs_h_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_hi_regs_h_ipkt_mac0_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_hi_regs_h_ipkt_mac0_ecc_ctrl Dprsr_hi_regs_h_ipkt_mac0_ecc_ctrl_base=0;
  Dprsr_hi_regs_h_ipkt_mac0_ecc_ctrl_map() : RegisterMapper( {
    { "ipkt_mac0_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_ipkt_mac0_ecc_ctrl_base->ipkt_mac0_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac0_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_ipkt_mac0_ecc_ctrl_base->ipkt_mac0_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_hi_regs_h_ipkt_mac1_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_hi_regs_h_ipkt_mac1_ecc_ctrl Dprsr_hi_regs_h_ipkt_mac1_ecc_ctrl_base=0;
  Dprsr_hi_regs_h_ipkt_mac1_ecc_ctrl_map() : RegisterMapper( {
    { "ipkt_mac1_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_ipkt_mac1_ecc_ctrl_base->ipkt_mac1_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac1_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_ipkt_mac1_ecc_ctrl_base->ipkt_mac1_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_hi_regs_h_ipkt_mac2_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_hi_regs_h_ipkt_mac2_ecc_ctrl Dprsr_hi_regs_h_ipkt_mac2_ecc_ctrl_base=0;
  Dprsr_hi_regs_h_ipkt_mac2_ecc_ctrl_map() : RegisterMapper( {
    { "ipkt_mac2_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_ipkt_mac2_ecc_ctrl_base->ipkt_mac2_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac2_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_ipkt_mac2_ecc_ctrl_base->ipkt_mac2_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_hi_regs_h_map: public RegisterMapper {
  static constexpr PTR_Dprsr_hi_regs_h Dprsr_hi_regs_h_base=0;
  Dprsr_hi_regs_h_map() : RegisterMapper( {
    { "compress_clot_sel", { const_cast<uint32_t(*)[0x10]>(&Dprsr_hi_regs_h_base->compress_clot_sel), 0, {0x10}, sizeof(uint32_t) } },
    { "hdr_xbar_const", { const_cast<Dprsr_hdr_xbar_const_defs_r(*)>(&Dprsr_hi_regs_h_base->hdr_xbar_const), new Dprsr_hdr_xbar_const_defs_r_map, {}, sizeof(Dprsr_hdr_xbar_const_defs_r) } },
    { "mirr_hdr_tbl", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->mirr_hdr_tbl), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "cred_thresh", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->cred_thresh), 0, {}, sizeof(uint32_t) } },
    { "cred_max", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->cred_max), 0, {}, sizeof(uint32_t) } },
    { "cred_dbg_chan_sel", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->cred_dbg_chan_sel), 0, {}, sizeof(uint32_t) } },
    { "cred_dbg_credits", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->cred_dbg_credits), 0, {}, sizeof(uint32_t) } },
    { "cred_dbg_reload", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->cred_dbg_reload), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Dprsr_hi_regs_h_base->intr, new Dprsr_hi_regs_h_intr_map, {}, sizeof(Dprsr_hi_regs_h_intr) } },
    { "mirrtbl_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->mirrtbl_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "mirrtbl_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->mirrtbl_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac0_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->ipkt_mac0_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac0_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->ipkt_mac0_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac1_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->ipkt_mac1_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac1_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->ipkt_mac1_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac2_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->ipkt_mac2_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac2_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->ipkt_mac2_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "mirrtbl_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->mirrtbl_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac0_ecc_ctrl", { const_cast<Dprsr_hi_regs_h_ipkt_mac0_ecc_ctrl(*)>(&Dprsr_hi_regs_h_base->ipkt_mac0_ecc_ctrl), new Dprsr_hi_regs_h_ipkt_mac0_ecc_ctrl_map, {}, sizeof(Dprsr_hi_regs_h_ipkt_mac0_ecc_ctrl) } },
    { "ipkt_mac1_ecc_ctrl", { const_cast<Dprsr_hi_regs_h_ipkt_mac1_ecc_ctrl(*)>(&Dprsr_hi_regs_h_base->ipkt_mac1_ecc_ctrl), new Dprsr_hi_regs_h_ipkt_mac1_ecc_ctrl_map, {}, sizeof(Dprsr_hi_regs_h_ipkt_mac1_ecc_ctrl) } },
    { "ipkt_mac2_ecc_ctrl", { const_cast<Dprsr_hi_regs_h_ipkt_mac2_ecc_ctrl(*)>(&Dprsr_hi_regs_h_base->ipkt_mac2_ecc_ctrl), new Dprsr_hi_regs_h_ipkt_mac2_ecc_ctrl_map, {}, sizeof(Dprsr_hi_regs_h_ipkt_mac2_ecc_ctrl) } },
    { "hdr_status", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->hdr_status), 0, {}, sizeof(uint32_t) } },
    { "hdr_latency_ctrl", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->hdr_latency_ctrl), 0, {}, sizeof(uint32_t) } },
    { "cfg48", { const_cast<Pp_ctr_cfg48_r(*)[0x2]>(&Dprsr_hi_regs_h_base->cfg48), new Pp_ctr_cfg48_r_map, {0x2}, sizeof(Pp_ctr_cfg48_r) } },
    { "cfg48_mask", { const_cast<uint32_t(*)[0x2]>(&Dprsr_hi_regs_h_base->cfg48_mask), 0, {0x2}, sizeof(uint32_t) } },
    { "cfg48_data", { const_cast<uint32_t(*)[0x2]>(&Dprsr_hi_regs_h_base->cfg48_data), 0, {0x2}, sizeof(uint32_t) } },
    { "max_src_chunks", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_h_base->max_src_chunks), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_header_ingress_meta_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_header_ingress_meta_g Dprsr_header_ingress_meta_g_base=0;
  Dprsr_header_ingress_meta_g_map() : RegisterMapper( {
    { "m_hash1", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_hash1), 0, {}, sizeof(uint32_t) } },
    { "m_hash2", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_hash2), 0, {}, sizeof(uint32_t) } },
    { "m_copy_to_cpu_cos", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_copy_to_cpu_cos), 0, {}, sizeof(uint32_t) } },
    { "m_deflect_on_drop", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_deflect_on_drop), 0, {}, sizeof(uint32_t) } },
    { "m_icos", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_icos), 0, {}, sizeof(uint32_t) } },
    { "m_pkt_color", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_pkt_color), 0, {}, sizeof(uint32_t) } },
    { "m_qid", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_qid), 0, {}, sizeof(uint32_t) } },
    { "m_xid_l1", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_xid_l1), 0, {}, sizeof(uint32_t) } },
    { "m_xid_l2", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_xid_l2), 0, {}, sizeof(uint32_t) } },
    { "m_rid", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_rid), 0, {}, sizeof(uint32_t) } },
    { "m_bypss_egr", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_bypss_egr), 0, {}, sizeof(uint32_t) } },
    { "m_ct_disable", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_ct_disable), 0, {}, sizeof(uint32_t) } },
    { "m_ct_mcast", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_ct_mcast), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_io_sel", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_mirr_io_sel), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_hash", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_mirr_hash), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_epipe_port", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_mirr_epipe_port), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_qid", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_mirr_qid), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_dond_ctrl", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_mirr_dond_ctrl), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_icos", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_mirr_icos), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_mc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_mirr_mc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_c2c_ctrl", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_mirr_c2c_ctrl), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_coal_smpl_len", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_mirr_coal_smpl_len), 0, {}, sizeof(uint32_t) } },
    { "m_afc", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_afc), 0, {}, sizeof(uint32_t) } },
    { "m_mtu_trunc_len", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_mtu_trunc_len), 0, {}, sizeof(uint32_t) } },
    { "m_mtu_trunc_err_f", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->m_mtu_trunc_err_f), 0, {}, sizeof(uint32_t) } },
    { "pre_version", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_meta_g_base->pre_version), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_hi_regs_map: public RegisterMapper {
  static constexpr PTR_Dprsr_hi_regs Dprsr_hi_regs_base=0;
  Dprsr_hi_regs_map() : RegisterMapper( {
    { "h", { &Dprsr_hi_regs_base->h, new Dprsr_hi_regs_h_map, {}, sizeof(Dprsr_hi_regs_h) } },
    { "meta", { &Dprsr_hi_regs_base->meta, new Dprsr_header_ingress_meta_g_map, {}, sizeof(Dprsr_header_ingress_meta_g) } },
    { "scratch", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_base->scratch), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_x_creds_pipe_map: public RegisterMapper {
  static constexpr PTR_Dprsr_x_creds_pipe Dprsr_x_creds_pipe_base=0;
  Dprsr_x_creds_pipe_map() : RegisterMapper( {
    { "chnl", { const_cast<uint32_t(*)[0x12]>(&Dprsr_x_creds_pipe_base->chnl), 0, {0x12}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_perf_byt_count_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_perf_byt_count_r Dprsr_perf_byt_count_r_base=0;
  Dprsr_perf_byt_count_r_map() : RegisterMapper( {
    { "perf_byt_0_2", { const_cast<uint32_t(*)>(&Dprsr_perf_byt_count_r_base->perf_byt_0_2), 0, {}, sizeof(uint32_t) } },
    { "perf_byt_1_2", { const_cast<uint32_t(*)>(&Dprsr_perf_byt_count_r_base->perf_byt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_perf_count_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_perf_count_r Dprsr_perf_count_r_base=0;
  Dprsr_perf_count_r_map() : RegisterMapper( {
    { "perf_pkt_0_2", { const_cast<uint32_t(*)>(&Dprsr_perf_count_r_base->perf_pkt_0_2), 0, {}, sizeof(uint32_t) } },
    { "perf_pkt_1_2", { const_cast<uint32_t(*)>(&Dprsr_perf_count_r_base->perf_pkt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_out_ingr_intr_0_map: public RegisterMapper {
  static constexpr PTR_Dprsr_out_ingr_intr_0 Dprsr_out_ingr_intr_0_base=0;
  Dprsr_out_ingr_intr_0_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_intr_0_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_intr_0_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_intr_0_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_intr_0_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_intr_0_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_out_ingr_intr_1_map: public RegisterMapper {
  static constexpr PTR_Dprsr_out_ingr_intr_1 Dprsr_out_ingr_intr_1_base=0;
  Dprsr_out_ingr_intr_1_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_intr_1_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_intr_1_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_intr_1_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_intr_1_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_intr_1_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_out_ingr_pktdata0_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_out_ingr_pktdata0_ecc_ctrl Dprsr_out_ingr_pktdata0_ecc_ctrl_base=0;
  Dprsr_out_ingr_pktdata0_ecc_ctrl_map() : RegisterMapper( {
    { "pktdata0_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_pktdata0_ecc_ctrl_base->pktdata0_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "pktdata0_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_pktdata0_ecc_ctrl_base->pktdata0_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_out_ingr_pktdata1_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_out_ingr_pktdata1_ecc_ctrl Dprsr_out_ingr_pktdata1_ecc_ctrl_base=0;
  Dprsr_out_ingr_pktdata1_ecc_ctrl_map() : RegisterMapper( {
    { "pktdata1_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_pktdata1_ecc_ctrl_base->pktdata1_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "pktdata1_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_pktdata1_ecc_ctrl_base->pktdata1_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_out_ingr_pktdata2_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_out_ingr_pktdata2_ecc_ctrl Dprsr_out_ingr_pktdata2_ecc_ctrl_base=0;
  Dprsr_out_ingr_pktdata2_ecc_ctrl_map() : RegisterMapper( {
    { "pktdata2_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_pktdata2_ecc_ctrl_base->pktdata2_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "pktdata2_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_pktdata2_ecc_ctrl_base->pktdata2_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_out_ingr_map: public RegisterMapper {
  static constexpr PTR_Dprsr_out_ingr Dprsr_out_ingr_base=0;
  Dprsr_out_ingr_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "cfg_s2p", { &Dprsr_out_ingr_base->cfg_s2p, new Dprsr_x_creds_pipe_map, {}, sizeof(Dprsr_x_creds_pipe) } },
    { "cfg_crc_dis", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->cfg_crc_dis), 0, {}, sizeof(uint32_t) } },
    { "cfg_crc_err_dis", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->cfg_crc_err_dis), 0, {}, sizeof(uint32_t) } },
    { "cfg_crc_err_inj", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->cfg_crc_err_inj), 0, {}, sizeof(uint32_t) } },
    { "cfg_crc_chk_dis", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->cfg_crc_chk_dis), 0, {}, sizeof(uint32_t) } },
    { "crd_status", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->crd_status), 0, {}, sizeof(uint32_t) } },
    { "perf_byt", { const_cast<Dprsr_perf_byt_count_r(*)[0x12]>(&Dprsr_out_ingr_base->perf_byt), new Dprsr_perf_byt_count_r_map, {0x12}, sizeof(Dprsr_perf_byt_count_r) } },
    { "perf_byt_time", { const_cast<Dprsr_perf_byt_count_r(*)>(&Dprsr_out_ingr_base->perf_byt_time), new Dprsr_perf_byt_count_r_map, {}, sizeof(Dprsr_perf_byt_count_r) } },
    { "perf_pkt", { const_cast<Dprsr_perf_count_r(*)[0x12]>(&Dprsr_out_ingr_base->perf_pkt), new Dprsr_perf_count_r_map, {0x12}, sizeof(Dprsr_perf_count_r) } },
    { "perf_pkt_time", { const_cast<Dprsr_perf_count_r(*)>(&Dprsr_out_ingr_base->perf_pkt_time), new Dprsr_perf_count_r_map, {}, sizeof(Dprsr_perf_count_r) } },
    { "perf_probe", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->perf_probe), 0, {}, sizeof(uint32_t) } },
    { "intr_0", { &Dprsr_out_ingr_base->intr_0, new Dprsr_out_ingr_intr_0_map, {}, sizeof(Dprsr_out_ingr_intr_0) } },
    { "intr_1", { &Dprsr_out_ingr_base->intr_1, new Dprsr_out_ingr_intr_1_map, {}, sizeof(Dprsr_out_ingr_intr_1) } },
    { "meta_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->meta_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "meta_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->meta_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "pkthdr_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->pkthdr_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "pkthdr_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->pkthdr_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "mirrhdr_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->mirrhdr_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "mirrhdr_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->mirrhdr_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "pktdata_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->pktdata_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "pktdata_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->pktdata_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "meta0_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->meta0_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "meta1_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->meta1_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "meta2_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->meta2_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pkthdr0_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->pkthdr0_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pkthdr1_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->pkthdr1_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pkthdr2_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->pkthdr2_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "mirrhdr0_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->mirrhdr0_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "mirrhdr1_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->mirrhdr1_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "mirrhdr2_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->mirrhdr2_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pktdata0_ecc_ctrl", { const_cast<Dprsr_out_ingr_pktdata0_ecc_ctrl(*)>(&Dprsr_out_ingr_base->pktdata0_ecc_ctrl), new Dprsr_out_ingr_pktdata0_ecc_ctrl_map, {}, sizeof(Dprsr_out_ingr_pktdata0_ecc_ctrl) } },
    { "pktdata1_ecc_ctrl", { const_cast<Dprsr_out_ingr_pktdata1_ecc_ctrl(*)>(&Dprsr_out_ingr_base->pktdata1_ecc_ctrl), new Dprsr_out_ingr_pktdata1_ecc_ctrl_map, {}, sizeof(Dprsr_out_ingr_pktdata1_ecc_ctrl) } },
    { "pktdata2_ecc_ctrl", { const_cast<Dprsr_out_ingr_pktdata2_ecc_ctrl(*)>(&Dprsr_out_ingr_base->pktdata2_ecc_ctrl), new Dprsr_out_ingr_pktdata2_ecc_ctrl_map, {}, sizeof(Dprsr_out_ingr_pktdata2_ecc_ctrl) } },
    { "u_mode", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->u_mode), 0, {}, sizeof(uint32_t) } },
    { "u_thresh_10G", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->u_thresh_10G), 0, {}, sizeof(uint32_t) } },
    { "u_thresh_25G", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->u_thresh_25G), 0, {}, sizeof(uint32_t) } },
    { "u_thresh_40G", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->u_thresh_40G), 0, {}, sizeof(uint32_t) } },
    { "u_thresh_50G", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->u_thresh_50G), 0, {}, sizeof(uint32_t) } },
    { "u_thresh_100G", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->u_thresh_100G), 0, {}, sizeof(uint32_t) } },
    { "u_thresh_200G", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->u_thresh_200G), 0, {}, sizeof(uint32_t) } },
    { "u_thresh_400G", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->u_thresh_400G), 0, {}, sizeof(uint32_t) } },
    { "ctl_chan_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->ctl_chan_err_log), 0, {}, sizeof(uint32_t) } },
    { "arb_fifo_cred", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->arb_fifo_cred), 0, {}, sizeof(uint32_t) } },
    { "cfg48", { const_cast<Pp_ctr_cfg48_r(*)[0x4]>(&Dprsr_out_ingr_base->cfg48), new Pp_ctr_cfg48_r_map, {0x4}, sizeof(Pp_ctr_cfg48_r) } },
    { "cfg48_mask", { const_cast<uint32_t(*)[0x4]>(&Dprsr_out_ingr_base->cfg48_mask), 0, {0x4}, sizeof(uint32_t) } },
    { "cfg48_data", { const_cast<uint32_t(*)[0x4]>(&Dprsr_out_ingr_base->cfg48_data), 0, {0x4}, sizeof(uint32_t) } },
    { "cfg48_data_sel", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->cfg48_data_sel), 0, {}, sizeof(uint32_t) } },
    { "ctrl_timeout", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->ctrl_timeout), 0, {}, sizeof(uint32_t) } },
    { "output_status", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->output_status), 0, {}, sizeof(uint32_t) } },
    { "diag_bus", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->diag_bus), 0, {}, sizeof(uint32_t) } },
    { "chan_status_cfg", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->chan_status_cfg), 0, {}, sizeof(uint32_t) } },
    { "chan_info", { const_cast<uint32_t(*)>(&Dprsr_out_ingr_base->chan_info), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ho_i_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ho_i Dprsr_ho_i_base=0;
  Dprsr_ho_i_map() : RegisterMapper( {
    { "him", { &Dprsr_ho_i_base->him, new Dprsr_hi_mem_map, {}, sizeof(Dprsr_hi_mem) } },
    { "hir", { &Dprsr_ho_i_base->hir, new Dprsr_hi_regs_map, {}, sizeof(Dprsr_hi_regs) } },
    { "out_ingr", { &Dprsr_ho_i_base->out_ingr, new Dprsr_out_ingr_map, {}, sizeof(Dprsr_out_ingr) } }
    } )
  {}
};

struct Dprsr_he_regs_h_intr_map: public RegisterMapper {
  static constexpr PTR_Dprsr_he_regs_h_intr Dprsr_he_regs_h_intr_base=0;
  Dprsr_he_regs_h_intr_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_intr_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_intr_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_intr_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_intr_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_he_regs_h_ipkt_mac0_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_he_regs_h_ipkt_mac0_ecc_ctrl Dprsr_he_regs_h_ipkt_mac0_ecc_ctrl_base=0;
  Dprsr_he_regs_h_ipkt_mac0_ecc_ctrl_map() : RegisterMapper( {
    { "ipkt_mac0_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_ipkt_mac0_ecc_ctrl_base->ipkt_mac0_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac0_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_ipkt_mac0_ecc_ctrl_base->ipkt_mac0_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_he_regs_h_ipkt_mac1_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_he_regs_h_ipkt_mac1_ecc_ctrl Dprsr_he_regs_h_ipkt_mac1_ecc_ctrl_base=0;
  Dprsr_he_regs_h_ipkt_mac1_ecc_ctrl_map() : RegisterMapper( {
    { "ipkt_mac1_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_ipkt_mac1_ecc_ctrl_base->ipkt_mac1_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac1_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_ipkt_mac1_ecc_ctrl_base->ipkt_mac1_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_he_regs_h_ipkt_mac2_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_he_regs_h_ipkt_mac2_ecc_ctrl Dprsr_he_regs_h_ipkt_mac2_ecc_ctrl_base=0;
  Dprsr_he_regs_h_ipkt_mac2_ecc_ctrl_map() : RegisterMapper( {
    { "ipkt_mac2_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_ipkt_mac2_ecc_ctrl_base->ipkt_mac2_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac2_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_ipkt_mac2_ecc_ctrl_base->ipkt_mac2_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_he_regs_h_map: public RegisterMapper {
  static constexpr PTR_Dprsr_he_regs_h Dprsr_he_regs_h_base=0;
  Dprsr_he_regs_h_map() : RegisterMapper( {
    { "compress_clot_sel", { const_cast<uint32_t(*)[0x10]>(&Dprsr_he_regs_h_base->compress_clot_sel), 0, {0x10}, sizeof(uint32_t) } },
    { "hdr_xbar_const", { const_cast<Dprsr_hdr_xbar_const_defs_r(*)>(&Dprsr_he_regs_h_base->hdr_xbar_const), new Dprsr_hdr_xbar_const_defs_r_map, {}, sizeof(Dprsr_hdr_xbar_const_defs_r) } },
    { "mirr_hdr_tbl", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->mirr_hdr_tbl), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "cred_thresh", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->cred_thresh), 0, {}, sizeof(uint32_t) } },
    { "cred_max", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->cred_max), 0, {}, sizeof(uint32_t) } },
    { "cred_dbg_chan_sel", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->cred_dbg_chan_sel), 0, {}, sizeof(uint32_t) } },
    { "cred_dbg_credits", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->cred_dbg_credits), 0, {}, sizeof(uint32_t) } },
    { "cred_dbg_reload", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->cred_dbg_reload), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Dprsr_he_regs_h_base->intr, new Dprsr_he_regs_h_intr_map, {}, sizeof(Dprsr_he_regs_h_intr) } },
    { "mirrtbl_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->mirrtbl_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "mirrtbl_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->mirrtbl_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac0_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->ipkt_mac0_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac0_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->ipkt_mac0_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac1_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->ipkt_mac1_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac1_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->ipkt_mac1_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac2_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->ipkt_mac2_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac2_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->ipkt_mac2_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "mirrtbl_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->mirrtbl_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "ipkt_mac0_ecc_ctrl", { const_cast<Dprsr_he_regs_h_ipkt_mac0_ecc_ctrl(*)>(&Dprsr_he_regs_h_base->ipkt_mac0_ecc_ctrl), new Dprsr_he_regs_h_ipkt_mac0_ecc_ctrl_map, {}, sizeof(Dprsr_he_regs_h_ipkt_mac0_ecc_ctrl) } },
    { "ipkt_mac1_ecc_ctrl", { const_cast<Dprsr_he_regs_h_ipkt_mac1_ecc_ctrl(*)>(&Dprsr_he_regs_h_base->ipkt_mac1_ecc_ctrl), new Dprsr_he_regs_h_ipkt_mac1_ecc_ctrl_map, {}, sizeof(Dprsr_he_regs_h_ipkt_mac1_ecc_ctrl) } },
    { "ipkt_mac2_ecc_ctrl", { const_cast<Dprsr_he_regs_h_ipkt_mac2_ecc_ctrl(*)>(&Dprsr_he_regs_h_base->ipkt_mac2_ecc_ctrl), new Dprsr_he_regs_h_ipkt_mac2_ecc_ctrl_map, {}, sizeof(Dprsr_he_regs_h_ipkt_mac2_ecc_ctrl) } },
    { "hdr_status", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->hdr_status), 0, {}, sizeof(uint32_t) } },
    { "hdr_latency_ctrl", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->hdr_latency_ctrl), 0, {}, sizeof(uint32_t) } },
    { "cfg48", { const_cast<Pp_ctr_cfg48_r(*)[0x2]>(&Dprsr_he_regs_h_base->cfg48), new Pp_ctr_cfg48_r_map, {0x2}, sizeof(Pp_ctr_cfg48_r) } },
    { "cfg48_mask", { const_cast<uint32_t(*)[0x2]>(&Dprsr_he_regs_h_base->cfg48_mask), 0, {0x2}, sizeof(uint32_t) } },
    { "cfg48_data", { const_cast<uint32_t(*)[0x2]>(&Dprsr_he_regs_h_base->cfg48_data), 0, {0x2}, sizeof(uint32_t) } },
    { "max_src_chunks", { const_cast<uint32_t(*)>(&Dprsr_he_regs_h_base->max_src_chunks), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_header_egress_meta_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_header_egress_meta_g Dprsr_header_egress_meta_g_base=0;
  Dprsr_header_egress_meta_g_map() : RegisterMapper( {
    { "m_force_tx_err", { const_cast<uint32_t(*)>(&Dprsr_header_egress_meta_g_base->m_force_tx_err), 0, {}, sizeof(uint32_t) } },
    { "m_capture_tx_ts", { const_cast<uint32_t(*)>(&Dprsr_header_egress_meta_g_base->m_capture_tx_ts), 0, {}, sizeof(uint32_t) } },
    { "m_tx_pkt_has_offsets", { const_cast<uint32_t(*)>(&Dprsr_header_egress_meta_g_base->m_tx_pkt_has_offsets), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_io_sel", { const_cast<uint32_t(*)>(&Dprsr_header_egress_meta_g_base->m_mirr_io_sel), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_hash", { const_cast<uint32_t(*)>(&Dprsr_header_egress_meta_g_base->m_mirr_hash), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_epipe_port", { const_cast<uint32_t(*)>(&Dprsr_header_egress_meta_g_base->m_mirr_epipe_port), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_qid", { const_cast<uint32_t(*)>(&Dprsr_header_egress_meta_g_base->m_mirr_qid), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_dond_ctrl", { const_cast<uint32_t(*)>(&Dprsr_header_egress_meta_g_base->m_mirr_dond_ctrl), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_icos", { const_cast<uint32_t(*)>(&Dprsr_header_egress_meta_g_base->m_mirr_icos), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_mc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_header_egress_meta_g_base->m_mirr_mc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_c2c_ctrl", { const_cast<uint32_t(*)>(&Dprsr_header_egress_meta_g_base->m_mirr_c2c_ctrl), 0, {}, sizeof(uint32_t) } },
    { "m_mirr_coal_smpl_len", { const_cast<uint32_t(*)>(&Dprsr_header_egress_meta_g_base->m_mirr_coal_smpl_len), 0, {}, sizeof(uint32_t) } },
    { "m_afc", { const_cast<uint32_t(*)>(&Dprsr_header_egress_meta_g_base->m_afc), 0, {}, sizeof(uint32_t) } },
    { "m_mtu_trunc_len", { const_cast<uint32_t(*)>(&Dprsr_header_egress_meta_g_base->m_mtu_trunc_len), 0, {}, sizeof(uint32_t) } },
    { "m_mtu_trunc_err_f", { const_cast<uint32_t(*)>(&Dprsr_header_egress_meta_g_base->m_mtu_trunc_err_f), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_he_regs_map: public RegisterMapper {
  static constexpr PTR_Dprsr_he_regs Dprsr_he_regs_base=0;
  Dprsr_he_regs_map() : RegisterMapper( {
    { "h", { &Dprsr_he_regs_base->h, new Dprsr_he_regs_h_map, {}, sizeof(Dprsr_he_regs_h) } },
    { "meta", { &Dprsr_he_regs_base->meta, new Dprsr_header_egress_meta_g_map, {}, sizeof(Dprsr_header_egress_meta_g) } },
    { "scratch", { const_cast<uint32_t(*)>(&Dprsr_he_regs_base->scratch), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_epb_bp_thresh_pipe_map: public RegisterMapper {
  static constexpr PTR_Dprsr_epb_bp_thresh_pipe Dprsr_epb_bp_thresh_pipe_base=0;
  Dprsr_epb_bp_thresh_pipe_map() : RegisterMapper( {
    { "chnl", { const_cast<uint32_t(*)[0x12]>(&Dprsr_epb_bp_thresh_pipe_base->chnl), 0, {0x12}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_out_egr_intr_0_map: public RegisterMapper {
  static constexpr PTR_Dprsr_out_egr_intr_0 Dprsr_out_egr_intr_0_base=0;
  Dprsr_out_egr_intr_0_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Dprsr_out_egr_intr_0_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Dprsr_out_egr_intr_0_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Dprsr_out_egr_intr_0_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Dprsr_out_egr_intr_0_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Dprsr_out_egr_intr_0_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_out_egr_intr_1_map: public RegisterMapper {
  static constexpr PTR_Dprsr_out_egr_intr_1 Dprsr_out_egr_intr_1_base=0;
  Dprsr_out_egr_intr_1_map() : RegisterMapper( {
    { "stat", { const_cast<uint32_t(*)>(&Dprsr_out_egr_intr_1_base->stat), 0, {}, sizeof(uint32_t) } },
    { "en0", { const_cast<uint32_t(*)>(&Dprsr_out_egr_intr_1_base->en0), 0, {}, sizeof(uint32_t) } },
    { "en1", { const_cast<uint32_t(*)>(&Dprsr_out_egr_intr_1_base->en1), 0, {}, sizeof(uint32_t) } },
    { "inj", { const_cast<uint32_t(*)>(&Dprsr_out_egr_intr_1_base->inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Dprsr_out_egr_intr_1_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_out_egr_pktdata0_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_out_egr_pktdata0_ecc_ctrl Dprsr_out_egr_pktdata0_ecc_ctrl_base=0;
  Dprsr_out_egr_pktdata0_ecc_ctrl_map() : RegisterMapper( {
    { "pktdata0_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_out_egr_pktdata0_ecc_ctrl_base->pktdata0_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "pktdata0_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_out_egr_pktdata0_ecc_ctrl_base->pktdata0_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_out_egr_pktdata1_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_out_egr_pktdata1_ecc_ctrl Dprsr_out_egr_pktdata1_ecc_ctrl_base=0;
  Dprsr_out_egr_pktdata1_ecc_ctrl_map() : RegisterMapper( {
    { "pktdata1_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_out_egr_pktdata1_ecc_ctrl_base->pktdata1_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "pktdata1_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_out_egr_pktdata1_ecc_ctrl_base->pktdata1_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_out_egr_pktdata2_ecc_ctrl_map: public RegisterMapper {
  static constexpr PTR_Dprsr_out_egr_pktdata2_ecc_ctrl Dprsr_out_egr_pktdata2_ecc_ctrl_base=0;
  Dprsr_out_egr_pktdata2_ecc_ctrl_map() : RegisterMapper( {
    { "pktdata2_ecc_ctrl_0_2", { const_cast<uint32_t(*)>(&Dprsr_out_egr_pktdata2_ecc_ctrl_base->pktdata2_ecc_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "pktdata2_ecc_ctrl_1_2", { const_cast<uint32_t(*)>(&Dprsr_out_egr_pktdata2_ecc_ctrl_base->pktdata2_ecc_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_out_egr_map: public RegisterMapper {
  static constexpr PTR_Dprsr_out_egr Dprsr_out_egr_base=0;
  Dprsr_out_egr_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "epb_bp", { &Dprsr_out_egr_base->epb_bp, new Dprsr_epb_bp_thresh_pipe_map, {}, sizeof(Dprsr_epb_bp_thresh_pipe) } },
    { "cfg_ebuf", { &Dprsr_out_egr_base->cfg_ebuf, new Dprsr_x_creds_pipe_map, {}, sizeof(Dprsr_x_creds_pipe) } },
    { "cfg_crc_dis", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->cfg_crc_dis), 0, {}, sizeof(uint32_t) } },
    { "cfg_crc_err_dis", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->cfg_crc_err_dis), 0, {}, sizeof(uint32_t) } },
    { "cfg_crc_err_inj", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->cfg_crc_err_inj), 0, {}, sizeof(uint32_t) } },
    { "cfg_crc_chk_dis", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->cfg_crc_chk_dis), 0, {}, sizeof(uint32_t) } },
    { "cfg_bytadj", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->cfg_bytadj), 0, {}, sizeof(uint32_t) } },
    { "crd_status", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->crd_status), 0, {}, sizeof(uint32_t) } },
    { "perf_byt", { const_cast<Dprsr_perf_byt_count_r(*)[0x12]>(&Dprsr_out_egr_base->perf_byt), new Dprsr_perf_byt_count_r_map, {0x12}, sizeof(Dprsr_perf_byt_count_r) } },
    { "perf_byt_time", { const_cast<Dprsr_perf_byt_count_r(*)>(&Dprsr_out_egr_base->perf_byt_time), new Dprsr_perf_byt_count_r_map, {}, sizeof(Dprsr_perf_byt_count_r) } },
    { "perf_pkt", { const_cast<Dprsr_perf_count_r(*)[0x12]>(&Dprsr_out_egr_base->perf_pkt), new Dprsr_perf_count_r_map, {0x12}, sizeof(Dprsr_perf_count_r) } },
    { "perf_pkt_time", { const_cast<Dprsr_perf_count_r(*)>(&Dprsr_out_egr_base->perf_pkt_time), new Dprsr_perf_count_r_map, {}, sizeof(Dprsr_perf_count_r) } },
    { "perf_probe", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->perf_probe), 0, {}, sizeof(uint32_t) } },
    { "intr_0", { &Dprsr_out_egr_base->intr_0, new Dprsr_out_egr_intr_0_map, {}, sizeof(Dprsr_out_egr_intr_0) } },
    { "intr_1", { &Dprsr_out_egr_base->intr_1, new Dprsr_out_egr_intr_1_map, {}, sizeof(Dprsr_out_egr_intr_1) } },
    { "meta_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->meta_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "meta_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->meta_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "pkthdr_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->pkthdr_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "pkthdr_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->pkthdr_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "mirrhdr_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->mirrhdr_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "mirrhdr_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->mirrhdr_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "pktdata_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->pktdata_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "pktdata_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->pktdata_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "tmsch_sbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->tmsch_sbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "tmsch_mbe_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->tmsch_mbe_err_log), 0, {}, sizeof(uint32_t) } },
    { "meta0_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->meta0_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "meta1_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->meta1_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "meta2_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->meta2_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pkthdr0_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->pkthdr0_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pkthdr1_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->pkthdr1_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pkthdr2_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->pkthdr2_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "mirrhdr0_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->mirrhdr0_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "mirrhdr1_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->mirrhdr1_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "mirrhdr2_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->mirrhdr2_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pktdata0_ecc_ctrl", { const_cast<Dprsr_out_egr_pktdata0_ecc_ctrl(*)>(&Dprsr_out_egr_base->pktdata0_ecc_ctrl), new Dprsr_out_egr_pktdata0_ecc_ctrl_map, {}, sizeof(Dprsr_out_egr_pktdata0_ecc_ctrl) } },
    { "pktdata1_ecc_ctrl", { const_cast<Dprsr_out_egr_pktdata1_ecc_ctrl(*)>(&Dprsr_out_egr_base->pktdata1_ecc_ctrl), new Dprsr_out_egr_pktdata1_ecc_ctrl_map, {}, sizeof(Dprsr_out_egr_pktdata1_ecc_ctrl) } },
    { "pktdata2_ecc_ctrl", { const_cast<Dprsr_out_egr_pktdata2_ecc_ctrl(*)>(&Dprsr_out_egr_base->pktdata2_ecc_ctrl), new Dprsr_out_egr_pktdata2_ecc_ctrl_map, {}, sizeof(Dprsr_out_egr_pktdata2_ecc_ctrl) } },
    { "tmsch0_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->tmsch0_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "tmsch1_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->tmsch1_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "tmsch2_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->tmsch2_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "u_mode", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->u_mode), 0, {}, sizeof(uint32_t) } },
    { "u_thresh_10G", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->u_thresh_10G), 0, {}, sizeof(uint32_t) } },
    { "u_thresh_25G", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->u_thresh_25G), 0, {}, sizeof(uint32_t) } },
    { "u_thresh_40G", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->u_thresh_40G), 0, {}, sizeof(uint32_t) } },
    { "u_thresh_50G", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->u_thresh_50G), 0, {}, sizeof(uint32_t) } },
    { "u_thresh_100G", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->u_thresh_100G), 0, {}, sizeof(uint32_t) } },
    { "u_thresh_200G", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->u_thresh_200G), 0, {}, sizeof(uint32_t) } },
    { "u_thresh_400G", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->u_thresh_400G), 0, {}, sizeof(uint32_t) } },
    { "teop", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->teop), 0, {}, sizeof(uint32_t) } },
    { "ctl_chan_err_log", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->ctl_chan_err_log), 0, {}, sizeof(uint32_t) } },
    { "arb_fifo_cred", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->arb_fifo_cred), 0, {}, sizeof(uint32_t) } },
    { "cfg48", { const_cast<Pp_ctr_cfg48_r(*)[0x4]>(&Dprsr_out_egr_base->cfg48), new Pp_ctr_cfg48_r_map, {0x4}, sizeof(Pp_ctr_cfg48_r) } },
    { "cfg48_mask", { const_cast<uint32_t(*)[0x4]>(&Dprsr_out_egr_base->cfg48_mask), 0, {0x4}, sizeof(uint32_t) } },
    { "cfg48_data", { const_cast<uint32_t(*)[0x4]>(&Dprsr_out_egr_base->cfg48_data), 0, {0x4}, sizeof(uint32_t) } },
    { "cfg48_data_sel", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->cfg48_data_sel), 0, {}, sizeof(uint32_t) } },
    { "ctrl_timeout", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->ctrl_timeout), 0, {}, sizeof(uint32_t) } },
    { "output_status", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->output_status), 0, {}, sizeof(uint32_t) } },
    { "diag_bus", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->diag_bus), 0, {}, sizeof(uint32_t) } },
    { "chan_status_cfg", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->chan_status_cfg), 0, {}, sizeof(uint32_t) } },
    { "chan_info", { const_cast<uint32_t(*)>(&Dprsr_out_egr_base->chan_info), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ho_e_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ho_e Dprsr_ho_e_base=0;
  Dprsr_ho_e_map() : RegisterMapper( {
    { "hem", { &Dprsr_ho_e_base->hem, new Dprsr_hi_mem_map, {}, sizeof(Dprsr_hi_mem) } },
    { "her", { &Dprsr_ho_e_base->her, new Dprsr_he_regs_map, {}, sizeof(Dprsr_he_regs) } },
    { "out_egr", { &Dprsr_ho_e_base->out_egr, new Dprsr_out_egr_map, {}, sizeof(Dprsr_out_egr) } }
    } )
  {}
};

struct Dprsr_reg_rspec_map: public RegisterMapper {
  static constexpr PTR_Dprsr_reg_rspec Dprsr_reg_rspec_base=0;
  Dprsr_reg_rspec_map() : RegisterMapper( {
    { "inp", { &Dprsr_reg_rspec_base->inp, new Dprsr_inp_map, {}, sizeof(Dprsr_inp) } },
    { "inpslice", { &Dprsr_reg_rspec_base->inpslice, new Dprsr_inp_slice_map, {0x4}, sizeof(Dprsr_inp_slice) } },
    { "dprsr_pbus", { &Dprsr_reg_rspec_base->dprsr_pbus, new Pbus_station_regs_rspec_map, {}, sizeof(Pbus_station_regs_rspec) } },
    { "dprsr_csr_ring", { &Dprsr_reg_rspec_base->dprsr_csr_ring, new Parde_dprsr_reg_rspec_map, {}, sizeof(Parde_dprsr_reg_rspec) } },
    { "ho_i", { &Dprsr_reg_rspec_base->ho_i, new Dprsr_ho_i_map, {0x4}, sizeof(Dprsr_ho_i) } },
    { "ho_e", { &Dprsr_reg_rspec_base->ho_e, new Dprsr_ho_e_map, {0x4}, sizeof(Dprsr_ho_e) } }
    } )
  {}
};

struct Dprsr_reg_map: public RegisterMapper {
  static constexpr PTR_Dprsr_reg Dprsr_reg_base=0;
  Dprsr_reg_map() : RegisterMapper( {
    { "dprsrreg", { &Dprsr_reg_base->dprsrreg, new Dprsr_reg_rspec_map, {}, sizeof(Dprsr_reg_rspec) } }
    } )
  {}
};

struct Parde_reg_map: public RegisterMapper {
  static constexpr PTR_Parde_reg Parde_reg_base=0;
  Parde_reg_map() : RegisterMapper( {
    { "pgstnreg", { &Parde_reg_base->pgstnreg, new Parde_glue_stn_reg_map, {}, sizeof(Parde_glue_stn_reg) } },
    { "mirreg", { &Parde_reg_base->mirreg, new Mirr_reg_map, {}, sizeof(Mirr_reg) } },
    { "dprsrreg", { &Parde_reg_base->dprsrreg, new Dprsr_reg_map, {}, sizeof(Dprsr_reg) } }
    } )
  {}
};

struct Pipe_addrmap_map: public RegisterMapper {
  static constexpr PTR_Pipe_addrmap Pipe_addrmap_base=0;
  Pipe_addrmap_map() : RegisterMapper( {
    { "mau", { &Pipe_addrmap_base->mau, new Mau_addrmap_map, {0x14}, sizeof(Mau_addrmap) } },
    { "pardereg", { &Pipe_addrmap_base->pardereg, new Parde_reg_map, {}, sizeof(Parde_reg) } }
    } )
  {}
};

struct Jbay_reg_map: public RegisterMapper {
  static constexpr PTR_Jbay_reg Jbay_reg_base=0;
  Jbay_reg_map() : RegisterMapper( {
    { "device_select", { &Jbay_reg_base->device_select, new Dvsl_addrmap_map, {}, sizeof(Dvsl_addrmap) } },
    { "eth100g_regs", { &Jbay_reg_base->eth100g_regs, new Eth100g_addrmap_map, {}, sizeof(Eth100g_addrmap) } },
    { "eth400g_p1", { &Jbay_reg_base->eth400g_p1, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p2", { &Jbay_reg_base->eth400g_p2, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p3", { &Jbay_reg_base->eth400g_p3, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p4", { &Jbay_reg_base->eth400g_p4, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p5", { &Jbay_reg_base->eth400g_p5, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p6", { &Jbay_reg_base->eth400g_p6, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p7", { &Jbay_reg_base->eth400g_p7, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p8", { &Jbay_reg_base->eth400g_p8, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p9", { &Jbay_reg_base->eth400g_p9, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p10", { &Jbay_reg_base->eth400g_p10, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p11", { &Jbay_reg_base->eth400g_p11, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p12", { &Jbay_reg_base->eth400g_p12, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p13", { &Jbay_reg_base->eth400g_p13, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p14", { &Jbay_reg_base->eth400g_p14, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p15", { &Jbay_reg_base->eth400g_p15, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p16", { &Jbay_reg_base->eth400g_p16, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p17", { &Jbay_reg_base->eth400g_p17, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p18", { &Jbay_reg_base->eth400g_p18, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p19", { &Jbay_reg_base->eth400g_p19, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p20", { &Jbay_reg_base->eth400g_p20, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p21", { &Jbay_reg_base->eth400g_p21, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p22", { &Jbay_reg_base->eth400g_p22, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p23", { &Jbay_reg_base->eth400g_p23, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p24", { &Jbay_reg_base->eth400g_p24, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p25", { &Jbay_reg_base->eth400g_p25, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p26", { &Jbay_reg_base->eth400g_p26, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p27", { &Jbay_reg_base->eth400g_p27, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p28", { &Jbay_reg_base->eth400g_p28, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p29", { &Jbay_reg_base->eth400g_p29, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p30", { &Jbay_reg_base->eth400g_p30, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p31", { &Jbay_reg_base->eth400g_p31, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "eth400g_p32", { &Jbay_reg_base->eth400g_p32, new Eth400g_addrmap_map, {}, sizeof(Eth400g_addrmap) } },
    { "ethgpiobr", { &Jbay_reg_base->ethgpiobr, new Eth_gpio_regs_map, {}, sizeof(Eth_gpio_regs) } },
    { "ethgpiotl", { &Jbay_reg_base->ethgpiotl, new Eth_gpio_regs_map, {}, sizeof(Eth_gpio_regs) } },
    { "gpio_iotile_bl", { &Jbay_reg_base->gpio_iotile_bl, new Gpio_iotile_rspec_map, {}, sizeof(Gpio_iotile_rspec) } },
    { "gpio_iotile_br", { &Jbay_reg_base->gpio_iotile_br, new Gpio_iotile_rspec_map, {}, sizeof(Gpio_iotile_rspec) } },
    { "gpio_iotile_tr", { &Jbay_reg_base->gpio_iotile_tr, new Gpio_iotile_rspec_map, {}, sizeof(Gpio_iotile_rspec) } },
    { "gpio_iotile_tl", { &Jbay_reg_base->gpio_iotile_tl, new Gpio_iotile_rspec_map, {}, sizeof(Gpio_iotile_rspec) } },
    { "eth100g_regs_rot", { &Jbay_reg_base->eth100g_regs_rot, new Eth100g_addrmap_map, {}, sizeof(Eth100g_addrmap) } },
    { "serdes", { &Jbay_reg_base->serdes, new Serdes_addrmap_map, {}, sizeof(Serdes_addrmap) } },
    { "pipes", { &Jbay_reg_base->pipes, new Pipe_addrmap_map, {0x4}, sizeof(Pipe_addrmap) } }
    } )
  {}
};

} // namespace MODEL_CHIP_TEST_NAMESPACE
#endif
