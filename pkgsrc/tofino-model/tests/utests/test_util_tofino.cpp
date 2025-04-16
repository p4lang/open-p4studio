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

#include <iostream>
#include <utests/test_util.h>
#include <parser-static-config.h>
#include <pktgen-reg.h>
#include <register_includes/register_map.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

// width 0 =>  8b instructions
// width 1 => 16b instructions (or dual 8b)
// width 2 => 32b instructions (or dual 16b)
// width 3 => 64b instructions (or dual 32b)

void TestUtil::set_stateful_log_instruction_regs( Mau_match_merge_addrmap& mm_regs, int alu,
                                                  int width, int vpn_offset, int vpn_limit ) {
  auto a_stateful_instr_width = &mm_regs.mau_stateful_log_instruction_width;
  uint32_t v_stateful_instr_width = InWord((void*)a_stateful_instr_width);
  v_stateful_instr_width &= ~(0x3 << (alu * 2));            // Clear
  v_stateful_instr_width |= (width & 0x3) << (alu * 2); // Set
  OutWord((void*)a_stateful_instr_width, v_stateful_instr_width);

  // Now setup VPN min/max
  auto a_stateful_vpn_offset = &mm_regs.mau_stateful_log_vpn_offset[alu / 2];
  uint32_t v_stateful_vpn_offset = InWord((void*)a_stateful_vpn_offset);
  v_stateful_vpn_offset &= ~(0x3F << ((alu % 2) * 6));           // Clear
  v_stateful_vpn_offset |=  (vpn_offset & 0x3F) << ((alu % 2) * 6); // Set
  OutWord((void*)a_stateful_vpn_offset, v_stateful_vpn_offset);
  auto a_stateful_vpn_limit = &mm_regs.mau_stateful_log_vpn_limit[alu / 2];
  uint32_t v_stateful_vpn_limit = InWord((void*)a_stateful_vpn_limit);
  v_stateful_vpn_limit &= ~(0x3F << ((alu % 2) * 6));           // Clear
  v_stateful_vpn_limit |=  (vpn_limit & 0x3F) << ((alu % 2) * 6); // Set
  OutWord((void*)a_stateful_vpn_limit, v_stateful_vpn_limit);
}
void TestUtil::set_stateful_clear_instruction_regs( Mau_match_merge_addrmap& mm_regs, int alu,
                                                    int width, int vpn_offset, int vpn_limit ) {
  // No such beastie on Tofino
}

void TestUtil::stateful_cntr_config(int pipe, int stage, int log_table, int alu,
                                    int cnt_what, int cntr_shift,
                                    int vpn_min, int vpn_max) {

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

  // First of all must configure ixbar mapping LT->ALU
  auto a_stateful_ixbar0 = &mm_regs.mau_stateful_log_ctl_ixbar_map[log_table / 8][0];
  auto a_stateful_ixbar1 = &mm_regs.mau_stateful_log_ctl_ixbar_map[log_table / 8][1];
  uint32_t v_stateful_ixbar = InWord((void*)a_stateful_ixbar0);
  v_stateful_ixbar &= ~(0x7 << ((log_table % 8) * 3));               // Clear cfg
  v_stateful_ixbar |=  (0x4 | (alu & 0x3)) << ((log_table % 8) * 3); // Set new ALU
  OutWord((void*)a_stateful_ixbar0, v_stateful_ixbar); // Duplicated ixbar reg...
  OutWord((void*)a_stateful_ixbar1, v_stateful_ixbar); // ...so write to both

  // Now configure instruction width corresponding to cntr_shift we're trying to achieve
  // Shift 3 => Config 0 =>  8b instructions
  // Shift 4 => Config 1 => 16b instructions (or dual 8b)
  // Shift 5 => Config 2 => 32b instructions (or dual 16b)
  // Shift 6 => Config 3 => 64b instructions (or dual 32b)
  set_stateful_log_instruction_regs( mm_regs, alu, cntr_shift-3, vpn_min, vpn_max );


  // And finally ctrl reg to specify what we're counting
  // (Note in all cases table MUST be predicated ON)
  // cnt_what 0   => Disabled
  // cnt_what 1   => Count TableMiss
  // cnt_what 2   => Count TableHit
  // cnt_what 3   => Count GW Inhibit
  // cnt_what 4   => Count unconditionally
  // cnt_what 5-7 => Reserved
  auto a_stateful_ctl = &mm_regs.mau_stateful_log_counter_ctl[log_table / 8];
  uint32_t v_stateful_ctl = InWord((void*)a_stateful_ctl);
  v_stateful_ctl &= ~(0x7 << ((log_table % 8) * 3));            // Clear
  v_stateful_ctl |=  (cnt_what & 0x7) << ((log_table % 8) * 3); // Set
  OutWord((void*)a_stateful_ctl, v_stateful_ctl);

  // Finally we write mau_stateful_log_counter_clear reg to reset
  // counter to initial minimum value
  auto a_stateful_clear = &mm_regs.mau_stateful_log_counter_clear;
  uint32_t v_stateful_clear = InWord((void*)a_stateful_clear);
  v_stateful_clear |= (1<<alu);
  OutWord((void*)a_stateful_clear, v_stateful_clear);
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
  auto a_ing2 = &dp_regs.phv_ingress_thread_alu[half][which_word];
  auto a_eg0 = &dp_regs.phv_egress_thread[half][which_word];
  auto a_eg1 = &dp_regs.phv_egress_thread_imem[half][which_word];
  auto a_eg2 = &dp_regs.phv_egress_thread_alu[half][which_word];
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
  OutWord((void*)a_ing2, ing);
  OutWord((void*)a_eg0, eg);
  OutWord((void*)a_eg1, eg);
  OutWord((void*)a_eg2, eg);
  OutWord((void*)a_actmux, actmux);
  OutWord((void*)a_mixbar, mixbar);
}


void TestUtil::set_dump_ctl_regs(int pipe, int stage) {
}

void TestUtil::set_stage_default_regs(int pipe, int stage) {
}

void TestUtil::set_table_default_regs(int pipe, int stage, int table) {
}

void TestUtil::set_teop_regs(int pipe, int stage, int table, int alu, int bus) {
}


// Some funcs to setup Tofino mirror CSRs

void TestUtil::set_mirror_global(int pipe,
                                 bool ing_en, bool egr_en,
                                 bool coal_ing_en, bool coal_egr_en,
                                 uint16_t coal_baseid, uint32_t coal_basetime) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  // Note, coalesce on ingress not supported on Tofino
  assert(!coal_ing_en);
  assert(coal_baseid < RmtDefs::kMirrorNormalSessions);
  assert(coal_basetime > 0u);

  uint32_t glb_ctrl = 0u;
  setp_mir_buf_regs_glb_ctrl_ingr_ena(&glb_ctrl, (ing_en)?1:0);
  setp_mir_buf_regs_glb_ctrl_egr_ena(&glb_ctrl, (egr_en)?1:0);
  setp_mir_buf_regs_glb_ctrl_coalescing_ena(&glb_ctrl, (coal_egr_en)?1:0);
  // XXX negative mirroring is not turned on yet - TODO
  // setp_mir_buf_regs_glb_ctrl_neg_mirror_ena(&glb_ctrl, 1);

  // baseid/basetime for coal sessions
  uint32_t base_id = 0u;
  setp_mir_buf_regs_coalescing_baseid_coal_sid(&base_id, coal_baseid);
  uint32_t base_time = 0u;
  setp_mir_buf_regs_coalescing_basetime_coal_basetime(&base_time, coal_basetime);

  auto mir = RegisterUtils::addr_mirbuf(pipe);
  OutWord(&mir->mir_buf_regs.mir_glb_group.glb_ctrl, glb_ctrl);
  OutWord(&mir->mir_buf_regs.mir_glb_group.coalescing_baseid, base_id);
  OutWord(&mir->mir_buf_regs.mir_glb_group.coalescing_basetime, base_time);
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

  // using new definitions from HW team
  uint32_t meta0 = (((icos & 0x7) << 0) |
                    ((egr_port_v & 0x1) << 3) |
                    ((egr_port & 0x1FF) << 4) |
                    ((eport_qid & 0x1F) << 13) |
                    ((color & 0x3) << 18) |
                    ((pipe_mask & 0xF) << 20) |
                    ((hash1 & 0xFF) << 24));

  uint32_t meta1 = (((hash1 >> 8) & 0x1F) |
                    ((hash2 & 0x1FFF) << 5) |
                    ((mgid1 & 0x3FFF) << 18));

  uint32_t meta2 = (((mgid1 >> 14) & 0x3) |
                    ((mgid1_v & 0x1) << 2) |
                    ((mgid2 & 0xFFFF) << 3) |
                    ((mgid2_v & 0x1) << 19) |
                    ((xid & 0xFFF) << 20));

  uint32_t meta3 = (((xid & 0xF000) >> 12) |
                    ((yid & 0x1FF) << 4) |
                    ((rid & 0xFFFF) << 13) |
                    ((egress_bypass & 0x1) << 29) |
                    ((c2c_cos & 0x3) << 30));

  uint32_t meta4 = (((c2c_cos >> 2) & 0x01) |
                    ((c2c_v & 0x1) << 1) |
                    ((deflect & 0x1) << 2) |
                    ((yid_tbl_sel & 0x1) << 3));

  auto mir = RegisterUtils::addr_mirbuf(pipe);
  OutWord(&mir->mir_buf_desc.norm_desc_grp[sess].session_meta0, meta0);
  OutWord(&mir->mir_buf_desc.norm_desc_grp[sess].session_meta1, meta1);
  OutWord(&mir->mir_buf_desc.norm_desc_grp[sess].session_meta2, meta2);
  OutWord(&mir->mir_buf_desc.norm_desc_grp[sess].session_meta3, meta3);
  OutWord(&mir->mir_buf_desc.norm_desc_grp[sess].session_meta4, meta4);
}
void TestUtil::set_mirror_meta_cfg(int pipe, int sess,
                                   bool hash_cfg, bool icos_cfg, bool dod_cfg,
                                   bool c2c_cfg, bool mc_cfg, bool epipe_cfg) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((sess >= 0) && (sess < RmtDefs::kMirrorNormalSessions));
  // Tofino doesn't support these so barf if called with any set
  assert(!hash_cfg && !icos_cfg && !c2c_cfg && !mc_cfg && !epipe_cfg);
}


