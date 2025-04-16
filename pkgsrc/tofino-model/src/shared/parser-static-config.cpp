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

#include <array>
#include <cstdint>
#include <rmt-log.h>
#include <phv.h>
#include <parser-static-config.h>


namespace MODEL_CHIP_NAMESPACE {

  bool ParserStaticConfig::T        = true;
  bool ParserStaticConfig::F        = false;
  bool ParserStaticConfig::b_FF[]   = { false,false };
  bool ParserStaticConfig::b_FT[]   = { false, true };
  bool ParserStaticConfig::b_FFFF[] = { false,false,false,false };
  bool ParserStaticConfig::b_FFFT[] = { false,false,false,true };
  bool ParserStaticConfig::b_TTTT[] = { true,true,true,true };

  uint8_t  ParserStaticConfig::u8_00[] = { 0,0 };
  uint16_t ParserStaticConfig::NoX = k_phv::kBadPhv;

  uint16_t ParserStaticConfig::DA_HI_16 = Phv::make_word_p(4,0);
  uint16_t ParserStaticConfig::DA_LO_32 = Phv::make_word_p(0,0);
  uint16_t ParserStaticConfig::SA_HI_16 = Phv::make_word_p(4,1);
  uint16_t ParserStaticConfig::SA_LO_32 = Phv::make_word_p(0,1);
  uint16_t ParserStaticConfig::ETH_TYPE = Phv::make_word_p(4,2);
  uint16_t ParserStaticConfig::IP4_HL   = Phv::make_word_p(2,0);
  uint16_t ParserStaticConfig::IP4_TTL  = Phv::make_word_p(2,2);
  uint16_t ParserStaticConfig::IP4_PROTO= Phv::make_word_p(2,3);
  uint16_t ParserStaticConfig::IP4_ERR  = Phv::make_word_p(2,30);
  uint16_t ParserStaticConfig::IP4_LEN  = Phv::make_word_p(4,4);
  uint16_t ParserStaticConfig::IP4_ID   = Phv::make_word_p(4,3);
  uint16_t ParserStaticConfig::IP4_FRAG = Phv::make_word_p(4,5);
  uint16_t ParserStaticConfig::IP4_CKSM = Phv::make_word_p(4,6);
  uint16_t ParserStaticConfig::IP4_SRC  = Phv::make_word_p(0,2);
  uint16_t ParserStaticConfig::IP4_DST  = Phv::make_word_p(0,3);
  uint16_t ParserStaticConfig::P_SPORT  = Phv::make_word_p(4,7);
  uint16_t ParserStaticConfig::P_DPORT  = Phv::make_word_p(4,8);
  uint16_t ParserStaticConfig::TCP_CKSM = Phv::make_word_p(4,9);
  uint16_t ParserStaticConfig::TCP_RESID= Phv::make_word_p(4,31);
  // 8 bit PHV containers...
  uint16_t ParserStaticConfig::PHV8_0   = Phv::make_word_p(3,0);
  uint16_t ParserStaticConfig::PHV8_1   = Phv::make_word_p(3,1);
  uint16_t ParserStaticConfig::PHV8_2   = Phv::make_word_p(3,2);
  uint16_t ParserStaticConfig::PHV8_3   = Phv::make_word_p(3,3);
  uint16_t ParserStaticConfig::PHV8_4   = Phv::make_word_p(3,4);
  uint16_t ParserStaticConfig::PHV8_5   = Phv::make_word_p(3,5);
  uint16_t ParserStaticConfig::PHV8_6   = Phv::make_word_p(3,6);
  uint16_t ParserStaticConfig::PHV8_7   = Phv::make_word_p(3,7);
  uint16_t ParserStaticConfig::PHV8_8   = Phv::make_word_p(3,8);
  uint16_t ParserStaticConfig::PHV8_9   = Phv::make_word_p(3,9);
  uint16_t ParserStaticConfig::PHV8_10  = Phv::make_word_p(3,10);
  uint16_t ParserStaticConfig::PHV8_11  = Phv::make_word_p(3,11);

