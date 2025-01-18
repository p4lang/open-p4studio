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

#ifndef _JBAY_SHARED_MAU_STATEFUL_COUNTERS_
#define _JBAY_SHARED_MAU_STATEFUL_COUNTERS_

#include <vector>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <mau-addr-dist.h>
#include <mau-lookup-result.h>
#include <mau-execute-step.h>

#include <register_includes/mau_stateful_log_counter_array2_mutable.h>

#include <register_includes/mau_stateful_log_counter_ctl_array2.h>
#include <register_includes/mau_stateful_log_counter_ctl2_array.h>
#include <register_includes/mau_stateful_log_counter_ctl3_array.h>
#include <register_includes/mau_stateful_log_counter_clear_mutable.h>
#include <register_includes/mau_stateful_log_ctl_ixbar_map_array3.h>
#include <register_includes/mau_stateful_log_counter_oxbar_map_array.h>
#include <register_includes/mau_stateful_log_counter_logical_map_array.h>
#include <register_includes/mau_stateful_log_stage_vpn_offset_array.h>
#include <register_includes/mau_stateful_log_fifo_level_array_mutable.h>
#include <register_includes/mau_stateful_log_watermark_threshold_array.h>
#include <register_includes/mau_stateful_log_cur_watermark_array_mutable.h>
#include <register_includes/meter_alu_adr_range_check_icxbar_map_array.h>
#include <register_includes/mau_meter_alu_vpn_range_array.h>
#include <register_includes/meter_adr_shift.h>
#include <register_includes/stateful_instr_width_logical_array.h>
#include <register_includes/adr_dist_table_thread_array2.h>

namespace MODEL_CHIP_NAMESPACE {

template <int w> constexpr uint32_t make_mask() {
  static_assert( w < 32, "Mask width must be 32 or less" );
  return (w==32) ? 0xFFFFFFFFu : (1u<<w)-1;
}

class MauDependencies;

class MauStatefulCounters : public MauObject {
  static constexpr int      kType = RmtTypes::kRmtTypeMauTableCounters; // TODO: add another type
  static constexpr int      kLogicalTables = MauDefs::kLogicalTablesPerMau;
  static constexpr int      kNumAlus = MauDefs::kNumAlus;
  static constexpr int      kNumStatefulCounters = MauDefs::kNumStatefulCounters;
  static constexpr int      kSramAddressWidth  = MauDefs::kSramAddressWidth;
  static constexpr int      kSramEntries = 1<<kSramAddressWidth;
  static constexpr int      kMaxSubwordBits = 4; // for 8 bit entries
  static constexpr int      kStatefulAddrStageBits = 3;
  static constexpr int      kStatefulAddrVpnBits = MauDefs::kMapramVpnBits + kStatefulAddrStageBits;
  static constexpr int      kWatermarkThresholdShift = 7; // threshold is in units of 128
  static constexpr int      kStatefulAddressWidth = 23;

  static constexpr int      kCounterPadding      = 3;  // zeros added before sending to ALU
  static constexpr uint64_t kTmax = UINT64_C(0xFFFFFFFFFFFFFFFF);
  static constexpr uint64_t kIsBeingClearedTmax = kTmax-1;
  static constexpr uint8_t  kFlagStartPending = 0x1;
  static constexpr uint8_t  kFlagRunning      = 0x2;
  static constexpr uint8_t  kFlagOverflowed   = 0x4;
  static constexpr uint8_t  kFlagUnderflowed  = 0x8;

 public:
  static bool kRelaxSwPushPopInvalidErrors; // Defined in rmt-config.cpp
  static bool kRelaxSwPushPopOverflowUnderflowErrors;
  static bool kSynchronousStatefulCounterClear;
  static bool kStatefulCounterTickCheckTime;