void TestUtil::set_mirror_norm_session(int pipe, int slice, int sess,
                                       bool ing_en, bool egr_en,
                                       bool coal_en, uint8_t coal_num,
                                       uint8_t pri, uint8_t max_n,
                                       uint16_t trunc_size) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert(slice == 0);
  assert((sess >= 0) && (sess < RmtDefs::kMirrorNormalSessions));

  uint32_t sess_ctrl = 0u;
  setp_mir_buf_desc_session_ctrl_norm_trunc_size(&sess_ctrl, trunc_size);
  setp_mir_buf_desc_session_ctrl_norm_ingr_ena(&sess_ctrl, (ing_en)?1:0);
  setp_mir_buf_desc_session_ctrl_norm_egr_ena(&sess_ctrl, (egr_en)?1:0);

  auto mir = RegisterUtils::addr_mirbuf(pipe);
  OutWord(&mir->mir_buf_desc.norm_desc_grp[sess].session_ctrl, sess_ctrl);
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
  assert(slice == 0);
  assert((coal_sess >= 0) && (coal_sess < RmtDefs::kMirrorCoalSessions));
  assert(tofino_mode);

  uint32_t csr0 = 0u;
  setp_mir_buf_regs_coal_ctrl0_coal_ena(&csr0, (en)?1:0);
  setp_mir_buf_regs_coal_ctrl0_coal_vid(&csr0, ver);
  setp_mir_buf_regs_coal_ctrl0_coal_timeout(&csr0, coal_timeout);
  setp_mir_buf_regs_coal_ctrl0_coal_minpkt_size(&csr0, min_pkt_size);
  setp_mir_buf_regs_coal_ctrl0_coal_pkthdr_length(&csr0, pkt_hdr_len);

  uint32_t csr1 = 0u;
  setp_mir_buf_regs_coal_ctrl1_coal_extract_length(&csr1, extract_len);
  setp_mir_buf_regs_coal_ctrl1_coal_sflow_type(&csr1, len_from_inp);

  auto mir = RegisterUtils::addr_mirbuf(pipe);
  OutWord(&mir->mir_buf_regs.coal_desc_grp[coal_sess].coal_ctrl0, csr0);
  OutWord(&mir->mir_buf_regs.coal_desc_grp[coal_sess].coal_ctrl1, csr1);
  OutWord(&mir->mir_buf_regs.coal_desc_grp[coal_sess].coal_pkt_header0, hdr0);
  OutWord(&mir->mir_buf_regs.coal_desc_grp[coal_sess].coal_pkt_header1, hdr1);
  OutWord(&mir->mir_buf_regs.coal_desc_grp[coal_sess].coal_pkt_header2, hdr2);
  OutWord(&mir->mir_buf_regs.coal_desc_grp[coal_sess].coal_pkt_header3, hdr3);
}



// Some funcs to setup Tofino packet-gen CSRs
// Note these currently just stubs
// Continue to use old pktgen func in test_util.cpp for moment

void TestUtil::pktgen_app_ctrl_set(int pipe, int app,
                                   bool en, uint8_t type, uint8_t chan, uint8_t prio, uint8_t flg) {
  // Type is 0->timer 1->periodic 2->linkdown 3->recirc 4->dprsr 5->PFC
  // Chan is what chan app belongs to - pktgen pkt ONLY to that chan
  // Prio ignored for now
  // Flags are NoKey|StopAtPktBndry|UsePortDownMask1|UseCurrTs
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((app >= 0) && (app < 8));
  assert((type >= kPgenAppTypeMin) && (type <= kPgenAppTypeMax));
  assert(0); // Not implemented yet
}
void TestUtil::pktgen_app_payload_ctrl_set(int pipe, int app,
                                           uint16_t addr_16B, uint16_t size_B, bool from_recirc) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((app >= 0) && (app < 8));
  assert(0); // Not implemented yet
}
void TestUtil::pktgen_app_ingr_port_ctrl_set(int pipe, int app,
                                             uint8_t ing_port, bool ing_inc, uint8_t ing_wrap,
                                             uint8_t ing_port_pipe_id) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((app >= 0) && (app < 8));
  assert(ing_port <= 72);
  assert(ing_port_pipe_id < kMaxPipes);
  assert(0); // Not implemented yet
}
void TestUtil::pktgen_app_recirc_set(int pipe, int app, int outport,
                                     const BitVector<128> &value, const BitVector<128> &mask) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((app >= 0) && (app < 8));
  assert((outport >= kPgenOutPortMin) && (outport <= kPgenOutPortMax));
}

void TestUtil::pktgen_app_event_set(int pipe, int app, uint16_t batchnum, uint16_t pktnum,
                                    uint32_t ibg_jit_base, uint8_t ibg_jit_max, uint8_t ibg_jit_scl,
                                    uint32_t ipg_jit_base, uint8_t ipg_jit_max, uint8_t ipg_jit_scl,
                                    uint32_t timer_cycles) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((app >= 0) && (app < 8));
  assert(0); // Not implemented yet
}

void TestUtil::pktgen_app_counters(int pipe, int app) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((app >= 0) && (app < 8));
  assert(0); // Not implemented yet
}
void TestUtil::pktgen_cmn_output_port_ctrl_set(int pipe, int outport,
                                               bool en, uint8_t chan_en, uint8_t chan_mode,
                                               uint64_t chan_seq) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert((outport >= kPgenOutPortMin) && (outport <= kPgenOutPortMax));
  assert(0); // Not implemented yet
}
void TestUtil::pktgen_cmn_input_port_ctrl_set(int pipe,
                                              uint32_t recirc_chan_en, uint32_t pgen_chan_en,
                                              uint64_t chan_seq) {
  assert(0); // Not implemented yet
  assert((pipe >= 0) && (pipe < kMaxPipes));
}
void TestUtil::pktgen_cmn_timestamp_set(int pipe, int outport,
                                        uint32_t recirc_ts_off, uint32_t csr_ts_off) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert(0); // Not implemented yet
}
void TestUtil::pktgen_cmn_portdown_ctrl_set(int pipe,
                                            const BitVector<72> &mask0,
                                            const BitVector<72> &mask1) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert(0); // Not implemented yet
}
BitVector<72> TestUtil::pktgen_cmn_portdown_ctrl_get(int pipe, int mask) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert(0); // Not implemented yet
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
// These to setup memories
void TestUtil::pktgen_mem_pkt_set(int pipe, int i, uint64_t data0, uint64_t data1) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert(0); // Not implemented yet
}
void TestUtil::pktgen_mem_meta1_set(int pipe, int i, uint64_t data0, uint64_t data1) {
  assert((pipe >= 0) && (pipe < kMaxPipes));
  assert(0); // Not implemented yet
}

