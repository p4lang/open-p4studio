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

#include <mau.h>
#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-result-bus.h>


namespace MODEL_CHIP_NAMESPACE {


void MauResultBus::match_to_logical_table_ixbar_callback(uint32_t xm_tm, uint32_t bus) {
  // Callback xm_tm/bus changed WHERE  m_t_lt_ixbar[xm_tm][bus] == ltab
  // In regs_25957_mau_dev this register was duplicated for PD reasons hence
  // the early return if xm_tm == 2 or xm_tm == 3
  // Now 2 and 3 are explicitly just for next_table so ignore
  if ((xm_tm == 2) || (xm_tm == 3)) return;
  RMT_ASSERT((xm_tm == 0) || (xm_tm == 1));
  RMT_ASSERT(bus < kOutputBuses);
  int new_ltab = -1;
  if (match_to_logical_table_ixbar_.enabled_4bit_muxctl_enable(xm_tm,bus)) {
    new_ltab = match_to_logical_table_ixbar_.enabled_4bit_muxctl_select(xm_tm,bus);
    RMT_ASSERT((new_ltab >= 0) && (new_ltab < kLogicalTables));
  }
  spinlock_.lock();
  // Go through ltab_to_bus_ clearing bus from any previous logical table
  for (int lt = 0; lt < kLogicalTables; lt++) ltab_to_bus_[xm_tm][lt] &= ~(1 << bus);
  // And setting bus for new logical tabale
  if (new_ltab >= 0) ltab_to_bus_[xm_tm][new_ltab] |= (1 << bus);
  spinlock_.unlock();
  //if (new_ltab >= 0)
  //printf("ltab_to_bus_[%d][%d] = 0x%04x\n", xm_tm, new_ltab, ltab_to_bus_[xm_tm][new_ltab]);
}

void MauResultBus::tcam_hit_to_logical_table_ixbar_callback(uint32_t ltcam) {
  RMT_ASSERT(ltcam < kLogicalTcams);
  int new_ltab = -1;
  if (tcam_hit_to_logical_table_ixbar_.enabled_4bit_muxctl_enable(ltcam)) {
    new_ltab = tcam_hit_to_logical_table_ixbar_.enabled_4bit_muxctl_select(ltcam);
    RMT_ASSERT((new_ltab >= 0) && (new_ltab < kLogicalTables));
  }
  spinlock_.lock();
  // Go through ltab_to_ltcams_ clearing ltcam from any previous logical table
  for (int lt = 0; lt < kLogicalTables; lt++) ltab_to_ltcams_[lt] &= ~(1 << ltcam);
  // And setting ltcam for new logical table
  if (new_ltab >= 0) ltab_to_ltcams_[new_ltab] |= (1 << ltcam);
  spinlock_.unlock();
}
// tofino only
void MauResultBus::phys_to_meter_alu_ixbar_callback(uint32_t xm_tm, uint32_t bus_grp) {
  // Callback xm_tm/bus_grp changed WHERE p_t_ma_ixbar[xm_tm][bus_grp] == 8x ALU_val_for_bus
  RMT_ASSERT((xm_tm == 0) || (xm_tm == 1));
  RMT_ASSERT((bus_grp == 0) || (bus_grp == 1));
  uint32_t alus = phys_to_meter_alu_ixbar_.mau_physical_to_meter_alu_ixbar_map(xm_tm,bus_grp);
  spinlock_.lock();
  for (int bus_grp_idx = 0; bus_grp_idx < 8; bus_grp_idx++) {
    uint32_t bus = (bus_grp * 8) + bus_grp_idx; // So a value in 0..15
    uint32_t ctl = alus >> (bus_grp_idx * 3);   // Pick out 3b ctrl field (1b EN, 2b ALU)
    // Go through meter_alu_to_bus_ clearing bus from any previous ALU
    for (int i = 0; i < kNumMeterAlus; i++) meter_alu_to_bus_[xm_tm][i] &= ~(1 << bus);
    if ((ctl & 0x4) != 0) {
      // ALU alu is enabled to use BUS bus, so set bus in meter_alu_to_bus_
      int alu = static_cast<int>(ctl & 0x3);
      meter_alu_to_bus_[xm_tm][alu] |= (1 << bus);
    }
  }
  spinlock_.unlock();
}

// jbay only
void MauResultBus::phys_to_meter_alu_icxbar_callback(uint32_t xm_tm, uint32_t bus_grp) {
  RMT_ASSERT((xm_tm == 0) || (xm_tm == 1));
  RMT_ASSERT((bus_grp == 0) || (bus_grp == 1));
  uint32_t alus = phys_to_meter_alu_icxbar_.mau_physical_to_meter_alu_icxbar_map(xm_tm,bus_grp);
  spinlock_.lock();
  for (int bus_grp_idx = 0; bus_grp_idx < 8; bus_grp_idx++) {
    uint32_t bus = (bus_grp * 8) + bus_grp_idx; // So a value in 0..15
    uint32_t v   = alus >> (bus_grp_idx * 4);   // Pick out 4b vector of ALUs that bus is mapped to

    for (int alu = 0; alu < kNumMeterAlus; ++alu) {
      if (((v >> alu) & 1) == 1) {
        meter_alu_to_bus_[xm_tm][alu] |= (1 << bus);  // set the corresponding bit
      } else {
        meter_alu_to_bus_[xm_tm][alu] &= ~(1 << bus); // clear the corresponding bit
      }
    }
  }
  spinlock_.unlock();
}

}
