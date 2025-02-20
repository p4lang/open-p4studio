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

// XXX -> test_dv20.cpp
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

  bool dv20_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv20Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv20_print) RMT_UT_LOG_INFO("test_dv20_packet1()\n");
    
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
    tu.set_dv_test(20);
    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
    

    auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);
    tu.OutWordPiT(0, 0, &mau_reg_map.dp.phv_ingress_thread[0][0], 0x90707ea0); /* 0x2060180 */
    tu.OutWordPeT(0, 0, &mau_reg_map.dp.phv_egress_thread[0][0], 0x22828057); /* 0x20601c0 */
    tu.OutWordPiT(0, 1, &mau_reg_map.dp.phv_ingress_thread[0][1], 0x12a1104b); /* 0x2060184 */
    tu.OutWordPeT(0, 1, &mau_reg_map.dp.phv_egress_thread[0][1], 0x4d18ad84); /* 0x20601c4 */
    tu.OutWordPiT(0, 2, &mau_reg_map.dp.phv_ingress_thread[0][2], 0x27c6d829); /* 0x2060188 */
    tu.OutWordPeT(0, 2, &mau_reg_map.dp.phv_egress_thread[0][2], 0x18180392); /* 0x20601c8 */
    tu.OutWordPiT(0, 3, &mau_reg_map.dp.phv_ingress_thread[0][3], 0xffaa17a3); /* 0x206018c */
    tu.OutWordPeT(0, 3, &mau_reg_map.dp.phv_egress_thread[0][3], 0x444004); /* 0x20601cc */
    tu.OutWordPiT(0, 4, &mau_reg_map.dp.phv_ingress_thread[0][4], 0xf9d3eb2a); /* 0x2060190 */
    tu.OutWordPeT(0, 4, &mau_reg_map.dp.phv_egress_thread[0][4], 0x60c1404); /* 0x20601d0 */
    tu.OutWordPiT(0, 5, &mau_reg_map.dp.phv_ingress_thread[0][5], 0x2bda7d22); /* 0x2060194 */
    tu.OutWordPeT(0, 5, &mau_reg_map.dp.phv_egress_thread[0][5], 0x54000259); /* 0x20601d4 */
    tu.OutWordPiT(0, 6, &mau_reg_map.dp.phv_ingress_thread[0][6], 0x3ab3776b); /* 0x2060198 */
    tu.OutWordPeT(0, 6, &mau_reg_map.dp.phv_egress_thread[0][6], 0x40c0010); /* 0x20601d8 */
    tu.OutWordPiT(1, 0, &mau_reg_map.dp.phv_ingress_thread[1][0], 0x90707ea0); /* 0x20601a0 */
    tu.OutWordPeT(1, 0, &mau_reg_map.dp.phv_egress_thread[1][0], 0x22828057); /* 0x20601e0 */
    tu.OutWordPiT(1, 1, &mau_reg_map.dp.phv_ingress_thread[1][1], 0x12a1104b); /* 0x20601a4 */
    tu.OutWordPeT(1, 1, &mau_reg_map.dp.phv_egress_thread[1][1], 0x4d18ad84); /* 0x20601e4 */
    tu.OutWordPiT(1, 2, &mau_reg_map.dp.phv_ingress_thread[1][2], 0x27c6d829); /* 0x20601a8 */
    tu.OutWordPeT(1, 2, &mau_reg_map.dp.phv_egress_thread[1][2], 0x18180392); /* 0x20601e8 */
    tu.OutWordPiT(1, 3, &mau_reg_map.dp.phv_ingress_thread[1][3], 0xffaa17a3); /* 0x20601ac */
    tu.OutWordPeT(1, 3, &mau_reg_map.dp.phv_egress_thread[1][3], 0x444004); /* 0x20601ec */
    tu.OutWordPiT(1, 4, &mau_reg_map.dp.phv_ingress_thread[1][4], 0xf9d3eb2a); /* 0x20601b0 */
    tu.OutWordPeT(1, 4, &mau_reg_map.dp.phv_egress_thread[1][4], 0x60c1404); /* 0x20601f0 */
    tu.OutWordPiT(1, 5, &mau_reg_map.dp.phv_ingress_thread[1][5], 0x2bda7d22); /* 0x20601b4 */
    tu.OutWordPeT(1, 5, &mau_reg_map.dp.phv_egress_thread[1][5], 0x54000259); /* 0x20601f4 */
    tu.OutWordPiT(1, 6, &mau_reg_map.dp.phv_ingress_thread[1][6], 0x3ab3776b); /* 0x20601b8 */
    tu.OutWordPeT(1, 6, &mau_reg_map.dp.phv_egress_thread[1][6], 0x40c0010); /* 0x20601f8 */
    tu.OutWord(&mau_reg_map.dp.action_output_delay[0], 0xa); /* 0x2060030 */
    //Removed in bfnregs 20150107_182406_7982_mau_dev
    //tu.OutWord(&mau_reg_map.dp.final_output_delay[0][0], 0x2); /* 0x2060040 */
    tu.OutWord(&mau_reg_map.dp.action_output_delay[1], 0xd); /* 0x2060038 */
    //Removed in bfnregs 20150107_182406_7982_mau_dev
    //tu.OutWord(&mau_reg_map.dp.final_output_delay[1][0], 0xd); /* 0x2060048 */
    tu.OutWord(&mau_reg_map.dp.pipelength_added_stages[1], 0x3); /* 0x2060078 */
    tu.OutWord(&mau_reg_map.dp.match_ie_input_mux_sel, 0x3); /* 0x2060108 */
    tu.OutWord(&mau_reg_map.dp.phv_fifo_enable, 0xc); /* 0x2060120 */
    tu.OutWord(&mau_reg_map.dp.imem_table_addr_egress, 0x8008); /* 0x2060100 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][0], 0x4e); /* 0x2064800 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][1], 0x64); /* 0x2064804 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][2], 0x14); /* 0x2067008 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][3], 0x14); /* 0x2067c0c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][4], 0x48); /* 0x2064010 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][5], 0x4b); /* 0x2064414 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][6], 0x17); /* 0x2067818 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][7], 0x1f); /* 0x206701c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][8], 0x62); /* 0x2064020 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][9], 0x16); /* 0x2067024 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][10], 0x44); /* 0x2064428 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][11], 0x52); /* 0x206442c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][12], 0x1b); /* 0x2067c30 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][13], 0x44); /* 0x2064434 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][14], 0x44); /* 0x2064038 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][15], 0x1f); /* 0x206783c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][16], 0x14); /* 0x2067040 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][17], 0x12); /* 0x2067044 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][18], 0x11); /* 0x2067048 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][19], 0x5c); /* 0x206484c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][20], 0x12); /* 0x2067050 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][21], 0x5a); /* 0x2064c54 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][22], 0x1d); /* 0x2067458 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][23], 0x1d); /* 0x206745c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][24], 0x12); /* 0x2067060 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][25], 0x43); /* 0x2064464 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][26], 0x62); /* 0x2064c68 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][27], 0x1b); /* 0x206786c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][28], 0x18); /* 0x2067870 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][29], 0x1b); /* 0x2067c74 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][30], 0x62); /* 0x2064c78 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][31], 0x12); /* 0x206707c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][32], 0x44); /* 0x2064480 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][33], 0x62); /* 0x2064084 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][34], 0x13); /* 0x2067c88 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][35], 0x1b); /* 0x2067c8c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][36], 0x5e); /* 0x2064890 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][37], 0x42); /* 0x2064894 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][38], 0x44); /* 0x2064098 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][39], 0x5a); /* 0x2064c9c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][40], 0x5a); /* 0x2064ca0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][41], 0x1e); /* 0x2067ca4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][42], 0x61); /* 0x20644a8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][43], 0x1d); /* 0x20678ac */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][44], 0x51); /* 0x20644b0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][45], 0x1a); /* 0x20678b4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][46], 0x62); /* 0x20640b8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][47], 0x51); /* 0x20644bc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][48], 0x17); /* 0x20674c0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][49], 0x1f); /* 0x20670c4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][50], 0x14); /* 0x20670c8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][51], 0x41); /* 0x20640cc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][52], 0x64); /* 0x20648d0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][53], 0x62); /* 0x2064cd4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][54], 0x42); /* 0x2064cd8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][55], 0x5a); /* 0x2064cdc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][56], 0x48); /* 0x20640e0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][57], 0x42); /* 0x2064ce4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][58], 0x12); /* 0x20670e8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][59], 0x49); /* 0x20640ec */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][60], 0x1a); /* 0x20678f0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][61], 0x17); /* 0x20674f4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][62], 0x42); /* 0x20648f8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][63], 0x62); /* 0x2064cfc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][64], 0x19); /* 0x2067100 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][65], 0x1c); /* 0x2067904 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][66], 0x58); /* 0x2064908 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][67], 0x57); /* 0x206490c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][68], 0x65); /* 0x2064510 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][69], 0x45); /* 0x2064914 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][70], 0x1e); /* 0x2067118 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][71], 0x51); /* 0x206491c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][72], 0x5b); /* 0x2064120 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][73], 0x58); /* 0x2064d24 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][74], 0x66); /* 0x2064128 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][75], 0x41); /* 0x206492c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][76], 0x5d); /* 0x2064d30 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][77], 0x48); /* 0x2064934 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][78], 0x4c); /* 0x2064138 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][79], 0x49); /* 0x206493c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][80], 0x40); /* 0x2064140 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][81], 0x50); /* 0x2064544 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][82], 0x64); /* 0x2064148 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][83], 0x48); /* 0x206454c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][84], 0x4d); /* 0x2064d50 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][85], 0x66); /* 0x2064954 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][86], 0x5c); /* 0x2064d58 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][87], 0x5d); /* 0x2064d5c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][88], 0x1c); /* 0x2067560 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][89], 0x48); /* 0x2064964 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][90], 0x19); /* 0x2067168 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][91], 0x5d); /* 0x206416c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][92], 0x62); /* 0x2064570 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][93], 0x5d); /* 0x2064574 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][94], 0x59); /* 0x2064d78 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][95], 0x43); /* 0x2064d7c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][96], 0x58); /* 0x2064980 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][97], 0x1c); /* 0x2067184 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][98], 0x49); /* 0x2064988 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][99], 0x19); /* 0x206718c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][100], 0x48); /* 0x2064d90 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][101], 0x15); /* 0x2067d94 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][102], 0x63); /* 0x2064d98 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][103], 0x65); /* 0x2064d9c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][104], 0x4c); /* 0x20649a0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][105], 0x15); /* 0x20671a4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][106], 0x45); /* 0x2064da8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][107], 0x4c); /* 0x2064dac */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][108], 0x10); /* 0x20679b0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][109], 0x51); /* 0x20649b4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][110], 0x17); /* 0x20671b8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][111], 0x5f); /* 0x20641bc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][112], 0x46); /* 0x20645c0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][113], 0x59); /* 0x20649c4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][114], 0x66); /* 0x20645c8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][115], 0x4a); /* 0x20645cc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][116], 0x64); /* 0x20645d0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][117], 0x51); /* 0x2064dd4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][118], 0x66); /* 0x20649d8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][119], 0x11); /* 0x20679dc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][120], 0x42); /* 0x20645e0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][121], 0x5b); /* 0x20649e4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][122], 0x56); /* 0x20645e8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][123], 0x16); /* 0x20675ec */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][124], 0x49); /* 0x20645f0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][125], 0x5f); /* 0x20641f4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][126], 0x50); /* 0x2064df8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][127], 0x1f); /* 0x20675fc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][128], 0x1e); /* 0x2067e00 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][129], 0x1e); /* 0x2067e04 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][130], 0x1e); /* 0x2067e08 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][131], 0x1e); /* 0x2067e0c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][132], 0x44); /* 0x2064610 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][133], 0x4c); /* 0x2064614 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][134], 0x4e); /* 0x2064a18 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][135], 0x5e); /* 0x206461c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][136], 0x5e); /* 0x2064620 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][137], 0x44); /* 0x2064224 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][138], 0x4c); /* 0x2064628 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][139], 0x1b); /* 0x2067e2c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][140], 0x1b); /* 0x2067e30 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][141], 0x1b); /* 0x2067e34 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][142], 0x1b); /* 0x2067e38 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][143], 0x46); /* 0x2064e3c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][144], 0x42); /* 0x2064a40 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][145], 0x62); /* 0x2064244 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][146], 0x62); /* 0x2064248 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][147], 0x58); /* 0x206464c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][148], 0x58); /* 0x2064650 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][149], 0x41); /* 0x2064254 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][150], 0x4c); /* 0x2064658 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][151], 0x1a); /* 0x2067a5c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][152], 0x1a); /* 0x2067a60 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][153], 0x1a); /* 0x2067a64 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][154], 0x1a); /* 0x2067a68 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][155], 0x4e); /* 0x2064a6c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][156], 0x47); /* 0x2064270 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][157], 0x5b); /* 0x2064674 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][158], 0x5b); /* 0x2064678 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][159], 0x62); /* 0x206427c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][160], 0x62); /* 0x2064280 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][161], 0x5c); /* 0x2064284 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][162], 0x5c); /* 0x2064288 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][163], 0x4e); /* 0x2064a8c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][164], 0x58); /* 0x2064690 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][165], 0x58); /* 0x2064694 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][166], 0x41); /* 0x2064298 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][167], 0x63); /* 0x206429c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][168], 0x63); /* 0x20642a0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][169], 0x4b); /* 0x20646a4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][170], 0x5b); /* 0x20646a8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][171], 0x5b); /* 0x20646ac */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][172], 0x5a); /* 0x2064eb0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][173], 0x5a); /* 0x2064eb4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][174], 0x44); /* 0x20642b8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][175], 0x62); /* 0x20642bc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][176], 0x62); /* 0x20642c0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][177], 0x44); /* 0x20646c4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][178], 0x5a); /* 0x20642c8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][179], 0x5a); /* 0x20642cc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][180], 0x49); /* 0x20642d0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][181], 0x44); /* 0x20646d4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][182], 0x4c); /* 0x20646d8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][183], 0x1e); /* 0x2067edc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][184], 0x1e); /* 0x2067ee0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][185], 0x1e); /* 0x2067ee4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][186], 0x1e); /* 0x2067ee8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][187], 0x4e); /* 0x2064aec */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][188], 0x4e); /* 0x2064af0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][189], 0x64); /* 0x2064af4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][190], 0x64); /* 0x2064af8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][191], 0x5a); /* 0x2064afc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][192], 0x5a); /* 0x2064b00 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][193], 0x48); /* 0x2064304 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][194], 0x48); /* 0x2064308 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][195], 0x47); /* 0x206430c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][196], 0x5b); /* 0x2064710 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][197], 0x5b); /* 0x2064714 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][198], 0x44); /* 0x2064718 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][199], 0x47); /* 0x206431c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][200], 0x42); /* 0x2064f20 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][201], 0x52); /* 0x2064724 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][202], 0x52); /* 0x2064728 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][203], 0x51); /* 0x206472c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][204], 0x51); /* 0x2064730 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][205], 0x52); /* 0x2064334 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][206], 0x52); /* 0x2064338 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][207], 0x41); /* 0x206433c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][208], 0x62); /* 0x2064340 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][209], 0x62); /* 0x2064344 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][210], 0x4c); /* 0x2064748 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][211], 0x5b); /* 0x206474c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][212], 0x5b); /* 0x2064750 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][213], 0x41); /* 0x2064354 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][214], 0x46); /* 0x2064f58 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][215], 0x48); /* 0x206435c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][2], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][6], 0x17); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][9], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][14], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][16], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][17], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][18], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][20], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][24], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][31], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][37], 0x4a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][38], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][50], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][51], 0x41); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][58], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][62], 0x4a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][66], 0x64); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][67], 0x63); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][69], 0x4d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][71], 0x5d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][72], 0x5b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][75], 0x49); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][80], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][96], 0x64); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][105], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][108], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][109], 0x5d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][110], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][113], 0x65); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][119], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][121], 0x67); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][137], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][144], 0x4a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][149], 0x41); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][156], 0x47); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][166], 0x41); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][174], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][178], 0x5a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][179], 0x5a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][191], 0x66); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][192], 0x66); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][195], 0x47); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][0], 0x4e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][1], 0x64); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][4], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][7], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][8], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][15], 0x17); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][19], 0x5c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][27], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][28], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][33], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][36], 0x5e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][43], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][45], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][46], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][49], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][52], 0x64); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][56], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][59], 0x41); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][60], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][64], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][65], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][70], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][74], 0x5a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][77], 0x48); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][78], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][79], 0x49); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][82], 0x58); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][85], 0x66); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][89], 0x48); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][90], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][91], 0x51); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][97], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][98], 0x49); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][99], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][104], 0x4c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][111], 0x53); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][118], 0x66); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][125], 0x53); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][134], 0x4e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][145], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][146], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][151], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][152], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][153], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][154], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][155], 0x4e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][159], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][160], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][161], 0x50); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][162], 0x50); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][163], 0x4e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][167], 0x57); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][168], 0x57); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][175], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][176], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][180], 0x41); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][187], 0x4e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][188], 0x4e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][189], 0x64); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][190], 0x64); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][193], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][194], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][3], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][10], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][11], 0x52); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][13], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][21], 0x66); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][25], 0x43); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][32], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][34], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][39], 0x66); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][40], 0x66); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][44], 0x51); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][47], 0x51); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][48], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][54], 0x4a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][55], 0x66); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][57], 0x4a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][61], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][73], 0x64); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][81], 0x50); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][94], 0x65); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][95], 0x4b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][101], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][106], 0x4d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][112], 0x46); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][117], 0x5d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][120], 0x42); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][122], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][123], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][126], 0x5c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][132], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][143], 0x4e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][147], 0x58); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][148], 0x58); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][157], 0x5b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][158], 0x5b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][164], 0x58); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][165], 0x58); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][170], 0x5b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][171], 0x5b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][172], 0x66); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][173], 0x66); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][177], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][181], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][5], 0x43); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][12], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][22], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][23], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][26], 0x62); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][29], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][30], 0x62); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][35], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][41], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][42], 0x55); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][53], 0x62); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][63], 0x62); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][68], 0x59); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][76], 0x5d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][83], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][84], 0x4d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][86], 0x5c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][87], 0x5d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][88], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][92], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][93], 0x51); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][100], 0x48); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][102], 0x63); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][103], 0x65); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][107], 0x4c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][114], 0x5a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][115], 0x42); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][116], 0x58); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][124], 0x41); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][127], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][128], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][129], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][130], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][131], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][133], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][135], 0x52); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][136], 0x52); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][138], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][139], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][140], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][141], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][142], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][150], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][169], 0x43); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][182], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][183], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][184], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][185], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][186], 0x16); // regs_31841 fix
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[7], 0x1); /* 0x206601c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[8], 0x3); /* 0x2066020 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[11], 0x1); /* 0x206602c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[12], 0x1); /* 0x2066030 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[13], 0x1); /* 0x2066034 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[14], 0x1); /* 0x2066038 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[17], 0x1); /* 0x2066044 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[18], 0x3); /* 0x2066048 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[19], 0x1); /* 0x206604c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[20], 0x3); /* 0x2066050 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[23], 0x1); /* 0x206605c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[24], 0x1); /* 0x2066060 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[25], 0x1); /* 0x2066064 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[26], 0x1); /* 0x2066068 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[29], 0x1); /* 0x2066074 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[30], 0x3); /* 0x2066078 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[31], 0x1); /* 0x206607c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[32], 0x3); /* 0x2066080 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[33], 0x1); /* 0x2066084 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[34], 0x3); /* 0x2066088 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[39], 0x1); /* 0x206609c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[40], 0x3); /* 0x20660a0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[47], 0x1); /* 0x20660bc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[48], 0x3); /* 0x20660c0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[55], 0x1); /* 0x20660dc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[56], 0x1); /* 0x20660e0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[57], 0x1); /* 0x20660e4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[58], 0x1); /* 0x20660e8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[61], 0x1); /* 0x20660f4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[62], 0x3); /* 0x20660f8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[63], 0x1); /* 0x20660fc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[64], 0x3); /* 0x2066100 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[73], 0x1); /* 0x2066124 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[74], 0x3); /* 0x2066128 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[75], 0x1); /* 0x206612c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[76], 0x3); /* 0x2066130 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[77], 0x1); /* 0x2066134 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[78], 0x3); /* 0x2066138 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[83], 0x1); /* 0x206614c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[84], 0x3); /* 0x2066150 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[1], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[2], 0x43); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[3], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[4], 0x74); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[5], 0x43); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[6], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[7], 0x74); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[8], 0x37); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[9], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[10], 0x3); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[11], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[12], 0x3); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[13], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[14], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[15], 0x74); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.tswizzle.tcam_byte_swizzle_ctl[16], 0x3); // regs_31841 fix
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_mode[0], 0x1c0b000); /* 0x20406c0 */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_mode[1], 0x1c18000); /* 0x20406c4 */
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_mode[14], 0x1c38000); /* 0x20407f8 */
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_mode[15], 0x1c2b000); /* 0x20407fc */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_mode[6], 0x1c4b000); /* 0x20406d8 */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_mode[7], 0x1c58000); /* 0x20406dc */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_mode[8], 0x7819000); /* 0x20406e0 */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_mode[9], 0x780a000); /* 0x20406e4 */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_mode[10], 0x780b000); /* 0x20406e8 */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_mode[11], 0x780a000); /* 0x20406ec */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_mode[12], 0x780a000); /* 0x20406f0 */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_mode[13], 0x780a000); /* 0x20406f4 */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_mode[14], 0x780b000); /* 0x20406f8 */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_mode[15], 0x780a000); /* 0x20406fc */
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_mode[0], 0x782a000); /* 0x20407c0 */
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_mode[1], 0x782b000); /* 0x20407c4 */
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_mode[2], 0x782b000); /* 0x20407c8 */
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_mode[3], 0x782b000); /* 0x20407cc */
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_mode[4], 0x782a000); /* 0x20407d0 */
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_mode[5], 0x782a000); /* 0x20407d4 */
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_mode[6], 0x782a000); /* 0x20407d8 */
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_mode[7], 0x7839000); /* 0x20407dc */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_table_map[0], 0x82); /* 0x2040680 */
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_table_map[0], 0x4000); /* 0x2040780 */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_table_map[1], 0x100); /* 0x2040684 */
    tu.OutWord(&mau_reg_map.tcams.col[1].tcam_table_map[1], 0x80); /* 0x2040784 */
    tu.OutWord(&mau_reg_map.tcams.tcam_output_table_thread[0], 0x2); /* 0x2040400 */
    tu.OutWord(&mau_reg_map.tcams.tcam_output_table_thread[1], 0x2); /* 0x2040404 */
    tu.OutWord(&mau_reg_map.tcams.tcam_match_adr_shift[0], 0x1); /* 0x2040460 */
    tu.OutWord(&mau_reg_map.tcams.tcam_piped, 0x2); /* 0x2040444 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.unitram_config[1][2], 0x3ca2); /* 0x2008fd0 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.unitram_config[0][5], 0xc0e); /* 0x200afa8 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.unitram_config[0][5], 0x3c06); /* 0x200cfa8 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.unitram_config[1][0], 0xc0a); /* 0x200ffc0 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[8].unit_ram_ctl, 0x20); /* 0x2038418 */
    tu.OutWord(&mau_reg_map.rams.array.row[2].ram[5].unit_ram_ctl, 0x200); /* 0x203a298 */
    tu.OutWord(&mau_reg_map.rams.array.row[4].ram[5].unit_ram_ctl, 0x200); /* 0x203c298 */
    tu.OutWord(&mau_reg_map.rams.array.row[7].ram[6].unit_ram_ctl, 0x20); /* 0x203f318 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tind_ram_data_size[4], 0x2); /* 0x2026150 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tind_ram_data_size[8], 0x1); /* 0x2026160 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][0], 0xe); /* 0x2040300 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][0], 0x4); /* 0x2040000 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][1], 0x4); /* 0x2040004 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][2], 0x5); /* 0x2040008 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][3], 0x0); /* 0x204000c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][4], 0x8); /* 0x2040010 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][5], 0x5); /* 0x2040014 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][6], 0x9); /* 0x2040018 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][7], 0xa); /* 0x204001c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][0], 0x17); /* 0x2040380 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][0], 0x4); /* 0x2040200 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][1], 0x11); /* 0x2040384 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][1], 0x4); /* 0x2040204 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][0], 0x9); /* 0x2040320 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][0], 0x4); /* 0x2040100 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][1], 0x1); /* 0x2040104 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][2], 0x2); /* 0x2040108 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][3], 0x2); /* 0x204010c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][4], 0xa); /* 0x2040110 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][5], 0x6); /* 0x2040114 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][6], 0xa); /* 0x2040118 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][7], 0x8); /* 0x204011c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][0], 0x19); /* 0x20403c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][0], 0x4); /* 0x2040240 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][1], 0x14); /* 0x20403c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][1], 0x5); /* 0x2040244 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][1], 0xf); /* 0x2040304 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][0], 0x5); /* 0x2040020 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][1], 0x1); /* 0x2040024 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][2], 0x1); /* 0x2040028 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][3], 0x0); /* 0x204002c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][4], 0x9); /* 0x2040030 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][5], 0x6); /* 0x2040034 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][6], 0x5); /* 0x2040038 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][7], 0x8); /* 0x204003c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][2], 0x11); /* 0x2040388 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][2], 0x4); /* 0x2040208 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][3], 0x0); /* 0x204038c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][3], 0x7); /* 0x204020c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][1], 0x8); /* 0x2040324 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][0], 0x1); /* 0x2040120 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][1], 0x5); /* 0x2040124 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][2], 0x4); /* 0x2040128 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][3], 0x5); /* 0x204012c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][4], 0x6); /* 0x2040130 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][5], 0x5); /* 0x2040134 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][6], 0x5); /* 0x2040138 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][7], 0x9); /* 0x204013c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][2], 0x1b); /* 0x20403c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][2], 0x6); /* 0x2040248 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][3], 0x10); /* 0x20403cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][3], 0x6); /* 0x204024c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][2], 0xf); /* 0x2040308 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][0], 0x4); /* 0x2040040 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][1], 0x2); /* 0x2040044 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][2], 0x5); /* 0x2040048 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][3], 0x0); /* 0x204004c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][4], 0x8); /* 0x2040050 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][5], 0x8); /* 0x2040054 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][6], 0x6); /* 0x2040058 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][7], 0xa); /* 0x204005c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][4], 0x10); /* 0x2040390 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][4], 0x6); /* 0x2040210 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][5], 0x1c); /* 0x2040394 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][5], 0x6); /* 0x2040214 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][2], 0x5); /* 0x2040328 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][0], 0x4); /* 0x2040140 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][1], 0x0); /* 0x2040144 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][2], 0x5); /* 0x2040148 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][3], 0x4); /* 0x204014c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][4], 0x5); /* 0x2040150 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][5], 0xa); /* 0x2040154 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][6], 0xa); /* 0x2040158 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][7], 0x8); /* 0x204015c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][4], 0x1c); /* 0x20403d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][4], 0x5); /* 0x2040250 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][5], 0x9); /* 0x20403d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][5], 0x5); /* 0x2040254 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][3], 0xf); /* 0x204030c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][0], 0x4); /* 0x2040060 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][1], 0x0); /* 0x2040064 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][2], 0x1); /* 0x2040068 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][3], 0x4); /* 0x204006c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][4], 0x6); /* 0x2040070 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][5], 0x6); /* 0x2040074 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][6], 0x5); /* 0x2040078 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][7], 0x8); /* 0x204007c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][6], 0x1c); /* 0x2040398 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][6], 0x6); /* 0x2040218 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][7], 0x11); /* 0x204039c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][7], 0x5); /* 0x204021c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][3], 0xf); /* 0x204032c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][0], 0x2); /* 0x2040160 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][1], 0x2); /* 0x2040164 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][2], 0x2); /* 0x2040168 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][3], 0x5); /* 0x204016c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][4], 0x5); /* 0x2040170 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][5], 0x9); /* 0x2040174 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][6], 0x5); /* 0x2040178 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][7], 0x9); /* 0x204017c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][6], 0x19); /* 0x20403d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][6], 0x5); /* 0x2040258 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][7], 0x18); /* 0x20403dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][7], 0x5); /* 0x204025c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][4], 0x9); /* 0x2040310 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][0], 0x3); /* 0x2040080 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][1], 0x3); /* 0x2040084 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][2], 0x0); /* 0x2040088 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][3], 0x1); /* 0x204008c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][4], 0x9); /* 0x2040090 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][5], 0x9); /* 0x2040094 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][6], 0x5); /* 0x2040098 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][7], 0x5); /* 0x204009c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][8], 0xb); /* 0x20403a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][8], 0x6); /* 0x2040220 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][9], 0x14); /* 0x20403a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][9], 0x5); /* 0x2040224 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][4], 0xe); /* 0x2040330 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][0], 0x1); /* 0x2040180 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][1], 0x2); /* 0x2040184 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][2], 0x2); /* 0x2040188 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][3], 0x5); /* 0x204018c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][4], 0x8); /* 0x2040190 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][5], 0xa); /* 0x2040194 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][6], 0xa); /* 0x2040198 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][7], 0x7); /* 0x204019c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][8], 0x17); /* 0x20403e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][8], 0x7); /* 0x2040260 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][9], 0xc); /* 0x20403e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][9], 0x7); /* 0x2040264 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][5], 0x8); /* 0x2040314 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][0], 0x3); /* 0x20400a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][1], 0x1); /* 0x20400a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][2], 0x4); /* 0x20400a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][3], 0x3); /* 0x20400ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][4], 0xa); /* 0x20400b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][5], 0x7); /* 0x20400b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][6], 0x5); /* 0x20400b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][7], 0x7); /* 0x20400bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][10], 0x9); /* 0x20403a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][10], 0x6); /* 0x2040228 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][11], 0x10); /* 0x20403ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][11], 0x4); /* 0x204022c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][5], 0xa); /* 0x2040334 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][0], 0x2); /* 0x20401a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][1], 0x1); /* 0x20401a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][2], 0x1); /* 0x20401a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][3], 0x5); /* 0x20401ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][4], 0x9); /* 0x20401b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][5], 0xa); /* 0x20401b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][6], 0xa); /* 0x20401b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][7], 0x9); /* 0x20401bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][10], 0x1b); /* 0x20403e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][10], 0x7); /* 0x2040268 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][11], 0x1f); /* 0x20403ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][11], 0x6); /* 0x204026c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][6], 0x9); /* 0x2040318 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][0], 0x2); /* 0x20400c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][1], 0x4); /* 0x20400c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][2], 0x2); /* 0x20400c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][3], 0x4); /* 0x20400cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][4], 0x8); /* 0x20400d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][5], 0x5); /* 0x20400d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][6], 0x5); /* 0x20400d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][7], 0x7); /* 0x20400dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][12], 0x10); /* 0x20403b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][12], 0x5); /* 0x2040230 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][13], 0x1c); /* 0x20403b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][13], 0x4); /* 0x2040234 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][6], 0x5); /* 0x2040338 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][0], 0x1); /* 0x20401c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][1], 0x3); /* 0x20401c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][2], 0x5); /* 0x20401c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][3], 0x4); /* 0x20401cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][4], 0x8); /* 0x20401d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][5], 0xa); /* 0x20401d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][6], 0x5); /* 0x20401d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][7], 0x9); /* 0x20401dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][12], 0x15); /* 0x20403f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][12], 0x6); /* 0x2040270 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][13], 0x18); /* 0x20403f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][13], 0x7); /* 0x2040274 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][7], 0xf); /* 0x204031c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][0], 0x1); /* 0x20400e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][1], 0x5); /* 0x20400e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][2], 0x5); /* 0x20400e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][3], 0x5); /* 0x20400ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][4], 0x6); /* 0x20400f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][5], 0x7); /* 0x20400f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][6], 0x8); /* 0x20400f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][7], 0x6); /* 0x20400fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][14], 0x19); /* 0x20403b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][14], 0x5); /* 0x2040238 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][15], 0x18); /* 0x20403bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][15], 0x6); /* 0x204023c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][7], 0xa); /* 0x204033c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][0], 0x3); /* 0x20401e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][1], 0x1); /* 0x20401e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][2], 0x3); /* 0x20401e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][3], 0x3); /* 0x20401ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][4], 0x6); /* 0x20401f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][5], 0xa); /* 0x20401f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][6], 0x9); /* 0x20401f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][7], 0x7); /* 0x20401fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][14], 0x1c); /* 0x20403f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][14], 0x4); /* 0x2040278 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][15], 0x11); /* 0x20403fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][15], 0x4); /* 0x204027c */
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
    tu.IndirectWrite(0x02008001db73, 0x56d877d11ce5d393, 0xda361356d6bca5fa); /* sram_ 7_ 6: a=0x2008001db73 d0=0x56d877d11ce5d393 d1=0xda361356d6bca5fa */
    //IndirectWrite 0xda361356d6bca5fa 56d877d11ce5d393 to MauMemory (0x000002008001db73) offset 0x1db73
    tu.IndirectWrite(0x0200800023aa, 0xbb7166042db99258, 0x7331d7eb676087c1); /* sram_ 0_ 8: a=0x200800023aa d0=0xbb7166042db99258 d1=0x7331d7eb676087c1 */
    //IndirectWrite 0x7331d7eb676087c1 bb7166042db99258 to MauMemory (0x00000200800023aa) offset 0x23aa
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_table_prop[0], 0x7); /* 0x2024440 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_table_prop[1], 0x7); /* 0x2024444 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tind_bus_prop[4], 0x7); /* 0x20274d0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tind_bus_prop[8], 0x7); /* 0x20274e0 */
  //    tu.OutWord(&mau_reg_map.rams.match.merge.exact_match_delay_config, 0xe0000); /* 0x2024620 */ // REMOVED EMDEL070915
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_hit_to_logical_table_ixbar_outputmap[0], 0x13); /* 0x2024400 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_hit_to_logical_table_ixbar_outputmap[1], 0x1f); /* 0x2024404 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_match_adr_to_physical_oxbar_outputmap[4], 0x8); /* 0x2026110 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_match_adr_to_physical_oxbar_outputmap[8], 0x9); /* 0x2026120 */
    tu.OutWord(&mau_reg_map.rams.match.merge.match_to_logical_table_ixbar_outputmap[1][4], 0x13); /* 0x20246d0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.match_to_logical_table_ixbar_outputmap[1][8], 0x1f); /* 0x20246e0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.col[0].hitmap_output_map[0], 0x10); /* 0x2026840 */
    tu.OutWord(&mau_reg_map.rams.match.merge.col[0].hitmap_output_map[1], 0x11); /* 0x2026844 */
    tu.OutWord(&mau_reg_map.rams.match.merge.col[0].row_action_nxtable_bus_drive[0], 0x1); /* 0x2026800 */
    tu.OutWord(&mau_reg_map.rams.match.merge.match_to_logical_table_ixbar_outputmap[0][0], 0x10); /* 0x2024680 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_tcam_shiftcount[4], 0x0); /* 0x2026250 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_mask[1][4], 0x0); /* 0x2024950 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_default[1][4], 0x2); /* 0x20249d0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_tcam_shiftcount[4], 0x0); /* 0x20261d0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_mask[1][4], 0x0); /* 0x20247d0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_default[1][4], 0x0); /* 0x2024850 */
    tu.OutWord(&mau_reg_map.rams.match.merge.next_table_format_data[3], 0xf0000); /* 0x202618c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_tcam_shiftcount[4], 0x9); /* 0x20262d0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_mask[1][4], 0xf); /* 0x2024ad0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_default[1][4], 0xed60); /* 0x2024b50 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_tcam_shiftcount[8], 0x0); /* 0x2026260 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_mask[1][8], 0x3); /* 0x2024960 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_default[1][8], 0x14); /* 0x20249e0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_tcam_shiftcount[8], 0x0); /* 0x20261e0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_mask[1][8], 0x0); /* 0x20247e0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_default[1][8], 0x0); /* 0x2024860 */
    tu.OutWord(&mau_reg_map.rams.match.merge.next_table_format_data[15], 0x0); /* 0x20261bc */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_tcam_shiftcount[8], 0x7); /* 0x20262e0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_mask[1][8], 0x3); /* 0x2024ae0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_default[1][8], 0xa09ac); /* 0x2024b60 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_miss_value[3], 0x17); /* 0x202628c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_miss_value[3], 0x373); /* 0x202630c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_miss_value[15], 0x4); /* 0x20262bc */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_miss_value[15], 0x3aa); /* 0x202633c */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.immediate_data_32b_ixbar_ctl[3], 0x20); /* 0x202038c */
    tu.OutWord(&mau_reg_map.rams.match.merge.predication_ctl[1], 0xc290008); /* 0x2024478 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.immediate_data_32b_ixbar_ctl[15], 0x20); /* 0x20203bc */
    tu.OutWord(&mau_reg_map.rams.match.merge.predication_ctl[1], 0xc298008); /* 0x2024478 */
    tu.OutWord(&mau_reg_map.rams.match.merge.next_table_format_data[0], 0x5a); /* 0x2026180 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_miss_value[0], 0xacedbaba); /* 0x2026200 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_miss_value[0], 0x2b); /* 0x2026280 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_miss_value[0], 0x3dbabe); /* 0x2026300 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_miss_value[0], 0x2bbebe); /* 0x2026380 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_miss_value[0], 0x101cab); /* 0x2027000 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_miss_value[0], 0x11dead); /* 0x2027080 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_exact_shiftcount[0][0], 0x0); /* 0x2027600 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_mask[0][0], 0xff); /* 0x2024780 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_default[0][0], 0x0); /* 0x2024800 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_exact_shiftcount[0][0], 0x7f); /* 0x2027800 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_mask[0][0], 0x0); /* 0x2024900 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_default[0][0], 0x0); /* 0x2024980 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_exact_shiftcount[0][0], 0x8); /* 0x2027a00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_vpn_shiftcount[0][0], 0x3); /* 0x2024a00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_mask[0][0], 0xfff); /* 0x2024a80 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_default[0][0], 0x0); /* 0x2024b00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_exact_shiftcount[0][0], 0x14); /* 0x2027c00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_mask[0][0], 0x7ffff); /* 0x2024c00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_default[0][0], 0x0); /* 0x2024c80 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_exact_shiftcount[0][0], 0x27); /* 0x2027e00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_mask[0][0], 0xffff); /* 0x2024d80 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_default[0][0], 0x0); /* 0x2024e00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_exact_shiftcount[0][0], 0x37); /* 0x2024000 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_mask[0][0], 0x1ff); /* 0x2024f00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_default[0][0], 0x0); /* 0x2024f80 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[1][2], 0x40); /* 0x2008f50 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].vh_xbars.adr_dist_tind_adr_xbar_ctl[1], 0x8); /* 0x200a338 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.ram_address_mux_ctl[0][5], 0xc0); /* 0x200af28 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].vh_xbars.adr_dist_tind_adr_xbar_ctl[1], 0x9); /* 0x200c338 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.ram_address_mux_ctl[0][5], 0xc0); /* 0x200cf28 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.ram_address_mux_ctl[1][0], 0x40); /* 0x200ff40 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.adr_dist_action_data_adr_icxbar_ctl[3], 0x8000); /* 0x20203cc */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.adr_dist_action_data_adr_icxbar_ctl[15], 0x2); /* 0x20203fc */
  ActionHvTranslator act_hv_translator;
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_half[1][0], 0x3); /* 0x203f8e0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(7,1,0,0x3); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_half[1][0], 0x3); /* 0x203f8e0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(7,1,0,0x3); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_half[1][0], 0xf); /* 0x203f8e0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(7,1,0,0xf); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_half[1][0], 0xf); /* 0x203f8e0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(7,1,0,0xf); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_half[1][0], 0x3f); /* 0x203f8e0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(7,1,0,0x3f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_half[1][0], 0x3f); /* 0x203f8e0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(7,1,0,0x3f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_half[1][0], 0xff); /* 0x203f8e0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(7,1,0,0xff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_half[1][0], 0xff); /* 0x203f8e0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(7,1,0,0xff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x1); /* 0x203f858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(7,1,6,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x1); /* 0x203f858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(7,1,6,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x1); /* 0x203f858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(7,1,6,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x1); /* 0x203f858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(7,1,6,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x5); /* 0x203f858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(7,1,6,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x5); /* 0x203f858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(7,1,6,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x5); /* 0x203f858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(7,1,6,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[7].action_hv_xbar.action_hv_xbar_ctl_word[1][6], 0x5); /* 0x203f858 */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(7,1,6,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_half[1][1], 0x3); /* 0x20388e4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(0,1,1,0x3); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_half[1][1], 0x3); /* 0x20388e4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(0,1,1,0x3); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_half[1][1], 0xf); /* 0x20388e4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(0,1,1,0xf); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_half[1][1], 0xf); /* 0x20388e4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(0,1,1,0xf); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_half[1][1], 0x3f); /* 0x20388e4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(0,1,1,0x3f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_half[1][1], 0x3f); /* 0x20388e4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(0,1,1,0x3f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_half[1][1], 0xff); /* 0x20388e4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(0,1,1,0xff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_half[1][1], 0xff); /* 0x20388e4 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(0,1,1,0xff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_word[1][7], 0x1); /* 0x203885c */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(0,1,7,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_word[1][7], 0x1); /* 0x203885c */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(0,1,7,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_word[1][7], 0x1); /* 0x203885c */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(0,1,7,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_word[1][7], 0x1); /* 0x203885c */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(0,1,7,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_word[1][7], 0x5); /* 0x203885c */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(0,1,7,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_word[1][7], 0x5); /* 0x203885c */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(0,1,7,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_word[1][7], 0x5); /* 0x203885c */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(0,1,7,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[0].action_hv_xbar.action_hv_xbar_ctl_word[1][7], 0x5); /* 0x203885c */ // REMOVED ACHV070915
  act_hv_translator.ctl_word(0,1,7,0x5); // ADDED ACHV070915
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
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[1][2], RM_B4_16(0x7c7e02d)); /* 0x2078088 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[2][5], RM_B4_16(0x607624c)); /* 0x2078114 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[3][0], RM_B4_16(0x50b42ef)); /* 0x2078180 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[5][10], RM_B4_16(0xc0e3f4)); /* 0x20782a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[8][6], RM_B4_16(0xe0b6)); /* 0x2078418 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[9][5], RM_B4_16(0x5aee168)); /* 0x2078494 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[10][12], RM_B4_16(0x48ba15a)); /* 0x2078530 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[11][3], RM_B4_16(0x4c0e16d)); /* 0x207858c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[12][7], RM_B4_16(0x45da152)); /* 0x207861c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[13][12], RM_B4_16(0xf0e1c5)); /* 0x20786b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[14][9], RM_B4_16(0x210e208)); /* 0x2078724 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[15][3], RM_B4_16(0x6214216)); /* 0x207878c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[16][11], RM_B4_16(0x6dbe3be)); /* 0x207882c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[17][8], RM_B4_16(0x2ec6203)); /* 0x20788a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[18][2], RM_B4_16(0x1946262)); /* 0x2078908 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[19][12], RM_B4_16(0x4076042)); /* 0x20789b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[20][1], RM_B4_16(0x7dbe0a8)); /* 0x2078a04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[22][7], RM_B4_16(0x446a208)); /* 0x2078b1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[23][11], RM_B4_16(0x20be31e)); /* 0x2078bac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[24][10], RM_B4_16(0x4d2a1c3)); /* 0x2078c28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[25][1], RM_B4_16(0x5746339)); /* 0x2078c84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[26][5], RM_B4_16(0x27ba279)); /* 0x2078d14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[27][6], RM_B4_16(0x23ce11d)); /* 0x2078d98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[28][9], RM_B4_16(0x1ca88a2)); /* 0x2078e24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[29][6], RM_B4_16(0x3cce229)); /* 0x2078e98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[30][8], RM_B4_16(0x65fb3af)); /* 0x2078f20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[31][0], RM_B4_16(0x3a44111)); /* 0x2078f80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[32][1], RM_B4_16(0x4c6635e)); /* 0x2079004 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[33][10], RM_B4_16(0x365518b)); /* 0x20790a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[35][2], RM_B4_16(0xa009)); /* 0x2079188 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[36][6], RM_B4_16(0x413609c)); /* 0x2079218 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[37][13], RM_B4_16(0x7cce2d7)); /* 0x20792b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[38][0], RM_B4_16(0x31a33c3)); /* 0x2079300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[40][13], RM_B4_16(0x215f2dd)); /* 0x2079434 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[41][11], RM_B4_16(0x24a8e67)); /* 0x20794ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[42][3], RM_B4_16(0x178e30c)); /* 0x207950c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[43][11], RM_B4_16(0xc0e141)); /* 0x20795ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[44][11], RM_B4_16(0x7d3e37b)); /* 0x207962c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[45][5], RM_B4_16(0x2156376)); /* 0x2079694 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[46][3], RM_B4_16(0x5826377)); /* 0x207970c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[49][3], RM_B4_16(0x17e231b)); /* 0x207988c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[51][12], RM_B4_16(0x5d15339)); /* 0x20799b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[52][7], RM_B4_16(0x2b660ad)); /* 0x2079a1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[54][0], RM_B4_16(0x8a1ac)); /* 0x2079b00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[55][9], RM_B4_16(0x311e294)); /* 0x2079ba4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[56][5], RM_B4_16(0x28e136e)); /* 0x2079c14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[57][5], RM_B4_16(0x2cce105)); /* 0x2079c94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[58][11], RM_B4_16(0x12e6083)); /* 0x2079d2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[59][12], RM_B4_16(0x503e281)); /* 0x2079db0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[60][12], RM_B4_16(0x696892b)); /* 0x2079e30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[61][8], RM_B4_16(0x1c4e10c)); /* 0x2079ea0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[62][3], RM_B4_16(0x6ece004)); /* 0x2079f0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[64][6], RM_B4_16(0x403611d)); /* 0x207a018 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[65][1], RM_B4_16(0x209e2f9)); /* 0x207a084 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[67][7], RM_B4_16(0x4c1a220)); /* 0x207a19c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[68][9], RM_B4_16(0x16de272)); /* 0x207a224 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[69][3], RM_B4_16(0x1929115)); /* 0x207a28c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[70][0], RM_B4_16(0x5f9f0cc)); /* 0x207a300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[72][4], RM_B4_16(0x481a190)); /* 0x207a410 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[73][4], RM_B4_16(0x1da2117)); /* 0x207a490 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[74][11], RM_B4_16(0x125e109)); /* 0x207a52c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[76][11], RM_B4_16(0x4256120)); /* 0x207a62c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[77][5], RM_B4_16(0x35de019)); /* 0x207a694 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[78][3], RM_B4_16(0x1d9e3b5)); /* 0x207a70c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[80][8], RM_B4_16(0x20b614d)); /* 0x207a820 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[81][2], RM_B4_16(0x424a139)); /* 0x207a888 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[82][7], RM_B4_16(0x165e35a)); /* 0x207a91c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[83][2], RM_B4_16(0x607627a)); /* 0x207a988 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[84][1], RM_B4_16(0x79de125)); /* 0x207aa04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[85][4], RM_B4_16(0x82874e)); /* 0x207aa90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[87][2], RM_B4_16(0x435f11b)); /* 0x207ab88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[89][5], RM_B4_16(0x1f4e33d)); /* 0x207ac94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[90][12], RM_B4_16(0xb73093)); /* 0x207ad30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[91][10], RM_B4_16(0x96011)); /* 0x207ada8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[92][10], RM_B4_16(0x69da131)); /* 0x207ae28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[93][3], RM_B4_16(0x565e14d)); /* 0x207ae8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][0], 0x29bf5); /* 0x2074000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][1], 0x37797); /* 0x2074004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][2], 0x1ae2); /* 0x2074008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][3], 0x23fb5); /* 0x207400c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][4], 0x23889); /* 0x2074010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][5], 0x2e15b); /* 0x2074014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][6], 0x3bc83); /* 0x2074018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][7], 0x15f36); /* 0x207401c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][8], 0x3f272); /* 0x2074020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][9], 0x28793); /* 0x2074024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][10], 0x2e052); /* 0x2074028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][11], 0xcb67); /* 0x207402c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][12], 0x9a31); /* 0x2074030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][13], 0xc0b5); /* 0x2074034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][14], 0x18228); /* 0x2074038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][15], 0x213b7); /* 0x207403c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][16], 0x1058f); /* 0x2074040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][17], 0x145c7); /* 0x2074044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][18], 0x11e2a); /* 0x2074048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][19], 0x2cce9); /* 0x207404c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][20], 0xa5bb); /* 0x2074050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][21], 0x1701f); /* 0x2074054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][22], 0x46bf); /* 0x2074058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][23], 0x14b22); /* 0x207405c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][24], 0x155a9); /* 0x2074060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][25], 0x20af9); /* 0x2074064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][26], 0x2dbaa); /* 0x2074068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][27], 0x33bbe); /* 0x207406c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][28], 0x3813); /* 0x2074070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][29], 0xc400); /* 0x2074074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][30], 0x3be2c); /* 0x2074078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][31], 0x17a7); /* 0x207407c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][32], 0x3149b); /* 0x2074080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][33], 0x8c7b); /* 0x2074084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][34], 0x32bf0); /* 0x2074088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][35], 0xdd10); /* 0x207408c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][36], 0x1317f); /* 0x2074090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][37], 0x15e81); /* 0x2074094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][38], 0x31a07); /* 0x2074098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][39], 0x31c09); /* 0x207409c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][40], 0x3bd94); /* 0x20740a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][41], 0x1eeb9); /* 0x20740a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][42], 0x11eb9); /* 0x20740a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][43], 0x3a692); /* 0x20740ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][44], 0x35d0); /* 0x20740b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][45], 0x217ba); /* 0x20740b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][46], 0x3863a); /* 0x20740b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][47], 0x34c0e); /* 0x20740bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][48], 0x289af); /* 0x20740c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][49], 0x14377); /* 0x20740c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][50], 0x1a3c); /* 0x20740c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][51], 0xd815); /* 0x20740cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][0], 0x308ed); /* 0x2074100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][1], 0x3bbca); /* 0x2074104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][2], 0x1fe8a); /* 0x2074108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][3], 0x1f92e); /* 0x207410c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][4], 0x22947); /* 0x2074110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][5], 0x3617c); /* 0x2074114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][6], 0x595f); /* 0x2074118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][7], 0x378d3); /* 0x207411c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][8], 0x3c66b); /* 0x2074120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][9], 0x1c6fe); /* 0x2074124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][10], 0xdb9); /* 0x2074128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][11], 0x376c7); /* 0x207412c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][12], 0x19b0c); /* 0x2074130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][13], 0x2f3b0); /* 0x2074134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][14], 0x316bc); /* 0x2074138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][15], 0x9bb4); /* 0x207413c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][16], 0x2c51f); /* 0x2074140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][17], 0x30f3c); /* 0x2074144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][18], 0x3b4bf); /* 0x2074148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][19], 0x2cc83); /* 0x207414c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][20], 0xb1c5); /* 0x2074150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][21], 0x8022); /* 0x2074154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][22], 0x3c1e0); /* 0x2074158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][23], 0xb2a9); /* 0x207415c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][24], 0x11e31); /* 0x2074160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][25], 0x31c3c); /* 0x2074164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][26], 0x3c153); /* 0x2074168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][27], 0x5522); /* 0x207416c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][28], 0x8211); /* 0x2074170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][29], 0x33ea5); /* 0x2074174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][30], 0x7120); /* 0x2074178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][31], 0x14ada); /* 0x207417c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][32], 0xce9b); /* 0x2074180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][33], 0x284a9); /* 0x2074184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][34], 0x2e570); /* 0x2074188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][35], 0x1889d); /* 0x207418c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][36], 0xe22d); /* 0x2074190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][37], 0xe5d1); /* 0x2074194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][38], 0x38ff3); /* 0x2074198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][39], 0x17215); /* 0x207419c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][40], 0xe279); /* 0x20741a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][41], 0x1ef4f); /* 0x20741a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][42], 0x1b037); /* 0x20741a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][43], 0x21cf6); /* 0x20741ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][44], 0x12d64); /* 0x20741b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][45], 0x349ce); /* 0x20741b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][46], 0x12c3b); /* 0x20741b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][47], 0x211fd); /* 0x20741bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][48], 0x141ea); /* 0x20741c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][49], 0x226c4); /* 0x20741c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][50], 0x2f402); /* 0x20741c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][51], 0x12ac); /* 0x20741cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][0], 0x93d1); /* 0x2074200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][1], 0x28072); /* 0x2074204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][2], 0x379aa); /* 0x2074208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][3], 0x1ebd9); /* 0x207420c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][4], 0x37888); /* 0x2074210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][5], 0x1e974); /* 0x2074214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][6], 0x9af9); /* 0x2074218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][7], 0x289e2); /* 0x207421c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][8], 0xba7c); /* 0x2074220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][9], 0x26892); /* 0x2074224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][10], 0x1bf40); /* 0x2074228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][11], 0x2fe08); /* 0x207422c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][12], 0x3493); /* 0x2074230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][13], 0x34acc); /* 0x2074234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][14], 0x1d20f); /* 0x2074238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][15], 0x30a51); /* 0x207423c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][16], 0x2149d); /* 0x2074240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][17], 0x34410); /* 0x2074244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][18], 0x1bc83); /* 0x2074248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][19], 0x1b863); /* 0x207424c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][20], 0x2597a); /* 0x2074250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][21], 0x28757); /* 0x2074254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][22], 0x2d672); /* 0x2074258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][23], 0x1a596); /* 0x207425c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][24], 0x8da0); /* 0x2074260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][25], 0x9c02); /* 0x2074264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][26], 0x9085); /* 0x2074268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][27], 0x366ce); /* 0x207426c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][28], 0xd376); /* 0x2074270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][29], 0x4c09); /* 0x2074274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][30], 0x19225); /* 0x2074278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][31], 0x395c5); /* 0x207427c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][32], 0x28d8b); /* 0x2074280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][33], 0x7fd1); /* 0x2074284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][34], 0x37ee7); /* 0x2074288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][35], 0x88af); /* 0x207428c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][36], 0x4ec3); /* 0x2074290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][37], 0x3d89); /* 0x2074294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][38], 0x47cd); /* 0x2074298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][39], 0x17b11); /* 0x207429c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][40], 0x10848); /* 0x20742a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][41], 0x2fcfc); /* 0x20742a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][42], 0x1156e); /* 0x20742a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][43], 0x2cf5d); /* 0x20742ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][44], 0x96a0); /* 0x20742b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][45], 0x3bbaf); /* 0x20742b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][46], 0x3fd00); /* 0x20742b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][47], 0x3f2f3); /* 0x20742bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][48], 0xaf7f); /* 0x20742c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][49], 0x2327a); /* 0x20742c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][50], 0x240dc); /* 0x20742c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][51], 0x76da); /* 0x20742cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][0], 0x10906); /* 0x2074300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][1], 0xe353); /* 0x2074304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][2], 0x3354f); /* 0x2074308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][3], 0x1c7e2); /* 0x207430c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][4], 0x184c2); /* 0x2074310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][5], 0x8b34); /* 0x2074314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][6], 0xcd28); /* 0x2074318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][7], 0x372f1); /* 0x207431c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][8], 0x14ab9); /* 0x2074320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][9], 0x117a3); /* 0x2074324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][10], 0x28648); /* 0x2074328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][11], 0x2ea73); /* 0x207432c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][12], 0x31736); /* 0x2074330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][13], 0x3a685); /* 0x2074334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][14], 0x3d784); /* 0x2074338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][15], 0xf824); /* 0x207433c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][16], 0x2306b); /* 0x2074340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][17], 0xe3dd); /* 0x2074344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][18], 0xf855); /* 0x2074348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][19], 0x38e4a); /* 0x207434c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][20], 0x3651c); /* 0x2074350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][21], 0x2d667); /* 0x2074354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][22], 0x23f1e); /* 0x2074358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][23], 0x22080); /* 0x207435c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][24], 0x21154); /* 0x2074360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][25], 0x2bf48); /* 0x2074364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][26], 0x17078); /* 0x2074368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][27], 0x2141b); /* 0x207436c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][28], 0x616e); /* 0x2074370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][29], 0x18701); /* 0x2074374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][30], 0x326de); /* 0x2074378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][31], 0x2befe); /* 0x207437c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][32], 0x16cfe); /* 0x2074380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][33], 0x19bc9); /* 0x2074384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][34], 0x33c94); /* 0x2074388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][35], 0x39162); /* 0x207438c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][36], 0x3692d); /* 0x2074390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][37], 0x2116c); /* 0x2074394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][38], 0x1be99); /* 0x2074398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][39], 0x311b8); /* 0x207439c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][40], 0x33ddd); /* 0x20743a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][41], 0x26188); /* 0x20743a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][42], 0x2ff35); /* 0x20743a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][43], 0x2854a); /* 0x20743ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][44], 0x3fd21); /* 0x20743b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][45], 0x29b3b); /* 0x20743b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][46], 0x2f5df); /* 0x20743b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][47], 0x136c); /* 0x20743bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][48], 0x23d41); /* 0x20743c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][49], 0x2c2f); /* 0x20743c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][50], 0x25d36); /* 0x20743c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][51], 0x7fc3); /* 0x20743cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][0], 0x12d74); /* 0x2074400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][1], 0x14045); /* 0x2074404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][2], 0x5028); /* 0x2074408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][3], 0x33657); /* 0x207440c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][4], 0x28b49); /* 0x2074410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][5], 0x2012c); /* 0x2074414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][6], 0x710f); /* 0x2074418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][7], 0x1af76); /* 0x207441c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][8], 0xc660); /* 0x2074420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][9], 0x1e67); /* 0x2074424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][10], 0x8ff2); /* 0x2074428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][11], 0x18d85); /* 0x207442c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][12], 0x30c82); /* 0x2074430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][13], 0x350e4); /* 0x2074434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][14], 0x1168e); /* 0x2074438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][15], 0xa0e8); /* 0x207443c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][16], 0x13300); /* 0x2074440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][17], 0x206b5); /* 0x2074444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][18], 0xe225); /* 0x2074448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][19], 0x145fe); /* 0x207444c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][20], 0x4b06); /* 0x2074450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][21], 0xd5cf); /* 0x2074454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][22], 0xcc38); /* 0x2074458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][23], 0x16f1c); /* 0x207445c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][24], 0x273e5); /* 0x2074460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][25], 0x27bf5); /* 0x2074464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][26], 0x14221); /* 0x2074468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][27], 0x6126); /* 0x207446c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][28], 0xad1); /* 0x2074470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][29], 0x7bb7); /* 0x2074474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][30], 0x4642); /* 0x2074478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][31], 0xb616); /* 0x207447c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][32], 0x1a9f6); /* 0x2074480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][33], 0x235d4); /* 0x2074484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][34], 0x3f2d3); /* 0x2074488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][35], 0x22f68); /* 0x207448c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][36], 0x17d65); /* 0x2074490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][37], 0xa9b3); /* 0x2074494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][38], 0x26169); /* 0x2074498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][39], 0x38d94); /* 0x207449c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][40], 0x3118e); /* 0x20744a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][41], 0x1d381); /* 0x20744a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][42], 0x3eb29); /* 0x20744a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][43], 0x240b); /* 0x20744ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][44], 0x28c1e); /* 0x20744b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][45], 0x35bc7); /* 0x20744b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][46], 0x42b3); /* 0x20744b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][47], 0x2c245); /* 0x20744bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][48], 0x3a05d); /* 0x20744c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][49], 0x3affb); /* 0x20744c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][50], 0x16ea); /* 0x20744c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][51], 0x1ffc6); /* 0x20744cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][0], 0x10b3); /* 0x2074500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][1], 0x106); /* 0x2074504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][2], 0x28d6b); /* 0x2074508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][3], 0x3de9a); /* 0x207450c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][4], 0x3bb13); /* 0x2074510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][5], 0x329f4); /* 0x2074514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][6], 0x4a4e); /* 0x2074518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][7], 0x30b5e); /* 0x207451c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][8], 0x14533); /* 0x2074520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][9], 0x2192c); /* 0x2074524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][10], 0x2bd07); /* 0x2074528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][11], 0x10495); /* 0x207452c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][12], 0x3ef89); /* 0x2074530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][13], 0x212cb); /* 0x2074534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][14], 0xe4b5); /* 0x2074538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][15], 0x1000b); /* 0x207453c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][16], 0x3195c); /* 0x2074540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][17], 0xd4df); /* 0x2074544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][18], 0x299f5); /* 0x2074548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][19], 0x1838c); /* 0x207454c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][20], 0x24156); /* 0x2074550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][21], 0x1a9ad); /* 0x2074554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][22], 0x1f394); /* 0x2074558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][23], 0x763e); /* 0x207455c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][24], 0x2180f); /* 0x2074560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][25], 0x14d49); /* 0x2074564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][26], 0x1c508); /* 0x2074568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][27], 0x1670e); /* 0x207456c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][28], 0x1a265); /* 0x2074570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][29], 0x38b0b); /* 0x2074574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][30], 0x34481); /* 0x2074578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][31], 0x1395c); /* 0x207457c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][32], 0x3fe08); /* 0x2074580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][33], 0x25626); /* 0x2074584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][34], 0x25014); /* 0x2074588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][35], 0x16d23); /* 0x207458c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][36], 0x1e104); /* 0x2074590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][37], 0x1c94e); /* 0x2074594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][38], 0x6fa8); /* 0x2074598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][39], 0x16061); /* 0x207459c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][40], 0x7885); /* 0x20745a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][41], 0x651f); /* 0x20745a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][42], 0xcf5); /* 0x20745a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][43], 0x3abe2); /* 0x20745ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][44], 0x9e94); /* 0x20745b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][45], 0x1f927); /* 0x20745b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][46], 0x35d5b); /* 0x20745b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][47], 0x34d99); /* 0x20745bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][48], 0x1b3d6); /* 0x20745c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][49], 0x13971); /* 0x20745c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][50], 0x3dde3); /* 0x20745c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][51], 0x36cb6); /* 0x20745cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][0], 0x34d45); /* 0x2074600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][1], 0x11f11); /* 0x2074604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][2], 0x2a3d4); /* 0x2074608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][3], 0x1209a); /* 0x207460c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][4], 0x1bedf); /* 0x2074610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][5], 0x3173c); /* 0x2074614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][6], 0x20a1c); /* 0x2074618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][7], 0x1b5b1); /* 0x207461c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][8], 0x373ff); /* 0x2074620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][9], 0x12eea); /* 0x2074624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][10], 0x220ba); /* 0x2074628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][11], 0x29722); /* 0x207462c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][12], 0x2b823); /* 0x2074630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][13], 0x3f096); /* 0x2074634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][14], 0x29ece); /* 0x2074638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][15], 0x3ad9b); /* 0x207463c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][16], 0x39494); /* 0x2074640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][17], 0xeed0); /* 0x2074644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][18], 0x2c4ea); /* 0x2074648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][19], 0xbff9); /* 0x207464c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][20], 0x2b33f); /* 0x2074650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][21], 0x21a85); /* 0x2074654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][22], 0x32d17); /* 0x2074658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][23], 0xcab2); /* 0x207465c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][24], 0x23333); /* 0x2074660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][25], 0x19e45); /* 0x2074664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][26], 0x39511); /* 0x2074668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][27], 0x39657); /* 0x207466c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][28], 0x3d31b); /* 0x2074670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][29], 0x36e53); /* 0x2074674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][30], 0x32dd); /* 0x2074678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][31], 0x270cb); /* 0x207467c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][32], 0x2ef92); /* 0x2074680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][33], 0x21170); /* 0x2074684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][34], 0x3a805); /* 0x2074688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][35], 0x204dd); /* 0x207468c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][36], 0x24a0a); /* 0x2074690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][37], 0x470a); /* 0x2074694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][38], 0x36c17); /* 0x2074698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][39], 0xad33); /* 0x207469c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][40], 0x3d466); /* 0x20746a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][41], 0x6271); /* 0x20746a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][42], 0x2bc35); /* 0x20746a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][43], 0xe69c); /* 0x20746ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][44], 0x269ba); /* 0x20746b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][45], 0x1423d); /* 0x20746b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][46], 0x635c); /* 0x20746b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][47], 0x1187c); /* 0x20746bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][48], 0x1ec); /* 0x20746c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][49], 0x28fb5); /* 0x20746c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][50], 0x106ca); /* 0x20746c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][51], 0x24b83); /* 0x20746cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][0], 0xe762); /* 0x2074700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][1], 0x3e79d); /* 0x2074704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][2], 0x1256b); /* 0x2074708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][3], 0x542b); /* 0x207470c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][4], 0x131ec); /* 0x2074710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][5], 0x3b4cc); /* 0x2074714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][6], 0x1ae60); /* 0x2074718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][7], 0x2d76d); /* 0x207471c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][8], 0xf1b0); /* 0x2074720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][9], 0x389ec); /* 0x2074724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][10], 0x7ba0); /* 0x2074728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][11], 0x19325); /* 0x207472c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][12], 0x30eb0); /* 0x2074730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][13], 0x21543); /* 0x2074734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][14], 0x364db); /* 0x2074738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][15], 0x319b0); /* 0x207473c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][16], 0x1593f); /* 0x2074740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][17], 0x33726); /* 0x2074744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][18], 0x26009); /* 0x2074748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][19], 0x38f52); /* 0x207474c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][20], 0x8a2e); /* 0x2074750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][21], 0x27c37); /* 0x2074754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][22], 0xe008); /* 0x2074758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][23], 0x14adf); /* 0x207475c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][24], 0x37747); /* 0x2074760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][25], 0xb3e4); /* 0x2074764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][26], 0x21ba0); /* 0x2074768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][27], 0x13463); /* 0x207476c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][28], 0x35abf); /* 0x2074770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][29], 0x37e75); /* 0x2074774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][30], 0xe43d); /* 0x2074778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][31], 0x23571); /* 0x207477c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][32], 0x154a5); /* 0x2074780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][33], 0x13c35); /* 0x2074784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][34], 0x2728); /* 0x2074788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][35], 0x26d03); /* 0x207478c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][36], 0x32c3c); /* 0x2074790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][37], 0x591b); /* 0x2074794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][38], 0x2ec89); /* 0x2074798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][39], 0x1d6b6); /* 0x207479c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][40], 0x27d29); /* 0x20747a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][41], 0x27d1c); /* 0x20747a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][42], 0x147e5); /* 0x20747a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][43], 0xadfc); /* 0x20747ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][44], 0x369d4); /* 0x20747b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][45], 0x332f4); /* 0x20747b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][46], 0x2288); /* 0x20747b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][47], 0x131de); /* 0x20747bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][48], 0x2e15e); /* 0x20747c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][49], 0x34300); /* 0x20747c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][50], 0x36f6f); /* 0x20747c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][51], 0x35db); /* 0x20747cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][0], 0x19e7); /* 0x2074800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][1], 0x154f6); /* 0x2074804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][2], 0x327cb); /* 0x2074808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][3], 0x3c053); /* 0x207480c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][4], 0x39dbb); /* 0x2074810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][5], 0x1799e); /* 0x2074814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][6], 0x3010); /* 0x2074818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][7], 0x2d92c); /* 0x207481c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][8], 0xc03f); /* 0x2074820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][9], 0xea69); /* 0x2074824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][10], 0x6de0); /* 0x2074828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][11], 0x16db2); /* 0x207482c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][12], 0x2151e); /* 0x2074830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][13], 0x27853); /* 0x2074834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][14], 0x332ca); /* 0x2074838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][15], 0x33999); /* 0x207483c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][16], 0x1958b); /* 0x2074840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][17], 0x1976); /* 0x2074844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][18], 0x90c4); /* 0x2074848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][19], 0x3216f); /* 0x207484c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][20], 0x29731); /* 0x2074850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][21], 0x24089); /* 0x2074854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][22], 0xbc26); /* 0x2074858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][23], 0x2b6f1); /* 0x207485c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][24], 0x36c0c); /* 0x2074860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][25], 0x198f); /* 0x2074864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][26], 0x1544e); /* 0x2074868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][27], 0x1108e); /* 0x207486c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][28], 0x235f7); /* 0x2074870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][29], 0x1b128); /* 0x2074874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][30], 0x247db); /* 0x2074878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][31], 0x2a4a5); /* 0x207487c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][32], 0xa452); /* 0x2074880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][33], 0x25449); /* 0x2074884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][34], 0x2eee6); /* 0x2074888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][35], 0x36702); /* 0x207488c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][36], 0x1dbb); /* 0x2074890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][37], 0x325f6); /* 0x2074894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][38], 0x217a7); /* 0x2074898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][39], 0x7643); /* 0x207489c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][40], 0x603e); /* 0x20748a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][41], 0x658c); /* 0x20748a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][42], 0x1d94f); /* 0x20748a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][43], 0x1663b); /* 0x20748ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][44], 0x288e2); /* 0x20748b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][45], 0x1b79); /* 0x20748b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][46], 0x12205); /* 0x20748b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][47], 0xf163); /* 0x20748bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][48], 0x192d9); /* 0x20748c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][49], 0x300b6); /* 0x20748c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][50], 0x3bdd3); /* 0x20748c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][51], 0x337d7); /* 0x20748cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][0], 0x1f4da); /* 0x2074900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][1], 0x5592); /* 0x2074904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][2], 0x8c0d); /* 0x2074908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][3], 0x12dda); /* 0x207490c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][4], 0x3c034); /* 0x2074910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][5], 0x130ba); /* 0x2074914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][6], 0x1d6e9); /* 0x2074918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][7], 0x3598e); /* 0x207491c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][8], 0x1db0f); /* 0x2074920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][9], 0x49f1); /* 0x2074924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][10], 0x9992); /* 0x2074928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][11], 0xf36f); /* 0x207492c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][12], 0x3c778); /* 0x2074930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][13], 0x238c8); /* 0x2074934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][14], 0x2519e); /* 0x2074938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][15], 0x1cf71); /* 0x207493c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][16], 0x1826a); /* 0x2074940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][17], 0xbbe); /* 0x2074944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][18], 0x192f1); /* 0x2074948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][19], 0x1ef63); /* 0x207494c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][20], 0x5035); /* 0x2074950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][21], 0x364d7); /* 0x2074954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][22], 0x35cdb); /* 0x2074958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][23], 0x14172); /* 0x207495c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][24], 0x2c7b1); /* 0x2074960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][25], 0x2e0cb); /* 0x2074964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][26], 0x3c397); /* 0x2074968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][27], 0xbfad); /* 0x207496c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][28], 0x1ccc7); /* 0x2074970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][29], 0x7da6); /* 0x2074974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][30], 0xa697); /* 0x2074978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][31], 0x2253e); /* 0x207497c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][32], 0x5065); /* 0x2074980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][33], 0xbc69); /* 0x2074984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][34], 0x8dcb); /* 0x2074988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][35], 0x25341); /* 0x207498c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][36], 0x81e2); /* 0x2074990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][37], 0x9a0e); /* 0x2074994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][38], 0xc59e); /* 0x2074998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][39], 0x1e792); /* 0x207499c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][40], 0x53be); /* 0x20749a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][41], 0x3cfcc); /* 0x20749a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][42], 0xb7df); /* 0x20749a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][43], 0x15cd3); /* 0x20749ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][44], 0x28932); /* 0x20749b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][45], 0x334f6); /* 0x20749b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][46], 0x14c0a); /* 0x20749b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][47], 0x3c7e7); /* 0x20749bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][48], 0x2161b); /* 0x20749c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][49], 0xc9bf); /* 0x20749c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][50], 0x37cd4); /* 0x20749c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][51], 0x6037); /* 0x20749cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][0], 0x125a5); /* 0x2074a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][1], 0x3c404); /* 0x2074a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][2], 0x5e86); /* 0x2074a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][3], 0x2231a); /* 0x2074a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][4], 0x1557f); /* 0x2074a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][5], 0x15d3b); /* 0x2074a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][6], 0x2a73f); /* 0x2074a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][7], 0x21bb1); /* 0x2074a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][8], 0x2968b); /* 0x2074a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][9], 0x4b01); /* 0x2074a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][10], 0xacfc); /* 0x2074a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][11], 0x1a2cb); /* 0x2074a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][12], 0x2ad7d); /* 0x2074a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][13], 0x11fdf); /* 0x2074a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][14], 0x11f8d); /* 0x2074a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][15], 0x38333); /* 0x2074a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][16], 0xff98); /* 0x2074a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][17], 0x4287); /* 0x2074a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][18], 0x39c42); /* 0x2074a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][19], 0x1792a); /* 0x2074a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][20], 0x36e9e); /* 0x2074a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][21], 0x354bd); /* 0x2074a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][22], 0xe821); /* 0x2074a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][23], 0x28666); /* 0x2074a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][24], 0x28126); /* 0x2074a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][25], 0x34167); /* 0x2074a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][26], 0x38761); /* 0x2074a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][27], 0x1f0af); /* 0x2074a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][28], 0x1b962); /* 0x2074a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][29], 0x1f55c); /* 0x2074a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][30], 0x375f1); /* 0x2074a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][31], 0x19cd5); /* 0x2074a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][32], 0x3daa0); /* 0x2074a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][33], 0x119c3); /* 0x2074a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][34], 0x1c3b8); /* 0x2074a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][35], 0x3ce99); /* 0x2074a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][36], 0x3fd78); /* 0x2074a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][37], 0x17cea); /* 0x2074a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][38], 0x34dd0); /* 0x2074a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][39], 0x3b118); /* 0x2074a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][40], 0x2de09); /* 0x2074aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][41], 0x14139); /* 0x2074aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][42], 0x1f809); /* 0x2074aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][43], 0x1404b); /* 0x2074aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][44], 0x19efc); /* 0x2074ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][45], 0x8460); /* 0x2074ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][46], 0x340d9); /* 0x2074ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][47], 0xa3e6); /* 0x2074abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][48], 0x1a5d7); /* 0x2074ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][49], 0x2e96); /* 0x2074ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][50], 0x15a23); /* 0x2074ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][51], 0x3427e); /* 0x2074acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][0], 0x2285c); /* 0x2074b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][1], 0x2ddf6); /* 0x2074b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][2], 0x1ca9c); /* 0x2074b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][3], 0x2178d); /* 0x2074b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][4], 0x353a4); /* 0x2074b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][5], 0xad8f); /* 0x2074b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][6], 0x30816); /* 0x2074b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][7], 0x7f9a); /* 0x2074b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][8], 0x2aa5a); /* 0x2074b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][9], 0x32d1e); /* 0x2074b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][10], 0x3f586); /* 0x2074b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][11], 0x1d3ac); /* 0x2074b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][12], 0x3865e); /* 0x2074b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][13], 0x655); /* 0x2074b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][14], 0x159c3); /* 0x2074b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][15], 0x3f6d1); /* 0x2074b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][16], 0x129d); /* 0x2074b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][17], 0xbe20); /* 0x2074b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][18], 0x2ba2c); /* 0x2074b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][19], 0x11208); /* 0x2074b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][20], 0x3ed74); /* 0x2074b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][21], 0x9599); /* 0x2074b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][22], 0x3c072); /* 0x2074b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][23], 0xebbd); /* 0x2074b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][24], 0xd92b); /* 0x2074b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][25], 0x1abfd); /* 0x2074b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][26], 0x1f68a); /* 0x2074b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][27], 0x1e07f); /* 0x2074b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][28], 0x2bab5); /* 0x2074b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][29], 0xc9f8); /* 0x2074b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][30], 0x19bc9); /* 0x2074b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][31], 0x39039); /* 0x2074b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][32], 0x1f801); /* 0x2074b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][33], 0x1229b); /* 0x2074b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][34], 0x8c7); /* 0x2074b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][35], 0x26939); /* 0x2074b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][36], 0x154b); /* 0x2074b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][37], 0x2277d); /* 0x2074b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][38], 0x30f51); /* 0x2074b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][39], 0x2580b); /* 0x2074b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][40], 0xd1f4); /* 0x2074ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][41], 0x512); /* 0x2074ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][42], 0x7afa); /* 0x2074ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][43], 0x3abbb); /* 0x2074bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][44], 0xf261); /* 0x2074bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][45], 0x2673b); /* 0x2074bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][46], 0x3978a); /* 0x2074bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][47], 0x1ddd7); /* 0x2074bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][48], 0x41ea); /* 0x2074bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][49], 0x358a7); /* 0x2074bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][50], 0x24dd); /* 0x2074bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][51], 0x237d5); /* 0x2074bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][0], 0x3db08); /* 0x2074c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][1], 0xbcf3); /* 0x2074c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][2], 0x13f69); /* 0x2074c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][3], 0x1a73e); /* 0x2074c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][4], 0x12cdc); /* 0x2074c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][5], 0x3a163); /* 0x2074c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][6], 0x1e8cc); /* 0x2074c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][7], 0x1fe10); /* 0x2074c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][8], 0x8ea0); /* 0x2074c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][9], 0x25cdb); /* 0x2074c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][10], 0x1fb22); /* 0x2074c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][11], 0x11041); /* 0x2074c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][12], 0x16d41); /* 0x2074c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][13], 0x15f4f); /* 0x2074c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][14], 0x5e49); /* 0x2074c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][15], 0x34e1f); /* 0x2074c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][16], 0x34d43); /* 0x2074c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][17], 0x31b24); /* 0x2074c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][18], 0x2c884); /* 0x2074c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][19], 0x24cc5); /* 0x2074c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][20], 0x2de3); /* 0x2074c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][21], 0xc36b); /* 0x2074c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][22], 0x204cc); /* 0x2074c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][23], 0x237ec); /* 0x2074c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][24], 0xe7b5); /* 0x2074c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][25], 0x33314); /* 0x2074c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][26], 0x2975c); /* 0x2074c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][27], 0x5396); /* 0x2074c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][28], 0x1a153); /* 0x2074c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][29], 0x1db7a); /* 0x2074c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][30], 0x512); /* 0x2074c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][31], 0x2f80); /* 0x2074c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][32], 0x33fa3); /* 0x2074c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][33], 0x19542); /* 0x2074c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][34], 0x23b55); /* 0x2074c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][35], 0x7ca0); /* 0x2074c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][36], 0x1a29c); /* 0x2074c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][37], 0x74ce); /* 0x2074c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][38], 0x3902c); /* 0x2074c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][39], 0x61dd); /* 0x2074c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][40], 0x2df82); /* 0x2074ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][41], 0x1766); /* 0x2074ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][42], 0x9c0a); /* 0x2074ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][43], 0x11cf6); /* 0x2074cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][44], 0x28ee7); /* 0x2074cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][45], 0x10004); /* 0x2074cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][46], 0x33cff); /* 0x2074cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][47], 0x3c745); /* 0x2074cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][48], 0x30583); /* 0x2074cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][49], 0x30e5b); /* 0x2074cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][50], 0x2d050); /* 0x2074cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][51], 0x16fd0); /* 0x2074ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][0], 0x181eb); /* 0x2074d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][1], 0xa082); /* 0x2074d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][2], 0xc192); /* 0x2074d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][3], 0x258a1); /* 0x2074d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][4], 0x26b73); /* 0x2074d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][5], 0x397ff); /* 0x2074d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][6], 0x3a44b); /* 0x2074d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][7], 0x11e51); /* 0x2074d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][8], 0x2589d); /* 0x2074d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][9], 0x114fb); /* 0x2074d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][10], 0x46e2); /* 0x2074d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][11], 0x4f7d); /* 0x2074d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][12], 0x14c2f); /* 0x2074d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][13], 0x1db01); /* 0x2074d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][14], 0x1732d); /* 0x2074d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][15], 0x3a42d); /* 0x2074d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][16], 0x15b1b); /* 0x2074d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][17], 0x7011); /* 0x2074d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][18], 0x20c5b); /* 0x2074d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][19], 0x32436); /* 0x2074d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][20], 0x354c1); /* 0x2074d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][21], 0xaa42); /* 0x2074d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][22], 0x3ba82); /* 0x2074d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][23], 0x17c22); /* 0x2074d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][24], 0x29fa7); /* 0x2074d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][25], 0x163df); /* 0x2074d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][26], 0x21d99); /* 0x2074d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][27], 0x1aec2); /* 0x2074d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][28], 0x14b41); /* 0x2074d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][29], 0x1648a); /* 0x2074d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][30], 0x2680d); /* 0x2074d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][31], 0xfc7c); /* 0x2074d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][32], 0x4887); /* 0x2074d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][33], 0x9ad); /* 0x2074d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][34], 0x10376); /* 0x2074d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][35], 0x1ce0b); /* 0x2074d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][36], 0x3f55b); /* 0x2074d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][37], 0x1a908); /* 0x2074d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][38], 0xae69); /* 0x2074d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][39], 0x3b36a); /* 0x2074d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][40], 0x2a5ec); /* 0x2074da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][41], 0x2cd8a); /* 0x2074da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][42], 0x11fb1); /* 0x2074da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][43], 0x2742f); /* 0x2074dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][44], 0x17e2f); /* 0x2074db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][45], 0x323d1); /* 0x2074db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][46], 0x1edff); /* 0x2074db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][47], 0x3e0fc); /* 0x2074dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][48], 0xf3f0); /* 0x2074dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][49], 0x2cd12); /* 0x2074dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][50], 0xd1d4); /* 0x2074dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][51], 0x1ee01); /* 0x2074dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][0], 0x3c32e); /* 0x2074e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][1], 0x2c210); /* 0x2074e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][2], 0x3df86); /* 0x2074e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][3], 0x25d); /* 0x2074e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][4], 0x1359f); /* 0x2074e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][5], 0x3debf); /* 0x2074e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][6], 0x27143); /* 0x2074e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][7], 0x2b5f3); /* 0x2074e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][8], 0x3e578); /* 0x2074e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][9], 0x34eae); /* 0x2074e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][10], 0x84c2); /* 0x2074e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][11], 0x1e5dd); /* 0x2074e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][12], 0x1dda4); /* 0x2074e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][13], 0x1f9f5); /* 0x2074e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][14], 0x6d64); /* 0x2074e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][15], 0x19062); /* 0x2074e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][16], 0x132e4); /* 0x2074e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][17], 0x1f79d); /* 0x2074e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][18], 0x37a4d); /* 0x2074e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][19], 0x27575); /* 0x2074e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][20], 0x3aeba); /* 0x2074e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][21], 0x55c1); /* 0x2074e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][22], 0x20c2); /* 0x2074e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][23], 0x59d9); /* 0x2074e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][24], 0x19b82); /* 0x2074e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][25], 0x13451); /* 0x2074e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][26], 0x28be3); /* 0x2074e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][27], 0x3f948); /* 0x2074e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][28], 0x38658); /* 0x2074e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][29], 0x39a73); /* 0x2074e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][30], 0x28f01); /* 0x2074e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][31], 0x3a691); /* 0x2074e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][32], 0xffa8); /* 0x2074e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][33], 0x3e4ce); /* 0x2074e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][34], 0xe3ef); /* 0x2074e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][35], 0x4ea3); /* 0x2074e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][36], 0x355da); /* 0x2074e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][37], 0x3a3dd); /* 0x2074e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][38], 0x35abd); /* 0x2074e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][39], 0x3c4a); /* 0x2074e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][40], 0x2d8d5); /* 0x2074ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][41], 0x18c11); /* 0x2074ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][42], 0x4a68); /* 0x2074ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][43], 0x134f6); /* 0x2074eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][44], 0x3042c); /* 0x2074eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][45], 0x1d54f); /* 0x2074eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][46], 0x3e7c4); /* 0x2074eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][47], 0x33b7a); /* 0x2074ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][48], 0x121bb); /* 0x2074ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][49], 0x35976); /* 0x2074ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][50], 0x1e18b); /* 0x2074ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][51], 0x3bb5d); /* 0x2074ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][0], 0xd79e); /* 0x2074f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][1], 0x29855); /* 0x2074f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][2], 0x1b0b6); /* 0x2074f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][3], 0x17d88); /* 0x2074f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][4], 0x395ef); /* 0x2074f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][5], 0x2c8d2); /* 0x2074f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][6], 0x259d4); /* 0x2074f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][7], 0x3b32f); /* 0x2074f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][8], 0x4c0f); /* 0x2074f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][9], 0x1796d); /* 0x2074f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][10], 0x1c2ab); /* 0x2074f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][11], 0x31818); /* 0x2074f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][12], 0x2740b); /* 0x2074f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][13], 0x48a8); /* 0x2074f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][14], 0x3b59); /* 0x2074f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][15], 0x29c35); /* 0x2074f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][16], 0x36441); /* 0x2074f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][17], 0x325e1); /* 0x2074f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][18], 0x11d92); /* 0x2074f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][19], 0x19e12); /* 0x2074f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][20], 0x19142); /* 0x2074f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][21], 0x1537b); /* 0x2074f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][22], 0x13e20); /* 0x2074f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][23], 0x127b9); /* 0x2074f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][24], 0x32a4d); /* 0x2074f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][25], 0x945e); /* 0x2074f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][26], 0x3d0ca); /* 0x2074f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][27], 0x211b9); /* 0x2074f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][28], 0x1495f); /* 0x2074f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][29], 0x3ac27); /* 0x2074f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][30], 0x1d464); /* 0x2074f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][31], 0x28115); /* 0x2074f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][32], 0x1691c); /* 0x2074f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][33], 0x30912); /* 0x2074f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][34], 0x18021); /* 0x2074f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][35], 0x2ce43); /* 0x2074f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][36], 0x3d502); /* 0x2074f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][37], 0x4fa4); /* 0x2074f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][38], 0x3209b); /* 0x2074f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][39], 0x27e79); /* 0x2074f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][40], 0x210a2); /* 0x2074fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][41], 0x1ce4d); /* 0x2074fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][42], 0x27993); /* 0x2074fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][43], 0xc526); /* 0x2074fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][44], 0x30086); /* 0x2074fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][45], 0x1bf5a); /* 0x2074fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][46], 0x3a976); /* 0x2074fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][47], 0x1f864); /* 0x2074fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][48], 0x32bb5); /* 0x2074fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][49], 0x1f10d); /* 0x2074fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][50], 0x3f3aa); /* 0x2074fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][51], 0x26592); /* 0x2074fcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][0], 0x3d2cc); /* 0x2075000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][1], 0x360b6); /* 0x2075004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][2], 0xea1b); /* 0x2075008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][3], 0x3fea4); /* 0x207500c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][4], 0x26d2b); /* 0x2075010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][5], 0x31bf1); /* 0x2075014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][6], 0xe430); /* 0x2075018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][7], 0x5173); /* 0x207501c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][8], 0x30672); /* 0x2075020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][9], 0x8729); /* 0x2075024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][10], 0x17151); /* 0x2075028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][11], 0x2117a); /* 0x207502c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][12], 0x481e); /* 0x2075030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][13], 0x322ab); /* 0x2075034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][14], 0x8b1b); /* 0x2075038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][15], 0x32179); /* 0x207503c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][16], 0x3055b); /* 0x2075040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][17], 0x25c94); /* 0x2075044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][18], 0x3aa3d); /* 0x2075048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][19], 0x31fc6); /* 0x207504c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][20], 0x2085b); /* 0x2075050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][21], 0x17c66); /* 0x2075054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][22], 0x2fd76); /* 0x2075058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][23], 0x2090a); /* 0x207505c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][24], 0x1dcb0); /* 0x2075060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][25], 0x2a7b4); /* 0x2075064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][26], 0xe3c9); /* 0x2075068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][27], 0x1d515); /* 0x207506c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][28], 0x15263); /* 0x2075070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][29], 0x2854c); /* 0x2075074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][30], 0x14f83); /* 0x2075078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][31], 0x295cf); /* 0x207507c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][32], 0xb759); /* 0x2075080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][33], 0x28c9b); /* 0x2075084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][34], 0x28302); /* 0x2075088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][35], 0x235a8); /* 0x207508c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][36], 0x1e2ea); /* 0x2075090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][37], 0x3cfb3); /* 0x2075094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][38], 0x13c50); /* 0x2075098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][39], 0x13d2c); /* 0x207509c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][40], 0xceb1); /* 0x20750a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][41], 0x1eedc); /* 0x20750a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][42], 0x37ee6); /* 0x20750a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][43], 0xc835); /* 0x20750ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][44], 0x36a97); /* 0x20750b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][45], 0x1e356); /* 0x20750b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][46], 0x2d7f6); /* 0x20750b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][47], 0x244fb); /* 0x20750bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][48], 0x2d8c6); /* 0x20750c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][49], 0x11bd5); /* 0x20750c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][50], 0xe4dd); /* 0x20750c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][51], 0x29b29); /* 0x20750cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][0], 0xf119); /* 0x2075100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][1], 0x3e970); /* 0x2075104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][2], 0x17d94); /* 0x2075108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][3], 0x291d6); /* 0x207510c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][4], 0x2b963); /* 0x2075110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][5], 0x18787); /* 0x2075114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][6], 0x1f6cf); /* 0x2075118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][7], 0x14bb0); /* 0x207511c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][8], 0x1c24e); /* 0x2075120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][9], 0x2f4a1); /* 0x2075124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][10], 0xf137); /* 0x2075128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][11], 0x30def); /* 0x207512c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][12], 0x852c); /* 0x2075130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][13], 0x15f79); /* 0x2075134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][14], 0x245ce); /* 0x2075138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][15], 0x35d61); /* 0x207513c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][16], 0x342c8); /* 0x2075140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][17], 0x39941); /* 0x2075144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][18], 0x1e260); /* 0x2075148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][19], 0x2df37); /* 0x207514c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][20], 0x2848c); /* 0x2075150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][21], 0x2230e); /* 0x2075154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][22], 0x1247); /* 0x2075158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][23], 0x6aa7); /* 0x207515c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][24], 0x19b34); /* 0x2075160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][25], 0x2007f); /* 0x2075164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][26], 0x639e); /* 0x2075168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][27], 0x34aa9); /* 0x207516c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][28], 0x1d0a0); /* 0x2075170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][29], 0x1473e); /* 0x2075174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][30], 0x22778); /* 0x2075178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][31], 0x34518); /* 0x207517c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][32], 0x2b559); /* 0x2075180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][33], 0x57e4); /* 0x2075184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][34], 0x16676); /* 0x2075188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][35], 0xdb44); /* 0x207518c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][36], 0x520a); /* 0x2075190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][37], 0x14bb5); /* 0x2075194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][38], 0x2ca34); /* 0x2075198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][39], 0xff69); /* 0x207519c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][40], 0xe0e6); /* 0x20751a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][41], 0x2c551); /* 0x20751a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][42], 0x5ecd); /* 0x20751a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][43], 0x304b8); /* 0x20751ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][44], 0x1ae3d); /* 0x20751b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][45], 0x33585); /* 0x20751b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][46], 0x3fd6d); /* 0x20751b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][47], 0x30b4a); /* 0x20751bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][48], 0x10819); /* 0x20751c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][49], 0x1490a); /* 0x20751c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][50], 0x1fcdd); /* 0x20751c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][51], 0x3ff5f); /* 0x20751cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][0], 0x1fb39); /* 0x2075200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][1], 0x29609); /* 0x2075204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][2], 0x30bf8); /* 0x2075208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][3], 0xc7ba); /* 0x207520c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][4], 0x12636); /* 0x2075210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][5], 0x22d6d); /* 0x2075214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][6], 0x39d3a); /* 0x2075218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][7], 0x161d); /* 0x207521c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][8], 0x1f950); /* 0x2075220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][9], 0xdf3a); /* 0x2075224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][10], 0x63); /* 0x2075228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][11], 0x20306); /* 0x207522c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][12], 0x38ce7); /* 0x2075230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][13], 0x4941); /* 0x2075234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][14], 0x31b58); /* 0x2075238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][15], 0x36e64); /* 0x207523c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][16], 0x2f0d); /* 0x2075240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][17], 0x15382); /* 0x2075244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][18], 0x17dfd); /* 0x2075248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][19], 0x15541); /* 0x207524c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][20], 0x49c1); /* 0x2075250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][21], 0x7995); /* 0x2075254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][22], 0x10e05); /* 0x2075258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][23], 0x27c5e); /* 0x207525c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][24], 0x2e64c); /* 0x2075260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][25], 0x2c962); /* 0x2075264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][26], 0x1169f); /* 0x2075268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][27], 0x9dad); /* 0x207526c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][28], 0x20086); /* 0x2075270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][29], 0xab0); /* 0x2075274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][30], 0x3b3ee); /* 0x2075278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][31], 0x333e5); /* 0x207527c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][32], 0x12d8); /* 0x2075280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][33], 0x3e265); /* 0x2075284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][34], 0x178aa); /* 0x2075288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][35], 0x1340d); /* 0x207528c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][36], 0xd64e); /* 0x2075290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][37], 0xc52b); /* 0x2075294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][38], 0x3ad78); /* 0x2075298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][39], 0x1b002); /* 0x207529c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][40], 0x38fcb); /* 0x20752a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][41], 0x31469); /* 0x20752a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][42], 0x32d0e); /* 0x20752a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][43], 0x6ffe); /* 0x20752ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][44], 0x10c2b); /* 0x20752b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][45], 0x25922); /* 0x20752b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][46], 0x8763); /* 0x20752b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][47], 0x14e6); /* 0x20752bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][48], 0x2b35d); /* 0x20752c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][49], 0x1d63d); /* 0x20752c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][50], 0x12842); /* 0x20752c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][51], 0x208de); /* 0x20752cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][0], 0x1ca47); /* 0x2075300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][1], 0x20b8a); /* 0x2075304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][2], 0x15431); /* 0x2075308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][3], 0x28a5c); /* 0x207530c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][4], 0x13f8c); /* 0x2075310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][5], 0x14246); /* 0x2075314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][6], 0x1fa3d); /* 0x2075318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][7], 0x2571d); /* 0x207531c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][8], 0x2ced2); /* 0x2075320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][9], 0x69c6); /* 0x2075324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][10], 0x29df7); /* 0x2075328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][11], 0x34ffd); /* 0x207532c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][12], 0x10fbf); /* 0x2075330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][13], 0x3308d); /* 0x2075334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][14], 0x2b38a); /* 0x2075338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][15], 0x2d386); /* 0x207533c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][16], 0x12c48); /* 0x2075340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][17], 0x29e54); /* 0x2075344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][18], 0x1d231); /* 0x2075348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][19], 0x214cd); /* 0x207534c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][20], 0x1d305); /* 0x2075350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][21], 0x31e4f); /* 0x2075354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][22], 0x2bdad); /* 0x2075358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][23], 0x282ad); /* 0x207535c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][24], 0x3b900); /* 0x2075360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][25], 0x39d1f); /* 0x2075364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][26], 0x168bf); /* 0x2075368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][27], 0x25d95); /* 0x207536c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][28], 0x56b7); /* 0x2075370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][29], 0x100ed); /* 0x2075374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][30], 0x320a6); /* 0x2075378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][31], 0x37cb5); /* 0x207537c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][32], 0x1e0b7); /* 0x2075380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][33], 0x15783); /* 0x2075384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][34], 0x1b477); /* 0x2075388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][35], 0x10f75); /* 0x207538c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][36], 0xc6f); /* 0x2075390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][37], 0x375ab); /* 0x2075394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][38], 0xa021); /* 0x2075398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][39], 0x27915); /* 0x207539c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][40], 0x38c67); /* 0x20753a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][41], 0x29a52); /* 0x20753a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][42], 0x1e13c); /* 0x20753a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][43], 0x1977); /* 0x20753ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][44], 0x3b1a4); /* 0x20753b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][45], 0x1401f); /* 0x20753b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][46], 0x2f95d); /* 0x20753b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][47], 0x2f144); /* 0x20753bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][48], 0x1be64); /* 0x20753c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][49], 0x2d647); /* 0x20753c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][50], 0x29279); /* 0x20753c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][51], 0x24db9); /* 0x20753cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][0], 0x1a28c); /* 0x2075400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][1], 0x2b473); /* 0x2075404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][2], 0x20eab); /* 0x2075408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][3], 0x6c4); /* 0x207540c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][4], 0x2b41f); /* 0x2075410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][5], 0x3aa2a); /* 0x2075414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][6], 0x3b7d); /* 0x2075418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][7], 0x25d4); /* 0x207541c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][8], 0x32a7e); /* 0x2075420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][9], 0x3f7bd); /* 0x2075424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][10], 0x1db90); /* 0x2075428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][11], 0x1aa82); /* 0x207542c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][12], 0x35020); /* 0x2075430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][13], 0xdeac); /* 0x2075434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][14], 0x32edc); /* 0x2075438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][15], 0x1be02); /* 0x207543c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][16], 0xd320); /* 0x2075440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][17], 0x1db08); /* 0x2075444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][18], 0xc3dc); /* 0x2075448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][19], 0x15610); /* 0x207544c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][20], 0xc043); /* 0x2075450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][21], 0x27663); /* 0x2075454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][22], 0x735b); /* 0x2075458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][23], 0x25a0); /* 0x207545c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][24], 0x2a75d); /* 0x2075460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][25], 0x17327); /* 0x2075464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][26], 0x22285); /* 0x2075468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][27], 0x7d1d); /* 0x207546c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][28], 0x1e76c); /* 0x2075470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][29], 0x9561); /* 0x2075474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][30], 0x35315); /* 0x2075478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][31], 0x254f0); /* 0x207547c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][32], 0x3af87); /* 0x2075480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][33], 0x1ffbd); /* 0x2075484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][34], 0x5067); /* 0x2075488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][35], 0x177e2); /* 0x207548c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][36], 0x3e239); /* 0x2075490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][37], 0x39649); /* 0x2075494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][38], 0x21cc6); /* 0x2075498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][39], 0x3bde2); /* 0x207549c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][40], 0x1e3e); /* 0x20754a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][41], 0x38297); /* 0x20754a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][42], 0x1352); /* 0x20754a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][43], 0x3eb2d); /* 0x20754ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][44], 0x34a74); /* 0x20754b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][45], 0x69e9); /* 0x20754b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][46], 0xc6c9); /* 0x20754b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][47], 0x2b1e8); /* 0x20754bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][48], 0x1ee8b); /* 0x20754c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][49], 0x2a232); /* 0x20754c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][50], 0x1da12); /* 0x20754c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][51], 0x3092b); /* 0x20754cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][0], 0x155f1); /* 0x2075500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][1], 0x1b55e); /* 0x2075504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][2], 0x26516); /* 0x2075508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][3], 0x4c9f); /* 0x207550c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][4], 0x2aa34); /* 0x2075510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][5], 0x2a8d); /* 0x2075514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][6], 0x26f6d); /* 0x2075518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][7], 0x32f37); /* 0x207551c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][8], 0x36a46); /* 0x2075520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][9], 0x4584); /* 0x2075524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][10], 0x3d053); /* 0x2075528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][11], 0x34e43); /* 0x207552c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][12], 0x8312); /* 0x2075530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][13], 0x6ff8); /* 0x2075534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][14], 0x25cc9); /* 0x2075538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][15], 0x774b); /* 0x207553c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][16], 0x242c); /* 0x2075540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][17], 0x1b243); /* 0x2075544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][18], 0x29c52); /* 0x2075548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][19], 0x320bf); /* 0x207554c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][20], 0x393c7); /* 0x2075550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][21], 0x38896); /* 0x2075554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][22], 0x39f9a); /* 0x2075558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][23], 0x36b20); /* 0x207555c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][24], 0xe805); /* 0x2075560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][25], 0x1865f); /* 0x2075564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][26], 0x1a83e); /* 0x2075568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][27], 0x359f3); /* 0x207556c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][28], 0x353d8); /* 0x2075570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][29], 0x155a3); /* 0x2075574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][30], 0x248d3); /* 0x2075578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][31], 0x22d4c); /* 0x207557c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][32], 0x27d3e); /* 0x2075580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][33], 0x2589a); /* 0x2075584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][34], 0x25cf8); /* 0x2075588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][35], 0x49c5); /* 0x207558c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][36], 0x22b27); /* 0x2075590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][37], 0x18874); /* 0x2075594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][38], 0x24a89); /* 0x2075598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][39], 0xee3a); /* 0x207559c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][40], 0x545e); /* 0x20755a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][41], 0x31e06); /* 0x20755a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][42], 0xb97a); /* 0x20755a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][43], 0x8c89); /* 0x20755ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][44], 0x3b4e0); /* 0x20755b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][45], 0x3061a); /* 0x20755b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][46], 0x21bdd); /* 0x20755b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][47], 0x34a23); /* 0x20755bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][48], 0x390e9); /* 0x20755c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][49], 0x34b1a); /* 0x20755c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][50], 0x15921); /* 0x20755c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][51], 0x3d3b5); /* 0x20755cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][0], 0x365fc); /* 0x2075600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][1], 0x3d969); /* 0x2075604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][2], 0x252e7); /* 0x2075608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][3], 0x38a67); /* 0x207560c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][4], 0x1f08d); /* 0x2075610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][5], 0x352c5); /* 0x2075614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][6], 0x384c8); /* 0x2075618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][7], 0x90c); /* 0x207561c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][8], 0x27807); /* 0x2075620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][9], 0x246d); /* 0x2075624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][10], 0x25e0b); /* 0x2075628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][11], 0x9d3a); /* 0x207562c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][12], 0x13661); /* 0x2075630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][13], 0x1d90c); /* 0x2075634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][14], 0x35b7b); /* 0x2075638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][15], 0x2dfbe); /* 0x207563c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][16], 0x1a886); /* 0x2075640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][17], 0x6e24); /* 0x2075644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][18], 0x2d63d); /* 0x2075648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][19], 0xa9fb); /* 0x207564c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][20], 0x12254); /* 0x2075650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][21], 0x2b519); /* 0x2075654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][22], 0x31429); /* 0x2075658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][23], 0x32d38); /* 0x207565c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][24], 0x3bd31); /* 0x2075660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][25], 0x20c6f); /* 0x2075664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][26], 0x15d1b); /* 0x2075668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][27], 0xbac0); /* 0x207566c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][28], 0x4849); /* 0x2075670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][29], 0x23ae6); /* 0x2075674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][30], 0x2b39); /* 0x2075678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][31], 0x3d584); /* 0x207567c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][32], 0x12374); /* 0x2075680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][33], 0x2342a); /* 0x2075684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][34], 0x25d55); /* 0x2075688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][35], 0x3dcf0); /* 0x207568c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][36], 0x3e848); /* 0x2075690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][37], 0x2efee); /* 0x2075694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][38], 0x1dc94); /* 0x2075698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][39], 0x3995b); /* 0x207569c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][40], 0x3b340); /* 0x20756a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][41], 0xe8f3); /* 0x20756a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][42], 0x1d528); /* 0x20756a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][43], 0x2c579); /* 0x20756ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][44], 0x9a74); /* 0x20756b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][45], 0x35dc); /* 0x20756b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][46], 0x55e0); /* 0x20756b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][47], 0x6364); /* 0x20756bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][48], 0x3a94d); /* 0x20756c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][49], 0x32514); /* 0x20756c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][50], 0x2d5bf); /* 0x20756c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][51], 0x8971); /* 0x20756cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][0], 0x250fa); /* 0x2075700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][1], 0x30008); /* 0x2075704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][2], 0x21d04); /* 0x2075708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][3], 0x22dae); /* 0x207570c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][4], 0x9fb7); /* 0x2075710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][5], 0x1f4f9); /* 0x2075714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][6], 0x3172a); /* 0x2075718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][7], 0x3bdd9); /* 0x207571c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][8], 0x2ef88); /* 0x2075720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][9], 0x3e530); /* 0x2075724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][10], 0x1fd74); /* 0x2075728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][11], 0x150b4); /* 0x207572c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][12], 0x1c335); /* 0x2075730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][13], 0x2bf65); /* 0x2075734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][14], 0x9c4e); /* 0x2075738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][15], 0x3c018); /* 0x207573c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][16], 0x38aaa); /* 0x2075740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][17], 0xaf9); /* 0x2075744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][18], 0x7765); /* 0x2075748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][19], 0x970d); /* 0x207574c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][20], 0x29ed5); /* 0x2075750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][21], 0x20a9f); /* 0x2075754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][22], 0x274d8); /* 0x2075758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][23], 0x1050b); /* 0x207575c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][24], 0x29a2e); /* 0x2075760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][25], 0xc42c); /* 0x2075764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][26], 0xb650); /* 0x2075768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][27], 0xdc7b); /* 0x207576c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][28], 0x29982); /* 0x2075770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][29], 0x2afe8); /* 0x2075774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][30], 0x31131); /* 0x2075778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][31], 0xf0ef); /* 0x207577c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][32], 0x2f71e); /* 0x2075780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][33], 0x3af55); /* 0x2075784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][34], 0x3268d); /* 0x2075788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][35], 0x1433f); /* 0x207578c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][36], 0x2578f); /* 0x2075790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][37], 0x16901); /* 0x2075794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][38], 0x37adf); /* 0x2075798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][39], 0x31f1e); /* 0x207579c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][40], 0x9fb0); /* 0x20757a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][41], 0x1c278); /* 0x20757a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][42], 0x168d6); /* 0x20757a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][43], 0x1768e); /* 0x20757ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][44], 0x8a3a); /* 0x20757b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][45], 0x37b2b); /* 0x20757b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][46], 0x2a67a); /* 0x20757b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][47], 0x35732); /* 0x20757bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][48], 0xe3a8); /* 0x20757c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][49], 0x28b19); /* 0x20757c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][50], 0x12814); /* 0x20757c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][51], 0x3eca0); /* 0x20757cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][0], 0x969f); /* 0x2075800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][1], 0x228c2); /* 0x2075804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][2], 0x2d8cf); /* 0x2075808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][3], 0x3b95b); /* 0x207580c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][4], 0x3033b); /* 0x2075810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][5], 0x28a07); /* 0x2075814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][6], 0x2eb44); /* 0x2075818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][7], 0x5206); /* 0x207581c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][8], 0x30b96); /* 0x2075820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][9], 0x15f2f); /* 0x2075824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][10], 0x12204); /* 0x2075828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][11], 0x217d3); /* 0x207582c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][12], 0x2a714); /* 0x2075830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][13], 0x2d636); /* 0x2075834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][14], 0x3ac45); /* 0x2075838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][15], 0x19d92); /* 0x207583c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][16], 0x7eb2); /* 0x2075840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][17], 0x184c7); /* 0x2075844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][18], 0x27e79); /* 0x2075848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][19], 0x3f9ab); /* 0x207584c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][20], 0x29d0c); /* 0x2075850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][21], 0xc508); /* 0x2075854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][22], 0x9cc); /* 0x2075858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][23], 0x2ce5e); /* 0x207585c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][24], 0x2d98e); /* 0x2075860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][25], 0x1a7a4); /* 0x2075864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][26], 0xa027); /* 0x2075868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][27], 0x573); /* 0x207586c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][28], 0x9a38); /* 0x2075870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][29], 0xd395); /* 0x2075874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][30], 0x25a8f); /* 0x2075878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][31], 0x15d8f); /* 0x207587c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][32], 0x1eadb); /* 0x2075880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][33], 0x28870); /* 0x2075884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][34], 0x2fc8); /* 0x2075888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][35], 0x1f7bc); /* 0x207588c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][36], 0x7a43); /* 0x2075890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][37], 0xbb0a); /* 0x2075894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][38], 0xce28); /* 0x2075898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][39], 0x1e3c8); /* 0x207589c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][40], 0x4a5f); /* 0x20758a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][41], 0x4160); /* 0x20758a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][42], 0x14439); /* 0x20758a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][43], 0x89ec); /* 0x20758ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][44], 0x7425); /* 0x20758b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][45], 0x14011); /* 0x20758b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][46], 0x1275b); /* 0x20758b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][47], 0x335fb); /* 0x20758bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][48], 0x20280); /* 0x20758c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][49], 0x3e076); /* 0x20758c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][50], 0x7c1d); /* 0x20758c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][51], 0x3f5d3); /* 0x20758cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][0], 0x372fb); /* 0x2075900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][1], 0x27103); /* 0x2075904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][2], 0x2767d); /* 0x2075908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][3], 0x37435); /* 0x207590c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][4], 0xbe79); /* 0x2075910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][5], 0x3df91); /* 0x2075914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][6], 0x135ae); /* 0x2075918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][7], 0xe608); /* 0x207591c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][8], 0x29da1); /* 0x2075920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][9], 0x4c54); /* 0x2075924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][10], 0xb7d4); /* 0x2075928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][11], 0xaefa); /* 0x207592c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][12], 0x36888); /* 0x2075930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][13], 0x1dbb3); /* 0x2075934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][14], 0x2689); /* 0x2075938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][15], 0x28854); /* 0x207593c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][16], 0x3ffad); /* 0x2075940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][17], 0x148cf); /* 0x2075944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][18], 0x29dbe); /* 0x2075948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][19], 0x92e9); /* 0x207594c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][20], 0x7a08); /* 0x2075950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][21], 0x14cd5); /* 0x2075954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][22], 0xfb73); /* 0x2075958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][23], 0x2c478); /* 0x207595c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][24], 0x15fc4); /* 0x2075960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][25], 0x16e9b); /* 0x2075964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][26], 0x21be0); /* 0x2075968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][27], 0x37a5c); /* 0x207596c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][28], 0x2f7e8); /* 0x2075970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][29], 0x29709); /* 0x2075974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][30], 0x2cdcd); /* 0x2075978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][31], 0x25eda); /* 0x207597c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][32], 0x3b44f); /* 0x2075980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][33], 0x1eca1); /* 0x2075984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][34], 0x38c79); /* 0x2075988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][35], 0x113c5); /* 0x207598c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][36], 0x1a581); /* 0x2075990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][37], 0x2e1eb); /* 0x2075994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][38], 0x3e37); /* 0x2075998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][39], 0x395a8); /* 0x207599c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][40], 0x2aee0); /* 0x20759a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][41], 0x26786); /* 0x20759a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][42], 0x31014); /* 0x20759a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][43], 0x5dd7); /* 0x20759ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][44], 0x1c3c4); /* 0x20759b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][45], 0x3115); /* 0x20759b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][46], 0xc70e); /* 0x20759b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][47], 0x14d1c); /* 0x20759bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][48], 0xf708); /* 0x20759c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][49], 0x4d6c); /* 0x20759c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][50], 0x3a91d); /* 0x20759c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][51], 0x600d); /* 0x20759cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][0], 0x3cd01); /* 0x2075a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][1], 0x1690); /* 0x2075a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][2], 0x30ba6); /* 0x2075a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][3], 0x24333); /* 0x2075a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][4], 0x14597); /* 0x2075a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][5], 0x37a46); /* 0x2075a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][6], 0x2d52b); /* 0x2075a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][7], 0x1dc73); /* 0x2075a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][8], 0x5e15); /* 0x2075a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][9], 0x16319); /* 0x2075a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][10], 0xfd0d); /* 0x2075a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][11], 0x3f511); /* 0x2075a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][12], 0x260dc); /* 0x2075a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][13], 0x1fe78); /* 0x2075a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][14], 0x394ae); /* 0x2075a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][15], 0x54f8); /* 0x2075a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][16], 0x3cd46); /* 0x2075a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][17], 0x1246d); /* 0x2075a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][18], 0x1b930); /* 0x2075a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][19], 0x176d6); /* 0x2075a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][20], 0x1b236); /* 0x2075a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][21], 0x336fa); /* 0x2075a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][22], 0x6a10); /* 0x2075a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][23], 0x3bb4d); /* 0x2075a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][24], 0x3c7e2); /* 0x2075a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][25], 0x2a86e); /* 0x2075a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][26], 0x2d5c3); /* 0x2075a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][27], 0x3af28); /* 0x2075a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][28], 0x37134); /* 0x2075a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][29], 0x386d0); /* 0x2075a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][30], 0x22301); /* 0x2075a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][31], 0x2d928); /* 0x2075a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][32], 0x299e4); /* 0x2075a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][33], 0xb1e4); /* 0x2075a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][34], 0x3d75); /* 0x2075a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][35], 0x92c0); /* 0x2075a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][36], 0x12364); /* 0x2075a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][37], 0x16e17); /* 0x2075a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][38], 0x14309); /* 0x2075a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][39], 0x1eaeb); /* 0x2075a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][40], 0x2393d); /* 0x2075aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][41], 0x1f706); /* 0x2075aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][42], 0xaf5e); /* 0x2075aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][43], 0x22f7d); /* 0x2075aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][44], 0x28948); /* 0x2075ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][45], 0x11e7e); /* 0x2075ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][46], 0x1327c); /* 0x2075ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][47], 0x1e458); /* 0x2075abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][48], 0xa1c); /* 0x2075ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][49], 0x2533c); /* 0x2075ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][50], 0xe53c); /* 0x2075ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][51], 0x18125); /* 0x2075acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][0], 0x31817); /* 0x2075b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][1], 0xf062); /* 0x2075b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][2], 0x1c50c); /* 0x2075b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][3], 0x2dcc); /* 0x2075b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][4], 0x2e0c5); /* 0x2075b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][5], 0x24266); /* 0x2075b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][6], 0x12ac9); /* 0x2075b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][7], 0x3dede); /* 0x2075b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][8], 0x9ff); /* 0x2075b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][9], 0x22efe); /* 0x2075b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][10], 0x314cf); /* 0x2075b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][11], 0x204ab); /* 0x2075b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][12], 0x2d7fc); /* 0x2075b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][13], 0x3f4fd); /* 0x2075b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][14], 0x23a61); /* 0x2075b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][15], 0x3fb3); /* 0x2075b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][16], 0x39864); /* 0x2075b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][17], 0x9b79); /* 0x2075b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][18], 0x3fb6e); /* 0x2075b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][19], 0xc5eb); /* 0x2075b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][20], 0x16c28); /* 0x2075b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][21], 0x2aa32); /* 0x2075b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][22], 0x36f74); /* 0x2075b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][23], 0x5325); /* 0x2075b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][24], 0xb6e4); /* 0x2075b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][25], 0x1145); /* 0x2075b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][26], 0x11be); /* 0x2075b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][27], 0x3fc4e); /* 0x2075b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][28], 0x2baea); /* 0x2075b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][29], 0xd0fe); /* 0x2075b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][30], 0x171d); /* 0x2075b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][31], 0x1d3c7); /* 0x2075b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][32], 0xee22); /* 0x2075b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][33], 0x3569); /* 0x2075b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][34], 0x8d30); /* 0x2075b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][35], 0x1241a); /* 0x2075b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][36], 0x27284); /* 0x2075b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][37], 0x1be02); /* 0x2075b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][38], 0x34710); /* 0x2075b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][39], 0x10372); /* 0x2075b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][40], 0x15f89); /* 0x2075ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][41], 0x3cfbd); /* 0x2075ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][42], 0x3a6f9); /* 0x2075ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][43], 0x3bcae); /* 0x2075bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][44], 0x38c1d); /* 0x2075bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][45], 0x3e717); /* 0x2075bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][46], 0x3ffab); /* 0x2075bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][47], 0x88e5); /* 0x2075bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][48], 0x19ec7); /* 0x2075bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][49], 0x15ddc); /* 0x2075bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][50], 0x3fee0); /* 0x2075bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][51], 0xba06); /* 0x2075bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][0], 0x369bd); /* 0x2075c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][1], 0x6186); /* 0x2075c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][2], 0x9f67); /* 0x2075c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][3], 0x2438a); /* 0x2075c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][4], 0x111bb); /* 0x2075c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][5], 0x357eb); /* 0x2075c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][6], 0x373ba); /* 0x2075c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][7], 0x1b7d3); /* 0x2075c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][8], 0x3899f); /* 0x2075c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][9], 0x2ba49); /* 0x2075c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][10], 0xd29a); /* 0x2075c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][11], 0x19fe8); /* 0x2075c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][12], 0x3f4f9); /* 0x2075c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][13], 0x2529); /* 0x2075c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][14], 0x3697b); /* 0x2075c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][15], 0x11a0); /* 0x2075c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][16], 0x38710); /* 0x2075c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][17], 0x7ad0); /* 0x2075c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][18], 0x108f8); /* 0x2075c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][19], 0x3b6c9); /* 0x2075c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][20], 0x21e11); /* 0x2075c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][21], 0x6e6); /* 0x2075c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][22], 0x1cfc0); /* 0x2075c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][23], 0x1f091); /* 0x2075c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][24], 0x39519); /* 0x2075c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][25], 0x3a477); /* 0x2075c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][26], 0x30278); /* 0x2075c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][27], 0x2d15); /* 0x2075c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][28], 0x1ae17); /* 0x2075c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][29], 0x3d22f); /* 0x2075c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][30], 0x3f70b); /* 0x2075c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][31], 0x3abfe); /* 0x2075c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][32], 0x13e32); /* 0x2075c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][33], 0x22627); /* 0x2075c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][34], 0x3a8d2); /* 0x2075c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][35], 0x33c5f); /* 0x2075c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][36], 0x3161e); /* 0x2075c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][37], 0x193b7); /* 0x2075c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][38], 0x39cc9); /* 0x2075c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][39], 0x244be); /* 0x2075c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][40], 0x13223); /* 0x2075ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][41], 0x13d4c); /* 0x2075ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][42], 0x20be4); /* 0x2075ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][43], 0x216c1); /* 0x2075cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][44], 0x4d7c); /* 0x2075cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][45], 0x32f8); /* 0x2075cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][46], 0x12a54); /* 0x2075cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][47], 0x22526); /* 0x2075cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][48], 0x1ae9c); /* 0x2075cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][49], 0x28801); /* 0x2075cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][50], 0x1a1c); /* 0x2075cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][51], 0x4607); /* 0x2075ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][0], 0x2b849); /* 0x2075d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][1], 0xfee0); /* 0x2075d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][2], 0xb9ea); /* 0x2075d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][3], 0x3270f); /* 0x2075d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][4], 0x3dd13); /* 0x2075d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][5], 0xcd2a); /* 0x2075d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][6], 0x1f19d); /* 0x2075d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][7], 0x2e868); /* 0x2075d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][8], 0x2b0e1); /* 0x2075d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][9], 0x27474); /* 0x2075d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][10], 0x1c6ea); /* 0x2075d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][11], 0x2d014); /* 0x2075d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][12], 0x2ea9f); /* 0x2075d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][13], 0x20251); /* 0x2075d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][14], 0x1031c); /* 0x2075d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][15], 0x2beab); /* 0x2075d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][16], 0x220ac); /* 0x2075d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][17], 0x5c4c); /* 0x2075d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][18], 0x1ded9); /* 0x2075d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][19], 0x31c4c); /* 0x2075d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][20], 0xe122); /* 0x2075d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][21], 0x2787b); /* 0x2075d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][22], 0x2b4c3); /* 0x2075d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][23], 0x248a2); /* 0x2075d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][24], 0x29983); /* 0x2075d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][25], 0x3e689); /* 0x2075d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][26], 0x31050); /* 0x2075d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][27], 0xf59); /* 0x2075d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][28], 0x3f353); /* 0x2075d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][29], 0x3d7a3); /* 0x2075d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][30], 0x10734); /* 0x2075d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][31], 0x3f8c3); /* 0x2075d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][32], 0x2e9b2); /* 0x2075d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][33], 0x11129); /* 0x2075d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][34], 0x2d1cf); /* 0x2075d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][35], 0x2b303); /* 0x2075d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][36], 0x3a41b); /* 0x2075d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][37], 0x1e8e5); /* 0x2075d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][38], 0x6ff9); /* 0x2075d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][39], 0x28519); /* 0x2075d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][40], 0x38c6d); /* 0x2075da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][41], 0x6eb0); /* 0x2075da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][42], 0x17df); /* 0x2075da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][43], 0xb22f); /* 0x2075dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][44], 0x296cd); /* 0x2075db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][45], 0x7410); /* 0x2075db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][46], 0xe51b); /* 0x2075db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][47], 0x14757); /* 0x2075dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][48], 0x2c817); /* 0x2075dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][49], 0xe1ca); /* 0x2075dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][50], 0x16741); /* 0x2075dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][51], 0x31272); /* 0x2075dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][0], 0x343e); /* 0x2075e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][1], 0x30875); /* 0x2075e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][2], 0x25492); /* 0x2075e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][3], 0xd5a); /* 0x2075e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][4], 0x3bd15); /* 0x2075e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][5], 0x1102e); /* 0x2075e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][6], 0x38b08); /* 0x2075e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][7], 0x1a6e); /* 0x2075e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][8], 0x310d4); /* 0x2075e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][9], 0x1be8c); /* 0x2075e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][10], 0x1d48c); /* 0x2075e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][11], 0x5904); /* 0x2075e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][12], 0x34e36); /* 0x2075e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][13], 0x11668); /* 0x2075e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][14], 0x3909f); /* 0x2075e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][15], 0x3f4ab); /* 0x2075e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][16], 0x617b); /* 0x2075e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][17], 0x13e3a); /* 0x2075e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][18], 0x2beb4); /* 0x2075e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][19], 0x36ca2); /* 0x2075e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][20], 0x217ab); /* 0x2075e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][21], 0x3c9b6); /* 0x2075e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][22], 0x2a520); /* 0x2075e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][23], 0x243a); /* 0x2075e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][24], 0xa1b9); /* 0x2075e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][25], 0xd6aa); /* 0x2075e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][26], 0x24e46); /* 0x2075e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][27], 0xb3a3); /* 0x2075e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][28], 0xd494); /* 0x2075e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][29], 0x1c6e8); /* 0x2075e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][30], 0x2375d); /* 0x2075e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][31], 0x33386); /* 0x2075e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][32], 0x5533); /* 0x2075e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][33], 0x160f); /* 0x2075e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][34], 0x3ddd5); /* 0x2075e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][35], 0x267df); /* 0x2075e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][36], 0x138f7); /* 0x2075e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][37], 0x3c6f7); /* 0x2075e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][38], 0x1bdf8); /* 0x2075e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][39], 0x2e11); /* 0x2075e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][40], 0x280ad); /* 0x2075ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][41], 0x24240); /* 0x2075ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][42], 0x2509e); /* 0x2075ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][43], 0x377f7); /* 0x2075eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][44], 0x2d766); /* 0x2075eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][45], 0xd0eb); /* 0x2075eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][46], 0x317d6); /* 0x2075eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][47], 0x2977); /* 0x2075ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][48], 0x35cd3); /* 0x2075ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][49], 0x2a355); /* 0x2075ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][50], 0x3f9b4); /* 0x2075ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][51], 0x18c78); /* 0x2075ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][0], 0x1610); /* 0x2075f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][1], 0xf239); /* 0x2075f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][2], 0x2b076); /* 0x2075f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][3], 0x877e); /* 0x2075f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][4], 0x32216); /* 0x2075f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][5], 0x428b); /* 0x2075f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][6], 0x17); /* 0x2075f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][7], 0x2fc6e); /* 0x2075f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][8], 0xa3f3); /* 0x2075f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][9], 0x1ec7c); /* 0x2075f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][10], 0xf464); /* 0x2075f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][11], 0x2bba3); /* 0x2075f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][12], 0xd27b); /* 0x2075f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][13], 0xc373); /* 0x2075f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][14], 0x1af27); /* 0x2075f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][15], 0x1d67b); /* 0x2075f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][16], 0x1e833); /* 0x2075f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][17], 0x1afe9); /* 0x2075f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][18], 0xdcee); /* 0x2075f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][19], 0x3fbb); /* 0x2075f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][20], 0x348b5); /* 0x2075f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][21], 0x31fbe); /* 0x2075f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][22], 0x1c0e4); /* 0x2075f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][23], 0x292df); /* 0x2075f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][24], 0x35319); /* 0x2075f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][25], 0xe498); /* 0x2075f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][26], 0x2031f); /* 0x2075f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][27], 0x279cb); /* 0x2075f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][28], 0x35509); /* 0x2075f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][29], 0xaa5d); /* 0x2075f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][30], 0x2414b); /* 0x2075f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][31], 0xed40); /* 0x2075f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][32], 0x11269); /* 0x2075f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][33], 0x18d76); /* 0x2075f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][34], 0x3b4f8); /* 0x2075f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][35], 0x1c0f3); /* 0x2075f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][36], 0xb73); /* 0x2075f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][37], 0x5bbb); /* 0x2075f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][38], 0xd5c8); /* 0x2075f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][39], 0x2271a); /* 0x2075f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][40], 0x200a); /* 0x2075fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][41], 0xc67e); /* 0x2075fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][42], 0x7c06); /* 0x2075fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][43], 0x2df4d); /* 0x2075fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][44], 0x82d5); /* 0x2075fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][45], 0x3c6bd); /* 0x2075fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][46], 0x227cf); /* 0x2075fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][47], 0x2d316); /* 0x2075fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][48], 0x12ea9); /* 0x2075fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][49], 0x3010c); /* 0x2075fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][50], 0x7514); /* 0x2075fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][51], 0x57d5); /* 0x2075fcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][0], 0x206ea); /* 0x2076000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][1], 0x33485); /* 0x2076004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][2], 0x374d0); /* 0x2076008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][3], 0x235a0); /* 0x207600c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][4], 0x2005f); /* 0x2076010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][5], 0xac6c); /* 0x2076014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][6], 0x2acef); /* 0x2076018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][7], 0x19638); /* 0x207601c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][8], 0x19194); /* 0x2076020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][9], 0x33d2); /* 0x2076024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][10], 0x37c97); /* 0x2076028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][11], 0xe29c); /* 0x207602c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][12], 0x3999); /* 0x2076030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][13], 0xd317); /* 0x2076034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][14], 0x298c6); /* 0x2076038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][15], 0x102c8); /* 0x207603c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][16], 0x37227); /* 0x2076040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][17], 0x887f); /* 0x2076044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][18], 0x1d2a1); /* 0x2076048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][19], 0x13d91); /* 0x207604c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][20], 0x3abc5); /* 0x2076050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][21], 0x17c40); /* 0x2076054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][22], 0x20a8d); /* 0x2076058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][23], 0x2fe63); /* 0x207605c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][24], 0x3d411); /* 0x2076060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][25], 0xb5ac); /* 0x2076064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][26], 0xb59c); /* 0x2076068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][27], 0xa842); /* 0x207606c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][28], 0x5919); /* 0x2076070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][29], 0x173c1); /* 0x2076074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][30], 0xf532); /* 0x2076078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][31], 0x30069); /* 0x207607c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][32], 0x7bb8); /* 0x2076080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][33], 0x1f4ca); /* 0x2076084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][34], 0xd782); /* 0x2076088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][35], 0x3e260); /* 0x207608c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][36], 0x2ae41); /* 0x2076090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][37], 0x1761f); /* 0x2076094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][38], 0x3329c); /* 0x2076098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][39], 0x30e77); /* 0x207609c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][40], 0x1878d); /* 0x20760a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][41], 0x269e); /* 0x20760a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][42], 0x372f3); /* 0x20760a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][43], 0x1e97c); /* 0x20760ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][44], 0x19f6c); /* 0x20760b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][45], 0xeb7b); /* 0x20760b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][46], 0xaf36); /* 0x20760b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][47], 0x231fb); /* 0x20760bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][48], 0x34c40); /* 0x20760c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][49], 0x2c248); /* 0x20760c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][50], 0xecf0); /* 0x20760c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][51], 0xe299); /* 0x20760cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][0], 0x17051); /* 0x2076100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][1], 0x240f9); /* 0x2076104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][2], 0x24d6); /* 0x2076108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][3], 0x1bb89); /* 0x207610c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][4], 0x3017); /* 0x2076110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][5], 0x3dee1); /* 0x2076114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][6], 0x2d70c); /* 0x2076118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][7], 0x104bc); /* 0x207611c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][8], 0x1c5a8); /* 0x2076120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][9], 0x504f); /* 0x2076124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][10], 0x1c488); /* 0x2076128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][11], 0x8555); /* 0x207612c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][12], 0x6fff); /* 0x2076130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][13], 0x1c7d4); /* 0x2076134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][14], 0x315ae); /* 0x2076138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][15], 0x33c2); /* 0x207613c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][16], 0x2c43d); /* 0x2076140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][17], 0x2e7b1); /* 0x2076144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][18], 0x1900a); /* 0x2076148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][19], 0x1f38c); /* 0x207614c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][20], 0x1bca0); /* 0x2076150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][21], 0x2150b); /* 0x2076154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][22], 0x30e01); /* 0x2076158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][23], 0x331fc); /* 0x207615c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][24], 0xda72); /* 0x2076160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][25], 0x101aa); /* 0x2076164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][26], 0x1d77e); /* 0x2076168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][27], 0x32831); /* 0x207616c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][28], 0x3c96d); /* 0x2076170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][29], 0x56d9); /* 0x2076174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][30], 0x2006); /* 0x2076178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][31], 0x260b8); /* 0x207617c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][32], 0x262ec); /* 0x2076180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][33], 0xffd3); /* 0x2076184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][34], 0x22c99); /* 0x2076188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][35], 0x34747); /* 0x207618c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][36], 0xf15); /* 0x2076190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][37], 0xac40); /* 0x2076194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][38], 0x3ea9a); /* 0x2076198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][39], 0xd7b1); /* 0x207619c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][40], 0x14cc0); /* 0x20761a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][41], 0x194e8); /* 0x20761a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][42], 0x1867f); /* 0x20761a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][43], 0x26133); /* 0x20761ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][44], 0xf644); /* 0x20761b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][45], 0x1670f); /* 0x20761b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][46], 0x8eb9); /* 0x20761b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][47], 0x121e4); /* 0x20761bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][48], 0x14e02); /* 0x20761c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][49], 0x9f31); /* 0x20761c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][50], 0x6b2f); /* 0x20761c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][51], 0x34ec0); /* 0x20761cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][0], 0x3785b); /* 0x2076200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][1], 0x1e237); /* 0x2076204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][2], 0x17c7); /* 0x2076208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][3], 0x33142); /* 0x207620c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][4], 0x142ad); /* 0x2076210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][5], 0x3fb5f); /* 0x2076214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][6], 0x3f20f); /* 0x2076218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][7], 0x1bd32); /* 0x207621c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][8], 0x19ae); /* 0x2076220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][9], 0x1126e); /* 0x2076224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][10], 0xc1b4); /* 0x2076228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][11], 0x3c4e0); /* 0x207622c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][12], 0x3e98); /* 0x2076230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][13], 0x175e6); /* 0x2076234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][14], 0x3b27a); /* 0x2076238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][15], 0x162db); /* 0x207623c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][16], 0x2115d); /* 0x2076240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][17], 0x18e5f); /* 0x2076244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][18], 0x31597); /* 0x2076248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][19], 0xcaed); /* 0x207624c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][20], 0x12825); /* 0x2076250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][21], 0x28198); /* 0x2076254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][22], 0x2e8b1); /* 0x2076258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][23], 0x1eddc); /* 0x207625c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][24], 0x2c39); /* 0x2076260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][25], 0x1b4); /* 0x2076264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][26], 0x175ce); /* 0x2076268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][27], 0x1f05f); /* 0x207626c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][28], 0x41b6); /* 0x2076270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][29], 0x98a2); /* 0x2076274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][30], 0x2db09); /* 0x2076278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][31], 0x38a32); /* 0x207627c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][32], 0x1fb21); /* 0x2076280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][33], 0x3ea7f); /* 0x2076284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][34], 0x460e); /* 0x2076288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][35], 0x146e4); /* 0x207628c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][36], 0x216fa); /* 0x2076290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][37], 0x3c0c8); /* 0x2076294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][38], 0x25d42); /* 0x2076298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][39], 0x3056c); /* 0x207629c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][40], 0x71de); /* 0x20762a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][41], 0x2aaba); /* 0x20762a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][42], 0x24aaa); /* 0x20762a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][43], 0x4421); /* 0x20762ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][44], 0x8ba9); /* 0x20762b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][45], 0x1f9a3); /* 0x20762b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][46], 0x1d850); /* 0x20762b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][47], 0x26fce); /* 0x20762bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][48], 0x1b6cb); /* 0x20762c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][49], 0xa0cb); /* 0x20762c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][50], 0x681b); /* 0x20762c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][51], 0x3afea); /* 0x20762cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][0], 0x1356f); /* 0x2076300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][1], 0x3137f); /* 0x2076304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][2], 0x1bcd9); /* 0x2076308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][3], 0x13289); /* 0x207630c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][4], 0x26650); /* 0x2076310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][5], 0x39ab1); /* 0x2076314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][6], 0x2c3f7); /* 0x2076318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][7], 0x18c4); /* 0x207631c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][8], 0x144de); /* 0x2076320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][9], 0x3f7a2); /* 0x2076324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][10], 0x2df47); /* 0x2076328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][11], 0x19354); /* 0x207632c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][12], 0xa8db); /* 0x2076330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][13], 0x37f7f); /* 0x2076334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][14], 0x186e8); /* 0x2076338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][15], 0x1ef4c); /* 0x207633c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][16], 0x67f6); /* 0x2076340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][17], 0x2788); /* 0x2076344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][18], 0x3fc54); /* 0x2076348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][19], 0x2418f); /* 0x207634c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][20], 0x30731); /* 0x2076350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][21], 0x2d382); /* 0x2076354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][22], 0x149e2); /* 0x2076358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][23], 0xc758); /* 0x207635c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][24], 0x1920); /* 0x2076360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][25], 0x2d94b); /* 0x2076364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][26], 0x23c4); /* 0x2076368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][27], 0x2e3fb); /* 0x207636c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][28], 0x3afba); /* 0x2076370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][29], 0x5183); /* 0x2076374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][30], 0x166f6); /* 0x2076378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][31], 0xe6e0); /* 0x207637c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][32], 0x15716); /* 0x2076380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][33], 0x3c77d); /* 0x2076384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][34], 0x2d7cc); /* 0x2076388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][35], 0x3cef3); /* 0x207638c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][36], 0xf8d); /* 0x2076390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][37], 0x94af); /* 0x2076394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][38], 0x1e8cb); /* 0x2076398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][39], 0x31a27); /* 0x207639c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][40], 0x6c8); /* 0x20763a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][41], 0x2287d); /* 0x20763a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][42], 0x390f9); /* 0x20763a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][43], 0xe809); /* 0x20763ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][44], 0x42e4); /* 0x20763b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][45], 0x1e9dd); /* 0x20763b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][46], 0x36f72); /* 0x20763b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][47], 0x329ce); /* 0x20763bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][48], 0x33c6c); /* 0x20763c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][49], 0x378c3); /* 0x20763c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][50], 0x17b58); /* 0x20763c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][51], 0x11cbb); /* 0x20763cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][0], 0x2dd9d); /* 0x2076400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][1], 0x3249f); /* 0x2076404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][2], 0x31ec4); /* 0x2076408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][3], 0xce71); /* 0x207640c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][4], 0x3cf0c); /* 0x2076410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][5], 0x387e5); /* 0x2076414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][6], 0x214e3); /* 0x2076418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][7], 0x3762b); /* 0x207641c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][8], 0x3191a); /* 0x2076420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][9], 0xd115); /* 0x2076424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][10], 0x2ed37); /* 0x2076428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][11], 0x32848); /* 0x207642c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][12], 0x35d2d); /* 0x2076430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][13], 0x3cbe9); /* 0x2076434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][14], 0x1e12c); /* 0x2076438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][15], 0x3b2fe); /* 0x207643c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][16], 0x228a9); /* 0x2076440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][17], 0x115aa); /* 0x2076444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][18], 0x23df2); /* 0x2076448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][19], 0x2ede8); /* 0x207644c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][20], 0x21d9); /* 0x2076450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][21], 0x735e); /* 0x2076454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][22], 0x2729); /* 0x2076458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][23], 0x1d485); /* 0x207645c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][24], 0x118f3); /* 0x2076460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][25], 0x23b9e); /* 0x2076464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][26], 0x39178); /* 0x2076468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][27], 0x95a4); /* 0x207646c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][28], 0x13cd3); /* 0x2076470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][29], 0x2135c); /* 0x2076474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][30], 0x3de1c); /* 0x2076478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][31], 0x283c4); /* 0x207647c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][32], 0x30807); /* 0x2076480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][33], 0x3b796); /* 0x2076484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][34], 0x299e9); /* 0x2076488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][35], 0x4121); /* 0x207648c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][36], 0x18064); /* 0x2076490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][37], 0x27e4a); /* 0x2076494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][38], 0x8677); /* 0x2076498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][39], 0xccf4); /* 0x207649c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][40], 0x3d929); /* 0x20764a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][41], 0x15e62); /* 0x20764a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][42], 0x3a216); /* 0x20764a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][43], 0x2346a); /* 0x20764ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][44], 0xab97); /* 0x20764b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][45], 0x1703f); /* 0x20764b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][46], 0x2acab); /* 0x20764b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][47], 0x5893); /* 0x20764bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][48], 0x2dea0); /* 0x20764c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][49], 0x10898); /* 0x20764c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][50], 0x12ab8); /* 0x20764c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][51], 0x32434); /* 0x20764cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][0], 0x155f2); /* 0x2076500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][1], 0x3216c); /* 0x2076504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][2], 0x2c3bd); /* 0x2076508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][3], 0x25669); /* 0x207650c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][4], 0x2324a); /* 0x2076510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][5], 0x5bbb); /* 0x2076514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][6], 0x3f696); /* 0x2076518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][7], 0x7841); /* 0x207651c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][8], 0x2a497); /* 0x2076520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][9], 0x1aa99); /* 0x2076524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][10], 0x2a5ae); /* 0x2076528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][11], 0xeef2); /* 0x207652c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][12], 0x1d4a3); /* 0x2076530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][13], 0x24d9f); /* 0x2076534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][14], 0xd29f); /* 0x2076538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][15], 0x381da); /* 0x207653c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][16], 0x4268); /* 0x2076540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][17], 0x39626); /* 0x2076544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][18], 0x9676); /* 0x2076548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][19], 0x2719); /* 0x207654c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][20], 0x3aac7); /* 0x2076550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][21], 0x1c8cf); /* 0x2076554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][22], 0x3a29d); /* 0x2076558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][23], 0x3ea72); /* 0x207655c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][24], 0x27106); /* 0x2076560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][25], 0xd0ac); /* 0x2076564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][26], 0x3a08e); /* 0x2076568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][27], 0x3f750); /* 0x207656c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][28], 0x36118); /* 0x2076570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][29], 0x25c); /* 0x2076574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][30], 0x35958); /* 0x2076578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][31], 0x10e5d); /* 0x207657c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][32], 0x275a0); /* 0x2076580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][33], 0x1ce3e); /* 0x2076584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][34], 0x6f56); /* 0x2076588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][35], 0x19558); /* 0x207658c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][36], 0x2362); /* 0x2076590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][37], 0x3053d); /* 0x2076594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][38], 0x1c956); /* 0x2076598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][39], 0x256dd); /* 0x207659c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][40], 0x21962); /* 0x20765a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][41], 0x22491); /* 0x20765a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][42], 0x1663d); /* 0x20765a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][43], 0x10a72); /* 0x20765ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][44], 0x105c6); /* 0x20765b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][45], 0x32bd3); /* 0x20765b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][46], 0x17c85); /* 0x20765b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][47], 0x1a519); /* 0x20765bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][48], 0x2bb0f); /* 0x20765c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][49], 0x3daba); /* 0x20765c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][50], 0x343f6); /* 0x20765c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][51], 0x3692b); /* 0x20765cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][0], 0x1ab22); /* 0x2076600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][1], 0x22bec); /* 0x2076604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][2], 0x1d913); /* 0x2076608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][3], 0x1bdfe); /* 0x207660c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][4], 0x15787); /* 0x2076610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][5], 0x39e98); /* 0x2076614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][6], 0x98d3); /* 0x2076618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][7], 0x2b2fc); /* 0x207661c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][8], 0x1f6d2); /* 0x2076620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][9], 0x38270); /* 0x2076624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][10], 0x216a4); /* 0x2076628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][11], 0x3cf2c); /* 0x207662c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][12], 0x93a8); /* 0x2076630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][13], 0xf769); /* 0x2076634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][14], 0x21167); /* 0x2076638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][15], 0x26afa); /* 0x207663c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][16], 0x21d58); /* 0x2076640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][17], 0xc76b); /* 0x2076644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][18], 0x6ecf); /* 0x2076648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][19], 0x3cdc4); /* 0x207664c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][20], 0x3f075); /* 0x2076650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][21], 0xd42d); /* 0x2076654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][22], 0xbd60); /* 0x2076658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][23], 0x168c8); /* 0x207665c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][24], 0x1eff9); /* 0x2076660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][25], 0x3238a); /* 0x2076664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][26], 0x2efc7); /* 0x2076668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][27], 0x87a7); /* 0x207666c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][28], 0xbfd9); /* 0x2076670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][29], 0x3bc2d); /* 0x2076674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][30], 0x15ba5); /* 0x2076678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][31], 0x2cb89); /* 0x207667c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][32], 0x141ac); /* 0x2076680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][33], 0x29535); /* 0x2076684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][34], 0x34dab); /* 0x2076688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][35], 0x2b80b); /* 0x207668c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][36], 0x385cd); /* 0x2076690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][37], 0x1e91a); /* 0x2076694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][38], 0x1aa8); /* 0x2076698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][39], 0x28f03); /* 0x207669c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][40], 0x3d108); /* 0x20766a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][41], 0x2cc1c); /* 0x20766a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][42], 0x3a85); /* 0x20766a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][43], 0x10365); /* 0x20766ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][44], 0xb48c); /* 0x20766b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][45], 0x1c12d); /* 0x20766b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][46], 0xde0b); /* 0x20766b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][47], 0x2e53); /* 0x20766bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][48], 0x36e05); /* 0x20766c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][49], 0x17120); /* 0x20766c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][50], 0x340c1); /* 0x20766c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][51], 0x1c968); /* 0x20766cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][0], 0x4597); /* 0x2076700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][1], 0x2c424); /* 0x2076704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][2], 0x1b270); /* 0x2076708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][3], 0x1a80); /* 0x207670c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][4], 0x333c8); /* 0x2076710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][5], 0x124c3); /* 0x2076714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][6], 0x1ad63); /* 0x2076718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][7], 0x36dc1); /* 0x207671c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][8], 0x3e4a1); /* 0x2076720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][9], 0x6965); /* 0x2076724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][10], 0x36133); /* 0x2076728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][11], 0x24380); /* 0x207672c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][12], 0xc706); /* 0x2076730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][13], 0x17a64); /* 0x2076734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][14], 0x3da08); /* 0x2076738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][15], 0x17acf); /* 0x207673c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][16], 0xf1cd); /* 0x2076740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][17], 0x35a32); /* 0x2076744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][18], 0x1d803); /* 0x2076748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][19], 0x4e23); /* 0x207674c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][20], 0xa0ba); /* 0x2076750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][21], 0xd719); /* 0x2076754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][22], 0xea2d); /* 0x2076758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][23], 0x262f6); /* 0x207675c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][24], 0x1ba74); /* 0x2076760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][25], 0xee4e); /* 0x2076764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][26], 0x347d1); /* 0x2076768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][27], 0x33367); /* 0x207676c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][28], 0x23558); /* 0x2076770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][29], 0x26d22); /* 0x2076774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][30], 0x390e0); /* 0x2076778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][31], 0x17a6b); /* 0x207677c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][32], 0x13d1); /* 0x2076780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][33], 0x3d407); /* 0x2076784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][34], 0x355c3); /* 0x2076788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][35], 0x157a0); /* 0x207678c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][36], 0x19f70); /* 0x2076790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][37], 0x3097d); /* 0x2076794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][38], 0x9dec); /* 0x2076798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][39], 0x3cb5); /* 0x207679c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][40], 0x2dd); /* 0x20767a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][41], 0x25cac); /* 0x20767a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][42], 0x1f7ea); /* 0x20767a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][43], 0x11621); /* 0x20767ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][44], 0x3cdc1); /* 0x20767b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][45], 0xe9c2); /* 0x20767b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][46], 0x38afb); /* 0x20767b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][47], 0x28e78); /* 0x20767bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][48], 0x2c285); /* 0x20767c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][49], 0x27e64); /* 0x20767c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][50], 0x39446); /* 0x20767c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][51], 0x2d56f); /* 0x20767cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][0], 0x149dd); /* 0x2076800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][1], 0x610e); /* 0x2076804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][2], 0x908d); /* 0x2076808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][3], 0x3dfab); /* 0x207680c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][4], 0x11527); /* 0x2076810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][5], 0x34835); /* 0x2076814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][6], 0x3b41a); /* 0x2076818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][7], 0x2a786); /* 0x207681c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][8], 0x308c2); /* 0x2076820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][9], 0x3a065); /* 0x2076824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][10], 0xd0ee); /* 0x2076828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][11], 0x431c); /* 0x207682c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][12], 0x1ecc0); /* 0x2076830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][13], 0x26720); /* 0x2076834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][14], 0x5a6e); /* 0x2076838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][15], 0xd39a); /* 0x207683c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][16], 0x6b89); /* 0x2076840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][17], 0x327c3); /* 0x2076844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][18], 0x240d2); /* 0x2076848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][19], 0x134be); /* 0x207684c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][20], 0x1ef18); /* 0x2076850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][21], 0x1ac4a); /* 0x2076854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][22], 0x2debd); /* 0x2076858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][23], 0x37ed1); /* 0x207685c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][24], 0x4c41); /* 0x2076860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][25], 0x15dbc); /* 0x2076864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][26], 0x2d5d8); /* 0x2076868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][27], 0x35233); /* 0x207686c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][28], 0x3db8b); /* 0x2076870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][29], 0x8c40); /* 0x2076874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][30], 0x10056); /* 0x2076878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][31], 0x2e8c1); /* 0x207687c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][32], 0x348ea); /* 0x2076880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][33], 0x362a0); /* 0x2076884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][34], 0xa91e); /* 0x2076888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][35], 0x4041); /* 0x207688c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][36], 0xbb39); /* 0x2076890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][37], 0x3dfac); /* 0x2076894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][38], 0x253c7); /* 0x2076898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][39], 0x28c44); /* 0x207689c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][40], 0x1accc); /* 0x20768a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][41], 0x31940); /* 0x20768a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][42], 0x1bdee); /* 0x20768a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][43], 0x30017); /* 0x20768ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][44], 0x23ce2); /* 0x20768b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][45], 0x2d449); /* 0x20768b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][46], 0x2c580); /* 0x20768b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][47], 0x4fe1); /* 0x20768bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][48], 0x37bcd); /* 0x20768c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][49], 0x2aefc); /* 0x20768c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][50], 0x1b7a8); /* 0x20768c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][51], 0x3b257); /* 0x20768cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][0], 0x34af6); /* 0x2076900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][1], 0x1d40b); /* 0x2076904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][2], 0x36b18); /* 0x2076908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][3], 0x17b63); /* 0x207690c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][4], 0xf596); /* 0x2076910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][5], 0x253e7); /* 0x2076914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][6], 0x1ef35); /* 0x2076918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][7], 0x11fb4); /* 0x207691c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][8], 0x2a736); /* 0x2076920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][9], 0x4216); /* 0x2076924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][10], 0x11d59); /* 0x2076928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][11], 0x2281e); /* 0x207692c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][12], 0x3b86); /* 0x2076930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][13], 0x17b3b); /* 0x2076934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][14], 0x28582); /* 0x2076938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][15], 0x4e8a); /* 0x207693c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][16], 0x3f526); /* 0x2076940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][17], 0x2a720); /* 0x2076944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][18], 0x2c4ff); /* 0x2076948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][19], 0x2ad8d); /* 0x207694c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][20], 0x30892); /* 0x2076950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][21], 0x28134); /* 0x2076954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][22], 0xed13); /* 0x2076958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][23], 0x14482); /* 0x207695c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][24], 0x24868); /* 0x2076960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][25], 0x28524); /* 0x2076964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][26], 0x1fe28); /* 0x2076968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][27], 0x16953); /* 0x207696c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][28], 0x2347a); /* 0x2076970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][29], 0x271b1); /* 0x2076974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][30], 0x1f59e); /* 0x2076978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][31], 0x3d68f); /* 0x207697c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][32], 0x2e01e); /* 0x2076980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][33], 0xfb6e); /* 0x2076984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][34], 0x26c5f); /* 0x2076988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][35], 0x35e47); /* 0x207698c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][36], 0x19c76); /* 0x2076990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][37], 0x33791); /* 0x2076994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][38], 0xb0bb); /* 0x2076998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][39], 0x3f2aa); /* 0x207699c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][40], 0x3f21c); /* 0x20769a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][41], 0x2786e); /* 0x20769a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][42], 0x33b12); /* 0x20769a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][43], 0x114d7); /* 0x20769ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][44], 0x2a2e0); /* 0x20769b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][45], 0x23ab); /* 0x20769b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][46], 0x2b16); /* 0x20769b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][47], 0x31d35); /* 0x20769bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][48], 0x3974d); /* 0x20769c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][49], 0x3cdee); /* 0x20769c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][50], 0x343ca); /* 0x20769c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][51], 0x3da50); /* 0x20769cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][0], 0x3c34a); /* 0x2076a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][1], 0x2912c); /* 0x2076a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][2], 0x8641); /* 0x2076a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][3], 0x2c87d); /* 0x2076a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][4], 0xade1); /* 0x2076a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][5], 0x2459d); /* 0x2076a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][6], 0x3cf79); /* 0x2076a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][7], 0x2f29); /* 0x2076a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][8], 0x638c); /* 0x2076a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][9], 0x30f3d); /* 0x2076a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][10], 0x26678); /* 0x2076a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][11], 0x14d3e); /* 0x2076a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][12], 0x1d0c4); /* 0x2076a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][13], 0x2b10d); /* 0x2076a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][14], 0x216f); /* 0x2076a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][15], 0x3dd4f); /* 0x2076a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][16], 0x3e63d); /* 0x2076a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][17], 0x395eb); /* 0x2076a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][18], 0x72cd); /* 0x2076a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][19], 0x9a14); /* 0x2076a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][20], 0x2f3f); /* 0x2076a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][21], 0x39a53); /* 0x2076a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][22], 0x84e5); /* 0x2076a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][23], 0x626); /* 0x2076a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][24], 0x162ee); /* 0x2076a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][25], 0x106d1); /* 0x2076a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][26], 0x16141); /* 0x2076a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][27], 0x229f2); /* 0x2076a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][28], 0x3babe); /* 0x2076a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][29], 0x19bed); /* 0x2076a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][30], 0x1d0aa); /* 0x2076a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][31], 0x18ba3); /* 0x2076a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][32], 0x2c657); /* 0x2076a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][33], 0x1b58b); /* 0x2076a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][34], 0x1a590); /* 0x2076a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][35], 0x12df); /* 0x2076a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][36], 0x38522); /* 0x2076a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][37], 0x367c4); /* 0x2076a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][38], 0x38a95); /* 0x2076a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][39], 0xecd8); /* 0x2076a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][40], 0x15f84); /* 0x2076aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][41], 0x3009b); /* 0x2076aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][42], 0x2f7f8); /* 0x2076aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][43], 0xfbaa); /* 0x2076aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][44], 0x39e5d); /* 0x2076ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][45], 0x229c8); /* 0x2076ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][46], 0x27af4); /* 0x2076ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][47], 0x28ce); /* 0x2076abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][48], 0x62ec); /* 0x2076ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][49], 0x237bd); /* 0x2076ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][50], 0x2f678); /* 0x2076ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][51], 0x2483c); /* 0x2076acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][0], 0x3609c); /* 0x2076b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][1], 0x37a75); /* 0x2076b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][2], 0x2f13f); /* 0x2076b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][3], 0x2c49a); /* 0x2076b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][4], 0x388e9); /* 0x2076b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][5], 0x254cf); /* 0x2076b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][6], 0x1bb6a); /* 0x2076b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][7], 0x2d78b); /* 0x2076b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][8], 0x3e557); /* 0x2076b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][9], 0x2c960); /* 0x2076b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][10], 0x30dfa); /* 0x2076b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][11], 0x22ec); /* 0x2076b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][12], 0x169e4); /* 0x2076b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][13], 0x7838); /* 0x2076b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][14], 0x369a5); /* 0x2076b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][15], 0x2617e); /* 0x2076b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][16], 0x115f9); /* 0x2076b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][17], 0x3535); /* 0x2076b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][18], 0x2beda); /* 0x2076b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][19], 0x20580); /* 0x2076b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][20], 0xb468); /* 0x2076b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][21], 0x23080); /* 0x2076b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][22], 0xad7b); /* 0x2076b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][23], 0x39d6b); /* 0x2076b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][24], 0x1c677); /* 0x2076b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][25], 0x12647); /* 0x2076b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][26], 0xce6f); /* 0x2076b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][27], 0x17606); /* 0x2076b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][28], 0x1aba6); /* 0x2076b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][29], 0x39b2f); /* 0x2076b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][30], 0x1ebdd); /* 0x2076b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][31], 0xc2d0); /* 0x2076b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][32], 0x132ce); /* 0x2076b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][33], 0xc0c8); /* 0x2076b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][34], 0x3efb1); /* 0x2076b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][35], 0x2ed35); /* 0x2076b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][36], 0x27765); /* 0x2076b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][37], 0x64f4); /* 0x2076b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][38], 0x2edcf); /* 0x2076b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][39], 0x1c105); /* 0x2076b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][40], 0xae1a); /* 0x2076ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][41], 0x2271f); /* 0x2076ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][42], 0x220a7); /* 0x2076ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][43], 0xab0a); /* 0x2076bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][44], 0xb975); /* 0x2076bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][45], 0x13dd6); /* 0x2076bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][46], 0x16b34); /* 0x2076bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][47], 0x32ea1); /* 0x2076bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][48], 0x256ff); /* 0x2076bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][49], 0x87f5); /* 0x2076bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][50], 0xa4d4); /* 0x2076bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][51], 0x5aa); /* 0x2076bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][0], 0x36b6e); /* 0x2076c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][1], 0x4bf); /* 0x2076c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][2], 0x3b0f0); /* 0x2076c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][3], 0x160b8); /* 0x2076c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][4], 0x38c02); /* 0x2076c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][5], 0x1f2ca); /* 0x2076c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][6], 0x2dde5); /* 0x2076c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][7], 0x3e5ee); /* 0x2076c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][8], 0x2a4e4); /* 0x2076c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][9], 0x38bb8); /* 0x2076c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][10], 0x11b77); /* 0x2076c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][11], 0x3d5a4); /* 0x2076c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][12], 0x1142); /* 0x2076c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][13], 0x3b31e); /* 0x2076c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][14], 0x31470); /* 0x2076c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][15], 0x3b408); /* 0x2076c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][16], 0x38c44); /* 0x2076c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][17], 0x2ded6); /* 0x2076c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][18], 0xf14a); /* 0x2076c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][19], 0x3804); /* 0x2076c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][20], 0x25eeb); /* 0x2076c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][21], 0x39e50); /* 0x2076c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][22], 0x19a55); /* 0x2076c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][23], 0x12ed0); /* 0x2076c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][24], 0x3abdc); /* 0x2076c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][25], 0x613e); /* 0x2076c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][26], 0x6095); /* 0x2076c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][27], 0x5594); /* 0x2076c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][28], 0x109f3); /* 0x2076c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][29], 0x15f4a); /* 0x2076c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][30], 0x3fce6); /* 0x2076c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][31], 0x1da34); /* 0x2076c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][32], 0x26661); /* 0x2076c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][33], 0x20a1b); /* 0x2076c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][34], 0x39b46); /* 0x2076c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][35], 0x12b00); /* 0x2076c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][36], 0x8a5b); /* 0x2076c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][37], 0x3d18b); /* 0x2076c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][38], 0x1cab6); /* 0x2076c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][39], 0x2af92); /* 0x2076c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][40], 0x2e024); /* 0x2076ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][41], 0x60b0); /* 0x2076ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][42], 0xb546); /* 0x2076ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][43], 0x1266d); /* 0x2076cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][44], 0x31823); /* 0x2076cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][45], 0x3ce78); /* 0x2076cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][46], 0x128df); /* 0x2076cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][47], 0x1d214); /* 0x2076cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][48], 0x5eb3); /* 0x2076cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][49], 0x1d92c); /* 0x2076cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][50], 0xef4e); /* 0x2076cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][51], 0xb3c2); /* 0x2076ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][0], 0x6929); /* 0x2076d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][1], 0x27f86); /* 0x2076d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][2], 0x24f92); /* 0x2076d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][3], 0xe703); /* 0x2076d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][4], 0x2b45b); /* 0x2076d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][5], 0x1b043); /* 0x2076d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][6], 0x26171); /* 0x2076d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][7], 0x3f22b); /* 0x2076d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][8], 0x392e1); /* 0x2076d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][9], 0x30001); /* 0x2076d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][10], 0x39b6a); /* 0x2076d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][11], 0x259f3); /* 0x2076d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][12], 0x6f23); /* 0x2076d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][13], 0x36523); /* 0x2076d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][14], 0x6a60); /* 0x2076d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][15], 0x4b79); /* 0x2076d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][16], 0xf2b6); /* 0x2076d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][17], 0x3ecd2); /* 0x2076d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][18], 0xf52c); /* 0x2076d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][19], 0x1bb7f); /* 0x2076d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][20], 0x80dc); /* 0x2076d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][21], 0x28860); /* 0x2076d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][22], 0x3224e); /* 0x2076d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][23], 0x26017); /* 0x2076d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][24], 0x10a12); /* 0x2076d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][25], 0x1b76); /* 0x2076d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][26], 0x13ebd); /* 0x2076d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][27], 0x25944); /* 0x2076d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][28], 0x1be12); /* 0x2076d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][29], 0x51dc); /* 0x2076d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][30], 0x3ba8b); /* 0x2076d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][31], 0x1bee6); /* 0x2076d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][32], 0x6a09); /* 0x2076d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][33], 0x6a13); /* 0x2076d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][34], 0x12f57); /* 0x2076d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][35], 0x2aa79); /* 0x2076d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][36], 0x3fd11); /* 0x2076d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][37], 0x165db); /* 0x2076d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][38], 0x3eef2); /* 0x2076d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][39], 0x2d5a4); /* 0x2076d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][40], 0x180f1); /* 0x2076da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][41], 0x325cb); /* 0x2076da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][42], 0x1c03d); /* 0x2076da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][43], 0x21511); /* 0x2076dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][44], 0x1d577); /* 0x2076db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][45], 0x3836c); /* 0x2076db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][46], 0x1c6e5); /* 0x2076db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][47], 0x2e83b); /* 0x2076dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][48], 0x16586); /* 0x2076dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][49], 0x39092); /* 0x2076dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][50], 0xef39); /* 0x2076dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][51], 0x19b23); /* 0x2076dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][0], 0x37012); /* 0x2076e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][1], 0xa2ea); /* 0x2076e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][2], 0x17ea9); /* 0x2076e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][3], 0xfafc); /* 0x2076e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][4], 0x3637c); /* 0x2076e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][5], 0xa00d); /* 0x2076e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][6], 0x21299); /* 0x2076e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][7], 0x16636); /* 0x2076e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][8], 0x3585); /* 0x2076e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][9], 0x293d4); /* 0x2076e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][10], 0xe384); /* 0x2076e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][11], 0x31865); /* 0x2076e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][12], 0x154ab); /* 0x2076e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][13], 0x2314d); /* 0x2076e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][14], 0x3d928); /* 0x2076e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][15], 0x302ad); /* 0x2076e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][16], 0x2b9b9); /* 0x2076e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][17], 0x3e043); /* 0x2076e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][18], 0x3499d); /* 0x2076e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][19], 0x2b5e7); /* 0x2076e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][20], 0x3b894); /* 0x2076e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][21], 0x22ecf); /* 0x2076e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][22], 0xea1d); /* 0x2076e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][23], 0x57eb); /* 0x2076e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][24], 0x15b3); /* 0x2076e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][25], 0x2ed88); /* 0x2076e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][26], 0x2b22e); /* 0x2076e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][27], 0x6d79); /* 0x2076e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][28], 0x733c); /* 0x2076e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][29], 0x31ef0); /* 0x2076e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][30], 0x18a45); /* 0x2076e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][31], 0xf801); /* 0x2076e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][32], 0x15c9f); /* 0x2076e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][33], 0x32d5c); /* 0x2076e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][34], 0x1a97a); /* 0x2076e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][35], 0x2efd1); /* 0x2076e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][36], 0xbe63); /* 0x2076e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][37], 0x31ba0); /* 0x2076e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][38], 0xdded); /* 0x2076e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][39], 0x3ce96); /* 0x2076e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][40], 0x1c756); /* 0x2076ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][41], 0x1a20c); /* 0x2076ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][42], 0x1baf); /* 0x2076ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][43], 0x1702e); /* 0x2076eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][44], 0x2c831); /* 0x2076eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][45], 0x29bdc); /* 0x2076eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][46], 0x23d0f); /* 0x2076eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][47], 0x3cb60); /* 0x2076ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][48], 0xe76e); /* 0x2076ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][49], 0x52e4); /* 0x2076ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][50], 0x35e2d); /* 0x2076ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][51], 0x2267d); /* 0x2076ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][0], 0x2af64); /* 0x2076f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][1], 0x20e85); /* 0x2076f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][2], 0x385ba); /* 0x2076f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][3], 0xdce8); /* 0x2076f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][4], 0x14a6c); /* 0x2076f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][5], 0x2c891); /* 0x2076f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][6], 0x613a); /* 0x2076f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][7], 0x2124b); /* 0x2076f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][8], 0x39180); /* 0x2076f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][9], 0x1b93b); /* 0x2076f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][10], 0xcfcb); /* 0x2076f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][11], 0x13133); /* 0x2076f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][12], 0x13978); /* 0x2076f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][13], 0x2b489); /* 0x2076f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][14], 0x7a39); /* 0x2076f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][15], 0x2a482); /* 0x2076f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][16], 0x112a1); /* 0x2076f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][17], 0x19a40); /* 0x2076f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][18], 0x1989b); /* 0x2076f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][19], 0xd20f); /* 0x2076f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][20], 0x391fb); /* 0x2076f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][21], 0x2a880); /* 0x2076f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][22], 0x1f2f2); /* 0x2076f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][23], 0xc222); /* 0x2076f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][24], 0x3b08); /* 0x2076f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][25], 0x1f4ff); /* 0x2076f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][26], 0x183fa); /* 0x2076f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][27], 0x2c601); /* 0x2076f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][28], 0x36e3e); /* 0x2076f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][29], 0x3ce42); /* 0x2076f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][30], 0xf90e); /* 0x2076f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][31], 0x1a76); /* 0x2076f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][32], 0x36454); /* 0x2076f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][33], 0x30376); /* 0x2076f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][34], 0x22855); /* 0x2076f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][35], 0x5242); /* 0x2076f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][36], 0x2a54b); /* 0x2076f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][37], 0xe82e); /* 0x2076f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][38], 0x29a23); /* 0x2076f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][39], 0xbe56); /* 0x2076f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][40], 0x2da64); /* 0x2076fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][41], 0x17685); /* 0x2076fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][42], 0x3fe5f); /* 0x2076fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][43], 0x269ce); /* 0x2076fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][44], 0x1c2de); /* 0x2076fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][45], 0xb791); /* 0x2076fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][46], 0x17ebc); /* 0x2076fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][47], 0x31df4); /* 0x2076fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][48], 0x9b40); /* 0x2076fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][49], 0x11682); /* 0x2076fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][50], 0x21fff); /* 0x2076fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][51], 0x2c50a); /* 0x2076fcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][0], 0x3e96f); /* 0x2077000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][1], 0x253ee); /* 0x2077004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][2], 0x1d6e); /* 0x2077008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][3], 0x1c660); /* 0x207700c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][4], 0x2a537); /* 0x2077010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][5], 0x3dcb9); /* 0x2077014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][6], 0x2f89e); /* 0x2077018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][7], 0x1ed48); /* 0x207701c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][8], 0x27914); /* 0x2077020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][9], 0x22f8b); /* 0x2077024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][10], 0x8674); /* 0x2077028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][11], 0x43c3); /* 0x207702c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][12], 0x1483b); /* 0x2077030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][13], 0x30ee1); /* 0x2077034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][14], 0x384ae); /* 0x2077038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][15], 0x6c1c); /* 0x207703c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][16], 0x12283); /* 0x2077040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][17], 0xc58d); /* 0x2077044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][18], 0x23994); /* 0x2077048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][19], 0x335fa); /* 0x207704c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][20], 0x2416a); /* 0x2077050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][21], 0x7bd2); /* 0x2077054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][22], 0x294f3); /* 0x2077058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][23], 0x23bf8); /* 0x207705c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][24], 0x36fb1); /* 0x2077060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][25], 0x2b099); /* 0x2077064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][26], 0x22528); /* 0x2077068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][27], 0x278a8); /* 0x207706c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][28], 0x196d2); /* 0x2077070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][29], 0x3b9cb); /* 0x2077074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][30], 0xd121); /* 0x2077078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][31], 0x3e4f8); /* 0x207707c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][32], 0x16f38); /* 0x2077080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][33], 0x680b); /* 0x2077084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][34], 0x3dd55); /* 0x2077088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][35], 0x164d7); /* 0x207708c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][36], 0x393c0); /* 0x2077090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][37], 0x18549); /* 0x2077094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][38], 0x1755f); /* 0x2077098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][39], 0xf2ea); /* 0x207709c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][40], 0x243e8); /* 0x20770a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][41], 0x27c0e); /* 0x20770a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][42], 0x8578); /* 0x20770a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][43], 0x385a7); /* 0x20770ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][44], 0x6855); /* 0x20770b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][45], 0x7459); /* 0x20770b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][46], 0x8e89); /* 0x20770b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][47], 0x26dc4); /* 0x20770bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][48], 0x3992c); /* 0x20770c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][49], 0x1e238); /* 0x20770c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][50], 0x3f595); /* 0x20770c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][51], 0x2e52d); /* 0x20770cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][0], 0x401a); /* 0x2077100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][1], 0xd791); /* 0x2077104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][2], 0xd6a8); /* 0x2077108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][3], 0x3a50a); /* 0x207710c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][4], 0x1e1f6); /* 0x2077110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][5], 0x29be1); /* 0x2077114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][6], 0xa0cf); /* 0x2077118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][7], 0x26f7); /* 0x207711c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][8], 0x3da75); /* 0x2077120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][9], 0x10b4a); /* 0x2077124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][10], 0x2f973); /* 0x2077128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][11], 0x1e545); /* 0x207712c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][12], 0x32518); /* 0x2077130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][13], 0x3dec9); /* 0x2077134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][14], 0xe079); /* 0x2077138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][15], 0xa8b2); /* 0x207713c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][16], 0x3ca35); /* 0x2077140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][17], 0x38a7f); /* 0x2077144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][18], 0x5356); /* 0x2077148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][19], 0x3605f); /* 0x207714c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][20], 0x33fe9); /* 0x2077150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][21], 0xa31e); /* 0x2077154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][22], 0x36a1d); /* 0x2077158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][23], 0x322d5); /* 0x207715c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][24], 0x1c881); /* 0x2077160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][25], 0xea41); /* 0x2077164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][26], 0x19c2b); /* 0x2077168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][27], 0x1eada); /* 0x207716c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][28], 0x20ad4); /* 0x2077170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][29], 0x23fd6); /* 0x2077174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][30], 0x167c7); /* 0x2077178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][31], 0x143ff); /* 0x207717c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][32], 0x3824d); /* 0x2077180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][33], 0x3d64d); /* 0x2077184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][34], 0xc8b3); /* 0x2077188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][35], 0x12b75); /* 0x207718c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][36], 0xcbe4); /* 0x2077190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][37], 0x18a6b); /* 0x2077194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][38], 0x1120); /* 0x2077198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][39], 0x9c31); /* 0x207719c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][40], 0x1b2ce); /* 0x20771a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][41], 0x4dfa); /* 0x20771a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][42], 0xb375); /* 0x20771a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][43], 0x151df); /* 0x20771ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][44], 0x12674); /* 0x20771b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][45], 0x2cd5a); /* 0x20771b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][46], 0x317c8); /* 0x20771b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][47], 0x8fcd); /* 0x20771bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][48], 0x1a4c8); /* 0x20771c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][49], 0x19aaa); /* 0x20771c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][50], 0x1ca12); /* 0x20771c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][51], 0x3edad); /* 0x20771cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][0], 0x2ddbb); /* 0x2077200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][1], 0x3f237); /* 0x2077204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][2], 0x2cd47); /* 0x2077208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][3], 0x37569); /* 0x207720c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][4], 0x1ab5); /* 0x2077210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][5], 0x6879); /* 0x2077214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][6], 0x1da4); /* 0x2077218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][7], 0x25d42); /* 0x207721c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][8], 0x33fa9); /* 0x2077220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][9], 0x2d4b8); /* 0x2077224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][10], 0x24887); /* 0x2077228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][11], 0x232ba); /* 0x207722c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][12], 0x4141); /* 0x2077230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][13], 0xcbc7); /* 0x2077234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][14], 0x1d11); /* 0x2077238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][15], 0x19408); /* 0x207723c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][16], 0x26eb6); /* 0x2077240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][17], 0x32a61); /* 0x2077244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][18], 0xa5f8); /* 0x2077248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][19], 0x45b5); /* 0x207724c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][20], 0x2d36e); /* 0x2077250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][21], 0x571a); /* 0x2077254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][22], 0xfb8b); /* 0x2077258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][23], 0x2f243); /* 0x207725c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][24], 0x6d20); /* 0x2077260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][25], 0x10d9b); /* 0x2077264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][26], 0x18555); /* 0x2077268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][27], 0xe766); /* 0x207726c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][28], 0x1b389); /* 0x2077270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][29], 0x909b); /* 0x2077274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][30], 0xebe8); /* 0x2077278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][31], 0x2c789); /* 0x207727c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][32], 0x3186c); /* 0x2077280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][33], 0xc7d5); /* 0x2077284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][34], 0x1ea6); /* 0x2077288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][35], 0xdc4); /* 0x207728c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][36], 0x34df7); /* 0x2077290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][37], 0x10acc); /* 0x2077294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][38], 0x16347); /* 0x2077298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][39], 0x2ce85); /* 0x207729c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][40], 0x19b95); /* 0x20772a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][41], 0x211b8); /* 0x20772a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][42], 0x148ea); /* 0x20772a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][43], 0x34748); /* 0x20772ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][44], 0x138c0); /* 0x20772b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][45], 0x27c94); /* 0x20772b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][46], 0x14f56); /* 0x20772b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][47], 0x6f01); /* 0x20772bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][48], 0x10307); /* 0x20772c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][49], 0x2bbdb); /* 0x20772c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][50], 0x3d000); /* 0x20772c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][51], 0x2a6ea); /* 0x20772cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][0], 0xd43f); /* 0x2077300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][1], 0x393f); /* 0x2077304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][2], 0x328af); /* 0x2077308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][3], 0x3ace1); /* 0x207730c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][4], 0x12556); /* 0x2077310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][5], 0x26bc4); /* 0x2077314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][6], 0xa701); /* 0x2077318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][7], 0x1ce26); /* 0x207731c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][8], 0x1b2b2); /* 0x2077320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][9], 0x17e78); /* 0x2077324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][10], 0x2429d); /* 0x2077328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][11], 0x3490d); /* 0x207732c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][12], 0x30e24); /* 0x2077330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][13], 0x10ac6); /* 0x2077334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][14], 0x111f1); /* 0x2077338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][15], 0x1c1c6); /* 0x207733c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][16], 0x21f5c); /* 0x2077340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][17], 0x23592); /* 0x2077344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][18], 0x3065e); /* 0x2077348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][19], 0x17f31); /* 0x207734c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][20], 0x3f4c1); /* 0x2077350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][21], 0x33b6b); /* 0x2077354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][22], 0x22cc); /* 0x2077358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][23], 0x29330); /* 0x207735c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][24], 0xd615); /* 0x2077360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][25], 0xea55); /* 0x2077364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][26], 0x385e3); /* 0x2077368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][27], 0x3e120); /* 0x207736c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][28], 0x2b19); /* 0x2077370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][29], 0x3094c); /* 0x2077374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][30], 0xe2c5); /* 0x2077378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][31], 0x34a5e); /* 0x207737c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][32], 0xbfa9); /* 0x2077380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][33], 0x2b03d); /* 0x2077384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][34], 0x3f527); /* 0x2077388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][35], 0x12576); /* 0x207738c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][36], 0x17c1a); /* 0x2077390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][37], 0x83b3); /* 0x2077394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][38], 0x3b498); /* 0x2077398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][39], 0x23e3c); /* 0x207739c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][40], 0x2b579); /* 0x20773a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][41], 0x3b3b1); /* 0x20773a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][42], 0x1dd5); /* 0x20773a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][43], 0x38e93); /* 0x20773ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][44], 0xc745); /* 0x20773b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][45], 0x280d0); /* 0x20773b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][46], 0xfdd8); /* 0x20773b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][47], 0x2098c); /* 0x20773bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][48], 0x5345); /* 0x20773c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][49], 0x3afe9); /* 0x20773c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][50], 0x81be); /* 0x20773c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][51], 0x2eea); /* 0x20773cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][0], 0x9ae1); /* 0x2077400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][1], 0x12774); /* 0x2077404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][2], 0x1afdc); /* 0x2077408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][3], 0x26f6b); /* 0x207740c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][4], 0x55e7); /* 0x2077410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][5], 0x31754); /* 0x2077414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][6], 0x1f439); /* 0x2077418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][7], 0x793b); /* 0x207741c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][8], 0x18719); /* 0x2077420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][9], 0x302ae); /* 0x2077424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][10], 0x8ed4); /* 0x2077428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][11], 0x503c); /* 0x207742c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][12], 0x229a0); /* 0x2077430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][13], 0xc2d4); /* 0x2077434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][14], 0x43a9); /* 0x2077438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][15], 0x3c2b0); /* 0x207743c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][16], 0x24c0); /* 0x2077440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][17], 0x3a6a4); /* 0x2077444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][18], 0x9d17); /* 0x2077448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][19], 0xde49); /* 0x207744c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][20], 0x2fef); /* 0x2077450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][21], 0x4c17); /* 0x2077454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][22], 0xf2ac); /* 0x2077458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][23], 0x4527); /* 0x207745c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][24], 0xade7); /* 0x2077460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][25], 0x351d3); /* 0x2077464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][26], 0x43d); /* 0x2077468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][27], 0x245bc); /* 0x207746c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][28], 0x134ba); /* 0x2077470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][29], 0x22f37); /* 0x2077474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][30], 0x10b50); /* 0x2077478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][31], 0x3fbb2); /* 0x207747c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][32], 0x360cb); /* 0x2077480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][33], 0x350ff); /* 0x2077484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][34], 0x3744e); /* 0x2077488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][35], 0xfd21); /* 0x207748c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][36], 0x21570); /* 0x2077490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][37], 0x2516d); /* 0x2077494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][38], 0xc4a7); /* 0x2077498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][39], 0x398a9); /* 0x207749c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][40], 0x30e95); /* 0x20774a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][41], 0x32a5e); /* 0x20774a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][42], 0x844d); /* 0x20774a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][43], 0xdf19); /* 0x20774ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][44], 0xa46d); /* 0x20774b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][45], 0x2ba37); /* 0x20774b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][46], 0x1d584); /* 0x20774b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][47], 0x30a46); /* 0x20774bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][48], 0x2e508); /* 0x20774c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][49], 0x3a805); /* 0x20774c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][50], 0xce23); /* 0x20774c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][51], 0x1ecc7); /* 0x20774cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][0], 0x3f0bf); /* 0x2077500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][1], 0x3124c); /* 0x2077504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][2], 0xf6cf); /* 0x2077508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][3], 0x10b5); /* 0x207750c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][4], 0x5855); /* 0x2077510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][5], 0x2f0c1); /* 0x2077514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][6], 0x32684); /* 0x2077518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][7], 0x13596); /* 0x207751c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][8], 0x2b5fc); /* 0x2077520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][9], 0x279); /* 0x2077524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][10], 0x2084c); /* 0x2077528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][11], 0x144ee); /* 0x207752c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][12], 0x3d2e4); /* 0x2077530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][13], 0x1c901); /* 0x2077534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][14], 0xb256); /* 0x2077538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][15], 0xb7ae); /* 0x207753c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][16], 0xa752); /* 0x2077540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][17], 0x29bc4); /* 0x2077544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][18], 0x411d); /* 0x2077548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][19], 0x36b4f); /* 0x207754c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][20], 0x3b0b5); /* 0x2077550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][21], 0x29742); /* 0x2077554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][22], 0x8704); /* 0x2077558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][23], 0x1df1e); /* 0x207755c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][24], 0x23bbe); /* 0x2077560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][25], 0x29040); /* 0x2077564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][26], 0x22a7c); /* 0x2077568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][27], 0x44cb); /* 0x207756c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][28], 0x1d33f); /* 0x2077570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][29], 0x1bc79); /* 0x2077574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][30], 0x26fe8); /* 0x2077578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][31], 0x1bb9f); /* 0x207757c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][32], 0x1daef); /* 0x2077580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][33], 0x2258); /* 0x2077584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][34], 0x3f1f5); /* 0x2077588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][35], 0x1a459); /* 0x207758c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][36], 0x2ac1b); /* 0x2077590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][37], 0x20586); /* 0x2077594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][38], 0x151de); /* 0x2077598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][39], 0x1a41d); /* 0x207759c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][40], 0x17e64); /* 0x20775a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][41], 0x25828); /* 0x20775a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][42], 0x1d987); /* 0x20775a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][43], 0x2f584); /* 0x20775ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][44], 0x2cf49); /* 0x20775b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][45], 0x33919); /* 0x20775b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][46], 0x37329); /* 0x20775b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][47], 0x22549); /* 0x20775bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][48], 0x2d266); /* 0x20775c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][49], 0x11fbb); /* 0x20775c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][50], 0x11789); /* 0x20775c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][51], 0x2a4a); /* 0x20775cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][0], 0x1e837); /* 0x2077600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][1], 0x27fa8); /* 0x2077604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][2], 0x3e85c); /* 0x2077608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][3], 0x30abb); /* 0x207760c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][4], 0x70ff); /* 0x2077610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][5], 0x13891); /* 0x2077614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][6], 0x1bf57); /* 0x2077618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][7], 0x2b755); /* 0x207761c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][8], 0x3b362); /* 0x2077620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][9], 0x3ee98); /* 0x2077624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][10], 0x993e); /* 0x2077628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][11], 0x291cc); /* 0x207762c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][12], 0x3c270); /* 0x2077630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][13], 0x11549); /* 0x2077634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][14], 0x1c1cb); /* 0x2077638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][15], 0x799c); /* 0x207763c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][16], 0x2866d); /* 0x2077640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][17], 0xc395); /* 0x2077644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][18], 0x8eb6); /* 0x2077648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][19], 0x35c34); /* 0x207764c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][20], 0x2441f); /* 0x2077650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][21], 0x81aa); /* 0x2077654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][22], 0x3eb9f); /* 0x2077658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][23], 0x3bc3b); /* 0x207765c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][24], 0x34f08); /* 0x2077660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][25], 0x2fd71); /* 0x2077664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][26], 0x2d97f); /* 0x2077668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][27], 0x3887e); /* 0x207766c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][28], 0x59a9); /* 0x2077670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][29], 0x3917e); /* 0x2077674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][30], 0x3be98); /* 0x2077678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][31], 0x39d47); /* 0x207767c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][32], 0x3c03); /* 0x2077680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][33], 0x3c7b); /* 0x2077684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][34], 0x3c4e9); /* 0x2077688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][35], 0x2f771); /* 0x207768c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][36], 0x11028); /* 0x2077690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][37], 0x31eb7); /* 0x2077694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][38], 0x3b5c7); /* 0x2077698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][39], 0x38b24); /* 0x207769c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][40], 0x381c5); /* 0x20776a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][41], 0x2837f); /* 0x20776a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][42], 0x9b56); /* 0x20776a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][43], 0x1a6cd); /* 0x20776ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][44], 0x21c4b); /* 0x20776b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][45], 0x242f8); /* 0x20776b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][46], 0x1c150); /* 0x20776b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][47], 0x1cca7); /* 0x20776bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][48], 0x393e1); /* 0x20776c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][49], 0x36b4a); /* 0x20776c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][50], 0x124a2); /* 0x20776c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][51], 0xc0b8); /* 0x20776cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][0], 0x42c9); /* 0x2077700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][1], 0x33fdd); /* 0x2077704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][2], 0x8625); /* 0x2077708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][3], 0x1c8bb); /* 0x207770c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][4], 0x4ab5); /* 0x2077710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][5], 0x279a4); /* 0x2077714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][6], 0x20316); /* 0x2077718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][7], 0x344d4); /* 0x207771c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][8], 0x3f666); /* 0x2077720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][9], 0x1ce10); /* 0x2077724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][10], 0x37c8); /* 0x2077728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][11], 0x3f26); /* 0x207772c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][12], 0x3f5b7); /* 0x2077730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][13], 0x7c43); /* 0x2077734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][14], 0x104dc); /* 0x2077738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][15], 0x3c9aa); /* 0x207773c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][16], 0x183fb); /* 0x2077740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][17], 0x16555); /* 0x2077744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][18], 0x2b955); /* 0x2077748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][19], 0x32643); /* 0x207774c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][20], 0xe4fe); /* 0x2077750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][21], 0x93e4); /* 0x2077754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][22], 0x67de); /* 0x2077758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][23], 0x3e98e); /* 0x207775c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][24], 0x1722); /* 0x2077760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][25], 0x13614); /* 0x2077764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][26], 0x1940f); /* 0x2077768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][27], 0x1dcb9); /* 0x207776c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][28], 0x13473); /* 0x2077770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][29], 0x1b362); /* 0x2077774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][30], 0x19299); /* 0x2077778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][31], 0x21841); /* 0x207777c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][32], 0x2c787); /* 0x2077780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][33], 0x1e193); /* 0x2077784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][34], 0x118b2); /* 0x2077788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][35], 0x28cae); /* 0x207778c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][36], 0x34a2c); /* 0x2077790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][37], 0x1454c); /* 0x2077794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][38], 0x3c027); /* 0x2077798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][39], 0x226d1); /* 0x207779c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][40], 0x12f18); /* 0x20777a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][41], 0x390d0); /* 0x20777a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][42], 0x19b7b); /* 0x20777a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][43], 0x36378); /* 0x20777ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][44], 0x7c4b); /* 0x20777b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][45], 0x9a72); /* 0x20777b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][46], 0xbfc8); /* 0x20777b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][47], 0x3f0b3); /* 0x20777bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][48], 0x20864); /* 0x20777c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][49], 0x2580b); /* 0x20777c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][50], 0x22ce4); /* 0x20777c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][51], 0x13539); /* 0x20777cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][0], 0x3c4d8); /* 0x2077800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][1], 0x1150d); /* 0x2077804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][2], 0x28df8); /* 0x2077808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][3], 0x2a59b); /* 0x207780c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][4], 0x20a48); /* 0x2077810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][5], 0x34b79); /* 0x2077814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][6], 0x2bf19); /* 0x2077818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][7], 0x281ca); /* 0x207781c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][8], 0x2b75); /* 0x2077820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][9], 0x5362); /* 0x2077824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][10], 0x24aec); /* 0x2077828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][11], 0x945e); /* 0x207782c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][12], 0x3ea9b); /* 0x2077830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][13], 0x3d1f2); /* 0x2077834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][14], 0x3f611); /* 0x2077838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][15], 0x3a754); /* 0x207783c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][16], 0xf90e); /* 0x2077840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][17], 0x5c00); /* 0x2077844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][18], 0xb1e6); /* 0x2077848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][19], 0xa2fb); /* 0x207784c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][20], 0xc2b9); /* 0x2077850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][21], 0x3755f); /* 0x2077854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][22], 0xf321); /* 0x2077858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][23], 0xeb16); /* 0x207785c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][24], 0x2a48f); /* 0x2077860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][25], 0xa0da); /* 0x2077864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][26], 0x308b6); /* 0x2077868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][27], 0x17a1d); /* 0x207786c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][28], 0x377); /* 0x2077870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][29], 0x1281f); /* 0x2077874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][30], 0x160be); /* 0x2077878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][31], 0x13c7d); /* 0x207787c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][32], 0x15204); /* 0x2077880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][33], 0x3c88f); /* 0x2077884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][34], 0x39ee0); /* 0x2077888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][35], 0x399e9); /* 0x207788c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][36], 0x196e0); /* 0x2077890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][37], 0x392ee); /* 0x2077894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][38], 0x737); /* 0x2077898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][39], 0x34d5b); /* 0x207789c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][40], 0xb9c1); /* 0x20778a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][41], 0x2f0f6); /* 0x20778a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][42], 0x20e55); /* 0x20778a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][43], 0x251e4); /* 0x20778ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][44], 0x1e943); /* 0x20778b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][45], 0x3424d); /* 0x20778b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][46], 0x112a3); /* 0x20778b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][47], 0x2371c); /* 0x20778bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][48], 0x19c4e); /* 0x20778c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][49], 0x9271); /* 0x20778c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][50], 0x14631); /* 0x20778c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][51], 0x1c207); /* 0x20778cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][0], 0x1f904); /* 0x2077900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][1], 0x2196b); /* 0x2077904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][2], 0x814); /* 0x2077908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][3], 0x5b75); /* 0x207790c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][4], 0xe264); /* 0x2077910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][5], 0x201d4); /* 0x2077914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][6], 0x3e5); /* 0x2077918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][7], 0x111d7); /* 0x207791c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][8], 0x14d9); /* 0x2077920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][9], 0x366e6); /* 0x2077924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][10], 0x1acf); /* 0x2077928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][11], 0x3b34c); /* 0x207792c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][12], 0x17e6b); /* 0x2077930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][13], 0xcf71); /* 0x2077934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][14], 0x38981); /* 0x2077938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][15], 0x292a7); /* 0x207793c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][16], 0x382ba); /* 0x2077940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][17], 0x1e735); /* 0x2077944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][18], 0x2c71a); /* 0x2077948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][19], 0x6332); /* 0x207794c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][20], 0x3555f); /* 0x2077950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][21], 0x3c123); /* 0x2077954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][22], 0x1f354); /* 0x2077958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][23], 0x38ff5); /* 0x207795c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][24], 0x1ee22); /* 0x2077960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][25], 0x18f65); /* 0x2077964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][26], 0x3b5f4); /* 0x2077968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][27], 0x33874); /* 0x207796c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][28], 0x39160); /* 0x2077970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][29], 0x2bb45); /* 0x2077974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][30], 0x1e46b); /* 0x2077978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][31], 0x15146); /* 0x207797c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][32], 0x308c5); /* 0x2077980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][33], 0x2f631); /* 0x2077984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][34], 0x2c357); /* 0x2077988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][35], 0x3f0c8); /* 0x207798c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][36], 0x25ddb); /* 0x2077990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][37], 0x91e2); /* 0x2077994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][38], 0x37d49); /* 0x2077998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][39], 0x14728); /* 0x207799c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][40], 0x5286); /* 0x20779a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][41], 0x5e12); /* 0x20779a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][42], 0x2112d); /* 0x20779a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][43], 0x36499); /* 0x20779ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][44], 0x2e397); /* 0x20779b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][45], 0x349a4); /* 0x20779b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][46], 0x3bac1); /* 0x20779b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][47], 0x160fa); /* 0x20779bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][48], 0x8704); /* 0x20779c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][49], 0x314a0); /* 0x20779c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][50], 0x273dc); /* 0x20779c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][51], 0x215b7); /* 0x20779cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][0], 0x3ca02); /* 0x2077a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][1], 0x2fc95); /* 0x2077a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][2], 0xdaa3); /* 0x2077a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][3], 0x1ba4c); /* 0x2077a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][4], 0x62c2); /* 0x2077a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][5], 0x3b9e1); /* 0x2077a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][6], 0x37dc2); /* 0x2077a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][7], 0x39115); /* 0x2077a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][8], 0x3ad4c); /* 0x2077a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][9], 0xa4ba); /* 0x2077a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][10], 0x39e08); /* 0x2077a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][11], 0x3be83); /* 0x2077a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][12], 0x22c42); /* 0x2077a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][13], 0x3c2bf); /* 0x2077a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][14], 0x3baf8); /* 0x2077a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][15], 0x2752e); /* 0x2077a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][16], 0x2073); /* 0x2077a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][17], 0x37b92); /* 0x2077a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][18], 0x2ce06); /* 0x2077a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][19], 0x26ab); /* 0x2077a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][20], 0x13524); /* 0x2077a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][21], 0xdef1); /* 0x2077a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][22], 0x1245b); /* 0x2077a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][23], 0x1f67a); /* 0x2077a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][24], 0x135e2); /* 0x2077a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][25], 0x31556); /* 0x2077a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][26], 0x15b3e); /* 0x2077a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][27], 0x38289); /* 0x2077a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][28], 0x15a0d); /* 0x2077a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][29], 0x2ad9a); /* 0x2077a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][30], 0x38b7c); /* 0x2077a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][31], 0x13a5b); /* 0x2077a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][32], 0xb700); /* 0x2077a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][33], 0x26739); /* 0x2077a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][34], 0x29382); /* 0x2077a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][35], 0xadf6); /* 0x2077a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][36], 0x1a97d); /* 0x2077a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][37], 0xf3ad); /* 0x2077a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][38], 0x2ebe3); /* 0x2077a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][39], 0x244ad); /* 0x2077a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][40], 0x25057); /* 0x2077aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][41], 0x22b99); /* 0x2077aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][42], 0x23eba); /* 0x2077aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][43], 0x1a2b0); /* 0x2077aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][44], 0x15ee3); /* 0x2077ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][45], 0x1a9d6); /* 0x2077ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][46], 0x23147); /* 0x2077ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][47], 0x3083b); /* 0x2077abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][48], 0x1e8fe); /* 0x2077ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][49], 0x132d4); /* 0x2077ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][50], 0x35a6a); /* 0x2077ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][51], 0x2ed8); /* 0x2077acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][0], 0x1d91b); /* 0x2077b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][1], 0x3dab5); /* 0x2077b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][2], 0x20246); /* 0x2077b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][3], 0x24c22); /* 0x2077b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][4], 0x335f4); /* 0x2077b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][5], 0x5150); /* 0x2077b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][6], 0x3cfaa); /* 0x2077b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][7], 0x3245a); /* 0x2077b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][8], 0x296b4); /* 0x2077b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][9], 0x15d53); /* 0x2077b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][10], 0x3892); /* 0x2077b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][11], 0x12090); /* 0x2077b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][12], 0x15abc); /* 0x2077b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][13], 0x36c82); /* 0x2077b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][14], 0x1c260); /* 0x2077b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][15], 0xc663); /* 0x2077b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][16], 0x26fae); /* 0x2077b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][17], 0x7dc9); /* 0x2077b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][18], 0xc21a); /* 0x2077b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][19], 0x98e1); /* 0x2077b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][20], 0x398f6); /* 0x2077b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][21], 0x10f9d); /* 0x2077b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][22], 0x2e42); /* 0x2077b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][23], 0x30b98); /* 0x2077b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][24], 0x3eff9); /* 0x2077b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][25], 0xfdfd); /* 0x2077b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][26], 0x39443); /* 0x2077b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][27], 0x2b9d6); /* 0x2077b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][28], 0x1d6e9); /* 0x2077b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][29], 0x8485); /* 0x2077b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][30], 0x1ad3c); /* 0x2077b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][31], 0x2a725); /* 0x2077b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][32], 0x54b1); /* 0x2077b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][33], 0x2486f); /* 0x2077b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][34], 0x17aea); /* 0x2077b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][35], 0x12796); /* 0x2077b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][36], 0x30d63); /* 0x2077b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][37], 0x318f0); /* 0x2077b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][38], 0xcd49); /* 0x2077b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][39], 0x1c852); /* 0x2077b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][40], 0x5ab0); /* 0x2077ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][41], 0x1d344); /* 0x2077ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][42], 0x1341a); /* 0x2077ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][43], 0x1051c); /* 0x2077bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][44], 0x3f38d); /* 0x2077bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][45], 0x864b); /* 0x2077bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][46], 0x1172a); /* 0x2077bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][47], 0xc67c); /* 0x2077bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][48], 0x3766a); /* 0x2077bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][49], 0x4693); /* 0x2077bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][50], 0x29fe0); /* 0x2077bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][51], 0x20e91); /* 0x2077bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][0], 0x97be); /* 0x2077c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][1], 0x18f62); /* 0x2077c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][2], 0x70e5); /* 0x2077c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][3], 0x22b8f); /* 0x2077c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][4], 0x29a0a); /* 0x2077c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][5], 0x11889); /* 0x2077c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][6], 0x2b2cd); /* 0x2077c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][7], 0x2e272); /* 0x2077c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][8], 0x335e8); /* 0x2077c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][9], 0x3d837); /* 0x2077c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][10], 0xae76); /* 0x2077c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][11], 0x19302); /* 0x2077c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][12], 0xe82c); /* 0x2077c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][13], 0x38e56); /* 0x2077c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][14], 0x3cd84); /* 0x2077c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][15], 0x505c); /* 0x2077c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][16], 0xcd34); /* 0x2077c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][17], 0xfc13); /* 0x2077c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][18], 0x1a47f); /* 0x2077c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][19], 0x15f0e); /* 0x2077c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][20], 0x9d4f); /* 0x2077c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][21], 0x33a02); /* 0x2077c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][22], 0x17faa); /* 0x2077c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][23], 0x174aa); /* 0x2077c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][24], 0x306b1); /* 0x2077c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][25], 0x3b8d8); /* 0x2077c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][26], 0x3260d); /* 0x2077c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][27], 0x1aa); /* 0x2077c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][28], 0x29e03); /* 0x2077c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][29], 0x3e039); /* 0x2077c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][30], 0x7fd4); /* 0x2077c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][31], 0x16572); /* 0x2077c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][32], 0x23ab); /* 0x2077c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][33], 0x9305); /* 0x2077c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][34], 0x3bd3d); /* 0x2077c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][35], 0xae8e); /* 0x2077c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][36], 0x20556); /* 0x2077c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][37], 0x37b38); /* 0x2077c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][38], 0xed81); /* 0x2077c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][39], 0x22ded); /* 0x2077c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][40], 0x1215); /* 0x2077ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][41], 0x1b209); /* 0x2077ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][42], 0x1f515); /* 0x2077ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][43], 0x1b06b); /* 0x2077cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][44], 0x8357); /* 0x2077cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][45], 0x12342); /* 0x2077cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][46], 0x28af0); /* 0x2077cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][47], 0x3a154); /* 0x2077cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][48], 0x109a9); /* 0x2077cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][49], 0x2d895); /* 0x2077cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][50], 0x256d5); /* 0x2077cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][51], 0x37ea5); /* 0x2077ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][0], 0x3c06e); /* 0x2077d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][1], 0x37a4a); /* 0x2077d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][2], 0x1879a); /* 0x2077d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][3], 0x19e64); /* 0x2077d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][4], 0x9d03); /* 0x2077d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][5], 0x25413); /* 0x2077d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][6], 0x3842d); /* 0x2077d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][7], 0x2f37a); /* 0x2077d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][8], 0x1443e); /* 0x2077d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][9], 0x3143b); /* 0x2077d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][10], 0x31541); /* 0x2077d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][11], 0x2ae24); /* 0x2077d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][12], 0x227a2); /* 0x2077d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][13], 0x344a6); /* 0x2077d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][14], 0x4dac); /* 0x2077d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][15], 0x19fd8); /* 0x2077d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][16], 0x22b15); /* 0x2077d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][17], 0x4eba); /* 0x2077d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][18], 0x16b25); /* 0x2077d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][19], 0x398a3); /* 0x2077d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][20], 0x1c87f); /* 0x2077d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][21], 0x3a2e3); /* 0x2077d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][22], 0x147e3); /* 0x2077d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][23], 0x332f7); /* 0x2077d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][24], 0x2c23b); /* 0x2077d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][25], 0x1b4a2); /* 0x2077d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][26], 0x2b198); /* 0x2077d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][27], 0x21f69); /* 0x2077d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][28], 0x25961); /* 0x2077d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][29], 0xc16b); /* 0x2077d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][30], 0x32fbb); /* 0x2077d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][31], 0xe25a); /* 0x2077d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][32], 0x1e2f0); /* 0x2077d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][33], 0x2237d); /* 0x2077d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][34], 0x23c47); /* 0x2077d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][35], 0x378cb); /* 0x2077d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][36], 0x1a55c); /* 0x2077d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][37], 0x19af6); /* 0x2077d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][38], 0x12e32); /* 0x2077d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][39], 0x17afb); /* 0x2077d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][40], 0x314e4); /* 0x2077da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][41], 0x34174); /* 0x2077da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][42], 0x1257e); /* 0x2077da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][43], 0xe8b8); /* 0x2077dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][44], 0x322c1); /* 0x2077db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][45], 0x827a); /* 0x2077db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][46], 0x1ab5); /* 0x2077db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][47], 0x2fc90); /* 0x2077dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][48], 0x2a033); /* 0x2077dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][49], 0x2d77c); /* 0x2077dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][50], 0x3a201); /* 0x2077dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][51], 0x28c2e); /* 0x2077dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][0], 0x3814b); /* 0x2077e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][1], 0x915b); /* 0x2077e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][2], 0x2e5d); /* 0x2077e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][3], 0x5d3b); /* 0x2077e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][4], 0x159d6); /* 0x2077e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][5], 0x296e8); /* 0x2077e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][6], 0x2d90d); /* 0x2077e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][7], 0x27463); /* 0x2077e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][8], 0x35d81); /* 0x2077e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][9], 0x31045); /* 0x2077e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][10], 0x3ab5d); /* 0x2077e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][11], 0x18275); /* 0x2077e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][12], 0xee5b); /* 0x2077e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][13], 0x2c2bd); /* 0x2077e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][14], 0x3439a); /* 0x2077e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][15], 0x2c50); /* 0x2077e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][16], 0x1ae55); /* 0x2077e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][17], 0x113c5); /* 0x2077e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][18], 0x23dbc); /* 0x2077e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][19], 0x26df2); /* 0x2077e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][20], 0x1e889); /* 0x2077e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][21], 0x370f3); /* 0x2077e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][22], 0x1da6e); /* 0x2077e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][23], 0x23f97); /* 0x2077e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][24], 0x5336); /* 0x2077e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][25], 0xdaab); /* 0x2077e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][26], 0x2e3fd); /* 0x2077e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][27], 0x28e7b); /* 0x2077e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][28], 0x3d53); /* 0x2077e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][29], 0x3afcd); /* 0x2077e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][30], 0x29f05); /* 0x2077e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][31], 0x1ecbf); /* 0x2077e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][32], 0x99f3); /* 0x2077e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][33], 0x36711); /* 0x2077e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][34], 0x37b52); /* 0x2077e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][35], 0x326d7); /* 0x2077e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][36], 0xa589); /* 0x2077e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][37], 0x3962d); /* 0x2077e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][38], 0x73f9); /* 0x2077e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][39], 0x24b8a); /* 0x2077e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][40], 0x1d083); /* 0x2077ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][41], 0x3c370); /* 0x2077ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][42], 0x6bbd); /* 0x2077ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][43], 0x3276f); /* 0x2077eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][44], 0x2b59); /* 0x2077eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][45], 0x127f5); /* 0x2077eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][46], 0x3651c); /* 0x2077eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][47], 0x18ade); /* 0x2077ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][48], 0x21da2); /* 0x2077ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][49], 0x216e1); /* 0x2077ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][50], 0x3163d); /* 0x2077ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][51], 0x1352f); /* 0x2077ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][0], 0x2d12d); /* 0x2077f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][1], 0x3938d); /* 0x2077f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][2], 0x1404b); /* 0x2077f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][3], 0x29f0c); /* 0x2077f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][4], 0x38c52); /* 0x2077f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][5], 0x891c); /* 0x2077f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][6], 0x25ad5); /* 0x2077f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][7], 0x7046); /* 0x2077f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][8], 0x12cb1); /* 0x2077f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][9], 0x22950); /* 0x2077f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][10], 0x1c9b2); /* 0x2077f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][11], 0x36b37); /* 0x2077f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][12], 0x2cf31); /* 0x2077f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][13], 0x2a44f); /* 0x2077f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][14], 0x25326); /* 0x2077f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][15], 0x44cc); /* 0x2077f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][16], 0x3903b); /* 0x2077f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][17], 0x3f322); /* 0x2077f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][18], 0x37434); /* 0x2077f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][19], 0x33eb3); /* 0x2077f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][20], 0x3d17e); /* 0x2077f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][21], 0x7922); /* 0x2077f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][22], 0x6f3e); /* 0x2077f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][23], 0xea7b); /* 0x2077f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][24], 0x26f3e); /* 0x2077f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][25], 0x3200b); /* 0x2077f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][26], 0x267bc); /* 0x2077f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][27], 0x38bac); /* 0x2077f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][28], 0xee99); /* 0x2077f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][29], 0x117d3); /* 0x2077f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][30], 0x330d1); /* 0x2077f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][31], 0x5b3b); /* 0x2077f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][32], 0x29275); /* 0x2077f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][33], 0x2f31a); /* 0x2077f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][34], 0x34faf); /* 0x2077f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][35], 0x10a65); /* 0x2077f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][36], 0x2329a); /* 0x2077f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][37], 0x34e19); /* 0x2077f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][38], 0x3b25a); /* 0x2077f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][39], 0x2505f); /* 0x2077f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][40], 0x21b68); /* 0x2077fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][41], 0x9655); /* 0x2077fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][42], 0x34bc5); /* 0x2077fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][43], 0x2b417); /* 0x2077fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][44], 0x8627); /* 0x2077fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][45], 0x2544e); /* 0x2077fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][46], 0x4ed8); /* 0x2077fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][47], 0x795a); /* 0x2077fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][48], 0x13965); /* 0x2077fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][49], 0x3236a); /* 0x2077fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][50], 0xd171); /* 0x2077fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][51], 0x2f852); /* 0x2077fcc */
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0], 0x1); /* 0x2070060 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0], 0x3); /* 0x2070060 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0][0], 0x3); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0][1], 0x0); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1], 0x8); /* 0x2070064 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1], 0xc0); /* 0x2070064 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1][0], 0xc0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1][1], 0x0); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2], 0x7); /* 0x2070068 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2], 0x3f); /* 0x2070068 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2][0], 0x3f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2][1], 0x0); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3], 0xf); /* 0x207006c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3], 0xff); /* 0x207006c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3][0], 0xff); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3][1], 0x0); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4], 0xf0); /* 0x2070070 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4], 0xff00); /* 0x2070070 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4][0], 0x0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4][1], 0xff); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5], 0x80); /* 0x2070074 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5], 0xc000); /* 0x2070074 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5][0], 0x0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5][1], 0xc0); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6], 0xf0); /* 0x2070078 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6], 0xff00); /* 0x2070078 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6][0], 0x0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6][1], 0xff); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7], 0x30); /* 0x207007c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7], 0xf00); /* 0x207007c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7][0], 0x0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7][1], 0xf); // regs_31841 fix
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[0][0], 0x18e1bbb); /* 0x2070000 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[0][1], 0x215753e); /* 0x2070004 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[1][0], 0x4442b7); /* 0x2070008 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[1][1], 0x1d00515); /* 0x207000c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[2][0], 0x2192cf6); /* 0x2070010 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[2][1], 0x901063); /* 0x2070014 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[3][0], 0x1178499); /* 0x2070018 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[3][1], 0x2e3e80a); /* 0x207001c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[4][0], 0x804ae3); /* 0x2070020 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[4][1], 0x362b1cf); /* 0x2070024 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[5][0], 0x8a0abb); /* 0x2070028 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[5][1], 0x12f341a); /* 0x207002c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[6][0], 0xf38453); /* 0x2070030 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[6][1], 0x2e131d9); /* 0x2070034 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[7][0], 0x32d48e3); /* 0x2070038 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[7][1], 0x3286b15); /* 0x207003c */
    //Renamed in bfnregs 20150107_182406_7982_mau_dev - comment out for now
    //tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hashout_paritycheck_enable, 0xb5); /* 0x2070040 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[0], 0xfb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[1], 0xf7); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[2], 0x6); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[3], 0x29); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[4], 0x6f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[5], 0xb7); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[6], 0xd4); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[7], 0xbf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[8], 0x1); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[9], 0x33); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[10], 0x4c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[11], 0xb5); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[12], 0x1); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[13], 0x4); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[14], 0x92); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[15], 0x48); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[16], 0xcc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[17], 0x69); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[18], 0x8b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[19], 0xa5); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[20], 0x4c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[21], 0xc0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[22], 0x42); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[23], 0x71); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[24], 0x89); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[25], 0x84); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[26], 0xd6); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[27], 0x3d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[28], 0x93); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[29], 0x79); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[30], 0xe3); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[31], 0x5); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[32], 0x54); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[33], 0x50); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[34], 0xd3); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[35], 0x80); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[36], 0x23); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[37], 0x88); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[38], 0x75); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[39], 0xf9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[40], 0x89); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[41], 0x18); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[42], 0x69); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[43], 0x38); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[44], 0x21); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[45], 0xa0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[46], 0x7); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[47], 0xf8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[48], 0x5a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[49], 0x4e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[50], 0xb2); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[51], 0xd9); // regs_31841 fix
    tu.IndirectWrite(0x0200801180b7, 0x00001377f3f9ffee, 0x00000cdcddeef07f); /* TCAM[ 0][ 6][183].word1 = 0x6e6ef7783f  pt=b00 VV=b01 key=b10 py=1 mr=1 */
    //IndirectWrite 0x00000cdcddeef07f 00001377f3f9ffef to MauMemory (0x00000200801180b7) offset 0x1180b7
    tu.IndirectWrite(0x02008011c0b7, 0x000013ff3bffedfa, 0x00000cfcfe3677de); /* TCAM[ 0][ 7][183].word1 = 0x7e7f1b3bef  pt=b00 VV=b01 key=b10 py=0 mr=1 */
    //IndirectWrite 0x00000cfcfe3677de 000013ff3bffedfb to MauMemory (0x000002008011c0b7) offset 0x11c0b7
    tu.IndirectWrite(0x02008000944b, 0xf000000000000000, 0x0000000000000000); /* sram_ 2_ 5: a=0x2008000944b d0=0xf000000000000000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 f000000000000000 to MauMemory (0x000002008000944b) offset 0x944b
    tu.IndirectWrite(0x02008001db6b, 0x720cfb1d69af2674, 0xb14c106b8d0e05f2); /* sram_ 7_ 6: a=0x2008001db6b d0=0x720cfb1d69af2674 d1=0xb14c106b8d0e05f2 */
    //IndirectWrite 0xb14c106b8d0e05f2 720cfb1d69af2674 to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x0200801001e0, 0x0000117597fbffca, 0x00000fff7bee70f5); /* TCAM[ 0][ 0][480].word1 = 0xffbdf7387a  pt=b00 VV=b01 key=b11 py=1 mr=1 */
    //IndirectWrite 0x00000fff7bee70f5 0000117597fbffcb to MauMemory (0x00000200801001e0) offset 0x1001e0
    tu.IndirectWrite(0x0200801041e0, 0x000011f92dffaf3a, 0x00000fdefe74fcfe); /* TCAM[ 0][ 1][480].word1 = 0xef7f3a7e7f  pt=b00 VV=b01 key=b11 py=0 mr=1 */
    //IndirectWrite 0x00000fdefe74fcfe 000011f92dffaf3b to MauMemory (0x00000200801041e0) offset 0x1041e0
    tu.IndirectWrite(0x02008000941e, 0x00000000000000f0, 0x0000000000000000); /* sram_ 2_ 5: a=0x2008000941e d0=0xf0 d1=0x0 */
    //IndirectWrite 0x0000000000000000 00000000000000f0 to MauMemory (0x000002008000941e) offset 0x941e
    tu.IndirectWrite(0x02008001db6b, 0xf75ffb1febefa675, 0xf57fb4ebed8f97f7); /* sram_ 7_ 6: a=0x2008001db6b d0=0xf75ffb1febefa675 d1=0xf57fb4ebed8f97f7 */
    //IndirectWrite 0xf57fb4ebed8f97f7 f75ffb1febefa675 to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x020080118183, 0x0000132d2d2dffd2, 0x00000cd2d2d2002d); /* TCAM[ 0][ 6][387].word1 = 0x6969690016  pt=b00 VV=b01 key=b10 py=1 mr=1 */
    //IndirectWrite 0x00000cd2d2d2002d 0000132d2d2dffd3 to MauMemory (0x0000020080118183) offset 0x118183
    tu.IndirectWrite(0x02008011c183, 0x0000132d2dff932c, 0x00000cd2d2006cd2); /* TCAM[ 0][ 7][387].word1 = 0x6969003669  pt=b00 VV=b01 key=b10 py=0 mr=1 */
    //IndirectWrite 0x00000cd2d2006cd2 0000132d2dff932d to MauMemory (0x000002008011c183) offset 0x11c183
    tu.IndirectWrite(0x020080009458, 0x00000000f0000000, 0x0000000000000000); /* sram_ 2_ 5: a=0x20080009458 d0=0xf0000000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 00000000f0000000 to MauMemory (0x0000020080009458) offset 0x9458
    tu.IndirectWrite(0x02008001db6b, 0xf7ffff9fefffe67d, 0xf57ff5efffcf97ff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xf7ffff9fefffe67d d1=0xf57ff5efffcf97ff */
    //IndirectWrite 0xf57ff5efffcf97ff f7ffff9fefffe67d to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x02008013853d, 0x000013292929ffd2, 0x00000cd6d6d6002c); /* TCAM[ 1][14][317].word1 = 0x6b6b6b0016  pt=b00 VV=b01 key=b10 py=0 mr=1 */
    //IndirectWrite 0x00000cd6d6d6002c 000013292929ffd3 to MauMemory (0x000002008013853d) offset 0x13853d
    tu.IndirectWrite(0x02008013c53d, 0x0000092929807328, 0x000016d6d67f8cd7); /* TCAM[ 1][15][317].word1 = 0x6b6b3fc66b  pt=b00 VV=b10 key=b11 py=1 mr=1 */
    //IndirectWrite 0x000016d6d67f8cd7 0000092929807329 to MauMemory (0x000002008013c53d) offset 0x13c53d
    tu.IndirectWrite(0x020080009433, 0x0000000000000000, 0x0000f00000000000); /* sram_ 2_ 5: a=0x20080009433 d0=0x0 d1=0xf00000000000 */
    //IndirectWrite 0x0000f00000000000 0000000000000000 to MauMemory (0x0000020080009433) offset 0x9433
    tu.IndirectWrite(0x02008001db6b, 0xf7ffffdffffff77d, 0xfdffffffffcfffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xf7ffffdffffff77d d1=0xfdffffffffcfffff */
    //IndirectWrite 0xfdffffffffcfffff f7ffffdffffff77d to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x020080138544, 0x000013272697ffd8, 0x00000dd8d9680027); /* TCAM[ 1][14][324].word1 = 0xec6cb40013  pt=b00 VV=b01 key=b10 py=1 mr=1 */
    //IndirectWrite 0x00000dd8d9680027 000013272697ffd9 to MauMemory (0x0000020080138544) offset 0x138544
    tu.IndirectWrite(0x02008013c544, 0x0000077fefff9fb6, 0x000018dcf996e6fd); /* TCAM[ 1][15][324].word1 = 0x6e7ccb737e  pt=b00 VV=b11 key=b00 py=1 mr=1 */
    //IndirectWrite 0x000018dcf996e6fd 0000077fefff9fb7 to MauMemory (0x000002008013c544) offset 0x13c544
    tu.IndirectWrite(0x020080009434, 0x000000f000000000, 0x0000000000000000); /* sram_ 2_ 5: a=0x20080009434 d0=0xf000000000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 000000f000000000 to MauMemory (0x0000020080009434) offset 0x9434
    tu.IndirectWrite(0x02008001db6b, 0xffffffdffffff7fd, 0xfdffffffffefffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffdffffff7fd d1=0xfdffffffffefffff */
    //IndirectWrite 0xfdffffffffefffff ffffffdffffff7fd to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x020080118184, 0x00001377ad6ffb9e, 0x00000cdfdafafe75); /* TCAM[ 0][ 6][388].word1 = 0x6fed7d7f3a  pt=b00 VV=b01 key=b10 py=1 mr=1 */
    //IndirectWrite 0x00000cdfdafafe75 00001377ad6ffb9f to MauMemory (0x0000020080118184) offset 0x118184
    tu.IndirectWrite(0x02008011c184, 0x0000132524730526, 0x00000cdadb8cfadb); /* TCAM[ 0][ 7][388].word1 = 0x6d6dc67d6d  pt=b00 VV=b01 key=b10 py=1 mr=1 */
    //IndirectWrite 0x00000cdadb8cfadb 0000132524730527 to MauMemory (0x000002008011c184) offset 0x11c184
    tu.IndirectWrite(0x020080009458, 0x000000f0f0000000, 0x0000000000000000); /* sram_ 2_ 5: a=0x20080009458 d0=0xf0f0000000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 000000f0f0000000 to MauMemory (0x0000020080009458) offset 0x9458
    tu.IndirectWrite(0x02008001db6b, 0xffffffdffffff7ff, 0xffffffffffefffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffdffffff7ff d1=0xffffffffffefffff */
    //IndirectWrite 0xffffffffffefffff ffffffdffffff7ff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x0200801385f3, 0x0000110b1addffe0, 0x00000ef4e522201e); /* TCAM[ 1][14][499].word1 = 0x7a7291100f  pt=b00 VV=b01 key=b11 py=0 mr=1 */
    //IndirectWrite 0x00000ef4e522201e 0000110b1addffe1 to MauMemory (0x00000200801385f3) offset 0x1385f3
    tu.IndirectWrite(0x02008013c5f3, 0x00001b3f3bffbd3e, 0x000005f6f4b9faf4); /* TCAM[ 1][15][499].word1 = 0xfb7a5cfd7a  pt=b00 VV=b00 key=b10 py=0 mr=1 */
    //IndirectWrite 0x000005f6f4b9faf4 00001b3f3bffbd3f to MauMemory (0x000002008013c5f3) offset 0x13c5f3
    tu.IndirectWrite(0x02008000943f, 0x00000000f0000000, 0x0000000000000000); /* sram_ 2_ 5: a=0x2008000943f d0=0xf0000000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 00000000f0000000 to MauMemory (0x000002008000943f) offset 0x943f
    tu.IndirectWrite(0x02008001db6b, 0xffffffdffffff7ff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffdffffff7ff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffdffffff7ff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x0200801001fe, 0x00001baf1addffe4, 0x000004fcfd637c9f); /* TCAM[ 0][ 0][510].word1 = 0x7e7eb1be4f  pt=b00 VV=b00 key=b10 py=1 mr=1 */
    //IndirectWrite 0x000004fcfd637c9f 00001baf1addffe5 to MauMemory (0x00000200801001fe) offset 0x1001fe
    tu.IndirectWrite(0x0200801041fe, 0x000019cb4bfffd3a, 0x000007fcfd2cfae5); /* TCAM[ 0][ 1][510].word1 = 0xfe7e967d72  pt=b00 VV=b00 key=b11 py=1 mr=1 */
    //IndirectWrite 0x000007fcfd2cfae5 000019cb4bfffd3b to MauMemory (0x00000200801041fe) offset 0x1041fe
    tu.IndirectWrite(0x02008000941f, 0x0000000000000000, 0x00f0000000000000); /* sram_ 2_ 5: a=0x2008000941f d0=0x0 d1=0xf0000000000000 */
    //IndirectWrite 0x00f0000000000000 0000000000000000 to MauMemory (0x000002008000941f) offset 0x941f
    tu.IndirectWrite(0x02008001db6b, 0xffffffdffffff7ff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffdffffff7ff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffdffffff7ff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x0200801001ff, 0x000011ac7b55ffee, 0x00000eff9ceb7c95); /* TCAM[ 0][ 0][511].word1 = 0x7fce75be4a  pt=b00 VV=b01 key=b11 py=1 mr=1 */
    //IndirectWrite 0x00000eff9ceb7c95 000011ac7b55ffef to MauMemory (0x00000200801001ff) offset 0x1001ff
    tu.IndirectWrite(0x0200801041ff, 0x000015cd4dacfce4, 0x00000bfafb7ffb3b); /* TCAM[ 0][ 1][511].word1 = 0xfd7dbffd9d  pt=b00 VV=b01 key=b01 py=1 mr=1 */
    //IndirectWrite 0x00000bfafb7ffb3b 000015cd4dacfce5 to MauMemory (0x00000200801041ff) offset 0x1041ff
    tu.IndirectWrite(0x02008000941f, 0x0000000000000000, 0xf0f0000000000000); /* sram_ 2_ 5: a=0x2008000941f d0=0x0 d1=0xf0f0000000000000 */
    //IndirectWrite 0xf0f0000000000000 0000000000000000 to MauMemory (0x000002008000941f) offset 0x941f
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x0200801001fd, 0x000011ac7b55ffee, 0x00000eff9ceb7c95); /* TCAM[ 0][ 0][509].word1 = 0x7fce75be4a  pt=b00 VV=b01 key=b11 py=1 mr=1 */
    //IndirectWrite 0x00000eff9ceb7c95 000011ac7b55ffef to MauMemory (0x00000200801001fd) offset 0x1001fd
    tu.IndirectWrite(0x0200801041fd, 0x000015cd4dacfce4, 0x00000bfafb7ffb3b); /* TCAM[ 0][ 1][509].word1 = 0xfd7dbffd9d  pt=b00 VV=b01 key=b01 py=1 mr=1 */
    //IndirectWrite 0x00000bfafb7ffb3b 000015cd4dacfce5 to MauMemory (0x00000200801041fd) offset 0x1041fd
    tu.IndirectWrite(0x02008000941f, 0x0000000000000000, 0xf0f0f00000000000); /* sram_ 2_ 5: a=0x2008000941f d0=0x0 d1=0xf0f0f00000000000 */
    //IndirectWrite 0xf0f0f00000000000 0000000000000000 to MauMemory (0x000002008000941f) offset 0x941f
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x02008011803f, 0x00001195f797fffe, 0x00000eff9eee14b7); /* TCAM[ 0][ 6][ 63].word1 = 0x7fcf770a5b  pt=b00 VV=b01 key=b11 py=1 mr=1 */
    //IndirectWrite 0x00000eff9eee14b7 00001195f797ffff to MauMemory (0x000002008011803f) offset 0x11803f
    tu.IndirectWrite(0x02008011c03f, 0x00001105048cd4e4, 0x00000efafb732b1b); /* TCAM[ 0][ 7][ 63].word1 = 0x7d7db9958d  pt=b00 VV=b01 key=b11 py=1 mr=1 */
    //IndirectWrite 0x00000efafb732b1b 00001105048cd4e5 to MauMemory (0x000002008011c03f) offset 0x11c03f
    tu.IndirectWrite(0x020080009443, 0x0000000000000000, 0xf000000000000000); /* sram_ 2_ 5: a=0x20080009443 d0=0x0 d1=0xf000000000000000 */
    //IndirectWrite 0xf000000000000000 0000000000000000 to MauMemory (0x0000020080009443) offset 0x9443
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x020080118187, 0x00001195f797fffe, 0x00000eff9eee14b7); /* TCAM[ 0][ 6][391].word1 = 0x7fcf770a5b  pt=b00 VV=b01 key=b11 py=1 mr=1 */
    //IndirectWrite 0x00000eff9eee14b7 00001195f797ffff to MauMemory (0x0000020080118187) offset 0x118187
    tu.IndirectWrite(0x02008011c187, 0x00001105048cd4e4, 0x00000efafb732b1b); /* TCAM[ 0][ 7][391].word1 = 0x7d7db9958d  pt=b00 VV=b01 key=b11 py=1 mr=1 */
    //IndirectWrite 0x00000efafb732b1b 00001105048cd4e5 to MauMemory (0x000002008011c187) offset 0x11c187
    tu.IndirectWrite(0x020080009458, 0xf00000f0f0000000, 0x0000000000000000); /* sram_ 2_ 5: a=0x20080009458 d0=0xf00000f0f0000000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 f00000f0f0000000 to MauMemory (0x0000020080009458) offset 0x9458
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x020080100144, 0x0000134bbb7d417e, 0x00000cfdfeeeffb6); /* TCAM[ 0][ 0][324].word1 = 0x7eff777fdb  pt=b00 VV=b01 key=b10 py=0 mr=1 */
    //IndirectWrite 0x00000cfdfeeeffb6 0000134bbb7d417f to MauMemory (0x0000020080100144) offset 0x100144
    tu.IndirectWrite(0x020080104144, 0x0000132f1bffbfb2, 0x00000dfffc5bfbfd); /* TCAM[ 0][ 1][324].word1 = 0xfffe2dfdfe  pt=b00 VV=b01 key=b10 py=1 mr=1 */
    //IndirectWrite 0x00000dfffc5bfbfd 0000132f1bffbfb3 to MauMemory (0x0000020080104144) offset 0x104144
    tu.IndirectWrite(0x020080009414, 0x000000f000000000, 0x0000000000000000); /* sram_ 2_ 5: a=0x20080009414 d0=0xf000000000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 000000f000000000 to MauMemory (0x0000020080009414) offset 0x9414
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x0200801000cc, 0x000011e4fbffffaa, 0x00000f1b7f8b61f6); /* TCAM[ 0][ 0][204].word1 = 0x8dbfc5b0fb  pt=b00 VV=b01 key=b11 py=0 mr=1 */
    //IndirectWrite 0x00000f1b7f8b61f6 000011e4fbffffab to MauMemory (0x00000200801000cc) offset 0x1000cc
    tu.IndirectWrite(0x0200801040cc, 0x00001ee4e5ff0ab4, 0x0000011b1a00f54b); /* TCAM[ 0][ 1][204].word1 = 0x8d8d007aa5  pt=b00 VV=b00 key=b00 py=1 mr=1 */
    //IndirectWrite 0x0000011b1a00f54b 00001ee4e5ff0ab5 to MauMemory (0x00000200801040cc) offset 0x1040cc
    tu.IndirectWrite(0x02008000940c, 0x0000000000000000, 0x000000f000000000); /* sram_ 2_ 5: a=0x2008000940c d0=0x0 d1=0xf000000000 */
    //IndirectWrite 0x000000f000000000 0000000000000000 to MauMemory (0x000002008000940c) offset 0x940c
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x020080118001, 0x00000ef7f0f3ff9e, 0x0000111f3f2f86f7); /* TCAM[ 0][ 6][  1].word1 = 0x8f9f97c37b  pt=b00 VV=b10 key=b00 py=1 mr=1 */
    //IndirectWrite 0x0000111f3f2f86f7 00000ef7f0f3ff9f to MauMemory (0x0000020080118001) offset 0x118001
    tu.IndirectWrite(0x02008011c001, 0x00000fe6e3ffaebc, 0x0000117fdfadf35e); /* TCAM[ 0][ 7][  1].word1 = 0xbfefd6f9af  pt=b00 VV=b10 key=b00 py=0 mr=1 */
    //IndirectWrite 0x0000117fdfadf35e 00000fe6e3ffaebd to MauMemory (0x000002008011c001) offset 0x11c001
    tu.IndirectWrite(0x020080009440, 0x000000000000f000, 0x0000000000000000); /* sram_ 2_ 5: a=0x20080009440 d0=0xf000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 000000000000f000 to MauMemory (0x0000020080009440) offset 0x9440
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x0200801181b6, 0x00000eeeeeec00ea, 0x000011111113ff15); /* TCAM[ 0][ 6][438].word1 = 0x888889ff8a  pt=b00 VV=b10 key=b00 py=1 mr=1 */
    //IndirectWrite 0x000011111113ff15 00000eeeeeec00eb to MauMemory (0x00000200801181b6) offset 0x1181b6
    tu.IndirectWrite(0x02008011c1b6, 0x00000eee2c00aeae, 0x00001111d3ff5150); /* TCAM[ 0][ 7][438].word1 = 0x88e9ffa8a8  pt=b00 VV=b10 key=b00 py=0 mr=1 */
    //IndirectWrite 0x00001111d3ff5150 00000eee2c00aeaf to MauMemory (0x000002008011c1b6) offset 0x11c1b6
    tu.IndirectWrite(0x02008000945b, 0x00f0000000000000, 0x0000000000000000); /* sram_ 2_ 5: a=0x2008000945b d0=0xf0000000000000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 00f0000000000000 to MauMemory (0x000002008000945b) offset 0x945b
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x02008011803a, 0x00000eedfdd9ff72, 0x00001177573e0fed); /* TCAM[ 0][ 6][ 58].word1 = 0xbbab9f07f6  pt=b00 VV=b10 key=b00 py=1 mr=1 */
    //IndirectWrite 0x00001177573e0fed 00000eedfdd9ff73 to MauMemory (0x000002008011803a) offset 0x11803a
    tu.IndirectWrite(0x02008011c03a, 0x00000eecedff12ac, 0x000011131200ed52); /* TCAM[ 0][ 7][ 58].word1 = 0x89890076a9  pt=b00 VV=b10 key=b00 py=0 mr=1 */
    //IndirectWrite 0x000011131200ed52 00000eecedff12ad to MauMemory (0x000002008011c03a) offset 0x11c03a
    tu.IndirectWrite(0x020080009443, 0x0000000000000000, 0xf000000000f00000); /* sram_ 2_ 5: a=0x20080009443 d0=0x0 d1=0xf000000000f00000 */
    //IndirectWrite 0xf000000000f00000 0000000000000000 to MauMemory (0x0000020080009443) offset 0x9443
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x020080118016, 0x00000ffeefffffd6, 0x000011ffbfd685ff); /* TCAM[ 0][ 6][ 22].word1 = 0xffdfeb42ff  pt=b00 VV=b10 key=b00 py=1 mr=1 */
    //IndirectWrite 0x000011ffbfd685ff 00000ffeefffffd7 to MauMemory (0x0000020080118016) offset 0x118016
    tu.IndirectWrite(0x02008011c016, 0x00000eef0bff75ea, 0x00001155fcf3fbd7); /* TCAM[ 0][ 7][ 22].word1 = 0xaafe79fdeb  pt=b00 VV=b10 key=b00 py=1 mr=1 */
    //IndirectWrite 0x00001155fcf3fbd7 00000eef0bff75eb to MauMemory (0x000002008011c016) offset 0x11c016
    tu.IndirectWrite(0x020080009441, 0x00f0000000000000, 0x0000000000000000); /* sram_ 2_ 5: a=0x20080009441 d0=0xf0000000000000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 00f0000000000000 to MauMemory (0x0000020080009441) offset 0x9441
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x0200801385fb, 0x00000febeae9ffb6, 0x0000115717ff7eef); /* TCAM[ 1][14][507].word1 = 0xab8bffbf77  pt=b00 VV=b10 key=b00 py=1 mr=1 */
    //IndirectWrite 0x0000115717ff7eef 00000febeae9ffb7 to MauMemory (0x00000200801385fb) offset 0x1385fb
    tu.IndirectWrite(0x02008013c5fb, 0x000009e8e9ff16a8, 0x000017171600e957); /* TCAM[ 1][15][507].word1 = 0x8b8b0074ab  pt=b00 VV=b10 key=b11 py=1 mr=1 */
    //IndirectWrite 0x000017171600e957 000009e8e9ff16a9 to MauMemory (0x000002008013c5fb) offset 0x13c5fb
    tu.IndirectWrite(0x02008000943f, 0x00000000f0000000, 0x00000000f0000000); /* sram_ 2_ 5: a=0x2008000943f d0=0xf0000000 d1=0xf0000000 */
    //IndirectWrite 0x00000000f0000000 00000000f0000000 to MauMemory (0x000002008000943f) offset 0x943f
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x0200801385f5, 0x00000ee6e6e5ff18, 0x00001119191a00e7); /* TCAM[ 1][14][501].word1 = 0x8c8c8d0073  pt=b00 VV=b10 key=b00 py=1 mr=1 */
    //IndirectWrite 0x00001119191a00e7 00000ee6e6e5ff19 to MauMemory (0x00000200801385f5) offset 0x1385f5
    tu.IndirectWrite(0x02008013c5f5, 0x000006e6e6a6c4a6, 0x0000191919593b59); /* TCAM[ 1][15][501].word1 = 0x8c8cac9dac  pt=b00 VV=b11 key=b00 py=1 mr=1 */
    //IndirectWrite 0x0000191919593b59 000006e6e6a6c4a7 to MauMemory (0x000002008013c5f5) offset 0x13c5f5
    tu.IndirectWrite(0x02008000943f, 0x0000f000f0000000, 0x00000000f0000000); /* sram_ 2_ 5: a=0x2008000943f d0=0xf000f0000000 d1=0xf0000000 */
    //IndirectWrite 0x00000000f0000000 0000f000f0000000 to MauMemory (0x000002008000943f) offset 0x943f
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x0200801385f6, 0x00001547e6edffbe, 0x00000bfb1bfb7ee7); /* TCAM[ 1][14][502].word1 = 0xfd8dfdbf73  pt=b00 VV=b01 key=b01 py=1 mr=1 */
    //IndirectWrite 0x00000bfb1bfb7ee7 00001547e6edffbf to MauMemory (0x00000200801385f6) offset 0x1385f6
    tu.IndirectWrite(0x02008013c5f6, 0x00001144e5ff1a30, 0x00000fbb1a00e5cf); /* TCAM[ 1][15][502].word1 = 0xdd8d0072e7  pt=b00 VV=b01 key=b11 py=1 mr=1 */
    //IndirectWrite 0x00000fbb1a00e5cf 00001144e5ff1a31 to MauMemory (0x000002008013c5f6) offset 0x13c5f6
    tu.IndirectWrite(0x02008000943f, 0x00f0f000f0000000, 0x00000000f0000000); /* sram_ 2_ 5: a=0x2008000943f d0=0xf0f000f0000000 d1=0xf0000000 */
    //IndirectWrite 0x00000000f0000000 00f0f000f0000000 to MauMemory (0x000002008000943f) offset 0x943f
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x0200801181a8, 0x0000155dfef7ff9e, 0x00000affffda4eff); /* TCAM[ 0][ 6][424].word1 = 0x7fffed277f  pt=b00 VV=b01 key=b01 py=1 mr=1 */
    //IndirectWrite 0x00000affffda4eff 0000155dfef7ff9f to MauMemory (0x00000200801181a8) offset 0x1181a8
    tu.IndirectWrite(0x02008011c1a8, 0x000015dee5ffdb7c, 0x00000bfb7bcbfdef); /* TCAM[ 0][ 7][424].word1 = 0xfdbde5fef7  pt=b00 VV=b01 key=b01 py=1 mr=1 */
    //IndirectWrite 0x00000bfb7bcbfdef 000015dee5ffdb7d to MauMemory (0x000002008011c1a8) offset 0x11c1a8
    tu.IndirectWrite(0x02008000945a, 0x0000000000000000, 0x00000000000000f0); /* sram_ 2_ 5: a=0x2008000945a d0=0x0 d1=0xf0 */
    //IndirectWrite 0x00000000000000f0 0000000000000000 to MauMemory (0x000002008000945a) offset 0x945a
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x020080100035, 0x000002e6e77ffff8, 0x00001dbd1db3e1ee); /* TCAM[ 0][ 0][ 53].word1 = 0xde8ed9f0f7  pt=b00 VV=b11 key=b10 py=0 mr=1 */
    //IndirectWrite 0x00001dbd1db3e1ee 000002e6e77ffff9 to MauMemory (0x0000020080100035) offset 0x100035
    tu.IndirectWrite(0x020080104035, 0x000003faebff7cfe, 0x00001ddd9da9ef7f); /* TCAM[ 0][ 1][ 53].word1 = 0xeeced4f7bf  pt=b00 VV=b11 key=b10 py=1 mr=1 */
    //IndirectWrite 0x00001ddd9da9ef7f 000003faebff7cff to MauMemory (0x0000020080104035) offset 0x104035
    tu.IndirectWrite(0x020080009403, 0x0000f00000000000, 0x0000000000000000); /* sram_ 2_ 5: a=0x20080009403 d0=0xf00000000000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 0000f00000000000 to MauMemory (0x0000020080009403) offset 0x9403
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x0200801385f7, 0x00000ffcecfbff3e, 0x000011ffdfffcee4); /* TCAM[ 1][14][503].word1 = 0xffefffe772  pt=b00 VV=b10 key=b00 py=0 mr=1 */
    //IndirectWrite 0x000011ffdfffcee4 00000ffcecfbff3f to MauMemory (0x00000200801385f7) offset 0x1385f7
    tu.IndirectWrite(0x02008013c5f7, 0x000001ffe5ff9eac, 0x00001f5f9f36ed5e); /* TCAM[ 1][15][503].word1 = 0xafcf9b76af  pt=b00 VV=b11 key=b11 py=0 mr=1 */
    //IndirectWrite 0x00001f5f9f36ed5e 000001ffe5ff9ead to MauMemory (0x000002008013c5f7) offset 0x13c5f7
    tu.IndirectWrite(0x02008000943f, 0xf0f0f000f0000000, 0x00000000f0000000); /* sram_ 2_ 5: a=0x2008000943f d0=0xf0f0f000f0000000 d1=0xf0000000 */
    //IndirectWrite 0x00000000f0000000 f0f0f000f0000000 to MauMemory (0x000002008000943f) offset 0x943f
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x0200801181ae, 0x00000dfecfdcbade, 0x00001377fd77ff66); /* TCAM[ 0][ 6][430].word1 = 0xbbfebbffb3  pt=b00 VV=b10 key=b01 py=0 mr=1 */
    //IndirectWrite 0x00001377fd77ff66 00000dfecfdcbadf to MauMemory (0x00000200801181ae) offset 0x1181ae
    tu.IndirectWrite(0x02008011c1ae, 0x00000ccece009f9e, 0x0000133131ff6161); /* TCAM[ 0][ 7][430].word1 = 0x9898ffb0b0  pt=b00 VV=b10 key=b01 py=1 mr=1 */
    //IndirectWrite 0x0000133131ff6161 00000ccece009f9f to MauMemory (0x000002008011c1ae) offset 0x11c1ae
    tu.IndirectWrite(0x02008000945a, 0x0000000000000000, 0x00f00000000000f0); /* sram_ 2_ 5: a=0x2008000945a d0=0x0 d1=0xf00000000000f0 */
    //IndirectWrite 0x00f00000000000f0 0000000000000000 to MauMemory (0x000002008000945a) offset 0x945a
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x020080100123, 0x00001bdefffd2dfe, 0x0000057973e7ffae); /* TCAM[ 0][ 0][291].word1 = 0xbcb9f3ffd7  pt=b00 VV=b00 key=b10 py=0 mr=1 */
    //IndirectWrite 0x0000057973e7ffae 00001bdefffd2dff to MauMemory (0x0000020080100123) offset 0x100123
    tu.IndirectWrite(0x020080104123, 0x00001fcfceecfe9e, 0x000001bf71ffeb65); /* TCAM[ 0][ 1][291].word1 = 0xdfb8fff5b2  pt=b00 VV=b00 key=b00 py=1 mr=1 */
    //IndirectWrite 0x000001bf71ffeb65 00001fcfceecfe9f to MauMemory (0x0000020080104123) offset 0x104123
    tu.IndirectWrite(0x020080009412, 0x00000000f0000000, 0x0000000000000000); /* sram_ 2_ 5: a=0x20080009412 d0=0xf0000000 d1=0x0 */
    //IndirectWrite 0x0000000000000000 00000000f0000000 to MauMemory (0x0000020080009412) offset 0x9412
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x0200801385ff, 0x00000dfeceffcefe, 0x000013fdfdfbff24); /* TCAM[ 1][14][511].word1 = 0xfefefdff92  pt=b00 VV=b10 key=b01 py=0 mr=1 */
    //IndirectWrite 0x000013fdfdfbff24 00000dfeceffceff to MauMemory (0x00000200801385ff) offset 0x1385ff
    tu.IndirectWrite(0x02008013c5ff, 0x00001fdfcf369e9e, 0x0000017fb5ffed6c); /* TCAM[ 1][15][511].word1 = 0xbfdafff6b6  pt=b00 VV=b00 key=b00 py=0 mr=1 */
    //IndirectWrite 0x0000017fb5ffed6c 00001fdfcf369e9f to MauMemory (0x000002008013c5ff) offset 0x13c5ff
    tu.IndirectWrite(0x02008000943f, 0xf0f0f000f0000000, 0xf0000000f0000000); /* sram_ 2_ 5: a=0x2008000943f d0=0xf0f0f000f0000000 d1=0xf0000000f0000000 */
    //IndirectWrite 0xf0000000f0000000 f0f0f000f0000000 to MauMemory (0x000002008000943f) offset 0x943f
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b
    tu.IndirectWrite(0x0200801000cb, 0x000019fadbdfffb2, 0x000007ff7ff7b8fd); /* TCAM[ 0][ 0][203].word1 = 0xffbffbdc7e  pt=b00 VV=b00 key=b11 py=1 mr=1 */
    //IndirectWrite 0x000007ff7ff7b8fd 000019fadbdfffb3 to MauMemory (0x00000200801000cb) offset 0x1000cb
    tu.IndirectWrite(0x0200801040cb, 0x000019d077ffa6bc, 0x000006efcf67db66); /* TCAM[ 0][ 1][203].word1 = 0x77e7b3edb3  pt=b00 VV=b00 key=b11 py=0 mr=1 */
    //IndirectWrite 0x000006efcf67db66 000019d077ffa6bd to MauMemory (0x00000200801040cb) offset 0x1040cb
    tu.IndirectWrite(0x02008000940c, 0x0000000000000000, 0x000000f0f0000000); /* sram_ 2_ 5: a=0x2008000940c d0=0x0 d1=0xf0f0000000 */
    //IndirectWrite 0x000000f0f0000000 0000000000000000 to MauMemory (0x000002008000940c) offset 0x940c
    tu.IndirectWrite(0x02008001db6b, 0xffffffffffffffff, 0xffffffffffffffff); /* sram_ 7_ 6: a=0x2008001db6b d0=0xffffffffffffffff d1=0xffffffffffffffff */
    //IndirectWrite 0xffffffffffffffff ffffffffffffffff to MauMemory (0x000002008001db6b) offset 0x1db6b



  act_hv_translator.do_writes(&tu);
    Phv *phv_in2 = tu.phv_alloc();
    phv_in2->set_egress();
    phv_in2->set(  0, 0x77303a93); 	/* iphv 32b[ 0,  0] (v=1, bytes   3-  0) #45# gen-RefModel */
    phv_in2->set(  1, 0x77303a93); 	/* iphv 32b[ 0,  1] (v=1, bytes   7-  4) #45# gen-RefModel */
    phv_in2->set(  2, 0x77303a93); 	/* iphv 32b[ 0,  2] (v=1, bytes  11-  8) #45# gen-RefModel */
    phv_in2->set(  3, 0x77303a97); 	/* iphv 32b[ 0,  3] (v=1, bytes  15- 12) #45# gen-RefModel */
    phv_in2->set(  4, 0x77303a9b); 	/* iphv 32b[ 0,  4] (v=1, bytes  19- 16) #45# gen-RefModel */
    phv_in2->set(  5, 0x77303a93); 	/* iphv 32b[ 0,  5] (v=1, bytes  23- 20) #45# gen-RefModel */
    phv_in2->set(  6, 0x77303ab3); 	/* iphv 32b[ 0,  6] (v=1, bytes  27- 24) #45# gen-RefModel */
    phv_in2->set(  7, 0x77303ad3); 	/* iphv 32b[ 0,  7] (v=1, bytes  31- 28) #45# gen-RefModel */
    phv_in2->set(  8, 0x77303a93); 	/* iphv 32b[ 0,  8] (v=1, bytes  35- 32) #45# gen-RefModel */
    phv_in2->set(  9, 0x77303b93); 	/* iphv 32b[ 0,  9] (v=1, bytes  39- 36) #45# gen-RefModel */
    phv_in2->set( 10, 0x77303a93); 	/* iphv 32b[ 0, 10] (v=1, bytes  43- 40) #45# gen-RefModel */
    phv_in2->set( 11, 0x77303e93); 	/* iphv 32b[ 0, 11] (v=1, bytes  47- 44) #45# gen-RefModel */
    phv_in2->set( 12, 0x77303a93); 	/* iphv 32b[ 0, 12] (v=1, bytes  51- 48) #45# gen-RefModel */
    phv_in2->set( 13, 0x77303a93); 	/* iphv 32b[ 0, 13] (v=1, bytes  55- 52) #45# gen-RefModel */
    phv_in2->set( 14, 0x77303a93); 	/* iphv 32b[ 0, 14] (v=1, bytes  59- 56) #45# gen-RefModel */
    phv_in2->set( 15, 0x77307a93); 	/* iphv 32b[ 0, 15] (v=1, bytes  63- 60) #45# gen-RefModel */
    phv_in2->set( 16, 0x7730ba93); 	/* iphv 32b[ 0, 16] (v=1, bytes  67- 64) #45# gen-RefModel */
    phv_in2->set( 17, 0x77313a93); 	/* iphv 32b[ 0, 17] (v=1, bytes  71- 68) #45# gen-RefModel */
    phv_in2->set( 18, 0x77323a93); 	/* iphv 32b[ 0, 18] (v=1, bytes  75- 72) #45# gen-RefModel */
    phv_in2->set( 19, 0x77343a93); 	/* iphv 32b[ 0, 19] (v=1, bytes  79- 76) #45# gen-RefModel */
    phv_in2->set( 20, 0x77383a93); 	/* iphv 32b[ 0, 20] (v=1, bytes  83- 80) #45# gen-RefModel */
    phv_in2->set( 21, 0x77303a93); 	/* iphv 32b[ 0, 21] (v=1, bytes  87- 84) #45# gen-RefModel */
    phv_in2->set( 22, 0x77303a93); 	/* iphv 32b[ 0, 22] (v=1, bytes  91- 88) #45# gen-RefModel */
    phv_in2->set( 23, 0x77703a93); 	/* iphv 32b[ 0, 23] (v=1, bytes  95- 92) #45# gen-RefModel */
    phv_in2->set( 24, 0x77b03a93); 	/* iphv 32b[ 0, 24] (v=1, bytes  99- 96) #45# gen-RefModel */
    phv_in2->set( 25, 0x77303a93); 	/* iphv 32b[ 0, 25] (v=1, bytes 103-100) #45# gen-RefModel */
    phv_in2->set( 26, 0x77303a93); 	/* iphv 32b[ 0, 26] (v=1, bytes 107-104) #45# gen-RefModel */
    phv_in2->set( 27, 0x77303a93); 	/* iphv 32b[ 0, 27] (v=1, bytes 111-108) #45# gen-RefModel */
    phv_in2->set( 28, 0x7f303a93); 	/* iphv 32b[ 0, 28] (v=1, bytes 115-112) #45# gen-RefModel */
    phv_in2->set( 29, 0x77303a93); 	/* iphv 32b[ 0, 29] (v=1, bytes 119-116) #45# gen-RefModel */
    phv_in2->set( 30, 0x77303a93); 	/* iphv 32b[ 0, 30] (v=1, bytes 123-120) #45# gen-RefModel */
    phv_in2->set( 31, 0x77303a93); 	/* iphv 32b[ 0, 31] (v=1, bytes 127-124) #45# gen-RefModel */
    phv_in2->set( 32, 0xf7303a93); 	/* iphv 32b[ 1,  0] (v=1, bytes 131-128) #45# gen-RefModel */
    phv_in2->set( 33, 0x77303a93); 	/* iphv 32b[ 1,  1] (v=1, bytes 135-132) #45# gen-RefModel */
    phv_in2->set( 34, 0x77303a93); 	/* iphv 32b[ 1,  2] (v=1, bytes 139-136) #45# gen-RefModel */
    phv_in2->set( 35, 0x77303a97); 	/* iphv 32b[ 1,  3] (v=1, bytes 143-140) #45# gen-RefModel */
    phv_in2->set( 36, 0x77303a9b); 	/* iphv 32b[ 1,  4] (v=1, bytes 147-144) #45# gen-RefModel */
    phv_in2->set( 37, 0x77303a93); 	/* iphv 32b[ 1,  5] (v=1, bytes 151-148) #45# gen-RefModel */
    phv_in2->set( 38, 0x77303ab3); 	/* iphv 32b[ 1,  6] (v=1, bytes 155-152) #45# gen-RefModel */
    phv_in2->set( 39, 0x77303ad3); 	/* iphv 32b[ 1,  7] (v=1, bytes 159-156) #45# gen-RefModel */
    phv_in2->set( 40, 0x77303a93); 	/* iphv 32b[ 1,  8] (v=1, bytes 163-160) #45# gen-RefModel */
    phv_in2->set( 41, 0x77303b93); 	/* iphv 32b[ 1,  9] (v=1, bytes 167-164) #45# gen-RefModel */
    phv_in2->set( 42, 0x77303a93); 	/* iphv 32b[ 1, 10] (v=1, bytes 171-168) #45# gen-RefModel */
    phv_in2->set( 43, 0x77303e93); 	/* iphv 32b[ 1, 11] (v=1, bytes 175-172) #45# gen-RefModel */
    phv_in2->set( 44, 0x77303a93); 	/* iphv 32b[ 1, 12] (v=1, bytes 179-176) #45# gen-RefModel */
    phv_in2->set( 45, 0x77303a93); 	/* iphv 32b[ 1, 13] (v=1, bytes 183-180) #45# gen-RefModel */
    phv_in2->set( 46, 0x77303a93); 	/* iphv 32b[ 1, 14] (v=1, bytes 187-184) #45# gen-RefModel */
    phv_in2->set( 47, 0x77307a93); 	/* iphv 32b[ 1, 15] (v=1, bytes 191-188) #45# gen-RefModel */
    phv_in2->set( 48, 0x7730ba93); 	/* iphv 32b[ 1, 16] (v=1, bytes 195-192) #45# gen-RefModel */
    phv_in2->set( 49, 0x77313a93); 	/* iphv 32b[ 1, 17] (v=1, bytes 199-196) #45# gen-RefModel */
    phv_in2->set( 50, 0x77323a93); 	/* iphv 32b[ 1, 18] (v=1, bytes 203-200) #45# gen-RefModel */
    phv_in2->set( 51, 0x77343a93); 	/* iphv 32b[ 1, 19] (v=1, bytes 207-204) #45# gen-RefModel */
    phv_in2->set( 52, 0x77383a93); 	/* iphv 32b[ 1, 20] (v=1, bytes 211-208) #45# gen-RefModel */
    phv_in2->set( 53, 0x77303a93); 	/* iphv 32b[ 1, 21] (v=1, bytes 215-212) #45# gen-RefModel */
    phv_in2->set( 54, 0x77303a93); 	/* iphv 32b[ 1, 22] (v=1, bytes 219-216) #45# gen-RefModel */
    phv_in2->set( 55, 0x77703a93); 	/* iphv 32b[ 1, 23] (v=1, bytes 223-220) #45# gen-RefModel */
    phv_in2->set( 56, 0x77b03a93); 	/* iphv 32b[ 1, 24] (v=1, bytes 227-224) #45# gen-RefModel */
    phv_in2->set( 57, 0x77303a93); 	/* iphv 32b[ 1, 25] (v=1, bytes 231-228) #45# gen-RefModel */
    phv_in2->set( 58, 0x77303a93); 	/* iphv 32b[ 1, 26] (v=1, bytes 235-232) #45# gen-RefModel */
    phv_in2->set( 59, 0x77303a93); 	/* iphv 32b[ 1, 27] (v=1, bytes 239-236) #45# gen-RefModel */
    phv_in2->set( 60, 0x7f303a93); 	/* iphv 32b[ 1, 28] (v=1, bytes 243-240) #45# gen-RefModel */
    phv_in2->set( 61, 0x77303a93); 	/* iphv 32b[ 1, 29] (v=1, bytes 247-244) #45# gen-RefModel */
    phv_in2->set( 62, 0x77303a93); 	/* iphv 32b[ 1, 30] (v=1, bytes 251-248) #45# gen-RefModel */
    phv_in2->set( 63, 0x77303a93); 	/* iphv 32b[ 1, 31] (v=1, bytes 255-252) #45# gen-RefModel */
    phv_in2->set( 64, 0x93); 	/* iphv 8b[ 2,  0] (v=1, byte 256-256) #45# gen-RefModel */
    phv_in2->set( 65, 0x93); 	/* iphv 8b[ 2,  1] (v=1, byte 257-257) #45# gen-RefModel */
    phv_in2->set( 66, 0x93); 	/* iphv 8b[ 2,  2] (v=1, byte 258-258) #45# gen-RefModel */
    phv_in2->set( 67, 0x97); 	/* iphv 8b[ 2,  3] (v=1, byte 259-259) #45# gen-RefModel */
    phv_in2->set( 68, 0xc7); 	/* iphv 8b[ 2,  4] (v=1, byte 260-260) #45# gen-RefModel */
    phv_in2->set( 69, 0x93); 	/* iphv 8b[ 2,  5] (v=1, byte 261-261) #45# gen-RefModel */
    phv_in2->set( 70, 0xb3); 	/* iphv 8b[ 2,  6] (v=1, byte 262-262) #45# gen-RefModel */
    phv_in2->set( 71, 0xd3); 	/* iphv 8b[ 2,  7] (v=1, byte 263-263) #45# gen-RefModel */
    phv_in2->set( 72, 0x93); 	/* iphv 8b[ 2,  8] (v=1, byte 264-264) #45# gen-RefModel */
    phv_in2->set( 73, 0x93); 	/* iphv 8b[ 2,  9] (v=1, byte 265-265) #45# gen-RefModel */
    phv_in2->set( 74, 0x93); 	/* iphv 8b[ 2, 10] (v=1, byte 266-266) #45# gen-RefModel */
    phv_in2->set( 75, 0x97); 	/* iphv 8b[ 2, 11] (v=1, byte 267-267) #45# gen-RefModel */
    phv_in2->set( 76, 0x9b); 	/* iphv 8b[ 2, 12] (v=1, byte 268-268) #45# gen-RefModel */
    phv_in2->set( 77, 0x93); 	/* iphv 8b[ 2, 13] (v=1, byte 269-269) #45# gen-RefModel */
    phv_in2->set( 79, 0xd3); 	/* iphv 8b[ 2, 15] (v=1, byte 271-271) #45# gen-RefModel */
    phv_in2->set( 80, 0x93); 	/* iphv 8b[ 2, 16] (v=1, byte 272-272) #45# gen-RefModel */
    phv_in2->set( 81, 0x93); 	/* iphv 8b[ 2, 17] (v=1, byte 273-273) #45# gen-RefModel */
    phv_in2->set( 82, 0x93); 	/* iphv 8b[ 2, 18] (v=1, byte 274-274) #45# gen-RefModel */
    phv_in2->set( 83, 0x97); 	/* iphv 8b[ 2, 19] (v=1, byte 275-275) #45# gen-RefModel */
    phv_in2->set( 84, 0x9b); 	/* iphv 8b[ 2, 20] (v=1, byte 276-276) #45# gen-RefModel */
    phv_in2->set( 85, 0x93); 	/* iphv 8b[ 2, 21] (v=1, byte 277-277) #45# gen-RefModel */
    phv_in2->set( 86, 0xb3); 	/* iphv 8b[ 2, 22] (v=1, byte 278-278) #45# gen-RefModel */
    phv_in2->set( 87, 0xd3); 	/* iphv 8b[ 2, 23] (v=1, byte 279-279) #45# gen-RefModel */
    phv_in2->set( 88, 0x93); 	/* iphv 8b[ 2, 24] (v=1, byte 280-280) #45# gen-RefModel */
    phv_in2->set( 89, 0x93); 	/* iphv 8b[ 2, 25] (v=1, byte 281-281) #45# gen-RefModel */
    phv_in2->set( 90, 0x93); 	/* iphv 8b[ 2, 26] (v=1, byte 282-282) #45# gen-RefModel */
    phv_in2->set( 91, 0x97); 	/* iphv 8b[ 2, 27] (v=1, byte 283-283) #45# gen-RefModel */
    phv_in2->set( 92, 0x37); 	/* iphv 8b[ 2, 28] (v=1, byte 284-284) #45# gen-RefModel */
    phv_in2->set( 93, 0x93); 	/* iphv 8b[ 2, 29] (v=1, byte 285-285) #45# gen-RefModel */
    phv_in2->set( 94, 0xb3); 	/* iphv 8b[ 2, 30] (v=1, byte 286-286) #45# gen-RefModel */
    phv_in2->set( 95, 0xd3); 	/* iphv 8b[ 2, 31] (v=1, byte 287-287) #45# gen-RefModel */
    phv_in2->set( 96, 0x93); 	/* iphv 8b[ 3,  0] (v=1, byte 288-288) #45# gen-RefModel */
    phv_in2->set( 97, 0x93); 	/* iphv 8b[ 3,  1] (v=1, byte 289-289) #45# gen-RefModel */
    phv_in2->set( 98, 0x93); 	/* iphv 8b[ 3,  2] (v=1, byte 290-290) #45# gen-RefModel */
    phv_in2->set( 99, 0x97); 	/* iphv 8b[ 3,  3] (v=1, byte 291-291) #45# gen-RefModel */
    phv_in2->set(100, 0x9b); 	/* iphv 8b[ 3,  4] (v=1, byte 292-292) #45# gen-RefModel */
    phv_in2->set(101, 0x93); 	/* iphv 8b[ 3,  5] (v=1, byte 293-293) #45# gen-RefModel */
    phv_in2->set(102, 0xb3); 	/* iphv 8b[ 3,  6] (v=1, byte 294-294) #45# gen-RefModel */
    phv_in2->set(104, 0x93); 	/* iphv 8b[ 3,  8] (v=1, byte 296-296) #45# gen-RefModel */
    phv_in2->set(105, 0x93); 	/* iphv 8b[ 3,  9] (v=1, byte 297-297) #45# gen-RefModel */
    phv_in2->set(106, 0x93); 	/* iphv 8b[ 3, 10] (v=1, byte 298-298) #45# gen-RefModel */
    phv_in2->set(107, 0x97); 	/* iphv 8b[ 3, 11] (v=1, byte 299-299) #45# gen-RefModel */
    phv_in2->set(108, 0x9b); 	/* iphv 8b[ 3, 12] (v=1, byte 300-300) #45# gen-RefModel */
    phv_in2->set(109, 0x93); 	/* iphv 8b[ 3, 13] (v=1, byte 301-301) #45# gen-RefModel */
    phv_in2->set(110, 0xb3); 	/* iphv 8b[ 3, 14] (v=1, byte 302-302) #45# gen-RefModel */
    phv_in2->set(111, 0xd3); 	/* iphv 8b[ 3, 15] (v=1, byte 303-303) #45# gen-RefModel */
    phv_in2->set(112, 0x93); 	/* iphv 8b[ 3, 16] (v=1, byte 304-304) #45# gen-RefModel */
    phv_in2->set(113, 0x93); 	/* iphv 8b[ 3, 17] (v=1, byte 305-305) #45# gen-RefModel */
    phv_in2->set(114, 0x93); 	/* iphv 8b[ 3, 18] (v=1, byte 306-306) #45# gen-RefModel */
    phv_in2->set(115, 0x97); 	/* iphv 8b[ 3, 19] (v=1, byte 307-307) #45# gen-RefModel */
    phv_in2->set(116, 0x9b); 	/* iphv 8b[ 3, 20] (v=1, byte 308-308) #45# gen-RefModel */
    phv_in2->set(117, 0x93); 	/* iphv 8b[ 3, 21] (v=1, byte 309-309) #45# gen-RefModel */
    phv_in2->set(118, 0xb3); 	/* iphv 8b[ 3, 22] (v=1, byte 310-310) #45# gen-RefModel */
    phv_in2->set(119, 0xd3); 	/* iphv 8b[ 3, 23] (v=1, byte 311-311) #45# gen-RefModel */
    phv_in2->set(120, 0x93); 	/* iphv 8b[ 3, 24] (v=1, byte 312-312) #45# gen-RefModel */
    phv_in2->set(121, 0x93); 	/* iphv 8b[ 3, 25] (v=1, byte 313-313) #45# gen-RefModel */
    phv_in2->set(122, 0x93); 	/* iphv 8b[ 3, 26] (v=1, byte 314-314) #45# gen-RefModel */
    phv_in2->set(123, 0x97); 	/* iphv 8b[ 3, 27] (v=1, byte 315-315) #45# gen-RefModel */
    phv_in2->set(124, 0x9b); 	/* iphv 8b[ 3, 28] (v=1, byte 316-316) #45# gen-RefModel */
    phv_in2->set(125, 0x93); 	/* iphv 8b[ 3, 29] (v=1, byte 317-317) #45# gen-RefModel */
    phv_in2->set(126, 0xb3); 	/* iphv 8b[ 3, 30] (v=1, byte 318-318) #45# gen-RefModel */
    phv_in2->set(127, 0xd3); 	/* iphv 8b[ 3, 31] (v=1, byte 319-319) #45# gen-RefModel */
    phv_in2->set(128, 0x006c); 	/* iphv 16b[ 4,  0] (v=1, 321-320) #45# gen-RefModel */
    phv_in2->set(129, 0x006d); 	/* iphv 16b[ 4,  1] (v=1, 323-322) #45# gen-RefModel */
    phv_in2->set(130, 0x006e); 	/* iphv 16b[ 4,  2] (v=1, 325-324) #45# gen-RefModel */
    phv_in2->set(131, 0x006c); 	/* iphv 16b[ 4,  3] (v=1, 327-326) #45# gen-RefModel */
    phv_in2->set(132, 0x006c); 	/* iphv 16b[ 4,  4] (v=1, 329-328) #45# gen-RefModel */
    phv_in2->set(133, 0x007c); 	/* iphv 16b[ 4,  5] (v=1, 331-330) #45# gen-RefModel */
    phv_in2->set(134, 0x006c); 	/* iphv 16b[ 4,  6] (v=1, 333-332) #45# gen-RefModel */
    phv_in2->set(135, 0x006c); 	/* iphv 16b[ 4,  7] (v=1, 335-334) #45# gen-RefModel */
    phv_in2->set(136, 0xffec); 	/* iphv 16b[ 4,  8] (v=1, 337-336) #45# gen-RefModel */
    phv_in2->set(138, 0x006e); 	/* iphv 16b[ 4, 10] (v=1, 341-340) #45# gen-RefModel */
    phv_in2->set(139, 0x006c); 	/* iphv 16b[ 4, 11] (v=1, 343-342) #45# gen-RefModel */
    phv_in2->set(140, 0x006c); 	/* iphv 16b[ 4, 12] (v=1, 345-344) #45# gen-RefModel */
    phv_in2->set(141, 0x007c); 	/* iphv 16b[ 4, 13] (v=1, 347-346) #45# gen-RefModel */
    phv_in2->set(142, 0x006c); 	/* iphv 16b[ 4, 14] (v=1, 349-348) #45# gen-RefModel */
    phv_in2->set(143, 0x006c); 	/* iphv 16b[ 4, 15] (v=1, 351-350) #45# gen-RefModel */
    phv_in2->set(144, 0xffec); 	/* iphv 16b[ 4, 16] (v=1, 353-352) #45# gen-RefModel */
    phv_in2->set(145, 0x006d); 	/* iphv 16b[ 4, 17] (v=1, 355-354) #45# gen-RefModel */
    phv_in2->set(146, 0x006e); 	/* iphv 16b[ 4, 18] (v=1, 357-356) #45# gen-RefModel */
    phv_in2->set(147, 0x006c); 	/* iphv 16b[ 4, 19] (v=1, 359-358) #45# gen-RefModel */
    phv_in2->set(148, 0x006c); 	/* iphv 16b[ 4, 20] (v=1, 361-360) #45# gen-RefModel */
    phv_in2->set(149, 0x007c); 	/* iphv 16b[ 4, 21] (v=1, 363-362) #45# gen-RefModel */
    phv_in2->set(150, 0x006c); 	/* iphv 16b[ 4, 22] (v=1, 365-364) #45# gen-RefModel */
    phv_in2->set(151, 0x006c); 	/* iphv 16b[ 4, 23] (v=1, 367-366) #45# gen-RefModel */
    phv_in2->set(152, 0xffec); 	/* iphv 16b[ 4, 24] (v=1, 369-368) #45# gen-RefModel */
    phv_in2->set(153, 0x006d); 	/* iphv 16b[ 4, 25] (v=1, 371-370) #45# gen-RefModel */
    phv_in2->set(154, 0x006e); 	/* iphv 16b[ 4, 26] (v=1, 373-372) #45# gen-RefModel */
    phv_in2->set(155, 0x006c); 	/* iphv 16b[ 4, 27] (v=1, 375-374) #45# gen-RefModel */
    phv_in2->set(156, 0x006c); 	/* iphv 16b[ 4, 28] (v=1, 377-376) #45# gen-RefModel */
    phv_in2->set(157, 0x007c); 	/* iphv 16b[ 4, 29] (v=1, 379-378) #45# gen-RefModel */
    phv_in2->set(158, 0x006c); 	/* iphv 16b[ 4, 30] (v=1, 381-380) #45# gen-RefModel */
    phv_in2->set(159, 0x006c); 	/* iphv 16b[ 4, 31] (v=1, 383-382) #45# gen-RefModel */
    phv_in2->set(160, 0xffec); 	/* iphv 16b[ 5,  0] (v=1, 385-384) #45# gen-RefModel */
    phv_in2->set(161, 0x006d); 	/* iphv 16b[ 5,  1] (v=1, 387-386) #45# gen-RefModel */
    phv_in2->set(162, 0x006e); 	/* iphv 16b[ 5,  2] (v=1, 389-388) #45# gen-RefModel */
    phv_in2->set(163, 0x006c); 	/* iphv 16b[ 5,  3] (v=1, 391-390) #45# gen-RefModel */
    phv_in2->set(164, 0x006c); 	/* iphv 16b[ 5,  4] (v=1, 393-392) #45# gen-RefModel */
    phv_in2->set(165, 0x007c); 	/* iphv 16b[ 5,  5] (v=1, 395-394) #45# gen-RefModel */
    phv_in2->set(166, 0x006c); 	/* iphv 16b[ 5,  6] (v=1, 397-396) #45# gen-RefModel */
    phv_in2->set(167, 0x006c); 	/* iphv 16b[ 5,  7] (v=1, 399-398) #45# gen-RefModel */
    phv_in2->set(168, 0xffec); 	/* iphv 16b[ 5,  8] (v=1, 401-400) #45# gen-RefModel */
    phv_in2->set(169, 0x006d); 	/* iphv 16b[ 5,  9] (v=1, 403-402) #45# gen-RefModel */
    phv_in2->set(170, 0x006e); 	/* iphv 16b[ 5, 10] (v=1, 405-404) #45# gen-RefModel */
    phv_in2->set(171, 0x006c); 	/* iphv 16b[ 5, 11] (v=1, 407-406) #45# gen-RefModel */
    phv_in2->set(172, 0x006c); 	/* iphv 16b[ 5, 12] (v=1, 409-408) #45# gen-RefModel */
    phv_in2->set(173, 0x007c); 	/* iphv 16b[ 5, 13] (v=1, 411-410) #45# gen-RefModel */
    phv_in2->set(174, 0x006c); 	/* iphv 16b[ 5, 14] (v=1, 413-412) #45# gen-RefModel */
    phv_in2->set(175, 0x006c); 	/* iphv 16b[ 5, 15] (v=1, 415-414) #45# gen-RefModel */
    phv_in2->set(176, 0xffec); 	/* iphv 16b[ 5, 16] (v=1, 417-416) #45# gen-RefModel */
    phv_in2->set(177, 0x006d); 	/* iphv 16b[ 5, 17] (v=1, 419-418) #45# gen-RefModel */
    phv_in2->set(178, 0x006e); 	/* iphv 16b[ 5, 18] (v=1, 421-420) #45# gen-RefModel */
    phv_in2->set(179, 0x006c); 	/* iphv 16b[ 5, 19] (v=1, 423-422) #45# gen-RefModel */
    phv_in2->set(180, 0x006c); 	/* iphv 16b[ 5, 20] (v=1, 425-424) #45# gen-RefModel */
    phv_in2->set(181, 0x007c); 	/* iphv 16b[ 5, 21] (v=1, 427-426) #45# gen-RefModel */
    phv_in2->set(182, 0x006c); 	/* iphv 16b[ 5, 22] (v=1, 429-428) #45# gen-RefModel */
    phv_in2->set(183, 0x006c); 	/* iphv 16b[ 5, 23] (v=1, 431-430) #45# gen-RefModel */
    phv_in2->set(184, 0xffec); 	/* iphv 16b[ 5, 24] (v=1, 433-432) #45# gen-RefModel */
    phv_in2->set(185, 0x006d); 	/* iphv 16b[ 5, 25] (v=1, 435-434) #45# gen-RefModel */
    phv_in2->set(186, 0x006e); 	/* iphv 16b[ 5, 26] (v=1, 437-436) #45# gen-RefModel */
    phv_in2->set(187, 0x006c); 	/* iphv 16b[ 5, 27] (v=1, 439-438) #45# gen-RefModel */
    phv_in2->set(188, 0x006c); 	/* iphv 16b[ 5, 28] (v=1, 441-440) #45# gen-RefModel */
    phv_in2->set(189, 0x007c); 	/* iphv 16b[ 5, 29] (v=1, 443-442) #45# gen-RefModel */
    phv_in2->set(190, 0x006c); 	/* iphv 16b[ 5, 30] (v=1, 445-444) #45# gen-RefModel */
    phv_in2->set(191, 0x006c); 	/* iphv 16b[ 5, 31] (v=1, 447-446) #45# gen-RefModel */
    phv_in2->set(192, 0xffec); 	/* iphv 16b[ 6,  0] (v=1, 449-448) #45# gen-RefModel */
    phv_in2->set(193, 0x006d); 	/* iphv 16b[ 6,  1] (v=1, 451-450) #45# gen-RefModel */
    phv_in2->set(194, 0x006e); 	/* iphv 16b[ 6,  2] (v=1, 453-452) #45# gen-RefModel */
    phv_in2->set(195, 0x006c); 	/* iphv 16b[ 6,  3] (v=1, 455-454) #45# gen-RefModel */
    phv_in2->set(196, 0x006c); 	/* iphv 16b[ 6,  4] (v=1, 457-456) #45# gen-RefModel */
    phv_in2->set(197, 0x007c); 	/* iphv 16b[ 6,  5] (v=1, 459-458) #45# gen-RefModel */
    phv_in2->set(198, 0x006c); 	/* iphv 16b[ 6,  6] (v=1, 461-460) #45# gen-RefModel */
    phv_in2->set(199, 0x006c); 	/* iphv 16b[ 6,  7] (v=1, 463-462) #45# gen-RefModel */
    phv_in2->set(200, 0xffec); 	/* iphv 16b[ 6,  8] (v=1, 465-464) #45# gen-RefModel */
    phv_in2->set(201, 0x006d); 	/* iphv 16b[ 6,  9] (v=1, 467-466) #45# gen-RefModel */
    phv_in2->set(202, 0x006e); 	/* iphv 16b[ 6, 10] (v=1, 469-468) #45# gen-RefModel */
    phv_in2->set(203, 0x006c); 	/* iphv 16b[ 6, 11] (v=1, 471-470) #45# gen-RefModel */
    phv_in2->set(204, 0x006c); 	/* iphv 16b[ 6, 12] (v=1, 473-472) #45# gen-RefModel */
    phv_in2->set(205, 0x007c); 	/* iphv 16b[ 6, 13] (v=1, 475-474) #45# gen-RefModel */
    phv_in2->set(206, 0x006c); 	/* iphv 16b[ 6, 14] (v=1, 477-476) #45# gen-RefModel */
    phv_in2->set(207, 0x006c); 	/* iphv 16b[ 6, 15] (v=1, 479-478) #45# gen-RefModel */
    phv_in2->set(208, 0xffec); 	/* iphv 16b[ 6, 16] (v=1, 481-480) #45# gen-RefModel */
    phv_in2->set(209, 0x006d); 	/* iphv 16b[ 6, 17] (v=1, 483-482) #45# gen-RefModel */
    phv_in2->set(210, 0x006e); 	/* iphv 16b[ 6, 18] (v=1, 485-484) #45# gen-RefModel */
    phv_in2->set(211, 0x006c); 	/* iphv 16b[ 6, 19] (v=1, 487-486) #45# gen-RefModel */
    phv_in2->set(212, 0x006c); 	/* iphv 16b[ 6, 20] (v=1, 489-488) #45# gen-RefModel */
    phv_in2->set(213, 0x007c); 	/* iphv 16b[ 6, 21] (v=1, 491-490) #45# gen-RefModel */
    phv_in2->set(215, 0x006c); 	/* iphv 16b[ 6, 23] (v=1, 495-494) #45# gen-RefModel */
    phv_in2->set(216, 0xffec); 	/* iphv 16b[ 6, 24] (v=1, 497-496) #45# gen-RefModel */
    phv_in2->set(217, 0x006d); 	/* iphv 16b[ 6, 25] (v=1, 499-498) #45# gen-RefModel */
    phv_in2->set(218, 0x006e); 	/* iphv 16b[ 6, 26] (v=1, 501-500) #45# gen-RefModel */
    phv_in2->set(219, 0x006c); 	/* iphv 16b[ 6, 27] (v=1, 503-502) #45# gen-RefModel */
    phv_in2->set(220, 0x006c); 	/* iphv 16b[ 6, 28] (v=1, 505-504) #45# gen-RefModel */
    phv_in2->set(221, 0x007c); 	/* iphv 16b[ 6, 29] (v=1, 507-506) #45# gen-RefModel */
    phv_in2->set(222, 0x006c); 	/* iphv 16b[ 6, 30] (v=1, 509-508) #45# gen-RefModel */
    phv_in2->set(223, 0x006c); 	/* iphv 16b[ 6, 31] (v=1, 511-510) #45# gen-RefModel */


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
    RMT_UT_LOG_INFO("Dv20Test::InPkt=%p [DA=%04X%08X]\n", p_in,
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
    EXPECT_EQ(0x0, phv_out2->get(3)); // was 0x77303a97 in input phv
    EXPECT_EQ(0x0, phv_out2->get(8)); // was 0x77303a93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(16)); // was 0x7730ba93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(18)); // was 0x77323a93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(19)); // was 0x77343a93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(24)); // was 0x77b03a93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(26)); // was 0x77303a93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(27)); // was 0x77303a93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(30)); // was 0x77303a93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(36)); // was 0x77303a9b in input phv
    EXPECT_EQ(0x0, phv_out2->get(37)); // was 0x77303a93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(41)); // was 0x77303b93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(46)); // was 0x77303a93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(49)); // was 0x77313a93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(50)); // was 0x77323a93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(54)); // was 0x77303a93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(61)); // was 0x77303a93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(63)); // was 0x77303a93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(66)); // was 0x93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(70)); // was 0xb3 in input phv
    EXPECT_EQ(0x0, phv_out2->get(74)); // was 0x93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(77)); // was 0x93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(80)); // was 0x93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(85)); // was 0x93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(94)); // was 0xb3 in input phv
    EXPECT_EQ(0x0, phv_out2->get(95)); // was 0xd3 in input phv
    EXPECT_EQ(0x0, phv_out2->get(99)); // was 0x97 in input phv
    EXPECT_EQ(0x0, phv_out2->get(100)); // was 0x9b in input phv
    EXPECT_EQ(0x0, phv_out2->get(102)); // was 0xb3 in input phv
    EXPECT_EQ(0x0, phv_out2->get(107)); // was 0x97 in input phv
    EXPECT_EQ(0x0, phv_out2->get(109)); // was 0x93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(111)); // was 0xd3 in input phv
    EXPECT_EQ(0x0, phv_out2->get(112)); // was 0x93 in input phv
    EXPECT_EQ(0x0, phv_out2->get(116)); // was 0x9b in input phv
    EXPECT_EQ(0x0, phv_out2->get(128)); // was 0x6c in input phv
    EXPECT_EQ(0x0, phv_out2->get(132)); // was 0x6c in input phv
    EXPECT_EQ(0x0, phv_out2->get(134)); // was 0x6c in input phv
    EXPECT_EQ(0x0, phv_out2->get(135)); // was 0x6c in input phv
    EXPECT_EQ(0x0, phv_out2->get(149)); // was 0x7c in input phv
    EXPECT_EQ(0x6c, phv_out2->get(160)); // was 0xffec in input phv
    EXPECT_EQ(0x0, phv_out2->get(162)); // was 0x6e in input phv
    EXPECT_EQ(0x6d, phv_out2->get(163)); // was 0x6c in input phv
    EXPECT_EQ(0x0, phv_out2->get(167)); // was 0x6c in input phv
    EXPECT_EQ(0x0, phv_out2->get(175)); // was 0x6c in input phv
    EXPECT_EQ(0x0, phv_out2->get(176)); // was 0xffec in input phv
    EXPECT_EQ(0x0, phv_out2->get(178)); // was 0x6e in input phv
    EXPECT_EQ(0x0, phv_out2->get(181)); // was 0x7c in input phv
    EXPECT_EQ(0x0, phv_out2->get(191)); // was 0x6c in input phv
    EXPECT_EQ(0x0, phv_out2->get(194)); // was 0x6e in input phv
    EXPECT_EQ(0x0, phv_out2->get(199)); // was 0x6c in input phv
    EXPECT_EQ(0x0, phv_out2->get(203)); // was 0x6c in input phv
    EXPECT_EQ(0x0, phv_out2->get(207)); // was 0x6c in input phv
    EXPECT_EQ(0x0, phv_out2->get(216)); // was 0xffec in input phv
    EXPECT_EQ(0x0, phv_out2->get(222)); // was 0x6c in input phv
    EXPECT_EQ(0x0, phv_out2->get(223)); // was 0x6c in input phv

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
