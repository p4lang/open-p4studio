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

// XXX -> test_dv48.cpp
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

  bool dv48_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv48Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv48_print) RMT_UT_LOG_INFO("test_dv48_packet1()\n");
    
    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    tu.set_dv_test(48);
    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
    
    
    auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);
    tu.OutWordPiT(0, 0, &mau_reg_map.dp.phv_ingress_thread[0][0], 0xffffffff); /* 0x20601c0 */
    tu.OutWordPiT(0, 1, &mau_reg_map.dp.phv_ingress_thread[0][1], 0xffffffff); /* 0x20601c4 */
    tu.OutWordPiT(0, 2, &mau_reg_map.dp.phv_ingress_thread[0][2], 0xffffffff); /* 0x20601c8 */
    tu.OutWordPiT(0, 3, &mau_reg_map.dp.phv_ingress_thread[0][3], 0xffffffff); /* 0x20601cc */
    tu.OutWordPiT(0, 4, &mau_reg_map.dp.phv_ingress_thread[0][4], 0xffffffff); /* 0x20601d0 */
    tu.OutWordPiT(0, 5, &mau_reg_map.dp.phv_ingress_thread[0][5], 0xffffffff); /* 0x20601d4 */
    tu.OutWordPiT(0, 6, &mau_reg_map.dp.phv_ingress_thread[0][6], 0xffffffff); /* 0x20601d8 */
    tu.OutWordPiT(1, 0, &mau_reg_map.dp.phv_ingress_thread[1][0], 0xffffffff); /* 0x20601e0 */
    tu.OutWordPiT(1, 1, &mau_reg_map.dp.phv_ingress_thread[1][1], 0xffffffff); /* 0x20601e4 */
    tu.OutWordPiT(1, 2, &mau_reg_map.dp.phv_ingress_thread[1][2], 0xffffffff); /* 0x20601e8 */
    tu.OutWordPiT(1, 3, &mau_reg_map.dp.phv_ingress_thread[1][3], 0xffffffff); /* 0x20601ec */
    tu.OutWordPiT(1, 4, &mau_reg_map.dp.phv_ingress_thread[1][4], 0xffffffff); /* 0x20601f0 */
    tu.OutWordPiT(1, 5, &mau_reg_map.dp.phv_ingress_thread[1][5], 0xffffffff); /* 0x20601f4 */
    tu.OutWordPiT(1, 6, &mau_reg_map.dp.phv_ingress_thread[1][6], 0xffffffff); /* 0x20601f8 */
    tu.OutWord(&mau_reg_map.dp.action_output_delay[0], 0xb); /* 0x2060060 */
    tu.OutWord(&mau_reg_map.dp.cur_stage_dependency_on_prev[0], 0x1); /* 0x2060070 */
    tu.OutWord(&mau_reg_map.dp.action_output_delay[1], 0xb); /* 0x2060068 */
    tu.OutWord(&mau_reg_map.dp.cur_stage_dependency_on_prev[1], 0x2); /* 0x2060078 */
    tu.OutWord(&mau_reg_map.dp.next_stage_dependency_on_cur[1], 0x2); /* 0x2060108 */
    tu.OutWord(&mau_reg_map.dp.phv_fifo_enable, 0x8); /* 0x2060138 */
    tu.OutWord(&mau_reg_map.dp.stage_concurrent_with_prev, 0x2); /* 0x2060180 */
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
    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.unitram_config[0][2], 0x2cba); /* 0x200af90 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[0], 0xb); /* 0x2008e40 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl, 0xb); /* 0x2008e44 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[2], 0xb); /* 0x2008e48 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[3], 0xb); /* 0x2008e4c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[4], 0xb); /* 0x2008e50 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[5], 0xb); /* 0x2008e54 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[0], 0xb); /* 0x2008e60 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[1], 0xb); /* 0x2008e64 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[2], 0xb); /* 0x2008e68 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[3], 0xb); /* 0x2008e6c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[4], 0xb); /* 0x2008e70 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[5], 0xb); /* 0x2008e74 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[0], 0xb); /* 0x2009e40 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl, 0xb); /* 0x2009e44 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[2], 0xb); /* 0x2009e48 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[3], 0xb); /* 0x2009e4c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[4], 0xb); /* 0x2009e50 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[5], 0xb); /* 0x2009e54 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[0], 0xb); /* 0x2009e60 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[1], 0xb); /* 0x2009e64 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[2], 0xb); /* 0x2009e68 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[3], 0xb); /* 0x2009e6c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[4], 0xb); /* 0x2009e70 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[5], 0xb); /* 0x2009e74 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[0], 0xb); /* 0x200ae40 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl, 0xb); /* 0x200ae44 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.unitram_config[0][2], 0x2cba); /* 0x200af90 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[2], 0xb); /* 0x200ae48 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[3], 0xb); /* 0x200ae4c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[4], 0xb); /* 0x200ae50 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[5], 0xb); /* 0x200ae54 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[0], 0xb); /* 0x200ae60 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[1], 0xb); /* 0x200ae64 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[2], 0xb); /* 0x200ae68 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[3], 0xb); /* 0x200ae6c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[4], 0xb); /* 0x200ae70 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[5], 0xb); /* 0x200ae74 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[0], 0xb); /* 0x200be40 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl, 0xb); /* 0x200be44 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[2], 0xb); /* 0x200be48 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[3], 0xb); /* 0x200be4c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[4], 0xb); /* 0x200be50 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[5], 0xb); /* 0x200be54 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[0], 0xb); /* 0x200be60 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[1], 0xb); /* 0x200be64 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[2], 0xb); /* 0x200be68 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[3], 0xb); /* 0x200be6c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[4], 0xb); /* 0x200be70 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[5], 0xb); /* 0x200be74 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[0], 0xb); /* 0x200ce40 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl, 0xb); /* 0x200ce44 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[2], 0xb); /* 0x200ce48 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[3], 0xb); /* 0x200ce4c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[4], 0xb); /* 0x200ce50 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[5], 0xb); /* 0x200ce54 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[0], 0xb); /* 0x200ce60 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[1], 0xb); /* 0x200ce64 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[2], 0xb); /* 0x200ce68 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[3], 0xb); /* 0x200ce6c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[4], 0xb); /* 0x200ce70 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[5], 0xb); /* 0x200ce74 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[0], 0xb); /* 0x200de40 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl, 0xb); /* 0x200de44 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[2], 0xb); /* 0x200de48 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[3], 0xb); /* 0x200de4c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[4], 0xb); /* 0x200de50 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[5], 0xb); /* 0x200de54 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[0], 0xb); /* 0x200de60 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[1], 0xb); /* 0x200de64 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[2], 0xb); /* 0x200de68 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[3], 0xb); /* 0x200de6c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[4], 0xb); /* 0x200de70 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[5], 0xb); /* 0x200de74 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[0], 0xb); /* 0x200ee40 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl, 0xb); /* 0x200ee44 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[2], 0xb); /* 0x200ee48 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[3], 0xb); /* 0x200ee4c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[4], 0xb); /* 0x200ee50 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[5], 0xb); /* 0x200ee54 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[0], 0xb); /* 0x200ee60 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[1], 0xb); /* 0x200ee64 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[2], 0xb); /* 0x200ee68 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[3], 0xb); /* 0x200ee6c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[4], 0xb); /* 0x200ee70 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[5], 0xb); /* 0x200ee74 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[0], 0xb); /* 0x200fe40 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl, 0xb); /* 0x200fe44 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[2], 0xb); /* 0x200fe48 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[3], 0xb); /* 0x200fe4c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[4], 0xb); /* 0x200fe50 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[5], 0xb); /* 0x200fe54 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[0], 0xb); /* 0x200fe60 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[1], 0xb); /* 0x200fe64 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[2], 0xb); /* 0x200fe68 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[3], 0xb); /* 0x200fe6c */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[4], 0xb); /* 0x200fe70 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[5], 0xb); /* 0x200fe74 */
    tu.OutWord(&mau_reg_map.rams.array.row[2].ram[2].unit_ram_ctl, 0x20); /* 0x203a118 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.unitram_config[0][0], 0x26ed9); /* 0x2008f80 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.unitram_config[0][0], 0x26d21); /* 0x2008f84 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_mask[0], 0xffffffff); /* 0x2038000 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_mask[1], 0xfffffff); /* 0x2038004 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].unit_ram_ctl, 0x60bf); /* 0x2038018 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].unit_ram_ctl, 0x60bf); /* 0x203801c */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_ram_vpn, 0x20203); /* 0x2038044 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_nibble_s1q0_enable, 0xffffffff); /* 0x203804c */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_nibble_s0q1_enable, 0x3fffffff); /* 0x2038048 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_bytemask[2], 0x1fcff); /* 0x2038068 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_bytemask[3], 0x2f3ff); /* 0x203806c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][0], 0x3); /* 0x2040f00 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][0], 0x2); /* 0x2040c00 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][1], 0x0); /* 0x2040c04 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][2], 0x0); /* 0x2040c08 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][3], 0x4); /* 0x2040c0c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][4], 0x6); /* 0x2040c10 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][5], 0x9); /* 0x2040c14 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][6], 0x8); /* 0x2040c18 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][7], 0x8); /* 0x2040c1c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][0], 0x5); /* 0x2040f80 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][0], 0xd); /* 0x2040e00 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][1], 0x1a); /* 0x2040f84 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][1], 0xc); /* 0x2040e04 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][0], 0x8); /* 0x2040f20 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][0], 0x4); /* 0x2040d00 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][1], 0x5); /* 0x2040d04 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][2], 0x0); /* 0x2040d08 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][3], 0x1); /* 0x2040d0c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][4], 0x8); /* 0x2040d10 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][5], 0xa); /* 0x2040d14 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][6], 0x8); /* 0x2040d18 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][7], 0xa); /* 0x2040d1c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][0], 0x8); /* 0x2040fc0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][0], 0xf); /* 0x2040e40 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][1], 0x1b); /* 0x2040fc4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][1], 0xc); /* 0x2040e44 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][1], 0x4); /* 0x2040f04 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][0], 0x3); /* 0x2040c20 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][1], 0x0); /* 0x2040c24 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][2], 0x2); /* 0x2040c28 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][3], 0x1); /* 0x2040c2c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][4], 0x5); /* 0x2040c30 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][5], 0x9); /* 0x2040c34 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][6], 0x5); /* 0x2040c38 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][7], 0x5); /* 0x2040c3c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][2], 0x1b); /* 0x2040f88 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][2], 0xd); /* 0x2040e08 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][3], 0x1c); /* 0x2040f8c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][3], 0xf); /* 0x2040e0c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][1], 0x5); /* 0x2040f24 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][0], 0x0); /* 0x2040d20 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][1], 0x3); /* 0x2040d24 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][2], 0x5); /* 0x2040d28 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][3], 0x1); /* 0x2040d2c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][4], 0x5); /* 0x2040d30 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][5], 0x7); /* 0x2040d34 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][6], 0x8); /* 0x2040d38 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][7], 0x6); /* 0x2040d3c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][2], 0x8); /* 0x2040fc8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][2], 0xc); /* 0x2040e48 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][3], 0x13); /* 0x2040fcc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][3], 0xc); /* 0x2040e4c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][2], 0xe); /* 0x2040f08 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][0], 0x1); /* 0x2040c40 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][1], 0x4); /* 0x2040c44 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][2], 0x4); /* 0x2040c48 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][3], 0x2); /* 0x2040c4c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][4], 0x9); /* 0x2040c50 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][5], 0x7); /* 0x2040c54 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][6], 0x7); /* 0x2040c58 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][7], 0x5); /* 0x2040c5c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][4], 0x1d); /* 0x2040f90 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][4], 0xf); /* 0x2040e10 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][5], 0x1); /* 0x2040f94 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][5], 0xd); /* 0x2040e14 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][2], 0x8); /* 0x2040f28 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][0], 0x3); /* 0x2040d40 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][1], 0x1); /* 0x2040d44 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][2], 0x1); /* 0x2040d48 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][3], 0x5); /* 0x2040d4c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][4], 0x9); /* 0x2040d50 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][5], 0x9); /* 0x2040d54 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][6], 0x7); /* 0x2040d58 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][7], 0xa); /* 0x2040d5c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][4], 0x1d); /* 0x2040fd0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][4], 0xf); /* 0x2040e50 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][5], 0x19); /* 0x2040fd4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][5], 0xd); /* 0x2040e54 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][3], 0x9); /* 0x2040f0c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][0], 0x1); /* 0x2040c60 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][1], 0x1); /* 0x2040c64 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][2], 0x4); /* 0x2040c68 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][3], 0x3); /* 0x2040c6c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][4], 0xa); /* 0x2040c70 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][5], 0xa); /* 0x2040c74 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][6], 0x9); /* 0x2040c78 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][7], 0x5); /* 0x2040c7c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][6], 0x17); /* 0x2040f98 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][6], 0xc); /* 0x2040e18 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][7], 0xa); /* 0x2040f9c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][7], 0xc); /* 0x2040e1c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][3], 0xd); /* 0x2040f2c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][0], 0x4); /* 0x2040d60 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][1], 0x0); /* 0x2040d64 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][2], 0x3); /* 0x2040d68 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][3], 0x1); /* 0x2040d6c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][4], 0x7); /* 0x2040d70 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][5], 0x7); /* 0x2040d74 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][6], 0x6); /* 0x2040d78 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][7], 0x5); /* 0x2040d7c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][6], 0x18); /* 0x2040fd8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][6], 0xf); /* 0x2040e58 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][7], 0x4); /* 0x2040fdc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][7], 0xc); /* 0x2040e5c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][4], 0x2); /* 0x2040f10 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][0], 0x2); /* 0x2040c80 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][1], 0x0); /* 0x2040c84 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][2], 0x3); /* 0x2040c88 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][3], 0x0); /* 0x2040c8c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][4], 0x7); /* 0x2040c90 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][5], 0x5); /* 0x2040c94 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][6], 0x8); /* 0x2040c98 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][7], 0x5); /* 0x2040c9c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][8], 0x5); /* 0x2040fa0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][8], 0xf); /* 0x2040e20 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][9], 0x6); /* 0x2040fa4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][9], 0xc); /* 0x2040e24 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][4], 0x8); /* 0x2040f30 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][0], 0x4); /* 0x2040d80 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][1], 0x0); /* 0x2040d84 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][2], 0x4); /* 0x2040d88 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][3], 0x1); /* 0x2040d8c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][4], 0x6); /* 0x2040d90 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][5], 0xa); /* 0x2040d94 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][6], 0x6); /* 0x2040d98 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][7], 0x6); /* 0x2040d9c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][8], 0x12); /* 0x2040fe0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][8], 0xd); /* 0x2040e60 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][9], 0x4); /* 0x2040fe4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][9], 0xc); /* 0x2040e64 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][5], 0xb); /* 0x2040f14 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][0], 0x1); /* 0x2040ca0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][1], 0x3); /* 0x2040ca4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][2], 0x2); /* 0x2040ca8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][3], 0x5); /* 0x2040cac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][4], 0x5); /* 0x2040cb0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][5], 0x8); /* 0x2040cb4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][6], 0xa); /* 0x2040cb8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][7], 0xa); /* 0x2040cbc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][10], 0x0); /* 0x2040fa8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][10], 0xd); /* 0x2040e28 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][11], 0x2); /* 0x2040fac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][11], 0xc); /* 0x2040e2c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][5], 0x3); /* 0x2040f34 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][0], 0x3); /* 0x2040da0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][1], 0x2); /* 0x2040da4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][2], 0x1); /* 0x2040da8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][3], 0x3); /* 0x2040dac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][4], 0x9); /* 0x2040db0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][5], 0x8); /* 0x2040db4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][6], 0x9); /* 0x2040db8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][7], 0x5); /* 0x2040dbc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][10], 0x16); /* 0x2040fe8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][10], 0xf); /* 0x2040e68 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][11], 0x3); /* 0x2040fec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][11], 0xc); /* 0x2040e6c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][6], 0x4); /* 0x2040f18 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][0], 0x2); /* 0x2040cc0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][1], 0x2); /* 0x2040cc4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][2], 0x1); /* 0x2040cc8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][3], 0x2); /* 0x2040ccc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][4], 0xa); /* 0x2040cd0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][5], 0xa); /* 0x2040cd4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][6], 0x9); /* 0x2040cd8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][7], 0x6); /* 0x2040cdc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][12], 0x3); /* 0x2040fb0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][12], 0xf); /* 0x2040e30 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][13], 0x10); /* 0x2040fb4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][13], 0xc); /* 0x2040e34 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][6], 0x5); /* 0x2040f38 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][0], 0x2); /* 0x2040dc0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][1], 0x2); /* 0x2040dc4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][2], 0x3); /* 0x2040dc8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][3], 0x4); /* 0x2040dcc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][4], 0xa); /* 0x2040dd0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][5], 0xa); /* 0x2040dd4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][6], 0x5); /* 0x2040dd8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][7], 0x6); /* 0x2040ddc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][12], 0x1d); /* 0x2040ff0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][12], 0xd); /* 0x2040e70 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][13], 0x9); /* 0x2040ff4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][13], 0xd); /* 0x2040e74 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][7], 0xa); /* 0x2040f1c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][0], 0x2); /* 0x2040ce0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][1], 0x1); /* 0x2040ce4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][2], 0x5); /* 0x2040ce8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][3], 0x0); /* 0x2040cec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][4], 0x6); /* 0x2040cf0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][5], 0x9); /* 0x2040cf4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][6], 0x9); /* 0x2040cf8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][7], 0x9); /* 0x2040cfc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][14], 0xd); /* 0x2040fb8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][14], 0xc); /* 0x2040e38 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][15], 0x16); /* 0x2040fbc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][15], 0xe); /* 0x2040e3c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][7], 0xa); /* 0x2040f3c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][0], 0x1); /* 0x2040de0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][1], 0x3); /* 0x2040de4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][2], 0x0); /* 0x2040de8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][3], 0x2); /* 0x2040dec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][4], 0x8); /* 0x2040df0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][5], 0x5); /* 0x2040df4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][6], 0x5); /* 0x2040df8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][7], 0x6); /* 0x2040dfc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][14], 0xd); /* 0x2040ff8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][14], 0xd); /* 0x2040e78 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][15], 0x15); /* 0x2040ffc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][15], 0xc); /* 0x2040e7c */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x10); /* 0x2038e58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xd4371); /* 0x2038e40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbd71f); /* 0x2038e44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xef2d3); /* 0x2038e48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xa665e); /* 0x2038e4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_validselect, 0xc0000000); /* 0x2038e5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x11); /* 0x2038e78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x95afd); /* 0x2038e60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xa4f3f); /* 0x2038e64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdfb5c); /* 0x2038e68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x85711); /* 0x2038e6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x2038e7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x16); /* 0x2039e58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9635d); /* 0x2039e40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xdd6de); /* 0x2039e44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0x84793); /* 0x2039e48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xfd2f9); /* 0x2039e4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x2039e5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x10); /* 0x2039e78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xa4e51); /* 0x2039e60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xc5ed5); /* 0x2039e64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xe6f59); /* 0x2039e68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x87fdd); /* 0x2039e6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x2039e7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x13); /* 0x203ae58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xa4e51); /* 0x203ae40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xc5ed5); /* 0x203ae44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xe6f59); /* 0x203ae48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x87fdd); /* 0x203ae4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x203ae5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x15); /* 0x203ae78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x203ae60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x203ae64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x203ae68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x203ae6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x203ae7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x12); /* 0x203be58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xfcb93); /* 0x203be40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xddb51); /* 0x203be44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xcf617); /* 0x203be48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xa7b15); /* 0x203be4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x203be5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x12); /* 0x203be78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xeced4); /* 0x203be60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xe66b7); /* 0x203be64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0x96fd8); /* 0x203be68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x8ff50); /* 0x203be6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x203be7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x13); /* 0x203ce58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xa4b53); /* 0x203ce40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xedf1b); /* 0x203ce44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xf5a15); /* 0x203ce48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xe67f1); /* 0x203ce4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x203ce5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x11); /* 0x203ce78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xd7b32); /* 0x203ce60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0x9eed5); /* 0x203ce64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xff717); /* 0x203ce68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xe4614); /* 0x203ce6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x203ce7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x13); /* 0x203de58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x973d3); /* 0x203de40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0x8577f); /* 0x203de44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xbe7b1); /* 0x203de48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xa5b58); /* 0x203de4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x203de5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x12); /* 0x203de78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x957b0); /* 0x203de60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xccf91); /* 0x203de64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xf6adf); /* 0x203de68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xde2f4); /* 0x203de6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x203de7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x17); /* 0x203ee58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x8ffb5); /* 0x203ee40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbced9); /* 0x203ee44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xf4392); /* 0x203ee48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xde354); /* 0x203ee4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x203ee5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x17); /* 0x203ee78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xb6e57); /* 0x203ee60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xa47d9); /* 0x203ee64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xaeb98); /* 0x203ee68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xecff0); /* 0x203ee6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x203ee7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x14); /* 0x203fe58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xa4e51); /* 0x203fe40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xc5ed5); /* 0x203fe44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xe6f59); /* 0x203fe48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x87fdd); /* 0x203fe4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x203fe5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x10); /* 0x203fe78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9c797); /* 0x203fe60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xaebb8); /* 0x203fe64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xfcade); /* 0x203fe68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xa4379); /* 0x203fe6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x203fe7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0x8); /* 0x2038f90 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0x9); /* 0x2038f94 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_mem_hashadr_xbar_ctl[0], 0x10); /* 0x2038fc0 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_bank_enable[0], 0x1000000); /* 0x2038f00 */
    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xe); /* 0x2039f90 */
    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0x8); /* 0x2039f94 */
    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xb); /* 0x203af90 */
    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0xd); /* 0x203af94 */
    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xa); /* 0x203bf90 */
    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0xa); /* 0x203bf94 */
    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xb); /* 0x203cf90 */
    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0x9); /* 0x203cf94 */
    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xb); /* 0x203df90 */
    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0xa); /* 0x203df94 */
    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xf); /* 0x203ef90 */
    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0xf); /* 0x203ef94 */
    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xc); /* 0x203ff90 */
    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0x8); /* 0x203ff94 */
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x14); // ADDED EMVH070915
    tu.IndirectWrite(0x02008000880e, 0x808b0cf856223852, 0xdb2e75ae2e6792d4); /* pipe0.stage0.mau_cfg.mau_array_cfg.sram_ 2_ 2: a=0x2008000880e d0=0x808b0cf856223852 d1=0xdb2e75ae2e6792d4 */
  //    tu.OutWord(&mau_reg_map.rams.match.merge.exact_match_delay_config, 0x0); /* 0x2026cf8 */ // REMOVED EMDEL070915
    tu.OutWord(&mau_reg_map.rams.match.merge.col[0].row_action_nxtable_bus_drive[0], 0x1); /* 0x2026000 */
    tu.OutWord(&mau_reg_map.rams.match.merge.match_to_logical_table_ixbar_outputmap[0][0], 0x1b); /* 0x2027d00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_miss_value[11], 0x1); /* 0x2024dec */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_miss_value[11], 0xe); /* 0x2024e6c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_miss_value[11], 0x123); /* 0x2024eec */
    tu.OutWord(&mau_reg_map.rams.match.merge.predication_ctl[0], 0x4010800); /* 0x2026cb0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.predication_ctl[1], 0x0); /* 0x2026cb8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.next_table_format_data[11], 0x5a); /* 0x2024cec */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_miss_value[11], 0xacedbaba); /* 0x2024d6c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_miss_value[11], 0x2b); /* 0x2024dec */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_miss_value[11], 0x3dbabe); /* 0x2024e6c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_miss_value[11], 0xbbebe); /* 0x2024eec */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_miss_value[11], 0x101cab); /* 0x2024f6c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_miss_value[11], 0x1dead); /* 0x2024fec */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_exact_shiftcount[0][2], 0x40); /* 0x2027008 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_exact_shiftcount[0][2], 0x5); /* 0x2027208 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_exact_shiftcount[0][2], 0x10); /* 0x2027408 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_exact_shiftcount[0][2], 0x40); /* 0x2027608 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_exact_shiftcount[0][2], 0x13); /* 0x2027808 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_exact_shiftcount[0][3], 0x40); /* 0x202700c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_exact_shiftcount[0][3], 0x23); /* 0x202720c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_exact_shiftcount[0][3], 0x2e); /* 0x202740c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_exact_shiftcount[0][3], 0x40); /* 0x202760c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_exact_shiftcount[0][3], 0x31); /* 0x202780c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_mask[0][0], 0x0); /* 0x2027e00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_default[0][0], 0x0); /* 0x2027e80 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_mask[0][0], 0x0); /* 0x2027f80 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_default[0][0], 0x0); /* 0x2024000 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_vpn_shiftcount[0][0], 0x0); /* 0x2024080 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_mask[0][0], 0xffff); /* 0x2024100 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_default[0][0], 0x0); /* 0x2024180 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_mask[0][0], 0x7); /* 0x2024280 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_default[0][0], 0x0); /* 0x2024300 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_mask[0][0], 0x0); /* 0x2024500 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_default[0][0], 0x0); /* 0x2024580 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_mask[0][0], 0x7ff); /* 0x2024700 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_default[0][0], 0x0); /* 0x2024780 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); /* 0x2008f00 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.ram_address_mux_ctl[0][2], 0x40); /* 0x200af10 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.immediate_data_32b_ixbar_ctl[11], 0x20); /* 0x202012c */
  ActionHvTranslator act_hv_translator;
  //    tu.OutWord(&mau_reg_map.rams.array.row[2].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x1); /* 0x203a880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(2,0,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[2].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x0); /* 0x203a888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(2,0,0x0); // ADDED ACHV070915
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[2].ctl.r_l_action_o_mux_select, 0x10); /* 0x2030280 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[2].ctl.r_action_o_mux_select, 0x1); /* 0x20302a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[0][0], RM_B4_16(0x3deea1b)); /* 0x2078000 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[1][0], RM_B4_16(0x2072a2b)); /* 0x2078080 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[2][0], RM_B4_16(0x5a6a00)); /* 0x2078100 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[3][0], RM_B4_16(0x4292a32)); /* 0x2078180 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[4][0], RM_B4_16(0x5907a2c)); /* 0x2078200 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[5][0], RM_B4_16(0x266aa3a)); /* 0x2078280 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[6][0], RM_B4_16(0x6b9ea1a)); /* 0x2078300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[7][0], RM_B4_16(0x6c22a07)); /* 0x2078380 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[8][0], RM_B4_16(0x6daea07)); /* 0x2078400 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[9][0], RM_B4_16(0x2032a2c)); /* 0x2078480 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[10][0], RM_B4_16(0x2fdca3d)); /* 0x2078500 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[11][0], RM_B4_16(0x6132a08)); /* 0x2078580 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[12][0], RM_B4_16(0x70eea06)); /* 0x2078600 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[13][0], RM_B4_16(0x7f42a12)); /* 0x2078680 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[14][0], RM_B4_16(0x38eea0c)); /* 0x2078700 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[15][0], RM_B4_16(0x3658c84)); /* 0x2078780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[16][0], RM_B4_16(0x69fca23)); /* 0x2078800 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[17][0], RM_B4_16(0x2f22a3a)); /* 0x2078880 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[18][0], RM_B4_16(0x5b26a27)); /* 0x2078900 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[19][0], RM_B4_16(0xa62a0d)); /* 0x2078980 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[20][0], RM_B4_16(0x108ea1a)); /* 0x2078a00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[21][0], RM_B4_16(0x5cc2a26)); /* 0x2078a80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[22][0], RM_B4_16(0x6d61a2f)); /* 0x2078b00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[23][0], RM_B4_16(0x3632a21)); /* 0x2078b80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[24][0], RM_B4_16(0x77c8dd0)); /* 0x2078c00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[25][0], RM_B4_16(0x6552a27)); /* 0x2078c80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[26][0], RM_B4_16(0x2626a25)); /* 0x2078d00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[27][0], RM_B4_16(0x21f6a34)); /* 0x2078d80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[28][0], RM_B4_16(0x6316a33)); /* 0x2078e00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[29][0], RM_B4_16(0x2182a25)); /* 0x2078e80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[30][0], RM_B4_16(0x7134a00)); /* 0x2078f00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[31][0], RM_B4_16(0x3a42a23)); /* 0x2078f80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[32][0], RM_B4_16(0x158ea19)); /* 0x2079000 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[33][0], RM_B4_16(0x4d52a19)); /* 0x2079080 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[34][0], RM_B4_16(0x1c8ea16)); /* 0x2079100 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[35][0], RM_B4_16(0x4672a0e)); /* 0x2079180 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[36][0], RM_B4_16(0x4ea4a1f)); /* 0x2079200 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[37][0], RM_B4_16(0x5732a05)); /* 0x2079280 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[38][0], RM_B4_16(0x157ea3d)); /* 0x2079300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[39][0], RM_B4_16(0x1dd2a06)); /* 0x2079380 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[40][0], RM_B4_16(0x46e6a1e)); /* 0x2079400 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[41][0], RM_B4_16(0x5a46a1c)); /* 0x2079480 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[42][0], RM_B4_16(0x2076a34)); /* 0x2079500 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[43][0], RM_B4_16(0x7082a0a)); /* 0x2079580 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[44][0], RM_B4_16(0x11d3a10)); /* 0x2079600 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[45][0], RM_B4_16(0x11c2a10)); /* 0x2079680 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[46][0], RM_B4_16(0x690ea17)); /* 0x2079700 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[47][0], RM_B4_16(0x7022a30)); /* 0x2079780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[48][0], RM_B4_16(0x508ea09)); /* 0x2079800 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[49][0], RM_B4_16(0x6f0aa15)); /* 0x2079880 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[50][0], RM_B4_16(0x647ea33)); /* 0x2079900 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[51][0], RM_B4_16(0x7912a33)); /* 0x2079980 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[52][0], RM_B4_16(0x7bf7a28)); /* 0x2079a00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[53][0], RM_B4_16(0x57eea04)); /* 0x2079a80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[54][0], RM_B4_16(0x17c6a06)); /* 0x2079b00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[55][0], RM_B4_16(0x19fea1e)); /* 0x2079b80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[56][0], RM_B4_16(0x4a55a37)); /* 0x2079c00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[57][0], RM_B4_16(0x4213a3b)); /* 0x2079c80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[58][0], RM_B4_16(0xb3ea35)); /* 0x2079d00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[59][0], RM_B4_16(0x222a0d)); /* 0x2079d80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[60][0], RM_B4_16(0x26c860b)); /* 0x2079e00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[61][0], RM_B4_16(0x3412a1b)); /* 0x2079e80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[62][0], RM_B4_16(0x5804a00)); /* 0x2079f00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[63][0], RM_B4_16(0xf5aa3f)); /* 0x2079f80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[64][0], RM_B4_16(0x1d6a3c)); /* 0x207a000 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[65][0], RM_B4_16(0x60b6a06)); /* 0x207a080 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[66][0], RM_B4_16(0x7644a18)); /* 0x207a100 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[67][0], RM_B4_16(0x7752a27)); /* 0x207a180 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[68][0], RM_B4_16(0x409ea2e)); /* 0x207a200 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[69][0], RM_B4_16(0x3b4ea1b)); /* 0x207a280 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[70][0], RM_B4_16(0x20bda03)); /* 0x207a300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[71][0], RM_B4_16(0xd6ea09)); /* 0x207a380 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[72][0], RM_B4_16(0x507ea3c)); /* 0x207a400 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[73][0], RM_B4_16(0x4c42a2a)); /* 0x207a480 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[74][0], RM_B4_16(0x6196a14)); /* 0x207a500 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[75][0], RM_B4_16(0x7282a34)); /* 0x207a580 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[76][0], RM_B4_16(0xc5aa2e)); /* 0x207a600 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[77][0], RM_B4_16(0xa32a0b)); /* 0x207a680 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[78][0], RM_B4_16(0x76aca12)); /* 0x207a700 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[79][0], RM_B4_16(0x26d2a1e)); /* 0x207a780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[80][0], RM_B4_16(0x1a6e382)); /* 0x207a800 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[81][0], RM_B4_16(0x409ea2b)); /* 0x207a880 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[82][0], RM_B4_16(0x71a2d)); /* 0x207a900 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[83][0], RM_B4_16(0x34c6a2e)); /* 0x207a980 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[84][0], RM_B4_16(0x701ea06)); /* 0x207aa00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[85][0], RM_B4_16(0x2a42a22)); /* 0x207aa80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[86][0], RM_B4_16(0x657ea20)); /* 0x207ab00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[87][0], RM_B4_16(0x6c12a10)); /* 0x207ab80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[88][0], RM_B4_16(0x600aa2c)); /* 0x207ac00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[89][0], RM_B4_16(0x7802a05)); /* 0x207ac80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[90][0], RM_B4_16(0x484aa31)); /* 0x207ad00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[91][0], RM_B4_16(0x4822a11)); /* 0x207ad80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[92][0], RM_B4_16(0x4b44a11)); /* 0x207ae00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[93][0], RM_B4_16(0x16a2a35)); /* 0x207ae80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[94][0], RM_B4_16(0x425ea2f)); /* 0x207af00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[95][0], RM_B4_16(0x5602a1c)); /* 0x207af80 */
    tu.OutWord(&mau_reg_map.dp.imem_parity_ctl, 0x2); /* 0x2060044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][0], 0x72a8); /* 0x2074000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][1], 0x33696); /* 0x2074004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][2], 0x20928); /* 0x2074008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][3], 0x37381); /* 0x207400c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][4], 0x2dbf3); /* 0x2074010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][5], 0x2852d); /* 0x2074014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][6], 0x3b7fe); /* 0x2074018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][7], 0x3b633); /* 0x207401c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][8], 0x1317c); /* 0x2074020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][9], 0x4452); /* 0x2074024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][10], 0x7daa); /* 0x2074028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][11], 0x6512); /* 0x207402c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][12], 0x26a33); /* 0x2074030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][13], 0x5172); /* 0x2074034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][14], 0x12daf); /* 0x2074038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][15], 0x3d8fc); /* 0x207403c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][16], 0x24c3f); /* 0x2074040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][17], 0x34e96); /* 0x2074044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][18], 0x16031); /* 0x2074048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][19], 0x1cc1b); /* 0x207404c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][20], 0x163dd); /* 0x2074050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][21], 0x27256); /* 0x2074054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][22], 0x2ed8a); /* 0x2074058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][23], 0xd4e7); /* 0x207405c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][24], 0xbfc8); /* 0x2074060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][25], 0x1b2b1); /* 0x2074064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][26], 0x2a222); /* 0x2074068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][27], 0x1b823); /* 0x207406c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][28], 0x1cce7); /* 0x2074070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][29], 0x52bb); /* 0x2074074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][30], 0x129f3); /* 0x2074078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][31], 0x1f04c); /* 0x207407c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][32], 0x6ed); /* 0x2074080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][33], 0x1d0f3); /* 0x2074084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][34], 0x28aba); /* 0x2074088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][35], 0x1f209); /* 0x207408c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][36], 0x2a3be); /* 0x2074090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][37], 0x19313); /* 0x2074094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][38], 0x113ed); /* 0x2074098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][39], 0x2d60b); /* 0x207409c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][40], 0x242b6); /* 0x20740a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][41], 0x1e03c); /* 0x20740a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][42], 0x11054); /* 0x20740a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][43], 0xf02f); /* 0x20740ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][44], 0x19d50); /* 0x20740b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][45], 0x25c6e); /* 0x20740b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][46], 0x37800); /* 0x20740b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][47], 0x1b3ba); /* 0x20740bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][48], 0x29fc); /* 0x20740c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][49], 0x145b7); /* 0x20740c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][50], 0x2d822); /* 0x20740c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][51], 0x21430); /* 0x20740cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][0], 0x1cd79); /* 0x2074100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][1], 0x29fb8); /* 0x2074104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][2], 0x17da2); /* 0x2074108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][3], 0xcb9d); /* 0x207410c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][4], 0x1e3c5); /* 0x2074110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][5], 0x26b86); /* 0x2074114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][6], 0x2a412); /* 0x2074118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][7], 0x1727c); /* 0x207411c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][8], 0x3df33); /* 0x2074120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][9], 0x112ae); /* 0x2074124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][10], 0x5f95); /* 0x2074128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][11], 0x31362); /* 0x207412c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][12], 0x50ff); /* 0x2074130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][13], 0x8190); /* 0x2074134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][14], 0x30045); /* 0x2074138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][15], 0x332ae); /* 0x207413c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][16], 0x1e269); /* 0x2074140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][17], 0x3de08); /* 0x2074144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][18], 0x776b); /* 0x2074148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][19], 0x3a75c); /* 0x207414c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][20], 0x1fde4); /* 0x2074150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][21], 0x4082); /* 0x2074154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][22], 0x3ef6f); /* 0x2074158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][23], 0x2cccf); /* 0x207415c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][24], 0x15d8e); /* 0x2074160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][25], 0xe6f0); /* 0x2074164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][26], 0x3dbfb); /* 0x2074168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][27], 0x1b95f); /* 0x207416c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][28], 0x2020d); /* 0x2074170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][29], 0xe9c1); /* 0x2074174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][30], 0x3ca84); /* 0x2074178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][31], 0x1597c); /* 0x207417c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][32], 0x354c1); /* 0x2074180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][33], 0x37871); /* 0x2074184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][34], 0x2fbc4); /* 0x2074188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][35], 0x1fde6); /* 0x207418c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][36], 0x13065); /* 0x2074190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][37], 0x318f3); /* 0x2074194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][38], 0x39325); /* 0x2074198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][39], 0x34a0a); /* 0x207419c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][40], 0x25596); /* 0x20741a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][41], 0x712f); /* 0x20741a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][42], 0x7190); /* 0x20741a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][43], 0x23707); /* 0x20741ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][44], 0x2ddf3); /* 0x20741b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][45], 0x270cc); /* 0x20741b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][46], 0x3246); /* 0x20741b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][47], 0x1964d); /* 0x20741bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][48], 0x1189c); /* 0x20741c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][49], 0x2e3c6); /* 0x20741c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][50], 0x2156c); /* 0x20741c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][51], 0x2c994); /* 0x20741cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][0], 0x1d9e6); /* 0x2074200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][1], 0x3fa41); /* 0x2074204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][2], 0x239d8); /* 0x2074208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][3], 0x33702); /* 0x207420c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][4], 0x1c52b); /* 0x2074210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][5], 0x141a1); /* 0x2074214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][6], 0x36e55); /* 0x2074218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][7], 0x36b6f); /* 0x207421c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][8], 0x1f456); /* 0x2074220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][9], 0x211db); /* 0x2074224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][10], 0x33d3a); /* 0x2074228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][11], 0x249e2); /* 0x207422c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][12], 0x3ce1d); /* 0x2074230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][13], 0x3825e); /* 0x2074234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][14], 0x3214c); /* 0x2074238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][15], 0x171ed); /* 0x207423c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][16], 0x26807); /* 0x2074240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][17], 0x27901); /* 0x2074244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][18], 0x4a59); /* 0x2074248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][19], 0xde61); /* 0x207424c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][20], 0x13cd9); /* 0x2074250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][21], 0x15e46); /* 0x2074254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][22], 0x1f7b6); /* 0x2074258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][23], 0x3150e); /* 0x207425c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][24], 0x3ad27); /* 0x2074260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][25], 0x148); /* 0x2074264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][26], 0x13071); /* 0x2074268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][27], 0x2d47b); /* 0x207426c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][28], 0x3c414); /* 0x2074270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][29], 0xe96c); /* 0x2074274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][30], 0x2c9d); /* 0x2074278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][31], 0x1af07); /* 0x207427c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][32], 0x3ba53); /* 0x2074280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][33], 0x32283); /* 0x2074284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][34], 0x16101); /* 0x2074288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][35], 0xbda5); /* 0x207428c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][36], 0x222bd); /* 0x2074290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][37], 0x27b92); /* 0x2074294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][38], 0x34d3b); /* 0x2074298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][39], 0x2e1e7); /* 0x207429c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][40], 0x170e0); /* 0x20742a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][41], 0x15761); /* 0x20742a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][42], 0x34978); /* 0x20742a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][43], 0x29bc3); /* 0x20742ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][44], 0x71d5); /* 0x20742b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][45], 0x2bb0f); /* 0x20742b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][46], 0x207a8); /* 0x20742b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][47], 0xa854); /* 0x20742bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][48], 0x26724); /* 0x20742c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][49], 0x35d92); /* 0x20742c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][50], 0x1ef3d); /* 0x20742c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][51], 0x7c17); /* 0x20742cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][0], 0x322ba); /* 0x2074300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][1], 0x2c3ee); /* 0x2074304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][2], 0x29229); /* 0x2074308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][3], 0xf250); /* 0x207430c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][4], 0x3a40c); /* 0x2074310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][5], 0x7e4b); /* 0x2074314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][6], 0xd297); /* 0x2074318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][7], 0x2c866); /* 0x207431c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][8], 0x10ce7); /* 0x2074320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][9], 0x25f0c); /* 0x2074324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][10], 0x1f666); /* 0x2074328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][11], 0x1d708); /* 0x207432c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][12], 0x33bd); /* 0x2074330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][13], 0xcbc); /* 0x2074334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][14], 0x4d81); /* 0x2074338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][15], 0x19fcf); /* 0x207433c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][16], 0x14b85); /* 0x2074340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][17], 0xbcb2); /* 0x2074344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][18], 0x3db8a); /* 0x2074348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][19], 0x367a3); /* 0x207434c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][20], 0x2d566); /* 0x2074350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][21], 0x1c29e); /* 0x2074354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][22], 0x3e640); /* 0x2074358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][23], 0x3a426); /* 0x207435c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][24], 0x33f14); /* 0x2074360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][25], 0x668c); /* 0x2074364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][26], 0x19025); /* 0x2074368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][27], 0x30ab8); /* 0x207436c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][28], 0x3c8b8); /* 0x2074370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][29], 0x4f22); /* 0x2074374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][30], 0x3cff7); /* 0x2074378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][31], 0x18fb6); /* 0x207437c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][32], 0x110cb); /* 0x2074380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][33], 0x20eb2); /* 0x2074384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][34], 0x18632); /* 0x2074388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][35], 0x2b0fa); /* 0x207438c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][36], 0x2989b); /* 0x2074390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][37], 0x34ed8); /* 0x2074394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][38], 0xaf7b); /* 0x2074398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][39], 0x23f98); /* 0x207439c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][40], 0xb5b1); /* 0x20743a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][41], 0x2ddfe); /* 0x20743a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][42], 0x20681); /* 0x20743a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][43], 0x2f22c); /* 0x20743ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][44], 0x29896); /* 0x20743b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][45], 0x4726); /* 0x20743b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][46], 0x25ab3); /* 0x20743b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][47], 0x1b23d); /* 0x20743bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][48], 0x32560); /* 0x20743c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][49], 0x34061); /* 0x20743c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][50], 0x10c3c); /* 0x20743c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][51], 0x200ce); /* 0x20743cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][0], 0x14607); /* 0x2074400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][1], 0x29181); /* 0x2074404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][2], 0xa3ed); /* 0x2074408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][3], 0x249ea); /* 0x207440c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][4], 0x14caa); /* 0x2074410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][5], 0x240cc); /* 0x2074414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][6], 0x16ac3); /* 0x2074418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][7], 0x9a38); /* 0x207441c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][8], 0x2f972); /* 0x2074420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][9], 0x3ca5e); /* 0x2074424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][10], 0x2d0d6); /* 0x2074428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][11], 0x1ac7); /* 0x207442c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][12], 0x25918); /* 0x2074430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][13], 0xbe44); /* 0x2074434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][14], 0x1ada9); /* 0x2074438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][15], 0x120b9); /* 0x207443c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][16], 0x21d5f); /* 0x2074440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][17], 0x2e136); /* 0x2074444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][18], 0x35a23); /* 0x2074448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][19], 0xdca4); /* 0x207444c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][20], 0x304eb); /* 0x2074450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][21], 0x3c4a2); /* 0x2074454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][22], 0xa1c); /* 0x2074458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][23], 0x34cd2); /* 0x207445c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][24], 0x3c363); /* 0x2074460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][25], 0x36500); /* 0x2074464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][26], 0x35fcb); /* 0x2074468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][27], 0x38bd5); /* 0x207446c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][28], 0x3a96d); /* 0x2074470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][29], 0x5226); /* 0x2074474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][30], 0x118ca); /* 0x2074478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][31], 0x2a604); /* 0x207447c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][32], 0x6879); /* 0x2074480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][33], 0x1f6e2); /* 0x2074484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][34], 0x29481); /* 0x2074488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][35], 0x24e21); /* 0x207448c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][36], 0x16808); /* 0x2074490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][37], 0x2895b); /* 0x2074494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][38], 0x3be83); /* 0x2074498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][39], 0x1ae82); /* 0x207449c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][40], 0x1c915); /* 0x20744a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][41], 0x45dd); /* 0x20744a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][42], 0x28895); /* 0x20744a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][43], 0x472c); /* 0x20744ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][44], 0x24961); /* 0x20744b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][45], 0x1ec8f); /* 0x20744b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][46], 0x39189); /* 0x20744b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][47], 0x3509d); /* 0x20744bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][48], 0x39cc8); /* 0x20744c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][49], 0x4a5f); /* 0x20744c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][50], 0x38882); /* 0x20744c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][51], 0x395b7); /* 0x20744cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][0], 0x10faa); /* 0x2074500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][1], 0x27cfc); /* 0x2074504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][2], 0x2b8ae); /* 0x2074508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][3], 0x35f62); /* 0x207450c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][4], 0xfbb3); /* 0x2074510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][5], 0x14fad); /* 0x2074514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][6], 0x2e08e); /* 0x2074518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][7], 0x26388); /* 0x207451c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][8], 0x2e04d); /* 0x2074520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][9], 0x1d214); /* 0x2074524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][10], 0x2c096); /* 0x2074528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][11], 0x5d8f); /* 0x207452c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][12], 0x1cf6e); /* 0x2074530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][13], 0xef8a); /* 0x2074534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][14], 0xaccc); /* 0x2074538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][15], 0x182ea); /* 0x207453c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][16], 0x20188); /* 0x2074540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][17], 0x1accc); /* 0x2074544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][18], 0xb22d); /* 0x2074548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][19], 0x36714); /* 0x207454c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][20], 0x3822b); /* 0x2074550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][21], 0x3cb1f); /* 0x2074554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][22], 0x3f133); /* 0x2074558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][23], 0x15204); /* 0x207455c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][24], 0xe17c); /* 0x2074560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][25], 0x198a8); /* 0x2074564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][26], 0x39b72); /* 0x2074568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][27], 0x38432); /* 0x207456c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][28], 0x2bf66); /* 0x2074570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][29], 0x2fcd); /* 0x2074574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][30], 0x2393a); /* 0x2074578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][31], 0x3d243); /* 0x207457c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][32], 0x264ac); /* 0x2074580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][33], 0x1eacb); /* 0x2074584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][34], 0x19950); /* 0x2074588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][35], 0x3b7b3); /* 0x207458c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][36], 0x19078); /* 0x2074590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][37], 0x39294); /* 0x2074594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][38], 0x35fcf); /* 0x2074598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][39], 0x24e69); /* 0x207459c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][40], 0x18902); /* 0x20745a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][41], 0x121e7); /* 0x20745a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][42], 0x1cc5b); /* 0x20745a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][43], 0x3685b); /* 0x20745ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][44], 0x14004); /* 0x20745b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][45], 0x3c44c); /* 0x20745b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][46], 0x3181d); /* 0x20745b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][47], 0x558c); /* 0x20745bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][48], 0x1ae42); /* 0x20745c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][49], 0x2cff1); /* 0x20745c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][50], 0x157b9); /* 0x20745c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][51], 0x71f); /* 0x20745cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][0], 0xbfca); /* 0x2074600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][1], 0x25e2c); /* 0x2074604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][2], 0x442f); /* 0x2074608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][3], 0x30386); /* 0x207460c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][4], 0x1c37c); /* 0x2074610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][5], 0x12d08); /* 0x2074614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][6], 0x224a7); /* 0x2074618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][7], 0x37d99); /* 0x207461c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][8], 0x903d); /* 0x2074620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][9], 0x3542e); /* 0x2074624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][10], 0x29c15); /* 0x2074628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][11], 0x2d78d); /* 0x207462c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][12], 0x1c616); /* 0x2074630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][13], 0x307c5); /* 0x2074634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][14], 0x2509b); /* 0x2074638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][15], 0x2ac70); /* 0x207463c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][16], 0x15329); /* 0x2074640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][17], 0x36b58); /* 0x2074644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][18], 0x267fc); /* 0x2074648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][19], 0xaf03); /* 0x207464c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][20], 0x2e73a); /* 0x2074650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][21], 0x10584); /* 0x2074654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][22], 0x19f3f); /* 0x2074658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][23], 0x12238); /* 0x207465c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][24], 0x10139); /* 0x2074660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][25], 0xe6b8); /* 0x2074664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][26], 0x114fd); /* 0x2074668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][27], 0x1c2e4); /* 0x207466c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][28], 0x1419d); /* 0x2074670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][29], 0x83f9); /* 0x2074674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][30], 0x325a7); /* 0x2074678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][31], 0x6206); /* 0x207467c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][32], 0x19b9c); /* 0x2074680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][33], 0x319c2); /* 0x2074684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][34], 0x1166b); /* 0x2074688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][35], 0xeee1); /* 0x207468c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][36], 0x19484); /* 0x2074690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][37], 0x3292b); /* 0x2074694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][38], 0x8701); /* 0x2074698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][39], 0x1e3a3); /* 0x207469c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][40], 0x38ada); /* 0x20746a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][41], 0xecd7); /* 0x20746a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][42], 0x7a6); /* 0x20746a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][43], 0x31d5f); /* 0x20746ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][44], 0x3c8ff); /* 0x20746b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][45], 0x3f562); /* 0x20746b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][46], 0x336a4); /* 0x20746b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][47], 0x22501); /* 0x20746bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][48], 0x2c552); /* 0x20746c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][49], 0x16593); /* 0x20746c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][50], 0x37637); /* 0x20746c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][51], 0x1d390); /* 0x20746cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][0], 0x13a3a); /* 0x2074700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][1], 0xd9d9); /* 0x2074704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][2], 0x1c3a1); /* 0x2074708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][3], 0x32150); /* 0x207470c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][4], 0x8090); /* 0x2074710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][5], 0x2d71c); /* 0x2074714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][6], 0x3f1ee); /* 0x2074718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][7], 0x37154); /* 0x207471c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][8], 0x2905f); /* 0x2074720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][9], 0x21170); /* 0x2074724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][10], 0x1b06); /* 0x2074728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][11], 0x17a3e); /* 0x207472c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][12], 0xce9b); /* 0x2074730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][13], 0xca3c); /* 0x2074734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][14], 0x20d5e); /* 0x2074738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][15], 0xd4a8); /* 0x207473c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][16], 0x2ec77); /* 0x2074740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][17], 0x1472); /* 0x2074744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][18], 0x3619a); /* 0x2074748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][19], 0x2697); /* 0x207474c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][20], 0x15a93); /* 0x2074750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][21], 0x2296f); /* 0x2074754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][22], 0x25846); /* 0x2074758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][23], 0x2504b); /* 0x207475c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][24], 0x1d7e6); /* 0x2074760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][25], 0x3c86); /* 0x2074764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][26], 0x24bf5); /* 0x2074768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][27], 0x238c6); /* 0x207476c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][28], 0x80c4); /* 0x2074770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][29], 0x3a1e0); /* 0x2074774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][30], 0x38cda); /* 0x2074778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][31], 0x3c2ae); /* 0x207477c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][32], 0xaba7); /* 0x2074780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][33], 0x3a255); /* 0x2074784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][34], 0x39cb4); /* 0x2074788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][35], 0xe0ce); /* 0x207478c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][36], 0x2b6f1); /* 0x2074790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][37], 0x1874e); /* 0x2074794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][38], 0x8bf6); /* 0x2074798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][39], 0x2cbe8); /* 0x207479c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][40], 0x28464); /* 0x20747a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][41], 0x36e7b); /* 0x20747a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][42], 0x195eb); /* 0x20747a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][43], 0x367fd); /* 0x20747ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][44], 0x765a); /* 0x20747b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][45], 0x35524); /* 0x20747b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][46], 0x3b033); /* 0x20747b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][47], 0x21af4); /* 0x20747bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][48], 0x3372f); /* 0x20747c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][49], 0x8002); /* 0x20747c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][50], 0x3d203); /* 0x20747c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][51], 0x37c99); /* 0x20747cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][0], 0x20c4b); /* 0x2074800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][1], 0x11936); /* 0x2074804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][2], 0x3c020); /* 0x2074808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][3], 0x32273); /* 0x207480c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][4], 0x103a2); /* 0x2074810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][5], 0x938); /* 0x2074814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][6], 0x22e47); /* 0x2074818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][7], 0x5b24); /* 0x207481c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][8], 0x25b88); /* 0x2074820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][9], 0x22697); /* 0x2074824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][10], 0x2f4be); /* 0x2074828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][11], 0x2505e); /* 0x207482c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][12], 0x10e9a); /* 0x2074830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][13], 0x1dbcd); /* 0x2074834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][14], 0x38019); /* 0x2074838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][15], 0x2e12d); /* 0x207483c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][16], 0x2a748); /* 0x2074840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][17], 0x156b); /* 0x2074844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][18], 0x9583); /* 0x2074848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][19], 0x26ebf); /* 0x207484c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][20], 0x2df39); /* 0x2074850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][21], 0x25092); /* 0x2074854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][22], 0x3fff4); /* 0x2074858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][23], 0x24e3e); /* 0x207485c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][24], 0x1cd8d); /* 0x2074860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][25], 0x199b6); /* 0x2074864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][26], 0x1679e); /* 0x2074868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][27], 0x159ab); /* 0x207486c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][28], 0x7f4e); /* 0x2074870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][29], 0x843e); /* 0x2074874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][30], 0x38946); /* 0x2074878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][31], 0x3c165); /* 0x207487c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][32], 0x37fcf); /* 0x2074880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][33], 0x2784); /* 0x2074884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][34], 0x31a97); /* 0x2074888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][35], 0x34e70); /* 0x207488c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][36], 0x2db28); /* 0x2074890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][37], 0x16aad); /* 0x2074894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][38], 0x1885); /* 0x2074898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][39], 0x2e178); /* 0x207489c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][40], 0x18c45); /* 0x20748a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][41], 0x3fb04); /* 0x20748a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][42], 0x33c6e); /* 0x20748a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][43], 0x7a35); /* 0x20748ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][44], 0x2825f); /* 0x20748b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][45], 0xbfbd); /* 0x20748b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][46], 0x3bb9a); /* 0x20748b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][47], 0x1a5b8); /* 0x20748bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][48], 0x26049); /* 0x20748c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][49], 0x1a391); /* 0x20748c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][50], 0xd746); /* 0x20748c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][51], 0x390bb); /* 0x20748cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][0], 0xa8c9); /* 0x2074900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][1], 0x1b327); /* 0x2074904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][2], 0xf07); /* 0x2074908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][3], 0xa6f); /* 0x207490c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][4], 0x2f0a5); /* 0x2074910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][5], 0xeb5); /* 0x2074914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][6], 0x3545c); /* 0x2074918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][7], 0x21bf2); /* 0x207491c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][8], 0x3cd5e); /* 0x2074920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][9], 0x46d6); /* 0x2074924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][10], 0xca26); /* 0x2074928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][11], 0x15a7); /* 0x207492c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][12], 0x9049); /* 0x2074930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][13], 0x35e19); /* 0x2074934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][14], 0x3cf3c); /* 0x2074938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][15], 0x3f94b); /* 0x207493c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][16], 0x2d279); /* 0x2074940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][17], 0x1f503); /* 0x2074944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][18], 0xc98e); /* 0x2074948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][19], 0x29764); /* 0x207494c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][20], 0x13f6c); /* 0x2074950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][21], 0x14ddb); /* 0x2074954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][22], 0x2954d); /* 0x2074958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][23], 0x31b61); /* 0x207495c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][24], 0x3c357); /* 0x2074960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][25], 0x2abea); /* 0x2074964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][26], 0x3492f); /* 0x2074968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][27], 0x33490); /* 0x207496c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][28], 0x215); /* 0x2074970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][29], 0x126c8); /* 0x2074974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][30], 0x32565); /* 0x2074978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][31], 0x3ae40); /* 0x207497c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][32], 0x343f0); /* 0x2074980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][33], 0xde20); /* 0x2074984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][34], 0x3c847); /* 0x2074988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][35], 0xf052); /* 0x207498c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][36], 0x31ddd); /* 0x2074990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][37], 0xb591); /* 0x2074994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][38], 0x423e); /* 0x2074998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][39], 0xa214); /* 0x207499c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][40], 0x21ab5); /* 0x20749a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][41], 0x26a62); /* 0x20749a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][42], 0x3b269); /* 0x20749a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][43], 0x29cbc); /* 0x20749ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][44], 0x28c2f); /* 0x20749b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][45], 0x33174); /* 0x20749b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][46], 0xa93d); /* 0x20749b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][47], 0xb94c); /* 0x20749bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][48], 0x24e9); /* 0x20749c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][49], 0x2ee90); /* 0x20749c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][50], 0x32eee); /* 0x20749c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][51], 0x24268); /* 0x20749cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][0], 0x1454a); /* 0x2074a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][1], 0x3d4e7); /* 0x2074a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][2], 0x24b06); /* 0x2074a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][3], 0x10acd); /* 0x2074a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][4], 0x285ac); /* 0x2074a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][5], 0x200b1); /* 0x2074a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][6], 0x3edcf); /* 0x2074a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][7], 0x12cf3); /* 0x2074a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][8], 0x22c31); /* 0x2074a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][9], 0x2f2bb); /* 0x2074a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][10], 0xdba); /* 0x2074a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][11], 0x3d01e); /* 0x2074a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][12], 0x3c4f6); /* 0x2074a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][13], 0x280); /* 0x2074a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][14], 0x1654e); /* 0x2074a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][15], 0x2c158); /* 0x2074a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][16], 0xe46); /* 0x2074a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][17], 0x3170b); /* 0x2074a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][18], 0x150fd); /* 0x2074a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][19], 0x25271); /* 0x2074a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][20], 0x34fa3); /* 0x2074a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][21], 0x1f4a9); /* 0x2074a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][22], 0x22924); /* 0x2074a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][23], 0x2aa79); /* 0x2074a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][24], 0x15773); /* 0x2074a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][25], 0x39d2a); /* 0x2074a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][26], 0x1cac4); /* 0x2074a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][27], 0x3ff2f); /* 0x2074a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][28], 0x96a); /* 0x2074a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][29], 0x760a); /* 0x2074a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][30], 0x604f); /* 0x2074a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][31], 0x3e8e); /* 0x2074a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][32], 0x29548); /* 0x2074a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][33], 0x3bfcd); /* 0x2074a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][34], 0x3758); /* 0x2074a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][35], 0x319e0); /* 0x2074a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][36], 0x221fa); /* 0x2074a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][37], 0x16aa); /* 0x2074a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][38], 0x36172); /* 0x2074a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][39], 0x2e0b3); /* 0x2074a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][40], 0x39e0a); /* 0x2074aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][41], 0x2a8b7); /* 0x2074aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][42], 0x16e7c); /* 0x2074aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][43], 0x19261); /* 0x2074aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][44], 0xd6e2); /* 0x2074ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][45], 0xb351); /* 0x2074ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][46], 0xcd69); /* 0x2074ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][47], 0x3f91b); /* 0x2074abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][48], 0xc924); /* 0x2074ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][49], 0x2f353); /* 0x2074ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][50], 0x148b9); /* 0x2074ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][51], 0x3ded5); /* 0x2074acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][0], 0x1a22e); /* 0x2074b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][1], 0xfad7); /* 0x2074b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][2], 0x3186d); /* 0x2074b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][3], 0x182f1); /* 0x2074b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][4], 0x21ede); /* 0x2074b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][5], 0x328d1); /* 0x2074b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][6], 0x249fe); /* 0x2074b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][7], 0x8bd4); /* 0x2074b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][8], 0x25591); /* 0x2074b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][9], 0x34fa1); /* 0x2074b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][10], 0x2b65b); /* 0x2074b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][11], 0x264e7); /* 0x2074b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][12], 0x7b6d); /* 0x2074b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][13], 0x2058c); /* 0x2074b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][14], 0x13394); /* 0x2074b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][15], 0x9337); /* 0x2074b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][16], 0x1701e); /* 0x2074b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][17], 0x1fdf); /* 0x2074b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][18], 0x1fbd1); /* 0x2074b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][19], 0x1e1f4); /* 0x2074b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][20], 0x3c468); /* 0x2074b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][21], 0x2b779); /* 0x2074b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][22], 0x2146d); /* 0x2074b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][23], 0x34a1f); /* 0x2074b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][24], 0xd618); /* 0x2074b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][25], 0x2246); /* 0x2074b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][26], 0x286a4); /* 0x2074b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][27], 0x1f89); /* 0x2074b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][28], 0x25f3c); /* 0x2074b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][29], 0x3589f); /* 0x2074b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][30], 0x2dbb7); /* 0x2074b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][31], 0xf9fc); /* 0x2074b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][32], 0x21998); /* 0x2074b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][33], 0x3777f); /* 0x2074b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][34], 0x36afb); /* 0x2074b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][35], 0x10414); /* 0x2074b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][36], 0x21712); /* 0x2074b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][37], 0x80a7); /* 0x2074b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][38], 0x1177c); /* 0x2074b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][39], 0x499f); /* 0x2074b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][40], 0xfcd1); /* 0x2074ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][41], 0x10fd5); /* 0x2074ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][42], 0x3aa87); /* 0x2074ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][43], 0xbd6a); /* 0x2074bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][44], 0x1e50f); /* 0x2074bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][45], 0x12d03); /* 0x2074bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][46], 0x22aca); /* 0x2074bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][47], 0x242ed); /* 0x2074bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][48], 0xa929); /* 0x2074bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][49], 0x1c8d7); /* 0x2074bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][50], 0x2fc25); /* 0x2074bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][51], 0xce7e); /* 0x2074bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][0], 0x14ff8); /* 0x2074c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][1], 0x225f2); /* 0x2074c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][2], 0x172ea); /* 0x2074c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][3], 0x10f24); /* 0x2074c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][4], 0x3c2cd); /* 0x2074c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][5], 0x28a5b); /* 0x2074c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][6], 0x3a19d); /* 0x2074c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][7], 0x288f); /* 0x2074c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][8], 0x11387); /* 0x2074c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][9], 0x11a91); /* 0x2074c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][10], 0x372fe); /* 0x2074c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][11], 0x30b17); /* 0x2074c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][12], 0xf55f); /* 0x2074c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][13], 0x1dcfc); /* 0x2074c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][14], 0x1656f); /* 0x2074c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][15], 0x3c65c); /* 0x2074c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][16], 0x29838); /* 0x2074c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][17], 0x149d6); /* 0x2074c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][18], 0x24078); /* 0x2074c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][19], 0x248aa); /* 0x2074c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][20], 0x224b1); /* 0x2074c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][21], 0xd1fa); /* 0x2074c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][22], 0x1ae34); /* 0x2074c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][23], 0xdac3); /* 0x2074c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][24], 0x3e3de); /* 0x2074c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][25], 0xb9d8); /* 0x2074c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][26], 0x33a1e); /* 0x2074c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][27], 0x115d9); /* 0x2074c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][28], 0x32760); /* 0x2074c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][29], 0x2d2fc); /* 0x2074c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][30], 0x14b37); /* 0x2074c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][31], 0x2fab5); /* 0x2074c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][32], 0x5718); /* 0x2074c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][33], 0x83cc); /* 0x2074c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][34], 0x14151); /* 0x2074c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][35], 0x2e4b5); /* 0x2074c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][36], 0x3a339); /* 0x2074c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][37], 0x23b2a); /* 0x2074c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][38], 0x3868a); /* 0x2074c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][39], 0x12341); /* 0x2074c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][40], 0xf0e7); /* 0x2074ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][41], 0x2a7b0); /* 0x2074ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][42], 0x7cf0); /* 0x2074ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][43], 0x10536); /* 0x2074cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][44], 0x1509f); /* 0x2074cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][45], 0x3c13a); /* 0x2074cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][46], 0x30475); /* 0x2074cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][47], 0x14b1e); /* 0x2074cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][48], 0x30c56); /* 0x2074cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][49], 0x380dd); /* 0x2074cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][50], 0x2b357); /* 0x2074cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][51], 0xd4f5); /* 0x2074ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][0], 0x1945a); /* 0x2074d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][1], 0x1e5af); /* 0x2074d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][2], 0x15555); /* 0x2074d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][3], 0x944a); /* 0x2074d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][4], 0x15087); /* 0x2074d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][5], 0x2e7f); /* 0x2074d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][6], 0x19838); /* 0x2074d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][7], 0x146a0); /* 0x2074d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][8], 0x13271); /* 0x2074d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][9], 0x12cde); /* 0x2074d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][10], 0x3ac16); /* 0x2074d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][11], 0x21afd); /* 0x2074d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][12], 0x27ccf); /* 0x2074d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][13], 0xdec0); /* 0x2074d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][14], 0x2ffa5); /* 0x2074d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][15], 0x2ad55); /* 0x2074d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][16], 0x3c30c); /* 0x2074d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][17], 0x2087f); /* 0x2074d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][18], 0x3ba50); /* 0x2074d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][19], 0x32b23); /* 0x2074d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][20], 0x11e14); /* 0x2074d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][21], 0x1884e); /* 0x2074d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][22], 0x17f3d); /* 0x2074d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][23], 0x3ac37); /* 0x2074d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][24], 0x3db03); /* 0x2074d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][25], 0x1c09a); /* 0x2074d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][26], 0x304a6); /* 0x2074d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][27], 0x3c388); /* 0x2074d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][28], 0x22c2c); /* 0x2074d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][29], 0x192cb); /* 0x2074d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][30], 0x2a0c9); /* 0x2074d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][31], 0x12708); /* 0x2074d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][32], 0x1dc1e); /* 0x2074d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][33], 0x32303); /* 0x2074d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][34], 0x2b0b6); /* 0x2074d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][35], 0x1d1d2); /* 0x2074d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][36], 0x2dd00); /* 0x2074d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][37], 0x9672); /* 0x2074d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][38], 0x243ef); /* 0x2074d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][39], 0x201f7); /* 0x2074d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][40], 0x2cd6c); /* 0x2074da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][41], 0x3ddad); /* 0x2074da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][42], 0x18ca8); /* 0x2074da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][43], 0x17532); /* 0x2074dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][44], 0x22638); /* 0x2074db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][45], 0x19979); /* 0x2074db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][46], 0xe6e8); /* 0x2074db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][47], 0x12e9); /* 0x2074dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][48], 0x38b5d); /* 0x2074dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][49], 0x179b8); /* 0x2074dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][50], 0x1906f); /* 0x2074dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][51], 0x24756); /* 0x2074dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][0], 0x11ba); /* 0x2074e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][1], 0xc70e); /* 0x2074e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][2], 0x2f7e4); /* 0x2074e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][3], 0x7683); /* 0x2074e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][4], 0x36b65); /* 0x2074e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][5], 0x56af); /* 0x2074e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][6], 0xd7a7); /* 0x2074e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][7], 0x2f7b1); /* 0x2074e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][8], 0x297be); /* 0x2074e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][9], 0xa518); /* 0x2074e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][10], 0x2bbc); /* 0x2074e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][11], 0x20a92); /* 0x2074e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][12], 0x1f958); /* 0x2074e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][13], 0x27178); /* 0x2074e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][14], 0x245bd); /* 0x2074e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][15], 0x2daf); /* 0x2074e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][16], 0x396ba); /* 0x2074e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][17], 0x4099); /* 0x2074e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][18], 0x1e215); /* 0x2074e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][19], 0x35466); /* 0x2074e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][20], 0x1d38d); /* 0x2074e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][21], 0x20ef6); /* 0x2074e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][22], 0x18005); /* 0x2074e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][23], 0x22cc2); /* 0x2074e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][24], 0x2be53); /* 0x2074e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][25], 0x37c96); /* 0x2074e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][26], 0x25957); /* 0x2074e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][27], 0x12625); /* 0x2074e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][28], 0x2693f); /* 0x2074e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][29], 0x15e82); /* 0x2074e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][30], 0x1d43b); /* 0x2074e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][31], 0x1efc0); /* 0x2074e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][32], 0x20417); /* 0x2074e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][33], 0x80ae); /* 0x2074e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][34], 0x14de7); /* 0x2074e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][35], 0x2dad0); /* 0x2074e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][36], 0xe0e); /* 0x2074e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][37], 0x10166); /* 0x2074e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][38], 0x3ca5b); /* 0x2074e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][39], 0x176ab); /* 0x2074e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][40], 0x1e7df); /* 0x2074ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][41], 0x3179a); /* 0x2074ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][42], 0x12873); /* 0x2074ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][43], 0x179be); /* 0x2074eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][44], 0x6698); /* 0x2074eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][45], 0x1699e); /* 0x2074eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][46], 0x16016); /* 0x2074eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][47], 0x70d5); /* 0x2074ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][48], 0x1504a); /* 0x2074ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][49], 0x14330); /* 0x2074ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][50], 0x27f1f); /* 0x2074ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][51], 0x72d6); /* 0x2074ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][0], 0xceff); /* 0x2074f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][1], 0x1a014); /* 0x2074f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][2], 0x24206); /* 0x2074f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][3], 0x201fb); /* 0x2074f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][4], 0x2dd48); /* 0x2074f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][5], 0x13aac); /* 0x2074f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][6], 0x1832); /* 0x2074f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][7], 0x31de); /* 0x2074f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][8], 0x26208); /* 0x2074f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][9], 0x3a5fd); /* 0x2074f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][10], 0xe417); /* 0x2074f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][11], 0x169b2); /* 0x2074f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][12], 0xb0ab); /* 0x2074f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][13], 0x2275); /* 0x2074f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][14], 0x2643d); /* 0x2074f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][15], 0x20c21); /* 0x2074f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][16], 0x2777e); /* 0x2074f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][17], 0xbde); /* 0x2074f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][18], 0xfb65); /* 0x2074f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][19], 0x67c); /* 0x2074f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][20], 0x15eda); /* 0x2074f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][21], 0x156ee); /* 0x2074f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][22], 0x21a58); /* 0x2074f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][23], 0x19223); /* 0x2074f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][24], 0x2a6c0); /* 0x2074f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][25], 0xa11d); /* 0x2074f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][26], 0x9e64); /* 0x2074f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][27], 0x967b); /* 0x2074f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][28], 0x38729); /* 0x2074f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][29], 0xad37); /* 0x2074f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][30], 0x1d280); /* 0x2074f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][31], 0x16aa4); /* 0x2074f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][32], 0x862d); /* 0x2074f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][33], 0x11818); /* 0x2074f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][34], 0x18777); /* 0x2074f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][35], 0x6994); /* 0x2074f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][36], 0x36b0f); /* 0x2074f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][37], 0x279f0); /* 0x2074f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][38], 0x38d36); /* 0x2074f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][39], 0xcc00); /* 0x2074f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][40], 0x24d4f); /* 0x2074fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][41], 0xac16); /* 0x2074fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][42], 0x3f215); /* 0x2074fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][43], 0x1ad61); /* 0x2074fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][44], 0x39505); /* 0x2074fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][45], 0x3442d); /* 0x2074fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][46], 0x2930f); /* 0x2074fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][47], 0x29e9c); /* 0x2074fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][48], 0x20229); /* 0x2074fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][49], 0x39e3a); /* 0x2074fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][50], 0x2c687); /* 0x2074fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][51], 0x2342a); /* 0x2074fcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][0], 0x1371c); /* 0x2075000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][1], 0x3c2d2); /* 0x2075004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][2], 0x339d3); /* 0x2075008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][3], 0x19ddb); /* 0x207500c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][4], 0x1b56); /* 0x2075010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][5], 0x31ec3); /* 0x2075014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][6], 0x2add4); /* 0x2075018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][7], 0x12c58); /* 0x207501c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][8], 0x137b); /* 0x2075020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][9], 0x3701); /* 0x2075024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][10], 0x3a9fd); /* 0x2075028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][11], 0x6462); /* 0x207502c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][12], 0x11297); /* 0x2075030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][13], 0x6d16); /* 0x2075034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][14], 0x1ff7e); /* 0x2075038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][15], 0xe0e8); /* 0x207503c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][16], 0x8d39); /* 0x2075040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][17], 0x3d714); /* 0x2075044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][18], 0x20f73); /* 0x2075048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][19], 0x33eb1); /* 0x207504c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][20], 0x262cc); /* 0x2075050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][21], 0x2e39b); /* 0x2075054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][22], 0x23669); /* 0x2075058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][23], 0x153ad); /* 0x207505c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][24], 0x13c64); /* 0x2075060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][25], 0x1bd38); /* 0x2075064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][26], 0x128be); /* 0x2075068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][27], 0x28061); /* 0x207506c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][28], 0x2d2c); /* 0x2075070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][29], 0x11c19); /* 0x2075074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][30], 0x3093a); /* 0x2075078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][31], 0x19aac); /* 0x207507c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][32], 0x2ffdb); /* 0x2075080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][33], 0x1eed0); /* 0x2075084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][34], 0x102b0); /* 0x2075088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][35], 0x29aa5); /* 0x207508c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][36], 0xafed); /* 0x2075090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][37], 0x34e97); /* 0x2075094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][38], 0xc39d); /* 0x2075098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][39], 0x2141a); /* 0x207509c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][40], 0x8caa); /* 0x20750a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][41], 0x17eed); /* 0x20750a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][42], 0x2b6a9); /* 0x20750a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][43], 0xc9fa); /* 0x20750ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][44], 0x281fd); /* 0x20750b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][45], 0x1b765); /* 0x20750b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][46], 0x32b3); /* 0x20750b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][47], 0x288af); /* 0x20750bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][48], 0x3498c); /* 0x20750c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][49], 0x1ac5); /* 0x20750c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][50], 0x28d0a); /* 0x20750c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][51], 0x178d9); /* 0x20750cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][0], 0x28f9b); /* 0x2075100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][1], 0xceab); /* 0x2075104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][2], 0x183ff); /* 0x2075108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][3], 0x7b37); /* 0x207510c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][4], 0x1f466); /* 0x2075110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][5], 0x1f3d8); /* 0x2075114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][6], 0x1cdc3); /* 0x2075118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][7], 0x32d95); /* 0x207511c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][8], 0x2b1c4); /* 0x2075120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][9], 0x9a9e); /* 0x2075124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][10], 0x2d392); /* 0x2075128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][11], 0x193e8); /* 0x207512c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][12], 0x39da6); /* 0x2075130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][13], 0x3ae72); /* 0x2075134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][14], 0x2eb2b); /* 0x2075138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][15], 0x25ae6); /* 0x207513c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][16], 0x3c0eb); /* 0x2075140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][17], 0x30eab); /* 0x2075144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][18], 0x2d893); /* 0x2075148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][19], 0x17d7a); /* 0x207514c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][20], 0x1d12e); /* 0x2075150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][21], 0x257b0); /* 0x2075154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][22], 0x781b); /* 0x2075158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][23], 0x38b47); /* 0x207515c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][24], 0xbaeb); /* 0x2075160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][25], 0xf316); /* 0x2075164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][26], 0x3ff2b); /* 0x2075168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][27], 0x34afa); /* 0x207516c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][28], 0x407b); /* 0x2075170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][29], 0x16131); /* 0x2075174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][30], 0x1dfa6); /* 0x2075178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][31], 0x3323f); /* 0x207517c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][32], 0x1b0f6); /* 0x2075180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][33], 0x3d20a); /* 0x2075184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][34], 0x379dd); /* 0x2075188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][35], 0x13406); /* 0x207518c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][36], 0x1202); /* 0x2075190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][37], 0x342cc); /* 0x2075194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][38], 0x1d7e4); /* 0x2075198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][39], 0xd9b8); /* 0x207519c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][40], 0x36150); /* 0x20751a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][41], 0x35ace); /* 0x20751a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][42], 0x9f26); /* 0x20751a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][43], 0x837c); /* 0x20751ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][44], 0x12a16); /* 0x20751b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][45], 0x7569); /* 0x20751b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][46], 0x359c6); /* 0x20751b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][47], 0x26b42); /* 0x20751bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][48], 0x345a); /* 0x20751c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][49], 0x9f8e); /* 0x20751c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][50], 0x2d3ef); /* 0x20751c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][51], 0x32d75); /* 0x20751cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][0], 0x2f839); /* 0x2075200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][1], 0x3498f); /* 0x2075204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][2], 0x593b); /* 0x2075208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][3], 0x32754); /* 0x207520c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][4], 0x19ba5); /* 0x2075210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][5], 0x3a0ff); /* 0x2075214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][6], 0x2b991); /* 0x2075218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][7], 0x102b4); /* 0x207521c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][8], 0x28b37); /* 0x2075220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][9], 0x3df33); /* 0x2075224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][10], 0x2ce72); /* 0x2075228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][11], 0x125a9); /* 0x207522c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][12], 0x30a17); /* 0x2075230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][13], 0x125af); /* 0x2075234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][14], 0x187dc); /* 0x2075238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][15], 0x19283); /* 0x207523c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][16], 0x397da); /* 0x2075240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][17], 0x1ebe2); /* 0x2075244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][18], 0xeaeb); /* 0x2075248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][19], 0x3b37c); /* 0x207524c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][20], 0xc2b); /* 0x2075250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][21], 0x1a220); /* 0x2075254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][22], 0x27a39); /* 0x2075258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][23], 0x1bf0f); /* 0x207525c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][24], 0x11c97); /* 0x2075260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][25], 0x25467); /* 0x2075264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][26], 0x2ebc0); /* 0x2075268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][27], 0x21739); /* 0x207526c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][28], 0x3e58); /* 0x2075270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][29], 0xe79); /* 0x2075274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][30], 0x2a3cd); /* 0x2075278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][31], 0x27734); /* 0x207527c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][32], 0xff25); /* 0x2075280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][33], 0x22579); /* 0x2075284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][34], 0x21378); /* 0x2075288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][35], 0x32237); /* 0x207528c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][36], 0x68a0); /* 0x2075290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][37], 0x289a1); /* 0x2075294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][38], 0x16bb7); /* 0x2075298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][39], 0x249f6); /* 0x207529c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][40], 0x2cc30); /* 0x20752a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][41], 0x37f73); /* 0x20752a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][42], 0x358d1); /* 0x20752a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][43], 0x37b0); /* 0x20752ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][44], 0x19373); /* 0x20752b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][45], 0x245e5); /* 0x20752b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][46], 0x6c6e); /* 0x20752b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][47], 0xb76f); /* 0x20752bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][48], 0x8b0d); /* 0x20752c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][49], 0xacc8); /* 0x20752c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][50], 0x20833); /* 0x20752c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][51], 0x30648); /* 0x20752cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][0], 0x18820); /* 0x2075300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][1], 0x2b187); /* 0x2075304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][2], 0x119a5); /* 0x2075308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][3], 0x1d50a); /* 0x207530c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][4], 0x13ab3); /* 0x2075310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][5], 0x1c28e); /* 0x2075314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][6], 0x15a65); /* 0x2075318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][7], 0x10079); /* 0x207531c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][8], 0x353fe); /* 0x2075320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][9], 0xc804); /* 0x2075324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][10], 0x136ef); /* 0x2075328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][11], 0x4b4c); /* 0x207532c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][12], 0x98d); /* 0x2075330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][13], 0x252e1); /* 0x2075334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][14], 0xedc1); /* 0x2075338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][15], 0x271b2); /* 0x207533c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][16], 0x1c621); /* 0x2075340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][17], 0x24d79); /* 0x2075344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][18], 0xf9c4); /* 0x2075348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][19], 0x303e6); /* 0x207534c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][20], 0x393b5); /* 0x2075350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][21], 0x29937); /* 0x2075354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][22], 0x15a3e); /* 0x2075358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][23], 0x32346); /* 0x207535c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][24], 0x86cc); /* 0x2075360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][25], 0x33a6a); /* 0x2075364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][26], 0x3426b); /* 0x2075368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][27], 0x2a1be); /* 0x207536c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][28], 0x27ee); /* 0x2075370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][29], 0x3317); /* 0x2075374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][30], 0x3cc15); /* 0x2075378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][31], 0x28736); /* 0x207537c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][32], 0x1e452); /* 0x2075380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][33], 0x55bc); /* 0x2075384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][34], 0x25a75); /* 0x2075388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][35], 0x25afd); /* 0x207538c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][36], 0x15824); /* 0x2075390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][37], 0x309cb); /* 0x2075394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][38], 0x2a40d); /* 0x2075398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][39], 0x3790d); /* 0x207539c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][40], 0x2ccb0); /* 0x20753a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][41], 0xce27); /* 0x20753a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][42], 0x45a6); /* 0x20753a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][43], 0x14878); /* 0x20753ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][44], 0x33403); /* 0x20753b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][45], 0x210d2); /* 0x20753b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][46], 0x98ce); /* 0x20753b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][47], 0x350ec); /* 0x20753bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][48], 0x2dfee); /* 0x20753c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][49], 0x1f6b4); /* 0x20753c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][50], 0x12daf); /* 0x20753c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][51], 0x28213); /* 0x20753cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][0], 0x39757); /* 0x2075400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][1], 0x2841e); /* 0x2075404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][2], 0x2351c); /* 0x2075408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][3], 0x1431); /* 0x207540c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][4], 0x231c9); /* 0x2075410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][5], 0x3589f); /* 0x2075414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][6], 0x14f1a); /* 0x2075418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][7], 0x189f4); /* 0x207541c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][8], 0x5617); /* 0x2075420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][9], 0xf652); /* 0x2075424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][10], 0x34fa3); /* 0x2075428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][11], 0x2b076); /* 0x207542c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][12], 0x380de); /* 0x2075430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][13], 0x1e891); /* 0x2075434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][14], 0xe57); /* 0x2075438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][15], 0x14f10); /* 0x207543c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][16], 0x2324b); /* 0x2075440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][17], 0xa6df); /* 0x2075444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][18], 0x1d105); /* 0x2075448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][19], 0xc62b); /* 0x207544c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][20], 0xfae0); /* 0x2075450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][21], 0x15ed3); /* 0x2075454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][22], 0x3b7be); /* 0x2075458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][23], 0x1ce4e); /* 0x207545c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][24], 0x1bfbc); /* 0x2075460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][25], 0x1b4f5); /* 0x2075464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][26], 0x2f55c); /* 0x2075468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][27], 0x13afe); /* 0x207546c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][28], 0x38362); /* 0x2075470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][29], 0x2e04c); /* 0x2075474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][30], 0x2e0a1); /* 0x2075478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][31], 0x28bc9); /* 0x207547c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][32], 0x2ed8f); /* 0x2075480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][33], 0x7645); /* 0x2075484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][34], 0x309bc); /* 0x2075488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][35], 0x7258); /* 0x207548c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][36], 0x92bb); /* 0x2075490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][37], 0x19385); /* 0x2075494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][38], 0x2e549); /* 0x2075498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][39], 0x1a2fe); /* 0x207549c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][40], 0x348b); /* 0x20754a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][41], 0xaac8); /* 0x20754a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][42], 0x5a6d); /* 0x20754a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][43], 0x2b45f); /* 0x20754ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][44], 0x2e9db); /* 0x20754b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][45], 0x11e9); /* 0x20754b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][46], 0x38629); /* 0x20754b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][47], 0x1a119); /* 0x20754bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][48], 0x3b64c); /* 0x20754c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][49], 0x229a7); /* 0x20754c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][50], 0x36042); /* 0x20754c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][51], 0x214f7); /* 0x20754cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][0], 0x8aaf); /* 0x2075500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][1], 0x320b2); /* 0x2075504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][2], 0xfa73); /* 0x2075508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][3], 0x3730a); /* 0x207550c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][4], 0x3bd15); /* 0x2075510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][5], 0x4155); /* 0x2075514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][6], 0x25642); /* 0x2075518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][7], 0x11a51); /* 0x207551c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][8], 0x8d7); /* 0x2075520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][9], 0x23dc1); /* 0x2075524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][10], 0x2fb25); /* 0x2075528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][11], 0x12e99); /* 0x207552c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][12], 0x81a6); /* 0x2075530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][13], 0xb15f); /* 0x2075534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][14], 0x2cb8d); /* 0x2075538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][15], 0x2b939); /* 0x207553c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][16], 0x10e5d); /* 0x2075540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][17], 0x383ee); /* 0x2075544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][18], 0x1ece1); /* 0x2075548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][19], 0x334e); /* 0x207554c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][20], 0x1d73f); /* 0x2075550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][21], 0x3cb0b); /* 0x2075554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][22], 0x2dad7); /* 0x2075558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][23], 0x25122); /* 0x207555c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][24], 0x2d0eb); /* 0x2075560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][25], 0x2133c); /* 0x2075564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][26], 0xf4f4); /* 0x2075568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][27], 0x1ebf0); /* 0x207556c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][28], 0x2a5d5); /* 0x2075570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][29], 0x29cd8); /* 0x2075574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][30], 0x23874); /* 0x2075578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][31], 0x2631f); /* 0x207557c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][32], 0x382eb); /* 0x2075580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][33], 0x28518); /* 0x2075584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][34], 0x31741); /* 0x2075588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][35], 0x2c3db); /* 0x207558c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][36], 0x2b400); /* 0x2075590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][37], 0x24378); /* 0x2075594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][38], 0xe62f); /* 0x2075598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][39], 0x18627); /* 0x207559c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][40], 0x1b75c); /* 0x20755a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][41], 0x9970); /* 0x20755a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][42], 0x28f4a); /* 0x20755a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][43], 0x2336a); /* 0x20755ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][44], 0x1a922); /* 0x20755b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][45], 0x34a5e); /* 0x20755b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][46], 0x21d2d); /* 0x20755b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][47], 0x32312); /* 0x20755bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][48], 0x317f6); /* 0x20755c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][49], 0x2dc25); /* 0x20755c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][50], 0x286e); /* 0x20755c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][51], 0x3dac4); /* 0x20755cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][0], 0x11e5e); /* 0x2075600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][1], 0x2f6d6); /* 0x2075604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][2], 0x3ceb7); /* 0x2075608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][3], 0x7cd0); /* 0x207560c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][4], 0xdc1); /* 0x2075610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][5], 0x3f5bd); /* 0x2075614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][6], 0x2dc2c); /* 0x2075618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][7], 0x7f35); /* 0x207561c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][8], 0x3ffc3); /* 0x2075620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][9], 0x1647e); /* 0x2075624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][10], 0x3b943); /* 0x2075628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][11], 0x318e3); /* 0x207562c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][12], 0x5172); /* 0x2075630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][13], 0x2f485); /* 0x2075634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][14], 0x86e4); /* 0x2075638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][15], 0x11f16); /* 0x207563c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][16], 0x2d559); /* 0x2075640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][17], 0x2f323); /* 0x2075644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][18], 0x2c67e); /* 0x2075648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][19], 0x3f602); /* 0x207564c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][20], 0x11d8b); /* 0x2075650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][21], 0x9de3); /* 0x2075654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][22], 0x3e8aa); /* 0x2075658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][23], 0x3a0ba); /* 0x207565c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][24], 0x2b16e); /* 0x2075660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][25], 0x158c8); /* 0x2075664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][26], 0x1d3db); /* 0x2075668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][27], 0x24dd3); /* 0x207566c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][28], 0x1e915); /* 0x2075670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][29], 0x221ac); /* 0x2075674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][30], 0x3c518); /* 0x2075678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][31], 0x11ae7); /* 0x207567c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][32], 0x3bebc); /* 0x2075680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][33], 0x36889); /* 0x2075684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][34], 0x2adb7); /* 0x2075688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][35], 0x3ffe3); /* 0x207568c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][36], 0x15404); /* 0x2075690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][37], 0x25fa3); /* 0x2075694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][38], 0x1433d); /* 0x2075698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][39], 0xc42d); /* 0x207569c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][40], 0x1d409); /* 0x20756a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][41], 0x16461); /* 0x20756a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][42], 0x14dab); /* 0x20756a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][43], 0x4c17); /* 0x20756ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][44], 0x35730); /* 0x20756b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][45], 0x3ebbf); /* 0x20756b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][46], 0x1616c); /* 0x20756b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][47], 0x1908b); /* 0x20756bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][48], 0x1e761); /* 0x20756c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][49], 0x2920); /* 0x20756c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][50], 0x39125); /* 0x20756c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][51], 0x1742d); /* 0x20756cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][0], 0x1b703); /* 0x2075700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][1], 0x18768); /* 0x2075704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][2], 0x1bac5); /* 0x2075708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][3], 0xfdf3); /* 0x207570c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][4], 0x1a08f); /* 0x2075710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][5], 0x123c9); /* 0x2075714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][6], 0x17ebc); /* 0x2075718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][7], 0x2611f); /* 0x207571c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][8], 0x1d877); /* 0x2075720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][9], 0x13366); /* 0x2075724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][10], 0x6eb4); /* 0x2075728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][11], 0x22653); /* 0x207572c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][12], 0xb7b); /* 0x2075730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][13], 0x22c4d); /* 0x2075734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][14], 0x1c258); /* 0x2075738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][15], 0x51e7); /* 0x207573c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][16], 0x3dd9); /* 0x2075740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][17], 0x7291); /* 0x2075744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][18], 0x215db); /* 0x2075748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][19], 0xdc0); /* 0x207574c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][20], 0x53b8); /* 0x2075750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][21], 0x347f); /* 0x2075754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][22], 0x1dd30); /* 0x2075758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][23], 0x512c); /* 0x207575c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][24], 0xfd4f); /* 0x2075760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][25], 0x38e3f); /* 0x2075764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][26], 0x1ed73); /* 0x2075768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][27], 0x1a378); /* 0x207576c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][28], 0x2651b); /* 0x2075770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][29], 0x9411); /* 0x2075774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][30], 0xa0ec); /* 0x2075778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][31], 0x3a50e); /* 0x207577c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][32], 0x1845f); /* 0x2075780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][33], 0x2eab5); /* 0x2075784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][34], 0x3a9c5); /* 0x2075788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][35], 0x29eef); /* 0x207578c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][36], 0x45b8); /* 0x2075790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][37], 0x1c49c); /* 0x2075794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][38], 0x2a92e); /* 0x2075798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][39], 0x21782); /* 0x207579c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][40], 0x28d9c); /* 0x20757a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][41], 0x2f2bc); /* 0x20757a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][42], 0x24f1a); /* 0x20757a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][43], 0x26372); /* 0x20757ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][44], 0x24a80); /* 0x20757b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][45], 0x34965); /* 0x20757b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][46], 0x6609); /* 0x20757b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][47], 0x2c0bb); /* 0x20757bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][48], 0x1f41b); /* 0x20757c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][49], 0xe96f); /* 0x20757c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][50], 0xbbca); /* 0x20757c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][51], 0x25e2d); /* 0x20757cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][0], 0x2afe1); /* 0x2075800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][1], 0x395fc); /* 0x2075804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][2], 0x3e401); /* 0x2075808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][3], 0xa3cf); /* 0x207580c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][4], 0x30c1c); /* 0x2075810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][5], 0x38a20); /* 0x2075814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][6], 0x1b149); /* 0x2075818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][7], 0x39cdc); /* 0x207581c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][8], 0x1690a); /* 0x2075820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][9], 0x2e646); /* 0x2075824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][10], 0x362dd); /* 0x2075828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][11], 0x290db); /* 0x207582c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][12], 0x4331); /* 0x2075830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][13], 0x3766e); /* 0x2075834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][14], 0xb69a); /* 0x2075838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][15], 0x22abf); /* 0x207583c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][16], 0x17f67); /* 0x2075840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][17], 0x107a6); /* 0x2075844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][18], 0x20a37); /* 0x2075848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][19], 0x2134f); /* 0x207584c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][20], 0x2a6b); /* 0x2075850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][21], 0x270e8); /* 0x2075854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][22], 0x1e736); /* 0x2075858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][23], 0x36b18); /* 0x207585c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][24], 0x24714); /* 0x2075860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][25], 0x9387); /* 0x2075864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][26], 0x1b4bc); /* 0x2075868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][27], 0x38ac1); /* 0x207586c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][28], 0x30b13); /* 0x2075870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][29], 0x9718); /* 0x2075874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][30], 0x2c7b0); /* 0x2075878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][31], 0x2a89b); /* 0x207587c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][32], 0x375e5); /* 0x2075880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][33], 0xa4dd); /* 0x2075884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][34], 0x6237); /* 0x2075888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][35], 0x38182); /* 0x207588c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][36], 0x3c985); /* 0x2075890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][37], 0x154e3); /* 0x2075894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][38], 0x269a5); /* 0x2075898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][39], 0x34b79); /* 0x207589c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][40], 0x7fed); /* 0x20758a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][41], 0x1966); /* 0x20758a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][42], 0x1dedd); /* 0x20758a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][43], 0x37fd3); /* 0x20758ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][44], 0x2a7ec); /* 0x20758b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][45], 0x1ce59); /* 0x20758b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][46], 0x385c0); /* 0x20758b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][47], 0x3f8f5); /* 0x20758bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][48], 0x21179); /* 0x20758c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][49], 0x3641); /* 0x20758c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][50], 0x3c3b2); /* 0x20758c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][51], 0x2c52d); /* 0x20758cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][0], 0x1acfc); /* 0x2075900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][1], 0x667f); /* 0x2075904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][2], 0x1fc25); /* 0x2075908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][3], 0x251bd); /* 0x207590c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][4], 0x32d80); /* 0x2075910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][5], 0x667b); /* 0x2075914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][6], 0x39c45); /* 0x2075918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][7], 0x1d529); /* 0x207591c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][8], 0x1f689); /* 0x2075920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][9], 0xdc04); /* 0x2075924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][10], 0x1eb1d); /* 0x2075928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][11], 0x135f7); /* 0x207592c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][12], 0xf445); /* 0x2075930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][13], 0x379dc); /* 0x2075934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][14], 0x1e3be); /* 0x2075938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][15], 0x1dd3e); /* 0x207593c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][16], 0x32120); /* 0x2075940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][17], 0x120f1); /* 0x2075944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][18], 0xeaf7); /* 0x2075948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][19], 0x2c7c8); /* 0x207594c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][20], 0x662); /* 0x2075950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][21], 0x39fbd); /* 0x2075954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][22], 0x17c2d); /* 0x2075958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][23], 0x1bf8f); /* 0x207595c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][24], 0x20fcb); /* 0x2075960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][25], 0x25b01); /* 0x2075964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][26], 0x1035d); /* 0x2075968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][27], 0x1b2ab); /* 0x207596c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][28], 0x3fe42); /* 0x2075970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][29], 0x2bd2); /* 0x2075974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][30], 0xa251); /* 0x2075978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][31], 0x26266); /* 0x207597c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][32], 0x132a4); /* 0x2075980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][33], 0x7a4a); /* 0x2075984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][34], 0x37b7b); /* 0x2075988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][35], 0x2ca3); /* 0x207598c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][36], 0x1f4a8); /* 0x2075990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][37], 0x15d9c); /* 0x2075994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][38], 0x3d69f); /* 0x2075998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][39], 0xfdf5); /* 0x207599c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][40], 0x2a500); /* 0x20759a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][41], 0x21a69); /* 0x20759a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][42], 0x24961); /* 0x20759a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][43], 0xfcda); /* 0x20759ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][44], 0x31ba5); /* 0x20759b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][45], 0x158bf); /* 0x20759b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][46], 0x169b5); /* 0x20759b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][47], 0x5ba2); /* 0x20759bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][48], 0x2f7eb); /* 0x20759c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][49], 0x1f99c); /* 0x20759c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][50], 0x24f5); /* 0x20759c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][51], 0x2c48e); /* 0x20759cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][0], 0xd4bc); /* 0x2075a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][1], 0x29d5b); /* 0x2075a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][2], 0xad47); /* 0x2075a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][3], 0x22cb5); /* 0x2075a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][4], 0x15b42); /* 0x2075a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][5], 0x3ac8b); /* 0x2075a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][6], 0x12e3a); /* 0x2075a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][7], 0x36e5d); /* 0x2075a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][8], 0x2e7ee); /* 0x2075a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][9], 0x124e3); /* 0x2075a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][10], 0x1385a); /* 0x2075a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][11], 0x25e4b); /* 0x2075a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][12], 0x3d600); /* 0x2075a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][13], 0x3ee6f); /* 0x2075a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][14], 0x19270); /* 0x2075a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][15], 0x1fff3); /* 0x2075a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][16], 0x1b17c); /* 0x2075a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][17], 0x1b7ec); /* 0x2075a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][18], 0x313b1); /* 0x2075a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][19], 0x2a869); /* 0x2075a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][20], 0x2b247); /* 0x2075a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][21], 0xf609); /* 0x2075a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][22], 0x6bc3); /* 0x2075a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][23], 0x19b20); /* 0x2075a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][24], 0x1dcef); /* 0x2075a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][25], 0x1c14c); /* 0x2075a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][26], 0x24622); /* 0x2075a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][27], 0x21c4b); /* 0x2075a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][28], 0x18929); /* 0x2075a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][29], 0x1ed14); /* 0x2075a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][30], 0x186f6); /* 0x2075a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][31], 0x2b5c9); /* 0x2075a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][32], 0x5085); /* 0x2075a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][33], 0xb1bf); /* 0x2075a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][34], 0x99b6); /* 0x2075a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][35], 0x6dfb); /* 0x2075a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][36], 0x31574); /* 0x2075a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][37], 0x38); /* 0x2075a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][38], 0x1ecd8); /* 0x2075a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][39], 0x12eb); /* 0x2075a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][40], 0x180a0); /* 0x2075aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][41], 0x3ab34); /* 0x2075aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][42], 0x9c25); /* 0x2075aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][43], 0x8a22); /* 0x2075aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][44], 0x1748c); /* 0x2075ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][45], 0x242b4); /* 0x2075ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][46], 0x304f7); /* 0x2075ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][47], 0x849b); /* 0x2075abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][48], 0x2ec9f); /* 0x2075ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][49], 0x885); /* 0x2075ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][50], 0x3d8af); /* 0x2075ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][51], 0x2f952); /* 0x2075acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][0], 0x114b3); /* 0x2075b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][1], 0x12f58); /* 0x2075b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][2], 0x8c50); /* 0x2075b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][3], 0x5f0b); /* 0x2075b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][4], 0x19f68); /* 0x2075b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][5], 0x9eb6); /* 0x2075b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][6], 0x21159); /* 0x2075b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][7], 0x19c2b); /* 0x2075b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][8], 0x38a91); /* 0x2075b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][9], 0x3615e); /* 0x2075b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][10], 0x34db3); /* 0x2075b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][11], 0x20084); /* 0x2075b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][12], 0x3c022); /* 0x2075b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][13], 0x2bbf7); /* 0x2075b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][14], 0x28788); /* 0x2075b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][15], 0xb69); /* 0x2075b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][16], 0x553e); /* 0x2075b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][17], 0x2db32); /* 0x2075b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][18], 0x26aff); /* 0x2075b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][19], 0x2ec25); /* 0x2075b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][20], 0x3d001); /* 0x2075b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][21], 0x1f994); /* 0x2075b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][22], 0x14350); /* 0x2075b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][23], 0x291fb); /* 0x2075b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][24], 0x2ff67); /* 0x2075b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][25], 0x2f4d6); /* 0x2075b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][26], 0x3ec5c); /* 0x2075b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][27], 0x1c88c); /* 0x2075b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][28], 0x397ec); /* 0x2075b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][29], 0x3d942); /* 0x2075b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][30], 0x19522); /* 0x2075b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][31], 0x1c18f); /* 0x2075b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][32], 0x386d6); /* 0x2075b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][33], 0x21006); /* 0x2075b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][34], 0x36684); /* 0x2075b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][35], 0x2541d); /* 0x2075b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][36], 0x26ac8); /* 0x2075b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][37], 0x2ce5b); /* 0x2075b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][38], 0x2cfc9); /* 0x2075b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][39], 0x1d88c); /* 0x2075b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][40], 0x3456f); /* 0x2075ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][41], 0x368f2); /* 0x2075ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][42], 0x1624d); /* 0x2075ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][43], 0xa62b); /* 0x2075bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][44], 0x2ee29); /* 0x2075bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][45], 0x35999); /* 0x2075bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][46], 0x2095e); /* 0x2075bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][47], 0x3aac1); /* 0x2075bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][48], 0x2aa8f); /* 0x2075bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][49], 0x3ee88); /* 0x2075bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][50], 0x3b2e3); /* 0x2075bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][51], 0x3963a); /* 0x2075bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][0], 0x1e82); /* 0x2075c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][1], 0x14caa); /* 0x2075c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][2], 0x324b6); /* 0x2075c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][3], 0x1265c); /* 0x2075c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][4], 0x20e2e); /* 0x2075c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][5], 0x1fc8a); /* 0x2075c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][6], 0x20b82); /* 0x2075c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][7], 0x19d9d); /* 0x2075c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][8], 0xa7f3); /* 0x2075c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][9], 0x37ca1); /* 0x2075c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][10], 0x16e3a); /* 0x2075c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][11], 0x1e695); /* 0x2075c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][12], 0xa1e4); /* 0x2075c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][13], 0x3afd2); /* 0x2075c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][14], 0xbf10); /* 0x2075c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][15], 0x3783c); /* 0x2075c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][16], 0x3cd28); /* 0x2075c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][17], 0x30d69); /* 0x2075c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][18], 0x2d58b); /* 0x2075c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][19], 0x38b4d); /* 0x2075c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][20], 0x4ffd); /* 0x2075c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][21], 0x3bcd2); /* 0x2075c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][22], 0x527f); /* 0x2075c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][23], 0x1f93b); /* 0x2075c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][24], 0x1019f); /* 0x2075c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][25], 0x1ab04); /* 0x2075c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][26], 0x16930); /* 0x2075c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][27], 0x2ee70); /* 0x2075c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][28], 0x5c08); /* 0x2075c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][29], 0x852c); /* 0x2075c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][30], 0x1425d); /* 0x2075c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][31], 0x2cdee); /* 0x2075c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][32], 0x36d6b); /* 0x2075c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][33], 0xf252); /* 0x2075c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][34], 0x22396); /* 0x2075c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][35], 0xb65); /* 0x2075c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][36], 0x2010e); /* 0x2075c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][37], 0x1c094); /* 0x2075c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][38], 0x2a70d); /* 0x2075c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][39], 0x541f); /* 0x2075c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][40], 0x2b889); /* 0x2075ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][41], 0x2695a); /* 0x2075ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][42], 0x2b784); /* 0x2075ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][43], 0x11c9e); /* 0x2075cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][44], 0x3e2a9); /* 0x2075cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][45], 0x8c); /* 0x2075cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][46], 0x399a5); /* 0x2075cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][47], 0xf4e1); /* 0x2075cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][48], 0x11e54); /* 0x2075cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][49], 0x35903); /* 0x2075cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][50], 0x2649c); /* 0x2075cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][51], 0x3fc03); /* 0x2075ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][0], 0x22512); /* 0x2075d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][1], 0x3e035); /* 0x2075d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][2], 0x12a28); /* 0x2075d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][3], 0x3971b); /* 0x2075d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][4], 0x21018); /* 0x2075d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][5], 0x21f71); /* 0x2075d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][6], 0x14c8a); /* 0x2075d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][7], 0x14529); /* 0x2075d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][8], 0x11eeb); /* 0x2075d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][9], 0x2eedd); /* 0x2075d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][10], 0x39a2c); /* 0x2075d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][11], 0x21f6d); /* 0x2075d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][12], 0x279f0); /* 0x2075d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][13], 0x2019c); /* 0x2075d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][14], 0x324fb); /* 0x2075d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][15], 0x2e24a); /* 0x2075d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][16], 0x6c62); /* 0x2075d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][17], 0x383cb); /* 0x2075d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][18], 0x2db47); /* 0x2075d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][19], 0x2c853); /* 0x2075d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][20], 0x1d14b); /* 0x2075d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][21], 0xdb47); /* 0x2075d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][22], 0x3e9ab); /* 0x2075d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][23], 0x38327); /* 0x2075d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][24], 0x11b20); /* 0x2075d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][25], 0x8057); /* 0x2075d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][26], 0x2508a); /* 0x2075d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][27], 0x22c1f); /* 0x2075d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][28], 0x26923); /* 0x2075d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][29], 0x21b57); /* 0x2075d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][30], 0x134bd); /* 0x2075d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][31], 0x2494d); /* 0x2075d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][32], 0x365af); /* 0x2075d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][33], 0x558a); /* 0x2075d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][34], 0x247f3); /* 0x2075d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][35], 0x281ce); /* 0x2075d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][36], 0x39710); /* 0x2075d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][37], 0x9cd9); /* 0x2075d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][38], 0x2ee30); /* 0x2075d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][39], 0xa15d); /* 0x2075d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][40], 0x2fa7a); /* 0x2075da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][41], 0x31113); /* 0x2075da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][42], 0x2c487); /* 0x2075da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][43], 0x3414d); /* 0x2075dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][44], 0x3269e); /* 0x2075db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][45], 0x2f9e6); /* 0x2075db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][46], 0x17760); /* 0x2075db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][47], 0x38955); /* 0x2075dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][48], 0x38046); /* 0x2075dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][49], 0x1d4bb); /* 0x2075dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][50], 0xf9c0); /* 0x2075dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][51], 0xffd0); /* 0x2075dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][0], 0x10f3e); /* 0x2075e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][1], 0x3adc6); /* 0x2075e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][2], 0x1ece2); /* 0x2075e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][3], 0x2a0f); /* 0x2075e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][4], 0x2255b); /* 0x2075e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][5], 0x17e5e); /* 0x2075e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][6], 0x1e09b); /* 0x2075e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][7], 0x17ec0); /* 0x2075e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][8], 0x36c1e); /* 0x2075e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][9], 0xb9e4); /* 0x2075e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][10], 0x6fde); /* 0x2075e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][11], 0xd9c5); /* 0x2075e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][12], 0x1134c); /* 0x2075e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][13], 0x37719); /* 0x2075e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][14], 0x2f858); /* 0x2075e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][15], 0x2fa28); /* 0x2075e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][16], 0x15059); /* 0x2075e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][17], 0x331ce); /* 0x2075e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][18], 0x1ea7a); /* 0x2075e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][19], 0x11ed4); /* 0x2075e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][20], 0x16eb8); /* 0x2075e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][21], 0x362f1); /* 0x2075e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][22], 0x39185); /* 0x2075e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][23], 0x59d9); /* 0x2075e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][24], 0x3b206); /* 0x2075e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][25], 0x1ffab); /* 0x2075e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][26], 0x952f); /* 0x2075e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][27], 0x39ec); /* 0x2075e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][28], 0x1902f); /* 0x2075e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][29], 0x6470); /* 0x2075e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][30], 0x254fa); /* 0x2075e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][31], 0x3c43b); /* 0x2075e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][32], 0x9e0d); /* 0x2075e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][33], 0x1f8c4); /* 0x2075e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][34], 0x1c9a2); /* 0x2075e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][35], 0x1ef8c); /* 0x2075e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][36], 0x306c6); /* 0x2075e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][37], 0x3edeb); /* 0x2075e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][38], 0x361b6); /* 0x2075e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][39], 0x3afcf); /* 0x2075e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][40], 0x5199); /* 0x2075ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][41], 0x1862b); /* 0x2075ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][42], 0x5701); /* 0x2075ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][43], 0x235); /* 0x2075eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][44], 0x36549); /* 0x2075eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][45], 0xf39b); /* 0x2075eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][46], 0x340e0); /* 0x2075eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][47], 0x2f9d4); /* 0x2075ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][48], 0x17cc1); /* 0x2075ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][49], 0x1c9b); /* 0x2075ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][50], 0x313); /* 0x2075ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][51], 0x6a24); /* 0x2075ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][0], 0x8c41); /* 0x2075f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][1], 0xb17); /* 0x2075f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][2], 0x222e7); /* 0x2075f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][3], 0x2962a); /* 0x2075f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][4], 0x2c512); /* 0x2075f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][5], 0x818b); /* 0x2075f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][6], 0xfdf8); /* 0x2075f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][7], 0x193d4); /* 0x2075f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][8], 0x1d4d4); /* 0x2075f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][9], 0x1873f); /* 0x2075f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][10], 0x8ff2); /* 0x2075f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][11], 0x3111); /* 0x2075f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][12], 0x3abe6); /* 0x2075f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][13], 0x12cda); /* 0x2075f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][14], 0x2ec19); /* 0x2075f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][15], 0x3cc48); /* 0x2075f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][16], 0x11eae); /* 0x2075f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][17], 0x2a13e); /* 0x2075f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][18], 0x20e9c); /* 0x2075f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][19], 0x1a66d); /* 0x2075f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][20], 0x275f8); /* 0x2075f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][21], 0x9bc6); /* 0x2075f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][22], 0x1de05); /* 0x2075f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][23], 0x26be3); /* 0x2075f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][24], 0x3939e); /* 0x2075f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][25], 0x22ecd); /* 0x2075f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][26], 0x20d06); /* 0x2075f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][27], 0x19c92); /* 0x2075f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][28], 0x21c7e); /* 0x2075f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][29], 0x28f75); /* 0x2075f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][30], 0x2763d); /* 0x2075f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][31], 0x34edf); /* 0x2075f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][32], 0x5d01); /* 0x2075f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][33], 0xfcd3); /* 0x2075f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][34], 0x35447); /* 0x2075f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][35], 0x39737); /* 0x2075f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][36], 0x167cf); /* 0x2075f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][37], 0x3e620); /* 0x2075f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][38], 0x243bf); /* 0x2075f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][39], 0x1c6f3); /* 0x2075f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][40], 0x3a76b); /* 0x2075fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][41], 0x1747a); /* 0x2075fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][42], 0x3e5bd); /* 0x2075fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][43], 0x2b872); /* 0x2075fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][44], 0x1d726); /* 0x2075fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][45], 0x1f8e2); /* 0x2075fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][46], 0x2c31c); /* 0x2075fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][47], 0x2fb2d); /* 0x2075fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][48], 0x2cb7e); /* 0x2075fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][49], 0x31e3); /* 0x2075fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][50], 0x28f26); /* 0x2075fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][51], 0x17150); /* 0x2075fcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][0], 0x2dd2e); /* 0x2076000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][1], 0x2c4f8); /* 0x2076004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][2], 0x2f3d0); /* 0x2076008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][3], 0x20fb0); /* 0x207600c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][4], 0x3f1df); /* 0x2076010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][5], 0x3de95); /* 0x2076014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][6], 0x20723); /* 0x2076018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][7], 0xfc08); /* 0x207601c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][8], 0x38541); /* 0x2076020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][9], 0x7285); /* 0x2076024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][10], 0x810); /* 0x2076028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][11], 0xd15); /* 0x207602c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][12], 0x228be); /* 0x2076030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][13], 0x2b372); /* 0x2076034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][14], 0x16798); /* 0x2076038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][15], 0x16673); /* 0x207603c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][16], 0x12faf); /* 0x2076040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][17], 0x38882); /* 0x2076044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][18], 0x4c97); /* 0x2076048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][19], 0x4944); /* 0x207604c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][20], 0x2a45c); /* 0x2076050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][21], 0x36f23); /* 0x2076054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][22], 0x656a); /* 0x2076058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][23], 0x3a09b); /* 0x207605c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][24], 0x30a59); /* 0x2076060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][25], 0x17b6f); /* 0x2076064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][26], 0x35bc8); /* 0x2076068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][27], 0x120ee); /* 0x207606c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][28], 0x1b8e7); /* 0x2076070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][29], 0x30243); /* 0x2076074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][30], 0x26a8c); /* 0x2076078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][31], 0x1b7d1); /* 0x207607c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][32], 0x244d8); /* 0x2076080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][33], 0x2d952); /* 0x2076084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][34], 0x2d100); /* 0x2076088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][35], 0x18332); /* 0x207608c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][36], 0x20e4f); /* 0x2076090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][37], 0x21501); /* 0x2076094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][38], 0x574); /* 0x2076098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][39], 0x14ad); /* 0x207609c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][40], 0x38f72); /* 0x20760a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][41], 0x3d0ce); /* 0x20760a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][42], 0x1e11b); /* 0x20760a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][43], 0x250f1); /* 0x20760ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][44], 0x26448); /* 0x20760b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][45], 0x1144c); /* 0x20760b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][46], 0x4994); /* 0x20760b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][47], 0x7315); /* 0x20760bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][48], 0x1d10d); /* 0x20760c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][49], 0x2b0ac); /* 0x20760c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][50], 0x39d9b); /* 0x20760c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][51], 0x26e23); /* 0x20760cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][0], 0x2448f); /* 0x2076100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][1], 0x17889); /* 0x2076104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][2], 0x36e89); /* 0x2076108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][3], 0x3d2b1); /* 0x207610c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][4], 0x3a804); /* 0x2076110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][5], 0x28102); /* 0x2076114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][6], 0x3d943); /* 0x2076118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][7], 0x3794a); /* 0x207611c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][8], 0x1a0ff); /* 0x2076120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][9], 0x24d08); /* 0x2076124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][10], 0x1e0c8); /* 0x2076128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][11], 0xb669); /* 0x207612c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][12], 0x318c7); /* 0x2076130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][13], 0x2c51f); /* 0x2076134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][14], 0x2e181); /* 0x2076138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][15], 0x4980); /* 0x207613c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][16], 0x3a011); /* 0x2076140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][17], 0x3ea7f); /* 0x2076144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][18], 0x1489f); /* 0x2076148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][19], 0x23495); /* 0x207614c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][20], 0x1457a); /* 0x2076150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][21], 0x1ecfb); /* 0x2076154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][22], 0x39b88); /* 0x2076158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][23], 0x1babf); /* 0x207615c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][24], 0x34f5c); /* 0x2076160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][25], 0x3d173); /* 0x2076164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][26], 0x7fd); /* 0x2076168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][27], 0x30af); /* 0x207616c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][28], 0x27e47); /* 0x2076170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][29], 0x3de2f); /* 0x2076174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][30], 0x246fd); /* 0x2076178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][31], 0xc4d2); /* 0x207617c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][32], 0x15343); /* 0x2076180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][33], 0x320a6); /* 0x2076184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][34], 0x18426); /* 0x2076188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][35], 0xbb66); /* 0x207618c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][36], 0x1dc32); /* 0x2076190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][37], 0x34c0b); /* 0x2076194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][38], 0xa03d); /* 0x2076198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][39], 0x3c8a0); /* 0x207619c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][40], 0x24ca3); /* 0x20761a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][41], 0x1e195); /* 0x20761a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][42], 0x21689); /* 0x20761a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][43], 0x3aa36); /* 0x20761ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][44], 0x1f7ed); /* 0x20761b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][45], 0x19d4d); /* 0x20761b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][46], 0xdc24); /* 0x20761b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][47], 0x136a0); /* 0x20761bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][48], 0x7804); /* 0x20761c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][49], 0x198fa); /* 0x20761c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][50], 0x2e19a); /* 0x20761c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][51], 0x3c1f6); /* 0x20761cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][0], 0x1ce1e); /* 0x2076200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][1], 0x29e3d); /* 0x2076204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][2], 0x27124); /* 0x2076208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][3], 0x196e1); /* 0x207620c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][4], 0x15baf); /* 0x2076210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][5], 0x16324); /* 0x2076214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][6], 0xc9c7); /* 0x2076218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][7], 0x3646f); /* 0x207621c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][8], 0x3b908); /* 0x2076220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][9], 0x118d6); /* 0x2076224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][10], 0x29871); /* 0x2076228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][11], 0x20426); /* 0x207622c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][12], 0x20aa1); /* 0x2076230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][13], 0x308ca); /* 0x2076234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][14], 0x23fd9); /* 0x2076238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][15], 0x30cd8); /* 0x207623c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][16], 0x1b9aa); /* 0x2076240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][17], 0xbd6); /* 0x2076244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][18], 0x24ef3); /* 0x2076248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][19], 0x25a10); /* 0x207624c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][20], 0x359c3); /* 0x2076250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][21], 0x1e931); /* 0x2076254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][22], 0x37aa1); /* 0x2076258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][23], 0x3375d); /* 0x207625c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][24], 0x31c7c); /* 0x2076260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][25], 0x21401); /* 0x2076264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][26], 0x37c08); /* 0x2076268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][27], 0x24d07); /* 0x207626c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][28], 0x3084b); /* 0x2076270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][29], 0xa70e); /* 0x2076274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][30], 0x179f3); /* 0x2076278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][31], 0x1283c); /* 0x207627c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][32], 0x110ae); /* 0x2076280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][33], 0x386b6); /* 0x2076284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][34], 0x143ff); /* 0x2076288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][35], 0x2f3f); /* 0x207628c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][36], 0x2f768); /* 0x2076290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][37], 0x1088b); /* 0x2076294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][38], 0x36aa1); /* 0x2076298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][39], 0x1f795); /* 0x207629c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][40], 0x2fa7a); /* 0x20762a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][41], 0xfd94); /* 0x20762a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][42], 0x37da4); /* 0x20762a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][43], 0xec05); /* 0x20762ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][44], 0x2ac2); /* 0x20762b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][45], 0x1c6e1); /* 0x20762b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][46], 0xf768); /* 0x20762b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][47], 0x3d990); /* 0x20762bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][48], 0x26fe); /* 0x20762c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][49], 0x30c8e); /* 0x20762c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][50], 0x2d748); /* 0x20762c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][51], 0x1b4ab); /* 0x20762cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][0], 0x1916b); /* 0x2076300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][1], 0x9ac); /* 0x2076304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][2], 0x23bfe); /* 0x2076308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][3], 0x1b1a9); /* 0x207630c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][4], 0xc077); /* 0x2076310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][5], 0x72fc); /* 0x2076314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][6], 0x2e82a); /* 0x2076318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][7], 0x1f452); /* 0x207631c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][8], 0x1f4be); /* 0x2076320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][9], 0xd21b); /* 0x2076324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][10], 0x2a2b6); /* 0x2076328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][11], 0x39e31); /* 0x207632c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][12], 0x1bb41); /* 0x2076330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][13], 0xbd0e); /* 0x2076334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][14], 0xb208); /* 0x2076338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][15], 0xf8ce); /* 0x207633c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][16], 0x21d6f); /* 0x2076340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][17], 0x1380f); /* 0x2076344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][18], 0x2705a); /* 0x2076348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][19], 0xd8a4); /* 0x207634c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][20], 0x16a6a); /* 0x2076350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][21], 0x2506b); /* 0x2076354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][22], 0x15ccb); /* 0x2076358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][23], 0x2def5); /* 0x207635c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][24], 0x341c2); /* 0x2076360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][25], 0x37a2a); /* 0x2076364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][26], 0x275b5); /* 0x2076368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][27], 0x2081c); /* 0x207636c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][28], 0x3d388); /* 0x2076370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][29], 0x1a38); /* 0x2076374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][30], 0x3b096); /* 0x2076378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][31], 0x2b0c2); /* 0x207637c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][32], 0x3a4a3); /* 0x2076380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][33], 0x252d9); /* 0x2076384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][34], 0x1bc3a); /* 0x2076388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][35], 0x3fcc8); /* 0x207638c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][36], 0x3da3a); /* 0x2076390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][37], 0x8a67); /* 0x2076394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][38], 0x1f1e7); /* 0x2076398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][39], 0x227ca); /* 0x207639c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][40], 0x6ab2); /* 0x20763a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][41], 0xe7e0); /* 0x20763a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][42], 0x21b57); /* 0x20763a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][43], 0x2a71c); /* 0x20763ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][44], 0x1ee74); /* 0x20763b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][45], 0x24a1f); /* 0x20763b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][46], 0x5104); /* 0x20763b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][47], 0x3e87a); /* 0x20763bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][48], 0x1cf19); /* 0x20763c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][49], 0x1ee4d); /* 0x20763c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][50], 0x389f0); /* 0x20763c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][51], 0x5067); /* 0x20763cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][0], 0x2a66b); /* 0x2076400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][1], 0xf0d6); /* 0x2076404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][2], 0x6ff3); /* 0x2076408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][3], 0x19ff0); /* 0x207640c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][4], 0xc13b); /* 0x2076410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][5], 0x25262); /* 0x2076414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][6], 0x108e); /* 0x2076418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][7], 0x1adb7); /* 0x207641c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][8], 0x15a5c); /* 0x2076420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][9], 0xb49d); /* 0x2076424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][10], 0x3d97a); /* 0x2076428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][11], 0x2b270); /* 0x207642c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][12], 0x374b1); /* 0x2076430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][13], 0x1cddf); /* 0x2076434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][14], 0x2399c); /* 0x2076438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][15], 0x32a67); /* 0x207643c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][16], 0x3423f); /* 0x2076440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][17], 0xb488); /* 0x2076444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][18], 0x35cd5); /* 0x2076448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][19], 0x292a4); /* 0x207644c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][20], 0x2951a); /* 0x2076450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][21], 0xd84f); /* 0x2076454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][22], 0x34b81); /* 0x2076458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][23], 0x63f6); /* 0x207645c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][24], 0x18713); /* 0x2076460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][25], 0x27e83); /* 0x2076464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][26], 0x211e8); /* 0x2076468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][27], 0xe6cc); /* 0x207646c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][28], 0x1d2cd); /* 0x2076470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][29], 0x21180); /* 0x2076474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][30], 0x2bee); /* 0x2076478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][31], 0x2f437); /* 0x207647c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][32], 0x38c44); /* 0x2076480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][33], 0xf2f4); /* 0x2076484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][34], 0x143d0); /* 0x2076488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][35], 0x3b25c); /* 0x207648c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][36], 0x36b21); /* 0x2076490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][37], 0x3e029); /* 0x2076494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][38], 0x1e125); /* 0x2076498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][39], 0x25114); /* 0x207649c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][40], 0x26a06); /* 0x20764a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][41], 0x1e9f6); /* 0x20764a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][42], 0x16900); /* 0x20764a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][43], 0x50b7); /* 0x20764ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][44], 0x164dc); /* 0x20764b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][45], 0x9925); /* 0x20764b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][46], 0xecab); /* 0x20764b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][47], 0x1ece3); /* 0x20764bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][48], 0xe5ba); /* 0x20764c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][49], 0xe59e); /* 0x20764c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][50], 0x9fe0); /* 0x20764c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][51], 0xec6); /* 0x20764cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][0], 0x2035f); /* 0x2076500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][1], 0x3b97f); /* 0x2076504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][2], 0x2c56e); /* 0x2076508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][3], 0xc5b2); /* 0x207650c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][4], 0x16d4e); /* 0x2076510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][5], 0x29e6d); /* 0x2076514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][6], 0x38b1b); /* 0x2076518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][7], 0x198ef); /* 0x207651c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][8], 0x3e20c); /* 0x2076520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][9], 0x2db12); /* 0x2076524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][10], 0x67b1); /* 0x2076528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][11], 0x369cc); /* 0x207652c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][12], 0xb493); /* 0x2076530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][13], 0x2f13c); /* 0x2076534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][14], 0xfa60); /* 0x2076538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][15], 0x2c48d); /* 0x207653c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][16], 0x1c099); /* 0x2076540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][17], 0x4f1b); /* 0x2076544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][18], 0x3238); /* 0x2076548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][19], 0x36bf1); /* 0x207654c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][20], 0x36261); /* 0x2076550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][21], 0x2bff2); /* 0x2076554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][22], 0x2cb8a); /* 0x2076558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][23], 0x3bd96); /* 0x207655c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][24], 0x3f3a1); /* 0x2076560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][25], 0x3107c); /* 0x2076564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][26], 0x287); /* 0x2076568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][27], 0x300f2); /* 0x207656c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][28], 0x36c66); /* 0x2076570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][29], 0x1d4ae); /* 0x2076574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][30], 0x1bc44); /* 0x2076578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][31], 0x2dade); /* 0x207657c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][32], 0x22792); /* 0x2076580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][33], 0x6883); /* 0x2076584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][34], 0x2c2ec); /* 0x2076588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][35], 0xdeb4); /* 0x207658c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][36], 0x7300); /* 0x2076590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][37], 0x3353e); /* 0x2076594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][38], 0x2a8e); /* 0x2076598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][39], 0x90f9); /* 0x207659c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][40], 0x39939); /* 0x20765a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][41], 0x1ee73); /* 0x20765a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][42], 0x23620); /* 0x20765a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][43], 0x2b885); /* 0x20765ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][44], 0x1f599); /* 0x20765b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][45], 0x15af5); /* 0x20765b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][46], 0x2a2d8); /* 0x20765b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][47], 0x12c33); /* 0x20765bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][48], 0x35afa); /* 0x20765c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][49], 0x153e4); /* 0x20765c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][50], 0x2bd0c); /* 0x20765c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][51], 0x2450a); /* 0x20765cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][0], 0x3a29f); /* 0x2076600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][1], 0xd097); /* 0x2076604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][2], 0xb16); /* 0x2076608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][3], 0x219dd); /* 0x207660c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][4], 0x15180); /* 0x2076610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][5], 0x2cad3); /* 0x2076614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][6], 0x232ba); /* 0x2076618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][7], 0x1dd7f); /* 0x207661c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][8], 0x1261e); /* 0x2076620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][9], 0x7bea); /* 0x2076624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][10], 0x2d329); /* 0x2076628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][11], 0x3f2fc); /* 0x207662c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][12], 0xb0ff); /* 0x2076630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][13], 0x302be); /* 0x2076634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][14], 0x24572); /* 0x2076638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][15], 0xc3b1); /* 0x207663c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][16], 0x275da); /* 0x2076640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][17], 0x7d51); /* 0x2076644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][18], 0x391c4); /* 0x2076648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][19], 0x1f989); /* 0x207664c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][20], 0x36359); /* 0x2076650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][21], 0x19a59); /* 0x2076654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][22], 0x320b5); /* 0x2076658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][23], 0x18a7c); /* 0x207665c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][24], 0x2abf9); /* 0x2076660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][25], 0x148aa); /* 0x2076664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][26], 0x14c94); /* 0x2076668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][27], 0x8de9); /* 0x207666c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][28], 0x33a57); /* 0x2076670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][29], 0x124e7); /* 0x2076674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][30], 0x73e9); /* 0x2076678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][31], 0x1db21); /* 0x207667c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][32], 0xe5d1); /* 0x2076680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][33], 0x24c6f); /* 0x2076684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][34], 0x1a141); /* 0x2076688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][35], 0x20475); /* 0x207668c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][36], 0x3aa54); /* 0x2076690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][37], 0x22ace); /* 0x2076694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][38], 0x3df1a); /* 0x2076698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][39], 0x1ad40); /* 0x207669c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][40], 0x139ed); /* 0x20766a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][41], 0x2c9); /* 0x20766a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][42], 0x224c1); /* 0x20766a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][43], 0x282d3); /* 0x20766ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][44], 0x1dfe2); /* 0x20766b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][45], 0x14812); /* 0x20766b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][46], 0x134ae); /* 0x20766b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][47], 0x35204); /* 0x20766bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][48], 0x12dcd); /* 0x20766c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][49], 0x1419d); /* 0x20766c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][50], 0x327a8); /* 0x20766c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][51], 0x3f627); /* 0x20766cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][0], 0x2035d); /* 0x2076700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][1], 0x38ea9); /* 0x2076704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][2], 0xb698); /* 0x2076708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][3], 0x1944c); /* 0x207670c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][4], 0x38c63); /* 0x2076710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][5], 0xfc2e); /* 0x2076714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][6], 0x11d80); /* 0x2076718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][7], 0x36da); /* 0x207671c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][8], 0x2059f); /* 0x2076720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][9], 0x18229); /* 0x2076724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][10], 0x239a4); /* 0x2076728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][11], 0x1c06e); /* 0x207672c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][12], 0x1be1f); /* 0x2076730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][13], 0x159d5); /* 0x2076734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][14], 0xbe25); /* 0x2076738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][15], 0x1ad87); /* 0x207673c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][16], 0x25323); /* 0x2076740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][17], 0x2d848); /* 0x2076744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][18], 0x3e507); /* 0x2076748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][19], 0x23d2a); /* 0x207674c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][20], 0x11873); /* 0x2076750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][21], 0x3d0c6); /* 0x2076754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][22], 0x2fb73); /* 0x2076758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][23], 0x286aa); /* 0x207675c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][24], 0x3bfe7); /* 0x2076760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][25], 0x12976); /* 0x2076764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][26], 0x39649); /* 0x2076768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][27], 0xb30c); /* 0x207676c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][28], 0x37a6c); /* 0x2076770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][29], 0x1b51f); /* 0x2076774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][30], 0x3c27b); /* 0x2076778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][31], 0x29052); /* 0x207677c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][32], 0x2a37a); /* 0x2076780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][33], 0x3eabe); /* 0x2076784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][34], 0x15704); /* 0x2076788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][35], 0x18c29); /* 0x207678c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][36], 0x3a0e8); /* 0x2076790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][37], 0x20268); /* 0x2076794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][38], 0x2bfd); /* 0x2076798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][39], 0x2672b); /* 0x207679c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][40], 0x62a4); /* 0x20767a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][41], 0x3ff2a); /* 0x20767a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][42], 0x20827); /* 0x20767a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][43], 0x1bff6); /* 0x20767ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][44], 0x2cd3e); /* 0x20767b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][45], 0x196ee); /* 0x20767b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][46], 0x272c8); /* 0x20767b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][47], 0x194ed); /* 0x20767bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][48], 0x28279); /* 0x20767c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][49], 0x6c7e); /* 0x20767c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][50], 0x85f); /* 0x20767c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][51], 0xe5f3); /* 0x20767cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][0], 0x32ca2); /* 0x2076800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][1], 0x5ab4); /* 0x2076804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][2], 0x18daa); /* 0x2076808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][3], 0x212a8); /* 0x207680c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][4], 0x2977f); /* 0x2076810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][5], 0x3ef63); /* 0x2076814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][6], 0x112aa); /* 0x2076818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][7], 0x11d96); /* 0x207681c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][8], 0x3a1ce); /* 0x2076820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][9], 0x13a37); /* 0x2076824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][10], 0xd883); /* 0x2076828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][11], 0x13742); /* 0x207682c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][12], 0x3429); /* 0x2076830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][13], 0xf5cf); /* 0x2076834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][14], 0x3de27); /* 0x2076838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][15], 0x2d1e5); /* 0x207683c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][16], 0xb14c); /* 0x2076840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][17], 0x1c98e); /* 0x2076844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][18], 0x3911e); /* 0x2076848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][19], 0x58c9); /* 0x207684c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][20], 0x30e6b); /* 0x2076850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][21], 0x23d41); /* 0x2076854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][22], 0x3414a); /* 0x2076858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][23], 0x2a602); /* 0x207685c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][24], 0x37fd5); /* 0x2076860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][25], 0xdb5); /* 0x2076864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][26], 0xd4df); /* 0x2076868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][27], 0x23f77); /* 0x207686c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][28], 0xe71d); /* 0x2076870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][29], 0x16648); /* 0x2076874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][30], 0x1a02e); /* 0x2076878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][31], 0x12343); /* 0x207687c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][32], 0x35231); /* 0x2076880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][33], 0x1edda); /* 0x2076884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][34], 0x21c37); /* 0x2076888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][35], 0x3b50); /* 0x207688c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][36], 0x3692d); /* 0x2076890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][37], 0xc84c); /* 0x2076894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][38], 0x2ea28); /* 0x2076898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][39], 0xed1d); /* 0x207689c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][40], 0x35e5b); /* 0x20768a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][41], 0x3eb8e); /* 0x20768a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][42], 0x8b6e); /* 0x20768a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][43], 0x2e9a0); /* 0x20768ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][44], 0x3d2d1); /* 0x20768b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][45], 0x4f51); /* 0x20768b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][46], 0x26d67); /* 0x20768b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][47], 0x1c6eb); /* 0x20768bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][48], 0x1dee6); /* 0x20768c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][49], 0x2576c); /* 0x20768c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][50], 0x3d131); /* 0x20768c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][51], 0x4ce8); /* 0x20768cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][0], 0x3b6ad); /* 0x2076900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][1], 0xa9ba); /* 0x2076904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][2], 0x16b34); /* 0x2076908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][3], 0x148bc); /* 0x207690c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][4], 0x20610); /* 0x2076910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][5], 0x197ca); /* 0x2076914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][6], 0xba8e); /* 0x2076918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][7], 0x27b41); /* 0x207691c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][8], 0xfa5f); /* 0x2076920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][9], 0x1fcc5); /* 0x2076924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][10], 0x213f3); /* 0x2076928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][11], 0x1c62a); /* 0x207692c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][12], 0x25647); /* 0x2076930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][13], 0x160be); /* 0x2076934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][14], 0x1573f); /* 0x2076938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][15], 0xfa46); /* 0x207693c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][16], 0x26f5d); /* 0x2076940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][17], 0x3e2bf); /* 0x2076944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][18], 0x1a844); /* 0x2076948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][19], 0x24ad); /* 0x207694c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][20], 0x38cf7); /* 0x2076950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][21], 0x2d028); /* 0x2076954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][22], 0x37b86); /* 0x2076958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][23], 0x5d6f); /* 0x207695c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][24], 0x10aae); /* 0x2076960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][25], 0x2cee4); /* 0x2076964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][26], 0x3351b); /* 0x2076968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][27], 0x23e82); /* 0x207696c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][28], 0x3170b); /* 0x2076970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][29], 0x8a3d); /* 0x2076974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][30], 0x11ed0); /* 0x2076978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][31], 0x3cb3e); /* 0x207697c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][32], 0x3b789); /* 0x2076980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][33], 0x18e62); /* 0x2076984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][34], 0xa04f); /* 0x2076988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][35], 0x300ab); /* 0x207698c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][36], 0x23e0b); /* 0x2076990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][37], 0x242c1); /* 0x2076994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][38], 0x1e070); /* 0x2076998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][39], 0x67f6); /* 0x207699c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][40], 0x2ff26); /* 0x20769a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][41], 0x37382); /* 0x20769a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][42], 0xa1df); /* 0x20769a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][43], 0x39be2); /* 0x20769ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][44], 0x3f0ae); /* 0x20769b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][45], 0x64b2); /* 0x20769b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][46], 0x38c63); /* 0x20769b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][47], 0xfac6); /* 0x20769bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][48], 0xb279); /* 0x20769c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][49], 0x16a21); /* 0x20769c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][50], 0xb9c8); /* 0x20769c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][51], 0x2004a); /* 0x20769cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][0], 0x31c47); /* 0x2076a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][1], 0x3deb4); /* 0x2076a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][2], 0xa57c); /* 0x2076a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][3], 0x34fd4); /* 0x2076a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][4], 0x1dba4); /* 0x2076a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][5], 0x2a372); /* 0x2076a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][6], 0x41a8); /* 0x2076a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][7], 0x24313); /* 0x2076a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][8], 0x120e6); /* 0x2076a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][9], 0x3e65d); /* 0x2076a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][10], 0x9286); /* 0x2076a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][11], 0x36bb3); /* 0x2076a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][12], 0x1fdd9); /* 0x2076a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][13], 0xcce0); /* 0x2076a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][14], 0x2b6eb); /* 0x2076a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][15], 0xbd27); /* 0x2076a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][16], 0x2725b); /* 0x2076a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][17], 0x1f481); /* 0x2076a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][18], 0x16fc3); /* 0x2076a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][19], 0x11074); /* 0x2076a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][20], 0x30997); /* 0x2076a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][21], 0x3ef07); /* 0x2076a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][22], 0x37a4b); /* 0x2076a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][23], 0x114d3); /* 0x2076a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][24], 0x132c6); /* 0x2076a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][25], 0x3670a); /* 0x2076a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][26], 0x29a73); /* 0x2076a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][27], 0xd8f8); /* 0x2076a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][28], 0x2f735); /* 0x2076a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][29], 0x441d); /* 0x2076a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][30], 0x242b8); /* 0x2076a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][31], 0x2aee); /* 0x2076a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][32], 0x2ac9c); /* 0x2076a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][33], 0x20b08); /* 0x2076a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][34], 0x358e7); /* 0x2076a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][35], 0x3e213); /* 0x2076a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][36], 0x6c5e); /* 0x2076a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][37], 0x11d84); /* 0x2076a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][38], 0x9e6d); /* 0x2076a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][39], 0x14a34); /* 0x2076a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][40], 0x1b082); /* 0x2076aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][41], 0x3086e); /* 0x2076aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][42], 0x83f3); /* 0x2076aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][43], 0x34682); /* 0x2076aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][44], 0x16783); /* 0x2076ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][45], 0x2cf0); /* 0x2076ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][46], 0x389e); /* 0x2076ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][47], 0x38f9b); /* 0x2076abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][48], 0x191f3); /* 0x2076ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][49], 0x2b356); /* 0x2076ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][50], 0xcf54); /* 0x2076ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][51], 0x1a24); /* 0x2076acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][0], 0x2a74a); /* 0x2076b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][1], 0x1ed36); /* 0x2076b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][2], 0x21346); /* 0x2076b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][3], 0x3db52); /* 0x2076b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][4], 0x2bdde); /* 0x2076b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][5], 0x8a13); /* 0x2076b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][6], 0x169b5); /* 0x2076b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][7], 0x3b96); /* 0x2076b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][8], 0x9ad1); /* 0x2076b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][9], 0x3da17); /* 0x2076b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][10], 0x120fb); /* 0x2076b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][11], 0x15f4e); /* 0x2076b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][12], 0x16354); /* 0x2076b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][13], 0x2f2e9); /* 0x2076b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][14], 0x2c177); /* 0x2076b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][15], 0x93ec); /* 0x2076b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][16], 0x24ac); /* 0x2076b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][17], 0x3dc65); /* 0x2076b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][18], 0x1e434); /* 0x2076b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][19], 0x15972); /* 0x2076b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][20], 0x3cb17); /* 0x2076b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][21], 0x3771d); /* 0x2076b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][22], 0x28ad4); /* 0x2076b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][23], 0x71d4); /* 0x2076b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][24], 0x3159c); /* 0x2076b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][25], 0x350ca); /* 0x2076b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][26], 0x35d85); /* 0x2076b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][27], 0x2fe72); /* 0x2076b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][28], 0x3803a); /* 0x2076b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][29], 0x3b01); /* 0x2076b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][30], 0x22e9c); /* 0x2076b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][31], 0x29221); /* 0x2076b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][32], 0x1a707); /* 0x2076b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][33], 0xa32e); /* 0x2076b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][34], 0x33a13); /* 0x2076b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][35], 0x3705a); /* 0x2076b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][36], 0x2fa97); /* 0x2076b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][37], 0x3d152); /* 0x2076b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][38], 0x1fea0); /* 0x2076b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][39], 0x1bbd); /* 0x2076b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][40], 0x10e05); /* 0x2076ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][41], 0x2241a); /* 0x2076ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][42], 0x8d05); /* 0x2076ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][43], 0x353ec); /* 0x2076bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][44], 0xf466); /* 0x2076bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][45], 0x2889e); /* 0x2076bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][46], 0x223); /* 0x2076bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][47], 0x1c936); /* 0x2076bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][48], 0x325e); /* 0x2076bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][49], 0x26392); /* 0x2076bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][50], 0x3281f); /* 0x2076bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][51], 0x1f80d); /* 0x2076bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][0], 0xcfab); /* 0x2076c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][1], 0x33c90); /* 0x2076c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][2], 0x9acf); /* 0x2076c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][3], 0x1cbd7); /* 0x2076c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][4], 0x20c66); /* 0x2076c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][5], 0x36aec); /* 0x2076c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][6], 0xeb20); /* 0x2076c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][7], 0x2c82b); /* 0x2076c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][8], 0x77a2); /* 0x2076c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][9], 0x1f38e); /* 0x2076c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][10], 0x2068b); /* 0x2076c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][11], 0x31e68); /* 0x2076c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][12], 0x372b7); /* 0x2076c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][13], 0x2f3f3); /* 0x2076c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][14], 0x10e60); /* 0x2076c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][15], 0x1939c); /* 0x2076c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][16], 0x18565); /* 0x2076c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][17], 0x33ee5); /* 0x2076c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][18], 0xbce2); /* 0x2076c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][19], 0x182d); /* 0x2076c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][20], 0x2f13e); /* 0x2076c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][21], 0x126f7); /* 0x2076c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][22], 0x29994); /* 0x2076c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][23], 0x20040); /* 0x2076c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][24], 0x2f7c0); /* 0x2076c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][25], 0x34c49); /* 0x2076c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][26], 0x3b5a2); /* 0x2076c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][27], 0x3ab78); /* 0x2076c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][28], 0x2cdf1); /* 0x2076c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][29], 0x28b09); /* 0x2076c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][30], 0x2a27a); /* 0x2076c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][31], 0x34407); /* 0x2076c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][32], 0x2974f); /* 0x2076c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][33], 0x138a4); /* 0x2076c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][34], 0x259eb); /* 0x2076c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][35], 0x29634); /* 0x2076c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][36], 0x125a0); /* 0x2076c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][37], 0x1ff5b); /* 0x2076c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][38], 0xdaff); /* 0x2076c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][39], 0x23c1); /* 0x2076c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][40], 0xdb05); /* 0x2076ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][41], 0x66b2); /* 0x2076ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][42], 0x2c7d1); /* 0x2076ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][43], 0x1750c); /* 0x2076cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][44], 0xaa4e); /* 0x2076cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][45], 0x224bf); /* 0x2076cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][46], 0x2ed1); /* 0x2076cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][47], 0x2a81a); /* 0x2076cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][48], 0x6d9a); /* 0x2076cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][49], 0x11be9); /* 0x2076cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][50], 0x38e5a); /* 0x2076cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][51], 0x17569); /* 0x2076ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][0], 0x154c0); /* 0x2076d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][1], 0x1d6c7); /* 0x2076d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][2], 0xe879); /* 0x2076d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][3], 0x25dc0); /* 0x2076d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][4], 0x3906e); /* 0x2076d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][5], 0x27dff); /* 0x2076d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][6], 0x1b9cc); /* 0x2076d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][7], 0x25900); /* 0x2076d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][8], 0x3c977); /* 0x2076d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][9], 0x1b22); /* 0x2076d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][10], 0x27f36); /* 0x2076d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][11], 0x182d1); /* 0x2076d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][12], 0x17918); /* 0x2076d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][13], 0x20059); /* 0x2076d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][14], 0x2502a); /* 0x2076d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][15], 0x3aeb); /* 0x2076d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][16], 0x4067); /* 0x2076d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][17], 0xc8d4); /* 0x2076d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][18], 0x2d389); /* 0x2076d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][19], 0x2fae3); /* 0x2076d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][20], 0x27cc8); /* 0x2076d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][21], 0x39c04); /* 0x2076d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][22], 0x3fdb); /* 0x2076d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][23], 0x136fe); /* 0x2076d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][24], 0xcbaa); /* 0x2076d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][25], 0x3ad6d); /* 0x2076d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][26], 0xa1d3); /* 0x2076d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][27], 0x1360f); /* 0x2076d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][28], 0x200c1); /* 0x2076d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][29], 0x18f9a); /* 0x2076d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][30], 0x201df); /* 0x2076d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][31], 0xbc77); /* 0x2076d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][32], 0x35de7); /* 0x2076d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][33], 0x8bc2); /* 0x2076d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][34], 0x32747); /* 0x2076d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][35], 0x3b1c7); /* 0x2076d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][36], 0x3f59f); /* 0x2076d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][37], 0x1caf7); /* 0x2076d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][38], 0x24ac3); /* 0x2076d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][39], 0x29fd0); /* 0x2076d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][40], 0x39392); /* 0x2076da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][41], 0x6793); /* 0x2076da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][42], 0x264f); /* 0x2076da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][43], 0x1692); /* 0x2076dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][44], 0xd9e); /* 0x2076db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][45], 0x5bd8); /* 0x2076db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][46], 0x1c64a); /* 0x2076db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][47], 0x1ef9a); /* 0x2076dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][48], 0x378f9); /* 0x2076dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][49], 0x139b); /* 0x2076dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][50], 0x38164); /* 0x2076dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][51], 0x2eaf2); /* 0x2076dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][0], 0x159cf); /* 0x2076e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][1], 0x3a23a); /* 0x2076e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][2], 0x16d2b); /* 0x2076e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][3], 0x8a7a); /* 0x2076e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][4], 0x21de1); /* 0x2076e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][5], 0x55f6); /* 0x2076e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][6], 0x2202d); /* 0x2076e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][7], 0x2e5ad); /* 0x2076e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][8], 0x1377b); /* 0x2076e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][9], 0x13b58); /* 0x2076e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][10], 0xd14e); /* 0x2076e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][11], 0x295e9); /* 0x2076e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][12], 0x2e53e); /* 0x2076e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][13], 0xc9d3); /* 0x2076e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][14], 0x3006e); /* 0x2076e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][15], 0x2f27); /* 0x2076e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][16], 0x9200); /* 0x2076e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][17], 0x1390b); /* 0x2076e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][18], 0x19b15); /* 0x2076e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][19], 0xeaa9); /* 0x2076e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][20], 0x24fc8); /* 0x2076e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][21], 0x2f2dd); /* 0x2076e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][22], 0xb72f); /* 0x2076e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][23], 0x2beb9); /* 0x2076e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][24], 0xfc01); /* 0x2076e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][25], 0x6318); /* 0x2076e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][26], 0xccfb); /* 0x2076e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][27], 0x25a94); /* 0x2076e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][28], 0x39015); /* 0x2076e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][29], 0x311c); /* 0x2076e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][30], 0x3af00); /* 0x2076e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][31], 0x3ebd4); /* 0x2076e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][32], 0x3b1ac); /* 0x2076e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][33], 0xea19); /* 0x2076e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][34], 0x5101); /* 0x2076e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][35], 0x1b452); /* 0x2076e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][36], 0x29596); /* 0x2076e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][37], 0x1094a); /* 0x2076e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][38], 0x246cf); /* 0x2076e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][39], 0x3f674); /* 0x2076e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][40], 0x1a673); /* 0x2076ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][41], 0x3711c); /* 0x2076ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][42], 0x8cc4); /* 0x2076ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][43], 0x27c39); /* 0x2076eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][44], 0x2de42); /* 0x2076eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][45], 0x21efe); /* 0x2076eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][46], 0x2f795); /* 0x2076eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][47], 0x2461d); /* 0x2076ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][48], 0x24e82); /* 0x2076ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][49], 0x6356); /* 0x2076ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][50], 0x2a91a); /* 0x2076ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][51], 0x18811); /* 0x2076ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][0], 0x13bf7); /* 0x2076f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][1], 0x37923); /* 0x2076f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][2], 0x3a731); /* 0x2076f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][3], 0x7183); /* 0x2076f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][4], 0x25562); /* 0x2076f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][5], 0x3093a); /* 0x2076f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][6], 0x22288); /* 0x2076f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][7], 0x1082e); /* 0x2076f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][8], 0x29c3f); /* 0x2076f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][9], 0x169db); /* 0x2076f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][10], 0x2a8b3); /* 0x2076f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][11], 0x9d7d); /* 0x2076f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][12], 0x2ef23); /* 0x2076f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][13], 0x2d050); /* 0x2076f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][14], 0x6549); /* 0x2076f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][15], 0x1f614); /* 0x2076f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][16], 0x3e395); /* 0x2076f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][17], 0x34c1); /* 0x2076f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][18], 0xe4dd); /* 0x2076f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][19], 0x2ac76); /* 0x2076f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][20], 0x23880); /* 0x2076f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][21], 0x1dc9); /* 0x2076f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][22], 0x2b14a); /* 0x2076f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][23], 0x11c14); /* 0x2076f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][24], 0x3977c); /* 0x2076f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][25], 0x2a3ae); /* 0x2076f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][26], 0xd1f); /* 0x2076f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][27], 0x34457); /* 0x2076f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][28], 0x2f381); /* 0x2076f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][29], 0x360d9); /* 0x2076f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][30], 0x1342f); /* 0x2076f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][31], 0x3af82); /* 0x2076f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][32], 0xf2a); /* 0x2076f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][33], 0x31e60); /* 0x2076f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][34], 0x2ef1d); /* 0x2076f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][35], 0x37fb2); /* 0x2076f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][36], 0x316f5); /* 0x2076f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][37], 0x200b9); /* 0x2076f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][38], 0x16d66); /* 0x2076f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][39], 0x24ca0); /* 0x2076f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][40], 0x26ed6); /* 0x2076fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][41], 0x24731); /* 0x2076fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][42], 0x15d21); /* 0x2076fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][43], 0x6375); /* 0x2076fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][44], 0x16bbe); /* 0x2076fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][45], 0x23989); /* 0x2076fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][46], 0x90c); /* 0x2076fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][47], 0x20920); /* 0x2076fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][48], 0x22eca); /* 0x2076fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][49], 0x11e98); /* 0x2076fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][50], 0x11199); /* 0x2076fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][51], 0x25656); /* 0x2076fcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][0], 0xadb5); /* 0x2077000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][1], 0x384bc); /* 0x2077004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][2], 0x3f6e1); /* 0x2077008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][3], 0x3504); /* 0x207700c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][4], 0x1958f); /* 0x2077010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][5], 0x37b48); /* 0x2077014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][6], 0x37e92); /* 0x2077018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][7], 0xaa50); /* 0x207701c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][8], 0x20e52); /* 0x2077020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][9], 0x31d99); /* 0x2077024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][10], 0x3d2ba); /* 0x2077028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][11], 0x2ecc6); /* 0x207702c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][12], 0xafbe); /* 0x2077030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][13], 0x20577); /* 0x2077034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][14], 0x3e9b2); /* 0x2077038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][15], 0x6709); /* 0x207703c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][16], 0x397ed); /* 0x2077040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][17], 0x27bd2); /* 0x2077044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][18], 0x30c2e); /* 0x2077048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][19], 0x36d10); /* 0x207704c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][20], 0x1c47c); /* 0x2077050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][21], 0x33989); /* 0x2077054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][22], 0x1cd69); /* 0x2077058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][23], 0x17c36); /* 0x207705c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][24], 0x65e); /* 0x2077060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][25], 0x12a1c); /* 0x2077064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][26], 0xf75e); /* 0x2077068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][27], 0x4626); /* 0x207706c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][28], 0x2e77f); /* 0x2077070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][29], 0x13e75); /* 0x2077074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][30], 0x30b7c); /* 0x2077078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][31], 0x39ae2); /* 0x207707c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][32], 0x30888); /* 0x2077080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][33], 0x199aa); /* 0x2077084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][34], 0x22739); /* 0x2077088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][35], 0x17091); /* 0x207708c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][36], 0xc918); /* 0x2077090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][37], 0x7a04); /* 0x2077094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][38], 0x2a6aa); /* 0x2077098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][39], 0x1d9c7); /* 0x207709c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][40], 0x9fba); /* 0x20770a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][41], 0x21c07); /* 0x20770a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][42], 0x3750e); /* 0x20770a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][43], 0x1dac8); /* 0x20770ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][44], 0x7de7); /* 0x20770b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][45], 0x8bf1); /* 0x20770b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][46], 0x26a2e); /* 0x20770b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][47], 0x3dc2d); /* 0x20770bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][48], 0x360d8); /* 0x20770c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][49], 0x3f432); /* 0x20770c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][50], 0x1ad76); /* 0x20770c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][51], 0x1488a); /* 0x20770cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][0], 0x2b5d7); /* 0x2077100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][1], 0x16a69); /* 0x2077104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][2], 0x26a07); /* 0x2077108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][3], 0x1c4bb); /* 0x207710c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][4], 0x1e341); /* 0x2077110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][5], 0x2ac85); /* 0x2077114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][6], 0x21a16); /* 0x2077118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][7], 0x4144); /* 0x207711c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][8], 0x10593); /* 0x2077120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][9], 0xe611); /* 0x2077124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][10], 0x5331); /* 0x2077128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][11], 0x27437); /* 0x207712c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][12], 0x1c2dc); /* 0x2077130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][13], 0x3147d); /* 0x2077134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][14], 0x290fd); /* 0x2077138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][15], 0x25db); /* 0x207713c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][16], 0x3c86f); /* 0x2077140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][17], 0x36bc2); /* 0x2077144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][18], 0x27727); /* 0x2077148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][19], 0x36ac4); /* 0x207714c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][20], 0x303ca); /* 0x2077150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][21], 0x2ed74); /* 0x2077154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][22], 0x2562d); /* 0x2077158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][23], 0x347b4); /* 0x207715c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][24], 0x2814); /* 0x2077160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][25], 0x23dfd); /* 0x2077164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][26], 0x23b5c); /* 0x2077168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][27], 0x123bc); /* 0x207716c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][28], 0x250f4); /* 0x2077170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][29], 0x5785); /* 0x2077174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][30], 0x1c4d5); /* 0x2077178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][31], 0x5008); /* 0x207717c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][32], 0x331b7); /* 0x2077180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][33], 0x2e6a1); /* 0x2077184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][34], 0x2e78e); /* 0x2077188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][35], 0x1a842); /* 0x207718c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][36], 0x22352); /* 0x2077190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][37], 0x1e72f); /* 0x2077194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][38], 0x14e24); /* 0x2077198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][39], 0x3f50); /* 0x207719c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][40], 0x3021b); /* 0x20771a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][41], 0x27633); /* 0x20771a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][42], 0x626a); /* 0x20771a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][43], 0xaa68); /* 0x20771ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][44], 0x3614f); /* 0x20771b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][45], 0xc8bc); /* 0x20771b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][46], 0x25693); /* 0x20771b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][47], 0xfd99); /* 0x20771bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][48], 0x34046); /* 0x20771c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][49], 0x31e12); /* 0x20771c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][50], 0x2287c); /* 0x20771c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][51], 0x1ab36); /* 0x20771cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][0], 0x3b436); /* 0x2077200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][1], 0x3ded2); /* 0x2077204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][2], 0x203b2); /* 0x2077208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][3], 0x345e9); /* 0x207720c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][4], 0xcea0); /* 0x2077210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][5], 0x3c9f); /* 0x2077214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][6], 0x4b8c); /* 0x2077218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][7], 0x31814); /* 0x207721c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][8], 0xd015); /* 0x2077220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][9], 0xf65b); /* 0x2077224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][10], 0x10818); /* 0x2077228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][11], 0x8a69); /* 0x207722c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][12], 0x239ce); /* 0x2077230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][13], 0x3636a); /* 0x2077234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][14], 0x8119); /* 0x2077238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][15], 0x6976); /* 0x207723c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][16], 0x230c8); /* 0x2077240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][17], 0x99d7); /* 0x2077244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][18], 0x3c82f); /* 0x2077248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][19], 0x1a7c3); /* 0x207724c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][20], 0xc91a); /* 0x2077250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][21], 0xd122); /* 0x2077254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][22], 0x2d5db); /* 0x2077258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][23], 0x200ab); /* 0x207725c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][24], 0x3d2eb); /* 0x2077260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][25], 0xd306); /* 0x2077264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][26], 0x23c7a); /* 0x2077268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][27], 0x13baf); /* 0x207726c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][28], 0x192ac); /* 0x2077270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][29], 0x1e888); /* 0x2077274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][30], 0x2fe10); /* 0x2077278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][31], 0x19f14); /* 0x207727c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][32], 0xd46); /* 0x2077280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][33], 0x2ecb7); /* 0x2077284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][34], 0x15bc5); /* 0x2077288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][35], 0x1eb7e); /* 0x207728c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][36], 0x2e63b); /* 0x2077290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][37], 0x2268c); /* 0x2077294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][38], 0x31164); /* 0x2077298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][39], 0x31106); /* 0x207729c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][40], 0x110da); /* 0x20772a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][41], 0xa40c); /* 0x20772a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][42], 0x2ce39); /* 0x20772a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][43], 0x25745); /* 0x20772ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][44], 0x9af2); /* 0x20772b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][45], 0xb4c5); /* 0x20772b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][46], 0x189a6); /* 0x20772b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][47], 0x164e3); /* 0x20772bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][48], 0x2a637); /* 0x20772c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][49], 0x20b76); /* 0x20772c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][50], 0x11ba); /* 0x20772c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][51], 0x37d82); /* 0x20772cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][0], 0x13d98); /* 0x2077300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][1], 0x328fb); /* 0x2077304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][2], 0x44da); /* 0x2077308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][3], 0x1b206); /* 0x207730c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][4], 0x18ab); /* 0x2077310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][5], 0x5383); /* 0x2077314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][6], 0x39d13); /* 0x2077318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][7], 0x3e1c9); /* 0x207731c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][8], 0x1b16c); /* 0x2077320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][9], 0x2135a); /* 0x2077324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][10], 0x1e317); /* 0x2077328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][11], 0x183cc); /* 0x207732c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][12], 0x2b30a); /* 0x2077330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][13], 0x189e2); /* 0x2077334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][14], 0x13441); /* 0x2077338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][15], 0x3d280); /* 0x207733c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][16], 0x178cd); /* 0x2077340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][17], 0x15ced); /* 0x2077344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][18], 0xee9d); /* 0x2077348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][19], 0x3f1f5); /* 0x207734c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][20], 0xcf44); /* 0x2077350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][21], 0xa9f0); /* 0x2077354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][22], 0x5ecd); /* 0x2077358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][23], 0x86fa); /* 0x207735c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][24], 0x1c4dc); /* 0x2077360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][25], 0x2b0a0); /* 0x2077364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][26], 0x1dc94); /* 0x2077368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][27], 0xb651); /* 0x207736c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][28], 0x195f6); /* 0x2077370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][29], 0xd9d8); /* 0x2077374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][30], 0x8ed1); /* 0x2077378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][31], 0xecf8); /* 0x207737c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][32], 0x1ead2); /* 0x2077380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][33], 0x37ae3); /* 0x2077384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][34], 0x19d0d); /* 0x2077388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][35], 0x3deea); /* 0x207738c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][36], 0xb99); /* 0x2077390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][37], 0x3df86); /* 0x2077394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][38], 0x27a1); /* 0x2077398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][39], 0x3e773); /* 0x207739c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][40], 0x38eab); /* 0x20773a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][41], 0x4905); /* 0x20773a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][42], 0x1c234); /* 0x20773a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][43], 0x19dd0); /* 0x20773ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][44], 0x3b3b7); /* 0x20773b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][45], 0x2c2c2); /* 0x20773b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][46], 0x823e); /* 0x20773b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][47], 0x2083a); /* 0x20773bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][48], 0x2d06c); /* 0x20773c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][49], 0x2b264); /* 0x20773c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][50], 0xa086); /* 0x20773c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][51], 0xdac); /* 0x20773cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][0], 0xdaa9); /* 0x2077400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][1], 0x3c8d3); /* 0x2077404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][2], 0x38d2d); /* 0x2077408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][3], 0x2de94); /* 0x207740c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][4], 0x1c90e); /* 0x2077410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][5], 0xde56); /* 0x2077414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][6], 0x24dfb); /* 0x2077418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][7], 0x2e70b); /* 0x207741c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][8], 0x32218); /* 0x2077420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][9], 0x3486b); /* 0x2077424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][10], 0x36a96); /* 0x2077428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][11], 0x931a); /* 0x207742c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][12], 0x17af0); /* 0x2077430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][13], 0x357d7); /* 0x2077434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][14], 0x11c9d); /* 0x2077438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][15], 0x3ddc2); /* 0x207743c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][16], 0xd9); /* 0x2077440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][17], 0x1ed76); /* 0x2077444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][18], 0x30e6); /* 0x2077448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][19], 0x182d1); /* 0x207744c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][20], 0x263df); /* 0x2077450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][21], 0x1c861); /* 0x2077454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][22], 0xf8b8); /* 0x2077458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][23], 0x36bbf); /* 0x207745c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][24], 0x33431); /* 0x2077460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][25], 0x5e03); /* 0x2077464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][26], 0x4be6); /* 0x2077468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][27], 0x22a2d); /* 0x207746c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][28], 0x33845); /* 0x2077470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][29], 0x304df); /* 0x2077474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][30], 0x1f693); /* 0x2077478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][31], 0xe3ae); /* 0x207747c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][32], 0x3c462); /* 0x2077480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][33], 0x3cc6c); /* 0x2077484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][34], 0x8243); /* 0x2077488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][35], 0x1549f); /* 0x207748c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][36], 0x3d4a5); /* 0x2077490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][37], 0x1aa71); /* 0x2077494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][38], 0x323c6); /* 0x2077498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][39], 0x264cf); /* 0x207749c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][40], 0x8838); /* 0x20774a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][41], 0x13d7f); /* 0x20774a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][42], 0x31859); /* 0x20774a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][43], 0x9231); /* 0x20774ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][44], 0x355cf); /* 0x20774b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][45], 0x109); /* 0x20774b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][46], 0x1e1a8); /* 0x20774b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][47], 0x4d4d); /* 0x20774bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][48], 0x2cb17); /* 0x20774c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][49], 0x293ff); /* 0x20774c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][50], 0x16a23); /* 0x20774c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][51], 0x4e2d); /* 0x20774cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][0], 0x2c3d5); /* 0x2077500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][1], 0x23715); /* 0x2077504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][2], 0x176a9); /* 0x2077508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][3], 0x8ea7); /* 0x207750c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][4], 0x30d3f); /* 0x2077510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][5], 0xd1d1); /* 0x2077514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][6], 0x2eda9); /* 0x2077518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][7], 0x3e28c); /* 0x207751c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][8], 0x26cb1); /* 0x2077520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][9], 0x22fd); /* 0x2077524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][10], 0x268f5); /* 0x2077528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][11], 0x30450); /* 0x207752c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][12], 0x274f4); /* 0x2077530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][13], 0x36df0); /* 0x2077534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][14], 0x34142); /* 0x2077538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][15], 0x21c81); /* 0x207753c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][16], 0x332cf); /* 0x2077540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][17], 0x22dc2); /* 0x2077544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][18], 0x2c57); /* 0x2077548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][19], 0x280be); /* 0x207754c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][20], 0x17671); /* 0x2077550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][21], 0x316b2); /* 0x2077554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][22], 0x3c978); /* 0x2077558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][23], 0x24cf3); /* 0x207755c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][24], 0x3e8d2); /* 0x2077560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][25], 0x2a99); /* 0x2077564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][26], 0xd3b2); /* 0x2077568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][27], 0x20819); /* 0x207756c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][28], 0xbf3e); /* 0x2077570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][29], 0x2c7b2); /* 0x2077574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][30], 0x28481); /* 0x2077578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][31], 0x11ed9); /* 0x207757c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][32], 0x3a627); /* 0x2077580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][33], 0x2fbbb); /* 0x2077584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][34], 0x23a04); /* 0x2077588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][35], 0x3afd3); /* 0x207758c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][36], 0x2618c); /* 0x2077590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][37], 0x26b86); /* 0x2077594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][38], 0x229c6); /* 0x2077598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][39], 0x253b1); /* 0x207759c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][40], 0x2c089); /* 0x20775a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][41], 0x2634e); /* 0x20775a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][42], 0x34066); /* 0x20775a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][43], 0x23ba7); /* 0x20775ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][44], 0x30e01); /* 0x20775b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][45], 0x18040); /* 0x20775b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][46], 0x84d); /* 0x20775b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][47], 0x1825e); /* 0x20775bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][48], 0x2f9f8); /* 0x20775c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][49], 0x1f567); /* 0x20775c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][50], 0x2a02b); /* 0x20775c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][51], 0x35a47); /* 0x20775cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][0], 0x33a00); /* 0x2077600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][1], 0xe593); /* 0x2077604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][2], 0x2b8b6); /* 0x2077608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][3], 0x3df49); /* 0x207760c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][4], 0x9e90); /* 0x2077610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][5], 0x1d188); /* 0x2077614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][6], 0xb7f1); /* 0x2077618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][7], 0x1a5ff); /* 0x207761c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][8], 0x29caa); /* 0x2077620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][9], 0x10267); /* 0x2077624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][10], 0x2f7e4); /* 0x2077628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][11], 0xfc03); /* 0x207762c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][12], 0x15dc2); /* 0x2077630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][13], 0x107f6); /* 0x2077634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][14], 0x233c9); /* 0x2077638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][15], 0x28d01); /* 0x207763c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][16], 0x267b6); /* 0x2077640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][17], 0x96e3); /* 0x2077644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][18], 0x594); /* 0x2077648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][19], 0x15076); /* 0x207764c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][20], 0x23a0a); /* 0x2077650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][21], 0x2680c); /* 0x2077654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][22], 0x2009e); /* 0x2077658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][23], 0x30295); /* 0x207765c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][24], 0x3e14); /* 0x2077660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][25], 0x27bc4); /* 0x2077664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][26], 0xef98); /* 0x2077668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][27], 0x2b173); /* 0x207766c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][28], 0x3f2d5); /* 0x2077670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][29], 0x3cf14); /* 0x2077674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][30], 0x2d3bb); /* 0x2077678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][31], 0x28c2a); /* 0x207767c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][32], 0x15fec); /* 0x2077680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][33], 0x25c6c); /* 0x2077684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][34], 0x68ed); /* 0x2077688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][35], 0x3922f); /* 0x207768c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][36], 0x7a36); /* 0x2077690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][37], 0x1214d); /* 0x2077694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][38], 0x212c1); /* 0x2077698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][39], 0x360ff); /* 0x207769c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][40], 0x2de19); /* 0x20776a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][41], 0x2a4); /* 0x20776a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][42], 0x11a8f); /* 0x20776a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][43], 0x91d3); /* 0x20776ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][44], 0x30e83); /* 0x20776b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][45], 0x23323); /* 0x20776b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][46], 0x2f61b); /* 0x20776b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][47], 0x38a4a); /* 0x20776bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][48], 0xc854); /* 0x20776c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][49], 0x23220); /* 0x20776c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][50], 0x527f); /* 0x20776c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][51], 0x19e8d); /* 0x20776cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][0], 0x3a5bf); /* 0x2077700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][1], 0x34f); /* 0x2077704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][2], 0xfc69); /* 0x2077708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][3], 0x580c); /* 0x207770c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][4], 0x29048); /* 0x2077710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][5], 0x69c5); /* 0x2077714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][6], 0x1d35f); /* 0x2077718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][7], 0x2a0b2); /* 0x207771c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][8], 0x20d6b); /* 0x2077720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][9], 0x378c7); /* 0x2077724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][10], 0x3280c); /* 0x2077728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][11], 0x3b865); /* 0x207772c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][12], 0x196e4); /* 0x2077730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][13], 0x2cc36); /* 0x2077734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][14], 0xd576); /* 0x2077738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][15], 0xab35); /* 0x207773c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][16], 0x35811); /* 0x2077740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][17], 0x39749); /* 0x2077744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][18], 0x9d85); /* 0x2077748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][19], 0x1e09f); /* 0x207774c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][20], 0xbc52); /* 0x2077750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][21], 0x3d2a6); /* 0x2077754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][22], 0x17582); /* 0x2077758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][23], 0x208f8); /* 0x207775c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][24], 0x2529e); /* 0x2077760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][25], 0x16912); /* 0x2077764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][26], 0x1b69a); /* 0x2077768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][27], 0x1c43c); /* 0x207776c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][28], 0x2fe61); /* 0x2077770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][29], 0x25072); /* 0x2077774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][30], 0x23745); /* 0x2077778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][31], 0x9f88); /* 0x207777c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][32], 0x21186); /* 0x2077780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][33], 0x5888); /* 0x2077784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][34], 0x3150d); /* 0x2077788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][35], 0x35a7a); /* 0x207778c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][36], 0x26498); /* 0x2077790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][37], 0x1b116); /* 0x2077794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][38], 0x55f3); /* 0x2077798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][39], 0x342f6); /* 0x207779c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][40], 0xcf00); /* 0x20777a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][41], 0x18b7); /* 0x20777a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][42], 0x15064); /* 0x20777a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][43], 0x2df73); /* 0x20777ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][44], 0x14bcb); /* 0x20777b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][45], 0x3bf9e); /* 0x20777b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][46], 0x2bd4c); /* 0x20777b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][47], 0x3060e); /* 0x20777bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][48], 0x102d8); /* 0x20777c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][49], 0x29c5a); /* 0x20777c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][50], 0x2a487); /* 0x20777c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][51], 0x16174); /* 0x20777cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][0], 0x2187a); /* 0x2077800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][1], 0x203dc); /* 0x2077804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][2], 0x9c5b); /* 0x2077808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][3], 0x1d22b); /* 0x207780c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][4], 0x182d7); /* 0x2077810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][5], 0x277d9); /* 0x2077814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][6], 0x1acd7); /* 0x2077818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][7], 0x9eb7); /* 0x207781c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][8], 0x2ca1f); /* 0x2077820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][9], 0x22e17); /* 0x2077824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][10], 0x2ed4c); /* 0x2077828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][11], 0x28e46); /* 0x207782c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][12], 0x19a2); /* 0x2077830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][13], 0xd2ba); /* 0x2077834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][14], 0x3cbca); /* 0x2077838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][15], 0x388fe); /* 0x207783c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][16], 0x2ac4c); /* 0x2077840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][17], 0x2892a); /* 0x2077844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][18], 0x1b7bd); /* 0x2077848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][19], 0x3b8a9); /* 0x207784c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][20], 0x21318); /* 0x2077850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][21], 0x338a8); /* 0x2077854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][22], 0x1e1de); /* 0x2077858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][23], 0x34f06); /* 0x207785c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][24], 0xbd0e); /* 0x2077860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][25], 0x20ead); /* 0x2077864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][26], 0x23a3e); /* 0x2077868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][27], 0x5ed0); /* 0x207786c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][28], 0xe79f); /* 0x2077870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][29], 0x1ad4); /* 0x2077874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][30], 0x3e3b4); /* 0x2077878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][31], 0x8132); /* 0x207787c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][32], 0x2336a); /* 0x2077880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][33], 0xb478); /* 0x2077884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][34], 0x324ad); /* 0x2077888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][35], 0x34c8b); /* 0x207788c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][36], 0x1bd91); /* 0x2077890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][37], 0xb18); /* 0x2077894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][38], 0x3c998); /* 0x2077898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][39], 0xdc66); /* 0x207789c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][40], 0x1165b); /* 0x20778a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][41], 0x359ec); /* 0x20778a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][42], 0x39efb); /* 0x20778a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][43], 0x3b4c8); /* 0x20778ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][44], 0x28379); /* 0x20778b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][45], 0x3dc94); /* 0x20778b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][46], 0x31507); /* 0x20778b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][47], 0xad28); /* 0x20778bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][48], 0xefda); /* 0x20778c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][49], 0x2aa16); /* 0x20778c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][50], 0x28e35); /* 0x20778c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][51], 0x27a13); /* 0x20778cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][0], 0x23600); /* 0x2077900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][1], 0x118cd); /* 0x2077904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][2], 0x356f8); /* 0x2077908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][3], 0x5134); /* 0x207790c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][4], 0x36a84); /* 0x2077910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][5], 0x3813); /* 0x2077914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][6], 0x25817); /* 0x2077918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][7], 0x2c5a2); /* 0x207791c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][8], 0x1bbc2); /* 0x2077920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][9], 0x33e5); /* 0x2077924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][10], 0x6de8); /* 0x2077928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][11], 0x15cee); /* 0x207792c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][12], 0x1af6a); /* 0x2077930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][13], 0x22618); /* 0x2077934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][14], 0x3334); /* 0x2077938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][15], 0x1a939); /* 0x207793c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][16], 0x229b); /* 0x2077940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][17], 0x38cb0); /* 0x2077944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][18], 0x17a0b); /* 0x2077948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][19], 0x3d34a); /* 0x207794c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][20], 0x247e8); /* 0x2077950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][21], 0x34d9d); /* 0x2077954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][22], 0x4d61); /* 0x2077958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][23], 0x3e327); /* 0x207795c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][24], 0x18856); /* 0x2077960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][25], 0x13d64); /* 0x2077964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][26], 0x1c291); /* 0x2077968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][27], 0x36120); /* 0x207796c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][28], 0x27001); /* 0x2077970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][29], 0x15777); /* 0x2077974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][30], 0x21990); /* 0x2077978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][31], 0x35813); /* 0x207797c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][32], 0x18359); /* 0x2077980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][33], 0x18803); /* 0x2077984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][34], 0x31186); /* 0x2077988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][35], 0x3d384); /* 0x207798c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][36], 0x18c23); /* 0x2077990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][37], 0x1a7d8); /* 0x2077994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][38], 0x3d5e5); /* 0x2077998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][39], 0xa0fb); /* 0x207799c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][40], 0x2369f); /* 0x20779a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][41], 0x21d3e); /* 0x20779a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][42], 0x18c1d); /* 0x20779a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][43], 0x99ef); /* 0x20779ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][44], 0x3c170); /* 0x20779b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][45], 0x2892c); /* 0x20779b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][46], 0x27993); /* 0x20779b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][47], 0x20a32); /* 0x20779bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][48], 0x31242); /* 0x20779c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][49], 0x1634e); /* 0x20779c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][50], 0x3d054); /* 0x20779c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][51], 0x345b2); /* 0x20779cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][0], 0x30ee5); /* 0x2077a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][1], 0xa046); /* 0x2077a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][2], 0x10989); /* 0x2077a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][3], 0x2fb03); /* 0x2077a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][4], 0xc1d9); /* 0x2077a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][5], 0x1f67f); /* 0x2077a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][6], 0x26b7e); /* 0x2077a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][7], 0x32f81); /* 0x2077a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][8], 0x2847f); /* 0x2077a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][9], 0x2cdcc); /* 0x2077a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][10], 0x3ae2); /* 0x2077a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][11], 0xbfc9); /* 0x2077a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][12], 0x25fff); /* 0x2077a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][13], 0x2e78a); /* 0x2077a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][14], 0x28ff8); /* 0x2077a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][15], 0x2b98a); /* 0x2077a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][16], 0x33126); /* 0x2077a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][17], 0x6fbc); /* 0x2077a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][18], 0x114ef); /* 0x2077a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][19], 0x2fa01); /* 0x2077a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][20], 0xffe3); /* 0x2077a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][21], 0x25674); /* 0x2077a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][22], 0x2ac83); /* 0x2077a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][23], 0x326de); /* 0x2077a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][24], 0x3b9fe); /* 0x2077a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][25], 0x9e52); /* 0x2077a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][26], 0xfe27); /* 0x2077a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][27], 0x2a2d7); /* 0x2077a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][28], 0x35490); /* 0x2077a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][29], 0x32c06); /* 0x2077a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][30], 0x6f0d); /* 0x2077a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][31], 0x836); /* 0x2077a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][32], 0x810); /* 0x2077a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][33], 0x11631); /* 0x2077a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][34], 0x12b12); /* 0x2077a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][35], 0x48c8); /* 0x2077a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][36], 0x19ae8); /* 0x2077a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][37], 0x10e48); /* 0x2077a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][38], 0x264fa); /* 0x2077a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][39], 0xfcf6); /* 0x2077a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][40], 0x163ba); /* 0x2077aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][41], 0x24e07); /* 0x2077aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][42], 0x320da); /* 0x2077aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][43], 0x19832); /* 0x2077aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][44], 0x22757); /* 0x2077ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][45], 0x1ebbd); /* 0x2077ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][46], 0x6540); /* 0x2077ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][47], 0x3f1a4); /* 0x2077abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][48], 0x22ce8); /* 0x2077ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][49], 0x37f6a); /* 0x2077ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][50], 0xe8c3); /* 0x2077ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][51], 0x35d40); /* 0x2077acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][0], 0x2fc76); /* 0x2077b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][1], 0x266a4); /* 0x2077b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][2], 0x21d20); /* 0x2077b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][3], 0x1a391); /* 0x2077b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][4], 0x2af0d); /* 0x2077b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][5], 0x13726); /* 0x2077b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][6], 0x1939a); /* 0x2077b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][7], 0x760); /* 0x2077b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][8], 0x8355); /* 0x2077b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][9], 0xbece); /* 0x2077b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][10], 0x31474); /* 0x2077b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][11], 0x2269e); /* 0x2077b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][12], 0x30860); /* 0x2077b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][13], 0x9d39); /* 0x2077b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][14], 0x1a82e); /* 0x2077b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][15], 0x3833c); /* 0x2077b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][16], 0x3db90); /* 0x2077b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][17], 0x1fa12); /* 0x2077b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][18], 0x3483d); /* 0x2077b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][19], 0x3da12); /* 0x2077b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][20], 0x138d4); /* 0x2077b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][21], 0x3a52e); /* 0x2077b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][22], 0x887d); /* 0x2077b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][23], 0x12d84); /* 0x2077b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][24], 0x3f959); /* 0x2077b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][25], 0x3d2da); /* 0x2077b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][26], 0x5e64); /* 0x2077b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][27], 0x3d116); /* 0x2077b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][28], 0x3c62a); /* 0x2077b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][29], 0x2b0a9); /* 0x2077b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][30], 0xa0c4); /* 0x2077b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][31], 0x2ed09); /* 0x2077b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][32], 0x3130a); /* 0x2077b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][33], 0x6ae6); /* 0x2077b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][34], 0x26500); /* 0x2077b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][35], 0x29563); /* 0x2077b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][36], 0x1c596); /* 0x2077b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][37], 0x32342); /* 0x2077b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][38], 0x2a860); /* 0x2077b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][39], 0x1391); /* 0x2077b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][40], 0x1ef3d); /* 0x2077ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][41], 0x3ac71); /* 0x2077ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][42], 0x37c77); /* 0x2077ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][43], 0x3e800); /* 0x2077bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][44], 0x293eb); /* 0x2077bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][45], 0x1bf20); /* 0x2077bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][46], 0x88f9); /* 0x2077bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][47], 0x1c7ae); /* 0x2077bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][48], 0xb633); /* 0x2077bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][49], 0x359f2); /* 0x2077bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][50], 0x274da); /* 0x2077bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][51], 0x2f0d5); /* 0x2077bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][0], 0x3d62a); /* 0x2077c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][1], 0x5926); /* 0x2077c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][2], 0x1bafd); /* 0x2077c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][3], 0x2d4b4); /* 0x2077c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][4], 0x27609); /* 0x2077c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][5], 0x314ba); /* 0x2077c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][6], 0x3392d); /* 0x2077c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][7], 0x2f5d9); /* 0x2077c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][8], 0xb8fd); /* 0x2077c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][9], 0x2f637); /* 0x2077c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][10], 0x1cb22); /* 0x2077c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][11], 0x1aaa4); /* 0x2077c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][12], 0x1a428); /* 0x2077c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][13], 0xd1ae); /* 0x2077c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][14], 0x15589); /* 0x2077c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][15], 0x32103); /* 0x2077c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][16], 0x3977a); /* 0x2077c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][17], 0x15026); /* 0x2077c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][18], 0x1c20f); /* 0x2077c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][19], 0x9a7e); /* 0x2077c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][20], 0x32da7); /* 0x2077c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][21], 0x3c15e); /* 0x2077c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][22], 0x31bcb); /* 0x2077c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][23], 0x8828); /* 0x2077c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][24], 0x37ff0); /* 0x2077c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][25], 0x74c7); /* 0x2077c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][26], 0x3ffcc); /* 0x2077c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][27], 0x1fd89); /* 0x2077c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][28], 0x2a1af); /* 0x2077c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][29], 0x1cbd6); /* 0x2077c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][30], 0x1858); /* 0x2077c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][31], 0x23219); /* 0x2077c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][32], 0xcf9e); /* 0x2077c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][33], 0x30db1); /* 0x2077c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][34], 0xbb41); /* 0x2077c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][35], 0x3e856); /* 0x2077c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][36], 0x162fb); /* 0x2077c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][37], 0x27a6e); /* 0x2077c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][38], 0x2dd62); /* 0x2077c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][39], 0x1aa00); /* 0x2077c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][40], 0x1f8b6); /* 0x2077ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][41], 0xa277); /* 0x2077ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][42], 0x23570); /* 0x2077ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][43], 0x37d69); /* 0x2077cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][44], 0x8b26); /* 0x2077cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][45], 0x11324); /* 0x2077cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][46], 0x26b60); /* 0x2077cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][47], 0x7b37); /* 0x2077cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][48], 0x2494b); /* 0x2077cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][49], 0x528); /* 0x2077cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][50], 0x4354); /* 0x2077cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][51], 0x15878); /* 0x2077ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][0], 0x2f212); /* 0x2077d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][1], 0x29aaf); /* 0x2077d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][2], 0x3a322); /* 0x2077d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][3], 0x1cfef); /* 0x2077d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][4], 0x212a); /* 0x2077d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][5], 0x9d0e); /* 0x2077d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][6], 0x3bffb); /* 0x2077d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][7], 0x155d3); /* 0x2077d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][8], 0x3410e); /* 0x2077d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][9], 0x13f87); /* 0x2077d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][10], 0x119c5); /* 0x2077d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][11], 0x3f918); /* 0x2077d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][12], 0x68c3); /* 0x2077d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][13], 0x2db9); /* 0x2077d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][14], 0x1a775); /* 0x2077d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][15], 0x29ce2); /* 0x2077d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][16], 0x296a4); /* 0x2077d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][17], 0x7e97); /* 0x2077d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][18], 0x2cd); /* 0x2077d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][19], 0x1e8d3); /* 0x2077d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][20], 0x3736e); /* 0x2077d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][21], 0x39ef9); /* 0x2077d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][22], 0x3a3ce); /* 0x2077d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][23], 0x24e03); /* 0x2077d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][24], 0x2ae97); /* 0x2077d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][25], 0x2e61e); /* 0x2077d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][26], 0xa61b); /* 0x2077d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][27], 0x3dfc8); /* 0x2077d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][28], 0x31ff1); /* 0x2077d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][29], 0x1c186); /* 0x2077d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][30], 0x1d14a); /* 0x2077d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][31], 0x268f7); /* 0x2077d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][32], 0x1f0b4); /* 0x2077d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][33], 0x27816); /* 0x2077d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][34], 0x1d650); /* 0x2077d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][35], 0x39468); /* 0x2077d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][36], 0x25f88); /* 0x2077d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][37], 0x88ef); /* 0x2077d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][38], 0x1f569); /* 0x2077d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][39], 0x3b57d); /* 0x2077d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][40], 0x25d6a); /* 0x2077da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][41], 0x2f984); /* 0x2077da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][42], 0x18290); /* 0x2077da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][43], 0x23e6c); /* 0x2077dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][44], 0x3f87a); /* 0x2077db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][45], 0x170be); /* 0x2077db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][46], 0x5b36); /* 0x2077db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][47], 0x21f0); /* 0x2077dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][48], 0x18664); /* 0x2077dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][49], 0xadac); /* 0x2077dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][50], 0x248f4); /* 0x2077dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][51], 0x2ca1c); /* 0x2077dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][0], 0x28b49); /* 0x2077e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][1], 0xd7ca); /* 0x2077e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][2], 0x3eb5c); /* 0x2077e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][3], 0x1c76); /* 0x2077e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][4], 0x8bb5); /* 0x2077e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][5], 0x331a8); /* 0x2077e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][6], 0x2b9ad); /* 0x2077e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][7], 0x1cba5); /* 0x2077e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][8], 0x38127); /* 0x2077e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][9], 0x35671); /* 0x2077e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][10], 0xf1bc); /* 0x2077e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][11], 0x17f70); /* 0x2077e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][12], 0x1bac3); /* 0x2077e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][13], 0x137ab); /* 0x2077e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][14], 0x188b7); /* 0x2077e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][15], 0x184db); /* 0x2077e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][16], 0x1d6cd); /* 0x2077e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][17], 0x260e3); /* 0x2077e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][18], 0x151d5); /* 0x2077e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][19], 0x2fde9); /* 0x2077e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][20], 0x178bb); /* 0x2077e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][21], 0x2f07e); /* 0x2077e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][22], 0x2a4f4); /* 0x2077e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][23], 0x26a2a); /* 0x2077e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][24], 0x35eb6); /* 0x2077e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][25], 0xaca4); /* 0x2077e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][26], 0x2b21d); /* 0x2077e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][27], 0x24109); /* 0x2077e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][28], 0x326c4); /* 0x2077e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][29], 0x26c80); /* 0x2077e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][30], 0x25e8d); /* 0x2077e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][31], 0x20ac6); /* 0x2077e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][32], 0x538e); /* 0x2077e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][33], 0x35185); /* 0x2077e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][34], 0x11888); /* 0x2077e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][35], 0x129d4); /* 0x2077e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][36], 0x59d1); /* 0x2077e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][37], 0x58c8); /* 0x2077e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][38], 0x1026f); /* 0x2077e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][39], 0x2a215); /* 0x2077e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][40], 0x7ef0); /* 0x2077ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][41], 0x246a2); /* 0x2077ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][42], 0x2453d); /* 0x2077ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][43], 0x7e05); /* 0x2077eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][44], 0x340b3); /* 0x2077eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][45], 0x34010); /* 0x2077eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][46], 0xafbb); /* 0x2077eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][47], 0x39d28); /* 0x2077ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][48], 0xcaf8); /* 0x2077ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][49], 0x32c55); /* 0x2077ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][50], 0x13006); /* 0x2077ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][51], 0x3c133); /* 0x2077ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][0], 0x283cb); /* 0x2077f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][1], 0x23ff1); /* 0x2077f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][2], 0x1643b); /* 0x2077f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][3], 0x22957); /* 0x2077f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][4], 0xbef); /* 0x2077f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][5], 0xc93a); /* 0x2077f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][6], 0x11732); /* 0x2077f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][7], 0x2cd1f); /* 0x2077f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][8], 0x35777); /* 0x2077f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][9], 0x30a73); /* 0x2077f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][10], 0x140f); /* 0x2077f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][11], 0x347e8); /* 0x2077f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][12], 0x33a92); /* 0x2077f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][13], 0xb2d0); /* 0x2077f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][14], 0x12b37); /* 0x2077f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][15], 0x5829); /* 0x2077f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][16], 0x403f); /* 0x2077f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][17], 0x32cf1); /* 0x2077f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][18], 0xe1b4); /* 0x2077f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][19], 0x2338b); /* 0x2077f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][20], 0x362ce); /* 0x2077f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][21], 0x34cfb); /* 0x2077f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][22], 0x1882b); /* 0x2077f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][23], 0x2fa5e); /* 0x2077f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][24], 0x3da14); /* 0x2077f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][25], 0x3dc5e); /* 0x2077f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][26], 0x22a2f); /* 0x2077f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][27], 0xa7fe); /* 0x2077f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][28], 0x2058a); /* 0x2077f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][29], 0x40b6); /* 0x2077f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][30], 0x15e50); /* 0x2077f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][31], 0x23ac1); /* 0x2077f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][32], 0x2025b); /* 0x2077f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][33], 0x15689); /* 0x2077f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][34], 0x35f02); /* 0x2077f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][35], 0x30cb5); /* 0x2077f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][36], 0x3677e); /* 0x2077f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][37], 0x2e82a); /* 0x2077f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][38], 0x3d5ef); /* 0x2077f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][39], 0x217b8); /* 0x2077f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][40], 0x1aae0); /* 0x2077fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][41], 0x323c6); /* 0x2077fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][42], 0x3388a); /* 0x2077fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][43], 0x709c); /* 0x2077fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][44], 0x6cc4); /* 0x2077fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][45], 0x2d306); /* 0x2077fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][46], 0x329c7); /* 0x2077fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][47], 0x1bcb7); /* 0x2077fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][48], 0x44b1); /* 0x2077fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][49], 0x25231); /* 0x2077fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][50], 0x3adca); /* 0x2077fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][51], 0x237e); /* 0x2077fcc */
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0], 0x40); /* 0x2070060 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0], 0x3000); /* 0x2070060 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0][0], 0x0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0][1], 0x30); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1], 0xe1); /* 0x2070064 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1], 0xfc03); /* 0x2070064 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1][0], 0x3); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1][1], 0xfc); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2], 0xfd); /* 0x2070068 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2], 0xfff3); /* 0x2070068 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2][0], 0xf3); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2][1], 0xff); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3], 0xff); /* 0x207006c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3], 0xffff); /* 0x207006c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3][0], 0xff); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3][1], 0xff); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4], 0x3); /* 0x2070070 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4], 0xf); /* 0x2070070 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4][0], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4][1], 0x0); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5], 0xff); /* 0x2070074 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5], 0xffff); /* 0x2070074 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5][0], 0xff); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5][1], 0xff); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6], 0xf8); /* 0x2070078 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6], 0xffc0); /* 0x2070078 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6][0], 0xc0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6][1], 0xff); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7], 0x70); /* 0x207007c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7], 0x3f00); /* 0x207007c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7][0], 0x0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7][1], 0x3f); // regs_31841 fix
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[0][0], 0x5a5d51); /* 0x2070000 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[0][1], 0x3822efb); /* 0x2070004 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[1][0], 0x1b92214); /* 0x2070008 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[1][1], 0x33fc5ee); /* 0x207000c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[2][0], 0x393c3d7); /* 0x2070010 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[2][1], 0x2d50f17); /* 0x2070014 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[3][0], 0xfabc68); /* 0x2070018 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[3][1], 0x120f748); /* 0x207001c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[4][0], 0x2f66dd0); /* 0x2070020 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[4][1], 0x1ffab61); /* 0x2070024 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[5][0], 0x3fb6684); /* 0x2070028 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[5][1], 0xaa9cee); /* 0x207002c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[6][0], 0x2e28a62); /* 0x2070030 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[6][1], 0x135ee0f); /* 0x2070034 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[7][0], 0x126cf41); /* 0x2070038 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[7][1], 0x22dbe65); /* 0x207003c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[0], 0x85); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[1], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[2], 0x26); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[3], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[4], 0x17); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[5], 0x48); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[6], 0xdd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[7], 0x34); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[8], 0x95); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[9], 0xe6); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[10], 0xb9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[11], 0xd9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[12], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[13], 0x3a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[14], 0xb5); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[15], 0xcc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[16], 0x26); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[17], 0xfd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[18], 0x90); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[19], 0x2b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[20], 0x3f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[21], 0xfa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[22], 0x79); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[23], 0x7e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[24], 0xa6); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[25], 0x74); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[26], 0xd5); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[27], 0x67); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[28], 0xe6); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[29], 0x6b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[30], 0x5); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[31], 0xb3); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[32], 0xbb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[33], 0x23); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[34], 0x1e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[35], 0xdd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[36], 0xef); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[37], 0xf5); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[38], 0xa8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[39], 0xd9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[40], 0x4a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[41], 0xfa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[42], 0xd6); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[43], 0x33); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[44], 0xd6); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[45], 0xb2); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[46], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[47], 0xfa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[48], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[49], 0x35); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[50], 0x5b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[51], 0x87); // regs_31841 fix
    tu.OutWord(&mau_reg_map.dp.hashout_ctl, 0x60ff00); /* 0x2070040 */
    tu.IndirectWrite(0x020080000152, 0x0000000000000000, 0xe00000000000ca07); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000152 d0=0x0 d1=0xe00000000000ca07 */
    tu.IndirectWrite(0x020080000152, 0x0000000000000000, 0xe00000000000ca07); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000152 d0=0x0 d1=0xe00000000000ca07 */
    tu.IndirectWrite(0x020080000152, 0x000000000da3b1fd, 0xe00000000000ca07); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000152 d0=0xda3b1fd d1=0xe00000000000ca07 */
    tu.IndirectWrite(0x0200800001eb, 0x0000000000000000, 0xb000000000006ffb); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800001eb d0=0x0 d1=0xb000000000006ffb */
    tu.IndirectWrite(0x0200800001eb, 0x0000000000000000, 0xb000000000006ffb); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800001eb d0=0x0 d1=0xb000000000006ffb */
    tu.IndirectWrite(0x0200800001eb, 0x000000000bb98171, 0xb000000000006ffb); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800001eb d0=0xbb98171 d1=0xb000000000006ffb */
    tu.IndirectWrite(0x0200800001b6, 0x0000000000000000, 0xe000000000002580); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800001b6 d0=0x0 d1=0xe000000000002580 */
    tu.IndirectWrite(0x0200800001b6, 0x0000000000000000, 0xe000000000002580); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800001b6 d0=0x0 d1=0xe000000000002580 */
    tu.IndirectWrite(0x0200800001b6, 0x0000000024d4bacc, 0xe000000000002580); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800001b6 d0=0x24d4bacc d1=0xe000000000002580 */
    tu.IndirectWrite(0x0200800002dc, 0x0000000000000000, 0xb00000000000dbbe); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800002dc d0=0x0 d1=0xb00000000000dbbe */
    tu.IndirectWrite(0x0200800002dc, 0x0000000000000000, 0xb00000000000dbbe); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800002dc d0=0x0 d1=0xb00000000000dbbe */
    tu.IndirectWrite(0x0200800002dc, 0x000000001081abea, 0xb00000000000dbbe); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800002dc d0=0x1081abea d1=0xb00000000000dbbe */
    tu.IndirectWrite(0x0200800002c4, 0x0000000000000000, 0xe00000000000ae14); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800002c4 d0=0x0 d1=0xe00000000000ae14 */
    tu.IndirectWrite(0x0200800002c4, 0x0000000000000000, 0xe00000000000ae14); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800002c4 d0=0x0 d1=0xe00000000000ae14 */
    tu.IndirectWrite(0x0200800002c4, 0x00000000283faedb, 0xe00000000000ae14); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800002c4 d0=0x283faedb d1=0xe00000000000ae14 */
    tu.IndirectWrite(0x020080000308, 0x0000000000000000, 0x0b000000c1fa0000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000308 d0=0x0 d1=0xb000000c1fa0000 */
    tu.IndirectWrite(0x020080000308, 0x0000000000000000, 0x0b000000c1fa0000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000308 d0=0x0 d1=0xb000000c1fa0000 */
    tu.IndirectWrite(0x020080000308, 0x0394e2aac0000000, 0x0b000000c1fa0000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000308 d0=0x394e2aac0000000 d1=0xb000000c1fa0000 */
    tu.IndirectWrite(0x0200800001bc, 0x0000000000000000, 0xe000000000004cd9); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800001bc d0=0x0 d1=0xe000000000004cd9 */
    tu.IndirectWrite(0x0200800001bc, 0x0000000000000000, 0xe000000000004cd9); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800001bc d0=0x0 d1=0xe000000000004cd9 */
    tu.IndirectWrite(0x0200800001bc, 0x0000000004a160cd, 0xe000000000004cd9); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800001bc d0=0x4a160cd d1=0xe000000000004cd9 */
    tu.IndirectWrite(0x020080000002, 0x0000000000000000, 0x0e0000007fd00000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000002 d0=0x0 d1=0xe0000007fd00000 */
    tu.IndirectWrite(0x020080000002, 0x0000000000000000, 0x0e0000007fd00000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000002 d0=0x0 d1=0xe0000007fd00000 */
    tu.IndirectWrite(0x020080000002, 0x06dbfce7c0000000, 0x0e0000007fd00000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000002 d0=0x6dbfce7c0000000 d1=0xe0000007fd00000 */
    tu.IndirectWrite(0x020080000354, 0x0000000000000000, 0xe0000000000005a0); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000354 d0=0x0 d1=0xe0000000000005a0 */
    tu.IndirectWrite(0x020080000354, 0x0000000000000000, 0xe0000000000005a0); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000354 d0=0x0 d1=0xe0000000000005a0 */
    tu.IndirectWrite(0x020080000354, 0x00000000332ba9f2, 0xe0000000000005a0); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000354 d0=0x332ba9f2 d1=0xe0000000000005a0 */
    tu.IndirectWrite(0x0200800000b8, 0x0000000000000000, 0xe000000000007bcf); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800000b8 d0=0x0 d1=0xe000000000007bcf */
    tu.IndirectWrite(0x0200800000b8, 0x0000000000000000, 0xe000000000007bcf); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800000b8 d0=0x0 d1=0xe000000000007bcf */
    tu.IndirectWrite(0x0200800000b8, 0x0000000036473f63, 0xe000000000007bcf); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x200800000b8 d0=0x36473f63 d1=0xe000000000007bcf */
    tu.IndirectWrite(0x020080000012, 0x0000000000000000, 0xe00000000000835a); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000012 d0=0x0 d1=0xe00000000000835a */
    tu.IndirectWrite(0x020080000012, 0x0000000000000000, 0xe00000000000835a); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000012 d0=0x0 d1=0xe00000000000835a */
    tu.IndirectWrite(0x020080000012, 0x000000000b66b975, 0xe00000000000835a); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000012 d0=0xb66b975 d1=0xe00000000000835a */
    tu.IndirectWrite(0x020080000360, 0x0000000000000000, 0xe000000000008fb9); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000360 d0=0x0 d1=0xe000000000008fb9 */
    tu.IndirectWrite(0x020080000360, 0x0000000000000000, 0xe000000000008fb9); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000360 d0=0x0 d1=0xe000000000008fb9 */
    tu.IndirectWrite(0x020080000360, 0x000000002d726de2, 0xe000000000008fb9); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM11_way0_wid0_dep0: a=0x20080000360 d0=0x2d726de2 d1=0xe000000000008fb9 */



  act_hv_translator.do_writes(&tu);
    Phv *phv_in2 = tu.phv_alloc();
    phv_in2->set_ingress();
    phv_in2->set_version(1, true);
    phv_in2->set(  0, 0xfbd493b3); 	/* [0, 0] v=1  bytes:  3-  0  #2e0# RefModel iPhv 2o */
    phv_in2->set(  1, 0x8a6fcbde); 	/* [0, 1] v=1  bytes:  7-  4  #2e0# RefModel iPhv 2o */
    phv_in2->set(  2, 0xb02573a2); 	/* [0, 2] v=1  bytes: 11-  8  #2e0# RefModel iPhv 2o */
    phv_in2->set(  3, 0x0f8e5498); 	/* [0, 3] v=1  bytes: 15- 12  #2e0# RefModel iPhv 2o */
    phv_in2->set(  4, 0x482462db); 	/* [0, 4] v=1  bytes: 19- 16  #2e0# RefModel iPhv 2o */
    phv_in2->set(  5, 0xdc507306); 	/* [0, 5] v=1  bytes: 23- 20  #2e0# RefModel iPhv 2o */
    phv_in2->set(  6, 0xd9495a19); 	/* [0, 6] v=1  bytes: 27- 24  #2e0# RefModel iPhv 2o */
    phv_in2->set(  7, 0xdbb70eda); 	/* [0, 7] v=1  bytes: 31- 28  #2e0# RefModel iPhv 2o */
    phv_in2->set(  8, 0x1403523c); 	/* [0, 8] v=1  bytes: 35- 32  #2e0# RefModel iPhv 2o */
    phv_in2->set(  9, 0xbf2828ab); 	/* [0, 9] v=1  bytes: 39- 36  #2e0# RefModel iPhv 2o */
    phv_in2->set( 10, 0xa9a5c5c3); 	/* [0,10] v=1  bytes: 43- 40  #2e0# RefModel iPhv 2o */
    phv_in2->set( 11, 0xfea457f0); 	/* [0,11] v=1  bytes: 47- 44  #2e0# RefModel iPhv 2o */
    phv_in2->set( 12, 0xfbc0037f); 	/* [0,12] v=1  bytes: 51- 48  #2e0# RefModel iPhv 2o */
    phv_in2->set( 13, 0x1b8d0b18); 	/* [0,13] v=1  bytes: 55- 52  #2e0# RefModel iPhv 2o */
    phv_in2->set( 14, 0x649f4238); 	/* [0,14] v=1  bytes: 59- 56  #2e0# RefModel iPhv 2o */
    phv_in2->set( 15, 0x87bb3855); 	/* [0,15] v=1  bytes: 63- 60  #2e0# RefModel iPhv 2o */
    phv_in2->set( 16, 0x51e9651e); 	/* [0,16] v=1  bytes: 67- 64  #2e0# RefModel iPhv 2o */
    phv_in2->set( 17, 0xe845bbcd); 	/* [0,17] v=1  bytes: 71- 68  #2e0# RefModel iPhv 2o */
    phv_in2->set( 18, 0xb2a8788f); 	/* [0,18] v=1  bytes: 75- 72  #2e0# RefModel iPhv 2o */
    phv_in2->set( 19, 0xea3cf1bb); 	/* [0,19] v=1  bytes: 79- 76  #2e0# RefModel iPhv 2o */
    phv_in2->set( 20, 0x247d5058); 	/* [0,20] v=1  bytes: 83- 80  #2e0# RefModel iPhv 2o */
    phv_in2->set( 21, 0x6d302a90); 	/* [0,21] v=1  bytes: 87- 84  #2e0# RefModel iPhv 2o */
    phv_in2->set( 22, 0x21a56f04); 	/* [0,22] v=1  bytes: 91- 88  #2e0# RefModel iPhv 2o */
    phv_in2->set( 23, 0x1a726ad1); 	/* [0,23] v=1  bytes: 95- 92  #2e0# RefModel iPhv 2o */
    phv_in2->set( 24, 0x900e76ef); 	/* [0,24] v=1  bytes: 99- 96  #2e0# RefModel iPhv 2o */
    phv_in2->set( 25, 0x508073f6); 	/* [0,25] v=1  bytes:103-100  #2e0# RefModel iPhv 2o */
    phv_in2->set( 26, 0x11bfd4c4); 	/* [0,26] v=1  bytes:107-104  #2e0# RefModel iPhv 2o */
    phv_in2->set( 27, 0x2ae1d21c); 	/* [0,27] v=1  bytes:111-108  #2e0# RefModel iPhv 2o */
    phv_in2->set( 28, 0xdafc5b69); 	/* [0,28] v=1  bytes:115-112  #2e0# RefModel iPhv 2o */
    phv_in2->set( 29, 0xbe57155e); 	/* [0,29] v=1  bytes:119-116  #2e0# RefModel iPhv 2o */
    phv_in2->set( 30, 0x12da7907); 	/* [0,30] v=1  bytes:123-120  #2e0# RefModel iPhv 2o */
    phv_in2->set( 31, 0x88afabb0); 	/* [0,31] v=1  bytes:127-124  #2e0# RefModel iPhv 2o */
    phv_in2->set( 32, 0x4f65009c); 	/* [1, 0] v=1  bytes:131-128  #2e0# RefModel iPhv 2o */
    phv_in2->set( 33, 0x460aad6c); 	/* [1, 1] v=1  bytes:135-132  #2e0# RefModel iPhv 2o */
    phv_in2->set( 34, 0xb7293554); 	/* [1, 2] v=1  bytes:139-136  #2e0# RefModel iPhv 2o */
    phv_in2->set( 35, 0x221032f3); 	/* [1, 3] v=1  bytes:143-140  #2e0# RefModel iPhv 2o */
    phv_in2->set( 36, 0x674e4ae2); 	/* [1, 4] v=1  bytes:147-144  #2e0# RefModel iPhv 2o */
    phv_in2->set( 37, 0xe207a2a7); 	/* [1, 5] v=1  bytes:151-148  #2e0# RefModel iPhv 2o */
    phv_in2->set( 38, 0xfde32ca3); 	/* [1, 6] v=1  bytes:155-152  #2e0# RefModel iPhv 2o */
    phv_in2->set( 39, 0x742ded5b); 	/* [1, 7] v=1  bytes:159-156  #2e0# RefModel iPhv 2o */
    phv_in2->set( 40, 0xbac47aa4); 	/* [1, 8] v=1  bytes:163-160  #2e0# RefModel iPhv 2o */
    phv_in2->set( 41, 0x280a9117); 	/* [1, 9] v=1  bytes:167-164  #2e0# RefModel iPhv 2o */
    phv_in2->set( 42, 0xacedd306); 	/* [1,10] v=1  bytes:171-168  #2e0# RefModel iPhv 2o */
    phv_in2->set( 43, 0x2a77d22e); 	/* [1,11] v=1  bytes:175-172  #2e0# RefModel iPhv 2o */
    phv_in2->set( 44, 0xf6b76ad7); 	/* [1,12] v=1  bytes:179-176  #2e0# RefModel iPhv 2o */
    phv_in2->set( 45, 0x4a325c9d); 	/* [1,13] v=1  bytes:183-180  #2e0# RefModel iPhv 2o */
    phv_in2->set( 46, 0x36b525ac); 	/* [1,14] v=1  bytes:187-184  #2e0# RefModel iPhv 2o */
    phv_in2->set( 47, 0x0af2ac90); 	/* [1,15] v=1  bytes:191-188  #2e0# RefModel iPhv 2o */
    phv_in2->set( 48, 0x6378d9fa); 	/* [1,16] v=1  bytes:195-192  #2e0# RefModel iPhv 2o */
    phv_in2->set( 49, 0xcae0bc16); 	/* [1,17] v=1  bytes:199-196  #2e0# RefModel iPhv 2o */
    phv_in2->set( 50, 0xc4aced5a); 	/* [1,18] v=1  bytes:203-200  #2e0# RefModel iPhv 2o */
    phv_in2->set( 51, 0x9baa5fbe); 	/* [1,19] v=1  bytes:207-204  #2e0# RefModel iPhv 2o */
    phv_in2->set( 52, 0xa9b0ac52); 	/* [1,20] v=1  bytes:211-208  #2e0# RefModel iPhv 2o */
    phv_in2->set( 53, 0xa1cf1c1a); 	/* [1,21] v=1  bytes:215-212  #2e0# RefModel iPhv 2o */
    phv_in2->set( 54, 0x73c84a44); 	/* [1,22] v=1  bytes:219-216  #2e0# RefModel iPhv 2o */
    phv_in2->set( 55, 0x79d2c126); 	/* [1,23] v=1  bytes:223-220  #2e0# RefModel iPhv 2o */
    phv_in2->set( 56, 0x7f71272b); 	/* [1,24] v=1  bytes:227-224  #2e0# RefModel iPhv 2o */
    phv_in2->set( 57, 0x23a3f799); 	/* [1,25] v=1  bytes:231-228  #2e0# RefModel iPhv 2o */
    phv_in2->set( 58, 0xc2f1f559); 	/* [1,26] v=1  bytes:235-232  #2e0# RefModel iPhv 2o */
    phv_in2->set( 59, 0xdc95c0c0); 	/* [1,27] v=1  bytes:239-236  #2e0# RefModel iPhv 2o */
    phv_in2->set( 60, 0x684e4d0f); 	/* [1,28] v=1  bytes:243-240  #2e0# RefModel iPhv 2o */
    phv_in2->set( 61, 0x7bc598f5); 	/* [1,29] v=1  bytes:247-244  #2e0# RefModel iPhv 2o */
    phv_in2->set( 62, 0x41457901); 	/* [1,30] v=1  bytes:251-248  #2e0# RefModel iPhv 2o */
    phv_in2->set( 63, 0x0c6cbb12); 	/* [1,31] v=1  bytes:255-252  #2e0# RefModel iPhv 2o */
    phv_in2->set( 64, 0xc7); 	/* [2, 0] v=1  bytes:256     #2e0# RefModel iPhv 2o */
    phv_in2->set( 65, 0x7a); 	/* [2, 1] v=1  bytes:257     #2e0# RefModel iPhv 2o */
    phv_in2->set( 66, 0x3f); 	/* [2, 2] v=1  bytes:258     #2e0# RefModel iPhv 2o */
    phv_in2->set( 67, 0x12); 	/* [2, 3] v=1  bytes:259     #2e0# RefModel iPhv 2o */
    phv_in2->set( 68, 0xd4); 	/* [2, 4] v=1  bytes:260     #2e0# RefModel iPhv 2o */
    phv_in2->set( 69, 0xc8); 	/* [2, 5] v=1  bytes:261     #2e0# RefModel iPhv 2o */
    phv_in2->set( 70, 0x26); 	/* [2, 6] v=1  bytes:262     #2e0# RefModel iPhv 2o */
    phv_in2->set( 71, 0x1e); 	/* [2, 7] v=1  bytes:263     #2e0# RefModel iPhv 2o */
    phv_in2->set( 72, 0xc3); 	/* [2, 8] v=1  bytes:264     #2e0# RefModel iPhv 2o */
    phv_in2->set( 73, 0x6c); 	/* [2, 9] v=1  bytes:265     #2e0# RefModel iPhv 2o */
    phv_in2->set( 74, 0xe7); 	/* [2,10] v=1  bytes:266     #2e0# RefModel iPhv 2o */
    phv_in2->set( 75, 0x84); 	/* [2,11] v=1  bytes:267     #2e0# RefModel iPhv 2o */
    phv_in2->set( 76, 0xa4); 	/* [2,12] v=1  bytes:268     #2e0# RefModel iPhv 2o */
    phv_in2->set( 77, 0xd5); 	/* [2,13] v=1  bytes:269     #2e0# RefModel iPhv 2o */
    phv_in2->set( 78, 0x3e); 	/* [2,14] v=1  bytes:270     #2e0# RefModel iPhv 2o */
    phv_in2->set( 79, 0x15); 	/* [2,15] v=1  bytes:271     #2e0# RefModel iPhv 2o */
    phv_in2->set( 80, 0xb7); 	/* [2,16] v=1  bytes:272     #2e0# RefModel iPhv 2o */
    phv_in2->set( 81, 0x77); 	/* [2,17] v=1  bytes:273     #2e0# RefModel iPhv 2o */
    phv_in2->set( 82, 0x04); 	/* [2,18] v=1  bytes:274     #2e0# RefModel iPhv 2o */
    phv_in2->set( 83, 0xb8); 	/* [2,19] v=1  bytes:275     #2e0# RefModel iPhv 2o */
    phv_in2->set( 84, 0x05); 	/* [2,20] v=1  bytes:276     #2e0# RefModel iPhv 2o */
    phv_in2->set( 85, 0xae); 	/* [2,21] v=1  bytes:277     #2e0# RefModel iPhv 2o */
    phv_in2->set( 86, 0x5e); 	/* [2,22] v=1  bytes:278     #2e0# RefModel iPhv 2o */
    phv_in2->set( 87, 0xf9); 	/* [2,23] v=1  bytes:279     #2e0# RefModel iPhv 2o */
    phv_in2->set( 88, 0xf9); 	/* [2,24] v=1  bytes:280     #2e0# RefModel iPhv 2o */
    phv_in2->set( 89, 0x28); 	/* [2,25] v=1  bytes:281     #2e0# RefModel iPhv 2o */
    phv_in2->set( 90, 0xeb); 	/* [2,26] v=1  bytes:282     #2e0# RefModel iPhv 2o */
    phv_in2->set( 91, 0xe1); 	/* [2,27] v=1  bytes:283     #2e0# RefModel iPhv 2o */
    phv_in2->set( 92, 0xc3); 	/* [2,28] v=1  bytes:284     #2e0# RefModel iPhv 2o */
    phv_in2->set( 93, 0xbf); 	/* [2,29] v=1  bytes:285     #2e0# RefModel iPhv 2o */
    phv_in2->set( 94, 0xee); 	/* [2,30] v=1  bytes:286     #2e0# RefModel iPhv 2o */
    phv_in2->set( 95, 0xd5); 	/* [2,31] v=1  bytes:287     #2e0# RefModel iPhv 2o */
    phv_in2->set( 96, 0xcc); 	/* [3, 0] v=1  bytes:288     #2e0# RefModel iPhv 2o */
    phv_in2->set( 97, 0xa7); 	/* [3, 1] v=1  bytes:289     #2e0# RefModel iPhv 2o */
    phv_in2->set( 98, 0x27); 	/* [3, 2] v=1  bytes:290     #2e0# RefModel iPhv 2o */
    phv_in2->set( 99, 0x31); 	/* [3, 3] v=1  bytes:291     #2e0# RefModel iPhv 2o */
    phv_in2->set(100, 0xb8); 	/* [3, 4] v=1  bytes:292     #2e0# RefModel iPhv 2o */
    phv_in2->set(101, 0xb7); 	/* [3, 5] v=1  bytes:293     #2e0# RefModel iPhv 2o */
    phv_in2->set(102, 0x12); 	/* [3, 6] v=1  bytes:294     #2e0# RefModel iPhv 2o */
    phv_in2->set(103, 0x7a); 	/* [3, 7] v=1  bytes:295     #2e0# RefModel iPhv 2o */
    phv_in2->set(104, 0xfd); 	/* [3, 8] v=1  bytes:296     #2e0# RefModel iPhv 2o */
    phv_in2->set(105, 0x60); 	/* [3, 9] v=1  bytes:297     #2e0# RefModel iPhv 2o */
    phv_in2->set(106, 0x9e); 	/* [3,10] v=1  bytes:298     #2e0# RefModel iPhv 2o */
    phv_in2->set(107, 0xbb); 	/* [3,11] v=1  bytes:299     #2e0# RefModel iPhv 2o */
    phv_in2->set(108, 0xe2); 	/* [3,12] v=1  bytes:300     #2e0# RefModel iPhv 2o */
    phv_in2->set(109, 0x0f); 	/* [3,13] v=1  bytes:301     #2e0# RefModel iPhv 2o */
    phv_in2->set(110, 0x98); 	/* [3,14] v=1  bytes:302     #2e0# RefModel iPhv 2o */
    phv_in2->set(111, 0xdd); 	/* [3,15] v=1  bytes:303     #2e0# RefModel iPhv 2o */
    phv_in2->set(112, 0x94); 	/* [3,16] v=1  bytes:304     #2e0# RefModel iPhv 2o */
    phv_in2->set(113, 0x60); 	/* [3,17] v=1  bytes:305     #2e0# RefModel iPhv 2o */
    phv_in2->set(114, 0xf8); 	/* [3,18] v=1  bytes:306     #2e0# RefModel iPhv 2o */
    phv_in2->set(115, 0x62); 	/* [3,19] v=1  bytes:307     #2e0# RefModel iPhv 2o */
    phv_in2->set(116, 0x14); 	/* [3,20] v=1  bytes:308     #2e0# RefModel iPhv 2o */
    phv_in2->set(117, 0x7b); 	/* [3,21] v=1  bytes:309     #2e0# RefModel iPhv 2o */
    phv_in2->set(118, 0x1e); 	/* [3,22] v=1  bytes:310     #2e0# RefModel iPhv 2o */
    phv_in2->set(119, 0x86); 	/* [3,23] v=1  bytes:311     #2e0# RefModel iPhv 2o */
    phv_in2->set(120, 0x83); 	/* [3,24] v=1  bytes:312     #2e0# RefModel iPhv 2o */
    phv_in2->set(121, 0x6b); 	/* [3,25] v=1  bytes:313     #2e0# RefModel iPhv 2o */
    phv_in2->set(122, 0x5a); 	/* [3,26] v=1  bytes:314     #2e0# RefModel iPhv 2o */
    phv_in2->set(123, 0x51); 	/* [3,27] v=1  bytes:315     #2e0# RefModel iPhv 2o */
    phv_in2->set(124, 0x7a); 	/* [3,28] v=1  bytes:316     #2e0# RefModel iPhv 2o */
    phv_in2->set(125, 0xf8); 	/* [3,29] v=1  bytes:317     #2e0# RefModel iPhv 2o */
    phv_in2->set(126, 0xf8); 	/* [3,30] v=1  bytes:318     #2e0# RefModel iPhv 2o */
    phv_in2->set(127, 0x8f); 	/* [3,31] v=1  bytes:319     #2e0# RefModel iPhv 2o */
    phv_in2->set(128, 0x8da8); 	/* [4, 0] v=1  bytes:321-320  #2e0# RefModel iPhv 2o */
    phv_in2->set(129, 0xe863); 	/* [4, 1] v=1  bytes:323-322  #2e0# RefModel iPhv 2o */
    phv_in2->set(130, 0x2e1c); 	/* [4, 2] v=1  bytes:325-324  #2e0# RefModel iPhv 2o */
    phv_in2->set(131, 0x2d92); 	/* [4, 3] v=1  bytes:327-326  #2e0# RefModel iPhv 2o */
    phv_in2->set(132, 0xf62a); 	/* [4, 4] v=1  bytes:329-328  #2e0# RefModel iPhv 2o */
    phv_in2->set(133, 0x4b42); 	/* [4, 5] v=1  bytes:331-330  #2e0# RefModel iPhv 2o */
    phv_in2->set(134, 0x91d3); 	/* [4, 6] v=1  bytes:333-332  #2e0# RefModel iPhv 2o */
    phv_in2->set(135, 0x8b11); 	/* [4, 7] v=1  bytes:335-334  #2e0# RefModel iPhv 2o */
    phv_in2->set(136, 0x28d9); 	/* [4, 8] v=1  bytes:337-336  #2e0# RefModel iPhv 2o */
    phv_in2->set(137, 0xff99); 	/* [4, 9] v=1  bytes:339-338  #2e0# RefModel iPhv 2o */
    phv_in2->set(138, 0xaff4); 	/* [4,10] v=1  bytes:341-340  #2e0# RefModel iPhv 2o */
    phv_in2->set(139, 0xefcb); 	/* [4,11] v=1  bytes:343-342  #2e0# RefModel iPhv 2o */
    phv_in2->set(140, 0x9a09); 	/* [4,12] v=1  bytes:345-344  #2e0# RefModel iPhv 2o */
    phv_in2->set(141, 0x4877); 	/* [4,13] v=1  bytes:347-346  #2e0# RefModel iPhv 2o */
    phv_in2->set(142, 0x1fc6); 	/* [4,14] v=1  bytes:349-348  #2e0# RefModel iPhv 2o */
    phv_in2->set(143, 0x1de3); 	/* [4,15] v=1  bytes:351-350  #2e0# RefModel iPhv 2o */
    phv_in2->set(144, 0x7481); 	/* [4,16] v=1  bytes:353-352  #2e0# RefModel iPhv 2o */
    phv_in2->set(145, 0x5005); 	/* [4,17] v=1  bytes:355-354  #2e0# RefModel iPhv 2o */
    phv_in2->set(146, 0x1b13); 	/* [4,18] v=1  bytes:357-356  #2e0# RefModel iPhv 2o */
    phv_in2->set(147, 0xb381); 	/* [4,19] v=1  bytes:359-358  #2e0# RefModel iPhv 2o */
    phv_in2->set(148, 0x057d); 	/* [4,20] v=1  bytes:361-360  #2e0# RefModel iPhv 2o */
    phv_in2->set(149, 0x5264); 	/* [4,21] v=1  bytes:363-362  #2e0# RefModel iPhv 2o */
    phv_in2->set(150, 0xf250); 	/* [4,22] v=1  bytes:365-364  #2e0# RefModel iPhv 2o */
    phv_in2->set(151, 0xec08); 	/* [4,23] v=1  bytes:367-366  #2e0# RefModel iPhv 2o */
    phv_in2->set(152, 0x2817); 	/* [4,24] v=1  bytes:369-368  #2e0# RefModel iPhv 2o */
    phv_in2->set(153, 0xc0e2); 	/* [4,25] v=1  bytes:371-370  #2e0# RefModel iPhv 2o */
    phv_in2->set(154, 0xab11); 	/* [4,26] v=1  bytes:373-372  #2e0# RefModel iPhv 2o */
    phv_in2->set(155, 0x3a88); 	/* [4,27] v=1  bytes:375-374  #2e0# RefModel iPhv 2o */
    phv_in2->set(156, 0xc4fa); 	/* [4,28] v=1  bytes:377-376  #2e0# RefModel iPhv 2o */
    phv_in2->set(157, 0x88fd); 	/* [4,29] v=1  bytes:379-378  #2e0# RefModel iPhv 2o */
    phv_in2->set(158, 0x79f7); 	/* [4,30] v=1  bytes:381-380  #2e0# RefModel iPhv 2o */
    phv_in2->set(159, 0x50ed); 	/* [4,31] v=1  bytes:383-382  #2e0# RefModel iPhv 2o */
    phv_in2->set(160, 0x0bc9); 	/* [5, 0] v=1  bytes:385-384  #2e0# RefModel iPhv 2o */
    phv_in2->set(161, 0x7bd0); 	/* [5, 1] v=1  bytes:387-386  #2e0# RefModel iPhv 2o */
    phv_in2->set(162, 0x014c); 	/* [5, 2] v=1  bytes:389-388  #2e0# RefModel iPhv 2o */
    phv_in2->set(163, 0x00c0); 	/* [5, 3] v=1  bytes:391-390  #2e0# RefModel iPhv 2o */
    phv_in2->set(164, 0x6b4a); 	/* [5, 4] v=1  bytes:393-392  #2e0# RefModel iPhv 2o */
    phv_in2->set(165, 0xa792); 	/* [5, 5] v=1  bytes:395-394  #2e0# RefModel iPhv 2o */
    phv_in2->set(166, 0xed7c); 	/* [5, 6] v=1  bytes:397-396  #2e0# RefModel iPhv 2o */
    phv_in2->set(167, 0x28b9); 	/* [5, 7] v=1  bytes:399-398  #2e0# RefModel iPhv 2o */
    phv_in2->set(168, 0xd31b); 	/* [5, 8] v=1  bytes:401-400  #2e0# RefModel iPhv 2o */
    phv_in2->set(169, 0x2592); 	/* [5, 9] v=1  bytes:403-402  #2e0# RefModel iPhv 2o */
    phv_in2->set(170, 0x3ad4); 	/* [5,10] v=1  bytes:405-404  #2e0# RefModel iPhv 2o */
    phv_in2->set(171, 0xc261); 	/* [5,11] v=1  bytes:407-406  #2e0# RefModel iPhv 2o */
    phv_in2->set(172, 0x76f5); 	/* [5,12] v=1  bytes:409-408  #2e0# RefModel iPhv 2o */
    phv_in2->set(173, 0xbc64); 	/* [5,13] v=1  bytes:411-410  #2e0# RefModel iPhv 2o */
    phv_in2->set(174, 0xab92); 	/* [5,14] v=1  bytes:413-412  #2e0# RefModel iPhv 2o */
    phv_in2->set(175, 0x10f3); 	/* [5,15] v=1  bytes:415-414  #2e0# RefModel iPhv 2o */
    phv_in2->set(176, 0xbef6); 	/* [5,16] v=1  bytes:417-416  #2e0# RefModel iPhv 2o */
    phv_in2->set(177, 0xd753); 	/* [5,17] v=1  bytes:419-418  #2e0# RefModel iPhv 2o */
    phv_in2->set(178, 0x2861); 	/* [5,18] v=1  bytes:421-420  #2e0# RefModel iPhv 2o */
    phv_in2->set(179, 0xff87); 	/* [5,19] v=1  bytes:423-422  #2e0# RefModel iPhv 2o */
    phv_in2->set(180, 0x0176); 	/* [5,20] v=1  bytes:425-424  #2e0# RefModel iPhv 2o */
    phv_in2->set(181, 0x68f4); 	/* [5,21] v=1  bytes:427-426  #2e0# RefModel iPhv 2o */
    phv_in2->set(182, 0x2c03); 	/* [5,22] v=1  bytes:429-428  #2e0# RefModel iPhv 2o */
    phv_in2->set(183, 0x2491); 	/* [5,23] v=1  bytes:431-430  #2e0# RefModel iPhv 2o */
    phv_in2->set(184, 0x7717); 	/* [5,24] v=1  bytes:433-432  #2e0# RefModel iPhv 2o */
    phv_in2->set(185, 0x4c10); 	/* [5,25] v=1  bytes:435-434  #2e0# RefModel iPhv 2o */
    phv_in2->set(186, 0xe4c4); 	/* [5,26] v=1  bytes:437-436  #2e0# RefModel iPhv 2o */
    phv_in2->set(187, 0x3b85); 	/* [5,27] v=1  bytes:439-438  #2e0# RefModel iPhv 2o */
    phv_in2->set(188, 0x3684); 	/* [5,28] v=1  bytes:441-440  #2e0# RefModel iPhv 2o */
    phv_in2->set(189, 0x94d5); 	/* [5,29] v=1  bytes:443-442  #2e0# RefModel iPhv 2o */
    phv_in2->set(190, 0x4db3); 	/* [5,30] v=1  bytes:445-444  #2e0# RefModel iPhv 2o */
    phv_in2->set(191, 0x5347); 	/* [5,31] v=1  bytes:447-446  #2e0# RefModel iPhv 2o */
    phv_in2->set(192, 0x289a); 	/* [6, 0] v=1  bytes:449-448  #2e0# RefModel iPhv 2o */
    phv_in2->set(193, 0x242b); 	/* [6, 1] v=1  bytes:451-450  #2e0# RefModel iPhv 2o */
    phv_in2->set(194, 0xb849); 	/* [6, 2] v=1  bytes:453-452  #2e0# RefModel iPhv 2o */
    phv_in2->set(195, 0x08f8); 	/* [6, 3] v=1  bytes:455-454  #2e0# RefModel iPhv 2o */
    phv_in2->set(196, 0x38d8); 	/* [6, 4] v=1  bytes:457-456  #2e0# RefModel iPhv 2o */
    phv_in2->set(197, 0x32ad); 	/* [6, 5] v=1  bytes:459-458  #2e0# RefModel iPhv 2o */
    phv_in2->set(198, 0x91ee); 	/* [6, 6] v=1  bytes:461-460  #2e0# RefModel iPhv 2o */
    phv_in2->set(199, 0x23a1); 	/* [6, 7] v=1  bytes:463-462  #2e0# RefModel iPhv 2o */
    phv_in2->set(200, 0x6f90); 	/* [6, 8] v=1  bytes:465-464  #2e0# RefModel iPhv 2o */
    phv_in2->set(201, 0x425d); 	/* [6, 9] v=1  bytes:467-466  #2e0# RefModel iPhv 2o */
    phv_in2->set(202, 0x698b); 	/* [6,10] v=1  bytes:469-468  #2e0# RefModel iPhv 2o */
    phv_in2->set(203, 0x67ff); 	/* [6,11] v=1  bytes:471-470  #2e0# RefModel iPhv 2o */
    phv_in2->set(204, 0xecf0); 	/* [6,12] v=1  bytes:473-472  #2e0# RefModel iPhv 2o */
    phv_in2->set(205, 0xfd44); 	/* [6,13] v=1  bytes:475-474  #2e0# RefModel iPhv 2o */
    phv_in2->set(206, 0x9c3f); 	/* [6,14] v=1  bytes:477-476  #2e0# RefModel iPhv 2o */
    phv_in2->set(207, 0x83e9); 	/* [6,15] v=1  bytes:479-478  #2e0# RefModel iPhv 2o */
    phv_in2->set(208, 0x9bf7); 	/* [6,16] v=1  bytes:481-480  #2e0# RefModel iPhv 2o */
    phv_in2->set(209, 0xc039); 	/* [6,17] v=1  bytes:483-482  #2e0# RefModel iPhv 2o */
    phv_in2->set(210, 0xbfb5); 	/* [6,18] v=1  bytes:485-484  #2e0# RefModel iPhv 2o */
    phv_in2->set(211, 0xbca6); 	/* [6,19] v=1  bytes:487-486  #2e0# RefModel iPhv 2o */
    phv_in2->set(212, 0x9999); 	/* [6,20] v=1  bytes:489-488  #2e0# RefModel iPhv 2o */
    phv_in2->set(213, 0x734b); 	/* [6,21] v=1  bytes:491-490  #2e0# RefModel iPhv 2o */
    phv_in2->set(214, 0x8ff0); 	/* [6,22] v=1  bytes:493-492  #2e0# RefModel iPhv 2o */
    phv_in2->set(215, 0xdc9b); 	/* [6,23] v=1  bytes:495-494  #2e0# RefModel iPhv 2o */
    phv_in2->set(216, 0xa5f0); 	/* [6,24] v=1  bytes:497-496  #2e0# RefModel iPhv 2o */
    phv_in2->set(217, 0x230b); 	/* [6,25] v=1  bytes:499-498  #2e0# RefModel iPhv 2o */
    phv_in2->set(218, 0xf1ed); 	/* [6,26] v=1  bytes:501-500  #2e0# RefModel iPhv 2o */
    phv_in2->set(219, 0x784b); 	/* [6,27] v=1  bytes:503-502  #2e0# RefModel iPhv 2o */
    phv_in2->set(220, 0x5d7d); 	/* [6,28] v=1  bytes:505-504  #2e0# RefModel iPhv 2o */
    phv_in2->set(221, 0x0dec); 	/* [6,29] v=1  bytes:507-506  #2e0# RefModel iPhv 2o */
    phv_in2->set(222, 0xf71b); 	/* [6,30] v=1  bytes:509-508  #2e0# RefModel iPhv 2o */
    phv_in2->set(223, 0x4d03); 	/* [6,31] v=1  bytes:511-510  #2e0# RefModel iPhv 2o */

    // Make an ophv too, identical to phv_in2 but with a slightly diff version
    Phv *ophv_in2 = phv_in2->clone(true);
    ophv_in2->set_version(2, true);



    // Setup a port on pipe 0
    // This also sets up basic config ingress parser and deparser
    Port *port = tu.port_get(16);

    /*
    RMT_UT_LOG_INFO("Lookup PHV of standard packet...\n");
    // Construct a packet - DA,SA,SIP,DIP,Proto,SPort,DPort
    Packet *p_in = tu.packet_make("08:00:22:AA:BB:CC", "08:00:11:DD:EE:FF",
                                  "10.17.34.51", "10.68.85.102",
                                  TestUtil::kProtoTCP, 0x1188, 0x1199);
    // Parse the packet to get Phv
    Phv *phv_in1 = tu.port_parse(port, p_in);
    RMT_UT_LOG_INFO("Dv48Test::InPkt=%p [DA=%04X%08X]\n", p_in,
                phv_in1->get(Phv::make_word(4,0)), phv_in1->get(Phv::make_word(0,0)));
    Phv *phv_out1 = tu.port_process_inbound(port, phv_in1);
    // Free PHVs}}
    if ((phv_out1 != NULL) && (phv_out1 != phv_in1)) tu.phv_free(phv_out1);
    tu.phv_free(phv_in1);
    */
    

    // Uncomment below to up the debug output
    //flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Switch on all parse loop output
    uint64_t pipes2 = UINT64_C(1) << pipe;
    uint64_t types2 = UINT64_C(1) << RmtTypes::kRmtTypeParser;
    uint64_t flags2 = RmtDebug::kRmtDebugParserParseLoop;
    tu.update_log_flags(pipes2, 0, types2, 0, 0, flags2, ALL);


    // Call 2 PHV variant of port_process_inbound
    Phv *phv_out2 = tu.port_process_inbound(port, phv_in2, ophv_in2);

    //TestUtil::compare_phvs(phv_in2,phv_out2,true);
    EXPECT_EQ(0x9a09, phv_out2->get(132)); // was 0xf62a in input phv
    EXPECT_EQ(0x0, phv_out2->get(148)); // was 0x57d in input phv
    EXPECT_EQ(0x0, phv_out2->get(160)); // was 0xbc9 in input phv
    EXPECT_EQ(0x0, phv_out2->get(162)); // was 0x14c in input phv
    EXPECT_EQ(0x29, phv_out2->get(164)); // was 0x6b4a in input phv
    EXPECT_EQ(0x0, phv_out2->get(165)); // was 0xa792 in input phv
    //EXPECT_EQ(0x94d5, phv_out2->get(166)); // was 0xed7c in input phv
    EXPECT_EQ(0xed7c, phv_out2->get(167)); // was 0x28b9 in input phv
    //EXPECT_EQ(0x4db3, phv_out2->get(168)); // was 0xd31b in input phv
    //EXPECT_EQ(0x3684, phv_out2->get(169)); // was 0x2592 in input phv
    //EXPECT_EQ(0xbef6, phv_out2->get(172)); // was 0x76f5 in input phv
    //EXPECT_EQ(0xbef6, phv_out2->get(173)); // was 0xbc64 in input phv
    EXPECT_EQ(0x0, phv_out2->get(176)); // was 0xbef6 in input phv
    //EXPECT_EQ(0x6b4a, phv_out2->get(181)); // was 0x68f4 in input phv
    //EXPECT_EQ(0xed7c, phv_out2->get(182)); // was 0x2c03 in input phv
    EXPECT_EQ(0x4db3, phv_out2->get(183)); // was 0x2491 in input phv
    EXPECT_EQ(0x2001, phv_out2->get(184)); // was 0x7717 in input phv
    EXPECT_EQ(0x3800, phv_out2->get(185)); // was 0x4c10 in input phv
    EXPECT_EQ(0x970c, phv_out2->get(186)); // was 0xe4c4 in input phv
    //EXPECT_EQ(0x439b, phv_out2->get(187)); // was 0x3b85 in input phv
    //EXPECT_EQ(0xbc9, phv_out2->get(190)); // was 0x4db3 in input phv
    EXPECT_EQ(0x4700, phv_out2->get(191)); // was 0x5347 in input phv
    EXPECT_EQ(0x0, phv_out2->get(192)); // was 0x289a in input phv
    EXPECT_EQ(0x9c3f, phv_out2->get(196)); // was 0x38d8 in input phv
    EXPECT_EQ(0x425d, phv_out2->get(199)); // was 0x23a1 in input phv
    //EXPECT_EQ(0x5d7d, phv_out2->get(200)); // was 0x6f90 in input phv
    EXPECT_EQ(0x698b, phv_out2->get(201)); // was 0x425d in input phv
    EXPECT_EQ(0x6700, phv_out2->get(204)); // was 0xecf0 in input phv
    EXPECT_EQ(0x9c00, phv_out2->get(205)); // was 0xfd44 in input phv
    //EXPECT_EQ(0x67ff, phv_out2->get(209)); // was 0xc039 in input phv
    //EXPECT_EQ(0xfc00, phv_out2->get(210)); // was 0xbfb5 in input phv
    EXPECT_EQ(0xc039, phv_out2->get(218)); // was 0xf1ed in input phv
    EXPECT_EQ(0xc000, phv_out2->get(219)); // was 0x784b in input phv
    EXPECT_EQ(0x9a5e, phv_out2->get(220)); // was 0x5d7d in input phv
    EXPECT_EQ(0x3, phv_out2->get(221)); // was 0xdec in input phv
    //EXPECT_EQ(0x7c16, phv_out2->get(222)); // was 0xf71b in input phv
    

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

    tu.finish_test();
    tu.quieten_log_flags();
}


}
