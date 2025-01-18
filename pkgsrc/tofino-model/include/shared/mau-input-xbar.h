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

#ifndef __MAU_INPUT_XBAR_H_
#define __MAU_INPUT_XBAR_H_

#include <array>
#include <cstdint>
#include <rmt-log.h>
#include <rmt-defs.h>
#include <rmt-object.h>
#include <bitvector.h>
#include <phv.h>

namespace MODEL_CHIP_NAMESPACE {

class MauInputXbar {
public:
  static constexpr int kExactMatchBytes   = MauDefs::kExactMatchBytes;
  // round up ternary match bytes to whole number of words (4 bytes) as swizzling is word based
  static constexpr int kTernaryPreSwizzleMatchBytes = (MauDefs::kTernaryMatchBytes+3) & ~3;
  static constexpr int kTotalPreSwizzleMatchBytes   = kExactMatchBytes + kTernaryPreSwizzleMatchBytes;
  
  MauInputXbar() {};
  ~MauInputXbar() {};

  void CalculateOutputByte(
      const Phv& phv,
      const int  which_phv_word,
      const int  which_output_byte,
      const bool phv_word_din_en,
      uint8_t *output_byte,
      bool    *output_valid ) {
    *output_valid = phv.is_valid(which_phv_word);
    if ((*output_valid) || (which_output_byte < kExactMatchBytes)) {
      *output_byte = phv_word_din_en ?phv.get_byte(which_phv_word,which_output_byte) :0;
    }
    else {
      // tcam does not zero invalid outputs, instead it leaves them
      //  unchanged to save power, might as well copy that.
      *output_byte = last_ternary_bytes_[which_output_byte-kExactMatchBytes];
    }
    if (which_output_byte >= kExactMatchBytes) {
      last_ternary_bytes_[which_output_byte-kExactMatchBytes] = *output_byte;
    }
  }

private:
  std::array<uint8_t,kTernaryPreSwizzleMatchBytes> last_ternary_bytes_;


};

}

#endif // __MAU_INPUT_XBAR_H_
