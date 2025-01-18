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
#include <parser-static-config.h>
#include <register_includes/register_map.h>
#include <mcn_test.h>


namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

// width 0 =>  8b instructions
// width 1 => 16b instructions (or dual 8b)
// width 2 => 32b instructions (or dual 16b)
// width 3 => 64b instructions (or dual 32b)
// jbay only 4 => 128b (or 64b dual-width instruction)

void TestUtil::set_stateful_log_instruction_regs( Mau_match_merge_addrmap& mm_regs, int counter,
                                                  int width, int vpn_offset, int vpn_limit ) {

  auto a_ctl2 =  &mm_regs.mau_stateful_log_counter_ctl2[counter];
  uint32_t v_ctl2 = InWord((void*)a_ctl2);
  setp_mau_stateful_log_counter_ctl2_slog_vpn_base( &v_ctl2, vpn_offset );
  setp_mau_stateful_log_counter_ctl2_slog_vpn_limit( &v_ctl2, vpn_limit );
  setp_mau_stateful_log_counter_ctl2_slog_instruction_width( &v_ctl2, width );
  setp_mau_stateful_log_counter_ctl2_slog_counter_function( &v_ctl2, 1 /*stateful logging*/ );
  OutWord((void*)a_ctl2, v_ctl2);
}
void TestUtil::set_stateful_clear_instruction_regs( Mau_match_merge_addrmap& mm_regs, int counter,
                                                  int width, int vpn_offset, int vpn_limit ) {

  auto a_ctl2 =  &mm_regs.mau_stateful_log_counter_ctl2[counter];
  uint32_t v_ctl2 = InWord((void*)a_ctl2);
  setp_mau_stateful_log_counter_ctl2_slog_vpn_base( &v_ctl2, vpn_offset );
  setp_mau_stateful_log_counter_ctl2_slog_vpn_limit( &v_ctl2, vpn_limit );
  setp_mau_stateful_log_counter_ctl2_slog_instruction_width( &v_ctl2, width );
  setp_mau_stateful_log_counter_ctl2_slog_counter_function( &v_ctl2, 4 /*stateful clear*/ );
  OutWord((void*)a_ctl2, v_ctl2);
}

void TestUtil::stateful_cntr_config(int pipe, int stage, int log_table, int counter,
                                    int cnt_what, int cntr_shift,
                                    int vpn_min, int vpn_max) {
  int alu = counter; // use one to one mapping from alu to counter

  if ((pipe < 0) || (pipe >= kMaxPipes)) return;
  if ((stage < 0) || (stage >= kMaxStages)) return;
  if ((log_table < 0) || (log_table >= kMaxLogTabs)) return;
  if ((alu < 0) || (alu >= kMaxAlus)) return;
  if ((cnt_what < 0) || (cnt_what > 7)) return;
  if ((cntr_shift < 3) || (cntr_shift > 6)) return;
  if ((vpn_min < 0) || (vpn_max < 0) || (vpn_max > 63) || (vpn_min > vpn_max)) return;

  auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
  //auto& cfg_regs = mau_base.cfg_regs;
  //auto& adist_regs = mau_base.rams.match.adrdist;
  auto& mm_regs = mau_base.rams.match.merge;

  // First of all must configure ixbar mapping LT->Counter ([0] on end is push, TODO: allow pop)
  auto a_stateful_ixbar0 = &mm_regs.mau_stateful_log_ctl_ixbar_map[log_table / 8][0][0];
  auto a_stateful_ixbar1 = &mm_regs.mau_stateful_log_ctl_ixbar_map[log_table / 8][1][0];
  uint32_t v_stateful_ixbar = InWord((void*)a_stateful_ixbar0);
  v_stateful_ixbar &= ~(0x7 << ((log_table % 8) * 3));               // Clear cfg
  v_stateful_ixbar |=  (0x4 | (counter & 0x3)) << ((log_table % 8) * 3); // Set new ALU
  OutWord((void*)a_stateful_ixbar0, v_stateful_ixbar); // Duplicated ixbar reg...
  OutWord((void*)a_stateful_ixbar1, v_stateful_ixbar); // ...so write to both

  // Now configure instruction width corresponding to cntr_shift we're trying to achieve
  // Shift 3 => Config 0 =>  8b instructions
  // Shift 4 => Config 1 => 16b instructions (or dual 8b)
  // Shift 5 => Config 2 => 32b instructions (or dual 16b)
  // Shift 6 => Config 3 => 64b instructions (or dual 32b)
  set_stateful_log_instruction_regs( mm_regs, counter, cntr_shift-3, vpn_min, vpn_max );


  // Write ctrl reg to specify what we're counting
  // (Note in all cases table MUST be predicated ON)
  // cnt_what 0   => Disabled
  // cnt_what 1   => Count TableMiss
  // cnt_what 2   => Count TableHit
  // cnt_what 3   => Count GW Inhibit
  // cnt_what 4   => Count unconditionally
  // cnt_what 5-7 => Reserved
  auto a_stateful_ctl = &mm_regs.mau_stateful_log_counter_ctl[log_table / 8];
  uint32_t v_stateful_ctl = InWord((void*)a_stateful_ctl);
  v_stateful_ctl &= ~(0xF << ((log_table % 8) * 4));            // Clear
  v_stateful_ctl |=  (cnt_what & 0xF) << ((log_table % 8) * 4); // Set
  OutWord((void*)a_stateful_ctl, v_stateful_ctl);

  // write mau_stateful_log_counter_clear reg to reset
  // counter to initial minimum value
  auto a_stateful_clear = &mm_regs.mau_stateful_log_counter_clear;
  uint32_t v_stateful_clear = InWord((void*)a_stateful_clear);
  v_stateful_clear |= (1<<counter);
  OutWord((void*)a_stateful_clear, v_stateful_clear);

  // set counter to alu map in addr dist, at the moment 1:1 relationship,
  auto& ad_regs = mau_base.rams.match.adrdist;
  auto a_oxbar_map = &ad_regs.mau_stateful_log_counter_oxbar_map[counter];
  int oxbar_alu = counter;
  OutWord((void*)a_oxbar_map, 0x4 /*enable*/ | oxbar_alu );

  // set the mapping from counter to logical table for immediate data
  //  (controls map to logical 2x14x28b oxbar)
  auto a_logical_map = &ad_regs.mau_stateful_log_counter_logical_map[log_table];
  OutWord((void*)a_logical_map, 0x4 /*enable*/ | counter );
}


void TestUtil::set_phv_ingress_egress(int pipe, int stage, int phv_word, bool ingress) {
  if ((pipe < 0) || (pipe >= kMaxPipes)) return;
  if ((stage < 0) || (stage >= kMaxStages)) return;
  if ((phv_word < 0) || (phv_word >= kMaxPhvNum)) return;
  if (get_debug()) printf("TestUtil<%d,%d>::set_phv_ingress_egress(%d,%d)\n",
                          pipe, stage, phv_word, ingress);
  int n_groups = kMaxPhvNum / (kPhvsPerGrp/2);
  RMT_ASSERT(n_groups == 14);
  int n_bits_per_word = kMaxPhvNum / (n_groups*2);
  RMT_ASSERT((n_bits_per_word == 8) || (n_bits_per_word == 10));
  uint32_t mask_bit = 1u << (phv_word % n_bits_per_word);
  int which_group = phv_word / (kPhvsPerGrp/2); // [0..13]
  int word_index = phv_word / (kPhvsPerGrp/4);  // [0..27]
  if (which_group > 13) return;
  int halves[14]       = {  0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0,  1,  1,  1 };
  int wordindexdec[14] = {  0, 0, 4, 4, 4, 4, 8, 8, 8, 8, 8, 14, 14, 14 };
  int half = halves[which_group];
  int which_word = word_index - wordindexdec[which_group];
  RMT_ASSERT((which_word >= 0) && (which_word < 14));
  auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
  auto& dp_regs = mau_base.dp;
  auto a_ing0 = &dp_regs.phv_ingress_thread[half][which_word];
  auto a_ing1 = &dp_regs.phv_ingress_thread_imem[half][which_word];
  auto a_eg0 = &dp_regs.phv_egress_thread[half][which_word];
  auto a_eg1 = &dp_regs.phv_egress_thread_imem[half][which_word];
  auto a_actmux = &dp_regs.actionmux_din_power_ctl[half][which_word];
  auto a_mixbar = &dp_regs.match_input_xbar_din_power_ctl[half][which_word];
  uint32_t ing = InWord((void*)a_ing0);
  uint32_t eg = InWord((void*)a_eg0);
  uint32_t actmux = InWord((void*)a_actmux);
  uint32_t mixbar = InWord((void*)a_mixbar);
  actmux |= mask_bit;
  mixbar |= mask_bit;
  if (ingress) {
    if (((ing & mask_bit) == mask_bit) && ((eg & mask_bit) == 0u)) return;
    ing |= mask_bit;
    eg &= ~mask_bit;
  } else {
    if (((ing & mask_bit) == 0u) && ((eg & mask_bit) == mask_bit)) return;
    ing &= ~mask_bit;
    eg |= mask_bit;
  }
  OutWord((void*)a_ing0, ing);
  OutWord((void*)a_ing1, ing);
  OutWord((void*)a_eg0, eg);
  OutWord((void*)a_eg1, eg);
  OutWord((void*)a_actmux, actmux);
  OutWord((void*)a_mixbar, mixbar);
}

void TestUtil::set_dump_ctl_regs(int pipe, int stage) {
  auto& mau_base = MODEL_CHIP_NAMESPACE::RegisterUtils::ref_mau(pipe,stage);
  auto& cfg_regs = mau_base.cfg_regs;
  auto a_stage_dump_ctl = &cfg_regs.stage_dump_ctl;
  OutWord((void*)a_stage_dump_ctl, ((pipe & 0x3) << 5) | ((stage & 0x1F) << 0));
}

