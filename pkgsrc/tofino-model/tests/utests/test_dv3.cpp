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

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

  bool dv3_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv3Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv3_print) RMT_UT_LOG_INFO("test_dv3_packet1()\n");
    
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
    tu.set_dv_test(3);
    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
    
    
    auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);

    tu.OutWordPiT(0, 0, &mau_reg_map.dp.phv_ingress_thread[0][0], 0xffffffff); /* 0x4060180 */
    tu.OutWordPiT(0, 1, &mau_reg_map.dp.phv_ingress_thread[0][1], 0xffffffff); /* 0x4060184 */
    tu.OutWordPiT(0, 2, &mau_reg_map.dp.phv_ingress_thread[0][2], 0xffffffff); /* 0x4060188 */
    tu.OutWordPiT(0, 3, &mau_reg_map.dp.phv_ingress_thread[0][3], 0xffffffff); /* 0x406018c */
    tu.OutWordPiT(0, 4, &mau_reg_map.dp.phv_ingress_thread[0][4], 0xffffffff); /* 0x4060190 */
    tu.OutWordPiT(0, 5, &mau_reg_map.dp.phv_ingress_thread[0][5], 0xffffffff); /* 0x4060194 */
    tu.OutWordPiT(0, 6, &mau_reg_map.dp.phv_ingress_thread[0][6], 0xffffffff); /* 0x4060198 */
    tu.OutWordPiT(1, 0, &mau_reg_map.dp.phv_ingress_thread[1][0], 0xffffffff); /* 0x40601a0 */
    tu.OutWordPiT(1, 1, &mau_reg_map.dp.phv_ingress_thread[1][1], 0xffffffff); /* 0x40601a4 */
    tu.OutWordPiT(1, 2, &mau_reg_map.dp.phv_ingress_thread[1][2], 0xffffffff); /* 0x40601a8 */
    tu.OutWordPiT(1, 3, &mau_reg_map.dp.phv_ingress_thread[1][3], 0xffffffff); /* 0x40601ac */
    tu.OutWordPiT(1, 4, &mau_reg_map.dp.phv_ingress_thread[1][4], 0xffffffff); /* 0x40601b0 */
    tu.OutWordPiT(1, 5, &mau_reg_map.dp.phv_ingress_thread[1][5], 0xffffffff); /* 0x40601b4 */
    tu.OutWordPiT(1, 6, &mau_reg_map.dp.phv_ingress_thread[1][6], 0xffffffff); /* 0x40601b8 */
    tu.OutWord(&mau_reg_map.dp.action_output_delay[0], 0xd); /* 0x4060030 */
    //Removed in bfnregs 20150107_182406_7982_mau_dev
    //tu.OutWord(&mau_reg_map.dp.final_output_delay[0][0], 0x10); /* 0x4060040 */
    tu.OutWord(&mau_reg_map.dp.pipelength_added_stages[0], 0x2); /* 0x4060070 */
    tu.OutWord(&mau_reg_map.dp.action_output_delay[1], 0xd); /* 0x4060038 */
    //Removed in bfnregs 20150107_182406_7982_mau_dev
    //tu.OutWord(&mau_reg_map.dp.final_output_delay[1][0], 0x14); /* 0x4060048 */
    tu.OutWord(&mau_reg_map.dp.phv_fifo_enable, 0xc); /* 0x4060120 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][0], 0x10); /* 0x4067000 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][1], 0x10); /* 0x4067004 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][2], 0x10); /* 0x4067008 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][3], 0x10); /* 0x406700c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][4], 0x11); /* 0x4067010 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][5], 0x11); /* 0x4067014 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][6], 0x11); /* 0x4067018 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][7], 0x11); /* 0x406701c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][8], 0x12); /* 0x4067020 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][9], 0x12); /* 0x4067024 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][10], 0x12); /* 0x4067028 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][11], 0x12); /* 0x406702c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][12], 0x13); /* 0x4067030 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][13], 0x13); /* 0x4067034 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][14], 0x13); /* 0x4067038 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][15], 0x13); /* 0x406703c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][16], 0x14); /* 0x4067040 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][17], 0x14); /* 0x4067044 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][18], 0x14); /* 0x4067048 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][19], 0x14); /* 0x406704c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][20], 0x15); /* 0x4067050 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][21], 0x15); /* 0x4067054 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][22], 0x15); /* 0x4067058 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][23], 0x15); /* 0x406705c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][24], 0x16); /* 0x4067060 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][25], 0x16); /* 0x4067064 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][26], 0x16); /* 0x4067068 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][27], 0x16); /* 0x406706c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][28], 0x17); /* 0x4067070 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][29], 0x17); /* 0x4067074 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][30], 0x17); /* 0x4067078 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][31], 0x17); /* 0x406707c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][32], 0x18); /* 0x4067080 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][33], 0x18); /* 0x4067084 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][34], 0x18); /* 0x4067088 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][35], 0x18); /* 0x406708c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][36], 0x19); /* 0x4067090 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][37], 0x19); /* 0x4067094 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][38], 0x19); /* 0x4067098 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][39], 0x19); /* 0x406709c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][40], 0x1a); /* 0x40670a0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][41], 0x1a); /* 0x40670a4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][42], 0x1a); /* 0x40670a8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][43], 0x1a); /* 0x40670ac */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][44], 0x1b); /* 0x40670b0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][45], 0x1b); /* 0x40670b4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][46], 0x1b); /* 0x40670b8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][47], 0x1b); /* 0x40670bc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][48], 0x1c); /* 0x40670c0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][49], 0x1c); /* 0x40670c4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][50], 0x1c); /* 0x40670c8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][51], 0x1c); /* 0x40670cc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][52], 0x1d); /* 0x40670d0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][53], 0x1d); /* 0x40670d4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][54], 0x1d); /* 0x40670d8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][55], 0x1d); /* 0x40670dc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][56], 0x1e); /* 0x40670e0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][57], 0x1e); /* 0x40670e4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][58], 0x1e); /* 0x40670e8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][59], 0x1e); /* 0x40670ec */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][60], 0x1f); /* 0x40670f0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][61], 0x1f); /* 0x40670f4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][62], 0x1f); /* 0x40670f8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][63], 0x1f); /* 0x40670fc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][64], 0x10); /* 0x4067500 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][65], 0x10); /* 0x4067504 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][66], 0x10); /* 0x4067508 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][67], 0x10); /* 0x406750c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][68], 0x11); /* 0x4067510 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][69], 0x11); /* 0x4067514 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][70], 0x11); /* 0x4067518 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][71], 0x11); /* 0x406751c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][72], 0x12); /* 0x4067520 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][73], 0x12); /* 0x4067524 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][74], 0x12); /* 0x4067528 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][75], 0x12); /* 0x406752c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][76], 0x13); /* 0x4067530 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][77], 0x13); /* 0x4067534 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][78], 0x13); /* 0x4067538 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][79], 0x13); /* 0x406753c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][80], 0x14); /* 0x4067540 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][81], 0x14); /* 0x4067544 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][82], 0x14); /* 0x4067548 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][83], 0x14); /* 0x406754c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][84], 0x15); /* 0x4067550 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][85], 0x15); /* 0x4067554 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][86], 0x15); /* 0x4067558 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][87], 0x15); /* 0x406755c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][88], 0x16); /* 0x4067560 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][89], 0x16); /* 0x4067564 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][90], 0x16); /* 0x4067568 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][91], 0x16); /* 0x406756c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][92], 0x17); /* 0x4067570 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][93], 0x17); /* 0x4067574 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][94], 0x17); /* 0x4067578 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][95], 0x17); /* 0x406757c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][96], 0x18); /* 0x4067580 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][97], 0x18); /* 0x4067584 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][98], 0x18); /* 0x4067588 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][99], 0x18); /* 0x406758c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][100], 0x19); /* 0x4067590 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][101], 0x19); /* 0x4067594 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][102], 0x19); /* 0x4067598 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][103], 0x19); /* 0x406759c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][104], 0x1a); /* 0x40675a0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][105], 0x1a); /* 0x40675a4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][106], 0x1a); /* 0x40675a8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][107], 0x1a); /* 0x40675ac */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][108], 0x1b); /* 0x40675b0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][109], 0x1b); /* 0x40675b4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][110], 0x1b); /* 0x40675b8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][111], 0x1b); /* 0x40675bc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][112], 0x1c); /* 0x40675c0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][113], 0x1c); /* 0x40675c4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][114], 0x1c); /* 0x40675c8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][115], 0x1c); /* 0x40675cc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][116], 0x1d); /* 0x40675d0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][117], 0x1d); /* 0x40675d4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][118], 0x1d); /* 0x40675d8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][119], 0x1d); /* 0x40675dc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][120], 0x1e); /* 0x40675e0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][121], 0x1e); /* 0x40675e4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][122], 0x1e); /* 0x40675e8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][123], 0x1e); /* 0x40675ec */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][124], 0x1f); /* 0x40675f0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][125], 0x1f); /* 0x40675f4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][126], 0x1f); /* 0x40675f8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][127], 0x1f); /* 0x40675fc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][128], 0x10); /* 0x4067a00 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][129], 0x10); /* 0x4067a04 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][130], 0x10); /* 0x4067a08 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][131], 0x10); /* 0x4067a0c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][132], 0x11); /* 0x4067a10 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][133], 0x11); /* 0x4067a14 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][134], 0x11); /* 0x4067a18 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][135], 0x11); /* 0x4067a1c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][136], 0x12); /* 0x4067a20 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][137], 0x12); /* 0x4067a24 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][138], 0x12); /* 0x4067a28 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][139], 0x12); /* 0x4067a2c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][140], 0x13); /* 0x4067a30 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][141], 0x13); /* 0x4067a34 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][142], 0x13); /* 0x4067a38 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][143], 0x13); /* 0x4067a3c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][144], 0x14); /* 0x4067a40 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][145], 0x14); /* 0x4067a44 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][146], 0x14); /* 0x4067a48 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][147], 0x14); /* 0x4067a4c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][148], 0x15); /* 0x4067a50 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][149], 0x15); /* 0x4067a54 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][150], 0x15); /* 0x4067a58 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][151], 0x15); /* 0x4067a5c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][152], 0x16); /* 0x4067a60 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][153], 0x16); /* 0x4067a64 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][154], 0x16); /* 0x4067a68 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][155], 0x16); /* 0x4067a6c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][156], 0x17); /* 0x4067a70 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][157], 0x17); /* 0x4067a74 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][158], 0x17); /* 0x4067a78 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][159], 0x17); /* 0x4067a7c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][160], 0x18); /* 0x4067a80 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][161], 0x18); /* 0x4067a84 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][162], 0x18); /* 0x4067a88 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][163], 0x18); /* 0x4067a8c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][164], 0x19); /* 0x4067a90 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][165], 0x19); /* 0x4067a94 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][166], 0x19); /* 0x4067a98 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][167], 0x19); /* 0x4067a9c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][168], 0x1a); /* 0x4067aa0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][169], 0x1a); /* 0x4067aa4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][170], 0x1a); /* 0x4067aa8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][171], 0x1a); /* 0x4067aac */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][172], 0x1b); /* 0x4067ab0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][173], 0x1b); /* 0x4067ab4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][174], 0x1b); /* 0x4067ab8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][175], 0x1b); /* 0x4067abc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][176], 0x1c); /* 0x4067ac0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][177], 0x1c); /* 0x4067ac4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][178], 0x1c); /* 0x4067ac8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][179], 0x1c); /* 0x4067acc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][180], 0x1d); /* 0x4067ad0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][181], 0x1d); /* 0x4067ad4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][182], 0x1d); /* 0x4067ad8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][183], 0x1d); /* 0x4067adc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][184], 0x1e); /* 0x4067ae0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][185], 0x1e); /* 0x4067ae4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][186], 0x1e); /* 0x4067ae8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][187], 0x1e); /* 0x4067aec */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][188], 0x1f); /* 0x4067af0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][189], 0x1f); /* 0x4067af4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][190], 0x1f); /* 0x4067af8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][191], 0x1f); /* 0x4067afc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][192], 0x10); /* 0x4067f00 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][193], 0x10); /* 0x4067f04 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][194], 0x10); /* 0x4067f08 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][195], 0x10); /* 0x4067f0c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][196], 0x11); /* 0x4067f10 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][197], 0x11); /* 0x4067f14 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][198], 0x11); /* 0x4067f18 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][199], 0x11); /* 0x4067f1c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][200], 0x12); /* 0x4067f20 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][201], 0x12); /* 0x4067f24 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][202], 0x12); /* 0x4067f28 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][203], 0x12); /* 0x4067f2c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][204], 0x13); /* 0x4067f30 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][205], 0x13); /* 0x4067f34 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][206], 0x13); /* 0x4067f38 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][207], 0x13); /* 0x4067f3c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][208], 0x14); /* 0x4067f40 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][209], 0x14); /* 0x4067f44 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][210], 0x14); /* 0x4067f48 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][211], 0x14); /* 0x4067f4c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][212], 0x15); /* 0x4067f50 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][213], 0x15); /* 0x4067f54 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][214], 0x15); /* 0x4067f58 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][215], 0x15); /* 0x4067f5c */
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
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_mode[0], 0x3695000); /* 0x40406c0 */
    tu.OutWord(&mau_reg_map.tcams.col[0].tcam_table_map[0], 0x1); /* 0x4040680 */
    tu.OutWord(&mau_reg_map.tcams.tcam_match_adr_shift[0], 0x4); /* 0x4040420 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.unitram_config[0][5], 0x186e); /* 0x401d3a8 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.unitram_config[0][5], 0x2586e); //AJEstats
    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.unitram_config[1][2], 0x1952); /* 0x401f3d0 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.unitram_config[1][2], 0x25952); //AJEstats
    tu.OutWord(&mau_reg_map.rams.array.row[2].ram[5].unit_ram_ctl, 0x200); /* 0x400a298 */
    tu.OutWord(&mau_reg_map.rams.array.row[6].ram[8].unit_ram_ctl, 0x20); /* 0x400e418 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tind_ram_data_size[4], 0x5); /* 0x4016650 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][0], 0x14); /* 0x4040380 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][0], 0x3); /* 0x4040200 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][1], 0xa); /* 0x4040384 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][1], 0x2); /* 0x4040204 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][0], 0x3); /* 0x4040300 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][0], 0x1); /* 0x4040000 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][1], 0x8); /* 0x4040004 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][2], 0x6); /* 0x4040008 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][3], 0x8); /* 0x404000c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][4], 0x8); /* 0x4040010 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][5], 0x9); /* 0x4040014 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][6], 0x3); /* 0x4040018 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][7], 0x6); /* 0x404001c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][0], 0x10); /* 0x40403c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][0], 0x1); /* 0x4040240 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][1], 0x4); /* 0x40403c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][1], 0x1); /* 0x4040244 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][0], 0x6); /* 0x4040320 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][0], 0x1); /* 0x4040100 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][1], 0x3); /* 0x4040104 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][2], 0x8); /* 0x4040108 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][3], 0x3); /* 0x404010c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][4], 0x7); /* 0x4040110 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][5], 0x0); /* 0x4040114 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][6], 0x9); /* 0x4040118 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][7], 0x0); /* 0x404011c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][2], 0x1c); /* 0x4040388 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][2], 0x2); /* 0x4040208 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][3], 0x1a); /* 0x404038c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][3], 0x0); /* 0x404020c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][1], 0x3); /* 0x4040304 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][0], 0x0); /* 0x4040020 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][1], 0xa); /* 0x4040024 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][2], 0x8); /* 0x4040028 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][3], 0x9); /* 0x404002c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][4], 0x6); /* 0x4040030 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][5], 0x5); /* 0x4040034 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][6], 0x6); /* 0x4040038 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][7], 0xa); /* 0x404003c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][2], 0x1); /* 0x40403c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][2], 0x1); /* 0x4040248 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][3], 0x15); /* 0x40403cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][3], 0x0); /* 0x404024c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][1], 0x3); /* 0x4040324 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][0], 0x8); /* 0x4040120 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][1], 0x2); /* 0x4040124 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][2], 0x6); /* 0x4040128 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][3], 0x6); /* 0x404012c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][4], 0x2); /* 0x4040130 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][5], 0x1); /* 0x4040134 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][6], 0x0); /* 0x4040138 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][7], 0x9); /* 0x404013c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][4], 0x17); /* 0x4040390 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][4], 0x3); /* 0x4040210 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][5], 0x13); /* 0x4040394 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][5], 0x0); /* 0x4040214 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][2], 0x2); /* 0x4040308 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][0], 0x9); /* 0x4040040 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][1], 0x3); /* 0x4040044 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][2], 0x7); /* 0x4040048 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][3], 0x8); /* 0x404004c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][4], 0x7); /* 0x4040050 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][5], 0x1); /* 0x4040054 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][6], 0x5); /* 0x4040058 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][7], 0x8); /* 0x404005c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][4], 0x5); /* 0x40403d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][4], 0x3); /* 0x4040250 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][5], 0x5); /* 0x40403d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][5], 0x0); /* 0x4040254 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][2], 0x5); /* 0x4040328 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][0], 0x4); /* 0x4040140 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][1], 0x2); /* 0x4040144 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][2], 0x0); /* 0x4040148 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][3], 0x9); /* 0x404014c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][4], 0x8); /* 0x4040150 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][5], 0x2); /* 0x4040154 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][6], 0x2); /* 0x4040158 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][7], 0x9); /* 0x404015c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][6], 0x1c); /* 0x4040398 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][6], 0x1); /* 0x4040218 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][7], 0x1); /* 0x404039c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][7], 0x1); /* 0x404021c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][3], 0xc); /* 0x404030c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][0], 0x5); /* 0x4040060 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][1], 0x4); /* 0x4040064 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][2], 0x9); /* 0x4040068 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][3], 0x8); /* 0x404006c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][4], 0x9); /* 0x4040070 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][5], 0x7); /* 0x4040074 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][6], 0x4); /* 0x4040078 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][7], 0x2); /* 0x404007c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][6], 0x6); /* 0x40403d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][6], 0x0); /* 0x4040258 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][7], 0xc); /* 0x40403dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][7], 0x2); /* 0x404025c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][3], 0x0); /* 0x404032c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][0], 0x5); /* 0x4040160 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][1], 0x8); /* 0x4040164 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][2], 0x9); /* 0x4040168 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][3], 0x2); /* 0x404016c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][4], 0x8); /* 0x4040170 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][5], 0x5); /* 0x4040174 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][6], 0x6); /* 0x4040178 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][7], 0x8); /* 0x404017c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][8], 0x16); /* 0x40403a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][8], 0x0); /* 0x4040220 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][9], 0x9); /* 0x40403a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][9], 0x3); /* 0x4040224 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][4], 0x3); /* 0x4040310 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][0], 0x1); /* 0x4040080 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][1], 0x0); /* 0x4040084 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][2], 0x7); /* 0x4040088 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][3], 0x2); /* 0x404008c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][4], 0xa); /* 0x4040090 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][5], 0xa); /* 0x4040094 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][6], 0x0); /* 0x4040098 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][7], 0xa); /* 0x404009c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][8], 0x18); /* 0x40403e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][8], 0x1); /* 0x4040260 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][9], 0x18); /* 0x40403e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][9], 0x2); /* 0x4040264 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][4], 0x2); /* 0x4040330 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][0], 0x0); /* 0x4040180 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][1], 0x2); /* 0x4040184 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][2], 0x7); /* 0x4040188 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][3], 0x7); /* 0x404018c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][4], 0x5); /* 0x4040190 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][5], 0x7); /* 0x4040194 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][6], 0x9); /* 0x4040198 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][7], 0x5); /* 0x404019c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][10], 0x18); /* 0x40403a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][10], 0x0); /* 0x4040228 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][11], 0x2); /* 0x40403ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][11], 0x0); /* 0x404022c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][5], 0x0); /* 0x4040314 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][0], 0x2); /* 0x40400a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][1], 0x9); /* 0x40400a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][2], 0x1); /* 0x40400a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][3], 0x3); /* 0x40400ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][4], 0x8); /* 0x40400b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][5], 0x1); /* 0x40400b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][6], 0x9); /* 0x40400b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][7], 0x6); /* 0x40400bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][10], 0x16); /* 0x40403e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][10], 0x1); /* 0x4040268 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][11], 0xd); /* 0x40403ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][11], 0x2); /* 0x404026c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][5], 0x2); /* 0x4040334 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][0], 0x3); /* 0x40401a0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][1], 0x5); /* 0x40401a4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][2], 0x4); /* 0x40401a8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][3], 0x1); /* 0x40401ac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][4], 0x2); /* 0x40401b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][5], 0x9); /* 0x40401b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][6], 0x3); /* 0x40401b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][7], 0x4); /* 0x40401bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][12], 0x15); /* 0x40403b0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][12], 0x1); /* 0x4040230 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][13], 0x12); /* 0x40403b4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][13], 0x3); /* 0x4040234 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][6], 0x5); /* 0x4040318 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][0], 0x5); /* 0x40400c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][1], 0x0); /* 0x40400c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][2], 0x1); /* 0x40400c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][3], 0x5); /* 0x40400cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][4], 0x8); /* 0x40400d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][5], 0x1); /* 0x40400d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][6], 0x0); /* 0x40400d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][7], 0x5); /* 0x40400dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][12], 0x8); /* 0x40403f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][12], 0x1); /* 0x4040270 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][13], 0xf); /* 0x40403f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][13], 0x2); /* 0x4040274 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][6], 0x4); /* 0x4040338 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][0], 0x5); /* 0x40401c0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][1], 0x4); /* 0x40401c4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][2], 0x9); /* 0x40401c8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][3], 0x1); /* 0x40401cc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][4], 0x0); /* 0x40401d0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][5], 0x5); /* 0x40401d4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][6], 0x5); /* 0x40401d8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][7], 0x7); /* 0x40401dc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][14], 0xf); /* 0x40403b8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][14], 0x0); /* 0x4040238 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][15], 0x6); /* 0x40403bc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][15], 0x1); /* 0x404023c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][7], 0x9); /* 0x404031c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][0], 0xa); /* 0x40400e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][1], 0x5); /* 0x40400e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][2], 0x5); /* 0x40400e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][3], 0xa); /* 0x40400ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][4], 0x0); /* 0x40400f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][5], 0x3); /* 0x40400f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][6], 0x8); /* 0x40400f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][7], 0x6); /* 0x40400fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][14], 0xd); /* 0x40403f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][14], 0x0); /* 0x4040278 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][15], 0x1e); /* 0x40403fc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][15], 0x0); /* 0x404027c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][7], 0x0); /* 0x404033c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][0], 0xa); /* 0x40401e0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][1], 0x4); /* 0x40401e4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][2], 0x9); /* 0x40401e8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][3], 0x8); /* 0x40401ec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][4], 0x3); /* 0x40401f0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][5], 0x9); /* 0x40401f4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][6], 0x6); /* 0x40401f8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][7], 0x0); /* 0x40401fc */
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_hit_to_logical_table_ixbar_outputmap[0], 0x16); /* 0x4014a60 */
    tu.OutWord(&mau_reg_map.rams.match.merge.tcam_match_adr_to_physical_oxbar_outputmap[4], 0x8); /* 0x4016610 */
    tu.OutWord(&mau_reg_map.rams.match.merge.match_to_logical_table_ixbar_outputmap[1][4], 0x16); /* 0x4014cd0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_tcam_shiftcount[4], 0x8); /* 0x4016750 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_mask[1][4], 0x3f); /* 0x4014f50 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_tcam_shiftcount[4], 0x0); /* 0x40166d0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_mask[1][4], 0xffffffff); /* 0x4014dd0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.next_table_format_data[6], 0x100); /* 0x4016698 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_tcam_shiftcount[4], 0x2f); /* 0x40167d0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_mask[1][4], 0x3fffff); /* 0x4016050 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.immediate_data_32b_ixbar_ctl[6], 0x20); /* 0x4010398 */
    tu.OutWord(&mau_reg_map.rams.match.merge.predication_ctl[0], 0x40); /* 0x4014c00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][0], RM_B4_32(0x1023e800)); /* 0x407c000 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][1], RM_B4_32(0x1023e800)); /* 0x407c004 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][2], RM_B4_32(0x1023e800)); /* 0x407c008 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][3], RM_B4_32(0x1023e800)); /* 0x407c00c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][4], RM_B4_32(0x1023e800)); /* 0x407c010 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][5], RM_B4_32(0x1023e800)); /* 0x407c014 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][6], RM_B4_32(0x1023e800)); /* 0x407c018 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][7], RM_B4_32(0x1023e800)); /* 0x407c01c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][8], RM_B4_32(0x1023e800)); /* 0x407c020 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][9], RM_B4_32(0x1023e800)); /* 0x407c024 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][10], RM_B4_32(0x1023e800)); /* 0x407c028 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][11], RM_B4_32(0x1023e800)); /* 0x407c02c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][12], RM_B4_32(0x1023e800)); /* 0x407c030 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][13], RM_B4_32(0x1023e800)); /* 0x407c034 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][14], RM_B4_32(0x1023e800)); /* 0x407c038 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][15], RM_B4_32(0x1023e800)); /* 0x407c03c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][16], RM_B4_32(0x1023e800)); /* 0x407c040 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][17], RM_B4_32(0x1023e800)); /* 0x407c044 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][18], RM_B4_32(0x1023e800)); /* 0x407c048 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][19], RM_B4_32(0x1023e800)); /* 0x407c04c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][20], RM_B4_32(0x1023e800)); /* 0x407c050 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][21], RM_B4_32(0x1023e800)); /* 0x407c054 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][22], RM_B4_32(0x1023e800)); /* 0x407c058 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][23], RM_B4_32(0x1023e800)); /* 0x407c05c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][24], RM_B4_32(0x1023e800)); /* 0x407c060 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][25], RM_B4_32(0x1023e800)); /* 0x407c064 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][26], RM_B4_32(0x1023e800)); /* 0x407c068 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][27], RM_B4_32(0x1023e800)); /* 0x407c06c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][28], RM_B4_32(0x1023e800)); /* 0x407c070 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][29], RM_B4_32(0x1023e800)); /* 0x407c074 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][30], RM_B4_32(0x1023e800)); /* 0x407c078 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][31], RM_B4_32(0x1023e800)); /* 0x407c07c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][0], RM_B4_32(0x1023e800)); /* 0x407c080 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][1], RM_B4_32(0x1023e800)); /* 0x407c084 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][2], RM_B4_32(0x1023e800)); /* 0x407c088 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][3], RM_B4_32(0x1023e800)); /* 0x407c08c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][4], RM_B4_32(0x1023e800)); /* 0x407c090 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][5], RM_B4_32(0x1023e800)); /* 0x407c094 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][6], RM_B4_32(0x1023e800)); /* 0x407c098 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][7], RM_B4_32(0x1023e800)); /* 0x407c09c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][8], RM_B4_32(0x1023e800)); /* 0x407c0a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][9], RM_B4_32(0x1023e800)); /* 0x407c0a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][10], RM_B4_32(0x1023e800)); /* 0x407c0a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][11], RM_B4_32(0x1023e800)); /* 0x407c0ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][12], RM_B4_32(0x1023e800)); /* 0x407c0b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][13], RM_B4_32(0x1023e800)); /* 0x407c0b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][14], RM_B4_32(0x1023e800)); /* 0x407c0b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][15], RM_B4_32(0x1023e800)); /* 0x407c0bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][16], RM_B4_32(0x1023e800)); /* 0x407c0c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][17], RM_B4_32(0x1023e800)); /* 0x407c0c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][18], RM_B4_32(0x1023e800)); /* 0x407c0c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][19], RM_B4_32(0x1023e800)); /* 0x407c0cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][20], RM_B4_32(0x1023e800)); /* 0x407c0d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][21], RM_B4_32(0x1023e800)); /* 0x407c0d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][22], RM_B4_32(0x1023e800)); /* 0x407c0d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][23], RM_B4_32(0x1023e800)); /* 0x407c0dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][24], RM_B4_32(0x1023e800)); /* 0x407c0e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][25], RM_B4_32(0x1023e800)); /* 0x407c0e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][26], RM_B4_32(0x1023e800)); /* 0x407c0e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][27], RM_B4_32(0x1023e800)); /* 0x407c0ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][28], RM_B4_32(0x1023e800)); /* 0x407c0f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][29], RM_B4_32(0x1023e800)); /* 0x407c0f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][30], RM_B4_32(0x1023e800)); /* 0x407c0f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][31], RM_B4_32(0x1023e800)); /* 0x407c0fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][0], RM_B4_32(0x1023e800)); /* 0x407c100 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][1], RM_B4_32(0x1023e800)); /* 0x407c104 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][2], RM_B4_32(0x1023e800)); /* 0x407c108 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][3], RM_B4_32(0x1023e800)); /* 0x407c10c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][4], RM_B4_32(0x1023e800)); /* 0x407c110 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][5], RM_B4_32(0x1023e800)); /* 0x407c114 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][6], RM_B4_32(0x1023e800)); /* 0x407c118 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][7], RM_B4_32(0x1023e800)); /* 0x407c11c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][8], RM_B4_32(0x1023e800)); /* 0x407c120 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][9], RM_B4_32(0x1023e800)); /* 0x407c124 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][10], RM_B4_32(0x1023e800)); /* 0x407c128 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][11], RM_B4_32(0x1023e800)); /* 0x407c12c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][12], RM_B4_32(0x1023e800)); /* 0x407c130 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][13], RM_B4_32(0x1023e800)); /* 0x407c134 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][14], RM_B4_32(0x1023e800)); /* 0x407c138 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][15], RM_B4_32(0x1023e800)); /* 0x407c13c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][16], RM_B4_32(0x1023e800)); /* 0x407c140 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][17], RM_B4_32(0x1023e800)); /* 0x407c144 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][18], RM_B4_32(0x1023e800)); /* 0x407c148 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][19], RM_B4_32(0x1023e800)); /* 0x407c14c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][20], RM_B4_32(0x1023e800)); /* 0x407c150 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][21], RM_B4_32(0x1023e800)); /* 0x407c154 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][22], RM_B4_32(0x1023e800)); /* 0x407c158 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][23], RM_B4_32(0x1023e800)); /* 0x407c15c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][24], RM_B4_32(0x1023e800)); /* 0x407c160 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][25], RM_B4_32(0x1023e800)); /* 0x407c164 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][26], RM_B4_32(0x1023e800)); /* 0x407c168 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][27], RM_B4_32(0x1023e800)); /* 0x407c16c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][28], RM_B4_32(0x1023e800)); /* 0x407c170 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][29], RM_B4_32(0x1023e800)); /* 0x407c174 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][30], RM_B4_32(0x1023e800)); /* 0x407c178 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][31], RM_B4_32(0x1023e800)); /* 0x407c17c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][0], RM_B4_32(0x1023e800)); /* 0x407c180 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][1], RM_B4_32(0x1023e800)); /* 0x407c184 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][2], RM_B4_32(0x1023e800)); /* 0x407c188 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][3], RM_B4_32(0x1023e800)); /* 0x407c18c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][4], RM_B4_32(0x1023e800)); /* 0x407c190 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][5], RM_B4_32(0x1023e800)); /* 0x407c194 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][6], RM_B4_32(0x1023e800)); /* 0x407c198 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][7], RM_B4_32(0x1023e800)); /* 0x407c19c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][8], RM_B4_32(0x1023e800)); /* 0x407c1a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][9], RM_B4_32(0x1023e800)); /* 0x407c1a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][10], RM_B4_32(0x1023e800)); /* 0x407c1a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][11], RM_B4_32(0x1023e800)); /* 0x407c1ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][12], RM_B4_32(0x1023e800)); /* 0x407c1b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][13], RM_B4_32(0x1023e800)); /* 0x407c1b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][14], RM_B4_32(0x1023e800)); /* 0x407c1b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][15], RM_B4_32(0x1023e800)); /* 0x407c1bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][16], RM_B4_32(0x1023e800)); /* 0x407c1c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][17], RM_B4_32(0x1023e800)); /* 0x407c1c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][18], RM_B4_32(0x1023e800)); /* 0x407c1c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][19], RM_B4_32(0x1023e800)); /* 0x407c1cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][20], RM_B4_32(0x1023e800)); /* 0x407c1d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][21], RM_B4_32(0x1023e800)); /* 0x407c1d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][22], RM_B4_32(0x1023e800)); /* 0x407c1d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][23], RM_B4_32(0x1023e800)); /* 0x407c1dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][24], RM_B4_32(0x1023e800)); /* 0x407c1e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][25], RM_B4_32(0x1023e800)); /* 0x407c1e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][26], RM_B4_32(0x1023e800)); /* 0x407c1e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][27], RM_B4_32(0x1023e800)); /* 0x407c1ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][28], RM_B4_32(0x1023e800)); /* 0x407c1f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][29], RM_B4_32(0x1023e800)); /* 0x407c1f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][30], RM_B4_32(0x1023e800)); /* 0x407c1f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][31], RM_B4_32(0x1023e800)); /* 0x407c1fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][0], RM_B4_32(0x1023e800)); /* 0x407c200 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][1], RM_B4_32(0x1023e800)); /* 0x407c204 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][2], RM_B4_32(0x1023e800)); /* 0x407c208 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][3], RM_B4_32(0x1023e800)); /* 0x407c20c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][4], RM_B4_32(0x1023e800)); /* 0x407c210 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][5], RM_B4_32(0x1023e800)); /* 0x407c214 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][6], RM_B4_32(0x1023e800)); /* 0x407c218 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][7], RM_B4_32(0x1023e800)); /* 0x407c21c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][8], RM_B4_32(0x1023e800)); /* 0x407c220 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][9], RM_B4_32(0x1023e800)); /* 0x407c224 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][10], RM_B4_32(0x1023e800)); /* 0x407c228 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][11], RM_B4_32(0x1023e800)); /* 0x407c22c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][12], RM_B4_32(0x1023e800)); /* 0x407c230 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][13], RM_B4_32(0x1023e800)); /* 0x407c234 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][14], RM_B4_32(0x1023e800)); /* 0x407c238 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][15], RM_B4_32(0x1023e800)); /* 0x407c23c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][16], RM_B4_32(0x1023e800)); /* 0x407c240 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][17], RM_B4_32(0x1023e800)); /* 0x407c244 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][18], RM_B4_32(0x1023e800)); /* 0x407c248 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][19], RM_B4_32(0x1023e800)); /* 0x407c24c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][20], RM_B4_32(0x1023e800)); /* 0x407c250 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][21], RM_B4_32(0x1023e800)); /* 0x407c254 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][22], RM_B4_32(0x1023e800)); /* 0x407c258 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][23], RM_B4_32(0x1023e800)); /* 0x407c25c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][24], RM_B4_32(0x1023e800)); /* 0x407c260 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][25], RM_B4_32(0x1023e800)); /* 0x407c264 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][26], RM_B4_32(0x1023e800)); /* 0x407c268 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][27], RM_B4_32(0x1023e800)); /* 0x407c26c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][28], RM_B4_32(0x1023e800)); /* 0x407c270 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][29], RM_B4_32(0x1023e800)); /* 0x407c274 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][30], RM_B4_32(0x1023e800)); /* 0x407c278 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][31], RM_B4_32(0x1023e800)); /* 0x407c27c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][0], RM_B4_32(0x1023e800)); /* 0x407c280 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][1], RM_B4_32(0x1023e800)); /* 0x407c284 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][2], RM_B4_32(0x1023e800)); /* 0x407c288 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][3], RM_B4_32(0x1023e800)); /* 0x407c28c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][4], RM_B4_32(0x1023e800)); /* 0x407c290 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][5], RM_B4_32(0x1023e800)); /* 0x407c294 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][6], RM_B4_32(0x1023e800)); /* 0x407c298 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][7], RM_B4_32(0x1023e800)); /* 0x407c29c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][8], RM_B4_32(0x1023e800)); /* 0x407c2a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][9], RM_B4_32(0x1023e800)); /* 0x407c2a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][10], RM_B4_32(0x1023e800)); /* 0x407c2a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][11], RM_B4_32(0x1023e800)); /* 0x407c2ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][12], RM_B4_32(0x1023e800)); /* 0x407c2b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][13], RM_B4_32(0x1023e800)); /* 0x407c2b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][14], RM_B4_32(0x1023e800)); /* 0x407c2b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][15], RM_B4_32(0x1023e800)); /* 0x407c2bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][16], RM_B4_32(0x1023e800)); /* 0x407c2c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][17], RM_B4_32(0x1023e800)); /* 0x407c2c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][18], RM_B4_32(0x1023e800)); /* 0x407c2c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][19], RM_B4_32(0x1023e800)); /* 0x407c2cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][20], RM_B4_32(0x1023e800)); /* 0x407c2d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][21], RM_B4_32(0x1023e800)); /* 0x407c2d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][22], RM_B4_32(0x1023e800)); /* 0x407c2d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][23], RM_B4_32(0x1023e800)); /* 0x407c2dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][24], RM_B4_32(0x1023e800)); /* 0x407c2e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][25], RM_B4_32(0x1023e800)); /* 0x407c2e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][26], RM_B4_32(0x1023e800)); /* 0x407c2e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][27], RM_B4_32(0x1023e800)); /* 0x407c2ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][28], RM_B4_32(0x1023e800)); /* 0x407c2f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][29], RM_B4_32(0x1023e800)); /* 0x407c2f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][30], RM_B4_32(0x1023e800)); /* 0x407c2f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][31], RM_B4_32(0x1023e800)); /* 0x407c2fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][0], RM_B4_32(0x1023e800)); /* 0x407c300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][1], RM_B4_32(0x1023e800)); /* 0x407c304 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][2], RM_B4_32(0x1023e800)); /* 0x407c308 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][3], RM_B4_32(0x1023e800)); /* 0x407c30c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][4], RM_B4_32(0x1023e800)); /* 0x407c310 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][5], RM_B4_32(0x1023e800)); /* 0x407c314 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][6], RM_B4_32(0x1023e800)); /* 0x407c318 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][7], RM_B4_32(0x1023e800)); /* 0x407c31c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][8], RM_B4_32(0x1023e800)); /* 0x407c320 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][9], RM_B4_32(0x1023e800)); /* 0x407c324 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][10], RM_B4_32(0x1023e800)); /* 0x407c328 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][11], RM_B4_32(0x1023e800)); /* 0x407c32c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][12], RM_B4_32(0x1023e800)); /* 0x407c330 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][13], RM_B4_32(0x1023e800)); /* 0x407c334 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][14], RM_B4_32(0x1023e800)); /* 0x407c338 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][15], RM_B4_32(0x1023e800)); /* 0x407c33c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][16], RM_B4_32(0x1023e800)); /* 0x407c340 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][17], RM_B4_32(0x1023e800)); /* 0x407c344 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][18], RM_B4_32(0x1023e800)); /* 0x407c348 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][19], RM_B4_32(0x1023e800)); /* 0x407c34c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][20], RM_B4_32(0x1023e800)); /* 0x407c350 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][21], RM_B4_32(0x1023e800)); /* 0x407c354 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][22], RM_B4_32(0x1023e800)); /* 0x407c358 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][23], RM_B4_32(0x1023e800)); /* 0x407c35c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][24], RM_B4_32(0x1023e800)); /* 0x407c360 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][25], RM_B4_32(0x1023e800)); /* 0x407c364 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][26], RM_B4_32(0x1023e800)); /* 0x407c368 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][27], RM_B4_32(0x1023e800)); /* 0x407c36c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][28], RM_B4_32(0x1023e800)); /* 0x407c370 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][29], RM_B4_32(0x1023e800)); /* 0x407c374 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][30], RM_B4_32(0x1023e800)); /* 0x407c378 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][31], RM_B4_32(0x1023e800)); /* 0x407c37c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][0], RM_B4_32(0x1023e800)); /* 0x407c380 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][1], RM_B4_32(0x1023e800)); /* 0x407c384 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][2], RM_B4_32(0x1023e800)); /* 0x407c388 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][3], RM_B4_32(0x1023e800)); /* 0x407c38c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][4], RM_B4_32(0x1023e800)); /* 0x407c390 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][5], RM_B4_32(0x1023e800)); /* 0x407c394 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][6], RM_B4_32(0x1023e800)); /* 0x407c398 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][7], RM_B4_32(0x1023e800)); /* 0x407c39c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][8], RM_B4_32(0x1023e800)); /* 0x407c3a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][9], RM_B4_32(0x1023e800)); /* 0x407c3a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][10], RM_B4_32(0x1023e800)); /* 0x407c3a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][11], RM_B4_32(0x1023e800)); /* 0x407c3ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][12], RM_B4_32(0x1023e800)); /* 0x407c3b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][13], RM_B4_32(0x1023e800)); /* 0x407c3b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][14], RM_B4_32(0x1023e800)); /* 0x407c3b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][15], RM_B4_32(0x1023e800)); /* 0x407c3bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][16], RM_B4_32(0x1023e800)); /* 0x407c3c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][17], RM_B4_32(0x1023e800)); /* 0x407c3c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][18], RM_B4_32(0x1023e800)); /* 0x407c3c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][19], RM_B4_32(0x1023e800)); /* 0x407c3cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][20], RM_B4_32(0x1023e800)); /* 0x407c3d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][21], RM_B4_32(0x1023e800)); /* 0x407c3d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][22], RM_B4_32(0x1023e800)); /* 0x407c3d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][23], RM_B4_32(0x1023e800)); /* 0x407c3dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][24], RM_B4_32(0x1023e800)); /* 0x407c3e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][25], RM_B4_32(0x1023e800)); /* 0x407c3e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][26], RM_B4_32(0x1023e800)); /* 0x407c3e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][27], RM_B4_32(0x1023e800)); /* 0x407c3ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][28], RM_B4_32(0x1023e800)); /* 0x407c3f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][29], RM_B4_32(0x1023e800)); /* 0x407c3f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][30], RM_B4_32(0x1023e800)); /* 0x407c3f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][31], RM_B4_32(0x1023e800)); /* 0x407c3fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][0], RM_B4_32(0x1023e800)); /* 0x407c400 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][1], RM_B4_32(0x1023e800)); /* 0x407c404 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][2], RM_B4_32(0x1023e800)); /* 0x407c408 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][3], RM_B4_32(0x1023e800)); /* 0x407c40c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][4], RM_B4_32(0x1023e800)); /* 0x407c410 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][5], RM_B4_32(0x1023e800)); /* 0x407c414 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][6], RM_B4_32(0x1023e800)); /* 0x407c418 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][7], RM_B4_32(0x1023e800)); /* 0x407c41c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][8], RM_B4_32(0x1023e800)); /* 0x407c420 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][9], RM_B4_32(0x1023e800)); /* 0x407c424 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][10], RM_B4_32(0x1023e800)); /* 0x407c428 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][11], RM_B4_32(0x1023e800)); /* 0x407c42c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][12], RM_B4_32(0x1023e800)); /* 0x407c430 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][13], RM_B4_32(0x1023e800)); /* 0x407c434 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][14], RM_B4_32(0x1023e800)); /* 0x407c438 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][15], RM_B4_32(0x1023e800)); /* 0x407c43c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][16], RM_B4_32(0x1023e800)); /* 0x407c440 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][17], RM_B4_32(0x1023e800)); /* 0x407c444 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][18], RM_B4_32(0x1023e800)); /* 0x407c448 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][19], RM_B4_32(0x1023e800)); /* 0x407c44c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][20], RM_B4_32(0x1023e800)); /* 0x407c450 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][21], RM_B4_32(0x1023e800)); /* 0x407c454 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][22], RM_B4_32(0x1023e800)); /* 0x407c458 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][23], RM_B4_32(0x1023e800)); /* 0x407c45c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][24], RM_B4_32(0x1023e800)); /* 0x407c460 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][25], RM_B4_32(0x1023e800)); /* 0x407c464 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][26], RM_B4_32(0x1023e800)); /* 0x407c468 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][27], RM_B4_32(0x1023e800)); /* 0x407c46c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][28], RM_B4_32(0x1023e800)); /* 0x407c470 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][29], RM_B4_32(0x1023e800)); /* 0x407c474 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][30], RM_B4_32(0x1023e800)); /* 0x407c478 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][31], RM_B4_32(0x1023e800)); /* 0x407c47c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][0], RM_B4_32(0x1023e800)); /* 0x407c480 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][1], RM_B4_32(0x1023e800)); /* 0x407c484 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][2], RM_B4_32(0x1023e800)); /* 0x407c488 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][3], RM_B4_32(0x1023e800)); /* 0x407c48c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][4], RM_B4_32(0x1023e800)); /* 0x407c490 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][5], RM_B4_32(0x1023e800)); /* 0x407c494 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][6], RM_B4_32(0x1023e800)); /* 0x407c498 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][7], RM_B4_32(0x1023e800)); /* 0x407c49c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][8], RM_B4_32(0x1023e800)); /* 0x407c4a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][9], RM_B4_32(0x1023e800)); /* 0x407c4a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][10], RM_B4_32(0x1023e800)); /* 0x407c4a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][11], RM_B4_32(0x1023e800)); /* 0x407c4ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][12], RM_B4_32(0x1023e800)); /* 0x407c4b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][13], RM_B4_32(0x1023e800)); /* 0x407c4b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][14], RM_B4_32(0x1023e800)); /* 0x407c4b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][15], RM_B4_32(0x1023e800)); /* 0x407c4bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][16], RM_B4_32(0x1023e800)); /* 0x407c4c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][17], RM_B4_32(0x1023e800)); /* 0x407c4c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][18], RM_B4_32(0x1023e800)); /* 0x407c4c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][19], RM_B4_32(0x1023e800)); /* 0x407c4cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][20], RM_B4_32(0x1023e800)); /* 0x407c4d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][21], RM_B4_32(0x1023e800)); /* 0x407c4d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][22], RM_B4_32(0x1023e800)); /* 0x407c4d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][23], RM_B4_32(0x1023e800)); /* 0x407c4dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][24], RM_B4_32(0x1023e800)); /* 0x407c4e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][25], RM_B4_32(0x1023e800)); /* 0x407c4e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][26], RM_B4_32(0x1023e800)); /* 0x407c4e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][27], RM_B4_32(0x1023e800)); /* 0x407c4ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][28], RM_B4_32(0x1023e800)); /* 0x407c4f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][29], RM_B4_32(0x1023e800)); /* 0x407c4f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][30], RM_B4_32(0x1023e800)); /* 0x407c4f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][31], RM_B4_32(0x1023e800)); /* 0x407c4fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][0], RM_B4_32(0x1023e800)); /* 0x407c500 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][1], RM_B4_32(0x1023e800)); /* 0x407c504 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][2], RM_B4_32(0x1023e800)); /* 0x407c508 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][3], RM_B4_32(0x1023e800)); /* 0x407c50c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][4], RM_B4_32(0x1023e800)); /* 0x407c510 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][5], RM_B4_32(0x1023e800)); /* 0x407c514 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][6], RM_B4_32(0x1023e800)); /* 0x407c518 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][7], RM_B4_32(0x1023e800)); /* 0x407c51c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][8], RM_B4_32(0x1023e800)); /* 0x407c520 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][9], RM_B4_32(0x1023e800)); /* 0x407c524 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][10], RM_B4_32(0x1023e800)); /* 0x407c528 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][11], RM_B4_32(0x1023e800)); /* 0x407c52c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][12], RM_B4_32(0x1023e800)); /* 0x407c530 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][13], RM_B4_32(0x1023e800)); /* 0x407c534 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][14], RM_B4_32(0x1023e800)); /* 0x407c538 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][15], RM_B4_32(0x1023e800)); /* 0x407c53c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][16], RM_B4_32(0x1023e800)); /* 0x407c540 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][17], RM_B4_32(0x1023e800)); /* 0x407c544 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][18], RM_B4_32(0x1023e800)); /* 0x407c548 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][19], RM_B4_32(0x1023e800)); /* 0x407c54c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][20], RM_B4_32(0x1023e800)); /* 0x407c550 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][21], RM_B4_32(0x1023e800)); /* 0x407c554 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][22], RM_B4_32(0x1023e800)); /* 0x407c558 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][23], RM_B4_32(0x1023e800)); /* 0x407c55c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][24], RM_B4_32(0x1023e800)); /* 0x407c560 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][25], RM_B4_32(0x1023e800)); /* 0x407c564 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][26], RM_B4_32(0x1023e800)); /* 0x407c568 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][27], RM_B4_32(0x1023e800)); /* 0x407c56c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][28], RM_B4_32(0x1023e800)); /* 0x407c570 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][29], RM_B4_32(0x1023e800)); /* 0x407c574 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][30], RM_B4_32(0x1023e800)); /* 0x407c578 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][31], RM_B4_32(0x1023e800)); /* 0x407c57c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][0], RM_B4_32(0x1023e800)); /* 0x407c580 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][1], RM_B4_32(0x1023e800)); /* 0x407c584 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][2], RM_B4_32(0x1023e800)); /* 0x407c588 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][3], RM_B4_32(0x1023e800)); /* 0x407c58c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][4], RM_B4_32(0x1023e800)); /* 0x407c590 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][5], RM_B4_32(0x1023e800)); /* 0x407c594 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][6], RM_B4_32(0x1023e800)); /* 0x407c598 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][7], RM_B4_32(0x1023e800)); /* 0x407c59c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][8], RM_B4_32(0x1023e800)); /* 0x407c5a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][9], RM_B4_32(0x1023e800)); /* 0x407c5a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][10], RM_B4_32(0x1023e800)); /* 0x407c5a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][11], RM_B4_32(0x1023e800)); /* 0x407c5ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][12], RM_B4_32(0x1023e800)); /* 0x407c5b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][13], RM_B4_32(0x1023e800)); /* 0x407c5b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][14], RM_B4_32(0x1023e800)); /* 0x407c5b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][15], RM_B4_32(0x1023e800)); /* 0x407c5bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][16], RM_B4_32(0x1023e800)); /* 0x407c5c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][17], RM_B4_32(0x1023e800)); /* 0x407c5c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][18], RM_B4_32(0x1023e800)); /* 0x407c5c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][19], RM_B4_32(0x1023e800)); /* 0x407c5cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][20], RM_B4_32(0x1023e800)); /* 0x407c5d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][21], RM_B4_32(0x1023e800)); /* 0x407c5d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][22], RM_B4_32(0x1023e800)); /* 0x407c5d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][23], RM_B4_32(0x1023e800)); /* 0x407c5dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][24], RM_B4_32(0x1023e800)); /* 0x407c5e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][25], RM_B4_32(0x1023e800)); /* 0x407c5e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][26], RM_B4_32(0x1023e800)); /* 0x407c5e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][27], RM_B4_32(0x1023e800)); /* 0x407c5ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][28], RM_B4_32(0x1023e800)); /* 0x407c5f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][29], RM_B4_32(0x1023e800)); /* 0x407c5f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][30], RM_B4_32(0x1023e800)); /* 0x407c5f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][31], RM_B4_32(0x1023e800)); /* 0x407c5fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][0], RM_B4_32(0x1023e800)); /* 0x407c600 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][1], RM_B4_32(0x1023e800)); /* 0x407c604 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][2], RM_B4_32(0x1023e800)); /* 0x407c608 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][3], RM_B4_32(0x1023e800)); /* 0x407c60c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][4], RM_B4_32(0x1023e800)); /* 0x407c610 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][5], RM_B4_32(0x1023e800)); /* 0x407c614 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][6], RM_B4_32(0x1023e800)); /* 0x407c618 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][7], RM_B4_32(0x1023e800)); /* 0x407c61c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][8], RM_B4_32(0x1023e800)); /* 0x407c620 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][9], RM_B4_32(0x1023e800)); /* 0x407c624 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][10], RM_B4_32(0x1023e800)); /* 0x407c628 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][11], RM_B4_32(0x1023e800)); /* 0x407c62c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][12], RM_B4_32(0x1023e800)); /* 0x407c630 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][13], RM_B4_32(0x1023e800)); /* 0x407c634 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][14], RM_B4_32(0x1023e800)); /* 0x407c638 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][15], RM_B4_32(0x1023e800)); /* 0x407c63c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][16], RM_B4_32(0x1023e800)); /* 0x407c640 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][17], RM_B4_32(0x1023e800)); /* 0x407c644 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][18], RM_B4_32(0x1023e800)); /* 0x407c648 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][19], RM_B4_32(0x1023e800)); /* 0x407c64c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][20], RM_B4_32(0x1023e800)); /* 0x407c650 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][21], RM_B4_32(0x1023e800)); /* 0x407c654 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][22], RM_B4_32(0x1023e800)); /* 0x407c658 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][23], RM_B4_32(0x1023e800)); /* 0x407c65c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][24], RM_B4_32(0x1023e800)); /* 0x407c660 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][25], RM_B4_32(0x1023e800)); /* 0x407c664 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][26], RM_B4_32(0x1023e800)); /* 0x407c668 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][27], RM_B4_32(0x1023e800)); /* 0x407c66c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][28], RM_B4_32(0x1023e800)); /* 0x407c670 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][29], RM_B4_32(0x1023e800)); /* 0x407c674 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][30], RM_B4_32(0x1023e800)); /* 0x407c678 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][31], RM_B4_32(0x1023e800)); /* 0x407c67c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][0], RM_B4_32(0x1023e800)); /* 0x407c680 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][1], RM_B4_32(0x1023e800)); /* 0x407c684 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][2], RM_B4_32(0x1023e800)); /* 0x407c688 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][3], RM_B4_32(0x1023e800)); /* 0x407c68c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][4], RM_B4_32(0x1023e800)); /* 0x407c690 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][5], RM_B4_32(0x1023e800)); /* 0x407c694 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][6], RM_B4_32(0x1023e800)); /* 0x407c698 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][7], RM_B4_32(0x1023e800)); /* 0x407c69c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][8], RM_B4_32(0x1023e800)); /* 0x407c6a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][9], RM_B4_32(0x1023e800)); /* 0x407c6a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][10], RM_B4_32(0x1023e800)); /* 0x407c6a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][11], RM_B4_32(0x1023e800)); /* 0x407c6ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][12], RM_B4_32(0x1023e800)); /* 0x407c6b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][13], RM_B4_32(0x1023e800)); /* 0x407c6b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][14], RM_B4_32(0x1023e800)); /* 0x407c6b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][15], RM_B4_32(0x1023e800)); /* 0x407c6bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][16], RM_B4_32(0x1023e800)); /* 0x407c6c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][17], RM_B4_32(0x1023e800)); /* 0x407c6c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][18], RM_B4_32(0x1023e800)); /* 0x407c6c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][19], RM_B4_32(0x1023e800)); /* 0x407c6cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][20], RM_B4_32(0x1023e800)); /* 0x407c6d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][21], RM_B4_32(0x1023e800)); /* 0x407c6d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][22], RM_B4_32(0x1023e800)); /* 0x407c6d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][23], RM_B4_32(0x1023e800)); /* 0x407c6dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][24], RM_B4_32(0x1023e800)); /* 0x407c6e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][25], RM_B4_32(0x1023e800)); /* 0x407c6e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][26], RM_B4_32(0x1023e800)); /* 0x407c6e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][27], RM_B4_32(0x1023e800)); /* 0x407c6ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][28], RM_B4_32(0x1023e800)); /* 0x407c6f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][29], RM_B4_32(0x1023e800)); /* 0x407c6f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][30], RM_B4_32(0x1023e800)); /* 0x407c6f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][31], RM_B4_32(0x1023e800)); /* 0x407c6fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][0], RM_B4_32(0x1023e800)); /* 0x407c700 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][1], RM_B4_32(0x1023e800)); /* 0x407c704 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][2], RM_B4_32(0x1023e800)); /* 0x407c708 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][3], RM_B4_32(0x1023e800)); /* 0x407c70c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][4], RM_B4_32(0x1023e800)); /* 0x407c710 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][5], RM_B4_32(0x1023e800)); /* 0x407c714 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][6], RM_B4_32(0x1023e800)); /* 0x407c718 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][7], RM_B4_32(0x1023e800)); /* 0x407c71c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][8], RM_B4_32(0x1023e800)); /* 0x407c720 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][9], RM_B4_32(0x1023e800)); /* 0x407c724 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][10], RM_B4_32(0x1023e800)); /* 0x407c728 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][11], RM_B4_32(0x1023e800)); /* 0x407c72c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][12], RM_B4_32(0x1023e800)); /* 0x407c730 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][13], RM_B4_32(0x1023e800)); /* 0x407c734 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][14], RM_B4_32(0x1023e800)); /* 0x407c738 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][15], RM_B4_32(0x1023e800)); /* 0x407c73c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][16], RM_B4_32(0x1023e800)); /* 0x407c740 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][17], RM_B4_32(0x1023e800)); /* 0x407c744 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][18], RM_B4_32(0x1023e800)); /* 0x407c748 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][19], RM_B4_32(0x1023e800)); /* 0x407c74c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][20], RM_B4_32(0x1023e800)); /* 0x407c750 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][21], RM_B4_32(0x1023e800)); /* 0x407c754 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][22], RM_B4_32(0x1023e800)); /* 0x407c758 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][23], RM_B4_32(0x1023e800)); /* 0x407c75c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][24], RM_B4_32(0x1023e800)); /* 0x407c760 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][25], RM_B4_32(0x1023e800)); /* 0x407c764 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][26], RM_B4_32(0x1023e800)); /* 0x407c768 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][27], RM_B4_32(0x1023e800)); /* 0x407c76c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][28], RM_B4_32(0x1023e800)); /* 0x407c770 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][29], RM_B4_32(0x1023e800)); /* 0x407c774 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][30], RM_B4_32(0x1023e800)); /* 0x407c778 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][31], RM_B4_32(0x1023e800)); /* 0x407c77c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][0], RM_B4_32(0x1023e800)); /* 0x407c780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][1], RM_B4_32(0x1023e800)); /* 0x407c784 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][2], RM_B4_32(0x1023e800)); /* 0x407c788 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][3], RM_B4_32(0x1023e800)); /* 0x407c78c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][4], RM_B4_32(0x1023e800)); /* 0x407c790 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][5], RM_B4_32(0x1023e800)); /* 0x407c794 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][6], RM_B4_32(0x1023e800)); /* 0x407c798 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][7], RM_B4_32(0x1023e800)); /* 0x407c79c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][8], RM_B4_32(0x1023e800)); /* 0x407c7a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][9], RM_B4_32(0x1023e800)); /* 0x407c7a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][10], RM_B4_32(0x1023e800)); /* 0x407c7a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][11], RM_B4_32(0x1023e800)); /* 0x407c7ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][12], RM_B4_32(0x1023e800)); /* 0x407c7b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][13], RM_B4_32(0x1023e800)); /* 0x407c7b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][14], RM_B4_32(0x1023e800)); /* 0x407c7b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][15], RM_B4_32(0x1023e800)); /* 0x407c7bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][16], RM_B4_32(0x1023e800)); /* 0x407c7c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][17], RM_B4_32(0x1023e800)); /* 0x407c7c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][18], RM_B4_32(0x1023e800)); /* 0x407c7c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][19], RM_B4_32(0x1023e800)); /* 0x407c7cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][20], RM_B4_32(0x1023e800)); /* 0x407c7d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][21], RM_B4_32(0x1023e800)); /* 0x407c7d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][22], RM_B4_32(0x1023e800)); /* 0x407c7d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][23], RM_B4_32(0x1023e800)); /* 0x407c7dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][24], RM_B4_32(0x1023e800)); /* 0x407c7e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][25], RM_B4_32(0x1023e800)); /* 0x407c7e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][26], RM_B4_32(0x1023e800)); /* 0x407c7e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][27], RM_B4_32(0x1023e800)); /* 0x407c7ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][28], RM_B4_32(0x1023e800)); /* 0x407c7f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][29], RM_B4_32(0x1023e800)); /* 0x407c7f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][30], RM_B4_32(0x1023e800)); /* 0x407c7f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][31], RM_B4_32(0x1023e800)); /* 0x407c7fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][0], RM_B4_32(0x1023e800)); /* 0x407c800 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][1], RM_B4_32(0x1023e800)); /* 0x407c804 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][2], RM_B4_32(0x1023e800)); /* 0x407c808 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][3], RM_B4_32(0x1023e800)); /* 0x407c80c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][4], RM_B4_32(0x1023e800)); /* 0x407c810 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][5], RM_B4_32(0x1023e800)); /* 0x407c814 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][6], RM_B4_32(0x1023e800)); /* 0x407c818 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][7], RM_B4_32(0x1023e800)); /* 0x407c81c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][8], RM_B4_32(0x1023e800)); /* 0x407c820 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][9], RM_B4_32(0x1023e800)); /* 0x407c824 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][10], RM_B4_32(0x1023e800)); /* 0x407c828 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][11], RM_B4_32(0x1023e800)); /* 0x407c82c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][12], RM_B4_32(0x1023e800)); /* 0x407c830 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][13], RM_B4_32(0x1023e800)); /* 0x407c834 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][14], RM_B4_32(0x1023e800)); /* 0x407c838 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][15], RM_B4_32(0x1023e800)); /* 0x407c83c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][16], RM_B4_32(0x1023e800)); /* 0x407c840 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][17], RM_B4_32(0x1023e800)); /* 0x407c844 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][18], RM_B4_32(0x1023e800)); /* 0x407c848 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][19], RM_B4_32(0x1023e800)); /* 0x407c84c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][20], RM_B4_32(0x1023e800)); /* 0x407c850 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][21], RM_B4_32(0x1023e800)); /* 0x407c854 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][22], RM_B4_32(0x1023e800)); /* 0x407c858 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][23], RM_B4_32(0x1023e800)); /* 0x407c85c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][24], RM_B4_32(0x1023e800)); /* 0x407c860 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][25], RM_B4_32(0x1023e800)); /* 0x407c864 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][26], RM_B4_32(0x1023e800)); /* 0x407c868 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][27], RM_B4_32(0x1023e800)); /* 0x407c86c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][28], RM_B4_32(0x1023e800)); /* 0x407c870 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][29], RM_B4_32(0x1023e800)); /* 0x407c874 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][30], RM_B4_32(0x1023e800)); /* 0x407c878 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][31], RM_B4_32(0x1023e800)); /* 0x407c87c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][0], RM_B4_32(0x1023e800)); /* 0x407c880 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][1], RM_B4_32(0x1023e800)); /* 0x407c884 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][2], RM_B4_32(0x1023e800)); /* 0x407c888 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][3], RM_B4_32(0x1023e800)); /* 0x407c88c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][4], RM_B4_32(0x1023e800)); /* 0x407c890 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][5], RM_B4_32(0x1023e800)); /* 0x407c894 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][6], RM_B4_32(0x1023e800)); /* 0x407c898 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][7], RM_B4_32(0x1023e800)); /* 0x407c89c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][8], RM_B4_32(0x1023e800)); /* 0x407c8a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][9], RM_B4_32(0x1023e800)); /* 0x407c8a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][10], RM_B4_32(0x1023e800)); /* 0x407c8a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][11], RM_B4_32(0x1023e800)); /* 0x407c8ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][12], RM_B4_32(0x1023e800)); /* 0x407c8b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][13], RM_B4_32(0x1023e800)); /* 0x407c8b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][14], RM_B4_32(0x1023e800)); /* 0x407c8b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][15], RM_B4_32(0x1023e800)); /* 0x407c8bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][16], RM_B4_32(0x1023e800)); /* 0x407c8c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][17], RM_B4_32(0x1023e800)); /* 0x407c8c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][18], RM_B4_32(0x1023e800)); /* 0x407c8c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][19], RM_B4_32(0x1023e800)); /* 0x407c8cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][20], RM_B4_32(0x1023e800)); /* 0x407c8d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][21], RM_B4_32(0x1023e800)); /* 0x407c8d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][22], RM_B4_32(0x1023e800)); /* 0x407c8d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][23], RM_B4_32(0x1023e800)); /* 0x407c8dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][24], RM_B4_32(0x1023e800)); /* 0x407c8e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][25], RM_B4_32(0x1023e800)); /* 0x407c8e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][26], RM_B4_32(0x1023e800)); /* 0x407c8e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][27], RM_B4_32(0x1023e800)); /* 0x407c8ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][28], RM_B4_32(0x1023e800)); /* 0x407c8f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][29], RM_B4_32(0x1023e800)); /* 0x407c8f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][30], RM_B4_32(0x1023e800)); /* 0x407c8f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][31], RM_B4_32(0x1023e800)); /* 0x407c8fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][0], RM_B4_32(0x1023e800)); /* 0x407c900 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][1], RM_B4_32(0x1023e800)); /* 0x407c904 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][2], RM_B4_32(0x1023e800)); /* 0x407c908 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][3], RM_B4_32(0x1023e800)); /* 0x407c90c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][4], RM_B4_32(0x1023e800)); /* 0x407c910 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][5], RM_B4_32(0x1023e800)); /* 0x407c914 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][6], RM_B4_32(0x1023e800)); /* 0x407c918 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][7], RM_B4_32(0x1023e800)); /* 0x407c91c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][8], RM_B4_32(0x1023e800)); /* 0x407c920 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][9], RM_B4_32(0x1023e800)); /* 0x407c924 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][10], RM_B4_32(0x1023e800)); /* 0x407c928 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][11], RM_B4_32(0x1023e800)); /* 0x407c92c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][12], RM_B4_32(0x1023e800)); /* 0x407c930 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][13], RM_B4_32(0x1023e800)); /* 0x407c934 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][14], RM_B4_32(0x1023e800)); /* 0x407c938 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][15], RM_B4_32(0x1023e800)); /* 0x407c93c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][16], RM_B4_32(0x1023e800)); /* 0x407c940 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][17], RM_B4_32(0x1023e800)); /* 0x407c944 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][18], RM_B4_32(0x1023e800)); /* 0x407c948 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][19], RM_B4_32(0x1023e800)); /* 0x407c94c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][20], RM_B4_32(0x1023e800)); /* 0x407c950 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][21], RM_B4_32(0x1023e800)); /* 0x407c954 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][22], RM_B4_32(0x1023e800)); /* 0x407c958 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][23], RM_B4_32(0x1023e800)); /* 0x407c95c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][24], RM_B4_32(0x1023e800)); /* 0x407c960 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][25], RM_B4_32(0x1023e800)); /* 0x407c964 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][26], RM_B4_32(0x1023e800)); /* 0x407c968 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][27], RM_B4_32(0x1023e800)); /* 0x407c96c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][28], RM_B4_32(0x1023e800)); /* 0x407c970 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][29], RM_B4_32(0x1023e800)); /* 0x407c974 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][30], RM_B4_32(0x1023e800)); /* 0x407c978 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][31], RM_B4_32(0x1023e800)); /* 0x407c97c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][0], RM_B4_32(0x1023e800)); /* 0x407c980 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][1], RM_B4_32(0x1023e800)); /* 0x407c984 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][2], RM_B4_32(0x1023e800)); /* 0x407c988 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][3], RM_B4_32(0x1023e800)); /* 0x407c98c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][4], RM_B4_32(0x1023e800)); /* 0x407c990 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][5], RM_B4_32(0x1023e800)); /* 0x407c994 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][6], RM_B4_32(0x1023e800)); /* 0x407c998 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][7], RM_B4_32(0x1023e800)); /* 0x407c99c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][8], RM_B4_32(0x1023e800)); /* 0x407c9a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][9], RM_B4_32(0x1023e800)); /* 0x407c9a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][10], RM_B4_32(0x1023e800)); /* 0x407c9a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][11], RM_B4_32(0x1023e800)); /* 0x407c9ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][12], RM_B4_32(0x1023e800)); /* 0x407c9b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][13], RM_B4_32(0x1023e800)); /* 0x407c9b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][14], RM_B4_32(0x1023e800)); /* 0x407c9b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][15], RM_B4_32(0x1023e800)); /* 0x407c9bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][16], RM_B4_32(0x1023e800)); /* 0x407c9c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][17], RM_B4_32(0x1023e800)); /* 0x407c9c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][18], RM_B4_32(0x1023e800)); /* 0x407c9c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][19], RM_B4_32(0x1023e800)); /* 0x407c9cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][20], RM_B4_32(0x1023e800)); /* 0x407c9d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][21], RM_B4_32(0x1023e800)); /* 0x407c9d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][22], RM_B4_32(0x1023e800)); /* 0x407c9d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][23], RM_B4_32(0x1023e800)); /* 0x407c9dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][24], RM_B4_32(0x1023e800)); /* 0x407c9e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][25], RM_B4_32(0x1023e800)); /* 0x407c9e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][26], RM_B4_32(0x1023e800)); /* 0x407c9e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][27], RM_B4_32(0x1023e800)); /* 0x407c9ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][28], RM_B4_32(0x1023e800)); /* 0x407c9f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][29], RM_B4_32(0x1023e800)); /* 0x407c9f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][30], RM_B4_32(0x1023e800)); /* 0x407c9f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][31], RM_B4_32(0x1023e800)); /* 0x407c9fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][0], RM_B4_32(0x1023e800)); /* 0x407ca00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][1], RM_B4_32(0x1023e800)); /* 0x407ca04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][2], RM_B4_32(0x1023e800)); /* 0x407ca08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][3], RM_B4_32(0x1023e800)); /* 0x407ca0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][4], RM_B4_32(0x1023e800)); /* 0x407ca10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][5], RM_B4_32(0x1023e800)); /* 0x407ca14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][6], RM_B4_32(0x1023e800)); /* 0x407ca18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][7], RM_B4_32(0x1023e800)); /* 0x407ca1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][8], RM_B4_32(0x1023e800)); /* 0x407ca20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][9], RM_B4_32(0x1023e800)); /* 0x407ca24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][10], RM_B4_32(0x1023e800)); /* 0x407ca28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][11], RM_B4_32(0x1023e800)); /* 0x407ca2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][12], RM_B4_32(0x1023e800)); /* 0x407ca30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][13], RM_B4_32(0x1023e800)); /* 0x407ca34 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][14], RM_B4_32(0x1023e800)); /* 0x407ca38 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][15], RM_B4_32(0x1023e800)); /* 0x407ca3c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][16], RM_B4_32(0x1023e800)); /* 0x407ca40 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][17], RM_B4_32(0x1023e800)); /* 0x407ca44 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][18], RM_B4_32(0x1023e800)); /* 0x407ca48 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][19], RM_B4_32(0x1023e800)); /* 0x407ca4c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][20], RM_B4_32(0x1023e800)); /* 0x407ca50 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][21], RM_B4_32(0x1023e800)); /* 0x407ca54 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][22], RM_B4_32(0x1023e800)); /* 0x407ca58 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][23], RM_B4_32(0x1023e800)); /* 0x407ca5c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][24], RM_B4_32(0x1023e800)); /* 0x407ca60 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][25], RM_B4_32(0x1023e800)); /* 0x407ca64 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][26], RM_B4_32(0x1023e800)); /* 0x407ca68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][27], RM_B4_32(0x1023e800)); /* 0x407ca6c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][28], RM_B4_32(0x1023e800)); /* 0x407ca70 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][29], RM_B4_32(0x1023e800)); /* 0x407ca74 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][30], RM_B4_32(0x1023e800)); /* 0x407ca78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][31], RM_B4_32(0x1023e800)); /* 0x407ca7c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][0], RM_B4_32(0x1023e800)); /* 0x407ca80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][1], RM_B4_32(0x1023e800)); /* 0x407ca84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][2], RM_B4_32(0x1023e800)); /* 0x407ca88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][3], RM_B4_32(0x1023e800)); /* 0x407ca8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][4], RM_B4_32(0x1023e800)); /* 0x407ca90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][5], RM_B4_32(0x1023e800)); /* 0x407ca94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][6], RM_B4_32(0x1023e800)); /* 0x407ca98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][7], RM_B4_32(0x1023e800)); /* 0x407ca9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][8], RM_B4_32(0x1023e800)); /* 0x407caa0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][9], RM_B4_32(0x1023e800)); /* 0x407caa4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][10], RM_B4_32(0x1023e800)); /* 0x407caa8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][11], RM_B4_32(0x1023e800)); /* 0x407caac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][12], RM_B4_32(0x1023e800)); /* 0x407cab0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][13], RM_B4_32(0x1023e800)); /* 0x407cab4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][14], RM_B4_32(0x1023e800)); /* 0x407cab8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][15], RM_B4_32(0x1023e800)); /* 0x407cabc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][16], RM_B4_32(0x1023e800)); /* 0x407cac0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][17], RM_B4_32(0x1023e800)); /* 0x407cac4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][18], RM_B4_32(0x1023e800)); /* 0x407cac8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][19], RM_B4_32(0x1023e800)); /* 0x407cacc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][20], RM_B4_32(0x1023e800)); /* 0x407cad0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][21], RM_B4_32(0x1023e800)); /* 0x407cad4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][22], RM_B4_32(0x1023e800)); /* 0x407cad8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][23], RM_B4_32(0x1023e800)); /* 0x407cadc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][24], RM_B4_32(0x1023e800)); /* 0x407cae0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][25], RM_B4_32(0x1023e800)); /* 0x407cae4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][26], RM_B4_32(0x1023e800)); /* 0x407cae8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][27], RM_B4_32(0x1023e800)); /* 0x407caec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][28], RM_B4_32(0x1023e800)); /* 0x407caf0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][29], RM_B4_32(0x1023e800)); /* 0x407caf4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][30], RM_B4_32(0x1023e800)); /* 0x407caf8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][31], RM_B4_32(0x1023e800)); /* 0x407cafc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][0], RM_B4_32(0x1023e800)); /* 0x407cb00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][1], RM_B4_32(0x1023e800)); /* 0x407cb04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][2], RM_B4_32(0x1023e800)); /* 0x407cb08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][3], RM_B4_32(0x1023e800)); /* 0x407cb0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][4], RM_B4_32(0x1023e800)); /* 0x407cb10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][5], RM_B4_32(0x1023e800)); /* 0x407cb14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][6], RM_B4_32(0x1023e800)); /* 0x407cb18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][7], RM_B4_32(0x1023e800)); /* 0x407cb1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][8], RM_B4_32(0x1023e800)); /* 0x407cb20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][9], RM_B4_32(0x1023e800)); /* 0x407cb24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][10], RM_B4_32(0x1023e800)); /* 0x407cb28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][11], RM_B4_32(0x1023e800)); /* 0x407cb2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][12], RM_B4_32(0x1023e800)); /* 0x407cb30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][13], RM_B4_32(0x1023e800)); /* 0x407cb34 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][14], RM_B4_32(0x1023e800)); /* 0x407cb38 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][15], RM_B4_32(0x1023e800)); /* 0x407cb3c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][16], RM_B4_32(0x1023e800)); /* 0x407cb40 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][17], RM_B4_32(0x1023e800)); /* 0x407cb44 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][18], RM_B4_32(0x1023e800)); /* 0x407cb48 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][19], RM_B4_32(0x1023e800)); /* 0x407cb4c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][20], RM_B4_32(0x1023e800)); /* 0x407cb50 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][21], RM_B4_32(0x1023e800)); /* 0x407cb54 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][22], RM_B4_32(0x1023e800)); /* 0x407cb58 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][23], RM_B4_32(0x1023e800)); /* 0x407cb5c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][24], RM_B4_32(0x1023e800)); /* 0x407cb60 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][25], RM_B4_32(0x1023e800)); /* 0x407cb64 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][26], RM_B4_32(0x1023e800)); /* 0x407cb68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][27], RM_B4_32(0x1023e800)); /* 0x407cb6c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][28], RM_B4_32(0x1023e800)); /* 0x407cb70 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][29], RM_B4_32(0x1023e800)); /* 0x407cb74 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][30], RM_B4_32(0x1023e800)); /* 0x407cb78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][31], RM_B4_32(0x1023e800)); /* 0x407cb7c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][0], RM_B4_32(0x1023e800)); /* 0x407cb80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][1], RM_B4_32(0x1023e800)); /* 0x407cb84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][2], RM_B4_32(0x1023e800)); /* 0x407cb88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][3], RM_B4_32(0x1023e800)); /* 0x407cb8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][4], RM_B4_32(0x1023e800)); /* 0x407cb90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][5], RM_B4_32(0x1023e800)); /* 0x407cb94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][6], RM_B4_32(0x1023e800)); /* 0x407cb98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][7], RM_B4_32(0x1023e800)); /* 0x407cb9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][8], RM_B4_32(0x1023e800)); /* 0x407cba0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][9], RM_B4_32(0x1023e800)); /* 0x407cba4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][10], RM_B4_32(0x1023e800)); /* 0x407cba8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][11], RM_B4_32(0x1023e800)); /* 0x407cbac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][12], RM_B4_32(0x1023e800)); /* 0x407cbb0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][13], RM_B4_32(0x1023e800)); /* 0x407cbb4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][14], RM_B4_32(0x1023e800)); /* 0x407cbb8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][15], RM_B4_32(0x1023e800)); /* 0x407cbbc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][16], RM_B4_32(0x1023e800)); /* 0x407cbc0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][17], RM_B4_32(0x1023e800)); /* 0x407cbc4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][18], RM_B4_32(0x1023e800)); /* 0x407cbc8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][19], RM_B4_32(0x1023e800)); /* 0x407cbcc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][20], RM_B4_32(0x1023e800)); /* 0x407cbd0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][21], RM_B4_32(0x1023e800)); /* 0x407cbd4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][22], RM_B4_32(0x1023e800)); /* 0x407cbd8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][23], RM_B4_32(0x1023e800)); /* 0x407cbdc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][24], RM_B4_32(0x1023e800)); /* 0x407cbe0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][25], RM_B4_32(0x1023e800)); /* 0x407cbe4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][26], RM_B4_32(0x1023e800)); /* 0x407cbe8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][27], RM_B4_32(0x1023e800)); /* 0x407cbec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][28], RM_B4_32(0x1023e800)); /* 0x407cbf0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][29], RM_B4_32(0x1023e800)); /* 0x407cbf4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][30], RM_B4_32(0x1023e800)); /* 0x407cbf8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][31], RM_B4_32(0x1023e800)); /* 0x407cbfc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][0], RM_B4_32(0x1023e800)); /* 0x407cc00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][1], RM_B4_32(0x1023e800)); /* 0x407cc04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][2], RM_B4_32(0x1023e800)); /* 0x407cc08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][3], RM_B4_32(0x1023e800)); /* 0x407cc0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][4], RM_B4_32(0x1023e800)); /* 0x407cc10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][5], RM_B4_32(0x1023e800)); /* 0x407cc14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][6], RM_B4_32(0x1023e800)); /* 0x407cc18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][7], RM_B4_32(0x1023e800)); /* 0x407cc1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][8], RM_B4_32(0x1023e800)); /* 0x407cc20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][9], RM_B4_32(0x1023e800)); /* 0x407cc24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][10], RM_B4_32(0x1023e800)); /* 0x407cc28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][11], RM_B4_32(0x1023e800)); /* 0x407cc2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][12], RM_B4_32(0x1023e800)); /* 0x407cc30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][13], RM_B4_32(0x1023e800)); /* 0x407cc34 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][14], RM_B4_32(0x1023e800)); /* 0x407cc38 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][15], RM_B4_32(0x1023e800)); /* 0x407cc3c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][16], RM_B4_32(0x1023e800)); /* 0x407cc40 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][17], RM_B4_32(0x1023e800)); /* 0x407cc44 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][18], RM_B4_32(0x1023e800)); /* 0x407cc48 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][19], RM_B4_32(0x1023e800)); /* 0x407cc4c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][20], RM_B4_32(0x1023e800)); /* 0x407cc50 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][21], RM_B4_32(0x1023e800)); /* 0x407cc54 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][22], RM_B4_32(0x1023e800)); /* 0x407cc58 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][23], RM_B4_32(0x1023e800)); /* 0x407cc5c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][24], RM_B4_32(0x1023e800)); /* 0x407cc60 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][25], RM_B4_32(0x1023e800)); /* 0x407cc64 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][26], RM_B4_32(0x1023e800)); /* 0x407cc68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][27], RM_B4_32(0x1023e800)); /* 0x407cc6c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][28], RM_B4_32(0x1023e800)); /* 0x407cc70 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][29], RM_B4_32(0x1023e800)); /* 0x407cc74 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][30], RM_B4_32(0x1023e800)); /* 0x407cc78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][31], RM_B4_32(0x1023e800)); /* 0x407cc7c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][0], RM_B4_32(0x1023e800)); /* 0x407cc80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][1], RM_B4_32(0x1023e800)); /* 0x407cc84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][2], RM_B4_32(0x1023e800)); /* 0x407cc88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][3], RM_B4_32(0x1023e800)); /* 0x407cc8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][4], RM_B4_32(0x1023e800)); /* 0x407cc90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][5], RM_B4_32(0x1023e800)); /* 0x407cc94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][6], RM_B4_32(0x1023e800)); /* 0x407cc98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][7], RM_B4_32(0x1023e800)); /* 0x407cc9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][8], RM_B4_32(0x1023e800)); /* 0x407cca0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][9], RM_B4_32(0x1023e800)); /* 0x407cca4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][10], RM_B4_32(0x1023e800)); /* 0x407cca8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][11], RM_B4_32(0x1023e800)); /* 0x407ccac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][12], RM_B4_32(0x1023e800)); /* 0x407ccb0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][13], RM_B4_32(0x1023e800)); /* 0x407ccb4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][14], RM_B4_32(0x1023e800)); /* 0x407ccb8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][15], RM_B4_32(0x1023e800)); /* 0x407ccbc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][16], RM_B4_32(0x1023e800)); /* 0x407ccc0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][17], RM_B4_32(0x1023e800)); /* 0x407ccc4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][18], RM_B4_32(0x1023e800)); /* 0x407ccc8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][19], RM_B4_32(0x1023e800)); /* 0x407cccc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][20], RM_B4_32(0x1023e800)); /* 0x407ccd0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][21], RM_B4_32(0x1023e800)); /* 0x407ccd4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][22], RM_B4_32(0x1023e800)); /* 0x407ccd8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][23], RM_B4_32(0x1023e800)); /* 0x407ccdc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][24], RM_B4_32(0x1023e800)); /* 0x407cce0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][25], RM_B4_32(0x1023e800)); /* 0x407cce4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][26], RM_B4_32(0x1023e800)); /* 0x407cce8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][27], RM_B4_32(0x1023e800)); /* 0x407ccec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][28], RM_B4_32(0x1023e800)); /* 0x407ccf0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][29], RM_B4_32(0x1023e800)); /* 0x407ccf4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][30], RM_B4_32(0x1023e800)); /* 0x407ccf8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][31], RM_B4_32(0x1023e800)); /* 0x407ccfc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][0], RM_B4_32(0x1023e800)); /* 0x407cd00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][1], RM_B4_32(0x1023e800)); /* 0x407cd04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][2], RM_B4_32(0x1023e800)); /* 0x407cd08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][3], RM_B4_32(0x1023e800)); /* 0x407cd0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][4], RM_B4_32(0x1023e800)); /* 0x407cd10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][5], RM_B4_32(0x1023e800)); /* 0x407cd14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][6], RM_B4_32(0x1023e800)); /* 0x407cd18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][7], RM_B4_32(0x1023e800)); /* 0x407cd1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][8], RM_B4_32(0x1023e800)); /* 0x407cd20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][9], RM_B4_32(0x1023e800)); /* 0x407cd24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][10], RM_B4_32(0x1023e800)); /* 0x407cd28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][11], RM_B4_32(0x1023e800)); /* 0x407cd2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][12], RM_B4_32(0x1023e800)); /* 0x407cd30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][13], RM_B4_32(0x1023e800)); /* 0x407cd34 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][14], RM_B4_32(0x1023e800)); /* 0x407cd38 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][15], RM_B4_32(0x1023e800)); /* 0x407cd3c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][16], RM_B4_32(0x1023e800)); /* 0x407cd40 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][17], RM_B4_32(0x1023e800)); /* 0x407cd44 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][18], RM_B4_32(0x1023e800)); /* 0x407cd48 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][19], RM_B4_32(0x1023e800)); /* 0x407cd4c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][20], RM_B4_32(0x1023e800)); /* 0x407cd50 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][21], RM_B4_32(0x1023e800)); /* 0x407cd54 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][22], RM_B4_32(0x1023e800)); /* 0x407cd58 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][23], RM_B4_32(0x1023e800)); /* 0x407cd5c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][24], RM_B4_32(0x1023e800)); /* 0x407cd60 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][25], RM_B4_32(0x1023e800)); /* 0x407cd64 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][26], RM_B4_32(0x1023e800)); /* 0x407cd68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][27], RM_B4_32(0x1023e800)); /* 0x407cd6c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][28], RM_B4_32(0x1023e800)); /* 0x407cd70 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][29], RM_B4_32(0x1023e800)); /* 0x407cd74 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][30], RM_B4_32(0x1023e800)); /* 0x407cd78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][31], RM_B4_32(0x1023e800)); /* 0x407cd7c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][0], RM_B4_32(0x1023e800)); /* 0x407cd80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][1], RM_B4_32(0x1023e800)); /* 0x407cd84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][2], RM_B4_32(0x1023e800)); /* 0x407cd88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][3], RM_B4_32(0x1023e800)); /* 0x407cd8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][4], RM_B4_32(0x1023e800)); /* 0x407cd90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][5], RM_B4_32(0x1023e800)); /* 0x407cd94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][6], RM_B4_32(0x1023e800)); /* 0x407cd98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][7], RM_B4_32(0x1023e800)); /* 0x407cd9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][8], RM_B4_32(0x1023e800)); /* 0x407cda0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][9], RM_B4_32(0x1023e800)); /* 0x407cda4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][10], RM_B4_32(0x1023e800)); /* 0x407cda8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][11], RM_B4_32(0x1023e800)); /* 0x407cdac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][12], RM_B4_32(0x1023e800)); /* 0x407cdb0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][13], RM_B4_32(0x1023e800)); /* 0x407cdb4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][14], RM_B4_32(0x1023e800)); /* 0x407cdb8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][15], RM_B4_32(0x1023e800)); /* 0x407cdbc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][16], RM_B4_32(0x1023e800)); /* 0x407cdc0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][17], RM_B4_32(0x1023e800)); /* 0x407cdc4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][18], RM_B4_32(0x1023e800)); /* 0x407cdc8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][19], RM_B4_32(0x1023e800)); /* 0x407cdcc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][20], RM_B4_32(0x1023e800)); /* 0x407cdd0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][21], RM_B4_32(0x1023e800)); /* 0x407cdd4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][22], RM_B4_32(0x1023e800)); /* 0x407cdd8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][23], RM_B4_32(0x1023e800)); /* 0x407cddc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][24], RM_B4_32(0x1023e800)); /* 0x407cde0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][25], RM_B4_32(0x1023e800)); /* 0x407cde4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][26], RM_B4_32(0x1023e800)); /* 0x407cde8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][27], RM_B4_32(0x1023e800)); /* 0x407cdec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][28], RM_B4_32(0x1023e800)); /* 0x407cdf0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][29], RM_B4_32(0x1023e800)); /* 0x407cdf4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][30], RM_B4_32(0x1023e800)); /* 0x407cdf8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][31], RM_B4_32(0x1023e800)); /* 0x407cdfc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][0], RM_B4_32(0x1023e800)); /* 0x407ce00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][1], RM_B4_32(0x1023e800)); /* 0x407ce04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][2], RM_B4_32(0x1023e800)); /* 0x407ce08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][3], RM_B4_32(0x1023e800)); /* 0x407ce0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][4], RM_B4_32(0x1023e800)); /* 0x407ce10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][5], RM_B4_32(0x1023e800)); /* 0x407ce14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][6], RM_B4_32(0x1023e800)); /* 0x407ce18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][7], RM_B4_32(0x1023e800)); /* 0x407ce1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][8], RM_B4_32(0x1023e800)); /* 0x407ce20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][9], RM_B4_32(0x1023e800)); /* 0x407ce24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][10], RM_B4_32(0x1023e800)); /* 0x407ce28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][11], RM_B4_32(0x1023e800)); /* 0x407ce2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][12], RM_B4_32(0x1023e800)); /* 0x407ce30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][13], RM_B4_32(0x1023e800)); /* 0x407ce34 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][14], RM_B4_32(0x1023e800)); /* 0x407ce38 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][15], RM_B4_32(0x1023e800)); /* 0x407ce3c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][16], RM_B4_32(0x1023e800)); /* 0x407ce40 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][17], RM_B4_32(0x1023e800)); /* 0x407ce44 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][18], RM_B4_32(0x1023e800)); /* 0x407ce48 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][19], RM_B4_32(0x1023e800)); /* 0x407ce4c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][20], RM_B4_32(0x1023e800)); /* 0x407ce50 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][21], RM_B4_32(0x1023e800)); /* 0x407ce54 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][22], RM_B4_32(0x1023e800)); /* 0x407ce58 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][23], RM_B4_32(0x1023e800)); /* 0x407ce5c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][24], RM_B4_32(0x1023e800)); /* 0x407ce60 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][25], RM_B4_32(0x1023e800)); /* 0x407ce64 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][26], RM_B4_32(0x1023e800)); /* 0x407ce68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][27], RM_B4_32(0x1023e800)); /* 0x407ce6c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][28], RM_B4_32(0x1023e800)); /* 0x407ce70 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][29], RM_B4_32(0x1023e800)); /* 0x407ce74 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][30], RM_B4_32(0x1023e800)); /* 0x407ce78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][31], RM_B4_32(0x1023e800)); /* 0x407ce7c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][0], RM_B4_32(0x1023e800)); /* 0x407ce80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][1], RM_B4_32(0x1023e800)); /* 0x407ce84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][2], RM_B4_32(0x1023e800)); /* 0x407ce88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][3], RM_B4_32(0x1023e800)); /* 0x407ce8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][4], RM_B4_32(0x1023e800)); /* 0x407ce90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][5], RM_B4_32(0x1023e800)); /* 0x407ce94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][6], RM_B4_32(0x1023e800)); /* 0x407ce98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][7], RM_B4_32(0x1023e800)); /* 0x407ce9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][8], RM_B4_32(0x1023e800)); /* 0x407cea0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][9], RM_B4_32(0x1023e800)); /* 0x407cea4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][10], RM_B4_32(0x1023e800)); /* 0x407cea8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][11], RM_B4_32(0x1023e800)); /* 0x407ceac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][12], RM_B4_32(0x1023e800)); /* 0x407ceb0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][13], RM_B4_32(0x1023e800)); /* 0x407ceb4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][14], RM_B4_32(0x1023e800)); /* 0x407ceb8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][15], RM_B4_32(0x1023e800)); /* 0x407cebc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][16], RM_B4_32(0x1023e800)); /* 0x407cec0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][17], RM_B4_32(0x1023e800)); /* 0x407cec4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][18], RM_B4_32(0x1023e800)); /* 0x407cec8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][19], RM_B4_32(0x1023e800)); /* 0x407cecc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][20], RM_B4_32(0x1023e800)); /* 0x407ced0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][21], RM_B4_32(0x1023e800)); /* 0x407ced4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][22], RM_B4_32(0x1023e800)); /* 0x407ced8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][23], RM_B4_32(0x1023e800)); /* 0x407cedc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][24], RM_B4_32(0x1023e800)); /* 0x407cee0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][25], RM_B4_32(0x1023e800)); /* 0x407cee4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][26], RM_B4_32(0x1023e800)); /* 0x407cee8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][27], RM_B4_32(0x1023e800)); /* 0x407ceec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][28], RM_B4_32(0x1023e800)); /* 0x407cef0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][29], RM_B4_32(0x1023e800)); /* 0x407cef4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][30], RM_B4_32(0x1023e800)); /* 0x407cef8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][31], RM_B4_32(0x1023e800)); /* 0x407cefc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][0], RM_B4_32(0x1023e800)); /* 0x407cf00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][1], RM_B4_32(0x1023e800)); /* 0x407cf04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][2], RM_B4_32(0x1023e800)); /* 0x407cf08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][3], RM_B4_32(0x1023e800)); /* 0x407cf0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][4], RM_B4_32(0x1023e800)); /* 0x407cf10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][5], RM_B4_32(0x1023e800)); /* 0x407cf14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][6], RM_B4_32(0x1023e800)); /* 0x407cf18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][7], RM_B4_32(0x1023e800)); /* 0x407cf1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][8], RM_B4_32(0x1023e800)); /* 0x407cf20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][9], RM_B4_32(0x1023e800)); /* 0x407cf24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][10], RM_B4_32(0x1023e800)); /* 0x407cf28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][11], RM_B4_32(0x1023e800)); /* 0x407cf2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][12], RM_B4_32(0x1023e800)); /* 0x407cf30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][13], RM_B4_32(0x1023e800)); /* 0x407cf34 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][14], RM_B4_32(0x1023e800)); /* 0x407cf38 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][15], RM_B4_32(0x1023e800)); /* 0x407cf3c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][16], RM_B4_32(0x1023e800)); /* 0x407cf40 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][17], RM_B4_32(0x1023e800)); /* 0x407cf44 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][18], RM_B4_32(0x1023e800)); /* 0x407cf48 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][19], RM_B4_32(0x1023e800)); /* 0x407cf4c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][20], RM_B4_32(0x1023e800)); /* 0x407cf50 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][21], RM_B4_32(0x1023e800)); /* 0x407cf54 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][22], RM_B4_32(0x1023e800)); /* 0x407cf58 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][23], RM_B4_32(0x1023e800)); /* 0x407cf5c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][24], RM_B4_32(0x1023e800)); /* 0x407cf60 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][25], RM_B4_32(0x1023e800)); /* 0x407cf64 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][26], RM_B4_32(0x1023e800)); /* 0x407cf68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][27], RM_B4_32(0x1023e800)); /* 0x407cf6c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][28], RM_B4_32(0x1023e800)); /* 0x407cf70 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][29], RM_B4_32(0x1023e800)); /* 0x407cf74 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][30], RM_B4_32(0x1023e800)); /* 0x407cf78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][31], RM_B4_32(0x1023e800)); /* 0x407cf7c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][0], RM_B4_32(0x1023e800)); /* 0x407cf80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][1], RM_B4_32(0x1023e800)); /* 0x407cf84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][2], RM_B4_32(0x1023e800)); /* 0x407cf88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][3], RM_B4_32(0x1023e800)); /* 0x407cf8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][4], RM_B4_32(0x1023e800)); /* 0x407cf90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][5], RM_B4_32(0x1023e800)); /* 0x407cf94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][6], RM_B4_32(0x1023e800)); /* 0x407cf98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][7], RM_B4_32(0x1023e800)); /* 0x407cf9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][8], RM_B4_32(0x1023e800)); /* 0x407cfa0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][9], RM_B4_32(0x1023e800)); /* 0x407cfa4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][10], RM_B4_32(0x1023e800)); /* 0x407cfa8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][11], RM_B4_32(0x1023e800)); /* 0x407cfac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][12], RM_B4_32(0x1023e800)); /* 0x407cfb0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][13], RM_B4_32(0x1023e800)); /* 0x407cfb4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][14], RM_B4_32(0x1023e800)); /* 0x407cfb8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][15], RM_B4_32(0x1023e800)); /* 0x407cfbc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][16], RM_B4_32(0x1023e800)); /* 0x407cfc0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][17], RM_B4_32(0x1023e800)); /* 0x407cfc4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][18], RM_B4_32(0x1023e800)); /* 0x407cfc8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][19], RM_B4_32(0x1023e800)); /* 0x407cfcc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][20], RM_B4_32(0x1023e800)); /* 0x407cfd0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][21], RM_B4_32(0x1023e800)); /* 0x407cfd4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][22], RM_B4_32(0x1023e800)); /* 0x407cfd8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][23], RM_B4_32(0x1023e800)); /* 0x407cfdc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][24], RM_B4_32(0x1023e800)); /* 0x407cfe0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][25], RM_B4_32(0x1023e800)); /* 0x407cfe4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][26], RM_B4_32(0x1023e800)); /* 0x407cfe8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][27], RM_B4_32(0x1023e800)); /* 0x407cfec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][28], RM_B4_32(0x1023e800)); /* 0x407cff0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][29], RM_B4_32(0x1023e800)); /* 0x407cff4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][30], RM_B4_32(0x1023e800)); /* 0x407cff8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][31], RM_B4_32(0x1023e800)); /* 0x407cffc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][0], RM_B4_32(0x1023e800)); /* 0x407d000 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][1], RM_B4_32(0x1023e800)); /* 0x407d004 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][2], RM_B4_32(0x1023e800)); /* 0x407d008 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][3], RM_B4_32(0x1023e800)); /* 0x407d00c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][4], RM_B4_32(0x1023e800)); /* 0x407d010 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][5], RM_B4_32(0x1023e800)); /* 0x407d014 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][6], RM_B4_32(0x1023e800)); /* 0x407d018 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][7], RM_B4_32(0x1023e800)); /* 0x407d01c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][8], RM_B4_32(0x1023e800)); /* 0x407d020 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][9], RM_B4_32(0x1023e800)); /* 0x407d024 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][10], RM_B4_32(0x1023e800)); /* 0x407d028 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][11], RM_B4_32(0x1023e800)); /* 0x407d02c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][12], RM_B4_32(0x1023e800)); /* 0x407d030 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][13], RM_B4_32(0x1023e800)); /* 0x407d034 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][14], RM_B4_32(0x1023e800)); /* 0x407d038 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][15], RM_B4_32(0x1023e800)); /* 0x407d03c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][16], RM_B4_32(0x1023e800)); /* 0x407d040 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][17], RM_B4_32(0x1023e800)); /* 0x407d044 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][18], RM_B4_32(0x1023e800)); /* 0x407d048 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][19], RM_B4_32(0x1023e800)); /* 0x407d04c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][20], RM_B4_32(0x1023e800)); /* 0x407d050 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][21], RM_B4_32(0x1023e800)); /* 0x407d054 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][22], RM_B4_32(0x1023e800)); /* 0x407d058 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][23], RM_B4_32(0x1023e800)); /* 0x407d05c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][24], RM_B4_32(0x1023e800)); /* 0x407d060 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][25], RM_B4_32(0x1023e800)); /* 0x407d064 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][26], RM_B4_32(0x1023e800)); /* 0x407d068 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][27], RM_B4_32(0x1023e800)); /* 0x407d06c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][28], RM_B4_32(0x1023e800)); /* 0x407d070 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][29], RM_B4_32(0x1023e800)); /* 0x407d074 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][30], RM_B4_32(0x1023e800)); /* 0x407d078 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][31], RM_B4_32(0x1023e800)); /* 0x407d07c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][0], RM_B4_32(0x1023e800)); /* 0x407d080 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][1], RM_B4_32(0x1023e800)); /* 0x407d084 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][2], RM_B4_32(0x1023e800)); /* 0x407d088 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][3], RM_B4_32(0x1023e800)); /* 0x407d08c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][4], RM_B4_32(0x1023e800)); /* 0x407d090 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][5], RM_B4_32(0x1023e800)); /* 0x407d094 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][6], RM_B4_32(0x1023e800)); /* 0x407d098 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][7], RM_B4_32(0x1023e800)); /* 0x407d09c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][8], RM_B4_32(0x1023e800)); /* 0x407d0a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][9], RM_B4_32(0x1023e800)); /* 0x407d0a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][10], RM_B4_32(0x1023e800)); /* 0x407d0a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][11], RM_B4_32(0x1023e800)); /* 0x407d0ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][12], RM_B4_32(0x1023e800)); /* 0x407d0b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][13], RM_B4_32(0x1023e800)); /* 0x407d0b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][14], RM_B4_32(0x1023e800)); /* 0x407d0b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][15], RM_B4_32(0x1023e800)); /* 0x407d0bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][16], RM_B4_32(0x1023e800)); /* 0x407d0c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][17], RM_B4_32(0x1023e800)); /* 0x407d0c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][18], RM_B4_32(0x1023e800)); /* 0x407d0c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][19], RM_B4_32(0x1023e800)); /* 0x407d0cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][20], RM_B4_32(0x1023e800)); /* 0x407d0d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][21], RM_B4_32(0x1023e800)); /* 0x407d0d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][22], RM_B4_32(0x1023e800)); /* 0x407d0d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][23], RM_B4_32(0x1023e800)); /* 0x407d0dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][24], RM_B4_32(0x1023e800)); /* 0x407d0e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][25], RM_B4_32(0x1023e800)); /* 0x407d0e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][26], RM_B4_32(0x1023e800)); /* 0x407d0e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][27], RM_B4_32(0x1023e800)); /* 0x407d0ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][28], RM_B4_32(0x1023e800)); /* 0x407d0f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][29], RM_B4_32(0x1023e800)); /* 0x407d0f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][30], RM_B4_32(0x1023e800)); /* 0x407d0f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][31], RM_B4_32(0x1023e800)); /* 0x407d0fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][0], RM_B4_32(0x1023e800)); /* 0x407d100 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][1], RM_B4_32(0x1023e800)); /* 0x407d104 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][2], RM_B4_32(0x1023e800)); /* 0x407d108 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][3], RM_B4_32(0x1023e800)); /* 0x407d10c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][4], RM_B4_32(0x1023e800)); /* 0x407d110 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][5], RM_B4_32(0x1023e800)); /* 0x407d114 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][6], RM_B4_32(0x1023e800)); /* 0x407d118 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][7], RM_B4_32(0x1023e800)); /* 0x407d11c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][8], RM_B4_32(0x1023e800)); /* 0x407d120 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][9], RM_B4_32(0x1023e800)); /* 0x407d124 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][10], RM_B4_32(0x1023e800)); /* 0x407d128 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][11], RM_B4_32(0x1023e800)); /* 0x407d12c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][12], RM_B4_32(0x1023e800)); /* 0x407d130 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][13], RM_B4_32(0x1023e800)); /* 0x407d134 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][14], RM_B4_32(0x1023e800)); /* 0x407d138 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][15], RM_B4_32(0x1023e800)); /* 0x407d13c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][16], RM_B4_32(0x1023e800)); /* 0x407d140 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][17], RM_B4_32(0x1023e800)); /* 0x407d144 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][18], RM_B4_32(0x1023e800)); /* 0x407d148 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][19], RM_B4_32(0x1023e800)); /* 0x407d14c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][20], RM_B4_32(0x1023e800)); /* 0x407d150 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][21], RM_B4_32(0x1023e800)); /* 0x407d154 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][22], RM_B4_32(0x1023e800)); /* 0x407d158 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][23], RM_B4_32(0x1023e800)); /* 0x407d15c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][24], RM_B4_32(0x1023e800)); /* 0x407d160 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][25], RM_B4_32(0x1023e800)); /* 0x407d164 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][26], RM_B4_32(0x1023e800)); /* 0x407d168 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][27], RM_B4_32(0x1023e800)); /* 0x407d16c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][28], RM_B4_32(0x1023e800)); /* 0x407d170 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][29], RM_B4_32(0x1023e800)); /* 0x407d174 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][30], RM_B4_32(0x1023e800)); /* 0x407d178 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][31], RM_B4_32(0x1023e800)); /* 0x407d17c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][0], RM_B4_32(0x1023e800)); /* 0x407d180 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][1], RM_B4_32(0x1023e800)); /* 0x407d184 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][2], RM_B4_32(0x1023e800)); /* 0x407d188 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][3], RM_B4_32(0x1023e800)); /* 0x407d18c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][4], RM_B4_32(0x1023e800)); /* 0x407d190 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][5], RM_B4_32(0x1023e800)); /* 0x407d194 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][6], RM_B4_32(0x1023e800)); /* 0x407d198 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][7], RM_B4_32(0x1023e800)); /* 0x407d19c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][8], RM_B4_32(0x1023e800)); /* 0x407d1a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][9], RM_B4_32(0x1023e800)); /* 0x407d1a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][10], RM_B4_32(0x1023e800)); /* 0x407d1a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][11], RM_B4_32(0x1023e800)); /* 0x407d1ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][12], RM_B4_32(0x1023e800)); /* 0x407d1b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][13], RM_B4_32(0x1023e800)); /* 0x407d1b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][14], RM_B4_32(0x1023e800)); /* 0x407d1b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][15], RM_B4_32(0x1023e800)); /* 0x407d1bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][16], RM_B4_32(0x1023e800)); /* 0x407d1c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][17], RM_B4_32(0x1023e800)); /* 0x407d1c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][18], RM_B4_32(0x1023e800)); /* 0x407d1c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][19], RM_B4_32(0x1023e800)); /* 0x407d1cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][20], RM_B4_32(0x1023e800)); /* 0x407d1d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][21], RM_B4_32(0x1023e800)); /* 0x407d1d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][22], RM_B4_32(0x1023e800)); /* 0x407d1d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][23], RM_B4_32(0x1023e800)); /* 0x407d1dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][24], RM_B4_32(0x1023e800)); /* 0x407d1e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][25], RM_B4_32(0x1023e800)); /* 0x407d1e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][26], RM_B4_32(0x1023e800)); /* 0x407d1e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][27], RM_B4_32(0x1023e800)); /* 0x407d1ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][28], RM_B4_32(0x1023e800)); /* 0x407d1f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][29], RM_B4_32(0x1023e800)); /* 0x407d1f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][30], RM_B4_32(0x1023e800)); /* 0x407d1f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][31], RM_B4_32(0x1023e800)); /* 0x407d1fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][0], RM_B4_32(0x1023e800)); /* 0x407d200 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][1], RM_B4_32(0x1023e800)); /* 0x407d204 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][2], RM_B4_32(0x1023e800)); /* 0x407d208 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][3], RM_B4_32(0x1023e800)); /* 0x407d20c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][4], RM_B4_32(0x1023e800)); /* 0x407d210 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][5], RM_B4_32(0x1023e800)); /* 0x407d214 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][6], RM_B4_32(0x1023e800)); /* 0x407d218 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][7], RM_B4_32(0x1023e800)); /* 0x407d21c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][8], RM_B4_32(0x1023e800)); /* 0x407d220 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][9], RM_B4_32(0x1023e800)); /* 0x407d224 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][10], RM_B4_32(0x1023e800)); /* 0x407d228 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][11], RM_B4_32(0x1023e800)); /* 0x407d22c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][12], RM_B4_32(0x1023e800)); /* 0x407d230 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][13], RM_B4_32(0x1023e800)); /* 0x407d234 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][14], RM_B4_32(0x1023e800)); /* 0x407d238 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][15], RM_B4_32(0x1023e800)); /* 0x407d23c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][16], RM_B4_32(0x1023e800)); /* 0x407d240 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][17], RM_B4_32(0x1023e800)); /* 0x407d244 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][18], RM_B4_32(0x1023e800)); /* 0x407d248 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][19], RM_B4_32(0x1023e800)); /* 0x407d24c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][20], RM_B4_32(0x1023e800)); /* 0x407d250 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][21], RM_B4_32(0x1023e800)); /* 0x407d254 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][22], RM_B4_32(0x1023e800)); /* 0x407d258 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][23], RM_B4_32(0x1023e800)); /* 0x407d25c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][24], RM_B4_32(0x1023e800)); /* 0x407d260 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][25], RM_B4_32(0x1023e800)); /* 0x407d264 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][26], RM_B4_32(0x1023e800)); /* 0x407d268 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][27], RM_B4_32(0x1023e800)); /* 0x407d26c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][28], RM_B4_32(0x1023e800)); /* 0x407d270 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][29], RM_B4_32(0x1023e800)); /* 0x407d274 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][30], RM_B4_32(0x1023e800)); /* 0x407d278 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][31], RM_B4_32(0x1023e800)); /* 0x407d27c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][0], RM_B4_32(0x1023e800)); /* 0x407d280 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][1], RM_B4_32(0x1023e800)); /* 0x407d284 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][2], RM_B4_32(0x1023e800)); /* 0x407d288 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][3], RM_B4_32(0x1023e800)); /* 0x407d28c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][4], RM_B4_32(0x1023e800)); /* 0x407d290 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][5], RM_B4_32(0x1023e800)); /* 0x407d294 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][6], RM_B4_32(0x1023e800)); /* 0x407d298 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][7], RM_B4_32(0x1023e800)); /* 0x407d29c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][8], RM_B4_32(0x1023e800)); /* 0x407d2a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][9], RM_B4_32(0x1023e800)); /* 0x407d2a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][10], RM_B4_32(0x1023e800)); /* 0x407d2a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][11], RM_B4_32(0x1023e800)); /* 0x407d2ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][12], RM_B4_32(0x1023e800)); /* 0x407d2b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][13], RM_B4_32(0x1023e800)); /* 0x407d2b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][14], RM_B4_32(0x1023e800)); /* 0x407d2b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][15], RM_B4_32(0x1023e800)); /* 0x407d2bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][16], RM_B4_32(0x1023e800)); /* 0x407d2c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][17], RM_B4_32(0x1023e800)); /* 0x407d2c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][18], RM_B4_32(0x1023e800)); /* 0x407d2c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][19], RM_B4_32(0x1023e800)); /* 0x407d2cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][20], RM_B4_32(0x1023e800)); /* 0x407d2d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][21], RM_B4_32(0x1023e800)); /* 0x407d2d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][22], RM_B4_32(0x1023e800)); /* 0x407d2d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][23], RM_B4_32(0x1023e800)); /* 0x407d2dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][24], RM_B4_32(0x1023e800)); /* 0x407d2e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][25], RM_B4_32(0x1023e800)); /* 0x407d2e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][26], RM_B4_32(0x1023e800)); /* 0x407d2e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][27], RM_B4_32(0x1023e800)); /* 0x407d2ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][28], RM_B4_32(0x1023e800)); /* 0x407d2f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][29], RM_B4_32(0x1023e800)); /* 0x407d2f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][30], RM_B4_32(0x1023e800)); /* 0x407d2f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][31], RM_B4_32(0x1023e800)); /* 0x407d2fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][0], RM_B4_32(0x1023e800)); /* 0x407d300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][1], RM_B4_32(0x1023e800)); /* 0x407d304 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][2], RM_B4_32(0x1023e800)); /* 0x407d308 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][3], RM_B4_32(0x1023e800)); /* 0x407d30c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][4], RM_B4_32(0x1023e800)); /* 0x407d310 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][5], RM_B4_32(0x1023e800)); /* 0x407d314 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][6], RM_B4_32(0x1023e800)); /* 0x407d318 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][7], RM_B4_32(0x1023e800)); /* 0x407d31c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][8], RM_B4_32(0x1023e800)); /* 0x407d320 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][9], RM_B4_32(0x1023e800)); /* 0x407d324 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][10], RM_B4_32(0x1023e800)); /* 0x407d328 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][11], RM_B4_32(0x1023e800)); /* 0x407d32c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][12], RM_B4_32(0x1023e800)); /* 0x407d330 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][13], RM_B4_32(0x1023e800)); /* 0x407d334 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][14], RM_B4_32(0x1023e800)); /* 0x407d338 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][15], RM_B4_32(0x1023e800)); /* 0x407d33c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][16], RM_B4_32(0x1023e800)); /* 0x407d340 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][17], RM_B4_32(0x1023e800)); /* 0x407d344 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][18], RM_B4_32(0x1023e800)); /* 0x407d348 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][19], RM_B4_32(0x1023e800)); /* 0x407d34c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][20], RM_B4_32(0x1023e800)); /* 0x407d350 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][21], RM_B4_32(0x1023e800)); /* 0x407d354 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][22], RM_B4_32(0x1023e800)); /* 0x407d358 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][23], RM_B4_32(0x1023e800)); /* 0x407d35c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][24], RM_B4_32(0x1023e800)); /* 0x407d360 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][25], RM_B4_32(0x1023e800)); /* 0x407d364 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][26], RM_B4_32(0x1023e800)); /* 0x407d368 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][27], RM_B4_32(0x1023e800)); /* 0x407d36c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][28], RM_B4_32(0x1023e800)); /* 0x407d370 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][29], RM_B4_32(0x1023e800)); /* 0x407d374 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][30], RM_B4_32(0x1023e800)); /* 0x407d378 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][31], RM_B4_32(0x1023e800)); /* 0x407d37c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][0], RM_B4_32(0x1023e800)); /* 0x407d380 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][1], RM_B4_32(0x1023e800)); /* 0x407d384 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][2], RM_B4_32(0x1023e800)); /* 0x407d388 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][3], RM_B4_32(0x1023e800)); /* 0x407d38c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][4], RM_B4_32(0x1023e800)); /* 0x407d390 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][5], RM_B4_32(0x1023e800)); /* 0x407d394 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][6], RM_B4_32(0x1023e800)); /* 0x407d398 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][7], RM_B4_32(0x1023e800)); /* 0x407d39c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][8], RM_B4_32(0x1023e800)); /* 0x407d3a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][9], RM_B4_32(0x1023e800)); /* 0x407d3a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][10], RM_B4_32(0x1023e800)); /* 0x407d3a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][11], RM_B4_32(0x1023e800)); /* 0x407d3ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][12], RM_B4_32(0x1023e800)); /* 0x407d3b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][13], RM_B4_32(0x1023e800)); /* 0x407d3b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][14], RM_B4_32(0x1023e800)); /* 0x407d3b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][15], RM_B4_32(0x1023e800)); /* 0x407d3bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][16], RM_B4_32(0x1023e800)); /* 0x407d3c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][17], RM_B4_32(0x1023e800)); /* 0x407d3c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][18], RM_B4_32(0x1023e800)); /* 0x407d3c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][19], RM_B4_32(0x1023e800)); /* 0x407d3cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][20], RM_B4_32(0x1023e800)); /* 0x407d3d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][21], RM_B4_32(0x1023e800)); /* 0x407d3d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][22], RM_B4_32(0x1023e800)); /* 0x407d3d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][23], RM_B4_32(0x1023e800)); /* 0x407d3dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][24], RM_B4_32(0x1023e800)); /* 0x407d3e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][25], RM_B4_32(0x1023e800)); /* 0x407d3e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][26], RM_B4_32(0x1023e800)); /* 0x407d3e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][27], RM_B4_32(0x1023e800)); /* 0x407d3ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][28], RM_B4_32(0x1023e800)); /* 0x407d3f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][29], RM_B4_32(0x1023e800)); /* 0x407d3f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][30], RM_B4_32(0x1023e800)); /* 0x407d3f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][31], RM_B4_32(0x1023e800)); /* 0x407d3fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][0], RM_B4_32(0x1023e800)); /* 0x407d400 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][1], RM_B4_32(0x1023e800)); /* 0x407d404 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][2], RM_B4_32(0x1023e800)); /* 0x407d408 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][3], RM_B4_32(0x1023e800)); /* 0x407d40c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][4], RM_B4_32(0x1023e800)); /* 0x407d410 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][5], RM_B4_32(0x1023e800)); /* 0x407d414 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][6], RM_B4_32(0x1023e800)); /* 0x407d418 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][7], RM_B4_32(0x1023e800)); /* 0x407d41c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][8], RM_B4_32(0x1023e800)); /* 0x407d420 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][9], RM_B4_32(0x1023e800)); /* 0x407d424 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][10], RM_B4_32(0x1023e800)); /* 0x407d428 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][11], RM_B4_32(0x1023e800)); /* 0x407d42c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][12], RM_B4_32(0x1023e800)); /* 0x407d430 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][13], RM_B4_32(0x1023e800)); /* 0x407d434 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][14], RM_B4_32(0x1023e800)); /* 0x407d438 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][15], RM_B4_32(0x1023e800)); /* 0x407d43c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][16], RM_B4_32(0x1023e800)); /* 0x407d440 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][17], RM_B4_32(0x1023e800)); /* 0x407d444 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][18], RM_B4_32(0x1023e800)); /* 0x407d448 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][19], RM_B4_32(0x1023e800)); /* 0x407d44c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][20], RM_B4_32(0x1023e800)); /* 0x407d450 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][21], RM_B4_32(0x1023e800)); /* 0x407d454 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][22], RM_B4_32(0x1023e800)); /* 0x407d458 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][23], RM_B4_32(0x1023e800)); /* 0x407d45c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][24], RM_B4_32(0x1023e800)); /* 0x407d460 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][25], RM_B4_32(0x1023e800)); /* 0x407d464 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][26], RM_B4_32(0x1023e800)); /* 0x407d468 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][27], RM_B4_32(0x1023e800)); /* 0x407d46c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][28], RM_B4_32(0x1023e800)); /* 0x407d470 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][29], RM_B4_32(0x1023e800)); /* 0x407d474 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][30], RM_B4_32(0x1023e800)); /* 0x407d478 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][31], RM_B4_32(0x1023e800)); /* 0x407d47c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][0], RM_B4_32(0x1023e800)); /* 0x407d480 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][1], RM_B4_32(0x1023e800)); /* 0x407d484 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][2], RM_B4_32(0x1023e800)); /* 0x407d488 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][3], RM_B4_32(0x1023e800)); /* 0x407d48c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][4], RM_B4_32(0x1023e800)); /* 0x407d490 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][5], RM_B4_32(0x1023e800)); /* 0x407d494 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][6], RM_B4_32(0x1023e800)); /* 0x407d498 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][7], RM_B4_32(0x1023e800)); /* 0x407d49c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][8], RM_B4_32(0x1023e800)); /* 0x407d4a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][9], RM_B4_32(0x1023e800)); /* 0x407d4a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][10], RM_B4_32(0x1023e800)); /* 0x407d4a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][11], RM_B4_32(0x1023e800)); /* 0x407d4ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][12], RM_B4_32(0x1023e800)); /* 0x407d4b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][13], RM_B4_32(0x1023e800)); /* 0x407d4b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][14], RM_B4_32(0x1023e800)); /* 0x407d4b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][15], RM_B4_32(0x1023e800)); /* 0x407d4bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][16], RM_B4_32(0x1023e800)); /* 0x407d4c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][17], RM_B4_32(0x1023e800)); /* 0x407d4c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][18], RM_B4_32(0x1023e800)); /* 0x407d4c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][19], RM_B4_32(0x1023e800)); /* 0x407d4cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][20], RM_B4_32(0x1023e800)); /* 0x407d4d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][21], RM_B4_32(0x1023e800)); /* 0x407d4d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][22], RM_B4_32(0x1023e800)); /* 0x407d4d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][23], RM_B4_32(0x1023e800)); /* 0x407d4dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][24], RM_B4_32(0x1023e800)); /* 0x407d4e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][25], RM_B4_32(0x1023e800)); /* 0x407d4e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][26], RM_B4_32(0x1023e800)); /* 0x407d4e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][27], RM_B4_32(0x1023e800)); /* 0x407d4ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][28], RM_B4_32(0x1023e800)); /* 0x407d4f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][29], RM_B4_32(0x1023e800)); /* 0x407d4f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][30], RM_B4_32(0x1023e800)); /* 0x407d4f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][31], RM_B4_32(0x1023e800)); /* 0x407d4fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][0], RM_B4_32(0x1023e800)); /* 0x407d500 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][1], RM_B4_32(0x1023e800)); /* 0x407d504 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][2], RM_B4_32(0x1023e800)); /* 0x407d508 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][3], RM_B4_32(0x1023e800)); /* 0x407d50c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][4], RM_B4_32(0x1023e800)); /* 0x407d510 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][5], RM_B4_32(0x1023e800)); /* 0x407d514 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][6], RM_B4_32(0x1023e800)); /* 0x407d518 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][7], RM_B4_32(0x1023e800)); /* 0x407d51c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][8], RM_B4_32(0x1023e800)); /* 0x407d520 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][9], RM_B4_32(0x1023e800)); /* 0x407d524 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][10], RM_B4_32(0x1023e800)); /* 0x407d528 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][11], RM_B4_32(0x1023e800)); /* 0x407d52c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][12], RM_B4_32(0x1023e800)); /* 0x407d530 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][13], RM_B4_32(0x1023e800)); /* 0x407d534 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][14], RM_B4_32(0x1023e800)); /* 0x407d538 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][15], RM_B4_32(0x1023e800)); /* 0x407d53c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][16], RM_B4_32(0x1023e800)); /* 0x407d540 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][17], RM_B4_32(0x1023e800)); /* 0x407d544 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][18], RM_B4_32(0x1023e800)); /* 0x407d548 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][19], RM_B4_32(0x1023e800)); /* 0x407d54c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][20], RM_B4_32(0x1023e800)); /* 0x407d550 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][21], RM_B4_32(0x1023e800)); /* 0x407d554 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][22], RM_B4_32(0x1023e800)); /* 0x407d558 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][23], RM_B4_32(0x1023e800)); /* 0x407d55c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][24], RM_B4_32(0x1023e800)); /* 0x407d560 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][25], RM_B4_32(0x1023e800)); /* 0x407d564 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][26], RM_B4_32(0x1023e800)); /* 0x407d568 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][27], RM_B4_32(0x1023e800)); /* 0x407d56c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][28], RM_B4_32(0x1023e800)); /* 0x407d570 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][29], RM_B4_32(0x1023e800)); /* 0x407d574 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][30], RM_B4_32(0x1023e800)); /* 0x407d578 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][31], RM_B4_32(0x1023e800)); /* 0x407d57c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][0], RM_B4_32(0x1023e800)); /* 0x407d580 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][1], RM_B4_32(0x1023e800)); /* 0x407d584 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][2], RM_B4_32(0x1023e800)); /* 0x407d588 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][3], RM_B4_32(0x1023e800)); /* 0x407d58c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][4], RM_B4_32(0x1023e800)); /* 0x407d590 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][5], RM_B4_32(0x1023e800)); /* 0x407d594 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][6], RM_B4_32(0x1023e800)); /* 0x407d598 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][7], RM_B4_32(0x1023e800)); /* 0x407d59c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][8], RM_B4_32(0x1023e800)); /* 0x407d5a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][9], RM_B4_32(0x1023e800)); /* 0x407d5a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][10], RM_B4_32(0x1023e800)); /* 0x407d5a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][11], RM_B4_32(0x1023e800)); /* 0x407d5ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][12], RM_B4_32(0x1023e800)); /* 0x407d5b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][13], RM_B4_32(0x1023e800)); /* 0x407d5b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][14], RM_B4_32(0x1023e800)); /* 0x407d5b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][15], RM_B4_32(0x1023e800)); /* 0x407d5bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][16], RM_B4_32(0x1023e800)); /* 0x407d5c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][17], RM_B4_32(0x1023e800)); /* 0x407d5c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][18], RM_B4_32(0x1023e800)); /* 0x407d5c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][19], RM_B4_32(0x1023e800)); /* 0x407d5cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][20], RM_B4_32(0x1023e800)); /* 0x407d5d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][21], RM_B4_32(0x1023e800)); /* 0x407d5d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][22], RM_B4_32(0x1023e800)); /* 0x407d5d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][23], RM_B4_32(0x1023e800)); /* 0x407d5dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][24], RM_B4_32(0x1023e800)); /* 0x407d5e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][25], RM_B4_32(0x1023e800)); /* 0x407d5e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][26], RM_B4_32(0x1023e800)); /* 0x407d5e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][27], RM_B4_32(0x1023e800)); /* 0x407d5ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][28], RM_B4_32(0x1023e800)); /* 0x407d5f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][29], RM_B4_32(0x1023e800)); /* 0x407d5f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][30], RM_B4_32(0x1023e800)); /* 0x407d5f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][31], RM_B4_32(0x1023e800)); /* 0x407d5fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][0], RM_B4_32(0x1023e800)); /* 0x407d600 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][1], RM_B4_32(0x1023e800)); /* 0x407d604 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][2], RM_B4_32(0x1023e800)); /* 0x407d608 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][3], RM_B4_32(0x1023e800)); /* 0x407d60c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][4], RM_B4_32(0x1023e800)); /* 0x407d610 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][5], RM_B4_32(0x1023e800)); /* 0x407d614 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][6], RM_B4_32(0x1023e800)); /* 0x407d618 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][7], RM_B4_32(0x1023e800)); /* 0x407d61c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][8], RM_B4_32(0x1023e800)); /* 0x407d620 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][9], RM_B4_32(0x1023e800)); /* 0x407d624 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][10], RM_B4_32(0x1023e800)); /* 0x407d628 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][11], RM_B4_32(0x1023e800)); /* 0x407d62c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][12], RM_B4_32(0x1023e800)); /* 0x407d630 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][13], RM_B4_32(0x1023e800)); /* 0x407d634 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][14], RM_B4_32(0x1023e800)); /* 0x407d638 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][15], RM_B4_32(0x1023e800)); /* 0x407d63c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][16], RM_B4_32(0x1023e800)); /* 0x407d640 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][17], RM_B4_32(0x1023e800)); /* 0x407d644 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][18], RM_B4_32(0x1023e800)); /* 0x407d648 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][19], RM_B4_32(0x1023e800)); /* 0x407d64c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][20], RM_B4_32(0x1023e800)); /* 0x407d650 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][21], RM_B4_32(0x1023e800)); /* 0x407d654 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][22], RM_B4_32(0x1023e800)); /* 0x407d658 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][23], RM_B4_32(0x1023e800)); /* 0x407d65c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][24], RM_B4_32(0x1023e800)); /* 0x407d660 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][25], RM_B4_32(0x1023e800)); /* 0x407d664 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][26], RM_B4_32(0x1023e800)); /* 0x407d668 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][27], RM_B4_32(0x1023e800)); /* 0x407d66c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][28], RM_B4_32(0x1023e800)); /* 0x407d670 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][29], RM_B4_32(0x1023e800)); /* 0x407d674 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][30], RM_B4_32(0x1023e800)); /* 0x407d678 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][31], RM_B4_32(0x1023e800)); /* 0x407d67c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][0], RM_B4_32(0x1023e800)); /* 0x407d680 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][1], RM_B4_32(0x1023e800)); /* 0x407d684 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][2], RM_B4_32(0x1023e800)); /* 0x407d688 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][3], RM_B4_32(0x1023e800)); /* 0x407d68c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][4], RM_B4_32(0x1023e800)); /* 0x407d690 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][5], RM_B4_32(0x1023e800)); /* 0x407d694 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][6], RM_B4_32(0x1023e800)); /* 0x407d698 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][7], RM_B4_32(0x1023e800)); /* 0x407d69c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][8], RM_B4_32(0x1023e800)); /* 0x407d6a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][9], RM_B4_32(0x1023e800)); /* 0x407d6a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][10], RM_B4_32(0x1023e800)); /* 0x407d6a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][11], RM_B4_32(0x1023e800)); /* 0x407d6ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][12], RM_B4_32(0x1023e800)); /* 0x407d6b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][13], RM_B4_32(0x1023e800)); /* 0x407d6b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][14], RM_B4_32(0x1023e800)); /* 0x407d6b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][15], RM_B4_32(0x1023e800)); /* 0x407d6bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][16], RM_B4_32(0x1023e800)); /* 0x407d6c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][17], RM_B4_32(0x1023e800)); /* 0x407d6c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][18], RM_B4_32(0x1023e800)); /* 0x407d6c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][19], RM_B4_32(0x1023e800)); /* 0x407d6cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][20], RM_B4_32(0x1023e800)); /* 0x407d6d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][21], RM_B4_32(0x1023e800)); /* 0x407d6d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][22], RM_B4_32(0x1023e800)); /* 0x407d6d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][23], RM_B4_32(0x1023e800)); /* 0x407d6dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][24], RM_B4_32(0x1023e800)); /* 0x407d6e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][25], RM_B4_32(0x1023e800)); /* 0x407d6e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][26], RM_B4_32(0x1023e800)); /* 0x407d6e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][27], RM_B4_32(0x1023e800)); /* 0x407d6ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][28], RM_B4_32(0x1023e800)); /* 0x407d6f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][29], RM_B4_32(0x1023e800)); /* 0x407d6f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][30], RM_B4_32(0x1023e800)); /* 0x407d6f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][31], RM_B4_32(0x1023e800)); /* 0x407d6fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][0], RM_B4_32(0x1023e800)); /* 0x407d700 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][1], RM_B4_32(0x1023e800)); /* 0x407d704 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][2], RM_B4_32(0x1023e800)); /* 0x407d708 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][3], RM_B4_32(0x1023e800)); /* 0x407d70c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][4], RM_B4_32(0x1023e800)); /* 0x407d710 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][5], RM_B4_32(0x1023e800)); /* 0x407d714 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][6], RM_B4_32(0x1023e800)); /* 0x407d718 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][7], RM_B4_32(0x1023e800)); /* 0x407d71c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][8], RM_B4_32(0x1023e800)); /* 0x407d720 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][9], RM_B4_32(0x1023e800)); /* 0x407d724 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][10], RM_B4_32(0x1023e800)); /* 0x407d728 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][11], RM_B4_32(0x1023e800)); /* 0x407d72c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][12], RM_B4_32(0x1023e800)); /* 0x407d730 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][13], RM_B4_32(0x1023e800)); /* 0x407d734 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][14], RM_B4_32(0x1023e800)); /* 0x407d738 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][15], RM_B4_32(0x1023e800)); /* 0x407d73c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][16], RM_B4_32(0x1023e800)); /* 0x407d740 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][17], RM_B4_32(0x1023e800)); /* 0x407d744 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][18], RM_B4_32(0x1023e800)); /* 0x407d748 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][19], RM_B4_32(0x1023e800)); /* 0x407d74c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][20], RM_B4_32(0x1023e800)); /* 0x407d750 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][21], RM_B4_32(0x1023e800)); /* 0x407d754 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][22], RM_B4_32(0x1023e800)); /* 0x407d758 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][23], RM_B4_32(0x1023e800)); /* 0x407d75c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][24], RM_B4_32(0x1023e800)); /* 0x407d760 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][25], RM_B4_32(0x1023e800)); /* 0x407d764 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][26], RM_B4_32(0x1023e800)); /* 0x407d768 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][27], RM_B4_32(0x1023e800)); /* 0x407d76c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][28], RM_B4_32(0x1023e800)); /* 0x407d770 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][29], RM_B4_32(0x1023e800)); /* 0x407d774 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][30], RM_B4_32(0x1023e800)); /* 0x407d778 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][31], RM_B4_32(0x1023e800)); /* 0x407d77c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][0], RM_B4_32(0x1023e800)); /* 0x407d780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][1], RM_B4_32(0x1023e800)); /* 0x407d784 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][2], RM_B4_32(0x1023e800)); /* 0x407d788 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][3], RM_B4_32(0x1023e800)); /* 0x407d78c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][4], RM_B4_32(0x1023e800)); /* 0x407d790 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][5], RM_B4_32(0x1023e800)); /* 0x407d794 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][6], RM_B4_32(0x1023e800)); /* 0x407d798 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][7], RM_B4_32(0x1023e800)); /* 0x407d79c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][8], RM_B4_32(0x1023e800)); /* 0x407d7a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][9], RM_B4_32(0x1023e800)); /* 0x407d7a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][10], RM_B4_32(0x1023e800)); /* 0x407d7a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][11], RM_B4_32(0x1023e800)); /* 0x407d7ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][12], RM_B4_32(0x1023e800)); /* 0x407d7b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][13], RM_B4_32(0x1023e800)); /* 0x407d7b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][14], RM_B4_32(0x1023e800)); /* 0x407d7b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][15], RM_B4_32(0x1023e800)); /* 0x407d7bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][16], RM_B4_32(0x1023e800)); /* 0x407d7c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][17], RM_B4_32(0x1023e800)); /* 0x407d7c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][18], RM_B4_32(0x1023e800)); /* 0x407d7c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][19], RM_B4_32(0x1023e800)); /* 0x407d7cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][20], RM_B4_32(0x1023e800)); /* 0x407d7d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][21], RM_B4_32(0x1023e800)); /* 0x407d7d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][22], RM_B4_32(0x1023e800)); /* 0x407d7d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][23], RM_B4_32(0x1023e800)); /* 0x407d7dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][24], RM_B4_32(0x1023e800)); /* 0x407d7e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][25], RM_B4_32(0x1023e800)); /* 0x407d7e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][26], RM_B4_32(0x1023e800)); /* 0x407d7e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][27], RM_B4_32(0x1023e800)); /* 0x407d7ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][28], RM_B4_32(0x1023e800)); /* 0x407d7f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][29], RM_B4_32(0x1023e800)); /* 0x407d7f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][30], RM_B4_32(0x1023e800)); /* 0x407d7f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][31], RM_B4_32(0x1023e800)); /* 0x407d7fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][0], RM_B4_32(0x1023e800)); /* 0x407d800 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][1], RM_B4_32(0x1023e800)); /* 0x407d804 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][2], RM_B4_32(0x1023e800)); /* 0x407d808 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][3], RM_B4_32(0x1023e800)); /* 0x407d80c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][4], RM_B4_32(0x1023e800)); /* 0x407d810 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][5], RM_B4_32(0x1023e800)); /* 0x407d814 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][6], RM_B4_32(0x1023e800)); /* 0x407d818 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][7], RM_B4_32(0x1023e800)); /* 0x407d81c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][8], RM_B4_32(0x1023e800)); /* 0x407d820 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][9], RM_B4_32(0x1023e800)); /* 0x407d824 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][10], RM_B4_32(0x1023e800)); /* 0x407d828 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][11], RM_B4_32(0x1023e800)); /* 0x407d82c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][12], RM_B4_32(0x1023e800)); /* 0x407d830 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][13], RM_B4_32(0x1023e800)); /* 0x407d834 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][14], RM_B4_32(0x1023e800)); /* 0x407d838 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][15], RM_B4_32(0x1023e800)); /* 0x407d83c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][16], RM_B4_32(0x1023e800)); /* 0x407d840 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][17], RM_B4_32(0x1023e800)); /* 0x407d844 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][18], RM_B4_32(0x1023e800)); /* 0x407d848 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][19], RM_B4_32(0x1023e800)); /* 0x407d84c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][20], RM_B4_32(0x1023e800)); /* 0x407d850 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][21], RM_B4_32(0x1023e800)); /* 0x407d854 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][22], RM_B4_32(0x1023e800)); /* 0x407d858 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][23], RM_B4_32(0x1023e800)); /* 0x407d85c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][24], RM_B4_32(0x1023e800)); /* 0x407d860 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][25], RM_B4_32(0x1023e800)); /* 0x407d864 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][26], RM_B4_32(0x1023e800)); /* 0x407d868 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][27], RM_B4_32(0x1023e800)); /* 0x407d86c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][28], RM_B4_32(0x1023e800)); /* 0x407d870 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][29], RM_B4_32(0x1023e800)); /* 0x407d874 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][30], RM_B4_32(0x1023e800)); /* 0x407d878 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][31], RM_B4_32(0x1023e800)); /* 0x407d87c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][0], RM_B4_32(0x1023e800)); /* 0x407d880 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][1], RM_B4_32(0x1023e800)); /* 0x407d884 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][2], RM_B4_32(0x1023e800)); /* 0x407d888 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][3], RM_B4_32(0x1023e800)); /* 0x407d88c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][4], RM_B4_32(0x1023e800)); /* 0x407d890 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][5], RM_B4_32(0x1023e800)); /* 0x407d894 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][6], RM_B4_32(0x1023e800)); /* 0x407d898 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][7], RM_B4_32(0x1023e800)); /* 0x407d89c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][8], RM_B4_32(0x1023e800)); /* 0x407d8a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][9], RM_B4_32(0x1023e800)); /* 0x407d8a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][10], RM_B4_32(0x1023e800)); /* 0x407d8a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][11], RM_B4_32(0x1023e800)); /* 0x407d8ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][12], RM_B4_32(0x1023e800)); /* 0x407d8b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][13], RM_B4_32(0x1023e800)); /* 0x407d8b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][14], RM_B4_32(0x1023e800)); /* 0x407d8b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][15], RM_B4_32(0x1023e800)); /* 0x407d8bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][16], RM_B4_32(0x1023e800)); /* 0x407d8c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][17], RM_B4_32(0x1023e800)); /* 0x407d8c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][18], RM_B4_32(0x1023e800)); /* 0x407d8c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][19], RM_B4_32(0x1023e800)); /* 0x407d8cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][20], RM_B4_32(0x1023e800)); /* 0x407d8d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][21], RM_B4_32(0x1023e800)); /* 0x407d8d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][22], RM_B4_32(0x1023e800)); /* 0x407d8d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][23], RM_B4_32(0x1023e800)); /* 0x407d8dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][24], RM_B4_32(0x1023e800)); /* 0x407d8e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][25], RM_B4_32(0x1023e800)); /* 0x407d8e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][26], RM_B4_32(0x1023e800)); /* 0x407d8e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][27], RM_B4_32(0x1023e800)); /* 0x407d8ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][28], RM_B4_32(0x1023e800)); /* 0x407d8f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][29], RM_B4_32(0x1023e800)); /* 0x407d8f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][30], RM_B4_32(0x1023e800)); /* 0x407d8f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][31], RM_B4_32(0x1023e800)); /* 0x407d8fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][0], RM_B4_32(0x1023e800)); /* 0x407d900 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][1], RM_B4_32(0x1023e800)); /* 0x407d904 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][2], RM_B4_32(0x1023e800)); /* 0x407d908 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][3], RM_B4_32(0x1023e800)); /* 0x407d90c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][4], RM_B4_32(0x1023e800)); /* 0x407d910 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][5], RM_B4_32(0x1023e800)); /* 0x407d914 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][6], RM_B4_32(0x1023e800)); /* 0x407d918 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][7], RM_B4_32(0x1023e800)); /* 0x407d91c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][8], RM_B4_32(0x1023e800)); /* 0x407d920 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][9], RM_B4_32(0x1023e800)); /* 0x407d924 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][10], RM_B4_32(0x1023e800)); /* 0x407d928 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][11], RM_B4_32(0x1023e800)); /* 0x407d92c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][12], RM_B4_32(0x1023e800)); /* 0x407d930 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][13], RM_B4_32(0x1023e800)); /* 0x407d934 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][14], RM_B4_32(0x1023e800)); /* 0x407d938 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][15], RM_B4_32(0x1023e800)); /* 0x407d93c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][16], RM_B4_32(0x1023e800)); /* 0x407d940 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][17], RM_B4_32(0x1023e800)); /* 0x407d944 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][18], RM_B4_32(0x1023e800)); /* 0x407d948 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][19], RM_B4_32(0x1023e800)); /* 0x407d94c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][20], RM_B4_32(0x1023e800)); /* 0x407d950 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][21], RM_B4_32(0x1023e800)); /* 0x407d954 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][22], RM_B4_32(0x1023e800)); /* 0x407d958 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][23], RM_B4_32(0x1023e800)); /* 0x407d95c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][24], RM_B4_32(0x1023e800)); /* 0x407d960 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][25], RM_B4_32(0x1023e800)); /* 0x407d964 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][26], RM_B4_32(0x1023e800)); /* 0x407d968 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][27], RM_B4_32(0x1023e800)); /* 0x407d96c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][28], RM_B4_32(0x1023e800)); /* 0x407d970 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][29], RM_B4_32(0x1023e800)); /* 0x407d974 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][30], RM_B4_32(0x1023e800)); /* 0x407d978 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][31], RM_B4_32(0x1023e800)); /* 0x407d97c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][0], RM_B4_32(0x1023e800)); /* 0x407d980 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][1], RM_B4_32(0x1023e800)); /* 0x407d984 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][2], RM_B4_32(0x1023e800)); /* 0x407d988 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][3], RM_B4_32(0x1023e800)); /* 0x407d98c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][4], RM_B4_32(0x1023e800)); /* 0x407d990 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][5], RM_B4_32(0x1023e800)); /* 0x407d994 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][6], RM_B4_32(0x1023e800)); /* 0x407d998 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][7], RM_B4_32(0x1023e800)); /* 0x407d99c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][8], RM_B4_32(0x1023e800)); /* 0x407d9a0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][9], RM_B4_32(0x1023e800)); /* 0x407d9a4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][10], RM_B4_32(0x1023e800)); /* 0x407d9a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][11], RM_B4_32(0x1023e800)); /* 0x407d9ac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][12], RM_B4_32(0x1023e800)); /* 0x407d9b0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][13], RM_B4_32(0x1023e800)); /* 0x407d9b4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][14], RM_B4_32(0x1023e800)); /* 0x407d9b8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][15], RM_B4_32(0x1023e800)); /* 0x407d9bc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][16], RM_B4_32(0x1023e800)); /* 0x407d9c0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][17], RM_B4_32(0x1023e800)); /* 0x407d9c4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][18], RM_B4_32(0x1023e800)); /* 0x407d9c8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][19], RM_B4_32(0x1023e800)); /* 0x407d9cc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][20], RM_B4_32(0x1023e800)); /* 0x407d9d0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][21], RM_B4_32(0x1023e800)); /* 0x407d9d4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][22], RM_B4_32(0x1023e800)); /* 0x407d9d8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][23], RM_B4_32(0x1023e800)); /* 0x407d9dc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][24], RM_B4_32(0x1023e800)); /* 0x407d9e0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][25], RM_B4_32(0x1023e800)); /* 0x407d9e4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][26], RM_B4_32(0x1023e800)); /* 0x407d9e8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][27], RM_B4_32(0x1023e800)); /* 0x407d9ec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][28], RM_B4_32(0x1023e800)); /* 0x407d9f0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][29], RM_B4_32(0x1023e800)); /* 0x407d9f4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][30], RM_B4_32(0x1023e800)); /* 0x407d9f8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][31], RM_B4_32(0x1023e800)); /* 0x407d9fc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][0], RM_B4_32(0x1023e800)); /* 0x407da00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][1], RM_B4_32(0x1023e800)); /* 0x407da04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][2], RM_B4_32(0x1023e800)); /* 0x407da08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][3], RM_B4_32(0x1023e800)); /* 0x407da0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][4], RM_B4_32(0x1023e800)); /* 0x407da10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][5], RM_B4_32(0x1023e800)); /* 0x407da14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][6], RM_B4_32(0x1023e800)); /* 0x407da18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][7], RM_B4_32(0x1023e800)); /* 0x407da1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][8], RM_B4_32(0x1023e800)); /* 0x407da20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][9], RM_B4_32(0x1023e800)); /* 0x407da24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][10], RM_B4_32(0x1023e800)); /* 0x407da28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][11], RM_B4_32(0x1023e800)); /* 0x407da2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][12], RM_B4_32(0x1023e800)); /* 0x407da30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][13], RM_B4_32(0x1023e800)); /* 0x407da34 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][14], RM_B4_32(0x1023e800)); /* 0x407da38 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][15], RM_B4_32(0x1023e800)); /* 0x407da3c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][16], RM_B4_32(0x1023e800)); /* 0x407da40 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][17], RM_B4_32(0x1023e800)); /* 0x407da44 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][18], RM_B4_32(0x1023e800)); /* 0x407da48 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][19], RM_B4_32(0x1023e800)); /* 0x407da4c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][20], RM_B4_32(0x1023e800)); /* 0x407da50 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][21], RM_B4_32(0x1023e800)); /* 0x407da54 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][22], RM_B4_32(0x1023e800)); /* 0x407da58 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][23], RM_B4_32(0x1023e800)); /* 0x407da5c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][24], RM_B4_32(0x1023e800)); /* 0x407da60 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][25], RM_B4_32(0x1023e800)); /* 0x407da64 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][26], RM_B4_32(0x1023e800)); /* 0x407da68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][27], RM_B4_32(0x1023e800)); /* 0x407da6c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][28], RM_B4_32(0x1023e800)); /* 0x407da70 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][29], RM_B4_32(0x1023e800)); /* 0x407da74 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][30], RM_B4_32(0x1023e800)); /* 0x407da78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][31], RM_B4_32(0x1023e800)); /* 0x407da7c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][0], RM_B4_32(0x1023e800)); /* 0x407da80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][1], RM_B4_32(0x1023e800)); /* 0x407da84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][2], RM_B4_32(0x1023e800)); /* 0x407da88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][3], RM_B4_32(0x1023e800)); /* 0x407da8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][4], RM_B4_32(0x1023e800)); /* 0x407da90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][5], RM_B4_32(0x1023e800)); /* 0x407da94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][6], RM_B4_32(0x1023e800)); /* 0x407da98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][7], RM_B4_32(0x1023e800)); /* 0x407da9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][8], RM_B4_32(0x1023e800)); /* 0x407daa0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][9], RM_B4_32(0x1023e800)); /* 0x407daa4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][10], RM_B4_32(0x1023e800)); /* 0x407daa8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][11], RM_B4_32(0x1023e800)); /* 0x407daac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][12], RM_B4_32(0x1023e800)); /* 0x407dab0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][13], RM_B4_32(0x1023e800)); /* 0x407dab4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][14], RM_B4_32(0x1023e800)); /* 0x407dab8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][15], RM_B4_32(0x1023e800)); /* 0x407dabc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][16], RM_B4_32(0x1023e800)); /* 0x407dac0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][17], RM_B4_32(0x1023e800)); /* 0x407dac4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][18], RM_B4_32(0x1023e800)); /* 0x407dac8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][19], RM_B4_32(0x1023e800)); /* 0x407dacc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][20], RM_B4_32(0x1023e800)); /* 0x407dad0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][21], RM_B4_32(0x1023e800)); /* 0x407dad4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][22], RM_B4_32(0x1023e800)); /* 0x407dad8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][23], RM_B4_32(0x1023e800)); /* 0x407dadc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][24], RM_B4_32(0x1023e800)); /* 0x407dae0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][25], RM_B4_32(0x1023e800)); /* 0x407dae4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][26], RM_B4_32(0x1023e800)); /* 0x407dae8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][27], RM_B4_32(0x1023e800)); /* 0x407daec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][28], RM_B4_32(0x1023e800)); /* 0x407daf0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][29], RM_B4_32(0x1023e800)); /* 0x407daf4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][30], RM_B4_32(0x1023e800)); /* 0x407daf8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][31], RM_B4_32(0x1023e800)); /* 0x407dafc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][0], RM_B4_32(0x1023e800)); /* 0x407db00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][1], RM_B4_32(0x1023e800)); /* 0x407db04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][2], RM_B4_32(0x1023e800)); /* 0x407db08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][3], RM_B4_32(0x1023e800)); /* 0x407db0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][4], RM_B4_32(0x1023e800)); /* 0x407db10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][5], RM_B4_32(0x1023e800)); /* 0x407db14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][6], RM_B4_32(0x1023e800)); /* 0x407db18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][7], RM_B4_32(0x1023e800)); /* 0x407db1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][8], RM_B4_32(0x1023e800)); /* 0x407db20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][9], RM_B4_32(0x1023e800)); /* 0x407db24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][10], RM_B4_32(0x1023e800)); /* 0x407db28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][11], RM_B4_32(0x1023e800)); /* 0x407db2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][12], RM_B4_32(0x1023e800)); /* 0x407db30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][13], RM_B4_32(0x1023e800)); /* 0x407db34 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][14], RM_B4_32(0x1023e800)); /* 0x407db38 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][15], RM_B4_32(0x1023e800)); /* 0x407db3c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][16], RM_B4_32(0x1023e800)); /* 0x407db40 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][17], RM_B4_32(0x1023e800)); /* 0x407db44 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][18], RM_B4_32(0x1023e800)); /* 0x407db48 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][19], RM_B4_32(0x1023e800)); /* 0x407db4c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][20], RM_B4_32(0x1023e800)); /* 0x407db50 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][21], RM_B4_32(0x1023e800)); /* 0x407db54 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][22], RM_B4_32(0x1023e800)); /* 0x407db58 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][23], RM_B4_32(0x1023e800)); /* 0x407db5c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][24], RM_B4_32(0x1023e800)); /* 0x407db60 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][25], RM_B4_32(0x1023e800)); /* 0x407db64 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][26], RM_B4_32(0x1023e800)); /* 0x407db68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][27], RM_B4_32(0x1023e800)); /* 0x407db6c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][28], RM_B4_32(0x1023e800)); /* 0x407db70 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][29], RM_B4_32(0x1023e800)); /* 0x407db74 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][30], RM_B4_32(0x1023e800)); /* 0x407db78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][31], RM_B4_32(0x1023e800)); /* 0x407db7c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][0], RM_B4_32(0x1023e800)); /* 0x407db80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][1], RM_B4_32(0x1023e800)); /* 0x407db84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][2], RM_B4_32(0x1023e800)); /* 0x407db88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][3], RM_B4_32(0x1023e800)); /* 0x407db8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][4], RM_B4_32(0x1023e800)); /* 0x407db90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][5], RM_B4_32(0x1023e800)); /* 0x407db94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][6], RM_B4_32(0x1023e800)); /* 0x407db98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][7], RM_B4_32(0x1023e800)); /* 0x407db9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][8], RM_B4_32(0x1023e800)); /* 0x407dba0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][9], RM_B4_32(0x1023e800)); /* 0x407dba4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][10], RM_B4_32(0x1023e800)); /* 0x407dba8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][11], RM_B4_32(0x1023e800)); /* 0x407dbac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][12], RM_B4_32(0x1023e800)); /* 0x407dbb0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][13], RM_B4_32(0x1023e800)); /* 0x407dbb4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][14], RM_B4_32(0x1023e800)); /* 0x407dbb8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][15], RM_B4_32(0x1023e800)); /* 0x407dbbc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][16], RM_B4_32(0x1023e800)); /* 0x407dbc0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][17], RM_B4_32(0x1023e800)); /* 0x407dbc4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][18], RM_B4_32(0x1023e800)); /* 0x407dbc8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][19], RM_B4_32(0x1023e800)); /* 0x407dbcc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][20], RM_B4_32(0x1023e800)); /* 0x407dbd0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][21], RM_B4_32(0x1023e800)); /* 0x407dbd4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][22], RM_B4_32(0x1023e800)); /* 0x407dbd8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][23], RM_B4_32(0x1023e800)); /* 0x407dbdc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][24], RM_B4_32(0x1023e800)); /* 0x407dbe0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][25], RM_B4_32(0x1023e800)); /* 0x407dbe4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][26], RM_B4_32(0x1023e800)); /* 0x407dbe8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][27], RM_B4_32(0x1023e800)); /* 0x407dbec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][28], RM_B4_32(0x1023e800)); /* 0x407dbf0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][29], RM_B4_32(0x1023e800)); /* 0x407dbf4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][30], RM_B4_32(0x1023e800)); /* 0x407dbf8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][31], RM_B4_32(0x1023e800)); /* 0x407dbfc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][0], RM_B4_32(0x1023e800)); /* 0x407dc00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][1], RM_B4_32(0x1023e800)); /* 0x407dc04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][2], RM_B4_32(0x1023e800)); /* 0x407dc08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][3], RM_B4_32(0x1023e800)); /* 0x407dc0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][4], RM_B4_32(0x1023e800)); /* 0x407dc10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][5], RM_B4_32(0x1023e800)); /* 0x407dc14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][6], RM_B4_32(0x1023e800)); /* 0x407dc18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][7], RM_B4_32(0x1023e800)); /* 0x407dc1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][8], RM_B4_32(0x1023e800)); /* 0x407dc20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][9], RM_B4_32(0x1023e800)); /* 0x407dc24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][10], RM_B4_32(0x1023e800)); /* 0x407dc28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][11], RM_B4_32(0x1023e800)); /* 0x407dc2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][12], RM_B4_32(0x1023e800)); /* 0x407dc30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][13], RM_B4_32(0x1023e800)); /* 0x407dc34 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][14], RM_B4_32(0x1023e800)); /* 0x407dc38 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][15], RM_B4_32(0x1023e800)); /* 0x407dc3c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][16], RM_B4_32(0x1023e800)); /* 0x407dc40 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][17], RM_B4_32(0x1023e800)); /* 0x407dc44 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][18], RM_B4_32(0x1023e800)); /* 0x407dc48 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][19], RM_B4_32(0x1023e800)); /* 0x407dc4c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][20], RM_B4_32(0x1023e800)); /* 0x407dc50 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][21], RM_B4_32(0x1023e800)); /* 0x407dc54 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][22], RM_B4_32(0x1023e800)); /* 0x407dc58 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][23], RM_B4_32(0x1023e800)); /* 0x407dc5c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][24], RM_B4_32(0x1023e800)); /* 0x407dc60 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][25], RM_B4_32(0x1023e800)); /* 0x407dc64 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][26], RM_B4_32(0x1023e800)); /* 0x407dc68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][27], RM_B4_32(0x1023e800)); /* 0x407dc6c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][28], RM_B4_32(0x1023e800)); /* 0x407dc70 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][29], RM_B4_32(0x1023e800)); /* 0x407dc74 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][30], RM_B4_32(0x1023e800)); /* 0x407dc78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][31], RM_B4_32(0x1023e800)); /* 0x407dc7c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][0], RM_B4_32(0x1023e800)); /* 0x407dc80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][1], RM_B4_32(0x1023e800)); /* 0x407dc84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][2], RM_B4_32(0x1023e800)); /* 0x407dc88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][3], RM_B4_32(0x1023e800)); /* 0x407dc8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][4], RM_B4_32(0x1023e800)); /* 0x407dc90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][5], RM_B4_32(0x1023e800)); /* 0x407dc94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][6], RM_B4_32(0x1023e800)); /* 0x407dc98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][7], RM_B4_32(0x1023e800)); /* 0x407dc9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][8], RM_B4_32(0x1023e800)); /* 0x407dca0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][9], RM_B4_32(0x1023e800)); /* 0x407dca4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][10], RM_B4_32(0x1023e800)); /* 0x407dca8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][11], RM_B4_32(0x1023e800)); /* 0x407dcac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][12], RM_B4_32(0x1023e800)); /* 0x407dcb0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][13], RM_B4_32(0x1023e800)); /* 0x407dcb4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][14], RM_B4_32(0x1023e800)); /* 0x407dcb8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][15], RM_B4_32(0x1023e800)); /* 0x407dcbc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][16], RM_B4_32(0x1023e800)); /* 0x407dcc0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][17], RM_B4_32(0x1023e800)); /* 0x407dcc4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][18], RM_B4_32(0x1023e800)); /* 0x407dcc8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][19], RM_B4_32(0x1023e800)); /* 0x407dccc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][20], RM_B4_32(0x1023e800)); /* 0x407dcd0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][21], RM_B4_32(0x1023e800)); /* 0x407dcd4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][22], RM_B4_32(0x1023e800)); /* 0x407dcd8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][23], RM_B4_32(0x1023e800)); /* 0x407dcdc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][24], RM_B4_32(0x1023e800)); /* 0x407dce0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][25], RM_B4_32(0x1023e800)); /* 0x407dce4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][26], RM_B4_32(0x1023e800)); /* 0x407dce8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][27], RM_B4_32(0x1023e800)); /* 0x407dcec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][28], RM_B4_32(0x1023e800)); /* 0x407dcf0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][29], RM_B4_32(0x1023e800)); /* 0x407dcf4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][30], RM_B4_32(0x1023e800)); /* 0x407dcf8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][31], RM_B4_32(0x1023e800)); /* 0x407dcfc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][0], RM_B4_32(0x1023e800)); /* 0x407dd00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][1], RM_B4_32(0x1023e800)); /* 0x407dd04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][2], RM_B4_32(0x1023e800)); /* 0x407dd08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][3], RM_B4_32(0x1023e800)); /* 0x407dd0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][4], RM_B4_32(0x1023e800)); /* 0x407dd10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][5], RM_B4_32(0x1023e800)); /* 0x407dd14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][6], RM_B4_32(0x1023e800)); /* 0x407dd18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][7], RM_B4_32(0x1023e800)); /* 0x407dd1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][8], RM_B4_32(0x1023e800)); /* 0x407dd20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][9], RM_B4_32(0x1023e800)); /* 0x407dd24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][10], RM_B4_32(0x1023e800)); /* 0x407dd28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][11], RM_B4_32(0x1023e800)); /* 0x407dd2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][12], RM_B4_32(0x1023e800)); /* 0x407dd30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][13], RM_B4_32(0x1023e800)); /* 0x407dd34 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][14], RM_B4_32(0x1023e800)); /* 0x407dd38 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][15], RM_B4_32(0x1023e800)); /* 0x407dd3c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][16], RM_B4_32(0x1023e800)); /* 0x407dd40 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][17], RM_B4_32(0x1023e800)); /* 0x407dd44 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][18], RM_B4_32(0x1023e800)); /* 0x407dd48 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][19], RM_B4_32(0x1023e800)); /* 0x407dd4c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][20], RM_B4_32(0x1023e800)); /* 0x407dd50 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][21], RM_B4_32(0x1023e800)); /* 0x407dd54 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][22], RM_B4_32(0x1023e800)); /* 0x407dd58 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][23], RM_B4_32(0x1023e800)); /* 0x407dd5c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][24], RM_B4_32(0x1023e800)); /* 0x407dd60 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][25], RM_B4_32(0x1023e800)); /* 0x407dd64 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][26], RM_B4_32(0x1023e800)); /* 0x407dd68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][27], RM_B4_32(0x1023e800)); /* 0x407dd6c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][28], RM_B4_32(0x1023e800)); /* 0x407dd70 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][29], RM_B4_32(0x1023e800)); /* 0x407dd74 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][30], RM_B4_32(0x1023e800)); /* 0x407dd78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][31], RM_B4_32(0x1023e800)); /* 0x407dd7c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][0], RM_B4_32(0x1023e800)); /* 0x407dd80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][1], RM_B4_32(0x1023e800)); /* 0x407dd84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][2], RM_B4_32(0x1023e800)); /* 0x407dd88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][3], RM_B4_32(0x1023e800)); /* 0x407dd8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][4], RM_B4_32(0x1023e800)); /* 0x407dd90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][5], RM_B4_32(0x1023e800)); /* 0x407dd94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][6], RM_B4_32(0x1023e800)); /* 0x407dd98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][7], RM_B4_32(0x1023e800)); /* 0x407dd9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][8], RM_B4_32(0x1023e800)); /* 0x407dda0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][9], RM_B4_32(0x1023e800)); /* 0x407dda4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][10], RM_B4_32(0x1023e800)); /* 0x407dda8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][11], RM_B4_32(0x1023e800)); /* 0x407ddac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][12], RM_B4_32(0x1023e800)); /* 0x407ddb0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][13], RM_B4_32(0x1023e800)); /* 0x407ddb4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][14], RM_B4_32(0x1023e800)); /* 0x407ddb8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][15], RM_B4_32(0x1023e800)); /* 0x407ddbc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][16], RM_B4_32(0x1023e800)); /* 0x407ddc0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][17], RM_B4_32(0x1023e800)); /* 0x407ddc4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][18], RM_B4_32(0x1023e800)); /* 0x407ddc8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][19], RM_B4_32(0x1023e800)); /* 0x407ddcc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][20], RM_B4_32(0x1023e800)); /* 0x407ddd0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][21], RM_B4_32(0x1023e800)); /* 0x407ddd4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][22], RM_B4_32(0x1023e800)); /* 0x407ddd8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][23], RM_B4_32(0x1023e800)); /* 0x407dddc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][24], RM_B4_32(0x1023e800)); /* 0x407dde0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][25], RM_B4_32(0x1023e800)); /* 0x407dde4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][26], RM_B4_32(0x1023e800)); /* 0x407dde8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][27], RM_B4_32(0x1023e800)); /* 0x407ddec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][28], RM_B4_32(0x1023e800)); /* 0x407ddf0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][29], RM_B4_32(0x1023e800)); /* 0x407ddf4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][30], RM_B4_32(0x1023e800)); /* 0x407ddf8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][31], RM_B4_32(0x1023e800)); /* 0x407ddfc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][0], RM_B4_32(0x1023e800)); /* 0x407de00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][1], RM_B4_32(0x1023e800)); /* 0x407de04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][2], RM_B4_32(0x1023e800)); /* 0x407de08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][3], RM_B4_32(0x1023e800)); /* 0x407de0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][4], RM_B4_32(0x1023e800)); /* 0x407de10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][5], RM_B4_32(0x1023e800)); /* 0x407de14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][6], RM_B4_32(0x1023e800)); /* 0x407de18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][7], RM_B4_32(0x1023e800)); /* 0x407de1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][8], RM_B4_32(0x1023e800)); /* 0x407de20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][9], RM_B4_32(0x1023e800)); /* 0x407de24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][10], RM_B4_32(0x1023e800)); /* 0x407de28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][11], RM_B4_32(0x1023e800)); /* 0x407de2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][12], RM_B4_32(0x1023e800)); /* 0x407de30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][13], RM_B4_32(0x1023e800)); /* 0x407de34 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][14], RM_B4_32(0x1023e800)); /* 0x407de38 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][15], RM_B4_32(0x1023e800)); /* 0x407de3c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][16], RM_B4_32(0x1023e800)); /* 0x407de40 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][17], RM_B4_32(0x1023e800)); /* 0x407de44 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][18], RM_B4_32(0x1023e800)); /* 0x407de48 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][19], RM_B4_32(0x1023e800)); /* 0x407de4c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][20], RM_B4_32(0x1023e800)); /* 0x407de50 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][21], RM_B4_32(0x1023e800)); /* 0x407de54 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][22], RM_B4_32(0x1023e800)); /* 0x407de58 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][23], RM_B4_32(0x1023e800)); /* 0x407de5c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][24], RM_B4_32(0x1023e800)); /* 0x407de60 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][25], RM_B4_32(0x1023e800)); /* 0x407de64 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][26], RM_B4_32(0x1023e800)); /* 0x407de68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][27], RM_B4_32(0x1023e800)); /* 0x407de6c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][28], RM_B4_32(0x1023e800)); /* 0x407de70 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][29], RM_B4_32(0x1023e800)); /* 0x407de74 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][30], RM_B4_32(0x1023e800)); /* 0x407de78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][31], RM_B4_32(0x1023e800)); /* 0x407de7c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][0], RM_B4_32(0x1023e800)); /* 0x407de80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][1], RM_B4_32(0x1023e800)); /* 0x407de84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][2], RM_B4_32(0x1023e800)); /* 0x407de88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][3], RM_B4_32(0x1023e800)); /* 0x407de8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][4], RM_B4_32(0x1023e800)); /* 0x407de90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][5], RM_B4_32(0x1023e800)); /* 0x407de94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][6], RM_B4_32(0x1023e800)); /* 0x407de98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][7], RM_B4_32(0x1023e800)); /* 0x407de9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][8], RM_B4_32(0x1023e800)); /* 0x407dea0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][9], RM_B4_32(0x1023e800)); /* 0x407dea4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][10], RM_B4_32(0x1023e800)); /* 0x407dea8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][11], RM_B4_32(0x1023e800)); /* 0x407deac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][12], RM_B4_32(0x1023e800)); /* 0x407deb0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][13], RM_B4_32(0x1023e800)); /* 0x407deb4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][14], RM_B4_32(0x1023e800)); /* 0x407deb8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][15], RM_B4_32(0x1023e800)); /* 0x407debc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][16], RM_B4_32(0x1023e800)); /* 0x407dec0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][17], RM_B4_32(0x1023e800)); /* 0x407dec4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][18], RM_B4_32(0x1023e800)); /* 0x407dec8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][19], RM_B4_32(0x1023e800)); /* 0x407decc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][20], RM_B4_32(0x1023e800)); /* 0x407ded0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][21], RM_B4_32(0x1023e800)); /* 0x407ded4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][22], RM_B4_32(0x1023e800)); /* 0x407ded8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][23], RM_B4_32(0x1023e800)); /* 0x407dedc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][24], RM_B4_32(0x1023e800)); /* 0x407dee0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][25], RM_B4_32(0x1023e800)); /* 0x407dee4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][26], RM_B4_32(0x1023e800)); /* 0x407dee8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][27], RM_B4_32(0x1023e800)); /* 0x407deec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][28], RM_B4_32(0x1023e800)); /* 0x407def0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][29], RM_B4_32(0x1023e800)); /* 0x407def4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][30], RM_B4_32(0x1023e800)); /* 0x407def8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][31], RM_B4_32(0x1023e800)); /* 0x407defc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][0], RM_B4_32(0x1023e800)); /* 0x407df00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][1], RM_B4_32(0x1023e800)); /* 0x407df04 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][2], RM_B4_32(0x1023e800)); /* 0x407df08 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][3], RM_B4_32(0x1023e800)); /* 0x407df0c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][4], RM_B4_32(0x1023e800)); /* 0x407df10 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][5], RM_B4_32(0x1023e800)); /* 0x407df14 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][6], RM_B4_32(0x1023e800)); /* 0x407df18 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][7], RM_B4_32(0x1023e800)); /* 0x407df1c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][8], RM_B4_32(0x1023e800)); /* 0x407df20 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][9], RM_B4_32(0x1023e800)); /* 0x407df24 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][10], RM_B4_32(0x1023e800)); /* 0x407df28 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][11], RM_B4_32(0x1023e800)); /* 0x407df2c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][12], RM_B4_32(0x1023e800)); /* 0x407df30 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][13], RM_B4_32(0x1023e800)); /* 0x407df34 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][14], RM_B4_32(0x1023e800)); /* 0x407df38 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][15], RM_B4_32(0x1023e800)); /* 0x407df3c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][16], RM_B4_32(0x1023e800)); /* 0x407df40 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][17], RM_B4_32(0x1023e800)); /* 0x407df44 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][18], RM_B4_32(0x1023e800)); /* 0x407df48 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][19], RM_B4_32(0x1023e800)); /* 0x407df4c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][20], RM_B4_32(0x1023e800)); /* 0x407df50 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][21], RM_B4_32(0x1023e800)); /* 0x407df54 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][22], RM_B4_32(0x1023e800)); /* 0x407df58 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][23], RM_B4_32(0x1023e800)); /* 0x407df5c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][24], RM_B4_32(0x1023e800)); /* 0x407df60 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][25], RM_B4_32(0x1023e800)); /* 0x407df64 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][26], RM_B4_32(0x1023e800)); /* 0x407df68 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][27], RM_B4_32(0x1023e800)); /* 0x407df6c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][28], RM_B4_32(0x1023e800)); /* 0x407df70 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][29], RM_B4_32(0x1023e800)); /* 0x407df74 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][30], RM_B4_32(0x1023e800)); /* 0x407df78 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][31], RM_B4_32(0x1023e800)); /* 0x407df7c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][0], RM_B4_32(0x1023e800)); /* 0x407df80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][1], RM_B4_32(0x1023e800)); /* 0x407df84 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][2], RM_B4_32(0x1023e800)); /* 0x407df88 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][3], RM_B4_32(0x1023e800)); /* 0x407df8c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][4], RM_B4_32(0x1023e800)); /* 0x407df90 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][5], RM_B4_32(0x1023e800)); /* 0x407df94 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][6], RM_B4_32(0x1023e800)); /* 0x407df98 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][7], RM_B4_32(0x1023e800)); /* 0x407df9c */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][8], RM_B4_32(0x1023e800)); /* 0x407dfa0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][9], RM_B4_32(0x1023e800)); /* 0x407dfa4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][10], RM_B4_32(0x1023e800)); /* 0x407dfa8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][11], RM_B4_32(0x1023e800)); /* 0x407dfac */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][12], RM_B4_32(0x1023e800)); /* 0x407dfb0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][13], RM_B4_32(0x1023e800)); /* 0x407dfb4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][14], RM_B4_32(0x1023e800)); /* 0x407dfb8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][15], RM_B4_32(0x1023e800)); /* 0x407dfbc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][16], RM_B4_32(0x1023e800)); /* 0x407dfc0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][17], RM_B4_32(0x1023e800)); /* 0x407dfc4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][18], RM_B4_32(0x1023e800)); /* 0x407dfc8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][19], RM_B4_32(0x1023e800)); /* 0x407dfcc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][20], RM_B4_32(0x1023e800)); /* 0x407dfd0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][21], RM_B4_32(0x1023e800)); /* 0x407dfd4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][22], RM_B4_32(0x1023e800)); /* 0x407dfd8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][23], RM_B4_32(0x1023e800)); /* 0x407dfdc */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][24], RM_B4_32(0x1023e800)); /* 0x407dfe0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][25], RM_B4_32(0x1023e800)); /* 0x407dfe4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][26], RM_B4_32(0x1023e800)); /* 0x407dfe8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][27], RM_B4_32(0x1023e800)); /* 0x407dfec */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][28], RM_B4_32(0x1023e800)); /* 0x407dff0 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][29], RM_B4_32(0x1023e800)); /* 0x407dff4 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][30], RM_B4_32(0x1023e800)); /* 0x407dff8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][31], RM_B4_32(0x1023e800)); /* 0x407dffc */
  //    tu.OutWord(&mau_reg_map.rams.match.merge.exact_match_delay_config, 0x20000); /* 0x4014c30 */ // REMOVED EMDEL070915
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    //Had this anomalous line which had to edit by hand
    //OutWord(0, 0x401d530, 0x8); /* m_chip_reg.pipes[ 0].mau[ 0].rams.map_alu.row[ 2].vh_xbars.adr_dist_tind_adr_xbar_ctl[0][0] */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].vh_xbars.adr_dist_tind_adr_xbar_ctl[0], 0x8); /* 0x401d530 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.ram_address_mux_ctl[0][5], 0x80); // Fixed. was 0x100 /* 0x401d328 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.ram_address_mux_ctl[1][2], 0x40); // Fixed. was 0x80 /* 0x401f350 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); // Fixed. was 0x300 /* 0x401c300 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.adr_dist_action_data_adr_icxbar_ctl[6], 0x2000); /* 0x40103d8 */
  ActionHvTranslator act_hv_translator;
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0xffff); /* 0x400e884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(6,1,0xffff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xffffaa50); /* 0x400e88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(6,1,0xffffaa50); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0xffff); /* 0x400e884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(6,1,0xffff); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xffffaa50); /* 0x400e88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(6,1,0xffffaa50); // ADDED ACHV070915
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[0].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x40000a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[0].ctl.r_l_action_o_mux_select, 0x8); /* 0x4000080 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[1].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x40001a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[1].ctl.r_l_action_o_mux_select, 0x8); /* 0x4000180 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[2].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x40002a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[2].ctl.r_l_action_o_mux_select, 0x8); /* 0x4000280 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[3].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x40003a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[3].ctl.r_l_action_o_mux_select, 0x8); /* 0x4000380 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[4].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x40004a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[4].ctl.r_l_action_o_mux_select, 0x8); /* 0x4000480 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[5].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x40005a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[5].ctl.r_l_action_o_mux_select, 0x8); /* 0x4000580 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[6].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x40006a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[6].ctl.r_l_action_o_mux_select, 0x8); /* 0x4000680 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[7].ctl.r_action_o_mux_select, r_action_muxsel); /* 0x40007a8 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[7].ctl.r_l_action_o_mux_select, 0x8); /* 0x4000780 */
  act_hv_translator.do_writes(&tu);
    tu.IndirectWrite(0x0200801001fb, 0x00001fbdffffffbf, 0x0000004200000040); /* TCAM[ 0][ 0][507].word1 = 0x2100000020 */
    tu.IndirectWrite(0x0200800094fd, 0x0000000000000000, 0x54fdbc0000001100); /* sram_ 2_ 5: a=0x100800094fd d0=0x0 d1=0x54fdbc0000001100 */
    tu.IndirectWrite(0x02008001a1fb, 0x5555555555555555, 0x5555555555555555); /* sram_ 6_ 8: a=0x1008001a1fb d0=0x5555555555555555 d1=0x5555555555555555 */

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
    RMT_UT_LOG_INFO("Dv3Test::InPkt=%p [DA=%04X%08X]\n", p_in,
                phv_in1->get(Phv::make_word(4,0)), phv_in1->get(Phv::make_word(0,0)));
    Phv *phv_out1 = tu.port_process_inbound(port, phv_in1);
    // Free PHVs}}
    if ((phv_out1 != NULL) && (phv_out1 != phv_in1)) tu.phv_free(phv_out1);
    tu.phv_free(phv_in1);
    */
    
    // PHVPHV
    RMT_UT_LOG_INFO("\n");
    RMT_UT_LOG_INFO("Lookup DV PHV\n");
    Phv *phv_in2 = tu.phv_alloc();
    phv_in2->set_ingress();
    phv_in2->set_phv_by_byte(511, 0xff);
    phv_in2->set_phv_by_byte(510, 0xdf);
    phv_in2->set_phv_by_byte(509, 0xff);
    phv_in2->set_phv_by_byte(508, 0xde);
    phv_in2->set_phv_by_byte(507, 0xff);
    phv_in2->set_phv_by_byte(506, 0xdd);
    phv_in2->set_phv_by_byte(505, 0xff);
    phv_in2->set_phv_by_byte(504, 0xdc);
    phv_in2->set_phv_by_byte(503, 0xff);
    phv_in2->set_phv_by_byte(502, 0xdb);
    phv_in2->set_phv_by_byte(501, 0xff);
    phv_in2->set_phv_by_byte(500, 0xda);
    phv_in2->set_phv_by_byte(499, 0xff);
    phv_in2->set_phv_by_byte(498, 0xd9);
    phv_in2->set_phv_by_byte(497, 0xff);
    phv_in2->set_phv_by_byte(496, 0xd8);
    phv_in2->set_phv_by_byte(495, 0xff);
    phv_in2->set_phv_by_byte(494, 0xd7);
    phv_in2->set_phv_by_byte(493, 0xff);
    phv_in2->set_phv_by_byte(492, 0xd6);
    phv_in2->set_phv_by_byte(491, 0xff);
    phv_in2->set_phv_by_byte(490, 0xd5);
    phv_in2->set_phv_by_byte(489, 0xff);
    phv_in2->set_phv_by_byte(488, 0xd4);
    phv_in2->set_phv_by_byte(487, 0xff);
    phv_in2->set_phv_by_byte(486, 0xd3);
    phv_in2->set_phv_by_byte(485, 0xff);
    phv_in2->set_phv_by_byte(484, 0xd2);
    phv_in2->set_phv_by_byte(483, 0xff);
    phv_in2->set_phv_by_byte(482, 0xd1);
    phv_in2->set_phv_by_byte(481, 0xff);
    phv_in2->set_phv_by_byte(480, 0xd0);
    phv_in2->set_phv_by_byte(479, 0xff);
    phv_in2->set_phv_by_byte(478, 0xcf);
    phv_in2->set_phv_by_byte(477, 0xff);
    phv_in2->set_phv_by_byte(476, 0xce);
    phv_in2->set_phv_by_byte(475, 0xff);
    phv_in2->set_phv_by_byte(474, 0xcd);
    phv_in2->set_phv_by_byte(473, 0xff);
    phv_in2->set_phv_by_byte(472, 0xcc);
    phv_in2->set_phv_by_byte(471, 0xff);
    phv_in2->set_phv_by_byte(470, 0xcb);
    phv_in2->set_phv_by_byte(469, 0xff);
    phv_in2->set_phv_by_byte(468, 0xca);
    phv_in2->set_phv_by_byte(467, 0xff);
    phv_in2->set_phv_by_byte(466, 0xc9);
    phv_in2->set_phv_by_byte(465, 0xff);
    phv_in2->set_phv_by_byte(464, 0xc8);
    phv_in2->set_phv_by_byte(463, 0xff);
    phv_in2->set_phv_by_byte(462, 0xc7);
    phv_in2->set_phv_by_byte(461, 0xff);
    phv_in2->set_phv_by_byte(460, 0xc6);
    phv_in2->set_phv_by_byte(459, 0xff);
    phv_in2->set_phv_by_byte(458, 0xc5);
    phv_in2->set_phv_by_byte(457, 0xff);
    phv_in2->set_phv_by_byte(456, 0xc4);
    phv_in2->set_phv_by_byte(455, 0xff);
    phv_in2->set_phv_by_byte(454, 0xc3);
    phv_in2->set_phv_by_byte(453, 0xff);
    phv_in2->set_phv_by_byte(452, 0xc2);
    phv_in2->set_phv_by_byte(451, 0xff);
    phv_in2->set_phv_by_byte(450, 0xc1);
    phv_in2->set_phv_by_byte(449, 0xff);
    phv_in2->set_phv_by_byte(448, 0xc0);
    phv_in2->set_phv_by_byte(447, 0xff);
    phv_in2->set_phv_by_byte(446, 0xbf);
    phv_in2->set_phv_by_byte(445, 0xff);
    phv_in2->set_phv_by_byte(444, 0xbe);
    phv_in2->set_phv_by_byte(443, 0xff);
    phv_in2->set_phv_by_byte(442, 0xbd);
    phv_in2->set_phv_by_byte(441, 0xff);
    phv_in2->set_phv_by_byte(440, 0xbc);
    phv_in2->set_phv_by_byte(439, 0xff);
    phv_in2->set_phv_by_byte(438, 0xbb);
    phv_in2->set_phv_by_byte(437, 0xff);
    phv_in2->set_phv_by_byte(436, 0xba);
    phv_in2->set_phv_by_byte(435, 0xff);
    phv_in2->set_phv_by_byte(434, 0xb9);
    phv_in2->set_phv_by_byte(433, 0xff);
    phv_in2->set_phv_by_byte(432, 0xb8);
    phv_in2->set_phv_by_byte(431, 0xff);
    phv_in2->set_phv_by_byte(430, 0xb7);
    phv_in2->set_phv_by_byte(429, 0xff);
    phv_in2->set_phv_by_byte(428, 0xb6);
    phv_in2->set_phv_by_byte(427, 0xff);
    phv_in2->set_phv_by_byte(426, 0xb5);
    phv_in2->set_phv_by_byte(425, 0xff);
    phv_in2->set_phv_by_byte(424, 0xb4);
    phv_in2->set_phv_by_byte(423, 0xff);
    phv_in2->set_phv_by_byte(422, 0xb3);
    phv_in2->set_phv_by_byte(421, 0xff);
    phv_in2->set_phv_by_byte(420, 0xb2);
    phv_in2->set_phv_by_byte(419, 0xff);
    phv_in2->set_phv_by_byte(418, 0xb1);
    phv_in2->set_phv_by_byte(417, 0xff);
    phv_in2->set_phv_by_byte(416, 0xb0);
    phv_in2->set_phv_by_byte(415, 0xff);
    phv_in2->set_phv_by_byte(414, 0xaf);
    phv_in2->set_phv_by_byte(413, 0xff);
    phv_in2->set_phv_by_byte(412, 0xae);
    phv_in2->set_phv_by_byte(411, 0xff);
    phv_in2->set_phv_by_byte(410, 0xad);
    phv_in2->set_phv_by_byte(409, 0xff);
    phv_in2->set_phv_by_byte(408, 0xac);
    phv_in2->set_phv_by_byte(407, 0xff);
    phv_in2->set_phv_by_byte(406, 0xab);
    phv_in2->set_phv_by_byte(405, 0xff);
    phv_in2->set_phv_by_byte(404, 0xaa);
    phv_in2->set_phv_by_byte(403, 0xff);
    phv_in2->set_phv_by_byte(402, 0xa9);
    phv_in2->set_phv_by_byte(401, 0xff);
    phv_in2->set_phv_by_byte(400, 0xa8);
    phv_in2->set_phv_by_byte(399, 0xff);
    phv_in2->set_phv_by_byte(398, 0xa7);
    phv_in2->set_phv_by_byte(397, 0xff);
    phv_in2->set_phv_by_byte(396, 0xa6);
    phv_in2->set_phv_by_byte(395, 0xff);
    phv_in2->set_phv_by_byte(394, 0xa5);
    phv_in2->set_phv_by_byte(393, 0xff);
    phv_in2->set_phv_by_byte(392, 0xa4);
    phv_in2->set_phv_by_byte(391, 0xff);
    phv_in2->set_phv_by_byte(390, 0xa3);
    phv_in2->set_phv_by_byte(389, 0xff);
    phv_in2->set_phv_by_byte(388, 0xa2);
    phv_in2->set_phv_by_byte(387, 0xff);
    phv_in2->set_phv_by_byte(386, 0xa1);
    phv_in2->set_phv_by_byte(385, 0xff);
    phv_in2->set_phv_by_byte(384, 0xa0);
    phv_in2->set_phv_by_byte(383, 0xff);
    phv_in2->set_phv_by_byte(382, 0x9f);
    phv_in2->set_phv_by_byte(381, 0xff);
    phv_in2->set_phv_by_byte(380, 0x9e);
    phv_in2->set_phv_by_byte(379, 0xff);
    phv_in2->set_phv_by_byte(378, 0x9d);
    phv_in2->set_phv_by_byte(377, 0xff);
    phv_in2->set_phv_by_byte(376, 0x9c);
    phv_in2->set_phv_by_byte(375, 0xff);
    phv_in2->set_phv_by_byte(374, 0x9b);
    phv_in2->set_phv_by_byte(373, 0xff);
    phv_in2->set_phv_by_byte(372, 0x9a);
    phv_in2->set_phv_by_byte(371, 0xff);
    phv_in2->set_phv_by_byte(370, 0x99);
    phv_in2->set_phv_by_byte(369, 0xff);
    phv_in2->set_phv_by_byte(368, 0x98);
    phv_in2->set_phv_by_byte(367, 0xff);
    phv_in2->set_phv_by_byte(366, 0x97);
    phv_in2->set_phv_by_byte(365, 0xff);
    phv_in2->set_phv_by_byte(364, 0x96);
    phv_in2->set_phv_by_byte(363, 0xff);
    phv_in2->set_phv_by_byte(362, 0x95);
    phv_in2->set_phv_by_byte(361, 0xff);
    phv_in2->set_phv_by_byte(360, 0x94);
    phv_in2->set_phv_by_byte(359, 0xff);
    phv_in2->set_phv_by_byte(358, 0x93);
    phv_in2->set_phv_by_byte(357, 0xff);
    phv_in2->set_phv_by_byte(356, 0x92);
    phv_in2->set_phv_by_byte(355, 0xff);
    phv_in2->set_phv_by_byte(354, 0x91);
    phv_in2->set_phv_by_byte(353, 0xff);
    phv_in2->set_phv_by_byte(352, 0x90);
    phv_in2->set_phv_by_byte(351, 0xff);
    phv_in2->set_phv_by_byte(350, 0x8f);
    phv_in2->set_phv_by_byte(349, 0xff);
    phv_in2->set_phv_by_byte(348, 0x8e);
    phv_in2->set_phv_by_byte(347, 0xff);
    phv_in2->set_phv_by_byte(346, 0x8d);
    phv_in2->set_phv_by_byte(345, 0xff);
    phv_in2->set_phv_by_byte(344, 0x8c);
    phv_in2->set_phv_by_byte(343, 0xff);
    phv_in2->set_phv_by_byte(342, 0x8b);
    phv_in2->set_phv_by_byte(341, 0xff);
    phv_in2->set_phv_by_byte(340, 0x8a);
    phv_in2->set_phv_by_byte(339, 0xff);
    phv_in2->set_phv_by_byte(338, 0x89);
    phv_in2->set_phv_by_byte(337, 0xff);
    phv_in2->set_phv_by_byte(336, 0x88);
    phv_in2->set_phv_by_byte(335, 0xff);
    phv_in2->set_phv_by_byte(334, 0x87);
    phv_in2->set_phv_by_byte(333, 0xff);
    phv_in2->set_phv_by_byte(332, 0x86);
    phv_in2->set_phv_by_byte(331, 0xff);
    phv_in2->set_phv_by_byte(330, 0x85);
    phv_in2->set_phv_by_byte(329, 0xff);
    phv_in2->set_phv_by_byte(328, 0x84);
    phv_in2->set_phv_by_byte(327, 0xff);
    phv_in2->set_phv_by_byte(326, 0x83);
    phv_in2->set_phv_by_byte(325, 0xff);
    phv_in2->set_phv_by_byte(324, 0x82);
    phv_in2->set_phv_by_byte(323, 0xff);
    phv_in2->set_phv_by_byte(322, 0x81);
    phv_in2->set_phv_by_byte(321, 0x00);
    phv_in2->set_phv_by_byte(320, 0x80);
    phv_in2->set_phv_by_byte(319, 0x7f);
    phv_in2->set_phv_by_byte(318, 0x7e);
    phv_in2->set_phv_by_byte(317, 0x7d);
    phv_in2->set_phv_by_byte(316, 0x7c);
    phv_in2->set_phv_by_byte(315, 0x7b);
    phv_in2->set_phv_by_byte(314, 0x7a);
    phv_in2->set_phv_by_byte(313, 0x79);
    phv_in2->set_phv_by_byte(312, 0x78);
    phv_in2->set_phv_by_byte(311, 0x77);
    phv_in2->set_phv_by_byte(310, 0x76);
    phv_in2->set_phv_by_byte(309, 0x75);
    phv_in2->set_phv_by_byte(308, 0x74);
    phv_in2->set_phv_by_byte(307, 0x73);
    phv_in2->set_phv_by_byte(306, 0x72);
    phv_in2->set_phv_by_byte(305, 0x71);
    phv_in2->set_phv_by_byte(304, 0x70);
    phv_in2->set_phv_by_byte(303, 0x6f);
    phv_in2->set_phv_by_byte(302, 0x6e);
    phv_in2->set_phv_by_byte(301, 0x6d);
    phv_in2->set_phv_by_byte(300, 0x6c);
    phv_in2->set_phv_by_byte(299, 0x6b);
    phv_in2->set_phv_by_byte(298, 0x6a);
    phv_in2->set_phv_by_byte(297, 0x69);
    phv_in2->set_phv_by_byte(296, 0x68);
    phv_in2->set_phv_by_byte(295, 0x67);
    phv_in2->set_phv_by_byte(294, 0x66);
    phv_in2->set_phv_by_byte(293, 0x65);
    phv_in2->set_phv_by_byte(292, 0x64);
    phv_in2->set_phv_by_byte(291, 0x63);
    phv_in2->set_phv_by_byte(290, 0x62);
    phv_in2->set_phv_by_byte(289, 0x61);
    phv_in2->set_phv_by_byte(288, 0x60);
    phv_in2->set_phv_by_byte(287, 0x5f);
    phv_in2->set_phv_by_byte(286, 0x5e);
    phv_in2->set_phv_by_byte(285, 0x5d);
    phv_in2->set_phv_by_byte(284, 0x5c);
    phv_in2->set_phv_by_byte(283, 0x5b);
    phv_in2->set_phv_by_byte(282, 0x5a);
    phv_in2->set_phv_by_byte(281, 0x59);
    phv_in2->set_phv_by_byte(280, 0x58);
    phv_in2->set_phv_by_byte(279, 0x57);
    phv_in2->set_phv_by_byte(278, 0x56);
    phv_in2->set_phv_by_byte(277, 0x55);
    phv_in2->set_phv_by_byte(276, 0x54);
    phv_in2->set_phv_by_byte(275, 0x53);
    phv_in2->set_phv_by_byte(274, 0x52);
    phv_in2->set_phv_by_byte(273, 0x51);
    phv_in2->set_phv_by_byte(272, 0x50);
    phv_in2->set_phv_by_byte(271, 0x4f);
    phv_in2->set_phv_by_byte(270, 0x4e);
    phv_in2->set_phv_by_byte(269, 0x4d);
    phv_in2->set_phv_by_byte(268, 0x4c);
    phv_in2->set_phv_by_byte(267, 0x4b);
    phv_in2->set_phv_by_byte(266, 0x4a);
    phv_in2->set_phv_by_byte(265, 0x49);
    phv_in2->set_phv_by_byte(264, 0x48);
    phv_in2->set_phv_by_byte(263, 0x47);
    phv_in2->set_phv_by_byte(262, 0x46);
    phv_in2->set_phv_by_byte(261, 0x45);
    phv_in2->set_phv_by_byte(260, 0x44);
    phv_in2->set_phv_by_byte(259, 0x43);
    phv_in2->set_phv_by_byte(258, 0x42);
    phv_in2->set_phv_by_byte(257, 0x41);
    phv_in2->set_phv_by_byte(256, 0x40);
    phv_in2->set_phv_by_byte(255, 0x00);
    phv_in2->set_phv_by_byte(254, 0x00);
    phv_in2->set_phv_by_byte(253, 0x00);
    phv_in2->set_phv_by_byte(252, 0x3f);
    phv_in2->set_phv_by_byte(251, 0x00);
    phv_in2->set_phv_by_byte(250, 0x00);
    phv_in2->set_phv_by_byte(249, 0x00);
    phv_in2->set_phv_by_byte(248, 0x3e);
    phv_in2->set_phv_by_byte(247, 0x00);
    phv_in2->set_phv_by_byte(246, 0x00);
    phv_in2->set_phv_by_byte(245, 0x00);
    phv_in2->set_phv_by_byte(244, 0x3d);
    phv_in2->set_phv_by_byte(243, 0x00);
    phv_in2->set_phv_by_byte(242, 0x00);
    phv_in2->set_phv_by_byte(241, 0x00);
    phv_in2->set_phv_by_byte(240, 0x3c);
    phv_in2->set_phv_by_byte(239, 0x00);
    phv_in2->set_phv_by_byte(238, 0x00);
    phv_in2->set_phv_by_byte(237, 0x00);
    phv_in2->set_phv_by_byte(236, 0x3b);
    phv_in2->set_phv_by_byte(235, 0x00);
    phv_in2->set_phv_by_byte(234, 0x00);
    phv_in2->set_phv_by_byte(233, 0x00);
    phv_in2->set_phv_by_byte(232, 0x3a);
    phv_in2->set_phv_by_byte(231, 0x00);
    phv_in2->set_phv_by_byte(230, 0x00);
    phv_in2->set_phv_by_byte(229, 0x00);
    phv_in2->set_phv_by_byte(228, 0x39);
    phv_in2->set_phv_by_byte(227, 0x00);
    phv_in2->set_phv_by_byte(226, 0x00);
    phv_in2->set_phv_by_byte(225, 0x00);
    phv_in2->set_phv_by_byte(224, 0x38);
    phv_in2->set_phv_by_byte(223, 0x00);
    phv_in2->set_phv_by_byte(222, 0x00);
    phv_in2->set_phv_by_byte(221, 0x00);
    phv_in2->set_phv_by_byte(220, 0x37);
    phv_in2->set_phv_by_byte(219, 0x00);
    phv_in2->set_phv_by_byte(218, 0x00);
    phv_in2->set_phv_by_byte(217, 0x00);
    phv_in2->set_phv_by_byte(216, 0x36);
    phv_in2->set_phv_by_byte(215, 0x00);
    phv_in2->set_phv_by_byte(214, 0x00);
    phv_in2->set_phv_by_byte(213, 0x00);
    phv_in2->set_phv_by_byte(212, 0x35);
    phv_in2->set_phv_by_byte(211, 0x00);
    phv_in2->set_phv_by_byte(210, 0x00);
    phv_in2->set_phv_by_byte(209, 0x00);
    phv_in2->set_phv_by_byte(208, 0x34);
    phv_in2->set_phv_by_byte(207, 0x00);
    phv_in2->set_phv_by_byte(206, 0x00);
    phv_in2->set_phv_by_byte(205, 0x00);
    phv_in2->set_phv_by_byte(204, 0x33);
    phv_in2->set_phv_by_byte(203, 0x00);
    phv_in2->set_phv_by_byte(202, 0x00);
    phv_in2->set_phv_by_byte(201, 0x00);
    phv_in2->set_phv_by_byte(200, 0x32);
    phv_in2->set_phv_by_byte(199, 0x00);
    phv_in2->set_phv_by_byte(198, 0x00);
    phv_in2->set_phv_by_byte(197, 0x00);
    phv_in2->set_phv_by_byte(196, 0x31);
    phv_in2->set_phv_by_byte(195, 0x00);
    phv_in2->set_phv_by_byte(194, 0x00);
    phv_in2->set_phv_by_byte(193, 0x00);
    phv_in2->set_phv_by_byte(192, 0x30);
    phv_in2->set_phv_by_byte(191, 0x00);
    phv_in2->set_phv_by_byte(190, 0x00);
    phv_in2->set_phv_by_byte(189, 0x00);
    phv_in2->set_phv_by_byte(188, 0x2f);
    phv_in2->set_phv_by_byte(187, 0x00);
    phv_in2->set_phv_by_byte(186, 0x00);
    phv_in2->set_phv_by_byte(185, 0x00);
    phv_in2->set_phv_by_byte(184, 0x2e);
    phv_in2->set_phv_by_byte(183, 0x00);
    phv_in2->set_phv_by_byte(182, 0x00);
    phv_in2->set_phv_by_byte(181, 0x00);
    phv_in2->set_phv_by_byte(180, 0x2d);
    phv_in2->set_phv_by_byte(179, 0x00);
    phv_in2->set_phv_by_byte(178, 0x00);
    phv_in2->set_phv_by_byte(177, 0x00);
    phv_in2->set_phv_by_byte(176, 0x2c);
    phv_in2->set_phv_by_byte(175, 0x00);
    phv_in2->set_phv_by_byte(174, 0x00);
    phv_in2->set_phv_by_byte(173, 0x00);
    phv_in2->set_phv_by_byte(172, 0x2b);
    phv_in2->set_phv_by_byte(171, 0x00);
    phv_in2->set_phv_by_byte(170, 0x00);
    phv_in2->set_phv_by_byte(169, 0x00);
    phv_in2->set_phv_by_byte(168, 0x2a);
    phv_in2->set_phv_by_byte(167, 0x00);
    phv_in2->set_phv_by_byte(166, 0x00);
    phv_in2->set_phv_by_byte(165, 0x00);
    phv_in2->set_phv_by_byte(164, 0x29);
    phv_in2->set_phv_by_byte(163, 0x00);
    phv_in2->set_phv_by_byte(162, 0x00);
    phv_in2->set_phv_by_byte(161, 0x00);
    phv_in2->set_phv_by_byte(160, 0x28);
    phv_in2->set_phv_by_byte(159, 0x00);
    phv_in2->set_phv_by_byte(158, 0x00);
    phv_in2->set_phv_by_byte(157, 0x00);
    phv_in2->set_phv_by_byte(156, 0x27);
    phv_in2->set_phv_by_byte(155, 0x00);
    phv_in2->set_phv_by_byte(154, 0x00);
    phv_in2->set_phv_by_byte(153, 0x00);
    phv_in2->set_phv_by_byte(152, 0x26);
    phv_in2->set_phv_by_byte(151, 0x00);
    phv_in2->set_phv_by_byte(150, 0x00);
    phv_in2->set_phv_by_byte(149, 0x00);
    phv_in2->set_phv_by_byte(148, 0x25);
    phv_in2->set_phv_by_byte(147, 0x00);
    phv_in2->set_phv_by_byte(146, 0x00);
    phv_in2->set_phv_by_byte(145, 0x00);
    phv_in2->set_phv_by_byte(144, 0x24);
    phv_in2->set_phv_by_byte(143, 0x00);
    phv_in2->set_phv_by_byte(142, 0x00);
    phv_in2->set_phv_by_byte(141, 0x00);
    phv_in2->set_phv_by_byte(140, 0x23);
    phv_in2->set_phv_by_byte(139, 0x00);
    phv_in2->set_phv_by_byte(138, 0x00);
    phv_in2->set_phv_by_byte(137, 0x00);
    phv_in2->set_phv_by_byte(136, 0x22);
    phv_in2->set_phv_by_byte(135, 0x00);
    phv_in2->set_phv_by_byte(134, 0x00);
    phv_in2->set_phv_by_byte(133, 0x00);
    phv_in2->set_phv_by_byte(132, 0x21);
    phv_in2->set_phv_by_byte(131, 0x00);
    phv_in2->set_phv_by_byte(130, 0x00);
    phv_in2->set_phv_by_byte(129, 0x00);
    phv_in2->set_phv_by_byte(128, 0x20);
    phv_in2->set_phv_by_byte(127, 0x00);
    phv_in2->set_phv_by_byte(126, 0x00);
    phv_in2->set_phv_by_byte(125, 0x00);
    phv_in2->set_phv_by_byte(124, 0x1f);
    phv_in2->set_phv_by_byte(123, 0x00);
    phv_in2->set_phv_by_byte(122, 0x00);
    phv_in2->set_phv_by_byte(121, 0x00);
    phv_in2->set_phv_by_byte(120, 0x1e);
    phv_in2->set_phv_by_byte(119, 0x00);
    phv_in2->set_phv_by_byte(118, 0x00);
    phv_in2->set_phv_by_byte(117, 0x00);
    phv_in2->set_phv_by_byte(116, 0x1d);
    phv_in2->set_phv_by_byte(115, 0x00);
    phv_in2->set_phv_by_byte(114, 0x00);
    phv_in2->set_phv_by_byte(113, 0x00);
    phv_in2->set_phv_by_byte(112, 0x1c);
    phv_in2->set_phv_by_byte(111, 0x00);
    phv_in2->set_phv_by_byte(110, 0x00);
    phv_in2->set_phv_by_byte(109, 0x00);
    phv_in2->set_phv_by_byte(108, 0x1b);
    phv_in2->set_phv_by_byte(107, 0x00);
    phv_in2->set_phv_by_byte(106, 0x00);
    phv_in2->set_phv_by_byte(105, 0x00);
    phv_in2->set_phv_by_byte(104, 0x1a);
    phv_in2->set_phv_by_byte(103, 0x00);
    phv_in2->set_phv_by_byte(102, 0x00);
    phv_in2->set_phv_by_byte(101, 0x00);
    phv_in2->set_phv_by_byte(100, 0x19);
    phv_in2->set_phv_by_byte( 99, 0x00);
    phv_in2->set_phv_by_byte( 98, 0x00);
    phv_in2->set_phv_by_byte( 97, 0x00);
    phv_in2->set_phv_by_byte( 96, 0x18);
    phv_in2->set_phv_by_byte( 95, 0x00);
    phv_in2->set_phv_by_byte( 94, 0x00);
    phv_in2->set_phv_by_byte( 93, 0x00);
    phv_in2->set_phv_by_byte( 92, 0x17);
    phv_in2->set_phv_by_byte( 91, 0x00);
    phv_in2->set_phv_by_byte( 90, 0x00);
    phv_in2->set_phv_by_byte( 89, 0x00);
    phv_in2->set_phv_by_byte( 88, 0x16);
    phv_in2->set_phv_by_byte( 87, 0x00);
    phv_in2->set_phv_by_byte( 86, 0x00);
    phv_in2->set_phv_by_byte( 85, 0x00);
    phv_in2->set_phv_by_byte( 84, 0x15);
    phv_in2->set_phv_by_byte( 83, 0x00);
    phv_in2->set_phv_by_byte( 82, 0x00);
    phv_in2->set_phv_by_byte( 81, 0x00);
    phv_in2->set_phv_by_byte( 80, 0x14);
    phv_in2->set_phv_by_byte( 79, 0x00);
    phv_in2->set_phv_by_byte( 78, 0x00);
    phv_in2->set_phv_by_byte( 77, 0x00);
    phv_in2->set_phv_by_byte( 76, 0x13);
    phv_in2->set_phv_by_byte( 75, 0x00);
    phv_in2->set_phv_by_byte( 74, 0x00);
    phv_in2->set_phv_by_byte( 73, 0x00);
    phv_in2->set_phv_by_byte( 72, 0x12);
    phv_in2->set_phv_by_byte( 71, 0x00);
    phv_in2->set_phv_by_byte( 70, 0x00);
    phv_in2->set_phv_by_byte( 69, 0x00);
    phv_in2->set_phv_by_byte( 68, 0x11);
    phv_in2->set_phv_by_byte( 67, 0x00);
    phv_in2->set_phv_by_byte( 66, 0x00);
    phv_in2->set_phv_by_byte( 65, 0x00);
    phv_in2->set_phv_by_byte( 64, 0x10);
    phv_in2->set_phv_by_byte( 63, 0x00);
    phv_in2->set_phv_by_byte( 62, 0x00);
    phv_in2->set_phv_by_byte( 61, 0x00);
    phv_in2->set_phv_by_byte( 60, 0x0f);
    phv_in2->set_phv_by_byte( 59, 0x00);
    phv_in2->set_phv_by_byte( 58, 0x00);
    phv_in2->set_phv_by_byte( 57, 0x00);
    phv_in2->set_phv_by_byte( 56, 0x0e);
    phv_in2->set_phv_by_byte( 55, 0x00);
    phv_in2->set_phv_by_byte( 54, 0x00);
    phv_in2->set_phv_by_byte( 53, 0x00);
    phv_in2->set_phv_by_byte( 52, 0x0d);
    phv_in2->set_phv_by_byte( 51, 0x00);
    phv_in2->set_phv_by_byte( 50, 0x00);
    phv_in2->set_phv_by_byte( 49, 0x00);
    phv_in2->set_phv_by_byte( 48, 0x0c);
    phv_in2->set_phv_by_byte( 47, 0x00);
    phv_in2->set_phv_by_byte( 46, 0x00);
    phv_in2->set_phv_by_byte( 45, 0x00);
    phv_in2->set_phv_by_byte( 44, 0x0b);
    phv_in2->set_phv_by_byte( 43, 0x00);
    phv_in2->set_phv_by_byte( 42, 0x00);
    phv_in2->set_phv_by_byte( 41, 0x00);
    phv_in2->set_phv_by_byte( 40, 0x0a);
    phv_in2->set_phv_by_byte( 39, 0x00);
    phv_in2->set_phv_by_byte( 38, 0x00);
    phv_in2->set_phv_by_byte( 37, 0x00);
    phv_in2->set_phv_by_byte( 36, 0x09);
    phv_in2->set_phv_by_byte( 35, 0x00);
    phv_in2->set_phv_by_byte( 34, 0x00);
    phv_in2->set_phv_by_byte( 33, 0x00);
    phv_in2->set_phv_by_byte( 32, 0x08);
    phv_in2->set_phv_by_byte( 31, 0x00);
    phv_in2->set_phv_by_byte( 30, 0x00);
    phv_in2->set_phv_by_byte( 29, 0x00);
    phv_in2->set_phv_by_byte( 28, 0x07);
    phv_in2->set_phv_by_byte( 27, 0x00);
    phv_in2->set_phv_by_byte( 26, 0x00);
    phv_in2->set_phv_by_byte( 25, 0x00);
    phv_in2->set_phv_by_byte( 24, 0x06);
    phv_in2->set_phv_by_byte( 23, 0x00);
    phv_in2->set_phv_by_byte( 22, 0x00);
    phv_in2->set_phv_by_byte( 21, 0x00);
    phv_in2->set_phv_by_byte( 20, 0x05);
    phv_in2->set_phv_by_byte( 19, 0x00);
    phv_in2->set_phv_by_byte( 18, 0x00);
    phv_in2->set_phv_by_byte( 17, 0x00);
    phv_in2->set_phv_by_byte( 16, 0x04);
    phv_in2->set_phv_by_byte( 15, 0x00);
    phv_in2->set_phv_by_byte( 14, 0x00);
    phv_in2->set_phv_by_byte( 13, 0x00);
    phv_in2->set_phv_by_byte( 12, 0x03);
    phv_in2->set_phv_by_byte( 11, 0x00);
    phv_in2->set_phv_by_byte( 10, 0x00);
    phv_in2->set_phv_by_byte(  9, 0x00);
    phv_in2->set_phv_by_byte(  8, 0x02);
    phv_in2->set_phv_by_byte(  7, 0x00);
    phv_in2->set_phv_by_byte(  6, 0x00);
    phv_in2->set_phv_by_byte(  5, 0x00);
    phv_in2->set_phv_by_byte(  4, 0x01);
    phv_in2->set_phv_by_byte(  3, 0x00);
    phv_in2->set_phv_by_byte(  2, 0x00);
    phv_in2->set_phv_by_byte(  1, 0x00);
    phv_in2->set_phv_by_byte(  0, 0x00);

    
    //EXPECT_EQ(0x00000000, phv_in2->get(0));

    // Uncomment below to up the debug output
    //flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Switch on all parse loop output
    uint64_t pipes2 = UINT64_C(1) << pipe;
    uint64_t types2 = UINT64_C(1) << RmtTypes::kRmtTypeParser;
    uint64_t flags2 = RmtDebug::kRmtDebugParserParseLoop;
    tu.update_log_flags(pipes2, 0, types2, 0, 0, flags2, ALL);

    
    Phv *phv_out2 = tu.port_process_inbound(port, phv_in2);
    EXPECT_EQ(0x55555555, phv_out2->get(0)); // was 0x0 in input phv
    EXPECT_EQ(0x55555555, phv_out2->get(1)); // was 0x1 in input phv
    EXPECT_EQ(0x55555555, phv_out2->get(2)); // was 0x2 in input phv
    EXPECT_EQ(0x55555555, phv_out2->get(3)); // was 0x3 in input phv
    EXPECT_EQ(0x55555555, phv_out2->get(4)); // was 0x4 in input phv
    EXPECT_EQ(0x55555555, phv_out2->get(5)); // was 0x5 in input phv
    EXPECT_EQ(0x55555555, phv_out2->get(6)); // was 0x6 in input phv
    EXPECT_EQ(0x55555555, phv_out2->get(7)); // was 0x7 in input phv
    EXPECT_EQ(0x55555555, phv_out2->get(8)); // was 0x8 in input phv
    EXPECT_EQ(0x55555555, phv_out2->get(9)); // was 0x9 in input phv
    EXPECT_EQ(0x55555555, phv_out2->get(10)); // was 0xa in input phv
    EXPECT_EQ(0x55555555, phv_out2->get(11)); // was 0xb in input phv
    EXPECT_EQ(0x55555555, phv_out2->get(12)); // was 0xc in input phv
    EXPECT_EQ(0x55555555, phv_out2->get(13)); // was 0xd in input phv
    EXPECT_EQ(0x55555555, phv_out2->get(14)); // was 0xe in input phv
    EXPECT_EQ(0x55555555, phv_out2->get(15)); // was 0xf in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(16)); // was 0x10 in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(17)); // was 0x11 in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(18)); // was 0x12 in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(19)); // was 0x13 in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(20)); // was 0x14 in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(21)); // was 0x15 in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(22)); // was 0x16 in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(23)); // was 0x17 in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(24)); // was 0x18 in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(25)); // was 0x19 in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(26)); // was 0x1a in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(27)); // was 0x1b in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(28)); // was 0x1c in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(29)); // was 0x1d in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(30)); // was 0x1e in input phv
    //EXPECT_EQ(0x55555555, phv_out2->get(31)); // was 0x1f in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(32)); // was 0x20 in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(33)); // was 0x21 in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(34)); // was 0x22 in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(35)); // was 0x23 in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(36)); // was 0x24 in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(37)); // was 0x25 in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(38)); // was 0x26 in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(39)); // was 0x27 in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(40)); // was 0x28 in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(41)); // was 0x29 in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(42)); // was 0x2a in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(43)); // was 0x2b in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(44)); // was 0x2c in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(45)); // was 0x2d in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(46)); // was 0x2e in input phv
    EXPECT_EQ(0x55555575, phv_out2->get(47)); // was 0x2f in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(48)); // was 0x30 in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(49)); // was 0x31 in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(50)); // was 0x32 in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(51)); // was 0x33 in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(52)); // was 0x34 in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(53)); // was 0x35 in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(54)); // was 0x36 in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(55)); // was 0x37 in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(56)); // was 0x38 in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(57)); // was 0x39 in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(58)); // was 0x3a in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(59)); // was 0x3b in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(60)); // was 0x3c in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(61)); // was 0x3d in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(62)); // was 0x3e in input phv
    //EXPECT_EQ(0x55555575, phv_out2->get(63)); // was 0x3f in input phv

    // Check phv_out2
    int i;
    for (i = 0; i < 32; i++) {
      uint32_t actual = phv_out2->get(i);
      //EXPECT_EQ(0x55555555, actual);
      if (i == 0) printf("EXPECTED=0x%08x ACTUAL=0x%08x\n", 0x55555555, actual);
    }
    for (i = 32; i < 64; i++) {
      uint32_t actual = phv_out2->get(i);
      //EXPECT_EQ(0x55555575, actual);
      if (i == 32) printf("EXPECTED=0x%08x ACTUAL=0x%08x\n", 0x55555575, actual);
    }

    // Check other PHVs stayed the same
    EXPECT_EQ(64, phv_out2->get(64));
    EXPECT_EQ(96, phv_out2->get(96));
    EXPECT_EQ(128, phv_out2->get(128));
    
    // Free PHVs

    tu.finish_test();
    tu.quieten_log_flags();
    if ((phv_out2 != NULL) && (phv_out2 != phv_in2)) tu.phv_free(phv_out2);
    tu.phv_free(phv_in2);
}


}
