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

#ifndef _SHARED_MAU_ADDR_DIST_
#define _SHARED_MAU_ADDR_DIST_

#include <string>
#include <cstdint>
#include <unordered_map>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <address.h>
#include <mau-mapram.h>
#include <eop.h>
#include <evict-info.h>
#include <sweep-time-info.h>
#include <mau-chip-addr-dist.h>
#include <model_core/spinlock.h>

#include <register_includes/adr_dist_action_data_adr_icxbar_ctl_array.h>
#include <register_includes/adr_dist_stats_adr_icxbar_ctl_array.h>
#include <register_includes/adr_dist_meter_adr_icxbar_ctl_array.h>
#include <register_includes/adr_dist_idletime_adr_oxbar_ctl_array.h>
#include <register_includes/packet_action_at_headertime_array2.h>
// These 2 now in mau-chip-addr-dist.h
//#include <register_includes/deferred_ram_ctl_array2.h>
//#include <register_includes/deferred_oflo_ctl.h>
#include <register_includes/idletime_sweep_ctl_array.h>
#include <register_includes/meter_sweep_ctl_array.h>
#include <register_includes/stats_lrt_fsm_sweep_size_array.h>
#include <register_includes/idle_dump_ctl_array.h>
#include <register_includes/stats_dump_ctl_array.h>
#include <register_includes/oflo_adr_user_array.h>
#include <register_includes/mau_cfg_stats_alu_lt_array.h>
#include <register_includes/intr_status_mau_ad_mutable.h>


// appeared recently, but now gone in uarch1.8
//#include <tofino/register_includes/mau_cfg_lt_has_idle.h>
//#include <tofino/register_includes/mau_cfg_lt_has_stats.h>
//#include <tofino/register_includes/mau_cfg_lt_stats_are_direct.h>
//#include <tofino/register_includes/mau_cfg_lt_meter_are_direct.h>


// Handles action/stats/meter/idle address distribution for single MAU stage
//
// In the case of stats/meters the AddrClaim mechanism allows addresses to be
// allocated to a single SRAM bank of the stats/meter table when first written
//
// SweepInfo tracks the sweep interval for each LogicalTable and stores the
// Idletime Maprams associated with the LogicalTable

namespace MODEL_CHIP_NAMESPACE {

  class Mau;
  class MauExecuteState;
  class MauStatefulCounters;
  class MauTeop;


  class SweepInfo {
    static constexpr uint8_t kMaxInterval = MauDefs::kMaxInterval;
    static constexpr uint8_t kVpnBits = MauDefs::kMapramVpnBits;
    static constexpr uint8_t kVpnMask = 0xFF >> (8-kVpnBits);
    static_assert( (kVpnBits <= 8),
                   "VPN hole handling assumes VPN fits in uint8_t");

    static inline bool mapram_sort_func(MauMapram *m1, MauMapram *m2) {
      return (m1->get_priority() < m2->get_priority());
    }


    static constexpr int     kHoleSize = 2;
    static constexpr int     kHoleMaxPos = static_cast<int>(kVpnBits) - kHoleSize;
    static constexpr uint8_t kHoleMask = 0xFF >> (8-kHoleSize);
    static constexpr uint8_t kHoleAvoidVal = 0x3u;
    static_assert( (kHoleSize < kVpnBits),
                   "Hole size must be less than VPN size");
    static_assert( ((kHoleAvoidVal & kHoleMask) == kHoleAvoidVal),
                   "Hole avoid val must be within hole");
    // All this nonsense just to increment a VPN with a hole


    static inline uint8_t holevpn_hole_to_lsb(uint8_t val, int hole_pos) {
      if (hole_pos <= 0) return val;
      uint8_t lo_mask = 0xFF >> (8-hole_pos);
      uint8_t hi_mask = ~((kHoleMask << hole_pos) | lo_mask);
      uint8_t hole_val = (val >> hole_pos) & kHoleMask;
      // Move lo part of val up and hole down into LSBs
      return (val & hi_mask) | ((val & lo_mask) << kHoleSize) | hole_val;
    }
    static inline uint8_t holevpn_lsb_to_hole(uint8_t val, int hole_pos) {
      if (hole_pos <= 0) return val;
      uint8_t lo_mask = (0xFF >> (8-hole_pos)) | (kHoleMask << hole_pos);
      uint8_t hi_mask = ~lo_mask;
      uint8_t lsb_val = val & kHoleMask;
      // Move hole up from LSBs to hole pos pushing val down
      return (val & hi_mask) | ((val & lo_mask) >> kHoleSize) | (lsb_val << hole_pos);
    }

 public:
    static inline uint8_t holevpn_init_min(uint8_t val, int hole_pos) {
      RMT_ASSERT((val & kVpnMask) == val);
      RMT_ASSERT(hole_pos <= kHoleMaxPos);
      if (hole_pos < 0) return val;
      uint8_t lo_mask = 0xFF >> (8-hole_pos);
      uint8_t hi_mask = ~lo_mask;
      return (((val & hi_mask) << kHoleSize) | (val & lo_mask)) & kVpnMask;
    }
    static inline bool holevpn_check(uint8_t min, uint8_t max, int hole_pos) {
      if (((min & kVpnMask) != min) || ((max & kVpnMask) != max)) return false;
      if (hole_pos >= 0) {
        if (hole_pos > kHoleMaxPos) return false;
        if (((max >> hole_pos) & kHoleMask) == kHoleAvoidVal) return false;
        min = holevpn_init_min(min, hole_pos); // Insert zeros at hole pos
        max &= ~(kHoleMask << hole_pos);       // Zeroise hole pos
        // Zeroising hole bits in min/max allows the min > max check
        // below to detect cases where the vpn would need to wrap in 6b
        // before becoming equal to max.
      }
      if (min > max) return false;
      return true;
    }
    static inline uint8_t holevpn_incr(uint8_t val, int hole_pos) {
      RMT_ASSERT((val & kVpnMask) == val);
      RMT_ASSERT(hole_pos < static_cast<int>(kVpnBits));
      if (hole_pos < 0) return (val+1) & kVpnMask; // Just normal increment
      uint8_t tmpval = holevpn_hole_to_lsb(val, hole_pos);
      tmpval++;
      if ((tmpval & kHoleMask) == kHoleAvoidVal) tmpval++;
      return holevpn_lsb_to_hole(tmpval & kVpnMask, hole_pos);
    }

    static constexpr uint8_t kVpnMax = MauDefs::kMapramVpnMax;
    static_assert( (kVpnMax <= 64), "VPN bitmask must fit in uint64_t");



    SweepInfo()  { reset(0); }
    ~SweepInfo() { reset(0); }

