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
#include <cstdint>
#include <string>
#include <array>
#include <cassert>

#include "gtest.h"

#include <bitvector.h>
#include <mau-tcam.h>
#include <rmt-object-manager.h>
#include <packet.h>
#include <phv.h>
#include <parser-static-config.h>

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool bv_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;

  TEST(BFN_TEST_NAME(BitvectorTest),Basic) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_basic()\n");
    std::array<uint64_t,2> b = {{UINT64_C(0xAAAABBBBCCCCDDDD), UINT64_C(0x1111222233334444)}};

    BitVector<128> bv(b);
    if (bv_print) bv.print();

    const BitVector<128> bv2{{UINT64_C(0x1234123412341234),UINT64_C(1)}};
    if (bv_print) bv2.print();

    uint32_t i0 = 0x12345678;
    uint32_t i1 = 0x9ABCDEF0;
    uint32_t i2 = 0xA0A1A2A3;
    uint32_t i3 = 0xB0B1B2B3;
    const BitVector<128> bv3(std::array<uint32_t,4>({{i0,i1,i2,i3}}));
    if (bv_print) bv3.print();

    const BitVector<120> bv4(std::array<uint8_t,15>({{1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf}}));
    if (bv_print) bv4.print();
  }

  TEST(BFN_TEST_NAME(BitvectorTest),EqualityUint8) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_equality_uint8()\n");
    BitVector<40> bv1(std::array<uint8_t,5>({{1,2,3,4,5}}));
    BitVector<40> bv2(std::array<uint8_t,5>({{1,2,3,4,5}}));
    BitVector<40> bv3(std::array<uint8_t,5>({{1,2,0,4,5}}));
    uint8_t M = 0xFFu;
    BitVector<40> bvM(std::array<uint8_t,5>({{M,M,0,M,M}}));
    if (bv_print) bv1.print();
    if (bv_print) bv2.print();
    ASSERT_TRUE(bv1.equals(bv2));
    if (bv_print) bv1.print();
    if (bv_print) bv3.print();
    ASSERT_TRUE(!bv1.equals(bv3));
    ASSERT_TRUE(bv1.masked_equals(bv3,bvM));
  }

  TEST(BFN_TEST_NAME(BitvectorTest),EqualityUint16) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_equality_uint16()\n");
    BitVector<80> bv1(std::array<uint16_t,5>({{1,2,3,4,5}}));
    BitVector<80> bv2(std::array<uint16_t,5>({{1,2,3,4,5}}));
    BitVector<80> bv3(std::array<uint16_t,5>({{1,2,0,4,5}}));
    uint16_t M = 0xFFFFu;
    BitVector<80> bvM(std::array<uint16_t,5>({{M,M,0,M,M}}));
    if (bv_print) bv1.print();
    if (bv_print) bv2.print();
    ASSERT_TRUE(bv1.equals(bv2));
    if (bv_print) bv1.print();
    if (bv_print) bv3.print();
    ASSERT_TRUE(!bv1.equals(bv3));
    ASSERT_TRUE(bv1.masked_equals(bv3,bvM));
  }

  TEST(BFN_TEST_NAME(BitvectorTest),EqualityUint32) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_equality_uint32()\n");
    BitVector<160> bv1(std::array<uint32_t,5>({{1,2,3,4,5}}));
    BitVector<160> bv2(std::array<uint32_t,5>({{1,2,3,4,5}}));
    BitVector<160> bv3(std::array<uint32_t,5>({{1,2,0,4,5}}));
    uint32_t M = 0xFFFFFFFFu;
    BitVector<160> bvM(std::array<uint32_t,5>({{M,M,0,M,M}}));
    if (bv_print) bv1.print();
    if (bv_print) bv2.print();
    ASSERT_TRUE(bv1.equals(bv2));
    if (bv_print) bv1.print();
    if (bv_print) bv3.print();
    ASSERT_TRUE(!bv1.equals(bv3));
    ASSERT_TRUE(bv1.masked_equals(bv3,bvM));
  }

  TEST(BFN_TEST_NAME(BitvectorTest),GetWord) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_get_word()\n");
    BitVector<80> bv1(std::array<uint8_t,10>(
      {{0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99}}));
    uint64_t w0 = (uint64_t)bv1.get_word(0,32);
    uint64_t w8 = (uint64_t)bv1.get_word(8,32);
    uint64_t w16 = (uint64_t)bv1.get_word(16,32);
    uint64_t w24 = (uint64_t)bv1.get_word(24,32);
    if (bv_print)
      RMT_UT_LOG_INFO("BV:get_word w0=0x%016" PRIx64 " w8=0x%016" PRIx64
             " w16=0x%016" PRIx64 " w24=0x%016" PRIx64 "\n",
             w0, w8, w16, w24);
    ASSERT_TRUE((bv1.get_word(0,32)  == UINT64_C(0x0000000033221100)));
    ASSERT_TRUE((bv1.get_word(8,32)  == UINT64_C(0x0000000044332211)));
    ASSERT_TRUE((bv1.get_word(16,32) == UINT64_C(0x0000000055443322)));
    ASSERT_TRUE((bv1.get_word(24,32) == UINT64_C(0x0000000066554433)));
    ASSERT_TRUE((bv1.get_word(32,32) == UINT64_C(0x0000000077665544)));
    ASSERT_TRUE((bv1.get_word(40,32) == UINT64_C(0x0000000088776655)));
    ASSERT_TRUE((bv1.get_word(48,32) == UINT64_C(0x0000000099887766)));
    ASSERT_TRUE((bv1.get_word(56,32) == UINT64_C(0x0000000000998877)));
    ASSERT_TRUE((bv1.get_word(64,32) == UINT64_C(0x0000000000009988)));
    ASSERT_TRUE((bv1.get_word(72,32) == UINT64_C(0x0000000000000099)));
    ASSERT_TRUE((bv1.get_word(76,32) == UINT64_C(0x0000000000000009)));

    ASSERT_TRUE((bv1.get_word(48,28) == UINT64_C(0x0000000009887766)));

  }

  TEST(BFN_TEST_NAME(BitvectorTest),GetByte) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_get_byte()\n");
    BitVector<80> bv1(std::array<uint8_t,10>(
      {{0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99}}));
    uint64_t w88 = (uint64_t)bv1.get_word(8,8);
    if (bv_print)
      RMT_UT_LOG_INFO("BV:get_word w88=0x%016" PRIx64 "\n", w88);
    ASSERT_EQ(0x00, bv1.get_byte(0));
    ASSERT_EQ(0x11, bv1.get_byte(1));
    ASSERT_EQ(0x22, bv1.get_byte(2));
    ASSERT_EQ(0x33, bv1.get_byte(3));
    ASSERT_EQ(0x44, bv1.get_byte(4));
    ASSERT_EQ(0x55, bv1.get_byte(5));
    ASSERT_EQ(0x66, bv1.get_byte(6));
    ASSERT_EQ(0x77, bv1.get_byte(7));
    ASSERT_EQ(0x88, bv1.get_byte(8));
    ASSERT_EQ(0x99, bv1.get_byte(9));
  }

  TEST(BFN_TEST_NAME(BitvectorTest),GetBit) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_get_bit()\n");
    uint64_t w = UINT64_C(0);
    uint64_t one = UINT64_C(1);
    w |= ((one << 1) | (one << 3) | (one << 5) | (one << 7));
    w |= ((one << 11) | (one << 13) | (one << 17) | (one << 19));
    BitVector<96> bv1(std::array<uint64_t,2>({{w,UINT64_C(0)}}));
    if (bv_print)
      RMT_UT_LOG_INFO("BV:get_word(0)=0x%016" PRIx64 "\n", bv1.get_word(0));
    ASSERT_EQ(0x0, bv1.get_bit(0));
    ASSERT_EQ(0x1, bv1.get_bit(1));
    ASSERT_EQ(0x0, bv1.get_bit(2));
    ASSERT_EQ(0x1, bv1.get_bit(3));
    ASSERT_EQ(0x0, bv1.get_bit(4));
    ASSERT_EQ(0x1, bv1.get_bit(5));
    ASSERT_EQ(0x0, bv1.get_bit(6));
    ASSERT_EQ(0x1, bv1.get_bit(7));
    ASSERT_EQ(0x0, bv1.get_bit(8));
    ASSERT_EQ(0x0, bv1.get_bit(8));
    ASSERT_EQ(0x0, bv1.get_bit(10));
    ASSERT_EQ(0x1, bv1.get_bit(11));
    ASSERT_EQ(0x0, bv1.get_bit(12));
    ASSERT_EQ(0x1, bv1.get_bit(13));
    ASSERT_EQ(0x0, bv1.get_bit(14));
    ASSERT_EQ(0x0, bv1.get_bit(15));
    ASSERT_EQ(0x0, bv1.get_bit(16));
    ASSERT_EQ(0x1, bv1.get_bit(17));
    ASSERT_EQ(0x0, bv1.get_bit(18));
    ASSERT_EQ(0x1, bv1.get_bit(19));
    ASSERT_EQ(0x0, bv1.get_bit(20));
  }

  TEST(BFN_TEST_NAME(BitvectorTest),BitSet) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_bit_set()\n");
    uint64_t w = UINT64_C(0);
    uint64_t one = UINT64_C(1);
    w |= ((one << 1) | (one << 3) | (one << 5) | (one << 7));
    w |= ((one << 11) | (one << 13) | (one << 17) | (one << 19));
    BitVector<96> bv1(std::array<uint64_t,2>({{w,UINT64_C(0)}}));
    if (bv_print)
      RMT_UT_LOG_INFO("BV:get_word(0)=0x%016" PRIx64 "\n", bv1.get_word(0));
    ASSERT_FALSE(bv1.bit_set(0));
    ASSERT_TRUE(bv1.bit_set(1));
    ASSERT_FALSE(bv1.bit_set(2));
    ASSERT_TRUE(bv1.bit_set(3));
    ASSERT_FALSE(bv1.bit_set(4));
    ASSERT_TRUE(bv1.bit_set(5));
    ASSERT_FALSE(bv1.bit_set(6));
    ASSERT_TRUE(bv1.bit_set(7));
    ASSERT_FALSE(bv1.bit_set(8));
    ASSERT_FALSE(bv1.bit_set(8));
    ASSERT_FALSE(bv1.bit_set(10));
    ASSERT_TRUE(bv1.bit_set(11));
    ASSERT_FALSE(bv1.bit_set(12));
    ASSERT_TRUE(bv1.bit_set(13));
    ASSERT_FALSE(bv1.bit_set(14));
    ASSERT_FALSE(bv1.bit_set(15));
    ASSERT_FALSE(bv1.bit_set(16));
    ASSERT_TRUE(bv1.bit_set(17));
    ASSERT_FALSE(bv1.bit_set(18));
    ASSERT_TRUE(bv1.bit_set(19));
    ASSERT_FALSE(bv1.bit_set(20));
  }

  TEST(BFN_TEST_NAME(BitvectorTest),Extract) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_extract()\n");
    BitVector<48> bvCMP1(std::array<uint8_t,6>({{0x1,0x2,0x3,0x4,0x5,0x6}}));
    BitVector<48> bvE1(std::array<uint8_t,6>({{0,0,0,0,0,0}}));
    BitVector<48> bvCMP2(std::array<uint8_t,6>({{0x7,0x8,0x9,0xA,0xB,0xC}}));
    BitVector<48> bvE2(std::array<uint8_t,6>({{0,0,0,0,0,0}}));
    BitVector<48> bvE3(std::array<uint8_t,6>({{0,0,0,0,0,0}}));

    BitVector<96> bv1(std::array<uint8_t,12>({{0x1,0x2,0x3,0x4,0x5,0x6,
              0x7,0x8,0x9,0xA,0xB,0xC}}));
    if (bv_print) bv1.print();
    BitVector<96> bv2(std::array<uint16_t,6>({{0x0201,0x0403,0x0605,0x0807,0x0A09,0x0C0B}}));
    BitVector<96> bv3(std::array<uint32_t,3>({{0x04030201,0x08070605,0x0C0B0A09}}));
    ASSERT_TRUE(bv1.equals(bv2));
    ASSERT_TRUE(bv2.equals(bv3));
    ASSERT_TRUE(bv1.equals(bv3));

    bv1.extract_into(0, &bvE1);
    if (bv_print) bv1.print();
    if (bv_print) bvCMP1.print();
    if (bv_print) bvE1.print();
    ASSERT_TRUE(bvCMP1.equals(bvE1));
    bv1.extract_into(48, &bvE2);
    if (bv_print) bv1.print();
    if (bv_print) bvCMP2.print();
    if (bv_print) bvE2.print();
    ASSERT_TRUE(bvCMP2.equals(bvE2));

    for (int i = 0; i < 48; ++i) {
      bv1.extract_into(i, &bvE1);
      bv2.extract_into(i, &bvE2);
      bv3.extract_into(i, &bvE3);

      ASSERT_TRUE(bvE1.equals(bvE2));
      ASSERT_TRUE(bvE2.equals(bvE3));
      ASSERT_TRUE(bvE1.equals(bvE3));

      ASSERT_TRUE(bv1.equals(bv2));
      ASSERT_TRUE(bv2.equals(bv3));
      ASSERT_TRUE(bv1.equals(bv3));
      //bv1.print();
    }
    for (int i = 0; i < 96; ++i) {
      uint64_t word = bv1.get_word(i);
      if ((i % 8) == 0)
        if (bv_print) RMT_UT_LOG_INFO("BV1(bitOff=%d) = %016" PRIx64 "\n", i, word);
    }
  }

  TEST(BFN_TEST_NAME(BitvectorTest),Fill) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_fill()\n");
    BitVector<80> bv1(std::array<uint16_t,5>({{1,2,3,4,5}}));
    BitVector<80> bv2(std::array<uint16_t,5>({{0x9999,0x9999,0x9999,0x9999,0x9999}}));

    if (bv_print) bv1.print();
    ASSERT_FALSE(bv1.equals(bv2));

    bv1.fill_all( 0x99 );
    if (bv_print) bv1.print();

    ASSERT_TRUE(bv1.equals(bv2));


  }

  TEST(BFN_TEST_NAME(BitvectorTest),SetBit) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_set_bit()\n");
    BitVector<80> bv1(std::array<uint16_t,5>({{0,0,0,0,0}}));

    ASSERT_TRUE((bv1.get_word(0) == UINT64_C(0x0000000000000000)));
    ASSERT_TRUE((bv1.get_word(64) == UINT64_C(0x0000000000000000)));
    bv1.set_bit(0);
    ASSERT_TRUE((bv1.get_word(0) == UINT64_C(0x0000000000000001)));
    ASSERT_TRUE((bv1.get_word(64) == UINT64_C(0x0000000000000000)));
    bv1.set_bit(79);
    ASSERT_TRUE((bv1.get_word(0) == UINT64_C(0x0000000000000001)));
    ASSERT_TRUE((bv1.get_word(64) == UINT64_C(0x0000000000008000)));
    bv1.set_bit(80);
    ASSERT_TRUE((bv1.get_word(0) == UINT64_C(0x0000000000000001)));
    ASSERT_TRUE((bv1.get_word(64) == UINT64_C(0x0000000000008000)));
    bv1.set_bit(76);
    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0x0000000000000001)));
    ASSERT_TRUE((bv1.get_word(64) == UINT64_C(0x0000000000009000)));
    bv1.clear_bit(76);
    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0x0000000000000001)));
    ASSERT_TRUE((bv1.get_word(64) == UINT64_C(0x0000000000008000)));

  }

  TEST(BFN_TEST_NAME(BitvectorTest),Parity) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_parity()\n");
    BitVector<256> bvp(std::array<uint64_t,4>({{
              UINT64_C(0x1111111111111111),
              UINT64_C(0x3333333333333333),
              UINT64_C(0x7777777777777777),
              UINT64_C(0xffffffffffffffff) }}));

    if (bv_print) bvp.print();

    EXPECT_EQ( 1, (bvp.parity< 0, 4 >()) );
    EXPECT_EQ( 0, (bvp.parity< 0, 8 >()) );
    EXPECT_EQ( 0, (bvp.parity< 0, 64 >()) );
    EXPECT_EQ( 1, (bvp.parity< 4, 64 >()) );
    EXPECT_EQ( 0, (bvp.parity< 64, 64 >()) );
    EXPECT_EQ( 1, (bvp.parity< 60, 68 >()) );
    EXPECT_EQ( 0, (bvp.parity< 60, 69 >()) );
    EXPECT_EQ( 1, (bvp.parity< 60, 70 >()) );
    EXPECT_EQ( 0, (bvp.parity< 60, 71 >()) );
    EXPECT_EQ( 0, (bvp.parity< 60, 72 >()) );
    EXPECT_EQ( 0, (bvp.parity< 0, 256 >()) );
    EXPECT_EQ( 1, (bvp.parity< 4, 252 >()) );
    EXPECT_EQ( 0, (bvp.parity< 4, 251 >()) );

  }

  TEST(BFN_TEST_NAME(BitvectorTest),SetByte) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_set_byte()\n");
    BitVector<80> bv1(std::array<uint16_t,5>({{0,0,0,0,0}}));

    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0x0000000000000000)));
    ASSERT_TRUE((bv1.get_word(64) == UINT64_C(0x0000000000000000)));
    bv1.set_byte(0xcc,0);
    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0x00000000000000cc)));
    ASSERT_TRUE((bv1.get_word(64) == UINT64_C(0x0000000000000000)));
    bv1.set_byte(0x11,0);
    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0x0000000000000011)));
    ASSERT_TRUE((bv1.get_word(64) == UINT64_C(0x0000000000000000)));
    bv1.set_byte(0xff,1);
    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0x000000000000ff11)));
    ASSERT_TRUE((bv1.get_word(64) == UINT64_C(0x0000000000000000)));
    bv1.set_byte(0x12,1);
    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0x0000000000001211)));
    ASSERT_TRUE((bv1.get_word(64) == UINT64_C(0x0000000000000000)));
    bv1.set_byte(0xee,7);
    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0xee00000000001211)));
    ASSERT_TRUE((bv1.get_word(64) == UINT64_C(0x0000000000000000)));
    bv1.set_byte(0x17,7);
    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0x1700000000001211)));
    ASSERT_TRUE((bv1.get_word(64) == UINT64_C(0x0000000000000000)));

    bv1.set_byte(0xdd,8);
    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0x1700000000001211)));
    ASSERT_TRUE((bv1.get_word(64) == UINT64_C(0x00000000000000dd)));
    bv1.set_byte(0x21,8);
    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0x1700000000001211)));
    ASSERT_TRUE((bv1.get_word(64) == UINT64_C(0x0000000000000021)));
  }

  TEST(BFN_TEST_NAME(BitvectorTest),GetFirstBitSet) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_get_first_bit_set()\n");

    // Find all combinations of bit pairs in [0..192]
    BitVector<193> bv0(UINT64_C(0));
    for (int i = 0; i < 192; i++) {
      for (int j = i+1; j < 193; j++) {
        bv0.set_bit(i);
        bv0.set_bit(j);
        EXPECT_EQ(i, bv0.get_first_bit_set());
        EXPECT_EQ(j, bv0.get_first_bit_set(i));
        EXPECT_EQ(-1,bv0.get_first_bit_set(j));
        bv0.clear_bit(j);
        bv0.clear_bit(i);
      }
    }
    bv0.set_bit(63);
    EXPECT_EQ(63,bv0.get_first_bit_set());
    EXPECT_EQ(-1,bv0.get_first_bit_set(63));

    BitVector<150> bv1(UINT64_C(0));
    bv1.set_bit(0);
    bv1.set_bit(1);
    bv1.set_bit(20);
    bv1.set_bit(40);
    bv1.set_bit(60);
    bv1.set_bit(80);
    bv1.set_bit(100);
    bv1.set_bit(120);
    bv1.set_bit(140);

    EXPECT_EQ(0,bv1.get_first_bit_set());
    EXPECT_EQ(1,bv1.get_first_bit_set(0));
    EXPECT_EQ(20,bv1.get_first_bit_set(1));
    EXPECT_EQ(40,bv1.get_first_bit_set(20));
    EXPECT_EQ(60,bv1.get_first_bit_set(40));
    EXPECT_EQ(80,bv1.get_first_bit_set(60));
    EXPECT_EQ(100,bv1.get_first_bit_set(80));
    EXPECT_EQ(120,bv1.get_first_bit_set(100));
    EXPECT_EQ(140,bv1.get_first_bit_set(120));
    EXPECT_EQ(-1,bv1.get_first_bit_set(140));

    EXPECT_EQ(0,bv1.get_first_bit_set());
    EXPECT_EQ(1,bv1.get_first_bit_set(0));
    EXPECT_EQ(20,bv1.get_first_bit_set(1));
    EXPECT_EQ(40,bv1.get_first_bit_set(20));
    EXPECT_EQ(60,bv1.get_first_bit_set(40));
    EXPECT_EQ(80,bv1.get_first_bit_set(60));
    EXPECT_EQ(100,bv1.get_first_bit_set(80));
    EXPECT_EQ(120,bv1.get_first_bit_set(100));
    EXPECT_EQ(140,bv1.get_first_bit_set(120));
    EXPECT_EQ(-1,bv1.get_first_bit_set(140));

    bv1.clear_bit(0);
    bv1.clear_bit(1);
    bv1.clear_bit(20);
    bv1.clear_bit(40);
    bv1.clear_bit(60);
    bv1.clear_bit(80);
    bv1.clear_bit(100);
    bv1.clear_bit(120);
    bv1.clear_bit(140);
    bv1.set_bit(0);
    bv1.set_bit(1);
    bv1.set_bit(10);
    bv1.set_bit(30);
    bv1.set_bit(50);
    bv1.set_bit(70);
    bv1.set_bit(90);
    bv1.set_bit(110);
    bv1.set_bit(130);

    EXPECT_EQ(0,bv1.get_first_bit_set());
    EXPECT_EQ(1,bv1.get_first_bit_set(0));
    EXPECT_EQ(10,bv1.get_first_bit_set(1));
    EXPECT_EQ(30,bv1.get_first_bit_set(10));
    EXPECT_EQ(50,bv1.get_first_bit_set(30));
    EXPECT_EQ(70,bv1.get_first_bit_set(50));
    EXPECT_EQ(90,bv1.get_first_bit_set(70));
    EXPECT_EQ(110,bv1.get_first_bit_set(90));
    EXPECT_EQ(130,bv1.get_first_bit_set(110));
    EXPECT_EQ(-1,bv1.get_first_bit_set(130));
  }

  TEST(BFN_TEST_NAME(BitvectorTest),GetLastBitSet) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_get_last_bit_set()\n");

    // Find all combinations of bit pairs in [0..192]
    BitVector<193> bv0(UINT64_C(0));
    for (int i = 0; i < 192; i++) {
      for (int j = i+1; j < 193; j++) {
        bv0.set_bit(i);
        bv0.set_bit(j);
        EXPECT_EQ(i, bv0.get_first_bit_set());
        EXPECT_EQ(j, bv0.get_first_bit_set(i));
        EXPECT_EQ(-1,bv0.get_first_bit_set(j));
        EXPECT_EQ(j, bv0.get_last_bit_set());
        bv0.clear_bit(j);
        bv0.clear_bit(i);
      }
    }
    bv0.set_bit(63);
    EXPECT_EQ(63,bv0.get_first_bit_set());
    EXPECT_EQ(-1,bv0.get_first_bit_set(63));
  }

  TEST(BFN_TEST_NAME(BitvectorTest),SetFrom) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_set_from()\n");
    BitVector<80> bv1(std::array<uint16_t,5>({{0xffff,0xffff,0xffff,0xfffA,0xffff}}));
    if (bv_print) RMT_UT_LOG_INFO("BV1:get_word(0)=0x%016" PRIx64 "\n", bv1.get_word(0));
    BitVector<4>  bv2(std::array<uint8_t,1>({{0x0}}));
    BitVector<64> bv3;
    bv3.set_word(UINT64_C(0x7aaaaaaaaaaaaaaa),0,64);
    ASSERT_TRUE((bv3.get_word(0)  == UINT64_C(0x7aaaaaaaaaaaaaaa)));
    bv3.set_word(UINT64_C(0xf0a0a0a0a0a0a0a0),0,63);
    ASSERT_TRUE((bv3.get_word(0)  == UINT64_C(0x70a0a0a0a0a0a0a0)));
    bv3.set_word(UINT64_C(0xfafafafafafafafa),0,64);
    ASSERT_TRUE((bv3.get_word(0)  == UINT64_C(0xfafafafafafafafa)));
    bv3.set_word(UINT64_C(0xa0a0a0a0a0a0a0a0),0,64);
    if (bv_print) RMT_UT_LOG_INFO("BV3:get_word(0)=0x%016" PRIx64 "\n", bv3.get_word(0));
    ASSERT_TRUE((bv3.get_word(0)  == UINT64_C(0xa0a0a0a0a0a0a0a0)));
    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0xFFFAFFFFFFFFFFFF)));
    ASSERT_TRUE((bv2.get_word(0)  == UINT64_C(0x0)));

    bv1.set_from( 0, bv2 );
    if (bv_print) RMT_UT_LOG_INFO("BV1:get_word(0)=0x%016" PRIx64 "\n", bv1.get_word(0));
    if (bv_print) RMT_UT_LOG_INFO("BV2:get_word(0)=0x%016" PRIx64 "\n", bv2.get_word(0));
    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0xFFFAFFFFFFFFFFF0)));

    bv1.set_from( 7, bv2 );
    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0xFFFAFFFFFFFFF870)));

    //bv1.set_from( 77, bv2 ); // bv2 goes off the end, does nothing

    bv1.set_from( 62, bv2 );
    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0x3FFAFFFFFFFFF870)));
    if (bv_print) RMT_UT_LOG_INFO("BV1:get_word(64)=0x%016" PRIx64 "\n", bv1.get_word(64));
    ASSERT_TRUE((bv1.get_word(64)  == UINT64_C(0xFFFC)));

    bv3.set_word(UINT64_C(0x1234567812345678),0,64);
    bv1.set_from( 12, bv3 );
    if (bv_print) RMT_UT_LOG_INFO("BV1:get_word(0)=0x%016" PRIx64 "\n", bv1.get_word(0));
    if (bv_print) RMT_UT_LOG_INFO("BV1:get_word(64)=0x%016" PRIx64 "\n", bv1.get_word(64));
    ASSERT_TRUE((bv1.get_word(0)  == UINT64_C(0x4567812345678870)));
    ASSERT_TRUE((bv1.get_word(64)  == UINT64_C(0xF123)));
  }


  TEST(BFN_TEST_NAME(BitvectorTest),SetFromExtra) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_set_from_extra()\n");
    BitVector<80> bv0(std::array<uint16_t,5>({{0xffff,0xffff,0xffff,0xfffA,0xffff}}));
    BitVector<4>  bv1(std::array<uint8_t,1>({{0x0}}));
    //bv0.set_from( 77, bv1 ); // bv1 goes off the end, does nothing

    BitVector<160> bv2(UINT64_C(0xF0E1D2C3B4A59687));
    BitVector<160> bvCopy2(UINT64_C(0));
    BitVector<80> bvFront2;
    BitVector<80> bvBack2;

    bv2.extract_into(0, &bvFront2);
    bv2.extract_into(80, &bvBack2);
    bvCopy2.set_from(0, bvFront2);
    bvCopy2.set_from(80, bvBack2);
    EXPECT_TRUE(bv2.equals(bvCopy2));
  }


  TEST(BFN_TEST_NAME(BitvectorTest),Zero) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_zero()\n");
    BitVector<64> *bv = new BitVector<64>(UINT64_C(0));
    EXPECT_TRUE(bv->is_zero());

    for (int i = 0; i < 64; i++) {
      delete bv;
      bv = new BitVector<64>(UINT64_C(1) << i);
      EXPECT_FALSE(bv->is_zero());
    }
  }


  TEST(BFN_TEST_NAME(BitvectorTest),SwapByteOrder) {
    if (bv_print) RMT_UT_LOG_INFO("test_bitvector_swap_byte_order()\n");

    BitVector<16> bv16(UINT64_C(0));
    EXPECT_TRUE(bv16.is_zero());
    bv16.set_word(UINT64_C(0x1122),0);
    bv16.swap_byte_order();
    EXPECT_EQ(UINT64_C(0x2211), bv16.get_word(0));

    BitVector<24> bv24(UINT64_C(0));
    EXPECT_TRUE(bv24.is_zero());
    bv24.set_word(UINT64_C(0x112233),0);
    bv24.swap_byte_order();
    EXPECT_EQ(UINT64_C(0x332211), bv24.get_word(0));

    BitVector<40> bv40(UINT64_C(0));
    EXPECT_TRUE(bv40.is_zero());
    bv40.set_word(UINT64_C(0x1122334455),0);
    bv40.swap_byte_order();
    EXPECT_EQ(UINT64_C(0x5544332211), bv40.get_word(0));

    BitVector<64> bv64(UINT64_C(0));
    EXPECT_TRUE(bv64.is_zero());
    bv64.set_word(UINT64_C(0x1122334455667788),0);
    bv64.swap_byte_order();
    EXPECT_EQ(UINT64_C(0x8877665544332211), bv64.get_word(0));

    BitVector<120> bv120(UINT64_C(0));
    EXPECT_TRUE(bv120.is_zero());
    bv120.set_word(UINT64_C(0x8877665544332211), 0);
    bv120.set_word(UINT64_C(0x00FFEEDDCCBBAA99),64);
    bv120.swap_byte_order();
    EXPECT_EQ(UINT64_C(0x8899AABBCCDDEEFF), bv120.get_word(0));
    EXPECT_EQ(UINT64_C(0x0011223344556677), bv120.get_word(64));

    BitVector<128> bv128(UINT64_C(0));
    EXPECT_TRUE(bv128.is_zero());
    bv128.set_word(UINT64_C(0x7766554433221100), 0);
    bv128.set_word(UINT64_C(0xFFEEDDCCBBAA9988),64);
    bv128.swap_byte_order();
    EXPECT_EQ(UINT64_C(0x8899AABBCCDDEEFF), bv128.get_word(0));
    EXPECT_EQ(UINT64_C(0x0011223344556677), bv128.get_word(64));
  }

  TEST(BFN_TEST_NAME(BitvectorTest),ByteShiftLeft) {
    if (bv_print) RMT_UT_LOG_INFO("test_byte_shift_left()\n");

    // byte shift left test
    BitVector<128> bsl128(UINT64_C(0));
    EXPECT_TRUE(bsl128.is_zero());
    bsl128.set_word(UINT64_C(0x7766554433221100), 0);
    bsl128.set_word(UINT64_C(0xFFEEDDCCBBAA9988),64);
    bsl128.byte_shift_left(3);
    EXPECT_EQ(UINT64_C(0x4433221100000000), bsl128.get_word(0));
    EXPECT_EQ(UINT64_C(0xCCBBAA9988776655), bsl128.get_word(64));

    bsl128.set_word(UINT64_C(0x77665544332211AA), 0);
    bsl128.set_word(UINT64_C(0xFFEEDDCCBBAA9988),64);
    bsl128.byte_shift_left(15);
    EXPECT_EQ(UINT64_C(0x0000000000000000), bsl128.get_word(0));
    EXPECT_EQ(UINT64_C(0xAA00000000000000), bsl128.get_word(64));

    bsl128.set_word(UINT64_C(0x77665544332211AA), 0);
    bsl128.set_word(UINT64_C(0xFFEEDDCCBBAA9988),64);
    bsl128.byte_shift_left(20);
    EXPECT_EQ(UINT64_C(0x0000000000000000), bsl128.get_word(0));
    EXPECT_EQ(UINT64_C(0x0000000000000000), bsl128.get_word(64));

    bsl128.set_word(UINT64_C(0x77665544332211AA), 0);
    bsl128.set_word(UINT64_C(0xFFEEDDCCBBAA9988),64);
    bsl128.byte_shift_left(0);
    EXPECT_EQ(UINT64_C(0x77665544332211AA), bsl128.get_word(0));
    EXPECT_EQ(UINT64_C(0xFFEEDDCCBBAA9988), bsl128.get_word(64));
  }

  TEST(BFN_TEST_NAME(BitvectorTest),MaskedSetOnes) {
    if (bv_print) RMT_UT_LOG_INFO("test_masked_set_ones()\n");

    BitVector<192> bv(UINT64_C(0));
    EXPECT_TRUE(bv.is_zero());

    bv.masked_set_ones(1); // Selector bitmap 1 means set word0
    EXPECT_EQ(UINT64_C(0xFFFFFFFFFFFFFFFF), bv.get_word(0));
    EXPECT_EQ(UINT64_C(0x0000000000000000), bv.get_word(64));
    EXPECT_EQ(UINT64_C(0x0000000000000000), bv.get_word(128));
    bv.fill_all_zeros();

    bv.masked_set_ones(2); // Selector bitmap 2 means set word1
    EXPECT_EQ(UINT64_C(0x0000000000000000), bv.get_word(0));
    EXPECT_EQ(UINT64_C(0xFFFFFFFFFFFFFFFF), bv.get_word(64));
    EXPECT_EQ(UINT64_C(0x0000000000000000), bv.get_word(128));
    bv.fill_all_zeros();

    bv.masked_set_ones(3); // Selector bitmap 3 means set word0 *and* word1
    EXPECT_EQ(UINT64_C(0xFFFFFFFFFFFFFFFF), bv.get_word(0));
    EXPECT_EQ(UINT64_C(0xFFFFFFFFFFFFFFFF), bv.get_word(64));
    EXPECT_EQ(UINT64_C(0x0000000000000000), bv.get_word(128));
    bv.fill_all_zeros();
  }

  TEST(BFN_TEST_NAME(BitvectorTest),MaskedQuarter) {
    if (bv_print) RMT_UT_LOG_INFO("test_masked_quarter()\n");

    BitVector<256> bv(UINT64_C(0));
    EXPECT_TRUE(bv.is_zero());

    // Some basic checks first
    bv.set_word(UINT64_C(0xFFFFFFFFFFFFFFFF),  0);
    bv.set_word(UINT64_C(0xFFFFFFFFFFFFFFFF), 64);
    bv.masked_quarter(1);
    EXPECT_EQ(UINT64_C(0x000000000000FFFF), bv.get_word(0));
    EXPECT_EQ(UINT64_C(0xFFFFFFFFFFFFFFFF), bv.get_word(64));
    EXPECT_EQ(UINT64_C(0x0000000000000000), bv.get_word(128));
    bv.fill_all_zeros();

    bv.set_word(UINT64_C(0xFFFFFFFFFFFFFFFF),  0);
    bv.set_word(UINT64_C(0xFFFFFFFFFFFFFFFF), 64);
    bv.masked_quarter(2);
    EXPECT_EQ(UINT64_C(0xFFFFFFFFFFFFFFFF), bv.get_word(0));
    EXPECT_EQ(UINT64_C(0x000000000000FFFF), bv.get_word(64));
    EXPECT_EQ(UINT64_C(0x0000000000000000), bv.get_word(128));
    bv.fill_all_zeros();

    bv.set_word(UINT64_C(0xFFFFFFFFFFFFFFFF),  0);
    bv.set_word(UINT64_C(0xFFFFFFFFFFFFFFFF), 64);
    bv.masked_quarter(3);
    EXPECT_EQ(UINT64_C(0x000000000000FFFF), bv.get_word(0));
    EXPECT_EQ(UINT64_C(0x000000000000FFFF), bv.get_word(64));
    EXPECT_EQ(UINT64_C(0x0000000000000000), bv.get_word(128));
    bv.fill_all_zeros();

    // Check that bit 0,4,8,...,60 are the ones selected
    bv.set_word(UINT64_C(0x8888888888888888),   0);
    bv.set_word(UINT64_C(0x4444444444444444),  64);
    bv.set_word(UINT64_C(0x2222222222222222), 128);
    bv.set_word(UINT64_C(0x1111111111111111), 192);
    bv.masked_quarter(0xF);
    EXPECT_EQ(UINT64_C(0x0000000000000000), bv.get_word(0));
    EXPECT_EQ(UINT64_C(0x0000000000000000), bv.get_word(64));
    EXPECT_EQ(UINT64_C(0x0000000000000000), bv.get_word(128));
    EXPECT_EQ(UINT64_C(0x000000000000FFFF), bv.get_word(192));
    bv.fill_all_zeros();

    // Bit more thorough checking
    const int num_iterations = 100000;
    uint64_t v = UINT64_C(1234567890);
    for (int iterations = 0; iterations < num_iterations; iterations++) {
      // Knuth MMIX
      v = (v * UINT64_C(6364136223846793005)) + UINT64_C(1442695040888963407);
      uint16_t v16 = static_cast<uint16_t>(v & UINT64_C(0xFFFF));
      uint64_t v64_quartered = static_cast<uint64_t>(v16), v64 = UINT64_C(0);
      for (int bit = 0; bit < 16; bit++) {
        // v16[0] -> v64[3], v16[1] -> v64[7], ..... , v16[15] -> v64[63]
        if (((v16 >> bit) & 1) == 1) v64 |= ( UINT64_C(1) << (bit*4) );
      }
      bv.set_word(v64,   0);
      bv.set_word(v64,  64);
      bv.set_word(v64, 128);
      bv.set_word(v64, 192);
      bv.masked_quarter(5); // Word0 and Word2
      EXPECT_EQ(v64_quartered, bv.get_word(0));
      EXPECT_EQ(v64,           bv.get_word(64));
      EXPECT_EQ(v64_quartered, bv.get_word(128));
      EXPECT_EQ(v64,           bv.get_word(192));
      bv.fill_all_zeros();
    }
  }

  TEST(BFN_TEST_NAME(BitvectorTest),MaskedAdjacentOr) {
    if (bv_print) RMT_UT_LOG_INFO("test_masked_adjacent_or()\n");

    BitVector<256> bv(UINT64_C(0));
    EXPECT_TRUE(bv.is_zero());

    const int num_iterations = 100000;
    uint64_t v = UINT64_C(1234567890);
    for (int iterations = 0; iterations < num_iterations; iterations++) {
      // Knuth MMIX
      v = (v * UINT64_C(6364136223846793005)) + UINT64_C(1442695040888963407);
      uint32_t a32 = static_cast<uint32_t>(v & UINT64_C(0xFFFFFFFF));
      uint32_t b32 = static_cast<uint32_t>(v >> 32);
      uint64_t or32 = static_cast<uint64_t>( a32 | b32 ), v64 = UINT64_C(0);
      for (int bit = 0; bit < 32; bit++) {
        // b32[0]a32[0] -> v64[1:0], b32[1]a32[1] -> v64[3:2] etc
        if (((a32 >> bit) & 1) == 1) v64 |= ( UINT64_C(1) << ((bit*2)+0) );
        if (((b32 >> bit) & 1) == 1) v64 |= ( UINT64_C(1) << ((bit*2)+1) );
      }
      bv.set_word(v64,   0);
      bv.set_word(v64,  64);
      bv.set_word(v64, 128);
      bv.set_word(v64, 192);
      bv.masked_adjacent_or(5); // Word0 and Word2
      EXPECT_EQ(or32, bv.get_word(0));
      EXPECT_EQ(v64,  bv.get_word(64));
      EXPECT_EQ(or32, bv.get_word(128));
      EXPECT_EQ(v64,  bv.get_word(192));
      bv.fill_all_zeros();
    }
  }


  TEST(BFN_TEST_NAME(BitvectorTest),MaskedConcat) {
    if (bv_print) RMT_UT_LOG_INFO("test_masked_concat()\n");

    BitVector<512> bv(UINT64_C(0));
    EXPECT_TRUE(bv.is_zero());

    const int num_iterations = 100000;
    uint64_t v = UINT64_C(1234567890);
    for (int iterations = 0; iterations < num_iterations; iterations++) {

      for (int zero = 0; zero <= 1; zero++) {
        // Second time zero upper 'concatted' word

        uint64_t w[8] = { UINT64_C(0) };
        for (int i = 0; i < 8; i++) {
          // Knuth MMIX
          v = (v * UINT64_C(6364136223846793005)) + UINT64_C(1442695040888963407);
          w[i] = v;
          bv.set_word(v, i*64);
        }
        // Check everything as expected
        for (int i = 0; i < 8; i++) {
          EXPECT_EQ(w[i], bv.get_word(i*64));
        }

        // Note masked_concat clears bit from selector for upper 'concatted' words
        // - but it does not change the upper 'concatted' word by default
        // - however setting penultimate arg true causes the upper word to be zeroed
        uint64_t selector = UINT64_C(0xFF);

        // First concat should put w[N+1]bits[7:0] with w[N]bits[7:0] into w[N]bits[15:0]
        w[0] = ((w[1] & UINT64_C(0xFF)) << 8) | ((w[0] & UINT64_C(0xFF)) << 0);
        w[2] = ((w[3] & UINT64_C(0xFF)) << 8) | ((w[2] & UINT64_C(0xFF)) << 0);
        w[4] = ((w[5] & UINT64_C(0xFF)) << 8) | ((w[4] & UINT64_C(0xFF)) << 0);
        w[6] = ((w[7] & UINT64_C(0xFF)) << 8) | ((w[6] & UINT64_C(0xFF)) << 0);
        if (zero == 1) w[1] = w[3] = w[5] = w[7] = UINT64_C(0);
        selector = bv.masked_concat(selector, 1, 8, (zero == 1), true);
        // Check1
        for (int i = 0; i < 8; i++) {
          EXPECT_EQ(w[i], bv.get_word(i*64));
        }

        // Second concat should put w[N+2]bits[15:0] with w[N]bits[15:0] into w[N]bits[31:0]
        w[0] = ((w[2] & UINT64_C(0xFFFF)) << 16) | ((w[0] & UINT64_C(0xFFFF)) << 0);
        w[4] = ((w[6] & UINT64_C(0xFFFF)) << 16) | ((w[4] & UINT64_C(0xFFFF)) << 0);
        if (zero == 1) w[2] = w[6] = UINT64_C(0);
        selector = bv.masked_concat(selector, 2, 16, (zero == 1), true);
        // Check2
        for (int i = 0; i < 8; i++) {
          EXPECT_EQ(w[i], bv.get_word(i*64));
        }

        // Third concat should put w[4]bits[31:0] with w[0]bits[31:0] into w[0]bits[63:0]
        w[0] = ((w[4] & UINT64_C(0xFFFFFFFF)) << 32) | ((w[0] & UINT64_C(0xFFFFFFFF)) << 0);
        if (zero == 1) w[4] = UINT64_C(0);
        selector = bv.masked_concat(selector, 4, 32, (zero == 1), true);
        // Check3
        for (int i = 0; i < 8; i++) {
          EXPECT_EQ(w[i], bv.get_word(i*64));
        }

        bv.fill_all_zeros();
      }
    }
  }


  TEST(BFN_TEST_NAME(BitvectorTest),RtlMismatch0) {
    if (bv_print) RMT_UT_LOG_INFO("test_rtl_mismatch0()\n");

    BitVector<512> bv(UINT64_C(0));
    BitVector<512> bv2(UINT64_C(0));
    EXPECT_TRUE(bv.is_zero());
    EXPECT_TRUE(bv2.is_zero());

    bv.set_bit(0);
    bv.set_bit(64);
    bv.set_bit(128);
    bv.set_bit(192);
    bv.set_bit(256);
    bv.set_bit(320);
    bv.set_bit(384);
    bv.set_bit(448);

    BitVector<44> tcam_entry(UINT64_C(0));
    Tcam3<512,44> tcam;
    tcam.set_lookup_return_pri(true);
    tcam.set_tcam_start(512 - 1);
    // Setup our TCAM3 with every 64th entry programmed to have a boundary.
    // This means the other entries have *no* boundary i.e. MRD-expansion on!
    for (int i = 0; i < 8; i++) {
      tcam.set_word0_word1_nolock(i*64, tcam_entry, tcam_entry, 1, 1);
    }
    // Now expand our inital BV to form BV2 then check not the same
    tcam.tcam_find_range(bv, &bv2);
    EXPECT_FALSE(bv.equals(bv2));

#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
    MauChipTcamReg reg(-1,0,0,0,0,nullptr);
    uint32_t v = reg.compute_bitmap_result(0,512, &bv);
    // And MRD-expansion transforms 0x5555 -> 0x7FFFF
    EXPECT_EQ(0x5555u, v);
    uint32_t v2 = reg.compute_bitmap_result(0,512, &bv2);
    EXPECT_EQ(0x7fffu, v2);
#endif
  }


  TEST(BFN_TEST_NAME(BitvectorTest),RtlMismatch1) {
    if (bv_print) RMT_UT_LOG_INFO("test_rtl_mismatch1()\n");

    BitVector<512> bv(UINT64_C(0));
    EXPECT_TRUE(bv.is_zero());

    bv.set_bit(0);
    bv.set_bit(64);
    bv.set_bit(128);
    bv.set_bit(192);
    bv.set_bit(256);
    bv.set_bit(320);
    bv.set_bit(384);
    bv.set_bit(448);

    // Check BV as expected
    for (int i = 0; i < 8; i++) {
      EXPECT_EQ(UINT64_C(0x0000000000000001), bv.get_word(64*i));
    }

    // Reduce by a quarter - nothing should change - but show as 16b
    bv.masked_quarter(0xFF);
    for (int i = 0; i < 8; i++) {
      EXPECT_EQ(UINT64_C(0x0001), bv.get_word(64*i));
    }

    // Note masked_concat calls below  will modify selector
    uint64_t selector = UINT64_C(0xFF); // Select 0,1,2,3,4,5,6,7

    // OR adjacent bits in all 8 words - reducing to 8b
    // So binary 0000000000000001 should become binary 00000001 (0x1)
    bv.masked_adjacent_or(selector);
    // Check
    for (int i = 0; i < 8; i++) {
      uint64_t exp = UINT64_C(0x01);
      EXPECT_EQ(exp, bv.get_word(64*i));
    }
    // First CONCAT word(x+1)[7:0] and word(x)[7:0]
    // zeroing word(x+1) and updating the selector
    selector = bv.masked_concat(selector, 1, 8, true, true);
    EXPECT_EQ(UINT64_C(0x55), selector); // Select 0,2,4,6
    // Check
    for (int i = 0; i < 8; i++) {
      uint64_t exp = ((i%2)==0) ?UINT64_C(0x0101) :UINT64_C(0x0000);
      EXPECT_EQ(exp, bv.get_word(64*i));
    }

    // OR adjacent bits in 4 words 0,2,4,6 - reducing to 8b again
    // So binary 0000000100000001 should become binary 00010001 (0x11)
    bv.masked_adjacent_or(selector);
    // Check
    for (int i = 0; i < 8; i++) {
      uint64_t exp = ((i%2)==0) ?UINT64_C(0x11) :UINT64_C(0x00);
      EXPECT_EQ(exp, bv.get_word(64*i));
    }
    // Second CONCAT word(x+2)[7:0] word(x)[7:0]
    // zeroing word(x+2) and updating the selector
    selector = bv.masked_concat(selector, 2, 8, true, true);
    EXPECT_EQ(UINT64_C(0x11), selector); // Select 0,4
    // Check
    for (int i = 0; i < 8; i++) {
      uint64_t exp = ((i%4)==0) ?UINT64_C(0x1111) :UINT64_C(0x0000);
      EXPECT_EQ(exp, bv.get_word(64*i));
    }

    // OR adjacent bits in 2 words 0,4
    // So binary 0001000100010001 should become binary 01010101 (0x55)
    bv.masked_adjacent_or(selector);
    // Check
    for (int i = 0; i < 8; i++) {
      uint64_t exp = ((i%4)==0) ?UINT64_C(0x55) :UINT64_C(0x00);
      EXPECT_EQ(exp, bv.get_word(64*i));
    }
    // Third CONCAT word(x+4)[7:0] word(x)[7:0]
    // zeroing word(x+4) and updating the selector (now should be 0x1)
    selector = bv.masked_concat(selector, 4, 8, true, true);
    EXPECT_EQ(UINT64_C(0x1), selector); // Select 0
    // Check
    for (int i = 0; i < 8; i++) {
      uint64_t exp = (i==0) ?UINT64_C(0x5555) :UINT64_C(0x0000);
      EXPECT_EQ(exp, bv.get_word(64*i));
    }
  }

}
