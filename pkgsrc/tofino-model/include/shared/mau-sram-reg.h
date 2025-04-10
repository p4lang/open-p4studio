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

#ifndef _SHARED_MAU_SRAM_REG_
#define _SHARED_MAU_SRAM_REG_

#include <cstdint>
#include <bitvector.h>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <mau-sram-row.h>

// Reg defs auto-generated from Semifore
#include <register_includes/model_mem.h>
#include <register_includes/match_ram_vpn.h>
#include <register_includes/match_nibble_s1q0_enable.h>
#include <register_includes/match_next_table_bitpos.h>
#include <register_includes/match_mask_array.h>
#include <register_includes/match_bytemask_array.h>
#include <register_includes/match_nibble_s0q1_enable.h>
#include <register_includes/unit_ram_ctl.h>
#include <register_includes/ram_address_mux_ctl.h>
#include <register_includes/unitram_config.h>
#include <register_includes/row_action_nxtable_bus_drive.h>
#include <register_includes/adr_dist_tind_adr_xbar_ctl_array.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauSram;

  class MauSramReg : public MauObject {
 public:
    // Defined in rmt-config.cpp
    static bool kRelaxSramIngressCheck;
    static bool kRelaxSramBitposCheck;

    static constexpr int kType = RmtTypes::kRmtTypeMauSramReg;
    static constexpr int kDataBusWidth = MauDefs::kDataBusWidth;
    static constexpr int kLogicalRows = MauDefs::kLogicalRowsPerMau;
    static constexpr int kMatchEntries = MauDefs::kMatchesPerSram;

    MauSramReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
               int rowIndex, int colIndex, int sramIndex, MauSram *mauSram);
    virtual ~MauSramReg();

    void match_mask_write_callback(uint32_t i);
    void match_bytemask_write_callback(uint32_t i);
    void match_ram_vpn_write_callback();
    void tind_xbar_write_callback();
    void config_change_callback();
    void match_nibble_s0q1_enable_callback();
    void match_nibble_s1q0_enable_callback();

    bool is_ingress();
    bool is_egress();
    bool is_active();
    bool check_ingress_egress(bool ingress);
    int  get_write_data_mux();
    int  get_read_data_mux();
    int  get_search_bus();
    bool is_match_sram();
    int  get_match_result_buses();
    int  get_match_result_bus();
    int  get_nxtab_result_buses();
    int  get_nxtab_result_bus();
    bool use_match_result_bus(int bus);
    int  get_tind_addr_bus();
    bool is_tind_sram();
    int  get_tind_result_buses();
    int  get_tind_result_bus();
    bool use_tind_result_bus(int bus);
    int  get_result_bus();
    int  get_nxtab_bus();
    bool output_rd_data(BitVector<kDataBusWidth> *data);
    bool input_rd_data(BitVector<kDataBusWidth> *data);
    bool input_wr_data(BitVector<kDataBusWidth> *data);
    int  get_addr_mux();


    bool is_action_sram();
    uint32_t action_addr();
    uint32_t action_addr_base();

    bool oflo_check();
    uint32_t oflo_addr(uint8_t addrtype);

    bool is_synth2port_sram();

    bool is_stats_sram();
    uint32_t stats_addr();
    uint32_t stats_waddr();

    bool is_meter_sram();
    uint32_t meter_addr();
    uint32_t meter_waddr();

    bool is_selector_sram();
    //uint32_t selector_addr();
    //uint32_t selector_waddr();

    bool is_stateful_sram();
    //uint32_t stateful_addr();
    //uint32_t stateful_waddr();

    bool output_action_subword();
    int  get_ram_type_check();

    uint8_t match_enables();
    bool get_next_table_bitpos(const int which, uint32_t *bitpos);
    uint16_t get_vpn(int which_vpn);
    int  get_logical_table();
    int  get_logical_tcam();
    int  get_ram_type();
    bool matches_type_vpn_table(const int type, const int vpn, const int table);
    bool is_selector_action_addr();

    int  get_alu_type();
    int  get_alu_index();
    int  get_alu_logrow_index();
    int  get_alu_row_index();
    MauSramRow *get_alu_row();

    void reset_resources();



 private:
    DISALLOW_COPY_AND_ASSIGN(MauSramReg);  // XXX
    MauSram                                        *mau_sram_;
    register_classes::MatchMaskArray                match_mask_array_;
    register_classes::MatchBytemaskArray            match_bytemask_array_;
    register_classes::MatchNextTableBitpos          match_nexttable_bitpos_;
    register_classes::MatchNibbleS0q1Enable         match_nibble_s0q1_enable_;
    register_classes::MatchNibbleS1q0Enable         match_nibble_s1q0_enable_;
    register_classes::RamAddressMuxCtl              ram_address_mux_ctl_;
    register_classes::UnitRamCtl                    unit_ram_ctl_;
    register_classes::UnitramConfig                 unitram_config_;
    register_classes::MatchRamVpn                   match_ram_vpn_;
    register_classes::RowActionNxtableBusDrive      row_action_nxtable_bus_drive_;
    register_classes::AdrDistTindAdrXbarCtlArray   *adr_dist_tind_adr_xbar_ctl_array_;
    bool                                            config_verify_;
    bool                                            config_complain_;
    int                                             tind_buses_;
    uint16_t                                        unit_vpn_;
    BitVector<MauDefs::kSramWidth>                  match_mask_bv_;
  };
}
#endif // _SHARED_MAU_SRAM_REG_
