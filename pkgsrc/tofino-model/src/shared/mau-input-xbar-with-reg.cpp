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

#include <mau.h>
#include <rmt-log.h>
#include <mau-input-xbar-with-reg.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

MauInputXbarWithReg::MauInputXbarWithReg( int chip, int pipe, int mau, Mau *mauobj ) :
    datapath_enable_check_(),
    ctl_32b_(default_adapter(ctl_32b_,chip,pipe,mau,
             [this](uint32_t a1,uint32_t a0){this->Ctl32bCallback(a1,a0);})),
    ctl_8b_16b_(default_adapter(ctl_8b_16b_,chip,pipe,mau,
                [this](uint32_t a1,uint32_t a0){this->Ctl816bCallback(a1,a0);})),
    byte_swizzle_ctl_array_(default_adapter(byte_swizzle_ctl_array_,chip,pipe,mau)),
    mauobj_(mauobj)
{
  ctl_32b_.reset();
  ctl_8b_16b_.reset();
  byte_swizzle_ctl_array_.reset();
}

void MauInputXbarWithReg::CalculateOutput(
    const Phv& phv,
    BitVector< kOutputBits >& output,
    BitVector< kOutputBytes >& output_valid ) {
  RMT_ASSERT( DoDataPathEnableCheck() );
  uint8_t byte;
  bool    valid;
  for (int i=0; i<kOutputBytes; ++i) {
    int unswizzled = i;
    int ternary_byte = i - kTernaryFirstByte;
    if ( ternary_byte >= 0 && ternary_byte < kTernaryMatchBytes) {

      // tcam_byte_swizzle_ctl 17 word x 8bit array can be considered a 68 word 2b array
      //  get the 2 bits that correspond to this word
      int which_word  = ternary_byte / 4;
      int which_chunk = ternary_byte % 4;
      auto ctl = 3 & (byte_swizzle_ctl_array_.tcam_byte_swizzle_ctl(which_word) >> (which_chunk*2));

      unswizzled += swizzle_offsets_[ ternary_byte%4 ][ ctl ];
      if ( unswizzled < kTernaryFirstByte ||
           unswizzled >= (kTernaryFirstByte+kTernaryPreSwizzleMatchBytes) ) {
        unswizzled = -1;
      }
    }

    if (unswizzled >= 0) {
      CalculateOutputByte(phv,unswizzled, &byte, &valid );
    }
    else {
      byte=0;
      valid=false;
    }
    output.set_byte( byte, i );
    output_valid.set_bit( valid, i );
  }

}

void MauInputXbarWithReg::Ctl32bCallback(uint32_t datapath,uint32_t which_output_byte) {
  RMT_ASSERT(which_output_byte < MauInputXbar::kTotalPreSwizzleMatchBytes);
  int address = ctl_32b_.match_input_xbar_32b_ctl_address(datapath,which_output_byte);
  bool enable_lo = ctl_32b_.match_input_xbar_32b_ctl_lo_enable(datapath,which_output_byte);
  bool enable_hi = ctl_32b_.match_input_xbar_32b_ctl_hi_enable(datapath,which_output_byte);
  //printf("which_output_byte=%d address=%08x enable=%d\n",which_output_byte,address,enable);
  RMT_ASSERT( datapath < kDatapaths );
  RMT_ASSERT( ! ( enable_hi && enable_lo ) ); // not both enabled
  UpdateDataPathEnableCheck(which_output_byte,datapath,enable_lo || enable_hi);
  // Array element 0 controls inputting of words 0-7,  32-39 via codes 0-7, enable bits 0,1.
  // Array element 1 controls inputting of words 8-15, 40-47 via codes 0-7, enable bits 0,1.
  if (enable_lo || enable_hi) {
    byte_selectors_[which_output_byte] = (datapath * 8) + address + ( enable_hi ? 32 : 0 );
  }
  byte_enables_[which_output_byte]   = datapath_enable_check_[which_output_byte] != 0;

}

