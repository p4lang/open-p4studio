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

#ifndef _SHARED_MAU_RESULT_BUS_
#define _SHARED_MAU_RESULT_BUS_

#include <string>
#include <atomic>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <address.h>
#include <action-output-hv-xbar.h>
#include <mau-meter-alu.h>
#include <register_adapters.h>

#include <register_includes/next_table_tcam_actionbit_map_en.h>

#include <register_includes/mau_immediate_data_tcam_actionbit_map_en.h>
#include <register_includes/mau_immediate_data_tcam_actionbit_map_data_array2.h>
#include <register_includes/mau_immediate_data_exact_shiftcount_array2.h>
#include <register_includes/mau_immediate_data_tcam_shiftcount_array.h>
#include <register_includes/mau_immediate_data_mask_array2.h>
#include <register_includes/mau_immediate_data_default_array2.h>
#include <register_includes/mau_immediate_data_miss_value_array.h>

#include <register_includes/mau_actiondata_adr_tcam_actionbit_map_en.h>
#include <register_includes/mau_actiondata_adr_tcam_actionbit_map_data_array2.h>
#include <register_includes/mau_actiondata_adr_exact_shiftcount_array2.h>
#include <register_includes/mau_actiondata_adr_tcam_shiftcount_array.h>
#include <register_includes/mau_actiondata_adr_vpn_shiftcount_array2.h>
#include <register_includes/mau_actiondata_adr_per_entry_en_mux_ctl_array2.h>
#include <register_includes/mau_actiondata_adr_mask_array2.h>
#include <register_includes/mau_actiondata_adr_default_array2.h>
#include <register_includes/mau_actiondata_adr_miss_value_array.h>

#include <register_includes/mau_action_instruction_adr_tcam_actionbit_map_en.h>
#include <register_includes/mau_action_instruction_adr_exact_shiftcount_array2.h>
#include <register_includes/mau_action_instruction_adr_tcam_shiftcount_array.h>
#include <register_includes/mau_action_instruction_adr_map_en_array.h>
#include <register_includes/mau_action_instruction_adr_per_entry_en_mux_ctl_array2.h>
#include <register_includes/mau_action_instruction_adr_mask_array2.h>
#include <register_includes/mau_action_instruction_adr_default_array2.h>
#include <register_includes/mau_action_instruction_adr_miss_value_array.h>

#include <register_includes/mau_stats_adr_tcam_actionbit_map_en.h>
#include <register_includes/mau_stats_adr_tcam_actionbit_map_data_array2.h>
#include <register_includes/mau_stats_adr_exact_shiftcount_array2.h>
#include <register_includes/mau_stats_adr_tcam_shiftcount_array.h>
#include <register_includes/mau_stats_adr_per_entry_en_mux_ctl_array2.h>
#include <register_includes/mau_stats_adr_mask_array2.h>
#include <register_includes/mau_stats_adr_default_array2.h>
#include <register_includes/mau_stats_adr_miss_value_array.h>
#include <register_includes/mau_stats_adr_hole_swizzle_mode_array2.h>

#include <register_includes/mau_meter_adr_tcam_actionbit_map_en.h>
#include <register_includes/mau_meter_adr_tcam_actionbit_map_data_array2.h>
#include <register_includes/mau_meter_adr_exact_shiftcount_array2.h>
#include <register_includes/mau_meter_adr_tcam_shiftcount_array.h>
#include <register_includes/mau_meter_adr_per_entry_en_mux_ctl_array2.h>
#include <register_includes/mau_meter_adr_mask_array2.h>
#include <register_includes/mau_meter_adr_default_array2.h>
#include <register_includes/mau_meter_adr_miss_value_array.h>

#include <register_includes/mau_idletime_adr_tcam_actionbit_map_en.h>
#include <register_includes/mau_idletime_adr_tcam_actionbit_map_data_array2.h>
#include <register_includes/mau_idletime_adr_exact_shiftcount_array2.h>
#include <register_includes/mau_idletime_adr_tcam_shiftcount_array.h>
#include <register_includes/mau_idletime_adr_per_entry_en_mux_ctl_array2.h>
#include <register_includes/mau_idletime_adr_mask_array2.h>
#include <register_includes/mau_idletime_adr_default_array2.h>
#include <register_includes/mau_idletime_adr_miss_value_array.h>

#include <register_includes/mau_payload_shifter_enable_array2.h>

#include <register_includes/tind_ram_data_size_array.h>
#include <register_includes/immediate_data_32b_ixbar_ctl_array.h>
#include <register_includes/immediate_data_16b_ixbar_ctl_array.h>
#include <register_includes/immediate_data_8b_ixbar_ctl_array.h>
#include <register_includes/immediate_data_8b_enable_array.h>
#include <register_includes/immediate_data_rng_logical_map_ctl_array.h>
#include <register_includes/immediate_data_rng_enable.h>

#include <register_includes/mau_selectorlength_default_array2.h>
#include <register_includes/mau_selectorlength_mask_array2.h>
#include <register_includes/mau_selectorlength_shiftcount_array2.h>

#include <register_includes/mau_match_central_mapram_read_color_oflo_ctl.h>
#include <register_includes/meter_color_output_map_array.h>

#include <register_includes/match_to_logical_table_ixbar_outputmap_array2.h>
#include <register_includes/tcam_hit_to_logical_table_ixbar_outputmap_array.h>
// Also put LTCAM->PhysTindBus reg in here as sort of a result bus!!
#include <register_includes/tcam_match_adr_to_physical_oxbar_outputmap_array.h>

// Map PhysicalResultBuses to MeterALU
#include <register_includes/mau_physical_to_meter_alu_ixbar_map_array2.h>   // tofino only
#include <register_includes/mau_physical_to_meter_alu_icxbar_map_array2.h>  // jbay only

#include <register_includes/mau_meter_alu_to_logical_map_array.h>

#include <register_includes/mau_meter_adr_type_position_array2.h>

#include <register_includes/meter_enable.h>

// Only used to verify virtual access allowed
#include <register_includes/adr_dist_stats_adr_icxbar_ctl_array.h>
#include <register_includes/adr_dist_meter_adr_icxbar_ctl_array.h>
#include <register_includes/mau_ad_stats_virt_lt_array.h>
#include <register_includes/mau_ad_meter_virt_lt_array.h>
// scratch register used for HA
#include <register_includes/mau_scratch.h>

#include <mau-chip-result-bus.h>


namespace MODEL_CHIP_NAMESPACE {

  class Mau;

  class MauResultBusRegExact {
 public:
    MauResultBusRegExact(int chipIndex, int pipeIndex, int mauIndex)
        : imm_data_shift_(default_adapter(imm_data_shift_,chipIndex, pipeIndex, mauIndex)),
           act_data_addr_shift_(default_adapter(act_data_addr_shift_,chipIndex, pipeIndex, mauIndex)),
           act_instr_addr_shift_(default_adapter(act_instr_addr_shift_,chipIndex, pipeIndex, mauIndex)),
           stats_addr_shift_(default_adapter(stats_addr_shift_,chipIndex, pipeIndex, mauIndex)),
           meter_addr_shift_(default_adapter(meter_addr_shift_,chipIndex, pipeIndex, mauIndex)),
           idle_addr_shift_(default_adapter(idle_addr_shift_,chipIndex, pipeIndex, mauIndex)) {

      imm_data_shift_.reset();
      act_data_addr_shift_.reset();
      act_instr_addr_shift_.reset();
      stats_addr_shift_.reset();
      meter_addr_shift_.reset();
      idle_addr_shift_.reset();
    }
    ~MauResultBusRegExact() { }

    inline uint8_t get_imm_data_shift(int rsltBus, int match) {
      return imm_data_shift_.mau_immediate_data_exact_shiftcount(rsltBus, match);
    }
    inline uint8_t get_act_data_addr_shift(int rsltBus, int match) {
      return act_data_addr_shift_.mau_actiondata_adr_exact_shiftcount(rsltBus, match);
    }
    inline uint8_t get_act_instr_addr_shift(int rsltBus, int match) {
      return act_instr_addr_shift_.mau_action_instruction_adr_exact_shiftcount(rsltBus, match);
    }
    inline uint8_t get_stats_addr_shift(int rsltBus, int match) {
      return stats_addr_shift_.mau_stats_adr_exact_shiftcount(rsltBus, match);
    }
    inline uint8_t get_meter_addr_shift(int rsltBus, int match) {
      return meter_addr_shift_.mau_meter_adr_exact_shiftcount(rsltBus, match);
    }
    inline uint8_t get_idle_addr_shift(int rsltBus, int match) {
      return idle_addr_shift_.mau_idletime_adr_exact_shiftcount(rsltBus, match);
    }
 private:
    register_classes::MauImmediateDataExactShiftcountArray2        imm_data_shift_;
    register_classes::MauActiondataAdrExactShiftcountArray2        act_data_addr_shift_;
    register_classes::MauActionInstructionAdrExactShiftcountArray2 act_instr_addr_shift_;
    register_classes::MauStatsAdrExactShiftcountArray2             stats_addr_shift_;
    register_classes::MauMeterAdrExactShiftcountArray2             meter_addr_shift_;
    register_classes::MauIdletimeAdrExactShiftcountArray2          idle_addr_shift_;
  };



