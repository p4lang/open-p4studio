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

#include <bitvector.h>
#include <tcam3.h>
#include <rmt-object-manager.h>
#include <packet.h>
#include <phv.h>
#include <parser-static-config.h>


namespace MODEL_CHIP_TEST_NAMESPACE {

  bool tc3_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;

  void tcam3_break() {
  }

  TEST(BFN_TEST_NAME(Tcam3Test),Basic) {
    if (tc3_print) RMT_UT_LOG_INFO("test_tcam3_basic()\n");    
    Tcam3<256,40> t;
    BitVector<40> bv1(std::array<uint8_t,5>({{1,2,3,4,5}}));
    BitVector<40> bv2(std::array<uint8_t,5>({{0,0,0,0,0}}));
    BitVector<40> bv3(std::array<uint8_t,5>({{0,0,0,0,0}}));
    BitVector<40> bv4(std::array<uint8_t,5>({{4,4,4,4,4}}));
    BitVector<40> bv5(std::array<uint8_t,5>({{5,5,5,5,5}}));
    uint8_t M = 0xFFu;
    BitVector<40> bvL4(std::array<uint8_t,5>({{4,4,4,4,4}}));
    BitVector<40> bvL5(std::array<uint8_t,5>({{5,5,5,5,5}}));
    BitVector<40> bvL4b(std::array<uint8_t,5>({{4,4,99,4,4}}));
    BitVector<40> bvL5b(std::array<uint8_t,5>({{5,5,99,5,5}}));
    BitVector<40> bvM(std::array<uint8_t,5>({{M,M,0,M,M}}));

    BitVector<40> *bvP = new BitVector<40>(std::array<uint8_t,5>({{99,99,99,99,99}}));
    if (tc3_print) bv1.print();
    if (tc3_print) bv2.print();

    t.reset();
    t.set_tcam_start(255);
    
    // All lookups should fail initially
    EXPECT_EQ(-1, t.tcam_lookup(bvL4));
    EXPECT_EQ(-1, t.tcam_lookup(bvL5));
    EXPECT_EQ(-1, t.tcam_lookup(bvL4b));
    EXPECT_EQ(-1, t.tcam_lookup(bvL5b));

    t.set_value(99, *bvP);
    t.set_value(0, bv1);
    if (tc3_print) t.print(0);
    t.get(0, &bv2, &bv3);
    if (tc3_print) bv1.print();
    if (tc3_print) bv2.print();
    if (tc3_print) bv3.print();

    // Test lookup
    t.set_value(224, bv4);
    t.set_value_mask(124, bv4, bvM);
    t.set_value(225, bv5);
    t.set_value_mask(125, bv5, bvM);
    tcam3_break();
    int l0 = t.tcam_lookup(bvL4);
    if (tc3_print) RMT_UT_LOG_INFO("Found bvL4 at index %d\n", l0);
    EXPECT_EQ(224, l0);
    int l1 = t.tcam_lookup(bvL4b);
    EXPECT_EQ(124, l1);
    if (tc3_print) RMT_UT_LOG_INFO("Found bvL4b at index %d\n", l1);

    // Test masked lookup
    int l2 = t.tcam_lookup(bvL5);
    EXPECT_EQ(225, l2);
    if (tc3_print) RMT_UT_LOG_INFO("Found bvL5 at index %d\n", l2);
    int l3 = t.tcam_lookup(bvL5b);
    EXPECT_EQ(125, l3);
    if (tc3_print) RMT_UT_LOG_INFO("Found bvL5b at index %d\n", l3);
  }

