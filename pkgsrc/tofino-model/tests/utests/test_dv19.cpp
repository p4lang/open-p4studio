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

// XXX -> test_dv19.cpp
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

  bool dv19_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv19Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv19_print) RMT_UT_LOG_INFO("test_dv19_packet1()\n");
    
    // ORIGINAL test had r_action_o_mux_select == 0x0
    // But that prevents RHS ActionSRAMs outputting to ActionHVBus
    // So CHANGE test to use value 0x1
    uint32_t r_action_muxsel = 0x1;

    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_evaluate_all(true);
    tu.set_free_on_exit(true);
    tu.set_dv_test(19);
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
    //tu.OutWord(&mau_reg_map.dp.final_output_delay[0][0], 0x4); /* 0x2060040 */
    tu.OutWord(&mau_reg_map.dp.pipelength_added_stages[0], 0x2); /* 0x2060070 */
    tu.OutWord(&mau_reg_map.dp.action_output_delay[1], 0xa); /* 0x2060038 */
    //Removed in bfnregs 20150107_182406_7982_mau_dev
    //tu.OutWord(&mau_reg_map.dp.final_output_delay[1][0], 0x1); /* 0x2060048 */
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
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_mode[0], 0x7515004); /* 0x20407c0 */
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_table_map[0], 0x1); /* 0x2040780 */
    tu.OutWord(&mau_reg_map.tcams.tcam_output_table_thread[0], 0x1); /* 0x2040400 */
    tu.OutWord(&mau_reg_map.tcams.tcam_match_adr_shift[0], 0x4); /* 0x2040460 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.unitram_config[0][5], 0x3856); /* 0x2009fa8 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.unitram_config[1][5], 0x3862); /* 0x200ffe8 */
    tu.OutWord(&mau_reg_map.rams.array.row[1].ram[5].unit_ram_ctl, 0x200); /* 0x2039298 */
    tu.OutWord(&mau_reg_map.rams.array.row[7].ram[11].unit_ram_ctl, 0x20); /* 0x203f598 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tind_ram_data_size[2], 0x5); /* 0x2026148 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][0], 0x5); /* 0x2040300 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][0], 0x4); /* 0x2040000 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][1], 0x3); /* 0x2040004 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][2], 0x3); /* 0x2040008 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][3], 0x2); /* 0x204000c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][4], 0x5); /* 0x2040010 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][5], 0x9); /* 0x2040014 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][6], 0x6); /* 0x2040018 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][7], 0x5); /* 0x204001c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][0], 0xd); /* 0x2040380 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][0], 0x6); /* 0x2040200 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][1], 0x9); /* 0x2040384 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][1], 0x5); /* 0x2040204 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][0], 0xd); /* 0x2040320 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][0], 0x3); /* 0x2040100 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][1], 0x1); /* 0x2040104 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][2], 0x4); /* 0x2040108 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][3], 0x2); /* 0x204010c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][4], 0x7); /* 0x2040110 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][5], 0xa); /* 0x2040114 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][6], 0x8); /* 0x2040118 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][7], 0x9); /* 0x204011c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][0], 0x19); /* 0x20403c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][0], 0x5); /* 0x2040240 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][1], 0x7); /* 0x20403c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][1], 0x7); /* 0x2040244 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][1], 0x7); /* 0x2040304 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][0], 0x1); /* 0x2040020 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][1], 0x2); /* 0x2040024 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][2], 0x5); /* 0x2040028 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][3], 0x0); /* 0x204002c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][4], 0xa); /* 0x2040030 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][5], 0x9); /* 0x2040034 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][6], 0x5); /* 0x2040038 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][7], 0xa); /* 0x204003c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][2], 0x17); /* 0x2040388 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][2], 0x5); /* 0x2040208 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][3], 0xe); /* 0x204038c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][3], 0x5); /* 0x204020c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][1], 0x1); /* 0x2040324 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][0], 0x4); /* 0x2040120 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][1], 0x0); /* 0x2040124 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][2], 0x4); /* 0x2040128 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][3], 0x1); /* 0x204012c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][4], 0x7); /* 0x2040130 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][5], 0xa); /* 0x2040134 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][6], 0x9); /* 0x2040138 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][7], 0x7); /* 0x204013c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][2], 0x6); /* 0x20403c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][2], 0x4); /* 0x2040248 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][3], 0xb); /* 0x20403cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][3], 0x7); /* 0x204024c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][2], 0x7); /* 0x2040308 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][0], 0x4); /* 0x2040040 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][1], 0x5); /* 0x2040044 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][2], 0x0); /* 0x2040048 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][3], 0x5); /* 0x204004c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][4], 0xa); /* 0x2040050 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][5], 0x5); /* 0x2040054 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][6], 0x5); /* 0x2040058 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][7], 0x5); /* 0x204005c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][4], 0xa); /* 0x2040390 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][4], 0x4); /* 0x2040210 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][5], 0xe); /* 0x2040394 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][5], 0x5); /* 0x2040214 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][2], 0xc); /* 0x2040328 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][0], 0x2); /* 0x2040140 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][1], 0x2); /* 0x2040144 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][2], 0x4); /* 0x2040148 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][3], 0x4); /* 0x204014c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][4], 0x5); /* 0x2040150 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][5], 0x8); /* 0x2040154 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][6], 0x5); /* 0x2040158 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][7], 0x9); /* 0x204015c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][4], 0x10); /* 0x20403d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][4], 0x6); /* 0x2040250 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][5], 0x4); /* 0x20403d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][5], 0x6); /* 0x2040254 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][3], 0x9); /* 0x204030c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][0], 0x5); /* 0x2040060 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][1], 0x5); /* 0x2040064 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][2], 0x1); /* 0x2040068 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][3], 0x5); /* 0x204006c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][4], 0x9); /* 0x2040070 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][5], 0x5); /* 0x2040074 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][6], 0x7); /* 0x2040078 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][7], 0xa); /* 0x204007c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][6], 0x17); /* 0x2040398 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][6], 0x5); /* 0x2040218 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][7], 0x2); /* 0x204039c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][7], 0x6); /* 0x204021c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][3], 0x2); /* 0x204032c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][0], 0x5); /* 0x2040160 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][1], 0x5); /* 0x2040164 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][2], 0x3); /* 0x2040168 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][3], 0x4); /* 0x204016c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][4], 0x9); /* 0x2040170 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][5], 0x9); /* 0x2040174 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][6], 0xa); /* 0x2040178 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][7], 0x7); /* 0x204017c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][6], 0x8); /* 0x20403d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][6], 0x7); /* 0x2040258 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][7], 0xd); /* 0x20403dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][7], 0x7); /* 0x204025c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][4], 0xa); /* 0x2040310 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][0], 0x3); /* 0x2040080 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][1], 0x3); /* 0x2040084 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][2], 0x0); /* 0x2040088 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][3], 0x1); /* 0x204008c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][4], 0x7); /* 0x2040090 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][5], 0x6); /* 0x2040094 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][6], 0x7); /* 0x2040098 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][7], 0xa); /* 0x204009c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][8], 0x8); /* 0x20403a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][8], 0x5); /* 0x2040220 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][9], 0x1d); /* 0x20403a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][9], 0x4); /* 0x2040224 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][4], 0x7); /* 0x2040330 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][0], 0x2); /* 0x2040180 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][1], 0x1); /* 0x2040184 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][2], 0x0); /* 0x2040188 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][3], 0x3); /* 0x204018c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][4], 0xa); /* 0x2040190 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][5], 0xa); /* 0x2040194 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][6], 0xa); /* 0x2040198 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][7], 0x9); /* 0x204019c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][8], 0x2); /* 0x20403e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][8], 0x5); /* 0x2040260 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][9], 0x8); /* 0x20403e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][9], 0x5); /* 0x2040264 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][5], 0x5); /* 0x2040314 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][0], 0x1); /* 0x20400a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][1], 0x2); /* 0x20400a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][2], 0x2); /* 0x20400a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][3], 0x3); /* 0x20400ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][4], 0x9); /* 0x20400b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][5], 0x8); /* 0x20400b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][6], 0x6); /* 0x20400b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][7], 0x7); /* 0x20400bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][10], 0x1d); /* 0x20403a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][10], 0x5); /* 0x2040228 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][11], 0x1); /* 0x20403ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][11], 0x6); /* 0x204022c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][5], 0xe); /* 0x2040334 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][0], 0x3); /* 0x20401a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][1], 0x1); /* 0x20401a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][2], 0x5); /* 0x20401a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][3], 0x4); /* 0x20401ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][4], 0x7); /* 0x20401b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][5], 0x8); /* 0x20401b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][6], 0x7); /* 0x20401b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][7], 0x7); /* 0x20401bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][10], 0xc); /* 0x20403e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][10], 0x4); /* 0x2040268 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][11], 0xd); /* 0x20403ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][11], 0x7); /* 0x204026c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][6], 0xb); /* 0x2040318 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][0], 0x0); /* 0x20400c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][1], 0x5); /* 0x20400c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][2], 0x4); /* 0x20400c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][3], 0x2); /* 0x20400cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][4], 0x5); /* 0x20400d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][5], 0x6); /* 0x20400d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][6], 0x8); /* 0x20400d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][7], 0x8); /* 0x20400dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][12], 0x11); /* 0x20403b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][12], 0x4); /* 0x2040230 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][13], 0xe); /* 0x20403b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][13], 0x5); /* 0x2040234 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][6], 0x4); /* 0x2040338 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][0], 0x5); /* 0x20401c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][1], 0x1); /* 0x20401c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][2], 0x3); /* 0x20401c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][3], 0x3); /* 0x20401cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][4], 0x7); /* 0x20401d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][5], 0xa); /* 0x20401d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][6], 0xa); /* 0x20401d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][7], 0x5); /* 0x20401dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][12], 0x0); /* 0x20403f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][12], 0x7); /* 0x2040270 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][13], 0x1); /* 0x20403f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][13], 0x4); /* 0x2040274 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][7], 0x0); /* 0x204031c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][0], 0x1); /* 0x20400e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][1], 0x1); /* 0x20400e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][2], 0x5); /* 0x20400e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][3], 0x3); /* 0x20400ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][4], 0x6); /* 0x20400f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][5], 0x5); /* 0x20400f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][6], 0x5); /* 0x20400f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][7], 0x5); /* 0x20400fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][14], 0x4); /* 0x20403b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][14], 0x5); /* 0x2040238 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][15], 0x10); /* 0x20403bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][15], 0x6); /* 0x204023c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][7], 0xd); /* 0x204033c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][0], 0x4); /* 0x20401e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][1], 0x0); /* 0x20401e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][2], 0x1); /* 0x20401e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][3], 0x3); /* 0x20401ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][4], 0x6); /* 0x20401f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][5], 0x5); /* 0x20401f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][6], 0xa); /* 0x20401f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][7], 0x9); /* 0x20401fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][14], 0xc); /* 0x20403f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][14], 0x6); /* 0x2040278 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][15], 0xd); /* 0x20403fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][15], 0x6); /* 0x204027c */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x10); /* 0x2038e58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x2038e40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x2038e44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x2038e48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x2038e4c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x11); /* 0x2038e78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x2038e60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x2038e64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x2038e68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x2038e6c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x12); /* 0x2039e58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x2039e40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x2039e44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x2039e48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x2039e4c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x13); /* 0x2039e78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x2039e60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x2039e64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x2039e68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x2039e6c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x14); /* 0x203ae58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x203ae40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x203ae44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x203ae48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x203ae4c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x15); /* 0x203ae78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x203ae60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x203ae64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x203ae68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x203ae6c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x16); /* 0x203be58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x203be40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x203be44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x203be48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x203be4c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x17); /* 0x203be78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x203be60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x203be64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x203be68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x203be6c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x10); /* 0x203ce58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x203ce40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x203ce44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x203ce48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x203ce4c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x11); /* 0x203ce78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x203ce60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x203ce64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x203ce68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x203ce6c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x12); /* 0x203de58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x203de40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x203de44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x203de48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x203de4c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x13); /* 0x203de78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x203de60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x203de64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x203de68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x203de6c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x14); /* 0x203ee58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x203ee40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x203ee44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x203ee48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x203ee4c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x15); /* 0x203ee78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x203ee60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x203ee64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x203ee68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x203ee6c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x16); /* 0x203fe58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x203fe40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x203fe44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x203fe48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x203fe4c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x17); /* 0x203fe78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x203fe60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x203fe64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x203fe68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x203fe6c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0x8); /* 0x2038f80 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_mem_hashadr_xbar_ctl[0], 0x10); /* 0x2038fc0 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_bank_enable[0], 0x1000000); /* 0x2038f00 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0x8); /* 0x2038f80 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_mem_hashadr_xbar_ctl[0], 0x10); /* 0x2038fc0 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_bank_enable[0], 0x1000000); /* 0x2038f00 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0x8); /* 0x2038f80 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_mem_hashadr_xbar_ctl[0], 0x10); /* 0x2038fc0 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_bank_enable[0], 0x1000000); /* 0x2038f00 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0x8); /* 0x2038f80 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_mem_hashadr_xbar_ctl[0], 0x10); /* 0x2038fc0 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_bank_enable[0], 0x1000000); /* 0x2038f00 */
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x1f); // ADDED EMVH070915
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
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1f); // ADDED EMVH070915
    tu.IndirectWrite(0x02008001edd3, 0x7748dc2d5cd08bdd, 0x773e87b51f205e8c); /* sram_ 7_11: a=0x2008001edd3 d0=0x7748dc2d5cd08bdd d1=0x773e87b51f205e8c */
    //IndirectWrite 0x773e87b51f205e8c 7748dc2d5cd08bdd to MauMemory (0x000002008001edd3) offset 0x1edd3
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_table_prop[0], 0x4); /* 0x2024440 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tind_bus_prop[2], 0x4); /* 0x20274c8 */
  //    tu.OutWord(&mau_reg_map.rams.match.merge.exact_match_delay_config, 0xa0000); /* 0x2024620 */ // REMOVED EMDEL070915
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_hit_to_logical_table_ixbar_outputmap[0], 0x1e); /* 0x2024400 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_match_adr_to_physical_oxbar_outputmap[2], 0x8); /* 0x2026108 */
    tu.OutWord(&mau_reg_map.rams.match.merge.match_to_logical_table_ixbar_outputmap[1][2], 0x1e); /* 0x20246c8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_tcam_shiftcount[2], 0x24); /* 0x2026248 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_mask[1][2], 0x3f); /* 0x2024948 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_default[1][2], 0x0); /* 0x20249c8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_tcam_shiftcount[2], 0x0); /* 0x20261c8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_mask[1][2], 0x0); /* 0x20247c8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_default[1][2], 0x0); /* 0x2024848 */
    tu.OutWord(&mau_reg_map.rams.match.merge.next_table_format_data[14], 0xff0000); /* 0x20261b8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_tcam_shiftcount[2], 0x2f); /* 0x20262c8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_mask[1][2], 0x3fffff); /* 0x2024ac8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_default[1][2], 0x0); /* 0x2024b48 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_miss_value[14], 0x23); /* 0x20262b8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_miss_value[14], 0x1d3); /* 0x2026338 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.immediate_data_32b_ixbar_ctl[14], 0x20); /* 0x20203b8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.predication_ctl[0], 0xc284000); /* 0x2024470 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].vh_xbars.adr_dist_tind_adr_xbar_ctl[1], 0x8); /* 0x2009338 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.ram_address_mux_ctl[0][5], 0xc0); /* 0x2009f28 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.ram_address_mux_ctl[1][5], 0x40); /* 0x200ff68 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.adr_dist_action_data_adr_icxbar_ctl[14], 0x8000); /* 0x20203f8 */
  ActionHvTranslator act_hv_translator;
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x1); /* 0x203f884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(7,1,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x2); /* 0x203f88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(7,1,0x2); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x3); /* 0x203f884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(7,1,0x3); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xa); /* 0x203f88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(7,1,0xa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x7); /* 0x203f884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(7,1,0x7); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x2a); /* 0x203f88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(7,1,0x2a); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0xf); /* 0x203f884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(7,1,0xf); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xaa); /* 0x203f88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(7,1,0xaa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x10f); /* 0x203f884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(7,1,0x10f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x300aa); /* 0x203f88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(7,1,0x300aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x30f); /* 0x203f884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(7,1,0x30f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xf00aa); /* 0x203f88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(7,1,0xf00aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x70f); /* 0x203f884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(7,1,0x70f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x3f00aa); /* 0x203f88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(7,1,0x3f00aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0xf0f); /* 0x203f884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(7,1,0xf0f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xff00aa); /* 0x203f88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(7,1,0xff00aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x1f0f); /* 0x203f884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(7,1,0x1f0f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x3ff00aa); /* 0x203f88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(7,1,0x3ff00aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x3f0f); /* 0x203f884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(7,1,0x3f0f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xfff00aa); /* 0x203f88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(7,1,0xfff00aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x7f0f); /* 0x203f884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(7,1,0x7f0f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x3fff00aa); /* 0x203f88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(7,1,0x3fff00aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0xff0f); /* 0x203f884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(7,1,0xff0f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xffff00aa); /* 0x203f88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(7,1,0xffff00aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_half[1][3], 0x1); /* 0x203f8ec */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(7,1,3,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_half[1][3], 0x1); /* 0x203f8ec */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(7,1,3,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_half[1][3], 0x5); /* 0x203f8ec */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(7,1,3,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_half[1][3], 0x5); /* 0x203f8ec */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(7,1,3,0x5); // ADDED ACHV070915
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[0].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x20300a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[0].ctl.r_l_action_o_mux_select, 0x10); /* 0x2030080 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[1].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x20301a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[1].ctl.r_l_action_o_mux_select, 0x10); /* 0x2030180 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[2].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x20302a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[2].ctl.r_l_action_o_mux_select, 0x10); /* 0x2030280 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[3].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x20303a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[3].ctl.r_l_action_o_mux_select, 0x10); /* 0x2030380 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[4].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x20304a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[4].ctl.r_l_action_o_mux_select, 0x10); /* 0x2030480 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[5].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x20305a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[5].ctl.r_l_action_o_mux_select, 0x10); /* 0x2030580 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[6].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x20306a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[6].ctl.r_l_action_o_mux_select, 0x10); /* 0x2030680 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[7].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x20307a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[7].ctl.r_l_action_o_mux_select, 0x10); /* 0x2030780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[0][13], RM_B4_8(0x97e94d)); /* 0x207e034 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[1][19], RM_B4_8(0xd36342)); /* 0x207e0cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[2][23], RM_B4_8(0x97e941)); /* 0x207e15c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[3][11], RM_B4_8(0xad63ae)); /* 0x207e1ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[4][29], RM_B4_8(0x1be287)); /* 0x207e274 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[5][23], RM_B4_8(0xdfe097)); /* 0x207e2dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[6][11], RM_B4_8(0xe9634d)); /* 0x207e32c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[7][1], RM_B4_8(0xd761d1)); /* 0x207e384 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[8][3], RM_B4_8(0x67e901)); /* 0x207e40c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[9][27], RM_B4_8(0x27e0ec)); /* 0x207e4ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[10][19], RM_B4_8(0xc3e819)); /* 0x207e54c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[11][9], RM_B4_8(0xbb69b8)); /* 0x207e5a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[12][6], RM_B4_8(0x5fe2ec)); /* 0x207e618 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[13][17], RM_B4_8(0xcfe17c)); /* 0x207e6c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[14][21], RM_B4_8(0x176905)); /* 0x207e754 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[15][14], RM_B4_8(0x9362a2)); /* 0x207e7b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[16][29], RM_B4_8(0x13e2b6)); /* 0x207e874 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[17][18], RM_B4_8(0xb5681d)); /* 0x207e8c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[18][11], RM_B4_8(0x23e862)); /* 0x207e92c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[19][27], RM_B4_8(0xd569f3)); /* 0x207e9ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[20][2], RM_B4_8(0xcbe332)); /* 0x207ea08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[21][16], RM_B4_8(0x57e2e1)); /* 0x207eac0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[22][10], RM_B4_8(0xf569c7)); /* 0x207eb28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[23][15], RM_B4_8(0x7e3a0)); /* 0x207ebbc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[24][11], RM_B4_8(0xe9630f)); /* 0x207ec2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[25][26], RM_B4_8(0x6d69dd)); /* 0x207ece8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[26][26], RM_B4_8(0xd762cf)); /* 0x207ed68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[27][15], RM_B4_8(0x27e9c7)); /* 0x207edbc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[28][0], RM_B4_8(0x41693e)); /* 0x207ee00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[29][13], RM_B4_8(0x57e86f)); /* 0x207eeb4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[30][27], RM_B4_8(0x936826)); /* 0x207ef6c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[31][4], RM_B4_8(0xc162f2)); /* 0x207ef90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[32][28], RM_B4_8(0x81686d)); /* 0x207f070 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[33][8], RM_B4_8(0x176372)); /* 0x207f0a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[34][13], RM_B4_8(0xe7e9c3)); /* 0x207f134 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[35][7], RM_B4_8(0xdb69b5)); /* 0x207f19c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[36][8], RM_B4_8(0xb7e980)); /* 0x207f220 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[37][22], RM_B4_8(0xd3e16f)); /* 0x207f2d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[38][14], RM_B4_8(0xd7e27f)); /* 0x207f338 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[39][5], RM_B4_8(0xc3e3cf)); /* 0x207f394 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[40][30], RM_B4_8(0x4be92e)); /* 0x207f478 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[41][23], RM_B4_8(0x83e864)); /* 0x207f4dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[42][3], RM_B4_8(0xe16989)); /* 0x207f50c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[43][29], RM_B4_8(0x76320)); /* 0x207f5f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[44][19], RM_B4_8(0x7369eb)); /* 0x207f64c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[45][24], RM_B4_8(0xdf625d)); /* 0x207f6e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[46][29], RM_B4_8(0x97e97b)); /* 0x207f774 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[47][20], RM_B4_8(0xc1627c)); /* 0x207f7d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[48][31], RM_B4_8(0xcfe21a)); /* 0x207f87c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[49][20], RM_B4_8(0x1be00b)); /* 0x207f8d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[50][31], RM_B4_8(0xcfe820)); /* 0x207f97c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[51][1], RM_B4_8(0x1b682a)); /* 0x207f984 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[52][15], RM_B4_8(0xa3e3f2)); /* 0x207fa3c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[53][2], RM_B4_8(0xd56905)); /* 0x207fa88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[54][15], RM_B4_8(0x5d6922)); /* 0x207fb3c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[55][15], RM_B4_8(0xb7e160)); /* 0x207fbbc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[56][17], RM_B4_8(0x67e9bf)); /* 0x207fc44 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[57][15], RM_B4_8(0x5b6804)); /* 0x207fcbc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[58][1], RM_B4_8(0x23e80e)); /* 0x207fd04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[59][14], RM_B4_8(0xcbe806)); /* 0x207fdb8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[60][7], RM_B4_8(0xff685c)); /* 0x207fe1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[61][7], RM_B4_8(0x16836)); /* 0x207fe9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[62][8], RM_B4_8(0x81680d)); /* 0x207ff20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[63][23], RM_B4_8(0xe7e9b4)); /* 0x207ffdc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[0][30], RM_B4_16(0x2016253)); /* 0x2078078 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[1][13], RM_B4_16(0x377e0b2)); /* 0x20780b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[2][13], RM_B4_16(0x307e82e)); /* 0x2078134 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[3][10], RM_B4_16(0x4356805)); /* 0x20781a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[4][28], RM_B4_16(0x1f7e2db)); /* 0x2078270 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[5][17], RM_B4_16(0x14be276)); /* 0x20782c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[6][1], RM_B4_16(0x4096828)); /* 0x2078304 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[7][8], RM_B4_16(0x6f7e228)); /* 0x20783a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[8][10], RM_B4_16(0x216292)); /* 0x2078428 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[9][17], RM_B4_16(0x17e357)); /* 0x20784c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[10][17], RM_B4_16(0x63f604f)); /* 0x2078544 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[11][9], RM_B4_16(0x1d3e155)); /* 0x20785a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[12][9], RM_B4_16(0x21f683a)); /* 0x2078624 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[13][31], RM_B4_16(0x41b680f)); /* 0x20786fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[14][30], RM_B4_16(0x603682e)); /* 0x2078778 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[15][28], RM_B4_16(0x3a3e81a)); /* 0x20787f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[16][0], RM_B4_16(0x147e1c8)); /* 0x2078800 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[17][1], RM_B4_16(0x236089)); /* 0x2078884 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[18][22], RM_B4_16(0x60f683f)); /* 0x2078958 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[19][0], RM_B4_16(0x633e82a)); /* 0x2078980 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[20][9], RM_B4_16(0x2236184)); /* 0x2078a24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[21][1], RM_B4_16(0x2b7e2bd)); /* 0x2078a84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[22][19], RM_B4_16(0x3cfe813)); /* 0x2078b4c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[23][6], RM_B4_16(0x4276376)); /* 0x2078b98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[24][1], RM_B4_16(0x2196822)); /* 0x2078c04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[25][26], RM_B4_16(0x61362c2)); /* 0x2078ce8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[26][8], RM_B4_16(0x62b632a)); /* 0x2078d20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[27][13], RM_B4_16(0xb3e834)); /* 0x2078db4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[28][29], RM_B4_16(0xd3e05a)); /* 0x2078e74 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[29][24], RM_B4_16(0x415683d)); /* 0x2078ee0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[30][10], RM_B4_16(0x23360aa)); /* 0x2078f28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[31][3], RM_B4_16(0x1f635e)); /* 0x2078f8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[32][4], RM_B4_16(0x23b607f)); /* 0x2079010 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[33][9], RM_B4_16(0x21f60d6)); /* 0x20790a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[34][23], RM_B4_16(0x61fe308)); /* 0x207915c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[35][2], RM_B4_16(0x21f631f)); /* 0x2079188 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[36][14], RM_B4_16(0x613609a)); /* 0x2079238 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[37][16], RM_B4_16(0x2962ff)); /* 0x20792c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[38][0], RM_B4_16(0x22d683e)); /* 0x2079300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[39][12], RM_B4_16(0x8be1f9)); /* 0x20793b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[40][3], RM_B4_16(0x4e3e166)); /* 0x207940c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[41][10], RM_B4_16(0x43b6186)); /* 0x20794a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[42][30], RM_B4_16(0x557e823)); /* 0x2079578 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[43][6], RM_B4_16(0x43b680d)); /* 0x2079598 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[44][21], RM_B4_16(0x10be100)); /* 0x2079654 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[45][11], RM_B4_16(0x23f6807)); /* 0x20796ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[46][29], RM_B4_16(0x2236814)); /* 0x2079774 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[47][10], RM_B4_16(0x29be82f)); /* 0x20797a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[48][28], RM_B4_16(0x61d63d5)); /* 0x2079870 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[49][21], RM_B4_16(0x49fe81a)); /* 0x20798d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[50][27], RM_B4_16(0x61b6828)); /* 0x207996c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[51][13], RM_B4_16(0x6a3e3dd)); /* 0x20799b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[52][27], RM_B4_16(0x40760fa)); /* 0x2079a6c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[53][26], RM_B4_16(0x19617b)); /* 0x2079ae8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[54][0], RM_B4_16(0x43f605b)); /* 0x2079b00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[55][17], RM_B4_16(0x21fe837)); /* 0x2079bc4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[56][25], RM_B4_16(0x48be810)); /* 0x2079c64 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[57][1], RM_B4_16(0x4236093)); /* 0x2079c84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[58][20], RM_B4_16(0x2236332)); /* 0x2079d50 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[59][19], RM_B4_16(0x3d635d)); /* 0x2079dcc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[60][26], RM_B4_16(0x683e803)); /* 0x2079e68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[61][18], RM_B4_16(0x487e050)); /* 0x2079ec8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[62][10], RM_B4_16(0x41be151)); /* 0x2079f28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[63][15], RM_B4_16(0x201635e)); /* 0x2079fbc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[64][15], RM_B4_16(0x4396839)); /* 0x207a03c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[65][8], RM_B4_16(0x4256356)); /* 0x207a0a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[66][14], RM_B4_16(0x5fe824)); /* 0x207a138 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[67][15], RM_B4_16(0x4a3e838)); /* 0x207a1bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[68][23], RM_B4_16(0x3a3e825)); /* 0x207a25c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[69][13], RM_B4_16(0x35633f)); /* 0x207a2b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[70][18], RM_B4_16(0x3d6821)); /* 0x207a348 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[71][21], RM_B4_16(0x5fe3cd)); /* 0x207a3d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[72][19], RM_B4_16(0x4f3e815)); /* 0x207a44c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[73][5], RM_B4_16(0x1d6060)); /* 0x207a494 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[74][19], RM_B4_16(0xb7e334)); /* 0x207a54c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[75][28], RM_B4_16(0x43d680e)); /* 0x207a5f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[76][18], RM_B4_16(0x4376815)); /* 0x207a648 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[77][22], RM_B4_16(0x96238)); /* 0x207a6d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[78][7], RM_B4_16(0x41960e4)); /* 0x207a71c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[79][9], RM_B4_16(0x48fe0f4)); /* 0x207a7a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[80][28], RM_B4_16(0x7dbe81b)); /* 0x207a870 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[81][3], RM_B4_16(0x6e3e31a)); /* 0x207a88c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[82][12], RM_B4_16(0x78be11d)); /* 0x207a930 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[83][28], RM_B4_16(0x116801)); /* 0x207a9f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[84][17], RM_B4_16(0x343e82d)); /* 0x207aa44 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[85][23], RM_B4_16(0x2f6827)); /* 0x207aadc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[86][30], RM_B4_16(0x63761da)); /* 0x207ab78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[87][13], RM_B4_16(0x153e095)); /* 0x207abb4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[88][4], RM_B4_16(0x647e035)); /* 0x207ac10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[89][30], RM_B4_16(0x71be833)); /* 0x207acf8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[90][2], RM_B4_16(0x4216817)); /* 0x207ad08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[91][4], RM_B4_16(0x5cfe828)); /* 0x207ad90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[92][17], RM_B4_16(0x2d3e1e6)); /* 0x207ae44 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[93][16], RM_B4_16(0x2f7e3c9)); /* 0x207aec0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[94][1], RM_B4_16(0x21f6818)); /* 0x207af04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[95][22], RM_B4_16(0x61963f0)); /* 0x207afd8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][0], 0xdbfd); /* 0x2074000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][1], 0x37599); /* 0x2074004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][2], 0x9136); /* 0x2074008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][3], 0x29d22); /* 0x207400c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][4], 0x1d121); /* 0x2074010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][5], 0x2ac9e); /* 0x2074014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][6], 0x13303); /* 0x2074018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][7], 0x18e64); /* 0x207401c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][8], 0x25631); /* 0x2074020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][9], 0xbb66); /* 0x2074024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][10], 0x37311); /* 0x2074028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][11], 0x24a74); /* 0x207402c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][12], 0x1fd18); /* 0x2074030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][13], 0x19d40); /* 0x2074034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][14], 0x1d399); /* 0x2074038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][15], 0x6893); /* 0x207403c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][16], 0x2e156); /* 0x2074040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][17], 0x3d1f7); /* 0x2074044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][18], 0xb88e); /* 0x2074048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][19], 0x3ae44); /* 0x207404c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][20], 0x18b19); /* 0x2074050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][21], 0x394d2); /* 0x2074054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][22], 0x3f685); /* 0x2074058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][23], 0x10601); /* 0x207405c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][24], 0x2117e); /* 0x2074060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][25], 0x2a8a1); /* 0x2074064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][26], 0x147be); /* 0x2074068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][27], 0x10588); /* 0x207406c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][28], 0x29cc6); /* 0x2074070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][29], 0x2b83); /* 0x2074074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][30], 0x22741); /* 0x2074078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][31], 0x2fd11); /* 0x207407c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][32], 0x258e5); /* 0x2074080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][33], 0x75cc); /* 0x2074084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][34], 0xdd97); /* 0x2074088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][35], 0x3e5c4); /* 0x207408c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][36], 0x1b0d4); /* 0x2074090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][37], 0x276b9); /* 0x2074094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][38], 0x1f86f); /* 0x2074098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][39], 0x1191d); /* 0x207409c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][40], 0x35860); /* 0x20740a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][41], 0x1730a); /* 0x20740a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][42], 0x26b7a); /* 0x20740a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][43], 0x219c2); /* 0x20740ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][44], 0x3e180); /* 0x20740b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][45], 0x168f8); /* 0x20740b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][46], 0x7ce1); /* 0x20740b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][47], 0x12289); /* 0x20740bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][48], 0x3f5ae); /* 0x20740c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][49], 0x5d75); /* 0x20740c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][50], 0x3ce56); /* 0x20740c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][51], 0x14c06); /* 0x20740cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][0], 0x258a6); /* 0x2074100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][1], 0x2de23); /* 0x2074104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][2], 0x175ed); /* 0x2074108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][3], 0xc19b); /* 0x207410c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][4], 0x24a7b); /* 0x2074110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][5], 0x671c); /* 0x2074114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][6], 0xec1f); /* 0x2074118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][7], 0x1670f); /* 0x207411c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][8], 0xaa8d); /* 0x2074120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][9], 0x3085c); /* 0x2074124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][10], 0x4406); /* 0x2074128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][11], 0x31094); /* 0x207412c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][12], 0x2be72); /* 0x2074130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][13], 0x214fb); /* 0x2074134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][14], 0x3e5f8); /* 0x2074138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][15], 0x2fb25); /* 0x207413c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][16], 0x31891); /* 0x2074140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][17], 0x1f722); /* 0x2074144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][18], 0xaca7); /* 0x2074148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][19], 0x1dc00); /* 0x207414c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][20], 0x39dae); /* 0x2074150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][21], 0xeb56); /* 0x2074154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][22], 0x17864); /* 0x2074158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][23], 0x597f); /* 0x207415c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][24], 0x1a6e1); /* 0x2074160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][25], 0x303a5); /* 0x2074164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][26], 0x290d9); /* 0x2074168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][27], 0x8395); /* 0x207416c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][28], 0x17524); /* 0x2074170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][29], 0x15ee8); /* 0x2074174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][30], 0xb136); /* 0x2074178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][31], 0x3b706); /* 0x207417c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][32], 0x116c3); /* 0x2074180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][33], 0x1b97c); /* 0x2074184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][34], 0x2c7e2); /* 0x2074188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][35], 0x2e43c); /* 0x207418c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][36], 0xa281); /* 0x2074190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][37], 0x384e5); /* 0x2074194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][38], 0x29af9); /* 0x2074198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][39], 0x1b801); /* 0x207419c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][40], 0x2a3bb); /* 0x20741a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][41], 0x24b46); /* 0x20741a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][42], 0x1075); /* 0x20741a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][43], 0x18b3a); /* 0x20741ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][44], 0x39649); /* 0x20741b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][45], 0x1f7d9); /* 0x20741b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][46], 0x20dd); /* 0x20741b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][47], 0x9a43); /* 0x20741bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][48], 0x370e2); /* 0x20741c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][49], 0x1ae54); /* 0x20741c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][50], 0x26432); /* 0x20741c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][51], 0x2c29e); /* 0x20741cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][0], 0x3c544); /* 0x2074200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][1], 0x385f7); /* 0x2074204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][2], 0x5ea6); /* 0x2074208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][3], 0x1b3b7); /* 0x207420c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][4], 0x22667); /* 0x2074210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][5], 0x28d35); /* 0x2074214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][6], 0x1c011); /* 0x2074218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][7], 0x1293); /* 0x207421c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][8], 0x8fe1); /* 0x2074220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][9], 0x3c20e); /* 0x2074224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][10], 0x22c13); /* 0x2074228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][11], 0x10633); /* 0x207422c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][12], 0x378a2); /* 0x2074230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][13], 0x1ad07); /* 0x2074234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][14], 0xc353); /* 0x2074238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][15], 0x2fa46); /* 0x207423c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][16], 0x2ec5b); /* 0x2074240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][17], 0x256ff); /* 0x2074244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][18], 0x12448); /* 0x2074248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][19], 0x3137); /* 0x207424c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][20], 0x26cc6); /* 0x2074250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][21], 0x11faa); /* 0x2074254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][22], 0x33c96); /* 0x2074258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][23], 0x2d093); /* 0x207425c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][24], 0x3527); /* 0x2074260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][25], 0x2a9b8); /* 0x2074264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][26], 0x1ac6c); /* 0x2074268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][27], 0xf5b2); /* 0x207426c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][28], 0x281b3); /* 0x2074270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][29], 0x118d0); /* 0x2074274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][30], 0x158bd); /* 0x2074278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][31], 0x249dd); /* 0x207427c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][32], 0x1772e); /* 0x2074280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][33], 0x25d51); /* 0x2074284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][34], 0x3bb96); /* 0x2074288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][35], 0x11e40); /* 0x207428c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][36], 0x3de06); /* 0x2074290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][37], 0x2bbc7); /* 0x2074294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][38], 0x2721f); /* 0x2074298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][39], 0x2551e); /* 0x207429c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][40], 0x246c3); /* 0x20742a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][41], 0x1f088); /* 0x20742a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][42], 0x366a2); /* 0x20742a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][43], 0x1ab27); /* 0x20742ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][44], 0x35a7); /* 0x20742b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][45], 0xf7bd); /* 0x20742b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][46], 0x2ebba); /* 0x20742b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][47], 0x1f161); /* 0x20742bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][48], 0x257f1); /* 0x20742c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][49], 0x1cbb6); /* 0x20742c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][50], 0x2d94a); /* 0x20742c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][51], 0x1816d); /* 0x20742cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][0], 0x3be9e); /* 0x2074300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][1], 0x3a28f); /* 0x2074304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][2], 0x29e2); /* 0x2074308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][3], 0x8c36); /* 0x207430c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][4], 0x11cd6); /* 0x2074310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][5], 0x2c4e); /* 0x2074314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][6], 0x1e08a); /* 0x2074318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][7], 0x39890); /* 0x207431c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][8], 0x190b6); /* 0x2074320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][9], 0x22898); /* 0x2074324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][10], 0x4806); /* 0x2074328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][11], 0xab02); /* 0x207432c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][12], 0x374bd); /* 0x2074330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][13], 0xe683); /* 0x2074334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][14], 0x9be7); /* 0x2074338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][15], 0x20051); /* 0x207433c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][16], 0x23cab); /* 0x2074340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][17], 0x1584d); /* 0x2074344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][18], 0x23a5a); /* 0x2074348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][19], 0x3870); /* 0x207434c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][20], 0x3e2e7); /* 0x2074350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][21], 0x21625); /* 0x2074354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][22], 0x34ad4); /* 0x2074358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][23], 0x7ef5); /* 0x207435c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][24], 0x2994a); /* 0x2074360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][25], 0x33e0b); /* 0x2074364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][26], 0x1432); /* 0x2074368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][27], 0x2d030); /* 0x207436c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][28], 0x872a); /* 0x2074370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][29], 0x348b2); /* 0x2074374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][30], 0x3efbd); /* 0x2074378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][31], 0x376cf); /* 0x207437c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][32], 0x3b881); /* 0x2074380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][33], 0xa525); /* 0x2074384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][34], 0x1ee41); /* 0x2074388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][35], 0x2944f); /* 0x207438c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][36], 0x1f29a); /* 0x2074390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][37], 0xe04b); /* 0x2074394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][38], 0x9e47); /* 0x2074398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][39], 0x34afc); /* 0x207439c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][40], 0x3d1f0); /* 0x20743a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][41], 0x29abd); /* 0x20743a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][42], 0x2975c); /* 0x20743a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][43], 0xa78a); /* 0x20743ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][44], 0x2a292); /* 0x20743b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][45], 0x1e1c3); /* 0x20743b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][46], 0x172dd); /* 0x20743b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][47], 0x1736); /* 0x20743bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][48], 0x3219a); /* 0x20743c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][49], 0x50b5); /* 0x20743c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][50], 0x37511); /* 0x20743c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][51], 0x36237); /* 0x20743cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][0], 0x226b7); /* 0x2074400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][1], 0xd91a); /* 0x2074404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][2], 0x271d8); /* 0x2074408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][3], 0x8713); /* 0x207440c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][4], 0x3ac79); /* 0x2074410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][5], 0x386b3); /* 0x2074414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][6], 0x364a3); /* 0x2074418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][7], 0x3bbf); /* 0x207441c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][8], 0x35124); /* 0x2074420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][9], 0x16064); /* 0x2074424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][10], 0xcc9d); /* 0x2074428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][11], 0x1903d); /* 0x207442c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][12], 0x1c03); /* 0x2074430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][13], 0x2a934); /* 0x2074434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][14], 0x28c03); /* 0x2074438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][15], 0x2e9e6); /* 0x207443c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][16], 0x17f0e); /* 0x2074440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][17], 0x2e6d1); /* 0x2074444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][18], 0x33c3); /* 0x2074448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][19], 0x705e); /* 0x207444c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][20], 0x146d7); /* 0x2074450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][21], 0x1cc18); /* 0x2074454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][22], 0x1e884); /* 0x2074458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][23], 0x16cc1); /* 0x207445c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][24], 0x1499b); /* 0x2074460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][25], 0x128be); /* 0x2074464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][26], 0x16bf); /* 0x2074468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][27], 0x46d); /* 0x207446c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][28], 0x7f2c); /* 0x2074470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][29], 0x3d6d); /* 0x2074474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][30], 0x308a3); /* 0x2074478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][31], 0x2ccba); /* 0x207447c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][32], 0x37d4f); /* 0x2074480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][33], 0x1415b); /* 0x2074484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][34], 0x34e5c); /* 0x2074488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][35], 0x340e5); /* 0x207448c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][36], 0x1b33b); /* 0x2074490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][37], 0x33783); /* 0x2074494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][38], 0x2e1f5); /* 0x2074498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][39], 0x33cc); /* 0x207449c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][40], 0xb8fa); /* 0x20744a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][41], 0x3aefc); /* 0x20744a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][42], 0xe79); /* 0x20744a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][43], 0x7c02); /* 0x20744ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][44], 0x23b64); /* 0x20744b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][45], 0x68ce); /* 0x20744b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][46], 0x3f44); /* 0x20744b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][47], 0x34f89); /* 0x20744bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][48], 0x1d278); /* 0x20744c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][49], 0x31b30); /* 0x20744c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][50], 0x25167); /* 0x20744c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][51], 0x73e1); /* 0x20744cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][0], 0x2b04f); /* 0x2074500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][1], 0x37d43); /* 0x2074504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][2], 0x6a4); /* 0x2074508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][3], 0x985b); /* 0x207450c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][4], 0x3f936); /* 0x2074510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][5], 0x1abf8); /* 0x2074514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][6], 0x2ee64); /* 0x2074518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][7], 0x2242d); /* 0x207451c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][8], 0x311a4); /* 0x2074520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][9], 0xd25e); /* 0x2074524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][10], 0x776d); /* 0x2074528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][11], 0x23dac); /* 0x207452c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][12], 0x11fcb); /* 0x2074530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][13], 0x11f92); /* 0x2074534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][14], 0x3b8c8); /* 0x2074538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][15], 0x2b7da); /* 0x207453c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][16], 0x3d986); /* 0x2074540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][17], 0x33a31); /* 0x2074544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][18], 0x1683b); /* 0x2074548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][19], 0x1e4f5); /* 0x207454c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][20], 0x3fca6); /* 0x2074550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][21], 0xcf7c); /* 0x2074554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][22], 0x214af); /* 0x2074558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][23], 0x3efc6); /* 0x207455c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][24], 0x79f3); /* 0x2074560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][25], 0x200bd); /* 0x2074564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][26], 0x29a9f); /* 0x2074568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][27], 0x1b4d2); /* 0x207456c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][28], 0x3534c); /* 0x2074570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][29], 0xc922); /* 0x2074574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][30], 0x893c); /* 0x2074578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][31], 0xc1fd); /* 0x207457c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][32], 0x23888); /* 0x2074580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][33], 0x8ca8); /* 0x2074584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][34], 0x27094); /* 0x2074588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][35], 0x24dd7); /* 0x207458c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][36], 0xac5c); /* 0x2074590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][37], 0x1bc0d); /* 0x2074594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][38], 0x13460); /* 0x2074598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][39], 0x301b7); /* 0x207459c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][40], 0x33dda); /* 0x20745a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][41], 0x30c96); /* 0x20745a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][42], 0x25452); /* 0x20745a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][43], 0x13249); /* 0x20745ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][44], 0x2617); /* 0x20745b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][45], 0x2c437); /* 0x20745b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][46], 0x236f9); /* 0x20745b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][47], 0x108ec); /* 0x20745bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][48], 0x3c263); /* 0x20745c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][49], 0x15ffa); /* 0x20745c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][50], 0xcafa); /* 0x20745c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][51], 0x191bf); /* 0x20745cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][0], 0x3cc5e); /* 0x2074600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][1], 0x17eab); /* 0x2074604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][2], 0x10204); /* 0x2074608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][3], 0x3946d); /* 0x207460c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][4], 0x12a9b); /* 0x2074610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][5], 0x1f101); /* 0x2074614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][6], 0x1fe27); /* 0x2074618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][7], 0x399c0); /* 0x207461c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][8], 0x19243); /* 0x2074620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][9], 0x131); /* 0x2074624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][10], 0x1b048); /* 0x2074628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][11], 0x3e369); /* 0x207462c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][12], 0x2ba4); /* 0x2074630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][13], 0x1cf6c); /* 0x2074634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][14], 0x35eab); /* 0x2074638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][15], 0x123de); /* 0x207463c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][16], 0x15c50); /* 0x2074640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][17], 0xc786); /* 0x2074644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][18], 0x2e0a3); /* 0x2074648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][19], 0xba2d); /* 0x207464c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][20], 0x37834); /* 0x2074650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][21], 0x15659); /* 0x2074654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][22], 0x6385); /* 0x2074658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][23], 0x347a2); /* 0x207465c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][24], 0x2e4f0); /* 0x2074660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][25], 0xe55a); /* 0x2074664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][26], 0x3e95d); /* 0x2074668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][27], 0x238); /* 0x207466c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][28], 0x1d70e); /* 0x2074670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][29], 0x348d); /* 0x2074674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][30], 0x17e95); /* 0x2074678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][31], 0x3f8a5); /* 0x207467c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][32], 0x2cf79); /* 0x2074680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][33], 0x2baca); /* 0x2074684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][34], 0x3b48c); /* 0x2074688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][35], 0x16c21); /* 0x207468c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][36], 0x2376f); /* 0x2074690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][37], 0x28cd0); /* 0x2074694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][38], 0xcd73); /* 0x2074698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][39], 0x26103); /* 0x207469c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][40], 0x1829); /* 0x20746a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][41], 0x6d82); /* 0x20746a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][42], 0xc83c); /* 0x20746a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][43], 0x36e06); /* 0x20746ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][44], 0x3d80f); /* 0x20746b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][45], 0x2e577); /* 0x20746b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][46], 0x18ddb); /* 0x20746b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][47], 0x3aff2); /* 0x20746bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][48], 0x194d); /* 0x20746c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][49], 0x2c35d); /* 0x20746c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][50], 0x94cc); /* 0x20746c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][51], 0x2fee8); /* 0x20746cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][0], 0x39de3); /* 0x2074700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][1], 0x1aa93); /* 0x2074704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][2], 0x13e30); /* 0x2074708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][3], 0xff09); /* 0x207470c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][4], 0x1db98); /* 0x2074710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][5], 0x3a451); /* 0x2074714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][6], 0x3d05d); /* 0x2074718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][7], 0x340df); /* 0x207471c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][8], 0x794); /* 0x2074720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][9], 0x31aac); /* 0x2074724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][10], 0x3336c); /* 0x2074728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][11], 0x39608); /* 0x207472c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][12], 0x3c119); /* 0x2074730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][13], 0x197d6); /* 0x2074734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][14], 0x2a4cc); /* 0x2074738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][15], 0x1e7cf); /* 0x207473c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][16], 0x2889e); /* 0x2074740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][17], 0x2e11); /* 0x2074744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][18], 0x39f0b); /* 0x2074748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][19], 0x31c50); /* 0x207474c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][20], 0xb2e9); /* 0x2074750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][21], 0x1126); /* 0x2074754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][22], 0x1c5c1); /* 0x2074758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][23], 0x32006); /* 0x207475c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][24], 0x14e7a); /* 0x2074760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][25], 0x900a); /* 0x2074764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][26], 0x2291e); /* 0x2074768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][27], 0x2717c); /* 0x207476c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][28], 0x2ab62); /* 0x2074770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][29], 0x2444b); /* 0x2074774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][30], 0x1abcf); /* 0x2074778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][31], 0x3728a); /* 0x207477c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][32], 0x7204); /* 0x2074780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][33], 0xec94); /* 0x2074784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][34], 0x1df00); /* 0x2074788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][35], 0xf427); /* 0x207478c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][36], 0x19e59); /* 0x2074790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][37], 0x3c31f); /* 0x2074794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][38], 0x1dd78); /* 0x2074798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][39], 0x18bee); /* 0x207479c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][40], 0xa8f); /* 0x20747a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][41], 0x23491); /* 0x20747a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][42], 0xbd44); /* 0x20747a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][43], 0x2324c); /* 0x20747ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][44], 0x2f655); /* 0x20747b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][45], 0x662a); /* 0x20747b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][46], 0x38077); /* 0x20747b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][47], 0xc1e9); /* 0x20747bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][48], 0x316d2); /* 0x20747c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][49], 0x191cc); /* 0x20747c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][50], 0x3dfec); /* 0x20747c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][51], 0x310c7); /* 0x20747cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][0], 0x5753); /* 0x2074800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][1], 0x30e4); /* 0x2074804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][2], 0x39e7b); /* 0x2074808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][3], 0x3fc36); /* 0x207480c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][4], 0x266de); /* 0x2074810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][5], 0x16309); /* 0x2074814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][6], 0x35d32); /* 0x2074818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][7], 0x27799); /* 0x207481c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][8], 0x287fe); /* 0x2074820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][9], 0x13cbc); /* 0x2074824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][10], 0xf7eb); /* 0x2074828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][11], 0x30cc7); /* 0x207482c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][12], 0x3f303); /* 0x2074830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][13], 0x24f08); /* 0x2074834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][14], 0x18a70); /* 0x2074838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][15], 0x379c4); /* 0x207483c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][16], 0x25f0a); /* 0x2074840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][17], 0x18ffc); /* 0x2074844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][18], 0x292d6); /* 0x2074848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][19], 0xc44a); /* 0x207484c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][20], 0x1eef1); /* 0x2074850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][21], 0x36598); /* 0x2074854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][22], 0x17575); /* 0x2074858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][23], 0x23a21); /* 0x207485c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][24], 0xd8ec); /* 0x2074860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][25], 0x1272); /* 0x2074864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][26], 0x1cada); /* 0x2074868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][27], 0x21c54); /* 0x207486c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][28], 0x2c9cb); /* 0x2074870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][29], 0x1dca1); /* 0x2074874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][30], 0x204b1); /* 0x2074878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][31], 0x20c7e); /* 0x207487c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][32], 0x3ceba); /* 0x2074880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][33], 0xf610); /* 0x2074884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][34], 0x344d9); /* 0x2074888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][35], 0x3ca34); /* 0x207488c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][36], 0xe7da); /* 0x2074890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][37], 0x166d2); /* 0x2074894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][38], 0x134ac); /* 0x2074898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][39], 0xd88e); /* 0x207489c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][40], 0x3a47e); /* 0x20748a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][41], 0x1a300); /* 0x20748a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][42], 0xed28); /* 0x20748a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][43], 0x1a2c4); /* 0x20748ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][44], 0x331c9); /* 0x20748b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][45], 0x395a3); /* 0x20748b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][46], 0x35a69); /* 0x20748b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][47], 0x1bf4d); /* 0x20748bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][48], 0x2a17e); /* 0x20748c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][49], 0x3a6da); /* 0x20748c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][50], 0x27d9a); /* 0x20748c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][51], 0x18d13); /* 0x20748cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][0], 0xddc7); /* 0x2074900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][1], 0xc729); /* 0x2074904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][2], 0x3c47a); /* 0x2074908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][3], 0x3d5ec); /* 0x207490c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][4], 0x7e5e); /* 0x2074910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][5], 0x3806c); /* 0x2074914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][6], 0xbbf8); /* 0x2074918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][7], 0xd206); /* 0x207491c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][8], 0x29f49); /* 0x2074920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][9], 0x6190); /* 0x2074924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][10], 0x289ae); /* 0x2074928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][11], 0x23f1c); /* 0x207492c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][12], 0x23491); /* 0x2074930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][13], 0x161c6); /* 0x2074934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][14], 0x7642); /* 0x2074938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][15], 0x1f64); /* 0x207493c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][16], 0xea2c); /* 0x2074940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][17], 0x49d9); /* 0x2074944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][18], 0x18c4d); /* 0x2074948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][19], 0x3854f); /* 0x207494c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][20], 0x21668); /* 0x2074950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][21], 0x1613); /* 0x2074954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][22], 0xf2a5); /* 0x2074958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][23], 0x109b3); /* 0x207495c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][24], 0x8e49); /* 0x2074960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][25], 0x3ad6f); /* 0x2074964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][26], 0x3ca14); /* 0x2074968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][27], 0x3e67c); /* 0x207496c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][28], 0x22913); /* 0x2074970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][29], 0x37ddc); /* 0x2074974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][30], 0x1c62); /* 0x2074978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][31], 0x241c); /* 0x207497c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][32], 0x291fc); /* 0x2074980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][33], 0x86a4); /* 0x2074984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][34], 0x29bad); /* 0x2074988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][35], 0x3a7c3); /* 0x207498c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][36], 0x1616e); /* 0x2074990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][37], 0x20987); /* 0x2074994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][38], 0xb855); /* 0x2074998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][39], 0x182ed); /* 0x207499c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][40], 0x1ad2a); /* 0x20749a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][41], 0x2b1c9); /* 0x20749a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][42], 0x34ce5); /* 0x20749a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][43], 0x72d3); /* 0x20749ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][44], 0x3e3a3); /* 0x20749b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][45], 0x12012); /* 0x20749b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][46], 0x1823c); /* 0x20749b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][47], 0x30005); /* 0x20749bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][48], 0x27957); /* 0x20749c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][49], 0x2f265); /* 0x20749c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][50], 0x2ce5f); /* 0x20749c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][51], 0x1de5c); /* 0x20749cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][0], 0x383dc); /* 0x2074a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][1], 0x975); /* 0x2074a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][2], 0x164f3); /* 0x2074a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][3], 0xe81); /* 0x2074a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][4], 0x375bd); /* 0x2074a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][5], 0x21ee3); /* 0x2074a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][6], 0x3b308); /* 0x2074a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][7], 0x2115); /* 0x2074a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][8], 0x2fb43); /* 0x2074a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][9], 0x39db6); /* 0x2074a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][10], 0x1d59e); /* 0x2074a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][11], 0x263d2); /* 0x2074a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][12], 0x21be4); /* 0x2074a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][13], 0x19c04); /* 0x2074a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][14], 0x264e1); /* 0x2074a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][15], 0x18360); /* 0x2074a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][16], 0x20e12); /* 0x2074a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][17], 0x24171); /* 0x2074a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][18], 0x14dbc); /* 0x2074a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][19], 0x2c5fa); /* 0x2074a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][20], 0x34651); /* 0x2074a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][21], 0x26bd7); /* 0x2074a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][22], 0x15b36); /* 0x2074a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][23], 0xa38c); /* 0x2074a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][24], 0x16b9d); /* 0x2074a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][25], 0x203a5); /* 0x2074a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][26], 0x1048f); /* 0x2074a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][27], 0x3f0ca); /* 0x2074a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][28], 0x182b8); /* 0x2074a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][29], 0x18ff7); /* 0x2074a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][30], 0x2dcbc); /* 0x2074a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][31], 0x16940); /* 0x2074a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][32], 0x48fb); /* 0x2074a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][33], 0x28095); /* 0x2074a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][34], 0x7f97); /* 0x2074a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][35], 0x1bcde); /* 0x2074a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][36], 0x100d9); /* 0x2074a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][37], 0x2f33a); /* 0x2074a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][38], 0x3e13); /* 0x2074a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][39], 0x34b89); /* 0x2074a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][40], 0x39008); /* 0x2074aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][41], 0x35792); /* 0x2074aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][42], 0x11381); /* 0x2074aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][43], 0x7771); /* 0x2074aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][44], 0x28e38); /* 0x2074ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][45], 0x24791); /* 0x2074ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][46], 0x2a7bd); /* 0x2074ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][47], 0x9557); /* 0x2074abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][48], 0xabb1); /* 0x2074ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][49], 0x53dd); /* 0x2074ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][50], 0x2037); /* 0x2074ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][51], 0x3104b); /* 0x2074acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][0], 0x30dc5); /* 0x2074b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][1], 0x18815); /* 0x2074b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][2], 0x18e2e); /* 0x2074b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][3], 0x10a17); /* 0x2074b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][4], 0x3cc6a); /* 0x2074b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][5], 0x31251); /* 0x2074b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][6], 0x2d827); /* 0x2074b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][7], 0x7c72); /* 0x2074b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][8], 0xf884); /* 0x2074b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][9], 0x30d90); /* 0x2074b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][10], 0x25d2e); /* 0x2074b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][11], 0x3bc52); /* 0x2074b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][12], 0x3d523); /* 0x2074b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][13], 0x33a2); /* 0x2074b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][14], 0x542d); /* 0x2074b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][15], 0x3c838); /* 0x2074b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][16], 0x9444); /* 0x2074b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][17], 0x21cd5); /* 0x2074b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][18], 0x99c5); /* 0x2074b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][19], 0xd48e); /* 0x2074b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][20], 0x1e79); /* 0x2074b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][21], 0x3bee9); /* 0x2074b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][22], 0x2356b); /* 0x2074b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][23], 0x16104); /* 0x2074b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][24], 0x4b73); /* 0x2074b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][25], 0x215c); /* 0x2074b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][26], 0x14d9b); /* 0x2074b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][27], 0x18e16); /* 0x2074b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][28], 0x2e253); /* 0x2074b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][29], 0x1bf12); /* 0x2074b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][30], 0x2c403); /* 0x2074b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][31], 0x3ccbc); /* 0x2074b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][32], 0x1ffdf); /* 0x2074b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][33], 0x7c12); /* 0x2074b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][34], 0xfab3); /* 0x2074b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][35], 0x1da15); /* 0x2074b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][36], 0x3823e); /* 0x2074b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][37], 0x1bcdb); /* 0x2074b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][38], 0x6459); /* 0x2074b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][39], 0x1914e); /* 0x2074b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][40], 0x5382); /* 0x2074ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][41], 0x1f1be); /* 0x2074ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][42], 0x3938f); /* 0x2074ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][43], 0x1e7fc); /* 0x2074bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][44], 0x3757a); /* 0x2074bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][45], 0x23125); /* 0x2074bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][46], 0x34095); /* 0x2074bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][47], 0x38c7b); /* 0x2074bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][48], 0x22d60); /* 0x2074bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][49], 0x1b4cd); /* 0x2074bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][50], 0x2462b); /* 0x2074bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][51], 0x36e32); /* 0x2074bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][0], 0x10665); /* 0x2074c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][1], 0x1b457); /* 0x2074c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][2], 0x1bb02); /* 0x2074c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][3], 0x2117b); /* 0x2074c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][4], 0x914); /* 0x2074c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][5], 0x163a0); /* 0x2074c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][6], 0x302ba); /* 0x2074c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][7], 0x165e1); /* 0x2074c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][8], 0x1489d); /* 0x2074c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][9], 0xcb74); /* 0x2074c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][10], 0x2ec21); /* 0x2074c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][11], 0x8eb6); /* 0x2074c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][12], 0x1a8c1); /* 0x2074c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][13], 0x21aa8); /* 0x2074c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][14], 0x17e21); /* 0x2074c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][15], 0x2d8d1); /* 0x2074c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][16], 0xf03c); /* 0x2074c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][17], 0x31a99); /* 0x2074c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][18], 0xfefe); /* 0x2074c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][19], 0x2cd43); /* 0x2074c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][20], 0x19d1d); /* 0x2074c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][21], 0x33af); /* 0x2074c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][22], 0x3201e); /* 0x2074c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][23], 0x2306e); /* 0x2074c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][24], 0x207d2); /* 0x2074c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][25], 0x3985f); /* 0x2074c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][26], 0x1e777); /* 0x2074c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][27], 0xba77); /* 0x2074c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][28], 0x198b9); /* 0x2074c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][29], 0x2173c); /* 0x2074c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][30], 0x54db); /* 0x2074c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][31], 0x29c1); /* 0x2074c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][32], 0x1f2da); /* 0x2074c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][33], 0x36340); /* 0x2074c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][34], 0x3ab60); /* 0x2074c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][35], 0x30154); /* 0x2074c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][36], 0x1799f); /* 0x2074c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][37], 0x175bf); /* 0x2074c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][38], 0x1732d); /* 0x2074c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][39], 0x3449); /* 0x2074c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][40], 0x3b164); /* 0x2074ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][41], 0x39bf); /* 0x2074ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][42], 0x1e521); /* 0x2074ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][43], 0x11ea3); /* 0x2074cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][44], 0x37eba); /* 0x2074cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][45], 0x280ba); /* 0x2074cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][46], 0x2938f); /* 0x2074cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][47], 0x275b5); /* 0x2074cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][48], 0x3e344); /* 0x2074cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][49], 0x26f6b); /* 0x2074cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][50], 0x1b7f7); /* 0x2074cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][51], 0x3a930); /* 0x2074ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][0], 0x23c06); /* 0x2074d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][1], 0x1000a); /* 0x2074d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][2], 0x3fedf); /* 0x2074d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][3], 0x21d7e); /* 0x2074d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][4], 0x36923); /* 0x2074d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][5], 0x2038f); /* 0x2074d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][6], 0xcbae); /* 0x2074d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][7], 0x3ff9e); /* 0x2074d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][8], 0x37e5f); /* 0x2074d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][9], 0x2fb1c); /* 0x2074d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][10], 0x210b2); /* 0x2074d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][11], 0x13f56); /* 0x2074d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][12], 0x2f4c2); /* 0x2074d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][13], 0x13a61); /* 0x2074d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][14], 0x16f90); /* 0x2074d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][15], 0x18abf); /* 0x2074d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][16], 0x3d2b1); /* 0x2074d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][17], 0x1abbf); /* 0x2074d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][18], 0x1e191); /* 0x2074d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][19], 0x61f); /* 0x2074d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][20], 0xbf7e); /* 0x2074d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][21], 0x33ee4); /* 0x2074d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][22], 0x2ff8b); /* 0x2074d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][23], 0x3d92a); /* 0x2074d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][24], 0x3ac60); /* 0x2074d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][25], 0x394d4); /* 0x2074d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][26], 0x144cc); /* 0x2074d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][27], 0x265e2); /* 0x2074d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][28], 0x2644); /* 0x2074d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][29], 0x23618); /* 0x2074d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][30], 0x19b6); /* 0x2074d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][31], 0x135ce); /* 0x2074d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][32], 0x3b298); /* 0x2074d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][33], 0x3acd7); /* 0x2074d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][34], 0x230af); /* 0x2074d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][35], 0x207e9); /* 0x2074d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][36], 0x2f590); /* 0x2074d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][37], 0x11aac); /* 0x2074d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][38], 0x30dbd); /* 0x2074d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][39], 0x1d57a); /* 0x2074d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][40], 0x3f0a5); /* 0x2074da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][41], 0xdd1); /* 0x2074da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][42], 0x36333); /* 0x2074da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][43], 0x2d70e); /* 0x2074dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][44], 0x3e8b9); /* 0x2074db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][45], 0x1edb8); /* 0x2074db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][46], 0x30cc1); /* 0x2074db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][47], 0x45c6); /* 0x2074dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][48], 0x39b4a); /* 0x2074dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][49], 0x25f51); /* 0x2074dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][50], 0xa6a4); /* 0x2074dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][51], 0x3247a); /* 0x2074dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][0], 0x1a7a3); /* 0x2074e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][1], 0x6f8f); /* 0x2074e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][2], 0x2a5d); /* 0x2074e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][3], 0x27b71); /* 0x2074e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][4], 0x1ab02); /* 0x2074e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][5], 0x29d70); /* 0x2074e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][6], 0x34b04); /* 0x2074e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][7], 0x30662); /* 0x2074e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][8], 0x10302); /* 0x2074e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][9], 0x2b29a); /* 0x2074e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][10], 0xd914); /* 0x2074e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][11], 0x25f29); /* 0x2074e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][12], 0x7a0b); /* 0x2074e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][13], 0x190b2); /* 0x2074e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][14], 0xd508); /* 0x2074e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][15], 0xac59); /* 0x2074e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][16], 0x3e936); /* 0x2074e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][17], 0x2fb3b); /* 0x2074e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][18], 0x27fe7); /* 0x2074e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][19], 0x21453); /* 0x2074e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][20], 0x16c56); /* 0x2074e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][21], 0x16e8); /* 0x2074e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][22], 0x2112c); /* 0x2074e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][23], 0x1854e); /* 0x2074e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][24], 0x26b1); /* 0x2074e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][25], 0x39da0); /* 0x2074e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][26], 0x384f7); /* 0x2074e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][27], 0x2b2be); /* 0x2074e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][28], 0x2ba27); /* 0x2074e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][29], 0x218dc); /* 0x2074e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][30], 0x19fcd); /* 0x2074e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][31], 0x35d88); /* 0x2074e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][32], 0x38b0d); /* 0x2074e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][33], 0x34ef7); /* 0x2074e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][34], 0x313ff); /* 0x2074e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][35], 0x1c6e1); /* 0x2074e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][36], 0x149d); /* 0x2074e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][37], 0x18ed6); /* 0x2074e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][38], 0x222af); /* 0x2074e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][39], 0xa7ce); /* 0x2074e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][40], 0x16604); /* 0x2074ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][41], 0x2b5db); /* 0x2074ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][42], 0x35820); /* 0x2074ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][43], 0x1eb5d); /* 0x2074eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][44], 0x52da); /* 0x2074eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][45], 0x16180); /* 0x2074eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][46], 0x82a3); /* 0x2074eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][47], 0x20b60); /* 0x2074ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][48], 0x37a06); /* 0x2074ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][49], 0x13002); /* 0x2074ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][50], 0x5236); /* 0x2074ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][51], 0x299ea); /* 0x2074ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][0], 0x31d16); /* 0x2074f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][1], 0x223f6); /* 0x2074f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][2], 0x3d72a); /* 0x2074f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][3], 0x2d651); /* 0x2074f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][4], 0x17ca1); /* 0x2074f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][5], 0x3e0b3); /* 0x2074f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][6], 0x12b6a); /* 0x2074f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][7], 0x25e34); /* 0x2074f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][8], 0x13fc0); /* 0x2074f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][9], 0x379ed); /* 0x2074f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][10], 0x28ed1); /* 0x2074f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][11], 0x3b740); /* 0x2074f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][12], 0x32962); /* 0x2074f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][13], 0x29afa); /* 0x2074f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][14], 0x13360); /* 0x2074f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][15], 0x22d37); /* 0x2074f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][16], 0x12f9d); /* 0x2074f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][17], 0x152a3); /* 0x2074f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][18], 0x2de93); /* 0x2074f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][19], 0x3ce96); /* 0x2074f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][20], 0x2641d); /* 0x2074f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][21], 0x1b46d); /* 0x2074f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][22], 0x3ca3); /* 0x2074f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][23], 0x368e0); /* 0x2074f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][24], 0x331f0); /* 0x2074f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][25], 0x6597); /* 0x2074f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][26], 0xbd5c); /* 0x2074f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][27], 0x12de4); /* 0x2074f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][28], 0x34600); /* 0x2074f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][29], 0x2212b); /* 0x2074f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][30], 0x17a68); /* 0x2074f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][31], 0x1dd43); /* 0x2074f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][32], 0x318a2); /* 0x2074f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][33], 0x12a35); /* 0x2074f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][34], 0x1c706); /* 0x2074f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][35], 0x3783a); /* 0x2074f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][36], 0x1fd90); /* 0x2074f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][37], 0x34db5); /* 0x2074f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][38], 0x1760d); /* 0x2074f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][39], 0x2a440); /* 0x2074f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][40], 0x34725); /* 0x2074fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][41], 0x2d6f4); /* 0x2074fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][42], 0x3f29d); /* 0x2074fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][43], 0x3201c); /* 0x2074fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][44], 0x10401); /* 0x2074fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][45], 0x19620); /* 0x2074fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][46], 0x14a86); /* 0x2074fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][47], 0x194cd); /* 0x2074fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][48], 0x35699); /* 0x2074fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][49], 0x34fc); /* 0x2074fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][50], 0x39e25); /* 0x2074fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][51], 0x2f856); /* 0x2074fcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][0], 0x223ac); /* 0x2075000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][1], 0x320f4); /* 0x2075004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][2], 0x27697); /* 0x2075008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][3], 0x32f77); /* 0x207500c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][4], 0x20329); /* 0x2075010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][5], 0x6a5f); /* 0x2075014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][6], 0x242b2); /* 0x2075018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][7], 0x2b19c); /* 0x207501c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][8], 0x1239a); /* 0x2075020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][9], 0x1f346); /* 0x2075024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][10], 0x1425f); /* 0x2075028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][11], 0x173d0); /* 0x207502c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][12], 0x298b4); /* 0x2075030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][13], 0x35155); /* 0x2075034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][14], 0x3023); /* 0x2075038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][15], 0x3f7b7); /* 0x207503c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][16], 0x14097); /* 0x2075040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][17], 0x14aad); /* 0x2075044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][18], 0x31cd9); /* 0x2075048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][19], 0x3ca6b); /* 0x207504c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][20], 0x187f9); /* 0x2075050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][21], 0x282d6); /* 0x2075054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][22], 0x27358); /* 0x2075058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][23], 0x1fc42); /* 0x207505c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][24], 0x38ddb); /* 0x2075060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][25], 0x99e3); /* 0x2075064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][26], 0x23292); /* 0x2075068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][27], 0x11c76); /* 0x207506c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][28], 0x1cfb8); /* 0x2075070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][29], 0x3cff0); /* 0x2075074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][30], 0x8a59); /* 0x2075078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][31], 0xac7c); /* 0x207507c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][32], 0x30ff3); /* 0x2075080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][33], 0x38e0); /* 0x2075084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][34], 0x2195); /* 0x2075088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][35], 0x19d98); /* 0x207508c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][36], 0x208d4); /* 0x2075090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][37], 0x7540); /* 0x2075094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][38], 0x28fb); /* 0x2075098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][39], 0x3fc53); /* 0x207509c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][40], 0x22e04); /* 0x20750a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][41], 0x39d2a); /* 0x20750a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][42], 0x1fad); /* 0x20750a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][43], 0x2547); /* 0x20750ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][44], 0x199d4); /* 0x20750b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][45], 0x3ef5a); /* 0x20750b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][46], 0x1512); /* 0x20750b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][47], 0x338c1); /* 0x20750bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][48], 0x6c69); /* 0x20750c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][49], 0xebc); /* 0x20750c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][50], 0x6602); /* 0x20750c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][51], 0x344f); /* 0x20750cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][0], 0x37f86); /* 0x2075100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][1], 0x3ef33); /* 0x2075104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][2], 0x1055f); /* 0x2075108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][3], 0x3e4ea); /* 0x207510c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][4], 0x1d2f2); /* 0x2075110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][5], 0xb705); /* 0x2075114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][6], 0x27bf8); /* 0x2075118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][7], 0x2c28b); /* 0x207511c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][8], 0x15ee2); /* 0x2075120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][9], 0xa1b6); /* 0x2075124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][10], 0x2f11d); /* 0x2075128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][11], 0xa900); /* 0x207512c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][12], 0x1338d); /* 0x2075130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][13], 0x14a50); /* 0x2075134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][14], 0x32283); /* 0x2075138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][15], 0x32202); /* 0x207513c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][16], 0x3e879); /* 0x2075140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][17], 0x162e7); /* 0x2075144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][18], 0x4238); /* 0x2075148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][19], 0x3b159); /* 0x207514c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][20], 0x1b20a); /* 0x2075150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][21], 0x32c9d); /* 0x2075154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][22], 0x6147); /* 0x2075158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][23], 0x27026); /* 0x207515c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][24], 0x10eb0); /* 0x2075160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][25], 0x16a45); /* 0x2075164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][26], 0x37079); /* 0x2075168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][27], 0x2dee8); /* 0x207516c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][28], 0x14aad); /* 0x2075170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][29], 0x116bd); /* 0x2075174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][30], 0x2e03); /* 0x2075178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][31], 0x1813); /* 0x207517c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][32], 0x9aad); /* 0x2075180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][33], 0x2800a); /* 0x2075184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][34], 0x3a4d4); /* 0x2075188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][35], 0x9273); /* 0x207518c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][36], 0x2fdcb); /* 0x2075190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][37], 0x1e121); /* 0x2075194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][38], 0x3c09f); /* 0x2075198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][39], 0x3f423); /* 0x207519c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][40], 0x10093); /* 0x20751a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][41], 0x2dd32); /* 0x20751a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][42], 0x7391); /* 0x20751a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][43], 0x160ba); /* 0x20751ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][44], 0xbf39); /* 0x20751b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][45], 0x3790b); /* 0x20751b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][46], 0x3403f); /* 0x20751b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][47], 0x26475); /* 0x20751bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][48], 0xb6e); /* 0x20751c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][49], 0x372f1); /* 0x20751c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][50], 0x2c192); /* 0x20751c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][51], 0x37a27); /* 0x20751cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][0], 0x14935); /* 0x2075200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][1], 0x14df); /* 0x2075204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][2], 0x3011a); /* 0x2075208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][3], 0x20181); /* 0x207520c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][4], 0x13489); /* 0x2075210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][5], 0x59d); /* 0x2075214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][6], 0x23d96); /* 0x2075218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][7], 0x2d2ff); /* 0x207521c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][8], 0x9d5e); /* 0x2075220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][9], 0x28b31); /* 0x2075224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][10], 0x2c61a); /* 0x2075228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][11], 0x14e81); /* 0x207522c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][12], 0x23ccb); /* 0x2075230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][13], 0x3c3c9); /* 0x2075234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][14], 0x3510); /* 0x2075238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][15], 0x30f1c); /* 0x207523c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][16], 0xd868); /* 0x2075240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][17], 0x2c79d); /* 0x2075244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][18], 0x34bb0); /* 0x2075248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][19], 0x2b983); /* 0x207524c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][20], 0x36207); /* 0x2075250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][21], 0xcc4b); /* 0x2075254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][22], 0x65bc); /* 0x2075258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][23], 0x21155); /* 0x207525c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][24], 0x1171c); /* 0x2075260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][25], 0x39a49); /* 0x2075264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][26], 0x4699); /* 0x2075268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][27], 0x2454c); /* 0x207526c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][28], 0x17e4c); /* 0x2075270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][29], 0x311f2); /* 0x2075274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][30], 0x12c8d); /* 0x2075278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][31], 0x12300); /* 0x207527c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][32], 0x201ec); /* 0x2075280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][33], 0x2b1cb); /* 0x2075284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][34], 0x2ec43); /* 0x2075288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][35], 0x17498); /* 0x207528c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][36], 0x25ec2); /* 0x2075290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][37], 0x13d4c); /* 0x2075294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][38], 0x21882); /* 0x2075298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][39], 0x4c3e); /* 0x207529c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][40], 0x641c); /* 0x20752a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][41], 0x3c8bf); /* 0x20752a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][42], 0xa016); /* 0x20752a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][43], 0x33c23); /* 0x20752ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][44], 0x39398); /* 0x20752b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][45], 0x3120f); /* 0x20752b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][46], 0x3a3e1); /* 0x20752b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][47], 0x23bad); /* 0x20752bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][48], 0xc471); /* 0x20752c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][49], 0xa69e); /* 0x20752c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][50], 0x9186); /* 0x20752c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][51], 0x18a37); /* 0x20752cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][0], 0x1fdd6); /* 0x2075300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][1], 0x2876a); /* 0x2075304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][2], 0x2c68b); /* 0x2075308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][3], 0x26971); /* 0x207530c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][4], 0x49f6); /* 0x2075310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][5], 0x1d416); /* 0x2075314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][6], 0x1f3df); /* 0x2075318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][7], 0x29ae3); /* 0x207531c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][8], 0x1e9b4); /* 0x2075320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][9], 0x1129c); /* 0x2075324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][10], 0xe152); /* 0x2075328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][11], 0x2f379); /* 0x207532c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][12], 0x33a54); /* 0x2075330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][13], 0x1d763); /* 0x2075334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][14], 0x3bc88); /* 0x2075338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][15], 0x1c3b3); /* 0x207533c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][16], 0x3eb79); /* 0x2075340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][17], 0xe513); /* 0x2075344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][18], 0x3cc8f); /* 0x2075348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][19], 0x1e937); /* 0x207534c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][20], 0x1d178); /* 0x2075350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][21], 0x2e263); /* 0x2075354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][22], 0x29712); /* 0x2075358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][23], 0x3c441); /* 0x207535c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][24], 0x175e7); /* 0x2075360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][25], 0x3376f); /* 0x2075364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][26], 0x1e269); /* 0x2075368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][27], 0x16f8a); /* 0x207536c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][28], 0x30dd5); /* 0x2075370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][29], 0x3624b); /* 0x2075374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][30], 0x34407); /* 0x2075378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][31], 0x203da); /* 0x207537c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][32], 0x7c2f); /* 0x2075380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][33], 0x1c95c); /* 0x2075384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][34], 0x2aed5); /* 0x2075388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][35], 0xf431); /* 0x207538c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][36], 0x1ec7e); /* 0x2075390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][37], 0x56c6); /* 0x2075394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][38], 0x2be65); /* 0x2075398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][39], 0x2f617); /* 0x207539c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][40], 0x30f7b); /* 0x20753a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][41], 0x3373b); /* 0x20753a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][42], 0x275f0); /* 0x20753a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][43], 0x33ca8); /* 0x20753ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][44], 0x34176); /* 0x20753b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][45], 0xba0); /* 0x20753b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][46], 0x33c0b); /* 0x20753b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][47], 0x25468); /* 0x20753bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][48], 0x20b4); /* 0x20753c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][49], 0x36c16); /* 0x20753c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][50], 0x1fd1b); /* 0x20753c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][51], 0x2e134); /* 0x20753cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][0], 0x2c065); /* 0x2075400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][1], 0x2d9e5); /* 0x2075404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][2], 0x324cf); /* 0x2075408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][3], 0x354c2); /* 0x207540c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][4], 0xb6f3); /* 0x2075410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][5], 0x2f069); /* 0x2075414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][6], 0x27175); /* 0x2075418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][7], 0xaef); /* 0x207541c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][8], 0x31d2e); /* 0x2075420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][9], 0x22d12); /* 0x2075424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][10], 0x30a8d); /* 0x2075428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][11], 0x3a0a4); /* 0x207542c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][12], 0x1d021); /* 0x2075430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][13], 0xbd8); /* 0x2075434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][14], 0x194); /* 0x2075438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][15], 0x2f23); /* 0x207543c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][16], 0x1ed21); /* 0x2075440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][17], 0x3e5ff); /* 0x2075444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][18], 0x38af0); /* 0x2075448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][19], 0x182a); /* 0x207544c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][20], 0x2d9ee); /* 0x2075450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][21], 0x32b09); /* 0x2075454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][22], 0x3681e); /* 0x2075458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][23], 0x13e8a); /* 0x207545c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][24], 0x343e8); /* 0x2075460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][25], 0xa56a); /* 0x2075464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][26], 0x25af8); /* 0x2075468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][27], 0x375b9); /* 0x207546c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][28], 0x23068); /* 0x2075470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][29], 0x14c72); /* 0x2075474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][30], 0x2456c); /* 0x2075478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][31], 0xd956); /* 0x207547c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][32], 0xca82); /* 0x2075480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][33], 0x34e6d); /* 0x2075484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][34], 0x22a21); /* 0x2075488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][35], 0xf47); /* 0x207548c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][36], 0x1f5cc); /* 0x2075490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][37], 0x1af6); /* 0x2075494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][38], 0x1cd66); /* 0x2075498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][39], 0x152fd); /* 0x207549c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][40], 0x3fe2d); /* 0x20754a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][41], 0x36489); /* 0x20754a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][42], 0x1181e); /* 0x20754a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][43], 0x23051); /* 0x20754ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][44], 0x3a73e); /* 0x20754b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][45], 0x32c1d); /* 0x20754b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][46], 0x1abb0); /* 0x20754b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][47], 0x11586); /* 0x20754bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][48], 0x3e03e); /* 0x20754c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][49], 0x36a4f); /* 0x20754c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][50], 0x12a4c); /* 0x20754c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][51], 0x281dc); /* 0x20754cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][0], 0xf008); /* 0x2075500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][1], 0x1640d); /* 0x2075504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][2], 0x522e); /* 0x2075508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][3], 0x16066); /* 0x207550c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][4], 0x111c7); /* 0x2075510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][5], 0x3cd83); /* 0x2075514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][6], 0x25039); /* 0x2075518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][7], 0x26797); /* 0x207551c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][8], 0x21f05); /* 0x2075520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][9], 0x24861); /* 0x2075524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][10], 0xcfe1); /* 0x2075528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][11], 0x3e90d); /* 0x207552c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][12], 0x185eb); /* 0x2075530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][13], 0x3010c); /* 0x2075534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][14], 0xf994); /* 0x2075538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][15], 0x1ddbb); /* 0x207553c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][16], 0x1ce04); /* 0x2075540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][17], 0x444f); /* 0x2075544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][18], 0x2b298); /* 0x2075548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][19], 0x13d89); /* 0x207554c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][20], 0x141d8); /* 0x2075550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][21], 0x1c756); /* 0x2075554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][22], 0x1ae64); /* 0x2075558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][23], 0x20700); /* 0x207555c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][24], 0xb52e); /* 0x2075560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][25], 0x17c95); /* 0x2075564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][26], 0x11bbb); /* 0x2075568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][27], 0x2917b); /* 0x207556c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][28], 0x3755d); /* 0x2075570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][29], 0x3a664); /* 0x2075574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][30], 0x6183); /* 0x2075578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][31], 0xfc54); /* 0x207557c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][32], 0x17cf9); /* 0x2075580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][33], 0x2b8f2); /* 0x2075584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][34], 0x1256); /* 0x2075588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][35], 0x3844); /* 0x207558c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][36], 0x2dd5d); /* 0x2075590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][37], 0x26709); /* 0x2075594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][38], 0x2b93); /* 0x2075598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][39], 0x1fd02); /* 0x207559c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][40], 0x13a3c); /* 0x20755a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][41], 0x1c1bd); /* 0x20755a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][42], 0x587e); /* 0x20755a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][43], 0x4ccf); /* 0x20755ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][44], 0xf286); /* 0x20755b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][45], 0x2c05e); /* 0x20755b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][46], 0x2140c); /* 0x20755b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][47], 0x1103e); /* 0x20755bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][48], 0x3aba6); /* 0x20755c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][49], 0x1eaaf); /* 0x20755c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][50], 0x24f2c); /* 0x20755c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][51], 0x11a02); /* 0x20755cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][0], 0x263c3); /* 0x2075600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][1], 0x3a3c0); /* 0x2075604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][2], 0x1ff3e); /* 0x2075608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][3], 0x26b76); /* 0x207560c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][4], 0x33449); /* 0x2075610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][5], 0x2f734); /* 0x2075614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][6], 0x3eb34); /* 0x2075618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][7], 0x10e49); /* 0x207561c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][8], 0x2d5ce); /* 0x2075620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][9], 0x1c943); /* 0x2075624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][10], 0x1de7b); /* 0x2075628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][11], 0x8671); /* 0x207562c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][12], 0xb4a7); /* 0x2075630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][13], 0x23ff8); /* 0x2075634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][14], 0x35d8f); /* 0x2075638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][15], 0x18625); /* 0x207563c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][16], 0xad09); /* 0x2075640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][17], 0x34c0f); /* 0x2075644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][18], 0x2224c); /* 0x2075648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][19], 0x2a144); /* 0x207564c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][20], 0x3cbc0); /* 0x2075650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][21], 0x2407c); /* 0x2075654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][22], 0x192c7); /* 0x2075658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][23], 0x330ae); /* 0x207565c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][24], 0x26caf); /* 0x2075660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][25], 0x124fd); /* 0x2075664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][26], 0x2903d); /* 0x2075668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][27], 0xe3a6); /* 0x207566c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][28], 0x202e4); /* 0x2075670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][29], 0x39381); /* 0x2075674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][30], 0x13211); /* 0x2075678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][31], 0x3b790); /* 0x207567c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][32], 0x13a78); /* 0x2075680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][33], 0x338da); /* 0x2075684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][34], 0x10b70); /* 0x2075688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][35], 0x3c795); /* 0x207568c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][36], 0x5999); /* 0x2075690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][37], 0x11634); /* 0x2075694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][38], 0x1d742); /* 0x2075698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][39], 0x3cde7); /* 0x207569c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][40], 0x1ca66); /* 0x20756a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][41], 0x24bd6); /* 0x20756a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][42], 0x3fff8); /* 0x20756a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][43], 0x1bb9f); /* 0x20756ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][44], 0xa3a7); /* 0x20756b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][45], 0x7a55); /* 0x20756b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][46], 0x31bcf); /* 0x20756b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][47], 0x3ffe7); /* 0x20756bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][48], 0x38a0d); /* 0x20756c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][49], 0x2710d); /* 0x20756c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][50], 0x32451); /* 0x20756c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][51], 0xc95c); /* 0x20756cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][0], 0x20a3e); /* 0x2075700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][1], 0x15a79); /* 0x2075704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][2], 0xfe20); /* 0x2075708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][3], 0x37bf0); /* 0x207570c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][4], 0x1930f); /* 0x2075710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][5], 0x15eac); /* 0x2075714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][6], 0xbb58); /* 0x2075718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][7], 0x3f943); /* 0x207571c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][8], 0x258ae); /* 0x2075720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][9], 0x3b9c0); /* 0x2075724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][10], 0x187a9); /* 0x2075728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][11], 0x1e72f); /* 0x207572c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][12], 0x186ce); /* 0x2075730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][13], 0x3dfe3); /* 0x2075734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][14], 0x3ade8); /* 0x2075738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][15], 0x152b7); /* 0x207573c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][16], 0x116d6); /* 0x2075740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][17], 0xacef); /* 0x2075744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][18], 0x25c29); /* 0x2075748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][19], 0x170f); /* 0x207574c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][20], 0x39fa9); /* 0x2075750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][21], 0x1ccf7); /* 0x2075754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][22], 0x3b8fd); /* 0x2075758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][23], 0x19f3); /* 0x207575c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][24], 0xbecb); /* 0x2075760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][25], 0x2c28f); /* 0x2075764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][26], 0x3e239); /* 0x2075768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][27], 0x997b); /* 0x207576c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][28], 0x3ffc3); /* 0x2075770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][29], 0x236f8); /* 0x2075774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][30], 0x1f127); /* 0x2075778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][31], 0x37a30); /* 0x207577c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][32], 0x3b5d7); /* 0x2075780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][33], 0x2fd38); /* 0x2075784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][34], 0x23be4); /* 0x2075788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][35], 0x2c9fa); /* 0x207578c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][36], 0xa12c); /* 0x2075790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][37], 0x3d5a9); /* 0x2075794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][38], 0x1ce94); /* 0x2075798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][39], 0x1684c); /* 0x207579c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][40], 0x1aa7b); /* 0x20757a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][41], 0x16f16); /* 0x20757a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][42], 0x274a5); /* 0x20757a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][43], 0x28437); /* 0x20757ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][44], 0x19f02); /* 0x20757b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][45], 0x17fd3); /* 0x20757b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][46], 0x3dce8); /* 0x20757b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][47], 0x611e); /* 0x20757bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][48], 0x3b8b4); /* 0x20757c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][49], 0x14658); /* 0x20757c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][50], 0x1d64c); /* 0x20757c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][51], 0x18d31); /* 0x20757cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][0], 0x1ce80); /* 0x2075800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][1], 0x2b736); /* 0x2075804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][2], 0x3322e); /* 0x2075808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][3], 0x2a099); /* 0x207580c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][4], 0x7dbb); /* 0x2075810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][5], 0x32aa9); /* 0x2075814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][6], 0x13e83); /* 0x2075818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][7], 0x3261d); /* 0x207581c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][8], 0x1c98a); /* 0x2075820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][9], 0x3a6eb); /* 0x2075824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][10], 0x1552f); /* 0x2075828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][11], 0x275db); /* 0x207582c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][12], 0x2cf7a); /* 0x2075830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][13], 0x25b82); /* 0x2075834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][14], 0x19f2e); /* 0x2075838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][15], 0x29c32); /* 0x207583c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][16], 0x12e79); /* 0x2075840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][17], 0x3f53d); /* 0x2075844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][18], 0xd270); /* 0x2075848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][19], 0x2b6bb); /* 0x207584c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][20], 0x24a1b); /* 0x2075850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][21], 0x10993); /* 0x2075854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][22], 0x315d7); /* 0x2075858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][23], 0x38280); /* 0x207585c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][24], 0x1f972); /* 0x2075860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][25], 0x1ba86); /* 0x2075864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][26], 0x1290e); /* 0x2075868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][27], 0x2b432); /* 0x207586c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][28], 0x1d460); /* 0x2075870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][29], 0x333bf); /* 0x2075874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][30], 0xf9d7); /* 0x2075878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][31], 0x2902a); /* 0x207587c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][32], 0x13b3e); /* 0x2075880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][33], 0xc456); /* 0x2075884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][34], 0x2a807); /* 0x2075888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][35], 0x1c170); /* 0x207588c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][36], 0xd3fc); /* 0x2075890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][37], 0x2fd82); /* 0x2075894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][38], 0x370a6); /* 0x2075898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][39], 0x1ba79); /* 0x207589c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][40], 0x36601); /* 0x20758a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][41], 0x70c8); /* 0x20758a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][42], 0x28510); /* 0x20758a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][43], 0x1330f); /* 0x20758ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][44], 0x3fab6); /* 0x20758b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][45], 0x21ee3); /* 0x20758b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][46], 0x39f25); /* 0x20758b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][47], 0x1b4fe); /* 0x20758bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][48], 0x1327e); /* 0x20758c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][49], 0x3ce8f); /* 0x20758c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][50], 0x3c148); /* 0x20758c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][51], 0x388f6); /* 0x20758cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][0], 0x31735); /* 0x2075900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][1], 0x3e969); /* 0x2075904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][2], 0x3301d); /* 0x2075908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][3], 0x3b1ca); /* 0x207590c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][4], 0x3193b); /* 0x2075910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][5], 0x11a73); /* 0x2075914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][6], 0x2e2fe); /* 0x2075918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][7], 0x2b96e); /* 0x207591c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][8], 0x247c6); /* 0x2075920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][9], 0x2e4f6); /* 0x2075924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][10], 0x10933); /* 0x2075928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][11], 0x1e90e); /* 0x207592c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][12], 0x2cb47); /* 0x2075930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][13], 0x3c4b4); /* 0x2075934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][14], 0x185f); /* 0x2075938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][15], 0x10dd5); /* 0x207593c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][16], 0x124b0); /* 0x2075940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][17], 0x1b498); /* 0x2075944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][18], 0x2b1b5); /* 0x2075948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][19], 0x1b780); /* 0x207594c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][20], 0x2f496); /* 0x2075950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][21], 0x24210); /* 0x2075954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][22], 0x23ca); /* 0x2075958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][23], 0xf6a3); /* 0x207595c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][24], 0x34c6a); /* 0x2075960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][25], 0xb638); /* 0x2075964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][26], 0x153ac); /* 0x2075968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][27], 0xac2c); /* 0x207596c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][28], 0x2dab4); /* 0x2075970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][29], 0x38ede); /* 0x2075974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][30], 0x132be); /* 0x2075978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][31], 0x2f589); /* 0x207597c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][32], 0x2401d); /* 0x2075980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][33], 0x26105); /* 0x2075984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][34], 0x2293a); /* 0x2075988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][35], 0x2bf70); /* 0x207598c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][36], 0x1a6ad); /* 0x2075990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][37], 0x2f237); /* 0x2075994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][38], 0x1fa8d); /* 0x2075998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][39], 0x140c2); /* 0x207599c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][40], 0x2ef92); /* 0x20759a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][41], 0x8de7); /* 0x20759a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][42], 0x1ca2a); /* 0x20759a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][43], 0x21c29); /* 0x20759ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][44], 0x36f2b); /* 0x20759b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][45], 0x10c29); /* 0x20759b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][46], 0x1d0fe); /* 0x20759b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][47], 0x332bf); /* 0x20759bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][48], 0x1bc00); /* 0x20759c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][49], 0x1eb38); /* 0x20759c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][50], 0x5c3d); /* 0x20759c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][51], 0x3e317); /* 0x20759cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][0], 0x45ff); /* 0x2075a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][1], 0x2ae54); /* 0x2075a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][2], 0x2af88); /* 0x2075a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][3], 0x318b7); /* 0x2075a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][4], 0x258e6); /* 0x2075a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][5], 0x371a); /* 0x2075a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][6], 0xba3a); /* 0x2075a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][7], 0xfbc3); /* 0x2075a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][8], 0xb02); /* 0x2075a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][9], 0x25467); /* 0x2075a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][10], 0x27607); /* 0x2075a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][11], 0x9a8e); /* 0x2075a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][12], 0x38a1a); /* 0x2075a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][13], 0xf8aa); /* 0x2075a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][14], 0x3b2f2); /* 0x2075a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][15], 0x8817); /* 0x2075a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][16], 0xa387); /* 0x2075a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][17], 0x15426); /* 0x2075a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][18], 0x122f4); /* 0x2075a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][19], 0x14505); /* 0x2075a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][20], 0x26243); /* 0x2075a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][21], 0x3b15f); /* 0x2075a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][22], 0x35bcf); /* 0x2075a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][23], 0x24469); /* 0x2075a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][24], 0x1140c); /* 0x2075a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][25], 0x36b48); /* 0x2075a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][26], 0x3a29e); /* 0x2075a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][27], 0x15ac6); /* 0x2075a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][28], 0x124fe); /* 0x2075a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][29], 0x2f6a1); /* 0x2075a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][30], 0x3b637); /* 0x2075a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][31], 0x3bee2); /* 0x2075a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][32], 0x13b99); /* 0x2075a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][33], 0x16574); /* 0x2075a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][34], 0x1cd0e); /* 0x2075a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][35], 0x47bf); /* 0x2075a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][36], 0x3fca1); /* 0x2075a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][37], 0x15579); /* 0x2075a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][38], 0x3f79d); /* 0x2075a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][39], 0x38ac6); /* 0x2075a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][40], 0xc3d0); /* 0x2075aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][41], 0x2681f); /* 0x2075aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][42], 0x9403); /* 0x2075aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][43], 0x2b2f); /* 0x2075aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][44], 0x3fa1f); /* 0x2075ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][45], 0x3d786); /* 0x2075ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][46], 0x1ba0d); /* 0x2075ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][47], 0xbe33); /* 0x2075abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][48], 0x3ac84); /* 0x2075ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][49], 0x27191); /* 0x2075ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][50], 0xca08); /* 0x2075ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][51], 0x1a2a8); /* 0x2075acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][0], 0x196b9); /* 0x2075b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][1], 0x1fd34); /* 0x2075b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][2], 0x3cee4); /* 0x2075b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][3], 0x3840f); /* 0x2075b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][4], 0x10d1); /* 0x2075b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][5], 0x23dc8); /* 0x2075b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][6], 0x31f14); /* 0x2075b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][7], 0x3d635); /* 0x2075b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][8], 0x163e7); /* 0x2075b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][9], 0x19d70); /* 0x2075b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][10], 0x7fb9); /* 0x2075b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][11], 0x1c163); /* 0x2075b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][12], 0x33661); /* 0x2075b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][13], 0x3368); /* 0x2075b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][14], 0x151b0); /* 0x2075b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][15], 0x2fc36); /* 0x2075b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][16], 0x25c92); /* 0x2075b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][17], 0x2b1a0); /* 0x2075b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][18], 0x35783); /* 0x2075b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][19], 0x1cc5c); /* 0x2075b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][20], 0x26564); /* 0x2075b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][21], 0x2481f); /* 0x2075b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][22], 0x15a84); /* 0x2075b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][23], 0x5945); /* 0x2075b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][24], 0x18efe); /* 0x2075b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][25], 0x1a353); /* 0x2075b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][26], 0x6c34); /* 0x2075b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][27], 0x18dad); /* 0x2075b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][28], 0x3c0f); /* 0x2075b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][29], 0xd14); /* 0x2075b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][30], 0x8ad); /* 0x2075b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][31], 0x1f0b6); /* 0x2075b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][32], 0x35d3f); /* 0x2075b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][33], 0x75c2); /* 0x2075b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][34], 0x3c09d); /* 0x2075b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][35], 0x233ac); /* 0x2075b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][36], 0xf493); /* 0x2075b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][37], 0xfbe7); /* 0x2075b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][38], 0x1b3f6); /* 0x2075b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][39], 0x2ca47); /* 0x2075b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][40], 0x1fe05); /* 0x2075ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][41], 0xa046); /* 0x2075ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][42], 0x1edb8); /* 0x2075ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][43], 0x195d6); /* 0x2075bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][44], 0x1e1d4); /* 0x2075bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][45], 0x1ed1e); /* 0x2075bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][46], 0xfd92); /* 0x2075bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][47], 0x37820); /* 0x2075bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][48], 0x659a); /* 0x2075bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][49], 0x3232b); /* 0x2075bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][50], 0x2cf17); /* 0x2075bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][51], 0x25360); /* 0x2075bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][0], 0x24ba); /* 0x2075c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][1], 0x1c38b); /* 0x2075c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][2], 0x33751); /* 0x2075c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][3], 0x922e); /* 0x2075c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][4], 0x6ef0); /* 0x2075c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][5], 0x1c565); /* 0x2075c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][6], 0x218eb); /* 0x2075c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][7], 0xb5a); /* 0x2075c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][8], 0x9f4e); /* 0x2075c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][9], 0x26c19); /* 0x2075c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][10], 0x24ab5); /* 0x2075c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][11], 0x3cf4d); /* 0x2075c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][12], 0x3b3f9); /* 0x2075c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][13], 0x35086); /* 0x2075c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][14], 0x16e1b); /* 0x2075c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][15], 0x134ec); /* 0x2075c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][16], 0x34ab3); /* 0x2075c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][17], 0x34302); /* 0x2075c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][18], 0x2aa4c); /* 0x2075c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][19], 0x507f); /* 0x2075c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][20], 0xcf6e); /* 0x2075c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][21], 0x386d8); /* 0x2075c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][22], 0x3f6c6); /* 0x2075c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][23], 0x24f2f); /* 0x2075c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][24], 0x1ae26); /* 0x2075c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][25], 0x2d44b); /* 0x2075c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][26], 0x1431d); /* 0x2075c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][27], 0x1dcd4); /* 0x2075c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][28], 0x243cd); /* 0x2075c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][29], 0x2a24d); /* 0x2075c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][30], 0x20ed); /* 0x2075c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][31], 0x21ec0); /* 0x2075c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][32], 0x14bb8); /* 0x2075c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][33], 0x37256); /* 0x2075c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][34], 0xd0d6); /* 0x2075c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][35], 0x32d17); /* 0x2075c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][36], 0x28941); /* 0x2075c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][37], 0x16e42); /* 0x2075c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][38], 0x38383); /* 0x2075c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][39], 0x1206); /* 0x2075c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][40], 0x27f14); /* 0x2075ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][41], 0x39e02); /* 0x2075ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][42], 0x3863d); /* 0x2075ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][43], 0x30e59); /* 0x2075cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][44], 0x5576); /* 0x2075cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][45], 0x30a47); /* 0x2075cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][46], 0x2d752); /* 0x2075cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][47], 0x3b8a6); /* 0x2075cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][48], 0xb294); /* 0x2075cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][49], 0x1849d); /* 0x2075cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][50], 0xcdf2); /* 0x2075cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][51], 0x5b97); /* 0x2075ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][0], 0xc5c4); /* 0x2075d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][1], 0x23d74); /* 0x2075d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][2], 0x7089); /* 0x2075d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][3], 0x1c23f); /* 0x2075d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][4], 0x17ced); /* 0x2075d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][5], 0x2c266); /* 0x2075d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][6], 0x3be06); /* 0x2075d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][7], 0x3f87e); /* 0x2075d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][8], 0x18eed); /* 0x2075d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][9], 0x27a35); /* 0x2075d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][10], 0x8f4); /* 0x2075d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][11], 0x133df); /* 0x2075d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][12], 0xab45); /* 0x2075d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][13], 0x26311); /* 0x2075d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][14], 0x18d3c); /* 0x2075d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][15], 0x34f05); /* 0x2075d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][16], 0x12a82); /* 0x2075d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][17], 0xf782); /* 0x2075d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][18], 0x20710); /* 0x2075d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][19], 0x392b2); /* 0x2075d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][20], 0x57d1); /* 0x2075d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][21], 0x13661); /* 0x2075d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][22], 0x1d029); /* 0x2075d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][23], 0x36757); /* 0x2075d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][24], 0x3890a); /* 0x2075d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][25], 0x2336e); /* 0x2075d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][26], 0x6637); /* 0x2075d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][27], 0x30f29); /* 0x2075d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][28], 0x2bff8); /* 0x2075d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][29], 0xfef3); /* 0x2075d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][30], 0x2693a); /* 0x2075d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][31], 0x1d6f2); /* 0x2075d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][32], 0x1c940); /* 0x2075d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][33], 0x39617); /* 0x2075d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][34], 0x1ea8b); /* 0x2075d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][35], 0x338b5); /* 0x2075d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][36], 0x20e22); /* 0x2075d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][37], 0x65df); /* 0x2075d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][38], 0x2f81a); /* 0x2075d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][39], 0x78ed); /* 0x2075d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][40], 0x3f730); /* 0x2075da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][41], 0x392d5); /* 0x2075da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][42], 0x15a83); /* 0x2075da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][43], 0x17053); /* 0x2075dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][44], 0x10143); /* 0x2075db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][45], 0x2b626); /* 0x2075db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][46], 0x28fca); /* 0x2075db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][47], 0x37a02); /* 0x2075dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][48], 0x23d03); /* 0x2075dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][49], 0x32f8a); /* 0x2075dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][50], 0x2c429); /* 0x2075dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][51], 0x176cf); /* 0x2075dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][0], 0x1986e); /* 0x2075e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][1], 0x278fd); /* 0x2075e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][2], 0x136d7); /* 0x2075e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][3], 0x34c81); /* 0x2075e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][4], 0x2b592); /* 0x2075e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][5], 0x45fc); /* 0x2075e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][6], 0x23e0); /* 0x2075e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][7], 0x23eb2); /* 0x2075e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][8], 0x2deab); /* 0x2075e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][9], 0x385d); /* 0x2075e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][10], 0x35a2c); /* 0x2075e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][11], 0xab18); /* 0x2075e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][12], 0x2918f); /* 0x2075e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][13], 0x2ea43); /* 0x2075e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][14], 0x2e6f0); /* 0x2075e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][15], 0x2ae8); /* 0x2075e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][16], 0x2ad19); /* 0x2075e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][17], 0x22df1); /* 0x2075e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][18], 0xf3); /* 0x2075e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][19], 0x3ff39); /* 0x2075e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][20], 0x37671); /* 0x2075e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][21], 0x22662); /* 0x2075e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][22], 0x26875); /* 0x2075e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][23], 0x2c3f4); /* 0x2075e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][24], 0x24a2b); /* 0x2075e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][25], 0x510f); /* 0x2075e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][26], 0x332c6); /* 0x2075e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][27], 0x35cee); /* 0x2075e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][28], 0x13b0); /* 0x2075e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][29], 0x1f85e); /* 0x2075e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][30], 0x3b551); /* 0x2075e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][31], 0x37eb9); /* 0x2075e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][32], 0x1a77d); /* 0x2075e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][33], 0x4a78); /* 0x2075e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][34], 0x37be2); /* 0x2075e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][35], 0xed5d); /* 0x2075e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][36], 0xdc7); /* 0x2075e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][37], 0x33e); /* 0x2075e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][38], 0x29818); /* 0x2075e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][39], 0x38f59); /* 0x2075e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][40], 0x3c2a9); /* 0x2075ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][41], 0xf318); /* 0x2075ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][42], 0x16a33); /* 0x2075ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][43], 0x3d652); /* 0x2075eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][44], 0xe9cf); /* 0x2075eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][45], 0x27cea); /* 0x2075eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][46], 0x93b2); /* 0x2075eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][47], 0x6230); /* 0x2075ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][48], 0x2927); /* 0x2075ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][49], 0x35095); /* 0x2075ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][50], 0x6466); /* 0x2075ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][51], 0x2869); /* 0x2075ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][0], 0x5c59); /* 0x2075f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][1], 0x2a0f); /* 0x2075f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][2], 0xed08); /* 0x2075f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][3], 0x17be); /* 0x2075f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][4], 0x25fbc); /* 0x2075f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][5], 0xdc9e); /* 0x2075f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][6], 0x3fa7b); /* 0x2075f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][7], 0x2287); /* 0x2075f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][8], 0x3e039); /* 0x2075f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][9], 0x343ef); /* 0x2075f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][10], 0xefb7); /* 0x2075f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][11], 0x3e888); /* 0x2075f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][12], 0x2198e); /* 0x2075f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][13], 0x500b); /* 0x2075f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][14], 0x3759); /* 0x2075f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][15], 0x5132); /* 0x2075f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][16], 0x3930d); /* 0x2075f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][17], 0x1a8d3); /* 0x2075f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][18], 0x3b45a); /* 0x2075f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][19], 0x33871); /* 0x2075f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][20], 0x1dd3c); /* 0x2075f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][21], 0x4e2d); /* 0x2075f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][22], 0x37488); /* 0x2075f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][23], 0x6aa6); /* 0x2075f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][24], 0x41c5); /* 0x2075f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][25], 0x121e6); /* 0x2075f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][26], 0x17623); /* 0x2075f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][27], 0x3fd22); /* 0x2075f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][28], 0x3543c); /* 0x2075f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][29], 0x18a39); /* 0x2075f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][30], 0x2fd31); /* 0x2075f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][31], 0xe2cc); /* 0x2075f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][32], 0x1c49a); /* 0x2075f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][33], 0x3ff4c); /* 0x2075f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][34], 0xafbd); /* 0x2075f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][35], 0x378de); /* 0x2075f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][36], 0x62bb); /* 0x2075f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][37], 0x251b0); /* 0x2075f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][38], 0x183bc); /* 0x2075f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][39], 0x11564); /* 0x2075f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][40], 0x2cc90); /* 0x2075fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][41], 0x331d4); /* 0x2075fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][42], 0x20382); /* 0x2075fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][43], 0x8f1); /* 0x2075fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][44], 0x257d5); /* 0x2075fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][45], 0xd988); /* 0x2075fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][46], 0x1c252); /* 0x2075fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][47], 0x1de72); /* 0x2075fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][48], 0x2c60); /* 0x2075fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][49], 0x6e2f); /* 0x2075fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][50], 0xcbf9); /* 0x2075fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][51], 0x3ce26); /* 0x2075fcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][0], 0x33725); /* 0x2076000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][1], 0xa287); /* 0x2076004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][2], 0x16e30); /* 0x2076008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][3], 0x37b61); /* 0x207600c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][4], 0x30fd); /* 0x2076010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][5], 0x1bbd1); /* 0x2076014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][6], 0x3f9ab); /* 0x2076018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][7], 0x3d28); /* 0x207601c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][8], 0x2f699); /* 0x2076020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][9], 0x32c1a); /* 0x2076024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][10], 0x15569); /* 0x2076028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][11], 0x35000); /* 0x207602c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][12], 0x1827b); /* 0x2076030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][13], 0x24a32); /* 0x2076034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][14], 0x21a1e); /* 0x2076038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][15], 0x243ff); /* 0x207603c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][16], 0xeecd); /* 0x2076040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][17], 0x2958b); /* 0x2076044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][18], 0x34f50); /* 0x2076048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][19], 0x41ba); /* 0x207604c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][20], 0x48d); /* 0x2076050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][21], 0x31d38); /* 0x2076054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][22], 0x320b4); /* 0x2076058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][23], 0x988b); /* 0x207605c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][24], 0x33750); /* 0x2076060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][25], 0x3e081); /* 0x2076064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][26], 0x2d4c8); /* 0x2076068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][27], 0x3efc6); /* 0x207606c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][28], 0x38ce9); /* 0x2076070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][29], 0x75ba); /* 0x2076074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][30], 0x3485c); /* 0x2076078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][31], 0x2d39e); /* 0x207607c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][32], 0x2fc4f); /* 0x2076080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][33], 0x23c28); /* 0x2076084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][34], 0x18956); /* 0x2076088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][35], 0x38e2b); /* 0x207608c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][36], 0xdc52); /* 0x2076090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][37], 0xd080); /* 0x2076094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][38], 0x3c781); /* 0x2076098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][39], 0x21d6); /* 0x207609c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][40], 0x855b); /* 0x20760a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][41], 0x15d23); /* 0x20760a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][42], 0x38a89); /* 0x20760a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][43], 0x3ef80); /* 0x20760ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][44], 0x3c1e4); /* 0x20760b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][45], 0x34af1); /* 0x20760b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][46], 0x368dd); /* 0x20760b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][47], 0xa528); /* 0x20760bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][48], 0x96a0); /* 0x20760c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][49], 0x1585a); /* 0x20760c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][50], 0x28238); /* 0x20760c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][51], 0x3ef4); /* 0x20760cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][0], 0x25d29); /* 0x2076100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][1], 0x1074d); /* 0x2076104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][2], 0x1fd3c); /* 0x2076108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][3], 0x3e40c); /* 0x207610c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][4], 0x8fce); /* 0x2076110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][5], 0x9d42); /* 0x2076114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][6], 0x3ac22); /* 0x2076118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][7], 0x1e4a1); /* 0x207611c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][8], 0x3e3d4); /* 0x2076120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][9], 0x2e036); /* 0x2076124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][10], 0x26e3d); /* 0x2076128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][11], 0x1010e); /* 0x207612c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][12], 0x381f1); /* 0x2076130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][13], 0x26dc0); /* 0x2076134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][14], 0x2e513); /* 0x2076138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][15], 0x34cce); /* 0x207613c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][16], 0x156c6); /* 0x2076140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][17], 0x13f61); /* 0x2076144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][18], 0x1ec82); /* 0x2076148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][19], 0x6c43); /* 0x207614c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][20], 0x1989e); /* 0x2076150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][21], 0x2e69b); /* 0x2076154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][22], 0x34083); /* 0x2076158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][23], 0x3c9de); /* 0x207615c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][24], 0x11ddd); /* 0x2076160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][25], 0x30c2b); /* 0x2076164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][26], 0x3ff82); /* 0x2076168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][27], 0x214d2); /* 0x207616c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][28], 0xba02); /* 0x2076170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][29], 0x35a36); /* 0x2076174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][30], 0x117de); /* 0x2076178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][31], 0x2d674); /* 0x207617c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][32], 0x18d0c); /* 0x2076180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][33], 0x6b6); /* 0x2076184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][34], 0x2e50a); /* 0x2076188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][35], 0x5a55); /* 0x207618c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][36], 0x2bf33); /* 0x2076190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][37], 0x311e9); /* 0x2076194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][38], 0x74a7); /* 0x2076198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][39], 0x3f52); /* 0x207619c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][40], 0x27bbb); /* 0x20761a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][41], 0x29f0b); /* 0x20761a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][42], 0x3eaf0); /* 0x20761a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][43], 0x3598c); /* 0x20761ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][44], 0x255a); /* 0x20761b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][45], 0x1b145); /* 0x20761b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][46], 0x7fd4); /* 0x20761b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][47], 0x19ca9); /* 0x20761bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][48], 0x30522); /* 0x20761c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][49], 0x303f); /* 0x20761c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][50], 0x3c42b); /* 0x20761c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][51], 0x10b0); /* 0x20761cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][0], 0x127a2); /* 0x2076200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][1], 0x272f1); /* 0x2076204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][2], 0x4904); /* 0x2076208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][3], 0x36e3e); /* 0x207620c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][4], 0x2fd8c); /* 0x2076210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][5], 0x22db2); /* 0x2076214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][6], 0x1d62b); /* 0x2076218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][7], 0x2313a); /* 0x207621c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][8], 0x2dd06); /* 0x2076220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][9], 0x31fe2); /* 0x2076224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][10], 0x4c89); /* 0x2076228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][11], 0x1bbd7); /* 0x207622c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][12], 0x24207); /* 0x2076230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][13], 0x1561); /* 0x2076234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][14], 0x1b018); /* 0x2076238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][15], 0x2a553); /* 0x207623c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][16], 0x2c364); /* 0x2076240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][17], 0x6bfb); /* 0x2076244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][18], 0x3d197); /* 0x2076248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][19], 0x6e74); /* 0x207624c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][20], 0x34523); /* 0x2076250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][21], 0x90c3); /* 0x2076254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][22], 0x31323); /* 0x2076258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][23], 0x12a5); /* 0x207625c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][24], 0x39ba4); /* 0x2076260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][25], 0x70cb); /* 0x2076264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][26], 0x128b7); /* 0x2076268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][27], 0x2e9b0); /* 0x207626c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][28], 0x465b); /* 0x2076270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][29], 0x3434); /* 0x2076274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][30], 0x15062); /* 0x2076278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][31], 0x3c77b); /* 0x207627c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][32], 0x6ba7); /* 0x2076280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][33], 0x35892); /* 0x2076284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][34], 0x25abf); /* 0x2076288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][35], 0xabc2); /* 0x207628c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][36], 0x31999); /* 0x2076290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][37], 0x1b869); /* 0x2076294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][38], 0x2b16); /* 0x2076298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][39], 0x26997); /* 0x207629c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][40], 0x2adf2); /* 0x20762a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][41], 0x32f76); /* 0x20762a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][42], 0x188f6); /* 0x20762a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][43], 0x100d7); /* 0x20762ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][44], 0x2bce); /* 0x20762b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][45], 0x6d03); /* 0x20762b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][46], 0x3a1a9); /* 0x20762b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][47], 0x38d7f); /* 0x20762bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][48], 0x18659); /* 0x20762c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][49], 0x2d5d7); /* 0x20762c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][50], 0xd0fc); /* 0x20762c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][51], 0x1e256); /* 0x20762cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][0], 0x566f); /* 0x2076300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][1], 0x31e4d); /* 0x2076304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][2], 0x72ea); /* 0x2076308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][3], 0x2ce8e); /* 0x207630c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][4], 0x22c9e); /* 0x2076310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][5], 0x24ace); /* 0x2076314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][6], 0x165f7); /* 0x2076318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][7], 0x125ed); /* 0x207631c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][8], 0x15219); /* 0x2076320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][9], 0x24210); /* 0x2076324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][10], 0x24347); /* 0x2076328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][11], 0x3ba8e); /* 0x207632c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][12], 0x3220a); /* 0x2076330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][13], 0x16029); /* 0x2076334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][14], 0x33bf7); /* 0x2076338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][15], 0x2332e); /* 0x207633c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][16], 0x2b538); /* 0x2076340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][17], 0x185d9); /* 0x2076344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][18], 0x3e15c); /* 0x2076348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][19], 0x1bcb1); /* 0x207634c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][20], 0x13b37); /* 0x2076350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][21], 0x2a82c); /* 0x2076354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][22], 0x11cd0); /* 0x2076358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][23], 0x92ee); /* 0x207635c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][24], 0x19d03); /* 0x2076360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][25], 0x22bcd); /* 0x2076364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][26], 0x2ce0b); /* 0x2076368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][27], 0x21b17); /* 0x207636c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][28], 0x2206f); /* 0x2076370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][29], 0x12448); /* 0x2076374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][30], 0x2332c); /* 0x2076378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][31], 0xacb7); /* 0x207637c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][32], 0x30df6); /* 0x2076380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][33], 0x202f0); /* 0x2076384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][34], 0x26eff); /* 0x2076388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][35], 0x34be6); /* 0x207638c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][36], 0x302f8); /* 0x2076390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][37], 0x2292f); /* 0x2076394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][38], 0x10490); /* 0x2076398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][39], 0x34f7); /* 0x207639c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][40], 0x1448a); /* 0x20763a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][41], 0x25fe4); /* 0x20763a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][42], 0x1a2b6); /* 0x20763a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][43], 0x35696); /* 0x20763ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][44], 0x30e36); /* 0x20763b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][45], 0xf120); /* 0x20763b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][46], 0x3f192); /* 0x20763b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][47], 0x3fcf7); /* 0x20763bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][48], 0x204a9); /* 0x20763c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][49], 0x2f6e); /* 0x20763c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][50], 0x3607c); /* 0x20763c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][51], 0x1750e); /* 0x20763cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][0], 0x43f9); /* 0x2076400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][1], 0x157f7); /* 0x2076404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][2], 0x1d33c); /* 0x2076408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][3], 0x34826); /* 0x207640c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][4], 0x132dc); /* 0x2076410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][5], 0xac89); /* 0x2076414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][6], 0x32bbc); /* 0x2076418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][7], 0xd894); /* 0x207641c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][8], 0x8a3c); /* 0x2076420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][9], 0xe776); /* 0x2076424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][10], 0x4a5a); /* 0x2076428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][11], 0x4f7f); /* 0x207642c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][12], 0xc2bf); /* 0x2076430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][13], 0x1df8c); /* 0x2076434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][14], 0xd66); /* 0x2076438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][15], 0x12167); /* 0x207643c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][16], 0x1ffd8); /* 0x2076440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][17], 0x259ba); /* 0x2076444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][18], 0x201c9); /* 0x2076448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][19], 0x3fa8); /* 0x207644c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][20], 0xd01c); /* 0x2076450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][21], 0x79af); /* 0x2076454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][22], 0x1ca98); /* 0x2076458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][23], 0x2a4db); /* 0x207645c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][24], 0x11d00); /* 0x2076460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][25], 0xaea1); /* 0x2076464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][26], 0x38de8); /* 0x2076468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][27], 0x1cb1c); /* 0x207646c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][28], 0x13d0); /* 0x2076470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][29], 0x1a601); /* 0x2076474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][30], 0x306cd); /* 0x2076478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][31], 0xa18); /* 0x207647c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][32], 0x39f0f); /* 0x2076480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][33], 0x3a61f); /* 0x2076484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][34], 0x2c74b); /* 0x2076488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][35], 0x24558); /* 0x207648c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][36], 0x2c361); /* 0x2076490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][37], 0x25d4e); /* 0x2076494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][38], 0x367b8); /* 0x2076498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][39], 0x3d17f); /* 0x207649c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][40], 0x3445); /* 0x20764a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][41], 0x5400); /* 0x20764a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][42], 0x27bed); /* 0x20764a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][43], 0x3e6b0); /* 0x20764ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][44], 0x6ca9); /* 0x20764b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][45], 0x26e48); /* 0x20764b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][46], 0x3b059); /* 0x20764b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][47], 0x29db8); /* 0x20764bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][48], 0x185b); /* 0x20764c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][49], 0xef90); /* 0x20764c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][50], 0x23875); /* 0x20764c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][51], 0x1e802); /* 0x20764cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][0], 0x147c3); /* 0x2076500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][1], 0x3dee0); /* 0x2076504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][2], 0xa769); /* 0x2076508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][3], 0x2f0f1); /* 0x207650c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][4], 0x1782a); /* 0x2076510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][5], 0x1e8b1); /* 0x2076514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][6], 0x315e3); /* 0x2076518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][7], 0x2d554); /* 0x207651c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][8], 0x34555); /* 0x2076520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][9], 0x3c9b5); /* 0x2076524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][10], 0x25bda); /* 0x2076528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][11], 0x19fe4); /* 0x207652c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][12], 0x20325); /* 0x2076530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][13], 0xda8a); /* 0x2076534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][14], 0x4a0); /* 0x2076538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][15], 0xb5e8); /* 0x207653c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][16], 0x2130e); /* 0x2076540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][17], 0x2820a); /* 0x2076544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][18], 0x1daa); /* 0x2076548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][19], 0xe2c5); /* 0x207654c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][20], 0x2d3e); /* 0x2076550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][21], 0x1cbf5); /* 0x2076554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][22], 0x8ebf); /* 0x2076558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][23], 0x3814e); /* 0x207655c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][24], 0x17da6); /* 0x2076560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][25], 0x27e81); /* 0x2076564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][26], 0x106bc); /* 0x2076568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][27], 0xe993); /* 0x207656c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][28], 0x14d68); /* 0x2076570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][29], 0x36c4d); /* 0x2076574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][30], 0x39e58); /* 0x2076578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][31], 0x17b15); /* 0x207657c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][32], 0x38f0a); /* 0x2076580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][33], 0x3c963); /* 0x2076584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][34], 0x3fb59); /* 0x2076588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][35], 0x1e857); /* 0x207658c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][36], 0x2430d); /* 0x2076590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][37], 0x36049); /* 0x2076594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][38], 0xb4f4); /* 0x2076598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][39], 0x20c89); /* 0x207659c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][40], 0x522d); /* 0x20765a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][41], 0x1d59c); /* 0x20765a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][42], 0x3311e); /* 0x20765a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][43], 0xe703); /* 0x20765ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][44], 0x36952); /* 0x20765b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][45], 0x34ad2); /* 0x20765b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][46], 0x26090); /* 0x20765b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][47], 0x3fa1f); /* 0x20765bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][48], 0x2426); /* 0x20765c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][49], 0x54b9); /* 0x20765c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][50], 0x11479); /* 0x20765c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][51], 0x25b25); /* 0x20765cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][0], 0x206c4); /* 0x2076600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][1], 0x1521e); /* 0x2076604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][2], 0x305b2); /* 0x2076608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][3], 0x243a8); /* 0x207660c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][4], 0x3677); /* 0x2076610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][5], 0x814c); /* 0x2076614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][6], 0x339dc); /* 0x2076618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][7], 0x25c92); /* 0x207661c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][8], 0x33edc); /* 0x2076620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][9], 0x842a); /* 0x2076624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][10], 0x593f); /* 0x2076628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][11], 0x1d42a); /* 0x207662c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][12], 0x116a9); /* 0x2076630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][13], 0x1db7a); /* 0x2076634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][14], 0x12f11); /* 0x2076638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][15], 0x4afd); /* 0x207663c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][16], 0x16070); /* 0x2076640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][17], 0x2c0b0); /* 0x2076644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][18], 0x319dc); /* 0x2076648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][19], 0x2f058); /* 0x207664c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][20], 0x1ba9a); /* 0x2076650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][21], 0x3e634); /* 0x2076654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][22], 0x2033e); /* 0x2076658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][23], 0x2ba50); /* 0x207665c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][24], 0x125da); /* 0x2076660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][25], 0x223e9); /* 0x2076664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][26], 0x3f766); /* 0x2076668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][27], 0x37e57); /* 0x207666c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][28], 0x39f36); /* 0x2076670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][29], 0x3577e); /* 0x2076674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][30], 0x3d3e); /* 0x2076678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][31], 0x30a22); /* 0x207667c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][32], 0x2b1fd); /* 0x2076680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][33], 0x2db9b); /* 0x2076684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][34], 0x12c1d); /* 0x2076688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][35], 0x1868d); /* 0x207668c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][36], 0x66f); /* 0x2076690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][37], 0x38a42); /* 0x2076694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][38], 0xbe21); /* 0x2076698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][39], 0x10c28); /* 0x207669c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][40], 0x6bdf); /* 0x20766a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][41], 0x315b8); /* 0x20766a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][42], 0x3206a); /* 0x20766a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][43], 0x11db6); /* 0x20766ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][44], 0x18e26); /* 0x20766b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][45], 0x2055d); /* 0x20766b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][46], 0x280f1); /* 0x20766b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][47], 0x17b9a); /* 0x20766bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][48], 0x2d0f6); /* 0x20766c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][49], 0x26274); /* 0x20766c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][50], 0x1919b); /* 0x20766c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][51], 0x15eb3); /* 0x20766cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][0], 0x10cae); /* 0x2076700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][1], 0xca8a); /* 0x2076704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][2], 0x28e1d); /* 0x2076708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][3], 0xa1e); /* 0x207670c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][4], 0x1eef6); /* 0x2076710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][5], 0x5007); /* 0x2076714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][6], 0x3f73); /* 0x2076718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][7], 0x5602); /* 0x207671c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][8], 0x78d1); /* 0x2076720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][9], 0x1ed3); /* 0x2076724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][10], 0x6ff6); /* 0x2076728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][11], 0x7667); /* 0x207672c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][12], 0x3541c); /* 0x2076730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][13], 0x17114); /* 0x2076734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][14], 0x3ce30); /* 0x2076738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][15], 0x1f85f); /* 0x207673c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][16], 0x2a081); /* 0x2076740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][17], 0x3b378); /* 0x2076744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][18], 0x119a3); /* 0x2076748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][19], 0x2aaec); /* 0x207674c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][20], 0x3893c); /* 0x2076750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][21], 0x297f8); /* 0x2076754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][22], 0x13c66); /* 0x2076758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][23], 0x384c); /* 0x207675c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][24], 0x2e953); /* 0x2076760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][25], 0x1b41c); /* 0x2076764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][26], 0x368c6); /* 0x2076768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][27], 0x5f11); /* 0x207676c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][28], 0x2f88c); /* 0x2076770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][29], 0x444e); /* 0x2076774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][30], 0x2e69f); /* 0x2076778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][31], 0x30aec); /* 0x207677c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][32], 0x116bc); /* 0x2076780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][33], 0xa48e); /* 0x2076784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][34], 0x18a44); /* 0x2076788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][35], 0x2d2f0); /* 0x207678c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][36], 0x32b41); /* 0x2076790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][37], 0x23f02); /* 0x2076794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][38], 0x392cd); /* 0x2076798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][39], 0x1733e); /* 0x207679c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][40], 0x3c323); /* 0x20767a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][41], 0x205f1); /* 0x20767a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][42], 0x2417c); /* 0x20767a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][43], 0x8474); /* 0x20767ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][44], 0x329a5); /* 0x20767b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][45], 0x2d583); /* 0x20767b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][46], 0x5a31); /* 0x20767b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][47], 0x125bf); /* 0x20767bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][48], 0xa0ad); /* 0x20767c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][49], 0x29b05); /* 0x20767c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][50], 0x2f9cf); /* 0x20767c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][51], 0x2fb6a); /* 0x20767cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][0], 0x2b458); /* 0x2076800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][1], 0x22da4); /* 0x2076804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][2], 0x337c3); /* 0x2076808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][3], 0x1bc3a); /* 0x207680c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][4], 0x1b058); /* 0x2076810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][5], 0x2b91a); /* 0x2076814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][6], 0x2e21d); /* 0x2076818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][7], 0x21731); /* 0x207681c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][8], 0x27453); /* 0x2076820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][9], 0x2de5e); /* 0x2076824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][10], 0xf93a); /* 0x2076828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][11], 0x297e2); /* 0x207682c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][12], 0x19b07); /* 0x2076830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][13], 0x4d60); /* 0x2076834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][14], 0x2a1be); /* 0x2076838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][15], 0x18588); /* 0x207683c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][16], 0x1e5bb); /* 0x2076840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][17], 0x2e7a7); /* 0x2076844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][18], 0x3a257); /* 0x2076848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][19], 0x17fde); /* 0x207684c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][20], 0x2fc36); /* 0x2076850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][21], 0x17d1); /* 0x2076854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][22], 0x3379a); /* 0x2076858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][23], 0x5801); /* 0x207685c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][24], 0x116b1); /* 0x2076860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][25], 0x30e44); /* 0x2076864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][26], 0x2668f); /* 0x2076868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][27], 0xc927); /* 0x207686c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][28], 0x12b71); /* 0x2076870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][29], 0x3de4b); /* 0x2076874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][30], 0x2b2a8); /* 0x2076878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][31], 0x2d3f0); /* 0x207687c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][32], 0x37431); /* 0x2076880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][33], 0xcd10); /* 0x2076884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][34], 0x2ccaf); /* 0x2076888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][35], 0x37427); /* 0x207688c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][36], 0x21320); /* 0x2076890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][37], 0x17593); /* 0x2076894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][38], 0x14db6); /* 0x2076898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][39], 0xda52); /* 0x207689c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][40], 0x3daa7); /* 0x20768a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][41], 0x2b40); /* 0x20768a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][42], 0x2146e); /* 0x20768a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][43], 0x31dab); /* 0x20768ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][44], 0x616); /* 0x20768b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][45], 0x1ea1b); /* 0x20768b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][46], 0x2447e); /* 0x20768b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][47], 0x26210); /* 0x20768bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][48], 0x108e9); /* 0x20768c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][49], 0x1c705); /* 0x20768c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][50], 0x3b374); /* 0x20768c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][51], 0x11ee2); /* 0x20768cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][0], 0x12efd); /* 0x2076900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][1], 0x3c2af); /* 0x2076904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][2], 0x2229c); /* 0x2076908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][3], 0x151ad); /* 0x207690c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][4], 0x114a7); /* 0x2076910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][5], 0x196f9); /* 0x2076914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][6], 0x1c616); /* 0x2076918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][7], 0x36289); /* 0x207691c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][8], 0x3f491); /* 0x2076920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][9], 0x212af); /* 0x2076924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][10], 0x28552); /* 0x2076928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][11], 0x2808d); /* 0x207692c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][12], 0x9756); /* 0x2076930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][13], 0x2cfc8); /* 0x2076934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][14], 0x2c142); /* 0x2076938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][15], 0x1e9ea); /* 0x207693c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][16], 0x2e7f); /* 0x2076940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][17], 0xc530); /* 0x2076944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][18], 0x2b77f); /* 0x2076948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][19], 0xa39d); /* 0x207694c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][20], 0x19c74); /* 0x2076950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][21], 0x2ee8f); /* 0x2076954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][22], 0x9ce9); /* 0x2076958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][23], 0x2bdcd); /* 0x207695c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][24], 0x3a80e); /* 0x2076960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][25], 0x3d42); /* 0x2076964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][26], 0x169a5); /* 0x2076968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][27], 0x1fecd); /* 0x207696c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][28], 0x2ed1a); /* 0x2076970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][29], 0x3a877); /* 0x2076974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][30], 0x1a64); /* 0x2076978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][31], 0x18a9e); /* 0x207697c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][32], 0x3750e); /* 0x2076980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][33], 0x1eaf5); /* 0x2076984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][34], 0x1fd1a); /* 0x2076988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][35], 0x291cb); /* 0x207698c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][36], 0x404e); /* 0x2076990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][37], 0xd94c); /* 0x2076994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][38], 0xb47b); /* 0x2076998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][39], 0x25b04); /* 0x207699c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][40], 0x20795); /* 0x20769a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][41], 0x29406); /* 0x20769a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][42], 0x13ebc); /* 0x20769a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][43], 0x31927); /* 0x20769ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][44], 0x16969); /* 0x20769b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][45], 0x37d0d); /* 0x20769b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][46], 0x31b30); /* 0x20769b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][47], 0xff3); /* 0x20769bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][48], 0x112a2); /* 0x20769c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][49], 0x6fb2); /* 0x20769c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][50], 0x3f75a); /* 0x20769c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][51], 0x2fe76); /* 0x20769cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][0], 0x1993d); /* 0x2076a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][1], 0x3ae57); /* 0x2076a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][2], 0xfc0a); /* 0x2076a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][3], 0x66b0); /* 0x2076a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][4], 0x24312); /* 0x2076a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][5], 0x1d65e); /* 0x2076a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][6], 0x346fa); /* 0x2076a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][7], 0x19485); /* 0x2076a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][8], 0x1069d); /* 0x2076a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][9], 0xcc45); /* 0x2076a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][10], 0x36d8); /* 0x2076a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][11], 0x99f7); /* 0x2076a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][12], 0x1a155); /* 0x2076a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][13], 0x3971f); /* 0x2076a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][14], 0x21986); /* 0x2076a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][15], 0xb72e); /* 0x2076a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][16], 0x28859); /* 0x2076a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][17], 0x1513); /* 0x2076a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][18], 0x3b40f); /* 0x2076a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][19], 0x157a4); /* 0x2076a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][20], 0x1c84d); /* 0x2076a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][21], 0x31fc0); /* 0x2076a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][22], 0x1393c); /* 0x2076a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][23], 0x1b16e); /* 0x2076a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][24], 0x192d); /* 0x2076a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][25], 0x1d05d); /* 0x2076a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][26], 0x9b29); /* 0x2076a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][27], 0x1268f); /* 0x2076a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][28], 0x239ec); /* 0x2076a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][29], 0x14425); /* 0x2076a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][30], 0x17695); /* 0x2076a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][31], 0x37552); /* 0x2076a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][32], 0x25923); /* 0x2076a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][33], 0x357d0); /* 0x2076a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][34], 0xf3a5); /* 0x2076a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][35], 0x1fb2b); /* 0x2076a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][36], 0x345a8); /* 0x2076a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][37], 0x30632); /* 0x2076a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][38], 0x28fa2); /* 0x2076a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][39], 0xcd1); /* 0x2076a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][40], 0x20753); /* 0x2076aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][41], 0x3a992); /* 0x2076aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][42], 0x4f75); /* 0x2076aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][43], 0x3a56); /* 0x2076aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][44], 0x3bfdd); /* 0x2076ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][45], 0x51bb); /* 0x2076ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][46], 0x39ca6); /* 0x2076ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][47], 0x105cc); /* 0x2076abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][48], 0x3ece3); /* 0x2076ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][49], 0x37807); /* 0x2076ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][50], 0x17ed9); /* 0x2076ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][51], 0xb1e); /* 0x2076acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][0], 0x19299); /* 0x2076b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][1], 0x121c2); /* 0x2076b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][2], 0x6e59); /* 0x2076b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][3], 0x22129); /* 0x2076b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][4], 0x14e5b); /* 0x2076b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][5], 0x30a25); /* 0x2076b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][6], 0x14eff); /* 0x2076b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][7], 0x210a3); /* 0x2076b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][8], 0x4856); /* 0x2076b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][9], 0x363bc); /* 0x2076b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][10], 0x1b89b); /* 0x2076b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][11], 0x17da6); /* 0x2076b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][12], 0x1b717); /* 0x2076b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][13], 0x1594a); /* 0x2076b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][14], 0x36dc8); /* 0x2076b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][15], 0x34862); /* 0x2076b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][16], 0x3ab2f); /* 0x2076b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][17], 0x16e4); /* 0x2076b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][18], 0x20af1); /* 0x2076b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][19], 0xe9fc); /* 0x2076b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][20], 0x17484); /* 0x2076b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][21], 0xe0d2); /* 0x2076b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][22], 0x3a701); /* 0x2076b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][23], 0x347f3); /* 0x2076b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][24], 0x138f0); /* 0x2076b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][25], 0xa65e); /* 0x2076b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][26], 0xcb15); /* 0x2076b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][27], 0x2b62e); /* 0x2076b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][28], 0x1a6c2); /* 0x2076b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][29], 0x166d9); /* 0x2076b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][30], 0x9d91); /* 0x2076b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][31], 0x2229c); /* 0x2076b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][32], 0x2bea2); /* 0x2076b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][33], 0x9893); /* 0x2076b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][34], 0x22c9c); /* 0x2076b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][35], 0x72c9); /* 0x2076b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][36], 0xae29); /* 0x2076b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][37], 0x1a729); /* 0x2076b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][38], 0x26963); /* 0x2076b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][39], 0x105d); /* 0x2076b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][40], 0x3431e); /* 0x2076ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][41], 0x3c1eb); /* 0x2076ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][42], 0x345b8); /* 0x2076ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][43], 0x27dbe); /* 0x2076bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][44], 0x19c9f); /* 0x2076bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][45], 0x39b37); /* 0x2076bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][46], 0x2066d); /* 0x2076bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][47], 0x3020c); /* 0x2076bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][48], 0x67a7); /* 0x2076bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][49], 0x1c393); /* 0x2076bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][50], 0x19430); /* 0x2076bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][51], 0x2fd9c); /* 0x2076bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][0], 0x23fa1); /* 0x2076c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][1], 0x306fd); /* 0x2076c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][2], 0x2fefe); /* 0x2076c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][3], 0x39e); /* 0x2076c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][4], 0x2c8cf); /* 0x2076c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][5], 0x281d5); /* 0x2076c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][6], 0x38fad); /* 0x2076c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][7], 0x3f473); /* 0x2076c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][8], 0x1ad11); /* 0x2076c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][9], 0x39ed0); /* 0x2076c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][10], 0x345aa); /* 0x2076c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][11], 0xf4f5); /* 0x2076c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][12], 0x30ac7); /* 0x2076c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][13], 0x10def); /* 0x2076c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][14], 0x2b7db); /* 0x2076c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][15], 0x1fa5); /* 0x2076c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][16], 0x5aa2); /* 0x2076c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][17], 0x3f7c7); /* 0x2076c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][18], 0x312a); /* 0x2076c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][19], 0x21cbb); /* 0x2076c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][20], 0x2c4a1); /* 0x2076c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][21], 0x21b05); /* 0x2076c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][22], 0x351cc); /* 0x2076c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][23], 0x22832); /* 0x2076c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][24], 0x25ae6); /* 0x2076c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][25], 0xce04); /* 0x2076c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][26], 0x23ed0); /* 0x2076c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][27], 0x26459); /* 0x2076c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][28], 0x136be); /* 0x2076c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][29], 0x25ca0); /* 0x2076c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][30], 0x36de3); /* 0x2076c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][31], 0x3b8ac); /* 0x2076c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][32], 0x23e65); /* 0x2076c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][33], 0x2b8d); /* 0x2076c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][34], 0x92b5); /* 0x2076c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][35], 0x37265); /* 0x2076c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][36], 0x1ba63); /* 0x2076c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][37], 0x3b37); /* 0x2076c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][38], 0x93df); /* 0x2076c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][39], 0x2b85); /* 0x2076c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][40], 0xc954); /* 0x2076ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][41], 0x39377); /* 0x2076ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][42], 0xd7f6); /* 0x2076ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][43], 0x3891e); /* 0x2076cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][44], 0x3dd6c); /* 0x2076cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][45], 0x2d9e5); /* 0x2076cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][46], 0x32d7d); /* 0x2076cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][47], 0x3c546); /* 0x2076cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][48], 0x36a90); /* 0x2076cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][49], 0x32c99); /* 0x2076cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][50], 0x8d9d); /* 0x2076cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][51], 0x11702); /* 0x2076ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][0], 0x5509); /* 0x2076d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][1], 0x2d39); /* 0x2076d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][2], 0x25701); /* 0x2076d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][3], 0x2733d); /* 0x2076d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][4], 0xa647); /* 0x2076d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][5], 0x276a4); /* 0x2076d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][6], 0x3d5af); /* 0x2076d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][7], 0x3d9c); /* 0x2076d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][8], 0x3ab3a); /* 0x2076d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][9], 0x1a17e); /* 0x2076d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][10], 0x9d14); /* 0x2076d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][11], 0x3090); /* 0x2076d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][12], 0x148e3); /* 0x2076d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][13], 0x2857b); /* 0x2076d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][14], 0x31e95); /* 0x2076d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][15], 0x25f8a); /* 0x2076d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][16], 0x2056f); /* 0x2076d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][17], 0x1ec54); /* 0x2076d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][18], 0x1b0ea); /* 0x2076d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][19], 0x120a2); /* 0x2076d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][20], 0x169d3); /* 0x2076d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][21], 0x266b6); /* 0x2076d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][22], 0x1d0df); /* 0x2076d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][23], 0x1430c); /* 0x2076d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][24], 0x3b9eb); /* 0x2076d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][25], 0x12c1e); /* 0x2076d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][26], 0x3a6bb); /* 0x2076d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][27], 0x2b9e4); /* 0x2076d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][28], 0x7a12); /* 0x2076d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][29], 0x36a16); /* 0x2076d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][30], 0x915); /* 0x2076d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][31], 0xfc73); /* 0x2076d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][32], 0x296b4); /* 0x2076d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][33], 0x2499c); /* 0x2076d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][34], 0x319f0); /* 0x2076d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][35], 0x3fdb9); /* 0x2076d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][36], 0x3aeee); /* 0x2076d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][37], 0x3bf7); /* 0x2076d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][38], 0x3fbfb); /* 0x2076d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][39], 0xf17a); /* 0x2076d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][40], 0x3f893); /* 0x2076da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][41], 0x5a94); /* 0x2076da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][42], 0x1f37d); /* 0x2076da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][43], 0x2b9d0); /* 0x2076dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][44], 0x185a4); /* 0x2076db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][45], 0x15ca); /* 0x2076db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][46], 0x175e5); /* 0x2076db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][47], 0x239cb); /* 0x2076dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][48], 0xd3e); /* 0x2076dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][49], 0x6142); /* 0x2076dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][50], 0x86a); /* 0x2076dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][51], 0x1fb9b); /* 0x2076dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][0], 0x2181); /* 0x2076e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][1], 0x9301); /* 0x2076e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][2], 0x4b7b); /* 0x2076e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][3], 0x11dfb); /* 0x2076e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][4], 0x34e04); /* 0x2076e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][5], 0xf989); /* 0x2076e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][6], 0x22f6c); /* 0x2076e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][7], 0x4847); /* 0x2076e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][8], 0x3c25f); /* 0x2076e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][9], 0x9d74); /* 0x2076e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][10], 0x3cfed); /* 0x2076e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][11], 0x13653); /* 0x2076e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][12], 0x3d96c); /* 0x2076e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][13], 0x3225e); /* 0x2076e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][14], 0x3e2f7); /* 0x2076e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][15], 0x306a8); /* 0x2076e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][16], 0xa4a2); /* 0x2076e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][17], 0x47); /* 0x2076e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][18], 0x28756); /* 0x2076e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][19], 0x383e0); /* 0x2076e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][20], 0x367d5); /* 0x2076e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][21], 0x15a61); /* 0x2076e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][22], 0x3286); /* 0x2076e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][23], 0x31034); /* 0x2076e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][24], 0x2f24a); /* 0x2076e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][25], 0x1a8c2); /* 0x2076e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][26], 0x3a927); /* 0x2076e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][27], 0x2b83e); /* 0x2076e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][28], 0x20afc); /* 0x2076e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][29], 0x1310b); /* 0x2076e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][30], 0x290c0); /* 0x2076e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][31], 0x33ade); /* 0x2076e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][32], 0x2c9d); /* 0x2076e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][33], 0x99fa); /* 0x2076e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][34], 0x3d46a); /* 0x2076e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][35], 0xd3f); /* 0x2076e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][36], 0xefdb); /* 0x2076e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][37], 0x1bb9); /* 0x2076e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][38], 0x74ae); /* 0x2076e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][39], 0x1b849); /* 0x2076e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][40], 0x25121); /* 0x2076ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][41], 0x22be6); /* 0x2076ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][42], 0x10f45); /* 0x2076ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][43], 0x1bb8b); /* 0x2076eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][44], 0x2b1b6); /* 0x2076eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][45], 0x3fcf9); /* 0x2076eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][46], 0xf23f); /* 0x2076eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][47], 0x28a7f); /* 0x2076ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][48], 0x21d3e); /* 0x2076ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][49], 0x209ed); /* 0x2076ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][50], 0xed42); /* 0x2076ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][51], 0x4b8); /* 0x2076ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][0], 0x3e510); /* 0x2076f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][1], 0x3bcd5); /* 0x2076f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][2], 0xb418); /* 0x2076f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][3], 0x136e9); /* 0x2076f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][4], 0x14f1d); /* 0x2076f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][5], 0x30f63); /* 0x2076f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][6], 0x31e23); /* 0x2076f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][7], 0xa6d2); /* 0x2076f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][8], 0x10d84); /* 0x2076f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][9], 0x10716); /* 0x2076f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][10], 0x3a18d); /* 0x2076f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][11], 0xdff); /* 0x2076f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][12], 0xea22); /* 0x2076f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][13], 0x146e0); /* 0x2076f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][14], 0x19f8d); /* 0x2076f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][15], 0x1239a); /* 0x2076f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][16], 0x16758); /* 0x2076f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][17], 0x36308); /* 0x2076f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][18], 0x23f64); /* 0x2076f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][19], 0xbbde); /* 0x2076f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][20], 0x2d61a); /* 0x2076f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][21], 0x36bbd); /* 0x2076f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][22], 0x31e8d); /* 0x2076f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][23], 0x295c5); /* 0x2076f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][24], 0x10f3d); /* 0x2076f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][25], 0x27bc4); /* 0x2076f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][26], 0x1e943); /* 0x2076f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][27], 0x13671); /* 0x2076f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][28], 0xa489); /* 0x2076f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][29], 0x2e92d); /* 0x2076f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][30], 0x136a9); /* 0x2076f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][31], 0xe970); /* 0x2076f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][32], 0x1a9ee); /* 0x2076f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][33], 0x3e721); /* 0x2076f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][34], 0x2b559); /* 0x2076f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][35], 0x664a); /* 0x2076f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][36], 0x5e4d); /* 0x2076f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][37], 0x2a6d4); /* 0x2076f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][38], 0x1478f); /* 0x2076f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][39], 0x2163); /* 0x2076f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][40], 0x28771); /* 0x2076fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][41], 0x279b); /* 0x2076fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][42], 0x16f6d); /* 0x2076fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][43], 0x16aac); /* 0x2076fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][44], 0x8a71); /* 0x2076fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][45], 0x36efa); /* 0x2076fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][46], 0x24ed4); /* 0x2076fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][47], 0x207c2); /* 0x2076fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][48], 0x353d1); /* 0x2076fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][49], 0x2ef46); /* 0x2076fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][50], 0x1b3f6); /* 0x2076fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][51], 0xfad9); /* 0x2076fcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][0], 0x3d4b3); /* 0x2077000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][1], 0x36b6e); /* 0x2077004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][2], 0x245f4); /* 0x2077008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][3], 0x19cb4); /* 0x207700c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][4], 0x1e771); /* 0x2077010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][5], 0x22023); /* 0x2077014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][6], 0x4fd); /* 0x2077018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][7], 0x102ca); /* 0x207701c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][8], 0x1a76); /* 0x2077020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][9], 0x31a9c); /* 0x2077024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][10], 0x2983b); /* 0x2077028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][11], 0x35e7e); /* 0x207702c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][12], 0x20cb2); /* 0x2077030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][13], 0x31663); /* 0x2077034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][14], 0x2a29e); /* 0x2077038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][15], 0x4525); /* 0x207703c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][16], 0x2856d); /* 0x2077040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][17], 0x5123); /* 0x2077044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][18], 0x1cc3e); /* 0x2077048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][19], 0x9214); /* 0x207704c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][20], 0xe75d); /* 0x2077050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][21], 0x13f05); /* 0x2077054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][22], 0x3a43e); /* 0x2077058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][23], 0xd512); /* 0x207705c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][24], 0x2dcac); /* 0x2077060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][25], 0x3ffc3); /* 0x2077064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][26], 0x12587); /* 0x2077068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][27], 0x282a); /* 0x207706c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][28], 0x38b47); /* 0x2077070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][29], 0x221ac); /* 0x2077074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][30], 0x11900); /* 0x2077078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][31], 0x2f9e9); /* 0x207707c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][32], 0x3943b); /* 0x2077080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][33], 0x25840); /* 0x2077084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][34], 0x4ae6); /* 0x2077088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][35], 0x272f5); /* 0x207708c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][36], 0x19089); /* 0x2077090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][37], 0x3f020); /* 0x2077094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][38], 0x2255d); /* 0x2077098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][39], 0x246ea); /* 0x207709c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][40], 0x3097f); /* 0x20770a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][41], 0x17cd9); /* 0x20770a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][42], 0x3f8c5); /* 0x20770a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][43], 0x5b9f); /* 0x20770ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][44], 0x1da1a); /* 0x20770b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][45], 0xa955); /* 0x20770b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][46], 0x340e); /* 0x20770b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][47], 0x823f); /* 0x20770bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][48], 0x7968); /* 0x20770c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][49], 0x3eff3); /* 0x20770c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][50], 0x2aa67); /* 0x20770c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][51], 0x2c6b6); /* 0x20770cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][0], 0xfd0b); /* 0x2077100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][1], 0x30e96); /* 0x2077104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][2], 0x3f73); /* 0x2077108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][3], 0xbbb4); /* 0x207710c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][4], 0x14202); /* 0x2077110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][5], 0x47f2); /* 0x2077114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][6], 0x37d2f); /* 0x2077118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][7], 0x3b965); /* 0x207711c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][8], 0x6eb4); /* 0x2077120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][9], 0x32eda); /* 0x2077124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][10], 0x3bc51); /* 0x2077128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][11], 0x3073d); /* 0x207712c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][12], 0x64e1); /* 0x2077130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][13], 0x3ffe7); /* 0x2077134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][14], 0x3d66b); /* 0x2077138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][15], 0x3cb3e); /* 0x207713c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][16], 0x5e0c); /* 0x2077140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][17], 0xef7c); /* 0x2077144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][18], 0x234d1); /* 0x2077148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][19], 0x1f0b2); /* 0x207714c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][20], 0x3da20); /* 0x2077150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][21], 0x24fb2); /* 0x2077154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][22], 0x2d330); /* 0x2077158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][23], 0x312c1); /* 0x207715c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][24], 0x55d4); /* 0x2077160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][25], 0x25b0f); /* 0x2077164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][26], 0x29c46); /* 0x2077168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][27], 0x8840); /* 0x207716c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][28], 0x3bedb); /* 0x2077170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][29], 0x2b4f1); /* 0x2077174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][30], 0x2882); /* 0x2077178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][31], 0x29da6); /* 0x207717c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][32], 0x10670); /* 0x2077180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][33], 0x13d2f); /* 0x2077184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][34], 0x3b6e0); /* 0x2077188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][35], 0x3b1f6); /* 0x207718c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][36], 0x329a0); /* 0x2077190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][37], 0x34fb3); /* 0x2077194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][38], 0x1094); /* 0x2077198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][39], 0xc6c2); /* 0x207719c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][40], 0x39713); /* 0x20771a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][41], 0x1199f); /* 0x20771a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][42], 0x1848c); /* 0x20771a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][43], 0x1532a); /* 0x20771ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][44], 0x1f0e6); /* 0x20771b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][45], 0x107d5); /* 0x20771b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][46], 0x13e20); /* 0x20771b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][47], 0xa3da); /* 0x20771bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][48], 0xb1b0); /* 0x20771c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][49], 0x3cf98); /* 0x20771c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][50], 0x24782); /* 0x20771c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][51], 0x18c52); /* 0x20771cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][0], 0xca01); /* 0x2077200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][1], 0x1f59b); /* 0x2077204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][2], 0x1f0c2); /* 0x2077208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][3], 0x29b45); /* 0x207720c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][4], 0x1d796); /* 0x2077210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][5], 0x1a8d0); /* 0x2077214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][6], 0x1393f); /* 0x2077218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][7], 0x39e9c); /* 0x207721c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][8], 0x1b877); /* 0x2077220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][9], 0xe460); /* 0x2077224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][10], 0x1b0dc); /* 0x2077228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][11], 0x2f5e7); /* 0x207722c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][12], 0x267fe); /* 0x2077230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][13], 0x29483); /* 0x2077234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][14], 0x83df); /* 0x2077238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][15], 0x32896); /* 0x207723c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][16], 0x3cfe5); /* 0x2077240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][17], 0x2829c); /* 0x2077244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][18], 0x12a53); /* 0x2077248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][19], 0x35009); /* 0x207724c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][20], 0xfc50); /* 0x2077250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][21], 0x2561e); /* 0x2077254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][22], 0x1ef7f); /* 0x2077258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][23], 0x5b44); /* 0x207725c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][24], 0x4e42); /* 0x2077260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][25], 0x10394); /* 0x2077264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][26], 0x3408e); /* 0x2077268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][27], 0x17940); /* 0x207726c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][28], 0x3dc06); /* 0x2077270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][29], 0x1c9c6); /* 0x2077274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][30], 0x896b); /* 0x2077278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][31], 0x71d9); /* 0x207727c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][32], 0x385e5); /* 0x2077280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][33], 0x22320); /* 0x2077284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][34], 0xc6bb); /* 0x2077288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][35], 0x3839c); /* 0x207728c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][36], 0x37a52); /* 0x2077290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][37], 0x1463); /* 0x2077294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][38], 0x188c8); /* 0x2077298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][39], 0x3447c); /* 0x207729c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][40], 0x37053); /* 0x20772a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][41], 0x115f1); /* 0x20772a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][42], 0x2ee9c); /* 0x20772a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][43], 0x1ffa0); /* 0x20772ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][44], 0x30d21); /* 0x20772b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][45], 0x2b08e); /* 0x20772b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][46], 0x34ea0); /* 0x20772b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][47], 0x1a243); /* 0x20772bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][48], 0x2be2e); /* 0x20772c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][49], 0x247f9); /* 0x20772c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][50], 0x2dbf9); /* 0x20772c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][51], 0x3a7c8); /* 0x20772cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][0], 0x3a04c); /* 0x2077300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][1], 0x1c486); /* 0x2077304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][2], 0x14a58); /* 0x2077308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][3], 0x32683); /* 0x207730c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][4], 0x1b128); /* 0x2077310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][5], 0x36a45); /* 0x2077314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][6], 0x39541); /* 0x2077318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][7], 0x327e2); /* 0x207731c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][8], 0x7dd9); /* 0x2077320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][9], 0x54ca); /* 0x2077324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][10], 0x332ef); /* 0x2077328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][11], 0xf27e); /* 0x207732c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][12], 0x3dc27); /* 0x2077330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][13], 0x3935d); /* 0x2077334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][14], 0x51fa); /* 0x2077338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][15], 0xb84c); /* 0x207733c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][16], 0x16895); /* 0x2077340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][17], 0x35ca6); /* 0x2077344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][18], 0x1d8bc); /* 0x2077348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][19], 0x2b073); /* 0x207734c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][20], 0x190d); /* 0x2077350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][21], 0x14aee); /* 0x2077354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][22], 0x1192); /* 0x2077358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][23], 0x212ca); /* 0x207735c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][24], 0x22c01); /* 0x2077360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][25], 0x3c16); /* 0x2077364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][26], 0x650); /* 0x2077368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][27], 0x9a37); /* 0x207736c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][28], 0xa04b); /* 0x2077370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][29], 0x298d5); /* 0x2077374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][30], 0x2b6d); /* 0x2077378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][31], 0x36e54); /* 0x207737c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][32], 0x2d4cd); /* 0x2077380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][33], 0xc100); /* 0x2077384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][34], 0x23a3c); /* 0x2077388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][35], 0x37308); /* 0x207738c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][36], 0x1ee6f); /* 0x2077390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][37], 0x26d41); /* 0x2077394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][38], 0x1273); /* 0x2077398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][39], 0xc84d); /* 0x207739c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][40], 0x1fdf2); /* 0x20773a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][41], 0x35ee0); /* 0x20773a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][42], 0x3d071); /* 0x20773a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][43], 0x1f4c8); /* 0x20773ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][44], 0x39125); /* 0x20773b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][45], 0x37227); /* 0x20773b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][46], 0x366d0); /* 0x20773b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][47], 0x1a97b); /* 0x20773bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][48], 0xbbd); /* 0x20773c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][49], 0x2d2a9); /* 0x20773c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][50], 0x15ec9); /* 0x20773c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][51], 0xbebf); /* 0x20773cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][0], 0x20e32); /* 0x2077400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][1], 0x1e948); /* 0x2077404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][2], 0x17c1e); /* 0x2077408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][3], 0x2119c); /* 0x207740c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][4], 0x1bb84); /* 0x2077410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][5], 0x14963); /* 0x2077414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][6], 0x374d2); /* 0x2077418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][7], 0xa3f4); /* 0x207741c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][8], 0x3a0f1); /* 0x2077420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][9], 0x16f7); /* 0x2077424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][10], 0x9d2b); /* 0x2077428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][11], 0x36061); /* 0x207742c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][12], 0x345e2); /* 0x2077430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][13], 0x31704); /* 0x2077434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][14], 0x171e0); /* 0x2077438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][15], 0x32f37); /* 0x207743c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][16], 0x15923); /* 0x2077440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][17], 0x3f0cd); /* 0x2077444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][18], 0x38374); /* 0x2077448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][19], 0x33898); /* 0x207744c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][20], 0x79e7); /* 0x2077450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][21], 0x2418a); /* 0x2077454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][22], 0x1992a); /* 0x2077458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][23], 0x36877); /* 0x207745c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][24], 0xfb38); /* 0x2077460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][25], 0x13375); /* 0x2077464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][26], 0x3319b); /* 0x2077468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][27], 0x18607); /* 0x207746c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][28], 0x36306); /* 0x2077470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][29], 0x7a22); /* 0x2077474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][30], 0x2b68b); /* 0x2077478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][31], 0xb9f9); /* 0x207747c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][32], 0x1eb90); /* 0x2077480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][33], 0x3927a); /* 0x2077484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][34], 0x15dfa); /* 0x2077488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][35], 0x23911); /* 0x207748c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][36], 0x32fdb); /* 0x2077490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][37], 0x3c02f); /* 0x2077494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][38], 0x10f15); /* 0x2077498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][39], 0x179d1); /* 0x207749c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][40], 0x3a68); /* 0x20774a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][41], 0x21add); /* 0x20774a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][42], 0x122d1); /* 0x20774a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][43], 0x1549a); /* 0x20774ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][44], 0x7739); /* 0x20774b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][45], 0x76dd); /* 0x20774b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][46], 0x9caa); /* 0x20774b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][47], 0x1092f); /* 0x20774bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][48], 0x6df0); /* 0x20774c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][49], 0x36139); /* 0x20774c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][50], 0x5203); /* 0x20774c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][51], 0x1da08); /* 0x20774cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][0], 0x7f97); /* 0x2077500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][1], 0x20bbf); /* 0x2077504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][2], 0xd355); /* 0x2077508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][3], 0x26b26); /* 0x207750c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][4], 0x374); /* 0x2077510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][5], 0x6dfd); /* 0x2077514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][6], 0x30f9a); /* 0x2077518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][7], 0x283d6); /* 0x207751c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][8], 0x209c2); /* 0x2077520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][9], 0x12aa9); /* 0x2077524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][10], 0x1b250); /* 0x2077528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][11], 0x3db6c); /* 0x207752c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][12], 0x1bd99); /* 0x2077530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][13], 0x4fff); /* 0x2077534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][14], 0x22c7a); /* 0x2077538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][15], 0x2e544); /* 0x207753c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][16], 0x16aae); /* 0x2077540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][17], 0x8939); /* 0x2077544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][18], 0x1b485); /* 0x2077548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][19], 0x168cc); /* 0x207754c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][20], 0x2653a); /* 0x2077550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][21], 0x24df); /* 0x2077554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][22], 0x17df7); /* 0x2077558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][23], 0x3c67); /* 0x207755c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][24], 0x2fa61); /* 0x2077560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][25], 0x3071); /* 0x2077564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][26], 0x3d98); /* 0x2077568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][27], 0x11915); /* 0x207756c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][28], 0x17328); /* 0x2077570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][29], 0x27dbb); /* 0x2077574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][30], 0x16625); /* 0x2077578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][31], 0x19b43); /* 0x207757c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][32], 0x35783); /* 0x2077580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][33], 0x118c1); /* 0x2077584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][34], 0x153fa); /* 0x2077588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][35], 0x2f2c5); /* 0x207758c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][36], 0x8e30); /* 0x2077590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][37], 0x9dd8); /* 0x2077594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][38], 0x19fa5); /* 0x2077598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][39], 0x1af8e); /* 0x207759c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][40], 0x35e1b); /* 0x20775a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][41], 0x2d143); /* 0x20775a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][42], 0x170d5); /* 0x20775a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][43], 0x3b481); /* 0x20775ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][44], 0x1ddd3); /* 0x20775b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][45], 0x36c0d); /* 0x20775b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][46], 0x3a19); /* 0x20775b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][47], 0x2f87c); /* 0x20775bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][48], 0x285a3); /* 0x20775c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][49], 0x7fa5); /* 0x20775c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][50], 0x21b68); /* 0x20775c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][51], 0x1dce3); /* 0x20775cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][0], 0x2b256); /* 0x2077600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][1], 0x34544); /* 0x2077604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][2], 0x2cc96); /* 0x2077608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][3], 0xb22f); /* 0x207760c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][4], 0x21d54); /* 0x2077610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][5], 0x282ef); /* 0x2077614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][6], 0x3288e); /* 0x2077618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][7], 0x3d178); /* 0x207761c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][8], 0x376ab); /* 0x2077620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][9], 0x326f); /* 0x2077624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][10], 0x20c90); /* 0x2077628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][11], 0x3a049); /* 0x207762c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][12], 0x2a830); /* 0x2077630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][13], 0x3a833); /* 0x2077634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][14], 0x299e6); /* 0x2077638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][15], 0x11166); /* 0x207763c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][16], 0x3d9e8); /* 0x2077640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][17], 0x18104); /* 0x2077644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][18], 0x2a4a0); /* 0x2077648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][19], 0x3ef07); /* 0x207764c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][20], 0x3d1be); /* 0x2077650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][21], 0x6d39); /* 0x2077654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][22], 0x126c); /* 0x2077658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][23], 0x161da); /* 0x207765c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][24], 0x1fa09); /* 0x2077660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][25], 0x30c39); /* 0x2077664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][26], 0x1b0b6); /* 0x2077668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][27], 0x35b13); /* 0x207766c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][28], 0x16f3e); /* 0x2077670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][29], 0x2d965); /* 0x2077674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][30], 0xfa7c); /* 0x2077678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][31], 0x2a8df); /* 0x207767c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][32], 0x386f4); /* 0x2077680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][33], 0xeabb); /* 0x2077684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][34], 0xf1c9); /* 0x2077688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][35], 0x27e08); /* 0x207768c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][36], 0x25cee); /* 0x2077690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][37], 0x35b29); /* 0x2077694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][38], 0x1cdc0); /* 0x2077698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][39], 0x4187); /* 0x207769c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][40], 0x28397); /* 0x20776a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][41], 0x1a8); /* 0x20776a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][42], 0x8136); /* 0x20776a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][43], 0x388e5); /* 0x20776ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][44], 0x2d99b); /* 0x20776b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][45], 0x32778); /* 0x20776b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][46], 0x305af); /* 0x20776b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][47], 0xc965); /* 0x20776bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][48], 0x10d60); /* 0x20776c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][49], 0xe9cb); /* 0x20776c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][50], 0x15afd); /* 0x20776c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][51], 0x5843); /* 0x20776cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][0], 0xc1ed); /* 0x2077700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][1], 0x2fd42); /* 0x2077704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][2], 0x27d80); /* 0x2077708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][3], 0x835b); /* 0x207770c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][4], 0x14ffb); /* 0x2077710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][5], 0x3f67f); /* 0x2077714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][6], 0x3ed08); /* 0x2077718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][7], 0x3b7ef); /* 0x207771c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][8], 0x36c50); /* 0x2077720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][9], 0x48bd); /* 0x2077724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][10], 0x333f9); /* 0x2077728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][11], 0xd295); /* 0x207772c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][12], 0x3d458); /* 0x2077730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][13], 0x838a); /* 0x2077734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][14], 0x3e21d); /* 0x2077738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][15], 0x2895d); /* 0x207773c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][16], 0x13653); /* 0x2077740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][17], 0x2edc8); /* 0x2077744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][18], 0x12f50); /* 0x2077748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][19], 0x2f94c); /* 0x207774c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][20], 0x22362); /* 0x2077750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][21], 0x29e6); /* 0x2077754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][22], 0x4809); /* 0x2077758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][23], 0x1198c); /* 0x207775c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][24], 0x1a9d6); /* 0x2077760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][25], 0x22b08); /* 0x2077764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][26], 0x14c69); /* 0x2077768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][27], 0x1e8ee); /* 0x207776c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][28], 0x3c575); /* 0x2077770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][29], 0xe4f1); /* 0x2077774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][30], 0x3dc5d); /* 0x2077778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][31], 0x1a2d0); /* 0x207777c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][32], 0x1a939); /* 0x2077780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][33], 0x2ffe1); /* 0x2077784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][34], 0x3a42f); /* 0x2077788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][35], 0x171eb); /* 0x207778c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][36], 0x3adc6); /* 0x2077790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][37], 0x27c6f); /* 0x2077794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][38], 0x3cf80); /* 0x2077798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][39], 0x1ee5e); /* 0x207779c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][40], 0x12783); /* 0x20777a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][41], 0x3fe64); /* 0x20777a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][42], 0x33ac1); /* 0x20777a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][43], 0x3641f); /* 0x20777ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][44], 0x298da); /* 0x20777b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][45], 0x3d30a); /* 0x20777b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][46], 0x1f10e); /* 0x20777b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][47], 0x23eb); /* 0x20777bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][48], 0x124fb); /* 0x20777c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][49], 0xf36a); /* 0x20777c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][50], 0x3e6eb); /* 0x20777c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][51], 0xfae6); /* 0x20777cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][0], 0x2257); /* 0x2077800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][1], 0x259c1); /* 0x2077804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][2], 0x1fe7a); /* 0x2077808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][3], 0x3003f); /* 0x207780c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][4], 0x295c6); /* 0x2077810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][5], 0x355cf); /* 0x2077814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][6], 0x69de); /* 0x2077818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][7], 0xae12); /* 0x207781c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][8], 0x184aa); /* 0x2077820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][9], 0x2756); /* 0x2077824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][10], 0x123ba); /* 0x2077828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][11], 0x3fa5c); /* 0x207782c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][12], 0x31194); /* 0x2077830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][13], 0xaf40); /* 0x2077834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][14], 0xd3b8); /* 0x2077838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][15], 0x34a60); /* 0x207783c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][16], 0x152c6); /* 0x2077840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][17], 0x12352); /* 0x2077844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][18], 0xe1ce); /* 0x2077848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][19], 0x2d32); /* 0x207784c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][20], 0x26b63); /* 0x2077850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][21], 0x3c0a5); /* 0x2077854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][22], 0x3f3ce); /* 0x2077858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][23], 0x37ec5); /* 0x207785c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][24], 0xfede); /* 0x2077860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][25], 0x14b3a); /* 0x2077864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][26], 0x378fa); /* 0x2077868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][27], 0x19cc6); /* 0x207786c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][28], 0x2f319); /* 0x2077870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][29], 0x1c179); /* 0x2077874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][30], 0x1a5a5); /* 0x2077878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][31], 0x361f1); /* 0x207787c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][32], 0x292ec); /* 0x2077880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][33], 0xc2a9); /* 0x2077884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][34], 0x34ea9); /* 0x2077888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][35], 0x267b1); /* 0x207788c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][36], 0x3761a); /* 0x2077890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][37], 0x38eb8); /* 0x2077894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][38], 0xfe6b); /* 0x2077898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][39], 0x290cf); /* 0x207789c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][40], 0x10d5b); /* 0x20778a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][41], 0x1e3bb); /* 0x20778a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][42], 0x2e7e9); /* 0x20778a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][43], 0x1bc6); /* 0x20778ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][44], 0x375b6); /* 0x20778b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][45], 0x1e007); /* 0x20778b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][46], 0x220a2); /* 0x20778b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][47], 0x8d60); /* 0x20778bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][48], 0x1e1a6); /* 0x20778c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][49], 0x1f890); /* 0x20778c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][50], 0x2e846); /* 0x20778c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][51], 0x2007c); /* 0x20778cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][0], 0xcfc2); /* 0x2077900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][1], 0x33376); /* 0x2077904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][2], 0x2d913); /* 0x2077908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][3], 0xb6f); /* 0x207790c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][4], 0x25044); /* 0x2077910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][5], 0x15918); /* 0x2077914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][6], 0x21458); /* 0x2077918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][7], 0x3c133); /* 0x207791c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][8], 0x29ea8); /* 0x2077920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][9], 0x29133); /* 0x2077924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][10], 0x2a33e); /* 0x2077928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][11], 0x12e0a); /* 0x207792c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][12], 0x21f87); /* 0x2077930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][13], 0x1a5dd); /* 0x2077934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][14], 0x18f5b); /* 0x2077938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][15], 0x3d4c9); /* 0x207793c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][16], 0xb9bc); /* 0x2077940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][17], 0x1450b); /* 0x2077944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][18], 0x27394); /* 0x2077948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][19], 0x2dc5f); /* 0x207794c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][20], 0x2f5f2); /* 0x2077950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][21], 0x30b82); /* 0x2077954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][22], 0x17c8a); /* 0x2077958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][23], 0x24467); /* 0x207795c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][24], 0x3fe42); /* 0x2077960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][25], 0xe427); /* 0x2077964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][26], 0x10a32); /* 0x2077968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][27], 0x30d31); /* 0x207796c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][28], 0x8924); /* 0x2077970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][29], 0x3e1d3); /* 0x2077974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][30], 0x3260e); /* 0x2077978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][31], 0x12318); /* 0x207797c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][32], 0x2bc81); /* 0x2077980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][33], 0xad59); /* 0x2077984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][34], 0x2b6e); /* 0x2077988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][35], 0x1a7ab); /* 0x207798c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][36], 0x374aa); /* 0x2077990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][37], 0x2fdd); /* 0x2077994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][38], 0x28e42); /* 0x2077998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][39], 0x3ff00); /* 0x207799c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][40], 0x3ad78); /* 0x20779a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][41], 0x5d5d); /* 0x20779a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][42], 0x1bfe4); /* 0x20779a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][43], 0x888e); /* 0x20779ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][44], 0x4ca3); /* 0x20779b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][45], 0x1da8b); /* 0x20779b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][46], 0x1cbc8); /* 0x20779b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][47], 0x3a6a7); /* 0x20779bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][48], 0x167ab); /* 0x20779c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][49], 0xd36a); /* 0x20779c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][50], 0x3e744); /* 0x20779c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][51], 0x3fefc); /* 0x20779cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][0], 0x2b031); /* 0x2077a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][1], 0x1823e); /* 0x2077a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][2], 0x270aa); /* 0x2077a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][3], 0x361fa); /* 0x2077a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][4], 0x21fd2); /* 0x2077a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][5], 0x1750b); /* 0x2077a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][6], 0x861b); /* 0x2077a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][7], 0x1ede4); /* 0x2077a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][8], 0x59e5); /* 0x2077a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][9], 0x38b70); /* 0x2077a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][10], 0xf869); /* 0x2077a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][11], 0x13e81); /* 0x2077a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][12], 0x2a710); /* 0x2077a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][13], 0x322f1); /* 0x2077a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][14], 0x2382b); /* 0x2077a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][15], 0x7dcd); /* 0x2077a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][16], 0x36e39); /* 0x2077a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][17], 0x112ee); /* 0x2077a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][18], 0x16f25); /* 0x2077a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][19], 0x22926); /* 0x2077a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][20], 0x28a6b); /* 0x2077a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][21], 0x2701a); /* 0x2077a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][22], 0x2cd40); /* 0x2077a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][23], 0x1818e); /* 0x2077a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][24], 0x192ec); /* 0x2077a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][25], 0x74e2); /* 0x2077a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][26], 0x3df02); /* 0x2077a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][27], 0x1b6a9); /* 0x2077a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][28], 0x320db); /* 0x2077a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][29], 0x26f98); /* 0x2077a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][30], 0x2bb03); /* 0x2077a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][31], 0x3a600); /* 0x2077a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][32], 0x2b419); /* 0x2077a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][33], 0x6e19); /* 0x2077a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][34], 0x32490); /* 0x2077a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][35], 0x34fa2); /* 0x2077a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][36], 0x35cf8); /* 0x2077a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][37], 0x39eb1); /* 0x2077a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][38], 0x2a467); /* 0x2077a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][39], 0xa431); /* 0x2077a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][40], 0x3b4f1); /* 0x2077aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][41], 0x139ab); /* 0x2077aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][42], 0x1d6bd); /* 0x2077aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][43], 0xa63c); /* 0x2077aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][44], 0x23b96); /* 0x2077ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][45], 0x24558); /* 0x2077ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][46], 0x7131); /* 0x2077ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][47], 0x2012); /* 0x2077abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][48], 0x25b86); /* 0x2077ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][49], 0x12882); /* 0x2077ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][50], 0x27357); /* 0x2077ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][51], 0x26b94); /* 0x2077acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][0], 0x2d4ea); /* 0x2077b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][1], 0x3382b); /* 0x2077b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][2], 0x282c0); /* 0x2077b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][3], 0x205e5); /* 0x2077b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][4], 0x8516); /* 0x2077b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][5], 0x2c4fa); /* 0x2077b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][6], 0x8d18); /* 0x2077b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][7], 0x2e1e0); /* 0x2077b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][8], 0x2409a); /* 0x2077b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][9], 0x34384); /* 0x2077b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][10], 0x20e61); /* 0x2077b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][11], 0x188df); /* 0x2077b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][12], 0x1d2ac); /* 0x2077b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][13], 0x3d803); /* 0x2077b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][14], 0x1a594); /* 0x2077b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][15], 0xcd9f); /* 0x2077b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][16], 0x3d88c); /* 0x2077b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][17], 0x26dad); /* 0x2077b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][18], 0xec93); /* 0x2077b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][19], 0x13c7a); /* 0x2077b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][20], 0x14572); /* 0x2077b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][21], 0x14d83); /* 0x2077b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][22], 0x1717a); /* 0x2077b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][23], 0x43fe); /* 0x2077b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][24], 0x12e01); /* 0x2077b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][25], 0x10e63); /* 0x2077b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][26], 0x5036); /* 0x2077b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][27], 0x8b4c); /* 0x2077b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][28], 0x24789); /* 0x2077b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][29], 0x14e33); /* 0x2077b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][30], 0x3c031); /* 0x2077b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][31], 0x24bc3); /* 0x2077b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][32], 0x3bf15); /* 0x2077b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][33], 0x2dd7c); /* 0x2077b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][34], 0x33a39); /* 0x2077b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][35], 0x6dc0); /* 0x2077b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][36], 0x1487e); /* 0x2077b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][37], 0x3ac93); /* 0x2077b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][38], 0x21f23); /* 0x2077b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][39], 0x2d0a6); /* 0x2077b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][40], 0x8e21); /* 0x2077ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][41], 0x28500); /* 0x2077ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][42], 0x1cb12); /* 0x2077ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][43], 0x32b54); /* 0x2077bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][44], 0x1707a); /* 0x2077bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][45], 0x36883); /* 0x2077bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][46], 0x135a6); /* 0x2077bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][47], 0x3ffd5); /* 0x2077bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][48], 0x2b201); /* 0x2077bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][49], 0x36d78); /* 0x2077bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][50], 0x1d5e); /* 0x2077bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][51], 0x6627); /* 0x2077bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][0], 0x36d0f); /* 0x2077c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][1], 0x3431b); /* 0x2077c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][2], 0x3ae42); /* 0x2077c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][3], 0x22c59); /* 0x2077c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][4], 0x13710); /* 0x2077c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][5], 0x3b208); /* 0x2077c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][6], 0x2dcd9); /* 0x2077c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][7], 0x4bb0); /* 0x2077c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][8], 0x3c4b3); /* 0x2077c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][9], 0x128fa); /* 0x2077c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][10], 0x3d365); /* 0x2077c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][11], 0x387c0); /* 0x2077c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][12], 0x2ab2b); /* 0x2077c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][13], 0x11294); /* 0x2077c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][14], 0x157cb); /* 0x2077c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][15], 0x320cf); /* 0x2077c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][16], 0x137bd); /* 0x2077c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][17], 0x263e2); /* 0x2077c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][18], 0x34476); /* 0x2077c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][19], 0x35c3f); /* 0x2077c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][20], 0x194ab); /* 0x2077c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][21], 0x7c16); /* 0x2077c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][22], 0x372af); /* 0x2077c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][23], 0x1ed01); /* 0x2077c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][24], 0x2a035); /* 0x2077c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][25], 0x24f15); /* 0x2077c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][26], 0xc115); /* 0x2077c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][27], 0x1792c); /* 0x2077c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][28], 0x2c485); /* 0x2077c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][29], 0x15e9); /* 0x2077c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][30], 0x23d0f); /* 0x2077c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][31], 0x20816); /* 0x2077c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][32], 0x46b8); /* 0x2077c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][33], 0x84b7); /* 0x2077c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][34], 0x27d98); /* 0x2077c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][35], 0x3f854); /* 0x2077c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][36], 0x180f4); /* 0x2077c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][37], 0x17d51); /* 0x2077c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][38], 0x39330); /* 0x2077c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][39], 0x1e69f); /* 0x2077c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][40], 0x16a23); /* 0x2077ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][41], 0x2fed); /* 0x2077ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][42], 0x37606); /* 0x2077ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][43], 0x2fa93); /* 0x2077cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][44], 0x1e6fb); /* 0x2077cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][45], 0x34c10); /* 0x2077cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][46], 0x1d58f); /* 0x2077cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][47], 0x8448); /* 0x2077cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][48], 0x4ddf); /* 0x2077cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][49], 0x199e9); /* 0x2077cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][50], 0x87df); /* 0x2077cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][51], 0x22374); /* 0x2077ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][0], 0x74ae); /* 0x2077d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][1], 0x117ae); /* 0x2077d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][2], 0xe000); /* 0x2077d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][3], 0x2f9f); /* 0x2077d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][4], 0x1b59e); /* 0x2077d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][5], 0x29584); /* 0x2077d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][6], 0x15d95); /* 0x2077d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][7], 0x182a3); /* 0x2077d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][8], 0x1763); /* 0x2077d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][9], 0x3bf2e); /* 0x2077d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][10], 0x3d3d8); /* 0x2077d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][11], 0x25fc0); /* 0x2077d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][12], 0xecba); /* 0x2077d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][13], 0x3ae3e); /* 0x2077d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][14], 0x24dc3); /* 0x2077d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][15], 0x3d415); /* 0x2077d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][16], 0x31e1); /* 0x2077d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][17], 0x263af); /* 0x2077d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][18], 0x16293); /* 0x2077d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][19], 0x2f706); /* 0x2077d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][20], 0xcc07); /* 0x2077d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][21], 0x963a); /* 0x2077d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][22], 0xbcc0); /* 0x2077d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][23], 0x221c2); /* 0x2077d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][24], 0x3837a); /* 0x2077d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][25], 0x3815f); /* 0x2077d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][26], 0x166f1); /* 0x2077d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][27], 0x32549); /* 0x2077d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][28], 0x2e026); /* 0x2077d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][29], 0x1144b); /* 0x2077d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][30], 0x3c3fa); /* 0x2077d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][31], 0x18679); /* 0x2077d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][32], 0x34ed6); /* 0x2077d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][33], 0x674e); /* 0x2077d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][34], 0x38178); /* 0x2077d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][35], 0xf88); /* 0x2077d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][36], 0x3e55f); /* 0x2077d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][37], 0x7df8); /* 0x2077d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][38], 0x180b9); /* 0x2077d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][39], 0x317e1); /* 0x2077d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][40], 0x2572d); /* 0x2077da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][41], 0x4add); /* 0x2077da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][42], 0x91da); /* 0x2077da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][43], 0x33d0c); /* 0x2077dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][44], 0x35a5e); /* 0x2077db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][45], 0x7a1a); /* 0x2077db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][46], 0xb740); /* 0x2077db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][47], 0xe92f); /* 0x2077dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][48], 0x24f1d); /* 0x2077dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][49], 0x386ca); /* 0x2077dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][50], 0x17fda); /* 0x2077dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][51], 0x185dc); /* 0x2077dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][0], 0x2f131); /* 0x2077e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][1], 0x7389); /* 0x2077e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][2], 0xee6c); /* 0x2077e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][3], 0x11e5f); /* 0x2077e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][4], 0x25065); /* 0x2077e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][5], 0x22e9f); /* 0x2077e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][6], 0x328bb); /* 0x2077e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][7], 0x38fe2); /* 0x2077e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][8], 0x2a869); /* 0x2077e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][9], 0x14e40); /* 0x2077e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][10], 0x58bf); /* 0x2077e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][11], 0x370b4); /* 0x2077e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][12], 0x38448); /* 0x2077e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][13], 0x18389); /* 0x2077e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][14], 0x20db5); /* 0x2077e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][15], 0xbec3); /* 0x2077e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][16], 0x326d6); /* 0x2077e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][17], 0x262c6); /* 0x2077e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][18], 0x18d64); /* 0x2077e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][19], 0x4a02); /* 0x2077e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][20], 0x254a5); /* 0x2077e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][21], 0x1e6f1); /* 0x2077e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][22], 0x3e1ad); /* 0x2077e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][23], 0xfd82); /* 0x2077e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][24], 0x2456); /* 0x2077e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][25], 0x2b447); /* 0x2077e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][26], 0xf0f6); /* 0x2077e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][27], 0x34c84); /* 0x2077e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][28], 0x14bdd); /* 0x2077e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][29], 0x1947e); /* 0x2077e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][30], 0x2ec6); /* 0x2077e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][31], 0x16a45); /* 0x2077e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][32], 0x2277); /* 0x2077e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][33], 0x2310); /* 0x2077e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][34], 0x2cb28); /* 0x2077e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][35], 0x255f6); /* 0x2077e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][36], 0xf9ba); /* 0x2077e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][37], 0x2256a); /* 0x2077e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][38], 0x28737); /* 0x2077e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][39], 0x6747); /* 0x2077e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][40], 0x27e9a); /* 0x2077ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][41], 0x28bc0); /* 0x2077ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][42], 0x24326); /* 0x2077ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][43], 0x1077a); /* 0x2077eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][44], 0x27a8a); /* 0x2077eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][45], 0x9854); /* 0x2077eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][46], 0xefb1); /* 0x2077eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][47], 0x1de3b); /* 0x2077ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][48], 0xaaa2); /* 0x2077ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][49], 0x3d47); /* 0x2077ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][50], 0x25e5a); /* 0x2077ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][51], 0x3e6e0); /* 0x2077ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][0], 0x19e72); /* 0x2077f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][1], 0x3da38); /* 0x2077f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][2], 0x13440); /* 0x2077f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][3], 0x17f2a); /* 0x2077f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][4], 0xaabe); /* 0x2077f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][5], 0x11b0f); /* 0x2077f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][6], 0x3ca73); /* 0x2077f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][7], 0x26640); /* 0x2077f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][8], 0x228f); /* 0x2077f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][9], 0x18fc6); /* 0x2077f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][10], 0x1ade5); /* 0x2077f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][11], 0x184e1); /* 0x2077f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][12], 0x1fe4b); /* 0x2077f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][13], 0xa6e4); /* 0x2077f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][14], 0x2740a); /* 0x2077f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][15], 0x3dfc8); /* 0x2077f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][16], 0x4087); /* 0x2077f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][17], 0x31104); /* 0x2077f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][18], 0x38ad3); /* 0x2077f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][19], 0x8701); /* 0x2077f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][20], 0x2d393); /* 0x2077f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][21], 0xb8ce); /* 0x2077f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][22], 0x26d5c); /* 0x2077f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][23], 0x31641); /* 0x2077f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][24], 0x12c1f); /* 0x2077f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][25], 0x27b70); /* 0x2077f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][26], 0x33791); /* 0x2077f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][27], 0x995); /* 0x2077f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][28], 0x5311); /* 0x2077f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][29], 0x1544e); /* 0x2077f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][30], 0x24108); /* 0x2077f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][31], 0x18ab5); /* 0x2077f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][32], 0x1721e); /* 0x2077f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][33], 0x2a3dc); /* 0x2077f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][34], 0x1cd80); /* 0x2077f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][35], 0xb41); /* 0x2077f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][36], 0xdf90); /* 0x2077f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][37], 0x25ac5); /* 0x2077f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][38], 0x3b73a); /* 0x2077f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][39], 0x12e21); /* 0x2077f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][40], 0x31f9e); /* 0x2077fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][41], 0x3446b); /* 0x2077fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][42], 0x271a0); /* 0x2077fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][43], 0x16f21); /* 0x2077fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][44], 0x3951d); /* 0x2077fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][45], 0x616d); /* 0x2077fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][46], 0x2680a); /* 0x2077fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][47], 0x3d3d6); /* 0x2077fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][48], 0x29adc); /* 0x2077fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][49], 0x386d7); /* 0x2077fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][50], 0x7c85); /* 0x2077fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][51], 0x26497); /* 0x2077fcc */
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0], 0x8f); /* 0x2070060 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0], 0xc0ff); /* 0x2070060 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0][0], 0xff); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0][1], 0xc0); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1], 0xf3); /* 0x2070064 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1], 0xff0f); /* 0x2070064 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1][0], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1][1], 0xff); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2], 0x2); /* 0x2070068 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2], 0xc); /* 0x2070068 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2][0], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2][1], 0x0); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3], 0xff); /* 0x207006c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3], 0xffff); /* 0x207006c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3][0], 0xff); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3][1], 0xff); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4], 0x20); /* 0x2070070 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4], 0xc00); /* 0x2070070 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4][0], 0x0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4][1], 0xc); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5], 0x38); /* 0x2070074 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5], 0xfc0); /* 0x2070074 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5][0], 0xc0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5][1], 0xf); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6], 0xff); /* 0x2070078 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6], 0xffff); /* 0x2070078 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6][0], 0xff); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6][1], 0xff); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7], 0xe); /* 0x207007c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7], 0xfc); /* 0x207007c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7][0], 0xfc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7][1], 0x0); // regs_31841 fix
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[0][0], 0x239af08); /* 0x2070000 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[0][1], 0x3274b15); /* 0x2070004 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[1][0], 0x2aa591b); /* 0x2070008 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[1][1], 0xb2876b); /* 0x207000c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[2][0], 0x1edb780); /* 0x2070010 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[2][1], 0x13dd782); /* 0x2070014 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[3][0], 0x3262dbb); /* 0x2070018 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[3][1], 0xd61bd5); /* 0x207001c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[4][0], 0x292f904); /* 0x2070020 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[4][1], 0xbdf7bf); /* 0x2070024 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[5][0], 0x2c55332); /* 0x2070028 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[5][1], 0x10c7fb4); /* 0x207002c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[6][0], 0x2aeeae9); /* 0x2070030 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[6][1], 0x1846bc5); /* 0x2070034 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[7][0], 0x1667487); /* 0x2070038 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[7][1], 0x33eed5e); /* 0x207003c */
    //Renamed in bfnregs 20150107_182406_7982_mau_dev - comment out for now
    //tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hashout_paritycheck_enable, 0x68); /* 0x2070040 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[0], 0xca); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[1], 0xaa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[2], 0x90); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[3], 0x4b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[4], 0x2a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[5], 0x68); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[6], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[7], 0xcc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[8], 0x3f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[9], 0x65); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[10], 0x8d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[11], 0x5b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[12], 0xb6); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[13], 0xdd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[14], 0xf2); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[15], 0x55); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[16], 0x25); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[17], 0xda); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[18], 0xec); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[19], 0x47); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[20], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[21], 0xcf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[22], 0xa4); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[23], 0x76); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[24], 0x8c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[25], 0x7b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[26], 0x5b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[27], 0x96); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[28], 0xf9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[29], 0x92); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[30], 0xb9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[31], 0x32); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[32], 0xca); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[33], 0x7c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[34], 0xff); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[35], 0x7f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[36], 0xb6); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[37], 0xe9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[38], 0x3c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[39], 0xf0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[40], 0xf5); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[41], 0x96); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[42], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[43], 0x8b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[44], 0xfd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[45], 0xb4); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[46], 0x9e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[47], 0x97); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[48], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[49], 0x5a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[50], 0xe5); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[51], 0x81); // regs_31841 fix
    tu.IndirectWrite(0x020080100412, 0x0000175dff58ab55, 0x000008a200a600aa); /* TCAM[ 1][ 0][ 18].word1 = 0x5100530055  pt=b00 VV=b01 key=b00 py=0 mr=0 */
    //IndirectWrite 0x000008a200a600aa 0000175dff58ab54 to MauMemory (0x0000020080100412) offset 0x100412
    tu.IndirectWrite(0x020080005409, 0x18c7be9000000000, 0x0000000000000000); /* sram_ 1_ 5: a=0x20080005409 d0=0x18c7be9000000000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 18c7be9000000000 to MauMemory (0x0000020080005409) offset 0x5409
    tu.IndirectWrite(0x02008001ed8f, 0x9f4cfe62b8c568fc, 0x50ca839e4acbd5bc); /* sram_ 7_11: a=0x2008001ed8f d0=0x9f4cfe62b8c568fc d1=0x50ca839e4acbd5bc */
    //IndirectWrite 0x50ca839e4acbd5bc 9f4cfe62b8c568fc to MauMemory (0x000002008001ed8f) offset 0x1ed8f
    tu.IndirectWrite(0x0200800003a7, 0x0000000000000000, 0xf00000000000bbff); /* EM0_way0_wid0_dep0: a=0x200800003a7 d0=0x0 d1=0xf00000000000bbff */
    //IndirectWrite 0xf00000000000bbff 0000000000000000 to MauMemory (0x00000200800003a7) offset 0x3a7
    tu.IndirectWrite(0x020080100409, 0x00000af708060313, 0x00001508f7f930ec); /* TCAM[ 1][ 0][  9].word1 = 0x847bfc9876  pt=b00 VV=b10 key=b10 py=0 mr=0 */
    //IndirectWrite 0x00001508f7f930ec 00000af708060312 to MauMemory (0x0000020080100409) offset 0x100409
    tu.IndirectWrite(0x020080005404, 0x0000000000000000, 0x19a43ef000000000); /* sram_ 1_ 5: a=0x20080005404 d0=0x0 d1=0x19a43ef000000000 */
    //IndirectWrite 0x19a43ef000000000 0000000000000000 to MauMemory (0x0000020080005404) offset 0x5404
    tu.IndirectWrite(0x02008001ef48, 0x58f1788cab502acc, 0x7f2804a064dc3ad9); /* sram_ 7_11: a=0x2008001ef48 d0=0x58f1788cab502acc d1=0x7f2804a064dc3ad9 */
    //IndirectWrite 0x7f2804a064dc3ad9 58f1788cab502acc to MauMemory (0x000002008001ef48) offset 0x1ef48
    tu.IndirectWrite(0x02008000018d, 0x0000000000000000, 0xf00000000000bb00); /* EM0_way0_wid0_dep0: a=0x2008000018d d0=0x0 d1=0xf00000000000bb00 */
    //IndirectWrite 0xf00000000000bb00 0000000000000000 to MauMemory (0x000002008000018d) offset 0x18d
    tu.IndirectWrite(0x02008010040f, 0x00000a7032d82111, 0x0000158fcd270cee); /* TCAM[ 1][ 0][ 15].word1 = 0xc7e6938677  pt=b00 VV=b10 key=b10 py=0 mr=0 */
    //IndirectWrite 0x0000158fcd270cee 00000a7032d82110 to MauMemory (0x000002008010040f) offset 0x10040f
    tu.IndirectWrite(0x020080005407, 0x0000000000000000, 0x193e3db000000000); /* sram_ 1_ 5: a=0x20080005407 d0=0x0 d1=0x193e3db000000000 */
    //IndirectWrite 0x193e3db000000000 0000000000000000 to MauMemory (0x0000020080005407) offset 0x5407
    tu.IndirectWrite(0x02008001ee7c, 0xfc4f1412d5b8c70a, 0x4ec96fab5afc7477); /* sram_ 7_11: a=0x2008001ee7c d0=0xfc4f1412d5b8c70a d1=0x4ec96fab5afc7477 */
    //IndirectWrite 0x4ec96fab5afc7477 fc4f1412d5b8c70a to MauMemory (0x000002008001ee7c) offset 0x1ee7c
    tu.IndirectWrite(0x02008000023f, 0x0000000000000000, 0xf000000000000000); /* EM0_way0_wid0_dep0: a=0x2008000023f d0=0x0 d1=0xf000000000000000 */
    //IndirectWrite 0xf000000000000000 0000000000000000 to MauMemory (0x000002008000023f) offset 0x23f
    tu.IndirectWrite(0x02008010040e, 0x000018e37a8041c1, 0x0000071c857f0c3e); /* TCAM[ 1][ 0][ 14].word1 = 0x8e42bf861f  pt=b00 VV=b00 key=b11 py=0 mr=0 */
    //IndirectWrite 0x0000071c857f0c3e 000018e37a8041c0 to MauMemory (0x000002008010040e) offset 0x10040e
    tu.IndirectWrite(0x020080005407, 0x18ffbd7000000000, 0x193e3db000000000); /* sram_ 1_ 5: a=0x20080005407 d0=0x18ffbd7000000000 d1=0x193e3db000000000 */
    //IndirectWrite 0x193e3db000000000 18ffbd7000000000 to MauMemory (0x0000020080005407) offset 0x5407
    tu.IndirectWrite(0x02008001edff, 0xbfd62f94bf0cac4e, 0x32ca73339bdcef5b); /* sram_ 7_11: a=0x2008001edff d0=0xbfd62f94bf0cac4e d1=0x32ca73339bdcef5b */
    //IndirectWrite 0x32ca73339bdcef5b bfd62f94bf0cac4e to MauMemory (0x000002008001edff) offset 0x1edff
    tu.IndirectWrite(0x0200800000f2, 0x0000000000000000, 0xf00000000000bc02); /* EM0_way0_wid0_dep0: a=0x200800000f2 d0=0x0 d1=0xf00000000000bc02 */
    //IndirectWrite 0xf00000000000bc02 0000000000000000 to MauMemory (0x00000200800000f2) offset 0xf2
    tu.IndirectWrite(0x020080100411, 0x00001646e23213bf, 0x000009b91dccc040); /* TCAM[ 1][ 0][ 17].word1 = 0xdc8ee66020  pt=b00 VV=b01 key=b00 py=0 mr=0 */
    //IndirectWrite 0x000009b91dccc040 00001646e23213be to MauMemory (0x0000020080100411) offset 0x100411
    tu.IndirectWrite(0x020080005408, 0x0000000000000000, 0x184b3fc000000000); /* sram_ 1_ 5: a=0x20080005408 d0=0x0 d1=0x184b3fc000000000 */
    //IndirectWrite 0x184b3fc000000000 0000000000000000 to MauMemory (0x0000020080005408) offset 0x5408
    tu.IndirectWrite(0x02008001ec96, 0x235a6d508e0e7489, 0x85c6882c87bcdfc2); /* sram_ 7_11: a=0x2008001ec96 d0=0x235a6d508e0e7489 d1=0x85c6882c87bcdfc2 */
    //IndirectWrite 0x85c6882c87bcdfc2 235a6d508e0e7489 to MauMemory (0x000002008001ec96) offset 0x1ec96
    tu.IndirectWrite(0x020080100404, 0x000000029e5444bb, 0x00001ffd61ab0944); /* TCAM[ 1][ 0][  4].word1 = 0xfeb0d584a2  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    //IndirectWrite 0x00001ffd61ab0944 000000029e5444ba to MauMemory (0x0000020080100404) offset 0x100404
    tu.IndirectWrite(0x020080005402, 0x1866be9000000000, 0x0000000000000000); /* sram_ 1_ 5: a=0x20080005402 d0=0x1866be9000000000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 1866be9000000000 to MauMemory (0x0000020080005402) offset 0x5402
    tu.IndirectWrite(0x02008001eccd, 0x4e1b805c6033ccd7, 0xb34eb75f0d2c949b); /* sram_ 7_11: a=0x2008001eccd d0=0x4e1b805c6033ccd7 d1=0xb34eb75f0d2c949b */
    //IndirectWrite 0xb34eb75f0d2c949b 4e1b805c6033ccd7 to MauMemory (0x000002008001eccd) offset 0x1eccd
    tu.IndirectWrite(0x020080000322, 0x0000000000000000, 0xf000000000000000); /* EM0_way0_wid0_dep0: a=0x20080000322 d0=0x0 d1=0xf000000000000000 */
    //IndirectWrite 0xf000000000000000 0000000000000000 to MauMemory (0x0000020080000322) offset 0x322
    tu.IndirectWrite(0x020080100410, 0x000005b353808123, 0x00001a4cac7e32dc); /* TCAM[ 1][ 0][ 16].word1 = 0x26563f196e  pt=b00 VV=b11 key=b01 py=0 mr=0 */
    //IndirectWrite 0x00001a4cac7e32dc 000005b353808122 to MauMemory (0x0000020080100410) offset 0x100410
    tu.IndirectWrite(0x020080005408, 0x182ebd5000000000, 0x184b3fc000000000); /* sram_ 1_ 5: a=0x20080005408 d0=0x182ebd5000000000 d1=0x184b3fc000000000 */
    //IndirectWrite 0x184b3fc000000000 182ebd5000000000 to MauMemory (0x0000020080005408) offset 0x5408
    tu.IndirectWrite(0x02008001ec5d, 0x4e81ea72c27ff2a9, 0x8d062907571cdd2c); /* sram_ 7_11: a=0x2008001ec5d d0=0x4e81ea72c27ff2a9 d1=0x8d062907571cdd2c */
    //IndirectWrite 0x8d062907571cdd2c 4e81ea72c27ff2a9 to MauMemory (0x000002008001ec5d) offset 0x1ec5d
    tu.IndirectWrite(0x02008010040a, 0x000009bb12fe1383, 0x00001644ed01407c); /* TCAM[ 1][ 0][ 10].word1 = 0x227680a03e  pt=b00 VV=b10 key=b11 py=0 mr=0 */
    //IndirectWrite 0x00001644ed01407c 000009bb12fe1382 to MauMemory (0x000002008010040a) offset 0x10040a
    tu.IndirectWrite(0x020080005405, 0x196e3ed000000000, 0x0000000000000000); /* sram_ 1_ 5: a=0x20080005405 d0=0x196e3ed000000000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 196e3ed000000000 to MauMemory (0x0000020080005405) offset 0x5405
    tu.IndirectWrite(0x02008001eedc, 0x115ed137a5cb8c80, 0x281c3b9f22709d6f); /* sram_ 7_11: a=0x2008001eedc d0=0x115ed137a5cb8c80 d1=0x281c3b9f22709d6f */
    //IndirectWrite 0x281c3b9f22709d6f 115ed137a5cb8c80 to MauMemory (0x000002008001eedc) offset 0x1eedc
    tu.IndirectWrite(0x0200801005fb, 0x0000185bb89a8963, 0x000007a44764449c); /* TCAM[ 1][ 0][507].word1 = 0xd223b2224e  pt=b00 VV=b00 key=b11 py=0 mr=0 */
    //IndirectWrite 0x000007a44764449c 0000185bb89a8962 to MauMemory (0x00000200801005fb) offset 0x1005fb
    tu.IndirectWrite(0x0200800054fd, 0x0000000000000000, 0x198d3e7000000000); /* sram_ 1_ 5: a=0x200800054fd d0=0x0 d1=0x198d3e7000000000 */
    //IndirectWrite 0x198d3e7000000000 0000000000000000 to MauMemory (0x00000200800054fd) offset 0x54fd
    tu.IndirectWrite(0x02008001ef1a, 0x5bd3498ece3ed0eb, 0xbdcfeed99fa3fd98); /* sram_ 7_11: a=0x2008001ef1a d0=0x5bd3498ece3ed0eb d1=0xbdcfeed99fa3fd98 */
    //IndirectWrite 0xbdcfeed99fa3fd98 5bd3498ece3ed0eb to MauMemory (0x000002008001ef1a) offset 0x1ef1a
    tu.IndirectWrite(0x020080100406, 0x00001094101108b7, 0x00000f6befee2348); /* TCAM[ 1][ 0][  6].word1 = 0xb5f7f711a4  pt=b00 VV=b01 key=b11 py=0 mr=0 */
    //IndirectWrite 0x00000f6befee2348 00001094101108b6 to MauMemory (0x0000020080100406) offset 0x100406
    tu.IndirectWrite(0x020080005403, 0x18ebbdf000000000, 0x0000000000000000); /* sram_ 1_ 5: a=0x20080005403 d0=0x18ebbdf000000000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 18ebbdf000000000 to MauMemory (0x0000020080005403) offset 0x5403
    tu.IndirectWrite(0x02008001edd7, 0xd0a2486f532ae678, 0xcc54f0c7e188cc52); /* sram_ 7_11: a=0x2008001edd7 d0=0xd0a2486f532ae678 d1=0xcc54f0c7e188cc52 */
    //IndirectWrite 0xcc54f0c7e188cc52 d0a2486f532ae678 to MauMemory (0x000002008001edd7) offset 0x1edd7
    tu.IndirectWrite(0x020080100407, 0x00001549ff44ab41, 0x00000ab600ba00be); /* TCAM[ 1][ 0][  7].word1 = 0x5b005d005f  pt=b00 VV=b01 key=b01 py=0 mr=0 */
    //IndirectWrite 0x00000ab600ba00be 00001549ff44ab40 to MauMemory (0x0000020080100407) offset 0x100407
    tu.IndirectWrite(0x020080005403, 0x18ebbdf000000000, 0x19b4bde000000000); /* sram_ 1_ 5: a=0x20080005403 d0=0x18ebbdf000000000 d1=0x19b4bde000000000 */
    //IndirectWrite 0x19b4bde000000000 18ebbdf000000000 to MauMemory (0x0000020080005403) offset 0x5403
    tu.IndirectWrite(0x02008001ef69, 0xe043c2bb487829df, 0x8928245a21031c69); /* sram_ 7_11: a=0x2008001ef69 d0=0xe043c2bb487829df d1=0x8928245a21031c69 */
    //IndirectWrite 0x8928245a21031c69 e043c2bb487829df to MauMemory (0x000002008001ef69) offset 0x1ef69
    tu.IndirectWrite(0x020080100408, 0x000013584b2952bb, 0x00000ca7b4d60144); /* TCAM[ 1][ 0][  8].word1 = 0x53da6b00a2  pt=b00 VV=b01 key=b10 py=0 mr=0 */
    //IndirectWrite 0x00000ca7b4d60144 000013584b2952ba to MauMemory (0x0000020080100408) offset 0x100408
    tu.IndirectWrite(0x020080005404, 0x1951bdd000000000, 0x19a43ef000000000); /* sram_ 1_ 5: a=0x20080005404 d0=0x1951bdd000000000 d1=0x19a43ef000000000 */
    //IndirectWrite 0x19a43ef000000000 1951bdd000000000 to MauMemory (0x0000020080005404) offset 0x5404
    tu.IndirectWrite(0x02008001eea3, 0x08e8d41407b011c8, 0x45004a51cf36bf2c); /* sram_ 7_11: a=0x2008001eea3 d0=0x8e8d41407b011c8 d1=0x45004a51cf36bf2c */
    //IndirectWrite 0x45004a51cf36bf2c 08e8d41407b011c8 to MauMemory (0x000002008001eea3) offset 0x1eea3
    tu.IndirectWrite(0x020080000321, 0x0000000000000000, 0xf00000000000bc0a); /* EM0_way0_wid0_dep0: a=0x20080000321 d0=0x0 d1=0xf00000000000bc0a */
    //IndirectWrite 0xf00000000000bc0a 0000000000000000 to MauMemory (0x0000020080000321) offset 0x321
    tu.IndirectWrite(0x020080100405, 0x0000101fc0b15245, 0x00000fe03f4e01ba); /* TCAM[ 1][ 0][  5].word1 = 0xf01fa700dd  pt=b00 VV=b01 key=b11 py=0 mr=0 */
    //IndirectWrite 0x00000fe03f4e01ba 0000101fc0b15244 to MauMemory (0x0000020080100405) offset 0x100405
    tu.IndirectWrite(0x020080005402, 0x1866be9000000000, 0x1934bc2000000000); /* sram_ 1_ 5: a=0x20080005402 d0=0x1866be9000000000 d1=0x1934bc2000000000 */
    //IndirectWrite 0x1934bc2000000000 1866be9000000000 to MauMemory (0x0000020080005402) offset 0x5402
    tu.IndirectWrite(0x02008001ee69, 0xc4272a3507b44724, 0x107fb571806e1d95); /* sram_ 7_11: a=0x2008001ee69 d0=0xc4272a3507b44724 d1=0x107fb571806e1d95 */
    //IndirectWrite 0x107fb571806e1d95 c4272a3507b44724 to MauMemory (0x000002008001ee69) offset 0x1ee69
    tu.IndirectWrite(0x020080000136, 0x0000000000000000, 0xf00000000000000b); /* EM0_way0_wid0_dep0: a=0x20080000136 d0=0x0 d1=0xf00000000000000b */
    //IndirectWrite 0xf00000000000000b 0000000000000000 to MauMemory (0x0000020080000136) offset 0x136

    


    Phv *phv_in2 = tu.phv_alloc();
    phv_in2->set_ingress();

    phv_in2->set(  0, 0x80c7bc07); 	/* [0, 0] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(  3, 0x80c7bc04); 	/* [0, 3] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(  4, 0x80c7bc03); 	/* [0, 4] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(  5, 0x80c7bc02); 	/* [0, 5] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(  6, 0x80c7bc01); 	/* [0, 6] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(  7, 0x80c7bc00); 	/* [0, 7] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(  8, 0x80c7bbff); 	/* [0, 8] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(  9, 0x80c7bbfe); 	/* [0, 9] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 10, 0x80c7bbfd); 	/* [0,10] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 11, 0x80c7bbfc); 	/* [0,11] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 12, 0x80c7bbfb); 	/* [0,12] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 13, 0x80c7bbfa); 	/* [0,13] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 14, 0x80c7bbf9); 	/* [0,14] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 15, 0x80c7bbf8); 	/* [0,15] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 16, 0x80c7bbf7); 	/* [0,16] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 17, 0x80c7bbf6); 	/* [0,17] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 18, 0x80c7bbf5); 	/* [0,18] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 19, 0x80c7bbf4); 	/* [0,19] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 20, 0x80c7bbf3); 	/* [0,20] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 21, 0x80c7bbf2); 	/* [0,21] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 22, 0x80c7bbf1); 	/* [0,22] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 23, 0x80c7bbf0); 	/* [0,23] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 25, 0x80c7bbee); 	/* [0,25] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 26, 0x80c7bbed); 	/* [0,26] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 27, 0x80c7bbec); 	/* [0,27] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 29, 0x80c7bbea); 	/* [0,29] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 30, 0x80c7bbe9); 	/* [0,30] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 31, 0x80c7bbe8); 	/* [0,31] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 32, 0x80c7bbe7); 	/* [1, 0] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 33, 0x80c7bbe6); 	/* [1, 1] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 35, 0x80c7bbe4); 	/* [1, 3] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 37, 0x80c7bbe2); 	/* [1, 5] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 38, 0x80c7bbe1); 	/* [1, 6] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 42, 0x80c7bbdd); 	/* [1,10] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 43, 0x80c7bbdc); 	/* [1,11] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 44, 0x80c7bbdb); 	/* [1,12] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 45, 0x80c7bbda); 	/* [1,13] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 47, 0x80c7bbd8); 	/* [1,15] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 48, 0x80c7bbd7); 	/* [1,16] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 49, 0x80c7bbd6); 	/* [1,17] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 50, 0x80c7bbd5); 	/* [1,18] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 53, 0x80c7bbd2); 	/* [1,21] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 54, 0x80c7bbd1); 	/* [1,22] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 55, 0x80c7bbd0); 	/* [1,23] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 56, 0x80c7bbcf); 	/* [1,24] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 57, 0x80c7bbce); 	/* [1,25] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 58, 0x80c7bbcd); 	/* [1,26] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 59, 0x80c7bbcc); 	/* [1,27] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 60, 0x80c7bbcb); 	/* [1,28] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 61, 0x80c7bbca); 	/* [1,29] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 62, 0x80c7bbc9); 	/* [1,30] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 63, 0x80c7bbc8); 	/* [1,31] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 64, 0xc7); 	/* [2, 0] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 65, 0xc6); 	/* [2, 1] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 66, 0xc5); 	/* [2, 2] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 67, 0xc4); 	/* [2, 3] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 68, 0xc3); 	/* [2, 4] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 69, 0xc2); 	/* [2, 5] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 70, 0xc1); 	/* [2, 6] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 71, 0xc0); 	/* [2, 7] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 72, 0xbf); 	/* [2, 8] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 73, 0xbe); 	/* [2, 9] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 74, 0xbd); 	/* [2,10] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 75, 0xbc); 	/* [2,11] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 76, 0xbb); 	/* [2,12] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 77, 0xba); 	/* [2,13] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 78, 0xb9); 	/* [2,14] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 79, 0xb8); 	/* [2,15] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 80, 0xb7); 	/* [2,16] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 81, 0xb6); 	/* [2,17] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 83, 0xb4); 	/* [2,19] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 84, 0xb3); 	/* [2,20] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 85, 0xb2); 	/* [2,21] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 86, 0xb1); 	/* [2,22] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 87, 0xb0); 	/* [2,23] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 88, 0xaf); 	/* [2,24] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 89, 0xae); 	/* [2,25] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 90, 0xad); 	/* [2,26] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 91, 0xac); 	/* [2,27] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 92, 0xab); 	/* [2,28] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 93, 0xaa); 	/* [2,29] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 94, 0xa9); 	/* [2,30] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 96, 0xa7); 	/* [3, 0] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 98, 0xa5); 	/* [3, 2] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set( 99, 0xa4); 	/* [3, 3] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(101, 0xa2); 	/* [3, 5] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(102, 0xa1); 	/* [3, 6] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(103, 0xa0); 	/* [3, 7] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(104, 0x9f); 	/* [3, 8] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(105, 0x9e); 	/* [3, 9] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(106, 0x9d); 	/* [3,10] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(107, 0x9c); 	/* [3,11] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(108, 0x9b); 	/* [3,12] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(110, 0x99); 	/* [3,14] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(111, 0x98); 	/* [3,15] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(112, 0x97); 	/* [3,16] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(113, 0x96); 	/* [3,17] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(114, 0x95); 	/* [3,18] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(115, 0x94); 	/* [3,19] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(116, 0x93); 	/* [3,20] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(117, 0x92); 	/* [3,21] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(120, 0x8f); 	/* [3,24] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(121, 0x8e); 	/* [3,25] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(122, 0x8d); 	/* [3,26] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(123, 0x8c); 	/* [3,27] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(124, 0x8b); 	/* [3,28] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(125, 0x8a); 	/* [3,29] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(126, 0x89); 	/* [3,30] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(127, 0x88); 	/* [3,31] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(128, 0xff87); 	/* [4, 0] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(129, 0xff86); 	/* [4, 1] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(131, 0xff84); 	/* [4, 3] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(132, 0xff83); 	/* [4, 4] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(133, 0xff82); 	/* [4, 5] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(134, 0xff81); 	/* [4, 6] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(135, 0xff80); 	/* [4, 7] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(136, 0x007f); 	/* [4, 8] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(137, 0x007e); 	/* [4, 9] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(138, 0x007d); 	/* [4,10] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(139, 0x007c); 	/* [4,11] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(140, 0x007b); 	/* [4,12] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(142, 0x0079); 	/* [4,14] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(143, 0x0078); 	/* [4,15] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(144, 0x0077); 	/* [4,16] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(145, 0x0076); 	/* [4,17] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(146, 0x0075); 	/* [4,18] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(147, 0x0074); 	/* [4,19] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(148, 0x0073); 	/* [4,20] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(149, 0x0072); 	/* [4,21] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(150, 0x0071); 	/* [4,22] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(151, 0x0070); 	/* [4,23] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(152, 0x006f); 	/* [4,24] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(153, 0x006e); 	/* [4,25] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(154, 0x006d); 	/* [4,26] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(155, 0x006c); 	/* [4,27] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(156, 0x006b); 	/* [4,28] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(157, 0x006a); 	/* [4,29] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(158, 0x0069); 	/* [4,30] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(159, 0x0068); 	/* [4,31] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(160, 0x0067); 	/* [5, 0] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(161, 0x0066); 	/* [5, 1] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(162, 0x0065); 	/* [5, 2] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(163, 0x0064); 	/* [5, 3] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(164, 0x0063); 	/* [5, 4] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(165, 0x0062); 	/* [5, 5] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(166, 0x0061); 	/* [5, 6] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(167, 0x0060); 	/* [5, 7] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(168, 0x005f); 	/* [5, 8] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(169, 0x005e); 	/* [5, 9] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(170, 0x005d); 	/* [5,10] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(171, 0x005c); 	/* [5,11] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(172, 0x005b); 	/* [5,12] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(173, 0x005a); 	/* [5,13] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(174, 0x0059); 	/* [5,14] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(176, 0x0057); 	/* [5,16] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(177, 0x0056); 	/* [5,17] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(178, 0x0055); 	/* [5,18] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(179, 0x0054); 	/* [5,19] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(180, 0x0053); 	/* [5,20] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(181, 0x0052); 	/* [5,21] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(182, 0x0051); 	/* [5,22] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(183, 0x0050); 	/* [5,23] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(184, 0x004f); 	/* [5,24] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(185, 0x004e); 	/* [5,25] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(186, 0x004d); 	/* [5,26] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(187, 0x004c); 	/* [5,27] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(188, 0x004b); 	/* [5,28] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(189, 0x004a); 	/* [5,29] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(190, 0x0049); 	/* [5,30] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(191, 0x0048); 	/* [5,31] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(192, 0x0047); 	/* [6, 0] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(193, 0x0046); 	/* [6, 1] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(195, 0x0044); 	/* [6, 3] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(196, 0x0043); 	/* [6, 4] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(197, 0x0042); 	/* [6, 5] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(198, 0x0041); 	/* [6, 6] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(199, 0x0040); 	/* [6, 7] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(200, 0x003f); 	/* [6, 8] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(201, 0x003e); 	/* [6, 9] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(202, 0x003d); 	/* [6,10] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(203, 0x003c); 	/* [6,11] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(204, 0x003b); 	/* [6,12] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(205, 0x003a); 	/* [6,13] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(206, 0x0039); 	/* [6,14] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(207, 0x0038); 	/* [6,15] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(208, 0x0037); 	/* [6,16] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(209, 0x0036); 	/* [6,17] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(210, 0x0035); 	/* [6,18] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(211, 0x0034); 	/* [6,19] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(212, 0x0033); 	/* [6,20] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(213, 0x0032); 	/* [6,21] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(215, 0x0030); 	/* [6,23] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(216, 0x002f); 	/* [6,24] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(217, 0x002e); 	/* [6,25] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(218, 0x002d); 	/* [6,26] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(219, 0x002c); 	/* [6,27] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(220, 0x002b); 	/* [6,28] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(222, 0x0029); 	/* [6,30] v=1  #1e0# RefModel iPhv 2o */
    phv_in2->set(223, 0x0028); 	/* [6,31] v=1  #1e0# RefModel iPhv 2o */

    


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
    RMT_UT_LOG_INFO("Dv19Test::InPkt=%p [DA=%04X%08X]\n", p_in,
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


    Phv *phv_out2 = tu.port_process_inbound(port, phv_in2);    
    //EXPECT_EQ(0x94, phv_out2->get(111)); // was 0x98 in input phv

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
    //EXPECT_EQ(64, phv_out2->get(64));
    //EXPECT_EQ(96, phv_out2->get(96));
    //EXPECT_EQ(128, phv_out2->get(128));
    
    // Free PHVs

    tu.finish_test();
    tu.quieten_log_flags();
}


}