  class MauResultBusRegTcam {
 public:
   MauResultBusRegTcam(int chipIndex, int pipeIndex, int mauIndex)
       : imm_data_shift_(default_adapter(imm_data_shift_,chipIndex, pipeIndex, mauIndex)),
         act_data_addr_shift_(default_adapter(act_data_addr_shift_,chipIndex, pipeIndex, mauIndex)),
         act_instr_addr_shift_(default_adapter(act_instr_addr_shift_,chipIndex, pipeIndex, mauIndex)),
         stats_addr_shift_(default_adapter(stats_addr_shift_,chipIndex, pipeIndex, mauIndex)),
         meter_addr_shift_(default_adapter(meter_addr_shift_,chipIndex, pipeIndex, mauIndex)),
         idle_addr_shift_(default_adapter(idle_addr_shift_,chipIndex, pipeIndex, mauIndex)) {

      imm_data_shift_.reset();
      act_data_addr_shift_.reset();
      act_instr_addr_shift_.reset();
      stats_addr_shift_.reset();
      meter_addr_shift_.reset();
      idle_addr_shift_.reset();
    }
    ~MauResultBusRegTcam() { }

    inline uint8_t get_imm_data_shift(int rsltBus, int match) {
      return imm_data_shift_.mau_immediate_data_tcam_shiftcount(rsltBus);
    }
    inline uint8_t get_act_data_addr_shift(int rsltBus, int match) {
      return act_data_addr_shift_.mau_actiondata_adr_tcam_shiftcount(rsltBus);
    }
    inline uint8_t get_act_instr_addr_shift(int rsltBus, int match) {
      return act_instr_addr_shift_.mau_action_instruction_adr_tcam_shiftcount(rsltBus);
    }
    inline uint8_t get_stats_addr_shift(int rsltBus, int match) {
      return stats_addr_shift_.mau_stats_adr_tcam_shiftcount(rsltBus);
    }
    inline uint8_t get_meter_addr_shift(int rsltBus, int match) {
      return meter_addr_shift_.mau_meter_adr_tcam_shiftcount(rsltBus);
    }
    inline uint8_t get_idle_addr_shift(int rsltBus, int match) {
      return idle_addr_shift_.mau_idletime_adr_tcam_shiftcount(rsltBus);
    }
 private:
    register_classes::MauImmediateDataTcamShiftcountArray        imm_data_shift_;
    register_classes::MauActiondataAdrTcamShiftcountArray        act_data_addr_shift_;
    register_classes::MauActionInstructionAdrTcamShiftcountArray act_instr_addr_shift_;
    register_classes::MauStatsAdrTcamShiftcountArray             stats_addr_shift_;
    register_classes::MauMeterAdrTcamShiftcountArray             meter_addr_shift_;
    register_classes::MauIdletimeAdrTcamShiftcountArray          idle_addr_shift_;
  };


  class MauResultBus {
 public:
    // Need these to figure out ActionHvBus byteposns for immediate data - TODO: old values, fix!
    static constexpr int  kXbarBytes        = 16;
    static constexpr int  kXbarHalfWords    = 24;
    static constexpr int  kXbarWords        = 32;
    static constexpr int  kLogicalTables    = MauDefs::kLogicalTablesPerMau;
    static constexpr int  kLogicalTcams     = MauDefs::kLogicalTcamsPerMau;
    static constexpr int  kNumStatsAlus     = MauDefs::kNumStatsAlus;
    static constexpr int  kNumMeterAlus     = MauDefs::kNumMeterAlus;
    static constexpr int  kMatchOutputBuses = MauDefs::kMatchOutputBusesPerMau;
    static constexpr int  kTindOutputBuses  = MauDefs::kTindOutputBusesPerMau;
    static constexpr int  kOutputBuses      = kMatchOutputBuses;
    static_assert( (kMatchOutputBuses == kTindOutputBuses),
                   "Code assumes same num Match/Tind result buses");
    static_assert( (kOutputBuses <= 16),
                   "Output bus bitmap must fit in uint16_t");

    static const uint32_t swizzle_to_lsb(uint32_t in, int nbits, int pos) {
      uint32_t topM = ~( (1u << (nbits+pos)) - 1u);
      uint32_t midM = ((1u << nbits) - 1u) << pos;
      uint32_t botM = ((1u << pos) - 1u) << 0;
      return (in & topM) | ((in & midM) >> pos) | ((in & botM) << pos);
    }


