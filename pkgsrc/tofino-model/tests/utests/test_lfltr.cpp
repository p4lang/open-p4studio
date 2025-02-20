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

#include <model_core/model.h>
#include <bitvector.h>
#include <rmt-object-manager.h>
#include <mau.h>
#include <packet.h>
#include <learning-filter.h>
#include "input_xbar_util.h"
#include "tcam_row_vh_util.h"

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool lfltr_print = true;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;

  // Test description
  // Disable learning filter
  // Send 20 LQs - no callback, no learning
  // Enable learning filter
  // Set push timer to 20
  // send 40 LQs, 30 unique, get the callback
  // Send another 30 LQs, 10 unique and 20 same as above, get callback (2nd buffer)
  // Send 20 new LQs - drop (no space) - no callback
  // clear (1st) buffer
  // send 10 LQs, 10 unique, => no callback (buffer not full)
  // clear 2nd buffer (1st buffer has 10 new LQs)
  // send 20 unique LQs - get 1 callback (10 in 2nd buffer)
  // clear buffer
  // send 10 LQs - get call back (for buffer 2)
  // clear buffer

  // Test: Max buffer limits
  //
  TEST(BFN_TEST_NAME(LearningFilterTest), LfltrTest_1) {
    if (lfltr_print) RMT_UT_LOG_INFO("Test Learning Filter");
    int chip = 202;
    int pipe = 0;
    int stage = 0;

    int n_entries = 2048;
    if (RmtObject::is_jbay_or_later()) {
      n_entries = 4096;
    }


    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);

    //uint64_t pipes, stages, types, rows_tabs, cols, flags;
    //pipes = ALL; stages = ALL; types = ALL; rows_tabs = TOP; cols = NON; flags = FEW;
    //pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
    //tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    LearningFilter *lf = om->learning_filter_lookup(pipe);
    assert(lf);

    tu.zero_dru_learn_callback_count();

    tu.lfltr_config(pipe, true/*disable*/, 20); // push threshold set to 20 packets

    LearnQuantumType    lq;
    lq.data.set_word(0x011100DEAD000000UL, 0);
    lq.valid = true;
    lq.length = (RmtDefs::kLearnQuantumWidth + 7) / 8;

    // Disable learning filter
    // Send 30 LQs - no callback, no learning
    if (lfltr_print) RMT_UT_LOG_INFO("Test1: Push LQs when Learning Filter is disabled");
    for (int loop = 0; loop < 30; loop++) {
      lq.data.set_byte((uint8_t)loop, 0);
      lf->learn(lq);
    }
    EXPECT_EQ(0, tu.dru_learn_callback_count() );

    // Enable Learning Filter
    // send 40 LQs, 30 unique, get one callback
    if (lfltr_print) RMT_UT_LOG_INFO("Learning Filter is enabled");
    tu.lfltr_config(pipe, false/*disable*/, 20);
    for (int loop = 0; loop < 30; loop++) {
      lq.data.set_byte((uint8_t)loop, 0);
      lf->learn(lq);
    }
    // send duplicate LQs
    for (int loop = 0; loop < 10; loop++) {
      lq.data.set_byte((uint8_t)loop, 0);
      lf->learn(lq);
    }
    if (lfltr_print) RMT_UT_LOG_INFO("=== Learning Filter callback 1 - send 30 (10/20) LQs");
    EXPECT_EQ(1, tu.dru_learn_callback_count() );
    // Send another 30 LQs, 10 unique and 20 same as above, get callback (2nd buffer)
    for (int loop = 10; loop < 40; loop++) {
      lq.data.set_byte((uint8_t)loop, 0);
      lf->learn(lq);
    }
    if (lfltr_print) RMT_UT_LOG_INFO("=== Learning Filter callback 2 - send 20 new - all to be Dropped");
    EXPECT_EQ(2, tu.dru_learn_callback_count() );
    tu.zero_dru_learn_callback_count();
    // Send 20 new LQs - drop (no space) - no callback
    for (int loop = 40; loop < 60; loop++) {
      lq.data.set_byte((uint8_t)loop, 0);
      lf->learn(lq);
    }
    if (lfltr_print) RMT_UT_LOG_INFO("=== Learning Filter no callback - all LQs Dropped");
    EXPECT_EQ(20u, lf->lq_count_dropped_buffer_full());
    EXPECT_EQ(0, tu.dru_learn_callback_count() );
    // clear (1st) buffer
    // send 10 LQs, 10 unique, => no callback (buffer not full)
    // clear 2nd buffer (1st buffer has 10 new LQs)
    // send 20 unique LQs - get 1 callback (10 in 2nd buffer)
    if (lfltr_print) RMT_UT_LOG_INFO("=== Clear Filter (1) - send 10 new LQs");
    tu.lfltr_clear(pipe);
    for (int loop = 40; loop < 50; loop++) {
      lq.data.set_byte((uint8_t)loop, 0);
      lf->learn(lq);
    }
    EXPECT_EQ(0, tu.dru_learn_callback_count() );
    if (lfltr_print) RMT_UT_LOG_INFO("=== Clear Filter (2) - send 20 new LQs");
    tu.lfltr_clear(pipe); // second buffer
    // Erroneous clearing.. should be fine
    tu.lfltr_clear(pipe);
    for (int loop = 10; loop < 30; loop++) {
      lq.data.set_byte((uint8_t)loop, 0);
      lf->learn(lq);
    }
    if (lfltr_print) RMT_UT_LOG_INFO("=== Learning Filter callback (1) - clear filter (1)");
    EXPECT_EQ(1, tu.dru_learn_callback_count() );
    // clear buffer (1)
    // send 10 LQs - get call back (for buffer 2)
    // clear buffer
    tu.lfltr_clear(pipe); // first buffer
    for (int loop = 0; loop < 10; loop++) {
      lq.data.set_byte((uint8_t)loop, 0);
      lf->learn(lq);
    }
    EXPECT_EQ(2, tu.dru_learn_callback_count() );
    tu.lfltr_clear(pipe); // second buffer
    tu.zero_dru_learn_callback_count();

    if (lfltr_print) RMT_UT_LOG_INFO("=== Learning Filter config w/o timer push");
    tu.lfltr_config(pipe, false/*disable*/, n_entries + (5000-2048) /*something above 2k/4k frames*/);
    // fill up 3k/5k LQs
    for (int loop = 0; loop < (n_entries + 1024); loop++) {
      lq.data.set32(0, (uint32_t)loop);    //XXX: not sure why params to set32 are reveresed - offset, uint32_t
      lf->learn(lq);
    }
    EXPECT_EQ(1, tu.dru_learn_callback_count() );
    // send another 2k - 1k will be dropped, expect second callback
    //   ^- comment does not make sense?? No one who works here anymore knows how this is meant to work.
    //
    for (int loop = (n_entries+1024); loop < (n_entries + n_entries); loop++) {
      lq.data.set32(0, (uint32_t)loop);   //XXX: not sure why params to set32 are reveresed - offset, uint32_t
      lf->learn(lq);
    }
    EXPECT_EQ(2, tu.dru_learn_callback_count() );
    tu.lfltr_clear(pipe); // first buffer
    tu.lfltr_clear(pipe); // second buffer
    tu.zero_dru_learn_callback_count();

    printf("\n");
    om->dump_stats();
  }
}
