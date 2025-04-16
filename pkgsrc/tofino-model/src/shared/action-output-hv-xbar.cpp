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
#include <action-output-hv-xbar.h>
#include <register_adapters.h>

namespace MODEL_CHIP_NAMESPACE {

ActionOutputHvXbar::ActionOutputHvXbar( int chip, int pipe, int mau, int row ) :
    byte_control_regs_(default_adapter(byte_control_regs_,chip, pipe, mau, row)),
    halfword_control_regs_(default_adapter(halfword_control_regs_,chip, pipe, mau, row)),
    word_control_regs_(default_adapter(word_control_regs_,chip, pipe, mau, row)),
    bytemask_regs_(default_adapter(bytemask_regs_,chip, pipe, mau, row))
{
  byte_control_regs_.reset();
  halfword_control_regs_.reset();
  word_control_regs_.reset();
  bytemask_regs_.reset();
}

ActionOutputHvXbar::~ActionOutputHvXbar()
{
}


// Private function called by CalculateOutput or CalculateOutputDupMask
void ActionOutputHvXbar::CalculateOutputNormal(
    const BitVector< kBytesIn * 8 >& input,
    int input_bus, int input_bitwidth, uint32_t input_bytemask,
    BitVector< kBytesOut *8 >* output)
{
  // byte region (first 16 byte output slice)
  if ( byte_control_regs_.action_hv_ixbar_ctl_byte_1to0_enable(input_bus) ) {
    MapBytes( 2 /*no.bytes*/, 0 /*from*/, 0 /*slice*/,
              byte_control_regs_.action_hv_ixbar_ctl_byte_1to0_ctl(input_bus),
              input_bytemask,input,output);
  }
  if ( byte_control_regs_.action_hv_ixbar_ctl_byte_3to2_enable(input_bus) ) {
    MapBytes( 2 /*no.bytes*/, 2 /*from*/, 0 /*slice*/,
              byte_control_regs_.action_hv_ixbar_ctl_byte_3to2_ctl(input_bus),
              input_bytemask,input,output);
  }
  if ( byte_control_regs_.action_hv_ixbar_ctl_byte_7to4_enable(input_bus) ) {
    MapBytes( 4 /*no.bytes*/, 4 /*from*/, 0 /*slice*/,
              byte_control_regs_.action_hv_ixbar_ctl_byte_7to4_ctl(input_bus),
              input_bytemask,input,output);
  }
  if ( byte_control_regs_.action_hv_ixbar_ctl_byte_15to8_enable(input_bus) ) {
    MapBytes( 8 /*no.bytes*/, 8 /*from*/, 0 /*slice*/,
              byte_control_regs_.action_hv_ixbar_ctl_byte_15to8_ctl(input_bus),
              input_bytemask,input,output);
  }

  // halfword slices (3 of)
  const int first_halfword_slice = 1;
  for (int slice=0;slice<3;++slice) {
    if ( halfword_control_regs_.action_hv_ixbar_ctl_halfword_3to0_enable(slice,input_bus) ) {
      int ctl = halfword_control_regs_.action_hv_ixbar_ctl_halfword_3to0_ctl(slice,input_bus);
      MapBytes( 4 /*no.bytes*/, 0 /*from*/, first_halfword_slice+slice,
                ctl,
                input_bytemask,input,output);
    }
    if ( halfword_control_regs_.action_hv_ixbar_ctl_halfword_7to4_enable(slice,input_bus) ) {
      int ctl = halfword_control_regs_.action_hv_ixbar_ctl_halfword_7to4_ctl(slice,input_bus);
      MapBytes( 4 /*no.bytes*/, 4 /*from*/, first_halfword_slice+slice,
                ctl,
                input_bytemask,input,output);
    }
    if ( halfword_control_regs_.action_hv_ixbar_ctl_halfword_15to8_enable(slice,input_bus) ) {
      int ctl = halfword_control_regs_.action_hv_ixbar_ctl_halfword_15to8_ctl(slice,input_bus);
      MapBytes( 8 /*no.bytes*/, 8 /*from*/, first_halfword_slice+slice,
                ctl,
                input_bytemask,input,output);
    }
  }

  // word slices (4 of)
  const int first_word_slice = first_halfword_slice + 3;
  for (int slice=0;slice<4;++slice) {
    if ( word_control_regs_.action_hv_ixbar_ctl_word_7to0_enable(slice,input_bus) ) {
      MapBytes( 8 /*no.bytes*/, 0 /*from*/, first_word_slice+slice,
                word_control_regs_.action_hv_ixbar_ctl_word_7to0_ctl(slice,input_bus),
                input_bytemask,input,output);
    }
    if ( word_control_regs_.action_hv_ixbar_ctl_word_15to8_enable(slice,input_bus) ) {
      MapBytes( 8 /*no.bytes*/, 8 /*from*/, first_word_slice+slice,
                word_control_regs_.action_hv_ixbar_ctl_word_15to8_ctl(slice,input_bus),
                input_bytemask,input,output);
    }
  }
}
// Private function called by CalculateOutput
void ActionOutputHvXbar::CalculateOutputDupMask(
    const BitVector< kBytesIn * 8 >& input,
    int input_bus, int input_bitwidth, uint32_t input_bytemask,
    BitVector< kBytesOut *8 >* output) {

  BitVector< kBytesIn * 8 > new_input;
  new_input.copy_from(input);
  
  // Do dup byte/half-word/word of new input FIRST if !BUG
  if (!kBugActionDataBusMaskBeforeDup) {
    switch (input_bitwidth) {
      case 8: case 16: case 32:
        new_input.set_word(input.get_word(0,input_bitwidth), input_bitwidth, input_bitwidth);
        break;
    }
  }
  // Do mask new input - clear any byte with bit not set in input_bytemask
  for (int i = 0; i < kBytesIn; i++) {
    if (((input_bytemask >> i) & 1u) == 0u) new_input.set_byte(0, i);
  }  
  // Do dup byte/half-word/word of new input LAST if BUG
  if (kBugActionDataBusMaskBeforeDup) {
    switch (input_bitwidth) {
      case 8: case 16: case 32:
        new_input.set_word(input.get_word(0,input_bitwidth), input_bitwidth, input_bitwidth);
        break;
    }
  }
  // Call CalculateOuputNormal with new input (mask already done so pass ~0u)
  CalculateOutputNormal(new_input, input_bus, input_bitwidth, ~0u, output);
}


// Public function
void ActionOutputHvXbar::CalculateOutput(
    const BitVector< kBytesIn * 8 >& input,
    int input_bus, int input_bitwidth,
    BitVector< kBytesOut *8 >* output) {

  uint32_t input_bytemask = bytemask_regs_.action_hv_ixbar_input_bytemask(input_bus);
  switch (input_bitwidth) {
    case 8: case 16: case 32:
      CalculateOutputDupMask(input, input_bus, input_bitwidth, input_bytemask, output);
      return;
    default:
      CalculateOutputNormal(input, input_bus, input_bitwidth, input_bytemask, output);
      return;
  }
}
  


/*********************************************************************************
  For testing, should make the picture Pat had in unit_ram_array.txt:
15               .               .               .               .               . 
14             .               .               .               .               .   
13           .               .               .               .               .     
12         .               .               .               .               .       
11       .               .               .               .               .         
10     .               .               .               .               .           
 9   .               .               .               .               .             
 8 .               .               .               .               .               
 7       .       .       .       .       .       .       .       .       .       . 
 6     .       .       .       .       .       .       .       .       .       .   
 5   .       .       .       .       .       .       .       .       .       .     
 4 .       .       .       .       .       .       .       .       .       .       
 3   .   .   .   .   .   .   .   .       .       .       .       .       .       . 
 2 .   .   .   .   .   .   .   .       .       .       .       .       .       .   
 1   .   .   .   .   .   .   .   .   .       .       .       .       .       .     
 0 .   .   .   .   .   .   .   .   .       .       .       .       .       .       
*/
// No longer works as InputByte() function is now obsolete with new registers.
//   Maybe could set the registers and call CalculateOutput to work it out??
#if 0
std::string ActionOutputHvXbar::MakePatsPicture() {
  std::string s("");
  for (int i=15;i>=0;--i) {
    s += (i<10 ? " ":"") +  std::to_string(i) + " ";
    // bytes
    for (int output_byte=0;output_byte<16;++output_byte) {
      std::string c(" ");
      for (int mux_in=0;mux_in<4;++mux_in ) {
        if ( i == InputByte(output_byte,mux_in) ) {
          RMT_ASSERT( c == " " ); // should be only one mux input connecting these two
          c = ".";
        }
      }
      s += c + " ";
    }

    // half words
    for (int output_hw=0;output_hw<24;++output_hw) {
      std::string c(" ");
      for (int mux_in=0;mux_in<3;++mux_in ) {
        if ( i == InputByte(16+output_hw,mux_in) ) {
          RMT_ASSERT( c == " " ); // should be only one mux input connecting these two
          c = ".";
        }
      }
      s += c + " ";
    }
    s += '\n';
  }
  return s;
}
#endif
}