    MauResultBus(int chipIndex, int pipeIndex, int mauIndex, Mau *mau)
        : next_table_actionbit_map_en_(default_adapter(next_table_actionbit_map_en_,chipIndex, pipeIndex, mauIndex)),
          imm_data_actionbit_map_en_(default_adapter(imm_data_actionbit_map_en_,chipIndex, pipeIndex, mauIndex)),
          imm_data_actionbit_map_data_(default_adapter(imm_data_actionbit_map_data_,chipIndex, pipeIndex, mauIndex)),
          imm_data_mask_(default_adapter(imm_data_mask_,chipIndex, pipeIndex, mauIndex)),
          imm_data_dflt_(default_adapter(imm_data_dflt_,chipIndex, pipeIndex, mauIndex)),
          imm_data_miss_(default_adapter(imm_data_miss_,chipIndex, pipeIndex, mauIndex)),
          act_data_addr_actionbit_map_en_(default_adapter(act_data_addr_actionbit_map_en_,chipIndex, pipeIndex, mauIndex)),
          act_data_addr_actionbit_map_data_(default_adapter(act_data_addr_actionbit_map_data_,chipIndex, pipeIndex, mauIndex)),
          act_data_addr_vpn_shift_(default_adapter(act_data_addr_vpn_shift_,chipIndex, pipeIndex, mauIndex)),
          act_data_addr_perentry_en_(default_adapter(act_data_addr_perentry_en_,chipIndex, pipeIndex, mauIndex)),
          act_data_addr_mask_(default_adapter(act_data_addr_mask_,chipIndex, pipeIndex, mauIndex)),
          act_data_addr_dflt_(default_adapter(act_data_addr_dflt_,chipIndex, pipeIndex, mauIndex)),
          act_data_addr_miss_(default_adapter(act_data_addr_miss_,chipIndex, pipeIndex, mauIndex)),
          act_instr_actionbit_map_en_(default_adapter(act_instr_actionbit_map_en_,chipIndex, pipeIndex, mauIndex)),
          act_instr_map_en_(default_adapter(act_instr_map_en_,chipIndex, pipeIndex, mauIndex)),
          act_instr_perentry_en_(default_adapter(act_instr_perentry_en_,chipIndex, pipeIndex, mauIndex)),
          act_instr_mask_(default_adapter(act_instr_mask_,chipIndex, pipeIndex, mauIndex)),
          act_instr_dflt_(default_adapter(act_instr_dflt_,chipIndex, pipeIndex, mauIndex)),
          act_instr_miss_(default_adapter(act_instr_miss_,chipIndex, pipeIndex, mauIndex)),
          stats_addr_actionbit_map_en_(default_adapter(stats_addr_actionbit_map_en_,chipIndex, pipeIndex, mauIndex)),
          stats_addr_actionbit_map_data_(default_adapter(stats_addr_actionbit_map_data_,chipIndex, pipeIndex, mauIndex)),
          stats_addr_perentry_en_(default_adapter(stats_addr_perentry_en_,chipIndex, pipeIndex, mauIndex)),
          stats_addr_mask_(default_adapter(stats_addr_mask_,chipIndex, pipeIndex, mauIndex)),
          stats_addr_dflt_(default_adapter(stats_addr_dflt_,chipIndex, pipeIndex, mauIndex)),
          stats_addr_miss_(default_adapter(stats_addr_miss_,chipIndex, pipeIndex, mauIndex)),
          stats_addr_hole_swizzle_(default_adapter(stats_addr_hole_swizzle_,chipIndex, pipeIndex, mauIndex)),
          meter_addr_actionbit_map_en_(default_adapter(meter_addr_actionbit_map_en_,chipIndex, pipeIndex, mauIndex)),
          meter_addr_actionbit_map_data_(default_adapter(meter_addr_actionbit_map_data_,chipIndex, pipeIndex, mauIndex)),
          meter_addr_perentry_en_(default_adapter(meter_addr_perentry_en_,chipIndex, pipeIndex, mauIndex)),
          meter_addr_mask_(default_adapter(meter_addr_mask_,chipIndex, pipeIndex, mauIndex)),
          meter_addr_dflt_(default_adapter(meter_addr_dflt_,chipIndex, pipeIndex, mauIndex)),
          meter_addr_miss_(default_adapter(meter_addr_miss_,chipIndex, pipeIndex, mauIndex)),
          idletime_addr_actionbit_map_en_(default_adapter(idletime_addr_actionbit_map_en_,chipIndex, pipeIndex, mauIndex)),
          idletime_addr_actionbit_map_data_(default_adapter(idletime_addr_actionbit_map_data_,chipIndex, pipeIndex, mauIndex)),
          idletime_addr_perentry_en_(default_adapter(idletime_addr_perentry_en_,chipIndex, pipeIndex, mauIndex)),
          idletime_addr_mask_(default_adapter(idletime_addr_mask_,chipIndex, pipeIndex, mauIndex)),
          idletime_addr_dflt_(default_adapter(idletime_addr_dflt_,chipIndex, pipeIndex, mauIndex)),
          idletime_addr_miss_(default_adapter(idletime_addr_miss_,chipIndex, pipeIndex, mauIndex)),
          payload_shifter_enable_(default_adapter(payload_shifter_enable_,chipIndex, pipeIndex, mauIndex)),
          tind_ram_data_size_(default_adapter(tind_ram_data_size_,chipIndex,pipeIndex,mauIndex)),
          immediate_data_32b_ixbar_ctl_(default_adapter(immediate_data_32b_ixbar_ctl_,chipIndex,pipeIndex,mauIndex)),
          immediate_data_16b_ixbar_ctl_(default_adapter(immediate_data_16b_ixbar_ctl_,chipIndex,pipeIndex,mauIndex)),
          immediate_data_8b_ixbar_ctl_(default_adapter(immediate_data_8b_ixbar_ctl_,chipIndex,pipeIndex,mauIndex)),
          immediate_data_8b_enable_(default_adapter(immediate_data_8b_enable_,chipIndex,pipeIndex,mauIndex)),
          imm_data_rng_logical_map_ctl_(default_adapter(imm_data_rng_logical_map_ctl_,chipIndex, pipeIndex, mauIndex)),
          imm_data_rng_enable_(default_adapter(imm_data_rng_enable_,chipIndex, pipeIndex, mauIndex)),
          match_to_logical_table_ixbar_(default_adapter(match_to_logical_table_ixbar_,
                                                        chipIndex, pipeIndex, mauIndex,
                                                        [this](uint32_t i,uint32_t j){this->match_to_logical_table_ixbar_callback(i,j);})),
          tcam_hit_to_logical_table_ixbar_(default_adapter(tcam_hit_to_logical_table_ixbar_,
                                                           chipIndex, pipeIndex, mauIndex,
                                                           [this](uint32_t i){this->tcam_hit_to_logical_table_ixbar_callback(i);})),
          phys_to_meter_alu_ixbar_(default_adapter(phys_to_meter_alu_ixbar_,
                                                   chipIndex, pipeIndex, mauIndex,
                                                   [this](uint32_t i,uint32_t j){this->phys_to_meter_alu_ixbar_callback(i,j);})),
          phys_to_meter_alu_icxbar_(default_adapter(phys_to_meter_alu_icxbar_,
                                                    chipIndex, pipeIndex, mauIndex,
                                                    [this](uint32_t i,uint32_t j){this->phys_to_meter_alu_icxbar_callback(i,j);})),
          meter_alu_to_logical_table_oxbar_(default_adapter(meter_alu_to_logical_table_oxbar_,chipIndex, pipeIndex, mauIndex)),
          ltcam_tind_outbus_map_(default_adapter(ltcam_tind_outbus_map_,chipIndex, pipeIndex, mauIndex)),
          selector_length_dflt_(default_adapter(selector_length_dflt_,chipIndex, pipeIndex, mauIndex)),
          selector_length_mask_(default_adapter(selector_length_mask_,chipIndex, pipeIndex, mauIndex)),
          selector_length_shift_(default_adapter(selector_length_shift_,chipIndex, pipeIndex, mauIndex)),
          mapram_read_color_oflo_ctl_(default_adapter(mapram_read_color_oflo_ctl_,chipIndex, pipeIndex, mauIndex)),
          meter_color_output_map_(default_adapter(meter_color_output_map_,chipIndex, pipeIndex, mauIndex)),
          meter_adr_type_position_(default_adapter(meter_adr_type_position_,chipIndex, pipeIndex, mauIndex)),
          meter_enable_(default_adapter(meter_enable_,chipIndex, pipeIndex, mauIndex)),
          adr_dist_stats_icxbar_(default_adapter(adr_dist_stats_icxbar_,chipIndex, pipeIndex, mauIndex)),
          adr_dist_meter_icxbar_(default_adapter(adr_dist_meter_icxbar_,chipIndex, pipeIndex, mauIndex)),
          mau_ad_stats_virt_lt_(default_adapter(mau_ad_stats_virt_lt_,chipIndex, pipeIndex, mauIndex)),
          mau_ad_meter_virt_lt_(default_adapter(mau_ad_meter_virt_lt_,chipIndex, pipeIndex, mauIndex)),
          mau_scratch_(default_adapter(mau_scratch_,chipIndex, pipeIndex, mauIndex)),
        ltab_to_bus_(), ltab_to_ltcams_(), meter_alu_to_bus_(), spinlock_(),
        exact_(chipIndex, pipeIndex, mauIndex),
        tcam_(chipIndex, pipeIndex, mauIndex),
        mau_chip_result_bus_(chipIndex, pipeIndex, mauIndex,mau)
    {

      next_table_actionbit_map_en_.reset();
      imm_data_actionbit_map_en_.reset();
      imm_data_actionbit_map_data_.reset();
      imm_data_mask_.reset();
      imm_data_dflt_.reset();
      imm_data_miss_.reset();
      act_data_addr_actionbit_map_en_.reset();
      act_data_addr_actionbit_map_data_.reset();
      act_data_addr_vpn_shift_.reset();
      act_data_addr_perentry_en_.reset();
      act_data_addr_mask_.reset();
      act_data_addr_dflt_.reset();
      act_data_addr_miss_.reset();
      act_instr_actionbit_map_en_.reset();
      act_instr_map_en_.reset();
      act_instr_perentry_en_.reset();
      act_instr_mask_.reset();
      act_instr_dflt_.reset();
      act_instr_miss_.reset();
      stats_addr_actionbit_map_en_.reset();
      stats_addr_actionbit_map_data_.reset();
      stats_addr_perentry_en_.reset();
      stats_addr_mask_.reset();
      stats_addr_dflt_.reset();
      stats_addr_miss_.reset();
      stats_addr_hole_swizzle_.reset();
      meter_addr_actionbit_map_en_.reset();
      meter_addr_actionbit_map_data_.reset();
      meter_addr_perentry_en_.reset();
      meter_addr_mask_.reset();
      meter_addr_dflt_.reset();
      meter_addr_miss_.reset();
      idletime_addr_actionbit_map_en_.reset();
      idletime_addr_actionbit_map_data_.reset();
      idletime_addr_perentry_en_.reset();
      idletime_addr_mask_.reset();
      idletime_addr_dflt_.reset();
      idletime_addr_miss_.reset();
      payload_shifter_enable_.reset();
      tind_ram_data_size_.reset();
      immediate_data_32b_ixbar_ctl_.reset();
      immediate_data_16b_ixbar_ctl_.reset();
      immediate_data_8b_ixbar_ctl_.reset();
      immediate_data_8b_enable_.reset();
      imm_data_rng_logical_map_ctl_.reset();
      imm_data_rng_enable_.reset();
      match_to_logical_table_ixbar_.reset();
      tcam_hit_to_logical_table_ixbar_.reset();
      phys_to_meter_alu_ixbar_.reset();
      phys_to_meter_alu_icxbar_.reset();
      meter_alu_to_logical_table_oxbar_.reset();
      ltcam_tind_outbus_map_.reset();
      selector_length_dflt_.reset();
      selector_length_mask_.reset();
      selector_length_shift_.reset();
      mapram_read_color_oflo_ctl_.reset();
      meter_color_output_map_.reset();
      meter_adr_type_position_.reset();
      meter_enable_.reset();
      adr_dist_stats_icxbar_.reset();
      adr_dist_meter_icxbar_.reset();
      mau_ad_stats_virt_lt_.reset();
      mau_ad_meter_virt_lt_.reset();
      mau_scratch_.reset();
    }
    ~MauResultBus() { }

