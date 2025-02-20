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

#include <bitvector.h>
#include <rmt-object-manager.h>
#include <model_core/model.h>
#include <mau.h>
#include <eop.h>
#include <deparser-block.h>
#include <deparser.h>
#include <port.h>
#include <register_utils.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(HomerowColorTest),Packet1) {
    RMT_UT_LOG_INFO("test_homerow_color_packet1()\n");
    bool jbay = RmtObject::is_jbay_or_later();
    ASSERT_TRUE(jbay);

    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = TOP; cols = NON; flags = FEW;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil *tu;
    RmtObjectManager *om;
    Mau *mau;


    // First try with kRelaxColorBusDiffAluCheck true
    GLOBAL_MODEL->Reset();
    tu = new TestUtil(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu->update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
    om = tu->get_objmgr();
    ASSERT_TRUE(om != NULL);
    mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    //(void)mau->mau_info_read("MAU_ERRORS", true);
    //(void)mau->mau_info_read("MAU_ERRORS_2", true);
    MauColorSwitchbox::kRelaxColorBusDiffAluCheck = true; // Relax
    EXPECT_NO_THROW(tu->read_test_file("extracted_tests/test_homerow_color.test"));
    int try1_errors1 = mau->mau_info_read("MAU_ERRORS", true);
    int try1_errors2 = mau->mau_info_read("MAU_ERRORS_2", true);
    EXPECT_EQ(0, try1_errors1);
    EXPECT_EQ(7, try1_errors2);
    tu->finish_test();
    tu->quieten_log_flags();
    delete tu;


    // Second try with kRelaxColorBusDiffAluCheck false
    GLOBAL_MODEL->Reset();
    tu = new TestUtil(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu->update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
    om = tu->get_objmgr();
    ASSERT_TRUE(om != NULL);
    mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    MauColorSwitchbox::kRelaxColorBusDiffAluCheck = false; // NO relax
    EXPECT_NO_THROW(tu->read_test_file("extracted_tests/test_homerow_color.test"));
    int try2_errors1 = mau->mau_info_read("MAU_ERRORS", false);
    int try2_errors2 = mau->mau_info_read("MAU_ERRORS_2", false);
    // Expect num 'ERROR's to increase and num 'error's to decrease
    EXPECT_EQ(7, try2_errors1);
    EXPECT_EQ(0, try2_errors2);
    tu->finish_test();
    tu->quieten_log_flags();
    delete tu;

    printf("Try1Errs=%d,%d Try2Errs=%d,%d\n",
           try1_errors1, try1_errors2, try2_errors1, try2_errors2);
}


}