uint64_t TestUtil::deparser_get_learn_counter(int pipe) {
  // return the value of the deparser cnt_i_learn counter
  auto *cnt_i_learn = new_fake_register(
      &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_learn, 48);
  return cnt_i_learn->read();
}

RegPtr TestUtil::lookup_register_map(std::vector<PathElement> path) {
  return Tofino_map().map(path);
}

void TestUtil::set_learn_tbl_entry( volatile Dprsr_learn_table_entry_r& entry,
                                    bool valid, int len, int phvs[48])
{
  uint32_t word=0;
  setp_dprsr_learn_table_entry_r_learn_tbl_12_13_valid (&word, valid? 1:0 );
  setp_dprsr_learn_table_entry_r_learn_tbl_12_13_len (&word, len);
  OutWord( &entry.learn_tbl_12_13 , word );

  word=0;
  setp_dprsr_learn_table_entry_r_learn_tbl_0_13_phvs_0 ( &word, phvs[0] );
  setp_dprsr_learn_table_entry_r_learn_tbl_0_13_phvs_1 ( &word, phvs[1] );
  setp_dprsr_learn_table_entry_r_learn_tbl_0_13_phvs_2 ( &word, phvs[2] );
  setp_dprsr_learn_table_entry_r_learn_tbl_0_13_phvs_3 ( &word, phvs[3] );
  OutWord( &entry.learn_tbl_0_13 , word );
  word=0;
  setp_dprsr_learn_table_entry_r_learn_tbl_1_13_phvs_4 ( &word, phvs[4] );
  setp_dprsr_learn_table_entry_r_learn_tbl_1_13_phvs_5 ( &word, phvs[5] );
  setp_dprsr_learn_table_entry_r_learn_tbl_1_13_phvs_6 ( &word, phvs[6] );
  setp_dprsr_learn_table_entry_r_learn_tbl_1_13_phvs_7 ( &word, phvs[7] );
  OutWord( &entry.learn_tbl_1_13 , word );
  word=0;
  setp_dprsr_learn_table_entry_r_learn_tbl_2_13_phvs_8 ( &word, phvs[8] );
  setp_dprsr_learn_table_entry_r_learn_tbl_2_13_phvs_9 ( &word, phvs[9] );
  setp_dprsr_learn_table_entry_r_learn_tbl_2_13_phvs_10 ( &word, phvs[10] );
  setp_dprsr_learn_table_entry_r_learn_tbl_2_13_phvs_11 ( &word, phvs[11] );
  OutWord( &entry.learn_tbl_2_13 , word );
  word=0;
  setp_dprsr_learn_table_entry_r_learn_tbl_3_13_phvs_12 ( &word, phvs[12] );
  setp_dprsr_learn_table_entry_r_learn_tbl_3_13_phvs_13 ( &word, phvs[13] );
  setp_dprsr_learn_table_entry_r_learn_tbl_3_13_phvs_14 ( &word, phvs[14] );
  setp_dprsr_learn_table_entry_r_learn_tbl_3_13_phvs_15 ( &word, phvs[15] );
  OutWord( &entry.learn_tbl_3_13 , word );
  word=0;
  setp_dprsr_learn_table_entry_r_learn_tbl_4_13_phvs_16 ( &word, phvs[16] );
  setp_dprsr_learn_table_entry_r_learn_tbl_4_13_phvs_17 ( &word, phvs[17] );
  setp_dprsr_learn_table_entry_r_learn_tbl_4_13_phvs_18 ( &word, phvs[18] );
  setp_dprsr_learn_table_entry_r_learn_tbl_4_13_phvs_19 ( &word, phvs[19] );
  OutWord( &entry.learn_tbl_4_13 , word );
  word=0;
  setp_dprsr_learn_table_entry_r_learn_tbl_5_13_phvs_20 ( &word, phvs[20] );
  setp_dprsr_learn_table_entry_r_learn_tbl_5_13_phvs_21 ( &word, phvs[21] );
  setp_dprsr_learn_table_entry_r_learn_tbl_5_13_phvs_22 ( &word, phvs[22] );
  setp_dprsr_learn_table_entry_r_learn_tbl_5_13_phvs_23 ( &word, phvs[23] );
  OutWord( &entry.learn_tbl_5_13 , word );
  word=0;
  setp_dprsr_learn_table_entry_r_learn_tbl_6_13_phvs_24 ( &word, phvs[24] );
  setp_dprsr_learn_table_entry_r_learn_tbl_6_13_phvs_25 ( &word, phvs[25] );
  setp_dprsr_learn_table_entry_r_learn_tbl_6_13_phvs_26 ( &word, phvs[26] );
  setp_dprsr_learn_table_entry_r_learn_tbl_6_13_phvs_27 ( &word, phvs[27] );
  OutWord( &entry.learn_tbl_6_13 , word );
  word=0;
  setp_dprsr_learn_table_entry_r_learn_tbl_7_13_phvs_28 ( &word, phvs[28] );
  setp_dprsr_learn_table_entry_r_learn_tbl_7_13_phvs_29 ( &word, phvs[29] );
  setp_dprsr_learn_table_entry_r_learn_tbl_7_13_phvs_30 ( &word, phvs[30] );
  setp_dprsr_learn_table_entry_r_learn_tbl_7_13_phvs_31 ( &word, phvs[31] );
  OutWord( &entry.learn_tbl_7_13 , word );
  word=0;
  setp_dprsr_learn_table_entry_r_learn_tbl_8_13_phvs_32 ( &word, phvs[32] );
  setp_dprsr_learn_table_entry_r_learn_tbl_8_13_phvs_33 ( &word, phvs[33] );
  setp_dprsr_learn_table_entry_r_learn_tbl_8_13_phvs_34 ( &word, phvs[34] );
  setp_dprsr_learn_table_entry_r_learn_tbl_8_13_phvs_35 ( &word, phvs[35] );
  OutWord( &entry.learn_tbl_8_13 , word );
  word=0;
  setp_dprsr_learn_table_entry_r_learn_tbl_9_13_phvs_36 ( &word, phvs[36] );
  setp_dprsr_learn_table_entry_r_learn_tbl_9_13_phvs_37 ( &word, phvs[37] );
  setp_dprsr_learn_table_entry_r_learn_tbl_9_13_phvs_38 ( &word, phvs[38] );
  setp_dprsr_learn_table_entry_r_learn_tbl_9_13_phvs_39 ( &word, phvs[39] );
  OutWord( &entry.learn_tbl_9_13 , word );
  word=0;
  setp_dprsr_learn_table_entry_r_learn_tbl_10_13_phvs_40 ( &word, phvs[40] );
  setp_dprsr_learn_table_entry_r_learn_tbl_10_13_phvs_41 ( &word, phvs[41] );
  setp_dprsr_learn_table_entry_r_learn_tbl_10_13_phvs_42 ( &word, phvs[42] );
  setp_dprsr_learn_table_entry_r_learn_tbl_10_13_phvs_43 ( &word, phvs[43] );
  OutWord( &entry.learn_tbl_10_13 , word );
  word=0;
  setp_dprsr_learn_table_entry_r_learn_tbl_11_13_phvs_44 ( &word, phvs[44] );
  setp_dprsr_learn_table_entry_r_learn_tbl_11_13_phvs_45 ( &word, phvs[45] );
  setp_dprsr_learn_table_entry_r_learn_tbl_11_13_phvs_46 ( &word, phvs[46] );
  setp_dprsr_learn_table_entry_r_learn_tbl_11_13_phvs_47 ( &word, phvs[47] );
  OutWord( &entry.learn_tbl_11_13 , word );
}


