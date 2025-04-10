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

#include <common/rmt-util.h>
#include <rmt-log.h>
#include <match-data-input-vh-xbar-with-reg.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

MatchDataInputVhXbarWithReg::MatchDataInputVhXbarWithReg(
    int chip, int pipe, int mau, int row, int bus,
    MauHashGeneratorWithReg* mau_hash_generator ) :
    bus_(bus), byte_swizzle_changed_(false), mau_hash_generator_(mau_hash_generator),
    // ByteSwizzle moved up in hierarchy in regs_25957_mau_dev so now array3 NOT array2.
    // So instantiate twice (once in each VhXbar) but ignore callback if not for correct bus
    byte_swizzle_(default_adapter(byte_swizzle_,chip,pipe,mau,row,
                  [this](uint32_t a2,uint32_t a1,uint32_t a0){this->ByteSwizzleCallback(a2,a1,a0); })),
    xbar_control_(default_adapter(xbar_control_,chip,pipe,mau,row,bus)),
    stateful_meter_alu_data_ctl_(default_adapter(stateful_meter_alu_data_ctl_, chip,pipe,mau,row,bus,
                                  [this](){this->StatefulMeterAluDataCtlCallback(); }))
{
  byte_swizzle_.reset();
  xbar_control_.reset();
}

void MatchDataInputVhXbarWithReg::CalculateSearchData(
      const BitVector<kExactMatchInputBits>  & input,
      const BitVector<kExactMatchInputBytes> & input_valid,
      const BitVector<kVersionDataWidth>& ingress_version_bits,
      const BitVector<kVersionDataWidth>& egress_version_bits,
      const CacheId& cache_id,
      BitVector<kOutputBits> *search_bus ) {

  //  could cache the seach bus too
  match_data_input_vh_xbar_.CalculateSearchData(
      xbar_control_.exactmatch_row_vh_xbar_enable(),
      input,
      input_valid,
      ingress_version_bits,
      egress_version_bits,
      xbar_control_.exactmatch_row_vh_xbar_thread(),
      xbar_control_.exactmatch_row_vh_xbar_select(),
      byte_swizzle_select_,
      byte_swizzle_enable_,
      byte_swizzle_version_enable_,
      mau_hash_generator_,
      cache_id,
      search_bus );
}

BitVector<MatchDataInputVhXbarWithReg::kStatefulMeterAluDataBits> MatchDataInputVhXbarWithReg::CalculateStatefulMeterAluData(
    const BitVector<kExactMatchInputBits>  & input )
{
  BitVector<kStatefulMeterAluDataBits> data{};
  int group   = 0x7 & stateful_meter_alu_data_ctl_.stateful_meter_alu_data_xbar_ctl();
  bool enable = (0x8 & stateful_meter_alu_data_ctl_.stateful_meter_alu_data_xbar_ctl()) != 0;
  if ( enable ) {
    if ( kStatefulMeterAluDataBits == 64 ) {
      // Tofino: take the 64 msbs in the chosen 128 bit group
      data.set_word( input.get_word( (group * 128) + 64 ), 0 );
    }
    else {
      input.extract_into( group*128, &data );
    }
  }
  data.mask( stateful_meter_alu_data_mask_ );
  return data;
}

}