  MauStatefulCounters(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
  ~MauStatefulCounters();

  bool stateful_counter_enabled(int lt);
  bool stateful_counter_at_max(int counter);
  uint32_t maybe_increment_stateful_counter(uint32_t addr, int lt, const MauLookupResult &res);
  void tick_counters( const std::array<MauLookupResult,kLogicalTables> &lookup_results, MauExecuteState* state );

  void reset_resources();
  // get_meter_addr for a particular ALU, must be run before get_immediate_data.
  uint32_t get_meter_addr( int alu, uint32_t meter_addr_in, uint64_t T );
  uint32_t get_immediate_data( int lt, uint32_t imm_data_in, uint32_t meter_addr_in );
  uint32_t get_final_addr(int alu); // For DV, return addr before final VPN squash

  bool is_counter_for_lt_being_cleared(int lt);
  void continue_clear_till(uint64_t active_counters, uint64_t T_stop);
  void start_clear_at(int counter, uint64_t T);
  void maybe_synchronous_clear(bool lock);
  void push_pop_stateful_instr(bool push, int counter, uint32_t incr, uint64_t T);
  bool any_counter_flagged()        { return is_any_counter_flagged(); }
  bool alu_counter_flagged(int alu) { return is_alu_counter_flagged(alu); }
  void advance_time(int lt, uint64_t T, const char *why_str, uint32_t why_val);




 private:
  void refresh_lt_cntr_gress();
  uint8_t counters_in_gress(uint8_t gress_mask);
  uint8_t counters_in_cntr_gress(int cntr);
  uint8_t counters_in_lt_gress(int lt);
  void stateful_ctl2_callback(uint32_t counter);
  void stateful_counter_clear_callback();
  void stateful_ixbar_map_callback(uint32_t g01, uint32_t lr, uint32_t pushpop);
  void ad_thread_change_callback(uint32_t ie, uint32_t repl);


  // Get type of MAU stateful log counter
  CntrType get_type_stateful_counter(int lt, int which) {

    uint32_t reg = mau_stateful_log_counter_ctl_.mau_stateful_log_counter_ctl(lt / 8, which);
    uint32_t ctl = (reg >> ((lt % 8) * 4)) & 0xf;
    switch (ctl) {
      case 0: return CntrType::kDisabled;
      case 1: return CntrType::kTableMiss;
      case 2: return CntrType::kTableHit;
      case 3: return CntrType::kGatewayInhibit;
      case 4: return CntrType::kUnconditional;
      case 5: return CntrType::kGatewayInhibitEntry0;
      case 6: return CntrType::kGatewayInhibitEntry1;
      case 7: return CntrType::kGatewayInhibitEntry2;
      case 8: return CntrType::kGatewayInhibitEntry3;
      case 9: return CntrType::kGatewayInhibitMiss;
      default: RMT_ASSERT(0); return CntrType::kDisabled;
    }
  }
  bool counter_event_happened(int lt, int which_event, const MauLookupResult& res );


  // Generic flag getter/setter/clearer
  bool is_cntr_flag_set(int counter, uint8_t flag) {
    return ((cntr_flags_[counter] & flag) != 0);
  }
  void set_cntr_flag(int counter, uint8_t flag) {
    cntr_flags_[counter] |= flag;
  }
  void clr_cntr_flag(int counter, uint8_t flag) {
    cntr_flags_[counter] &= ~flag;
  }
  // Specific kFlagStartPending get/set/clear
  bool is_clear_counter_start_pending(int counter) {
    return is_cntr_flag_set(counter, kFlagStartPending);
  }
  bool is_clear_counter_start_pending(int counter, uint64_t T) {
    return (is_clear_counter_start_pending(counter) && (T == T_clear_next_[counter]));
  }
  void set_clear_counter_start_pending(int counter) {
    set_cntr_flag(counter, kFlagStartPending);
  }
  void clr_clear_counter_start_pending(int counter) {
    clr_cntr_flag(counter, kFlagStartPending);
  }
  // Specific kFlagRunning get/set/clear
  bool is_clear_counter_running(int counter) {
    return is_cntr_flag_set(counter, kFlagRunning);
  }
  bool is_clear_counter_running(int counter, uint64_t T) {
    return (is_clear_counter_running(counter) && (T == T_clear_next_[counter]));
  }
  void set_clear_counter_running(int counter) {
    set_cntr_flag(counter, kFlagRunning);
  }
  void clr_clear_counter_running(int counter) {
    clr_cntr_flag(counter, kFlagRunning);
  }
  // StartPending OR Running
  bool is_clear_counter_active(int counter) {
    return is_clear_counter_start_pending(counter) || is_clear_counter_running(counter);
  }
  bool is_clear_counter_active(int counter, uint64_t T) {
    return is_clear_counter_start_pending(counter, T) || is_clear_counter_running(counter, T);
  }
  // StartPending OR Running OR Overflowed OR Underflowed
  bool is_counter_flagged(int counter) {
    uint8_t any_flag = kFlagStartPending|kFlagRunning|kFlagOverflowed|kFlagUnderflowed;
    return is_cntr_flag_set(counter, any_flag);
  }
  // StartPending OR Running OR Overflowed OR Underflowed for ANY counter
  bool is_any_counter_flagged() {
    for (int i = 0; i < kNumStatefulCounters; i++) {
      if (is_counter_flagged(i)) return true;
    }
    return false;
  }
  // StartPending OR Running OR Overflowed OR Underflowed for the counter of a given ALU
  bool is_alu_counter_flagged(int alu) {
    if (mau_stateful_log_counter_oxbar_map_.stateful_log_counter_oxbar_enable(alu) == 1) {
      int counter = mau_stateful_log_counter_oxbar_map_.stateful_log_counter_oxbar_ctl(alu);
      return is_counter_flagged(counter);
    }
    return false;
  }


  bool event_enabled(int counter, bool is_push) {
    return is_push ?
        mau_stateful_log_counter_ctl2_.slog_push_event_ctl(counter) :
        mau_stateful_log_counter_ctl2_.slog_pop_event_ctl(counter);
  }

  bool vpn_in_range(int alu, int vpn) {
    int enable = mau_meter_alu_vpn_range_.meter_vpn_range_check_enable(alu);
    if ( enable ) {
      int base    = mau_meter_alu_vpn_range_.meter_vpn_base(alu);
      int limit   = mau_meter_alu_vpn_range_.meter_vpn_limit(alu);
      if ( vpn >= base && vpn <= limit ) {
        return true;
      }
      return false;
    }
    return true; // in range if check not enabled
  }

  void set_vpn_in_range_bit(int alu) {
    // one bit per logical table
    auto& v = meter_alu_adr_range_check_icxbar_map_.meter_alu_adr_range_check_icxbar_map(alu);
    for (int lt=0;lt<kLogicalTables;++lt) {
      if ((v>>lt) & 1)
        vpn_in_range_[lt] = true;
    }
  }

  void set_counter_output_value(int counter,uint32_t value) {
    counter_output_[counter].or_in( value & no_extra_bit_mask_[counter] );
    RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::set_counter_output_value counter %d: value=%06x output_value=%06x (mask=%06x)\n",
            counter,value,counter_output_[counter].to_uint32(),no_extra_bit_mask_[counter]);
  }
  void set_output_overflow(int counter) {
    int instr = mau_stateful_log_counter_ctl3_.slog_overflow_instruction(counter);
    counter_output_[counter].set_overflow_or_underflow(instr);
  }
  void set_output_underflow(int counter) {
    int instr = mau_stateful_log_counter_ctl3_.slog_underflow_instruction(counter);
    counter_output_[counter].set_overflow_or_underflow(instr);
  }

