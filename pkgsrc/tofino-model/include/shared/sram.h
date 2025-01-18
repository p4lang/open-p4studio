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

#ifndef _SHARED_SRAM_
#define _SHARED_SRAM_

#include <iostream>
#include <string>
#include <array>
#include <atomic>
#include <rmt-defs.h>
#include <bitvector.h>

namespace MODEL_CHIP_NAMESPACE {

  template <size_t N_ENTRIES, size_t WIDTH, bool INVERT=false>
      class Sram {

 private:
    static constexpr bool kSramUseSpinlocks = MauDefs::kSramUseSpinlocks;
    static constexpr int  kSramLocks        = (((N_ENTRIES/64)>2) ?(N_ENTRIES/64) :(2));

 public:
    Sram()  { reset(); }
    ~Sram() {}

    inline void lock(const int index) const {
      if (!kSramUseSpinlocks) return;
      spinlocks_[index % kSramLocks].lock();
    }
    inline void unlock(const int index) const {
      if (!kSramUseSpinlocks) return;
      spinlocks_[index % kSramLocks].unlock();
    }

    inline bool ok_index(const unsigned int index) const {
      return (index < N_ENTRIES);
    }
    inline bool valid_index(const int index) const {
      return (ok_index(index) && (valid_[index]));
    }
    inline bool get_nolock(const int index, BitVector<WIDTH> *bv_val) const {
      if (!ok_index(index) || (bv_val == NULL)) return false;
      *bv_val = values_[index];
      if (INVERT) (*bv_val).invert();
      return true;
    }
    inline bool set_nolock(const int index, const BitVector<WIDTH> &bv_val) {
      if (!ok_index(index)) return false;
      values_[index] = bv_val;
      if (INVERT) (values_[index]).invert();
      valid_[index] = true;
      return true;
    }


    inline bool get(const int index, BitVector<WIDTH> *bv_val) const {
      lock(index);
      bool ret = get_nolock(index, bv_val);
      unlock(index);
      return ret;
    }
    inline void set(const int index, const BitVector<WIDTH> &bv_val) {
      lock(index);
      set_nolock(index, bv_val);
      unlock(index);
    }
    inline void set_masked(const int index, const BitVector<WIDTH> &bv_val,
                           const BitVector<WIDTH> &mask) {
      // Create inverse mask for old_data
      BitVector<WIDTH> inverted_mask(UINT64_C(0));
      inverted_mask.copy_from(mask); inverted_mask.invert();
      // Mask input data -> new_data
      BitVector<WIDTH> new_data(UINT64_C(0));
      new_data.copy_from(bv_val);
      new_data.mask(mask);
      BitVector<WIDTH> old_data(UINT64_C(0));

      lock(index);
      // Read old data
      bool ret = get_nolock(index, &old_data);
      if (ret) {
        // Inverse mask old_data then OR in new_data
        old_data.mask(inverted_mask);
        old_data.or_with(new_data);
        set_nolock(index, old_data);
      }
      unlock(index);
    }
    inline bool get_and_preserve(const int index, BitVector<WIDTH> *bv_val,
                                 const BitVector<WIDTH> &preserve_mask) {
      BitVector<WIDTH> curr_data(UINT64_C(0));
      lock(index);
      // Read curr data
      bool ret = get_nolock(index, &curr_data);
      if (ret) {
        // Pass out curr data
        bv_val->copy_from(curr_data);
        // Modify curr data using mask then write back
        curr_data.mask(preserve_mask);
        set_nolock(index, curr_data);
      }
      unlock(index);
      return ret;
    }
    inline bool get_and_clear_old(const int index, BitVector<WIDTH> *bv_val,
                                  const BitVector<WIDTH> &clear_mask) {
      BitVector<WIDTH> preserve_mask(UINT64_C(0));
      preserve_mask.copy_from(clear_mask);
      preserve_mask.invert();
      return get_and_preserve(index, bv_val, preserve_mask);
    }
    inline bool get_and_clear(const int index, BitVector<WIDTH> *bv_val,
                              const BitVector<WIDTH> &clear_mask,
                              bool unless_exactly_equals_mask=false) {
      BitVector<WIDTH> curr_data(UINT64_C(0));
      lock(index);
      // Read curr data
      bool ret = get_nolock(index, &curr_data);
      if (ret) {
        bool do_clear = true;
        // Check if data *exactly* matches mask
        if (unless_exactly_equals_mask) {
          BitVector<WIDTH> tmp_data(UINT64_C(0));
          tmp_data.copy_from(curr_data);
          tmp_data.mask(clear_mask);
          if (tmp_data.equals(clear_mask)) do_clear = false;
        }
        // Pass out curr data
        bv_val->copy_from(curr_data);
        // (Maybe) modify curr data using mask then write back
        if (do_clear) {
          BitVector<WIDTH> preserve_mask(UINT64_C(0));
          preserve_mask.copy_from(clear_mask);
          preserve_mask.invert();
          curr_data.mask(preserve_mask);
          set_nolock(index, curr_data);
        }
      }
      unlock(index);
      return ret;
    }


