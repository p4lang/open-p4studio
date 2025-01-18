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

#include "test_parse.h"
#include <packet.h>
#include <parser-static-config.h>

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

void BFN_TEST_NAME(ParserTestBase)::verify_chnl_parser_send_pkt_err(
    IpbCounters *ipb_counters,
    uint expected) {
#ifdef MODEL_CHIP_CB_OR_LATER
  EXPECT_EQ(expected, ipb_counters->chnl_parser_send_err_pkt_.err_pkt());
#else
  EXPECT_EQ(expected, ipb_counters->chnl_parser_send_pkt_.err_pkt());
#endif
}

void BFN_TEST_NAME(ParserTestBase)::set_max_chnl_parser_send_pkt(IpbCounters *ipb_counters) {
  ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt(UINT64_C(0xfffffffff));
}

void BFN_TEST_NAME(ParserTestBase)::set_max_chnl_parser_send_pkt_err(IpbCounters *ipb_counters) {
#ifdef MODEL_CHIP_CB_OR_LATER
  ipb_counters->chnl_parser_send_err_pkt_.err_pkt(UINT64_C(0xfffffffff));
#else
  ipb_counters->chnl_parser_send_pkt_.err_pkt(UINT64_C(0xfffffffff));
#endif
}

void BFN_TEST_NAME(ParserTestBase)::verify_epb_counters(
    Epb *epb,
    int epb_chan,
    uint64_t expected_sent) {
  // jbay specific counters
  auto *counters = epb->get_epb_counters(epb_chan);
  EXPECT_EQ(expected_sent,
            counters->chnl_parser_send_pkt_.epb_chnl_parser_send_pkt());
  // egress packets are never dropped
#ifdef MODEL_CHIP_CB_OR_LATER
  EXPECT_EQ(0u, counters->chnl_parser_send_err_pkt_.err_pkt());
#else
  EXPECT_EQ(0u, counters->chnl_parser_send_pkt_.err_pkt());
#endif
}

uint64_t BFN_TEST_NAME(ParserTestBase)::set_max_epb_chnl_parser_send_pkt(
    Epb *epb,
    int epb_chan) {
  // 36 bit counter or jbay
  uint64_t max_val = 0xfffffffff;
  auto *counters = epb->get_epb_counters(epb_chan);
  counters->chnl_parser_send_pkt_.epb_chnl_parser_send_pkt(max_val);
  return max_val;
}

