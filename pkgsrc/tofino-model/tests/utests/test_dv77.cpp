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

//  test_dv77.cpp
// Amit's RED test
//   runSim.py +test=mau_first_test_c -f knobs/exact_lpfred_curve.knobs +*.meter_red_exp=0x3 +*.meter_red_l100_mant=0xB0 +*.meter_red_max_mant=0x20 +*.meter_red_l0_mant=0x10 -f knobs/huffman_rand_small.knobs +*.em_imem_addr_mask=0x3f +*.em_max_mdsize=1 +*.meter_alu_bmask=0x1 +*.ematch_addrxbar_hash_bmask=0 +*.meter_lpf_value=0 +*.meter_red_prob_scale=0 -f knobs/meter_hashdist_enable.knobs +UVM_VERBOSITY=UVM_NONE +UVM_MAX_QUIT_COUNT=1 -f config/refmodel.debugall.knobs +seed=1586242971 +seed=1586242971 +*n_phvs=10 +seed=1586242971
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

  bool dv77_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv77Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv77_print) RMT_UT_LOG_INFO("test_dv77_packet1()\n");
    
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
    tu.set_dv_test(77);    

    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);    
    
    
    // PUT GENERATED OutWord/IndirectWrite calls HERE
    
    ASSERT_TRUE(tu.read_test_file("extracted_tests/test_dv77.test"));



    tu.finish_test();
    tu.quieten_log_flags();
}


}
    
