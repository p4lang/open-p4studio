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

struct Pcie_bar01_group_map: public RegisterMapper {
  static constexpr PTR_Pcie_bar01_group Pcie_bar01_group_base=0;
  Pcie_bar01_group_map() : RegisterMapper( {
    { "scratch_reg", { const_cast<uint32_t(*)[0x4]>(&Pcie_bar01_group_base->scratch_reg), 0, {0x4}, sizeof(uint32_t) } },
    { "freerun_cnt", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->freerun_cnt), 0, {}, sizeof(uint32_t) } },
    { "dma_glb_ctrl", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->dma_glb_ctrl), 0, {}, sizeof(uint32_t) } },
    { "wrr_table0", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->wrr_table0), 0, {}, sizeof(uint32_t) } },
    { "wrr_table1", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->wrr_table1), 0, {}, sizeof(uint32_t) } },
    { "wrr_table2", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->wrr_table2), 0, {}, sizeof(uint32_t) } },
    { "wrr_table3", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->wrr_table3), 0, {}, sizeof(uint32_t) } },
    { "dmard_thruput_ctrl", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->dmard_thruput_ctrl), 0, {}, sizeof(uint32_t) } },
    { "int_timeout_ctrl", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->int_timeout_ctrl), 0, {}, sizeof(uint32_t) } },
    { "cpu_glb_ctrl", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->cpu_glb_ctrl), 0, {}, sizeof(uint32_t) } },
    { "cpu_ind_addr_low", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->cpu_ind_addr_low), 0, {}, sizeof(uint32_t) } },
    { "cpu_ind_addr_high", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->cpu_ind_addr_high), 0, {}, sizeof(uint32_t) } },
    { "cpu_ind_data00", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->cpu_ind_data00), 0, {}, sizeof(uint32_t) } },
    { "cpu_ind_data01", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->cpu_ind_data01), 0, {}, sizeof(uint32_t) } },
    { "cpu_ind_data10", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->cpu_ind_data10), 0, {}, sizeof(uint32_t) } },
    { "cpu_ind_data11", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->cpu_ind_data11), 0, {}, sizeof(uint32_t) } },
    { "cpu_ind_rerr", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->cpu_ind_rerr), 0, {}, sizeof(uint32_t) } },
    { "dma_tag_pndg", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->dma_tag_pndg), 0, {}, sizeof(uint32_t) } },
    { "glb_shadow_int", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->glb_shadow_int), 0, {}, sizeof(uint32_t) } },
    { "shadow_int", { const_cast<uint32_t(*)[0x10]>(&Pcie_bar01_group_base->shadow_int), 0, {0x10}, sizeof(uint32_t) } },
    { "shadow_msk", { const_cast<uint32_t(*)[0x10]>(&Pcie_bar01_group_base->shadow_msk), 0, {0x10}, sizeof(uint32_t) } },
    { "window0_base_param", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->window0_base_param), 0, {}, sizeof(uint32_t) } },
    { "window0_base_high", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->window0_base_high), 0, {}, sizeof(uint32_t) } },
    { "window0_limit_low", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->window0_limit_low), 0, {}, sizeof(uint32_t) } },
    { "window0_limit_high", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->window0_limit_high), 0, {}, sizeof(uint32_t) } },
    { "window1_base_param", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->window1_base_param), 0, {}, sizeof(uint32_t) } },
    { "window1_base_high", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->window1_base_high), 0, {}, sizeof(uint32_t) } },
    { "window1_limit_low", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->window1_limit_low), 0, {}, sizeof(uint32_t) } },
    { "window1_limit_high", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->window1_limit_high), 0, {}, sizeof(uint32_t) } },
    { "default_pciehdr_param", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->default_pciehdr_param), 0, {}, sizeof(uint32_t) } },
    { "pcie_int_stat", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->pcie_int_stat), 0, {}, sizeof(uint32_t) } },
    { "pcie_int_en", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->pcie_int_en), 0, {}, sizeof(uint32_t) } },
    { "pcie_int_inj", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->pcie_int_inj), 0, {}, sizeof(uint32_t) } },
    { "pcie_ram_err_addr", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->pcie_ram_err_addr), 0, {}, sizeof(uint32_t) } },
    { "msix_ram_err_addr", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->msix_ram_err_addr), 0, {}, sizeof(uint32_t) } },
    { "pcie_dev_info", { const_cast<uint32_t(*)[0x8]>(&Pcie_bar01_group_base->pcie_dev_info), 0, {0x8}, sizeof(uint32_t) } },
    { "pcie_bus_dev", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->pcie_bus_dev), 0, {}, sizeof(uint32_t) } },
    { "pcie_dma_temp_stall", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->pcie_dma_temp_stall), 0, {}, sizeof(uint32_t) } },
    { "pcie_mst_cred", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->pcie_mst_cred), 0, {}, sizeof(uint32_t) } },
    { "pcie_int_freeze", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->pcie_int_freeze), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Pcie_bar01_group_base->dft_csr), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pcie_bar01_regs_map: public RegisterMapper {
  static constexpr PTR_Pcie_bar01_regs Pcie_bar01_regs_base=0;
  Pcie_bar01_regs_map() : RegisterMapper( {
    { "pcie_regs", { &Pcie_bar01_regs_base->pcie_regs, new Pcie_bar01_group_map, {}, sizeof(Pcie_bar01_group) } }
    } )
  {}
};

struct Misc_regs_map: public RegisterMapper {
  static constexpr PTR_Misc_regs Misc_regs_base=0;
  Misc_regs_map() : RegisterMapper( {
    { "soft_reset", { const_cast<uint32_t(*)>(&Misc_regs_base->soft_reset), 0, {}, sizeof(uint32_t) } },
    { "refclk_pad_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->refclk_pad_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pcie_pll_ctrl0", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_pll_ctrl0), 0, {}, sizeof(uint32_t) } },
    { "pcie_pll_ctrl1", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_pll_ctrl1), 0, {}, sizeof(uint32_t) } },
    { "core_pll_ctrl0", { const_cast<uint32_t(*)>(&Misc_regs_base->core_pll_ctrl0), 0, {}, sizeof(uint32_t) } },
    { "core_pll_ctrl1", { const_cast<uint32_t(*)>(&Misc_regs_base->core_pll_ctrl1), 0, {}, sizeof(uint32_t) } },
    { "mac_pll_ctrl0", { const_cast<uint32_t(*)>(&Misc_regs_base->mac_pll_ctrl0), 0, {}, sizeof(uint32_t) } },
    { "mac_pll_ctrl1", { const_cast<uint32_t(*)>(&Misc_regs_base->mac_pll_ctrl1), 0, {}, sizeof(uint32_t) } },
    { "pcie_int_pwrup_enable", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_int_pwrup_enable), 0, {}, sizeof(uint32_t) } },
    { "pcie_int_gen1_enable", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_int_gen1_enable), 0, {}, sizeof(uint32_t) } },
    { "pcie_int_gen2_enable", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_int_gen2_enable), 0, {}, sizeof(uint32_t) } },
    { "pcie_int_gen3_enable", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_int_gen3_enable), 0, {}, sizeof(uint32_t) } },
    { "pcie_int_pwrup_ctrl", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pcie_int_pwrup_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "pcie_int_gen1_ctrl", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pcie_int_gen1_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "pcie_int_gen2_ctrl", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pcie_int_gen2_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "pcie_int_gen3_ctrl", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pcie_int_gen3_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "pciephy_side_cntl", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pciephy_side_cntl), 0, {0x4}, sizeof(uint32_t) } },
    { "pciephy_status0", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pciephy_status0), 0, {0x4}, sizeof(uint32_t) } },
    { "pciephy_status1", { const_cast<uint32_t(*)[0x4]>(&Misc_regs_base->pciephy_status1), 0, {0x4}, sizeof(uint32_t) } },
    { "int_stat", { const_cast<uint32_t(*)>(&Misc_regs_base->int_stat), 0, {}, sizeof(uint32_t) } },
    { "int_en", { const_cast<uint32_t(*)>(&Misc_regs_base->int_en), 0, {}, sizeof(uint32_t) } },
    { "int_pri", { const_cast<uint32_t(*)>(&Misc_regs_base->int_pri), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Misc_regs_base->int_inj), 0, {}, sizeof(uint32_t) } },
    { "gpio_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->gpio_ctrl), 0, {}, sizeof(uint32_t) } },
    { "sbm_ind_wdata", { const_cast<uint32_t(*)>(&Misc_regs_base->sbm_ind_wdata), 0, {}, sizeof(uint32_t) } },
    { "sbm_ind_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->sbm_ind_ctrl), 0, {}, sizeof(uint32_t) } },
    { "sbm_ind_rdata", { const_cast<uint32_t(*)>(&Misc_regs_base->sbm_ind_rdata), 0, {}, sizeof(uint32_t) } },
    { "sbm_ind_rslt", { const_cast<uint32_t(*)>(&Misc_regs_base->sbm_ind_rslt), 0, {}, sizeof(uint32_t) } },
    { "sbm_timeout_ena", { const_cast<uint32_t(*)>(&Misc_regs_base->sbm_timeout_ena), 0, {}, sizeof(uint32_t) } },
    { "spi_outdata0", { const_cast<uint32_t(*)>(&Misc_regs_base->spi_outdata0), 0, {}, sizeof(uint32_t) } },
    { "spi_outdata1", { const_cast<uint32_t(*)>(&Misc_regs_base->spi_outdata1), 0, {}, sizeof(uint32_t) } },
    { "spi_command", { const_cast<uint32_t(*)>(&Misc_regs_base->spi_command), 0, {}, sizeof(uint32_t) } },
    { "spi_indata", { const_cast<uint32_t(*)>(&Misc_regs_base->spi_indata), 0, {}, sizeof(uint32_t) } },
    { "spi_idcode", { const_cast<uint32_t(*)>(&Misc_regs_base->spi_idcode), 0, {}, sizeof(uint32_t) } },
    { "pciectl_gen3_default", { const_cast<uint32_t(*)>(&Misc_regs_base->pciectl_gen3_default), 0, {}, sizeof(uint32_t) } },
    { "pcie_rxeq_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_rxeq_ctrl), 0, {}, sizeof(uint32_t) } },
    { "fuse_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->fuse_ctrl), 0, {}, sizeof(uint32_t) } },
    { "baresync_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->baresync_ctrl), 0, {}, sizeof(uint32_t) } },
    { "linkdown_ctrl", { const_cast<uint32_t(*)>(&Misc_regs_base->linkdown_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pcie_reset_trig", { const_cast<uint32_t(*)>(&Misc_regs_base->pcie_reset_trig), 0, {}, sizeof(uint32_t) } },
    { "func_fuse", { const_cast<uint32_t(*)[0x8]>(&Misc_regs_base->func_fuse), 0, {0x8}, sizeof(uint32_t) } },
    { "fuse_status", { const_cast<uint32_t(*)>(&Misc_regs_base->fuse_status), 0, {}, sizeof(uint32_t) } },
    { "dbg_rst0", { const_cast<uint32_t(*)>(&Misc_regs_base->dbg_rst0), 0, {}, sizeof(uint32_t) } },
    { "dbg_rst1", { const_cast<uint32_t(*)>(&Misc_regs_base->dbg_rst1), 0, {}, sizeof(uint32_t) } },
    { "tcu_control0", { const_cast<uint32_t(*)>(&Misc_regs_base->tcu_control0), 0, {}, sizeof(uint32_t) } },
    { "tcu_control1", { const_cast<uint32_t(*)>(&Misc_regs_base->tcu_control1), 0, {}, sizeof(uint32_t) } },
    { "tcu_wrack", { const_cast<uint32_t(*)>(&Misc_regs_base->tcu_wrack), 0, {}, sizeof(uint32_t) } },
    { "tcu_status", { const_cast<uint32_t(*)>(&Misc_regs_base->tcu_status), 0, {}, sizeof(uint32_t) } }
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

struct Mbus_baresync_ts_inc_value_map: public RegisterMapper {
  static constexpr PTR_Mbus_baresync_ts_inc_value Mbus_baresync_ts_inc_value_base=0;
  Mbus_baresync_ts_inc_value_map() : RegisterMapper( {
    { "baresync_ts_inc_value_0_2", { const_cast<uint32_t(*)>(&Mbus_baresync_ts_inc_value_base->baresync_ts_inc_value_0_2), 0, {}, sizeof(uint32_t) } },
    { "baresync_ts_inc_value_1_2", { const_cast<uint32_t(*)>(&Mbus_baresync_ts_inc_value_base->baresync_ts_inc_value_1_2), 0, {}, sizeof(uint32_t) } }
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

struct Mbus_rspec_map: public RegisterMapper {
  static constexpr PTR_Mbus_rspec Mbus_rspec_base=0;
  Mbus_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)[0x4]>(&Mbus_rspec_base->scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Mbus_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "ctrl", { const_cast<uint32_t(*)>(&Mbus_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "flush", { const_cast<uint32_t(*)>(&Mbus_rspec_base->flush), 0, {}, sizeof(uint32_t) } },
    { "baresync_ts_set_value", { const_cast<Mbus_baresync_ts_set_value(*)>(&Mbus_rspec_base->baresync_ts_set_value), new Mbus_baresync_ts_set_value_map, {}, sizeof(Mbus_baresync_ts_set_value) } },
    { "baresync_ts_inc_value", { const_cast<Mbus_baresync_ts_inc_value(*)>(&Mbus_rspec_base->baresync_ts_inc_value), new Mbus_baresync_ts_inc_value_map, {}, sizeof(Mbus_baresync_ts_inc_value) } },
    { "global_ts_set", { const_cast<Mbus_global_ts_set(*)>(&Mbus_rspec_base->global_ts_set), new Mbus_global_ts_set_map, {}, sizeof(Mbus_global_ts_set) } },
    { "global_ts_inc", { const_cast<Mbus_global_ts_inc(*)>(&Mbus_rspec_base->global_ts_inc), new Mbus_global_ts_inc_map, {}, sizeof(Mbus_global_ts_inc) } },
    { "global_ts_inc_value", { const_cast<uint32_t(*)>(&Mbus_rspec_base->global_ts_inc_value), 0, {}, sizeof(uint32_t) } },
    { "global_ts_offset_value", { const_cast<Mbus_global_ts_offset_value(*)>(&Mbus_rspec_base->global_ts_offset_value), new Mbus_global_ts_offset_value_map, {}, sizeof(Mbus_global_ts_offset_value) } },
    { "ts_timer", { const_cast<uint32_t(*)>(&Mbus_rspec_base->ts_timer), 0, {}, sizeof(uint32_t) } },
    { "ts_capture", { const_cast<uint32_t(*)>(&Mbus_rspec_base->ts_capture), 0, {}, sizeof(uint32_t) } },
    { "global_ts_value", { const_cast<Mbus_global_ts_value(*)>(&Mbus_rspec_base->global_ts_value), new Mbus_global_ts_value_map, {}, sizeof(Mbus_global_ts_value) } },
    { "baresync_ts_value", { const_cast<Mbus_baresync_ts_value(*)>(&Mbus_rspec_base->baresync_ts_value), new Mbus_baresync_ts_value_map, {}, sizeof(Mbus_baresync_ts_value) } },
    { "int_stat", { const_cast<uint32_t(*)>(&Mbus_rspec_base->int_stat), 0, {}, sizeof(uint32_t) } },
    { "int_en_0", { const_cast<uint32_t(*)>(&Mbus_rspec_base->int_en_0), 0, {}, sizeof(uint32_t) } },
    { "int_en_1", { const_cast<uint32_t(*)>(&Mbus_rspec_base->int_en_1), 0, {}, sizeof(uint32_t) } },
    { "freeze_en", { const_cast<uint32_t(*)>(&Mbus_rspec_base->freeze_en), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Mbus_rspec_base->int_inj), 0, {}, sizeof(uint32_t) } },
    { "mac_tx_dr_rd_err_log", { const_cast<uint32_t(*)>(&Mbus_rspec_base->mac_tx_dr_rd_err_log), 0, {}, sizeof(uint32_t) } },
    { "controller_mbe_log", { const_cast<uint32_t(*)>(&Mbus_rspec_base->controller_mbe_log), 0, {}, sizeof(uint32_t) } },
    { "controller_sbe_log", { const_cast<uint32_t(*)>(&Mbus_rspec_base->controller_sbe_log), 0, {}, sizeof(uint32_t) } },
    { "parity_err_log", { const_cast<uint32_t(*)[0x4]>(&Mbus_rspec_base->parity_err_log), 0, {0x4}, sizeof(uint32_t) } },
    { "host_creq_credit", { const_cast<uint32_t(*)[0x4]>(&Mbus_rspec_base->host_creq_credit), 0, {0x4}, sizeof(uint32_t) } },
    { "mac_creq_credit", { const_cast<uint32_t(*)[0x4]>(&Mbus_rspec_base->mac_creq_credit), 0, {0x4}, sizeof(uint32_t) } },
    { "host_slave_credit", { const_cast<uint32_t(*)>(&Mbus_rspec_base->host_slave_credit), 0, {}, sizeof(uint32_t) } },
    { "mac_dma_statemachine", { const_cast<uint32_t(*)>(&Mbus_rspec_base->mac_dma_statemachine), 0, {}, sizeof(uint32_t) } }
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
    { "status", { const_cast<uint32_t(*)>(&Dru_rspec_base->status), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mbc_rspec_map: public RegisterMapper {
  static constexpr PTR_Mbc_rspec Mbc_rspec_base=0;
  Mbc_rspec_map() : RegisterMapper( {
    { "mbc_mbus", { &Mbc_rspec_base->mbc_mbus, new Mbus_rspec_map, {}, sizeof(Mbus_rspec) } },
    { "mbc_mac_tx_dr", { &Mbc_rspec_base->mbc_mac_tx_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "mbc_mac_cpl_dr", { &Mbc_rspec_base->mbc_mac_cpl_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } }
    } )
  {}
};

struct Pbus_host_creq_credit_map: public RegisterMapper {
  static constexpr PTR_Pbus_host_creq_credit Pbus_host_creq_credit_base=0;
  Pbus_host_creq_credit_map() : RegisterMapper( {
    { "host_creq_credit_0_12", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_0_12), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_1_12", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_1_12), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_2_12", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_2_12), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_3_12", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_3_12), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_4_12", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_4_12), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_5_12", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_5_12), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_6_12", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_6_12), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_7_12", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_7_12), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_8_12", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_8_12), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_9_12", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_9_12), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_10_12", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_10_12), 0, {}, sizeof(uint32_t) } },
    { "host_creq_credit_11_12", { const_cast<uint32_t(*)>(&Pbus_host_creq_credit_base->host_creq_credit_11_12), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pbus_il_creq_credit_map: public RegisterMapper {
  static constexpr PTR_Pbus_il_creq_credit Pbus_il_creq_credit_base=0;
  Pbus_il_creq_credit_map() : RegisterMapper( {
    { "il_creq_credit_0_12", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_0_12), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_1_12", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_1_12), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_2_12", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_2_12), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_3_12", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_3_12), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_4_12", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_4_12), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_5_12", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_5_12), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_6_12", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_6_12), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_7_12", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_7_12), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_8_12", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_8_12), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_9_12", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_9_12), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_10_12", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_10_12), 0, {}, sizeof(uint32_t) } },
    { "il_creq_credit_11_12", { const_cast<uint32_t(*)>(&Pbus_il_creq_credit_base->il_creq_credit_11_12), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pbus_wb_creq_credit_map: public RegisterMapper {
  static constexpr PTR_Pbus_wb_creq_credit Pbus_wb_creq_credit_base=0;
  Pbus_wb_creq_credit_map() : RegisterMapper( {
    { "wb_creq_credit_0_12", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_0_12), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_1_12", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_1_12), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_2_12", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_2_12), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_3_12", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_3_12), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_4_12", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_4_12), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_5_12", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_5_12), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_6_12", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_6_12), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_7_12", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_7_12), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_8_12", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_8_12), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_9_12", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_9_12), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_10_12", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_10_12), 0, {}, sizeof(uint32_t) } },
    { "wb_creq_credit_11_12", { const_cast<uint32_t(*)>(&Pbus_wb_creq_credit_base->wb_creq_credit_11_12), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pbus_rb_creq_credit_map: public RegisterMapper {
  static constexpr PTR_Pbus_rb_creq_credit Pbus_rb_creq_credit_base=0;
  Pbus_rb_creq_credit_map() : RegisterMapper( {
    { "rb_creq_credit_0_12", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_0_12), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_1_12", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_1_12), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_2_12", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_2_12), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_3_12", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_3_12), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_4_12", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_4_12), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_5_12", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_5_12), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_6_12", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_6_12), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_7_12", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_7_12), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_8_12", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_8_12), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_9_12", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_9_12), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_10_12", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_10_12), 0, {}, sizeof(uint32_t) } },
    { "rb_creq_credit_11_12", { const_cast<uint32_t(*)>(&Pbus_rb_creq_credit_base->rb_creq_credit_11_12), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pbus_rspec_map: public RegisterMapper {
  static constexpr PTR_Pbus_rspec Pbus_rspec_base=0;
  Pbus_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)[0x4]>(&Pbus_rspec_base->scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "ctrl", { const_cast<uint32_t(*)>(&Pbus_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "flush", { const_cast<uint32_t(*)>(&Pbus_rspec_base->flush), 0, {}, sizeof(uint32_t) } },
    { "arb_ctrl0", { const_cast<uint32_t(*)>(&Pbus_rspec_base->arb_ctrl0), 0, {}, sizeof(uint32_t) } },
    { "arb_ctrl1", { const_cast<uint32_t(*)[0x4]>(&Pbus_rspec_base->arb_ctrl1), 0, {0x4}, sizeof(uint32_t) } },
    { "pri_ctrl", { const_cast<uint32_t(*)>(&Pbus_rspec_base->pri_ctrl), 0, {}, sizeof(uint32_t) } },
    { "int_stat0", { const_cast<uint32_t(*)>(&Pbus_rspec_base->int_stat0), 0, {}, sizeof(uint32_t) } },
    { "int_stat1", { const_cast<uint32_t(*)>(&Pbus_rspec_base->int_stat1), 0, {}, sizeof(uint32_t) } },
    { "int_stat2", { const_cast<uint32_t(*)>(&Pbus_rspec_base->int_stat2), 0, {}, sizeof(uint32_t) } },
    { "int_stat3", { const_cast<uint32_t(*)>(&Pbus_rspec_base->int_stat3), 0, {}, sizeof(uint32_t) } },
    { "int_en0_0", { const_cast<uint32_t(*)>(&Pbus_rspec_base->int_en0_0), 0, {}, sizeof(uint32_t) } },
    { "int_en0_1", { const_cast<uint32_t(*)>(&Pbus_rspec_base->int_en0_1), 0, {}, sizeof(uint32_t) } },
    { "int_en1_0", { const_cast<uint32_t(*)>(&Pbus_rspec_base->int_en1_0), 0, {}, sizeof(uint32_t) } },
    { "int_en1_1", { const_cast<uint32_t(*)>(&Pbus_rspec_base->int_en1_1), 0, {}, sizeof(uint32_t) } },
    { "int_en2_0", { const_cast<uint32_t(*)>(&Pbus_rspec_base->int_en2_0), 0, {}, sizeof(uint32_t) } },
    { "int_en2_1", { const_cast<uint32_t(*)>(&Pbus_rspec_base->int_en2_1), 0, {}, sizeof(uint32_t) } },
    { "int_en3_0", { const_cast<uint32_t(*)>(&Pbus_rspec_base->int_en3_0), 0, {}, sizeof(uint32_t) } },
    { "int_en3_1", { const_cast<uint32_t(*)>(&Pbus_rspec_base->int_en3_1), 0, {}, sizeof(uint32_t) } },
    { "freeze_en0", { const_cast<uint32_t(*)>(&Pbus_rspec_base->freeze_en0), 0, {}, sizeof(uint32_t) } },
    { "freeze_en1", { const_cast<uint32_t(*)>(&Pbus_rspec_base->freeze_en1), 0, {}, sizeof(uint32_t) } },
    { "freeze_en2", { const_cast<uint32_t(*)>(&Pbus_rspec_base->freeze_en2), 0, {}, sizeof(uint32_t) } },
    { "freeze_en3", { const_cast<uint32_t(*)>(&Pbus_rspec_base->freeze_en3), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Pbus_rspec_base->int_inj), 0, {}, sizeof(uint32_t) } },
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
    { "il_creq_credit", { const_cast<Pbus_il_creq_credit(*)[0x4]>(&Pbus_rspec_base->il_creq_credit), new Pbus_il_creq_credit_map, {0x4}, sizeof(Pbus_il_creq_credit) } },
    { "wb_creq_credit", { const_cast<Pbus_wb_creq_credit(*)>(&Pbus_rspec_base->wb_creq_credit), new Pbus_wb_creq_credit_map, {}, sizeof(Pbus_wb_creq_credit) } },
    { "rb_creq_credit", { const_cast<Pbus_rb_creq_credit(*)>(&Pbus_rspec_base->rb_creq_credit), new Pbus_rb_creq_credit_map, {}, sizeof(Pbus_rb_creq_credit) } },
    { "sreq_slot_credit", { const_cast<uint32_t(*)>(&Pbus_rspec_base->sreq_slot_credit), 0, {}, sizeof(uint32_t) } },
    { "host_slave_credit", { const_cast<uint32_t(*)>(&Pbus_rspec_base->host_slave_credit), 0, {}, sizeof(uint32_t) } },
    { "il_dma_statemachine", { const_cast<uint32_t(*)>(&Pbus_rspec_base->il_dma_statemachine), 0, {}, sizeof(uint32_t) } },
    { "dma_statemachine", { const_cast<uint32_t(*)>(&Pbus_rspec_base->dma_statemachine), 0, {}, sizeof(uint32_t) } }
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

struct Cbus_rspec_map: public RegisterMapper {
  static constexpr PTR_Cbus_rspec Cbus_rspec_base=0;
  Cbus_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)[0x4]>(&Cbus_rspec_base->scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "ctrl", { const_cast<uint32_t(*)>(&Cbus_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "flush", { const_cast<uint32_t(*)>(&Cbus_rspec_base->flush), 0, {}, sizeof(uint32_t) } },
    { "arb_ctrl", { const_cast<uint32_t(*)>(&Cbus_rspec_base->arb_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pri_ctrl", { const_cast<uint32_t(*)>(&Cbus_rspec_base->pri_ctrl), 0, {}, sizeof(uint32_t) } },
    { "int_stat", { const_cast<uint32_t(*)>(&Cbus_rspec_base->int_stat), 0, {}, sizeof(uint32_t) } },
    { "int_en_0", { const_cast<uint32_t(*)>(&Cbus_rspec_base->int_en_0), 0, {}, sizeof(uint32_t) } },
    { "int_en_1", { const_cast<uint32_t(*)>(&Cbus_rspec_base->int_en_1), 0, {}, sizeof(uint32_t) } },
    { "freeze_en", { const_cast<uint32_t(*)>(&Cbus_rspec_base->freeze_en), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Cbus_rspec_base->int_inj), 0, {}, sizeof(uint32_t) } },
    { "wl_tx_dr_rd_err_log", { const_cast<uint32_t(*)>(&Cbus_rspec_base->wl_tx_dr_rd_err_log), 0, {}, sizeof(uint32_t) } },
    { "lq_fm_dr_rd_err_log", { const_cast<uint32_t(*)>(&Cbus_rspec_base->lq_fm_dr_rd_err_log), 0, {}, sizeof(uint32_t) } },
    { "controller_mbe_log", { const_cast<uint32_t(*)>(&Cbus_rspec_base->controller_mbe_log), 0, {}, sizeof(uint32_t) } },
    { "controller_sbe_log", { const_cast<uint32_t(*)>(&Cbus_rspec_base->controller_sbe_log), 0, {}, sizeof(uint32_t) } },
    { "parity_err_log", { const_cast<uint32_t(*)[0x6]>(&Cbus_rspec_base->parity_err_log), 0, {0x6}, sizeof(uint32_t) } },
    { "host_creq_credit", { const_cast<Cbus_host_creq_credit(*)>(&Cbus_rspec_base->host_creq_credit), new Cbus_host_creq_credit_map, {}, sizeof(Cbus_host_creq_credit) } },
    { "wl_creq_credit", { const_cast<Cbus_wl_creq_credit(*)>(&Cbus_rspec_base->wl_creq_credit), new Cbus_wl_creq_credit_map, {}, sizeof(Cbus_wl_creq_credit) } },
    { "lq_slot_credit", { const_cast<uint32_t(*)>(&Cbus_rspec_base->lq_slot_credit), 0, {}, sizeof(uint32_t) } },
    { "host_slave_credit", { const_cast<uint32_t(*)>(&Cbus_rspec_base->host_slave_credit), 0, {}, sizeof(uint32_t) } },
    { "dma_statemachine", { const_cast<uint32_t(*)>(&Cbus_rspec_base->dma_statemachine), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Cbc_rspec_map: public RegisterMapper {
  static constexpr PTR_Cbc_rspec Cbc_rspec_base=0;
  Cbc_rspec_map() : RegisterMapper( {
    { "cbc_cbus", { &Cbc_rspec_base->cbc_cbus, new Cbus_rspec_map, {}, sizeof(Cbus_rspec) } },
    { "cbc_wl_tx_dr", { &Cbc_rspec_base->cbc_wl_tx_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
    { "cbc_wl_cpl_dr", { &Cbc_rspec_base->cbc_wl_cpl_dr, new Dru_rspec_map, {}, sizeof(Dru_rspec) } },
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
    { "ts", { const_cast<uint32_t(*)>(&Tbus_rspec_base->ts), 0, {}, sizeof(uint32_t) } },
    { "int_stat0", { const_cast<uint32_t(*)>(&Tbus_rspec_base->int_stat0), 0, {}, sizeof(uint32_t) } },
    { "int_stat1", { const_cast<uint32_t(*)>(&Tbus_rspec_base->int_stat1), 0, {}, sizeof(uint32_t) } },
    { "int_stat2", { const_cast<uint32_t(*)>(&Tbus_rspec_base->int_stat2), 0, {}, sizeof(uint32_t) } },
    { "int_en0_0", { const_cast<uint32_t(*)>(&Tbus_rspec_base->int_en0_0), 0, {}, sizeof(uint32_t) } },
    { "int_en0_1", { const_cast<uint32_t(*)>(&Tbus_rspec_base->int_en0_1), 0, {}, sizeof(uint32_t) } },
    { "int_en1_0", { const_cast<uint32_t(*)>(&Tbus_rspec_base->int_en1_0), 0, {}, sizeof(uint32_t) } },
    { "int_en1_1", { const_cast<uint32_t(*)>(&Tbus_rspec_base->int_en1_1), 0, {}, sizeof(uint32_t) } },
    { "int_en2_0", { const_cast<uint32_t(*)>(&Tbus_rspec_base->int_en2_0), 0, {}, sizeof(uint32_t) } },
    { "int_en2_1", { const_cast<uint32_t(*)>(&Tbus_rspec_base->int_en2_1), 0, {}, sizeof(uint32_t) } },
    { "freeze_en0", { const_cast<uint32_t(*)>(&Tbus_rspec_base->freeze_en0), 0, {}, sizeof(uint32_t) } },
    { "freeze_en1", { const_cast<uint32_t(*)>(&Tbus_rspec_base->freeze_en1), 0, {}, sizeof(uint32_t) } },
    { "freeze_en2", { const_cast<uint32_t(*)>(&Tbus_rspec_base->freeze_en2), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Tbus_rspec_base->int_inj), 0, {}, sizeof(uint32_t) } },
    { "tx_dr_rd_err_log", { const_cast<uint32_t(*)[0x4]>(&Tbus_rspec_base->tx_dr_rd_err_log), 0, {0x4}, sizeof(uint32_t) } },
    { "fm_dr_rd_err_log", { const_cast<uint32_t(*)[0x8]>(&Tbus_rspec_base->fm_dr_rd_err_log), 0, {0x8}, sizeof(uint32_t) } },
    { "controller_mbe_log", { const_cast<uint32_t(*)>(&Tbus_rspec_base->controller_mbe_log), 0, {}, sizeof(uint32_t) } },
    { "controller_sbe_log", { const_cast<uint32_t(*)>(&Tbus_rspec_base->controller_sbe_log), 0, {}, sizeof(uint32_t) } },
    { "host_slave_credit", { const_cast<uint32_t(*)>(&Tbus_rspec_base->host_slave_credit), 0, {}, sizeof(uint32_t) } },
    { "dma_statemachine", { const_cast<uint32_t(*)>(&Tbus_rspec_base->dma_statemachine), 0, {}, sizeof(uint32_t) } },
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
    { "hash_array", { const_cast<Lfltr_hash_array(*)[0xe]>(&Lfltr_hash_rspec_base->hash_array), new Lfltr_hash_array_map, {0xe}, sizeof(Lfltr_hash_array) } }
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
    { "int_stat", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->int_stat), 0, {}, sizeof(uint32_t) } },
    { "int_en0", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->int_en0), 0, {}, sizeof(uint32_t) } },
    { "int_en1", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->int_en1), 0, {}, sizeof(uint32_t) } },
    { "freeze_en", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->freeze_en), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Lfltr_ctrl_rspec_base->int_inj), 0, {}, sizeof(uint32_t) } },
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

struct Pipe_reg_ctr_vld_sop_map: public RegisterMapper {
  static constexpr PTR_Pipe_reg_ctr_vld_sop Pipe_reg_ctr_vld_sop_base=0;
  Pipe_reg_ctr_vld_sop_map() : RegisterMapper( {
    { "ctr_vld_sop_0_2", { const_cast<uint32_t(*)>(&Pipe_reg_ctr_vld_sop_base->ctr_vld_sop_0_2), 0, {}, sizeof(uint32_t) } },
    { "ctr_vld_sop_1_2", { const_cast<uint32_t(*)>(&Pipe_reg_ctr_vld_sop_base->ctr_vld_sop_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pipe_reg_wac_drop_buf_full_map: public RegisterMapper {
  static constexpr PTR_Pipe_reg_wac_drop_buf_full Pipe_reg_wac_drop_buf_full_base=0;
  Pipe_reg_wac_drop_buf_full_map() : RegisterMapper( {
    { "wac_drop_buf_full_0_2", { const_cast<uint32_t(*)>(&Pipe_reg_wac_drop_buf_full_base->wac_drop_buf_full_0_2), 0, {}, sizeof(uint32_t) } },
    { "wac_drop_buf_full_1_2", { const_cast<uint32_t(*)>(&Pipe_reg_wac_drop_buf_full_base->wac_drop_buf_full_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pipe_reg_intr_map: public RegisterMapper {
  static constexpr PTR_Pipe_reg_intr Pipe_reg_intr_base=0;
  Pipe_reg_intr_map() : RegisterMapper( {
    { "status", { const_cast<uint32_t(*)>(&Pipe_reg_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Pipe_reg_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Pipe_reg_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Pipe_reg_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Pipe_reg_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_drop_count_port_map: public RegisterMapper {
  static constexpr PTR_Wac_drop_count_port Wac_drop_count_port_base=0;
  Wac_drop_count_port_map() : RegisterMapper( {
    { "wac_drop_count_port_0_2", { const_cast<uint32_t(*)>(&Wac_drop_count_port_base->wac_drop_count_port_0_2), 0, {}, sizeof(uint32_t) } },
    { "wac_drop_count_port_1_2", { const_cast<uint32_t(*)>(&Wac_drop_count_port_base->wac_drop_count_port_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_drop_count_ppg_map: public RegisterMapper {
  static constexpr PTR_Wac_drop_count_ppg Wac_drop_count_ppg_base=0;
  Wac_drop_count_ppg_map() : RegisterMapper( {
    { "wac_drop_count_ppg_0_2", { const_cast<uint32_t(*)>(&Wac_drop_count_ppg_base->wac_drop_count_ppg_0_2), 0, {}, sizeof(uint32_t) } },
    { "wac_drop_count_ppg_1_2", { const_cast<uint32_t(*)>(&Wac_drop_count_ppg_base->wac_drop_count_ppg_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_ppg_drop_state_map: public RegisterMapper {
  static constexpr PTR_Wac_ppg_drop_state Wac_ppg_drop_state_base=0;
  Wac_ppg_drop_state_map() : RegisterMapper( {
    { "wac_ppg_drop_state0_0_4", { const_cast<uint32_t(*)>(&Wac_ppg_drop_state_base->wac_ppg_drop_state0_0_4), 0, {}, sizeof(uint32_t) } },
    { "wac_ppg_drop_state0_1_4", { const_cast<uint32_t(*)>(&Wac_ppg_drop_state_base->wac_ppg_drop_state0_1_4), 0, {}, sizeof(uint32_t) } },
    { "wac_ppg_drop_state0_2_4", { const_cast<uint32_t(*)>(&Wac_ppg_drop_state_base->wac_ppg_drop_state0_2_4), 0, {}, sizeof(uint32_t) } },
    { "wac_ppg_drop_state0_3_4", { const_cast<uint32_t(*)>(&Wac_ppg_drop_state_base->wac_ppg_drop_state0_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_port_drop_state_map: public RegisterMapper {
  static constexpr PTR_Wac_port_drop_state Wac_port_drop_state_base=0;
  Wac_port_drop_state_map() : RegisterMapper( {
    { "wac_port_drop_state_0_3", { const_cast<uint32_t(*)>(&Wac_port_drop_state_base->wac_port_drop_state_0_3), 0, {}, sizeof(uint32_t) } },
    { "wac_port_drop_state_1_3", { const_cast<uint32_t(*)>(&Wac_port_drop_state_base->wac_port_drop_state_1_3), 0, {}, sizeof(uint32_t) } },
    { "wac_port_drop_state_2_3", { const_cast<uint32_t(*)>(&Wac_port_drop_state_base->wac_port_drop_state_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pipe_reg_map: public RegisterMapper {
  static constexpr PTR_Pipe_reg Pipe_reg_base=0;
  Pipe_reg_map() : RegisterMapper( {
    { "wac_bypass_config", { const_cast<uint32_t(*)>(&Pipe_reg_base->wac_bypass_config), 0, {}, sizeof(uint32_t) } },
    { "ctr_drop_no_dst", { const_cast<uint32_t(*)>(&Pipe_reg_base->ctr_drop_no_dst), 0, {}, sizeof(uint32_t) } },
    { "ctr_drop_sop_by_sop", { const_cast<uint32_t(*)>(&Pipe_reg_base->ctr_drop_sop_by_sop), 0, {}, sizeof(uint32_t) } },
    { "ctr_vld_sop", { const_cast<Pipe_reg_ctr_vld_sop(*)>(&Pipe_reg_base->ctr_vld_sop), new Pipe_reg_ctr_vld_sop_map, {}, sizeof(Pipe_reg_ctr_vld_sop) } },
    { "wac_drop_buf_full", { const_cast<Pipe_reg_wac_drop_buf_full(*)>(&Pipe_reg_base->wac_drop_buf_full), new Pipe_reg_wac_drop_buf_full_map, {}, sizeof(Pipe_reg_wac_drop_buf_full) } },
    { "wac_debug_register", { const_cast<uint32_t(*)>(&Pipe_reg_base->wac_debug_register), 0, {}, sizeof(uint32_t) } },
    { "offset_profile", { const_cast<uint32_t(*)[0x20]>(&Pipe_reg_base->offset_profile), 0, {0x20}, sizeof(uint32_t) } },
    { "port_pfc_en", { const_cast<uint32_t(*)[0x48]>(&Pipe_reg_base->port_pfc_en), 0, {0x48}, sizeof(uint32_t) } },
    { "port_pause_en", { const_cast<uint32_t(*)[0x48]>(&Pipe_reg_base->port_pause_en), 0, {0x48}, sizeof(uint32_t) } },
    { "wac_ecc", { const_cast<uint32_t(*)>(&Pipe_reg_base->wac_ecc), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Pipe_reg_base->intr, new Pipe_reg_intr_map, {}, sizeof(Pipe_reg_intr) } },
    { "ppg_mapping_table_sbe_errlog", { const_cast<uint32_t(*)>(&Pipe_reg_base->ppg_mapping_table_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "ppg_mapping_table_mbe_errlog", { const_cast<uint32_t(*)>(&Pipe_reg_base->ppg_mapping_table_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "ppg_drop_cnt_table_sbe_errlog", { const_cast<uint32_t(*)>(&Pipe_reg_base->ppg_drop_cnt_table_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "ppg_drop_cnt_table_mbe_errlog", { const_cast<uint32_t(*)>(&Pipe_reg_base->ppg_drop_cnt_table_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "wac_drop_count_port", { const_cast<Wac_drop_count_port(*)[0x49]>(&Pipe_reg_base->wac_drop_count_port), new Wac_drop_count_port_map, {0x49}, sizeof(Wac_drop_count_port) } },
    { "wac_drop_count_ppg", { const_cast<Wac_drop_count_ppg(*)[0xc9]>(&Pipe_reg_base->wac_drop_count_ppg), new Wac_drop_count_ppg_map, {0xc9}, sizeof(Wac_drop_count_ppg) } },
    { "wac_pfc_state", { const_cast<uint32_t(*)[0x49]>(&Pipe_reg_base->wac_pfc_state), 0, {0x49}, sizeof(uint32_t) } },
    { "wac_ppg_drop_state0", { const_cast<Wac_ppg_drop_state(*)>(&Pipe_reg_base->wac_ppg_drop_state0), new Wac_ppg_drop_state_map, {}, sizeof(Wac_ppg_drop_state) } },
    { "wac_ppg_drop_state1", { const_cast<Wac_ppg_drop_state(*)>(&Pipe_reg_base->wac_ppg_drop_state1), new Wac_ppg_drop_state_map, {}, sizeof(Wac_ppg_drop_state) } },
    { "wac_port_drop_state", { const_cast<Wac_port_drop_state(*)>(&Pipe_reg_base->wac_port_drop_state), new Wac_port_drop_state_map, {}, sizeof(Wac_port_drop_state) } },
    { "wac_pre_fifo_mapping", { const_cast<uint32_t(*)[0x8]>(&Pipe_reg_base->wac_pre_fifo_mapping), 0, {0x8}, sizeof(uint32_t) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Pipe_reg_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_ppg_min_lmt_map: public RegisterMapper {
  static constexpr PTR_Wac_ppg_min_lmt Wac_ppg_min_lmt_base=0;
  Wac_ppg_min_lmt_map() : RegisterMapper( {
    { "min_lmt", { const_cast<uint32_t(*)[0xc9]>(&Wac_ppg_min_lmt_base->min_lmt), 0, {0xc9}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_ppg_hdr_lmt_map: public RegisterMapper {
  static constexpr PTR_Wac_ppg_hdr_lmt Wac_ppg_hdr_lmt_base=0;
  Wac_ppg_hdr_lmt_map() : RegisterMapper( {
    { "hdr_lmt", { const_cast<uint32_t(*)[0x80]>(&Wac_ppg_hdr_lmt_base->hdr_lmt), 0, {0x80}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_port_max_lmt_map: public RegisterMapper {
  static constexpr PTR_Wac_port_max_lmt Wac_port_max_lmt_base=0;
  Wac_port_max_lmt_map() : RegisterMapper( {
    { "max_lmt", { const_cast<uint32_t(*)[0x49]>(&Wac_port_max_lmt_base->max_lmt), 0, {0x49}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_ppg_shr_cnt_map: public RegisterMapper {
  static constexpr PTR_Wac_ppg_shr_cnt Wac_ppg_shr_cnt_base=0;
  Wac_ppg_shr_cnt_map() : RegisterMapper( {
    { "shr_cnt", { const_cast<uint32_t(*)[0xc9]>(&Wac_ppg_shr_cnt_base->shr_cnt), 0, {0xc9}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_ppg_min_cnt_map: public RegisterMapper {
  static constexpr PTR_Wac_ppg_min_cnt Wac_ppg_min_cnt_base=0;
  Wac_ppg_min_cnt_map() : RegisterMapper( {
    { "min_cnt", { const_cast<uint32_t(*)[0xc9]>(&Wac_ppg_min_cnt_base->min_cnt), 0, {0xc9}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_ppg_hdr_cnt_map: public RegisterMapper {
  static constexpr PTR_Wac_ppg_hdr_cnt Wac_ppg_hdr_cnt_base=0;
  Wac_ppg_hdr_cnt_map() : RegisterMapper( {
    { "hdr_cnt", { const_cast<uint32_t(*)[0x80]>(&Wac_ppg_hdr_cnt_base->hdr_cnt), 0, {0x80}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_ppg_resume_map: public RegisterMapper {
  static constexpr PTR_Wac_ppg_resume Wac_ppg_resume_base=0;
  Wac_ppg_resume_map() : RegisterMapper( {
    { "ppg_resume", { const_cast<uint32_t(*)[0xc9]>(&Wac_ppg_resume_base->ppg_resume), 0, {0xc9}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_port_ppg_mapping_map: public RegisterMapper {
  static constexpr PTR_Wac_port_ppg_mapping Wac_port_ppg_mapping_base=0;
  Wac_port_ppg_mapping_map() : RegisterMapper( {
    { "entry", { const_cast<uint32_t(*)[0x124]>(&Wac_port_ppg_mapping_base->entry), 0, {0x124}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_ppg_shr_lmt_map: public RegisterMapper {
  static constexpr PTR_Wac_ppg_shr_lmt Wac_ppg_shr_lmt_base=0;
  Wac_ppg_shr_lmt_map() : RegisterMapper( {
    { "shr_lmt", { const_cast<uint32_t(*)[0xc9]>(&Wac_ppg_shr_lmt_base->shr_lmt), 0, {0xc9}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_ppg_wm_cnt_map: public RegisterMapper {
  static constexpr PTR_Wac_ppg_wm_cnt Wac_ppg_wm_cnt_base=0;
  Wac_ppg_wm_cnt_map() : RegisterMapper( {
    { "wm_cnt", { const_cast<uint32_t(*)[0xc9]>(&Wac_ppg_wm_cnt_base->wm_cnt), 0, {0xc9}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_ppg_icos_mapping_map: public RegisterMapper {
  static constexpr PTR_Wac_ppg_icos_mapping Wac_ppg_icos_mapping_base=0;
  Wac_ppg_icos_mapping_map() : RegisterMapper( {
    { "entry", { const_cast<uint32_t(*)[0x80]>(&Wac_ppg_icos_mapping_base->entry), 0, {0x80}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_port_cnt_map: public RegisterMapper {
  static constexpr PTR_Wac_port_cnt Wac_port_cnt_base=0;
  Wac_port_cnt_map() : RegisterMapper( {
    { "cnt", { const_cast<uint32_t(*)[0x49]>(&Wac_port_cnt_base->cnt), 0, {0x49}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_port_wm_cnt_map: public RegisterMapper {
  static constexpr PTR_Wac_port_wm_cnt Wac_port_wm_cnt_base=0;
  Wac_port_wm_cnt_map() : RegisterMapper( {
    { "wm_cnt", { const_cast<uint32_t(*)[0x49]>(&Wac_port_wm_cnt_base->wm_cnt), 0, {0x49}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_wac_pipe_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_wac_pipe_rspec Tm_wac_pipe_rspec_base=0;
  Tm_wac_pipe_rspec_map() : RegisterMapper( {
    { "wac_reg", { &Tm_wac_pipe_rspec_base->wac_reg, new Pipe_reg_map, {}, sizeof(Pipe_reg) } },
    { "csr_mem_wac_ppg_min_lmt", { &Tm_wac_pipe_rspec_base->csr_mem_wac_ppg_min_lmt, new Wac_ppg_min_lmt_map, {}, sizeof(Wac_ppg_min_lmt) } },
    { "csr_mem_wac_ppg_hdr_lmt", { &Tm_wac_pipe_rspec_base->csr_mem_wac_ppg_hdr_lmt, new Wac_ppg_hdr_lmt_map, {}, sizeof(Wac_ppg_hdr_lmt) } },
    { "csr_mem_wac_port_max_lmt", { &Tm_wac_pipe_rspec_base->csr_mem_wac_port_max_lmt, new Wac_port_max_lmt_map, {}, sizeof(Wac_port_max_lmt) } },
    { "csr_mem_wac_ppg_shr_cnt", { &Tm_wac_pipe_rspec_base->csr_mem_wac_ppg_shr_cnt, new Wac_ppg_shr_cnt_map, {}, sizeof(Wac_ppg_shr_cnt) } },
    { "csr_mem_wac_ppg_min_cnt", { &Tm_wac_pipe_rspec_base->csr_mem_wac_ppg_min_cnt, new Wac_ppg_min_cnt_map, {}, sizeof(Wac_ppg_min_cnt) } },
    { "csr_mem_wac_ppg_hdr_cnt", { &Tm_wac_pipe_rspec_base->csr_mem_wac_ppg_hdr_cnt, new Wac_ppg_hdr_cnt_map, {}, sizeof(Wac_ppg_hdr_cnt) } },
    { "csr_mem_wac_ppg_resume", { &Tm_wac_pipe_rspec_base->csr_mem_wac_ppg_resume, new Wac_ppg_resume_map, {}, sizeof(Wac_ppg_resume) } },
    { "csr_mem_wac_port_ppg_mapping", { &Tm_wac_pipe_rspec_base->csr_mem_wac_port_ppg_mapping, new Wac_port_ppg_mapping_map, {}, sizeof(Wac_port_ppg_mapping) } },
    { "csr_mem_wac_ppg_shr_lmt", { &Tm_wac_pipe_rspec_base->csr_mem_wac_ppg_shr_lmt, new Wac_ppg_shr_lmt_map, {}, sizeof(Wac_ppg_shr_lmt) } },
    { "csr_mem_wac_ppg_wm_cnt", { &Tm_wac_pipe_rspec_base->csr_mem_wac_ppg_wm_cnt, new Wac_ppg_wm_cnt_map, {}, sizeof(Wac_ppg_wm_cnt) } },
    { "csr_mem_wac_ppg_icos_mapping", { &Tm_wac_pipe_rspec_base->csr_mem_wac_ppg_icos_mapping, new Wac_ppg_icos_mapping_map, {}, sizeof(Wac_ppg_icos_mapping) } },
    { "csr_mem_wac_port_cnt", { &Tm_wac_pipe_rspec_base->csr_mem_wac_port_cnt, new Wac_port_cnt_map, {}, sizeof(Wac_port_cnt) } },
    { "csr_mem_wac_port_wm_cnt", { &Tm_wac_pipe_rspec_base->csr_mem_wac_port_wm_cnt, new Wac_port_wm_cnt_map, {}, sizeof(Wac_port_wm_cnt) } }
    } )
  {}
};

struct Wac_drop_cnt_pre_fifo_map: public RegisterMapper {
  static constexpr PTR_Wac_drop_cnt_pre_fifo Wac_drop_cnt_pre_fifo_base=0;
  Wac_drop_cnt_pre_fifo_map() : RegisterMapper( {
    { "wac_drop_cnt_pre0_fifo_0_2", { const_cast<uint32_t(*)>(&Wac_drop_cnt_pre_fifo_base->wac_drop_cnt_pre0_fifo_0_2), 0, {}, sizeof(uint32_t) } },
    { "wac_drop_cnt_pre0_fifo_1_2", { const_cast<uint32_t(*)>(&Wac_drop_cnt_pre_fifo_base->wac_drop_cnt_pre0_fifo_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_common_block_map: public RegisterMapper {
  static constexpr PTR_Wac_common_block Wac_common_block_base=0;
  Wac_common_block_map() : RegisterMapper( {
    { "wac_scratch", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Wac_common_block_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "wac_intr_status", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_intr_status), 0, {}, sizeof(uint32_t) } },
    { "wac_glb_config", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_glb_config), 0, {}, sizeof(uint32_t) } },
    { "wac_mem_init_done", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_mem_init_done), 0, {}, sizeof(uint32_t) } },
    { "wac_ap_limit_cell", { const_cast<uint32_t(*)[0x4]>(&Wac_common_block_base->wac_ap_limit_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "wac_ap_offset_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_ap_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_ap_red_limit_cell", { const_cast<uint32_t(*)[0x4]>(&Wac_common_block_base->wac_ap_red_limit_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "wac_ap_red_offset_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_ap_red_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_ap_yel_limit_cell", { const_cast<uint32_t(*)[0x4]>(&Wac_common_block_base->wac_ap_yel_limit_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "wac_ap_yel_offset_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_ap_yel_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_dod_limit_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_dod_limit_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_ap_cnt_cell", { const_cast<uint32_t(*)[0x4]>(&Wac_common_block_base->wac_ap_cnt_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "wac_wm_ap_cnt_cell", { const_cast<uint32_t(*)[0x4]>(&Wac_common_block_base->wac_wm_ap_cnt_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "wac_hdr_cnt_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_hdr_cnt_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_wm_hdr_cnt_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_wm_hdr_cnt_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_hdr_limit_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_hdr_limit_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_hdr_offset_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_hdr_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_dod_cnt_cell", { const_cast<uint32_t(*)>(&Wac_common_block_base->wac_dod_cnt_cell), 0, {}, sizeof(uint32_t) } },
    { "wac_pfc_pool_0_limit_cell", { const_cast<uint32_t(*)[0x8]>(&Wac_common_block_base->wac_pfc_pool_0_limit_cell), 0, {0x8}, sizeof(uint32_t) } },
    { "wac_pfc_pool_1_limit_cell", { const_cast<uint32_t(*)[0x8]>(&Wac_common_block_base->wac_pfc_pool_1_limit_cell), 0, {0x8}, sizeof(uint32_t) } },
    { "wac_pfc_pool_2_limit_cell", { const_cast<uint32_t(*)[0x8]>(&Wac_common_block_base->wac_pfc_pool_2_limit_cell), 0, {0x8}, sizeof(uint32_t) } },
    { "wac_pfc_pool_3_limit_cell", { const_cast<uint32_t(*)[0x8]>(&Wac_common_block_base->wac_pfc_pool_3_limit_cell), 0, {0x8}, sizeof(uint32_t) } },
    { "wac_drop_cnt_pre0_fifo", { const_cast<Wac_drop_cnt_pre_fifo(*)[0x4]>(&Wac_common_block_base->wac_drop_cnt_pre0_fifo), new Wac_drop_cnt_pre_fifo_map, {0x4}, sizeof(Wac_drop_cnt_pre_fifo) } },
    { "wac_drop_cnt_pre1_fifo", { const_cast<Wac_drop_cnt_pre_fifo(*)[0x4]>(&Wac_common_block_base->wac_drop_cnt_pre1_fifo), new Wac_drop_cnt_pre_fifo_map, {0x4}, sizeof(Wac_drop_cnt_pre_fifo) } },
    { "wac_drop_cnt_pre2_fifo", { const_cast<Wac_drop_cnt_pre_fifo(*)[0x4]>(&Wac_common_block_base->wac_drop_cnt_pre2_fifo), new Wac_drop_cnt_pre_fifo_map, {0x4}, sizeof(Wac_drop_cnt_pre_fifo) } },
    { "wac_drop_cnt_pre3_fifo", { const_cast<Wac_drop_cnt_pre_fifo(*)[0x4]>(&Wac_common_block_base->wac_drop_cnt_pre3_fifo), new Wac_drop_cnt_pre_fifo_map, {0x4}, sizeof(Wac_drop_cnt_pre_fifo) } },
    { "wac_egress_qid_mapping", { const_cast<uint32_t(*)[0x200]>(&Wac_common_block_base->wac_egress_qid_mapping), 0, {0x200}, sizeof(uint32_t) } }
    } )
  {}
};

struct Wac_common_block_drop_st_map: public RegisterMapper {
  static constexpr PTR_Wac_common_block_drop_st Wac_common_block_drop_st_base=0;
  Wac_common_block_drop_st_map() : RegisterMapper( {
    { "drop_state_cell", { const_cast<uint32_t(*)[0x4]>(&Wac_common_block_drop_st_base->drop_state_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "wac_queue_state_shadow", { const_cast<uint32_t(*)[0x240]>(&Wac_common_block_drop_st_base->wac_queue_state_shadow), 0, {0x240}, sizeof(uint32_t) } }
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

struct Caa_bank_ctrl_r_map: public RegisterMapper {
  static constexpr PTR_Caa_bank_ctrl_r Caa_bank_ctrl_r_base=0;
  Caa_bank_ctrl_r_map() : RegisterMapper( {
    { "bank_indir_access_data_0_2", { const_cast<uint32_t(*)>(&Caa_bank_ctrl_r_base->bank_indir_access_data_0_2), 0, {}, sizeof(uint32_t) } },
    { "bank_indir_access_data_1_2", { const_cast<uint32_t(*)>(&Caa_bank_ctrl_r_base->bank_indir_access_data_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_caa_rspec_ecc_map: public RegisterMapper {
  static constexpr PTR_Tm_caa_rspec_ecc Tm_caa_rspec_ecc_base=0;
  Tm_caa_rspec_ecc_map() : RegisterMapper( {
    { "ecc_0_4", { const_cast<uint32_t(*)>(&Tm_caa_rspec_ecc_base->ecc_0_4), 0, {}, sizeof(uint32_t) } },
    { "ecc_1_4", { const_cast<uint32_t(*)>(&Tm_caa_rspec_ecc_base->ecc_1_4), 0, {}, sizeof(uint32_t) } },
    { "ecc_2_4", { const_cast<uint32_t(*)>(&Tm_caa_rspec_ecc_base->ecc_2_4), 0, {}, sizeof(uint32_t) } },
    { "ecc_3_4", { const_cast<uint32_t(*)>(&Tm_caa_rspec_ecc_base->ecc_3_4), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_caa_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_caa_rspec_intr Tm_caa_rspec_intr_base=0;
  Tm_caa_rspec_intr_map() : RegisterMapper( {
    { "status", { const_cast<uint32_t(*)>(&Tm_caa_rspec_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Tm_caa_rspec_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Tm_caa_rspec_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Tm_caa_rspec_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_caa_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_caa_cdm_rspec_ecc_map: public RegisterMapper {
  static constexpr PTR_Tm_caa_cdm_rspec_ecc Tm_caa_cdm_rspec_ecc_base=0;
  Tm_caa_cdm_rspec_ecc_map() : RegisterMapper( {
    { "ecc_0_2", { const_cast<uint32_t(*)>(&Tm_caa_cdm_rspec_ecc_base->ecc_0_2), 0, {}, sizeof(uint32_t) } },
    { "ecc_1_2", { const_cast<uint32_t(*)>(&Tm_caa_cdm_rspec_ecc_base->ecc_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_caa_cdm_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_caa_cdm_rspec_intr Tm_caa_cdm_rspec_intr_base=0;
  Tm_caa_cdm_rspec_intr_map() : RegisterMapper( {
    { "status", { const_cast<uint32_t(*)>(&Tm_caa_cdm_rspec_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Tm_caa_cdm_rspec_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Tm_caa_cdm_rspec_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Tm_caa_cdm_rspec_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_caa_cdm_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Caa_cdm_indir_access_data_r_map: public RegisterMapper {
  static constexpr PTR_Caa_cdm_indir_access_data_r Caa_cdm_indir_access_data_r_base=0;
  Caa_cdm_indir_access_data_r_map() : RegisterMapper( {
    { "cdm_indir_access_data_0_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_0_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_1_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_1_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_2_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_2_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_3_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_3_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_4_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_4_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_5_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_5_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_6_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_6_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_7_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_7_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_8_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_8_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_9_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_9_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_10_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_10_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_11_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_11_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_12_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_12_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_13_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_13_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_14_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_14_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_15_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_15_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_16_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_16_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_17_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_17_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_18_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_18_20), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data_19_20", { const_cast<uint32_t(*)>(&Caa_cdm_indir_access_data_r_base->cdm_indir_access_data_19_20), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_caa_cdm_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_caa_cdm_rspec Tm_caa_cdm_rspec_base=0;
  Tm_caa_cdm_rspec_map() : RegisterMapper( {
    { "dft_csr", { const_cast<uint32_t(*)>(&Tm_caa_cdm_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<Tm_caa_cdm_rspec_ecc(*)>(&Tm_caa_cdm_rspec_base->ecc), new Tm_caa_cdm_rspec_ecc_map, {}, sizeof(Tm_caa_cdm_rspec_ecc) } },
    { "intr", { &Tm_caa_cdm_rspec_base->intr, new Tm_caa_cdm_rspec_intr_map, {}, sizeof(Tm_caa_cdm_rspec_intr) } },
    { "cdm_sbe_errlog", { const_cast<uint32_t(*)>(&Tm_caa_cdm_rspec_base->cdm_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "cdm_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_caa_cdm_rspec_base->cdm_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_addr", { const_cast<uint32_t(*)>(&Tm_caa_cdm_rspec_base->cdm_indir_access_addr), 0, {}, sizeof(uint32_t) } },
    { "cdm_indir_access_data", { const_cast<Caa_cdm_indir_access_data_r(*)>(&Tm_caa_cdm_rspec_base->cdm_indir_access_data), new Caa_cdm_indir_access_data_r_map, {}, sizeof(Caa_cdm_indir_access_data_r) } }
    } )
  {}
};

struct Tm_caa_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_caa_rspec Tm_caa_rspec_base=0;
  Tm_caa_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "cdm_org", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->cdm_org), 0, {}, sizeof(uint32_t) } },
    { "ctrl", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "full_threshold", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->full_threshold), 0, {}, sizeof(uint32_t) } },
    { "hyst_threshold", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->hyst_threshold), 0, {}, sizeof(uint32_t) } },
    { "block_valid", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->block_valid), 0, {}, sizeof(uint32_t) } },
    { "block_reset", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->block_reset), 0, {}, sizeof(uint32_t) } },
    { "block_enable", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->block_enable), 0, {}, sizeof(uint32_t) } },
    { "block_ready", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->block_ready), 0, {}, sizeof(uint32_t) } },
    { "blocks_freecnt", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->blocks_freecnt), 0, {}, sizeof(uint32_t) } },
    { "epipe", { &Tm_caa_rspec_base->epipe, new Caa_epipe_g_map, {0x4}, sizeof(Caa_epipe_g) } },
    { "block", { &Tm_caa_rspec_base->block, new Caa_block_g_map, {0x1c}, sizeof(Caa_block_g) } },
    { "bank_indir_access_addr", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->bank_indir_access_addr), 0, {}, sizeof(uint32_t) } },
    { "bank_indir_access_data", { const_cast<Caa_bank_ctrl_r(*)>(&Tm_caa_rspec_base->bank_indir_access_data), new Caa_bank_ctrl_r_map, {}, sizeof(Caa_bank_ctrl_r) } },
    { "lmem_indir_access_addr", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->lmem_indir_access_addr), 0, {}, sizeof(uint32_t) } },
    { "lmem_indir_access_data", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->lmem_indir_access_data), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<Tm_caa_rspec_ecc(*)>(&Tm_caa_rspec_base->ecc), new Tm_caa_rspec_ecc_map, {}, sizeof(Tm_caa_rspec_ecc) } },
    { "intr", { &Tm_caa_rspec_base->intr, new Tm_caa_rspec_intr_map, {}, sizeof(Tm_caa_rspec_intr) } },
    { "linkmem_sbe_errlog", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->linkmem_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "linkmem_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->linkmem_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "overflow_errlog", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->overflow_errlog), 0, {}, sizeof(uint32_t) } },
    { "underflow_errlog", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->underflow_errlog), 0, {}, sizeof(uint32_t) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Tm_caa_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } },
    { "cdm", { &Tm_caa_rspec_base->cdm, new Tm_caa_cdm_rspec_map, {}, sizeof(Tm_caa_cdm_rspec) } }
    } )
  {}
};

struct Qac_queue_min_thrd_config_map: public RegisterMapper {
  static constexpr PTR_Qac_queue_min_thrd_config Qac_queue_min_thrd_config_base=0;
  Qac_queue_min_thrd_config_map() : RegisterMapper( {
    { "entry", { const_cast<uint32_t(*)[0x240]>(&Qac_queue_min_thrd_config_base->entry), 0, {0x240}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qac_queue_shr_thrd_config_map: public RegisterMapper {
  static constexpr PTR_Qac_queue_shr_thrd_config Qac_queue_shr_thrd_config_base=0;
  Qac_queue_shr_thrd_config_map() : RegisterMapper( {
    { "entry", { const_cast<uint32_t(*)[0x240]>(&Qac_queue_shr_thrd_config_base->entry), 0, {0x240}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qac_queue_ap_config_map: public RegisterMapper {
  static constexpr PTR_Qac_queue_ap_config Qac_queue_ap_config_base=0;
  Qac_queue_ap_config_map() : RegisterMapper( {
    { "entry", { const_cast<uint32_t(*)[0x240]>(&Qac_queue_ap_config_base->entry), 0, {0x240}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qac_queue_color_limit_map: public RegisterMapper {
  static constexpr PTR_Qac_queue_color_limit Qac_queue_color_limit_base=0;
  Qac_queue_color_limit_map() : RegisterMapper( {
    { "entry", { const_cast<uint32_t(*)[0x240]>(&Qac_queue_color_limit_base->entry), 0, {0x240}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qac_queue_cell_count_map: public RegisterMapper {
  static constexpr PTR_Qac_queue_cell_count Qac_queue_cell_count_base=0;
  Qac_queue_cell_count_map() : RegisterMapper( {
    { "entry", { const_cast<uint32_t(*)[0x240]>(&Qac_queue_cell_count_base->entry), 0, {0x240}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qac_queue_wm_cell_count_map: public RegisterMapper {
  static constexpr PTR_Qac_queue_wm_cell_count Qac_queue_wm_cell_count_base=0;
  Qac_queue_wm_cell_count_map() : RegisterMapper( {
    { "count", { const_cast<uint32_t(*)[0x240]>(&Qac_queue_wm_cell_count_base->count), 0, {0x240}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qac_port_wm_cell_count_map: public RegisterMapper {
  static constexpr PTR_Qac_port_wm_cell_count Qac_port_wm_cell_count_base=0;
  Qac_port_wm_cell_count_map() : RegisterMapper( {
    { "cell_count", { const_cast<uint32_t(*)[0x48]>(&Qac_port_wm_cell_count_base->cell_count), 0, {0x48}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qac_port_config_map: public RegisterMapper {
  static constexpr PTR_Qac_port_config Qac_port_config_base=0;
  Qac_port_config_map() : RegisterMapper( {
    { "qac_port_config", { const_cast<uint32_t(*)[0x48]>(&Qac_port_config_base->qac_port_config), 0, {0x48}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qac_port_cell_count_map: public RegisterMapper {
  static constexpr PTR_Qac_port_cell_count Qac_port_cell_count_base=0;
  Qac_port_cell_count_map() : RegisterMapper( {
    { "qac_port_cell_count", { const_cast<uint32_t(*)[0x48]>(&Qac_port_cell_count_base->qac_port_cell_count), 0, {0x48}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qac_drop_count_data_map: public RegisterMapper {
  static constexpr PTR_Qac_drop_count_data Qac_drop_count_data_base=0;
  Qac_drop_count_data_map() : RegisterMapper( {
    { "qac_queue_drop_count_data_0_2", { const_cast<uint32_t(*)>(&Qac_drop_count_data_base->qac_queue_drop_count_data_0_2), 0, {}, sizeof(uint32_t) } },
    { "qac_queue_drop_count_data_1_2", { const_cast<uint32_t(*)>(&Qac_drop_count_data_base->qac_queue_drop_count_data_1_2), 0, {}, sizeof(uint32_t) } }
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
    { "status", { const_cast<uint32_t(*)>(&Pipe_block_reg_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Pipe_block_reg_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Pipe_block_reg_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Pipe_block_reg_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Pipe_block_reg_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pipe_block_reg_map: public RegisterMapper {
  static constexpr PTR_Pipe_block_reg Pipe_block_reg_base=0;
  Pipe_block_reg_map() : RegisterMapper( {
    { "qac_ctr32_drop_no_dst", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac_ctr32_drop_no_dst), 0, {}, sizeof(uint32_t) } },
    { "qac_ctr32_pre_mc_drop", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac_ctr32_pre_mc_drop), 0, {}, sizeof(uint32_t) } },
    { "offset_profile", { const_cast<uint32_t(*)[0x20]>(&Pipe_block_reg_base->offset_profile), 0, {0x20}, sizeof(uint32_t) } },
    { "mc_apid", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->mc_apid), 0, {}, sizeof(uint32_t) } },
    { "qac_qid_profile_config", { const_cast<uint32_t(*)[0x48]>(&Pipe_block_reg_base->qac_qid_profile_config), 0, {0x48}, sizeof(uint32_t) } },
    { "qac_qid_mapping", { const_cast<uint32_t(*)[0x200]>(&Pipe_block_reg_base->qac_qid_mapping), 0, {0x200}, sizeof(uint32_t) } },
    { "pipe_config", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->pipe_config), 0, {}, sizeof(uint32_t) } },
    { "discard_queue_cnt_cell", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->discard_queue_cnt_cell), 0, {}, sizeof(uint32_t) } },
    { "discard_queue_wm_cnt_cell", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->discard_queue_wm_cnt_cell), 0, {}, sizeof(uint32_t) } },
    { "qac_queue_drop_count_data", { const_cast<Qac_drop_count_data(*)>(&Pipe_block_reg_base->qac_queue_drop_count_data), new Qac_drop_count_data_map, {}, sizeof(Qac_drop_count_data) } },
    { "qac_port_drop_count_data", { const_cast<Qac_drop_count_data(*)>(&Pipe_block_reg_base->qac_port_drop_count_data), new Qac_drop_count_data_map, {}, sizeof(Qac_drop_count_data) } },
    { "qac_queue_drop_count_addr", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac_queue_drop_count_addr), 0, {}, sizeof(uint32_t) } },
    { "qac_port_drop_count_addr", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac_port_drop_count_addr), 0, {}, sizeof(uint32_t) } },
    { "qac_port_rx_disable", { const_cast<Qac_port_rx_disable(*)>(&Pipe_block_reg_base->qac_port_rx_disable), new Qac_port_rx_disable_map, {}, sizeof(Qac_port_rx_disable) } },
    { "qac_debug_register", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac_debug_register), 0, {}, sizeof(uint32_t) } },
    { "qac_ecc", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac_ecc), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Pipe_block_reg_base->intr, new Pipe_block_reg_intr_map, {}, sizeof(Pipe_block_reg_intr) } },
    { "queue_drop_table_sbe_errlog", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->queue_drop_table_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "queue_drop_table_mbe_errlog", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->queue_drop_table_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "port_drop_cnt_table_sbe_errlog", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->port_drop_cnt_table_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "port_drop_cnt_table_mbe_errlog", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->port_drop_cnt_table_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } },
    { "qac_indir_queue_resume_mem_addr", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac_indir_queue_resume_mem_addr), 0, {}, sizeof(uint32_t) } },
    { "qac_indir_queue_resume_mem_data_data", { const_cast<uint32_t(*)>(&Pipe_block_reg_base->qac_indir_queue_resume_mem_data_data), 0, {}, sizeof(uint32_t) } },
    { "queue_drop_state", { const_cast<uint32_t(*)[0x12]>(&Pipe_block_reg_base->queue_drop_state), 0, {0x12}, sizeof(uint32_t) } },
    { "queue_drop_yel_state", { const_cast<uint32_t(*)[0x12]>(&Pipe_block_reg_base->queue_drop_yel_state), 0, {0x12}, sizeof(uint32_t) } },
    { "queue_drop_red_state", { const_cast<uint32_t(*)[0x12]>(&Pipe_block_reg_base->queue_drop_red_state), 0, {0x12}, sizeof(uint32_t) } },
    { "port_drop_state", { const_cast<uint32_t(*)[0x48]>(&Pipe_block_reg_base->port_drop_state), 0, {0x48}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_qac_pipe_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_qac_pipe_rspec Tm_qac_pipe_rspec_base=0;
  Tm_qac_pipe_rspec_map() : RegisterMapper( {
    { "csr_mem_qac_queue_min_thrd_config", { &Tm_qac_pipe_rspec_base->csr_mem_qac_queue_min_thrd_config, new Qac_queue_min_thrd_config_map, {}, sizeof(Qac_queue_min_thrd_config) } },
    { "csr_mem_qac_queue_shr_thrd_config", { &Tm_qac_pipe_rspec_base->csr_mem_qac_queue_shr_thrd_config, new Qac_queue_shr_thrd_config_map, {}, sizeof(Qac_queue_shr_thrd_config) } },
    { "csr_mem_qac_queue_ap_config", { &Tm_qac_pipe_rspec_base->csr_mem_qac_queue_ap_config, new Qac_queue_ap_config_map, {}, sizeof(Qac_queue_ap_config) } },
    { "csr_mem_qac_queue_color_limit", { &Tm_qac_pipe_rspec_base->csr_mem_qac_queue_color_limit, new Qac_queue_color_limit_map, {}, sizeof(Qac_queue_color_limit) } },
    { "csr_mem_qac_queue_cell_count", { &Tm_qac_pipe_rspec_base->csr_mem_qac_queue_cell_count, new Qac_queue_cell_count_map, {}, sizeof(Qac_queue_cell_count) } },
    { "csr_mem_qac_queue_wm_cell_count", { &Tm_qac_pipe_rspec_base->csr_mem_qac_queue_wm_cell_count, new Qac_queue_wm_cell_count_map, {}, sizeof(Qac_queue_wm_cell_count) } },
    { "csr_mem_qac_port_wm_cell_count", { &Tm_qac_pipe_rspec_base->csr_mem_qac_port_wm_cell_count, new Qac_port_wm_cell_count_map, {}, sizeof(Qac_port_wm_cell_count) } },
    { "csr_mem_qac_port_config", { &Tm_qac_pipe_rspec_base->csr_mem_qac_port_config, new Qac_port_config_map, {}, sizeof(Qac_port_config) } },
    { "csr_mem_qac_port_cell_count", { &Tm_qac_pipe_rspec_base->csr_mem_qac_port_cell_count, new Qac_port_cell_count_map, {}, sizeof(Qac_port_cell_count) } },
    { "qac_reg", { &Tm_qac_pipe_rspec_base->qac_reg, new Pipe_block_reg_map, {}, sizeof(Pipe_block_reg) } }
    } )
  {}
};

struct Qac_common_block_map: public RegisterMapper {
  static constexpr PTR_Qac_common_block Qac_common_block_base=0;
  Qac_common_block_map() : RegisterMapper( {
    { "qac_scratch", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_scratch), 0, {}, sizeof(uint32_t) } },
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
    { "qac_intr_bmp", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_intr_bmp), 0, {}, sizeof(uint32_t) } },
    { "qac_glb_ap_gre_limit_cell", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_glb_ap_gre_limit_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_glb_ap_gre_resume_offset_cell", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_glb_ap_gre_resume_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "qac_glb_ap_red_limit_cell", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_glb_ap_red_limit_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_glb_ap_red_resume_offset_cell", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_glb_ap_red_resume_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "qac_glb_ap_yel_limit_cell", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_glb_ap_yel_limit_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_glb_ap_yel_resume_offset_cell", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_glb_ap_yel_resume_offset_cell), 0, {}, sizeof(uint32_t) } },
    { "qac_ep_cnt_cell", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_ep_cnt_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_wm_ep_cnt_cell", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_wm_ep_cnt_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_glb_ap_cnt_cell", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_glb_ap_cnt_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_wm_glb_ap_cnt_cell", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_wm_glb_ap_cnt_cell), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_ep_cnt_ph", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_ep_cnt_ph), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_glb_ap_cnt_ph", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_glb_ap_cnt_ph), 0, {0x4}, sizeof(uint32_t) } },
    { "qac_wm_glb_ap_cnt_ph", { const_cast<uint32_t(*)[0x4]>(&Qac_common_block_base->qac_wm_glb_ap_cnt_ph), 0, {0x4}, sizeof(uint32_t) } },
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
    { "qac_mcct_cnt_pkt", { const_cast<uint32_t(*)>(&Qac_common_block_base->qac_mcct_cnt_pkt), 0, {}, sizeof(uint32_t) } }
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

struct Tm_qac_top_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_qac_top_rspec Tm_qac_top_rspec_base=0;
  Tm_qac_top_rspec_map() : RegisterMapper( {
    { "qac_pipe", { &Tm_qac_top_rspec_base->qac_pipe, new Tm_qac_pipe_rspec_map, {0x4}, sizeof(Tm_qac_pipe_rspec) } },
    { "qac_common", { &Tm_qac_top_rspec_base->qac_common, new Tm_qac_common_rspec_map, {}, sizeof(Tm_qac_common_rspec) } }
    } )
  {}
};

struct Tm_sch_pipe_rspec_ecc_map: public RegisterMapper {
  static constexpr PTR_Tm_sch_pipe_rspec_ecc Tm_sch_pipe_rspec_ecc_base=0;
  Tm_sch_pipe_rspec_ecc_map() : RegisterMapper( {
    { "ecc_0_3", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_ecc_base->ecc_0_3), 0, {}, sizeof(uint32_t) } },
    { "ecc_1_3", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_ecc_base->ecc_1_3), 0, {}, sizeof(uint32_t) } },
    { "ecc_2_3", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_ecc_base->ecc_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_sch_pipe_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_sch_pipe_rspec_intr Tm_sch_pipe_rspec_intr_base=0;
  Tm_sch_pipe_rspec_intr_map() : RegisterMapper( {
    { "status", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
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
    { "tdm_table_sbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->tdm_table_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "tdm_table_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->tdm_table_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "q_minrate_sbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->q_minrate_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "q_minrate_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->q_minrate_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "q_excrate_sbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->q_excrate_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "q_excrate_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->q_excrate_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "q_maxrate_sbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->q_maxrate_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "q_maxrate_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->q_maxrate_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "p_maxrate_sbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->p_maxrate_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "p_maxrate_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->p_maxrate_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "upd_pex0_sbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_pex0_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "upd_pex0_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_pex0_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "upd_pex1_sbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_pex1_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "upd_pex1_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_pex1_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "upd_edprsr_sbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_edprsr_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "upd_edprsr_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->upd_edprsr_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "pex_credit_errlog", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->pex_credit_errlog), 0, {}, sizeof(uint32_t) } },
    { "ctrl", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "ready", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->ready), 0, {}, sizeof(uint32_t) } },
    { "global_bytecnt_adj", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->global_bytecnt_adj), 0, {}, sizeof(uint32_t) } },
    { "tdm_config", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->tdm_config), 0, {}, sizeof(uint32_t) } },
    { "tdm_table", { const_cast<uint32_t(*)[0x80]>(&Tm_sch_pipe_rspec_base->tdm_table), 0, {0x80}, sizeof(uint32_t) } },
    { "port_config", { const_cast<uint32_t(*)[0x48]>(&Tm_sch_pipe_rspec_base->port_config), 0, {0x48}, sizeof(uint32_t) } },
    { "port_max_lb_static_mem", { const_cast<uint32_t(*)[0x48]>(&Tm_sch_pipe_rspec_base->port_max_lb_static_mem), 0, {0x48}, sizeof(uint32_t) } },
    { "port_max_lb_dynamic_mem", { const_cast<uint32_t(*)[0x48]>(&Tm_sch_pipe_rspec_base->port_max_lb_dynamic_mem), 0, {0x48}, sizeof(uint32_t) } },
    { "port_pfc_status_mem", { const_cast<uint32_t(*)[0x48]>(&Tm_sch_pipe_rspec_base->port_pfc_status_mem), 0, {0x48}, sizeof(uint32_t) } },
    { "port_pex_status_mem", { const_cast<uint32_t(*)[0x48]>(&Tm_sch_pipe_rspec_base->port_pex_status_mem), 0, {0x48}, sizeof(uint32_t) } },
    { "queue_config", { const_cast<uint32_t(*)[0x240]>(&Tm_sch_pipe_rspec_base->queue_config), 0, {0x240}, sizeof(uint32_t) } },
    { "q_min_lb_static_mem", { const_cast<uint32_t(*)[0x240]>(&Tm_sch_pipe_rspec_base->q_min_lb_static_mem), 0, {0x240}, sizeof(uint32_t) } },
    { "q_min_lb_dynamic_mem", { const_cast<uint32_t(*)[0x240]>(&Tm_sch_pipe_rspec_base->q_min_lb_dynamic_mem), 0, {0x240}, sizeof(uint32_t) } },
    { "q_max_lb_static_mem", { const_cast<uint32_t(*)[0x240]>(&Tm_sch_pipe_rspec_base->q_max_lb_static_mem), 0, {0x240}, sizeof(uint32_t) } },
    { "q_max_lb_dynamic_mem", { const_cast<uint32_t(*)[0x240]>(&Tm_sch_pipe_rspec_base->q_max_lb_dynamic_mem), 0, {0x240}, sizeof(uint32_t) } },
    { "q_exc_static_mem", { const_cast<uint32_t(*)[0x240]>(&Tm_sch_pipe_rspec_base->q_exc_static_mem), 0, {0x240}, sizeof(uint32_t) } },
    { "q_exc_dynamic_mem", { const_cast<uint32_t(*)[0x240]>(&Tm_sch_pipe_rspec_base->q_exc_dynamic_mem), 0, {0x240}, sizeof(uint32_t) } },
    { "q_pfc_status_mem", { const_cast<uint32_t(*)[0x240]>(&Tm_sch_pipe_rspec_base->q_pfc_status_mem), 0, {0x240}, sizeof(uint32_t) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Tm_sch_pipe_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_sch_top_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_sch_top_rspec Tm_sch_top_rspec_base=0;
  Tm_sch_top_rspec_map() : RegisterMapper( {
    { "sch", { &Tm_sch_top_rspec_base->sch, new Tm_sch_pipe_rspec_map, {0x4}, sizeof(Tm_sch_pipe_rspec) } }
    } )
  {}
};

struct Tm_clc_pipe_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_clc_pipe_rspec_intr Tm_clc_pipe_rspec_intr_base=0;
  Tm_clc_pipe_rspec_intr_map() : RegisterMapper( {
    { "status", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Clc_inport_cell_cnt_map: public RegisterMapper {
  static constexpr PTR_Clc_inport_cell_cnt Clc_inport_cell_cnt_base=0;
  Clc_inport_cell_cnt_map() : RegisterMapper( {
    { "inport_cell_cnt_0_2", { const_cast<uint32_t(*)>(&Clc_inport_cell_cnt_base->inport_cell_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "inport_cell_cnt_1_2", { const_cast<uint32_t(*)>(&Clc_inport_cell_cnt_base->inport_cell_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Clc_inport_pkt_cnt_map: public RegisterMapper {
  static constexpr PTR_Clc_inport_pkt_cnt Clc_inport_pkt_cnt_base=0;
  Clc_inport_pkt_cnt_map() : RegisterMapper( {
    { "inport_pkt_cnt_0_2", { const_cast<uint32_t(*)>(&Clc_inport_pkt_cnt_base->inport_pkt_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "inport_pkt_cnt_1_2", { const_cast<uint32_t(*)>(&Clc_inport_pkt_cnt_base->inport_pkt_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Clc_tot_cell_cnt_map: public RegisterMapper {
  static constexpr PTR_Clc_tot_cell_cnt Clc_tot_cell_cnt_base=0;
  Clc_tot_cell_cnt_map() : RegisterMapper( {
    { "tot_cell_cnt_0_2", { const_cast<uint32_t(*)>(&Clc_tot_cell_cnt_base->tot_cell_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "tot_cell_cnt_1_2", { const_cast<uint32_t(*)>(&Clc_tot_cell_cnt_base->tot_cell_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Clc_tot_pkt_cnt_map: public RegisterMapper {
  static constexpr PTR_Clc_tot_pkt_cnt Clc_tot_pkt_cnt_base=0;
  Clc_tot_pkt_cnt_map() : RegisterMapper( {
    { "tot_pkt_cnt_0_2", { const_cast<uint32_t(*)>(&Clc_tot_pkt_cnt_base->tot_pkt_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "tot_pkt_cnt_1_2", { const_cast<uint32_t(*)>(&Clc_tot_pkt_cnt_base->tot_pkt_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Clc_uc_ct_cnt_map: public RegisterMapper {
  static constexpr PTR_Clc_uc_ct_cnt Clc_uc_ct_cnt_base=0;
  Clc_uc_ct_cnt_map() : RegisterMapper( {
    { "uc_ct_cnt_0_2", { const_cast<uint32_t(*)>(&Clc_uc_ct_cnt_base->uc_ct_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "uc_ct_cnt_1_2", { const_cast<uint32_t(*)>(&Clc_uc_ct_cnt_base->uc_ct_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Clc_mc_ct_cnt_map: public RegisterMapper {
  static constexpr PTR_Clc_mc_ct_cnt Clc_mc_ct_cnt_base=0;
  Clc_mc_ct_cnt_map() : RegisterMapper( {
    { "mc_ct_cnt_0_2", { const_cast<uint32_t(*)>(&Clc_mc_ct_cnt_base->mc_ct_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "mc_ct_cnt_1_2", { const_cast<uint32_t(*)>(&Clc_mc_ct_cnt_base->mc_ct_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_clc_pipe_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_clc_pipe_rspec Tm_clc_pipe_rspec_base=0;
  Tm_clc_pipe_rspec_map() : RegisterMapper( {
    { "pipe_ctrl", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->pipe_ctrl), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->ecc), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Tm_clc_pipe_rspec_base->intr, new Tm_clc_pipe_rspec_intr_map, {}, sizeof(Tm_clc_pipe_rspec_intr) } },
    { "fifo_threshold", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->fifo_threshold), 0, {}, sizeof(uint32_t) } },
    { "ind_addr", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->ind_addr), 0, {}, sizeof(uint32_t) } },
    { "ind_data", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->ind_data), 0, {}, sizeof(uint32_t) } },
    { "inport_cell_cnt", { const_cast<Clc_inport_cell_cnt(*)>(&Tm_clc_pipe_rspec_base->inport_cell_cnt), new Clc_inport_cell_cnt_map, {}, sizeof(Clc_inport_cell_cnt) } },
    { "inport_pkt_cnt", { const_cast<Clc_inport_pkt_cnt(*)>(&Tm_clc_pipe_rspec_base->inport_pkt_cnt), new Clc_inport_pkt_cnt_map, {}, sizeof(Clc_inport_pkt_cnt) } },
    { "tot_cell_cnt", { const_cast<Clc_tot_cell_cnt(*)>(&Tm_clc_pipe_rspec_base->tot_cell_cnt), new Clc_tot_cell_cnt_map, {}, sizeof(Clc_tot_cell_cnt) } },
    { "tot_pkt_cnt", { const_cast<Clc_tot_pkt_cnt(*)>(&Tm_clc_pipe_rspec_base->tot_pkt_cnt), new Clc_tot_pkt_cnt_map, {}, sizeof(Clc_tot_pkt_cnt) } },
    { "uc_ct_cnt", { const_cast<Clc_uc_ct_cnt(*)>(&Tm_clc_pipe_rspec_base->uc_ct_cnt), new Clc_uc_ct_cnt_map, {}, sizeof(Clc_uc_ct_cnt) } },
    { "mc_ct_cnt", { const_cast<Clc_mc_ct_cnt(*)>(&Tm_clc_pipe_rspec_base->mc_ct_cnt), new Clc_mc_ct_cnt_map, {}, sizeof(Clc_mc_ct_cnt) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Tm_clc_pipe_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_pex_pipe_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_pex_pipe_rspec_intr Tm_pex_pipe_rspec_intr_base=0;
  Tm_pex_pipe_rspec_intr_map() : RegisterMapper( {
    { "status", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pex_port_fp_cnt_map: public RegisterMapper {
  static constexpr PTR_Pex_port_fp_cnt Pex_port_fp_cnt_base=0;
  Pex_port_fp_cnt_map() : RegisterMapper( {
    { "port_first_cnt_0_2", { const_cast<uint32_t(*)>(&Pex_port_fp_cnt_base->port_first_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "port_first_cnt_1_2", { const_cast<uint32_t(*)>(&Pex_port_fp_cnt_base->port_first_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pex_port_sp_cnt_map: public RegisterMapper {
  static constexpr PTR_Pex_port_sp_cnt Pex_port_sp_cnt_base=0;
  Pex_port_sp_cnt_map() : RegisterMapper( {
    { "port_second_cnt_0_2", { const_cast<uint32_t(*)>(&Pex_port_sp_cnt_base->port_second_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "port_second_cnt_1_2", { const_cast<uint32_t(*)>(&Pex_port_sp_cnt_base->port_second_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pex_port_cell_cnt_map: public RegisterMapper {
  static constexpr PTR_Pex_port_cell_cnt Pex_port_cell_cnt_base=0;
  Pex_port_cell_cnt_map() : RegisterMapper( {
    { "port_cell_cnt_0_2", { const_cast<uint32_t(*)>(&Pex_port_cell_cnt_base->port_cell_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "port_cell_cnt_1_2", { const_cast<uint32_t(*)>(&Pex_port_cell_cnt_base->port_cell_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pex_port_pkt_cnt_map: public RegisterMapper {
  static constexpr PTR_Pex_port_pkt_cnt Pex_port_pkt_cnt_base=0;
  Pex_port_pkt_cnt_map() : RegisterMapper( {
    { "port_pkt_cnt_0_2", { const_cast<uint32_t(*)>(&Pex_port_pkt_cnt_base->port_pkt_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "port_pkt_cnt_1_2", { const_cast<uint32_t(*)>(&Pex_port_pkt_cnt_base->port_pkt_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pex_tot_cell_cnt0_map: public RegisterMapper {
  static constexpr PTR_Pex_tot_cell_cnt0 Pex_tot_cell_cnt0_base=0;
  Pex_tot_cell_cnt0_map() : RegisterMapper( {
    { "tot_cell_cnt0_0_2", { const_cast<uint32_t(*)>(&Pex_tot_cell_cnt0_base->tot_cell_cnt0_0_2), 0, {}, sizeof(uint32_t) } },
    { "tot_cell_cnt0_1_2", { const_cast<uint32_t(*)>(&Pex_tot_cell_cnt0_base->tot_cell_cnt0_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pex_tot_cell_cnt1_map: public RegisterMapper {
  static constexpr PTR_Pex_tot_cell_cnt1 Pex_tot_cell_cnt1_base=0;
  Pex_tot_cell_cnt1_map() : RegisterMapper( {
    { "tot_cell_cnt1_0_2", { const_cast<uint32_t(*)>(&Pex_tot_cell_cnt1_base->tot_cell_cnt1_0_2), 0, {}, sizeof(uint32_t) } },
    { "tot_cell_cnt1_1_2", { const_cast<uint32_t(*)>(&Pex_tot_cell_cnt1_base->tot_cell_cnt1_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pex_tot_cell_cnt2_map: public RegisterMapper {
  static constexpr PTR_Pex_tot_cell_cnt2 Pex_tot_cell_cnt2_base=0;
  Pex_tot_cell_cnt2_map() : RegisterMapper( {
    { "tot_cell_cnt2_0_2", { const_cast<uint32_t(*)>(&Pex_tot_cell_cnt2_base->tot_cell_cnt2_0_2), 0, {}, sizeof(uint32_t) } },
    { "tot_cell_cnt2_1_2", { const_cast<uint32_t(*)>(&Pex_tot_cell_cnt2_base->tot_cell_cnt2_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pex_tot_pkt_cnt_map: public RegisterMapper {
  static constexpr PTR_Pex_tot_pkt_cnt Pex_tot_pkt_cnt_base=0;
  Pex_tot_pkt_cnt_map() : RegisterMapper( {
    { "tot_pkt_cnt_0_2", { const_cast<uint32_t(*)>(&Pex_tot_pkt_cnt_base->tot_pkt_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "tot_pkt_cnt_1_2", { const_cast<uint32_t(*)>(&Pex_tot_pkt_cnt_base->tot_pkt_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pex_tot_err_pkt_cnt_map: public RegisterMapper {
  static constexpr PTR_Pex_tot_err_pkt_cnt Pex_tot_err_pkt_cnt_base=0;
  Pex_tot_err_pkt_cnt_map() : RegisterMapper( {
    { "tot_err_pkt_cnt_0_2", { const_cast<uint32_t(*)>(&Pex_tot_err_pkt_cnt_base->tot_err_pkt_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "tot_err_pkt_cnt_1_2", { const_cast<uint32_t(*)>(&Pex_tot_err_pkt_cnt_base->tot_err_pkt_cnt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_pex_pipe_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_pex_pipe_rspec Tm_pex_pipe_rspec_base=0;
  Tm_pex_pipe_rspec_map() : RegisterMapper( {
    { "pipe_ctrl", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->pipe_ctrl), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Tm_pex_pipe_rspec_base->intr, new Tm_pex_pipe_rspec_intr_map, {}, sizeof(Tm_pex_pipe_rspec_intr) } },
    { "linkmem_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->linkmem_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "os_pg_map", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->os_pg_map), 0, {}, sizeof(uint32_t) } },
    { "pt_state", { const_cast<uint32_t(*)[0xb]>(&Tm_pex_pipe_rspec_base->pt_state), 0, {0xb}, sizeof(uint32_t) } },
    { "pt_epb_cred", { const_cast<uint32_t(*)[0x12]>(&Tm_pex_pipe_rspec_base->pt_epb_cred), 0, {0x12}, sizeof(uint32_t) } },
    { "q_empty", { const_cast<uint32_t(*)[0x14]>(&Tm_pex_pipe_rspec_base->q_empty), 0, {0x14}, sizeof(uint32_t) } },
    { "pt_gap_lim", { const_cast<uint32_t(*)[0x12]>(&Tm_pex_pipe_rspec_base->pt_gap_lim), 0, {0x12}, sizeof(uint32_t) } },
    { "pg_single", { const_cast<uint32_t(*)[0x12]>(&Tm_pex_pipe_rspec_base->pg_single), 0, {0x12}, sizeof(uint32_t) } },
    { "pt_gap_wm", { const_cast<uint32_t(*)[0x48]>(&Tm_pex_pipe_rspec_base->pt_gap_wm), 0, {0x48}, sizeof(uint32_t) } },
    { "ct_timer", { const_cast<uint32_t(*)[0x12]>(&Tm_pex_pipe_rspec_base->ct_timer), 0, {0x12}, sizeof(uint32_t) } },
    { "ind_addr", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->ind_addr), 0, {}, sizeof(uint32_t) } },
    { "ind_data", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->ind_data), 0, {}, sizeof(uint32_t) } },
    { "port_first_cnt", { const_cast<Pex_port_fp_cnt(*)>(&Tm_pex_pipe_rspec_base->port_first_cnt), new Pex_port_fp_cnt_map, {}, sizeof(Pex_port_fp_cnt) } },
    { "port_second_cnt", { const_cast<Pex_port_sp_cnt(*)>(&Tm_pex_pipe_rspec_base->port_second_cnt), new Pex_port_sp_cnt_map, {}, sizeof(Pex_port_sp_cnt) } },
    { "port_cell_cnt", { const_cast<Pex_port_cell_cnt(*)>(&Tm_pex_pipe_rspec_base->port_cell_cnt), new Pex_port_cell_cnt_map, {}, sizeof(Pex_port_cell_cnt) } },
    { "port_pkt_cnt", { const_cast<Pex_port_pkt_cnt(*)>(&Tm_pex_pipe_rspec_base->port_pkt_cnt), new Pex_port_pkt_cnt_map, {}, sizeof(Pex_port_pkt_cnt) } },
    { "tot_cell_cnt0", { const_cast<Pex_tot_cell_cnt0(*)>(&Tm_pex_pipe_rspec_base->tot_cell_cnt0), new Pex_tot_cell_cnt0_map, {}, sizeof(Pex_tot_cell_cnt0) } },
    { "tot_cell_cnt1", { const_cast<Pex_tot_cell_cnt1(*)>(&Tm_pex_pipe_rspec_base->tot_cell_cnt1), new Pex_tot_cell_cnt1_map, {}, sizeof(Pex_tot_cell_cnt1) } },
    { "tot_cell_cnt2", { const_cast<Pex_tot_cell_cnt2(*)>(&Tm_pex_pipe_rspec_base->tot_cell_cnt2), new Pex_tot_cell_cnt2_map, {}, sizeof(Pex_tot_cell_cnt2) } },
    { "tot_pkt_cnt", { const_cast<Pex_tot_pkt_cnt(*)>(&Tm_pex_pipe_rspec_base->tot_pkt_cnt), new Pex_tot_pkt_cnt_map, {}, sizeof(Pex_tot_pkt_cnt) } },
    { "tot_err_pkt_cnt", { const_cast<Pex_tot_err_pkt_cnt(*)>(&Tm_pex_pipe_rspec_base->tot_err_pkt_cnt), new Pex_tot_err_pkt_cnt_map, {}, sizeof(Pex_tot_err_pkt_cnt) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Tm_pex_pipe_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_clc_common_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_clc_common_rspec Tm_clc_common_rspec_base=0;
  Tm_clc_common_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Tm_clc_common_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Tm_clc_common_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "clm_dft_csr", { const_cast<uint32_t(*)>(&Tm_clc_common_rspec_base->clm_dft_csr), 0, {}, sizeof(uint32_t) } },
    { "top_ctrl", { const_cast<uint32_t(*)>(&Tm_clc_common_rspec_base->top_ctrl), 0, {}, sizeof(uint32_t) } },
    { "pt_speed", { const_cast<uint32_t(*)[0x24]>(&Tm_clc_common_rspec_base->pt_speed), 0, {0x24}, sizeof(uint32_t) } },
    { "clm_blk_rdy", { const_cast<uint32_t(*)>(&Tm_clc_common_rspec_base->clm_blk_rdy), 0, {}, sizeof(uint32_t) } },
    { "clm_blk_reset", { const_cast<uint32_t(*)>(&Tm_clc_common_rspec_base->clm_blk_reset), 0, {}, sizeof(uint32_t) } },
    { "ct_state", { const_cast<uint32_t(*)[0xc]>(&Tm_clc_common_rspec_base->ct_state), 0, {0xc}, sizeof(uint32_t) } },
    { "tot_th", { const_cast<uint32_t(*)>(&Tm_clc_common_rspec_base->tot_th), 0, {}, sizeof(uint32_t) } },
    { "pt_th", { const_cast<uint32_t(*)[0x24]>(&Tm_clc_common_rspec_base->pt_th), 0, {0x24}, sizeof(uint32_t) } },
    { "ct_tot_cnt", { const_cast<uint32_t(*)>(&Tm_clc_common_rspec_base->ct_tot_cnt), 0, {}, sizeof(uint32_t) } },
    { "ct_pt_cnt", { const_cast<uint32_t(*)[0x48]>(&Tm_clc_common_rspec_base->ct_pt_cnt), 0, {0x48}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_clc_top_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_clc_top_rspec Tm_clc_top_rspec_base=0;
  Tm_clc_top_rspec_map() : RegisterMapper( {
    { "clc", { &Tm_clc_top_rspec_base->clc, new Tm_clc_pipe_rspec_map, {0x4}, sizeof(Tm_clc_pipe_rspec) } },
    { "pex", { &Tm_clc_top_rspec_base->pex, new Tm_pex_pipe_rspec_map, {0x4}, sizeof(Tm_pex_pipe_rspec) } },
    { "clc_common", { &Tm_clc_top_rspec_base->clc_common, new Tm_clc_common_rspec_map, {}, sizeof(Tm_clc_common_rspec) } }
    } )
  {}
};

struct Tm_qlc_pipe_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_qlc_pipe_rspec_intr Tm_qlc_pipe_rspec_intr_base=0;
  Tm_qlc_pipe_rspec_intr_map() : RegisterMapper( {
    { "status", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Qlc_ind_data_map: public RegisterMapper {
  static constexpr PTR_Qlc_ind_data Qlc_ind_data_base=0;
  Qlc_ind_data_map() : RegisterMapper( {
    { "ind_data_0_2", { const_cast<uint32_t(*)>(&Qlc_ind_data_base->ind_data_0_2), 0, {}, sizeof(uint32_t) } },
    { "ind_data_1_2", { const_cast<uint32_t(*)>(&Qlc_ind_data_base->ind_data_1_2), 0, {}, sizeof(uint32_t) } }
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
    { "control", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_base->control), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_base->ecc), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Tm_qlc_pipe_rspec_base->intr, new Tm_qlc_pipe_rspec_intr_map, {}, sizeof(Tm_qlc_pipe_rspec_intr) } },
    { "linkmem_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_base->linkmem_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "linkmem_sbe_errlog", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_base->linkmem_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "dis_qlen", { const_cast<uint32_t(*)[0x20]>(&Tm_qlc_pipe_rspec_base->dis_qlen), 0, {0x20}, sizeof(uint32_t) } },
    { "dis_cred", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_base->dis_cred), 0, {}, sizeof(uint32_t) } },
    { "ind_target", { const_cast<uint32_t(*)>(&Tm_qlc_pipe_rspec_base->ind_target), 0, {}, sizeof(uint32_t) } },
    { "ind_data", { const_cast<Qlc_ind_data(*)>(&Tm_qlc_pipe_rspec_base->ind_data), new Qlc_ind_data_map, {}, sizeof(Qlc_ind_data) } },
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

struct Tm_qlc_common_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_qlc_common_rspec Tm_qlc_common_rspec_base=0;
  Tm_qlc_common_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Tm_qlc_common_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Tm_qlc_common_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "qlm_dft_csr", { const_cast<uint32_t(*)>(&Tm_qlc_common_rspec_base->qlm_dft_csr), 0, {}, sizeof(uint32_t) } },
    { "blk_rdy", { const_cast<uint32_t(*)[0x2]>(&Tm_qlc_common_rspec_base->blk_rdy), 0, {0x2}, sizeof(uint32_t) } },
    { "qlm_blk_reset", { const_cast<uint32_t(*)[0x2]>(&Tm_qlc_common_rspec_base->qlm_blk_reset), 0, {0x2}, sizeof(uint32_t) } }
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
    { "status", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
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
    { "control", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_base->control), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_base->ecc), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Tm_prc_pipe_rspec_base->intr, new Tm_prc_pipe_rspec_intr_map, {}, sizeof(Tm_prc_pipe_rspec_intr) } },
    { "prm_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_base->prm_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "prm_sbe_errlog", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_base->prm_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "cache_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_base->cache_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "cache_sbe_errlog", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_base->cache_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "qac_cnt", { const_cast<Prc_qac_cnt(*)>(&Tm_prc_pipe_rspec_base->qac_cnt), new Prc_qac_cnt_map, {}, sizeof(Prc_qac_cnt) } },
    { "qac_zero_cnt", { const_cast<Prc_qac_zero_cnt(*)>(&Tm_prc_pipe_rspec_base->qac_zero_cnt), new Prc_qac_zero_cnt_map, {}, sizeof(Prc_qac_zero_cnt) } },
    { "pex_cnt", { const_cast<Prc_pex_cnt(*)>(&Tm_prc_pipe_rspec_base->pex_cnt), new Prc_pex_cnt_map, {}, sizeof(Prc_pex_cnt) } },
    { "pex_zero_cnt", { const_cast<Prc_pex_zero_cnt(*)>(&Tm_prc_pipe_rspec_base->pex_zero_cnt), new Prc_pex_zero_cnt_map, {}, sizeof(Prc_pex_zero_cnt) } },
    { "ind_addr", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_base->ind_addr), 0, {}, sizeof(uint32_t) } },
    { "ind_data_0", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_base->ind_data_0), 0, {}, sizeof(uint32_t) } },
    { "ind_data_1", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_base->ind_data_1), 0, {}, sizeof(uint32_t) } },
    { "ind_data_2", { const_cast<uint32_t(*)>(&Tm_prc_pipe_rspec_base->ind_data_2), 0, {}, sizeof(uint32_t) } },
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
    { "int_stat", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->int_stat), 0, {}, sizeof(uint32_t) } },
    { "int_en0", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->int_en0), 0, {}, sizeof(uint32_t) } },
    { "int_en1", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->int_en1), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->int_inj), 0, {}, sizeof(uint32_t) } },
    { "freeze_en", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->freeze_en), 0, {}, sizeof(uint32_t) } },
    { "fifo_ph_count", { const_cast<uint32_t(*)[0x4]>(&Tm_pre_pipe_rspec_base->fifo_ph_count), 0, {0x4}, sizeof(uint32_t) } },
    { "table_ph_count", { const_cast<uint32_t(*)[0x2]>(&Tm_pre_pipe_rspec_base->table_ph_count), 0, {0x2}, sizeof(uint32_t) } },
    { "cpu_copies", { const_cast<Pre_ctr48_cpu_copies(*)>(&Tm_pre_pipe_rspec_base->cpu_copies), new Pre_ctr48_cpu_copies_map, {}, sizeof(Pre_ctr48_cpu_copies) } },
    { "ph_processed", { const_cast<Pre_ctr48_ph_processed(*)>(&Tm_pre_pipe_rspec_base->ph_processed), new Pre_ctr48_ph_processed_map, {}, sizeof(Pre_ctr48_ph_processed) } },
    { "total_copies", { const_cast<Pre_ctr48_total_copies(*)>(&Tm_pre_pipe_rspec_base->total_copies), new Pre_ctr48_total_copies_map, {}, sizeof(Pre_ctr48_total_copies) } },
    { "xid_prunes", { const_cast<Pre_ctr48_xid_prunes(*)>(&Tm_pre_pipe_rspec_base->xid_prunes), new Pre_ctr48_xid_prunes_map, {}, sizeof(Pre_ctr48_xid_prunes) } },
    { "yid_prunes", { const_cast<Pre_ctr48_yid_prunes(*)>(&Tm_pre_pipe_rspec_base->yid_prunes), new Pre_ctr48_yid_prunes_map, {}, sizeof(Pre_ctr48_yid_prunes) } },
    { "filtered_ph_processed", { const_cast<Pre_ctr48_ph_processed(*)>(&Tm_pre_pipe_rspec_base->filtered_ph_processed), new Pre_ctr48_ph_processed_map, {}, sizeof(Pre_ctr48_ph_processed) } },
    { "filtered_total_copies", { const_cast<Pre_ctr48_total_copies(*)>(&Tm_pre_pipe_rspec_base->filtered_total_copies), new Pre_ctr48_total_copies_map, {}, sizeof(Pre_ctr48_total_copies) } },
    { "filtered_xid_prunes", { const_cast<Pre_ctr48_xid_prunes(*)>(&Tm_pre_pipe_rspec_base->filtered_xid_prunes), new Pre_ctr48_xid_prunes_map, {}, sizeof(Pre_ctr48_xid_prunes) } },
    { "filtered_yid_prunes", { const_cast<Pre_ctr48_yid_prunes(*)>(&Tm_pre_pipe_rspec_base->filtered_yid_prunes), new Pre_ctr48_yid_prunes_map, {}, sizeof(Pre_ctr48_yid_prunes) } },
    { "filtered_port_vector", { const_cast<Pre_port_vector(*)>(&Tm_pre_pipe_rspec_base->filtered_port_vector), new Pre_port_vector_map, {}, sizeof(Pre_port_vector) } },
    { "rdm_ph_log", { const_cast<uint32_t(*)[0x5]>(&Tm_pre_pipe_rspec_base->rdm_ph_log), 0, {0x5}, sizeof(uint32_t) } },
    { "ph_lost", { const_cast<Pre_ctr48_ph_lost(*)>(&Tm_pre_pipe_rspec_base->ph_lost), new Pre_ctr48_ph_lost_map, {}, sizeof(Pre_ctr48_ph_lost) } },
    { "packet_drop", { const_cast<Pre_ctr48_packet_drop(*)>(&Tm_pre_pipe_rspec_base->packet_drop), new Pre_ctr48_packet_drop_map, {}, sizeof(Pre_ctr48_packet_drop) } },
    { "max_l1_node_log", { const_cast<Pre_max_l1_node_log(*)>(&Tm_pre_pipe_rspec_base->max_l1_node_log), new Pre_max_l1_node_log_map, {}, sizeof(Pre_max_l1_node_log) } },
    { "max_l2_node_log", { const_cast<Pre_max_l2_node_log(*)>(&Tm_pre_pipe_rspec_base->max_l2_node_log), new Pre_max_l2_node_log_map, {}, sizeof(Pre_max_l2_node_log) } },
    { "illegal_l1_node_log", { const_cast<Pre_illegal_l1_node_log(*)>(&Tm_pre_pipe_rspec_base->illegal_l1_node_log), new Pre_illegal_l1_node_log_map, {}, sizeof(Pre_illegal_l1_node_log) } },
    { "illegal_l2_node_log", { const_cast<Pre_illegal_l2_node_log(*)>(&Tm_pre_pipe_rspec_base->illegal_l2_node_log), new Pre_illegal_l2_node_log_map, {}, sizeof(Pre_illegal_l2_node_log) } },
    { "sbe_log", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->sbe_log), 0, {}, sizeof(uint32_t) } },
    { "mbe_log", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->mbe_log), 0, {}, sizeof(uint32_t) } },
    { "credit_log", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->credit_log), 0, {}, sizeof(uint32_t) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Tm_pre_pipe_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pre_rdm_blk_id_map: public RegisterMapper {
  static constexpr PTR_Pre_rdm_blk_id Pre_rdm_blk_id_base=0;
  Pre_rdm_blk_id_map() : RegisterMapper( {
    { "blk_id_0_13", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_0_13), 0, {}, sizeof(uint32_t) } },
    { "blk_id_1_13", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_1_13), 0, {}, sizeof(uint32_t) } },
    { "blk_id_2_13", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_2_13), 0, {}, sizeof(uint32_t) } },
    { "blk_id_3_13", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_3_13), 0, {}, sizeof(uint32_t) } },
    { "blk_id_4_13", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_4_13), 0, {}, sizeof(uint32_t) } },
    { "blk_id_5_13", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_5_13), 0, {}, sizeof(uint32_t) } },
    { "blk_id_6_13", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_6_13), 0, {}, sizeof(uint32_t) } },
    { "blk_id_7_13", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_7_13), 0, {}, sizeof(uint32_t) } },
    { "blk_id_8_13", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_8_13), 0, {}, sizeof(uint32_t) } },
    { "blk_id_9_13", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_9_13), 0, {}, sizeof(uint32_t) } },
    { "blk_id_10_13", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_10_13), 0, {}, sizeof(uint32_t) } },
    { "blk_id_11_13", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_11_13), 0, {}, sizeof(uint32_t) } },
    { "blk_id_12_13", { const_cast<uint32_t(*)>(&Pre_rdm_blk_id_base->blk_id_12_13), 0, {}, sizeof(uint32_t) } }
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

struct Tm_pre_reg_top_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_pre_reg_top_rspec Tm_pre_reg_top_rspec_base=0;
  Tm_pre_reg_top_rspec_map() : RegisterMapper( {
    { "pre", { &Tm_pre_reg_top_rspec_base->pre, new Tm_pre_pipe_rspec_map, {0x4}, sizeof(Tm_pre_pipe_rspec) } },
    { "pre_common", { &Tm_pre_reg_top_rspec_base->pre_common, new Tm_pre_common_rspec_map, {}, sizeof(Tm_pre_common_rspec) } }
    } )
  {}
};

struct Tm_psc_pipe_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_psc_pipe_rspec_intr Tm_psc_pipe_rspec_intr_base=0;
  Tm_psc_pipe_rspec_intr_map() : RegisterMapper( {
    { "status", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Psc_psm_indir_access_data_r_map: public RegisterMapper {
  static constexpr PTR_Psc_psm_indir_access_data_r Psc_psm_indir_access_data_r_base=0;
  Psc_psm_indir_access_data_r_map() : RegisterMapper( {
    { "psm_indir_access_data_0_6", { const_cast<uint32_t(*)>(&Psc_psm_indir_access_data_r_base->psm_indir_access_data_0_6), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_data_1_6", { const_cast<uint32_t(*)>(&Psc_psm_indir_access_data_r_base->psm_indir_access_data_1_6), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_data_2_6", { const_cast<uint32_t(*)>(&Psc_psm_indir_access_data_r_base->psm_indir_access_data_2_6), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_data_3_6", { const_cast<uint32_t(*)>(&Psc_psm_indir_access_data_r_base->psm_indir_access_data_3_6), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_data_4_6", { const_cast<uint32_t(*)>(&Psc_psm_indir_access_data_r_base->psm_indir_access_data_4_6), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_data_5_6", { const_cast<uint32_t(*)>(&Psc_psm_indir_access_data_r_base->psm_indir_access_data_5_6), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_psc_pipe_rspec_map: public RegisterMapper {
  static constexpr PTR_Tm_psc_pipe_rspec Tm_psc_pipe_rspec_base=0;
  Tm_psc_pipe_rspec_map() : RegisterMapper( {
    { "psc_ph_used", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_base->psc_ph_used), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_base->ecc), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Tm_psc_pipe_rspec_base->intr, new Tm_psc_pipe_rspec_intr_map, {}, sizeof(Tm_psc_pipe_rspec_intr) } },
    { "psm_sbe_errlog", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_base->psm_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "psm_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_base->psm_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_addr", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_base->psm_indir_access_addr), 0, {}, sizeof(uint32_t) } },
    { "psm_indir_access_data", { const_cast<Psc_psm_indir_access_data_r(*)>(&Tm_psc_pipe_rspec_base->psm_indir_access_data), new Psc_psm_indir_access_data_r_map, {}, sizeof(Psc_psm_indir_access_data_r) } },
    { "debug_bus_ctrl", { const_cast<uint32_t(*)>(&Tm_psc_pipe_rspec_base->debug_bus_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Psc_block_valid_r_map: public RegisterMapper {
  static constexpr PTR_Psc_block_valid_r Psc_block_valid_r_base=0;
  Psc_block_valid_r_map() : RegisterMapper( {
    { "block_valid_0_2", { const_cast<uint32_t(*)>(&Psc_block_valid_r_base->block_valid_0_2), 0, {}, sizeof(uint32_t) } },
    { "block_valid_1_2", { const_cast<uint32_t(*)>(&Psc_block_valid_r_base->block_valid_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Psc_block_reset_r_map: public RegisterMapper {
  static constexpr PTR_Psc_block_reset_r Psc_block_reset_r_base=0;
  Psc_block_reset_r_map() : RegisterMapper( {
    { "block_reset_0_2", { const_cast<uint32_t(*)>(&Psc_block_reset_r_base->block_reset_0_2), 0, {}, sizeof(uint32_t) } },
    { "block_reset_1_2", { const_cast<uint32_t(*)>(&Psc_block_reset_r_base->block_reset_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Psc_block_enable_r_map: public RegisterMapper {
  static constexpr PTR_Psc_block_enable_r Psc_block_enable_r_base=0;
  Psc_block_enable_r_map() : RegisterMapper( {
    { "block_enable_0_2", { const_cast<uint32_t(*)>(&Psc_block_enable_r_base->block_enable_0_2), 0, {}, sizeof(uint32_t) } },
    { "block_enable_1_2", { const_cast<uint32_t(*)>(&Psc_block_enable_r_base->block_enable_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Psc_block_ready_r_map: public RegisterMapper {
  static constexpr PTR_Psc_block_ready_r Psc_block_ready_r_base=0;
  Psc_block_ready_r_map() : RegisterMapper( {
    { "block_ready_0_2", { const_cast<uint32_t(*)>(&Psc_block_ready_r_base->block_ready_0_2), 0, {}, sizeof(uint32_t) } },
    { "block_ready_1_2", { const_cast<uint32_t(*)>(&Psc_block_ready_r_base->block_ready_1_2), 0, {}, sizeof(uint32_t) } }
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

struct Psc_bank_ctrl_r_map: public RegisterMapper {
  static constexpr PTR_Psc_bank_ctrl_r Psc_bank_ctrl_r_base=0;
  Psc_bank_ctrl_r_map() : RegisterMapper( {
    { "bank_indir_access_data_0_2", { const_cast<uint32_t(*)>(&Psc_bank_ctrl_r_base->bank_indir_access_data_0_2), 0, {}, sizeof(uint32_t) } },
    { "bank_indir_access_data_1_2", { const_cast<uint32_t(*)>(&Psc_bank_ctrl_r_base->bank_indir_access_data_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_psc_common_rspec_ecc_map: public RegisterMapper {
  static constexpr PTR_Tm_psc_common_rspec_ecc Tm_psc_common_rspec_ecc_base=0;
  Tm_psc_common_rspec_ecc_map() : RegisterMapper( {
    { "ecc_0_5", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_0_5), 0, {}, sizeof(uint32_t) } },
    { "ecc_1_5", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_1_5), 0, {}, sizeof(uint32_t) } },
    { "ecc_2_5", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_2_5), 0, {}, sizeof(uint32_t) } },
    { "ecc_3_5", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_3_5), 0, {}, sizeof(uint32_t) } },
    { "ecc_4_5", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_ecc_base->ecc_4_5), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Tm_psc_common_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Tm_psc_common_rspec_intr Tm_psc_common_rspec_intr_base=0;
  Tm_psc_common_rspec_intr_map() : RegisterMapper( {
    { "status", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_intr_base->inject), 0, {}, sizeof(uint32_t) } },
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
    { "block", { &Tm_psc_common_rspec_base->block, new Psc_block_g_map, {0x24}, sizeof(Psc_block_g) } },
    { "bank_indir_access_addr", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->bank_indir_access_addr), 0, {}, sizeof(uint32_t) } },
    { "bank_indir_access_data", { const_cast<Psc_bank_ctrl_r(*)>(&Tm_psc_common_rspec_base->bank_indir_access_data), new Psc_bank_ctrl_r_map, {}, sizeof(Psc_bank_ctrl_r) } },
    { "lmem_indir_access_addr", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->lmem_indir_access_addr), 0, {}, sizeof(uint32_t) } },
    { "lmem_indir_access_data", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->lmem_indir_access_data), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<Tm_psc_common_rspec_ecc(*)>(&Tm_psc_common_rspec_base->ecc), new Tm_psc_common_rspec_ecc_map, {}, sizeof(Tm_psc_common_rspec_ecc) } },
    { "intr", { &Tm_psc_common_rspec_base->intr, new Tm_psc_common_rspec_intr_map, {}, sizeof(Tm_psc_common_rspec_intr) } },
    { "linkmem_sbe_errlog", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->linkmem_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "linkmem_mbe_errlog", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->linkmem_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "overflow_errlog", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->overflow_errlog), 0, {}, sizeof(uint32_t) } },
    { "underflow_errlog", { const_cast<uint32_t(*)>(&Tm_psc_common_rspec_base->underflow_errlog), 0, {}, sizeof(uint32_t) } }
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
    { "tm_caa", { &Tm_top_rspec_base->tm_caa, new Tm_caa_rspec_map, {}, sizeof(Tm_caa_rspec) } },
    { "tm_qac_top", { &Tm_top_rspec_base->tm_qac_top, new Tm_qac_top_rspec_map, {}, sizeof(Tm_qac_top_rspec) } },
    { "tm_sch_top", { &Tm_top_rspec_base->tm_sch_top, new Tm_sch_top_rspec_map, {}, sizeof(Tm_sch_top_rspec) } },
    { "tm_clc_top", { &Tm_top_rspec_base->tm_clc_top, new Tm_clc_top_rspec_map, {}, sizeof(Tm_clc_top_rspec) } },
    { "tm_qlc_top", { &Tm_top_rspec_base->tm_qlc_top, new Tm_qlc_top_rspec_map, {}, sizeof(Tm_qlc_top_rspec) } },
    { "tm_prc_top", { &Tm_top_rspec_base->tm_prc_top, new Tm_prc_top_rspec_map, {}, sizeof(Tm_prc_top_rspec) } },
    { "tm_pre_top", { &Tm_top_rspec_base->tm_pre_top, new Tm_pre_reg_top_rspec_map, {}, sizeof(Tm_pre_reg_top_rspec) } },
    { "tm_psc_top", { &Tm_top_rspec_base->tm_psc_top, new Tm_psc_top_rspec_map, {}, sizeof(Tm_psc_top_rspec) } }
    } )
  {}
};

struct Dvsl_addrmap_map: public RegisterMapper {
  static constexpr PTR_Dvsl_addrmap Dvsl_addrmap_base=0;
  Dvsl_addrmap_map() : RegisterMapper( {
    { "pcie_bar01_regs", { &Dvsl_addrmap_base->pcie_bar01_regs, new Pcie_bar01_regs_map, {}, sizeof(Pcie_bar01_regs) } },
    { "misc_regs", { &Dvsl_addrmap_base->misc_regs, new Misc_regs_map, {}, sizeof(Misc_regs) } },
    { "mbc", { &Dvsl_addrmap_base->mbc, new Mbc_rspec_map, {}, sizeof(Mbc_rspec) } },
    { "pbc", { &Dvsl_addrmap_base->pbc, new Pbc_rspec_map, {}, sizeof(Pbc_rspec) } },
    { "cbc", { &Dvsl_addrmap_base->cbc, new Cbc_rspec_map, {}, sizeof(Cbc_rspec) } },
    { "tbc", { &Dvsl_addrmap_base->tbc, new Tbc_rspec_map, {}, sizeof(Tbc_rspec) } },
    { "lfltr0", { &Dvsl_addrmap_base->lfltr0, new Lfltr_rspec_map, {}, sizeof(Lfltr_rspec) } },
    { "lfltr1", { &Dvsl_addrmap_base->lfltr1, new Lfltr_rspec_map, {}, sizeof(Lfltr_rspec) } },
    { "lfltr2", { &Dvsl_addrmap_base->lfltr2, new Lfltr_rspec_map, {}, sizeof(Lfltr_rspec) } },
    { "lfltr3", { &Dvsl_addrmap_base->lfltr3, new Lfltr_rspec_map, {}, sizeof(Lfltr_rspec) } },
    { "tm_top", { &Dvsl_addrmap_base->tm_top, new Tm_top_rspec_map, {}, sizeof(Tm_top_rspec) } }
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

struct Comira_regs_map: public RegisterMapper {
  static constexpr PTR_Comira_regs Comira_regs_base=0;
  Comira_regs_map() : RegisterMapper( {
    { "dummy_register", { const_cast<uint32_t(*)>(&Comira_regs_base->dummy_register), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth_regs_eth_mac_ts_offset_ctrl_map: public RegisterMapper {
  static constexpr PTR_Eth_regs_eth_mac_ts_offset_ctrl Eth_regs_eth_mac_ts_offset_ctrl_base=0;
  Eth_regs_eth_mac_ts_offset_ctrl_map() : RegisterMapper( {
    { "eth_mac_ts_offset_ctrl_0_2", { const_cast<uint32_t(*)>(&Eth_regs_eth_mac_ts_offset_ctrl_base->eth_mac_ts_offset_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "eth_mac_ts_offset_ctrl_1_2", { const_cast<uint32_t(*)>(&Eth_regs_eth_mac_ts_offset_ctrl_base->eth_mac_ts_offset_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth_regs_eth_onestep_ets_offset_ctrl_map: public RegisterMapper {
  static constexpr PTR_Eth_regs_eth_onestep_ets_offset_ctrl Eth_regs_eth_onestep_ets_offset_ctrl_base=0;
  Eth_regs_eth_onestep_ets_offset_ctrl_map() : RegisterMapper( {
    { "eth_onestep_ets_offset_ctrl_0_2", { const_cast<uint32_t(*)>(&Eth_regs_eth_onestep_ets_offset_ctrl_base->eth_onestep_ets_offset_ctrl_0_2), 0, {}, sizeof(uint32_t) } },
    { "eth_onestep_ets_offset_ctrl_1_2", { const_cast<uint32_t(*)>(&Eth_regs_eth_onestep_ets_offset_ctrl_base->eth_onestep_ets_offset_ctrl_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Eth_regs_map: public RegisterMapper {
  static constexpr PTR_Eth_regs Eth_regs_base=0;
  Eth_regs_map() : RegisterMapper( {
    { "eth_soft_reset", { const_cast<uint32_t(*)>(&Eth_regs_base->eth_soft_reset), 0, {}, sizeof(uint32_t) } },
    { "eth_sel_altclk", { const_cast<uint32_t(*)>(&Eth_regs_base->eth_sel_altclk), 0, {}, sizeof(uint32_t) } },
    { "ethsds_int_broadcast", { const_cast<uint32_t(*)>(&Eth_regs_base->ethsds_int_broadcast), 0, {}, sizeof(uint32_t) } },
    { "eth_mac_ts_offset_ctrl", { const_cast<Eth_regs_eth_mac_ts_offset_ctrl(*)>(&Eth_regs_base->eth_mac_ts_offset_ctrl), new Eth_regs_eth_mac_ts_offset_ctrl_map, {}, sizeof(Eth_regs_eth_mac_ts_offset_ctrl) } },
    { "ethsds_int_ctrl", { const_cast<uint32_t(*)[0x4]>(&Eth_regs_base->ethsds_int_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "ethsds_int_stat", { const_cast<uint32_t(*)[0x4]>(&Eth_regs_base->ethsds_int_stat), 0, {0x4}, sizeof(uint32_t) } },
    { "ethsds_rxsigok_ctrl", { const_cast<uint32_t(*)[0x4]>(&Eth_regs_base->ethsds_rxsigok_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "ethsds_core_cntl", { const_cast<uint32_t(*)[0x4]>(&Eth_regs_base->ethsds_core_cntl), 0, {0x4}, sizeof(uint32_t) } },
    { "ethsds_status0", { const_cast<uint32_t(*)[0x4]>(&Eth_regs_base->ethsds_status0), 0, {0x4}, sizeof(uint32_t) } },
    { "ethsds_status1", { const_cast<uint32_t(*)[0x4]>(&Eth_regs_base->ethsds_status1), 0, {0x4}, sizeof(uint32_t) } },
    { "eth_onestep_ets_offset_ctrl", { const_cast<Eth_regs_eth_onestep_ets_offset_ctrl(*)[0x4]>(&Eth_regs_base->eth_onestep_ets_offset_ctrl), new Eth_regs_eth_onestep_ets_offset_ctrl_map, {0x4}, sizeof(Eth_regs_eth_onestep_ets_offset_ctrl) } },
    { "int_stat", { const_cast<uint32_t(*)>(&Eth_regs_base->int_stat), 0, {}, sizeof(uint32_t) } },
    { "int_en", { const_cast<uint32_t(*)>(&Eth_regs_base->int_en), 0, {}, sizeof(uint32_t) } },
    { "int_pri", { const_cast<uint32_t(*)>(&Eth_regs_base->int_pri), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Eth_regs_base->int_inj), 0, {}, sizeof(uint32_t) } },
    { "txff_ctrl", { const_cast<uint32_t(*)>(&Eth_regs_base->txff_ctrl), 0, {}, sizeof(uint32_t) } },
    { "txff_pream0", { const_cast<uint32_t(*)>(&Eth_regs_base->txff_pream0), 0, {}, sizeof(uint32_t) } },
    { "txff_pream1", { const_cast<uint32_t(*)>(&Eth_regs_base->txff_pream1), 0, {}, sizeof(uint32_t) } },
    { "eth_txff_err_addr", { const_cast<uint32_t(*)>(&Eth_regs_base->eth_txff_err_addr), 0, {}, sizeof(uint32_t) } },
    { "eth_status", { const_cast<uint32_t(*)>(&Eth_regs_base->eth_status), 0, {}, sizeof(uint32_t) } },
    { "rxpkt_err_sts", { const_cast<uint32_t(*)[0x4]>(&Eth_regs_base->rxpkt_err_sts), 0, {0x4}, sizeof(uint32_t) } },
    { "rxpkt_err_hdr0", { const_cast<uint32_t(*)[0x4]>(&Eth_regs_base->rxpkt_err_hdr0), 0, {0x4}, sizeof(uint32_t) } },
    { "rxpkt_err_hdr1", { const_cast<uint32_t(*)[0x4]>(&Eth_regs_base->rxpkt_err_hdr1), 0, {0x4}, sizeof(uint32_t) } },
    { "rxpkt_err_hdr2", { const_cast<uint32_t(*)[0x4]>(&Eth_regs_base->rxpkt_err_hdr2), 0, {0x4}, sizeof(uint32_t) } },
    { "rxpkt_err_hdr3", { const_cast<uint32_t(*)[0x4]>(&Eth_regs_base->rxpkt_err_hdr3), 0, {0x4}, sizeof(uint32_t) } },
    { "eth_clkobs_ctrl", { const_cast<uint32_t(*)>(&Eth_regs_base->eth_clkobs_ctrl), 0, {}, sizeof(uint32_t) } },
    { "eth_gpio_ring", { const_cast<uint32_t(*)[0x4]>(&Eth_regs_base->eth_gpio_ring), 0, {0x4}, sizeof(uint32_t) } },
    { "pfcff_ctrl", { const_cast<uint32_t(*)[0x10]>(&Eth_regs_base->pfcff_ctrl), 0, {0x10}, sizeof(uint32_t) } },
    { "trunc_ctrl", { const_cast<uint32_t(*)[0x4]>(&Eth_regs_base->trunc_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "port_alive_lut", { const_cast<uint32_t(*)>(&Eth_regs_base->port_alive_lut), 0, {}, sizeof(uint32_t) } },
    { "force_pfc_flush", { const_cast<uint32_t(*)[0x4]>(&Eth_regs_base->force_pfc_flush), 0, {0x4}, sizeof(uint32_t) } },
    { "tx_prbs23", { const_cast<uint32_t(*)>(&Eth_regs_base->tx_prbs23), 0, {}, sizeof(uint32_t) } },
    { "scratch_r", { const_cast<uint32_t(*)>(&Eth_regs_base->scratch_r), 0, {}, sizeof(uint32_t) } },
    { "int_freeze", { const_cast<uint32_t(*)>(&Eth_regs_base->int_freeze), 0, {}, sizeof(uint32_t) } },
    { "ebuf_cred", { const_cast<uint32_t(*)>(&Eth_regs_base->ebuf_cred), 0, {}, sizeof(uint32_t) } },
    { "min_thr", { const_cast<uint32_t(*)>(&Eth_regs_base->min_thr), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Eth_regs_base->dft_csr), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mac_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mac_addrmap Mac_addrmap_base=0;
  Mac_addrmap_map() : RegisterMapper( {
    { "comira_regs", { &Mac_addrmap_base->comira_regs, new Comira_regs_map, {}, sizeof(Comira_regs) } },
    { "eth_regs", { &Mac_addrmap_base->eth_regs, new Eth_regs_map, {}, sizeof(Eth_regs) } }
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
    { "mdio_clkdiv", { const_cast<uint32_t(*)>(&Gpio_pair_regs_base->mdio_clkdiv), 0, {}, sizeof(uint32_t) } }
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
    { "int_stat", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->int_stat), 0, {}, sizeof(uint32_t) } },
    { "int_en", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->int_en), 0, {}, sizeof(uint32_t) } },
    { "int_pri", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->int_pri), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->int_inj), 0, {}, sizeof(uint32_t) } },
    { "shld_ctrl", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->shld_ctrl), 0, {}, sizeof(uint32_t) } },
    { "gpio_status", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->gpio_status), 0, {}, sizeof(uint32_t) } },
    { "refclk_ctrl", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->refclk_ctrl), 0, {}, sizeof(uint32_t) } },
    { "refclk_select", { const_cast<uint32_t(*)[0x4]>(&Gpio_common_regs_base->refclk_select), 0, {0x4}, sizeof(uint32_t) } },
    { "stateout_mask", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->stateout_mask), 0, {}, sizeof(uint32_t) } },
    { "scratch_r", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->scratch_r), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Gpio_common_regs_base->dft_csr), 0, {}, sizeof(uint32_t) } }
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
    { "stash_hashkey_data", { const_cast<uint32_t(*)[0x4]>(&Mau_stash_addrmap_base->stash_hashkey_data), 0, {0x4}, sizeof(uint32_t) } },
    { "stash_bank_enable", { const_cast<uint32_t(*)[0x4]>(&Mau_stash_addrmap_base->stash_bank_enable), 0, {0x4}, sizeof(uint32_t) } },
    { "stash_match_address", { const_cast<uint32_t(*)[0x4]>(&Mau_stash_addrmap_base->stash_match_address), 0, {0x4}, sizeof(uint32_t) } },
    { "stash_version_valid", { const_cast<uint32_t(*)[0x4]>(&Mau_stash_addrmap_base->stash_version_valid), 0, {0x4}, sizeof(uint32_t) } },
    { "stash_data", { const_cast<uint32_t(*)[0x4][0x4]>(&Mau_stash_addrmap_base->stash_data), 0, {0x4,0x4}, sizeof(uint32_t) } },
    { "stash_match_input_data_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_stash_addrmap_base->stash_match_input_data_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "stash_match_result_bus_select", { const_cast<uint32_t(*)[0x2]>(&Mau_stash_addrmap_base->stash_match_result_bus_select), 0, {0x2}, sizeof(uint32_t) } },
    { "stash_match_mask", { const_cast<uint32_t(*)[0x2][0x4]>(&Mau_stash_addrmap_base->stash_match_mask), 0, {0x2,0x4}, sizeof(uint32_t) } }
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

struct Mau_stats_alu_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_stats_alu_addrmap Mau_stats_alu_addrmap_base=0;
  Mau_stats_alu_addrmap_map() : RegisterMapper( {
    { "intr_enable1_mau_stats_alu", { const_cast<uint32_t(*)>(&Mau_stats_alu_addrmap_base->intr_enable1_mau_stats_alu), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_stats_alu", { const_cast<uint32_t(*)>(&Mau_stats_alu_addrmap_base->intr_inject_mau_stats_alu), 0, {}, sizeof(uint32_t) } },
    { "lrt_threshold", { const_cast<uint32_t(*)[0x3]>(&Mau_stats_alu_addrmap_base->lrt_threshold), 0, {0x3}, sizeof(uint32_t) } },
    { "lrt_update_interval", { const_cast<uint32_t(*)[0x3]>(&Mau_stats_alu_addrmap_base->lrt_update_interval), 0, {0x3}, sizeof(uint32_t) } },
    { "statistics_ctl", { const_cast<uint32_t(*)>(&Mau_stats_alu_addrmap_base->statistics_ctl), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_stats_alu", { const_cast<uint32_t(*)>(&Mau_stats_alu_addrmap_base->mau_diag_stats_alu), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_stats_alu", { const_cast<uint32_t(*)>(&Mau_stats_alu_addrmap_base->intr_status_mau_stats_alu), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_stats_alu", { const_cast<uint32_t(*)>(&Mau_stats_alu_addrmap_base->intr_enable0_mau_stats_alu), 0, {}, sizeof(uint32_t) } }
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

struct Mau_meter_alu_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_meter_alu_addrmap Mau_meter_alu_addrmap_base=0;
  Mau_meter_alu_addrmap_map() : RegisterMapper( {
    { "red_value_ctl", { const_cast<uint32_t(*)>(&Mau_meter_alu_addrmap_base->red_value_ctl), 0, {}, sizeof(uint32_t) } },
    { "meter_ctl", { const_cast<uint32_t(*)>(&Mau_meter_alu_addrmap_base->meter_ctl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_selector_alu_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_selector_alu_addrmap Mau_selector_alu_addrmap_base=0;
  Mau_selector_alu_addrmap_map() : RegisterMapper( {
    { "intr_inject_mau_selector_alu", { const_cast<uint32_t(*)>(&Mau_selector_alu_addrmap_base->intr_inject_mau_selector_alu), 0, {}, sizeof(uint32_t) } },
    { "mau_selector_alu_errlog", { const_cast<uint32_t(*)>(&Mau_selector_alu_addrmap_base->mau_selector_alu_errlog), 0, {}, sizeof(uint32_t) } },
    { "selector_alu_ctl", { const_cast<uint32_t(*)>(&Mau_selector_alu_addrmap_base->selector_alu_ctl), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_selector_alu", { const_cast<uint32_t(*)>(&Mau_selector_alu_addrmap_base->intr_status_mau_selector_alu), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_selector_alu", { const_cast<uint32_t(*)>(&Mau_selector_alu_addrmap_base->intr_enable0_mau_selector_alu), 0, {}, sizeof(uint32_t) } },
    { "intr_enable1_mau_selector_alu", { const_cast<uint32_t(*)>(&Mau_selector_alu_addrmap_base->intr_enable1_mau_selector_alu), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_stateful_alu_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_stateful_alu_addrmap Mau_stateful_alu_addrmap_base=0;
  Mau_stateful_alu_addrmap_map() : RegisterMapper( {
    { "salu_const_regfile", { const_cast<uint32_t(*)[0x4]>(&Mau_stateful_alu_addrmap_base->salu_const_regfile), 0, {0x4}, sizeof(uint32_t) } },
    { "salu_mathtable", { const_cast<uint32_t(*)[0x4]>(&Mau_stateful_alu_addrmap_base->salu_mathtable), 0, {0x4}, sizeof(uint32_t) } },
    { "salu_instr_output_alu", { const_cast<uint32_t(*)[0x4]>(&Mau_stateful_alu_addrmap_base->salu_instr_output_alu), 0, {0x4}, sizeof(uint32_t) } },
    { "salu_instr_common", { const_cast<uint32_t(*)[0x4]>(&Mau_stateful_alu_addrmap_base->salu_instr_common), 0, {0x4}, sizeof(uint32_t) } },
    { "salu_instr_state_alu", { const_cast<uint32_t(*)[0x4][0x4]>(&Mau_stateful_alu_addrmap_base->salu_instr_state_alu), 0, {0x4,0x4}, sizeof(uint32_t) } },
    { "mau_diag_meter_alu_group", { const_cast<uint32_t(*)>(&Mau_stateful_alu_addrmap_base->mau_diag_meter_alu_group), 0, {}, sizeof(uint32_t) } },
    { "salu_mathunit_ctl", { const_cast<uint32_t(*)>(&Mau_stateful_alu_addrmap_base->salu_mathunit_ctl), 0, {}, sizeof(uint32_t) } },
    { "stateful_ctl", { const_cast<uint32_t(*)>(&Mau_stateful_alu_addrmap_base->stateful_ctl), 0, {}, sizeof(uint32_t) } },
    { "salu_instr_cmp_alu", { const_cast<uint32_t(*)[0x4][0x2]>(&Mau_stateful_alu_addrmap_base->salu_instr_cmp_alu), 0, {0x4,0x2}, sizeof(uint32_t) } }
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
    { "mapram_mbe_errlog", { const_cast<uint32_t(*)[0x6]>(&Mau_adrmux_row_addrmap_base->mapram_mbe_errlog), 0, {0x6}, sizeof(uint32_t) } },
    { "idletime_logical_to_physical_sweep_grant_ctl", { const_cast<uint32_t(*)[0x6]>(&Mau_adrmux_row_addrmap_base->idletime_logical_to_physical_sweep_grant_ctl), 0, {0x6}, sizeof(uint32_t) } },
    { "idletime_physical_to_logical_req_inc_ctl", { const_cast<uint32_t(*)[0x6]>(&Mau_adrmux_row_addrmap_base->idletime_physical_to_logical_req_inc_ctl), 0, {0x6}, sizeof(uint32_t) } },
    { "idletime_cfg_rd_clear_val", { const_cast<uint32_t(*)[0x6]>(&Mau_adrmux_row_addrmap_base->idletime_cfg_rd_clear_val), 0, {0x6}, sizeof(uint32_t) } },
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
    { "mapram_sbe_errlog", { const_cast<uint32_t(*)[0x6]>(&Mau_adrmux_row_addrmap_base->mapram_sbe_errlog), 0, {0x6}, sizeof(uint32_t) } }
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

struct Mau_map_and_alu_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_map_and_alu_addrmap Mau_map_and_alu_addrmap_base=0;
  Mau_map_and_alu_addrmap_map() : RegisterMapper( {
    { "mapram_color_switchbox", { &Mau_map_and_alu_addrmap_base->mapram_color_switchbox, new Mau_mapram_color_switchbox_addrmap_map, {}, sizeof(Mau_mapram_color_switchbox_addrmap) } },
    { "selector_action_adr_fallback", { const_cast<uint32_t(*)[0x8][0x2]>(&Mau_map_and_alu_addrmap_base->selector_action_adr_fallback), 0, {0x8,0x2}, sizeof(uint32_t) } },
    { "mapram_color_write_switchbox", { &Mau_map_and_alu_addrmap_base->mapram_color_write_switchbox, new Mau_mapram_color_write_switchbox_addrmap_map, {0x4}, sizeof(Mau_mapram_color_write_switchbox_addrmap) } },
    { "mau_selector_action_adr_shift", { const_cast<uint32_t(*)[0x8]>(&Mau_map_and_alu_addrmap_base->mau_selector_action_adr_shift), 0, {0x8}, sizeof(uint32_t) } },
    { "stats_wrap", { &Mau_map_and_alu_addrmap_base->stats_wrap, new Mau_stats_alu_wrap_addrmap_map, {0x4}, sizeof(Mau_stats_alu_wrap_addrmap) } },
    { "intr_mau_decode_memory_core", { const_cast<uint32_t(*)[0x2]>(&Mau_map_and_alu_addrmap_base->intr_mau_decode_memory_core), 0, {0x2}, sizeof(uint32_t) } },
    { "meter_alu_group_data_delay_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_map_and_alu_addrmap_base->meter_alu_group_data_delay_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "meter_alu_group_action_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_map_and_alu_addrmap_base->meter_alu_group_action_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "meter_alu_group_error_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_map_and_alu_addrmap_base->meter_alu_group_error_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "selector_adr_switchbox", { &Mau_map_and_alu_addrmap_base->selector_adr_switchbox, new Mau_selector_action_adr_switchbox_addrmap_map, {}, sizeof(Mau_selector_action_adr_switchbox_addrmap) } },
    { "meter_group", { &Mau_map_and_alu_addrmap_base->meter_group, new Mau_meter_alu_group_addrmap_map, {0x4}, sizeof(Mau_meter_alu_group_addrmap) } },
    { "row", { &Mau_map_and_alu_addrmap_base->row, new Mau_map_and_alu_row_addrmap_map, {0x8}, sizeof(Mau_map_and_alu_row_addrmap) } }
    } )
  {}
};

struct Mau_address_distribution_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_address_distribution_addrmap Mau_address_distribution_addrmap_base=0;
  Mau_address_distribution_addrmap_map() : RegisterMapper( {
    { "idletime_slip_intr_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->idletime_slip_intr_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "movereg_idle_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->movereg_idle_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "adr_dist_idletime_adr_oxbar_ctl", { const_cast<uint32_t(*)[0x5]>(&Mau_address_distribution_addrmap_base->adr_dist_idletime_adr_oxbar_ctl), 0, {0x5}, sizeof(uint32_t) } },
    { "packet_action_at_headertime", { const_cast<uint32_t(*)[0x2][0x4]>(&Mau_address_distribution_addrmap_base->packet_action_at_headertime), 0, {0x2,0x4}, sizeof(uint32_t) } },
    { "deferred_ram_ctl", { const_cast<uint32_t(*)[0x2][0x4]>(&Mau_address_distribution_addrmap_base->deferred_ram_ctl), 0, {0x2,0x4}, sizeof(uint32_t) } },
    { "meter_sweep_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_address_distribution_addrmap_base->meter_sweep_ctl), 0, {0x4}, sizeof(uint32_t) } },
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
    { "meter_sweep_errlog", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->meter_sweep_errlog), 0, {}, sizeof(uint32_t) } },
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
    { "intr_enable1_mau_ad", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->intr_enable1_mau_ad), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_ad", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->intr_inject_mau_ad), 0, {}, sizeof(uint32_t) } },
    { "intr_freeze_enable_mau_ad", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->intr_freeze_enable_mau_ad), 0, {}, sizeof(uint32_t) } },
    { "idletime_slip_errlog", { const_cast<uint32_t(*)>(&Mau_address_distribution_addrmap_base->idletime_slip_errlog), 0, {}, sizeof(uint32_t) } },
    { "immediate_data_8b_ixbar_ctl", { const_cast<uint32_t(*)[0x20]>(&Mau_address_distribution_addrmap_base->immediate_data_8b_ixbar_ctl), 0, {0x20}, sizeof(uint32_t) } },
    { "immediate_data_16b_ixbar_ctl", { const_cast<uint32_t(*)[0x20]>(&Mau_address_distribution_addrmap_base->immediate_data_16b_ixbar_ctl), 0, {0x20}, sizeof(uint32_t) } },
    { "immediate_data_32b_ixbar_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->immediate_data_32b_ixbar_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "meter_color_output_map", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->meter_color_output_map), 0, {0x10}, sizeof(uint32_t) } },
    { "meter_color_logical_to_phys_ixbar_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->meter_color_logical_to_phys_ixbar_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "adr_dist_action_data_adr_icxbar_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->adr_dist_action_data_adr_icxbar_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "adr_dist_stats_adr_icxbar_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->adr_dist_stats_adr_icxbar_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "adr_dist_meter_adr_icxbar_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->adr_dist_meter_adr_icxbar_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "idletime_sweep_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->idletime_sweep_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "idletime_slip", { const_cast<uint32_t(*)[0x10]>(&Mau_address_distribution_addrmap_base->idletime_slip), 0, {0x10}, sizeof(uint32_t) } }
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
    { "mau_idletime_adr_miss_value", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_idletime_adr_miss_value), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_hash_group_xbar_ctl", { const_cast<uint32_t(*)[0x6][0x2]>(&Mau_match_merge_addrmap_base->mau_hash_group_xbar_ctl), 0, {0x6,0x2}, sizeof(uint32_t) } },
    { "stash_hitmap_output_map", { const_cast<uint32_t(*)[0x2][0x8]>(&Mau_match_merge_addrmap_base->stash_hitmap_output_map), 0, {0x2,0x8}, sizeof(uint32_t) } },
    { "stash_next_table_lut", { const_cast<uint32_t(*)[0x2][0x8]>(&Mau_match_merge_addrmap_base->stash_next_table_lut), 0, {0x2,0x8}, sizeof(uint32_t) } },
    { "stash_row_nxtable_bus_drive", { const_cast<uint32_t(*)[0x2][0x8]>(&Mau_match_merge_addrmap_base->stash_row_nxtable_bus_drive), 0, {0x2,0x8}, sizeof(uint32_t) } },
    { "gateway_to_logicaltable_xbar_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->gateway_to_logicaltable_xbar_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "gateway_inhibit_lut", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->gateway_inhibit_lut), 0, {0x10}, sizeof(uint32_t) } },
    { "gateway_to_pbus_xbar_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->gateway_to_pbus_xbar_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "tind_bus_prop", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->tind_bus_prop), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_snapshot_physical_tcam_hit_address", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_snapshot_physical_tcam_hit_address), 0, {0x10}, sizeof(uint32_t) } },
    { "col", { &Mau_match_merge_addrmap_base->col, new Mau_match_merge_col_addrmap_map, {0xc}, sizeof(Mau_match_merge_col_addrmap) } },
    { "mau_snapshot_physical_exact_match_hit_address", { const_cast<uint32_t(*)[0x10]>(&Mau_match_merge_addrmap_base->mau_snapshot_physical_exact_match_hit_address), 0, {0x10}, sizeof(uint32_t) } },
    { "mau_table_counter", { const_cast<uint32_t(*)[0x10][0x1]>(&Mau_match_merge_addrmap_base->mau_table_counter), 0, {0x10,0x1}, sizeof(uint32_t) } },
    { "tcam_hit_to_logical_table_ixbar_outputmap", { const_cast<uint32_t(*)[0x8]>(&Mau_match_merge_addrmap_base->tcam_hit_to_logical_table_ixbar_outputmap), 0, {0x8}, sizeof(uint32_t) } },
    { "mau_hash_group_mask", { const_cast<uint32_t(*)[0x6]>(&Mau_match_merge_addrmap_base->mau_hash_group_mask), 0, {0x6}, sizeof(uint32_t) } },
    { "gateway_payload_tind_pbus", { const_cast<uint32_t(*)[0x8]>(&Mau_match_merge_addrmap_base->gateway_payload_tind_pbus), 0, {0x8}, sizeof(uint32_t) } },
    { "gateway_payload_exact_pbus", { const_cast<uint32_t(*)[0x8]>(&Mau_match_merge_addrmap_base->gateway_payload_exact_pbus), 0, {0x8}, sizeof(uint32_t) } },
    { "tcam_table_prop", { const_cast<uint32_t(*)[0x8]>(&Mau_match_merge_addrmap_base->tcam_table_prop), 0, {0x8}, sizeof(uint32_t) } },
    { "exact_match_delay_thread", { const_cast<uint32_t(*)[0x3]>(&Mau_match_merge_addrmap_base->exact_match_delay_thread), 0, {0x3}, sizeof(uint32_t) } },
    { "logical_table_thread", { const_cast<uint32_t(*)[0x3]>(&Mau_match_merge_addrmap_base->logical_table_thread), 0, {0x3}, sizeof(uint32_t) } },
    { "mau_physical_to_meter_alu_ixbar_map", { const_cast<uint32_t(*)[0x2][0x2]>(&Mau_match_merge_addrmap_base->mau_physical_to_meter_alu_ixbar_map), 0, {0x2,0x2}, sizeof(uint32_t) } },
    { "mau_meter_precolor_hash_map_to_logical_ctl", { const_cast<uint32_t(*)[0x4]>(&Mau_match_merge_addrmap_base->mau_meter_precolor_hash_map_to_logical_ctl), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_selector_action_entry_size", { const_cast<uint32_t(*)[0x4]>(&Mau_match_merge_addrmap_base->mau_selector_action_entry_size), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_stateful_log_counter", { const_cast<uint32_t(*)[0x4][0x1]>(&Mau_match_merge_addrmap_base->mau_stateful_log_counter), 0, {0x4,0x1}, sizeof(uint32_t) } },
    { "mau_stateful_log_ctl_ixbar_map", { const_cast<uint32_t(*)[0x2][0x2]>(&Mau_match_merge_addrmap_base->mau_stateful_log_ctl_ixbar_map), 0, {0x2,0x2}, sizeof(uint32_t) } },
    { "meter_alu_thread", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->meter_alu_thread), 0, {0x2}, sizeof(uint32_t) } },
    { "exact_match_phys_result_delay", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->exact_match_phys_result_delay), 0, {0x2}, sizeof(uint32_t) } },
    { "exact_match_phys_result_en", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->exact_match_phys_result_en), 0, {0x2}, sizeof(uint32_t) } },
    { "exact_match_phys_result_thread", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->exact_match_phys_result_thread), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_action_instruction_adr_map_en", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_action_instruction_adr_map_en), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_mapram_color_map_to_logical_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_mapram_color_map_to_logical_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_hash_group_expand", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_hash_group_expand), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_meter_alu_to_logical_map", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_meter_alu_to_logical_map), 0, {0x2}, sizeof(uint32_t) } },
    { "predication_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->predication_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_snapshot_next_table_out", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_snapshot_next_table_out), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_snapshot_capture_datapath_error", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_snapshot_capture_datapath_error), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_table_counter_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_table_counter_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_stateful_log_counter_ctl", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_stateful_log_counter_ctl), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_stateful_log_vpn_limit", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_stateful_log_vpn_limit), 0, {0x2}, sizeof(uint32_t) } },
    { "mau_immediate_data_exact_shiftcount", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->mau_immediate_data_exact_shiftcount), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "mau_action_instruction_adr_exact_shiftcount", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->mau_action_instruction_adr_exact_shiftcount), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "mau_actiondata_adr_exact_shiftcount", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->mau_actiondata_adr_exact_shiftcount), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "mau_stats_adr_exact_shiftcount", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->mau_stats_adr_exact_shiftcount), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "mau_meter_adr_exact_shiftcount", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->mau_meter_adr_exact_shiftcount), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "mau_idletime_adr_exact_shiftcount", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->mau_idletime_adr_exact_shiftcount), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "gateway_next_table_lut", { const_cast<uint32_t(*)[0x10][0x8]>(&Mau_match_merge_addrmap_base->gateway_next_table_lut), 0, {0x10,0x8}, sizeof(uint32_t) } },
    { "mau_stateful_log_vpn_offset", { const_cast<uint32_t(*)[0x2]>(&Mau_match_merge_addrmap_base->mau_stateful_log_vpn_offset), 0, {0x2}, sizeof(uint32_t) } },
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
    { "next_table_tcam_actionbit_map_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->next_table_tcam_actionbit_map_en), 0, {}, sizeof(uint32_t) } },
    { "mau_immediate_data_tcam_actionbit_map_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_immediate_data_tcam_actionbit_map_en), 0, {}, sizeof(uint32_t) } },
    { "mau_action_instruction_adr_tcam_actionbit_map_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_action_instruction_adr_tcam_actionbit_map_en), 0, {}, sizeof(uint32_t) } },
    { "mau_actiondata_adr_tcam_actionbit_map_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_actiondata_adr_tcam_actionbit_map_en), 0, {}, sizeof(uint32_t) } },
    { "mau_stats_adr_tcam_actionbit_map_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_stats_adr_tcam_actionbit_map_en), 0, {}, sizeof(uint32_t) } },
    { "mau_meter_adr_tcam_actionbit_map_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_meter_adr_tcam_actionbit_map_en), 0, {}, sizeof(uint32_t) } },
    { "mau_match_central_mapram_read_color_oflo_ctl", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_match_central_mapram_read_color_oflo_ctl), 0, {}, sizeof(uint32_t) } },
    { "match_to_logical_table_ixbar_outputmap", { const_cast<uint32_t(*)[0x4][0x10]>(&Mau_match_merge_addrmap_base->match_to_logical_table_ixbar_outputmap), 0, {0x4,0x10}, sizeof(uint32_t) } },
    { "mau_action_instruction_adr_map_data", { const_cast<uint32_t(*)[0x2][0x10][0x2]>(&Mau_match_merge_addrmap_base->mau_action_instruction_adr_map_data), 0, {0x2,0x10,0x2}, sizeof(uint32_t) } },
    { "gateway_payload_data", { const_cast<uint32_t(*)[0x8][0x2][0x2][0x2]>(&Mau_match_merge_addrmap_base->gateway_payload_data), 0, {0x8,0x2,0x2,0x2}, sizeof(uint32_t) } },
    { "mau_idletime_adr_tcam_actionbit_map_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_idletime_adr_tcam_actionbit_map_en), 0, {}, sizeof(uint32_t) } },
    { "mau_selector_hash_sps_enable", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_selector_hash_sps_enable), 0, {}, sizeof(uint32_t) } },
    { "mau_hash_group_config", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_hash_group_config), 0, {}, sizeof(uint32_t) } },
    { "mau_hash_group_shiftcount", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_hash_group_shiftcount), 0, {}, sizeof(uint32_t) } },
    { "mau_logical_to_meter_alu_map", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_logical_to_meter_alu_map), 0, {}, sizeof(uint32_t) } },
    { "mau_meter_precolor_hash_sel", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_meter_precolor_hash_sel), 0, {}, sizeof(uint32_t) } },
    { "gateway_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->gateway_en), 0, {}, sizeof(uint32_t) } },
    { "mau_snapshot_logical_table_hit", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_snapshot_logical_table_hit), 0, {}, sizeof(uint32_t) } },
    { "mau_snapshot_gateway_table_inhibit_logical", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_snapshot_gateway_table_inhibit_logical), 0, {}, sizeof(uint32_t) } },
    { "mau_snapshot_table_active", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_snapshot_table_active), 0, {}, sizeof(uint32_t) } },
    { "mau_table_counter_clear", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_table_counter_clear), 0, {}, sizeof(uint32_t) } },
    { "mau_stateful_log_counter_clear", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_stateful_log_counter_clear), 0, {}, sizeof(uint32_t) } },
    { "mau_stateful_log_vpn_hole_en", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_stateful_log_vpn_hole_en), 0, {}, sizeof(uint32_t) } },
    { "mau_stateful_log_instruction_width", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->mau_stateful_log_instruction_width), 0, {}, sizeof(uint32_t) } },
    { "actiondata_error_ctl", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->actiondata_error_ctl), 0, {}, sizeof(uint32_t) } },
    { "imem_parity_error_ctl", { const_cast<uint32_t(*)>(&Mau_match_merge_addrmap_base->imem_parity_error_ctl), 0, {}, sizeof(uint32_t) } },
    { "next_table_map_data", { const_cast<uint32_t(*)[0x10][0x2]>(&Mau_match_merge_addrmap_base->next_table_map_data), 0, {0x10,0x2}, sizeof(uint32_t) } },
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
    { "mau_meter_adr_mask", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_match_merge_addrmap_base->mau_meter_adr_mask), 0, {0x2,0x10}, sizeof(uint32_t) } }
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

struct Mau_cfg_regs_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_cfg_regs_addrmap Mau_cfg_regs_addrmap_base=0;
  Mau_cfg_regs_addrmap_map() : RegisterMapper( {
    { "mau_diag_pbus_enable", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_diag_pbus_enable), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_valid_ctl", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_diag_valid_ctl), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_cfg_ctl", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_diag_cfg_ctl), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_tcam_hit_xbar_ctl", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_diag_tcam_hit_xbar_ctl), 0, {}, sizeof(uint32_t) } },
    { "pbs_creq_errlog", { const_cast<uint32_t(*)[0x4]>(&Mau_cfg_regs_addrmap_base->pbs_creq_errlog), 0, {0x4}, sizeof(uint32_t) } },
    { "pbs_cresp_errlog", { const_cast<uint32_t(*)[0x4]>(&Mau_cfg_regs_addrmap_base->pbs_cresp_errlog), 0, {0x4}, sizeof(uint32_t) } },
    { "pbs_sreq_errlog", { const_cast<uint32_t(*)[0x4]>(&Mau_cfg_regs_addrmap_base->pbs_sreq_errlog), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_cfg_stats_alu_lt", { const_cast<uint32_t(*)[0x4]>(&Mau_cfg_regs_addrmap_base->mau_cfg_stats_alu_lt), 0, {0x4}, sizeof(uint32_t) } },
    { "mau_cfg_uram_thread", { const_cast<uint32_t(*)[0x3]>(&Mau_cfg_regs_addrmap_base->mau_cfg_uram_thread), 0, {0x3}, sizeof(uint32_t) } },
    { "mau_cfg_mram_thread", { const_cast<uint32_t(*)[0x2]>(&Mau_cfg_regs_addrmap_base->mau_cfg_mram_thread), 0, {0x2}, sizeof(uint32_t) } },
    { "dft_csr_memctrl", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->dft_csr_memctrl), 0, {}, sizeof(uint32_t) } },
    { "tcam_scrub_ctl", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->tcam_scrub_ctl), 0, {}, sizeof(uint32_t) } },
    { "pbs_creq_ecc", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->pbs_creq_ecc), 0, {}, sizeof(uint32_t) } },
    { "pbs_cresp_ecc", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->pbs_cresp_ecc), 0, {}, sizeof(uint32_t) } },
    { "pbs_sreq_ecc", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->pbs_sreq_ecc), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_cfg", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->intr_status_mau_cfg), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_cfg", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->intr_enable0_mau_cfg), 0, {}, sizeof(uint32_t) } },
    { "intr_enable1_mau_cfg", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->intr_enable1_mau_cfg), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_cfg", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->intr_inject_mau_cfg), 0, {}, sizeof(uint32_t) } },
    { "intr_freeze_enable_mau_cfg", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->intr_freeze_enable_mau_cfg), 0, {}, sizeof(uint32_t) } },
    { "intr_decode_top", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->intr_decode_top), 0, {}, sizeof(uint32_t) } },
    { "q_hole_acc_errlog_hi", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->q_hole_acc_errlog_hi), 0, {}, sizeof(uint32_t) } },
    { "q_hole_acc_errlog_lo", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->q_hole_acc_errlog_lo), 0, {}, sizeof(uint32_t) } },
    { "mau_cfg_movereg_tcam_only", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_cfg_movereg_tcam_only), 0, {}, sizeof(uint32_t) } },
    { "mau_cfg_mem_slow_mode", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_cfg_mem_slow_mode), 0, {}, sizeof(uint32_t) } },
    { "mau_cfg_lt_thread", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_cfg_lt_thread), 0, {}, sizeof(uint32_t) } },
    { "mau_cfg_dram_thread", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_cfg_dram_thread), 0, {}, sizeof(uint32_t) } },
    { "mau_cfg_imem_bubble_req", { const_cast<uint32_t(*)>(&Mau_cfg_regs_addrmap_base->mau_cfg_imem_bubble_req), 0, {}, sizeof(uint32_t) } },
    { "stats_dump_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_cfg_regs_addrmap_base->stats_dump_ctl), 0, {0x10}, sizeof(uint32_t) } },
    { "idle_dump_ctl", { const_cast<uint32_t(*)[0x10]>(&Mau_cfg_regs_addrmap_base->idle_dump_ctl), 0, {0x10}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_tcam_column_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_tcam_column_addrmap Mau_tcam_column_addrmap_base=0;
  Mau_tcam_column_addrmap_map() : RegisterMapper( {
    { "tcam_table_map", { const_cast<uint32_t(*)[0x8]>(&Mau_tcam_column_addrmap_base->tcam_table_map), 0, {0x8}, sizeof(uint32_t) } },
    { "tcam_mode", { const_cast<uint32_t(*)[0x10]>(&Mau_tcam_column_addrmap_base->tcam_mode), 0, {0x10}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_tcam_row_vh_xbar_array_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_tcam_row_vh_xbar_array_addrmap Mau_tcam_row_vh_xbar_array_addrmap_base=0;
  Mau_tcam_row_vh_xbar_array_addrmap_map() : RegisterMapper( {
    { "tcam_validbit_xbar_ctl", { const_cast<uint32_t(*)[0x2][0x8][0x8]>(&Mau_tcam_row_vh_xbar_array_addrmap_base->tcam_validbit_xbar_ctl), 0, {0x2,0x8,0x8}, sizeof(uint32_t) } },
    { "tcam_row_halfbyte_mux_ctl", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_tcam_row_vh_xbar_array_addrmap_base->tcam_row_halfbyte_mux_ctl), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "tcam_extra_byte_ctl", { const_cast<uint32_t(*)[0x2][0x8]>(&Mau_tcam_row_vh_xbar_array_addrmap_base->tcam_extra_byte_ctl), 0, {0x2,0x8}, sizeof(uint32_t) } },
    { "tcam_row_output_ctl", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_tcam_row_vh_xbar_array_addrmap_base->tcam_row_output_ctl), 0, {0x2,0x10}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_tcam_array_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_tcam_array_addrmap Mau_tcam_array_addrmap_base=0;
  Mau_tcam_array_addrmap_map() : RegisterMapper( {
    { "col", { &Mau_tcam_array_addrmap_base->col, new Mau_tcam_column_addrmap_map, {0x2}, sizeof(Mau_tcam_column_addrmap) } },
    { "tcam_parity_control", { const_cast<uint32_t(*)[0xc]>(&Mau_tcam_array_addrmap_base->tcam_parity_control), 0, {0xc}, sizeof(uint32_t) } },
    { "tcam_sbe_errlog", { const_cast<uint32_t(*)[0xc]>(&Mau_tcam_array_addrmap_base->tcam_sbe_errlog), 0, {0xc}, sizeof(uint32_t) } },
    { "tcam_match_adr_shift", { const_cast<uint32_t(*)[0x8]>(&Mau_tcam_array_addrmap_base->tcam_match_adr_shift), 0, {0x8}, sizeof(uint32_t) } },
    { "tcam_output_table_thread", { const_cast<uint32_t(*)[0x8]>(&Mau_tcam_array_addrmap_base->tcam_output_table_thread), 0, {0x8}, sizeof(uint32_t) } },
    { "tcam_logical_channel_errlog_lo", { const_cast<uint32_t(*)[0x4]>(&Mau_tcam_array_addrmap_base->tcam_logical_channel_errlog_lo), 0, {0x4}, sizeof(uint32_t) } },
    { "tcam_logical_channel_errlog_hi", { const_cast<uint32_t(*)[0x4]>(&Mau_tcam_array_addrmap_base->tcam_logical_channel_errlog_hi), 0, {0x4}, sizeof(uint32_t) } },
    { "tcam_piped", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->tcam_piped), 0, {}, sizeof(uint32_t) } },
    { "tcam_error_detect_enable", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->tcam_error_detect_enable), 0, {}, sizeof(uint32_t) } },
    { "mau_diag_tcam_clk_en", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->mau_diag_tcam_clk_en), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_tcam_array", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->intr_status_mau_tcam_array), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_tcam_array", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->intr_enable0_mau_tcam_array), 0, {}, sizeof(uint32_t) } },
    { "intr_enable1_mau_tcam_array", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->intr_enable1_mau_tcam_array), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_tcam_array", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->intr_inject_mau_tcam_array), 0, {}, sizeof(uint32_t) } },
    { "intr_freeze_enable_mau_tcam_array", { const_cast<uint32_t(*)>(&Mau_tcam_array_addrmap_base->intr_freeze_enable_mau_tcam_array), 0, {}, sizeof(uint32_t) } },
    { "vh_data_xbar", { &Mau_tcam_array_addrmap_base->vh_data_xbar, new Mau_tcam_row_vh_xbar_array_addrmap_map, {}, sizeof(Mau_tcam_row_vh_xbar_array_addrmap) } }
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

struct Mau_snapshot_capture_half_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_snapshot_capture_half_addrmap Mau_snapshot_capture_half_addrmap_base=0;
  Mau_snapshot_capture_half_addrmap_map() : RegisterMapper( {
    { "mau_snapshot_capture_subword32b_lo", { const_cast<uint32_t(*)[0x20]>(&Mau_snapshot_capture_half_addrmap_base->mau_snapshot_capture_subword32b_lo), 0, {0x20}, sizeof(uint32_t) } },
    { "mau_snapshot_capture_subword32b_hi", { const_cast<uint32_t(*)[0x20]>(&Mau_snapshot_capture_half_addrmap_base->mau_snapshot_capture_subword32b_hi), 0, {0x20}, sizeof(uint32_t) } },
    { "mau_snapshot_capture_subword8b", { const_cast<uint32_t(*)[0x20]>(&Mau_snapshot_capture_half_addrmap_base->mau_snapshot_capture_subword8b), 0, {0x20}, sizeof(uint32_t) } },
    { "mau_snapshot_capture_subword16b", { const_cast<uint32_t(*)[0x30]>(&Mau_snapshot_capture_half_addrmap_base->mau_snapshot_capture_subword16b), 0, {0x30}, sizeof(uint32_t) } }
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

struct Mau_snapshot_datapath_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_snapshot_datapath_addrmap Mau_snapshot_datapath_addrmap_base=0;
  Mau_snapshot_datapath_addrmap_map() : RegisterMapper( {
    { "snapshot_capture", { &Mau_snapshot_datapath_addrmap_base->snapshot_capture, new Mau_snapshot_capture_half_addrmap_map, {0x2}, sizeof(Mau_snapshot_capture_half_addrmap) } },
    { "snapshot_match", { &Mau_snapshot_datapath_addrmap_base->snapshot_match, new Mau_snapshot_match_addrmap_map, {}, sizeof(Mau_snapshot_match_addrmap) } }
    } )
  {}
};

struct Mau_imem_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_imem_addrmap Mau_imem_addrmap_base=0;
  Mau_imem_addrmap_map() : RegisterMapper( {
    { "imem_subword16", { const_cast<uint32_t(*)[0x60][0x20]>(&Mau_imem_addrmap_base->imem_subword16), 0, {0x60,0x20}, sizeof(uint32_t) } },
    { "imem_subword32", { const_cast<uint32_t(*)[0x40][0x20]>(&Mau_imem_addrmap_base->imem_subword32), 0, {0x40,0x20}, sizeof(uint32_t) } },
    { "imem_subword8", { const_cast<uint32_t(*)[0x40][0x20]>(&Mau_imem_addrmap_base->imem_subword8), 0, {0x40,0x20}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mau_datapath_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_datapath_addrmap Mau_datapath_addrmap_base=0;
  Mau_datapath_addrmap_map() : RegisterMapper( {
    { "xbar_hash", { &Mau_datapath_addrmap_base->xbar_hash, new Mau_match_input_xbar_hash_addrmap_map, {}, sizeof(Mau_match_input_xbar_hash_addrmap) } },
    { "phv_ingress_thread_imem", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_datapath_addrmap_base->phv_ingress_thread_imem), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "phv_egress_thread_imem", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_datapath_addrmap_base->phv_egress_thread_imem), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "actionmux_din_power_ctl", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_datapath_addrmap_base->actionmux_din_power_ctl), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "match_input_xbar_din_power_ctl", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_datapath_addrmap_base->match_input_xbar_din_power_ctl), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "intr_freeze_enable_mau_imem", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_freeze_enable_mau_imem), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_snapshot", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_status_mau_snapshot), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_snapshot", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_enable0_mau_snapshot), 0, {}, sizeof(uint32_t) } },
    { "intr_enable1_mau_snapshot", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_enable1_mau_snapshot), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_snapshot", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_inject_mau_snapshot), 0, {}, sizeof(uint32_t) } },
    { "intr_freeze_enable_mau_snapshot", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_freeze_enable_mau_snapshot), 0, {}, sizeof(uint32_t) } },
    { "mau_scratch", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->mau_scratch), 0, {}, sizeof(uint32_t) } },
    { "imem_sbe_errlog", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->imem_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "snapshot_ctl", { &Mau_datapath_addrmap_base->snapshot_ctl, new Mau_snapshot_control_addrmap_map, {}, sizeof(Mau_snapshot_control_addrmap) } },
    { "hashout_ctl", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->hashout_ctl), 0, {}, sizeof(uint32_t) } },
    { "intr_status_mau_gfm_hash", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_status_mau_gfm_hash), 0, {}, sizeof(uint32_t) } },
    { "intr_enable0_mau_gfm_hash", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_enable0_mau_gfm_hash), 0, {}, sizeof(uint32_t) } },
    { "intr_enable1_mau_gfm_hash", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_enable1_mau_gfm_hash), 0, {}, sizeof(uint32_t) } },
    { "intr_inject_mau_gfm_hash", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_inject_mau_gfm_hash), 0, {}, sizeof(uint32_t) } },
    { "intr_freeze_enable_mau_gfm_hash", { const_cast<uint32_t(*)>(&Mau_datapath_addrmap_base->intr_freeze_enable_mau_gfm_hash), 0, {}, sizeof(uint32_t) } },
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
    { "phv_ingress_thread", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_datapath_addrmap_base->phv_ingress_thread), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "phv_egress_thread", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_datapath_addrmap_base->phv_egress_thread), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "phv_ingress_thread_alu", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_datapath_addrmap_base->phv_ingress_thread_alu), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "phv_egress_thread_alu", { const_cast<uint32_t(*)[0x2][0x10]>(&Mau_datapath_addrmap_base->phv_egress_thread_alu), 0, {0x2,0x10}, sizeof(uint32_t) } },
    { "snapshot_dp", { &Mau_datapath_addrmap_base->snapshot_dp, new Mau_snapshot_datapath_addrmap_map, {}, sizeof(Mau_snapshot_datapath_addrmap) } },
    { "imem", { &Mau_datapath_addrmap_base->imem, new Mau_imem_addrmap_map, {}, sizeof(Mau_imem_addrmap) } }
    } )
  {}
};

struct Mau_addrmap_map: public RegisterMapper {
  static constexpr PTR_Mau_addrmap Mau_addrmap_base=0;
  Mau_addrmap_map() : RegisterMapper( {
    { "rams", { &Mau_addrmap_base->rams, new Mau_memory_core_addrmap_map, {}, sizeof(Mau_memory_core_addrmap) } },
    { "cfg_regs", { &Mau_addrmap_base->cfg_regs, new Mau_cfg_regs_addrmap_map, {}, sizeof(Mau_cfg_regs_addrmap) } },
    { "tcams", { &Mau_addrmap_base->tcams, new Mau_tcam_array_addrmap_map, {}, sizeof(Mau_tcam_array_addrmap) } },
    { "dp", { &Mau_addrmap_base->dp, new Mau_datapath_addrmap_map, {}, sizeof(Mau_datapath_addrmap) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_phv_owner_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_phv_owner Prsr_reg_main_rspec_phv_owner_base=0;
  Prsr_reg_main_rspec_phv_owner_map() : RegisterMapper( {
    { "phv_owner_0_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_0_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_1_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_1_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_2_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_2_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_3_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_3_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_4_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_4_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_5_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_5_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_6_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_6_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_7_12", { &Prsr_reg_main_rspec_phv_owner_base->phv_owner_7_12, 0, {}, sizeof(uint32_t) } },
    { "phv_owner_8_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_8_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_9_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_9_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_10_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_10_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_11_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_phv_owner_base->phv_owner_11_12), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_no_multi_wr_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_no_multi_wr Prsr_reg_main_rspec_no_multi_wr_base=0;
  Prsr_reg_main_rspec_no_multi_wr_map() : RegisterMapper( {
    { "no_multi_wr_0_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_0_12), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_1_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_1_12), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_2_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_2_12), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_3_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_3_12), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_4_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_4_12), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_5_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_5_12), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_6_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_6_12), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_7_12", { &Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_7_12, 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_8_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_8_12), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_9_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_9_12), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_10_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_10_12), 0, {}, sizeof(uint32_t) } },
    { "no_multi_wr_11_12", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_no_multi_wr_base->no_multi_wr_11_12), 0, {}, sizeof(uint32_t) } }
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

struct Prsr_reg_main_rspec_pkt_drop_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_pkt_drop_cnt Prsr_reg_main_rspec_pkt_drop_cnt_base=0;
  Prsr_reg_main_rspec_pkt_drop_cnt_map() : RegisterMapper( {
    { "pkt_drop_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_pkt_drop_cnt_base->pkt_drop_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "pkt_drop_cnt_1_2", { &Prsr_reg_main_rspec_pkt_drop_cnt_base->pkt_drop_cnt_1_2, 0, {}, sizeof(uint32_t) } }
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

struct Prsr_reg_main_rspec_dst_cont_err_cnt_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_dst_cont_err_cnt Prsr_reg_main_rspec_dst_cont_err_cnt_base=0;
  Prsr_reg_main_rspec_dst_cont_err_cnt_map() : RegisterMapper( {
    { "dst_cont_err_cnt_0_2", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_dst_cont_err_cnt_base->dst_cont_err_cnt_0_2), 0, {}, sizeof(uint32_t) } },
    { "dst_cont_err_cnt_1_2", { &Prsr_reg_main_rspec_dst_cont_err_cnt_base->dst_cont_err_cnt_1_2, 0, {}, sizeof(uint32_t) } }
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

struct Prsr_reg_main_rspec_intr_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec_intr Prsr_reg_main_rspec_intr_base=0;
  Prsr_reg_main_rspec_intr_map() : RegisterMapper( {
    { "status", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_main_rspec_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_main_rspec Prsr_reg_main_rspec_base=0;
  Prsr_reg_main_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "mode", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->mode), 0, {}, sizeof(uint32_t) } },
    { "enable", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->enable), 0, {}, sizeof(uint32_t) } },
    { "start_state", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->start_state), 0, {}, sizeof(uint32_t) } },
    { "max_iter", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->max_iter), 0, {}, sizeof(uint32_t) } },
    { "max_cycle", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->max_cycle), 0, {}, sizeof(uint32_t) } },
    { "pri_start", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->pri_start), 0, {}, sizeof(uint32_t) } },
    { "pri_thresh", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->pri_thresh), 0, {}, sizeof(uint32_t) } },
    { "a_emp_thresh", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->a_emp_thresh), 0, {}, sizeof(uint32_t) } },
    { "hdr_len_adj", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->hdr_len_adj), 0, {}, sizeof(uint32_t) } },
    { "phv_owner", { const_cast<Prsr_reg_main_rspec_phv_owner(*)>(&Prsr_reg_main_rspec_base->phv_owner), new Prsr_reg_main_rspec_phv_owner_map, {}, sizeof(Prsr_reg_main_rspec_phv_owner) } },
    { "no_multi_wr", { const_cast<Prsr_reg_main_rspec_no_multi_wr(*)>(&Prsr_reg_main_rspec_base->no_multi_wr), new Prsr_reg_main_rspec_no_multi_wr_map, {}, sizeof(Prsr_reg_main_rspec_no_multi_wr) } },
    { "err_phv_cfg", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->err_phv_cfg), 0, {}, sizeof(uint32_t) } },
    { "hdr_byte_cnt", { const_cast<Prsr_reg_main_rspec_hdr_byte_cnt(*)[0x4]>(&Prsr_reg_main_rspec_base->hdr_byte_cnt), new Prsr_reg_main_rspec_hdr_byte_cnt_map, {0x4}, sizeof(Prsr_reg_main_rspec_hdr_byte_cnt) } },
    { "idle_cnt", { const_cast<Prsr_reg_main_rspec_idle_cnt(*)[0x4]>(&Prsr_reg_main_rspec_base->idle_cnt), new Prsr_reg_main_rspec_idle_cnt_map, {0x4}, sizeof(Prsr_reg_main_rspec_idle_cnt) } },
    { "pkt_drop_cnt", { const_cast<Prsr_reg_main_rspec_pkt_drop_cnt(*)[0x4]>(&Prsr_reg_main_rspec_base->pkt_drop_cnt), new Prsr_reg_main_rspec_pkt_drop_cnt_map, {0x4}, sizeof(Prsr_reg_main_rspec_pkt_drop_cnt) } },
    { "op_fifo_full_cnt", { const_cast<Prsr_reg_main_rspec_op_fifo_full_cnt(*)>(&Prsr_reg_main_rspec_base->op_fifo_full_cnt), new Prsr_reg_main_rspec_op_fifo_full_cnt_map, {}, sizeof(Prsr_reg_main_rspec_op_fifo_full_cnt) } },
    { "op_fifo_full_stall_cnt", { const_cast<Prsr_reg_main_rspec_op_fifo_full_stall_cnt(*)>(&Prsr_reg_main_rspec_base->op_fifo_full_stall_cnt), new Prsr_reg_main_rspec_op_fifo_full_stall_cnt_map, {}, sizeof(Prsr_reg_main_rspec_op_fifo_full_stall_cnt) } },
    { "no_tcam_match_err_cnt", { const_cast<Prsr_reg_main_rspec_no_tcam_match_err_cnt(*)>(&Prsr_reg_main_rspec_base->no_tcam_match_err_cnt), new Prsr_reg_main_rspec_no_tcam_match_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_no_tcam_match_err_cnt) } },
    { "partial_hdr_err_cnt", { const_cast<Prsr_reg_main_rspec_partial_hdr_err_cnt(*)>(&Prsr_reg_main_rspec_base->partial_hdr_err_cnt), new Prsr_reg_main_rspec_partial_hdr_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_partial_hdr_err_cnt) } },
    { "ctr_range_err_cnt", { const_cast<Prsr_reg_main_rspec_ctr_range_err_cnt(*)>(&Prsr_reg_main_rspec_base->ctr_range_err_cnt), new Prsr_reg_main_rspec_ctr_range_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_ctr_range_err_cnt) } },
    { "timeout_iter_err_cnt", { const_cast<Prsr_reg_main_rspec_timeout_iter_err_cnt(*)>(&Prsr_reg_main_rspec_base->timeout_iter_err_cnt), new Prsr_reg_main_rspec_timeout_iter_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_timeout_iter_err_cnt) } },
    { "timeout_cycle_err_cnt", { const_cast<Prsr_reg_main_rspec_timeout_cycle_err_cnt(*)>(&Prsr_reg_main_rspec_base->timeout_cycle_err_cnt), new Prsr_reg_main_rspec_timeout_cycle_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_timeout_cycle_err_cnt) } },
    { "src_ext_err_cnt", { const_cast<Prsr_reg_main_rspec_src_ext_err_cnt(*)>(&Prsr_reg_main_rspec_base->src_ext_err_cnt), new Prsr_reg_main_rspec_src_ext_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_src_ext_err_cnt) } },
    { "dst_cont_err_cnt", { const_cast<Prsr_reg_main_rspec_dst_cont_err_cnt(*)>(&Prsr_reg_main_rspec_base->dst_cont_err_cnt), new Prsr_reg_main_rspec_dst_cont_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_dst_cont_err_cnt) } },
    { "phv_owner_err_cnt", { const_cast<Prsr_reg_main_rspec_phv_owner_err_cnt(*)>(&Prsr_reg_main_rspec_base->phv_owner_err_cnt), new Prsr_reg_main_rspec_phv_owner_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_phv_owner_err_cnt) } },
    { "multi_wr_err_cnt", { const_cast<Prsr_reg_main_rspec_multi_wr_err_cnt(*)>(&Prsr_reg_main_rspec_base->multi_wr_err_cnt), new Prsr_reg_main_rspec_multi_wr_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_multi_wr_err_cnt) } },
    { "aram_sbe_cnt", { const_cast<Prsr_reg_main_rspec_aram_sbe_cnt(*)>(&Prsr_reg_main_rspec_base->aram_sbe_cnt), new Prsr_reg_main_rspec_aram_sbe_cnt_map, {}, sizeof(Prsr_reg_main_rspec_aram_sbe_cnt) } },
    { "aram_mbe_cnt", { const_cast<Prsr_reg_main_rspec_aram_mbe_cnt(*)>(&Prsr_reg_main_rspec_base->aram_mbe_cnt), new Prsr_reg_main_rspec_aram_mbe_cnt_map, {}, sizeof(Prsr_reg_main_rspec_aram_mbe_cnt) } },
    { "fcs_err_cnt", { const_cast<Prsr_reg_main_rspec_fcs_err_cnt(*)>(&Prsr_reg_main_rspec_base->fcs_err_cnt), new Prsr_reg_main_rspec_fcs_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_fcs_err_cnt) } },
    { "csum_err_cnt", { const_cast<Prsr_reg_main_rspec_csum_err_cnt(*)>(&Prsr_reg_main_rspec_base->csum_err_cnt), new Prsr_reg_main_rspec_csum_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_csum_err_cnt) } },
    { "tcam_par_err_cnt", { const_cast<Prsr_reg_main_rspec_tcam_par_err_cnt(*)>(&Prsr_reg_main_rspec_base->tcam_par_err_cnt), new Prsr_reg_main_rspec_tcam_par_err_cnt_map, {}, sizeof(Prsr_reg_main_rspec_tcam_par_err_cnt) } },
    { "intr", { &Prsr_reg_main_rspec_base->intr, new Prsr_reg_main_rspec_intr_map, {}, sizeof(Prsr_reg_main_rspec_intr) } },
    { "no_tcam_match_errlog", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->no_tcam_match_errlog), 0, {}, sizeof(uint32_t) } },
    { "partial_hdr_errlog", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->partial_hdr_errlog), 0, {}, sizeof(uint32_t) } },
    { "ctr_range_errlog", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->ctr_range_errlog), 0, {}, sizeof(uint32_t) } },
    { "src_ext_errlog", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->src_ext_errlog), 0, {}, sizeof(uint32_t) } },
    { "dst_cont_errlog", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->dst_cont_errlog), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_errlog", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->phv_owner_errlog), 0, {}, sizeof(uint32_t) } },
    { "multi_wr_errlog", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->multi_wr_errlog), 0, {}, sizeof(uint32_t) } },
    { "aram_sbe_errlog", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->aram_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "aram_mbe_errlog", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->aram_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "tcam_par_errlog", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->tcam_par_errlog), 0, {}, sizeof(uint32_t) } },
    { "ibuf_oflow_errlog", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->ibuf_oflow_errlog), 0, {}, sizeof(uint32_t) } },
    { "ibuf_uflow_errlog", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->ibuf_uflow_errlog), 0, {}, sizeof(uint32_t) } },
    { "op_fifo_oflow_errlog", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->op_fifo_oflow_errlog), 0, {}, sizeof(uint32_t) } },
    { "op_fifo_uflow_errlog", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->op_fifo_uflow_errlog), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->ecc), 0, {}, sizeof(uint32_t) } },
    { "parity", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->parity), 0, {}, sizeof(uint32_t) } },
    { "debug_ctrl", { const_cast<uint32_t(*)>(&Prsr_reg_main_rspec_base->debug_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Glb_group_map: public RegisterMapper {
  static constexpr PTR_Glb_group Glb_group_base=0;
  Glb_group_map() : RegisterMapper( {
    { "glb_ctrl", { const_cast<uint32_t(*)>(&Glb_group_base->glb_ctrl), 0, {}, sizeof(uint32_t) } },
    { "glb_parser_maxbyte", { const_cast<uint32_t(*)>(&Glb_group_base->glb_parser_maxbyte), 0, {}, sizeof(uint32_t) } },
    { "glb_meta_avail", { const_cast<uint32_t(*)>(&Glb_group_base->glb_meta_avail), 0, {}, sizeof(uint32_t) } },
    { "bank_watermark_afull", { const_cast<uint32_t(*)>(&Glb_group_base->bank_watermark_afull), 0, {}, sizeof(uint32_t) } },
    { "bank_watermark_tx_xoff", { const_cast<uint32_t(*)>(&Glb_group_base->bank_watermark_tx_xoff), 0, {}, sizeof(uint32_t) } },
    { "bank_watermark_drop", { const_cast<uint32_t(*)>(&Glb_group_base->bank_watermark_drop), 0, {}, sizeof(uint32_t) } },
    { "int_stat", { const_cast<uint32_t(*)>(&Glb_group_base->int_stat), 0, {}, sizeof(uint32_t) } },
    { "int_en", { const_cast<uint32_t(*)>(&Glb_group_base->int_en), 0, {}, sizeof(uint32_t) } },
    { "int_pri", { const_cast<uint32_t(*)>(&Glb_group_base->int_pri), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Glb_group_base->int_inj), 0, {}, sizeof(uint32_t) } },
    { "glb_err_addr0", { const_cast<uint32_t(*)>(&Glb_group_base->glb_err_addr0), 0, {}, sizeof(uint32_t) } },
    { "glb_err_addr1", { const_cast<uint32_t(*)>(&Glb_group_base->glb_err_addr1), 0, {}, sizeof(uint32_t) } },
    { "glb_err_addr2", { const_cast<uint32_t(*)>(&Glb_group_base->glb_err_addr2), 0, {}, sizeof(uint32_t) } },
    { "indmfree_cnt", { const_cast<uint32_t(*)>(&Glb_group_base->indmfree_cnt), 0, {}, sizeof(uint32_t) } },
    { "indmfree_reg", { const_cast<uint32_t(*)[0x10]>(&Glb_group_base->indmfree_reg), 0, {0x10}, sizeof(uint32_t) } },
    { "freelist_cnt", { const_cast<uint32_t(*)[0x4]>(&Glb_group_base->freelist_cnt), 0, {0x4}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Glb_group_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "tim_off", { const_cast<uint32_t(*)>(&Glb_group_base->tim_off), 0, {}, sizeof(uint32_t) } },
    { "scratch", { const_cast<uint32_t(*)>(&Glb_group_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "int_freeze", { const_cast<uint32_t(*)>(&Glb_group_base->int_freeze), 0, {}, sizeof(uint32_t) } },
    { "dbg_ctrl", { const_cast<uint32_t(*)>(&Glb_group_base->dbg_ctrl), 0, {}, sizeof(uint32_t) } },
    { "cred_ctrl", { const_cast<uint32_t(*)>(&Glb_group_base->cred_ctrl), 0, {}, sizeof(uint32_t) } },
    { "cred_cur", { const_cast<uint32_t(*)>(&Glb_group_base->cred_cur), 0, {}, sizeof(uint32_t) } },
    { "freelist_reg", { const_cast<uint32_t(*)[0x40]>(&Glb_group_base->freelist_reg), 0, {0x40}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_metadata_fix_map: public RegisterMapper {
  static constexpr PTR_Chnl_metadata_fix Chnl_metadata_fix_base=0;
  Chnl_metadata_fix_map() : RegisterMapper( {
    { "chnl_metadata_fix_0_2", { const_cast<uint32_t(*)>(&Chnl_metadata_fix_base->chnl_metadata_fix_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_metadata_fix_1_2", { const_cast<uint32_t(*)>(&Chnl_metadata_fix_base->chnl_metadata_fix_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_parser_send_pkt_map: public RegisterMapper {
  static constexpr PTR_Chnl_parser_send_pkt Chnl_parser_send_pkt_base=0;
  Chnl_parser_send_pkt_map() : RegisterMapper( {
    { "chnl_parser_send_pkt_0_2", { const_cast<uint32_t(*)>(&Chnl_parser_send_pkt_base->chnl_parser_send_pkt_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_parser_send_pkt_1_2", { const_cast<uint32_t(*)>(&Chnl_parser_send_pkt_base->chnl_parser_send_pkt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_deparser_send_pkt_map: public RegisterMapper {
  static constexpr PTR_Chnl_deparser_send_pkt Chnl_deparser_send_pkt_base=0;
  Chnl_deparser_send_pkt_map() : RegisterMapper( {
    { "chnl_deparser_send_pkt_0_2", { const_cast<uint32_t(*)>(&Chnl_deparser_send_pkt_base->chnl_deparser_send_pkt_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_send_pkt_1_2", { const_cast<uint32_t(*)>(&Chnl_deparser_send_pkt_base->chnl_deparser_send_pkt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_macs_received_pkt_map: public RegisterMapper {
  static constexpr PTR_Chnl_macs_received_pkt Chnl_macs_received_pkt_base=0;
  Chnl_macs_received_pkt_map() : RegisterMapper( {
    { "chnl_macs_received_pkt_0_2", { const_cast<uint32_t(*)>(&Chnl_macs_received_pkt_base->chnl_macs_received_pkt_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_macs_received_pkt_1_2", { const_cast<uint32_t(*)>(&Chnl_macs_received_pkt_base->chnl_macs_received_pkt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Chnl_recirc_received_pkt_map: public RegisterMapper {
  static constexpr PTR_Chnl_recirc_received_pkt Chnl_recirc_received_pkt_base=0;
  Chnl_recirc_received_pkt_map() : RegisterMapper( {
    { "chnl_recirc_received_pkt_0_2", { const_cast<uint32_t(*)>(&Chnl_recirc_received_pkt_base->chnl_recirc_received_pkt_0_2), 0, {}, sizeof(uint32_t) } },
    { "chnl_recirc_received_pkt_1_2", { const_cast<uint32_t(*)>(&Chnl_recirc_received_pkt_base->chnl_recirc_received_pkt_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ing_buf_regs_chan0_group_map: public RegisterMapper {
  static constexpr PTR_Ing_buf_regs_chan0_group Ing_buf_regs_chan0_group_base=0;
  Ing_buf_regs_chan0_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan0_group_base->chnl_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_afull", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan0_group_base->chnl_afull), 0, {}, sizeof(uint32_t) } },
    { "chnl_tx_xoff", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan0_group_base->chnl_tx_xoff), 0, {}, sizeof(uint32_t) } },
    { "chnl_drop", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan0_group_base->chnl_drop), 0, {}, sizeof(uint32_t) } },
    { "chnl_metadata_fix", { const_cast<Chnl_metadata_fix(*)>(&Ing_buf_regs_chan0_group_base->chnl_metadata_fix), new Chnl_metadata_fix_map, {}, sizeof(Chnl_metadata_fix) } },
    { "chnl_metadata_fix2", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan0_group_base->chnl_metadata_fix2), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan0_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan0_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_ptr_fifo_min_max", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan0_group_base->chnl_ptr_fifo_min_max), 0, {}, sizeof(uint32_t) } },
    { "chnl_recirc_fifo_min_max", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan0_group_base->chnl_recirc_fifo_min_max), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_drop_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan0_group_base->chnl_deparser_drop_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_wsch_discard_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan0_group_base->chnl_wsch_discard_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_wsch_trunc_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan0_group_base->chnl_wsch_trunc_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_recirc_discard_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan0_group_base->chnl_recirc_discard_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_parser_discard_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan0_group_base->chnl_parser_discard_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_parser_send_pkt", { const_cast<Chnl_parser_send_pkt(*)>(&Ing_buf_regs_chan0_group_base->chnl_parser_send_pkt), new Chnl_parser_send_pkt_map, {}, sizeof(Chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Chnl_deparser_send_pkt(*)>(&Ing_buf_regs_chan0_group_base->chnl_deparser_send_pkt), new Chnl_deparser_send_pkt_map, {}, sizeof(Chnl_deparser_send_pkt) } },
    { "chnl_macs_received_pkt", { const_cast<Chnl_macs_received_pkt(*)>(&Ing_buf_regs_chan0_group_base->chnl_macs_received_pkt), new Chnl_macs_received_pkt_map, {}, sizeof(Chnl_macs_received_pkt) } },
    { "chnl_recirc_received_pkt", { const_cast<Chnl_recirc_received_pkt(*)>(&Ing_buf_regs_chan0_group_base->chnl_recirc_received_pkt), new Chnl_recirc_received_pkt_map, {}, sizeof(Chnl_recirc_received_pkt) } }
    } )
  {}
};

struct Ing_buf_regs_chan1_group_map: public RegisterMapper {
  static constexpr PTR_Ing_buf_regs_chan1_group Ing_buf_regs_chan1_group_base=0;
  Ing_buf_regs_chan1_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan1_group_base->chnl_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_afull", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan1_group_base->chnl_afull), 0, {}, sizeof(uint32_t) } },
    { "chnl_tx_xoff", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan1_group_base->chnl_tx_xoff), 0, {}, sizeof(uint32_t) } },
    { "chnl_drop", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan1_group_base->chnl_drop), 0, {}, sizeof(uint32_t) } },
    { "chnl_metadata_fix", { const_cast<Chnl_metadata_fix(*)>(&Ing_buf_regs_chan1_group_base->chnl_metadata_fix), new Chnl_metadata_fix_map, {}, sizeof(Chnl_metadata_fix) } },
    { "chnl_metadata_fix2", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan1_group_base->chnl_metadata_fix2), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan1_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan1_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_ptr_fifo_min_max", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan1_group_base->chnl_ptr_fifo_min_max), 0, {}, sizeof(uint32_t) } },
    { "chnl_recirc_fifo_min_max", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan1_group_base->chnl_recirc_fifo_min_max), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_drop_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan1_group_base->chnl_deparser_drop_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_wsch_discard_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan1_group_base->chnl_wsch_discard_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_wsch_trunc_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan1_group_base->chnl_wsch_trunc_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_recirc_discard_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan1_group_base->chnl_recirc_discard_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_parser_discard_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan1_group_base->chnl_parser_discard_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_parser_send_pkt", { const_cast<Chnl_parser_send_pkt(*)>(&Ing_buf_regs_chan1_group_base->chnl_parser_send_pkt), new Chnl_parser_send_pkt_map, {}, sizeof(Chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Chnl_deparser_send_pkt(*)>(&Ing_buf_regs_chan1_group_base->chnl_deparser_send_pkt), new Chnl_deparser_send_pkt_map, {}, sizeof(Chnl_deparser_send_pkt) } },
    { "chnl_macs_received_pkt", { const_cast<Chnl_macs_received_pkt(*)>(&Ing_buf_regs_chan1_group_base->chnl_macs_received_pkt), new Chnl_macs_received_pkt_map, {}, sizeof(Chnl_macs_received_pkt) } },
    { "chnl_recirc_received_pkt", { const_cast<Chnl_recirc_received_pkt(*)>(&Ing_buf_regs_chan1_group_base->chnl_recirc_received_pkt), new Chnl_recirc_received_pkt_map, {}, sizeof(Chnl_recirc_received_pkt) } }
    } )
  {}
};

struct Ing_buf_regs_chan2_group_map: public RegisterMapper {
  static constexpr PTR_Ing_buf_regs_chan2_group Ing_buf_regs_chan2_group_base=0;
  Ing_buf_regs_chan2_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan2_group_base->chnl_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_afull", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan2_group_base->chnl_afull), 0, {}, sizeof(uint32_t) } },
    { "chnl_tx_xoff", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan2_group_base->chnl_tx_xoff), 0, {}, sizeof(uint32_t) } },
    { "chnl_drop", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan2_group_base->chnl_drop), 0, {}, sizeof(uint32_t) } },
    { "chnl_metadata_fix", { const_cast<Chnl_metadata_fix(*)>(&Ing_buf_regs_chan2_group_base->chnl_metadata_fix), new Chnl_metadata_fix_map, {}, sizeof(Chnl_metadata_fix) } },
    { "chnl_metadata_fix2", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan2_group_base->chnl_metadata_fix2), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan2_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan2_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_ptr_fifo_min_max", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan2_group_base->chnl_ptr_fifo_min_max), 0, {}, sizeof(uint32_t) } },
    { "chnl_recirc_fifo_min_max", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan2_group_base->chnl_recirc_fifo_min_max), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_drop_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan2_group_base->chnl_deparser_drop_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_wsch_discard_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan2_group_base->chnl_wsch_discard_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_wsch_trunc_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan2_group_base->chnl_wsch_trunc_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_recirc_discard_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan2_group_base->chnl_recirc_discard_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_parser_discard_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan2_group_base->chnl_parser_discard_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_parser_send_pkt", { const_cast<Chnl_parser_send_pkt(*)>(&Ing_buf_regs_chan2_group_base->chnl_parser_send_pkt), new Chnl_parser_send_pkt_map, {}, sizeof(Chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Chnl_deparser_send_pkt(*)>(&Ing_buf_regs_chan2_group_base->chnl_deparser_send_pkt), new Chnl_deparser_send_pkt_map, {}, sizeof(Chnl_deparser_send_pkt) } },
    { "chnl_macs_received_pkt", { const_cast<Chnl_macs_received_pkt(*)>(&Ing_buf_regs_chan2_group_base->chnl_macs_received_pkt), new Chnl_macs_received_pkt_map, {}, sizeof(Chnl_macs_received_pkt) } },
    { "chnl_recirc_received_pkt", { const_cast<Chnl_recirc_received_pkt(*)>(&Ing_buf_regs_chan2_group_base->chnl_recirc_received_pkt), new Chnl_recirc_received_pkt_map, {}, sizeof(Chnl_recirc_received_pkt) } }
    } )
  {}
};

struct Ing_buf_regs_chan3_group_map: public RegisterMapper {
  static constexpr PTR_Ing_buf_regs_chan3_group Ing_buf_regs_chan3_group_base=0;
  Ing_buf_regs_chan3_group_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan3_group_base->chnl_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_afull", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan3_group_base->chnl_afull), 0, {}, sizeof(uint32_t) } },
    { "chnl_tx_xoff", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan3_group_base->chnl_tx_xoff), 0, {}, sizeof(uint32_t) } },
    { "chnl_drop", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan3_group_base->chnl_drop), 0, {}, sizeof(uint32_t) } },
    { "chnl_metadata_fix", { const_cast<Chnl_metadata_fix(*)>(&Ing_buf_regs_chan3_group_base->chnl_metadata_fix), new Chnl_metadata_fix_map, {}, sizeof(Chnl_metadata_fix) } },
    { "chnl_metadata_fix2", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan3_group_base->chnl_metadata_fix2), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum0", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan3_group_base->chnl_pktnum0), 0, {}, sizeof(uint32_t) } },
    { "chnl_pktnum1", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan3_group_base->chnl_pktnum1), 0, {}, sizeof(uint32_t) } },
    { "chnl_ptr_fifo_min_max", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan3_group_base->chnl_ptr_fifo_min_max), 0, {}, sizeof(uint32_t) } },
    { "chnl_recirc_fifo_min_max", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan3_group_base->chnl_recirc_fifo_min_max), 0, {}, sizeof(uint32_t) } },
    { "chnl_deparser_drop_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan3_group_base->chnl_deparser_drop_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_wsch_discard_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan3_group_base->chnl_wsch_discard_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_wsch_trunc_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan3_group_base->chnl_wsch_trunc_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_recirc_discard_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan3_group_base->chnl_recirc_discard_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_parser_discard_pkt", { const_cast<uint32_t(*)>(&Ing_buf_regs_chan3_group_base->chnl_parser_discard_pkt), 0, {}, sizeof(uint32_t) } },
    { "chnl_parser_send_pkt", { const_cast<Chnl_parser_send_pkt(*)>(&Ing_buf_regs_chan3_group_base->chnl_parser_send_pkt), new Chnl_parser_send_pkt_map, {}, sizeof(Chnl_parser_send_pkt) } },
    { "chnl_deparser_send_pkt", { const_cast<Chnl_deparser_send_pkt(*)>(&Ing_buf_regs_chan3_group_base->chnl_deparser_send_pkt), new Chnl_deparser_send_pkt_map, {}, sizeof(Chnl_deparser_send_pkt) } },
    { "chnl_macs_received_pkt", { const_cast<Chnl_macs_received_pkt(*)>(&Ing_buf_regs_chan3_group_base->chnl_macs_received_pkt), new Chnl_macs_received_pkt_map, {}, sizeof(Chnl_macs_received_pkt) } },
    { "chnl_recirc_received_pkt", { const_cast<Chnl_recirc_received_pkt(*)>(&Ing_buf_regs_chan3_group_base->chnl_recirc_received_pkt), new Chnl_recirc_received_pkt_map, {}, sizeof(Chnl_recirc_received_pkt) } }
    } )
  {}
};

struct Ing_buf_regs_map: public RegisterMapper {
  static constexpr PTR_Ing_buf_regs Ing_buf_regs_base=0;
  Ing_buf_regs_map() : RegisterMapper( {
    { "glb_group", { &Ing_buf_regs_base->glb_group, new Glb_group_map, {}, sizeof(Glb_group) } },
    { "chan0_group", { &Ing_buf_regs_base->chan0_group, new Ing_buf_regs_chan0_group_map, {}, sizeof(Ing_buf_regs_chan0_group) } },
    { "chan1_group", { &Ing_buf_regs_base->chan1_group, new Ing_buf_regs_chan1_group_map, {}, sizeof(Ing_buf_regs_chan1_group) } },
    { "chan2_group", { &Ing_buf_regs_base->chan2_group, new Ing_buf_regs_chan2_group_map, {}, sizeof(Ing_buf_regs_chan2_group) } },
    { "chan3_group", { &Ing_buf_regs_base->chan3_group, new Ing_buf_regs_chan3_group_map, {}, sizeof(Ing_buf_regs_chan3_group) } }
    } )
  {}
};

struct Ibp_rspec_map: public RegisterMapper {
  static constexpr PTR_Ibp_rspec Ibp_rspec_base=0;
  Ibp_rspec_map() : RegisterMapper( {
    { "prsr_reg", { &Ibp_rspec_base->prsr_reg, new Prsr_reg_main_rspec_map, {}, sizeof(Prsr_reg_main_rspec) } },
    { "ing_buf_regs", { &Ibp_rspec_base->ing_buf_regs, new Ing_buf_regs_map, {}, sizeof(Ing_buf_regs) } }
    } )
  {}
};

struct Ibp18_rspec_map: public RegisterMapper {
  static constexpr PTR_Ibp18_rspec Ibp18_rspec_base=0;
  Ibp18_rspec_map() : RegisterMapper( {
    { "ibp_reg", { &Ibp18_rspec_base->ibp_reg, new Ibp_rspec_map, {0x12}, sizeof(Ibp_rspec) } }
    } )
  {}
};

struct Parb_regs_hipri_phv_count_map: public RegisterMapper {
  static constexpr PTR_Parb_regs_hipri_phv_count Parb_regs_hipri_phv_count_base=0;
  Parb_regs_hipri_phv_count_map() : RegisterMapper( {
    { "i_hipri_phv_count_0_2", { const_cast<uint32_t(*)>(&Parb_regs_hipri_phv_count_base->i_hipri_phv_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_hipri_phv_count_1_2", { const_cast<uint32_t(*)>(&Parb_regs_hipri_phv_count_base->i_hipri_phv_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_regs_cong_phv_count_map: public RegisterMapper {
  static constexpr PTR_Parb_regs_cong_phv_count Parb_regs_cong_phv_count_base=0;
  Parb_regs_cong_phv_count_map() : RegisterMapper( {
    { "i_cong_phv_count_0_2", { const_cast<uint32_t(*)>(&Parb_regs_cong_phv_count_base->i_cong_phv_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_cong_phv_count_1_2", { const_cast<uint32_t(*)>(&Parb_regs_cong_phv_count_base->i_cong_phv_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_regs_pfc0_phv_count_map: public RegisterMapper {
  static constexpr PTR_Parb_regs_pfc0_phv_count Parb_regs_pfc0_phv_count_base=0;
  Parb_regs_pfc0_phv_count_map() : RegisterMapper( {
    { "i_pfc0_phv_count_0_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc0_phv_count_base->i_pfc0_phv_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_pfc0_phv_count_1_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc0_phv_count_base->i_pfc0_phv_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_regs_pfc1_phv_count_map: public RegisterMapper {
  static constexpr PTR_Parb_regs_pfc1_phv_count Parb_regs_pfc1_phv_count_base=0;
  Parb_regs_pfc1_phv_count_map() : RegisterMapper( {
    { "i_pfc1_phv_count_0_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc1_phv_count_base->i_pfc1_phv_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_pfc1_phv_count_1_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc1_phv_count_base->i_pfc1_phv_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_regs_pfc2_phv_count_map: public RegisterMapper {
  static constexpr PTR_Parb_regs_pfc2_phv_count Parb_regs_pfc2_phv_count_base=0;
  Parb_regs_pfc2_phv_count_map() : RegisterMapper( {
    { "i_pfc2_phv_count_0_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc2_phv_count_base->i_pfc2_phv_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_pfc2_phv_count_1_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc2_phv_count_base->i_pfc2_phv_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_regs_pfc3_phv_count_map: public RegisterMapper {
  static constexpr PTR_Parb_regs_pfc3_phv_count Parb_regs_pfc3_phv_count_base=0;
  Parb_regs_pfc3_phv_count_map() : RegisterMapper( {
    { "i_pfc3_phv_count_0_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc3_phv_count_base->i_pfc3_phv_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_pfc3_phv_count_1_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc3_phv_count_base->i_pfc3_phv_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_regs_pfc4_phv_count_map: public RegisterMapper {
  static constexpr PTR_Parb_regs_pfc4_phv_count Parb_regs_pfc4_phv_count_base=0;
  Parb_regs_pfc4_phv_count_map() : RegisterMapper( {
    { "i_pfc4_phv_count_0_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc4_phv_count_base->i_pfc4_phv_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_pfc4_phv_count_1_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc4_phv_count_base->i_pfc4_phv_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_regs_pfc5_phv_count_map: public RegisterMapper {
  static constexpr PTR_Parb_regs_pfc5_phv_count Parb_regs_pfc5_phv_count_base=0;
  Parb_regs_pfc5_phv_count_map() : RegisterMapper( {
    { "i_pfc5_phv_count_0_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc5_phv_count_base->i_pfc5_phv_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_pfc5_phv_count_1_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc5_phv_count_base->i_pfc5_phv_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_regs_pfc6_phv_count_map: public RegisterMapper {
  static constexpr PTR_Parb_regs_pfc6_phv_count Parb_regs_pfc6_phv_count_base=0;
  Parb_regs_pfc6_phv_count_map() : RegisterMapper( {
    { "i_pfc6_phv_count_0_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc6_phv_count_base->i_pfc6_phv_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_pfc6_phv_count_1_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc6_phv_count_base->i_pfc6_phv_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_regs_pfc7_phv_count_map: public RegisterMapper {
  static constexpr PTR_Parb_regs_pfc7_phv_count Parb_regs_pfc7_phv_count_base=0;
  Parb_regs_pfc7_phv_count_map() : RegisterMapper( {
    { "i_pfc7_phv_count_0_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc7_phv_count_base->i_pfc7_phv_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_pfc7_phv_count_1_2", { const_cast<uint32_t(*)>(&Parb_regs_pfc7_phv_count_base->i_pfc7_phv_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_regs_bubble_count_map: public RegisterMapper {
  static constexpr PTR_Parb_regs_bubble_count Parb_regs_bubble_count_base=0;
  Parb_regs_bubble_count_map() : RegisterMapper( {
    { "i_bubble_count_0_2", { const_cast<uint32_t(*)>(&Parb_regs_bubble_count_base->i_bubble_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_bubble_count_1_2", { const_cast<uint32_t(*)>(&Parb_regs_bubble_count_base->i_bubble_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_regs_phv_count_map: public RegisterMapper {
  static constexpr PTR_Parb_regs_phv_count Parb_regs_phv_count_base=0;
  Parb_regs_phv_count_map() : RegisterMapper( {
    { "i_phv_count_0_2", { const_cast<uint32_t(*)>(&Parb_regs_phv_count_base->i_phv_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_phv_count_1_2", { const_cast<uint32_t(*)>(&Parb_regs_phv_count_base->i_phv_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_regs_parb_group_map: public RegisterMapper {
  static constexpr PTR_Parb_regs_parb_group Parb_regs_parb_group_base=0;
  Parb_regs_parb_group_map() : RegisterMapper( {
    { "cong_slot_limiter", { const_cast<uint32_t(*)>(&Parb_regs_parb_group_base->cong_slot_limiter), 0, {}, sizeof(uint32_t) } },
    { "i_output_rate_ctrl", { const_cast<uint32_t(*)>(&Parb_regs_parb_group_base->i_output_rate_ctrl), 0, {}, sizeof(uint32_t) } },
    { "e_output_rate_ctrl", { const_cast<uint32_t(*)>(&Parb_regs_parb_group_base->e_output_rate_ctrl), 0, {}, sizeof(uint32_t) } },
    { "i_hipri_phv_count", { const_cast<Parb_regs_hipri_phv_count(*)>(&Parb_regs_parb_group_base->i_hipri_phv_count), new Parb_regs_hipri_phv_count_map, {}, sizeof(Parb_regs_hipri_phv_count) } },
    { "i_cong_phv_count", { const_cast<Parb_regs_cong_phv_count(*)>(&Parb_regs_parb_group_base->i_cong_phv_count), new Parb_regs_cong_phv_count_map, {}, sizeof(Parb_regs_cong_phv_count) } },
    { "i_pfc0_phv_count", { const_cast<Parb_regs_pfc0_phv_count(*)>(&Parb_regs_parb_group_base->i_pfc0_phv_count), new Parb_regs_pfc0_phv_count_map, {}, sizeof(Parb_regs_pfc0_phv_count) } },
    { "i_pfc1_phv_count", { const_cast<Parb_regs_pfc1_phv_count(*)>(&Parb_regs_parb_group_base->i_pfc1_phv_count), new Parb_regs_pfc1_phv_count_map, {}, sizeof(Parb_regs_pfc1_phv_count) } },
    { "i_pfc2_phv_count", { const_cast<Parb_regs_pfc2_phv_count(*)>(&Parb_regs_parb_group_base->i_pfc2_phv_count), new Parb_regs_pfc2_phv_count_map, {}, sizeof(Parb_regs_pfc2_phv_count) } },
    { "i_pfc3_phv_count", { const_cast<Parb_regs_pfc3_phv_count(*)>(&Parb_regs_parb_group_base->i_pfc3_phv_count), new Parb_regs_pfc3_phv_count_map, {}, sizeof(Parb_regs_pfc3_phv_count) } },
    { "i_pfc4_phv_count", { const_cast<Parb_regs_pfc4_phv_count(*)>(&Parb_regs_parb_group_base->i_pfc4_phv_count), new Parb_regs_pfc4_phv_count_map, {}, sizeof(Parb_regs_pfc4_phv_count) } },
    { "i_pfc5_phv_count", { const_cast<Parb_regs_pfc5_phv_count(*)>(&Parb_regs_parb_group_base->i_pfc5_phv_count), new Parb_regs_pfc5_phv_count_map, {}, sizeof(Parb_regs_pfc5_phv_count) } },
    { "i_pfc6_phv_count", { const_cast<Parb_regs_pfc6_phv_count(*)>(&Parb_regs_parb_group_base->i_pfc6_phv_count), new Parb_regs_pfc6_phv_count_map, {}, sizeof(Parb_regs_pfc6_phv_count) } },
    { "i_pfc7_phv_count", { const_cast<Parb_regs_pfc7_phv_count(*)>(&Parb_regs_parb_group_base->i_pfc7_phv_count), new Parb_regs_pfc7_phv_count_map, {}, sizeof(Parb_regs_pfc7_phv_count) } },
    { "e_pfc0_phv_count", { const_cast<Parb_regs_pfc0_phv_count(*)>(&Parb_regs_parb_group_base->e_pfc0_phv_count), new Parb_regs_pfc0_phv_count_map, {}, sizeof(Parb_regs_pfc0_phv_count) } },
    { "e_pfc1_phv_count", { const_cast<Parb_regs_pfc1_phv_count(*)>(&Parb_regs_parb_group_base->e_pfc1_phv_count), new Parb_regs_pfc1_phv_count_map, {}, sizeof(Parb_regs_pfc1_phv_count) } },
    { "e_pfc2_phv_count", { const_cast<Parb_regs_pfc2_phv_count(*)>(&Parb_regs_parb_group_base->e_pfc2_phv_count), new Parb_regs_pfc2_phv_count_map, {}, sizeof(Parb_regs_pfc2_phv_count) } },
    { "e_pfc3_phv_count", { const_cast<Parb_regs_pfc3_phv_count(*)>(&Parb_regs_parb_group_base->e_pfc3_phv_count), new Parb_regs_pfc3_phv_count_map, {}, sizeof(Parb_regs_pfc3_phv_count) } },
    { "e_pfc4_phv_count", { const_cast<Parb_regs_pfc4_phv_count(*)>(&Parb_regs_parb_group_base->e_pfc4_phv_count), new Parb_regs_pfc4_phv_count_map, {}, sizeof(Parb_regs_pfc4_phv_count) } },
    { "e_pfc5_phv_count", { const_cast<Parb_regs_pfc5_phv_count(*)>(&Parb_regs_parb_group_base->e_pfc5_phv_count), new Parb_regs_pfc5_phv_count_map, {}, sizeof(Parb_regs_pfc5_phv_count) } },
    { "e_pfc6_phv_count", { const_cast<Parb_regs_pfc6_phv_count(*)>(&Parb_regs_parb_group_base->e_pfc6_phv_count), new Parb_regs_pfc6_phv_count_map, {}, sizeof(Parb_regs_pfc6_phv_count) } },
    { "e_pfc7_phv_count", { const_cast<Parb_regs_pfc7_phv_count(*)>(&Parb_regs_parb_group_base->e_pfc7_phv_count), new Parb_regs_pfc7_phv_count_map, {}, sizeof(Parb_regs_pfc7_phv_count) } },
    { "e_hipri_phv_count", { const_cast<Parb_regs_hipri_phv_count(*)>(&Parb_regs_parb_group_base->e_hipri_phv_count), new Parb_regs_hipri_phv_count_map, {}, sizeof(Parb_regs_hipri_phv_count) } },
    { "i_bubble_count", { const_cast<Parb_regs_bubble_count(*)>(&Parb_regs_parb_group_base->i_bubble_count), new Parb_regs_bubble_count_map, {}, sizeof(Parb_regs_bubble_count) } },
    { "e_bubble_count", { const_cast<Parb_regs_bubble_count(*)>(&Parb_regs_parb_group_base->e_bubble_count), new Parb_regs_bubble_count_map, {}, sizeof(Parb_regs_bubble_count) } },
    { "mau_micro_update", { const_cast<uint32_t(*)>(&Parb_regs_parb_group_base->mau_micro_update), 0, {}, sizeof(uint32_t) } },
    { "dprs_input_fifo_count", { const_cast<uint32_t(*)>(&Parb_regs_parb_group_base->dprs_input_fifo_count), 0, {}, sizeof(uint32_t) } },
    { "dprs_input_fifo_max", { const_cast<uint32_t(*)>(&Parb_regs_parb_group_base->dprs_input_fifo_max), 0, {}, sizeof(uint32_t) } },
    { "i_phv_count", { const_cast<Parb_regs_phv_count(*)>(&Parb_regs_parb_group_base->i_phv_count), new Parb_regs_phv_count_map, {}, sizeof(Parb_regs_phv_count) } },
    { "i_eop_count", { const_cast<Parb_regs_phv_count(*)>(&Parb_regs_parb_group_base->i_eop_count), new Parb_regs_phv_count_map, {}, sizeof(Parb_regs_phv_count) } },
    { "e_phv_count", { const_cast<Parb_regs_phv_count(*)>(&Parb_regs_parb_group_base->e_phv_count), new Parb_regs_phv_count_map, {}, sizeof(Parb_regs_phv_count) } },
    { "e_eop_count", { const_cast<Parb_regs_phv_count(*)>(&Parb_regs_parb_group_base->e_eop_count), new Parb_regs_phv_count_map, {}, sizeof(Parb_regs_phv_count) } },
    { "i_norm_phv_count", { const_cast<Parb_regs_phv_count(*)>(&Parb_regs_parb_group_base->i_norm_phv_count), new Parb_regs_phv_count_map, {}, sizeof(Parb_regs_phv_count) } },
    { "i_norm_eop_count", { const_cast<Parb_regs_phv_count(*)>(&Parb_regs_parb_group_base->i_norm_eop_count), new Parb_regs_phv_count_map, {}, sizeof(Parb_regs_phv_count) } },
    { "i_resub_phv_count", { const_cast<Parb_regs_phv_count(*)>(&Parb_regs_parb_group_base->i_resub_phv_count), new Parb_regs_phv_count_map, {}, sizeof(Parb_regs_phv_count) } },
    { "i_resub_eop_count", { const_cast<Parb_regs_phv_count(*)>(&Parb_regs_parb_group_base->i_resub_eop_count), new Parb_regs_phv_count_map, {}, sizeof(Parb_regs_phv_count) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Parb_regs_parb_group_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "scratch", { const_cast<uint32_t(*)>(&Parb_regs_parb_group_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "i_chnl_ctrl", { const_cast<uint32_t(*)[0x48]>(&Parb_regs_parb_group_base->i_chnl_ctrl), 0, {0x48}, sizeof(uint32_t) } },
    { "e_chnl_ctrl", { const_cast<uint32_t(*)[0x48]>(&Parb_regs_parb_group_base->e_chnl_ctrl), 0, {0x48}, sizeof(uint32_t) } },
    { "i_port_dbg", { const_cast<uint32_t(*)[0x12]>(&Parb_regs_parb_group_base->i_port_dbg), 0, {0x12}, sizeof(uint32_t) } },
    { "e_port_dbg", { const_cast<uint32_t(*)[0x12]>(&Parb_regs_parb_group_base->e_port_dbg), 0, {0x12}, sizeof(uint32_t) } }
    } )
  {}
};

struct Parb_regs_map: public RegisterMapper {
  static constexpr PTR_Parb_regs Parb_regs_base=0;
  Parb_regs_map() : RegisterMapper( {
    { "parb_group", { &Parb_regs_base->parb_group, new Parb_regs_parb_group_map, {}, sizeof(Parb_regs_parb_group) } }
    } )
  {}
};

struct Prsr_reg_merge_rspec_phv_owner_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_merge_rspec_phv_owner Prsr_reg_merge_rspec_phv_owner_base=0;
  Prsr_reg_merge_rspec_phv_owner_map() : RegisterMapper( {
    { "phv_owner_0_12", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_owner_base->phv_owner_0_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_1_12", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_owner_base->phv_owner_1_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_2_12", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_owner_base->phv_owner_2_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_3_12", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_owner_base->phv_owner_3_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_4_12", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_owner_base->phv_owner_4_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_5_12", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_owner_base->phv_owner_5_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_6_12", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_owner_base->phv_owner_6_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_7_12", { &Prsr_reg_merge_rspec_phv_owner_base->phv_owner_7_12, 0, {}, sizeof(uint32_t) } },
    { "phv_owner_8_12", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_owner_base->phv_owner_8_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_9_12", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_owner_base->phv_owner_9_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_10_12", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_owner_base->phv_owner_10_12), 0, {}, sizeof(uint32_t) } },
    { "phv_owner_11_12", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_owner_base->phv_owner_11_12), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_merge_rspec_phv_valid_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_merge_rspec_phv_valid Prsr_reg_merge_rspec_phv_valid_base=0;
  Prsr_reg_merge_rspec_phv_valid_map() : RegisterMapper( {
    { "phv_valid_0_7", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_valid_base->phv_valid_0_7), 0, {}, sizeof(uint32_t) } },
    { "phv_valid_1_7", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_valid_base->phv_valid_1_7), 0, {}, sizeof(uint32_t) } },
    { "phv_valid_2_7", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_valid_base->phv_valid_2_7), 0, {}, sizeof(uint32_t) } },
    { "phv_valid_3_7", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_valid_base->phv_valid_3_7), 0, {}, sizeof(uint32_t) } },
    { "phv_valid_4_7", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_valid_base->phv_valid_4_7), 0, {}, sizeof(uint32_t) } },
    { "phv_valid_5_7", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_valid_base->phv_valid_5_7), 0, {}, sizeof(uint32_t) } },
    { "phv_valid_6_7", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_phv_valid_base->phv_valid_6_7), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Prsr_reg_merge_rspec_map: public RegisterMapper {
  static constexpr PTR_Prsr_reg_merge_rspec Prsr_reg_merge_rspec_base=0;
  Prsr_reg_merge_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Prsr_reg_merge_rspec_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "phv_owner", { const_cast<Prsr_reg_merge_rspec_phv_owner(*)>(&Prsr_reg_merge_rspec_base->phv_owner), new Prsr_reg_merge_rspec_phv_owner_map, {}, sizeof(Prsr_reg_merge_rspec_phv_owner) } },
    { "phv_valid", { const_cast<Prsr_reg_merge_rspec_phv_valid(*)>(&Prsr_reg_merge_rspec_base->phv_valid), new Prsr_reg_merge_rspec_phv_valid_map, {}, sizeof(Prsr_reg_merge_rspec_phv_valid) } },
    { "mode", { const_cast<uint32_t(*)[0x12]>(&Prsr_reg_merge_rspec_base->mode), 0, {0x12}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pbus_station_regs_intr_map: public RegisterMapper {
  static constexpr PTR_Pbus_station_regs_intr Pbus_station_regs_intr_base=0;
  Pbus_station_regs_intr_map() : RegisterMapper( {
    { "status", { const_cast<uint32_t(*)>(&Pbus_station_regs_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Pbus_station_regs_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Pbus_station_regs_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Pbus_station_regs_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Pbus_station_regs_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pbus_station_regs_map: public RegisterMapper {
  static constexpr PTR_Pbus_station_regs Pbus_station_regs_base=0;
  Pbus_station_regs_map() : RegisterMapper( {
    { "intr", { &Pbus_station_regs_base->intr, new Pbus_station_regs_intr_map, {}, sizeof(Pbus_station_regs_intr) } },
    { "ecc", { const_cast<uint32_t(*)>(&Pbus_station_regs_base->ecc), 0, {}, sizeof(uint32_t) } },
    { "ecc_dis", { const_cast<uint32_t(*)>(&Pbus_station_regs_base->ecc_dis), 0, {}, sizeof(uint32_t) } },
    { "capt_dual_ecc_addr", { const_cast<uint32_t(*)>(&Pbus_station_regs_base->capt_dual_ecc_addr), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Pbus_station_regs_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "scratch", { const_cast<uint32_t(*)>(&Pbus_station_regs_base->scratch), 0, {}, sizeof(uint32_t) } }
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

struct Party_pgr_app_reg_rspec_map: public RegisterMapper {
  static constexpr PTR_Party_pgr_app_reg_rspec Party_pgr_app_reg_rspec_base=0;
  Party_pgr_app_reg_rspec_map() : RegisterMapper( {
    { "ctrl", { const_cast<uint32_t(*)>(&Party_pgr_app_reg_rspec_base->ctrl), 0, {}, sizeof(uint32_t) } },
    { "payload_ctrl", { const_cast<uint32_t(*)>(&Party_pgr_app_reg_rspec_base->payload_ctrl), 0, {}, sizeof(uint32_t) } },
    { "ingr_port_ctrl", { const_cast<uint32_t(*)>(&Party_pgr_app_reg_rspec_base->ingr_port_ctrl), 0, {}, sizeof(uint32_t) } },
    { "recir_match_value", { const_cast<uint32_t(*)>(&Party_pgr_app_reg_rspec_base->recir_match_value), 0, {}, sizeof(uint32_t) } },
    { "recir_match_mask", { const_cast<uint32_t(*)>(&Party_pgr_app_reg_rspec_base->recir_match_mask), 0, {}, sizeof(uint32_t) } },
    { "event_number", { const_cast<uint32_t(*)>(&Party_pgr_app_reg_rspec_base->event_number), 0, {}, sizeof(uint32_t) } },
    { "event_ibg", { const_cast<uint32_t(*)>(&Party_pgr_app_reg_rspec_base->event_ibg), 0, {}, sizeof(uint32_t) } },
    { "event_ibg_jitter_value", { const_cast<uint32_t(*)>(&Party_pgr_app_reg_rspec_base->event_ibg_jitter_value), 0, {}, sizeof(uint32_t) } },
    { "event_ibg_jitter_mask", { const_cast<uint32_t(*)>(&Party_pgr_app_reg_rspec_base->event_ibg_jitter_mask), 0, {}, sizeof(uint32_t) } },
    { "event_ipg", { const_cast<uint32_t(*)>(&Party_pgr_app_reg_rspec_base->event_ipg), 0, {}, sizeof(uint32_t) } },
    { "event_ipg_jitter_value", { const_cast<uint32_t(*)>(&Party_pgr_app_reg_rspec_base->event_ipg_jitter_value), 0, {}, sizeof(uint32_t) } },
    { "event_ipg_jitter_mask", { const_cast<uint32_t(*)>(&Party_pgr_app_reg_rspec_base->event_ipg_jitter_mask), 0, {}, sizeof(uint32_t) } },
    { "event_timer", { const_cast<uint32_t(*)>(&Party_pgr_app_reg_rspec_base->event_timer), 0, {}, sizeof(uint32_t) } },
    { "ctr48_trigger", { const_cast<Pgr_app_ctr48_trigger(*)>(&Party_pgr_app_reg_rspec_base->ctr48_trigger), new Pgr_app_ctr48_trigger_map, {}, sizeof(Pgr_app_ctr48_trigger) } },
    { "ctr48_batch", { const_cast<Pgr_app_ctr48_batch(*)>(&Party_pgr_app_reg_rspec_base->ctr48_batch), new Pgr_app_ctr48_batch_map, {}, sizeof(Pgr_app_ctr48_batch) } },
    { "ctr48_packet", { const_cast<Pgr_app_ctr48_packet(*)>(&Party_pgr_app_reg_rspec_base->ctr48_packet), new Pgr_app_ctr48_packet_map, {}, sizeof(Pgr_app_ctr48_packet) } },
    { "log", { const_cast<uint32_t(*)>(&Party_pgr_app_reg_rspec_base->log), 0, {}, sizeof(uint32_t) } }
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

struct Pgr_ctr48_port16_crc_err_map: public RegisterMapper {
  static constexpr PTR_Pgr_ctr48_port16_crc_err Pgr_ctr48_port16_crc_err_base=0;
  Pgr_ctr48_port16_crc_err_map() : RegisterMapper( {
    { "port16_crc_err_count_0_2", { const_cast<uint32_t(*)>(&Pgr_ctr48_port16_crc_err_base->port16_crc_err_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "port16_crc_err_count_1_2", { const_cast<uint32_t(*)>(&Pgr_ctr48_port16_crc_err_base->port16_crc_err_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Pgr_ctr48_port17_crc_err_map: public RegisterMapper {
  static constexpr PTR_Pgr_ctr48_port17_crc_err Pgr_ctr48_port17_crc_err_base=0;
  Pgr_ctr48_port17_crc_err_map() : RegisterMapper( {
    { "port17_crc_err_count_0_2", { const_cast<uint32_t(*)>(&Pgr_ctr48_port17_crc_err_base->port17_crc_err_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "port17_crc_err_count_1_2", { const_cast<uint32_t(*)>(&Pgr_ctr48_port17_crc_err_base->port17_crc_err_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Party_pgr_common_reg_rspec_map: public RegisterMapper {
  static constexpr PTR_Party_pgr_common_reg_rspec Party_pgr_common_reg_rspec_base=0;
  Party_pgr_common_reg_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)[0x4]>(&Party_pgr_common_reg_rspec_base->scratch), 0, {0x4}, sizeof(uint32_t) } },
    { "port16_ts", { const_cast<uint32_t(*)>(&Party_pgr_common_reg_rspec_base->port16_ts), 0, {}, sizeof(uint32_t) } },
    { "port17_ts", { const_cast<uint32_t(*)>(&Party_pgr_common_reg_rspec_base->port17_ts), 0, {}, sizeof(uint32_t) } },
    { "port16_ctrl", { const_cast<uint32_t(*)>(&Party_pgr_common_reg_rspec_base->port16_ctrl), 0, {}, sizeof(uint32_t) } },
    { "port17_ctrl1", { const_cast<uint32_t(*)>(&Party_pgr_common_reg_rspec_base->port17_ctrl1), 0, {}, sizeof(uint32_t) } },
    { "port17_ctrl2", { const_cast<uint32_t(*)>(&Party_pgr_common_reg_rspec_base->port17_ctrl2), 0, {}, sizeof(uint32_t) } },
    { "port_down_dis", { const_cast<Pgr_port_down_dis(*)>(&Party_pgr_common_reg_rspec_base->port_down_dis), new Pgr_port_down_dis_map, {}, sizeof(Pgr_port_down_dis) } },
    { "int_stat", { const_cast<uint32_t(*)>(&Party_pgr_common_reg_rspec_base->int_stat), 0, {}, sizeof(uint32_t) } },
    { "int_en0", { const_cast<uint32_t(*)>(&Party_pgr_common_reg_rspec_base->int_en0), 0, {}, sizeof(uint32_t) } },
    { "int_en1", { const_cast<uint32_t(*)>(&Party_pgr_common_reg_rspec_base->int_en1), 0, {}, sizeof(uint32_t) } },
    { "freeze_en", { const_cast<uint32_t(*)>(&Party_pgr_common_reg_rspec_base->freeze_en), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Party_pgr_common_reg_rspec_base->int_inj), 0, {}, sizeof(uint32_t) } },
    { "port16_crc_err_count", { const_cast<Pgr_ctr48_port16_crc_err(*)[0x4]>(&Party_pgr_common_reg_rspec_base->port16_crc_err_count), new Pgr_ctr48_port16_crc_err_map, {0x4}, sizeof(Pgr_ctr48_port16_crc_err) } },
    { "port17_crc_err_count", { const_cast<Pgr_ctr48_port17_crc_err(*)[0x4]>(&Party_pgr_common_reg_rspec_base->port17_crc_err_count), new Pgr_ctr48_port17_crc_err_map, {0x4}, sizeof(Pgr_ctr48_port17_crc_err) } },
    { "sbe_log", { const_cast<uint32_t(*)>(&Party_pgr_common_reg_rspec_base->sbe_log), 0, {}, sizeof(uint32_t) } },
    { "mbe_log", { const_cast<uint32_t(*)>(&Party_pgr_common_reg_rspec_base->mbe_log), 0, {}, sizeof(uint32_t) } },
    { "pgen_log", { const_cast<uint32_t(*)>(&Party_pgr_common_reg_rspec_base->pgen_log), 0, {}, sizeof(uint32_t) } },
    { "credit_ctrl", { const_cast<uint32_t(*)>(&Party_pgr_common_reg_rspec_base->credit_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Party_pgr_reg_rspec_map: public RegisterMapper {
  static constexpr PTR_Party_pgr_reg_rspec Party_pgr_reg_rspec_base=0;
  Party_pgr_reg_rspec_map() : RegisterMapper( {
    { "pgr_app", { &Party_pgr_reg_rspec_base->pgr_app, new Party_pgr_app_reg_rspec_map, {0x8}, sizeof(Party_pgr_app_reg_rspec) } },
    { "pgr_common", { &Party_pgr_reg_rspec_base->pgr_common, new Party_pgr_common_reg_rspec_map, {}, sizeof(Party_pgr_common_reg_rspec) } }
    } )
  {}
};

struct Party_glue_reg_rspec_ibp18_intr_map: public RegisterMapper {
  static constexpr PTR_Party_glue_reg_rspec_ibp18_intr Party_glue_reg_rspec_ibp18_intr_base=0;
  Party_glue_reg_rspec_ibp18_intr_map() : RegisterMapper( {
    { "status0", { const_cast<uint32_t(*)>(&Party_glue_reg_rspec_ibp18_intr_base->status0), 0, {}, sizeof(uint32_t) } },
    { "status1", { const_cast<uint32_t(*)>(&Party_glue_reg_rspec_ibp18_intr_base->status1), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Party_glue_reg_rspec_ebp18_intr_map: public RegisterMapper {
  static constexpr PTR_Party_glue_reg_rspec_ebp18_intr Party_glue_reg_rspec_ebp18_intr_base=0;
  Party_glue_reg_rspec_ebp18_intr_map() : RegisterMapper( {
    { "status0", { const_cast<uint32_t(*)>(&Party_glue_reg_rspec_ebp18_intr_base->status0), 0, {}, sizeof(uint32_t) } },
    { "status1", { const_cast<uint32_t(*)>(&Party_glue_reg_rspec_ebp18_intr_base->status1), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Party_glue_reg_rspec_egr18_intr_map: public RegisterMapper {
  static constexpr PTR_Party_glue_reg_rspec_egr18_intr Party_glue_reg_rspec_egr18_intr_base=0;
  Party_glue_reg_rspec_egr18_intr_map() : RegisterMapper( {
    { "status0", { const_cast<uint32_t(*)>(&Party_glue_reg_rspec_egr18_intr_base->status0), 0, {}, sizeof(uint32_t) } },
    { "status1", { const_cast<uint32_t(*)>(&Party_glue_reg_rspec_egr18_intr_base->status1), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Party_glue_reg_rspec_parde_intr_map: public RegisterMapper {
  static constexpr PTR_Party_glue_reg_rspec_parde_intr Party_glue_reg_rspec_parde_intr_base=0;
  Party_glue_reg_rspec_parde_intr_map() : RegisterMapper( {
    { "status0", { const_cast<uint32_t(*)>(&Party_glue_reg_rspec_parde_intr_base->status0), 0, {}, sizeof(uint32_t) } },
    { "status1", { const_cast<uint32_t(*)>(&Party_glue_reg_rspec_parde_intr_base->status1), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Party_glue_reg_rspec_map: public RegisterMapper {
  static constexpr PTR_Party_glue_reg_rspec Party_glue_reg_rspec_base=0;
  Party_glue_reg_rspec_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Party_glue_reg_rspec_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "debug_ctrl", { const_cast<uint32_t(*)>(&Party_glue_reg_rspec_base->debug_ctrl), 0, {}, sizeof(uint32_t) } },
    { "csr_ring_full_thresh", { const_cast<uint32_t(*)>(&Party_glue_reg_rspec_base->csr_ring_full_thresh), 0, {}, sizeof(uint32_t) } },
    { "csr_ring_fifo_err", { const_cast<uint32_t(*)>(&Party_glue_reg_rspec_base->csr_ring_fifo_err), 0, {}, sizeof(uint32_t) } },
    { "ibp18_intr", { &Party_glue_reg_rspec_base->ibp18_intr, new Party_glue_reg_rspec_ibp18_intr_map, {}, sizeof(Party_glue_reg_rspec_ibp18_intr) } },
    { "ebp18_intr", { &Party_glue_reg_rspec_base->ebp18_intr, new Party_glue_reg_rspec_ebp18_intr_map, {}, sizeof(Party_glue_reg_rspec_ebp18_intr) } },
    { "egr18_intr", { &Party_glue_reg_rspec_base->egr18_intr, new Party_glue_reg_rspec_egr18_intr_map, {}, sizeof(Party_glue_reg_rspec_egr18_intr) } },
    { "parde_intr", { &Party_glue_reg_rspec_base->parde_intr, new Party_glue_reg_rspec_parde_intr_map, {}, sizeof(Party_glue_reg_rspec_parde_intr) } },
    { "intr_period", { const_cast<uint32_t(*)>(&Party_glue_reg_rspec_base->intr_period), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_prsr_port_regs_map: public RegisterMapper {
  static constexpr PTR_Epb_prsr_port_regs Epb_prsr_port_regs_base=0;
  Epb_prsr_port_regs_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<uint32_t(*)[0x4]>(&Epb_prsr_port_regs_base->chnl_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "port_id", { const_cast<uint32_t(*)>(&Epb_prsr_port_regs_base->port_id), 0, {}, sizeof(uint32_t) } },
    { "tim_off", { const_cast<uint32_t(*)>(&Epb_prsr_port_regs_base->tim_off), 0, {}, sizeof(uint32_t) } },
    { "multi_threading", { const_cast<uint32_t(*)>(&Epb_prsr_port_regs_base->multi_threading), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Epb_prsr_port_regs_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "scratch", { const_cast<uint32_t(*)>(&Epb_prsr_port_regs_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dbg_ctrl", { const_cast<uint32_t(*)>(&Epb_prsr_port_regs_base->dbg_ctrl), 0, {}, sizeof(uint32_t) } },
    { "int_stat", { const_cast<uint32_t(*)>(&Epb_prsr_port_regs_base->int_stat), 0, {}, sizeof(uint32_t) } },
    { "int_en", { const_cast<uint32_t(*)>(&Epb_prsr_port_regs_base->int_en), 0, {}, sizeof(uint32_t) } },
    { "int_pri", { const_cast<uint32_t(*)>(&Epb_prsr_port_regs_base->int_pri), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Epb_prsr_port_regs_base->int_inj), 0, {}, sizeof(uint32_t) } },
    { "int_freeze", { const_cast<uint32_t(*)>(&Epb_prsr_port_regs_base->int_freeze), 0, {}, sizeof(uint32_t) } },
    { "epb_meta_fifo_avail", { const_cast<uint32_t(*)>(&Epb_prsr_port_regs_base->epb_meta_fifo_avail), 0, {}, sizeof(uint32_t) } },
    { "chnl_ctrl2", { const_cast<uint32_t(*)[0x4]>(&Epb_prsr_port_regs_base->chnl_ctrl2), 0, {0x4}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebp_rspec_map: public RegisterMapper {
  static constexpr PTR_Ebp_rspec Ebp_rspec_base=0;
  Ebp_rspec_map() : RegisterMapper( {
    { "prsr_reg", { &Ebp_rspec_base->prsr_reg, new Prsr_reg_main_rspec_map, {}, sizeof(Prsr_reg_main_rspec) } },
    { "epb_prsr_port_regs", { &Ebp_rspec_base->epb_prsr_port_regs, new Epb_prsr_port_regs_map, {}, sizeof(Epb_prsr_port_regs) } }
    } )
  {}
};

struct Ebuf_disp_regs_map: public RegisterMapper {
  static constexpr PTR_Ebuf_disp_regs Ebuf_disp_regs_base=0;
  Ebuf_disp_regs_map() : RegisterMapper( {
    { "port_mode", { const_cast<uint32_t(*)>(&Ebuf_disp_regs_base->port_mode), 0, {}, sizeof(uint32_t) } },
    { "cred_ctrl", { const_cast<uint32_t(*)>(&Ebuf_disp_regs_base->cred_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_ctrl", { const_cast<uint32_t(*)[0x4]>(&Ebuf_disp_regs_base->chnl_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "int_stat", { const_cast<uint32_t(*)>(&Ebuf_disp_regs_base->int_stat), 0, {}, sizeof(uint32_t) } },
    { "int_en", { const_cast<uint32_t(*)>(&Ebuf_disp_regs_base->int_en), 0, {}, sizeof(uint32_t) } },
    { "int_pri", { const_cast<uint32_t(*)>(&Ebuf_disp_regs_base->int_pri), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Ebuf_disp_regs_base->int_inj), 0, {}, sizeof(uint32_t) } },
    { "ebuf_disp_fifo_avail", { const_cast<uint32_t(*)>(&Ebuf_disp_regs_base->ebuf_disp_fifo_avail), 0, {}, sizeof(uint32_t) } },
    { "int_freeze", { const_cast<uint32_t(*)>(&Ebuf_disp_regs_base->int_freeze), 0, {}, sizeof(uint32_t) } },
    { "dbg_ctrl", { const_cast<uint32_t(*)>(&Ebuf_disp_regs_base->dbg_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebuf_fifo_regs_map: public RegisterMapper {
  static constexpr PTR_Ebuf_fifo_regs Ebuf_fifo_regs_base=0;
  Ebuf_fifo_regs_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<uint32_t(*)[0x4]>(&Ebuf_fifo_regs_base->chnl_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "int_stat", { const_cast<uint32_t(*)>(&Ebuf_fifo_regs_base->int_stat), 0, {}, sizeof(uint32_t) } },
    { "int_en", { const_cast<uint32_t(*)>(&Ebuf_fifo_regs_base->int_en), 0, {}, sizeof(uint32_t) } },
    { "int_pri", { const_cast<uint32_t(*)>(&Ebuf_fifo_regs_base->int_pri), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Ebuf_fifo_regs_base->int_inj), 0, {}, sizeof(uint32_t) } },
    { "capt_dual_ecc_addr", { const_cast<uint32_t(*)>(&Ebuf_fifo_regs_base->capt_dual_ecc_addr), 0, {}, sizeof(uint32_t) } },
    { "egrbyp_ctrl", { const_cast<uint32_t(*)>(&Ebuf_fifo_regs_base->egrbyp_ctrl), 0, {}, sizeof(uint32_t) } },
    { "aemp_thr", { const_cast<uint32_t(*)>(&Ebuf_fifo_regs_base->aemp_thr), 0, {}, sizeof(uint32_t) } },
    { "int_freeze", { const_cast<uint32_t(*)>(&Ebuf_fifo_regs_base->int_freeze), 0, {}, sizeof(uint32_t) } },
    { "dbg_ctrl", { const_cast<uint32_t(*)>(&Ebuf_fifo_regs_base->dbg_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_dprs_regs_map: public RegisterMapper {
  static constexpr PTR_Epb_dprs_regs Epb_dprs_regs_base=0;
  Epb_dprs_regs_map() : RegisterMapper( {
    { "chnl_ctrl", { const_cast<uint32_t(*)[0x4]>(&Epb_dprs_regs_base->chnl_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "port_id", { const_cast<uint32_t(*)>(&Epb_dprs_regs_base->port_id), 0, {}, sizeof(uint32_t) } },
    { "int_stat", { const_cast<uint32_t(*)>(&Epb_dprs_regs_base->int_stat), 0, {}, sizeof(uint32_t) } },
    { "int_en", { const_cast<uint32_t(*)>(&Epb_dprs_regs_base->int_en), 0, {}, sizeof(uint32_t) } },
    { "int_pri", { const_cast<uint32_t(*)>(&Epb_dprs_regs_base->int_pri), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Epb_dprs_regs_base->int_inj), 0, {}, sizeof(uint32_t) } },
    { "capt_dual_ecc_addr", { const_cast<uint32_t(*)>(&Epb_dprs_regs_base->capt_dual_ecc_addr), 0, {}, sizeof(uint32_t) } },
    { "scratch", { const_cast<uint32_t(*)>(&Epb_dprs_regs_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "cred_ctrl", { const_cast<uint32_t(*)>(&Epb_dprs_regs_base->cred_ctrl), 0, {}, sizeof(uint32_t) } },
    { "cred_cur", { const_cast<uint32_t(*)>(&Epb_dprs_regs_base->cred_cur), 0, {}, sizeof(uint32_t) } },
    { "dprsff_max", { const_cast<uint32_t(*)>(&Epb_dprs_regs_base->dprsff_max), 0, {}, sizeof(uint32_t) } },
    { "dprsff_emp", { const_cast<uint32_t(*)>(&Epb_dprs_regs_base->dprsff_emp), 0, {}, sizeof(uint32_t) } },
    { "int_freeze", { const_cast<uint32_t(*)>(&Epb_dprs_regs_base->int_freeze), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_disp_port_regs_egr_bypass_count_map: public RegisterMapper {
  static constexpr PTR_Epb_disp_port_regs_egr_bypass_count Epb_disp_port_regs_egr_bypass_count_base=0;
  Epb_disp_port_regs_egr_bypass_count_map() : RegisterMapper( {
    { "egr_bypass_count_0_2", { const_cast<uint32_t(*)>(&Epb_disp_port_regs_egr_bypass_count_base->egr_bypass_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "egr_bypass_count_1_2", { const_cast<uint32_t(*)>(&Epb_disp_port_regs_egr_bypass_count_base->egr_bypass_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_disp_port_regs_egr_pipe_count_map: public RegisterMapper {
  static constexpr PTR_Epb_disp_port_regs_egr_pipe_count Epb_disp_port_regs_egr_pipe_count_base=0;
  Epb_disp_port_regs_egr_pipe_count_map() : RegisterMapper( {
    { "egr_pipe_count_0_2", { const_cast<uint32_t(*)>(&Epb_disp_port_regs_egr_pipe_count_base->egr_pipe_count_0_2), 0, {}, sizeof(uint32_t) } },
    { "egr_pipe_count_1_2", { const_cast<uint32_t(*)>(&Epb_disp_port_regs_egr_pipe_count_base->egr_pipe_count_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Epb_disp_port_regs_map: public RegisterMapper {
  static constexpr PTR_Epb_disp_port_regs Epb_disp_port_regs_base=0;
  Epb_disp_port_regs_map() : RegisterMapper( {
    { "port_mode", { const_cast<uint32_t(*)>(&Epb_disp_port_regs_base->port_mode), 0, {}, sizeof(uint32_t) } },
    { "cred_ctrl", { const_cast<uint32_t(*)>(&Epb_disp_port_regs_base->cred_ctrl), 0, {}, sizeof(uint32_t) } },
    { "chnl_ctrl", { const_cast<uint32_t(*)[0x4]>(&Epb_disp_port_regs_base->chnl_ctrl), 0, {0x4}, sizeof(uint32_t) } },
    { "egr_bypass_count", { const_cast<Epb_disp_port_regs_egr_bypass_count(*)[0x4]>(&Epb_disp_port_regs_base->egr_bypass_count), new Epb_disp_port_regs_egr_bypass_count_map, {0x4}, sizeof(Epb_disp_port_regs_egr_bypass_count) } },
    { "egr_pipe_count", { const_cast<Epb_disp_port_regs_egr_pipe_count(*)[0x4]>(&Epb_disp_port_regs_base->egr_pipe_count), new Epb_disp_port_regs_egr_pipe_count_map, {0x4}, sizeof(Epb_disp_port_regs_egr_pipe_count) } },
    { "int_stat", { const_cast<uint32_t(*)>(&Epb_disp_port_regs_base->int_stat), 0, {}, sizeof(uint32_t) } },
    { "int_en", { const_cast<uint32_t(*)>(&Epb_disp_port_regs_base->int_en), 0, {}, sizeof(uint32_t) } },
    { "int_pri", { const_cast<uint32_t(*)>(&Epb_disp_port_regs_base->int_pri), 0, {}, sizeof(uint32_t) } },
    { "int_inj", { const_cast<uint32_t(*)>(&Epb_disp_port_regs_base->int_inj), 0, {}, sizeof(uint32_t) } },
    { "epb_disp_fifo_avail", { const_cast<uint32_t(*)>(&Epb_disp_port_regs_base->epb_disp_fifo_avail), 0, {}, sizeof(uint32_t) } },
    { "int_freeze", { const_cast<uint32_t(*)>(&Epb_disp_port_regs_base->int_freeze), 0, {}, sizeof(uint32_t) } },
    { "dbg_ctrl", { const_cast<uint32_t(*)>(&Epb_disp_port_regs_base->dbg_ctrl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct EgrNx_regs_map: public RegisterMapper {
  static constexpr PTR_EgrNx_regs EgrNx_regs_base=0;
  EgrNx_regs_map() : RegisterMapper( {
    { "ebuf_disp_regs", { &EgrNx_regs_base->ebuf_disp_regs, new Ebuf_disp_regs_map, {}, sizeof(Ebuf_disp_regs) } },
    { "ebuf_fifo_regs", { &EgrNx_regs_base->ebuf_fifo_regs, new Ebuf_fifo_regs_map, {}, sizeof(Ebuf_fifo_regs) } },
    { "epb_dprs_regs", { &EgrNx_regs_base->epb_dprs_regs, new Epb_dprs_regs_map, {}, sizeof(Epb_dprs_regs) } },
    { "epb_disp_port_regs", { &EgrNx_regs_base->epb_disp_port_regs, new Epb_disp_port_regs_map, {}, sizeof(Epb_disp_port_regs) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&EgrNx_regs_base->dft_csr), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Ebp18_rspec_map: public RegisterMapper {
  static constexpr PTR_Ebp18_rspec Ebp18_rspec_base=0;
  Ebp18_rspec_map() : RegisterMapper( {
    { "ebp_reg", { &Ebp18_rspec_base->ebp_reg, new Ebp_rspec_map, {0x12}, sizeof(Ebp_rspec) } },
    { "egrNx_reg", { &Ebp18_rspec_base->egrNx_reg, new EgrNx_regs_map, {0x12}, sizeof(EgrNx_regs) } }
    } )
  {}
};

struct Pmarb_rspec_map: public RegisterMapper {
  static constexpr PTR_Pmarb_rspec Pmarb_rspec_base=0;
  Pmarb_rspec_map() : RegisterMapper( {
    { "ibp18_reg", { &Pmarb_rspec_base->ibp18_reg, new Ibp18_rspec_map, {}, sizeof(Ibp18_rspec) } },
    { "parb_reg", { &Pmarb_rspec_base->parb_reg, new Parb_regs_map, {}, sizeof(Parb_regs) } },
    { "prsr_reg", { &Pmarb_rspec_base->prsr_reg, new Prsr_reg_merge_rspec_map, {}, sizeof(Prsr_reg_merge_rspec) } },
    { "pbusstat_reg", { &Pmarb_rspec_base->pbusstat_reg, new Pbus_station_regs_map, {}, sizeof(Pbus_station_regs) } },
    { "pgr_reg", { &Pmarb_rspec_base->pgr_reg, new Party_pgr_reg_rspec_map, {}, sizeof(Party_pgr_reg_rspec) } },
    { "party_glue_reg", { &Pmarb_rspec_base->party_glue_reg, new Party_glue_reg_rspec_map, {}, sizeof(Party_glue_reg_rspec) } },
    { "ebp18_reg", { &Pmarb_rspec_base->ebp18_reg, new Ebp18_rspec_map, {}, sizeof(Ebp18_rspec) } }
    } )
  {}
};

struct Dprsr_phv_csum_cfg_entry_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_phv_csum_cfg_entry_g Dprsr_phv_csum_cfg_entry_g_base=0;
  Dprsr_phv_csum_cfg_entry_g_map() : RegisterMapper( {
    { "csum_cfg_entry", { const_cast<uint32_t(*)[0x120]>(&Dprsr_phv_csum_cfg_entry_g_base->csum_cfg_entry), 0, {0x120}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_i_phv_csum_map_map: public RegisterMapper {
  static constexpr PTR_Dprsr_i_phv_csum_map Dprsr_i_phv_csum_map_base=0;
  Dprsr_i_phv_csum_map_map() : RegisterMapper( {
    { "csum_cfg", { &Dprsr_i_phv_csum_map_base->csum_cfg, new Dprsr_phv_csum_cfg_entry_g_map, {0x6}, sizeof(Dprsr_phv_csum_cfg_entry_g) } }
    } )
  {}
};

struct Dprsr_i_fde_pov_map_map: public RegisterMapper {
  static constexpr PTR_Dprsr_i_fde_pov_map Dprsr_i_fde_pov_map_base=0;
  Dprsr_i_fde_pov_map_map() : RegisterMapper( {
    { "fde_pov", { const_cast<uint32_t(*)[0xc0]>(&Dprsr_i_fde_pov_map_base->fde_pov), 0, {0xc0}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ii_mem_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ii_mem Dprsr_ii_mem_base=0;
  Dprsr_ii_mem_map() : RegisterMapper( {
    { "ii_phv_csum", { &Dprsr_ii_mem_base->ii_phv_csum, new Dprsr_i_phv_csum_map_map, {}, sizeof(Dprsr_i_phv_csum_map) } },
    { "ii_fde_pov", { &Dprsr_ii_mem_base->ii_fde_pov, new Dprsr_i_fde_pov_map_map, {}, sizeof(Dprsr_i_fde_pov_map) } },
    { "scratch", { const_cast<uint32_t(*)>(&Dprsr_ii_mem_base->scratch), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ie_mem_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ie_mem Dprsr_ie_mem_base=0;
  Dprsr_ie_mem_map() : RegisterMapper( {
    { "ie_phv_csum", { &Dprsr_ie_mem_base->ie_phv_csum, new Dprsr_i_phv_csum_map_map, {}, sizeof(Dprsr_i_phv_csum_map) } },
    { "ie_fde_pov", { &Dprsr_ie_mem_base->ie_fde_pov, new Dprsr_i_fde_pov_map_map, {}, sizeof(Dprsr_i_fde_pov_map) } },
    { "scratch", { const_cast<uint32_t(*)>(&Dprsr_ie_mem_base->scratch), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_pov_position_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_pov_position_r Dprsr_pov_position_r_base=0;
  Dprsr_pov_position_r_map() : RegisterMapper( {
    { "pov_0_8", { const_cast<uint32_t(*)>(&Dprsr_pov_position_r_base->pov_0_8), 0, {}, sizeof(uint32_t) } },
    { "pov_1_8", { const_cast<uint32_t(*)>(&Dprsr_pov_position_r_base->pov_1_8), 0, {}, sizeof(uint32_t) } },
    { "pov_2_8", { const_cast<uint32_t(*)>(&Dprsr_pov_position_r_base->pov_2_8), 0, {}, sizeof(uint32_t) } },
    { "pov_3_8", { const_cast<uint32_t(*)>(&Dprsr_pov_position_r_base->pov_3_8), 0, {}, sizeof(uint32_t) } },
    { "pov_4_8", { const_cast<uint32_t(*)>(&Dprsr_pov_position_r_base->pov_4_8), 0, {}, sizeof(uint32_t) } },
    { "pov_5_8", { const_cast<uint32_t(*)>(&Dprsr_pov_position_r_base->pov_5_8), 0, {}, sizeof(uint32_t) } },
    { "pov_6_8", { const_cast<uint32_t(*)>(&Dprsr_pov_position_r_base->pov_6_8), 0, {}, sizeof(uint32_t) } },
    { "pov_7_8", { const_cast<uint32_t(*)>(&Dprsr_pov_position_r_base->pov_7_8), 0, {}, sizeof(uint32_t) } }
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

struct Dprsr_cnt_vld_i_tphv_map: public RegisterMapper {
  static constexpr PTR_Dprsr_cnt_vld_i_tphv Dprsr_cnt_vld_i_tphv_base=0;
  Dprsr_cnt_vld_i_tphv_map() : RegisterMapper( {
    { "cnt_i_tphv_0_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_vld_i_tphv_base->cnt_i_tphv_0_2), 0, {}, sizeof(uint32_t) } },
    { "cnt_i_tphv_1_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_vld_i_tphv_base->cnt_i_tphv_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_input_ing_and_egr_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_input_ing_and_egr_g Dprsr_input_ing_and_egr_g_base=0;
  Dprsr_input_ing_and_egr_g_map() : RegisterMapper( {
    { "pov", { const_cast<Dprsr_pov_position_r(*)>(&Dprsr_input_ing_and_egr_g_base->pov), new Dprsr_pov_position_r_map, {}, sizeof(Dprsr_pov_position_r) } },
    { "egress_unicast_port", { const_cast<uint32_t(*)>(&Dprsr_input_ing_and_egr_g_base->egress_unicast_port), 0, {}, sizeof(uint32_t) } },
    { "cnt_i_phv", { const_cast<Dprsr_cnt_vld_i_phv(*)>(&Dprsr_input_ing_and_egr_g_base->cnt_i_phv), new Dprsr_cnt_vld_i_phv_map, {}, sizeof(Dprsr_cnt_vld_i_phv) } },
    { "cnt_i_tphv", { const_cast<Dprsr_cnt_vld_i_tphv(*)>(&Dprsr_input_ing_and_egr_g_base->cnt_i_tphv), new Dprsr_cnt_vld_i_tphv_map, {}, sizeof(Dprsr_cnt_vld_i_tphv) } },
    { "drop_ctl", { const_cast<uint32_t(*)>(&Dprsr_input_ing_and_egr_g_base->drop_ctl), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_resubmit_table_entry_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_resubmit_table_entry_r Dprsr_resubmit_table_entry_r_base=0;
  Dprsr_resubmit_table_entry_r_map() : RegisterMapper( {
    { "resub_tbl_0_3", { const_cast<uint32_t(*)>(&Dprsr_resubmit_table_entry_r_base->resub_tbl_0_3), 0, {}, sizeof(uint32_t) } },
    { "resub_tbl_1_3", { const_cast<uint32_t(*)>(&Dprsr_resubmit_table_entry_r_base->resub_tbl_1_3), 0, {}, sizeof(uint32_t) } },
    { "resub_tbl_2_3", { const_cast<uint32_t(*)>(&Dprsr_resubmit_table_entry_r_base->resub_tbl_2_3), 0, {}, sizeof(uint32_t) } }
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

struct Dprsr_cnt_i_dec_read_map: public RegisterMapper {
  static constexpr PTR_Dprsr_cnt_i_dec_read Dprsr_cnt_i_dec_read_base=0;
  Dprsr_cnt_i_dec_read_map() : RegisterMapper( {
    { "cnt_i_read_0_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_i_dec_read_base->cnt_i_read_0_2), 0, {}, sizeof(uint32_t) } },
    { "cnt_i_read_1_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_i_dec_read_base->cnt_i_read_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_cnt_i_dec_discard_map: public RegisterMapper {
  static constexpr PTR_Dprsr_cnt_i_dec_discard Dprsr_cnt_i_dec_discard_base=0;
  Dprsr_cnt_i_dec_discard_map() : RegisterMapper( {
    { "cnt_i_discard_0_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_i_dec_discard_base->cnt_i_discard_0_2), 0, {}, sizeof(uint32_t) } },
    { "cnt_i_discard_1_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_i_dec_discard_base->cnt_i_discard_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_cnt_i_dec_resubmit_map: public RegisterMapper {
  static constexpr PTR_Dprsr_cnt_i_dec_resubmit Dprsr_cnt_i_dec_resubmit_base=0;
  Dprsr_cnt_i_dec_resubmit_map() : RegisterMapper( {
    { "cnt_i_resubmit_0_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_i_dec_resubmit_base->cnt_i_resubmit_0_2), 0, {}, sizeof(uint32_t) } },
    { "cnt_i_resubmit_1_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_i_dec_resubmit_base->cnt_i_resubmit_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_cnt_i_dec_learn_map: public RegisterMapper {
  static constexpr PTR_Dprsr_cnt_i_dec_learn Dprsr_cnt_i_dec_learn_base=0;
  Dprsr_cnt_i_dec_learn_map() : RegisterMapper( {
    { "cnt_i_learn_0_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_i_dec_learn_base->cnt_i_learn_0_2), 0, {}, sizeof(uint32_t) } },
    { "cnt_i_learn_1_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_i_dec_learn_base->cnt_i_learn_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_input_ingress_only_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_input_ingress_only_g Dprsr_input_ingress_only_g_base=0;
  Dprsr_input_ingress_only_g_map() : RegisterMapper( {
    { "copy_to_cpu", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->copy_to_cpu), 0, {}, sizeof(uint32_t) } },
    { "resub_cfg", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->resub_cfg), 0, {}, sizeof(uint32_t) } },
    { "resub_tbl", { const_cast<Dprsr_resubmit_table_entry_r(*)[0x8]>(&Dprsr_input_ingress_only_g_base->resub_tbl), new Dprsr_resubmit_table_entry_r_map, {0x8}, sizeof(Dprsr_resubmit_table_entry_r) } },
    { "learn_cfg", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->learn_cfg), 0, {}, sizeof(uint32_t) } },
    { "learn_tbl", { const_cast<Dprsr_learn_table_entry_r(*)[0x8]>(&Dprsr_input_ingress_only_g_base->learn_tbl), new Dprsr_learn_table_entry_r_map, {0x8}, sizeof(Dprsr_learn_table_entry_r) } },
    { "phv8_grp", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->phv8_grp), 0, {}, sizeof(uint32_t) } },
    { "phv8_split", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->phv8_split), 0, {}, sizeof(uint32_t) } },
    { "phv16_grp", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->phv16_grp), 0, {}, sizeof(uint32_t) } },
    { "phv16_split", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->phv16_split), 0, {}, sizeof(uint32_t) } },
    { "phv32_grp", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->phv32_grp), 0, {}, sizeof(uint32_t) } },
    { "phv32_split", { const_cast<uint32_t(*)>(&Dprsr_input_ingress_only_g_base->phv32_split), 0, {}, sizeof(uint32_t) } },
    { "cnt_i_read", { const_cast<Dprsr_cnt_i_dec_read(*)>(&Dprsr_input_ingress_only_g_base->cnt_i_read), new Dprsr_cnt_i_dec_read_map, {}, sizeof(Dprsr_cnt_i_dec_read) } },
    { "cnt_i_discard", { const_cast<Dprsr_cnt_i_dec_discard(*)>(&Dprsr_input_ingress_only_g_base->cnt_i_discard), new Dprsr_cnt_i_dec_discard_map, {}, sizeof(Dprsr_cnt_i_dec_discard) } },
    { "cnt_i_resubmit", { const_cast<Dprsr_cnt_i_dec_resubmit(*)>(&Dprsr_input_ingress_only_g_base->cnt_i_resubmit), new Dprsr_cnt_i_dec_resubmit_map, {}, sizeof(Dprsr_cnt_i_dec_resubmit) } },
    { "cnt_i_learn", { const_cast<Dprsr_cnt_i_dec_learn(*)>(&Dprsr_input_ingress_only_g_base->cnt_i_learn), new Dprsr_cnt_i_dec_learn_map, {}, sizeof(Dprsr_cnt_i_dec_learn) } }
    } )
  {}
};

struct Dprsr_input_ing_and_egr_ecc_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_input_ing_and_egr_ecc_g Dprsr_input_ing_and_egr_ecc_g_base=0;
  Dprsr_input_ing_and_egr_ecc_g_map() : RegisterMapper( {
    { "meta", { const_cast<uint32_t(*)>(&Dprsr_input_ing_and_egr_ecc_g_base->meta), 0, {}, sizeof(uint32_t) } },
    { "meta_sbe", { const_cast<uint32_t(*)>(&Dprsr_input_ing_and_egr_ecc_g_base->meta_sbe), 0, {}, sizeof(uint32_t) } },
    { "meta_mbe", { const_cast<uint32_t(*)>(&Dprsr_input_ing_and_egr_ecc_g_base->meta_mbe), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ii_regs_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ii_regs Dprsr_ii_regs_base=0;
  Dprsr_ii_regs_map() : RegisterMapper( {
    { "main_i", { &Dprsr_ii_regs_base->main_i, new Dprsr_input_ing_and_egr_g_map, {}, sizeof(Dprsr_input_ing_and_egr_g) } },
    { "ingr", { &Dprsr_ii_regs_base->ingr, new Dprsr_input_ingress_only_g_map, {}, sizeof(Dprsr_input_ingress_only_g) } },
    { "ecc_i", { &Dprsr_ii_regs_base->ecc_i, new Dprsr_input_ing_and_egr_ecc_g_map, {}, sizeof(Dprsr_input_ing_and_egr_ecc_g) } },
    { "mau_err_i", { const_cast<uint32_t(*)>(&Dprsr_ii_regs_base->mau_err_i), 0, {}, sizeof(uint32_t) } },
    { "hdr_too_long_i", { const_cast<uint32_t(*)>(&Dprsr_ii_regs_base->hdr_too_long_i), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_input_egress_only_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_input_egress_only_g Dprsr_input_egress_only_g_base=0;
  Dprsr_input_egress_only_g_map() : RegisterMapper( {
    { "phv8_grp", { const_cast<uint32_t(*)>(&Dprsr_input_egress_only_g_base->phv8_grp), 0, {}, sizeof(uint32_t) } },
    { "phv8_split", { const_cast<uint32_t(*)>(&Dprsr_input_egress_only_g_base->phv8_split), 0, {}, sizeof(uint32_t) } },
    { "phv16_grp", { const_cast<uint32_t(*)>(&Dprsr_input_egress_only_g_base->phv16_grp), 0, {}, sizeof(uint32_t) } },
    { "phv16_split", { const_cast<uint32_t(*)>(&Dprsr_input_egress_only_g_base->phv16_split), 0, {}, sizeof(uint32_t) } },
    { "phv32_grp", { const_cast<uint32_t(*)>(&Dprsr_input_egress_only_g_base->phv32_grp), 0, {}, sizeof(uint32_t) } },
    { "phv32_split", { const_cast<uint32_t(*)>(&Dprsr_input_egress_only_g_base->phv32_split), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ie_regs_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ie_regs Dprsr_ie_regs_base=0;
  Dprsr_ie_regs_map() : RegisterMapper( {
    { "main_e", { &Dprsr_ie_regs_base->main_e, new Dprsr_input_ing_and_egr_g_map, {}, sizeof(Dprsr_input_ing_and_egr_g) } },
    { "egr", { &Dprsr_ie_regs_base->egr, new Dprsr_input_egress_only_g_map, {}, sizeof(Dprsr_input_egress_only_g) } },
    { "ecc_e", { &Dprsr_ie_regs_base->ecc_e, new Dprsr_input_ing_and_egr_ecc_g_map, {}, sizeof(Dprsr_input_ing_and_egr_ecc_g) } },
    { "mau_err_e", { const_cast<uint32_t(*)>(&Dprsr_ie_regs_base->mau_err_e), 0, {}, sizeof(uint32_t) } },
    { "hdr_too_long_e", { const_cast<uint32_t(*)>(&Dprsr_ie_regs_base->hdr_too_long_e), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ic_regs_intr_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ic_regs_intr Dprsr_ic_regs_intr_base=0;
  Dprsr_ic_regs_intr_map() : RegisterMapper( {
    { "status", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dpr_i_dbg_r_map: public RegisterMapper {
  static constexpr PTR_Dpr_i_dbg_r Dpr_i_dbg_r_base=0;
  Dpr_i_dbg_r_map() : RegisterMapper( {
    { "i_dbg_0_2", { const_cast<uint32_t(*)>(&Dpr_i_dbg_r_base->i_dbg_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_dbg_1_2", { const_cast<uint32_t(*)>(&Dpr_i_dbg_r_base->i_dbg_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_ic_regs_map: public RegisterMapper {
  static constexpr PTR_Dprsr_ic_regs Dprsr_ic_regs_base=0;
  Dprsr_ic_regs_map() : RegisterMapper( {
    { "dft_csr", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "inp_cfg", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->inp_cfg), 0, {}, sizeof(uint32_t) } },
    { "tphv_cfg", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->tphv_cfg), 0, {}, sizeof(uint32_t) } },
    { "tphv_buff", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->tphv_buff), 0, {}, sizeof(uint32_t) } },
    { "tphv_buff_ecc_sbe", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->tphv_buff_ecc_sbe), 0, {}, sizeof(uint32_t) } },
    { "tphv_buff_ecc_mbe", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->tphv_buff_ecc_mbe), 0, {}, sizeof(uint32_t) } },
    { "phv8", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv8), 0, {}, sizeof(uint32_t) } },
    { "phv8_sbe", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv8_sbe), 0, {}, sizeof(uint32_t) } },
    { "phv8_mbe", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv8_mbe), 0, {}, sizeof(uint32_t) } },
    { "phv16", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv16), 0, {}, sizeof(uint32_t) } },
    { "phv16_sbe", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv16_sbe), 0, {}, sizeof(uint32_t) } },
    { "phv16_mbe", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv16_mbe), 0, {}, sizeof(uint32_t) } },
    { "phv32", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv32), 0, {}, sizeof(uint32_t) } },
    { "phv32_sbe", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv32_sbe), 0, {}, sizeof(uint32_t) } },
    { "phv32_mbe", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv32_mbe), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Dprsr_ic_regs_base->intr, new Dprsr_ic_regs_intr_map, {}, sizeof(Dprsr_ic_regs_intr) } },
    { "i_tphv_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_tphv_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "i_tphv_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_tphv_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "e_tphv_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_tphv_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "e_tphv_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_tphv_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "phv8_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv8_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "phv8_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv8_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "phv16_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv16_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "phv16_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv16_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "phv32_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv32_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "phv32_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv32_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "i_meta_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_meta_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "i_meta_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_meta_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "e_meta_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_meta_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "e_meta_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->e_meta_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "phv8_sbe_err", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv8_sbe_err), 0, {}, sizeof(uint32_t) } },
    { "phv8_mbe_err", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv8_mbe_err), 0, {}, sizeof(uint32_t) } },
    { "phv16_sbe_err", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv16_sbe_err), 0, {}, sizeof(uint32_t) } },
    { "phv16_mbe_err", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv16_mbe_err), 0, {}, sizeof(uint32_t) } },
    { "phv32_sbe_err", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv32_sbe_err), 0, {}, sizeof(uint32_t) } },
    { "phv32_mbe_err", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->phv32_mbe_err), 0, {}, sizeof(uint32_t) } },
    { "tphv_sbe_err", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->tphv_sbe_err), 0, {}, sizeof(uint32_t) } },
    { "tphv_mbe_err", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->tphv_mbe_err), 0, {}, sizeof(uint32_t) } },
    { "meta_sbe_err", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->meta_sbe_err), 0, {}, sizeof(uint32_t) } },
    { "meta_mbe_err", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->meta_mbe_err), 0, {}, sizeof(uint32_t) } },
    { "i_dbg", { const_cast<Dpr_i_dbg_r(*)>(&Dprsr_ic_regs_base->i_dbg), new Dpr_i_dbg_r_map, {}, sizeof(Dpr_i_dbg_r) } },
    { "i_diagbus_cfg", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_diagbus_cfg), 0, {}, sizeof(uint32_t) } },
    { "i_meta_fifo", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_meta_fifo), 0, {}, sizeof(uint32_t) } },
    { "i_delay_fifo", { const_cast<uint32_t(*)>(&Dprsr_ic_regs_base->i_delay_fifo), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_inp_map: public RegisterMapper {
  static constexpr PTR_Dprsr_inp Dprsr_inp_base=0;
  Dprsr_inp_map() : RegisterMapper( {
    { "iim", { &Dprsr_inp_base->iim, new Dprsr_ii_mem_map, {}, sizeof(Dprsr_ii_mem) } },
    { "iem", { &Dprsr_inp_base->iem, new Dprsr_ie_mem_map, {}, sizeof(Dprsr_ie_mem) } },
    { "iir", { &Dprsr_inp_base->iir, new Dprsr_ii_regs_map, {}, sizeof(Dprsr_ii_regs) } },
    { "ier", { &Dprsr_inp_base->ier, new Dprsr_ie_regs_map, {}, sizeof(Dprsr_ie_regs) } },
    { "icr", { &Dprsr_inp_base->icr, new Dprsr_ic_regs_map, {}, sizeof(Dprsr_ic_regs) } }
    } )
  {}
};

struct Dprsr_cnt_oi_pkts_map: public RegisterMapper {
  static constexpr PTR_Dprsr_cnt_oi_pkts Dprsr_cnt_oi_pkts_base=0;
  Dprsr_cnt_oi_pkts_map() : RegisterMapper( {
    { "cnt_pkts_0_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_oi_pkts_base->cnt_pkts_0_2), 0, {}, sizeof(uint32_t) } },
    { "cnt_pkts_1_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_oi_pkts_base->cnt_pkts_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_CRC_cfg_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_CRC_cfg_r Dprsr_CRC_cfg_r_base=0;
  Dprsr_CRC_cfg_r_map() : RegisterMapper( {
    { "crc_zero_0_3", { const_cast<uint32_t(*)>(&Dprsr_CRC_cfg_r_base->crc_zero_0_3), 0, {}, sizeof(uint32_t) } },
    { "crc_zero_1_3", { const_cast<uint32_t(*)>(&Dprsr_CRC_cfg_r_base->crc_zero_1_3), 0, {}, sizeof(uint32_t) } },
    { "crc_zero_2_3", { const_cast<uint32_t(*)>(&Dprsr_CRC_cfg_r_base->crc_zero_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_CRC_chk_dis_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_CRC_chk_dis_r Dprsr_CRC_chk_dis_r_base=0;
  Dprsr_CRC_chk_dis_r_map() : RegisterMapper( {
    { "crc_chk_0_3", { const_cast<uint32_t(*)>(&Dprsr_CRC_chk_dis_r_base->crc_chk_0_3), 0, {}, sizeof(uint32_t) } },
    { "crc_chk_1_3", { const_cast<uint32_t(*)>(&Dprsr_CRC_chk_dis_r_base->crc_chk_1_3), 0, {}, sizeof(uint32_t) } },
    { "crc_chk_2_3", { const_cast<uint32_t(*)>(&Dprsr_CRC_chk_dis_r_base->crc_chk_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_prt_ge40G_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_prt_ge40G_r Dprsr_prt_ge40G_r_base=0;
  Dprsr_prt_ge40G_r_map() : RegisterMapper( {
    { "ctm_ch_rate_0_3", { const_cast<uint32_t(*)>(&Dprsr_prt_ge40G_r_base->ctm_ch_rate_0_3), 0, {}, sizeof(uint32_t) } },
    { "ctm_ch_rate_1_3", { const_cast<uint32_t(*)>(&Dprsr_prt_ge40G_r_base->ctm_ch_rate_1_3), 0, {}, sizeof(uint32_t) } },
    { "ctm_ch_rate_2_3", { const_cast<uint32_t(*)>(&Dprsr_prt_ge40G_r_base->ctm_ch_rate_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_CTM_ch_empty_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_CTM_ch_empty_r Dprsr_CTM_ch_empty_r_base=0;
  Dprsr_CTM_ch_empty_r_map() : RegisterMapper( {
    { "ctm_ch_fifo_0_3", { const_cast<uint32_t(*)>(&Dprsr_CTM_ch_empty_r_base->ctm_ch_fifo_0_3), 0, {}, sizeof(uint32_t) } },
    { "ctm_ch_fifo_1_3", { const_cast<uint32_t(*)>(&Dprsr_CTM_ch_empty_r_base->ctm_ch_fifo_1_3), 0, {}, sizeof(uint32_t) } },
    { "ctm_ch_fifo_2_3", { const_cast<uint32_t(*)>(&Dprsr_CTM_ch_empty_r_base->ctm_ch_fifo_2_3), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_oi_regs_intr_map: public RegisterMapper {
  static constexpr PTR_Dprsr_oi_regs_intr Dprsr_oi_regs_intr_base=0;
  Dprsr_oi_regs_intr_map() : RegisterMapper( {
    { "status", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_oi_regs_i_fwd_pkts_map: public RegisterMapper {
  static constexpr PTR_Dprsr_oi_regs_i_fwd_pkts Dprsr_oi_regs_i_fwd_pkts_base=0;
  Dprsr_oi_regs_i_fwd_pkts_map() : RegisterMapper( {
    { "i_fwd_pkts_0_2", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_i_fwd_pkts_base->i_fwd_pkts_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_fwd_pkts_1_2", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_i_fwd_pkts_base->i_fwd_pkts_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_oi_regs_i_disc_pkts_map: public RegisterMapper {
  static constexpr PTR_Dprsr_oi_regs_i_disc_pkts Dprsr_oi_regs_i_disc_pkts_base=0;
  Dprsr_oi_regs_i_disc_pkts_map() : RegisterMapper( {
    { "i_disc_pkts_0_2", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_i_disc_pkts_base->i_disc_pkts_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_disc_pkts_1_2", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_i_disc_pkts_base->i_disc_pkts_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_oi_regs_i_mirr_pkts_map: public RegisterMapper {
  static constexpr PTR_Dprsr_oi_regs_i_mirr_pkts Dprsr_oi_regs_i_mirr_pkts_base=0;
  Dprsr_oi_regs_i_mirr_pkts_map() : RegisterMapper( {
    { "i_mirr_pkts_0_2", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_i_mirr_pkts_base->i_mirr_pkts_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_mirr_pkts_1_2", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_i_mirr_pkts_base->i_mirr_pkts_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_oi_regs_map: public RegisterMapper {
  static constexpr PTR_Dprsr_oi_regs Dprsr_oi_regs_base=0;
  Dprsr_oi_regs_map() : RegisterMapper( {
    { "scratch", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "tm_idprsr_bucket_limit", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->tm_idprsr_bucket_limit), 0, {}, sizeof(uint32_t) } },
    { "mirror_wrr", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->mirror_wrr), 0, {}, sizeof(uint32_t) } },
    { "arb_ctrl", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->arb_ctrl), 0, {}, sizeof(uint32_t) } },
    { "cnt_pkts", { const_cast<Dprsr_cnt_oi_pkts(*)>(&Dprsr_oi_regs_base->cnt_pkts), new Dprsr_cnt_oi_pkts_map, {}, sizeof(Dprsr_cnt_oi_pkts) } },
    { "ipipe_id", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->ipipe_id), 0, {}, sizeof(uint32_t) } },
    { "epipe_id", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->epipe_id), 0, {}, sizeof(uint32_t) } },
    { "crc_zero", { const_cast<Dprsr_CRC_cfg_r(*)>(&Dprsr_oi_regs_base->crc_zero), new Dprsr_CRC_cfg_r_map, {}, sizeof(Dprsr_CRC_cfg_r) } },
    { "crc_chk", { const_cast<Dprsr_CRC_chk_dis_r(*)>(&Dprsr_oi_regs_base->crc_chk), new Dprsr_CRC_chk_dis_r_map, {}, sizeof(Dprsr_CRC_chk_dis_r) } },
    { "crc_gen_cfg", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->crc_gen_cfg), 0, {}, sizeof(uint32_t) } },
    { "tm_dbg_d", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->tm_dbg_d), 0, {}, sizeof(uint32_t) } },
    { "tm_dbg_sel", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->tm_dbg_sel), 0, {}, sizeof(uint32_t) } },
    { "tm_dbg_a", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->tm_dbg_a), 0, {}, sizeof(uint32_t) } },
    { "ctm_fifo_rst", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->ctm_fifo_rst), 0, {}, sizeof(uint32_t) } },
    { "ctm_ch_rate", { const_cast<Dprsr_prt_ge40G_r(*)>(&Dprsr_oi_regs_base->ctm_ch_rate), new Dprsr_prt_ge40G_r_map, {}, sizeof(Dprsr_prt_ge40G_r) } },
    { "ctm_ch_fifo", { const_cast<Dprsr_CTM_ch_empty_r(*)>(&Dprsr_oi_regs_base->ctm_ch_fifo), new Dprsr_CTM_ch_empty_r_map, {}, sizeof(Dprsr_CTM_ch_empty_r) } },
    { "ctm_fifo_size", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->ctm_fifo_size), 0, {}, sizeof(uint32_t) } },
    { "hdr_fifo_size", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->hdr_fifo_size), 0, {}, sizeof(uint32_t) } },
    { "arb_dbg_ctrl", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->arb_dbg_ctrl), 0, {}, sizeof(uint32_t) } },
    { "arb_dbg", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->arb_dbg), 0, {}, sizeof(uint32_t) } },
    { "ecc", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->ecc), 0, {}, sizeof(uint32_t) } },
    { "i_mirr_hdr", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->i_mirr_hdr), 0, {}, sizeof(uint32_t) } },
    { "i_mirr_hdr_sbe", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->i_mirr_hdr_sbe), 0, {}, sizeof(uint32_t) } },
    { "i_mirr_hdr_mbe", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->i_mirr_hdr_mbe), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Dprsr_oi_regs_base->intr, new Dprsr_oi_regs_intr_map, {}, sizeof(Dprsr_oi_regs_intr) } },
    { "i_mirr_hdr_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->i_mirr_hdr_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "i_mirr_hdr_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->i_mirr_hdr_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "pbs_req_ctrl_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->pbs_req_ctrl_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "pbs_req_ctrl_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->pbs_req_ctrl_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "pbs_req_data_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->pbs_req_data_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "pbs_req_data_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->pbs_req_data_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "pbs_resp_ctrl_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->pbs_resp_ctrl_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "pbs_resp_ctrl_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->pbs_resp_ctrl_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "pbs_resp_data_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->pbs_resp_data_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "pbs_resp_data_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->pbs_resp_data_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "dpr_int_summary", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->dpr_int_summary), 0, {}, sizeof(uint32_t) } },
    { "mirr_hdr_i_sbe_err", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->mirr_hdr_i_sbe_err), 0, {}, sizeof(uint32_t) } },
    { "mirr_hdr_i_mbe_err", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->mirr_hdr_i_mbe_err), 0, {}, sizeof(uint32_t) } },
    { "ctm_crc_err", { const_cast<uint32_t(*)[0x48]>(&Dprsr_oi_regs_base->ctm_crc_err), 0, {0x48}, sizeof(uint32_t) } },
    { "i_egr_pkt_err", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->i_egr_pkt_err), 0, {}, sizeof(uint32_t) } },
    { "i_ctm_pkt_err", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->i_ctm_pkt_err), 0, {}, sizeof(uint32_t) } },
    { "i_fwd_pkts", { const_cast<Dprsr_oi_regs_i_fwd_pkts(*)>(&Dprsr_oi_regs_base->i_fwd_pkts), new Dprsr_oi_regs_i_fwd_pkts_map, {}, sizeof(Dprsr_oi_regs_i_fwd_pkts) } },
    { "i_disc_pkts", { const_cast<Dprsr_oi_regs_i_disc_pkts(*)>(&Dprsr_oi_regs_base->i_disc_pkts), new Dprsr_oi_regs_i_disc_pkts_map, {}, sizeof(Dprsr_oi_regs_i_disc_pkts) } },
    { "i_mirr_pkts", { const_cast<Dprsr_oi_regs_i_mirr_pkts(*)>(&Dprsr_oi_regs_base->i_mirr_pkts), new Dprsr_oi_regs_i_mirr_pkts_map, {}, sizeof(Dprsr_oi_regs_i_mirr_pkts) } },
    { "i_metadata", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->i_metadata), 0, {}, sizeof(uint32_t) } },
    { "i_metadata_sbe", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->i_metadata_sbe), 0, {}, sizeof(uint32_t) } },
    { "i_metadata_mbe", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->i_metadata_mbe), 0, {}, sizeof(uint32_t) } },
    { "i_metadata_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->i_metadata_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "i_metadata_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->i_metadata_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "diagbus_cfg", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->diagbus_cfg), 0, {}, sizeof(uint32_t) } },
    { "mstr_diagbus_cfg", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->mstr_diagbus_cfg), 0, {}, sizeof(uint32_t) } },
    { "mirr_tm_forced_crc_err", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->mirr_tm_forced_crc_err), 0, {}, sizeof(uint32_t) } },
    { "ctm_fcu_cfg", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->ctm_fcu_cfg), 0, {}, sizeof(uint32_t) } },
    { "mirr_tb_cfg", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->mirr_tb_cfg), 0, {}, sizeof(uint32_t) } },
    { "offset_beyond_pkt_err", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->offset_beyond_pkt_err), 0, {}, sizeof(uint32_t) } },
    { "edf_min_thresh", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->edf_min_thresh), 0, {}, sizeof(uint32_t) } },
    { "edf_hi_thresh", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->edf_hi_thresh), 0, {}, sizeof(uint32_t) } },
    { "load_interval", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->load_interval), 0, {}, sizeof(uint32_t) } },
    { "ctm_sop_thresh", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->ctm_sop_thresh), 0, {}, sizeof(uint32_t) } },
    { "hdr_sop_thresh", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->hdr_sop_thresh), 0, {}, sizeof(uint32_t) } },
    { "edf_cfg", { const_cast<uint32_t(*)>(&Dprsr_oi_regs_base->edf_cfg), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_h_edf_cfg_map_map: public RegisterMapper {
  static constexpr PTR_Dprsr_h_edf_cfg_map Dprsr_h_edf_cfg_map_base=0;
  Dprsr_h_edf_cfg_map_map() : RegisterMapper( {
    { "edf_inc", { const_cast<uint32_t(*)[0x48]>(&Dprsr_h_edf_cfg_map_base->edf_inc), 0, {0x48}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_oi_mem_map: public RegisterMapper {
  static constexpr PTR_Dprsr_oi_mem Dprsr_oi_mem_base=0;
  Dprsr_oi_mem_map() : RegisterMapper( {
    { "edf_cfg", { &Dprsr_oi_mem_base->edf_cfg, new Dprsr_h_edf_cfg_map_map, {}, sizeof(Dprsr_h_edf_cfg_map) } }
    } )
  {}
};

struct Dprsr_out_ingr_map: public RegisterMapper {
  static constexpr PTR_Dprsr_out_ingr Dprsr_out_ingr_base=0;
  Dprsr_out_ingr_map() : RegisterMapper( {
    { "regs", { &Dprsr_out_ingr_base->regs, new Dprsr_oi_regs_map, {}, sizeof(Dprsr_oi_regs) } },
    { "mem", { &Dprsr_out_ingr_base->mem, new Dprsr_oi_mem_map, {}, sizeof(Dprsr_oi_mem) } }
    } )
  {}
};

struct Dprsr_cnt_oe_pkts_map: public RegisterMapper {
  static constexpr PTR_Dprsr_cnt_oe_pkts Dprsr_cnt_oe_pkts_base=0;
  Dprsr_cnt_oe_pkts_map() : RegisterMapper( {
    { "cnt_pkts_0_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_oe_pkts_base->cnt_pkts_0_2), 0, {}, sizeof(uint32_t) } },
    { "cnt_pkts_1_2", { const_cast<uint32_t(*)>(&Dprsr_cnt_oe_pkts_base->cnt_pkts_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_oe_regs_intr_map: public RegisterMapper {
  static constexpr PTR_Dprsr_oe_regs_intr Dprsr_oe_regs_intr_base=0;
  Dprsr_oe_regs_intr_map() : RegisterMapper( {
    { "status", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_oe_regs_e_fwd_pkts_map: public RegisterMapper {
  static constexpr PTR_Dprsr_oe_regs_e_fwd_pkts Dprsr_oe_regs_e_fwd_pkts_base=0;
  Dprsr_oe_regs_e_fwd_pkts_map() : RegisterMapper( {
    { "e_fwd_pkts_0_2", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_e_fwd_pkts_base->e_fwd_pkts_0_2), 0, {}, sizeof(uint32_t) } },
    { "e_fwd_pkts_1_2", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_e_fwd_pkts_base->e_fwd_pkts_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_oe_regs_e_disc_pkts_map: public RegisterMapper {
  static constexpr PTR_Dprsr_oe_regs_e_disc_pkts Dprsr_oe_regs_e_disc_pkts_base=0;
  Dprsr_oe_regs_e_disc_pkts_map() : RegisterMapper( {
    { "e_disc_pkts_0_2", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_e_disc_pkts_base->e_disc_pkts_0_2), 0, {}, sizeof(uint32_t) } },
    { "e_disc_pkts_1_2", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_e_disc_pkts_base->e_disc_pkts_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_oe_regs_e_mirr_pkts_map: public RegisterMapper {
  static constexpr PTR_Dprsr_oe_regs_e_mirr_pkts Dprsr_oe_regs_e_mirr_pkts_base=0;
  Dprsr_oe_regs_e_mirr_pkts_map() : RegisterMapper( {
    { "e_mirr_pkts_0_2", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_e_mirr_pkts_base->e_mirr_pkts_0_2), 0, {}, sizeof(uint32_t) } },
    { "e_mirr_pkts_1_2", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_e_mirr_pkts_base->e_mirr_pkts_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_oe_regs_map: public RegisterMapper {
  static constexpr PTR_Dprsr_oe_regs Dprsr_oe_regs_base=0;
  Dprsr_oe_regs_map() : RegisterMapper( {
    { "dft_csr", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "arb_ctrl", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->arb_ctrl), 0, {}, sizeof(uint32_t) } },
    { "cnt_pkts", { const_cast<Dprsr_cnt_oe_pkts(*)>(&Dprsr_oe_regs_base->cnt_pkts), new Dprsr_cnt_oe_pkts_map, {}, sizeof(Dprsr_cnt_oe_pkts) } },
    { "ebuf_ctrl", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->ebuf_ctrl), 0, {}, sizeof(uint32_t) } },
    { "crc_zero", { const_cast<Dprsr_CRC_cfg_r(*)>(&Dprsr_oe_regs_base->crc_zero), new Dprsr_CRC_cfg_r_map, {}, sizeof(Dprsr_CRC_cfg_r) } },
    { "crc_chk", { const_cast<Dprsr_CRC_chk_dis_r(*)>(&Dprsr_oe_regs_base->crc_chk), new Dprsr_CRC_chk_dis_r_map, {}, sizeof(Dprsr_CRC_chk_dis_r) } },
    { "crc_gen_cfg", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->crc_gen_cfg), 0, {}, sizeof(uint32_t) } },
    { "ctm_ch_fifo", { const_cast<Dprsr_CTM_ch_empty_r(*)>(&Dprsr_oe_regs_base->ctm_ch_fifo), new Dprsr_CTM_ch_empty_r_map, {}, sizeof(Dprsr_CTM_ch_empty_r) } },
    { "byte_adj", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->byte_adj), 0, {}, sizeof(uint32_t) } },
    { "arb_dbg_ctrl", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->arb_dbg_ctrl), 0, {}, sizeof(uint32_t) } },
    { "arb_dbg", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->arb_dbg), 0, {}, sizeof(uint32_t) } },
    { "e_mirr_hdr", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->e_mirr_hdr), 0, {}, sizeof(uint32_t) } },
    { "e_mirr_hdr_sbe", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->e_mirr_hdr_sbe), 0, {}, sizeof(uint32_t) } },
    { "e_mirr_hdr_mbe", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->e_mirr_hdr_mbe), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Dprsr_oe_regs_base->intr, new Dprsr_oe_regs_intr_map, {}, sizeof(Dprsr_oe_regs_intr) } },
    { "e_mirr_hdr_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->e_mirr_hdr_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "e_mirr_hdr_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->e_mirr_hdr_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "mirr_hdr_e_sbe_err", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->mirr_hdr_e_sbe_err), 0, {}, sizeof(uint32_t) } },
    { "mirr_hdr_e_mbe_err", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->mirr_hdr_e_mbe_err), 0, {}, sizeof(uint32_t) } },
    { "ctm_crc_err", { const_cast<uint32_t(*)[0x48]>(&Dprsr_oe_regs_base->ctm_crc_err), 0, {0x48}, sizeof(uint32_t) } },
    { "e_egr_pkt_err", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->e_egr_pkt_err), 0, {}, sizeof(uint32_t) } },
    { "e_ctm_pkt_err", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->e_ctm_pkt_err), 0, {}, sizeof(uint32_t) } },
    { "e_fwd_pkts", { const_cast<Dprsr_oe_regs_e_fwd_pkts(*)>(&Dprsr_oe_regs_base->e_fwd_pkts), new Dprsr_oe_regs_e_fwd_pkts_map, {}, sizeof(Dprsr_oe_regs_e_fwd_pkts) } },
    { "e_disc_pkts", { const_cast<Dprsr_oe_regs_e_disc_pkts(*)>(&Dprsr_oe_regs_base->e_disc_pkts), new Dprsr_oe_regs_e_disc_pkts_map, {}, sizeof(Dprsr_oe_regs_e_disc_pkts) } },
    { "e_mirr_pkts", { const_cast<Dprsr_oe_regs_e_mirr_pkts(*)>(&Dprsr_oe_regs_base->e_mirr_pkts), new Dprsr_oe_regs_e_mirr_pkts_map, {}, sizeof(Dprsr_oe_regs_e_mirr_pkts) } },
    { "diagbus_cfg", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->diagbus_cfg), 0, {}, sizeof(uint32_t) } },
    { "ctm_fcu_cfg", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->ctm_fcu_cfg), 0, {}, sizeof(uint32_t) } },
    { "offset_beyond_pkt_err", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->offset_beyond_pkt_err), 0, {}, sizeof(uint32_t) } },
    { "edf_min_thresh", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->edf_min_thresh), 0, {}, sizeof(uint32_t) } },
    { "edf_hi_thresh", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->edf_hi_thresh), 0, {}, sizeof(uint32_t) } },
    { "load_interval", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->load_interval), 0, {}, sizeof(uint32_t) } },
    { "ctm_sop_thresh", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->ctm_sop_thresh), 0, {}, sizeof(uint32_t) } },
    { "hdr_sop_thresh", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->hdr_sop_thresh), 0, {}, sizeof(uint32_t) } },
    { "edf_cfg", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->edf_cfg), 0, {}, sizeof(uint32_t) } },
    { "edf_ctr_limit", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->edf_ctr_limit), 0, {}, sizeof(uint32_t) } },
    { "edf_water", { const_cast<uint32_t(*)>(&Dprsr_oe_regs_base->edf_water), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_oe_cred_cfg_map_map: public RegisterMapper {
  static constexpr PTR_Dprsr_oe_cred_cfg_map Dprsr_oe_cred_cfg_map_base=0;
  Dprsr_oe_cred_cfg_map_map() : RegisterMapper( {
    { "credits", { const_cast<uint32_t(*)[0x48]>(&Dprsr_oe_cred_cfg_map_base->credits), 0, {0x48}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_oe_mem_map: public RegisterMapper {
  static constexpr PTR_Dprsr_oe_mem Dprsr_oe_mem_base=0;
  Dprsr_oe_mem_map() : RegisterMapper( {
    { "edf_cfg", { &Dprsr_oe_mem_base->edf_cfg, new Dprsr_h_edf_cfg_map_map, {}, sizeof(Dprsr_h_edf_cfg_map) } },
    { "ebuf_cred_cfg", { &Dprsr_oe_mem_base->ebuf_cred_cfg, new Dprsr_oe_cred_cfg_map_map, {}, sizeof(Dprsr_oe_cred_cfg_map) } }
    } )
  {}
};

struct Dprsr_out_egr_map: public RegisterMapper {
  static constexpr PTR_Dprsr_out_egr Dprsr_out_egr_base=0;
  Dprsr_out_egr_map() : RegisterMapper( {
    { "regs", { &Dprsr_out_egr_base->regs, new Dprsr_oe_regs_map, {}, sizeof(Dprsr_oe_regs) } },
    { "mem", { &Dprsr_out_egr_base->mem, new Dprsr_oe_mem_map, {}, sizeof(Dprsr_oe_mem) } }
    } )
  {}
};

struct Mir_buf_desc_norm_desc_grp_map: public RegisterMapper {
  static constexpr PTR_Mir_buf_desc_norm_desc_grp Mir_buf_desc_norm_desc_grp_base=0;
  Mir_buf_desc_norm_desc_grp_map() : RegisterMapper( {
    { "session_meta0", { const_cast<uint32_t(*)>(&Mir_buf_desc_norm_desc_grp_base->session_meta0), 0, {}, sizeof(uint32_t) } },
    { "session_meta1", { const_cast<uint32_t(*)>(&Mir_buf_desc_norm_desc_grp_base->session_meta1), 0, {}, sizeof(uint32_t) } },
    { "session_meta2", { const_cast<uint32_t(*)>(&Mir_buf_desc_norm_desc_grp_base->session_meta2), 0, {}, sizeof(uint32_t) } },
    { "session_meta3", { const_cast<uint32_t(*)>(&Mir_buf_desc_norm_desc_grp_base->session_meta3), 0, {}, sizeof(uint32_t) } },
    { "session_meta4", { const_cast<uint32_t(*)>(&Mir_buf_desc_norm_desc_grp_base->session_meta4), 0, {}, sizeof(uint32_t) } },
    { "session_ctrl", { const_cast<uint32_t(*)>(&Mir_buf_desc_norm_desc_grp_base->session_ctrl), 0, {}, sizeof(uint32_t) } },
    { "dummy1", { const_cast<uint32_t(*)>(&Mir_buf_desc_norm_desc_grp_base->dummy1), 0, {}, sizeof(uint32_t) } },
    { "dummy2", { const_cast<uint32_t(*)>(&Mir_buf_desc_norm_desc_grp_base->dummy2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mir_buf_desc_map: public RegisterMapper {
  static constexpr PTR_Mir_buf_desc Mir_buf_desc_base=0;
  Mir_buf_desc_map() : RegisterMapper( {
    { "norm_desc_grp", { &Mir_buf_desc_base->norm_desc_grp, new Mir_buf_desc_norm_desc_grp_map, {0x400}, sizeof(Mir_buf_desc_norm_desc_grp) } }
    } )
  {}
};

struct Mir_buf_regs_ingr_pktdrop_map: public RegisterMapper {
  static constexpr PTR_Mir_buf_regs_ingr_pktdrop Mir_buf_regs_ingr_pktdrop_base=0;
  Mir_buf_regs_ingr_pktdrop_map() : RegisterMapper( {
    { "ingr_pktdrop_0_2", { const_cast<uint32_t(*)>(&Mir_buf_regs_ingr_pktdrop_base->ingr_pktdrop_0_2), 0, {}, sizeof(uint32_t) } },
    { "ingr_pktdrop_1_2", { const_cast<uint32_t(*)>(&Mir_buf_regs_ingr_pktdrop_base->ingr_pktdrop_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mir_buf_regs_egr_pktdrop_map: public RegisterMapper {
  static constexpr PTR_Mir_buf_regs_egr_pktdrop Mir_buf_regs_egr_pktdrop_base=0;
  Mir_buf_regs_egr_pktdrop_map() : RegisterMapper( {
    { "egr_pktdrop_0_2", { const_cast<uint32_t(*)>(&Mir_buf_regs_egr_pktdrop_base->egr_pktdrop_0_2), 0, {}, sizeof(uint32_t) } },
    { "egr_pktdrop_1_2", { const_cast<uint32_t(*)>(&Mir_buf_regs_egr_pktdrop_base->egr_pktdrop_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mir_buf_regs_neg_pktdrop_map: public RegisterMapper {
  static constexpr PTR_Mir_buf_regs_neg_pktdrop Mir_buf_regs_neg_pktdrop_base=0;
  Mir_buf_regs_neg_pktdrop_map() : RegisterMapper( {
    { "neg_pktdrop_0_2", { const_cast<uint32_t(*)>(&Mir_buf_regs_neg_pktdrop_base->neg_pktdrop_0_2), 0, {}, sizeof(uint32_t) } },
    { "neg_pktdrop_1_2", { const_cast<uint32_t(*)>(&Mir_buf_regs_neg_pktdrop_base->neg_pktdrop_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mir_buf_regs_coal_pktdrop_map: public RegisterMapper {
  static constexpr PTR_Mir_buf_regs_coal_pktdrop Mir_buf_regs_coal_pktdrop_base=0;
  Mir_buf_regs_coal_pktdrop_map() : RegisterMapper( {
    { "coal_pktdrop_0_2", { const_cast<uint32_t(*)>(&Mir_buf_regs_coal_pktdrop_base->coal_pktdrop_0_2), 0, {}, sizeof(uint32_t) } },
    { "coal_pktdrop_1_2", { const_cast<uint32_t(*)>(&Mir_buf_regs_coal_pktdrop_base->coal_pktdrop_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mir_buf_regs_coal_pkterr_map: public RegisterMapper {
  static constexpr PTR_Mir_buf_regs_coal_pkterr Mir_buf_regs_coal_pkterr_base=0;
  Mir_buf_regs_coal_pkterr_map() : RegisterMapper( {
    { "coal_pkterr_0_2", { const_cast<uint32_t(*)>(&Mir_buf_regs_coal_pkterr_base->coal_pkterr_0_2), 0, {}, sizeof(uint32_t) } },
    { "coal_pkterr_1_2", { const_cast<uint32_t(*)>(&Mir_buf_regs_coal_pkterr_base->coal_pkterr_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mir_buf_regs_mir_glb_group_map: public RegisterMapper {
  static constexpr PTR_Mir_buf_regs_mir_glb_group Mir_buf_regs_mir_glb_group_base=0;
  Mir_buf_regs_mir_glb_group_map() : RegisterMapper( {
    { "glb_ctrl", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->glb_ctrl), 0, {}, sizeof(uint32_t) } },
    { "mir_watermark_drop", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->mir_watermark_drop), 0, {}, sizeof(uint32_t) } },
    { "neg_mirr_ctrl", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->neg_mirr_ctrl), 0, {}, sizeof(uint32_t) } },
    { "neg_mirr_wtmk", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->neg_mirr_wtmk), 0, {}, sizeof(uint32_t) } },
    { "coalescing_basetime", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->coalescing_basetime), 0, {}, sizeof(uint32_t) } },
    { "coalescing_baseid", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->coalescing_baseid), 0, {}, sizeof(uint32_t) } },
    { "session_fifonum", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->session_fifonum), 0, {}, sizeof(uint32_t) } },
    { "norm_bank_entries", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->norm_bank_entries), 0, {}, sizeof(uint32_t) } },
    { "tot_bank_entries", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->tot_bank_entries), 0, {}, sizeof(uint32_t) } },
    { "mir_int_stat", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->mir_int_stat), 0, {}, sizeof(uint32_t) } },
    { "mir_int_en", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->mir_int_en), 0, {}, sizeof(uint32_t) } },
    { "mir_int_pri", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->mir_int_pri), 0, {}, sizeof(uint32_t) } },
    { "mir_int_dual_inj", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->mir_int_dual_inj), 0, {}, sizeof(uint32_t) } },
    { "mir_int_sngl_inj", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->mir_int_sngl_inj), 0, {}, sizeof(uint32_t) } },
    { "mir_addr_err_dbuf", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->mir_addr_err_dbuf), 0, {}, sizeof(uint32_t) } },
    { "mir_addr_err_idesc", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->mir_addr_err_idesc), 0, {}, sizeof(uint32_t) } },
    { "mir_addr_err_edesc", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->mir_addr_err_edesc), 0, {}, sizeof(uint32_t) } },
    { "mir_addr_err_odesc", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->mir_addr_err_odesc), 0, {}, sizeof(uint32_t) } },
    { "mir_addr_err_ptrff", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->mir_addr_err_ptrff), 0, {}, sizeof(uint32_t) } },
    { "disable_mir_err", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->disable_mir_err), 0, {}, sizeof(uint32_t) } },
    { "stall_drop_stat", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->stall_drop_stat), 0, {}, sizeof(uint32_t) } },
    { "ingr_pktdrop", { const_cast<Mir_buf_regs_ingr_pktdrop(*)>(&Mir_buf_regs_mir_glb_group_base->ingr_pktdrop), new Mir_buf_regs_ingr_pktdrop_map, {}, sizeof(Mir_buf_regs_ingr_pktdrop) } },
    { "egr_pktdrop", { const_cast<Mir_buf_regs_egr_pktdrop(*)>(&Mir_buf_regs_mir_glb_group_base->egr_pktdrop), new Mir_buf_regs_egr_pktdrop_map, {}, sizeof(Mir_buf_regs_egr_pktdrop) } },
    { "neg_pktdrop", { const_cast<Mir_buf_regs_neg_pktdrop(*)>(&Mir_buf_regs_mir_glb_group_base->neg_pktdrop), new Mir_buf_regs_neg_pktdrop_map, {}, sizeof(Mir_buf_regs_neg_pktdrop) } },
    { "coal_pktdrop", { const_cast<Mir_buf_regs_coal_pktdrop(*)[0x8]>(&Mir_buf_regs_mir_glb_group_base->coal_pktdrop), new Mir_buf_regs_coal_pktdrop_map, {0x8}, sizeof(Mir_buf_regs_coal_pktdrop) } },
    { "coal_cred_used", { const_cast<uint32_t(*)[0x8]>(&Mir_buf_regs_mir_glb_group_base->coal_cred_used), 0, {0x8}, sizeof(uint32_t) } },
    { "negm_cred_used", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->negm_cred_used), 0, {}, sizeof(uint32_t) } },
    { "dft_csr", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->dft_csr), 0, {}, sizeof(uint32_t) } },
    { "mir_int_freeze", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->mir_int_freeze), 0, {}, sizeof(uint32_t) } },
    { "cred_ini", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->cred_ini), 0, {}, sizeof(uint32_t) } },
    { "coal_pkterr", { const_cast<Mir_buf_regs_coal_pkterr(*)[0x8]>(&Mir_buf_regs_mir_glb_group_base->coal_pkterr), new Mir_buf_regs_coal_pkterr_map, {0x8}, sizeof(Mir_buf_regs_coal_pkterr) } },
    { "odesc_init_done", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->odesc_init_done), 0, {}, sizeof(uint32_t) } },
    { "dbg_ctrl", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->dbg_ctrl), 0, {}, sizeof(uint32_t) } },
    { "scratch", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->scratch), 0, {}, sizeof(uint32_t) } },
    { "min_bcnt", { const_cast<uint32_t(*)>(&Mir_buf_regs_mir_glb_group_base->min_bcnt), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mir_buf_regs_coal_desc_grp_map: public RegisterMapper {
  static constexpr PTR_Mir_buf_regs_coal_desc_grp Mir_buf_regs_coal_desc_grp_base=0;
  Mir_buf_regs_coal_desc_grp_map() : RegisterMapper( {
    { "coal_ctrl0", { const_cast<uint32_t(*)>(&Mir_buf_regs_coal_desc_grp_base->coal_ctrl0), 0, {}, sizeof(uint32_t) } },
    { "coal_ctrl1", { const_cast<uint32_t(*)>(&Mir_buf_regs_coal_desc_grp_base->coal_ctrl1), 0, {}, sizeof(uint32_t) } },
    { "coal_pkt_header0", { const_cast<uint32_t(*)>(&Mir_buf_regs_coal_desc_grp_base->coal_pkt_header0), 0, {}, sizeof(uint32_t) } },
    { "coal_pkt_header1", { const_cast<uint32_t(*)>(&Mir_buf_regs_coal_desc_grp_base->coal_pkt_header1), 0, {}, sizeof(uint32_t) } },
    { "coal_pkt_header2", { const_cast<uint32_t(*)>(&Mir_buf_regs_coal_desc_grp_base->coal_pkt_header2), 0, {}, sizeof(uint32_t) } },
    { "coal_pkt_header3", { const_cast<uint32_t(*)>(&Mir_buf_regs_coal_desc_grp_base->coal_pkt_header3), 0, {}, sizeof(uint32_t) } },
    { "coal_ctrl2", { const_cast<uint32_t(*)>(&Mir_buf_regs_coal_desc_grp_base->coal_ctrl2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Mir_buf_regs_map: public RegisterMapper {
  static constexpr PTR_Mir_buf_regs Mir_buf_regs_base=0;
  Mir_buf_regs_map() : RegisterMapper( {
    { "mir_glb_group", { &Mir_buf_regs_base->mir_glb_group, new Mir_buf_regs_mir_glb_group_map, {}, sizeof(Mir_buf_regs_mir_glb_group) } },
    { "coal_desc_grp", { &Mir_buf_regs_base->coal_desc_grp, new Mir_buf_regs_coal_desc_grp_map, {0x8}, sizeof(Mir_buf_regs_coal_desc_grp) } }
    } )
  {}
};

struct Mir_buf_all_map: public RegisterMapper {
  static constexpr PTR_Mir_buf_all Mir_buf_all_base=0;
  Mir_buf_all_map() : RegisterMapper( {
    { "mir_buf_desc", { &Mir_buf_all_base->mir_buf_desc, new Mir_buf_desc_map, {}, sizeof(Mir_buf_desc) } },
    { "mir_buf_regs", { &Mir_buf_all_base->mir_buf_regs, new Mir_buf_regs_map, {}, sizeof(Mir_buf_regs) } }
    } )
  {}
};

struct Dprsr_mirror_table_entry_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_mirror_table_entry_r Dprsr_mirror_table_entry_r_base=0;
  Dprsr_mirror_table_entry_r_map() : RegisterMapper( {
    { "mirror_tbl_0_9", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->mirror_tbl_0_9), 0, {}, sizeof(uint32_t) } },
    { "mirror_tbl_1_9", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->mirror_tbl_1_9), 0, {}, sizeof(uint32_t) } },
    { "mirror_tbl_2_9", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->mirror_tbl_2_9), 0, {}, sizeof(uint32_t) } },
    { "mirror_tbl_3_9", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->mirror_tbl_3_9), 0, {}, sizeof(uint32_t) } },
    { "mirror_tbl_4_9", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->mirror_tbl_4_9), 0, {}, sizeof(uint32_t) } },
    { "mirror_tbl_5_9", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->mirror_tbl_5_9), 0, {}, sizeof(uint32_t) } },
    { "mirror_tbl_6_9", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->mirror_tbl_6_9), 0, {}, sizeof(uint32_t) } },
    { "mirror_tbl_7_9", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->mirror_tbl_7_9), 0, {}, sizeof(uint32_t) } },
    { "mirror_tbl_8_9", { const_cast<uint32_t(*)>(&Dprsr_mirror_table_entry_r_base->mirror_tbl_8_9), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_header_ing_and_egr_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_header_ing_and_egr_g Dprsr_header_ing_and_egr_g_base=0;
  Dprsr_header_ing_and_egr_g_map() : RegisterMapper( {
    { "mirror_cfg", { const_cast<uint32_t(*)>(&Dprsr_header_ing_and_egr_g_base->mirror_cfg), 0, {}, sizeof(uint32_t) } },
    { "mirror_tbl", { const_cast<Dprsr_mirror_table_entry_r(*)[0x8]>(&Dprsr_header_ing_and_egr_g_base->mirror_tbl), new Dprsr_mirror_table_entry_r_map, {0x8}, sizeof(Dprsr_mirror_table_entry_r) } }
    } )
  {}
};

struct Dprsr_header_ingress_only_g_intr_map: public RegisterMapper {
  static constexpr PTR_Dprsr_header_ingress_only_g_intr Dprsr_header_ingress_only_g_intr_base=0;
  Dprsr_header_ingress_only_g_intr_map() : RegisterMapper( {
    { "status", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_intr_base->status), 0, {}, sizeof(uint32_t) } },
    { "enable0", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_intr_base->enable0), 0, {}, sizeof(uint32_t) } },
    { "enable1", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_intr_base->enable1), 0, {}, sizeof(uint32_t) } },
    { "inject", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_intr_base->inject), 0, {}, sizeof(uint32_t) } },
    { "freeze_enable", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_intr_base->freeze_enable), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_header_ingress_only_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_header_ingress_only_g Dprsr_header_ingress_only_g_base=0;
  Dprsr_header_ingress_only_g_map() : RegisterMapper( {
    { "ingress_port", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->ingress_port), 0, {}, sizeof(uint32_t) } },
    { "egress_multicast_group", { const_cast<uint32_t(*)[0x2]>(&Dprsr_header_ingress_only_g_base->egress_multicast_group), 0, {0x2}, sizeof(uint32_t) } },
    { "hash_lag_ecmp_mcast", { const_cast<uint32_t(*)[0x2]>(&Dprsr_header_ingress_only_g_base->hash_lag_ecmp_mcast), 0, {0x2}, sizeof(uint32_t) } },
    { "copy_to_cpu_cos", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->copy_to_cpu_cos), 0, {}, sizeof(uint32_t) } },
    { "copy_to_cpu_pv", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->copy_to_cpu_pv), 0, {}, sizeof(uint32_t) } },
    { "deflect_on_drop", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->deflect_on_drop), 0, {}, sizeof(uint32_t) } },
    { "meter_color", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->meter_color), 0, {}, sizeof(uint32_t) } },
    { "icos", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->icos), 0, {}, sizeof(uint32_t) } },
    { "qid", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->qid), 0, {}, sizeof(uint32_t) } },
    { "xid", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->xid), 0, {}, sizeof(uint32_t) } },
    { "yid", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->yid), 0, {}, sizeof(uint32_t) } },
    { "rid", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->rid), 0, {}, sizeof(uint32_t) } },
    { "yid_tbl", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->yid_tbl), 0, {}, sizeof(uint32_t) } },
    { "bypss_egr", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->bypss_egr), 0, {}, sizeof(uint32_t) } },
    { "ct_disable", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->ct_disable), 0, {}, sizeof(uint32_t) } },
    { "ct_mcast", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->ct_mcast), 0, {}, sizeof(uint32_t) } },
    { "pvt_ecc_ctrl", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->pvt_ecc_ctrl), 0, {}, sizeof(uint32_t) } },
    { "intr", { &Dprsr_header_ingress_only_g_base->intr, new Dprsr_header_ingress_only_g_intr_map, {}, sizeof(Dprsr_header_ingress_only_g_intr) } },
    { "pv_tbl0_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->pv_tbl0_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "pv_tbl0_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->pv_tbl0_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "pv_tbl1_sbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->pv_tbl1_sbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "pv_tbl1_mbe_errlog", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->pv_tbl1_mbe_errlog), 0, {}, sizeof(uint32_t) } },
    { "pv_tbl_sbe_err", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->pv_tbl_sbe_err), 0, {}, sizeof(uint32_t) } },
    { "pv_tbl_mbe_err", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->pv_tbl_mbe_err), 0, {}, sizeof(uint32_t) } },
    { "edf_thresh", { const_cast<uint32_t(*)>(&Dprsr_header_ingress_only_g_base->edf_thresh), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_hi_regs_i_hdr_pkt_ctr_map: public RegisterMapper {
  static constexpr PTR_Dprsr_hi_regs_i_hdr_pkt_ctr Dprsr_hi_regs_i_hdr_pkt_ctr_base=0;
  Dprsr_hi_regs_i_hdr_pkt_ctr_map() : RegisterMapper( {
    { "i_hdr_pkt_ctr_0_2", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_i_hdr_pkt_ctr_base->i_hdr_pkt_ctr_0_2), 0, {}, sizeof(uint32_t) } },
    { "i_hdr_pkt_ctr_1_2", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_i_hdr_pkt_ctr_base->i_hdr_pkt_ctr_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_hi_regs_map: public RegisterMapper {
  static constexpr PTR_Dprsr_hi_regs Dprsr_hi_regs_base=0;
  Dprsr_hi_regs_map() : RegisterMapper( {
    { "main_i", { &Dprsr_hi_regs_base->main_i, new Dprsr_header_ing_and_egr_g_map, {}, sizeof(Dprsr_header_ing_and_egr_g) } },
    { "ingr", { &Dprsr_hi_regs_base->ingr, new Dprsr_header_ingress_only_g_map, {}, sizeof(Dprsr_header_ingress_only_g) } },
    { "i_hdr_cr_ctrl", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_base->i_hdr_cr_ctrl), 0, {}, sizeof(uint32_t) } },
    { "i_hdr_cr_status", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_base->i_hdr_cr_status), 0, {}, sizeof(uint32_t) } },
    { "i_hdr_pkt_ctr", { const_cast<Dprsr_hi_regs_i_hdr_pkt_ctr(*)>(&Dprsr_hi_regs_base->i_hdr_pkt_ctr), new Dprsr_hi_regs_i_hdr_pkt_ctr_map, {}, sizeof(Dprsr_hi_regs_i_hdr_pkt_ctr) } },
    { "i_hdr_dbg_ctrl", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_base->i_hdr_dbg_ctrl), 0, {}, sizeof(uint32_t) } },
    { "i_hdr_dbg", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_base->i_hdr_dbg), 0, {}, sizeof(uint32_t) } },
    { "h_diagbus_cfg", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_base->h_diagbus_cfg), 0, {}, sizeof(uint32_t) } },
    { "h_cr_cfg", { const_cast<uint32_t(*)>(&Dprsr_hi_regs_base->h_cr_cfg), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_header_egress_only_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_header_egress_only_g Dprsr_header_egress_only_g_base=0;
  Dprsr_header_egress_only_g_map() : RegisterMapper( {
    { "force_tx_err", { const_cast<uint32_t(*)>(&Dprsr_header_egress_only_g_base->force_tx_err), 0, {}, sizeof(uint32_t) } },
    { "capture_tx_ts", { const_cast<uint32_t(*)>(&Dprsr_header_egress_only_g_base->capture_tx_ts), 0, {}, sizeof(uint32_t) } },
    { "tx_pkt_has_offsets", { const_cast<uint32_t(*)>(&Dprsr_header_egress_only_g_base->tx_pkt_has_offsets), 0, {}, sizeof(uint32_t) } },
    { "ecos", { const_cast<uint32_t(*)>(&Dprsr_header_egress_only_g_base->ecos), 0, {}, sizeof(uint32_t) } },
    { "coal", { const_cast<uint32_t(*)>(&Dprsr_header_egress_only_g_base->coal), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_he_regs_e_hdr_pkt_ctr_map: public RegisterMapper {
  static constexpr PTR_Dprsr_he_regs_e_hdr_pkt_ctr Dprsr_he_regs_e_hdr_pkt_ctr_base=0;
  Dprsr_he_regs_e_hdr_pkt_ctr_map() : RegisterMapper( {
    { "e_hdr_pkt_ctr_0_2", { const_cast<uint32_t(*)>(&Dprsr_he_regs_e_hdr_pkt_ctr_base->e_hdr_pkt_ctr_0_2), 0, {}, sizeof(uint32_t) } },
    { "e_hdr_pkt_ctr_1_2", { const_cast<uint32_t(*)>(&Dprsr_he_regs_e_hdr_pkt_ctr_base->e_hdr_pkt_ctr_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_he_regs_map: public RegisterMapper {
  static constexpr PTR_Dprsr_he_regs Dprsr_he_regs_base=0;
  Dprsr_he_regs_map() : RegisterMapper( {
    { "main_e", { &Dprsr_he_regs_base->main_e, new Dprsr_header_ing_and_egr_g_map, {}, sizeof(Dprsr_header_ing_and_egr_g) } },
    { "egr", { &Dprsr_he_regs_base->egr, new Dprsr_header_egress_only_g_map, {}, sizeof(Dprsr_header_egress_only_g) } },
    { "e_hdr_cr_ctrl", { const_cast<uint32_t(*)>(&Dprsr_he_regs_base->e_hdr_cr_ctrl), 0, {}, sizeof(uint32_t) } },
    { "e_hdr_cr_status", { const_cast<uint32_t(*)>(&Dprsr_he_regs_base->e_hdr_cr_status), 0, {}, sizeof(uint32_t) } },
    { "e_hdr_pkt_ctr", { const_cast<Dprsr_he_regs_e_hdr_pkt_ctr(*)>(&Dprsr_he_regs_base->e_hdr_pkt_ctr), new Dprsr_he_regs_e_hdr_pkt_ctr_map, {}, sizeof(Dprsr_he_regs_e_hdr_pkt_ctr) } },
    { "e_hdr_dbg_ctrl", { const_cast<uint32_t(*)>(&Dprsr_he_regs_base->e_hdr_dbg_ctrl), 0, {}, sizeof(uint32_t) } },
    { "e_hdr_dbg", { const_cast<uint32_t(*)>(&Dprsr_he_regs_base->e_hdr_dbg), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_tphv_csum_cfg_entry_g_map: public RegisterMapper {
  static constexpr PTR_Dprsr_tphv_csum_cfg_entry_g Dprsr_tphv_csum_cfg_entry_g_base=0;
  Dprsr_tphv_csum_cfg_entry_g_map() : RegisterMapper( {
    { "csum_cfg_entry", { const_cast<uint32_t(*)[0x9c]>(&Dprsr_tphv_csum_cfg_entry_g_base->csum_cfg_entry), 0, {0x9c}, sizeof(uint32_t) } },
    { "zeros_as_ones", { const_cast<uint32_t(*)>(&Dprsr_tphv_csum_cfg_entry_g_base->zeros_as_ones), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_h_tphv_csum_map_map: public RegisterMapper {
  static constexpr PTR_Dprsr_h_tphv_csum_map Dprsr_h_tphv_csum_map_base=0;
  Dprsr_h_tphv_csum_map_map() : RegisterMapper( {
    { "csum_cfg", { &Dprsr_h_tphv_csum_map_base->csum_cfg, new Dprsr_tphv_csum_cfg_entry_g_map, {0x6}, sizeof(Dprsr_tphv_csum_cfg_entry_g) } }
    } )
  {}
};

struct Dprsr_fde_phv_r_map: public RegisterMapper {
  static constexpr PTR_Dprsr_fde_phv_r Dprsr_fde_phv_r_base=0;
  Dprsr_fde_phv_r_map() : RegisterMapper( {
    { "fde_phv_0_2", { const_cast<uint32_t(*)>(&Dprsr_fde_phv_r_base->fde_phv_0_2), 0, {}, sizeof(uint32_t) } },
    { "fde_phv_1_2", { const_cast<uint32_t(*)>(&Dprsr_fde_phv_r_base->fde_phv_1_2), 0, {}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_h_fde_phv_map_map: public RegisterMapper {
  static constexpr PTR_Dprsr_h_fde_phv_map Dprsr_h_fde_phv_map_base=0;
  Dprsr_h_fde_phv_map_map() : RegisterMapper( {
    { "fde_phv", { const_cast<Dprsr_fde_phv_r(*)[0xc0]>(&Dprsr_h_fde_phv_map_base->fde_phv), new Dprsr_fde_phv_r_map, {0xc0}, sizeof(Dprsr_fde_phv_r) } }
    } )
  {}
};

struct Dprsr_he_mem_map: public RegisterMapper {
  static constexpr PTR_Dprsr_he_mem Dprsr_he_mem_base=0;
  Dprsr_he_mem_map() : RegisterMapper( {
    { "he_tphv_csum", { &Dprsr_he_mem_base->he_tphv_csum, new Dprsr_h_tphv_csum_map_map, {}, sizeof(Dprsr_h_tphv_csum_map) } },
    { "he_fde_phv", { &Dprsr_he_mem_base->he_fde_phv, new Dprsr_h_fde_phv_map_map, {}, sizeof(Dprsr_h_fde_phv_map) } },
    { "he_edf_cfg", { &Dprsr_he_mem_base->he_edf_cfg, new Dprsr_h_edf_cfg_map_map, {}, sizeof(Dprsr_h_edf_cfg_map) } }
    } )
  {}
};

struct Dprsr_h_pv_table_map_map: public RegisterMapper {
  static constexpr PTR_Dprsr_h_pv_table_map Dprsr_h_pv_table_map_base=0;
  Dprsr_h_pv_table_map_map() : RegisterMapper( {
    { "tbl0", { const_cast<uint32_t(*)[0x2000]>(&Dprsr_h_pv_table_map_base->tbl0), 0, {0x2000}, sizeof(uint32_t) } },
    { "tbl1", { const_cast<uint32_t(*)[0x2000]>(&Dprsr_h_pv_table_map_base->tbl1), 0, {0x2000}, sizeof(uint32_t) } }
    } )
  {}
};

struct Dprsr_hi_mem_map: public RegisterMapper {
  static constexpr PTR_Dprsr_hi_mem Dprsr_hi_mem_base=0;
  Dprsr_hi_mem_map() : RegisterMapper( {
    { "hi_tphv_csum", { &Dprsr_hi_mem_base->hi_tphv_csum, new Dprsr_h_tphv_csum_map_map, {}, sizeof(Dprsr_h_tphv_csum_map) } },
    { "hi_fde_phv", { &Dprsr_hi_mem_base->hi_fde_phv, new Dprsr_h_fde_phv_map_map, {}, sizeof(Dprsr_h_fde_phv_map) } },
    { "hi_edf_cfg", { &Dprsr_hi_mem_base->hi_edf_cfg, new Dprsr_h_edf_cfg_map_map, {}, sizeof(Dprsr_h_edf_cfg_map) } },
    { "hi_pv_table", { &Dprsr_hi_mem_base->hi_pv_table, new Dprsr_h_pv_table_map_map, {}, sizeof(Dprsr_h_pv_table_map) } }
    } )
  {}
};

struct Dprsr_hdr_map: public RegisterMapper {
  static constexpr PTR_Dprsr_hdr Dprsr_hdr_base=0;
  Dprsr_hdr_map() : RegisterMapper( {
    { "hir", { &Dprsr_hdr_base->hir, new Dprsr_hi_regs_map, {}, sizeof(Dprsr_hi_regs) } },
    { "her", { &Dprsr_hdr_base->her, new Dprsr_he_regs_map, {}, sizeof(Dprsr_he_regs) } },
    { "hem", { &Dprsr_hdr_base->hem, new Dprsr_he_mem_map, {}, sizeof(Dprsr_he_mem) } },
    { "him", { &Dprsr_hdr_base->him, new Dprsr_hi_mem_map, {}, sizeof(Dprsr_hi_mem) } }
    } )
  {}
};

struct Dprsr_reg_rspec_map: public RegisterMapper {
  static constexpr PTR_Dprsr_reg_rspec Dprsr_reg_rspec_base=0;
  Dprsr_reg_rspec_map() : RegisterMapper( {
    { "inp", { &Dprsr_reg_rspec_base->inp, new Dprsr_inp_map, {}, sizeof(Dprsr_inp) } },
    { "out_ingr", { &Dprsr_reg_rspec_base->out_ingr, new Dprsr_out_ingr_map, {}, sizeof(Dprsr_out_ingr) } },
    { "out_egr", { &Dprsr_reg_rspec_base->out_egr, new Dprsr_out_egr_map, {}, sizeof(Dprsr_out_egr) } },
    { "mirror", { &Dprsr_reg_rspec_base->mirror, new Mir_buf_all_map, {}, sizeof(Mir_buf_all) } },
    { "hdr", { &Dprsr_reg_rspec_base->hdr, new Dprsr_hdr_map, {}, sizeof(Dprsr_hdr) } }
    } )
  {}
};

struct Pipe_addrmap_map: public RegisterMapper {
  static constexpr PTR_Pipe_addrmap Pipe_addrmap_base=0;
  Pipe_addrmap_map() : RegisterMapper( {
    { "mau", { &Pipe_addrmap_base->mau, new Mau_addrmap_map, {0xc}, sizeof(Mau_addrmap) } },
    { "pmarb", { &Pipe_addrmap_base->pmarb, new Pmarb_rspec_map, {}, sizeof(Pmarb_rspec) } },
    { "deparser", { &Pipe_addrmap_base->deparser, new Dprsr_reg_rspec_map, {}, sizeof(Dprsr_reg_rspec) } }
    } )
  {}
};

struct Tofino_map: public RegisterMapper {
  static constexpr PTR_Tofino Tofino_base=0;
  Tofino_map() : RegisterMapper( {
    { "device_select", { &Tofino_base->device_select, new Dvsl_addrmap_map, {}, sizeof(Dvsl_addrmap) } },
    { "ethgpiobr", { &Tofino_base->ethgpiobr, new Gpio_regs_map, {}, sizeof(Gpio_regs) } },
    { "ethgpiotl", { &Tofino_base->ethgpiotl, new Gpio_regs_map, {}, sizeof(Gpio_regs) } },
    { "pipes", { &Tofino_base->pipes, new Pipe_addrmap_map, {0x4}, sizeof(Pipe_addrmap) } }
    } )
  {}
};

} // namespace MODEL_CHIP_TEST_NAMESPACE
#endif