    void reset(int type=0) {
      spinlock_.clear();
      vpn_valid_mask_ = UINT64_C(0);
      for (uint8_t i = 0; i < kVpnMax; i++) {
        seq_to_vpn_[i] = 0xFF; vpn_to_seq_[i] = 0xFF;
      }
      min_vpn_ = 0xFF; max_vpn_ = 0xFF; hole_pos_ = -1;
      sweep_interval_ = kMaxInterval;
      logical_table_ = 0xFF; alu_ = 0xFF; type_ = type;
      enabled_ = false; idle_ = false; hit_ = false;
      clear();
    }
    void clear()                            { maprams_.clear(); }
    void add_mapram(MauMapram *mapram)      { maprams_.push_back(mapram); }
    void sort_maprams() {
      bool order_maprams = (hole_pos_ >= 0);
      if (order_maprams) maprams_set_ordering();
      std::sort(maprams_.begin(), maprams_.end(), mapram_sort_func);
      if (order_maprams) maprams_clear_ordering();
    }
    uint8_t sweep_interval()                { return sweep_interval_; }
    uint8_t logical_table()                 { return logical_table_; }
    uint8_t alu()                           { return alu_; }
    uint8_t min_vpn()                       { return min_vpn_; }
    uint8_t max_vpn()                       { return max_vpn_; }
    int8_t  hole_pos()                      { return hole_pos_; }
    uint64_t vpn_valid_mask()               { return vpn_valid_mask_; }
    bool     vpn_valid(uint8_t i) {
      return (i < 64) ?(((vpn_valid_mask_ >> i) & UINT64_C(1)) != UINT64_C(0)) :false;
    }
    uint8_t seq_to_vpn(uint8_t i)           { return (i < kVpnMax) ?seq_to_vpn_[i] :0xFF; }
    uint8_t vpn_to_seq(uint8_t i)           { return (i < kVpnMax) ?vpn_to_seq_[i] :0xFF; }
    bool enabled()                          { return enabled_; }
    void set_sweep_interval(uint8_t intvl)  { sweep_interval_ = intvl; }
    void set_logical_table(uint8_t lt)      { logical_table_ = lt; }
    void set_alu(uint8_t alu)               { alu_ = alu; }
    void set_enabled(bool tf)               { enabled_ = tf; }

    bool set_vpn_range(MauAddrDist *mad,
                       uint8_t min_vpn, uint8_t max_vpn, int8_t hole_pos);
    void maprams_clear_ordering();
    void maprams_set_ordering();

    void idle_dump(bool clear);
    void idle_dump_word(uint32_t addr, bool clear);
    void idle_sweep(MauAddrDist *mad, int lt, uint64_t t_now_psecs);
    void idle_hit(MauAddrDist *mad, int lt);
    void idle_sweep_start(MauAddrDist *mad, int lt);
    void idle_sweep_end(MauAddrDist *mad, int lt, bool now_idle);


 private:
    model_core::Spinlock        spinlock_;
    std::vector<MauMapram*>     maprams_;
    std::array<uint8_t,kVpnMax> seq_to_vpn_;
    std::array<uint8_t,kVpnMax> vpn_to_seq_;
    uint64_t                    vpn_valid_mask_;
    uint8_t                     min_vpn_;
    uint8_t                     max_vpn_;
    int8_t                      hole_pos_;
    uint8_t                     sweep_interval_;
    uint8_t                     logical_table_;
    uint8_t                     alu_;
    uint8_t                     type_;
    bool                        enabled_;
    bool                        idle_;
    bool                        hit_;
  };





  class AddrClaim {
    // Track highest priority claimant for all distributed addresses
    static constexpr int kLogicalRows = MauDefs::kLogicalRowsPerMau;
    static constexpr int kLogicalColumns = MauDefs::kLogicalColumnsPerMau;

 public:
    AddrClaim()      { reset(); }
    ~AddrClaim()     { }
    void reset()     {
      logrow_ = -1; logcol_ = -1; prio_ = -1; claimants_ = 0;
      for (auto &bm : claimants_bitmap_) bm=0;
    }
    int  claimants() { return claimants_; }

    void complain(int lr, int lc, int prio, Mau *maulog);
    bool claim(int lr, int lc, int prio, Mau *maulog);

    uint32_t get_claimant_columns(int lr) const { return claimants_bitmap_[lr]; }

 private:
    int8_t  logrow_;
    int8_t  logcol_;
    int8_t  prio_;
    uint8_t claimants_;
    static_assert( kLogicalColumns < 32, "logical columns must fit in 32 bits" );
    std::array<uint32_t, kLogicalRows>  claimants_bitmap_;
  };




  class DeferredRamRewriteInfo {
    // Store S->D rewrite info per Stats/Meter LT
 public:
    DeferredRamRewriteInfo()  { src_ = -1; dst_ = -1; mask_ = 0u; }
    ~DeferredRamRewriteInfo() { }

    void setup_rewrite_addrs(int src, int dst, uint32_t mask) {
      src_ = (src < 0) ?src :static_cast<int>(static_cast<uint32_t>(src) & mask);
      dst_ = (dst < 0) ?dst :static_cast<int>(static_cast<uint32_t>(dst) & mask);
      mask_ = mask;
    }
    uint32_t rewrite_addr(uint32_t a) {
      if ((src_ < 0) || (dst_ < 0) || ((a & mask_) != static_cast<uint32_t>(src_))) return a;
      // a matches src_ so rewrite a to be dst_ (but under mask_ so color etc unaffected)
      a &= ~mask_;
      a |= dst_;
      return a;
    }
 private:
    int      src_;
    int      dst_;
    uint32_t mask_;
  };




  struct ColorWriteEntry {
    // Maintain a fifo of these ColorWriteEntries per Meter ALU
    uint64_t relative_time_;
    uint64_t wr_latency_;
    uint32_t addr_;
    uint8_t  color_;
    uint8_t  wr_mapram_row_;
    uint8_t  wr_mapram_col_;
    bool     valid_;
  };

  // Flags for call to calculate_X_rows
  struct When {
    static constexpr uint8_t  kNever    = 0x00;
    static constexpr uint8_t  kHdrTime  = 0x01;
    static constexpr uint8_t  kEopTime  = 0x02;
    static constexpr uint8_t  kTeopTime = 0x04;
  };


  class RmtSweeper;