TEST_F(MCN_CONCAT(BFN_TEST_NAME(ParseTest), UsingConfigBase), ExtractConstant) {
  // verify which extractors can extract from constant source
  using CONF = ParserStaticConfig;
  uint8_t  a0_u8_src[] = {0x11,0x22,0x33,0x44};
  uint16_t a0_u8_dst[] = {CONF::PHV8_0,
                          CONF::PHV8_1,
                          CONF::PHV8_2,
                          CONF::PHV8_3};
  uint8_t u8_0000[] = {0, 0, 0, 0};

  // set first state up to have first 8 extractors disabled, then 4
  // extractors from a constant, then 4 16b extractors, then 2 32b
  // extractors; total number of extractors is capped at 20 by set_action so
  // final 2 extractors must be to CONF::NoX
  int n_disabled_extractors = 8;
  uint16_t a0_u16_dst[] = {0, 0, 0, 0};
  uint16_t a0_u32_dst[] = {0, 0, CONF::NoX, CONF::NoX};
  parser_->set_action(
      255,
      false, 0, CONF::b_FF, CONF::u8_00,                 // adj  RESET, adj_inc, checksum ENABLE, checksum_addr
      CONF::b_TTTT, CONF::b_FFFF, a0_u8_src,  a0_u8_dst, // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
      CONF::b_FFFF, CONF::b_FFFF, u8_0000, a0_u16_dst,   // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
      CONF::b_FFFF, CONF::b_FFFF, u8_0000, a0_u32_dst,   // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
      model_core::ChipType::kTofino,                     // dst_phv_format_
      n_disabled_extractors);

  Phv *phv1 = parser_->parse(pkt1_, 0);
  ASSERT_TRUE(phv1 != nullptr);
  EXPECT_EQ(0x11u, phv1->get_p(CONF::PHV8_0));
  EXPECT_EQ(0x22u, phv1->get_p(CONF::PHV8_1));
  if (RmtObject::is_jbayXX()) {
    // for jbay extractor indexes > 9 do not support constant extraction
    EXPECT_EQ(0x0u, phv1->get_p(CONF::PHV8_2));
    EXPECT_EQ(0x0u, phv1->get_p(CONF::PHV8_3));
  } else if (RmtObject::is_chip1_or_later()) {
    // for WIP extractor all indexes support constant extraction
    EXPECT_EQ(0x33u, phv1->get_p(CONF::PHV8_2));
    EXPECT_EQ(0x44u, phv1->get_p(CONF::PHV8_3));
  } else {
    FAIL() << "Unexpected chip type " << RmtObject::chip_type();
  }

  // set first state up to have first 16 extractors disabled, then 4
  // extractors from a constant; total number of extractors is capped at 20
  // by set_action so remaining extractors must be to CONF::NoX
  n_disabled_extractors = 16;
  uint16_t no_u16_dst[] = {CONF::NoX, CONF::NoX, CONF::NoX, CONF::NoX};
  parser_->set_action(
      255,
      false, 0, CONF::b_FF, CONF::u8_00,                 // adj  RESET, adj_inc, checksum ENABLE, checksum_addr
      CONF::b_TTTT, CONF::b_FFFF, a0_u8_src,  a0_u8_dst, // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
      CONF::b_FFFF, CONF::b_FFFF, u8_0000, no_u16_dst,   // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
      CONF::b_FFFF, CONF::b_FFFF, u8_0000, no_u16_dst,   // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
      model_core::ChipType::kTofino,                     // dst_phv_format_
      n_disabled_extractors);

  Phv *phv2 = parser_->parse(pkt2_, 0);
  ASSERT_TRUE(phv2 != nullptr);
  if (RmtObject::is_jbayXX()) {
    // for jbay extractor indexes > 9 do not support constant extraction
    EXPECT_EQ(0x0u, phv2->get_p(CONF::PHV8_0));
    EXPECT_EQ(0x0u, phv2->get_p(CONF::PHV8_1));
    EXPECT_EQ(0x0u, phv2->get_p(CONF::PHV8_2));
    EXPECT_EQ(0x0u, phv2->get_p(CONF::PHV8_3));
  } else if (RmtObject::is_chip1_or_later()) {
    // for WIP extractor all indexes support constant extraction
    EXPECT_EQ(0x11u, phv2->get_p(CONF::PHV8_0));
    EXPECT_EQ(0x22u, phv2->get_p(CONF::PHV8_1));
    EXPECT_EQ(0x33u, phv2->get_p(CONF::PHV8_2));
    EXPECT_EQ(0x44u, phv2->get_p(CONF::PHV8_3));
  } else {
    FAIL() << "Unexpected chip type " << RmtObject::chip_type();
  }
}