void TestUtil::learn_config(int pipe, bool valid, int phv_for_table_index,
                            // these parameters currently ignored, all entries are
                            //  programmed to pick up the SA
                            int table_entry_index,int length,int phvs[48]) {

  //auto& learn_registers = kTofinoPtr->pipes[pipe].deparser.inp.iir.ingr;
  auto& learn_registers = RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr;

  uint32_t dprsr_cfg=0;
  setp_dprsr_learn_cfg_r_valid( &dprsr_cfg, valid ? 1 : 0 );
  setp_dprsr_learn_cfg_r_phv ( &dprsr_cfg, phv_for_table_index );
  OutWord( &learn_registers.learn_cfg, dprsr_cfg );

  // for now, write all the table entries the same, so I don't rely on any PHVs
  //auto& learn_table_entry = learn_registers.learn_tbl[table_entry_index];
  for (int i=0;i<8;++i) {
    int sa_phvs[48] {};
    int len=0;
    sa_phvs[0] = Phv::make_word(4,1); // SA_HI_16
    sa_phvs[1] = Phv::make_word(4,1); // SA_HI_16
    len += 2;
    sa_phvs[2] = Phv::make_word(0,1); // SA_LO_32
    sa_phvs[3] = Phv::make_word(0,1); // SA_LO_32
    sa_phvs[4] = Phv::make_word(0,1); // SA_LO_32
    sa_phvs[5] = Phv::make_word(0,1); // SA_LO_32
    len += 4;
    auto& learn_table_entry = learn_registers.learn_tbl[i];
    set_learn_tbl_entry(learn_table_entry,true,len,sa_phvs);
  }
}


uint32_t TestUtil::deparser_create_info_word(int phv, bool valid,
                                             uint16_t default_value,
                                             uint32_t default_value_offset,
                                             const uint8_t &shift,
                                             const uint8_t &shift_offset) {
  uint32_t info_word;
  info_word = phv & 0xFF;
  if (valid) info_word |= (1 << 8);
  info_word |= (((uint32_t)default_value) << default_value_offset);
  if (shift != 0) {
    info_word |= ((shift & 0x07) << shift_offset);
  }
  return info_word;
}

uint32_t TestUtil::deparser_create_info_word(int phv, bool valid,
                                             bool default_valid,
                                             uint16_t default_value,
                                             uint32_t default_value_offset,
                                             const uint8_t &shift,
                                             const uint8_t &shift_offset) {
  uint32_t info_word = deparser_create_info_word(phv, valid, default_value, default_value_offset, shift, shift_offset);
  if (default_valid) info_word |= (1 << 15);
  return info_word;
}

void TestUtil::deparser_set_capture_tx_ts_info(int pipe, int phv, bool valid, bool default_value) {
  //auto& capture_tx_ts_reg = kTofinoPtr->pipes[pipe].deparser.hdr.her.egr.capture_tx_ts;
  auto& capture_tx_ts_reg = RegisterUtils::addr_dprsr(pipe)->hdr.her.egr.capture_tx_ts;
  OutWord(&capture_tx_ts_reg, deparser_create_info_word(phv, valid, default_value));
}

void TestUtil::deparser_set_copy_to_cpu_info(int pipe, int phv, bool valid,
                                             uint16_t default_value,
                                             const uint8_t &shift) {
  //auto& copy_to_cpu_reg = kTofinoPtr->pipes[pipe].deparser.inp.iir.ingr.copy_to_cpu;
  auto& copy_to_cpu_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.copy_to_cpu;
  OutWord(&copy_to_cpu_reg, deparser_create_info_word(phv, valid, default_value, 16, shift, 20));
}

void TestUtil::deparser_set_copy_to_cpu_cos_info(int pipe, int phv, bool valid, uint16_t default_value) {
  //auto& copy_to_cpu_cos_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.copy_to_cpu_cos;
  auto& copy_to_cpu_cos_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.copy_to_cpu_cos;
  OutWord(&copy_to_cpu_cos_reg, deparser_create_info_word(phv, valid, default_value));
}

void TestUtil::deparser_set_copy_to_cpu_pipe_vector(int pipe, uint8_t pipe_vector) {
  //auto& copy_to_cpu_pipe_vector_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.copy_to_cpu_pv;
  auto& copy_to_cpu_pipe_vector_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.copy_to_cpu_pv;
  OutWord(&copy_to_cpu_pipe_vector_reg, pipe_vector);
}

void TestUtil::deparser_set_ct_disable_info(int pipe, int phv, bool valid, uint16_t default_value) {
  //auto& ct_disable_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.ct_disable;
  auto& ct_disable_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.ct_disable;
  OutWord(&ct_disable_reg, deparser_create_info_word(phv, valid, default_value, 9));
}

void TestUtil::deparser_set_ct_mcast_info(int pipe, int phv, bool valid, uint16_t default_value) {
  //auto& ct_mcast_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.ct_mcast;
  auto& ct_mcast_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.ct_mcast;
  OutWord(&ct_mcast_reg, deparser_create_info_word(phv, valid, default_value, 9));
}

void TestUtil::deparser_set_deflect_on_drop_info(int pipe, int phv, bool valid, uint16_t default_value) {
  //auto& deflect_on_drop_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.deflect_on_drop;
  auto& deflect_on_drop_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.deflect_on_drop;
  OutWord(&deflect_on_drop_reg, deparser_create_info_word(phv, valid, default_value));
}

void TestUtil::deparser_set_drop_ctl_info(int pipe, int phv, bool valid, uint16_t default_value, bool is_ingress) {
  //auto& drop_ctl_reg = kTofinoPtr->pipes[pipe].deparser.inp.iir.main_i.drop_ctl;
  if (is_ingress) {
    auto& drop_ctl_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.drop_ctl;
    OutWord(&drop_ctl_reg, deparser_create_info_word(phv, valid, default_value, 9));
  } else {
    auto& drop_ctl_reg = RegisterUtils::addr_dprsr(pipe)->inp.ier.main_e.drop_ctl;
    OutWord(&drop_ctl_reg, deparser_create_info_word(phv, valid, default_value, 9));
  }
}

void TestUtil::deparser_set_ecos_info(int pipe, int phv, bool valid, uint16_t default_value) {
  //auto& ecos_reg = kTofinoPtr->pipes[pipe].deparser.hdr.her.egr.ecos;
  auto& ecos_reg = RegisterUtils::addr_dprsr(pipe)->hdr.her.egr.ecos;
  OutWord(&ecos_reg, deparser_create_info_word(phv, valid, default_value, 9));
}

void TestUtil::deparser_set_egress_unicast_port_info(int pipe, int phv, bool valid, bool default_valid, uint16_t default_value) {
  //auto& egress_unicast_port_reg = kTofinoPtr->pipes[pipe].deparser.inp.iir.main_i.egress_unicast_port;
  auto& egress_unicast_port_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.egress_unicast_port;
  OutWord(&egress_unicast_port_reg, deparser_create_info_word(phv, valid, default_valid, default_value));
  //auto& egress_unicast_port_reg_e = kTofinoPtr->pipes[pipe].deparser.inp.ier.main_e.egress_unicast_port;
  auto& egress_unicast_port_reg_e = RegisterUtils::addr_dprsr(pipe)->inp.ier.main_e.egress_unicast_port;
  OutWord(&egress_unicast_port_reg_e, deparser_create_info_word(phv, valid, default_valid, default_value));
}

void TestUtil::deparser_set_force_tx_error_info(int pipe, int phv, bool valid, bool default_value) {
  //auto& force_tx_error_reg = kTofinoPtr->pipes[pipe].deparser.hdr.her.egr.force_tx_err;
  auto& force_tx_error_reg = RegisterUtils::addr_dprsr(pipe)->hdr.her.egr.force_tx_err;
  OutWord(&force_tx_error_reg, deparser_create_info_word(phv, valid, default_value));
}

void TestUtil::deparser_set_hash1_info(int pipe, int phv, bool valid, uint16_t default_value) {
  //auto& hash1_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.hash_lag_ecmp_mcast[0];
  auto& hash1_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.hash_lag_ecmp_mcast[0];
  OutWord(&hash1_reg, deparser_create_info_word(phv, valid, default_value));
}

void TestUtil::deparser_set_hash2_info(int pipe, int phv, bool valid, uint16_t default_value) {
  //auto& hash2_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.hash_lag_ecmp_mcast[1];
  auto& hash2_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.hash_lag_ecmp_mcast[1];
  OutWord(&hash2_reg, deparser_create_info_word(phv, valid, default_value));
}

