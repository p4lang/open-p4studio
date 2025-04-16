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

#include <rmt-object-manager.h>
#include <packet.h>
#include <phv.h>
#include <deparser-block.h>
#include <deparser-reg.h>
#include <model_core/model.h>
#include <port.h>
#include <iostream>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

// Only test MGID2/PipeVec extract on Tofino*
#if MCN_TEST(MODEL_CHIP_NAMESPACE,tofino)
#define DO_TEST_MGID2_EXTRACTION
#define DO_TEST_MULTICAST_PIPE_VECTOR_EXTRACTION
#endif
#if MCN_TEST(MODEL_CHIP_NAMESPACE,tofinoB0)
#define DO_TEST_MGID2_EXTRACTION
#define DO_TEST_MULTICAST_PIPE_VECTOR_EXTRACTION
#endif


  bool dprs_print = false;
  bool dprs_printf = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;

  constexpr int PKT_PORT = 5;

  TEST(BFN_TEST_NAME(DeparseTest),TEST_HEADER_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    if (dprs_print) RMT_UT_LOG_INFO("test_deparse_using_config()\n");
    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    RmtObjectManager *om = tu.get_objmgr();
    tu.set_debug(false);
    ASSERT_TRUE(om != nullptr);
    //                    <   DA           SA      ET I4 C LN  ID  OF  T P CK   SRC     DST     SP  DP>
    //                    <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST   ><SP><DP>
    const char *pktstr = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A44556611881199";

    // Create PHV
    Phv* phv = om->phv_create();
    ASSERT_TRUE(phv != nullptr);
    for (int i=0; i<Phv::kWordsMax; i++) {
      phv->set(i,0x0u);
    }
    // Packet Header bytes
    phv->set(Phv::make_word(0,10), 0xBAADDAADu);
    phv->set(Phv::make_word(0,11), 0xDEADBEEFu);
    phv->set(Phv::make_word(0,12), 0xC001C0DEu);

    phv->set(Phv::make_word(6,13), 0x1122u);
    phv->set(Phv::make_word(6,14), 0x3344u);
    phv->set(Phv::make_word(6,15), 0x5566u);
    phv->set(Phv::make_word(6,16), 0x7788u);

    phv->set(Phv::make_word(4,7), 0x99u);
    phv->set(Phv::make_word(4,8), 0xAAu);
    phv->set(Phv::make_word(4,9), 0xBBu);
    phv->set(Phv::make_word(4,10), 0xCCu);
    phv->set(Phv::make_word(4,21), 0xDDu);
    phv->set(Phv::make_word(4,22), 0xEEu);
    phv->set(Phv::make_word(4,23), 0xFFu);
    phv->set(Phv::make_word(4,24), 0x00u);

    phv->set(Phv::make_word(5,11), 0xABu);
    phv->set(Phv::make_word(5,12), 0xCDu);
    phv->set(Phv::make_word(2,12), 0xEFu);

    // POV inside PHV
    phv->set(Phv::make_word(6,2), 0x1111u);

    // Create a deparser
    DeparserBlock *deparser_block = om->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);
    deparser_block->Reset();
    auto deparser = deparser_block->ingress();
    ASSERT_TRUE(deparser != nullptr);

    // POV_POSITION
    Dprsr_pov_position_r temp; // a wide register
    for (int i=0; i<32; i++)
      setp_dprsr_pov_position_r_phvs(&temp,i,Phv::make_word(3,1) /*PHV8_1*/);
    setp_dprsr_pov_position_r_phvs(&temp, 2, Phv::make_word(6,2) /*PHV8_1*/);
    setp_dprsr_pov_position_r_phvs(&temp, 3, Phv::make_word(6,2) /*PHV8_1*/);

    //auto& pov_pos_reg = TestUtil::kTofinoPtr->pipes[pipe].deparser.inp.iir.main_i.pov;
    auto& pov_pos_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.pov;
    tu.OutWord( &pov_pos_reg.pov_0_8 , temp.pov_0_8 );
    tu.OutWord( &pov_pos_reg.pov_1_8 , temp.pov_1_8 );
    tu.OutWord( &pov_pos_reg.pov_2_8 , temp.pov_2_8 );
    tu.OutWord( &pov_pos_reg.pov_3_8 , temp.pov_3_8 );
    tu.OutWord( &pov_pos_reg.pov_4_8 , temp.pov_4_8 );
    tu.OutWord( &pov_pos_reg.pov_5_8 , temp.pov_5_8 );
    tu.OutWord( &pov_pos_reg.pov_6_8 , temp.pov_6_8 );
    tu.OutWord( &pov_pos_reg.pov_7_8 , temp.pov_7_8 );

    // Set a default egress unicast port.
    tu.deparser_set_egress_unicast_port_info(pipe, 0, true, true, 0x01);

    // CSUM_CFG
    for (int i=0; i<Deparser::kNumChecksumEngines; i++) {
      for (int csum_entry_idx=0;
           csum_entry_idx < Deparser::kNumPHVChecksumCfgEntries;
           csum_entry_idx++) {
        tu.deparser_set_csum_cfg_entry(pipe, i, csum_entry_idx, false, true, true);
      }
      for (int csum_entry_idx=0;
           csum_entry_idx < Deparser::kNumTagalongPHVChecksumCfgEntries;
           csum_entry_idx++) {
        tu.deparser_set_tphv_csum_cfg_entry(pipe, i, csum_entry_idx,
                                            false, true, true);
      }
    }
    // Checksum engine mappings:
    // 17 : Phv::make_word(5,11)
    // 15 : Phv::make_word(5,12)
    // 87 : Phv::make_word(2,12)
    tu.deparser_set_csum_cfg_entry(pipe, 0, 17, false, true, false);
    tu.deparser_set_csum_cfg_entry(pipe, 3, 15, true, false, true);
    tu.deparser_set_csum_cfg_entry(pipe, 5, 87, true, false, true);
    tu.deparser_set_tphv_csum_cfg_entry(pipe, 0, 144, false, false, false);
    tu.deparser_set_tphv_csum_cfg_entry(pipe, 1, 147, false, false, false);
    tu.deparser_set_tphv_csum_cfg_entry(pipe, 5, 149, false, false, false);

    // FIELD_DICTIONARY_ENTRY
    for (int i=0; i<Deparser::kNumFieldDictionaryEntries; i++) {
      tu.deparser_set_field_dictionary_entry(pipe, i, 0, 0, 0, 0, 0, 0, 0, 0);
    }

    tu.deparser_set_field_dictionary_entry(pipe, 0, 1, Phv::make_word(0,10),
        Phv::make_word(0,10), Phv::make_word(0,10), Phv::make_word(0,10), 16, 4,
        0x0F);
    tu.deparser_set_field_dictionary_entry(pipe, 1, 1, Phv::make_word(6,13),
        Phv::make_word(6,13), Phv::make_word(0,11), Phv::make_word(0,11), 20, 4,
        0x0F);
    tu.deparser_set_field_dictionary_entry(pipe, 7, 1, Phv::make_word(0,11),
        Phv::make_word(0,11), Phv::make_word(0,12), Phv::make_word(0,12), 24, 4,
        0x0F);
    tu.deparser_set_field_dictionary_entry(pipe, 8, 1, Phv::make_word(0,12),
        Phv::make_word(0,12), Phv::make_word(6,14), Phv::make_word(6,14), 24, 4,
        0x0F);
    tu.deparser_set_field_dictionary_entry(pipe, 180, 1, Phv::make_word(6,15),
        Phv::make_word(6,15), Phv::make_word(4,7), Phv::make_word(6,16), 24, 4,
        0x0F);
    tu.deparser_set_field_dictionary_entry(pipe, 181, 1, Phv::make_word(6,16),
        Phv::make_word(4,8), Phv::make_word(4,9), Phv::make_word(4,10), 24, 4,
        0x0F);

    // Read from Checksum engine
    tu.deparser_set_field_dictionary_entry(pipe, 189, 1, 224,
        224, 0, 0, 24, 2,
        0x0F);
    tu.deparser_set_field_dictionary_entry(pipe, 190, 1, 225,
        227, 0, 0, 24, 2,
        0x0F);
    tu.deparser_set_field_dictionary_entry(pipe, 191, 1, 229,
        229, 0, 0, 24, 1,
        0x0F);

    // Now see if we can Deparse packets
    if (dprs_print) RMT_UT_LOG_INFO("Deparse packet\n");
    {
      Packet *pkt = om->pkt_create(pktstr);
      ASSERT_TRUE(pkt != nullptr);
      pkt->set_ingress();
      pkt->set_port(new Port(om, PKT_PORT));
      phv->set_packet(pkt);

      auto new_pkt = deparser_block->DeparseIngress(*phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
      std::unique_ptr<uint8_t[]> new_pkt_buf(new uint8_t[new_pkt->len()]);
      new_pkt->get_buf(new_pkt_buf.get(), 0, new_pkt->len());

      const char *expected_pktstr = "BAADDAAD1122DEADBEEFC001C0DE33445566007788000000FF5432FF10080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A44556611881199";
      const int expected_pkt_len = strlen(expected_pktstr) / 2;
      std::unique_ptr<uint8_t[]> expected_pkt_buf(new uint8_t[expected_pkt_len]);
      model_common::Util::hexmakebuf(expected_pktstr, expected_pkt_len * 2, expected_pkt_buf.get());

      ASSERT_TRUE(expected_pkt_len == new_pkt->len());
      ASSERT_FALSE(memcmp(new_pkt_buf.get(), expected_pkt_buf.get(),
                   expected_pkt_len));
    }

    {
      Packet *pkt = om->pkt_create(pktstr);
      ASSERT_TRUE(pkt != nullptr);
      pkt->set_ingress();
      pkt->set_orig_hdr_len(2);
      pkt->set_port(new Port(om, PKT_PORT));
      phv->set_packet(pkt);

      auto new_pkt = deparser_block->DeparseIngress(*phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
      std::unique_ptr<uint8_t[]> new_pkt_buf(new uint8_t[new_pkt->len()]);
      new_pkt->get_buf(new_pkt_buf.get(), 0, new_pkt->len());

      const char *expected_pktstr = "BAADDAAD1122DEADBEEFC001C0DE33445566007788000000FF5432FF1022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A44556611881199";
      const int expected_pkt_len = strlen(expected_pktstr) / 2;
      std::unique_ptr<uint8_t[]> expected_pkt_buf(new uint8_t[expected_pkt_len]);
      model_common::Util::hexmakebuf(expected_pktstr, expected_pkt_len * 2, expected_pkt_buf.get());

      ASSERT_TRUE(expected_pkt_len == new_pkt->len());
      ASSERT_FALSE(memcmp(new_pkt_buf.get(), expected_pkt_buf.get(), expected_pkt_len));
    }
  }

  const char default_pktstr[] = "44556611881199AA";
  constexpr int default_pktstr_len = sizeof(default_pktstr) / 2;

  static void
  create_packet(TestUtil *tu, Phv *phv, const bool is_egress) {
    const char *pktstr = default_pktstr;

    // Now a Packet on heap
    Packet *pkt = tu->get_objmgr()->pkt_create(pktstr);
    assert(pkt != nullptr);
    if (is_egress) {
      pkt->set_egress();
    }
    else {
      pkt->set_ingress();
    }

    pkt->set_port(new Port(tu->get_objmgr(), PKT_PORT));

    // Set packet in PHV.
    assert(phv != nullptr);
    phv->set_packet(pkt);
  }

  static void
  compare_headers(Packet *new_pkt, std::vector<uint8_t>expected_header) {
    std::unique_ptr<uint8_t[]> new_pkt_buf(new uint8_t[new_pkt->len()]);
    new_pkt->get_buf(new_pkt_buf.get(), 0, new_pkt->len());

    const int expected_pkt_len = expected_header.size() + default_pktstr_len;
    ASSERT_TRUE(expected_pkt_len == new_pkt->len());
    ASSERT_FALSE(memcmp(new_pkt_buf.get(), &expected_header.at(0),
                        expected_header.size()));
    std::unique_ptr<uint8_t[]> default_pktbuf(new uint8_t[default_pktstr_len]);
    model_common::Util::hexmakebuf(default_pktstr, default_pktstr_len * 2,
                                   default_pktbuf.get());
    ASSERT_FALSE(memcmp(new_pkt_buf.get() + expected_header.size(),
                        default_pktbuf.get(), default_pktstr_len));
  }

  TestUtil*
  setup_deparser(int chip, int pipe, Phv *phv, int stage, bool is_egress=false) {
    TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), chip, pipe, stage);

    create_packet(tu, phv, is_egress);

    assert(phv != nullptr);
    for (int i=0; i<Phv::kWordsMax; i++) {
      phv->set(i,0x0u);
    }

    // Set a default egress unicast port.
    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    assert(deparser_block != nullptr);
    deparser_block->Reset();
    tu->deparser_set_egress_unicast_port_info(pipe, 0, true, true, 0x01);

    return tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_HEADER_EXTRACTION2) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    // Create a deparser
    deparser_block->Reset();
    auto deparser = deparser_block->ingress();
    ASSERT_TRUE(deparser != nullptr);

    for (int i=0; i<Phv::kWordsMax; i++) {
      phv.clobber(i,0);
    }

    // POV inside PHV
    phv.clobber(Phv::make_word(6,2), 0x1111u);
    phv.clobber(Phv::make_word(6,1), 0x0000u);

    // POV_POSITION
    Dprsr_pov_position_r temp; // a wide register
    for (int i=0; i<32; i++)
      setp_dprsr_pov_position_r_phvs(&temp,i,Phv::make_word(6,1) /*PHV8_1*/);
    setp_dprsr_pov_position_r_phvs(&temp, 2, Phv::make_word(6,2) /*PHV8_1*/);
    setp_dprsr_pov_position_r_phvs(&temp, 3, Phv::make_word(6,2) /*PHV8_1*/);

    //auto& pov_pos_reg = TestUtil::kTofinoPtr->pipes[pipe].deparser.inp.iir.main_i.pov;
    auto& pov_pos_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.pov;
    tu->OutWord( &pov_pos_reg.pov_0_8 , temp.pov_0_8 );
    tu->OutWord( &pov_pos_reg.pov_1_8 , temp.pov_1_8 );
    tu->OutWord( &pov_pos_reg.pov_2_8 , temp.pov_2_8 );
    tu->OutWord( &pov_pos_reg.pov_3_8 , temp.pov_3_8 );
    tu->OutWord( &pov_pos_reg.pov_4_8 , temp.pov_4_8 );
    tu->OutWord( &pov_pos_reg.pov_5_8 , temp.pov_5_8 );
    tu->OutWord( &pov_pos_reg.pov_6_8 , temp.pov_6_8 );
    tu->OutWord( &pov_pos_reg.pov_7_8 , temp.pov_7_8 );

    // Set a default egress unicast port.
    tu->deparser_set_egress_unicast_port_info(pipe, 0, true, true, 0x01);

    // CSUM_CFG
    for (int i=0; i<Deparser::kNumChecksumEngines; i++) {
      for (int csum_entry_idx=0;
           csum_entry_idx < Deparser::kNumPHVChecksumCfgEntries;
           csum_entry_idx++) {
        tu->deparser_set_csum_cfg_entry(pipe, i, csum_entry_idx, false, true, true);
      }
    }

    // FIELD_DICTIONARY_ENTRY
    for (int i=0; i<Deparser::kNumFieldDictionaryEntries; i++) {
      tu->deparser_set_field_dictionary_entry(pipe, i, 0, 0, 0, 0, 0, 0, 0, 0);
    }

    std::vector<uint8_t> expected_header;
    for (int i=Phv::make_word(1,0); i<=Phv::make_word(1,31); ++i) {
      uint32_t value = 0;
      for (int j = 0; j < 4; ++j) {
        value = (value << 8);
        value |= ((uint32_t)(i + j));
        expected_header.push_back((uint8_t)(i + j));
      }
      phv.clobber(i,value);
      tu->deparser_set_field_dictionary_entry(pipe, i, 1, i, i, i, i, 16, 4,
                                              0x0F);
    }

    // Now see if we can Deparse packets
    {
      auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt,
                                                    &resubmit_pkt,&packet_gen_metadata);
      std::unique_ptr<uint8_t[]> new_pkt_buf(new uint8_t[new_pkt->len()]);
      new_pkt->get_buf(new_pkt_buf.get(), 0, new_pkt->len());

      const int expected_pkt_len = expected_header.size() + default_pktstr_len;
      ASSERT_TRUE(expected_pkt_len == new_pkt->len());
      ASSERT_FALSE(memcmp(new_pkt_buf.get(), &expected_header.at(0),
                          expected_header.size()));
      std::unique_ptr<uint8_t[]> default_pktbuf(new uint8_t[default_pktstr_len]);
      model_common::Util::hexmakebuf(default_pktstr, default_pktstr_len * 2,
                                     default_pktbuf.get());
      ASSERT_FALSE(memcmp(new_pkt_buf.get() + expected_header.size(),
                          default_pktbuf.get(), default_pktstr_len));
      // Reset the ingress packet in the PHV.
      tu->get_objmgr()->pkt_delete(new_pkt);
      create_packet(tu, &phv, false);
    }

    expected_header.clear();
    // Put 236B in the FDE. We need to work around 16 PHVs tied to egress.
    for (int i=0; i<=Phv::make_word(0,15); ++i) {
      uint32_t value = 0;
      for (int j = 0; j < 4; ++j) {
        value = (value << 8);
        value |= ((uint32_t)(i+j));
        expected_header.push_back((uint8_t)value);
      }
      phv.clobber(i,value);
      tu->deparser_set_field_dictionary_entry(pipe, i, 1, i, i, i, i, 16, 4,
                                              0x0F);
    }
    for (int i=Phv::make_word(5,0); i<=Phv::make_word(5,31); ++i) {
      uint32_t value = 0;
      for (int j = 0; j < 2; ++j) {
        value = (value << 8);
        value |= ((uint32_t)(i+j));
        expected_header.push_back((uint8_t)value);
      }
      phv.clobber(i,value);
    }
    for (int i=0; i < 16; ++i) {
      tu->deparser_set_field_dictionary_entry(pipe, Phv::make_word(0,16) + i,
                                              1, Phv::make_word(5,0) + (2 * i),
                                              Phv::make_word(5,0) + (2 * i),
                                              Phv::make_word(5,0) + (2 * i) + 1,
                                              Phv::make_word(5,0) + (2 * i) + 1,
                                              16, 4, 0x0F);
    }
    for (int i=Phv::make_word(1,0); i<=Phv::make_word(1,26); ++i) {
      uint32_t value = 0;
      for (int j = 0; j < 4; ++j) {
        value = (value << 8);
        value |= ((uint32_t)(i+j));
        expected_header.push_back((uint8_t)value);
      }
      phv.clobber(i,value);
      tu->deparser_set_field_dictionary_entry(pipe, i, 1, i, i, i, i, 16, 4,
                                              0x0F);
    }
    // Setup FDE so that a PHV straddles the 240B boundary in the header.
    phv.clobber(Phv::make_word(2,0),0x42);
    phv.clobber(Phv::make_word(1,27),0x43444546);
    expected_header.push_back((uint8_t)0x42);
    expected_header.push_back((uint8_t)0x43);
    expected_header.push_back((uint8_t)0x44);
    expected_header.push_back((uint8_t)0x45);
    expected_header.push_back((uint8_t)0x46);
    tu->deparser_set_field_dictionary_entry(pipe, Phv::make_word(1,27), 1,
                                            Phv::make_word(2,0),
                                            Phv::make_word(1,27),
                                            Phv::make_word(1,27),
                                            Phv::make_word(1,27),
                                            16, 4, 0x0F);
    tu->deparser_set_field_dictionary_entry(pipe, Phv::make_word(1,28), 1,
                                            Phv::make_word(1,27), 0,
                                            Phv::make_word(1,27),
                                            Phv::make_word(1,27), 16, 1, 0x0F);
    // Invalidate the FDEs that we set for the previous packet.
    for (int i=Phv::make_word(1,29); i<=Phv::make_word(1,31); ++i) {
      tu->deparser_set_field_dictionary_entry(pipe, i, 0, i, i, i, i, 16, 4,
                                              0x0F);
    }

    // Now see if we can Deparse packets
    {
      auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt,
                                                    &resubmit_pkt,&packet_gen_metadata);
      std::unique_ptr<uint8_t[]> new_pkt_buf(new uint8_t[new_pkt->len()]);
      new_pkt->get_buf(new_pkt_buf.get(), 0, new_pkt->len());

      const int expected_pkt_len = expected_header.size() + default_pktstr_len;
      ASSERT_TRUE(expected_pkt_len == new_pkt->len());
      ASSERT_FALSE(memcmp(new_pkt_buf.get(), &expected_header.at(0),
                          expected_header.size()));
      std::unique_ptr<uint8_t[]> default_pktbuf(new uint8_t[default_pktstr_len]);
      model_common::Util::hexmakebuf(default_pktstr, default_pktstr_len * 2,
                                     default_pktbuf.get());
      ASSERT_FALSE(memcmp(new_pkt_buf.get() + expected_header.size(),
                          default_pktbuf.get(), default_pktstr_len));
    }
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_HEADER_EXTRACTION4) {
    int pipe = 1;
    for (int seed = 0; seed < 8; seed++) {
      Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
      LearnQuantumType lq;
      TestUtil *tu = nullptr;
      Phv phv(nullptr, nullptr);
      tu = setup_deparser(202, pipe, &phv, 0);
      ASSERT_TRUE(nullptr != tu);
      std::vector<uint8_t> expected_header;

      DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
      PacketGenMetadata* packet_gen_metadata = nullptr;
      // Create a deparser
      deparser_block->Reset();
      auto deparser = deparser_block->ingress();
      ASSERT_TRUE(deparser != nullptr);

      for (int i=0; i<Phv::kWordsMax; i++) {
        phv.clobber(i,0);
      }

      // POV inside PHV
      phv.clobber(Phv::make_word(6,2), 0x1111u);
      phv.clobber(Phv::make_word(6,1), 0x0000u);

      // POV_POSITION
      Dprsr_pov_position_r temp; // a wide register
      for (int i=0; i<32; i++)
        setp_dprsr_pov_position_r_phvs(&temp,i,Phv::make_word(6,1) /*PHV8_1*/);
      setp_dprsr_pov_position_r_phvs(&temp, 2, Phv::make_word(6,2) /*PHV8_1*/);
      setp_dprsr_pov_position_r_phvs(&temp, 3, Phv::make_word(6,2) /*PHV8_1*/);

      //auto& pov_pos_reg = TestUtil::kTofinoPtr->pipes[pipe].deparser.inp.iir.main_i.pov;
      auto& pov_pos_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.pov;
      tu->OutWord( &pov_pos_reg.pov_0_8 , temp.pov_0_8 );
      tu->OutWord( &pov_pos_reg.pov_1_8 , temp.pov_1_8 );
      tu->OutWord( &pov_pos_reg.pov_2_8 , temp.pov_2_8 );
      tu->OutWord( &pov_pos_reg.pov_3_8 , temp.pov_3_8 );
      tu->OutWord( &pov_pos_reg.pov_4_8 , temp.pov_4_8 );
      tu->OutWord( &pov_pos_reg.pov_5_8 , temp.pov_5_8 );
      tu->OutWord( &pov_pos_reg.pov_6_8 , temp.pov_6_8 );
      tu->OutWord( &pov_pos_reg.pov_7_8 , temp.pov_7_8 );

      // Set a default egress unicast port.
      tu->deparser_set_egress_unicast_port_info(pipe, 0, true, true, 0x01);

      // CSUM_CFG
      for (int i=0; i<Deparser::kNumChecksumEngines; i++) {
        for (int csum_entry_idx=0;
             csum_entry_idx < Deparser::kNumPHVChecksumCfgEntries;
             csum_entry_idx++) {
          tu->deparser_set_csum_cfg_entry(pipe, i, csum_entry_idx, false, true, true);
        }
      }

      // FIELD_DICTIONARY_ENTRY
      for (int i=0; i<Deparser::kNumFieldDictionaryEntries; i++) {
        tu->deparser_set_field_dictionary_entry(pipe, i, 0, 0, 0, 0, 0, 0, 0, 0);
      }

      std::default_random_engine generator(seed);
      std::uniform_int_distribution<int> distribution(0, 191);
      std::list<int> fdes;
      for (int i = 0; i < 64; ++i) {
          int new_fde;
          do {
            new_fde = distribution(generator);
          } while (std::find(fdes.begin(), fdes.end(), new_fde) != fdes.end());
          fdes.push_back(new_fde);
      }
      fdes.sort();
      std::deque<int> phv_indices;
      std::uniform_int_distribution<int> distribution2(Phv::make_word(0,2),
                                                       Phv::make_word(6,15));
      std::set<int> egress_phvs = {
        Phv::make_word(0,16), Phv::make_word(0,17), Phv::make_word(0,18),
        Phv::make_word(0,19), Phv::make_word(0,20), Phv::make_word(0,21),
        Phv::make_word(0,22), Phv::make_word(0,23), Phv::make_word(0,24),
        Phv::make_word(0,25), Phv::make_word(0,26), Phv::make_word(0,27),
        Phv::make_word(0,28), Phv::make_word(0,29), Phv::make_word(0,30),
        Phv::make_word(0,31),
        Phv::make_word(2,16), Phv::make_word(2,17), Phv::make_word(2,18),
        Phv::make_word(2,19), Phv::make_word(2,20), Phv::make_word(2,21),
        Phv::make_word(2,22), Phv::make_word(2,23), Phv::make_word(2,24),
        Phv::make_word(2,25), Phv::make_word(2,26), Phv::make_word(2,27),
        Phv::make_word(2,28), Phv::make_word(2,29), Phv::make_word(2,30),
        Phv::make_word(2,31),
        Phv::make_word(4,16), Phv::make_word(4,17), Phv::make_word(4,18),
        Phv::make_word(4,19), Phv::make_word(4,20), Phv::make_word(4,21),
        Phv::make_word(4,22), Phv::make_word(4,23), Phv::make_word(4,24),
        Phv::make_word(4,25), Phv::make_word(4,26), Phv::make_word(4,27),
        Phv::make_word(4,28), Phv::make_word(4,29), Phv::make_word(4,30),
        Phv::make_word(4,31) };
      while (phv_indices.size() < 58 * 4) {
        int phv_idx;
        do {
          phv_idx = distribution2(generator);
        } while (std::find(phv_indices.begin(), phv_indices.end(),
                           phv_idx) != phv_indices.end() ||
                 egress_phvs.find(phv_idx) != egress_phvs.end());
        for (int i = 0; i < Phv::which_width_in_bytes(phv_idx); ++i) {
          phv_indices.push_back(phv_idx);
        }
      }
      auto fde_iter = fdes.begin();
      for (int i = 0; i < 58; ++i) {
        auto phv_idx0 = phv_indices.at(0);
        auto phv_idx1 = phv_indices.at(1);
        auto phv_idx2 = phv_indices.at(2);
        auto phv_idx3 = phv_indices.at(3);
        phv_indices.pop_front();
        phv_indices.pop_front();
        phv_indices.pop_front();
        phv_indices.pop_front();
        tu->deparser_set_field_dictionary_entry(pipe, *fde_iter, 1, phv_idx0,
                                                phv_idx1, phv_idx2, phv_idx3,
                                                16, 4, 0x0F);
        std::advance(fde_iter, 1);
      }
      int next_phv_idx = Phv::make_word(6,16);
      std::deque<int> fde58_phv_idx;
      while ((fde58_phv_idx.size() < 2) || (!phv_indices.empty())) {
        if (!phv_indices.empty()) {
          fde58_phv_idx.push_back(phv_indices.front());
          phv_indices.pop_front();
        }
        else {
          // Asumming the next_phv_idx always contains 16b container indices.
          fde58_phv_idx.push_back(next_phv_idx);
          fde58_phv_idx.push_back(next_phv_idx);
          ++next_phv_idx;
        }
      }
      auto phv_idx2 = -1;
      if (fde58_phv_idx.size() > 2) phv_idx2 = fde58_phv_idx.at(2);
      tu->deparser_set_field_dictionary_entry(pipe, *fde_iter, 1,
                                              fde58_phv_idx.at(0),
                                              fde58_phv_idx.at(1), phv_idx2,
                                              -1, 16, fde58_phv_idx.size(), 0x0F);
      std::advance(fde_iter, 1);
      tu->deparser_set_field_dictionary_entry(pipe, *fde_iter, 1,
                                              Phv::make_word(2,0),
                                              -1, -1, -1, 16, 1, 0x0F);
      std::advance(fde_iter, 1);
      tu->deparser_set_field_dictionary_entry(pipe, *fde_iter, 1,
                                              next_phv_idx,
                                              -1, -1, -1, 16, 1, 0x0F);
      std::advance(fde_iter, 1);
      tu->deparser_set_field_dictionary_entry(pipe, *fde_iter, 1,
                                              next_phv_idx,
                                              -1, -1, -1, 16, 1, 0x0F);
      std::advance(fde_iter, 1);
      tu->deparser_set_field_dictionary_entry(pipe, *fde_iter, 1,
                                              Phv::make_word(0,0),
                                              Phv::make_word(0,0),
                                              Phv::make_word(0,0),
                                              Phv::make_word(0,0),
                                              16, 4, 0x0F);
      std::advance(fde_iter, 1);
      tu->deparser_set_field_dictionary_entry(pipe, *fde_iter, 1,
                                              Phv::make_word(0,1),
                                              Phv::make_word(0,1),
                                              Phv::make_word(0,1),
                                              Phv::make_word(0,1),
                                              16, 4, 0x0F);

      // Now see if we can Deparse packets
      {
        auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt,
                                                      &resubmit_pkt,&packet_gen_metadata);
      // expected header len = (4 * 58) + fde58_phv_idx.size + 1 + 1 + 1 + 4 + 4
      const int expected_pkt_len = 243 + fde58_phv_idx.size() + default_pktstr_len;
      EXPECT_EQ(expected_pkt_len, new_pkt->len()) << " Failed when using seed " << seed;
        //compare_headers(new_pkt, expected_header);
      }
    }
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_HEADER_EXTRACTION3) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);
    std::vector<uint8_t> expected_header;

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    // Create a deparser
    deparser_block->Reset();
    auto deparser = deparser_block->ingress();
    ASSERT_TRUE(deparser != nullptr);

    for (int i=0; i<Phv::kWordsMax; i++) {
      phv.clobber(i,0);
    }

    // POV inside PHV
    phv.clobber(Phv::make_word(6,2), 0x1111u);
    phv.clobber(Phv::make_word(6,1), 0x0000u);
    phv.clobber(Phv::make_word(0,0), 0xAAABACAD);
    phv.clobber(Phv::make_word(1,27), 0xDEADBEEF);
    expected_header.push_back((uint8_t)0xAA);
    expected_header.push_back((uint8_t)0xAB);
    expected_header.push_back((uint8_t)0xAC);
    expected_header.push_back((uint8_t)0xAD);
    expected_header.push_back((uint8_t)0xDE);
    expected_header.push_back((uint8_t)0xAD);
    expected_header.push_back((uint8_t)0xBE);
    expected_header.push_back((uint8_t)0xEF);

    // POV_POSITION
    Dprsr_pov_position_r temp; // a wide register
    for (int i=0; i<32; i++)
      setp_dprsr_pov_position_r_phvs(&temp,i,Phv::make_word(6,1) /*PHV8_1*/);
    setp_dprsr_pov_position_r_phvs(&temp, 2, Phv::make_word(6,2) /*PHV8_1*/);
    setp_dprsr_pov_position_r_phvs(&temp, 3, Phv::make_word(6,2) /*PHV8_1*/);

    //auto& pov_pos_reg = TestUtil::kTofinoPtr->pipes[pipe].deparser.inp.iir.main_i.pov;
    auto& pov_pos_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.pov;
    tu->OutWord( &pov_pos_reg.pov_0_8 , temp.pov_0_8 );
    tu->OutWord( &pov_pos_reg.pov_1_8 , temp.pov_1_8 );
    tu->OutWord( &pov_pos_reg.pov_2_8 , temp.pov_2_8 );
    tu->OutWord( &pov_pos_reg.pov_3_8 , temp.pov_3_8 );
    tu->OutWord( &pov_pos_reg.pov_4_8 , temp.pov_4_8 );
    tu->OutWord( &pov_pos_reg.pov_5_8 , temp.pov_5_8 );
    tu->OutWord( &pov_pos_reg.pov_6_8 , temp.pov_6_8 );
    tu->OutWord( &pov_pos_reg.pov_7_8 , temp.pov_7_8 );

    // Set a default egress unicast port.
    tu->deparser_set_egress_unicast_port_info(pipe, 0, true, true, 0x01);

    // CSUM_CFG
    for (int i=0; i<Deparser::kNumChecksumEngines; i++) {
      for (int csum_entry_idx=0;
           csum_entry_idx < Deparser::kNumPHVChecksumCfgEntries;
           csum_entry_idx++) {
        tu->deparser_set_csum_cfg_entry(pipe, i, csum_entry_idx, false, true, true);
      }
    }

    // FIELD_DICTIONARY_ENTRY
    for (int i=0; i<Deparser::kNumFieldDictionaryEntries; i++) {
      tu->deparser_set_field_dictionary_entry(pipe, i, 0, 0, 0, 0, 0, 0, 0, 0);
    }

    tu->deparser_set_field_dictionary_entry(pipe, 151, 1,
                                            Phv::make_word(0,0),
                                            Phv::make_word(1,27),
                                            Phv::make_word(1,27),
                                            Phv::make_word(1,27),
                                            16, 1, 0x0F);
    tu->deparser_set_field_dictionary_entry(pipe, 152, 1,
                                            Phv::make_word(0,0),
                                            Phv::make_word(1,27),
                                            Phv::make_word(1,27),
                                            Phv::make_word(1,27),
                                            16, 1, 0x0F);
    tu->deparser_set_field_dictionary_entry(pipe, 155, 1,
                                            Phv::make_word(0,0),
                                            Phv::make_word(1,27),
                                            Phv::make_word(1,27),
                                            Phv::make_word(1,27),
                                            16, 1, 0x0F);
    tu->deparser_set_field_dictionary_entry(pipe, 158, 1,
                                            Phv::make_word(0,0),
                                            Phv::make_word(1,27),
                                            Phv::make_word(1,27),
                                            Phv::make_word(1,27),
                                            16, 1, 0x0F);
    tu->deparser_set_field_dictionary_entry(pipe, 161, 1,
                                            Phv::make_word(1,27),
                                            Phv::make_word(1,27),
                                            Phv::make_word(1,27),
                                            Phv::make_word(1,27),
                                            16, 4, 0x0F);

    // Now see if we can Deparse packets
    {
      auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt,
                                                    &resubmit_pkt,&packet_gen_metadata);
      compare_headers(new_pkt, expected_header);
    }
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_COPY_TO_CPU_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    auto *cnt_i_phv_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.cnt_i_phv, 48);
    EXPECT_EQ(0u, cnt_i_phv_ingress->read());  // sanity check
    auto *cnt_i_tphv_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.cnt_i_tphv, 48);
    EXPECT_EQ(0u, cnt_i_tphv_ingress->read());  // sanity check
    auto *cnt_i_read = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_read, 48);
    EXPECT_EQ(0u, cnt_i_read->read());  // sanity check
    auto *cnt_i_discard = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_discard, 48);
    EXPECT_EQ(0u, cnt_i_discard->read());  // sanity check
    auto *cnt_pkts_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.cnt_pkts, 48);
    EXPECT_EQ(0u, cnt_pkts_ingress->read());  // sanity check
    auto *i_fwd_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_fwd_pkts, 48);
    EXPECT_EQ(0u, i_fwd_pkts->read());  // sanity check
    auto *i_disc_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_disc_pkts, 48);
    EXPECT_EQ(0u, i_disc_pkts->read());  // sanity check
    auto *i_mirr_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_mirr_pkts, 48);
    EXPECT_EQ(0u, i_mirr_pkts->read());  // sanity check
    Port *port = phv.packet()->port();
    auto *ipb_counters = tu->get_objmgr()->ipb_lookup(
        deparser_block->pipe_index(), port->ipb_index())->get_ipb_counters(
            port->ipb_chan());
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());  // sanity check
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());  // sanity check

    phv.set(Phv::make_word(4,2), (0x01u << 3));
    tu->deparser_set_copy_to_cpu_info(pipe, Phv::make_word(4,2), true, false, 3);
    tu->deparser_set_copy_to_cpu_pipe_vector(pipe, 0x0F);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->cpu_needs_copy());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x0F);
    EXPECT_EQ(1u, cnt_i_phv_ingress->read());
    EXPECT_EQ(1u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(1u, cnt_i_read->read());
    EXPECT_EQ(1u, cnt_pkts_ingress->read());
    EXPECT_EQ(1u, i_fwd_pkts->read());
    EXPECT_EQ(0u, cnt_i_discard->read());
    EXPECT_EQ(0u, i_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());
    EXPECT_EQ(1u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());

    tu->deparser_set_copy_to_cpu_info(pipe, Phv::make_word(4,2), false, false, 3);
    tu->deparser_set_copy_to_cpu_pipe_vector(pipe, 0x0F);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_FALSE(new_pkt->i2qing_metadata()->cpu_needs_copy());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x00);
    EXPECT_EQ(2u, cnt_i_phv_ingress->read());
    EXPECT_EQ(2u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(2u, cnt_i_read->read());
    EXPECT_EQ(2u, cnt_pkts_ingress->read());
    EXPECT_EQ(2u, i_fwd_pkts->read());
    EXPECT_EQ(0u, cnt_i_discard->read());
    EXPECT_EQ(0u, i_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());
    EXPECT_EQ(2u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());

    tu->deparser_set_copy_to_cpu_info(pipe, Phv::make_word(4,2), false, true, 3);
    tu->deparser_set_copy_to_cpu_pipe_vector(pipe, 0x01);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->cpu_needs_copy());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x01);
    EXPECT_EQ(3u, cnt_i_phv_ingress->read());
    EXPECT_EQ(3u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(3u, cnt_i_read->read());
    EXPECT_EQ(3u, cnt_pkts_ingress->read());
    EXPECT_EQ(3u, i_fwd_pkts->read());
    EXPECT_EQ(0u, cnt_i_discard->read());
    EXPECT_EQ(0u, i_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());
    EXPECT_EQ(3u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());

    phv.clobber(Phv::make_word(4,2), 0x07u);
    tu->deparser_set_copy_to_cpu_info(pipe, Phv::make_word(4,2), true, true, 3);
    tu->deparser_set_copy_to_cpu_pipe_vector(pipe, 0x0A);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_FALSE(new_pkt->i2qing_metadata()->cpu_needs_copy());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x00);
    EXPECT_EQ(4u, cnt_i_phv_ingress->read());
    EXPECT_EQ(4u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(4u, cnt_i_read->read());
    EXPECT_EQ(4u, cnt_pkts_ingress->read());
    EXPECT_EQ(4u, i_fwd_pkts->read());
    EXPECT_EQ(0u, cnt_i_discard->read());
    EXPECT_EQ(0u, i_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());
    EXPECT_EQ(4u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());

    delete cnt_i_phv_ingress;
    delete cnt_i_tphv_ingress;
    delete cnt_i_read;
    delete cnt_i_discard;
    delete cnt_pkts_ingress;
    delete i_fwd_pkts;
    delete i_disc_pkts;
    delete i_mirr_pkts;
    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_CT_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;

    phv.set(Phv::make_word(4,2), 0x01u);
    phv.set(Phv::make_word(4,3), 0x01u);
    tu->deparser_set_ct_disable_info(pipe, Phv::make_word(4,2), true, 0x00u);
    tu->deparser_set_ct_mcast_info(pipe, Phv::make_word(4,3), true, 0x00u);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->ct_disable_mode());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->ct_mcast_mode());

    tu->deparser_set_ct_disable_info(pipe, Phv::make_word(4,2), false, 0x00u);
    tu->deparser_set_ct_mcast_info(pipe, Phv::make_word(4,3), false, 0xFEu);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_FALSE(new_pkt->i2qing_metadata()->ct_disable_mode());
    ASSERT_FALSE(new_pkt->i2qing_metadata()->ct_mcast_mode());

    phv.clobber(Phv::make_word(4,3), 0x42u);
    tu->deparser_set_ct_disable_info(pipe, Phv::make_word(4,2), false, 0x01u);
    tu->deparser_set_ct_mcast_info(pipe, Phv::make_word(4,3), true, 0x01);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->ct_disable_mode());
    ASSERT_FALSE(new_pkt->i2qing_metadata()->ct_mcast_mode());

    phv.clobber(Phv::make_word(4,2), 0x12u);
    tu->deparser_set_ct_disable_info(pipe, Phv::make_word(4,2), true, 0x01);
    tu->deparser_set_ct_mcast_info(pipe, Phv::make_word(4,3), false, 0x01u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_FALSE(new_pkt->i2qing_metadata()->ct_disable_mode());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->ct_mcast_mode());

    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_DEFLECT_ON_DROP_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;

    phv.set(Phv::make_word(4,2), 0x01u);
    tu->deparser_set_deflect_on_drop_info(pipe, Phv::make_word(4,2), true, 0x00u);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->dod());

    tu->deparser_set_deflect_on_drop_info(pipe, Phv::make_word(4,2), false, 0x00u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_FALSE(new_pkt->i2qing_metadata()->dod());

    tu->deparser_set_deflect_on_drop_info(pipe, Phv::make_word(4,2), false, 0x01u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->dod());

    phv.clobber(Phv::make_word(4,2), 0x12u);
    tu->deparser_set_deflect_on_drop_info(pipe, Phv::make_word(4,2), true, 0x01);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_FALSE(new_pkt->i2qing_metadata()->dod());

    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_EGRESS_DEPARSER_METADATA_EXTRACTION) {
    Packet *mirror_pkt = nullptr;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0, true);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);

    tu->deparser_set_egress_unicast_port_info(pipe, 0, true, true, 0x01);
    phv.clobber(Phv::make_word(3,31), 0x00u);
    tu->deparser_set_capture_tx_ts_info(pipe, Phv::make_word(3,31), true, true);
    phv.set(Phv::make_word(1,22), 0x04u);
    tu->deparser_set_ecos_info(pipe, Phv::make_word(1,22), true, 0x03u);
    phv.set(Phv::make_word(3,30), 0x02u);
    tu->deparser_set_force_tx_error_info(pipe, Phv::make_word(3,30), true, true);

    auto *cnt_i_phv_egress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.ier.main_e.cnt_i_phv, 48);
    EXPECT_EQ(0u, cnt_i_phv_egress->read());  // sanity check
    auto *cnt_i_phv_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.cnt_i_phv, 48);
    EXPECT_EQ(0u, cnt_i_phv_ingress->read());  // sanity check
    auto *cnt_i_tphv_egress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.ier.main_e.cnt_i_tphv, 48);
    EXPECT_EQ(0u, cnt_i_tphv_egress->read());  // sanity check
    auto *cnt_i_tphv_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.cnt_i_tphv, 48);
    EXPECT_EQ(0u, cnt_i_tphv_ingress->read());  // sanity check
    auto *cnt_pkts_egress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.cnt_pkts, 48);
    EXPECT_EQ(0u, cnt_pkts_egress->read());  // sanity check
    auto *cnt_pkts_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.cnt_pkts, 48);
    EXPECT_EQ(0u, cnt_pkts_ingress->read());  // sanity check
    auto *i_fwd_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_fwd_pkts, 48);
    EXPECT_EQ(0u, i_fwd_pkts->read());  // sanity check
    auto *i_disc_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_disc_pkts, 48);
    EXPECT_EQ(0u, i_disc_pkts->read());  // sanity check
    auto *i_mirr_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_mirr_pkts, 48);
    EXPECT_EQ(0u, i_mirr_pkts->read());  // sanity check
    auto *e_fwd_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.e_fwd_pkts, 48);
    EXPECT_EQ(0u, e_fwd_pkts->read());  // sanity check
    auto *e_disc_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.e_disc_pkts, 48);
    EXPECT_EQ(0u, e_disc_pkts->read());  // sanity check
    auto *e_mirr_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.e_mirr_pkts, 48);
    EXPECT_EQ(0u, e_mirr_pkts->read());  // sanity check

    Packet *pkt = deparser_block->DeparseEgress(phv, &mirror_pkt);
    ASSERT_TRUE(nullptr != pkt);
    EXPECT_EQ(1u, cnt_i_phv_egress->read());
    EXPECT_EQ(0u, cnt_i_phv_ingress->read());
    EXPECT_EQ(1u, cnt_i_tphv_egress->read());
    EXPECT_EQ(0u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(1u, cnt_pkts_egress->read());
    EXPECT_EQ(0u, cnt_pkts_ingress->read());
    EXPECT_EQ(1u, e_fwd_pkts->read());
    EXPECT_EQ(0u, e_disc_pkts->read());
    EXPECT_EQ(0u, e_mirr_pkts->read());
    EXPECT_EQ(0u, i_fwd_pkts->read());
    EXPECT_EQ(0u, i_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());

    E2MacMetadata *e2mac_metadata = pkt->e2mac_metadata();
    ASSERT_FALSE(e2mac_metadata->capture_tx_ts());
    ASSERT_TRUE(e2mac_metadata->ecos() == 0x04u);
    ASSERT_FALSE(e2mac_metadata->force_tx_error());

    tu->deparser_set_egress_unicast_port_info(pipe, 0, true, true, 0x21u);
    phv.clobber(Phv::make_word(3,31), 0x01u);
    tu->deparser_set_capture_tx_ts_info(pipe, Phv::make_word(3,31), true, false);
    tu->deparser_set_ecos_info(pipe, Phv::make_word(1,22), false, 0x05u);
    phv.clobber(Phv::make_word(3,30), 0xFFu);
    tu->deparser_set_force_tx_error_info(pipe, Phv::make_word(3,30), false, false);
    cnt_i_tphv_egress->write_max();  // set counter to max value
    pkt = deparser_block->DeparseEgress(phv, &mirror_pkt);
    ASSERT_TRUE(nullptr != pkt);
    EXPECT_EQ(2u, cnt_i_phv_egress->read());
    EXPECT_EQ(0u, cnt_i_phv_ingress->read());
    EXPECT_EQ(0u, cnt_i_tphv_egress->read());  // counter should wrap to zero
    EXPECT_EQ(0u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(2u, cnt_pkts_egress->read());
    EXPECT_EQ(0u, cnt_pkts_ingress->read());
    EXPECT_EQ(2u, e_fwd_pkts->read());
    EXPECT_EQ(0u, e_disc_pkts->read());
    EXPECT_EQ(0u, e_mirr_pkts->read());
    EXPECT_EQ(0u, i_fwd_pkts->read());
    EXPECT_EQ(0u, i_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());

    e2mac_metadata = pkt->e2mac_metadata();
    ASSERT_TRUE(e2mac_metadata->capture_tx_ts());
    ASSERT_TRUE(e2mac_metadata->ecos() == 0x05u);
    ASSERT_FALSE(e2mac_metadata->force_tx_error());

    phv.clobber(Phv::make_word(5,1), 0x1Fu);
    tu->deparser_set_egress_unicast_port_info(pipe, Phv::make_word(5,1), true, false, 0xFF);
    phv.clobber(Phv::make_word(3,31), 0x0Fu);
    tu->deparser_set_capture_tx_ts_info(pipe, Phv::make_word(3,31), false, false);
    phv.clobber(Phv::make_word(1,22), 0x89u);
    tu->deparser_set_ecos_info(pipe, Phv::make_word(1,22), true, 0x02u);
    phv.clobber(Phv::make_word(3,30), 0x00u);
    tu->deparser_set_force_tx_error_info(pipe, Phv::make_word(3,30), false, true);
    phv.clobber(Phv::make_word(3,20), 0x00u);
    tu->deparser_set_tx_pkt_has_offsets_info(pipe, Phv::make_word(3,20), false, true);

    cnt_i_phv_egress->write_max();  // set counter to max value
    cnt_pkts_egress->write_max();  // set counter to max value
    pkt = deparser_block->DeparseEgress(phv, &mirror_pkt);
    ASSERT_TRUE(nullptr != pkt);
    EXPECT_EQ(0u, cnt_i_phv_egress->read());  // counter should wrap to zero
    EXPECT_EQ(0u, cnt_i_phv_ingress->read());
    EXPECT_EQ(1u, cnt_i_tphv_egress->read());
    EXPECT_EQ(0u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(0u, cnt_pkts_egress->read());  // counter should wrap to zero
    EXPECT_EQ(0u, cnt_pkts_ingress->read());
    EXPECT_EQ(3u, e_fwd_pkts->read());
    EXPECT_EQ(0u, e_disc_pkts->read());
    EXPECT_EQ(0u, e_mirr_pkts->read());
    EXPECT_EQ(0u, i_fwd_pkts->read());
    EXPECT_EQ(0u, i_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());

    e2mac_metadata = pkt->e2mac_metadata();
    ASSERT_FALSE(e2mac_metadata->capture_tx_ts());
    ASSERT_TRUE(e2mac_metadata->ecos() == 0x01u);
    ASSERT_TRUE(e2mac_metadata->force_tx_error());
    ASSERT_TRUE(e2mac_metadata->update_delay_on_tx());

    //tu->get_objmgr()->phv_delete(phv);
    delete cnt_i_phv_ingress;
    delete cnt_i_phv_egress;
    delete cnt_i_tphv_ingress;
    delete cnt_i_tphv_egress;
    delete cnt_pkts_egress;
    delete cnt_pkts_ingress;
    delete i_fwd_pkts;
    delete i_disc_pkts;
    delete i_mirr_pkts;
    delete e_fwd_pkts;
    delete e_disc_pkts;
    delete e_mirr_pkts;
    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_EGRESS_DEPARSER_DROP) {
    Packet *mirror_pkt = nullptr;
    int pipe = 1;
    {
      // sanity check: egress unicast port valid - expect pkt
      Phv phv(nullptr, nullptr);
      TestUtil *tu = setup_deparser(202, pipe, &phv, 0, true);
      ASSERT_TRUE(nullptr != tu);
      DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
      //deparser_block->egress()->set_log_flags(0xff);
      phv.clobber(Phv::make_word(5,1), 0x21u);  // 33 is valid port
      tu->deparser_set_egress_unicast_port_info(
        pipe,
        Phv::make_word(5, 1),  // phv word containing egress unicast port
        true,                  // egress unicast port valid
        true,                  // default egress unicast port valid
        0x0F);                 // default egress unicast port
      Packet *pkt = deparser_block->DeparseEgress(phv, &mirror_pkt);
      ASSERT_TRUE(nullptr != pkt);
      E2MacMetadata *e2mac_metadata = pkt->e2mac_metadata();
      EXPECT_TRUE(e2mac_metadata->is_egress_uc());
      EXPECT_EQ(0x21u, e2mac_metadata->egress_unicast_port());
      delete tu;
    }
    {
      // egress unicast port invalid value - expect drop
      Phv phv(nullptr, nullptr);
      TestUtil *tu = setup_deparser(202, pipe, &phv, 0, true);
      ASSERT_TRUE(nullptr != tu);
      DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
      //deparser_block->egress()->set_log_flags(0xff);
      phv.clobber(Phv::make_word(5,1), 0x50u);  // 80 is invalid port
      tu->deparser_set_egress_unicast_port_info(
        pipe,
        Phv::make_word(5, 1),  // phv word containing egress unicast port
        true,                  // egress unicast port valid
        true,                  // default egress unicast port valid
        0x0F);                 // default egress unicast port
      Packet *pkt = deparser_block->DeparseEgress(phv, &mirror_pkt);
      ASSERT_TRUE(nullptr == pkt);
      delete tu;
    }
    {
      // egress unicast port info not valid - expect drop
      Phv phv(nullptr, nullptr);
      TestUtil *tu = setup_deparser(202, pipe, &phv, 0, true);
      ASSERT_TRUE(nullptr != tu);
      DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
      //deparser_block->egress()->set_log_flags(0xff);
      phv.clobber(Phv::make_word(5,1), 0x21u);
      tu->deparser_set_egress_unicast_port_info(
        pipe,
        Phv::make_word(5, 1),  // phv word containing egress unicast port
        false,                 // egress unicast port valid
        false,                 // default egress unicast port valid
        0x0F);                 // default egress unicast port
      Packet *pkt = deparser_block->DeparseEgress(phv, &mirror_pkt);
      ASSERT_TRUE(nullptr == pkt);
      delete tu;
    }
    {
      // egress unicast port info default is valid - expect pkt
      Phv phv(nullptr, nullptr);
      TestUtil *tu = setup_deparser(202, pipe, &phv, 0, true);
      ASSERT_TRUE(nullptr != tu);
      DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
      //deparser_block->egress()->set_log_flags(0xff);
      phv.clobber(Phv::make_word(5,1), 0x1Fu);
      tu->deparser_set_egress_unicast_port_info(
        pipe,
        Phv::make_word(5, 1),  // phv word containing egress unicast port
        false,                 // egress unicast port valid
        true,                  // default egress unicast port valid
        0x0F);                 // default egress unicast port
      Packet *pkt = deparser_block->DeparseEgress(phv, &mirror_pkt);
      ASSERT_TRUE(nullptr != pkt);
      E2MacMetadata *e2mac_metadata = pkt->e2mac_metadata();
      EXPECT_TRUE(e2mac_metadata->is_egress_uc());
      EXPECT_EQ(0x0Fu, e2mac_metadata->egress_unicast_port());
      delete tu;
    }
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_EGRESS_UNICAST_PORT_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    auto *cnt_i_phv_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.cnt_i_phv, 48);
    auto *cnt_i_tphv_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.cnt_i_tphv, 48);
    auto *cnt_i_read = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_read, 48);
    EXPECT_EQ(0u, cnt_i_read->read());  // sanity check
    auto *cnt_i_discard = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_discard, 48);
    EXPECT_EQ(0u, cnt_i_discard->read());  // sanity check
    auto *cnt_pkts_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.cnt_pkts, 48);
    EXPECT_EQ(0u, cnt_pkts_ingress->read());  // sanity check
    // start counter at max value
    cnt_i_phv_ingress->write_max();
    EXPECT_EQ(UINT64_C(0xffffffffffff), cnt_i_phv_ingress->read());  // sanity check
    Port *port = phv.packet()->port();
    auto *ipb_counters = tu->get_objmgr()->ipb_lookup(
        deparser_block->pipe_index(), port->ipb_index())->get_ipb_counters(
            port->ipb_chan());
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());  // sanity check
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());  // sanity check

    phv.set(Phv::make_word(5,12), 0x1314u);
    tu->deparser_set_egress_unicast_port_info(pipe, Phv::make_word(5,12), true, false, 0x03u);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->is_egress_uc());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->egress_uc_port() == 0x0114u);
    EXPECT_EQ(0u, cnt_i_phv_ingress->read());  // counter wrapped
    EXPECT_EQ(1u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(1u, cnt_i_read->read());
    EXPECT_EQ(1u, cnt_pkts_ingress->read());
    EXPECT_EQ(0u, cnt_i_discard->read());
    EXPECT_EQ(1u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());

    cnt_i_tphv_ingress->write_max();  // set counter to max value
    tu->deparser_set_egress_unicast_port_info(pipe, Phv::make_word(5,12), false, true, 0xF011u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->is_egress_uc());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->egress_uc_port() == 0x0011u);
    EXPECT_EQ(1u, cnt_i_phv_ingress->read());
    EXPECT_EQ(0u, cnt_i_tphv_ingress->read());  // counter should wrap
    EXPECT_EQ(2u, cnt_i_read->read());
    EXPECT_EQ(2u, cnt_pkts_ingress->read());
    EXPECT_EQ(0u, cnt_i_discard->read());
    EXPECT_EQ(2u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());

    tu->deparser_set_egress_unicast_port_info(pipe, Phv::make_word(5,12), false, false, 0xF006u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(nullptr == new_pkt);
    EXPECT_EQ(2u, cnt_i_phv_ingress->read());
    EXPECT_EQ(1u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(2u, cnt_i_read->read());
    EXPECT_EQ(2u, cnt_pkts_ingress->read());
    EXPECT_EQ(1u, cnt_i_discard->read());
    EXPECT_EQ(3u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(1u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());

    create_packet(tu, &phv, false);

    phv.set_valid(Phv::make_word(5,12), false);
    tu->deparser_set_egress_unicast_port_info(pipe, Phv::make_word(5,12), true, true, 0x1181u);
    // set counters to max value...
    ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt(UINT64_C(0xffffffffffffffff));
    ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt(0xffffffff);
    cnt_i_read->write_max();  // set counter to max value
    // sanity checks...
    EXPECT_EQ(UINT64_C(0xffffffffffffffff), ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(0xffffffffu, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->is_egress_uc());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->egress_uc_port() == 0x0181u);
    EXPECT_EQ(3u, cnt_i_phv_ingress->read());
    EXPECT_EQ(2u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(0u, cnt_i_read->read());  // counter should wrap to zero
    EXPECT_EQ(3u, cnt_pkts_ingress->read());
    EXPECT_EQ(1u, cnt_i_discard->read());
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());  // check counter wraps
    EXPECT_EQ(0xffffffffu, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());

    phv.clobber(Phv::make_word(5,12), 0xF047);
    tu->deparser_set_egress_unicast_port_info(pipe, Phv::make_word(5,12), true, true, 0x1181u);
    cnt_pkts_ingress->write_max();  // set counter to max value
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->is_egress_uc());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->egress_uc_port() == 0x0047u);
    EXPECT_EQ(4u, cnt_i_phv_ingress->read());
    EXPECT_EQ(3u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(1u, cnt_i_read->read());
    EXPECT_EQ(0u, cnt_pkts_ingress->read());  // counter should wrap to zero
    EXPECT_EQ(1u, cnt_i_discard->read());
    EXPECT_EQ(1u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());  // check counter wraps
    EXPECT_EQ(0xffffffffu, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());

    phv.set_valid(Phv::make_word(5,12), false);
    tu->deparser_set_egress_unicast_port_info(pipe, Phv::make_word(5,12), true, false, 0x1181u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(nullptr == new_pkt);
    EXPECT_EQ(5u, cnt_i_phv_ingress->read());
    EXPECT_EQ(4u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(1u, cnt_i_read->read());
    EXPECT_EQ(0u, cnt_pkts_ingress->read());
    EXPECT_EQ(2u, cnt_i_discard->read());
    EXPECT_EQ(2u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());  // check counter wraps

    delete cnt_i_phv_ingress;
    delete cnt_i_tphv_ingress;
    delete cnt_i_read;
    delete cnt_i_discard;
    delete cnt_pkts_ingress;
    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_FDE_VERSION_MATCHING) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    // Packet Header bytes
    const int phv_word = Phv::make_word(3,11);
    for (uint8_t i = 0; i < 16; ++i) {
      phv.clobber(phv_word + i, i);
    }

    int fde_base_idx = 50;
    for (uint8_t i = 0; i < 16; ++i) {
      tu->deparser_set_field_dictionary_entry(pipe,
        fde_base_idx + i, // We are programming 16 consecutive words.
        1,
        phv_word + i,
        0, 0, 0, // These are garbage PHV words.
        0, // Use POV bit 0.
        1,
        i);
    }

    // POV inside PHV
    phv.set(Phv::make_word(6,3), 0xFFFFu);

    // POV_POSITION
    Dprsr_pov_position_r temp; // a wide register
    for (int i=0; i<32; i++)
      setp_dprsr_pov_position_r_phvs(&temp,i,Phv::make_word(3,1) /*PHV8_1*/);
    setp_dprsr_pov_position_r_phvs(&temp, 0, Phv::make_word(6,3) /*PHV8_1*/);

    //auto& pov_pos_reg = TestUtil::kTofinoPtr->pipes[pipe].deparser.inp.iir.main_i.pov;
    auto& pov_pos_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.pov;
    tu->OutWord(&pov_pos_reg.pov_0_8, temp.pov_0_8);
    tu->OutWord(&pov_pos_reg.pov_1_8, temp.pov_1_8);
    tu->OutWord(&pov_pos_reg.pov_2_8, temp.pov_2_8);
    tu->OutWord(&pov_pos_reg.pov_3_8, temp.pov_3_8);
    tu->OutWord(&pov_pos_reg.pov_4_8, temp.pov_4_8);
    tu->OutWord(&pov_pos_reg.pov_5_8, temp.pov_5_8);
    tu->OutWord(&pov_pos_reg.pov_6_8, temp.pov_6_8);
    tu->OutWord(&pov_pos_reg.pov_7_8, temp.pov_7_8);

    // CSUM_CFG
    for (int i=0; i<Deparser::kNumChecksumEngines; i++) {
      for (int csum_entry_idx=0;
           csum_entry_idx < Deparser::kNumPHVChecksumCfgEntries;
           csum_entry_idx++) {
        tu->deparser_set_csum_cfg_entry(pipe, i, csum_entry_idx, false, true, true);
      }
    }

    RmtObjectManager *om = tu->get_objmgr();
    Packet *pkt = om->pkt_create(default_pktstr);
    ASSERT_TRUE(pkt != nullptr);
    pkt->set_ingress();
    pkt->set_port(new Port(om, PKT_PORT));
    pkt->set_orig_hdr_len(0);
    phv.set_packet(pkt);

    for (uint8_t pkt_version = 0; pkt_version < 4; ++pkt_version) {
      pkt->ingress_info()->set_version(pkt_version);
      auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
      ASSERT_TRUE(new_pkt == pkt);
      uint8_t buf[4];
      new_pkt->get_buf(buf, 0, 4);
      std::vector<uint8_t> v0, v1;
      if (pkt_version & 0x01) {
        v0.push_back(0x09);
        v0.push_back(0x03);
        v0.push_back(0x0B);
        v0.push_back(0x0D);
        v0.push_back(0x07);
        v0.push_back(0x0F);
      }
      else {
        v0.push_back(0x0C);
        v0.push_back(0x06);
        v0.push_back(0x0E);
        v0.push_back(0x0D);
        v0.push_back(0x07);
        v0.push_back(0x0F);
      }
      if (pkt_version & 0x02) {
        v1.push_back(0x06);
        v1.push_back(0x03);
        v1.push_back(0x0E);
        v1.push_back(0x0B);
        v1.push_back(0x07);
        v1.push_back(0x0F);
      }
      else {
        v1.push_back(0x0C);
        v1.push_back(0x09);
        v1.push_back(0x0E);
        v1.push_back(0x0B);
        v1.push_back(0x0D);
        v1.push_back(0x0F);
      }
      std::vector<uint8_t> matched_fde_versions;
      std::sort(v0.begin(), v0.end());
      std::sort(v1.begin(), v1.end());
      std::set_intersection(v0.begin(), v0.end(), v1.begin(), v1.end(),
                            std::back_inserter(matched_fde_versions));
      const uint8_t *buf2 = &matched_fde_versions[0];
      ASSERT_FALSE(memcmp(buf, buf2, 4));
    }
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_HASH1_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 2;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;

    phv.set(Phv::make_word(1,12), 0x1304u);
    tu->deparser_set_hash1_info(pipe, Phv::make_word(1,12), true, 0x03u);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->hash1() == 0x1304u);

    tu->deparser_set_hash1_info(pipe, Phv::make_word(1,12), false, 0xF005u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->hash1() == 0x1005u);

    phv.clobber(Phv::make_word(1,12), 0x81u);
    tu->deparser_set_hash1_info(pipe, Phv::make_word(1,12), true, 0x02u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->hash1() == 0x81u);

    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_HASH2_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 3;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    phv.set(Phv::make_word(1,12), 0x1304u);
    tu->deparser_set_hash2_info(pipe, Phv::make_word(1,12), true, 0x03u);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->hash2() == 0x1304u);

    tu->deparser_set_hash2_info(pipe, Phv::make_word(1,12), false, 0xF005u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->hash2() == 0x1005u);

    phv.clobber(Phv::make_word(1,12), 0x0181u);
    tu->deparser_set_hash2_info(pipe, Phv::make_word(1,12), true, 0x02u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->hash2() == 0x0181u);

    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_ICOS_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    phv.set(Phv::make_word(1,22), 0x04u);
    tu->deparser_set_icos_info(pipe, Phv::make_word(1,22), true, 0x03u);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->icos() == 0x04u);

    tu->deparser_set_icos_info(pipe, Phv::make_word(1,22), false, 0x05u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->icos() == 0x05u);

    phv.clobber(Phv::make_word(1,22), 0x81u);
    tu->deparser_set_icos_info(pipe, Phv::make_word(1,22), true, 0x02u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->icos() == 0x01u);

    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_METER_COLOR_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    phv.set(Phv::make_word(0,11), 0x11u);
    tu->deparser_set_meter_color_info(pipe, Phv::make_word(0,11), true, 0x03u);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->meter_color() == 0x01u);

    tu->deparser_set_meter_color_info(pipe, Phv::make_word(0,11), false, 0x07u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->meter_color() == 0x03u);

    phv.clobber(Phv::make_word(0,11), 0x82u);
    tu->deparser_set_meter_color_info(pipe, Phv::make_word(0,11), true, 0x02u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->meter_color() == 0x02u);

    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_MGID1_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 3;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    tu->deparser_set_pipe_vector_table(pipe, 0, 0x1314u, 0xF1u);
    tu->deparser_set_pipe_vector_table(pipe, 0, 0x7005u, 0x02u);
    tu->deparser_set_pipe_vector_table(pipe, 0, 0x1181u, 0x14u);
    tu->deparser_set_pipe_vector_table(pipe, 0, 0x70FFu, 0x05u);

    phv.set(Phv::make_word(5,12), 0x1314u);
    tu->deparser_set_mgid1_info(pipe, Phv::make_word(5,12), true, false, 0x03u);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->has_mgid1());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->mgid1() == 0x1314u);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x01u);

    tu->deparser_set_mgid1_info(pipe, Phv::make_word(5,12), false, true, 0x7005u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->has_mgid1());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->mgid1() == 0x7005u);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x02u);

    tu->deparser_set_mgid1_info(pipe, Phv::make_word(5,12), false, false, 0x7005u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_FALSE(new_pkt->i2qing_metadata()->has_mgid1());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->mgid1() == 0x00u);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x00u);

    phv.set_valid(Phv::make_word(5,12), false);
    tu->deparser_set_mgid1_info(pipe, Phv::make_word(5,12), true, true, 0x1181u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->has_mgid1());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->mgid1() == 0x1181u);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x04u);

    phv.clobber(Phv::make_word(5,12), 0x70FF);
    tu->deparser_set_mgid1_info(pipe, Phv::make_word(5,12), true, true, 0x1181u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->has_mgid1());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->mgid1() == 0x70FFu);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x05u);

    phv.set_valid(Phv::make_word(5,12), false);
    tu->deparser_set_mgid1_info(pipe, Phv::make_word(5,12), true, false, 0x1181u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_FALSE(new_pkt->i2qing_metadata()->has_mgid1());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->mgid1() == 0x00u);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x00u);

    delete tu;
  }