void TestUtil::set_stage_default_regs(int pipe, int stage) {
  auto& mau_base = MODEL_CHIP_NAMESPACE::RegisterUtils::ref_mau(pipe,stage);
  auto& mm_regs = mau_base.rams.match.merge;
  auto& dp_regs = mau_base.dp;
  auto a_pred_stage = &mm_regs.pred_stage_id;
  auto a_ing_mpr_stage_id = &mm_regs.mpr_stage_id[0];
  auto a_egr_mpr_stage_id = &mm_regs.mpr_stage_id[1];
  auto a_ght_mpr_stage_id = &mm_regs.mpr_stage_id[2];
  auto a_ing_pred_ctl = &mm_regs.predication_ctl[0];
  auto a_egr_pred_ctl = &mm_regs.predication_ctl[1];

  set_dump_ctl_regs(pipe, stage);
  OutWord((void*)a_pred_stage, stage);
  OutWord((void*)a_ing_mpr_stage_id, stage);
  OutWord((void*)a_egr_mpr_stage_id, stage);
  OutWord((void*)a_ght_mpr_stage_id, stage);
  int nxt_mau_ing_dep = -1;
  int nxt_mau_egr_dep = -1;
  if (stage+1 < TestUtil::kMaxStages) {
    auto& mau_base_next = MODEL_CHIP_NAMESPACE::RegisterUtils::ref_mau(pipe,stage+1);
    auto& dp_regs_next = mau_base_next.dp;
    auto a_nxt_mau_ing_dep = &dp_regs_next.cur_stage_dependency_on_prev[0];
    auto a_nxt_mau_egr_dep = &dp_regs_next.cur_stage_dependency_on_prev[1];
    nxt_mau_ing_dep = static_cast<int>(InWord((void*)a_nxt_mau_ing_dep));
    nxt_mau_egr_dep = static_cast<int>(InWord((void*)a_nxt_mau_egr_dep));
  }
  uint16_t lt_ing = static_cast<uint16_t>(InWord((void*)a_ing_pred_ctl) & 0xFFFF);
  uint16_t lt_egr = static_cast<uint16_t>(InWord((void*)a_egr_pred_ctl) & 0xFFFF);
  uint32_t mbd_ing = (nxt_mau_ing_dep > 0) ?1u :0u;
  uint32_t mbd_egr = (nxt_mau_egr_dep > 0) ?1u :0u;
  uint32_t mbd = static_cast<uint32_t>((mbd_ing << 1) | (mbd_egr << 0));
  auto a_mpr_bus_dep = &mm_regs.mpr_bus_dep;
  uint32_t v_mpr_bus_dep = InWord((void*)a_mpr_bus_dep);
  OutWord((void*)a_mpr_bus_dep, (v_mpr_bus_dep & 0xFFFC0000) | (mbd & 0x3FFFF));

  auto a_this_mau_ing_dep = &dp_regs.cur_stage_dependency_on_prev[0];
  auto a_this_mau_egr_dep = &dp_regs.cur_stage_dependency_on_prev[1];
  auto a_mpr_del_ing = &mm_regs.mpr_thread_delay[0];
  auto a_mpr_del_egr = &mm_regs.mpr_thread_delay[1];
  auto a_mpr_gex = &mm_regs.mpr_glob_exec_thread;
  auto a_mpr_lbr = &mm_regs.mpr_long_brch_thread;
  OutWord((void*)a_mpr_gex, lt_egr & ~lt_ing & 0xFFFF);
  OutWord((void*)a_mpr_lbr, 0u);

  int this_mau_ing_dep = static_cast<int>(InWord((void*)a_this_mau_ing_dep));
  int this_mau_egr_dep = static_cast<int>(InWord((void*)a_this_mau_egr_dep));
  uint8_t v_mpr_del_ing = ((stage > 0) && (this_mau_ing_dep == 0)) ?11 :0;
  uint8_t v_mpr_del_egr = ((stage > 0) && (this_mau_egr_dep == 0)) ?11 :0;
  OutWord((void*)a_mpr_del_ing, v_mpr_del_ing); // Just a non-0 val
  OutWord((void*)a_mpr_del_egr, v_mpr_del_egr); // Just a non-0 val

  if (stage == 0) {
    auto a_mpr_always_run_ing = &mm_regs.mpr_always_run;
    uint32_t v_mpr_always_run_ing = InWord((void*)a_mpr_always_run_ing);
    if (v_mpr_always_run_ing == 0) OutWord((void*)a_mpr_always_run_ing, lt_ing);
    auto a_mpr_always_run_egr = &mm_regs.mpr_always_run;
    uint32_t v_mpr_always_run_egr = InWord((void*)a_mpr_always_run_egr);
    if (v_mpr_always_run_egr == 0) OutWord((void*)a_mpr_always_run_egr, lt_egr);
  }
}

void TestUtil::set_table_default_regs(int pipe, int stage, int table) {
  auto& mau_base = MODEL_CHIP_NAMESPACE::RegisterUtils::ref_mau(pipe,stage);
  auto& mm_regs = mau_base.rams.match.merge;
  auto a_ing_pred_ctl = &mm_regs.predication_ctl[0];
  auto a_egr_pred_ctl = &mm_regs.predication_ctl[1];
  auto a_pred_is_brch = &mm_regs.pred_is_a_brch;
  uint32_t v_pred_is_brch = InWord((void*)a_pred_is_brch);
  OutWord((void*)a_pred_is_brch, v_pred_is_brch | (1u<<table));

  uint16_t lt_ing = static_cast<uint16_t>(InWord((void*)a_ing_pred_ctl) & 0xFFFF);
  uint16_t lt_egr = static_cast<uint16_t>(InWord((void*)a_egr_pred_ctl) & 0xFFFF);
  bool ingress = (((lt_ing >> table) & 1) == 1);
  bool egress  = (((lt_egr >> table) & 1) == 1);
  int ie = -1;
  if     (ingress) ie = 0;
  else if (egress) ie = 1;
  if (ie >= 0) {
    auto a_mpr_always_run = &mm_regs.mpr_always_run;
    uint32_t v_mpr_always_run = InWord((void*)a_mpr_always_run);
    OutWord((void*)a_mpr_always_run, v_mpr_always_run | (1u<<table));
  }
}


void TestUtil::set_teop_regs(int pipe, int stage, int table, int alu, int bus) {
  auto& mau_base = MODEL_CHIP_NAMESPACE::RegisterUtils::ref_mau(pipe,stage);
  auto& stats_regs = mau_base.rams.map_alu.stats_wrap[alu].stats;
  auto& ad_regs = mau_base.rams.match.adrdist;

  //auto& meter_regs = mau_base.rams.map_alu.meter_group[alu].meter;
  //auto a_meter_icxbar = &ad_regs.adr_dist_meter_adr_icxbar_ctl[table];
  //auto a_meter_paah = &ad_regs.packet_action_at_headertime[1][alu];
  //auto a_alu_teop_meter = &meter_regs.meter_ctl_teop_en;
  //auto a_dp_teop_meter = &ad_regs.dp_teop_meter_ctl[alu];
  //auto a_teop_to_meter = &ad_regs.teop_to_meter_adr_oxbar_ctl[alu];
  //auto a_meter_to_teop = &ad_regs.meter_to_teop_adr_oxbar_ctl[bus];

  auto a_stats_icxbar = &ad_regs.adr_dist_stats_adr_icxbar_ctl[table];
  auto a_stats_paah = &ad_regs.packet_action_at_headertime[0][alu];
  auto a_alu_teop_stats = &stats_regs.statistics_ctl_teop_en;
  auto a_dp_teop_stats = &ad_regs.dp_teop_stats_ctl[alu];
  auto a_teop_to_stats = &ad_regs.teop_to_stats_adr_oxbar_ctl[alu];
  auto a_stats_to_teop = &ad_regs.stats_to_teop_adr_oxbar_ctl[bus];

  auto a_teop_bus_ctl =  &ad_regs.teop_bus_ctl[bus];

  uint32_t delay = 3;
  //uint32_t v_teop_bus_ctl = InWord((void*)a_teop_bus_ctl);

  //uint32_t v_meter_icxbar = InWord((void*)a_meter_icxbar);
  //uint32_t v_alu_teop_meter = InWord((void*)a_alu_teop_meter);
  //uint32_t v_dp_teop_meter = InWord((void*)a_dp_teop_meter);
  //uint32_t v_teop_to_meter = InWord((void*)a_teop_to_meter);
  //uint32_t v_meter_to_teop = InWord((void*)a_meter_to_teop);

  uint32_t v_stats_icxbar = InWord((void*)a_stats_icxbar);
  //uint32_t v_alu_teop_stats = InWord((void*)a_alu_teop_stats);
  //uint32_t v_dp_teop_stats = InWord((void*)a_dp_teop_stats);
  //uint32_t v_teop_to_stats = InWord((void*)a_teop_to_stats);
  //uint32_t v_stats_to_teop = InWord((void*)a_stats_to_teop);

  // Setup ALU in adr_dist_stats_icxbar[LT]
  OutWord((void*)a_stats_icxbar, v_stats_icxbar | (1u<<alu));
  // Switch off packet_action_at_headertime for Stats ALU
  OutWord((void*)a_stats_paah, 0);
  // Enable StatsALU for TEOP
  OutWord((void*)a_alu_teop_stats, 1u);
  // Set Datapath to get stuff BUS->ALU
  OutWord((void*)a_dp_teop_stats, (1u<<6) | (2u<<3)); //RxEN RxShift=2
  // Enable BUS->ALU inbound
  OutWord((void*)a_teop_to_stats, (1u<<2) | (bus & 3));
  // Enable ALU->BUS outbound
  OutWord((void*)a_stats_to_teop, (1u<<2) | (alu & 3));
  // Configure TEOP bus for STATS
  OutWord((void*)a_teop_bus_ctl, (1u<<7) | (1u<<5) | (delay & 0x1F));
}


// Some funcs to setup JBay mirror CSRs