void TestUtil::deparser_set_icos_info(int pipe, int phv, bool valid, uint16_t default_value) {
  //auto& icos_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.icos;
  auto& icos_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.icos;
  OutWord(&icos_reg, deparser_create_info_word(phv, valid, default_value));
}

void TestUtil::deparser_set_meter_color_info(int pipe, int phv, bool valid, uint16_t default_value) {
  //auto& meter_color_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.meter_color;
  auto& meter_color_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.meter_color;
  OutWord(&meter_color_reg, deparser_create_info_word(phv, valid, default_value));
}

void TestUtil::deparser_set_mgid1_info(int pipe, int phv, bool valid, bool default_valid, uint16_t default_value) {
  //auto& mgid1_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.egress_multicast_group[0];
  auto& mgid1_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.egress_multicast_group[0];
  OutWord(&mgid1_reg, deparser_create_info_word(phv, valid, default_valid, default_value));
}

void TestUtil::deparser_set_mgid2_info(int pipe, int phv, bool valid, bool default_valid, uint16_t default_value) {
  //auto& mgid2_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.egress_multicast_group[1];
  auto& mgid2_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.egress_multicast_group[1];
  OutWord(&mgid2_reg, deparser_create_info_word(phv, valid, default_valid, default_value));
}

void TestUtil::deparser_set_mirror_cfg(int pipe, bool is_ingress, uint8_t phv, bool valid) {
  //auto& imirror_cfg_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.main_i.mirror_cfg;
  //auto& emirror_cfg_reg = kTofinoPtr->pipes[pipe].deparser.hdr.her.main_e.mirror_cfg;
  auto& imirror_cfg_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.main_i.mirror_cfg;
  auto& emirror_cfg_reg = RegisterUtils::addr_dprsr(pipe)->hdr.her.main_e.mirror_cfg;
  uint32_t value = ((uint32_t)phv) | (valid ? (1ul << 8) : 0);
  if (is_ingress) {
    OutWord(&imirror_cfg_reg, value);
  }
  else {
    OutWord(&emirror_cfg_reg, value);
  }
}

void TestUtil::deparser_set_mirror_metadata(int pipe, int table_entry_idx, bool is_ingress, uint8_t metadata_phv_idx[32]) {
  for (int i = 0; i < 12; ++i) {
    //auto& imirror_table_entry_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.main_i.mirror_tbl[table_entry_idx].mirror_tbl_0_9;
    //auto& emirror_table_entry_reg = kTofinoPtr->pipes[pipe].deparser.hdr.her.main_e.mirror_tbl[table_entry_idx].mirror_tbl_0_9;
    auto& imirror_table_entry_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.main_i.mirror_tbl[table_entry_idx].mirror_tbl_0_9;
    auto& emirror_table_entry_reg = RegisterUtils::addr_dprsr(pipe)->hdr.her.main_e.mirror_tbl[table_entry_idx].mirror_tbl_0_9;
    uint32_t value = 0;
    for (int j = 0; j < 4; ++j) {
      value |= (metadata_phv_idx[i*4 + j] << (j*8));
    }
    if (is_ingress) {
      OutWord(&imirror_table_entry_reg + i, value);
    }
    else {
      OutWord(&emirror_table_entry_reg + i, value);
    }
  }
}

void TestUtil::deparser_set_mirror_table_entry(int pipe, int table_entry_idx, bool is_ingress, uint8_t id_phv, uint8_t len, bool valid) {
  //auto& imirror_table_entry_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.main_i.mirror_tbl[table_entry_idx].mirror_tbl_8_9;
  //auto& emirror_table_entry_reg = kTofinoPtr->pipes[pipe].deparser.hdr.her.main_e.mirror_tbl[table_entry_idx].mirror_tbl_8_9;
  auto& imirror_table_entry_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.main_i.mirror_tbl[table_entry_idx].mirror_tbl_8_9;
  auto& emirror_table_entry_reg = RegisterUtils::addr_dprsr(pipe)->hdr.her.main_e.mirror_tbl[table_entry_idx].mirror_tbl_8_9;
  uint32_t value = ((uint32_t)id_phv) | ((uint32_t)(len & 0x3Fu) << 8) | (valid ? (1ul << 14) : 0);
  if (is_ingress) {
    OutWord(&imirror_table_entry_reg, value);
  }
  else {
    OutWord(&emirror_table_entry_reg, value);
  }
}

void TestUtil::deparser_set_physical_ingress_port_info(int pipe, uint8_t phv, bool sel) {
  //auto& physical_ingress_port_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.ingress_port;
  auto& physical_ingress_port_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.ingress_port;
  uint32_t value = phv;
  value |= (sel ? (1ul << 8ul) : 0ul);
  OutWord(&physical_ingress_port_reg, value);
}

void TestUtil::deparser_set_pipe_vector_table(int pipe, int table_num, uint16_t index, uint8_t pipe_vector) {
  assert(table_num == 0 || table_num == 1);

  int sub_bits=0;
  int vector_bits=0;
  if (RmtObject::is_tofino_or_later()) {
    sub_bits = 3;
    vector_bits = 4;
  }
  else {
    RMT_ASSERT(0);
  }
  int sub_bits_mask = (1<<sub_bits) - 1;
  int vector_bits_mask = (1<<vector_bits) - 1;

  index &= 0x7FFF;
  auto& pipe_vector_table0_reg = RegisterUtils::addr_dprsr(pipe)->hdr.him.hi_pv_table.tbl0[index >> sub_bits];
  //auto& pipe_vector_table1_reg = RegisterUtils::addr_dprsr(pipe)->hdr.him.hi_pv_table.tbl1[index >> sub_bits];
  auto& pipe_vector_table1_reg = RegisterUtils::addr_dprsr(pipe)->hdr.him.hi_pv_table.tbl0[4096+(index >> sub_bits)];
  decltype(pipe_vector_table1_reg) pipe_vector_table_reg = (table_num == 0 ? pipe_vector_table0_reg : pipe_vector_table1_reg);

  auto value = InWord(&pipe_vector_table_reg);
  uint32_t offset = ((index & sub_bits_mask) * vector_bits);
  uint32_t mask = vector_bits_mask << offset;
  value &= (~mask);
  value |= (mask & ((uint32_t)pipe_vector) << offset);
  OutWord(&pipe_vector_table_reg, value);
}

void TestUtil::deparser_set_qid_info(int pipe, int phv, bool valid, uint16_t default_value) {
  //auto& qid_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.qid;
  auto& qid_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.qid;
  OutWord(&qid_reg, deparser_create_info_word(phv, valid, default_value));
}

void TestUtil::deparser_set_resubmit_cfg(int pipe, bool valid, uint8_t phv) {
  //auto& resub_cfg_reg = kTofinoPtr->pipes[pipe].deparser.inp.iir.ingr.resub_cfg;
  auto resub_cfg_reg = RegisterUtils::resub_cfg_reg(pipe);
  uint32_t value = phv;
  value |= (valid ? 1 << 8 : 0);
  OutWord(resub_cfg_reg, value);
}

void TestUtil::deparser_set_resubmit_table_entry(int pipe, uint8_t table_idx, bool valid, int len, const std::array<uint8_t, 8> metadata_phv_idx) {
  //auto& resub_tbl_reg = kTofinoPtr->pipes[pipe].deparser.inp.iir.ingr.resub_tbl[table_idx].resub_tbl_0_3;
  auto& resub_tbl_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.resub_tbl[table_idx].resub_tbl_0_3;
  for (int i = 0; i < 2; ++i) {
    uint32_t value = 0;
    for (int j = 0; j < 4; ++j) {
      uint32_t phv_idx = metadata_phv_idx[(i * 4) + j];
      value |= (phv_idx << (j * 8));
    }
    OutWord(&resub_tbl_reg + i, value);
  }

  uint32_t value = (len & 0x0F);
  value |= (valid ? 1 << 4 : 0);
  OutWord(&resub_tbl_reg + 2, value);
}

void TestUtil::deparser_set_rid_info(int pipe, int phv, bool valid, uint16_t default_value) {
  //auto& rid_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.rid;
  auto& rid_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.rid;
  OutWord(&rid_reg, deparser_create_info_word(phv, valid, default_value));
}