  TEST(BFN_TEST_NAME(Tcam3Test),SetGet) {
    if (tc3_print) RMT_UT_LOG_INFO("test_tcam3_set_get()\n");    
    Tcam3<256,40> t;
    BitVector<40> bv4(std::array<uint8_t,5>({{4,4,4,4,4}}));
    BitVector<40> bv5(std::array<uint8_t,5>({{5,5,5,5,5}}));
    uint8_t M = 0xFFu;
    BitVector<40> bvL4(std::array<uint8_t,5>({{4,4,4,4,4}}));
    BitVector<40> bvL5(std::array<uint8_t,5>({{5,5,5,5,5}}));
    BitVector<40> bvL4b(std::array<uint8_t,5>({{4,4,99,4,4}}));
    BitVector<40> bvL5b(std::array<uint8_t,5>({{5,5,99,5,5}}));
    BitVector<40> bvM(std::array<uint8_t,5>({{M,M,0,M,M}}));
    BitVector<40> bvG4(std::array<uint8_t,5>({{99,99,99,99,99}}));
    BitVector<40> bvG5(std::array<uint8_t,5>({{99,99,99,99,99}}));
    BitVector<40> bvGM(std::array<uint8_t,5>({{99,99,99,99,99}}));

    t.reset();
    t.set_tcam_start(255);

    // Test lookup
    t.set_value(224, bv4);
    t.set_value_mask(124, bv4, bvM);
    t.set_value(225, bv5);
    t.set_value_mask(125, bv5, bvM);
    int l0 = t.tcam_lookup(bvL4);
    ASSERT_EQ(224, l0);
    int l1 = t.tcam_lookup(bvL4b);
    ASSERT_EQ(124, l1);

    // Test we can get what we set
    //t.get(124, &bvG4, &bvGM);
    t.get(124, &bvGM, &bvG4);
    bv4.mask(bvM);
    bvG4.mask(bvM);
    ASSERT_TRUE(bv4.equals(bvG4));
    ASSERT_TRUE(bvG4.equals(bv4));

    //t.get(125, &bvG5, &bvGM);
    t.get(125, &bvGM, &bvG5);
    bv5.mask(bvM);
    bvG5.mask(bvM);
    ASSERT_TRUE(bv5.equals(bvG5)); 
    ASSERT_TRUE(bvG5.equals(bv5));
 }

  TEST(BFN_TEST_NAME(Tcam3Test),Reset) {
    if (tc3_print) RMT_UT_LOG_INFO("test_tcam3_reset()\n");    
    Tcam3<256,40> t;
    BitVector<40> bv4(std::array<uint8_t,5>({{4,4,4,4,4}}));
    BitVector<40> bv5(std::array<uint8_t,5>({{5,5,5,5,5}}));
    uint8_t M = 0xFFu;
    BitVector<40> bvL4(std::array<uint8_t,5>({{4,4,4,4,4}}));
    BitVector<40> bvL5(std::array<uint8_t,5>({{5,5,5,5,5}}));
    BitVector<40> bvL4b(std::array<uint8_t,5>({{4,4,99,4,4}}));
    BitVector<40> bvL5b(std::array<uint8_t,5>({{5,5,99,5,5}}));
    BitVector<40> bvM(std::array<uint8_t,5>({{M,M,0,M,M}}));

    t.reset();
    t.set_tcam_start(255);

    // Test lookup
    t.set_value(224, bv4);
    t.set_value_mask(124, bv4, bvM);
    t.set_value(225, bv5);
    t.set_value_mask(125, bv5, bvM);
    int l0 = t.tcam_lookup(bvL4);
    ASSERT_EQ(224, l0);
    int l1 = t.tcam_lookup(bvL4b);
    ASSERT_EQ(124, l1);

    // Now reset and try again - lookups should get -1
    t.reset();
    t.set_tcam_start(255);
    int l2 = t.tcam_lookup(bvL4);
    ASSERT_EQ(-1, l2);
    int l3 = t.tcam_lookup(bvL4b);
    ASSERT_EQ(-1, l3);
  }

  TEST(BFN_TEST_NAME(Tcam3Test),StartOffset) {
    if (tc3_print) RMT_UT_LOG_INFO("test_tcam3_start_offset()\n");    
    Tcam3<256,40> t;
    BitVector<40> bv4(std::array<uint8_t,5>({{4,4,4,4,4}}));
    BitVector<40> bv5(std::array<uint8_t,5>({{5,5,5,5,5}}));
    uint8_t M = 0xFFu;
    BitVector<40> bvL4(std::array<uint8_t,5>({{4,4,4,4,4}}));
    BitVector<40> bvL5(std::array<uint8_t,5>({{5,5,5,5,5}}));
    BitVector<40> bvL4b(std::array<uint8_t,5>({{4,4,99,4,4}}));
    BitVector<40> bvL5b(std::array<uint8_t,5>({{5,5,99,5,5}}));
    BitVector<40> bvM(std::array<uint8_t,5>({{M,M,0,M,M}}));

    t.reset();
    t.set_tcam_start(255);

    // Test lookup
    t.set_value(224, bv4);
    t.set_value_mask(124, bv4, bvM);
    t.set_value(225, bv5);
    t.set_value_mask(125, bv5, bvM);
    int l0 = t.tcam_lookup(bvL4);
    ASSERT_EQ(224, l0);
    int l1 = t.tcam_lookup(bvL4b);
    ASSERT_EQ(124, l1);

    // Test masked lookup
    int l2 = t.tcam_lookup(bvL5);
    ASSERT_EQ(225, l2);
    int l3 = t.tcam_lookup(bvL5b);
    ASSERT_EQ(125, l3);

    // Now set TCAM start point and try again
    // NB. With TCAM3 you just get real index
    // returned in default mode
    int tcam_start_offset = 200;
    t.set_tcam_start(tcam_start_offset);
    int l4 = t.tcam_lookup(bvL4);
    ASSERT_EQ(124, l4);
    int l5 = t.tcam_lookup(bvL4b);
    ASSERT_EQ(124, l5);
  }

