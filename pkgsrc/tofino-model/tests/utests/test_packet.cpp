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

#include "gtest.h"

#include <model_core/model.h>
#include <bitvector.h>
#include <tcam.h>
#include <rmt-object-manager.h>
#include <packet.h>
#include <phv.h>
#include <parser-static-config.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool pk_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


  TEST(BFN_TEST_NAME(PacketTest),Basic) {
    GLOBAL_MODEL->Reset();
    if (pk_print) RMT_UT_LOG_INFO("test_packet_basic()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    uint8_t buf[32];
    uint8_t buf1[256] = {'h','e','l','l','o','\0'};
    uint8_t buf2[256] = {'g','o','o','d','b','y','e','\0'};
    buf2[253] = 'a'; buf2[254] = 'n'; buf2[255] = 'd';
    uint8_t buf3[256] = {'t','h','a','n','k','y','o','u','\0'};
    // Create 3 PacketBuffers on heap
    PacketBuffer *pb1 = om->pktbuf_create(buf1, 256);
    PacketBuffer *pb2 = om->pktbuf_create(buf2, 256);
    PacketBuffer *pb3 = om->pktbuf_create(buf3, 256);
    if (pk_print) pb1->print("pb1");
    if (pk_print) pb2->print("pb2");
    if (pk_print) pb3->print("pb3");

    // Now a Phv and a Packet on heap
    Phv *phv = om->phv_create();
    Packet *p1 = om->pkt_create();

    // Add Phv and PacketBuffers to packet
    p1->set_phv(phv);
    p1->append(pb1);
    p1->append(pb2);
    p1->append(pb3);
    ASSERT_EQ(768, p1->len());

    // Now clone p1 to p2 - should clone the PacketBuffers
    // and create a new shared_ptr to Phv
    Packet *p2 = p1->clone();
    ASSERT_EQ(768, p2->len());

    // Delete p1
    // Should see 3 PacketBuffer destroys and 1 Packet destroy
    if (pk_print) RMT_UT_LOG_INFO("Delete p1\n");
    om->pkt_delete(p1);

    // Get a buffer out of p2
    p2->get_buf(buf, 509, 12);
    if (pk_print) RMT_UT_LOG_INFO("get_buf<p2,509,12>=%s\n", buf);
    // Delete p2
    // Should see 3 PacketBuffer destroys, a Phv destroy
    // and a Packet destroy
    if (pk_print) RMT_UT_LOG_INFO("Delete p2\n");
    om->pkt_delete(p2);
  }

  TEST(BFN_TEST_NAME(PacketTest),Clone) {
    GLOBAL_MODEL->Reset();
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    Phv *phv = om->phv_create();
    Packet *p1 = om->pkt_create();
    p1->pkt_id(UINT64_C(1234));
    ASSERT_EQ(nullptr, p1->phv());
    ASSERT_EQ(0, p1->i2qing_metadata()->version());
    ASSERT_EQ(1234u, p1->pkt_id());
    ASSERT_FALSE(p1->is_generated());
    ASSERT_EQ(UINT64_C(0), p1->generated_T());
    ASSERT_EQ(0, p1->metadata_len());
    ASSERT_FALSE(p1->is_resubmit());
    ASSERT_FALSE(p1->truncated());
    ASSERT_EQ(0, p1->orig_ingress_pkt_len());
    ASSERT_EQ(0, p1->priority());

    p1->set_phv(phv);
    p1->i2qing_metadata()->set_version(2u);
    p1->set_generated(true);
    p1->set_generated_T(1234567890987654321);
    p1->set_metadata_len(11);
    p1->set_resubmit(true);
    p1->set_truncated(true);
    p1->set_orig_ingress_pkt_len(987);
    p1->set_priority(3);
    ASSERT_EQ(phv, p1->phv());
    ASSERT_EQ(2u, p1->i2qing_metadata()->version());
    ASSERT_EQ(1234u, p1->pkt_id());
    ASSERT_TRUE(p1->is_generated());
    ASSERT_EQ(UINT64_C(1234567890987654321), p1->generated_T());
    ASSERT_EQ(11, p1->metadata_len());
    ASSERT_TRUE(p1->is_resubmit());
    ASSERT_TRUE(p1->truncated());
    ASSERT_EQ(987, p1->orig_ingress_pkt_len());
    ASSERT_EQ(3, p1->priority());

    // make a clone
    Packet *p2 = p1->clone();
    // sanity check p1
    ASSERT_EQ(phv, p1->phv());
    ASSERT_EQ(2u, p1->i2qing_metadata()->version());
    ASSERT_EQ(1234u, p1->pkt_id());
    ASSERT_TRUE(p1->is_generated());
    ASSERT_EQ(UINT64_C(1234567890987654321), p1->generated_T());
    ASSERT_EQ(11, p1->metadata_len());
    ASSERT_TRUE(p1->is_resubmit());
    ASSERT_TRUE(p1->truncated());
    ASSERT_EQ(987, p1->orig_ingress_pkt_len());
    ASSERT_EQ(3, p1->priority());
    // phv ptr is copied to clone
    ASSERT_EQ(phv, p2->phv());
    // i2q metadata is copied by default
    ASSERT_EQ(2u, p2->i2qing_metadata()->version());
    // pkt id is copied
    ASSERT_EQ(1234u, p2->pkt_id());
    // as is orig_ingress_pkt_len and priority
    ASSERT_EQ(987, p2->orig_ingress_pkt_len());
    ASSERT_EQ(3, p2->priority());
    // other metadata is copied
    ASSERT_TRUE(p2->is_generated());
    ASSERT_EQ(UINT64_C(1234567890987654321), p2->generated_T());
    ASSERT_EQ(11, p2->metadata_len());
    ASSERT_TRUE(p2->is_resubmit());
    ASSERT_TRUE(p2->truncated());

    // make a clone, do not copy metadata
    p2 = p1->clone(false);
    // sanity check p1
    ASSERT_EQ(phv, p1->phv());
    ASSERT_EQ(2u, p1->i2qing_metadata()->version());
    ASSERT_EQ(1234u, p1->pkt_id());
    ASSERT_TRUE(p1->is_generated());
    ASSERT_EQ(UINT64_C(1234567890987654321), p1->generated_T());
    ASSERT_EQ(11, p1->metadata_len());
    ASSERT_TRUE(p1->is_resubmit());
    ASSERT_TRUE(p1->truncated());
    ASSERT_EQ(987, p1->orig_ingress_pkt_len());
    ASSERT_EQ(3, p1->priority());
    // phv is not copied to clone
    ASSERT_EQ(nullptr, p2->phv());
    // metadata is not copied
    ASSERT_EQ(0u, p2->i2qing_metadata()->version());
    // pkt id is copied
    ASSERT_EQ(1234u, p2->pkt_id());
    // as is orig_ingress_pkt_len and priority
    ASSERT_EQ(987, p2->orig_ingress_pkt_len());
    ASSERT_EQ(3, p2->priority());
    // other metadata *not* copied
    ASSERT_FALSE(p2->is_generated());
    ASSERT_EQ(UINT64_C(0), p2->generated_T());
    ASSERT_EQ(0, p2->metadata_len());
    ASSERT_FALSE(p2->is_resubmit());
    ASSERT_FALSE(p2->truncated());

    om->pkt_delete(p1);
    om->pkt_delete(p2);
  }

  TEST(BFN_TEST_NAME(PacketTest),Buffers) {
    GLOBAL_MODEL->Reset();
    if (pk_print) RMT_UT_LOG_INFO("test_packet_buffers()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    uint8_t buf[32];
    uint8_t buf1[256] = {'h','e','l','l','o','\0'};
    uint8_t buf2[256] = {'g','o','o','d','b','y','e','\0'};
    buf2[253] = 'a'; buf2[254] = 'n'; buf2[255] = 'd';
    uint8_t buf3[256] = {'t','h','a','n','k','y','o','u','\0'};
    // Create 3 PacketBuffers on heap
    PacketBuffer *pb1 = om->pktbuf_create(buf1, 256);
    PacketBuffer *pb2 = om->pktbuf_create(buf2, 256);
    PacketBuffer *pb3 = om->pktbuf_create(buf3, 256);

    // Now a Phv and a Packet on heap
    Phv *phv = om->phv_create();
    Packet *p1 = om->pkt_create();
    int res = 0;

    // Add Phv and PacketBuffers to packet
    p1->set_phv(phv);
    p1->append(pb1);
    p1->append(pb2);
    p1->append(pb3);
    ASSERT_EQ(768, p1->len());
    p1->get_buf(buf, 0, 6);
    res = std::memcmp(buf1,buf,6);
    ASSERT_EQ(0, res);
    p1->get_buf(buf, 256, 8);
    res = std::memcmp(buf2,buf,8);
    ASSERT_EQ(0, res);
    p1->get_buf(buf, 512, 9);
    res = std::memcmp(buf3,buf,0);
    ASSERT_EQ(0, res);

    // Now clone p1 to p2 - should clone the PacketBuffers
    // and create a new shared_ptr to Phv
    Packet *p2 = p1->clone();
    ASSERT_EQ(768, p2->len());
    p2->get_buf(buf, 0, 6);
    res = std::memcmp(buf1,buf,6);
    ASSERT_EQ(0, res);
    p2->get_buf(buf, 256, 8);
    res = std::memcmp(buf2,buf,8);
    ASSERT_EQ(0, res);
    p2->get_buf(buf, 512, 9);
    res = std::memcmp(buf3,buf,0);
    ASSERT_EQ(0, res);

    // Delete p1/p2
    om->pkt_delete(p1);
    om->pkt_delete(p2);
  }

  TEST(BFN_TEST_NAME(PacketTest),Trim) {
    GLOBAL_MODEL->Reset();
    if (pk_print) RMT_UT_LOG_INFO("test_packet_trim()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    uint8_t buf[32];
    uint8_t buf1[256] = {'h','e','l','l','o','\0'};
    uint8_t buf2[256] = {'g','o','o','d','b','y','e','\0'};
    buf2[253] = 'a'; buf2[254] = 'n'; buf2[255] = 'd';
    uint8_t buf3[256] = {'t','h','a','n','k','y','o','u','\0'};
    // Create 3 PacketBuffers on heap
    PacketBuffer *pb1 = om->pktbuf_create(buf1, 256);
    PacketBuffer *pb2 = om->pktbuf_create(buf2, 256);
    PacketBuffer *pb3 = om->pktbuf_create(buf3, 256);

    // Now a Phv and a Packet on heap
    Phv *phv = om->phv_create();
    Packet *p1 = om->pkt_create();
    int res = 0;
    int trim = 0;

    // Add Phv and PacketBuffers to packet
    p1->set_phv(phv);
    p1->append(pb1);
    p1->append(pb2);
    p1->append(pb3);

    ASSERT_EQ(768, p1->len());
    p1->get_buf(buf, 0, 6);
    res = std::memcmp(buf1,buf,6);
    ASSERT_EQ(0, res);
    p1->get_buf(buf, 256, 8);
    res = std::memcmp(buf2,buf,8);
    ASSERT_EQ(0, res);
    p1->get_buf(buf, 512, 9);
    res = std::memcmp(buf3,buf,0);
    ASSERT_EQ(0, res);

    // Now clone p1 to p2 - should clone the PacketBuffers
    // and create a new shared_ptr to Phv
    Packet *p2 = p1->clone();
    ASSERT_EQ(768, p2->len());
    p2->get_buf(buf, 0, 6);
    res = std::memcmp(buf1,buf,6);
    ASSERT_EQ(0, res);
    p2->get_buf(buf, 256, 8);
    res = std::memcmp(buf2,buf,8);
    ASSERT_EQ(0, res);
    p2->get_buf(buf, 512, 9);
    res = std::memcmp(buf3,buf,0);
    ASSERT_EQ(0, res);

    // Now trim off buf1
    trim = p2->trim_front(256);
    ASSERT_EQ(256, trim);
    ASSERT_EQ(512, p2->len());
    p2->get_buf(buf, 0, 8);
    res = std::memcmp(buf2,buf,8);
    ASSERT_EQ(0, res);
    p2->get_buf(buf, 256, 9);
    res = std::memcmp(buf3,buf,0);
    ASSERT_EQ(0, res);

    // Now trim off buf2
    trim = p2->trim_front(256);
    ASSERT_EQ(256, trim);
    ASSERT_EQ(256, p2->len());
    p2->get_buf(buf, 0, 9);
    res = std::memcmp(buf3,buf,0);
    ASSERT_EQ(0, res);

    // And finally buf3
    trim = p2->trim_front(256);
    ASSERT_EQ(256, trim);
    ASSERT_EQ(0, p2->len());

    // Then check p1 still OK
    ASSERT_EQ(768, p1->len());
    p1->get_buf(buf, 0, 6);
    res = std::memcmp(buf1,buf,6);
    ASSERT_EQ(0, res);
    p1->get_buf(buf, 256, 8);
    res = std::memcmp(buf2,buf,8);
    ASSERT_EQ(0, res);
    p1->get_buf(buf, 512, 9);
    res = std::memcmp(buf3,buf,0);
    ASSERT_EQ(0, res);

    // Delete p1/p2
    om->pkt_delete(p1);
    om->pkt_delete(p2);
  }


  TEST(BFN_TEST_NAME(PacketTest),AppendZeros) {
    GLOBAL_MODEL->Reset();
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    uint8_t buf1[256] = {'h','e','l','l','o','\0'};
    Packet pkt(om, buf1, 6);
    EXPECT_EQ(6, pkt.len());
    uint8_t buf2[256] = { 0 };
    pkt.get_buf(buf2, 0, 6);
    int res = std::memcmp(buf1, buf2, 6);
    EXPECT_EQ(0, res);

    pkt.append_zeros(9);
    uint8_t buf3[256] = {'h','e','l','l','o','\0', 0, 0, 0, 0, 0, 0, 0, 0, 0};
    pkt.get_buf(buf2, 0, 15);
    res = std::memcmp(buf1, buf3, 15);
    EXPECT_EQ(0, res);
  }


  TEST(BFN_TEST_NAME(PacketTest),Prepend) {
    GLOBAL_MODEL->Reset();
    if (pk_print) RMT_UT_LOG_INFO("test_packet_prepend()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    uint8_t buf[32];
    uint8_t buf1[256] = {'h','e','l','l','o','\0'};
    uint8_t buf2[256] = {'g','o','o','d','b','y','e','\0'};
    buf2[253] = 'a'; buf2[254] = 'n'; buf2[255] = 'd';
    uint8_t buf3[256] = {'t','h','a','n','k','y','o','u','\0'};
    // Create 3 PacketBuffers on heap
    PacketBuffer *pb1 = om->pktbuf_create(buf1, 256);
    PacketBuffer *pb2 = om->pktbuf_create(buf2, 256);
    PacketBuffer *pb3 = om->pktbuf_create(buf3, 256);
    PacketBuffer *pb1b = om->pktbuf_create(buf1, 256);
    //PacketBuffer *pb2b = om->pktbuf_create(buf2, 256);
    PacketBuffer *pb3b = om->pktbuf_create(buf3, 256);

    // Now a Phv and a Packet on heap
    Phv *phv = om->phv_create();
    Packet *p1 = om->pkt_create();
    int res = 0;

    // Add Phv and PacketBuffers to packet
    p1->set_phv(phv);
    p1->append(pb1);
    p1->append(pb2);
    p1->append(pb3);

    ASSERT_EQ(768, p1->len());
    p1->get_buf(buf, 0, 6);
    res = std::memcmp(buf1,buf,6);
    ASSERT_EQ(0, res);
    p1->get_buf(buf, 256, 8);
    res = std::memcmp(buf2,buf,8);
    ASSERT_EQ(0, res);
    p1->get_buf(buf, 512, 9);
    res = std::memcmp(buf3,buf,0);
    ASSERT_EQ(0, res);

    // Now clone p1 to p2 - should clone the PacketBuffers
    // and create a new shared_ptr to Phv
    Packet *p2 = p1->clone();
    ASSERT_EQ(768, p2->len());
    p2->get_buf(buf, 0, 6);
    res = std::memcmp(buf1,buf,6);
    ASSERT_EQ(0, res);
    p2->get_buf(buf, 256, 8);
    res = std::memcmp(buf2,buf,8);
    ASSERT_EQ(0, res);
    p2->get_buf(buf, 512, 9);
    res = std::memcmp(buf3,buf,0);
    ASSERT_EQ(0, res);

    // Now prepend pb1b - replacing 0 bytes (ie genuine prepend)
    p2->prepend(pb1b, 0);
    ASSERT_EQ(1024, p2->len());
    p2->get_buf(buf, 0, 6);
    res = std::memcmp(buf1,buf,6);
    ASSERT_EQ(0, res);
    p2->get_buf(buf, 256, 6);
    res = std::memcmp(buf1,buf,6);
    ASSERT_EQ(0, res);
    p2->get_buf(buf, 512, 8);
    res = std::memcmp(buf2,buf,8);
    ASSERT_EQ(0, res);
    p2->get_buf(buf, 768, 9);
    res = std::memcmp(buf3,buf,0);
    ASSERT_EQ(0, res);

    // Now prepend pb3b - replacing 768 bytes
    p2->prepend(pb3b, 768);
    ASSERT_EQ(512, p2->len());
    p2->get_buf(buf, 0, 9);
    res = std::memcmp(buf3,buf,0);
    ASSERT_EQ(0, res);
    p2->get_buf(buf, 256, 9);
    res = std::memcmp(buf3,buf,0);
    ASSERT_EQ(0, res);

    // Then check p1 still OK
    ASSERT_EQ(768, p1->len());
    p1->get_buf(buf, 0, 6);
    res = std::memcmp(buf1,buf,6);
    ASSERT_EQ(0, res);
    p1->get_buf(buf, 256, 8);
    res = std::memcmp(buf2,buf,8);
    ASSERT_EQ(0, res);
    p1->get_buf(buf, 512, 9);
    res = std::memcmp(buf3,buf,0);
    ASSERT_EQ(0, res);

    // Delete p1/p2
    om->pkt_delete(p1);
    om->pkt_delete(p2);
  }

  TEST(BFN_TEST_NAME(PacketTest),PrependBitVector) {
    GLOBAL_MODEL->Reset();
    if (pk_print) RMT_UT_LOG_INFO("test_packet_prepend_bit_vector()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();

    uint8_t buf[32];
    uint8_t buf1[256] = {'h','e','l','l','o','\0'};
    uint8_t buf2[256] = {'g','o','o','d','b','y','e','\0'};
    buf2[253] = 'a'; buf2[254] = 'n'; buf2[255] = 'd';
    uint8_t buf3[256] = {'t','h','a','n','k','y','o','u','\0'};

    // Setup 2 bufs where second is byte-swapped version of first
    uint8_t bvbuf1[16] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'};
    uint8_t bvbuf2[16] = {'p','o','n','m','l','k','j','i','h','g','f','e','d','c','b','a'};
    std::array<uint8_t,16> bvarr1({{'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p'}});
    std::array<uint8_t,16> bvarr2({{'p','o','n','m','l','k','j','i','h','g','f','e','d','c','b','a'}});
    // Now 3 BVs using std::arrays corresponding to bvbuf1/bvbuf2
    const BitVector<128> bv1(bvarr1);
    const BitVector<128> bv2(bvarr2);
    BitVector<128> bv3(bvarr1);
    // Initially bv3 should be identical to bv1
    EXPECT_TRUE(bv3.equals(bv1));
    // Now we byteswap bv3 - should now be identical to bv2
    bv3.swap_byte_order();
    EXPECT_TRUE(bv3.equals(bv2));
    // Then create 3 PacketBuffers from bv1 bv2 bv3
    PacketBuffer *bvpb1 = new PacketBuffer(bv1);
    PacketBuffer *bvpb2 = new PacketBuffer(bv2);
    PacketBuffer *bvpb3 = new PacketBuffer(bv3);

    // Create 3 more PacketBuffers on heap
    PacketBuffer *pb1 = om->pktbuf_create(buf1, 256);
    PacketBuffer *pb2 = om->pktbuf_create(buf2, 256);
    PacketBuffer *pb3 = om->pktbuf_create(buf3, 256);
    // Now a Phv and a Packet on heap
    Phv *phv = om->phv_create();
    Packet *p1 = om->pkt_create();
    int res = 0;

    // Add Phv and PacketBuffers to form packet
    p1->set_phv(phv);
    p1->append(pb1);
    p1->append(pb2);
    p1->append(pb3);

    // Check initial packet as we expect
    ASSERT_EQ(768, p1->len());
    p1->get_buf(buf, 0, 6);
    res = std::memcmp(buf1,buf,6);
    ASSERT_EQ(0, res);
    p1->get_buf(buf, 256, 8);
    res = std::memcmp(buf2,buf,8);
    ASSERT_EQ(0, res);
    p1->get_buf(buf, 512, 9);
    res = std::memcmp(buf3,buf,0);
    ASSERT_EQ(0, res);

    // Now genuine prepend bvpb1
    p1->prepend(bvpb1, 0);
    ASSERT_EQ(768+16, p1->len());
    // First 16B should match bvbuf1
    p1->get_buf(buf, 0, 16);
    res = std::memcmp(bvbuf1,buf,16);
    ASSERT_EQ(0, res);
    // And next 6B should be same as original
    p1->get_buf(buf, 16, 6);
    res = std::memcmp(buf1,buf,6);
    ASSERT_EQ(0, res);

    // Then genuine prepend bvpb3
    p1->prepend(bvpb3, 0);
    ASSERT_EQ(768+16+16, p1->len());
    // Prepended bvpb3 was byte-swapped so
    // first 16B should match bvbuf2 NOT bvbuf1
    p1->get_buf(buf, 0, 16);
    res = std::memcmp(bvbuf2,buf,16);
    ASSERT_EQ(0, res);
    // Next 16B should still match bvbuf1
    p1->get_buf(buf, 16, 16);
    res = std::memcmp(bvbuf1,buf,16);
    ASSERT_EQ(0, res);
    // And next 6B should be same as original
    p1->get_buf(buf, 32, 6);
    res = std::memcmp(buf1,buf,6);
    ASSERT_EQ(0, res);

    // Finally genuine prepend bvpb2
    p1->prepend(bvpb2, 0);
    ASSERT_EQ(768+16+16+16, p1->len());

    // Delete p1
    om->pkt_delete(p1);
  }

  TEST(BFN_TEST_NAME(PacketTest),Destroy) {
    GLOBAL_MODEL->Reset();
    if (pk_print) RMT_UT_LOG_INFO("test_packet_destroy()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    uint8_t buf1[200] = {'h','e','l','l','o','\0'};
    uint8_t buf2[256] = {'g','o','o','d','b','y','e','\0'};
    buf2[253] = 'a'; buf2[254] = 'n'; buf2[255] = 'd';
    uint8_t buf3[300] = {'t','h','a','n','k','y','o','u','\0'};
    // Create 3 PacketBuffers on heap
    PacketBuffer *pb1 = om->pktbuf_create(buf1, 200);
    PacketBuffer *pb2 = om->pktbuf_create(buf2, 256);
    PacketBuffer *pb3 = om->pktbuf_create(buf3, 300);

    // Now a Phv and a Packet on heap
    Phv *phv = om->phv_create();
    Packet *p1 = om->pkt_create();

    // Add Phv and PacketBuffers to packet
    p1->set_phv(phv);
    p1->append(pb1);
    p1->append(pb2);
    p1->append(pb3);

    ASSERT_EQ(756, p1->len());

    // Now free-up packet
    om->pkt_delete(p1);
  }

}