void TestUtil::deparser_set_tx_pkt_has_offsets_info(int pipe, int phv, bool valid, bool default_value) {
  //auto& tx_pkt_has_offsets_reg = kTofinoPtr->pipes[pipe].deparser.hdr.her.egr.tx_pkt_has_offsets;
  auto& tx_pkt_has_offsets_reg = RegisterUtils::addr_dprsr(pipe)->hdr.her.egr.tx_pkt_has_offsets;
  OutWord(&tx_pkt_has_offsets_reg, deparser_create_info_word(phv, valid, default_value));
}

void TestUtil::deparser_set_use_yid_tbl_info(int pipe, uint32_t value) {
  //auto& use_yid_tbl_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.yid_tbl;
  auto& use_yid_tbl_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.yid_tbl;
  OutWord(&use_yid_tbl_reg, value);
}

void TestUtil::deparser_set_bypass_egr_mode_info(int pipe, int phv, bool valid, uint16_t default_value) {
  //auto& mode_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.bypss_egr;
  auto& mode_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.bypss_egr;
  OutWord(&mode_reg, deparser_create_info_word(phv, valid, default_value));
}

void TestUtil::deparser_set_xid_info(int pipe, int phv, bool valid, uint16_t default_value) {
  //auto& xid_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.xid;
  auto& xid_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.xid;
  OutWord(&xid_reg, deparser_create_info_word(phv, valid, default_value));
}

void TestUtil::deparser_set_yid_info(int pipe, int phv, bool valid, uint16_t default_value) {
  //auto& yid_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hir.ingr.yid;
  auto& yid_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hir.ingr.yid;
  OutWord(&yid_reg, deparser_create_info_word(phv, valid, default_value));
}

void TestUtil::deparser_set_phv_group_config(volatile uint32_t *group_reg,
                                             const int &group,
                                             const int &valid) {
  auto value = InWord(group_reg);
  if ((valid & 0x01ul) != 0) {
    OutWord(group_reg, value | (1u << group));
  }
  else {
    OutWord(group_reg, value & ~(1u << group));
  }
}

void TestUtil::deparser_set_phv_split_config(volatile uint32_t *split_reg,
                                             const int &split_offset,
                                             const int &valid) {
  auto value = InWord(split_reg);
  if ((valid & 0x01ul) != 0) {
    OutWord(split_reg, value | (1u << split_offset));
  }
  else {
    OutWord(split_reg, value & ~(1u << split_offset));
  }
}

void TestUtil::deparser_set_field_dictionary_entry(int pipe, int index, int valid,
                                         int phv0, int phv1, int phv2, int phv3,
                                         int pov_sel, int num_bytes,int version) {
  assert(index < Deparser::kNumFieldDictionaryEntries);
  // TODO: make able todo egree too
  //auto& fde_pov_reg = kTofinoPtr->pipes[pipe].deparser.inp.iim.ii_fde_pov.fde_pov[index];
  //auto& fde_phv_reg = kTofinoPtr->pipes[pipe].deparser.hdr.him.hi_fde_phv.fde_phv[index];
  auto& fde_pov_reg = RegisterUtils::addr_dprsr(pipe)->inp.iim.ii_fde_pov.fde_pov[index];
  auto& fde_phv_reg = RegisterUtils::addr_dprsr(pipe)->hdr.him.hi_fde_phv.fde_phv[index];

  Dprsr_fde_phv_r temp_phv_reg{};
  setp_dprsr_fde_phv_r_num_bytes ( &temp_phv_reg, num_bytes );
  setp_dprsr_fde_phv_r_phv( &temp_phv_reg, 0, phv0 );
  setp_dprsr_fde_phv_r_phv( &temp_phv_reg, 1, phv1 );
  setp_dprsr_fde_phv_r_phv( &temp_phv_reg, 2, phv2 );
  setp_dprsr_fde_phv_r_phv( &temp_phv_reg, 3, phv3 );
  OutWord( &fde_phv_reg.fde_phv_0_2, temp_phv_reg.fde_phv_0_2);
  OutWord( &fde_phv_reg.fde_phv_1_2, temp_phv_reg.fde_phv_1_2);

  uint32_t temp_pov_reg = 0;
  setp_dprsr_fde_pov_r_valid ( &temp_pov_reg, valid );
  setp_dprsr_fde_pov_r_num_bytes (  &temp_pov_reg, num_bytes );
  setp_dprsr_fde_pov_r_version (  &temp_pov_reg, version );
  setp_dprsr_fde_pov_r_pov_sel (  &temp_pov_reg, pov_sel );
  OutWord( &fde_pov_reg, temp_pov_reg);

  int phv_idxs[4] = { phv0, phv1, phv2, phv3 };
  for (int i = 0; i < ((num_bytes & 0x03) == 0 ? 4 : (num_bytes & 0x03)); ++i) {
    auto phv_idx = phv_idxs[i];
    if (phv_idx < 60) {
      //auto& group_reg = kTofinoPtr->pipes[pipe].deparser.inp.iir.ingr.phv32_grp;
      auto& group_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.phv32_grp;
      deparser_set_phv_group_config(&group_reg, phv_idx >> 2, valid);
    }
    else if (phv_idx < 64) {
      //auto& split_reg = kTofinoPtr->pipes[pipe].deparser.inp.iir.ingr.phv32_split;
      auto& split_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.phv32_split;
      deparser_set_phv_split_config(&split_reg, phv_idx - 60, valid);
    }
    else if (phv_idx < 120) {
      //auto& group_reg = kTofinoPtr->pipes[pipe].deparser.inp.iir.ingr.phv8_grp;
      auto& group_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.phv8_grp;
      deparser_set_phv_group_config(&group_reg, (phv_idx - 64) >> 3, valid);
    }
    else if (phv_idx < 128) {
      //auto& split_reg = kTofinoPtr->pipes[pipe].deparser.inp.iir.ingr.phv8_split;
      auto& split_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.phv8_split;
      deparser_set_phv_split_config(&split_reg, phv_idx - 120, valid);
    }
    else if (phv_idx < 216) {
      //auto& group_reg = kTofinoPtr->pipes[pipe].deparser.inp.iir.ingr.phv16_grp;
      auto& group_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.phv16_grp;
      deparser_set_phv_group_config(&group_reg, (phv_idx - 128) >> 3, valid);
    }
    else if (phv_idx < 224) {
      //auto& split_reg = kTofinoPtr->pipes[pipe].deparser.inp.iir.ingr.phv16_split;
      auto& split_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.phv16_split;
      deparser_set_phv_split_config(&split_reg, phv_idx - 216, valid);
    }
    else {
      // T-PHV does not need to be configured here.
    }
  }
}

void
TestUtil::deparser_set_csum_cfg_entry(int pipe, int csum_idx,
                                      int csum_entry_idx, bool swap,
                                      bool zero_m_s_b, bool zero_l_s_b) {
  uint32_t temp_csum_cfg_reg = 0u;
  setp_dprsr_csum_row_entry_swap( &temp_csum_cfg_reg, swap );
  setp_dprsr_csum_row_entry_zero_m_s_b( &temp_csum_cfg_reg, zero_m_s_b );
  setp_dprsr_csum_row_entry_zero_l_s_b( &temp_csum_cfg_reg, zero_l_s_b );
  if (csum_idx < 6) {
    //auto& csum_cfg_entry_reg = kTofinoPtr->pipes[pipe].deparser.inp.iim.ii_phv_csum.csum_cfg[csum_idx].csum_cfg_entry[csum_entry_idx];
    auto& csum_cfg_entry_reg = RegisterUtils::addr_dprsr(pipe)->inp.iim.ii_phv_csum.csum_cfg[csum_idx].csum_cfg_entry[csum_entry_idx];
    OutWord( &csum_cfg_entry_reg, temp_csum_cfg_reg );
  }
  else {
    //auto& csum_cfg_entry_reg = kTofinoPtr->pipes[pipe].deparser.inp.iem.ie_phv_csum.csum_cfg[csum_idx-6].csum_cfg_entry[csum_entry_idx];
    auto& csum_cfg_entry_reg = RegisterUtils::addr_dprsr(pipe)->inp.iem.ie_phv_csum.csum_cfg[csum_idx-6].csum_cfg_entry[csum_entry_idx];
    OutWord( &csum_cfg_entry_reg, temp_csum_cfg_reg );
  }
}