    // NEXT TABLE actionbit_enable/enable/map/shift/mask/default/miss
    inline bool get_nxt_tab_actionbit_map_enable(int bus_or_tab, int exact_or_tcam) {
      uint16_t en = next_table_actionbit_map_en_.next_table_tcam_actionbit_map_en();
      return ((exact_or_tcam == 1) && ((en & (1<<bus_or_tab)) != 0));
    }
    inline uint16_t get_nxt_tab_actionbit_map_data(int bus_or_tab, int exact_or_tcam,
                                                   int payload) {
      RMT_ASSERT ((exact_or_tcam == 1) && ((payload == 0) || (payload == 1)));
      return get_nxt_tab_map_data(bus_or_tab, payload & 0x1);
    }
    inline bool get_nxt_tab_map_enable(int bus_or_tab, bool match, bool gw_inhibit) {
      return mau_chip_result_bus_.get_nxt_tab_map_enable(bus_or_tab, match, gw_inhibit);
    }
    inline uint16_t get_nxt_tab_map_data(int bus_or_tab, int which) {
      return mau_chip_result_bus_.get_nxt_tab_map_data(bus_or_tab, which);
    }
    inline uint8_t get_nxt_tab_shift(int bus_or_tab) {
      // As of regs 20140930 there is no longer a next_table shift field
      //return next_table_format_data_.match_next_table_adr_shiftcount(bus_or_tab);
      return 0;
    }
    inline uint16_t get_nxt_tab_mask(int bus_or_tab) {
      return mau_chip_result_bus_.get_nxt_tab_mask(bus_or_tab);
    }
    inline uint16_t get_nxt_tab_dflt(int bus_or_tab) {
      return mau_chip_result_bus_.get_nxt_tab_dflt(bus_or_tab);
    }
    inline uint16_t get_nxt_tab_miss(int bus_or_tab) {
      return mau_chip_result_bus_.get_nxt_tab_miss(bus_or_tab);
    }
    inline uint16_t get_nxt_tab(uint64_t data, uint8_t shift, uint16_t mask, uint16_t dflt) {
      return ((static_cast<uint16_t>(data >> shift) & mask) | dflt);
    }
    // Only useful if we get shift/mask/default ALL from looking up SAME bus_or_tab
    inline uint16_t get_nxt_tab(uint64_t data, int bus_or_tab) {
      uint8_t  s = get_nxt_tab_shift(bus_or_tab);
      uint16_t m = get_nxt_tab_mask(bus_or_tab);
      uint16_t d = get_nxt_tab_dflt(bus_or_tab);
      uint16_t v = get_nxt_tab(data, s, m, d);
      return map_nxt_tab(v, bus_or_tab);
    }
    inline uint16_t get_nxt_tab_mapped(uint8_t data, int bus_or_tab) {
      // Use lower 3 bits to index 8 6-bit vals in map_data
      return get_nxt_tab_map_data(bus_or_tab, (data & 0x7));
    }
    inline uint16_t map_nxt_tab(uint8_t data, int bus_or_tab) {
      if (get_nxt_tab_map_enable(bus_or_tab, true, false))
        data = get_nxt_tab_mapped(data, bus_or_tab);
      return data;
    }


    // IMMEDIATE DATA actionbit enable/actionbit data/shift/mask/default/miss
    inline bool get_imm_data_actionbit_map_enable(int bus_or_tab, int exact_or_tcam) {
      uint16_t en = imm_data_actionbit_map_en_.mau_immediate_data_tcam_actionbit_map_en();
      return ((exact_or_tcam == 1) && ((en & (1<<bus_or_tab)) != 0));
    }
    inline uint32_t get_imm_data_actionbit_map_data(int bus_or_tab, int exact_or_tcam,
                                                    int payload) {
      RMT_ASSERT ((exact_or_tcam == 1) && ((payload == 0) || (payload == 1)));
      return imm_data_actionbit_map_data_.mau_immediate_data_tcam_actionbit_map_data(bus_or_tab,
                                                                                     payload & 0x1);
    }
    inline uint8_t get_imm_data_shift(int bus_or_tab, int exact_or_tcam, int match=0) {
      if (exact_or_tcam == 0)
        return exact_.get_imm_data_shift(bus_or_tab, match);
      else
        return tcam_.get_imm_data_shift(bus_or_tab, match);
    }
    inline uint32_t get_imm_data_mask(int bus_or_tab, int exact_or_tcam, int match=0) {
      return imm_data_mask_.mau_immediate_data_mask(exact_or_tcam, bus_or_tab);
    }
    inline uint32_t get_imm_data_dflt(int bus_or_tab, int exact_or_tcam, int match=0) {
      return imm_data_dflt_.mau_immediate_data_default(exact_or_tcam, bus_or_tab);
    }
    inline uint32_t get_imm_data_miss(int bus_or_tab, int exact_or_tcam, int match=0) {
      return imm_data_miss_.mau_immediate_data_miss_value(bus_or_tab);
    }
    inline uint32_t get_imm_data(uint64_t data, uint8_t shift, uint32_t mask, uint32_t dflt) {
      return ((static_cast<uint32_t>(data >> shift) & mask) | dflt);
    }
    // Only useful if we get shift/mask/default ALL from looking up SAME bus_or_tab
    inline uint32_t get_imm_data(uint64_t data, int bus_or_tab, int exact_or_tcam, int match=0) {
      uint8_t  s = get_imm_data_shift(bus_or_tab, exact_or_tcam, match);
      uint32_t m = get_imm_data_mask(bus_or_tab, exact_or_tcam, match);
      uint32_t d = get_imm_data_dflt(bus_or_tab, exact_or_tcam, match);
      return get_imm_data(data, s, m, d);
    }
    inline bool get_imm_data_payload_shifter_enable(int bus_or_tab, int exact_or_tcam) {
      return (payload_shifter_enable_.immediate_data_payload_shifter_en(exact_or_tcam, bus_or_tab) == 1);
    }

    // ACTION DATA actionbit enable/actionbit data/shift/mask/default/miss
    inline bool get_act_data_addr_actionbit_map_enable(int bus_or_tab, int exact_or_tcam) {
      uint16_t en = act_data_addr_actionbit_map_en_.mau_actiondata_adr_tcam_actionbit_map_en();
      return ((exact_or_tcam == 1) && ((en & (1<<bus_or_tab)) != 0));
    }
    inline uint32_t get_act_data_addr_actionbit_map_data(int bus_or_tab, int exact_or_tcam,
                                                         int payload) {
      RMT_ASSERT ((exact_or_tcam == 1) && ((payload == 0) || (payload == 1)));
      return act_data_addr_actionbit_map_data_.mau_actiondata_adr_tcam_actionbit_map_data(bus_or_tab,
                                                                                          payload & 0x1);
    }
    inline uint8_t get_act_data_addr_shift(int bus_or_tab, int exact_or_tcam, int match=0) {
      if (exact_or_tcam == 0)
        return exact_.get_act_data_addr_shift(bus_or_tab, match);
      else
        return tcam_.get_act_data_addr_shift(bus_or_tab, match);
    }
    inline uint8_t get_act_data_addr_vpn_shift(int bus_or_tab, int exact_or_tcam, int match=0) {
      return act_data_addr_vpn_shift_.mau_actiondata_adr_vpn_shiftcount(exact_or_tcam, bus_or_tab);
    }
    inline uint8_t get_act_data_addr_perentry_enable(int bus_or_tab, int exact_or_tcam, int match=0) {
      uint8_t v = act_data_addr_perentry_en_.mau_actiondata_adr_per_entry_en_mux_ctl(exact_or_tcam, bus_or_tab);
      //if ((v >> 5) != 0) { THROW_ERROR(-2); } // Reg is 7b wide but only 5b should be used
      return (v & 0x1f);
    }
    inline uint32_t get_act_data_addr_mask(int bus_or_tab, int exact_or_tcam, int match=0) {
      return act_data_addr_mask_.mau_actiondata_adr_mask(exact_or_tcam, bus_or_tab);
    }
    inline uint32_t get_act_data_addr_dflt(int bus_or_tab, int exact_or_tcam, int match=0) {
      return act_data_addr_dflt_.mau_actiondata_adr_default(exact_or_tcam, bus_or_tab);
    }
    inline uint32_t get_act_data_addr_miss(int bus_or_tab, int exact_or_tcam, int match=0) {
      return act_data_addr_miss_.mau_actiondata_adr_miss_value(bus_or_tab);
    }
    inline uint32_t get_act_data_addr(uint64_t data, uint8_t shift, uint32_t mask, uint32_t dflt) {
      return ((static_cast<uint32_t>(data >> shift) & mask) | dflt);
    }
    // Only useful if we get shift/mask/default ALL from looking up SAME bus_or_tab
    inline uint32_t get_act_data_addr(uint64_t data, int bus_or_tab, int exact_or_tcam, int match=0) {
      uint8_t  s = get_act_data_addr_shift(bus_or_tab, exact_or_tcam, match);
      uint32_t m = get_act_data_addr_mask(bus_or_tab, exact_or_tcam, match);
      uint32_t d = get_act_data_addr_dflt(bus_or_tab, exact_or_tcam, match);
      return get_act_data_addr(data, s, m, d);
    }
    inline bool get_act_data_addr_payload_shifter_enable(int bus_or_tab, int exact_or_tcam) {
      return (payload_shifter_enable_.actiondata_adr_payload_shifter_en(exact_or_tcam, bus_or_tab) == 1);
    }