TEST_F(MCN_CONCAT(BFN_TEST_NAME(ParseTest), UsingConfigBase), HdrLenInc) {
  bool is_jbay = RmtObject::is_jbayXX();
  // verify hdr length inc amounts and stop/hold regs are applied correctly
  auto do_test = [&](const char *pktstr)->uint16_t {
    Packet *pkt = om_->pkt_create(pktstr);
    // couldn't compile ASSERT_TRUE inside lambda so use unexpected value
    if (nullptr == pkt) return -999;
    Phv *phv = parser_->parse(pkt, 0);
    if (nullptr == phv) return -999;
    uint16_t res = pkt->orig_hdr_len();
    om_->pkt_delete(pkt);
    return res;
  };

  // for WIP we must enable hdr_len_inc for all relevant states
  std::list<int> states = {255, 254, 253, 252};
  for (int i : states) {
    parser_->set_hdr_len_inc(i, true);
  }
  // jbay/WIP subtracts 32 bytes hdr_len_adj amt from header len by default
  uint actual_len = do_test(udp_pktstr_);
  EXPECT_EQ(10u, actual_len);  // shift 14 + shift 20 + shift 8 - hdr_len_adj 32
  actual_len = do_test(tcp_pktstr_);
  EXPECT_EQ(22u, actual_len);  // shift 14 + shift 20 + shift 20 - hdr_len_adj 32

  // set hdr_len_adj to zero so that remaining tests are consistent with no IPB
  // metatdata being prepended
  parser_->set_hdr_len_adj(0u);

  // default increments
  actual_len = do_test(udp_pktstr_);
  EXPECT_EQ(42u, actual_len);  // shift 14 + shift 20 + shift 8
  actual_len = do_test(tcp_pktstr_);
  EXPECT_EQ(54u, actual_len);  // shift 14 + shift 20 + shift 20

  // jbay: final amt specified for udp state; WIP doesn't support final amt
  parser_->set_hdr_len_inc_final_amt(252, 3);  // jbay
  parser_->set_hdr_len_inc_stop(252, true);
  actual_len = do_test(udp_pktstr_);
  if (is_jbay) EXPECT_EQ(37u, actual_len);  // shift 14 + shift 20 + amt 3 + hold
  else EXPECT_EQ(42u, actual_len);  // shift 14 + shift 20 + shift 8 + hold
  actual_len = do_test(tcp_pktstr_);
  EXPECT_EQ(54u, actual_len);  // shift 14 + shift 20 + shift 20
  parser_->set_hdr_len_inc_final_amt(252, 0);
  parser_->set_hdr_len_inc_stop(252, false);

  // jbay: final amt specified for tcp state; WIP doesn't support final amt
  parser_->set_hdr_len_inc_final_amt(253, 24);
  parser_->set_hdr_len_inc_stop(253, true);
  actual_len = do_test(udp_pktstr_);
  EXPECT_EQ(42u, actual_len);  // shift 14 + shift 20 + shift 8
  actual_len = do_test(tcp_pktstr_);
  if (is_jbay) EXPECT_EQ(58u, actual_len);  // shift 14 + shift 20 + amt 24 + hold
  else EXPECT_EQ(54u, actual_len);  // shift 14 + shift 20 + shift 20 + hold
  parser_->set_hdr_len_inc_final_amt(253, 0);
  parser_->set_hdr_len_inc_stop(253, false);

  // stop hdr inc in ip state
  parser_->set_hdr_len_inc_stop(254, true);
  // NB: second 'stop' op will be ignored and no more inc applied
  parser_->set_hdr_len_inc_stop(253, true);
  parser_->set_hdr_len_inc_final_amt(253, 24);
  actual_len = do_test(udp_pktstr_);
  if (is_jbay) EXPECT_EQ(14u, actual_len);  // shift 14 + amt 0 + hold
  else EXPECT_EQ(34u, actual_len);  // shift 14 + shift 20 + hold
  actual_len = do_test(tcp_pktstr_);
  if (is_jbay) EXPECT_EQ(14u, actual_len);  // shift 14 + amt 0 + hold
  else EXPECT_EQ(34u, actual_len);  // shift 14 + shift 20 + hold

  // ... set an amount for final inc state (jbay only)
  parser_->set_hdr_len_inc_final_amt(254, 6);
  actual_len = do_test(udp_pktstr_);
  if (is_jbay) EXPECT_EQ(20u, actual_len);  // shift 14 + amt 6 + hold
  else EXPECT_EQ(34u, actual_len);  // shift 14 + shift 20 + hold
  actual_len = do_test(tcp_pktstr_);
  if (is_jbay) EXPECT_EQ(20u, actual_len);  // shift 14 + amt 6 + hold
  else EXPECT_EQ(34u, actual_len);  // shift 14 + shift 20 + hold

  parser_->set_hdr_len_inc_stop(254, false);
  parser_->set_hdr_len_inc_final_amt(254, 0);
  parser_->set_hdr_len_inc_stop(253, false);
  parser_->set_hdr_len_inc_final_amt(253, 0);

  // default increments - WIP hdr_len_inc not set in all states
  if (RmtObject::is_chip1_or_later()) {
    parser_->set_hdr_len_inc(252, false);
    actual_len = do_test(udp_pktstr_);
    EXPECT_EQ(34u, actual_len);  // shift 14 + shift 20 + no inc
    parser_->set_hdr_len_inc(253, false);
    actual_len = do_test(tcp_pktstr_);
    EXPECT_EQ(34u, actual_len);  // shift 14 + shift 20 + no inc
    parser_->set_hdr_len_inc(254, false);
    actual_len = do_test(udp_pktstr_);
    EXPECT_EQ(14u, actual_len);  // shift 14 + no inc
    actual_len = do_test(tcp_pktstr_);
    EXPECT_EQ(14u, actual_len);  // shift 14 + no inc
    parser_->set_hdr_len_inc(255, false);
    actual_len = do_test(udp_pktstr_);
    EXPECT_EQ(0u, actual_len);  // no inc
    actual_len = do_test(tcp_pktstr_);
    EXPECT_EQ(0u, actual_len);  // no inc

    // WIP: hdr_len_inc set in final state only is sufficient to get all shifts
    parser_->set_hdr_len_inc(252, true);
    actual_len = do_test(udp_pktstr_);
    EXPECT_EQ(42u, actual_len);  // shift 14 + shift 20 + shift 8
    parser_->set_hdr_len_inc(252, false);
    parser_->set_hdr_len_inc(253, true);
    actual_len = do_test(tcp_pktstr_);
    EXPECT_EQ(54u, actual_len);  // shift 14 + shift 20 + shift 20
  }
}

