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

// XXX -> test_dv79.cpp
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

  bool dv79_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv79Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv79_print) RMT_UT_LOG_INFO("test_dv79_packet1()\n");
    
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
    tu.set_dv_test(79);

    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);    



    // Get handles for ObjectManager, MAU, port
    // Setup a port on pipe 0
    // This also sets up basic config ingress parser and deparser
    RmtObjectManager *om = tu.get_objmgr();
    (void)om->mau_lookup(pipe, stage);
    (void)tu.port_get(16);
    // Uncomment below to up the debug output
    //flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    
    // don't check that the total is correct in Adjust Total instructions to avoid assertions when
    //  data is put in which hasn't got a correct total (eg some forwarding cases)
    for (int which_alu=0; which_alu<4; which_alu++) {
      int logrow = MauMeterAlu::get_meter_alu_logrow_index(which_alu);
      MauStatefulAlu* salu = om->mau_get(pipe, stage)->logical_row_lookup(logrow)->mau_meter_alu()->get_salu();
      assert( salu );
      salu->set_check_total_correct(false);
    }

    // meter thread registers don't seem to be programmed in this test
    MauMeterAlu::kRelaxThreadCheck = true;

    // PUT GENERATED OutWord/IndirectWrite calls HERE
    ASSERT_TRUE(tu.read_test_file("extracted_tests/test_dv79.test"));

    tu.finish_test();
    tu.quieten_log_flags();
}


}
    