    inline uint64_t get_word(const int index, unsigned int off, int nbits) const {
      RMT_ASSERT((off < WIDTH));
      RMT_ASSERT((nbits >= 0) && (nbits <= std::min((int)WIDTH,64)));
      uint64_t retword = UINT64_C(0);
      BitVector<WIDTH> bv(UINT64_C(0));
      lock(index);
      if (get_nolock(index, &bv)) retword = bv.get_word(off, nbits);
      unlock(index);
      return retword;
    }
    inline void set_word(const int index, unsigned int off, int nbits, uint64_t word) {
      RMT_ASSERT((off < WIDTH));
      RMT_ASSERT((nbits >= 0) && (nbits <= std::min((int)WIDTH,64)));
      BitVector<WIDTH> bv(UINT64_C(0));
      lock(index);
      get_nolock(index, &bv);
      bv.set_word(word, off, nbits);
      set_nolock(index, bv);
      unlock(index);
    }
    inline uint64_t swap_word(const int index, unsigned int off, int nbits, uint64_t word) {
      RMT_ASSERT((off < WIDTH));
      RMT_ASSERT((nbits >= 0) && (nbits <= std::min((int)WIDTH,64)));
      uint64_t retword = UINT64_C(0);
      BitVector<WIDTH> bv(UINT64_C(0));
      lock(index);
      get_nolock(index, &bv);
      retword = bv.get_word(off, nbits);
      bv.set_word(word, off, nbits);
      set_nolock(index, bv);
      unlock(index);
      return retword;
    }
    inline bool test_and_set_word(const int index, int off, int nbits,
                                  uint64_t old_word, uint64_t new_word) {
      RMT_ASSERT((off >= 0) && (off < WIDTH));
      RMT_ASSERT((nbits >= 0) && (nbits <= std::min((int)WIDTH,64)));
      BitVector<WIDTH> bv(UINT64_C(0));
      uint64_t curr_word = UINT64_C(0);
      bool did_set = false;
      lock(index);
      if (get_nolock(index, &bv)) curr_word = bv.get_word(off, nbits);
      if (curr_word == old_word) {
        bv.set_word(new_word, off, nbits);
        set_nolock(index, bv);
        did_set = true;
      }
      unlock(index);
      return did_set;
    }
    // Returns final value post reset
    inline uint64_t reset_word(const int index, unsigned int off, int nbits,
                               uint64_t min_val, uint64_t max_val, uint64_t reset_val) {
      RMT_ASSERT((off < WIDTH));
      RMT_ASSERT((nbits >= 0) && (nbits <= std::min((int)WIDTH,64)));
      BitVector<WIDTH> bv(UINT64_C(0));
      // Logic here is we reset to 0 UNLESS curr_val in (min,max)
      // in which case we reset to passed-in reset_val
      // (NB. strictly within min,max so NOT including min,max endpoints)
      // So, normal reset to 0 behaviour can always be achieved by setting reset_val=0
      uint64_t curr_val = UINT64_C(0), new_val = UINT64_C(0);
      lock(index);
      if (get_nolock(index, &bv)) curr_val = bv.get_word(off, nbits);
      if ((curr_val > min_val) && (curr_val < max_val)) new_val = reset_val;
      bv.set_word(new_val, off, nbits);
      set_nolock(index, bv);
      unlock(index);
      return new_val;
    }
    // Returns original value pre increment
    // Can pass 2 different increments for case where curr_val==0 and curr_val!=0
    inline uint64_t incr_word(const int index, unsigned int off, int nbits, uint64_t max_val,
                              uint64_t incrEQ0=UINT64_C(1), uint64_t incrNE0=UINT64_C(1)) {
      RMT_ASSERT((off < WIDTH));
      RMT_ASSERT((nbits >= 0) && (nbits <= std::min((int)WIDTH,64)));
      BitVector<WIDTH> bv(UINT64_C(0));
      uint64_t orig_val = UINT64_C(0);
      lock(index);
      if (get_nolock(index, &bv)) orig_val = bv.get_word(off, nbits);
      if (orig_val < max_val) {
        uint64_t new_val = orig_val + ((orig_val == UINT64_C(0)) ?incrEQ0 :incrNE0);
        bv.set_word(new_val, off, nbits);
        set_nolock(index, bv);
      }
      unlock(index);
      return orig_val;
    }

    inline void print(const int index) const {
      BitVector<WIDTH> bv_val;
      if (get_nolock(index, bv_val)) bv_val.print();
    }
    inline void reset() {
      for (unsigned int i = 0; i < N_ENTRIES; i++) {
        valid_[i] = false;
        values_[i] = BitVector<WIDTH>().fill_all_zeros();
      }
      for (int lock = 0; lock < kSramLocks; lock++) {
        spinlocks_[lock].clear();
      }
    }

 private:
    mutable std::array<
        model_core::Spinlock,kSramLocks>             spinlocks_ { };
    std::array<bool,N_ENTRIES>                       valid_ { };
    std::array<BitVector<WIDTH>,N_ENTRIES>           values_ { };
  };
}

#endif // _SHARED_SRAM_