void TestUtil::set_mirror_global(int pipe,
                                 bool ing_en, bool egr_en,
                                 bool coal_ing_en, bool coal_egr_en,
                                 uint16_t coal_baseid, uint32_t coal_basetime) {
    assert((pipe >= 0) && (pipe < kMaxPipes));
    assert(coal_baseid < RmtDefs::kMirrorNormalSessions);
    // On JBay timeout is CSR+1 so decrement.
    // But leave 0 alone (==> disable) and leave 1 alone (as 1->0 would disable)
    if (coal_basetime >= 2) coal_basetime--;

    uint32_t base_time = 0u;
    setp_mirr_s2p_coal_to_interval_r_val(&base_time, coal_basetime);

    auto mir = RegisterUtils::addr_mirbuf(pipe);
    OutWord(&mir->mirror.s2p_regs.coal_to_interval, base_time);

    // NO other global enables on JBay so nowt else to do
}

void TestUtil::set_mirror_meta(int pipe, int sess,
                               uint32_t egr_port, uint32_t egr_port_v,
                               uint32_t eport_qid, uint32_t icos,
                               uint32_t pipe_mask, uint32_t color,
                               uint32_t hash1, uint32_t hash2,
                               uint32_t mgid1, uint32_t mgid1_v,
                               uint32_t mgid2, uint32_t mgid2_v,
                               uint32_t c2c_v, uint32_t c2c_cos,
                               uint32_t xid, uint32_t yid, uint32_t rid,
                               uint32_t egress_bypass, uint32_t yid_tbl_sel,
                               uint32_t deflect)
{
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((sess >= 0) && (sess < RmtDefs::kMirrorNormalSessions));

  // Here we need to write cfg into 5x MirrS2pSessCfgWord{X}RMutable CSR regs
  // where X = 0,1,2,3,4, and then when done write to word X [0.255] in
  // the MirrS2pSessRArray CSR. This causes the 5 words to get written into
  // entry X in a shadow RAM. This mechanism allows for atomic update.

  auto mir = RegisterUtils::addr_mirbuf(pipe);
  uint32_t w0 = InWord((void*)&mir->mirror.s2p_regs.sess_entry_word0);
  uint32_t w1 = InWord((void*)&mir->mirror.s2p_regs.sess_entry_word1);
  uint32_t w2 = InWord((void*)&mir->mirror.s2p_regs.sess_entry_word2);
  uint32_t w3 = InWord((void*)&mir->mirror.s2p_regs.sess_entry_word3);
  uint32_t w4 = InWord((void*)&mir->mirror.s2p_regs.sess_entry_word4);

  setp_mirr_s2p_sess_cfg_word2_r_epipe_port_f(&w2, egr_port);
  setp_mirr_s2p_sess_cfg_word2_r_epipe_port_vld_f(&w2, egr_port_v);

  setp_mirr_s2p_sess_cfg_word4_r_eport_qid_f(&w4, eport_qid);
  setp_mirr_s2p_sess_cfg_word4_r_icos_f(&w4, icos);

#if MCN_TEST(MODEL_CHIP_NAMESPACE, rsvd0)
  // XXX: WIP mirror doesn't have pipe_vec
  // for WIP set the given pipe_mask in first tm wac pvt table
  set_multicast_pipe_vector(pipe, 0, mgid1, pipe_mask);
#else
  // for jbay set pipe mask in the mirror session regs
  setp_mirr_s2p_sess_cfg_word3_r_pipe_vec_f(&w3, pipe_mask);
#endif
  setp_mirr_s2p_sess_cfg_word4_r_color_f(&w4, color);

  setp_mirr_s2p_sess_cfg_word3_r_hash1_f(&w3, hash1);
  setp_mirr_s2p_sess_cfg_word3_r_hash2_f(&w3, hash2);

  setp_mirr_s2p_sess_cfg_word1_r_mcid1_id_f(&w1, mgid1);
  setp_mirr_s2p_sess_cfg_word4_r_mcid1_vld_f(&w4, mgid1_v);

  setp_mirr_s2p_sess_cfg_word1_r_mcid2_id_f(&w1, mgid2);
  setp_mirr_s2p_sess_cfg_word4_r_mcid2_vld_f(&w4, mgid2_v);

  setp_mirr_s2p_sess_cfg_word4_r_c2c_vld_f(&w4, c2c_v);
  setp_mirr_s2p_sess_cfg_word4_r_c2c_cos_f(&w4, c2c_cos);

  setp_mirr_s2p_sess_cfg_word0_r_xid_f(&w0, xid);
#if MCN_TEST(MODEL_CHIP_NAMESPACE, rsvd0)
  setp_mirr_s2p_sess_cfg_word4_r_yid_f(&w4, yid);
#else
  setp_mirr_s2p_sess_cfg_word0_r_yid_f(&w0, yid);
#endif
  setp_mirr_s2p_sess_cfg_word2_r_rid_f(&w2, rid);
  setp_mirr_s2p_sess_cfg_word3_r_egress_bypass_f(&w3, egress_bypass);
  setp_mirr_s2p_sess_cfg_word4_r_yid_tbl_sel_f(&w4, yid_tbl_sel);

  setp_mirr_s2p_sess_cfg_word2_r_def_on_drop_f(&w2, deflect);

  OutWord(&mir->mirror.s2p_regs.sess_entry_word0, w0);
  OutWord(&mir->mirror.s2p_regs.sess_entry_word1, w1);
  OutWord(&mir->mirror.s2p_regs.sess_entry_word2, w2);
  OutWord(&mir->mirror.s2p_regs.sess_entry_word3, w3);
  OutWord(&mir->mirror.s2p_regs.sess_entry_word4, w4);

  // Trigger copy into shadow RAM
  OutWord(&mir->mirror.s2p_sess.tbl0[sess], 1);
}
void TestUtil::set_mirror_meta_cfg(int pipe, int sess,
                                   bool hash_cfg, bool icos_cfg, bool dod_cfg,
                                   bool c2c_cfg, bool mc_cfg, bool epipe_cfg) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((sess >= 0) && (sess < RmtDefs::kMirrorNormalSessions));

  auto mir = RegisterUtils::addr_mirbuf(pipe);
  uint32_t w0 = InWord((void*)&mir->mirror.s2p_regs.sess_entry_word0);
  //uint32_t w1 = InWord((void*)&mir->mirror.s2p_regs.sess_entry_word1);
  //uint32_t w2 = InWord((void*)&mir->mirror.s2p_regs.sess_entry_word2);
  //uint32_t w3 = InWord((void*)&mir->mirror.s2p_regs.sess_entry_word3);
  //uint32_t w4 = InWord((void*)&mir->mirror.s2p_regs.sess_entry_word4);

  // NB hash_cfg[0]==1 ==> get from MAU
  //    hash_cfg[1]==1 ==> repace hash1, hash_cfg[1]==0 ==> replace hash2
  // So here we're always replacing hash2
  setp_mirr_s2p_sess_cfg_word0_r_hash_cfg_f(&w0, hash_cfg?1:0);
  setp_mirr_s2p_sess_cfg_word0_r_icos_cfg_f(&w0, icos_cfg?1:0);
  setp_mirr_s2p_sess_cfg_word0_r_dod_cfg_f(&w0, dod_cfg?1:0);
  setp_mirr_s2p_sess_cfg_word0_r_c2c_cfg_f(&w0, c2c_cfg?1:0);
  setp_mirr_s2p_sess_cfg_word0_r_mc_cfg_f(&w0, mc_cfg?1:0);
  setp_mirr_s2p_sess_cfg_word0_r_epipe_cfg_f(&w0, epipe_cfg?1:0);

  OutWord(&mir->mirror.s2p_regs.sess_entry_word0, w0);
  //OutWord(&mir->mirror.s2p_regs.sess_entry_word1, w1);
  //OutWord(&mir->mirror.s2p_regs.sess_entry_word2, w2);
  //OutWord(&mir->mirror.s2p_regs.sess_entry_word3, w3);
  //OutWord(&mir->mirror.s2p_regs.sess_entry_word4, w4);

  // Trigger copy into shadow RAM
  OutWord(&mir->mirror.s2p_sess.tbl0[sess], 1);
}

void TestUtil::set_mirror_norm_session(int pipe, int slice, int sess,
                                       bool ing_en, bool egr_en,
                                       bool coal_en, uint8_t coal_num,
                                       uint8_t pri, uint8_t max_n,
                                       uint16_t trunc_size) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((slice >= 0) && (slice < RmtDefs::kMirrorSlices));
  assert((sess >= 0) && (sess < RmtDefs::kMirrorNormalSessions));

  uint32_t csr = 0u;
  setp_mirror_sess_entry_r_sess_en(&csr, (ing_en || egr_en)?1:0);
  setp_mirror_sess_entry_r_ingr_en(&csr, ing_en?1:0);
  setp_mirror_sess_entry_r_egr_en(&csr, egr_en?1:0);
  setp_mirror_sess_entry_r_coal_en(&csr, coal_en?1:0);
  setp_mirror_sess_entry_r_coal_num(&csr, coal_num);
  setp_mirror_sess_entry_r_pkt_len(&csr, trunc_size);

  auto mir = RegisterUtils::addr_mirbuf(pipe);
  OutWord(&mir->mirror.slice_mem[slice].sess_cfg.entry[sess], csr);
}
void TestUtil::set_mirror_coal_session(int pipe, int slice, int coal_sess,
                                       bool en, uint8_t ver, uint8_t pri,
                                       uint8_t pkt_hdr_len, uint16_t min_pkt_size,
                                       uint16_t extract_len,
                                       bool len_from_inp, bool tofino_mode,
                                       uint32_t hdr0, uint32_t hdr1,
                                       uint32_t hdr2, uint32_t hdr3,
                                       uint32_t coal_timeout) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((slice >= 0) && (slice < RmtDefs::kMirrorSlices));
  assert((coal_sess >= 0) && (coal_sess < RmtDefs::kMirrorCoalSessions));
  assert((pkt_hdr_len == 8) || (pkt_hdr_len == 12) || (pkt_hdr_len == 16));
  // Insist hdr vals 0 unless Slice0
  assert((slice == 0) || ((hdr0 == 0u) && (hdr1 == 0u) && (hdr2 == 0u) && (hdr3 == 0u)));

  uint32_t csr = 0u;
  setp_mirror_coal_sess_entry_r_coal_hdr(&csr, (pkt_hdr_len/4)-2);
  setp_mirror_coal_sess_entry_r_len_cfg(&csr, (len_from_inp)?1:0);
  setp_mirror_coal_sess_entry_r_coal_mode(&csr, (tofino_mode)?1:0);
  setp_mirror_coal_sess_entry_r_sample_pkt_len(&csr, (extract_len/4));
  setp_mirror_coal_sess_entry_r_pri(&csr, pri);

  uint32_t timeo = 0u;
  setp_mirror_coal_to_entry_r_coal_timeout(&timeo, coal_timeout);

  auto mir = RegisterUtils::addr_mirbuf(pipe);
  OutWord(&mir->mirror.slice_mem[slice].coal_sess_cfg.entry[coal_sess], csr);
  OutWord(&mir->mirror.slice_mem[slice].coal_to_cfg.entry[coal_sess], timeo);

  if (slice != 0) return;
  // Only write config below when called with Slice == 0 as these CSRs are NOT per-slice
  // NOTE odd ordering of register names vs header words hdr 0,1,2,3 -> CSR 0,3,1,2
  OutWord(&mir->mirror.s2p_coal.coal_hdr_tbl[coal_sess].coal_hdr_tbl_0_4, hdr0);
  OutWord(&mir->mirror.s2p_coal.coal_hdr_tbl[coal_sess].coal_hdr_tbl_3_4, hdr1);
  OutWord(&mir->mirror.s2p_coal.coal_hdr_tbl[coal_sess].coal_hdr_tbl_1_4, hdr2);
  OutWord(&mir->mirror.s2p_coal.coal_hdr_tbl[coal_sess].coal_hdr_tbl_2_4, hdr3);
}