uint32_t
TestUtil::deparser_get_csum_cfg_entry(int pipe, int csum_idx,
                                      int csum_entry_idx) {
  // Do not use this function! The csum cfg entries are not readable, so it won't work!
  RMT_ASSERT(0);
  if (csum_idx < 6) {
    //auto& csum_cfg_entry_reg = kTofinoPtr->pipes[pipe].deparser.inp.iim.ii_phv_csum.csum_cfg[csum_idx].csum_cfg_entry[csum_entry_idx];
    auto& csum_cfg_entry_reg = RegisterUtils::addr_dprsr(pipe)->inp.iim.ii_phv_csum.csum_cfg[csum_idx].csum_cfg_entry[csum_entry_idx];
    return InWord( &csum_cfg_entry_reg );
  }
  else {
    //auto& csum_cfg_entry_reg = kTofinoPtr->pipes[pipe].deparser.inp.iem.ie_phv_csum.csum_cfg[csum_idx-6].csum_cfg_entry[csum_entry_idx];
    auto& csum_cfg_entry_reg = RegisterUtils::addr_dprsr(pipe)->inp.iem.ie_phv_csum.csum_cfg[csum_idx-6].csum_cfg_entry[csum_entry_idx];
    return InWord( &csum_cfg_entry_reg );
  }
}

void
TestUtil::deparser_set_tphv_csum_cfg_entry(int pipe, int csum_engine_idx,
                                           int csum_entry_idx, bool swap,
                                           bool zero_msb, bool zero_lsb) {
  uint32_t temp_csum_cfg_reg = 0u;
  setp_dprsr_csum_row_entry_swap( &temp_csum_cfg_reg, swap );
  setp_dprsr_csum_row_entry_zero_m_s_b( &temp_csum_cfg_reg, zero_msb );
  setp_dprsr_csum_row_entry_zero_l_s_b( &temp_csum_cfg_reg, zero_lsb );
  if (csum_engine_idx < 6) {
    //auto& csum_cfg_entry_reg = kTofinoPtr->pipes[pipe].deparser.hdr.him.hi_tphv_csum.csum_cfg[csum_engine_idx].csum_cfg_entry[csum_entry_idx];
    auto& csum_cfg_entry_reg = RegisterUtils::addr_dprsr(pipe)->hdr.him.hi_tphv_csum.csum_cfg[csum_engine_idx].csum_cfg_entry[csum_entry_idx];
    OutWord( &csum_cfg_entry_reg, temp_csum_cfg_reg );
  }
  else {
    //auto& csum_cfg_entry_reg = kTofinoPtr->pipes[pipe].deparser.hdr.hem.he_tphv_csum.csum_cfg[csum_engine_idx-6].csum_cfg_entry[csum_entry_idx];
    auto& csum_cfg_entry_reg = RegisterUtils::addr_dprsr(pipe)->hdr.hem.he_tphv_csum.csum_cfg[csum_engine_idx-6].csum_cfg_entry[csum_entry_idx];
    OutWord( &csum_cfg_entry_reg, temp_csum_cfg_reg );
  }
}

void TestUtil::deparser_init(Deparser *dp) {
  int pipe=0; //TODO: this should be setable
  // TODO: this just works on the ingress deparser, needs to be updated
  //   to handle the egress deparser
  //auto& pov_pos_reg = kTofinoPtr->pipes[pipe].deparser.inp.iir.main_i.pov;
  auto& pov_pos_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.pov;

  Dprsr_pov_position_r temp{}; // a wide register
  for (int i=0; i<32; i++) {
    setp_dprsr_pov_position_r_phvs(&temp,i,Phv::make_word(3,1) /*PHV8_1*/);
  }
  OutWord( &pov_pos_reg.pov_0_8 , temp.pov_0_8 );
  OutWord( &pov_pos_reg.pov_1_8 , temp.pov_1_8 );
  OutWord( &pov_pos_reg.pov_2_8 , temp.pov_2_8 );
  OutWord( &pov_pos_reg.pov_3_8 , temp.pov_3_8 );
  OutWord( &pov_pos_reg.pov_4_8 , temp.pov_4_8 );
  OutWord( &pov_pos_reg.pov_5_8 , temp.pov_5_8 );
  OutWord( &pov_pos_reg.pov_6_8 , temp.pov_6_8 );
  OutWord( &pov_pos_reg.pov_7_8 , temp.pov_7_8 );

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
  deparser_set_field_dictionary_entry(pipe,123, 1, IP4_SRC, IP4_SRC, IP4_SRC, IP4_DST, 0, 0, 0x0F);
  deparser_set_field_dictionary_entry(pipe,133, 1, IP4_DST, IP4_DST, IP4_DST, P_SPORT, 0, 0, 0x0F);
  deparser_set_field_dictionary_entry(pipe,190, 1, P_SPORT, P_DPORT, P_DPORT, PHV8_0, 0, 0, 0x0F);
  deparser_set_field_dictionary_entry(pipe,191, 1, PHV8_1, PHV8_2, PHV8_3, 0, 0, 3, 0x0F);

  // Set a default egress port so that packet is not dropped at deparser.
  deparser_set_egress_unicast_port_info(pipe, 0, false, true, 0x0011u);

  deparser_config_basic_eth_ip_tcp(dp);
}

void TestUtil::pktgen_portdown_en(int pipe, uint16_t port) {
  if ((pipe < 0) || (pipe >= kMaxPipes)) return;
  //auto& pgr_common_reg = kTofinoPtr->pipes[pipe].pmarb.pgr_reg.pgr_common;
  auto& pgr_common_reg = RegisterUtils::addr_pmarb(pipe)->pgr_reg.pgr_common;
  uint32_t val = port & 0x7F;
  uint32_t csr = 0;
  setp_pgr_port_down_dis_set(&csr, val, 1);
  OutWord(&(pgr_common_reg.port_down_dis), csr);
}

void TestUtil::pktgen_recirc_set(int pipe, bool valid, uint16_t port) {
  if ((pipe < 0) || (pipe >= kMaxPipes)) return;
  uint16_t m_port = ((port >> 2) >> pipe);
  assert((m_port == RmtDefs::kPktGen_P16) || (m_port == RmtDefs::kPktGen_P17));
  //auto& pgr_common_reg = kTofinoPtr->pipes[pipe].pmarb.pgr_reg.pgr_common;
  auto& pgr_common_reg = RegisterUtils::addr_pmarb(pipe)->pgr_reg.pgr_common;
  uint32_t val;
  uint32_t csr = 0;
  if (m_port == RmtDefs::kPktGen_P16) {
    //val = 0x1 << (port - ((m_port << 2) << pipe));
    val = 0xF;
    setp_pgr_port16_ctrl_channel_en(&csr, val);

    val = 0x4;
    setp_pgr_port16_ctrl_channel_mode(&csr, val);

    val = 0xD8;
    setp_pgr_port16_ctrl_channel_seq(&csr, val);

    val = valid?1:0;
    setp_pgr_port16_ctrl_recir_en(&csr, val);

    OutWord(&(pgr_common_reg.port16_ctrl), csr);
  } else {
    //val = 0x1 << (port - ((m_port << 2) << pipe));
    val = 0xF;
    setp_pgr_port17_ctrl1_channel_en(&csr, val);

    val = 0x4;
    setp_pgr_port17_ctrl1_channel_mode(&csr, val);

    val = 0xD8;
    setp_pgr_port17_ctrl1_channel_seq(&csr, val);

    val = valid?1:0;
    setp_pgr_port17_ctrl1_recir_en(&csr, val);
    OutWord(&(pgr_common_reg.port17_ctrl1), csr);
  }
}

void TestUtil::pktgen_mxbar_set(int pipe, bool valid, uint16_t port) {
  if ((pipe < 0) || (pipe >= kMaxPipes)) return;
  uint16_t m_port = ((port >> 2) >> pipe);
  assert(m_port == RmtDefs::kPktGen_P16);
  //auto& pgr_common_reg = kTofinoPtr->pipes[pipe].pmarb.pgr_reg.pgr_common;
  auto& pgr_common_reg = RegisterUtils::addr_pmarb(pipe)->pgr_reg.pgr_common;
  uint32_t val;
  uint32_t csr = 0;

  // Channel enable all channels
  val = 0xF;
  setp_pgr_port16_ctrl_channel_en(&csr, val);

  val = 0x4;
  setp_pgr_port16_ctrl_channel_mode(&csr, val);

  val = 0xD8;
  setp_pgr_port16_ctrl_channel_seq(&csr, val);

  val = (valid)?1:0;
  setp_pgr_port16_ctrl_mxbar_en(&csr, val);

  OutWord(&(pgr_common_reg.port16_ctrl), csr);
}