  void ParserStaticConfig::config_eth_header(Parser *p, int index, int state, int next_state) {
    // Raw packet header - strip off Eth header (*no* VLAN) - stash DA/SA/ETH_TYPE
    p->set_early_action(index,
                        0, F, F,   // Counter load src, IMM_VAL, LOAD
                        F, 14,     // DONE, shift_amount
                        14,15,12,  // field8_1 off, field8_0 off, field16 off
                        T,T,T,     // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, next_state);  // next_state_mask, next_state

    uint8_t  a0_u8_src[]  = {0,0x11,0x22,0x33};
    uint16_t a0_u8_dst[]  = {PHV8_0,PHV8_1,PHV8_2,PHV8_3};
    uint8_t  a0_u16_src[] = {0,6,12,0};
    uint16_t a0_u16_dst[] = {DA_HI_16,SA_HI_16,ETH_TYPE,NoX};
    uint8_t  a0_u32_src[] = {2,8,0,0};
    uint16_t a0_u32_dst[] = {DA_LO_32,SA_LO_32,NoX,NoX};

    p->set_action(index,
                  F, 0, b_FF, u8_00,                       // adj  RESET, adj_inc, checksum ENABLE, checksum_addr
                  b_TTTT, b_FFFF, a0_u8_src,  a0_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a0_u16_src, a0_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a0_u32_src, a0_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    p->set_tcam_match(index,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(state,    (uint16_t)0x9999, (uint8_t)0x99, (uint8_t)0x99),  // value
                      p->make_tcam_entry( 0xFF, (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask

    //BitVector<40>(std::array<uint8_t,5>({0x00,0x99,0x99,0x99,0x99}))
    //BitVector<40>(std::array<uint8_t,5>({0xFF,0x00,0x00,0x00,0x00}))
  }

  void ParserStaticConfig::config_ip_header(Parser *p, int index, int state, int next_state) {
    // Stripped off Ether header - see if we have Eth.Ethertype=IPv4 and IP.version=4 - stash IP fields
    p->set_early_action(index,
                        0, F, F,   // Counter load src, IMM_VAL, LOAD
                        F, 20,     // DONE, shift_amount (get rid IP)
                        11,10,8,   // field8_1 off, field8_0 off, field16 off
                        T,T,T,     // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, next_state);  // next_state_mask, next_state

    // NB. XXX: ActionRam[254] must have phv_8b_dst_3 active or we'll see an error
    uint16_t a1_u8_ext3   = (RmtObject::is_jbay_or_later()) ?NoX :IP4_ERR;
    uint8_t  a1_u8_src[]  = {0,8,9,0};
    uint16_t a1_u8_dst[]  = {IP4_HL,IP4_TTL,IP4_PROTO,a1_u8_ext3}; // XXX: extract3 ignored here (Tofino)
    uint8_t  a1_u16_src[] = {2,4,6,10};
    uint16_t a1_u16_dst[] = {IP4_LEN,IP4_ID,IP4_FRAG,IP4_CKSM};
    uint8_t  a1_u32_src[] = {12,16,0,0};
    uint16_t a1_u32_dst[] = {IP4_SRC,IP4_DST,NoX,NoX};

    p->set_action(index,
                  F, 0, b_FF, u8_00,                       // adj  RESET, adj_inc, checksum ENABLE, checksum_addr
                  b_FFFF, b_FFFF, a1_u8_src,  a1_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a1_u16_src, a1_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a1_u32_src, a1_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    p->set_action_pri(index, 0x0, 0, 0x1, 2);   // Immediate val - pri=2
    p->set_tcam_match(index,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(state, (uint16_t)0x0800, (uint8_t)0x40, (uint8_t)0x99),  // value
                      p->make_tcam_entry( 0xFF, (uint16_t)0xFFFF, (uint8_t)0xF0, (uint8_t)   0)); // mask

    //BitVector<40>(std::array<uint8_t,5>({0x01,0x08,0x00,0x40,0x99}))
    //BitVector<40>(std::array<uint8_t,5>({0xFF,0xFF,0xFF,0xF0,0x00}))
  }

  void ParserStaticConfig::config_tcp_header(Parser *p, int index, int state, int next_state, bool done) {
    // Stripped off IP header - see if we have IP_PROTO==TCP - stash TCP_SPORT/TCP_DPORT fields
    p->set_early_action(index,
                        0, F, F,    // Counter load src, IMM_VAL, LOAD
                        done, 20,   // ****DONE****, shift_amount (get rid TCP)
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        T,T,T,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, next_state); // next_state_mask, next_state

    uint8_t  a2_u8_src[]  = {0,0,0,0};
    uint16_t a2_u8_dst[]  = {NoX,NoX,NoX,NoX};
    uint8_t  a2_u16_src[] = {0,2,0,0};
    uint16_t a2_u16_dst[] = {P_SPORT,P_DPORT,NoX,NoX};
    uint8_t  a2_u32_src[] = {0,0,0,0};
    uint16_t a2_u32_dst[] = {NoX,NoX,NoX,NoX};

    p->set_action(index,
                  F, 0, b_FF, u8_00,                       // adj  RESET, adj_inc, checksum ENABLE, checksum_addr
                  b_FFFF, b_FFFF, a2_u8_src,  a2_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a2_u16_src, a2_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a2_u32_src, a2_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    p->set_action_pri(index, 0x1, 2, 0, 0x7);   // Packet val - use offset 2 (DPORT), shift 0, use lower 3 bits (TCP PRI in [0..7])
    p->set_tcam_match(index,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(state, (uint16_t)0x9906, (uint8_t)0x99, (uint8_t)0x99),  // value
                      p->make_tcam_entry( 0xFF, (uint16_t)0x00FF, (uint8_t)   0, (uint8_t)   0)); // mask
  }


  void ParserStaticConfig::config_udp_header(Parser *p, int index, int state, int next_state) {
    // Stripped off IP header - see if we have IP_PROTO==UDP - stash UDP_SPORT/UDP_DPORT fields
    p->set_early_action(index,
                        0, F, F,    // Counter load src, IMM_VAL, LOAD
                        T, 8,      // ****DONE****, shift_amount (get rid UDP)
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        T,T,T,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, next_state); // next_state_mask, next_state

    uint8_t  a3_u8_src[]  = {0,0,0,0};
    uint16_t a3_u8_dst[]  = {NoX,NoX,NoX,NoX};
    uint8_t  a3_u16_src[] = {0,2,0,0};
    uint16_t a3_u16_dst[] = {P_SPORT,P_DPORT,NoX,NoX};
    uint8_t  a3_u32_src[] = {0,0,0,0};
    uint16_t a3_u32_dst[] = {NoX,NoX,NoX,NoX};

    p->set_action(index,
                  F, 0, b_FF, u8_00,                       // adj  RESET, adj_inc, checksum ENABLE, checksum_addr
                  b_FFFF, b_FFFF, a3_u8_src,  a3_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a3_u16_src, a3_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a3_u32_src, a3_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    p->set_action_pri(index, 0x1, 2, 0, 0x3);   // Packet val - use offset 2 (DPORT), shift 0, use lower 2 bits (UDP PRI in [0..3])
    p->set_tcam_match(index,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(state, (uint16_t)0x9911, (uint8_t)0x99, (uint8_t)0x99),  // value
                      p->make_tcam_entry( 0xFF, (uint16_t)0x00FF, (uint8_t)   0, (uint8_t)   0)); // mask
  }

  void ParserStaticConfig::config_catch_all(Parser *p, int index, int state, int next_state) {
    // Catch-all entry - will match anything not matched above
    p->set_early_action(index,
                        0, F, F,    // Counter load src, IMM_VAL, LOAD
                        T, 0,      // ****DONE****, shift_amount
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        T,T,T,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, next_state); // next_state_mask, next_state

    uint8_t  a4_u8_src[]  = {0,0,0,0};
    uint16_t a4_u8_dst[]  = {NoX,NoX,NoX,NoX};
    uint8_t  a4_u16_src[] = {0,0,0,0};
    uint16_t a4_u16_dst[] = {NoX,NoX,NoX,NoX};
    uint8_t  a4_u32_src[] = {0,0,0,0};
    uint16_t a4_u32_dst[] = {NoX,NoX,NoX,NoX};

    p->set_action(index,
                  F, 0, b_FF, u8_00,                       // adj  RESET, adj_inc, checksum ENABLE, checksum_addr
                  b_FFFF, b_FFFF, a4_u8_src,  a4_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a4_u16_src, a4_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a4_u32_src, a4_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    p->set_tcam_match(index,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(state, (uint16_t)0x0000, (uint8_t)0x00, (uint8_t)0x00),  // value
                      p->make_tcam_entry( 0x00, (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask
  }

  void ParserStaticConfig::config_payload_n(Parser *p, int index, int state, int next_state,
                                            bool done, std::array<uint16_t, 4> dst_phv,
                                            int shift) {
    // set up a parser state to: shift in <shift> payload bytes; extract a sample of
    // payload to phv; extract an imm val to phv; dst_phv can be used to
    // specify to which phv words the sample of payload bytes and imm value are
    // extracted; dst_phv defaults to all NoX.
    p->set_early_action(index,
                      0, F, F,     // Counter load src, LD_SRC, LOAD
                      done, shift, // ****DONE****, shift_amount (get rid <shift> bytes payload)
                      0,0,0,       // field8_1 off, field8_0 off, field16 off
                      T,T,T,       // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                      0xFF, next_state); // next_state_mask, next_state
    // extract a sample of payload bytes...
    uint8_t  a4_u8_src[]  = {0,8,16,0xba};  // final entry is an immediate value
    uint16_t a4_u8_dst[]  = {dst_phv[0],dst_phv[1],dst_phv[2],dst_phv[3]};
    uint8_t  a4_u16_src[] = {0,0,0,0};
    uint16_t a4_u16_dst[] = {NoX,NoX,NoX,NoX};
    uint8_t  a4_u32_src[] = {0,0,0,0};
    uint16_t a4_u32_dst[] = {NoX,NoX,NoX,NoX};

    // Enable checksum engine 1 again - use checksum addr=4
    p->set_action(index,
                  F, 0, b_FF, u8_00,                       // adj  RESET, adj_inc, checksum **ENABLE**, checksum_addr=4
                  b_FFFT, b_FFFF, a4_u8_src,  a4_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a4_u16_src, a4_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a4_u32_src, a4_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    p->set_tcam_match(index,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(state, (uint16_t)0x0000, (uint8_t)0x00, (uint8_t)0x00),  // value
                      p->make_tcam_entry( 0xFF, (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask
  }

  void ParserStaticConfig::parser_config_basic_eth_ip_tcp(Parser *p) {

    // Setup priority map that does nothing
    p->set_identity_priority_map();

    config_eth_header(p, 255, 0, 1);
    p->set_action_pri(255, 0x0, 0, 0x1, 1);   // Immediate val - pri=1
    config_ip_header(p, 254, 1, 2);
    config_tcp_header(p, 253, 2, 255, true);
    config_udp_header(p, 252, 2, 255);
    config_catch_all(p, 251, 0, 255);
    p->set_action_pri(251, 0x0, 0, 0x1, 1);   // Immediate val - pri=1 - catch all has very lo pri

    p->set_channel(0, true, 0);
    // If congested, any packet with pri 0 or 1 wil be dropped
    // This could be packets that only match the catch-all entry (251) or that are just IP (254)
    // Or UDP packets with DPORT & 0x3 = 0 or 1
    // Or TCP packets with DPORT & 0x7 = 0 or 1
    p->set_pri_thresh(0, 2);
    // WIP requires hdr_len_inc to be set; ignored for jbay...
    std::list<int> states {255, 254, 253, 252, 251, 250};
    for (int state : states) {
      p->set_hdr_len_inc(state, true);
    }
  }


  void ParserStaticConfig::deparser_config_basic_eth_ip_tcp(Deparser *dp) {

    //printf("deparser_config_basic_eth_ip_tcp %d\n",dp->pipe_index());

    // POV_POSITION
    //OD    for (int i=0; i<32; i++)
    //OD      dp->get_dprsr_reg_rspec().pov_pos().pov_position(i,PHV8_1);

    // CSUM_CFG
    //OD    for (int i=0; i<Deparser::kNumCheckSumEngines; i++) {
    //OD      for (int csum_entry_idx=0; csum_entry_idx<Deparser::kNumCheckSumCfgEntries; csum_entry_idx++) {
    //OD        dp->get_dprsr_reg_rspec().csum_cfg(i).csum_cfg_csum_cfg_entry(csum_entry_idx).plus_minus_sel(0);
    //OD        dp->get_dprsr_reg_rspec().csum_cfg(i).csum_cfg_csum_cfg_entry(csum_entry_idx).hi_lo_sel(0);
    //OD        dp->get_dprsr_reg_rspec().csum_cfg(i).csum_cfg_csum_cfg_entry(csum_entry_idx).sel_0_1x_2x(0);
    //OD      }
    //OD    }

    /*
    // FIELD_DICTIONARY_ENTRY
    for (int i=0; i<Deparser::kNumFieldDictionaryEntries; i++) {
      dp->get_dprsr_mem_rspec().field_dictionary(i).valid(0);
    }

    dp->get_dprsr_mem_rspec().field_dictionary(0).valid(1);
    dp->get_dprsr_mem_rspec().field_dictionary(0).phv_word_num(0,DA_HI_16);
    dp->get_dprsr_mem_rspec().field_dictionary(0).phv_word_num(1,DA_HI_16);
    dp->get_dprsr_mem_rspec().field_dictionary(0).phv_word_num(2,DA_LO_32);
    dp->get_dprsr_mem_rspec().field_dictionary(0).phv_word_num(3,DA_LO_32);
    dp->get_dprsr_mem_rspec().field_dictionary(0).pov_sel(0);
    dp->get_dprsr_mem_rspec().field_dictionary(0).num_bytes(3);

    dp->get_dprsr_mem_rspec().field_dictionary(1).valid(1);
    dp->get_dprsr_mem_rspec().field_dictionary(1).phv_word_num(0,DA_LO_32);
    dp->get_dprsr_mem_rspec().field_dictionary(1).phv_word_num(1,DA_LO_32);
    dp->get_dprsr_mem_rspec().field_dictionary(1).phv_word_num(2,SA_HI_16);
    dp->get_dprsr_mem_rspec().field_dictionary(1).phv_word_num(3,SA_HI_16);
    dp->get_dprsr_mem_rspec().field_dictionary(1).pov_sel(0);
    dp->get_dprsr_mem_rspec().field_dictionary(1).num_bytes(3);

    dp->get_dprsr_mem_rspec().field_dictionary(7).valid(1);
    dp->get_dprsr_mem_rspec().field_dictionary(7).phv_word_num(0,SA_LO_32);
    dp->get_dprsr_mem_rspec().field_dictionary(7).phv_word_num(1,SA_LO_32);
    dp->get_dprsr_mem_rspec().field_dictionary(7).phv_word_num(2,SA_LO_32);
    dp->get_dprsr_mem_rspec().field_dictionary(7).phv_word_num(3,SA_LO_32);
    dp->get_dprsr_mem_rspec().field_dictionary(7).pov_sel(0);
    dp->get_dprsr_mem_rspec().field_dictionary(7).num_bytes(3);

    dp->get_dprsr_mem_rspec().field_dictionary(8).valid(1);
    dp->get_dprsr_mem_rspec().field_dictionary(8).phv_word_num(0,ETH_TYPE);
    dp->get_dprsr_mem_rspec().field_dictionary(8).phv_word_num(1,ETH_TYPE);
    dp->get_dprsr_mem_rspec().field_dictionary(8).phv_word_num(2,IP4_HL);
    dp->get_dprsr_mem_rspec().field_dictionary(8).phv_word_num(3,IP4_TTL);
    dp->get_dprsr_mem_rspec().field_dictionary(8).pov_sel(0);
    dp->get_dprsr_mem_rspec().field_dictionary(8).num_bytes(3);

    dp->get_dprsr_mem_rspec().field_dictionary(200).valid(1);
    dp->get_dprsr_mem_rspec().field_dictionary(200).phv_word_num(0,IP4_PROTO);
    dp->get_dprsr_mem_rspec().field_dictionary(200).phv_word_num(1,IP4_LEN);
    dp->get_dprsr_mem_rspec().field_dictionary(200).phv_word_num(2,IP4_LEN);
    dp->get_dprsr_mem_rspec().field_dictionary(200).phv_word_num(3,IP4_FRAG);
    dp->get_dprsr_mem_rspec().field_dictionary(200).pov_sel(0);
    dp->get_dprsr_mem_rspec().field_dictionary(200).num_bytes(3);

    dp->get_dprsr_mem_rspec().field_dictionary(210).valid(1);
    dp->get_dprsr_mem_rspec().field_dictionary(210).phv_word_num(0,IP4_FRAG);
    dp->get_dprsr_mem_rspec().field_dictionary(210).phv_word_num(1,IP4_CKSM);
    dp->get_dprsr_mem_rspec().field_dictionary(210).phv_word_num(2,IP4_CKSM);
    dp->get_dprsr_mem_rspec().field_dictionary(210).phv_word_num(3,IP4_SRC);
    dp->get_dprsr_mem_rspec().field_dictionary(210).pov_sel(0);
    dp->get_dprsr_mem_rspec().field_dictionary(210).num_bytes(3);

    dp->get_dprsr_mem_rspec().field_dictionary(223).valid(1);
    dp->get_dprsr_mem_rspec().field_dictionary(223).phv_word_num(0,IP4_SRC);
    dp->get_dprsr_mem_rspec().field_dictionary(223).phv_word_num(1,IP4_SRC);
    dp->get_dprsr_mem_rspec().field_dictionary(223).phv_word_num(2,IP4_SRC);
    dp->get_dprsr_mem_rspec().field_dictionary(223).phv_word_num(3,IP4_DST);
    dp->get_dprsr_mem_rspec().field_dictionary(223).pov_sel(0);
    dp->get_dprsr_mem_rspec().field_dictionary(223).num_bytes(3);

    dp->get_dprsr_mem_rspec().field_dictionary(233).valid(1);
    dp->get_dprsr_mem_rspec().field_dictionary(233).phv_word_num(0,IP4_DST);
    dp->get_dprsr_mem_rspec().field_dictionary(233).phv_word_num(1,IP4_DST);
    dp->get_dprsr_mem_rspec().field_dictionary(233).phv_word_num(2,IP4_DST);
    dp->get_dprsr_mem_rspec().field_dictionary(233).phv_word_num(3,P_SPORT);
    dp->get_dprsr_mem_rspec().field_dictionary(233).pov_sel(0);
    dp->get_dprsr_mem_rspec().field_dictionary(233).num_bytes(3);

    dp->get_dprsr_mem_rspec().field_dictionary(243).valid(1);
    dp->get_dprsr_mem_rspec().field_dictionary(243).phv_word_num(0,P_SPORT);
    dp->get_dprsr_mem_rspec().field_dictionary(243).phv_word_num(1,P_DPORT);
    dp->get_dprsr_mem_rspec().field_dictionary(243).phv_word_num(2,P_DPORT);
    dp->get_dprsr_mem_rspec().field_dictionary(243).phv_word_num(3,PHV8_0);
    dp->get_dprsr_mem_rspec().field_dictionary(243).pov_sel(0);
    dp->get_dprsr_mem_rspec().field_dictionary(243).num_bytes(3);

    dp->get_dprsr_mem_rspec().field_dictionary(253).valid(1);
    dp->get_dprsr_mem_rspec().field_dictionary(253).phv_word_num(0,PHV8_1);
    dp->get_dprsr_mem_rspec().field_dictionary(253).phv_word_num(1,PHV8_2);
    dp->get_dprsr_mem_rspec().field_dictionary(253).phv_word_num(2,PHV8_3);
    dp->get_dprsr_mem_rspec().field_dictionary(253).pov_sel(0);
    dp->get_dprsr_mem_rspec().field_dictionary(253).num_bytes(2);
    */
  }



  void parser_config_basic_eth_ip_tcp(Parser *parser) {
    ParserStaticConfig::parser_config_basic_eth_ip_tcp(parser);
  }

  void deparser_config_basic_eth_ip_tcp(Deparser *deparser) {
    ParserStaticConfig::deparser_config_basic_eth_ip_tcp(deparser);
  }
}
