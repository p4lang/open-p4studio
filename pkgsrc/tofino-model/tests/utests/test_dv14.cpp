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

// XXX -> test_dv14.cpp
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

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

  bool dv14_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv14Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv14_print) RMT_UT_LOG_INFO("test_dv14_packet1()\n");
    
    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_evaluate_all(true);
    tu.set_free_on_exit(true);
    tu.set_dv_test(14);
    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
    
    
    auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);
    tu.OutWordPiT(0, 0, &mau_reg_map.dp.phv_ingress_thread[0][0], 0xffffffff); /* 0x2060180 */
    tu.OutWordPiT(0, 1, &mau_reg_map.dp.phv_ingress_thread[0][1], 0xffffffff); /* 0x2060184 */
    tu.OutWordPiT(0, 2, &mau_reg_map.dp.phv_ingress_thread[0][2], 0xffffffff); /* 0x2060188 */
    tu.OutWordPiT(0, 3, &mau_reg_map.dp.phv_ingress_thread[0][3], 0xffffffff); /* 0x206018c */
    tu.OutWordPiT(0, 4, &mau_reg_map.dp.phv_ingress_thread[0][4], 0xffffffff); /* 0x2060190 */
    tu.OutWordPiT(0, 5, &mau_reg_map.dp.phv_ingress_thread[0][5], 0xffffffff); /* 0x2060194 */
    tu.OutWordPiT(0, 6, &mau_reg_map.dp.phv_ingress_thread[0][6], 0xffffffff); /* 0x2060198 */
    tu.OutWordPiT(1, 0, &mau_reg_map.dp.phv_ingress_thread[1][0], 0xffffffff); /* 0x20601a0 */
    tu.OutWordPiT(1, 1, &mau_reg_map.dp.phv_ingress_thread[1][1], 0xffffffff); /* 0x20601a4 */
    tu.OutWordPiT(1, 2, &mau_reg_map.dp.phv_ingress_thread[1][2], 0xffffffff); /* 0x20601a8 */
    tu.OutWordPiT(1, 3, &mau_reg_map.dp.phv_ingress_thread[1][3], 0xffffffff); /* 0x20601ac */
    tu.OutWordPiT(1, 4, &mau_reg_map.dp.phv_ingress_thread[1][4], 0xffffffff); /* 0x20601b0 */
    tu.OutWordPiT(1, 5, &mau_reg_map.dp.phv_ingress_thread[1][5], 0xffffffff); /* 0x20601b4 */
    tu.OutWordPiT(1, 6, &mau_reg_map.dp.phv_ingress_thread[1][6], 0xffffffff); /* 0x20601b8 */
    tu.OutWord(&mau_reg_map.dp.action_output_delay[0], 0xc); /* 0x2060030 */
    //Removed in bfnregs 20150107_182406_7982_mau_dev
    //tu.OutWord(&mau_reg_map.dp.final_output_delay[0][0], 0x1); /* 0x2060040 */
    tu.OutWord(&mau_reg_map.dp.pipelength_added_stages[0], 0x1); /* 0x2060070 */
    tu.OutWord(&mau_reg_map.dp.action_output_delay[1], 0xb); /* 0x2060038 */
    //Removed in bfnregs 20150107_182406_7982_mau_dev
    //tu.OutWord(&mau_reg_map.dp.final_output_delay[1][0], 0x3); /* 0x2060048 */
    tu.OutWord(&mau_reg_map.dp.match_ie_input_mux_sel, 0x3); /* 0x2060108 */
    tu.OutWord(&mau_reg_map.dp.phv_fifo_enable, 0xc); /* 0x2060120 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][0], 0x10); /* 0x2067000 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][1], 0x10); /* 0x2067004 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][2], 0x10); /* 0x2067008 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][3], 0x10); /* 0x206700c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][4], 0x11); /* 0x2067010 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][5], 0x11); /* 0x2067014 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][6], 0x11); /* 0x2067018 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][7], 0x11); /* 0x206701c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][8], 0x12); /* 0x2067020 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][9], 0x12); /* 0x2067024 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][10], 0x12); /* 0x2067028 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][11], 0x12); /* 0x206702c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][12], 0x13); /* 0x2067030 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][13], 0x13); /* 0x2067034 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][14], 0x13); /* 0x2067038 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][15], 0x13); /* 0x206703c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][16], 0x14); /* 0x2067040 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][17], 0x14); /* 0x2067044 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][18], 0x14); /* 0x2067048 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][19], 0x14); /* 0x206704c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][20], 0x15); /* 0x2067050 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][21], 0x15); /* 0x2067054 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][22], 0x15); /* 0x2067058 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][23], 0x15); /* 0x206705c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][24], 0x16); /* 0x2067060 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][25], 0x16); /* 0x2067064 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][26], 0x16); /* 0x2067068 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][27], 0x16); /* 0x206706c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][28], 0x17); /* 0x2067070 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][29], 0x17); /* 0x2067074 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][30], 0x17); /* 0x2067078 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][31], 0x17); /* 0x206707c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][32], 0x18); /* 0x2067080 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][33], 0x18); /* 0x2067084 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][34], 0x18); /* 0x2067088 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][35], 0x18); /* 0x206708c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][36], 0x19); /* 0x2067090 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][37], 0x19); /* 0x2067094 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][38], 0x19); /* 0x2067098 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][39], 0x19); /* 0x206709c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][40], 0x1a); /* 0x20670a0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][41], 0x1a); /* 0x20670a4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][42], 0x1a); /* 0x20670a8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][43], 0x1a); /* 0x20670ac */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][44], 0x1b); /* 0x20670b0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][45], 0x1b); /* 0x20670b4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][46], 0x1b); /* 0x20670b8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][47], 0x1b); /* 0x20670bc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][48], 0x1c); /* 0x20670c0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][49], 0x1c); /* 0x20670c4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][50], 0x1c); /* 0x20670c8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][51], 0x1c); /* 0x20670cc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][52], 0x1d); /* 0x20670d0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][53], 0x1d); /* 0x20670d4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][54], 0x1d); /* 0x20670d8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][55], 0x1d); /* 0x20670dc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][56], 0x1e); /* 0x20670e0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][57], 0x1e); /* 0x20670e4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][58], 0x1e); /* 0x20670e8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][59], 0x1e); /* 0x20670ec */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][60], 0x1f); /* 0x20670f0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][61], 0x1f); /* 0x20670f4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][62], 0x1f); /* 0x20670f8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][63], 0x1f); /* 0x20670fc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][64], 0x10); /* 0x2067500 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][65], 0x10); /* 0x2067504 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][66], 0x10); /* 0x2067508 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][67], 0x10); /* 0x206750c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][68], 0x11); /* 0x2067510 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][69], 0x11); /* 0x2067514 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][70], 0x11); /* 0x2067518 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][71], 0x11); /* 0x206751c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][72], 0x12); /* 0x2067520 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][73], 0x12); /* 0x2067524 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][74], 0x12); /* 0x2067528 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][75], 0x12); /* 0x206752c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][76], 0x13); /* 0x2067530 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][77], 0x13); /* 0x2067534 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][78], 0x13); /* 0x2067538 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][79], 0x13); /* 0x206753c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][80], 0x14); /* 0x2067540 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][81], 0x14); /* 0x2067544 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][82], 0x14); /* 0x2067548 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][83], 0x14); /* 0x206754c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][84], 0x15); /* 0x2067550 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][85], 0x15); /* 0x2067554 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][86], 0x15); /* 0x2067558 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][87], 0x15); /* 0x206755c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][88], 0x16); /* 0x2067560 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][89], 0x16); /* 0x2067564 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][90], 0x16); /* 0x2067568 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][91], 0x16); /* 0x206756c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][92], 0x17); /* 0x2067570 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][93], 0x17); /* 0x2067574 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][94], 0x17); /* 0x2067578 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][95], 0x17); /* 0x206757c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][96], 0x18); /* 0x2067580 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][97], 0x18); /* 0x2067584 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][98], 0x18); /* 0x2067588 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][99], 0x18); /* 0x206758c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][100], 0x19); /* 0x2067590 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][101], 0x19); /* 0x2067594 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][102], 0x19); /* 0x2067598 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][103], 0x19); /* 0x206759c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][104], 0x1a); /* 0x20675a0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][105], 0x1a); /* 0x20675a4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][106], 0x1a); /* 0x20675a8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][107], 0x1a); /* 0x20675ac */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][108], 0x1b); /* 0x20675b0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][109], 0x1b); /* 0x20675b4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][110], 0x1b); /* 0x20675b8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][111], 0x1b); /* 0x20675bc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][112], 0x1c); /* 0x20675c0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][113], 0x1c); /* 0x20675c4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][114], 0x1c); /* 0x20675c8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][115], 0x1c); /* 0x20675cc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][116], 0x1d); /* 0x20675d0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][117], 0x1d); /* 0x20675d4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][118], 0x1d); /* 0x20675d8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][119], 0x1d); /* 0x20675dc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][120], 0x1e); /* 0x20675e0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][121], 0x1e); /* 0x20675e4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][122], 0x1e); /* 0x20675e8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][123], 0x1e); /* 0x20675ec */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][124], 0x1f); /* 0x20675f0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][125], 0x1f); /* 0x20675f4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][126], 0x1f); /* 0x20675f8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][127], 0x1f); /* 0x20675fc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][128], 0x10); /* 0x2067a00 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][129], 0x10); /* 0x2067a04 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][130], 0x10); /* 0x2067a08 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][131], 0x10); /* 0x2067a0c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][132], 0x11); /* 0x2067a10 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][133], 0x11); /* 0x2067a14 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][134], 0x11); /* 0x2067a18 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][135], 0x11); /* 0x2067a1c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][136], 0x12); /* 0x2067a20 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][137], 0x12); /* 0x2067a24 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][138], 0x12); /* 0x2067a28 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][139], 0x12); /* 0x2067a2c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][140], 0x13); /* 0x2067a30 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][141], 0x13); /* 0x2067a34 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][142], 0x13); /* 0x2067a38 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][143], 0x13); /* 0x2067a3c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][144], 0x14); /* 0x2067a40 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][145], 0x14); /* 0x2067a44 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][146], 0x14); /* 0x2067a48 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][147], 0x14); /* 0x2067a4c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][148], 0x15); /* 0x2067a50 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][149], 0x15); /* 0x2067a54 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][150], 0x15); /* 0x2067a58 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][151], 0x15); /* 0x2067a5c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][152], 0x16); /* 0x2067a60 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][153], 0x16); /* 0x2067a64 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][154], 0x16); /* 0x2067a68 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][155], 0x16); /* 0x2067a6c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][156], 0x17); /* 0x2067a70 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][157], 0x17); /* 0x2067a74 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][158], 0x17); /* 0x2067a78 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][159], 0x17); /* 0x2067a7c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][160], 0x18); /* 0x2067a80 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][161], 0x18); /* 0x2067a84 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][162], 0x18); /* 0x2067a88 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][163], 0x18); /* 0x2067a8c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][164], 0x19); /* 0x2067a90 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][165], 0x19); /* 0x2067a94 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][166], 0x19); /* 0x2067a98 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][167], 0x19); /* 0x2067a9c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][168], 0x1a); /* 0x2067aa0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][169], 0x1a); /* 0x2067aa4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][170], 0x1a); /* 0x2067aa8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][171], 0x1a); /* 0x2067aac */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][172], 0x1b); /* 0x2067ab0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][173], 0x1b); /* 0x2067ab4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][174], 0x1b); /* 0x2067ab8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][175], 0x1b); /* 0x2067abc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][176], 0x1c); /* 0x2067ac0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][177], 0x1c); /* 0x2067ac4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][178], 0x1c); /* 0x2067ac8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][179], 0x1c); /* 0x2067acc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][180], 0x1d); /* 0x2067ad0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][181], 0x1d); /* 0x2067ad4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][182], 0x1d); /* 0x2067ad8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][183], 0x1d); /* 0x2067adc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][184], 0x1e); /* 0x2067ae0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][185], 0x1e); /* 0x2067ae4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][186], 0x1e); /* 0x2067ae8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][187], 0x1e); /* 0x2067aec */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][188], 0x1f); /* 0x2067af0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][189], 0x1f); /* 0x2067af4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][190], 0x1f); /* 0x2067af8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][191], 0x1f); /* 0x2067afc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][192], 0x10); /* 0x2067f00 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][193], 0x10); /* 0x2067f04 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][194], 0x10); /* 0x2067f08 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][195], 0x10); /* 0x2067f0c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][196], 0x11); /* 0x2067f10 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][197], 0x11); /* 0x2067f14 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][198], 0x11); /* 0x2067f18 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][199], 0x11); /* 0x2067f1c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][200], 0x12); /* 0x2067f20 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][201], 0x12); /* 0x2067f24 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][202], 0x12); /* 0x2067f28 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][203], 0x12); /* 0x2067f2c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][204], 0x13); /* 0x2067f30 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][205], 0x13); /* 0x2067f34 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][206], 0x13); /* 0x2067f38 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][207], 0x13); /* 0x2067f3c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][208], 0x14); /* 0x2067f40 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][209], 0x14); /* 0x2067f44 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][210], 0x14); /* 0x2067f48 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][211], 0x14); /* 0x2067f4c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][212], 0x15); /* 0x2067f50 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][213], 0x15); /* 0x2067f54 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][214], 0x15); /* 0x2067f58 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][215], 0x15); /* 0x2067f5c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][0], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][1], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][2], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][3], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][4], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][5], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][6], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][7], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][8], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][9], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][10], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][11], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][12], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][13], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][14], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][15], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][16], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][17], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][18], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][19], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][20], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][21], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][22], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][23], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][24], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][25], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][26], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][27], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][28], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][29], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][30], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][31], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][128], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][129], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][130], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][131], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][132], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][133], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][134], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][135], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][136], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][137], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][138], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][139], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][140], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][141], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][142], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][143], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][144], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][145], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][146], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][147], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][148], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][149], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][150], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][151], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][152], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][153], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][154], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][155], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][156], 0x17); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][157], 0x17); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][158], 0x17); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][159], 0x17); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][32], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][33], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][34], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][35], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][36], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][37], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][38], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][39], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][40], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][41], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][42], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][43], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][44], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][45], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][46], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][47], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][48], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][49], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][50], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][51], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][52], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][53], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][54], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][55], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][56], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][57], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][58], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][59], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][60], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][61], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][62], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][63], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][160], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][161], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][162], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][163], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][164], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][165], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][166], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][167], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][168], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][169], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][170], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][171], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][172], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][173], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][174], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][175], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][176], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][177], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][178], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][179], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][180], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][181], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][182], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][183], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][184], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][185], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][186], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][187], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][188], 0x17); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][189], 0x17); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][190], 0x17); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][191], 0x17); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][64], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][65], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][66], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][67], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][68], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][69], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][70], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][71], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][72], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][73], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][74], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][75], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][76], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][77], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][78], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][79], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][80], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][81], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][82], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][83], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][84], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][85], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][86], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][87], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][88], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][89], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][90], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][91], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][92], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][93], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][94], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][95], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][192], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][193], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][194], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][195], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][96], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][97], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][98], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][99], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][100], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][101], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][102], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][103], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][104], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][105], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][106], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][107], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][108], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][109], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][110], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][111], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][112], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][113], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][114], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][115], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][116], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][117], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][118], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][119], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][120], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][121], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][122], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][123], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][124], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][125], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][126], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][127], 0xf); // regs_31841 fix
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_mode[0], 0x3594000); /* 0x20406c0 */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_table_map[0], 0x1); /* 0x2040680 */
    tu.OutWord(&mau_reg_map.tcams.tcam_match_adr_shift[0], 0x4); /* 0x2040420 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.unitram_config[0][0], 0x18ca); /* 0x201cb80 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.unitram_config[0][4], 0x185e); /* 0x201eba0 */
    tu.OutWord(&mau_reg_map.rams.array.row[1].ram[0].unit_ram_ctl, 0x20); /* 0x2009018 */
    tu.OutWord(&mau_reg_map.rams.array.row[5].ram[4].unit_ram_ctl, 0x200); /* 0x200d218 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tind_ram_data_size[10], 0x5); /* 0x2016528 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][0], 0x19); /* 0x2040380 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][0], 0x2); /* 0x2040200 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][1], 0x6); /* 0x2040384 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][1], 0x1); /* 0x2040204 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][0], 0x0); /* 0x2040300 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][0], 0x3); /* 0x2040000 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][1], 0x7); /* 0x2040004 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][2], 0x0); /* 0x2040008 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][3], 0x8); /* 0x204000c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][4], 0x3); /* 0x2040010 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][5], 0x6); /* 0x2040014 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][6], 0xa); /* 0x2040018 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][7], 0x2); /* 0x204001c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][0], 0x1d); /* 0x20403c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][0], 0x2); /* 0x2040240 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][1], 0x10); /* 0x20403c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][1], 0x1); /* 0x2040244 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][0], 0xf); /* 0x2040320 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][0], 0x2); /* 0x2040100 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][1], 0x6); /* 0x2040104 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][2], 0x0); /* 0x2040108 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][3], 0x4); /* 0x204010c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][4], 0x0); /* 0x2040110 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][5], 0xa); /* 0x2040114 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][6], 0x4); /* 0x2040118 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][7], 0x6); /* 0x204011c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][2], 0x1a); /* 0x2040388 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][2], 0x2); /* 0x2040208 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][3], 0x1); /* 0x204038c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][3], 0x0); /* 0x204020c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][1], 0x2); /* 0x2040304 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][0], 0x0); /* 0x2040020 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][1], 0x5); /* 0x2040024 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][2], 0x8); /* 0x2040028 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][3], 0xa); /* 0x204002c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][4], 0x7); /* 0x2040030 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][5], 0x4); /* 0x2040034 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][6], 0x5); /* 0x2040038 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][7], 0x4); /* 0x204003c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][2], 0xa); /* 0x20403c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][2], 0x1); /* 0x2040248 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][3], 0x1a); /* 0x20403cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][3], 0x2); /* 0x204024c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][1], 0x9); /* 0x2040324 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][0], 0x5); /* 0x2040120 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][1], 0x2); /* 0x2040124 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][2], 0x5); /* 0x2040128 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][3], 0x5); /* 0x204012c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][4], 0x7); /* 0x2040130 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][5], 0x1); /* 0x2040134 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][6], 0xa); /* 0x2040138 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][7], 0x6); /* 0x204013c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][4], 0x0); /* 0x2040390 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][4], 0x0); /* 0x2040210 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][5], 0x15); /* 0x2040394 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][5], 0x2); /* 0x2040214 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][2], 0xc); /* 0x2040308 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][0], 0xa); /* 0x2040040 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][1], 0x1); /* 0x2040044 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][2], 0x0); /* 0x2040048 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][3], 0x2); /* 0x204004c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][4], 0x3); /* 0x2040050 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][5], 0x3); /* 0x2040054 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][6], 0x0); /* 0x2040058 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][7], 0x7); /* 0x204005c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][4], 0x11); /* 0x20403d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][4], 0x0); /* 0x2040250 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][5], 0x9); /* 0x20403d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][5], 0x0); /* 0x2040254 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][2], 0xd); /* 0x2040328 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][0], 0x9); /* 0x2040140 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][1], 0x9); /* 0x2040144 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][2], 0x5); /* 0x2040148 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][3], 0x1); /* 0x204014c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][4], 0x9); /* 0x2040150 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][5], 0x3); /* 0x2040154 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][6], 0x4); /* 0x2040158 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][7], 0x7); /* 0x204015c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][6], 0x1b); /* 0x2040398 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][6], 0x2); /* 0x2040218 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][7], 0x1d); /* 0x204039c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][7], 0x1); /* 0x204021c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][3], 0xe); /* 0x204030c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][0], 0x3); /* 0x2040060 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][1], 0x8); /* 0x2040064 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][2], 0x0); /* 0x2040068 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][3], 0x2); /* 0x204006c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][4], 0x6); /* 0x2040070 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][5], 0x7); /* 0x2040074 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][6], 0x1); /* 0x2040078 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][7], 0x1); /* 0x204007c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][6], 0x12); /* 0x20403d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][6], 0x1); /* 0x2040258 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][7], 0x1e); /* 0x20403dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][7], 0x1); /* 0x204025c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][3], 0x7); /* 0x204032c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][0], 0x9); /* 0x2040160 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][1], 0x0); /* 0x2040164 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][2], 0x2); /* 0x2040168 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][3], 0x6); /* 0x204016c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][4], 0x4); /* 0x2040170 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][5], 0x1); /* 0x2040174 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][6], 0x7); /* 0x2040178 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][7], 0x2); /* 0x204017c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][8], 0xf); /* 0x20403a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][8], 0x2); /* 0x2040220 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][9], 0x1); /* 0x20403a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][9], 0x2); /* 0x2040224 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][4], 0xf); /* 0x2040310 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][0], 0x8); /* 0x2040080 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][1], 0x6); /* 0x2040084 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][2], 0x5); /* 0x2040088 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][3], 0x1); /* 0x204008c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][4], 0x7); /* 0x2040090 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][5], 0x5); /* 0x2040094 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][6], 0x8); /* 0x2040098 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][7], 0x5); /* 0x204009c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][8], 0x7); /* 0x20403e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][8], 0x2); /* 0x2040260 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][9], 0x18); /* 0x20403e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][9], 0x3); /* 0x2040264 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][4], 0x0); /* 0x2040330 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][0], 0x0); /* 0x2040180 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][1], 0x7); /* 0x2040184 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][2], 0x5); /* 0x2040188 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][3], 0x1); /* 0x204018c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][4], 0x9); /* 0x2040190 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][5], 0x7); /* 0x2040194 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][6], 0x6); /* 0x2040198 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][7], 0x8); /* 0x204019c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][10], 0x14); /* 0x20403a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][10], 0x0); /* 0x2040228 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][11], 0x3); /* 0x20403ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][11], 0x0); /* 0x204022c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][5], 0x8); /* 0x2040314 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][0], 0x0); /* 0x20400a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][1], 0x0); /* 0x20400a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][2], 0x3); /* 0x20400a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][3], 0x9); /* 0x20400ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][4], 0x4); /* 0x20400b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][5], 0x4); /* 0x20400b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][6], 0xa); /* 0x20400b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][7], 0x7); /* 0x20400bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][10], 0x1d); /* 0x20403e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][10], 0x0); /* 0x2040268 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][11], 0x1e); /* 0x20403ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][11], 0x2); /* 0x204026c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][5], 0xb); /* 0x2040334 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][0], 0x2); /* 0x20401a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][1], 0xa); /* 0x20401a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][2], 0x4); /* 0x20401a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][3], 0x3); /* 0x20401ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][4], 0xa); /* 0x20401b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][5], 0x9); /* 0x20401b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][6], 0x2); /* 0x20401b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][7], 0x2); /* 0x20401bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][12], 0x5); /* 0x20403b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][12], 0x3); /* 0x2040230 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][13], 0x8); /* 0x20403b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][13], 0x3); /* 0x2040234 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][6], 0xa); /* 0x2040318 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][0], 0x7); /* 0x20400c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][1], 0x1); /* 0x20400c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][2], 0x9); /* 0x20400c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][3], 0xa); /* 0x20400cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][4], 0x1); /* 0x20400d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][5], 0x6); /* 0x20400d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][6], 0x8); /* 0x20400d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][7], 0x6); /* 0x20400dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][12], 0x1c); /* 0x20403f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][12], 0x3); /* 0x2040270 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][13], 0x2); /* 0x20403f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][13], 0x3); /* 0x2040274 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][6], 0xb); /* 0x2040338 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][0], 0x3); /* 0x20401c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][1], 0x7); /* 0x20401c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][2], 0x1); /* 0x20401c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][3], 0x3); /* 0x20401cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][4], 0x2); /* 0x20401d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][5], 0x9); /* 0x20401d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][6], 0x0); /* 0x20401d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][7], 0x4); /* 0x20401dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][14], 0x1d); /* 0x20403b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][14], 0x2); /* 0x2040238 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][15], 0x1b); /* 0x20403bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][15], 0x3); /* 0x204023c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][7], 0xb); /* 0x204031c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][0], 0x6); /* 0x20400e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][1], 0x0); /* 0x20400e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][2], 0x6); /* 0x20400e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][3], 0x5); /* 0x20400ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][4], 0x5); /* 0x20400f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][5], 0x8); /* 0x20400f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][6], 0x8); /* 0x20400f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][7], 0x8); /* 0x20400fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][14], 0xe); /* 0x20403f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][14], 0x1); /* 0x2040278 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][15], 0x5); /* 0x20403fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][15], 0x3); /* 0x204027c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][7], 0x7); /* 0x204033c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][0], 0x9); /* 0x20401e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][1], 0x7); /* 0x20401e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][2], 0xa); /* 0x20401e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][3], 0x8); /* 0x20401ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][4], 0x7); /* 0x20401f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][5], 0x8); /* 0x20401f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][6], 0x2); /* 0x20401f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][7], 0x1); /* 0x20401fc */
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_table_prop[0], 0x4); /* 0x2014a00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tind_bus_prop[10], 0x4); /* 0x2014828 */
  //    tu.OutWord(&mau_reg_map.rams.match.merge.exact_match_delay_config, 0x50000); /* 0x2016490 */ // REMOVED EMDEL070915
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_hit_to_logical_table_ixbar_outputmap[0], 0x16); /* 0x2014860 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_match_adr_to_physical_oxbar_outputmap[10], 0x8); /* 0x20164e8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.match_to_logical_table_ixbar_outputmap[1][10], 0x16); /* 0x2014ae8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_tcam_shiftcount[10], 0x24); /* 0x2016628 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_mask[1][10], 0x3f); /* 0x2014d68 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_tcam_shiftcount[10], 0x0); /* 0x20165a8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_mask[1][10], 0xffffffff); /* 0x2014be8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.next_table_format_data[6], 0x100); /* 0x2016558 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_tcam_shiftcount[10], 0x2f); /* 0x20166a8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_mask[1][10], 0x3fffff); /* 0x2014ee8 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.immediate_data_32b_ixbar_ctl[6], 0x20); /* 0x2010398 */
    tu.OutWord(&mau_reg_map.rams.match.merge.predication_ctl[0], 0x40); /* 0x2014a20 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.ram_address_mux_ctl[0][0], 0x80); /* 0x201cb00 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].vh_xbars.adr_dist_tind_adr_xbar_ctl[0], 0x8); /* 0x201ed30 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.ram_address_mux_ctl[0][4], 0x100); /* 0x201eb20 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.adr_dist_action_data_adr_icxbar_ctl[6], 0x4); /* 0x20103d8 */
  ActionHvTranslator act_hv_translator;
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x1); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x0); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0x0); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x3); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0x3); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x0); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0x0); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x7); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0x7); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x10); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0x10); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0xf); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0xf); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x50); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0x50); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x1f); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0x1f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x250); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0x250); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x3f); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0x3f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0xa50); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0xa50); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x7f); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0x7f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x2a50); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0x2a50); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0xff); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0xff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0xaa50); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0xaa50); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x1ff); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0x1ff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x3aa50); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0x3aa50); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x3ff); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0x3ff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0xfaa50); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0xfaa50); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x7ff); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0x7ff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x3faa50); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0x3faa50); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0xfff); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0xfff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0xffaa50); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0xffaa50); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x1fff); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0x1fff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x3ffaa50); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0x3ffaa50); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x3fff); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0x3fff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0xfffaa50); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0xfffaa50); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x7fff); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0x7fff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x3fffaa50); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0x3fffaa50); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0xffff); /* 0x2009880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,0,0xffff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0xffffaa50); /* 0x2009888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,0,0xffffaa50); // ADDED ACHV070915
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[0].ctl.r_action_o_mux_select, 0x0); /* 0x20000a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[0].ctl.r_l_action_o_mux_select, 0x8); /* 0x2000080 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[1].ctl.r_action_o_mux_select, 0x0); /* 0x20001a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[1].ctl.r_l_action_o_mux_select, 0x8); /* 0x2000180 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[2].ctl.r_action_o_mux_select, 0x0); /* 0x20002a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[2].ctl.r_l_action_o_mux_select, 0x8); /* 0x2000280 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[3].ctl.r_action_o_mux_select, 0x0); /* 0x20003a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[3].ctl.r_l_action_o_mux_select, 0x8); /* 0x2000380 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[4].ctl.r_action_o_mux_select, 0x0); /* 0x20004a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[4].ctl.r_l_action_o_mux_select, 0x8); /* 0x2000480 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[5].ctl.r_action_o_mux_select, 0x0); /* 0x20005a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[5].ctl.r_l_action_o_mux_select, 0x8); /* 0x2000580 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[6].ctl.r_action_o_mux_select, 0x0); /* 0x20006a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[6].ctl.r_l_action_o_mux_select, 0x8); /* 0x2000680 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[7].ctl.r_action_o_mux_select, 0x0); /* 0x20007a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[7].ctl.r_l_action_o_mux_select, 0x8); /* 0x2000780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][9], RM_B4_32(0x3075e1f6)); /* 0x207c024 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][6], RM_B4_32(0x33e3e227)); /* 0x207c098 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][10], RM_B4_32(0x2881e2b2)); /* 0x207c128 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][9], RM_B4_32(0x1107e3b5)); /* 0x207c1a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][5], RM_B4_32(0x15f9e3ab)); /* 0x207c214 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][11], RM_B4_32(0xf4be1ac)); /* 0x207c2ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][3], RM_B4_32(0x3d9fe32a)); /* 0x207c30c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][2], RM_B4_32(0x144be009)); /* 0x207c388 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][9], RM_B4_32(0xfb9e143)); /* 0x207c424 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][3], RM_B4_32(0xf71e338)); /* 0x207c48c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][0], RM_B4_32(0x1f77e23a)); /* 0x207c500 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][10], RM_B4_32(0x3b5fe0a7)); /* 0x207c5a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][9], RM_B4_32(0x3a45e2eb)); /* 0x207c624 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][3], RM_B4_32(0x11b1e2c8)); /* 0x207c68c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][1], RM_B4_32(0x281de30d)); /* 0x207c704 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][2], RM_B4_32(0x3191e12a)); /* 0x207c788 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][2], RM_B4_32(0x2b2de3ba)); /* 0x207c808 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][11], RM_B4_32(0x4cbe239)); /* 0x207c8ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][6], RM_B4_32(0x1e5fe2cd)); /* 0x207c918 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][6], RM_B4_32(0x3067e274)); /* 0x207c998 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][0], RM_B4_32(0x2ab9e051)); /* 0x207ca00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][4], RM_B4_32(0x7ede225)); /* 0x207ca90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][3], RM_B4_32(0x10b3e07b)); /* 0x207cb0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][9], RM_B4_32(0x15b7e228)); /* 0x207cba4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][4], RM_B4_32(0xb97e1f0)); /* 0x207cc10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][8], RM_B4_32(0x821e252)); /* 0x207cca0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][7], RM_B4_32(0x20f1e326)); /* 0x207cd1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][9], RM_B4_32(0x3001e1d8)); /* 0x207cda4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][0], RM_B4_32(0x3361e3e9)); /* 0x207ce00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][5], RM_B4_32(0x2931e2b8)); /* 0x207ce94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][2], RM_B4_32(0x29a1e2e1)); /* 0x207cf08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][2], RM_B4_32(0x2da5e370)); /* 0x207cf88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][3], RM_B4_32(0x677e1db)); /* 0x207d00c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][10], RM_B4_32(0x2c49e10a)); /* 0x207d0a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][0], RM_B4_32(0x2b55e3d9)); /* 0x207d100 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][4], RM_B4_32(0x2d35e184)); /* 0x207d190 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][4], RM_B4_32(0x1b03e1b9)); /* 0x207d210 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][0], RM_B4_32(0x2203e15f)); /* 0x207d280 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][8], RM_B4_32(0x34c3e1b2)); /* 0x207d320 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][11], RM_B4_32(0x3eb3e15e)); /* 0x207d3ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][7], RM_B4_32(0x3343e04a)); /* 0x207d41c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][2], RM_B4_32(0x713e3b5)); /* 0x207d488 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][6], RM_B4_32(0x2671e055)); /* 0x207d518 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][9], RM_B4_32(0x1f73e149)); /* 0x207d5a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][3], RM_B4_32(0x2ddde32c)); /* 0x207d60c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][5], RM_B4_32(0x5d3e003)); /* 0x207d694 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][0], RM_B4_32(0x437e18d)); /* 0x207d700 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][7], RM_B4_32(0x32d3e28a)); /* 0x207d79c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][7], RM_B4_32(0x149e2bd)); /* 0x207d81c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][7], RM_B4_32(0x1f83e3bd)); /* 0x207d89c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][0], RM_B4_32(0x4f9e009)); /* 0x207d900 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][1], RM_B4_32(0x3c1fe1e4)); /* 0x207d984 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][3], RM_B4_32(0x1c13e1e0)); /* 0x207da0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][9], RM_B4_32(0x2381e26b)); /* 0x207daa4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][10], RM_B4_32(0x1381e385)); /* 0x207db28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][8], RM_B4_32(0xd5be0f2)); /* 0x207dba0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][4], RM_B4_32(0x2977e264)); /* 0x207dc10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][10], RM_B4_32(0x1127e186)); /* 0x207dca8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][6], RM_B4_32(0x10a7e251)); /* 0x207dd18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][4], RM_B4_32(0x2907e3d6)); /* 0x207dd90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][11], RM_B4_32(0x3ce5e218)); /* 0x207de2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][8], RM_B4_32(0x2765e379)); /* 0x207dea0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][1], RM_B4_32(0x617e364)); /* 0x207df04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][2], RM_B4_32(0x3b07e25d)); /* 0x207df88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[0][1], RM_B4_16(0x21fe1ab)); /* 0x2078004 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[1][8], RM_B4_16(0x379e061)); /* 0x20780a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[2][0], RM_B4_16(0x697e319)); /* 0x2078100 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[3][6], RM_B4_16(0x45e104)); /* 0x2078198 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[4][4], RM_B4_16(0x99e2ff)); /* 0x2078210 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[5][0], RM_B4_16(0x337e3ce)); /* 0x2078280 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[6][10], RM_B4_16(0x19fe24f)); /* 0x2078328 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[7][9], RM_B4_16(0x789e108)); /* 0x20783a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[8][11], RM_B4_16(0x1e7e11c)); /* 0x207842c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[9][8], RM_B4_16(0x513e3b4)); /* 0x20784a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[10][0], RM_B4_16(0x1d1e2f7)); /* 0x2078500 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[11][8], RM_B4_16(0x239e2a0)); /* 0x20785a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[12][7], RM_B4_16(0x195e20e)); /* 0x207861c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[13][8], RM_B4_16(0x30de32c)); /* 0x20786a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[14][9], RM_B4_16(0x6dbe231)); /* 0x2078724 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[15][6], RM_B4_16(0x5dfe077)); /* 0x2078798 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[16][1], RM_B4_16(0x359e3d1)); /* 0x2078804 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[17][9], RM_B4_16(0x12de1b3)); /* 0x20788a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[18][9], RM_B4_16(0x9be27f)); /* 0x2078924 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[19][0], RM_B4_16(0x1f1e398)); /* 0x2078980 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[20][11], RM_B4_16(0xfde262)); /* 0x2078a2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[21][5], RM_B4_16(0x5f7e06a)); /* 0x2078a94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[22][2], RM_B4_16(0x179e310)); /* 0x2078b08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[23][10], RM_B4_16(0x3e3e1e7)); /* 0x2078ba8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[24][4], RM_B4_16(0x74be013)); /* 0x2078c10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[25][1], RM_B4_16(0x48be2b9)); /* 0x2078c84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[26][0], RM_B4_16(0x667e2b7)); /* 0x2078d00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[27][4], RM_B4_16(0x60de0c1)); /* 0x2078d90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[28][1], RM_B4_16(0x6c1e004)); /* 0x2078e04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[29][10], RM_B4_16(0x619e015)); /* 0x2078ea8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[30][6], RM_B4_16(0x657e16c)); /* 0x2078f18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[31][11], RM_B4_16(0x45fe1b3)); /* 0x2078fac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[32][5], RM_B4_16(0x4e3e199)); /* 0x2079014 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[33][3], RM_B4_16(0x349e2aa)); /* 0x207908c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[34][6], RM_B4_16(0xb3e306)); /* 0x2079118 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[35][4], RM_B4_16(0x5dfe08e)); /* 0x2079190 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[36][4], RM_B4_16(0x2dbe323)); /* 0x2079210 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[37][9], RM_B4_16(0x177e3a6)); /* 0x20792a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[38][8], RM_B4_16(0x125e370)); /* 0x2079320 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[39][11], RM_B4_16(0x683e25a)); /* 0x20793ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[40][2], RM_B4_16(0x411e232)); /* 0x2079408 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[41][6], RM_B4_16(0x433e3c9)); /* 0x2079498 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[42][0], RM_B4_16(0x4b7e17f)); /* 0x2079500 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[43][2], RM_B4_16(0x641e0ff)); /* 0x2079588 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[44][10], RM_B4_16(0x3dfe236)); /* 0x2079628 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[45][6], RM_B4_16(0x353e267)); /* 0x2079698 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[46][9], RM_B4_16(0x121e06c)); /* 0x2079724 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[47][0], RM_B4_16(0x517e2ff)); /* 0x2079780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[48][5], RM_B4_16(0x7dfe275)); /* 0x2079814 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[49][8], RM_B4_16(0x577e19f)); /* 0x20798a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[50][11], RM_B4_16(0x6dfe2eb)); /* 0x207992c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[51][10], RM_B4_16(0x283e39f)); /* 0x20799a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[52][9], RM_B4_16(0x7a7e1ad)); /* 0x2079a24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[53][5], RM_B4_16(0x11be283)); /* 0x2079a94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[54][7], RM_B4_16(0x2c7e0bb)); /* 0x2079b1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[55][5], RM_B4_16(0x13de09d)); /* 0x2079b94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[56][6], RM_B4_16(0x7ede26f)); /* 0x2079c18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[57][10], RM_B4_16(0x18de2f7)); /* 0x2079ca8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[58][3], RM_B4_16(0x1e322)); /* 0x2079d0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[59][9], RM_B4_16(0xbde2c9)); /* 0x2079da4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[60][11], RM_B4_16(0x3f5e164)); /* 0x2079e2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[61][1], RM_B4_16(0xbe237)); /* 0x2079e84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[62][4], RM_B4_16(0x5a3e362)); /* 0x2079f10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[63][0], RM_B4_16(0xd3e107)); /* 0x2079f80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[64][9], RM_B4_16(0x24be075)); /* 0x207a024 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[65][4], RM_B4_16(0x1b3e29c)); /* 0x207a090 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[66][10], RM_B4_16(0x4f9e1e6)); /* 0x207a128 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[67][9], RM_B4_16(0x4d9e3d5)); /* 0x207a1a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[68][7], RM_B4_16(0x239e00d)); /* 0x207a21c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[69][8], RM_B4_16(0x747e30e)); /* 0x207a2a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[70][10], RM_B4_16(0x1bde1d3)); /* 0x207a328 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[71][4], RM_B4_16(0x287e3e0)); /* 0x207a390 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[72][7], RM_B4_16(0x283e3e4)); /* 0x207a41c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[73][4], RM_B4_16(0xade08e)); /* 0x207a490 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[74][4], RM_B4_16(0x3c7e2d4)); /* 0x207a510 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[75][0], RM_B4_16(0x4f1e316)); /* 0x207a580 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[76][6], RM_B4_16(0x97e095)); /* 0x207a618 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[77][4], RM_B4_16(0x5fe064)); /* 0x207a690 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[78][2], RM_B4_16(0x1dfe111)); /* 0x207a708 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[79][7], RM_B4_16(0x5dbe1b8)); /* 0x207a79c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[80][0], RM_B4_16(0x3a3e3ca)); /* 0x207a800 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[81][9], RM_B4_16(0x1a1e2e7)); /* 0x207a8a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[82][2], RM_B4_16(0x1a9e347)); /* 0x207a908 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[83][4], RM_B4_16(0xd3e2d9)); /* 0x207a990 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[84][6], RM_B4_16(0xe3e22c)); /* 0x207aa18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[85][3], RM_B4_16(0xdbe2c0)); /* 0x207aa8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[86][0], RM_B4_16(0x531e03a)); /* 0x207ab00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[87][0], RM_B4_16(0x5c3e034)); /* 0x207ab80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[88][8], RM_B4_16(0x61be336)); /* 0x207ac20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[89][8], RM_B4_16(0x21be080)); /* 0x207aca0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[90][4], RM_B4_16(0x11e304)); /* 0x207ad10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[91][9], RM_B4_16(0x635e317)); /* 0x207ada4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[92][0], RM_B4_16(0x61de012)); /* 0x207ae00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[93][5], RM_B4_16(0x465e2bf)); /* 0x207ae94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[94][8], RM_B4_16(0x229e0c9)); /* 0x207af20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[95][11], RM_B4_16(0x3e9e043)); /* 0x207afac */

    // Here we need to do a lookup on a specific TCAM - tcam 0,0 - ensure zeroised
    RmtObjectManager *om = tu.get_objmgr();
    assert (om != NULL);
    // Assume pipe 0, mau 0
    Mau *mau = om->mau_lookup(0,0); 
    assert (mau != NULL);
    // Get tcam 0,0
    MauTcam *tcam = mau->tcam_lookup(0,0);
    assert (tcam != NULL);
    for (int index = 0; index < 512; index++) tcam->write(index, UINT64_C(0), UINT64_C(0), UINT64_C(0));
    
  act_hv_translator.do_writes(&tu);
    tu.IndirectWrite(0x0200801001c1, 0x00001fffdfffffff, 0x00001c0020000000); /* TCAM[ 0][ 0][449].word1 = 0x0010000000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e0, 0x0000000000000000, 0x32e0bd1000000000); /* sram_ 5_ 4: a=0x200800150e0 d0=0x0 d1=0x32e0bd1000000000 */
    tu.IndirectWrite(0x0200800041c1, 0x96963e085d732c2c, 0xfca5ec21b05cebcc); /* sram_ 1_ 0: a=0x200800041c1 d0=0x96963e085d732c2c d1=0xfca5ec21b05cebcc */
    tu.IndirectWrite(0x020080100117, 0x00001fffffffffff, 0x00001c0000000000); /* TCAM[ 0][ 0][279].word1 = 0x0000000000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008001508b, 0x0000000000000000, 0x328bbd5000000000); /* sram_ 5_ 4: a=0x2008001508b d0=0x0 d1=0x328bbd5000000000 */
    tu.IndirectWrite(0x020080004117, 0xb7c94072065255e9, 0x57060cfcc24a07c9); /* sram_ 1_ 0: a=0x20080004117 d0=0xb7c94072065255e9 d1=0x57060cfcc24a07c9 */
    tu.IndirectWrite(0x0200801000e5, 0x00001ffdc2b4b431, 0x00001c023d4b4bce); /* TCAM[ 0][ 0][229].word1 = 0x011ea5a5e7  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015072, 0x0000000000000000, 0x3272bd5000000000); /* sram_ 5_ 4: a=0x20080015072 d0=0x0 d1=0x3272bd5000000000 */
    tu.IndirectWrite(0x0200800040e5, 0xb38cdf7b1d16c685, 0xbab561fcccb914cd); /* sram_ 1_ 0: a=0x200800040e5 d0=0xb38cdf7b1d16c685 d1=0xbab561fcccb914cd */
    tu.IndirectWrite(0x0200801001e7, 0x00001fffffffffff, 0x00001c0000000000); /* TCAM[ 0][ 0][487].word1 = 0x0000000000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150f3, 0x0000000000000000, 0x32f3bd4000000000); /* sram_ 5_ 4: a=0x200800150f3 d0=0x0 d1=0x32f3bd4000000000 */
    tu.IndirectWrite(0x0200800041e7, 0xf4b8b8480a941974, 0xdb13e850d99a9ce8); /* sram_ 1_ 0: a=0x200800041e7 d0=0xf4b8b8480a941974 d1=0xdb13e850d99a9ce8 */
    tu.IndirectWrite(0x020080100106, 0x00001f00f748938f, 0x00001cff08b76c70); /* TCAM[ 0][ 0][262].word1 = 0x7f845bb638  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015083, 0x32833c9000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x20080015083 d0=0x32833c9000000000 d1=0x0 */
    tu.IndirectWrite(0x020080004106, 0xbac2edc973d9efeb, 0x5c433c842a081073); /* sram_ 1_ 0: a=0x20080004106 d0=0xbac2edc973d9efeb d1=0x5c433c842a081073 */
    tu.IndirectWrite(0x0200801001c9, 0x00001f4def5569c5, 0x00001cb210aa963a); /* TCAM[ 0][ 0][457].word1 = 0x5908554b1d  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e4, 0x0000000000000000, 0x32e4bc7000000000); /* sram_ 5_ 4: a=0x200800150e4 d0=0x0 d1=0x32e4bc7000000000 */
    tu.IndirectWrite(0x0200800041c9, 0x5c81f5dafb422bc9, 0xbe6368c0559d5914); /* sram_ 1_ 0: a=0x200800041c9 d0=0x5c81f5dafb422bc9 d1=0xbe6368c0559d5914 */
    tu.IndirectWrite(0x020080100072, 0x00001fd7af5ebf7d, 0x00001c2850a14082); /* TCAM[ 0][ 0][114].word1 = 0x142850a041  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015039, 0x32393ca000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x20080015039 d0=0x32393ca000000000 d1=0x0 */
    tu.IndirectWrite(0x020080004072, 0x23ec688b9d0db477, 0x3e60663b0006ad6e); /* sram_ 1_ 0: a=0x20080004072 d0=0x23ec688b9d0db477 d1=0x3e60663b0006ad6e */
    tu.IndirectWrite(0x0200801001f5, 0x00001e00200c0001, 0x00001dffdff3fffe); /* TCAM[ 0][ 0][501].word1 = 0xffeff9ffff  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150fa, 0x0000000000000000, 0x32fabd1000000000); /* sram_ 5_ 4: a=0x200800150fa d0=0x0 d1=0x32fabd1000000000 */
    tu.IndirectWrite(0x0200800041f5, 0xf53495e050247cf8, 0x47177803208c5e34); /* sram_ 1_ 0: a=0x200800041f5 d0=0xf53495e050247cf8 d1=0x47177803208c5e34 */
    tu.IndirectWrite(0x0200801001fa, 0x00001fffff97ffff, 0x00001c0000680000); /* TCAM[ 0][ 0][506].word1 = 0x0000340000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150fd, 0x32fd3d6000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x200800150fd d0=0x32fd3d6000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800041fa, 0x87acadb73bd03ac5, 0xec338583e73c2e4b); /* sram_ 1_ 0: a=0x200800041fa d0=0x87acadb73bd03ac5 d1=0xec338583e73c2e4b */
    tu.IndirectWrite(0x02008010019e, 0x00001fc161c1a37f, 0x00001c3e9e3e5c80); /* TCAM[ 0][ 0][414].word1 = 0x1f4f1f2e40  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150cf, 0x32cf3d5000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x200800150cf d0=0x32cf3d5000000000 d1=0x0 */
    tu.IndirectWrite(0x02008000419e, 0x361bd8f010c31951, 0x2f9ab958f54917a3); /* sram_ 1_ 0: a=0x2008000419e d0=0x361bd8f010c31951 d1=0x2f9ab958f54917a3 */
    tu.IndirectWrite(0x020080100085, 0x00001eda4c897e15, 0x00001d25b37681ea); /* TCAM[ 0][ 0][133].word1 = 0x92d9bb40f5  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015042, 0x0000000000000000, 0x3242bd2000000000); /* sram_ 5_ 4: a=0x20080015042 d0=0x0 d1=0x3242bd2000000000 */
    tu.IndirectWrite(0x020080004085, 0x95c7c1b9171f33e6, 0x4cc5a4594f234196); /* sram_ 1_ 0: a=0x20080004085 d0=0x95c7c1b9171f33e6 d1=0x4cc5a4594f234196 */
    tu.IndirectWrite(0x0200801001cd, 0x00001fca8c3c2137, 0x00001c3573c3dec8); /* TCAM[ 0][ 0][461].word1 = 0x1ab9e1ef64  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e6, 0x0000000000000000, 0x32e6bd1000000000); /* sram_ 5_ 4: a=0x200800150e6 d0=0x0 d1=0x32e6bd1000000000 */
    tu.IndirectWrite(0x0200800041cd, 0xb743801724b873c0, 0x53810249adbeef43); /* sram_ 1_ 0: a=0x200800041cd d0=0xb743801724b873c0 d1=0x53810249adbeef43 */
    tu.IndirectWrite(0x0200801001af, 0x00001ee4c942974f, 0x00001d1b36bd68b0); /* TCAM[ 0][ 0][431].word1 = 0x8d9b5eb458  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150d7, 0x0000000000000000, 0x32d7bd6000000000); /* sram_ 5_ 4: a=0x200800150d7 d0=0x0 d1=0x32d7bd6000000000 */
    tu.IndirectWrite(0x0200800041af, 0x1d23ed6048021caf, 0x9cc3a16683ce8410); /* sram_ 1_ 0: a=0x200800041af d0=0x1d23ed6048021caf d1=0x9cc3a16683ce8410 */
    tu.IndirectWrite(0x0200801001d6, 0x00001ff8fc7f3f9f, 0x00001c070380c060); /* TCAM[ 0][ 0][470].word1 = 0x0381c06030  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150eb, 0x32eb3cd000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x200800150eb d0=0x32eb3cd000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800041d6, 0x89e4bd1d18fdac69, 0x878528cc8f0d727b); /* sram_ 1_ 0: a=0x200800041d6 d0=0x89e4bd1d18fdac69 d1=0x878528cc8f0d727b */
    tu.IndirectWrite(0x0200801001a7, 0x00001fffffffffff, 0x00001c0000000000); /* TCAM[ 0][ 0][423].word1 = 0x0000000000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150d3, 0x0000000000000000, 0x32d3bc0000000000); /* sram_ 5_ 4: a=0x200800150d3 d0=0x0 d1=0x32d3bc0000000000 */
    tu.IndirectWrite(0x0200800041a7, 0x2fd662619c5cd0e9, 0x921dab278faa1289); /* sram_ 1_ 0: a=0x200800041a7 d0=0x2fd662619c5cd0e9 d1=0x921dab278faa1289 */
    tu.IndirectWrite(0x0200801001b2, 0x00001e13660d295f, 0x00001dec99f2d6a0); /* TCAM[ 0][ 0][434].word1 = 0xf64cf96b50  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150d9, 0x32d93c2000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x200800150d9 d0=0x32d93c2000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800041b2, 0x288481d573edbe95, 0x41dc3478bebfc609); /* sram_ 1_ 0: a=0x200800041b2 d0=0x288481d573edbe95 d1=0x41dc3478bebfc609 */
    tu.IndirectWrite(0x0200801001b1, 0x00001e088875f301, 0x00001df7778a0cfe); /* TCAM[ 0][ 0][433].word1 = 0xfbbbc5067f  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150d8, 0x0000000000000000, 0x32d8bc0000000000); /* sram_ 5_ 4: a=0x200800150d8 d0=0x0 d1=0x32d8bc0000000000 */
    tu.IndirectWrite(0x0200800041b1, 0x33a95f153647c8a4, 0x2286fa55a82c24a1); /* sram_ 1_ 0: a=0x200800041b1 d0=0x33a95f153647c8a4 d1=0x2286fa55a82c24a1 */
    tu.IndirectWrite(0x0200801001c7, 0x00001ff7fbfcff7f, 0x00001c0804030080); /* TCAM[ 0][ 0][455].word1 = 0x0402018040  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e3, 0x0000000000000000, 0x32e3bd7000000000); /* sram_ 5_ 4: a=0x200800150e3 d0=0x0 d1=0x32e3bd7000000000 */
    tu.IndirectWrite(0x0200800041c7, 0xc6479b70ac576830, 0x297b9b2c31cd49f5); /* sram_ 1_ 0: a=0x200800041c7 d0=0xc6479b70ac576830 d1=0x297b9b2c31cd49f5 */
    tu.IndirectWrite(0x0200801001c8, 0x00001e7072747679, 0x00001d8f8d8b8986); /* TCAM[ 0][ 0][456].word1 = 0xc7c6c5c4c3  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e4, 0x32e43c8000000000, 0x32e4bc7000000000); /* sram_ 5_ 4: a=0x200800150e4 d0=0x32e43c8000000000 d1=0x32e4bc7000000000 */
    tu.IndirectWrite(0x0200800041c8, 0x8ff15a0f596d791f, 0x6260e16d7f75e041); /* sram_ 1_ 0: a=0x200800041c8 d0=0x8ff15a0f596d791f d1=0x6260e16d7f75e041 */
    tu.IndirectWrite(0x0200801001ca, 0x00001fffdfdbffff, 0x00001c0020240000); /* TCAM[ 0][ 0][458].word1 = 0x0010120000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e5, 0x32e53c4000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x200800150e5 d0=0x32e53c4000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800041ca, 0xd2fccf5b0513a75a, 0x73b4fa4213f283ab); /* sram_ 1_ 0: a=0x200800041ca d0=0xd2fccf5b0513a75a d1=0x73b4fa4213f283ab */
    tu.IndirectWrite(0x0200801000e7, 0x00001e0000320001, 0x00001dffffcdfffe); /* TCAM[ 0][ 0][231].word1 = 0xffffe6ffff  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015073, 0x0000000000000000, 0x3273bc7000000000); /* sram_ 5_ 4: a=0x20080015073 d0=0x0 d1=0x3273bc7000000000 */
    tu.IndirectWrite(0x0200800040e7, 0xc19d2d2bf1478991, 0xe00c5a6ede4183d6); /* sram_ 1_ 0: a=0x200800040e7 d0=0xc19d2d2bf1478991 d1=0xe00c5a6ede4183d6 */
    tu.IndirectWrite(0x020080100012, 0x00001ffffffffdf5, 0x00001c000000020a); /* TCAM[ 0][ 0][ 18].word1 = 0x0000000105  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015009, 0x32093d2000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x20080015009 d0=0x32093d2000000000 d1=0x0 */
    tu.IndirectWrite(0x020080004012, 0xb83d95ceffd00a6d, 0x2d814f2c93b44fb2); /* sram_ 1_ 0: a=0x20080004012 d0=0xb83d95ceffd00a6d d1=0x2d814f2c93b44fb2 */
    tu.IndirectWrite(0x0200801001f8, 0x00001fffffffffff, 0x00001c0000000000); /* TCAM[ 0][ 0][504].word1 = 0x0000000000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150fc, 0x32fc3c4000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x200800150fc d0=0x32fc3c4000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800041f8, 0x5a4d2e170cebfe82, 0xd58e139a6e9d8b30); /* sram_ 1_ 0: a=0x200800041f8 d0=0x5a4d2e170cebfe82 d1=0xd58e139a6e9d8b30 */
    tu.IndirectWrite(0x0200801001a5, 0x00001fffffffffff, 0x00001c0000000000); /* TCAM[ 0][ 0][421].word1 = 0x0000000000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150d2, 0x0000000000000000, 0x32d2bc7000000000); /* sram_ 5_ 4: a=0x200800150d2 d0=0x0 d1=0x32d2bc7000000000 */
    tu.IndirectWrite(0x0200800041a5, 0x7bb5ec84020a78e2, 0x766669db5d47791f); /* sram_ 1_ 0: a=0x200800041a5 d0=0x7bb5ec84020a78e2 d1=0x766669db5d47791f */
    tu.IndirectWrite(0x020080100079, 0x00001fffff77ffff, 0x00001c0000880000); /* TCAM[ 0][ 0][121].word1 = 0x0000440000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008001503c, 0x0000000000000000, 0x323cbc8000000000); /* sram_ 5_ 4: a=0x2008001503c d0=0x0 d1=0x323cbc8000000000 */
    tu.IndirectWrite(0x020080004079, 0x317a7335dc705d45, 0x869e44cca3b76541); /* sram_ 1_ 0: a=0x20080004079 d0=0x317a7335dc705d45 d1=0x869e44cca3b76541 */
    tu.IndirectWrite(0x020080100101, 0x00001e0000280001, 0x00001dffffd7fffe); /* TCAM[ 0][ 0][257].word1 = 0xffffebffff  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015080, 0x0000000000000000, 0x3280bc2000000000); /* sram_ 5_ 4: a=0x20080015080 d0=0x0 d1=0x3280bc2000000000 */
    tu.IndirectWrite(0x020080004101, 0xb7fe81debad6619f, 0xe177be7ea25cb776); /* sram_ 1_ 0: a=0x20080004101 d0=0xb7fe81debad6619f d1=0xe177be7ea25cb776 */
    tu.IndirectWrite(0x0200801001a4, 0x00001fffdfcdffff, 0x00001c0020320000); /* TCAM[ 0][ 0][420].word1 = 0x0010190000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150d2, 0x32d23d1000000000, 0x32d2bc7000000000); /* sram_ 5_ 4: a=0x200800150d2 d0=0x32d23d1000000000 d1=0x32d2bc7000000000 */
    tu.IndirectWrite(0x0200800041a4, 0xb0aaedc4627feaf8, 0x686e3794d5b51b08); /* sram_ 1_ 0: a=0x200800041a4 d0=0xb0aaedc4627feaf8 d1=0x686e3794d5b51b08 */
    tu.IndirectWrite(0x0200801001ee, 0x00001e0499634d0b, 0x00001dfb669cb2f4); /* TCAM[ 0][ 0][494].word1 = 0xfdb34e597a  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150f7, 0x32f73c4000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x200800150f7 d0=0x32f73c4000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800041ee, 0xddad0a93fca3e12e, 0xeecec53ea4ac72e8); /* sram_ 1_ 0: a=0x200800041ee d0=0xddad0a93fca3e12e d1=0xeecec53ea4ac72e8 */
    tu.IndirectWrite(0x0200801001a6, 0x00001ff93ffffffd, 0x00001c06c0000002); /* TCAM[ 0][ 0][422].word1 = 0x0360000001  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150d3, 0x32d33c8000000000, 0x32d3bc0000000000); /* sram_ 5_ 4: a=0x200800150d3 d0=0x32d33c8000000000 d1=0x32d3bc0000000000 */
    tu.IndirectWrite(0x0200800041a6, 0x955536445e66fd2e, 0xfd9e3767f618d799); /* sram_ 1_ 0: a=0x200800041a6 d0=0x955536445e66fd2e d1=0xfd9e3767f618d799 */
    tu.IndirectWrite(0x0200801001ef, 0x00001fc0f3d99323, 0x00001c3f0c266cdc); /* TCAM[ 0][ 0][495].word1 = 0x1f8613366e  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150f7, 0x32f73c4000000000, 0x32f7bc5000000000); /* sram_ 5_ 4: a=0x200800150f7 d0=0x32f73c4000000000 d1=0x32f7bc5000000000 */
    tu.IndirectWrite(0x0200800041ef, 0xaae3df4390cdac71, 0x410f9f9d0093c038); /* sram_ 1_ 0: a=0x200800041ef d0=0xaae3df4390cdac71 d1=0x410f9f9d0093c038 */
    tu.IndirectWrite(0x0200801001cb, 0x00001f27c7c11e27, 0x00001cd8383ee1d8); /* TCAM[ 0][ 0][459].word1 = 0x6c1c1f70ec  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e5, 0x32e53c4000000000, 0x32e5bc1000000000); /* sram_ 5_ 4: a=0x200800150e5 d0=0x32e53c4000000000 d1=0x32e5bc1000000000 */
    tu.IndirectWrite(0x0200800041cb, 0x4c646c9dc21c1e94, 0x3a147cc14eea6676); /* sram_ 1_ 0: a=0x200800041cb d0=0x4c646c9dc21c1e94 d1=0x3a147cc14eea6676 */
    tu.IndirectWrite(0x0200801001b0, 0x00001e56585a5c5f, 0x00001da9a7a5a3a0); /* TCAM[ 0][ 0][432].word1 = 0xd4d3d2d1d0  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150d8, 0x32d83c0000000000, 0x32d8bc0000000000); /* sram_ 5_ 4: a=0x200800150d8 d0=0x32d83c0000000000 d1=0x32d8bc0000000000 */
    tu.IndirectWrite(0x0200800041b0, 0xde03cafff9457ae0, 0x9ae2a29e08fc225a); /* sram_ 1_ 0: a=0x200800041b0 d0=0xde03cafff9457ae0 d1=0x9ae2a29e08fc225a */
    tu.IndirectWrite(0x0200801001c3, 0x00001ff83ffffffd, 0x00001c07c0000002); /* TCAM[ 0][ 0][451].word1 = 0x03e0000001  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e1, 0x0000000000000000, 0x32e1bd0000000000); /* sram_ 5_ 4: a=0x200800150e1 d0=0x0 d1=0x32e1bd0000000000 */
    tu.IndirectWrite(0x0200800041c3, 0xb0cd04ea1ea78fd2, 0x5b39113d7e4b93a0); /* sram_ 1_ 0: a=0x200800041c3 d0=0xb0cd04ea1ea78fd2 d1=0x5b39113d7e4b93a0 */
    tu.IndirectWrite(0x0200801000d1, 0x00001fb3b35caf63, 0x00001c4c4ca3509c); /* TCAM[ 0][ 0][209].word1 = 0x262651a84e  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015068, 0x0000000000000000, 0x3268bc0000000000); /* sram_ 5_ 4: a=0x20080015068 d0=0x0 d1=0x3268bc0000000000 */
    tu.IndirectWrite(0x0200800040d1, 0x57df946ae7542fa4, 0x739b1d2496fdb755); /* sram_ 1_ 0: a=0x200800040d1 d0=0x57df946ae7542fa4 d1=0x739b1d2496fdb755 */
    tu.IndirectWrite(0x020080100081, 0x00001e64f3781b9b, 0x00001d9b0c87e464); /* TCAM[ 0][ 0][129].word1 = 0xcd8643f232  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015040, 0x0000000000000000, 0x3240bd7000000000); /* sram_ 5_ 4: a=0x20080015040 d0=0x0 d1=0x3240bd7000000000 */
    tu.IndirectWrite(0x020080004081, 0x877c18467680eede, 0xa246902eff497760); /* sram_ 1_ 0: a=0x20080004081 d0=0x877c18467680eede d1=0xa246902eff497760 */
    tu.IndirectWrite(0x02008010013e, 0x00001ef3b9980625, 0x00001d0c4667f9da); /* TCAM[ 0][ 0][318].word1 = 0x862333fced  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008001509f, 0x329f3d6000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x2008001509f d0=0x329f3d6000000000 d1=0x0 */
    tu.IndirectWrite(0x02008000413e, 0x454c13bab63dba2d, 0xaa364891507f05f8); /* sram_ 1_ 0: a=0x2008000413e d0=0x454c13bab63dba2d d1=0xaa364891507f05f8 */
    tu.IndirectWrite(0x0200801000e6, 0x00001fffffff9ee7, 0x00001c0000006118); /* TCAM[ 0][ 0][230].word1 = 0x000000308c  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015073, 0x32733d7000000000, 0x3273bc7000000000); /* sram_ 5_ 4: a=0x20080015073 d0=0x32733d7000000000 d1=0x3273bc7000000000 */
    tu.IndirectWrite(0x0200800040e6, 0x35cc3358b07bdb70, 0x8836eabe1284aa9b); /* sram_ 1_ 0: a=0x200800040e6 d0=0x35cc3358b07bdb70 d1=0x8836eabe1284aa9b */
    tu.IndirectWrite(0x0200801001d3, 0x00001fffffffffff, 0x00001c0000000000); /* TCAM[ 0][ 0][467].word1 = 0x0000000000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e9, 0x0000000000000000, 0x32e9bc1000000000); /* sram_ 5_ 4: a=0x200800150e9 d0=0x0 d1=0x32e9bc1000000000 */
    tu.IndirectWrite(0x0200800041d3, 0xed33daf71e40728d, 0x70b9ec4496adbddc); /* sram_ 1_ 0: a=0x200800041d3 d0=0xed33daf71e40728d d1=0x70b9ec4496adbddc */
    tu.IndirectWrite(0x0200801001a0, 0x00001fffffffffff, 0x00001c0000000000); /* TCAM[ 0][ 0][416].word1 = 0x0000000000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150d0, 0x32d03d3000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x200800150d0 d0=0x32d03d3000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800041a0, 0x7a94c9024dbf6bb2, 0x6e645843ea229869); /* sram_ 1_ 0: a=0x200800041a0 d0=0x7a94c9024dbf6bb2 d1=0x6e645843ea229869 */
    tu.IndirectWrite(0x0200801000e0, 0x00001f706c92032b, 0x00001c8f936dfcd4); /* TCAM[ 0][ 0][224].word1 = 0x47c9b6fe6a  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015070, 0x32703c6000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x20080015070 d0=0x32703c6000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800040e0, 0xdd378fb03c28b4a6, 0x409debee08c0868f); /* sram_ 1_ 0: a=0x200800040e0 d0=0xdd378fb03c28b4a6 d1=0x409debee08c0868f */
    tu.IndirectWrite(0x0200801001a8, 0x00001ee55d654b61, 0x00001d1aa29ab49e); /* TCAM[ 0][ 0][424].word1 = 0x8d514d5a4f  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150d4, 0x32d43d6000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x200800150d4 d0=0x32d43d6000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800041a8, 0xf808bf3ae5b4e173, 0xdc8fce54c5a52686); /* sram_ 1_ 0: a=0x200800041a8 d0=0xf808bf3ae5b4e173 d1=0xdc8fce54c5a52686 */
    tu.IndirectWrite(0x0200801001f7, 0x00001ffffffffbeb, 0x00001c0000000414); /* TCAM[ 0][ 0][503].word1 = 0x000000020a  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150fb, 0x0000000000000000, 0x32fbbcc000000000); /* sram_ 5_ 4: a=0x200800150fb d0=0x0 d1=0x32fbbcc000000000 */
    tu.IndirectWrite(0x0200800041f7, 0x6291046679eeae24, 0x70b3c8b9061ee425); /* sram_ 1_ 0: a=0x200800041f7 d0=0x6291046679eeae24 d1=0x70b3c8b9061ee425 */
    tu.IndirectWrite(0x0200801001cc, 0x00001fffff53ffff, 0x00001c0000ac0000); /* TCAM[ 0][ 0][460].word1 = 0x0000560000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e6, 0x32e63d7000000000, 0x32e6bd1000000000); /* sram_ 5_ 4: a=0x200800150e6 d0=0x32e63d7000000000 d1=0x32e6bd1000000000 */
    tu.IndirectWrite(0x0200800041cc, 0xc0cbfdb168a0b73a, 0x16c04769a4e7daf5); /* sram_ 1_ 0: a=0x200800041cc d0=0xc0cbfdb168a0b73a d1=0x16c04769a4e7daf5 */
    tu.IndirectWrite(0x0200801001d0, 0x00001e27589ac73b, 0x00001dd8a76538c4); /* TCAM[ 0][ 0][464].word1 = 0xec53b29c62  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e8, 0x32e83c8000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x200800150e8 d0=0x32e83c8000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800041d0, 0x4f1de68105057c42, 0xf62f89668ca5d791); /* sram_ 1_ 0: a=0x200800041d0 d0=0x4f1de68105057c42 d1=0xf62f89668ca5d791 */
    tu.IndirectWrite(0x02008010002b, 0x00001f54ac244edd, 0x00001cab53dbb122); /* TCAM[ 0][ 0][ 43].word1 = 0x55a9edd891  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015015, 0x0000000000000000, 0x3215bc9000000000); /* sram_ 5_ 4: a=0x20080015015 d0=0x0 d1=0x3215bc9000000000 */
    tu.IndirectWrite(0x02008000402b, 0x35adec5982ea54be, 0xd942ea127c6be0e9); /* sram_ 1_ 0: a=0x2008000402b d0=0x35adec5982ea54be d1=0xd942ea127c6be0e9 */
    tu.IndirectWrite(0x0200801000db, 0x00001ffffffe7e9f, 0x00001c0000018160); /* TCAM[ 0][ 0][219].word1 = 0x000000c0b0  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008001506d, 0x0000000000000000, 0x326dbc8000000000); /* sram_ 5_ 4: a=0x2008001506d d0=0x0 d1=0x326dbc8000000000 */
    tu.IndirectWrite(0x0200800040db, 0xd5eeafb4bca217a6, 0x12c86adf104cdd30); /* sram_ 1_ 0: a=0x200800040db d0=0xd5eeafb4bca217a6 d1=0x12c86adf104cdd30 */
    tu.IndirectWrite(0x0200801001f9, 0x00001fffffffffff, 0x00001c0000000000); /* TCAM[ 0][ 0][505].word1 = 0x0000000000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150fc, 0x32fc3c4000000000, 0x32fcbd3000000000); /* sram_ 5_ 4: a=0x200800150fc d0=0x32fc3c4000000000 d1=0x32fcbd3000000000 */
    tu.IndirectWrite(0x0200800041f9, 0x6fb925af40b0337e, 0x800882d361c3ef71); /* sram_ 1_ 0: a=0x200800041f9 d0=0x6fb925af40b0337e d1=0x800882d361c3ef71 */
    tu.IndirectWrite(0x02008010007e, 0x00001fffff49ffff, 0x00001c0000b60000); /* TCAM[ 0][ 0][126].word1 = 0x00005b0000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008001503f, 0x323f3d1000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x2008001503f d0=0x323f3d1000000000 d1=0x0 */
    tu.IndirectWrite(0x02008000407e, 0xc2fb0121e4f6e98d, 0xdc798daefb176ec6); /* sram_ 1_ 0: a=0x2008000407e d0=0xc2fb0121e4f6e98d d1=0xdc798daefb176ec6 */
    tu.IndirectWrite(0x0200801001cf, 0x00001ffffffe1e87, 0x00001c000001e178); /* TCAM[ 0][ 0][463].word1 = 0x000000f0bc  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e7, 0x0000000000000000, 0x32e7bc7000000000); /* sram_ 5_ 4: a=0x200800150e7 d0=0x0 d1=0x32e7bc7000000000 */
    tu.IndirectWrite(0x0200800041cf, 0x9ee012dba7cb6765, 0x0488ea502e16e394); /* sram_ 1_ 0: a=0x200800041cf d0=0x9ee012dba7cb6765 d1=0x488ea502e16e394 */
    tu.IndirectWrite(0x0200801001bc, 0x00001eef1009b51d, 0x00001d10eff64ae2); /* TCAM[ 0][ 0][444].word1 = 0x8877fb2571  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150de, 0x32de3d2000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x200800150de d0=0x32de3d2000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800041bc, 0xc26cb40ceaa77567, 0xc6707e9b63e47bc7); /* sram_ 1_ 0: a=0x200800041bc d0=0xc26cb40ceaa77567 d1=0xc6707e9b63e47bc7 */
    tu.IndirectWrite(0x02008010010c, 0x00001e0020620001, 0x00001dffdf9dfffe); /* TCAM[ 0][ 0][268].word1 = 0xffefceffff  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015086, 0x32863d2000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x20080015086 d0=0x32863d2000000000 d1=0x0 */
    tu.IndirectWrite(0x02008000410c, 0xf2da4e2d7fe63d21, 0x9a53a36f36eb7ead); /* sram_ 1_ 0: a=0x2008000410c d0=0xf2da4e2d7fe63d21 d1=0x9a53a36f36eb7ead */
    tu.IndirectWrite(0x020080100104, 0x00001ffffff5ffff, 0x00001c00000a0000); /* TCAM[ 0][ 0][260].word1 = 0x0000050000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015082, 0x32823ca000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x20080015082 d0=0x32823ca000000000 d1=0x0 */
    tu.IndirectWrite(0x020080004104, 0xa591f56ef1fc1b59, 0x7daf7709e5798709); /* sram_ 1_ 0: a=0x20080004104 d0=0xa591f56ef1fc1b59 d1=0x7daf7709e5798709 */
    tu.IndirectWrite(0x02008010019f, 0x00001f050300fefd, 0x00001cfafcff0102); /* TCAM[ 0][ 0][415].word1 = 0x7d7e7f8081  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150cf, 0x32cf3d5000000000, 0x32cfbc9000000000); /* sram_ 5_ 4: a=0x200800150cf d0=0x32cf3d5000000000 d1=0x32cfbc9000000000 */
    tu.IndirectWrite(0x02008000419f, 0x88e2c13908858bec, 0x9d6e87af75ca4647); /* sram_ 1_ 0: a=0x2008000419f d0=0x88e2c13908858bec d1=0x9d6e87af75ca4647 */
    tu.IndirectWrite(0x0200801001f6, 0x00001ebffffff97f, 0x00001d4000000680); /* TCAM[ 0][ 0][502].word1 = 0xa000000340  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150fb, 0x32fb3cd000000000, 0x32fbbcc000000000); /* sram_ 5_ 4: a=0x200800150fb d0=0x32fb3cd000000000 d1=0x32fbbcc000000000 */
    tu.IndirectWrite(0x0200800041f6, 0x0ec575350d60ff2b, 0x6d0f305efed1c5d0); /* sram_ 1_ 0: a=0x200800041f6 d0=0xec575350d60ff2b d1=0x6d0f305efed1c5d0 */
    tu.IndirectWrite(0x0200801001b4, 0x00001fd5b5959d95, 0x00001c2a4a6a626a); /* TCAM[ 0][ 0][436].word1 = 0x1525353135  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150da, 0x32da3cf000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x200800150da d0=0x32da3cf000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800041b4, 0xc18ca2f59ea1f0a6, 0x64fddb86fdfa0a50); /* sram_ 1_ 0: a=0x200800041b4 d0=0xc18ca2f59ea1f0a6 d1=0x64fddb86fdfa0a50 */
    tu.IndirectWrite(0x0200801001aa, 0x00001ebfbecd9553, 0x00001d4041326aac); /* TCAM[ 0][ 0][426].word1 = 0xa020993556  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150d5, 0x32d53c3000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x200800150d5 d0=0x32d53c3000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800041aa, 0x6e59ddc033dbd28f, 0x07d519e05001c96d); /* sram_ 1_ 0: a=0x200800041aa d0=0x6e59ddc033dbd28f d1=0x7d519e05001c96d */
    tu.IndirectWrite(0x0200801001ab, 0x00001f8119972c57, 0x00001c7ee668d3a8); /* TCAM[ 0][ 0][427].word1 = 0x3f733469d4  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150d5, 0x32d53c3000000000, 0x32d5bcc000000000); /* sram_ 5_ 4: a=0x200800150d5 d0=0x32d53c3000000000 d1=0x32d5bcc000000000 */
    tu.IndirectWrite(0x0200800041ab, 0xcf12769844cd9b6e, 0xcf73fbbbf8cf33eb); /* sram_ 1_ 0: a=0x200800041ab d0=0xcf12769844cd9b6e d1=0xcf73fbbbf8cf33eb */
    tu.IndirectWrite(0x02008010002d, 0x00001fffdf8fffff, 0x00001c0020700000); /* TCAM[ 0][ 0][ 45].word1 = 0x0010380000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015016, 0x0000000000000000, 0x3216bd3000000000); /* sram_ 5_ 4: a=0x20080015016 d0=0x0 d1=0x3216bd3000000000 */
    tu.IndirectWrite(0x02008000402d, 0xf932a73829660e05, 0x29b4e2702e905c6c); /* sram_ 1_ 0: a=0x2008000402d d0=0xf932a73829660e05 d1=0x29b4e2702e905c6c */
    tu.IndirectWrite(0x0200801000ea, 0x00001e1e7b5d5c5b, 0x00001de184a2a3a4); /* TCAM[ 0][ 0][234].word1 = 0xf0c25151d2  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015075, 0x32753d3000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x20080015075 d0=0x32753d3000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800040ea, 0xe199547b4d08a800, 0x6a11ad083b560aca); /* sram_ 1_ 0: a=0x200800040ea d0=0xe199547b4d08a800 d1=0x6a11ad083b560aca */
    tu.IndirectWrite(0x0200801001a9, 0x00001fbd53001f99, 0x00001c42acffe066); /* TCAM[ 0][ 0][425].word1 = 0x21567ff033  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150d4, 0x32d43d6000000000, 0x32d4bc3000000000); /* sram_ 5_ 4: a=0x200800150d4 d0=0x32d43d6000000000 d1=0x32d4bc3000000000 */
    tu.IndirectWrite(0x0200800041a9, 0xcb9eaeb95aeffe48, 0x13a737aa6a7b201f); /* sram_ 1_ 0: a=0x200800041a9 d0=0xcb9eaeb95aeffe48 d1=0x13a737aa6a7b201f */
    tu.IndirectWrite(0x0200801001bd, 0x00001fffffffffff, 0x00001c0000000000); /* TCAM[ 0][ 0][445].word1 = 0x0000000000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150de, 0x32de3d2000000000, 0x32debc8000000000); /* sram_ 5_ 4: a=0x200800150de d0=0x32de3d2000000000 d1=0x32debc8000000000 */
    tu.IndirectWrite(0x0200800041bd, 0xa485f9d4ee5ac451, 0x736a359fd6ffc4f1); /* sram_ 1_ 0: a=0x200800041bd d0=0xa485f9d4ee5ac451 d1=0x736a359fd6ffc4f1 */
    tu.IndirectWrite(0x020080100007, 0x00001fe0f0783c1f, 0x00001c1f0f87c3e0); /* TCAM[ 0][ 0][  7].word1 = 0x0f87c3e1f0  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015003, 0x0000000000000000, 0x3203bd4000000000); /* sram_ 5_ 4: a=0x20080015003 d0=0x0 d1=0x3203bd4000000000 */
    tu.IndirectWrite(0x020080004007, 0x7252a97d7c471313, 0x5b30221e76c39b03); /* sram_ 1_ 0: a=0x20080004007 d0=0x7252a97d7c471313 d1=0x5b30221e76c39b03 */
    tu.IndirectWrite(0x0200801001b5, 0x00001e3573398a2d, 0x00001dca8cc675d2); /* TCAM[ 0][ 0][437].word1 = 0xe546633ae9  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150da, 0x32da3cf000000000, 0x32dabcc000000000); /* sram_ 5_ 4: a=0x200800150da d0=0x32da3cf000000000 d1=0x32dabcc000000000 */
    tu.IndirectWrite(0x0200800041b5, 0x1268f5752fdbdf97, 0xbd2bb525f3d13fbf); /* sram_ 1_ 0: a=0x200800041b5 d0=0x1268f5752fdbdf97 d1=0xbd2bb525f3d13fbf */
    tu.IndirectWrite(0x0200801001f4, 0x00001fffff29ffff, 0x00001c0000d60000); /* TCAM[ 0][ 0][500].word1 = 0x00006b0000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150fa, 0x32fa3d7000000000, 0x32fabd1000000000); /* sram_ 5_ 4: a=0x200800150fa d0=0x32fa3d7000000000 d1=0x32fabd1000000000 */
    tu.IndirectWrite(0x0200800041f4, 0x431e4d659a0d4b1b, 0xe836efcda8a992af); /* sram_ 1_ 0: a=0x200800041f4 d0=0x431e4d659a0d4b1b d1=0xe836efcda8a992af */
    tu.IndirectWrite(0x020080100103, 0x00001fc1a1918985, 0x00001c3e5e6e767a); /* TCAM[ 0][ 0][259].word1 = 0x1f2f373b3d  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015081, 0x0000000000000000, 0x3281bc1000000000); /* sram_ 5_ 4: a=0x20080015081 d0=0x0 d1=0x3281bc1000000000 */
    tu.IndirectWrite(0x020080004103, 0xeeeb3a90959acd38, 0x012f63ee64cff38b); /* sram_ 1_ 0: a=0x20080004103 d0=0xeeeb3a90959acd38 d1=0x12f63ee64cff38b */
    tu.IndirectWrite(0x0200801001c5, 0x00001fffffd9ffff, 0x00001c0000260000); /* TCAM[ 0][ 0][453].word1 = 0x0000130000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e2, 0x0000000000000000, 0x32e2bc9000000000); /* sram_ 5_ 4: a=0x200800150e2 d0=0x0 d1=0x32e2bc9000000000 */
    tu.IndirectWrite(0x0200800041c5, 0x3dc3ca90fb1e48e3, 0x641406e897ae63e5); /* sram_ 1_ 0: a=0x200800041c5 d0=0x3dc3ca90fb1e48e3 d1=0x641406e897ae63e5 */
    tu.IndirectWrite(0x020080100107, 0x00001f1fd35f00b3, 0x00001ce02ca0ff4c); /* TCAM[ 0][ 0][263].word1 = 0x7016507fa6  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015083, 0x32833c9000000000, 0x3283bc8000000000); /* sram_ 5_ 4: a=0x20080015083 d0=0x32833c9000000000 d1=0x3283bc8000000000 */
    tu.IndirectWrite(0x020080004107, 0xc76b04fb3984eefa, 0x2de1e8df007477fd); /* sram_ 1_ 0: a=0x20080004107 d0=0xc76b04fb3984eefa d1=0x2de1e8df007477fd */
    tu.IndirectWrite(0x020080100105, 0x00001fdffffff7bf, 0x00001c2000000840); /* TCAM[ 0][ 0][261].word1 = 0x1000000420  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015082, 0x32823ca000000000, 0x3282bd4000000000); /* sram_ 5_ 4: a=0x20080015082 d0=0x32823ca000000000 d1=0x3282bd4000000000 */
    tu.IndirectWrite(0x020080004105, 0x599eb5d964708f35, 0x45a6472e3d4625a2); /* sram_ 1_ 0: a=0x20080004105 d0=0x599eb5d964708f35 d1=0x45a6472e3d4625a2 */
    tu.IndirectWrite(0x0200801001dc, 0x00001ee4e2e0dedd, 0x00001d1b1d1f2122); /* TCAM[ 0][ 0][476].word1 = 0x8d8e8f9091  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150ee, 0x32ee3d6000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x200800150ee d0=0x32ee3d6000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800041dc, 0x869a332ee82b3c34, 0x0bab0d64e0d6afcc); /* sram_ 1_ 0: a=0x200800041dc d0=0x869a332ee82b3c34 d1=0xbab0d64e0d6afcc */
    tu.IndirectWrite(0x0200801001db, 0x00001fffdf77ffff, 0x00001c0020880000); /* TCAM[ 0][ 0][475].word1 = 0x0010440000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150ed, 0x0000000000000000, 0x32edbc1000000000); /* sram_ 5_ 4: a=0x200800150ed d0=0x0 d1=0x32edbc1000000000 */
    tu.IndirectWrite(0x0200800041db, 0x0f16c521a7f9dc44, 0xf257d93a42c78b37); /* sram_ 1_ 0: a=0x200800041db d0=0xf16c521a7f9dc44 d1=0xf257d93a42c78b37 */
    tu.IndirectWrite(0x0200801001b7, 0x00001f5caf57abd5, 0x00001ca350a8542a); /* TCAM[ 0][ 0][439].word1 = 0x51a8542a15  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150db, 0x0000000000000000, 0x32dbbc0000000000); /* sram_ 5_ 4: a=0x200800150db d0=0x0 d1=0x32dbbc0000000000 */
    tu.IndirectWrite(0x0200800041b7, 0xcf6cc526ebf09a72, 0x48acad1aea52aed9); /* sram_ 1_ 0: a=0x200800041b7 d0=0xcf6cc526ebf09a72 d1=0x48acad1aea52aed9 */
    tu.IndirectWrite(0x0200801000e8, 0x00001edc6f379bcd, 0x00001d2390c86432); /* TCAM[ 0][ 0][232].word1 = 0x91c8643219  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015074, 0x32743ca000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x20080015074 d0=0x32743ca000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800040e8, 0x5f9def0990524877, 0x61989a2573425335); /* sram_ 1_ 0: a=0x200800040e8 d0=0x5f9def0990524877 d1=0x61989a2573425335 */
    tu.IndirectWrite(0x0200801001d2, 0x00001e99541bafed, 0x00001d66abe45012); /* TCAM[ 0][ 0][466].word1 = 0xb355f22809  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e9, 0x32e93d7000000000, 0x32e9bc1000000000); /* sram_ 5_ 4: a=0x200800150e9 d0=0x32e93d7000000000 d1=0x32e9bc1000000000 */
    tu.IndirectWrite(0x0200800041d2, 0xa957a1e68d7a0a49, 0x956dc52098b2c102); /* sram_ 1_ 0: a=0x200800041d2 d0=0xa957a1e68d7a0a49 d1=0x956dc52098b2c102 */
    tu.IndirectWrite(0x02008010006c, 0x00001fffff15ffff, 0x00001c0000ea0000); /* TCAM[ 0][ 0][108].word1 = 0x0000750000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015036, 0x32363d2000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x20080015036 d0=0x32363d2000000000 d1=0x0 */
    tu.IndirectWrite(0x02008000406c, 0xc80133a2377fe34b, 0x49d2f7b17b8a1f38); /* sram_ 1_ 0: a=0x2008000406c d0=0xc80133a2377fe34b d1=0x49d2f7b17b8a1f38 */
    tu.IndirectWrite(0x020080100006, 0x00001f8600f6eff9, 0x00001c79ff091006); /* TCAM[ 0][ 0][  6].word1 = 0x3cff848803  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015003, 0x32033d0000000000, 0x3203bd4000000000); /* sram_ 5_ 4: a=0x20080015003 d0=0x32033d0000000000 d1=0x3203bd4000000000 */
    tu.IndirectWrite(0x020080004006, 0x9656c6fe647ac08e, 0xbd22a227eb47aa22); /* sram_ 1_ 0: a=0x20080004006 d0=0x9656c6fe647ac08e d1=0xbd22a227eb47aa22 */
    tu.IndirectWrite(0x0200801001e9, 0x00001f78b591d381, 0x00001c874a6e2c7e); /* TCAM[ 0][ 0][489].word1 = 0x43a537163f  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150f4, 0x0000000000000000, 0x32f4bc8000000000); /* sram_ 5_ 4: a=0x200800150f4 d0=0x0 d1=0x32f4bc8000000000 */
    tu.IndirectWrite(0x0200800041e9, 0x7306535f48cfcae1, 0x3c9fdd536d9d5e2f); /* sram_ 1_ 0: a=0x200800041e9 d0=0x7306535f48cfcae1 d1=0x3c9fdd536d9d5e2f */
    tu.IndirectWrite(0x0200801001c2, 0x00001ffffffe9fa7, 0x00001c0000016058); /* TCAM[ 0][ 0][450].word1 = 0x000000b02c  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e1, 0x32e13d1000000000, 0x32e1bd0000000000); /* sram_ 5_ 4: a=0x200800150e1 d0=0x32e13d1000000000 d1=0x32e1bd0000000000 */
    tu.IndirectWrite(0x0200800041c2, 0xe93b78bdb0d96703, 0xa1812cb5e1995dfa); /* sram_ 1_ 0: a=0x200800041c2 d0=0xe93b78bdb0d96703 d1=0xa1812cb5e1995dfa */
    tu.IndirectWrite(0x020080100074, 0x00001fecfffffff7, 0x00001c1300000008); /* TCAM[ 0][ 0][116].word1 = 0x0980000004  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008001503a, 0x323a3c6000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x2008001503a d0=0x323a3c6000000000 d1=0x0 */
    tu.IndirectWrite(0x020080004074, 0x98f2f7b0e735856c, 0x45fcada35b726dc6); /* sram_ 1_ 0: a=0x20080004074 d0=0x98f2f7b0e735856c d1=0x45fcada35b726dc6 */
    tu.IndirectWrite(0x0200801000e9, 0x00001f6565756d65, 0x00001c9a9a8a929a); /* TCAM[ 0][ 0][233].word1 = 0x4d4d45494d  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015074, 0x32743ca000000000, 0x3274bd0000000000); /* sram_ 5_ 4: a=0x20080015074 d0=0x32743ca000000000 d1=0x3274bd0000000000 */
    tu.IndirectWrite(0x0200800040e9, 0xc4ea420eebe817b4, 0xd932d76892e50e38); /* sram_ 1_ 0: a=0x200800040e9 d0=0xc4ea420eebe817b4 d1=0xd932d76892e50e38 */
    tu.IndirectWrite(0x02008010002c, 0x00001ffffffff7d9, 0x00001c0000000826); /* TCAM[ 0][ 0][ 44].word1 = 0x0000000413  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015016, 0x32163d1000000000, 0x3216bd3000000000); /* sram_ 5_ 4: a=0x20080015016 d0=0x32163d1000000000 d1=0x3216bd3000000000 */
    tu.IndirectWrite(0x02008000402c, 0xb5584aef31f4e03d, 0x67cdfac9553fd491); /* sram_ 1_ 0: a=0x2008000402c d0=0xb5584aef31f4e03d d1=0x67cdfac9553fd491 */
    tu.IndirectWrite(0x020080100082, 0x00001f850a162c59, 0x00001c7af5e9d3a6); /* TCAM[ 0][ 0][130].word1 = 0x3d7af4e9d3  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015041, 0x32413cd000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x20080015041 d0=0x32413cd000000000 d1=0x0 */
    tu.IndirectWrite(0x020080004082, 0xe2b872a33f224964, 0xfd85b05c756a90c0); /* sram_ 1_ 0: a=0x20080004082 d0=0xe2b872a33f224964 d1=0xfd85b05c756a90c0 */
    tu.IndirectWrite(0x0200801001bb, 0x00001ff3f5f7f9fb, 0x00001c0c0a080604); /* TCAM[ 0][ 0][443].word1 = 0x0605040302  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150dd, 0x0000000000000000, 0x32ddbca000000000); /* sram_ 5_ 4: a=0x200800150dd d0=0x0 d1=0x32ddbca000000000 */
    tu.IndirectWrite(0x0200800041bb, 0xb892042f78641e83, 0xecbf92a4da6d8b90); /* sram_ 1_ 0: a=0x200800041bb d0=0xb892042f78641e83 d1=0xecbf92a4da6d8b90 */
    tu.IndirectWrite(0x0200801001e8, 0x00001fffdf5dffff, 0x00001c0020a20000); /* TCAM[ 0][ 0][488].word1 = 0x0010510000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150f4, 0x32f43cc000000000, 0x32f4bc8000000000); /* sram_ 5_ 4: a=0x200800150f4 d0=0x32f43cc000000000 d1=0x32f4bc8000000000 */
    tu.IndirectWrite(0x0200800041e8, 0xe7e07d5b20af4bce, 0xd1593e6075bbb400); /* sram_ 1_ 0: a=0x200800041e8 d0=0xe7e07d5b20af4bce d1=0xd1593e6075bbb400 */
    tu.IndirectWrite(0x0200801001d5, 0x00001f0bd84fc123, 0x00001cf427b03edc); /* TCAM[ 0][ 0][469].word1 = 0x7a13d81f6e  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150ea, 0x0000000000000000, 0x32eabd2000000000); /* sram_ 5_ 4: a=0x200800150ea d0=0x0 d1=0x32eabd2000000000 */
    tu.IndirectWrite(0x0200800041d5, 0xecf223ce82ef21ce, 0x02b531eae00436be); /* sram_ 1_ 0: a=0x200800041d5 d0=0xecf223ce82ef21ce d1=0x2b531eae00436be */
    tu.IndirectWrite(0x0200801001d4, 0x00001fedeff1f3f5, 0x00001c12100e0c0a); /* TCAM[ 0][ 0][468].word1 = 0x0908070605  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150ea, 0x32ea3d7000000000, 0x32eabd2000000000); /* sram_ 5_ 4: a=0x200800150ea d0=0x32ea3d7000000000 d1=0x32eabd2000000000 */
    tu.IndirectWrite(0x0200800041d4, 0x057d6c74c1bbb7f8, 0x3ddc17dfad6b6424); /* sram_ 1_ 0: a=0x200800041d4 d0=0x57d6c74c1bbb7f8 d1=0x3ddc17dfad6b6424 */
    tu.IndirectWrite(0x0200801000fd, 0x00001e082d08960f, 0x00001df7d2f769f0); /* TCAM[ 0][ 0][253].word1 = 0xfbe97bb4f8  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008001507e, 0x0000000000000000, 0x327ebd0000000000); /* sram_ 5_ 4: a=0x2008001507e d0=0x0 d1=0x327ebd0000000000 */
    tu.IndirectWrite(0x0200800040fd, 0x007905816b54f768, 0x86e90db2a740eb42); /* sram_ 1_ 0: a=0x200800040fd d0=0x7905816b54f768 d1=0x86e90db2a740eb42 */
    tu.IndirectWrite(0x0200801001be, 0x00001fffffffffff, 0x00001c0000000000); /* TCAM[ 0][ 0][446].word1 = 0x0000000000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150df, 0x32df3cc000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x200800150df d0=0x32df3cc000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800041be, 0x57192b8d3638e658, 0x6da9d5f7f1364bea); /* sram_ 1_ 0: a=0x200800041be d0=0x57192b8d3638e658 d1=0x6da9d5f7f1364bea */
    tu.IndirectWrite(0x0200801001f1, 0x00001e0020ac0001, 0x00001dffdf53fffe); /* TCAM[ 0][ 0][497].word1 = 0xffefa9ffff  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150f8, 0x0000000000000000, 0x32f8bc8000000000); /* sram_ 5_ 4: a=0x200800150f8 d0=0x0 d1=0x32f8bc8000000000 */
    tu.IndirectWrite(0x0200800041f1, 0x9b2dba1aaa3a5dbf, 0xb3af8b6e20fb5045); /* sram_ 1_ 0: a=0x200800041f1 d0=0x9b2dba1aaa3a5dbf d1=0xb3af8b6e20fb5045 */
    tu.IndirectWrite(0x0200801000fa, 0x00001e8ffffff51f, 0x00001d7000000ae0); /* TCAM[ 0][ 0][250].word1 = 0xb800000570  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008001507d, 0x327d3c2000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x2008001507d d0=0x327d3c2000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800040fa, 0xe7f7b6c1bb02ad62, 0x02acc1315af060ed); /* sram_ 1_ 0: a=0x200800040fa d0=0xe7f7b6c1bb02ad62 d1=0x2acc1315af060ed */
    tu.IndirectWrite(0x0200801000e4, 0x00001f3ca346e799, 0x00001cc35cb91866); /* TCAM[ 0][ 0][228].word1 = 0x61ae5c8c33  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015072, 0x32723c8000000000, 0x3272bd5000000000); /* sram_ 5_ 4: a=0x20080015072 d0=0x32723c8000000000 d1=0x3272bd5000000000 */
    tu.IndirectWrite(0x0200800040e4, 0xcfaec5aa25a8f90f, 0xd202c6ccde2bc06b); /* sram_ 1_ 0: a=0x200800040e4 d0=0xcfaec5aa25a8f90f d1=0xd202c6ccde2bc06b */
    tu.IndirectWrite(0x0200801001f0, 0x00001ffffef3ffff, 0x00001c00010c0000); /* TCAM[ 0][ 0][496].word1 = 0x0000860000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150f8, 0x32f83d2000000000, 0x32f8bc8000000000); /* sram_ 5_ 4: a=0x200800150f8 d0=0x32f83d2000000000 d1=0x32f8bc8000000000 */
    tu.IndirectWrite(0x0200800041f0, 0xa5f72d03577f105e, 0x304ef79700cf0839); /* sram_ 1_ 0: a=0x200800041f0 d0=0xa5f72d03577f105e d1=0x304ef79700cf0839 */
    tu.IndirectWrite(0x020080100120, 0x00001eb6b4b2b0af, 0x00001d494b4d4f50); /* TCAM[ 0][ 0][288].word1 = 0xa4a5a6a7a8  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015090, 0x32903d7000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x20080015090 d0=0x32903d7000000000 d1=0x0 */
    tu.IndirectWrite(0x020080004120, 0x4edca4000e13f912, 0x3e283cbc28e55c12); /* sram_ 1_ 0: a=0x20080004120 d0=0x4edca4000e13f912 d1=0x3e283cbc28e55c12 */
    tu.IndirectWrite(0x0200801000fc, 0x00001ef8851652bb, 0x00001d077ae9ad44); /* TCAM[ 0][ 0][252].word1 = 0x83bd74d6a2  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008001507e, 0x327e3c0000000000, 0x327ebd0000000000); /* sram_ 5_ 4: a=0x2008001507e d0=0x327e3c0000000000 d1=0x327ebd0000000000 */
    tu.IndirectWrite(0x0200800040fc, 0xc9684a6aac380f24, 0x816ccd0e63d67b6e); /* sram_ 1_ 0: a=0x200800040fc d0=0xc9684a6aac380f24 d1=0x816ccd0e63d67b6e */
    tu.IndirectWrite(0x0200801001ff, 0x00001f1c3a74e9d1, 0x00001ce3c58b162e); /* TCAM[ 0][ 0][511].word1 = 0x71e2c58b17  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150ff, 0x0000000000000000, 0x32ffbd0000000000); /* sram_ 5_ 4: a=0x200800150ff d0=0x0 d1=0x32ffbd0000000000 */
    tu.IndirectWrite(0x0200800041ff, 0xab4af6d476eda804, 0xda1cd9527053b352); /* sram_ 1_ 0: a=0x200800041ff d0=0xab4af6d476eda804 d1=0xda1cd9527053b352 */
    tu.IndirectWrite(0x0200801001ce, 0x00001f63240932af, 0x00001c9cdbf6cd50); /* TCAM[ 0][ 0][462].word1 = 0x4e6dfb66a8  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e7, 0x32e73d5000000000, 0x32e7bc7000000000); /* sram_ 5_ 4: a=0x200800150e7 d0=0x32e73d5000000000 d1=0x32e7bc7000000000 */
    tu.IndirectWrite(0x0200800041ce, 0x15654bd4ca3ddd5c, 0x2baef4c7a785e08c); /* sram_ 1_ 0: a=0x200800041ce d0=0x15654bd4ca3ddd5c d1=0x2baef4c7a785e08c */
    tu.IndirectWrite(0x0200801000fe, 0x00001f39612312f5, 0x00001cc69edced0a); /* TCAM[ 0][ 0][254].word1 = 0x634f6e7685  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008001507f, 0x327f3d6000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x2008001507f d0=0x327f3d6000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800040fe, 0x90a80446321ceb1f, 0xbe109a584246e5a2); /* sram_ 1_ 0: a=0x200800040fe d0=0x90a80446321ceb1f d1=0xbe109a584246e5a2 */
    tu.IndirectWrite(0x02008010007f, 0x00001fffdf41ffff, 0x00001c0020be0000); /* TCAM[ 0][ 0][127].word1 = 0x00105f0000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008001503f, 0x323f3d1000000000, 0x323fbd1000000000); /* sram_ 5_ 4: a=0x2008001503f d0=0x323f3d1000000000 d1=0x323fbd1000000000 */
    tu.IndirectWrite(0x02008000407f, 0x4ddb53defc35bdbf, 0x6d4100d585473f89); /* sram_ 1_ 0: a=0x2008000407f d0=0x4ddb53defc35bdbf d1=0x6d4100d585473f89 */
    tu.IndirectWrite(0x0200801001c4, 0x00001ec0e0d0c8c5, 0x00001d3f1f2f373a); /* TCAM[ 0][ 0][452].word1 = 0x9f8f979b9d  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150e2, 0x32e23cd000000000, 0x32e2bc9000000000); /* sram_ 5_ 4: a=0x200800150e2 d0=0x32e23cd000000000 d1=0x32e2bc9000000000 */
    tu.IndirectWrite(0x0200800041c4, 0x725767e5da273a68, 0x3a19b6d94558b19f); /* sram_ 1_ 0: a=0x200800041c4 d0=0x725767e5da273a68 d1=0x3a19b6d94558b19f */
    tu.IndirectWrite(0x020080100002, 0x00001fe7bffffff3, 0x00001c184000000c); /* TCAM[ 0][ 0][  2].word1 = 0x0c20000006  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080015001, 0x32013c6000000000, 0x0000000000000000); /* sram_ 5_ 4: a=0x20080015001 d0=0x32013c6000000000 d1=0x0 */
    tu.IndirectWrite(0x020080004002, 0x186eb63450bf178e, 0xd879d367540e8fb0); /* sram_ 1_ 0: a=0x20080004002 d0=0x186eb63450bf178e d1=0xd879d367540e8fb0 */
    tu.IndirectWrite(0x0200801001b6, 0x00001efeeb558cbd, 0x00001d0114aa7342); /* TCAM[ 0][ 0][438].word1 = 0x808a5539a1  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800150db, 0x32db3d1000000000, 0x32dbbc0000000000); /* sram_ 5_ 4: a=0x200800150db d0=0x32db3d1000000000 d1=0x32dbbc0000000000 */
    tu.IndirectWrite(0x0200800041b6, 0x63810bc8619c789b, 0x2581f0b537497a30); /* sram_ 1_ 0: a=0x200800041b6 d0=0x63810bc8619c789b d1=0x2581f0b537497a30 */


    // Read back word0/1 from index 279/487 to check correct
    uint64_t word0, word1;
    tcam->read(279, &word0, &word1, UINT64_C(0));
    printf("TCAM(0,0,279) word0 = 0x%016" PRIx64 " word1 = 0x%016" PRIx64 "\n",
           word0, word1);
    tcam->read(487, &word0, &word1, UINT64_C(0));
    printf("TCAM(0,0,487) word0 = 0x%016" PRIx64 " word1 = 0x%016" PRIx64 "\n",
           word0, word1);

    // Now look up search0/search1 where search0=<all ones> search1=<all zeros>
    uint64_t srch1 = UINT64_C(0x0);
    BitVector<44> bv1(srch1);

    //flags = ALL;
    //tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
    int index_pri = 512; // Index & Pri identical in this case
    while (index_pri > 0) {
      int index_pri_lookup = index_pri-1;
      printf("Looking up TCAM(0,0,%d..0) >>>>>>\n", index_pri_lookup);
      index_pri = tcam->lookup(bv1, index_pri_lookup, 0);
      printf("Looked up TCAM(0,0,%d..0) = %d <<<<<<\n", index_pri_lookup, index_pri);
    }


   
    /*
    Phv *phv_in2 = tu.phv_alloc();
    phv_in2->set_ingress();

    
    // Setup a port on pipe 0
    // This also sets up basic config ingress parser and deparser
    Port *port = tu.port_get(16);


    RMT_UT_LOG_INFO("Lookup PHV of standard packet...\n");
    // Construct a packet - DA,SA,SIP,DIP,Proto,SPort,DPort
    Packet *p_in = tu.packet_make("08:00:22:AA:BB:CC", "08:00:11:DD:EE:FF",
                                  "10.17.34.51", "10.68.85.102",
                                  TestUtil::kProtoTCP, 0x1188, 0x1199);
    // Parse the packet to get Phv
    Phv *phv_in1 = tu.port_parse(port, p_in);
    RMT_UT_LOG_INFO("Dv14Test::InPkt=%p [DA=%04X%08X]\n", p_in,
                phv_in1->get(Phv::make_word(4,0)), phv_in1->get(Phv::make_word(0,0)));
    Phv *phv_out1 = tu.port_process_inbound(port, phv_in1);
    // Free PHVs}}
    if ((phv_out1 != NULL) && (phv_out1 != phv_in1)) tu.phv_free(phv_out1);
    tu.phv_free(phv_in1);
    

    // Uncomment below to up the debug output
    //flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Switch on all parse loop output
    uint64_t pipes2 = UINT64_C(1) << pipe;
    uint64_t types2 = UINT64_C(1) << RmtTypes::kRmtTypeParser;
    uint64_t flags2 = RmtDebug::kRmtDebugParserParseLoop;
    tu.update_log_flags(pipes2, 0, types2, 0, 0, flags2, ALL);

    Phv *phv_out2 = tu.port_process_inbound(port, phv_in2);    

    // Check phv_out2
    int i;
    for (i = 0; i < 32; i++) {
      uint32_t actual = phv_out2->get(i);
      //if (i == 0) printf("EXPECTED=0x%08x ACTUAL=0x%08x\n", 0x55555555, actual);
      if (i < 10) printf("OutputPHV<%d>=0x%08x\n", i, actual);
    }
    for (i = 32; i < 64; i++) {
      uint32_t actual = phv_out2->get(i);
      if (i == 32) printf("EXPECTED=0x%08x ACTUAL=0x%08x\n", 0x55555575, actual);
    }
    */

    
    // Check other PHVs stayed the same
    //EXPECT_EQ(64, phv_out2->get(64));
    //EXPECT_EQ(96, phv_out2->get(96));
    //EXPECT_EQ(128, phv_out2->get(128));
    
    // Free PHVs

    tu.finish_test();
    tu.quieten_log_flags();
}


}