  // What is VPN pos in counter -
  int get_vpn_pos_in_counter(int counter) {
    return word_portion_width_[counter];
  }

  // Get raw value from _vpn_limit reg
  int get_vpn_limit(int counter) {
    return ( mau_stateful_log_counter_ctl2_.slog_vpn_limit(counter) );
  }
  int get_vpn_base(int counter) {
    return ( mau_stateful_log_counter_ctl2_.slog_vpn_base(counter) );
  }
  // Return min allowed value stateful counter (determined by vpn min)
  int min_value(int counter) {
    int pos = get_vpn_pos_in_counter(counter);
    // Return min VPN shifted up to correct pos with 0s in LSBs
    return (get_vpn_base(counter) << pos) | 0u;
  }
  // Return max allowed value stateful counter (determined by vpn max)
  int max_value(int counter) {
    int pos = get_vpn_pos_in_counter(counter);
    // Return max VPN shifted up to correct pos with 1s in LSBs
    return (get_vpn_limit(counter) << pos) | ((1u << pos) - 1);
  }
  // this is the value that stacks and stateful logging have when full
  int wrapped_max_value(int counter) {
    return extra_bit_mask_[counter] | min_value(counter);
  }
  int increment_with_wrap(int counter, int value);
  int decrement_with_wrap(int counter, int value);

  bool tick_logging(int counter, uint64_t T);
  bool tick_clear_counter(int counter, uint64_t T);
  bool push_fifo(int counter, bool pop_also, uint64_t T);
  bool pop_fifo(int counter, bool push_also, uint64_t T);
  bool push_stack(int counter, bool pop_also, uint64_t T);
  bool pop_stack(int counter, bool push_also, uint64_t T);

  // check the counter is in range, it can get out of range if
  //  the vpn base and limit are changed, or the counter value
  //  is written incorrectly
  bool check_counter_value( int counter, int value );

