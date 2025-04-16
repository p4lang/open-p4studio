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

#ifndef _TOFINOXX_MAU_CHIP_RESULT_BUS_
#define _TOFINOXX_MAU_CHIP_RESULT_BUS_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <address.h>
#include <register_adapters.h>

#include <register_includes/next_table_format_data_array.h>
#include <register_includes/next_table_map_en.h>
#include <register_includes/next_table_map_data_array2.h>
#include <register_includes/mau_action_instruction_adr_map_data_array3.h>
#include <register_includes/mau_logical_to_meter_alu_map.h>
#include <register_includes/mau_mapram_color_map_to_logical_ctl_array.h>

namespace MODEL_CHIP_NAMESPACE {

  class MauChipResultBus {
   public:
    MauChipResultBus(int chipIndex, int pipeIndex, int mauIndex, Mau *mau) :
        next_table_format_data_(default_adapter(next_table_format_data_,chipIndex, pipeIndex, mauIndex)),
        next_table_map_en_(default_adapter(next_table_map_en_,chipIndex, pipeIndex, mauIndex)),
        next_table_map_data_(default_adapter(next_table_map_data_,chipIndex, pipeIndex, mauIndex)),
        act_instr_map_data_(default_adapter(act_instr_map_data_,chipIndex, pipeIndex, mauIndex)),
        logical_to_meter_alu_map_(default_adapter(logical_to_meter_alu_map_,chipIndex, pipeIndex, mauIndex)),
        mapram_color_map_to_logical_ctl_(default_adapter(mapram_color_map_to_logical_ctl_,chipIndex, pipeIndex, mauIndex))
    {
      next_table_format_data_.reset();
      next_table_map_en_.reset();
      next_table_map_data_.reset();
      act_instr_map_data_.reset();
      logical_to_meter_alu_map_.reset();
      mapram_color_map_to_logical_ctl_.reset();
    }
    ~MauChipResultBus() { }

    inline bool get_nxt_tab_map_enable(int bus_or_tab, bool match,  bool gw_inhibit) {
      bool en = (((next_table_map_en_.next_table_map_en() >> bus_or_tab) & 1) == 1);
      return (en && match && !gw_inhibit);
    }
    uint16_t get_nxt_tab_map_data(int bus_or_tab, int which) {
      uint32_t index = which / 4;
      switch (which & 0x3) { // Use lower 2 bits to index vals 0,1,2,3 in map_data
        case 0:  return next_table_map_data_.next_table_map_data0(bus_or_tab,index);
        case 1:  return next_table_map_data_.next_table_map_data1(bus_or_tab,index);
        case 2:  return next_table_map_data_.next_table_map_data2(bus_or_tab,index);
        case 3:  return next_table_map_data_.next_table_map_data3(bus_or_tab,index);
        default: return 0xFF;
      }
    }
    uint16_t get_nxt_tab_mask(int bus_or_tab) {
      return next_table_format_data_.match_next_table_adr_mask(bus_or_tab);
    }
    uint16_t get_nxt_tab_dflt(int bus_or_tab) {
      return next_table_format_data_.match_next_table_adr_default(bus_or_tab);
    }
    uint16_t get_nxt_tab_miss(int bus_or_tab) {
      return next_table_format_data_.match_next_table_adr_miss_value(bus_or_tab);
    }

    uint8_t get_act_instr_addr_mapped(uint8_t data, int bus_or_tab, int exact_or_tcam, int match) {
      static_assert( ( Address::kActionInstrWidth == 7), "Expected InstrWidth to be 7" );
      uint8_t s = (data & 0x3) *  7; // use lower 2 bits to index 4 7-bit vals in map_data word
      uint8_t i = (data & 0x4) >> 2; // use bit 2 to choose which word to get map_data from
      uint8_t m = 0x7F;              // form 7-bit mask
      uint32_t map_data = act_instr_map_data_.mau_action_instruction_adr_map_data(exact_or_tcam,bus_or_tab,i);
      return static_cast<uint8_t>((map_data >> s) & m);
    }

    std::vector<int> get_meter_alus_for_logical_table(int lt) {
      std::vector<int> r;
      for (int alu=0;alu<4;++alu) {
        int shift = alu * 5;
        // This register maps takes a meter alu and gives back a logical table.
        //  the idea of the name is that this information is used to map various
        //  items from logical space to alu space.
        int reg = logical_to_meter_alu_map_.mau_logical_to_meter_alu_map();
        int ctl = reg >> shift;
        if ((ctl & 0x10) && ((ctl & 0xF) == lt))
          r.push_back(alu);
      }
      // in Tofino at most one ALU should be associated with a LT
      RMT_ASSERT(r.size() <= 1);
      return r;
    }

    bool get_gateway_payload_exact_shift_ovr(int bus) {
      return GLOBAL_FALSE; // JBay only
    }

    // returns which color bus is used by this logical table (only ever 1 on Tofino)
    uint8_t get_which_color_buses(int logical_table) {
      // the sixteen logical tables are packed into 2 registers, 1 bit enable + 2 bits select
      int reg = logical_table / 8;
      int shift = (logical_table % 8) * 3;
      int d = mapram_color_map_to_logical_ctl_.mau_mapram_color_map_to_logical_ctl(reg);
      bool enable = (d>>shift) & 0x4;
      int  select = (d>>shift) & 0x3;
      return enable ? 1<<select : 0;
    }


   private:
    register_classes::NextTableFormatDataArray              next_table_format_data_;
    register_classes::NextTableMapEn                        next_table_map_en_;
    register_classes::NextTableMapDataArray2                next_table_map_data_;
    register_classes::MauActionInstructionAdrMapDataArray3  act_instr_map_data_;
    register_classes::MauLogicalToMeterAluMap               logical_to_meter_alu_map_;
    register_classes::MauMapramColorMapToLogicalCtlArray    mapram_color_map_to_logical_ctl_;
  };
}

#endif