// Some funcs to setup JBay PacketGen CSRs

void TestUtil::pktgen_app_ctrl_set(int pipe, int app,
                                   bool en, uint8_t type, uint8_t chan, uint8_t prio, uint8_t flg) {
  // Type is 0->timer 1->periodic 2->linkdown 3->recirc 4->dprsr 5->PFC
  // Chan is what chan app belongs to
  // - pktgen pkt ONLY to that chan - but this IGNORED for recirc
  // Prio ignored for now
  // Flags are NoKey|StopAtPktBndry|UsePortDownMask1|UseCurrTs
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((app >= 0) && (app < 16));
  assert((type >= kPgenAppTypeMin) && (type <= kPgenAppTypeMax));
  assert((chan >= kPgenAppChanMin) && (chan <= kPgenAppChanMax));
  uint32_t v = 0u;
  setp_pgr_app_ctrl_app_en  (&v, en ?1u:0u);
  setp_pgr_app_ctrl_app_type(&v, static_cast<uint32_t>(type));
  setp_pgr_app_ctrl_app_chnl(&v, static_cast<uint32_t>(chan));
  setp_pgr_app_ctrl_app_prio(&v, static_cast<uint32_t>(prio));
  setp_pgr_app_ctrl_app_nokey             (&v, (((flg & kPgenAppFlagNoKey)!=0)            ?1u:0u));
  setp_pgr_app_ctrl_app_stop_at_pkt_bndry (&v, (((flg & kPgenAppFlagStopAtPktBndry)!=0)   ?1u:0u));
  setp_pgr_app_ctrl_app_port_down_mask_sel(&v, (((flg & kPgenAppFlagUsePortDownMask1)!=0) ?1u:0u));
#ifdef MODEL_CHIP_JBAYXX
  // WIP had this removed in bfnregs 20190929_140119_87870_main_cb
  setp_pgr_app_ctrl_app_sel_curr_timestamp(&v, (((flg & kPgenAppFlagUseCurrTs)!=0)        ?1u:0u));
#endif
  auto pgr = RegisterUtils::addr_pgr(pipe);
  OutWord(&pgr->pgr_app[app].ctrl, v);
}

void TestUtil::pktgen_app_payload_ctrl_set(int pipe, int app,
                                           uint16_t addr_16B, uint16_t size_B, bool from_recirc) {
  // from_recirc determines whether addr[6:0] ([0-12]) is addr in
  //             recirc packet from which to extract addr_16B/size_B
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((app >= 0) && (app < 16));
  uint64_t eltsz = MemUtils::pktgen_buffer_mem_word_array_element_size(pipe);
  uint64_t max = MemUtils::pktgen_buffer_mem_word_array_count(pipe);
  if (from_recirc) {
    assert((addr_16B & 0x7F) < 13);
  } else {
    uint64_t last_B = (static_cast<uint64_t>(addr_16B) * UINT64_C(16)) + static_cast<uint64_t>(size_B);
    assert((addr_16B < max) && (size_B >= 64) && (size_B <= (max*eltsz)));
    assert( last_B <= (max*eltsz) );
  }
  uint32_t v = 0u;
  setp_pgr_app_payload_ctrl_app_payload_addr  (&v, static_cast<uint32_t>(addr_16B));
  setp_pgr_app_payload_ctrl_app_payload_size  (&v, static_cast<uint32_t>(size_B));
  setp_pgr_app_payload_ctrl_app_recirc_extract(&v, from_recirc ?1u:0u);
  auto pgr = RegisterUtils::addr_pgr(pipe);
  OutWord(&pgr->pgr_app[app].payload_ctrl, v);
}

void TestUtil::pktgen_app_ingr_port_ctrl_set(int pipe, int app,
                                             uint8_t ing_port, bool ing_inc, uint8_t ing_wrap,
                                             uint8_t ing_port_pipe_id) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((app >= 0) && (app < 16));
  assert(ing_port <= 72);
  assert(ing_wrap <= 72);
  assert(ing_port_pipe_id < kMaxPipes);
  uint32_t v = 0u;
  setp_pgr_app_ingr_port_ctrl_app_ingr_port        (&v, static_cast<uint32_t>(ing_port));
  setp_pgr_app_ingr_port_ctrl_app_ingr_port_inc    (&v, ing_inc ?1u: 0u);
  setp_pgr_app_ingr_port_ctrl_app_ingr_port_wrap   (&v, ing_wrap);
  setp_pgr_app_ingr_port_ctrl_app_ingr_port_pipe_id(&v, static_cast<uint32_t>(ing_port_pipe_id));
  auto pgr = RegisterUtils::addr_pgr(pipe);
  OutWord(&pgr->pgr_app[app].ingr_port_ctrl, v);
}

void TestUtil::pktgen_app_recirc_set(int pipe, int app, int outport,
                                     const BitVector<128> &value, const BitVector<128> &mask) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((app >= 0) && (app < 16));
  assert((outport >= kPgenOutPortEbuf0) && (outport <= kPgenOutPortEbuf3));
  auto pgr = RegisterUtils::addr_pgr(pipe);
  if (get_debug()) printf("TestUtil::pktgen_app_recirc_set(%d,%d,%d) "
                          "val=0x%016" PRIx64 "%016" PRIx64 " mask=0x%016" PRIx64 "%016" PRIx64 "\n",
                          pipe, app, outport,
                          value.get_word(64),value.get_word(0),
                          mask.get_word(64),mask.get_word(0));

  uint64_t vlo64  = value.get_word(0), vhi64 = value.get_word(64);
  uint32_t v0 = static_cast<uint32_t>( (vlo64 >>  0) & UINT64_C(0xFFFFFFFF));
  uint32_t v1 = static_cast<uint32_t>( (vlo64 >> 32) & UINT64_C(0xFFFFFFFF));
  uint32_t v2 = static_cast<uint32_t>( (vhi64 >>  0) & UINT64_C(0xFFFFFFFF));
  uint32_t v3 = static_cast<uint32_t>( (vhi64 >> 32) & UINT64_C(0xFFFFFFFF));
  OutWord(&pgr->pgr_app[app].recir_match_value.recir_match_value_0_4, v0);
  OutWord(&pgr->pgr_app[app].recir_match_value.recir_match_value_1_4, v1);
  OutWord(&pgr->pgr_app[app].recir_match_value.recir_match_value_2_4, v2);
  OutWord(&pgr->pgr_app[app].recir_match_value.recir_match_value_3_4, v3);

  uint64_t mlo64  = mask.get_word(0), mhi64 = mask.get_word(64);
  // Invert as 1 means don't care (not really a mask!)
  uint32_t m0 = ~ static_cast<uint32_t>( (mlo64 >>  0) & UINT64_C(0xFFFFFFFF));
  uint32_t m1 = ~ static_cast<uint32_t>( (mlo64 >> 32) & UINT64_C(0xFFFFFFFF));
  uint32_t m2 = ~ static_cast<uint32_t>( (mhi64 >>  0) & UINT64_C(0xFFFFFFFF));
  uint32_t m3 = ~ static_cast<uint32_t>( (mhi64 >> 32) & UINT64_C(0xFFFFFFFF));

  // Tie app to particular recirc src [EBuf0,..,Ebuf3]
  uint32_t v = InWord((void*)&pgr->pgr_common.cfg_app_recirc_src);
  setp_pgr_cfg_app_recirc_port_src_app_recirc_src(&v, app, outport);
  OutWord(&pgr->pgr_common.cfg_app_recirc_src, v);
  OutWord(&pgr->pgr_app[app].recir_match_mask.recir_match_mask_0_4, m0);
  OutWord(&pgr->pgr_app[app].recir_match_mask.recir_match_mask_1_4, m1);
  OutWord(&pgr->pgr_app[app].recir_match_mask.recir_match_mask_2_4, m2);
  OutWord(&pgr->pgr_app[app].recir_match_mask.recir_match_mask_3_4, m3);
}

