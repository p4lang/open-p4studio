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

#include <model_core/model.h>
#include <bitvector.h>
#include <mau.h>
#include <tcam.h>
#include <rmt-object-manager.h>
#include <packet.h>
#include <phv-factory.h>
#include <phv.h>
#include <parser.h>
#include <deparser-block.h>
#include <deparser.h>
#include <parser-block.h>
#include <parser-static-config.h>
#include <port.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool prt_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


  TEST(BFN_TEST_NAME(PortTest),Basic) {
    GLOBAL_MODEL->Reset();
    if (prt_print) RMT_UT_LOG_INFO("test_port_basic()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    //RmtObjectManager *om = new RmtObjectManager();
    if (prt_print) RMT_UT_LOG_INFO("Create PORT\n");
    Port *port0 = om->port_get(0);
    if (prt_print) RMT_UT_LOG_INFO("Port0 hdrWord = %d\n", port0->hdr_word0());
    EXPECT_EQ(0, port0->port_index());
    Port *port1 = om->port_get(1);
    EXPECT_EQ(1, port1->port_index());
    if (prt_print) RMT_UT_LOG_INFO("Port1 hdrWord = %d\n", port1->hdr_word0());
  }

  TEST(BFN_TEST_NAME(PortTest),UsingConfig) {
    GLOBAL_MODEL->Reset();
    if (prt_print) RMT_UT_LOG_INFO("test_port_using_config()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    //                    <   DA           SA      ET I4 C LN  ID  OF  T P CK   SRC     DST     SP  DP>
    //                    <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST   ><SP><DP>
    const char *pktstr1 = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000";
    const char *pktstr2 = "080022AABBCC080011DDEEFF080045000100123400001006FFFF0A1122330A4455660688069900000000000000000000000000000000";
    // Create Packets on heap
    if (prt_print) RMT_UT_LOG_INFO("Create packet\n");
    Packet *pkt1 = om->pkt_create(pktstr1);
    Packet *pkt2 = om->pkt_create(pktstr2);

    // Create a port - make sure it ends up on channel 0 at parser
    Port *port = om->port_get(12);
    ASSERT_TRUE(port != NULL);
    EXPECT_EQ(12, port->port_index());
    // Get parser associated with port
    // Should have been automatically instantiated
    if (prt_print) RMT_UT_LOG_INFO("Get parser\n");
    Parser *parser = port->parser()->ingress();
    ASSERT_TRUE(parser != NULL);
    //These default configs don't work with JBay
    //EXPECT_EQ(RmtDefs::kPort_Config_Default[12].parser_index, parser->parser_index());
    parser->reset();

    // Initialize with basic config
    if (prt_print) RMT_UT_LOG_INFO("Initialize parser\n");
    parser_config_basic_eth_ip_tcp(parser);
    parser->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
    parser->set_channel(0, true, 0);
    if (prt_print) parser->print();

    // Now see if we can parse packets through port
    if (prt_print) RMT_UT_LOG_INFO("Parse packet\n");
    Phv *phv1 = port->parse(pkt1);
    ASSERT_TRUE(phv1 != NULL);
    EXPECT_EQ(0x1188u, phv1->get_p(Phv::make_word_p(4,7)));
    EXPECT_EQ(0x1199u, phv1->get_p(Phv::make_word_p(4,8)));
    EXPECT_EQ(0x11u, phv1->get_p(Phv::make_word_p(3,1)));
    EXPECT_EQ(0x22u, phv1->get_p(Phv::make_word_p(3,2)));
    EXPECT_EQ(0x33u, phv1->get_p(Phv::make_word_p(3,3)));
    if (prt_print) phv1->print_p("PHV1 DUMP", true);

    Phv *phv2 = port->parse(pkt2);
    ASSERT_TRUE(phv2 != NULL);
    EXPECT_EQ(0x0688u, phv2->get_p(Phv::make_word_p(4,7)));
    EXPECT_EQ(0x0699u, phv2->get_p(Phv::make_word_p(4,8)));
    EXPECT_EQ(0x11u, phv2->get_p(Phv::make_word_p(3,1)));
    EXPECT_EQ(0x22u, phv2->get_p(Phv::make_word_p(3,2)));
    EXPECT_EQ(0x33u, phv2->get_p(Phv::make_word_p(3,3)));
    if (prt_print) phv2->print_p("PHV2 DUMP", true);

    // Cleanup
    if (prt_print) RMT_UT_LOG_INFO("Delete pkt1\n");
    om->pkt_delete(pkt1);
    if (prt_print) RMT_UT_LOG_INFO("Delete pkt2\n");
    om->pkt_delete(pkt2);
  }

  TEST(BFN_TEST_NAME(PortTest),FullChipUsingConfig) {
    GLOBAL_MODEL->Reset();
    if (prt_print) RMT_UT_LOG_INFO("test_port_full_chip_using_config()\n");
    // Initializing full chip in this test so use a
    // chip 202 which is restricted to 2 MAUs per-pipe
    int chip = 202;
    TestUtil tu(GLOBAL_MODEL.get(), chip);
    RmtObjectManager *om = tu.get_objmgr();

    //om->set_initial_log_flags(ALL);
    //                    <   DA           SA      ET I4 C LN  ID  OF  T P CK   SRC     DST     SP  DP>
    //                    <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST   ><SP><DP>
    const char *pktstr1 = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000";
    const char *pktstr2 = "080022AABBCC080011DDEEFF080045000100123400001006FFFF0A1122330A4455660688069900000000000000000000000000000000";
    // Create Packets on heap
    if (prt_print) printf("Create packet\n");
    Packet *pkt1 = om->pkt_create(pktstr1);
    Packet *pkt2 = om->pkt_create(pktstr2);

    // Create a port - make sure it ends up on channel 0 at parser
    Port *port = om->port_get(12);
    ASSERT_TRUE(port != NULL);
    EXPECT_EQ(12, port->port_index());
    // Get parser associated with port
    // Should have been automatically instantiated
    if (prt_print) printf("Get parser\n");
    Parser *parser = port->parser()->ingress();
    ASSERT_TRUE(parser != NULL);
    //EXPECT_EQ(RmtDefs::kPort_Config_Default[12].parser_index, parser->parser_index());
    parser->reset();

    // Initialize with basic config
    if (prt_print) printf("Initialize parser\n");
    parser_config_basic_eth_ip_tcp(parser);
    parser->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
    parser->set_channel(0, true, 0);
    if (prt_print) parser->print();

    // Now see if we can parse packets through port
    if (prt_print) printf("Parse packet\n");
    Phv *phv1 = port->parse(pkt1);
    ASSERT_TRUE(phv1 != NULL);
    EXPECT_EQ(0x1188u, phv1->get_p(Phv::make_word_p(4,7)));
    EXPECT_EQ(0x1199u, phv1->get_p(Phv::make_word_p(4,8)));
    EXPECT_EQ(0x11u, phv1->get_p(Phv::make_word_p(3,1)));
    EXPECT_EQ(0x22u, phv1->get_p(Phv::make_word_p(3,2)));
    EXPECT_EQ(0x33u, phv1->get_p(Phv::make_word_p(3,3)));
    if (prt_print) phv1->print_p("PHV1 DUMP", false);

    Phv *phv2 = port->parse(pkt2);
    ASSERT_TRUE(phv2 != NULL);
    EXPECT_EQ(0x0688u, phv2->get_p(Phv::make_word_p(4,7)));
    EXPECT_EQ(0x0699u, phv2->get_p(Phv::make_word_p(4,8)));
    EXPECT_EQ(0x11u, phv2->get_p(Phv::make_word_p(3,1)));
    EXPECT_EQ(0x22u, phv2->get_p(Phv::make_word_p(3,2)));
    EXPECT_EQ(0x33u, phv2->get_p(Phv::make_word_p(3,3)));
    if (prt_print) phv2->print_p("PHV2 DUMP", false);

    // Cleanup
    if (prt_print) printf("Delete pkt1\n");
    om->pkt_delete(pkt1);
    if (prt_print) printf("Delete pkt2\n");
    om->pkt_delete(pkt2);
  }

  TEST(BFN_TEST_NAME(PortTest),FullChipFullPipeUsingConfig) {
    GLOBAL_MODEL->Reset();
    if (prt_print) RMT_UT_LOG_INFO("test_port_full_chip_full_pipe_using_config()\n");
    // Chip 202 so only 2 MAUs per pipe
    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_stage_default_regs(0);
    tu.set_stage_default_regs(1);
    RmtObjectManager *om = tu.get_objmgr();

    //                    <   DA           SA      ET I4 C LN  ID  OF  T P CK   SRC     DST     SP  DP>
    //                    <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST   ><SP><DP>
    const char *pktstr1 = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000";
    //const char *pktstr2 = "080022AABBCC080011DDEEFF080045000100123400001006FFFF0A1122330A4455660688069900000000000000000000000000000000";
    // Create Packets on heap
    if (prt_print) RMT_UT_LOG_INFO("Create packet\n");
    Packet *pkt_in = om->pkt_create(pktstr1);
    ASSERT_TRUE(pkt_in != NULL);

    // Create a port - make sure it ends up on channel 0 at parser
    Port *port = om->port_get(12);
    ASSERT_TRUE(port != NULL);
    EXPECT_EQ(12, port->port_index());
    // Get parser associated with port
    // Should have been automatically instantiated
    Parser *parser = port->parser()->ingress();
    ASSERT_TRUE(parser != NULL);
    //EXPECT_EQ(RmtDefs::kPort_Config_Default[12].parser_index, parser->parser_index());

    // Initialize with basic config
    if (prt_print) RMT_UT_LOG_INFO("Initialize parser\n");
    parser_config_basic_eth_ip_tcp(parser);
    parser->set_parser_mode(RmtDefs::kParserChannels,
                            RmtDefs::kParserMaxStates);
    parser->set_channel(0, true, 0);
    if (prt_print) parser->print();

    // Get deparser associated with port
    if (prt_print) RMT_UT_LOG_INFO("Get deparser\n");
    Deparser *deparser = port->deparser()->ingress();
    ASSERT_TRUE(deparser != NULL);

    // Initialize with basic config
    if (prt_print) RMT_UT_LOG_INFO("Initialize deparser\n");
    tu.deparser_init(deparser);

    // Now see if we can *process* packets through port
    // This sends packet into *pipe* which parses
    // packet then runs it through MAUs (no deparse_
    //
    // Note not expecting much to happen here as we
    // have no MAU config in place
    //
    if (prt_print) RMT_UT_LOG_INFO("Process packet\n");
    Phv *phv_out = port->parse_matchaction(pkt_in);

    ASSERT_TRUE(phv_out != NULL);

    // Now try sending through whole pipe
    // Comment this out else get assert when deparser tries to
    // read PHV word 240!!
    //
    //Packet *packet_out = port->process_inbound(pkt_in);
    //ASSERT_TRUE(packet_out != NULL);

    Phv *phv = port->parse(pkt_in);
    if (prt_print) RMT_UT_LOG_INFO("Process this PHV\n");
    if (prt_print) phv->print_p("this PHV\n", false);

    LearnQuantumType lq;
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    PacketGenMetadata* packet_gen_metadata = nullptr;
    Packet *packet_out = port->deparser()->DeparseIngress(*phv,&lq,&mirror_pkt,&resubmit_pkt,&packet_gen_metadata);

    if (packet_out != NULL) {
      int len=40;
      uint8_t buf[len];
      packet_out->get_buf(buf,0,len);
      for (int i=0; i<len; i++)  {
        if (prt_print) RMT_UT_LOG_INFO("buf[%d] : %x\n",i,buf[i]);
      }
    }
  }

  // This test doesn't check anything, it was just to look at the time of packets in the MAU
  //  in Tofino. In JBay the deparser drops the ingress packet so the egress path does not get run.
  TEST(BFN_TEST_NAME(PortTest),PacketTime) {
    GLOBAL_MODEL->Reset();
    if (prt_print) RMT_UT_LOG_INFO("test_port_packet_time)\n");
    // Chip 202 so only 2 MAUs per pipe
    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_stage_default_regs(0);
    tu.set_stage_default_regs(1);
    RmtObjectManager *om = tu.get_objmgr();

    // Get logging
    tu.update_log_flags(ALL, ALL, ALL, ALL, ALL, FEW, FEW);

    //                    <   DA           SA      ET I4 C LN  ID  OF  T P CK   SRC     DST     SP  DP>
    //                    <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST   ><SP><DP>
    const char *pktstr1 = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000";
    //const char *pktstr2 = "080022AABBCC080011DDEEFF080045000100123400001006FFFF0A1122330A4455660688069900000000000000000000000000000000";

    // Create a port - make sure it ends up on channel 0 at parser
    Port *port = om->port_get(12);
    ASSERT_TRUE(port != NULL);
    EXPECT_EQ(12, port->port_index());

    // Setup egress parser first - will set ALL prs_merge
    // owner bits to egress - later when we setup ingress
    // parser they'll all be switched back to ingress
    Parser *egr_parser = port->parser()->egress();
    ASSERT_TRUE(egr_parser != NULL);
    if (prt_print) RMT_UT_LOG_INFO("Initialize egress parser\n");
    egr_parser->set_parser_mode(RmtDefs::kParserChannels,
                                RmtDefs::kParserMaxStates);
    egr_parser->set_channel(0, true, 0);
    egr_parser->set_identity_priority_map();
    // Set ALL egress parser TCAM entries to match on state=0 and to exit immediately (DONE=true)
    BitVector<44> bvV = egr_parser->make_tcam_entry(   0, (uint16_t)0x9999, (uint8_t)0x99, (uint8_t)0x99);
    BitVector<44> bvM = egr_parser->make_tcam_entry(0xFF, (uint16_t)     0, (uint8_t)   0, (uint8_t)   0);
    uint16_t NoX = k_phv::kBadPhv;
    bool     T=true, F=false;
    bool     b_FF[]  =   {F,F};
    bool     b_FFFF[]  = {F,F,F,F};
    uint8_t  u8_00[]   = {0,0};
    uint8_t  u8_src[]  = {0,0,0,0};
    uint16_t u8_dst[]  = {NoX,NoX,NoX,NoX};
    uint8_t  u16_src[] = {0,0,0,0};
    uint16_t u16_dst[] = {NoX,NoX,NoX,NoX};
    uint8_t  u32_src[] = {0,0,0,0};
    uint16_t u32_dst[] = {NoX,NoX,NoX,NoX};
    for (int i = 0; i < RmtDefs::kParserStates; i++) {
      egr_parser->set_tcam_match(i, bvV, bvM);
      egr_parser->set_early_action(i,
                                   0,F,F,   // Counter load src, LD_VAL, LOAD
                                   T,0,     // ****DONE***, shift_amount
                                   0,0,0,   // field8_1 off, field8_0 off, field16 off
                                   F,F,F,   // field8_1 LOAD, field8_0 LOAD, field16 LOAD ***NO LOAD***
                                   0xFF,0); // next_state_mask, next_state
      egr_parser->set_action(i,
                             F,0,b_FF,u8_00,                 // adj RESET,adj_inc,cksum ENABLE,cksum_addr
                             b_FFFF,b_FFFF, u8_src, u8_dst,  // extract8:  SRC_IMM_VAL,ADJ_OFF,src_offs,dst_phv
                             b_FFFF,b_FFFF,u16_src,u16_dst,  // extract16: SRC_IMM_VAL,ADJ_OFF,src_offs,dst_phv
                             b_FFFF,b_FFFF,u32_src,u32_dst); // extract32: SRC_IMM_VAL,ADJ_OFF,src_offs,dst_phv
    }
    if (prt_print) egr_parser->print();

    // Get ingress parser associated with port and initialize
    Parser *ing_parser = port->parser()->ingress();
    ASSERT_TRUE(ing_parser != NULL);
    //EXPECT_EQ(RmtDefs::kPort_Config_Default[12].parser_index, ing_parser->parser_index());
    if (prt_print) RMT_UT_LOG_INFO("Initialize ingress parser\n");
    parser_config_basic_eth_ip_tcp(ing_parser);
    ing_parser->set_parser_mode(RmtDefs::kParserChannels,
                                RmtDefs::kParserMaxStates);
    ing_parser->set_channel(0, true, 0);
    if (prt_print) ing_parser->print();


    // Get deparser associated with port and initialize
    if (prt_print) RMT_UT_LOG_INFO("Get deparser\n");
    Deparser *deparser = port->deparser()->ingress();
    ASSERT_TRUE(deparser != NULL);
    if (prt_print) RMT_UT_LOG_INFO("Initialize deparser\n");
    tu.deparser_init(deparser);


    // Set things up so Pkts/Phvs get Time allocated
    Packet::kPktInitTimeRandOnAlloc = true;
    PhvFactory::kPhvInitTimeRandOnAlloc = true;

    // Process a bunch of packets - see if MAU complains
    // about time going backwards
    if (prt_print) RMT_UT_LOG_INFO("Process packets\n");
    Packet *pkt_in  = NULL, *pkt_out = NULL, *pkt_tx  = NULL;
    uint64_t TT = UINT64_C(800000000), CC;
    om->time_set(TT);

    for (int i = 0; i < 2; i++) {
      TT = om->time_get(); CC = om->time_get_cycles();
      printf(">>>>> Time=%" PRId64 "ps (Cycles=%" PRId64 ") Created pkt %d\n", TT, CC, i);
      pkt_in = om->pkt_create(pktstr1);
      ASSERT_TRUE(pkt_in != NULL);

      TT = om->time_get(); CC = om->time_get_cycles();
      printf(">>>>> Time=%" PRId64 "ps (Cycles=%" PRId64 ") Inbound pkt %d\n", TT, CC, i);
      // Reconfigure ingress parser so all prs_merge owner bits owned by ingress
      ing_parser->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
      pkt_out = tu.port_process_inbound(port, pkt_in);

      if (pkt_out != NULL) {
        //om->time_increment(UINT64_C(800000)); // 800000ps = 1000 cycles
        TT = om->time_get(); CC = om->time_get_cycles();
        printf(">>>>> Time=%" PRId64 "ps (Cycles=%" PRId64 ") Outbound pkt %d\n", TT, CC, i);
        // Reconfigure egress parser so all prs_merge owner bits owned by egress
        egr_parser->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
        pkt_tx = tu.port_process_outbound(port, pkt_out);

        // Cleanup
        if ((pkt_tx != NULL) && (pkt_tx != pkt_out) && (pkt_tx != pkt_in))
          om->pkt_delete(pkt_tx);
        if ((pkt_out != NULL) && (pkt_out != pkt_in))
          om->pkt_delete(pkt_out);
        om->pkt_delete(pkt_in);
      }


      // Move on time
      om->time_increment(UINT64_C(800000)); // 800000ps = 1000 cycles
    }

    // Quieten down!
    tu.finish_test();
    tu.quieten_log_flags();
  }


  TEST(BFN_TEST_NAME(PortTest),Dependencies1) {
    GLOBAL_MODEL->Reset();
    if (prt_print) RMT_UT_LOG_INFO("test_port_dependencies1()\n");
    int chip = 0;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    RmtObjectManager *om = tu.get_objmgr();

    // Setup some dynamic dependency features in MAUs for pipe0
    Mau *maus[12];
    for (int i = 0; i < 12; i++) maus[i] = om->mau_lookup(0,i);
    // First setup basic deps
    int mdep = 0, adep = 1, conc = 2;
    // Don't use concurrent mode on JBay
    if (RmtObject::is_jbay_or_later()) conc = 1;
    tu.set_dependency(pipe, 0x0, mdep, true);
    tu.set_dependency(pipe, 0x1, conc, true);
    tu.set_dependency(pipe, 0x2, mdep, true);
    tu.set_dependency(pipe, 0x3, mdep, true);
    tu.set_dependency(pipe, 0x4, conc, true);
    tu.set_dependency(pipe, 0x5, conc, true);
    tu.set_dependency(pipe, 0x6, mdep, true);
    tu.set_dependency(pipe, 0x7, mdep, true);
    tu.set_dependency(pipe, 0x8, mdep, true);
    tu.set_dependency(pipe, 0x9, adep, true);
    tu.set_dependency(pipe, 0xa, adep, true);
    tu.set_dependency(pipe, 0xb, conc, true);
    // Then configure features
    maus[0x0]->mau_dependencies()->set_dynamic_features(0,0x0002);
    maus[0x1]->mau_dependencies()->set_dynamic_features(0,0x0002);
    maus[0x2]->mau_dependencies()->set_dynamic_features(0,0x0000);
    maus[0x3]->mau_dependencies()->set_dynamic_features(0,0x0002);
    maus[0x4]->mau_dependencies()->set_dynamic_features(0,0x0002);
    maus[0x5]->mau_dependencies()->set_dynamic_features(0,0x0300);
    maus[0x6]->mau_dependencies()->set_dynamic_features(0,0x0003);
    maus[0x7]->mau_dependencies()->set_dynamic_features(0,0x0302);
    maus[0x8]->mau_dependencies()->set_dynamic_features(0,0x1003);
    maus[0x9]->mau_dependencies()->set_dynamic_features(0,0x0302);
    maus[0xa]->mau_dependencies()->set_dynamic_features(0,0x0302);
    maus[0xb]->mau_dependencies()->set_dynamic_features(0,0x0000);
    // Check config MAU 9
    maus[0x9]->mau_dependencies()->check_config();
  }


  TEST(BFN_TEST_NAME(PortDump),Dump) {
    // Pseudo-test that just dumps out Port->Pipe->Prsr mappings
    using PORT = MODEL_CHIP_NAMESPACE::Port;
    const char *chip_types[8] = { "None", "TofinoA0", "TofinoB0", "Trestles", "JBay",
                                  "JBayB0", "CloudBreak", "Unknown3" };
    const char *chip_type = chip_types[MODEL_CHIP_NAMESPACE::RmtDefs::kChipType];
    for (int port = 0; port <= 99; port++) {
      int group = PORT::get_group_num(port);
      int chan = PORT::get_chan_num(port);
      // Only dump info for valid MACs
      if ((group >= PORT::kPortGroupMin) && (group <= PORT::kPortGroupMax) &&
          (chan >= PORT::kPortChanMin) && (chan <= PORT::kPortChanMax)) {
        // XXX: ignore odd chans on WIP - show extPort/intPort mapping
        if (RmtObject::is_chip1() && ((chan & 1) == 1)) continue;
        int ext_port = PORT::port_map_outbound(port);
        int int_port = PORT::port_map_inbound(ext_port);
        int pipe = PORT::get_pipe_num(port);
        int pipe_port = PORT::get_pipe_local_port_index(port);
        int parser = PORT::get_parser_num(port);
        int parser_chan = PORT::get_parser_chan(port);
        int ipb = PORT::get_ipb_num(port);
        int ipb_chan = PORT::get_ipb_chan(port);
        int epb = PORT::get_epb_num(port);
        int epb_chan = PORT::get_epb_chan(port);
        int mac = PORT::get_mac_num(port);
        int mac_chan = PORT::get_mac_chan(port);
        printf("%8s:  ExtPort=%2d IntPort=%2d  Group/Chan=%2d/%1d  PIPE=%1d,%2d  "
               "IPB=%2d,%1d  PRSR=%2d,%1d  EPB=%2d,%1d  MAC=%2d/%1d\n",
               chip_type, ext_port, int_port, group, chan, pipe, pipe_port,
               ipb, ipb_chan, parser, parser_chan, epb, epb_chan, mac, mac_chan);
      }
    }
  }

  TEST(BFN_TEST_NAME(PortTest),PortIndexHelpers) {
    int expected = RmtObject::is_tofinoXX() ? 0x9e : 0xba;
    EXPECT_EQ(expected, Port::make_port_index(1, 7, 2));
    EXPECT_EQ(0x97, Port::make_port_index(1, 23));
    EXPECT_EQ(0x13d, Port::make_port_index(2, 61));
    EXPECT_EQ(61, Port::get_pipe_local_port_index(0x13d));
    int value = RmtObject::is_tofinoXX() ? 0x9e : 0xba;
    EXPECT_EQ(1, Port::get_pipe_num(value));
    EXPECT_EQ(7, Port::get_group_num(value));
    EXPECT_EQ(2, Port::get_chan_num(value));
    if (RmtObject::is_chip1()) {
      EXPECT_EQ(0x13d, Port::add_die_index(0x13d, 0));
      EXPECT_EQ(0x33d, Port::add_die_index(0x13d, 1));
      EXPECT_EQ(0x53d, Port::add_die_index(0x13d, 2));
      EXPECT_EQ(0x73d, Port::add_die_index(0x13d, 3));
      EXPECT_EQ(0x73d, Port::add_die_index(0x13d, 7));  // die-id > 3 truncated
    } else {
      // die id ignored for all chips pre-WIP
      EXPECT_EQ(0x13d, Port::add_die_index(0x13d, 0));
      EXPECT_EQ(0x13d, Port::add_die_index(0x13d, 1));
      EXPECT_EQ(0x13d, Port::add_die_index(0x13d, 2));
      EXPECT_EQ(0x13d, Port::add_die_index(0x13d, 3));
      EXPECT_EQ(0x13d, Port::add_die_index(0x13d, 7));
    }

    EXPECT_TRUE(Port::is_valid_pipe_local_port_index(0));
    EXPECT_TRUE(Port::is_valid_pipe_local_port_index(71));
    EXPECT_FALSE(Port::is_valid_pipe_local_port_index(-1));
    EXPECT_FALSE(Port::is_valid_pipe_local_port_index(72));
    EXPECT_TRUE(Port::is_valid_chan_num(0));
    EXPECT_FALSE(Port::is_valid_chan_num(-1));
    EXPECT_TRUE(Port::is_valid_group_num(0));
    EXPECT_FALSE(Port::is_valid_group_num(-1));
    if (RmtObject::is_jbay_or_later()) {
      EXPECT_TRUE(Port::is_valid_chan_num(7));
      EXPECT_FALSE(Port::is_valid_chan_num(8));
      EXPECT_TRUE(Port::is_valid_group_num(8));
      EXPECT_FALSE(Port::is_valid_chan_num(9));
    } else {
      EXPECT_TRUE(Port::is_valid_chan_num(3));
      EXPECT_FALSE(Port::is_valid_chan_num(4));
      EXPECT_TRUE(Port::is_valid_group_num(17));
      EXPECT_FALSE(Port::is_valid_chan_num(18));
    }
  }
}
