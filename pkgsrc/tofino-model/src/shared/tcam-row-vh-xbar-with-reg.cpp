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

#include <rmt-log.h>
#include <tcam-row-vh-xbar-with-reg.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

TcamRowVhWithReg::TcamRowVhWithReg( int chip, int pipe, int mau, int half_row, int bus) :
  half_row_(half_row),
  bus_(bus),
  row_output_ctl_{{
    {default_adapter(row_output_ctl_[0],chip, pipe, mau, bus,  half_row*2,    [this](){this->CalculateSources();})},
    {default_adapter(row_output_ctl_[1],chip, pipe, mau, bus, (half_row*2)+1, [this](){this->CalculateSources();})} }},
  half_byte_mux_ctl_{{
      {default_adapter(half_byte_mux_ctl_[0],chip, pipe, mau, bus,  half_row*2,    [this](){this->CalculateSources();})},
      {default_adapter(half_byte_mux_ctl_[1],chip, pipe, mau, bus, (half_row*2)+1, [this](){this->CalculateSources();})} }},
  extra_byte_ctl_(default_adapter(extra_byte_ctl_,chip, pipe, mau, bus, half_row,  [this](){this->CalculateSources();})),
  valid_bit_xbar_ctl_array_(default_adapter(valid_bit_xbar_ctl_array_,chip, pipe, mau,
                            [this](uint32_t a2,uint32_t a1,uint32_t a0){
                                              this->ValidBitXbarArrayCallback(a2,a1,a0);} )),
  extra_bit_mux_ctl_{{
      {default_adapter(extra_bit_mux_ctl_[0],chip, pipe, mau, bus,  half_row*2,    [this](){this->CalculateSources();})},
      {default_adapter(extra_bit_mux_ctl_[1],chip, pipe, mau, bus, (half_row*2)+1, [this](){this->CalculateSources(); })} }}
  {
    RMT_ASSERT(bus_ < MauDefs::kTcamSearchBusesPerRow );
    RMT_ASSERT(half_row_ < (MauDefs::kTcamRowsPerMau / 2));
    row_output_ctl_[0].reset();
    row_output_ctl_[1].reset();
    half_byte_mux_ctl_[0].reset();
    half_byte_mux_ctl_[1].reset();
    extra_byte_ctl_.reset();
    valid_bit_xbar_ctl_array_.reset();
    extra_bit_mux_ctl_[0].reset();
    extra_bit_mux_ctl_[1].reset();
    // prevent warnings from valgrind about unitialized data
    //  by waiting until everything is reset before allowing
    //  CalculateSources() to run.
    reset_complete_=true;
    CalculateSources();
}


void TcamRowVhWithReg::CalculateSearchData(
      const BitVector<kTotalMatchBits>&  input,
      const BitVector<kTotalMatchBytes>& input_valid,
      const BitVector<kVersionDataWidth>& ingress_version_bits,
      const BitVector<kVersionDataWidth>& egress_version_bits,
      BitVector<kOutputBits> *search_data_even,
      BitVector<kOutputBits> *search_data_odd ) {

  // search data even first 5 bytes
  for (int i=0;i<5;++i) {
    if ( output_byte_enable_[i] ) {
      uint8_t byte = input.get_byte( output_byte_source_[i] );
      search_data_even->set_byte( byte, i );
    }
    else {
      search_data_even->set_byte(0,i);
    }
  }
  
  // search data odd first 5 bytes
  for (int i=0;i<5;++i) {
    if ( output_byte_enable_[i+6] ) {
      uint8_t byte = input.get_byte( output_byte_source_[i+6] );
      search_data_odd->set_byte( byte, i );
    }
    else {
      search_data_odd->set_byte(0,i);
    }
  }

  // the top nibble of each search word
  for (int bus=0;bus<2;++bus) {

    uint8_t byte = CalculateTopNibble(bus, input, input_valid,
                                      ingress_version_bits, egress_version_bits);


    if (bus==0) {
      search_data_even->set_byte(byte,5);
    }
    else {
      search_data_odd->set_byte(byte,5);
    }
  }
          
}

void TcamRowVhWithReg::ValidBitXbarArrayCallback(uint32_t a2, uint32_t a1, uint32_t a0) {
  if ( (a2 != bus_) || (a1 != half_row_) )
    return;
  RMT_ASSERT( a0 < kSecondRankValids );
  CalculateSources();
}

void TcamRowVhWithReg::CalculateSources()
{
  if (!reset_complete_) return;
  // Work out where the outputs of the first stage come from
  for (int first_rank_byte=0;first_rank_byte<kFirstRankBytes;++first_rank_byte) {
    output_byte_enable_[first_rank_byte] = false;

    if (first_rank_byte == 5) {  // the extra byte in the middle
      bool first_rank_extra_byte_en = extra_byte_ctl_.enabled_3bit_muxctl_enable();
      if (first_rank_extra_byte_en) {
        output_byte_enable_[first_rank_byte] = true;
        auto sel = extra_byte_ctl_.enabled_3bit_muxctl_select();
        output_byte_source_[first_rank_byte] = kTernaryFirstByte + 5 + (sel * kFirstRankBytes);
      }
    }
    else { // 0-4 or 6-10
      int index,pos;
      if (first_rank_byte < 5) {
        index=0;
        pos = first_rank_byte; // position within the 5 byte chunk
      }
      else {
        index=1;
        pos = first_rank_byte-6; // position within the 5 byte chunk
      }
      bool first_rank_en  = row_output_ctl_[index].enabled_4bit_muxctl_enable();
      if (first_rank_en) {
        int first_rank_sel  = row_output_ctl_[index].enabled_4bit_muxctl_select();
        output_byte_enable_[first_rank_byte] = true;
        output_byte_source_[first_rank_byte] = kTernaryFirstByte + pos +
            // pairs of 5 byte chunks are 11 apart
            ((first_rank_sel/2) * kFirstRankBytes) +
            // odd chunks come after the extra byte
            ((first_rank_sel&1) ? 6 : 0); 
      }
    }
  }

  // the valid bits
  for (int i=0; i<kSecondRankValids; ++i) {
    valid_bit_enable_[i] = false;

    int v = valid_bit_xbar_ctl_array_.tcam_validbit_xbar_ctl(bus_,half_row_,i);
    if (v < kFirstRankBytes && output_byte_enable_[v] ) {
      valid_bit_enable_[i] = true;
      valid_bit_sel_[i]    = output_byte_source_[v];
    }
  }
  
}


}