void TestUtil::pktgen_get_counters(int pipe, bool* en,
                                   pgr_app_counter* ctr_arr) {
  if ((pipe < 0) || (pipe >= kMaxPipes)) return;
  for (int i = 0; i < 8; i ++) {
    if (en[i]) {
      //auto& p_app = kTofinoPtr->pipes[pipe].pmarb.pgr_reg.pgr_app[i];
      auto& p_app = RegisterUtils::addr_pmarb(pipe)->pgr_reg.pgr_app[i];
      uint32_t p_trigger = InWord((void *)&(p_app.ctr48_trigger.ctr48_trigger_0_2));
      uint32_t p_batch = InWord((void *)(&(p_app.ctr48_batch.ctr48_batch_0_2)));
      uint32_t p_pkt = InWord((void*)(&(p_app.ctr48_packet.ctr48_packet_0_2)));
      //auto p_pkt = InWord(const_cast<void *>(reinterpret_cast<volatile void*>(&(p_app.ctr48_packet))));
      std::cout << "Address: " << &p_trigger << std::endl;
      ctr_arr[i].trig_cnt = getp_pgr_app_ctr48_trigger_ctr48(&p_trigger);
      ctr_arr[i].batch_cnt = getp_pgr_app_ctr48_batch_ctr48(&p_batch);
      ctr_arr[i].pkt_cnt = getp_pgr_app_ctr48_packet_ctr48(&p_pkt);
      //ctr_arr[i].pkt_cnt = getp_pgr_app_ctr48_packet_ctr48(reinterpret_cast<void *>(&p_pkt));
      std::cout << "Trig: " << ctr_arr[i].trig_cnt << "BATCH: " << ctr_arr[i].batch_cnt << "PKT: " << ctr_arr[i].pkt_cnt << std::endl;
    }
  }
}

void TestUtil::pktgen_pgen_set(int pipe,
                               uint16_t port,
                               app_config* a_config,
                               bool* en,
                               uint8_t a_sz,
                               bool recir_en) {

  if ((pipe < 0) || (pipe >= kMaxPipes)) return;
  uint16_t m_port = ((port >> 2) >> pipe);
  assert(m_port == RmtDefs::kPktGen_P17);

  uint32_t csr = 0;
  uint32_t val;
  bool pgen_en = false;

  // Enable packet generation on all channels
  //auto& p_ctl = kTofinoPtr->pipes[pipe].pmarb.pgr_reg.pgr_common;
  auto& p_ctl = RegisterUtils::addr_pmarb(pipe)->pgr_reg.pgr_common;

  csr = 0;
  val = 0x0;
  if (recir_en) {
    val = 0xF;
  }
  setp_pgr_port17_ctrl2_recir_channel_en(&csr, val);
  val = 0xF;
  setp_pgr_port17_ctrl2_pgen_channel_en(&csr, val);
  OutWord(&(p_ctl.port17_ctrl2), csr);


  uint32_t buf_sz;
  uint32_t remain_words;
  uint64_t i = 0;
  uint64_t data_0 = 0x00000000AAAAAAAA;
  uint64_t data_1 = data_0 << 32;
  uint64_t addr = (PktGenBufferMem::BUF_ST_ADDR + (pipe * PktGenBufferMem::BUF_ADDR_STEP)) >> 4;
  while (i < PktGenBufferMem::BUF_ENTRIES) {
    GLOBAL_MODEL->IndirectWrite(chip_,
                                addr + i,
                                data_0,
                                data_1);
    i++;
  }

  for (uint8_t i = 0; i < a_sz; i ++) {
    //auto& p = kTofinoPtr->pipes[pipe].pmarb.pgr_reg.pgr_app[i];
    auto& p = RegisterUtils::addr_pmarb(pipe)->pgr_reg.pgr_app[i];

    csr = 0;
    // Pick a random offset
    val = rand()%PktGenBufferMem::BUF_ENTRIES;
    setp_pgr_app_payload_ctrl_app_payload_addr(&csr, val);

    remain_words = PktGenBufferMem::BUF_SZ - val;
    buf_sz = rand()%remain_words;
    if (buf_sz == 0) buf_sz ++;
    setp_pgr_app_payload_ctrl_app_payload_size(&csr, buf_sz);
    OutWord(&(p.payload_ctrl), csr);

    csr = 0;
    val = (a_config[i].app_ingr_port_inc)?1:0;
    setp_pgr_app_ingr_port_ctrl_app_ingr_port_inc(&csr, val);
    val = (a_config[i].app_ingr_port);
    setp_pgr_app_ingr_port_ctrl_app_ingr_port(&csr, val);
    OutWord(&(p.ingr_port_ctrl), csr);

    csr = 0;
    val = (a_config[i].recir_match_value);
    setp_pgr_app_recir_match_value_recir_match_value(&csr, val);
    OutWord(&(p.recir_match_value), csr);

    csr = 0;
    val = (a_config[i].recir_match_mask);
    setp_pgr_app_recir_match_mask_recir_match_mask(&csr, val);
    OutWord(&(p.recir_match_mask), csr);

    csr = 0;
    val = (a_config[i].packet_num);
    setp_pgr_app_event_number_packet_num(&csr, val);
    val = (a_config[i].batch_num);
    setp_pgr_app_event_number_batch_num(&csr, val);
    OutWord(&(p.event_number), csr);

    csr = 0;
    val = (a_config[i].ibg_count);
    setp_pgr_app_event_ibg_ibg_count(&csr, val);
    OutWord(&(p.event_ibg), csr);

    csr = 0;
    val = (a_config[i].ipg_count);
    setp_pgr_app_event_ipg_ipg_count(&csr, val);
    OutWord(&(p.event_ipg), csr);

    csr = 0;
    val = (a_config[i].event_ipg_jitter_value);
    setp_pgr_app_event_jitter_value_value(&csr, val);
    OutWord(&(p.event_ipg_jitter_value), csr);

    csr = 0;
    val = (a_config[i].event_ipg_jitter_mask);
    setp_pgr_app_event_jitter_mask_mask(&csr, val);
    OutWord(&(p.event_ipg_jitter_mask), csr);

    csr = 0;
    val = (a_config[i].event_ibg_jitter_value);
    setp_pgr_app_event_jitter_value_value(&csr, val);
    OutWord(&(p.event_ibg_jitter_value), csr);

    csr = 0;
    val = (a_config[i].event_ibg_jitter_mask);
    setp_pgr_app_event_jitter_mask_mask(&csr, val);
    OutWord(&(p.event_ibg_jitter_mask), csr);

    csr = 0;
    val = (a_config[i].event_timer);
    setp_pgr_app_event_timer_timer_count(&csr, val);
    OutWord(&(p.event_timer), csr);

    csr = 0;
    val = (en[i])?1:0;
    if (en[i]) {
      pgen_en = en[i];
    }
    setp_pgr_app_ctrl_app_en(&csr, val);
    val = (a_config[i].app_type);
    setp_pgr_app_ctrl_app_type(&csr, val);
    OutWord(&(p.ctrl), csr);
  }


  csr = 0;
  val = 0x4;
  setp_pgr_port17_ctrl1_channel_mode(&csr, val);

  // Seq: 0, 2, 1, 3
  // 11 01 10 00
  val = 0x8D;
  setp_pgr_port17_ctrl1_channel_seq(&csr, val);

  val = 0x0;
  if (recir_en || pgen_en) {
    val = 0xF;
  }
  setp_pgr_port17_ctrl1_channel_en(&csr, val);

  val = 0x0;
  if (recir_en) {
    val = 0x1;
  }
  setp_pgr_port17_ctrl1_recir_en(&csr, val);

  val = 0x0;
  if (pgen_en) val = 0x1;
  setp_pgr_port17_ctrl1_pgen_en(&csr, val);

  OutWord(&(p_ctl.port17_ctrl1), csr);
}

void TestUtil::parser_update_extract16_type_cnt(MODEL_CHIP_NAMESPACE::Parser *p,
                                                int pi,
                                                uint8_t extract_type,
                                                uint8_t val) {
  (void)p;
  (void)pi;
  (void)extract_type;
  (void)val;
  // not required for tofinoXX
}

}