    // ACTION INSTRUCTION actionbit enable/enable/map/shift/mask/default/miss
    inline bool get_act_instr_actionbit_map_enable(int bus_or_tab, int exact_or_tcam) {
      uint16_t en = act_instr_actionbit_map_en_.mau_action_instruction_adr_tcam_actionbit_map_en();
      return ((exact_or_tcam == 1) && ((en & (1<<bus_or_tab)) != 0));
    }
    inline uint8_t get_act_instr_actionbit_map_data(int bus_or_tab, int exact_or_tcam,
                                                    int payload) {
      RMT_ASSERT ((exact_or_tcam == 1) && ((payload == 0) || (payload == 1)));
      return get_act_instr_addr_mapped(payload & 0x1, bus_or_tab, exact_or_tcam);
    }
    inline bool get_act_instr_map_enable(int bus_or_tab, int exact_or_tcam, int match=0) {
      uint16_t en = act_instr_map_en_.mau_action_instruction_adr_map_en(exact_or_tcam);
      return ((en & (1<<bus_or_tab)) != 0);
    }
    inline uint8_t get_act_instr_addr_shift(int bus_or_tab, int exact_or_tcam, int match=0) {
      if (exact_or_tcam == 0)
        return exact_.get_act_instr_addr_shift(bus_or_tab, match);
      else
        return tcam_.get_act_instr_addr_shift(bus_or_tab, match);
    }
    inline uint8_t get_act_instr_addr_perentry_enable(int bus_or_tab, int exact_or_tcam, int match=0) {
      uint8_t v = act_instr_perentry_en_.mau_action_instruction_adr_per_entry_en_mux_ctl(exact_or_tcam, bus_or_tab);
      //if ((v >> 5) != 0) { THROW_ERROR(-2); } // Reg is 7b wide but only 5b should be used
      return (v & 0x1f);
    }
    inline uint8_t get_act_instr_addr_mask(int bus_or_tab, int exact_or_tcam, int match=0) {
      return act_instr_mask_.mau_action_instruction_adr_mask(exact_or_tcam, bus_or_tab);
    }
    inline uint8_t get_act_instr_addr_dflt(int bus_or_tab, int exact_or_tcam, int match=0) {
      return act_instr_dflt_.mau_action_instruction_adr_default(exact_or_tcam, bus_or_tab);
    }
    inline uint8_t get_act_instr_addr_miss(int bus_or_tab, int exact_or_tcam, int match=0) {
      return act_instr_miss_.mau_action_instruction_adr_miss_value(bus_or_tab);
    }
    inline uint32_t get_act_instr_addr(uint64_t data, uint8_t shift, uint8_t mask, uint8_t dflt) {
      return ((static_cast<uint8_t>(data >> shift) & mask) | dflt);
    }
    // Only useful if we get shift/mask/default ALL from looking up SAME bus_or_tab
    inline uint8_t get_act_instr_addr(uint64_t data, int bus_or_tab, int exact_or_tcam, int match=0) {
      uint8_t s = get_act_instr_addr_shift(bus_or_tab, exact_or_tcam, match);
      uint8_t m = get_act_instr_addr_mask(bus_or_tab, exact_or_tcam, match);
      uint8_t d = get_act_instr_addr_dflt(bus_or_tab, exact_or_tcam, match);
      uint8_t v = get_act_instr_addr(data, s, m, d);
      return map_act_instr_addr(v, bus_or_tab, exact_or_tcam, match);
    }
    inline uint8_t get_act_instr_addr_mapped(uint8_t data, int bus_or_tab, int exact_or_tcam, int match=0) {
      return mau_chip_result_bus_.get_act_instr_addr_mapped(data, bus_or_tab, exact_or_tcam, match);
    }
    inline uint8_t map_act_instr_addr(uint8_t data, int bus_or_tab, int exact_or_tcam, int match=0) {
      if (get_act_instr_map_enable(bus_or_tab, exact_or_tcam, match))
        data = get_act_instr_addr_mapped(data, bus_or_tab, exact_or_tcam, match);
      return data;
    }
    inline bool get_act_instr_payload_shifter_enable(int bus_or_tab, int exact_or_tcam) {
      return (payload_shifter_enable_.action_instruction_adr_payload_shifter_en(exact_or_tcam, bus_or_tab) == 1);
    }


    // STATS ADDR actionbit enable/actionbit data/shift/perentry enable/make/default/miss/hole_swizzle_mode
    inline bool get_stats_addr_actionbit_map_enable(int bus_or_tab, int exact_or_tcam) {
      uint16_t en = stats_addr_actionbit_map_en_.mau_stats_adr_tcam_actionbit_map_en();
      return ((exact_or_tcam == 1) && ((en & (1<<bus_or_tab)) != 0));
    }
    inline uint32_t get_stats_addr_actionbit_map_data(int bus_or_tab, int exact_or_tcam,
                                                      int payload) {
      RMT_ASSERT ((exact_or_tcam == 1) && ((payload == 0) || (payload == 1)));
      return stats_addr_actionbit_map_data_.mau_stats_adr_tcam_actionbit_map_data(bus_or_tab,
                                                                                  payload & 0x1);
    }
    inline uint8_t get_stats_addr_shift(int bus_or_tab, int exact_or_tcam, int match=0) {
      if (exact_or_tcam == 0)
        return exact_.get_stats_addr_shift(bus_or_tab, match);
      else
        return tcam_.get_stats_addr_shift(bus_or_tab, match);
    }
    inline uint8_t get_stats_addr_perentry_enable(int bus_or_tab, int exact_or_tcam, int match=0) {
      uint8_t v = stats_addr_perentry_en_.mau_stats_adr_per_entry_en_mux_ctl(exact_or_tcam, bus_or_tab);
      //if ((v >> 5) != 0) { THROW_ERROR(-2); } // Reg is 7b wide but only 5b should be used
      return (v & 0x1f);
    }
    inline uint32_t get_stats_addr_mask(int bus_or_tab, int exact_or_tcam, int match=0) {
      return stats_addr_mask_.mau_stats_adr_mask(exact_or_tcam, bus_or_tab);
    }
    inline uint32_t get_stats_addr_dflt(int bus_or_tab, int exact_or_tcam, int match=0) {
      return stats_addr_dflt_.mau_stats_adr_default(exact_or_tcam, bus_or_tab);
    }
    inline uint32_t get_stats_addr_miss(int bus_or_tab, int exact_or_tcam, int match=0) {
      return stats_addr_miss_.mau_stats_adr_miss_value(bus_or_tab);
    }
    inline uint8_t get_stats_addr_hole_swizzle(int bus_or_tab, int exact_or_tcam, int match=0) {
      RMT_ASSERT((exact_or_tcam == 0) || (exact_or_tcam == 1));
      uint8_t sm = stats_addr_hole_swizzle_.mau_stats_adr_hole_swizzle_mode(exact_or_tcam, bus_or_tab);
      return (exact_or_tcam << 4) | (sm & 0xF);
    }
    inline uint32_t swizzle_stats_addr(uint32_t v, uint8_t swizzle_mode) {
      switch (swizzle_mode) {
        case 0x01: return swizzle_to_lsb(v,2,15); // exact_match, swizzle_mode 0x1
        case 0x02: return swizzle_to_lsb(v,2,14); // exact_match, swizzle_mode 0x2
        case 0x11: return swizzle_to_lsb(v,2,14); // tcam_match, swizzle_mode 0x1
        case 0x12: return swizzle_to_lsb(v,2,13); // tcam_match, swizzle_mode 0x2
        default:   return v;
      }
    }
    inline uint32_t get_stats_addr(uint64_t data, uint8_t shift, uint8_t msbit,
                                   uint32_t mask, uint32_t dflt, uint8_t swizzle_mode) {
      uint32_t data32 = static_cast<uint32_t>(data >> shift);
      uint32_t msb = ((data32 >> msbit) & 0x1) << (Address::kStatsAddrWidth-1);
      uint32_t v = (((data32 | msb) & mask) | dflt);
      return swizzle_stats_addr(v, swizzle_mode);
    }
    // Only useful if we get shift/mask/default ALL from looking up SAME bus_or_tab
    inline uint32_t get_stats_addr(uint64_t data, int bus_or_tab, int exact_or_tcam, int match=0) {
      uint8_t  s = get_stats_addr_shift(bus_or_tab, exact_or_tcam, match);
      uint8_t  b = get_stats_addr_perentry_enable(bus_or_tab, exact_or_tcam, match);
      uint32_t m = get_stats_addr_mask(bus_or_tab, exact_or_tcam, match);
      uint32_t d = get_stats_addr_dflt(bus_or_tab, exact_or_tcam, match);
      uint32_t h = get_stats_addr_hole_swizzle(bus_or_tab, exact_or_tcam, match);
      return get_stats_addr(data, s, b, m, d, h);
    }
    inline bool get_stats_addr_payload_shifter_enable(int bus_or_tab, int exact_or_tcam) {
      return (payload_shifter_enable_.stats_adr_payload_shifter_en(exact_or_tcam, bus_or_tab) == 1);
    }