void TestUtil::pktgen_app_event_set(int pipe, int app, uint16_t batchnum, uint16_t pktnum,
                                    uint32_t ibg_jit_base, uint8_t ibg_jit_max, uint8_t ibg_jit_scl,
                                    uint32_t ipg_jit_base, uint8_t ipg_jit_max, uint8_t ipg_jit_scl,
                                    uint32_t timer_cycles) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((app >= 0) && (app < 16));
  auto pgr = RegisterUtils::addr_pgr(pipe);

  uint32_t v = InWord((void*)&pgr->pgr_app[app].ctrl);
  int apptype = getp_pgr_app_ctrl_app_type((void*)&v);
  if (apptype == kPgenAppTypeLinkdown) {
    assert(batchnum == 0); // Insist just 1 batch for Linkdown
  }
  uint32_t num = 0u;
  setp_pgr_app_event_number_batch_num (&num, static_cast<uint32_t>(batchnum));
  setp_pgr_app_event_number_packet_num(&num, static_cast<uint32_t>(pktnum));
  uint32_t ibg_bas = 0u, ibg_max = 0u, ibg_scl = 0u;
  setp_pgr_app_event_base_jitter_value_value(&ibg_bas, ibg_jit_base);
  setp_pgr_app_event_max_jitter_value       (&ibg_max, static_cast<uint32_t>(ibg_jit_max));
  setp_pgr_app_event_jitter_scaling_value   (&ibg_scl, static_cast<uint32_t>(ibg_jit_scl));
  uint32_t ipg_bas = 0u, ipg_max = 0u, ipg_scl = 0u;
  setp_pgr_app_event_base_jitter_value_value(&ipg_bas, ipg_jit_base);
  setp_pgr_app_event_max_jitter_value       (&ipg_max, static_cast<uint32_t>(ipg_jit_max));
  setp_pgr_app_event_jitter_scaling_value   (&ipg_scl, static_cast<uint32_t>(ipg_jit_scl));
  uint32_t tim = 0u;
  setp_pgr_app_event_timer_timer_count(&tim, timer_cycles);

  OutWord(&pgr->pgr_app[app].event_number, num);
  OutWord(&pgr->pgr_app[app].event_ibg_jitter_base_value, ibg_bas);
  OutWord(&pgr->pgr_app[app].event_max_ibg_jitter, ibg_max);
  OutWord(&pgr->pgr_app[app].event_ibg_jitter_scale, ibg_scl);
  OutWord(&pgr->pgr_app[app].event_ipg_jitter_base_value, ipg_bas);
  OutWord(&pgr->pgr_app[app].event_max_ipg_jitter, ipg_max);
  OutWord(&pgr->pgr_app[app].event_ipg_jitter_scale, ipg_scl);
  OutWord(&pgr->pgr_app[app].event_timer, tim);
}

void TestUtil::pktgen_app_counters(int pipe, int app) {
  // Read counters
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((app >= 0) && (app < 16));
  assert(0); // Not implemented yet
}

// NB Here output is WRT pipe - so from EBUF (but also from TBC/PCIe and EthCPU Rx)
void TestUtil::pktgen_cmn_output_port_ctrl_set(int pipe, int outport,
                                               bool en, uint8_t chan_en, uint8_t chan_mode,
                                               uint64_t chan_seq) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((outport >= kPgenOutPortMin) && (outport <= kPgenOutPortMax));
  auto pgr = RegisterUtils::addr_pgr(pipe);
  uint32_t v = 0u;
  if ((outport >= kPgenOutPortEbuf0) && (outport <= kPgenOutPortEbuf3)) {
    assert((chan_en & ~0x3) == 0);
    assert(chan_mode <= 1);
    setp_pgr_ebuf_port_ctrl_port_en     (&v, en?1:0);
    setp_pgr_ebuf_port_ctrl_channel_en  (&v, static_cast<uint32_t>(chan_en));
    setp_pgr_ebuf_port_ctrl_channel_mode(&v, static_cast<uint32_t>(chan_mode));
    OutWord(&pgr->pgr_common.ebuf_port_ctrl[outport], v);
  } else if (outport == kPgenOutPortTbc) {
    assert((pipe == 0) || (pipe == 2));
    setp_pgr_tbc_port_ctrl_port_en(&v, en?1:0);
    OutWord(&pgr->pgr_common.tbc_port_ctrl, v);
  } else if (outport == kPgenOutPortEthCpu) {
    assert((pipe == 0) || (pipe == 2));
    assert((chan_en & ~0xF) == 0);
    assert(chan_mode <= 4);
    assert((chan_seq & ~UINT64_C(0xFF)) == UINT64_C(0));
    if (en) { // Check correct chans enabled given mode
      switch (chan_mode) {
        case 0: assert((chan_en & 0x1) == 0x1); break;
        case 1: assert((chan_en & 0x5) == 0x5); break;
        case 2: assert((chan_en & 0xD) == 0xD); break;
        case 3: assert((chan_en & 0x7) == 0x7); break;
        case 4: assert((chan_en & 0xF) == 0xF); break;
      }
    }
    if (chan_seq == UINT64_C(0)) {
      uint8_t modes[5] = { 0x00, 0x88, 0xC8, 0x98, 0xD8 };
      chan_seq = static_cast<uint64_t>(modes[chan_mode]);
    }
    setp_pgr_eth_cpu_ctrl_port_en(&v, en?1:0);
    setp_pgr_eth_cpu_ctrl_channel_en   (&v, static_cast<uint32_t>(chan_en));
    setp_pgr_eth_cpu_ctrl_channel_mode (&v, static_cast<uint32_t>(chan_mode));
    setp_pgr_eth_cpu_ctrl_channel_seq(&v, static_cast<uint32_t>(chan_seq));
    OutWord(&pgr->pgr_common.eth_cpu_port_ctrl, v);
  }
}
// NB Here input is WRT pipe - so to IPB (but also to TBC/PCIe and EthCPU Tx)
void TestUtil::pktgen_cmn_input_port_ctrl_set(int pipe,
                                              uint32_t recirc_chan_en, uint32_t pgen_chan_en,
                                              uint64_t chan_seq) {
  // Only IPB(TBC/EthCPU) channels enabled in recirc_chan_en can trigger a PktGen pkt
  // Only IPB(TBC/EthCPU) channels enabled in pgen_chan_en do RR arbitration for channel
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((recirc_chan_en & ~0xFFu) == 0u);
  assert((pgen_chan_en & ~0xFFu) == 0u);
  uint32_t tbc_eth_chans = 0u;
  auto pgr = RegisterUtils::addr_pgr(pipe);
  if (pipe == 0) {
    // Work out what chans used for TBC/Eth
    // If BOTH recirc/pgen using a chan it MUST NOT be one used for TBC/Eth
    // as these cannot handle the contention
    uint32_t v = 0u;
    v = InWord((void*)&pgr->pgr_common.tbc_port_ctrl);
    bool tbc_en = ( getp_pgr_tbc_port_ctrl_port_en((void*)&v) == 1u );
    uint32_t tbc_chans = (tbc_en) ?1u :0u;
    v = InWord((void*)&pgr->pgr_common.eth_cpu_port_ctrl);
    bool eth_en = ( getp_pgr_eth_cpu_ctrl_port_en((void*)&v) == 1u );
    uint32_t eth_chans = (eth_en) ?getp_pgr_eth_cpu_ctrl_channel_en((void*)&v) :0u;
    tbc_eth_chans = (eth_chans << 2) | tbc_chans;
  }
  assert((recirc_chan_en & pgen_chan_en & tbc_eth_chans) == 0u);

  pgen_chan_en &= recirc_chan_en; // Silently clear disabled bits
  uint32_t v0 = 0u, v1 = 0u;
  setp_pgr_ipb_port_ctrl_ipb_port_ctrl_0_2_recir_channel_en(&v0, recirc_chan_en);
  setp_pgr_ipb_port_ctrl_ipb_port_ctrl_0_2_pgen_channel_en(&v0, pgen_chan_en);
  // Enable 8-chan mode by default 7,6,5,4,3,2,1,0 (or 4-chan mode 6,4,2,0,6,4,2,0)
  if (chan_seq == UINT64_C(0)) chan_seq = UINT64_C(0xFAC688);
  //if (chan_seq == UINT64_C(0)) chan_seq = UINT64_C(0xD01D10);
  uint32_t chan_seq_lo = static_cast<uint32_t>((chan_seq >> 0) & UINT64_C(0x00FF));
  uint32_t chan_seq_hi = static_cast<uint32_t>((chan_seq >> 8) & UINT64_C(0xFFFF));
  setp_pgr_ipb_port_ctrl_ipb_port_ctrl_0_2_channel_seq_7_0(&v0, chan_seq_lo);
  setp_pgr_ipb_port_ctrl_ipb_port_ctrl_1_2_channel_seq_23_8(&v1, chan_seq_hi);
  OutWord(&pgr->pgr_common.ipb_port_ctrl.ipb_port_ctrl_0_2, v0);
  OutWord(&pgr->pgr_common.ipb_port_ctrl.ipb_port_ctrl_1_2, v1);
}
void TestUtil::pktgen_cmn_timestamp_set(int pipe, int outport,
                                        uint32_t recirc_ts_off, uint32_t csr_ts_off) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((outport >= kPgenOutPortEbuf0) && (outport <= kPgenOutPortEbuf3));
  // Insist csr_ts_off 0 - only honour when outport==0
  assert((outport == 0) || (csr_ts_off == 0u));
  auto pgr = RegisterUtils::addr_pgr(pipe);
  uint32_t rcr_ts = 0u, csr_ts = 0u;
  setp_pgr_port_ebuf_ts_offset (&rcr_ts, recirc_ts_off);
  setp_pgr_port_csr_ts_offset (&csr_ts, csr_ts_off);
  OutWord(&pgr->pgr_common.recirc_ts[outport], rcr_ts);
  if (outport == 0) OutWord(&pgr->pgr_common.csr_ts_offset, csr_ts);
}
void TestUtil::pktgen_cmn_portdown_ctrl_set(int pipe,
                                            const BitVector<72> &mask0,
                                            const BitVector<72> &mask1) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  uint32_t m0[3] = { static_cast<uint32_t>(mask0.get_word( 0,32)),
                     static_cast<uint32_t>(mask0.get_word(32,32)),
                     static_cast<uint32_t>(mask0.get_word(64, 8)) };
  uint32_t m1[3] = { static_cast<uint32_t>(mask1.get_word( 0,32)),
                     static_cast<uint32_t>(mask1.get_word(32,32)),
                     static_cast<uint32_t>(mask1.get_word(64, 8)) };
  auto pgr = RegisterUtils::addr_pgr(pipe);
  OutWord(&pgr->pgr_common.pgen_port_down_mask[0].pgen_port_down_mask_0_3, m0[0]);
  OutWord(&pgr->pgr_common.pgen_port_down_mask[0].pgen_port_down_mask_1_3, m0[1]);
  OutWord(&pgr->pgr_common.pgen_port_down_mask[0].pgen_port_down_mask_2_3, m0[2]);
  OutWord(&pgr->pgr_common.pgen_port_down_mask[1].pgen_port_down_mask_0_3, m1[0]);
  OutWord(&pgr->pgr_common.pgen_port_down_mask[1].pgen_port_down_mask_1_3, m1[1]);
  OutWord(&pgr->pgr_common.pgen_port_down_mask[1].pgen_port_down_mask_2_3, m1[2]);
  // Also set the pipe into the pgr_common.pgen_ctrl.pipe_id field
  // PktGen puts this pipe_id (the real pipe_id) in the Batch field of
  // PortDown generated packets.
  uint32_t csr = 0u;
  setp_pgr_pgen_ctrl_pipe_id (&csr, pipe);
  OutWord(&pgr->pgr_common.pgen_ctrl, csr);
 }