void MauInputXbarWithReg::Ctl816bCallback(uint32_t datapath,uint32_t which_output_byte) {
  RMT_ASSERT(which_output_byte < MauInputXbar::kTotalPreSwizzleMatchBytes);
  int address = ctl_8b_16b_.match_input_xbar_816b_ctl_address(datapath,which_output_byte);
  bool enable = ctl_8b_16b_.match_input_xbar_816b_ctl_enable(datapath,which_output_byte);

  RMT_ASSERT( (datapath + 4) < kDatapaths );
  // register [0] controls datapath 4 and so on...
  UpdateDataPathEnableCheck(which_output_byte,datapath+4,enable);

  if (enable) {
    if ( address <= 15 ) { // 8 bit phv words
      // Array element 0 controls inputting of 8b words 0-7, 32-39 via codes 0-15
      // Array element 1 controls inputting of 8b words 8-15,40-47 via codes 0-15 ...
      //  (64 is the first 8bit phv word)
      if ( address <= 7 ) {
        byte_selectors_[which_output_byte] = 64 + (datapath*8) + address;
      }
      else {
        byte_selectors_[which_output_byte] = 64 + (datapath*8) + (address - 8) + 32;
      }

    }
    else { // 16 bit phv words
      // Array element 0 16b words 0-11, 48-59 via codes 16-27 and 28-39
      // Array element 1 16b words 12-23,60-71 via codes 16-27 and 28-39 ...
      //  (128 is the first 8bit phv word)
      if ( address <= 27 ) {
        byte_selectors_[which_output_byte] = 128 + (datapath*12) + (address-16);
      }
      else {
        byte_selectors_[which_output_byte] = 128 + (datapath*12) + (address-28) + 48;
      }
    }
  }
  byte_enables_[which_output_byte]   = datapath_enable_check_[which_output_byte] != 0;
}

bool MauInputXbarWithReg::PhvWordAvailable(int phvWord) {
  RMT_ASSERT((phvWord >= 0) && (phvWord < Phv::kWordsMax));
  int gress = mauobj_->phv_get_gress(phvWord);
  if ( ! ((gress == 0) || (gress == 1)) ) return false; // Not in either thread

  if (RmtObject::is_jbay_or_later()) {
    // Check NOT a DarkPHV word - these never available to MatchInputXbar
    RMT_ASSERT(!Instr::isOperand2OnlyAlu(phvWord));
    bool matchDep = mauobj_->is_match_dependent((gress==0));
    // If we have a MochaPHV word insist this stage is match dependent
    // otherwise the word is NOT available to MatchInputXbar
    RMT_ASSERT(matchDep || !Instr::isOperand1OnlyAlu(phvWord));
  }
  // Return whether the phvWord is Din enabled
  return mauobj_->matchxbar_en_selector()->bit_set(phvWord);
}

void MauInputXbarWithReg::CalculateOutputByte(
    const Phv& phv,
    const int which_output_byte,
    uint8_t *output_byte,
    bool    *output_valid ) {
  RMT_ASSERT(which_output_byte < MauInputXbar::kTotalPreSwizzleMatchBytes);

  //  assert detects an old overwrite bug, code could be cleaned up to remove byte_enables_ array
  RMT_ASSERT (byte_enables_[which_output_byte] == (datapath_enable_check_[which_output_byte] != 0));

  if ( byte_enables_[which_output_byte] ) {
    int which_phv_word = byte_selectors_[which_output_byte];
    // For JBay need to map phvWord here as will be in [0-223]
    // (on other chips this will be an identity func)
    int mapped_phv_word = RmtDefs::map_maumixbar_phv_index(which_phv_word);
    bool phv_word_din_en = PhvWordAvailable(mapped_phv_word);
    input_xbar_.CalculateOutputByte(phv,mapped_phv_word,which_output_byte,phv_word_din_en,
                                    output_byte,output_valid);
    // XXX: Use verbose log if PHV word invalid or NOT Din enabled
    uint64_t log_flags;
    if (*output_valid && phv_word_din_en)
      log_flags = RmtDebug::trace(RmtDebug::kRmtDebugMauInput);
    else
      log_flags = RmtDebug::verbose(RmtDebug::kRmtDebugMauInput);
    RMT_LOG_OBJ(mauobj_, log_flags,
                "MauInputXbarWithReg::CalculateOutputByte byte_number=%d "
                "phv_word=%d mapped_phv_word=%d value=0x%02x %s %s\n",
                which_output_byte, which_phv_word, mapped_phv_word,
                *output_byte, *output_valid ?"valid" :"**INVALID**",
                phv_word_din_en ?"" :"**NO gress or NOT Din en**");

  } else {
    *output_valid = false;
    *output_byte  = 0;
    RMT_LOG_OBJ(mauobj_, RmtDebug::trace(RmtDebug::kRmtDebugMauInput),
                "MauInputXbarWithReg::CalculateOutputByte byte_number=%d value=0x%02x %s\n",
                which_output_byte,*output_byte,*output_valid ?"valid" :"invalid, byte NOT enabled");
  }
}

}
