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

// XXX -> test_dv42.cpp
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

  bool dv42_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv42Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv42_print) RMT_UT_LOG_INFO("test_dv42_packet1()\n");
    
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
    tu.set_dv_test(42);
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
    tu.OutWord(&mau_reg_map.dp.action_output_delay[1], 0xb); /* 0x2060068 */
    tu.OutWord(&mau_reg_map.dp.match_ie_input_mux_sel, 0x3); /* 0x2060128 */
    tu.OutWord(&mau_reg_map.dp.phv_fifo_enable, 0xc); /* 0x2060138 */
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
    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.unitram_config[0][0], 0x1482); /* 0x200ef80 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.unitram_config[1][1], 0x147a); /* 0x200efc8 */
    tu.OutWord(&mau_reg_map.rams.array.row[6].ram[0].unit_ram_ctl, 0x20); /* 0x203e018 */
    tu.OutWord(&mau_reg_map.rams.array.row[6].ram[7].unit_ram_ctl, 0x20); /* 0x203e398 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.unitram_config[0][0], 0x25569); /* 0x2008f80 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.unitram_config[0][0], 0x255a1); /* 0x2008f84 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_mask[0], 0x7ffffff); /* 0x2038000 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_mask[1], 0xffffffe); /* 0x2038004 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].unit_ram_ctl, 0x60bf); /* 0x2038018 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].unit_ram_ctl, 0x60bf); /* 0x203801c */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_ram_vpn, 0x225ca); /* 0x2038044 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_nibble_s1q0_enable, 0xffffffff); /* 0x203804c */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_nibble_s0q1_enable, 0x3fffffff); /* 0x2038048 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_bytemask[2], 0x1feff); /* 0x2038068 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_bytemask[3], 0x2fdff); /* 0x203806c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][0], 0xb); /* 0x2040f00 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][0], 0x5); /* 0x2040c00 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][1], 0x3); /* 0x2040c04 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][2], 0x5); /* 0x2040c08 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][3], 0x0); /* 0x2040c0c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][4], 0x8); /* 0x2040c10 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][5], 0x6); /* 0x2040c14 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][6], 0x5); /* 0x2040c18 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][7], 0x7); /* 0x2040c1c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][0], 0x8); /* 0x2040f80 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][0], 0xe); /* 0x2040e00 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][1], 0xd); /* 0x2040f84 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][1], 0xd); /* 0x2040e04 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][0], 0x5); /* 0x2040f20 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][0], 0x4); /* 0x2040d00 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][1], 0x0); /* 0x2040d04 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][2], 0x2); /* 0x2040d08 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][3], 0x3); /* 0x2040d0c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][4], 0xa); /* 0x2040d10 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][5], 0x5); /* 0x2040d14 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][6], 0x7); /* 0x2040d18 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][7], 0x8); /* 0x2040d1c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][0], 0x13); /* 0x2040fc0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][0], 0xd); /* 0x2040e40 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][1], 0x18); /* 0x2040fc4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][1], 0xc); /* 0x2040e44 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][1], 0xe); /* 0x2040f04 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][0], 0x1); /* 0x2040c20 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][1], 0x1); /* 0x2040c24 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][2], 0x5); /* 0x2040c28 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][3], 0x0); /* 0x2040c2c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][4], 0x6); /* 0x2040c30 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][5], 0x7); /* 0x2040c34 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][6], 0x7); /* 0x2040c38 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][7], 0xa); /* 0x2040c3c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][2], 0x7); /* 0x2040f88 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][2], 0xd); /* 0x2040e08 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][3], 0x0); /* 0x2040f8c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][3], 0xc); /* 0x2040e0c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][1], 0x8); /* 0x2040f24 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][0], 0x0); /* 0x2040d20 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][1], 0x1); /* 0x2040d24 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][2], 0x2); /* 0x2040d28 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][3], 0x2); /* 0x2040d2c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][4], 0x9); /* 0x2040d30 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][5], 0x8); /* 0x2040d34 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][6], 0x9); /* 0x2040d38 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][7], 0xa); /* 0x2040d3c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][2], 0x1); /* 0x2040fc8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][2], 0xf); /* 0x2040e48 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][3], 0x1f); /* 0x2040fcc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][3], 0xe); /* 0x2040e4c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][2], 0xa); /* 0x2040f08 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][0], 0x3); /* 0x2040c40 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][1], 0x4); /* 0x2040c44 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][2], 0x5); /* 0x2040c48 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][3], 0x2); /* 0x2040c4c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][4], 0xa); /* 0x2040c50 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][5], 0x6); /* 0x2040c54 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][6], 0x8); /* 0x2040c58 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][7], 0x9); /* 0x2040c5c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][4], 0x8); /* 0x2040f90 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][4], 0xf); /* 0x2040e10 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][5], 0x11); /* 0x2040f94 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][5], 0xe); /* 0x2040e14 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][2], 0x8); /* 0x2040f28 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][0], 0x3); /* 0x2040d40 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][1], 0x1); /* 0x2040d44 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][2], 0x2); /* 0x2040d48 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][3], 0x4); /* 0x2040d4c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][4], 0x7); /* 0x2040d50 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][5], 0x8); /* 0x2040d54 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][6], 0x5); /* 0x2040d58 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][7], 0x5); /* 0x2040d5c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][4], 0x8); /* 0x2040fd0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][4], 0xc); /* 0x2040e50 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][5], 0x5); /* 0x2040fd4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][5], 0xe); /* 0x2040e54 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][3], 0x9); /* 0x2040f0c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][0], 0x3); /* 0x2040c60 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][1], 0x1); /* 0x2040c64 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][2], 0x4); /* 0x2040c68 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][3], 0x1); /* 0x2040c6c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][4], 0x5); /* 0x2040c70 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][5], 0x7); /* 0x2040c74 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][6], 0x8); /* 0x2040c78 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][7], 0xa); /* 0x2040c7c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][6], 0x2); /* 0x2040f98 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][6], 0xc); /* 0x2040e18 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][7], 0x1f); /* 0x2040f9c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][7], 0xc); /* 0x2040e1c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][3], 0x8); /* 0x2040f2c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][0], 0x0); /* 0x2040d60 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][1], 0x1); /* 0x2040d64 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][2], 0x4); /* 0x2040d68 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][3], 0x2); /* 0x2040d6c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][4], 0x9); /* 0x2040d70 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][5], 0x5); /* 0x2040d74 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][6], 0x7); /* 0x2040d78 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][7], 0x5); /* 0x2040d7c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][6], 0x6); /* 0x2040fd8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][6], 0xf); /* 0x2040e58 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][7], 0x18); /* 0x2040fdc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][7], 0xe); /* 0x2040e5c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][4], 0xe); /* 0x2040f10 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][0], 0x1); /* 0x2040c80 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][1], 0x5); /* 0x2040c84 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][2], 0x5); /* 0x2040c88 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][3], 0x1); /* 0x2040c8c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][4], 0x5); /* 0x2040c90 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][5], 0xa); /* 0x2040c94 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][6], 0xa); /* 0x2040c98 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][7], 0x7); /* 0x2040c9c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][8], 0x14); /* 0x2040fa0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][8], 0xd); /* 0x2040e20 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][9], 0x8); /* 0x2040fa4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][9], 0xe); /* 0x2040e24 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][4], 0x6); /* 0x2040f30 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][0], 0x2); /* 0x2040d80 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][1], 0x1); /* 0x2040d84 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][2], 0x1); /* 0x2040d88 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][3], 0x0); /* 0x2040d8c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][4], 0x8); /* 0x2040d90 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][5], 0x7); /* 0x2040d94 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][6], 0x9); /* 0x2040d98 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][7], 0x9); /* 0x2040d9c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][8], 0x1b); /* 0x2040fe0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][8], 0xe); /* 0x2040e60 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][9], 0x4); /* 0x2040fe4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][9], 0xe); /* 0x2040e64 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][5], 0x9); /* 0x2040f14 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][0], 0x4); /* 0x2040ca0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][1], 0x1); /* 0x2040ca4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][2], 0x3); /* 0x2040ca8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][3], 0x1); /* 0x2040cac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][4], 0x8); /* 0x2040cb0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][5], 0x5); /* 0x2040cb4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][6], 0xa); /* 0x2040cb8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][7], 0x6); /* 0x2040cbc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][10], 0x7); /* 0x2040fa8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][10], 0xc); /* 0x2040e28 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][11], 0x13); /* 0x2040fac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][11], 0xf); /* 0x2040e2c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][5], 0xc); /* 0x2040f34 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][0], 0x0); /* 0x2040da0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][1], 0x3); /* 0x2040da4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][2], 0x4); /* 0x2040da8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][3], 0x3); /* 0x2040dac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][4], 0x7); /* 0x2040db0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][5], 0x6); /* 0x2040db4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][6], 0x6); /* 0x2040db8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][7], 0x6); /* 0x2040dbc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][10], 0x1a); /* 0x2040fe8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][10], 0xf); /* 0x2040e68 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][11], 0xf); /* 0x2040fec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][11], 0xf); /* 0x2040e6c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][6], 0x1); /* 0x2040f18 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][0], 0x3); /* 0x2040cc0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][1], 0x3); /* 0x2040cc4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][2], 0x4); /* 0x2040cc8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][3], 0x3); /* 0x2040ccc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][4], 0x9); /* 0x2040cd0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][5], 0xa); /* 0x2040cd4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][6], 0x7); /* 0x2040cd8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][7], 0x6); /* 0x2040cdc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][12], 0x11); /* 0x2040fb0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][12], 0xe); /* 0x2040e30 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][13], 0x11); /* 0x2040fb4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][13], 0xe); /* 0x2040e34 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][6], 0xd); /* 0x2040f38 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][0], 0x1); /* 0x2040dc0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][1], 0x3); /* 0x2040dc4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][2], 0x5); /* 0x2040dc8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][3], 0x1); /* 0x2040dcc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][4], 0x8); /* 0x2040dd0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][5], 0xa); /* 0x2040dd4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][6], 0x9); /* 0x2040dd8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][7], 0x7); /* 0x2040ddc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][12], 0xa); /* 0x2040ff0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][12], 0xe); /* 0x2040e70 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][13], 0x0); /* 0x2040ff4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][13], 0xf); /* 0x2040e74 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][7], 0x8); /* 0x2040f1c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][0], 0x0); /* 0x2040ce0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][1], 0x2); /* 0x2040ce4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][2], 0x3); /* 0x2040ce8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][3], 0x2); /* 0x2040cec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][4], 0x5); /* 0x2040cf0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][5], 0x7); /* 0x2040cf4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][6], 0x9); /* 0x2040cf8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][7], 0xa); /* 0x2040cfc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][14], 0x4); /* 0x2040fb8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][14], 0xf); /* 0x2040e38 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][15], 0x3); /* 0x2040fbc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][15], 0xd); /* 0x2040e3c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][7], 0x2); /* 0x2040f3c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][0], 0x1); /* 0x2040de0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][1], 0x1); /* 0x2040de4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][2], 0x0); /* 0x2040de8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][3], 0x4); /* 0x2040dec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][4], 0x6); /* 0x2040df0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][5], 0x6); /* 0x2040df4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][6], 0xa); /* 0x2040df8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][7], 0x6); /* 0x2040dfc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][14], 0x9); /* 0x2040ff8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][14], 0xc); /* 0x2040e78 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][15], 0x1); /* 0x2040ffc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][15], 0xf); /* 0x2040e7c */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x18); /* 0x2038e58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xa63f6); /* 0x2038e40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xdea7c); /* 0x2038e44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0x956f1); /* 0x2038e48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xcfa1d); /* 0x2038e4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_validselect, 0xc0000000); /* 0x2038e5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x12); /* 0x2038e78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xecfd4); /* 0x2038e60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xce2f2); /* 0x2038e64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0x8eb70); /* 0x2038e68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xe56df); /* 0x2038e6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x2038e7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x14); /* 0x2039e58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x2039e40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x2039e44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x2039e48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x2039e4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x2039e5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x13); /* 0x2039e78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xa4e51); /* 0x2039e60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xc5ed5); /* 0x2039e64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xe6f59); /* 0x2039e68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x87fdd); /* 0x2039e6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x2039e7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x14); /* 0x203ae58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xade71); /* 0x203ae40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xa771c); /* 0x203ae44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xcc352); /* 0x203ae48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xb7f7e); /* 0x203ae4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x203ae5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x16); /* 0x203ae78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xc4bd1); /* 0x203ae60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xff214); /* 0x203ae64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xeeeb3); /* 0x203ae68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xd66d7); /* 0x203ae6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x203ae7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x12); /* 0x203be58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xf5f95); /* 0x203be40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xdda1a); /* 0x203be44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xff632); /* 0x203be48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xa6713); /* 0x203be4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x203be5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x10); /* 0x203be78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xbcf14); /* 0x203be60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0x8f6b9); /* 0x203be64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xb4bf0); /* 0x203be68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xe6fda); /* 0x203be6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x203be7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x14); /* 0x203ce58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xcea35); /* 0x203ce40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xdda98); /* 0x203ce44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xe43d2); /* 0x203ce48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xedff3); /* 0x203ce4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x203ce5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x13); /* 0x203ce78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xa4e51); /* 0x203ce60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xc5ed5); /* 0x203ce64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xe6f59); /* 0x203ce68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x87fdd); /* 0x203ce6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x203ce7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x14); /* 0x203de58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xdd3f9); /* 0x203de40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xae396); /* 0x203de44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0x9ea3e); /* 0x203de48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xede50); /* 0x203de4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x203de5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x13); /* 0x203de78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xd76d9); /* 0x203de60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xdfb15); /* 0x203de64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0x94670); /* 0x203de68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xa73f7); /* 0x203de6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x203de7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x15); /* 0x203ee58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xb779f); /* 0x203ee40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xd7af9); /* 0x203ee44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdca75); /* 0x203ee48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x86291); /* 0x203ee4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x203ee5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x11); /* 0x203ee78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xd5bf9); /* 0x203ee60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbd311); /* 0x203ee64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0x9f790); /* 0x203ee68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x96fd5); /* 0x203ee6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x203ee7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x12); /* 0x203fe58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xa4e51); /* 0x203fe40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xc5ed5); /* 0x203fe44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xe6f59); /* 0x203fe48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x87fdd); /* 0x203fe4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x203fe5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x12); /* 0x203fe78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9ca30); /* 0x203fe60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xbdab4); /* 0x203fe64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xdeb38); /* 0x203fe68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xffbbc); /* 0x203fe6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x203fe7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0x8); /* 0x2038f90 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0xa); /* 0x2038f94 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_mem_hashadr_xbar_ctl[0], 0x10); /* 0x2038fc0 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_bank_enable[0], 0x1000000); /* 0x2038f00 */
    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xc); /* 0x2039f90 */
    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0xb); /* 0x2039f94 */
    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xc); /* 0x203af90 */
    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0xe); /* 0x203af94 */
    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xa); /* 0x203bf90 */
    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0x8); /* 0x203bf94 */
    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xc); /* 0x203cf90 */
    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0xb); /* 0x203cf94 */
    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xc); /* 0x203df90 */
    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0xb); /* 0x203df94 */
    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xd); /* 0x203ef90 */
    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0x9); /* 0x203ef94 */
    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xa); /* 0x203ff90 */
    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0xa); /* 0x203ff94 */
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1c); // ADDED EMVH070915
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
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x10); // ADDED EMVH070915
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
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x19); // ADDED EMVH070915
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
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x14); // ADDED EMVH070915
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
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x12); // ADDED EMVH070915
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
    tu.IndirectWrite(0x020080019c0a, 0x411adb764bc76827, 0xedc65fc0847364a1); /* pipe0.stage0.mau_cfg.mau_array_cfg.sram_ 6_ 7: a=0x20080019c0a d0=0x411adb764bc76827 d1=0xedc65fc0847364a1 */
    tu.IndirectWrite(0x02008001800a, 0x7f05e041c632a090, 0xcc92b58d5e8f2473); /* pipe0.stage0.mau_cfg.mau_array_cfg.sram_ 6_ 0: a=0x2008001800a d0=0x7f05e041c632a090 d1=0xcc92b58d5e8f2473 */
  //    tu.OutWord(&mau_reg_map.rams.match.merge.exact_match_delay_config, 0x0); /* 0x2026cf8 */ // REMOVED EMDEL070915
    tu.OutWord(&mau_reg_map.rams.match.merge.col[0].row_action_nxtable_bus_drive[0], 0x1); /* 0x2026000 */
    tu.OutWord(&mau_reg_map.rams.match.merge.match_to_logical_table_ixbar_outputmap[0][0], 0x15); /* 0x2027d00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_miss_value[5], 0x1); /* 0x2024dd4 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_miss_value[5], 0xa); /* 0x2024e54 */
    tu.OutWord(&mau_reg_map.rams.match.merge.predication_ctl[0], 0xc260020); /* 0x2026cb0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.predication_ctl[1], 0xc260000); /* 0x2026cb8 */
    tu.OutWord(&mau_reg_map.rams.match.merge.next_table_format_data[5], 0x5a); /* 0x2024cd4 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_miss_value[5], 0xacedbaba); /* 0x2024d54 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_miss_value[5], 0x2b); /* 0x2024dd4 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_miss_value[5], 0x3dbabe); /* 0x2024e54 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_miss_value[5], 0xbbebe); /* 0x2024ed4 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_miss_value[5], 0x101cab); /* 0x2024f54 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_miss_value[5], 0x1dead); /* 0x2024fd4 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_exact_shiftcount[0][2], 0x0); /* 0x2026e08 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_exact_shiftcount[0][2], 0x40); /* 0x2027008 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_exact_shiftcount[0][2], 0xd); /* 0x2027208 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_exact_shiftcount[0][2], 0x11); /* 0x2027408 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_exact_shiftcount[0][2], 0x13); /* 0x2027608 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_exact_shiftcount[0][2], 0x16); /* 0x2027808 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_exact_shiftcount[0][3], 0x21); /* 0x2026e0c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_exact_shiftcount[0][3], 0x40); /* 0x202700c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_exact_shiftcount[0][3], 0x2e); /* 0x202720c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_exact_shiftcount[0][3], 0x32); /* 0x202740c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_exact_shiftcount[0][3], 0x34); /* 0x202760c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_exact_shiftcount[0][3], 0x37); /* 0x202780c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_mask[0][0], 0xff); /* 0x2027e00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_default[0][0], 0x0); /* 0x2027e80 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_mask[0][0], 0x0); /* 0x2027f80 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_default[0][0], 0x0); /* 0x2024000 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_vpn_shiftcount[0][0], 0x0); /* 0x2024080 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_mask[0][0], 0x1ff); /* 0x2024100 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_default[0][0], 0x0); /* 0x2024180 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_mask[0][0], 0x3); /* 0x2024280 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_default[0][0], 0x0); /* 0x2024300 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_mask[0][0], 0x7); /* 0x2024500 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_default[0][0], 0x0); /* 0x2024580 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_mask[0][0], 0x1f); /* 0x2024700 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_default[0][0], 0x0); /* 0x2024780 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.ram_address_mux_ctl[0][0], 0x180); /* 0x2008f00 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.ram_address_mux_ctl[0][0], 0x40); /* 0x200ef00 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.ram_address_mux_ctl[1][1], 0x40); /* 0x200ef48 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.immediate_data_32b_ixbar_ctl[5], 0x20); /* 0x2020114 */
  ActionHvTranslator act_hv_translator;
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x1); /* 0x203e884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(6,1,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x2); /* 0x203e88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(6,1,0x2); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x3); /* 0x203e884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(6,1,0x3); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xa); /* 0x203e88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(6,1,0xa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x7); /* 0x203e884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(6,1,0x7); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x2a); /* 0x203e88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(6,1,0x2a); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0xf); /* 0x203e884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(6,1,0xf); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xaa); /* 0x203e88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(6,1,0xaa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x10f); /* 0x203e884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(6,1,0x10f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x300aa); /* 0x203e88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(6,1,0x300aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x30f); /* 0x203e884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(6,1,0x30f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xf00aa); /* 0x203e88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(6,1,0xf00aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x70f); /* 0x203e884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(6,1,0x70f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x3f00aa); /* 0x203e88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(6,1,0x3f00aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0xf0f); /* 0x203e884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(6,1,0xf0f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xff00aa); /* 0x203e88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(6,1,0xff00aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x1f0f); /* 0x203e884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(6,1,0x1f0f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x3ff00aa); /* 0x203e88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(6,1,0x3ff00aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x3f0f); /* 0x203e884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(6,1,0x3f0f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xfff00aa); /* 0x203e88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(6,1,0xfff00aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0x7f0f); /* 0x203e884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(6,1,0x7f0f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0x3fff00aa); /* 0x203e88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(6,1,0x3fff00aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte_enable[1], 0xff0f); /* 0x203e884 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(6,1,0xff0f); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_byte[1], 0xffff00aa); /* 0x203e88c */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(6,1,0xffff00aa); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_half[1][0], 0x1); /* 0x203e8e0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(6,1,0,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_half[1][0], 0x1); /* 0x203e8e0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(6,1,0,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_half[1][0], 0x5); /* 0x203e8e0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(6,1,0,0x5); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[6].action_hv_xbar.action_hv_xbar_ctl_half[1][0], 0x5); /* 0x203e8e0 */ // REMOVED ACHV070915
  act_hv_translator.ctl_half(6,1,0,0x5); // ADDED ACHV070915
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[6].ctl.r_l_action_o_mux_select, 0x10); /* 0x2030680 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[6].ctl.r_action_o_mux_select, 0x1); /* 0x20306a8 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[0][0], RM_B4_32(0x24b8e874)); /* 0x207c000 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[1][0], RM_B4_32(0x13f2c8a5)); /* 0x207c080 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[2][0], RM_B4_32(0x3027e819)); /* 0x207c100 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][0], RM_B4_32(0x160dc850)); /* 0x207c180 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][0], RM_B4_32(0x1c186808)); /* 0x207c200 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][0], RM_B4_32(0x32ade867)); /* 0x207c280 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[6][0], RM_B4_32(0x30316849)); /* 0x207c300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][0], RM_B4_32(0x1eb7aad5)); /* 0x207c380 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[8][0], RM_B4_32(0x3f0ce816)); /* 0x207c400 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][0], RM_B4_32(0x2ce0ead6)); /* 0x207c480 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][0], RM_B4_32(0x1bf38a7b)); /* 0x207c500 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][0], RM_B4_32(0x308beaac)); /* 0x207c580 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][0], RM_B4_32(0x2081e8ad)); /* 0x207c600 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][0], RM_B4_32(0x5846ab7)); /* 0x207c680 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][0], RM_B4_32(0x3b8cea8b)); /* 0x207c700 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[15][0], RM_B4_32(0x2bcfe859)); /* 0x207c780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][0], RM_B4_32(0xdfc6884)); /* 0x207c800 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[17][0], RM_B4_32(0x57b1abf)); /* 0x207c880 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][0], RM_B4_32(0x787e810)); /* 0x207c900 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[19][0], RM_B4_32(0x37dceabd)); /* 0x207c980 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[20][0], RM_B4_32(0x316884)); /* 0x207ca00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][0], RM_B4_32(0x114aea97)); /* 0x207ca80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][0], RM_B4_32(0x3fc46ac0)); /* 0x207cb00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[23][0], RM_B4_32(0x6cae871)); /* 0x207cb80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][0], RM_B4_32(0x128e84c)); /* 0x207cc00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][0], RM_B4_32(0x1069e89c)); /* 0x207cc80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[26][0], RM_B4_32(0x15ccc85e)); /* 0x207cd00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[27][0], RM_B4_32(0x1156e88a)); /* 0x207cd80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[28][0], RM_B4_32(0x3e0de8bc)); /* 0x207ce00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][0], RM_B4_32(0x1d6f3814)); /* 0x207ce80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[30][0], RM_B4_32(0xc606851)); /* 0x207cf00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][0], RM_B4_32(0x2572e88a)); /* 0x207cf80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[32][0], RM_B4_32(0x16c66864)); /* 0x207d000 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][0], RM_B4_32(0x28b6187f)); /* 0x207d080 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][0], RM_B4_32(0x369880e3)); /* 0x207d100 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][0], RM_B4_32(0x157f4a50)); /* 0x207d180 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][0], RM_B4_32(0xb37a88f)); /* 0x207d200 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][0], RM_B4_32(0x37d9ea56)); /* 0x207d280 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][0], RM_B4_32(0x22ffaac9)); /* 0x207d300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][0], RM_B4_32(0x331eaaa1)); /* 0x207d380 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][0], RM_B4_32(0x1b6ad4)); /* 0x207d400 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[41][0], RM_B4_32(0x451485f)); /* 0x207d480 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][0], RM_B4_32(0x65ae874)); /* 0x207d500 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][0], RM_B4_32(0xca4989c)); /* 0x207d580 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][0], RM_B4_32(0x1ed68aa4)); /* 0x207d600 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][0], RM_B4_32(0x2615e8af)); /* 0x207d680 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][0], RM_B4_32(0x15eac871)); /* 0x207d700 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[47][0], RM_B4_32(0x3a3c6a98)); /* 0x207d780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][0], RM_B4_32(0x1531c841)); /* 0x207d800 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][0], RM_B4_32(0xcdbea5d)); /* 0x207d880 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][0], RM_B4_32(0x2bbf8e7c)); /* 0x207d900 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][0], RM_B4_32(0x95be84a)); /* 0x207d980 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[52][0], RM_B4_32(0x3c409aa6)); /* 0x207da00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[53][0], RM_B4_32(0x18b5e817)); /* 0x207da80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][0], RM_B4_32(0x2c02eaa1)); /* 0x207db00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[55][0], RM_B4_32(0x16c3ead0)); /* 0x207db80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][0], RM_B4_32(0x188a800)); /* 0x207dc00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[57][0], RM_B4_32(0x16a83)); /* 0x207dc80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[58][0], RM_B4_32(0x2f203892)); /* 0x207dd00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][0], RM_B4_32(0x5b0eac6)); /* 0x207dd80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[60][0], RM_B4_32(0x2172e863)); /* 0x207de00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][0], RM_B4_32(0x2862e840)); /* 0x207de80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][0], RM_B4_32(0x35f5eaa6)); /* 0x207df00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[63][0], RM_B4_32(0x3e05b81e)); /* 0x207df80 */
    tu.OutWord(&mau_reg_map.dp.imem_parity_ctl, 0x6); /* 0x2060044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][0], 0xa6af); /* 0x2074000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][1], 0x1246); /* 0x2074004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][2], 0x39623); /* 0x2074008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][3], 0x3e8fb); /* 0x207400c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][4], 0x31586); /* 0x2074010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][5], 0x7bc4); /* 0x2074014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][6], 0x32f68); /* 0x2074018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][7], 0x1596); /* 0x207401c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][8], 0xbc77); /* 0x2074020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][9], 0x23172); /* 0x2074024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][10], 0x2a11b); /* 0x2074028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][11], 0x36b57); /* 0x207402c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][12], 0x10cf3); /* 0x2074030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][13], 0x2b32a); /* 0x2074034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][14], 0x17e66); /* 0x2074038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][15], 0x23b4d); /* 0x207403c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][16], 0x23530); /* 0x2074040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][17], 0x1fcbb); /* 0x2074044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][18], 0x3cb97); /* 0x2074048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][19], 0xc025); /* 0x207404c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][20], 0x40e8); /* 0x2074050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][21], 0x98da); /* 0x2074054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][22], 0x24353); /* 0x2074058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][23], 0xe7d9); /* 0x207405c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][24], 0x3763a); /* 0x2074060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][25], 0x2158b); /* 0x2074064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][26], 0x203fb); /* 0x2074068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][27], 0x3cf5); /* 0x207406c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][28], 0xf86d); /* 0x2074070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][29], 0xfa6f); /* 0x2074074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][30], 0x2d952); /* 0x2074078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][31], 0x30428); /* 0x207407c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][32], 0x3b7fd); /* 0x2074080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][33], 0x13e6a); /* 0x2074084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][34], 0x3d2c3); /* 0x2074088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][35], 0xc0d6); /* 0x207408c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][36], 0x1e0bb); /* 0x2074090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][37], 0x2b5c8); /* 0x2074094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][38], 0x376d3); /* 0x2074098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][39], 0x2cdea); /* 0x207409c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][40], 0x31ec9); /* 0x20740a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][41], 0x82b9); /* 0x20740a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][42], 0x1bee4); /* 0x20740a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][43], 0xeecc); /* 0x20740ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][44], 0x1152f); /* 0x20740b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][45], 0x5b3c); /* 0x20740b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][46], 0x3f269); /* 0x20740b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][47], 0x2d23b); /* 0x20740bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][48], 0x2007c); /* 0x20740c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][49], 0x18d8c); /* 0x20740c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][50], 0xad6b); /* 0x20740c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][51], 0x22e09); /* 0x20740cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][0], 0x12d7a); /* 0x2074100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][1], 0x19a0e); /* 0x2074104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][2], 0x19ead); /* 0x2074108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][3], 0x1fe82); /* 0x207410c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][4], 0x35dde); /* 0x2074110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][5], 0x28522); /* 0x2074114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][6], 0xf298); /* 0x2074118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][7], 0x3112b); /* 0x207411c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][8], 0xa032); /* 0x2074120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][9], 0x2aad0); /* 0x2074124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][10], 0x3321e); /* 0x2074128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][11], 0x1d296); /* 0x207412c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][12], 0xefff); /* 0x2074130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][13], 0xc4c5); /* 0x2074134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][14], 0x24da8); /* 0x2074138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][15], 0x24259); /* 0x207413c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][16], 0x17d26); /* 0x2074140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][17], 0x3c514); /* 0x2074144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][18], 0x22d95); /* 0x2074148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][19], 0x381dc); /* 0x207414c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][20], 0x89fe); /* 0x2074150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][21], 0x32bd2); /* 0x2074154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][22], 0x25cf7); /* 0x2074158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][23], 0x2f7b1); /* 0x207415c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][24], 0x3a07a); /* 0x2074160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][25], 0x3a782); /* 0x2074164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][26], 0x21bee); /* 0x2074168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][27], 0x9ed7); /* 0x207416c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][28], 0x11b5); /* 0x2074170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][29], 0xc7ec); /* 0x2074174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][30], 0x236a2); /* 0x2074178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][31], 0xa734); /* 0x207417c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][32], 0x40f0); /* 0x2074180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][33], 0x230ed); /* 0x2074184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][34], 0x16fe0); /* 0x2074188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][35], 0x11483); /* 0x207418c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][36], 0x355b5); /* 0x2074190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][37], 0x13e44); /* 0x2074194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][38], 0x28d2f); /* 0x2074198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][39], 0x1f3fb); /* 0x207419c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][40], 0x295d0); /* 0x20741a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][41], 0x31b5c); /* 0x20741a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][42], 0x35935); /* 0x20741a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][43], 0xd166); /* 0x20741ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][44], 0x24d9c); /* 0x20741b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][45], 0x2c07d); /* 0x20741b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][46], 0x165f8); /* 0x20741b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][47], 0x3f62b); /* 0x20741bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][48], 0x50c8); /* 0x20741c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][49], 0x32ecd); /* 0x20741c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][50], 0x939d); /* 0x20741c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][51], 0xde71); /* 0x20741cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][0], 0x86a5); /* 0x2074200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][1], 0x3a0d); /* 0x2074204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][2], 0x19136); /* 0x2074208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][3], 0x21778); /* 0x207420c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][4], 0x25636); /* 0x2074210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][5], 0x17daf); /* 0x2074214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][6], 0xc1ca); /* 0x2074218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][7], 0x2f120); /* 0x207421c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][8], 0x70ef); /* 0x2074220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][9], 0x35b8c); /* 0x2074224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][10], 0x2486e); /* 0x2074228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][11], 0x1fd7d); /* 0x207422c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][12], 0x1b1ae); /* 0x2074230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][13], 0x1af00); /* 0x2074234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][14], 0x14f46); /* 0x2074238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][15], 0x2e2b5); /* 0x207423c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][16], 0xbf4a); /* 0x2074240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][17], 0xe7ed); /* 0x2074244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][18], 0x23800); /* 0x2074248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][19], 0x1be0d); /* 0x207424c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][20], 0x3ccaf); /* 0x2074250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][21], 0xdc24); /* 0x2074254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][22], 0x38131); /* 0x2074258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][23], 0x17617); /* 0x207425c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][24], 0x1c591); /* 0x2074260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][25], 0xbfe4); /* 0x2074264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][26], 0x295a0); /* 0x2074268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][27], 0x1fcb6); /* 0x207426c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][28], 0x11496); /* 0x2074270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][29], 0x33d5d); /* 0x2074274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][30], 0xdb5e); /* 0x2074278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][31], 0x536d); /* 0x207427c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][32], 0x20b20); /* 0x2074280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][33], 0x2ecfa); /* 0x2074284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][34], 0x703e); /* 0x2074288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][35], 0x10666); /* 0x207428c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][36], 0x37117); /* 0x2074290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][37], 0x22f80); /* 0x2074294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][38], 0x12a56); /* 0x2074298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][39], 0x2b436); /* 0x207429c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][40], 0x32aa8); /* 0x20742a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][41], 0x15fc3); /* 0x20742a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][42], 0x28c0b); /* 0x20742a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][43], 0x77e8); /* 0x20742ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][44], 0x8511); /* 0x20742b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][45], 0x1af9d); /* 0x20742b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][46], 0x20e9f); /* 0x20742b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][47], 0x26b26); /* 0x20742bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][48], 0xbc05); /* 0x20742c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][49], 0x30e96); /* 0x20742c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][50], 0x13bad); /* 0x20742c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][51], 0x39def); /* 0x20742cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][0], 0x2071); /* 0x2074300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][1], 0x16040); /* 0x2074304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][2], 0x1c133); /* 0x2074308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][3], 0x21c69); /* 0x207430c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][4], 0xa5b3); /* 0x2074310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][5], 0x6c4f); /* 0x2074314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][6], 0x11483); /* 0x2074318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][7], 0x7958); /* 0x207431c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][8], 0x36697); /* 0x2074320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][9], 0x2c05); /* 0x2074324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][10], 0x3fff9); /* 0x2074328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][11], 0x215cb); /* 0x207432c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][12], 0x3b86d); /* 0x2074330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][13], 0x17785); /* 0x2074334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][14], 0x39e94); /* 0x2074338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][15], 0xaef8); /* 0x207433c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][16], 0x28fb4); /* 0x2074340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][17], 0x19bcf); /* 0x2074344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][18], 0x5963); /* 0x2074348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][19], 0x2ff05); /* 0x207434c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][20], 0x1ae21); /* 0x2074350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][21], 0x24b8a); /* 0x2074354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][22], 0x1614a); /* 0x2074358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][23], 0x236eb); /* 0x207435c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][24], 0x2dfc8); /* 0x2074360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][25], 0x4c43); /* 0x2074364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][26], 0x22e94); /* 0x2074368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][27], 0x1f510); /* 0x207436c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][28], 0x4bda); /* 0x2074370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][29], 0x15146); /* 0x2074374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][30], 0x3c460); /* 0x2074378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][31], 0x14e6e); /* 0x207437c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][32], 0x1a64); /* 0x2074380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][33], 0x39d39); /* 0x2074384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][34], 0x85a6); /* 0x2074388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][35], 0x2faa0); /* 0x207438c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][36], 0x1599a); /* 0x2074390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][37], 0x16340); /* 0x2074394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][38], 0x34dd6); /* 0x2074398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][39], 0x172c6); /* 0x207439c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][40], 0x30dd3); /* 0x20743a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][41], 0x20d31); /* 0x20743a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][42], 0x3493b); /* 0x20743a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][43], 0x1d93b); /* 0x20743ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][44], 0x18a8e); /* 0x20743b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][45], 0x3fc94); /* 0x20743b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][46], 0x224f6); /* 0x20743b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][47], 0x15068); /* 0x20743bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][48], 0xb201); /* 0x20743c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][49], 0x8504); /* 0x20743c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][50], 0x2b43a); /* 0x20743c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][51], 0x2fd63); /* 0x20743cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][0], 0x1e35a); /* 0x2074400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][1], 0xa141); /* 0x2074404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][2], 0x2d242); /* 0x2074408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][3], 0x3bd97); /* 0x207440c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][4], 0x1e9be); /* 0x2074410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][5], 0x327d3); /* 0x2074414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][6], 0x28a97); /* 0x2074418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][7], 0x3015a); /* 0x207441c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][8], 0x29629); /* 0x2074420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][9], 0x3a10f); /* 0x2074424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][10], 0x1187a); /* 0x2074428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][11], 0x2eb6); /* 0x207442c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][12], 0x2772a); /* 0x2074430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][13], 0x2ebeb); /* 0x2074434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][14], 0x1791c); /* 0x2074438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][15], 0xc1a8); /* 0x207443c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][16], 0x2d24c); /* 0x2074440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][17], 0x34e2b); /* 0x2074444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][18], 0xb8f4); /* 0x2074448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][19], 0x9820); /* 0x207444c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][20], 0x2d41e); /* 0x2074450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][21], 0x3ba08); /* 0x2074454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][22], 0xc0a8); /* 0x2074458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][23], 0x38492); /* 0x207445c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][24], 0x1e8a6); /* 0x2074460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][25], 0x1bb4b); /* 0x2074464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][26], 0x3070e); /* 0x2074468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][27], 0x132fa); /* 0x207446c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][28], 0x8030); /* 0x2074470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][29], 0x1a62); /* 0x2074474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][30], 0x21e80); /* 0x2074478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][31], 0x3a2ed); /* 0x207447c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][32], 0x32c41); /* 0x2074480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][33], 0x3d31f); /* 0x2074484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][34], 0x4023); /* 0x2074488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][35], 0x7a3d); /* 0x207448c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][36], 0x1a4c5); /* 0x2074490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][37], 0x32618); /* 0x2074494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][38], 0x1b990); /* 0x2074498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][39], 0x16b39); /* 0x207449c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][40], 0x2136c); /* 0x20744a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][41], 0x8b90); /* 0x20744a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][42], 0x4654); /* 0x20744a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][43], 0x30c61); /* 0x20744ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][44], 0x1c9fc); /* 0x20744b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][45], 0x105e9); /* 0x20744b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][46], 0x1a6a3); /* 0x20744b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][47], 0x29600); /* 0x20744bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][48], 0x23896); /* 0x20744c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][49], 0x31d6); /* 0x20744c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][50], 0x251b8); /* 0x20744c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][51], 0x143d); /* 0x20744cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][0], 0x19a8e); /* 0x2074500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][1], 0x3206c); /* 0x2074504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][2], 0x18440); /* 0x2074508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][3], 0x3b94e); /* 0x207450c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][4], 0xbbf0); /* 0x2074510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][5], 0x29ee5); /* 0x2074514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][6], 0x25cd6); /* 0x2074518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][7], 0x33d09); /* 0x207451c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][8], 0x1b8ff); /* 0x2074520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][9], 0x2a21f); /* 0x2074524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][10], 0x29fb8); /* 0x2074528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][11], 0x1743); /* 0x207452c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][12], 0x1ce21); /* 0x2074530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][13], 0x3cac8); /* 0x2074534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][14], 0x387a); /* 0x2074538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][15], 0x2cb5); /* 0x207453c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][16], 0x3d1cf); /* 0x2074540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][17], 0x2298b); /* 0x2074544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][18], 0x25547); /* 0x2074548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][19], 0x2ab6d); /* 0x207454c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][20], 0x1e310); /* 0x2074550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][21], 0x3e689); /* 0x2074554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][22], 0x335ad); /* 0x2074558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][23], 0x3bcce); /* 0x207455c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][24], 0x1d7fa); /* 0x2074560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][25], 0x19a7a); /* 0x2074564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][26], 0x1411f); /* 0x2074568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][27], 0x2129b); /* 0x207456c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][28], 0x125ab); /* 0x2074570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][29], 0x283e6); /* 0x2074574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][30], 0x2987b); /* 0x2074578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][31], 0x3ac07); /* 0x207457c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][32], 0x8668); /* 0x2074580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][33], 0x25f62); /* 0x2074584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][34], 0x2372b); /* 0x2074588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][35], 0x35ae8); /* 0x207458c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][36], 0x2f893); /* 0x2074590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][37], 0xa684); /* 0x2074594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][38], 0x2b7de); /* 0x2074598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][39], 0x366ad); /* 0x207459c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][40], 0x242fb); /* 0x20745a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][41], 0x34d0f); /* 0x20745a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][42], 0x7795); /* 0x20745a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][43], 0x315eb); /* 0x20745ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][44], 0x140c8); /* 0x20745b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][45], 0x38dd1); /* 0x20745b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][46], 0x1d46d); /* 0x20745b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][47], 0x2169f); /* 0x20745bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][48], 0x7a8b); /* 0x20745c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][49], 0x2896a); /* 0x20745c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][50], 0x3725a); /* 0x20745c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][51], 0x1cad1); /* 0x20745cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][0], 0xb7d2); /* 0x2074600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][1], 0x2f671); /* 0x2074604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][2], 0x1157); /* 0x2074608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][3], 0x3cfe7); /* 0x207460c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][4], 0x30146); /* 0x2074610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][5], 0x3e882); /* 0x2074614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][6], 0x1a372); /* 0x2074618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][7], 0x23c8d); /* 0x207461c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][8], 0x27edd); /* 0x2074620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][9], 0x17bea); /* 0x2074624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][10], 0xc176); /* 0x2074628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][11], 0x284c9); /* 0x207462c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][12], 0x39f1b); /* 0x2074630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][13], 0x3c92a); /* 0x2074634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][14], 0x2d507); /* 0x2074638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][15], 0x37094); /* 0x207463c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][16], 0x1e9fc); /* 0x2074640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][17], 0x3411); /* 0x2074644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][18], 0x34f81); /* 0x2074648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][19], 0x28b35); /* 0x207464c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][20], 0x13155); /* 0x2074650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][21], 0x3d657); /* 0x2074654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][22], 0x58); /* 0x2074658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][23], 0x182e3); /* 0x207465c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][24], 0x2b935); /* 0x2074660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][25], 0x2b2e6); /* 0x2074664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][26], 0x22460); /* 0x2074668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][27], 0xaf85); /* 0x207466c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][28], 0x29139); /* 0x2074670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][29], 0x2daf1); /* 0x2074674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][30], 0x34856); /* 0x2074678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][31], 0x5dc5); /* 0x207467c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][32], 0x2513c); /* 0x2074680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][33], 0xa1b8); /* 0x2074684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][34], 0x2707c); /* 0x2074688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][35], 0x3285e); /* 0x207468c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][36], 0x14d02); /* 0x2074690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][37], 0x3c08f); /* 0x2074694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][38], 0x157f); /* 0x2074698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][39], 0x8ef8); /* 0x207469c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][40], 0x1400e); /* 0x20746a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][41], 0x300bf); /* 0x20746a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][42], 0x2b0a4); /* 0x20746a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][43], 0x19e97); /* 0x20746ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][44], 0x1e7dc); /* 0x20746b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][45], 0xc4a2); /* 0x20746b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][46], 0x14765); /* 0x20746b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][47], 0x10c9a); /* 0x20746bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][48], 0xbd1f); /* 0x20746c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][49], 0x16d5); /* 0x20746c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][50], 0x14274); /* 0x20746c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][51], 0xea23); /* 0x20746cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][0], 0x2b66a); /* 0x2074700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][1], 0x1fc18); /* 0x2074704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][2], 0x7f5); /* 0x2074708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][3], 0x33726); /* 0x207470c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][4], 0x3d7b3); /* 0x2074710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][5], 0x8a3f); /* 0x2074714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][6], 0x3855); /* 0x2074718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][7], 0x29101); /* 0x207471c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][8], 0x3864b); /* 0x2074720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][9], 0x1e720); /* 0x2074724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][10], 0x2d82a); /* 0x2074728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][11], 0xe52c); /* 0x207472c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][12], 0x27747); /* 0x2074730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][13], 0x1c1b0); /* 0x2074734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][14], 0x2c4a9); /* 0x2074738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][15], 0x2493); /* 0x207473c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][16], 0x10c33); /* 0x2074740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][17], 0x252a1); /* 0x2074744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][18], 0x3f867); /* 0x2074748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][19], 0x11b8c); /* 0x207474c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][20], 0x36e5f); /* 0x2074750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][21], 0x20458); /* 0x2074754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][22], 0x15940); /* 0x2074758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][23], 0x27de0); /* 0x207475c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][24], 0x272e4); /* 0x2074760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][25], 0x3bcbd); /* 0x2074764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][26], 0x1e1c0); /* 0x2074768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][27], 0x3cfe); /* 0x207476c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][28], 0x2799f); /* 0x2074770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][29], 0x15837); /* 0x2074774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][30], 0x3df20); /* 0x2074778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][31], 0x33dea); /* 0x207477c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][32], 0x1b294); /* 0x2074780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][33], 0xfa67); /* 0x2074784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][34], 0x13059); /* 0x2074788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][35], 0x2840f); /* 0x207478c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][36], 0x332f5); /* 0x2074790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][37], 0x3e642); /* 0x2074794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][38], 0x20fcf); /* 0x2074798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][39], 0x3464c); /* 0x207479c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][40], 0x21bf0); /* 0x20747a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][41], 0x33de8); /* 0x20747a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][42], 0x2476a); /* 0x20747a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][43], 0x695f); /* 0x20747ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][44], 0x29b0a); /* 0x20747b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][45], 0x25c2c); /* 0x20747b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][46], 0xba34); /* 0x20747b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][47], 0x33db5); /* 0x20747bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][48], 0xb226); /* 0x20747c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][49], 0x33878); /* 0x20747c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][50], 0x27114); /* 0x20747c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][51], 0xd608); /* 0x20747cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][0], 0x1480); /* 0x2074800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][1], 0x366c2); /* 0x2074804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][2], 0x19a62); /* 0x2074808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][3], 0x2e99); /* 0x207480c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][4], 0x2d299); /* 0x2074810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][5], 0x3e462); /* 0x2074814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][6], 0x630a); /* 0x2074818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][7], 0x32930); /* 0x207481c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][8], 0xe29b); /* 0x2074820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][9], 0xc735); /* 0x2074824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][10], 0x3c140); /* 0x2074828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][11], 0x7ea9); /* 0x207482c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][12], 0x8697); /* 0x2074830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][13], 0x2e39); /* 0x2074834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][14], 0x1cafc); /* 0x2074838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][15], 0x318a8); /* 0x207483c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][16], 0x20651); /* 0x2074840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][17], 0x1a071); /* 0x2074844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][18], 0x38fd2); /* 0x2074848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][19], 0x36f48); /* 0x207484c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][20], 0x3a39b); /* 0x2074850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][21], 0x2bfda); /* 0x2074854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][22], 0x3a9ea); /* 0x2074858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][23], 0x3ef27); /* 0x207485c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][24], 0x2accc); /* 0x2074860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][25], 0xa61b); /* 0x2074864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][26], 0x1bacd); /* 0x2074868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][27], 0x3860f); /* 0x207486c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][28], 0x243a9); /* 0x2074870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][29], 0x25cdd); /* 0x2074874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][30], 0x3dc43); /* 0x2074878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][31], 0x39ac4); /* 0x207487c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][32], 0x40c0); /* 0x2074880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][33], 0x3a3b7); /* 0x2074884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][34], 0x116a7); /* 0x2074888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][35], 0x35e6e); /* 0x207488c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][36], 0x212b0); /* 0x2074890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][37], 0xafc1); /* 0x2074894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][38], 0x398e9); /* 0x2074898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][39], 0x25772); /* 0x207489c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][40], 0x2ac1c); /* 0x20748a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][41], 0x73e4); /* 0x20748a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][42], 0x3cc3f); /* 0x20748a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][43], 0x12294); /* 0x20748ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][44], 0x29bde); /* 0x20748b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][45], 0x2a65d); /* 0x20748b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][46], 0x305e4); /* 0x20748b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][47], 0x20263); /* 0x20748bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][48], 0x18b37); /* 0x20748c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][49], 0x2253b); /* 0x20748c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][50], 0x1a3b0); /* 0x20748c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][51], 0x2d5a0); /* 0x20748cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][0], 0x3683a); /* 0x2074900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][1], 0x1be30); /* 0x2074904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][2], 0x187a4); /* 0x2074908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][3], 0x1ab12); /* 0x207490c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][4], 0x17784); /* 0x2074910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][5], 0x39e34); /* 0x2074914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][6], 0x3e358); /* 0x2074918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][7], 0x4d3f); /* 0x207491c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][8], 0x2951f); /* 0x2074920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][9], 0x314ed); /* 0x2074924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][10], 0x3b8b7); /* 0x2074928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][11], 0x3abd); /* 0x207492c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][12], 0x32412); /* 0x2074930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][13], 0x1adc3); /* 0x2074934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][14], 0x1314b); /* 0x2074938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][15], 0x395b8); /* 0x207493c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][16], 0x3d137); /* 0x2074940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][17], 0xa06a); /* 0x2074944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][18], 0x3bfa0); /* 0x2074948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][19], 0xdbf0); /* 0x207494c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][20], 0x18489); /* 0x2074950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][21], 0x16347); /* 0x2074954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][22], 0xfbfe); /* 0x2074958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][23], 0x30ed0); /* 0x207495c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][24], 0xcfdd); /* 0x2074960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][25], 0xe5cf); /* 0x2074964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][26], 0x109f7); /* 0x2074968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][27], 0x2a2b1); /* 0x207496c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][28], 0x22b7c); /* 0x2074970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][29], 0x22283); /* 0x2074974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][30], 0x6f64); /* 0x2074978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][31], 0xfab3); /* 0x207497c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][32], 0xb6a7); /* 0x2074980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][33], 0x3ffff); /* 0x2074984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][34], 0x17b36); /* 0x2074988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][35], 0x54); /* 0x207498c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][36], 0x298e3); /* 0x2074990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][37], 0x3e228); /* 0x2074994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][38], 0x343d4); /* 0x2074998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][39], 0x19bb2); /* 0x207499c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][40], 0xedfb); /* 0x20749a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][41], 0x5024); /* 0x20749a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][42], 0x1afbf); /* 0x20749a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][43], 0xc7df); /* 0x20749ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][44], 0x39ac4); /* 0x20749b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][45], 0x1b88c); /* 0x20749b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][46], 0x7bda); /* 0x20749b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][47], 0x353ce); /* 0x20749bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][48], 0x3142); /* 0x20749c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][49], 0x1d43e); /* 0x20749c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][50], 0x2fde6); /* 0x20749c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][51], 0x177a); /* 0x20749cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][0], 0x17ff0); /* 0x2074a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][1], 0x3e26f); /* 0x2074a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][2], 0x16d9); /* 0x2074a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][3], 0x3926b); /* 0x2074a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][4], 0x2fe81); /* 0x2074a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][5], 0x3921c); /* 0x2074a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][6], 0x329e7); /* 0x2074a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][7], 0x1a5c8); /* 0x2074a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][8], 0x3bb54); /* 0x2074a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][9], 0xb5e6); /* 0x2074a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][10], 0x3b735); /* 0x2074a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][11], 0x2bbd1); /* 0x2074a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][12], 0x2a9a0); /* 0x2074a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][13], 0x2f40e); /* 0x2074a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][14], 0x20782); /* 0x2074a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][15], 0x1bde3); /* 0x2074a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][16], 0x2b0ad); /* 0x2074a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][17], 0x38d69); /* 0x2074a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][18], 0x2c9c6); /* 0x2074a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][19], 0x325d0); /* 0x2074a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][20], 0x3c417); /* 0x2074a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][21], 0x249a3); /* 0x2074a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][22], 0x15baf); /* 0x2074a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][23], 0x3e8e2); /* 0x2074a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][24], 0x392b8); /* 0x2074a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][25], 0x21d1e); /* 0x2074a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][26], 0x1c207); /* 0x2074a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][27], 0x9715); /* 0x2074a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][28], 0x2a540); /* 0x2074a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][29], 0x1d15c); /* 0x2074a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][30], 0xeeea); /* 0x2074a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][31], 0x369f5); /* 0x2074a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][32], 0x19a69); /* 0x2074a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][33], 0x32766); /* 0x2074a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][34], 0x105b9); /* 0x2074a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][35], 0x36678); /* 0x2074a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][36], 0x34c65); /* 0x2074a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][37], 0xdfde); /* 0x2074a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][38], 0xe6b); /* 0x2074a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][39], 0xf064); /* 0x2074a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][40], 0x36bc3); /* 0x2074aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][41], 0x1f207); /* 0x2074aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][42], 0x31441); /* 0x2074aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][43], 0x21e53); /* 0x2074aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][44], 0x29f9f); /* 0x2074ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][45], 0x25e99); /* 0x2074ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][46], 0x36c49); /* 0x2074ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][47], 0xd0f2); /* 0x2074abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][48], 0x34af7); /* 0x2074ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][49], 0x39c53); /* 0x2074ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][50], 0x36c0f); /* 0x2074ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][51], 0x195a5); /* 0x2074acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][0], 0xc785); /* 0x2074b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][1], 0xc3be); /* 0x2074b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][2], 0x3940e); /* 0x2074b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][3], 0xc1ef); /* 0x2074b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][4], 0x10b07); /* 0x2074b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][5], 0x1fdb9); /* 0x2074b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][6], 0x224f9); /* 0x2074b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][7], 0x34cc1); /* 0x2074b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][8], 0x2ef1e); /* 0x2074b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][9], 0x2a852); /* 0x2074b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][10], 0x17057); /* 0x2074b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][11], 0x2ecdd); /* 0x2074b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][12], 0x33cf3); /* 0x2074b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][13], 0x10d1f); /* 0x2074b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][14], 0x2b977); /* 0x2074b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][15], 0x154c6); /* 0x2074b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][16], 0x2b6f5); /* 0x2074b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][17], 0x201b2); /* 0x2074b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][18], 0x271ee); /* 0x2074b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][19], 0x3a1e3); /* 0x2074b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][20], 0x257bd); /* 0x2074b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][21], 0x7490); /* 0x2074b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][22], 0x24433); /* 0x2074b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][23], 0x213b7); /* 0x2074b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][24], 0x16965); /* 0x2074b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][25], 0x20d50); /* 0x2074b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][26], 0x14526); /* 0x2074b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][27], 0xed83); /* 0x2074b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][28], 0x1e38a); /* 0x2074b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][29], 0x376fb); /* 0x2074b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][30], 0x1e68e); /* 0x2074b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][31], 0x6fc1); /* 0x2074b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][32], 0x3be00); /* 0x2074b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][33], 0x1a77d); /* 0x2074b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][34], 0x1a885); /* 0x2074b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][35], 0xe7a); /* 0x2074b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][36], 0x1185b); /* 0x2074b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][37], 0x161f0); /* 0x2074b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][38], 0xcf3d); /* 0x2074b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][39], 0x320a2); /* 0x2074b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][40], 0x3380); /* 0x2074ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][41], 0x3fe7a); /* 0x2074ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][42], 0xd5a7); /* 0x2074ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][43], 0x3dcf8); /* 0x2074bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][44], 0x2d0c); /* 0x2074bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][45], 0x1e2a6); /* 0x2074bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][46], 0x3c362); /* 0x2074bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][47], 0x28eff); /* 0x2074bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][48], 0x3c45d); /* 0x2074bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][49], 0x1cabb); /* 0x2074bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][50], 0x10eec); /* 0x2074bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][51], 0x2922d); /* 0x2074bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][0], 0x37ee9); /* 0x2074c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][1], 0xd592); /* 0x2074c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][2], 0xcd63); /* 0x2074c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][3], 0x1aa68); /* 0x2074c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][4], 0x12177); /* 0x2074c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][5], 0x22e9e); /* 0x2074c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][6], 0x32b4f); /* 0x2074c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][7], 0x1b08d); /* 0x2074c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][8], 0x1ab5); /* 0x2074c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][9], 0x313fe); /* 0x2074c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][10], 0x327e8); /* 0x2074c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][11], 0x316db); /* 0x2074c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][12], 0x8726); /* 0x2074c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][13], 0x2a7e); /* 0x2074c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][14], 0x3fcee); /* 0x2074c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][15], 0x180bb); /* 0x2074c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][16], 0x1bc90); /* 0x2074c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][17], 0xc6a8); /* 0x2074c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][18], 0x3659); /* 0x2074c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][19], 0x399); /* 0x2074c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][20], 0x290d8); /* 0x2074c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][21], 0xdac); /* 0x2074c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][22], 0x2ccd2); /* 0x2074c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][23], 0x5816); /* 0x2074c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][24], 0x4b20); /* 0x2074c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][25], 0x5433); /* 0x2074c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][26], 0x1b1d8); /* 0x2074c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][27], 0x22d04); /* 0x2074c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][28], 0x450a); /* 0x2074c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][29], 0x38647); /* 0x2074c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][30], 0x8007); /* 0x2074c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][31], 0x33757); /* 0x2074c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][32], 0x34834); /* 0x2074c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][33], 0x1d7fb); /* 0x2074c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][34], 0x14475); /* 0x2074c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][35], 0x12a); /* 0x2074c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][36], 0x27053); /* 0x2074c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][37], 0x2f4c4); /* 0x2074c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][38], 0x3a941); /* 0x2074c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][39], 0x168d1); /* 0x2074c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][40], 0x24190); /* 0x2074ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][41], 0x2c7b3); /* 0x2074ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][42], 0x69c); /* 0x2074ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][43], 0x14ca6); /* 0x2074cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][44], 0x80e6); /* 0x2074cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][45], 0x32d2c); /* 0x2074cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][46], 0x97c9); /* 0x2074cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][47], 0xd435); /* 0x2074cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][48], 0x59a2); /* 0x2074cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][49], 0x104ba); /* 0x2074cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][50], 0xd6a3); /* 0x2074cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][51], 0x1d3c8); /* 0x2074ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][0], 0x3740d); /* 0x2074d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][1], 0x38e6f); /* 0x2074d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][2], 0x1dbf); /* 0x2074d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][3], 0x3e1b6); /* 0x2074d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][4], 0x3911e); /* 0x2074d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][5], 0x30987); /* 0x2074d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][6], 0x34029); /* 0x2074d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][7], 0x3f81a); /* 0x2074d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][8], 0x6de); /* 0x2074d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][9], 0xad5); /* 0x2074d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][10], 0x2bfec); /* 0x2074d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][11], 0x31425); /* 0x2074d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][12], 0x11ecd); /* 0x2074d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][13], 0xf88e); /* 0x2074d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][14], 0x1281c); /* 0x2074d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][15], 0xedca); /* 0x2074d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][16], 0xb30c); /* 0x2074d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][17], 0xfa3e); /* 0x2074d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][18], 0x5f24); /* 0x2074d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][19], 0x32d5c); /* 0x2074d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][20], 0x25587); /* 0x2074d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][21], 0x20254); /* 0x2074d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][22], 0x1135); /* 0x2074d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][23], 0x39db); /* 0x2074d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][24], 0x1b3ad); /* 0x2074d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][25], 0x3a5c1); /* 0x2074d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][26], 0x1a14a); /* 0x2074d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][27], 0x31e43); /* 0x2074d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][28], 0x1d32a); /* 0x2074d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][29], 0x1fec8); /* 0x2074d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][30], 0x33322); /* 0x2074d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][31], 0x3282d); /* 0x2074d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][32], 0x1b4a9); /* 0x2074d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][33], 0x39277); /* 0x2074d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][34], 0x2746c); /* 0x2074d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][35], 0x1fbc5); /* 0x2074d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][36], 0x2f77c); /* 0x2074d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][37], 0x2e687); /* 0x2074d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][38], 0x2278e); /* 0x2074d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][39], 0x19930); /* 0x2074d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][40], 0x102fd); /* 0x2074da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][41], 0x3c55e); /* 0x2074da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][42], 0x3cfe2); /* 0x2074da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][43], 0x2e6b2); /* 0x2074dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][44], 0x18919); /* 0x2074db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][45], 0xb004); /* 0x2074db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][46], 0x34399); /* 0x2074db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][47], 0x21b30); /* 0x2074dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][48], 0x2b79a); /* 0x2074dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][49], 0x36c2a); /* 0x2074dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][50], 0x16577); /* 0x2074dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][51], 0x25847); /* 0x2074dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][0], 0x36989); /* 0x2074e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][1], 0x39039); /* 0x2074e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][2], 0x18fa6); /* 0x2074e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][3], 0xd459); /* 0x2074e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][4], 0x31136); /* 0x2074e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][5], 0x2c4d4); /* 0x2074e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][6], 0x4c63); /* 0x2074e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][7], 0x264f8); /* 0x2074e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][8], 0x3a55c); /* 0x2074e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][9], 0x358dc); /* 0x2074e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][10], 0x2be87); /* 0x2074e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][11], 0x668d); /* 0x2074e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][12], 0x20390); /* 0x2074e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][13], 0x1bb0f); /* 0x2074e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][14], 0x116b8); /* 0x2074e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][15], 0xdfb3); /* 0x2074e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][16], 0x2833f); /* 0x2074e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][17], 0x12bd8); /* 0x2074e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][18], 0x366e2); /* 0x2074e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][19], 0x3e410); /* 0x2074e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][20], 0x3607a); /* 0x2074e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][21], 0x2b511); /* 0x2074e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][22], 0x328a2); /* 0x2074e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][23], 0x978f); /* 0x2074e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][24], 0x2cacd); /* 0x2074e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][25], 0x1b97); /* 0x2074e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][26], 0xe22); /* 0x2074e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][27], 0xb962); /* 0x2074e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][28], 0x19f62); /* 0x2074e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][29], 0x3456b); /* 0x2074e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][30], 0x1a9ba); /* 0x2074e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][31], 0xe420); /* 0x2074e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][32], 0x1a9e1); /* 0x2074e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][33], 0x21429); /* 0x2074e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][34], 0x2ff2a); /* 0x2074e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][35], 0x17001); /* 0x2074e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][36], 0x37cbc); /* 0x2074e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][37], 0x12a23); /* 0x2074e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][38], 0x2b7a0); /* 0x2074e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][39], 0x20864); /* 0x2074e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][40], 0x3cf7b); /* 0x2074ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][41], 0x1d3ca); /* 0x2074ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][42], 0x12f5); /* 0x2074ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][43], 0x143b4); /* 0x2074eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][44], 0x1c747); /* 0x2074eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][45], 0x1d7d9); /* 0x2074eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][46], 0x1a81b); /* 0x2074eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][47], 0xb3c6); /* 0x2074ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][48], 0x17169); /* 0x2074ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][49], 0x21b73); /* 0x2074ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][50], 0x1a25e); /* 0x2074ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][51], 0x2510d); /* 0x2074ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][0], 0x8a47); /* 0x2074f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][1], 0x20fc2); /* 0x2074f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][2], 0x1ead6); /* 0x2074f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][3], 0x265ad); /* 0x2074f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][4], 0x3660c); /* 0x2074f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][5], 0x1db46); /* 0x2074f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][6], 0xfafd); /* 0x2074f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][7], 0x19658); /* 0x2074f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][8], 0x2a5d9); /* 0x2074f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][9], 0x3b431); /* 0x2074f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][10], 0x13fa0); /* 0x2074f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][11], 0x3686b); /* 0x2074f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][12], 0x2be3d); /* 0x2074f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][13], 0x1358f); /* 0x2074f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][14], 0x3e3c2); /* 0x2074f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][15], 0x3cff6); /* 0x2074f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][16], 0x1a29f); /* 0x2074f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][17], 0x27016); /* 0x2074f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][18], 0x2f578); /* 0x2074f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][19], 0xefb2); /* 0x2074f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][20], 0x2e8a3); /* 0x2074f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][21], 0xe065); /* 0x2074f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][22], 0x22843); /* 0x2074f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][23], 0xc623); /* 0x2074f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][24], 0x5249); /* 0x2074f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][25], 0x10ac4); /* 0x2074f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][26], 0x22c8a); /* 0x2074f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][27], 0x19ea3); /* 0x2074f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][28], 0x1cd84); /* 0x2074f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][29], 0x2cede); /* 0x2074f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][30], 0x228f1); /* 0x2074f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][31], 0xe9e); /* 0x2074f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][32], 0x11ab3); /* 0x2074f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][33], 0xf21e); /* 0x2074f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][34], 0x3b9e8); /* 0x2074f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][35], 0x20595); /* 0x2074f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][36], 0xe714); /* 0x2074f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][37], 0x1c933); /* 0x2074f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][38], 0x19abf); /* 0x2074f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][39], 0x4782); /* 0x2074f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][40], 0x13d23); /* 0x2074fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][41], 0x83a4); /* 0x2074fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][42], 0x3a3f8); /* 0x2074fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][43], 0x2cb49); /* 0x2074fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][44], 0x1dab0); /* 0x2074fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][45], 0x3d894); /* 0x2074fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][46], 0x1d929); /* 0x2074fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][47], 0x119a7); /* 0x2074fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][48], 0x7fd0); /* 0x2074fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][49], 0xd4ff); /* 0x2074fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][50], 0x19ec4); /* 0x2074fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][51], 0x2a6a2); /* 0x2074fcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][0], 0x11435); /* 0x2075000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][1], 0x380fc); /* 0x2075004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][2], 0x9c39); /* 0x2075008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][3], 0x28037); /* 0x207500c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][4], 0x306c5); /* 0x2075010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][5], 0x2be73); /* 0x2075014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][6], 0x36fdf); /* 0x2075018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][7], 0x1e54a); /* 0x207501c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][8], 0xe63b); /* 0x2075020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][9], 0x1beb0); /* 0x2075024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][10], 0x2ed11); /* 0x2075028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][11], 0x6c88); /* 0x207502c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][12], 0x11604); /* 0x2075030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][13], 0x2fbe3); /* 0x2075034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][14], 0x3cff6); /* 0x2075038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][15], 0x1d1a); /* 0x207503c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][16], 0x30ed9); /* 0x2075040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][17], 0x2cc04); /* 0x2075044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][18], 0x34dc8); /* 0x2075048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][19], 0x20fb3); /* 0x207504c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][20], 0x80ea); /* 0x2075050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][21], 0x34a91); /* 0x2075054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][22], 0x3e2b6); /* 0x2075058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][23], 0x3f56e); /* 0x207505c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][24], 0x2793f); /* 0x2075060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][25], 0x20fc0); /* 0x2075064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][26], 0x6454); /* 0x2075068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][27], 0x28d2); /* 0x207506c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][28], 0x121a); /* 0x2075070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][29], 0x2901b); /* 0x2075074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][30], 0x22ee2); /* 0x2075078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][31], 0xe5a3); /* 0x207507c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][32], 0x9fcc); /* 0x2075080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][33], 0x29ee1); /* 0x2075084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][34], 0x8925); /* 0x2075088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][35], 0x15540); /* 0x207508c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][36], 0x32ef6); /* 0x2075090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][37], 0x11482); /* 0x2075094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][38], 0x2bb9b); /* 0x2075098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][39], 0x1a237); /* 0x207509c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][40], 0xf369); /* 0x20750a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][41], 0x14a7); /* 0x20750a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][42], 0x15fa7); /* 0x20750a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][43], 0x7826); /* 0x20750ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][44], 0x142a4); /* 0x20750b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][45], 0x27d5e); /* 0x20750b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][46], 0x1af16); /* 0x20750b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][47], 0x17c5); /* 0x20750bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][48], 0x311fc); /* 0x20750c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][49], 0x33cdf); /* 0x20750c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][50], 0x179a7); /* 0x20750c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][51], 0x2bffe); /* 0x20750cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][0], 0x2c1ba); /* 0x2075100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][1], 0x2acc5); /* 0x2075104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][2], 0x1829b); /* 0x2075108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][3], 0xe08); /* 0x207510c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][4], 0x2eaee); /* 0x2075110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][5], 0x26fbb); /* 0x2075114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][6], 0x3ad8a); /* 0x2075118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][7], 0x31616); /* 0x207511c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][8], 0x20fcf); /* 0x2075120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][9], 0x3e0c); /* 0x2075124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][10], 0x2b051); /* 0x2075128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][11], 0x20f23); /* 0x207512c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][12], 0x33776); /* 0x2075130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][13], 0x8dcf); /* 0x2075134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][14], 0x1cdcd); /* 0x2075138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][15], 0x11e38); /* 0x207513c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][16], 0x2cb1f); /* 0x2075140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][17], 0xafb5); /* 0x2075144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][18], 0x39a92); /* 0x2075148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][19], 0x10808); /* 0x207514c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][20], 0x18dc1); /* 0x2075150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][21], 0x1e617); /* 0x2075154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][22], 0x4248); /* 0x2075158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][23], 0x1d5c5); /* 0x207515c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][24], 0x22940); /* 0x2075160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][25], 0x20343); /* 0x2075164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][26], 0xfe32); /* 0x2075168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][27], 0x2836d); /* 0x207516c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][28], 0x13c2d); /* 0x2075170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][29], 0x385a9); /* 0x2075174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][30], 0x1b8df); /* 0x2075178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][31], 0xc15b); /* 0x207517c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][32], 0x34825); /* 0x2075180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][33], 0x1b026); /* 0x2075184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][34], 0x11513); /* 0x2075188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][35], 0x3c3dd); /* 0x207518c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][36], 0x5d0c); /* 0x2075190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][37], 0x22e80); /* 0x2075194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][38], 0x25406); /* 0x2075198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][39], 0x29260); /* 0x207519c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][40], 0x211a0); /* 0x20751a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][41], 0x361de); /* 0x20751a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][42], 0xc7d0); /* 0x20751a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][43], 0x35c6d); /* 0x20751ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][44], 0x3e8b3); /* 0x20751b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][45], 0x37c1e); /* 0x20751b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][46], 0x34c2d); /* 0x20751b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][47], 0x28d49); /* 0x20751bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][48], 0xc507); /* 0x20751c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][49], 0x26304); /* 0x20751c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][50], 0x6247); /* 0x20751c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][51], 0x20a3c); /* 0x20751cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][0], 0x2b0a3); /* 0x2075200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][1], 0x22ff0); /* 0x2075204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][2], 0x5c2e); /* 0x2075208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][3], 0xe763); /* 0x207520c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][4], 0x24cb6); /* 0x2075210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][5], 0xb7e4); /* 0x2075214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][6], 0x38b6e); /* 0x2075218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][7], 0x15a6); /* 0x207521c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][8], 0x2bbbf); /* 0x2075220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][9], 0x1710); /* 0x2075224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][10], 0x1aeff); /* 0x2075228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][11], 0x35a8); /* 0x207522c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][12], 0x202d8); /* 0x2075230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][13], 0x12621); /* 0x2075234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][14], 0x286de); /* 0x2075238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][15], 0x2446d); /* 0x207523c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][16], 0x2c2c2); /* 0x2075240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][17], 0x4b06); /* 0x2075244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][18], 0x1cd65); /* 0x2075248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][19], 0xe8ed); /* 0x207524c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][20], 0x326c0); /* 0x2075250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][21], 0x17c6f); /* 0x2075254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][22], 0x12bc); /* 0x2075258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][23], 0x21c1f); /* 0x207525c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][24], 0x1788d); /* 0x2075260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][25], 0xf6f); /* 0x2075264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][26], 0x3da2a); /* 0x2075268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][27], 0x7684); /* 0x207526c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][28], 0x3d82d); /* 0x2075270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][29], 0x31899); /* 0x2075274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][30], 0x3601e); /* 0x2075278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][31], 0x3c13f); /* 0x207527c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][32], 0x279f3); /* 0x2075280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][33], 0x3b4d7); /* 0x2075284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][34], 0x3e1e0); /* 0x2075288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][35], 0xb8d4); /* 0x207528c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][36], 0xc502); /* 0x2075290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][37], 0x2adc7); /* 0x2075294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][38], 0x2a681); /* 0x2075298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][39], 0x114d9); /* 0x207529c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][40], 0x20875); /* 0x20752a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][41], 0x29cab); /* 0x20752a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][42], 0x11eab); /* 0x20752a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][43], 0x15211); /* 0x20752ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][44], 0x14660); /* 0x20752b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][45], 0x749f); /* 0x20752b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][46], 0x3e754); /* 0x20752b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][47], 0x13559); /* 0x20752bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][48], 0x1be70); /* 0x20752c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][49], 0x30d17); /* 0x20752c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][50], 0x2e198); /* 0x20752c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][51], 0x159dd); /* 0x20752cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][0], 0x1ff5); /* 0x2075300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][1], 0x14649); /* 0x2075304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][2], 0x727f); /* 0x2075308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][3], 0x1f448); /* 0x207530c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][4], 0x24a01); /* 0x2075310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][5], 0x1fd73); /* 0x2075314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][6], 0x1c766); /* 0x2075318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][7], 0x1a967); /* 0x207531c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][8], 0x348bf); /* 0x2075320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][9], 0xb431); /* 0x2075324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][10], 0x1858d); /* 0x2075328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][11], 0x36d20); /* 0x207532c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][12], 0xa906); /* 0x2075330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][13], 0xa6b3); /* 0x2075334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][14], 0x9407); /* 0x2075338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][15], 0x39d48); /* 0x207533c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][16], 0xcfa); /* 0x2075340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][17], 0x3721b); /* 0x2075344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][18], 0x2eb0); /* 0x2075348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][19], 0x2120b); /* 0x207534c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][20], 0x3c421); /* 0x2075350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][21], 0x3feff); /* 0x2075354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][22], 0x3b802); /* 0x2075358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][23], 0x33cd6); /* 0x207535c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][24], 0x11205); /* 0x2075360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][25], 0x77bc); /* 0x2075364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][26], 0xcb8e); /* 0x2075368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][27], 0x2530f); /* 0x207536c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][28], 0x1b782); /* 0x2075370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][29], 0x2dfbe); /* 0x2075374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][30], 0xe60d); /* 0x2075378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][31], 0x30b0c); /* 0x207537c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][32], 0x3b8ac); /* 0x2075380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][33], 0x3f403); /* 0x2075384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][34], 0x30364); /* 0x2075388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][35], 0x32bfc); /* 0x207538c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][36], 0x1140e); /* 0x2075390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][37], 0x1397c); /* 0x2075394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][38], 0x9a0b); /* 0x2075398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][39], 0x39029); /* 0x207539c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][40], 0x2c218); /* 0x20753a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][41], 0x2394b); /* 0x20753a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][42], 0x2d107); /* 0x20753a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][43], 0xc8e); /* 0x20753ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][44], 0x10c15); /* 0x20753b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][45], 0x2b915); /* 0x20753b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][46], 0x3564); /* 0x20753b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][47], 0x30160); /* 0x20753bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][48], 0x7cab); /* 0x20753c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][49], 0xc3ad); /* 0x20753c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][50], 0x1308f); /* 0x20753c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][51], 0x2b24d); /* 0x20753cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][0], 0x2ede); /* 0x2075400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][1], 0x18dc6); /* 0x2075404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][2], 0xaf60); /* 0x2075408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][3], 0x239a); /* 0x207540c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][4], 0x3500); /* 0x2075410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][5], 0x1b627); /* 0x2075414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][6], 0xb914); /* 0x2075418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][7], 0x32d67); /* 0x207541c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][8], 0x1c0c7); /* 0x2075420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][9], 0x1df7c); /* 0x2075424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][10], 0x37f90); /* 0x2075428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][11], 0x350bc); /* 0x207542c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][12], 0x17d02); /* 0x2075430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][13], 0x3524d); /* 0x2075434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][14], 0xc85b); /* 0x2075438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][15], 0x180ba); /* 0x207543c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][16], 0x232eb); /* 0x2075440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][17], 0x105e5); /* 0x2075444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][18], 0x1bee7); /* 0x2075448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][19], 0xf200); /* 0x207544c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][20], 0x14002); /* 0x2075450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][21], 0x2d2a6); /* 0x2075454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][22], 0x1120f); /* 0x2075458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][23], 0xf4a6); /* 0x207545c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][24], 0x37ac1); /* 0x2075460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][25], 0x2e493); /* 0x2075464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][26], 0xf5c); /* 0x2075468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][27], 0x303d); /* 0x207546c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][28], 0xb0cd); /* 0x2075470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][29], 0x1288); /* 0x2075474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][30], 0x31606); /* 0x2075478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][31], 0x17b9a); /* 0x207547c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][32], 0x7c1f); /* 0x2075480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][33], 0x3b448); /* 0x2075484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][34], 0x103d8); /* 0x2075488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][35], 0x265f1); /* 0x207548c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][36], 0x28689); /* 0x2075490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][37], 0x39415); /* 0x2075494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][38], 0x1005); /* 0x2075498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][39], 0x398e2); /* 0x207549c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][40], 0x37392); /* 0x20754a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][41], 0x38e36); /* 0x20754a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][42], 0x31b99); /* 0x20754a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][43], 0x25298); /* 0x20754ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][44], 0x1367d); /* 0x20754b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][45], 0x4269); /* 0x20754b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][46], 0x3c796); /* 0x20754b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][47], 0x4d48); /* 0x20754bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][48], 0x364b5); /* 0x20754c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][49], 0x1302); /* 0x20754c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][50], 0x800a); /* 0x20754c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][51], 0x3a53b); /* 0x20754cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][0], 0x8e54); /* 0x2075500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][1], 0x309de); /* 0x2075504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][2], 0x1e2c2); /* 0x2075508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][3], 0xc0e4); /* 0x207550c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][4], 0x36f4a); /* 0x2075510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][5], 0x13fd5); /* 0x2075514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][6], 0x33a1e); /* 0x2075518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][7], 0x105e8); /* 0x207551c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][8], 0x14083); /* 0x2075520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][9], 0x230fc); /* 0x2075524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][10], 0x35859); /* 0x2075528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][11], 0xddab); /* 0x207552c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][12], 0x19dc8); /* 0x2075530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][13], 0x2f617); /* 0x2075534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][14], 0x26b08); /* 0x2075538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][15], 0x259d2); /* 0x207553c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][16], 0x2057b); /* 0x2075540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][17], 0x2320e); /* 0x2075544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][18], 0x3a94f); /* 0x2075548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][19], 0x2f889); /* 0x207554c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][20], 0xe0b1); /* 0x2075550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][21], 0x2b559); /* 0x2075554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][22], 0x1d04e); /* 0x2075558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][23], 0xd094); /* 0x207555c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][24], 0x2097e); /* 0x2075560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][25], 0x33a22); /* 0x2075564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][26], 0x3d948); /* 0x2075568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][27], 0x3923d); /* 0x207556c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][28], 0xc0be); /* 0x2075570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][29], 0x3ad91); /* 0x2075574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][30], 0xbe02); /* 0x2075578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][31], 0x16328); /* 0x207557c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][32], 0x1f84); /* 0x2075580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][33], 0x3e64); /* 0x2075584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][34], 0x29a5b); /* 0x2075588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][35], 0x1bc87); /* 0x207558c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][36], 0x20b88); /* 0x2075590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][37], 0x1da6a); /* 0x2075594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][38], 0x3b898); /* 0x2075598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][39], 0x21a6c); /* 0x207559c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][40], 0x6daa); /* 0x20755a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][41], 0x242d3); /* 0x20755a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][42], 0x1882f); /* 0x20755a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][43], 0x14ff); /* 0x20755ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][44], 0x3b1b4); /* 0x20755b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][45], 0x1b369); /* 0x20755b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][46], 0xb593); /* 0x20755b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][47], 0x1f3a9); /* 0x20755bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][48], 0x5302); /* 0x20755c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][49], 0x430d); /* 0x20755c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][50], 0x24a88); /* 0x20755c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][51], 0xbe87); /* 0x20755cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][0], 0xcfb); /* 0x2075600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][1], 0x311db); /* 0x2075604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][2], 0x10d20); /* 0x2075608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][3], 0x38be7); /* 0x207560c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][4], 0x52dc); /* 0x2075610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][5], 0x1da0b); /* 0x2075614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][6], 0x25034); /* 0x2075618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][7], 0x394f2); /* 0x207561c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][8], 0x32a88); /* 0x2075620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][9], 0xc427); /* 0x2075624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][10], 0x2cda6); /* 0x2075628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][11], 0x399e5); /* 0x207562c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][12], 0x2143b); /* 0x2075630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][13], 0xd705); /* 0x2075634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][14], 0x222e); /* 0x2075638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][15], 0x29f20); /* 0x207563c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][16], 0x2a651); /* 0x2075640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][17], 0x1740b); /* 0x2075644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][18], 0x19449); /* 0x2075648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][19], 0x1dcd4); /* 0x207564c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][20], 0x2ddc3); /* 0x2075650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][21], 0x2e597); /* 0x2075654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][22], 0x368a3); /* 0x2075658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][23], 0x1ab29); /* 0x207565c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][24], 0x36478); /* 0x2075660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][25], 0xdaa9); /* 0x2075664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][26], 0x2b88d); /* 0x2075668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][27], 0x286a9); /* 0x207566c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][28], 0x1ad14); /* 0x2075670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][29], 0x24bcb); /* 0x2075674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][30], 0x260e3); /* 0x2075678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][31], 0x46e); /* 0x207567c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][32], 0x2025b); /* 0x2075680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][33], 0x36969); /* 0x2075684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][34], 0x809c); /* 0x2075688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][35], 0x17dd3); /* 0x207568c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][36], 0x256a7); /* 0x2075690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][37], 0x2797a); /* 0x2075694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][38], 0x3ebc1); /* 0x2075698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][39], 0x3e380); /* 0x207569c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][40], 0x22af1); /* 0x20756a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][41], 0xd1ce); /* 0x20756a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][42], 0x34e3f); /* 0x20756a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][43], 0x2550e); /* 0x20756ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][44], 0x1027c); /* 0x20756b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][45], 0x194fe); /* 0x20756b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][46], 0xd55e); /* 0x20756b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][47], 0x26f4c); /* 0x20756bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][48], 0x1e7c6); /* 0x20756c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][49], 0x1d811); /* 0x20756c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][50], 0xb19e); /* 0x20756c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][51], 0x22f20); /* 0x20756cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][0], 0x3fd5e); /* 0x2075700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][1], 0x32f49); /* 0x2075704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][2], 0x25f8b); /* 0x2075708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][3], 0x2d3f3); /* 0x207570c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][4], 0x22583); /* 0x2075710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][5], 0x3caca); /* 0x2075714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][6], 0x9a73); /* 0x2075718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][7], 0x26961); /* 0x207571c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][8], 0x3699f); /* 0x2075720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][9], 0x1aef5); /* 0x2075724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][10], 0x2ee07); /* 0x2075728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][11], 0x2838); /* 0x207572c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][12], 0x2d7cb); /* 0x2075730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][13], 0x2602f); /* 0x2075734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][14], 0x8a57); /* 0x2075738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][15], 0x23c5c); /* 0x207573c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][16], 0x27c15); /* 0x2075740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][17], 0x26ad2); /* 0x2075744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][18], 0x18f39); /* 0x2075748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][19], 0x10e72); /* 0x207574c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][20], 0x1cf36); /* 0x2075750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][21], 0x31ccd); /* 0x2075754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][22], 0x298c5); /* 0x2075758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][23], 0x11e5e); /* 0x207575c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][24], 0x29fa2); /* 0x2075760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][25], 0x2740b); /* 0x2075764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][26], 0x3b7f6); /* 0x2075768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][27], 0x3b247); /* 0x207576c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][28], 0x36234); /* 0x2075770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][29], 0x4b2f); /* 0x2075774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][30], 0x37b96); /* 0x2075778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][31], 0x3cc43); /* 0x207577c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][32], 0xfe7e); /* 0x2075780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][33], 0x1e7bc); /* 0x2075784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][34], 0x11f1); /* 0x2075788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][35], 0xa6d1); /* 0x207578c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][36], 0x3379b); /* 0x2075790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][37], 0xc33e); /* 0x2075794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][38], 0x2d21d); /* 0x2075798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][39], 0x234d8); /* 0x207579c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][40], 0x2dbdd); /* 0x20757a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][41], 0x4c0d); /* 0x20757a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][42], 0x3cfd8); /* 0x20757a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][43], 0x264ea); /* 0x20757ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][44], 0x10d14); /* 0x20757b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][45], 0x1fd25); /* 0x20757b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][46], 0x1bc62); /* 0x20757b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][47], 0x20ba8); /* 0x20757bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][48], 0x1a776); /* 0x20757c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][49], 0x143bb); /* 0x20757c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][50], 0x3c4bf); /* 0x20757c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][51], 0x146b1); /* 0x20757cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][0], 0x17cfe); /* 0x2075800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][1], 0x2d768); /* 0x2075804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][2], 0x14c6); /* 0x2075808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][3], 0x6e76); /* 0x207580c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][4], 0x15846); /* 0x2075810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][5], 0x342ad); /* 0x2075814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][6], 0x194da); /* 0x2075818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][7], 0x204d9); /* 0x207581c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][8], 0x33623); /* 0x2075820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][9], 0x10157); /* 0x2075824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][10], 0x16540); /* 0x2075828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][11], 0x2bc66); /* 0x207582c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][12], 0x3a398); /* 0x2075830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][13], 0x132fd); /* 0x2075834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][14], 0x1e83e); /* 0x2075838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][15], 0x28d3d); /* 0x207583c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][16], 0x94cf); /* 0x2075840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][17], 0x31976); /* 0x2075844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][18], 0x2249b); /* 0x2075848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][19], 0x480a); /* 0x207584c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][20], 0x2dcf6); /* 0x2075850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][21], 0x1958); /* 0x2075854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][22], 0x2484b); /* 0x2075858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][23], 0x32093); /* 0x207585c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][24], 0x32cde); /* 0x2075860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][25], 0xa742); /* 0x2075864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][26], 0x32428); /* 0x2075868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][27], 0x374eb); /* 0x207586c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][28], 0x2ecec); /* 0x2075870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][29], 0x8f0); /* 0x2075874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][30], 0xad05); /* 0x2075878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][31], 0x17cf9); /* 0x207587c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][32], 0x17c4c); /* 0x2075880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][33], 0x2cfa1); /* 0x2075884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][34], 0x2181e); /* 0x2075888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][35], 0x3bb98); /* 0x207588c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][36], 0xc038); /* 0x2075890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][37], 0x344df); /* 0x2075894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][38], 0x2ab19); /* 0x2075898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][39], 0x24f8a); /* 0x207589c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][40], 0x23277); /* 0x20758a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][41], 0x38f93); /* 0x20758a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][42], 0x3215a); /* 0x20758a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][43], 0x1b8ef); /* 0x20758ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][44], 0x2b5d0); /* 0x20758b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][45], 0xef2a); /* 0x20758b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][46], 0xef74); /* 0x20758b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][47], 0x2f79a); /* 0x20758bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][48], 0xcfe3); /* 0x20758c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][49], 0x204bd); /* 0x20758c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][50], 0x30239); /* 0x20758c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][51], 0x53b1); /* 0x20758cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][0], 0x3d7c2); /* 0x2075900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][1], 0xbe03); /* 0x2075904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][2], 0x36692); /* 0x2075908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][3], 0x16be9); /* 0x207590c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][4], 0x33f25); /* 0x2075910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][5], 0x315d9); /* 0x2075914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][6], 0x3fef2); /* 0x2075918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][7], 0x2e2fd); /* 0x207591c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][8], 0x20215); /* 0x2075920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][9], 0x692); /* 0x2075924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][10], 0x213e6); /* 0x2075928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][11], 0x11ed4); /* 0x207592c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][12], 0x1c745); /* 0x2075930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][13], 0x22269); /* 0x2075934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][14], 0x1c1bd); /* 0x2075938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][15], 0x24075); /* 0x207593c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][16], 0x30772); /* 0x2075940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][17], 0x27035); /* 0x2075944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][18], 0x1242e); /* 0x2075948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][19], 0x27d98); /* 0x207594c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][20], 0x2b244); /* 0x2075950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][21], 0x24eda); /* 0x2075954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][22], 0x2a004); /* 0x2075958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][23], 0x251fa); /* 0x207595c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][24], 0x38547); /* 0x2075960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][25], 0x2f285); /* 0x2075964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][26], 0x1818f); /* 0x2075968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][27], 0x39c0); /* 0x207596c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][28], 0x1054); /* 0x2075970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][29], 0x34abe); /* 0x2075974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][30], 0x30b48); /* 0x2075978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][31], 0x286c3); /* 0x207597c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][32], 0x270bd); /* 0x2075980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][33], 0x2897b); /* 0x2075984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][34], 0x9677); /* 0x2075988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][35], 0x1f76a); /* 0x207598c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][36], 0x194ba); /* 0x2075990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][37], 0x88bd); /* 0x2075994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][38], 0x32124); /* 0x2075998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][39], 0x36f16); /* 0x207599c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][40], 0x21fb1); /* 0x20759a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][41], 0x4720); /* 0x20759a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][42], 0x3950a); /* 0x20759a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][43], 0x3571c); /* 0x20759ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][44], 0x34e); /* 0x20759b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][45], 0x1b23e); /* 0x20759b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][46], 0x2f4e3); /* 0x20759b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][47], 0x29a36); /* 0x20759bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][48], 0x12a4e); /* 0x20759c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][49], 0x36ddc); /* 0x20759c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][50], 0x3c41f); /* 0x20759c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][51], 0x32549); /* 0x20759cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][0], 0x22787); /* 0x2075a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][1], 0x1e65b); /* 0x2075a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][2], 0xef38); /* 0x2075a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][3], 0x3c6c8); /* 0x2075a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][4], 0x2d5ce); /* 0x2075a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][5], 0x9ea3); /* 0x2075a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][6], 0x328aa); /* 0x2075a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][7], 0x29238); /* 0x2075a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][8], 0x2063d); /* 0x2075a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][9], 0x372b); /* 0x2075a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][10], 0x1f444); /* 0x2075a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][11], 0x2a14f); /* 0x2075a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][12], 0x794); /* 0x2075a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][13], 0x31453); /* 0x2075a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][14], 0x3a6ad); /* 0x2075a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][15], 0x3697f); /* 0x2075a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][16], 0x27a7); /* 0x2075a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][17], 0x2a2e3); /* 0x2075a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][18], 0xf9b4); /* 0x2075a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][19], 0xc6d8); /* 0x2075a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][20], 0xffa6); /* 0x2075a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][21], 0x39752); /* 0x2075a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][22], 0x3661); /* 0x2075a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][23], 0xbd3f); /* 0x2075a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][24], 0xbc41); /* 0x2075a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][25], 0x34379); /* 0x2075a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][26], 0x149cb); /* 0x2075a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][27], 0x3d6b4); /* 0x2075a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][28], 0x2ecb0); /* 0x2075a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][29], 0x1e73a); /* 0x2075a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][30], 0x23906); /* 0x2075a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][31], 0x13b0c); /* 0x2075a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][32], 0xe126); /* 0x2075a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][33], 0x272c1); /* 0x2075a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][34], 0x221ee); /* 0x2075a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][35], 0x25664); /* 0x2075a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][36], 0x2ed97); /* 0x2075a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][37], 0x270b8); /* 0x2075a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][38], 0x164f7); /* 0x2075a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][39], 0x1068d); /* 0x2075a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][40], 0x19821); /* 0x2075aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][41], 0x2e9d4); /* 0x2075aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][42], 0x2187c); /* 0x2075aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][43], 0xf89d); /* 0x2075aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][44], 0x2ebdb); /* 0x2075ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][45], 0x1aef3); /* 0x2075ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][46], 0xcaca); /* 0x2075ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][47], 0x1fe54); /* 0x2075abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][48], 0x21206); /* 0x2075ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][49], 0x51b7); /* 0x2075ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][50], 0x3deb2); /* 0x2075ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][51], 0xe76a); /* 0x2075acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][0], 0x72f2); /* 0x2075b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][1], 0x1fa71); /* 0x2075b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][2], 0x1c045); /* 0x2075b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][3], 0x16a05); /* 0x2075b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][4], 0x12ff6); /* 0x2075b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][5], 0x22658); /* 0x2075b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][6], 0x6bb); /* 0x2075b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][7], 0x56ce); /* 0x2075b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][8], 0x39141); /* 0x2075b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][9], 0x4970); /* 0x2075b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][10], 0x24376); /* 0x2075b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][11], 0x25fe9); /* 0x2075b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][12], 0xf12d); /* 0x2075b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][13], 0xc9b2); /* 0x2075b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][14], 0x1df58); /* 0x2075b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][15], 0xbed8); /* 0x2075b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][16], 0x209a2); /* 0x2075b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][17], 0x1092e); /* 0x2075b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][18], 0x1ab04); /* 0x2075b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][19], 0x1028a); /* 0x2075b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][20], 0x13cf4); /* 0x2075b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][21], 0x9b4f); /* 0x2075b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][22], 0x20d20); /* 0x2075b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][23], 0x26d8a); /* 0x2075b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][24], 0x33961); /* 0x2075b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][25], 0x460f); /* 0x2075b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][26], 0x1a618); /* 0x2075b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][27], 0x2fe09); /* 0x2075b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][28], 0x357a8); /* 0x2075b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][29], 0x1b9a2); /* 0x2075b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][30], 0x2455a); /* 0x2075b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][31], 0x9ce8); /* 0x2075b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][32], 0x2f2c8); /* 0x2075b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][33], 0x3aa72); /* 0x2075b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][34], 0x16154); /* 0x2075b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][35], 0xc908); /* 0x2075b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][36], 0x2f08); /* 0x2075b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][37], 0x3bfea); /* 0x2075b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][38], 0x38055); /* 0x2075b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][39], 0x3dd58); /* 0x2075b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][40], 0x1f3d2); /* 0x2075ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][41], 0x2ce30); /* 0x2075ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][42], 0x1a84b); /* 0x2075ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][43], 0x337ee); /* 0x2075bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][44], 0x372ee); /* 0x2075bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][45], 0x2b314); /* 0x2075bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][46], 0x38ff4); /* 0x2075bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][47], 0x13bee); /* 0x2075bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][48], 0x34f6f); /* 0x2075bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][49], 0x32a82); /* 0x2075bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][50], 0x273dc); /* 0x2075bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][51], 0x112bf); /* 0x2075bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][0], 0x3d627); /* 0x2075c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][1], 0x27e76); /* 0x2075c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][2], 0x30226); /* 0x2075c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][3], 0x13add); /* 0x2075c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][4], 0x3aee); /* 0x2075c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][5], 0x2277d); /* 0x2075c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][6], 0x341ca); /* 0x2075c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][7], 0x11b1a); /* 0x2075c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][8], 0x247c6); /* 0x2075c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][9], 0x273ad); /* 0x2075c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][10], 0x391e1); /* 0x2075c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][11], 0x39f81); /* 0x2075c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][12], 0x3c0ef); /* 0x2075c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][13], 0x2beb); /* 0x2075c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][14], 0x3b1ec); /* 0x2075c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][15], 0x92f9); /* 0x2075c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][16], 0x9e0f); /* 0x2075c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][17], 0x3fb8e); /* 0x2075c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][18], 0x1dd78); /* 0x2075c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][19], 0x285b5); /* 0x2075c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][20], 0x14bd0); /* 0x2075c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][21], 0x25228); /* 0x2075c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][22], 0x2239a); /* 0x2075c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][23], 0x2aee7); /* 0x2075c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][24], 0x25f47); /* 0x2075c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][25], 0x14411); /* 0x2075c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][26], 0xf031); /* 0x2075c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][27], 0x39304); /* 0x2075c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][28], 0x398e4); /* 0x2075c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][29], 0x9492); /* 0x2075c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][30], 0x288f4); /* 0x2075c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][31], 0x3907e); /* 0x2075c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][32], 0x3e27f); /* 0x2075c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][33], 0x11c44); /* 0x2075c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][34], 0x3deb1); /* 0x2075c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][35], 0x16eb5); /* 0x2075c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][36], 0x3d22); /* 0x2075c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][37], 0x3f55d); /* 0x2075c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][38], 0x29a8c); /* 0x2075c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][39], 0x1d59); /* 0x2075c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][40], 0x2fe7b); /* 0x2075ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][41], 0x1617b); /* 0x2075ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][42], 0x2267e); /* 0x2075ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][43], 0x12d4a); /* 0x2075cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][44], 0x158fd); /* 0x2075cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][45], 0xeb8b); /* 0x2075cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][46], 0x34e05); /* 0x2075cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][47], 0x16ca1); /* 0x2075cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][48], 0x34eca); /* 0x2075cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][49], 0x272b1); /* 0x2075cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][50], 0x12f2b); /* 0x2075cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][51], 0x8e0e); /* 0x2075ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][0], 0x3def4); /* 0x2075d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][1], 0x6f7d); /* 0x2075d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][2], 0x18b53); /* 0x2075d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][3], 0x3325b); /* 0x2075d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][4], 0xa7f0); /* 0x2075d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][5], 0x23641); /* 0x2075d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][6], 0x16a59); /* 0x2075d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][7], 0x1f6a8); /* 0x2075d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][8], 0x1c15e); /* 0x2075d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][9], 0x2c630); /* 0x2075d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][10], 0x26eb5); /* 0x2075d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][11], 0x859a); /* 0x2075d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][12], 0xdd58); /* 0x2075d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][13], 0x3d91b); /* 0x2075d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][14], 0x6cb5); /* 0x2075d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][15], 0x29f01); /* 0x2075d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][16], 0x2d5c8); /* 0x2075d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][17], 0x3f7f7); /* 0x2075d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][18], 0x3e517); /* 0x2075d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][19], 0x5389); /* 0x2075d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][20], 0x3f3c7); /* 0x2075d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][21], 0x2bf34); /* 0x2075d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][22], 0x716b); /* 0x2075d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][23], 0x5ab2); /* 0x2075d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][24], 0x33eeb); /* 0x2075d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][25], 0x3330b); /* 0x2075d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][26], 0x1fafd); /* 0x2075d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][27], 0x24c); /* 0x2075d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][28], 0xaf9d); /* 0x2075d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][29], 0x314f0); /* 0x2075d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][30], 0x6a89); /* 0x2075d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][31], 0x3ebf); /* 0x2075d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][32], 0x3d587); /* 0x2075d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][33], 0x17434); /* 0x2075d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][34], 0xd48e); /* 0x2075d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][35], 0xc64d); /* 0x2075d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][36], 0x1a7d); /* 0x2075d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][37], 0x33906); /* 0x2075d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][38], 0x177); /* 0x2075d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][39], 0x13d50); /* 0x2075d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][40], 0x4d53); /* 0x2075da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][41], 0x3ea66); /* 0x2075da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][42], 0x2c95); /* 0x2075da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][43], 0x31c21); /* 0x2075dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][44], 0x1a192); /* 0x2075db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][45], 0x1e33b); /* 0x2075db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][46], 0x692); /* 0x2075db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][47], 0x2fc1c); /* 0x2075dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][48], 0xa7d8); /* 0x2075dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][49], 0x4636); /* 0x2075dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][50], 0x3e26d); /* 0x2075dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][51], 0x1b87c); /* 0x2075dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][0], 0x1dd6e); /* 0x2075e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][1], 0x31c57); /* 0x2075e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][2], 0x17579); /* 0x2075e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][3], 0x31be7); /* 0x2075e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][4], 0x2038e); /* 0x2075e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][5], 0x293c1); /* 0x2075e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][6], 0x22850); /* 0x2075e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][7], 0x171ae); /* 0x2075e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][8], 0x3dbc); /* 0x2075e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][9], 0xda9e); /* 0x2075e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][10], 0x3fe03); /* 0x2075e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][11], 0x4056); /* 0x2075e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][12], 0x1921a); /* 0x2075e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][13], 0x264d8); /* 0x2075e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][14], 0x2fbf5); /* 0x2075e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][15], 0x3e09c); /* 0x2075e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][16], 0x3b4f5); /* 0x2075e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][17], 0x56c9); /* 0x2075e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][18], 0x35a72); /* 0x2075e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][19], 0x1dfa0); /* 0x2075e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][20], 0x20e54); /* 0x2075e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][21], 0xe9cd); /* 0x2075e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][22], 0x1c52e); /* 0x2075e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][23], 0x228bc); /* 0x2075e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][24], 0x618f); /* 0x2075e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][25], 0x251e); /* 0x2075e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][26], 0x2d7f9); /* 0x2075e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][27], 0x20da9); /* 0x2075e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][28], 0x37729); /* 0x2075e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][29], 0x3cf3e); /* 0x2075e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][30], 0x130d7); /* 0x2075e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][31], 0x194bf); /* 0x2075e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][32], 0x15481); /* 0x2075e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][33], 0xed58); /* 0x2075e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][34], 0x5878); /* 0x2075e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][35], 0x28f63); /* 0x2075e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][36], 0x9b4e); /* 0x2075e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][37], 0x251a6); /* 0x2075e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][38], 0x7e5a); /* 0x2075e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][39], 0x23444); /* 0x2075e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][40], 0x20682); /* 0x2075ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][41], 0x22466); /* 0x2075ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][42], 0x79e6); /* 0x2075ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][43], 0x11df1); /* 0x2075eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][44], 0x240eb); /* 0x2075eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][45], 0x1df5e); /* 0x2075eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][46], 0x38106); /* 0x2075eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][47], 0x212d8); /* 0x2075ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][48], 0x94de); /* 0x2075ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][49], 0x250d); /* 0x2075ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][50], 0x293d1); /* 0x2075ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][51], 0x1079a); /* 0x2075ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][0], 0x69d9); /* 0x2075f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][1], 0x34eea); /* 0x2075f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][2], 0x2fb2b); /* 0x2075f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][3], 0x355b9); /* 0x2075f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][4], 0xa9f0); /* 0x2075f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][5], 0x1750); /* 0x2075f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][6], 0x27796); /* 0x2075f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][7], 0x1e865); /* 0x2075f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][8], 0x3f09a); /* 0x2075f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][9], 0x3f1bf); /* 0x2075f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][10], 0x14bd9); /* 0x2075f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][11], 0xb379); /* 0x2075f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][12], 0xcbc3); /* 0x2075f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][13], 0x28cd3); /* 0x2075f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][14], 0xe102); /* 0x2075f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][15], 0x3cd64); /* 0x2075f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][16], 0x26611); /* 0x2075f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][17], 0x3cb66); /* 0x2075f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][18], 0x2089); /* 0x2075f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][19], 0x241d6); /* 0x2075f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][20], 0x928); /* 0x2075f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][21], 0x3116f); /* 0x2075f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][22], 0x1b683); /* 0x2075f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][23], 0x3d5b); /* 0x2075f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][24], 0x1f3e0); /* 0x2075f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][25], 0x9820); /* 0x2075f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][26], 0x21d98); /* 0x2075f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][27], 0x85ab); /* 0x2075f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][28], 0x2af5e); /* 0x2075f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][29], 0x3a6b5); /* 0x2075f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][30], 0x248be); /* 0x2075f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][31], 0x260ec); /* 0x2075f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][32], 0x2021e); /* 0x2075f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][33], 0x1f756); /* 0x2075f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][34], 0x1e778); /* 0x2075f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][35], 0x32aeb); /* 0x2075f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][36], 0x330d1); /* 0x2075f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][37], 0x35ede); /* 0x2075f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][38], 0x36575); /* 0x2075f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][39], 0x30e0a); /* 0x2075f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][40], 0x247fb); /* 0x2075fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][41], 0x11500); /* 0x2075fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][42], 0x171d8); /* 0x2075fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][43], 0x21a71); /* 0x2075fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][44], 0x26cb6); /* 0x2075fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][45], 0xe0eb); /* 0x2075fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][46], 0x6fb); /* 0x2075fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][47], 0x25d5b); /* 0x2075fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][48], 0x3d5c2); /* 0x2075fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][49], 0x39609); /* 0x2075fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][50], 0x1456a); /* 0x2075fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][51], 0x1fb88); /* 0x2075fcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][0], 0x6e94); /* 0x2076000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][1], 0x103a8); /* 0x2076004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][2], 0x312c9); /* 0x2076008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][3], 0x3662); /* 0x207600c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][4], 0xd3fb); /* 0x2076010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][5], 0x1a405); /* 0x2076014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][6], 0x34518); /* 0x2076018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][7], 0x3571e); /* 0x207601c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][8], 0x1f23d); /* 0x2076020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][9], 0x22ed9); /* 0x2076024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][10], 0xd498); /* 0x2076028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][11], 0x3ac52); /* 0x207602c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][12], 0x2a4cc); /* 0x2076030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][13], 0x33707); /* 0x2076034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][14], 0x16c); /* 0x2076038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][15], 0x68d); /* 0x207603c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][16], 0x3550f); /* 0x2076040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][17], 0x31839); /* 0x2076044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][18], 0x1d508); /* 0x2076048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][19], 0x9827); /* 0x207604c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][20], 0x18ce4); /* 0x2076050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][21], 0x26f26); /* 0x2076054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][22], 0x3a3c2); /* 0x2076058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][23], 0x2119b); /* 0x207605c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][24], 0x17a29); /* 0x2076060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][25], 0x346ba); /* 0x2076064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][26], 0x37aee); /* 0x2076068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][27], 0x35d37); /* 0x207606c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][28], 0x355ca); /* 0x2076070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][29], 0x110e6); /* 0x2076074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][30], 0x142da); /* 0x2076078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][31], 0x3dff9); /* 0x207607c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][32], 0x1cec6); /* 0x2076080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][33], 0x34191); /* 0x2076084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][34], 0x24cbc); /* 0x2076088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][35], 0x2007d); /* 0x207608c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][36], 0x2bd6f); /* 0x2076090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][37], 0x37b7c); /* 0x2076094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][38], 0xcbc4); /* 0x2076098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][39], 0x6367); /* 0x207609c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][40], 0x38e5d); /* 0x20760a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][41], 0x3e40c); /* 0x20760a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][42], 0xfae9); /* 0x20760a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][43], 0x1f21e); /* 0x20760ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][44], 0x5f89); /* 0x20760b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][45], 0x18a12); /* 0x20760b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][46], 0xdb4c); /* 0x20760b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][47], 0x28e91); /* 0x20760bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][48], 0x8cc3); /* 0x20760c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][49], 0x2b3d8); /* 0x20760c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][50], 0xe950); /* 0x20760c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][51], 0x34aaf); /* 0x20760cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][0], 0xd179); /* 0x2076100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][1], 0x39e0); /* 0x2076104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][2], 0x282a0); /* 0x2076108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][3], 0xf70f); /* 0x207610c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][4], 0x1e9d3); /* 0x2076110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][5], 0x1c6d); /* 0x2076114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][6], 0x10db0); /* 0x2076118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][7], 0x3f273); /* 0x207611c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][8], 0x20b0c); /* 0x2076120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][9], 0x2ece8); /* 0x2076124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][10], 0x19ef1); /* 0x2076128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][11], 0xa17b); /* 0x207612c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][12], 0x27a5b); /* 0x2076130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][13], 0x29aa9); /* 0x2076134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][14], 0xa073); /* 0x2076138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][15], 0x26e21); /* 0x207613c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][16], 0x16f7d); /* 0x2076140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][17], 0x20fd8); /* 0x2076144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][18], 0x17409); /* 0x2076148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][19], 0x1d390); /* 0x207614c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][20], 0x2a730); /* 0x2076150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][21], 0x9a08); /* 0x2076154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][22], 0x3fed6); /* 0x2076158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][23], 0x3423d); /* 0x207615c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][24], 0x11da8); /* 0x2076160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][25], 0xba66); /* 0x2076164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][26], 0x2ff6); /* 0x2076168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][27], 0x26261); /* 0x207616c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][28], 0x2499a); /* 0x2076170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][29], 0x2ba44); /* 0x2076174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][30], 0x2903); /* 0x2076178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][31], 0x3a59d); /* 0x207617c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][32], 0x1b19c); /* 0x2076180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][33], 0x11ddb); /* 0x2076184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][34], 0x85ad); /* 0x2076188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][35], 0x29f75); /* 0x207618c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][36], 0x16fdd); /* 0x2076190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][37], 0x2827d); /* 0x2076194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][38], 0x2f07b); /* 0x2076198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][39], 0x3f451); /* 0x207619c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][40], 0x466c); /* 0x20761a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][41], 0x30ddc); /* 0x20761a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][42], 0x32aeb); /* 0x20761a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][43], 0x29530); /* 0x20761ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][44], 0x39d1a); /* 0x20761b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][45], 0x90d2); /* 0x20761b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][46], 0x6a38); /* 0x20761b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][47], 0x2768); /* 0x20761bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][48], 0x11fb1); /* 0x20761c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][49], 0x18f04); /* 0x20761c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][50], 0x2cf21); /* 0x20761c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][51], 0x3e692); /* 0x20761cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][0], 0x3e004); /* 0x2076200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][1], 0x1595); /* 0x2076204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][2], 0x25112); /* 0x2076208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][3], 0x3c538); /* 0x207620c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][4], 0x6d14); /* 0x2076210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][5], 0x1c0d2); /* 0x2076214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][6], 0x1715); /* 0x2076218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][7], 0x14e75); /* 0x207621c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][8], 0x33f31); /* 0x2076220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][9], 0x14a77); /* 0x2076224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][10], 0x19566); /* 0x2076228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][11], 0x35298); /* 0x207622c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][12], 0x17ba6); /* 0x2076230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][13], 0x1da9b); /* 0x2076234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][14], 0x21e0); /* 0x2076238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][15], 0x2ce85); /* 0x207623c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][16], 0x35542); /* 0x2076240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][17], 0x30c0f); /* 0x2076244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][18], 0x31272); /* 0x2076248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][19], 0x12b62); /* 0x207624c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][20], 0x2de7); /* 0x2076250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][21], 0x2a313); /* 0x2076254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][22], 0x3cb78); /* 0x2076258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][23], 0x3a0cc); /* 0x207625c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][24], 0x61ed); /* 0x2076260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][25], 0x1176); /* 0x2076264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][26], 0x364ee); /* 0x2076268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][27], 0x3bb68); /* 0x207626c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][28], 0x2a842); /* 0x2076270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][29], 0x18804); /* 0x2076274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][30], 0x2ad77); /* 0x2076278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][31], 0xb287); /* 0x207627c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][32], 0x13fcf); /* 0x2076280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][33], 0x1c218); /* 0x2076284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][34], 0x1cfe3); /* 0x2076288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][35], 0xac0e); /* 0x207628c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][36], 0x17d06); /* 0x2076290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][37], 0x38894); /* 0x2076294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][38], 0x273b); /* 0x2076298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][39], 0x299fa); /* 0x207629c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][40], 0xc4bf); /* 0x20762a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][41], 0x7e8d); /* 0x20762a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][42], 0x11db9); /* 0x20762a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][43], 0x25d7b); /* 0x20762ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][44], 0xab18); /* 0x20762b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][45], 0x15852); /* 0x20762b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][46], 0x14df3); /* 0x20762b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][47], 0x17bc5); /* 0x20762bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][48], 0x1aa9f); /* 0x20762c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][49], 0x16adc); /* 0x20762c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][50], 0x10a7c); /* 0x20762c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][51], 0x10e97); /* 0x20762cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][0], 0x2707a); /* 0x2076300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][1], 0x3001c); /* 0x2076304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][2], 0x1a778); /* 0x2076308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][3], 0x300cf); /* 0x207630c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][4], 0xeb5d); /* 0x2076310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][5], 0x11c99); /* 0x2076314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][6], 0x2a04f); /* 0x2076318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][7], 0x2922d); /* 0x207631c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][8], 0x3f07b); /* 0x2076320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][9], 0x3d983); /* 0x2076324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][10], 0x1dcd); /* 0x2076328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][11], 0x15141); /* 0x207632c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][12], 0x4fe5); /* 0x2076330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][13], 0x3123d); /* 0x2076334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][14], 0x16f69); /* 0x2076338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][15], 0x1e9f0); /* 0x207633c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][16], 0x3b484); /* 0x2076340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][17], 0x2f8f1); /* 0x2076344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][18], 0x34997); /* 0x2076348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][19], 0xefff); /* 0x207634c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][20], 0x3c834); /* 0x2076350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][21], 0x1cbcd); /* 0x2076354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][22], 0x1c045); /* 0x2076358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][23], 0x2e975); /* 0x207635c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][24], 0x3582); /* 0x2076360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][25], 0x22e02); /* 0x2076364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][26], 0x4eee); /* 0x2076368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][27], 0x15343); /* 0x207636c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][28], 0x15f7); /* 0x2076370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][29], 0xea15); /* 0x2076374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][30], 0x3bd3); /* 0x2076378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][31], 0x2d177); /* 0x207637c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][32], 0x12ecd); /* 0x2076380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][33], 0x1d8b8); /* 0x2076384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][34], 0x11c8); /* 0x2076388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][35], 0x1bfb); /* 0x207638c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][36], 0x3b2c9); /* 0x2076390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][37], 0x272d0); /* 0x2076394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][38], 0x37372); /* 0x2076398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][39], 0x21bce); /* 0x207639c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][40], 0x281f4); /* 0x20763a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][41], 0x20eeb); /* 0x20763a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][42], 0x329f4); /* 0x20763a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][43], 0x299ca); /* 0x20763ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][44], 0xa15e); /* 0x20763b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][45], 0x10865); /* 0x20763b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][46], 0x30dc5); /* 0x20763b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][47], 0x26593); /* 0x20763bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][48], 0xf6a9); /* 0x20763c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][49], 0x4dc); /* 0x20763c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][50], 0x1ab10); /* 0x20763c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][51], 0x11e9c); /* 0x20763cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][0], 0x3c9e6); /* 0x2076400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][1], 0x1bf6e); /* 0x2076404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][2], 0x3b82f); /* 0x2076408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][3], 0x2ac5a); /* 0x207640c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][4], 0x23c52); /* 0x2076410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][5], 0x3b7ba); /* 0x2076414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][6], 0x3e88d); /* 0x2076418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][7], 0x3dd3e); /* 0x207641c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][8], 0x31fd2); /* 0x2076420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][9], 0xec5f); /* 0x2076424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][10], 0x2de19); /* 0x2076428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][11], 0x154d2); /* 0x207642c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][12], 0xf49e); /* 0x2076430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][13], 0x3fec5); /* 0x2076434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][14], 0x11cd8); /* 0x2076438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][15], 0x64d); /* 0x207643c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][16], 0x3a1d4); /* 0x2076440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][17], 0x2a09b); /* 0x2076444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][18], 0xc514); /* 0x2076448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][19], 0x28c6e); /* 0x207644c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][20], 0x1eeb6); /* 0x2076450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][21], 0x3ca99); /* 0x2076454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][22], 0x266f6); /* 0x2076458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][23], 0x2c5cc); /* 0x207645c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][24], 0x2faa3); /* 0x2076460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][25], 0x2ecd4); /* 0x2076464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][26], 0x1b5a1); /* 0x2076468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][27], 0x18783); /* 0x207646c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][28], 0xed4c); /* 0x2076470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][29], 0x35d17); /* 0x2076474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][30], 0x734b); /* 0x2076478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][31], 0x31a5f); /* 0x207647c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][32], 0x238c5); /* 0x2076480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][33], 0x13667); /* 0x2076484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][34], 0xda92); /* 0x2076488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][35], 0x2a69c); /* 0x207648c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][36], 0x2330e); /* 0x2076490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][37], 0x3f43b); /* 0x2076494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][38], 0x2b944); /* 0x2076498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][39], 0x11068); /* 0x207649c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][40], 0x6ba1); /* 0x20764a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][41], 0x38318); /* 0x20764a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][42], 0x2eb4); /* 0x20764a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][43], 0x127a5); /* 0x20764ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][44], 0x3367b); /* 0x20764b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][45], 0x202c2); /* 0x20764b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][46], 0x2ee8b); /* 0x20764b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][47], 0x3fdba); /* 0x20764bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][48], 0x246fa); /* 0x20764c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][49], 0x9405); /* 0x20764c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][50], 0x3e320); /* 0x20764c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][51], 0x18988); /* 0x20764cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][0], 0x3a12a); /* 0x2076500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][1], 0xafb1); /* 0x2076504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][2], 0x3262e); /* 0x2076508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][3], 0xd12e); /* 0x207650c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][4], 0x3f8bd); /* 0x2076510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][5], 0x152e5); /* 0x2076514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][6], 0x316a8); /* 0x2076518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][7], 0x3b11d); /* 0x207651c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][8], 0x9e98); /* 0x2076520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][9], 0x1ca22); /* 0x2076524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][10], 0x70c7); /* 0x2076528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][11], 0x3f7c0); /* 0x207652c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][12], 0x24fc1); /* 0x2076530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][13], 0xeebc); /* 0x2076534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][14], 0xb2df); /* 0x2076538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][15], 0x1eabe); /* 0x207653c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][16], 0x39997); /* 0x2076540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][17], 0xfd4c); /* 0x2076544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][18], 0xe957); /* 0x2076548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][19], 0x1fdca); /* 0x207654c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][20], 0x2697d); /* 0x2076550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][21], 0x1c5de); /* 0x2076554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][22], 0x20b3b); /* 0x2076558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][23], 0x1269); /* 0x207655c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][24], 0x1b9b9); /* 0x2076560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][25], 0xadc5); /* 0x2076564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][26], 0x16904); /* 0x2076568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][27], 0x10c6c); /* 0x207656c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][28], 0xc36f); /* 0x2076570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][29], 0x17a77); /* 0x2076574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][30], 0x1f04); /* 0x2076578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][31], 0x1af67); /* 0x207657c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][32], 0x1a739); /* 0x2076580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][33], 0x29640); /* 0x2076584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][34], 0x25a1c); /* 0x2076588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][35], 0xa4c5); /* 0x207658c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][36], 0x21525); /* 0x2076590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][37], 0x285a4); /* 0x2076594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][38], 0x6742); /* 0x2076598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][39], 0x3f150); /* 0x207659c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][40], 0x12925); /* 0x20765a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][41], 0x15bfa); /* 0x20765a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][42], 0x348b1); /* 0x20765a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][43], 0x3f802); /* 0x20765ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][44], 0x2f238); /* 0x20765b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][45], 0xa893); /* 0x20765b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][46], 0x62ae); /* 0x20765b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][47], 0x2263f); /* 0x20765bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][48], 0x255bf); /* 0x20765c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][49], 0x226d); /* 0x20765c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][50], 0x298d6); /* 0x20765c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][51], 0x2b9ff); /* 0x20765cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][0], 0xce3b); /* 0x2076600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][1], 0x32a8d); /* 0x2076604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][2], 0x2c795); /* 0x2076608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][3], 0x1f096); /* 0x207660c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][4], 0x10bf3); /* 0x2076610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][5], 0x3ef4f); /* 0x2076614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][6], 0x3fc1); /* 0x2076618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][7], 0x3b87d); /* 0x207661c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][8], 0x2f40c); /* 0x2076620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][9], 0x30fbb); /* 0x2076624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][10], 0x1b363); /* 0x2076628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][11], 0x21e82); /* 0x207662c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][12], 0x2d7bd); /* 0x2076630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][13], 0x407f); /* 0x2076634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][14], 0xa890); /* 0x2076638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][15], 0x31c45); /* 0x207663c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][16], 0xa3b0); /* 0x2076640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][17], 0x1af3d); /* 0x2076644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][18], 0x7e76); /* 0x2076648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][19], 0xef29); /* 0x207664c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][20], 0x953e); /* 0x2076650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][21], 0x26112); /* 0x2076654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][22], 0x3d6bd); /* 0x2076658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][23], 0x4e3c); /* 0x207665c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][24], 0x1154); /* 0x2076660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][25], 0x15fd4); /* 0x2076664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][26], 0x37142); /* 0x2076668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][27], 0x16bb5); /* 0x207666c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][28], 0x341d5); /* 0x2076670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][29], 0x1447a); /* 0x2076674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][30], 0x3b4aa); /* 0x2076678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][31], 0x17e24); /* 0x207667c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][32], 0x3c0c2); /* 0x2076680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][33], 0x3d91c); /* 0x2076684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][34], 0x6274); /* 0x2076688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][35], 0x3eea4); /* 0x207668c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][36], 0x109ad); /* 0x2076690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][37], 0xcfaf); /* 0x2076694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][38], 0xd1a8); /* 0x2076698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][39], 0xc7b1); /* 0x207669c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][40], 0x55b5); /* 0x20766a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][41], 0x16fb5); /* 0x20766a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][42], 0x20d4d); /* 0x20766a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][43], 0x1b3be); /* 0x20766ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][44], 0x211e0); /* 0x20766b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][45], 0x216b2); /* 0x20766b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][46], 0x13d57); /* 0x20766b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][47], 0x28aec); /* 0x20766bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][48], 0x3a20d); /* 0x20766c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][49], 0xd9e); /* 0x20766c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][50], 0x16e2f); /* 0x20766c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][51], 0xfa0f); /* 0x20766cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][0], 0x2a8b3); /* 0x2076700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][1], 0x3ca78); /* 0x2076704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][2], 0x24f74); /* 0x2076708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][3], 0x284ab); /* 0x207670c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][4], 0x1ff1e); /* 0x2076710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][5], 0x2667f); /* 0x2076714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][6], 0x585f); /* 0x2076718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][7], 0x2c5f); /* 0x207671c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][8], 0x3fd83); /* 0x2076720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][9], 0x17a2b); /* 0x2076724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][10], 0x28fb8); /* 0x2076728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][11], 0x3d3ae); /* 0x207672c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][12], 0x23cbe); /* 0x2076730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][13], 0x23d62); /* 0x2076734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][14], 0x38504); /* 0x2076738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][15], 0x1577b); /* 0x207673c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][16], 0xa4b2); /* 0x2076740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][18], 0x19c94); /* 0x2076748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][19], 0x34efa); /* 0x207674c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][20], 0x360a7); /* 0x2076750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][21], 0x911); /* 0x2076754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][22], 0x22e95); /* 0x2076758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][23], 0xa68e); /* 0x207675c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][24], 0x39b58); /* 0x2076760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][25], 0x1e973); /* 0x2076764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][26], 0x272fe); /* 0x2076768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][27], 0x12b5f); /* 0x207676c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][28], 0x14b85); /* 0x2076770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][29], 0x1c5a8); /* 0x2076774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][30], 0xf218); /* 0x2076778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][31], 0x17de8); /* 0x207677c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][32], 0x1d4de); /* 0x2076780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][33], 0x3190c); /* 0x2076784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][34], 0x221d6); /* 0x2076788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][35], 0x17b7); /* 0x207678c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][36], 0x3f4f); /* 0x2076790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][37], 0x26e7); /* 0x2076794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][38], 0x39d81); /* 0x2076798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][39], 0x13bcc); /* 0x207679c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][40], 0xd04e); /* 0x20767a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][41], 0x20d46); /* 0x20767a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][42], 0x8e25); /* 0x20767a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][43], 0x1938b); /* 0x20767ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][44], 0x2720c); /* 0x20767b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][45], 0x81f7); /* 0x20767b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][46], 0x28c88); /* 0x20767b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][47], 0x228a5); /* 0x20767bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][48], 0x2eebc); /* 0x20767c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][49], 0x3a316); /* 0x20767c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][50], 0x4e7); /* 0x20767c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][51], 0x3f9f5); /* 0x20767cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][0], 0x1c4db); /* 0x2076800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][1], 0x3b332); /* 0x2076804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][2], 0xb215); /* 0x2076808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][3], 0x10454); /* 0x207680c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][4], 0x164c2); /* 0x2076810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][5], 0x271c6); /* 0x2076814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][6], 0x1a683); /* 0x2076818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][7], 0x24dbe); /* 0x207681c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][8], 0x1e276); /* 0x2076820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][9], 0x25e7f); /* 0x2076824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][10], 0x2f2b8); /* 0x2076828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][11], 0x162b2); /* 0x207682c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][12], 0xc033); /* 0x2076830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][13], 0xee99); /* 0x2076834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][14], 0x3ac26); /* 0x2076838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][15], 0x25e3f); /* 0x207683c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][16], 0xacb9); /* 0x2076840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][17], 0x91f6); /* 0x2076844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][18], 0xd9e2); /* 0x2076848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][19], 0x2e8ce); /* 0x207684c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][20], 0x3fe9b); /* 0x2076850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][21], 0x3d48c); /* 0x2076854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][22], 0x36268); /* 0x2076858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][23], 0x2e329); /* 0x207685c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][24], 0x1df89); /* 0x2076860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][25], 0x3f54c); /* 0x2076864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][26], 0xc4fe); /* 0x2076868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][27], 0x10b07); /* 0x207686c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][28], 0x290c7); /* 0x2076870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][29], 0x11074); /* 0x2076874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][30], 0x1cf68); /* 0x2076878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][31], 0x212da); /* 0x207687c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][32], 0xfdd8); /* 0x2076880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][33], 0x17a88); /* 0x2076884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][34], 0x9303); /* 0x2076888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][35], 0x10c20); /* 0x207688c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][36], 0x35d36); /* 0x2076890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][37], 0x3c380); /* 0x2076894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][38], 0x1afe7); /* 0x2076898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][39], 0x391e1); /* 0x207689c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][40], 0x2e5ee); /* 0x20768a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][41], 0x23ab7); /* 0x20768a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][42], 0x375ab); /* 0x20768a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][43], 0x32ba8); /* 0x20768ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][44], 0x1356b); /* 0x20768b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][45], 0x295db); /* 0x20768b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][46], 0x184e7); /* 0x20768b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][47], 0x2ab0b); /* 0x20768bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][48], 0x18608); /* 0x20768c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][49], 0x25d1e); /* 0x20768c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][50], 0x20ced); /* 0x20768c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][51], 0x16a52); /* 0x20768cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][0], 0x147b4); /* 0x2076900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][1], 0x1b0e0); /* 0x2076904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][2], 0x3100a); /* 0x2076908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][3], 0x1c2e2); /* 0x207690c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][4], 0x13460); /* 0x2076910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][5], 0x362c5); /* 0x2076914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][6], 0x2356f); /* 0x2076918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][7], 0xf69); /* 0x207691c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][8], 0x3b00a); /* 0x2076920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][9], 0x2f17e); /* 0x2076924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][10], 0x26dd4); /* 0x2076928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][11], 0xf291); /* 0x207692c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][12], 0x29f2f); /* 0x2076930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][13], 0x850b); /* 0x2076934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][14], 0x3667d); /* 0x2076938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][15], 0x250e3); /* 0x207693c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][16], 0x6410); /* 0x2076940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][17], 0x1eba3); /* 0x2076944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][18], 0x31113); /* 0x2076948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][19], 0x656); /* 0x207694c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][20], 0x6643); /* 0x2076950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][21], 0x35848); /* 0x2076954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][22], 0xe536); /* 0x2076958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][23], 0x1ef2f); /* 0x207695c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][24], 0x7e97); /* 0x2076960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][25], 0xbc01); /* 0x2076964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][26], 0x1deb5); /* 0x2076968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][27], 0x12b63); /* 0x207696c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][28], 0x295e3); /* 0x2076970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][29], 0x38f05); /* 0x2076974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][30], 0x3e5f0); /* 0x2076978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][31], 0x1ccd8); /* 0x207697c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][32], 0x9ecc); /* 0x2076980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][33], 0x5818); /* 0x2076984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][34], 0x2d8e6); /* 0x2076988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][35], 0x1713c); /* 0x207698c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][36], 0x105a6); /* 0x2076990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][37], 0xa9dd); /* 0x2076994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][38], 0x31614); /* 0x2076998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][39], 0x23fc9); /* 0x207699c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][40], 0x2091f); /* 0x20769a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][41], 0x14434); /* 0x20769a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][42], 0x80a6); /* 0x20769a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][43], 0x1470b); /* 0x20769ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][44], 0x1133b); /* 0x20769b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][45], 0x3233e); /* 0x20769b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][46], 0xb105); /* 0x20769b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][47], 0x2dbe8); /* 0x20769bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][48], 0xb439); /* 0x20769c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][49], 0x231b3); /* 0x20769c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][50], 0x2d9fc); /* 0x20769c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][51], 0x7d57); /* 0x20769cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][0], 0x38b6); /* 0x2076a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][1], 0x215d4); /* 0x2076a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][2], 0x1a020); /* 0x2076a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][3], 0x38a1a); /* 0x2076a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][4], 0x162ea); /* 0x2076a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][5], 0x3a454); /* 0x2076a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][6], 0x5082); /* 0x2076a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][7], 0x30fb8); /* 0x2076a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][8], 0x276dc); /* 0x2076a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][9], 0x1e3f0); /* 0x2076a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][10], 0x27afe); /* 0x2076a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][11], 0x33fab); /* 0x2076a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][12], 0x6c6e); /* 0x2076a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][13], 0x2f0e2); /* 0x2076a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][14], 0x369e8); /* 0x2076a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][15], 0x267f8); /* 0x2076a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][16], 0x27a4c); /* 0x2076a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][17], 0x22c07); /* 0x2076a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][18], 0x362c9); /* 0x2076a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][19], 0xf7b6); /* 0x2076a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][20], 0x1c8ce); /* 0x2076a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][21], 0xd109); /* 0x2076a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][22], 0x2998a); /* 0x2076a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][23], 0x281f3); /* 0x2076a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][24], 0x1f7a8); /* 0x2076a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][25], 0x266f6); /* 0x2076a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][26], 0x2fbf7); /* 0x2076a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][27], 0x1d2cc); /* 0x2076a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][28], 0x2242b); /* 0x2076a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][29], 0x1a5a3); /* 0x2076a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][30], 0x35570); /* 0x2076a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][31], 0x41c2); /* 0x2076a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][32], 0x1cb44); /* 0x2076a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][33], 0xbbca); /* 0x2076a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][34], 0x2ac73); /* 0x2076a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][35], 0x310e1); /* 0x2076a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][36], 0x307c5); /* 0x2076a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][37], 0x115); /* 0x2076a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][38], 0xe9b1); /* 0x2076a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][39], 0x25337); /* 0x2076a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][40], 0x3bb9b); /* 0x2076aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][41], 0x123fd); /* 0x2076aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][42], 0x25714); /* 0x2076aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][43], 0x3533a); /* 0x2076aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][44], 0xd43); /* 0x2076ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][45], 0x6440); /* 0x2076ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][46], 0x12514); /* 0x2076ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][47], 0x2dc69); /* 0x2076abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][48], 0x68ad); /* 0x2076ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][49], 0x28b8); /* 0x2076ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][50], 0x32401); /* 0x2076ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][51], 0x8f6f); /* 0x2076acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][0], 0x2a238); /* 0x2076b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][1], 0x2ba85); /* 0x2076b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][2], 0x36863); /* 0x2076b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][3], 0x2e460); /* 0x2076b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][4], 0x23421); /* 0x2076b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][5], 0x3ad81); /* 0x2076b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][6], 0x14c08); /* 0x2076b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][7], 0x2018d); /* 0x2076b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][8], 0x2f72b); /* 0x2076b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][9], 0x64b9); /* 0x2076b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][10], 0x6b38); /* 0x2076b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][11], 0x1285a); /* 0x2076b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][12], 0x32a24); /* 0x2076b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][13], 0x23353); /* 0x2076b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][14], 0x29da0); /* 0x2076b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][15], 0x2fe63); /* 0x2076b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][16], 0x2b51d); /* 0x2076b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][17], 0x3f086); /* 0x2076b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][18], 0x1bfc1); /* 0x2076b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][19], 0x20db6); /* 0x2076b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][20], 0x4c74); /* 0x2076b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][21], 0x4e89); /* 0x2076b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][22], 0x34669); /* 0x2076b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][23], 0xb53d); /* 0x2076b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][24], 0x26869); /* 0x2076b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][25], 0xe81d); /* 0x2076b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][26], 0x3ddf0); /* 0x2076b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][27], 0x30d51); /* 0x2076b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][28], 0x33f); /* 0x2076b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][29], 0x7a3); /* 0x2076b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][30], 0xa763); /* 0x2076b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][31], 0x26bf6); /* 0x2076b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][32], 0x153a7); /* 0x2076b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][33], 0x32741); /* 0x2076b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][34], 0x179e8); /* 0x2076b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][35], 0x2fe67); /* 0x2076b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][36], 0x1e05c); /* 0x2076b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][37], 0x3d1b3); /* 0x2076b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][38], 0x23fb0); /* 0x2076b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][39], 0xee68); /* 0x2076b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][40], 0x1eea4); /* 0x2076ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][41], 0x23719); /* 0x2076ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][42], 0x3df1c); /* 0x2076ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][43], 0x8c37); /* 0x2076bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][44], 0x38819); /* 0x2076bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][45], 0x13e8b); /* 0x2076bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][46], 0x3ae15); /* 0x2076bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][47], 0x255d7); /* 0x2076bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][48], 0x1acb2); /* 0x2076bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][49], 0x3d72b); /* 0x2076bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][50], 0x9fc9); /* 0x2076bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][51], 0x1e722); /* 0x2076bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][0], 0x2bfaa); /* 0x2076c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][1], 0x7e6b); /* 0x2076c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][2], 0x2da7d); /* 0x2076c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][3], 0x3e7a3); /* 0x2076c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][4], 0x1932); /* 0x2076c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][5], 0x16f78); /* 0x2076c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][6], 0x24c20); /* 0x2076c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][7], 0xeb6e); /* 0x2076c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][8], 0xbac3); /* 0x2076c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][9], 0x16a5f); /* 0x2076c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][10], 0x36115); /* 0x2076c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][11], 0x36852); /* 0x2076c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][12], 0x3dc20); /* 0x2076c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][13], 0xe648); /* 0x2076c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][14], 0x815a); /* 0x2076c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][15], 0x29b50); /* 0x2076c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][16], 0x30359); /* 0x2076c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][17], 0x3a5f8); /* 0x2076c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][18], 0x328c7); /* 0x2076c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][19], 0x19598); /* 0x2076c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][20], 0x10ca4); /* 0x2076c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][21], 0x2c58); /* 0x2076c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][22], 0x36d43); /* 0x2076c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][23], 0x37a10); /* 0x2076c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][24], 0x2b919); /* 0x2076c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][25], 0x124cb); /* 0x2076c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][26], 0x32392); /* 0x2076c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][27], 0x1c225); /* 0x2076c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][28], 0xcd77); /* 0x2076c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][29], 0x3f88b); /* 0x2076c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][30], 0x250ea); /* 0x2076c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][31], 0x3a03c); /* 0x2076c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][32], 0x5f6b); /* 0x2076c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][33], 0x755b); /* 0x2076c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][34], 0x64f9); /* 0x2076c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][35], 0x38273); /* 0x2076c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][36], 0x5c8f); /* 0x2076c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][37], 0x30464); /* 0x2076c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][38], 0x2e2d4); /* 0x2076c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][39], 0x1d7cf); /* 0x2076c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][40], 0x1adba); /* 0x2076ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][41], 0x2ea99); /* 0x2076ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][42], 0x8a); /* 0x2076ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][43], 0x226f1); /* 0x2076cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][44], 0x3fa5d); /* 0x2076cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][45], 0xcee8); /* 0x2076cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][46], 0xc258); /* 0x2076cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][47], 0x265ee); /* 0x2076cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][48], 0x39d13); /* 0x2076cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][49], 0x19d5f); /* 0x2076cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][50], 0x33a70); /* 0x2076cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][51], 0x1d871); /* 0x2076ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][0], 0x25886); /* 0x2076d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][1], 0x18ef0); /* 0x2076d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][2], 0x1b9a4); /* 0x2076d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][3], 0x341ef); /* 0x2076d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][4], 0x2a0bb); /* 0x2076d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][5], 0xe6bc); /* 0x2076d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][6], 0x16382); /* 0x2076d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][7], 0x244e5); /* 0x2076d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][8], 0x38f52); /* 0x2076d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][9], 0x2c8e9); /* 0x2076d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][10], 0x23bf7); /* 0x2076d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][11], 0x10974); /* 0x2076d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][12], 0xdd54); /* 0x2076d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][13], 0x3ee24); /* 0x2076d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][14], 0x124ac); /* 0x2076d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][15], 0xb70f); /* 0x2076d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][16], 0x22163); /* 0x2076d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][17], 0x16287); /* 0x2076d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][18], 0x76bd); /* 0x2076d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][19], 0x4337); /* 0x2076d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][20], 0x2405); /* 0x2076d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][21], 0x3e83f); /* 0x2076d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][22], 0x22be3); /* 0x2076d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][23], 0x30169); /* 0x2076d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][24], 0xac58); /* 0x2076d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][25], 0x1a106); /* 0x2076d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][26], 0x1d3f1); /* 0x2076d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][27], 0x27825); /* 0x2076d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][28], 0x29dcf); /* 0x2076d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][29], 0x124c6); /* 0x2076d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][30], 0x37b23); /* 0x2076d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][31], 0x3474); /* 0x2076d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][32], 0x29430); /* 0x2076d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][33], 0x20440); /* 0x2076d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][34], 0x25572); /* 0x2076d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][35], 0x3fb5a); /* 0x2076d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][36], 0x3d0f); /* 0x2076d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][37], 0xba7c); /* 0x2076d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][38], 0x1ae4a); /* 0x2076d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][39], 0x375c2); /* 0x2076d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][40], 0x3cb97); /* 0x2076da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][41], 0x30124); /* 0x2076da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][42], 0x28e50); /* 0x2076da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][43], 0x2f63a); /* 0x2076dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][44], 0xfac0); /* 0x2076db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][45], 0x374f1); /* 0x2076db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][46], 0x227da); /* 0x2076db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][47], 0x31b2c); /* 0x2076dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][48], 0x14feb); /* 0x2076dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][49], 0x8752); /* 0x2076dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][50], 0x14694); /* 0x2076dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][51], 0x1f979); /* 0x2076dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][0], 0x2668e); /* 0x2076e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][1], 0x3f11c); /* 0x2076e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][2], 0x6867); /* 0x2076e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][3], 0x16faf); /* 0x2076e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][4], 0x31bb); /* 0x2076e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][5], 0x2462b); /* 0x2076e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][6], 0x13fea); /* 0x2076e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][7], 0x31742); /* 0x2076e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][8], 0x189e1); /* 0x2076e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][9], 0x2d2ef); /* 0x2076e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][10], 0x35e01); /* 0x2076e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][11], 0x37419); /* 0x2076e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][12], 0x244e4); /* 0x2076e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][13], 0x28b56); /* 0x2076e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][14], 0x1be8c); /* 0x2076e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][15], 0xeded); /* 0x2076e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][16], 0x23fad); /* 0x2076e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][17], 0x171b3); /* 0x2076e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][18], 0x2b6e); /* 0x2076e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][19], 0x1c3b7); /* 0x2076e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][20], 0x30cff); /* 0x2076e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][21], 0x2dba3); /* 0x2076e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][22], 0x35ea6); /* 0x2076e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][23], 0x1a143); /* 0x2076e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][24], 0x2578); /* 0x2076e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][25], 0x33951); /* 0x2076e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][26], 0x3c7c1); /* 0x2076e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][27], 0x3533d); /* 0x2076e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][28], 0x3aab1); /* 0x2076e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][29], 0x338a6); /* 0x2076e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][30], 0x4552); /* 0x2076e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][31], 0xe53a); /* 0x2076e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][32], 0x1abc4); /* 0x2076e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][33], 0x33fb8); /* 0x2076e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][34], 0x11d0e); /* 0x2076e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][35], 0x15679); /* 0x2076e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][36], 0x1ba13); /* 0x2076e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][37], 0x39cda); /* 0x2076e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][38], 0x24dcd); /* 0x2076e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][39], 0x1f95); /* 0x2076e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][40], 0x166cd); /* 0x2076ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][41], 0xd4e3); /* 0x2076ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][42], 0xd1e4); /* 0x2076ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][43], 0x1ca71); /* 0x2076eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][44], 0x9e4); /* 0x2076eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][45], 0x3bb39); /* 0x2076eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][46], 0x2738f); /* 0x2076eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][47], 0x224f5); /* 0x2076ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][48], 0x18946); /* 0x2076ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][49], 0x32645); /* 0x2076ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][50], 0x1f5b0); /* 0x2076ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][51], 0x39775); /* 0x2076ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][0], 0xcba3); /* 0x2076f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][1], 0x29916); /* 0x2076f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][2], 0x6e9a); /* 0x2076f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][3], 0xe0c6); /* 0x2076f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][4], 0x2c8cc); /* 0x2076f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][5], 0xace6); /* 0x2076f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][6], 0x25e88); /* 0x2076f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][7], 0x1b446); /* 0x2076f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][8], 0x33a4d); /* 0x2076f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][9], 0xc71b); /* 0x2076f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][10], 0x28561); /* 0x2076f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][11], 0x3825d); /* 0x2076f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][12], 0x6337); /* 0x2076f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][13], 0xe51a); /* 0x2076f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][14], 0xe5c5); /* 0x2076f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][15], 0x28228); /* 0x2076f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][16], 0x2827e); /* 0x2076f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][17], 0xb646); /* 0x2076f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][18], 0xedff); /* 0x2076f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][19], 0x32907); /* 0x2076f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][20], 0x34f00); /* 0x2076f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][21], 0x12a5b); /* 0x2076f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][22], 0x3aec7); /* 0x2076f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][23], 0x3c02a); /* 0x2076f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][24], 0x1576); /* 0x2076f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][25], 0xfec3); /* 0x2076f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][26], 0xdcfc); /* 0x2076f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][27], 0x2d724); /* 0x2076f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][28], 0x20c52); /* 0x2076f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][29], 0x1475b); /* 0x2076f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][30], 0x16699); /* 0x2076f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][31], 0x39618); /* 0x2076f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][32], 0x10ee3); /* 0x2076f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][33], 0x112d0); /* 0x2076f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][34], 0x13326); /* 0x2076f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][35], 0x396db); /* 0x2076f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][36], 0x2cfa3); /* 0x2076f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][37], 0x119a0); /* 0x2076f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][38], 0x23bdb); /* 0x2076f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][39], 0x34b19); /* 0x2076f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][40], 0x158d4); /* 0x2076fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][41], 0x36371); /* 0x2076fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][42], 0xb5a4); /* 0x2076fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][43], 0x24d93); /* 0x2076fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][44], 0x2d15d); /* 0x2076fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][45], 0x18cb5); /* 0x2076fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][46], 0x27260); /* 0x2076fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][47], 0x2eff0); /* 0x2076fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][48], 0x1a39d); /* 0x2076fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][49], 0x3208a); /* 0x2076fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][50], 0xfe64); /* 0x2076fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][51], 0x3688d); /* 0x2076fcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][0], 0x760e); /* 0x2077000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][1], 0x730e); /* 0x2077004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][2], 0x363a6); /* 0x2077008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][3], 0x2f648); /* 0x207700c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][4], 0x30942); /* 0x2077010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][5], 0x33ba6); /* 0x2077014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][6], 0x1e854); /* 0x2077018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][7], 0x2cd80); /* 0x207701c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][8], 0x1287f); /* 0x2077020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][9], 0x219b7); /* 0x2077024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][10], 0x1ea76); /* 0x2077028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][11], 0x10113); /* 0x207702c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][12], 0x34dfb); /* 0x2077030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][13], 0x606e); /* 0x2077034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][14], 0x2dc00); /* 0x2077038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][15], 0x1a775); /* 0x207703c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][16], 0x2e91d); /* 0x2077040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][17], 0x280de); /* 0x2077044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][18], 0x143bc); /* 0x2077048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][19], 0x57f); /* 0x207704c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][20], 0x5665); /* 0x2077050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][21], 0x1121c); /* 0x2077054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][22], 0x1c309); /* 0x2077058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][23], 0x11050); /* 0x207705c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][24], 0x30d1e); /* 0x2077060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][25], 0x205f3); /* 0x2077064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][26], 0x364fc); /* 0x2077068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][27], 0x1ab0c); /* 0x207706c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][28], 0x38822); /* 0x2077070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][29], 0x3effe); /* 0x2077074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][30], 0x10048); /* 0x2077078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][31], 0x2d625); /* 0x207707c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][32], 0xb039); /* 0x2077080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][33], 0xeba6); /* 0x2077084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][34], 0x113ec); /* 0x2077088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][35], 0x12474); /* 0x207708c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][36], 0x2aa40); /* 0x2077090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][37], 0xdcb6); /* 0x2077094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][38], 0x36c07); /* 0x2077098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][39], 0x2e1b8); /* 0x207709c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][40], 0x233f4); /* 0x20770a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][41], 0x156f6); /* 0x20770a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][42], 0x18ce8); /* 0x20770a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][43], 0x1157); /* 0x20770ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][44], 0xb82d); /* 0x20770b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][45], 0x21970); /* 0x20770b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][46], 0xdca1); /* 0x20770b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][47], 0x3b8e6); /* 0x20770bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][48], 0x29dc4); /* 0x20770c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][49], 0xb0fb); /* 0x20770c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][50], 0x2120f); /* 0x20770c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][51], 0xe3bb); /* 0x20770cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][0], 0xe94c); /* 0x2077100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][1], 0x35e94); /* 0x2077104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][2], 0x3292b); /* 0x2077108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][3], 0x3cbde); /* 0x207710c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][4], 0x5fb); /* 0x2077110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][5], 0xf1a7); /* 0x2077114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][6], 0x2be07); /* 0x2077118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][7], 0x1dd4d); /* 0x207711c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][8], 0x2cfc2); /* 0x2077120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][9], 0x34521); /* 0x2077124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][10], 0x23772); /* 0x2077128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][11], 0x2490); /* 0x207712c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][12], 0x22026); /* 0x2077130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][13], 0x3b5f6); /* 0x2077134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][14], 0xf0ee); /* 0x2077138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][15], 0x19aab); /* 0x207713c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][16], 0xb90f); /* 0x2077140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][17], 0x1494c); /* 0x2077144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][18], 0x2ce21); /* 0x2077148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][19], 0x2f856); /* 0x207714c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][20], 0x3aa5e); /* 0x2077150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][21], 0x31075); /* 0x2077154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][22], 0x27048); /* 0x2077158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][23], 0x368a8); /* 0x207715c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][24], 0x10be4); /* 0x2077160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][25], 0x3ea3f); /* 0x2077164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][26], 0x1e754); /* 0x2077168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][27], 0x248a5); /* 0x207716c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][28], 0x16524); /* 0x2077170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][29], 0x23f44); /* 0x2077174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][30], 0x30523); /* 0x2077178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][31], 0x2f97b); /* 0x207717c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][32], 0x3b06f); /* 0x2077180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][33], 0x2ffa5); /* 0x2077184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][34], 0x6cc7); /* 0x2077188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][35], 0x3f69e); /* 0x207718c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][36], 0x3a62b); /* 0x2077190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][37], 0x261d6); /* 0x2077194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][38], 0x3c218); /* 0x2077198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][39], 0x3178b); /* 0x207719c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][40], 0x5995); /* 0x20771a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][41], 0x1be96); /* 0x20771a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][42], 0x31f89); /* 0x20771a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][43], 0xfcfc); /* 0x20771ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][44], 0x34d51); /* 0x20771b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][45], 0x3fdd0); /* 0x20771b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][46], 0x25731); /* 0x20771b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][47], 0x39ec5); /* 0x20771bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][48], 0x325f7); /* 0x20771c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][49], 0xa697); /* 0x20771c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][50], 0x3a1ea); /* 0x20771c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][51], 0x2807b); /* 0x20771cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][0], 0x1695e); /* 0x2077200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][1], 0x3769e); /* 0x2077204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][2], 0x1b303); /* 0x2077208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][3], 0x35d77); /* 0x207720c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][4], 0x31460); /* 0x2077210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][5], 0x3ac1b); /* 0x2077214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][6], 0x39ca0); /* 0x2077218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][7], 0x1e024); /* 0x207721c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][8], 0x1ded7); /* 0x2077220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][9], 0x1e3e8); /* 0x2077224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][10], 0x273de); /* 0x2077228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][11], 0x3f4f5); /* 0x207722c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][12], 0x2cac4); /* 0x2077230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][13], 0x12b25); /* 0x2077234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][14], 0x30f7f); /* 0x2077238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][15], 0x10629); /* 0x207723c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][16], 0x10470); /* 0x2077240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][17], 0x26fcd); /* 0x2077244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][18], 0x209dc); /* 0x2077248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][19], 0x3a568); /* 0x207724c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][20], 0x15488); /* 0x2077250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][21], 0x2b50d); /* 0x2077254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][22], 0x321df); /* 0x2077258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][23], 0x2156f); /* 0x207725c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][24], 0x2b5af); /* 0x2077260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][25], 0x3aa5a); /* 0x2077264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][26], 0x8c58); /* 0x2077268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][27], 0x125f9); /* 0x207726c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][28], 0x33d4e); /* 0x2077270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][29], 0x172d); /* 0x2077274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][30], 0xb1); /* 0x2077278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][31], 0x39f89); /* 0x207727c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][32], 0x1ae1d); /* 0x2077280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][33], 0xb216); /* 0x2077284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][34], 0x1b69e); /* 0x2077288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][35], 0x3063f); /* 0x207728c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][36], 0x1cf3); /* 0x2077290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][37], 0x1a5d2); /* 0x2077294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][38], 0x10eb1); /* 0x2077298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][39], 0x37c74); /* 0x207729c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][40], 0xf079); /* 0x20772a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][41], 0x217e0); /* 0x20772a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][42], 0x33281); /* 0x20772a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][43], 0x9dbe); /* 0x20772ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][44], 0xd4f4); /* 0x20772b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][45], 0x2a2a6); /* 0x20772b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][46], 0x30d9e); /* 0x20772b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][47], 0x22776); /* 0x20772bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][48], 0x265c3); /* 0x20772c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][49], 0x3ca38); /* 0x20772c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][50], 0x7027); /* 0x20772c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][51], 0x1b5e1); /* 0x20772cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][0], 0x27c4b); /* 0x2077300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][1], 0x2b99d); /* 0x2077304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][2], 0x25535); /* 0x2077308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][3], 0x19a5f); /* 0x207730c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][4], 0x343d); /* 0x2077310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][5], 0x152a9); /* 0x2077314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][6], 0xf55a); /* 0x2077318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][7], 0x2e758); /* 0x207731c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][8], 0x3380d); /* 0x2077320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][9], 0x33a29); /* 0x2077324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][10], 0x24eb5); /* 0x2077328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][11], 0x3072d); /* 0x207732c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][12], 0x10611); /* 0x2077330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][13], 0x35d85); /* 0x2077334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][14], 0xa02a); /* 0x2077338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][15], 0x1556c); /* 0x207733c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][16], 0x5afd); /* 0x2077340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][17], 0x9224); /* 0x2077344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][18], 0x3073); /* 0x2077348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][19], 0x31c2d); /* 0x207734c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][20], 0x22b72); /* 0x2077350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][21], 0x2dffe); /* 0x2077354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][22], 0x283b1); /* 0x2077358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][23], 0x12d71); /* 0x207735c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][24], 0x2cb0c); /* 0x2077360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][25], 0x27fc2); /* 0x2077364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][26], 0x16b34); /* 0x2077368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][27], 0x147c4); /* 0x207736c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][28], 0xa6e0); /* 0x2077370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][29], 0x20c48); /* 0x2077374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][30], 0x3ed64); /* 0x2077378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][31], 0x38bc3); /* 0x207737c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][32], 0x1d07f); /* 0x2077380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][33], 0x36694); /* 0x2077384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][34], 0x2a884); /* 0x2077388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][35], 0x2ffa6); /* 0x207738c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][36], 0x35bca); /* 0x2077390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][37], 0x3fbff); /* 0x2077394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][38], 0x1ed65); /* 0x2077398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][39], 0x2c68); /* 0x207739c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][40], 0x99c2); /* 0x20773a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][41], 0x2c976); /* 0x20773a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][42], 0x14809); /* 0x20773a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][43], 0x3e892); /* 0x20773ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][44], 0x354c7); /* 0x20773b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][45], 0x33875); /* 0x20773b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][46], 0x1d8d9); /* 0x20773b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][47], 0x6a7a); /* 0x20773bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][48], 0x2092d); /* 0x20773c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][49], 0x17087); /* 0x20773c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][50], 0x5b45); /* 0x20773c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][51], 0x1643a); /* 0x20773cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][0], 0x20b14); /* 0x2077400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][1], 0x19333); /* 0x2077404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][2], 0x37695); /* 0x2077408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][3], 0x39d97); /* 0x207740c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][4], 0x3819); /* 0x2077410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][5], 0x39bd4); /* 0x2077414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][6], 0x2cb3b); /* 0x2077418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][7], 0x12208); /* 0x207741c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][8], 0x2c607); /* 0x2077420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][9], 0x644c); /* 0x2077424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][10], 0x15788); /* 0x2077428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][11], 0xf66b); /* 0x207742c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][12], 0x2ca88); /* 0x2077430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][13], 0x31696); /* 0x2077434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][14], 0x38c7); /* 0x2077438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][15], 0x33230); /* 0x207743c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][16], 0x1e3e2); /* 0x2077440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][17], 0xe6f4); /* 0x2077444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][18], 0x2f573); /* 0x2077448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][19], 0xa1c3); /* 0x207744c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][20], 0x3e895); /* 0x2077450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][21], 0x2dafb); /* 0x2077454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][22], 0xc886); /* 0x2077458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][23], 0x27584); /* 0x207745c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][24], 0x3e6df); /* 0x2077460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][25], 0x20138); /* 0x2077464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][26], 0x2cc73); /* 0x2077468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][27], 0x231f5); /* 0x207746c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][28], 0x382f1); /* 0x2077470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][29], 0x19b01); /* 0x2077474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][30], 0xfe45); /* 0x2077478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][31], 0x15052); /* 0x207747c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][32], 0x2be2c); /* 0x2077480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][33], 0xb1b3); /* 0x2077484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][34], 0x33260); /* 0x2077488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][35], 0x3d652); /* 0x207748c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][36], 0x23606); /* 0x2077490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][37], 0x108cb); /* 0x2077494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][38], 0x24966); /* 0x2077498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][39], 0x70cf); /* 0x207749c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][40], 0x3bb42); /* 0x20774a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][41], 0x2e88c); /* 0x20774a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][42], 0x12606); /* 0x20774a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][43], 0x277aa); /* 0x20774ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][44], 0x6bd); /* 0x20774b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][45], 0x1baeb); /* 0x20774b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][46], 0x18cc7); /* 0x20774b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][47], 0x1e93a); /* 0x20774bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][48], 0xd1cb); /* 0x20774c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][49], 0x87ed); /* 0x20774c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][50], 0x1cf96); /* 0x20774c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][51], 0x1f26e); /* 0x20774cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][0], 0x27391); /* 0x2077500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][1], 0x29f57); /* 0x2077504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][2], 0x3b555); /* 0x2077508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][3], 0x3a6b3); /* 0x207750c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][4], 0x21ca3); /* 0x2077510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][5], 0x3fdc5); /* 0x2077514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][6], 0x1a5e9); /* 0x2077518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][7], 0x31bbe); /* 0x207751c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][8], 0x3ccf1); /* 0x2077520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][9], 0x37bd7); /* 0x2077524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][10], 0x35dc0); /* 0x2077528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][11], 0x236b1); /* 0x207752c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][12], 0x2413b); /* 0x2077530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][13], 0x2075c); /* 0x2077534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][14], 0xddc5); /* 0x2077538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][15], 0x18761); /* 0x207753c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][16], 0x27032); /* 0x2077540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][17], 0xdbda); /* 0x2077544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][18], 0xbb90); /* 0x2077548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][19], 0x1e2ac); /* 0x207754c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][20], 0x291c5); /* 0x2077550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][21], 0x7d57); /* 0x2077554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][22], 0x24455); /* 0x2077558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][23], 0x26609); /* 0x207755c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][24], 0x3cd3c); /* 0x2077560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][25], 0xacb0); /* 0x2077564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][26], 0x1a561); /* 0x2077568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][27], 0x199a9); /* 0x207756c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][28], 0x2c216); /* 0x2077570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][29], 0x11e35); /* 0x2077574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][30], 0xb7a5); /* 0x2077578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][31], 0x3da7e); /* 0x207757c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][32], 0x7639); /* 0x2077580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][33], 0x4453); /* 0x2077584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][34], 0x33db4); /* 0x2077588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][35], 0x3258a); /* 0x207758c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][36], 0x16870); /* 0x2077590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][37], 0x249a9); /* 0x2077594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][38], 0x1423b); /* 0x2077598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][39], 0xea9a); /* 0x207759c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][40], 0x8a18); /* 0x20775a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][41], 0xe553); /* 0x20775a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][42], 0x34128); /* 0x20775a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][43], 0x1c4c4); /* 0x20775ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][44], 0x2a8c5); /* 0x20775b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][45], 0x242e4); /* 0x20775b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][46], 0x14a84); /* 0x20775b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][47], 0x1197a); /* 0x20775bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][48], 0x2b298); /* 0x20775c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][49], 0x3b60c); /* 0x20775c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][50], 0x343b0); /* 0x20775c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][51], 0x107d6); /* 0x20775cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][0], 0x22066); /* 0x2077600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][1], 0x7286); /* 0x2077604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][2], 0x310f9); /* 0x2077608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][3], 0x35aae); /* 0x207760c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][4], 0x2079); /* 0x2077610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][5], 0x5e09); /* 0x2077614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][6], 0xf956); /* 0x2077618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][7], 0x10ed7); /* 0x207761c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][8], 0xc29c); /* 0x2077620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][9], 0x287be); /* 0x2077624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][10], 0x3c0df); /* 0x2077628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][11], 0x31d0a); /* 0x207762c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][12], 0x1bcaa); /* 0x2077630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][13], 0x2e203); /* 0x2077634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][14], 0x9258); /* 0x2077638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][15], 0x16d40); /* 0x207763c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][16], 0x26c84); /* 0x2077640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][17], 0x1f37f); /* 0x2077644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][18], 0x175c); /* 0x2077648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][19], 0x34b5f); /* 0x207764c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][20], 0x8cd3); /* 0x2077650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][21], 0x26e21); /* 0x2077654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][22], 0x24619); /* 0x2077658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][23], 0xd04a); /* 0x207765c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][24], 0x38e58); /* 0x2077660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][25], 0x3c5d6); /* 0x2077664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][26], 0x147c2); /* 0x2077668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][27], 0x37e73); /* 0x207766c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][28], 0x1736f); /* 0x2077670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][29], 0x2a18b); /* 0x2077674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][30], 0x124b8); /* 0x2077678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][31], 0x39556); /* 0x207767c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][32], 0x52f); /* 0x2077680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][33], 0x390c5); /* 0x2077684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][34], 0x20761); /* 0x2077688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][35], 0x16e32); /* 0x207768c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][36], 0x23a3b); /* 0x2077690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][37], 0x3e977); /* 0x2077694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][38], 0x7525); /* 0x2077698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][39], 0x3f3cd); /* 0x207769c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][40], 0x26377); /* 0x20776a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][41], 0xe6d1); /* 0x20776a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][42], 0x3df74); /* 0x20776a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][43], 0x20c3f); /* 0x20776ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][44], 0x2e165); /* 0x20776b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][45], 0x2615e); /* 0x20776b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][46], 0x33293); /* 0x20776b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][47], 0x1c753); /* 0x20776bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][48], 0x2571); /* 0x20776c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][49], 0x12b4d); /* 0x20776c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][50], 0x20abc); /* 0x20776c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][51], 0x57b1); /* 0x20776cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][0], 0x5d4e); /* 0x2077700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][1], 0x3cf6f); /* 0x2077704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][2], 0x18178); /* 0x2077708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][3], 0x9a7c); /* 0x207770c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][4], 0x37523); /* 0x2077710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][5], 0x11f4c); /* 0x2077714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][6], 0x8b85); /* 0x2077718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][7], 0x13c01); /* 0x207771c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][8], 0x1035a); /* 0x2077720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][9], 0x29d49); /* 0x2077724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][10], 0x3057c); /* 0x2077728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][11], 0x3ca7); /* 0x207772c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][12], 0x366a2); /* 0x2077730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][13], 0x108f1); /* 0x2077734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][14], 0x35604); /* 0x2077738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][15], 0x25190); /* 0x207773c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][16], 0x3f6cf); /* 0x2077740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][17], 0x3694); /* 0x2077744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][18], 0x1eb9e); /* 0x2077748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][19], 0x33b48); /* 0x207774c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][20], 0x1b755); /* 0x2077750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][21], 0x3c85b); /* 0x2077754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][22], 0x13317); /* 0x2077758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][23], 0x1d526); /* 0x207775c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][24], 0x3a6); /* 0x2077760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][25], 0x1be01); /* 0x2077764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][26], 0xef4f); /* 0x2077768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][27], 0x3c21); /* 0x207776c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][28], 0x26686); /* 0x2077770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][29], 0x165ef); /* 0x2077774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][30], 0x159e0); /* 0x2077778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][31], 0x3065c); /* 0x207777c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][32], 0xd598); /* 0x2077780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][33], 0x3c77e); /* 0x2077784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][34], 0x1151f); /* 0x2077788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][35], 0x8fdc); /* 0x207778c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][36], 0x32b3f); /* 0x2077790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][37], 0x6f48); /* 0x2077794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][38], 0x23f11); /* 0x2077798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][39], 0x4c18); /* 0x207779c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][40], 0x1ea8d); /* 0x20777a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][41], 0x3cf6c); /* 0x20777a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][42], 0x39bf7); /* 0x20777a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][43], 0x3e2f6); /* 0x20777ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][44], 0x34673); /* 0x20777b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][45], 0x38dde); /* 0x20777b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][46], 0x37b7a); /* 0x20777b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][47], 0x1792b); /* 0x20777bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][48], 0x1bb61); /* 0x20777c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][49], 0xd230); /* 0x20777c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][50], 0xa57b); /* 0x20777c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][51], 0x255cc); /* 0x20777cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][0], 0x38e7b); /* 0x2077800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][1], 0x62b6); /* 0x2077804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][2], 0x3ca09); /* 0x2077808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][3], 0x259eb); /* 0x207780c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][4], 0x3ad66); /* 0x2077810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][5], 0x1ae4f); /* 0x2077814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][6], 0x22a22); /* 0x2077818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][7], 0x15097); /* 0x207781c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][8], 0x2a5ae); /* 0x2077820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][9], 0x3da5); /* 0x2077824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][10], 0x3014f); /* 0x2077828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][11], 0x11d9f); /* 0x207782c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][12], 0x2e9f2); /* 0x2077830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][13], 0x33ffb); /* 0x2077834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][14], 0x35fa3); /* 0x2077838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][15], 0x13e65); /* 0x207783c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][16], 0x69a0); /* 0x2077840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][17], 0x37ed8); /* 0x2077844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][18], 0x1ead9); /* 0x2077848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][19], 0x2bbfe); /* 0x207784c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][20], 0x1d7ed); /* 0x2077850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][21], 0xc153); /* 0x2077854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][22], 0x384de); /* 0x2077858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][23], 0x13a51); /* 0x207785c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][24], 0x1d38a); /* 0x2077860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][25], 0x69de); /* 0x2077864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][26], 0x3bd84); /* 0x2077868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][27], 0x1c5db); /* 0x207786c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][28], 0x13edf); /* 0x2077870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][29], 0x719e); /* 0x2077874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][30], 0x52a4); /* 0x2077878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][31], 0x32db5); /* 0x207787c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][32], 0x26a12); /* 0x2077880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][33], 0x197a5); /* 0x2077884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][34], 0x19ad5); /* 0x2077888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][35], 0x3b703); /* 0x207788c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][36], 0x31c5d); /* 0x2077890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][37], 0x2f9c4); /* 0x2077894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][38], 0x1a3f8); /* 0x2077898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][39], 0x1db39); /* 0x207789c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][40], 0x871a); /* 0x20778a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][41], 0x282d8); /* 0x20778a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][42], 0x38a5d); /* 0x20778a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][43], 0x256cf); /* 0x20778ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][44], 0x2a7ac); /* 0x20778b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][45], 0x17481); /* 0x20778b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][46], 0x4567); /* 0x20778b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][47], 0x328dd); /* 0x20778bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][48], 0xe104); /* 0x20778c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][49], 0x2f884); /* 0x20778c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][50], 0x10f85); /* 0x20778c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][51], 0x2458a); /* 0x20778cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][0], 0x239b6); /* 0x2077900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][1], 0x2aba7); /* 0x2077904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][2], 0x41b3); /* 0x2077908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][3], 0x3ad8c); /* 0x207790c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][4], 0x1e6d); /* 0x2077910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][5], 0xb6e5); /* 0x2077914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][6], 0x1e3c6); /* 0x2077918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][7], 0xccc7); /* 0x207791c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][8], 0x39f1a); /* 0x2077920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][9], 0x10002); /* 0x2077924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][10], 0x1e931); /* 0x2077928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][11], 0x219b); /* 0x207792c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][12], 0x25176); /* 0x2077930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][13], 0x183a8); /* 0x2077934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][14], 0x17374); /* 0x2077938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][15], 0x34f75); /* 0x207793c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][16], 0x307e); /* 0x2077940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][17], 0x18c21); /* 0x2077944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][18], 0x1c836); /* 0x2077948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][19], 0x21743); /* 0x207794c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][20], 0x7f72); /* 0x2077950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][21], 0x26823); /* 0x2077954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][22], 0x31612); /* 0x2077958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][23], 0x1c91d); /* 0x207795c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][24], 0xf724); /* 0x2077960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][25], 0x2344f); /* 0x2077964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][26], 0x14fe5); /* 0x2077968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][27], 0x32eb7); /* 0x207796c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][28], 0x37a1c); /* 0x2077970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][29], 0x24c86); /* 0x2077974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][30], 0xc1a2); /* 0x2077978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][31], 0x34b9); /* 0x207797c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][32], 0x3ad8); /* 0x2077980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][33], 0x3fd1f); /* 0x2077984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][34], 0x2ffa3); /* 0x2077988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][35], 0xc230); /* 0x207798c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][36], 0x3abdb); /* 0x2077990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][37], 0x3eb54); /* 0x2077994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][38], 0xa552); /* 0x2077998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][39], 0x17f34); /* 0x207799c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][40], 0x10756); /* 0x20779a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][41], 0x266cb); /* 0x20779a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][42], 0x1c6e3); /* 0x20779a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][43], 0x27ca2); /* 0x20779ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][44], 0x19705); /* 0x20779b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][45], 0x1da5a); /* 0x20779b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][46], 0x2dd9); /* 0x20779b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][47], 0x39619); /* 0x20779bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][48], 0x3d879); /* 0x20779c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][49], 0x1ec4c); /* 0x20779c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][50], 0x3f25d); /* 0x20779c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][51], 0x10c14); /* 0x20779cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][0], 0x12a1a); /* 0x2077a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][1], 0x3ccc5); /* 0x2077a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][2], 0x3bc86); /* 0x2077a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][3], 0x1cde); /* 0x2077a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][4], 0x2064a); /* 0x2077a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][5], 0x36b6c); /* 0x2077a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][6], 0x26e22); /* 0x2077a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][7], 0x15a8); /* 0x2077a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][8], 0x36bda); /* 0x2077a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][9], 0xab1f); /* 0x2077a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][10], 0x312bc); /* 0x2077a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][11], 0x2658a); /* 0x2077a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][12], 0x24b70); /* 0x2077a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][13], 0x19ad4); /* 0x2077a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][14], 0x246a8); /* 0x2077a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][15], 0x33d43); /* 0x2077a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][16], 0x2ff6); /* 0x2077a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][17], 0x2a97b); /* 0x2077a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][18], 0x2fbe7); /* 0x2077a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][19], 0xf47d); /* 0x2077a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][20], 0x223f4); /* 0x2077a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][21], 0x31fb3); /* 0x2077a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][22], 0x2d093); /* 0x2077a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][23], 0x256b8); /* 0x2077a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][24], 0x3d8d1); /* 0x2077a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][25], 0x3a45c); /* 0x2077a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][26], 0x5206); /* 0x2077a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][27], 0x17198); /* 0x2077a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][28], 0x31a26); /* 0x2077a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][29], 0x18998); /* 0x2077a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][30], 0x1be7); /* 0x2077a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][31], 0x3c5dd); /* 0x2077a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][32], 0x29719); /* 0x2077a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][33], 0x36a5e); /* 0x2077a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][34], 0x100bc); /* 0x2077a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][35], 0x24b2); /* 0x2077a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][36], 0x3d850); /* 0x2077a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][37], 0x28450); /* 0x2077a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][38], 0xc0f6); /* 0x2077a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][39], 0x3b147); /* 0x2077a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][40], 0x1eb29); /* 0x2077aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][41], 0x24237); /* 0x2077aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][42], 0x933a); /* 0x2077aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][43], 0x1999e); /* 0x2077aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][44], 0x36d5e); /* 0x2077ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][45], 0x2f06c); /* 0x2077ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][46], 0x2435a); /* 0x2077ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][47], 0x1456c); /* 0x2077abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][48], 0xa4b7); /* 0x2077ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][49], 0x27a47); /* 0x2077ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][50], 0x37847); /* 0x2077ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][51], 0x3427d); /* 0x2077acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][0], 0x10f99); /* 0x2077b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][1], 0x27f34); /* 0x2077b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][2], 0x2f806); /* 0x2077b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][3], 0x20793); /* 0x2077b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][4], 0x29935); /* 0x2077b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][5], 0x28e1a); /* 0x2077b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][6], 0x35a40); /* 0x2077b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][7], 0x13a3b); /* 0x2077b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][8], 0x1ddd3); /* 0x2077b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][9], 0x3956); /* 0x2077b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][10], 0x3cc3b); /* 0x2077b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][11], 0x2b680); /* 0x2077b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][12], 0xd3d2); /* 0x2077b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][13], 0x3d393); /* 0x2077b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][14], 0x34970); /* 0x2077b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][15], 0x2c93c); /* 0x2077b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][16], 0x32199); /* 0x2077b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][17], 0x30c1a); /* 0x2077b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][18], 0x314c1); /* 0x2077b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][19], 0xdf08); /* 0x2077b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][20], 0x2b8a2); /* 0x2077b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][21], 0x34ba0); /* 0x2077b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][22], 0x1b333); /* 0x2077b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][23], 0x2744a); /* 0x2077b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][24], 0x2becb); /* 0x2077b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][25], 0x2bffe); /* 0x2077b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][26], 0x20b27); /* 0x2077b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][27], 0xfeec); /* 0x2077b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][28], 0x1a8f6); /* 0x2077b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][29], 0x34e92); /* 0x2077b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][30], 0x28fa1); /* 0x2077b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][31], 0x17269); /* 0x2077b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][32], 0xc14b); /* 0x2077b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][33], 0x1ddc5); /* 0x2077b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][34], 0x6f2d); /* 0x2077b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][35], 0x3f70a); /* 0x2077b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][36], 0x1f6de); /* 0x2077b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][37], 0x18bce); /* 0x2077b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][38], 0xa7fa); /* 0x2077b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][39], 0x3ee4d); /* 0x2077b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][40], 0x257bf); /* 0x2077ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][41], 0x19328); /* 0x2077ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][42], 0x27fbd); /* 0x2077ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][43], 0x376b2); /* 0x2077bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][44], 0x201b7); /* 0x2077bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][45], 0x3db0e); /* 0x2077bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][46], 0x3f7e4); /* 0x2077bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][47], 0x194d9); /* 0x2077bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][48], 0x21089); /* 0x2077bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][49], 0x5a1b); /* 0x2077bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][50], 0x14731); /* 0x2077bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][51], 0x2648); /* 0x2077bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][0], 0x28082); /* 0x2077c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][1], 0x5e09); /* 0x2077c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][2], 0x3beac); /* 0x2077c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][3], 0x21751); /* 0x2077c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][4], 0x528a); /* 0x2077c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][5], 0x12aff); /* 0x2077c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][6], 0x3b68e); /* 0x2077c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][7], 0x35215); /* 0x2077c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][8], 0xd595); /* 0x2077c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][9], 0x6c97); /* 0x2077c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][10], 0x37a9c); /* 0x2077c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][11], 0x1fce1); /* 0x2077c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][12], 0x184f8); /* 0x2077c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][13], 0x11e0b); /* 0x2077c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][14], 0x1a8ac); /* 0x2077c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][15], 0x1c466); /* 0x2077c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][16], 0x8e5); /* 0x2077c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][17], 0x20ee5); /* 0x2077c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][18], 0xeea5); /* 0x2077c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][19], 0x20608); /* 0x2077c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][20], 0x3dd72); /* 0x2077c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][21], 0x140c4); /* 0x2077c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][22], 0x23408); /* 0x2077c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][23], 0x1b7f7); /* 0x2077c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][24], 0x3b6c3); /* 0x2077c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][25], 0x260ce); /* 0x2077c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][26], 0x1094e); /* 0x2077c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][27], 0x3b2ec); /* 0x2077c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][28], 0x216a0); /* 0x2077c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][29], 0x16d37); /* 0x2077c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][30], 0x7e15); /* 0x2077c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][31], 0x1d68e); /* 0x2077c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][32], 0x1e28); /* 0x2077c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][33], 0x21711); /* 0x2077c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][34], 0x2b4d0); /* 0x2077c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][35], 0x2f5b0); /* 0x2077c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][36], 0x17e02); /* 0x2077c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][37], 0x16868); /* 0x2077c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][38], 0x14a23); /* 0x2077c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][39], 0x167c); /* 0x2077c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][40], 0x33949); /* 0x2077ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][41], 0x1afd7); /* 0x2077ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][42], 0x2085d); /* 0x2077ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][43], 0x3dc65); /* 0x2077cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][44], 0x2eea5); /* 0x2077cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][45], 0x2b126); /* 0x2077cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][46], 0x3a9c); /* 0x2077cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][47], 0x30d6); /* 0x2077cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][48], 0x25a40); /* 0x2077cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][49], 0x2e0c); /* 0x2077cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][50], 0x9a20); /* 0x2077cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][51], 0x38dbf); /* 0x2077ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][0], 0x3b573); /* 0x2077d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][1], 0x285f4); /* 0x2077d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][2], 0x22ab9); /* 0x2077d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][3], 0x2e636); /* 0x2077d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][4], 0x38e6f); /* 0x2077d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][5], 0x110e9); /* 0x2077d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][6], 0x16954); /* 0x2077d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][7], 0x170ce); /* 0x2077d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][8], 0xfbdd); /* 0x2077d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][9], 0x23603); /* 0x2077d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][10], 0x3e77c); /* 0x2077d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][11], 0x3ed19); /* 0x2077d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][12], 0x1f93b); /* 0x2077d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][13], 0x3d747); /* 0x2077d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][14], 0x23fcc); /* 0x2077d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][15], 0x8b57); /* 0x2077d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][16], 0x14229); /* 0x2077d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][17], 0x3d1a7); /* 0x2077d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][18], 0x11ae9); /* 0x2077d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][19], 0x37fa8); /* 0x2077d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][20], 0x229b); /* 0x2077d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][21], 0x363b1); /* 0x2077d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][22], 0x33050); /* 0x2077d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][23], 0x19f10); /* 0x2077d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][24], 0x2c061); /* 0x2077d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][25], 0x33708); /* 0x2077d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][26], 0x1264a); /* 0x2077d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][27], 0x1af35); /* 0x2077d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][28], 0x35a04); /* 0x2077d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][29], 0x6a8c); /* 0x2077d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][30], 0x28281); /* 0x2077d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][31], 0x1b098); /* 0x2077d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][32], 0x2639b); /* 0x2077d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][33], 0xb3f3); /* 0x2077d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][34], 0x238fd); /* 0x2077d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][35], 0x3c9aa); /* 0x2077d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][36], 0x3ec2e); /* 0x2077d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][37], 0x1c32e); /* 0x2077d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][38], 0x3aad5); /* 0x2077d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][39], 0x3ab3a); /* 0x2077d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][40], 0x23801); /* 0x2077da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][41], 0x3a7c2); /* 0x2077da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][42], 0x1a993); /* 0x2077da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][43], 0x1f7a0); /* 0x2077dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][44], 0x2abcc); /* 0x2077db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][45], 0x2e456); /* 0x2077db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][46], 0x2c477); /* 0x2077db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][47], 0x1169); /* 0x2077dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][48], 0x1ff57); /* 0x2077dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][49], 0xdbde); /* 0x2077dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][50], 0x3bb3b); /* 0x2077dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][51], 0x20498); /* 0x2077dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][0], 0x10bf7); /* 0x2077e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][1], 0x7426); /* 0x2077e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][2], 0x2c849); /* 0x2077e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][3], 0x2563b); /* 0x2077e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][4], 0x13d7d); /* 0x2077e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][5], 0x1878f); /* 0x2077e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][6], 0x32ad9); /* 0x2077e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][7], 0x2e6ba); /* 0x2077e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][8], 0x3aeca); /* 0x2077e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][9], 0x3b81d); /* 0x2077e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][10], 0xb93); /* 0x2077e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][11], 0x1aaec); /* 0x2077e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][12], 0x25275); /* 0x2077e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][13], 0x12c04); /* 0x2077e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][14], 0xb2d3); /* 0x2077e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][15], 0x3bb40); /* 0x2077e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][16], 0x13cb0); /* 0x2077e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][17], 0x37c4); /* 0x2077e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][18], 0x1c0e); /* 0x2077e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][19], 0x174fe); /* 0x2077e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][20], 0x261cb); /* 0x2077e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][21], 0x2c8b0); /* 0x2077e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][22], 0x3f538); /* 0x2077e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][23], 0x1ecea); /* 0x2077e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][24], 0x5bc4); /* 0x2077e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][25], 0x27a7a); /* 0x2077e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][26], 0x3f520); /* 0x2077e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][27], 0xfb40); /* 0x2077e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][28], 0x33d73); /* 0x2077e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][29], 0x8ffa); /* 0x2077e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][30], 0xbf3e); /* 0x2077e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][31], 0x29002); /* 0x2077e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][32], 0x111fd); /* 0x2077e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][33], 0x8569); /* 0x2077e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][34], 0x3b0c5); /* 0x2077e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][35], 0xfe39); /* 0x2077e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][36], 0xcef9); /* 0x2077e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][37], 0x3d82b); /* 0x2077e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][38], 0x386d7); /* 0x2077e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][39], 0x2f8a7); /* 0x2077e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][40], 0x234d5); /* 0x2077ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][41], 0x18fbc); /* 0x2077ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][42], 0xd3b5); /* 0x2077ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][43], 0x1c452); /* 0x2077eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][44], 0xa960); /* 0x2077eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][45], 0x382ce); /* 0x2077eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][46], 0x2242e); /* 0x2077eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][47], 0x25755); /* 0x2077ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][48], 0x72ff); /* 0x2077ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][49], 0x138ef); /* 0x2077ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][50], 0x3ae34); /* 0x2077ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][51], 0x2c6e1); /* 0x2077ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][0], 0x1ecd8); /* 0x2077f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][1], 0xdfa1); /* 0x2077f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][2], 0x11f09); /* 0x2077f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][3], 0x2bf86); /* 0x2077f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][4], 0x146b2); /* 0x2077f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][5], 0x14e56); /* 0x2077f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][6], 0x17309); /* 0x2077f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][7], 0x12118); /* 0x2077f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][8], 0x17394); /* 0x2077f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][9], 0x2a99a); /* 0x2077f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][10], 0x1a1e0); /* 0x2077f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][11], 0x15465); /* 0x2077f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][12], 0x34da3); /* 0x2077f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][13], 0x398c9); /* 0x2077f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][14], 0x1fb74); /* 0x2077f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][15], 0x184d2); /* 0x2077f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][16], 0x1dc24); /* 0x2077f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][17], 0x4efb); /* 0x2077f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][18], 0x187c2); /* 0x2077f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][19], 0x2da03); /* 0x2077f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][20], 0x1d8e9); /* 0x2077f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][21], 0x8050); /* 0x2077f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][22], 0x3fced); /* 0x2077f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][23], 0x3bec8); /* 0x2077f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][24], 0x2b634); /* 0x2077f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][25], 0x284b3); /* 0x2077f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][26], 0x2f7b4); /* 0x2077f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][27], 0x29391); /* 0x2077f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][28], 0x34aac); /* 0x2077f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][29], 0x37ad7); /* 0x2077f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][30], 0xa60c); /* 0x2077f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][31], 0x24090); /* 0x2077f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][32], 0x28e80); /* 0x2077f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][33], 0x1dff1); /* 0x2077f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][34], 0x30533); /* 0x2077f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][35], 0x3dc44); /* 0x2077f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][36], 0x8e72); /* 0x2077f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][37], 0x3617a); /* 0x2077f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][38], 0x722); /* 0x2077f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][39], 0x1d69b); /* 0x2077f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][40], 0x281ee); /* 0x2077fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][41], 0x4e7e); /* 0x2077fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][42], 0x3605b); /* 0x2077fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][43], 0xf61a); /* 0x2077fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][44], 0x3ddb8); /* 0x2077fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][45], 0x1e513); /* 0x2077fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][46], 0x85f3); /* 0x2077fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][47], 0xa5bb); /* 0x2077fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][48], 0x29db0); /* 0x2077fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][49], 0x17cf); /* 0x2077fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][50], 0x7206); /* 0x2077fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][51], 0x1899a); /* 0x2077fcc */
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0], 0xf1); /* 0x2070060 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0], 0xff03); /* 0x2070060 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0][0], 0x3); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0][1], 0xff); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1], 0xc); /* 0x2070064 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1], 0xf0); /* 0x2070064 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1][0], 0xf0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1][1], 0x0); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2], 0xc3); /* 0x2070068 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2], 0xf00f); /* 0x2070068 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2][0], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2][1], 0xf0); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3], 0xff); /* 0x207006c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3], 0xffff); /* 0x207006c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3][0], 0xff); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3][1], 0xff); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4], 0x10); /* 0x2070070 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4], 0x300); /* 0x2070070 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4][0], 0x0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4][1], 0x3); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5], 0x7c); /* 0x2070074 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5], 0x3ff0); /* 0x2070074 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5][0], 0xf0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5][1], 0x3f); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6], 0x3); /* 0x2070078 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6], 0xf); /* 0x2070078 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6][0], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6][1], 0x0); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7], 0x3); /* 0x207007c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7], 0xf); /* 0x207007c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7][0], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7][1], 0x0); // regs_31841 fix
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[0][0], 0x224ac0e); /* 0x2070000 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[0][1], 0x54315); /* 0x2070004 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[1][0], 0x3549197); /* 0x2070008 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[1][1], 0x2cea6b9); /* 0x207000c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[2][0], 0x2063fe4); /* 0x2070010 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[2][1], 0x14346a6); /* 0x2070014 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[3][0], 0x2e04daf); /* 0x2070018 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[3][1], 0x3d1fecb); /* 0x207001c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[4][0], 0x317dd7); /* 0x2070020 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[4][1], 0x39e1d97); /* 0x2070024 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[5][0], 0x1a954); /* 0x2070028 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[5][1], 0x2d9ff0c); /* 0x207002c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[6][0], 0x1c9aacf); /* 0x2070030 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[6][1], 0x2e86708); /* 0x2070034 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[7][0], 0x37448c5); /* 0x2070038 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[7][1], 0x123b13); /* 0x207003c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[0], 0xda); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[1], 0x5b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[2], 0xff); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[3], 0x49); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[4], 0x32); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[5], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[6], 0xf4); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[7], 0xde); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[8], 0x3e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[9], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[10], 0x1d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[11], 0xfd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[12], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[13], 0x75); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[14], 0x98); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[15], 0x63); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[16], 0x70); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[17], 0x4); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[18], 0x87); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[19], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[20], 0x92); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[21], 0x99); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[22], 0xca); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[23], 0x48); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[24], 0xc2); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[25], 0x8f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[26], 0x9b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[27], 0x9c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[28], 0x35); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[29], 0x6a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[30], 0x93); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[31], 0x6); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[32], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[33], 0x1e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[34], 0xf1); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[35], 0xef); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[36], 0x7e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[37], 0xb8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[38], 0xb8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[39], 0xea); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[40], 0x6d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[41], 0x2a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[42], 0x2d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[43], 0x96); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[44], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[45], 0x72); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[46], 0xb8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[47], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[48], 0x6e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[49], 0x7a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[50], 0x1c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[51], 0x7a); // regs_31841 fix
    tu.OutWord(&mau_reg_map.dp.hashout_ctl, 0xbdff00); /* 0x2070040 */
    tu.IndirectWrite(0x020080000038, 0x0000000000000000, 0x0d00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM5_way0_wid0_dep0: a=0x20080000038 d0=0x0 d1=0xd00000000000000 */
    tu.IndirectWrite(0x020080000038, 0x0000000000000000, 0x0d00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM5_way0_wid0_dep0: a=0x20080000038 d0=0x0 d1=0xd00000000000000 */
    tu.IndirectWrite(0x020080000038, 0x075687a000000000, 0x0d00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM5_way0_wid0_dep0: a=0x20080000038 d0=0x75687a000000000 d1=0xd00000000000000 */
    tu.IndirectWrite(0x020080000002, 0x0000000000000000, 0x0d00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM5_way0_wid0_dep0: a=0x20080000002 d0=0x0 d1=0xd00000000000000 */
    tu.IndirectWrite(0x020080000002, 0x0000000000000000, 0x0d00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM5_way0_wid0_dep0: a=0x20080000002 d0=0x0 d1=0xd00000000000000 */
    tu.IndirectWrite(0x020080000002, 0x08db231600000000, 0x0d00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM5_way0_wid0_dep0: a=0x20080000002 d0=0x8db231600000000 d1=0xd00000000000000 */
    tu.IndirectWrite(0x020080000197, 0x0000000000000000, 0xd0000000000000dd); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM5_way0_wid0_dep0: a=0x20080000197 d0=0x0 d1=0xd0000000000000dd */
    tu.IndirectWrite(0x020080000197, 0x0000000000000000, 0xd0000000000000dd); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM5_way0_wid0_dep0: a=0x20080000197 d0=0x0 d1=0xd0000000000000dd */
    tu.IndirectWrite(0x020080000197, 0x0000000001eff185, 0xd0000000000000dd); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM5_way0_wid0_dep0: a=0x20080000197 d0=0x1eff185 d1=0xd0000000000000dd */
    tu.IndirectWrite(0x020080000378, 0x0000000000000000, 0x0d00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM5_way0_wid0_dep0: a=0x20080000378 d0=0x0 d1=0xd00000000000000 */
    tu.IndirectWrite(0x020080000378, 0x0000000000000000, 0x0d00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM5_way0_wid0_dep0: a=0x20080000378 d0=0x0 d1=0xd00000000000000 */
    tu.IndirectWrite(0x020080000378, 0x01ad79c000000000, 0x0d00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM5_way0_wid0_dep0: a=0x20080000378 d0=0x1ad79c000000000 d1=0xd00000000000000 */
    tu.IndirectWrite(0x020080000002, 0x08db231600000000, 0xdd0000000000006c); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM5_way0_wid0_dep0: a=0x20080000002 d0=0x8db231600000000 d1=0xdd0000000000006c */
    tu.IndirectWrite(0x020080000002, 0x08db231600000000, 0xdd0000000000006c); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM5_way0_wid0_dep0: a=0x20080000002 d0=0x8db231600000000 d1=0xdd0000000000006c */
    tu.IndirectWrite(0x020080000002, 0x08db2316040ec09f, 0xdd0000000000006c); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM5_way0_wid0_dep0: a=0x20080000002 d0=0x8db2316040ec09f d1=0xdd0000000000006c */



  act_hv_translator.do_writes(&tu);
    Phv *phv_in2 = tu.phv_alloc();
    phv_in2->set_ingress();
    phv_in2->set_version(1,true);
    phv_in2->set(  0, 0x756a045e); 	/* [0, 0] v=1  bytes:  3-  0  #5e0# RefModel iPhv 2o */
    phv_in2->set(  1, 0x978a9d41); 	/* [0, 1] v=1  bytes:  7-  4  #5e0# RefModel iPhv 2o */
    phv_in2->set(  2, 0xcbc20595); 	/* [0, 2] v=1  bytes: 11-  8  #5e0# RefModel iPhv 2o */
    phv_in2->set(  3, 0x13cfc55c); 	/* [0, 3] v=1  bytes: 15- 12  #5e0# RefModel iPhv 2o */
    phv_in2->set(  4, 0x12296ffd); 	/* [0, 4] v=1  bytes: 19- 16  #5e0# RefModel iPhv 2o */
    phv_in2->set(  5, 0xe0db127d); 	/* [0, 5] v=1  bytes: 23- 20  #5e0# RefModel iPhv 2o */
    phv_in2->set(  6, 0xf8465c65); 	/* [0, 6] v=1  bytes: 27- 24  #5e0# RefModel iPhv 2o */
    phv_in2->set(  7, 0xa79baddc); 	/* [0, 7] v=1  bytes: 31- 28  #5e0# RefModel iPhv 2o */
    phv_in2->set(  8, 0xcbbf460f); 	/* [0, 8] v=1  bytes: 35- 32  #5e0# RefModel iPhv 2o */
    phv_in2->set(  9, 0x2afc3a71); 	/* [0, 9] v=1  bytes: 39- 36  #5e0# RefModel iPhv 2o */
    phv_in2->set( 10, 0xcae334e3); 	/* [0,10] v=1  bytes: 43- 40  #5e0# RefModel iPhv 2o */
    phv_in2->set( 11, 0xc921b2a7); 	/* [0,11] v=1  bytes: 47- 44  #5e0# RefModel iPhv 2o */
    phv_in2->set( 12, 0xb3f7d4c3); 	/* [0,12] v=1  bytes: 51- 48  #5e0# RefModel iPhv 2o */
    phv_in2->set( 13, 0xeb2671d3); 	/* [0,13] v=1  bytes: 55- 52  #5e0# RefModel iPhv 2o */
    phv_in2->set( 14, 0x9f15b21a); 	/* [0,14] v=1  bytes: 59- 56  #5e0# RefModel iPhv 2o */
    phv_in2->set( 15, 0xfbb0107c); 	/* [0,15] v=1  bytes: 63- 60  #5e0# RefModel iPhv 2o */
    phv_in2->set( 16, 0xc21f9aa6); 	/* [0,16] v=1  bytes: 67- 64  #5e0# RefModel iPhv 2o */
    phv_in2->set( 17, 0x4461fe9a); 	/* [0,17] v=1  bytes: 71- 68  #5e0# RefModel iPhv 2o */
    phv_in2->set( 18, 0x3b28f576); 	/* [0,18] v=1  bytes: 75- 72  #5e0# RefModel iPhv 2o */
    phv_in2->set( 19, 0x3ad46a37); 	/* [0,19] v=1  bytes: 79- 76  #5e0# RefModel iPhv 2o */
    phv_in2->set( 20, 0x40f0c39a); 	/* [0,20] v=1  bytes: 83- 80  #5e0# RefModel iPhv 2o */
    phv_in2->set( 21, 0x088e6024); 	/* [0,21] v=1  bytes: 87- 84  #5e0# RefModel iPhv 2o */
    phv_in2->set( 22, 0xf3dee221); 	/* [0,22] v=1  bytes: 91- 88  #5e0# RefModel iPhv 2o */
    phv_in2->set( 23, 0x0ee079d2); 	/* [0,23] v=1  bytes: 95- 92  #5e0# RefModel iPhv 2o */
    phv_in2->set( 24, 0x93013971); 	/* [0,24] v=1  bytes: 99- 96  #5e0# RefModel iPhv 2o */
    phv_in2->set( 25, 0x37a2061e); 	/* [0,25] v=1  bytes:103-100  #5e0# RefModel iPhv 2o */
    phv_in2->set( 26, 0xed2d72d2); 	/* [0,26] v=1  bytes:107-104  #5e0# RefModel iPhv 2o */
    phv_in2->set( 27, 0x30c181db); 	/* [0,27] v=1  bytes:111-108  #5e0# RefModel iPhv 2o */
    phv_in2->set( 28, 0x181d9fa0); 	/* [0,28] v=1  bytes:115-112  #5e0# RefModel iPhv 2o */
    phv_in2->set( 29, 0xa5fb1ec1); 	/* [0,29] v=1  bytes:119-116  #5e0# RefModel iPhv 2o */
    phv_in2->set( 30, 0x2b9de2c9); 	/* [0,30] v=1  bytes:123-120  #5e0# RefModel iPhv 2o */
    phv_in2->set( 31, 0x202eab85); 	/* [0,31] v=1  bytes:127-124  #5e0# RefModel iPhv 2o */
    phv_in2->set( 32, 0x14143fbd); 	/* [1, 0] v=1  bytes:131-128  #5e0# RefModel iPhv 2o */
    phv_in2->set( 33, 0x5ed0933e); 	/* [1, 1] v=1  bytes:135-132  #5e0# RefModel iPhv 2o */
    phv_in2->set( 34, 0xf2bfdf27); 	/* [1, 2] v=1  bytes:139-136  #5e0# RefModel iPhv 2o */
    phv_in2->set( 35, 0xaff31d58); 	/* [1, 3] v=1  bytes:143-140  #5e0# RefModel iPhv 2o */
    phv_in2->set( 36, 0xd295fa53); 	/* [1, 4] v=1  bytes:147-144  #5e0# RefModel iPhv 2o */
    phv_in2->set( 37, 0xca374673); 	/* [1, 5] v=1  bytes:151-148  #5e0# RefModel iPhv 2o */
    phv_in2->set( 38, 0xe8f39402); 	/* [1, 6] v=1  bytes:155-152  #5e0# RefModel iPhv 2o */
    phv_in2->set( 39, 0x1f79e1bf); 	/* [1, 7] v=1  bytes:159-156  #5e0# RefModel iPhv 2o */
    phv_in2->set( 40, 0x3f5b7e2d); 	/* [1, 8] v=1  bytes:163-160  #5e0# RefModel iPhv 2o */
    phv_in2->set( 41, 0x950b1534); 	/* [1, 9] v=1  bytes:167-164  #5e0# RefModel iPhv 2o */
    phv_in2->set( 42, 0xb423d965); 	/* [1,10] v=1  bytes:171-168  #5e0# RefModel iPhv 2o */
    phv_in2->set( 43, 0x68d73bc5); 	/* [1,11] v=1  bytes:175-172  #5e0# RefModel iPhv 2o */
    phv_in2->set( 44, 0x369c794e); 	/* [1,12] v=1  bytes:179-176  #5e0# RefModel iPhv 2o */
    phv_in2->set( 45, 0xbaa3bc15); 	/* [1,13] v=1  bytes:183-180  #5e0# RefModel iPhv 2o */
    phv_in2->set( 46, 0x12cedde6); 	/* [1,14] v=1  bytes:187-184  #5e0# RefModel iPhv 2o */
    phv_in2->set( 47, 0xe8436b10); 	/* [1,15] v=1  bytes:191-188  #5e0# RefModel iPhv 2o */
    phv_in2->set( 48, 0xd7d0a1ba); 	/* [1,16] v=1  bytes:195-192  #5e0# RefModel iPhv 2o */
    phv_in2->set( 49, 0x60581634); 	/* [1,17] v=1  bytes:199-196  #5e0# RefModel iPhv 2o */
    phv_in2->set( 50, 0x0074db03); 	/* [1,18] v=1  bytes:203-200  #5e0# RefModel iPhv 2o */
    phv_in2->set( 51, 0x67625321); 	/* [1,19] v=1  bytes:207-204  #5e0# RefModel iPhv 2o */
    phv_in2->set( 52, 0x0ae1bfce); 	/* [1,20] v=1  bytes:211-208  #5e0# RefModel iPhv 2o */
    phv_in2->set( 53, 0xfcbec02e); 	/* [1,21] v=1  bytes:215-212  #5e0# RefModel iPhv 2o */
    phv_in2->set( 54, 0x9d82314e); 	/* [1,22] v=1  bytes:219-216  #5e0# RefModel iPhv 2o */
    phv_in2->set( 55, 0xd1ff4a19); 	/* [1,23] v=1  bytes:223-220  #5e0# RefModel iPhv 2o */
    phv_in2->set( 56, 0xcfd0e33b); 	/* [1,24] v=1  bytes:227-224  #5e0# RefModel iPhv 2o */
    phv_in2->set( 57, 0xda596e9b); 	/* [1,25] v=1  bytes:231-228  #5e0# RefModel iPhv 2o */
    phv_in2->set( 58, 0x6fdf6b70); 	/* [1,26] v=1  bytes:235-232  #5e0# RefModel iPhv 2o */
    phv_in2->set( 59, 0x7cb62484); 	/* [1,27] v=1  bytes:239-236  #5e0# RefModel iPhv 2o */
    phv_in2->set( 60, 0x25ea40c6); 	/* [1,28] v=1  bytes:243-240  #5e0# RefModel iPhv 2o */
    phv_in2->set( 61, 0x8f9c86a1); 	/* [1,29] v=1  bytes:247-244  #5e0# RefModel iPhv 2o */
    phv_in2->set( 62, 0x67e8aefc); 	/* [1,30] v=1  bytes:251-248  #5e0# RefModel iPhv 2o */
    phv_in2->set( 63, 0x12da55d8); 	/* [1,31] v=1  bytes:255-252  #5e0# RefModel iPhv 2o */
    phv_in2->set( 64, 0xc2); 	/* [2, 0] v=1  bytes:256     #5e0# RefModel iPhv 2o */
    phv_in2->set( 65, 0x7f); 	/* [2, 1] v=1  bytes:257     #5e0# RefModel iPhv 2o */
    phv_in2->set( 66, 0xec); 	/* [2, 2] v=1  bytes:258     #5e0# RefModel iPhv 2o */
    phv_in2->set( 67, 0x5e); 	/* [2, 3] v=1  bytes:259     #5e0# RefModel iPhv 2o */
    phv_in2->set( 68, 0x56); 	/* [2, 4] v=1  bytes:260     #5e0# RefModel iPhv 2o */
    phv_in2->set( 69, 0x72); 	/* [2, 5] v=1  bytes:261     #5e0# RefModel iPhv 2o */
    phv_in2->set( 70, 0x63); 	/* [2, 6] v=1  bytes:262     #5e0# RefModel iPhv 2o */
    phv_in2->set( 71, 0x78); 	/* [2, 7] v=1  bytes:263     #5e0# RefModel iPhv 2o */
    phv_in2->set( 72, 0xf9); 	/* [2, 8] v=1  bytes:264     #5e0# RefModel iPhv 2o */
    phv_in2->set( 73, 0x6b); 	/* [2, 9] v=1  bytes:265     #5e0# RefModel iPhv 2o */
    phv_in2->set( 74, 0x7e); 	/* [2,10] v=1  bytes:266     #5e0# RefModel iPhv 2o */
    phv_in2->set( 75, 0xa9); 	/* [2,11] v=1  bytes:267     #5e0# RefModel iPhv 2o */
    phv_in2->set( 76, 0x64); 	/* [2,12] v=1  bytes:268     #5e0# RefModel iPhv 2o */
    phv_in2->set( 77, 0x85); 	/* [2,13] v=1  bytes:269     #5e0# RefModel iPhv 2o */
    phv_in2->set( 78, 0xea); 	/* [2,14] v=1  bytes:270     #5e0# RefModel iPhv 2o */
    phv_in2->set( 79, 0xb5); 	/* [2,15] v=1  bytes:271     #5e0# RefModel iPhv 2o */
    phv_in2->set( 80, 0x6f); 	/* [2,16] v=1  bytes:272     #5e0# RefModel iPhv 2o */
    phv_in2->set( 81, 0x58); 	/* [2,17] v=1  bytes:273     #5e0# RefModel iPhv 2o */
    phv_in2->set( 82, 0x2f); 	/* [2,18] v=1  bytes:274     #5e0# RefModel iPhv 2o */
    phv_in2->set( 83, 0x8e); 	/* [2,19] v=1  bytes:275     #5e0# RefModel iPhv 2o */
    phv_in2->set( 84, 0x63); 	/* [2,20] v=1  bytes:276     #5e0# RefModel iPhv 2o */
    phv_in2->set( 85, 0x49); 	/* [2,21] v=1  bytes:277     #5e0# RefModel iPhv 2o */
    phv_in2->set( 86, 0x2e); 	/* [2,22] v=1  bytes:278     #5e0# RefModel iPhv 2o */
    phv_in2->set( 87, 0x9f); 	/* [2,23] v=1  bytes:279     #5e0# RefModel iPhv 2o */
    phv_in2->set( 88, 0x41); 	/* [2,24] v=1  bytes:280     #5e0# RefModel iPhv 2o */
    phv_in2->set( 89, 0xa2); 	/* [2,25] v=1  bytes:281     #5e0# RefModel iPhv 2o */
    phv_in2->set( 90, 0xa0); 	/* [2,26] v=1  bytes:282     #5e0# RefModel iPhv 2o */
    phv_in2->set( 91, 0x40); 	/* [2,27] v=1  bytes:283     #5e0# RefModel iPhv 2o */
    phv_in2->set( 92, 0xad); 	/* [2,28] v=1  bytes:284     #5e0# RefModel iPhv 2o */
    phv_in2->set( 93, 0x54); 	/* [2,29] v=1  bytes:285     #5e0# RefModel iPhv 2o */
    phv_in2->set( 94, 0xcb); 	/* [2,30] v=1  bytes:286     #5e0# RefModel iPhv 2o */
    phv_in2->set( 95, 0xdf); 	/* [2,31] v=1  bytes:287     #5e0# RefModel iPhv 2o */
    phv_in2->set( 96, 0xb1); 	/* [3, 0] v=1  bytes:288     #5e0# RefModel iPhv 2o */
    phv_in2->set( 97, 0xf1); 	/* [3, 1] v=1  bytes:289     #5e0# RefModel iPhv 2o */
    phv_in2->set( 98, 0xf2); 	/* [3, 2] v=1  bytes:290     #5e0# RefModel iPhv 2o */
    phv_in2->set( 99, 0x03); 	/* [3, 3] v=1  bytes:291     #5e0# RefModel iPhv 2o */
    phv_in2->set(100, 0x7c); 	/* [3, 4] v=1  bytes:292     #5e0# RefModel iPhv 2o */
    phv_in2->set(101, 0x48); 	/* [3, 5] v=1  bytes:293     #5e0# RefModel iPhv 2o */
    phv_in2->set(102, 0x9e); 	/* [3, 6] v=1  bytes:294     #5e0# RefModel iPhv 2o */
    phv_in2->set(103, 0x19); 	/* [3, 7] v=1  bytes:295     #5e0# RefModel iPhv 2o */
    phv_in2->set(104, 0x64); 	/* [3, 8] v=1  bytes:296     #5e0# RefModel iPhv 2o */
    phv_in2->set(105, 0x19); 	/* [3, 9] v=1  bytes:297     #5e0# RefModel iPhv 2o */
    phv_in2->set(106, 0x36); 	/* [3,10] v=1  bytes:298     #5e0# RefModel iPhv 2o */
    phv_in2->set(107, 0xa3); 	/* [3,11] v=1  bytes:299     #5e0# RefModel iPhv 2o */
    phv_in2->set(108, 0x5a); 	/* [3,12] v=1  bytes:300     #5e0# RefModel iPhv 2o */
    phv_in2->set(109, 0xf6); 	/* [3,13] v=1  bytes:301     #5e0# RefModel iPhv 2o */
    phv_in2->set(110, 0x9d); 	/* [3,14] v=1  bytes:302     #5e0# RefModel iPhv 2o */
    phv_in2->set(111, 0xf9); 	/* [3,15] v=1  bytes:303     #5e0# RefModel iPhv 2o */
    phv_in2->set(112, 0xf6); 	/* [3,16] v=1  bytes:304     #5e0# RefModel iPhv 2o */
    phv_in2->set(113, 0xaa); 	/* [3,17] v=1  bytes:305     #5e0# RefModel iPhv 2o */
    phv_in2->set(114, 0xad); 	/* [3,18] v=1  bytes:306     #5e0# RefModel iPhv 2o */
    phv_in2->set(115, 0xda); 	/* [3,19] v=1  bytes:307     #5e0# RefModel iPhv 2o */
    phv_in2->set(116, 0x9e); 	/* [3,20] v=1  bytes:308     #5e0# RefModel iPhv 2o */
    phv_in2->set(117, 0x7a); 	/* [3,21] v=1  bytes:309     #5e0# RefModel iPhv 2o */
    phv_in2->set(118, 0x46); 	/* [3,22] v=1  bytes:310     #5e0# RefModel iPhv 2o */
    phv_in2->set(119, 0xa9); 	/* [3,23] v=1  bytes:311     #5e0# RefModel iPhv 2o */
    phv_in2->set(120, 0xb0); 	/* [3,24] v=1  bytes:312     #5e0# RefModel iPhv 2o */
    phv_in2->set(121, 0x6d); 	/* [3,25] v=1  bytes:313     #5e0# RefModel iPhv 2o */
    phv_in2->set(122, 0x31); 	/* [3,26] v=1  bytes:314     #5e0# RefModel iPhv 2o */
    phv_in2->set(123, 0x87); 	/* [3,27] v=1  bytes:315     #5e0# RefModel iPhv 2o */
    phv_in2->set(124, 0x62); 	/* [3,28] v=1  bytes:316     #5e0# RefModel iPhv 2o */
    phv_in2->set(125, 0x8c); 	/* [3,29] v=1  bytes:317     #5e0# RefModel iPhv 2o */
    phv_in2->set(126, 0x8f); 	/* [3,30] v=1  bytes:318     #5e0# RefModel iPhv 2o */
    phv_in2->set(127, 0x31); 	/* [3,31] v=1  bytes:319     #5e0# RefModel iPhv 2o */
    phv_in2->set(128, 0xa94e); 	/* [4, 0] v=1  bytes:321-320  #5e0# RefModel iPhv 2o */
    phv_in2->set(129, 0xd077); 	/* [4, 1] v=1  bytes:323-322  #5e0# RefModel iPhv 2o */
    phv_in2->set(130, 0x3d3e); 	/* [4, 2] v=1  bytes:325-324  #5e0# RefModel iPhv 2o */
    phv_in2->set(131, 0x03c6); 	/* [4, 3] v=1  bytes:327-326  #5e0# RefModel iPhv 2o */
    phv_in2->set(132, 0x3412); 	/* [4, 4] v=1  bytes:329-328  #5e0# RefModel iPhv 2o */
    phv_in2->set(133, 0x6690); 	/* [4, 5] v=1  bytes:331-330  #5e0# RefModel iPhv 2o */
    phv_in2->set(134, 0xe18b); 	/* [4, 6] v=1  bytes:333-332  #5e0# RefModel iPhv 2o */
    phv_in2->set(135, 0x78b0); 	/* [4, 7] v=1  bytes:335-334  #5e0# RefModel iPhv 2o */
    phv_in2->set(136, 0x6c31); 	/* [4, 8] v=1  bytes:337-336  #5e0# RefModel iPhv 2o */
    phv_in2->set(137, 0x665c); 	/* [4, 9] v=1  bytes:339-338  #5e0# RefModel iPhv 2o */
    phv_in2->set(138, 0x65ba); 	/* [4,10] v=1  bytes:341-340  #5e0# RefModel iPhv 2o */
    phv_in2->set(139, 0x5fb0); 	/* [4,11] v=1  bytes:343-342  #5e0# RefModel iPhv 2o */
    phv_in2->set(140, 0x07a4); 	/* [4,12] v=1  bytes:345-344  #5e0# RefModel iPhv 2o */
    phv_in2->set(141, 0xf255); 	/* [4,13] v=1  bytes:347-346  #5e0# RefModel iPhv 2o */
    phv_in2->set(142, 0xb7d4); 	/* [4,14] v=1  bytes:349-348  #5e0# RefModel iPhv 2o */
    phv_in2->set(143, 0xd51f); 	/* [4,15] v=1  bytes:351-350  #5e0# RefModel iPhv 2o */
    phv_in2->set(144, 0x4cd8); 	/* [4,16] v=1  bytes:353-352  #5e0# RefModel iPhv 2o */
    phv_in2->set(145, 0xc28d); 	/* [4,17] v=1  bytes:355-354  #5e0# RefModel iPhv 2o */
    phv_in2->set(146, 0xcf17); 	/* [4,18] v=1  bytes:357-356  #5e0# RefModel iPhv 2o */
    phv_in2->set(147, 0x44f3); 	/* [4,19] v=1  bytes:359-358  #5e0# RefModel iPhv 2o */
    phv_in2->set(148, 0x4724); 	/* [4,20] v=1  bytes:361-360  #5e0# RefModel iPhv 2o */
    phv_in2->set(149, 0x279b); 	/* [4,21] v=1  bytes:363-362  #5e0# RefModel iPhv 2o */
    phv_in2->set(150, 0xdaf8); 	/* [4,22] v=1  bytes:365-364  #5e0# RefModel iPhv 2o */
    phv_in2->set(151, 0xb28b); 	/* [4,23] v=1  bytes:367-366  #5e0# RefModel iPhv 2o */
    phv_in2->set(152, 0xc153); 	/* [4,24] v=1  bytes:369-368  #5e0# RefModel iPhv 2o */
    phv_in2->set(153, 0x0ce6); 	/* [4,25] v=1  bytes:371-370  #5e0# RefModel iPhv 2o */
    phv_in2->set(154, 0xa7b2); 	/* [4,26] v=1  bytes:373-372  #5e0# RefModel iPhv 2o */
    phv_in2->set(155, 0x78d4); 	/* [4,27] v=1  bytes:375-374  #5e0# RefModel iPhv 2o */
    phv_in2->set(156, 0x9fa6); 	/* [4,28] v=1  bytes:377-376  #5e0# RefModel iPhv 2o */
    phv_in2->set(157, 0xb3f1); 	/* [4,29] v=1  bytes:379-378  #5e0# RefModel iPhv 2o */
    phv_in2->set(158, 0x0a0d); 	/* [4,30] v=1  bytes:381-380  #5e0# RefModel iPhv 2o */
    phv_in2->set(159, 0x5464); 	/* [4,31] v=1  bytes:383-382  #5e0# RefModel iPhv 2o */
    phv_in2->set(160, 0xd931); 	/* [5, 0] v=1  bytes:385-384  #5e0# RefModel iPhv 2o */
    phv_in2->set(161, 0x7eb5); 	/* [5, 1] v=1  bytes:387-386  #5e0# RefModel iPhv 2o */
    phv_in2->set(162, 0x3351); 	/* [5, 2] v=1  bytes:389-388  #5e0# RefModel iPhv 2o */
    phv_in2->set(163, 0x55f4); 	/* [5, 3] v=1  bytes:391-390  #5e0# RefModel iPhv 2o */
    phv_in2->set(164, 0x566f); 	/* [5, 4] v=1  bytes:393-392  #5e0# RefModel iPhv 2o */
    phv_in2->set(165, 0x07c0); 	/* [5, 5] v=1  bytes:395-394  #5e0# RefModel iPhv 2o */
    phv_in2->set(166, 0xc0a1); 	/* [5, 6] v=1  bytes:397-396  #5e0# RefModel iPhv 2o */
    phv_in2->set(167, 0x0d4d); 	/* [5, 7] v=1  bytes:399-398  #5e0# RefModel iPhv 2o */
    phv_in2->set(168, 0xfcdb); 	/* [5, 8] v=1  bytes:401-400  #5e0# RefModel iPhv 2o */
    phv_in2->set(169, 0xab14); 	/* [5, 9] v=1  bytes:403-402  #5e0# RefModel iPhv 2o */
    phv_in2->set(170, 0xdce8); 	/* [5,10] v=1  bytes:405-404  #5e0# RefModel iPhv 2o */
    phv_in2->set(171, 0x8000); 	/* [5,11] v=1  bytes:407-406  #5e0# RefModel iPhv 2o */
    phv_in2->set(172, 0x05f0); 	/* [5,12] v=1  bytes:409-408  #5e0# RefModel iPhv 2o */
    phv_in2->set(173, 0x096b); 	/* [5,13] v=1  bytes:411-410  #5e0# RefModel iPhv 2o */
    phv_in2->set(174, 0x0398); 	/* [5,14] v=1  bytes:413-412  #5e0# RefModel iPhv 2o */
    phv_in2->set(175, 0x3a0b); 	/* [5,15] v=1  bytes:415-414  #5e0# RefModel iPhv 2o */
    phv_in2->set(176, 0x3b74); 	/* [5,16] v=1  bytes:417-416  #5e0# RefModel iPhv 2o */
    phv_in2->set(177, 0xaa9f); 	/* [5,17] v=1  bytes:419-418  #5e0# RefModel iPhv 2o */
    phv_in2->set(178, 0x3ffb); 	/* [5,18] v=1  bytes:421-420  #5e0# RefModel iPhv 2o */
    phv_in2->set(179, 0x64e3); 	/* [5,19] v=1  bytes:423-422  #5e0# RefModel iPhv 2o */
    phv_in2->set(180, 0x4da0); 	/* [5,20] v=1  bytes:425-424  #5e0# RefModel iPhv 2o */
    phv_in2->set(181, 0xcd62); 	/* [5,21] v=1  bytes:427-426  #5e0# RefModel iPhv 2o */
    phv_in2->set(182, 0xd59f); 	/* [5,22] v=1  bytes:429-428  #5e0# RefModel iPhv 2o */
    phv_in2->set(183, 0xe46f); 	/* [5,23] v=1  bytes:431-430  #5e0# RefModel iPhv 2o */
    phv_in2->set(184, 0xddaa); 	/* [5,24] v=1  bytes:433-432  #5e0# RefModel iPhv 2o */
    phv_in2->set(185, 0x9c9b); 	/* [5,25] v=1  bytes:435-434  #5e0# RefModel iPhv 2o */
    phv_in2->set(186, 0x5e42); 	/* [5,26] v=1  bytes:437-436  #5e0# RefModel iPhv 2o */
    phv_in2->set(187, 0x68a1); 	/* [5,27] v=1  bytes:439-438  #5e0# RefModel iPhv 2o */
    phv_in2->set(188, 0xbc45); 	/* [5,28] v=1  bytes:441-440  #5e0# RefModel iPhv 2o */
    phv_in2->set(189, 0x83b1); 	/* [5,29] v=1  bytes:443-442  #5e0# RefModel iPhv 2o */
    phv_in2->set(190, 0xed67); 	/* [5,30] v=1  bytes:445-444  #5e0# RefModel iPhv 2o */
    phv_in2->set(191, 0xef6a); 	/* [5,31] v=1  bytes:447-446  #5e0# RefModel iPhv 2o */
    phv_in2->set(192, 0x7155); 	/* [6, 0] v=1  bytes:449-448  #5e0# RefModel iPhv 2o */
    phv_in2->set(193, 0x6a8d); 	/* [6, 1] v=1  bytes:451-450  #5e0# RefModel iPhv 2o */
    phv_in2->set(194, 0xf0f2); 	/* [6, 2] v=1  bytes:453-452  #5e0# RefModel iPhv 2o */
    phv_in2->set(195, 0xcdb4); 	/* [6, 3] v=1  bytes:455-454  #5e0# RefModel iPhv 2o */
    phv_in2->set(196, 0x963b); 	/* [6, 4] v=1  bytes:457-456  #5e0# RefModel iPhv 2o */
    phv_in2->set(197, 0xeb0c); 	/* [6, 5] v=1  bytes:459-458  #5e0# RefModel iPhv 2o */
    phv_in2->set(198, 0x3df7); 	/* [6, 6] v=1  bytes:461-460  #5e0# RefModel iPhv 2o */
    phv_in2->set(199, 0x3d9f); 	/* [6, 7] v=1  bytes:463-462  #5e0# RefModel iPhv 2o */
    phv_in2->set(200, 0x06ac); 	/* [6, 8] v=1  bytes:465-464  #5e0# RefModel iPhv 2o */
    phv_in2->set(201, 0xc8b1); 	/* [6, 9] v=1  bytes:467-466  #5e0# RefModel iPhv 2o */
    phv_in2->set(202, 0x5e15); 	/* [6,10] v=1  bytes:469-468  #5e0# RefModel iPhv 2o */
    phv_in2->set(203, 0x1569); 	/* [6,11] v=1  bytes:471-470  #5e0# RefModel iPhv 2o */
    phv_in2->set(204, 0xf259); 	/* [6,12] v=1  bytes:473-472  #5e0# RefModel iPhv 2o */
    phv_in2->set(205, 0x1989); 	/* [6,13] v=1  bytes:475-474  #5e0# RefModel iPhv 2o */
    phv_in2->set(206, 0x8c6c); 	/* [6,14] v=1  bytes:477-476  #5e0# RefModel iPhv 2o */
    phv_in2->set(207, 0x4332); 	/* [6,15] v=1  bytes:479-478  #5e0# RefModel iPhv 2o */
    phv_in2->set(208, 0xe664); 	/* [6,16] v=1  bytes:481-480  #5e0# RefModel iPhv 2o */
    phv_in2->set(209, 0x224a); 	/* [6,17] v=1  bytes:483-482  #5e0# RefModel iPhv 2o */
    phv_in2->set(210, 0x90ba); 	/* [6,18] v=1  bytes:485-484  #5e0# RefModel iPhv 2o */
    phv_in2->set(211, 0xb9e7); 	/* [6,19] v=1  bytes:487-486  #5e0# RefModel iPhv 2o */
    phv_in2->set(212, 0x2e13); 	/* [6,20] v=1  bytes:489-488  #5e0# RefModel iPhv 2o */
    phv_in2->set(213, 0xcba8); 	/* [6,21] v=1  bytes:491-490  #5e0# RefModel iPhv 2o */
    phv_in2->set(214, 0x43ab); 	/* [6,22] v=1  bytes:493-492  #5e0# RefModel iPhv 2o */
    phv_in2->set(215, 0xa391); 	/* [6,23] v=1  bytes:495-494  #5e0# RefModel iPhv 2o */
    phv_in2->set(216, 0x67f5); 	/* [6,24] v=1  bytes:497-496  #5e0# RefModel iPhv 2o */
    phv_in2->set(217, 0x21d3); 	/* [6,25] v=1  bytes:499-498  #5e0# RefModel iPhv 2o */
    phv_in2->set(218, 0x97ea); 	/* [6,26] v=1  bytes:501-500  #5e0# RefModel iPhv 2o */
    phv_in2->set(219, 0xe82c); 	/* [6,27] v=1  bytes:503-502  #5e0# RefModel iPhv 2o */
    phv_in2->set(220, 0x39f0); 	/* [6,28] v=1  bytes:505-504  #5e0# RefModel iPhv 2o */
    phv_in2->set(221, 0xa07f); 	/* [6,29] v=1  bytes:507-506  #5e0# RefModel iPhv 2o */
    phv_in2->set(222, 0xf4a3); 	/* [6,30] v=1  bytes:509-508  #5e0# RefModel iPhv 2o */
    phv_in2->set(223, 0x6bdd); 	/* [6,31] v=1  bytes:511-510  #5e0# RefModel iPhv 2o */



    
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
    RMT_UT_LOG_INFO("Dv42Test::InPkt=%p [DA=%04X%08X]\n", p_in,
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


    Phv *phv_out2 = tu.port_process_inbound(port,    phv_in2);
    //TestUtil::compare_phvs(phv_in2,phv_out2,true);
    EXPECT_EQ(0x0, phv_out2->get(0)); // was 0x756a045e in input phv
    EXPECT_EQ(0x0, phv_out2->get(9)); // was 0x2afc3a71 in input phv
    EXPECT_EQ(0x0, phv_out2->get(12)); // was 0xb3f7d4c3 in input phv
    //EXPECT_EQ(0xee079d2, phv_out2->get(13)); // was 0xeb2671d3 in input phv
    //EXPECT_EQ(0xc85df9e2, phv_out2->get(15)); // was 0xfbb0107c in input phv
    //EXPECT_EQ(0x12296ffd, phv_out2->get(16)); // was 0xc21f9aa6 in input phv
    EXPECT_EQ(0x20000385, phv_out2->get(17)); // was 0x4461fe9a in input phv
    EXPECT_EQ(0xc21f9aa6, phv_out2->get(18)); // was 0x3b28f576 in input phv
    EXPECT_EQ(0x0, phv_out2->get(20)); // was 0x40f0c39a in input phv
    EXPECT_EQ(0x4461fe9a, phv_out2->get(23)); // was 0xee079d2 in input phv
    EXPECT_EQ(0x0, phv_out2->get(24)); // was 0x93013971 in input phv
    EXPECT_EQ(0x4461fe9a, phv_out2->get(30)); // was 0x2b9de2c9 in input phv
    //EXPECT_EQ(0xcae334e3, phv_out2->get(31)); // was 0x202eab85 in input phv
    //EXPECT_EQ(0x12da55d8, phv_out2->get(33)); // was 0x5ed0933e in input phv
    EXPECT_EQ(0x10000000, phv_out2->get(36)); // was 0xd295fa53 in input phv
    EXPECT_EQ(0x0, phv_out2->get(38)); // was 0xe8f39402 in input phv
    //EXPECT_EQ(0x12d, phv_out2->get(41)); // was 0x950b1534 in input phv
    //EXPECT_EQ(0xae1bfce, phv_out2->get(42)); // was 0xb423d965 in input phv
    //EXPECT_EQ(0x25ea40c6, phv_out2->get(43)); // was 0x68d73bc5 in input phv
    EXPECT_EQ(0x17bc94ef, phv_out2->get(45)); // was 0xbaa3bc15 in input phv
    EXPECT_EQ(0x8f9c86a1, phv_out2->get(49)); // was 0x60581634 in input phv
    EXPECT_EQ(0x5dfe7c, phv_out2->get(50)); // was 0x74db03 in input phv
    //EXPECT_EQ(0xb423d965, phv_out2->get(51)); // was 0x67625321 in input phv
    //EXPECT_EQ(0x5ed0933e, phv_out2->get(54)); // was 0x9d82314e in input phv
    //EXPECT_EQ(0x143fbd, phv_out2->get(56)); // was 0xcfd0e33b in input phv
    EXPECT_EQ(0x0, phv_out2->get(57)); // was 0xda596e9b in input phv
    EXPECT_EQ(0x74db03, phv_out2->get(58)); // was 0x6fdf6b70 in input phv
    EXPECT_EQ(0x0, phv_out2->get(59)); // was 0x7cb62484 in input phv
    //EXPECT_EQ(0xaff31d58, phv_out2->get(60)); // was 0x25ea40c6 in input phv
    //EXPECT_EQ(0x14143fbd, phv_out2->get(61)); // was 0x8f9c86a1 in input phv


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