  // Reset MAU stateful counter (to min value)
  void reset_stateful_counter(int counter) {
    spinlock();
    uint32_t min_v = min_value(counter);
    mau_stateful_log_counter_array_.mau_stateful_log_counter(counter,0,min_v);
    mau_stateful_log_counter_array_.mau_stateful_log_counter(counter,1,min_v);
    mau_stateful_log_fifo_level_.mau_stateful_log_fifo_level(counter,0);
    mau_stateful_log_cur_watermark_.mau_stateful_log_cur_watermark(counter,0);
    spinunlock();
  }

  void update_levels(int counter, bool push) {
    //lock(); // already locked when this is called
    int v = mau_stateful_log_fifo_level_.mau_stateful_log_fifo_level(counter);
    mau_stateful_log_fifo_level_.mau_stateful_log_fifo_level(counter, v + (push ? 1 : -1));
  }
  void update_levels_push(int counter) { update_levels(counter,true); }
  void update_levels_pop(int counter)  { update_levels(counter,false); }

  void update_thresholds(int counter, bool push) {
    if (mau_stateful_log_counter_ctl2_.slog_watermark_enable(counter)) {
      // 0==count pushes, 1==count pops.
      bool count_pushes = (0 == mau_stateful_log_counter_ctl2_.slog_watermark_ctl(counter));
      if ( push == count_pushes ) {
        // update the current watermark, wrap using the threshold
        int cw = mau_stateful_log_cur_watermark_.mau_stateful_log_cur_watermark(counter);
        int threshold = mau_stateful_log_watermark_threshold_.mau_stateful_log_watermark_threshold(counter);
        // threshold is given in terms of multiple pushes/pops, so needs shifting
        int new_cw = ( cw == ((threshold << kWatermarkThresholdShift)-1)) ? 0 : cw+1;
        mau_stateful_log_cur_watermark_.mau_stateful_log_cur_watermark(counter,new_cw);
        RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::update_thresholds counter %d: count %s old value=%d new value=%d (threshold=%d)\n",
                counter,push?"pushes":"pops",cw,new_cw,threshold);
      }
    }
    //unlock();
  }
  void update_thresholds_push(int counter) { update_thresholds(counter,true); }
  void update_thresholds_pop(int counter)  { update_thresholds(counter,false); }


  int meter_adr_shift(int counter) {
    int shift=0;
    switch (counter) {
      case 0: shift = meter_adr_shift_.meter_adr_shift0(); break;
      case 1: shift = meter_adr_shift_.meter_adr_shift1(); break;
      case 2: shift = meter_adr_shift_.meter_adr_shift2(); break;
      case 3: shift = meter_adr_shift_.meter_adr_shift3(); break;
      default: RMT_ASSERT(0); break;
    }
    // test for illegal values
    RMT_ASSERT( (shift != 1) && (shift != 2) );
    return shift;
  }

  ///// Classes for the 3 different address formats in Stateful logging
  class AddrNoStageId;
  class AddrUnpadded;
  static constexpr int kCounterPartPos   = 0;
  static constexpr int kInstrPartWidth   = 4;
  // AddrWithStageId has a stage ID in the VPN and VPN is in fixed position.
  class AddrWithStageId {
    friend class AddrNoStageId;
    friend class AddrUnpadded;
   public:
    AddrWithStageId() : addr_(0) {}
    AddrWithStageId(const AddrWithStageId& o) : addr_(o.addr_) {}
    ~AddrWithStageId() {}
    AddrWithStageId& operator=(const AddrWithStageId&) = delete;
    AddrWithStageId or_in( AddrWithStageId o ) {  addr_ |= o.addr_;  return *this; }
    AddrWithStageId instr_part()   const { return AddrWithStageId( addr_ & kInstrPartMask ); }
    AddrWithStageId counter_part() const { return AddrWithStageId( addr_ & kCounterPartMask ); }
    int get_vpn() const { return (addr_ & kVpnPartMask) >> kVpnPartPos; }
    AddrWithStageId shift_left(int x) const { return AddrWithStageId( addr_ << x ); }
    bool squash_bit_non_zero() const { return ( (addr_ & (kInstrPartMask|kSquashBitMask)) != 0 ); }
    bool instr_non_zero() const { return ( (addr_ & (kInstrPartMask)) != 0 ); }
    AddrNoStageId remove_stage_id() const;
    uint32_t to_uint32() { return addr_; }
   private:
    AddrWithStageId(uint32_t v) : addr_(v) {} // only for use internally and AddrUnpadded::pad()
    uint32_t addr_;
    static constexpr int kCounterPartWidth = kStatefulAddressWidth + kCounterPadding;
    static constexpr int kCounterPartMask  = make_mask<kCounterPartWidth>() << kCounterPartPos;
    static constexpr int kCounterNoStageIdPartMask  = make_mask<kCounterPartWidth-kStatefulAddrStageBits>() << kCounterPartPos;
    static constexpr int kInstrPartPos     = kCounterPartPos + kCounterPartWidth;
    static constexpr int kInstrPartMask    = make_mask<kInstrPartWidth>() << kInstrPartPos;
    static constexpr int kSquashBitPos     = kInstrPartPos + kInstrPartWidth;
    static constexpr int kSquashBitMask    = 1 << kSquashBitPos;

