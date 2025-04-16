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

#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-logical-table-reg.h>
#include <mau-logical-table.h>
#include <register_adapters.h>



namespace MODEL_CHIP_NAMESPACE {

  MauLogicalTableReg::MauLogicalTableReg(RmtObjectManager *om, int pipeIndex, int mauIndex, int ltIndex,
                                         MauLogicalTable *mauLogicalTable)
      : MauObject(om, pipeIndex, mauIndex),
        lt_index_(ltIndex), mau_logical_table_(mauLogicalTable),
        imem_table_addr_egress_(default_adapter(imem_table_addr_egress_,chip_index(), pipeIndex, mauIndex)),
        ingress_predication_ctl_(default_adapter(ingress_predication_ctl_,chip_index(), pipeIndex, mauIndex, 0)),
        egress_predication_ctl_(default_adapter(egress_predication_ctl_,chip_index(), pipeIndex, mauIndex, 1)),

        //exact_match_delay_config_(chip_index(), pipeIndex, mauIndex), // Removed with Hyperdev
        next_table_map_en_(default_adapter(next_table_map_en_,chip_index(), pipeIndex, mauIndex)),
        next_table_format_data_(default_adapter(next_table_format_data_,chip_index(), pipeIndex, mauIndex, ltIndex)),
        next_table_map_data_{ { {default_adapter(next_table_map_data_[0], chip_index(), pipeIndex, mauIndex, ltIndex, 0 )} ,
                                {default_adapter(next_table_map_data_[1], chip_index(), pipeIndex, mauIndex, ltIndex, 1 )} }},
        adr_dist_action_data_adr_icxbar_ctl_(default_adapter(adr_dist_action_data_adr_icxbar_ctl_,chip_index(), pipeIndex, mauIndex, ltIndex)),
        gateway_to_logicaltable_xbar_ctl_(default_adapter(gateway_to_logicaltable_xbar_ctl_,chip_index(), pipeIndex, mauIndex, ltIndex)),
        gateway_next_table_lut_array_{
          { {default_adapter(gateway_next_table_lut_array_[0], chip_index(), pipeIndex, mauIndex, ltIndex, 0 )} ,
            {default_adapter(gateway_next_table_lut_array_[1], chip_index(), pipeIndex, mauIndex, ltIndex, 1 )} ,
            {default_adapter(gateway_next_table_lut_array_[2], chip_index(), pipeIndex, mauIndex, ltIndex, 2 )} ,
            {default_adapter(gateway_next_table_lut_array_[3], chip_index(), pipeIndex, mauIndex, ltIndex, 3 )} ,
            {default_adapter(gateway_next_table_lut_array_[4], chip_index(), pipeIndex, mauIndex, ltIndex, 4 )}
          }},
        gateway_inhibit_lut_(default_adapter(gateway_inhibit_lut_,chip_index(), pipeIndex, mauIndex, ltIndex)),
        gateway_en_(default_adapter(gateway_en_,chip_index(), pipeIndex, mauIndex))
  {

    imem_table_addr_egress_.reset();
    ingress_predication_ctl_.reset();
    egress_predication_ctl_.reset();
    //exact_match_delay_config_.reset(); // Removed with Hyperdev
    next_table_map_en_.reset();
    next_table_format_data_.reset();
    next_table_map_data_[0].reset();
    next_table_map_data_[1].reset();
    adr_dist_action_data_adr_icxbar_ctl_.reset();
    gateway_to_logicaltable_xbar_ctl_.reset();
    for (int i=0;i<(kGatewayEntries+1);++i) {
      gateway_next_table_lut_array_[i].reset();
    }
    gateway_inhibit_lut_.reset();
    gateway_en_.reset();
  }
  MauLogicalTableReg::~MauLogicalTableReg() {
  }

  // Next 2 funcs not used - just for debug
  void MauLogicalTableReg::ingress_pred_callback() {
    RMT_LOG_VERBOSE("ingress_cb: ingress predication - logical table %d\n", lt_index_);
    uint32_t ing = ingress_predication_ctl_.table_thread();
    uint32_t eg  = egress_predication_ctl_.table_thread();
    uint32_t ita = imem_table_addr_egress_.imem_table_addr_egress();
    RMT_LOG_VERBOSE("ingress_cb: ing=0x%08x eg=0x%08x ita=0x%08x\n", ing, eg, ita);
  }
  void MauLogicalTableReg::egress_pred_callback() {
    RMT_LOG_VERBOSE("egress_cb: egress predication - logical table %d\n", lt_index_);
    uint32_t ing = ingress_predication_ctl_.table_thread();
    uint32_t eg  = egress_predication_ctl_.table_thread();
    uint32_t ita = imem_table_addr_egress_.imem_table_addr_egress();
    RMT_LOG_VERBOSE("egress_cb: ing=0x%08x eg=0x%08x ita=0x%08x\n", ing, eg, ita);
  }

  bool MauLogicalTableReg::has_exact_match() {
    return 1;
    // Reg no longer exists since mau_dev_07092015 (Hyperdev) but func not used anywhere so just return 1
    //return (((exact_match_delay_config_.exact_match_bus_thread() >> lt_index_) & 0x1) != 0);
  }
  bool MauLogicalTableReg::has_ternary_match() {
    return 1;
    // Reg no longer exists since mau_dev_07092015 (Hyperdev) but func not used anywhere so just return 1
    //return (((exact_match_delay_config_.exact_match_bus_thread() >> lt_index_) & 0x1) == 0);
  }

  uint16_t MauLogicalTableReg::get_action_logical_rows() {
    return adr_dist_action_data_adr_icxbar_ctl_.address_distr_to_logical_rows();
  }
  bool MauLogicalTableReg::get_action_overflow() {
    return ((adr_dist_action_data_adr_icxbar_ctl_.address_distr_to_overflow() & 0x1) != 0);
  }
  bool MauLogicalTableReg::get_action_overflow2_up() {
    return ((adr_dist_action_data_adr_icxbar_ctl_.address_distr_to_overflow2_up() & 0x1) != 0);
  }
  bool MauLogicalTableReg::get_action_overflow2_down() {
    return ((adr_dist_action_data_adr_icxbar_ctl_.address_distr_to_overflow2_down() & 0x1) != 0);
  }
  uint8_t MauLogicalTableReg::get_action_overflow_buses() {
    uint8_t buses = 0;
    if (get_action_overflow()) buses |= 0x1;
    if (get_action_overflow2_up()) buses |= 0x2;
    if (get_action_overflow2_down()) buses |= 0x4;
    return buses;
  }

}
