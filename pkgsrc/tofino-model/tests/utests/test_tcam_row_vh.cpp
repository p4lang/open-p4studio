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
#include <iostream>
#include <string>
#include <array>
#include <cassert>

#include "gtest.h"

#include <utests/test_namespace.h>
#include <model_core/model.h>
#include <rmt-object-manager.h>
#include <mau-input-xbar-with-reg.h>
#include <tcam-row-vh-xbar-with-reg.h>
#include <register_utils.h> // for with registers test
#include "tcam_row_vh_util.h"


extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;

  static constexpr int kTotalMatchBytes  = MauDefs::kExactMatchBytes + MauDefs::kTernaryMatchBytes;
  static constexpr int kTotalMatchBits   = kTotalMatchBytes*8;
  static constexpr int kOutputBits       = MauDefs::kTernaryRowMatchBits;
  static constexpr int kVersionDataWidth = MauDefs::kVersionBits;


  bool trv_print=false;
  TEST(BFN_TEST_NAME(TestTcamRowVh),Basic) {
    if (trv_print) printf("test_tcam_row_vh_basic()\n");

    int chip=0;
    int pipe=1;
    int mau=2;
    int half_row=3;

    TestUtil tu(GLOBAL_MODEL.get(),chip,pipe,mau);
    RmtObjectManager *om = tu.get_objmgr();    
    ASSERT_TRUE(om != NULL);

    TcamRowVhWithReg trv_bus0( chip, pipe, mau, half_row , 0);
    TcamRowVhWithReg trv_bus1( chip, pipe, mau, half_row , 1);


    BitVector<kTotalMatchBits>  input;
    BitVector<kTotalMatchBytes> input_valid;
    BitVector<kVersionDataWidth> ingress_version_bits;
    BitVector<kVersionDataWidth> egress_version_bits;
    // search data is [bus][ even row, odd row ]
    std::array<std::array<BitVector<kOutputBits>,2>,2> search_data;

    // set the ternary bytes to the byte number
    input.fill_all_zeros();
    input_valid.fill_all_zeros();
    for (int i=128;i<kTotalMatchBytes;++i) {
      input.set_byte( i /*value*/, i /*which byte*/ );
      input_valid.set_bit( true, i );  // all valid
      //input_valid.set_bit( (i%2)==0, i );  // even valid
    }
    ingress_version_bits.set_byte(0xC,0);
    egress_version_bits.set_byte(0x3,0);

    for(int main_src=0;main_src<12;++main_src) {
      for(int extra_src=0;extra_src<6;++extra_src) {
        for (int extra_nibble=0;extra_nibble<2;++extra_nibble) {
          for (int row=half_row*2;row<(half_row+1)*2;++row) {
            for (int bus=0;bus<2; bus++) {
              tcam_row_vh_util::set_input_src_simple( chip, pipe, mau, row, bus ,
                                                      true  /* enable */,
                                                      main_src,
                                                      extra_src,
                                                      extra_nibble);
              
        
              trv_bus0.CalculateSearchData(
                  input,
                  input_valid,
                  ingress_version_bits,
                  egress_version_bits,
                  &search_data[0][0],
                  &search_data[0][1] );
              trv_bus1.CalculateSearchData(
                  input,
                  input_valid,
                  ingress_version_bits,
                  egress_version_bits,
                  &search_data[1][0],
                  &search_data[1][1] );
              
              auto data_out = &search_data[bus][row%2];
        
              int expected_first_byte = 128 + (main_src * 5) + ((main_src+1)/2);
              for (int i=0;i<5;++i) {
                if (trv_print) printf(" row=%d bus=%d main_src=%d i=%d\n",row,bus,main_src,i);
                EXPECT_EQ( (expected_first_byte+i), data_out->get_byte(i) );
              }
              
              int expected_extra_byte = 128 + (extra_src * 11) + 5;
              assert(extra_nibble<2); // doesn't handle cases 2 and 3
              uint8_t last_nibble = data_out->get_byte(5);
              EXPECT_EQ( 0xf & (extra_nibble ? (expected_extra_byte>>4) : expected_extra_byte),
                         last_nibble );
            }
          }
        }
      }
    }
          
  }


}


