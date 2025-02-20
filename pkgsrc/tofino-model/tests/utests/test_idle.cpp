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

#include <utests/test_util.h>
#include <iostream>
#include <string>
#include <ctime>
#include <chrono>
#include <array>
#include <cassert>

#include "gtest.h"

#include <bitvector.h>
#include <rmt-object-manager.h>
#include <model_core/model.h>
#include <mau.h>
#include <packet.h>
#include <rmt-sweeper.h>
#include <model_core/rmt-dru-callback.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

// 2b idletime mode invalid on JBay
#ifdef MODEL_CHIP_JBAY_OR_LATER
#define ALLOWED_IDLE_WIDTHS 0xD
#else
#define ALLOWED_IDLE_WIDTHS 0xF
#endif

bool idle_print = false;

using namespace std;
using namespace MODEL_CHIP_NAMESPACE;


void idle_update_callback_print(int asic, uint8_t *idle_timeout_data, int len) {
  uint64_t msg = *(uint64_t*)idle_timeout_data;
  int pipe = (msg >> (56+TestUtil::kStageBits)) & 0x3;
  int stage = (msg >> 56) & TestUtil::kStageMask;
  int logtab = (msg >> 52) & 0xF;
  int vpn = (msg >> 46) & 0x3F;
  int index = (msg >> 36) & 0x3FF;
  int ia = (msg >> 4) & 0xFF;
  printf("IdleUpdateCallback(chip=%d,pipe=%d,stage=%d,lt=%d,vpn=%d,index=%d AI=0x%02x)\n",
         asic, pipe, stage, logtab, vpn, index, ia);
}


// Helper class for Idle unit testing
class Idler {

  // Some funcs to get random nums from rand
  static inline int randval() { return rand(); }
  static inline int randval(int max) { return randval() % max; }
  static inline int randval(int min, int max)  {
    return (min < max) ?(randval(max-min) + min) :(randval(min-max) + max);
  }

  // Event types
  static const char *kEventNameTab[];
  static constexpr int kEventMin = 1;
  static constexpr int kEventHit = 1;
  static constexpr int kEventActiveCallback = 2;
  static constexpr int kEventIdleCallback = 3;
  static constexpr int kEventMax = 3;
  // Some FSM errors
  static constexpr int kStateErrorDisabledActive   = 0x001;
  static constexpr int kStateErrorUnexpectedActive = 0x002;
  static constexpr int kStateErrorLateActive       = 0x004;
  static constexpr int kStateErrorMissingActive    = 0x008;
  static constexpr int kStateErrorDisabledIdle     = 0x010;
  static constexpr int kStateErrorUnexpectedIdle   = 0x020;
  static constexpr int kStateErrorLateIdle         = 0x040;
  static constexpr int kStateErrorMissingIdle      = 0x080;
  static constexpr int kExtErrorMissingLock        = 0x100;
  static constexpr int kExtErrorMissingUnlock      = 0x200;
  static constexpr int kExtErrorMissingOp          = 0x400;
  static constexpr int kExtErrorMissingDump        = 0x800;
  // Composite error masks
  static constexpr int kErrorLateMask              = 0x044;
  static constexpr int kErrorBasicMask             = 0x033;
  static constexpr int kErrorMissingMask           = 0x388;
  static constexpr int kErrorRelaxedMask           = kErrorBasicMask|kErrorMissingMask;
  static constexpr int kErrorStrictMask            = 0xFFF;


  // Preserve per-entry FSM in MAU SRAM along with idle_addr to use etc
  static constexpr int kStateZero                      = 0;
  static constexpr int kStateMin                       = 1;
  static constexpr int kStateDisabled                  = 1;
  static constexpr int kStateIdle                      = 2;
  static constexpr int kStateHitWaitingActive          = 3;
  static constexpr int kStateHitWaitingIdle            = 4;
  static constexpr int kStateMax                       = 4;
  static inline int state_get_fsm(uint64_t s)          { return ((s >>  0) & 0xF); }
  static inline int state_get_errors(uint64_t s)       { return ((s >>  4) & 0xFF); }
  static inline int state_get_idle_addr(uint64_t s)    { return ((s >> 12) & 0xFFFFF); }
  static inline int state_get_mapram_flags(uint64_t s) { return  (s >> 32); }
  static inline uint64_t state_make(int flags, int addr, int errors, int fsm) {
    uint32_t lo = ( ((addr & 0xFFFFF) << 12) | ((errors & 0xFF) << 4) | ((fsm & 0xF) << 0) );
    return (static_cast<uint64_t>(flags) << 32) | (static_cast<uint64_t>(lo));
  }

  static constexpr int kItersDflt = 128;
  static constexpr int kItersMin = 16;
  static constexpr int kItersMax = 1024*1024;
  static constexpr int kEntriesDflt = 128;
  static constexpr int kEntriesMin = 16;
  static constexpr int kEntriesMax = 1024;
  static constexpr int kIntervalAbsMin = 0;
  static constexpr int kIntervalAbsMax = 11;
  static constexpr int kBitWidthMin = 0;
  static constexpr int kBitWidthMax = 3;

  static constexpr int kPipeBits = 1;           // Pipe 0 or 1
  static constexpr int kStageBits = 1;          // Stage 0 or 1
  static constexpr int kRowBits = 3;            // Any row in [0..7]
  static constexpr int kTableBits = kRowBits;   // Only 8 tables - 1 per row
  static constexpr int kColBase = 2;            // So we can avoid col0 col1
  static constexpr int kColBits = 2;            // Col kColBase+ 0,1,2,3 => upto 4096 entries per tab
  static constexpr int kVpnBits = kColBits;     // 2 col bits split between VPN/Subword
  static constexpr int kSubwordBits = kColBits; // 2 col bits split between VPN/Subword
  static constexpr int kEntryBits = 10;         // 1024 entries max - can be configured smaller

  static constexpr int kPipeMask = (1<<kPipeBits)-1;
  static constexpr int kStageMask = (1<<kStageBits)-1;
  static constexpr int kRowMask = (1<<kRowBits)-1;
  static constexpr int kTableMask = kRowMask;
  static constexpr int kColMask = (1<<kColBits)-1;
  static constexpr int kVpnMask = kColMask;
  static constexpr int kSubwordMask = kColMask;
  static constexpr int kEntryMask = (1<<kEntryBits)-1;

  static constexpr int kEntryShift = 0;
  static constexpr int kColShift = kEntryShift + kEntryBits;
  static constexpr int kSubwordShift = kColShift;
  static constexpr int kVpnShift = kColShift;
  static constexpr int kRowShift = kColShift + kColBits;
  static constexpr int kTableShift = kRowShift;
  static constexpr int kStageShift = kRowShift + kRowBits;
  static constexpr int kPipeShift = kStageShift + kStageBits;

  // Funcs to access pipe/stage/row/col/entry
  static inline int pipe(int id)    { return (id >> kPipeShift) & kPipeMask; }
  static inline int stage(int id)   { return (id >> kStageShift) & kStageMask; }
  static inline int row(int id)     { return (id >> kRowShift) & kRowMask; }
  static inline int table(int id)   { return (id >> kTableShift) & kTableMask; }
  static inline int col(int id)     { return (id >> kColShift) & kColMask; }
  static inline int vpn(int id)     { return (id >> kVpnShift) & kVpnMask; }
  static inline int subword(int id) { return (id >> kSubwordShift) & kSubwordMask; }
  static inline int entry(int id)   { return (id >> kEntryShift) & kEntryMask; }
  // Make an id from pipe/stage/row/col/entry
  static inline int make_id(int p, int s, int r, int c, int e) {
    int val = 0;
    val |= ((e & kEntryMask) << kEntryShift);
    val |= ((c & kColMask) << kColShift);
    val |= ((r & kRowMask) << kRowShift);
    val |= ((s & kStageMask) << kStageShift);
    val |= ((p & kPipeMask) << kPipeShift);
    return val;
  }

  // Funcs to get idletime width, nents etc
  static inline int idletime_width(int bitwidth) { // 1,2,3,6
    switch (bitwidth & 0x3) {
    case 0: return 1;
    case 1: return 2;
    case 2: return 3;
    case 3: return 6;
    default: assert(0);
    }
  }
  static inline int idletime_nentries(int bitwidth) { // 8,4,2,1
    switch (bitwidth & 0x3) {
    case 0: return 8;
    case 1: return 4;
    case 2: return 2;
    case 3: return 1;
    default: assert(0);
    }
  }
  static inline int idletime_offset(int bitwidth, int subword) {
    assert((subword >= 0) && (subword < idletime_nentries(bitwidth)));
    return subword * idletime_width(bitwidth);
  }


