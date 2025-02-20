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
#include <cstring>
#include <cassert>

#include <crafter/Crafter.h>
#include <crafter/Utils/CrafterUtils.h>
#include "gtest.h"

#include <rmt-log.h>
#include <bitvector.h>
#include <tcam.h>
#include <rmt-object-manager.h>
#include <packet.h>
#include <phv.h>
#include <parser.h>
#include <parser-block.h>
#include <parser-static-config.h>
#include <model_core/model.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

  bool crft_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


  TEST(BFN_TEST_NAME(CrafterTest),Basic) {
    GLOBAL_MODEL->Reset();
    if (crft_print) RMT_UT_LOG_INFO("test_crafter_basic()\n");
    // Try and setup a basic packet a la
    // Create a Raw layer with some data on it
    Crafter::RawLayer hello("Hello ");
    Crafter::RawLayer world("World!");

    // Create a packet to hold both layers
    Crafter::Packet packet = hello / world;

    // You can print the packet in your screen in different formats
    // packet.Print();     /* Human-readable */
    // packet.HexDump();   /* Machine-readable */
    // packet.RawString(); /* A C hex string  */

    // And last but not least, you can write the packet on the wire
    // packet.Send("wlan0");
  }

  TEST(BFN_TEST_NAME(CrafterTest),UsingConfig) {
    GLOBAL_MODEL->Reset();
    if (crft_print) RMT_UT_LOG_INFO("test_crafter_using_config()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();

    //                    <   DA           SA      ET I4 C LN  ID  OF  T P CK   SRC     DST     SP  DP>
    //                    <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST   ><SP><DP>
    //const char *pktstr1 = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A44556611881199";

    // Now a Packet on heap
    if (crft_print) RMT_UT_LOG_INFO("Create packet\n");

    // Create an Ethernet header
    Crafter::Ethernet eth_header;
    eth_header.SetDestinationMAC("08:00:22:AA:BB:CC");
    eth_header.SetSourceMAC("08:00:11:DD:EE:FF");
    //eth_header.SetType(0x0800);

    // Create an IP header
    Crafter::IP ip_header;
    /* Set the Source and Destination IP address */
    //ip_header.SetVersion(4);
    ip_header.SetSourceIP(std::string("10.17.34.51"));
    ip_header.SetDestinationIP(std::string("10.68.85.102"));
    //ip_header.SetTotalLength(256);

    // Create a TCP header
    Crafter::TCP tcp_header;
    tcp_header.SetSrcPort(0x1188);
    tcp_header.SetDstPort(0x1199);
    //tcp_header.SetSeqNumber(Crafter::RNG32());
    //tcp_header.SetFlags(Crafter::TCP::SYN);

    // A raw layer, this could be any array of bytes or chars
    Crafter::RawLayer payload("SomeBobbins");

    // Create a packet
    Crafter::Packet the_packet = eth_header / ip_header / tcp_header / payload;

    // Send it so we get checksums/lengths filled in
    // GetData does this too!
    //the_packet.Send("");
    //the_packet.Print();
    //the_packet.RawString();

    // Now create a RMT flavour packet
    uint8_t pkt_data[9999];
    size_t  pkt_len = the_packet.GetData(pkt_data);
    Packet *pkt1 = om->pkt_create(pkt_data, pkt_len);
    ASSERT_TRUE(pkt1 != NULL);

    // Create a parser
    if (crft_print) RMT_UT_LOG_INFO("Get parser\n");
    Parser *parser = om->parser_get(0,5)->ingress();
    ASSERT_TRUE(parser != NULL);
    parser->reset();

    // Initialize with basic config
    if (crft_print) RMT_UT_LOG_INFO("Initialize parser\n");
    parser_config_basic_eth_ip_tcp(parser);
    parser->set_parser_mode(RmtDefs::kParserChannels, RmtDefs::kParserMaxStates);
    parser->set_channel(0, true, 0);
    if (crft_print) parser->print();
    // Now see if we can parse packets
    if (crft_print) RMT_UT_LOG_INFO("Parse packet\n");
    Phv *phv1 = parser->parse(pkt1, 0);
    ASSERT_TRUE(phv1 != NULL);
    if (crft_print) phv1->print_p("PHV1 DUMP", false);
    EXPECT_EQ(0x1188u, phv1->get_p(Phv::make_word_p(4,7)));
    EXPECT_EQ(0x1199u, phv1->get_p(Phv::make_word_p(4,8)));
    EXPECT_EQ(0x11u, phv1->get_p(Phv::make_word_p(3,1)));
    EXPECT_EQ(0x22u, phv1->get_p(Phv::make_word_p(3,2)));
    EXPECT_EQ(0x33u, phv1->get_p(Phv::make_word_p(3,3)));
    // Cleanup
    if (crft_print) RMT_UT_LOG_INFO("Delete pkt1\n");
    om->pkt_delete(pkt1);
  }

  TEST(BFN_TEST_NAME(CrafterTest),Inline1) {
    GLOBAL_MODEL->Reset();
    // Configure parser from within test
    if (crft_print) RMT_UT_LOG_INFO("test_crafter_inline1()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    bool jbay = RmtObject::is_jbay_or_later();

    bool      T = true;
    bool      F = false;
    uint16_t  NoX = k_phv::kBadPhv;
    uint16_t  DA_HI_16 = Phv::make_word_p(4,0);
    uint16_t  DA_LO_32 = Phv::make_word_p(0,0);
    uint16_t  SA_HI_16 = Phv::make_word_p(4,1);
    uint16_t  SA_LO_32 = Phv::make_word_p(0,1);
    uint16_t  ETH_TYPE = Phv::make_word_p(4,2);
    uint16_t  IP4_ERR  = Phv::make_word_p(2,30);
    uint16_t  IP4_HL   = Phv::make_word_p(2,0);
    uint16_t  IP4_TTL  = Phv::make_word_p(2,2);
    uint16_t  IP4_PROTO= Phv::make_word_p(2,3);
    uint16_t  IP4_LEN  = Phv::make_word_p(4,4);
    uint16_t  IP4_ID   = Phv::make_word_p(4,3);
    uint16_t  IP4_FRAG = Phv::make_word_p(4,5);
    uint16_t  IP4_CKSM = Phv::make_word_p(4,6);
    uint16_t  IP4_SRC  = Phv::make_word_p(0,2);
    uint16_t  IP4_DST  = Phv::make_word_p(0,3);
    uint16_t  P_SPORT  = Phv::make_word_p(4,7);
    uint16_t  P_DPORT  = Phv::make_word_p(4,8);
    uint16_t  PHV8_0   = Phv::make_word_p(3,0);
    uint16_t  PHV8_1   = Phv::make_word_p(3,1);
    uint16_t  PHV8_2   = Phv::make_word_p(3,2);
    //uint16_t  PHV8_3   = Phv::make_word_p(3,3);
    bool      b_TF[]   = { T,F };
    bool      b_FF[]   = { F,F };
    bool      b_FFFF[] = { F,F,F,F };
    bool      b_TTTT[] = { T,T,T,T };
    //bool      b_FFFT[] = { F,F,F,T };
    uint8_t   u8_10[]  = { 1,0 };
    uint8_t   u8_00[]  = { 0,0 };

    // Create a parser and initialize with config
    Parser *p = om->parser_get(0,4)->ingress();
    ASSERT_TRUE(p != NULL);
    p->reset();

    // Setup a priority map that does nothing
    p->set_identity_priority_map();

    // ENTRY 0:
    // Raw packet header - strip off Eth header (*no* VLAN) - stash DA/SA/ETH_TYPE
    p->set_early_action(255,
                        0, F, F,   // Counter load src, IMM_VAL, LOAD
                        F, 14,     // DONE, shift_amount (get rid Ether hdrs)
                        14,15,12,  // field8_1 off, field8_0 off, field16 off
                        T,T,T,     // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, 1);  // next_state_mask, next_state

    uint8_t  a0_u8_src[]  = {0,0x11,0x22,0x0F};
    uint16_t a0_u8_dst[]  = {PHV8_0,PHV8_1,PHV8_2,IP4_ERR}; // Put imm value 15 (0xF) into IP4_ERR
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
    //
    // Also calculate/verify IP checksum - look at lower 20 bytes - set err-bit on IP4_ERR PHV
    //                 ROTL_F    SWAP        MASK  ROTR_P  ADD DST_BIT  FINAL   DST_PHV   UPD? RESID? START?
    p->set_checksum(1, 0x0000, 0x0000, 0x000FFFFF,  false,   0,     7,  true, IP4_ERR, true, false,  true);

    p->set_early_action(254,
                        0, F, F,   // Counter load src, IMM_VAL, LOAD
                        F, 20,     // DONE, shift_amount (get rid IP)
                        11,10,8,   // field8_1 off, field8_0 off, field16 off
                        T,T,T,     // field8_1 LOAD, field8_0 LOAD, field16 LOAD
                        0xFF, 2);  // next_state_mask, next_state

    // NB. XXX: ActionRam[254] must have phv_8b_dst_3 active or we'll see an error
    uint16_t a1_u8_ext3   = (jbay) ?NoX :IP4_ERR;
    uint8_t  a1_u8_src[]  = {0,8,9,0};
    uint16_t a1_u8_dst[]  = {IP4_HL,IP4_TTL,IP4_PROTO,a1_u8_ext3}; // XXX: extract3 ignored here
    uint8_t  a1_u16_src[] = {2,4,6,10};
    uint16_t a1_u16_dst[] = {IP4_LEN,IP4_ID,IP4_FRAG,IP4_CKSM};
    uint8_t  a1_u32_src[] = {12,16,0,0};
    uint16_t a1_u32_dst[] = {IP4_SRC,IP4_DST,NoX,NoX};

    p->set_action(254,
                  F, 0, b_TF, u8_10,                       // adj  RESET, adj_inc, ***** ENABLE, checksum_addr *****
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

    // Setup parser so checksum errors (kErrCsum|kErrCsumConfig) get reported via PHV
    p->set_perr_phv_mask(0x600);


    // Now define some packets to lookup - we're trying to create something like this
    //                    <   DA           SA      ET I4 C LN  ID  OF  T P CK   SRC     DST     SP  DP>
    //                    <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST   ><SP><DP>
    //const char *pktstr1 = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A44556611881199";

    // Create an Ethernet header
    Crafter::Ethernet eth_header;
    eth_header.SetDestinationMAC("08:00:22:AA:BB:CC");
    eth_header.SetSourceMAC("08:00:11:DD:EE:FF");
    //eth_header.SetType(0x0800);

    // Create an IP header
    Crafter::IP ip_header;
    /* Set the Source and Destination IP address */
    //ip_header.SetVersion(4);
    ip_header.SetSourceIP(std::string("10.17.34.51"));
    ip_header.SetDestinationIP(std::string("10.68.85.102"));
    //ip_header.SetTotalLength(256);

    // Create a TCP header
    Crafter::TCP tcp_header;
    tcp_header.SetSrcPort(0x1188);
    tcp_header.SetDstPort(0x1199);
    //tcp_header.SetSeqNumber(Crafter::RNG32());
    //tcp_header.SetFlags(Crafter::TCP::SYN);

    // A raw layer, this could be any array of bytes or chars
    Crafter::RawLayer payload("SomeMoreBobbins");

    // Create a packet
    Crafter::Packet the_packet = eth_header / ip_header / tcp_header / payload;

    // Send it so we get checksums/lengths filled in
    // GetData does this too!
    //the_packet.Send("");

    // Now create a RMT flavour packet from Crafter packet
    uint8_t pkt_data[9999];
    size_t  pkt_len = the_packet.GetData(pkt_data);
    Packet *pkt1 = om->pkt_create(pkt_data, pkt_len);

    // Switch on some debug
    //om->update_log_flags(NON,NON,NON,NON,NON,ALL,ALL);

    // Parse using parser/config we setup above
    Phv *phv1 = p->parse(pkt1, 0);
    ASSERT_TRUE(phv1 != NULL);

    // Check PHV setup as we expect - TCP ports etc
    EXPECT_EQ(0x1188u, phv1->get_p(Phv::make_word_p(4,7)));
    EXPECT_EQ(0x1199u, phv1->get_p(Phv::make_word_p(4,8)));
    EXPECT_EQ(0x11u, phv1->get_p(Phv::make_word_p(3,1)));
    EXPECT_EQ(0x22u, phv1->get_p(Phv::make_word_p(3,2)));
    //EXPECT_EQ(0x33u, phv1->get_p(Phv::make_word_p(3,3)));
    EXPECT_EQ(0xFu, phv1->get_p(IP4_ERR)); // Original value 0xF => no csum error

    // Tone down debug
    om->update_log_flags(NON,NON,NON,NON,NON,FEW,FEW);

    // Cleanup
    if (crft_print) RMT_UT_LOG_INFO("Delete pkt1\n");
    om->pkt_delete(pkt1);
  }

}