TEST_F(BFN_TEST_NAME(ParserTestBase), TmStatus) {
  set_pipe_index(3);
  // verify TM status is set up in ParseMerge and written to ingress PHV
  // om_->update_log_flags(ALL,ALL,TYP_PARSER,ALL,ALL,ALL,ALL);
  // om_->update_log_type_levels(pipes_mask(),ALL,RMT_LOG_TYPE_P4,ALL,ALL);

  // Get an *egress* parser
  Parser *parser_e = om_->parser_get(pipe_index(), 0)->egress();
  parser_e->reset();
  parser_config_basic_eth_ip_tcp(parser_e);
  parser_e->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);

  //                     <   DA           SA      ET I4 C LN  ID  OF  T P CK   SRC     DST     SP  DP>
  //                     <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST   ><SP><DP>
  const char *pkt_str = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A4455661188110100000000000000000000000000000000";

  // send egress pkts to load various tm status params into ParseMerge fifo
  std::list<std::tuple<int, int, int>> status_params {
  //   port, q_dpth, q_id
      std::make_tuple(63,   0xab,   0x3d),
      std::make_tuple(9,    0x58,   0x44),
      std::make_tuple(17,   0x5abcd, 0x7f)};
  std::list<std::tuple<uint32_t, uint32_t>> expected;
  for(auto it : status_params) {
    int port, q_depth, q_id;
    std::tie(port, q_depth, q_id) = it;
    // Parse egress packets to load tm status in parse merge
    Packet *egress_pkt = om_->pkt_create(pkt_str);
    // set up metadata that should be used to compose tm status in parse merge
    egress_pkt->set_port(om_->port_lookup(port));
    egress_pkt->qing2e_metadata()->set_ing_q_depth(q_depth);
    egress_pkt->i2qing_metadata()->set_qid(q_id);
    // parse egress packet to get tm status into ParseMerge
    Phv *egress_phv = parser_e->parse(egress_pkt, 0);
    ASSERT_TRUE(egress_phv != NULL);
    EXPECT_EQ(0x1188u, egress_phv->get_p(ParserStaticConfig::P_SPORT)); // sanity check
    om_->pkt_delete(egress_pkt);
    // stash expected tm_status for current params...
    uint32_t expected_tm_status = 0, expected_tm_status_lsb = 0;
    if (RmtObject::is_jbayXX()) {
      expected_tm_status |= (pipe_index() << 29);  // pipe id
      expected_tm_status |= (Port::get_group_num(port) << 25);  // mac id
      expected_tm_status |= (q_id << 18);  // queue id
      expected_tm_status |= (q_depth >> 1); // 18 msb's of 19 bit queue depth
    } else {
      expected_tm_status |= (pipe_index() << 27);  // pipe id
      expected_tm_status |= (Port::get_group_num(port) << 23);  // mac id
      expected_tm_status |= (q_id << 16);  // queue id
      expected_tm_status |= (q_depth >> 3); // 16 msb's of 19 bit queue depth
      expected_tm_status_lsb |= (q_depth & 0x7);  // 3 lsb's of queue depth
    }
    // for each egress pkt we'll check 2 ingress pkts so add 2 expected
    // tm_status to the list, flipping ping pong bit
    expected.push_back(std::make_tuple(expected_tm_status, expected_tm_status_lsb));
    expected_tm_status |= 0x80000000;  // ping pong bit
    expected.push_back(std::make_tuple(expected_tm_status, expected_tm_status_lsb));
  }

  // Get an *ingress* parser for same pipe
  Parser *parser_i = om_->parser_get(pipe_index(), 0)->ingress();
  parser_i->reset();
  parser_config_basic_eth_ip_tcp(parser_i);
  parser_i->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);

  // send ingress pkts and check tm status written to expected PHV
  int rpt = 0;
  for (auto it : expected) {
    SCOPED_TRACE(rpt);
    Packet *ingress_pkt = om_->pkt_create(pkt_str);
    Phv *ingress_phv;
    if (2 == rpt) {
      // set tm status dest phv using odd off16, LSB should be ignored
      parser_i->phv_set_tm_status_phv(pipe_index(), 253);
    } else {
      // set tm status dest phv using even off16
      parser_i->phv_set_tm_status_phv(pipe_index(), 252);
    }
    if ((!RmtObject::is_jbayXX()) && (4 == rpt)) {
      // enable phv_sec for LSBs of tm status, write to lower half of word
      parser_i->phv_set_tm_status_phv_sec(249, 0);
    } else if (!RmtObject::is_jbayXX() && (5 == rpt)) {
      // enable phv_sec for LSBs of tm status, write to upper half of word
      parser_i->phv_set_tm_status_phv_sec(249, 1);
    }
    if (3 == rpt) {
      // check a ghost phv
      ingress_phv = parser_i->get_ghost_phv();
    } else {
      ingress_phv = parser_i->parse(ingress_pkt, 0);
    }
    ASSERT_TRUE(ingress_phv != NULL);
    uint32_t expected_tm_status = 0, expected_tm_status_lsb = 0;
    std::tie(expected_tm_status, expected_tm_status_lsb) = it;
    EXPECT_EQ(expected_tm_status >> 16, ingress_phv->get_p(221));
    EXPECT_EQ(expected_tm_status & 0xffff, ingress_phv->get_p(220));
    // for WIP check th tm_status_phv_sec
    if (!RmtObject::is_jbayXX() && (4 == rpt)) {
      EXPECT_EQ(expected_tm_status_lsb, ingress_phv->get_p(217));
    } else if (!RmtObject::is_jbayXX() && (5 == rpt)) {
      EXPECT_EQ(expected_tm_status_lsb << 8, ingress_phv->get_p(217));
    } else {
      EXPECT_EQ(0u, ingress_phv->get_p(217));
    }
    om_->pkt_delete(ingress_pkt);
    rpt++;
  }
  tu_->quieten_p4_log_flags(pipes_mask());
}

}
