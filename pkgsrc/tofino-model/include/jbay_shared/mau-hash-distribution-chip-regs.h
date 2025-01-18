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

#ifndef _JBAY_MAU_HASH_DISTRIBUTION_CHIP_REGS_H_
#define _JBAY_MAU_HASH_DISTRIBUTION_CHIP_REGS_H_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <rmt-log.h>
#include <mau-defs.h>
#include <address.h>
#include <register_adapters.h>

#include <register_includes/selector_action_adr_shift_array.h>
#include <register_includes/action_adr_vpn_mod_enable_array.h>
#include <register_includes/meter_group_table_vpn_mod_enable_array.h>
#include <register_includes/meter_group_table_vpn_max.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauHashDistributionChipRegs : public MauObject {

    static constexpr int kNumAlus = MauDefs::kNumMeterAlus;

 public:
    MauHashDistributionChipRegs(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau) :
        MauObject(om, pipeIndex, mauIndex, mau),
        selector_action_adr_shift_(default_adapter(selector_action_adr_shift_, chip_index(), pipeIndex, mauIndex)),
        action_adr_vpn_mod_enable_(default_adapter(action_adr_vpn_mod_enable_, chip_index(), pipeIndex, mauIndex)),
        meter_group_table_vpn_mod_enable_(default_adapter(meter_group_table_vpn_mod_enable_, chip_index(), pipeIndex, mauIndex)),
        meter_group_table_vpn_max_(default_adapter(meter_group_table_vpn_max_, chip_index(), pipeIndex, mauIndex))
    {
      selector_action_adr_shift_.reset();
      action_adr_vpn_mod_enable_.reset();
      meter_group_table_vpn_mod_enable_.reset();
      meter_group_table_vpn_max_.reset();
    }
    ~MauHashDistributionChipRegs() { }

 private:
    inline bool action_vpn_mod_enabled(int alu) {
      // Why can't these things be 2d arrays?!
      RMT_ASSERT((alu >= 0) && (alu < kNumAlus));
      uint8_t val0 = 0, val1 = 0;
      switch (alu) {
        case 0: val0 = action_adr_vpn_mod_enable_.group0_action_adr_vpn_mod_enable(0); break;
        case 1: val0 = action_adr_vpn_mod_enable_.group1_action_adr_vpn_mod_enable(0); break;
        case 2: val0 = action_adr_vpn_mod_enable_.group2_action_adr_vpn_mod_enable(0); break;
        case 3: val0 = action_adr_vpn_mod_enable_.group3_action_adr_vpn_mod_enable(0); break;
        default: RMT_ASSERT(0);
      }
      switch (alu) {
        case 0: val1 = action_adr_vpn_mod_enable_.group0_action_adr_vpn_mod_enable(1); break;
        case 1: val1 = action_adr_vpn_mod_enable_.group1_action_adr_vpn_mod_enable(1); break;
        case 2: val1 = action_adr_vpn_mod_enable_.group2_action_adr_vpn_mod_enable(1); break;
        case 3: val1 = action_adr_vpn_mod_enable_.group3_action_adr_vpn_mod_enable(1); break;
        default: RMT_ASSERT(0);
      }
      RMT_ASSERT((val0 == val1) && "action_adr_vpn_mod_enable not replicated correctly");
      return ((val0 & 1) == 1);
    }

    inline bool meter_vpn_mod_enabled(int alu) {
      RMT_ASSERT((alu >= 0) && (alu < kNumAlus));
      uint8_t val0 = 0, val1 = 0;
      switch (alu) {
        case 0: val0 = meter_group_table_vpn_mod_enable_.group0_vpn_mod_enable(0); break;
        case 1: val0 = meter_group_table_vpn_mod_enable_.group1_vpn_mod_enable(0); break;
        case 2: val0 = meter_group_table_vpn_mod_enable_.group2_vpn_mod_enable(0); break;
        case 3: val0 = meter_group_table_vpn_mod_enable_.group3_vpn_mod_enable(0); break;
        default: RMT_ASSERT(0);
      }
      switch (alu) {
        case 0: val1 = meter_group_table_vpn_mod_enable_.group0_vpn_mod_enable(1); break;
        case 1: val1 = meter_group_table_vpn_mod_enable_.group1_vpn_mod_enable(1); break;
        case 2: val1 = meter_group_table_vpn_mod_enable_.group2_vpn_mod_enable(1); break;
        case 3: val1 = meter_group_table_vpn_mod_enable_.group3_vpn_mod_enable(1); break;
        default: RMT_ASSERT(0);
      }
      RMT_ASSERT((val0 == val1) && "meter_group_table_vpn_mod_enable not replicated correctly");
      return ((val0 & 1) == 1);
    }

    inline bool vpn_mod_enabled(int alu, uint8_t addr_type) {
      switch (addr_type) {
        case AddrType::kAction: return action_vpn_mod_enabled(alu);
        case AddrType::kMeter:  return meter_vpn_mod_enabled(alu);
        default: RMT_ASSERT(0);
      }
    }

    inline uint8_t meter_vpn_max(int alu) {
      RMT_ASSERT((alu >= 0) && (alu < kNumAlus));
      switch (alu) {
        case 0: return meter_group_table_vpn_max_.group0_vpn_max();
        case 1: return meter_group_table_vpn_max_.group1_vpn_max();
        case 2: return meter_group_table_vpn_max_.group2_vpn_max();
        case 3: return meter_group_table_vpn_max_.group3_vpn_max();
        default: RMT_ASSERT(0);
      }
    }

 public:
    inline int selector_mod_shift_fixup(int alu) {
      RMT_ASSERT((alu >= 0) && (alu < kNumAlus));
      // Purpose of this is to fixup hashmod shift to allow mod to go
      // into MSBs of Selector ALU Live Index (see JBay uArch fig 6-40)
      // Then Selector/Stateful ALU emits 3b/4b qLAG index to fill rest.
      //
      // Result here is a delta to apply to the shift returned by
      // MauHashDistributionRegs::selector_action_entry_shift()
      switch (selector_action_adr_shift_.selector_action_adr_shift(alu)) {
        case 0:  return  0;
        case 1:  return -4;
        case 2:  return -3;
        default: RMT_ASSERT(0 && "Bad value selector_action_adr_shift for ALU");
      }
    }
    inline bool selector_mod_is_vpn(int alu, uint8_t addr_type) {
      return vpn_mod_enabled(alu, addr_type);
    }

    inline uint32_t selector_dividend(int alu, uint8_t addr_type, uint32_t hash_input) {
      bool vpn_mod_en = vpn_mod_enabled(alu, addr_type);
      // Return low 14b of hash_input if VPN mod enabled for addr_type else just low 10b
      return vpn_mod_en ?((hash_input >> 0) & 0x3FFFu) :((hash_input >> 0) & 0x3FFu);
    }

    inline uint8_t selector_divisor(int alu, uint8_t addr_type, uint32_t selector_len) {
      bool vpn_mod_en = vpn_mod_enabled(alu, addr_type);
      uint8_t vpn_max = meter_vpn_max(alu);
      if (vpn_mod_en) {
        const char *reg_pfx = (addr_type==AddrType::kAction) ?"action_adr" :"meter_group_table";
        if (selector_len != 0u) {
          RMT_LOG(RmtDebug::warn(),
                  "MauHashDistributionChipRegs: %s_vpn_mod_enable for ALU %d is 1 but "
                  "selector_len is non-zero (%d)\n", reg_pfx, alu, selector_len);
        }
        if (vpn_max == 0) {
          RMT_LOG(RmtDebug::warn(),
                  "MauHashDistributionChipRegs: %s_vpn_mod_enable for ALU %d is 1 but "
                  "meter_group_table_vpn_max is 0\n", reg_pfx, alu);
        }
      }
      // Return low 6b of VPN if VPN mod enabled for addr_type otherwise low 5b of selector_len
      return vpn_mod_en ?((vpn_max >> 0) & 0x3F) :static_cast<uint8_t>((selector_len >> 0) & 0x1Fu);
    }

 private:
    register_classes::SelectorActionAdrShiftArray       selector_action_adr_shift_;
    register_classes::ActionAdrVpnModEnableArray        action_adr_vpn_mod_enable_;
    register_classes::MeterGroupTableVpnModEnableArray  meter_group_table_vpn_mod_enable_;
    register_classes::MeterGroupTableVpnMax             meter_group_table_vpn_max_;
  };
}

#endif // _JBAY_MAU_HASH_DISTRIBUTION_CHIP_REGS_H_