 public:
  Idler(TestUtil *tu, uint64_t seed,
        int n_iters, int n_entries, int n_active_entries,
        int min_intvl, int max_intvl,
        uint8_t allowed_idle_width_masks,
        bool allow_2way, bool allow_pfe, bool force_2way, bool debug) {
    assert(tu != NULL);
    tu_ = tu;
    phv_ = tu->phv_alloc();
    T_ = UINT64_C(0);
    // Bound n_iters
    n_iters_ = ((n_iters >= kItersMin) &&
                (n_iters <= kItersMax)) ?n_iters :kItersDflt;
    // Bound n_entries and round up to power of 2
    n_entries_ = ((n_entries >= kEntriesMin) &&
                  (n_entries <= kEntriesMax)) ?n_entries :kEntriesDflt;
    entry_bits_ = 0;
    while ((1<<entry_bits_) < n_entries_) entry_bits_++;
    n_entries_ = 1<<entry_bits_;
    assert(entry_bits_ <= kEntryBits);
    id_bits_ = kPipeBits + kStageBits + kRowBits + kColBits + entry_bits_;
    // Active entries must be <= n_entries (use -1 to let system decide)
    n_active_entries_ = ((n_active_entries >= 0) &&
                         (n_active_entries <= n_entries_)) ?n_active_entries :n_entries_/2;
    printf("IdlerINIT: n_iters=%d n_entries=%d n_active_entries=%d\n",
           n_iters_, n_entries_, n_active_entries_);
    // Make non-zero seed if 0 passed
    seed_ = (seed != UINT64_C(0)) ?seed :static_cast<uint64_t>(99999997 * n_iters * n_entries);
    srand(static_cast<int>((seed >> 32) ^ (seed_ & 0xFFFFFFFF)));
    // Ensure min interval less than max then apply bounds
    if (min_intvl > max_intvl) {
      int tmp = min_intvl; min_intvl = max_intvl; max_intvl = tmp;
    }
    min_intvl_ = (min_intvl < kIntervalAbsMin) ?kIntervalAbsMin :min_intvl;
    max_intvl_ = (max_intvl > kIntervalAbsMax) ?kIntervalAbsMax :max_intvl;
    // 0x1 => 1-bit mode, 0x2 => 2-bit mode, 0x4 => 3-bit mode, 0x8 => 6-bit mode
    allowed_idle_width_masks_ = allowed_idle_width_masks & 0xF;
    // If 0 passed allow all bitwidths
    if (allowed_idle_width_masks_ == 0) allowed_idle_width_masks_ = 0xF;
    allow_2way_ = (force_2way) ?true :allow_2way;
    allow_pfe_ = allow_pfe;
    force_2way_ = force_2way;
    callbacks_ = false;
    debug_ = debug;
    lock_debug_ = debug;
    dump_debug_ = debug;
    dump_cnt_ = 0;
    lock_cnt_ = 0;
    unlock_cnt_ = 0;
    pending_cnt_ = 0;
    for (int p = 0; p <= kPipeMask; p++) {
      for (int s = 0; s <= kStageMask; s++) {
        for (int t = 0; t <= kTableMask; t++) {
          table_locks_[p][s][t] = false;
          pending_lock_unlock_[p][s][t] = false;
        }
      }
    }
  }
  ~Idler() { }

 private:
  int get_table_sweep_interval(int p, int s, int lt) {
    // Deterministic based on seed/pipe/stage/table
    int baseval = (123457 * seed_ * (p+3) * (s+5) * (lt+7)) >> 3;
    if (baseval < 0) baseval = -baseval;
    return (baseval % (1 + max_intvl_ - min_intvl_)) + min_intvl_;
  }
  int get_mapram_flags(int p, int s, int lt, int bit_width) {
    // Deterministic based on seed/pipe/stage/table and config
    int baseval = (987659 * seed_ * (p+3) * (s+5) * (lt+7)) >> 5;
    if (baseval < 0) baseval = -baseval;
    int mask = TestUtil::kIdleFlagUnused; // So mask never 0
    baseval |= mask; // So returned mask never 0
    // If force_2way_ set then allow_2way_ will also be set
    if (force_2way_) baseval |= TestUtil::kIdleFlagTwoWay;
    if (bit_width == 1) {
      // bit_width 1=>2b mode
      if (allow_2way_) mask |= TestUtil::kIdleFlagTwoWay;
    } else if ((bit_width == 2) || (bit_width == 3)) {
      // bit_width 2=>3b mode, 3=>6b mode
      if (allow_2way_)  mask |= TestUtil::kIdleFlagTwoWay;
      if (allow_pfe_)  mask |= TestUtil::kIdleFlagPerFlow;
    }
    return baseval & mask;
  }
  // Returns 0=>1-bit mode, 1=>2-bit mode, 2=>3-bit mode, 3=>6-bit mode
  int get_idle_cfg(int sweep_intvl) {
    // sweep_intvl is in [0,11] so subtract from 11 and divide by 3 to get val in [3,0]
    int bw = (kIntervalAbsMax - sweep_intvl) / 3;
    // Tick up from bw (in [0,3]) till we find bit set in allowed_idle_width_masks_
    for (int i = bw; i <= kBitWidthMax; i++) {
      if ((allowed_idle_width_masks_ & (1<<i)) != 0) return i;
    }
    // Else tick down
    for (int i = bw; i >= kBitWidthMin; i--) {
      if ((allowed_idle_width_masks_ & (1<<i)) != 0) return i;
    }
    // Should result in hi-freq table using big bitwidths, low-freq table using small
    assert(0);
    return -1;
  }
  void get_mapram_cfg(int p, int s, int r, int *bitwidth, int *nmaprams, int *flags) {
    // NB r === lt and r == idle_bus for this test rig
    int lt = r;
    int half_way_row = (1<<kRowBits)/2;
    // Buses >=10 in top, <10 in bot, so use 0,1,2,3 in bot and 14,15,16,17 in top
    int idle_bus = (r < half_way_row) ?r :r+10;
    int sweep_intvl = get_table_sweep_interval(p, s, lt);
    int bit_width = get_idle_cfg(sweep_intvl); // 0-3
    // 1b=>1 mapram, 2b=>1 mapram, 3b=>2 maprams, 6b=>4 maprams
    int n_maprams = 0;
    // Use 1,2 or 4 maprams based on bit_width
    switch (bit_width) {
      case 0: n_maprams = 1; break;
      case 1: n_maprams = 1; break;
      case 2: n_maprams = 2; break;
      case 3: n_maprams = 4; break;
      default: assert(0);
    }
    int idle_flags = get_mapram_flags(p, s, r, bit_width);
    int m_flags = TestUtil::idle_flags_make(idle_flags, sweep_intvl, idle_bus, bit_width);
    *bitwidth = bit_width;
    *nmaprams = n_maprams;
    *flags = m_flags;
  }
  uint64_t get_idle_disabled_val(int bit_width) {
    switch (bit_width) {
      case 0: return UINT64_C(0xFF);
      case 1: return UINT64_C(0xFF); // No disable in 2b mode (we prefer 2way to PFE)
      case 2: return static_cast<uint64_t>((7<<3)|(7<<0));
      case 3: return UINT64_C(63);
      default: assert(0); return UINT64_C(0xFFFFFFFFFFFFFFFF);
    }
  }
  uint64_t get_idle_idle_val(int bit_width, bool pfe) {
    switch (bit_width) {
      case 0: return UINT64_C(0);
      case 1: return UINT64_C(0xFF);
      case 2: return (pfe) ?static_cast<uint64_t>((6<<3)|(6<<0)) :static_cast<uint64_t>((7<<3)|(7<<0));
      case 3: return (pfe) ?UINT64_C(62) :UINT64_C(63);
      default: assert(0); return UINT64_C(0xFFFFFFFFFFFFFFF);;
    }
  }
  int get_idle_n_sweeps(int bit_width, bool pfe) {
    switch (bit_width) {
      case 0: return 0;
      case 1: return 3;
      case 2: return (pfe) ? 6 : 7;
      case 3: return (pfe) ?62 :63;
      default: assert(0); return -1;
    }
  }
  int generate_id() {
    // Generate an id using the calculated id bits but then
    // transform into a p/s/r/c/i format id with fixed id bits
    // by opening up a 'hole' if necessary
    int entry_mask = (1<<entry_bits_)-1;
    int id_mask = (1<<id_bits_)-1;
    int id = randval(id_mask+1);
    return ((id & ~entry_mask) << (kEntryBits - entry_bits_)) | (id & entry_mask);
  }
  void check_state(int p, int s, int r, int c, int i, uint64_t t, uint64_t state) {
    // Use MAU sram to store state
    int flags = state_get_mapram_flags(state);
    //int idle_addr = state_get_idle_addr(state);
    //int errors = state_get_errors(state);
    //int idle_bus = TestUtil::idle_flags_idlebus(flags);
    int fsm = state_get_fsm(state);
    int sweep_intvl = TestUtil::idle_flags_interval(flags);
    int bit_width = TestUtil::idle_flags_bitwidth(flags);
    if (flags == 0) assert(0);
    if ((fsm < kStateMin) || (fsm > kStateMax)) assert(0);
    if (i >= n_entries_) assert(0);
    if ((sweep_intvl < kIntervalAbsMin) || (sweep_intvl > kIntervalAbsMax)) assert(0);
    if ((bit_width < kBitWidthMin) || (bit_width > kBitWidthMax)) assert(0);
  }
  void set_state(int p, int s, int r, int c, int i, uint64_t t, uint64_t state, bool dump) {
    // Use MAU sram to store state
    if (debug_ && dump)
      printf("%15" PRId64 " set_state(p=%d,s=%d,r=%d,c=%d,i=%d,T=%" PRId64 ",State=0x%016" PRIx64 ")\n",
             T_, p, s, r, c, i, t, state);
    check_state(p, s, r, c, i, t, state);
    tu_->sram_write(p, s, r, kColBase+c, i, t, state); // Avoid srams in col0/1
  }
  int set_state_init(int p, int s, int r, int c, int i,
                      int bit_width, int mapram_flags, int fsm0) {
    int idle_addr = 0;
    assert((c >= 0) && (c <= 3));
    // column var, c determines idle_addr vpn/subword based on bit_width
    switch (bit_width) {
      case 0: idle_addr =      (0 << 14) | (i << 4) |       (c << 1) | 0x0; break;
      case 1: idle_addr =      (0 << 14) | (i << 4) |       (c << 2) | 0x1; break;
      case 2: idle_addr = ((c>>1) << 14) | (i << 4) | ((c&0x1) << 3) | 0x3; break;
      case 3: idle_addr =      (c << 14) | (i << 4) |            (0) | 0x7; break;
      default: assert(0); break;
    }
    if (mapram_flags == 0) assert(0);
    uint64_t state0 = state_make(mapram_flags, idle_addr, 0, fsm0);
    set_state(p, s, r, c, i, T_, state0, (i==0) ); // Use initial time val
    return idle_addr;
  }
  void get_state(int p, int s, int r, int c, int i, uint64_t *t, uint64_t *state, bool dump) {
    tu_->sram_read(p, s, r, kColBase+c, i, t, state); // Avoid srams in col0/1
    if (debug_ && dump)
      printf("%15" PRId64 " get_state(p=%d,s=%d,r=%d,c=%d,i=%d,T=%" PRId64 ",State=0x%016" PRIx64 ")\n",
             T_, p, s, r, c, i, *t, *state);
    check_state(p, s, r, c, i, *t, *state);
  }
  void mapram_read_sub(int p, int s, int r, int c, int i, int off, int nbits, uint16_t *data) {
    uint64_t data0, data1;
    tu_->mapram_read(p, s, r, c, i, &data0, &data1);
    *data = static_cast<uint16_t>((data0 >> off) & ((UINT64_C(1) << nbits) - UINT64_C(1)));
  }
  void mapram_read(int p, int s, int r, int c, int i, int sub, int bitwidth, uint16_t *data) {
    int off = idletime_offset(bitwidth, sub);
    int nbits = idletime_width(bitwidth);
    mapram_read_sub(p, s, r, c, i, off, nbits, data);
  }
  void mapram_read(int p, int s, int r, int c, int i, uint16_t *data) {
    mapram_read_sub(p, s, r, c, i, 0, 8, data);
  }