    static constexpr int kVpnPartWidth = kStatefulAddrVpnBits;
    static constexpr int kVpnPartPos   = kSramAddressWidth + kMaxSubwordBits + kCounterPadding;
    static constexpr int kVpnPartMask  = make_mask<kVpnPartWidth>() << kVpnPartPos;
  };
  // AddrNoStageId does not have a stage ID, VPN is in a fixed position
  class AddrNoStageId {
   public:
    AddrNoStageId(uint32_t v) : addr_(v) {}
    AddrNoStageId(const AddrNoStageId& o) : addr_(o.addr_) {}
    AddrNoStageId& operator=(const AddrNoStageId&) = delete;
    AddrWithStageId add_stage_id() const {
      return AddrWithStageId( ((addr_ & (kInstrPartMask|kSquashBitMask)) << kStatefulAddrStageBits) |
                              ( addr_ & kCounterPartMask ) );
    }
    uint32_t to_uint32() { return addr_; }
    ~AddrNoStageId() {}
   private:
    uint32_t addr_;

    static constexpr int kCounterPartWidth = kStatefulAddressWidth + kCounterPadding - kStatefulAddrStageBits;
    static constexpr int kCounterPartMask  = make_mask<kCounterPartWidth>() << kCounterPartPos;
    static constexpr int kInstrPartPos     = kCounterPartPos + kCounterPartWidth;
    static constexpr int kInstrPartMask    = make_mask<kInstrPartWidth>() << kInstrPartPos;
    static constexpr int kSquashBitPos     = kInstrPartPos + kInstrPartWidth;
    static constexpr int kSquashBitMask    = 1 << kSquashBitPos;
  };
  // AddrUnpadded does not have a stage ID, but the VPN position depends on the
  //   the stateful instr width.
  class AddrUnpadded {
   public:
    AddrUnpadded(uint32_t v) : addr_(v) {}
    AddrUnpadded() : addr_(0) {}
    AddrUnpadded(const AddrUnpadded& o) : addr_(o.addr_) {}
    ~AddrUnpadded() {}
    AddrUnpadded& operator=(const AddrUnpadded&) = delete;
    AddrUnpadded reset()  { addr_=0; return *this; }
    AddrUnpadded or_in( AddrUnpadded o )  { addr_ |= o.addr_; return *this; }
    AddrUnpadded or_in_instr( uint32_t instr ) {
      addr_ |= ((instr << kInstrPartPos) & kInstrPartMask);
      return *this;
    }
    AddrWithStageId pad( uint32_t extra_padding ) {
      // Shift everything up kCounterPadding. Shift counter part by extra_padding too.
      return AddrWithStageId( ( (addr_ & (kInstrPartMask|kSquashBitMask)) << kCounterPadding) |
                              ( (addr_ & kCounterPartMask) << (extra_padding + kCounterPadding)));
    }
    AddrUnpadded set_overflow_or_underflow(int instr) {
      // set the instr, unless it is zero, in which case set the squash bit
      addr_ |= (instr ? ( instr << kInstrPartPos ) :
                ( 1 << kSquashBitPos) );
      return *this;
    }
    AddrUnpadded instr_part()   const { return AddrUnpadded( addr_ & kInstrPartMask ); }
    AddrUnpadded counter_part() const { return AddrUnpadded( addr_ & kCounterPartMask ); }
    bool instr_or_squash_bit_non_zero() const { return ( (addr_ & (kInstrPartMask|kSquashBitMask)) != 0 ); }
    AddrUnpadded reduce_4b_instr_to_3b_instr()  {
      addr_&= make_mask<kCounterPartWidth+kInstrPartWidth-1>();
      return *this;
    }
    AddrUnpadded or_in_range_bit( bool in_range )  {
      const int kInRangeBit = kCounterPartWidth+kInstrPartWidth-1;
      addr_ |=  (in_range?1:0) << kInRangeBit;
      return *this;
    }
    AddrUnpadded subtract_vpn_offset( int vpn_offset, int instr_width ) {
      int vpn_pos = kSramAddressWidth + kMaxSubwordBits - instr_width;
      int vpn = (addr_ >> vpn_pos) & kStatefulAddrVpnMask;
      vpn -= vpn_offset;
      vpn &= kStatefulAddrVpnMask;
      addr_ &= ~ (kStatefulAddrVpnMask << vpn_pos);
      addr_ |= vpn << vpn_pos;
      return *this;
    }
    uint32_t to_uint32() { return addr_; }
   private:
    static constexpr int kCounterPartWidth = kStatefulAddressWidth + kCounterPadding - kStatefulAddrStageBits;
    static constexpr int kCounterPartMask  = make_mask<kCounterPartWidth>() << kCounterPartPos;
    static constexpr int kInstrPartPos     = kCounterPartPos + kCounterPartWidth;
    static constexpr int kInstrPartMask    = make_mask<kInstrPartWidth>() << kInstrPartPos;
    static constexpr int kSquashBitPos     = kInstrPartPos + kInstrPartWidth;
    static constexpr int kSquashBitMask    = 1 << kSquashBitPos;
    static constexpr int kInstructionAndSquashMask = make_mask<kInstrPartWidth+1>();
    static constexpr uint32_t kStatefulAddrVpnMask = make_mask<kStatefulAddrVpnBits>(); // shift vpn down before using
    uint32_t addr_;

  };

