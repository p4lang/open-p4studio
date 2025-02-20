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

// XXX -> test_dv83.cpp
// Selector ALU input mismatches between RTL and refmodel (forwarding case)
//  (Search for XXX to find the code that checks the input of the selector in the failing case)
// runSim.py +test=mau_exact_backed_tcam_test_c -f knobs/huffman128.knobs -f knobs/dataplane.knobs +*stateful_cfg_num=2 -f knobs/stateful_alu3.knobs -f questa.knobs +seed=3185501992663370201 -f config/dump.knobs +UVM_VERBOSITY=UVM_HIGH --run_dir=run_dataplane_dev5 -f config/refmodel.debugall.knobs +seed=3185501992663370201

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

  bool dv83_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv83Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv83_print) RMT_UT_LOG_INFO("test_dv83_packet1()\n");
    
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
    tu.set_dv_test(83);    

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

    ASSERT_TRUE(tu.read_test_file("extracted_tests/test_dv83.test"));

    tu.finish_test();
    tu.quieten_log_flags();
}


}
    