 public:
  void init() {
    // Now we go initialise all pipe/stages/rows
    // We configure a single table on each row so only 8
    // We use 4 SRAMs per-row with VPNs 0,1,2,3
    // We use upto 4 MapRAMs per-row with VPNs 0,1,2,3
    // We configure MapRAMs to use table/ibus corresponding to row number
    //    and to have bitwidth determined by sweep freq of tables
    for (int p = 0; p <= kPipeMask; p++) {
      for (int s = 0; s <= kStageMask; s++) {
        tu_->set_dump_ctl_regs(p, s); // Setup stage_dump_ctl regs
        for (int r = 0; r <= kRowMask; r++) {
          int lt = r;  // NB r === lt for purposes of this test
          int bit_width, n_maprams, mapram_flags;
          get_mapram_cfg(p, s, r, &bit_width, &n_maprams, &mapram_flags);
          bool pfe = ((mapram_flags & TestUtil::kIdleFlagPerFlow) != 0);
	  if (debug_)
	    printf("Table[%d,%d,%d] Interval=%d BitWidth=%d Flags=0x%08x nMaprams=%d\n", p, s, lt,
		   TestUtil::idle_flags_interval(mapram_flags),
		   TestUtil::idle_flags_bitwidth(mapram_flags),
		   mapram_flags, n_maprams);
          // If NO per-flow-enable ALL entries will be active
          int n_active = (pfe) ?n_active_entries_ :n_entries_;
          for (int c = 0; c <= kColMask; c++) {
            // We only configure maprams we plan to use
            int vpn = c; // NB c === vpn for purposes of this test
            if (c < n_maprams) {
              tu_->mapram_config(p, s, r, c+6, TestUtil::kMapramTypeIdletime,
                                 vpn, vpn, mapram_flags, lt, false);
            }
            // But write initialization vals to all maprams irrespective
            // of whether we'll be using them
            // NB SRAM x uses MapRAM x+6
            uint64_t idle_val = get_idle_idle_val(bit_width, pfe);
            uint64_t disabled_val = get_idle_disabled_val(bit_width);
            for (int i = 0; i < n_active; i++) {
              (void)set_state_init(p, s, r, c, i,
                                   bit_width, mapram_flags, kStateIdle);
              tu_->mapram_write(p, s, r, c+6, i, idle_val, idle_val);
            }
            // If NO per-flow-enable ALL entries will be active so
            // we won't be writing any disabled SRAM/MapRAM entries here...
            for (int i = n_active; i < n_entries_; i++) {
              int idle_addr = set_state_init(p, s, r, c, i,
                                             bit_width, mapram_flags, kStateDisabled);
              tu_->mapram_write(p, s, r, c+6, i, disabled_val, disabled_val);
              // Test out these disabled entries by doing a quick sweep
              // Nothing should happen - NO callback should occur
              if (c < n_maprams) do_sweep(p, s, lt, idle_addr);
            }
            // ...except for entries > n_entries_ which are ALWAYS disabled
            for (int i = n_entries_; i < TestUtil::kMapramMaxEntries; i++) {
              tu_->mapram_write(p, s, r, c+6, i, disabled_val, disabled_val);
            }
          }
        }
      }
    }
  }
  int check() {
    printf("\nCHECKING...\n");
    // Return accumulate error bitmask
    int accum_errors = 0;
    for (int p = 0; p <= kPipeMask; p++) {
      for (int s = 0; s <= kStageMask; s++) {
        for (int r = 0; r <= kRowMask; r++) {
          for (int c = 0; c <= kColMask; c++) {
            for (int i = 0; i < n_entries_; i++) {
	      if ((i % 100) == 0) { printf("."); fflush(stdout); }
              uint64_t t, state;
              get_state(p, s, r, c, i, &t, &state, false);
              int flags = state_get_mapram_flags(state);
              //int idle_addr = state_get_idle_addr(state);
              int state_errors = state_get_errors(state);
              int fsm = state_get_fsm(state);
	      int bit_width = TestUtil::idle_flags_bitwidth(flags);
	      if ((fsm != kStateIdle) && (bit_width == 0)) {
		// Special case for 1b idletimes - check mapram says hit
		// No callbacks in this case
		uint16_t val;
		mapram_read(p, s, r, 6, i, c, bit_width, &val);
		if (val == 1) fsm = kStateIdle;
	      }
              accum_errors |= state_errors;
              switch (fsm) {
                case kStateHitWaitingActive:
                  accum_errors |= kStateErrorMissingActive;
                  if (debug_)
                    printf("%15" PRId64 " MissingActive: %" PRId64 " psecs late (expectedT=%" PRId64 ")\n",
			 T_, (T_ - t), t);
                  break;
                case kStateHitWaitingIdle:
                  accum_errors |= kStateErrorMissingIdle;
                  break;
              }
              // Don't write back missing errors
              //state = state_make(flags, idle_addr, state_errors, fsm);
              //set_state(p, s, r, c, i, t, state, false);

              if (state_errors != 0) {
                if (debug_) {
		  printf("ERR(p=%d,s=%d,r=%d,c=%d,i=%d ERRFLAGS=0x%02x)"
                         " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",
                         p, s, r, c, i, state_errors);
                } else {
                  printf("!");
		}
              }
            }
          }
        }
      }
    }
    if (lock_cnt_ > 0)    accum_errors |= kExtErrorMissingLock;
    if (unlock_cnt_ > 0)  accum_errors |= kExtErrorMissingUnlock;
    if (pending_cnt_ > 0) accum_errors |= kExtErrorMissingOp;
    if (dump_cnt_ > 0)    accum_errors |= kExtErrorMissingDump;
    return accum_errors;
  }

