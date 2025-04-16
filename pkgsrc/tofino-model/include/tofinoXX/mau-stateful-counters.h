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

#ifndef _TOFINOXX_MAU_STATEFUL_COUNTERS_
#define _TOFINOXX_MAU_STATEFUL_COUNTERS_

#include <vector>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <mau-addr-dist.h>
#include <mau-lookup-result.h>
#include <mau-execute-step.h>

#include <register_includes/mau_stateful_log_counter_array2_mutable.h>
#include <register_includes/mau_stateful_log_counter_ctl_array.h>
#include <register_includes/mau_stateful_log_counter_clear_mutable.h>
#include <register_includes/mau_stateful_log_vpn_offset_array.h>
#include <register_includes/mau_stateful_log_vpn_limit_array.h>
#include <register_includes/mau_stateful_log_vpn_hole_en.h>
#include <register_includes/mau_stateful_log_instruction_width.h>
#include <register_includes/mau_stateful_log_ctl_ixbar_map_array2.h>

namespace MODEL_CHIP_NAMESPACE {

  class MauStatefulCounters : public MauObject {
    static constexpr int      kType = RmtTypes::kRmtTypeMauTableCounters; // TODO: add another type
    static constexpr int      kLogicalTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int      kNumAlus = MauDefs::kNumAlus;
    static constexpr int      kSramAddressWidth  = MauDefs::kSramAddressWidth;
    static constexpr int      kSramEntries = 1<<kSramAddressWidth;
    static constexpr int      kStatefulAddrVpnPos = 17;
    static constexpr int      kStatefulAddrVpnBits = MauDefs::kMapramVpnBits;
    static constexpr int      kStatefulAddrVpnMax  = MauDefs::kMapramVpnMax;
    static constexpr uint32_t kStatefulAddrVpnMask = (1u<<kStatefulAddrVpnBits)-1;
    static constexpr uint32_t kStatefulAddrMask = (1u<<(kStatefulAddrVpnPos+kStatefulAddrVpnBits))-1;
    static constexpr uint32_t kTableCntrMask = 0xFFFFFFFFu;
    static_assert( (kStatefulAddrVpnBits <= 8),
                   "VPN hole handling assumes VPN fits in uint8_t");

 public:
    static bool kRelaxSwPushPopInvalidErrors; // Defined in rmt-config.cpp
    static bool kRelaxSwPushPopOverflowUnderflowErrors;
    static bool kSynchronousStatefulCounterClear;
    static bool kStatefulCounterTickCheckTime;