    // METER ADDR actionbit enable/actionbit data/shift/perentry enable/make/default/miss
    inline bool get_meter_addr_actionbit_map_enable(int bus_or_tab, int exact_or_tcam) {
      uint16_t en = meter_addr_actionbit_map_en_.mau_meter_adr_tcam_actionbit_map_en();
      return ((exact_or_tcam == 1) && ((en & (1<<bus_or_tab)) != 0));
    }
    inline uint32_t get_meter_addr_actionbit_map_data(int bus_or_tab, int exact_or_tcam,
                                                      int payload) {
      RMT_ASSERT ((exact_or_tcam == 1) && ((payload == 0) || (payload == 1)));
      return meter_addr_actionbit_map_data_.mau_meter_adr_tcam_actionbit_map_data(bus_or_tab,
                                                                                  payload & 0x1);
    }
    inline uint8_t get_meter_addr_shift(int bus_or_tab, int exact_or_tcam, int match=0) {
      if (exact_or_tcam == 0)
        return exact_.get_meter_addr_shift(bus_or_tab, match);
      else
        return tcam_.get_meter_addr_shift(bus_or_tab, match);
    }
    inline uint8_t get_meter_addr_perentry_enable(int bus_or_tab, int exact_or_tcam, int match=0) {
      uint8_t v = meter_addr_perentry_en_.mau_meter_adr_per_entry_en_mux_ctl(exact_or_tcam, bus_or_tab);
      //if ((v >> 5) != 0) { THROW_ERROR(-2); } // Reg is 7b wide but only 5b should be used
      return (v & 0x1f);
    }
    inline uint32_t get_meter_addr_mask(int bus_or_tab, int exact_or_tcam, int match=0) {
      return meter_addr_mask_.mau_meter_adr_mask(exact_or_tcam, bus_or_tab);
    }
    inline uint32_t get_meter_addr_dflt(int bus_or_tab, int exact_or_tcam, int match=0) {
      return meter_addr_dflt_.mau_meter_adr_default(exact_or_tcam, bus_or_tab);
    }
    inline uint32_t get_meter_addr_miss(int bus_or_tab, int exact_or_tcam, int match=0) {
      uint32_t data32 = meter_addr_miss_.mau_meter_adr_miss_value(bus_or_tab);
      return data32;
    }
    inline uint32_t get_meter_addr(uint64_t data, uint8_t shift, uint8_t msbit,
                                   uint32_t mask, uint32_t dflt) {
      uint32_t data32 = static_cast<uint32_t>(data >> shift);
      uint32_t msb = ((data32 >> msbit) & 0x1) << (Address::kMeterAddrWidth-1);
      return (((data32 | msb) & mask) | dflt);
    }
    // Only useful if we get shift/mask/default ALL from looking up SAME bus_or_tab
    inline uint32_t get_meter_addr(uint64_t data, int bus_or_tab, int exact_or_tcam, int match=0) {
      uint8_t  s = get_meter_addr_shift(bus_or_tab, exact_or_tcam, match);
      uint8_t  b = get_meter_addr_perentry_enable(bus_or_tab, exact_or_tcam, match);
      uint32_t m = get_meter_addr_mask(bus_or_tab, exact_or_tcam, match);
      uint32_t d = get_meter_addr_dflt(bus_or_tab, exact_or_tcam, match);
      return get_meter_addr(data, s, b, m, d);
    }
    inline bool get_meter_addr_payload_shifter_enable(int bus_or_tab, int exact_or_tcam) {
      return (payload_shifter_enable_.meter_adr_payload_shifter_en(exact_or_tcam, bus_or_tab) == 1);
    }


    // IDLETIME ADDR actionbit enable/actionbit data/shift/perentry enable/make/default/miss
    inline bool get_idletime_addr_actionbit_map_enable(int bus_or_tab, int exact_or_tcam) {
      uint16_t en = idletime_addr_actionbit_map_en_.mau_idletime_adr_tcam_actionbit_map_en();
      return ((exact_or_tcam == 1) && ((en & (1<<bus_or_tab)) != 0));
    }
    inline uint32_t get_idletime_addr_actionbit_map_data(int bus_or_tab, int exact_or_tcam,
                                                         int payload) {
      RMT_ASSERT ((exact_or_tcam == 1) && ((payload == 0) || (payload == 1)));
      return idletime_addr_actionbit_map_data_.mau_idletime_adr_tcam_actionbit_map_data(bus_or_tab,
                                                                                        payload & 0x1);
    }
    inline uint8_t get_idletime_addr_shift(int bus_or_tab, int exact_or_tcam, int match=0) {
      if (exact_or_tcam == 0)
        return exact_.get_idle_addr_shift(bus_or_tab, match);
      else
        return tcam_.get_idle_addr_shift(bus_or_tab, match);
    }
    inline uint8_t get_idletime_addr_perentry_enable(int bus_or_tab, int exact_or_tcam, int match=0) {
      uint8_t v = idletime_addr_perentry_en_.mau_idletime_adr_per_entry_en_mux_ctl(exact_or_tcam, bus_or_tab);
      //if ((v >> 5) != 0) { THROW_ERROR(-2); } // Reg is 7b wide but only 5b should be used
      return (v & 0x1f);
    }
    inline uint32_t get_idletime_addr_mask(int bus_or_tab, int exact_or_tcam, int match=0) {
      return idletime_addr_mask_.mau_idletime_adr_mask(exact_or_tcam, bus_or_tab);
    }
    inline uint32_t get_idletime_addr_dflt(int bus_or_tab, int exact_or_tcam, int match=0) {
      return idletime_addr_dflt_.mau_idletime_adr_default(exact_or_tcam, bus_or_tab);
    }
    inline uint32_t get_idletime_addr_miss(int bus_or_tab, int exact_or_tcam, int match=0) {
      return idletime_addr_miss_.mau_idletime_adr_miss_value(bus_or_tab);
    }
    inline uint32_t get_idletime_addr(uint64_t data, uint8_t shift, uint8_t msbit,
                                      uint32_t mask, uint32_t dflt) {
      uint32_t data32 = static_cast<uint32_t>(data >> shift);
      uint32_t msb = ((data32 >> msbit) & 0x1) << (Address::kIdletimeAddrWidth-1);
      return (((data32 | msb) & mask) | dflt);
    }
    // Only useful if we get shift/mask/default ALL from looking up SAME bus_or_tab
    inline uint32_t get_idletime_addr(uint64_t data, int bus_or_tab, int exact_or_tcam, int match=0) {
      uint8_t  s = get_idletime_addr_shift(bus_or_tab, exact_or_tcam, match);
      uint8_t  b = get_idletime_addr_perentry_enable(bus_or_tab, exact_or_tcam, match);
      uint32_t m = get_idletime_addr_mask(bus_or_tab, exact_or_tcam, match);
      uint32_t d = get_idletime_addr_dflt(bus_or_tab, exact_or_tcam, match);
      return get_idletime_addr(data, s, b, m, d);
    }
    inline bool get_idletime_addr_payload_shifter_enable(int bus_or_tab, int exact_or_tcam) {
      return (payload_shifter_enable_.idletime_adr_payload_shifter_en(exact_or_tcam, bus_or_tab) == 1);
    }

