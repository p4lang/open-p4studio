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

// XXX -> test_dv81.cpp
//  Comparing output of stateful alu when combined with selector. For first phv stateful runs, so 
//    get_output call should return stuff. For second alu stateful does not run (selector runs) so
//    stateful's get_output should return all zeros.
// runSim.py +test=mau_exact_backed_tcam_test_c -f knobs/huffman128.knobs -f knobs/dataplane.knobs -f config/refmodel.debugall.knobs +seed=4176419001230391343 -f config/dump.knobs +UVM_VERBOSITY=UVM_HIGH --run_dir=run_dataplane_dev4 +*stateful_cfg_num=2 +seed=1234 -f knobs/stateful_alu3.knobs +seed=1234 

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

  bool dv81_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv81Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv81_print) RMT_UT_LOG_INFO("test_dv81_packet1()\n");
    
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
    tu.set_dv_test(81);    

    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);    
    
    
    // PUT GENERATED OutWord/IndirectWrite calls HERE
    
    // Get handles for ObjectManager, MAU, port
    // Setup a port on pipe 0
    // This also sets up basic config ingress parser and deparser
    RmtObjectManager *om = tu.get_objmgr();
    Mau *mau = om->mau_lookup(pipe, stage);
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


    ASSERT_TRUE(tu.read_test_file("extracted_tests/test_dv81.test"));

    /////////////////////////////////////////////////////////////////////////////////
    // As this needs extra checking do the process_match / process_action calls here

    mau->handle_eop(*( tu.get_read_eop(1,0) ));
    int ingress_next_table_1e0 = 0;
    int egress_next_table_1e0 = 0;
    Phv *iphv_out_got_1e0 = mau->process_match(tu.get_read_phv('i',"in",1,0), tu.get_read_phv('o',"in",1,0),
                                                &ingress_next_table_1e0, &egress_next_table_1e0);
    Phv *ophv_out_got_1e0 = mau->process_action(tu.get_read_phv('i',"in",1,0), tu.get_read_phv('o',"in",1,0));
    

    {
         int alu = 3;
         int logrow = MauMeterAlu::get_meter_alu_logrow_index(alu);
         uint32_t addr, salu_action_data;
 
         MauMeterAlu* malu = om->mau_get(pipe, stage)->logical_row_lookup(logrow)->mau_meter_alu();
         assert( malu );
         BitVector<128> data0;
         malu->get_output(&data0, &addr, &salu_action_data); //4-32b
         EXPECT_EQ( 0xeefdfbdfu, data0.get_word(0,  32));
         EXPECT_EQ( 0xf3edfff9u, data0.get_word(32, 32));
         EXPECT_EQ( 0xfaf67befu, data0.get_word(64, 32));
         EXPECT_EQ( 0x6700007bu, data0.get_word(96, 32));
    }

    EXPECT_TRUE( TestUtil::compare_phvs(ophv_out_got_1e0,tu.get_read_phv('o',"out",1,0),false) );
    // this works for some tests, but not for others:
    RMT_UT_LOG_INFO("iphv_out_got %s iphv_in\n",
                    TestUtil::compare_phvs(iphv_out_got_1e0,tu.get_read_phv('i',"in",1,0),false) ?
                    "==":"!=");
    

    // this dones the same thing as above (including the phv checking)
    int ingress_next_table_2e0 = 0;
    int egress_next_table_2e0 = 0;
    tu.process_read_phv_match_and_action(2,0, &ingress_next_table_2e0, &egress_next_table_2e0);

    // now check the meter stuff
    {
         int alu = 3;
         int logrow = MauMeterAlu::get_meter_alu_logrow_index(alu);
         uint32_t addr, salu_action_data;
 
         MauMeterAlu* malu = om->mau_get(pipe, stage)->logical_row_lookup(logrow)->mau_meter_alu();
         assert( malu );
         BitVector<128> data0;
         malu->get_output(&data0, &addr, &salu_action_data); //4-32b
         EXPECT_EQ( 0u, data0.get_word(0,  32));
         EXPECT_EQ( 0u, data0.get_word(32, 32));
         EXPECT_EQ( 0u, data0.get_word(64, 32));
         EXPECT_EQ( 0u, data0.get_word(96, 32));
    }
    
    tu.finish_test();
    tu.quieten_log_flags();
}


}
    
