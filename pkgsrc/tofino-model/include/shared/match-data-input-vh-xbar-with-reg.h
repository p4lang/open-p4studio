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

#ifndef __MATCH_DATA_INPUT_VH_XBAR_WITH_REG_H_
#define __MATCH_DATA_INPUT_VH_XBAR_WITH_REG_H_

#include <array>
#include <cstdint>
#include <rmt-log.h>
#include <rmt-defs.h>
#include <rmt-object.h>
#include <bitvector.h>
#include <mau-hash-generator-with-reg.h>
#include <match-data-input-vh-xbar.h>
#include <cache-id.h>
#include <register_includes/exactmatch_row_vh_xbar_ctl.h>
// ByteSwizzle moved up in hierarchy in regs_25957_mau_dev so now array3 NOT array2
#include <register_includes/exactmatch_row_vh_xbar_byteswizzle_ctl_array3.h>
#include <register_includes/stateful_meter_alu_data_ctl.h>

namespace MODEL_CHIP_NAMESPACE {

class MatchDataInputVhXbarWithReg {
public:
  static constexpr int kExactMatchInputBits  = MatchDataInputVhXbar::kExactMatchInputBits;
  static constexpr int kExactMatchInputBytes = MatchDataInputVhXbar::kExactMatchInputBytes;
  static constexpr int kVersionDataWidth     = MatchDataInputVhXbar::kVersionDataWidth;
  static constexpr int kOutputBits           = MatchDataInputVhXbar::kOutputBits;
  static constexpr int kOutputBytes          = MatchDataInputVhXbar::kOutputBytes;
  static constexpr int kVersionPositions     = MatchDataInputVhXbar::kVersionPositions;
  static constexpr int kStatefulMeterAluDataBits  = MauDefs::kStatefulMeterAluDataBits;
  static constexpr int kStatefulMeterAluDataBytes = MauDefs::kStatefulMeterAluDataBytes;

  MatchDataInputVhXbarWithReg( int chip, int pipe, int mau, int row, int bus,
                               MauHashGeneratorWithReg* mau_hash_generator );
  ~MatchDataInputVhXbarWithReg() {};

  /** Calculate a bit vector of a particular search bus
   *
   */
  void CalculateSearchData(
      const BitVector<kExactMatchInputBits>  & input,
      const BitVector<kExactMatchInputBytes> & input_valid,
      const BitVector<kVersionDataWidth>& ingress_version_bits,
      const BitVector<kVersionDataWidth>& egress_version_bits,
      const CacheId& cache_id,
      BitVector<MatchDataInputVhXbar::kOutputBits> *search_bus );

  /** Calculate the 64 bits that get ored on the row before heading for the
   *    Meter/Stateful/Selector ALUs
   */
  BitVector<kStatefulMeterAluDataBits> CalculateStatefulMeterAluData(
      const BitVector<kExactMatchInputBits>  & input );

private:
  int                           bus_;
  bool                          byte_swizzle_changed_;
  MauHashGeneratorWithReg*      mau_hash_generator_;
  const MatchDataInputVhXbar    match_data_input_vh_xbar_;

  register_classes::ExactmatchRowVhXbarByteswizzleCtlArray3 byte_swizzle_;
  register_classes::ExactmatchRowVhXbarCtl                  xbar_control_;
  register_classes::StatefulMeterAluDataCtl                 stateful_meter_alu_data_ctl_;


  bool ByteSwizzleVerifyReplication() {
    // Byteswizzle is not actually replicated - this func NOT called
    if (!byte_swizzle_changed_) return true;
    for (int a1 = 0; a1 < kOutputBytes; a1++) {
      uint32_t v0 = byte_swizzle_.exactmatch_row_vh_xbar_byteswizzle_ctl(bus_,a1,0);
      int sel0, sel, en0, en, v_en0, v_en;
      if      (v0 <  8)  { sel0 =     0; en0 = 0; v_en0 = 0; }
      else if (v0 < 16)  { sel0 =     0; en0 = 0; v_en0 = 1; }
      else if (v0 < 32)  { sel0 = v0-16; en0 = 1; v_en0 = 0; }
      else               { sel0 =     0; en0 = 0; v_en0 = 0; }
      for (int a0 = 1; a0 < 8; a0++) {
        uint32_t v = byte_swizzle_.exactmatch_row_vh_xbar_byteswizzle_ctl(bus_,a1,a0);
        if      (v <  8) { sel  =     0; en  = 0; v_en  = 0; }
        else if (v < 16) { sel  =     0; en  = 0; v_en  = 1; }
        else if (v < 32) { sel  =  v-16; en  = 1; v_en  = 0; }
        else             { sel  =     0; en  = 0; v_en  = 0; }
        if ((sel != sel0) || (en != en0) || (v_en != v_en0))
          return false;
      }
    }
    byte_swizzle_changed_ = false;
    return true;
  }


  void ByteSwizzleCallback(uint32_t a2,uint32_t a1,uint32_t a0) {

    // ByteSwizzle moved up in hierarchy in regs_25957_mau_dev so now array3 NOT array2.
    // So instantiate twice (once in each VhXbar) but ignore callback if not for correct bus
    if (a2 != static_cast<uint32_t>(bus_)) return;

    byte_swizzle_changed_ = true; // Only verify replication on change

    RMT_ASSERT(a0<8); // bit number
    RMT_ASSERT(a1 < kOutputBytes);

    uint32_t v = byte_swizzle_.exactmatch_row_vh_xbar_byteswizzle_ctl (a2,a1,a0);
    // decode the value
    if ( v < 8 ) {
      byte_swizzle_select_[a1][a0]=0;
      byte_swizzle_enable_[a1][a0]=false;
      byte_swizzle_version_enable_[a1][a0]=false;
    }
    else if (v<16) {
      byte_swizzle_select_[a1][a0]=0;
      byte_swizzle_enable_[a1][a0]=false;
      byte_swizzle_version_enable_[a1][a0]=true;
    }
    else {
      RMT_ASSERT(v<32);
      byte_swizzle_select_[a1][a0]= v-16;
      byte_swizzle_enable_[a1][a0]=true;
      byte_swizzle_version_enable_[a1][a0]=false;
    }
  }

  void StatefulMeterAluDataCtlCallback() {
    int byte_mask = stateful_meter_alu_data_ctl_.stateful_meter_alu_data_bytemask();
    stateful_meter_alu_data_mask_.fill_all_zeros();
    for (int b=0;b<kStatefulMeterAluDataBytes;++b) {
      if ( byte_mask & ( 1<<b ) ) {
        stateful_meter_alu_data_mask_.set_byte(0xFF,b);
      }
    }
  }

  MatchDataInputVhXbar::ByteControlT      byte_swizzle_select_{};
  MatchDataInputVhXbar::ByteControlBoolT  byte_swizzle_enable_{};
  MatchDataInputVhXbar::ByteControlBoolT  byte_swizzle_version_enable_{};

  BitVector<kStatefulMeterAluDataBits> stateful_meter_alu_data_mask_{};

private:
  DISALLOW_COPY_AND_ASSIGN(MatchDataInputVhXbarWithReg);
};

}

#endif // __MATCH_DATA_INPUT_VH_XBAR_WITH_REG_H_