#ifdef DO_TEST_MGID2_EXTRACTION
  TEST(BFN_TEST_NAME(DeparseTest),TEST_MGID2_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 3;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    tu->deparser_set_pipe_vector_table(pipe, 1, 0x1304u, 0xF4u);
    tu->deparser_set_pipe_vector_table(pipe, 1, 0x7005u, 0x05u);
    tu->deparser_set_pipe_vector_table(pipe, 1, 0x1181u, 0x11u);
    tu->deparser_set_pipe_vector_table(pipe, 1, 0x70F0u, 0x02u);

    phv.set(Phv::make_word(1,12), 0x1304u);
    tu->deparser_set_mgid2_info(pipe, Phv::make_word(1,12), true, false, 0x03u);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->has_mgid2());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->mgid2() == 0x1304u);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x04u);

    tu->deparser_set_mgid2_info(pipe, Phv::make_word(1,12), false, true, 0x7005u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->has_mgid2());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->mgid2() == 0x7005u);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x05u);

    tu->deparser_set_mgid2_info(pipe, Phv::make_word(1,12), false, false, 0x7005u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_FALSE(new_pkt->i2qing_metadata()->has_mgid2());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->mgid2() == 0x00u);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x00u);

    phv.set_valid(Phv::make_word(1,12), false);
    tu->deparser_set_mgid2_info(pipe, Phv::make_word(1,12), true, true, 0x1181u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->has_mgid2());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->mgid2() == 0x1181u);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x01u);

    phv.clobber(Phv::make_word(1,12), 0x70F0);
    tu->deparser_set_mgid2_info(pipe, Phv::make_word(1,12), true, true, 0x1181u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->has_mgid2());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->mgid2() == 0x70F0u);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x02u);

    phv.set_valid(Phv::make_word(1,12), false);
    tu->deparser_set_mgid2_info(pipe, Phv::make_word(1,12), true, false, 0x1181u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_FALSE(new_pkt->i2qing_metadata()->has_mgid2());
    ASSERT_TRUE(new_pkt->i2qing_metadata()->mgid2() == 0x00u);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x00u);

    delete tu;
  }