  ///////////////////////////////
 private:
  bool                                                     ctor_running_;

  register_classes::MauStatefulLogCounterArray2Mutable     mau_stateful_log_counter_array_;
  register_classes::MauStatefulLogCounterCtlArray2         mau_stateful_log_counter_ctl_;
  register_classes::MauStatefulLogCounterCtl2Array         mau_stateful_log_counter_ctl2_;
  register_classes::MauStatefulLogCounterCtl3Array         mau_stateful_log_counter_ctl3_;
  register_classes::MauStatefulLogCounterClearMutable      mau_stateful_log_counter_clear_;
  register_classes::MauStatefulLogCtlIxbarMapArray3        mau_stateful_log_ctl_ixbar_map_;
  register_classes::MauStatefulLogCounterOxbarMapArray     mau_stateful_log_counter_oxbar_map_;
  register_classes::MauStatefulLogCounterLogicalMapArray   mau_stateful_log_counter_logical_map_;
  register_classes::MauStatefulLogStageVpnOffsetArray      mau_stateful_log_stage_vpn_offset_;
  register_classes::MauStatefulLogFifoLevelArrayMutable    mau_stateful_log_fifo_level_;
  register_classes::MauStatefulLogWatermarkThresholdArray  mau_stateful_log_watermark_threshold_;
  register_classes::MauStatefulLogCurWatermarkArrayMutable mau_stateful_log_cur_watermark_;
  register_classes::MeterAluAdrRangeCheckIcxbarMapArray    meter_alu_adr_range_check_icxbar_map_;
  register_classes::MauMeterAluVpnRangeArray               mau_meter_alu_vpn_range_;
  register_classes::MeterAdrShift                          meter_adr_shift_;
  register_classes::StatefulInstrWidthLogicalArray         stateful_instr_width_logical_;
  register_classes::AdrDistTableThreadArray2               adr_dist_table_thread_;

  std::array<uint16_t, 2>                          lts_in_gress_ {};
  std::array<uint8_t, 2>                           cntrs_in_gress_ {};

  std::array<int, kNumStatefulCounters>            word_portion_width_ {};
  std::array<int, kNumStatefulCounters>            extra_bit_mask_ {};
  std::array<int, kNumStatefulCounters>            no_extra_bit_mask_ {};
  std::array<uint32_t, kNumStatefulCounters>       word_portion_mask_ {};
  std::array<AddrUnpadded, kNumStatefulCounters>   counter_output_ {};

  std::array<bool, kLogicalTables>                 vpn_in_range_ {};
  std::array<uint64_t, kNumStatefulCounters>       T_clear_next_ {};
  std::array<uint8_t, kNumStatefulCounters>        cntr_flags_ {};
  std::array<uint32_t, kNumAlus>                   final_addrs_ {};

}; // MauStatefulCounters

}

#endif // _JBAY_SHARED_MAU_COUNTERS_
