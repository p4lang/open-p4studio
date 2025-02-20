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

#include <utests/test_util.h>
#include <utests/action_hv_translator.h>


namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

// TODO_0709 : implement it!
void ActionHvTranslator::ctl_word(int row,int side,int index,uint32_t value) {
  assert(row<8);
  assert(side<2);
  assert(index<11);

  for (int i=0;i<2;++i) {
    int v = (value>>(i*2))&3;
    int ind = (index*2) + i;
    int output_byte = (ind*4) + kXbarBytes + (2*kXbarHalfWords);

    if (v==0) {
      word_outputs[row][side][ind].enable = false;
      word_outputs[row][side][ind].input = 0;
    }
    else {
      int input_byte = InputByte( output_byte, v-1 );
      word_outputs[row][side][ind].enable = true;
      word_outputs[row][side][ind].input  = input_byte;
    }
  }
    
}
void ActionHvTranslator::ctl_half(int row,int side,int index,uint32_t value) {
  assert(row<8);
  assert(side<2);
  assert(index<7);

  for (int i=0;i<4;++i) {
    int v = (value >> ((i)*2)) & 0x3;;
    int ind = (index*4) + i;
    int output_byte = (ind*2) + kXbarBytes;

    if (v==0) {
      half_outputs[row][side][ind].enable = false;
      half_outputs[row][side][ind].input = 0;
    }
    else {
      int input_byte = InputByte( output_byte, v-1 );
      half_outputs[row][side][ind].enable = true;
      half_outputs[row][side][ind].input  = input_byte;
    }
  }

}
void ActionHvTranslator::ctl_byte(int row,int side,uint32_t value) {
  assert(row<8);
  assert(side<2);

  for (int i=0;i<16;++i) {
    int v = (value >> (i*2)) & 0x3;;
    int output_byte = i;

    int input_byte = InputByte( output_byte, v );
    byte_outputs[row][side][i].input  = input_byte;
  }
}
void ActionHvTranslator::ctl_byte_enable(int row,int side,uint32_t value) {
  assert(row<8);
  assert(side<2);

  for (int i=0;i<16;++i) {
    int v = (value >> i) & 1;;
    byte_outputs[row][side][i].enable = v;
  }
}
void ActionHvTranslator::do_writes(TestUtil* tu) {

  auto& mau_reg_map = RegisterUtils::ref_mau(0,0);

  // output bytes
  for (int row=0;row<8;++row) {
    for (int side=0;side<2;++side) {
      uint32_t word=0;
      for (int i=0;i<kXbarBytes; ++i) {
        int output_byte = i;
        if ( byte_outputs[row][side][i].enable ) {
          int input_byte = byte_outputs[row][side][i].input;
          printf(" input byte[%d][%d] %d  attached to output byte %d\n",
                 row,side,
                 input_byte,output_byte);

          switch (input_byte) {
            case 0: case 1:
              setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_1to0_enable(
                  &word, 1 );
              setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_1to0_ctl(
                  &word, output_byte/2 );
              break;
            case 2: case 3:
              setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_3to2_enable(
                  &word, 1 );
              setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_3to2_ctl(
                  &word, output_byte/2 );
              break;
            case 4: case 5: case 6: case 7:
              setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_7to4_enable(
                  &word, 1 );
              setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_7to4_ctl(
                  &word, output_byte/4 );
              break;
            case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15:
              setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_15to8_enable(
                  &word, 1 );
              setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_15to8_ctl(
                  &word, output_byte/8 );
              break;
          }
        }
      }
      tu->OutWord(&mau_reg_map.rams.array.row[row].action_hv_xbar.action_hv_ixbar_ctl_byte[side], word);
    }
  }

  // half word outputs
  uint32_t half_slice_cfg[8][2][4]{};
  for (int row=0;row<8;++row) {
    for (int side=0;side<2;++side) {
      for (int i=0;i<kXbarHalfWords; ++i) {
        int output_byte = (i*2) + kXbarBytes;
        if ( half_outputs[row][side][i].enable ) {
          int input_byte = half_outputs[row][side][i].input;
          printf(" input byte[%d][%d] %d  attached to output byte %d\n",
                 row,side,
                 input_byte,output_byte);
          int slice = i/8;
          if ( input_byte >= 0 && input_byte <= 3 ) {
            setp_action_hv_ixbar_ctl_halfword_action_hv_ixbar_ctl_halfword_3to0_enable(
                &(half_slice_cfg[row][side][slice]), 1 );
            setp_action_hv_ixbar_ctl_halfword_action_hv_ixbar_ctl_halfword_3to0_ctl(
                &(half_slice_cfg[row][side][slice]), i/2 );
          }
          else if ( input_byte >= 4 && input_byte <= 7 ) {
            setp_action_hv_ixbar_ctl_halfword_action_hv_ixbar_ctl_halfword_7to4_enable(
                &(half_slice_cfg[row][side][slice]), 1 );
            setp_action_hv_ixbar_ctl_halfword_action_hv_ixbar_ctl_halfword_7to4_ctl(
                &(half_slice_cfg[row][side][slice]), i/2 );
          }
          else if ( input_byte >= 8 && input_byte <= 15 ) {
            setp_action_hv_ixbar_ctl_halfword_action_hv_ixbar_ctl_halfword_15to8_enable(
                &(half_slice_cfg[row][side][slice]), 1 );
            setp_action_hv_ixbar_ctl_halfword_action_hv_ixbar_ctl_halfword_15to8_ctl(
                &(half_slice_cfg[row][side][slice]), i/4 );
            //&(half_slice_cfg[row][side][slice]), 1 ); // hack!
          }
        }
      }
    }
  }

  // full word outputs
  for (int row=0;row<8;++row) {
    for (int side=0;side<2;++side) {
      uint32_t word_slice_cfg[5]{};
      for (int i=0;i<kXbarWords; ++i) {
        int output_byte = (i*4) + kXbarBytes + (2*kXbarHalfWords);
        if ( word_outputs[row][side][i].enable ) {
          int input_byte = word_outputs[row][side][i].input;
          printf(" input byte[%d][%d] %d  attached to output byte %d\n",
                 row,side,
                 input_byte,output_byte);
          if (output_byte>= 72 && output_byte <=79 ) {
            printf("Special case %d,%d program half words!\n",row,side);

            // have to set up the halfwords 3to0 and 7to4 in slice 3 to: 2 and 3
            setp_action_hv_ixbar_ctl_halfword_action_hv_ixbar_ctl_halfword_3to0_enable(
                &(half_slice_cfg[row][side][3]), 1 );
            setp_action_hv_ixbar_ctl_halfword_action_hv_ixbar_ctl_halfword_3to0_ctl(
                &(half_slice_cfg[row][side][3]), 2 );
            setp_action_hv_ixbar_ctl_halfword_action_hv_ixbar_ctl_halfword_7to4_enable(
                &(half_slice_cfg[row][side][3]), 1 );
            setp_action_hv_ixbar_ctl_halfword_action_hv_ixbar_ctl_halfword_7to4_ctl(
                &(half_slice_cfg[row][side][3]), 3 );
          }
          else {
            int slice = (output_byte - 80)/16;
            int ind = output_byte - 80;
            if ( input_byte >= 0 && input_byte <= 7 ) {
              setp_action_hv_ixbar_ctl_word_action_hv_ixbar_ctl_word_7to0_enable(
                  &(word_slice_cfg[slice]), 1 );
              setp_action_hv_ixbar_ctl_word_action_hv_ixbar_ctl_word_7to0_ctl(
                  &(word_slice_cfg[slice]), ind/8 );
            }
            else if ( input_byte >= 8 && input_byte <= 15 ) {
              setp_action_hv_ixbar_ctl_word_action_hv_ixbar_ctl_word_15to8_enable(
                  &(word_slice_cfg[slice]), 1 );
              setp_action_hv_ixbar_ctl_word_action_hv_ixbar_ctl_word_15to8_ctl(
                  &(word_slice_cfg[slice]), ind/8 );
            }
            
          }
        }
      }
      for (int slice=0;slice<5;++slice) {
        tu->OutWord(&mau_reg_map.rams.array.row[row].action_hv_xbar.action_hv_ixbar_ctl_word[slice][side], word_slice_cfg[slice]);
      }

      
    }
  }


  // Now we can actualy do the half slice writes ( as the last slice
  //  entry can beinfluenced by the full words )
  for (int row=0;row<8;++row) {
    for (int side=0;side<2;++side) {
      for (int slice=0;slice<4;++slice) {
        tu->OutWord(&mau_reg_map.rams.array.row[row].action_hv_xbar.action_hv_ixbar_ctl_halfword[slice][side], half_slice_cfg[row][side][slice]);
      }
    }
  }

  // turn all the input bytemasks on
  for (int row=0;row<8;++row) {
    for (int side=0;side<2;++side) {
        tu->OutWord(&mau_reg_map.rams.array.row[row].action_hv_xbar.action_hv_ixbar_input_bytemask[side], 0xFFFF);
    }
  }

}


// tells you which input byte is connected to a particular input of an output_byte's mux
// taken from old action-output-hv-xbar.cpp
int ActionHvTranslator::InputByte( int output_byte, int mux_input )
{
  if (output_byte < kXbarBytes) {  // bytes
    switch (mux_input) {
      case 0:
        return output_byte % 2;
      case 1:
        return 2 + (output_byte % 2);
      case 2:
        return 4 + (output_byte % 4);
      case 3:
        return 8 + (output_byte % 8);
      default:
        RMT_ASSERT(0);
    }
  }
  else if (output_byte < (kXbarBytes + (kXbarHalfWords*2))) { // half words
    switch (mux_input) {
      case 0:
        return output_byte % 4;
      case 1:
        return 4 + (output_byte % 4);
      case 2:
        return 8 + (output_byte % 8);
      default:
        RMT_ASSERT(0);
    }
  }
  else if (output_byte < kBytesOut ) { // words
    switch (mux_input) {
      case 0:
        return output_byte % 8;
      case 1:
        return 8 + (output_byte % 8);
      default:
        RMT_ASSERT(0);
    }
  }
  else {
    RMT_ASSERT(0);
  }
}

}