    MauStatefulCounters(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    ~MauStatefulCounters();

    bool stateful_counter_enabled(int lt);
    bool stateful_counter_at_max(int alu);
    uint32_t maybe_increment_stateful_counter(uint32_t addr, int lt, const MauLookupResult &res);
    void tick_counters( const std::array<MauLookupResult,kLogicalTables> &lookup_results, MauExecuteState* state ) {
      // does nothing on Tofino as counters are ticked in maybe_increment_stateful_counter()
    }

    void reset_resources() { // For DV, reset per-ALU meter_addr
      for (int i = 0; i < kNumAlus; i++) final_addrs_[i] = 0u;
    }
    uint32_t get_meter_addr( int alu, uint32_t meter_addr_in, uint64_t T ) {
      final_addrs_[alu] = meter_addr_in; // For DV, stash last meter_addr for ALU
      return meter_addr_in; // does nothing on Tofino
    }
    uint32_t get_immediate_data( int lt, uint32_t imm_data_in, uint32_t meter_addr ) {
      return imm_data_in;   // does nothing on Tofino
    }
    uint32_t get_final_addr(int alu) {
      // For DV, return last meter_addr for ALU
      return ((alu >= 0) && (alu < kNumAlus)) ?final_addrs_[alu] :0u;
    }
    bool any_counter_flagged()                    { return false; }
    bool alu_counter_flagged(int alu)             { return false; }
    bool is_counter_for_lt_being_cleared(int lt)  { return false; }
    void advance_time(int lt, uint64_t T, const char *why_str, uint32_t why_val) {  }



   private:
    void stateful_counter_clear_callback();
    // Sanity check vpn min/max/hole_pos
    void maybe_check_vpn_range(int lt);

    void stateful_counter_vpn_callback(uint32_t alu);
    void stateful_counter_vpn_en_callback();

    // Get type of MAU stateful log counter - note per LT
    CntrType get_type_stateful_cntr(int lt) {

      uint32_t reg = mau_stateful_log_counter_ctl_.mau_stateful_log_counter_ctl(lt / 8);
      uint32_t ctl = (reg >> ((lt % 8) * 3)) & 0x7;
      switch (ctl) {
        case 0: case 5: case 6: case 7: return CntrType::kDisabled;
        case 1: return CntrType::kTableMiss;
        case 2: return CntrType::kTableHit;
        case 3: return CntrType::kGatewayInhibit;
        case 4: return CntrType::kUnconditional;
        default: RMT_ASSERT(0); break;
      }
    }
    // Which ALU corresponds to LT
    inline int which_stateful_alu(int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      uint32_t regA = mau_stateful_log_ctl_ixbar_map_.mau_stateful_log_ctl_ixbar_map(lt / 8,0);
      uint32_t regB = mau_stateful_log_ctl_ixbar_map_.mau_stateful_log_ctl_ixbar_map(lt / 8,1);
      RMT_ASSERT(regA == regB);
      uint32_t ctl = (regA >> ((lt % 8) * 3)) & 0x7;
      if ((ctl & 0x4) == 0u) return -1;
      return static_cast<int>(ctl & 0x3);
    }
    // How far should we shift counter left before ORing into meter_addr
    inline int get_stateful_counter_shift(int alu) {
      RMT_ASSERT((alu >= 0) && (alu < kNumAlus));
      uint32_t reg = mau_stateful_log_instruction_width_.mau_stateful_log_instruction_width();
      // 0x0 => shift 3, 0x1 => shift 4, 0x2 => shift 5, 0x3 => shift 6
      return static_cast<int>(((reg >> (alu * 2)) & 0x3) + 3);
    }
    // What is VPN pos in unshifted counter - work backwards from final vpn pos (17) in addr
    inline int get_vpn_pos_in_counter(int alu) {
      return kStatefulAddrVpnPos - get_stateful_counter_shift(alu);
    }
    // What mask should we apply to unshifted counter - shift maximal mask right
    inline uint32_t get_counter_mask(int alu) {
      return kStatefulAddrMask >> get_stateful_counter_shift(alu);
    }
    // Get VPN hole pos - if enabled then always 4 (no CSR for this)
    inline int get_hole_pos(int alu) {
      bool en = mau_stateful_log_vpn_hole_en_.mau_stateful_log_vpn_hole_en();
      return (((en >> alu) & 1) == 1) ?4 :-1;
    }
    // Get raw value from _vpn_offset reg
    inline uint32_t get_vpn_offset(int alu) {
      static_assert( (kStatefulAddrVpnBits == 6), "Register assumes vpn width = 6");
      RMT_ASSERT((alu >= 0) && (alu < kNumAlus));
      uint32_t reg = mau_stateful_log_vpn_offset_.mau_stateful_log_vpn_offset(alu/2);
      return (reg >> ((alu % 2) * kStatefulAddrVpnBits))  & kStatefulAddrVpnMask;
    }
    // Get raw value from _vpn_limit reg
    inline uint32_t get_vpn_limit(int alu) {
      static_assert( (kStatefulAddrVpnBits == 6), "Register assumes vpn width = 6");
      RMT_ASSERT((alu >= 0) && (alu < kNumAlus));
      uint32_t reg = mau_stateful_log_vpn_limit_.mau_stateful_log_vpn_limit(alu/2);
      return (reg >> ((alu % 2) * kStatefulAddrVpnBits))  & kStatefulAddrVpnMask;
    }
    // Derive value VPN min from _vpn_offset (might need to insert zeros at hole_pos)
    inline uint32_t get_vpn_min(int alu) {
      uint32_t vpn_min = get_vpn_offset(alu);
      int hole_pos = get_hole_pos(alu);
      if (hole_pos < 0) return vpn_min;
      uint8_t vpn_min8 = static_cast<uint8_t>(vpn_min);
      return static_cast<uint32_t>(SweepInfo::holevpn_init_min(vpn_min8, hole_pos));
    }
    // Derive value VPN max from _vpn_limit (no zero insertion at hole_pos);
    inline uint32_t get_vpn_max(int alu) {
      return get_vpn_limit(alu);
    }
    // Return min allowed value stateful counter (determined by vpn min)
    inline uint32_t min_stateful_cntr(int alu) {
      RMT_ASSERT((alu >= 0) && (alu < kNumAlus));
      int pos = get_vpn_pos_in_counter(alu);
      // Return min VPN shifted up to correct pos with 0s in LSBs
      uint32_t min_v = (get_vpn_min(alu) << pos) | 0u;
      return min_v & get_counter_mask(alu);
    }
    // Return max allowed value stateful counter (determined by vpn max)
    inline uint32_t max_stateful_cntr(int alu) {
      RMT_ASSERT((alu >= 0) && (alu < kNumAlus));
      int pos = get_vpn_pos_in_counter(alu);
      // Return max VPN shifted up to correct pos with 1s in LSBs
      uint32_t max_v = (get_vpn_max(alu) << pos) | ((1u << pos) - 1);
      return max_v & get_counter_mask(alu);
    }
    // Reset MAU stateful counter (to min value)
    inline void reset_stateful_cntr(int alu) {
      RMT_ASSERT((alu >= 0) && (alu < kNumAlus));
      spinlock();
      uint32_t min_v = min_stateful_cntr(alu);
      mau_stateful_log_counter_array_.mau_stateful_log_counter(alu,0,min_v);
      spinunlock();
    }
    // Initialize stateful counter with min val if not yet done so
    inline bool maybe_initialize_stateful_cntr(int alu, bool dolock=false) {
      RMT_ASSERT((alu >= 0) && (alu < kNumAlus));
      // Doesn't do anything these days
      return false;
    }
    // Increment stateful counter moving it past any VPN 'holes'
    inline uint32_t incr_cntr_past_any_vpn_hole(int alu, uint32_t v, int inc) {
      RMT_ASSERT((alu >= 0) && (alu < kNumAlus));
      RMT_ASSERT((inc >= 0) && (inc < kSramEntries)); // Being cautious
      uint32_t cntr_mask = get_counter_mask(alu);
      // If no hole just inc and return
      int hole_pos = get_hole_pos(alu);
      if (hole_pos < 0) return (v+inc) & cntr_mask;

      int      vpn_pos = get_vpn_pos_in_counter(alu);
      uint32_t old_vpn = ((v)     >> vpn_pos) & kStatefulAddrVpnMask;
      uint32_t new_vpn = ((v+inc) >> vpn_pos) & kStatefulAddrVpnMask;
      // If inc does not cause vpn change then just inc and return
      if (new_vpn == old_vpn) return (v+inc) & cntr_mask;

      // VPN would change if we applied increment and in the 'hole'
      // case the next VPN to use is not necessarily old_vpn+1
      uint8_t old_vpn8 = static_cast<uint8_t>(old_vpn);
      uint8_t new_vpn8 = SweepInfo::holevpn_incr(old_vpn8, hole_pos);
      new_vpn = static_cast<uint32_t>(new_vpn8) & kStatefulAddrVpnMask;

      // Use lo bits of incremented val but OR in new_vpn
      uint32_t lo_mask = (1u << vpn_pos)-1;
      uint32_t v_inc_lo = (v+inc) & lo_mask;
      return ((new_vpn << vpn_pos) | v_inc_lo) & cntr_mask;
    }
    // Increment a stateful counter but not past max
    inline void incr_stateful_cntr(int lt, int inc=1) {
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables) && (inc >= 0));
      int alu = which_stateful_alu(lt);
      if (alu < 0) return;
      uint32_t max_v = max_stateful_cntr(alu);
      spinlock();
      //(void)maybe_initialize_stateful_cntr(alu, false);
      uint32_t v = mau_stateful_log_counter_array_.mau_stateful_log_counter(alu,0);
      if (v != max_v) {
        uint32_t v2 = incr_cntr_past_any_vpn_hole(alu, v, inc);
        mau_stateful_log_counter_array_.mau_stateful_log_counter(alu,0,v2);
      }
      spinunlock();
    }
    // Get a stateful counter as an addr to OR with meter_addr
    inline uint32_t get_stateful_cntr_for_alu(int alu) {
      RMT_ASSERT((alu >= 0) && (alu < kNumAlus));
      int pos = get_stateful_counter_shift(alu);
      //(void)maybe_initialize_stateful_cntr(alu, true);
      uint32_t v = mau_stateful_log_counter_array_.mau_stateful_log_counter(alu,0);
      return (v << pos) & kStatefulAddrMask;
    }
    // Get a stateful counter as an addr to OR with meter_addr
    inline uint32_t get_stateful_cntr_for_lt(int lt) {
      int alu = which_stateful_alu(lt);
      return (alu >= 0) ?get_stateful_cntr_for_alu(alu) :0u;
    }

    uint8_t                                               stateful_cntr_hole_en_;
    std::array< bool, kNumAlus>                           alu_vpn_change_;
    register_classes::MauStatefulLogCounterArray2Mutable  mau_stateful_log_counter_array_;
    register_classes::MauStatefulLogCounterCtlArray       mau_stateful_log_counter_ctl_;
    register_classes::MauStatefulLogCounterClearMutable   mau_stateful_log_counter_clear_;
    register_classes::MauStatefulLogVpnOffsetArray        mau_stateful_log_vpn_offset_;
    register_classes::MauStatefulLogVpnLimitArray         mau_stateful_log_vpn_limit_;
    register_classes::MauStatefulLogVpnHoleEn             mau_stateful_log_vpn_hole_en_;
    register_classes::MauStatefulLogInstructionWidth      mau_stateful_log_instruction_width_;
    register_classes::MauStatefulLogCtlIxbarMapArray2     mau_stateful_log_ctl_ixbar_map_;
    std::array<uint32_t, kNumAlus>                        final_addrs_;

  }; // MauStatefulCounters

}

#endif // _TOFINOXX_MAU_STATEFUL_COUNTERS_