BitVector<72> TestUtil::pktgen_cmn_portdown_ctrl_get(int pipe, int mask) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((mask == 0) || (mask == 1));
  uint32_t m[3] = { 0u, 0u, 0u };
  auto pgr = RegisterUtils::addr_pgr(pipe);
  m[0] = InWord((void*)&pgr->pgr_common.pgen_port_down_mask[mask].pgen_port_down_mask_0_3);
  m[1] = InWord((void*)&pgr->pgr_common.pgen_port_down_mask[mask].pgen_port_down_mask_1_3);
  m[2] = InWord((void*)&pgr->pgr_common.pgen_port_down_mask[mask].pgen_port_down_mask_2_3);
  BitVector<72> bv(UINT64_C(0));
  bv.set_word(static_cast<uint64_t>(m[0]),  0, 32);
  bv.set_word(static_cast<uint64_t>(m[1]), 32, 32);
  bv.set_word(static_cast<uint64_t>(m[2]), 64,  8);
  return bv;
}
void TestUtil::pktgen_cmn_portdown_disable_set(int pipe,
                                            const BitVector<72> &disable) {
  uint32_t dis[3] = { static_cast<uint32_t>(disable.get_word( 0,32)),
                      static_cast<uint32_t>(disable.get_word(32,32)),
                      static_cast<uint32_t>(disable.get_word(64, 8)) };
  auto pgr = RegisterUtils::addr_pgr(pipe);
  OutWord(&pgr->pgr_common.port_down_dis.port_down_dis_0_3, dis[0]);
  OutWord(&pgr->pgr_common.port_down_dis.port_down_dis_1_3, dis[1]);
  OutWord(&pgr->pgr_common.port_down_dis.port_down_dis_2_3, dis[2]);
}
bool TestUtil::pktgen_cmn_portdown_disable_get(int pipe,int port) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((port >= 0) && (port < 72));
  auto pgr_pdd = &(RegisterUtils::addr_pgr(pipe)->pgr_common.port_down_dis);
  void* mask_addr;
  switch ( port / 32 ) {
    case 0: mask_addr = (void*)&pgr_pdd->port_down_dis_0_3 ; break;
    case 1: mask_addr = (void*)&pgr_pdd->port_down_dis_1_3 ; break;
    case 2: mask_addr = (void*)&pgr_pdd->port_down_dis_2_3 ; break;
    default: RMT_ASSERT(0); break;
  }
  auto m = InWord(mask_addr);
  return (m >> (port%32)) & 1;
}
// These 2 just stubs
void TestUtil::pktgen_cmn_pfc_xoff_ctrl_set(int pipe) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert(0); // Not implemented yet, or ever?
}
void TestUtil::pktgen_cmn_credit_dwrr_ctrl_set(int pipe) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert(0); // Not implemented yet, or ever?
}

void TestUtil::pktgen_port_down_vec_clr_set(int pipe,
                                            bool en, bool app_disable, bool set_sent) {
  if ((pipe < 0) || (pipe >= kMaxPipes)) return;
  auto& pgr_common_reg = RegisterUtils::addr_pgr(pipe)->pgr_common;
  uint32_t csr = 0;
  if (en)          setp_pgr_pgen_port_down_vec_clr_en (&csr, 1);
  if (app_disable) setp_pgr_pgen_port_down_vec_clr_app_disable (&csr, 1);
  if (set_sent)    setp_pgr_pgen_port_down_vec_clr_set_sent(&csr, 1);
  OutWord(&(pgr_common_reg.pgen_port_down_vec_clr), csr);
}


// These to setup memories which are (usually) byte-swapped
// ***** these functions assume data is word-swapped  *****
// ***** (so data0 is MSword, data1 is LSword)        *****
// ***** but handle the byte-swapping themselves.     *****
void TestUtil::pktgen_mem_pkt_set(int pipe, int i, uint64_t data0, uint64_t data1) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  // Pktgen mems are byte-swapped!
  data0 = __builtin_bswap64(data0); data1 = __builtin_bswap64(data1);
  // Note eltsz/max don't vary per-pipe but pass pipe anyway
  uint64_t eltsz = MemUtils::pktgen_buffer_mem_word_array_element_size(pipe);
  uint64_t max = MemUtils::pktgen_buffer_mem_word_array_count(pipe);
  assert((i >= 0) && (i < static_cast<int>(max)));
  uint64_t byteaddr = MemUtils::pktgen_mem_address(pipe) + (eltsz * i);
  IndirectWrite(byteaddr/16, data0, data1); // IndirectWrite needs 16B address
}
void TestUtil::pktgen_mem_meta1_set(int pipe, int i, uint64_t data0, uint64_t data1) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  if (MemUtils::pktgen_buffer_ph0_is_byte_swapped) {
    // Pktgen 'phase0' mems ***were thought to be*** byte-swapped on JBay, but....
    // XXX: Pktgen mems are *not* byte-swapped in JBay
    //            so fixup MemUtils:: var above to be false
    data0 = __builtin_bswap64(data0); data1 = __builtin_bswap64(data1);
  } else {
    // But they're part of IPB in WIP and *not* byte-swapped
    // Also, data0 argument contains MSWord so need to swap data0<->data1
    uint64_t tmp = data0;
    data0 = data1;
    data1 = tmp;
  }
  // Note eltsz/max don't vary per-pipe but pass pipe anyway
  uint64_t eltsz = MemUtils::pktgen_buffer_ph0_word_array_element_size(pipe);
  uint64_t max = MemUtils::pktgen_buffer_ph0_word_array_count(pipe);
  assert((i >= 0) && (i < static_cast<int>(max)));
  uint64_t byteaddr = MemUtils::pktgen_ph0_address(pipe) + (eltsz * i);
  IndirectWrite(byteaddr/16, data0, data1); // IndirectWrite needs 16B address
}

// stub for compatibility with tofino testing
uint64_t TestUtil::deparser_get_learn_counter(int pipe) { return UINT64_C(0); }

RegPtr TestUtil::lookup_register_map(std::vector<PathElement> path) {
  // chip-agnostic map lookup using BFN_CHIP_REG from jbay_shared/register_utils.h
  return BFN_CHIP_REG_CONCAT(map)().map(path);
}


void TestUtil::learn_config(int pipe, bool valid, int phv_for_table_index,
                              // these parameters currently ignored, all entries are
                              //  programmed to pick up the SA
                              int table_entry_index,int length,int phvs[48]) {
  // TODO:JBAY: Fix: Just to keep linker happy

  // TODO: this jbay specific code was previously #if'd in the tofino specific
  // implementation i.e. never used; moved here for reference
  //  auto& learn_sel_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.m_learn_sel;
  //  int pov = 0; // TODO: check this
  //  OutWord(&copy_to_cpu_reg, deparser_create_info_word(phv_for_table_index, pov, false /*disable*/,0 /*shift*/));
}