 private:
  void do_assert(int x) {
    assert(0);
  }
  void do_op(int p, int s, int lt, int idle_addr, int op) {
    RmtObjectManager *om = tu_->get_objmgr();
    assert(om != NULL);
    Mau *mau = om->mau_lookup(p, s);
    ASSERT_TRUE(mau != NULL);
    MauAddrDist *adist = mau->mau_addr_dist();
    ASSERT_TRUE(adist != NULL);
    mau->reset_resources();
    // OR op at MSB of idle addr (enable bit is also set)
    adist->distrib_idletime_addresses(lt, true, op|0x100000|idle_addr);
    // Execute backend code
    Eop eop;
    mau->process_for_tests(phv_, eop);
  }
  void do_hit(int p, int s, int lt, int idle_addr) {
    do_op(p, s, lt, idle_addr, 0x1 << 20);
  }
  void do_movereg(int p, int s, int lt, int idle_addr) {
    if (idle_addr < 0) {
      int instr_pop_table_move_reg  = 0x1e00000|((lt & 0xF)<<0)|(1<<5)|(1<<4);
      uint64_t pop_addr = TestUtil::make_instruction_address(p,s,0,instr_pop_table_move_reg);
      tu_->IndirectWrite(pop_addr, UINT64_C(0), UINT64_C(0));
    } else {
      int instr_push_table_move_reg = 0x8000000|((lt & 0xF)<<20)|(idle_addr & 0x7ffff);
      uint64_t push_addr = TestUtil::make_instruction_address(p,s,0,instr_push_table_move_reg);
      tu_->IndirectWrite(push_addr, UINT64_C(0), UINT64_C(0));
    }
  }
  void do_idle_dump(int p, int s, int lt, int idle_addr) {
    // Use PBUS instruction write
    int instr_dump_idle_word = 0x1800000|((lt & 0xF)<<17)|(idle_addr & 0xFFFF);
    uint64_t addr = TestUtil::make_instruction_address(p,s,0,instr_dump_idle_word);
    dump_cnt_++; // Count outstanding DUMP requests
    if (dump_debug_)
      printf("IDLE_DUMP(p=%d,s=%d,lt=%d,idle_addr=0x%08x)  [DumpCnt=%d]\n",
             p, s, lt, idle_addr, dump_cnt_);
    tu_->IndirectWrite(addr, UINT64_C(0), UINT64_C(0));
  }
  void do_barrier_lock(int p, int s, int lt, int lock_type, uint16_t lock_id) {
    // Use PBUS instruction write
    int instr_barrier_lock = 0x0800000|((lt & 0xF)<<19);
    instr_barrier_lock |= ((lt & 0xF)<<19);
    instr_barrier_lock |= ((lock_type & 0x7)<<16);
    instr_barrier_lock |= ((lock_id & 0xFFFF)<<0);
    uint64_t addr = TestUtil::make_instruction_address(p,s,0,instr_barrier_lock);
    tu_->IndirectWrite(addr, UINT64_C(0), UINT64_C(0));
  }
  void do_idle_lock(int p, int s, int lt, uint16_t lock_id) {
    // If already locked or op in flight bail
    if (table_locks_[p][s][lt] || pending_lock_unlock_[p][s][lt]) return;
    pending_lock_unlock_[p][s][lt] = true;
    pending_cnt_++;
    // Reserve lock_id = 0xFFFF to mean unlock
    if (lock_id == 0xFFFF) lock_id = 0;
    lock_cnt_++; // Count outstanding lock requests
    if (lock_debug_) printf("IDLE_LOCK(%d,%d,%d,LockId=%d) [LockCnt=%d]\n", p,s,lt,lock_id,lock_cnt_);
    do_barrier_lock(p, s, lt, 6, lock_id);
  }
  void do_idle_unlock(int p, int s, int lt) {
    // If already unlocked or op in flight bail
    if (!table_locks_[p][s][lt] || pending_lock_unlock_[p][s][lt]) return;
    pending_lock_unlock_[p][s][lt] = true;
    pending_cnt_++;
    unlock_cnt_++; // Count outstanding unlock requests
    if (lock_debug_) printf("IDLE_UNLOCK(%d,%d,%d) [UnlockCnt=%d]\n", p,s,lt,unlock_cnt_);
    do_barrier_lock(p, s, lt, 7, 0xFFFF);
  }
  void do_idle_unlock_all() {
    for (int p = 0; p <= kPipeMask; p++) {
      for (int s = 0; s <= kStageMask; s++) {
        for (int t = 0; t <= kTableMask; t++) {
          if (table_locks_[p][s][t]) do_idle_unlock(p,s,t);
        }
      }
    }
  }
  void do_sweep(int p, int s, int lt, int idle_addr) {
    do_op(p, s, lt, idle_addr, 0x5 << 20);
  }
  void handle_event(int p, int s, int r, int c, int i, int event_type) {
    uint64_t T_now = T_; // Stash global time
    assert((event_type >= kEventMin) && (event_type <= kEventMax));
    // If we see an Active/Idle we should not be locked
    if (event_type != kEventHit) assert(!table_locks_[p][s][r]);
    int bit_width, n_sweeps, sweep_intvl;
    uint64_t state, T_state_start, T_state_end, T_expected;
    // Lookup state
    get_state(p, s, r, c, i, &T_state_start, &state, true);
    T_state_end = T_state_start; // Keep T_state the same by default
    int flags = state_get_mapram_flags(state);
    int idle_addr = state_get_idle_addr(state);
    int errors = state_get_errors(state);
    int fsm_start = state_get_fsm(state);
    int fsm_end = fsm_start; // Keep fsm same by default
    if (debug_)
      printf("%15" PRId64 " handle_event(in, p=%d,s=%d,r=%d,c=%d,i=%d,FSM=%d,EV=%s)\n",
	     T_now, p, s, r, c, i, fsm_start, kEventNameTab[event_type]);

    switch (fsm_start) {
      case kStateDisabled:
        switch (event_type) {
          // Test rig may HIT disabled entries but there should be no callback
          case kEventHit: break;
          case kEventActiveCallback: errors |= kStateErrorDisabledActive; break;
          case kEventIdleCallback:   errors |= kStateErrorDisabledIdle;   break;
          default: assert(0); break;
        }
        if (event_type == kEventHit) {
          //printf("Just HIT on a disabled entry\n");
        } else {
          printf("Got %d callback on disabled entry... WHY?\n", event_type);
        }
        break;
      case kStateIdle:
        switch (event_type) {
          case kEventHit:
            T_state_end = T_now; // Update T in state
            if ((flags & TestUtil::kIdleFlagTwoWay) != 0)
              fsm_end = kStateHitWaitingActive;
            else
              fsm_end = kStateHitWaitingIdle;
            break;
          case kEventActiveCallback: errors |= kStateErrorUnexpectedActive; break;
          case kEventIdleCallback:
            errors |= kStateErrorUnexpectedIdle;
	    if (debug_)
	      printf("%15" PRId64 " UNEXPECTED_IDLE(p=%d,s=%d,r=%d,c=%d,i=%d,EV=%s) WasAlreadyIDLE\n",
		     T_now, p, s, r, c, i, kEventNameTab[event_type]);
            break;
          default: assert(0); break;
        }
        break;
      case kStateHitWaitingActive:
        switch (event_type) {
          case kEventHit:
            T_state_end = T_now; // Hit again - update T in state
            break;
          case kEventActiveCallback:
            // WAS expecting Active Callback - check it happened within right time
            // T_now should be <= T in state + sweep interval
            sweep_intvl = TestUtil::idle_flags_interval(flags);
            T_expected = T_state_start + RmtSweeper::interval_to_psecs(sweep_intvl);
            assert(T_now >= T_state_start);
            if (T_now > T_expected) {
              if (T_now - T_expected > RmtSweeper::interval_to_psecs(0)) {
                errors |= kStateErrorLateActive;
		if (debug_)
		  printf("%15" PRId64 " LateActive: %" PRId64 " psecs late (expectedT=%" PRId64 ")\n",
			 T_now, (T_now - T_expected), T_expected);
              }
            }
            fsm_end = kStateHitWaitingIdle;
            break;
          case kEventIdleCallback:
            // Wasn't expecting Idle Callback - store an error
            errors |= kStateErrorUnexpectedIdle;
	    if (debug_)
	      printf("%15" PRId64 " UNEXPECTED_IDLE(p=%d,s=%d,r=%d,c=%d,i=%d,EV=%s) - WasWaitingACTIVE\n",
		     T_now, p, s, r, c, i, kEventNameTab[event_type]);
            break;
          default: assert(0); break;
        }
        break;
      case kStateHitWaitingIdle:
        switch (event_type) {
          case kEventHit:
            T_state_end = T_now; // Hit again - update T in state
            break;
          case kEventActiveCallback:
            // Wasn't expecting Active Callback - store an error
            errors |= kStateErrorUnexpectedActive;
            break;
          case kEventIdleCallback:
            // WAS expecting Idle Callback - check it happened within right time
            // T_now should be <= T in state + (sweeps * sweep interval)
            bit_width = TestUtil::idle_flags_bitwidth(flags);
            n_sweeps = get_idle_n_sweeps(bit_width,
                                         ((flags & TestUtil::kIdleFlagPerFlow) != 0));
            sweep_intvl = TestUtil::idle_flags_interval(flags);
            T_expected = T_state_start +
                (n_sweeps * RmtSweeper::interval_to_psecs(sweep_intvl));
            assert(T_now >= T_state_start);
            if (T_now > T_expected) {
	      if (T_now - T_expected > RmtSweeper::interval_to_psecs(0)) {
		 if (debug_)
		   printf("%15" PRId64 " LateIdle: %" PRId64 " psecs late (expectedT=%" PRId64 ")\n",
			  T_now, (T_now - T_expected), T_expected);
                 errors |= kStateErrorLateIdle;
               }
             }
             fsm_end = kStateIdle;
             break;
           default: assert(0); break;
         }
         break;
       case kStateZero: break; // Hack for now
       default: assert(0); break;
     }
     state = state_make(flags, idle_addr, errors, fsm_end);
     if (debug_)
       printf("%15" PRId64 " handle_event(out,p=%d,s=%d,r=%d,c=%d,i=%d,FSM=%d,EV=%s)\n",
	      T_, p, s, r, c, i, fsm_end, kEventNameTab[event_type]);
     set_state(p, s, r, c, i, T_state_end, state, true);
     if (event_type == kEventHit) do_hit(p, s, r, idle_addr);
   }