  TEST(BFN_TEST_NAME(Tcam3Test),StartOffsetPri) {
    if (tc3_print) RMT_UT_LOG_INFO("test_tcam3_start_offset_pri()\n");    
    Tcam3<256,40> t;
    BitVector<40> bv4(std::array<uint8_t,5>({{4,4,4,4,4}}));
    BitVector<40> bv5(std::array<uint8_t,5>({{5,5,5,5,5}}));
    uint8_t M = 0xFFu;
    BitVector<40> bvL4(std::array<uint8_t,5>({{4,4,4,4,4}}));
    BitVector<40> bvL5(std::array<uint8_t,5>({{5,5,5,5,5}}));
    BitVector<40> bvL4b(std::array<uint8_t,5>({{4,4,99,4,4}}));
    BitVector<40> bvL5b(std::array<uint8_t,5>({{5,5,99,5,5}}));
    BitVector<40> bvM(std::array<uint8_t,5>({{M,M,0,M,M}}));

    t.reset(true); // Ask for pri to be returned
    t.set_tcam_start(255);

    // Test lookup
    t.set_value(224, bv4);
    t.set_value_mask(124, bv4, bvM);
    t.set_value(225, bv5);
    t.set_value_mask(125, bv5, bvM);
    int l0 = t.tcam_lookup(bvL4);
    ASSERT_EQ(224, l0);
    int l1 = t.tcam_lookup(bvL4b);
    ASSERT_EQ(124, l1);

    // Test masked lookup
    int l2 = t.tcam_lookup(bvL5);
    ASSERT_EQ(225, l2);
    int l3 = t.tcam_lookup(bvL5b);
    ASSERT_EQ(125, l3);

    // Now set TCAM start point and try again
    // Should get pri return now
    int tcam_start_offset = 230;
    t.set_tcam_start(tcam_start_offset);
    int l4 = t.tcam_lookup(bvL4);
    ASSERT_EQ(255-(tcam_start_offset-224), l4);
    int l5 = t.tcam_lookup(bvL4b);
    ASSERT_EQ(255-(tcam_start_offset-124), l5);
  }

  uint64_t tcam3_compare_func_local(uint64_t w0, uint64_t w1,
                                   uint64_t s0, uint64_t s1) {
    //uint64_t val = ~(s0|s1) | (w0&w1) | (s0&w0) | (s1&w1);
    //uint64_t msk = ~(s0&s1) | (w0&w1);
    //return ~(val & msk);
    // Dimitri says this does the same as the truth table in the uArch document
    return (((~w0) & s0 ) | ((~w1) & s1 ));
  }

