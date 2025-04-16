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
#include <array>
#include <register_utils.h>
#include "tcam_row_vh_util.h"

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

namespace tcam_row_vh_util {

using namespace MODEL_CHIP_NAMESPACE;


// set the input source for a bus (0 or 1) on a tcam row (0->11)
//  main_src 0->11 gets 5 bytes from input xbar as follows:
//    0   ->    128 .. 132    (ie bytes 128,129,130,131,132  then 133 is extra byte)
//    1   ->    134 .. 138
//    2   ->    139 .. 143    (144 is extra byte)
//    3   ->    145 .. 149
//  etc
//  extra_byte_src 0->5 selects bytes:
//    0   ->  133
//    1   ->  144
//    2   ->  155
//   etc
//  extra_nibble selects where the extra nibble comes from:
//   0 = low nibble of extra byte
//   1 = high nibble of extra byte
//   2 = valid bit xbar output, low nibble for even rows, high nibble for odd rows
//   3 = valid on bits 1:0, version on bits 3:2
void set_input_src_simple(int chip, int pipe, int mau, int row, int bus ,
                          bool enable, int main_src, int extra_byte_src, int extra_nibble) {
  assert( main_src < 12 );
  assert( extra_byte_src < 6 );
  assert( pipe < 4 );
  assert( mau < 12 );
  assert( row < 16 );
  assert( bus < 2 );

  int half_row = row / 2;

  auto& vdx = RegisterUtils::ref_mau(pipe,mau).tcams.vh_data_xbar;

  // disable all valids
  for (int valid_bit=0;valid_bit<8;++valid_bit) {
    // Codes 0-10 select the valid bit from byte 0-10. Byte 5 is the extrabyte.
    // Codes 11-15 mean disable.
    GLOBAL_MODEL->OutWord(chip, &(vdx.tcam_validbit_xbar_ctl[bus][half_row][valid_bit]), 11);
  }

  uint32_t word;

  // set main source
  // There are 88 bytes output from the match input xbar to ternary matching.
  // These are organized into 16 groups of 5B separated with 1B between each group
  // (plus valid bits). The 16 groups of 5B are the inputs to this mux.
  // Codes 0-15 select from these 16 inputs with bits 3:0
  word=0;
  RegisterUtils::do_setp_tcam_row_output_ctl_enabled_4bit_muxctl_enable(&word, enable);
  RegisterUtils::do_setp_tcam_row_output_ctl_enabled_4bit_muxctl_select(&word, main_src);
  GLOBAL_MODEL->OutWord(chip, &(vdx.tcam_row_output_ctl[bus][row]), word);

  // Set the half byte mux
  // select:
  // Code 0 selects the low nibble from the byte swizzle mux.
  // Code 1 selects the hi  nibble from the byte swizzle mux.
  // Code 2 selects the valid bit xbar output. Lo nibble for even rows, hi nibble for odd rows.
  // Code 3 selects the valid bit xbar output on bits 1:0 and version bits on bits 3:2.
  word=0;
  RegisterUtils::do_setp_tcam_row_halfbyte_mux_ctl_tcam_row_search_thread(&word, 0);
  RegisterUtils::do_setp_tcam_row_halfbyte_mux_ctl_tcam_row_halfbyte_mux_ctl_enable(&word, enable);
  RegisterUtils::do_setp_tcam_row_halfbyte_mux_ctl_tcam_row_halfbyte_mux_ctl_select(&word, extra_nibble);
  GLOBAL_MODEL->OutWord(chip, &(vdx.tcam_row_halfbyte_mux_ctl[bus][row]) , word);

  // For every rowpair of tcams, there is a mux to select one byte of match data
  // to go to the 4 extra match bits on each row.
  // The remaining 8 single bytes that were sandwiched between the 5B groups
  // go to an 8 input mux, controlled by this register.
  word=0;
  setp_tcam_extra_byte_ctl_enabled_3bit_muxctl_enable(&word, enable);
  setp_tcam_extra_byte_ctl_enabled_3bit_muxctl_select(&word, extra_byte_src);
  GLOBAL_MODEL->OutWord(chip, &(vdx.tcam_extra_byte_ctl[bus][half_row]), word);
}


}

}