  public:
  void callback_fsm(int p, int s, int lt, int vpn, int index,
                    uint8_t idle_active_flags) {
     int bit_width = get_idle_cfg(get_table_sweep_interval(p, s, lt));
     if ((index < 0) || (index >= n_entries_)) assert(0);
     assert(vpn >= 0);
     assert(bit_width != 0); // Should not see callbacks for 1b idletime
     if (debug_)
       printf("%15" PRId64 " callback(p=%d,s=%d,lt=%d,vpn=%d,i=%d IA=0x%02x)\n",
	      T_, p, s, lt, vpn, index, idle_active_flags);
     int n, shift;
     switch (bit_width) {
       case 0: n = 8; shift = 1; break;
       case 1: n = 4; shift = 2; break;
       case 2: n = 2; shift = 4; break; // NB shift 4 NOT 3!
       case 3: n = 1; shift = 6; break;
       default: assert(0);  break;
     }
     // Call out active/idle for each subword
     int row = lt; // Always true for this test
     int col = 99;
     int sub = 0;
     while (sub < n) {
       uint8_t ia = idle_active_flags & 0x3; // Active=0x2 Idle=0x1
       idle_active_flags >>= shift;
       if (ia != 0) {
	 switch (bit_width) {
	 case 0: col = sub; break;
	 case 1: col = sub; break;
	 case 2: col = ((vpn<<1)|(sub & 0x1)); break;
	 case 3: col = vpn; break;
	 default: assert(0); break;
	 }
	 if (debug_)
	   printf("%15" PRId64 " callback(p=%d,s=%d,lt=%d,vpn=%d,i=%d "
		  "IA=0x%02x,N=%d,sub=%d,ia=0x%02x,col=%d))\n",
		  T_, p, s, lt, vpn, index, idle_active_flags, n, sub, ia, col);
         //int col = (c >= 0) ?c :((vpn<<1)|(sub & 0x1));
         if ((ia & 0x2) != 0) handle_event(p, s, row, col, index, kEventActiveCallback);
         if ((ia & 0x1) != 0) handle_event(p, s, row, col, index, kEventIdleCallback);
       }
       sub++;
     }
   }
  void callback_lock_ack(int p, int s, int lt, uint64_t msg) {
    uint16_t lock_id = static_cast<uint16_t>((msg >> 36) & 0xFFFFF);
    // Should think there is an op in flight for this table
    assert(pending_lock_unlock_[p][s][lt]);
    assert(pending_cnt_ >= 0);
    if (lock_id == 0xFFFF) {
      if (lock_debug_)
        printf("IDLE_UNLOCK_ACK(%d,%d,%d,UNLOCK) [UnlockCnt=%d]\n", p,s,lt,unlock_cnt_);
      // This means unlock so table should be locked at moment - check then unlock
      assert(table_locks_[p][s][lt]);
      unlock_cnt_--;
      assert(unlock_cnt_ >= 0);
      table_locks_[p][s][lt] = false;
    } else {
      if (lock_debug_)
        printf("IDLE_LOCK_ACK(%d,%d,%d,LockId=%d) [LockCnt=%d]\n", p,s,lt,lock_id,lock_cnt_);
      // This means lock so table should be unlocked at moment - check then lock
      assert(!table_locks_[p][s][lt]);
      lock_cnt_--;
      assert(lock_cnt_ >= 0);
      table_locks_[p][s][lt] = true;
    }
    pending_cnt_--;
    pending_lock_unlock_[p][s][lt] = false;
  }
  void callback_dump(int p, int s, int lt, int vpn, int index) {
    dump_cnt_--;
    assert(dump_cnt_ >= 0);
    if (dump_debug_)
      printf("IDLE_DUMP_CB(p=%d,s=%d,lt=%d,vpn=%d,i=%d)  [DumpCnt=%d]\n",
             p, s, lt, vpn, index, dump_cnt_);
  }