void TestUtil::deparser_init(Deparser *dp) { // JBay
  int pipe=0; //TODO: this should be setable
  // TODO: this just works on the ingress deparser, needs to be updated
  //   to handle the egress deparser
  //auto& pov_pos_reg = kTofinoPtr->pipes[pipe].deparser.inp.iir.main_i.pov;
  //auto& pov_pos_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.pov;
  auto& pov_pos_reg = RegisterUtils::addr_dprsr(pipe)->inp.ipp.main_i.pov;

  Dprsr_pov_position_r temp{}; // a wide register
  for (int i=0; i<16; i++) {
    setp_dprsr_pov_position_r_phvs(&temp,i,Phv::make_word(3,1) /*PHV8_1*/);
  }
  OutWord( &pov_pos_reg.pov_0_4 , temp.pov_0_4 );
  OutWord( &pov_pos_reg.pov_1_4 , temp.pov_1_4 );
  OutWord( &pov_pos_reg.pov_2_4 , temp.pov_2_4 );
  OutWord( &pov_pos_reg.pov_3_4 , temp.pov_3_4 );


  // FIELD_DICTIONARY_ENTRY
  for (int i=0; i<Deparser::kNumFieldDictionaryEntries; i++) {
    //OD dp->get_dprsr_mem_rspec().field_dictionary(i).valid(0);
    deparser_set_field_dictionary_entry(pipe, i, 0, 0, 0, 0, 0, 0, 0, 0);
  }

  const uint16_t  DA_HI_16 = Phv::make_word(4,0);
  const uint16_t  DA_LO_32 = Phv::make_word(0,0);
  const uint16_t  SA_HI_16 = Phv::make_word(4,1);
  const uint16_t  SA_LO_32 = Phv::make_word(0,1);
  const uint16_t  ETH_TYPE = Phv::make_word(4,2);
  const uint16_t  IP4_HL   = Phv::make_word(2,1);
  const uint16_t  IP4_TTL  = Phv::make_word(2,2);
  const uint16_t  IP4_PROTO= Phv::make_word(2,3);
  const uint16_t  IP4_LEN  = Phv::make_word(4,4);
  //const uint16_t  IP4_ID   = Phv::make_word(4,3);
  const uint16_t  IP4_FRAG = Phv::make_word(4,5);
  const uint16_t  IP4_CKSM = Phv::make_word(4,6);
  const uint16_t  IP4_SRC  = Phv::make_word(0,2);
  const uint16_t  IP4_DST  = Phv::make_word(0,3);
  const uint16_t  P_SPORT  = Phv::make_word(4,7);
  const uint16_t  P_DPORT  = Phv::make_word(4,8);
  const uint16_t  PHV8_0   = Phv::make_word(3,0);
  const uint16_t  PHV8_1   = Phv::make_word(3,1);
  const uint16_t  PHV8_2   = Phv::make_word(3,2);
  const uint16_t  PHV8_3   = Phv::make_word(3,3);


  deparser_set_field_dictionary_entry(pipe, 0, 1, DA_HI_16, DA_HI_16, DA_LO_32, DA_LO_32, 0, 0, 0x0F);
  deparser_set_field_dictionary_entry(pipe,1, 1, DA_LO_32, DA_LO_32, SA_HI_16, SA_HI_16, 0, 0, 0x0F);
  deparser_set_field_dictionary_entry(pipe,7, 1, SA_LO_32, SA_LO_32, SA_LO_32, SA_LO_32, 0, 0, 0x0F);
  deparser_set_field_dictionary_entry(pipe,8, 1, ETH_TYPE, ETH_TYPE, IP4_HL, IP4_TTL, 0, 0, 0x0F);
  deparser_set_field_dictionary_entry(pipe,100, 1, IP4_PROTO, IP4_LEN, IP4_LEN, IP4_FRAG, 0, 0, 0x0F);
  deparser_set_field_dictionary_entry(pipe,110, 1, IP4_FRAG, IP4_CKSM, IP4_CKSM, IP4_SRC, 0, 0, 0x0F);
  // changed to all indexes below 128 so that it works for JBay
  deparser_set_field_dictionary_entry(pipe,113, 1, IP4_SRC, IP4_SRC, IP4_SRC, IP4_DST, 0, 0, 0x0F);
  deparser_set_field_dictionary_entry(pipe,117, 1, IP4_DST, IP4_DST, IP4_DST, P_SPORT, 0, 0, 0x0F);
  deparser_set_field_dictionary_entry(pipe,120, 1, P_SPORT, P_DPORT, P_DPORT, PHV8_0, 0, 0, 0x0F);
  deparser_set_field_dictionary_entry(pipe,121, 1, PHV8_1, PHV8_2, PHV8_3, 0, 0, 3, 0x0F);

  // Set a default egress port so that packet is not dropped at deparser.

  // Note no default egress port on JBay....
  deparser_set_egress_unicast_port_info(pipe, 0, false, true);
  // ...so just enable *all* ports in the Deparser
  auto& icr = RegisterUtils::addr_dprsr(pipe)->inp.icr;
  OutWord( &icr.mac0_en, 0xFF );
  OutWord( &icr.mac1_en, 0xFF );
  OutWord( &icr.mac2_en, 0xFF );
  OutWord( &icr.mac3_en, 0xFF );
  OutWord( &icr.mac4_en, 0xFF );
  OutWord( &icr.mac5_en, 0xFF );
  OutWord( &icr.mac6_en, 0xFF );
  OutWord( &icr.mac7_en, 0xFF );
  OutWord( &icr.mac8_en, 0xFF );


  deparser_config_basic_eth_ip_tcp(dp);

  // enable all ports on all slices
  for (int s=0;s<4;++s) {
    //TODO:JBAY:Missing DEPARSER fields
    //auto& h = RegisterUtils::addr_dprsr(pipe)->slice[s].hir.h;
    //OutWord( &h.port0_en, 0x03 );
    //OutWord( &h.port1_en, 0xFF );
    //OutWord( &h.port2_en, 0xFF );
  }
}

// backwards compatible version to get Tofino tests working
void TestUtil::deparser_set_field_dictionary_entry(int pipe, int index, int valid,
                                         int phv0, int phv1, int phv2, int phv3,
                                                   int pov_sel, int num_bytes,int version) { // JBay
  int phvs[8] { phv0, phv1, phv2, phv3, 0, 0, 0, 0 };
  int n = num_bytes ? num_bytes : 4; // num_bytes == 0 in tofino means 4 bytes in use
  deparser_set_field_dictionary_entry( pipe,index,valid, pov_sel, false, 0, 0, phvs, n, 0xFF);
}

void TestUtil::deparser_set_field_dictionary_entry(int pipe, int index, int valid, int pov, bool seg_vld,
                                                   int seg_sel, int seg_slice,
                                                   int phvs[8], int num_bytes, int is_phv ) {  // JBay
  assert(index < Deparser::kNumFieldDictionaryEntries);

  if (! seg_vld ) {
    seg_sel   = 1 & (num_bytes >> 3);
    seg_slice = num_bytes & 0x7;
  }

  uint32_t csr=0 ;
  setp_fd_chunk_info_r_seg_slice(&csr, seg_slice );
  setp_fd_chunk_info_r_seg_sel(&csr, seg_sel);
  setp_fd_chunk_info_r_seg_vld(&csr,seg_vld);
  setp_fd_chunk_info_r_pov (&csr,pov);
  setp_fd_chunk_info_r_chunk_vld(&csr, valid);

  uint32_t csr2=0 ;
  setp_fd_bytesel_chunk_info_r_seg_slice(&csr2, seg_slice );
  setp_fd_bytesel_chunk_info_r_seg_sel(&csr2, seg_sel);
  setp_fd_bytesel_chunk_info_r_seg_vld(&csr2,seg_vld);

  for (int s=0;s<4;++s) {
    //auto& fd_partial_chunk_cfg = RegisterUtils::addr_dprsr(pipe)->slice[s].him.fd_len.fd_len_chunk.cfg[index]; // [DPRSR_NUM_FD_CHUNKS] = 128
    auto& fd_partial_chunk_cfg = RegisterUtils::addr_dprsr(pipe)->inp.icr.ingr.chunk_info[index];
    OutWord( &fd_partial_chunk_cfg, csr );  // [DPRSR_NUM_FD_CHUNKS] = 128

    auto& fd_full_chunk_cfg = RegisterUtils::addr_dprsr(pipe)->ho_i[s].him.fd_compress.chunk[index].cfg;
    OutWord( &fd_full_chunk_cfg, csr2 );
    auto& fd_full_chunk_is_phv = RegisterUtils::addr_dprsr(pipe)->ho_i[s].him.fd_compress.chunk[index].is_phv;
    OutWord( &fd_full_chunk_is_phv, is_phv );

    auto& fd_full_chunk_byte_off = RegisterUtils::addr_dprsr(pipe)->ho_i[s].him.fd_compress.chunk[index].byte_off;
    Fd_byte_off_info_r byte_off_reg {};
    for (int i=0;i<8;++i) {
      setp_fd_byte_off_info_r_phv_offset (&byte_off_reg,i, phvs[i]);
    }
    OutWord( &fd_full_chunk_byte_off.byte_off_0_2, byte_off_reg.byte_off_0_2 );
    OutWord( &fd_full_chunk_byte_off.byte_off_1_2, byte_off_reg.byte_off_1_2 );

  }

  // do egress too
  for (int s=0;s<4;++s) {
    //auto& fd_partial_chunk_cfg = RegisterUtils::addr_dprsr(pipe)->slice[s].him.fd_len.fd_len_chunk.cfg[index]; // [DPRSR_NUM_FD_CHUNKS] = 128
    auto& fd_partial_chunk_cfg = RegisterUtils::addr_dprsr(pipe)->inp.icr.egr.chunk_info[index];
    OutWord( &fd_partial_chunk_cfg, csr );  // [DPRSR_NUM_FD_CHUNKS] = 128

    auto& fd_full_chunk_cfg = RegisterUtils::addr_dprsr(pipe)->ho_e[s].hem.fd_compress.chunk[index].cfg;
    OutWord( &fd_full_chunk_cfg, csr2 );
    auto& fd_full_chunk_is_phv = RegisterUtils::addr_dprsr(pipe)->ho_e[s].hem.fd_compress.chunk[index].is_phv;
    OutWord( &fd_full_chunk_is_phv, is_phv );

    auto& fd_full_chunk_byte_off = RegisterUtils::addr_dprsr(pipe)->ho_e[s].hem.fd_compress.chunk[index].byte_off;
    Fd_byte_off_info_r byte_off_reg {};
    for (int i=0;i<8;++i) {
      setp_fd_byte_off_info_r_phv_offset (&byte_off_reg,i, phvs[i]);
    }
    OutWord( &fd_full_chunk_byte_off.byte_off_0_2, byte_off_reg.byte_off_0_2 );
    OutWord( &fd_full_chunk_byte_off.byte_off_1_2, byte_off_reg.byte_off_1_2 );

  }

}
void TestUtil::deparser_set_egress_unicast_port_info(int pipe, int phv, int pov, bool disable) { // JBay
    auto& egress_unicast_port_reg   = RegisterUtils::addr_dprsr(pipe)->inp.ipp.ingr.m_egress_unicast_port;
    OutWord(&egress_unicast_port_reg, deparser_create_info_word(phv, pov, disable));
    auto& egress_unicast_port_reg_e = RegisterUtils::addr_dprsr(pipe)->inp.ipp.egr.m_egress_unicast_port;
    OutWord(&egress_unicast_port_reg_e, deparser_create_info_word(phv, pov, disable));
  }

