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

#ifndef _SHARED_MAC_COUNTERS_H_
#define _SHARED_MAC_COUNTERS_H_

#include <cstdint>
#include <cinttypes>

#include <common/rmt-assert.h>
#include <model_core/spinlock.h>


namespace MODEL_CHIP_NAMESPACE {

class Packet;

// Struct to define config for array that maps
// Vendor MAC counter indices to generic indices
struct MacCounterMapperConfig {
  int       vendor_index;
  bool      is_rx;
  int       cntr_flavor0;
  uint32_t  cntr_mask0;
  int       cntr_flavor1;
  uint32_t  cntr_mask1;
  bool      do_subtract;
};


// Define the types of things we plan to count
struct CntrFlavor {
  static constexpr int None      = 0;
  static constexpr int Frames    = 1;
  static constexpr int Octets    = 2;
  static constexpr int Pri       = 3;
  static constexpr int PriPause  = 4;
  static constexpr int Bucket64  = 5;
  static constexpr int Bucket512 = 6;
};
// Define all the things we plan to count
struct Cntr {
  static constexpr int OK                     =  0;
  static constexpr int All                    =  1;
  static constexpr int Error                  =  2;
  static constexpr int CrcError               =  3;
  static constexpr int Unicast                =  4;
  static constexpr int Multicast              =  5;
  static constexpr int Broadcast              =  6;
  static constexpr int Vlan                   =  7;
  static constexpr int Pause                  =  8;
  static constexpr int PriPause               =  9;
  // Track 'special' sizes twice so we can safely clear
  static constexpr int SizeEq64               = 10;
  static constexpr int SizeEq64_B             = 11;
  static constexpr int Size15xx_A             = 12;
  static constexpr int Size15xx_B             = 13;
  // Error detail counters (16)
  static constexpr int Truncated              = 14;
  static constexpr int Undersized             = 15;
  static constexpr int Oversized              = 16;
  static constexpr int Drained                = 17;
  static constexpr int Padded                 = 18;
  static constexpr int Dropped                = 19;
  static constexpr int Fragment               = 20;
  static constexpr int Jabber                 = 21;
  static constexpr int LenErr                 = 22;
  static constexpr int TooLongErr             = 23;
  static constexpr int CrcErrStomp            = 24;
  static constexpr int MaxFrmSizeViolErr      = 25;
  static constexpr int InvalidPreambleErr     = 26;
  static constexpr int NormalLenInvalidCrcErr = 27;
  static constexpr int UnusedErr1             = 28;
  static constexpr int UnusedErr2             = 29;

  // Var to track how many - should be last one above + 1
  static constexpr int kNumFrameCounters      = 30;
  static constexpr int kNumOctetCounters      = All+1; // Only OK/All

  static_assert( (kNumOctetCounters <= kNumFrameCounters),
                 "Invalid value kNumOctetCounters");
  static_assert( (kNumFrameCounters <= 32),
                 "Invalid value kNumFrameCounters");

  // Used when we're inferring flags
  static constexpr uint32_t kAddrtypeMask =    0x7u <<  4;
  static constexpr uint32_t kPayloadMask  =    0x7u <<  7;
  static constexpr uint32_t kErrorMask    = 0xFFFFu << 14;
  static constexpr uint32_t kCrcErrorMask =   0x24u << 22;
};


class MacCounters {
 public:
  static constexpr int kNumFrameCounters       =  Cntr::kNumFrameCounters;
  static constexpr int kNumOctetCounters       =  Cntr::kNumOctetCounters;
  // MACs track 8 priority levels for normal packets and Pause packets
  static constexpr int kNumPriCounters         =  8; // Must be <= 32
  static constexpr int kNumPriPauseCounters    =  9; // Must be <= 32
  // Break [0-2047]  into 16x64B buckets and track packets in each [0-63]..[960-1023]
  static constexpr int kNumBucket64Counters    = 16; // Must be <= 32
  // Break [0-16383] into 32x512B buckets and track packets in each [0-511]..[15872..16383]
  static constexpr int kNumBucket512Counters   = 32; // Must be <= 32