  TEST(BFN_TEST_NAME(Tcam3Test),TcamCompare) {
    if (tc3_print) RMT_UT_LOG_INFO("test_tcam3_tcam_compare()\n");

    // Not really a Tcam test
    // More a test of the funky RMT TCAM match logic
    uint64_t v0 = UINT64_C(0x0000000000000000);
    uint64_t v1 = UINT64_C(0xFFFFFFFFFFFFFFFF);
    
    uint64_t r0000 = BitVector<64>::tcam_compare_func(v0,v0,v0,v0);
    uint64_t r0001 = BitVector<64>::tcam_compare_func(v0,v0,v0,v1);
    uint64_t r0010 = BitVector<64>::tcam_compare_func(v0,v0,v1,v0);
    uint64_t r0011 = BitVector<64>::tcam_compare_func(v0,v0,v1,v1);
    uint64_t r0100 = BitVector<64>::tcam_compare_func(v0,v1,v0,v0);
    uint64_t r0101 = BitVector<64>::tcam_compare_func(v0,v1,v0,v1);
    uint64_t r0110 = BitVector<64>::tcam_compare_func(v0,v1,v1,v0);
    uint64_t r0111 = BitVector<64>::tcam_compare_func(v0,v1,v1,v1);
    uint64_t r1000 = BitVector<64>::tcam_compare_func(v1,v0,v0,v0);
    uint64_t r1001 = BitVector<64>::tcam_compare_func(v1,v0,v0,v1);
    uint64_t r1010 = BitVector<64>::tcam_compare_func(v1,v0,v1,v0);
    uint64_t r1011 = BitVector<64>::tcam_compare_func(v1,v0,v1,v1);
    uint64_t r1100 = BitVector<64>::tcam_compare_func(v1,v1,v0,v0);
    uint64_t r1101 = BitVector<64>::tcam_compare_func(v1,v1,v0,v1);
    uint64_t r1110 = BitVector<64>::tcam_compare_func(v1,v1,v1,v0);
    uint64_t r1111 = BitVector<64>::tcam_compare_func(v1,v1,v1,v1);

    // According to MAU uArch spec "TCAM truth table"
    // entries 0,4,5,8,10,12,13,14,15 should be 0.
    // These correspond to r0000,r0100,r0101,r1000,r1001,
    // r1100,r1101,r1110 and r1111
    EXPECT_EQ(v0,r0000);
    EXPECT_EQ(v1,r0001);
    EXPECT_EQ(v1,r0010);
    EXPECT_EQ(v1,r0011);
    EXPECT_EQ(v0,r0100);
    EXPECT_EQ(v0,r0101);
    EXPECT_EQ(v1,r0110);
    EXPECT_EQ(v1,r0111);
    EXPECT_EQ(v0,r1000);
    EXPECT_EQ(v1,r1001);
    EXPECT_EQ(v0,r1010);
    EXPECT_EQ(v1,r1011);
    EXPECT_EQ(v0,r1100);
    EXPECT_EQ(v0,r1101);
    EXPECT_EQ(v0,r1110);
    EXPECT_EQ(v0,r1111);
    
    if (tc3_print) {
      RMT_UT_LOG_INFO("r0000=%016" PRIx64 "  0\n", r0000);
      RMT_UT_LOG_INFO("r0001=%016" PRIx64 "  1\n", r0001);
      RMT_UT_LOG_INFO("r0010=%016" PRIx64 "  2\n", r0010);
      RMT_UT_LOG_INFO("r0011=%016" PRIx64 "  3\n", r0011);
      RMT_UT_LOG_INFO("r0100=%016" PRIx64 "  4\n", r0100);
      RMT_UT_LOG_INFO("r0101=%016" PRIx64 "  5\n", r0101);
      RMT_UT_LOG_INFO("r0110=%016" PRIx64 "  6\n", r0110);
      RMT_UT_LOG_INFO("r0111=%016" PRIx64 "  7\n", r0111);
      RMT_UT_LOG_INFO("r1000=%016" PRIx64 "  8\n", r1000);
      RMT_UT_LOG_INFO("r1001=%016" PRIx64 "  9\n", r1001);
      RMT_UT_LOG_INFO("r1010=%016" PRIx64 " 10\n", r1010);
      RMT_UT_LOG_INFO("r1011=%016" PRIx64 " 11\n", r1011);
      RMT_UT_LOG_INFO("r1100=%016" PRIx64 " 12\n", r1100);
      RMT_UT_LOG_INFO("r1101=%016" PRIx64 " 13\n", r1101);
      RMT_UT_LOG_INFO("r1110=%016" PRIx64 " 14\n", r1110);
      RMT_UT_LOG_INFO("r1111=%016" PRIx64 " 15\n", r1111);
    }

    // Repeat but with BitVectors
    BitVector<96> B(v0);
    BitVector<96> bWa0(v0);
    BitVector<96> bWa1(v1);
    BitVector<96> bWb0(v0);
    BitVector<96> bWb1(v1);
    BitVector<96> bSa0(v0);
    BitVector<96> bSa1(v1);
    BitVector<96> bSb0(v0);
    BitVector<96> bSb1(v1);
    
    EXPECT_TRUE (B.tcam_compare(bWa0,bWb0,bSa0,bSb0)); //0
    EXPECT_FALSE(B.tcam_compare(bWa0,bWb0,bSa0,bSb1));
    EXPECT_FALSE(B.tcam_compare(bWa0,bWb0,bSa1,bSb0));
    EXPECT_FALSE(B.tcam_compare(bWa0,bWb0,bSa1,bSb1));
    EXPECT_TRUE (B.tcam_compare(bWa0,bWb1,bSa0,bSb0)); //4
    EXPECT_TRUE (B.tcam_compare(bWa0,bWb1,bSa0,bSb1)); //5
    EXPECT_FALSE(B.tcam_compare(bWa0,bWb1,bSa1,bSb0));
    EXPECT_FALSE(B.tcam_compare(bWa0,bWb1,bSa1,bSb1));
    EXPECT_TRUE (B.tcam_compare(bWa1,bWb0,bSa0,bSb0)); //8
    EXPECT_FALSE(B.tcam_compare(bWa1,bWb0,bSa0,bSb1));
    EXPECT_TRUE (B.tcam_compare(bWa1,bWb0,bSa1,bSb0)); //10
    EXPECT_FALSE(B.tcam_compare(bWa1,bWb0,bSa1,bSb1));
    EXPECT_TRUE (B.tcam_compare(bWa1,bWb1,bSa0,bSb0)); //12
    EXPECT_TRUE (B.tcam_compare(bWa1,bWb1,bSa0,bSb1)); //13
    EXPECT_TRUE (B.tcam_compare(bWa1,bWb1,bSa1,bSb0)); //14
    EXPECT_TRUE (B.tcam_compare(bWa1,bWb1,bSa1,bSb1)); //15             
  }