uint32_t TestUtil::deparser_create_info_word(int phv, int pov, bool disable, int shift) { // JBay
    uint32_t info_word;
    info_word  =  phv & 0xFF;
    info_word |= (pov & 0x7f) << 8;
    info_word |= disable ? 1<<15 : 0;
    // most shifts are 3 bits wide, some are 4... assume all are four!
    info_word |= (shift & 0xf) << 16;
    return info_word;
}

void TestUtil::deparser_set_csum_row_entry(int pipe, int engine,
                                           int csum_entry_idx, bool swap,
                                           bool zero_m_s_b, bool zero_l_s_b,
                                           int pov) {  // JBay
    uint32_t temp_csum_cfg_reg = 0u;
    setp_dprsr_csum_row_entry_swap( &temp_csum_cfg_reg, swap );
    setp_dprsr_csum_row_entry_zero_m_s_b( &temp_csum_cfg_reg, zero_m_s_b );
    setp_dprsr_csum_row_entry_zero_l_s_b( &temp_csum_cfg_reg, zero_l_s_b );
    setp_dprsr_csum_row_entry_pov( &temp_csum_cfg_reg, pov );
    auto& csum_row_entry_reg = RegisterUtils::addr_dprsr(pipe)->inp.ipp_m.i_csum.engine[engine].entry[csum_entry_idx];

    OutWord( &csum_row_entry_reg, temp_csum_cfg_reg );

}

void TestUtil::deparser_set_capture_tx_ts_info(int pipe, int phv, int pov, bool disable, int shift) {   // JBay
  uint32_t ptmp=0;
  auto& m_pov = RegisterUtils::addr_dprsr(pipe)->inp.icr.egr_meta_pov;
  setp_dprsr_egress_hdr_meta_for_input_g_m_capture_tx_ts_pov(&ptmp,pov);
  OutWord( &m_pov.m_capture_tx_ts, ptmp );

  uint32_t tmp=0;
  setp_dprsr_header_egress_meta_g_m_capture_tx_ts_phv(&tmp,phv);
  setp_dprsr_header_egress_meta_g_m_capture_tx_ts_shft(&tmp,shift);
  setp_dprsr_header_egress_meta_g_m_capture_tx_ts_dis(&tmp,disable?1:0);
  for (int s=0;s<4;++s) {
    auto& m = RegisterUtils::addr_dprsr(pipe)->ho_e[s].her.meta;
    OutWord(&m.m_capture_tx_ts, tmp);
  }
}
void TestUtil::deparser_set_copy_to_cpu_info( int pipe, int phv, int pov, bool disable, int shift ) {  // JBay
  auto& copy_to_cpu_reg = RegisterUtils::addr_dprsr(pipe)->inp.ipp.ingr.m_copy_to_cpu;
  OutWord(&copy_to_cpu_reg, deparser_create_info_word(phv, pov, disable , shift));
}
void TestUtil::deparser_set_copy_to_cpu_pipe_vector(int pipe, uint8_t pipe_vector) { // JBay
#if MCN_TEST(MODEL_CHIP_NAMESPACE, rsvd0)
  // NB: for deparser this is actually setting a TM WAC register, not deparser
  auto& copy_to_cpu_pipe_vector_reg = RegisterUtils::addr_tm_wac()->wac_common.wac_common.wac_copy_to_cpu_pv;
  OutWord(&copy_to_cpu_pipe_vector_reg, pipe_vector);
#else
  auto& copy_to_cpu_pipe_vector_reg = RegisterUtils::addr_dprsr(pipe)->inp.ipp.ingr.copy_to_cpu_pv;
  OutWord(&copy_to_cpu_pipe_vector_reg, pipe_vector);
#endif
}

void TestUtil::deparser_set_ct_disable_info( int pipe, int phv, int pov, bool disable, int shift ) { // JBay
  uint32_t ptmp=0;
  auto& m_pov = RegisterUtils::addr_dprsr(pipe)->inp.icr.ingr_meta_pov;
  setp_dprsr_ingress_hdr_meta_for_input_g_m_ct_disable_pov(&ptmp,pov);
  OutWord( &m_pov.m_ct_disable, ptmp );

  uint32_t tmp=0;
  setp_dprsr_header_ingress_meta_g_m_ct_disable_phv(&tmp,phv);
  setp_dprsr_header_ingress_meta_g_m_ct_disable_shft(&tmp,shift);
  setp_dprsr_header_ingress_meta_g_m_ct_disable_dis(&tmp,disable?1:0);
  for (int s=0;s<4;++s) {
    auto& m = RegisterUtils::addr_dprsr(pipe)->ho_i[s].hir.meta;
    OutWord(&m.m_ct_disable, tmp);
  }
}

void TestUtil::deparser_set_ct_mcast_info( int pipe, int phv, int pov, bool disable, int shift ) { // JBay
  uint32_t ptmp=0;
  auto& m_pov = RegisterUtils::addr_dprsr(pipe)->inp.icr.ingr_meta_pov;
  setp_dprsr_ingress_hdr_meta_for_input_g_m_ct_mcast_pov(&ptmp,pov);
  OutWord( &m_pov.m_ct_mcast, ptmp );

  uint32_t tmp=0;
  setp_dprsr_header_ingress_meta_g_m_ct_mcast_phv(&tmp,phv);
  setp_dprsr_header_ingress_meta_g_m_ct_mcast_shft(&tmp,shift);
  setp_dprsr_header_ingress_meta_g_m_ct_mcast_dis(&tmp,disable?1:0);
  for (int s=0;s<4;++s) {
    auto& m = RegisterUtils::addr_dprsr(pipe)->ho_i[s].hir.meta;
    OutWord(&m.m_ct_mcast, tmp);
  }
}

void TestUtil::deparser_set_deflect_on_drop_info( int pipe, int phv, int pov, bool disable, int shift ) { // JBay
  uint32_t ptmp=0;
  auto& m_pov = RegisterUtils::addr_dprsr(pipe)->inp.icr.ingr_meta_pov;
  setp_dprsr_ingress_hdr_meta_for_input_g_m_deflect_on_drop_pov(&ptmp,pov);
  OutWord( &m_pov.m_deflect_on_drop, ptmp );

  uint32_t tmp=0;
  setp_dprsr_header_ingress_meta_g_m_deflect_on_drop_phv(&tmp,phv);
  setp_dprsr_header_ingress_meta_g_m_deflect_on_drop_shft(&tmp,shift);
  setp_dprsr_header_ingress_meta_g_m_deflect_on_drop_dis(&tmp,disable?1:0);
  for (int s=0;s<4;++s) {
    auto& m = RegisterUtils::addr_dprsr(pipe)->ho_i[s].hir.meta;
    OutWord(&m.m_deflect_on_drop, tmp);
  }
}

void TestUtil::deparser_set_drop_ctl_info( int pipe, int phv, int pov, bool disable, int shift ) { // JBay
  auto& drop_ctl_reg = RegisterUtils::addr_dprsr(pipe)->inp.ipp.ingr.m_drop_ctl;
  OutWord(&drop_ctl_reg, deparser_create_info_word(phv, pov, disable , shift));
}


void TestUtil::deparser_set_mtu_trunc_len_info( int pipe, int phv, int pov, bool disable, int shift ) { // JBay
  uint32_t ptmp=0;
  auto& m_pov = RegisterUtils::addr_dprsr(pipe)->inp.icr.ingr_meta_pov;
  setp_dprsr_ingress_hdr_meta_for_input_g_m_mtu_trunc_len_pov(&ptmp,pov);
  OutWord( &m_pov.m_mtu_trunc_len, ptmp );

  uint32_t tmp=0;
  setp_dprsr_header_ingress_meta_g_m_mtu_trunc_len_phv(&tmp,phv);
  setp_dprsr_header_ingress_meta_g_m_mtu_trunc_len_shft(&tmp,shift);
  setp_dprsr_header_ingress_meta_g_m_mtu_trunc_len_dis(&tmp,disable?1:0);
  for (int s=0;s<4;++s) {
    auto& m = RegisterUtils::addr_dprsr(pipe)->ho_i[s].hir.meta;
    OutWord(&m.m_mtu_trunc_len, tmp);
  }
}

void TestUtil::deparser_set_mtu_trunc_err_f_info( int pipe, int phv, int pov, bool disable, int shift ) { // JBay
  uint32_t ptmp=0;
  auto& m_pov = RegisterUtils::addr_dprsr(pipe)->inp.icr.ingr_meta_pov;
  setp_dprsr_ingress_hdr_meta_for_input_g_m_mtu_trunc_err_f_pov(&ptmp,pov);
  OutWord( &m_pov.m_mtu_trunc_err_f, ptmp );

  uint32_t tmp=0;
  setp_dprsr_header_ingress_meta_g_m_mtu_trunc_err_f_phv(&tmp,phv);
  setp_dprsr_header_ingress_meta_g_m_mtu_trunc_err_f_shft(&tmp,shift);
  setp_dprsr_header_ingress_meta_g_m_mtu_trunc_err_f_dis(&tmp,disable?1:0);
  for (int s=0;s<4;++s) {
    auto& m = RegisterUtils::addr_dprsr(pipe)->ho_i[s].hir.meta;
    OutWord(&m.m_mtu_trunc_err_f, tmp);
  }
}

void TestUtil::parser_update_extract16_type_cnt(MODEL_CHIP_NAMESPACE::Parser *p,
                                                int pi,
                                                uint8_t extract_type,
                                                uint8_t val) {
  p->update_extract16_type_cnt(pi, extract_type, val);
}

void TestUtil::set_dprsr_clot_csum_invert(int pipe, int csum_eng, int clot_num) {
  assert((clot_num >= 0) && (clot_num < 16));
  set_dprsr_csum_invert(pipe, csum_eng, 0 + clot_num); // Bits [15..0]
}
void TestUtil::set_dprsr_phv_csum_invert(int pipe, int csum_eng, int phv_num) {
  assert((phv_num >= 0) && (phv_num < 8));
  set_dprsr_csum_invert(pipe, csum_eng, 16 + phv_num); // Bits [23..16]
}


}