  class MauAddrDist {
    static constexpr bool     kDisableOflow2 = true;
    static constexpr uint32_t kStatsAluLogicalRows = MauDefs::kStatsAluLogicalRows;
    static constexpr uint32_t kMeterAluLogicalRows = MauDefs::kMeterAluLogicalRows;
    static constexpr uint32_t kAdistMaskHomeRows = 0x0FFFF;
    static constexpr uint32_t kAdistMaskOflow    = 0x10000;
    static constexpr uint32_t kAdistMaskOflow2Up = 0x20000;
    static constexpr uint32_t kAdistMaskOflow2Dn = 0x40000;
    // XXX: what about lpf meters? They run at header time and can overflow.
    // Oflow now only valid in deferred_ctl (since regs_20465_mau_dev)
    // calculate_X_rows now sets oflow for both EOP/SOP (based on deferred_ctl)
    static constexpr uint32_t kAdistMaskValidStatsRows = kStatsAluLogicalRows;
    // XXX: added kAdistMaskOflow back in, but not sure what it does! test_dv65 works even without this added back
    // this is meant to be the set of rows that can be specified in the icxbars so should NOT contain oflow these days
    static constexpr uint32_t kAdistMaskValidMeterRows = kMeterAluLogicalRows;
    static constexpr uint32_t kAdistMaskValidStatsOflo = (1u<<13)|(1u<<9);
    // XXX: why isn't it this?
    // static constexpr uint32_t kAdistMaskValidMeterOflo = (1u<<15)|(1u<<11)|(1u<<7)|(1u<<3);
    // only *upper* half rows that have stats/meter can use oflow
    static constexpr uint32_t kAdistMaskValidMeterOflo = (1u<<15)|(1u<<11);
    static constexpr int      kNumStatsAlus = MauDefs::kNumStatsAlus;
    static constexpr int      kNumMeterAlus = MauDefs::kNumMeterAlus;
    static constexpr int      kLogicalTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int      kLogicalRows = MauDefs::kLogicalRowsPerMau;
    static constexpr int      kLogicalRowsPerPhysicalRow = MauDefs::kLogicalRowsPerPhysicalRow;
    static constexpr int      kSramRowsPerMau = MauDefs::kSramRowsPerMau;
    static constexpr int      kLogicalColumns = MauDefs::kLogicalColumnsPerMau;
    static constexpr int      kMapramColumns = MauDefs::kMapramColumnsPerMau;
    static constexpr int      kMapramRows = MauDefs::kMapramRowsPerMau;
    static constexpr int      kSramAddressWidth  = MauDefs::kSramAddressWidth;
    static constexpr int      kSramEntries = 1<<kSramAddressWidth;
    static constexpr int      kMapramAddressWidth  = MauDefs::kMapramAddressWidth;
    static constexpr int      kMapramEntries = 1<<kMapramAddressWidth;
    static constexpr int      kDataBusWidth = MauDefs::kDataBusWidth;
    static constexpr int      kIdletimeBuses = MauDefs::kIdletimeBusesPerMau;
    static constexpr int      kIdletimeWords = 5; // Size of idletime oxbar register
    static constexpr int      kIdletimeEntriesPerWord = 4;
    static constexpr int      kIdletimeEntries = kIdletimeWords*kIdletimeEntriesPerWord;
    static constexpr uint32_t kIdletimeInvalidEntryMask = 0; // all busses exist now!
    static constexpr int      kIdletimeEntryBits = 5;
    static constexpr int      kIdletimeEntryEnabledBit = 4;
    static constexpr uint32_t kIdletimeEntryMask = (1u<<kIdletimeEntryBits)-1;
    static constexpr uint32_t kIdletimeEntryEnabledMask = (1u<<kIdletimeEntryEnabledBit);
    static constexpr uint32_t kIdletimeEntryValueMask = kIdletimeEntryMask & ~kIdletimeEntryEnabledMask;
    static_assert( ((kStatsAluLogicalRows & ~kAdistMaskHomeRows) == 0u),
                   "StatsALU rows must be in range 0-15");
    static_assert( ((kStatsAluLogicalRows & kAdistMaskValidStatsOflo) ==
                    kAdistMaskValidStatsOflo),
                   "StatsALU Oflo rows must be a subset of StatsALU logical rows");
    static_assert( ((kMeterAluLogicalRows & ~kAdistMaskHomeRows) == 0u),
                   "MeterALU rows must be in range 0-15");
    static_assert( ((kMeterAluLogicalRows & kAdistMaskValidMeterOflo) ==
                    kAdistMaskValidMeterOflo),
                   "MeterALU Oflo rows must be a subset of MeterALU logical rows");
    static_assert( (kIdletimeBuses == kIdletimeEntries),
                   "Idletime entry/buscount mismatch");
    static_assert( (kIdletimeEntryEnabledBit+1 == kIdletimeEntryBits),
                   "Idletime enabled must be top bit");

    static inline bool mapram_sort_func(MauMapram *m1, MauMapram *m2) {
      return (m1->get_priority() < m2->get_priority());
    }

    static bool is_idletime_valid(int entry_num) {
      if (!((entry_num >= 0) && (entry_num < kIdletimeEntries))) return false;
      if ((kIdletimeInvalidEntryMask & (1u<<entry_num)) != 0) return false;
      return true;
    }
    static int get_idletime_which_word(int entry_num) {
      RMT_ASSERT((entry_num >= 0) && (entry_num < kIdletimeEntries));
      return entry_num / kIdletimeEntriesPerWord;
    }
    static int get_idletime_which_entry(int entry_num) {
      RMT_ASSERT((entry_num >= 0) && (entry_num < kIdletimeEntries));
      return entry_num % kIdletimeEntriesPerWord;
    }
    static int get_idletime_entry(uint32_t idle_word, int entry_in_word) {
      RMT_ASSERT((entry_in_word >= 0) && (entry_in_word < kIdletimeEntriesPerWord));
      uint32_t entry = (idle_word >> (kIdletimeEntryBits * entry_in_word)) & kIdletimeEntryMask;
      if ((entry & kIdletimeEntryEnabledMask) == 0u) return -1;
      return static_cast<int>(entry & kIdletimeEntryValueMask);
    }

    static uint16_t make_map_key(uint8_t addrtype, int lt, uint8_t eop_num) {
      RMT_ASSERT((addrtype == AddrType::kStats) || (addrtype == AddrType::kMeter));
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      RMT_ASSERT(Eop::valid_eopnum(eop_num));
      uint32_t key = 0u;
      key |= static_cast<uint32_t>(addrtype & 0xF) << 12;
      key |= static_cast<uint32_t>(lt & 0xF) << 8;
      key |= static_cast<uint32_t>(eop_num) << 0;
      return key;
    }
    static int map_key_get_addrtype(uint16_t map_key) {
      return static_cast<int>((map_key >> 12) & 0xF);
    }
    static int map_key_get_logtab(uint16_t map_key) {
      return static_cast<int>((map_key >> 8) & 0xF);
    }


 public:
    static bool kRelaxPrePfeAddrCheck; // Defined in rmt-config.cpp
    static bool kMeterSweepOnDemand;
    static bool kMeterSweepOnDemandPipe0;
    static bool kRelaxPacketActionAtHdrCheck;
    static bool kRelaxAllAddrsConsumedCheck;
    static bool kRelaxActionAddrsConsumedCheck;
    static bool kRelaxStatsAddrsConsumedCheck;
    static bool kRelaxMeterAddrsConsumedCheck;
    static bool kRelaxIdletimeAddrsConsumedCheck;


