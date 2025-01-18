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

#ifndef __HASH_ADDRESS_VH_XBAR_H__
#define __HASH_ADDRESS_VH_XBAR_H__

#include <array>
#include <cstdint>
#include <rmt-log.h>
#include <rmt-defs.h>
#include <rmt-object.h>
#include <bitvector.h>

namespace MODEL_CHIP_NAMESPACE {

class HashAddressVhXbar {
public:
  static constexpr int kSramAddressWidth   = MauDefs::kSramAddressWidth;
  static constexpr int kSramsPerRow        = MauDefs::kSramRowsPerMau;
  static constexpr int kSramsPerColumn     = MauDefs::kSramColumnsPerMau;
  static constexpr int kHashOutputWidth    = MauDefs::kHashOutputWidth;
  static constexpr int kMaskedEqualsWidth  = MauDefs::kMaskedEqualsWidth;

  HashAddressVhXbar() {};
  ~HashAddressVhXbar() {};

  void CalculateHashAddress(
      const MauHashGenerator::InputT& input,
      const MauHashGenerator::ValidT& input_valid,
      const int ram_hash_address_xbar_control,
      const bool ram_hash_address_xbar_enable,
      const std::array<int,4>& row_hash_address_xbar_control,
      const std::array<bool,4>& row_hash_address_xbar_enable,
      const bool  masked_equals_mux_control,
      const bool  masked_equals_mux_enable,
      const BitVector<kMaskedEqualsWidth>& masked_equals_value,
      const BitVector<kMaskedEqualsWidth>& masked_equals_mask,
      MauHashGeneratorWithReg* mau_hash_generator,
      const CacheId& cache_id,
      bool* address_valid,
      BitVector<kSramAddressWidth> *address ) const
  {
    RMT_ASSERT( ram_hash_address_xbar_control < 16 );
    RMT_ASSERT( row_hash_address_xbar_control[0] < 8 );
    RMT_ASSERT( row_hash_address_xbar_control[1] < 8 );

    if ( (!ram_hash_address_xbar_enable) ||
         (ram_hash_address_xbar_control >= 10)) {
      // codes 10->15 produce zero output
      *address_valid = false;
      address->fill_all_zeros();
      return;
    }

    // could cache the results here
    std::array<bool,4>  need_hash{};

    // work out which hash we need for the masked equals
    if ( masked_equals_mux_enable ) {
      for (int i=0;i<kMaskedEqualsWidth;++i) {
        if (masked_equals_mask.bit_set(i)) {
          need_hash[ masked_equals_mux_control ? 1:0 ] = true;
          break;
        }
      }
    }
    // work out which hash we need for the address
    if ( ram_hash_address_xbar_control < 5 ) {
      need_hash[0] = true;
    }
    else {
      need_hash[1] = true;
    }
      //need_hash[2] = true;
      //need_hash[3] = true;

    // calculate the needed hashes
    std::array<BitVector<kHashOutputWidth>,4> hash_outputs;
    for (int i=0; i<4; ++i) {
      if (need_hash[i]) {
        // assert: it seems to me, if the logic is programmed up to
        //  need a particular hash_output then the mux that selects which
        //  particular hash is chosen should be enabled.
        RMT_ASSERT( row_hash_address_xbar_enable[i] );
        mau_hash_generator->CalculateOutput(input,
                                            input_valid,
                                            row_hash_address_xbar_control[i], // group
                                            cache_id,
                                            &(hash_outputs[i]));
      }
    }

    BitVector<kMaskedEqualsWidth> top_bits;
    if ( masked_equals_mux_enable ) {
      hash_outputs[masked_equals_mux_control?1:0].extract_into(
          kHashOutputWidth-kMaskedEqualsWidth , &top_bits );
    }
    else {
      top_bits.fill_all_zeros();
    }
    *address_valid = top_bits.masked_equals( masked_equals_value, masked_equals_mask);

    int start_offset; 
    int which_hash;
    if ( ram_hash_address_xbar_control < 5 ) {
      start_offset = ram_hash_address_xbar_control * kSramAddressWidth;
      which_hash = 0;
    }
    else {
      start_offset = (ram_hash_address_xbar_control - 5) * kSramAddressWidth;
      which_hash = 1;
    }
    
    hash_outputs[which_hash].extract_into( start_offset, address );
  }


  void CalculateHash(
      const MauHashGenerator::InputT& input,
      const MauHashGenerator::ValidT& input_valid,
      const std::array<int,4>& row_hash_address_xbar_control,
      const std::array<bool,4>& row_hash_address_xbar_enable,
      const int which,
      MauHashGeneratorWithReg* mau_hash_generator,
      const CacheId& cache_id,
      BitVector<kHashOutputWidth> *hash ) const
  {
    RMT_ASSERT( which < 4 );

    if (row_hash_address_xbar_enable[which]) {
      mau_hash_generator->CalculateOutput(input,
                                          input_valid,
                                          row_hash_address_xbar_control[which], // group
                                          cache_id,
                                          hash);
    }
    else {
      hash->fill_all_zeros();
    }
  }
  
private:
  DISALLOW_COPY_AND_ASSIGN(HashAddressVhXbar);
};

}

#endif // __EXACT_MATCH_HASH_ADDRESS_VH_XBAR_H__
