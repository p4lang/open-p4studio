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

#ifndef _SHARED_TCAM_
#define _SHARED_TCAM_

#include <iostream>
#include <string>
#include <array>
#include <bitvector.h>

namespace MODEL_CHIP_NAMESPACE {

  template <size_t N_ENTRIES, size_t WIDTH>
      class Tcam {

 public:
    Tcam() {}
    ~Tcam() {}

    inline bool is_valid(const int index) const {
      return valid_[index];
    }
    inline void get(const int index,
                    BitVector<WIDTH> *bv_val, BitVector<WIDTH> *bv_mask) const {
      if (bv_val != NULL)
        *bv_val = values_[index];
      if (bv_mask != NULL)
        *bv_mask = masks_[index];
    }
    inline void set(const int index,
                    const BitVector<WIDTH> &bv_val, const BitVector<WIDTH> &bv_mask,
                    bool valid=true) {
      values_[index] = bv_val; 
      masks_[index] = bv_mask;
      valid_[index] = valid;
    }
    inline void set(const int index, const BitVector<WIDTH> &bv_val) {
      set(index, bv_val, BitVector<WIDTH>().fill_all_ones());
    }
    inline void set_tcam_start(const int start) {
      tcam_start_ = start;  
    }
    // first match wins as stands 
    inline int value_lookup(const BitVector<WIDTH>& bv) const {
      for (int i = 0; i < (int)N_ENTRIES; i++) {
        int index = tcam_start_ + i;
        if (index >= (int)N_ENTRIES)
          index -= N_ENTRIES;
        if (valid_[index] && bv.equals(values_[index]))
          return i;
      }
      return -1;
    }  
    inline int lookup(const BitVector<WIDTH>& bv) const {
      for (int i = 0; i < (int)N_ENTRIES; i++) {
        int index = tcam_start_ + i;
        if (index >= (int)N_ENTRIES)
          index -= N_ENTRIES;
        if (valid_[index] && bv.masked_equals(values_[index], masks_[index]))
          return i;
      }
      return -1;
    }
    inline int tcam_lookup(const BitVector<WIDTH>& s0,
                           const BitVector<WIDTH>& s1) const {
      for (int i = 0; i < (int)N_ENTRIES; i++) {
        int index = tcam_start_ + i;
        if (index >= (int)N_ENTRIES)
          index -= N_ENTRIES;
        if (valid_[index] &&
            BitVector<WIDTH>::tcam_compare(values_[index], masks_[index], s0, s1))
          return i;
      }
      return -1;
    }
    inline void print(const int index) const {
      values_[index].print();
      masks_[index].print();
    }
    inline void reset() {
      for (int i = 0; i < (int)N_ENTRIES; i++) {
        valid_[i] = false;
        values_[i] = BitVector<WIDTH>().fill_all_ones(); 
        masks_[i] = BitVector<WIDTH>().fill_all_ones(); 
      }
      tcam_start_ = 0;
    }
    
 private:
    int tcam_start_ = 0;
    std::array<bool,N_ENTRIES> valid_ { };
    std::array<BitVector<WIDTH>,N_ENTRIES> values_ { };
    std::array<BitVector<WIDTH>,N_ENTRIES> masks_ { };
  };
}

#endif // _SHARED_TCAM_