    MauAddrDist(int chipIndex, int pipeIndex, int mauIndex, Mau *mau);
    ~MauAddrDist();

    Mau *mau() { return mau_; }

    uint32_t oflow_addr_nocheck()         const { return oflow_addr_; }
    uint32_t oflow2_up_addr_nocheck()     const { return oflow2_up_addr_; }
    uint32_t oflow2_down_addr_nocheck()   const { return oflow2_down_addr_; }
    uint32_t action_addr_nocheck(int row) const { return action_addrs_[row]; }
    uint32_t stats_addr_nocheck(int row)  const { return stats_addrs_[row]; }
    uint32_t meter_addr_nocheck(int row)  const { return meter_addrs_[row]; }
    uint32_t idle_addr_nocheck(int ibus)  const { return idletime_addrs_[ibus]; }


    // Retrieve addrs
    bool check(bool ingress, uint8_t lt, uint8_t addrtype,
               const char *bus, int row=-1) const {
      bool ok = true;
      if (lt >= kLogicalTables) ok = false;

      // Complaints, ingress/egress checks skipped for AddrType kNone
      if (addrtype == AddrType::kNone) return ok;

      if (lt >= kLogicalTables) {
        // If bus not driven LT will still be 0xFF - complain
        bus_not_driven_complain(bus, row);
      } else if ((ingress && ((ingress_logtabs_ & (1u<<lt)) == 0u)) ||
               (!ingress && ((egress_logtabs_ & (1u<<lt)) == 0u))) {
        // If SRAM not same ingress/egress type as LogicalTable using bus complain
        ingress_egress_mismatch_complain(bus, row);
        ok = false;
      }
      return ok;
    }
    uint32_t oflow_addr(bool ingress, uint8_t addrtype) const {
      const char *bus = "oflow_addr";
      if (!check(ingress, oflow_src_logtab_, addrtype, bus))
        return Address::invalid();
      addrtype_mismatch_check(bus, oflow_addrtype_, addrtype);
      return oflow_addr_nocheck();
    }
    uint32_t oflow2_up_addr(bool ingress, uint8_t addrtype) const {
      const char *bus = "oflow2_up_addr";
      if (!check(ingress, oflow2_up_src_logtab_, addrtype, bus))
        return Address::invalid();
      addrtype_mismatch_check(bus, oflow2_up_addrtype_, addrtype);
      return oflow2_up_addr_nocheck();
    }
    uint32_t oflow2_down_addr(bool ingress, uint8_t addrtype) const {
      const char *bus = "oflow2_down_addr";
      if (!check(ingress, oflow2_down_src_logtab_, addrtype, bus))
        return Address::invalid();
      addrtype_mismatch_check(bus, oflow2_down_addrtype_, addrtype);
      return oflow2_down_addr_nocheck();
    }
    uint32_t action_addr(int which_row, bool ingress, uint8_t addrtype=AddrType::kAction) const {
      const char *bus = "action_addr";
      if (!check(ingress, action_src_logtabs_[which_row], addrtype, bus, which_row))
        return Address::invalid();
      addrtype_mismatch_check("action_addr", AddrType::kAction, addrtype);
      return action_addr_nocheck(which_row);
    }
    uint32_t stats_addr(int which_row, bool ingress, uint8_t addrtype=AddrType::kStats) const {
      const char *bus = "stats_addr";
      if (!check(ingress, stats_src_logtabs_[which_row], addrtype, bus, which_row))
        return Address::invalid();
      addrtype_mismatch_check("stats_addr", AddrType::kStats, addrtype);
      return stats_addr_nocheck(which_row);
    }
    uint32_t meter_addr(int which_row, bool ingress, uint8_t addrtype=AddrType::kMeter) const {
      const char *bus = "meter_addr";
      if (!check(ingress, meter_src_logtabs_[which_row], addrtype, bus, which_row))
        return Address::invalid();
      addrtype_mismatch_check("meter_addr", AddrType::kMeter, addrtype);
      return meter_addr_nocheck(which_row);
    }
    uint32_t idle_addr(int ibus, bool ingress) const {
      const char *bus = "idle_addr";
      // We can be calling idle_addr from every mapram so disable complaints (use kNone)
      if (!check(ingress, idle_src_logtabs_[ibus], AddrType::kNone, bus, ibus))
        return Address::invalid();
      return idle_addr_nocheck(ibus);
    }



    // Claim logical table action/stats/meter addr from specified logical row/col
    bool stats_alu_claim(uint8_t alu, int lr, int lc, int prio) {
      RMT_ASSERT(alu<=kNumStatsAlus);
      return stats_addrclaims_[alu].claim(lr, lc, prio, mau_);
    }
    bool meter_alu_claim(uint8_t alu, int lr, int lc, int prio) {
      RMT_ASSERT(alu<=kNumMeterAlus);
      return meter_addrclaims_[alu].claim(lr, lc, prio, mau_);
    }


    // Figure out stats_alu/meter_alu for LT for sanity checking
    // DOES *NOT* check only one ALU for any given LT
    int get_stats_alu(int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      int max_alu_index = -1;
      uint32_t alus = adr_dist_stats_icxbar_.adr_dist_stats_adr_icxbar_ctl(lt);
      (void)MauStatsAlu::map_stats_alus_to_rows(alus, &max_alu_index);
      return max_alu_index;
    }
    int get_meter_alu(int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      int max_alu_index = -1;
      uint32_t alus = adr_dist_meter_icxbar_.adr_dist_meter_adr_icxbar_ctl(lt);
      (void)MauMeterAlu::map_meter_alus_to_rows(alus, &max_alu_index);
      return max_alu_index;
    }
    uint32_t get_meter_color_alu_rows(int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      return mau_chip_addr_dist_.get_meter_color_alu_rows(lt);
    }

    // Find logical table for idle_bus
    int get_idletime_logical_table(int idle_bus) {
      return get_idletime_entry(get_idle_oxbar_word(get_idletime_which_word(idle_bus)),
                                get_idletime_which_entry(idle_bus));
    }

    // For sanity checking
    // When addresses are distributed we set a bit for all rows/buses the
    // address is distributed to. When we consume an address in an sram/mapram
    // we clear the bit of any row/bus that had the address distributed to it.
    // At end of processing ALL addrs should have been consumed.
    void action_addr_consume(uint32_t addr) {
      for (int i = 0; i < kLogicalRows; i++) {
	if (action_addrs_[i] == addr) action_addr_not_consumed_ &= ~(1u<<i);
      }
    }
    void stats_addr_consume(uint32_t addr) {
      for (int i = 0; i < kLogicalRows; i++) {
	if (stats_addrs_[i] == addr) stats_addr_not_consumed_ &= ~(1u<<i);
      }
    }
    void meter_addr_consume(uint32_t addr) {
      for (int i = 0; i < kLogicalRows; i++) {
	if (meter_addrs_[i] == addr) meter_addr_not_consumed_ &= ~(1u<<i);
      }
    }
    void idletime_addr_consume(uint32_t addr) {
      for (int i = 0; i < kIdletimeBuses; i++) {
	if (idletime_addrs_[i] == addr) idletime_addr_not_consumed_ &= ~(1u<<i);
      }
    }
    bool all_addrs_consumed() {
      return ((action_addr_not_consumed_ == 0u) && (stats_addr_not_consumed_ == 0u) &&
	      (meter_addr_not_consumed_ == 0u) && (idletime_addr_not_consumed_ == 0u));
    }
    bool addr_consumed(uint32_t addr);
    void addrs_leftover_check();

