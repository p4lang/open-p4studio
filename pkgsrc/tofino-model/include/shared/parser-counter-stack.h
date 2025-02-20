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

#ifndef _SHARED_PARSER_COUNTER_STACK_
#define _SHARED_PARSER_COUNTER_STACK_

#include <common/rmt-assert.h>


namespace MODEL_CHIP_NAMESPACE {

  template <size_t STACK_ENTRIES, bool DUMP=false> class ParserCounterStack {

 public:
    ParserCounterStack()  { reset(); }
    ~ParserCounterStack() { n_entries_ = 0; }

 private:
    inline void copy_to_from(int x, int y) {
      stack_entries_[x] = stack_entries_[y];
      propagate_incr_[x] = propagate_incr_[y];
    }
    inline void assign(int x, const int8_t cntr, const bool propagate_incr) {
      stack_entries_[x] = cntr;
      propagate_incr_[x] = propagate_incr;
    }
    inline void zeroise(int x) {
      assign(x, 0, false);
    }

 public:
    inline void reset() {
      n_entries_ = 0;
      for (size_t i = 0; i <= STACK_ENTRIES; i++) zeroise(i);
    }
    inline int peek(int8_t cntrs[], bool propagates[], int size) {
      size_t sz = ((size < 0) || (size > (int)STACK_ENTRIES)) ?STACK_ENTRIES :(size_t)size;
      for (size_t i = 0; i < sz; i++) {
        cntrs[i] = stack_entries_[i];
        propagates[i] = propagate_incr_[i];
      }
      return static_cast<int>(n_entries_);
    }
    inline uint64_t peek64(int8_t curr) {
      // Format is:
      //   valid[61:56] propagates[53:48] cntr5[47:40] cntr4[39:32] .. cntr0[7:0]
      // The passed in 'curr' value is always cntr0
      // In JBay/WIP usage cntr5 cntr4 both 0 as only entries 3,2,1,0 used
      // Less signif bits of valid/propagate refer to less signif counters so:
      // - valid[0] (ie bit56) and propagates[0] (ie bit48) refer to cntr0
      // - valid[1] (ie bit57) and propagates[1] (ie bit49) refer to cntr1
      // Note valid[0] is always 1 and propagates[0] is always 0
      //
      uint8_t  b   = static_cast<uint8_t>(curr);
      uint64_t v64 = static_cast<uint64_t>( b );
      uint64_t valid = UINT64_C(0xFF) >> (8 - 1 - n_entries_);
      uint64_t props = UINT64_C(0);
      for (size_t i = 0; i < STACK_ENTRIES; i++) {
        b = static_cast<uint8_t>(stack_entries_[i]);
        // 'curr' cntr in byte 0; Stack cntrs in bytes 1..2..3
        v64 |= static_cast<uint64_t>(b) << ((i + 1) * 8);
        // Propagate flags for stack cntrs in bits 1..2..3 (bit0 always 0)
        if (propagate_incr_[i]) props |= (UINT64_C(1) << (i + 1));
      }
      v64 |= (valid << 56) | (props << 48);
      return v64;
    }
    inline uint32_t hash() {
      uint32_t h = 0u;
      if (STACK_ENTRIES <= 3) {
        // Put stack info as is into hash (n_entries/propagate bitmasks in MSByte)
        for (size_t i = 0; i < STACK_ENTRIES; i++) {
          uint8_t b = static_cast<uint8_t>(stack_entries_[i]);
          h |= static_cast<uint32_t>(b) << (i*8); // Bytes 0..2
          if (propagate_incr_[i]) h |= 1u<<(24+i); // Bits 24..26
        }
        h |= (0xFu >> (4-n_entries_)) << 28; // Bits 28..30
      } else {
        h = static_cast<uint32_t>( n_entries_ );
        for (size_t i = 0; i <= STACK_ENTRIES; i++) {
          uint8_t b = static_cast<uint8_t>(stack_entries_[i]);
          h ^= static_cast<uint32_t>( ((propagate_incr_[i]?1:0) << 8) | (b << 0) );
        }
      }
      return h;
    }
    inline void dump(const char *inf, int8_t v) {
      printf("%s<%d>  CounterStack[Top->Bot]:  [ ", inf, v);
      for (size_t i = 0; i < n_entries_; i++) {
        const char *prop = propagate_incr_[i] ?"*" :"";
        printf("%d%s ", static_cast<int>(stack_entries_[i]), prop);
      }
      printf("]  {*=>prop_incr}\n");
    }
    inline bool push(const int8_t cntr, const bool propagate_incr) {
      if (DUMP) dump(" PrePush", cntr);
      bool overflow = (n_entries_ >= STACK_ENTRIES);
      if (n_entries_ > 0) { // Copy all entries up 1
        for (size_t i = n_entries_; i > 0; i--) copy_to_from(i, i-1);
      }
      if (!overflow) n_entries_++;
      if (STACK_ENTRIES > 0) assign(0, cntr, propagate_incr);
      if (DUMP) dump("PostPush", cntr);
      return !overflow;
    }
    inline bool pop(int8_t *cntr) {
      RMT_ASSERT(cntr != NULL);
      if (DUMP) dump(" PrePop", 0);
      bool underflow = (n_entries_ <= 0);
      *cntr = stack_entries_[0]; // Maybe unused (so zero)
      if (n_entries_ > 1) {      // Copy all entries down 1
        for (size_t i = 0; i < n_entries_; i++) copy_to_from(i, i+1);
      }
      if (!underflow) n_entries_--;
      zeroise(n_entries_);
      if (DUMP) dump("PostPop",*cntr);
      return !underflow;
    }
    inline void incr(const int8_t incr) {
      if (DUMP) dump(" PreIncr",incr);
      if (n_entries_ <= 0) return;
      for (size_t i = 0; i < n_entries_; i++) {
        if (!propagate_incr_[i]) break;
        stack_entries_[i] += incr;
      }
      if (DUMP) dump("PostIncr",incr);
    }

 private:
    size_t                                 n_entries_;
    std::array< int8_t, STACK_ENTRIES+1 >  stack_entries_;
    std::array< bool, STACK_ENTRIES+1 >    propagate_incr_;

  };
}

#endif // _SHARED_PARSER_COUNTER_STACK_
