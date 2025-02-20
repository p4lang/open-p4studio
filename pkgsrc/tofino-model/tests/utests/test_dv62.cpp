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

// XXX -> test_dv62.cpp
// Exact Match inner hits not prioritized
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

  bool dv62_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv62Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv62_print) RMT_UT_LOG_INFO("test_dv62_packet1()\n");
    
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
    tu.set_dv_test(62);
    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
    
    
    auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);
    tu.OutWordPiT(0, 0, &mau_reg_map.dp.phv_ingress_thread[0][0], 0xa2244628); /* 0x2060080 */
    tu.OutWordPeT(0, 0, &mau_reg_map.dp.phv_egress_thread[0][0], 0x1413890); /* 0x20600c0 */
    tu.OutWordPiT(0, 1, &mau_reg_map.dp.phv_ingress_thread[0][1], 0x214d7dfe); /* 0x2060084 */
    tu.OutWordPeT(0, 1, &mau_reg_map.dp.phv_egress_thread[0][1], 0x48020000); /* 0x20600c4 */
    tu.OutWordPiT(0, 2, &mau_reg_map.dp.phv_ingress_thread[0][2], 0x64add69e); /* 0x2060088 */
    tu.OutWordPeT(0, 2, &mau_reg_map.dp.phv_egress_thread[0][2], 0x92020040); /* 0x20600c8 */
    tu.OutWordPiT(0, 3, &mau_reg_map.dp.phv_ingress_thread[0][3], 0x623712eb); /* 0x206008c */
    tu.OutWordPeT(0, 3, &mau_reg_map.dp.phv_egress_thread[0][3], 0x90c8c000); /* 0x20600cc */
    tu.OutWordPiT(0, 4, &mau_reg_map.dp.phv_ingress_thread[0][4], 0xfd18bb75); /* 0x2060090 */
    tu.OutWordPeT(0, 4, &mau_reg_map.dp.phv_egress_thread[0][4], 0x260008); /* 0x20600d0 */
    tu.OutWordPiT(0, 5, &mau_reg_map.dp.phv_ingress_thread[0][5], 0xacc1aa63); /* 0x2060094 */
    tu.OutWordPeT(0, 5, &mau_reg_map.dp.phv_egress_thread[0][5], 0x40264010); /* 0x20600d4 */
    tu.OutWordPiT(0, 6, &mau_reg_map.dp.phv_ingress_thread[0][6], 0xa971334a); /* 0x2060098 */
    tu.OutWordPeT(0, 6, &mau_reg_map.dp.phv_egress_thread[0][6], 0x54860cb1); /* 0x20600d8 */
    tu.OutWordPiT(1, 0, &mau_reg_map.dp.phv_ingress_thread[1][0], 0xa2244628); /* 0x20600a0 */
    tu.OutWordPeT(1, 0, &mau_reg_map.dp.phv_egress_thread[1][0], 0x1413890); /* 0x20600e0 */
    tu.OutWordPiT(1, 1, &mau_reg_map.dp.phv_ingress_thread[1][1], 0x214d7dfe); /* 0x20600a4 */
    tu.OutWordPeT(1, 1, &mau_reg_map.dp.phv_egress_thread[1][1], 0x48020000); /* 0x20600e4 */
    tu.OutWordPiT(1, 2, &mau_reg_map.dp.phv_ingress_thread[1][2], 0x64add69e); /* 0x20600a8 */
    tu.OutWordPeT(1, 2, &mau_reg_map.dp.phv_egress_thread[1][2], 0x92020040); /* 0x20600e8 */
    tu.OutWordPiT(1, 3, &mau_reg_map.dp.phv_ingress_thread[1][3], 0x623712eb); /* 0x20600ac */
    tu.OutWordPeT(1, 3, &mau_reg_map.dp.phv_egress_thread[1][3], 0x90c8c000); /* 0x20600ec */
    tu.OutWordPiT(1, 4, &mau_reg_map.dp.phv_ingress_thread[1][4], 0xfd18bb75); /* 0x20600b0 */
    tu.OutWordPeT(1, 4, &mau_reg_map.dp.phv_egress_thread[1][4], 0x260008); /* 0x20600f0 */
    tu.OutWordPiT(1, 5, &mau_reg_map.dp.phv_ingress_thread[1][5], 0xacc1aa63); /* 0x20600b4 */
    tu.OutWordPeT(1, 5, &mau_reg_map.dp.phv_egress_thread[1][5], 0x40264010); /* 0x20600f4 */
    tu.OutWordPiT(1, 6, &mau_reg_map.dp.phv_ingress_thread[1][6], 0xa971334a); /* 0x20600b8 */
    tu.OutWordPeT(1, 6, &mau_reg_map.dp.phv_egress_thread[1][6], 0x54860cb1); /* 0x20600f8 */
    tu.OutWord(&mau_reg_map.dp.action_output_delay[0], 0xb); /* 0x2060018 */
    tu.OutWord(&mau_reg_map.dp.action_output_delay[1], 0xb); /* 0x206001c */
    tu.OutWord(&mau_reg_map.dp.match_ie_input_mux_sel, 0x3); /* 0x2060038 */
    tu.OutWord(&mau_reg_map.dp.phv_fifo_enable, 0xc); /* 0x2060000 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.deferred_eop_bus_delay[0], 0x5c7); /* 0x2018728 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.deferred_eop_bus_delay[1], 0x5c7); /* 0x201872c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][0], 0x13); /* 0x2067000 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][1], 0x1e); /* 0x2067804 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][2], 0x44); /* 0x2064008 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][3], 0x1d); /* 0x2067c0c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][4], 0x41); /* 0x2064c10 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][5], 0x12); /* 0x2067814 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][6], 0x56); /* 0x2064018 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][7], 0x4c); /* 0x206481c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][8], 0x15); /* 0x2067420 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][9], 0x12); /* 0x2067824 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][10], 0x66); /* 0x2064828 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][11], 0x1d); /* 0x206742c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][12], 0x58); /* 0x2064030 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][13], 0x5c); /* 0x2064c34 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][14], 0x1a); /* 0x2067838 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][15], 0x65); /* 0x206443c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][16], 0x1b); /* 0x2067c40 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][17], 0x59); /* 0x2064c44 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][18], 0x11); /* 0x2067c48 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][19], 0x1b); /* 0x206704c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][20], 0x4f); /* 0x2064850 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][21], 0x17); /* 0x2067054 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][22], 0x65); /* 0x2064858 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][23], 0x60); /* 0x206485c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][24], 0x52); /* 0x2064c60 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][25], 0x4c); /* 0x2064c64 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][26], 0x64); /* 0x2064868 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][27], 0x5f); /* 0x2064c6c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][28], 0x17); /* 0x2067070 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][29], 0x46); /* 0x2064074 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][30], 0x41); /* 0x2064478 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][31], 0x52); /* 0x206487c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][32], 0x1e); /* 0x2067c80 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][33], 0x1b); /* 0x2067c84 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][34], 0x60); /* 0x2064888 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][35], 0x14); /* 0x206708c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][36], 0x43); /* 0x2064c90 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][37], 0x62); /* 0x2064c94 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][38], 0x41); /* 0x2064498 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][39], 0x1e); /* 0x2067c9c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][40], 0x53); /* 0x2064ca0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][41], 0x16); /* 0x20674a4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][42], 0x67); /* 0x20648a8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][43], 0x51); /* 0x20648ac */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][44], 0x4e); /* 0x20648b0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][45], 0x62); /* 0x2064cb4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][46], 0x5a); /* 0x2064cb8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][47], 0x46); /* 0x20640bc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][48], 0x61); /* 0x20648c0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][49], 0x5d); /* 0x2064cc4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][50], 0x42); /* 0x20640c8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][51], 0x1d); /* 0x2067ccc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][52], 0x47); /* 0x20648d0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][53], 0x1a); /* 0x20670d4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][54], 0x63); /* 0x20640d8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][55], 0x52); /* 0x20640dc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][56], 0x5d); /* 0x2064ce0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][57], 0x4d); /* 0x20644e4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][58], 0x45); /* 0x2064ce8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][59], 0x54); /* 0x20640ec */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][60], 0x61); /* 0x20648f0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][61], 0x55); /* 0x20640f4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][62], 0x52); /* 0x20644f8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][63], 0x58); /* 0x2064cfc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][64], 0x52); /* 0x2064900 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][65], 0x5e); /* 0x2064904 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][66], 0x53); /* 0x2064108 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][67], 0x46); /* 0x2064d0c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][68], 0x1e); /* 0x2067d10 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][69], 0x17); /* 0x2067114 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][70], 0x1c); /* 0x2067118 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][71], 0x11); /* 0x2067d1c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][72], 0x5e); /* 0x2064920 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][73], 0x4f); /* 0x2064524 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][74], 0x46); /* 0x2064128 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][75], 0x18); /* 0x206752c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][76], 0x65); /* 0x2064130 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][77], 0x5f); /* 0x2064d34 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][78], 0x1b); /* 0x2067d38 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][79], 0x16); /* 0x206753c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][80], 0x42); /* 0x2064540 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][81], 0x65); /* 0x2064d44 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][82], 0x4d); /* 0x2064d48 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][83], 0x41); /* 0x206494c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][84], 0x12); /* 0x2067d50 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][85], 0x58); /* 0x2064d54 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][86], 0x1d); /* 0x2067d58 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][87], 0x47); /* 0x206415c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][88], 0x4c); /* 0x2064160 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][89], 0x1e); /* 0x2067164 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][90], 0x46); /* 0x2064968 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][91], 0x61); /* 0x206456c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][92], 0x43); /* 0x2064170 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][93], 0x4c); /* 0x2064174 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][94], 0x18); /* 0x2067d78 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][95], 0x5d); /* 0x2064d7c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][96], 0x1e); /* 0x2067180 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][97], 0x47); /* 0x2064584 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][98], 0x55); /* 0x2064d88 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][99], 0x1e); /* 0x206718c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][100], 0x5e); /* 0x2064d90 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][101], 0x45); /* 0x2064594 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][102], 0x19); /* 0x2067198 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][103], 0x66); /* 0x206499c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][104], 0x4e); /* 0x20641a0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][105], 0x1e); /* 0x20671a4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][106], 0x47); /* 0x20645a8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][107], 0x18); /* 0x2067dac */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][108], 0x1c); /* 0x20679b0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][109], 0x1e); /* 0x20679b4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][110], 0x5e); /* 0x20645b8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][111], 0x53); /* 0x20645bc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][112], 0x49); /* 0x20649c0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][113], 0x5f); /* 0x20641c4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][114], 0x42); /* 0x20641c8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][115], 0x42); /* 0x2064dcc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][116], 0x5e); /* 0x2064dd0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][117], 0x47); /* 0x20649d4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][118], 0x52); /* 0x20641d8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][119], 0x13); /* 0x20671dc */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][120], 0x58); /* 0x20641e0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][121], 0x55); /* 0x20641e4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][122], 0x1a); /* 0x20679e8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][123], 0x40); /* 0x20645ec */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][124], 0x49); /* 0x2064df0 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][125], 0x1f); /* 0x20675f4 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][126], 0x43); /* 0x20645f8 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][127], 0x40); /* 0x20649fc */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][0], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][2], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][5], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][6], 0x56); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][9], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][12], 0x58); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][21], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][28], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][29], 0x46); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][31], 0x5e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][35], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][43], 0x5d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][47], 0x46); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][50], 0x42); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][52], 0x4f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][55], 0x52); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][59], 0x54); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][61], 0x55); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][64], 0x5e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][66], 0x53); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][69], 0xf); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][74], 0x46); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][83], 0x49); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][87], 0x47); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][90], 0x4e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][92], 0x43); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][114], 0x42); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][117], 0x4f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][118], 0x52); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[0][119], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][120], 0x58); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][121], 0x55); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[0][127], 0x48); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][1], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][7], 0x4c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][10], 0x66); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][14], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][19], 0xb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][20], 0x4f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][22], 0x65); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][23], 0x60); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][26], 0x64); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][34], 0x60); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][42], 0x67); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][44], 0x4e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][48], 0x61); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][53], 0xa); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][54], 0x57); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][60], 0x61); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][65], 0x5e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][70], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][72], 0x5e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][76], 0x59); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][88], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][89], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][93], 0x44); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][96], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][99], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][102], 0x9); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][103], 0x66); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][104], 0x46); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][105], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][108], 0x14); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][109], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][112], 0x49); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[1][113], 0x53); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[1][122], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][4], 0x49); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][8], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][17], 0x65); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][18], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][24], 0x5e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][30], 0x41); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][36], 0x4b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][38], 0x41); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][40], 0x5f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][41], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][46], 0x66); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][58], 0x4d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][62], 0x52); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][63], 0x64); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][67], 0x4e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][71], 0x11); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][79], 0xe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][80], 0x42); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[2][84], 0x12); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][85], 0x64); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][97], 0x47); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][98], 0x61); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][101], 0x45); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][106], 0x47); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][111], 0x53); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][115], 0x4a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][123], 0x40); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[2][126], 0x43); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][3], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][11], 0xd); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][13], 0x5c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][15], 0x59); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][16], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][25], 0x4c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][27], 0x5f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][32], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][33], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][37], 0x62); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][39], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][45], 0x62); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][49], 0x5d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][51], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][56], 0x5d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][57], 0x45); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][68], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][73], 0x47); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][75], 0x8); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][77], 0x5f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][78], 0x13); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][81], 0x65); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][82], 0x4d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][86], 0x15); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][91], 0x55); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][94], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][95], 0x5d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][100], 0x5e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][107], 0x10); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][110], 0x52); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][116], 0x5e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_816b_ctl[3][124], 0x49); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.xbar.match_input_xbar_32b_ctl[3][125], 0xf); // regs_31841 fix
    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.unitram_config[0][2], 0x45ba); /* 0x2012948 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[0], 0xb); /* 0x20121c0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl, 0xb); /* 0x20121c4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[2], 0xb); /* 0x20121c8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[3], 0xb); /* 0x20121cc */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[4], 0xb); /* 0x20121d0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[5], 0xb); /* 0x20121d4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[0], 0xb); /* 0x20121e0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[1], 0xb); /* 0x20121e4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[2], 0xb); /* 0x20121e8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[3], 0xb); /* 0x20121ec */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[4], 0xb); /* 0x20121f0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.mapram_ctl[5], 0xb); /* 0x20121f4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[0], 0xb); /* 0x20125c0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl, 0xb); /* 0x20125c4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[2], 0xb); /* 0x20125c8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[3], 0xb); /* 0x20125cc */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[4], 0xb); /* 0x20125d0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[5], 0xb); /* 0x20125d4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[0], 0xb); /* 0x20125e0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[1], 0xb); /* 0x20125e4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[2], 0xb); /* 0x20125e8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[3], 0xb); /* 0x20125ec */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[4], 0xb); /* 0x20125f0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[1].adrmux.mapram_ctl[5], 0xb); /* 0x20125f4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[0], 0xb); /* 0x20129c0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl, 0xb); /* 0x20129c4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[2], 0xb); /* 0x20129c8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[3], 0xb); /* 0x20129cc */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[4], 0xb); /* 0x20129d0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[5], 0xb); /* 0x20129d4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[0], 0xb); /* 0x20129e0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[1], 0xb); /* 0x20129e4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[2], 0xb); /* 0x20129e8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[3], 0xb); /* 0x20129ec */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[4], 0xb); /* 0x20129f0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.mapram_ctl[5], 0xb); /* 0x20129f4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[0], 0xb); /* 0x2012dc0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl, 0xb); /* 0x2012dc4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[2], 0xb); /* 0x2012dc8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[3], 0xb); /* 0x2012dcc */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[4], 0xb); /* 0x2012dd0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[5], 0xb); /* 0x2012dd4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[0], 0xb); /* 0x2012de0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[1], 0xb); /* 0x2012de4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[2], 0xb); /* 0x2012de8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[3], 0xb); /* 0x2012dec */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[4], 0xb); /* 0x2012df0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[3].adrmux.mapram_ctl[5], 0xb); /* 0x2012df4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[0], 0xb); /* 0x20131c0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl, 0xb); /* 0x20131c4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[2], 0xb); /* 0x20131c8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[3], 0xb); /* 0x20131cc */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[4], 0xb); /* 0x20131d0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[5], 0xb); /* 0x20131d4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[0], 0xb); /* 0x20131e0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[1], 0xb); /* 0x20131e4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[2], 0xb); /* 0x20131e8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[3], 0xb); /* 0x20131ec */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[4], 0xb); /* 0x20131f0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[4].adrmux.mapram_ctl[5], 0xb); /* 0x20131f4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[0], 0xb); /* 0x20135c0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl, 0xb); /* 0x20135c4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[2], 0xb); /* 0x20135c8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[3], 0xb); /* 0x20135cc */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[4], 0xb); /* 0x20135d0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[5], 0xb); /* 0x20135d4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[0], 0xb); /* 0x20135e0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[1], 0xb); /* 0x20135e4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[2], 0xb); /* 0x20135e8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[3], 0xb); /* 0x20135ec */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[4], 0xb); /* 0x20135f0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[5].adrmux.mapram_ctl[5], 0xb); /* 0x20135f4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[0], 0xb); /* 0x20139c0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl, 0xb); /* 0x20139c4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[2], 0xb); /* 0x20139c8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[3], 0xb); /* 0x20139cc */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[4], 0xb); /* 0x20139d0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[5], 0xb); /* 0x20139d4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[0], 0xb); /* 0x20139e0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[1], 0xb); /* 0x20139e4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[2], 0xb); /* 0x20139e8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[3], 0xb); /* 0x20139ec */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[4], 0xb); /* 0x20139f0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.mapram_ctl[5], 0xb); /* 0x20139f4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[0], 0xb); /* 0x2013dc0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl, 0xb); /* 0x2013dc4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[2], 0xb); /* 0x2013dc8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[3], 0xb); /* 0x2013dcc */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[4], 0xb); /* 0x2013dd0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[5], 0xb); /* 0x2013dd4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[0], 0xb); /* 0x2013de0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[1], 0xb); /* 0x2013de4 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[2], 0xb); /* 0x2013de8 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[3], 0xb); /* 0x2013dec */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[4], 0xb); /* 0x2013df0 */