    void stats_synth2port_fabric_check(int alu,int lr);
    void meter_synth2port_fabric_check(int alu,int lr);

    void synth2port_fabric_check(int alu,const AddrClaim& claim,int lr,bool is_stats);

    // Idletime Sweep/DumpWord/Dump/Hit in single table
    void idletime_notify(int lt, uint32_t addr, uint64_t data);
    void idletime_sweep(int lt, uint64_t t_now);
    void idletime_dump_word(int lt, uint32_t addr, bool clear);
    void idletime_dump(int lt, bool clear);
    void idletime_hit(int lt);

    // DV func
    // Handle idletime sweep for *single address*
    // NOTE, also as a convenience takes cycle count not picosecs.
    // Negative return means no sweep performed and indicates error:
    // >=0 => sweep performed
    //  -1 => either sweep not enabled OR address vpn > max_vpn
    //  -2 => in hole
    //  -3 => address vpn < min_vpn
    //  -4 => config error (min_vpn > max_vpn)
    //  -5 => param error (inval LT/addr)
    int idletime_sweep_one_address(int lt, uint32_t addr, uint64_t t_now_cycles);

    // DV func
    // Handle idletime sweep for *single index*
    // Index MSBs can be used to indicate VPN (which can be relative
    // to table min_vpn if relative_vpn=true)
    // NOTE, subword MUST include Huffman bits
    // NOTE, also as a convenience takes cycle count not picosecs.
    // Return vals as in idletime_sweep_one_address()
    int idletime_sweep_one_index(int lt, int index, int subword,
                                 uint64_t t_now_cycles, bool relative_vpn=true);


    // Stats DumpWord/Dump
    int  stats_format(int lt);
    void stats_notify(int lt, uint32_t addr, uint64_t data, bool dump);
    void stats_dump_word(int lt, uint32_t addr, bool dump);
    void stats_dump(int lt);

    // Support for LRT-lite evictions
    void handle_evictions(const EvictInfo &evictinfo);
    void update_queued_addr(uint8_t addrtype, int lt, uint32_t old_addr, uint32_t new_addr);



    // Meter sweep for *LogicalTable* - called by RmtSweeper
    void meter_sweep(int lt, uint64_t t_now_psecs);

    // DV func
    // Handle meter sweep for *single address* within *ALU*
    // NOTE, also as a convenience takes cycle count not picosecs.
    // Negative return means no sweep performed and indicates error:
    // >=0 => sweep performed
    //  -1 => either sweep not enabled OR address vpn > max_vpn
    //  -2 => in hole
    //  -3 => address vpn < min_vpn
    //  -4 => config error (bad LT or min_vpn > max_vpn)
    //  -5 => param error (inval ALU/addr)
    int meter_sweep_one_address(int alu, uint32_t addr,
                                uint64_t t_metertick_cycles,
                                uint64_t t_relative_cycles);

    // DV func
    // Handle meter sweep for *single index/subword* within *ALU*
    // Index MSBs can be used to indicate VPN (which can be relative
    // to table min_vpn if relative_vpn=true)
    // NOTE, also as a convenience takes cycle count not picosecs.
    // Return vals as in meter_sweep_one_address()
    // First variant allows subword to be specified too - for SALU sweeps
    int meter_sweep_one_index(int alu, int index, int subword,
                              uint64_t t_metertick_cycles,
                              bool     relative_vpn=true,
                              uint64_t t_relative_cycles=UINT64_C(0xFFFFFFFFFFFFFFFF));
    // Same signature as meter_sweep_one_index above but NO optional arguments
    int meter_sweep_one_index_with_reltime(int alu, int index, int subword,
                                           uint64_t t_metertick_cycles,
                                           bool     relative_vpn,
                                           uint64_t t_relative_cycles);
    // This variant always uses subword 0
    int meter_sweep_one_index(int alu, int index,
                              uint64_t t_metertick_cycles,
                              bool     relative_vpn=true,
                              uint64_t t_relative_cycles=UINT64_C(0XFFFFFFFFFFFFFFFF));



    void mapram_change_callback();

    // Upcall RmtSweeper to notify change in sweep interval etc
    void meter_sweeper_upcall(int index);
    // Upcall RmtSweeper to notify idle/active etc
    void idletime_sweeper_upcall(int lt, bool idle);

    // Reset buses
    void reset_resources();

    // Check VPNs
    bool vpn_range_check(int logical_row, uint32_t addr, uint8_t addr_type, bool quiet);

    // These 2 always called at Headertime
    void distrib_action_addresses(int logicalTable, bool ingress, uint32_t action_addr);
    void distrib_idletime_addresses(int logicalTable, bool ingress, uint32_t idletime_addr);
    void distrib_stats_addresses(int logical_table, bool ingress,
                                 uint32_t stats_addr, uint32_t stats_rows,
                                 bool overwrite_addr=false);
    void distrib_meter_addresses(int logical_table, bool ingress,
                                 uint32_t meter_addr, uint32_t meter_rows,
                                 bool overwrite_addr=false);

    // These called at HeaderTime
    void teop_output_stats_addr(Teop *teop, int lt, bool ingress,
                                uint32_t stats_rows, uint32_t stats_addr);
    void teop_output_meter_addr(Teop *teop, int lt, bool ingress,
                                uint32_t meter_rows, uint32_t meter_addr);
    bool distrib_stats_addresses(int logical_table, bool ingress,
                                 uint32_t stats_addr, Teop *teop);
    bool distrib_meter_addresses(int logical_table, bool ingress,
                                 uint32_t meter_addr, Teop *teop, uint8_t color);
    void defer_stats_addresses(int logical_table, bool ingress,
                               uint32_t stats_addr, Teop *teop, uint8_t eop_num);
    void defer_meter_addresses(int logical_table, bool ingress,
                               uint32_t meter_addr, Teop *teop, uint8_t eop_num);
    // These called at HeaderTime at end of ALL logical-table HeaderTime processing
    void finalize_stats_addresses(MauExecuteState *state);
    void finalize_meter_addresses(MauExecuteState *state);

    // This one called at EopTime
    void distrib_eoptime_addresses(MauExecuteState *state);
    // This one called at TeopTime
    void distrib_teoptime_addresses(MauExecuteState *state);

