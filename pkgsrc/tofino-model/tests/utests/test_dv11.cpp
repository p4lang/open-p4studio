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

// XXX -> test_dv11.cpp
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

  bool dv11_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv11Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv11_print) RMT_UT_LOG_INFO("test_dv11_packet1()\n");
    
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
    tu.set_dv_test(11);
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
    //tu.OutWord(&mau_reg_map.dp.final_output_delay[0][0], 0x12); /* 0x2060040 */
    tu.OutWord(&mau_reg_map.dp.pipelength_added_stages[0], 0x1); /* 0x2060070 */
    tu.OutWord(&mau_reg_map.dp.action_output_delay[1], 0xb); /* 0x2060038 */
    //Removed in bfnregs 20150107_182406_7982_mau_dev
    //tu.OutWord(&mau_reg_map.dp.final_output_delay[1][0], 0x16); /* 0x2060048 */
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
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_mode[15], 0x6c95000); /* 0x20407fc */
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_table_map[0], 0x8000); /* 0x2040780 */
    tu.OutWord(&mau_reg_map.tcams.tcam_match_adr_shift[0], 0x4); /* 0x2040420 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.unitram_config[0][1], 0x344e); /* 0x201cb88 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.unitram_config[0][5], 0x351a); /* 0x201eba8 */
    tu.OutWord(&mau_reg_map.rams.array.row[1].ram[1].unit_ram_ctl, 0x200); /* 0x2009098 */
    tu.OutWord(&mau_reg_map.rams.array.row[5].ram[5].unit_ram_ctl, 0x20); /* 0x200d298 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tind_ram_data_size[2], 0x5); /* 0x2016508 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][0], 0x14); /* 0x2040380 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][0], 0x1); /* 0x2040200 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][1], 0x12); /* 0x2040384 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][1], 0x3); /* 0x2040204 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][0], 0x2); /* 0x2040300 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][0], 0x2); /* 0x2040000 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][1], 0x5); /* 0x2040004 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][2], 0x3); /* 0x2040008 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][3], 0x7); /* 0x204000c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][4], 0x3); /* 0x2040010 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][5], 0x7); /* 0x2040014 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][6], 0x9); /* 0x2040018 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][7], 0x5); /* 0x204001c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][0], 0x3); /* 0x20403c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][0], 0x2); /* 0x2040240 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][1], 0x1); /* 0x20403c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][1], 0x2); /* 0x2040244 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][0], 0xf); /* 0x2040320 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][0], 0x2); /* 0x2040100 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][1], 0x3); /* 0x2040104 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][2], 0x4); /* 0x2040108 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][3], 0xa); /* 0x204010c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][4], 0xa); /* 0x2040110 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][5], 0x8); /* 0x2040114 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][6], 0x6); /* 0x2040118 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][7], 0xa); /* 0x204011c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][2], 0xa); /* 0x2040388 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][2], 0x0); /* 0x2040208 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][3], 0xa); /* 0x204038c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][3], 0x1); /* 0x204020c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][1], 0x4); /* 0x2040304 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][0], 0x5); /* 0x2040020 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][1], 0x3); /* 0x2040024 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][2], 0x9); /* 0x2040028 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][3], 0x5); /* 0x204002c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][4], 0x8); /* 0x2040030 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][5], 0xa); /* 0x2040034 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][6], 0xa); /* 0x2040038 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][7], 0x3); /* 0x204003c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][2], 0x4); /* 0x20403c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][2], 0x1); /* 0x2040248 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][3], 0x14); /* 0x20403cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][3], 0x0); /* 0x204024c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][1], 0x1); /* 0x2040324 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][0], 0x9); /* 0x2040120 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][1], 0xa); /* 0x2040124 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][2], 0x9); /* 0x2040128 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][3], 0x0); /* 0x204012c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][4], 0xa); /* 0x2040130 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][5], 0x7); /* 0x2040134 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][6], 0x9); /* 0x2040138 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][7], 0x5); /* 0x204013c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][4], 0x1c); /* 0x2040390 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][4], 0x2); /* 0x2040210 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][5], 0x19); /* 0x2040394 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][5], 0x2); /* 0x2040214 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][2], 0xf); /* 0x2040308 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][0], 0x2); /* 0x2040040 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][1], 0x1); /* 0x2040044 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][2], 0x7); /* 0x2040048 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][3], 0x0); /* 0x204004c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][4], 0x7); /* 0x2040050 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][5], 0x2); /* 0x2040054 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][6], 0x8); /* 0x2040058 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][7], 0x7); /* 0x204005c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][4], 0x18); /* 0x20403d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][4], 0x1); /* 0x2040250 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][5], 0x4); /* 0x20403d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][5], 0x1); /* 0x2040254 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][2], 0xb); /* 0x2040328 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][0], 0x0); /* 0x2040140 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][1], 0x1); /* 0x2040144 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][2], 0xa); /* 0x2040148 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][3], 0xa); /* 0x204014c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][4], 0x6); /* 0x2040150 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][5], 0x6); /* 0x2040154 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][6], 0xa); /* 0x2040158 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][7], 0xa); /* 0x204015c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][6], 0x18); /* 0x2040398 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][6], 0x0); /* 0x2040218 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][7], 0x10); /* 0x204039c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][7], 0x3); /* 0x204021c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][3], 0xd); /* 0x204030c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][0], 0x1); /* 0x2040060 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][1], 0x3); /* 0x2040064 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][2], 0x9); /* 0x2040068 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][3], 0x3); /* 0x204006c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][4], 0x6); /* 0x2040070 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][5], 0x9); /* 0x2040074 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][6], 0x2); /* 0x2040078 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][7], 0x4); /* 0x204007c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][6], 0x11); /* 0x20403d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][6], 0x0); /* 0x2040258 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][7], 0x13); /* 0x20403dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][7], 0x1); /* 0x204025c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][3], 0xd); /* 0x204032c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][0], 0x6); /* 0x2040160 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][1], 0x6); /* 0x2040164 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][2], 0x3); /* 0x2040168 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][3], 0x1); /* 0x204016c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][4], 0x6); /* 0x2040170 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][5], 0x5); /* 0x2040174 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][6], 0x7); /* 0x2040178 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][7], 0x3); /* 0x204017c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][8], 0x15); /* 0x20403a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][8], 0x3); /* 0x2040220 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][9], 0x1); /* 0x20403a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][9], 0x0); /* 0x2040224 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][4], 0xe); /* 0x2040310 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][0], 0x3); /* 0x2040080 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][1], 0x0); /* 0x2040084 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][2], 0x7); /* 0x2040088 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][3], 0x4); /* 0x204008c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][4], 0x5); /* 0x2040090 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][5], 0x0); /* 0x2040094 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][6], 0x4); /* 0x2040098 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][7], 0x8); /* 0x204009c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][8], 0x16); /* 0x20403e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][8], 0x1); /* 0x2040260 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][9], 0x0); /* 0x20403e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][9], 0x0); /* 0x2040264 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][4], 0xa); /* 0x2040330 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][0], 0x8); /* 0x2040180 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][1], 0xa); /* 0x2040184 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][2], 0x9); /* 0x2040188 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][3], 0x3); /* 0x204018c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][4], 0x8); /* 0x2040190 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][5], 0x4); /* 0x2040194 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][6], 0x5); /* 0x2040198 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][7], 0xa); /* 0x204019c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][10], 0x1a); /* 0x20403a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][10], 0x3); /* 0x2040228 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][11], 0xb); /* 0x20403ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][11], 0x0); /* 0x204022c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][5], 0x3); /* 0x2040314 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][0], 0x9); /* 0x20400a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][1], 0x3); /* 0x20400a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][2], 0x7); /* 0x20400a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][3], 0x8); /* 0x20400ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][4], 0x7); /* 0x20400b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][5], 0x4); /* 0x20400b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][6], 0x8); /* 0x20400b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][7], 0x7); /* 0x20400bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][10], 0xe); /* 0x20403e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][10], 0x0); /* 0x2040268 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][11], 0x2); /* 0x20403ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][11], 0x0); /* 0x204026c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][5], 0x3); /* 0x2040334 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][0], 0x6); /* 0x20401a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][1], 0x3); /* 0x20401a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][2], 0x5); /* 0x20401a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][3], 0x6); /* 0x20401ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][4], 0x9); /* 0x20401b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][5], 0x8); /* 0x20401b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][6], 0x4); /* 0x20401b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][7], 0xa); /* 0x20401bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][12], 0x0); /* 0x20403b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][12], 0x3); /* 0x2040230 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][13], 0x19); /* 0x20403b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][13], 0x1); /* 0x2040234 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][6], 0xc); /* 0x2040318 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][0], 0x5); /* 0x20400c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][1], 0x9); /* 0x20400c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][2], 0x6); /* 0x20400c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][3], 0x9); /* 0x20400cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][4], 0xa); /* 0x20400d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][5], 0x8); /* 0x20400d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][6], 0x6); /* 0x20400d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][7], 0x7); /* 0x20400dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][12], 0x8); /* 0x20403f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][12], 0x2); /* 0x2040270 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][13], 0xb); /* 0x20403f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][13], 0x1); /* 0x2040274 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][6], 0x7); /* 0x2040338 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][0], 0x1); /* 0x20401c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][1], 0x0); /* 0x20401c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][2], 0x9); /* 0x20401c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][3], 0x9); /* 0x20401cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][4], 0x3); /* 0x20401d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][5], 0x7); /* 0x20401d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][6], 0x5); /* 0x20401d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][7], 0x5); /* 0x20401dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][14], 0x15); /* 0x20403b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][14], 0x3); /* 0x2040238 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][15], 0xa); /* 0x20403bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][15], 0x0); /* 0x204023c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][7], 0xb); /* 0x204031c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][0], 0x8); /* 0x20400e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][1], 0x8); /* 0x20400e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][2], 0x8); /* 0x20400e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][3], 0x3); /* 0x20400ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][4], 0x2); /* 0x20400f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][5], 0x7); /* 0x20400f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][6], 0x8); /* 0x20400f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][7], 0xa); /* 0x20400fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][14], 0x1); /* 0x20403f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][14], 0x0); /* 0x2040278 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][15], 0x15); /* 0x20403fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][15], 0x0); /* 0x204027c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][7], 0x7); /* 0x204033c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][0], 0x9); /* 0x20401e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][1], 0x6); /* 0x20401e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][2], 0x2); /* 0x20401e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][3], 0x0); /* 0x20401ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][4], 0x2); /* 0x20401f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][5], 0x7); /* 0x20401f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][6], 0x5); /* 0x20401f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][7], 0x9); /* 0x20401fc */
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_table_prop[0], 0x4); /* 0x2014a00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tind_bus_prop[2], 0x4); /* 0x2014808 */
  //    tu.OutWord(&mau_reg_map.rams.match.merge.exact_match_delay_config, 0x50000); /* 0x2016490 */ // REMOVED EMDEL070915
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_hit_to_logical_table_ixbar_outputmap[0], 0x1d); /* 0x2014860 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_match_adr_to_physical_oxbar_outputmap[2], 0x8); /* 0x20164c8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.match_to_logical_table_ixbar_outputmap[1][2], 0x1d); /* 0x2014ac8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_tcam_shiftcount[2], 0x24); /* 0x2016608 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_mask[1][2], 0x3f); /* 0x2014d48 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_tcam_shiftcount[2], 0x0); /* 0x2016588 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_mask[1][2], 0xffffffff); /* 0x2014bc8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.next_table_format_data[13], 0x100); /* 0x2016574 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_tcam_shiftcount[2], 0x2f); /* 0x2016688 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_mask[1][2], 0x3fffff); /* 0x2014ec8 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.immediate_data_32b_ixbar_ctl[13], 0x20); /* 0x20103b4 */
    tu.OutWord(&mau_reg_map.rams.match.merge.predication_ctl[0], 0x2000); /* 0x2014a20 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[ 1].vh_xbars.adr_dist_tind_adr_xbar_ctl[0], 0x8); /* 0x201cd30 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.ram_address_mux_ctl[0][1], 0x100); /* 0x201cb08 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.ram_address_mux_ctl[0][5], 0x80); /* 0x201eb28 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.adr_dist_action_data_adr_icxbar_ctl[13], 0x400); /* 0x20103f4 */
  ActionHvTranslator act_hv_translator;
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][0], 0x1); /* 0x200d8c0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,0,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][0], 0x1); /* 0x200d8c0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,0,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][0], 0x5); /* 0x200d8c0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,0,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][0], 0x5); /* 0x200d8c0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,0,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][0], 0x25); /* 0x200d8c0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,0,0x25); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][0], 0x25); /* 0x200d8c0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,0,0x25); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][0], 0xa5); /* 0x200d8c0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,0,0xa5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][0], 0xa5); /* 0x200d8c0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,0,0xa5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][1], 0x3); /* 0x200d8c4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,1,0x3); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][1], 0x3); /* 0x200d8c4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,1,0x3); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][1], 0xf); /* 0x200d8c4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,1,0xf); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][1], 0xf); /* 0x200d8c4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,1,0xf); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][1], 0x3f); /* 0x200d8c4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,1,0x3f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][1], 0x3f); /* 0x200d8c4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,1,0x3f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][1], 0xff); /* 0x200d8c4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,1,0xff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_half[0][1], 0xff); /* 0x200d8c4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(5,0,1,0xff); // ADDED ACHV070915
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
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][6], RM_B4_32(0xd47e042)); /* 0x207c018 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][3], RM_B4_32(0x1747e2fb)); /* 0x207c08c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][12], RM_B4_32(0x891e195)); /* 0x207c130 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][8], RM_B4_32(0x427e374)); /* 0x207c1a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][15], RM_B4_32(0x3d65e325)); /* 0x207c23c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][9], RM_B4_32(0x12dbe2b0)); /* 0x207c2a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][7], RM_B4_32(0x1b0fe3ff)); /* 0x207c31c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][13], RM_B4_32(0x967e05f)); /* 0x207c3b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][13], RM_B4_32(0x6f5e0da)); /* 0x207c434 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][0], RM_B4_32(0x335fe194)); /* 0x207c480 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][10], RM_B4_32(0x11a3e291)); /* 0x207c528 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][10], RM_B4_32(0x12dde369)); /* 0x207c5a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][11], RM_B4_32(0x703e050)); /* 0x207c62c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][6], RM_B4_32(0x2f85e05b)); /* 0x207c698 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][9], RM_B4_32(0x1e9fe123)); /* 0x207c724 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][5], RM_B4_32(0x110de36d)); /* 0x207c794 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][7], RM_B4_32(0x198fe3b2)); /* 0x207c81c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][13], RM_B4_32(0x2737e1c8)); /* 0x207c8b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][13], RM_B4_32(0xcf9e0ce)); /* 0x207c934 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][15], RM_B4_32(0xae3e273)); /* 0x207c9bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][8], RM_B4_32(0x3d4fe191)); /* 0x207ca20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][12], RM_B4_32(0x12f1e0e6)); /* 0x207cab0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][5], RM_B4_32(0xec9e03c)); /* 0x207cb14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][2], RM_B4_32(0x933e0cb)); /* 0x207cb88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][8], RM_B4_32(0x1a81e0a7)); /* 0x207cc20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][4], RM_B4_32(0x1ab1e0e3)); /* 0x207cc90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][3], RM_B4_32(0x11d7e26b)); /* 0x207cd0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][7], RM_B4_32(0x372de0e0)); /* 0x207cd9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][3], RM_B4_32(0x34cfe21e)); /* 0x207ce0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][2], RM_B4_32(0x2d93e0b3)); /* 0x207ce88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][1], RM_B4_32(0xc05e1eb)); /* 0x207cf04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][6], RM_B4_32(0x3c4be0a9)); /* 0x207cf98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][13], RM_B4_32(0x641e24d)); /* 0x207d034 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][14], RM_B4_32(0x3457e2ce)); /* 0x207d0b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][14], RM_B4_32(0xbe3e3ec)); /* 0x207d138 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][5], RM_B4_32(0x13e9e3f9)); /* 0x207d194 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][3], RM_B4_32(0x141fe0a1)); /* 0x207d20c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][14], RM_B4_32(0x677e1a6)); /* 0x207d2b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][4], RM_B4_32(0x3d59e248)); /* 0x207d310 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][5], RM_B4_32(0x338de0f3)); /* 0x207d394 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][13], RM_B4_32(0x36a1e361)); /* 0x207d434 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][8], RM_B4_32(0x3755e0e5)); /* 0x207d4a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][13], RM_B4_32(0x3f51e28e)); /* 0x207d534 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][10], RM_B4_32(0x1151e09e)); /* 0x207d5a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][5], RM_B4_32(0x30d1e0f0)); /* 0x207d614 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][3], RM_B4_32(0x1317e299)); /* 0x207d68c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][11], RM_B4_32(0x1867e100)); /* 0x207d72c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][3], RM_B4_32(0x3385e0f8)); /* 0x207d78c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][2], RM_B4_32(0x91fe2ce)); /* 0x207d808 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][14], RM_B4_32(0x1c67e093)); /* 0x207d8b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][7], RM_B4_32(0x3a9de2e8)); /* 0x207d91c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][6], RM_B4_32(0x2f5be38f)); /* 0x207d998 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][6], RM_B4_32(0x1f17e187)); /* 0x207da18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][8], RM_B4_32(0x2e3de208)); /* 0x207daa0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][4], RM_B4_32(0x8c9e3e8)); /* 0x207db10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][15], RM_B4_32(0xb29e36e)); /* 0x207dbbc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][3], RM_B4_32(0x1989e21f)); /* 0x207dc0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][11], RM_B4_32(0x87e2e1)); /* 0x207dcac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][11], RM_B4_32(0x319be335)); /* 0x207dd2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][6], RM_B4_32(0x10a1e0ee)); /* 0x207dd98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][0], RM_B4_32(0x2eb7e181)); /* 0x207de00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][11], RM_B4_32(0x3597e224)); /* 0x207deac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][11], RM_B4_32(0x22e3e2d3)); /* 0x207df2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][14], RM_B4_32(0x3dbe35b)); /* 0x207dfb8 */
  act_hv_translator.do_writes(&tu);
    tu.IndirectWrite(0x02008013c4e3, 0x00001fffffffefff, 0x00001ec685aa5544); /* TCAM[ 1][15][227].word1 = 0x6342d52aa2  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x020080004471, 0x0000000000000000, 0x4671bda000000000); /* sram_ 1_ 1: a=0x20080004471 d0=0x0 d1=0x4671bda000000000 */
    tu.IndirectWrite(0x0200800154e3, 0x9efe3293dcf72e3c, 0xe9d55661b0118680); /* sram_ 5_ 5: a=0x200800154e3 d0=0x9efe3293dcf72e3c d1=0xe9d55661b0118680 */
    tu.IndirectWrite(0x02008013c58a, 0x00001ef68fbb77c7, 0x00001fcff5eecd7c); /* TCAM[ 1][15][394].word1 = 0xe7faf766be  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044c5, 0x46c53d3000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x200800044c5 d0=0x46c53d3000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001558a, 0x2a3cb01f60ccbf7c, 0xa5728cfaddd04b02); /* sram_ 5_ 5: a=0x2008001558a d0=0x2a3cb01f60ccbf7c d1=0xa5728cfaddd04b02 */
    tu.IndirectWrite(0x02008013c490, 0x00001ef68fbb77c7, 0x00001fcff5eecd7c); /* TCAM[ 1][15][144].word1 = 0xe7faf766be  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x020080004448, 0x46483dc000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004448 d0=0x46483dc000000000 d1=0x0 */
    tu.IndirectWrite(0x020080015490, 0xf8c20d54e2889e23, 0x0bb6f59ffed87e91); /* sram_ 5_ 5: a=0x20080015490 d0=0xf8c20d54e2889e23 d1=0xbb6f59ffed87e91 */
    tu.IndirectWrite(0x02008013c458, 0x00001ef96bbfc4c9, 0x00001d6ffff7ff7e); /* TCAM[ 1][15][ 88].word1 = 0xb7fffbffbf  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008000442c, 0x462c3d1000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x2008000442c d0=0x462c3d1000000000 d1=0x0 */
    tu.IndirectWrite(0x020080015458, 0x978e0da09a1d7701, 0x1e010de97cd779c5); /* sram_ 5_ 5: a=0x20080015458 d0=0x978e0da09a1d7701 d1=0x1e010de97cd779c5 */
    tu.IndirectWrite(0x02008013c46e, 0x00001fb5ddf1fc7b, 0x00001ddeff3fd7fc); /* TCAM[ 1][15][110].word1 = 0xef7f9febfe  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004437, 0x46373c8000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004437 d0=0x46373c8000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001546e, 0x41e39bcc8e81a65a, 0x45b84965bb81a42e); /* sram_ 5_ 5: a=0x2008001546e d0=0x41e39bcc8e81a65a d1=0x45b84965bb81a42e */
    tu.IndirectWrite(0x02008013c46a, 0x00001ff6fdfbfcfb, 0x00001d9ddf35d77c); /* TCAM[ 1][15][106].word1 = 0xceef9aebbe  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004435, 0x46353d7000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004435 d0=0x46353d7000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001546a, 0x362a6899a5758612, 0x83672bdacaaf3d81); /* sram_ 5_ 5: a=0x2008001546a d0=0x362a6899a5758612 d1=0x83672bdacaaf3d81 */
    tu.IndirectWrite(0x02008013c581, 0x00001ff6fdfbfcfb, 0x00001d9ddf35d77c); /* TCAM[ 1][15][385].word1 = 0xceef9aebbe  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044c0, 0x0000000000000000, 0x46c0bd6000000000); /* sram_ 1_ 1: a=0x200800044c0 d0=0x0 d1=0x46c0bd6000000000 */
    tu.IndirectWrite(0x020080015581, 0xa3a58e2c83375564, 0x0f012aae2457278d); /* sram_ 5_ 5: a=0x20080015581 d0=0xa3a58e2c83375564 d1=0xf012aae2457278d */
    tu.IndirectWrite(0x02008013c410, 0x00001ff4fff9ff79, 0x00001d9fdd37d4fe); /* TCAM[ 1][15][ 16].word1 = 0xcfee9bea7f  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004408, 0x46083d0000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004408 d0=0x46083d0000000000 d1=0x0 */
    tu.IndirectWrite(0x020080015410, 0xfade19bc65806bb9, 0x5205e6f2ee3d2a16); /* sram_ 5_ 5: a=0x20080015410 d0=0xfade19bc65806bb9 d1=0x5205e6f2ee3d2a16 */
    tu.IndirectWrite(0x02008013c48b, 0x00001ff4fff9ff79, 0x00001d9fdd37d4fe); /* TCAM[ 1][15][139].word1 = 0xcfee9bea7f  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004445, 0x0000000000000000, 0x4645bd6000000000); /* sram_ 1_ 1: a=0x20080004445 d0=0x0 d1=0x4645bd6000000000 */
    tu.IndirectWrite(0x02008001548b, 0x4eddd78c805ab72b, 0xf62744db90e5cd52); /* sram_ 5_ 5: a=0x2008001548b d0=0x4eddd78c805ab72b d1=0xf62744db90e5cd52 */
    tu.IndirectWrite(0x02008013c471, 0x00001f0cca5edc41, 0x00001cf335a123be); /* TCAM[ 1][15][113].word1 = 0x799ad091df  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004438, 0x0000000000000000, 0x4638bc8000000000); /* sram_ 1_ 1: a=0x20080004438 d0=0x0 d1=0x4638bc8000000000 */
    tu.IndirectWrite(0x020080015471, 0x7ec904426ab7d071, 0x43eb11db3c607322); /* sram_ 5_ 5: a=0x20080015471 d0=0x7ec904426ab7d071 d1=0x43eb11db3c607322 */
    tu.IndirectWrite(0x02008013c469, 0x00001feeeff4fffd, 0x00001dbbd79fbb02); /* TCAM[ 1][15][105].word1 = 0xddebcfdd81  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004434, 0x0000000000000000, 0x4634bde000000000); /* sram_ 1_ 1: a=0x20080004434 d0=0x0 d1=0x4634bde000000000 */
    tu.IndirectWrite(0x020080015469, 0xbecb692702ca824d, 0x0bf961bbfcb370da); /* sram_ 5_ 5: a=0x20080015469 d0=0xbecb692702ca824d d1=0xbf961bbfcb370da */
    tu.IndirectWrite(0x02008013c4e6, 0x00001fbfefffbba3, 0x00001dead794ff5c); /* TCAM[ 1][15][230].word1 = 0xf56bca7fae  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004473, 0x46733dd000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004473 d0=0x46733dd000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800154e6, 0xedf823d9913d39d7, 0x3ea7eb737bebc810); /* sram_ 5_ 5: a=0x200800154e6 d0=0xedf823d9913d39d7 d1=0x3ea7eb737bebc810 */
    tu.IndirectWrite(0x02008013c48a, 0x00001fbfefffbba3, 0x00001dead794ff5c); /* TCAM[ 1][15][138].word1 = 0xf56bca7fae  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004445, 0x46453cc000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004445 d0=0x46453cc000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001548a, 0x1ab19e87f1cd6a68, 0x7edc455235212d59); /* sram_ 5_ 5: a=0x2008001548a d0=0x1ab19e87f1cd6a68 d1=0x7edc455235212d59 */
    tu.IndirectWrite(0x02008013c4e4, 0x00001ffae7f4bbb7, 0x00001dafdf9fff48); /* TCAM[ 1][15][228].word1 = 0xd7efcfffa4  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004472, 0x46723d3000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004472 d0=0x46723d3000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800154e4, 0xf43c4dc015caf985, 0x204fba6161c04525); /* sram_ 5_ 5: a=0x200800154e4 d0=0xf43c4dc015caf985 d1=0x204fba6161c04525 */
    tu.IndirectWrite(0x02008013c41b, 0x00001ffae7f4bbb7, 0x00001dafdf9fff48); /* TCAM[ 1][15][ 27].word1 = 0xd7efcfffa4  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008000440d, 0x0000000000000000, 0x460dbc8000000000); /* sram_ 1_ 1: a=0x2008000440d d0=0x0 d1=0x460dbc8000000000 */
    tu.IndirectWrite(0x02008001541b, 0xd387edeadc68a1e4, 0x02dd8186622952e7); /* sram_ 5_ 5: a=0x2008001541b d0=0xd387edeadc68a1e4 d1=0x2dd8186622952e7 */
    tu.IndirectWrite(0x02008013c4a1, 0x00001ffeffffbbbb, 0x00001d4bbff9ed7c); /* TCAM[ 1][15][161].word1 = 0xa5dffcf6be  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004450, 0x0000000000000000, 0x4650bc1000000000); /* sram_ 1_ 1: a=0x20080004450 d0=0x0 d1=0x4650bc1000000000 */
    tu.IndirectWrite(0x0200800154a1, 0x7a0fd6ca0d2602f6, 0x4a5308a0d1f17624); /* sram_ 5_ 5: a=0x200800154a1 d0=0x7a0fd6ca0d2602f6 d1=0x4a5308a0d1f17624 */
    tu.IndirectWrite(0x02008013c43c, 0x00001f74fde7fdeb, 0x00001effcffdfff4); /* TCAM[ 1][15][ 60].word1 = 0x7fe7fefffa  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x02008000441e, 0x461e3d6000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x2008000441e d0=0x461e3d6000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001543c, 0x9b63c8b1afbd6e30, 0x05061a832f4fafc3); /* sram_ 5_ 5: a=0x2008001543c d0=0x9b63c8b1afbd6e30 d1=0x5061a832f4fafc3 */
    tu.IndirectWrite(0x02008013c4f6, 0x00001e2f57afb387, 0x00001ff7ed5ddffc); /* TCAM[ 1][15][246].word1 = 0xfbf6aeeffe  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x02008000447b, 0x467b3c4000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x2008000447b d0=0x467b3c4000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800154f6, 0xbc676ca4519ee47f, 0x2c316bf3b925eae6); /* sram_ 5_ 5: a=0x200800154f6 d0=0xbc676ca4519ee47f d1=0x2c316bf3b925eae6 */
    tu.IndirectWrite(0x02008013c42d, 0x00001ff315f9c49f, 0x00001e3fff97ffe8); /* TCAM[ 1][15][ 45].word1 = 0x1fffcbfff4  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x020080004416, 0x0000000000000000, 0x4616bcc000000000); /* sram_ 1_ 1: a=0x20080004416 d0=0x0 d1=0x4616bcc000000000 */
    tu.IndirectWrite(0x02008001542d, 0x969bec47203fcadd, 0x4b771300656a64a6); /* sram_ 5_ 5: a=0x2008001542d d0=0x969bec47203fcadd d1=0x4b771300656a64a6 */
    tu.IndirectWrite(0x02008013c4fe, 0x00001e9e00a200a7, 0x00001d61ff5dff58); /* TCAM[ 1][15][254].word1 = 0xb0ffaeffac  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008000447f, 0x467f3d7000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x2008000447f d0=0x467f3d7000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800154fe, 0xd7b33bddda8ca559, 0x70d4109255354f7f); /* sram_ 5_ 5: a=0x200800154fe d0=0xd7b33bddda8ca559 d1=0x70d4109255354f7f */
    tu.IndirectWrite(0x02008013c45d, 0x00001fbffdff77ff, 0x00001cfbefbbbafc); /* TCAM[ 1][15][ 93].word1 = 0x7df7dddd7e  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008000442e, 0x0000000000000000, 0x462ebc7000000000); /* sram_ 1_ 1: a=0x2008000442e d0=0x0 d1=0x462ebc7000000000 */
    tu.IndirectWrite(0x02008001545d, 0x8b3497c0b8cdb740, 0x170cc6f909abd8b0); /* sram_ 5_ 5: a=0x2008001545d d0=0x8b3497c0b8cdb740 d1=0x170cc6f909abd8b0 */
    tu.IndirectWrite(0x02008013c584, 0x00001fffffffffff, 0x00001c0000000000); /* TCAM[ 1][15][388].word1 = 0x0000000000  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044c2, 0x46c23d1000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x200800044c2 d0=0x46c23d1000000000 d1=0x0 */
    tu.IndirectWrite(0x020080015584, 0xf009edc53e51d61b, 0xd3658cdff8491277); /* sram_ 5_ 5: a=0x20080015584 d0=0xf009edc53e51d61b d1=0xd3658cdff8491277 */
    tu.IndirectWrite(0x02008013c417, 0x00001ff5dffffff5, 0x00001ceeab2b332e); /* TCAM[ 1][15][ 23].word1 = 0x7755959997  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008000440b, 0x0000000000000000, 0x460bbc7000000000); /* sram_ 1_ 1: a=0x2008000440b d0=0x0 d1=0x460bbc7000000000 */
    tu.IndirectWrite(0x020080015417, 0x24c47cc6917403e9, 0x74b80acbcadbc2f4); /* sram_ 5_ 5: a=0x20080015417 d0=0x24c47cc6917403e9 d1=0x74b80acbcadbc2f4 */
    tu.IndirectWrite(0x02008013c470, 0x00001f0fff0bff07, 0x00001cf000f400f8); /* TCAM[ 1][15][112].word1 = 0x78007a007c  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004438, 0x46383c2000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004438 d0=0x46383c2000000000 d1=0x0 */
    tu.IndirectWrite(0x020080015470, 0x5530fbcc6ec18f23, 0x87a67af3a47812cf); /* sram_ 5_ 5: a=0x20080015470 d0=0x5530fbcc6ec18f23 d1=0x87a67af3a47812cf */
    tu.IndirectWrite(0x02008013c45c, 0x00001f0dff09ff05, 0x00001cf200f600fa); /* TCAM[ 1][15][ 92].word1 = 0x79007b007d  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008000442e, 0x462e3d7000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x2008000442e d0=0x462e3d7000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001545c, 0xb0b039aae80e5f4b, 0xf1d88c559015a2d4); /* sram_ 5_ 5: a=0x2008001545c d0=0xb0b039aae80e5f4b d1=0xf1d88c559015a2d4 */
    tu.IndirectWrite(0x02008013c480, 0x00001edeeefbd7fb, 0x00001fbf3b7ffffc); /* TCAM[ 1][15][128].word1 = 0xdf9dbffffe  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x020080004440, 0x46403d9000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004440 d0=0x46403d9000000000 d1=0x0 */
    tu.IndirectWrite(0x020080015480, 0x3105a36a05fd128e, 0x01d90e4d3d6e467e); /* sram_ 5_ 5: a=0x20080015480 d0=0x3105a36a05fd128e d1=0x1d90e4d3d6e467e */
    tu.IndirectWrite(0x02008013c45b, 0x00001ffefdfbddff, 0x00001df7e78776f8); /* TCAM[ 1][15][ 91].word1 = 0xfbf3c3bb7c  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008000442d, 0x0000000000000000, 0x462dbd3000000000); /* sram_ 1_ 1: a=0x2008000442d d0=0x0 d1=0x462dbd3000000000 */
    tu.IndirectWrite(0x02008001545b, 0x7ff70b0daef92d6c, 0x72ad751496347a54); /* sram_ 5_ 5: a=0x2008001545b d0=0x7ff70b0daef92d6c d1=0x72ad751496347a54 */
    tu.IndirectWrite(0x02008013c46b, 0x00001fd2c2e719df, 0x00001d7fff79ff7c); /* TCAM[ 1][15][107].word1 = 0xbfffbcffbe  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004435, 0x0000000000000000, 0x4635bcb000000000); /* sram_ 1_ 1: a=0x20080004435 d0=0x0 d1=0x4635bcb000000000 */
    tu.IndirectWrite(0x02008001546b, 0x672cf6dd7f66bf43, 0xbf9fc785b79d2818); /* sram_ 5_ 5: a=0x2008001546b d0=0x672cf6dd7f66bf43 d1=0xbf9fc785b79d2818 */
    tu.IndirectWrite(0x02008013c4fb, 0x00001f280efc39fd, 0x00001cffff43ff86); /* TCAM[ 1][15][251].word1 = 0x7fffa1ffc3  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008000447d, 0x0000000000000000, 0x467dbc2000000000); /* sram_ 1_ 1: a=0x2008000447d d0=0x0 d1=0x467dbc2000000000 */
    tu.IndirectWrite(0x0200800154fb, 0x4a6ad5159f23fabe, 0x3f667a026254353b); /* sram_ 5_ 5: a=0x200800154fb d0=0x4a6ad5159f23fabe d1=0x3f667a026254353b */
    tu.IndirectWrite(0x02008013c404, 0x00001ebcbab4b2ad, 0x00001d43454b4d52); /* TCAM[ 1][15][  4].word1 = 0xa1a2a5a6a9  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004402, 0x46023c9000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004402 d0=0x46023c9000000000 d1=0x0 */
    tu.IndirectWrite(0x020080015404, 0x3b7962d512629165, 0x25acb85861d38a8f); /* sram_ 5_ 5: a=0x20080015404 d0=0x3b7962d512629165 d1=0x25acb85861d38a8f */
    tu.IndirectWrite(0x02008013c5ac, 0x00001e7ec7ff22b7, 0x00001df7ffeffffc); /* TCAM[ 1][15][428].word1 = 0xfbfff7fffe  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044d6, 0x46d63d5000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x200800044d6 d0=0x46d63d5000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800155ac, 0x304f0beec3414d8b, 0x0069a88425577a29); /* sram_ 5_ 5: a=0x200800155ac d0=0x304f0beec3414d8b d1=0x69a88425577a29 */
    tu.IndirectWrite(0x02008013c48f, 0x00001ef6c7ff22ff, 0x00001d7fffefffb4); /* TCAM[ 1][15][143].word1 = 0xbffff7ffda  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004447, 0x0000000000000000, 0x4647bd7000000000); /* sram_ 1_ 1: a=0x20080004447 d0=0x0 d1=0x4647bd7000000000 */
    tu.IndirectWrite(0x02008001548f, 0x4c9de8ecdd4e64fb, 0x16ca437c0f0dda4a); /* sram_ 5_ 5: a=0x2008001548f d0=0x4c9de8ecdd4e64fb d1=0x16ca437c0f0dda4a */
    tu.IndirectWrite(0x02008013c46f, 0x00001ef6c7ff22ff, 0x00001d7fffefffb4); /* TCAM[ 1][15][111].word1 = 0xbffff7ffda  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004437, 0x0000000000000000, 0x4637bcf000000000); /* sram_ 1_ 1: a=0x20080004437 d0=0x0 d1=0x4637bcf000000000 */
    tu.IndirectWrite(0x02008001546f, 0x465a0a63ab00a2e2, 0x31c6e045e2d0a934); /* sram_ 5_ 5: a=0x2008001546f d0=0x465a0a63ab00a2e2 d1=0x31c6e045e2d0a934 */
    tu.IndirectWrite(0x02008013c453, 0x00001ff57afdd0bd, 0x00001c7bff1fffea); /* TCAM[ 1][15][ 83].word1 = 0x3dff8ffff5  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004429, 0x0000000000000000, 0x4629bda000000000); /* sram_ 1_ 1: a=0x20080004429 d0=0x0 d1=0x4629bda000000000 */
    tu.IndirectWrite(0x020080015453, 0xf0a9608b0b73c315, 0x48f1541a93352d77); /* sram_ 5_ 5: a=0x20080015453 d0=0xf0a9608b0b73c315 d1=0x48f1541a93352d77 */
    tu.IndirectWrite(0x02008013c48c, 0x00001fb7fff3f6f7, 0x00001d6fdc5fdf7c); /* TCAM[ 1][15][140].word1 = 0xb7ee2fefbe  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004446, 0x46463d6000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004446 d0=0x46463d6000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001548c, 0xa6e2399151338cdf, 0xf39170d31a94b8be); /* sram_ 5_ 5: a=0x2008001548c d0=0xa6e2399151338cdf d1=0xf39170d31a94b8be */
    tu.IndirectWrite(0x02008013c416, 0x00001ffffffffebf, 0x00001cf0faaafbcc); /* TCAM[ 1][15][ 22].word1 = 0x787d557de6  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008000440b, 0x460b3cf000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x2008000440b d0=0x460b3cf000000000 d1=0x0 */
    tu.IndirectWrite(0x020080015416, 0xa3a88c27535c0fc2, 0x896ccbe461e87e9a); /* sram_ 5_ 5: a=0x20080015416 d0=0xa3a88c27535c0fc2 d1=0x896ccbe461e87e9a */
    tu.IndirectWrite(0x02008013c414, 0x00001e53ff13ff43, 0x00001dac00ec00bc); /* TCAM[ 1][15][ 20].word1 = 0xd60076005e  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008000440a, 0x460a3c7000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x2008000440a d0=0x460a3c7000000000 d1=0x0 */
    tu.IndirectWrite(0x020080015414, 0x737d42680f801615, 0xd2bf42c013f01e7b); /* sram_ 5_ 5: a=0x20080015414 d0=0x737d42680f801615 d1=0xd2bf42c013f01e7b */
    tu.IndirectWrite(0x02008013c545, 0x00001ffffffffeff, 0x00001db0f6354ffc); /* TCAM[ 1][15][325].word1 = 0xd87b1aa7fe  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044a2, 0x0000000000000000, 0x46a2bd0000000000); /* sram_ 1_ 1: a=0x200800044a2 d0=0x0 d1=0x46a2bd0000000000 */
    tu.IndirectWrite(0x020080015545, 0x0ca66b4ed74990f6, 0x289f8f36b74e7b77); /* sram_ 5_ 5: a=0x20080015545 d0=0xca66b4ed74990f6 d1=0x289f8f36b74e7b77 */
    tu.IndirectWrite(0x02008013c48d, 0x00001ff7da7ffdef, 0x00001efb2de7fe58); /* TCAM[ 1][15][141].word1 = 0x7d96f3ff2c  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x020080004446, 0x0000000000000000, 0x4646bcc000000000); /* sram_ 1_ 1: a=0x20080004446 d0=0x0 d1=0x4646bcc000000000 */
    tu.IndirectWrite(0x02008001548d, 0x8e37dff4e46f41c7, 0xb6848a046d439605); /* sram_ 5_ 5: a=0x2008001548d d0=0x8e37dff4e46f41c7 d1=0xb6848a046d439605 */
    tu.IndirectWrite(0x02008013c449, 0x00001efbac67ff49, 0x00001ff75bfffcfe); /* TCAM[ 1][15][ 73].word1 = 0xfbadfffe7f  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x020080004424, 0x0000000000000000, 0x4624bde000000000); /* sram_ 1_ 1: a=0x20080004424 d0=0x0 d1=0x4624bde000000000 */
    tu.IndirectWrite(0x020080015449, 0xeeb359eaf12f89f4, 0xb0b4de9265808e86); /* sram_ 5_ 5: a=0x20080015449 d0=0xeeb359eaf12f89f4 d1=0xb0b4de9265808e86 */
    tu.IndirectWrite(0x02008013c41a, 0x00001efbac67ff49, 0x00001ff75bfffcfe); /* TCAM[ 1][15][ 26].word1 = 0xfbadfffe7f  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x02008000440d, 0x460d3d7000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x2008000440d d0=0x460d3d7000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001541a, 0x50170408705a0a31, 0xbc721b8926fbb2e5); /* sram_ 5_ 5: a=0x2008001541a d0=0x50170408705a0a31 d1=0xbc721b8926fbb2e5 */
    tu.IndirectWrite(0x02008013c421, 0x00001f67deec826d, 0x00001c9821137d92); /* TCAM[ 1][15][ 33].word1 = 0x4c1089bec9  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004410, 0x0000000000000000, 0x4610bcb000000000); /* sram_ 1_ 1: a=0x20080004410 d0=0x0 d1=0x4610bcb000000000 */
    tu.IndirectWrite(0x020080015421, 0xfb31d1612b69bba8, 0x002c972bef35f853); /* sram_ 5_ 5: a=0x20080015421 d0=0xfb31d1612b69bba8 d1=0x2c972bef35f853 */
    tu.IndirectWrite(0x02008013c4e5, 0x00001fcfd4fcdbb7, 0x00001dff6bfbfec8); /* TCAM[ 1][15][229].word1 = 0xffb5fdff64  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004472, 0x0000000000000000, 0x4672bd1000000000); /* sram_ 1_ 1: a=0x20080004472 d0=0x0 d1=0x4672bd1000000000 */
    tu.IndirectWrite(0x0200800154e5, 0xa76f65bb887f44a6, 0x499e674c644bd339); /* sram_ 5_ 5: a=0x200800154e5 d0=0xa76f65bb887f44a6 d1=0x499e674c644bd339 */
    tu.IndirectWrite(0x02008013c48e, 0x00001e33a8ede7af, 0x00001fdf7ffafe5c); /* TCAM[ 1][15][142].word1 = 0xefbffd7f2e  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x020080004447, 0x46473da000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004447 d0=0x46473da000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001548e, 0xa34811be764c4996, 0x28ee2ec78c5fbc50); /* sram_ 5_ 5: a=0x2008001548e d0=0xa34811be764c4996 d1=0x28ee2ec78c5fbc50 */
    tu.IndirectWrite(0x02008013c530, 0x00001eb33eebef7f, 0x00001f5fe9fcf68c); /* TCAM[ 1][15][304].word1 = 0xaff4fe7b46  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x020080004498, 0x46983da000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004498 d0=0x46983da000000000 d1=0x0 */
    tu.IndirectWrite(0x020080015530, 0x09373a2442dac589, 0x70d0b7bfafd3f130); /* sram_ 5_ 5: a=0x20080015530 d0=0x9373a2442dac589 d1=0x70d0b7bfafd3f130 */
    tu.IndirectWrite(0x02008013c40f, 0x00001eb33eebef7f, 0x00001f5fe9fcf68c); /* TCAM[ 1][15][ 15].word1 = 0xaff4fe7b46  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x020080004407, 0x0000000000000000, 0x4607bc9000000000); /* sram_ 1_ 1: a=0x20080004407 d0=0x0 d1=0x4607bc9000000000 */
    tu.IndirectWrite(0x02008001540f, 0xce151aba6e02c916, 0xf8fbaa23c157b3ce); /* sram_ 5_ 5: a=0x2008001540f d0=0xce151aba6e02c916 d1=0xf8fbaa23c157b3ce */
    tu.IndirectWrite(0x02008013c402, 0x00001e96948e8c87, 0x00001d696b717378); /* TCAM[ 1][15][  2].word1 = 0xb4b5b8b9bc  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004401, 0x46013cd000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004401 d0=0x46013cd000000000 d1=0x0 */
    tu.IndirectWrite(0x020080015402, 0x0d5e81d0e90e1336, 0x42a499449dae7b68); /* sram_ 5_ 5: a=0x20080015402 d0=0xd5e81d0e90e1336 d1=0x42a499449dae7b68 */
    tu.IndirectWrite(0x02008013c487, 0x00001fbeb9e3fe5d, 0x00001fffd79dd1fe); /* TCAM[ 1][15][135].word1 = 0xffebcee8ff  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x020080004443, 0x0000000000000000, 0x4643bca000000000); /* sram_ 1_ 1: a=0x20080004443 d0=0x0 d1=0x4643bca000000000 */
    tu.IndirectWrite(0x020080015487, 0x6f92c372f040537e, 0x2af48ae2c8800148); /* sram_ 5_ 5: a=0x20080015487 d0=0x6f92c372f040537e d1=0x2af48ae2c8800148 */
    tu.IndirectWrite(0x02008013c46d, 0x00001f4bbefd85ab, 0x00001ffce3977bfc); /* TCAM[ 1][15][109].word1 = 0xfe71cbbdfe  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x020080004436, 0x0000000000000000, 0x4636bda000000000); /* sram_ 1_ 1: a=0x20080004436 d0=0x0 d1=0x4636bda000000000 */
    tu.IndirectWrite(0x02008001546d, 0xbb25d0a57524c949, 0x6f4e557f1f42b1eb); /* sram_ 5_ 5: a=0x2008001546d d0=0xbb25d0a57524c949 d1=0x6f4e557f1f42b1eb */
    tu.IndirectWrite(0x02008013c40a, 0x00001f8fc6f87d8f, 0x00001c7039078270); /* TCAM[ 1][15][ 10].word1 = 0x381c83c138  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004405, 0x46053ca000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004405 d0=0x46053ca000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001540a, 0xc9942dc4d4d56d29, 0x4c13a63910695c69); /* sram_ 5_ 5: a=0x2008001540a d0=0xc9942dc4d4d56d29 d1=0x4c13a63910695c69 */
    tu.IndirectWrite(0x02008013c531, 0x00001e73ff33ff63, 0x00001d8c00cc009c); /* TCAM[ 1][15][305].word1 = 0xc60066004e  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004498, 0x0000000000000000, 0x4698bc0000000000); /* sram_ 1_ 1: a=0x20080004498 d0=0x0 d1=0x4698bc0000000000 */
    tu.IndirectWrite(0x020080015531, 0x4db0f0ce498e1073, 0xcf7cc96cb412b7ba); /* sram_ 5_ 5: a=0x20080015531 d0=0x4db0f0ce498e1073 d1=0xcf7cc96cb412b7ba */
    tu.IndirectWrite(0x02008013c44a, 0x00001e4a004e0053, 0x00001db5ffb1ffac); /* TCAM[ 1][15][ 74].word1 = 0xdaffd8ffd6  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004425, 0x46253df000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004425 d0=0x46253df000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001544a, 0xe4507a5b7d8d3737, 0x720cf2446d153a9d); /* sram_ 5_ 5: a=0x2008001544a d0=0xe4507a5b7d8d3737 d1=0x720cf2446d153a9d */
    tu.IndirectWrite(0x02008013c401, 0x00001e4a004e0053, 0x00001db5ffb1ffac); /* TCAM[ 1][15][  1].word1 = 0xdaffd8ffd6  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004400, 0x0000000000000000, 0x4600bd0000000000); /* sram_ 1_ 1: a=0x20080004400 d0=0x0 d1=0x4600bd0000000000 */
    tu.IndirectWrite(0x020080015401, 0xe05445e47ca729e3, 0x1530aa439be4718f); /* sram_ 5_ 5: a=0x20080015401 d0=0xe05445e47ca729e3 d1=0x1530aa439be4718f */
    tu.IndirectWrite(0x02008013c40d, 0x00001fc2a5ddccb3, 0x00001c3d5a22334c); /* TCAM[ 1][15][ 13].word1 = 0x1ead1119a6  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004406, 0x0000000000000000, 0x4606bd6000000000); /* sram_ 1_ 1: a=0x20080004406 d0=0x0 d1=0x4606bd6000000000 */
    tu.IndirectWrite(0x02008001540d, 0x7b2ddd22c75e28b9, 0xb177f89098f88475); /* sram_ 5_ 5: a=0x2008001540d d0=0x7b2ddd22c75e28b9 d1=0xb177f89098f88475 */
    tu.IndirectWrite(0x02008013c586, 0x00001f9777bd79e7, 0x00001effffcfce7c); /* TCAM[ 1][15][390].word1 = 0x7fffe7e73e  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044c3, 0x46c33da000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x200800044c3 d0=0x46c33da000000000 d1=0x0 */
    tu.IndirectWrite(0x020080015586, 0xd33d16f5723abab1, 0x43b8085281d756f1); /* sram_ 5_ 5: a=0x20080015586 d0=0xd33d16f5723abab1 d1=0x43b8085281d756f1 */
    tu.IndirectWrite(0x02008013c41f, 0x00001eb777dd4fed, 0x00001fdfffaff876); /* TCAM[ 1][15][ 31].word1 = 0xefffd7fc3b  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x02008000440f, 0x0000000000000000, 0x460fbd7000000000); /* sram_ 1_ 1: a=0x2008000440f d0=0x0 d1=0x460fbd7000000000 */
    tu.IndirectWrite(0x02008001541f, 0x9d39f7478a066c0b, 0xa6820affabbf7041); /* sram_ 5_ 5: a=0x2008001541f d0=0x9d39f7478a066c0b d1=0xa6820affabbf7041 */
    tu.IndirectWrite(0x02008013c599, 0x00001eb777dd4fed, 0x00001fdfffaff876); /* TCAM[ 1][15][409].word1 = 0xefffd7fc3b  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044cc, 0x0000000000000000, 0x46ccbd8000000000); /* sram_ 1_ 1: a=0x200800044cc d0=0x0 d1=0x46ccbd8000000000 */
    tu.IndirectWrite(0x020080015599, 0x0da296b0df0c1880, 0x03fe1f1bbed32e25); /* sram_ 5_ 5: a=0x20080015599 d0=0xda296b0df0c1880 d1=0x3fe1f1bbed32e25 */
    tu.IndirectWrite(0x02008013c40b, 0x00001f7ffff7fbff, 0x00001cef768e26f4); /* TCAM[ 1][15][ 11].word1 = 0x77bb47137a  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004405, 0x0000000000000000, 0x4605bd8000000000); /* sram_ 1_ 1: a=0x20080004405 d0=0x0 d1=0x4605bd8000000000 */
    tu.IndirectWrite(0x02008001540b, 0x9ea9559ad4a7be47, 0x6b19c44c244a337d); /* sram_ 5_ 5: a=0x2008001540b d0=0x9ea9559ad4a7be47 d1=0x6b19c44c244a337d */
    tu.IndirectWrite(0x02008013c40c, 0x00001eff7ff7ff6d, 0x00001dd3efdf8df6); /* TCAM[ 1][15][ 12].word1 = 0xe9f7efc6fb  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004406, 0x46063db000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004406 d0=0x46063db000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001540c, 0xd6723933ff00b56f, 0x61366fb35b441b56); /* sram_ 5_ 5: a=0x2008001540c d0=0xd6723933ff00b56f d1=0x61366fb35b441b56 */
    tu.IndirectWrite(0x02008013c5c8, 0x00001fcd02de1d6d, 0x00001c32fd21e292); /* TCAM[ 1][15][456].word1 = 0x197e90f149  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044e4, 0x46e43d7000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x200800044e4 d0=0x46e43d7000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800155c8, 0x94aceb100b7e7995, 0xfeb528455d14b9b0); /* sram_ 5_ 5: a=0x200800155c8 d0=0x94aceb100b7e7995 d1=0xfeb528455d14b9b0 */
    tu.IndirectWrite(0x02008013c53e, 0x00001fe1e2171503, 0x00001c1e1de8eafc); /* TCAM[ 1][15][318].word1 = 0x0f0ef4757e  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008000449f, 0x469f3da000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x2008000449f d0=0x469f3da000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001553e, 0x8fc509beaec0e225, 0x949dbde6df9a1eb4); /* sram_ 5_ 5: a=0x2008001553e d0=0x8fc509beaec0e225 d1=0x949dbde6df9a1eb4 */
    tu.IndirectWrite(0x02008013c5b4, 0x00001ffdf7fdfd75, 0x00001cfe1b9aaffa); /* TCAM[ 1][15][436].word1 = 0x7f0dcd57fd  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044da, 0x46da3dc000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x200800044da d0=0x46da3dc000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800155b4, 0x442f78a0a15e9c84, 0x3d3cc1f186cb220a); /* sram_ 5_ 5: a=0x200800155b4 d0=0x442f78a0a15e9c84 d1=0x3d3cc1f186cb220a */
    tu.IndirectWrite(0x02008013c575, 0x00001fffffff7ff3, 0x00001dde9ceea4dc); /* TCAM[ 1][15][373].word1 = 0xef4e77526e  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044ba, 0x0000000000000000, 0x46babcc000000000); /* sram_ 1_ 1: a=0x200800044ba d0=0x0 d1=0x46babcc000000000 */
    tu.IndirectWrite(0x020080015575, 0xf7008e101d907acf, 0x840a1b3c5446ce10); /* sram_ 5_ 5: a=0x20080015575 d0=0xf7008e101d907acf d1=0x840a1b3c5446ce10 */
    tu.IndirectWrite(0x02008013c450, 0x00001ffefcfe7cff, 0x00001ddf9fefa7d0); /* TCAM[ 1][15][ 80].word1 = 0xefcff7d3e8  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004428, 0x46283c9000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004428 d0=0x46283c9000000000 d1=0x0 */
    tu.IndirectWrite(0x020080015450, 0x787ddf2e4112b436, 0xda7eb8c2d832f6bf); /* sram_ 5_ 5: a=0x20080015450 d0=0x787ddf2e4112b436 d1=0xda7eb8c2d832f6bf */
    tu.IndirectWrite(0x02008013c5a1, 0x00001ffefcfe7cff, 0x00001ddf9fefa7d0); /* TCAM[ 1][15][417].word1 = 0xefcff7d3e8  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044d0, 0x0000000000000000, 0x46d0bcd000000000); /* sram_ 1_ 1: a=0x200800044d0 d0=0x0 d1=0x46d0bcd000000000 */
    tu.IndirectWrite(0x0200800155a1, 0x404e1cb3a7a07491, 0xac23c45625173987); /* sram_ 5_ 5: a=0x200800155a1 d0=0x404e1cb3a7a07491 d1=0xac23c45625173987 */
    tu.IndirectWrite(0x02008013c551, 0x00001ee393b344ed, 0x00001d1c6c4cbb16); /* TCAM[ 1][15][337].word1 = 0x8e36265d8b  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044a8, 0x0000000000000000, 0x46a8bc7000000000); /* sram_ 1_ 1: a=0x200800044a8 d0=0x0 d1=0x46a8bc7000000000 */
    tu.IndirectWrite(0x020080015551, 0x30a82b1527d0301b, 0x6046a9b16d646a6b); /* sram_ 5_ 5: a=0x20080015551 d0=0x30a82b1527d0301b d1=0x6046a9b16d646a6b */
    tu.IndirectWrite(0x02008013c4fc, 0x00001faffb5bfbb7, 0x00001f779eaf37c8); /* TCAM[ 1][15][252].word1 = 0xbbcf579be4  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x02008000447e, 0x467e3c7000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x2008000447e d0=0x467e3c7000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800154fc, 0xf353cc3c90db2fff, 0xefb96ad8ba6c006c); /* sram_ 5_ 5: a=0x200800154fc d0=0xf353cc3c90db2fff d1=0xefb96ad8ba6c006c */
    tu.IndirectWrite(0x02008013c4a4, 0x00001f779a2ff793, 0x00001fafffdb3bec); /* TCAM[ 1][15][164].word1 = 0xd7ffed9df6  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x020080004452, 0x46523df000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004452 d0=0x46523df000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800154a4, 0xb8f37d11d698cd2e, 0xb509bdc875de768d); /* sram_ 5_ 5: a=0x200800154a4 d0=0xb8f37d11d698cd2e d1=0xb509bdc875de768d */
    tu.IndirectWrite(0x02008013c49a, 0x00001f779a2ff793, 0x00001fafffdb3bec); /* TCAM[ 1][15][154].word1 = 0xd7ffed9df6  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x02008000444d, 0x464d3d1000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x2008000444d d0=0x464d3d1000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001549a, 0x50fa514c92f8ad8f, 0xd47a96c7b62d82a9); /* sram_ 5_ 5: a=0x2008001549a d0=0x50fa514c92f8ad8f d1=0xd47a96c7b62d82a9 */
    tu.IndirectWrite(0x02008013c488, 0x00001fb943f39517, 0x00001c46bc0c6ae8); /* TCAM[ 1][15][136].word1 = 0x235e063574  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004444, 0x46443cb000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004444 d0=0x46443cb000000000 d1=0x0 */
    tu.IndirectWrite(0x020080015488, 0x1cd7d7f256996d19, 0xcc5167c5a4361a5a); /* sram_ 5_ 5: a=0x20080015488 d0=0x1cd7d7f256996d19 d1=0xcc5167c5a4361a5a */
    tu.IndirectWrite(0x02008013c59a, 0x00001ff93a6dcb01, 0x00001c06c59234fe); /* TCAM[ 1][15][410].word1 = 0x0362c91a7f  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044cd, 0x46cd3cf000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x200800044cd d0=0x46cd3cf000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001559a, 0xd3f98b9242e812ed, 0xd6244dbdecb6a83e); /* sram_ 5_ 5: a=0x2008001559a d0=0xd3f98b9242e812ed d1=0xd6244dbdecb6a83e */
    tu.IndirectWrite(0x02008013c585, 0x00001ea1ffa1ffa1, 0x00001d5e005e005e); /* TCAM[ 1][15][389].word1 = 0xaf002f002f  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044c2, 0x0000000000000000, 0x46c2bda000000000); /* sram_ 1_ 1: a=0x200800044c2 d0=0x0 d1=0x46c2bda000000000 */
    tu.IndirectWrite(0x020080015585, 0xd3cb6b5de6cceeab, 0x25412004f7e3c06b); /* sram_ 5_ 5: a=0x20080015585 d0=0xd3cb6b5de6cceeab d1=0x25412004f7e3c06b */
    tu.IndirectWrite(0x02008013c40e, 0x00001f665f773cfb, 0x00001dfbffdbff04); /* TCAM[ 1][15][ 14].word1 = 0xfdffedff82  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x020080004407, 0x46073cb000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004407 d0=0x46073cb000000000 d1=0x0 */
    tu.IndirectWrite(0x02008001540e, 0x758110856b159207, 0x28ee01720e33a06b); /* sram_ 5_ 5: a=0x2008001540e d0=0x758110856b159207 d1=0x28ee01720e33a06b */
    tu.IndirectWrite(0x02008013c544, 0x00001f5800580059, 0x00001ca7ffa7ffa6); /* TCAM[ 1][15][324].word1 = 0x53ffd3ffd3  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044a2, 0x46a23d1000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x200800044a2 d0=0x46a23d1000000000 d1=0x0 */
    tu.IndirectWrite(0x020080015544, 0xe4878fa2b1888f65, 0xbe5ee90ba7559b89); /* sram_ 5_ 5: a=0x20080015544 d0=0xe4878fa2b1888f65 d1=0xbe5ee90ba7559b89 */
    tu.IndirectWrite(0x02008013c4fd, 0x00001e412788516b, 0x00001dbed877ae94); /* TCAM[ 1][15][253].word1 = 0xdf6c3bd74a  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x02008000447e, 0x0000000000000000, 0x467ebc4000000000); /* sram_ 1_ 1: a=0x2008000447e d0=0x0 d1=0x467ebc4000000000 */
    tu.IndirectWrite(0x0200800154fd, 0xd5a60ee55944a756, 0x06a51e284243f876); /* sram_ 5_ 5: a=0x200800154fd d0=0xd5a60ee55944a756 d1=0x6a51e284243f876 */
    tu.IndirectWrite(0x02008013c5b0, 0x00001f6ff7de6f95, 0x00001ebfafbffbfe); /* TCAM[ 1][15][432].word1 = 0x5fd7dffdff  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044d8, 0x46d83c7000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x200800044d8 d0=0x46d83c7000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800155b0, 0xe2625c8aee8f8239, 0x1dee58759eac2c54); /* sram_ 5_ 5: a=0x200800155b0 d0=0xe2625c8aee8f8239 d1=0x1dee58759eac2c54 */
    tu.IndirectWrite(0x02008013c57d, 0x00001f3fe2f7bf7f, 0x00001defffebfffc); /* TCAM[ 1][15][381].word1 = 0xf7fff5fffe  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044be, 0x0000000000000000, 0x46bebdd000000000); /* sram_ 1_ 1: a=0x200800044be d0=0x0 d1=0x46bebdd000000000 */
    tu.IndirectWrite(0x02008001557d, 0xa2257b9c0bf63cca, 0x7f575bf500fff0cf); /* sram_ 5_ 5: a=0x2008001557d d0=0xa2257b9c0bf63cca d1=0x7f575bf500fff0cf */
    tu.IndirectWrite(0x02008013c595, 0x00001f7c3f7fcadf, 0x00001cafe7d2f7f4); /* TCAM[ 1][15][405].word1 = 0x57f3e97bfa  pt=b00 VV=b11 key=b10 py=0 mr=0 */
    tu.IndirectWrite(0x0200800044ca, 0x0000000000000000, 0x46cabd6000000000); /* sram_ 1_ 1: a=0x200800044ca d0=0x0 d1=0x46cabd6000000000 */
    tu.IndirectWrite(0x020080015595, 0x800c484e83976ebe, 0xdb16c2745bbaeaab); /* sram_ 5_ 5: a=0x20080015595 d0=0x800c484e83976ebe d1=0xdb16c2745bbaeaab */
    tu.IndirectWrite(0x02008013c4b0, 0x00001f34fdbdddf9, 0x00001eff12f23f66); /* TCAM[ 1][15][176].word1 = 0x7f89791fb3  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x020080004458, 0x46583d5000000000, 0x0000000000000000); /* sram_ 1_ 1: a=0x20080004458 d0=0x46583d5000000000 d1=0x0 */
    tu.IndirectWrite(0x0200800154b0, 0x0707d748f799be9c, 0xa421c5dae11fd202); /* sram_ 5_ 5: a=0x200800154b0 d0=0x707d748f799be9c d1=0xa421c5dae11fd202 */
    tu.IndirectWrite(0x02008013c477, 0x00001eb6d4f57d7d, 0x00001f7d3bba9fe2); /* TCAM[ 1][15][119].word1 = 0xbe9ddd4ff1  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x02008000443b, 0x0000000000000000, 0x463bbcb000000000); /* sram_ 1_ 1: a=0x2008000443b d0=0x0 d1=0x463bbcb000000000 */
    tu.IndirectWrite(0x020080015477, 0x22579ab36e48aa3f, 0xf7cfe45ecba96bfe); /* sram_ 5_ 5: a=0x20080015477 d0=0x22579ab36e48aa3f d1=0xf7cfe45ecba96bfe */
    tu.IndirectWrite(0x02008013c445, 0x00001eb6d4f57d7d, 0x00001f7d3bba9fe2); /* TCAM[ 1][15][ 69].word1 = 0xbe9ddd4ff1  pt=b00 VV=b11 key=b11 py=0 mr=0 */
    tu.IndirectWrite(0x020080004422, 0x0000000000000000, 0x4622bd7000000000); /* sram_ 1_ 1: a=0x20080004422 d0=0x0 d1=0x4622bd7000000000 */
    tu.IndirectWrite(0x020080015445, 0xc1219c3a8893d0d8, 0xfc0f49b1093df000); /* sram_ 5_ 5: a=0x20080015445 d0=0xc1219c3a8893d0d8 d1=0xfc0f49b1093df000 */



    Phv *phv_in2 = tu.phv_alloc();
    phv_in2->set_ingress();
    phv_in2->set(  0, 0x26622662); 	/* [0, 0] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(  1, 0x13311331); 	/* [0, 1] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(  5, 0x11331133); 	/* [0, 5] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(  6, 0x88998899); 	/* [0, 6] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(  7, 0xc44cc44c); 	/* [0, 7] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 11, 0xcc44cc44); 	/* [0,11] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 12, 0x66226622); 	/* [0,12] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 13, 0x33113311); 	/* [0,13] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 14, 0x99889988); 	/* [0,14] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 16, 0x26622662); 	/* [0,16] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 17, 0x13311331); 	/* [0,17] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 18, 0x89988998); 	/* [0,18] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 19, 0x44cc44cc); 	/* [0,19] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 23, 0xc44cc44c); 	/* [0,23] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 24, 0x62266226); 	/* [0,24] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 25, 0x31133113); 	/* [0,25] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 27, 0xcc44cc44); 	/* [0,27] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 28, 0x66226622); 	/* [0,28] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 29, 0x33113311); 	/* [0,29] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 30, 0x99889988); 	/* [0,30] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 31, 0x4cc44cc4); 	/* [0,31] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 32, 0x26622662); 	/* [1, 0] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 33, 0x13311331); 	/* [1, 1] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 34, 0x89988998); 	/* [1, 2] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 35, 0x44cc44cc); 	/* [1, 3] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 36, 0x22662266); 	/* [1, 4] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 38, 0x88998899); 	/* [1, 6] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 39, 0xc44cc44c); 	/* [1, 7] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 40, 0x62266226); 	/* [1, 8] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 42, 0x98899889); 	/* [1,10] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 43, 0xcc44cc44); 	/* [1,11] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 44, 0x66226622); 	/* [1,12] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 47, 0x4cc44cc4); 	/* [1,15] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 48, 0x26622662); 	/* [1,16] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 49, 0x13311331); 	/* [1,17] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 50, 0x89988998); 	/* [1,18] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 52, 0x22662266); 	/* [1,20] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 53, 0x11331133); 	/* [1,21] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 54, 0x88998899); 	/* [1,22] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 55, 0xc44cc44c); 	/* [1,23] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 56, 0x62266226); 	/* [1,24] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 58, 0x98899889); 	/* [1,26] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 60, 0x66226622); 	/* [1,28] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 61, 0x33113311); 	/* [1,29] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 63, 0x4cc44cc4); 	/* [1,31] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 66, 0x98); 	/* [2, 2] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 67, 0x4c); 	/* [2, 3] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 70, 0x89); 	/* [2, 6] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 72, 0x62); 	/* [2, 8] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 73, 0x31); 	/* [2, 9] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 74, 0x98); 	/* [2,10] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 76, 0x26); 	/* [2,12] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 77, 0x13); 	/* [2,13] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 80, 0x62); 	/* [2,16] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 83, 0x4c); 	/* [2,19] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 84, 0x26); 	/* [2,20] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 85, 0x13); 	/* [2,21] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 86, 0x89); 	/* [2,22] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 87, 0xc4); 	/* [2,23] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 88, 0x62); 	/* [2,24] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 90, 0x98); 	/* [2,26] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 92, 0x26); 	/* [2,28] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 94, 0x89); 	/* [2,30] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 96, 0x62); 	/* [3, 0] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 97, 0x31); 	/* [3, 1] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 98, 0x98); 	/* [3, 2] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set( 99, 0x4c); 	/* [3, 3] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(101, 0x13); 	/* [3, 5] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(105, 0x31); 	/* [3, 9] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(106, 0x98); 	/* [3,10] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(107, 0x4c); 	/* [3,11] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(110, 0x89); 	/* [3,14] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(113, 0x31); 	/* [3,17] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(115, 0x4c); 	/* [3,19] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(116, 0x26); 	/* [3,20] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(117, 0x13); 	/* [3,21] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(118, 0x89); 	/* [3,22] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(119, 0xc4); 	/* [3,23] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(121, 0x31); 	/* [3,25] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(123, 0x4c); 	/* [3,27] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(124, 0x26); 	/* [3,28] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(125, 0x13); 	/* [3,29] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(126, 0x89); 	/* [3,30] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(127, 0xc4); 	/* [3,31] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(129, 0x0031); 	/* [4, 1] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(130, 0x0018); 	/* [4, 2] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(131, 0x000c); 	/* [4, 3] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(132, 0x0006); 	/* [4, 4] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(133, 0x0003); 	/* [4, 5] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(134, 0x0001); 	/* [4, 6] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(138, 0x0000); 	/* [4,10] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(142, 0x0000); 	/* [4,14] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(147, 0x0000); 	/* [4,19] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(148, 0x0000); 	/* [4,20] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(149, 0x0000); 	/* [4,21] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(150, 0x0000); 	/* [4,22] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(153, 0x0000); 	/* [4,25] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(154, 0x0000); 	/* [4,26] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(155, 0x0000); 	/* [4,27] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(156, 0x0000); 	/* [4,28] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(157, 0x0000); 	/* [4,29] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(158, 0x0000); 	/* [4,30] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(160, 0x0000); 	/* [5, 0] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(161, 0x0000); 	/* [5, 1] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(162, 0x0000); 	/* [5, 2] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(164, 0x0000); 	/* [5, 4] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(167, 0x0000); 	/* [5, 7] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(169, 0x0000); 	/* [5, 9] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(170, 0x0000); 	/* [5,10] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(171, 0x0000); 	/* [5,11] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(172, 0x0000); 	/* [5,12] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(175, 0x0000); 	/* [5,15] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(176, 0x0000); 	/* [5,16] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(177, 0x0000); 	/* [5,17] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(179, 0x0000); 	/* [5,19] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(183, 0x0000); 	/* [5,23] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(186, 0x0000); 	/* [5,26] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(187, 0x0000); 	/* [5,27] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(188, 0x0000); 	/* [5,28] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(189, 0x0000); 	/* [5,29] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(193, 0x0000); 	/* [6, 1] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(194, 0x0000); 	/* [6, 2] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(195, 0x0000); 	/* [6, 3] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(198, 0x0000); 	/* [6, 6] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(199, 0x0000); 	/* [6, 7] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(200, 0x0000); 	/* [6, 8] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(201, 0x0000); 	/* [6, 9] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(202, 0x0000); 	/* [6,10] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(203, 0x0000); 	/* [6,11] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(204, 0x0000); 	/* [6,12] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(206, 0x0000); 	/* [6,14] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(207, 0x0000); 	/* [6,15] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(209, 0x0000); 	/* [6,17] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(210, 0x0000); 	/* [6,18] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(211, 0x0000); 	/* [6,19] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(212, 0x0000); 	/* [6,20] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(213, 0x0000); 	/* [6,21] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(218, 0x0000); 	/* [6,26] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(221, 0x0000); 	/* [6,29] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(222, 0x0000); 	/* [6,30] v=1  #0# e=0 RefModel iPhv 2o */
    phv_in2->set(223, 0x0000); 	/* [6,31] v=1  #0# e=0 RefModel iPhv 2o */


    // Disable ingress/egress counting for phv_in2
    phv_in2->set_countable(false, true);  // ingress
    phv_in2->set_countable(false, false); // egress
    

    Phv *phv_in_a = phv_in2->clone(true); // Copydata
    //Phv *phv_in_b = phv_in2->clone(true); // Copydata
    Phv *phv_in_c = phv_in2->clone(true); // Copydata
    


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
    RMT_UT_LOG_INFO("Dv11Test::InPkt=%p [DA=%04X%08X]\n", p_in,
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


    uint32_t phv_in_a_hash_before = phv_in_a->hash();
    Phv *phv_out_a = tu.port_process_inbound(port, phv_in_a);    
    uint32_t phv_in_a_hash_after = phv_in_a->hash();
    EXPECT_EQ(phv_in_a_hash_before, phv_in_a_hash_after);
    
    //Phv *phv_out_b = tu.port_process_inbound(port, phv_in_b);

    // Up this value to get test to loop a few times
    // Apparently were getting diff results on lookup 1,2,3
    // than on lookup 0
    int loopmax = 1;
    for (int i = 0; i < loopmax; i++) {
      Phv *phv_out_c = tu.port_process_inbound(port, phv_in_c);
      tu.phv_free(phv_out_c);
    }

    // Check phv_out_a
    int i;
    for (i = 0; i < 32; i++) {
      uint32_t actual = phv_out_a->get(i);
      //if (i == 0) printf("EXPECTED=0x%08x ACTUAL=0x%08x\n", 0x55555555, actual);
      if (i < 10) printf("OutputPHV<%d>=0x%08x\n", i, actual);
    }
    for (i = 32; i < 64; i++) {
      uint32_t actual = phv_out_a->get(i);
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
