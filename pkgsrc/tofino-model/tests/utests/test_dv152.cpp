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

// XXX -> test_dv152.cpp
// Meter is not read correctly as Refmodel doesn't source read from the correct address
// runSim.py +test=mau_first_test_c -f knobs/exact_2tblmeter_rand.knobs -f knobs/huffman_rand_small.knobs +*.mss_alu_mask=0x0880 +*.ram_unit_mask07=0x3F +*.ram_unit_mask05=0x3F +*.mapram_mask3=0XFC0 +*.mapram_mask2=0XFC0 +*.em_imem_addr_mask=0x3f +*.em_max_mdsize=1 -f knobs/meter_hashdist_enable.knobs -f questa.knobs +UVM_VERBOSITY=UVM_HIGH +UVM_MAX_QUIT_COUNT=1 +seed=4154101120332707970 --skiplogzip -f config/dump.knobs -f config/refmodel.debugall.knobs +seed=4154101120332707970 --run_dir=run_dir_test_dv152
//# UVM_ERROR @  60800ps uvm_test_top.mau_env.dpath_sb>mau_datapath_sb.svh(489) [uvm_test_top.mau_env.dpath_sb] phv: #4e1# eop:#1e1# meter_input[ 2] rtl = 0x0000000000200000200c08602c80b201, ref = 0x00000000000000000000000000000000
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

  bool dv152_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv152Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv152_print) RMT_UT_LOG_INFO("test_dv152_packet1()\n");

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
    tu.set_dv_test(152);
    // Relax OfloWrMUX multiple active input check
    MauSramRowReg::kRelaxOfloWrMuxCheck = true;
    // Relax Synth2Port checking
    MauSramRowReg::kRelaxSynth2PortFabricCheck = true;
    // Not using DinPower
    Mau::kMauDinPowerMode = false;



    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Get handles for ObjectManager, MAU, port
    // Setup a port on pipe 0
    // This also sets up basic config ingress parser and deparser
    RmtObjectManager *om = tu.get_objmgr();
    (void)om->mau_lookup(pipe, stage);
    (void)tu.port_get(16);



    ASSERT_TRUE(tu.read_test_file("extracted_tests/test_dv152.test"));



    tu.finish_test();
    tu.quieten_log_flags();
}


}