    // These 2 distribute to both headertime and eoptime rows - for PBUS/Dump access
    void distrib_stats_addresses(int logical_table, bool ingress, uint32_t stats_addr, bool allow_eop);
    void distrib_meter_addresses(int logical_table, bool ingress, uint32_t meter_addr, bool allow_eop);

    // To allow MauLogicalRowReg to decode what ALU is consuming oflow addr
    uint8_t get_oflo_ctl() {
      return mau_chip_addr_dist_.get_deferred_oflo_ctl();
    }

    // For PBUS read/write
    bool row_read_eop_stats_addr(int logrow, uint8_t eop_num, uint32_t *addr) {
      return row_read_eop_addr(AddrType::kStats, logrow, eop_num, addr);
    }
    bool row_read_eop_meter_addr(int logrow, uint8_t eop_num, uint32_t *addr) {
      return row_read_eop_addr(AddrType::kMeter, logrow, eop_num, addr);
    }
    void row_write_eop_stats_addr(int logrow, uint8_t eop_num, uint32_t addr) {
      row_write_eop_addr(AddrType::kStats, logrow, eop_num, addr);
    }
    void row_write_eop_meter_addr(int logrow, uint8_t eop_num, uint32_t addr) {
      row_write_eop_addr(AddrType::kMeter, logrow, eop_num, addr);
    }
    // For moveregs
    void update_eop_addr(uint8_t addrtype, int lt, int old_addr, int new_addr);

    // Queue/Dequeue/Update Color Writes per-MeterALU
    void queue_color_write(int alu, uint64_t relative_time, uint64_t wr_latency,
                           MauMapram *mapram, uint32_t addr, uint8_t color);
    void dequeue_color_writes(int alu, uint64_t relative_time, MauMapram *mapram);
    void update_queued_color_writes(int alu, int old_addr, int new_addr);
    void flush_queued_color_writes(int alu);

    void pre_pfe_checks_action(int logical_table, bool ingress, uint32_t addr);
    void pre_pfe_checks_stats(int logical_table, bool ingress, uint32_t addr);
    void pre_pfe_checks_meter(int logical_table, bool ingress, uint32_t addr);
    void pre_pfe_checks_idle(int logical_table, bool ingress, uint32_t addr);

    SweepTimeInfo *get_sweep_time_info(int alu) { return &meter_sweep_time_info_[alu]; }
    void meter_sweep_change_callback(int alu);


 private:
    void reset();
    void intr_status_mau_ad_read_callback();

    int  meter_lt_find_alu(int lt);
    void meter_icxbar_change_callback(int lt);

    void idletime_sweep_change_callback(int lt);
    void idletime_dump_change_callback(int lt);
    void idletime_oxbar_change_callback();
    void stats_dump_change_callback(int lt);

    void update_mapram_mappings();

    uint32_t calculate_stats_rows(int logical_table, bool ingress,
                                  uint8_t eop_num, uint8_t when);
    uint32_t calculate_meter_rows(int logical_table, bool ingress,
                                  uint8_t eop_num, uint8_t when);
    void logical_table_track_ingress_egress(int logical_table, bool ingress);

    uint32_t vpn_range_check_rows(uint32_t rows,  uint32_t addr, uint8_t addr_type, bool quiet);
    uint32_t vpn_range_check_rows2(uint32_t rows,  uint32_t addr, uint8_t addr_type);
    void pre_pfe_checks(std::array<uint32_t,kLogicalRows> *en_addr_array,
                        std::array<uint32_t,kLogicalRows> *not_en_addr_array,
                        std::array<uint8_t,kLogicalRows> *srctab_array,
                        uint32_t adistMask, int lt, bool addr_en,
                        uint32_t addr, uint8_t addr_type, const std::string& bus_name);
    void distribute_to_rows(std::array<uint32_t,kLogicalRows> *addr_array,
                            std::array<uint8_t,kLogicalRows> *srctab_array,
                            std::array<uint32_t,kLogicalRows> *srctabmask_array,
                            uint32_t adistMask, int lt,
                            uint32_t addr, uint8_t addr_type,
                            const std::string& bus_name, bool overwrite_addr=false);

    // Shim through to PER_CHIP funcs
    bool    get_deferred_ram_en(int s_or_m, int idx)       { return mau_chip_addr_dist_.get_deferred_ram_en(s_or_m, idx); }
    bool    get_deferred_ram_thread(int s_or_m, int idx)   { return mau_chip_addr_dist_.get_deferred_ram_thread(s_or_m, idx); }
    uint8_t get_deferred_ram_err_ctl(int s_or_m, int idx)  { return mau_chip_addr_dist_.get_deferred_ram_err_ctl(s_or_m, idx); }
    uint8_t get_deferred_oflo_ctl()                        { return mau_chip_addr_dist_.get_deferred_oflo_ctl(); }
    bool    vpn_range_check_meter(int lrow, uint32_t addr) { return mau_chip_addr_dist_.vpn_range_check_meter(lrow, addr); }
    bool    vpn_range_check_stats(int lrow, uint32_t addr) { return mau_chip_addr_dist_.vpn_range_check_stats(lrow, addr); }
    int     get_meter_sweep_subword_shift(int alu)         { return mau_chip_addr_dist_.get_meter_sweep_subword_shift(alu); }
    int     get_meter_sweep_subwords(int alu)              { return mau_chip_addr_dist_.get_meter_sweep_subwords(alu); }
    int     get_meter_sweep_op4(int alu)                   { return mau_chip_addr_dist_.get_meter_sweep_op4(alu); }

    void     get_row_usage(uint32_t rows, bool ingress, int s_or_m,
                           uint32_t *hdr_rows, uint32_t *eop_rows, uint32_t *teop_rows);
    uint32_t deferred_ram_rows(uint32_t rows, bool ingress, int s_or_m);
    uint32_t deferred_ram_oflow(uint32_t rows, bool ingress, int s_or_m);
    uint32_t deferred_ram_err_squash(uint32_t rows, bool ingress, int s_or_m, bool err);
    void     setup_rewrite_eop_addrs(uint8_t addrtype, int lt, int src, int dst, uint32_t mask);
    uint32_t rewrite_eop_addr(uint8_t addrtype, int lt, uint32_t addr);
    bool get_eop_addr(uint8_t addrtype, int lt, uint8_t eop_num, uint32_t *addr, uint32_t *rows);
    void set_eop_addr(uint8_t addrtype, int lt, uint8_t eop_num, uint32_t addr, uint32_t rows);
    void clear_eop_addr(uint8_t addrtype, int lt, uint8_t eop_num);

