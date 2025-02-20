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

#include <rmt-object-manager.h>
#include <action-output-hv-xbar.h>
#include <model_core/model.h>
#include <register_includes/reg.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

  bool aohx_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;

  TEST(BFN_TEST_NAME(ActionOutputHvXbar),Basic) {
    // Reset Model in case we have register subscribers from some previous tests
    GLOBAL_MODEL->Reset();
    TestUtil tu(GLOBAL_MODEL.get());
    (void)tu.get_objmgr();

    if (aohx_print) RMT_UT_LOG_INFO("test_action_output_hv_xbar_basic()\n");
    ActionOutputHvXbar a(0,0,0,0);
    BitVector<128> input{ { { UINT64_C(0xA7A6A5A4A3A2A1A0), UINT64_C(0xAfAeAdAcAbAaA9A8) } } };
    BitVector<128*8> output{ UINT64_C(0x0) };

    // this is the register for row 0
    auto& ahv_xbar = RegisterUtils::ref_mau(0,0).rams.array.row[0].action_hv_xbar;
    int chip=0;

    // set the input bytemask to all ones
    GLOBAL_MODEL->OutWord( chip,
                           &(ahv_xbar.action_hv_ixbar_input_bytemask[0]),
                           0xFFFF );

    // nothing is enabled so should produce all zeros.
    a.CalculateOutput( input, 0, 128, &output);
    for (int i=0;i<128;++i) {
      EXPECT_EQ( 0, output.get_byte(i) );
    }

    uint32_t word;
    word=0;

    // route first 16 input bytes straight though to the output
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_15to8_enable(&word,1);
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_15to8_ctl(&word,1);
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_7to4_enable(&word,1);
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_7to4_ctl(&word,1);
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_3to2_enable(&word,1);
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_3to2_ctl(&word,1);
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_1to0_enable(&word,1);
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_1to0_ctl(&word,0);


    GLOBAL_MODEL->OutWord( chip,
                           &(ahv_xbar.action_hv_ixbar_ctl_byte[0]),
                           word );

    a.CalculateOutput( input, 0, 128, &output);
    for (int i=0;i<128;++i) {
      if ( i<16 ) {
        EXPECT_EQ( 0xA0 | (i&0xf) , output.get_byte(i) );
      }
      else {
        EXPECT_EQ( 0, output.get_byte(i) );
      }
    }
    // TODO: test bytemask. And half words... and words...
  }


  TEST(BFN_TEST_NAME(ActionOutputHvXbar),DupMask) {
    // Reset Model in case we have register subscribers from some previous tests
    GLOBAL_MODEL->Reset();
    TestUtil tu(GLOBAL_MODEL.get());
    (void)tu.get_objmgr();
    if (aohx_print) RMT_UT_LOG_INFO("test_action_output_hv_xbar_dup_mask()\n");

    bool prev_mask_before_dup = ActionOutputHvXbar::kBugActionDataBusMaskBeforeDup;
    ActionOutputHvXbar a(0,0,0,0);
    BitVector<128> input{ { { UINT64_C(0xA7A6A5A4A3A2A1A0), UINT64_C(0xAfAeAdAcAbAaA9A8) } } };
    BitVector<128*8> output{ UINT64_C(0x0) };

    // this is the register for row 0
    auto& ahv_xbar = RegisterUtils::ref_mau(0,0).rams.array.row[0].action_hv_xbar;
    int      chip = 0;
    uint32_t word = 0u;

    // Route first 16 input bytes straight though to the output as before
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_15to8_enable(&word,1);
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_15to8_ctl(&word,1);
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_7to4_enable(&word,1);
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_7to4_ctl(&word,1);
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_3to2_enable(&word,1);
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_3to2_ctl(&word,1);
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_1to0_enable(&word,1);
    setp_action_hv_ixbar_ctl_byte_action_hv_ixbar_ctl_byte_1to0_ctl(&word,0);
    GLOBAL_MODEL->OutWord( chip,
                           &(ahv_xbar.action_hv_ixbar_ctl_byte[0]),
                           word );
    // But now set the input bytemask to just take the bottom byte
    GLOBAL_MODEL->OutWord( chip,
                           &(ahv_xbar.action_hv_ixbar_input_bytemask[0]),
                           0x0001 );

    // First test bug behaviour where mask occurs before dup
    ActionOutputHvXbar::kBugActionDataBusMaskBeforeDup = true;
    output.fill_all_zeros();
    a.CalculateOutput( input, 0, 8, &output);
    EXPECT_EQ( 0xA0, output.get_byte(0) ); // Got byte0
    EXPECT_EQ( 0xA0, output.get_byte(1) ); // And byte0 dupped to byte1
    for (int i = 2; i < 128; i++) {
      EXPECT_EQ( 0, output.get_byte(i) );
    }
    // Now test non-bug behaviour where dup occurs before mask
    ActionOutputHvXbar::kBugActionDataBusMaskBeforeDup = false;
    output.fill_all_zeros();
    a.CalculateOutput( input, 0, 8, &output);
    EXPECT_EQ( 0xA0, output.get_byte(0) ); // Only original byte0 left
    for (int i = 1; i < 128; i++) {
      EXPECT_EQ( 0, output.get_byte(i) );
    }

    // Restore original behaviour
    ActionOutputHvXbar::kBugActionDataBusMaskBeforeDup = prev_mask_before_dup;
  }


  TEST(BFN_TEST_NAME(ActionOutputHvXbar),DISABLED_PatsPicture) {
    // Reset Model in case we have register subscribers from some previous tests
    GLOBAL_MODEL->Reset();
    TestUtil tu(GLOBAL_MODEL.get());
    (void)tu.get_objmgr();
    if (aohx_print) RMT_UT_LOG_INFO("test_action_output_hv_xbar_pats_picture()\n");

    std::string pats_picture("15               .               .               .               .               . \n"
                             "14             .               .               .               .               .   \n"
                             "13           .               .               .               .               .     \n"
                             "12         .               .               .               .               .       \n"
                             "11       .               .               .               .               .         \n"
                             "10     .               .               .               .               .           \n"
                             " 9   .               .               .               .               .             \n"
                             " 8 .               .               .               .               .               \n"
                             " 7       .       .       .       .       .       .       .       .       .       . \n"
                             " 6     .       .       .       .       .       .       .       .       .       .   \n"
                             " 5   .       .       .       .       .       .       .       .       .       .     \n"
                             " 4 .       .       .       .       .       .       .       .       .       .       \n"
                             " 3   .   .   .   .   .   .   .   .       .       .       .       .       .       . \n"
                             " 2 .   .   .   .   .   .   .   .       .       .       .       .       .       .   \n"
                             " 1   .   .   .   .   .   .   .   .   .       .       .       .       .       .     \n"
                             " 0 .   .   .   .   .   .   .   .   .       .       .       .       .       .       \n");
    ActionOutputHvXbar a(0,0,0,0);
    // MakePatsPicture() doesn't work at the moment
    //EXPECT_EQ( pats_picture, a.MakePatsPicture());

  }
}
