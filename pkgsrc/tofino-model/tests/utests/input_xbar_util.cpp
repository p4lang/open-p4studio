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

#include <cstdint>
#include <utests/test_namespace.h>
#include <register_utils.h>
#include "input_xbar_util.h"
#include <model_core/model.h>
#include <register_includes/reg.h> // for with registers test
#include <mau-defs.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

namespace input_xbar_util {

using namespace MODEL_CHIP_NAMESPACE;


// set a particular output byte's source to a particular phv word. For 16 and 32 bit words you don't
//   get to choose which byte within the word, you just get the one the xbar gives you. And no
//   swizzling is done for the Ternary bytes (and of course there is no swizzler for the exact match
//   bytes) - for swizzling use one of the later functions
void set_byte_src(int chip,int pipe, int mau, int output_byte, bool enable, int which_phv_word ) {
  assert(output_byte<196);
  auto& xbar = RegisterUtils::ref_mau(pipe,mau).dp.xbar_hash.xbar;

  uint32_t word;

  // zero all the other registers to make sure we don't enable more than one ever
  for (int i=0;i<4;++i) {
    word=0;
    setp_mau_match_input_xbar_addrmap_match_input_xbar_32b_ctl_match_input_xbar_32b_ctl_lo_enable(&word,0);
    setp_mau_match_input_xbar_addrmap_match_input_xbar_32b_ctl_match_input_xbar_32b_ctl_hi_enable(&word,0);
    setp_mau_match_input_xbar_addrmap_match_input_xbar_32b_ctl_match_input_xbar_32b_ctl_address(&word,0);
    assert(i<4);
    GLOBAL_MODEL->OutWord( chip, &xbar.match_input_xbar_32b_ctl[i][output_byte],
                                    word );

    word=0;
    setp_mau_match_input_xbar_addrmap_match_input_xbar_816b_ctl_match_input_xbar_816b_ctl_enable(&word,0);
    setp_mau_match_input_xbar_addrmap_match_input_xbar_816b_ctl_match_input_xbar_816b_ctl_address(&word,0);
    assert(i<4);
    GLOBAL_MODEL->OutWord( chip, &xbar.match_input_xbar_816b_ctl[i][output_byte],
                                    word );
  }

  if (!enable)
    return;

  if (which_phv_word < 64) {
    // 32 bit word
    bool lo = which_phv_word < 32;
    int reg = lo ? (which_phv_word / 8) : ((which_phv_word-32) / 8);

    word=0;
    setp_mau_match_input_xbar_addrmap_match_input_xbar_32b_ctl_match_input_xbar_32b_ctl_lo_enable(&word, lo);
    setp_mau_match_input_xbar_addrmap_match_input_xbar_32b_ctl_match_input_xbar_32b_ctl_hi_enable(&word,!lo);

    setp_mau_match_input_xbar_addrmap_match_input_xbar_32b_ctl_match_input_xbar_32b_ctl_address(&word,
                                                                   which_phv_word % 8);
    assert(reg<4);
    GLOBAL_MODEL->OutWord( chip, &xbar.match_input_xbar_32b_ctl[reg][output_byte]  , word );

    // zero all the other registers to make sure disabling doesn't affect the enabled ones
    for (int i=0;i<4;++i) {
      // disable the other 32b ones
      if (i != reg ) {
        word=0;
        setp_mau_match_input_xbar_addrmap_match_input_xbar_32b_ctl_match_input_xbar_32b_ctl_lo_enable(&word,0);
        setp_mau_match_input_xbar_addrmap_match_input_xbar_32b_ctl_match_input_xbar_32b_ctl_hi_enable(&word,0);
        setp_mau_match_input_xbar_addrmap_match_input_xbar_32b_ctl_match_input_xbar_32b_ctl_address(&word,0);
        assert(i<4);
        GLOBAL_MODEL->OutWord( chip, &xbar.match_input_xbar_32b_ctl[i][output_byte],
                                        word );
      }
      // disable all the 816b
      word=0;
      setp_mau_match_input_xbar_addrmap_match_input_xbar_816b_ctl_match_input_xbar_816b_ctl_enable(&word,0);
      setp_mau_match_input_xbar_addrmap_match_input_xbar_816b_ctl_match_input_xbar_816b_ctl_address(&word,0);
      assert(i<4);
      GLOBAL_MODEL->OutWord( chip, &xbar.match_input_xbar_816b_ctl[i][output_byte],
                                      word );
    }
  }
  else {
    int reg;
    int code;
    word=0;
    setp_mau_match_input_xbar_addrmap_match_input_xbar_816b_ctl_match_input_xbar_816b_ctl_enable(&word,1);
    if (which_phv_word<128) {
      // 8 bit
      int which_8bit = which_phv_word - 64;
      bool lo = which_8bit < 32;
      reg = lo ? (which_8bit / 8) : ((which_8bit-32) / 8 );

      code = (which_8bit%8) + ( lo ? 0 : 8 );

    }
    else {
      // 16 bit
      int which_16bit = which_phv_word - 128;
      bool lo = which_16bit < 48;
      reg = lo ? (which_16bit / 12) : ((which_16bit-48) / 12 );

      code = 16 + (which_16bit%12) + ( lo ? 0 : 12 );
    }
    setp_mau_match_input_xbar_addrmap_match_input_xbar_816b_ctl_match_input_xbar_816b_ctl_address(&word,
                                                                                                  code);
    assert(reg<4);
    GLOBAL_MODEL->OutWord( chip, &xbar.match_input_xbar_816b_ctl[reg][output_byte] , word );

    // zero all the other registers to make sure disabling doesn't affect the enabled ones
    for (int i=0;i<4;++i) {
      // disable all the 32b ones
      word=0;
      setp_mau_match_input_xbar_addrmap_match_input_xbar_32b_ctl_match_input_xbar_32b_ctl_lo_enable(&word,0);
      setp_mau_match_input_xbar_addrmap_match_input_xbar_32b_ctl_match_input_xbar_32b_ctl_hi_enable(&word,0);
      setp_mau_match_input_xbar_addrmap_match_input_xbar_32b_ctl_match_input_xbar_32b_ctl_address(&word,0);
      assert(i<4);
      GLOBAL_MODEL->OutWord( chip, &xbar.match_input_xbar_32b_ctl[i][output_byte],
                                      word );
      // disable the other 816b ones
      if ( i != reg ) {
        word=0;
        setp_mau_match_input_xbar_addrmap_match_input_xbar_816b_ctl_match_input_xbar_816b_ctl_enable(&word,0);
        setp_mau_match_input_xbar_addrmap_match_input_xbar_816b_ctl_match_input_xbar_816b_ctl_address(&word,0);
        assert(i<4);
        GLOBAL_MODEL->OutWord( chip, &xbar.match_input_xbar_816b_ctl[i][output_byte],
                                        word );
      }
    }

  }
}

// Sets a four consecutive output bytes to output a particular 32 bit phv word. If the output bytes are
//  in the ternary part it also sets the ternary swizzlers to swizzle the bytes back into the correct
//  order.
// Probably won't work if the bytes straddle the exact match and ternary parts.
void set_32_bit_word_src(int chip,int pipe, int mau, int start_output_byte, bool enable, int which_phv_word ) {
  assert(which_phv_word < 64); // must be called with a 32 bit word
  assert((start_output_byte+3)<MauDefs::kTotalMatchBytes); // check last of the 4 bytes are not off the end
  for (int b=0;b<4;++b) {
    int required_byte = start_output_byte + b;
    // for the tcam words we can set the unswizzlers to get the word back in the correct order
    int ternary_byte = required_byte - MauDefs::kTernaryFirstByte;
    if ( ternary_byte >= 0 ) {
      int ctl_reg   = ternary_byte / 4;
      int ctl_shift = (ternary_byte % 4) * 2;
      uint32_t ctl_mask  = 3 << ctl_shift;
      auto& ternary_swizzle_ctl = RegisterUtils::ref_mau(pipe,mau).dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[ctl_reg];
      uint32_t before = GLOBAL_MODEL->InWord(chip,&ternary_swizzle_ctl);
      uint32_t new_value = before & ~ctl_mask;
      if ( enable ) {
        int p = ternary_byte % 4;
        uint32_t reg_value = (b-p)%4; // from csr comments
        new_value |= reg_value << ctl_shift;
        GLOBAL_MODEL->OutWord(chip,&ternary_swizzle_ctl,new_value);
      }
      else {
        GLOBAL_MODEL->OutWord(chip,&ternary_swizzle_ctl, new_value);
      }
    }
    set_byte_src(chip,pipe,mau,required_byte,enable,which_phv_word);
  }
}

// Sets two consecutive output bytes to output a particular 16 bit phv word. If the output bytes are
//  in the ternary part it also sets the ternary swizzlers to swizzle the bytes back into the correct
//  order.
// Probably won't work if the bytes straddle the exact match and ternary parts.
void set_16_bit_word_src(int chip,int pipe, int mau, int start_output_byte, bool enable, int which_phv_word ) {
  assert(which_phv_word >= 128); // must be called with a 16 bit word
  assert((start_output_byte+1)<MauDefs::kTotalMatchBytes); // check second byte is not off the end
  for (int b=0;b<2;++b) {
    int required_byte = start_output_byte + b;
    // for the tcam words we can set the unswizzlers to get the word back in the correct order
    int ternary_byte = required_byte - MauDefs::kTernaryFirstByte;
    int ctl_reg   = ternary_byte / 4;
    int ctl_shift = (ternary_byte % 4) * 2;
    uint32_t ctl_mask  = 3 << ctl_shift;
    auto& ternary_swizzle_ctl = RegisterUtils::ref_mau(pipe,mau).dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[ctl_reg];
    uint32_t before = GLOBAL_MODEL->InWord(chip,&ternary_swizzle_ctl);
    uint32_t new_value = before & ~ctl_mask;
    if ( ternary_byte >= 0 ) {
      if ( enable ) {
        int p = ternary_byte % 4;
        // truth table boils down to: If p+b is even then set zero. Otherwise if b=1 set 3 else set 1
        uint32_t reg_value = (((b+p)%2)==0) ? 0 : ( b ? 3 : 1 );
        new_value |= reg_value << ctl_shift;
        GLOBAL_MODEL->OutWord(chip,&ternary_swizzle_ctl,new_value);
      }
      else {
        GLOBAL_MODEL->OutWord(chip,&ternary_swizzle_ctl,new_value);
      }
    }
    set_byte_src(chip,pipe,mau,required_byte,enable,which_phv_word);
  }
}

}
}