    void bus_not_driven_complain(const std::string& bus_name, int row) const;
    void ingress_egress_mismatch_complain(const std::string& bus_name, int row) const;
    void addr_dist_debug(const std::string& bus_name, int lt, uint32_t curr_val, uint32_t or_val);
    void pre_pfe_addr_en_noten_check(const std::string& bus_name, uint32_t en_addr, uint32_t not_en_addr);
    void addr_multi_write_check(const std::string& bus_name,
                                uint32_t curr_val, uint32_t or_val, int curr_lt, int or_lt);
    void addrtype_multi_write_check(const std::string& bus_name, uint8_t curr_typ, uint8_t or_typ);
    void addrtype_mismatch_check(const std::string& bus_name, uint8_t written_typ, uint8_t use_typ) const;
    void eop_free_check(const char *eop_name, uint32_t eop_addr, uint8_t eop_num);
    void invalid_row_used_check(const std::string& bus_name, uint32_t rows_used, uint32_t rows_valid);
    void oflow2_used_check(const std::string& bus_name, uint32_t val);
    void oflow_used_check(const std::string& bus_name, uint8_t use_typ);
    void single_alu_check(const char *alu_name, uint32_t alus);


    // To ease access to AdrDist regs which all have same fields
    template <typename T> uint32_t get_home_rows(T *reg, int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      return static_cast<uint32_t>(reg->address_distr_to_logical_rows(lt));
    }
    template <typename T> uint32_t get_oflow(T *reg, int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      return static_cast<uint32_t>(reg->address_distr_to_overflow(lt));
    }
    template <typename T> uint32_t get_oflow2_up(T *reg, int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      uint32_t val = static_cast<uint32_t>(reg->address_distr_to_overflow2_up(lt));
      oflow2_used_check("oflow2_up_addr", val);
      return (kDisableOflow2) ?0u :val;
    }
    template <typename T> uint32_t get_oflow2_down(T *reg, int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      uint32_t val = static_cast<uint32_t>(reg->address_distr_to_overflow2_down(lt));
      oflow2_used_check("oflow2_down_addr", val);
      return (kDisableOflow2) ?0u :val;
    }
    template <typename T> uint32_t get_adist_mask(T *reg, int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      uint32_t mask = (get_home_rows(reg, lt) & kAdistMaskHomeRows);
      if (get_oflow(reg, lt) != 0u) mask |= kAdistMaskOflow;
      if (get_oflow2_up(reg, lt) != 0u) mask |= kAdistMaskOflow2Up;
      if (get_oflow2_down(reg, lt) != 0u) mask |= kAdistMaskOflow2Dn;
      return mask;
    }
    template <typename T> uint32_t get_stats_adist_mask(T *reg, int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      uint32_t alus = reg->adr_dist_stats_adr_icxbar_ctl(lt);
      single_alu_check("stats_alu_icxbar", alus);
      return MauStatsAlu::map_stats_alus_to_rows(alus);
    }
    template <typename T> uint32_t get_meter_adist_mask(T *reg, int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      uint32_t alus = reg->adr_dist_meter_adr_icxbar_ctl(lt);
      single_alu_check("meter_alu_icxbar", alus);
      return MauMeterAlu::map_meter_alus_to_rows(alus);
    }


    bool distribute_to_oflow(uint32_t adistMask, int lt,
                             uint32_t addr, uint8_t addrtype) {
      bool dist_to_oflow = false;
      if ((adistMask & kAdistMaskOflow) != 0u) {
        const char *bus = "oflow_addr";
        addr_dist_debug(bus, lt, oflow_addr_, addr);
        addr_multi_write_check(bus, oflow_addr_, addr,
                               oflow_src_logtab_, lt);
        addrtype_multi_write_check(bus, oflow_addrtype_, addrtype);
        oflow_used_check(bus, addrtype);
        // OR in addrs exactly as H/W but only maintain last addrtype/first logtab
        oflow_addr_ |= addr;
        oflow_addrtype_ = addrtype;
        if (oflow_src_logtab_ == 0xFF) oflow_src_logtab_ = lt;
        dist_to_oflow = true;
      }
      if ((adistMask & kAdistMaskOflow2Up) != 0u) {
        const char *bus = "oflow2_up_addr";
        addr_dist_debug(bus, lt, oflow2_up_addr_, addr);
        addr_multi_write_check(bus, oflow2_up_addr_, addr,
                               oflow2_up_src_logtab_, lt);
        addrtype_multi_write_check(bus, oflow2_up_addrtype_, addrtype);
        oflow2_up_addr_ |= addr;
        oflow2_up_addrtype_ = addrtype;
        if (oflow2_up_src_logtab_ == 0xFF) oflow2_up_src_logtab_ = lt;
        dist_to_oflow = true;
      }
      if ((adistMask & kAdistMaskOflow2Dn) != 0u) {
        const char *bus = "oflow2_down_addr";
        addr_dist_debug(bus, lt, oflow2_down_addr_, addr);
        addr_multi_write_check(bus, oflow2_down_addr_, addr,
                               oflow2_down_src_logtab_, lt);
        addrtype_multi_write_check(bus, oflow2_down_addrtype_, addrtype);
        oflow2_down_addr_ |= addr;
        oflow2_down_addrtype_ = addrtype;
        if (oflow2_down_src_logtab_ == 0xFF) oflow2_down_src_logtab_ = lt;
        dist_to_oflow = true;
      }
      return dist_to_oflow;
    }

    uint32_t get_idle_oxbar_word(int which_word) {
      RMT_ASSERT((which_word >=0) && (which_word < kIdletimeWords));
      return adr_dist_idletime_oxbar_.adr_dist_idletime_adr_oxbar_ctl(which_word);
    }

    // Funcs to handle deferred stats/meter addresses
    bool is_stats_deferred(int alu) {
      return (packet_action_at_hdrtime_.packet_action_at_headertime(0,alu) == 0);
    }
    bool is_meter_deferred(int alu) {
      return (packet_action_at_hdrtime_.packet_action_at_headertime(1,alu) == 0);
    }

    bool get_eop_stats_addr(int lt, uint8_t eop_num, uint32_t *addr, uint32_t *rows) {
      return get_eop_addr(AddrType::kStats, lt, eop_num, addr, rows);
    }
    bool get_eop_meter_addr(int lt, uint8_t eop_num, uint32_t *addr, uint32_t *rows) {
      return get_eop_addr(AddrType::kMeter, lt, eop_num, addr, rows);
    }

    void set_eop_stats_addr(int lt, uint8_t eop_num, uint32_t addr, uint32_t rows) {
      set_eop_addr(AddrType::kStats, lt, eop_num, addr, rows);
    }
    void set_eop_meter_addr(int lt, uint8_t eop_num, uint32_t addr, uint32_t rows) {
      set_eop_addr(AddrType::kMeter, lt, eop_num, addr, rows);
    }
    void clear_eop_stats_addr(int lt, uint8_t eop_num) {
      clear_eop_addr(AddrType::kStats, lt, eop_num);
    }
    void clear_eop_meter_addr(int lt, uint8_t eop_num) {
      clear_eop_addr(AddrType::kMeter, lt, eop_num);
    }