  // Figure out total size of counter array
  static constexpr int kCounter0          = 0;
  static constexpr int kFrameCounter0     = kCounter0;
  static constexpr int kOctetCounter0     = kFrameCounter0 + kNumFrameCounters;
  static constexpr int kPriCounter0       = kOctetCounter0 + kNumOctetCounters;
  static constexpr int kPriPauseCounter0  = kPriCounter0 + kNumPriCounters;
  static constexpr int kBucket64Counter0  = kPriPauseCounter0 + kNumPriPauseCounters;
  static constexpr int kBucket512Counter0 = kBucket64Counter0 + kNumBucket64Counters;
  static constexpr int kNextCounter0      = kBucket512Counter0 + kNumBucket512Counters;
  static constexpr int kTotCounters       = kNextCounter0;

  static int get_frame_index(int i) {
    return ((i >= 0) && (i < kNumFrameCounters)) ?kFrameCounter0 + i :-1;
  }
  static int get_octet_index(int i) {
    return ((i >= 0) && (i < kNumOctetCounters)) ?kOctetCounter0 + i :-1;
  }
  static int get_pri_index(int pri) {
    return ((pri >= 0) && (pri < kNumPriCounters)) ?kPriCounter0 + pri :-1;
  }
  static int get_pripause_index(int pri) {
    return ((pri >= 0) && (pri < kNumPriPauseCounters)) ?kPriPauseCounter0 + pri :-1;
  }
  static int get_bucket64_index(int len) {
    return ((len >= 0) && ((len/64) < kNumBucket64Counters)) ?kBucket64Counter0 + (len/64) :-1;
  }
  static int get_bucket512_index(int len) {
    return ((len >= 0) && ((len/512) < kNumBucket512Counters)) ?kBucket512Counter0 + (len/512) :-1;
  }
  static int get_index(int flavor, int i) {
    switch (flavor) {
      case CntrFlavor::None:      return -1;
      case CntrFlavor::Frames:    return get_frame_index(i);
      case CntrFlavor::Octets:    return get_octet_index(i);
      case CntrFlavor::Pri:       return get_pri_index(i);
      case CntrFlavor::PriPause:  return get_pripause_index(i);
      case CntrFlavor::Bucket64:  return get_bucket64_index(i);
      case CntrFlavor::Bucket512: return get_bucket512_index(i);
      default:                    return -1;
    }
  }
  static int get_base(int flavor) { return get_index(flavor, 0); }

  static uint32_t make_range(int lo, int hi) {
    if ((lo < 0) || (hi < 0) || (hi > 31) || (lo > hi)) return 0u;
    return (0xFFFFFFFFu >> (31-hi)) & (0xFFFFFFFFu << lo);
  }


  MacCounters()          { spinlock_.clear(); reset(); }
  virtual ~MacCounters() { reset(); }


 private:
  void     reset();
  uint64_t do_get(int cntr) const;
  void     do_set(int cntr, uint64_t val);
  void     do_zero(int cntr);
  void     do_inc(int cntr, uint64_t val);
  uint64_t read_counter_mask_nolock(int base, uint32_t mask, bool clear);


 public:
  uint64_t read_counter(int i, bool clear);
  uint64_t read_counter_range(int a, int z, bool clear);
  uint64_t read_counter_mask(int base, uint32_t mask, bool clear);
  uint64_t read_counter_mask(int base1, uint32_t mask1, int base2, uint32_t mask2,
                             bool clear, bool subtract);
  void     clear_all();

  uint32_t infer_flags(int len, uint32_t pri, uint32_t flags);
  uint32_t flags_from_packet(Packet *pkt, uint32_t *pri=nullptr);
  // Increment ALL applicable counters based on len/pri/flags
  void increment_counters(int len, uint32_t pri, uint32_t flags);
  // Augment flags from Packet data
  void increment_counters(Packet *pkt, uint32_t flags);


 private:
  mutable  model_core::Spinlock         spinlock_;
  std::array< uint64_t, kTotCounters >  counters_;

}; // Class MacCounters



}

#endif // _SHARED_MAC_COUNTERS_H_
