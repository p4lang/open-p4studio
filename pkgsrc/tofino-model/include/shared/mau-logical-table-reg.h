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

#ifndef _SHARED_MAU_LOGICAL_TABLE_REG_
#define _SHARED_MAU_LOGICAL_TABLE_REG_

#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>

// Reg defs auto-generated from Semifore
//#include <tofino/register_includes/exact_match_delay_config.h> // Removed with HyperDev
#include <register_includes/imem_table_addr_egress.h>
#include <register_includes/next_table_format_data.h>
#include <register_includes/next_table_map_en.h>
#include <register_includes/gateway_to_logicaltable_xbar_ctl.h>
#include <register_includes/gateway_inhibit_lut.h>
#include <register_includes/gateway_next_table_lut.h>
#include <register_includes/gateway_en.h>
#include <register_includes/next_table_map_data.h>
#include <register_includes/predication_ctl.h>
#include <register_includes/adr_dist_action_data_adr_icxbar_ctl.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauLogicalTable;

  class MauLogicalTableReg : public MauObject {

    static constexpr int kGatewayEntries = MauDefs::kGatewayTableEntries;

 public:
    MauLogicalTableReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                       int ltIndex, MauLogicalTable *mauLogicalTable);
    virtual ~MauLogicalTableReg();

    void     ingress_pred_callback();
    void     egress_pred_callback();

    bool     has_exact_match();
    bool     has_ternary_match();

    uint16_t get_action_logical_rows();
    bool     get_action_overflow();
    bool     get_action_overflow2_up();
    bool     get_action_overflow2_down();
    uint8_t  get_action_overflow_buses();
    uint8_t  get_tind_ram_data_size();

    bool gateway_is_enabled() {
      return (1 & (gateway_en_.gateway_en() >> lt_index_));
    }
    int gateway_table_row() {
      if (gateway_to_logicaltable_xbar_ctl_.enabled_4bit_muxctl_enable())
        return gateway_to_logicaltable_xbar_ctl_.enabled_4bit_muxctl_select() / 2;
      else
        return 0;
    }
    int gateway_table_which() {
      if (gateway_to_logicaltable_xbar_ctl_.enabled_4bit_muxctl_enable())
        return gateway_to_logicaltable_xbar_ctl_.enabled_4bit_muxctl_select() % 2;
      else
        return 0;
    }

    uint16_t gateway_get_next_table(bool hit,int hit_index) {
      if (hit) {
        RMT_ASSERT(hit_index>=0 && hit_index<kGatewayEntries);
        return gateway_next_table_lut_array_[hit_index].gateway_next_table_lut();
      }
      else {
        // miss - return last entry
        return gateway_next_table_lut_array_[kGatewayEntries].gateway_next_table_lut();
      }
    }
    bool gateway_get_inhibit(bool hit,int hit_index) {
      int all_inhibits = gateway_inhibit_lut_.gateway_inhibit_lut();
      if (hit) {
        RMT_ASSERT(hit_index>=0 && hit_index<kGatewayEntries);
        return (all_inhibits >> hit_index) & 1;
      }
      else {
        // miss - return last entry
        return (all_inhibits >> kGatewayEntries) & 1;
      }
    }

 private:
    int                                              lt_index_;
    MauLogicalTable                                 *mau_logical_table_;
    register_classes::ImemTableAddrEgress                imem_table_addr_egress_;
    register_classes::PredicationCtl                     ingress_predication_ctl_;
    register_classes::PredicationCtl                     egress_predication_ctl_;

    // register_classes::ExactMatchDelayConfig           exact_match_delay_config_; // Removed with Hyperdev
    register_classes::NextTableMapEn                     next_table_map_en_;
    register_classes::NextTableFormatData                next_table_format_data_;
    std::array<register_classes::NextTableMapData,2>     next_table_map_data_;
    register_classes::AdrDistActionDataAdrIcxbarCtl      adr_dist_action_data_adr_icxbar_ctl_;

    register_classes::GatewayToLogicaltableXbarCtl                      gateway_to_logicaltable_xbar_ctl_;
    std::array<register_classes::GatewayNextTableLut,kGatewayEntries+1> gateway_next_table_lut_array_;
    register_classes::GatewayInhibitLut                                 gateway_inhibit_lut_;
    register_classes::GatewayEn                                         gateway_en_; //enable
  };
}
#endif // _SHARED_MAU_LOGICAL_TABLE_REG_
