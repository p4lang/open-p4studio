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

// gateway_cntrs -> test_dv184.cpp
// 
//

#include <utests/test_util.h>
#include <iostream>
#include <string>
#include <array>
#include <cassert>

#include "gtest.h"

#include <bitvector.h>
#include <rmt-object-manager.h>
#include <model_core/model.h>
#include <mau.h>
#include <eop.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool dv184_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv184Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv184_print) RMT_UT_LOG_INFO("test_dv184_packet1()\n");
    
    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Set 'vintage' of DV test to configure other global vars correctly
    tu.set_dv_test(184);    

    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);    


    auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);
    // PUT GENERATED OutWord/IndirectWrite calls HERE
    ASSERT_TRUE(tu.read_test_file("extracted_tests/test_dv184.test"));
    // Check counters are what we expect
    EXPECT_EQ(0u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[0][0]));
    EXPECT_EQ(4u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[1][0]));
    EXPECT_EQ(0u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[2][0]));
    EXPECT_EQ(4u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[3][0]));
    EXPECT_EQ(0u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[4][0]));
    EXPECT_EQ(4u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[5][0]));
    EXPECT_EQ(0u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[6][0]));
    EXPECT_EQ(4u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[7][0]));
    EXPECT_EQ(4u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[8][0]));
    EXPECT_EQ(0u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[9][0]));
    EXPECT_EQ(4u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[10][0]));
    EXPECT_EQ(0u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[11][0]));
    EXPECT_EQ(0u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[12][0]));
    EXPECT_EQ(4u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[13][0]));
    EXPECT_EQ(4u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[14][0]));
    EXPECT_EQ(0u, tu.InWord(&mau_reg_map.rams.match.merge.mau_table_counter[15][0]));

    tu.finish_test();
    tu.quieten_log_flags();
}


}
    