    // Internal funcs allowing PBUS access
    uint32_t calculate_deferred_rows(uint8_t addrtype, int lt, uint8_t eop_num);
    bool row_read_eop_addr(uint8_t addrtype, int logrow, uint8_t eop_num, uint32_t *addr);
    void row_write_eop_addr(uint8_t addrtype, int logrow, uint8_t eop_num, uint32_t addr);



    Mau                                                   *mau_;
    RmtSweeper                                            *sweeper_;
    MauStatefulCounters                                   *mau_stateful_counters_;
    MauTeop                                               *mau_teop_;

    MauChipAddrDist                                        mau_chip_addr_dist_;
    register_classes::AdrDistActionDataAdrIcxbarCtlArray   adr_dist_action_icxbar_;
    register_classes::AdrDistStatsAdrIcxbarCtlArray        adr_dist_stats_icxbar_;
    register_classes::AdrDistMeterAdrIcxbarCtlArray        adr_dist_meter_icxbar_;
    register_classes::AdrDistIdletimeAdrOxbarCtlArray      adr_dist_idletime_oxbar_;
    register_classes::PacketActionAtHeadertimeArray2       packet_action_at_hdrtime_;
    // These 2 now in mau-chip-addr-dist.h
    //register_classes::DeferredRamCtlArray2               deferred_ram_ctl_;
    //register_classes::DeferredOfloCtl                    deferred_oflo_ctl_;
    register_classes::MeterSweepCtlArray                   meter_sweep_ctl_;
    register_classes::IdletimeSweepCtlArray                idletime_sweep_ctl_;
    register_classes::StatsLrtFsmSweepSizeArray            stats_lrt_fsm_sweep_size_;
    register_classes::IdleDumpCtlArray                     idletime_dump_ctl_;
    register_classes::StatsDumpCtlArray                    stats_dump_ctl_;
    register_classes::OfloAdrUserArray                     oflo_adr_user_;
    register_classes::MauCfgStatsAluLtArray                stats_alu_lt_map_;
    register_classes::IntrStatusMauAdMutable               intr_status_mau_ad_;

    //register_classes::MauCfgLtHasIdle                    cfg_has_idle_;
    //register_classes::MauCfgLtHasStats                   cfg_has_stats_;
    //register_classes::MauCfgLtStatsAreDirect             cfg_stats_are_direct_;
    //register_classes::MauCfgLtMeterAreDirect             cfg_meter_are_direct_;

    uint8_t                                            oflow_addrtype_;
    uint8_t                                            oflow2_up_addrtype_;
    uint8_t                                            oflow2_down_addrtype_;
    uint32_t                                           oflow_addr_;
    uint32_t                                           oflow2_up_addr_;
    uint32_t                                           oflow2_down_addr_;

    std::array< uint32_t, kLogicalRows>                action_addrs_ { };
    std::array< uint32_t, kLogicalRows>                stats_addrs_ { };
    std::array< uint32_t, kLogicalRows>                meter_addrs_ { };
    std::array< uint32_t, kIdletimeBuses>              idletime_addrs_ { };

    // For debug keep track per-row/per-bus what addrs are consumed by RAMs
    uint32_t                                           action_addr_not_consumed_;
    uint32_t                                           stats_addr_not_consumed_;
    uint32_t                                           meter_addr_not_consumed_;
    uint32_t                                           idletime_addr_not_consumed_;

    uint8_t                                            oflow_src_logtab_;
    uint8_t                                            oflow2_up_src_logtab_;
    uint8_t                                            oflow2_down_src_logtab_;
    std::array< uint8_t, kLogicalRows>                 action_src_logtabs_ { };
    std::array< uint8_t, kLogicalRows>                 stats_src_logtabs_ { };
    std::array< uint8_t, kLogicalRows>                 meter_src_logtabs_ { };
    std::array< uint8_t, kIdletimeBuses>               idle_src_logtabs_ { };
    uint32_t                                           ingress_logtabs_;
    uint32_t                                           egress_logtabs_;
    std::array< uint32_t, kLogicalRows>                action_src_logtabmasks_ { };
    std::array< uint32_t, kLogicalRows>                stats_src_logtabmasks_ { };
    std::array< uint32_t, kLogicalRows>                meter_src_logtabmasks_ { };
    std::array< AddrClaim, kNumStatsAlus>              stats_addrclaims_ { };
    std::array< AddrClaim, kNumMeterAlus>              meter_addrclaims_ { };

    // Addresses/LTs pre-PFE check
    std::array< uint32_t, kLogicalRows>                pre_pfe_en_action_addrs_ { };
    std::array< uint32_t, kLogicalRows>                pre_pfe_en_stats_addrs_ { };
    std::array< uint32_t, kLogicalRows>                pre_pfe_en_meter_addrs_ { };
    std::array< uint32_t, kIdletimeBuses>              pre_pfe_en_idletime_addrs_ { };
    std::array< uint32_t, kLogicalRows>                pre_pfe_not_en_action_addrs_ { };
    std::array< uint32_t, kLogicalRows>                pre_pfe_not_en_stats_addrs_ { };
    std::array< uint32_t, kLogicalRows>                pre_pfe_not_en_meter_addrs_ { };
    std::array< uint32_t, kIdletimeBuses>              pre_pfe_not_en_idletime_addrs_ { };
    std::array< uint8_t, kLogicalRows>                 pre_pfe_action_src_logtabs_ { };
    std::array< uint8_t, kLogicalRows>                 pre_pfe_stats_src_logtabs_ { };
    std::array< uint8_t, kLogicalRows>                 pre_pfe_meter_src_logtabs_ { };
    std::array< uint8_t, kIdletimeBuses>               pre_pfe_idle_src_logtabs_ { };

    uint32_t                                                curr_seq_;
    uint32_t                                                pending_seq_;
    std::unordered_map< uint16_t, std::array<uint32_t,2> >  eop_map_;
    std::array< DeferredRamRewriteInfo, kLogicalTables >    stats_deferred_ram_rewrite_info_;
    std::array< DeferredRamRewriteInfo, kLogicalTables >    meter_deferred_ram_rewrite_info_;
    std::array< SweepInfo, kLogicalTables >                 idle_sweep_info_;
    std::array< SweepInfo, kLogicalTables >                 idle_dump_info_;
    std::array< SweepInfo, kLogicalTables >                 stats_dump_info_;
    std::array< SweepInfo, kNumMeterAlus >                  meter_sweep_info_;
    std::array< SweepTimeInfo, kNumMeterAlus >              meter_sweep_time_info_;
    std::array< std::deque<ColorWriteEntry>, kNumMeterAlus> color_write_info_;

  };
}
#endif // _SHARED_MAU_ADDR_DIST_
