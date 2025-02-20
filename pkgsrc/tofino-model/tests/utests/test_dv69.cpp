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

// Stateful tests from running RTL
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

  bool dv69_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv69Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv69_print) RMT_UT_LOG_INFO("test_dv69_packet1()\n");
    
    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = ALL; flags = ALL;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Added next line 10/04/2015 - DV tests derived from this file
    // from now on, will be assumed to set PerFlowEnables in addresses.
    tu.set_pfe_mode(true); 

    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    //flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);    
    

    RmtObjectManager *om = tu.get_objmgr();
    //Mau *mau = om->mau_lookup(pipe, stage);
    int which_alu=0;
    int logrow = MauMeterAlu::get_meter_alu_logrow_index(which_alu);
    MauStatefulAlu* salu = om->mau_get(pipe, stage)->logical_row_lookup(logrow)->mau_meter_alu()->get_salu();
    assert( salu );

    //uint32_t addr;
    //BitVector<64> phv_data_word;
    //BitVector<MauDefs::kDataBusWidth> data_in_bv;
    //BitVector<MauDefs::kDataBusWidth> data_out_bv;
    //BitVector<MauDefs::kActionOutputBusWidth> action_out;
    //
    //auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);

    ASSERT_TRUE(tu.read_test_file("extracted_tests/test_dv69.test"));
#pragma GCC diagnostic ignored "-Wunused-variable"    

    // Uncomment below to up the debug output
    //flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


    

    tu.finish_test();
    tu.quieten_log_flags();
}


}
    