//    tu.OutWord(&mau_reg_map.rams.map_alu.row[7].adrmux.mapram_ctl[5], 0xb); /* 0x2013df4 */
    tu.OutWord(&mau_reg_map.rams.array.row[2].ram[2].unit_ram_ctl, 0x20); /* 0x200a148 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[0].adrmux.unitram_config[0][0], 0x244a9); /* 0x2012140 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_mask[0], 0xffffffff); /* 0x2008010 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_mask[1], 0x7fffffff); /* 0x2008014 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].unit_ram_ctl, 0xe0bf); /* 0x2008048 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_ram_vpn, 0x223365); /* 0x200804c */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_nibble_s1q0_enable, 0xffffffff); /* 0x2008004 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_nibble_s0q1_enable, 0x1fffffff); /* 0x2008000 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_bytemask[2], 0x1feff); /* 0x2008068 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_bytemask[3], 0x2fdff); /* 0x200806c */
    tu.OutWord(&mau_reg_map.rams.array.row[0].ram[0].match_bytemask[4], 0x37bff); /* 0x2008070 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][0], 0x6); /* 0x2040f00 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][0], 0x3); /* 0x2040c00 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][1], 0x0); /* 0x2040c04 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][2], 0x0); /* 0x2040c08 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][3], 0x4); /* 0x2040c0c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][4], 0x5); /* 0x2040c10 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][5], 0x9); /* 0x2040c14 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][6], 0xa); /* 0x2040c18 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][0][7], 0x6); /* 0x2040c1c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][0], 0x9); /* 0x2040f80 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][0], 0xf); /* 0x2040e00 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][1], 0x1d); /* 0x2040f84 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][1], 0xd); /* 0x2040e04 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][0], 0xf); /* 0x2040f20 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][0], 0x4); /* 0x2040d00 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][1], 0x1); /* 0x2040d04 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][2], 0x2); /* 0x2040d08 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][3], 0x5); /* 0x2040d0c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][4], 0x9); /* 0x2040d10 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][5], 0x7); /* 0x2040d14 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][6], 0x9); /* 0x2040d18 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][0][7], 0x7); /* 0x2040d1c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][0], 0x9); /* 0x2040fc0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][0], 0xe); /* 0x2040e40 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][1], 0x12); /* 0x2040fc4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][1], 0xf); /* 0x2040e44 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][1], 0xd); /* 0x2040f04 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][0], 0x4); /* 0x2040c20 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][1], 0x5); /* 0x2040c24 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][2], 0x1); /* 0x2040c28 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][3], 0x1); /* 0x2040c2c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][4], 0x8); /* 0x2040c30 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][5], 0x7); /* 0x2040c34 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][6], 0x7); /* 0x2040c38 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][1][7], 0x9); /* 0x2040c3c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][2], 0xd); /* 0x2040f88 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][2], 0xf); /* 0x2040e08 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][3], 0x13); /* 0x2040f8c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][3], 0xf); /* 0x2040e0c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][1], 0x6); /* 0x2040f24 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][0], 0x4); /* 0x2040d20 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][1], 0x3); /* 0x2040d24 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][2], 0x2); /* 0x2040d28 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][3], 0x1); /* 0x2040d2c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][4], 0x7); /* 0x2040d30 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][5], 0x5); /* 0x2040d34 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][6], 0x5); /* 0x2040d38 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][1][7], 0x7); /* 0x2040d3c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][2], 0x7); /* 0x2040fc8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][2], 0xc); /* 0x2040e48 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][3], 0xd); /* 0x2040fcc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][3], 0xd); /* 0x2040e4c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][2], 0x3); /* 0x2040f08 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][0], 0x2); /* 0x2040c40 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][1], 0x0); /* 0x2040c44 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][2], 0x1); /* 0x2040c48 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][3], 0x3); /* 0x2040c4c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][4], 0x9); /* 0x2040c50 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][5], 0x8); /* 0x2040c54 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][6], 0x6); /* 0x2040c58 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][2][7], 0x7); /* 0x2040c5c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][4], 0x1f); /* 0x2040f90 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][4], 0xc); /* 0x2040e10 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][5], 0x1a); /* 0x2040f94 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][5], 0xd); /* 0x2040e14 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][2], 0x6); /* 0x2040f28 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][0], 0x5); /* 0x2040d40 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][1], 0x4); /* 0x2040d44 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][2], 0x5); /* 0x2040d48 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][3], 0x0); /* 0x2040d4c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][4], 0x5); /* 0x2040d50 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][5], 0x6); /* 0x2040d54 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][6], 0x7); /* 0x2040d58 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][2][7], 0x5); /* 0x2040d5c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][4], 0xf); /* 0x2040fd0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][4], 0xf); /* 0x2040e50 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][5], 0x14); /* 0x2040fd4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][5], 0xe); /* 0x2040e54 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][3], 0x0); /* 0x2040f0c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][0], 0x4); /* 0x2040c60 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][1], 0x5); /* 0x2040c64 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][2], 0x2); /* 0x2040c68 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][3], 0x4); /* 0x2040c6c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][4], 0x5); /* 0x2040c70 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][5], 0x7); /* 0x2040c74 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][6], 0x5); /* 0x2040c78 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][3][7], 0xa); /* 0x2040c7c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][6], 0x9); /* 0x2040f98 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][6], 0xd); /* 0x2040e18 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][7], 0x8); /* 0x2040f9c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][7], 0xc); /* 0x2040e1c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][3], 0x3); /* 0x2040f2c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][0], 0x4); /* 0x2040d60 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][1], 0x5); /* 0x2040d64 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][2], 0x5); /* 0x2040d68 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][3], 0x2); /* 0x2040d6c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][4], 0x6); /* 0x2040d70 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][5], 0x8); /* 0x2040d74 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][6], 0x6); /* 0x2040d78 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][3][7], 0x6); /* 0x2040d7c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][6], 0x0); /* 0x2040fd8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][6], 0xe); /* 0x2040e58 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][7], 0x3); /* 0x2040fdc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][7], 0xd); /* 0x2040e5c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][4], 0x6); /* 0x2040f10 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][0], 0x0); /* 0x2040c80 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][1], 0x5); /* 0x2040c84 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][2], 0x1); /* 0x2040c88 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][3], 0x1); /* 0x2040c8c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][4], 0x5); /* 0x2040c90 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][5], 0x9); /* 0x2040c94 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][6], 0x8); /* 0x2040c98 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][4][7], 0x7); /* 0x2040c9c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][8], 0xe); /* 0x2040fa0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][8], 0xc); /* 0x2040e20 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][9], 0xb); /* 0x2040fa4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][9], 0xc); /* 0x2040e24 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][4], 0xb); /* 0x2040f30 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][0], 0x2); /* 0x2040d80 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][1], 0x0); /* 0x2040d84 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][2], 0x0); /* 0x2040d88 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][3], 0x3); /* 0x2040d8c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][4], 0x7); /* 0x2040d90 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][5], 0x5); /* 0x2040d94 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][6], 0xa); /* 0x2040d98 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][4][7], 0x7); /* 0x2040d9c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][8], 0x19); /* 0x2040fe0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][8], 0xd); /* 0x2040e60 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][9], 0xf); /* 0x2040fe4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][9], 0xe); /* 0x2040e64 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][5], 0x2); /* 0x2040f14 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][0], 0x4); /* 0x2040ca0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][1], 0x4); /* 0x2040ca4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][2], 0x0); /* 0x2040ca8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][3], 0x3); /* 0x2040cac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][4], 0x7); /* 0x2040cb0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][5], 0x7); /* 0x2040cb4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][6], 0x9); /* 0x2040cb8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][5][7], 0x5); /* 0x2040cbc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][10], 0x9); /* 0x2040fa8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][10], 0xf); /* 0x2040e28 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][11], 0xc); /* 0x2040fac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][11], 0xd); /* 0x2040e2c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][5], 0xb); /* 0x2040f34 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][0], 0x3); /* 0x2040da0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][1], 0x0); /* 0x2040da4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][2], 0x0); /* 0x2040da8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][3], 0x3); /* 0x2040dac */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][4], 0x7); /* 0x2040db0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][5], 0x5); /* 0x2040db4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][6], 0x8); /* 0x2040db8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][5][7], 0x8); /* 0x2040dbc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][10], 0x16); /* 0x2040fe8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][10], 0xd); /* 0x2040e68 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][11], 0x17); /* 0x2040fec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][11], 0xe); /* 0x2040e6c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][6], 0xb); /* 0x2040f18 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][0], 0x3); /* 0x2040cc0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][1], 0x4); /* 0x2040cc4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][2], 0x4); /* 0x2040cc8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][3], 0x2); /* 0x2040ccc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][4], 0x7); /* 0x2040cd0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][5], 0x8); /* 0x2040cd4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][6], 0x6); /* 0x2040cd8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][6][7], 0x7); /* 0x2040cdc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][12], 0x3); /* 0x2040fb0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][12], 0xc); /* 0x2040e30 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][13], 0x1c); /* 0x2040fb4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][13], 0xd); /* 0x2040e34 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][6], 0x2); /* 0x2040f38 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][0], 0x2); /* 0x2040dc0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][1], 0x5); /* 0x2040dc4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][2], 0x5); /* 0x2040dc8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][3], 0x4); /* 0x2040dcc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][4], 0x9); /* 0x2040dd0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][5], 0x7); /* 0x2040dd4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][6], 0x6); /* 0x2040dd8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][6][7], 0x6); /* 0x2040ddc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][12], 0x11); /* 0x2040ff0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][12], 0xf); /* 0x2040e70 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][13], 0x0); /* 0x2040ff4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][13], 0xf); /* 0x2040e74 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[0][7], 0x9); /* 0x2040f1c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][0], 0x3); /* 0x2040ce0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][1], 0x2); /* 0x2040ce4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][2], 0x4); /* 0x2040ce8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][3], 0x1); /* 0x2040cec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][4], 0x7); /* 0x2040cf0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][5], 0x8); /* 0x2040cf4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][6], 0x9); /* 0x2040cf8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[0][7][7], 0x7); /* 0x2040cfc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][14], 0x0); /* 0x2040fb8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][14], 0xf); /* 0x2040e38 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[0][15], 0xa); /* 0x2040fbc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[0][15], 0xf); /* 0x2040e3c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_extra_byte_ctl[1][7], 0xc); /* 0x2040f3c */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][0], 0x2); /* 0x2040de0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][1], 0x0); /* 0x2040de4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][2], 0x4); /* 0x2040de8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][3], 0x1); /* 0x2040dec */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][4], 0x8); /* 0x2040df0 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][5], 0x6); /* 0x2040df4 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][6], 0x7); /* 0x2040df8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_validbit_xbar_ctl[1][7][7], 0x7); /* 0x2040dfc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][14], 0x3); /* 0x2040ff8 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][14], 0xc); /* 0x2040e78 */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_output_ctl[1][15], 0x19); /* 0x2040ffc */
    tu.OutWord(&mau_reg_map.tcams.vh_data_xbar.tcam_row_halfbyte_mux_ctl[1][15], 0xe); /* 0x2040e7c */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x18); /* 0x2008e58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x97fbe); /* 0x2008e40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xd5a74); /* 0x2008e44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xcd777); /* 0x2008e48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xc7230); /* 0x2008e4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[0].exactmatch_validselect, 0xe0000000); /* 0x2008e5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x30); /* 0x2008e78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9fbfa); /* 0x2008e60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xa72f0); /* 0x2008e64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xae239); /* 0x2008e68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xb765b); /* 0x2008e6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x2008e7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x34); /* 0x2009e58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x962ba); /* 0x2009e40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xdd390); /* 0x2009e44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xcc6d3); /* 0x2009e48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xff6fe); /* 0x2009e4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x2009e5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x31); /* 0x2009e78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xc7a79); /* 0x2009e60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xac656); /* 0x2009e64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xa779a); /* 0x2009e68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x85f7f); /* 0x2009e6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x2009e7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x34); /* 0x200ae58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xa5e59); /* 0x200ae40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xb7f70); /* 0x200ae44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0x9e23d); /* 0x200ae48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xaf35e); /* 0x200ae4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x200ae5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x36); /* 0x200ae78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xc66b2); /* 0x200ae60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0x8c2fb); /* 0x200ae64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xb77fe); /* 0x200ae68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xe4f54); /* 0x200ae6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x200ae7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x36); /* 0x200be58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x9f3d5); /* 0x200be40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0x8ee97); /* 0x200be44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xc7e19); /* 0x200be48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xeda5a); /* 0x200be4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x200be5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x34); /* 0x200be78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x97b31); /* 0x200be60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xb73bf); /* 0x200be64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xae277); /* 0x200be68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x8535b); /* 0x200be6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x200be7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x35); /* 0x200ce58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xdd272); /* 0x200ce40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xec2fa); /* 0x200ce44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xfe3d6); /* 0x200ce48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x8e795); /* 0x200ce4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x200ce5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x33); /* 0x200ce78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xa4e51); /* 0x200ce60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xc5ed5); /* 0x200ce64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xe6f59); /* 0x200ce68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x87fdd); /* 0x200ce6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x200ce7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x36); /* 0x200de58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x95a35); /* 0x200de40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xa63ba); /* 0x200de44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xff3db); /* 0x200de48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x9c2f9); /* 0x200de4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x200de5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x36); /* 0x200de78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xa4e51); /* 0x200de60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xc5ed5); /* 0x200de64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xe6f59); /* 0x200de68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x87fdd); /* 0x200de6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x200de7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x34); /* 0x200ee58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xe6717); /* 0x200ee40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xff612); /* 0x200ee44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xf6a7b); /* 0x200ee48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x8d2d5); /* 0x200ee4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x200ee5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x33); /* 0x200ee78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xb439b); /* 0x200ee60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0x97e74); /* 0x200ee64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xf5f15); /* 0x200ee68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xd7731); /* 0x200ee6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x200ee7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_ctl, 0x31); /* 0x200fe58 */
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0xa4e51); /* 0x200fe40 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0xc5ed5); /* 0x200fe44 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0xe6f59); /* 0x200fe48 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0x87fdd); /* 0x200fe4c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[0].exactmatch_validselect, 0x0); /* 0x200fe5c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_ctl, 0x31); /* 0x200fe78 */
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0], 0x87ab3); /* 0x200fe60 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1], 0x972ff); /* 0x200fe64 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[2], 0x8e3b6); /* 0x200fe68 */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_row_vh_xbar_byteswizzle_ctl[3], 0xde69a); /* 0x200fe6c */ // REMOVED EMVH070915
//    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_xbar[1].exactmatch_validselect, 0x0); /* 0x200fe7c */ // REMOVED EMVH070915
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0x8); /* 0x2008f90 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0x8); /* 0x2008f94 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_mem_hashadr_xbar_ctl[0], 0x10); /* 0x2008fc0 */
    tu.OutWord(&mau_reg_map.rams.array.row[0].vh_adr_xbar.exactmatch_bank_enable[0], 0x1000000); /* 0x2008f00 */
    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xc); /* 0x2009f90 */
    tu.OutWord(&mau_reg_map.rams.array.row[1].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0x9); /* 0x2009f94 */
    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xc); /* 0x200af90 */
    tu.OutWord(&mau_reg_map.rams.array.row[2].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0xe); /* 0x200af94 */
    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xe); /* 0x200bf90 */
    tu.OutWord(&mau_reg_map.rams.array.row[3].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0xc); /* 0x200bf94 */
    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xd); /* 0x200cf90 */
    tu.OutWord(&mau_reg_map.rams.array.row[4].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0xb); /* 0x200cf94 */
    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xe); /* 0x200df90 */
    tu.OutWord(&mau_reg_map.rams.array.row[5].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0xe); /* 0x200df94 */
    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0xc); /* 0x200ef90 */
    tu.OutWord(&mau_reg_map.rams.array.row[6].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0xb); /* 0x200ef94 */
    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[0], 0x9); /* 0x200ff90 */
    tu.OutWord(&mau_reg_map.rams.array.row[7].vh_adr_xbar.exactmatch_row_hashadr_xbar_ctl[1], 0x9); /* 0x200ff94 */
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x8); // ADDED EMVH070915 version valid
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x15); // ADDED EMVH070915
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
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x14); // ADDED EMVH070915
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
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[1].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[2].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[3].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[4].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x11); // ADDED EMVH070915
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
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[5].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][0][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][1][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][2][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][3][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][4][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][5][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][6][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][7][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][8][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][9][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][10][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][11][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][12][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][13][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][14][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[0][15][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[6].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1a); // ADDED EMVH070915
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
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][0], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][1], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][2], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][3], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][4], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][5], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][6], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][0][7], 0x13); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][0], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][1], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][2], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][3], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][4], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][5], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][6], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][1][7], 0x15); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][0], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][1], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][2], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][3], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][4], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][5], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][6], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][2][7], 0x1e); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][0], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][1], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][2], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][3], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][4], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][5], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][6], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][3][7], 0x10); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][0], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][1], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][2], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][3], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][4], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][5], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][6], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][4][7], 0x1f); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][0], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][1], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][2], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][3], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][4], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][5], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][6], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][5][7], 0x17); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][0], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][1], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][2], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][3], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][4], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][5], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][6], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][6][7], 0x1c); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][0], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][1], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][2], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][3], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][4], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][5], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][6], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][7][7], 0x12); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][0], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][1], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][2], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][3], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][4], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][5], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][6], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][8][7], 0x16); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][0], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][1], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][2], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][3], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][4], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][5], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][6], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][9][7], 0x1d); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][0], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][1], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][2], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][3], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][4], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][5], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][6], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][10][7], 0x18); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][0], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][1], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][2], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][3], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][4], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][5], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][6], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][11][7], 0x11); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][0], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][1], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][2], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][3], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][4], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][5], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][6], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][12][7], 0x1a); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][0], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][1], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][2], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][3], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][4], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][5], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][6], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][13][7], 0x14); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][0], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][1], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][2], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][3], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][4], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][5], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][6], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][14][7], 0x19); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][0], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][1], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][2], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][3], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][4], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][5], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][6], 0x1b); // ADDED EMVH070915
  tu.OutWord(&mau_reg_map.rams.array.row[7].exactmatch_row_vh_xbar_byteswizzle_ctl[1][15][7], 0x1b); // ADDED EMVH070915
    tu.IndirectWrite(0x020080008805, 0xd6a337db8ce2884f, 0x1e87e016656fcc9c); /* pipe0.stage0.mau_cfg.mau_array_cfg.sram_ 2_ 2: a=0x20080008805 d0=0xd6a337db8ce2884f d1=0x1e87e016656fcc9c */
  //    tu.OutWord(&mau_reg_map.rams.match.merge.exact_match_delay_config, 0x0); /* 0x201ece8 */ // REMOVED EMDEL070915
    tu.OutWord(&mau_reg_map.rams.match.merge.col[0].row_action_nxtable_bus_drive[0], 0x1); /* 0x201e000 */
    tu.OutWord(&mau_reg_map.rams.match.merge.match_to_logical_table_ixbar_outputmap[0][0], 0x11); /* 0x201fd00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_miss_value[1], 0x0); /* 0x201cdc4 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_miss_value[1], 0x5); /* 0x201ce44 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_miss_value[1], 0x3c0); /* 0x201cec4 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_miss_value[1], 0x86); /* 0x201cf44 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_miss_value[1], 0x125); /* 0x201cfc4 */
    tu.OutWord(&mau_reg_map.rams.match.merge.predication_ctl[0], 0xc260002); /* 0x201ece0 */
    tu.OutWord(&mau_reg_map.rams.match.merge.predication_ctl[1], 0xc260000); /* 0x201ece4 */
    tu.OutWord(&mau_reg_map.rams.match.merge.next_table_format_data[1], 0x5a); /* 0x201ccc4 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_miss_value[1], 0xacedbaba); /* 0x201cd44 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_miss_value[1], 0x2b); /* 0x201cdc4 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_miss_value[1], 0x3dbabe); /* 0x201ce44 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_miss_value[1], 0xbbebe); /* 0x201cec4 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_miss_value[1], 0x101cab); /* 0x201cf44 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_miss_value[1], 0x1dead); /* 0x201cfc4 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_exact_shiftcount[0][2], 0x0); /* 0x201ee08 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_exact_shiftcount[0][2], 0x40); /* 0x201f008 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_exact_shiftcount[0][2], 0xd); /* 0x201f208 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_exact_shiftcount[0][2], 0x14); /* 0x201f408 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_exact_shiftcount[0][2], 0x40); /* 0x201f608 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_exact_shiftcount[0][2], 0x40); /* 0x201f808 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_exact_shiftcount[0][3], 0x15); /* 0x201ee0c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_exact_shiftcount[0][3], 0x40); /* 0x201f00c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_exact_shiftcount[0][3], 0x22); /* 0x201f20c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_exact_shiftcount[0][3], 0x29); /* 0x201f40c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_exact_shiftcount[0][3], 0x40); /* 0x201f60c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_exact_shiftcount[0][3], 0x40); /* 0x201f80c */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_exact_shiftcount[0][4], 0x2a); /* 0x201ee10 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_exact_shiftcount[0][4], 0x40); /* 0x201f010 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_exact_shiftcount[0][4], 0x37); /* 0x201f210 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_exact_shiftcount[0][4], 0x3e); /* 0x201f410 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_exact_shiftcount[0][4], 0x40); /* 0x201f610 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_exact_shiftcount[0][4], 0x40); /* 0x201f810 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_mask[0][0], 0xff); /* 0x201fe00 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_immediate_data_default[0][0], 0x0); /* 0x201fe80 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_mask[0][0], 0x0); /* 0x201ff80 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_action_instruction_adr_default[0][0], 0x0); /* 0x201c000 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_vpn_shiftcount[0][0], 0x0); /* 0x201c080 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_mask[0][0], 0xfff); /* 0x201c100 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_actiondata_adr_default[0][0], 0x0); /* 0x201c180 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_mask[0][0], 0x1); /* 0x201c280 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_stats_adr_default[0][0], 0x0); /* 0x201c300 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_mask[0][0], 0xf); /* 0x201c500 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_meter_adr_default[0][0], 0x0); /* 0x201c580 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_mask[0][0], 0x7fff); /* 0x201c700 */
    tu.OutWord(&mau_reg_map.rams.match.merge.mau_idletime_adr_default[0][0], 0x0); /* 0x201c780 */
    tu.OutWord(&mau_reg_map.rams.map_alu.row[2].adrmux.ram_address_mux_ctl[0][2], 0x40); /* 0x2012908 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.immediate_data_16b_ixbar_ctl[2], 0x20); /* 0x2018788 */
    tu.OutWord(&mau_reg_map.rams.match.adrdist.immediate_data_16b_ixbar_ctl[3], 0x21); /* 0x201878c */
  ActionHvTranslator act_hv_translator;
  //    tu.OutWord(&mau_reg_map.rams.array.row[2].action_hv_xbar.action_hv_xbar_ctl_byte_enable[0], 0x1); /* 0x200a880 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte_enable(2,0,0x1); // ADDED ACHV070915
  //    tu.OutWord(&mau_reg_map.rams.array.row[2].action_hv_xbar.action_hv_xbar_ctl_byte[0], 0x0); /* 0x200a888 */ // REMOVED ACHV070915
  act_hv_translator.ctl_byte(2,0,0x0); // ADDED ACHV070915
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[2].ctl.r_l_action_o_mux_select, 0x10); /* 0x2000080 */
    tu.OutWord(&mau_reg_map.rams.array.switchbox.row[2].ctl.r_action_o_mux_select, 0x1); /* 0x2000094 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[3][0], RM_B4_32(0x538e883)); /* 0x207c180 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[4][0], RM_B4_32(0x23a6837d)); /* 0x207c200 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[5][0], RM_B4_32(0x10dce89d)); /* 0x207c280 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[7][0], RM_B4_32(0x3587e08b)); /* 0x207c380 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[9][0], RM_B4_32(0x778c88e)); /* 0x207c480 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[10][0], RM_B4_32(0x3a7980a8)); /* 0x207c500 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[11][0], RM_B4_32(0xac7e30c)); /* 0x207c580 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[12][0], RM_B4_32(0x3070e2d8)); /* 0x207c600 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[13][0], RM_B4_32(0x2c5b920d)); /* 0x207c680 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[14][0], RM_B4_32(0x3daa6883)); /* 0x207c700 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[16][0], RM_B4_32(0x336e91b6)); /* 0x207c800 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[18][0], RM_B4_32(0x2332b895)); /* 0x207c900 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[21][0], RM_B4_32(0x365d8fe0)); /* 0x207ca80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[22][0], RM_B4_32(0x38a8e207)); /* 0x207cb00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[24][0], RM_B4_32(0xabba0e7)); /* 0x207cc00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[25][0], RM_B4_32(0x1478e88a)); /* 0x207cc80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[29][0], RM_B4_32(0xadfe88a)); /* 0x207ce80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[31][0], RM_B4_32(0x1463c899)); /* 0x207cf80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[33][0], RM_B4_32(0xfd18700)); /* 0x207d080 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[34][0], RM_B4_32(0x153c81af)); /* 0x207d100 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[35][0], RM_B4_32(0x182ee065)); /* 0x207d180 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[36][0], RM_B4_32(0x152e4882)); /* 0x207d200 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[37][0], RM_B4_32(0x39479890)); /* 0x207d280 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[38][0], RM_B4_32(0x3bfee1a8)); /* 0x207d300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[39][0], RM_B4_32(0x1bdfe887)); /* 0x207d380 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[40][0], RM_B4_32(0x18aea898)); /* 0x207d400 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[42][0], RM_B4_32(0x188f8f51)); /* 0x207d500 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[43][0], RM_B4_32(0x3b40688b)); /* 0x207d580 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[44][0], RM_B4_32(0x2f9a6882)); /* 0x207d600 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[45][0], RM_B4_32(0x2605c88d)); /* 0x207d680 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[46][0], RM_B4_32(0x39d2e158)); /* 0x207d700 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[48][0], RM_B4_32(0x321883d6)); /* 0x207d800 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[49][0], RM_B4_32(0xaf8e37e)); /* 0x207d880 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[50][0], RM_B4_32(0x3c5e83e3)); /* 0x207d900 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[51][0], RM_B4_32(0x43de881)); /* 0x207d980 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[54][0], RM_B4_32(0x964e88a)); /* 0x207db00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[56][0], RM_B4_32(0x3667c883)); /* 0x207dc00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[59][0], RM_B4_32(0x31a1d37e)); /* 0x207dd80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[61][0], RM_B4_32(0x205de888)); /* 0x207de80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword32[62][0], RM_B4_32(0x9478224)); /* 0x207df00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[1][0], RM_B4_8(0x8a6a13)); /* 0x207e080 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[2][0], RM_B4_8(0x27480f)); /* 0x207e100 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[3][0], RM_B4_8(0xcb8d1d)); /* 0x207e180 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[4][0], RM_B4_8(0x69e802)); /* 0x207e200 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[6][0], RM_B4_8(0xa5e3f1)); /* 0x207e300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[7][0], RM_B4_8(0xb0e813)); /* 0x207e380 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[9][0], RM_B4_8(0x2e280f)); /* 0x207e480 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[10][0], RM_B4_8(0x796a4e)); /* 0x207e500 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[12][0], RM_B4_8(0x0)); /* 0x207e600 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[14][0], RM_B4_8(0xc4ea1a)); /* 0x207e700 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[15][0], RM_B4_8(0xcaea4e)); /* 0x207e780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[16][0], RM_B4_8(0xc4ea75)); /* 0x207e800 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[17][0], RM_B4_8(0x59e0c6)); /* 0x207e880 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[18][0], RM_B4_8(0x39e81d)); /* 0x207e900 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[19][0], RM_B4_8(0xedea35)); /* 0x207e980 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[21][0], RM_B4_8(0x77ea53)); /* 0x207ea80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[23][0], RM_B4_8(0x94a04)); /* 0x207eb80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[25][0], RM_B4_8(0x83e0d1)); /* 0x207ec80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[26][0], RM_B4_8(0x8dea29)); /* 0x207ed00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[28][0], RM_B4_8(0x38e0d1)); /* 0x207ee00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[29][0], RM_B4_8(0xb16810)); /* 0x207ee80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[30][0], RM_B4_8(0x996a7e)); /* 0x207ef00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[31][0], RM_B4_8(0xf1e386)); /* 0x207ef80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[32][0], RM_B4_8(0x506a34)); /* 0x207f000 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[33][0], RM_B4_8(0x556a49)); /* 0x207f080 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[35][0], RM_B4_8(0x26ea05)); /* 0x207f180 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[37][0], RM_B4_8(0xbc2a69)); /* 0x207f280 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[38][0], RM_B4_8(0x8d6a65)); /* 0x207f300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[39][0], RM_B4_8(0xb92a72)); /* 0x207f380 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[41][0], RM_B4_8(0x546a60)); /* 0x207f480 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[44][0], RM_B4_8(0x18ea32)); /* 0x207f600 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[46][0], RM_B4_8(0x74e276)); /* 0x207f700 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[47][0], RM_B4_8(0xf621df)); /* 0x207f780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[48][0], RM_B4_8(0xf6681e)); /* 0x207f800 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[49][0], RM_B4_8(0x562a55)); /* 0x207f880 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[50][0], RM_B4_8(0xeee027)); /* 0x207f900 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[51][0], RM_B4_8(0x3ae2f6)); /* 0x207f980 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[52][0], RM_B4_8(0xdcea3d)); /* 0x207fa00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[53][0], RM_B4_8(0x54ea29)); /* 0x207fa80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[54][0], RM_B4_8(0x8862df)); /* 0x207fb00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[55][0], RM_B4_8(0x3922ff)); /* 0x207fb80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[57][0], RM_B4_8(0x172800)); /* 0x207fc80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[60][0], RM_B4_8(0x9ae397)); /* 0x207fe00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[61][0], RM_B4_8(0x6dea45)); /* 0x207fe80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[62][0], RM_B4_8(0x91e807)); /* 0x207ff00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword8[63][0], RM_B4_8(0xa492ee)); /* 0x207ff80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[0][0], RM_B4_16(0x7c78035)); /* 0x2078000 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[2][0], RM_B4_16(0x5fde19b)); /* 0x2078100 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[3][0], RM_B4_16(0x416e255)); /* 0x2078180 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[4][0], RM_B4_16(0x7f3e133)); /* 0x2078200 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[5][0], RM_B4_16(0x20f230b)); /* 0x2078280 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[6][0], RM_B4_16(0x7734164)); /* 0x2078300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[8][0], RM_B4_16(0x5a0627f)); /* 0x2078400 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[9][0], RM_B4_16(0x15e238c)); /* 0x2078480 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[11][0], RM_B4_16(0x12d23a6)); /* 0x2078580 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[12][0], RM_B4_16(0x79f8b06)); /* 0x2078600 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[13][0], RM_B4_16(0x2dfe09f)); /* 0x2078680 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[15][0], RM_B4_16(0x3fb229c)); /* 0x2078780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[17][0], RM_B4_16(0x35a9255)); /* 0x2078880 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[18][0], RM_B4_16(0x41ae2a3)); /* 0x2078900 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[19][0], RM_B4_16(0x27ae198)); /* 0x2078980 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[20][0], RM_B4_16(0x288a268)); /* 0x2078a00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[21][0], RM_B4_16(0x388867d)); /* 0x2078a80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[24][0], RM_B4_16(0x6c18d2e)); /* 0x2078c00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[26][0], RM_B4_16(0x60ca3c2)); /* 0x2078d00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[27][0], RM_B4_16(0x3f023c6)); /* 0x2078d80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[28][0], RM_B4_16(0x763e05b)); /* 0x2078e00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[29][0], RM_B4_16(0x73d235a)); /* 0x2078e80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[30][0], RM_B4_16(0x4256125)); /* 0x2078f00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[31][0], RM_B4_16(0x15823d3)); /* 0x2078f80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[32][0], RM_B4_16(0x2b6e0af)); /* 0x2079000 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[33][0], RM_B4_16(0x65a23bd)); /* 0x2079080 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[36][0], RM_B4_16(0x36e62a4)); /* 0x2079200 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[37][0], RM_B4_16(0x15ae3a5)); /* 0x2079280 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[38][0], RM_B4_16(0x4e5e0c1)); /* 0x2079300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[41][0], RM_B4_16(0x1832165)); /* 0x2079480 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[43][0], RM_B4_16(0x5ae50d0)); /* 0x2079580 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[45][0], RM_B4_16(0x5be20c1)); /* 0x2079680 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[46][0], RM_B4_16(0x496e3de)); /* 0x2079700 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[47][0], RM_B4_16(0x67ce2f0)); /* 0x2079780 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[48][0], RM_B4_16(0x227a2cd)); /* 0x2079800 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[49][0], RM_B4_16(0x50263d5)); /* 0x2079880 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[50][0], RM_B4_16(0x72282a0)); /* 0x2079900 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[53][0], RM_B4_16(0x1de622e)); /* 0x2079a80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[54][0], RM_B4_16(0x780e0bb)); /* 0x2079b00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[55][0], RM_B4_16(0x37e22e0)); /* 0x2079b80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[58][0], RM_B4_16(0x236e0c1)); /* 0x2079d00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[59][0], RM_B4_16(0xc6c0bd)); /* 0x2079d80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[61][0], RM_B4_16(0x5f32029)); /* 0x2079e80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[62][0], RM_B4_16(0x21362b2)); /* 0x2079f00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[63][0], RM_B4_16(0x7fc62cd)); /* 0x2079f80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[64][0], RM_B4_16(0x619614a)); /* 0x207a000 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[65][0], RM_B4_16(0x66ce115)); /* 0x207a080 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[67][0], RM_B4_16(0x3a920cc)); /* 0x207a180 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[68][0], RM_B4_16(0x39be2eb)); /* 0x207a200 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[69][0], RM_B4_16(0x30e2091)); /* 0x207a280 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[70][0], RM_B4_16(0xe4e1ac)); /* 0x207a300 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[71][0], RM_B4_16(0xa8e3d7)); /* 0x207a380 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[72][0], RM_B4_16(0x2d65130)); /* 0x207a400 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[73][0], RM_B4_16(0x6032129)); /* 0x207a480 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[74][0], RM_B4_16(0x1dfe2e4)); /* 0x207a500 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[75][0], RM_B4_16(0x57f216b)); /* 0x207a580 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[76][0], RM_B4_16(0x7526308)); /* 0x207a600 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[77][0], RM_B4_16(0x5d5e10d)); /* 0x207a680 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[80][0], RM_B4_16(0xe86035)); /* 0x207a800 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[81][0], RM_B4_16(0x1ab8a68)); /* 0x207a880 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[82][0], RM_B4_16(0x5798b1c)); /* 0x207a900 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[84][0], RM_B4_16(0x66de301)); /* 0x207aa00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[85][0], RM_B4_16(0x62122bf)); /* 0x207aa80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[86][0], RM_B4_16(0xc4e3ed)); /* 0x207ab00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[87][0], RM_B4_16(0x615c145)); /* 0x207ab80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[88][0], RM_B4_16(0x70ee2d6)); /* 0x207ac00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[90][0], RM_B4_16(0x31e38b)); /* 0x207ad00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[91][0], RM_B4_16(0x12080d9)); /* 0x207ad80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[92][0], RM_B4_16(0x60d62f7)); /* 0x207ae00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[93][0], RM_B4_16(0x6a88667)); /* 0x207ae80 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[94][0], RM_B4_16(0x61560e4)); /* 0x207af00 */
    tu.OutWord(&mau_reg_map.dp.imem.imem_subword16[95][0], RM_B4_16(0x7cee028)); /* 0x207af80 */
    tu.OutWord(&mau_reg_map.dp.imem_parity_ctl, 0x1); /* 0x206003c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][0], 0x310b8); /* 0x2074000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][1], 0x3a94); /* 0x2074004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][2], 0x72e); /* 0x2074008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][3], 0x3f3f9); /* 0x207400c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][4], 0xd206); /* 0x2074010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][5], 0x2db27); /* 0x2074014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][6], 0x28563); /* 0x2074018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][7], 0x229f7); /* 0x207401c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][8], 0x280c5); /* 0x2074020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][9], 0x3b6d); /* 0x2074024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][10], 0x2778a); /* 0x2074028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][11], 0x3f397); /* 0x207402c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][12], 0x8ee); /* 0x2074030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][13], 0x38ade); /* 0x2074034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][14], 0xdb47); /* 0x2074038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][15], 0x343d1); /* 0x207403c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][16], 0xb8e0); /* 0x2074040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][17], 0x173f3); /* 0x2074044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][18], 0x2c8f8); /* 0x2074048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][19], 0x1b4c3); /* 0x207404c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][20], 0x18878); /* 0x2074050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][21], 0x1a5eb); /* 0x2074054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][22], 0x261a5); /* 0x2074058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][23], 0x3f6a); /* 0x207405c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][24], 0x412); /* 0x2074060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][25], 0x3f4ac); /* 0x2074064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][26], 0x1ba9e); /* 0x2074068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][27], 0xac7f); /* 0x207406c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][28], 0x17df8); /* 0x2074070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][29], 0x23466); /* 0x2074074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][30], 0x29089); /* 0x2074078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][31], 0xe738); /* 0x207407c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][32], 0xe816); /* 0x2074080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][33], 0x3a4c3); /* 0x2074084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][34], 0xb556); /* 0x2074088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][35], 0x44ed); /* 0x207408c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][36], 0x363f5); /* 0x2074090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][37], 0x3cbc4); /* 0x2074094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][38], 0x2fc12); /* 0x2074098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][39], 0x1af02); /* 0x207409c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][40], 0x393b6); /* 0x20740a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][41], 0x351c4); /* 0x20740a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][42], 0x3121c); /* 0x20740a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][43], 0x1ce1e); /* 0x20740ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][44], 0x2fcbd); /* 0x20740b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][45], 0x23739); /* 0x20740b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][46], 0xe6b8); /* 0x20740b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][47], 0x3c2da); /* 0x20740bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][48], 0x24348); /* 0x20740c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][49], 0x2f943); /* 0x20740c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][50], 0xdcd1); /* 0x20740c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[0][51], 0x3d01a); /* 0x20740cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][0], 0x30fbc); /* 0x2074100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][1], 0x3d96e); /* 0x2074104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][2], 0x3517a); /* 0x2074108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][3], 0x14dcf); /* 0x207410c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][4], 0x1dd6c); /* 0x2074110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][5], 0x18a40); /* 0x2074114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][6], 0xc67b); /* 0x2074118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][7], 0x1d206); /* 0x207411c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][8], 0x33390); /* 0x2074120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][9], 0x2f5e3); /* 0x2074124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][10], 0x34f1a); /* 0x2074128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][11], 0x1a3ce); /* 0x207412c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][12], 0x117fc); /* 0x2074130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][13], 0xa0e8); /* 0x2074134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][14], 0x1af1e); /* 0x2074138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][15], 0x97ca); /* 0x207413c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][16], 0x140b0); /* 0x2074140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][17], 0x1ddd8); /* 0x2074144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][18], 0x364c0); /* 0x2074148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][19], 0x3fa84); /* 0x207414c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][20], 0x299fd); /* 0x2074150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][21], 0x16d88); /* 0x2074154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][22], 0x17d55); /* 0x2074158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][23], 0x36c9e); /* 0x207415c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][24], 0x23410); /* 0x2074160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][25], 0x3765b); /* 0x2074164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][26], 0x658a); /* 0x2074168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][27], 0x2b3a); /* 0x207416c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][28], 0x39a2); /* 0x2074170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][29], 0x24b89); /* 0x2074174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][30], 0x25bcf); /* 0x2074178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][31], 0x28c30); /* 0x207417c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][32], 0x1ecfa); /* 0x2074180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][33], 0xa0b4); /* 0x2074184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][34], 0x289a4); /* 0x2074188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][35], 0x231b); /* 0x207418c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][36], 0x29678); /* 0x2074190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][37], 0x2cc3e); /* 0x2074194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][38], 0xcfb); /* 0x2074198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][39], 0x1a93d); /* 0x207419c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][40], 0x26864); /* 0x20741a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][41], 0x2fcd8); /* 0x20741a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][42], 0x26d08); /* 0x20741a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][43], 0x2e7be); /* 0x20741ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][44], 0xbcdd); /* 0x20741b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][45], 0x1130d); /* 0x20741b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][46], 0x21823); /* 0x20741b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][47], 0x3720a); /* 0x20741bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][48], 0x178c7); /* 0x20741c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][49], 0x29f95); /* 0x20741c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][50], 0x8a60); /* 0x20741c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[1][51], 0x36e1); /* 0x20741cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][0], 0x9819); /* 0x2074200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][1], 0x1ac6f); /* 0x2074204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][2], 0x3af96); /* 0x2074208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][3], 0x1c2a0); /* 0x207420c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][4], 0x39c26); /* 0x2074210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][5], 0x10188); /* 0x2074214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][6], 0x16ab); /* 0x2074218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][7], 0x154fb); /* 0x207421c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][8], 0x1de04); /* 0x2074220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][9], 0x3ff21); /* 0x2074224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][10], 0x3864a); /* 0x2074228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][11], 0x15b21); /* 0x207422c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][12], 0x3dc9b); /* 0x2074230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][13], 0x39f95); /* 0x2074234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][14], 0x2f02); /* 0x2074238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][15], 0x124b5); /* 0x207423c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][16], 0x3de88); /* 0x2074240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][17], 0x16206); /* 0x2074244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][18], 0x34244); /* 0x2074248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][19], 0x2e04b); /* 0x207424c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][20], 0x2cbd6); /* 0x2074250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][21], 0x1255e); /* 0x2074254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][22], 0x2e6); /* 0x2074258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][23], 0xad83); /* 0x207425c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][24], 0x1c338); /* 0x2074260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][25], 0x35988); /* 0x2074264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][26], 0x19dcc); /* 0x2074268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][27], 0x14ffc); /* 0x207426c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][28], 0x2c0ee); /* 0x2074270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][29], 0x34b3c); /* 0x2074274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][30], 0x2316a); /* 0x2074278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][31], 0x308c3); /* 0x207427c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][32], 0x305a3); /* 0x2074280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][33], 0x37720); /* 0x2074284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][34], 0x39436); /* 0x2074288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][35], 0x9c14); /* 0x207428c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][36], 0x1ea2e); /* 0x2074290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][37], 0x1adb5); /* 0x2074294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][38], 0x3644d); /* 0x2074298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][39], 0x295ea); /* 0x207429c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][40], 0x2b870); /* 0x20742a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][41], 0x179ce); /* 0x20742a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][42], 0x1afac); /* 0x20742a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][43], 0x3ab9f); /* 0x20742ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][44], 0x15f36); /* 0x20742b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][45], 0x3a3d5); /* 0x20742b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][46], 0x245ff); /* 0x20742b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][47], 0x340c); /* 0x20742bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][48], 0x4464); /* 0x20742c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][49], 0x1777e); /* 0x20742c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][50], 0x1bfd); /* 0x20742c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[2][51], 0x2edde); /* 0x20742cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][0], 0x28be5); /* 0x2074300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][1], 0x23073); /* 0x2074304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][2], 0x3ea39); /* 0x2074308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][3], 0xcefc); /* 0x207430c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][4], 0x2ea5e); /* 0x2074310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][5], 0x39c70); /* 0x2074314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][6], 0xbe37); /* 0x2074318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][7], 0xb12f); /* 0x207431c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][8], 0x1f416); /* 0x2074320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][9], 0x29b38); /* 0x2074324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][10], 0x2e5da); /* 0x2074328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][11], 0x37f65); /* 0x207432c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][12], 0x2ad96); /* 0x2074330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][13], 0x36819); /* 0x2074334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][14], 0x85d2); /* 0x2074338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][15], 0x34bc8); /* 0x207433c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][16], 0x6a80); /* 0x2074340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][17], 0x1a018); /* 0x2074344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][18], 0x21e8a); /* 0x2074348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][19], 0x1415b); /* 0x207434c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][20], 0x1d790); /* 0x2074350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][21], 0x33bcf); /* 0x2074354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][22], 0x13524); /* 0x2074358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][23], 0x318de); /* 0x207435c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][24], 0x2268f); /* 0x2074360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][25], 0x1b2ff); /* 0x2074364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][26], 0x3bf5b); /* 0x2074368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][27], 0x29d08); /* 0x207436c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][28], 0xb782); /* 0x2074370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][29], 0x22d01); /* 0x2074374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][30], 0x3fe60); /* 0x2074378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][31], 0x1c70e); /* 0x207437c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][32], 0x2f825); /* 0x2074380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][33], 0x130f4); /* 0x2074384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][34], 0x21450); /* 0x2074388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][35], 0x3edfb); /* 0x207438c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][36], 0x38954); /* 0x2074390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][37], 0x381a1); /* 0x2074394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][38], 0x3125b); /* 0x2074398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][39], 0x3c6f5); /* 0x207439c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][40], 0x3e768); /* 0x20743a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][41], 0x32b22); /* 0x20743a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][42], 0x2f51d); /* 0x20743a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][43], 0x2af94); /* 0x20743ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][44], 0x3cc94); /* 0x20743b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][45], 0x1adfc); /* 0x20743b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][46], 0x21b91); /* 0x20743b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][47], 0x2eac8); /* 0x20743bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][48], 0x11ab4); /* 0x20743c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][49], 0x353fb); /* 0x20743c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][50], 0x2417c); /* 0x20743c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[3][51], 0x3c7be); /* 0x20743cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][0], 0x2711); /* 0x2074400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][1], 0x3818b); /* 0x2074404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][2], 0x222e7); /* 0x2074408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][3], 0x1e7c1); /* 0x207440c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][4], 0x36436); /* 0x2074410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][5], 0x32cb8); /* 0x2074414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][6], 0x211b2); /* 0x2074418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][7], 0x5c3e); /* 0x207441c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][8], 0x1e132); /* 0x2074420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][9], 0x1a336); /* 0x2074424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][10], 0x1224b); /* 0x2074428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][11], 0x16bc8); /* 0x207442c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][12], 0x2bc6a); /* 0x2074430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][13], 0x34cc9); /* 0x2074434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][14], 0x24368); /* 0x2074438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][15], 0xf747); /* 0x207443c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][16], 0xeaa); /* 0x2074440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][17], 0x3ed0d); /* 0x2074444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][18], 0x12175); /* 0x2074448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][19], 0x19a3); /* 0x207444c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][20], 0xe167); /* 0x2074450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][21], 0x26c06); /* 0x2074454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][22], 0x13662); /* 0x2074458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][23], 0x7269); /* 0x207445c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][24], 0xa734); /* 0x2074460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][25], 0x1f86d); /* 0x2074464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][26], 0x5666); /* 0x2074468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][27], 0x3094b); /* 0x207446c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][28], 0x28c73); /* 0x2074470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][29], 0x135f7); /* 0x2074474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][30], 0x8c06); /* 0x2074478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][31], 0x93d7); /* 0x207447c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][32], 0x3c65b); /* 0x2074480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][33], 0x14f33); /* 0x2074484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][34], 0x1076e); /* 0x2074488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][35], 0x109d2); /* 0x207448c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][36], 0x3d739); /* 0x2074490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][37], 0x18972); /* 0x2074494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][38], 0x140b0); /* 0x2074498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][39], 0x3d75a); /* 0x207449c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][40], 0x25dfd); /* 0x20744a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][41], 0x3b1df); /* 0x20744a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][42], 0x24ef2); /* 0x20744a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][43], 0x34c1e); /* 0x20744ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][44], 0xed93); /* 0x20744b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][45], 0x3ee62); /* 0x20744b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][46], 0x446a); /* 0x20744b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][47], 0x3297a); /* 0x20744bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][48], 0x301c0); /* 0x20744c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][49], 0xbc7c); /* 0x20744c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][50], 0x373aa); /* 0x20744c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[4][51], 0xdf0); /* 0x20744cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][0], 0x2ee83); /* 0x2074500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][1], 0x119ad); /* 0x2074504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][2], 0x3169d); /* 0x2074508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][3], 0x1ab06); /* 0x207450c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][4], 0x345ed); /* 0x2074510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][5], 0x14c67); /* 0x2074514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][6], 0x302b0); /* 0x2074518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][7], 0x3f858); /* 0x207451c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][8], 0xda36); /* 0x2074520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][9], 0xe1f5); /* 0x2074524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][10], 0x28f01); /* 0x2074528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][11], 0x1f61c); /* 0x207452c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][12], 0x27639); /* 0x2074530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][13], 0x10b58); /* 0x2074534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][14], 0x3f765); /* 0x2074538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][15], 0x273cb); /* 0x207453c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][16], 0x9269); /* 0x2074540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][17], 0x351b1); /* 0x2074544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][18], 0x41ef); /* 0x2074548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][19], 0x3038d); /* 0x207454c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][20], 0x1e57a); /* 0x2074550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][21], 0x21a79); /* 0x2074554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][22], 0x1020f); /* 0x2074558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][23], 0x2a364); /* 0x207455c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][24], 0xb202); /* 0x2074560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][25], 0xd1bf); /* 0x2074564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][26], 0xaa53); /* 0x2074568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][27], 0xdf3); /* 0x207456c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][28], 0x15185); /* 0x2074570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][29], 0x1bb6c); /* 0x2074574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][30], 0x9987); /* 0x2074578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][31], 0x393de); /* 0x207457c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][32], 0x30676); /* 0x2074580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][33], 0x29f99); /* 0x2074584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][34], 0x35753); /* 0x2074588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][35], 0x1a9cf); /* 0x207458c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][36], 0x2cb6b); /* 0x2074590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][37], 0x3c1bb); /* 0x2074594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][38], 0x1096f); /* 0x2074598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][39], 0x3f0a7); /* 0x207459c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][40], 0x76ad); /* 0x20745a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][41], 0xe58); /* 0x20745a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][42], 0x1b636); /* 0x20745a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][43], 0x3c332); /* 0x20745ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][44], 0xda56); /* 0x20745b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][45], 0x24efa); /* 0x20745b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][46], 0x15a7c); /* 0x20745b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][47], 0xe024); /* 0x20745bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][48], 0xbcc9); /* 0x20745c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][49], 0x3342d); /* 0x20745c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][50], 0x2b4e2); /* 0x20745c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[5][51], 0x23433); /* 0x20745cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][0], 0x357a0); /* 0x2074600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][1], 0x29fb2); /* 0x2074604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][2], 0x2719b); /* 0x2074608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][3], 0x22ef4); /* 0x207460c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][4], 0x2e1ce); /* 0x2074610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][5], 0xa81c); /* 0x2074614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][6], 0x349bc); /* 0x2074618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][7], 0x16922); /* 0x207461c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][8], 0x3441a); /* 0x2074620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][9], 0x1b6f9); /* 0x2074624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][10], 0x1cd3b); /* 0x2074628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][11], 0x40d9); /* 0x207462c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][12], 0x12e89); /* 0x2074630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][13], 0x36aa1); /* 0x2074634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][14], 0x39b93); /* 0x2074638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][15], 0x24e2a); /* 0x207463c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][16], 0x1f4d2); /* 0x2074640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][17], 0x354e4); /* 0x2074644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][18], 0x2db79); /* 0x2074648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][19], 0x1c122); /* 0x207464c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][20], 0x20429); /* 0x2074650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][21], 0x3ff79); /* 0x2074654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][22], 0x1775); /* 0x2074658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][23], 0x34f97); /* 0x207465c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][24], 0x310c3); /* 0x2074660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][25], 0x8d9a); /* 0x2074664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][26], 0x974d); /* 0x2074668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][27], 0x2d4f7); /* 0x207466c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][28], 0x2a64e); /* 0x2074670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][29], 0x3ee1c); /* 0x2074674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][30], 0xc75c); /* 0x2074678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][31], 0x2e532); /* 0x207467c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][32], 0x34dc); /* 0x2074680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][33], 0x1fb9a); /* 0x2074684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][34], 0x30442); /* 0x2074688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][35], 0x20056); /* 0x207468c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][36], 0x3b15c); /* 0x2074690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][37], 0x3f4e4); /* 0x2074694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][38], 0x18ebc); /* 0x2074698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][39], 0x15907); /* 0x207469c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][40], 0x19c08); /* 0x20746a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][41], 0x39606); /* 0x20746a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][42], 0x260bf); /* 0x20746a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][43], 0x8cc3); /* 0x20746ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][44], 0x2cb3d); /* 0x20746b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][45], 0x98ec); /* 0x20746b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][46], 0x3ab76); /* 0x20746b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][47], 0x1bbc9); /* 0x20746bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][48], 0x3a6d4); /* 0x20746c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][49], 0xed5); /* 0x20746c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][50], 0x1a24b); /* 0x20746c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[6][51], 0x38a7d); /* 0x20746cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][0], 0xa693); /* 0x2074700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][1], 0x1980c); /* 0x2074704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][2], 0x3cc2); /* 0x2074708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][3], 0xd856); /* 0x207470c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][4], 0x3cb4b); /* 0x2074710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][5], 0x240ee); /* 0x2074714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][6], 0x29115); /* 0x2074718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][7], 0x2f81); /* 0x207471c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][8], 0x2bb03); /* 0x2074720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][9], 0xb074); /* 0x2074724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][10], 0x1ab80); /* 0x2074728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][11], 0x22bcf); /* 0x207472c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][12], 0x22b1e); /* 0x2074730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][13], 0x19283); /* 0x2074734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][14], 0x14e0e); /* 0x2074738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][15], 0x1c2c5); /* 0x207473c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][16], 0x289fe); /* 0x2074740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][17], 0x28654); /* 0x2074744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][18], 0x37aaf); /* 0x2074748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][19], 0x267b6); /* 0x207474c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][20], 0x4c6c); /* 0x2074750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][21], 0x2fdc2); /* 0x2074754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][22], 0x6658); /* 0x2074758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][23], 0xe236); /* 0x207475c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][24], 0x18442); /* 0x2074760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][25], 0x33eae); /* 0x2074764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][26], 0x3ceba); /* 0x2074768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][27], 0x8b93); /* 0x207476c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][28], 0x26257); /* 0x2074770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][29], 0x19bcb); /* 0x2074774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][30], 0x11872); /* 0x2074778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][31], 0x3dd41); /* 0x207477c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][32], 0x35f71); /* 0x2074780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][33], 0x3d16e); /* 0x2074784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][34], 0x6757); /* 0x2074788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][35], 0x11dfb); /* 0x207478c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][36], 0x1962d); /* 0x2074790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][37], 0x2bf70); /* 0x2074794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][38], 0x1adf6); /* 0x2074798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][39], 0x2f7bf); /* 0x207479c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][40], 0x35987); /* 0x20747a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][41], 0x1c2e9); /* 0x20747a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][42], 0x27a92); /* 0x20747a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][43], 0x39f38); /* 0x20747ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][44], 0x22fbb); /* 0x20747b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][45], 0x2a414); /* 0x20747b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][46], 0x36059); /* 0x20747b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][47], 0x3f9e2); /* 0x20747bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][48], 0x2e268); /* 0x20747c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][49], 0x323db); /* 0x20747c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][50], 0x2890c); /* 0x20747c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[7][51], 0xe175); /* 0x20747cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][0], 0x1f517); /* 0x2074800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][1], 0xbe76); /* 0x2074804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][2], 0x3ceca); /* 0x2074808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][3], 0x15f11); /* 0x207480c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][4], 0x3db0d); /* 0x2074810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][5], 0x3e3a1); /* 0x2074814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][6], 0x1c830); /* 0x2074818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][7], 0x3b753); /* 0x207481c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][8], 0x17a35); /* 0x2074820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][9], 0x38a80); /* 0x2074824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][10], 0x3007b); /* 0x2074828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][11], 0x4497); /* 0x207482c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][12], 0x265dd); /* 0x2074830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][13], 0x10fdb); /* 0x2074834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][14], 0x1603a); /* 0x2074838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][15], 0x2cb9b); /* 0x207483c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][16], 0x263d7); /* 0x2074840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][17], 0x94db); /* 0x2074844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][18], 0x12468); /* 0x2074848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][19], 0x2e1f); /* 0x207484c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][20], 0x1f1b3); /* 0x2074850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][21], 0x39507); /* 0x2074854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][22], 0x1b71b); /* 0x2074858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][23], 0x2d003); /* 0x207485c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][24], 0x445f); /* 0x2074860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][25], 0x2849f); /* 0x2074864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][26], 0x32994); /* 0x2074868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][27], 0x15688); /* 0x207486c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][28], 0x3eaab); /* 0x2074870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][29], 0x2f861); /* 0x2074874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][30], 0x78e4); /* 0x2074878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][31], 0x26368); /* 0x207487c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][32], 0x313ee); /* 0x2074880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][33], 0x2864e); /* 0x2074884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][34], 0x30eef); /* 0x2074888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][35], 0x22884); /* 0x207488c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][36], 0x29932); /* 0x2074890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][37], 0x3dbdd); /* 0x2074894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][38], 0x10c28); /* 0x2074898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][39], 0x1b2d2); /* 0x207489c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][40], 0x3f04e); /* 0x20748a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][41], 0x20ecb); /* 0x20748a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][42], 0x3145c); /* 0x20748a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][43], 0x155e2); /* 0x20748ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][44], 0xbae7); /* 0x20748b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][45], 0x15c03); /* 0x20748b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][46], 0xd7e9); /* 0x20748b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][47], 0x39c0d); /* 0x20748bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][48], 0x9ccc); /* 0x20748c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][49], 0x362f0); /* 0x20748c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][50], 0x12613); /* 0x20748c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[8][51], 0x1f142); /* 0x20748cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][0], 0x2f508); /* 0x2074900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][1], 0x15214); /* 0x2074904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][2], 0x285c0); /* 0x2074908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][3], 0x14fa1); /* 0x207490c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][4], 0x12f64); /* 0x2074910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][5], 0x38094); /* 0x2074914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][6], 0x18a50); /* 0x2074918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][7], 0x33722); /* 0x207491c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][8], 0x17963); /* 0x2074920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][9], 0x118cb); /* 0x2074924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][10], 0x2eab6); /* 0x2074928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][11], 0x393d1); /* 0x207492c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][12], 0x1dde); /* 0x2074930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][13], 0x18f54); /* 0x2074934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][14], 0x2b2af); /* 0x2074938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][15], 0x18c93); /* 0x207493c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][16], 0x32cda); /* 0x2074940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][17], 0x4559); /* 0x2074944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][18], 0x353ee); /* 0x2074948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][19], 0x3cfd1); /* 0x207494c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][20], 0x29e08); /* 0x2074950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][21], 0x134e); /* 0x2074954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][22], 0x2118e); /* 0x2074958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][23], 0x21c96); /* 0x207495c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][24], 0x3c71d); /* 0x2074960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][25], 0x18311); /* 0x2074964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][26], 0x1d155); /* 0x2074968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][27], 0x1db90); /* 0x207496c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][28], 0x13671); /* 0x2074970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][29], 0x1a989); /* 0x2074974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][30], 0x35172); /* 0x2074978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][31], 0x24f9); /* 0x207497c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][32], 0x36544); /* 0x2074980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][33], 0x1fa95); /* 0x2074984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][34], 0x2e63); /* 0x2074988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][35], 0x25fde); /* 0x207498c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][36], 0x5430); /* 0x2074990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][37], 0x5023); /* 0x2074994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][38], 0x255e9); /* 0x2074998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][39], 0x1fb25); /* 0x207499c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][40], 0x2e4b3); /* 0x20749a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][41], 0x155b6); /* 0x20749a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][42], 0x1363a); /* 0x20749a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][43], 0x2648f); /* 0x20749ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][44], 0x384d); /* 0x20749b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][45], 0xc815); /* 0x20749b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][46], 0x13d88); /* 0x20749b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][47], 0x3e7fb); /* 0x20749bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][48], 0x1d144); /* 0x20749c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][49], 0x137eb); /* 0x20749c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][50], 0x19170); /* 0x20749c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[9][51], 0x3ac22); /* 0x20749cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][0], 0x208ec); /* 0x2074a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][1], 0xf5a7); /* 0x2074a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][2], 0x32faa); /* 0x2074a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][3], 0x179ca); /* 0x2074a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][4], 0xecc9); /* 0x2074a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][5], 0x1a1ed); /* 0x2074a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][6], 0x684b); /* 0x2074a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][7], 0x4f17); /* 0x2074a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][8], 0x2e9a6); /* 0x2074a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][9], 0x2f1ec); /* 0x2074a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][10], 0x1b666); /* 0x2074a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][11], 0x2ad5d); /* 0x2074a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][12], 0x3a8b4); /* 0x2074a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][13], 0x3a8a5); /* 0x2074a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][14], 0x15563); /* 0x2074a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][15], 0x33c57); /* 0x2074a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][16], 0x2f6d7); /* 0x2074a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][17], 0x3f8b2); /* 0x2074a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][18], 0x21e01); /* 0x2074a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][19], 0x2127b); /* 0x2074a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][20], 0xab16); /* 0x2074a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][21], 0x1ea7b); /* 0x2074a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][22], 0x36d10); /* 0x2074a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][23], 0x22b10); /* 0x2074a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][24], 0x31f37); /* 0x2074a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][25], 0x5972); /* 0x2074a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][26], 0x2b49b); /* 0x2074a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][27], 0x3660d); /* 0x2074a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][28], 0x2eb50); /* 0x2074a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][29], 0x6b84); /* 0x2074a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][30], 0x24bf); /* 0x2074a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][31], 0x3de79); /* 0x2074a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][32], 0x3cb31); /* 0x2074a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][33], 0x17049); /* 0x2074a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][34], 0x2cdc6); /* 0x2074a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][35], 0x38baf); /* 0x2074a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][36], 0x225ab); /* 0x2074a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][37], 0x3a17c); /* 0x2074a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][38], 0x2a69b); /* 0x2074a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][39], 0x397b1); /* 0x2074a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][40], 0x15b95); /* 0x2074aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][41], 0x67f7); /* 0x2074aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][42], 0x3110); /* 0x2074aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][43], 0x48c2); /* 0x2074aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][44], 0x3ab17); /* 0x2074ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][45], 0x9c6a); /* 0x2074ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][46], 0x15049); /* 0x2074ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][47], 0x1c834); /* 0x2074abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][48], 0x55dd); /* 0x2074ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][49], 0x1e3d); /* 0x2074ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][50], 0x137e9); /* 0x2074ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[10][51], 0x22f3e); /* 0x2074acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][0], 0x2ca54); /* 0x2074b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][1], 0x2e7f1); /* 0x2074b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][2], 0x2ce53); /* 0x2074b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][3], 0x2612c); /* 0x2074b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][4], 0x22f4e); /* 0x2074b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][5], 0x22e68); /* 0x2074b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][6], 0x9734); /* 0x2074b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][7], 0x35574); /* 0x2074b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][8], 0x27cb6); /* 0x2074b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][9], 0x9c84); /* 0x2074b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][10], 0x126b3); /* 0x2074b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][11], 0xe6cd); /* 0x2074b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][12], 0x3f188); /* 0x2074b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][13], 0x167a6); /* 0x2074b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][14], 0x3977d); /* 0x2074b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][15], 0x4d50); /* 0x2074b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][16], 0x16dd7); /* 0x2074b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][17], 0x388b6); /* 0x2074b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][18], 0x3120d); /* 0x2074b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][19], 0x31170); /* 0x2074b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][20], 0x24d8d); /* 0x2074b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][21], 0x1d627); /* 0x2074b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][22], 0xa544); /* 0x2074b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][23], 0x3d145); /* 0x2074b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][24], 0xa25f); /* 0x2074b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][25], 0x1eeb3); /* 0x2074b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][26], 0x137ec); /* 0x2074b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][27], 0x3a997); /* 0x2074b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][28], 0x680b); /* 0x2074b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][29], 0x25f7); /* 0x2074b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][30], 0x34343); /* 0x2074b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][31], 0x1646); /* 0x2074b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][32], 0x2f47a); /* 0x2074b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][33], 0x956b); /* 0x2074b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][34], 0xfcc3); /* 0x2074b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][35], 0xf22c); /* 0x2074b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][36], 0x1164e); /* 0x2074b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][37], 0x36123); /* 0x2074b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][38], 0x2c5d3); /* 0x2074b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][39], 0x389fa); /* 0x2074b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][40], 0x380da); /* 0x2074ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][41], 0x15d0b); /* 0x2074ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][42], 0x1bceb); /* 0x2074ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][43], 0x3a0e3); /* 0x2074bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][44], 0x10722); /* 0x2074bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][45], 0x153b0); /* 0x2074bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][46], 0x13aed); /* 0x2074bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][47], 0x208a2); /* 0x2074bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][48], 0x36049); /* 0x2074bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][49], 0x21d5); /* 0x2074bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][50], 0x2e1b); /* 0x2074bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[11][51], 0x265ec); /* 0x2074bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][0], 0x2f0fa); /* 0x2074c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][1], 0x1da24); /* 0x2074c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][2], 0xd0ba); /* 0x2074c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][3], 0x2839a); /* 0x2074c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][4], 0x1cd60); /* 0x2074c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][5], 0x9b71); /* 0x2074c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][6], 0x222bf); /* 0x2074c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][7], 0x3833b); /* 0x2074c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][8], 0x4c61); /* 0x2074c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][9], 0x37a2e); /* 0x2074c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][10], 0x7006); /* 0x2074c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][11], 0x3e514); /* 0x2074c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][12], 0x124b4); /* 0x2074c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][13], 0x1ffee); /* 0x2074c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][14], 0x3e8ec); /* 0x2074c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][15], 0xd7b); /* 0x2074c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][16], 0x3aae1); /* 0x2074c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][17], 0x32561); /* 0x2074c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][18], 0x15564); /* 0x2074c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][19], 0x33052); /* 0x2074c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][20], 0x284ac); /* 0x2074c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][21], 0x3a65c); /* 0x2074c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][22], 0x1dc7c); /* 0x2074c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][23], 0xd7fb); /* 0x2074c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][24], 0x1ba50); /* 0x2074c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][25], 0x1f3d4); /* 0x2074c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][26], 0x1e01); /* 0x2074c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][27], 0x107ea); /* 0x2074c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][28], 0xc60f); /* 0x2074c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][29], 0x8c3a); /* 0x2074c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][30], 0x1c764); /* 0x2074c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][31], 0x4786); /* 0x2074c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][32], 0x30f50); /* 0x2074c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][33], 0x1c150); /* 0x2074c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][34], 0x28e8b); /* 0x2074c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][35], 0x21fb8); /* 0x2074c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][36], 0x36634); /* 0x2074c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][37], 0x6373); /* 0x2074c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][38], 0x3d922); /* 0x2074c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][39], 0x3d768); /* 0x2074c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][40], 0x1d5a1); /* 0x2074ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][41], 0x3a9d5); /* 0x2074ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][42], 0x34fd6); /* 0x2074ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][43], 0xf09c); /* 0x2074cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][44], 0x103f8); /* 0x2074cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][45], 0x23ae3); /* 0x2074cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][46], 0x36b3b); /* 0x2074cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][47], 0x34f3d); /* 0x2074cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][48], 0x3f475); /* 0x2074cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][49], 0x3b37a); /* 0x2074cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][50], 0x24257); /* 0x2074cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[12][51], 0xb8fa); /* 0x2074ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][0], 0x34677); /* 0x2074d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][1], 0xdb64); /* 0x2074d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][2], 0x31d44); /* 0x2074d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][3], 0x35746); /* 0x2074d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][4], 0x21f45); /* 0x2074d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][5], 0x3d260); /* 0x2074d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][6], 0x235ef); /* 0x2074d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][7], 0x3d034); /* 0x2074d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][8], 0xaed7); /* 0x2074d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][9], 0x2be76); /* 0x2074d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][10], 0x2f40a); /* 0x2074d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][11], 0x1fe5c); /* 0x2074d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][12], 0xcdf7); /* 0x2074d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][13], 0x6f85); /* 0x2074d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][14], 0x26bf5); /* 0x2074d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][15], 0x10861); /* 0x2074d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][16], 0x3def6); /* 0x2074d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][17], 0x2fc2d); /* 0x2074d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][18], 0x2f0b8); /* 0x2074d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][19], 0x112f4); /* 0x2074d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][20], 0x34e45); /* 0x2074d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][21], 0x2a787); /* 0x2074d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][22], 0x1540d); /* 0x2074d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][23], 0x196a2); /* 0x2074d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][24], 0x7d8e); /* 0x2074d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][25], 0x348f5); /* 0x2074d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][26], 0x209d4); /* 0x2074d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][27], 0x21b22); /* 0x2074d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][28], 0x316d6); /* 0x2074d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][29], 0xa805); /* 0x2074d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][30], 0x29d5e); /* 0x2074d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][31], 0x3525b); /* 0x2074d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][32], 0x5579); /* 0x2074d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][33], 0x2889c); /* 0x2074d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][34], 0x316e0); /* 0x2074d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][35], 0xb94f); /* 0x2074d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][36], 0x21722); /* 0x2074d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][37], 0x109f8); /* 0x2074d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][38], 0x9b8d); /* 0x2074d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][39], 0x3694b); /* 0x2074d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][40], 0xa6f0); /* 0x2074da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][41], 0x36bcc); /* 0x2074da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][42], 0xe50a); /* 0x2074da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][43], 0x1a85); /* 0x2074dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][44], 0x1eb8c); /* 0x2074db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][45], 0x2fc7c); /* 0x2074db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][46], 0x1fdd2); /* 0x2074db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][47], 0x1a9f0); /* 0x2074dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][48], 0x20e65); /* 0x2074dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][49], 0x27f31); /* 0x2074dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][50], 0x3e90b); /* 0x2074dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[13][51], 0x73cb); /* 0x2074dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][0], 0x30a64); /* 0x2074e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][1], 0x39c33); /* 0x2074e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][2], 0x220d4); /* 0x2074e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][3], 0x1598c); /* 0x2074e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][4], 0x3222c); /* 0x2074e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][5], 0x1cc2d); /* 0x2074e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][6], 0x1da83); /* 0x2074e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][7], 0xbb73); /* 0x2074e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][8], 0x20d98); /* 0x2074e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][9], 0x39f77); /* 0x2074e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][10], 0x1b7a5); /* 0x2074e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][11], 0xa012); /* 0x2074e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][12], 0x354ad); /* 0x2074e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][13], 0x121be); /* 0x2074e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][14], 0x2bcdd); /* 0x2074e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][15], 0x1f599); /* 0x2074e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][16], 0x36c76); /* 0x2074e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][17], 0x1b9cc); /* 0x2074e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][18], 0x29e45); /* 0x2074e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][19], 0x3d19); /* 0x2074e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][20], 0x21cb1); /* 0x2074e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][21], 0x155d2); /* 0x2074e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][22], 0x30727); /* 0x2074e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][23], 0x2c520); /* 0x2074e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][24], 0x1446b); /* 0x2074e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][25], 0x74d9); /* 0x2074e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][26], 0x1990e); /* 0x2074e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][27], 0x9a02); /* 0x2074e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][28], 0xd10a); /* 0x2074e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][29], 0x21b72); /* 0x2074e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][30], 0x1211f); /* 0x2074e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][31], 0x3c7b1); /* 0x2074e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][32], 0x1b305); /* 0x2074e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][33], 0x2ee3b); /* 0x2074e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][34], 0x4b1d); /* 0x2074e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][35], 0x37aa4); /* 0x2074e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][36], 0x1293e); /* 0x2074e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][37], 0x2488); /* 0x2074e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][38], 0xd089); /* 0x2074e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][39], 0xc98d); /* 0x2074e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][40], 0x12593); /* 0x2074ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][41], 0x3f101); /* 0x2074ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][42], 0x1ab14); /* 0x2074ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][43], 0x15c2b); /* 0x2074eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][44], 0x6f8c); /* 0x2074eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][45], 0x2fda9); /* 0x2074eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][46], 0x29e1b); /* 0x2074eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][47], 0x2db73); /* 0x2074ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][48], 0x65f0); /* 0x2074ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][49], 0x1490b); /* 0x2074ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][50], 0x3689b); /* 0x2074ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[14][51], 0xeaca); /* 0x2074ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][0], 0x3e64b); /* 0x2074f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][1], 0xf943); /* 0x2074f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][2], 0x31cf6); /* 0x2074f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][3], 0x1e3aa); /* 0x2074f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][4], 0x1d5cd); /* 0x2074f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][5], 0xdbb5); /* 0x2074f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][6], 0x13479); /* 0x2074f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][7], 0x2e10f); /* 0x2074f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][8], 0x29461); /* 0x2074f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][9], 0x24be6); /* 0x2074f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][10], 0x3c5fd); /* 0x2074f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][11], 0x1a943); /* 0x2074f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][12], 0x169b2); /* 0x2074f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][13], 0x25d4); /* 0x2074f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][14], 0x1a9b4); /* 0x2074f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][15], 0x23f53); /* 0x2074f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][16], 0x41d3); /* 0x2074f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][17], 0x3a94); /* 0x2074f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][18], 0x12029); /* 0x2074f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][19], 0x3379d); /* 0x2074f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][20], 0x15663); /* 0x2074f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][21], 0x93e4); /* 0x2074f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][22], 0x60ec); /* 0x2074f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][23], 0x2e801); /* 0x2074f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][24], 0x2f0d); /* 0x2074f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][25], 0x1b20a); /* 0x2074f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][26], 0x2561b); /* 0x2074f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][27], 0x256b9); /* 0x2074f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][28], 0x5b68); /* 0x2074f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][29], 0x2e697); /* 0x2074f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][30], 0x47de); /* 0x2074f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][31], 0x18dc3); /* 0x2074f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][32], 0x33f40); /* 0x2074f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][33], 0x22ae3); /* 0x2074f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][34], 0x37c1c); /* 0x2074f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][35], 0x257bf); /* 0x2074f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][36], 0x34c13); /* 0x2074f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][37], 0x1106e); /* 0x2074f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][38], 0x2a581); /* 0x2074f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][39], 0xbdde); /* 0x2074f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][40], 0x189be); /* 0x2074fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][41], 0x4a22); /* 0x2074fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][42], 0x133d8); /* 0x2074fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][43], 0xef2a); /* 0x2074fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][44], 0x3435e); /* 0x2074fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][45], 0x1f9ce); /* 0x2074fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][46], 0xad0b); /* 0x2074fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][47], 0x3b171); /* 0x2074fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][48], 0x805c); /* 0x2074fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][49], 0xd089); /* 0x2074fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][50], 0x36f66); /* 0x2074fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[15][51], 0x22faf); /* 0x2074fcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][0], 0x14a3a); /* 0x2075000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][1], 0x3685b); /* 0x2075004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][2], 0x3f0c9); /* 0x2075008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][3], 0x1e7ae); /* 0x207500c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][4], 0x3e98); /* 0x2075010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][5], 0xe7c7); /* 0x2075014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][6], 0xefbc); /* 0x2075018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][7], 0x23cd9); /* 0x207501c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][8], 0xf3b0); /* 0x2075020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][9], 0x3c186); /* 0x2075024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][10], 0xfe66); /* 0x2075028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][11], 0x1dda3); /* 0x207502c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][12], 0x3be09); /* 0x2075030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][13], 0x2b303); /* 0x2075034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][14], 0x10267); /* 0x2075038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][15], 0x2c85f); /* 0x207503c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][16], 0x3ff7c); /* 0x2075040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][17], 0x9b4b); /* 0x2075044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][18], 0x33cca); /* 0x2075048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][19], 0x684d); /* 0x207504c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][20], 0x1824d); /* 0x2075050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][21], 0x126e2); /* 0x2075054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][22], 0x3469e); /* 0x2075058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][23], 0x20d3); /* 0x207505c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][24], 0xc54d); /* 0x2075060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][25], 0x2a986); /* 0x2075064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][26], 0xadcf); /* 0x2075068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][27], 0xc151); /* 0x207506c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][28], 0xd22a); /* 0x2075070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][29], 0x1acdd); /* 0x2075074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][30], 0x13c2f); /* 0x2075078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][31], 0x261b3); /* 0x207507c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][32], 0x1c1ff); /* 0x2075080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][33], 0x1ab4a); /* 0x2075084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][34], 0x2690e); /* 0x2075088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][35], 0x3876f); /* 0x207508c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][36], 0x30029); /* 0x2075090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][37], 0x992); /* 0x2075094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][38], 0x2206b); /* 0x2075098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][39], 0x37f3b); /* 0x207509c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][40], 0x8c6f); /* 0x20750a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][41], 0x3dbdd); /* 0x20750a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][42], 0x2f721); /* 0x20750a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][43], 0x37863); /* 0x20750ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][44], 0x2c517); /* 0x20750b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][45], 0xd699); /* 0x20750b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][46], 0x7d9f); /* 0x20750b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][47], 0x130a3); /* 0x20750bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][48], 0x2a212); /* 0x20750c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][49], 0x2d141); /* 0x20750c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][50], 0x226f7); /* 0x20750c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[16][51], 0x3f996); /* 0x20750cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][0], 0xc7ad); /* 0x2075100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][1], 0x25964); /* 0x2075104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][2], 0x3e8e1); /* 0x2075108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][3], 0x3652c); /* 0x207510c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][4], 0x3a9b4); /* 0x2075110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][5], 0xfc5b); /* 0x2075114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][6], 0x2173f); /* 0x2075118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][7], 0x8020); /* 0x207511c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][8], 0x2e5b7); /* 0x2075120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][9], 0x2380a); /* 0x2075124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][10], 0x3b806); /* 0x2075128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][11], 0x9065); /* 0x207512c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][12], 0x1d9ee); /* 0x2075130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][13], 0x2e851); /* 0x2075134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][14], 0xe7e0); /* 0x2075138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][15], 0xa1f2); /* 0x207513c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][16], 0x175dd); /* 0x2075140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][17], 0x35fc8); /* 0x2075144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][18], 0x37268); /* 0x2075148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][19], 0x31294); /* 0x207514c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][20], 0xc816); /* 0x2075150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][21], 0x1f829); /* 0x2075154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][22], 0x3e480); /* 0x2075158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][23], 0x37132); /* 0x207515c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][24], 0x3d430); /* 0x2075160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][25], 0x31ace); /* 0x2075164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][26], 0xb7ed); /* 0x2075168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][27], 0x34b7b); /* 0x207516c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][28], 0x2650a); /* 0x2075170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][29], 0x1d154); /* 0x2075174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][30], 0x297a2); /* 0x2075178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][31], 0x15a19); /* 0x207517c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][32], 0x11e29); /* 0x2075180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][33], 0x24fae); /* 0x2075184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][34], 0xb78f); /* 0x2075188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][35], 0x1102e); /* 0x207518c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][36], 0xbde2); /* 0x2075190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][37], 0xf296); /* 0x2075194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][38], 0x44fd); /* 0x2075198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][39], 0x31f31); /* 0x207519c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][40], 0x2cb9a); /* 0x20751a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][41], 0xf086); /* 0x20751a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][42], 0x21524); /* 0x20751a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][43], 0x391d7); /* 0x20751ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][44], 0x224ce); /* 0x20751b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][45], 0x6ebb); /* 0x20751b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][46], 0x5b8e); /* 0x20751b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][47], 0xea61); /* 0x20751bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][48], 0x29147); /* 0x20751c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][49], 0xaaf9); /* 0x20751c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][50], 0xdc3d); /* 0x20751c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[17][51], 0x3ce72); /* 0x20751cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][0], 0x312a4); /* 0x2075200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][1], 0x120b); /* 0x2075204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][2], 0xe3f1); /* 0x2075208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][3], 0x3a1ce); /* 0x207520c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][4], 0x36985); /* 0x2075210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][5], 0x221e8); /* 0x2075214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][6], 0xce99); /* 0x2075218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][7], 0x226c7); /* 0x207521c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][8], 0x22963); /* 0x2075220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][9], 0x62dd); /* 0x2075224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][10], 0x22f70); /* 0x2075228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][11], 0x3cea); /* 0x207522c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][12], 0x34108); /* 0x2075230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][13], 0x24359); /* 0x2075234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][14], 0x5f6b); /* 0x2075238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][15], 0x216c3); /* 0x207523c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][16], 0x3d77a); /* 0x2075240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][17], 0x31e00); /* 0x2075244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][18], 0xc905); /* 0x2075248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][19], 0x37fb8); /* 0x207524c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][20], 0x10252); /* 0x2075250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][21], 0x20b1f); /* 0x2075254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][22], 0x2122f); /* 0x2075258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][23], 0x222ef); /* 0x207525c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][24], 0x1d5b3); /* 0x2075260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][25], 0xbc94); /* 0x2075264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][26], 0xe859); /* 0x2075268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][27], 0x14259); /* 0x207526c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][28], 0x3a9f5); /* 0x2075270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][29], 0x1a0c4); /* 0x2075274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][30], 0x1ce31); /* 0x2075278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][31], 0xde8); /* 0x207527c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][32], 0xbf54); /* 0x2075280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][33], 0x1c77f); /* 0x2075284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][34], 0x3c617); /* 0x2075288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][35], 0x923d); /* 0x207528c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][36], 0x1f471); /* 0x2075290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][37], 0x36be0); /* 0x2075294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][38], 0x216d7); /* 0x2075298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][39], 0xc40c); /* 0x207529c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][40], 0x2be06); /* 0x20752a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][41], 0x18dce); /* 0x20752a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][42], 0x30763); /* 0x20752a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][43], 0x2a07b); /* 0x20752ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][44], 0x1eee7); /* 0x20752b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][45], 0x14a17); /* 0x20752b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][46], 0x129); /* 0x20752b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][47], 0x3bf1c); /* 0x20752bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][48], 0x34e9b); /* 0x20752c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][49], 0x15e25); /* 0x20752c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][50], 0x24734); /* 0x20752c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[18][51], 0x1b43a); /* 0x20752cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][0], 0x3830a); /* 0x2075300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][1], 0xcef6); /* 0x2075304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][2], 0x3e70c); /* 0x2075308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][3], 0xbd6a); /* 0x207530c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][4], 0x2135d); /* 0x2075310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][5], 0xb574); /* 0x2075314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][6], 0x2caf0); /* 0x2075318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][7], 0x3564e); /* 0x207531c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][8], 0x3ab2c); /* 0x2075320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][9], 0xb26a); /* 0x2075324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][10], 0xbd77); /* 0x2075328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][11], 0x2a03e); /* 0x207532c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][12], 0x332b1); /* 0x2075330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][13], 0x33e45); /* 0x2075334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][14], 0x1b207); /* 0x2075338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][15], 0x146e2); /* 0x207533c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][16], 0x11da3); /* 0x2075340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][17], 0x1e74c); /* 0x2075344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][18], 0x2d8e1); /* 0x2075348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][19], 0x34fd6); /* 0x207534c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][20], 0x16672); /* 0x2075350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][21], 0x2ce5e); /* 0x2075354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][22], 0x25dea); /* 0x2075358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][23], 0x270c3); /* 0x207535c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][24], 0x3948a); /* 0x2075360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][25], 0xf8f3); /* 0x2075364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][26], 0x21f0f); /* 0x2075368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][27], 0x14137); /* 0x207536c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][28], 0x4eed); /* 0x2075370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][29], 0x26861); /* 0x2075374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][30], 0x6112); /* 0x2075378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][31], 0x34c22); /* 0x207537c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][32], 0x702c); /* 0x2075380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][33], 0x2eed4); /* 0x2075384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][34], 0x1b3f5); /* 0x2075388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][35], 0x1ddcb); /* 0x207538c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][36], 0x1dff9); /* 0x2075390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][37], 0x3748e); /* 0x2075394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][38], 0xfc94); /* 0x2075398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][39], 0x2084c); /* 0x207539c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][40], 0x202e6); /* 0x20753a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][41], 0x2306a); /* 0x20753a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][42], 0x2e12); /* 0x20753a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][43], 0x16499); /* 0x20753ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][44], 0x207c1); /* 0x20753b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][45], 0x35189); /* 0x20753b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][46], 0x338d1); /* 0x20753b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][47], 0x36848); /* 0x20753bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][48], 0x3415); /* 0x20753c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][49], 0x17771); /* 0x20753c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][50], 0x986f); /* 0x20753c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[19][51], 0x22540); /* 0x20753cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][0], 0x180b5); /* 0x2075400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][1], 0xf48f); /* 0x2075404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][2], 0x2046c); /* 0x2075408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][3], 0x48e1); /* 0x207540c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][4], 0x65cf); /* 0x2075410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][5], 0x12b07); /* 0x2075414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][6], 0x37236); /* 0x2075418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][7], 0x28dbf); /* 0x207541c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][8], 0x6576); /* 0x2075420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][9], 0x37f67); /* 0x2075424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][10], 0x3d1be); /* 0x2075428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][11], 0x39e49); /* 0x207542c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][12], 0x48cf); /* 0x2075430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][13], 0x117e9); /* 0x2075434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][14], 0x8c1b); /* 0x2075438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][15], 0x5ca8); /* 0x207543c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][16], 0x1fa87); /* 0x2075440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][17], 0x16a44); /* 0x2075444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][18], 0x23f27); /* 0x2075448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][19], 0x30278); /* 0x207544c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][20], 0x38f71); /* 0x2075450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][21], 0x11212); /* 0x2075454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][22], 0x31516); /* 0x2075458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][23], 0x28e3c); /* 0x207545c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][24], 0x36d9f); /* 0x2075460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][25], 0x2ad22); /* 0x2075464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][26], 0x1442); /* 0x2075468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][27], 0x14b7f); /* 0x207546c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][28], 0x2d41e); /* 0x2075470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][29], 0x17feb); /* 0x2075474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][30], 0x30ca0); /* 0x2075478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][31], 0x31ea1); /* 0x207547c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][32], 0x238e3); /* 0x2075480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][33], 0x3eb21); /* 0x2075484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][34], 0xecb2); /* 0x2075488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][35], 0x13c6c); /* 0x207548c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][36], 0x296f); /* 0x2075490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][37], 0x1bd4); /* 0x2075494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][38], 0x381e1); /* 0x2075498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][39], 0xcade); /* 0x207549c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][40], 0x3246); /* 0x20754a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][41], 0x40fb); /* 0x20754a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][42], 0x1cc9); /* 0x20754a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][43], 0x28ddb); /* 0x20754ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][44], 0x15ea3); /* 0x20754b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][45], 0x3c6e); /* 0x20754b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][46], 0x38790); /* 0x20754b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][47], 0x2dd28); /* 0x20754bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][48], 0x2ae0f); /* 0x20754c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][49], 0x1fdb6); /* 0x20754c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][50], 0x3a403); /* 0x20754c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[20][51], 0x2fa39); /* 0x20754cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][0], 0x17df); /* 0x2075500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][1], 0x2dd70); /* 0x2075504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][2], 0xb15c); /* 0x2075508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][3], 0xe4b); /* 0x207550c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][4], 0x3ab3f); /* 0x2075510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][5], 0x10e67); /* 0x2075514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][6], 0x2786b); /* 0x2075518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][7], 0x195a1); /* 0x207551c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][8], 0x22d6a); /* 0x2075520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][9], 0xdb50); /* 0x2075524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][10], 0x14356); /* 0x2075528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][11], 0x3fdff); /* 0x207552c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][12], 0x8cdb); /* 0x2075530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][13], 0x2a32); /* 0x2075534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][14], 0x158dc); /* 0x2075538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][15], 0x2e196); /* 0x207553c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][16], 0x122ae); /* 0x2075540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][17], 0x37079); /* 0x2075544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][18], 0x10ff4); /* 0x2075548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][19], 0x107ca); /* 0x207554c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][20], 0x3dd1); /* 0x2075550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][21], 0x31ad3); /* 0x2075554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][22], 0x3c99d); /* 0x2075558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][23], 0x2bd43); /* 0x207555c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][24], 0x3cf63); /* 0x2075560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][25], 0x19994); /* 0x2075564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][26], 0x154f0); /* 0x2075568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][27], 0x2b516); /* 0x207556c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][28], 0x3191); /* 0x2075570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][29], 0x20d0f); /* 0x2075574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][30], 0x1020c); /* 0x2075578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][31], 0x34389); /* 0x207557c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][32], 0x31a12); /* 0x2075580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][33], 0x2267b); /* 0x2075584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][34], 0x69ca); /* 0x2075588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][35], 0x28cf3); /* 0x207558c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][36], 0x2e7f5); /* 0x2075590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][37], 0x1f8bc); /* 0x2075594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][38], 0x234a8); /* 0x2075598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][39], 0xdcfa); /* 0x207559c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][40], 0x2427); /* 0x20755a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][41], 0x7088); /* 0x20755a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][42], 0x94a4); /* 0x20755a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][43], 0x26dad); /* 0x20755ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][44], 0x2aac9); /* 0x20755b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][45], 0x2ca40); /* 0x20755b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][46], 0x13d23); /* 0x20755b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][47], 0x266f1); /* 0x20755bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][48], 0x3bc13); /* 0x20755c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][49], 0x3196e); /* 0x20755c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][50], 0x27d26); /* 0x20755c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[21][51], 0x1dc81); /* 0x20755cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][0], 0x1bd7c); /* 0x2075600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][1], 0xff92); /* 0x2075604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][2], 0x2db59); /* 0x2075608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][3], 0x25c13); /* 0x207560c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][4], 0xc9db); /* 0x2075610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][5], 0x21743); /* 0x2075614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][6], 0x180af); /* 0x2075618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][7], 0x39189); /* 0x207561c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][8], 0x3ae05); /* 0x2075620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][9], 0x18e44); /* 0x2075624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][10], 0x15d42); /* 0x2075628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][11], 0x10da3); /* 0x207562c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][12], 0x2e542); /* 0x2075630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][13], 0xad05); /* 0x2075634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][14], 0x14ca1); /* 0x2075638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][15], 0x3f9ee); /* 0x207563c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][16], 0x30dc8); /* 0x2075640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][17], 0x1e6bf); /* 0x2075644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][18], 0x120c3); /* 0x2075648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][19], 0x2121c); /* 0x207564c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][20], 0x6c6c); /* 0x2075650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][21], 0x25a19); /* 0x2075654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][22], 0x1e614); /* 0x2075658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][23], 0x16d3d); /* 0x207565c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][24], 0x2d945); /* 0x2075660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][25], 0x26e5c); /* 0x2075664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][26], 0x13c33); /* 0x2075668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][27], 0x3a743); /* 0x207566c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][28], 0x8798); /* 0x2075670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][29], 0x7821); /* 0x2075674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][30], 0x1d50d); /* 0x2075678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][31], 0x20e3b); /* 0x207567c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][32], 0x1403c); /* 0x2075680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][33], 0x24d97); /* 0x2075684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][34], 0x32b4a); /* 0x2075688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][35], 0x2b15d); /* 0x207568c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][36], 0x2c240); /* 0x2075690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][37], 0x322d9); /* 0x2075694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][38], 0x3cd11); /* 0x2075698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][39], 0x3f49f); /* 0x207569c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][40], 0x22ab8); /* 0x20756a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][41], 0x2a885); /* 0x20756a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][42], 0x18924); /* 0x20756a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][43], 0x1c3d3); /* 0x20756ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][44], 0x2f30); /* 0x20756b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][45], 0x2e705); /* 0x20756b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][46], 0x2feab); /* 0x20756b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][47], 0x16ec); /* 0x20756bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][48], 0x8f83); /* 0x20756c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][49], 0xfbf8); /* 0x20756c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][50], 0x5bad); /* 0x20756c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[22][51], 0x37f71); /* 0x20756cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][0], 0x1a4ec); /* 0x2075700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][1], 0x3b8ae); /* 0x2075704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][2], 0x2c93f); /* 0x2075708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][3], 0x28a3b); /* 0x207570c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][4], 0x34bd6); /* 0x2075710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][5], 0xcb74); /* 0x2075714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][6], 0x3c0ea); /* 0x2075718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][7], 0x18d8f); /* 0x207571c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][8], 0x33fec); /* 0x2075720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][9], 0x3fc07); /* 0x2075724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][10], 0x37868); /* 0x2075728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][11], 0x38a33); /* 0x207572c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][12], 0x13a6); /* 0x2075730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][13], 0x26ff8); /* 0x2075734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][14], 0x76bf); /* 0x2075738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][15], 0x276d1); /* 0x207573c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][16], 0x1b93); /* 0x2075740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][17], 0x29a65); /* 0x2075744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][18], 0x20562); /* 0x2075748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][19], 0x9b80); /* 0x207574c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][20], 0x1eca9); /* 0x2075750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][21], 0x121ba); /* 0x2075754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][22], 0x144d5); /* 0x2075758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][23], 0xf1c4); /* 0x207575c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][24], 0x14bc1); /* 0x2075760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][25], 0x12897); /* 0x2075764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][26], 0x2bec9); /* 0x2075768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][27], 0x26735); /* 0x207576c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][28], 0x7bb4); /* 0x2075770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][29], 0x195a); /* 0x2075774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][30], 0x13c8c); /* 0x2075778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][31], 0x56c6); /* 0x207577c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][32], 0x10419); /* 0x2075780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][33], 0xb7f2); /* 0x2075784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][34], 0x23b68); /* 0x2075788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][35], 0x250ec); /* 0x207578c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][36], 0x22339); /* 0x2075790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][37], 0xd1da); /* 0x2075794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][38], 0x1e3d0); /* 0x2075798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][39], 0x27028); /* 0x207579c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][40], 0x2ec8b); /* 0x20757a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][41], 0x20f83); /* 0x20757a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][42], 0xd78e); /* 0x20757a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][43], 0x3517b); /* 0x20757ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][44], 0x2582d); /* 0x20757b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][45], 0x1d42d); /* 0x20757b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][46], 0x36d4a); /* 0x20757b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][47], 0x2b3c8); /* 0x20757bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][48], 0x39595); /* 0x20757c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][49], 0x431a); /* 0x20757c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][50], 0x12c45); /* 0x20757c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[23][51], 0x1e0e); /* 0x20757cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][0], 0x1583c); /* 0x2075800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][1], 0x1d963); /* 0x2075804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][2], 0x35471); /* 0x2075808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][3], 0x1e3ea); /* 0x207580c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][4], 0x373e9); /* 0x2075810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][5], 0xad11); /* 0x2075814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][6], 0x2df3a); /* 0x2075818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][7], 0x1199d); /* 0x207581c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][8], 0x2df77); /* 0x2075820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][9], 0x8708); /* 0x2075824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][10], 0x513a); /* 0x2075828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][11], 0x3114b); /* 0x207582c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][12], 0x2552c); /* 0x2075830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][13], 0x2c57d); /* 0x2075834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][14], 0x20c8d); /* 0x2075838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][15], 0x23fab); /* 0x207583c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][16], 0xd1d9); /* 0x2075840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][17], 0x33dd8); /* 0x2075844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][18], 0x207e0); /* 0x2075848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][19], 0x1ea83); /* 0x207584c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][20], 0x1f60b); /* 0x2075850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][21], 0x5fbd); /* 0x2075854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][22], 0x15469); /* 0x2075858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][23], 0x3f771); /* 0x207585c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][24], 0x2b33f); /* 0x2075860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][25], 0x29005); /* 0x2075864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][26], 0xe832); /* 0x2075868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][27], 0x1040b); /* 0x207586c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][28], 0x5e7b); /* 0x2075870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][29], 0x38eed); /* 0x2075874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][30], 0x2db8d); /* 0x2075878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][31], 0xb60f); /* 0x207587c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][32], 0x25786); /* 0x2075880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][33], 0x1c94d); /* 0x2075884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][34], 0x2370d); /* 0x2075888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][35], 0x3a8cd); /* 0x207588c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][36], 0x20dc6); /* 0x2075890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][37], 0x1dd7b); /* 0x2075894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][38], 0x3dc90); /* 0x2075898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][39], 0x21c61); /* 0x207589c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][40], 0x2c846); /* 0x20758a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][41], 0x38e17); /* 0x20758a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][42], 0x3a2ba); /* 0x20758a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][43], 0xdc76); /* 0x20758ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][44], 0x24b76); /* 0x20758b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][45], 0x7c42); /* 0x20758b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][46], 0x5427); /* 0x20758b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][47], 0x13b6c); /* 0x20758bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][48], 0x114b2); /* 0x20758c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][49], 0x2a785); /* 0x20758c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][50], 0x1594b); /* 0x20758c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[24][51], 0x36526); /* 0x20758cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][0], 0x18b21); /* 0x2075900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][1], 0x20d59); /* 0x2075904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][2], 0x27559); /* 0x2075908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][3], 0x36a83); /* 0x207590c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][4], 0x2d733); /* 0x2075910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][5], 0x39c1a); /* 0x2075914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][6], 0x20c26); /* 0x2075918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][7], 0x21ee9); /* 0x207591c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][8], 0xc0dd); /* 0x2075920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][9], 0x5fe1); /* 0x2075924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][10], 0x16666); /* 0x2075928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][11], 0x1486c); /* 0x207592c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][12], 0x3cf34); /* 0x2075930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][13], 0xa932); /* 0x2075934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][14], 0x4875); /* 0x2075938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][15], 0x1bc07); /* 0x207593c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][16], 0x196f2); /* 0x2075940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][17], 0x132b7); /* 0x2075944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][18], 0x2561); /* 0x2075948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][19], 0xe682); /* 0x207594c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][20], 0x130bf); /* 0x2075950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][21], 0x22eee); /* 0x2075954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][22], 0x10a1d); /* 0x2075958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][23], 0x3d720); /* 0x207595c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][24], 0x32b84); /* 0x2075960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][25], 0x2695); /* 0x2075964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][26], 0x11a53); /* 0x2075968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][27], 0x1260); /* 0x207596c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][28], 0x13816); /* 0x2075970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][29], 0x571e); /* 0x2075974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][30], 0x2d67c); /* 0x2075978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][31], 0x24d19); /* 0x207597c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][32], 0x25d85); /* 0x2075980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][33], 0x7b1d); /* 0x2075984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][34], 0x1e9dc); /* 0x2075988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][35], 0x18eed); /* 0x207598c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][36], 0xd199); /* 0x2075990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][37], 0x3f829); /* 0x2075994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][38], 0x32775); /* 0x2075998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][39], 0x109d9); /* 0x207599c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][40], 0x30d46); /* 0x20759a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][41], 0x493); /* 0x20759a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][42], 0x2beef); /* 0x20759a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][43], 0x361ae); /* 0x20759ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][44], 0x33e4); /* 0x20759b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][45], 0x7e43); /* 0x20759b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][46], 0x26b0e); /* 0x20759b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][47], 0x31980); /* 0x20759bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][48], 0x15e86); /* 0x20759c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][49], 0x1ac06); /* 0x20759c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][50], 0x15bc2); /* 0x20759c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[25][51], 0x7efb); /* 0x20759cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][0], 0x3e1a4); /* 0x2075a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][1], 0x16ac4); /* 0x2075a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][2], 0x12381); /* 0x2075a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][3], 0x79bf); /* 0x2075a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][4], 0x3981a); /* 0x2075a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][5], 0x2f709); /* 0x2075a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][6], 0x2a2f2); /* 0x2075a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][7], 0x1b093); /* 0x2075a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][8], 0x1ddee); /* 0x2075a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][9], 0x3a9cd); /* 0x2075a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][10], 0x37a7a); /* 0x2075a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][11], 0x2025e); /* 0x2075a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][12], 0xfb6a); /* 0x2075a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][13], 0xe979); /* 0x2075a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][14], 0x2b4e9); /* 0x2075a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][15], 0x7748); /* 0x2075a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][16], 0x30ac4); /* 0x2075a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][17], 0x2f6d7); /* 0x2075a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][18], 0x36254); /* 0x2075a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][19], 0x404d); /* 0x2075a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][20], 0x3decd); /* 0x2075a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][21], 0x32161); /* 0x2075a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][22], 0x2ba1a); /* 0x2075a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][23], 0x237eb); /* 0x2075a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][24], 0x38ea6); /* 0x2075a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][25], 0x37d5a); /* 0x2075a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][26], 0x5db); /* 0x2075a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][27], 0x28a4a); /* 0x2075a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][28], 0x1ab0a); /* 0x2075a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][29], 0x3b1dc); /* 0x2075a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][30], 0x37647); /* 0x2075a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][31], 0x2da3d); /* 0x2075a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][32], 0x11a3a); /* 0x2075a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][33], 0x1126f); /* 0x2075a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][34], 0x2592c); /* 0x2075a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][35], 0x30378); /* 0x2075a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][36], 0xe4f); /* 0x2075a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][37], 0x17ac2); /* 0x2075a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][38], 0x1dd14); /* 0x2075a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][39], 0x3e76c); /* 0x2075a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][40], 0x39dd8); /* 0x2075aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][41], 0x28d42); /* 0x2075aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][42], 0x1432f); /* 0x2075aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][43], 0x2b1b6); /* 0x2075aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][44], 0x3849); /* 0x2075ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][45], 0x523f); /* 0x2075ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][46], 0x3b1c); /* 0x2075ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][47], 0x1ec0c); /* 0x2075abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][48], 0x369a8); /* 0x2075ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][49], 0x3cfeb); /* 0x2075ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][50], 0x3a04d); /* 0x2075ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[26][51], 0x334c); /* 0x2075acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][0], 0x27871); /* 0x2075b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][1], 0x2112f); /* 0x2075b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][2], 0x19f64); /* 0x2075b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][3], 0x3f47d); /* 0x2075b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][4], 0x12425); /* 0x2075b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][5], 0xb535); /* 0x2075b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][6], 0x26a4c); /* 0x2075b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][7], 0x33fc9); /* 0x2075b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][8], 0x1799d); /* 0x2075b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][9], 0x2f791); /* 0x2075b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][10], 0x22639); /* 0x2075b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][11], 0x21a86); /* 0x2075b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][12], 0x131cb); /* 0x2075b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][13], 0x2dafb); /* 0x2075b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][14], 0x38c0a); /* 0x2075b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][15], 0x24001); /* 0x2075b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][16], 0x2e99a); /* 0x2075b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][17], 0xae05); /* 0x2075b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][18], 0x27e94); /* 0x2075b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][19], 0x2faab); /* 0x2075b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][20], 0x2846); /* 0x2075b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][21], 0x10454); /* 0x2075b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][22], 0x37dd3); /* 0x2075b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][23], 0x17226); /* 0x2075b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][24], 0x362a1); /* 0x2075b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][25], 0xc224); /* 0x2075b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][26], 0x379a0); /* 0x2075b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][27], 0x138da); /* 0x2075b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][28], 0x21ff5); /* 0x2075b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][29], 0x1d5ca); /* 0x2075b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][30], 0x37d7f); /* 0x2075b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][31], 0x25683); /* 0x2075b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][32], 0x1b381); /* 0x2075b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][33], 0xb686); /* 0x2075b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][34], 0x2be2b); /* 0x2075b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][35], 0x3e08c); /* 0x2075b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][36], 0x1a7f4); /* 0x2075b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][37], 0x20e28); /* 0x2075b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][38], 0x38a73); /* 0x2075b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][39], 0x19c6c); /* 0x2075b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][40], 0x1283a); /* 0x2075ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][41], 0x18e1b); /* 0x2075ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][42], 0x23a41); /* 0x2075ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][43], 0x32a1e); /* 0x2075bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][44], 0x123e8); /* 0x2075bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][45], 0xe8d3); /* 0x2075bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][46], 0x24007); /* 0x2075bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][47], 0x8055); /* 0x2075bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][48], 0x24504); /* 0x2075bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][49], 0x3e95a); /* 0x2075bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][50], 0x2c412); /* 0x2075bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[27][51], 0x25c97); /* 0x2075bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][0], 0x15d8e); /* 0x2075c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][1], 0x3e267); /* 0x2075c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][2], 0x16ba9); /* 0x2075c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][3], 0x1a9a3); /* 0x2075c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][4], 0x1e650); /* 0x2075c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][5], 0x2b450); /* 0x2075c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][6], 0xbb55); /* 0x2075c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][7], 0x10743); /* 0x2075c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][8], 0x20f18); /* 0x2075c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][9], 0x18c4d); /* 0x2075c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][10], 0x39065); /* 0x2075c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][11], 0x3a4e0); /* 0x2075c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][12], 0xcca3); /* 0x2075c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][13], 0x29334); /* 0x2075c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][14], 0x3f3a9); /* 0x2075c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][15], 0x34046); /* 0x2075c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][16], 0x1ffa4); /* 0x2075c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][17], 0x79a4); /* 0x2075c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][18], 0x3d0c); /* 0x2075c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][19], 0x227bf); /* 0x2075c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][20], 0x3aee3); /* 0x2075c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][21], 0x1262); /* 0x2075c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][22], 0x2b546); /* 0x2075c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][23], 0x2d271); /* 0x2075c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][24], 0x25c79); /* 0x2075c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][25], 0x13c57); /* 0x2075c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][26], 0x29c08); /* 0x2075c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][27], 0x3ed3c); /* 0x2075c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][28], 0x2aaa5); /* 0x2075c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][29], 0x2b600); /* 0x2075c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][30], 0xf2cc); /* 0x2075c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][31], 0x302ab); /* 0x2075c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][32], 0x3cea7); /* 0x2075c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][33], 0x291ff); /* 0x2075c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][34], 0xbc2e); /* 0x2075c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][35], 0x33f9f); /* 0x2075c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][36], 0x27068); /* 0x2075c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][37], 0x29d5b); /* 0x2075c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][38], 0x1d27f); /* 0x2075c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][39], 0x28a5d); /* 0x2075c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][40], 0x3a555); /* 0x2075ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][41], 0x2936d); /* 0x2075ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][42], 0x2f675); /* 0x2075ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][43], 0x2a30d); /* 0x2075cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][44], 0x20fb8); /* 0x2075cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][45], 0x2e4bd); /* 0x2075cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][46], 0x24a71); /* 0x2075cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][47], 0xe4be); /* 0x2075cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][48], 0x2c251); /* 0x2075cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][49], 0x2c43c); /* 0x2075cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][50], 0x15a11); /* 0x2075cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[28][51], 0x148c9); /* 0x2075ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][0], 0x7492); /* 0x2075d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][1], 0x27f1b); /* 0x2075d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][2], 0x4b20); /* 0x2075d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][3], 0x17fac); /* 0x2075d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][4], 0x12df8); /* 0x2075d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][5], 0x25a); /* 0x2075d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][6], 0x181b2); /* 0x2075d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][7], 0x1f294); /* 0x2075d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][8], 0x2efba); /* 0x2075d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][9], 0x1a33b); /* 0x2075d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][10], 0x1ebb5); /* 0x2075d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][11], 0xcf70); /* 0x2075d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][12], 0x1e535); /* 0x2075d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][13], 0x3a358); /* 0x2075d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][14], 0x126f4); /* 0x2075d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][15], 0x38b78); /* 0x2075d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][16], 0x632d); /* 0x2075d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][17], 0x36a4a); /* 0x2075d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][18], 0x149b); /* 0x2075d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][19], 0x25f4f); /* 0x2075d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][20], 0x3cbcc); /* 0x2075d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][21], 0x18086); /* 0x2075d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][22], 0x1ad9b); /* 0x2075d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][23], 0x3a737); /* 0x2075d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][24], 0x9df2); /* 0x2075d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][25], 0x3e86c); /* 0x2075d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][26], 0x8b48); /* 0x2075d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][27], 0x28527); /* 0x2075d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][28], 0x99); /* 0x2075d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][29], 0x9e6e); /* 0x2075d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][30], 0x3db90); /* 0x2075d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][31], 0x2a55d); /* 0x2075d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][32], 0x204f0); /* 0x2075d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][33], 0x3b0cb); /* 0x2075d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][34], 0x3f0d7); /* 0x2075d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][35], 0x35431); /* 0x2075d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][36], 0x19174); /* 0x2075d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][37], 0x24ade); /* 0x2075d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][38], 0x2c910); /* 0x2075d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][39], 0x7e4e); /* 0x2075d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][40], 0x3b9e1); /* 0x2075da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][41], 0x34eb); /* 0x2075da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][42], 0x342fc); /* 0x2075da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][43], 0x266c6); /* 0x2075dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][44], 0x22fb7); /* 0x2075db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][45], 0xc919); /* 0x2075db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][46], 0x742d); /* 0x2075db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][47], 0x8971); /* 0x2075dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][48], 0x3acab); /* 0x2075dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][49], 0x239db); /* 0x2075dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][50], 0x20804); /* 0x2075dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[29][51], 0x2597d); /* 0x2075dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][0], 0x33a3d); /* 0x2075e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][1], 0x2d317); /* 0x2075e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][2], 0xd94); /* 0x2075e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][3], 0x2c467); /* 0x2075e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][4], 0x3de96); /* 0x2075e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][5], 0x2522e); /* 0x2075e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][6], 0x261c); /* 0x2075e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][7], 0x22837); /* 0x2075e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][8], 0x17bb); /* 0x2075e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][9], 0x4884); /* 0x2075e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][10], 0x387ce); /* 0x2075e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][11], 0x32dcd); /* 0x2075e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][12], 0x380e9); /* 0x2075e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][13], 0x2283f); /* 0x2075e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][14], 0x32b8f); /* 0x2075e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][15], 0x30621); /* 0x2075e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][16], 0x3ccb3); /* 0x2075e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][17], 0x372d2); /* 0x2075e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][18], 0x2815c); /* 0x2075e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][19], 0x5b1c); /* 0x2075e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][20], 0x1b88d); /* 0x2075e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][21], 0x285cd); /* 0x2075e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][22], 0x20cfd); /* 0x2075e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][23], 0x9543); /* 0x2075e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][24], 0x36d4b); /* 0x2075e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][25], 0x32849); /* 0x2075e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][26], 0x2d513); /* 0x2075e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][27], 0x5acb); /* 0x2075e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][28], 0x3e69f); /* 0x2075e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][29], 0x214d1); /* 0x2075e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][30], 0x3c1e6); /* 0x2075e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][31], 0x215c4); /* 0x2075e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][32], 0x3036a); /* 0x2075e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][33], 0x19f70); /* 0x2075e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][34], 0x12286); /* 0x2075e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][35], 0x3668d); /* 0x2075e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][36], 0x3d909); /* 0x2075e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][37], 0x19e18); /* 0x2075e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][38], 0x1b578); /* 0x2075e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][39], 0x37227); /* 0x2075e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][40], 0x1acbc); /* 0x2075ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][41], 0x38805); /* 0x2075ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][42], 0x2b16b); /* 0x2075ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][43], 0x2cc3a); /* 0x2075eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][44], 0x12c40); /* 0x2075eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][45], 0x2e05b); /* 0x2075eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][46], 0x14f2b); /* 0x2075eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][47], 0x286aa); /* 0x2075ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][48], 0x1c044); /* 0x2075ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][49], 0x1aae7); /* 0x2075ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][50], 0xfcb3); /* 0x2075ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[30][51], 0x3cc95); /* 0x2075ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][0], 0x2918c); /* 0x2075f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][1], 0x10e56); /* 0x2075f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][2], 0x3b54a); /* 0x2075f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][3], 0x1ed75); /* 0x2075f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][4], 0x19c9c); /* 0x2075f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][5], 0x31666); /* 0x2075f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][6], 0x182c5); /* 0x2075f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][7], 0x3b006); /* 0x2075f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][8], 0xf216); /* 0x2075f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][9], 0xd44c); /* 0x2075f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][10], 0x3c042); /* 0x2075f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][11], 0x1f42e); /* 0x2075f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][12], 0x2e822); /* 0x2075f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][13], 0x19938); /* 0x2075f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][14], 0x359a6); /* 0x2075f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][15], 0x4dbb); /* 0x2075f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][16], 0xb9a8); /* 0x2075f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][17], 0x14af8); /* 0x2075f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][18], 0x1f008); /* 0x2075f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][19], 0x32239); /* 0x2075f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][20], 0x31c52); /* 0x2075f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][21], 0x34562); /* 0x2075f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][22], 0x1cb5a); /* 0x2075f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][23], 0x29dc6); /* 0x2075f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][24], 0x21c2e); /* 0x2075f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][25], 0x978e); /* 0x2075f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][26], 0x18a0a); /* 0x2075f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][27], 0x33ca7); /* 0x2075f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][28], 0x39efb); /* 0x2075f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][29], 0x303bb); /* 0x2075f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][30], 0xf512); /* 0x2075f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][31], 0x372b8); /* 0x2075f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][32], 0x33a0b); /* 0x2075f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][33], 0x2dce2); /* 0x2075f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][34], 0x2f3a2); /* 0x2075f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][35], 0x5dd0); /* 0x2075f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][36], 0x1cb3e); /* 0x2075f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][37], 0x35d0c); /* 0x2075f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][38], 0x51ac); /* 0x2075f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][39], 0x11d95); /* 0x2075f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][40], 0x396e0); /* 0x2075fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][41], 0x37433); /* 0x2075fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][42], 0x3dd3c); /* 0x2075fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][43], 0x3e6a8); /* 0x2075fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][44], 0x103c); /* 0x2075fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][45], 0x3b0e6); /* 0x2075fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][46], 0x5845); /* 0x2075fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][47], 0x1f0dd); /* 0x2075fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][48], 0x2d3cc); /* 0x2075fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][49], 0x2e8f6); /* 0x2075fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][50], 0x29511); /* 0x2075fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[31][51], 0xd50); /* 0x2075fcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][0], 0x1ed89); /* 0x2076000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][1], 0x3f708); /* 0x2076004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][2], 0x284c3); /* 0x2076008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][3], 0x3698); /* 0x207600c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][4], 0x347cd); /* 0x2076010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][5], 0x8641); /* 0x2076014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][6], 0x7e1d); /* 0x2076018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][7], 0x75a1); /* 0x207601c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][8], 0x2fdd0); /* 0x2076020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][9], 0x15b53); /* 0x2076024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][10], 0x31f8b); /* 0x2076028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][11], 0x3c15); /* 0x207602c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][12], 0x27136); /* 0x2076030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][13], 0x368e5); /* 0x2076034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][14], 0x37381); /* 0x2076038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][15], 0x28219); /* 0x207603c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][16], 0x3cf6a); /* 0x2076040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][17], 0x1c279); /* 0x2076044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][18], 0x23fe9); /* 0x2076048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][19], 0x11894); /* 0x207604c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][20], 0x2d406); /* 0x2076050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][21], 0x35769); /* 0x2076054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][22], 0x22906); /* 0x2076058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][23], 0xf538); /* 0x207605c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][24], 0x38323); /* 0x2076060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][25], 0x12c05); /* 0x2076064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][26], 0x2f924); /* 0x2076068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][27], 0xcaae); /* 0x207606c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][28], 0x29cd9); /* 0x2076070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][29], 0x36dcb); /* 0x2076074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][30], 0x13e69); /* 0x2076078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][31], 0x91da); /* 0x207607c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][32], 0x374a0); /* 0x2076080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][33], 0x1a9c0); /* 0x2076084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][34], 0x15764); /* 0x2076088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][35], 0xf9c3); /* 0x207608c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][36], 0x2d776); /* 0x2076090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][37], 0x1c69b); /* 0x2076094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][38], 0x3b253); /* 0x2076098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][39], 0x11db2); /* 0x207609c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][40], 0x178c); /* 0x20760a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][41], 0x97a5); /* 0x20760a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][42], 0xa08a); /* 0x20760a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][43], 0x1f64); /* 0x20760ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][44], 0x3488e); /* 0x20760b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][45], 0x36b3b); /* 0x20760b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][46], 0x12de7); /* 0x20760b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][47], 0x3d7eb); /* 0x20760bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][48], 0x10e8c); /* 0x20760c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][49], 0x17a3d); /* 0x20760c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][50], 0xe767); /* 0x20760c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[32][51], 0x30d52); /* 0x20760cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][0], 0x1de89); /* 0x2076100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][1], 0x1962b); /* 0x2076104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][2], 0xd345); /* 0x2076108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][3], 0x6e69); /* 0x207610c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][4], 0x29bd2); /* 0x2076110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][5], 0x21785); /* 0x2076114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][6], 0x10bf); /* 0x2076118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][7], 0x1a42); /* 0x207611c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][8], 0x3e1af); /* 0x2076120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][9], 0x35ec4); /* 0x2076124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][10], 0x13e40); /* 0x2076128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][11], 0x10535); /* 0x207612c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][12], 0x13d89); /* 0x2076130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][13], 0x2209d); /* 0x2076134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][14], 0x1b00); /* 0x2076138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][15], 0x3f427); /* 0x207613c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][16], 0x6127); /* 0x2076140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][17], 0x22bd5); /* 0x2076144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][18], 0x17b60); /* 0x2076148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][19], 0x3bcc3); /* 0x207614c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][20], 0x15e8c); /* 0x2076150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][21], 0xe9f2); /* 0x2076154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][22], 0x32ca6); /* 0x2076158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][23], 0x2e4bf); /* 0x207615c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][24], 0xdfbf); /* 0x2076160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][25], 0x2573f); /* 0x2076164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][26], 0x64cd); /* 0x2076168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][27], 0x38743); /* 0x207616c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][28], 0x3e155); /* 0x2076170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][29], 0x7ef); /* 0x2076174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][30], 0x2f1af); /* 0x2076178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][31], 0xc7e0); /* 0x207617c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][32], 0x20a7b); /* 0x2076180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][33], 0xd727); /* 0x2076184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][34], 0x178d2); /* 0x2076188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][35], 0x1d273); /* 0x207618c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][36], 0x25511); /* 0x2076190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][37], 0x20a0c); /* 0x2076194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][38], 0x26e0b); /* 0x2076198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][39], 0x3ec6a); /* 0x207619c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][40], 0x30d54); /* 0x20761a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][41], 0x2680d); /* 0x20761a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][42], 0xf199); /* 0x20761a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][43], 0x2af85); /* 0x20761ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][44], 0x26550); /* 0x20761b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][45], 0x7d55); /* 0x20761b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][46], 0xfed4); /* 0x20761b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][47], 0x3c9e4); /* 0x20761bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][48], 0x2f517); /* 0x20761c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][49], 0x2c9bc); /* 0x20761c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][50], 0x297ba); /* 0x20761c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[33][51], 0x2ce65); /* 0x20761cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][0], 0x14b0b); /* 0x2076200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][1], 0x3d683); /* 0x2076204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][2], 0x2fb89); /* 0x2076208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][3], 0x8651); /* 0x207620c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][4], 0x60e4); /* 0x2076210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][5], 0x2c437); /* 0x2076214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][6], 0x26dab); /* 0x2076218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][7], 0x3a2ba); /* 0x207621c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][8], 0x162c5); /* 0x2076220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][9], 0x26695); /* 0x2076224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][10], 0x35d20); /* 0x2076228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][11], 0x83e2); /* 0x207622c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][12], 0x4301); /* 0x2076230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][13], 0x2781c); /* 0x2076234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][14], 0x17867); /* 0x2076238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][15], 0x76c7); /* 0x207623c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][16], 0x3932); /* 0x2076240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][17], 0x4de4); /* 0x2076244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][18], 0x183e1); /* 0x2076248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][19], 0x29d05); /* 0x207624c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][20], 0x10b3e); /* 0x2076250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][21], 0x44d0); /* 0x2076254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][22], 0x7b9d); /* 0x2076258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][23], 0x4918); /* 0x207625c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][24], 0x220c); /* 0x2076260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][25], 0x38ffa); /* 0x2076264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][26], 0x3c8d9); /* 0x2076268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][27], 0x30c33); /* 0x207626c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][28], 0x1fba6); /* 0x2076270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][29], 0x24ada); /* 0x2076274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][30], 0x9137); /* 0x2076278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][31], 0x3625a); /* 0x207627c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][32], 0x2646a); /* 0x2076280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][33], 0x3f74f); /* 0x2076284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][34], 0x2150f); /* 0x2076288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][35], 0x17eb1); /* 0x207628c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][36], 0x30e11); /* 0x2076290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][37], 0xfa03); /* 0x2076294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][38], 0x19040); /* 0x2076298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][39], 0x33e6a); /* 0x207629c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][40], 0x1e85f); /* 0x20762a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][41], 0x73dd); /* 0x20762a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][42], 0x37377); /* 0x20762a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][43], 0x34988); /* 0x20762ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][44], 0x61f4); /* 0x20762b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][45], 0xedce); /* 0x20762b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][46], 0x1e841); /* 0x20762b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][47], 0xa94d); /* 0x20762bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][48], 0x23f3f); /* 0x20762c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][49], 0x10903); /* 0x20762c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][50], 0x8e0); /* 0x20762c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[34][51], 0x2f74c); /* 0x20762cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][0], 0x112f0); /* 0x2076300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][1], 0x16f36); /* 0x2076304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][2], 0x2eb72); /* 0x2076308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][3], 0x358a4); /* 0x207630c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][4], 0x17794); /* 0x2076310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][5], 0x11f2); /* 0x2076314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][6], 0x37ad1); /* 0x2076318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][7], 0x150a5); /* 0x207631c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][8], 0x26c0f); /* 0x2076320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][9], 0x2d248); /* 0x2076324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][10], 0x24ffc); /* 0x2076328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][11], 0x1794a); /* 0x207632c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][12], 0xc34f); /* 0x2076330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][13], 0xa5b9); /* 0x2076334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][14], 0x3393d); /* 0x2076338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][15], 0xbd53); /* 0x207633c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][16], 0xb8f); /* 0x2076340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][17], 0x199aa); /* 0x2076344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][18], 0x1aa18); /* 0x2076348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][19], 0x3b24e); /* 0x207634c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][20], 0x19eb5); /* 0x2076350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][21], 0xe15e); /* 0x2076354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][22], 0x32962); /* 0x2076358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][23], 0x25392); /* 0x207635c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][24], 0x9b8f); /* 0x2076360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][25], 0x24ff6); /* 0x2076364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][26], 0x3d451); /* 0x2076368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][27], 0x1cf43); /* 0x207636c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][28], 0x235b); /* 0x2076370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][29], 0x335a); /* 0x2076374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][30], 0x316b0); /* 0x2076378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][31], 0x2cc37); /* 0x207637c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][32], 0xdfc7); /* 0x2076380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][33], 0x3710b); /* 0x2076384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][34], 0x19ed5); /* 0x2076388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][35], 0x36975); /* 0x207638c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][36], 0x1842); /* 0x2076390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][37], 0x3a1f1); /* 0x2076394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][38], 0x19455); /* 0x2076398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][39], 0x261c0); /* 0x207639c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][40], 0x1e493); /* 0x20763a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][41], 0x154af); /* 0x20763a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][42], 0x36760); /* 0x20763a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][43], 0xf3b); /* 0x20763ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][44], 0x2dc83); /* 0x20763b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][45], 0x12765); /* 0x20763b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][46], 0x135f3); /* 0x20763b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][47], 0xbac); /* 0x20763bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][48], 0x343fb); /* 0x20763c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][49], 0x43cc); /* 0x20763c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][50], 0x10587); /* 0x20763c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[35][51], 0x45bf); /* 0x20763cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][0], 0x3773b); /* 0x2076400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][1], 0x28a0d); /* 0x2076404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][2], 0xe399); /* 0x2076408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][3], 0x19c0b); /* 0x207640c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][4], 0x36aee); /* 0x2076410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][5], 0xf7ab); /* 0x2076414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][6], 0xb96c); /* 0x2076418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][7], 0x31fdc); /* 0x207641c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][8], 0x11117); /* 0x2076420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][9], 0x2a8a4); /* 0x2076424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][10], 0x43ae); /* 0x2076428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][11], 0x2dce); /* 0x207642c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][12], 0x2b8dc); /* 0x2076430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][13], 0x3a4f4); /* 0x2076434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][14], 0x10856); /* 0x2076438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][15], 0x6b80); /* 0x207643c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][16], 0x385e8); /* 0x2076440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][17], 0x279ee); /* 0x2076444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][18], 0x102d1); /* 0x2076448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][19], 0x27b8c); /* 0x207644c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][20], 0xad38); /* 0x2076450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][21], 0x2c222); /* 0x2076454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][22], 0x2edd4); /* 0x2076458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][23], 0x205a7); /* 0x207645c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][24], 0x285b5); /* 0x2076460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][25], 0x3f02c); /* 0x2076464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][26], 0xa13c); /* 0x2076468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][27], 0x1e2a3); /* 0x207646c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][28], 0x38191); /* 0x2076470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][29], 0x1f800); /* 0x2076474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][30], 0xfdfa); /* 0x2076478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][31], 0xf893); /* 0x207647c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][32], 0x391f1); /* 0x2076480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][33], 0x3c7aa); /* 0x2076484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][34], 0x15622); /* 0x2076488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][35], 0x206ab); /* 0x207648c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][36], 0x6af2); /* 0x2076490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][37], 0x331ef); /* 0x2076494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][38], 0xef3f); /* 0x2076498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][39], 0x14274); /* 0x207649c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][40], 0x7dea); /* 0x20764a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][41], 0x58ae); /* 0x20764a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][42], 0x186cc); /* 0x20764a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][43], 0x807e); /* 0x20764ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][44], 0xd984); /* 0x20764b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][45], 0x2d821); /* 0x20764b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][46], 0x189bd); /* 0x20764b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][47], 0x13b1e); /* 0x20764bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][48], 0x14db2); /* 0x20764c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][49], 0x2c204); /* 0x20764c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][50], 0x2b038); /* 0x20764c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[36][51], 0x29720); /* 0x20764cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][0], 0x3681a); /* 0x2076500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][1], 0x3108e); /* 0x2076504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][2], 0x2d56d); /* 0x2076508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][3], 0x2f588); /* 0x207650c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][4], 0x2a5fb); /* 0x2076510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][5], 0x2fc1f); /* 0x2076514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][6], 0x3a4ab); /* 0x2076518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][7], 0x33a10); /* 0x207651c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][8], 0x2f4e7); /* 0x2076520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][9], 0x395e4); /* 0x2076524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][10], 0x188b5); /* 0x2076528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][11], 0x39a30); /* 0x207652c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][12], 0xf989); /* 0x2076530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][13], 0x1d216); /* 0x2076534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][14], 0x227b7); /* 0x2076538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][15], 0x58c7); /* 0x207653c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][16], 0x2f6ed); /* 0x2076540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][17], 0x21509); /* 0x2076544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][18], 0x17ae4); /* 0x2076548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][19], 0x2aa4e); /* 0x207654c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][20], 0x351d9); /* 0x2076550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][21], 0x2734f); /* 0x2076554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][22], 0x1fda7); /* 0x2076558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][23], 0xa9f6); /* 0x207655c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][24], 0x860); /* 0x2076560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][25], 0x279d9); /* 0x2076564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][26], 0xb983); /* 0x2076568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][27], 0x2d11e); /* 0x207656c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][28], 0x37f13); /* 0x2076570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][29], 0x394c5); /* 0x2076574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][30], 0x24910); /* 0x2076578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][31], 0x1771e); /* 0x207657c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][32], 0x29136); /* 0x2076580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][33], 0x32d34); /* 0x2076584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][34], 0x325f2); /* 0x2076588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][35], 0x30ecb); /* 0x207658c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][36], 0x2773a); /* 0x2076590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][37], 0x336c4); /* 0x2076594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][38], 0x11964); /* 0x2076598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][39], 0x4636); /* 0x207659c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][40], 0x3ec); /* 0x20765a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][41], 0x1c6f9); /* 0x20765a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][42], 0x2ebb1); /* 0x20765a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][43], 0x4670); /* 0x20765ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][44], 0xb581); /* 0x20765b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][45], 0x2b423); /* 0x20765b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][46], 0x3280e); /* 0x20765b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][47], 0x194d4); /* 0x20765bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][48], 0x1f5cd); /* 0x20765c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][49], 0x187b3); /* 0x20765c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][50], 0x33858); /* 0x20765c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[37][51], 0x1b708); /* 0x20765cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][0], 0x1d122); /* 0x2076600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][1], 0x24eac); /* 0x2076604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][2], 0x11b07); /* 0x2076608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][3], 0x12379); /* 0x207660c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][4], 0x2ada8); /* 0x2076610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][5], 0xeba7); /* 0x2076614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][6], 0x345ba); /* 0x2076618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][7], 0x29b86); /* 0x207661c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][8], 0x29fcb); /* 0x2076620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][9], 0x167ec); /* 0x2076624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][10], 0x12748); /* 0x2076628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][11], 0x281c7); /* 0x207662c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][12], 0x991f); /* 0x2076630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][13], 0x6aef); /* 0x2076634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][14], 0x8220); /* 0x2076638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][15], 0x35802); /* 0x207663c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][16], 0x21bcb); /* 0x2076640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][17], 0x2f04d); /* 0x2076644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][18], 0x23355); /* 0x2076648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][19], 0x29436); /* 0x207664c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][20], 0x291f9); /* 0x2076650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][21], 0x1c9e4); /* 0x2076654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][22], 0x21af4); /* 0x2076658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][23], 0xe3f6); /* 0x207665c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][24], 0xf76a); /* 0x2076660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][25], 0x164ce); /* 0x2076664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][26], 0x3ecce); /* 0x2076668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][27], 0x1ca2e); /* 0x207666c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][28], 0xbc4a); /* 0x2076670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][29], 0x2ea5c); /* 0x2076674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][30], 0x11b2a); /* 0x2076678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][31], 0x25934); /* 0x207667c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][32], 0x22444); /* 0x2076680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][33], 0x3c592); /* 0x2076684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][34], 0x3779f); /* 0x2076688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][35], 0x32318); /* 0x207668c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][36], 0x3d255); /* 0x2076690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][37], 0x3cf6c); /* 0x2076694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][38], 0x30f8); /* 0x2076698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][39], 0x3e2f7); /* 0x207669c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][40], 0xa028); /* 0x20766a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][41], 0xfaf1); /* 0x20766a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][42], 0x2b4cd); /* 0x20766a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][43], 0x2620); /* 0x20766ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][44], 0xf668); /* 0x20766b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][45], 0x9cb3); /* 0x20766b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][46], 0x5137); /* 0x20766b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][47], 0x335c7); /* 0x20766bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][48], 0x21290); /* 0x20766c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][49], 0x51b6); /* 0x20766c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][50], 0x18cbd); /* 0x20766c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[38][51], 0x4c10); /* 0x20766cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][0], 0x31566); /* 0x2076700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][1], 0x37a3); /* 0x2076704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][2], 0x209e2); /* 0x2076708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][3], 0x4467); /* 0x207670c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][4], 0x1e82c); /* 0x2076710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][5], 0x24d30); /* 0x2076714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][6], 0x217de); /* 0x2076718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][7], 0x1a4e4); /* 0x207671c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][8], 0xcf0b); /* 0x2076720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][9], 0x2e809); /* 0x2076724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][10], 0x23c6c); /* 0x2076728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][11], 0xe526); /* 0x207672c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][12], 0xfec9); /* 0x2076730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][13], 0x32e8e); /* 0x2076734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][14], 0x27a89); /* 0x2076738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][15], 0xe3b2); /* 0x207673c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][16], 0x19253); /* 0x2076740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][17], 0x35284); /* 0x2076744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][18], 0x394cc); /* 0x2076748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][19], 0x931d); /* 0x207674c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][20], 0x133d); /* 0x2076750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][21], 0x3957a); /* 0x2076754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][22], 0x2c377); /* 0x2076758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][23], 0x349b9); /* 0x207675c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][24], 0x32da5); /* 0x2076760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][25], 0xdd53); /* 0x2076764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][26], 0x303d2); /* 0x2076768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][27], 0x363cc); /* 0x207676c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][28], 0x847e); /* 0x2076770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][29], 0x1c784); /* 0x2076774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][30], 0x1bcc1); /* 0x2076778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][31], 0x21a07); /* 0x207677c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][32], 0x4469); /* 0x2076780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][33], 0xdbb1); /* 0x2076784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][34], 0x7df3); /* 0x2076788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][35], 0x2a018); /* 0x207678c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][36], 0x30192); /* 0x2076790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][37], 0x21c9a); /* 0x2076794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][38], 0xd2a7); /* 0x2076798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][39], 0x265f0); /* 0x207679c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][40], 0x408); /* 0x20767a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][41], 0x2c74e); /* 0x20767a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][42], 0x17a8); /* 0x20767a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][43], 0x1d5b6); /* 0x20767ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][44], 0x32820); /* 0x20767b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][45], 0x139ae); /* 0x20767b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][46], 0x28442); /* 0x20767b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][47], 0x3e047); /* 0x20767bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][48], 0x14224); /* 0x20767c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][49], 0x3b87e); /* 0x20767c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][50], 0x12cf3); /* 0x20767c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[39][51], 0x14414); /* 0x20767cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][0], 0x3623b); /* 0x2076800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][1], 0x5500); /* 0x2076804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][2], 0x9c4d); /* 0x2076808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][3], 0x13759); /* 0x207680c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][4], 0x241f3); /* 0x2076810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][5], 0x2fd7b); /* 0x2076814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][6], 0x962a); /* 0x2076818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][7], 0x138fe); /* 0x207681c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][8], 0x1cbdf); /* 0x2076820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][9], 0xaf72); /* 0x2076824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][10], 0x38f56); /* 0x2076828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][11], 0x2f0df); /* 0x207682c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][12], 0x2dd01); /* 0x2076830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][13], 0x390cf); /* 0x2076834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][14], 0x3f2b5); /* 0x2076838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][15], 0x392d5); /* 0x207683c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][16], 0x82f8); /* 0x2076840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][17], 0x3bcbf); /* 0x2076844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][18], 0x4537); /* 0x2076848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][19], 0x2941a); /* 0x207684c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][20], 0xd6bd); /* 0x2076850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][21], 0x1fd30); /* 0x2076854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][22], 0x2f8b3); /* 0x2076858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][23], 0x3925d); /* 0x207685c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][24], 0x3ee3); /* 0x2076860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][25], 0x4032); /* 0x2076864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][26], 0x33f04); /* 0x2076868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][27], 0x14877); /* 0x207686c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][28], 0x1e8da); /* 0x2076870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][29], 0x24eb4); /* 0x2076874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][30], 0x1ac78); /* 0x2076878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][31], 0xe6f0); /* 0x207687c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][32], 0x3c2b); /* 0x2076880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][33], 0x107dc); /* 0x2076884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][34], 0x83df); /* 0x2076888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][35], 0x12287); /* 0x207688c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][36], 0x2c8f7); /* 0x2076890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][37], 0xa590); /* 0x2076894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][38], 0x2e4ef); /* 0x2076898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][39], 0x2efec); /* 0x207689c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][40], 0x2f180); /* 0x20768a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][41], 0x13aca); /* 0x20768a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][42], 0x378d1); /* 0x20768a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][43], 0x372dd); /* 0x20768ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][44], 0x17470); /* 0x20768b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][45], 0x28cf4); /* 0x20768b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][46], 0x3925); /* 0x20768b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][47], 0x5a36); /* 0x20768bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][48], 0x12f31); /* 0x20768c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][49], 0x30524); /* 0x20768c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][50], 0x5c5e); /* 0x20768c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[40][51], 0x1974e); /* 0x20768cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][0], 0x25813); /* 0x2076900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][1], 0x2b093); /* 0x2076904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][2], 0x2afc0); /* 0x2076908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][3], 0x2a385); /* 0x207690c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][4], 0xd4ec); /* 0x2076910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][5], 0x1ecf9); /* 0x2076914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][6], 0x4e02); /* 0x2076918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][7], 0x2781c); /* 0x207691c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][8], 0x14588); /* 0x2076920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][9], 0x24e4a); /* 0x2076924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][10], 0x30e9d); /* 0x2076928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][11], 0x16d41); /* 0x207692c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][12], 0x1d546); /* 0x2076930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][13], 0x14085); /* 0x2076934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][14], 0x35719); /* 0x2076938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][15], 0x2f4f8); /* 0x207693c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][16], 0x6fb7); /* 0x2076940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][17], 0xa25c); /* 0x2076944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][18], 0x3b01); /* 0x2076948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][19], 0x3b6b9); /* 0x207694c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][20], 0x249ae); /* 0x2076950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][21], 0x185e7); /* 0x2076954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][22], 0x3c386); /* 0x2076958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][23], 0x27daf); /* 0x207695c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][24], 0x1a18e); /* 0x2076960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][25], 0x347d9); /* 0x2076964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][26], 0x1b008); /* 0x2076968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][27], 0x19c14); /* 0x207696c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][28], 0x1dfc2); /* 0x2076970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][29], 0x3a622); /* 0x2076974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][30], 0x1857); /* 0x2076978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][31], 0x2f96); /* 0x207697c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][32], 0x11e93); /* 0x2076980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][33], 0x5b32); /* 0x2076984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][34], 0x18779); /* 0x2076988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][35], 0x4b94); /* 0x207698c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][36], 0x10035); /* 0x2076990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][37], 0x346dc); /* 0x2076994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][38], 0x1d5b9); /* 0x2076998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][39], 0x2bf69); /* 0x207699c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][40], 0x7006); /* 0x20769a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][41], 0x2d8f5); /* 0x20769a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][42], 0x28960); /* 0x20769a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][43], 0x2b8d9); /* 0x20769ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][44], 0x3192e); /* 0x20769b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][45], 0x24bb6); /* 0x20769b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][46], 0x990b); /* 0x20769b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][47], 0x2f21a); /* 0x20769bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][48], 0x1e733); /* 0x20769c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][49], 0x3ba9a); /* 0x20769c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][50], 0x2c3ce); /* 0x20769c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[41][51], 0x86a5); /* 0x20769cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][0], 0xf0e0); /* 0x2076a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][1], 0x471a); /* 0x2076a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][2], 0x122a7); /* 0x2076a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][3], 0x1e97e); /* 0x2076a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][4], 0x27943); /* 0x2076a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][5], 0x17b03); /* 0x2076a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][6], 0x1af36); /* 0x2076a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][7], 0xdf08); /* 0x2076a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][8], 0x3ee97); /* 0x2076a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][9], 0x3513a); /* 0x2076a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][10], 0x334ae); /* 0x2076a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][11], 0x28f7); /* 0x2076a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][12], 0x1c900); /* 0x2076a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][13], 0x34c5d); /* 0x2076a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][14], 0x2c98a); /* 0x2076a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][15], 0x2d6f5); /* 0x2076a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][16], 0xd9d2); /* 0x2076a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][17], 0x3f846); /* 0x2076a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][18], 0xae2); /* 0x2076a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][19], 0x3fa8f); /* 0x2076a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][20], 0x23286); /* 0x2076a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][21], 0xe567); /* 0x2076a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][22], 0xa2e1); /* 0x2076a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][23], 0x29db5); /* 0x2076a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][24], 0x34e4d); /* 0x2076a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][25], 0x315cc); /* 0x2076a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][26], 0x2368b); /* 0x2076a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][27], 0x3be86); /* 0x2076a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][28], 0x2c0b1); /* 0x2076a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][29], 0x15811); /* 0x2076a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][30], 0x2a081); /* 0x2076a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][31], 0x1efb9); /* 0x2076a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][32], 0xb9e8); /* 0x2076a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][33], 0x35aa9); /* 0x2076a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][34], 0x3ca81); /* 0x2076a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][35], 0x3c2ca); /* 0x2076a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][36], 0x13b1f); /* 0x2076a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][37], 0x1e110); /* 0x2076a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][38], 0x12c45); /* 0x2076a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][39], 0x38e4a); /* 0x2076a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][40], 0x2e32f); /* 0x2076aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][41], 0x2744e); /* 0x2076aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][42], 0x357bb); /* 0x2076aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][43], 0x10cb1); /* 0x2076aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][44], 0x172e1); /* 0x2076ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][45], 0x3e038); /* 0x2076ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][46], 0x877d); /* 0x2076ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][47], 0x1dc0e); /* 0x2076abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][48], 0x328d0); /* 0x2076ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][49], 0x243c); /* 0x2076ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][50], 0x386d8); /* 0x2076ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[42][51], 0x2b2f5); /* 0x2076acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][0], 0x129f9); /* 0x2076b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][1], 0x190f3); /* 0x2076b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][2], 0x1ea5d); /* 0x2076b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][3], 0x336dc); /* 0x2076b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][4], 0x17e09); /* 0x2076b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][5], 0x7864); /* 0x2076b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][6], 0x3f87c); /* 0x2076b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][7], 0x1def1); /* 0x2076b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][8], 0x26cb3); /* 0x2076b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][9], 0x18d4d); /* 0x2076b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][10], 0x3b713); /* 0x2076b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][11], 0x3a1db); /* 0x2076b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][12], 0x26909); /* 0x2076b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][13], 0xaccf); /* 0x2076b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][14], 0x2e660); /* 0x2076b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][15], 0xf66f); /* 0x2076b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][16], 0x3458f); /* 0x2076b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][17], 0x339c0); /* 0x2076b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][18], 0x72cb); /* 0x2076b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][19], 0x2ba01); /* 0x2076b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][20], 0x3729e); /* 0x2076b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][21], 0x3cb0c); /* 0x2076b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][22], 0x34510); /* 0x2076b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][23], 0x1afda); /* 0x2076b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][24], 0x2fb23); /* 0x2076b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][25], 0x350d2); /* 0x2076b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][26], 0x12226); /* 0x2076b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][27], 0x31e05); /* 0x2076b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][28], 0x2eb30); /* 0x2076b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][29], 0x502e); /* 0x2076b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][30], 0x3f726); /* 0x2076b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][31], 0x21a05); /* 0x2076b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][32], 0x36104); /* 0x2076b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][33], 0x18afb); /* 0x2076b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][34], 0x1edcc); /* 0x2076b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][35], 0x2894c); /* 0x2076b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][36], 0x524c); /* 0x2076b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][37], 0x2ffcd); /* 0x2076b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][38], 0x9dfe); /* 0x2076b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][39], 0x4d9e); /* 0x2076b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][40], 0x28c89); /* 0x2076ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][41], 0x25293); /* 0x2076ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][42], 0x3857e); /* 0x2076ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][43], 0x1681d); /* 0x2076bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][44], 0x1a40e); /* 0x2076bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][45], 0x26e55); /* 0x2076bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][46], 0x2417); /* 0x2076bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][47], 0x367e3); /* 0x2076bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][48], 0x7884); /* 0x2076bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][49], 0x332de); /* 0x2076bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][50], 0x3053b); /* 0x2076bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[43][51], 0x11995); /* 0x2076bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][0], 0x1daaa); /* 0x2076c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][1], 0x3b834); /* 0x2076c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][2], 0x151af); /* 0x2076c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][3], 0x6739); /* 0x2076c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][4], 0x2719a); /* 0x2076c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][5], 0xdadf); /* 0x2076c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][6], 0x2f53a); /* 0x2076c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][7], 0x26b25); /* 0x2076c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][8], 0x489); /* 0x2076c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][9], 0x2f69b); /* 0x2076c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][10], 0x1a9f9); /* 0x2076c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][11], 0xf391); /* 0x2076c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][12], 0x2fa9a); /* 0x2076c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][13], 0x32c21); /* 0x2076c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][14], 0x2b02f); /* 0x2076c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][15], 0x87a5); /* 0x2076c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][16], 0x2637c); /* 0x2076c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][17], 0x20bf8); /* 0x2076c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][18], 0xa18f); /* 0x2076c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][19], 0x16a35); /* 0x2076c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][20], 0x8202); /* 0x2076c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][21], 0xb9c3); /* 0x2076c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][22], 0x39c9a); /* 0x2076c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][23], 0x1c0e4); /* 0x2076c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][24], 0x3d057); /* 0x2076c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][25], 0x14ca2); /* 0x2076c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][26], 0x3a347); /* 0x2076c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][27], 0x2f001); /* 0x2076c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][28], 0x16fcf); /* 0x2076c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][29], 0x6c98); /* 0x2076c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][30], 0x3bbc5); /* 0x2076c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][31], 0x3a847); /* 0x2076c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][32], 0xa87f); /* 0x2076c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][33], 0x32bf5); /* 0x2076c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][34], 0x3b970); /* 0x2076c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][35], 0xf116); /* 0x2076c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][36], 0x2693a); /* 0x2076c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][37], 0xbf6a); /* 0x2076c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][38], 0x10606); /* 0x2076c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][39], 0x3f590); /* 0x2076c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][40], 0x1aa91); /* 0x2076ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][41], 0x4b23); /* 0x2076ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][42], 0x1a9d9); /* 0x2076ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][43], 0xffad); /* 0x2076cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][44], 0x3fefd); /* 0x2076cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][45], 0x22b7); /* 0x2076cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][46], 0x1fba6); /* 0x2076cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][47], 0x3a67a); /* 0x2076cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][48], 0x365d4); /* 0x2076cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][49], 0xa80d); /* 0x2076cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][50], 0x1a0ff); /* 0x2076cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[44][51], 0x2e42d); /* 0x2076ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][0], 0xa881); /* 0x2076d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][1], 0xa01d); /* 0x2076d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][2], 0x36c95); /* 0x2076d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][3], 0x2b3fe); /* 0x2076d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][4], 0x5e2c); /* 0x2076d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][5], 0x3f341); /* 0x2076d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][6], 0x26f8b); /* 0x2076d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][7], 0x1efde); /* 0x2076d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][8], 0x1404f); /* 0x2076d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][9], 0x30432); /* 0x2076d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][10], 0x62ba); /* 0x2076d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][11], 0x406e); /* 0x2076d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][12], 0x141a6); /* 0x2076d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][13], 0x1fc2b); /* 0x2076d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][14], 0x15c41); /* 0x2076d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][15], 0x3c388); /* 0x2076d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][16], 0x1c12); /* 0x2076d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][17], 0x245ba); /* 0x2076d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][18], 0x36e85); /* 0x2076d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][19], 0x2e1f8); /* 0x2076d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][20], 0x197cc); /* 0x2076d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][21], 0x3e4d5); /* 0x2076d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][22], 0x29c84); /* 0x2076d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][23], 0x1b35e); /* 0x2076d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][24], 0x5a8d); /* 0x2076d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][25], 0x3207c); /* 0x2076d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][26], 0x1af15); /* 0x2076d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][27], 0x1a7d6); /* 0x2076d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][28], 0x6); /* 0x2076d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][29], 0x32358); /* 0x2076d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][30], 0x11a75); /* 0x2076d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][31], 0x35146); /* 0x2076d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][32], 0x1801d); /* 0x2076d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][33], 0x26111); /* 0x2076d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][34], 0x4e42); /* 0x2076d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][35], 0x37d6a); /* 0x2076d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][36], 0x27dbf); /* 0x2076d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][37], 0x389a5); /* 0x2076d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][38], 0x2b08f); /* 0x2076d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][39], 0x1033c); /* 0x2076d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][40], 0x336b8); /* 0x2076da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][41], 0x1f593); /* 0x2076da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][42], 0x1f9c9); /* 0x2076da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][43], 0x9c3f); /* 0x2076dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][44], 0x7de0); /* 0x2076db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][45], 0x2b51a); /* 0x2076db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][46], 0x1b424); /* 0x2076db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][47], 0x39481); /* 0x2076dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][48], 0x11e10); /* 0x2076dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][49], 0x28444); /* 0x2076dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][50], 0x3f79d); /* 0x2076dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[45][51], 0x1c326); /* 0x2076dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][0], 0x2336d); /* 0x2076e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][1], 0x3c6db); /* 0x2076e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][2], 0x38f81); /* 0x2076e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][3], 0x3349a); /* 0x2076e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][4], 0x1491f); /* 0x2076e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][5], 0x34cb3); /* 0x2076e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][6], 0x1b05f); /* 0x2076e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][7], 0x35b3c); /* 0x2076e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][8], 0xb632); /* 0x2076e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][9], 0x3d3e2); /* 0x2076e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][10], 0x35776); /* 0x2076e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][11], 0xd234); /* 0x2076e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][12], 0x23172); /* 0x2076e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][13], 0x15089); /* 0x2076e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][14], 0x152fe); /* 0x2076e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][15], 0x140a0); /* 0x2076e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][16], 0x2e087); /* 0x2076e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][17], 0x10e0c); /* 0x2076e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][18], 0x3f708); /* 0x2076e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][19], 0x19240); /* 0x2076e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][20], 0xb455); /* 0x2076e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][21], 0xf5aa); /* 0x2076e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][22], 0x1fcae); /* 0x2076e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][23], 0x1c0d1); /* 0x2076e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][24], 0x2ee8b); /* 0x2076e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][25], 0x2e43b); /* 0x2076e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][26], 0x3f865); /* 0x2076e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][27], 0x144ba); /* 0x2076e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][28], 0x1ef2); /* 0x2076e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][29], 0x33d5d); /* 0x2076e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][30], 0x171bf); /* 0x2076e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][31], 0x2f030); /* 0x2076e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][32], 0x5dc8); /* 0x2076e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][33], 0x1a5c9); /* 0x2076e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][34], 0x159f); /* 0x2076e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][35], 0x147d5); /* 0x2076e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][36], 0x34858); /* 0x2076e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][37], 0x38f2f); /* 0x2076e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][38], 0x13625); /* 0x2076e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][39], 0x2e1c3); /* 0x2076e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][40], 0x37597); /* 0x2076ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][41], 0xdf86); /* 0x2076ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][42], 0x199c0); /* 0x2076ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][43], 0x18eb8); /* 0x2076eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][44], 0x1739); /* 0x2076eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][45], 0x38868); /* 0x2076eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][46], 0x8991); /* 0x2076eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][47], 0x2004c); /* 0x2076ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][48], 0xcdea); /* 0x2076ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][49], 0x1894e); /* 0x2076ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][50], 0xcba9); /* 0x2076ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[46][51], 0x1252b); /* 0x2076ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][0], 0x24f90); /* 0x2076f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][1], 0x16e19); /* 0x2076f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][2], 0x333d3); /* 0x2076f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][3], 0x2919f); /* 0x2076f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][4], 0x2e2fc); /* 0x2076f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][5], 0xda02); /* 0x2076f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][6], 0x370cb); /* 0x2076f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][7], 0x22496); /* 0x2076f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][8], 0xbf15); /* 0x2076f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][9], 0x344b5); /* 0x2076f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][10], 0xdc4b); /* 0x2076f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][11], 0x1fc92); /* 0x2076f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][12], 0x3598); /* 0x2076f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][13], 0x2f84); /* 0x2076f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][14], 0x29206); /* 0x2076f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][15], 0x2dfa8); /* 0x2076f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][16], 0x1696a); /* 0x2076f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][17], 0x1ce1e); /* 0x2076f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][18], 0x2fff7); /* 0x2076f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][19], 0x2ae9e); /* 0x2076f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][20], 0xf366); /* 0x2076f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][21], 0x31dc4); /* 0x2076f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][22], 0x2ac76); /* 0x2076f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][23], 0x19c0a); /* 0x2076f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][24], 0x1a569); /* 0x2076f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][25], 0xe857); /* 0x2076f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][26], 0x36e16); /* 0x2076f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][27], 0x2fc43); /* 0x2076f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][28], 0x1d6d4); /* 0x2076f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][29], 0x37f4f); /* 0x2076f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][30], 0x1a9a); /* 0x2076f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][31], 0x1efa7); /* 0x2076f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][32], 0x2816); /* 0x2076f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][33], 0x8a3); /* 0x2076f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][34], 0x2d5a4); /* 0x2076f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][35], 0x18bd5); /* 0x2076f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][36], 0x35c30); /* 0x2076f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][37], 0xeed0); /* 0x2076f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][38], 0x99f0); /* 0x2076f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][39], 0xf00f); /* 0x2076f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][40], 0x1e2d0); /* 0x2076fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][41], 0x2cfa5); /* 0x2076fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][42], 0x30ded); /* 0x2076fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][43], 0x12920); /* 0x2076fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][44], 0x35c1c); /* 0x2076fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][45], 0x167ea); /* 0x2076fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][46], 0x19c62); /* 0x2076fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][47], 0x18099); /* 0x2076fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][48], 0x9f37); /* 0x2076fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][49], 0x2a07c); /* 0x2076fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][50], 0x2c1c9); /* 0x2076fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[47][51], 0x36a14); /* 0x2076fcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][0], 0xd97d); /* 0x2077000 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][1], 0x6b44); /* 0x2077004 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][2], 0x387fa); /* 0x2077008 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][3], 0x382f5); /* 0x207700c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][4], 0x288e2); /* 0x2077010 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][5], 0xdced); /* 0x2077014 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][6], 0xe8c4); /* 0x2077018 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][7], 0x562e); /* 0x207701c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][8], 0x21660); /* 0x2077020 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][9], 0x3c41f); /* 0x2077024 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][10], 0x2e4cf); /* 0x2077028 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][11], 0x1cd14); /* 0x207702c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][12], 0xef6e); /* 0x2077030 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][13], 0x7a7f); /* 0x2077034 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][14], 0x172b4); /* 0x2077038 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][15], 0x802f); /* 0x207703c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][16], 0x230a8); /* 0x2077040 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][17], 0xd7f9); /* 0x2077044 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][18], 0x3ae35); /* 0x2077048 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][19], 0x2e3de); /* 0x207704c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][20], 0xbd17); /* 0x2077050 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][21], 0x1694d); /* 0x2077054 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][22], 0x11833); /* 0x2077058 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][23], 0x1f4bf); /* 0x207705c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][24], 0x14ec9); /* 0x2077060 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][25], 0x12def); /* 0x2077064 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][26], 0x21b48); /* 0x2077068 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][27], 0x6d3d); /* 0x207706c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][28], 0x26373); /* 0x2077070 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][29], 0x1e345); /* 0x2077074 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][30], 0x3e335); /* 0x2077078 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][31], 0x39184); /* 0x207707c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][32], 0x1fd1); /* 0x2077080 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][33], 0x2e0a7); /* 0x2077084 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][34], 0x3287); /* 0x2077088 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][35], 0x10414); /* 0x207708c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][36], 0x8e99); /* 0x2077090 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][37], 0x25d85); /* 0x2077094 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][38], 0x22de4); /* 0x2077098 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][39], 0x3c7d9); /* 0x207709c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][40], 0x286ec); /* 0x20770a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][41], 0x14d13); /* 0x20770a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][42], 0x9f97); /* 0x20770a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][43], 0x3a1a2); /* 0x20770ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][44], 0x38381); /* 0x20770b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][45], 0x3f3c2); /* 0x20770b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][46], 0x10377); /* 0x20770b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][47], 0x12b8); /* 0x20770bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][48], 0x50f6); /* 0x20770c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][49], 0x131df); /* 0x20770c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][50], 0x5823); /* 0x20770c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[48][51], 0x929f); /* 0x20770cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][0], 0x37204); /* 0x2077100 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][1], 0x33a09); /* 0x2077104 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][2], 0x3c0be); /* 0x2077108 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][3], 0x5276); /* 0x207710c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][4], 0x1f38f); /* 0x2077110 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][5], 0x22b98); /* 0x2077114 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][6], 0x25637); /* 0x2077118 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][7], 0x1b6df); /* 0x207711c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][8], 0x34ab); /* 0x2077120 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][9], 0x4cc5); /* 0x2077124 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][10], 0xff6e); /* 0x2077128 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][11], 0x34607); /* 0x207712c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][12], 0x34c6c); /* 0x2077130 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][13], 0x31a38); /* 0x2077134 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][14], 0x200cc); /* 0x2077138 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][15], 0x72f5); /* 0x207713c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][16], 0x32d62); /* 0x2077140 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][17], 0xd488); /* 0x2077144 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][18], 0x96a5); /* 0x2077148 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][19], 0x1e2ce); /* 0x207714c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][20], 0x3b744); /* 0x2077150 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][21], 0x1ce15); /* 0x2077154 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][22], 0x25688); /* 0x2077158 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][23], 0x5585); /* 0x207715c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][24], 0x3eae0); /* 0x2077160 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][25], 0x16511); /* 0x2077164 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][26], 0x3e65b); /* 0x2077168 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][27], 0xae3e); /* 0x207716c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][28], 0x2e337); /* 0x2077170 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][29], 0x3980b); /* 0x2077174 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][30], 0x386d1); /* 0x2077178 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][31], 0x10d21); /* 0x207717c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][32], 0x24f21); /* 0x2077180 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][33], 0xcbfc); /* 0x2077184 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][34], 0x1c733); /* 0x2077188 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][35], 0x3eae3); /* 0x207718c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][36], 0x3794f); /* 0x2077190 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][37], 0xece4); /* 0x2077194 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][38], 0x2d732); /* 0x2077198 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][39], 0x352f5); /* 0x207719c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][40], 0xa34b); /* 0x20771a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][41], 0x41c8); /* 0x20771a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][42], 0x3872c); /* 0x20771a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][43], 0x32ce1); /* 0x20771ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][44], 0x1d870); /* 0x20771b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][45], 0xb479); /* 0x20771b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][46], 0x28297); /* 0x20771b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][47], 0x3807c); /* 0x20771bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][48], 0x39ace); /* 0x20771c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][49], 0x3c0a0); /* 0x20771c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][50], 0x27611); /* 0x20771c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[49][51], 0x1577); /* 0x20771cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][0], 0x223b3); /* 0x2077200 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][1], 0x2ff26); /* 0x2077204 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][2], 0x23986); /* 0x2077208 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][3], 0x239a2); /* 0x207720c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][4], 0x28f41); /* 0x2077210 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][5], 0x162c2); /* 0x2077214 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][6], 0x262b4); /* 0x2077218 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][7], 0x5477); /* 0x207721c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][8], 0x28dc9); /* 0x2077220 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][9], 0xb502); /* 0x2077224 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][10], 0x7b21); /* 0x2077228 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][11], 0x15a91); /* 0x207722c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][12], 0x220f6); /* 0x2077230 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][13], 0x39c55); /* 0x2077234 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][14], 0x2b515); /* 0x2077238 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][15], 0x96e3); /* 0x207723c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][16], 0xa85e); /* 0x2077240 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][17], 0x21681); /* 0x2077244 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][18], 0xfe67); /* 0x2077248 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][19], 0x10d9c); /* 0x207724c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][20], 0x2f9e6); /* 0x2077250 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][21], 0x23a27); /* 0x2077254 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][22], 0x299e2); /* 0x2077258 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][23], 0x164d1); /* 0x207725c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][24], 0x12eea); /* 0x2077260 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][25], 0x11bc6); /* 0x2077264 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][26], 0x3eced); /* 0x2077268 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][27], 0x1bab3); /* 0x207726c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][28], 0x673d); /* 0x2077270 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][29], 0x536b); /* 0x2077274 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][30], 0x3efbd); /* 0x2077278 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][31], 0x11c61); /* 0x207727c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][32], 0x4f76); /* 0x2077280 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][33], 0x20f95); /* 0x2077284 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][34], 0x23401); /* 0x2077288 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][35], 0x3508); /* 0x207728c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][36], 0x372f8); /* 0x2077290 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][37], 0x16001); /* 0x2077294 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][38], 0x3d505); /* 0x2077298 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][39], 0xeea4); /* 0x207729c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][40], 0x18e8a); /* 0x20772a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][41], 0x79a3); /* 0x20772a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][42], 0x221eb); /* 0x20772a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][43], 0x3c94); /* 0x20772ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][44], 0x10c85); /* 0x20772b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][45], 0x30b83); /* 0x20772b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][46], 0x38882); /* 0x20772b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][47], 0x31429); /* 0x20772bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][48], 0x316b9); /* 0x20772c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][49], 0x14648); /* 0x20772c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][50], 0x237ac); /* 0x20772c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[50][51], 0x2dcd5); /* 0x20772cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][0], 0x42d2); /* 0x2077300 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][1], 0x25d49); /* 0x2077304 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][2], 0x39c3a); /* 0x2077308 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][3], 0x1d07f); /* 0x207730c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][4], 0x3ef3a); /* 0x2077310 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][5], 0x3cc45); /* 0x2077314 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][6], 0x152ee); /* 0x2077318 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][7], 0x221d7); /* 0x207731c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][8], 0x1979b); /* 0x2077320 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][9], 0x29717); /* 0x2077324 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][10], 0x1b9ae); /* 0x2077328 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][11], 0xb3de); /* 0x207732c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][12], 0x22d6); /* 0x2077330 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][13], 0x6b71); /* 0x2077334 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][14], 0x14f49); /* 0x2077338 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][15], 0x12cb2); /* 0x207733c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][16], 0x74fe); /* 0x2077340 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][17], 0x264a2); /* 0x2077344 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][18], 0x2f674); /* 0x2077348 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][19], 0x1b805); /* 0x207734c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][20], 0x14c15); /* 0x2077350 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][21], 0x266ba); /* 0x2077354 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][22], 0x3a7d5); /* 0x2077358 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][23], 0x3089d); /* 0x207735c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][24], 0x224cf); /* 0x2077360 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][25], 0x2b29d); /* 0x2077364 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][26], 0x2f477); /* 0x2077368 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][27], 0x2ced7); /* 0x207736c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][28], 0x2087a); /* 0x2077370 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][29], 0x35a69); /* 0x2077374 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][30], 0x2ad54); /* 0x2077378 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][31], 0x3d12c); /* 0x207737c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][32], 0x2e6f); /* 0x2077380 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][33], 0xc264); /* 0x2077384 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][34], 0x21dfc); /* 0x2077388 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][35], 0x28385); /* 0x207738c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][36], 0x23d40); /* 0x2077390 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][37], 0x30791); /* 0x2077394 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][38], 0x1a54a); /* 0x2077398 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][39], 0x570); /* 0x207739c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][40], 0x178a3); /* 0x20773a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][41], 0x10d07); /* 0x20773a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][42], 0x33fc9); /* 0x20773a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][43], 0x7332); /* 0x20773ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][44], 0x382e3); /* 0x20773b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][45], 0x27b37); /* 0x20773b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][46], 0x2590); /* 0x20773b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][47], 0x1eb2c); /* 0x20773bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][48], 0x13906); /* 0x20773c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][49], 0x1362e); /* 0x20773c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][50], 0x24251); /* 0x20773c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[51][51], 0xc8c5); /* 0x20773cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][0], 0x8fc5); /* 0x2077400 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][1], 0x9993); /* 0x2077404 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][2], 0x22e29); /* 0x2077408 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][3], 0x49c6); /* 0x207740c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][4], 0x3b0e0); /* 0x2077410 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][5], 0x158c4); /* 0x2077414 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][6], 0x2c8ba); /* 0x2077418 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][7], 0x37d05); /* 0x207741c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][8], 0x12137); /* 0x2077420 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][9], 0xb6a5); /* 0x2077424 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][10], 0xb723); /* 0x2077428 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][11], 0xc8e5); /* 0x207742c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][12], 0x2936e); /* 0x2077430 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][13], 0x1e6e7); /* 0x2077434 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][14], 0x1bc11); /* 0x2077438 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][15], 0x3bca); /* 0x207743c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][16], 0x17dc9); /* 0x2077440 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][17], 0xd433); /* 0x2077444 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][18], 0x1dc18); /* 0x2077448 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][19], 0x9428); /* 0x207744c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][20], 0x12579); /* 0x2077450 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][21], 0x2d986); /* 0x2077454 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][22], 0xac56); /* 0x2077458 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][23], 0x21a06); /* 0x207745c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][24], 0x2fcd4); /* 0x2077460 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][25], 0x9afa); /* 0x2077464 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][26], 0x23912); /* 0x2077468 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][27], 0x1f33c); /* 0x207746c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][28], 0x16c95); /* 0x2077470 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][29], 0x2f3e); /* 0x2077474 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][30], 0x1f0cf); /* 0x2077478 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][31], 0xb98e); /* 0x207747c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][32], 0x374cc); /* 0x2077480 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][33], 0x13c30); /* 0x2077484 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][34], 0x3e09c); /* 0x2077488 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][35], 0x2242c); /* 0x207748c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][36], 0x1f047); /* 0x2077490 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][37], 0x4b6a); /* 0x2077494 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][38], 0x8d6a); /* 0x2077498 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][39], 0x9bf7); /* 0x207749c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][40], 0x46bf); /* 0x20774a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][41], 0x4f2); /* 0x20774a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][42], 0xe874); /* 0x20774a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][43], 0x3becd); /* 0x20774ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][44], 0x17d94); /* 0x20774b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][45], 0xd233); /* 0x20774b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][46], 0x16a72); /* 0x20774b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][47], 0x22af6); /* 0x20774bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][48], 0x13844); /* 0x20774c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][49], 0xb41f); /* 0x20774c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][50], 0xe5d2); /* 0x20774c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[52][51], 0x11aa6); /* 0x20774cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][0], 0x33d32); /* 0x2077500 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][1], 0x2c3dc); /* 0x2077504 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][2], 0x25dac); /* 0x2077508 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][3], 0x2c84a); /* 0x207750c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][4], 0x24d1d); /* 0x2077510 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][5], 0x8cce); /* 0x2077514 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][6], 0xf099); /* 0x2077518 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][7], 0x22133); /* 0x207751c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][8], 0xaf1f); /* 0x2077520 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][9], 0x23128); /* 0x2077524 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][10], 0x164aa); /* 0x2077528 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][11], 0x1cb96); /* 0x207752c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][12], 0xcc67); /* 0x2077530 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][13], 0x29e99); /* 0x2077534 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][14], 0x4d01); /* 0x2077538 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][15], 0x1c88b); /* 0x207753c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][16], 0x779a); /* 0x2077540 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][17], 0x1ebfd); /* 0x2077544 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][18], 0x1ced2); /* 0x2077548 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][19], 0x38606); /* 0x207754c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][20], 0x10258); /* 0x2077550 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][21], 0x263ea); /* 0x2077554 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][22], 0x1e4b3); /* 0x2077558 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][23], 0x9c7e); /* 0x207755c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][24], 0x36246); /* 0x2077560 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][25], 0x8504); /* 0x2077564 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][26], 0x25fb6); /* 0x2077568 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][27], 0x3ab84); /* 0x207756c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][28], 0x3aa1b); /* 0x2077570 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][29], 0x37721); /* 0x2077574 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][30], 0x3f53c); /* 0x2077578 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][31], 0x2cf93); /* 0x207757c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][32], 0x16018); /* 0x2077580 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][33], 0x5efc); /* 0x2077584 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][34], 0x1fdb1); /* 0x2077588 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][35], 0x2d23b); /* 0x207758c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][36], 0x2e6b7); /* 0x2077590 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][37], 0xe788); /* 0x2077594 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][38], 0x707e); /* 0x2077598 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][39], 0x3735a); /* 0x207759c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][40], 0x1755b); /* 0x20775a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][41], 0x1acb7); /* 0x20775a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][42], 0x20b13); /* 0x20775a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][43], 0x3230b); /* 0x20775ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][44], 0x1ede2); /* 0x20775b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][45], 0x35903); /* 0x20775b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][46], 0x2172c); /* 0x20775b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][47], 0xbcb3); /* 0x20775bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][48], 0x3a8d7); /* 0x20775c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][49], 0x34ebf); /* 0x20775c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][50], 0xdb2a); /* 0x20775c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[53][51], 0x3d00a); /* 0x20775cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][0], 0x1a265); /* 0x2077600 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][1], 0x344e); /* 0x2077604 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][2], 0xc8a3); /* 0x2077608 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][3], 0x3c96c); /* 0x207760c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][4], 0x3a1cb); /* 0x2077610 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][5], 0x28886); /* 0x2077614 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][6], 0x14388); /* 0x2077618 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][7], 0x34a17); /* 0x207761c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][8], 0x1858e); /* 0x2077620 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][9], 0x3e6cd); /* 0x2077624 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][10], 0x31b68); /* 0x2077628 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][11], 0x2fbab); /* 0x207762c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][12], 0x18de8); /* 0x2077630 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][13], 0x2615c); /* 0x2077634 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][14], 0x71f7); /* 0x2077638 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][15], 0x39cd5); /* 0x207763c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][16], 0x277f); /* 0x2077640 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][17], 0x3e502); /* 0x2077644 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][18], 0x1b90b); /* 0x2077648 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][19], 0x10aca); /* 0x207764c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][20], 0x1ab4e); /* 0x2077650 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][21], 0xcb8c); /* 0x2077654 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][22], 0xd3f7); /* 0x2077658 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][23], 0x37493); /* 0x207765c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][24], 0x10e2d); /* 0x2077660 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][25], 0x1d753); /* 0x2077664 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][26], 0xd837); /* 0x2077668 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][27], 0x29796); /* 0x207766c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][28], 0x1e874); /* 0x2077670 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][29], 0x9a35); /* 0x2077674 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][30], 0x1a2f8); /* 0x2077678 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][31], 0x2d8a3); /* 0x207767c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][32], 0x31016); /* 0x2077680 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][33], 0x19e04); /* 0x2077684 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][34], 0x3d2b2); /* 0x2077688 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][35], 0x307aa); /* 0x207768c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][36], 0x260c8); /* 0x2077690 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][37], 0xbffd); /* 0x2077694 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][38], 0x144aa); /* 0x2077698 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][39], 0x35); /* 0x207769c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][40], 0x17422); /* 0x20776a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][41], 0x3e20f); /* 0x20776a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][42], 0x240d0); /* 0x20776a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][43], 0x12899); /* 0x20776ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][44], 0x27c7c); /* 0x20776b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][45], 0x9ad1); /* 0x20776b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][46], 0x3bf07); /* 0x20776b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][47], 0xf964); /* 0x20776bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][48], 0x378e6); /* 0x20776c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][49], 0x266e2); /* 0x20776c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][50], 0x2b6a1); /* 0x20776c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[54][51], 0x1325e); /* 0x20776cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][0], 0x106d1); /* 0x2077700 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][1], 0x2dbbd); /* 0x2077704 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][2], 0xce8); /* 0x2077708 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][3], 0x2a835); /* 0x207770c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][4], 0x34a55); /* 0x2077710 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][5], 0x3cba); /* 0x2077714 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][6], 0x21460); /* 0x2077718 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][7], 0x4eff); /* 0x207771c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][8], 0x27ea9); /* 0x2077720 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][9], 0x2612); /* 0x2077724 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][10], 0x13aad); /* 0x2077728 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][11], 0xac3); /* 0x207772c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][12], 0x27b82); /* 0x2077730 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][13], 0x19e49); /* 0x2077734 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][14], 0x1e8e1); /* 0x2077738 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][15], 0x38791); /* 0x207773c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][16], 0x2e2d0); /* 0x2077740 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][17], 0x308e0); /* 0x2077744 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][18], 0x37fd); /* 0x2077748 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][19], 0x206f4); /* 0x207774c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][20], 0x1acde); /* 0x2077750 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][21], 0x350fc); /* 0x2077754 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][22], 0x10362); /* 0x2077758 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][23], 0x7ed2); /* 0x207775c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][24], 0x3f64c); /* 0x2077760 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][25], 0x1688f); /* 0x2077764 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][26], 0xc857); /* 0x2077768 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][27], 0x29b9e); /* 0x207776c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][28], 0x25c1e); /* 0x2077770 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][29], 0x16ada); /* 0x2077774 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][30], 0x1dd26); /* 0x2077778 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][31], 0x7f63); /* 0x207777c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][32], 0x26751); /* 0x2077780 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][33], 0x6966); /* 0x2077784 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][34], 0x2aec9); /* 0x2077788 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][35], 0xd7fe); /* 0x207778c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][36], 0x1f68); /* 0x2077790 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][37], 0x37140); /* 0x2077794 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][38], 0x379d); /* 0x2077798 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][39], 0xb0bf); /* 0x207779c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][40], 0x33a54); /* 0x20777a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][41], 0xb5cc); /* 0x20777a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][42], 0x7efd); /* 0x20777a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][43], 0x3a634); /* 0x20777ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][44], 0x64be); /* 0x20777b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][45], 0x20aa8); /* 0x20777b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][46], 0x1238b); /* 0x20777b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][47], 0x102a4); /* 0x20777bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][48], 0x3cc3a); /* 0x20777c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][49], 0x15af3); /* 0x20777c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][50], 0x34016); /* 0x20777c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[55][51], 0x3aa63); /* 0x20777cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][0], 0x6e24); /* 0x2077800 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][1], 0x24011); /* 0x2077804 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][2], 0x2ab4f); /* 0x2077808 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][3], 0x244b); /* 0x207780c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][4], 0x2c854); /* 0x2077810 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][5], 0x902a); /* 0x2077814 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][6], 0x15b4c); /* 0x2077818 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][7], 0x2a465); /* 0x207781c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][8], 0x21d08); /* 0x2077820 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][9], 0x1a6c9); /* 0x2077824 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][10], 0xdd6c); /* 0x2077828 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][11], 0x3909f); /* 0x207782c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][12], 0x3a2d5); /* 0x2077830 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][13], 0xb74b); /* 0x2077834 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][14], 0xe9c4); /* 0x2077838 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][15], 0x2a147); /* 0x207783c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][16], 0x2c464); /* 0x2077840 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][17], 0x25abc); /* 0x2077844 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][18], 0xed28); /* 0x2077848 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][19], 0xb421); /* 0x207784c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][20], 0x2d67b); /* 0x2077850 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][21], 0xa16e); /* 0x2077854 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][22], 0x3b636); /* 0x2077858 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][23], 0xc53f); /* 0x207785c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][24], 0x2d94e); /* 0x2077860 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][25], 0xa1aa); /* 0x2077864 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][26], 0x3214c); /* 0x2077868 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][27], 0xcb77); /* 0x207786c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][28], 0x17535); /* 0x2077870 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][29], 0x3e56e); /* 0x2077874 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][30], 0x25785); /* 0x2077878 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][31], 0xd12d); /* 0x207787c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][32], 0x3e487); /* 0x2077880 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][33], 0xca0e); /* 0x2077884 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][34], 0x2c6bc); /* 0x2077888 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][35], 0x26d7c); /* 0x207788c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][36], 0xcf6e); /* 0x2077890 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][37], 0x27f61); /* 0x2077894 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][38], 0x36ec1); /* 0x2077898 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][39], 0x63b6); /* 0x207789c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][40], 0x37d4f); /* 0x20778a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][41], 0x5e0c); /* 0x20778a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][42], 0x3504b); /* 0x20778a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][43], 0x16ac5); /* 0x20778ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][44], 0x159e7); /* 0x20778b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][45], 0x13913); /* 0x20778b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][46], 0x26957); /* 0x20778b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][47], 0x19b5b); /* 0x20778bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][48], 0x7fb0); /* 0x20778c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][49], 0x64fe); /* 0x20778c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][50], 0x12dd8); /* 0x20778c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[56][51], 0x127dd); /* 0x20778cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][0], 0x1c087); /* 0x2077900 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][1], 0x31175); /* 0x2077904 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][2], 0xc19); /* 0x2077908 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][3], 0x2b10b); /* 0x207790c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][4], 0x39c16); /* 0x2077910 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][5], 0x1e890); /* 0x2077914 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][6], 0x39df1); /* 0x2077918 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][7], 0x2a054); /* 0x207791c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][8], 0x275b0); /* 0x2077920 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][9], 0x34d84); /* 0x2077924 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][10], 0x2a2bb); /* 0x2077928 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][11], 0x3f21d); /* 0x207792c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][12], 0x1933f); /* 0x2077930 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][13], 0x92cd); /* 0x2077934 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][14], 0x17f6f); /* 0x2077938 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][15], 0x1ce1d); /* 0x207793c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][16], 0x3a75d); /* 0x2077940 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][17], 0x1d9d9); /* 0x2077944 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][18], 0x314eb); /* 0x2077948 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][19], 0x3dbf2); /* 0x207794c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][20], 0x3598d); /* 0x2077950 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][21], 0x24d73); /* 0x2077954 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][22], 0xb10d); /* 0x2077958 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][23], 0x3f33f); /* 0x207795c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][24], 0x1a59a); /* 0x2077960 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][25], 0x17602); /* 0x2077964 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][26], 0x1f5a0); /* 0x2077968 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][27], 0x1affc); /* 0x207796c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][28], 0x3376b); /* 0x2077970 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][29], 0x3e4a5); /* 0x2077974 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][30], 0x263be); /* 0x2077978 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][31], 0xdf35); /* 0x207797c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][32], 0x1cd27); /* 0x2077980 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][33], 0x2e6e6); /* 0x2077984 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][34], 0xd858); /* 0x2077988 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][35], 0x1c332); /* 0x207798c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][36], 0x2e359); /* 0x2077990 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][37], 0x25345); /* 0x2077994 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][38], 0x3312d); /* 0x2077998 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][39], 0x35e75); /* 0x207799c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][40], 0x1e037); /* 0x20779a0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][41], 0x349a9); /* 0x20779a4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][42], 0x1e4a7); /* 0x20779a8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][43], 0x25fe0); /* 0x20779ac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][44], 0x22759); /* 0x20779b0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][45], 0x1bd67); /* 0x20779b4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][46], 0x2fa93); /* 0x20779b8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][47], 0x1caa2); /* 0x20779bc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][48], 0x142e); /* 0x20779c0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][49], 0x14487); /* 0x20779c4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][50], 0x10c7f); /* 0x20779c8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[57][51], 0xbe6e); /* 0x20779cc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][0], 0x15f3f); /* 0x2077a00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][1], 0x2ed97); /* 0x2077a04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][2], 0x2cc44); /* 0x2077a08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][3], 0x9313); /* 0x2077a0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][4], 0x20959); /* 0x2077a10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][5], 0xcfc8); /* 0x2077a14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][6], 0x1a6b4); /* 0x2077a18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][7], 0x32f78); /* 0x2077a1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][8], 0x3fd36); /* 0x2077a20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][9], 0x224e9); /* 0x2077a24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][10], 0x13dd9); /* 0x2077a28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][11], 0x331ed); /* 0x2077a2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][12], 0x21c1d); /* 0x2077a30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][13], 0x36aee); /* 0x2077a34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][14], 0x3a272); /* 0x2077a38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][15], 0x1ec97); /* 0x2077a3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][16], 0x2082c); /* 0x2077a40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][17], 0x3f811); /* 0x2077a44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][18], 0x25394); /* 0x2077a48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][19], 0x1f352); /* 0x2077a4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][20], 0x3a151); /* 0x2077a50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][21], 0x225b5); /* 0x2077a54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][22], 0x2dd74); /* 0x2077a58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][23], 0x357f5); /* 0x2077a5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][24], 0x2cf1f); /* 0x2077a60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][25], 0x18b46); /* 0x2077a64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][26], 0x37b32); /* 0x2077a68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][27], 0x34ba0); /* 0x2077a6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][28], 0x11551); /* 0x2077a70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][29], 0x3b3c2); /* 0x2077a74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][30], 0x163d6); /* 0x2077a78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][31], 0x331c6); /* 0x2077a7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][32], 0x3098a); /* 0x2077a80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][33], 0x1e8f0); /* 0x2077a84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][34], 0x48df); /* 0x2077a88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][35], 0xfbb9); /* 0x2077a8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][36], 0xa22e); /* 0x2077a90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][37], 0x24e45); /* 0x2077a94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][38], 0xe915); /* 0x2077a98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][39], 0x1a5d4); /* 0x2077a9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][40], 0x2dd18); /* 0x2077aa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][41], 0x3ada5); /* 0x2077aa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][42], 0x7096); /* 0x2077aa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][43], 0x3573a); /* 0x2077aac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][44], 0x1590c); /* 0x2077ab0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][45], 0x3e4f1); /* 0x2077ab4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][46], 0x32a00); /* 0x2077ab8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][47], 0x24b87); /* 0x2077abc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][48], 0x149f7); /* 0x2077ac0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][49], 0x13568); /* 0x2077ac4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][50], 0x1e47c); /* 0x2077ac8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[58][51], 0x2c7d3); /* 0x2077acc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][0], 0x8ac); /* 0x2077b00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][1], 0x2ca88); /* 0x2077b04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][2], 0xc2eb); /* 0x2077b08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][3], 0xf26e); /* 0x2077b0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][4], 0x28769); /* 0x2077b10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][5], 0x39c6f); /* 0x2077b14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][6], 0x371f6); /* 0x2077b18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][7], 0x129f2); /* 0x2077b1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][8], 0x22df6); /* 0x2077b20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][9], 0x2684b); /* 0x2077b24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][10], 0x3b7f0); /* 0x2077b28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][11], 0x2f553); /* 0x2077b2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][12], 0x7c7c); /* 0x2077b30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][13], 0x30904); /* 0x2077b34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][14], 0x349f4); /* 0x2077b38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][15], 0x3d5ca); /* 0x2077b3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][16], 0x32c17); /* 0x2077b40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][17], 0x13536); /* 0x2077b44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][18], 0x2c50e); /* 0x2077b48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][19], 0x1710a); /* 0x2077b4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][20], 0x2563c); /* 0x2077b50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][21], 0x1c041); /* 0x2077b54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][22], 0x323b5); /* 0x2077b58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][23], 0x1f3b0); /* 0x2077b5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][24], 0xbe23); /* 0x2077b60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][25], 0x1a55c); /* 0x2077b64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][26], 0xad55); /* 0x2077b68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][27], 0x1074b); /* 0x2077b6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][28], 0x3caa4); /* 0x2077b70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][29], 0x3b274); /* 0x2077b74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][30], 0x22dc5); /* 0x2077b78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][31], 0x272ad); /* 0x2077b7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][32], 0x15b7); /* 0x2077b80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][33], 0x38ebd); /* 0x2077b84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][34], 0x1c3d1); /* 0x2077b88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][35], 0x2e1f2); /* 0x2077b8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][36], 0x2314b); /* 0x2077b90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][37], 0x3e882); /* 0x2077b94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][38], 0x1cee3); /* 0x2077b98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][39], 0x3a4f3); /* 0x2077b9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][40], 0x16335); /* 0x2077ba0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][41], 0x281cd); /* 0x2077ba4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][42], 0xbd3c); /* 0x2077ba8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][43], 0x24b5f); /* 0x2077bac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][44], 0x9dbf); /* 0x2077bb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][45], 0x1a7fb); /* 0x2077bb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][46], 0x202f8); /* 0x2077bb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][47], 0x5528); /* 0x2077bbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][48], 0x3fdd1); /* 0x2077bc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][49], 0x2ee5); /* 0x2077bc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][50], 0x3d01); /* 0x2077bc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[59][51], 0x30f5d); /* 0x2077bcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][0], 0x4de0); /* 0x2077c00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][1], 0x2f6df); /* 0x2077c04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][2], 0x3d3b8); /* 0x2077c08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][3], 0x19f02); /* 0x2077c0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][4], 0x36518); /* 0x2077c10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][5], 0x306de); /* 0x2077c14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][6], 0xb6c4); /* 0x2077c18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][7], 0x326cf); /* 0x2077c1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][8], 0x34a83); /* 0x2077c20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][9], 0x28ac6); /* 0x2077c24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][10], 0x2ace8); /* 0x2077c28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][11], 0x2dce7); /* 0x2077c2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][12], 0x3a5e2); /* 0x2077c30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][13], 0x1186a); /* 0x2077c34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][14], 0x69dc); /* 0x2077c38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][15], 0x3032a); /* 0x2077c3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][16], 0x246af); /* 0x2077c40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][17], 0x1ff96); /* 0x2077c44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][18], 0xb704); /* 0x2077c48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][19], 0xa39e); /* 0x2077c4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][20], 0x1b5d0); /* 0x2077c50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][21], 0x18c54); /* 0x2077c54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][22], 0x20529); /* 0x2077c58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][23], 0x2e30b); /* 0x2077c5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][24], 0x36968); /* 0x2077c60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][25], 0x34b92); /* 0x2077c64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][26], 0x347e9); /* 0x2077c68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][27], 0x2ec53); /* 0x2077c6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][28], 0x17755); /* 0x2077c70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][29], 0x3143a); /* 0x2077c74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][30], 0x24bf2); /* 0x2077c78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][31], 0x132cc); /* 0x2077c7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][32], 0x3ce3); /* 0x2077c80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][33], 0x3463f); /* 0x2077c84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][34], 0x7823); /* 0x2077c88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][35], 0x2fd52); /* 0x2077c8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][36], 0x364a0); /* 0x2077c90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][37], 0x17d6d); /* 0x2077c94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][38], 0xc46b); /* 0x2077c98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][39], 0xbeef); /* 0x2077c9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][40], 0x3d86a); /* 0x2077ca0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][41], 0x23da1); /* 0x2077ca4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][42], 0x12baf); /* 0x2077ca8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][43], 0x29673); /* 0x2077cac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][44], 0x285ab); /* 0x2077cb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][45], 0x270e4); /* 0x2077cb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][46], 0x2278f); /* 0x2077cb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][47], 0x2231); /* 0x2077cbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][48], 0xf842); /* 0x2077cc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][49], 0x3350c); /* 0x2077cc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][50], 0x240d5); /* 0x2077cc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[60][51], 0x107e2); /* 0x2077ccc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][0], 0xd23b); /* 0x2077d00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][1], 0x153e8); /* 0x2077d04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][2], 0x2eb9c); /* 0x2077d08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][3], 0x2c29a); /* 0x2077d0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][4], 0x38d3); /* 0x2077d10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][5], 0x13b61); /* 0x2077d14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][6], 0x3335d); /* 0x2077d18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][7], 0x14ff3); /* 0x2077d1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][8], 0x3d572); /* 0x2077d20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][9], 0x36d2c); /* 0x2077d24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][10], 0x12dbb); /* 0x2077d28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][11], 0x3122); /* 0x2077d2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][12], 0x1d5b); /* 0x2077d30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][13], 0x29888); /* 0x2077d34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][14], 0xe886); /* 0x2077d38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][15], 0x1f265); /* 0x2077d3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][16], 0x2fbb9); /* 0x2077d40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][17], 0xf842); /* 0x2077d44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][18], 0x4479); /* 0x2077d48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][19], 0x2dfd4); /* 0x2077d4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][20], 0x1f02); /* 0x2077d50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][21], 0x36672); /* 0x2077d54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][22], 0x1d437); /* 0x2077d58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][23], 0x3ece9); /* 0x2077d5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][24], 0x35d30); /* 0x2077d60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][25], 0x3e018); /* 0x2077d64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][26], 0x25e3); /* 0x2077d68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][27], 0x12b94); /* 0x2077d6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][28], 0x3775c); /* 0x2077d70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][29], 0x210f8); /* 0x2077d74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][30], 0x39b22); /* 0x2077d78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][31], 0x1e243); /* 0x2077d7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][32], 0x16e3f); /* 0x2077d80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][33], 0x11c31); /* 0x2077d84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][34], 0x1f446); /* 0x2077d88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][35], 0x14873); /* 0x2077d8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][36], 0x5ff); /* 0x2077d90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][37], 0x1072f); /* 0x2077d94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][38], 0x1f87c); /* 0x2077d98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][39], 0x241ab); /* 0x2077d9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][40], 0x24e8b); /* 0x2077da0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][41], 0x2f39a); /* 0x2077da4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][42], 0x39151); /* 0x2077da8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][43], 0x5839); /* 0x2077dac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][44], 0x8e97); /* 0x2077db0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][45], 0x14401); /* 0x2077db4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][46], 0xf5f); /* 0x2077db8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][47], 0x2e69a); /* 0x2077dbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][48], 0x1fa01); /* 0x2077dc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][49], 0x2117f); /* 0x2077dc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][50], 0x22ecb); /* 0x2077dc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[61][51], 0xf489); /* 0x2077dcc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][0], 0x12a01); /* 0x2077e00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][1], 0x1fdd3); /* 0x2077e04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][2], 0x13507); /* 0x2077e08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][3], 0xc040); /* 0x2077e0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][4], 0x26bf1); /* 0x2077e10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][5], 0x12bca); /* 0x2077e14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][6], 0x183da); /* 0x2077e18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][7], 0x17d88); /* 0x2077e1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][8], 0x1d7fa); /* 0x2077e20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][9], 0x10094); /* 0x2077e24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][10], 0xb52); /* 0x2077e28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][11], 0x1f93b); /* 0x2077e2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][12], 0x7d3e); /* 0x2077e30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][13], 0x11912); /* 0x2077e34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][14], 0x3d7f2); /* 0x2077e38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][15], 0x12c51); /* 0x2077e3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][16], 0x118c8); /* 0x2077e40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][17], 0x8351); /* 0x2077e44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][18], 0x8bd8); /* 0x2077e48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][19], 0x391); /* 0x2077e4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][20], 0x1eada); /* 0x2077e50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][21], 0x90e0); /* 0x2077e54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][22], 0x1589e); /* 0x2077e58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][23], 0x2aa45); /* 0x2077e5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][24], 0x29cdd); /* 0x2077e60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][25], 0x3a41d); /* 0x2077e64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][26], 0x2f466); /* 0x2077e68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][27], 0x2de90); /* 0x2077e6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][28], 0x19d25); /* 0x2077e70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][29], 0x3ab8d); /* 0x2077e74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][30], 0xfc44); /* 0x2077e78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][31], 0x91aa); /* 0x2077e7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][32], 0xb7c3); /* 0x2077e80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][33], 0x53ae); /* 0x2077e84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][34], 0x14848); /* 0x2077e88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][35], 0x3047); /* 0x2077e8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][36], 0x2c12f); /* 0x2077e90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][37], 0x34ba); /* 0x2077e94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][38], 0xa9dc); /* 0x2077e98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][39], 0xdabc); /* 0x2077e9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][40], 0x1463f); /* 0x2077ea0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][41], 0x14e8e); /* 0x2077ea4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][42], 0x31fef); /* 0x2077ea8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][43], 0xc9b3); /* 0x2077eac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][44], 0x8ed4); /* 0x2077eb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][45], 0x1f72a); /* 0x2077eb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][46], 0x1d3fa); /* 0x2077eb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][47], 0x3684c); /* 0x2077ebc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][48], 0x319f7); /* 0x2077ec0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][49], 0x14729); /* 0x2077ec4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][50], 0x3d196); /* 0x2077ec8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[62][51], 0xd74f); /* 0x2077ecc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][0], 0x34252); /* 0x2077f00 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][1], 0x2b815); /* 0x2077f04 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][2], 0x1b038); /* 0x2077f08 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][3], 0x6835); /* 0x2077f0c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][4], 0x9dc1); /* 0x2077f10 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][5], 0x1b435); /* 0x2077f14 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][6], 0x1c1a7); /* 0x2077f18 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][7], 0x36f71); /* 0x2077f1c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][8], 0xdaac); /* 0x2077f20 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][9], 0x1d1f0); /* 0x2077f24 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][10], 0x258b0); /* 0x2077f28 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][11], 0x4400); /* 0x2077f2c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][12], 0x3a95d); /* 0x2077f30 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][13], 0x2c123); /* 0x2077f34 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][14], 0x25df9); /* 0x2077f38 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][15], 0x2a30e); /* 0x2077f3c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][16], 0x38359); /* 0x2077f40 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][17], 0x1d0e3); /* 0x2077f44 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][18], 0x5def); /* 0x2077f48 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][19], 0x30b04); /* 0x2077f4c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][20], 0x2e154); /* 0x2077f50 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][21], 0x13f53); /* 0x2077f54 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][22], 0x31d4f); /* 0x2077f58 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][23], 0x3d4d1); /* 0x2077f5c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][24], 0x24896); /* 0x2077f60 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][25], 0x11edc); /* 0x2077f64 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][26], 0x354d2); /* 0x2077f68 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][27], 0x183a7); /* 0x2077f6c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][28], 0x2ad85); /* 0x2077f70 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][29], 0xdcdb); /* 0x2077f74 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][30], 0x2eb61); /* 0x2077f78 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][31], 0x3533e); /* 0x2077f7c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][32], 0x30ec5); /* 0x2077f80 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][33], 0x1707f); /* 0x2077f84 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][34], 0x1ec2c); /* 0x2077f88 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][35], 0x2e204); /* 0x2077f8c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][36], 0x8bc8); /* 0x2077f90 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][37], 0x73d3); /* 0x2077f94 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][38], 0x1ce5); /* 0x2077f98 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][39], 0x2a08a); /* 0x2077f9c */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][40], 0x17513); /* 0x2077fa0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][41], 0x16503); /* 0x2077fa4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][42], 0x199e); /* 0x2077fa8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][43], 0xf451); /* 0x2077fac */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][44], 0x3d17c); /* 0x2077fb0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][45], 0x170f2); /* 0x2077fb4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][46], 0xd1b3); /* 0x2077fb8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][47], 0x8922); /* 0x2077fbc */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][48], 0x17a17); /* 0x2077fc0 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][49], 0x16f54); /* 0x2077fc4 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][50], 0xaa0d); /* 0x2077fc8 */
    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.galois_field_matrix[63][51], 0x8928); /* 0x2077fcc */
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0], 0x29); /* 0x2070060 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0], 0xcc3); /* 0x2070060 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0][0], 0xc3); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[0][1], 0xc); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1], 0x12); /* 0x2070064 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1], 0x30c); /* 0x2070064 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1][0], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[1][1], 0x3); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2], 0x12); /* 0x2070068 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2], 0x30c); /* 0x2070068 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2][0], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[2][1], 0x3); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3], 0x8); /* 0x207006c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3], 0xc0); /* 0x207006c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3][0], 0xc0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[3][1], 0x0); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4], 0x2); /* 0x2070070 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4], 0xc); /* 0x2070070 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4][0], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[4][1], 0x0); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5], 0x29); /* 0x2070074 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5], 0xcc3); /* 0x2070074 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5][0], 0xc3); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[5][1], 0xc); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6], 0xe9); /* 0x2070078 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6], 0xfcc3); /* 0x2070078 */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6][0], 0xc3); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[6][1], 0xfc); // regs_31841 fix
// Before fix for uArch1.8:    tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7], 0xe8); /* 0x207007c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7], 0xfcc0); /* 0x207007c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7][0], 0xc0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.parity_group_mask[7][1], 0xfc); // regs_31841 fix
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[0][0], 0xc318c6); /* 0x2070000 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[0][1], 0x38d64f7); /* 0x2070004 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[1][0], 0x1717e3a); /* 0x2070008 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[1][1], 0x1799c08); /* 0x207000c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[2][0], 0x2873679); /* 0x2070010 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[2][1], 0x118013c); /* 0x2070014 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[3][0], 0x3750a06); /* 0x2070018 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[3][1], 0x3b2c015); /* 0x207001c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[4][0], 0x737fe1); /* 0x2070020 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[4][1], 0x2664016); /* 0x2070024 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[5][0], 0x332a7c7); /* 0x2070028 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[5][1], 0x33a376e); /* 0x207002c */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[6][0], 0x3737cc2); /* 0x2070030 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[6][1], 0x1cdfb26); /* 0x2070034 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[7][0], 0x188e2da); /* 0x2070038 */
// Before regs_31841 fix:     tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[7][1], 0x3441f39); /* 0x207003c */
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[0], 0x34); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[1], 0xeb); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[2], 0x29); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[3], 0x86); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[4], 0x86); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[5], 0x16); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[6], 0xf5); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[7], 0xf1); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[8], 0x30); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[9], 0xbe); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[10], 0x76); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[11], 0x5b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[12], 0x57); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[13], 0xf6); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[14], 0xd2); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[15], 0xa0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[16], 0x5f); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[17], 0x75); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[18], 0xc); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[19], 0x80); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[20], 0x7a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[21], 0x7a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[22], 0x5b); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[23], 0x85); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[24], 0xea); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[25], 0x6c); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[26], 0x89); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[27], 0x71); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[28], 0x7d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[29], 0xa6); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[30], 0x9d); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[31], 0xe5); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[32], 0x21); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[33], 0x1); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[34], 0xe4); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[35], 0xe0); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[36], 0xa3); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[37], 0xc2); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[38], 0xe2); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[39], 0x61); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[40], 0x59); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[41], 0x4a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[42], 0x43); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[43], 0x38); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[44], 0xd1); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[45], 0x67); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[46], 0x2e); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[47], 0x3a); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[48], 0xd2); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[49], 0x49); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[50], 0xef); // regs_31841 fix
  tu.OutWord(&mau_reg_map.dp.xbar_hash.hash.hash_seed[51], 0xb9); // regs_31841 fix
    tu.OutWord(&mau_reg_map.dp.hashout_ctl, 0x3de916); /* 0x2070040 */
    tu.IndirectWrite(0x020080000018, 0x0000000000000000, 0xf000000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000018 d0=0x0 d1=0xf000000000000000 */
    tu.IndirectWrite(0x020080000018, 0x0000000000000000, 0xf000000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000018 d0=0x0 d1=0xf000000000000000 */
    tu.IndirectWrite(0x020080000018, 0x00000000001d1836, 0xf000000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000018 d0=0x1d1836 d1=0xf000000000000000 */
    tu.IndirectWrite(0x020080000323, 0x0000000000000000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000323 d0=0x0 d1=0xf00000000000000 */
    tu.IndirectWrite(0x020080000323, 0x0000000000000000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000323 d0=0x0 d1=0xf00000000000000 */
    tu.IndirectWrite(0x020080000323, 0x000003fd53800000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000323 d0=0x3fd53800000 d1=0xf00000000000000 */
    tu.IndirectWrite(0x0200800001f6, 0x0000000000000000, 0x00f0000000510000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x200800001f6 d0=0x0 d1=0xf0000000510000 */
    tu.IndirectWrite(0x0200800001f6, 0x0000000000000000, 0x00f0000000510000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x200800001f6 d0=0x0 d1=0xf0000000510000 */
    tu.IndirectWrite(0x0200800001f6, 0x6359480000000000, 0x00f0000000510000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x200800001f6 d0=0x6359480000000000 d1=0xf0000000510000 */
    tu.IndirectWrite(0x020080000176, 0x0000000000000000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000176 d0=0x0 d1=0xf00000000000000 */
    tu.IndirectWrite(0x020080000176, 0x0000000000000000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000176 d0=0x0 d1=0xf00000000000000 */
    tu.IndirectWrite(0x020080000176, 0x0000030d1a200000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000176 d0=0x30d1a200000 d1=0xf00000000000000 */
    tu.IndirectWrite(0x02008000024d, 0x0000000000000000, 0x00f0000000310000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x2008000024d d0=0x0 d1=0xf0000000310000 */
    tu.IndirectWrite(0x02008000024d, 0x0000000000000000, 0x00f0000000310000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x2008000024d d0=0x0 d1=0xf0000000310000 */
    tu.IndirectWrite(0x02008000024d, 0x5ef8240000000000, 0x00f0000000310000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x2008000024d d0=0x5ef8240000000000 d1=0xf0000000310000 */
    tu.IndirectWrite(0x020080000325, 0x0000000000000000, 0x00f0000000210000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000325 d0=0x0 d1=0xf0000000210000 */
    tu.IndirectWrite(0x020080000325, 0x0000000000000000, 0x00f0000000210000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000325 d0=0x0 d1=0xf0000000210000 */
    tu.IndirectWrite(0x020080000325, 0x50c2480000000000, 0x00f0000000210000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000325 d0=0x50c2480000000000 d1=0xf0000000210000 */
    tu.IndirectWrite(0x0200800002e4, 0x0000000000000000, 0x00f0000000e70000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x200800002e4 d0=0x0 d1=0xf0000000e70000 */
    tu.IndirectWrite(0x0200800002e4, 0x0000000000000000, 0x00f0000000e70000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x200800002e4 d0=0x0 d1=0xf0000000e70000 */
    tu.IndirectWrite(0x0200800002e4, 0x4a88400000000000, 0x00f0000000e70000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x200800002e4 d0=0x4a88400000000000 d1=0xf0000000e70000 */
    tu.IndirectWrite(0x020080000248, 0x0000000000000000, 0xf000000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000248 d0=0x0 d1=0xf000000000000000 */
    tu.IndirectWrite(0x020080000248, 0x0000000000000000, 0xf000000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000248 d0=0x0 d1=0xf000000000000000 */
    tu.IndirectWrite(0x020080000248, 0x00000000001a15a8, 0xf000000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000248 d0=0x1a15a8 d1=0xf000000000000000 */
    tu.IndirectWrite(0x020080000234, 0x0000000000000000, 0xf000000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000234 d0=0x0 d1=0xf000000000000000 */
    tu.IndirectWrite(0x020080000234, 0x0000000000000000, 0xf000000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000234 d0=0x0 d1=0xf000000000000000 */
    tu.IndirectWrite(0x020080000234, 0x00000000001435e5, 0xf000000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000234 d0=0x1435e5 d1=0xf000000000000000 */
    tu.IndirectWrite(0x020080000057, 0x0000000000000000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000057 d0=0x0 d1=0xf00000000000000 */
    tu.IndirectWrite(0x020080000057, 0x0000000000000000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000057 d0=0x0 d1=0xf00000000000000 */
    tu.IndirectWrite(0x020080000057, 0x000002ce24a00000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000057 d0=0x2ce24a00000 d1=0xf00000000000000 */
    tu.IndirectWrite(0x0200800000b7, 0x0000000000000000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x200800000b7 d0=0x0 d1=0xf00000000000000 */
    tu.IndirectWrite(0x0200800000b7, 0x0000000000000000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x200800000b7 d0=0x0 d1=0xf00000000000000 */
    tu.IndirectWrite(0x0200800000b7, 0x0000023e33a00000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x200800000b7 d0=0x23e33a00000 d1=0xf00000000000000 */
    tu.IndirectWrite(0x020080000363, 0x0000000000000000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000363 d0=0x0 d1=0xf00000000000000 */
    tu.IndirectWrite(0x020080000363, 0x0000000000000000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000363 d0=0x0 d1=0xf00000000000000 */
    tu.IndirectWrite(0x020080000363, 0x0000024024400000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000363 d0=0x24024400000 d1=0xf00000000000000 */
    tu.IndirectWrite(0x020080000382, 0x0000000000000000, 0xf000000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000382 d0=0x0 d1=0xf000000000000000 */
    tu.IndirectWrite(0x020080000382, 0x0000000000000000, 0xf000000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000382 d0=0x0 d1=0xf000000000000000 */
    tu.IndirectWrite(0x020080000382, 0x0000000000148da0, 0xf000000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000382 d0=0x148da0 d1=0xf000000000000000 */
    tu.IndirectWrite(0x0200800002ea, 0x0000000000000000, 0x00f0000000100000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x200800002ea d0=0x0 d1=0xf0000000100000 */
    tu.IndirectWrite(0x0200800002ea, 0x0000000000000000, 0x00f0000000100000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x200800002ea d0=0x0 d1=0xf0000000100000 */
    tu.IndirectWrite(0x0200800002ea, 0x66fd780000000000, 0x00f0000000100000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x200800002ea d0=0x66fd780000000000 d1=0xf0000000100000 */
    tu.IndirectWrite(0x020080000069, 0x0000000000000000, 0x00f0000000210000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000069 d0=0x0 d1=0xf0000000210000 */
    tu.IndirectWrite(0x020080000069, 0x0000000000000000, 0x00f0000000210000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000069 d0=0x0 d1=0xf0000000210000 */
    tu.IndirectWrite(0x020080000069, 0x7814040000000000, 0x00f0000000210000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000069 d0=0x7814040000000000 d1=0xf0000000210000 */
    tu.IndirectWrite(0x020080000301, 0x0000000000000000, 0x00f0000000a70000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000301 d0=0x0 d1=0xf0000000a70000 */
    tu.IndirectWrite(0x020080000301, 0x0000000000000000, 0x00f0000000a70000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000301 d0=0x0 d1=0xf0000000a70000 */
    tu.IndirectWrite(0x020080000301, 0x7ccfb00000000000, 0x00f0000000a70000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000301 d0=0x7ccfb00000000000 d1=0xf0000000a70000 */
    tu.IndirectWrite(0x02008000035e, 0x0000000000000000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x2008000035e d0=0x0 d1=0xf00000000000000 */
    tu.IndirectWrite(0x02008000035e, 0x0000000000000000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x2008000035e d0=0x0 d1=0xf00000000000000 */
    tu.IndirectWrite(0x02008000035e, 0x0000036b54400000, 0x0f00000000000000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x2008000035e d0=0x36b54400000 d1=0xf00000000000000 */
    tu.IndirectWrite(0x020080000018, 0x00000000001d1836, 0xf0f0000000380000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000018 d0=0x1d1836 d1=0xf0f0000000380000 */
    tu.IndirectWrite(0x020080000018, 0x00000000001d1836, 0xf0f0000000380000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000018 d0=0x1d1836 d1=0xf0f0000000380000 */
    tu.IndirectWrite(0x020080000018, 0x5df02800001d1836, 0xf0f0000000380000); /* pipe0.stage0.mau_cfg.mau_array_cfg.ematch_array.EM1_way0_wid0_dep0: a=0x20080000018 d0=0x5df02800001d1836 d1=0xf0f0000000380000 */



  act_hv_translator.do_writes(&tu);
    Phv *phv_in2 = tu.phv_alloc();
    phv_in2->set_ingress();
    phv_in2->set_version(0x3);
    phv_in2->set(  3, 0x7d96dc67); 	/* [0, 3] v=1  bytes: 15- 12  #11e0# RefModel */
    phv_in2->set(  5, 0x85eca4f8); 	/* [0, 5] v=1  bytes: 23- 20  #11e0# RefModel */
    phv_in2->set(  9, 0x6b20b339); 	/* [0, 9] v=1  bytes: 39- 36  #11e0# RefModel */
    phv_in2->set( 10, 0x02ce7cd8); 	/* [0,10] v=1  bytes: 43- 40  #11e0# RefModel */
    phv_in2->set( 14, 0xde8a6cf7); 	/* [0,14] v=1  bytes: 59- 56  #11e0# RefModel */
    phv_in2->set( 18, 0x8c0472df); 	/* [0,18] v=1  bytes: 75- 72  #11e0# RefModel */
    phv_in2->set( 21, 0x45891a5d); 	/* [0,21] v=1  bytes: 87- 84  #11e0# RefModel */
    phv_in2->set( 25, 0xe4d8a194); 	/* [0,25] v=1  bytes:103-100  #11e0# RefModel */
    phv_in2->set( 29, 0xe3b7dd13); 	/* [0,29] v=1  bytes:119-116  #11e0# RefModel */
    phv_in2->set( 31, 0x59b08a20); 	/* [0,31] v=1  bytes:127-124  #11e0# RefModel */
    phv_in2->set( 33, 0x405e33fa); 	/* [1, 1] v=1  bytes:135-132  #11e0# RefModel */
    phv_in2->set( 34, 0x1aa69e82); 	/* [1, 2] v=1  bytes:139-136  #11e0# RefModel */
    phv_in2->set( 35, 0x6c0624d2); 	/* [1, 3] v=1  bytes:143-140  #11e0# RefModel */
    phv_in2->set( 36, 0xf6a39d98); 	/* [1, 4] v=1  bytes:147-144  #11e0# RefModel */
    phv_in2->set( 37, 0xff60806a); 	/* [1, 5] v=1  bytes:151-148  #11e0# RefModel */
    phv_in2->set( 38, 0x336e76e2); 	/* [1, 6] v=1  bytes:155-152  #11e0# RefModel */
    phv_in2->set( 39, 0x289b0c70); 	/* [1, 7] v=1  bytes:159-156  #11e0# RefModel */
    phv_in2->set( 40, 0x2165de35); 	/* [1, 8] v=1  bytes:163-160  #11e0# RefModel */
    phv_in2->set( 42, 0xaafba74f); 	/* [1,10] v=1  bytes:171-168  #11e0# RefModel */
    phv_in2->set( 43, 0xa5f84c14); 	/* [1,11] v=1  bytes:175-172  #11e0# RefModel */
    phv_in2->set( 44, 0xade5d586); 	/* [1,12] v=1  bytes:179-176  #11e0# RefModel */
    phv_in2->set( 45, 0xbb8cd0cd); 	/* [1,13] v=1  bytes:183-180  #11e0# RefModel */
    phv_in2->set( 46, 0x8db22568); 	/* [1,14] v=1  bytes:187-184  #11e0# RefModel */
    phv_in2->set( 48, 0xefdfa8ae); 	/* [1,16] v=1  bytes:195-192  #11e0# RefModel */
    phv_in2->set( 50, 0xb0be35d5); 	/* [1,18] v=1  bytes:203-200  #11e0# RefModel */
    phv_in2->set( 51, 0xfe44b5c9); 	/* [1,19] v=1  bytes:207-204  #11e0# RefModel */
    phv_in2->set( 54, 0x139f89a7); 	/* [1,22] v=1  bytes:219-216  #11e0# RefModel */
    phv_in2->set( 56, 0x170d76a8); 	/* [1,24] v=1  bytes:227-224  #11e0# RefModel */
    phv_in2->set( 61, 0x1fbb91d2); 	/* [1,29] v=1  bytes:247-244  #11e0# RefModel */
    phv_in2->set( 65, 0x1c); 	/* [2, 1] v=1  bytes:257     #11e0# RefModel */
    phv_in2->set( 66, 0x21); 	/* [2, 2] v=1  bytes:258     #11e0# RefModel */
    phv_in2->set( 67, 0xbc); 	/* [2, 3] v=1  bytes:259     #11e0# RefModel */
    phv_in2->set( 68, 0x3a); 	/* [2, 4] v=1  bytes:260     #11e0# RefModel */
    phv_in2->set( 71, 0x65); 	/* [2, 7] v=1  bytes:263     #11e0# RefModel */
    phv_in2->set( 73, 0xb0); 	/* [2, 9] v=1  bytes:265     #11e0# RefModel */
    phv_in2->set( 74, 0x88); 	/* [2,10] v=1  bytes:266     #11e0# RefModel */
    phv_in2->set( 76, 0x80); 	/* [2,12] v=1  bytes:268     #11e0# RefModel */
    phv_in2->set( 78, 0x80); 	/* [2,14] v=1  bytes:270     #11e0# RefModel */
    phv_in2->set( 79, 0x48); 	/* [2,15] v=1  bytes:271     #11e0# RefModel */
    phv_in2->set( 80, 0xce); 	/* [2,16] v=1  bytes:272     #11e0# RefModel */
    phv_in2->set( 82, 0xcf); 	/* [2,18] v=1  bytes:274     #11e0# RefModel */
    phv_in2->set( 83, 0xf3); 	/* [2,19] v=1  bytes:275     #11e0# RefModel */
    phv_in2->set( 85, 0xb3); 	/* [2,21] v=1  bytes:277     #11e0# RefModel */
    phv_in2->set( 87, 0x6a); 	/* [2,23] v=1  bytes:279     #11e0# RefModel */
    phv_in2->set( 90, 0x3e); 	/* [2,26] v=1  bytes:282     #11e0# RefModel */
    phv_in2->set( 93, 0x1d); 	/* [2,29] v=1  bytes:285     #11e0# RefModel */
    phv_in2->set( 94, 0x5e); 	/* [2,30] v=1  bytes:286     #11e0# RefModel */
    phv_in2->set( 96, 0x5f); 	/* [3, 0] v=1  bytes:288     #11e0# RefModel */
    phv_in2->set( 97, 0x26); 	/* [3, 1] v=1  bytes:289     #11e0# RefModel */
    phv_in2->set( 99, 0x38); 	/* [3, 3] v=1  bytes:291     #11e0# RefModel */
    phv_in2->set(101, 0x58); 	/* [3, 5] v=1  bytes:293     #11e0# RefModel */
    phv_in2->set(102, 0x5c); 	/* [3, 6] v=1  bytes:294     #11e0# RefModel */
    phv_in2->set(103, 0xe2); 	/* [3, 7] v=1  bytes:295     #11e0# RefModel */
    phv_in2->set(105, 0xbc); 	/* [3, 9] v=1  bytes:297     #11e0# RefModel */
    phv_in2->set(108, 0x4c); 	/* [3,12] v=1  bytes:300     #11e0# RefModel */
    phv_in2->set(112, 0x57); 	/* [3,16] v=1  bytes:304     #11e0# RefModel */
    phv_in2->set(113, 0x3d); 	/* [3,17] v=1  bytes:305     #11e0# RefModel */
    phv_in2->set(114, 0x95); 	/* [3,18] v=1  bytes:306     #11e0# RefModel */
    phv_in2->set(116, 0xe3); 	/* [3,20] v=1  bytes:308     #11e0# RefModel */
    phv_in2->set(117, 0x77); 	/* [3,21] v=1  bytes:309     #11e0# RefModel */
    phv_in2->set(121, 0x1e); 	/* [3,25] v=1  bytes:313     #11e0# RefModel */
    phv_in2->set(125, 0xc9); 	/* [3,29] v=1  bytes:317     #11e0# RefModel */
    phv_in2->set(126, 0xfc); 	/* [3,30] v=1  bytes:318     #11e0# RefModel */
    phv_in2->set(128, 0x0696); 	/* [4, 0] v=1  bytes:321-320  #11e0# RefModel */
    phv_in2->set(130, 0xd170); 	/* [4, 2] v=1  bytes:325-324  #11e0# RefModel */
    phv_in2->set(132, 0xf3c3); 	/* [4, 4] v=1  bytes:329-328  #11e0# RefModel */
    phv_in2->set(133, 0x27cb); 	/* [4, 5] v=1  bytes:331-330  #11e0# RefModel */
    phv_in2->set(134, 0x39fc); 	/* [4, 6] v=1  bytes:333-332  #11e0# RefModel */
    phv_in2->set(136, 0xf24f); 	/* [4, 8] v=1  bytes:337-336  #11e0# RefModel */
    phv_in2->set(137, 0xa4ac); 	/* [4, 9] v=1  bytes:339-338  #11e0# RefModel */
    phv_in2->set(139, 0x8a70); 	/* [4,11] v=1  bytes:343-342  #11e0# RefModel */
    phv_in2->set(140, 0xb3e9); 	/* [4,12] v=1  bytes:345-344  #11e0# RefModel */
    phv_in2->set(141, 0xc0ce); 	/* [4,13] v=1  bytes:347-346  #11e0# RefModel */
    phv_in2->set(143, 0x5a88); 	/* [4,15] v=1  bytes:351-350  #11e0# RefModel */
    phv_in2->set(147, 0x7c5e); 	/* [4,19] v=1  bytes:359-358  #11e0# RefModel */
    phv_in2->set(148, 0xf643); 	/* [4,20] v=1  bytes:361-360  #11e0# RefModel */
    phv_in2->set(152, 0x3f40); 	/* [4,24] v=1  bytes:369-368  #11e0# RefModel */
    phv_in2->set(154, 0x9d0d); 	/* [4,26] v=1  bytes:373-372  #11e0# RefModel */
    phv_in2->set(155, 0xf826); 	/* [4,27] v=1  bytes:375-374  #11e0# RefModel */
    phv_in2->set(156, 0x2ef6); 	/* [4,28] v=1  bytes:377-376  #11e0# RefModel */
    phv_in2->set(157, 0xcadb); 	/* [4,29] v=1  bytes:379-378  #11e0# RefModel */
    phv_in2->set(158, 0xb9c3); 	/* [4,30] v=1  bytes:381-380  #11e0# RefModel */
    phv_in2->set(159, 0x33d3); 	/* [4,31] v=1  bytes:383-382  #11e0# RefModel */
    phv_in2->set(160, 0x6b0e); 	/* [5, 0] v=1  bytes:385-384  #11e0# RefModel */
    phv_in2->set(161, 0x6b4f); 	/* [5, 1] v=1  bytes:387-386  #11e0# RefModel */
    phv_in2->set(165, 0x448e); 	/* [5, 5] v=1  bytes:395-394  #11e0# RefModel */
    phv_in2->set(166, 0xa0c4); 	/* [5, 6] v=1  bytes:397-396  #11e0# RefModel */
    phv_in2->set(169, 0xe26e); 	/* [5, 9] v=1  bytes:403-402  #11e0# RefModel */
    phv_in2->set(171, 0xebc7); 	/* [5,11] v=1  bytes:407-406  #11e0# RefModel */
    phv_in2->set(173, 0x9031); 	/* [5,13] v=1  bytes:411-410  #11e0# RefModel */
    phv_in2->set(175, 0x3875); 	/* [5,15] v=1  bytes:415-414  #11e0# RefModel */
    phv_in2->set(176, 0x1dd5); 	/* [5,16] v=1  bytes:417-416  #11e0# RefModel */
    phv_in2->set(182, 0x1e20); 	/* [5,22] v=1  bytes:429-428  #11e0# RefModel */
    phv_in2->set(183, 0xd523); 	/* [5,23] v=1  bytes:431-430  #11e0# RefModel */
    phv_in2->set(186, 0x6c02); 	/* [5,26] v=1  bytes:437-436  #11e0# RefModel */
    phv_in2->set(187, 0x0caf); 	/* [5,27] v=1  bytes:439-438  #11e0# RefModel */
    phv_in2->set(189, 0x12b4); 	/* [5,29] v=1  bytes:443-442  #11e0# RefModel */
    phv_in2->set(191, 0xd5c9); 	/* [5,31] v=1  bytes:447-446  #11e0# RefModel */
    phv_in2->set(193, 0x3bb6); 	/* [6, 1] v=1  bytes:451-450  #11e0# RefModel */
    phv_in2->set(195, 0xf531); 	/* [6, 3] v=1  bytes:455-454  #11e0# RefModel */
    phv_in2->set(198, 0xa889); 	/* [6, 6] v=1  bytes:461-460  #11e0# RefModel */
    phv_in2->set(200, 0x709e); 	/* [6, 8] v=1  bytes:465-464  #11e0# RefModel */
    phv_in2->set(201, 0x3f04); 	/* [6, 9] v=1  bytes:467-466  #11e0# RefModel */
    phv_in2->set(204, 0xe086); 	/* [6,12] v=1  bytes:473-472  #11e0# RefModel */
    phv_in2->set(205, 0x01b2); 	/* [6,13] v=1  bytes:475-474  #11e0# RefModel */
    phv_in2->set(208, 0xc766); 	/* [6,16] v=1  bytes:481-480  #11e0# RefModel */
    phv_in2->set(212, 0xb897); 	/* [6,20] v=1  bytes:489-488  #11e0# RefModel */
    phv_in2->set(213, 0xe77c); 	/* [6,21] v=1  bytes:491-490  #11e0# RefModel */
    phv_in2->set(214, 0xa1e1); 	/* [6,22] v=1  bytes:493-492  #11e0# RefModel */
    phv_in2->set(216, 0x8d40); 	/* [6,24] v=1  bytes:497-496  #11e0# RefModel */
    phv_in2->set(219, 0xf308); 	/* [6,27] v=1  bytes:503-502  #11e0# RefModel */
    phv_in2->set(221, 0x6a6a); 	/* [6,29] v=1  bytes:507-506  #11e0# RefModel */
    phv_in2->set(223, 0x1265); 	/* [6,31] v=1  bytes:511-510  #11e0# RefModel */


    


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
    RMT_UT_LOG_INFO("Dv62Test::InPkt=%p [DA=%04X%08X]\n", p_in,
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
    // print out commands to test that the differences between in and out are the
    //  same as they are now.
    TestUtil::compare_phvs(phv_in2,phv_out2,true);

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
