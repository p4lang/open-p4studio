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
#include <string>
#include <array>
#include <cassert>
#include <random>

#include "gtest.h"

#include <utests/test_namespace.h>
#include <model_core/model.h>
#include <register_utils.h>
#include <rmt-object-manager.h>
#include <rmt-packet-coordinator.h>
#include <queueing.h>
#include <port.h>
#include <packet-replication-engine.h>
#include <mem_utils.h>


extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  using namespace MODEL_CHIP_NAMESPACE;


// Only build most of these tests for Tofino
//
  TEST(BFN_TEST_NAME(QueueingTest), Reg) {
    assert(GLOBAL_MODEL.get());

    //printf("1.#################\n");
    //GLOBAL_MODEL->print_the_indirect_subscribers_on_chip_zero();

    RmtObjectManager *om;
    GLOBAL_MODEL->DestroyChip(0);
    GLOBAL_MODEL->CreateChip(0, RmtDefs::kChipType);
    GLOBAL_MODEL->InitChip(0);
    GLOBAL_MODEL->GetObjectManager(0, &om);
    assert(om != NULL);

    //printf("2.#################\n");
    //GLOBAL_MODEL->print_the_indirect_subscribers_on_chip_zero();
    //om->chip_init_all();

    //printf("3.#################\n");
    //GLOBAL_MODEL->print_the_indirect_subscribers_on_chip_zero();

    PacketReplicationEngine *pre[RmtDefs::kPresTotal] = {};
    for (int i=0; i < RmtDefs::kPresTotal; ++i) {
      pre[i] = om->pre_get(i);
    }

    uint64_t x=0, y=0, x_expect=0, y_expect=0;

    // RDM
    x = 0x1234500000000000;
    x |= (static_cast<uint64_t>(RdmDefs::RdmNodeType::kInvalid)) << RdmDefs::kType1Shift;
    x |= (static_cast<uint64_t>(RdmDefs::RdmNodeType::kL1RidEnd)) << RdmDefs::kType2Shift;
    GLOBAL_MODEL->IndirectWrite(0,
                                BFN_MEM_TM_PRE(rdm_mem_word_address)/16,
                                x, UINT64_C(0xABCD));

    x = 0x6789A00000000000;
    x |= (static_cast<uint64_t>(RdmDefs::RdmNodeType::kInvalid)) << RdmDefs::kType1Shift;
    x |= (static_cast<uint64_t>(RdmDefs::RdmNodeType::kL1RidEnd)) << RdmDefs::kType2Shift;
    GLOBAL_MODEL->IndirectWrite(0,
                                BFN_MEM_TM_PRE(rdm_mem_word_address)/16 + 2048,
                                x, UINT64_C(0xEF12));

    x = 0;
    y = 0;
    x_expect = 0x1234500000000000;
    x_expect |= (static_cast<uint64_t>(RdmDefs::RdmNodeType::kL1RidEnd)) << RdmDefs::kType2Shift;
    GLOBAL_MODEL->IndirectRead(0,
                               BFN_MEM_TM_PRE(rdm_mem_word_address)/16,
                               &x, &y);
    EXPECT_EQ(x_expect, x);
    EXPECT_EQ(UINT64_C(0xABCD), y);

    GLOBAL_MODEL->IndirectRead(0,
                               BFN_MEM_TM_PRE(rdm_mem_word_address)/16 + 2048,
                               &x, &y);
    x_expect = 0x6789A00000000000;
    x_expect |= (static_cast<uint64_t>(RdmDefs::RdmNodeType::kL1RidEnd)) << RdmDefs::kType2Shift;
    EXPECT_EQ(x_expect, x);
    EXPECT_EQ(UINT64_C(0xEF12), y);

    uint32_t next_L1=0, next_L2=0;
    uint16_t xid=0, rid=0;
    bool xid_valid=false;
    bool rid_hash=false;
    pre[0]->rdm()->decode_L1(1, 0, 0, next_L1, xid, xid_valid, rid, next_L2, rid_hash);
    EXPECT_EQ(0u, next_L1);
    EXPECT_EQ(0x12345u, next_L2);
    EXPECT_EQ(0u, xid);
    EXPECT_EQ(0, xid_valid);
    EXPECT_EQ(0xABCD, rid);
    pre[0]->rdm()->decode_L1(2*2048+1, 0, 0, next_L1, xid, xid_valid, rid, next_L2, rid_hash);
    EXPECT_EQ(0u, next_L1);
    EXPECT_EQ(0x6789Au, next_L2);
    EXPECT_EQ(0u, xid);
    EXPECT_EQ(0, xid_valid);
    EXPECT_EQ(0xEF12, rid);

    // MIT
    GLOBAL_MODEL->IndirectWrite(0,
                                MemUtils::pre_mit_mem_address(0)/16,
                                UINT64_C(0x40000400), UINT64_C(0x40));
    GLOBAL_MODEL->IndirectWrite(0,
                                MemUtils::pre_mit_mem_address(1)/16+64,
                                UINT64_C(0x4333332222211111), UINT64_C(0x4444));
    GLOBAL_MODEL->IndirectRead(0,
                               MemUtils::pre_mit_mem_address(0)/16,
                               &x, &y);
    EXPECT_EQ(UINT64_C(0x40000400), x);
    EXPECT_EQ(UINT64_C(0x40), y);
    GLOBAL_MODEL->IndirectRead(0,
                               MemUtils::pre_mit_mem_address(1)/16+64,
                               &x, &y);
    EXPECT_EQ(UINT64_C(0x4333332222211111), x);
    EXPECT_EQ(UINT64_C(0x4444), y);

    ASSERT_TRUE(pre[0]->mit(0) == 0x00400);
    ASSERT_TRUE(pre[0]->mit(1) == 0x00400);
    ASSERT_TRUE(pre[0]->mit(2) == 0x00000);
    ASSERT_TRUE(pre[0]->mit(3) == 0x00400);

    ASSERT_TRUE(pre[1]->mit(256) == 0x11111);
    ASSERT_TRUE(pre[1]->mit(257) == 0x22222);
    ASSERT_TRUE(pre[1]->mit(258) == 0x33333);
    ASSERT_TRUE(pre[1]->mit(259) == 0x44444);


    // PBT
    GLOBAL_MODEL->IndirectWrite(0,
                                BFN_MEM_TM_PRE(pbt0_mem_word_address)/16,
                                UINT64_C(0xFFFFFFFFFFFFF51F), ~UINT64_C(0));
    GLOBAL_MODEL->IndirectWrite(0,
                                BFN_MEM_TM_PRE(pbt1_mem_word_address)/16+MemUtils::pre_pbt_mem_size()-1,
                                UINT64_C(0x43333322222110AB), UINT64_C(0xFFEEEEEDDDDDCCCC));
    GLOBAL_MODEL->IndirectRead(0,
                               BFN_MEM_TM_PRE(pbt0_mem_word_address)/16,
                               &x, &y);
    x_expect = 0xFFFFFFFFFFFFF51F & MemUtils::pre_pbt_entry_mask();
    EXPECT_EQ(x_expect, x);
    EXPECT_EQ(UINT64_C(0), y);
    GLOBAL_MODEL->IndirectRead(0,
                               BFN_MEM_TM_PRE(pbt1_mem_word_address)/16+MemUtils::pre_pbt_mem_size()-1,
                               &x, &y);
    x_expect = 0x43333322222110AB & MemUtils::pre_pbt_entry_mask();
    EXPECT_EQ(x_expect, x);
    EXPECT_EQ(UINT64_C(0x0), y);

    // LAG NP
    GLOBAL_MODEL->IndirectWrite(0,
                                BFN_MEM_TM_PRE(lit0_np_mem_word_address)/16,
                                UINT64_C(0xFFFFFFFFFFFFFFFF), ~UINT64_C(0));
    GLOBAL_MODEL->IndirectWrite(0,
                                BFN_MEM_TM_PRE(lit1_np_mem_word_address)/16+MemUtils::pre_lit_np_mem_size()-1,
                                UINT64_C(0xFEDCBA9876543210), UINT64_C(0xFFEEEEEDDDDDCCCC));
    GLOBAL_MODEL->IndirectRead(0,
                               BFN_MEM_TM_PRE(lit0_np_mem_word_address)/16,
                               &x, &y);
    EXPECT_EQ(UINT64_C(0x3FFFFFF), x);
    EXPECT_EQ(UINT64_C(0), y);
    GLOBAL_MODEL->IndirectRead(0,
                               BFN_MEM_TM_PRE(lit1_np_mem_word_address)/16+MemUtils::pre_lit_np_mem_size()-1,
                               &x, &y);
    EXPECT_EQ(UINT64_C(0x2543210), x);
    EXPECT_EQ(UINT64_C(0x0), y);
    // Clear the NP memory so it doesn't affect later tests.
    GLOBAL_MODEL->IndirectWrite(0, BFN_MEM_TM_PRE(lit0_np_mem_word_address)/16, 0, 0);
    GLOBAL_MODEL->IndirectWrite(0, BFN_MEM_TM_PRE(lit1_np_mem_word_address)/16+MemUtils::pre_lit_np_mem_size()-1, 0, 0);

    // LAG Membership
    GLOBAL_MODEL->IndirectWrite(0,
                                BFN_MEM_TM_PRE(lit0_bm_mem_word0_address)/16,
                                UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF));
    GLOBAL_MODEL->IndirectWrite(0,
                                BFN_MEM_TM_PRE(lit0_bm_mem_word1_address)/16+MemUtils::pre_lit_mem_size()-1,
				UINT64_C(0x0123456789ABCDEF), UINT64_C(0x1122334455667788));
    GLOBAL_MODEL->IndirectWrite(0,
                                BFN_MEM_TM_PRE(lit0_bm_mem_word3_address)/16+MemUtils::pre_lit_mem_size()-1,
                                UINT64_C(0x99AABBCCDDEEFF00), UINT64_C(0x8877665544332211));
    GLOBAL_MODEL->IndirectWrite(0,
                                BFN_MEM_TM_PRE(lit1_bm_mem_word0_address)/16,
                                UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0));
    GLOBAL_MODEL->IndirectWrite(0,
                                BFN_MEM_TM_PRE(lit1_bm_mem_word3_address)/16+MemUtils::pre_lit_mem_size()-1,
                                UINT64_C(0xFEDCBA9876543210), UINT64_C(0xFFEEEEEDDDDDCCCC));
    GLOBAL_MODEL->IndirectRead(0,
                               BFN_MEM_TM_PRE(lit0_bm_mem_word0_address)/16,
                               &x, &y);
    x_expect = 0xFFFFFFFFFFFFFFFF & MemUtils::pre_lit_entry_lower_mask();
    y_expect = 0xFF & MemUtils::pre_lit_entry_upper_mask();
    EXPECT_EQ(x_expect, x);
    EXPECT_EQ(y_expect, y);
    GLOBAL_MODEL->IndirectRead(0,
                               BFN_MEM_TM_PRE(lit0_bm_mem_word1_address)/16+MemUtils::pre_lit_mem_size()-1,
                               &x, &y);
    x_expect = 0x0123456789ABCDEF & MemUtils::pre_lit_entry_lower_mask();
    y_expect = 0x88 & MemUtils::pre_lit_entry_upper_mask();
    EXPECT_EQ(x_expect, x);
    EXPECT_EQ(y_expect, y);
    GLOBAL_MODEL->IndirectRead(0,
                               BFN_MEM_TM_PRE(lit0_bm_mem_word3_address)/16+MemUtils::pre_lit_mem_size()-1,
                               &x, &y);
    x_expect = 0x99AABBCCDDEEFF00 & MemUtils::pre_lit_entry_lower_mask();
    y_expect = 0x11 & MemUtils::pre_lit_entry_upper_mask();
    EXPECT_EQ(x_expect, x);
    EXPECT_EQ(y_expect, y);
    GLOBAL_MODEL->IndirectRead(0,
                               BFN_MEM_TM_PRE(lit1_bm_mem_word0_address)/16,
                               &x, &y);
    x_expect = 0xFFFFFFFFFFFFFFFF & MemUtils::pre_lit_entry_lower_mask();
    y_expect = 0x00 & MemUtils::pre_lit_entry_upper_mask();
    EXPECT_EQ(x_expect, x);
    EXPECT_EQ(y_expect, y);
    x_expect = 0xFEDCBA9876543210 & MemUtils::pre_lit_entry_lower_mask();
    y_expect = 0xCC & MemUtils::pre_lit_entry_upper_mask();
    GLOBAL_MODEL->IndirectRead(0,
                               BFN_MEM_TM_PRE(lit1_bm_mem_word3_address)/16+MemUtils::pre_lit_mem_size()-1,
                               &x, &y);
    EXPECT_EQ(x_expect, x);
    EXPECT_EQ(y_expect, y);

    BitVector<PacketReplicationEngine::kPortWidth> l0 = pre[0]->lag(0,0);
    BitVector<PacketReplicationEngine::kPortWidth> l1 = pre[1]->lag(0,MemUtils::pre_lit_mem_size()-1);
    BitVector<PacketReplicationEngine::kPortWidth> l2 = pre[2]->lag(1,0);
    BitVector<PacketReplicationEngine::kPortWidth> l3 = pre[3]->lag(1,MemUtils::pre_lit_mem_size()-1);
    x_expect = 0xFFFFFFFFFFFFFFFF & MemUtils::pre_lit_entry_lower_mask();
    y_expect = 0xFF & MemUtils::pre_lit_entry_upper_mask();
    EXPECT_EQ(x_expect, l0.get_word(0, 64));
    EXPECT_EQ(y_expect,               l0.get_word(64, 8));
    x_expect = 0x0123456789ABCDEF & MemUtils::pre_lit_entry_lower_mask();
    y_expect = 0x88 & MemUtils::pre_lit_entry_upper_mask();
    EXPECT_EQ(x_expect, l1.get_word(PacketReplicationEngine::kLocalPortWidth+0, 64));
    EXPECT_EQ(y_expect,               l1.get_word(PacketReplicationEngine::kLocalPortWidth+64, 8));
    x_expect = 0x99AABBCCDDEEFF00 & MemUtils::pre_lit_entry_lower_mask();
    y_expect = 0x11 & MemUtils::pre_lit_entry_upper_mask();
    EXPECT_EQ(x_expect, l1.get_word((PacketReplicationEngine::kLocalPortWidth*3)+0, 64));
    EXPECT_EQ(y_expect,               l1.get_word((PacketReplicationEngine::kLocalPortWidth*3)+64, 8));
    x_expect = 0xFFFFFFFFFFFFFFFF & MemUtils::pre_lit_entry_lower_mask();
    y_expect = 0x00 & MemUtils::pre_lit_entry_upper_mask();
    EXPECT_EQ(x_expect, l2.get_word(0, 64));
    EXPECT_EQ(y_expect,               l2.get_word(64, 8));
    x_expect = 0xFEDCBA9876543210 & MemUtils::pre_lit_entry_lower_mask();
    y_expect = 0xCC & MemUtils::pre_lit_entry_upper_mask();
    EXPECT_EQ(x_expect, l3.get_word((PacketReplicationEngine::kLocalPortWidth*3)+0, 64));
    EXPECT_EQ(y_expect,               l3.get_word((PacketReplicationEngine::kLocalPortWidth*3)+64, 8));
    // Clear the LAG membership memory so it doesn't affect later tests.
    GLOBAL_MODEL->IndirectWrite(0, BFN_MEM_TM_PRE(lit0_bm_mem_word0_address)/16,     0,0);
    GLOBAL_MODEL->IndirectWrite(0, BFN_MEM_TM_PRE(lit0_bm_mem_word1_address)/16+MemUtils::pre_lit_mem_size()-1, 0,0);
    GLOBAL_MODEL->IndirectWrite(0, BFN_MEM_TM_PRE(lit0_bm_mem_word3_address)/16+MemUtils::pre_lit_mem_size()-1, 0,0);
    GLOBAL_MODEL->IndirectWrite(0, BFN_MEM_TM_PRE(lit1_bm_mem_word0_address)/16,     0,0);
    GLOBAL_MODEL->IndirectWrite(0, BFN_MEM_TM_PRE(lit1_bm_mem_word3_address)/16+MemUtils::pre_lit_mem_size()-1, 0,0);

    // PMT
    GLOBAL_MODEL->IndirectWrite(0,
                                BFN_MEM_TM_PRE(pmt0_mem_word0_address)/16,
                                UINT64_C(0x8000000000000001), UINT64_C(0xFFFF0009082));
    GLOBAL_MODEL->IndirectWrite(0,
                                BFN_MEM_TM_PRE(pmt1_mem_word3_address)/16+MemUtils::pre_pmt_mem_size()-1,
                                UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000003));
    GLOBAL_MODEL->IndirectRead(0,
                               BFN_MEM_TM_PRE(pmt0_mem_word0_address)/16,
                               &x, &y);
    x_expect = 0x8000000000000001 & MemUtils::pre_pmt_entry_lower_mask();
    y_expect = 0x82 & MemUtils::pre_pmt_entry_upper_mask();
    EXPECT_EQ(x_expect, x);
    EXPECT_EQ(y_expect, y);
    GLOBAL_MODEL->IndirectRead(0,
                               BFN_MEM_TM_PRE(pmt1_mem_word3_address)/16+MemUtils::pre_pmt_mem_size()-1,
                               &x, &y);
    x_expect = 0x0000000000000002 & MemUtils::pre_pmt_entry_lower_mask();
    y_expect = 0x03 & MemUtils::pre_pmt_entry_upper_mask();
    EXPECT_EQ(x_expect, x);
    EXPECT_EQ(y_expect, y);
    BitVector<PacketReplicationEngine::kPortWidth> p0 = pre[2]->pmt(0,0);
    BitVector<PacketReplicationEngine::kPortWidth> p1 = pre[2]->pmt(1,MemUtils::pre_pmt_mem_size()-1);
    x_expect = 0x8000000000000001 & MemUtils::pre_pmt_entry_lower_mask();
    y_expect = 0x82 & MemUtils::pre_pmt_entry_upper_mask();
    EXPECT_EQ(x_expect, p0.get_word(0, 64));
    EXPECT_EQ(y_expect,               p0.get_word(64, 8));
    x_expect = 0x0000000000000002 & MemUtils::pre_pmt_entry_lower_mask();
    y_expect = 0x03 & MemUtils::pre_pmt_entry_upper_mask();
    EXPECT_EQ(x_expect, p1.get_word((PacketReplicationEngine::kLocalPortWidth*3)+0, 64));
    EXPECT_EQ(y_expect,               p1.get_word((PacketReplicationEngine::kLocalPortWidth*3)+64, 8));
    // Clear the PMT memory so it doesn't affect later tests.
    GLOBAL_MODEL->IndirectWrite(0, BFN_MEM_TM_PRE(pmt0_mem_word0_address)/16, 0, 0);
    GLOBAL_MODEL->IndirectWrite(0, BFN_MEM_TM_PRE(pmt1_mem_word3_address)/16+MemUtils::pre_pmt_mem_size()-1, 0, 0);
    p0 = pre[2]->pmt(0,0);
    p1 = pre[2]->pmt(1,MemUtils::pre_pmt_mem_size()-1);
    EXPECT_EQ(UINT64_C(0), p0.get_word(0, 64));
    EXPECT_EQ(UINT64_C(0), p0.get_word(64, 8));
    EXPECT_EQ(UINT64_C(0), p1.get_word((PacketReplicationEngine::kLocalPortWidth*3)+0, 64));
    EXPECT_EQ(UINT64_C(0), p1.get_word((PacketReplicationEngine::kLocalPortWidth*3)+64, 8));

    // FIFO
    if (BFN_MEM_TM_PRE_FIFO_ADDR != BFN_MEM_INVAL_MEM_ADDRESS) {
      x = 20; y = 30;
      GLOBAL_MODEL->IndirectWrite(0,
                                  BFN_MEM_TM_PRE_FIFO_ADDR/16 + 0,
                                  (x << 37) | (y << 26), UINT64_C(0xABCD));
      x = 0x3FF; y = 0x7FF;
      GLOBAL_MODEL->IndirectWrite(0,
                                  BFN_MEM_TM_PRE_FIFO_ADDR/16 + 1,
                                  (x << 37) | (y << 26), UINT64_C(0xABCD));
      x = 0; y = 0;
      GLOBAL_MODEL->IndirectWrite(0,
                                  BFN_MEM_TM_PRE_FIFO_ADDR/16 + 2,
                                  (x << 37) | (y << 26), UINT64_C(0xABCD));
      x = 0xFFFFFF; y = 0xFFFFFFF;
      GLOBAL_MODEL->IndirectWrite(0,
                                  BFN_MEM_TM_PRE_FIFO_ADDR/16 + 3,
                                  (x << 37) | (y << 26), UINT64_C(0xABCD));
      GLOBAL_MODEL->IndirectRead(0,
                                 BFN_MEM_TM_PRE_FIFO_ADDR/16 + 0,
                                           &x, &y);
      EXPECT_EQ((UINT64_C(20) << 37) | (UINT64_C(30) << 26), x);
      EXPECT_EQ(0u, y);
      GLOBAL_MODEL->IndirectRead(0,
                                 BFN_MEM_TM_PRE_FIFO_ADDR/16 + 1,
                                           &x, &y);
      EXPECT_EQ((UINT64_C(0x3FF) << 37) | (UINT64_C(0x7FF) << 26), x);
      EXPECT_EQ(0u, y);
      GLOBAL_MODEL->IndirectRead(0,
                                 BFN_MEM_TM_PRE_FIFO_ADDR/16 + 2,
                                           &x, &y);
      EXPECT_EQ((UINT64_C(0) << 37) | (UINT64_C(0) << 26), x);
      EXPECT_EQ(0u, y);
      GLOBAL_MODEL->IndirectRead(0,
                                 BFN_MEM_TM_PRE_FIFO_ADDR/16 + 3,
                                 &x, &y);
      EXPECT_EQ((UINT64_C(0x3FF) << 37) | (UINT64_C(0x7FF) << 26), x);
      EXPECT_EQ(0u, y);

      EXPECT_EQ(8 * 30,    pre[0]->fifo_sz(0));
      EXPECT_EQ(8 * 0x7FF, pre[0]->fifo_sz(1));
      EXPECT_EQ(8 * 0,     pre[0]->fifo_sz(2));
      EXPECT_EQ(8 * 0x7FF, pre[0]->fifo_sz(3));

      x = 0x000; y = 0x100;
      GLOBAL_MODEL->IndirectWrite(0,
                                  BFN_MEM_TM_PRE_FIFO_ADDR/16 + 0,
                                  (x << 37) | (y << 26), UINT64_C(0xABCD));
      x = 0x100; y = 0x100;
      GLOBAL_MODEL->IndirectWrite(0,
                                  BFN_MEM_TM_PRE_FIFO_ADDR/16 + 1,
                                  (x << 37) | (y << 26), UINT64_C(0xABCD));
      x = 0x200; y = 0x100;
      GLOBAL_MODEL->IndirectWrite(0,
                                  BFN_MEM_TM_PRE_FIFO_ADDR/16 + 2,
                                  (x << 37) | (y << 26), UINT64_C(0xABCD));
      x = 0x300; y = 0x100;
      GLOBAL_MODEL->IndirectWrite(0,
                                  BFN_MEM_TM_PRE_FIFO_ADDR/16 + 3,
                                  (x << 37) | (y << 26), UINT64_C(0xABCD));
    }
    EXPECT_EQ(8 * 0x100, pre[3]->fifo_sz(0));
    EXPECT_EQ(8 * 0x100, pre[2]->fifo_sz(1));
    EXPECT_EQ(8 * 0x100, pre[1]->fifo_sz(2));
    EXPECT_EQ(8 * 0x100, pre[0]->fifo_sz(3));
  }


  TEST(BFN_TEST_NAME(QueueingTest), Basic) {
    RmtObjectManager *om;
    GLOBAL_MODEL->DestroyChip(0);
    GLOBAL_MODEL->CreateChip(0, RmtDefs::kChipType);
    GLOBAL_MODEL->InitChip(0);
    GLOBAL_MODEL->GetObjectManager(0, &om);
    assert(om != NULL);
    const char *pkt_string = "123456789abcdeffedcba987654321";
    std::array<Packet*, 10> p = {};
    Packet *pp = nullptr;
    Queueing *q = om->queueing_get();
    unsigned start_packets = om->n_packets();

    // Stop PRE and packet coordinator since they are being started by default.
    om->pre_stop();
    om->packet_coordinator_get()->stop();

    // Create a few packets to test with
    p[0] = om->pkt_create(pkt_string);
    for (unsigned i=1; i<p.size(); ++i) {
      p[i] = p[0]->clone();
    }

    // Enqueue packets to each port on each pipe.
    // Then dequeue and receive them in the expected order.
    for (int pipe=0; pipe < RmtDefs::kPipesMax; ++pipe) {
      for (int port=0; port<64; ++port) {
        for (auto pkt : p) {
          pkt->i2qing_metadata()->set_egress_unicast_port( (pipe << 7) | port);
          q->enqueue(pkt);
        }
        ASSERT_TRUE(q->hasPackets());
        for (int i=0; i < RmtDefs::kPipesMax; ++i) {
          if (i == pipe)
            ASSERT_TRUE(q->pipeHasPackets(i));
          else
            ASSERT_FALSE(q->pipeHasPackets(i));
        }
        for (unsigned i=0; i<p.size(); ++i) {
          q->dequeue(pipe, pp);
          ASSERT_TRUE(pp == p[i]);
          ASSERT_TRUE(pp->port()->pipe_index() == pipe);
          ASSERT_TRUE(pp->port()->port_index() == ((pipe << 7) | port));
          ASSERT_TRUE(pp->qing2e_metadata()->ing_q_depth() == (i));
          ASSERT_TRUE(pp->qing2e_metadata()->egr_q_depth() == (p.size()-1-i));
        }
        ASSERT_FALSE(q->hasPackets());
        for (int i=0; i < RmtDefs::kPipesMax; ++i) {
          ASSERT_FALSE(q->pipeHasPackets(i));
        }
        q->dequeue(pp);
        ASSERT_TRUE(nullptr == pp);
      }
    }

    for (unsigned i=0; i<p.size(); ++i) {
      int pipe = i & 0x3;
      int port = 72+i;
      p[i]->i2qing_metadata()->set_egress_unicast_port((pipe << 7) | port);
      q->enqueue(p[i]);
    }
    ASSERT_FALSE(q->hasPackets());
    for (int i=0; i < RmtDefs::kPipesMax; ++i) {
      ASSERT_FALSE(q->pipeHasPackets(i));
    }
    q->dequeue(pp);
    ASSERT_TRUE(nullptr == pp);

    // Cleanup
    unsigned end_packets = om->n_packets();
    ASSERT_EQ(end_packets, start_packets);
  }

  TEST(BFN_TEST_NAME(QueueingTest), Basic_Multicast) {
    RmtObjectManager *om;
    GLOBAL_MODEL->DestroyChip(0);
    GLOBAL_MODEL->CreateChip(0, RmtDefs::kChipType);
    GLOBAL_MODEL->InitChip(0);
    GLOBAL_MODEL->GetObjectManager(0, &om);
    assert(om != NULL);
    unsigned start_packets = om->n_packets();
    const char *pkt_string = "123456789abcdeffedcba987654321";
    Packet *p = om->pkt_create(pkt_string);
    Packet *pp = NULL;
    Queueing *q = om->queueing_get();
    PacketReplicationEngine *pre[RmtDefs::kPresTotal] = {};
    Rdm *rdm = om->rdm_get();
    om->packet_coordinator_get()->stop();
    for (int i=0; i < RmtDefs::kPresTotal; ++i) {
      pre[i] = om->pre_get(i);
      pre[i]->reset();
    }

    for (int i=0; i < RmtDefs::kPresTotal; ++i) {
      int base = (i+1)*0x10000;
      pre[i]->mit(1, base);

      int rdm_blk = base >> 12;
      int rdm_word_off = RegisterUtils::rdm_blk_word_off(rdm_blk);
      int rdm_bit_off = RegisterUtils::rdm_blk_bit_off(rdm_blk);
      GLOBAL_MODEL->OutWord(0,
                            RegisterUtils::offset_tm_pre_common_blk() + 4 * rdm_word_off,
                            i << rdm_bit_off);

      Rdm::RdmLine l = {};
      // Multicast tree:
      // L1 0x10000 --> 0x10010 --> X
      // L2 0x10020     0x10030
      l.type[0] = RdmDefs::RdmNodeType::kL1Rid;
      l.u.L1Rid.next_L1 = base + 0x10;
      l.u.L1Rid.next_L2 = base + 0x20;
      l.u.L1Rid.rid     = 0x1234;
      rdm->write_line(base >> 1, l);
      l.type[0] = RdmDefs::RdmNodeType::kL1Rid;
      l.u.L1Rid.next_L1 = 0x000;
      l.u.L1Rid.next_L2 = base + 0x30;
      l.u.L1Rid.rid     = 0x5678;
      rdm->write_line((base+0x10) >> 1, l);
      l.type[0] = RdmDefs::RdmNodeType::kL2Port16;
      l.u.L2Port16[0].spv   = 0;
      l.u.L2Port16[0].ports = 0x0010;
      l.u.L2Port16[0].pipe  = i;
      l.u.L2Port16[0].chip  = 0;
      l.u.L2Port16[0].last  = true;
      rdm->write_line((base+0x20) >> 1, l);
      l.type[0] = RdmDefs::RdmNodeType::kL2Port64;
      l.u.L2Port64.spv   = 0;
      l.u.L2Port64.ports = 0x0002;
      l.u.L2Port64.pipe  = i;
      l.u.L2Port64.chip  = 0;
      l.u.L2Port64.last  = true;
      rdm->write_line((base+0x30) >> 1, l);
    }

    // Inject one packet for replication to pipes 0,1, and 3.
    // It will also unicast to pipe 2 port 50
    p->i2qing_metadata()->set_egress_unicast_port((2 << 7) | 50);
    p->i2qing_metadata()->set_mgid1(1);
    p->i2qing_metadata()->set_pipe_mask(0xB);
    q->enqueue(p);

    // Wait for all copies to be made.
    bool has_packets = false;
    do {
      has_packets = false;
      std::this_thread::yield();
        for (int i=0; i < RmtDefs::kPresTotal; ++i)
          has_packets = has_packets || !pre[i]->fifos_empty();
    } while(has_packets);

    // Logic below used to check *all* pipes.
    // But this can be >4 these days - so limit to 4
    int pipesToCheck = 4;
    ASSERT_TRUE(q->hasPackets());
    for (int i=0; i < pipesToCheck; ++i) {
      ASSERT_TRUE(q->pipeHasPackets(i));
    }

    // Verify the unicast copy is the only copy in pipe 2.
    q->dequeue(2, pp);
    ASSERT_TRUE(pp != p); // In UC+MC cases, MC gets the original copy.
    ASSERT_TRUE(pp->port()->pipe_index() == 2);
    ASSERT_TRUE(pp->port()->port_index() == ((2 << 7) | 50));
    om->pkt_delete(pp);
    q->dequeue(2, pp);
    ASSERT_TRUE(pp == nullptr);
    ASSERT_FALSE(q->pipeHasPackets(2));

    // Verify the multicast copies in pipes 0, 1, and 3.
    std::array<bool, 6> pkt_seen = {};
    for (int i=0; i<6; ++i) {
      q->dequeue(pp);
      int pipe = pp->port()->pipe_index();
      int port = pp->port()->port_index() & 0x7F;
      ASSERT_TRUE(0 == pipe || 1 == pipe || 3 == pipe);
      ASSERT_TRUE(1 == port || 16 == port);
      int idx = 2 * (pipe == 3 ? 2 : pipe) + (port == 1 ? 0 : 1);
      ASSERT_FALSE(pkt_seen[idx]);
      // N.B. Mirror logic is handled by PRE only for FTR. Field defaults to false.
      ASSERT_FALSE(pp->qing2e_metadata()->mir_copy());
      pkt_seen[idx] = true;
      om->pkt_delete(pp);
    }
    for (int i=0; i<6; ++i) {
      ASSERT_TRUE(pkt_seen[i]);
    }
    q->dequeue(pp);
    ASSERT_TRUE(nullptr == pp);

    // Cleanup
    unsigned end_packets = om->n_packets();
    ASSERT_EQ(end_packets, start_packets);
  }


  TEST(BFN_TEST_NAME(DISABLED_QueueingTest), Multicast_LagOneMbr) {
    RmtObjectManager *om;
    GLOBAL_MODEL->DestroyChip(0);
    GLOBAL_MODEL->CreateChip(0, RmtDefs::kChipType);
    GLOBAL_MODEL->InitChip(0);
    GLOBAL_MODEL->GetObjectManager(0, &om);
    assert(om != NULL);
    unsigned start_packets = om->n_packets();
    const char *pkt_string = "123456789abcdeffedcba987654321";
    Queueing *q = om->queueing_get();
    std::array<PacketReplicationEngine*, RmtDefs::kPresTotal> pre = {};
    Rdm *rdm = om->rdm_get();
    om->packet_coordinator_get()->stop();
    for (int i=0; i < RmtDefs::kPresTotal; ++i) {
      pre[i] = om->pre_get(i);
      pre[i]->reset();
    }

    // Set up the multicast tree.
    for (int i=0; i < RmtDefs::kPresTotal; ++i) {
      int base = 1 + (i*0x10000);

      int rdm_blk = base >> 12;
      int rdm_word_off = RegisterUtils::rdm_blk_word_off(rdm_blk);
      int rdm_bit_off = RegisterUtils::rdm_blk_bit_off(rdm_blk);
      GLOBAL_MODEL->OutWord(0,
                            RegisterUtils::offset_tm_pre_common_blk() + 4 * rdm_word_off,
                            i << rdm_bit_off);
      pre[i]->mit(0xFFFF, base);
      Rdm::RdmLine l = {};
      l.type[0] = RdmDefs::RdmNodeType::kInvalid;
      l.type[1] = RdmDefs::RdmNodeType::kL1RidEnd;
      l.u.L1RidEnd[1].next_L2 = base+1;
      l.u.L1RidEnd[1].rid = 0x1234;
      rdm->write_line(base >> 1, l);
      l.type[0] = RdmDefs::RdmNodeType::kL2Lag;
      l.type[1] = RdmDefs::RdmNodeType::kInvalid;
      l.u.L2Lag[0].next_L2 = 0;
      l.u.L2Lag[0].lag_id = 0;
      rdm->write_line((base+1) >> 1, l);
    }

    // Set each bit in the LAG vector, one at a time, and verify a packet is
    // created for the correct port.
    for (int lag_mbr=0; lag_mbr<(PacketReplicationEngine::kLocalPortWidth*RmtDefs::kPipesMax); ++lag_mbr) {
      int tgt_pipe = lag_mbr/PacketReplicationEngine::kLocalPortWidth;
      int tgt_port = lag_mbr%PacketReplicationEngine::kLocalPortWidth;
      // Program the LAG table.
      BitVector<PacketReplicationEngine::kPortWidth> entry = {};
      entry.set_bit(1, lag_mbr);
      for (auto x : pre) {
        x->set_lag(0, 0, entry);
      }

      // Inject a packet.
      Packet *p = om->pkt_create(pkt_string);
      Packet *pp1 = NULL, *pp2 = NULL;
      p->i2qing_metadata()->set_mgid1(0xFFFF);
      p->i2qing_metadata()->set_mgid2(0x5678);
      p->i2qing_metadata()->set_pipe_mask(0xF);
      p->set_ph_ver(0);
      p->i2qing_metadata()->set_qid(0);
      q->enqueue(p);

      // Wait for all copies to be made.
      bool has_packets = false;
      do {
        has_packets = false;
        std::this_thread::yield();
          for (int i=0; i < RmtDefs::kPresTotal; ++i)
            has_packets = has_packets || !pre[i]->fifos_empty();
      } while(has_packets);

      // Only a single pipe should have a packet.
      for (int i=0; i < RmtDefs::kPresTotal; ++i) {
        ASSERT_TRUE(q->pipeHasPackets(i) == (tgt_pipe == i));
      }

      // There should only be a single packet generated.
      q->dequeue(pp1);
      ASSERT_TRUE(nullptr != pp1);
      q->dequeue(pp2);
      ASSERT_TRUE(nullptr == pp2);

      // Verify it came to the correct pipe and port.
      ASSERT_TRUE(pp1->port()->pipe_index() == tgt_pipe);
      ASSERT_TRUE(pp1->port()->port_index() == ((tgt_pipe << 7) | tgt_port));

      om->pkt_delete(pp1);
    }

    // Cleanup
    unsigned end_packets = om->n_packets();
    ASSERT_EQ(end_packets, start_packets);
  }


  TEST(BFN_TEST_NAME(DISABLED_QueueingTest), Multicast_LagHash) {
    RmtObjectManager *om;
    GLOBAL_MODEL->DestroyChip(0);
    GLOBAL_MODEL->CreateChip(0, RmtDefs::kChipType);
    GLOBAL_MODEL->InitChip(0);
    GLOBAL_MODEL->GetObjectManager(0, &om);
    assert(om != NULL);
    unsigned start_packets = om->n_packets();
    const char *pkt_string = "123456789abcdeffedcba987654321";
    Queueing *q = om->queueing_get();
    std::array<PacketReplicationEngine*, RmtDefs::kPresTotal> pre = {};
    Rdm *rdm = om->rdm_get();
    om->packet_coordinator_get()->stop();
    for (int i=0; i < RmtDefs::kPresTotal; ++i) {
      pre[i] = om->pre_get(i);
      pre[i]->reset();
    }

    // Set up the multicast tree.
    uint32_t bids[204] = {0}; // RDM Block Ids.
    for (int i=0; i < RmtDefs::kPresTotal; ++i) {
      int base = 1 + i * 0x1000;
      int rdm_blk = base >> 12;
      int rdm_word_off = RegisterUtils::rdm_blk_word_off(rdm_blk);
      int rdm_bit_off = RegisterUtils::rdm_blk_bit_off(rdm_blk);
      bids[rdm_word_off] |= (i << rdm_bit_off);
      GLOBAL_MODEL->OutWord(0,
                            RegisterUtils::offset_tm_pre_common_blk() + 4 * rdm_word_off,
                            bids[rdm_word_off]);
      pre[i]->mit(0xFFFF, base);
      Rdm::RdmLine l = {};
      l.type[0] = RdmDefs::RdmNodeType::kInvalid;
      l.type[1] = RdmDefs::RdmNodeType::kL1RidEnd;
      l.u.L1RidEnd[1].next_L2 = base+1;
      l.u.L1RidEnd[1].rid = 0x1234;
      rdm->write_line(base >> 1, l);
      l.type[0] = RdmDefs::RdmNodeType::kL2Lag;
      l.type[1] = RdmDefs::RdmNodeType::kInvalid;
      l.u.L2Lag[0].next_L2 = 0;
      l.u.L2Lag[0].lag_id = 0;
      rdm->write_line((base+1) >> 1, l);
    }

    // Initialize the LAG member to all zero.
    BitVector<PacketReplicationEngine::kPortWidth> entry = {};
    for (auto x : pre) {
      x->set_lag(0, 0, entry);
    }

    // Set each bit in the LAG vector, one at a time, and verify a packet is
    // always created for the first member set.
    for (int bit_val=1; bit_val>=0; --bit_val) {
      for (int lag_mbr=0; lag_mbr<(PacketReplicationEngine::kLocalPortWidth*RmtDefs::kPipesMax); ++lag_mbr) {
        // Early stop condition to avoid a LAG vector that is all zero.
        if (!bit_val && lag_mbr == ((PacketReplicationEngine::kLocalPortWidth*RmtDefs::kPipesMax)-1)) continue;

        // Program the LAG table.
        for (auto x : pre) {
          x->set_lag_mbr(0, 0, lag_mbr, bit_val);
        }

        // Calculate the expected port to be the first bit set in the LAG table
        // entry.
        int first_mbr = pre[0]->lag(0,0).get_first_bit_set();
        int tgt_pipe = first_mbr/PacketReplicationEngine::kLocalPortWidth;
        int tgt_port = first_mbr%PacketReplicationEngine::kLocalPortWidth;


        // Inject a packet.
        Packet *p = om->pkt_create(pkt_string);
        Packet *pp1 = NULL, *pp2 = NULL;
        p->i2qing_metadata()->set_mgid1(0xFFFF);
        p->i2qing_metadata()->set_mgid2(0x0000);
        p->i2qing_metadata()->set_pipe_mask(0xF);
        p->set_ph_ver(0);
        p->i2qing_metadata()->set_qid(0);
        p->i2qing_metadata()->set_hash2(0);
        q->enqueue(p);

        // Wait for all copies to be made.
        bool has_packets = false;
        do {
          has_packets = false;
          std::this_thread::yield();
            for (int i=0; i < RmtDefs::kPresTotal; ++i)
              has_packets = has_packets || !pre[i]->fifos_empty();
        } while(has_packets);

        // Only a single pipe should have a packet.
        for (int i=0; i < RmtDefs::kPresTotal; ++i) {
          ASSERT_TRUE(q->pipeHasPackets(i) == (tgt_pipe == i));
        }

        // There should only be a single packet generated.
        q->dequeue(pp1);
        ASSERT_TRUE(nullptr != pp1);
        q->dequeue(pp2);
        ASSERT_TRUE(nullptr == pp2);

        // Verify it came to the correct pipe and port.
        ASSERT_TRUE(pp1->port()->pipe_index() == tgt_pipe);
        ASSERT_TRUE(pp1->port()->port_index() == ((tgt_pipe << 7) | tgt_port));

        om->pkt_delete(pp1);
      }
    }

    // Construct a few different LAG membership vectors.
    // LAG entry zero is specifically set so that the HW mask and SW mask will
    // turn off all ports.
    BitVector<PacketReplicationEngine::kPortWidth> lag_table_entry(0x4444444444444444ull);
    // FIX-ME : Constrain to 4 pipes, model does not have multi die support
    if (PacketReplicationEngine::kPortWidth > (PacketReplicationEngine::kLocalPortWidth*RmtDefs::kPipesMax)) {
      for (int i=(PacketReplicationEngine::kLocalPortWidth*RmtDefs::kPipesMax); i<PacketReplicationEngine::kPortWidth; i++)
	lag_table_entry.set_bit(0, i);
    }
    for (auto x : pre) {
      x->set_lag(0, 0, lag_table_entry);
    }
    std::default_random_engine re;
    for (int lag_id=1; lag_id<PacketReplicationEngine::kLags; ++lag_id) {
      std::array<uint64_t, PacketReplicationEngine::kLagArrSize> m = {};
      for (auto &x : m) {
        x = re(); x = (x<<32) | re();
      }
      BitVector<PacketReplicationEngine::kPortWidth> entry(m);
      // FIX-ME : Constrain to 4 pipes, model does not have multi die support
      if (PacketReplicationEngine::kPortWidth > (PacketReplicationEngine::kLocalPortWidth*RmtDefs::kPipesMax)) {
	for (int i=(PacketReplicationEngine::kLocalPortWidth*RmtDefs::kPipesMax); i<PacketReplicationEngine::kPortWidth; i++)
	  entry.set_bit(0, i);
      }
      for (auto x : pre) {
        x->set_lag(0, lag_id, entry);
      }
    }

    // Set up a HW and SW port mask.
    for (int i=0; i<PacketReplicationEngine::kPortWidth; ++i) {
      for (auto x : pre) {
        x->set_hw_mask_bit(i & 1, i);
        x->set_sw_mask_bit(0, i & 2, i);
      }
    }

    for (auto x : pre) {
      x->ctl_hw_mask(true);
      x->ctl_sw_mask(true);
    }
    // Store the number of members in each LAG for later use.
    int mbr_cnt[PacketReplicationEngine::kLags][4] = {};
    // Note, changing the loop limit to 3 to speed up the unit test.
    for (int i=0; i<3/*PacketReplicationEngine::kLags*/; ++i) {
      const BitVector<PacketReplicationEngine::kPortWidth> &l = pre[0]->lag(0, i);
      const BitVector<PacketReplicationEngine::kPortWidth> &s = pre[0]->get_sw_mask(0);
      const BitVector<PacketReplicationEngine::kPortWidth> &h = pre[0]->get_hw_mask();
      BitVector<PacketReplicationEngine::kPortWidth> v = {};
      BitVector<PacketReplicationEngine::kPortWidth> w = {};

      // LAG vector unmasked
      mbr_cnt[i][0] = l.popcount();
      // LAG vector with SW mask applied
      v.copy_from(s);
      v.invert();
      v.mask(l);
      mbr_cnt[i][1] = v.popcount();
      // LAG vector with HW mask applied
      v.copy_from(h);
      v.invert();
      v.mask(l);
      mbr_cnt[i][2] = v.popcount();
      // LAG vector with both mask applied
      v.copy_from(h);
      v.invert();
      w.copy_from(s);
      w.invert();
      v.mask(w);
      v.mask(l);
      mbr_cnt[i][3] = v.popcount();
    }

    // Program the RDM such that there is a tree for each LAG.
    // Note, changing the loop limit to 6 to speed up the unit test.
    memset(bids, 0, sizeof(bids));
    for (int i=0; i<3/*PacketReplicationEngine::kLags*/; ++i) {
      for (int pipe=0; pipe < RmtDefs::kPipesMax; ++pipe) {
        int base = (pipe+4)*0x1000 + 2*i;
        int rdm_blk = base >> 12;
        int rdm_word_off = RegisterUtils::rdm_blk_word_off(rdm_blk);
        int rdm_bit_off = RegisterUtils::rdm_blk_bit_off(rdm_blk);
        bids[rdm_word_off] |= (pipe << rdm_bit_off);
        GLOBAL_MODEL->OutWord(0,
                              RegisterUtils::offset_tm_pre_common_blk() + 4 * rdm_word_off,
                              bids[rdm_word_off]);
        pre[pipe]->mit(i, base);
        Rdm::RdmLine l = {};
        l.type[0] = RdmDefs::RdmNodeType::kL1RidEnd;
        l.type[1] = RdmDefs::RdmNodeType::kL2Lag;
        l.u.L1RidEnd[0].next_L2 = base+1;
        l.u.L1RidEnd[0].rid = i+1;
        l.u.L2Lag[1].next_L2 = 0;
        l.u.L2Lag[1].lag_id = i;
        rdm->write_line(base >> 1, l);
      }
    }

    // For each hash value, send a packet and ensure it comes back on the
    // correct member.
    for (int masks = 0; masks < 4; ++masks) {
      for (auto x : pre) {
        x->ctl_sw_mask(masks & 1);
        x->ctl_hw_mask(masks & 2);
      }
      const BitVector<PacketReplicationEngine::kPortWidth> &s = pre[0]->get_sw_mask(0);
      const BitVector<PacketReplicationEngine::kPortWidth> &h = pre[0]->get_hw_mask();
      BitVector<PacketReplicationEngine::kPortWidth> p_mask = {};
      if (0 == masks) {
        p_mask.fill_all_ones();
      } else if (1 == masks) {
        p_mask.copy_from(s);
        p_mask.invert();
      } else if (2 == masks) {
        p_mask.copy_from(h);
        p_mask.invert();
      } else {
        p_mask.copy_from(h);
        p_mask.or_with(s);
        p_mask.invert();
      }
      // Note, changing the loop limit to 3 to speed up the unit test.
      for (int lag_id = 0; lag_id<3/*PacketReplicationEngine::kLags*/; ++lag_id) {
        for (int hash=0; hash<=0xFF; hash+=3) {
          BitVector<PacketReplicationEngine::kPortWidth> l = pre[0]->lag(0, lag_id);
          BitVector<PacketReplicationEngine::kPortWidth> lm = pre[0]->lag(0, lag_id);
          lm.mask(p_mask);
          uint16_t computed_hash = hash;
          // Find the expected LAG member.
          int mbr = -1;
          int mbr_bit_idx = -1;
          // First pick from the unmasked vector.
          mbr = computed_hash % mbr_cnt[lag_id][0];
          do {
            mbr_bit_idx = l.get_first_bit_set(mbr_bit_idx);
            --mbr;
          } while (mbr >= 0 && mbr_bit_idx != -1);
          // If the masked vector is not empty and the selected member is not
          // in the masked vector then pick again from the masked vector.
          // Otherwise use the member selected from the unmasked vector.
          if (masks && mbr_cnt[lag_id][masks]) {
            if (lm.get_bit(mbr_bit_idx) == 0) {
              mbr = computed_hash % mbr_cnt[lag_id][masks];
              mbr_bit_idx = -1;
              do {
                mbr_bit_idx = lm.get_first_bit_set(mbr_bit_idx);
                --mbr;
              } while (mbr >= 0 && mbr_bit_idx != -1);
            }
          }

          // Based on the bit index of the selected LAG member, calculate the
          // pipe and port number of that member.
          int tgt_pipe = mbr_bit_idx/PacketReplicationEngine::kLocalPortWidth;
          int tgt_port = mbr_bit_idx%PacketReplicationEngine::kLocalPortWidth;

          // Inject a packet.
          Packet *p = om->pkt_create(pkt_string);
          Packet *pp1 = NULL, *pp2 = NULL;
          p->i2qing_metadata()->set_mgid1(lag_id);
          p->i2qing_metadata()->set_pipe_mask(0xF);
          p->set_ph_ver(0);
          p->i2qing_metadata()->set_qid(0);
          p->i2qing_metadata()->set_hash2(hash);
          q->enqueue(p);

          // Wait for all copies to be made.
          bool has_packets = false;
          do {
            has_packets = false;
            std::this_thread::yield();
              for (int i=0; i < RmtDefs::kPresTotal; ++i)
                has_packets = has_packets || !pre[i]->fifos_empty();
          } while(has_packets);

          // Only a single pipe should have a packet.
          for (int i=0; i < RmtDefs::kPresTotal; ++i) {
            ASSERT_TRUE(q->pipeHasPackets(i) == (tgt_pipe == i));
          }

          // There should only be a single packet generated.
          q->dequeue(pp1);
          ASSERT_TRUE(nullptr != pp1);
          q->dequeue(pp2);
          if (pp2) {
              printf("lag_id %d, hash 0x%x, has extra packet\n", lag_id, hash);
              printf("Pipe %d Port 0x%x\n", pp2->port()->pipe_index(), pp2->port()->port_index());
          }
          ASSERT_TRUE(nullptr == pp2);

          // Verify it came to the correct pipe and port.
          ASSERT_TRUE(pp1->port()->pipe_index() == tgt_pipe);
          ASSERT_TRUE(pp1->port()->port_index() == ((tgt_pipe << 7) | tgt_port));
          // Verify it came with the right RID.
          ASSERT_TRUE(pp1->qing2e_metadata()->erid() == (lag_id + 1));

          om->pkt_delete(pp1);
        }
      }
    }


    // Cleanup
    unsigned end_packets = om->n_packets();
    ASSERT_EQ(end_packets, start_packets);
  }

  TEST(BFN_TEST_NAME(QueueingTest), Multicast_Pruning) {
    RmtObjectManager *om;
    GLOBAL_MODEL->DestroyChip(0);
    GLOBAL_MODEL->CreateChip(0, RmtDefs::kChipType);
    GLOBAL_MODEL->InitChip(0);
    GLOBAL_MODEL->GetObjectManager(0, &om);
    assert(om != NULL);
    unsigned start_packets = om->n_packets();
    const char *pkt_string = "123456789abcdeffedcba987654321";
    Queueing *q = om->queueing_get();
    std::array<PacketReplicationEngine*, RmtDefs::kPresTotal> pre = {};
    Rdm *rdm = om->rdm_get();
    om->packet_coordinator_get()->stop();
    for (int i=0; i < RmtDefs::kPresTotal; ++i) {
      pre[i] = om->pre_get(i);
      pre[i]->reset();
    }

    // WIP has 36 ports/pipe vs 72 in Tofino & JBAY
    int adjust_port_num;
    if (PacketReplicationEngine::kLocalPortWidth < 64)
      adjust_port_num = PacketReplicationEngine::kLocalPortWidth;
    else
      adjust_port_num = 0;

    for (int i=0; i < RmtDefs::kPresTotal; ++i) {
      int base = (i+1) * 0x10000;

      int rdm_blk = base >> 12;
      int rdm_word_off = RegisterUtils::rdm_blk_word_off(rdm_blk);
      int rdm_bit_off = RegisterUtils::rdm_blk_bit_off(rdm_blk);
      GLOBAL_MODEL->OutWord(0,
                            RegisterUtils::offset_tm_pre_common_blk() + 4 * rdm_word_off,
                            i << rdm_bit_off);

      pre[i]->mit(0, base);
      // RID, RID-Xid, EcmpPtr, EcmpPtr-Xid, RidEnd
      Rdm::RdmLine l = {};
      l.type[0] = RdmDefs::RdmNodeType::kL1Rid;
      l.u.L1Rid.next_L1 = base+2;
      l.u.L1Rid.next_L2 = base+0x100;
      l.u.L1Rid.rid     = 0x12;
      rdm->write_line(base >> 1, l);

      l.type[0] = RdmDefs::RdmNodeType::kL1RidXid;
      l.u.L1RidXid.next_L1 = base+4;
      l.u.L1RidXid.next_L2 = base+0x102;
      l.u.L1RidXid.rid     = 0x23;
      l.u.L1RidXid.xid     = 0x1111;
      rdm->write_line((base+2) >> 1, l);

      l.type[0] = RdmDefs::RdmNodeType::kL1EcmpPtr;
      l.u.L1EcmpPtr.next_L1 = base+6;
      l.u.L1EcmpPtr.vector0 = base+0x200;
      l.u.L1EcmpPtr.vector1 = base+0x200;
      rdm->write_line((base+4) >> 1, l);

      l.type[0] = RdmDefs::RdmNodeType::kL1EcmpVector;
      l.u.L1EcmpVector.base_L1 = base+0x200+2;
      l.u.L1EcmpVector.length  = 0;
      l.u.L1EcmpVector.vector  = 1;
      rdm->write_line((base+0x200) >> 1, l);

      l.type[0] = RdmDefs::RdmNodeType::kL1RidEnd;
      l.u.L1RidEnd[0].rid = 0x34;
      l.u.L1RidEnd[0].next_L2 = base+0x104;
      rdm->write_line((base+0x202) >> 1, l);

      l.type[0] = RdmDefs::RdmNodeType::kL1EcmpPtrXid;
      l.u.L1EcmpPtrXid.next_L1 = base+8;
      l.u.L1EcmpPtrXid.xid     = 0x2222;
      l.u.L1EcmpPtrXid.vector0 = base+0x300;
      l.u.L1EcmpPtrXid.vector1 = base+0x300;
      rdm->write_line((base+6) >> 1, l);

      l.type[0] = RdmDefs::RdmNodeType::kL1EcmpVector;
      l.u.L1EcmpVector.base_L1 = base+0x300+2;
      l.u.L1EcmpVector.length  = 0;
      l.u.L1EcmpVector.vector  = 1;
      rdm->write_line((base+0x300) >> 1, l);

      l.type[0] = RdmDefs::RdmNodeType::kL1RidEnd;
      l.u.L1RidEnd[0].rid = 0x45;
      l.u.L1RidEnd[0].next_L2 = base+0x106;
      rdm->write_line((base+0x302) >> 1, l);

      l.type[0] = RdmDefs::RdmNodeType::kL1RidEnd;
      l.u.L1RidEnd[0].next_L2 = base+0x108;
      l.u.L1RidEnd[0].rid     = 0x56;
      rdm->write_line((base+8) >> 1, l);

      // L1 RID RID 0x12 XID -- L2 tree: Local Port #s 1 and 64
      // For WIP adjust port num by 36. L2 tree: Local Port #s 1 and 28
      l.type[0] = RdmDefs::RdmNodeType::kL2Port64;
      l.u.L2Port64.chip  = 0;
      l.u.L2Port64.pipe  = i;
      l.u.L2Port64.last  = true;
      if (PacketReplicationEngine::kLocalPortWidth < 64) {
	l.u.L2Port64.spv   = 0;
	l.u.L2Port64.ports = 0x10000002;
      } else {
	l.u.L2Port64.spv   = 1;
	l.u.L2Port64.ports = 2;
      }
      rdm->write_line((base+0x100) >> 1, l);

      // L1 RID-Xid RID 0x23 XID 0x1111 L2 tree: Local Port #s 4 and 68
      // For WIP adjust port num by 36. L2 tree: Local Port #s 4 and 32
      l.type[0] = RdmDefs::RdmNodeType::kL2Port16;
      l.u.L2Port16[0].chip  = 0;
      l.u.L2Port16[0].pipe  = i;
      l.u.L2Port16[0].last  = true;
      if (PacketReplicationEngine::kLocalPortWidth < 64) {
	l.u.L2Port16[0].spv   = 0;
	l.u.L2Port16[0].ports = 0x102;
      } else {
	l.u.L2Port16[0].spv   = 2;
	l.u.L2Port16[0].ports = 2;
      }
      rdm->write_line((base+0x102) >> 1, l);

      // L1 ECMP-Ptr RID 0x34 XID -- L2 tree
      l.type[0] = RdmDefs::RdmNodeType::kL2Lag;
      l.u.L2Lag[0].next_L2 = 0;
      l.u.L2Lag[0].lag_id  = 0;
      rdm->write_line((base+0x104) >> 1, l);
      // Set LAG table.
      pre[i]->set_lag_mbr(0, 0, PacketReplicationEngine::kLocalPortWidth, 1); // Pipe 1 port 0

      // L1 ECMP-Ptr-Xid RID 0x45 XID 0x2222 L2 tree
      l.type[0] = RdmDefs::RdmNodeType::kL2Lag;
      l.u.L2Lag[0].next_L2 = 0;
      l.u.L2Lag[0].lag_id  = 1;
      rdm->write_line((base+0x106) >> 1, l);
      // Set LAG table.
      pre[i]->set_lag_mbr(0, 1, (PacketReplicationEngine::kLocalPortWidth+2), 1); // Pipe 1 port 2

      // L1 RidEnd's RID 0x56 L2 tree: Local Port #s 5 and 65
      // For WIP adjust port num by 36. L2 tree: Local Port #s 5 and 29
      l.type[0] = RdmDefs::RdmNodeType::kL2Port64;
      l.u.L2Port64.chip  = 0;
      l.u.L2Port64.pipe  = i;
      l.u.L2Port64.last  = true;
      if (PacketReplicationEngine::kLocalPortWidth < 64) {
	l.u.L2Port64.spv   = 0;
	l.u.L2Port64.ports = 0x20000020;
      } else {
	l.u.L2Port64.spv   = 2;
	l.u.L2Port64.ports = 0x20;
      }
      rdm->write_line((base+0x108) >> 1, l);
    }

    // Set Global RID.
    for (auto &x : pre) {
      x->global_rid(0);
    }

    // Set YID table.
    for (auto &x : pre) {
      x->set_pmt_bit(0, 0, PacketReplicationEngine::kLocalPortWidth+1,  1);
      x->set_pmt_bit(0, 0, PacketReplicationEngine::kLocalPortWidth+64-adjust_port_num, 1);
      x->set_pmt_bit(0, 0, PacketReplicationEngine::kLocalPortWidth+4,  1);
      x->set_pmt_bit(0, 0, PacketReplicationEngine::kLocalPortWidth+0,  1);
      x->set_pmt_bit(0, 0, PacketReplicationEngine::kLocalPortWidth+5,  1);

      x->set_pmt_bit(0, 1, PacketReplicationEngine::kLocalPortWidth+1,  1);
      x->set_pmt_bit(0, 1, PacketReplicationEngine::kLocalPortWidth+64-adjust_port_num, 1);
      x->set_pmt_bit(0, 1, PacketReplicationEngine::kLocalPortWidth+68-adjust_port_num, 1);
      x->set_pmt_bit(0, 1, PacketReplicationEngine::kLocalPortWidth+65-adjust_port_num, 1);
    }


    // Send packets with various RID/XID/YID combinations
    std::array<int, 7> input_rid = {{0, 0x12, 0x23, 0x34, 0x45, 0x56, 0x83}};
    std::array<int, 4> input_xid = {{0, 0x1111, 0x2222, 0xFEDC}};
    std::array<int, 2> input_yid = {{0, 1}};
    for (unsigned idx_r=0; idx_r<input_rid.size(); ++idx_r) {
      for (unsigned idx_x=0; idx_x<input_xid.size(); ++idx_x) {
        for (unsigned idx_y=0; idx_y<input_yid.size(); ++idx_y) {
          Packet *p = om->pkt_create(pkt_string);
          p->i2qing_metadata()->set_pipe_mask(2);
          p->i2qing_metadata()->set_mgid2(0);
          p->i2qing_metadata()->set_irid(input_rid[idx_r]);
          p->i2qing_metadata()->set_xid(input_xid[idx_x]);
          p->i2qing_metadata()->set_yid(input_yid[idx_y]);
          q->enqueue(p);
        }
      }
    }

    // Wait for all copies to be made.
    bool has_packets = false;
    do {
      has_packets = false;
      std::this_thread::yield();
        for (int i=0; i < RmtDefs::kPresTotal; ++i)
          has_packets = has_packets || !pre[i]->fifos_empty();
    } while(has_packets);

    // All packets should be in pipe 1.
    ASSERT_FALSE(q->pipeHasPackets(0));
    ASSERT_FALSE(q->pipeHasPackets(2));
    ASSERT_FALSE(q->pipeHasPackets(3));
    ASSERT_TRUE(q->pipeHasPackets(1));

    // Collect the output packets and record which packets came back by using
    // an array of bools indexed by the expected output port and the original
    // packet's RID/XID/YID.
    std::array<int, 8> out_ports = {{0x80|1, 0x80|(64-adjust_port_num), 0x80|4, 0x80|(68-adjust_port_num), 0x80|0, 0x80|2, 0x80|5, 0x80|(65-adjust_port_num)}};
    std::array<std::array<bool, input_rid.size()*input_xid.size()*input_yid.size()>, out_ports.size()> out_pkts = {};
    while (q->pipeHasPackets(1)) {
      Packet *pp = nullptr;
      q->dequeue(pp);
      unsigned idx_r=0, idx_x=0, idx_y=0;
      for (; idx_r<input_rid.size() && pp->i2qing_metadata()->irid() != input_rid[idx_r]; ++idx_r);
      for (; idx_x<input_xid.size() && pp->i2qing_metadata()->xid() != input_xid[idx_x]; ++idx_x);
      for (; idx_y<input_yid.size() && pp->i2qing_metadata()->yid() != input_yid[idx_y]; ++idx_y);
      assert(idx_r < input_rid.size());
      assert(idx_x < input_xid.size());
      assert(idx_y < input_yid.size());
      unsigned idx = 0;
      idx += idx_r*(input_xid.size() * input_yid.size());
      idx += idx_x*(input_yid.size());
      idx += idx_y;

      unsigned port_idx = 0;
      for (; port_idx<out_ports.size() && pp->port()->port_index() != out_ports[port_idx]; ++port_idx);
      assert(port_idx < out_ports.size());

      ASSERT_FALSE(out_pkts[port_idx][idx]);
      out_pkts[port_idx][idx] = true;
      om->pkt_delete(pp);
    }

    // For each expected output port, ensure that the expected combinations of
    // RID/XID/YID came back and that the pruning happened (copy didn't come
    // back) for other combinations of RID/XID/YID.
    // Note the comments below for RID/XID/YID indicate which combinations are
    // expected to be pruned.
    int rid_step = input_xid.size()*input_yid.size();
    int xid_step = input_yid.size();
    // RID 0 YID 0/1, RID 0x12 YID 0/1
    // Pipe 1 Port 1:
    for (unsigned i=0; i<input_xid.size(); ++i) {
      for (unsigned j=0; j<input_yid.size(); ++j) {
        ASSERT_FALSE(out_pkts[0][0*rid_step + i*xid_step + j]);
        ASSERT_FALSE(out_pkts[0][1*rid_step + i*xid_step + j]);
        out_pkts[0][0*rid_step + i*xid_step + j] = true;
        out_pkts[0][1*rid_step + i*xid_step + j] = true;
      }
    }
    for (auto x : out_pkts[0]) {
      ASSERT_TRUE(x);
    }
    // RID 0 YID 0/1, RID 0x12 YID 0/1
    // Pipe 1 Port 64
    for (unsigned i=0; i<input_xid.size(); ++i) {
      for (unsigned j=0; j<input_yid.size(); ++j) {
        ASSERT_FALSE(out_pkts[1][0*rid_step + i*xid_step + j]);
        ASSERT_FALSE(out_pkts[1][1*rid_step + i*xid_step + j]);
        out_pkts[1][0*rid_step + i*xid_step + j] = true;
        out_pkts[1][1*rid_step + i*xid_step + j] = true;
      }
    }
    for (auto x : out_pkts[1]) {
      ASSERT_TRUE(x);
    }

    // RID 0 YID 0, RID 0x23 YID 0, XID 0x1111
    // Pipe 1 Port 4:
    for (unsigned i=0; i<input_rid.size(); ++i) {
      for (unsigned j=0; j<input_yid.size(); ++j) {
        ASSERT_FALSE(out_pkts[2][i*rid_step + 1*xid_step + j]);
        out_pkts[2][i*rid_step + 1*xid_step + j] = true;
      }
    }
    for (unsigned i=0; i<input_xid.size(); ++i) {
      if (i == 1) continue;
      ASSERT_FALSE(out_pkts[2][0*rid_step + i*xid_step + 0]);
      ASSERT_FALSE(out_pkts[2][2*rid_step + i*xid_step + 0]);
      out_pkts[2][0*rid_step + i*xid_step + 0] = true;
      out_pkts[2][2*rid_step + i*xid_step + 0] = true;
    }
    for (auto x : out_pkts[2]) {
      ASSERT_TRUE(x);
    }
    // RID 0 YID 1, RID 0x23 YID 1, XID 0x1111
    // Pipe 1 Port 68:
    for (unsigned i=0; i<input_rid.size(); ++i) {
      for (unsigned j=0; j<input_yid.size(); ++j) {
        ASSERT_FALSE(out_pkts[3][i*rid_step + 1*xid_step + j]);
        out_pkts[3][i*rid_step + 1*xid_step + j] = true;
      }
    }
    for (unsigned i=0; i<input_xid.size(); ++i) {
      if (i == 1) continue;
      ASSERT_FALSE(out_pkts[3][0*rid_step + i*xid_step + 1]);
      ASSERT_FALSE(out_pkts[3][2*rid_step + i*xid_step + 1]);
      out_pkts[3][0*rid_step + i*xid_step + 1] = true;
      out_pkts[3][2*rid_step + i*xid_step + 1] = true;
    }
    for (auto x : out_pkts[3]) {
      ASSERT_TRUE(x);
    }

    // RID 0 YID 0, RID 0x34 YID 0
    // Pipe 1 Port 0
    for (unsigned i=0; i<input_xid.size(); ++i) {
      ASSERT_FALSE(out_pkts[4][0*rid_step + i*xid_step + 0]);
      ASSERT_FALSE(out_pkts[4][3*rid_step + i*xid_step + 0]);
      out_pkts[4][0*rid_step + i*xid_step + 0] = true;
      out_pkts[4][3*rid_step + i*xid_step + 0] = true;
    }
    for (auto x : out_pkts[4]) {
      ASSERT_TRUE(x);
    }

    // XID 0x2222
    // Pipe 1 Port 2
    for (unsigned i=0; i<input_rid.size(); ++i) {
      for (unsigned j=0; j<input_yid.size(); ++j) {
        ASSERT_FALSE(out_pkts[5][i*rid_step + 2*xid_step + j]);
        out_pkts[5][i*rid_step + 2*xid_step + j] = true;
      }
    }
    for (auto x : out_pkts[5]) {
      ASSERT_TRUE(x);
    }

    // RID 0 YID 0, RID 56 YID 0
    // Pipe 1 port 5
    for (unsigned i=0; i<input_xid.size(); ++i) {
      ASSERT_FALSE(out_pkts[6][0*rid_step + i*xid_step + 0]);
      ASSERT_FALSE(out_pkts[6][5*rid_step + i*xid_step + 0]);
      out_pkts[6][0*rid_step + i*xid_step + 0] = true;
      out_pkts[6][5*rid_step + i*xid_step + 0] = true;
    }
    for (auto x : out_pkts[6]) {
      ASSERT_TRUE(x);
    }

    // RID 0 YID 1, RID 56 YID 1
    // Pipe 1 Port 65
    for (unsigned i=0; i<input_xid.size(); ++i) {
      ASSERT_FALSE(out_pkts[7][0*rid_step + i*xid_step + 1]);
      ASSERT_FALSE(out_pkts[7][5*rid_step + i*xid_step + 1]);
      out_pkts[7][0*rid_step + i*xid_step + 1] = true;
      out_pkts[7][5*rid_step + i*xid_step + 1] = true;
    }
    for (auto x : out_pkts[7]) {
      ASSERT_TRUE(x);
    }

    // Cleanup
    unsigned end_packets = om->n_packets();
    ASSERT_EQ(end_packets, start_packets);
  }


  TEST(BFN_TEST_NAME(DISABLED_QueueingTest), Multicast_L2PortNode) {
    RmtObjectManager *om;
    GLOBAL_MODEL->DestroyChip(0);
    GLOBAL_MODEL->CreateChip(0, RmtDefs::kChipType);
    GLOBAL_MODEL->InitChip(0);
    GLOBAL_MODEL->GetObjectManager(0, &om);
    assert(om != NULL);
    unsigned start_packets = om->n_packets();
    const char *pkt_string = "123456789abcdeffedcba987654321";
    Queueing *q = om->queueing_get();
    std::array<PacketReplicationEngine*, RmtDefs::kPresTotal> pre = {};
    Rdm *rdm = om->rdm_get();
    om->packet_coordinator_get()->stop();
    for (int i=0; i < RmtDefs::kPresTotal; ++i) {
      pre[i] = om->pre_get(i);
      pre[i]->reset();
    }

    for (int i=0; i < RmtDefs::kPresTotal; ++i) {
      // FIX-ME : Constrain to 4 pipes, model does not have multi die support
      if (i >= RmtDefs::kPipesMax) continue;
      for (auto &x : pre) {
        x->reset();
      }
      int base = (i+1) * 0x10000;

      int rdm_blk = base >> 12;
      int rdm_word_off = RegisterUtils::rdm_blk_word_off(rdm_blk);
      int rdm_bit_off = RegisterUtils::rdm_blk_bit_off(rdm_blk);
      GLOBAL_MODEL->OutWord(0,
                            RegisterUtils::offset_tm_pre_common_blk() + 4 * rdm_word_off,
                            i << rdm_bit_off);

      pre[i]->mit(i, base);
      Rdm::RdmLine l = {};
      for (int p=0; p<PacketReplicationEngine::kLocalPortWidth; ++p) {
        l.type[0] = RdmDefs::RdmNodeType::kL1Rid;
        l.u.L1Rid.next_L1 = base+2;
        l.u.L1Rid.next_L2 = base+0x100;
        l.u.L1Rid.rid     = p;
        rdm->write_line(base >> 1, l);

        l.type[0] = RdmDefs::RdmNodeType::kL2Port64;
	l.u.L2Port64.chip  = 0;
        l.u.L2Port64.pipe  = i;
        l.u.L2Port64.last  = true;
        l.u.L2Port64.spv   = p < 64 ? 0 : 1 << (p-64);
        l.u.L2Port64.ports = p < 64 ? 1ul << p : 0;
        rdm->write_line((base+0x100) >> 1, l);

        base += 2;
      }
      l.type[0] = RdmDefs::RdmNodeType::kL1RidEnd;
      l.u.L1RidEnd[0].next_L2 = base+0x100;
      l.u.L1RidEnd[0].rid     = PacketReplicationEngine::kLocalPortWidth;
      rdm->write_line(base >> 1, l);
      l.type[0] = RdmDefs::RdmNodeType::kL2Port64;
      l.u.L2Port64.chip  = 0;
      l.u.L2Port64.pipe  = i;
      l.u.L2Port64.last  = true;
      l.u.L2Port64.spv   = ~0 & MemUtils::pre_rdm_l2port64_upper_mask();
      l.u.L2Port64.ports = ~0ul & MemUtils::pre_rdm_l2port64_lower_mask();
      rdm->write_line((base+0x100) >> 1, l);

      Packet *p = om->pkt_create(pkt_string);
      p->i2qing_metadata()->set_pipe_mask(0xF);
      p->i2qing_metadata()->set_mgid1(i);
      q->enqueue(p);

      // Wait for all copies to be made.
      bool has_packets = false;
      do {
        has_packets = false;
        std::this_thread::yield();
          for (int i=0; i < RmtDefs::kPresTotal; ++i)
            has_packets = has_packets || !pre[i]->fifos_empty();
      } while(has_packets);

      std::array<int, PacketReplicationEngine::kLocalPortWidth> rx_cnt = {};
      for (int x=0; x<PacketReplicationEngine::kLocalPortWidth*2; ++x) {
        Packet *p = nullptr;
        q->dequeue(p);
        if (nullptr == p) break;
        int port = p->port()->port_index() & 0x7F;
        int pipe = p->port()->pipe_index();
        ASSERT_TRUE(pipe == i);
        rx_cnt[port] += 1;
        ASSERT_TRUE(p->qing2e_metadata()->erid() == port || p->qing2e_metadata()->erid() == PacketReplicationEngine::kLocalPortWidth);
        om->pkt_delete(p);
      }
      ASSERT_FALSE(q->hasPackets());
      for (auto &x : rx_cnt) {
        ASSERT_TRUE(x == 2);
      }
    }

    // Cleanup
    unsigned end_packets = om->n_packets();
    ASSERT_EQ(end_packets, start_packets);
  }


  TEST(BFN_TEST_NAME(QueueingTest), Multicast_L1Ecmp) {
    RmtObjectManager *om;
    GLOBAL_MODEL->DestroyChip(0);
    GLOBAL_MODEL->CreateChip(0, RmtDefs::kChipType);
    GLOBAL_MODEL->InitChip(0);
    GLOBAL_MODEL->GetObjectManager(0, &om);
    assert(om != NULL);
    unsigned start_packets = om->n_packets();
    const char *pkt_string = "123456789abcdeffedcba987654321";
    Queueing *q = om->queueing_get();
    std::array<PacketReplicationEngine*, RmtDefs::kPresTotal> pre = {};
    Rdm *rdm = om->rdm_get();
    om->packet_coordinator_get()->stop();
    for (int i=0; i < RmtDefs::kPresTotal; ++i) {
      pre[i] = om->pre_get(i);
      pre[i]->reset();
    }


    Rdm::RdmLine l = {};
    int base = (0+1) * 0x1000;
    pre[0]->mit(0, base);
    l.type[0] = RdmDefs::RdmNodeType::kL1EcmpPtr;
    l.u.L1EcmpPtr.next_L1 = 0;
    l.u.L1EcmpPtr.vector0 = base+2;
    l.u.L1EcmpPtr.vector1 = base+4;
    rdm->write_line(base >> 1, l);

    l.type[0] = RdmDefs::RdmNodeType::kL1EcmpVector;
    l.u.L1EcmpVector.base_L1 = base+0x100;
    l.u.L1EcmpVector.length  = 31;
    l.u.L1EcmpVector.vector  = 0xFFFFFFFF;
    rdm->write_line((base+2) >> 1, l);
    l.type[0] = RdmDefs::RdmNodeType::kL1EcmpVector;
    l.u.L1EcmpVector.base_L1 = base+0x200;
    l.u.L1EcmpVector.length  = 27;
    l.u.L1EcmpVector.vector  = 0x913456F1;
    rdm->write_line((base+4) >> 1, l);

    for (int k=0; k<16; ++k) {
      l.type[0] = RdmDefs::RdmNodeType::kL1RidEnd;
      l.type[1] = RdmDefs::RdmNodeType::kL1RidEnd;
      l.u.L1RidEnd[0].next_L2 = base+6;
      l.u.L1RidEnd[1].next_L2 = base+6;
      l.u.L1RidEnd[0].rid = 2*k;
      l.u.L1RidEnd[1].rid = 2*k+1;
      rdm->write_line((base+0x100+2*k) >> 1, l);
      l.type[0] = RdmDefs::RdmNodeType::kL1RidEnd;
      l.type[1] = RdmDefs::RdmNodeType::kL1RidEnd;
      l.u.L1RidEnd[0].next_L2 = base+6;
      l.u.L1RidEnd[1].next_L2 = base+6;
      l.u.L1RidEnd[0].rid = 0x8000 | (2*k);
      l.u.L1RidEnd[1].rid = 0x8000 | (2*k+1);
      rdm->write_line((base+0x200+2*k) >> 1, l);
    }

    l.type[0] = RdmDefs::RdmNodeType::kL2Port64;
    l.u.L2Port64.chip  = 0;
    l.u.L2Port64.pipe  = 0;
    l.u.L2Port64.last  = true;
    l.u.L2Port64.spv   = 0;
    l.u.L2Port64.ports = 1;
    rdm->write_line((base+6) >> 1, l);


    for (int hash=0; hash<64; ++hash) {
      Packet *p = nullptr;
      p = om->pkt_create(pkt_string);
      p->i2qing_metadata()->set_pipe_mask(1<<0);
      p->i2qing_metadata()->set_mgid1(0);
      p->set_ph_ver(0);
      p->i2qing_metadata()->set_hash1(hash);
      q->enqueue(p);

      // Wait for all copies to be made.
      bool has_packets = false;
      do {
        has_packets = false;
        std::this_thread::yield();
          for (int i=0; i < RmtDefs::kPresTotal; ++i)
            has_packets = has_packets || !pre[i]->fifos_empty();
      } while(has_packets);

      q->dequeue(p);
      ASSERT_TRUE(p);
      ASSERT_TRUE(0 == p->port()->port_index());
      ASSERT_TRUE(0 == p->port()->pipe_index());
      ASSERT_TRUE((hash%32) == p->qing2e_metadata()->erid());
      om->pkt_delete(p);
    }

    for (int hash=0; hash<64; ++hash) {
      Packet *p = nullptr;
      p = om->pkt_create(pkt_string);
      p->i2qing_metadata()->set_pipe_mask(1<<0);
      p->i2qing_metadata()->set_mgid1(0);
      p->set_ph_ver(1);
      p->i2qing_metadata()->set_hash1(hash);
      q->enqueue(p);

      // Wait for all copies to be made.
      bool has_packets = false;
      do {
        has_packets = false;
        std::this_thread::yield();
          for (int i=0; i < RmtDefs::kPresTotal; ++i)
            has_packets = has_packets || !pre[i]->fifos_empty();
      } while(has_packets);

      q->dequeue(p);
      ASSERT_TRUE(p);
      ASSERT_TRUE(0 == p->port()->port_index());
      ASSERT_TRUE(0 == p->port()->pipe_index());
      // Calculate the selected LAG member.
      int idx1 = hash % 28;
      int idx2 = hash % (__builtin_popcount(0x013456F1));
      int rid = -1;
      if ((0x013456F1 >> idx1) & 1) {
        // First attempt successful.
        rid = idx1;
      } else {
        for (int m=0; m<32; ++m) {
          if ((0x013456F1 >> m) & 1) {
            if (!idx2) {
              // Member is m
              rid = m;
              break;
            }
            --idx2;
          }
        }
      }
      ASSERT_TRUE((0x8000|rid) == p->qing2e_metadata()->erid());
      om->pkt_delete(p);
    }

    // Cleanup
    unsigned end_packets = om->n_packets();
    ASSERT_EQ(end_packets, start_packets);
  }