    // Figure out ActionHVBus byte positions for immediate data bytes
    inline int get_action_hv_bus_imm_bytepos_32(int tab, int which_byte) {
      RMT_ASSERT ((which_byte >= 0) && (which_byte <= 3));
      int index = tab; // Just use logical table number as index
      // TODO: 29733_mau_dev: Just changed 5bit to 4bit
      if (!immediate_data_32b_ixbar_ctl_.enabled_4bit_muxctl_enable(index)) return -1;
      int word = immediate_data_32b_ixbar_ctl_.enabled_4bit_muxctl_select(index);
      return (word * 4) + kXbarBytes + (2*kXbarHalfWords) + which_byte;
    }
    inline int ix_ctl_index_8_16(int tab, int which_byte) {
      RMT_ASSERT ((which_byte >= 0) && (which_byte <= 3));
      // There are two ixbar configs, one for the high 2 bytes and one for the low 2 bytes
      return (tab << 1) + (which_byte >> 1);
    }
    inline int get_action_hv_bus_imm_bytepos_16(int tab, int which_byte) {
      RMT_ASSERT ((which_byte >= 0) && (which_byte <= 3));
      int ix_index = ix_ctl_index_8_16(tab,which_byte);
      if (!immediate_data_16b_ixbar_ctl_.enabled_4bit_muxctl_enable(ix_index)) return -1;
      int word = immediate_data_16b_ixbar_ctl_.enabled_4bit_muxctl_select(ix_index);
      RMT_ASSERT( word < (kXbarHalfWords/2));
      return (word * 4) + kXbarBytes + which_byte;
    }
    inline int get_action_hv_bus_imm_bytepos_8(int tab, int which_byte) {
      RMT_ASSERT ((which_byte >= 0) && (which_byte <= 3));

      // each byte has an individual enable
      int byte_num = (tab << 2) | (which_byte);  // overall byte_num 0-63
      uint32_t enable = immediate_data_8b_enable_.immediate_data_8b_enable( byte_num/32 );
      int enable_bit = byte_num % 32;
      if ( 0 == ((enable>>enable_bit)&1) ) return -1;

      int ix_index = ix_ctl_index_8_16(tab,which_byte);
      if (!immediate_data_8b_ixbar_ctl_.enabled_2bit_muxctl_enable(ix_index)) return -1;
      int word = which_byte +
          (4 * immediate_data_8b_ixbar_ctl_.enabled_2bit_muxctl_select(ix_index));

      return word;
    }
    inline void get_action_hv_bus_imm_bytepos(int tab, int which_byte,
                                              int *posA, int *posB, int *posC) {
      if (posA != NULL) *posA = get_action_hv_bus_imm_bytepos_32(tab, which_byte);
      if (posB != NULL) *posB = get_action_hv_bus_imm_bytepos_16(tab, which_byte);
      if (posC != NULL) *posC = get_action_hv_bus_imm_bytepos_8(tab, which_byte);
    }

    inline void get_imm_data_rng_ctl(int tab, uint8_t *which_rng, uint8_t *byte_mask) {
      RMT_ASSERT((tab >= 0) && (tab < kLogicalTables));
      RMT_ASSERT((which_rng != NULL) && (byte_mask != NULL));
      *which_rng = 0; *byte_mask = 0;
      if (imm_data_rng_enable_.immediate_data_rng_enable() == 0) return;
      uint32_t reg = imm_data_rng_logical_map_ctl_.immediate_data_rng_logical_map_ctl(tab/4);
      uint32_t ctl = (reg >> ((tab%4)*5)) & 0x1F;
      *which_rng = ctl >> 4; // 0 or 1
      *byte_mask = ctl & 0xF;
    }

    // Figure out which logical table is connected to a given nxtable bus
    inline int get_logical_table_for_nxtab_bus(int xm_tm, int nxtab_bus) {
      RMT_ASSERT((xm_tm == 0) || (xm_tm == 1));
      if ((nxtab_bus < 0) || (nxtab_bus >= kOutputBuses)) return -1;
      if (match_to_logical_table_ixbar_.enabled_4bit_muxctl_enable(xm_tm+2,nxtab_bus)==0)
        return -1;
      return match_to_logical_table_ixbar_.enabled_4bit_muxctl_select(xm_tm+2,nxtab_bus);
    }

    // Lookup result buses mapped to a logical table
    // Calculated from match_to_logical_table_ixbar (which maps the other way)
    inline uint16_t get_buses(int exact_or_tcam, int logical_table) {
      RMT_ASSERT((exact_or_tcam == 0) || (exact_or_tcam == 1));
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      spinlock_.lock();
      uint16_t buses = ltab_to_bus_[exact_or_tcam][logical_table];
      spinlock_.unlock();
      return buses;
    }
    inline uint16_t get_match_buses(int logical_table) {
      return get_buses(0, logical_table);
    }
    inline uint16_t get_tind_buses(int logical_table) {
      return get_buses(1, logical_table);
    }
    inline uint16_t get_meter_alu_buses(int exact_or_tcam, int meter_alu) {
      RMT_ASSERT((exact_or_tcam == 0) || (exact_or_tcam == 1));
      RMT_ASSERT((meter_alu >= 0) && (meter_alu < kNumMeterAlus));
      spinlock_.lock();
      uint16_t buses = meter_alu_to_bus_[exact_or_tcam][meter_alu];
      spinlock_.unlock();
      return buses;
    }
    inline int get_meter_alu_for_logical_table(int logical_table) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      int idx = (logical_table / 8);
      int shift = (logical_table % 8) * 3;
      uint32_t reg = meter_alu_to_logical_table_oxbar_.mau_meter_alu_to_logical_map(idx);
      uint32_t ctl = reg >> shift;
      if ((ctl & 0x4) == 0u) return -1;
      return static_cast<int>(ctl & 0x3);
    }
    std::vector<int> get_meter_alus_for_logical_table(int logical_table) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      return mau_chip_result_bus_.get_meter_alus_for_logical_table(logical_table);
    }
    bool get_gateway_payload_exact_shift_ovr(int bus) {
      return mau_chip_result_bus_.get_gateway_payload_exact_shift_ovr(bus);
    }



    // Lookup LTCAMs map to a logical table
    inline uint8_t get_ltcams(int logical_table) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      spinlock_.lock();
      uint8_t ltcams = ltab_to_ltcams_[logical_table];
      spinlock_.unlock();
      return ltcams;
    }


    // Just lookup to find which LTCAM drives TindBus
    inline int get_ltcam(int tind_bus) {
      if ((tind_bus < 0) || (tind_bus > MauDefs::kTindOutputBusesPerMau)) return -1;
      if (!ltcam_tind_outbus_map_.enabled_3bit_muxctl_enable(tind_bus)) return -1;
      return ltcam_tind_outbus_map_.enabled_3bit_muxctl_select(tind_bus);
    }

    inline uint8_t get_tind_ram_data_size(int tind_bus) {
      return tind_ram_data_size_.tind_ram_data_size(tind_bus) & 0x7;
    }

    // SELECTOR table related code...
    /* Read selector-num and selector shift value using shift, mask , default regsiters */
    inline uint8_t get_selectorlength_shift(int bus, int exact_or_tcam) {
      return selector_length_shift_.mau_selectorlength_shiftcount(exact_or_tcam, bus);
    }
    inline uint32_t get_selectorlength_mask(int bus, int exact_or_tcam) {
      return selector_length_mask_.mau_selectorlength_mask(exact_or_tcam, bus);
    }
    inline uint32_t get_selectorlength_dflt(int bus, int exact_or_tcam) {
      return selector_length_dflt_.mau_selectorlength_default(exact_or_tcam, bus);
    }
    inline uint32_t get_selectorlength_value(uint64_t data, uint8_t shift, uint32_t mask, uint32_t dflt) {
      return ((static_cast<uint32_t>(data >> shift) & mask) | dflt);
    }
    inline uint32_t get_selectorlength_shiftcount(uint64_t data, int bus, int exact_or_tcam) {
      uint8_t  s = get_selectorlength_shift(bus, exact_or_tcam);
      uint32_t m = get_selectorlength_mask(bus, exact_or_tcam);
      //The default reg is 11 bits out of which lower 8 are the actual default
      // value to or in. Rest 3 indicate type
      uint32_t d = get_selectorlength_dflt(bus, exact_or_tcam) & 0xFF;
      return get_selectorlength_value(data, s, m, d);
    }

    ///// Meter color related code

    // returns which color buses are used by this logical table
    uint8_t get_which_color_buses( int logical_table ) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      return mau_chip_result_bus_.get_which_color_buses(logical_table);
    }
    bool get_color_read_oflo_enable( int which_color_bus ) {
      switch (which_color_bus) {
        case 0: return false; // no overflow
        case 1: return false; // no overflow
        case 2: return 1 & mapram_read_color_oflo_ctl_.mau_match_central_mapram_read_color_oflo_ctl();
        case 3: return 2 & mapram_read_color_oflo_ctl_.mau_match_central_mapram_read_color_oflo_ctl();
        default: return false;
      }
    }

    uint8_t map_color(int color, int logical_table) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      RMT_ASSERT( color < 4 );
      uint32_t v = meter_color_output_map_.meter_color_output_map( logical_table );
      uint8_t mapped = 0xff & ( v >> (8*color));
      return mapped;
    }

    inline uint8_t get_meter_adr_type_position(int bus, int exact_or_tcam) {
      uint8_t v = meter_adr_type_position_.mau_meter_adr_type_position(exact_or_tcam, bus);
      //if ((v >> 5) != 0) { THROW_ERROR(-2); } // Reg is 7b wide but only 5b should be used
      return (v & 0x1f);
    }

    inline bool get_meter_enable(int logical_table) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      return (meter_enable_.meter_enable() >> logical_table) & 1;
    }


    // Verify virtual access allowed to stats ALU
    // Check all ALUs listed in stats_icxbar allow access for this LT
    inline bool stats_virt_allow(int logical_table) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      uint32_t alus = adr_dist_stats_icxbar_.adr_dist_stats_adr_icxbar_ctl(logical_table);
      uint32_t alus_allowed = 0u;
      for (int alu = 0; alu < kNumStatsAlus; alu++) {
        if ((alus & (1u<<alu)) != 0u) {
          uint32_t lts = mau_ad_stats_virt_lt_.mau_ad_stats_virt_lt(alu);
          if ((lts & (1u<<logical_table)) != 0u) alus_allowed |= (1u<<alu);
        }
      }
      return ((alus != 0u) && (alus == alus_allowed));
    }
    // Verify virtual access allowed to meter ALU
    // Check all ALUs listed in meter_icxbar allow access for this LT
    inline bool meter_virt_allow(int logical_table) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      uint32_t alus = adr_dist_meter_icxbar_.adr_dist_meter_adr_icxbar_ctl(logical_table);
      uint32_t alus_allowed = 0u;
      for (int alu = 0; alu < kNumMeterAlus; alu++) {
        if ((alus & (1u<<alu)) != 0u) {
          uint32_t lts = mau_ad_meter_virt_lt_.mau_ad_meter_virt_lt(alu);
          if ((lts & (1u<<logical_table)) != 0u) alus_allowed |= (1u<<alu);
        }
      }
      return ((alus != 0u) && (alus == alus_allowed));
    }