#endif /* DO_TEST_MGID2_EXTRACTION */

  TEST(BFN_TEST_NAME(DeparseTest),TEST_MIRROR_RESUBMIT_DEPARSER) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 2;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(2, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    auto *cnt_i_phv_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.cnt_i_phv, 48);
    EXPECT_EQ(0u, cnt_i_phv_ingress->read());  // sanity check
    auto *cnt_i_tphv_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.cnt_i_tphv, 48);
    EXPECT_EQ(0u, cnt_i_tphv_ingress->read());  // sanity check
    auto *cnt_i_phv_egress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.ier.main_e.cnt_i_phv, 48);
    EXPECT_EQ(0u, cnt_i_phv_egress->read());  // sanity check
    auto *cnt_i_tphv_egress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.ier.main_e.cnt_i_tphv, 48);
    EXPECT_EQ(0u, cnt_i_tphv_egress->read());  // sanity check
    auto *cnt_i_resubmit = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_resubmit, 48);
    EXPECT_EQ(0u, cnt_i_resubmit->read());  // sanity check
    auto *cnt_i_read = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_read, 48);
    EXPECT_EQ(0u, cnt_i_read->read());  // sanity check
    auto *cnt_pkts_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.cnt_pkts, 48);
    EXPECT_EQ(0u, cnt_pkts_ingress->read());  // sanity check
    auto *cnt_pkts_egress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.cnt_pkts, 48);
    EXPECT_EQ(0u, cnt_pkts_egress->read());  // sanity check
    auto *i_fwd_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_fwd_pkts, 48);
    EXPECT_EQ(0u, i_fwd_pkts->read());  // sanity check
    auto *i_disc_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_disc_pkts, 48);
    EXPECT_EQ(0u, i_disc_pkts->read());  // sanity check
    auto *i_mirr_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_mirr_pkts, 48);
    EXPECT_EQ(0u, i_mirr_pkts->read());  // sanity check
    auto *e_fwd_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.e_fwd_pkts, 48);
    EXPECT_EQ(0u, e_fwd_pkts->read());  // sanity check
    auto *e_disc_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.e_disc_pkts, 48);
    EXPECT_EQ(0u, e_disc_pkts->read());  // sanity check
    auto *e_mirr_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.e_mirr_pkts, 48);
    EXPECT_EQ(0u, e_mirr_pkts->read());  // sanity check

    Port *port = phv.packet()->port();
    auto *ipb_counters = tu->get_objmgr()->ipb_lookup(
        deparser_block->pipe_index(), port->ipb_index())->get_ipb_counters(
            port->ipb_chan());
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());  // sanity check
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());  // sanity check

    // This prevents deparser from dropping packet.
    tu->deparser_set_egress_unicast_port_info(pipe, 0, true, true, 0x01);

    std::array<uint8_t, 8> resubmit_metadata_phv_idx = {{
      (uint8_t)Phv::make_word(1,1), (uint8_t)Phv::make_word(1,1),
      (uint8_t)Phv::make_word(1,1), (uint8_t)Phv::make_word(1,2),
      (uint8_t)Phv::make_word(3,4), 0xFF, 0xFF, 0xFF }};
    {
      phv.set(Phv::make_word(1,12), 0x131Cu);
      tu->deparser_set_mirror_cfg(pipe, true, Phv::make_word(1,12), true);
      // Set resubmit registers.
      phv.set(Phv::make_word(2,9), 0x93C2u);
      tu->deparser_set_resubmit_cfg(pipe, true, Phv::make_word(2,9));
      phv.set(Phv::make_word(1,1), 0x1122131Cu);
      phv.set(Phv::make_word(1,2), 0xFF230000u);
      phv.set(Phv::make_word(3,4), 0xFCu);
      phv.ingress_packet()->set_orig_hdr_len(2);
      tu->deparser_set_resubmit_table_entry(pipe, 0x2, true, 5, resubmit_metadata_phv_idx);
      cnt_i_resubmit->write_max();  // set counter to max value
      auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
      ASSERT_TRUE(nullptr != resubmit_pkt);
      ASSERT_TRUE(nullptr == mirror_pkt);
      ASSERT_TRUE(nullptr == new_pkt);
      EXPECT_EQ(1u, cnt_i_phv_ingress->read());
      EXPECT_EQ(1u, cnt_i_tphv_ingress->read());
      EXPECT_EQ(0u, cnt_i_phv_egress->read());
      EXPECT_EQ(0u, cnt_i_tphv_egress->read());
      EXPECT_EQ(0u, cnt_i_resubmit->read());  // counter should wrap to zero
      EXPECT_EQ(0u, cnt_i_read->read());
      EXPECT_EQ(0u, cnt_pkts_ingress->read());
      EXPECT_EQ(0u, cnt_pkts_egress->read());
      EXPECT_EQ(0u, i_fwd_pkts->read());
      EXPECT_EQ(0u, i_disc_pkts->read());
      EXPECT_EQ(0u, i_mirr_pkts->read());
      EXPECT_EQ(0u, e_fwd_pkts->read());
      EXPECT_EQ(0u, e_disc_pkts->read());
      EXPECT_EQ(0u, e_mirr_pkts->read());
      EXPECT_EQ(1u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
      // resubmits are not counted as drops
      EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());

      auto resubmit_header = resubmit_pkt->get_resubmit_header();
      ASSERT_TRUE(resubmit_header->len() == 5);
      uint8_t buf[5], expected_buf[5] = { 0x11, 0x22, 0x13, 0xFF, 0xFC };
      resubmit_header->get_buf(buf, 0, 5);
      ASSERT_FALSE(memcmp(buf, expected_buf, 5));
      tu->get_objmgr()->pkt_delete(resubmit_pkt);
    }

    const char *pktstr = default_pktstr;
    Packet *pkt = tu->get_objmgr()->pkt_create(pktstr);
    assert(pkt != nullptr);
    pkt->set_ingress();
    pkt->set_port(new Port(tu->get_objmgr(), PKT_PORT));
    phv.set_packet(pkt);

    phv.set_valid(Phv::make_word(5,2), false);
    tu->deparser_set_mirror_table_entry(pipe, 4, true, Phv::make_word(5,2), 0, true);
    tu->deparser_set_resubmit_table_entry(pipe, 0x2, false, 3, resubmit_metadata_phv_idx);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(nullptr == resubmit_pkt);
    ASSERT_TRUE(nullptr == mirror_pkt);
    ASSERT_TRUE(nullptr != new_pkt);
    EXPECT_EQ(2u, cnt_i_phv_ingress->read());
    EXPECT_EQ(2u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(0u, cnt_i_phv_egress->read());
    EXPECT_EQ(0u, cnt_i_tphv_egress->read());
    EXPECT_EQ(0u, cnt_i_resubmit->read());
    EXPECT_EQ(1u, cnt_i_read->read());
    EXPECT_EQ(1u, cnt_pkts_ingress->read());
    EXPECT_EQ(0u, cnt_pkts_egress->read());
    EXPECT_EQ(1u, i_fwd_pkts->read());
    EXPECT_EQ(0u, i_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());
    EXPECT_EQ(0u, e_fwd_pkts->read());
    EXPECT_EQ(0u, e_disc_pkts->read());
    EXPECT_EQ(0u, e_mirr_pkts->read());
    EXPECT_EQ(2u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());

    // Add a 2B header field to packet in the deparser.
    Dprsr_pov_position_r temp; // a wide register
    for (int i=0; i<32; i++)
      setp_dprsr_pov_position_r_phvs(&temp, i, Phv::make_word(3,1) /*PHV8_1*/);
    setp_dprsr_pov_position_r_phvs(&temp, 1, Phv::make_word(6,2));
    //auto& pov_pos_reg = TestUtil::kTofinoPtr->pipes[pipe].deparser.inp.iir.main_i.pov;
    auto& pov_pos_reg = RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.pov;
    tu->OutWord( &pov_pos_reg.pov_0_8 , temp.pov_0_8 );
    tu->OutWord( &pov_pos_reg.pov_1_8 , temp.pov_1_8 );
    tu->OutWord( &pov_pos_reg.pov_2_8 , temp.pov_2_8 );
    tu->OutWord( &pov_pos_reg.pov_3_8 , temp.pov_3_8 );
    tu->OutWord( &pov_pos_reg.pov_4_8 , temp.pov_4_8 );
    tu->OutWord( &pov_pos_reg.pov_5_8 , temp.pov_5_8 );
    tu->OutWord( &pov_pos_reg.pov_6_8 , temp.pov_6_8 );
    tu->OutWord( &pov_pos_reg.pov_7_8 , temp.pov_7_8 );

    // POV inside PHV
    phv.set(Phv::make_word(6,2), 0x0001u);
    tu->deparser_set_field_dictionary_entry(pipe, 0, 1, Phv::make_word(0,10),
        Phv::make_word(0,10), Phv::make_word(0,10), Phv::make_word(0,10), 8, 2,
        0x0F);
    phv.set(Phv::make_word(0,10), 0xBAADDAADu);

    phv.set(Phv::make_word(5,2), 0x0F1Cu);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(nullptr == resubmit_pkt);
    ASSERT_TRUE(nullptr != mirror_pkt);
    ASSERT_TRUE(nullptr != new_pkt);
    EXPECT_EQ(3u, cnt_i_phv_ingress->read());
    EXPECT_EQ(3u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(0u, cnt_i_phv_egress->read());
    EXPECT_EQ(0u, cnt_i_tphv_egress->read());
    EXPECT_EQ(0u, cnt_i_resubmit->read());
    EXPECT_EQ(2u, cnt_i_read->read());
    EXPECT_EQ(2u, cnt_pkts_ingress->read());
    EXPECT_EQ(0u, cnt_pkts_egress->read());
    EXPECT_EQ(2u, i_fwd_pkts->read());
    EXPECT_EQ(0u, i_disc_pkts->read());
    EXPECT_EQ(1u, i_mirr_pkts->read());
    EXPECT_EQ(0u, e_fwd_pkts->read());
    EXPECT_EQ(0u, e_disc_pkts->read());
    EXPECT_EQ(0u, e_mirr_pkts->read());
    EXPECT_EQ(3u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());

    ASSERT_TRUE((default_pktstr_len + 2) == new_pkt->len());
    new_pkt->trim_front(2);

    ASSERT_TRUE(new_pkt->len() == mirror_pkt->len());
    ASSERT_TRUE(0x31Cu == mirror_pkt->mirror_metadata()->mirror_id());

    uint8_t buf[default_pktstr_len], buf2[default_pktstr_len];
    new_pkt->get_buf(buf, 0, default_pktstr_len);
    ASSERT_TRUE(memcmp(buf, buf2, default_pktstr_len));
    mirror_pkt->get_buf(buf2, 0, default_pktstr_len);
    ASSERT_FALSE(memcmp(buf, buf2, default_pktstr_len));

    tu->get_objmgr()->pkt_delete(mirror_pkt);
    mirror_pkt = nullptr;

    pkt = phv.ingress_packet();
    pkt->set_egress();
    phv.set_packet(pkt);

    phv.set(Phv::make_word(3,12), 0x131Du);
    tu->deparser_set_mirror_cfg(pipe, false, Phv::make_word(3,12), true);
    phv.set(Phv::make_word(1,20), 0x0F2Cu);
    tu->deparser_set_mirror_table_entry(pipe, 5, false, Phv::make_word(1,20), 7, true);
    uint8_t metadata_phv_idx[32] = {
      (uint8_t)Phv::make_word(0,8),
      (uint8_t)Phv::make_word(1,11),
      (uint8_t)Phv::make_word(1,11),
      (uint8_t)Phv::make_word(1,11),
      (uint8_t)Phv::make_word(1,11),
      (uint8_t)Phv::make_word(4,22),
      (uint8_t)Phv::make_word(4,22),
      (uint8_t)Phv::make_word(4,23), };
    phv.set(Phv::make_word(0,8), 0x2324u);
    phv.set(Phv::make_word(1,11), 0xAABB13A4u);
    phv.set(Phv::make_word(4,22), 0xABAB1B2Bu);
    tu->deparser_set_mirror_metadata(pipe, 6, false, metadata_phv_idx);

    new_pkt = deparser_block->DeparseEgress(phv, &mirror_pkt);
    EXPECT_EQ(1u, cnt_i_phv_egress->read());
    EXPECT_EQ(1u, cnt_i_tphv_egress->read());
    EXPECT_EQ(1u, cnt_pkts_egress->read());
    EXPECT_EQ(1u, e_fwd_pkts->read());
    EXPECT_EQ(0u, e_disc_pkts->read());
    EXPECT_EQ(1u, e_mirr_pkts->read());
    // following are unchanged...
    EXPECT_EQ(3u, cnt_i_phv_ingress->read());
    EXPECT_EQ(3u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(2u, cnt_i_read->read());  // sanity check
    EXPECT_EQ(2u, cnt_pkts_ingress->read());
    EXPECT_EQ(2u, i_fwd_pkts->read());
    EXPECT_EQ(0u, i_disc_pkts->read());
    EXPECT_EQ(1u, i_mirr_pkts->read());

    ASSERT_TRUE(nullptr != mirror_pkt);
    ASSERT_TRUE(nullptr != new_pkt);
    ASSERT_TRUE((new_pkt->len() + 7) == mirror_pkt->len());
    ASSERT_TRUE(0x32Cu == mirror_pkt->mirror_metadata()->mirror_id());

    uint8_t mirror_metadata[7];
    mirror_pkt->get_buf(mirror_metadata, 0, 7);
    mirror_pkt->trim_front(7);

    new_pkt->get_buf(buf, 0, default_pktstr_len);
    mirror_pkt->get_buf(buf2, 0, default_pktstr_len);
    ASSERT_FALSE(memcmp(buf, buf2, default_pktstr_len));

    tu->get_objmgr()->pkt_delete(mirror_pkt);

    delete cnt_i_phv_ingress;
    delete cnt_i_tphv_ingress;
    delete cnt_i_resubmit;
    delete cnt_i_phv_egress;
    delete cnt_i_tphv_egress;
    delete cnt_pkts_ingress;
    delete cnt_pkts_egress;
    delete cnt_i_read;
    delete i_fwd_pkts;
    delete i_disc_pkts;
    delete i_mirr_pkts;
    delete e_fwd_pkts;
    delete e_disc_pkts;
    delete e_mirr_pkts;
    delete tu;
  }

#ifdef DO_TEST_MULTICAST_PIPE_VECTOR_EXTRACTION
  TEST(BFN_TEST_NAME(DeparseTest),TEST_MULTICAST_PIPE_VECTOR_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 3;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(2, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    tu->deparser_set_pipe_vector_table(pipe, 0, 0x1324u, 0xF4u);
    tu->deparser_set_pipe_vector_table(pipe, 1, 0x7025u, 0x01u);

    phv.set(Phv::make_word(1,12), 0x1324u);
    tu->deparser_set_mgid1_info(pipe, Phv::make_word(1,12), true, false, 0x03u);
    phv.set(Phv::make_word(6,22), 0x7025u);
    tu->deparser_set_mgid2_info(pipe, Phv::make_word(6,22), true, true, 0x01u);
    {
      auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
      ASSERT_TRUE(new_pkt->i2qing_metadata()->has_mgid1());
      ASSERT_TRUE(new_pkt->i2qing_metadata()->mgid1() == 0x1324u);
      ASSERT_TRUE(new_pkt->i2qing_metadata()->has_mgid2());
      ASSERT_TRUE(new_pkt->i2qing_metadata()->mgid2() == 0x7025u);
      ASSERT_FALSE(new_pkt->i2qing_metadata()->cpu_needs_copy());
      ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x05u);
    }

    phv.set_valid(Phv::make_word(1,12), false);
    tu->deparser_set_copy_to_cpu_info(pipe, Phv::make_word(4,2), false, true);
    tu->deparser_set_copy_to_cpu_pipe_vector(pipe, 0x02u);
    {
      auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
      ASSERT_FALSE(new_pkt->i2qing_metadata()->has_mgid1());
      ASSERT_TRUE(new_pkt->i2qing_metadata()->has_mgid2());
      ASSERT_TRUE(new_pkt->i2qing_metadata()->mgid2() == 0x7025u);
      ASSERT_TRUE(new_pkt->i2qing_metadata()->cpu_needs_copy());
      ASSERT_TRUE(new_pkt->i2qing_metadata()->multicast_pipe_vector() == 0x03u);
    }

    delete tu;
  }
#endif /* DO_TEST_MULTICAST_PIPE_VECTOR_EXTRACTION */

  TEST(BFN_TEST_NAME(DeparseTest),TEST_PHYSICAL_INGRESS_PORT_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    uint16_t ingress_port_id = 0x46 | (pipe << 7);  // set arbitrary port id
    phv.ingress_packet()->set_port(new Port(tu->get_objmgr(), ingress_port_id));

    // port index from phv - disabled
    phv.set(Phv::make_word(1,23), 0xF101u);
    tu->deparser_set_physical_ingress_port_info(pipe, Phv::make_word(1,23), true);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_EQ(ingress_port_id, new_pkt->i2qing_metadata()->physical_ingress_port());

    // port index from phv - enabled
    tu->deparser_set_physical_ingress_port_info(pipe, Phv::make_word(1,23), false);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_EQ(0x0101u, new_pkt->i2qing_metadata()->physical_ingress_port());
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_QID_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    phv.set(Phv::make_word(1,23), 0x11u);
    tu->deparser_set_qid_info(pipe, Phv::make_word(1,23), true, 0x03u);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->qid() == 0x11u);

    tu->deparser_set_qid_info(pipe, Phv::make_word(1,23), false, 0x05u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->qid() == 0x05u);

    phv.clobber(Phv::make_word(1,23), 0x82u);
    tu->deparser_set_qid_info(pipe, Phv::make_word(1,23), true, 0x02u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->qid() == 0x02u);

    delete tu;
  }

#if 0   // the checksum config is not readable any more, so this test won't work
  TEST(BFN_TEST_NAME(DeparseTest),TEST_REGISTER_COPY) {
    LearnQuantumType lq;
    int pipe = 2;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(2, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);
    uint32_t cfg = tu->deparser_get_csum_cfg_entry(pipe, 8, 20);
    ASSERT_TRUE((cfg & 0x07) == 0x00);
    tu->deparser_set_csum_cfg_entry(pipe, 2, 20, true, true, true);
    cfg = tu->deparser_get_csum_cfg_entry(pipe, 2, 20);
    ASSERT_TRUE((cfg & 0x07) == 0x07);
    cfg = tu->deparser_get_csum_cfg_entry(pipe, 8, 20);
    ASSERT_TRUE((cfg & 0x07) == 0x07);

    tu->deparser_set_csum_cfg_entry(pipe, 9, 22, true, false, true);
    cfg = tu->deparser_get_csum_cfg_entry(pipe, 9, 22);
    ASSERT_TRUE((cfg & 0x07) == 0x06);
    cfg = tu->deparser_get_csum_cfg_entry(pipe, 3, 22);
    ASSERT_TRUE((cfg & 0x07) == 0x06);

    tu->deparser_set_csum_cfg_entry(pipe, 1, 42, false, false, true);
    tu->deparser_set_csum_cfg_entry(pipe, 7, 42, false, true, false);
    cfg = tu->deparser_get_csum_cfg_entry(pipe, 1, 42);
    ASSERT_TRUE((cfg & 0x07) == 0x02);
    cfg = tu->deparser_get_csum_cfg_entry(pipe, 7, 42);
    ASSERT_TRUE((cfg & 0x07) == 0x01);
  }
#endif

  TEST(BFN_TEST_NAME(DeparseTest),TEST_RESUBMIT_DEPARSER) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 2;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(2, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);
    auto *cnt_i_phv_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.cnt_i_phv, 48);
    EXPECT_EQ(0u, cnt_i_phv_ingress->read());  // sanity check
    auto *cnt_i_tphv_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.cnt_i_tphv, 48);
    cnt_i_tphv_ingress->write(UINT64_C(0x000000000001));  //  skew phv and tphv counters
    EXPECT_EQ(UINT64_C(0x000000000001), cnt_i_tphv_ingress->read());  // sanity check
    auto *cnt_i_resubmit = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_resubmit, 48);
    EXPECT_EQ(0u, cnt_i_resubmit->read());  // sanity check
    auto *cnt_i_read = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_read, 48);
    EXPECT_EQ(0u, cnt_i_read->read());  // sanity check
    auto *cnt_i_discard = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_discard, 48);
    EXPECT_EQ(0u, cnt_i_discard->read());  // sanity check
    auto *cnt_pkts_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.cnt_pkts, 48);
    EXPECT_EQ(0u, cnt_pkts_ingress->read());  // sanity check
    Port *port = phv.packet()->port();
    auto *ipb_counters = tu->get_objmgr()->ipb_lookup(
        deparser_block->pipe_index(), port->ipb_index())->get_ipb_counters(
            port->ipb_chan());
    auto *i_fwd_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_fwd_pkts, 48);
    EXPECT_EQ(0u, i_fwd_pkts->read());  // sanity check
    auto *i_disc_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_disc_pkts, 48);
    EXPECT_EQ(0u, i_disc_pkts->read());  // sanity check
    auto *i_mirr_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_mirr_pkts, 48);
    EXPECT_EQ(0u, i_mirr_pkts->read());  // sanity check
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());  // sanity check
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());  // sanity check

    phv.ingress_packet()->set_orig_hdr_len(0);

    // This prevents deparser from dropping packet.
    tu->deparser_set_egress_unicast_port_info(pipe, 0, false, true, 0x01);

    const std::array<uint8_t, 8> resubmit_table_entries[8] = {
      {{(uint8_t)Phv::make_word(4,5),
        (uint8_t)Phv::make_word(4,5),
        (uint8_t)Phv::make_word(2,9),
        (uint8_t)Phv::make_word(6,13),
        (uint8_t)Phv::make_word(6,13),
        (uint8_t)Phv::make_word(5,2),
        (uint8_t)Phv::make_word(5,2),
        (uint8_t)Phv::make_word(3,0)}},
      {{(uint8_t)Phv::make_word(4,15),
        (uint8_t)Phv::make_word(4,15),
        (uint8_t)Phv::make_word(0,19),
        (uint8_t)Phv::make_word(0,19),
        (uint8_t)Phv::make_word(0,19),
        (uint8_t)Phv::make_word(0,19),
        (uint8_t)Phv::make_word(3,18),
        (uint8_t)Phv::make_word(2,7)}},
      {{(uint8_t)Phv::make_word(2,6),
        (uint8_t)Phv::make_word(4,17),
        (uint8_t)Phv::make_word(4,17),
        (uint8_t)Phv::make_word(2,3),
        (uint8_t)Phv::make_word(6,21),
        (uint8_t)Phv::make_word(6,21),
        (uint8_t)Phv::make_word(2,24),
        (uint8_t)Phv::make_word(2,31)}},
      {{(uint8_t)Phv::make_word(2,13),
        (uint8_t)Phv::make_word(3,23),
        (uint8_t)Phv::make_word(6,4),
        (uint8_t)Phv::make_word(6,4),
        (uint8_t)Phv::make_word(0,22),
        (uint8_t)Phv::make_word(0,22),
        (uint8_t)Phv::make_word(0,22),
        (uint8_t)Phv::make_word(0,22)}},
      {{(uint8_t)Phv::make_word(6,2),
        (uint8_t)Phv::make_word(6,2),
        (uint8_t)Phv::make_word(5,27),
        (uint8_t)Phv::make_word(5,27),
        (uint8_t)Phv::make_word(0,16),
        (uint8_t)Phv::make_word(0,16),
        (uint8_t)Phv::make_word(0,16),
        (uint8_t)Phv::make_word(0,16)}},
      {{(uint8_t)Phv::make_word(1,23),
        (uint8_t)Phv::make_word(1,23),
        (uint8_t)Phv::make_word(1,23),
        (uint8_t)Phv::make_word(1,23),
        (uint8_t)Phv::make_word(0,25),
        (uint8_t)Phv::make_word(0,25),
        (uint8_t)Phv::make_word(0,25),
        (uint8_t)Phv::make_word(0,25)}},
      {{(uint8_t)Phv::make_word(4,13),
        (uint8_t)Phv::make_word(4,13),
        (uint8_t)Phv::make_word(2,18),
        (uint8_t)Phv::make_word(3,20),
        (uint8_t)Phv::make_word(1,9),
        (uint8_t)Phv::make_word(1,9),
        (uint8_t)Phv::make_word(1,9),
        (uint8_t)Phv::make_word(1,9)}},
      {{(uint8_t)Phv::make_word(2,28),
        (uint8_t)Phv::make_word(1,18),
        (uint8_t)Phv::make_word(1,18),
        (uint8_t)Phv::make_word(1,18),
        (uint8_t)Phv::make_word(1,18),
        (uint8_t)Phv::make_word(4,25),
        (uint8_t)Phv::make_word(4,25),
        (uint8_t)Phv::make_word(3,19)}}
    };

    constexpr std::array<int, 8> resubmit_table_entry_len = {{8, 2, 6, 2, 6, 3, 8, 2}};
    const std::array<bool, 8> resubmit_table_entry_valid = {{false, false, false, false, true, true, true, true}};

    // Configure all the resubmit tables.
    for (uint8_t i = 0; i < 8; ++i) {
      tu->deparser_set_resubmit_table_entry(pipe, i,
        resubmit_table_entry_valid[i], resubmit_table_entry_len[i],
        resubmit_table_entries[i]);
    }

    // Set every PHV container to contain its bit-inverted PHV index. That way,
    // we can deduce the resubmit header just from the PHV indices in the
    // resubmit table.
    for (int k = 0; k < Phv::kWordsMax; ++k) {
      uint32_t value = k | (k << 8) | (k << 16) | (k << 24);
      phv.set(k, ~(value));
    }

    tu->deparser_set_resubmit_cfg(pipe, true, Phv::make_word(3,10));

    uint64_t sent = 0, resubmitted = 0, read = 0;
    for (uint8_t tbl_idx = 0; tbl_idx < 8; ++tbl_idx) {
      phv.clobber(Phv::make_word(3,10), tbl_idx | (0xF8u));

      resubmit_pkt = nullptr;
      mirror_pkt = nullptr;
      auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
      sent++;

      ASSERT_TRUE(nullptr == mirror_pkt);
      switch (tbl_idx) {
        case 0:
        case 1:
        case 2:
        case 3:
          read++;
          ASSERT_TRUE(nullptr == resubmit_pkt);
          ASSERT_TRUE(nullptr != new_pkt);
          ASSERT_TRUE(default_pktstr_len == new_pkt->len());
          break;

        case 4:
        case 5:
        case 6:
        case 7: {
          resubmitted++;
          ASSERT_TRUE(nullptr != resubmit_pkt);
          ASSERT_TRUE(nullptr == new_pkt);
          const auto resubmit_header_len = resubmit_table_entry_len[tbl_idx];
          auto resubmit_header = resubmit_pkt->get_resubmit_header();
          ASSERT_TRUE(resubmit_header->len() == resubmit_header_len);

          // Extract the resubmit header.
          std::unique_ptr<uint8_t[]> buf(new uint8_t[resubmit_header_len]);
          resubmit_header->get_buf(buf.get(), 0, resubmit_header_len);
          // We expect every byte in the resubmit header to contain the
          // bit-inverted value of its PHV index.
          std::array<uint8_t, 8> expected_buf = resubmit_table_entries[tbl_idx];
          for (int i = 0; i < 8; ++i) expected_buf[i] = ~expected_buf[i];
          ASSERT_FALSE(memcmp(buf.get(), &expected_buf[0], resubmit_header_len));

          // Strip off the resubmit header since we use the same packet for the
          // next iteration of this for-loop.
          resubmit_pkt->set_resubmit_header(nullptr);
          resubmit_pkt->unmark_for_resubmit();
          tu->get_objmgr()->pktbuf_delete(resubmit_header);
          break;
        }
      }
      EXPECT_EQ(sent, cnt_i_phv_ingress->read());
      EXPECT_EQ(sent + 1u, cnt_i_tphv_ingress->read());
      EXPECT_EQ(sent, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt()) << (int)tbl_idx;
      // don't count resubmitted packets as dropped
      EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt()) << (int)tbl_idx;
      EXPECT_EQ(resubmitted, cnt_i_resubmit->read());
      // don't count resubmitted packets as 'read'
      EXPECT_EQ(read, cnt_i_read->read());
      EXPECT_EQ(read, cnt_pkts_ingress->read());
      EXPECT_EQ(read, i_fwd_pkts->read());
      EXPECT_EQ(0u, i_disc_pkts->read());
      EXPECT_EQ(0u, i_mirr_pkts->read());
      EXPECT_EQ(0u, cnt_i_discard->read());
    }

    // Force a packet drop due to drop_ctl bit 0.
    resubmit_pkt = nullptr;
    mirror_pkt = nullptr;
    phv.clobber(Phv::make_word(3,23), 0x11u);
    tu->deparser_set_drop_ctl_info(pipe, Phv::make_word(3,23), true, 0);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    sent++;
    ASSERT_TRUE(nullptr == resubmit_pkt);
    ASSERT_TRUE(nullptr == mirror_pkt);
    ASSERT_TRUE(nullptr == new_pkt);
    EXPECT_EQ(sent, cnt_i_phv_ingress->read());
    EXPECT_EQ(sent + 1u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(sent, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(1u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());
    EXPECT_EQ(resubmitted, cnt_i_resubmit->read());
    // dropped packet is not counted as 'read'
    EXPECT_EQ(read, cnt_i_read->read());
    EXPECT_EQ(read, cnt_pkts_ingress->read());
    EXPECT_EQ(1u, cnt_i_discard->read());
    EXPECT_EQ(read, i_fwd_pkts->read());
    EXPECT_EQ(1u, i_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());

    delete cnt_i_phv_ingress;
    delete cnt_i_tphv_ingress;
    delete cnt_i_resubmit;
    delete cnt_i_read;
    delete cnt_i_discard;
    delete cnt_pkts_ingress;
    delete i_fwd_pkts;
    delete i_disc_pkts;
    delete i_mirr_pkts;
    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_RID_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 0;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    phv.set(Phv::make_word(5,23), 0x11u);
    tu->deparser_set_rid_info(pipe, Phv::make_word(5,23), true, 0x03u);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->irid() == 0x11u);

    tu->deparser_set_rid_info(pipe, Phv::make_word(5,23), false, 0x0705u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->irid() == 0x0705u);

    phv.clobber(Phv::make_word(5,23), 0x8002u);
    tu->deparser_set_rid_info(pipe, Phv::make_word(5,23), true, 0x02u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->irid() == 0x8002u);

    // Testing error-case where 16b metadata is extracted from 8b PHV.
    phv.clobber(Phv::make_word(2,23), 0x12u);
    tu->deparser_set_rid_info(pipe, Phv::make_word(2,23), true, 0x02u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->irid() == 0x1212u);

    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_USE_YID_TBL_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 0;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    tu->deparser_set_use_yid_tbl_info(pipe, 0x03u);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    tu->get_objmgr()->pkt_delete(mirror_pkt);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->use_yid_tbl());

    tu->deparser_set_use_yid_tbl_info(pipe, 0x0704u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    tu->get_objmgr()->pkt_delete(mirror_pkt);
    ASSERT_FALSE(new_pkt->i2qing_metadata()->use_yid_tbl());

    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_BYPASS_MODE_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 0;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    phv.set(Phv::make_word(5,23), 0x11u);
    tu->deparser_set_bypass_egr_mode_info(pipe, Phv::make_word(5,23), true, 0x03u);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->bypass_egr_mode());

    tu->deparser_set_bypass_egr_mode_info(pipe, Phv::make_word(5,23), false, 0x0704u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_FALSE(new_pkt->i2qing_metadata()->bypass_egr_mode());

    phv.clobber(Phv::make_word(5,23), 0x8002u);
    tu->deparser_set_bypass_egr_mode_info(pipe, Phv::make_word(5,23), true, 0x02u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_FALSE(new_pkt->i2qing_metadata()->bypass_egr_mode());

    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_XID_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    phv.set(Phv::make_word(0,23), 0x11u);
    tu->deparser_set_xid_info(pipe, Phv::make_word(0,23), true, 0x03u);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->xid() == 0x11u);

    tu->deparser_set_xid_info(pipe, Phv::make_word(0,23), false, 0x05u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->xid() == 0x05u);

    phv.clobber(Phv::make_word(0,23), 0x8002u);
    tu->deparser_set_xid_info(pipe, Phv::make_word(0,23), true, 0x02u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->xid() == 0x8002u);

    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_YID_EXTRACTION) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    phv.set(Phv::make_word(1,23), 0x31u);
    tu->deparser_set_yid_info(pipe, Phv::make_word(1,23), true, 0x03u);
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->yid() == 0x31u);

    tu->deparser_set_yid_info(pipe, Phv::make_word(1,23), false, 0x05u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->yid() == 0x05u);

    phv.clobber(Phv::make_word(1,23), 0x0102u);
    tu->deparser_set_yid_info(pipe, Phv::make_word(1,23), true, 0x02u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    ASSERT_TRUE(new_pkt->i2qing_metadata()->yid() == 0x0102u);

    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_DROP_CTL_INGRESS) {
    Packet *mirror_pkt = nullptr, *resubmit_pkt = nullptr;
    LearnQuantumType lq;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    PacketGenMetadata* packet_gen_metadata = nullptr;
    ASSERT_TRUE(deparser_block != nullptr);

    auto *cnt_i_phv_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.cnt_i_phv, 48);
    EXPECT_EQ(0u, cnt_i_phv_ingress->read());  // sanity check
    auto *cnt_i_tphv_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.cnt_i_tphv, 48);
    EXPECT_EQ(0u, cnt_i_tphv_ingress->read());  // sanity check
    auto *cnt_i_read = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_read, 48);
    EXPECT_EQ(0u, cnt_i_read->read());  // sanity check
    auto *cnt_i_discard = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_discard, 48);
    EXPECT_EQ(0u, cnt_i_discard->read());  // sanity check
    auto *cnt_pkts_ingress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.cnt_pkts, 48);
    EXPECT_EQ(0u, cnt_pkts_ingress->read());  // sanity check
    auto *i_disc_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_disc_pkts, 48);
    EXPECT_EQ(0u, i_disc_pkts->read());  // sanity check
    auto *e_disc_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.e_disc_pkts, 48);
    EXPECT_EQ(0u, e_disc_pkts->read());  // sanity check
    auto *i_mirr_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_mirr_pkts, 48);
    EXPECT_EQ(0u, i_mirr_pkts->read());  // sanity check
    auto *e_mirr_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.e_mirr_pkts, 48);
    EXPECT_EQ(0u, e_mirr_pkts->read());  // sanity check
    Port *port = phv.packet()->port();
    auto *ipb_counters = tu->get_objmgr()->ipb_lookup(
        deparser_block->pipe_index(), port->ipb_index())->get_ipb_counters(
            port->ipb_chan());
    // sanity checks
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());

    // sanity check - packet is not dropped
    auto new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    EXPECT_FALSE(nullptr == new_pkt);
    EXPECT_TRUE(nullptr == mirror_pkt);
    EXPECT_TRUE(nullptr == resubmit_pkt);
    EXPECT_EQ(1u, cnt_i_phv_ingress->read());
    EXPECT_EQ(1u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(1u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());
    EXPECT_EQ(1u, cnt_i_read->read());
    EXPECT_EQ(1u, cnt_pkts_ingress->read());
    EXPECT_EQ(0u, cnt_i_discard->read());
    EXPECT_EQ(0u, i_disc_pkts->read());
    EXPECT_EQ(0u, e_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());
    EXPECT_EQ(0u, e_mirr_pkts->read());

    // set drop ctl bits[0] in PHV and set drop_ctl_info to read this from PHV
    phv.clobber_d(Phv::make_word_d(4,2), 0x1u);
    tu->deparser_set_drop_ctl_info(pipe, Phv::make_word_d(4,2), true, 0);
    // packet should now be dropped
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    EXPECT_TRUE(nullptr == new_pkt);
    EXPECT_TRUE(nullptr == mirror_pkt);
    EXPECT_TRUE(nullptr == resubmit_pkt);
    EXPECT_EQ(2u, cnt_i_phv_ingress->read());
    EXPECT_EQ(2u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(2u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(1u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());
    EXPECT_EQ(1u, cnt_i_read->read());
    EXPECT_EQ(1u, cnt_pkts_ingress->read());
    EXPECT_EQ(1u, cnt_i_discard->read());
    EXPECT_EQ(1u, i_disc_pkts->read());
    EXPECT_EQ(0u, e_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());
    EXPECT_EQ(0u, e_mirr_pkts->read());

    // set copy to cpu - packet should not be dropped
    create_packet(tu, &phv, false);
    phv.set_d(Phv::make_word_d(4,3), (0x01u << 3));
    tu->deparser_set_copy_to_cpu_info(pipe, Phv::make_word_d(4,3), false, true);
    tu->deparser_set_copy_to_cpu_pipe_vector(pipe, 0x0F);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    EXPECT_FALSE(nullptr == new_pkt);
    EXPECT_TRUE(nullptr == mirror_pkt);
    EXPECT_TRUE(nullptr == resubmit_pkt);
    EXPECT_TRUE(new_pkt->i2qing_metadata()->cpu_needs_copy());
    EXPECT_EQ(3u, cnt_i_phv_ingress->read());
    EXPECT_EQ(3u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(3u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(1u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());
    EXPECT_EQ(2u, cnt_i_read->read());
    EXPECT_EQ(2u, cnt_pkts_ingress->read());
    EXPECT_EQ(1u, cnt_i_discard->read());
    EXPECT_EQ(1u, i_disc_pkts->read());
    EXPECT_EQ(0u, e_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());
    EXPECT_EQ(0u, e_mirr_pkts->read());

    // unless we set drop ctl bits[1] in PHV
    phv.clobber_d(Phv::make_word_d(4,2), 0x3u);
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    EXPECT_TRUE(nullptr == new_pkt);
    EXPECT_TRUE(nullptr == mirror_pkt);
    EXPECT_TRUE(nullptr == resubmit_pkt);
    EXPECT_EQ(4u, cnt_i_phv_ingress->read());
    EXPECT_EQ(4u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(4u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(2u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());
    EXPECT_EQ(2u, cnt_i_read->read());
    EXPECT_EQ(2u, cnt_pkts_ingress->read());
    EXPECT_EQ(2u, cnt_i_discard->read());
    EXPECT_EQ(2u, i_disc_pkts->read());
    EXPECT_EQ(0u, e_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());
    EXPECT_EQ(0u, e_mirr_pkts->read());

    // set to mirror - packet can still be dropped
    create_packet(tu, &phv, false);
    phv.set(Phv::make_word(1,12), 0x131Cu);
    tu->deparser_set_mirror_cfg(pipe, true, Phv::make_word(1,12), true);
    phv.set(Phv::make_word(5,2), 0x0F1Cu);
    phv.set_valid(Phv::make_word(5,2), true);
    tu->deparser_set_mirror_table_entry(pipe, 4, true, Phv::make_word(5,2), 0, true);
    cnt_i_discard->write_max();  // set counter to max value
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    EXPECT_TRUE(nullptr == new_pkt);
    EXPECT_FALSE(nullptr == mirror_pkt);
    EXPECT_TRUE(nullptr == resubmit_pkt);
    EXPECT_EQ(5u, cnt_i_phv_ingress->read());
    EXPECT_EQ(5u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(5u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(3u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());
    EXPECT_EQ(2u, cnt_i_read->read());
    EXPECT_EQ(2u, cnt_pkts_ingress->read());
    EXPECT_EQ(0u, cnt_i_discard->read());  // counter should wrap
    EXPECT_EQ(3u, i_disc_pkts->read());
    EXPECT_EQ(0u, e_disc_pkts->read());
    EXPECT_EQ(1u, i_mirr_pkts->read());
    EXPECT_EQ(0u, e_mirr_pkts->read());

    // mirror packet also 'dropped' if ctl bits[2] is set in PHV
    create_packet(tu, &phv, false);
    phv.clobber_d(Phv::make_word_d(4,2), 0x7u);
    cnt_i_phv_ingress->write_max();  // set phv counter to max
    i_disc_pkts->write_max();  // set discard counter to max
    new_pkt = deparser_block->DeparseIngress(phv, &lq, &mirror_pkt, &resubmit_pkt,&packet_gen_metadata);
    EXPECT_TRUE(nullptr == new_pkt);
    EXPECT_TRUE(nullptr == mirror_pkt);
    EXPECT_TRUE(nullptr == resubmit_pkt);
    EXPECT_EQ(0u, cnt_i_phv_ingress->read());  // counter should wrap to zero
    EXPECT_EQ(6u, cnt_i_tphv_ingress->read());
    EXPECT_EQ(6u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
    EXPECT_EQ(4u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());
    EXPECT_EQ(2u, cnt_i_read->read());
    EXPECT_EQ(2u, cnt_pkts_ingress->read());
    EXPECT_EQ(1u, cnt_i_discard->read());
    EXPECT_EQ(0u, i_disc_pkts->read());  // counter should wrap to zero
    EXPECT_EQ(0u, e_disc_pkts->read());
    EXPECT_EQ(1u, i_mirr_pkts->read());
    EXPECT_EQ(0u, e_mirr_pkts->read());

    delete cnt_i_phv_ingress;
    delete cnt_i_tphv_ingress;
    delete cnt_i_read;
    delete cnt_i_discard;
    delete cnt_pkts_ingress;
    delete i_disc_pkts;
    delete e_disc_pkts;
    delete tu;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TEST_DROP_CTL_EGRESS) {
    Packet *mirror_pkt = nullptr;
    int pipe = 1;
    TestUtil *tu = nullptr;
    Phv phv(nullptr, nullptr);
    tu = setup_deparser(202, pipe, &phv, 0, true);
    ASSERT_TRUE(nullptr != tu);

    DeparserBlock *deparser_block = tu->get_objmgr()->deparser_get(pipe);
    ASSERT_TRUE(deparser_block != nullptr);

    auto *cnt_i_phv_egress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.ier.main_e.cnt_i_phv, 48);
    EXPECT_EQ(0u, cnt_i_phv_egress->read());  // sanity check
    auto *cnt_i_tphv_egress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->inp.ier.main_e.cnt_i_tphv, 48);
    EXPECT_EQ(0u, cnt_i_tphv_egress->read());  // sanity check
    auto *cnt_pkts_egress = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.cnt_pkts, 48);
    EXPECT_EQ(0u, cnt_pkts_egress->read());  // sanity check
    auto *i_disc_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_disc_pkts, 48);
    EXPECT_EQ(0u, i_disc_pkts->read());  // sanity check
    auto *e_disc_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.e_disc_pkts, 48);
    EXPECT_EQ(0u, e_disc_pkts->read());  // sanity check
    auto *i_mirr_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_mirr_pkts, 48);
    EXPECT_EQ(0u, i_mirr_pkts->read());  // sanity check
    auto *e_mirr_pkts = tu->new_fake_register(
        &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.e_mirr_pkts, 48);
    EXPECT_EQ(0u, e_mirr_pkts->read());  // sanity check

    // sanity check - packet is not dropped
    auto new_pkt = deparser_block->DeparseEgress(phv, &mirror_pkt);
    ASSERT_FALSE(nullptr == new_pkt);
    EXPECT_TRUE(nullptr == mirror_pkt);
    EXPECT_EQ(1u, cnt_i_phv_egress->read());
    EXPECT_EQ(1u, cnt_i_phv_egress->read());
    EXPECT_EQ(1u, cnt_pkts_egress->read());
    EXPECT_EQ(0u, i_disc_pkts->read());
    EXPECT_EQ(0u, e_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());
    EXPECT_EQ(0u, e_mirr_pkts->read());

    // set drop ctl bits[0] in PHV and set drop_ctl_info to read this from PHV
    phv.clobber_d(Phv::make_word_d(4,2), 0x1u);
    tu->deparser_set_drop_ctl_info(pipe, Phv::make_word_d(4,2), true, 0, false);
    // packet should now be dropped
    new_pkt = deparser_block->DeparseEgress(phv, &mirror_pkt);
    EXPECT_TRUE(nullptr == new_pkt);
    EXPECT_TRUE(nullptr == mirror_pkt);
    EXPECT_EQ(2u, cnt_i_phv_egress->read());
    EXPECT_EQ(2u, cnt_i_phv_egress->read());
    EXPECT_EQ(1u, cnt_pkts_egress->read());
    EXPECT_EQ(0u, i_disc_pkts->read());
    EXPECT_EQ(1u, e_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());
    EXPECT_EQ(0u, e_mirr_pkts->read());

    // set to mirror - packet can still be dropped
    create_packet(tu, &phv, true);
    phv.set(Phv::make_word(1,12), 0x131Cu);
    tu->deparser_set_mirror_cfg(pipe, false, Phv::make_word(1,12), true);
    phv.set(Phv::make_word(5,2), 0x0F1Cu);
    phv.set_valid(Phv::make_word(5,2), true);
    tu->deparser_set_mirror_table_entry(pipe, 4, false, Phv::make_word(5,2), 0, true);
    new_pkt = deparser_block->DeparseEgress(phv, &mirror_pkt);
    EXPECT_TRUE(nullptr == new_pkt);
    EXPECT_FALSE(nullptr == mirror_pkt);
    EXPECT_EQ(3u, cnt_i_phv_egress->read());
    EXPECT_EQ(3u, cnt_i_tphv_egress->read());
    EXPECT_EQ(1u, cnt_pkts_egress->read());
    EXPECT_EQ(0u, i_disc_pkts->read());
    EXPECT_EQ(2u, e_disc_pkts->read());
    EXPECT_EQ(0u, i_mirr_pkts->read());
    EXPECT_EQ(1u, e_mirr_pkts->read());

    // mirror packet also 'dropped' if ctl bits[2] is set in PHV
    create_packet(tu, &phv, true);
    phv.clobber_d(Phv::make_word_d(4,2), 0x7u);
    cnt_i_phv_egress->write_max();  // set phv counter to max
    e_disc_pkts->write_max();  // set discard counter to max
    new_pkt = deparser_block->DeparseEgress(phv, &mirror_pkt);
    EXPECT_TRUE(nullptr == new_pkt);
    EXPECT_TRUE(nullptr == mirror_pkt);
    EXPECT_EQ(0u, cnt_i_phv_egress->read());  // counter should wrap to zero
    EXPECT_EQ(4u, cnt_i_tphv_egress->read());
    EXPECT_EQ(1u, cnt_pkts_egress->read());
    EXPECT_EQ(0u, i_disc_pkts->read());
    EXPECT_EQ(0u, e_disc_pkts->read());  // counter should wrap to zero
    EXPECT_EQ(0u, i_mirr_pkts->read());
    EXPECT_EQ(1u, e_mirr_pkts->read());

    delete cnt_i_phv_egress;
    delete cnt_i_tphv_egress;
    delete cnt_pkts_egress;
    delete i_disc_pkts;
    delete e_disc_pkts;
    delete tu;
  }

  void check_counter(TestUtil *tu,
                     volatile void *addr,
                     int width,
                     const std::function<void()> &incrementer,
                     std::string counter_name) {
    auto *test_counter = tu->new_fake_register(addr, width);
    EXPECT_EQ(0u, test_counter->read()) << counter_name;  // sanity check
    test_counter->write(0xabba);
    EXPECT_EQ(0xabbau, test_counter->read()) << counter_name;

    if (nullptr != incrementer) {
      incrementer();
      EXPECT_EQ(0xabbbu, test_counter->read()) << counter_name;

      test_counter->write_max();
      incrementer();
      EXPECT_EQ(0u, test_counter->read()) << counter_name;

      incrementer();
      EXPECT_EQ(1u, test_counter->read()) << counter_name;
    }

    delete test_counter;
  }

  TEST(BFN_TEST_NAME(DeparseTest),TestCountersWrap) {
    // verify that counters can be read/written and, where applicable,
    // incremented and wrap
    GLOBAL_MODEL->Reset();
    int pipe = 1;
    TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(),202, pipe);
    auto *deparser_reg = tu->get_objmgr()->deparser_get(pipe)->deparser_reg();

    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.cnt_i_phv, 48,
        [deparser_reg]() { deparser_reg->increment_phv_counter(false); },
        "cnt_i_phv_ingress");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->inp.ier.main_e.cnt_i_phv, 48,
        [deparser_reg]() { deparser_reg->increment_phv_counter(true); },
        "cnt_i_phv_egress");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->inp.iir.main_i.cnt_i_tphv, 48,
        [deparser_reg]() { deparser_reg->increment_tphv_counter(false); },
        "cnt_i_tphv_ingress");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->inp.ier.main_e.cnt_i_tphv, 48,
        [deparser_reg]() { deparser_reg->increment_tphv_counter(true); },
        "cnt_i_tphv_egress");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_resubmit, 48,
        [deparser_reg]() { deparser_reg->increment_resubmit_counter(); },
        "cnt_i_resubmit");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_read, 48,
        [deparser_reg]() { deparser_reg->increment_read_counter(); },
        "cnt_i_read");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_discard, 48,
        [deparser_reg]() { deparser_reg->increment_discard_counter(); },
        "cnt_i_discard");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->inp.iir.ingr.cnt_i_learn, 48,
        [deparser_reg]() { deparser_reg->increment_learn_counter(); },
        "cnt_i_learn");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->inp.iir.hdr_too_long_i, 32,
        [deparser_reg]() { deparser_reg->increment_hdr_too_long(false); },
        "hdr_too_long_i");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->inp.ier.hdr_too_long_e, 32,
        [deparser_reg]() { deparser_reg->increment_hdr_too_long(true); },
        "hdr_too_long_e");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.cnt_pkts, 48,
        [deparser_reg]() { deparser_reg->increment_cnt_pkts(false); },
        "cnt_pkts_i");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.cnt_pkts, 48,
        [deparser_reg]() { deparser_reg->increment_cnt_pkts(true); },
        "cnt_pkts_e");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_fwd_pkts, 48,
        [deparser_reg]() { deparser_reg->increment_fwd_pkts(false); },
        "i_fwd_pkts");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.e_fwd_pkts, 48,
        [deparser_reg]() { deparser_reg->increment_fwd_pkts(true); },
        "e_fwd_pkts");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_disc_pkts, 48,
        [deparser_reg]() { deparser_reg->increment_disc_pkts(false); },
        "i_disc_pkts");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.e_disc_pkts, 48,
        [deparser_reg]() { deparser_reg->increment_disc_pkts(true); },
        "e_disc_pkts");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_mirr_pkts, 48,
        [deparser_reg]() { deparser_reg->increment_mirr_pkts(false); },
        "i_mirr_pkts");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.e_mirr_pkts, 48,
        [deparser_reg]() { deparser_reg->increment_mirr_pkts(true); },
        "e_mirr_pkts");

    //immutable counters...
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_egr_pkt_err, 32,
        nullptr, "i_egr_pkt_err");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.e_egr_pkt_err, 32,
        nullptr, "e_egr_pkt_err");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.i_ctm_pkt_err, 32,
        nullptr, "i_ctm_pkt_err");
    check_counter(
        tu, &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.e_ctm_pkt_err, 32,
        nullptr, "e_ctm_pkt_err");
    char msg[30];
    for (int i = 0; i < 48; i++) {
      //verify each per-port member of counter arrays
      sprintf(msg, "ctm_pkt_err_i (index %d)", i);
      check_counter(
          tu, &RegisterUtils::addr_dprsr(pipe)->out_ingr.regs.ctm_crc_err[i],
          16, nullptr, msg);
      sprintf(msg, "ctm_pkt_err_e (index %d)", i);
      check_counter(
          tu, &RegisterUtils::addr_dprsr(pipe)->out_egr.regs.ctm_crc_err[i],
          16, nullptr, msg);
    }
    delete tu;
  }

}