#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)

  TEST(BFN_TEST_NAME(QueueingTest), Basic_Multicast_Cross_Die) {
    RmtObjectManager *om[2];
    int read_chip_id;
    // Set up the 2 chips
    for (int chip = 0; chip < 2; chip++) {
      GLOBAL_MODEL->DestroyChip(chip);
      GLOBAL_MODEL->CreateChip(chip, RmtDefs::kChipType);
    }

    // Set up the 2 chip package
    GLOBAL_MODEL->SetPackage(0,1,-1,-1);

    // Init the chip
    for (int chip = 0; chip < 2; chip++) {
      GLOBAL_MODEL->InitChip(chip);
      GLOBAL_MODEL->GetObjectManager(chip, &om[chip]);
      assert(om[chip] != NULL);
      om[chip]->packet_coordinator_get()->stop();
    }

    // Send 1 packet cross chip from both chips
    for (int chip = 0; chip < 2; chip++) {
      read_chip_id = om[chip]->chip()->GetReadDieId();
      printf("Start test for chip:%0d read chip:%0d\n", chip, read_chip_id);
      unsigned start_packets = om[chip]->n_packets();
      const char *pkt_string = "123456789abcdeffedcba987654321";
      Packet *p = om[chip]->pkt_create(pkt_string);
      Packet *pp = NULL;
      Queueing *q_in = om[chip]->queueing_get();
      Queueing *q_out = om[read_chip_id]->queueing_get();
      PacketReplicationEngine *pre[RmtDefs::kPresTotal] = {};
      Rdm *rdm = om[chip]->rdm_get();
      for (int i=0; i < RmtDefs::kPresTotal; ++i) {
	pre[i] = om[chip]->pre_get(i);
	pre[i]->reset();
      }

      for (int i=0; i < RmtDefs::kPresTotal; ++i) {
	int base = (i+1)*0x10000;
	pre[i]->mit(1, base);

	int rdm_blk = base >> 12;
	int rdm_word_off = RegisterUtils::rdm_blk_word_off(rdm_blk);
	int rdm_bit_off = RegisterUtils::rdm_blk_bit_off(rdm_blk);
	GLOBAL_MODEL->OutWord(chip,
			      RegisterUtils::offset_tm_pre_common_blk() + 4 * rdm_word_off,
			      i << rdm_bit_off);

	Rdm::RdmLine l = {};
	// Multicast tree:
	// L1 0x10000 --> 0x10010 --> X
	// L2 0x10020     0x10030
	l.type[0] = RdmDefs::RdmNodeType::kL1Rid;
	l.u.L1Rid.next_L1 = base + 0x10;
	l.u.L1Rid.next_L2 = base + 0x20;
	l.u.L1Rid.rid     = 0x1234;
	rdm->write_line(base >> 1, l);
	l.type[0] = RdmDefs::RdmNodeType::kL1Rid;
	l.u.L1Rid.next_L1 = 0x000;
	l.u.L1Rid.next_L2 = base + 0x30;
	l.u.L1Rid.rid     = 0x5678;
	rdm->write_line((base+0x10) >> 1, l);
	l.type[0] = RdmDefs::RdmNodeType::kL2Port16;
	l.u.L2Port16[0].spv   = 0;
	l.u.L2Port16[0].ports = 0x0010;
	l.u.L2Port16[0].pipe  = i & RmtDefs::kPipeMask;
	l.u.L2Port16[0].chip  = (i >= RmtDefs::kPipesMax) ? read_chip_id : chip;
	l.u.L2Port16[0].last  = true;
	rdm->write_line((base+0x20) >> 1, l);
	l.type[0] = RdmDefs::RdmNodeType::kL2Port64;
	l.u.L2Port64.spv   = 0;
	l.u.L2Port64.ports = 0x0002;
	l.u.L2Port64.pipe  = i & RmtDefs::kPipeMask;
	l.u.L2Port64.chip  = (i >= RmtDefs::kPipesMax) ? read_chip_id : chip;
	l.u.L2Port64.last  = true;
	rdm->write_line((base+0x30) >> 1, l);
      }

      // Inject one packet for replication on TM Chips 0 & 1 pipes 4,5, and 7.
      // Inject a unicast on TM Chip 0 pipe 6 port 50 and TM Chip 1 pipe 2 port 50
      // Packets should come out on TM Chip 0 & 1 Pipes 0 - 3
      p->i2qing_metadata()->set_egress_unicast_port(((2 + (read_chip_id * RmtDefs::kPipesMax)) << 7) | 50);
      p->i2qing_metadata()->set_mgid1(1);
      p->i2qing_metadata()->set_pipe_mask(0xB0);
      q_in->enqueue(p);

      // Wait for all copies to be made.
      bool has_packets = false;
      do {
	has_packets = false;
	std::this_thread::yield();
        for (int i=0; i < RmtDefs::kPresTotal; ++i)
          has_packets = has_packets || !pre[i]->fifos_empty();
      } while(has_packets);

      ASSERT_TRUE(q_out->hasPackets());
      for (int i=0; i < RmtDefs::kPipesMax; ++i) {
	ASSERT_TRUE(q_out->pipeHasPackets(i));
      }

      // Verify the unicast copy is the only copy in TM Chip 1 pipe 2.
      q_out->dequeue(2, pp);
      ASSERT_TRUE(pp != p); // In UC+MC cases, MC gets the original copy.
      ASSERT_TRUE(pp->port()->pipe_index() == 2);
      ASSERT_TRUE(pp->port()->port_index() == ((2 << 7) | 50));
      om[chip]->pkt_delete(pp);
      q_out->dequeue(2, pp);
      ASSERT_TRUE(pp == nullptr);
      ASSERT_FALSE(q_out->pipeHasPackets(2));

      // Verify the multicast copies in TM Chip 1 pipes 0, 1, and 3.
      std::array<bool, 6> pkt_seen = {};
      for (int i=0; i<6; ++i) {
	q_out->dequeue(pp);
	int pipe = pp->port()->pipe_index();
	int port = pp->port()->port_index() & 0x7F;
	ASSERT_TRUE(0 == pipe || 1 == pipe || 3 == pipe);
	ASSERT_TRUE(1 == port || 16 == port);
	int idx = 2 * (pipe == 3 ? 2 : pipe) + (port == 1 ? 0 : 1);
	ASSERT_FALSE(pkt_seen[idx]);
	pkt_seen[idx] = true;
	om[chip]->pkt_delete(pp);
      }
      for (int i=0; i<6; ++i) {
	ASSERT_TRUE(pkt_seen[i]);
      }
      q_out->dequeue(pp);
      ASSERT_TRUE(nullptr == pp);

      // Cleanup
      unsigned end_packets = om[chip]->n_packets();
      ASSERT_EQ(end_packets, start_packets);
      printf("Packet sent successfully from chip:%0d to chip:%0d\n",chip, read_chip_id);
    }

    // Teardown the 2 chips - must unpackage first
    GLOBAL_MODEL->UnPackage(0,1,-1,-1);
    for (int chip = 0; chip < 2; chip++) GLOBAL_MODEL->DestroyChip(chip);
  }
