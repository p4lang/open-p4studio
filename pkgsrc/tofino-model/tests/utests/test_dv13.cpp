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

// runaway lookup -> test_dv13.cpp
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

  bool dv13_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv13Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv13_print) RMT_UT_LOG_INFO("test_dv13_packet1()\n");
    
    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    int chip = 201;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_evaluate_all(true);
    tu.set_free_on_exit(true);
    tu.set_dv_test(13);
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
    //tu.OutWord(&mau_reg_map.dp.final_output_delay[0][0], 0x16); /* 0x2060040 */
    tu.OutWord(&mau_reg_map.dp.pipelength_added_stages[0], 0x1); /* 0x2060070 */
    tu.OutWord(&mau_reg_map.dp.action_output_delay[1], 0xb); /* 0x2060038 */
    //Removed in bfnregs 20150107_182406_7982_mau_dev
    //tu.OutWord(&mau_reg_map.dp.final_output_delay[1][0], 0x1e); /* 0x2060048 */
    tu.OutWord(&mau_reg_map.dp.match_ie_input_mux_sel, 0x3); /* 0x2060108 */
    tu.OutWord(&mau_reg_map.dp.phv_fifo_enable, 0xc); /* 0x2060120 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][0], 0x10); /* 0x2067000 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][1], 0x11); /* 0x2067004 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][2], 0x12); /* 0x2067008 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][3], 0x13); /* 0x206700c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][4], 0x14); /* 0x2067010 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][5], 0x15); /* 0x2067014 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][6], 0x16); /* 0x2067018 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][7], 0x17); /* 0x206701c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][8], 0x18); /* 0x2067020 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][9], 0x19); /* 0x2067024 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][10], 0x1a); /* 0x2067028 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][11], 0x1b); /* 0x206702c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][12], 0x1c); /* 0x2067030 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][13], 0x1d); /* 0x2067034 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][14], 0x1e); /* 0x2067038 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][15], 0x1f); /* 0x206703c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][16], 0x10); /* 0x2067440 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][17], 0x11); /* 0x2067444 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][18], 0x12); /* 0x2067448 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][19], 0x13); /* 0x206744c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][20], 0x14); /* 0x2067450 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][21], 0x15); /* 0x2067454 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][22], 0x16); /* 0x2067458 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][23], 0x17); /* 0x206745c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][24], 0x18); /* 0x2067460 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][25], 0x19); /* 0x2067464 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][26], 0x1a); /* 0x2067468 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][27], 0x1b); /* 0x206746c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][28], 0x1c); /* 0x2067470 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][29], 0x1d); /* 0x2067474 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][30], 0x1e); /* 0x2067478 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][31], 0x1f); /* 0x206747c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][32], 0x10); /* 0x2067880 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][33], 0x11); /* 0x2067884 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][34], 0x12); /* 0x2067888 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][35], 0x13); /* 0x206788c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][36], 0x14); /* 0x2067890 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][37], 0x15); /* 0x2067894 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][38], 0x16); /* 0x2067898 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][39], 0x17); /* 0x206789c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][40], 0x18); /* 0x20678a0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][41], 0x19); /* 0x20678a4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][42], 0x1a); /* 0x20678a8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][43], 0x1b); /* 0x20678ac */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][44], 0x1c); /* 0x20678b0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][45], 0x1d); /* 0x20678b4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][46], 0x1e); /* 0x20678b8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][47], 0x1f); /* 0x20678bc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][48], 0x10); /* 0x2067cc0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][49], 0x11); /* 0x2067cc4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][50], 0x12); /* 0x2067cc8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][51], 0x13); /* 0x2067ccc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][52], 0x14); /* 0x2067cd0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][53], 0x15); /* 0x2067cd4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][54], 0x16); /* 0x2067cd8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][55], 0x17); /* 0x2067cdc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][56], 0x18); /* 0x2067ce0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][57], 0x19); /* 0x2067ce4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][58], 0x1a); /* 0x2067ce8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][59], 0x1b); /* 0x2067cec */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][60], 0x1c); /* 0x2067cf0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][61], 0x1d); /* 0x2067cf4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][62], 0x1e); /* 0x2067cf8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][63], 0x1f); /* 0x2067cfc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][64], 0x40); /* 0x2064100 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][65], 0x41); /* 0x2064104 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][66], 0x42); /* 0x2064108 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][67], 0x43); /* 0x206410c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][68], 0x44); /* 0x2064110 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][69], 0x45); /* 0x2064114 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][70], 0x46); /* 0x2064118 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][71], 0x47); /* 0x206411c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][72], 0x48); /* 0x2064120 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][73], 0x49); /* 0x2064124 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][74], 0x4a); /* 0x2064128 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][75], 0x4b); /* 0x206412c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][76], 0x4c); /* 0x2064130 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][77], 0x4d); /* 0x2064134 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][78], 0x4e); /* 0x2064138 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][79], 0x4f); /* 0x206413c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][80], 0x40); /* 0x2064540 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][81], 0x41); /* 0x2064544 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][82], 0x42); /* 0x2064548 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][83], 0x43); /* 0x206454c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][84], 0x44); /* 0x2064550 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][85], 0x45); /* 0x2064554 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][86], 0x46); /* 0x2064558 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][87], 0x47); /* 0x206455c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][88], 0x48); /* 0x2064560 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][89], 0x49); /* 0x2064564 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][90], 0x4a); /* 0x2064568 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][91], 0x4b); /* 0x206456c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][92], 0x4c); /* 0x2064570 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][93], 0x4d); /* 0x2064574 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][94], 0x4e); /* 0x2064578 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][95], 0x4f); /* 0x206457c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][96], 0x40); /* 0x2064980 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][97], 0x41); /* 0x2064984 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][98], 0x42); /* 0x2064988 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][99], 0x43); /* 0x206498c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][100], 0x44); /* 0x2064990 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][101], 0x45); /* 0x2064994 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][102], 0x46); /* 0x2064998 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][103], 0x47); /* 0x206499c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][104], 0x48); /* 0x20649a0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][105], 0x49); /* 0x20649a4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][106], 0x4a); /* 0x20649a8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][107], 0x4b); /* 0x20649ac */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][108], 0x4c); /* 0x20649b0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][109], 0x4d); /* 0x20649b4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][110], 0x4e); /* 0x20649b8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][111], 0x4f); /* 0x20649bc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][112], 0x40); /* 0x2064dc0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][113], 0x41); /* 0x2064dc4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][114], 0x42); /* 0x2064dc8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][115], 0x43); /* 0x2064dcc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][116], 0x44); /* 0x2064dd0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][117], 0x45); /* 0x2064dd4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][118], 0x46); /* 0x2064dd8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][119], 0x47); /* 0x2064ddc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][120], 0x48); /* 0x2064de0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][121], 0x49); /* 0x2064de4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][122], 0x4a); /* 0x2064de8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][123], 0x4b); /* 0x2064dec */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][124], 0x4c); /* 0x2064df0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][125], 0x4d); /* 0x2064df4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][126], 0x4e); /* 0x2064df8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][127], 0x4f); /* 0x2064dfc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][128], 0x50); /* 0x2064200 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][129], 0x51); /* 0x2064204 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][130], 0x52); /* 0x2064208 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][131], 0x53); /* 0x206420c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][132], 0x54); /* 0x2064210 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][133], 0x55); /* 0x2064214 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][134], 0x56); /* 0x2064218 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][135], 0x57); /* 0x206421c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][136], 0x58); /* 0x2064220 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][137], 0x59); /* 0x2064224 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][138], 0x5a); /* 0x2064228 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][139], 0x5b); /* 0x206422c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][140], 0x5c); /* 0x2064230 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][141], 0x5d); /* 0x2064234 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][142], 0x5e); /* 0x2064238 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][143], 0x5f); /* 0x206423c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][144], 0x60); /* 0x2064240 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][145], 0x61); /* 0x2064244 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][146], 0x62); /* 0x2064248 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][147], 0x63); /* 0x206424c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][148], 0x64); /* 0x2064250 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][149], 0x65); /* 0x2064254 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][150], 0x66); /* 0x2064258 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][151], 0x67); /* 0x206425c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][152], 0x50); /* 0x2064660 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][153], 0x51); /* 0x2064664 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][154], 0x52); /* 0x2064668 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][155], 0x53); /* 0x206466c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][156], 0x54); /* 0x2064670 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][157], 0x55); /* 0x2064674 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][158], 0x56); /* 0x2064678 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][159], 0x57); /* 0x206467c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][160], 0x58); /* 0x2064680 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][161], 0x59); /* 0x2064684 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][162], 0x5a); /* 0x2064688 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][163], 0x5b); /* 0x206468c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][164], 0x5c); /* 0x2064690 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][165], 0x5d); /* 0x2064694 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][166], 0x5e); /* 0x2064698 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][167], 0x5f); /* 0x206469c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][168], 0x60); /* 0x20646a0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][169], 0x61); /* 0x20646a4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][170], 0x62); /* 0x20646a8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][171], 0x63); /* 0x20646ac */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][172], 0x64); /* 0x20646b0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][173], 0x65); /* 0x20646b4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][174], 0x66); /* 0x20646b8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][175], 0x67); /* 0x20646bc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][176], 0x50); /* 0x2064ac0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][177], 0x51); /* 0x2064ac4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][178], 0x52); /* 0x2064ac8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][179], 0x53); /* 0x2064acc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][180], 0x54); /* 0x2064ad0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][181], 0x55); /* 0x2064ad4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][182], 0x56); /* 0x2064ad8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][183], 0x57); /* 0x2064adc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][184], 0x58); /* 0x2064ae0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][185], 0x59); /* 0x2064ae4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][186], 0x5a); /* 0x2064ae8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][187], 0x5b); /* 0x2064aec */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][188], 0x5c); /* 0x2064af0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][189], 0x5d); /* 0x2064af4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][190], 0x5e); /* 0x2064af8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][191], 0x5f); /* 0x2064afc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][192], 0x60); /* 0x2064b00 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][193], 0x61); /* 0x2064b04 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][194], 0x62); /* 0x2064b08 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][195], 0x63); /* 0x2064b0c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][196], 0x64); /* 0x2064b10 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][197], 0x65); /* 0x2064b14 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][198], 0x66); /* 0x2064b18 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][199], 0x67); /* 0x2064b1c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][200], 0x50); /* 0x2064f20 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][201], 0x51); /* 0x2064f24 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][202], 0x52); /* 0x2064f28 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][203], 0x53); /* 0x2064f2c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][204], 0x54); /* 0x2064f30 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][205], 0x55); /* 0x2064f34 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][206], 0x56); /* 0x2064f38 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][207], 0x57); /* 0x2064f3c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][208], 0x58); /* 0x2064f40 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][209], 0x59); /* 0x2064f44 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][210], 0x5a); /* 0x2064f48 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][211], 0x5b); /* 0x2064f4c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][212], 0x5c); /* 0x2064f50 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][213], 0x5d); /* 0x2064f54 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][214], 0x5e); /* 0x2064f58 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][215], 0x5f); /* 0x2064f5c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][0], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][1], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][2], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][3], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][4], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][5], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][6], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][7], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][32], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][33], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][34], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][35], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][36], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][37], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][38], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][39], 0x17); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][64], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][65], 0x41); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][66], 0x42); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][67], 0x43); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][68], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][69], 0x45); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][70], 0x46); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][71], 0x47); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][96], 0x48); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][97], 0x49); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][98], 0x4a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][99], 0x4b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][100], 0x4c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][101], 0x4d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][102], 0x4e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][103], 0x4f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][128], 0x50); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][129], 0x51); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][130], 0x52); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][131], 0x53); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][132], 0x54); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][133], 0x55); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][134], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][135], 0x57); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][136], 0x58); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][137], 0x59); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][138], 0x5a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][139], 0x5b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][176], 0x5c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][177], 0x5d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][178], 0x5e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][179], 0x5f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][180], 0x60); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][181], 0x61); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][182], 0x62); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][183], 0x63); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][184], 0x64); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][185], 0x65); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][186], 0x66); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][187], 0x67); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][8], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][9], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][10], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][11], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][12], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][13], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][14], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][15], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][40], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][41], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][42], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][43], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][44], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][45], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][46], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][47], 0x17); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][72], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][73], 0x41); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][74], 0x42); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][75], 0x43); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][76], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][77], 0x45); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][78], 0x46); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][79], 0x47); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][104], 0x48); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][105], 0x49); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][106], 0x4a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][107], 0x4b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][108], 0x4c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][109], 0x4d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][110], 0x4e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][111], 0x4f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][140], 0x50); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][141], 0x51); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][142], 0x52); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][143], 0x53); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][144], 0x54); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][145], 0x55); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][146], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][147], 0x57); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][148], 0x58); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][149], 0x59); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][150], 0x5a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][151], 0x5b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][188], 0x5c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][189], 0x5d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][190], 0x5e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][191], 0x5f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][192], 0x60); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][193], 0x61); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][194], 0x62); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][195], 0x63); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][16], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][17], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][18], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][19], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][20], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][21], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][22], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][23], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][48], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][49], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][50], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][51], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][52], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][53], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][54], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][55], 0x17); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][80], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][81], 0x41); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][82], 0x42); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][83], 0x43); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][84], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][85], 0x45); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][86], 0x46); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][87], 0x47); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][112], 0x48); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][113], 0x49); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][114], 0x4a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][115], 0x4b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][116], 0x4c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][117], 0x4d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][118], 0x4e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][119], 0x4f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][152], 0x50); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][153], 0x51); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][154], 0x52); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][155], 0x53); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][156], 0x54); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][157], 0x55); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][158], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][159], 0x57); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][160], 0x58); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][161], 0x59); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][162], 0x5a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][163], 0x5b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][24], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][25], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][26], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][27], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][28], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][29], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][30], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][31], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][56], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][57], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][58], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][59], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][60], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][61], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][62], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][63], 0x17); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][88], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][89], 0x41); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][90], 0x42); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][91], 0x43); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][92], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][93], 0x45); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][94], 0x46); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][95], 0x47); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][120], 0x48); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][121], 0x49); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][122], 0x4a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][123], 0x4b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][124], 0x4c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][125], 0x4d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][126], 0x4e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][127], 0x4f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][164], 0x50); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][165], 0x51); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][166], 0x52); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][167], 0x53); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][168], 0x54); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][169], 0x55); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][170], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][171], 0x57); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][172], 0x58); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][173], 0x59); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][174], 0x5a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][175], 0x5b); // regs_31841 fix
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_mode[0], 0x7f15000); /* 0x20406c0 */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_table_map[0], 0x1); /* 0x2040680 */
    tu.OutWord(&mau_reg_map.tcams.tcam_match_adr_shift[0], 0x4); /* 0x2040420 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.unitram_config[1][1], 0x3c8a); /* 0x201cbc8 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.unitram_config[0][3], 0x3c76); /* 0x201eb98 */
    tu.OutWord(&mau_reg_map.rams.array.row[1].ram[7].unit_ram_ctl, 0x20); /* 0x2009398 */
    tu.OutWord(&mau_reg_map.rams.array.row[5].ram[3].unit_ram_ctl, 0x200); /* 0x200d198 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tind_ram_data_size[10], 0x5); /* 0x2016528 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][0], 0x16); /* 0x2040380 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][0], 0x3); /* 0x2040200 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][1], 0x12); /* 0x2040384 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][1], 0x1); /* 0x2040204 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][0], 0x5); /* 0x2040300 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][0], 0x9); /* 0x2040000 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][1], 0x6); /* 0x2040004 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][2], 0xa); /* 0x2040008 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][3], 0x0); /* 0x204000c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][4], 0x1); /* 0x2040010 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][5], 0x1); /* 0x2040014 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][6], 0xa); /* 0x2040018 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][7], 0x2); /* 0x204001c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][0], 0x16); /* 0x20403c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][0], 0x0); /* 0x2040240 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][1], 0x8); /* 0x20403c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][1], 0x2); /* 0x2040244 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][0], 0xc); /* 0x2040320 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][0], 0x8); /* 0x2040100 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][1], 0x2); /* 0x2040104 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][2], 0x9); /* 0x2040108 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][3], 0x9); /* 0x204010c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][4], 0xa); /* 0x2040110 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][5], 0x2); /* 0x2040114 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][6], 0x9); /* 0x2040118 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][7], 0x2); /* 0x204011c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][2], 0x13); /* 0x2040388 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][2], 0x2); /* 0x2040208 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][3], 0xc); /* 0x204038c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][3], 0x0); /* 0x204020c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][1], 0xa); /* 0x2040304 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][0], 0x8); /* 0x2040020 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][1], 0xa); /* 0x2040024 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][2], 0x2); /* 0x2040028 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][3], 0x8); /* 0x204002c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][4], 0x4); /* 0x2040030 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][5], 0x4); /* 0x2040034 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][6], 0x6); /* 0x2040038 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][7], 0x6); /* 0x204003c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][2], 0x1f); /* 0x20403c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][2], 0x2); /* 0x2040248 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][3], 0xa); /* 0x20403cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][3], 0x1); /* 0x204024c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][1], 0xd); /* 0x2040324 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][0], 0x8); /* 0x2040120 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][1], 0x2); /* 0x2040124 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][2], 0x0); /* 0x2040128 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][3], 0x6); /* 0x204012c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][4], 0x9); /* 0x2040130 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][5], 0x9); /* 0x2040134 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][6], 0x2); /* 0x2040138 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][7], 0x4); /* 0x204013c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][4], 0x18); /* 0x2040390 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][4], 0x1); /* 0x2040210 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][5], 0x5); /* 0x2040394 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][5], 0x3); /* 0x2040214 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][2], 0xe); /* 0x2040308 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][0], 0x6); /* 0x2040040 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][1], 0x0); /* 0x2040044 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][2], 0x6); /* 0x2040048 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][3], 0x4); /* 0x204004c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][4], 0x6); /* 0x2040050 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][5], 0x0); /* 0x2040054 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][6], 0x6); /* 0x2040058 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][7], 0x6); /* 0x204005c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][4], 0x0); /* 0x20403d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][4], 0x1); /* 0x2040250 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][5], 0x8); /* 0x20403d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][5], 0x1); /* 0x2040254 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][2], 0xb); /* 0x2040328 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][0], 0x3); /* 0x2040140 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][1], 0x4); /* 0x2040144 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][2], 0xa); /* 0x2040148 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][3], 0x8); /* 0x204014c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][4], 0x6); /* 0x2040150 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][5], 0x8); /* 0x2040154 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][6], 0x1); /* 0x2040158 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][7], 0xa); /* 0x204015c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][6], 0x0); /* 0x2040398 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][6], 0x3); /* 0x2040218 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][7], 0x17); /* 0x204039c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][7], 0x3); /* 0x204021c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][3], 0x4); /* 0x204030c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][0], 0x9); /* 0x2040060 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][1], 0x2); /* 0x2040064 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][2], 0x7); /* 0x2040068 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][3], 0x2); /* 0x204006c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][4], 0x5); /* 0x2040070 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][5], 0x9); /* 0x2040074 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][6], 0x7); /* 0x2040078 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][7], 0x5); /* 0x204007c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][6], 0x17); /* 0x20403d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][6], 0x0); /* 0x2040258 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][7], 0x12); /* 0x20403dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][7], 0x3); /* 0x204025c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][3], 0xc); /* 0x204032c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][0], 0x9); /* 0x2040160 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][1], 0x9); /* 0x2040164 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][2], 0x8); /* 0x2040168 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][3], 0x7); /* 0x204016c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][4], 0x6); /* 0x2040170 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][5], 0x6); /* 0x2040174 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][6], 0x1); /* 0x2040178 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][7], 0x1); /* 0x204017c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][8], 0xb); /* 0x20403a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][8], 0x0); /* 0x2040220 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][9], 0x1d); /* 0x20403a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][9], 0x0); /* 0x2040224 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][4], 0xe); /* 0x2040310 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][0], 0x6); /* 0x2040080 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][1], 0x0); /* 0x2040084 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][2], 0x2); /* 0x2040088 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][3], 0x8); /* 0x204008c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][4], 0x8); /* 0x2040090 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][5], 0x9); /* 0x2040094 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][6], 0xa); /* 0x2040098 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][7], 0x9); /* 0x204009c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][8], 0xb); /* 0x20403e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][8], 0x0); /* 0x2040260 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][9], 0x0); /* 0x20403e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][9], 0x2); /* 0x2040264 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][4], 0xf); /* 0x2040330 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][0], 0x8); /* 0x2040180 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][1], 0x1); /* 0x2040184 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][2], 0x5); /* 0x2040188 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][3], 0x1); /* 0x204018c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][4], 0x2); /* 0x2040190 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][5], 0x0); /* 0x2040194 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][6], 0x4); /* 0x2040198 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][7], 0x0); /* 0x204019c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][10], 0x4); /* 0x20403a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][10], 0x3); /* 0x2040228 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][11], 0x5); /* 0x20403ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][11], 0x2); /* 0x204022c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][5], 0x6); /* 0x2040314 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][0], 0x4); /* 0x20400a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][1], 0xa); /* 0x20400a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][2], 0x0); /* 0x20400a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][3], 0x6); /* 0x20400ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][4], 0x5); /* 0x20400b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][5], 0x4); /* 0x20400b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][6], 0x1); /* 0x20400b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][7], 0x9); /* 0x20400bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][10], 0x1); /* 0x20403e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][10], 0x3); /* 0x2040268 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][11], 0xa); /* 0x20403ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][11], 0x3); /* 0x204026c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][5], 0x8); /* 0x2040334 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][0], 0x5); /* 0x20401a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][1], 0x1); /* 0x20401a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][2], 0x7); /* 0x20401a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][3], 0x2); /* 0x20401ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][4], 0x8); /* 0x20401b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][5], 0x0); /* 0x20401b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][6], 0x5); /* 0x20401b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][7], 0x4); /* 0x20401bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][12], 0x1b); /* 0x20403b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][12], 0x2); /* 0x2040230 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][13], 0x1d); /* 0x20403b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][13], 0x3); /* 0x2040234 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][6], 0xe); /* 0x2040318 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][0], 0x1); /* 0x20400c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][1], 0xa); /* 0x20400c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][2], 0xa); /* 0x20400c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][3], 0xa); /* 0x20400cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][4], 0xa); /* 0x20400d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][5], 0x4); /* 0x20400d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][6], 0x4); /* 0x20400d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][7], 0x6); /* 0x20400dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][12], 0x13); /* 0x20403f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][12], 0x3); /* 0x2040270 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][13], 0x13); /* 0x20403f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][13], 0x0); /* 0x2040274 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][6], 0xf); /* 0x2040338 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][0], 0x2); /* 0x20401c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][1], 0x0); /* 0x20401c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][2], 0x2); /* 0x20401c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][3], 0x5); /* 0x20401cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][4], 0x4); /* 0x20401d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][5], 0xa); /* 0x20401d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][6], 0x2); /* 0x20401d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][7], 0x6); /* 0x20401dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][14], 0xb); /* 0x20403b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][14], 0x3); /* 0x2040238 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][15], 0x7); /* 0x20403bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][15], 0x3); /* 0x204023c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][7], 0xb); /* 0x204031c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][0], 0x1); /* 0x20400e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][1], 0x0); /* 0x20400e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][2], 0x9); /* 0x20400e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][3], 0x8); /* 0x20400ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][4], 0x7); /* 0x20400f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][5], 0x0); /* 0x20400f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][6], 0x3); /* 0x20400f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][7], 0x1); /* 0x20400fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][14], 0x0); /* 0x20403f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][14], 0x1); /* 0x2040278 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][15], 0x1c); /* 0x20403fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][15], 0x0); /* 0x204027c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][7], 0x0); /* 0x204033c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][0], 0x5); /* 0x20401e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][1], 0x8); /* 0x20401e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][2], 0x0); /* 0x20401e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][3], 0x0); /* 0x20401ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][4], 0x7); /* 0x20401f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][5], 0x0); /* 0x20401f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][6], 0x8); /* 0x20401f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][7], 0x7); /* 0x20401fc */
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_table_prop[0], 0x4); /* 0x2014a00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tind_bus_prop[10], 0x4); /* 0x2014828 */
  //    tu.OutWord(&mau_reg_map.rams.match.merge.exact_match_delay_config, 0x50000); /* 0x2016490 */ // REMOVED EMDEL070915
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_hit_to_logical_table_ixbar_outputmap[0], 0x1f); /* 0x2014860 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_match_adr_to_physical_oxbar_outputmap[10], 0x8); /* 0x20164e8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.match_to_logical_table_ixbar_outputmap[1][10], 0x1f); /* 0x2014ae8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_tcam_shiftcount[10], 0x8); /* 0x2016628 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_mask[1][10], 0x3f); /* 0x2014d68 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_tcam_shiftcount[10], 0x0); /* 0x20165a8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_mask[1][10], 0xffffffff); /* 0x2014be8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.next_table_format_data[15], 0x100); /* 0x201657c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_tcam_shiftcount[10], 0x2f); /* 0x20166a8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_mask[1][10], 0x3fffff); /* 0x2014ee8 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.immediate_data_32b_ixbar_ctl[15], 0x20); /* 0x20103bc */
    tu.OutWord(&mau_reg_map.rams.match.merge.predication_ctl[0], 0x8000); /* 0x2014a20 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.ram_address_mux_ctl[1][1], 0x80); /* 0x201cb48 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[ 5].vh_xbars.adr_dist_tind_adr_xbar_ctl[0], 0x8); /* 0x201ed30 */ 
    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.ram_address_mux_ctl[0][3], 0x100); /* 0x201eb18 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.adr_dist_action_data_adr_icxbar_ctl[15], 0x8); /* 0x20103fc */
  ActionHvTranslator act_hv_translator;
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x1); /* 0x2009884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,1,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x3); /* 0x200988c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,1,0x3); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x3); /* 0x2009884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,1,0x3); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xf); /* 0x200988c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,1,0xf); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x7); /* 0x2009884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,1,0x7); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x3f); /* 0x200988c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,1,0x3f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0xf); /* 0x2009884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,1,0xf); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xff); /* 0x200988c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,1,0xff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x1f); /* 0x2009884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,1,0x1f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x3ff); /* 0x200988c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,1,0x3ff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x3f); /* 0x2009884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,1,0x3f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xfff); /* 0x200988c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,1,0xfff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x7f); /* 0x2009884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,1,0x7f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x3fff); /* 0x200988c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,1,0x3fff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0xff); /* 0x2009884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(1,1,0xff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xffff); /* 0x200988c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(1,1,0xffff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x1); /* 0x2009858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(1,1,6,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x1); /* 0x2009858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(1,1,6,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x1); /* 0x2009858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(1,1,6,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x1); /* 0x2009858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(1,1,6,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x5); /* 0x2009858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(1,1,6,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x5); /* 0x2009858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(1,1,6,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x5); /* 0x2009858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(1,1,6,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[1].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x5); /* 0x2009858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(1,1,6,0x5); // ADDED ACHV070915
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
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][0], RM_B4_32(0x1523e2a2)); /* 0x207c000 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][9], RM_B4_32(0x3df1e240)); /* 0x207c0a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][4], RM_B4_32(0x1799e34b)); /* 0x207c110 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][11], RM_B4_32(0x1535e0e0)); /* 0x207c1ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][7], RM_B4_32(0x2fb7e036)); /* 0x207c21c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][9], RM_B4_32(0x519e09a)); /* 0x207c2a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][7], RM_B4_32(0xd9fe2ec)); /* 0x207c31c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][11], RM_B4_32(0x2207e3bb)); /* 0x207c3ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][11], RM_B4_32(0x38b5e279)); /* 0x207c42c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][5], RM_B4_32(0x3903e1f7)); /* 0x207c494 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][4], RM_B4_32(0x2781e148)); /* 0x207c510 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][2], RM_B4_32(0x1931e2da)); /* 0x207c588 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][7], RM_B4_32(0x3c03e3ac)); /* 0x207c61c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][3], RM_B4_32(0x26ade323)); /* 0x207c68c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][6], RM_B4_32(0x8cbe010)); /* 0x207c718 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][4], RM_B4_32(0x15bde058)); /* 0x207c790 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][3], RM_B4_32(0x2317e1d6)); /* 0x207c80c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][11], RM_B4_32(0x657e2d5)); /* 0x207c8ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][1], RM_B4_32(0x3fc7e37a)); /* 0x207c904 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][4], RM_B4_32(0x2383e27d)); /* 0x207c990 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][10], RM_B4_32(0x157e142)); /* 0x207ca28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][6], RM_B4_32(0x1195e1ba)); /* 0x207ca98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][4], RM_B4_32(0x3fa7e0d0)); /* 0x207cb10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][4], RM_B4_32(0x889e315)); /* 0x207cb90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][1], RM_B4_32(0x803e06c)); /* 0x207cc04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][2], RM_B4_32(0x3e3e01e)); /* 0x207cc88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][5], RM_B4_32(0x3d5e050)); /* 0x207cd14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][1], RM_B4_32(0x2e33e218)); /* 0x207cd84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][12], RM_B4_32(0x803e180)); /* 0x207ce30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][3], RM_B4_32(0xa57e1e3)); /* 0x207ce8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][3], RM_B4_32(0x14e9e1f3)); /* 0x207cf0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][2], RM_B4_32(0x3925e282)); /* 0x207cf88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][6], RM_B4_32(0x2f87e35f)); /* 0x207d018 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][7], RM_B4_32(0x2b75e1b6)); /* 0x207d09c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][2], RM_B4_32(0x539e153)); /* 0x207d108 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][5], RM_B4_32(0xec3e1b3)); /* 0x207d194 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][11], RM_B4_32(0x3c45e29f)); /* 0x207d22c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][8], RM_B4_32(0x3185e1f8)); /* 0x207d2a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][2], RM_B4_32(0x8d7e33a)); /* 0x207d308 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][5], RM_B4_32(0x12c9e28a)); /* 0x207d394 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][6], RM_B4_32(0x32dbe106)); /* 0x207d418 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][7], RM_B4_32(0x2c95e158)); /* 0x207d49c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][10], RM_B4_32(0xfb9e3bd)); /* 0x207d528 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][4], RM_B4_32(0x12dbe095)); /* 0x207d590 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][4], RM_B4_32(0x3f99e37d)); /* 0x207d610 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][1], RM_B4_32(0x2563e140)); /* 0x207d684 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][1], RM_B4_32(0x2cc5e164)); /* 0x207d704 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][6], RM_B4_32(0x18f7e1b1)); /* 0x207d798 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][12], RM_B4_32(0x769e328)); /* 0x207d830 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][7], RM_B4_32(0x1719e38f)); /* 0x207d89c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][6], RM_B4_32(0x187de234)); /* 0x207d918 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][9], RM_B4_32(0x19dbe1dd)); /* 0x207d9a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][0], RM_B4_32(0xdf7e0fb)); /* 0x207da00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][10], RM_B4_32(0x3d51e264)); /* 0x207daa8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][5], RM_B4_32(0x1633e1a9)); /* 0x207db14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][1], RM_B4_32(0x3501e301)); /* 0x207db84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][6], RM_B4_32(0x1891e0df)); /* 0x207dc18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][0], RM_B4_32(0x2e15e09f)); /* 0x207dc80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][9], RM_B4_32(0x2da5e1d7)); /* 0x207dd24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][2], RM_B4_32(0x3937e187)); /* 0x207dd88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][1], RM_B4_32(0xd87e069)); /* 0x207de04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][8], RM_B4_32(0x1a19e146)); /* 0x207dea0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][6], RM_B4_32(0x1e93e22a)); /* 0x207df18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][5], RM_B4_32(0x1c7e3f8)); /* 0x207df94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[0][7], RM_B4_8(0x83e158)); /* 0x207e01c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[1][2], RM_B4_8(0x71e104)); /* 0x207e088 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[2][0], RM_B4_8(0xdde290)); /* 0x207e100 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[3][0], RM_B4_8(0xd3206e)); /* 0x207e180 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[4][9], RM_B4_8(0x77e0aa)); /* 0x207e224 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[5][9], RM_B4_8(0x7e2104)); /* 0x207e2a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[6][5], RM_B4_8(0x7de2fa)); /* 0x207e314 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[7][5], RM_B4_8(0xf722be)); /* 0x207e394 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[8][2], RM_B4_8(0x9fe0b6)); /* 0x207e408 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[9][2], RM_B4_8(0x9b2012)); /* 0x207e488 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[10][3], RM_B4_8(0x47e360)); /* 0x207e50c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[11][5], RM_B4_8(0xfe0e9)); /* 0x207e594 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[12][1], RM_B4_8(0xb5e045)); /* 0x207e604 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[13][1], RM_B4_8(0x262292)); /* 0x207e684 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[14][12], RM_B4_8(0x59e0b6)); /* 0x207e730 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[15][12], RM_B4_8(0xe3216a)); /* 0x207e7b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[16][11], RM_B4_8(0xd5e1f3)); /* 0x207e82c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[17][11], RM_B4_8(0x622010)); /* 0x207e8ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[18][11], RM_B4_8(0x27e12e)); /* 0x207e92c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[19][11], RM_B4_8(0xf7e394)); /* 0x207e9ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[20][2], RM_B4_8(0x97e06c)); /* 0x207ea08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[21][2], RM_B4_8(0x41e152)); /* 0x207ea88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[22][0], RM_B4_8(0xb1e169)); /* 0x207eb00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[23][5], RM_B4_8(0x47e1d3)); /* 0x207eb94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[24][5], RM_B4_8(0xc1e296)); /* 0x207ec14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[25][5], RM_B4_8(0xe82035)); /* 0x207ec94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[26][3], RM_B4_8(0x5de1f4)); /* 0x207ed0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[27][3], RM_B4_8(0x7b21b9)); /* 0x207ed8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[28][4], RM_B4_8(0xc3e1d5)); /* 0x207ee10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[29][7], RM_B4_8(0xd5e398)); /* 0x207ee9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[30][1], RM_B4_8(0x4be1f1)); /* 0x207ef04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[31][1], RM_B4_8(0x612092)); /* 0x207ef84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[32][4], RM_B4_8(0x7e2db)); /* 0x207f010 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[33][4], RM_B4_8(0x902271)); /* 0x207f090 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[34][2], RM_B4_8(0x73e128)); /* 0x207f108 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[35][2], RM_B4_8(0x6622ea)); /* 0x207f188 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[36][11], RM_B4_8(0x77e2bb)); /* 0x207f22c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[37][11], RM_B4_8(0xc6208e)); /* 0x207f2ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[38][12], RM_B4_8(0x25e3ad)); /* 0x207f330 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[39][12], RM_B4_8(0x142380)); /* 0x207f3b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[40][10], RM_B4_8(0x15e22b)); /* 0x207f428 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[41][10], RM_B4_8(0xba22a8)); /* 0x207f4a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[42][8], RM_B4_8(0x1be05b)); /* 0x207f520 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[43][6], RM_B4_8(0x21e0b3)); /* 0x207f598 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[44][4], RM_B4_8(0x5de1e8)); /* 0x207f610 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[45][4], RM_B4_8(0xdb231b)); /* 0x207f690 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[46][10], RM_B4_8(0x35e3b5)); /* 0x207f728 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[47][10], RM_B4_8(0x36207c)); /* 0x207f7a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[48][3], RM_B4_8(0xa3e295)); /* 0x207f80c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[49][2], RM_B4_8(0x61e3b5)); /* 0x207f888 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[50][5], RM_B4_8(0x97e26e)); /* 0x207f914 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[51][5], RM_B4_8(0xba2176)); /* 0x207f994 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[52][8], RM_B4_8(0x4fe203)); /* 0x207fa20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[53][8], RM_B4_8(0x4122c7)); /* 0x207faa0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[54][2], RM_B4_8(0xd7e036)); /* 0x207fb08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[55][2], RM_B4_8(0x6222cf)); /* 0x207fb88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[56][1], RM_B4_8(0xf9e1cb)); /* 0x207fc04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[57][1], RM_B4_8(0x6721ed)); /* 0x207fc84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[58][11], RM_B4_8(0x27e0b8)); /* 0x207fd2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[59][11], RM_B4_8(0x9c2163)); /* 0x207fdac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[60][4], RM_B4_8(0xfe3ee)); /* 0x207fe10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[61][4], RM_B4_8(0x9923ab)); /* 0x207fe90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[62][7], RM_B4_8(0xdbe151)); /* 0x207ff1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[63][2], RM_B4_8(0xa7e069)); /* 0x207ff88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[0][7], RM_B4_16(0x40be27e)); /* 0x207801c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[1][6], RM_B4_16(0x6dbe3be)); /* 0x2078098 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[2][10], RM_B4_16(0x6c5e1e0)); /* 0x2078128 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[3][10], RM_B4_16(0x26b22ba)); /* 0x20781a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[4][11], RM_B4_16(0x78fe384)); /* 0x207822c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[5][3], RM_B4_16(0x317e210)); /* 0x207828c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[6][11], RM_B4_16(0x1c7e04f)); /* 0x207832c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[7][7], RM_B4_16(0x3c5e2c2)); /* 0x207839c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[8][4], RM_B4_16(0x729e129)); /* 0x2078410 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[9][5], RM_B4_16(0x24be2a6)); /* 0x2078494 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[10][10], RM_B4_16(0x449e340)); /* 0x2078528 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[11][10], RM_B4_16(0x461227d)); /* 0x20785a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[12][9], RM_B4_16(0x203e21d)); /* 0x2078624 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[13][10], RM_B4_16(0x497e10c)); /* 0x20786a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[14][6], RM_B4_16(0xb7e187)); /* 0x2078718 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[15][9], RM_B4_16(0x3cfe04d)); /* 0x20787a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[16][11], RM_B4_16(0x413e153)); /* 0x207882c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[17][2], RM_B4_16(0x2b7e35d)); /* 0x2078888 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[18][7], RM_B4_16(0x7f3e111)); /* 0x207891c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[19][8], RM_B4_16(0xf7e223)); /* 0x20789a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[20][11], RM_B4_16(0x11fe2c9)); /* 0x2078a2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[21][11], RM_B4_16(0x57c2366)); /* 0x2078aac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[22][0], RM_B4_16(0x5e7e368)); /* 0x2078b00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[23][0], RM_B4_16(0x5c7208b)); /* 0x2078b80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[24][12], RM_B4_16(0x75be0c8)); /* 0x2078c30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[25][12], RM_B4_16(0x315209d)); /* 0x2078cb0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[26][9], RM_B4_16(0x45de27d)); /* 0x2078d24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[27][8], RM_B4_16(0x241e03d)); /* 0x2078da0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[28][10], RM_B4_16(0x2dfe25a)); /* 0x2078e28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[29][10], RM_B4_16(0x3e32010)); /* 0x2078ea8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[30][0], RM_B4_16(0x55e320)); /* 0x2078f00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[31][0], RM_B4_16(0xc12035)); /* 0x2078f80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[32][2], RM_B4_16(0x423e3fc)); /* 0x2079008 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[33][2], RM_B4_16(0x118226d)); /* 0x2079088 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[34][2], RM_B4_16(0x593e27e)); /* 0x2079108 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[35][2], RM_B4_16(0x5622ca)); /* 0x2079188 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[36][3], RM_B4_16(0x5a3e076)); /* 0x207920c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[37][3], RM_B4_16(0x4f62048)); /* 0x207928c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[38][1], RM_B4_16(0x1a5e1c1)); /* 0x2079304 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[39][1], RM_B4_16(0xe623a3)); /* 0x2079384 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[40][3], RM_B4_16(0x645e230)); /* 0x207940c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[41][3], RM_B4_16(0x759e0d3)); /* 0x207948c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[42][6], RM_B4_16(0x70fe21b)); /* 0x2079518 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[43][6], RM_B4_16(0x70c20fb)); /* 0x2079598 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[44][4], RM_B4_16(0x4dbe24f)); /* 0x2079610 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[45][12], RM_B4_16(0x291e257)); /* 0x20796b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[46][0], RM_B4_16(0x75fe0f9)); /* 0x2079700 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[47][0], RM_B4_16(0x33d20f1)); /* 0x2079780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[48][5], RM_B4_16(0x2fde08a)); /* 0x2079814 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[49][5], RM_B4_16(0x66b2338)); /* 0x2079894 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[50][3], RM_B4_16(0xe7e01d)); /* 0x207990c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[51][3], RM_B4_16(0x1c32378)); /* 0x207998c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[52][3], RM_B4_16(0x8be147)); /* 0x2079a0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[53][3], RM_B4_16(0x5722112)); /* 0x2079a8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[54][5], RM_B4_16(0x1b1e2c7)); /* 0x2079b14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[55][5], RM_B4_16(0x83e1c2)); /* 0x2079b94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[56][7], RM_B4_16(0xc7e2ac)); /* 0x2079c1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[57][7], RM_B4_16(0x17320e6)); /* 0x2079c9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[58][1], RM_B4_16(0x633e149)); /* 0x2079d04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[59][1], RM_B4_16(0x6e82314)); /* 0x2079d84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[60][7], RM_B4_16(0x4a7e0de)); /* 0x2079e1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[61][7], RM_B4_16(0x6a22a5)); /* 0x2079e9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[62][8], RM_B4_16(0x6c7e026)); /* 0x2079f20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[63][6], RM_B4_16(0x79be23e)); /* 0x2079f98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[64][8], RM_B4_16(0x229e25e)); /* 0x207a020 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[65][0], RM_B4_16(0x1e7e321)); /* 0x207a080 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[66][10], RM_B4_16(0x1c9e3ec)); /* 0x207a128 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[67][10], RM_B4_16(0x1cf232d)); /* 0x207a1a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[68][5], RM_B4_16(0x2bde2aa)); /* 0x207a214 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[69][5], RM_B4_16(0x7122307)); /* 0x207a294 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[70][12], RM_B4_16(0x741e35c)); /* 0x207a330 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[71][12], RM_B4_16(0x66d204f)); /* 0x207a3b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[72][12], RM_B4_16(0x75be2cd)); /* 0x207a430 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[73][12], RM_B4_16(0x7ed22f9)); /* 0x207a4b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[74][11], RM_B4_16(0x67e31a)); /* 0x207a52c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[75][11], RM_B4_16(0x1912266)); /* 0x207a5ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[76][10], RM_B4_16(0x60de32c)); /* 0x207a628 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[77][10], RM_B4_16(0x6802302)); /* 0x207a6a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[78][8], RM_B4_16(0x551e2af)); /* 0x207a720 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[79][8], RM_B4_16(0x1f520c5)); /* 0x207a7a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[80][6], RM_B4_16(0x3cfe2c8)); /* 0x207a818 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[81][6], RM_B4_16(0x68f2159)); /* 0x207a898 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[82][1], RM_B4_16(0xf9e009)); /* 0x207a904 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[83][1], RM_B4_16(0x4a3201b)); /* 0x207a984 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[84][2], RM_B4_16(0xcbe12b)); /* 0x207aa08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[85][2], RM_B4_16(0x43b23bc)); /* 0x207aa88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[86][10], RM_B4_16(0x143e303)); /* 0x207ab28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[87][10], RM_B4_16(0x5ed2345)); /* 0x207aba8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[88][0], RM_B4_16(0x2a1e048)); /* 0x207ac00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[89][0], RM_B4_16(0x6f223cd)); /* 0x207ac80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[90][4], RM_B4_16(0x385e033)); /* 0x207ad10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[91][4], RM_B4_16(0x296236e)); /* 0x207ad90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[92][4], RM_B4_16(0x67e36f)); /* 0x207ae10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[93][4], RM_B4_16(0x1a12274)); /* 0x207ae90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[94][0], RM_B4_16(0x483e20b)); /* 0x207af00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[95][0], RM_B4_16(0x45220b3)); /* 0x207af80 */
  act_hv_translator.do_writes(&tu);
    tu.IndirectWrite(0x0200801001d1, 0x00001e799c677a03, 0x00001d86639885fc); /* TCAM[ 0][ 0][465].word1 = 0xc331cc42fe  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080014ce8, 0x0000000000000000, 0x22e8bc0000001900); /* sram_ 5_ 3: a=0x20080014ce8 d0=0x0 d1=0x22e8bc0000001900 */
    tu.IndirectWrite(0x020080005dd1, 0x795454454e9234e7, 0x2c6ca071c0f0c2e5); /* sram_ 1_ 7: a=0x20080005dd1 d0=0x795454454e9234e7 d1=0x2c6ca071c0f0c2e5 */

    Phv *phv_in2 = tu.phv_alloc();
    phv_in2->set_ingress();
    phv_in2->set(  0, 0x476db1e8); 	/* [0, 0] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(  1, 0x99df072c); 	/* [0, 1] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(  2, 0x63db3a8f); 	/* [0, 2] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(  3, 0x28619ce4); 	/* [0, 3] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(  4, 0x876ba4af); 	/* [0, 4] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(  5, 0xc0a02440); 	/* [0, 5] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(  6, 0x46ef8d05); 	/* [0, 6] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(  7, 0xc6aabff0); 	/* [0, 7] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(  8, 0xe02a9f08); 	/* [0, 8] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(  9, 0x8fdaaf27); 	/* [0, 9] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 10, 0xff2b47d0); 	/* [0,10] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 11, 0x0570ce73); 	/* [0,11] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 12, 0xaa8efc94); 	/* [0,12] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 13, 0x9c621926); 	/* [0,13] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 14, 0x48522928); 	/* [0,14] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 15, 0x53a8a025); 	/* [0,15] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 16, 0x45ce569a); 	/* [0,16] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 17, 0xa8156bc5); 	/* [0,17] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 18, 0x247fe7c0); 	/* [0,18] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 19, 0x007a6d35); 	/* [0,19] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 20, 0xd9778f2c); 	/* [0,20] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 21, 0xe912c617); 	/* [0,21] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 22, 0xba1f5346); 	/* [0,22] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 23, 0xdd9359d2); 	/* [0,23] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 24, 0x4d54c4e4); 	/* [0,24] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 25, 0x2c7ef3cb); 	/* [0,25] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 26, 0x85b83401); 	/* [0,26] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 27, 0x518f9a5b); 	/* [0,27] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 28, 0x87270bc7); 	/* [0,28] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 29, 0x76e863f1); 	/* [0,29] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 30, 0xdeee41fb); 	/* [0,30] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 31, 0x015af5ab); 	/* [0,31] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 32, 0xe9d3f9ee); 	/* [1, 0] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 33, 0xccc23000); 	/* [1, 1] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 34, 0xaf4077a8); 	/* [1, 2] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 35, 0x862fd879); 	/* [1, 3] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 36, 0x0dc9ebf5); 	/* [1, 4] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 37, 0x4e82b09d); 	/* [1, 5] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 38, 0xb9fbc140); 	/* [1, 6] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 39, 0x8f2b6da3); 	/* [1, 7] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 40, 0xcaf4dfb4); 	/* [1, 8] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 41, 0xb620e889); 	/* [1, 9] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 42, 0xe6c6ca17); 	/* [1,10] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 43, 0x99c2a1d3); 	/* [1,11] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 44, 0xe0cb08b1); 	/* [1,12] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 45, 0x3abb4bc5); 	/* [1,13] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 46, 0x9165081e); 	/* [1,14] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 47, 0xe1c67f93); 	/* [1,15] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 48, 0xb25b2ce8); 	/* [1,16] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 49, 0x0489c540); 	/* [1,17] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 50, 0x142692a1); 	/* [1,18] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 51, 0x8fcb21e0); 	/* [1,19] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 52, 0x56311398); 	/* [1,20] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 53, 0x3ad77d8b); 	/* [1,21] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 54, 0x42f185bb); 	/* [1,22] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 55, 0xd54e6daf); 	/* [1,23] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 56, 0xcc4dc0e1); 	/* [1,24] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 57, 0x39856fb9); 	/* [1,25] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 58, 0xa9b9c167); 	/* [1,26] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 59, 0x767469d1); 	/* [1,27] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 60, 0xeaba281e); 	/* [1,28] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 61, 0x5c6ca4b1); 	/* [1,29] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 62, 0xaa7aa2cd); 	/* [1,30] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 63, 0xc1e69dbf); 	/* [1,31] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 64, 0xc6); 	/* [2, 0] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 65, 0xe1); 	/* [2, 1] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 66, 0x40); 	/* [2, 2] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 67, 0x4c); 	/* [2, 3] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 68, 0xbd); 	/* [2, 4] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 69, 0xf5); 	/* [2, 5] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 70, 0x5d); 	/* [2, 6] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 71, 0x07); 	/* [2, 7] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 72, 0xd4); 	/* [2, 8] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 73, 0x7b); 	/* [2, 9] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 74, 0xc7); 	/* [2,10] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 75, 0x68); 	/* [2,11] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 76, 0x51); 	/* [2,12] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 77, 0x3a); 	/* [2,13] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 78, 0xf2); 	/* [2,14] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 79, 0x6c); 	/* [2,15] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 80, 0xcf); 	/* [2,16] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 81, 0x52); 	/* [2,17] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 82, 0x0b); 	/* [2,18] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 83, 0x8f); 	/* [2,19] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 84, 0xa0); 	/* [2,20] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 85, 0x4a); 	/* [2,21] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 86, 0x0c); 	/* [2,22] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 87, 0x30); 	/* [2,23] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 88, 0x49); 	/* [2,24] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 89, 0x04); 	/* [2,25] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 90, 0xd0); 	/* [2,26] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 91, 0xac); 	/* [2,27] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 92, 0xe6); 	/* [2,28] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 93, 0x06); 	/* [2,29] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 94, 0xd1); 	/* [2,30] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 95, 0x96); 	/* [2,31] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 96, 0xe3); 	/* [3, 0] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 97, 0xf5); 	/* [3, 1] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 98, 0x31); 	/* [3, 2] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 99, 0xaf); 	/* [3, 3] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(100, 0xda); 	/* [3, 4] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(101, 0x75); 	/* [3, 5] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(102, 0xce); 	/* [3, 6] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(103, 0xa1); 	/* [3, 7] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(104, 0x13); 	/* [3, 8] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(105, 0x09); 	/* [3, 9] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(106, 0x55); 	/* [3,10] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(107, 0xb2); 	/* [3,11] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(108, 0xbd); 	/* [3,12] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(109, 0x07); 	/* [3,13] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(110, 0x47); 	/* [3,14] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(111, 0xbc); 	/* [3,15] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(112, 0x58); 	/* [3,16] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(113, 0x69); 	/* [3,17] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(114, 0x2d); 	/* [3,18] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(115, 0x83); 	/* [3,19] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(116, 0x10); 	/* [3,20] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(117, 0x0d); 	/* [3,21] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(118, 0x13); 	/* [3,22] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(119, 0x6b); 	/* [3,23] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(120, 0x8d); 	/* [3,24] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(121, 0x62); 	/* [3,25] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(122, 0x92); 	/* [3,26] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(123, 0x5c); 	/* [3,27] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(124, 0x45); 	/* [3,28] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(125, 0x96); 	/* [3,29] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(126, 0xd3); 	/* [3,30] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(127, 0xd2); 	/* [3,31] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(128, 0x1f77); 	/* [4, 0] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(129, 0x8328); 	/* [4, 1] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(130, 0x540f); 	/* [4, 2] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(131, 0x8486); 	/* [4, 3] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(132, 0x1f0c); 	/* [4, 4] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(133, 0x588e); 	/* [4, 5] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(134, 0xd6e2); 	/* [4, 6] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(135, 0x6dfd); 	/* [4, 7] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(136, 0xac0f); 	/* [4, 8] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(137, 0x4ecc); 	/* [4, 9] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(138, 0xefef); 	/* [4,10] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(139, 0x088e); 	/* [4,11] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(140, 0x3f53); 	/* [4,12] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(141, 0x2f3c); 	/* [4,13] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(142, 0x5575); 	/* [4,14] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(143, 0x9af7); 	/* [4,15] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(144, 0x258b); 	/* [4,16] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(145, 0x061b); 	/* [4,17] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(146, 0x52b2); 	/* [4,18] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(147, 0x40d4); 	/* [4,19] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(148, 0x8fe2); 	/* [4,20] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(149, 0x035b); 	/* [4,21] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(150, 0x005b); 	/* [4,22] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(151, 0x8419); 	/* [4,23] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(152, 0x898c); 	/* [4,24] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(153, 0xae2e); 	/* [4,25] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(154, 0x857c); 	/* [4,26] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(155, 0xc6a8); 	/* [4,27] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(156, 0x62f4); 	/* [4,28] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(157, 0x28e0); 	/* [4,29] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(158, 0x522b); 	/* [4,30] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(159, 0x1371); 	/* [4,31] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(160, 0xf742); 	/* [5, 0] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(161, 0xfe85); 	/* [5, 1] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(162, 0x7742); 	/* [5, 2] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(163, 0xccce); 	/* [5, 3] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(164, 0x1231); 	/* [5, 4] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(165, 0xc3f9); 	/* [5, 5] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(166, 0xbac1); 	/* [5, 6] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(167, 0x3433); 	/* [5, 7] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(168, 0x7aaa); 	/* [5, 8] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(169, 0x201a); 	/* [5, 9] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(170, 0x4c5e); 	/* [5,10] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(171, 0x05ce); 	/* [5,11] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(172, 0xe248); 	/* [5,12] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(173, 0x4c8a); 	/* [5,13] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(174, 0x50fb); 	/* [5,14] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(175, 0x9fd2); 	/* [5,15] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(176, 0x7e8d); 	/* [5,16] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(177, 0x133f); 	/* [5,17] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(178, 0x8438); 	/* [5,18] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(179, 0xa5e7); 	/* [5,19] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(180, 0xd9bd); 	/* [5,20] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(181, 0xe81c); 	/* [5,21] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(182, 0x64a3); 	/* [5,22] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(183, 0x9640); 	/* [5,23] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(184, 0xd9d5); 	/* [5,24] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(185, 0xa5f6); 	/* [5,25] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(186, 0xf0fa); 	/* [5,26] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(187, 0xecba); 	/* [5,27] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(188, 0x06b7); 	/* [5,28] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(189, 0xcd74); 	/* [5,29] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(190, 0x34f6); 	/* [5,30] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(191, 0xdb78); 	/* [5,31] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(192, 0x757d); 	/* [6, 0] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(193, 0x1986); 	/* [6, 1] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(194, 0x25eb); 	/* [6, 2] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(195, 0x5dac); 	/* [6, 3] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(196, 0x9bbb); 	/* [6, 4] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(197, 0xe2ea); 	/* [6, 5] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(198, 0x9e31); 	/* [6, 6] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(199, 0x1b2a); 	/* [6, 7] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(200, 0x83db); 	/* [6, 8] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(201, 0x6167); 	/* [6, 9] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(202, 0x3b46); 	/* [6,10] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(203, 0x95ec); 	/* [6,11] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(204, 0xdcef); 	/* [6,12] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(205, 0x9d47); 	/* [6,13] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(206, 0x304e); 	/* [6,14] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(207, 0x31fe); 	/* [6,15] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(208, 0x370d); 	/* [6,16] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(209, 0xa147); 	/* [6,17] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(210, 0xabf2); 	/* [6,18] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(211, 0x8181); 	/* [6,19] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(212, 0x57aa); 	/* [6,20] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(213, 0x8fad); 	/* [6,21] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(214, 0xfe3b); 	/* [6,22] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(215, 0x1b5e); 	/* [6,23] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(216, 0x46c0); 	/* [6,24] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(217, 0xa8c9); 	/* [6,25] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(218, 0xa4cb); 	/* [6,26] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(219, 0xfb64); 	/* [6,27] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(220, 0x04b7); 	/* [6,28] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(221, 0xc779); 	/* [6,29] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(222, 0x3e67); 	/* [6,30] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(223, 0xd520); 	/* [6,31] v=1  #0# e=0 RefModel iPhv 2o */


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
    RMT_UT_LOG_INFO("Dv13Test::InPkt=%p [DA=%04X%08X]\n", p_in,
                phv_in1->get(Phv::make_word(4,0)), phv_in1->get(Phv::make_word(0,0)));
    Phv *phv_out1 = tu.port_process_inbound(port, phv_in1);
    // Free PHVs}}
    if ((phv_out1 != NULL) && (phv_out1 != phv_in1)) tu.phv_free(phv_out1);
    tu.phv_free(phv_in1);
    */
    
    

    // Uncomment below to up the debug output
    //flags = ALL;
    //tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Switch on all parse loop output
    uint64_t pipes2 = UINT64_C(1) << pipe;
    uint64_t types2 = UINT64_C(1) << RmtTypes::kRmtTypeParser;
    uint64_t flags2 = RmtDebug::kRmtDebugParserParseLoop;
    tu.update_log_flags(pipes2, 0, types2, 0, 0, flags2, ALL);


    Phv *phv_out2 = tu.port_process_inbound(port, phv_in2);    
    //EXPECT_EQ(0x18b703d2, phv_out2->get(52)); // was 0x56311398 in input phv
    EXPECT_EQ(0x3e196240u, phv_out2->get(57)); // was 0x39856fb9 in input phv
    //EXPECT_EQ(0x68u, phv_out2->get(86)); // was 0xc in input phv
    //EXPECT_EQ(0x72b7u, phv_out2->get(150)); // was 0x5b in input phv
    //EXPECT_EQ(0x279bu, phv_out2->get(151)); // was 0x8419 in input phv
    //EXPECT_EQ(0xe088u, phv_out2->get(158)); // was 0x522b in input phv
    //EXPECT_EQ(0xc24fu, phv_out2->get(193)); // was 0x1986 in input phv
    EXPECT_EQ(0xffffu, phv_out2->get(222)); // was 0x3e67 in input phv
    EXPECT_EQ(0xffffu, phv_out2->get(223)); // was 0xd520 in input phv

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

    // Check other PHVs stayed the same
    //EXPECT_EQ(64u, phv_out2->get(64));
    //EXPECT_EQ(96u, phv_out2->get(96));
    //EXPECT_EQ(128u, phv_out2->get(128));
    
    // Free PHVs

    tu.finish_test();
    tu.quieten_log_flags();
}


}