  void callback_raw(int asic, uint8_t *idle_timeout_data, int len) {
     assert(callbacks_);
     uint64_t msg = *(uint64_t*)idle_timeout_data;
     int typ = (msg >> 0) & 0x3;
     int p = (msg >> (56+TestUtil::kStageBits)) & 0x3;
     int s = (msg >> 56) & TestUtil::kStageMask;
     int lt = (msg >> 52) & 0xF;
     int vpn = (msg >> 46) & 0x3F;
     int i = (msg >> 36) & 0x3FF;
     int idle_active_flags = (msg >> 4) & 0xFF;
     switch (typ) {
       case 0: callback_fsm(p, s, lt, vpn, i, idle_active_flags); break;
       case 1: callback_lock_ack(p, s, lt, msg); break;
       case 2: callback_dump(p, s, lt, vpn, i); break;
       default: assert(0); break;
     }
   }
   void idle_lock(int p, int s, int r, int lock_id) {
     do_idle_lock(p, s, r, lock_id);
   }
   void idle_unlock(int p, int s, int r) {
     do_idle_unlock(p, s, r);
   }
   void idle_lock_unlock(int p, int s, int r, int lock_id) {
     if (table_locks_[p][s][r])
       idle_unlock(p, s, r);
     else
       idle_lock(p, s, r, lock_id);
   }
  void hit(int p, int s, int r, int c, int i, bool allow_lock) {
     if ((i < 0) || (i >= n_entries_)) assert(0);
     handle_event(p, s, r, c, i, kEventHit);
     // On even tables, once in every 50 hits we lock/unlock
     if ((allow_lock) && ((r % 2) == 0) && (randval(0,50) == 0))
       idle_lock_unlock(p, s, r, (c+1)*(i+1));
   }
   void hit(int id, bool allow_lock) {
     hit(pipe(id), stage(id), row(id), col(id), entry(id), allow_lock);
   }
   void movereg(int p, int s, int r, int c, int i, bool pop=false) {
     int idle_addr = -1;
     if (!pop) {
       uint64_t state, T_tmp;
       get_state(p, s, r, c, i, &T_tmp, &state, true);
       idle_addr = state_get_idle_addr(state);
     }
     do_movereg(p, s, r, idle_addr); // r == lt
   }
   void idle_dump(int p, int s, int r, int c, int i) {
     uint64_t state, T_tmp;
     get_state(p, s, r, c, i, &T_tmp, &state, true);
     int idle_addr = state_get_idle_addr(state);
     do_idle_dump(p, s, r, idle_addr); // r == lt
   }
   void idle_dump(int id) {
     idle_dump(pipe(id), stage(id), row(id), col(id), entry(id));
   }
   void sweep(uint64_t T_delta) {
     RmtObjectManager *om = tu_->get_objmgr();
     assert(om != NULL);
     RmtSweeper *sweeper = om->sweeper_get();
     assert(sweeper != NULL);
     T_ += T_delta; // Move time on
     //printf("%15" PRId64 " sweep(T=%" PRId64 ")\n", T_, T_);
     sweeper->sweep(T_);
   }
   void mainloop(bool allow_lock) {
     // Generate hits for a while, sweeping as we go
     for (int i = 0; i < n_iters_; i++) {
       if (randval(0,10) == 0) idle_dump(generate_id());
       printf("*"); fflush(stdout);
       int lim = randval(5,10);
       for (int j = 0; j < lim; j++) hit(generate_id(), allow_lock);
       sweep(RmtSweeper::interval_to_psecs(0)); // Min delta
     }
   }
   void drain() {
      // Just sweep till all timers expired
     int n_sweeps = 1 + get_idle_n_sweeps(kBitWidthMax, false);
     int n_ticks = RmtSweeper::interval_to_ticks(max_intvl_);
     for (int i = 0; i < n_sweeps * n_ticks; i++) {
       if ((i % 100) == 0) { printf("@"); fflush(stdout); }
       sweep(RmtSweeper::interval_to_psecs(0)); // Min delta
     }
   }
   int test1() {
     int errors0, errors1, errors2;
     callbacks_ = true; // Allow callbacks now

     mainloop(false); // No locking
     drain();
     // No locking so should be no errors at all
     errors0 = check() & kErrorStrictMask;
     if (errors0 != 0) printf("Errors0 = 0x%04x\n", errors0);
     if (errors0 != 0) return errors0;
     // Uncomment below to disable lock/unlock testing
     //return errors0;

     mainloop(true); // Allow locking
     drain();
     // Some tables may still be locked so allow Late/Missing stuff
     // on first check
     errors1 = check() & kErrorBasicMask;
     if (errors1 != 0) printf("Errors1 = 0x%04x\n", errors1);
     if (errors1 != 0) return errors1;

     // Now unlock ALL tables
     do_idle_unlock_all();
     drain();
     // All tables should be unlocked but Active/Idle for tables
     // that were locked could have turned up late so allow Late
     // stuff but insist no Missing stuff no Unexpected stuff
     errors2 = check() & kErrorRelaxedMask;
     if (errors2 != 0) printf("Errors2 = 0x%04x\n", errors2);
     if (errors2 != 0) return errors2;

     // HACK to allow quick movereg test
     if (n_iters_ != n_entries_) return errors2;
     // Finish off with some moveregs
     int id0 = generate_id();
     int p = pipe(id0);
     int s = stage(id0);
     int r = row(id0);
     // Push addresses A,B,C
     int id1 = generate_id();
     movereg(p, s, r, col(id1), entry(id1));
     int id2 = generate_id();
     movereg(p, s, r, col(id2), entry(id2));
     int id3 = generate_id();
     movereg(p, s, r, col(id3), entry(id3));
     // Then pop
     movereg(p, s, r, -1, -1, true); // Pop
     movereg(p, s, r, -1, -1, true); // Pop
     return errors2;
   }


