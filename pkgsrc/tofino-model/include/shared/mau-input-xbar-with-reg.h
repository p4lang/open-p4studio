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

#ifndef _SHARED_MAU_INPUT_XBAR_WITH_REG_H_
#define _SHARED_MAU_INPUT_XBAR_WITH_REG_H_

#include <array>
#include <cstdint>
#include <mau-input-xbar.h>

#include <register_includes/tcam_byte_swizzle_ctl_array.h>
#include <register_includes/match_input_xbar_32b_ctl_array2.h>
#include <register_includes/match_input_xbar_816b_ctl_array2.h>

#include <register_includes/match_input_xbar_32b_ctl_array2.h>
#include <register_includes/match_input_xbar_816b_ctl_array2.h>
#include <register_includes/tcam_byte_swizzle_ctl_array.h>


namespace MODEL_CHIP_NAMESPACE {

class Mau;

class MauInputXbarWithReg {
public:
  // there are eight datapaths that feed every byte, each controlled
  //  by one regsiter. Only one datapath should be enabled at a time
  static constexpr int  kDatapaths = 8;
  static constexpr int  kOutputBytes = MauDefs::kExactMatchBytes + MauDefs::kTernaryMatchBytes;
  static constexpr int  kOutputBits  = kOutputBytes * 8;

  static constexpr int  kTernaryMatchBytes = MauDefs::kTernaryMatchBytes;
  static constexpr int  kTernaryFirstByte  = MauDefs::kTernaryFirstByte;
  static constexpr int  kTernaryPreSwizzleMatchBytes = MauInputXbar::kTernaryPreSwizzleMatchBytes;

  MauInputXbarWithReg( int chip, int pipe, int mau, Mau *mauobj );

  ~MauInputXbarWithReg() {};

  /** Calculate all the output and place data and valids in bit vectors
   */
  void CalculateOutput(
      const Phv& phv,
      BitVector< kOutputBits >& output,
      BitVector< kOutputBytes >& output_valid );

  /** Check whether phvWord available to Xbar
   */
  bool PhvWordAvailable(int phvWord);

  /** Calculate one output byte, use this for efficiency if you only want some bytes
   */
  void CalculateOutputByte(
      const Phv& phv,
      const int which_output_byte,
      uint8_t *output_byte,
      bool    *output_valid );

private:
  void Ctl32bCallback(uint32_t datapath,uint32_t which_output_byte);
  void Ctl816bCallback(uint32_t datapath,uint32_t which_output_byte);

  std::array<bool, MauInputXbar::kTotalPreSwizzleMatchBytes>  byte_enables_;
  std::array<int,  MauInputXbar::kTotalPreSwizzleMatchBytes>  byte_selectors_;

  static_assert( kDatapaths <= 8, "datapath_enable_check_ is a uint8_t can only handle 8 datapaths");
  std::array<uint8_t, MauInputXbar::kTotalPreSwizzleMatchBytes>  datapath_enable_check_;

  register_classes::MatchInputXbar_32bCtlArray2   ctl_32b_;
  register_classes::MatchInputXbar_816bCtlArray2  ctl_8b_16b_;
  register_classes::TcamByteSwizzleCtlArray       byte_swizzle_ctl_array_;

  Mau *mauobj_;
  MauInputXbar input_xbar_;
  bool check_data_path_=false;

  // swizzle_offsets_[ output_byte % 4 ][ control_reg_value ]
  const std::array<std::array<int,4>, 4> swizzle_offsets_{{
      {{ 0,  1, -2, -1 }},
      {{ 0,  1, -2,  3 }},
      {{ 0, -3,  2, -1 }},
      {{ 0,  1,  2, -1 }}
    }};



  void UpdateDataPathEnableCheck(uint32_t which_output_byte,uint32_t datapath,bool enable) {
    check_data_path_ = true;

    if (enable) {
      datapath_enable_check_[which_output_byte] |= 1<<datapath;
    }
    else {
      datapath_enable_check_[which_output_byte] &= ~(1<<datapath);
    }
  }

  bool DoDataPathEnableCheck() {
    if ( check_data_path_ ) {
      check_data_path_=false;
      for (int i=0; i< MauInputXbar::kTotalPreSwizzleMatchBytes; ++i) {
        int x = datapath_enable_check_[i];
        // it's ok if x == 0  or x only has one bit set (is a power of 2)
        if ( !((x == 0) || !(x & (x - 1))) ) {
          return false;
        }
      }
    }
    return true;
  }

};

}

#endif // _SHARED_MAU_INPUT_XBAR_WITH_REG_H_
