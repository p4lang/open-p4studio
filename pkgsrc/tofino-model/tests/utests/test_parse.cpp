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
#include "test_parse.h"
#include "cntrstack_util.h"

#include <model_core/model.h>
#include <register_includes/model_mem.h>
#include <bitvector.h>
#include <tcam.h>
#include <rmt-object-manager.h>
#include <packet.h>
#include <phv.h>
#include <parser.h>
#include <parser-block.h>
#include <parser-static-config.h>
#include <ipb.h>
#include <p4-name-lookup.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool prs_print = false;
  bool prs_printf = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


  void MCN_CONCAT(BFN_TEST_NAME(ParseTest), UsingConfigBase)::SetUp() {
    set_pipe_index(0);
    BaseTest::SetUp();

    if (prs_print) RMT_UT_LOG_INFO("test_parse_using_config()\n");
    // Now a Packet on heap
    if (prs_print) RMT_UT_LOG_INFO("Create packet\n");
    pkt1_ = om_->pkt_create(udp_pktstr_);
    ASSERT_TRUE(pkt1_ != NULL);
    pkt2_ = om_->pkt_create(tcp_pktstr_);
    ASSERT_TRUE(pkt2_ != NULL);
    // Create a parser
    if (prs_print) RMT_UT_LOG_INFO("Get parser\n");
    parser_ = om_->parser_get(pipe_index(), 16)->ingress();
    ASSERT_TRUE(parser_ != NULL);
    parser_->reset();

    // Initialize with basic config
    if (prs_print) RMT_UT_LOG_INFO("Initialize parser\n");
    parser_config_basic_eth_ip_tcp(parser_);
    parser_->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
    parser_->set_channel(0, true, 0);
  }

  void MCN_CONCAT(BFN_TEST_NAME(ParseTest), UsingConfigBase)::do_test() {
    // Switch on some debug
    om_->update_log_flags(ALL,ALL,TYP_PARSER,ALL,ALL,FEW,FEW);
    //om_->update_log_flags(ALL,ALL,TYP_PARSER,ALL,ALL,ALL,ALL);
    load_context_json_file(pipe_index());
    EXPECT_TRUE(om_->p4_name_lookup(pipe_index()).IsLoaded());

    if (prs_print) parser_->print();
    // Now see if we can parse packets
    if (prs_print) RMT_UT_LOG_INFO("Parse packet\n");
    // switch on more P4 logging
    om_->update_log_type_levels(pipes_mask(),ALL,RMT_LOG_TYPE_P4,ALL,RmtDebug::kRmtDebugVerbose);
    RmtLoggerCapture *log_capture = rmt_logger_capture();
    log_capture->start();
    Phv *phv1 = parser_->parse(pkt1_, 0);
    log_capture->stop();
    // revert to less logging
    tu_->quieten_p4_log_flags(pipes_mask());
    ASSERT_TRUE(phv1 != NULL);
    if (prs_print) phv1->print_p("PHV1 DUMP", prs_printf);
    EXPECT_EQ(0x1188u, phv1->get_p(Phv::make_word(4,7)));
    EXPECT_EQ(0x1199u, phv1->get_p(Phv::make_word(4,8)));
    EXPECT_EQ(0u, phv1->get_p(Phv::make_word(3,0)));
    EXPECT_EQ(0x11u, phv1->get_p(Phv::make_word(3,1)));
    EXPECT_EQ(0x22u, phv1->get_p(Phv::make_word(3,2)));
    EXPECT_EQ(0x33u, phv1->get_p(Phv::make_word(3,3)));
    // check a subset of logging lines...
    int line_count = log_capture->for_each_line_containing(
        "PHV word 128",
        [](int line_num, size_t pos, std::string line)->void {
          EXPECT_FALSE(line.find("I [my_metadata.da_hi_16[7:0]]\n") == std::string::npos);
        });
    EXPECT_GT(line_count, 0);
    line_count = log_capture->for_each_line_containing(
        "PHV word   3",
        [](int line_num, size_t pos, std::string line)->void {
          EXPECT_FALSE(line.find("I [my_metadata.ip4_dst[31:0]]\n") == std::string::npos);
        });
    EXPECT_GT(line_count, 0);
    line_count = log_capture->for_each_line_containing(
        "PHV word  67",
        [](int line_num, size_t pos, std::string line)->void {
          EXPECT_FALSE(line.find("I [my_metadata.ip4_proto[7:0]]\n") == std::string::npos);
        });
    EXPECT_GT(line_count, 0);

    Phv *phv2 = parser_->parse(pkt2_, 0);
    ASSERT_TRUE(phv2 != NULL);
    EXPECT_EQ(0x0688u, phv2->get_p(Phv::make_word(4,7)));
    EXPECT_EQ(0x0699u, phv2->get_p(Phv::make_word(4,8)));
    EXPECT_EQ(0u, phv1->get_p(Phv::make_word(3,0)));
    EXPECT_EQ(0x11u, phv2->get_p(Phv::make_word(3,1)));
    EXPECT_EQ(0x22u, phv2->get_p(Phv::make_word(3,2)));
    EXPECT_EQ(0x33u, phv2->get_p(Phv::make_word(3,3)));
    if (prs_print) phv2->print_p("PHV2 DUMP", prs_printf);
  }

  void MCN_CONCAT(BFN_TEST_NAME(ParseTest), UsingConfigBase)::TearDown() {
    // Cleanup
    if (prs_print) RMT_UT_LOG_INFO("Delete pkt1\n");
    om_->pkt_delete(pkt1_);
    if (prs_print) RMT_UT_LOG_INFO("Delete pkt2\n");
    om_->pkt_delete(pkt2_);
    BaseTest::TearDown();
  }


  TEST_F(MCN_CONCAT(BFN_TEST_NAME(ParseTest), UsingConfigBase), UsingConfig) {
    do_test();
  }

  TEST_F(MCN_CONCAT(BFN_TEST_NAME(ParseTest), UsingConfigBase), CounterRamAccess) {
    // Change index 254 to load from counter ram
    // NB OP2 will be mapped on WIP to equivalent OP4
    parser_->set_counter_load_addr(254, 0x0);
    parser_->set_counter_init(0, 0, 7, 0, 0xFF, 0, 1);
    parser_->set_counter_ctr_op(254, Parser::kCounter2LoadFromCntrRam);
    do_test();

    // Set unexpected bits in address (addr should only use bits 3:0)
    // Should be no throw given relaxation active
    parser_->set_counter_load_addr(254, 0xF0);
    parser_->kRelaxExtractionCheck = true;
    GLOBAL_THROW_ON_ERROR = 1;
    EXPECT_NO_THROW(do_test());
    GLOBAL_THROW_ON_ERROR = 0;

    // Switch off relaxation.
    // Should now see throws on JBay and WIP
    parser_->kRelaxExtractionCheck = false;
    GLOBAL_THROW_ON_ERROR = 1;
    EXPECT_THROW(do_test(), std::runtime_error);

    GLOBAL_THROW_ON_ERROR = 0;
  }


  TEST_F(MCN_CONCAT(BFN_TEST_NAME(ParseTest), UsingConfigBase),UsingConfigMultiWR) {
    if (prs_print) RMT_UT_LOG_INFO("test_parse_using_config_multi_wr()\n");
    bool jbay = RmtObject::is_jbay_or_later();
    // Note: this test uses slightly different pktstr vs base class
    //                    <   DA            SA      ET I4 C LN  ID  OF  T P CK   SRC     DST    SP  DP>
    //                    <   DA      ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST  ><SP><DP>
    const char *pktstr1 = "080022AABBCC080011DDEEFF080045000100123400001006FFFF0A1122330A4455661188119900000000000000000000000000000000";

    // Now a Packet on heap
    Packet *pkt1 = om_->pkt_create(pktstr1);
    ASSERT_TRUE(pkt1 != NULL);

    // Switch on some debug
    om_->update_log_flags(ALL,ALL,TYP_PARSER,ALL,ALL,FEW,FEW);
    //om_->update_log_flags(ALL,ALL,TYP_PARSER,ALL,ALL,ALL,ALL);

    // Hunt for first 16b extractors that are extracting
    // SPORT/DPORT (should be TCP at index 253)
    int sport = Phv::make_word(4,7);
    int dport = Phv::make_word(4,8);
    int i_sport, i_dport, indexTCP = -1;

    // Figure our SPORT/DPORT off16 vals
    int sport_off16, dport_off16, sport_sz, dport_sz;
    Phv::phv_index_to_off16_p(sport, &sport_sz, &sport_off16, NULL, NULL);
    Phv::phv_index_to_off16_p(dport, &dport_sz, &dport_off16, NULL, NULL);
    EXPECT_EQ(16, sport_sz);
    EXPECT_EQ(16, dport_sz);
    int sport_mr = (jbay) ?sport_off16 :sport;

    // ingress parser created by Setup
    Parser *ingress_parser = parser_;

    // create egress_parser now
    Parser *egress_parser = om_->parser_get(pipe_index(), 16)->egress();
    ASSERT_TRUE(egress_parser != NULL);
    egress_parser->reset();
    parser_config_basic_eth_ip_tcp(egress_parser);
    egress_parser->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
    egress_parser->set_channel(0, true, 0);

    for (int ie = 0; ie <= 1; ie++) {

      Parser *parser = (ie == 0) ?ingress_parser :egress_parser;

      // Tie all PHV words to ingress/egress as appropriate
      int phv_or_off_max = (jbay) ?256 :224;
      for (int phv_or_off = 0; phv_or_off < phv_or_off_max; phv_or_off++) {
        parser->phv_set_owner(phv_or_off);
      }

      for (int index = 255; index > 0; index--) {
        // Check NO parser entries setup to use offset_adj yet
        ASSERT_FALSE(parser->offset_reset(index));
        EXPECT_EQ(0, parser->offset_incr_val(index));

        i_sport = -1; i_dport = -1;
        for (int i = 0; i < RmtDefs::kParserExtract16bWordsPerCycle; i++) {
          int phv_word = parser->get_phv_to_write(parser->extract16_dst_phv_by_phv(index, i));
          if      (phv_word == sport)  i_sport = i;
          else if (phv_word == dport)  i_dport = i;
        }
        if ((i_sport >= 0) && (i_dport >= 0)) {
          indexTCP = index;
          break;
        }
      }
      EXPECT_EQ(253, indexTCP);
      EXPECT_NE(i_sport, i_dport);
      EXPECT_LT(i_sport, i_dport);

      // Now see if we can parse a TCP packet (pkt1)
      if (prs_print) RMT_UT_LOG_INFO("Parse packet\n");
      Phv *phv1 = parser->parse(pkt1, 0);
      ASSERT_TRUE(phv1 != NULL);
      EXPECT_EQ(0x1188u, phv1->get_p(sport));
      EXPECT_EQ(0x1199u, phv1->get_p(dport));

      // Now change parser config to do MultiWR to a field
      // We set DPORT extractor to extract to SPORT word
      // Should now see DPORT val 0x1199 in SPORT phv word
      //
      parser->set_extract16_dst_phv_by_phv(indexTCP, i_dport, sport);
      Phv *phv2 = parser->parse(pkt1, 0);
      ASSERT_TRUE(phv2 != NULL);
      EXPECT_EQ(0x1199u, phv2->get_p(sport));

      // Now we do MultiWR again but this time disallow MultiWR
      // First write should succeed second should fail.
      // We also enable multi_wr_err reporting so we get back a PHV
      // Should go back to seeing SPORT val 0x1188 in SPORT phv word
      // (DPORT extract will have been ignored)
      // !!!!!!!!!!!!!!!!!!!!!!!!!!! NOTE !!!!!!!!!!!!!!!!!!!!!!!!!!
      // On JBay (as of Mar 2018) ANY errors in a cycle prevent ALL
      // extractions for that cycle so we expect the sport value to
      // remain 0 on JBay.
      // !!!!!!!!!!!!!!!!!!!!!!!!!!! NOTE !!!!!!!!!!!!!!!!!!!!!!!!!!
      //
      parser->phv_set_multi_write(sport_mr, false);

      // store multi wr errors in phv word (4,9)
      // Note since XXX, model is more faithful to RTL and does not
      // require individual parser errors to be enabled ***on egress***
      // (correct error, here kErrMultiWr, should still be reported though)
      uint32_t prs_errors = (ie == 0) ?Parser::kErrMultiWr :0u;
      uint32_t perr_word = 137;
      parser->set_perr_phv_output(0, prs_errors, perr_word);
      EXPECT_EQ(0u, parser->last_err_flags());
      // switch on more P4 logging
      om_->update_log_type_levels(ALL, ALL, RMT_LOG_TYPE_P4, ALL, RmtDebug::kRmtDebugVerbose);
      RmtLoggerCapture *log_capture = rmt_logger_capture();
      log_capture->start();
      Phv *phv3 = parser->parse(pkt1, 0);
      log_capture->stop();
      tu_->quieten_p4_log_flags();
      ASSERT_TRUE(phv3 != NULL);
      uint32_t exp = (jbay) ?0u :0x1188u;
      EXPECT_EQ(exp, phv3->get_p(sport));
      uint32_t expected_multi_wr_err = Parser::kErrMultiWr;
      EXPECT_EQ(expected_multi_wr_err, parser->last_err_flags());
      EXPECT_EQ(expected_multi_wr_err, phv3->get_p(perr_word));

      // check logging lines include parser errors being written to PHV
      int line_count = log_capture->for_each_line_containing(
          "Ingress Parser emitted errors 0x0100, updating PHV word 137 to value 0x0100",
          nullptr);
      EXPECT_EQ(line_count, 1) << log_capture->dump_lines();

      // Put DPORT extractor back to extracting to DPORT word
      // Things should go back to how they were
      //
      parser->phv_set_multi_write(sport_mr, true);
      parser->set_extract16_dst_phv_by_phv(indexTCP, i_dport, dport);
      Phv *phv4 = parser->parse(pkt1, 0);
      ASSERT_TRUE(phv4 != NULL);
      EXPECT_EQ(0x1188u, phv4->get_p(sport));
      EXPECT_EQ(0x1199u, phv4->get_p(dport));
      EXPECT_EQ(0u, phv4->get_p(perr_word));


      // Repeat above but now use offset_adjustment facility to
      // make Parser do MultiWr to a field.
      //
      // SPORT extractor is still ostensibly configured to extract to
      // SPORT word BUT we configure SPORT extractor to take account of
      // offset_adjust (and we arrange for offset_adj to be +1).
      // This means the SPORT extractor actually writes to DPORT word!
      //
      // Should still see DPORT val 0x1199 in DPORT phv word as
      // that extract happens second and MultiWr permitted.
      // However SPORT val will now NOT be in SPORT phv word which
      // should remain 0

      // Setup first parser entry (255) such that offset_adj becomes +1
      parser->set_offset_reset(255, true); // Set TRUE
      parser->set_offset_incr_val(255, 1); // Set offset_adj +1
      // Then set SPORT extractor to take account of offset_adj +1 val
      parser->set_extract16_add_off(indexTCP, i_sport, true);

      Phv *phv5 = parser->parse(pkt1, 0);
      ASSERT_TRUE(phv5 != NULL);
      EXPECT_EQ(0x0u, phv5->get_p(sport));
      EXPECT_EQ(0x1199u, phv5->get_p(dport));

      // Now we do MultiWR again but this time disallow MultiWR
      // First write should succeed second should fail.
      // We also enable multi_wr_err reporting so we get back a PHV
      // Should go back to seeing SPORT val 0x1188 in DPORT phv word
      // (second DPORT extract will have been ignored)
      // !!!!!!!!!!!!!!!!!!!!!!!!!!! NOTE !!!!!!!!!!!!!!!!!!!!!!!!!!
      // On JBay (as of Mar 2018) ANY errors in a cycle prevent ALL
      // extractions for that cycle so this time we expect the sport
      // value and the dport value to remain 0 on JBay.
      // !!!!!!!!!!!!!!!!!!!!!!!!!!! NOTE !!!!!!!!!!!!!!!!!!!!!!!!!!
      //
      int dport_mr = (jbay) ?dport_off16 :dport;

      parser->phv_set_multi_write(dport_mr, false);
      parser->set_perr_phv_output(0, prs_errors, perr_word);
      // last successful parse will have reset err flags, but sanity check...
      EXPECT_EQ(0u, parser->last_err_flags());
      Phv *phv6 = parser->parse(pkt1, 0);
      ASSERT_TRUE(phv6 != NULL);
      EXPECT_EQ(0x0u, phv6->get_p(sport));
      EXPECT_EQ(expected_multi_wr_err, phv6->get_p(perr_word));

      // This next check fails without the XXX fix in src/jbay/parser.cpp
      // (the fix takes account of offset_adjust when checking for
      //  PhvOwner/MultiWR errors)
      // Without the fix, there is NO detection of the MultiWR in
      // Parser::other_early_error_checks(), so extraction DOES occur
      // and 0x1188u DOES get put into the DPORT phv word.
      uint32_t exp_dport = (jbay) ?0u :0x1188u;
      EXPECT_EQ(exp_dport, phv6->get_p(dport));
      EXPECT_EQ(expected_multi_wr_err, parser->last_err_flags());

    } // for (int ie = 0; ie <= 1; ie++)

    // Quieten
    om_->update_log_flags(ALL,ALL,ALL,ALL,ALL,FEW,FEW);
    // Cleanup
    if (prs_print) RMT_UT_LOG_INFO("Delete pkt1\n");
    om_->pkt_delete(pkt1);
  }


  TEST_F(BFN_TEST_NAME(ParserTestBase),UsingConfigTestPri) {
    if (prs_print) RMT_UT_LOG_INFO("test_parse_using_config_test_pri()\n");

    //                    <   DA           SA      ET I4 C LN  ID  OF  T P CK   SRC     DST     SP  DP>
    //                    <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST   ><SP><DP>
    const char *pktudp1 = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A4455661188110100000000000000000000000000000000";
    const char *pkttcp1 = "080022AABBCC080011DDEEFF080045000100123400001006FFFF0A1122330A4455660688060100000000000000000000000000000000";
    const char *pktudp2 = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A4455661188110200000000000000000000000000000000";
    const char *pkttcp2 = "080022AABBCC080011DDEEFF080045000100123400001006FFFF0A1122330A4455660688060200000000000000000000000000000000";
    const char *pktudp3 = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A4455661188110400000000000000000000000000000000";
    const char *pkttcp3 = "080022AABBCC080011DDEEFF080045000100123400001006FFFF0A1122330A4455660688060300000000000000000000000000000000";
    // Now a Packet on heap
    if (prs_print) RMT_UT_LOG_INFO("Create packet\n");
    Packet *udp1 = om_->pkt_create(pktudp1);
    Packet *udp2 = om_->pkt_create(pktudp2);
    Packet *udp3 = om_->pkt_create(pktudp3);
    ASSERT_TRUE((udp1 != NULL) && (udp2 != NULL) && (udp3 != NULL));
    Packet *tcp1 = om_->pkt_create(pkttcp1);
    Packet *tcp2 = om_->pkt_create(pkttcp2);
    Packet *tcp3 = om_->pkt_create(pkttcp3);
    ASSERT_TRUE((tcp1 != NULL) && (tcp2 != NULL) && (tcp3 != NULL));

    // Create a parser
    if (prs_print) RMT_UT_LOG_INFO("Get parser\n");
    Parser *parser = om_->parser_get(0,0)->ingress();
    ASSERT_TRUE(parser != NULL);

    // Switch on some debug
    om_->update_log_flags(ALL,ALL,TYP_PARSER,ALL,ALL,FEW,FEW);
    //om_->update_log_flags(ALL,ALL,TYP_PARSER,ALL,ALL,ALL,ALL);
    parser->reset();

    // Initialize with basic config - pri_thresh set to 2
    if (prs_print) RMT_UT_LOG_INFO("Initialize parser\n");
    parser_config_basic_eth_ip_tcp(parser);
    parser->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
    if (prs_print) parser->print();
    // Now see if we can parse packets
    auto *ipb_counters = om_->ipb_lookup(
        parser->pipe_index(), parser->ipb_index(0))->get_ipb_counters(
            parser->ipb_chan(0));
    auto *epb = om_->epb_lookup(parser->pipe_index(),
                                parser->epb_index(0));
    int epb_chan = parser->epb_chan(0);
    // sanity checks...
    EXPECT_EQ(0u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 0u);
    verify_epb_counters(epb, epb_chan, 0u);
    // Uncongested state - UDP - no drop
    Phv *phvudp1 = parser->parse(udp1, 0);
    EXPECT_TRUE(phvudp1 != NULL);
    EXPECT_EQ(0x1188u, phvudp1->get_p(Phv::make_word(4,7)));
    EXPECT_EQ(1u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 0u);
    verify_epb_counters(epb, epb_chan, 0u);
    // Uncongested state - TCP - no drop
    Phv *phvtcp1 = parser->parse(tcp1, 0);
    EXPECT_TRUE(phvtcp1 != NULL);
    EXPECT_EQ(0x0688u, phvtcp1->get_p(Phv::make_word(4,7)));
    EXPECT_EQ(2u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 0);
    verify_epb_counters(epb, epb_chan, 0u);

    // Congested channel - UDP - packet pri is 1 - DROP so no PHV
    parser->set_channel_congested(0, true);
    Phv *phvudp1_a = parser->parse(udp1, 0);
    EXPECT_TRUE(phvudp1_a == NULL);
    EXPECT_EQ(2u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 1);
    verify_epb_counters(epb, epb_chan, 0u);
    // Congested channel - TCP - packet pri is 1 - DROP so no PHV
    Phv *phvtcp1_a = parser->parse(tcp1, 0);
    EXPECT_TRUE(phvtcp1_a == NULL);
    EXPECT_EQ(2u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 2);
    verify_epb_counters(epb, epb_chan, 0u);
    parser->set_channel_congested(0, false);

    // Congested packet - UDP - packet pri is 1 - DROP so no PHV
    udp1->qing2e_metadata()->set_ing_congested(true);
    Phv *phvudp1_b = parser->parse(udp1, 0);
    EXPECT_TRUE(phvudp1_b == NULL);
    EXPECT_EQ(2u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 3);
    verify_epb_counters(epb, epb_chan, 0u);
    // Congested packet - TCP - packet pri is 1 - DROP so no PHV
    tcp1->qing2e_metadata()->set_ing_congested(true);
    Phv *phvtcp1_b = parser->parse(tcp1, 0);
    EXPECT_TRUE(phvtcp1_b == NULL);
    EXPECT_EQ(2u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 4);
    verify_epb_counters(epb, epb_chan, 0u);

    // Congested channel - UDP - packet pri is 2 - NO DROP
    parser->set_channel_congested(0, true);
    Phv *phvudp2 = parser->parse(udp2, 0);
    EXPECT_TRUE(phvudp2 != NULL);
    EXPECT_EQ(0x1188u, phvudp2->get_p(Phv::make_word(4,7)));
    EXPECT_EQ(3u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 4);
    verify_epb_counters(epb, epb_chan, 0u);
    // Congested channel - TCP - packet pri is 2 - NO DROP
    Phv *phvtcp2 = parser->parse(tcp2, 0);
    EXPECT_TRUE(phvtcp2 != NULL);
    EXPECT_EQ(0x0688u, phvtcp2->get_p(Phv::make_word(4,7)));
    EXPECT_EQ(4u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 4);
    verify_epb_counters(epb, epb_chan, 0u);
    parser->set_channel_congested(0, false);

    // Congested packet - UDP - packet pri is 2 - NO DROP
    udp2->qing2e_metadata()->set_ing_congested(true);
    Phv *phvudp2_a = parser->parse(udp2, 0);
    EXPECT_TRUE(phvudp2_a != NULL);
    EXPECT_EQ(0x1188u, phvudp2_a->get_p(Phv::make_word(4,7)));
    EXPECT_EQ(5u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 4);
    verify_epb_counters(epb, epb_chan, 0u);
    // Congested packet - TCP - packet pri is 2 - NO DROP
    tcp2->qing2e_metadata()->set_ing_congested(true);
    Phv *phvtcp2_a = parser->parse(tcp2, 0);
    EXPECT_TRUE(phvtcp2_a != NULL);
    EXPECT_EQ(0x0688u, phvtcp2_a->get_p(Phv::make_word(4,7)));
    EXPECT_EQ(6u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 4);
    verify_epb_counters(epb, epb_chan, 0u);

    // Congested channel - UDP - packet pri is 0 (4 & 0x3) - DROP
    // NB. Regardless of action_pri mask 0x3 setup for UDP, final
    //     output pri (the one seen in pkt->priority() always masked
    //     in 0x3 (Tofino) or identity mapped 4->4 by pri_map (JBay/WIP)
    //     and then masked in 0x3.
    parser->set_channel_congested(0, true);
    Phv *phvudp3 = parser->parse(udp3, 0);
    EXPECT_TRUE(phvudp3 == NULL);
    EXPECT_EQ(6u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 5);
    verify_epb_counters(epb, epb_chan, 0u);
    // Congested channel - TCP - packet pri is 3 (3 & 0x7) - NO DROP
    // NB. Regardless of action_pri mask 0x7 setup for TCP, final
    //     output pri (the one seen in pkt->priority() always masked
    //     in 0x3 (Tofino) or identity mapped 3->3 by pri_map (JBay/WIP)
    //     and then masked in 0x3.
    Phv *phvtcp3 = parser->parse(tcp3, 0);
    EXPECT_TRUE(phvtcp3 != NULL);
    EXPECT_EQ(0x0688u, phvtcp3->get_p(Phv::make_word(4,7)));
    EXPECT_EQ(7u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 5);
    verify_epb_counters(epb, epb_chan, 0u);
    parser->set_channel_congested(0, false);

    // Congested packet - UDP - packet pri is 0 (4 & 0x3) - DROP
    set_max_chnl_parser_send_pkt_err(ipb_counters);  // should wrap to zero
    udp3->qing2e_metadata()->set_ing_congested(true);
    Phv *phvudp3_a = parser->parse(udp3, 0);
    EXPECT_TRUE(phvudp3_a == NULL);
    EXPECT_EQ(7u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 0);
    verify_epb_counters(epb, epb_chan, 0u);
    // Congested packet - TCP - packet pri is 3 (3 & 0x7) - NO DROP
    set_max_chnl_parser_send_pkt(ipb_counters);  // should wrap to zero
    tcp3->qing2e_metadata()->set_ing_congested(true);
    Phv *phvtcp3_a = parser->parse(tcp3, 0);
    EXPECT_TRUE(phvtcp3_a != NULL);
    EXPECT_EQ(0x0688u, phvtcp3_a->get_p(Phv::make_word(4,7)));
    EXPECT_EQ(0u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 0);
    verify_epb_counters(epb, epb_chan, 0u);


    // Quieten
    om_->update_log_flags(ALL,ALL,ALL,ALL,ALL,FEW,FEW);

    // Cleanup
    om_->pkt_delete(udp1);
    om_->pkt_delete(udp2);
    om_->pkt_delete(udp3);
    om_->pkt_delete(tcp1);
    om_->pkt_delete(tcp2);
    om_->pkt_delete(tcp3);
  }

  TEST_F(BFN_TEST_NAME(ParserTestBase), EgressCounters) {
    // verify that EPB counters are incremented when parsing egress packets,
    // and that IPB counters are not incremented
    om_->update_log_flags(ALL,ALL,TYP_PARSER,ALL,ALL,FEW,FEW);

    //                    <   DA           SA      ET I4 C LN  ID  OF  T P CK   SRC     DST     SP  DP>
    //                    <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST   ><SP><DP>
    const char *pktudp1 = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A4455661188110100000000000000000000000000000000";
    Packet *udp1 = om_->pkt_create(pktudp1);
    // Create an *egress*  parser
    if (prs_print) RMT_UT_LOG_INFO("Get parser\n");
    int parser_chan = 0;
    Parser *parser = om_->parser_get(pipe_index(), 0)->egress();
    parser->reset();
    parser_config_basic_eth_ip_tcp(parser);
    parser->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
    auto *ipb_counters = om_->ipb_lookup(
        parser->pipe_index(), parser->ipb_index(parser_chan))->get_ipb_counters(
            parser->ipb_chan(parser_chan));
    auto *epb = om_->epb_lookup(parser->pipe_index(),
                               parser->epb_index(parser_chan));
    int epb_chan = parser->epb_chan(parser_chan);
    // sanity checks
    EXPECT_EQ(0u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 0u);
    verify_epb_counters(epb, epb_chan, 0u);

    // Uncongested state - UDP - no drop
    Phv *phvudp1 = parser->parse(udp1, parser_chan);
    ASSERT_TRUE(phvudp1 != NULL);
    EXPECT_EQ(0x1188u, phvudp1->get_p(Phv::make_word(4, 7)));
    // ipb counters should not be incremented
    EXPECT_EQ(0u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 0u);
    verify_epb_counters(epb, epb_chan, 1u);

    // Congested channel - UDP - packet pri is 1 - *egress* pkts aren't dropped
    parser->set_channel_congested(parser_chan, true);
    phvudp1 = parser->parse(udp1, parser_chan);
    ASSERT_TRUE(phvudp1 != NULL);
    EXPECT_EQ(0x1188u, phvudp1->get_p(Phv::make_word(4, 7)));
    EXPECT_EQ(0u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 0);
    verify_epb_counters(epb, epb_chan, 2u);

    // check the epb counter wraps
    uint64_t max_val = set_max_epb_chnl_parser_send_pkt(epb, epb_chan);
    // sanity check ...
    verify_epb_counters(epb, epb_chan, max_val);
    phvudp1 = parser->parse(udp1, parser_chan);
    ASSERT_TRUE(phvudp1 != NULL);
    EXPECT_EQ(0x1188u, phvudp1->get_p(Phv::make_word(4, 7)));
    EXPECT_EQ(0u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
    verify_chnl_parser_send_pkt_err(ipb_counters, 0);
    verify_epb_counters(epb, epb_chan, 0u);

    // Cleanup
    om_->update_log_flags(ALL,ALL,ALL,ALL,ALL,FEW,FEW);
    om_->pkt_delete(udp1);
  }

  TEST(BFN_TEST_NAME(ParseTest),Inline1) {
    // Configure parser from within test
    GLOBAL_MODEL->Reset();
    if (prs_print) RMT_UT_LOG_INFO("test_parse_inline1()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();

    bool      T = true;
    bool      F = false;
    uint16_t  NoX = k_phv::kBadPhv;
    uint16_t  DA_HI_16 = Phv::make_word(4,0);
    uint16_t  DA_LO_32 = Phv::make_word(0,0);
    uint16_t  SA_HI_16 = Phv::make_word(4,1);
    uint16_t  SA_LO_32 = Phv::make_word(0,1);
    uint16_t  ETH_TYPE = Phv::make_word(4,2);
    uint16_t  IP4_HL   = Phv::make_word(2,0);
    uint16_t  IP4_TTL  = Phv::make_word(2,2);
    uint16_t  IP4_PROTO= Phv::make_word(2,3);
    uint16_t  IP4_LEN  = Phv::make_word(4,4);
    uint16_t  IP4_ID   = Phv::make_word(4,3);
    uint16_t  IP4_FRAG = Phv::make_word(4,5);
    uint16_t  IP4_CKSM = Phv::make_word(4,6);
    uint16_t  IP4_SRC  = Phv::make_word(0,2);
    uint16_t  IP4_DST  = Phv::make_word(0,3);
    uint16_t  P_SPORT  = Phv::make_word(4,7);
    uint16_t  P_DPORT  = Phv::make_word(4,8);
    uint16_t  PHV8_0   = Phv::make_word(3,0);
    uint16_t  PHV8_1   = Phv::make_word(3,1);
    uint16_t  PHV8_2   = Phv::make_word(3,2);
    uint16_t  PHV8_3   = Phv::make_word(3,3);
    bool      b_FF[] = { F,F };
    bool      b_FFFF[] = { F,F,F,F };
    bool      b_TTTT[] = { T,T,T,T };
    uint8_t   u8_00[] = { 0,0 };

    // Create a parser and initialize with config
    Parser *p = om->parser_get(0,0)->ingress();
    ASSERT_TRUE(p != NULL);
    p->reset();

    // Setup a priority map that does nothing
    p->set_identity_priority_map();

    // ENTRY 0:
    // Raw packet header - strip off Eth header (*no* VLAN) - stash DA/SA/ETH_TYPE
    p->set_early_action(255,
                        0, F, F,   // Counter load src, IMM_VAL, LOAD
                        F, 14,     // DONE, shift_amount
                        14,15,12,  // field8_1 off, field8_0 off, field16 off
                        T,T,T,     // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, 1);  // next_state_mask, next_state

    uint8_t  a0_u8_src[]  = {0,0x11,0x22,0x33};
    uint16_t a0_u8_dst[]  = {PHV8_0,PHV8_1,PHV8_2,PHV8_3};
    uint8_t  a0_u16_src[] = {0,6,12,0};
    uint16_t a0_u16_dst[] = {DA_HI_16,SA_HI_16,ETH_TYPE,NoX};
    uint8_t  a0_u32_src[] = {2,8,0,0};
    uint16_t a0_u32_dst[] = {DA_LO_32,SA_LO_32,NoX,NoX};

    p->set_action(255,
                  F, 0, b_FF, u8_00,                       // adj  RESET, adj_inc, checksum ENABLE, checksum_addr
                  b_TTTT, b_FFFF, a0_u8_src,  a0_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a0_u16_src, a0_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a0_u32_src, a0_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    p->set_tcam_match(255,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(0,    (uint16_t)0x9999, (uint8_t)0x99, (uint8_t)0x99),  // value (state=0)
                      p->make_tcam_entry(0xFF, (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask
    // ENTRY 1:
    // Stripped off Ether header - see if we have Eth.Ethertype=IPv4 and IP.version=4 - stash IP fields
    p->set_early_action(254,
                        0, F, F,   // Counter load src, IMM_VAL, LOAD
                        F, 20,     // DONE, shift_amount (get rid IP)
                        11,10,8,   // field8_1 off, field8_0 off, field16 off
                        T,T,T,     // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, 2);  // next_state_mask, next_state

    uint8_t  a1_u8_src[]  = {0,8,9,0};
    uint16_t a1_u8_dst[]  = {IP4_HL,IP4_TTL,IP4_PROTO,NoX};
    uint8_t  a1_u16_src[] = {2,4,6,10};
    uint16_t a1_u16_dst[] = {IP4_LEN,IP4_ID,IP4_FRAG,IP4_CKSM};
    uint8_t  a1_u32_src[] = {12,16,0,0};
    uint16_t a1_u32_dst[] = {IP4_SRC,IP4_DST,NoX,NoX};

    p->set_action(254,
                  F, 0, b_FF, u8_00,                       // adj  RESET, adj_inc, checksum ENABLE, checksum_addr
                  b_FFFF, b_FFFF, a1_u8_src,  a1_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a1_u16_src, a1_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a1_u32_src, a1_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    p->set_tcam_match(254,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(   1, (uint16_t)0x0800, (uint8_t)0x40, (uint8_t)0x99),  // value (state=1)
                      p->make_tcam_entry(0xFF, (uint16_t)0xFFFF, (uint8_t)0xF0, (uint8_t)   0)); // mask
    // ENTRY 2:
    // Stripped off IP header - see if we have IP_PROTO==TCP - stash TCP_SPORT/TCP_DPORT fields
    p->set_early_action(253,
                        0, F, F,    // Counter load src, IMM_VAL, LOAD
                        T, 20,      // ****DONE****, shift_amount (get rid TCP)
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        T,T,T,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, 255); // next_state_mask, next_state

    uint8_t  a2_u8_src[]  = {0,0,0,0};
    uint16_t a2_u8_dst[]  = {NoX,NoX,NoX,NoX};
    uint8_t  a2_u16_src[] = {0,2,0,0};
    uint16_t a2_u16_dst[] = {P_SPORT,P_DPORT,NoX,NoX};
    uint8_t  a2_u32_src[] = {0,0,0,0};
    uint16_t a2_u32_dst[] = {NoX,NoX,NoX,NoX};

    p->set_action(253,
                  F, 0, b_FF, u8_00,                       // adj  RESET, adj_inc, checksum ENABLE, checksum_addr
                  b_FFFF, b_FFFF, a2_u8_src,  a2_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a2_u16_src, a2_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a2_u32_src, a2_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    p->set_tcam_match(253,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(   2, (uint16_t)0x9906, (uint8_t)0x99, (uint8_t)0x99),  // value (state=2)
                      p->make_tcam_entry(0xFF, (uint16_t)0x00FF, (uint8_t)   0, (uint8_t)   0)); // mask
    // ENTRY 3:
    // Stripped off IP header - see if we have IP_PROTO==UDP - stash UDP_SPORT/UDP_DPORT fields
    p->set_early_action(252,
                        0, F, F,    // Counter load src, IMM_VAL, LOAD
                        T, 8,      // ****DONE****, shift_amount (get rid UDP)
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        T,T,T,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, 255); // next_state_mask, next_state

    uint8_t  a3_u8_src[]  = {0,0,0,0};
    uint16_t a3_u8_dst[]  = {NoX,NoX,NoX,NoX};
    uint8_t  a3_u16_src[] = {0,2,0,0};
    uint16_t a3_u16_dst[] = {P_SPORT,P_DPORT,NoX,NoX};
    uint8_t  a3_u32_src[] = {0,0,0,0};
    uint16_t a3_u32_dst[] = {NoX,NoX,NoX,NoX};

    p->set_action(252,
                  F, 0, b_FF, u8_00,                       // adj  RESET, adj_inc, checksum ENABLE, checksum_addr
                  b_FFFF, b_FFFF, a3_u8_src,  a3_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a3_u16_src, a3_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a3_u32_src, a3_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    p->set_tcam_match(252,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(   2, (uint16_t)0x9911, (uint8_t)0x99, (uint8_t)0x99),  // value (state=2 REUSE)
                      p->make_tcam_entry(0xFF, (uint16_t)0x00FF, (uint8_t)   0, (uint8_t)   0)); // mask
    // ENTRY 4:
    // Catch-all entry - will match anything not matched above
    p->set_early_action(251,
                        0, F, F,    // Counter load src, IMM_VAL, LOAD
                        T, 0,      // ****DONE****, shift_amount
                        0,0,0,      // field8_1 off, field8_0 off, field16 off
                        T,T,T,      // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, 255); // next_state_mask, next_state

    uint8_t  a4_u8_src[]  = {0,0,0,0};
    uint16_t a4_u8_dst[]  = {NoX,NoX,NoX,NoX};
    uint8_t  a4_u16_src[] = {0,0,0,0};
    uint16_t a4_u16_dst[] = {NoX,NoX,NoX,NoX};
    uint8_t  a4_u32_src[] = {0,0,0,0};
    uint16_t a4_u32_dst[] = {NoX,NoX,NoX,NoX};

    p->set_action(251,
                  F, 0, b_FF, u8_00,                       // adj  RESET, adj_inc, checksum ENABLE, checksum_addr
                  b_FFFF, b_FFFF, a4_u8_src,  a4_u8_dst,   // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a4_u16_src, a4_u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
                  b_FFFF, b_FFFF, a4_u32_src, a4_u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offsets, dst_phv
    p->set_tcam_match(251,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(   0, (uint16_t)0x0000, (uint8_t)0x00, (uint8_t)0x00),  // value
                      p->make_tcam_entry(0x00, (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask

    p->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
    p->set_channel(0, true, 0);

    // Dump out parser state
    p->print();


    // Now define some packets to lookup
    //                    <   DA           SA      ET I4 C LN  ID  OF  T P CK   SRC     DST     SP  DP>
    //                    <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST   ><SP><DP>
    const char *pktstr1 = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000";
    const char *pktstr2 = "080022AABBCC080011DDEEFF080045000100123400001006FFFF0A1122330A4455660688069900000000000000000000000000000000";
    // Now a Packet on heap
    Packet *pkt1 = om->pkt_create(pktstr1);
    Packet *pkt2 = om->pkt_create(pktstr2);

    // Switch on some debug
    om->update_log_flags(ALL,ALL,TYP_PARSER,ALL,ALL,FEW,FEW);
    //om->update_log_flags(ALL,ALL,TYP_PARSER,ALL,ALL,ALL,ALL);

    Phv *phv1 = p->parse(pkt1, 0);
    ASSERT_TRUE(phv1 != NULL);
    EXPECT_EQ(0x1188u, phv1->get_p(Phv::make_word(4,7)));
    EXPECT_EQ(0x1199u, phv1->get_p(Phv::make_word(4,8)));
    EXPECT_EQ(0x11u, phv1->get_p(Phv::make_word(3,1)));
    EXPECT_EQ(0x22u, phv1->get_p(Phv::make_word(3,2)));
    EXPECT_EQ(0x33u, phv1->get_p(Phv::make_word(3,3)));
    Phv *phv2 = p->parse(pkt2, 0);
    EXPECT_EQ(0x0688u, phv2->get_p(Phv::make_word(4,7)));
    EXPECT_EQ(0x0699u, phv2->get_p(Phv::make_word(4,8)));
    EXPECT_EQ(0x11u, phv2->get_p(Phv::make_word(3,1)));
    EXPECT_EQ(0x22u, phv2->get_p(Phv::make_word(3,2)));
    EXPECT_EQ(0x33u, phv2->get_p(Phv::make_word(3,3)));

    // Cleanup
    if (prs_print) RMT_UT_LOG_INFO("Delete pkt1\n");
    om->pkt_delete(pkt1);
    if (prs_print) RMT_UT_LOG_INFO("Delete pkt2\n");
    om->pkt_delete(pkt2);
  }


  TEST(BFN_TEST_NAME(ParseTest),CopyPacketToPhv) {
    GLOBAL_MODEL->Reset();
    // Configure parser from within test
    if (prs_print) RMT_UT_LOG_INFO("test_parse_copy_packet_to_phv()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();

    bool     F           = false;
    uint16_t NoX         = k_phv::kBadPhv;
    uint16_t PHV8_0      = Phv::make_word(2,0);
    uint16_t PHV16_0     = Phv::make_word(4,0);
    uint16_t PHV32_0     = Phv::make_word(0,0);
    uint8_t  u8_00[]     = { 0,0 };
    bool     b_FF[]      = { F,F };
    bool     b_FFFF[]    = { F,F,F,F };

    // Create a parser and initialize with config
    // THIS PARSER CONFIG SHOULD PUT first 64 bytes of packet into:
    // a) 64 8-bit PHV words
    // b) 32 16-bit PHV words
    // c) 16 32-bit PHV words

    Parser *p = om->parser_get(0,0)->ingress();
    ASSERT_TRUE(p != NULL);
    p->reset();
    p->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
    p->set_channel(0, true, 0);
    p->set_start_state(0, 255);
    // Setup a priority map that does nothing
    p->set_identity_priority_map();

    // Setup 16 entries 255..240 as 16 states 255..240
    // Each state extracts 4B 3x times, then shifts buffer 4B, then repeats
    for (uint16_t i = 0; i < 16; i++) {
      bool done = (i == 15);
      int state = 255 - i;
      // Extract 4x1B to 4xPHV8, 2x2B to 2xPHV16, 1x4B to 1xPHV32
      uint8_t  u8_src[]  = { 0, 1, 2, 3};
      uint16_t u8_dst[]  = { static_cast<uint16_t>( (i*4)+0+ PHV8_0 ), static_cast<uint16_t>( (i*4)+1+ PHV8_0 ),
                             static_cast<uint16_t>( (i*4)+2+ PHV8_0 ), static_cast<uint16_t>( (i*4)+3+ PHV8_0 ) };
      uint8_t  u16_src[] = { 0, 2, 0, 0};
      uint16_t u16_dst[] = { static_cast<uint16_t>( (i*2)+0+PHV16_0 ), static_cast<uint16_t>( (i*2)+1+PHV16_0 ), NoX, NoX };
      uint8_t  u32_src[] = { 0, 0, 0, 0};
      uint16_t u32_dst[] = { static_cast<uint16_t>( i+PHV32_0 ), NoX, NoX, NoX};

      p->set_tcam_match(state,
                        p->make_tcam_entry(state, (uint16_t)0, (uint8_t)0, (uint8_t)0),  // val
                        p->make_tcam_entry( 0xFF, (uint16_t)0, (uint8_t)0, (uint8_t)0)); // mask
      p->set_early_action(state,
                          0, F, F,        // Counter load src, LD_SRC, LOAD
                          done, 4,        // done, shift_amount == 4B
                          0,0,0,          // field8_1 off, field8_0 off, field16 off   ***UNUSED***
                          F,F,F,          // field8_1 LOAD, field8_0 LOAD, field16 LOAD ***NONE***
                          0xFF, state-1); // next_state_mask, next_state == STATE AFTER
      p->set_action(state,
                    F, 0, b_FF, u8_00,                 // adj RESET, adj_inc, csum ENABLE, csum_addr
                    b_FFFF, b_FFFF, u8_src,  u8_dst,   // xtrac8:  SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv
                    b_FFFF, b_FFFF, u16_src, u16_dst,  // xtrac16: SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv
                    b_FFFF, b_FFFF, u32_src, u32_dst); // xtrac32: SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv
    }

    // Now define a packet to lookup
    const char *pktstr1 =
        "A0A1A2A3A4A5A6A7B0B1B2B3B4B5B6B7C0C1C2C3C4C5C6C7D0D1D2D3D4D5D6D7" \
        "E0E1E2E3E4E5E6E7F0F1F2F3F4F5F6F790919293949596978081828384858687";
    // Create a Packet on heap
    Packet *pkt1 = om->pkt_create(pktstr1);
    // Parse using above config
    Phv *phv1 = p->parse(pkt1, 0);
    ASSERT_TRUE(phv1 != NULL);

    // Check out ALL 8-bit words - should be 0xA0 0xA1 etc
    EXPECT_EQ(0xA0u, phv1->get_p(Phv::make_word(2,0)));
    EXPECT_EQ(0xA1u, phv1->get_p(Phv::make_word(2,1)));
    EXPECT_EQ(0xA2u, phv1->get_p(Phv::make_word(2,2)));
    EXPECT_EQ(0xA3u, phv1->get_p(Phv::make_word(2,3)));
    EXPECT_EQ(0xA4u, phv1->get_p(Phv::make_word(2,4)));
    EXPECT_EQ(0xA5u, phv1->get_p(Phv::make_word(2,5)));
    EXPECT_EQ(0xA6u, phv1->get_p(Phv::make_word(2,6)));
    EXPECT_EQ(0xA7u, phv1->get_p(Phv::make_word(2,7)));
    EXPECT_EQ(0xB0u, phv1->get_p(Phv::make_word(2,8)));
    EXPECT_EQ(0xB1u, phv1->get_p(Phv::make_word(2,9)));
    EXPECT_EQ(0xB2u, phv1->get_p(Phv::make_word(2,10)));
    EXPECT_EQ(0xB3u, phv1->get_p(Phv::make_word(2,11)));
    EXPECT_EQ(0xB4u, phv1->get_p(Phv::make_word(2,12)));
    EXPECT_EQ(0xB5u, phv1->get_p(Phv::make_word(2,13)));
    EXPECT_EQ(0xB6u, phv1->get_p(Phv::make_word(2,14)));
    EXPECT_EQ(0xB7u, phv1->get_p(Phv::make_word(2,15)));
    EXPECT_EQ(0xC0u, phv1->get_p(Phv::make_word(2,16)));
    EXPECT_EQ(0xC1u, phv1->get_p(Phv::make_word(2,17)));
    EXPECT_EQ(0xC2u, phv1->get_p(Phv::make_word(2,18)));
    EXPECT_EQ(0xC3u, phv1->get_p(Phv::make_word(2,19)));
    EXPECT_EQ(0xC4u, phv1->get_p(Phv::make_word(2,20)));
    EXPECT_EQ(0xC5u, phv1->get_p(Phv::make_word(2,21)));
    EXPECT_EQ(0xC6u, phv1->get_p(Phv::make_word(2,22)));
    EXPECT_EQ(0xC7u, phv1->get_p(Phv::make_word(2,23)));
    EXPECT_EQ(0xD0u, phv1->get_p(Phv::make_word(2,24)));
    EXPECT_EQ(0xD1u, phv1->get_p(Phv::make_word(2,25)));
    EXPECT_EQ(0xD2u, phv1->get_p(Phv::make_word(2,26)));
    EXPECT_EQ(0xD3u, phv1->get_p(Phv::make_word(2,27)));
    EXPECT_EQ(0xD4u, phv1->get_p(Phv::make_word(2,28)));
    EXPECT_EQ(0xD5u, phv1->get_p(Phv::make_word(2,29)));
    EXPECT_EQ(0xD6u, phv1->get_p(Phv::make_word(2,30)));
    EXPECT_EQ(0xD7u, phv1->get_p(Phv::make_word(2,31)));
    EXPECT_EQ(0xE0u, phv1->get_p(Phv::make_word(3,0)));
    EXPECT_EQ(0xE1u, phv1->get_p(Phv::make_word(3,1)));
    EXPECT_EQ(0xE2u, phv1->get_p(Phv::make_word(3,2)));
    EXPECT_EQ(0xE3u, phv1->get_p(Phv::make_word(3,3)));
    EXPECT_EQ(0xE4u, phv1->get_p(Phv::make_word(3,4)));
    EXPECT_EQ(0xE5u, phv1->get_p(Phv::make_word(3,5)));
    EXPECT_EQ(0xE6u, phv1->get_p(Phv::make_word(3,6)));
    EXPECT_EQ(0xE7u, phv1->get_p(Phv::make_word(3,7)));
    EXPECT_EQ(0xF0u, phv1->get_p(Phv::make_word(3,8)));
    EXPECT_EQ(0xF1u, phv1->get_p(Phv::make_word(3,9)));
    EXPECT_EQ(0xF2u, phv1->get_p(Phv::make_word(3,10)));
    EXPECT_EQ(0xF3u, phv1->get_p(Phv::make_word(3,11)));
    EXPECT_EQ(0xF4u, phv1->get_p(Phv::make_word(3,12)));
    EXPECT_EQ(0xF5u, phv1->get_p(Phv::make_word(3,13)));
    EXPECT_EQ(0xF6u, phv1->get_p(Phv::make_word(3,14)));
    EXPECT_EQ(0xF7u, phv1->get_p(Phv::make_word(3,15)));
    EXPECT_EQ(0x90u, phv1->get_p(Phv::make_word(3,16)));
    EXPECT_EQ(0x91u, phv1->get_p(Phv::make_word(3,17)));
    EXPECT_EQ(0x92u, phv1->get_p(Phv::make_word(3,18)));
    EXPECT_EQ(0x93u, phv1->get_p(Phv::make_word(3,19)));
    EXPECT_EQ(0x94u, phv1->get_p(Phv::make_word(3,20)));
    EXPECT_EQ(0x95u, phv1->get_p(Phv::make_word(3,21)));
    EXPECT_EQ(0x96u, phv1->get_p(Phv::make_word(3,22)));
    EXPECT_EQ(0x97u, phv1->get_p(Phv::make_word(3,23)));
    EXPECT_EQ(0x80u, phv1->get_p(Phv::make_word(3,24)));
    EXPECT_EQ(0x81u, phv1->get_p(Phv::make_word(3,25)));
    EXPECT_EQ(0x82u, phv1->get_p(Phv::make_word(3,26)));
    EXPECT_EQ(0x83u, phv1->get_p(Phv::make_word(3,27)));
    EXPECT_EQ(0x84u, phv1->get_p(Phv::make_word(3,28)));
    EXPECT_EQ(0x85u, phv1->get_p(Phv::make_word(3,29)));
    EXPECT_EQ(0x86u, phv1->get_p(Phv::make_word(3,30)));
    EXPECT_EQ(0x87u, phv1->get_p(Phv::make_word(3,31)));

    // Check out 32x 16-bit words - should be 0xA0A1 0xA2A3 etc
    EXPECT_EQ(0xA0A1u, phv1->get_p(Phv::make_word(4,0)));
    EXPECT_EQ(0xA2A3u, phv1->get_p(Phv::make_word(4,1)));
    EXPECT_EQ(0xA4A5u, phv1->get_p(Phv::make_word(4,2)));
    EXPECT_EQ(0xA6A7u, phv1->get_p(Phv::make_word(4,3)));
    EXPECT_EQ(0xB0B1u, phv1->get_p(Phv::make_word(4,4)));
    EXPECT_EQ(0xB2B3u, phv1->get_p(Phv::make_word(4,5)));
    EXPECT_EQ(0xB4B5u, phv1->get_p(Phv::make_word(4,6)));
    EXPECT_EQ(0xB6B7u, phv1->get_p(Phv::make_word(4,7)));
    EXPECT_EQ(0xC0C1u, phv1->get_p(Phv::make_word(4,8)));
    EXPECT_EQ(0xC2C3u, phv1->get_p(Phv::make_word(4,9)));
    EXPECT_EQ(0xC4C5u, phv1->get_p(Phv::make_word(4,10)));
    EXPECT_EQ(0xC6C7u, phv1->get_p(Phv::make_word(4,11)));
    EXPECT_EQ(0xD0D1u, phv1->get_p(Phv::make_word(4,12)));
    EXPECT_EQ(0xD2D3u, phv1->get_p(Phv::make_word(4,13)));
    EXPECT_EQ(0xD4D5u, phv1->get_p(Phv::make_word(4,14)));
    EXPECT_EQ(0xD6D7u, phv1->get_p(Phv::make_word(4,15)));
    EXPECT_EQ(0xE0E1u, phv1->get_p(Phv::make_word(4,16)));
    EXPECT_EQ(0xE2E3u, phv1->get_p(Phv::make_word(4,17)));
    EXPECT_EQ(0xE4E5u, phv1->get_p(Phv::make_word(4,18)));
    EXPECT_EQ(0xE6E7u, phv1->get_p(Phv::make_word(4,19)));
    EXPECT_EQ(0xF0F1u, phv1->get_p(Phv::make_word(4,20)));
    EXPECT_EQ(0xF2F3u, phv1->get_p(Phv::make_word(4,21)));
    EXPECT_EQ(0xF4F5u, phv1->get_p(Phv::make_word(4,22)));
    EXPECT_EQ(0xF6F7u, phv1->get_p(Phv::make_word(4,23)));
    EXPECT_EQ(0x9091u, phv1->get_p(Phv::make_word(4,24)));
    EXPECT_EQ(0x9293u, phv1->get_p(Phv::make_word(4,25)));
    EXPECT_EQ(0x9495u, phv1->get_p(Phv::make_word(4,26)));
    EXPECT_EQ(0x9697u, phv1->get_p(Phv::make_word(4,27)));
    EXPECT_EQ(0x8081u, phv1->get_p(Phv::make_word(4,28)));
    EXPECT_EQ(0x8283u, phv1->get_p(Phv::make_word(4,29)));
    EXPECT_EQ(0x8485u, phv1->get_p(Phv::make_word(4,30)));
    EXPECT_EQ(0x8687u, phv1->get_p(Phv::make_word(4,31)));
    // Check remaining 16-bit words (groups 5,6) are 0
    for (int i = 5; i <= 6; i++) {
      for (int j = 0; j <= 31; j++) {
        EXPECT_EQ(0u, phv1->get_p(Phv::make_word(i,j)));
      }
    }

    // Check out 16x 32-bit words - should be 0xA0A1A2A3 0xA4A5A6A7 etc
    EXPECT_EQ(0xA0A1A2A3u, phv1->get_p(Phv::make_word(0,0)));
    EXPECT_EQ(0xA4A5A6A7u, phv1->get_p(Phv::make_word(0,1)));
    EXPECT_EQ(0xB0B1B2B3u, phv1->get_p(Phv::make_word(0,2)));
    EXPECT_EQ(0xB4B5B6B7u, phv1->get_p(Phv::make_word(0,3)));
    EXPECT_EQ(0xC0C1C2C3u, phv1->get_p(Phv::make_word(0,4)));
    EXPECT_EQ(0xC4C5C6C7u, phv1->get_p(Phv::make_word(0,5)));
    EXPECT_EQ(0xD0D1D2D3u, phv1->get_p(Phv::make_word(0,6)));
    EXPECT_EQ(0xD4D5D6D7u, phv1->get_p(Phv::make_word(0,7)));
    EXPECT_EQ(0xE0E1E2E3u, phv1->get_p(Phv::make_word(0,8)));
    EXPECT_EQ(0xE4E5E6E7u, phv1->get_p(Phv::make_word(0,9)));
    EXPECT_EQ(0xF0F1F2F3u, phv1->get_p(Phv::make_word(0,10)));
    EXPECT_EQ(0xF4F5F6F7u, phv1->get_p(Phv::make_word(0,11)));
    EXPECT_EQ(0x90919293u, phv1->get_p(Phv::make_word(0,12)));
    EXPECT_EQ(0x94959697u, phv1->get_p(Phv::make_word(0,13)));
    EXPECT_EQ(0x80818283u, phv1->get_p(Phv::make_word(0,14)));
    EXPECT_EQ(0x84858687u, phv1->get_p(Phv::make_word(0,15)));
    // Check remaining 32-bit words (group 0 16-31 group1 0-31) are 0
    for (int i = 0; i <= 1; i++) {
      for (int j = 0; j <= 31; j++) {
        if (  ( (i == 0) && (j >= 16) )  ||  (i == 1)  ) {
            EXPECT_EQ(0u, phv1->get_p(Phv::make_word(i,j)));
        }
      }
    }

    //phv1->print_p("DUMP PHV1", prs_printf);

    // Cleanup
    if (prs_print) RMT_UT_LOG_INFO("Delete pkt1\n");
    om->pkt_delete(pkt1);
  }




  int8_t parser_op_shift(int8_t val, int shft) {
    return (val >> shft);
  }
  int8_t parser_op_rotate(int8_t val, int rot) {
    uint8_t mask = (1<<rot)-1;
    int8_t v1 = ((val & mask) << (8-rot)) | (val >> (rot));
    int8_t v2 = ((val) << (8-rot)) | (val >> (rot));
    EXPECT_EQ(v1,v2);
    return v2;
  }
  int8_t parser_op_urotate(int8_t val, int rot) {
    uint8_t uval = (uint8_t)val;
    uint8_t mask = (1<<rot)-1;
    int8_t v1 = ((val & mask) << (8-rot)) | (val >> (rot));
    int8_t v2 = ((val) << (8-rot)) | (val >> (rot));
    EXPECT_EQ(v1,v2);
    int8_t v3 = (int8_t)((uval & mask) << (8-rot)) | (uval >> (rot));
    return v3;
  }
  TEST(BFN_TEST_NAME(ParseTest),ParserOps) {
    int shft=3;
    for (int8_t i = 0; i < 64; i++) {
      int8_t shift_plus_i = parser_op_shift(i, shft);
      int8_t shift_minus_i = parser_op_shift(-i, shft);
      int8_t rot_plus_i = parser_op_rotate(i, shft);
      int8_t rot_minus_i = parser_op_rotate(-i, shft);
      int8_t urot_plus_i = parser_op_urotate(i, shft);
      int8_t urot_minus_i = parser_op_urotate(-i, shft);
      printf("\t i=%d \tshift(%d,%d)=%d \trot(%d,%d)=%d,%d "
             "\t-i=%d \tshift(%d,%d)=%d \trot(%d,%d)=%d,%d\n",
              i,  i, shft, shift_plus_i, i, shft, rot_plus_i, urot_plus_i,
              -i,  -i, shft, shift_minus_i, -i, shft, rot_minus_i, urot_minus_i);
    }
  }


  TEST_F(MCN_CONCAT(BFN_TEST_NAME(ParseTest), UsingConfigBase), CounterRamStress) {
    // Loop many times verifying that CounterRam ops behave as expected on all chips

    const int      num_iterations  = 50000;
    const int      debug_mod       = 100;
    const uint64_t root_rand       = UINT64_C(0xDAB0D1B0D0B0DEB0);
    const uint64_t debug_rand      = UINT64_C(0); // 0==>use root_rand
    const bool     DEBUG_ITER      = true;
    const bool     CHECK_ERRS      = false;

    parser_->set_log_flags(UINT64_C(0));

    // Loop doing many iterations
    //
    uint32_t err_cnt = 0u;
    uint64_t fail_rand = UINT64_C(0);
    uint64_t iter_rand = root_rand;

    for (int iteration = 0; iteration < num_iterations; iteration++) {
      bool dmod = ((iteration % debug_mod) == 0);

      parser_->counter_reset();

      iter_rand = tu_->mmix_rand64(iter_rand); // Maybe force iter_rand
      if ((iteration == 0) && (debug_rand != UINT64_C(0))) iter_rand = debug_rand;
      if (DEBUG_ITER && dmod) printf("ITER[%4d] Seed=%016" PRIx64 " Errors=%d "
                                     "[FirstFailSeed=%016" PRIx64 "]\n",
                                     iteration, iter_rand, err_cnt, fail_rand);
      uint64_t loop_rand = iter_rand;
      uint64_t my_cntr_ram[16] = { UINT64_C(0) };
      uint8_t  ea_ram_to_cntr_ram_map[256] = { 0 };


      // Setup 16 random CounterInit entries...
      for (int ci = 0; ci < RmtDefs::kParserCounterInitEntries; ci++) {
        loop_rand = tu_->mmix_rand64(loop_rand);

        // Stash rand vals for homebrew impl CntrRam
        my_cntr_ram[ci] = loop_rand;

        // Program real CntrRam with derived random add/maskW/rot/max/src
        uint8_t add   = static_cast<uint8_t>((loop_rand >>  8) & 0xFF);
        uint8_t maskw = static_cast<uint8_t>((loop_rand >> 16) & 0x07);
        uint8_t rot   = static_cast<uint8_t>((loop_rand >> 24) & 0x07);
        uint8_t max   = static_cast<uint8_t>((loop_rand >> 32) & 0xFF);
        uint8_t src   = static_cast<uint8_t>((loop_rand >> 40) & 0x03);
        parser_->set_counter_init(ci, add, maskw, rot, max, src, true);
      }

      // Make EARam[0] be a simple LoadImmediate to clear down pending cntr state
      parser_->set_counter_ctr_op(0, Parser::kCounter2LoadImmediate);
      parser_->set_counter_load_imm(0, 0);

      // Setup 255 random EARam entries that load from CntrRam
      for (int eai = 1; eai < RmtDefs::kParserStates; eai++) {
        loop_rand = tu_->mmix_rand64(loop_rand);

        // Stash rand vals for homebrew impl EARam (just need CntrRam index)
        ea_ram_to_cntr_ram_map[eai] = static_cast<uint8_t>(loop_rand % 16);

        // Program real EARam with LoadFromCntrRam OPs
        // (the ctr_op will be mapped appropriately on each chip type)
        parser_->set_counter_ctr_op(eai, Parser::kCounter2LoadFromCntrRam);
        parser_->set_counter_load_addr(eai, static_cast<uint8_t>(loop_rand % 16));
      }


      // Now do 255 calls to counter_handle on parser and on homebrew impl
      // (using each randomly programmed entry above)
      for (int eai = 1; eai < RmtDefs::kParserStates; eai++) {
        loop_rand = tu_->mmix_rand64(loop_rand);

        uint8_t f8[4] = { static_cast<uint8_t>((loop_rand >>  0) & 0xFF),
                          static_cast<uint8_t>((loop_rand >>  8) & 0xFF),
                          static_cast<uint8_t>((loop_rand >> 16) & 0xFF),
                          static_cast<uint8_t>((loop_rand >> 24) & 0xFF) };

        // Real parser implementation
        int    p_dummy_shift = 0;
        int8_t p_cntr = 0, p_next = 0;
        bool   p_pnd = false, p_err = false;
        parser_->counter_handle(eai, f8[3], f8[2], f8[1], f8[0], &p_dummy_shift);
        // And check state of counter_ counter_next_ etc
        parser_->counter_info_get(&p_cntr, &p_next, &p_pnd, &p_err);
        if (p_pnd) {
          EXPECT_EQ(127, p_cntr);
          // Note we need counter_next value as CntrRam takes an extra cycle
          p_cntr = p_next;
          // Call parser again to clear down pending cntr state (use EARam[0])
          parser_->counter_handle(0, 0, 0, 0, 0, &p_dummy_shift);
        }

        // Our homebrew implementation
        int      ci = ea_ram_to_cntr_ram_map[eai];
        uint64_t cntr_ram_rand = my_cntr_ram[ci];
        bool     err   = false;
        int8_t   cntr  = 0;
        int8_t   add   = static_cast<int8_t> ((cntr_ram_rand >>  8) & 0xFF);
        uint8_t  maskw = static_cast<uint8_t>((cntr_ram_rand >> 16) & 0x07);
        uint8_t  rot   = static_cast<uint8_t>((cntr_ram_rand >> 24) & 0x07);
        uint8_t  max   = static_cast<uint8_t>((cntr_ram_rand >> 32) & 0xFF);
        uint8_t  src   = static_cast<uint8_t>((cntr_ram_rand >> 40) & 0x03);
        uint8_t  mask  = static_cast<uint8_t>((1u << (maskw+1)) - 1);
        uint8_t  pkt_imm_val = f8[src];
        if (RmtObject::is_chip1()) {
          // Early rot/mask on WIP
          pkt_imm_val = ( (pkt_imm_val >> (rot)) | (pkt_imm_val << (8-rot)) ) & mask;
        }
        if (pkt_imm_val > max) err = true;
        if (!err) {
          if (RmtObject::is_tofinoXX() || RmtObject::is_jbayXX()) {
            // Late rot/mask on Tofino/JBay
            pkt_imm_val = ( (pkt_imm_val >> (rot)) | (pkt_imm_val << (8-rot)) ) & mask;
          }
          cntr = static_cast<int8_t>(pkt_imm_val + add);
        }

        // Check results are the same
        bool mismatch = ((p_cntr != cntr) || (p_err != err));
        if ((mismatch) || (iter_rand == debug_rand)) {
          if ((mismatch) && (fail_rand == UINT64_C(0))) fail_rand = iter_rand;
          printf("ITER[%4d] Seed=%016" PRIx64 " nCycles=%2d "
                 "Prsr_Ctr=%d,My_Ctr0=%d  Prsr_Err=%c,My_Err=%c  "
                 "[FirstFailSeed=%016" PRIx64 "]\n",
                 iteration, iter_rand, eai, p_cntr, cntr,
                 p_err?'T':'F', err?'T':'F', fail_rand);
          if (CHECK_ERRS) {
            EXPECT_EQ(p_cntr, cntr);
            EXPECT_EQ(p_err, err);
          }
          else if (mismatch) {
            err_cnt++;
            // Call again so we can debug
            parser_->counter_handle(eai, f8[3], f8[2], f8[1], f8[0], &p_dummy_shift);
            break;
          }
        }
      }

      EXPECT_EQ(0u, err_cnt);

    }  // for (int iteration = 0; iteration < num_iterations; iteration++)
  }



// ******************** TOFINO only tests ********************

#if MCN_TEST(MODEL_CHIP_NAMESPACE,tofino) || MCN_TEST(MODEL_CHIP_NAMESPACE,tofinoB0)

  BitVector<44> parse_local_make_tcam_entry(bool ver1, bool ver0,
                                            bool cnt_eq_0, bool cnt_lt_0,
                                            uint8_t state, uint16_t v16,
                                            uint8_t v8_1, uint8_t v8_0) {
      uint64_t w = ((uint64_t)state << 32) | ((uint64_t)v16 << 0);
      w |= ((uint64_t)v8_1 << 24) | ((uint64_t)v8_0 << 16);
      if (ver1)  w |= (UINT64_C(1) << 43);
      if (ver0)  w |= (UINT64_C(1) << 42);
      if (cnt_eq_0) w |= (UINT64_C(1) << 40);
      if (cnt_lt_0) w |= (UINT64_C(1) << 41);
      return BitVector<44>(std::array<uint64_t,1>({{w}}));
  }

  TEST(BFN_TEST_NAME(ParseTest),TofManualSetupTcam1) {
    GLOBAL_MODEL->Reset();

    // Get ourself a Parser so we can call make_tcam_entry
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    Parser *parser = om->parser_get(0,0)->ingress();
    ASSERT_TRUE(parser != NULL);

    // Create row0 (word0/word1) object in chip0,pipe0,parser0,ingress
    int           row=0;
        memory_classes::PrsrMlTcamRow::PipeAddrmapEnum enum_iprsr =
            memory_classes::PrsrMlTcamRow::PipeAddrmapEnum::kIPrsr;
    memory_classes::PrsrMlTcamRow::PrsrMemMainRspecEnum enum_w0 =
        memory_classes::PrsrMlTcamRow::PrsrMemMainRspecEnum::kMlTcamRowWord0;
    memory_classes::PrsrMlTcamRow::PrsrMemMainRspecEnum enum_w1 =
        memory_classes::PrsrMlTcamRow::PrsrMemMainRspecEnum::kMlTcamRowWord1;
    memory_classes::PrsrMlTcamRow row0_w0(0, 0, enum_iprsr, 0, enum_w0, row);
    memory_classes::PrsrMlTcamRow row0_w1(0, 0, enum_iprsr, 0, enum_w1, row);
    uint64_t zero = UINT64_C(0);
    uint64_t pad = UINT64_C(0xF8F7F6F5F4F3F2F1);
    uint64_t w1 = UINT64_C(0x0807060504030201);
    uint64_t w0 = ~w1;

    // Fill in with initial vals
    row0_w0.write(zero, w0, pad, UINT64_C(0));
    row0_w1.write(zero, w1, pad, UINT64_C(0));

    uint16_t v16;
    uint8_t v8_0, v8_1, state;
    bool cnt_eq_0, cnt_lt_0, ver0, ver1;

    // Now fish out fields we've written to row0_w0 and make a BitVector
    v16 = row0_w0.lookup_16();
    v8_0 = row0_w0.lookup_8(0);
    v8_1 = row0_w0.lookup_8(1);
    state = row0_w0.curr_state();
    cnt_eq_0 = ((row0_w0.ctr_zero() & 0x1) == 0x1);
    cnt_lt_0 = ((row0_w0.ctr_neg() & 0x1) == 0x1);
    ver0 = ((row0_w0.ver_0() & 0x1) == 0x1);
    ver1 = ((row0_w0.ver_1() & 0x1) == 0x1);
    BitVector<44> bv_w0 = parser->make_tcam_entry(ver1, ver0, cnt_eq_0, cnt_lt_0,
                                                  state, v16, v8_1, v8_0);

    // Likewise for row0_w1
    v16 = row0_w1.lookup_16();
    v8_0 = row0_w1.lookup_8(0);
    v8_1 = row0_w1.lookup_8(1);
    state = row0_w1.curr_state();
    cnt_eq_0 = ((row0_w1.ctr_zero() & 0x1) == 0x1);
    cnt_lt_0 = ((row0_w1.ctr_neg() & 0x1) == 0x1);
    ver0 = ((row0_w1.ver_0() & 0x1) == 0x1);
    ver1 = ((row0_w1.ver_1() & 0x1) == 0x1);
    BitVector<44> bv_w1 = parser->make_tcam_entry(ver1, ver0, cnt_eq_0, cnt_lt_0,
                                                  state, v16, v8_1, v8_0);

    // Make a new BV using local make_tcam_entry
    BitVector<44> my_w1 = parse_local_make_tcam_entry(ver1, ver0, cnt_eq_0, cnt_lt_0,
                                                      state, v16, v8_1, v8_0);

    // Push both BitVectors into a Tcam3 index=0
    Tcam3<16,44> tcam;
    tcam.set_word0_word1(0, bv_w0, bv_w1);
    // And get new BitVectors out
    BitVector<44> tcam_w0;
    BitVector<44> tcam_w1;
    tcam.get(0, &tcam_w0, &tcam_w1);

    // Then print out all BitVectors and see what we have
    //printf("w1=0x%016" PRIx64 "\n", w1);
    //printf("w1 vals v16=0x%04x v8_0=0x%02x v8_1=0x%02x\n", v16, v8_0, v8_1);
    //printf("w1 vals state=%d cnt_eq_0=%d cnt_lt_0=%d ver0=%d ver1=%d\n",
    //       state, cnt_eq_0, cnt_lt_0, ver0, ver1);
    //printf("Initial BVs w1: ");
    //bv_w1.print_with_printf();
    //printf("Initial BVs w0: ");
    //bv_w0.print_with_printf();

    ASSERT_TRUE(bv_w0.equals(tcam_w0));
    //printf("Tcam BVs w0   : ");
    //tcam_w0.print_with_printf();
    ASSERT_TRUE(bv_w1.equals(tcam_w1));
    //printf("Tcam BVs w1   : ");
    //tcam_w1.print_with_printf();

    // Would be ideal if we could arrange things so
    // bits in BitVector are same order as initial word input
    //printf("\n");
    //printf("w1=0x%016" PRIx64 "\n", w1);
    //printf("w1 vals v16=0x%04x v8_0=0x%02x v8_1=0x%02x\n", v16, v8_0, v8_1);
    //printf("w1 vals state=%d cnt_eq_0=%d cnt_lt_0=%d ver0=%d ver1=%d\n",
    //       state, cnt_eq_0, cnt_lt_0, ver0, ver1);
    //printf("Initial BVs w1: ");
    //bv_w1.print_with_printf();
    //printf("MY BV w0      : ");
    //my_w1.print_with_printf();
    //printf("\n");

  }

#define SetActionArray(a, b0, b1, b2, b3) { a[0] = b0; a[1] = b1; a[2] = b2; a[3] = b3; }

  TEST(BFN_TEST_NAME(ParseTest),NarrowToWide) {
    GLOBAL_MODEL->Reset();
    // Configure parser from within test
    if (prs_print) RMT_UT_LOG_INFO("test_narrow_to_wide_constraints\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();

    bool     F           = false;
    uint16_t NoX         = k_phv::kBadPhv;
    uint16_t PHV8_0      = Phv::make_word(2,0);
    uint16_t PHV16_0     = Phv::make_word(4,0);
    uint16_t PHV32_0     = Phv::make_word(0,0);
    uint8_t  u8_00[]     = { 0,0 };
    bool     b_FF[]      = { F,F };
    bool     b_FFFF[]    = { F,F,F,F };

    uint8_t  u8_src[]   = {0, 0, 0, 0};
    uint16_t u8_dst[]   = {NoX, NoX, NoX, NoX };
    uint8_t  u16_src[]  = {0, 0, 0, 0};
    uint16_t u16_dst[]  = {NoX, NoX, NoX, NoX };
    uint8_t  u32_src[]  = {0, 0, 0, 0};
    uint16_t u32_dst[]  = {NoX, NoX, NoX, NoX };

    bool done;
    int state;

    // Create a parser and initialize with config
    // Testing narrow-to-wide parser constraints

    Parser *p = om->parser_get(0,0)->ingress();
    ASSERT_TRUE(p != NULL);
    p->reset();
    p->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
    p->set_channel(0, true, 0);
    p->set_start_state(0, 255);
    // Setup a priority map that does nothing
    p->set_identity_priority_map();

    // Define a packet to lookup
    const char *pktstr1 =
        "A0A1A2A3A4A5A6A7B0B1B2B3B4B5B6B7C0C1C2C3C4C5C6C7D0D1D2D3D4D5D6D7" \
        "E0E1E2E3E4E5E6E7F0F1F2F3F4F5F6F790919293949596978081828384858687";
    // Create a Packet on heap
    Packet *pkt1 = om->pkt_create(pktstr1);

    // Test of narrow-to-wide (should pass):
    for (uint16_t i = 0; i < 5; i++) {
      done = (i == 4);
      state = 255 - i;
      switch (i) {
      case 0:
        // State 1 (index=255):
        // 4 x 32b extracts
        // 4 x 16b extracts
        // 4 x 8b extracts
        SetActionArray(u8_src, 0, 1, 2, 3);
        SetActionArray(u8_dst, static_cast<uint16_t>(0 + PHV8_0), static_cast<uint16_t>(1 + PHV8_0),
                               static_cast<uint16_t>(2 + PHV8_0), static_cast<uint16_t>(3 + PHV8_0));
        SetActionArray(u16_src, 0, 2, 4, 6);
        SetActionArray(u16_dst, static_cast<uint16_t>(0 + PHV16_0), static_cast<uint16_t>(1 + PHV16_0),
                                static_cast<uint16_t>(2 + PHV16_0), static_cast<uint16_t>(3 + PHV16_0));
        SetActionArray(u32_src, 0, 4, 8, 12);
        SetActionArray(u32_dst, static_cast<uint16_t>(0 + PHV32_0), static_cast<uint16_t>(1 + PHV32_0),
                                static_cast<uint16_t>(2 + PHV32_0), static_cast<uint16_t>(3 + PHV32_0));
        break;
      case 1:
        SetActionArray(u8_src, 0, 0, 0, 0);
        SetActionArray(u8_dst, NoX, NoX, NoX, NoX);
        SetActionArray(u16_src, 8, 10, 0, 0);
        SetActionArray(u16_dst, static_cast<uint16_t>(4 + PHV32_0), static_cast<uint16_t>(4 + PHV32_0), NoX, NoX); // 2x16b->1x32b
        SetActionArray(u32_src, 0, 0, 0, 0);
        SetActionArray(u32_dst, NoX, NoX, NoX, NoX);
        break;
      case 2:
        SetActionArray(u8_src, 6, 7, 4, 5);     // Note: The odd alignment here is due to HW arbiter implementation
        SetActionArray(u8_dst, static_cast<uint16_t>(5 + PHV32_0), static_cast<uint16_t>(5 + PHV32_0),
                               static_cast<uint16_t>(5 + PHV32_0), static_cast<uint16_t>(5 + PHV32_0));    // 4x8b->1x32b
        SetActionArray(u16_src, 0, 0, 0, 0);
        SetActionArray(u16_dst, NoX, NoX, NoX, NoX);
        SetActionArray(u32_src, 0, 0, 0, 0);
        SetActionArray(u32_dst, NoX, NoX, NoX, NoX);
        break;
      case 3:
        SetActionArray(u8_src, 4, 5, 6, 7);
        SetActionArray(u8_dst, static_cast<uint16_t>(6 + PHV16_0), static_cast<uint16_t>(6 + PHV16_0),
                               static_cast<uint16_t>(7 + PHV16_0), static_cast<uint16_t>(7 + PHV16_0));     // 4x8b->2x16b
        SetActionArray(u16_src, 0, 0, 0, 0);
        SetActionArray(u16_dst, NoX, NoX, NoX, NoX);
        SetActionArray(u32_src, 0, 0, 0, 0);
        SetActionArray(u32_dst, NoX, NoX, NoX, NoX);
        break;
      case 4:
        SetActionArray(u8_src, 8, 9, 10, 11);
        SetActionArray(u8_dst, static_cast<uint16_t>(8 + PHV8_0), static_cast<uint16_t>(9 + PHV8_0),
                               static_cast<uint16_t>(10 + PHV8_0), static_cast<uint16_t>(11 + PHV8_0));
        SetActionArray(u16_src, 12, 14, 0, 0);
        SetActionArray(u16_dst, static_cast<uint16_t>(8 + PHV16_0), static_cast<uint16_t>(9 + PHV16_0), NoX, NoX);
        SetActionArray(u32_src, 6, 0, 0, 0);
        SetActionArray(u32_dst, static_cast<uint16_t>(6 + PHV32_0), NoX, NoX, NoX);
        break;
      default:
        break;
      }

      p->set_early_action(state,
                          0, F, F,        // Counter load src, LD_SRC, LOAD
                          done, 0,        // done, shift_amount == 0B
                          0,0,0,          // field8_1 off, field8_0 off, field16 off   ***UNUSED***
                          F,F,F,          // field8_1 LOAD, field8_0 LOAD, field16 LOAD ***NONE***
                          0xFF, state-1); // next_state_mask, next_state == STATE AFTER
      // Set buf_req to 16 since we actually _want_ data in the buffer (needed after Jira XXX fix).
      p->set_buf_req(state, 16);

      p->set_action(state,
                    F, 0, b_FF, u8_00,                       // adj RESET, adj_inc, csum ENABLE, csum_addr
                    b_FFFF, b_FFFF, u8_src,  u8_dst,   // xtrac8:  SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv
                    b_FFFF, b_FFFF, u16_src, u16_dst,  // xtrac16: SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv
                    b_FFFF, b_FFFF, u32_src, u32_dst); // xtrac32: SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv

      p->set_tcam_match(state,                    // Use 0x99 in value field if we don't care
                        p->make_tcam_entry(state, (uint16_t)0x9999, (uint8_t)0x99, (uint8_t)0x99),  // value (state=0)
                        p->make_tcam_entry(0xFF,  (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask
    }

    // Parse using above config
    Phv *phv1 = p->parse(pkt1, 0);
    ASSERT_TRUE(phv1 != NULL);

    // Check out the 8-bit words - should be 0xA0 0xA1 etc
    EXPECT_EQ(0xA0u, phv1->get_p(Phv::make_word(2,0)));
    EXPECT_EQ(0xA1u, phv1->get_p(Phv::make_word(2,1)));
    EXPECT_EQ(0xA2u, phv1->get_p(Phv::make_word(2,2)));
    EXPECT_EQ(0xA3u, phv1->get_p(Phv::make_word(2,3)));
    EXPECT_EQ(0x00u, phv1->get_p(Phv::make_word(2,4)));     // Check the 8b -> 32b is not place here.
    EXPECT_EQ(0x00u, phv1->get_p(Phv::make_word(2,5)));     // Check the 8b -> 32b is not place here.
    EXPECT_EQ(0x00u, phv1->get_p(Phv::make_word(2,6)));     // Check the 8b -> 32b is not place here.
    EXPECT_EQ(0x00u, phv1->get_p(Phv::make_word(2,7)));     // Check the 8b -> 32b is not place here.
    EXPECT_EQ(0xB0u, phv1->get_p(Phv::make_word(2,8)));
    EXPECT_EQ(0xB1u, phv1->get_p(Phv::make_word(2,9)));
    EXPECT_EQ(0xB2u, phv1->get_p(Phv::make_word(2,10)));
    EXPECT_EQ(0xB3u, phv1->get_p(Phv::make_word(2,11)));

    // Check out the 16-bit words - should be 0xA0A1 0xA2A3 etc
    EXPECT_EQ(0xA0A1u, phv1->get_p(Phv::make_word(4,0)));
    EXPECT_EQ(0xA2A3u, phv1->get_p(Phv::make_word(4,1)));
    EXPECT_EQ(0xA4A5u, phv1->get_p(Phv::make_word(4,2)));
    EXPECT_EQ(0xA6A7u, phv1->get_p(Phv::make_word(4,3)));
    EXPECT_EQ(0x0000u, phv1->get_p(Phv::make_word(4,4)));   // Check the 16b -> 32b is not place here.
    EXPECT_EQ(0x0000u, phv1->get_p(Phv::make_word(4,5)));   // Check the 16b -> 32b is not place here.
    EXPECT_EQ(0xA4A5u, phv1->get_p(Phv::make_word(4,6)));   // Check the 8b -> 16b is place here.
    EXPECT_EQ(0xA6A7u, phv1->get_p(Phv::make_word(4,7)));   // Check the 8b -> 16b is place here.
    EXPECT_EQ(0xB4B5u, phv1->get_p(Phv::make_word(4,8)));
    EXPECT_EQ(0xB6B7u, phv1->get_p(Phv::make_word(4,9)));

    // Check out the 32-bit words - should be 0xA0A1A2A3 0xA4A5A6A7 etc
    EXPECT_EQ(0xA0A1A2A3u, phv1->get_p(Phv::make_word(0,0)));
    EXPECT_EQ(0xA4A5A6A7u, phv1->get_p(Phv::make_word(0,1)));
    EXPECT_EQ(0xB0B1B2B3u, phv1->get_p(Phv::make_word(0,2)));
    EXPECT_EQ(0xB4B5B6B7u, phv1->get_p(Phv::make_word(0,3)));
    EXPECT_EQ(0xB0B1B2B3u, phv1->get_p(Phv::make_word(0,4)));   // Check the 16b -> 32b is place here.
    EXPECT_EQ(0xA4A5A6A7u, phv1->get_p(Phv::make_word(0,5)));   // Check the 8b -> 32b is place here.

    // Test of narrow-to-wide constraints (should fail):
    // Test that in every previous state in the packet, this is done:
    // 0, 2, or 4 x 16b extracts
    // 0 or 4 x 8b extracts
    for (uint16_t i = 0; i < 4; i++) {
      done = (i == 3);
      state = 255 - i;
      switch (i) {
      case 0:
        // Break constraint rule by only doing 2x8b extracts.
        SetActionArray(u8_src, 0, 1, 0, 0);
        SetActionArray(u8_dst, static_cast<uint16_t>(0 + PHV8_0), static_cast<uint16_t>(1 + PHV8_0), NoX, NoX );
        SetActionArray(u16_src, 0, 0, 0, 0);
        SetActionArray(u16_dst, NoX, NoX, NoX, NoX);
        SetActionArray(u32_src, 0, 0, 0, 0);
        SetActionArray(u32_dst, NoX, NoX, NoX, NoX);
        break;
      case 1:
        // Extract 4x1B to 4xPHV8, 2x2B to 2xPHV16, 1x4B to 1xPHV32
        SetActionArray(u8_src, 0, 1, 2, 3);
        SetActionArray(u8_dst, static_cast<uint16_t>( (i*4)+0+ PHV8_0 ), static_cast<uint16_t>( (i*4)+1+ PHV8_0 ),
                               static_cast<uint16_t>( (i*4)+2+ PHV8_0 ), static_cast<uint16_t>( (i*4)+3+ PHV8_0 ));
        SetActionArray(u16_src, 0, 2, 0, 0);
        SetActionArray(u16_dst, static_cast<uint16_t>( (i*2)+0+PHV16_0 ), static_cast<uint16_t>( (i*2)+1+PHV16_0 ), NoX, NoX);
        SetActionArray(u32_src, 0, 0, 0, 0);
        SetActionArray(u32_dst, static_cast<uint16_t>( i+PHV32_0 ), NoX, NoX, NoX);
        break;
      case 2:
        break; // Same as 1
      case 3:
        // Now do narrow-to-wide extraction
        SetActionArray(u16_src, 0, 2, 0, 0);
        SetActionArray(u16_dst, static_cast<uint16_t>(4 + PHV32_0), static_cast<uint16_t>(4 + PHV32_0), NoX, NoX); // 2x16b->1x32b
        break;
      default:
        break;
      }

      p->set_early_action(state,
                          0, F, F,        // Counter load src, LD_SRC, LOAD
                          done, 4,        // done, shift_amount == 4B
                          0,0,0,          // field8_1 off, field8_0 off, field16 off   ***UNUSED***
                          F,F,F,          // field8_1 LOAD, field8_0 LOAD, field16 LOAD ***NONE***
                          0xFF, state-1); // next_state_mask, next_state == STATE AFTER

      p->set_action(state,
                    F, 0, b_FF, u8_00,                 // adj RESET, adj_inc, csum ENABLE, csum_addr
                    b_FFFF, b_FFFF, u8_src,  u8_dst,   // xtrac8:  SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv
                    b_FFFF, b_FFFF, u16_src, u16_dst,  // xtrac16: SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv
                    b_FFFF, b_FFFF, u32_src, u32_dst); // xtrac32: SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv

      p->set_tcam_match(state,                    // Use 0x99 in value field if we don't care
                        p->make_tcam_entry(state, (uint16_t)0x9999, (uint8_t)0x99, (uint8_t)0x99),  // value (state=0)
                        p->make_tcam_entry(0xFF,  (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask
    }

    // Parse using above config
    try {
      Phv *phv1 = p->parse(pkt1, 0);
      EXPECT_TRUE(phv1 == NULL);
    }
    catch(...) {
    }

    // Test of Jira XXX (should fail):
    // Test that in the current state in the packet, this is done (when doing n2w extraction):
    // 0, 2, or 4 x 16b extracts
    // 0 or 4 x 8b extracts
    state = 255;
    done = true;
    SetActionArray(u8_src, 0, 1, 0, 0);
    SetActionArray(u8_dst, static_cast<uint16_t>(0 + PHV8_0), static_cast<uint16_t>(1 + PHV8_0), NoX, NoX );
    // Narrow-to-wide extraction
    SetActionArray(u16_src, 0, 2, 0, 0);
    SetActionArray(u16_dst, static_cast<uint16_t>(4 + PHV32_0), static_cast<uint16_t>(4 + PHV32_0), NoX, NoX); // 2x16b->1x32b
    SetActionArray(u32_src, 0, 0, 0, 0);
    SetActionArray(u32_dst, NoX, NoX, NoX, NoX);

    p->set_early_action(state,
                        0, F, F,        // Counter load src, LD_SRC, LOAD
                        done, 4,        // done, shift_amount == 4B
                        0,0,0,          // field8_1 off, field8_0 off, field16 off   ***UNUSED***
                        F,F,F,          // field8_1 LOAD, field8_0 LOAD, field16 LOAD ***NONE***
                        0xFF, state-1); // next_state_mask, next_state == STATE AFTER

    p->set_action(state,
                  F, 0, b_FF, u8_00,                 // adj RESET, adj_inc, csum ENABLE, csum_addr
                  b_FFFF, b_FFFF, u8_src,  u8_dst,   // xtrac8:  SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv
                  b_FFFF, b_FFFF, u16_src, u16_dst,  // xtrac16: SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv
                  b_FFFF, b_FFFF, u32_src, u32_dst); // xtrac32: SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv

    p->set_tcam_match(state,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(state, (uint16_t)0x9999, (uint8_t)0x99, (uint8_t)0x99),  // value (state=0)
                      p->make_tcam_entry(0xFF,  (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask

    // Parse using above config
    try {
      Phv *phv1 = p->parse(pkt1, 0);
      EXPECT_TRUE(phv1 == NULL);
    }
    catch(...) {
    }

    // Test of Jira XXX (should fail):

    // State 1 (index=255):
    // 4 x 32b extracts
    // 4 x 16b extracts
    // 4 x 8b extracts
    state = 255;
    done = false;
    p->set_early_action(state,
                        0, F, F,        // Counter load src, LD_SRC, LOAD
                        done, 0,        // done, shift_amount == 0B
                        0,0,0,          // field8_1 off, field8_0 off, field16 off   ***UNUSED***
                        F,F,F,          // field8_1 LOAD, field8_0 LOAD, field16 LOAD ***NONE***
                        0xFF, state-1); // next_state_mask, next_state == STATE AFTER
    // Set buf_req to 16 since we actually _want_ data in the buffer (needed after Jira XXX fix).
    p->set_buf_req(255, 16);

    uint8_t  a0_u8_src[]  = {0, 1, 2, 3};
    uint16_t a0_u8_dst[]  = {static_cast<uint16_t>(0 + PHV8_0), static_cast<uint16_t>(1 + PHV8_0),
                             static_cast<uint16_t>(2 + PHV8_0), static_cast<uint16_t>(3 + PHV8_0) };
    uint8_t  a0_u16_src[] = {0, 2, 4, 6};
    uint16_t a0_u16_dst[] = {static_cast<uint16_t>(0 + PHV16_0), static_cast<uint16_t>(1 + PHV16_0),
                             static_cast<uint16_t>(2 + PHV16_0), static_cast<uint16_t>(3 + PHV16_0) };
    uint8_t  a0_u32_src[] = {0, 4, 8, 12};
    uint16_t a0_u32_dst[] = {static_cast<uint16_t>(0 + PHV32_0), static_cast<uint16_t>(1 + PHV32_0),
                             static_cast<uint16_t>(2 + PHV32_0), static_cast<uint16_t>(3 + PHV32_0) };

    p->set_action(state,
                  F, 0, b_FF, u8_00,                       // adj RESET, adj_inc, csum ENABLE, csum_addr
                  b_FFFF, b_FFFF, a0_u8_src,  a0_u8_dst,   // xtrac8:  SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv
                  b_FFFF, b_FFFF, a0_u16_src, a0_u16_dst,  // xtrac16: SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv
                  b_FFFF, b_FFFF, a0_u32_src, a0_u32_dst); // xtrac32: SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv

    p->set_tcam_match(state,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(state, (uint16_t)0x9999, (uint8_t)0x99, (uint8_t)0x99),  // value (state=0)
                      p->make_tcam_entry(0xFF,  (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask

    // State 2 (index=254):
    // 2 x 16b extracts implementing a narrow-to-wide 32b extract. (The compiler should use a 32b extractor in this
    // case since they're free...
    state -= 1;

    p->set_early_action(state,
                        0, F, F,        // Counter load src, LD_SRC, LOAD
                        done, 0,        // done, shift_amount == 0B
                        0,0,0,          // field8_1 off, field8_0 off, field16 off   ***UNUSED***
                        F,F,F,          // field8_1 LOAD, field8_0 LOAD, field16 LOAD ***NONE***
                        0xFF, state-1); // next_state_mask, next_state == STATE AFTER
    // Set buf_req to 16 since we actually _want_ data in the buffer (needed after Jira XXX fix).
    p->set_buf_req(254, 16);

    uint8_t  a1_u8_src[]  = {0, 0, 0, 0};
    uint16_t a1_u8_dst[]  = {NoX, NoX, NoX, NoX};
    uint8_t  a1_u16_src[] = {8, 10, 0, 0};
    uint16_t a1_u16_dst[] = {static_cast<uint16_t>(4 + PHV32_0), static_cast<uint16_t>(4 + PHV32_0), NoX, NoX}; // 2x16b->1x32b
    uint8_t  a1_u32_src[] = {0, 0, 0, 0};
    uint16_t a1_u32_dst[] = {NoX, NoX, NoX, NoX};

    p->set_action(state,
                  F, 0, b_FF, u8_00,                       // adj RESET, adj_inc, csum ENABLE, csum_addr
                  b_FFFF, b_FFFF, a1_u8_src,  a1_u8_dst,   // xtrac8:  SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv
                  b_FFFF, b_FFFF, a1_u16_src, a1_u16_dst,  // xtrac16: SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv
                  b_FFFF, b_FFFF, a1_u32_src, a1_u32_dst); // xtrac32: SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv

    p->set_tcam_match(state,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(state,  (uint16_t)0x9999, (uint8_t)0x99, (uint8_t)0x99),  // value (state=0)
                      p->make_tcam_entry(0xFF,   (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask

    // State 3 (index=253):
    // 2 x 8b extracts
    // This is problematic when the three states are accumulated in the FIFO.
    // In this case, we'll end up with the following occupancies:
    //   32b: 4 entries.
    //   16b: 6 entries (4 x regular 16b writes and 2 x 16b-to-32b narrow-to-wide write).
    //    8b: 6 entries.
    state -= 1;
    done = true;
    p->set_early_action(state,
                        0, F, F,        // Counter load src, LD_SRC, LOAD
                        done, 0,        // done, shift_amount == 0B
                        0,0,0,          // field8_1 off, field8_0 off, field16 off   ***UNUSED***
                        F,F,F,          // field8_1 LOAD, field8_0 LOAD, field16 LOAD ***NONE***
                        0xFF, state-1); // next_state_mask, next_state == STATE AFTER
    // Set buf_req to 16 since we actually _want_ data in the buffer (needed after Jira XXX fix).
    p->set_buf_req(253, 16);

    uint8_t  a2_u8_src[]  = {4, 5, 0, 0};
    uint16_t a2_u8_dst[]  = {static_cast<uint16_t>(4 + PHV8_0), static_cast<uint16_t>(5 + PHV8_0), NoX, NoX };
    uint8_t  a2_u16_src[] = {0, 0, 0, 0};
    uint16_t a2_u16_dst[] = {NoX, NoX, NoX, NoX};
    uint8_t  a2_u32_src[] = {0, 0, 0, 0};
    uint16_t a2_u32_dst[] = {NoX,NoX,NoX,NoX};

    p->set_action(state,
                  F, 0, b_FF, u8_00,                       // adj RESET, adj_inc, csum ENABLE, csum_addr
                  b_FFFF, b_FFFF, a2_u8_src,  a2_u8_dst,   // xtrac8:  SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv
                  b_FFFF, b_FFFF, a2_u16_src, a2_u16_dst,  // xtrac16: SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv
                  b_FFFF, b_FFFF, a2_u32_src, a2_u32_dst); // xtrac32: SRC_IMM_VAL,ADJ_OFF,src_off, dst_phv

    p->set_tcam_match(state,                    // Use 0x99 in value field if we don't care
                      p->make_tcam_entry(state, (uint16_t)0x9999, (uint8_t)0x99, (uint8_t)0x99),  // value (state=0)
                      p->make_tcam_entry(0xFF,  (uint16_t)     0, (uint8_t)   0, (uint8_t)   0)); // mask

    // Parse using above config
    try {
      Phv *phv1 = p->parse(pkt1, 0);
      EXPECT_TRUE(phv1 == NULL);
    }
    catch(...) {
    }

    // Cleanup
    if (prs_print) RMT_UT_LOG_INFO("Delete pkt1\n");
    om->pkt_delete(pkt1);
  }
#endif


// ******************** JBAY only tests ********************

#ifdef MODEL_CHIP_JBAY_OR_LATER

   TEST(BFN_TEST_NAME(ParseTest),CounterStack) {
     bool uses_val_post_pop = false;
     bool uses_val_pre_pop = !uses_val_post_pop;
     GLOBAL_MODEL->Reset();

     if (prs_print) printf("test_parse_counter_stack()\n");
     TestUtil tu(GLOBAL_MODEL.get());
     RmtObjectManager *om = tu.get_objmgr();
     ASSERT_TRUE(om != NULL);
     int8_t cntr;

     // Check size0 stack returns error on push/pop
     // and does not store anything
     ParserCounterStack<0> stack0;
     EXPECT_FALSE(stack0.push((int8_t)99, false));
     cntr = 111;
     EXPECT_FALSE(stack0.pop(&cntr));
     EXPECT_EQ(0, cntr);

     // Check size1 stack returns error on 2nd push
     // and discards 1st pushed val
     ParserCounterStack<1> stack1;
     EXPECT_TRUE( stack1.push((int8_t)99, false));
     EXPECT_FALSE(stack1.push((int8_t)88, false));
     cntr = 111;
     EXPECT_TRUE( stack1.pop(&cntr));
     if (uses_val_pre_pop) {
       EXPECT_EQ(88, cntr);
     } else {
       EXPECT_EQ(0, cntr);
     }
     cntr = 111;
     EXPECT_FALSE(stack1.pop(&cntr));
     EXPECT_EQ(0, cntr);

     // Check size2 stack returns error on 3rd push
     // and discards 1st pushed val
     ParserCounterStack<2> stack2;
     EXPECT_TRUE( stack2.push((int8_t)99, false));
     EXPECT_TRUE( stack2.push((int8_t)88, false));
     EXPECT_FALSE(stack2.push((int8_t)77, false));
     cntr = 111;
     EXPECT_TRUE( stack2.pop(&cntr));
     if (uses_val_pre_pop) {
       EXPECT_EQ(77, cntr);
     } else {
       EXPECT_EQ(88, cntr);
     }
     cntr = 111;
     EXPECT_TRUE( stack2.pop(&cntr));
     if (uses_val_pre_pop) {
       EXPECT_EQ(88, cntr);
     } else {
       EXPECT_EQ(0, cntr);
     }
     cntr = 111;
     EXPECT_FALSE(stack2.pop(&cntr));
     EXPECT_EQ(0, cntr);

     ParserCounterStack<3> stack3;
     // Loop pushing vals into a size3 stack
     // Check we always get back what we put in
     for (int ii = -127; ii <= 127; ii++) {
       int8_t i = static_cast<int8_t>(ii);
       EXPECT_TRUE( stack3.push((int8_t)-99, false) );
       EXPECT_TRUE( stack3.push(          i, false) );
       EXPECT_TRUE( stack3.push(         -i, false) );
       EXPECT_FALSE(stack3.push((int8_t) 99, false) );
       cntr = 111;
       EXPECT_TRUE(stack3.pop(&cntr));
       if (uses_val_pre_pop) {
         EXPECT_EQ(99, cntr);
       } else {
         EXPECT_EQ(-i, cntr);
       }
       cntr = 111;
       EXPECT_TRUE(stack3.pop(&cntr));
       if (uses_val_pre_pop) {
         EXPECT_EQ(-i, cntr);
       } else {
         EXPECT_EQ(i, cntr);
       }
       cntr = 111;
       EXPECT_TRUE(stack3.pop(&cntr));
       if (uses_val_pre_pop) {
         EXPECT_EQ(i, cntr);
       } else {
         EXPECT_EQ(0, cntr);
       }
       cntr = 111;
       EXPECT_FALSE(stack3.pop(&cntr));
       EXPECT_EQ(0, cntr);
     }

     ParserCounterStack<100> stack;
     for (int inc = -11; inc <= 11; inc+=2) {
       // Push in a 100 vals but set last/top n
       // as propagate_incr. Then incr and check
       // only the top n vals get modified
       for (int n = 0; n < 100; n++) {
         // Push in vals 0,1,2,3,....,99
         // but each time reduce number vals
         // that have the propagate flag set
         for (int i = 0; i < 100; i++) {
           EXPECT_TRUE( stack.push((int8_t)i, (i>n)) );
         }
         // Now increment all vals with flag set by inc
         // First time top 99 should be incremented
         // Next time top 98 should be incremented
         stack.incr(inc);
         // Read back pushed vals - first few should
         // have been incremented, remainder not
         for (int j = 0; j < 100; j++) {
           // When j=0,1 we pop entry pushed in at i=99,98
           // (if !uses_val_pre_pop then 98,97,)
           int k = (uses_val_pre_pop) ?99-j :99-1-j;
           int exp = (k<0) ?0 :( (k>n) ?k+inc :k );
           cntr = -111;
           EXPECT_TRUE( stack.pop(&cntr) );
           EXPECT_EQ(exp, cntr);
         }
         // Check stack now empty
         cntr = -111;
         EXPECT_FALSE(stack.pop(&cntr));
         EXPECT_EQ(0, cntr);
       }
     }
   }


   TEST(BFN_TEST_NAME(ParseTest),Clots) {
     GLOBAL_MODEL->Reset();
     if (prs_print) printf("test_parse_clots()\n");
     TestUtil tu(GLOBAL_MODEL.get());
     RmtObjectManager *om = tu.get_objmgr();
     ASSERT_TRUE(om != NULL);
     Clot clot;

     // Most of these tests were written before
     // adjacent clots were allowed so disable for now
     // (we enable later before Overlap checks 2)
     Clot::kAllowAdjacent = false;

     // Check we can only add tags 0-63
     for (int i = 0; i < 256; i++) {
       clot.reset();
       EXPECT_EQ(0,clot.n_tags_set());
       // Add 10 other tags first not including i
       int lim = (i < 10) ?11 :10;
       for (int j = 0; j < lim; j++) {
         //printf("ClotTest1: i=%d j=%d\n", i, j);
         if (j != i) {
           uint8_t  tag = static_cast<uint8_t>(j);
           // Space out clots to avoid overlap fails
           uint16_t len = static_cast<uint16_t>(((i*257) % 16) + 1);
           uint16_t off = static_cast<uint16_t>(((i*263) % 111) + (j*20));
           uint16_t cksum = static_cast<uint16_t>(i*j*97);
           EXPECT_TRUE(clot.is_valid_tag(tag));
           EXPECT_TRUE(clot.is_valid_length_offset(len,off));
           EXPECT_FALSE(clot.is_full());
           EXPECT_TRUE(clot.set(tag,len,off,cksum));
         }
       }
       EXPECT_EQ(10,clot.n_tags_set());
       // Now add single further tag=i
       uint8_t  tag = static_cast<uint8_t>(i);
       uint16_t len = 1+(i%13);
       uint16_t off = 330+(i%23);
       uint16_t cksum = i*157;
       uint16_t len2, off2, cksum2;
       if (i <= 63) {
         EXPECT_TRUE(clot.set(tag,len,off,cksum));
         EXPECT_EQ(11,clot.n_tags_set());
         EXPECT_TRUE(clot.get(tag,&len2,&off2,&cksum2));
         EXPECT_EQ(len,len2);
         EXPECT_EQ(off,off2);
         EXPECT_EQ(cksum,cksum2);
       } else {
         EXPECT_FALSE(clot.set(tag,len,off,cksum));
         EXPECT_EQ(10,clot.n_tags_set());
         EXPECT_FALSE(clot.get(tag,&len2,&off2,&cksum2));
       }
     }

     // Check we can only add length <= 64, offset <= 383
     // and length+offset <= 384 (when using dflt min/max off)
     for (uint16_t len = 1; len < 200; len++) {
       for (uint16_t off = 0; off < 500; off++) {
         //printf("ClotTest2: len=%d off=%d\n", len, off);
         clot.reset();
         uint8_t  tag = (len+off)%64;
         uint16_t cksum = len*off*199;
         if ((len <= 64) && (off <= 383) && (len + off <= 384)) {
           EXPECT_TRUE(clot.set(tag,len,off,cksum));
           EXPECT_EQ(1,clot.n_tags_set());
         } else {
           EXPECT_FALSE(clot.set(tag,len,off,cksum));
           EXPECT_EQ(0,clot.n_tags_set());
         }
       }
     }

     // Vary min/max offset and check len/off
     // verified correctly against those too
     GLOBAL_THROW_ON_ERROR = 1;
     for (int ii = 0; ii <= 20; ii++) {
       int i = ii - 10; // so i in [-10,10]
       int min_off = (i+0)*26;
       int max_off = (i+1)*39;
       int tmp;
       bool swap = ((ii % 10) == 0); // Every so often make max < min
       if (swap) { tmp = max_off; max_off = min_off; min_off = tmp; }

       clot.reset();
       if ((min_off < -255) || (max_off > 383) || (min_off > max_off)) {
         //printf("ClotTest3a: ii=%d\n", ii);
         EXPECT_THROW( clot.set_min_max_offset(min_off, max_off), std::runtime_error);
       } else {
         EXPECT_NO_THROW( clot.set_min_max_offset(min_off, max_off) );
         for (int len = -100; len < 100; len += 3) {
           for (int off = -300; off < 900; off += 3) {
             //printf("ClotTest3b: ii=%d len=%d off=%d\n", ii, len, off);
             clot.reset();
             clot.set_min_max_offset(min_off, max_off);
             int len_off = len + off;
             int pos_len_off = (len_off >= 0) ?len_off :-len_off;
             uint8_t  tag = static_cast<uint8_t>( (pos_len_off*88) % 64 );
             uint16_t cksum = static_cast<uint16_t>( (pos_len_off*199) % 65536 );
             if ((len > 0) && (len <= 64) &&
                 (off >= min_off) && (off <= max_off) && (len + off <= (max_off+1))) {
               EXPECT_TRUE(clot.set(tag,len,off,cksum));
               EXPECT_EQ(1,clot.n_tags_set());
             } else {
               EXPECT_FALSE(clot.set(tag,len,off,cksum));
               EXPECT_EQ(0,clot.n_tags_set());
             }
           }
         }
       }
     }

     // Check we can only add 16 tuples at a time
     for (int i = 10; i < 40 ; i++) {
       clot.reset();
       EXPECT_EQ(0,clot.n_tags_set());
       // Add 16 vals
       for (int j = 0; j < 16 ; j++) {
         //printf("ClotTest4a: i=%d j=%d\n", i, j);
         uint8_t tag = static_cast<uint8_t>(i+j);
         EXPECT_FALSE(clot.is_full());
         EXPECT_EQ(j,clot.n_tags_set());
         // Space out clots to avoid overlap fails
         EXPECT_TRUE(clot.set(tag,1+j,i+(j*20),i+i+j+j));
         EXPECT_EQ(j+1,clot.n_tags_set());
       }
       EXPECT_TRUE(clot.is_full());
       EXPECT_EQ(16,clot.n_tags_set());
       // Check can't add any more
       for (int k = 0; k < 10 ; k++) {
         //printf("ClotTest4b: i=%d k=%d\n", i, k);
         uint8_t tag = static_cast<uint8_t>(k);
         EXPECT_FALSE(clot.set(tag,i+k,i+i+k,i+i+k+k));
       }
       // Read back and check initial 16 vals
       for (int j = 0; j < 16 ; j++) {
         //printf("ClotTest4c: i=%d j=%d\n", i, j);
         uint8_t tag = static_cast<uint8_t>(i+j);
         uint16_t len, off, cksum;
         EXPECT_TRUE(clot.get(tag,&len,&off,&cksum));
         EXPECT_EQ(1+j,len);
         EXPECT_EQ(i+(j*20),off);
         EXPECT_EQ(i+i+j+j,cksum);
       }
     }
   }

   TEST(BFN_TEST_NAME(ParseTest),AdjacentClots) {
     GLOBAL_MODEL->Reset();
     if (prs_print) printf("test_parse_adjacent_clots()\n");
     TestUtil tu(GLOBAL_MODEL.get());
     RmtObjectManager *om = tu.get_objmgr();
     ASSERT_TRUE(om != NULL);
     Clot clot;

     // Overlap checks
     //
     // 1. Relax all overlap checking - verify anything goes
     //
     Clot::kRelaxOverlapCheck = true;
     for (int i = 10; i < 40; i++) {
       clot.reset();
       EXPECT_EQ(0,clot.n_tags_set());
       // Add 16 vals
       for (int j = 0; j < 16 ; j++) {
         //printf("ClotTest5a: i=%d j=%d\n", i, j);
         uint8_t tag = static_cast<uint8_t>(i+j);
         EXPECT_FALSE(clot.is_full());
         EXPECT_EQ(j,clot.n_tags_set());
         // Use identical len/off for all clots - should be ok
         EXPECT_TRUE(clot.set(tag,i+j,i+j,i+i+j+j));
         EXPECT_EQ(j+1,clot.n_tags_set());
       }
       EXPECT_TRUE(clot.is_full());
       EXPECT_EQ(16,clot.n_tags_set());
     }

     // 2. Don't relax overlap checking
     // 2a. Don't allow adjacent clots - verify gap >= 3
     // 2b. Do allow adjacent clots - verify gap==0 or gap >= 3
     // NB. Value 3 determined by RmtDefs::kClotMinGap
     //
     int min_gap = RmtDefs::kClotMinGap;
     if (min_gap >= 0) {
       EXPECT_LT(min_gap, 9); // Test assumes min_gap in [0..9]
       Clot::kRelaxOverlapCheck = false;
       for (int allow_adjacent = 0; allow_adjacent <= 1; allow_adjacent++) {

         // Here we allow adjacent clots for the first time
         Clot::kAllowAdjacent = (allow_adjacent == 1);
         for (int gap = 0; gap <= 9; gap++) {
           //printf("ClotTest5b: aa=%d gap=%d\n", allow_adjacent, gap);
           bool gap_ok = ( ((gap == 0) && (allow_adjacent == 1)) || (gap >= min_gap) );
           clot.reset();
           EXPECT_EQ(0,clot.n_tags_set());
           // Add 3 clots of len 11,22,33 at offsets 100,200,300
           EXPECT_TRUE(clot.set(10, 11, 100, 0x0101));
           EXPECT_TRUE(clot.set(20, 22, 200, 0x0202));
           EXPECT_TRUE(clot.set(30, 33, 300, 0x0303));
           EXPECT_EQ(3,clot.n_tags_set());
           // Now try to add 6 more clots with varying gaps
           // After...
           EXPECT_EQ((gap_ok), clot.set(10+1, 11, 100+11+gap, 0x1111));
           EXPECT_EQ((gap_ok), clot.set(20+1, 22, 200+22+gap, 0x2222));
           EXPECT_EQ((gap_ok), clot.set(30+1, 33, 300+33+gap, 0x3333));
           // Before...
           EXPECT_EQ((gap_ok), clot.set(10-1, 11, 100-11-gap, 0x1111));
           EXPECT_EQ((gap_ok), clot.set(20-1, 22, 200-22-gap, 0x2222));
           EXPECT_EQ((gap_ok), clot.set(30-1, 33, 300-33-gap, 0x3333));
         }
       }
     }
   }

   TEST(BFN_TEST_NAME(ParseTest),DuplicateClots) {
     GLOBAL_MODEL->Reset();
     if (prs_print) printf("test_parse_duplicate_clots()\n");
     TestUtil tu(GLOBAL_MODEL.get());
     RmtObjectManager *om = tu.get_objmgr();
     ASSERT_TRUE(om != NULL);
     Clot clot;
     // Relax overlap checking so we don't have to get len/offset right
     Clot::kRelaxOverlapCheck = true;

     // Duplicate checks
     //
     // 1. Relax all duplicate checking - verify anything goes
     //
     Clot::kAllowDuplicate = true;
     for (int i = 10; i < 40; i++) {
       clot.reset();
       EXPECT_EQ(0,clot.n_tags_set());
       uint8_t tag = static_cast<uint8_t>(i);
       // Add 16 vals all with same tag
       for (int j = 0; j < 16 ; j++) {
         EXPECT_FALSE(clot.is_full());
         EXPECT_EQ(j,clot.n_tags_set());
         // Use identical tag for all clots - should be ok
         EXPECT_TRUE(clot.set(tag,i+j,i+j,i+i+j+j));
         EXPECT_EQ(j+1,clot.n_tags_set());
       }
       EXPECT_TRUE(clot.is_full());
       EXPECT_EQ(16,clot.n_tags_set());
     }

     // 2. Don't relax duplicate checking
     //    clot.set should fail to add duplicate tag
     //
     Clot::kAllowDuplicate = false;
     for (int i = 10; i < 40; i++) {
       clot.reset();
       EXPECT_EQ(0,clot.n_tags_set());
       // Add 16 vals
       // But for i [11,25] for one j in [1,15] add a tag that's a dup of the j[0] tag
       uint8_t tag0 = static_cast<uint8_t>(i);
       for (int j = 0; j < 16 ; j++) {
         bool try_adding_dup = ((j > 0) && ((j + 10) == i));
         uint8_t tag = (try_adding_dup) ?tag0 :static_cast<uint8_t>(i+j);
         EXPECT_FALSE(clot.is_full());
         // Non-dups should be ok, dups should fail
         bool set_ok = clot.set(tag,i+j,i+j,i+i+j+j);
         //printf("i=%d j=%d Tag0=%d Tag=%d Dup=%c SetOK=%c\n",
         //       i, j, tag0, tag, try_adding_dup?'T':'F', set_ok?'T':'F');
         EXPECT_EQ(!try_adding_dup, set_ok);
       }
     }
   }

   TEST(BFN_TEST_NAME(ParseTest),ExtractChecksForAdjacentClots) {
     GLOBAL_MODEL->Reset();
     if (prs_print) printf("test_parse_extract_checks_for_adjacent_clots()\n");
     TestUtil tu(GLOBAL_MODEL.get());
     RmtObjectManager *om = tu.get_objmgr();
     ASSERT_TRUE(om != NULL);
     Clot clot;
     Clot::kAllowAdjacent = true;
     // Check we can only extract adjacent clots in packet order

     // Add 2 to 16 adjacent clots to a fresh clot
     for (int n_clots = 2; n_clots <= 16; n_clots++) {
       clot.reset();

       // Form adjacent clots like this:
       // n_clots=2  [0,1] [2,3]
       // n_clots=3  [0,2] [3,5] [6,8]
       // ...
       // n_clots=16 [0,15] [16,31] [32,47] ... [240,255]
       //
       for (int i = 0; i < n_clots; i++) {
         EXPECT_TRUE(clot.set(n_clots+i, n_clots, i*n_clots, i*i+n_clots*n_clots));
       }
       // Validate we can successfully extract in order 0,1,2....
       for (int i = 0; i < n_clots; i++) {
         uint16_t len, off, cksum;
         EXPECT_TRUE(clot.get(n_clots+i, &len, &off, &cksum));
         EXPECT_FALSE(clot.err_prev_adjacent_unemitted());
         EXPECT_EQ(n_clots, len);
         EXPECT_EQ(i*n_clots, off);
         EXPECT_EQ(i*i+n_clots*n_clots, cksum);
       }

       // Reset and re-add
       clot.reset();
       for (int i = 0; i < n_clots; i++) {
         EXPECT_TRUE(clot.set(n_clots+i, n_clots, i*n_clots, i*i+n_clots*n_clots));
       }
       // Validate we fail to extract if we start on clot 1 or greater
       for (int i = 1; i < n_clots; i++) {
         uint16_t len, off, cksum;
         EXPECT_FALSE(clot.get(n_clots+i, &len, &off, &cksum));
         EXPECT_TRUE(clot.err_prev_adjacent_unemitted());
       }
       // Validate again we can successfully extract in correct order 0,1,2....
       for (int i = 0; i < n_clots; i++) {
         uint16_t len, off, cksum;
         EXPECT_TRUE(clot.get(n_clots+i, &len, &off, &cksum));
         EXPECT_FALSE(clot.err_prev_adjacent_unemitted());
       }
     }
   }

   TEST(BFN_TEST_NAME(ParseTest),SingleExtractOfEntriesInClots) {
     GLOBAL_MODEL->Reset();
     if (prs_print) printf("test_parse_single_extract_of_entries_in_clots()\n");
     TestUtil tu(GLOBAL_MODEL.get());
     RmtObjectManager *om = tu.get_objmgr();
     ASSERT_TRUE(om != NULL);
     Clot clot;

     for (int allow_repeat_emit = 0; allow_repeat_emit <= 1; allow_repeat_emit++) {
       Clot::kAllowRepeatEmit = (allow_repeat_emit == 1);

       // Add 1 to 16 clots to a fresh clot
       for (int n_clots = 1; n_clots <= 16; n_clots++) {
         clot.reset();
         // Form clots entries [0,1] [20,2] [40,3] ...
         int i;
         for (i = 0; i < n_clots; i++) {
           bool ok = clot.set(n_clots+i, i+1, i*20, i*i+n_clots*n_clots);
           //printf("Clot(set) Tag=%d Len=%d Off=%d OK=%c\n", n_clots+i, i+4, i*20, ok?'T':'F');
           EXPECT_TRUE(ok);
         }
         // Get the middle clot entry and sanity check it as set
         i = n_clots/2;
         uint16_t len, off, cksum;
         bool act_ok1 = clot.get(n_clots+i, &len, &off, &cksum);
         if (!act_ok1) { printf("Clot(get1) Tag=%d err=%s\n", n_clots+i, clot.err_str()); }
         EXPECT_TRUE(act_ok1);
         EXPECT_EQ(i+1, len);
         EXPECT_EQ(i*20, off);
         EXPECT_EQ(i*i+n_clots*n_clots, cksum);

         // Validate we can successfully extract a clot entry twice if allow_repeat_emit
         bool exp_ok2 = (allow_repeat_emit == 1);
         bool act_ok2 = clot.get(n_clots+i, &len, &off, &cksum);
         //if (!act_ok2) printf("Clot(get2) Tag=%d err=%s\n", n_clots+i, clot.err_str());
         EXPECT_EQ(exp_ok2, act_ok2);
         if (!act_ok2) { EXPECT_TRUE(clot.err_already_emitted()); }
       }
     }
   }

   TEST(BFN_TEST_NAME(ParseTest),ResetEmitAllowsMultipleExtractOfEntriesInClots) {
     GLOBAL_MODEL->Reset();
     if (prs_print) printf("test_parse_reset_emit_allows_multiple_extract_of_entries_in_clots()\n");
     TestUtil tu(GLOBAL_MODEL.get());
     RmtObjectManager *om = tu.get_objmgr();
     ASSERT_TRUE(om != NULL);
     Clot clot;
     Clot::kAllowRepeatEmit = false;

     // Add 1 to 16 clots to a fresh clot
     for (int n_clots = 1; n_clots <= 16; n_clots++) {
       clot.reset();
       // Form clots entries [0,1] [20,2] [40,3] ...
       int i;
       for (i = 0; i < n_clots; i++) {
         bool ok = clot.set(n_clots+i, i+1, i*20, i*i+n_clots*n_clots);
         //printf("Clot(set) Tag=%d Len=%d Off=%d OK=%c\n", n_clots+i, i+4, i*20, ok?'T':'F');
         EXPECT_TRUE(ok);
       }
       // Get the middle clot entry and sanity check it as set
       i = n_clots/2;
       uint16_t len, off, cksum;
       bool act_ok1 = clot.get(n_clots+i, &len, &off, &cksum);
       //if (!act_ok1) { printf("Clot(get1) Tag=%d err=%s\n", n_clots+i, clot.err_str()); }
       EXPECT_TRUE(act_ok1);
       EXPECT_EQ(i+1, len);
       EXPECT_EQ(i*20, off);
       EXPECT_EQ(i*i+n_clots*n_clots, cksum);
       EXPECT_FALSE(clot.err_already_emitted());

       // Validate we fail if we attempt to extract the clot entry again
       bool act_ok2 = clot.get(n_clots+i, &len, &off, &cksum);
       //printf("Clot(get2) Tag=%d err=%s\n", n_clots+i, clot.err_str());
       EXPECT_FALSE(act_ok2);
       EXPECT_TRUE(clot.err_already_emitted());

       // Now call reset_emit to reset clot emission state
       // Check we now succeed in extracting the clot entry again
       clot.reset_emit();
       bool act_ok3 = clot.get(n_clots+i, &len, &off, &cksum);
       //if (!act_ok3) printf("Clot(get3) Tag=%d err=%s\n", n_clots+i, clot.err_str());
       EXPECT_TRUE(act_ok3);
       EXPECT_EQ(i+1, len);
       EXPECT_EQ(i*20, off);
       EXPECT_EQ(i*i+n_clots*n_clots, cksum);
       EXPECT_FALSE(clot.err_already_emitted());
     }
   }

  TEST_F(MCN_CONCAT(BFN_TEST_NAME(ParseTest), UsingConfigBase), PrematureExtractOfClots) {
    // Ensure we see errors when they occur
    parser_->kRelaxExtractionCheck = false;
    GLOBAL_THROW_ON_ERROR = 1;

    // Enable all ActionRam banks for all entries (this a WIP only feature; ignored on JBay)
    for (int i = 0; i < 256; i++) parser_->set_action_ram_en(i, 3);

    // Change start state to be 77 (arbitrary value)
    parser_->set_channel(0, true, 77);
    // Use index 7 to handle state 77...
    parser_->set_tcam_match(7,          // We use 0x99 in value field if we don't care
                            parser_->make_tcam_entry(  77, (uint16_t)0x99, 0x99, 0x99),
                            parser_->make_tcam_entry(0xFF, (uint16_t)   0,    0,    0));
    // ...but do nothing except move to state 0 (our normal initial state)
    parser_->set_next_state(7, 0);
    parser_->set_next_state_mask(7, 0xFF);

    // Finally fixup catch-all TCAM entry 251 to only match on states [0..15]
    // (ie mask=0xF0), so TCAM lookup can actually get to our new index 7
    parser_->set_tcam_match(251,
                            parser_->make_tcam_entry(   0, (uint16_t)0x0, 0x00, 0x00),
                            parser_->make_tcam_entry(0xF0, (uint16_t)  0,    0,    0));

    // Run do_test - all should be well
    EXPECT_NO_THROW(do_test());


    // Now configure our new index 7 to also extract a CLOT
    // (this is INVALID as a CLOT extract may not precede PHV extracts)
    // do_test should provoke an error (and maybe an exception)
    parser_->set_action_clot(7,
                             0, 0, 32, 1,     // ClotIndex=0 ClotType=IMM, ClotLen=32, ClotEN,
                             0x3F, 0,         // ClotLenMask=0x3F, ClotLenAdd=0,
                             0, 1, false, 0); // ClotOffset=0 ClotTag=1 ClotTagOffsetAdd=F


    // Run do_test - an ERROR will be logged but there will be no exception
    parser_->kRelaxExtractionCheck = true;
    EXPECT_NO_THROW(do_test());
    // Free up clots else we'll see 'Tag reused'
    pkt1_->reset_clots(); pkt2_->reset_clots();


    // Run do_test - an ERROR will be logged and there will be an exception
    parser_->kRelaxExtractionCheck = false;
    EXPECT_THROW(do_test(), std::runtime_error);
  }

#endif /* MODEL_CHIP_JBAY_OR_LATER */


}
