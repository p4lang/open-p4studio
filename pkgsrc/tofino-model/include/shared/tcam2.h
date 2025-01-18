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

#ifndef _SHARED_TCAM2_
#define _SHARED_TCAM2_

#include <iostream>
#include <string>
#include <array>
#include <bitvector.h>

namespace MODEL_CHIP_NAMESPACE {

  template <size_t N_ENTRIES, size_t WIDTH>
      class Tcam2 {

 public:
    Tcam2() {}
    ~Tcam2() {}

    inline bool is_valid(const int index) const {
      return valid_[index];
    }
    inline void get(const int index,
                    BitVector<WIDTH> *bv_word1, BitVector<WIDTH> *bv_word0) const {
      if (bv_word1 != NULL)
        *bv_word1 = word1s_[index];
      if (bv_word0 != NULL)
        *bv_word0 = word0s_[index];
    }
    inline void set_word1_word0(const int index,
                                const BitVector<WIDTH> &bv_word1,
                                const BitVector<WIDTH> &bv_word0,
                                bool valid=true) {
      word1s_[index] = bv_word1; 
      word0s_[index] = bv_word0;
      valid_[index] = valid;
    }
    inline void set_value_mask(const int index,
                               const BitVector<WIDTH> &bv_value,
                               const BitVector<WIDTH> &bv_mask,
                               bool valid=true) {
      BitVector<WIDTH> bv_word1(UINT64_C(0));
      BitVector<WIDTH> bv_word0(UINT64_C(0));
      BitVector<WIDTH> bv_inverted_mask(UINT64_C(0));
      // Create ~mask
      bv_inverted_mask.copy_from(bv_mask);
      bv_inverted_mask.invert();
      // Word1 = value & mask | ~mask
      bv_word1.copy_from(bv_value);
      bv_word1.mask(bv_mask);
      bv_word1.or_with(bv_inverted_mask);
      // Word0 = ~value & mask | ~mask
      bv_word0.copy_from(bv_value);
      bv_word0.invert();
      bv_word0.mask(bv_mask);
      bv_word0.or_with(bv_inverted_mask);
      // Call word1/word0 set
      set_word1_word0(index, bv_word1, bv_word0, valid);
    }
    inline void set_value(const int index,
                          const BitVector<WIDTH> &bv_value) {
      set_value_mask(index, bv_value, BitVector<WIDTH>().fill_all_ones());
    }


    inline void set_tcam_start(const int start) {
      tcam_start_ = start;  
    }

    inline int tcam_lookup(const BitVector<WIDTH>& s0,
                           const BitVector<WIDTH>& s1) const {
      for (int i = 0; i < (int)N_ENTRIES; i++) {
        int index = tcam_start_ + i;
        if (index >= (int)N_ENTRIES)
          index -= N_ENTRIES;
        if (valid_[index] &&
            BitVector<WIDTH>::tcam_compare(word0s_[index], word1s_[index], s0, s1))
          return i;
      }
      return -1;
    }
    inline int tcam_lookup(const BitVector<WIDTH>& search1) {
      BitVector<WIDTH> search0(UINT64_C(0));
      search0.copy_from(search1);
      search0.invert();
      return tcam_lookup(search0, search1);
    }

    inline void print(const int index) const {
      word1s_[index].print();
      word0s_[index].print();
    }

    inline void reset() {
      for (int i = 0; i < (int)N_ENTRIES; i++) {
        valid_[i] = false;
        word1s_[i] = BitVector<WIDTH>().fill_all_zeros(); 
        word0s_[i] = BitVector<WIDTH>().fill_all_zeros(); 
      }
      tcam_start_ = 0;
    }
    
 private:
    int tcam_start_ = 0;
    std::array<bool,N_ENTRIES> valid_ { };
    std::array<BitVector<WIDTH>,N_ENTRIES> word1s_ { };
    std::array<BitVector<WIDTH>,N_ENTRIES> word0s_ { };
  };
}

#endif // _SHARED_TCAM2_