  private:
   TestUtil  *tu_;
   Phv       *phv_;
   uint64_t   seed_;
   uint64_t   T_;
   int        n_iters_;
   int        n_entries_;
   int        n_active_entries_;
   int        min_intvl_;
   int        max_intvl_;
   uint8_t    allowed_idle_width_masks_;
   bool       allow_2way_;
   bool       allow_pfe_;
   bool       force_2way_;
   bool       callbacks_;
   bool       debug_;
   bool       lock_debug_;
   bool       dump_debug_;
   int        entry_bits_;
   int        id_bits_;
   int        dump_cnt_;
   int        lock_cnt_;
   int        unlock_cnt_;
   int        pending_cnt_;
   std::array<std::array<std::array<bool,kTableMask+1>,kStageMask+1>,kPipeMask+1> table_locks_;
   std::array<std::array<std::array<bool,kTableMask+1>,kStageMask+1>,kPipeMask+1> pending_lock_unlock_;

 };

 // Can't define this inside class for some reason
 const char *Idler::kEventNameTab[] = { "ERR", "HIT", "ACTIVE", "IDLE" };
 Idler *GLOBAL_IDLER = NULL;
 void global_idler_callback(int asic, uint8_t *idle_timeout_data, int len) {
   if (GLOBAL_IDLER != NULL)
     GLOBAL_IDLER->callback_raw(asic, idle_timeout_data, len);
 }



   TEST(BFN_TEST_NAME(IdleTest),CheckTimeMovesOn) {
     if (idle_print) RMT_UT_LOG_INFO("test_idle_check_time_moves_on()\n");
     int chip = 7; // Full complement stages
     int pipe = 0;
     int stage = 0;

     // Create our TestUtil class
     // Instantiating new TestUtil obj should free all existing
     // RmtObjectManagers (if this has not already occurred) then
     // recreate a RmtObjectManager just for chip
     TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
     //auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);


     // DEBUG setup....................
     uint64_t pipes, stages, types, rows_tabs, cols, flags;
     pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;
     pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

     tu.set_debug(false);
     tu.set_evaluate_all(true, false); // Don't test evaluateAll
     tu.set_free_on_exit(true);
     // Just to stop compiler complaining about unused vars
     flags = FEW;
     tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


     // Instantiate whole chip and fish out objmgr
     RmtObjectManager *om = tu.get_objmgr();
     ASSERT_TRUE(om != NULL);
     // Lookup this Pipe/Stage MAU and MAU_ADDR_DIST obj
     Mau *mau = om->mau_lookup(pipe, stage);
     ASSERT_TRUE(mau != NULL);
     MauAddrDist *adist = mau->mau_addr_dist();
     ASSERT_TRUE(adist != NULL);
     // Lookup sweeper
     RmtSweeper *sweeper = om->sweeper_get();


     // UP debug now
     //flags = ALL;
     tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


     // Switch off kDisableSweepOnStageIdle - we want sweeps to
     // carry on regardless of whether stage is active
     bool prevDisableSweep = TableInfo::kDisableIdleSweepOnTableIdle;
     TableInfo::kDisableIdleSweepOnTableIdle = false;

     int iterations = 100;
     int tick_div = 17;
     uint64_t T_inc = MauDefs::kOneTickPicosecs/tick_div;

     // Set all sweep intervals to Min (0 => 1.6ms)
     for (int s = 0; s < 12; s++) sweeper->idle_set_stage_sweep_interval(0, s, 0);

     // Call sweep and check time moves on - and we occasionally get sweep
     int tot_sweeps = 0;
     for (int i = 0; i < iterations; i++) {
       uint64_t t_before = sweeper->get_time_now();
       sweeper->sweep_increment(T_inc);
       uint64_t t_after = sweeper->get_time_now();
       int n_sweeps = sweeper->get_stage_sweep_cnt(0);
       tot_sweeps += n_sweeps;
       if (idle_print)
         printf("TimeBefore=%16" PRId64 " TimeAfter=%16" PRId64 " Sweeps=%d\n",
                t_before, t_after, n_sweeps);
       EXPECT_LT(t_before, t_after);
     }
     EXPECT_EQ(iterations/tick_div, tot_sweeps);


     // Schtum
     tu.finish_test();
     tu.quieten_log_flags();
     TableInfo::kDisableIdleSweepOnTableIdle = prevDisableSweep;
   }



   TEST(BFN_TEST_NAME(IdleTest),CheckSweepCounts) {
     if (idle_print) RMT_UT_LOG_INFO("test_idle_check_sweep_counts()\n");
     int chip = 7; // Full complement stages
     int pipe = 0;
     int stage = 0;

     // Create our TestUtil class
     // Instantiating new TestUtil obj should free all existing
     // RmtObjectManagers (if this has not already occurred) then
     // recreate a RmtObjectManager just for chip
     TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
     //auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);


     // DEBUG setup....................
     uint64_t pipes, stages, types, rows_tabs, cols, flags;
     pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;
     pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

     tu.set_debug(false);
     tu.set_evaluate_all(true, false); // Don't test evaluateAll
     tu.set_free_on_exit(true);
     // Just to stop compiler complaining about unused vars
     flags = FEW;
     tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

     // Switch on fatal/error/warn messages - happens by default these days
     //tu.update_log_flags(0, 0, 0, 0, 0, UINT64_C(0x7), ALL);

     // Switch *ON* specific outputs
     //pipes = UINT64_C(1) << pipe;
     //types = UINT64_C(1) << RmtTypes::kRmtTypeParser;
     //flags = RmtDebug::kRmtDebugParserParseLoop;
     //tu.update_log_flags(pipes, 0, types, 0, 0, flags, ALL);

     // Switch *OFF* certain output
     //pipes = UINT64_C(1) << pipe;
     //types = UINT64_C(1) << RmtTypes::kRmtTypeMauTcam;
     //flags = RmtDebug::kRmtDebugMauTcam_Tcam3LookupDetail|RmtDebug::kRmtDebugMauTcam_Tcam3DebugMiss;
     //tu.update_log_flags(pipes, 0, types, 0, 0, 0, ~flags);

     // DEBUG setup done!!!!!!!!!!!!!!!!!!!!!!


     // Instantiate whole chip and fish out objmgr
     RmtObjectManager *om = tu.get_objmgr();
     ASSERT_TRUE(om != NULL);
     // Lookup this Pipe/Stage MAU and MAU_ADDR_DIST obj
     Mau *mau = om->mau_lookup(pipe, stage);
     ASSERT_TRUE(mau != NULL);
     MauAddrDist *adist = mau->mau_addr_dist();
     ASSERT_TRUE(adist != NULL);
     // Lookup sweeper
     RmtSweeper *sweeper = om->sweeper_get();


     // Set all PHV ingress/egress threads for our 12 stages
     for (int s = 0; s < 12; s++) {
       tu.set_phv_range_all(s, false);
       // Then set 1st 32 of the 64x32 64x8 to be ingress
       // and the 1st 48 of the 96x16 to be ingress
       tu.set_phv_ranges(s, 32,0, 32,0, 32,16,0, true);

       // Setup ingress dependencies for stages
       tu.set_dependency(s, TestUtil::kDepAction, true);
       tu.set_dependency(s, TestUtil::kDepConcurrent, false);

       // Setup single logical table for ingress in all stages
       tu.table_config(s, 0, true);    // stage  table0  ingress
     }


     // UP debug now
     //flags = ALL;
     tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


     // Switch off kDisableSweepOnStageIdle - we want sweeps to
     // carry on regardless of whether stage is active
     bool prevDisableSweep = TableInfo::kDisableIdleSweepOnTableIdle;
     TableInfo::kDisableIdleSweepOnTableIdle = false;

     uint8_t  MAX_INTVL = MauDefs::kMaxInterval-1;

     uint64_t picos_per_cycle = UINT64_C(1000000000000) / RmtDefs::kRmtClocksPerSec;
     uint64_t T = UINT64_C(4294967296) * picos_per_cycle;
     // T=2^32 cycles in picoseconds - this is 3.4s on Tofino, 4.3s on JBay
     // NB. Remaining times in this test all apply to **Tofino**

     // Set all intervals to MAX (30 => 20.3 days)
     for (int s = 0; s < 12; s++)
       sweeper->idle_set_stage_sweep_interval(0, s, MAX_INTVL);

     // Now program up sweeper to sweep different stages at different frequencies
     // (all for pipe0, we don't bother with pipes 1,2,3 for moment)
     sweeper->idle_set_stage_sweep_interval(0, 5, 0); // Highest freq - stage5 - every 1.677 ms
     sweeper->idle_set_stage_sweep_interval(0, 6, 1);
     sweeper->idle_set_stage_sweep_interval(0, 4, 2);
     sweeper->idle_set_stage_sweep_interval(0, 7, 3);
     sweeper->idle_set_stage_sweep_interval(0, 3, 4);
     sweeper->idle_set_stage_sweep_interval(0, 8, 5);
     sweeper->idle_set_stage_sweep_interval(0, 2, 6);
     sweeper->idle_set_stage_sweep_interval(0, 9, 7);
     sweeper->idle_set_stage_sweep_interval(0, 1, 8);
     sweeper->idle_set_stage_sweep_interval(0,10, 9);
     sweeper->idle_set_stage_sweep_interval(0, 0,10);
     sweeper->idle_set_stage_sweep_interval(0,11,11);// Lowest freq - stage11 - every 3.4 seconds


     // Then call sweeper saying 3.4s has elapsed (actually 3435973836800  psecs)
     // Stage11 should get swept once - stage5 should get swept 2048 times
     sweeper->sweep(T); // Time interval (0 -> 3.4]

     // Check/reset sweep cnts
     for (int s = 0; s < 12; s++) {
       uint32_t cnt = sweeper->get_stage_sweep_cnt(s);
       printf("SweepA: Stage%d: sweep_cnt=%d\n", s, cnt);
       if (s ==  2) {
         EXPECT_EQ(  32u, cnt);
       }
       if (s ==  5) {
         EXPECT_EQ(2048u, cnt);
       }
       if (s ==  9) {
         EXPECT_EQ(  16u, cnt);
       }
       if (s == 11) {
         EXPECT_EQ(   1u, cnt);
       }
     }

     // Set all intervals to MAX (30 => 20.3 days)
     for (int s = 0; s < 12; s++)
       sweeper->idle_set_stage_sweep_interval(0, s, MAX_INTVL);

     // Now just setup stages so only a subset of possible frequencies
     // are used - should still see same sweep cnts
     sweeper->idle_set_stage_sweep_interval(0, 3, 4);
     sweeper->idle_set_stage_sweep_interval(0, 8, 5);
     sweeper->idle_set_stage_sweep_interval(0, 2, 6);
     sweeper->idle_set_stage_sweep_interval(0, 9, 7);

     sweeper->sweep(T + T); // Time interval (3.4 -> 6.8)

     for (int s = 0; s < 12; s++) {
       uint32_t cnt = sweeper->get_stage_sweep_cnt(s);
       printf("SweepB: Stage%d: sweep_cnt=%d\n", s, cnt);
       if (s == 2) {
         EXPECT_EQ(32u, cnt);
       }
       if (s == 9) {
         EXPECT_EQ(16u, cnt);
       }
     }

     // Now let another 3.4s elapse but rather than
     // doing it all in one fell swoop, do it a few
     // picoseconds at a time and check we get same
     // answers as before.
     // Time interval (6.8 -> 10.2]
     uint64_t stepT = T / UINT64_C(128);
     for (int t = 1; t <= 128; t++) {
       sweeper->sweep(T + T + (t * stepT));
     }

     for (int s = 0; s < 12; s++) {
       uint32_t cnt = sweeper->get_stage_sweep_cnt(s);
       printf("SweepC: Stage%d: sweep_cnt=%d\n", s, cnt);
       if (s == 2) {
         EXPECT_EQ(32u, cnt);
       }
       if (s == 9) {
         EXPECT_EQ(16u, cnt);
       }
     }


     // Schtum
     tu.finish_test();
     tu.quieten_log_flags();
     TableInfo::kDisableIdleSweepOnTableIdle = prevDisableSweep;
   }



   TEST(BFN_TEST_NAME(IdleTest),SingleMapRam) {
     if (idle_print) RMT_UT_LOG_INFO("test_idle_single_map_ram()\n");
     int chip = 7; // Full complement stages
     int pipe = 0;
     int stage = 0;

     // Create our TestUtil class
     // Instantiating new TestUtil obj should free all existing
     // RmtObjectManagers (if this has not already occurred) then
     // recreate a RmtObjectManager just for chip
     TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
     //auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);

     // Register IDLE callback func with Model
     GLOBAL_MODEL.get()->
         register_callback_dru_idle_update(idle_update_callback_print);

     uint64_t MAX = UINT64_C(0x7FF); // MAX value possible in MapRAM

     // DEBUG setup....................
     uint64_t pipes, stages, types, rows_tabs, cols, flags;
     pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;
     pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

     tu.set_debug(false);
     tu.set_evaluate_all(true, false); // Don't test evaluateAll
     tu.set_free_on_exit(true);
     // Just to stop compiler complaining about unused vars
     flags = FEW;
     tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

     // Switch on fatal/error/warn messages - happens by default these days
     //tu.update_log_flags(0, 0, 0, 0, 0, UINT64_C(0x7), ALL);

     // Switch *ON* specific outputs
     //pipes = UINT64_C(1) << pipe;
     //types = UINT64_C(1) << RmtTypes::kRmtTypeParser;
     //flags = RmtDebug::kRmtDebugParserParseLoop;
     //tu.update_log_flags(pipes, 0, types, 0, 0, flags, ALL);

     // Switch *OFF* certain output
     //pipes = UINT64_C(1) << pipe;
     //types = UINT64_C(1) << RmtTypes::kRmtTypeMauTcam;
     //flags = RmtDebug::kRmtDebugMauTcam_Tcam3LookupDetail|RmtDebug::kRmtDebugMauTcam_Tcam3DebugMiss;
     //tu.update_log_flags(pipes, 0, types, 0, 0, 0, ~flags);

     // DEBUG setup done!!!!!!!!!!!!!!!!!!!!!!


     // Instantiate whole chip and fish out objmgr
     RmtObjectManager *om = tu.get_objmgr();
     ASSERT_TRUE(om != NULL);
     // Lookup this Pipe/Stage MAU and MAU_ADDR_DIST obj
     Mau *mau = om->mau_lookup(pipe, stage);
     ASSERT_TRUE(mau != NULL);
     MauAddrDist *adist = mau->mau_addr_dist();
     ASSERT_TRUE(adist != NULL);
     // Lookup sweeper
     RmtSweeper *sweeper = om->sweeper_get();


     // Set all PHV ingress/egress threads for our 12 stages
     for (int s = 0; s < 12; s++) {
       tu.set_phv_range_all(s, false);
       // Then set 1st 32 of the 64x32 64x8 to be ingress
       // and the 1st 48 of the 96x16 to be ingress
       tu.set_phv_ranges(s, 32,0, 32,0, 32,16,0, true);

       // Setup ingress dependencies for stages
       tu.set_dependency(s, TestUtil::kDepAction, true);
       tu.set_dependency(s, TestUtil::kDepConcurrent, false);

       // Setup single logical table for ingress in all stages
       tu.table_config(s, 0, true);    // stage  table0  ingress

       // And setup a single mapram in all stages
       // TofinoIdleInfo=[Interval=9 ( =>859ms), IdleBus=1, IdletimeBitWidth=3 (=>6bits)]
       // JBayIdleInfo  =[Interval=9 (=>1073ms), IdleBus=1, IdletimeBitWidth=3 (=>6bits)]
       // This call automatically sets up sweeper for table0
       int idle_flags = TestUtil::idle_flags_make(TestUtil::kIdleFlagTwoWay, 9, 1, 3);
       tu.mapram_config(s, 0, 6, TestUtil::kMapramTypeIdletime, 7, 7,
                        idle_flags, 0, false);
       // Set maximal value across all mapram entries so initially
       // mapram is completely idle... apart from entry 0 which we set to 1
       // (if we set to 0 we'd also get active notification on 0->1)
       for (int i = 0; i < TestUtil::kMapramMaxEntries; i++)
         tu.mapram_write(s, 0, 6, i, MAX, ALL);
       tu.mapram_write(s, 0, 6, 0, ONE, ONE);
     }


     // UP debug now
     //flags = ALL;
     tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


     // Leave kDisableSweepOnStageIdle alone
     bool prevDisableSweep = TableInfo::kDisableIdleSweepOnTableIdle;
     //TableInfo::kDisableIdleSweepOnTableIdle = false;


     // Sweep chip for 70 seconds - should see IDLE timeout
     // for mapram entry 0 after about 63* 859ms = 54.1 secs (Tofino)
     //                 or after about 63*1073ms = 67.6 secs (JBay)
     uint64_t t_100ms = UINT64_C(100000000000);
     for (int t = 1; t <= 700; t++) {
       if ((t % 100) == 1) printf("IdleTest::SingleMapRam(A,t=%d)\n", t);
       sweeper->sweep(t * t_100ms);
     }

     for (int s = 0; s < 12; s++) {
       uint32_t cnt = sweeper->get_stage_sweep_cnt(s);
       EXPECT_EQ(63u, cnt);
       printf("SweepC: Stage%d: sweep_cnt=%d\n", s, cnt);
     }

     // Do more sweeping - shouldn't see any further notifies
     for (int t = 1; t <= 600; t++) {
       if ((t % 100) == 1) printf("IdleTest::SingleMapRam(B,t=%d)\n", t);
       sweeper->sweep_increment(t * t_100ms);
     }


     // Schtum
     tu.finish_test();
     tu.quieten_log_flags();
     TableInfo::kDisableIdleSweepOnTableIdle = prevDisableSweep;
   }


   TEST(BFN_TEST_NAME(IdleTest),StressTest) {
     if (idle_print) RMT_UT_LOG_INFO("test_idle_stress_test()\n");
     if (MauMemory::kIdletimeVAddrSweepBubbleEmulate) {
       printf("**************************************************************\n");
       printf("*****    *NOT* RUNNING IdleTest.StressTest because       *****\n");
       printf("***** MauMemory::kIdletimeVAddrSweepBubbleEmulate = true *****\n");
       printf("***** and the test would take WAY too long to run        *****\n");
       printf("**************************************************************\n");
       return;
     }

     int chip = 7; // Full complement stages
     int pipe = 0;
     int stage = 0;

     // DEBUG setup....................
     uint64_t pipes, stages, types, rows_tabs, cols, flags;
     uint64_t CLR = RmtDebug::kRmtDebugRead|RmtDebug::kRmtDebugWrite;
     uint64_t CLR2 = CLR;
     CLR2 |= RmtDebug::kRmtDebugMauMapramIdleSweep;
     CLR2 |= RmtDebug::kRmtDebugMauMapramIdleActive;
     pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;
     pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

     // Create our TestUtil class
     // Instantiating new TestUtil obj should free all existing
     // RmtObjectManagers (if this has not already occurred) then
     // recreate a RmtObjectManager just for chip
     TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
     tu.set_debug(false);
     tu.set_evaluate_all(true, false); // Don't test evaluateAll
     tu.set_free_on_exit(true);

     // Otherwise dumps post lock will just block
     TableInfo::kSynchronousIdleOps = false;

     srand(time(NULL));
     //uint64_t seed = UINT64_C(0);
     uint64_t seed = static_cast<uint64_t>(rand());
     printf("IdleTest:StressTest(SEED = %" PRId64 ")\n", seed);
     int n_iters = 1999; // Must be in [16,1024*1024]
     int n_entries = 101; // Must be in [16,1024]
     int n_active = 71;
     int min_intvl = 1;
     int max_intvl = 7;
     uint8_t allowed_idle_widths = ALLOWED_IDLE_WIDTHS;
     bool force_2way = false;
     bool allow_2way = true;
     bool allow_pfe = true;
     bool debug = false;
     Idler idler(&tu, UINT64_C(0),
                 n_iters, n_entries, n_active, min_intvl, max_intvl,
                 allowed_idle_widths, allow_2way, allow_pfe, force_2way,
		 debug);

    GLOBAL_IDLER = &idler;
    // Register IDLER callback func with Model
    GLOBAL_MODEL.get()->
        register_callback_dru_idle_update(global_idler_callback);

    tu.update_log_flags(ALL, ALL, ALL, ALL, ALL, NON, ~CLR2);
    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ~CLR2);

    // Initialise Idler and all MapRAMs etc
    idler.init();

    // UP debug now - not on maprams - and still no read/write
    //flags = ALL;
    uint64_t types2 = RmtTypes::kRmtTypeMauMapram;
    // TODO: expect next 2 lines to:
    // Line1: switch on all debug but rd/wr in all non-mapram objects
    // Line2: switch on all debug but rd/wr/idle/active in mapram objects
    // but doesn't seem to do that - why? - disable Line1 for now
    //tu.update_log_flags(pipes, stages, ~types2, rows_tabs, cols, flags, ~CLR);
    tu.update_log_flags(pipes, stages, types2, rows_tabs, cols, flags, ~CLR2);

    // Run test1
    int res = idler.test1();
    EXPECT_EQ(0,res);
    //assert(res == 0);

    // Schtum
    tu.finish_test();
    tu.quieten_log_flags();
    GLOBAL_IDLER = NULL;
  }



}
