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

#ifndef _JBAY_SHARED_CHIP_RESULT_BUS_
#define _JBAY_SHARED_CHIP_RESULT_BUS_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <address.h>
#include <register_adapters.h>

#include <register_includes/next_table_format_data_array.h>
#include <register_includes/next_table_map_en.h>
#include <register_includes/next_table_map_en_gateway.h>
#include <register_includes/pred_map_loca_array2.h>
#include <register_includes/mau_action_instruction_adr_map_data_array3.h>
#include <register_includes/mau_logical_to_meter_alu_map_array.h>
#include <register_includes/gateway_payload_exact_shift_ovr_array.h>
#include <register_includes/mau_mapram_color_map_to_logical_ctl_array.h>


namespace MODEL_CHIP_NAMESPACE {

  class Mau;

  class MauChipResultBus {
   public:
    MauChipResultBus(int chipIndex, int pipeIndex, int mauIndex, Mau *mau) :
        mau_index_(mauIndex), mau_(mau),
        next_table_format_data_(default_adapter(next_table_format_data_,chipIndex, pipeIndex, mauIndex)),
        next_table_map_en_(default_adapter(next_table_map_en_,chipIndex, pipeIndex, mauIndex)),
        next_table_map_en_gateway_(default_adapter(next_table_map_en_gateway_,chipIndex, pipeIndex, mauIndex)),
        pred_map_loca_(default_adapter(pred_map_loca_,chipIndex, pipeIndex, mauIndex)),
        act_instr_map_data_(default_adapter(act_instr_map_data_,chipIndex, pipeIndex, mauIndex)),
        logical_to_meter_alu_map_(default_adapter(logical_to_meter_alu_map_,chipIndex, pipeIndex, mauIndex)),
        gateway_payload_exact_shift_ovr_(default_adapter(gateway_payload_exact_shift_ovr_,chipIndex, pipeIndex, mauIndex)),
        mapram_color_map_to_logical_ctl_(default_adapter(mapram_color_map_to_logical_ctl_,chipIndex, pipeIndex, mauIndex))
    {
      next_table_format_data_.reset();
      next_table_map_en_.reset();
      next_table_map_en_gateway_.reset();
      pred_map_loca_.reset();
      act_instr_map_data_.reset();
      logical_to_meter_alu_map_.reset();
      gateway_payload_exact_shift_ovr_.reset();
      mapram_color_map_to_logical_ctl_.reset();
    }
    ~MauChipResultBus() { }


    inline bool get_nxt_tab_map_enable(int bus_or_tab, bool match, bool gw_inhibit) {
      bool en = (((next_table_map_en_.next_table_map_en() >> bus_or_tab) & 1) == 1);
      bool en_gw = (((next_table_map_en_gateway_.next_table_map_en_gateway() >> bus_or_tab) & 1) == 1);
      return ((match && !gw_inhibit && en) || (gw_inhibit && en_gw));
    }
    uint16_t get_nxt_tab_map_data(int bus_or_tab, int which) {
      return pred_map_loca_.pred_map_loca_next_table(bus_or_tab, which % 8);
    }
    uint16_t get_nxt_tab_mask(int bus_or_tab) {
      // On JBay we always let through bit8 (mask CSR only covers bits 0-7)
      // (see JBay uArch 0.6 section 6.4.3.3.1 - Next Table Addressing Beyond 16 Stages)
      return next_table_format_data_.match_next_table_adr_mask(bus_or_tab) | 0x100;
    }
    uint16_t get_nxt_tab_dflt(int bus_or_tab) {
      uint16_t dflt = next_table_format_data_.match_next_table_adr_default(bus_or_tab);
      if ((next_table_map_en_.next_table_map_en() & (1<<bus_or_tab)) == 0) {
        // If NO mapping in force check MSB set in dflt in high MAU stages
        // (see JBay uArch 0.6 section 6.4.3.3.1)
        RMT_ASSERT((mau_index_ < 16) || (((dflt >> 8) & 1) == 1));
      }
      return dflt;
    }
    uint16_t get_nxt_tab_miss(int bus_or_tab) {
      return next_table_format_data_.match_next_table_adr_miss_value(bus_or_tab);
    }

    uint8_t get_act_instr_addr_mapped(uint8_t data, int bus_or_tab, int exact_or_tcam, int match) {
      static_assert( ( Address::kActionInstrWidth == 8), "Expected InstrWidth to be 8" );
      uint8_t s = (data & 0x3) *  8; // use lower 2 bits to index 4 8-bit vals in map_data word
      uint8_t i = (data & 0x4) >> 2; // use bit 2 to choose which word to get map_data from
      uint8_t m = 0xFF;              // form 8-bit mask
      uint32_t map_data = act_instr_map_data_.mau_action_instruction_adr_map_data(exact_or_tcam,bus_or_tab,i);
      return static_cast<uint8_t>((map_data >> s) & m);
    }

    std::vector<int> get_meter_alus_for_logical_table(int lt) {
      std::vector<int> r;
      int index = lt / 8;
      int shift = (lt % 8) * 4;
      int reg = logical_to_meter_alu_map_.mau_logical_to_meter_alu_map(index);
      int v = 0xF & (reg >> shift );
      for (int alu=0;alu<4;++alu) {
        if ( v & (1<<alu) )
          r.push_back(alu);
      }
      return r;
    }

    bool get_gateway_payload_exact_shift_ovr(int bus) {
      int index = bus / 8;
      int bit   = bus % 8;
      int v = gateway_payload_exact_shift_ovr_.gateway_payload_exact_shift_ovr( index );
      return ( (v>>bit) & 1 );
    }

    // returns which color buses are used by this logical table
    uint8_t get_which_color_buses(int logical_table) {
      uint8_t buses = 0;
      for (uint8_t bus = 0; bus < MauDefs::kNumColorBuses; bus++) {
        uint16_t lts = mapram_color_map_to_logical_ctl_.mau_mapram_color_map_to_logical_ctl(bus);
        if (((lts >> logical_table) & 1) == 1) buses |= 1<<bus;
      }
      return buses;
    }


 private:
    void atomic_mod_sram_go_rd_callback(int ie);
    void atomic_mod_sram_go_wr_callback(int ie);
    void atomic_mod_tcam_go_rd_callback(int ie);
    void atomic_mod_tcam_go_wr_callback(int ie);

 private:
    int                                                     mau_index_;
    Mau                                                    *mau_;
    register_classes::NextTableFormatDataArray              next_table_format_data_;
    register_classes::NextTableMapEn                        next_table_map_en_;
    register_classes::NextTableMapEnGateway                 next_table_map_en_gateway_;
    register_classes::PredMapLocaArray2                     pred_map_loca_;
    register_classes::MauActionInstructionAdrMapDataArray3  act_instr_map_data_;
    register_classes::MauLogicalToMeterAluMapArray          logical_to_meter_alu_map_;
    register_classes::GatewayPayloadExactShiftOvrArray      gateway_payload_exact_shift_ovr_;
    register_classes::MauMapramColorMapToLogicalCtlArray    mapram_color_map_to_logical_ctl_;
  };
}

#endif