  TEST(BFN_TEST_NAME(Tcam3Test),TcamLookup) {
    if (tc3_print) RMT_UT_LOG_INFO("test_tcam3_tcam_lookup)\n");
    Tcam3<256,44> tcam3;
    tcam3.reset();
    
    uint64_t      s0 = UINT64_C(0x00000ffffffffffa);
    uint64_t      s1 = UINT64_C(0x5);
    uint64_t      w0 = UINT64_C(0x00000ffffffffffe);
    uint64_t      w1 = UINT64_C(0x00000ffffffff00f);
    uint64_t      cmp = tcam3_compare_func_local(w0, w1, s0, s1);
    BitVector<44> bv_value(UINT64_C(0x0000099999999001));
    BitVector<44> bv_mask(UINT64_C(0x0000000000000ff1));
    BitVector<44> bv_search1(s1);
    BitVector<44> bv_search0(s0);
    BitVector<44> bv_word1(UINT64_C(0));
    BitVector<44> bv_word0(UINT64_C(0));
    BitVector<44> bv_new_word1(w1);
    BitVector<44> bv_new_word0(w0);
    bv_search0.copy_from(bv_search1);
    bv_search0.invert();

    RMT_UT_LOG_INFO("srch0=%016" PRIx64 "\n", s0);
    RMT_UT_LOG_INFO("srch1=%016" PRIx64 "\n", s1);
    RMT_UT_LOG_INFO("word0=%016" PRIx64 "\n", w0);
    RMT_UT_LOG_INFO("word1=%016" PRIx64 "\n", w1);
    RMT_UT_LOG_INFO("cmp  =%016" PRIx64 "\n", cmp);
    
    tcam3.set_word0_word1(22, bv_new_word0, bv_new_word1);
    tcam3.set_value_mask(11, bv_value, bv_mask);
    tcam3.get(11, &bv_word0, &bv_word1);

    RMT_UT_LOG_INFO("bv_word0=      ");
    bv_word0.print();
    RMT_UT_LOG_INFO("bv_word1=      ");
    bv_word1.print();
    RMT_UT_LOG_INFO("bv_search0=    ");
    bv_search0.print();
    RMT_UT_LOG_INFO("bv_search1=    ");
    bv_search1.print();
    RMT_UT_LOG_INFO("bv_new_word0=  ");
    bv_new_word0.print();
    RMT_UT_LOG_INFO("bv_new_word1=  ");
    bv_new_word1.print();
    
    int r1 = tcam3.tcam_lookup(bv_search0,bv_search1);
    int r2 = tcam3.tcam_lookup(bv_search1);
    RMT_UT_LOG_INFO("r1=%d r2=%d\n", r1, r2);
    EXPECT_EQ(22, r1);
    EXPECT_EQ(22, r2);
  }

}
