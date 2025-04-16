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

#ifndef _SHARED_ACTION_OUTPUT_HV_XBAR__
#define _SHARED_ACTION_OUTPUT_HV_XBAR__

#include <array>
#include <cstdint>
#include <string>
#include <register_includes/action_hv_ixbar_ctl_byte_array.h>
#include <register_includes/action_hv_ixbar_ctl_halfword_array2.h>
#include <register_includes/action_hv_ixbar_ctl_word_array2.h>
#include <register_includes/action_hv_ixbar_input_bytemask_array.h>
#include <bitvector.h>

namespace MODEL_CHIP_NAMESPACE {

class ActionOutputHvXbar {
public:
  static constexpr int  kBytesIn  = 16;
  static constexpr int  kBytesOut = 128;
  static bool kBugActionDataBusMaskBeforeDup; // Defined in rmt-config.cpp


  ActionOutputHvXbar( int chip, int pipe, int mau, int row );
  virtual ~ActionOutputHvXbar();


  // Or n_bytes from the input into the output (at a given n_byte chunk in a given
  //   16 byte output slice)
  // Also applies the input byte mask
  void MapBytes( int n_bytes, int from_start_byte,int slice, int to_chunk,
                 const uint32_t input_bytemask,
                 const BitVector< kBytesIn * 8 >& input,
                 BitVector< kBytesOut *8 >* output) {
    int to_start = (16 * slice) + (to_chunk * n_bytes);
    for (int i=0;i<n_bytes;++i) {
      int from_byte = from_start_byte + i;
      if ( (input_bytemask >> from_byte) & 1) {
        int to_byte = to_start + i;
        RMT_ASSERT(from_byte < kBytesIn);
        RMT_ASSERT(to_byte < kBytesOut);
        output->set_byte( input.get_byte( from_byte ) | output->get_byte( to_byte ),
                          to_byte );
      }
    }
  }


  // just for testing
  std::string MakePatsPicture();


private:
  void CalculateOutputNormal(
      const BitVector< kBytesIn * 8 >& input,
      int input_bus, int input_bitwidth, uint32_t input_bytemask,
      BitVector< kBytesOut *8 >* output);

  void CalculateOutputDupMask(
      const BitVector< kBytesIn * 8 >& input,
      int input_bus, int input_bitwidth, uint32_t input_bytemask,
      BitVector< kBytesOut *8 >* output);

public:
  // Calculates a new output by taking the input, feeding it into the xbar
  //   then ORing the result into the output bitvector
  void CalculateOutput(
      const BitVector< kBytesIn * 8 >& input,
      int input_bus, int input_bitwidth,
      BitVector< kBytesOut *8 >* output);



private:
  register_classes::ActionHvIxbarCtlByteArray       byte_control_regs_;
  register_classes::ActionHvIxbarCtlHalfwordArray2  halfword_control_regs_;
  register_classes::ActionHvIxbarCtlWordArray2      word_control_regs_;
  register_classes::ActionHvIxbarInputBytemaskArray bytemask_regs_;

private:
  DISALLOW_COPY_AND_ASSIGN(ActionOutputHvXbar);
};

}

#endif // _SHARED_ACTION_OUTPUT_HV_XBAR__

