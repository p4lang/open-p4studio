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
#include <packet.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool actions_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;



  TEST(BFN_TEST_NAME(ActionsTest),Basic) {
    if (actions_print) RMT_UT_LOG_INFO("test_actions_basic()\n");    
    int chip = 202;
    int pipe = 0;
    int stage = 0;
 
    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);

    
    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ONE; stages = HI; types = ALL; rows_tabs = FEW; cols = TOP; flags = NON;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Relax check on whether mutliple writers write a data bus
    MauLogicalRow::kRelaxDataMultiWriteCheck = true;


    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
    
    // Switch on fatal/error/warn messages - happens by default these days
    //tu.update_log_flags(0, 0, 0, 0, 0, UINT64_C(0x7), ALL);
    
    // Switch *ON* specific outputs
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeParser;
    //flags = RmtDebug::kRmtDebugParserParseLoop;
    //tu.update_log_flags(pipes, 0, types, 0, 0, flags, ALL);
    
    // Switch *OFF* certain output
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeMauTcam;
    //flags = RmtDebug::kRmtDebugMauTcam_Tcam3LookupDetail|RmtDebug::kRmtDebugMauTcam_Tcam3DebugMiss;
    //tu.update_log_flags(pipes, 0, types, 0, 0, 0, ~flags);
    
    // DEBUG setup done!!!!!!!!!!!!!!!!!!!!!!

    
    // Instantiate whole chip and fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    // Lookup this Pipe/Stage MAU and MAU_ADDR_DIST obj
    Mau *mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    MauAddrDist *adist = mau->mau_addr_dist();
    ASSERT_TRUE(adist != NULL);
    MauSramRow *row6 = mau->sram_row_lookup(6);
    ASSERT_TRUE(row6 != NULL);
    MauSramRowReg *rowreg6 = row6->row_registers();
    ASSERT_TRUE(rowreg6 != NULL);

    
    
    // Set all PHV ingress/egress threads for our 2 stages (chip9!)
    for (stage = 0; stage < 2; stage++) {
      tu.set_phv_range_all(stage, false);
      // Then set 1st 32 of the 64x32 64x8 to be ingress
      // and the 1st 48 of the 96x16 to be ingress
      tu.set_phv_ranges(stage, 32,0, 32,0, 32,16,0, true);
    }
    
    // Setup ingress dependencies for stages 
    tu.set_dependency(0, TestUtil::kDepAction, true);
    tu.set_dependency(1, TestUtil::kDepAction, true);
    // Setup egress dependencies for stages 
    tu.set_dependency(0, TestUtil::kDepConcurrent, false);
    tu.set_dependency(1, TestUtil::kDepConcurrent, false);

    // Setup single logical table for ingress in stage0
    tu.table_config(0, 0, true);    // stage0  table0  ingress


    // UP debug now
    //flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    uint64_t val;
    
    // Then setup some ACTION SRAMs on logical row 12
    tu.sram_config(0, 6, 2,                            // stage0 row6 col2
                   TestUtil::kUnitramTypeAction, 0, 0, // type in_bus=0 out_bus=0
                   0, -1,                              // log_table=0 log_tcam=-1
                   7, 8, false);                       // vpn0=7 vpn1=8 egress=F
    tu.sram_config(0, 6, 3,                            // stage0 row6 col3
                   TestUtil::kUnitramTypeAction, 0, 0, // type in_bus=0 out_bus=0
                   0, -1,                              // log_table=0 log_tcam=-1
                   7, 8, false);                       // vpn0=7 vpn1=8 egress=F
    // Both set mux_inp=4 (oflo) with src_idx=5(logrow13) and src_sel=0(ACTION)
    // and drive RD data onto oflo(2)
    //            S  R  C   MI  RD  WR  LT
    tu.mux_config(0, 6, 2,   4,  2, -1,  0, true, 5, 0, false);
    tu.mux_config(0, 6, 3,   4,  2, -1,  0, true, 5, 0, false);

    val = UINT64_C(0x1111111111111111); 
    for (int i = 0; i < 1024; i++) tu.sram_write(0, 6, 2, i, val, val);
    val = UINT64_C(0x2222222222222222); 
    for (int i = 0; i < 1024; i++) tu.sram_write(0, 6, 3, i, val, val);


    
    // Now ACTION SRAMs on logical row 13
    tu.sram_config(0, 6, 10,                           // stage0 row6 col10
                   TestUtil::kUnitramTypeAction, 0, 0, // type in_bus=0 out_bus=0
                   0, -1,                              // log_table=0 log_tcam=-1
                   7, 8, false);                       // vpn0=7 vpn1=8 egress=F
    tu.sram_config(0, 6, 11,                           // stage0 row6 col11
                   TestUtil::kUnitramTypeAction, 0, 0, // type in_bus=0 out_bus=0
                   0, -1,                              // log_table=0 log_tcam=-1
                   7, 8, false);                       // vpn0=7 vpn1=8 egress=F
    // Both set mux_inp=1 (action) - drive RD data onto action(4)
    //            S  R   C  MI  RD  WR   LT
    tu.mux_config(0, 6, 10,  1,  4, -1,  0, false, 0, 0, false);
    tu.mux_config(0, 6, 11,  1,  4, -1,  0, false, 0, 0, false);

    val = UINT64_C(0x4444444444444444); 
    for (int i = 0; i < 1024; i++) tu.sram_write(0, 6, 10, i, val, val);
    val = UINT64_C(0x8888888888888888); 
    for (int i = 0; i < 1024; i++) tu.sram_write(0, 6, 11, i, val, val);


    
    // Create a simple PHV (so we can set a
    // pkt length increment for byte counters)
    Eop eop;
    Phv *phv = tu.phv_alloc();
    int action_vpn;
    int action_index;
    uint32_t action_addr;

    //int pktlen = 100;
    int n_pkts = 1;

    // Now loop pushing packets in.....
    for (int i = 0; i < n_pkts; i++) {
      if ((i % 2) == 0) action_vpn = 7; else action_vpn = 8;
      action_index = 15;
      action_addr = (1u << 22) | (action_vpn << 15) | (action_index << 5) | 0xF;

      // Now loop calling:
      //  MAU->reset_resources()
      //  MAU-ADDR_DIST->distrib_action_addresses to distribute a fake ACTION address
      //  MAU->process_actions_for_test_actions() to see SRAMs do their work!
      //
      mau->reset_resources();
      adist->distrib_action_addresses(0, true, action_addr);
      mau->process_for_tests(phv, eop);

      BitVector<128> data(UINT64_C(0));
      uint32_t addr = 0u; 
      rowreg6->r_action(&data, &addr);
      // All 128 bits should be 1
      EXPECT_EQ(data.get_word(0), ALL);
      EXPECT_EQ(data.get_word(64), ALL);
    }

    
    // Free
    tu.phv_free(phv);
    // Schtum
    tu.finish_test();
    tu.quieten_log_flags();
  }
}
