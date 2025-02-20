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

// XXX -> test_dv16.cpp
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

  bool dv16_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv16Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv16_print) RMT_UT_LOG_INFO("test_dv16_packet1()\n");
    
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
    tu.set_dv_test(16);
    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
    
    
    auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);
    tu.OutWordPeT(0, 0, &mau_reg_map.dp.phv_egress_thread[0][0], 0xffffffff); /* 0x20601c0 */
    tu.OutWordPeT(0, 1, &mau_reg_map.dp.phv_egress_thread[0][1], 0xffffffff); /* 0x20601c4 */
    tu.OutWordPeT(0, 2, &mau_reg_map.dp.phv_egress_thread[0][2], 0xffffffff); /* 0x20601c8 */
    tu.OutWordPeT(0, 3, &mau_reg_map.dp.phv_egress_thread[0][3], 0xffffffff); /* 0x20601cc */
    tu.OutWordPeT(0, 4, &mau_reg_map.dp.phv_egress_thread[0][4], 0xffffffff); /* 0x20601d0 */
    tu.OutWordPeT(0, 5, &mau_reg_map.dp.phv_egress_thread[0][5], 0xffffffff); /* 0x20601d4 */
    tu.OutWordPeT(0, 6, &mau_reg_map.dp.phv_egress_thread[0][6], 0xffffffff); /* 0x20601d8 */
    tu.OutWordPeT(1, 0, &mau_reg_map.dp.phv_egress_thread[1][0], 0xffffffff); /* 0x20601e0 */
    tu.OutWordPeT(1, 1, &mau_reg_map.dp.phv_egress_thread[1][1], 0xffffffff); /* 0x20601e4 */
    tu.OutWordPeT(1, 2, &mau_reg_map.dp.phv_egress_thread[1][2], 0xffffffff); /* 0x20601e8 */
    tu.OutWordPeT(1, 3, &mau_reg_map.dp.phv_egress_thread[1][3], 0xffffffff); /* 0x20601ec */
    tu.OutWordPeT(1, 4, &mau_reg_map.dp.phv_egress_thread[1][4], 0xffffffff); /* 0x20601f0 */
    tu.OutWordPeT(1, 5, &mau_reg_map.dp.phv_egress_thread[1][5], 0xffffffff); /* 0x20601f4 */
    tu.OutWordPeT(1, 6, &mau_reg_map.dp.phv_egress_thread[1][6], 0xffffffff); /* 0x20601f8 */
    tu.OutWord(&mau_reg_map.dp.action_output_delay[0], 0xb); /* 0x2060030 */
    //Removed in bfnregs 20150107_182406_7982_mau_dev
    //tu.OutWord(&mau_reg_map.dp.final_output_delay[0][0], 0x1); /* 0x2060040 */
    tu.OutWord(&mau_reg_map.dp.action_output_delay[1], 0xc); /* 0x2060038 */
    //Removed in bfnregs 20150107_182406_7982_mau_dev
    //tu.OutWord(&mau_reg_map.dp.final_output_delay[1][0], 0x4); /* 0x2060048 */
    tu.OutWord(&mau_reg_map.dp.pipelength_added_stages[1], 0x1); /* 0x2060078 */
    tu.OutWord(&mau_reg_map.dp.cur_stage_dependency_on_prev[1], 0x1); /* 0x2060058 */
    tu.OutWord(&mau_reg_map.dp.match_ie_input_mux_sel, 0x1); /* 0x2060108 */
    tu.OutWord(&mau_reg_map.dp.phv_fifo_enable, 0x4); /* 0x2060120 */
    tu.OutWord(&mau_reg_map.dp.imem_table_addr_egress, 0x8000); /* 0x2060100 */
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
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_mode[15], 0x7d98000); /* 0x20407fc */
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_table_map[0], 0x8000); /* 0x2040780 */
    tu.OutWord(&mau_reg_map.tcams.tcam_match_adr_shift[0], 0x4); /* 0x2040420 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.unitram_config[0][3], 0x3c5e); /* 0x201c398 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.unitram_config[0][3], 0x3d1a); /* 0x201eb98 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[3].unit_ram_ctl, 0x200); /* 0x2008198 */
    tu.OutWord(&mau_reg_map.rams.array.row[5].ram[3].unit_ram_ctl, 0x20); /* 0x200d198 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tind_ram_data_size[0], 0x5); /* 0x2016500 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][0], 0x4); /* 0x2040380 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][0], 0x2); /* 0x2040200 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][1], 0x1d); /* 0x2040384 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][1], 0x2); /* 0x2040204 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][0], 0xc); /* 0x2040300 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][0], 0x5); /* 0x2040000 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][1], 0x0); /* 0x2040004 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][2], 0x0); /* 0x2040008 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][3], 0x0); /* 0x204000c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][4], 0x7); /* 0x2040010 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][5], 0x4); /* 0x2040014 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][6], 0xa); /* 0x2040018 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][7], 0x8); /* 0x204001c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][0], 0x8); /* 0x20403c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][0], 0x0); /* 0x2040240 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][1], 0x13); /* 0x20403c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][1], 0x3); /* 0x2040244 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][0], 0xd); /* 0x2040320 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][0], 0x0); /* 0x2040100 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][1], 0x6); /* 0x2040104 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][2], 0x2); /* 0x2040108 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][3], 0x2); /* 0x204010c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][4], 0x9); /* 0x2040110 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][5], 0x8); /* 0x2040114 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][6], 0x6); /* 0x2040118 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][7], 0x9); /* 0x204011c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][2], 0x2); /* 0x2040388 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][2], 0x0); /* 0x2040208 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][3], 0x6); /* 0x204038c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][3], 0x0); /* 0x204020c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][1], 0x7); /* 0x2040304 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][0], 0x2); /* 0x2040020 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][1], 0x3); /* 0x2040024 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][2], 0x7); /* 0x2040028 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][3], 0x6); /* 0x204002c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][4], 0x2); /* 0x2040030 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][5], 0x5); /* 0x2040034 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][6], 0x3); /* 0x2040038 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][7], 0x2); /* 0x204003c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][2], 0x14); /* 0x20403c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][2], 0x3); /* 0x2040248 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][3], 0xd); /* 0x20403cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][3], 0x3); /* 0x204024c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][1], 0xb); /* 0x2040324 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][0], 0x6); /* 0x2040120 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][1], 0x8); /* 0x2040124 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][2], 0x6); /* 0x2040128 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][3], 0xa); /* 0x204012c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][4], 0x5); /* 0x2040130 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][5], 0x5); /* 0x2040134 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][6], 0x7); /* 0x2040138 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][7], 0x8); /* 0x204013c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][4], 0x5); /* 0x2040390 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][4], 0x3); /* 0x2040210 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][5], 0x2); /* 0x2040394 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][5], 0x0); /* 0x2040214 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][2], 0xd); /* 0x2040308 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][0], 0x6); /* 0x2040040 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][1], 0x3); /* 0x2040044 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][2], 0x3); /* 0x2040048 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][3], 0x7); /* 0x204004c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][4], 0x7); /* 0x2040050 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][5], 0x7); /* 0x2040054 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][6], 0xa); /* 0x2040058 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][7], 0x3); /* 0x204005c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][4], 0x1c); /* 0x20403d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][4], 0x3); /* 0x2040250 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][5], 0x16); /* 0x20403d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][5], 0x3); /* 0x2040254 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][2], 0xd); /* 0x2040328 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][0], 0xa); /* 0x2040140 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][1], 0x8); /* 0x2040144 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][2], 0x8); /* 0x2040148 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][3], 0x0); /* 0x204014c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][4], 0x1); /* 0x2040150 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][5], 0x0); /* 0x2040154 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][6], 0xa); /* 0x2040158 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][7], 0x5); /* 0x204015c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][6], 0xf); /* 0x2040398 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][6], 0x3); /* 0x2040218 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][7], 0x1f); /* 0x204039c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][7], 0x2); /* 0x204021c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][3], 0x8); /* 0x204030c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][0], 0x1); /* 0x2040060 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][1], 0x1); /* 0x2040064 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][2], 0x5); /* 0x2040068 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][3], 0x1); /* 0x204006c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][4], 0x2); /* 0x2040070 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][5], 0x3); /* 0x2040074 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][6], 0x6); /* 0x2040078 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][7], 0x1); /* 0x204007c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][6], 0xc); /* 0x20403d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][6], 0x3); /* 0x2040258 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][7], 0x1b); /* 0x20403dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][7], 0x2); /* 0x204025c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][3], 0x0); /* 0x204032c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][0], 0x0); /* 0x2040160 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][1], 0x0); /* 0x2040164 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][2], 0x7); /* 0x2040168 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][3], 0x9); /* 0x204016c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][4], 0x9); /* 0x2040170 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][5], 0x0); /* 0x2040174 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][6], 0x0); /* 0x2040178 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][7], 0x7); /* 0x204017c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][8], 0xf); /* 0x20403a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][8], 0x2); /* 0x2040220 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][9], 0x10); /* 0x20403a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][9], 0x3); /* 0x2040224 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][4], 0xc); /* 0x2040310 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][0], 0x6); /* 0x2040080 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][1], 0x6); /* 0x2040084 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][2], 0x7); /* 0x2040088 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][3], 0x0); /* 0x204008c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][4], 0x2); /* 0x2040090 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][5], 0x1); /* 0x2040094 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][6], 0x6); /* 0x2040098 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][7], 0x2); /* 0x204009c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][8], 0x6); /* 0x20403e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][8], 0x0); /* 0x2040260 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][9], 0xd); /* 0x20403e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][9], 0x1); /* 0x2040264 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][4], 0xc); /* 0x2040330 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][0], 0x8); /* 0x2040180 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][1], 0x4); /* 0x2040184 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][2], 0x7); /* 0x2040188 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][3], 0x6); /* 0x204018c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][4], 0x3); /* 0x2040190 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][5], 0x9); /* 0x2040194 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][6], 0x7); /* 0x2040198 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][7], 0x9); /* 0x204019c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][10], 0x13); /* 0x20403a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][10], 0x0); /* 0x2040228 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][11], 0x7); /* 0x20403ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][11], 0x0); /* 0x204022c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][5], 0xd); /* 0x2040314 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][0], 0x1); /* 0x20400a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][1], 0x8); /* 0x20400a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][2], 0x0); /* 0x20400a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][3], 0x0); /* 0x20400ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][4], 0x3); /* 0x20400b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][5], 0x1); /* 0x20400b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][6], 0x5); /* 0x20400b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][7], 0x9); /* 0x20400bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][10], 0x7); /* 0x20403e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][10], 0x3); /* 0x2040268 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][11], 0x14); /* 0x20403ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][11], 0x1); /* 0x204026c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][5], 0x3); /* 0x2040334 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][0], 0x8); /* 0x20401a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][1], 0x9); /* 0x20401a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][2], 0x0); /* 0x20401a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][3], 0xa); /* 0x20401ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][4], 0x4); /* 0x20401b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][5], 0x9); /* 0x20401b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][6], 0x0); /* 0x20401b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][7], 0x0); /* 0x20401bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][12], 0x12); /* 0x20403b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][12], 0x1); /* 0x2040230 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][13], 0x9); /* 0x20403b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][13], 0x0); /* 0x2040234 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][6], 0x2); /* 0x2040318 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][0], 0x6); /* 0x20400c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][1], 0x3); /* 0x20400c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][2], 0x5); /* 0x20400c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][3], 0x8); /* 0x20400cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][4], 0xa); /* 0x20400d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][5], 0x6); /* 0x20400d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][6], 0xa); /* 0x20400d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][7], 0x9); /* 0x20400dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][12], 0x5); /* 0x20403f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][12], 0x1); /* 0x2040270 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][13], 0x8); /* 0x20403f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][13], 0x1); /* 0x2040274 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][6], 0xa); /* 0x2040338 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][0], 0x8); /* 0x20401c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][1], 0x1); /* 0x20401c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][2], 0x4); /* 0x20401c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][3], 0x3); /* 0x20401cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][4], 0x6); /* 0x20401d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][5], 0x8); /* 0x20401d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][6], 0xa); /* 0x20401d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][7], 0xa); /* 0x20401dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][14], 0x6); /* 0x20403b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][14], 0x0); /* 0x2040238 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][15], 0x1a); /* 0x20403bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][15], 0x3); /* 0x204023c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][7], 0xc); /* 0x204031c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][0], 0x5); /* 0x20400e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][1], 0x1); /* 0x20400e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][2], 0x2); /* 0x20400e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][3], 0x2); /* 0x20400ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][4], 0xa); /* 0x20400f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][5], 0x6); /* 0x20400f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][6], 0x7); /* 0x20400f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][7], 0x9); /* 0x20400fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][14], 0x6); /* 0x20403f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][14], 0x2); /* 0x2040278 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][15], 0x1c); /* 0x20403fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][15], 0x1); /* 0x204027c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][7], 0x2); /* 0x204033c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][0], 0x1); /* 0x20401e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][1], 0x8); /* 0x20401e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][2], 0x9); /* 0x20401e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][3], 0xa); /* 0x20401ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][4], 0x9); /* 0x20401f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][5], 0x9); /* 0x20401f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][6], 0x0); /* 0x20401f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][7], 0x7); /* 0x20401fc */
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_table_prop[0], 0x6); /* 0x2014a00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tind_bus_prop[0], 0x6); /* 0x2014800 */
  //    tu.OutWord(&mau_reg_map.rams.match.merge.exact_match_delay_config, 0x50000); /* 0x2016490 */ // REMOVED EMDEL070915
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_hit_to_logical_table_ixbar_outputmap[0], 0x1f); /* 0x2014860 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_match_adr_to_physical_oxbar_outputmap[0], 0x8); /* 0x20164c0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.match_to_logical_table_ixbar_outputmap[1][0], 0x1f); /* 0x2014ac0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_tcam_shiftcount[0], 0x8); /* 0x2016600 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_mask[1][0], 0x3f); /* 0x2014d40 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_tcam_shiftcount[0], 0x0); /* 0x2016580 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_mask[1][0], 0xffffffff); /* 0x2014bc0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.next_table_format_data[15], 0x100); /* 0x201657c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_tcam_shiftcount[0], 0x2f); /* 0x2016680 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_mask[1][0], 0x3fffff); /* 0x2014ec0 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.immediate_data_32b_ixbar_ctl[15], 0x20); /* 0x20103bc */
    tu.OutWord(&mau_reg_map.rams.match.merge.predication_ctl[1], 0x8000); /* 0x2014a28 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[ 0].vh_xbars.adr_dist_tind_adr_xbar_ctl[0], 0x8); /* 0x201c530 */ 
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][3], 0x100); /* 0x201c318 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.ram_address_mux_ctl[0][3], 0x80); /* 0x201eb18 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.adr_dist_action_data_adr_icxbar_ctl[15], 0x400); /* 0x20103fc */
  ActionHvTranslator act_hv_translator;
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x1); /* 0x200d880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(5,0,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x3); /* 0x200d888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(5,0,0x3); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x3); /* 0x200d880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(5,0,0x3); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0xf); /* 0x200d888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(5,0,0xf); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x7); /* 0x200d880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(5,0,0x7); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x3f); /* 0x200d888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(5,0,0x3f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0xf); /* 0x200d880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(5,0,0xf); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0xff); /* 0x200d888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(5,0,0xff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x1f); /* 0x200d880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(5,0,0x1f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x3ff); /* 0x200d888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(5,0,0x3ff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x3f); /* 0x200d880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(5,0,0x3f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0xfff); /* 0x200d888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(5,0,0xfff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x7f); /* 0x200d880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(5,0,0x7f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x3fff); /* 0x200d888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(5,0,0x3fff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0xff); /* 0x200d880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(5,0,0xff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0xffff); /* 0x200d888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(5,0,0xffff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_word[0][0], 0x1); /* 0x200d800 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(5,0,0,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_word[0][0], 0x1); /* 0x200d800 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(5,0,0,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_word[0][0], 0x1); /* 0x200d800 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(5,0,0,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_word[0][0], 0x1); /* 0x200d800 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(5,0,0,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_word[0][0], 0x5); /* 0x200d800 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(5,0,0,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_word[0][0], 0x5); /* 0x200d800 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(5,0,0,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_word[0][0], 0x5); /* 0x200d800 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(5,0,0,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[5].action_hv_xbar.action_hv_xbar_ctl_word[0][0], 0x5); /* 0x200d800 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(5,0,0,0x5); // ADDED ACHV070915
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
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][22], RM_B4_32(0x269de361)); /* 0x207c058 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][14], RM_B4_32(0x2633e311)); /* 0x207c0b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][17], RM_B4_32(0xda5e30c)); /* 0x207c144 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][2], RM_B4_32(0x12a5e0a5)); /* 0x207c188 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][10], RM_B4_32(0x943e2dc)); /* 0x207c228 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][16], RM_B4_32(0xbc7e308)); /* 0x207c2c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][29], RM_B4_32(0x4c9e00a)); /* 0x207c374 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][26], RM_B4_32(0x2181e054)); /* 0x207c3e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][1], RM_B4_32(0x12f7e11d)); /* 0x207c404 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][26], RM_B4_32(0x28e1e0d9)); /* 0x207c4e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][23], RM_B4_32(0x2275e1b3)); /* 0x207c55c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][31], RM_B4_32(0x1225e38f)); /* 0x207c5fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][19], RM_B4_32(0x219be0c5)); /* 0x207c64c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][18], RM_B4_32(0xb07e310)); /* 0x207c6c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][1], RM_B4_32(0x16e1e206)); /* 0x207c704 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][7], RM_B4_32(0x3e71e136)); /* 0x207c79c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][5], RM_B4_32(0x3ad1e157)); /* 0x207c814 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][28], RM_B4_32(0x3dd5e2d0)); /* 0x207c8f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][2], RM_B4_32(0x27a3e2d1)); /* 0x207c908 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][13], RM_B4_32(0x1033e0fb)); /* 0x207c9b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][26], RM_B4_32(0x1af3e32c)); /* 0x207ca68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][2], RM_B4_32(0x1e51e2e4)); /* 0x207ca88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][29], RM_B4_32(0x3eade2df)); /* 0x207cb74 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][30], RM_B4_32(0x1419e2e8)); /* 0x207cbf8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][19], RM_B4_32(0x1bc7e32a)); /* 0x207cc4c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][24], RM_B4_32(0x1603e007)); /* 0x207cce0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][3], RM_B4_32(0x19bde239)); /* 0x207cd0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][13], RM_B4_32(0x2d5be146)); /* 0x207cdb4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][30], RM_B4_32(0xec9e33c)); /* 0x207ce78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][15], RM_B4_32(0x307de024)); /* 0x207cebc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][13], RM_B4_32(0x18f9e0aa)); /* 0x207cf34 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][21], RM_B4_32(0x3c87e2c7)); /* 0x207cfd4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][10], RM_B4_32(0xb5de287)); /* 0x207d028 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][6], RM_B4_32(0xd91e3a8)); /* 0x207d098 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][16], RM_B4_32(0x1f77e302)); /* 0x207d140 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][12], RM_B4_32(0x3499e1c0)); /* 0x207d1b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][0], RM_B4_32(0x1e55e14f)); /* 0x207d200 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][10], RM_B4_32(0x3201e279)); /* 0x207d2a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][5], RM_B4_32(0x3c43e06b)); /* 0x207d314 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][19], RM_B4_32(0x115be1aa)); /* 0x207d3cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][19], RM_B4_32(0x715e14d)); /* 0x207d44c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][30], RM_B4_32(0x3cd9e3ef)); /* 0x207d4f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][7], RM_B4_32(0x3e25e258)); /* 0x207d51c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][4], RM_B4_32(0x921e118)); /* 0x207d590 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][6], RM_B4_32(0x35c7e031)); /* 0x207d618 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][5], RM_B4_32(0xe21e011)); /* 0x207d694 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][25], RM_B4_32(0x2b51e15b)); /* 0x207d764 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][18], RM_B4_32(0x2899e0d5)); /* 0x207d7c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][11], RM_B4_32(0x5d1e203)); /* 0x207d82c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][31], RM_B4_32(0x779e195)); /* 0x207d8fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][17], RM_B4_32(0x3f6de389)); /* 0x207d944 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][11], RM_B4_32(0x3021e1cb)); /* 0x207d9ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][30], RM_B4_32(0x4be0ec)); /* 0x207da78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][28], RM_B4_32(0x35e3e38a)); /* 0x207daf0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][30], RM_B4_32(0x31be079)); /* 0x207db78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][28], RM_B4_32(0xed1e1ea)); /* 0x207dbf0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][30], RM_B4_32(0x653e156)); /* 0x207dc78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][23], RM_B4_32(0x3551e2aa)); /* 0x207dcdc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][9], RM_B4_32(0x3f7e0b2)); /* 0x207dd24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][19], RM_B4_32(0x1677e111)); /* 0x207ddcc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][31], RM_B4_32(0x309be1fa)); /* 0x207de7c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][13], RM_B4_32(0x394fe37c)); /* 0x207deb4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][7], RM_B4_32(0x2fdbe2f7)); /* 0x207df1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][8], RM_B4_32(0x1923e0c5)); /* 0x207dfa0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[0][11], RM_B4_16(0x389e008)); /* 0x207802c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[1][24], RM_B4_16(0x2c3e3c1)); /* 0x20780e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[2][21], RM_B4_16(0x505e2f3)); /* 0x2078154 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[3][20], RM_B4_16(0x16de242)); /* 0x20781d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[4][15], RM_B4_16(0x10fe1d9)); /* 0x207823c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[5][22], RM_B4_16(0x451e073)); /* 0x20782d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[6][28], RM_B4_16(0x5dfe1cb)); /* 0x2078370 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[7][16], RM_B4_16(0x5c7e348)); /* 0x20783c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[8][27], RM_B4_16(0x401e2d1)); /* 0x207846c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[9][3], RM_B4_16(0x44de1c1)); /* 0x207848c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[10][20], RM_B4_16(0x739e1eb)); /* 0x2078550 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[11][8], RM_B4_16(0x2b7e1c3)); /* 0x20785a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[12][22], RM_B4_16(0x295e2de)); /* 0x2078658 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[13][1], RM_B4_16(0x263e0d6)); /* 0x2078684 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[14][22], RM_B4_16(0xf9e17b)); /* 0x2078758 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[15][23], RM_B4_16(0x3b9e271)); /* 0x20787dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[16][3], RM_B4_16(0x4d9e2b4)); /* 0x207880c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[17][12], RM_B4_16(0x153e000)); /* 0x20788b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[18][25], RM_B4_16(0x343e26a)); /* 0x2078964 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[19][15], RM_B4_16(0x725e258)); /* 0x20789bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[20][21], RM_B4_16(0x6cbe228)); /* 0x2078a54 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[21][21], RM_B4_16(0x41be1b5)); /* 0x2078ad4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[22][8], RM_B4_16(0x175e190)); /* 0x2078b20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[23][8], RM_B4_16(0x221e32f)); /* 0x2078ba0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[24][2], RM_B4_16(0x759e199)); /* 0x2078c08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[25][28], RM_B4_16(0x127e298)); /* 0x2078cf0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[26][30], RM_B4_16(0xede2ff)); /* 0x2078d78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[27][5], RM_B4_16(0x399e099)); /* 0x2078d94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[28][28], RM_B4_16(0x1fde020)); /* 0x2078e70 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[29][30], RM_B4_16(0x603e3cf)); /* 0x2078ef8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[30][29], RM_B4_16(0x577e188)); /* 0x2078f74 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[31][26], RM_B4_16(0x23de1d5)); /* 0x2078fe8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[32][1], RM_B4_16(0x207e0be)); /* 0x2079004 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[33][14], RM_B4_16(0x64fe00d)); /* 0x20790b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[34][18], RM_B4_16(0x61e3d1)); /* 0x2079148 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[35][15], RM_B4_16(0x11fe1d4)); /* 0x20791bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[36][22], RM_B4_16(0x18fe332)); /* 0x2079258 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[37][5], RM_B4_16(0x6f5e2df)); /* 0x2079294 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[38][10], RM_B4_16(0x569e1ed)); /* 0x2079328 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[39][18], RM_B4_16(0x5fe3c8)); /* 0x20793c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[40][16], RM_B4_16(0x4fe167)); /* 0x2079440 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[41][4], RM_B4_16(0x5f9e221)); /* 0x2079490 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[42][25], RM_B4_16(0x31be3b7)); /* 0x2079564 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[43][0], RM_B4_16(0xa7e2bc)); /* 0x2079580 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[44][11], RM_B4_16(0x40be351)); /* 0x207962c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[45][27], RM_B4_16(0x719e289)); /* 0x20796ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[46][30], RM_B4_16(0x6cde159)); /* 0x2079778 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[47][0], RM_B4_16(0x17de1d0)); /* 0x2079780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[48][28], RM_B4_16(0x7bde1fc)); /* 0x2079870 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[49][25], RM_B4_16(0x41e33d)); /* 0x20798e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[50][28], RM_B4_16(0x68be27c)); /* 0x2079970 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[51][8], RM_B4_16(0x333e24f)); /* 0x20799a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[52][11], RM_B4_16(0x2e3e077)); /* 0x2079a2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[53][9], RM_B4_16(0x5a7e082)); /* 0x2079aa4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[54][3], RM_B4_16(0x473e1a6)); /* 0x2079b0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[55][4], RM_B4_16(0x35be1e8)); /* 0x2079b90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[56][15], RM_B4_16(0x467e1d8)); /* 0x2079c3c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[57][26], RM_B4_16(0x6bde107)); /* 0x2079ce8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[58][16], RM_B4_16(0x7f9e2dd)); /* 0x2079d40 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[59][12], RM_B4_16(0x743e014)); /* 0x2079db0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[60][23], RM_B4_16(0x30de349)); /* 0x2079e5c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[61][30], RM_B4_16(0x25be011)); /* 0x2079ef8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[62][7], RM_B4_16(0x1e7e1bf)); /* 0x2079f1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[63][16], RM_B4_16(0x53e18d)); /* 0x2079fc0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[64][6], RM_B4_16(0x157e255)); /* 0x207a018 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[65][8], RM_B4_16(0x3c1e076)); /* 0x207a0a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[66][24], RM_B4_16(0x57e31e)); /* 0x207a160 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[67][6], RM_B4_16(0x111e04b)); /* 0x207a198 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[68][26], RM_B4_16(0x9e1c9)); /* 0x207a268 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[69][20], RM_B4_16(0x18fe11e)); /* 0x207a2d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[70][30], RM_B4_16(0x67de04b)); /* 0x207a378 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[71][4], RM_B4_16(0x3a9e356)); /* 0x207a390 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[72][0], RM_B4_16(0x21fe2ad)); /* 0x207a400 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[73][21], RM_B4_16(0x7cfe30b)); /* 0x207a4d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[74][26], RM_B4_16(0x281e067)); /* 0x207a568 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[75][4], RM_B4_16(0x4a7e380)); /* 0x207a590 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[76][6], RM_B4_16(0x35fe0c9)); /* 0x207a618 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[77][14], RM_B4_16(0xf9e36b)); /* 0x207a6b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[78][30], RM_B4_16(0x235e0c2)); /* 0x207a778 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[79][12], RM_B4_16(0x6f3e1d5)); /* 0x207a7b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[80][30], RM_B4_16(0x727e085)); /* 0x207a878 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[81][19], RM_B4_16(0x71de21e)); /* 0x207a8cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[82][15], RM_B4_16(0x2c5e2b4)); /* 0x207a93c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[83][7], RM_B4_16(0x745e3b2)); /* 0x207a99c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[84][26], RM_B4_16(0x65be067)); /* 0x207aa68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[85][6], RM_B4_16(0xd5e305)); /* 0x207aa98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[86][1], RM_B4_16(0x7d1e3e3)); /* 0x207ab04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[87][5], RM_B4_16(0x755e07f)); /* 0x207ab94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[88][26], RM_B4_16(0x45fe285)); /* 0x207ac68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[89][12], RM_B4_16(0x3cbe3f2)); /* 0x207acb0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[90][2], RM_B4_16(0x451e357)); /* 0x207ad08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[91][3], RM_B4_16(0x273e279)); /* 0x207ad8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[92][30], RM_B4_16(0x447e03f)); /* 0x207ae78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[93][16], RM_B4_16(0x4b1e348)); /* 0x207aec0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[94][24], RM_B4_16(0x187e017)); /* 0x207af60 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[95][11], RM_B4_16(0x323e02f)); /* 0x207afac */



    Phv *phv_in2 = tu.phv_alloc();
    phv_in2->set_egress();
    phv_in2->set( 17, 0x84422190); 	/* [0,17] v=1  #0# e=1 RefModel iPhv 2o */
    phv_in2->set( 31, 0x84422190); 	/* [0,31] v=1  #0# e=1 RefModel iPhv 2o */
    phv_in2->set( 60, 0x48241209); 	/* [1,28] v=1  #0# e=1 RefModel iPhv 2o */
    phv_in2->set( 64, 0x09); 	/* [2, 0] v=1  #0# e=1 RefModel iPhv 2o */
    phv_in2->set(116, 0x90); 	/* [3,20] v=1  #0# e=1 RefModel iPhv 2o */
    phv_in2->set(145, 0x4824); 	/* [4,17] v=1  #0# e=1 RefModel iPhv 2o */
    phv_in2->set(169, 0x4824); 	/* [5, 9] v=1  #0# e=1 RefModel iPhv 2o */
    phv_in2->set(199, 0x8442); 	/* [6, 7] v=1  #0# e=1 RefModel iPhv 2o */

    


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
    RMT_UT_LOG_INFO("Dv16Test::InPkt=%p [DA=%04X%08X]\n", p_in,
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
    EXPECT_EQ(0xffffu, phv_out2->get(175)); // was 0x0 in input phv

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
