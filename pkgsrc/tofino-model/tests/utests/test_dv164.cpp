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

// ref_model_issue -> test_dv164.cpp
//  runSim.py +test=mau_first_test_c -f knobs/exact_1tblhit.knobs +*.exact_ram_mask*=0 +*.exact_ram_mask0=0x008 -f knobs/huffman8.knobs +*.n_phvs=50 +*.em_imem_addr_mask=0x3f -f questa.knobs +UVM_VERBOSITY=UVM_HIGH +UVM_MAX_QUIT_COUNT=1 +seed=1733638364820645281 --skiplogzip -f config/dump.knobs -f config/refmodel.debugall.knobs +seed=1733638364820645281 --run_dir=run_dir_test_dv164
// run_dir_test_dv164 # UVM_ERROR @  57600ps uvm_test_top.mau_env.dpath_sb>mau_datapath_sb.svh(2286) [SB_RTL] #1e0#  Match Action action_databus RTL not match RefM bus=4 ( mask=b0001 rtl=0x0000_cdcd vs ref=0x0000_0000)
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

  bool dv164_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv164Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv164_print) RMT_UT_LOG_INFO("test_dv164_packet1()\n");
    
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
    tu.set_dv_test(164);    

    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);    
    
    
    ASSERT_TRUE(tu.read_test_file("extracted_tests/test_dv164.test"));




    tu.finish_test();
    tu.quieten_log_flags();
}


}
    