private:
    void match_to_logical_table_ixbar_callback(uint32_t xm_tm, uint32_t bus);
    void tcam_hit_to_logical_table_ixbar_callback(uint32_t ltcam);
    void phys_to_meter_alu_ixbar_callback(uint32_t xm_tm, uint32_t bus_grp);   // tofino only
    void phys_to_meter_alu_icxbar_callback(uint32_t xm_tm, uint32_t bus_grp);  // jbay only


    register_classes::NextTableTcamActionbitMapEn                next_table_actionbit_map_en_;

    register_classes::MauImmediateDataTcamActionbitMapEn         imm_data_actionbit_map_en_;
    register_classes::MauImmediateDataTcamActionbitMapDataArray2 imm_data_actionbit_map_data_;
    register_classes::MauImmediateDataMaskArray2                 imm_data_mask_;
    register_classes::MauImmediateDataDefaultArray2              imm_data_dflt_;
    register_classes::MauImmediateDataMissValueArray             imm_data_miss_;

    register_classes::MauActiondataAdrTcamActionbitMapEn         act_data_addr_actionbit_map_en_;
    register_classes::MauActiondataAdrTcamActionbitMapDataArray2 act_data_addr_actionbit_map_data_;
    register_classes::MauActiondataAdrVpnShiftcountArray2        act_data_addr_vpn_shift_;
    register_classes::MauActiondataAdrPerEntryEnMuxCtlArray2     act_data_addr_perentry_en_;
    register_classes::MauActiondataAdrMaskArray2                 act_data_addr_mask_;
    register_classes::MauActiondataAdrDefaultArray2              act_data_addr_dflt_;
    register_classes::MauActiondataAdrMissValueArray             act_data_addr_miss_;

    register_classes::MauActionInstructionAdrTcamActionbitMapEn  act_instr_actionbit_map_en_;
    register_classes::MauActionInstructionAdrMapEnArray          act_instr_map_en_;
    register_classes::MauActionInstructionAdrPerEntryEnMuxCtlArray2  act_instr_perentry_en_;
    register_classes::MauActionInstructionAdrMaskArray2          act_instr_mask_;
    register_classes::MauActionInstructionAdrDefaultArray2       act_instr_dflt_;
    register_classes::MauActionInstructionAdrMissValueArray      act_instr_miss_;

    register_classes::MauStatsAdrTcamActionbitMapEn              stats_addr_actionbit_map_en_;
    register_classes::MauStatsAdrTcamActionbitMapDataArray2      stats_addr_actionbit_map_data_;
    register_classes::MauStatsAdrPerEntryEnMuxCtlArray2          stats_addr_perentry_en_;
    register_classes::MauStatsAdrMaskArray2                      stats_addr_mask_;
    register_classes::MauStatsAdrDefaultArray2                   stats_addr_dflt_;
    register_classes::MauStatsAdrMissValueArray                  stats_addr_miss_;
    register_classes::MauStatsAdrHoleSwizzleModeArray2           stats_addr_hole_swizzle_;

    register_classes::MauMeterAdrTcamActionbitMapEn              meter_addr_actionbit_map_en_;
    register_classes::MauMeterAdrTcamActionbitMapDataArray2      meter_addr_actionbit_map_data_;
    register_classes::MauMeterAdrPerEntryEnMuxCtlArray2          meter_addr_perentry_en_;
    register_classes::MauMeterAdrMaskArray2                      meter_addr_mask_;
    register_classes::MauMeterAdrDefaultArray2                   meter_addr_dflt_;
    register_classes::MauMeterAdrMissValueArray                  meter_addr_miss_;

    register_classes::MauIdletimeAdrTcamActionbitMapEn           idletime_addr_actionbit_map_en_;
    register_classes::MauIdletimeAdrTcamActionbitMapDataArray2   idletime_addr_actionbit_map_data_;
    register_classes::MauIdletimeAdrPerEntryEnMuxCtlArray2       idletime_addr_perentry_en_;
    register_classes::MauIdletimeAdrMaskArray2                   idletime_addr_mask_;
    register_classes::MauIdletimeAdrDefaultArray2                idletime_addr_dflt_;
    register_classes::MauIdletimeAdrMissValueArray               idletime_addr_miss_;

    register_classes::MauPayloadShifterEnableArray2              payload_shifter_enable_;

    register_classes::TindRamDataSizeArray                       tind_ram_data_size_;
    register_classes::ImmediateData_32bIxbarCtlArray             immediate_data_32b_ixbar_ctl_;
    register_classes::ImmediateData_16bIxbarCtlArray             immediate_data_16b_ixbar_ctl_;
    register_classes::ImmediateData_8bIxbarCtlArray              immediate_data_8b_ixbar_ctl_;
    register_classes::ImmediateData_8bEnableArray                immediate_data_8b_enable_;
    register_classes::ImmediateDataRngLogicalMapCtlArray         imm_data_rng_logical_map_ctl_;
    register_classes::ImmediateDataRngEnable                     imm_data_rng_enable_;

    register_classes::MatchToLogicalTableIxbarOutputmapArray2    match_to_logical_table_ixbar_;
    register_classes::TcamHitToLogicalTableIxbarOutputmapArray   tcam_hit_to_logical_table_ixbar_;
    register_classes::MauPhysicalToMeterAluIxbarMapArray2        phys_to_meter_alu_ixbar_;  // tofino only
    register_classes::MauPhysicalToMeterAluIcxbarMapArray2       phys_to_meter_alu_icxbar_; // jbay only
    register_classes::MauMeterAluToLogicalMapArray               meter_alu_to_logical_table_oxbar_;
    register_classes::TcamMatchAdrToPhysicalOxbarOutputmapArray  ltcam_tind_outbus_map_;

    register_classes::MauSelectorlengthDefaultArray2             selector_length_dflt_;
    register_classes::MauSelectorlengthMaskArray2                selector_length_mask_;
    register_classes::MauSelectorlengthShiftcountArray2          selector_length_shift_;

    register_classes::MauMatchCentralMapramReadColorOfloCtl      mapram_read_color_oflo_ctl_;

    register_classes::MeterColorOutputMapArray                   meter_color_output_map_;
    register_classes::MauMeterAdrTypePositionArray2              meter_adr_type_position_;

    register_classes::MeterEnable                                meter_enable_;

    register_classes::AdrDistStatsAdrIcxbarCtlArray              adr_dist_stats_icxbar_;
    register_classes::AdrDistMeterAdrIcxbarCtlArray              adr_dist_meter_icxbar_;
    register_classes::MauAdStatsVirtLtArray                      mau_ad_stats_virt_lt_;
    register_classes::MauAdMeterVirtLtArray                      mau_ad_meter_virt_lt_;

    register_classes::MauScratch                                 mau_scratch_;

    std::array< std::array<uint16_t,kLogicalTables>, 2>      ltab_to_bus_;
    std::array< uint8_t, kLogicalTables>                     ltab_to_ltcams_;
    std::array< std::array<uint16_t,kNumMeterAlus>, 2>       meter_alu_to_bus_;
    model_core::Spinlock                                     spinlock_;
    MauResultBusRegExact                                     exact_;
    MauResultBusRegTcam                                      tcam_;
    MauChipResultBus                                         mau_chip_result_bus_;

  };
}
#endif // _SHARED_MAU_RESULT_BUS_
