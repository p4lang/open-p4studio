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

// Generic DV utest - symlink extracted_tests/test_dv999.test
//                    to whichever run.log.out you want.

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

  bool dv999_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv999Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv999_print) RMT_UT_LOG_INFO("test_dv999_packet1()\n");

    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = TOP; cols = NON; flags = FEW;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Set 'vintage' of DV test to configure other global vars correctly
    tu.set_dv_test(999);
    MauLookupResult::kRelaxLookupShiftOpPosCheck = true;
    MauPredicationCommon::kRelaxPredicationCheck = true;
    MauStatefulCounters::kRelaxSwPushPopInvalidErrors = true;
    MauDependencies::kRelaxThreadCheck = true;

    // Just to stop compiler complaining about unused vars
    //flags = ALL; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    //auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);
    try {
      (void)tu.read_test_file("extracted_tests/test_dv999.test");
      // Can comment out above and use this to include extracted deparser tests
      //  (until test_reader can read them)
#ifdef MODEL_CHIP_JBAY_OR_LATER
      //#include "deparser_dv_test.cpp"
#endif
    } catch (...) {
      EXPECT_FALSE(true) << "Exception thrown!";
    }

    tu.finish_test();
    tu.quieten_log_flags();
}


}
