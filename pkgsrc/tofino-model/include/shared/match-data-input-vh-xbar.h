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

#ifndef _SHARED_MATCH_DATA_INPUT_VH_XBAR_H_
#define _SHARED_MATCH_DATA_INPUT_VH_XBAR_H_

#include <array>
#include <cstdint>
#include <rmt-log.h>
#include <rmt-defs.h>
#include <rmt-object.h>
#include <bitvector.h>
#include <mau-hash-generator-with-reg.h>

namespace MODEL_CHIP_NAMESPACE {

class MatchDataInputVhXbar {
public:
  static constexpr int kExactMatchInputBits  = MauDefs::kExactMatchBytes*8;
  static constexpr int kExactMatchInputBytes = MauDefs::kExactMatchBytes;
  static constexpr int kOutputBits           = kExactMatchInputBits / 8;
  static constexpr int kOutputBytes          = kOutputBits / 8;
  // for exact match you need to store twice as many version bits as
  //   on a ternary match
  static constexpr int kVersionDataWidth     = MauDefs::kVersionBits * 2;
  static constexpr int kVersionPositions     = kOutputBits / kVersionDataWidth;

  using ByteControlT      = std::array< std::array<int,8> , kOutputBytes >;
  using ByteControlBoolT  = std::array< std::array<bool,8> , kOutputBytes >;

  MatchDataInputVhXbar() {};
  ~MatchDataInputVhXbar() {};

  void CalculateSearchData(
      bool xbar_enabled,
      const BitVector<kExactMatchInputBits>&  input,
      const BitVector<kExactMatchInputBytes>& input_valid,
      const BitVector<kVersionDataWidth>& ingress_version_bits,
      const BitVector<kVersionDataWidth>& egress_version_bits,
      const bool use_egress_version_bits,
      const int mux_control,
      const ByteControlT&      byte_swizzle_select,
      const ByteControlBoolT&  byte_swizzle_enable,
      const ByteControlBoolT&  byte_swizzle_version_enable,
      MauHashGeneratorWithReg* mau_hash_generator,
      const CacheId& cache_id,
      BitVector<kOutputBits> *search_data ) const
  {
    RMT_ASSERT( mux_control>=0 && mux_control<16 );
    if (xbar_enabled) {
      // the first mux
      BitVector<kOutputBits> mux_output {};
      if ( mux_control < 8 ) {
        input.extract_into( mux_control*128, &mux_output );
      }
      else {
        // only calculate the one hash output we need
        BitVector<MauDefs::kHashOutputWidth> hash_output;
        mau_hash_generator->CalculateOutput(input,
                                            input_valid,
                                            mux_control-8, // group
                                            cache_id,
                                            &hash_output);
        // mux output's high bits are already 0 so just set low bits
        mux_output.set_from(0,hash_output);
      }
      // byte swizzle - control is per bit
      for (int i=0;i<kOutputBytes;++i) {
        for (int b=0;b<8;++b) {
          bool out_bit;
          if ( byte_swizzle_version_enable[i][b] ) {
            if (use_egress_version_bits)
              out_bit = egress_version_bits.get_bit(b%4);
            else
              out_bit = ingress_version_bits.get_bit(b%4);
          }
          else if ( byte_swizzle_enable[i][b] ) {
            int in_byte = byte_swizzle_select[i][b];
            out_bit = mux_output.get_bit( (in_byte*8) + b );
          }
          else {
            out_bit = false;
          }
          search_data->set_bit( out_bit, (i*8) + b );
        }
      }
    }
    else {
      search_data->fill_all_zeros();
    }
  }

private:
  DISALLOW_COPY_AND_ASSIGN(MatchDataInputVhXbar);
};

}

#endif // _SHARED_MATCH_DATA_INPUT_VH_XBAR_H_