#endif

#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)

  TEST(BFN_TEST_NAME(QueueingTest), PreMirror) {
    RmtObjectManager *om;
    GLOBAL_MODEL->DestroyChip(0);
    GLOBAL_MODEL->CreateChip(0, RmtDefs::kChipType);
    GLOBAL_MODEL->InitChip(0);
    GLOBAL_MODEL->GetObjectManager(0, &om);
    assert(om != NULL);
    unsigned start_packets = om->n_packets();
    const char *pkt_string = "123456789abcdeffedcba987654321";
    Packet *p = om->pkt_create(pkt_string);
    Packet *pp = NULL;
    Queueing *q = om->queueing_get();

    PacketReplicationEngine *pre = {};
    Rdm *rdm = om->rdm_get();
    om->packet_coordinator_get()->stop();
    pre = om->pre_get(0);
    pre->reset();

    // N.B. It's possible that after a register update the below start address has changed. To find the new address
    //      go to model_mem.h and follow TmPreMbtMemWordEntryMutable::StartOffset(0, 0).
    uint64_t constexpr mbt_start_addr = 0x4900004000;
    int base = 0x10000;  // The L1 root node address
    int mbt_entry_0 = (base << 11) | 0x12;  // Bottom 7 bits = eQID.
    GLOBAL_MODEL->IndirectWrite(0, mbt_start_addr + 0x0, mbt_entry_0, 0);
    int mbt_entry_7 = (base << 11) | (0x1 << 7) | 0x12;  // Set pipe=0x1 (N.B. we're using PRE at pipe 0)
    GLOBAL_MODEL->IndirectWrite(0, mbt_start_addr + 0x7, mbt_entry_7, 0);
    int mbt_entry_15 = ((base + 0x10) << 11) | 0x34;
    GLOBAL_MODEL->IndirectWrite(0, mbt_start_addr + 0xf, mbt_entry_15, 0);

    int rdm_blk = base >> 12;
    int rdm_word_off = RegisterUtils::rdm_blk_word_off(rdm_blk);
    GLOBAL_MODEL->OutWord(0, RegisterUtils::offset_tm_pre_common_blk() + 4 * rdm_word_off, 0);


    Rdm::RdmLine l = {};
    // Multicast tree:
    //    [mbt_0]     [mbt_15]
    // L1 0x10000 --> 0x10010 --> X
    // L2 0x10020     0x10030
    l.type[0] = RdmDefs::RdmNodeType::kL1Rid;
    l.u.L1Rid.next_L1 = base + 0x10;
    l.u.L1Rid.next_L2 = base + 0x20;
    l.u.L1Rid.rid     = 0x1234;
    rdm->write_line(base >> 1, l);
    l.type[0] = RdmDefs::RdmNodeType::kL1Rid;
    l.u.L1Rid.next_L1 = 0x000;
    l.u.L1Rid.next_L2 = base + 0x30;
    l.u.L1Rid.rid     = 0x5678;
    rdm->write_line((base+0x10) >> 1, l);
    l.type[0] = RdmDefs::RdmNodeType::kL2Port16;
    l.u.L2Port16[0].spv   = 0;
    l.u.L2Port16[0].ports = 0x0010;
    l.u.L2Port16[0].pipe  = 0;
    l.u.L2Port16[0].chip  = 0;
    l.u.L2Port16[0].last  = true;
    rdm->write_line((base+0x20) >> 1, l);
    l.type[0] = RdmDefs::RdmNodeType::kL2Port64;
    l.u.L2Port64.spv   = 0;
    l.u.L2Port64.ports = 0x0002;
    l.u.L2Port64.pipe  = 0;
    l.u.L2Port64.chip  = 0;
    l.u.L2Port64.last  = true;
    rdm->write_line((base+0x30) >> 1, l);

    // Inject one packet for mirror replication (three mirror sessions, session 7 is invalid.)
    p->i2qing_metadata()->set_mirror_bmp(0x8081);
    p->i2qing_metadata()->set_pipe_mask(0xB);
    q->enqueue(p);

    // Wait for copies to be made.
    do {
      std::this_thread::yield();
    } while (!pre->fifos_empty());

    // A PRE can only handle mirrors for it's own pipe. Check that only our pipe has packets.
    ASSERT_TRUE(q->hasPackets());
    ASSERT_TRUE(q->pipeHasPackets(0));
    for (int i = 1; i < RmtDefs::kTmPipesMax; ++i)
      ASSERT_FALSE(q->pipeHasPackets(i));

    // Verify there are 3 mirror replications (on the same pipe) from two valid mirror sessions.
    for (int i = 0; i < 3; ++i) {
      q->dequeue(0, pp);
      int pipe = pp->port()->pipe_index();
      int port = pp->port()->port_index() & 0x7F;
      ASSERT_EQ(pipe, 0);
      ASSERT_TRUE(1 == port || 16 == port);
      ASSERT_TRUE(pp->qing2e_metadata()->mir_copy());
      int erid = pp->qing2e_metadata()->erid();
      ASSERT_TRUE(erid == 0x1234 || erid == 0x5678);
      int eqid = pp->qing2e_metadata()->eqid();
      ASSERT_TRUE(eqid == 0x12 || eqid == 0x34);
      om->pkt_delete(pp);
    }
    ASSERT_FALSE(q->hasPackets());
    q->dequeue(pp);
    ASSERT_TRUE(nullptr == pp);

    // Cleanup
    unsigned end_packets = om->n_packets();
    ASSERT_EQ(end_packets, start_packets);
  }

#endif

}
