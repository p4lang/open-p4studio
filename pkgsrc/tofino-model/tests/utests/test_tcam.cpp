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
#include <tcam.h>
#include <rmt-object-manager.h>
#include <packet.h>
#include <phv.h>
#include <parser-static-config.h>


namespace MODEL_CHIP_TEST_NAMESPACE {

  bool tc_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;

  TEST(BFN_TEST_NAME(TcamTest),Basic) {
    if (tc_print) RMT_UT_LOG_INFO("test_tcam_basic()\n");    
    Tcam<256,40> t;
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
    if (tc_print) bv1.print();
    if (tc_print) bv2.print();

    t.reset();
    t.set(99, *bvP);
    t.set(0, bv1);
    if (tc_print) t.print(0);
    t.get(0, &bv2, &bv3);
    if (tc_print) bv1.print();
    if (tc_print) bv2.print();
    if (tc_print) bv3.print();

    // Test lookup
    t.set(124, bv4);
    t.set(224, bv4, bvM);
    t.set(125, bv5);
    t.set(225, bv5, bvM);
    int l0 = t.lookup(bvL4);
    if (tc_print) RMT_UT_LOG_INFO("Found bvL4 at index %d\n", l0);
    ASSERT_EQ(124, l0);
    int l1 = t.lookup(bvL4b);
    ASSERT_EQ(224, l1);
    if (tc_print) RMT_UT_LOG_INFO("Found bvL4b at index %d\n", l1);

    // Test masked lookup
    int l2 = t.lookup(bvL5);
    ASSERT_EQ(125, l2);
    if (tc_print) RMT_UT_LOG_INFO("Found bvL5 at index %d\n", l2);
    int l3 = t.lookup(bvL5b);
    ASSERT_EQ(225, l3);
    if (tc_print) RMT_UT_LOG_INFO("Found bvL5b at index %d\n", l3);
  }

  TEST(BFN_TEST_NAME(TcamTest),SetGet) {
    if (tc_print) RMT_UT_LOG_INFO("test_tcam_set_get()\n");    
    Tcam<256,40> t;
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

    // Test lookup
    t.set(124, bv4);
    t.set(224, bv4, bvM);
    t.set(125, bv5);
    t.set(225, bv5, bvM);
    int l0 = t.lookup(bvL4);
    ASSERT_EQ(124, l0);
    int l1 = t.lookup(bvL4b);
    ASSERT_EQ(224, l1);

    // Test we can get what we set
    t.get(224, &bvG4, &bvGM);
    ASSERT_TRUE(bvG4.equals(bv4));
    ASSERT_TRUE(bv4.equals(bvG4));
    ASSERT_TRUE(bvGM.equals(bvM));
    ASSERT_TRUE(bvM.equals(bvGM));

    t.get(225, &bvG5, &bvGM);
    ASSERT_TRUE(bvG5.equals(bv5));
    ASSERT_TRUE(bv5.equals(bvG5));
    ASSERT_TRUE(bvGM.equals(bvM));
    ASSERT_TRUE(bvM.equals(bvGM));
  }

  TEST(BFN_TEST_NAME(TcamTest),Reset) {
    if (tc_print) RMT_UT_LOG_INFO("test_tcam_reset()\n");    
    Tcam<256,40> t;
    BitVector<40> bv4(std::array<uint8_t,5>({{4,4,4,4,4}}));
    BitVector<40> bv5(std::array<uint8_t,5>({{5,5,5,5,5}}));
    uint8_t M = 0xFFu;
    BitVector<40> bvL4(std::array<uint8_t,5>({{4,4,4,4,4}}));
    BitVector<40> bvL5(std::array<uint8_t,5>({{5,5,5,5,5}}));
    BitVector<40> bvL4b(std::array<uint8_t,5>({{4,4,99,4,4}}));
    BitVector<40> bvL5b(std::array<uint8_t,5>({{5,5,99,5,5}}));
    BitVector<40> bvM(std::array<uint8_t,5>({{M,M,0,M,M}}));

    t.reset();

    // Test lookup
    t.set(124, bv4);
    t.set(224, bv4, bvM);
    t.set(125, bv5);
    t.set(225, bv5, bvM);
    int l0 = t.lookup(bvL4);
    ASSERT_EQ(124, l0);
    int l1 = t.lookup(bvL4b);
    ASSERT_EQ(224, l1);

    // Now reset and try again - lookups should get -1
    t.reset();
    int l2 = t.lookup(bvL4);
    ASSERT_EQ(-1, l2);
    int l3 = t.lookup(bvL4b);
    ASSERT_EQ(-1, l3);
  }

  TEST(BFN_TEST_NAME(TcamTest),StartOffset) {
    if (tc_print) RMT_UT_LOG_INFO("test_tcam_start_offset()\n");    
    Tcam<256,40> t;
    BitVector<40> bv4(std::array<uint8_t,5>({{4,4,4,4,4}}));
    BitVector<40> bv5(std::array<uint8_t,5>({{5,5,5,5,5}}));
    uint8_t M = 0xFFu;
    BitVector<40> bvL4(std::array<uint8_t,5>({{4,4,4,4,4}}));
    BitVector<40> bvL5(std::array<uint8_t,5>({{5,5,5,5,5}}));
    BitVector<40> bvL4b(std::array<uint8_t,5>({{4,4,99,4,4}}));
    BitVector<40> bvL5b(std::array<uint8_t,5>({{5,5,99,5,5}}));
    BitVector<40> bvM(std::array<uint8_t,5>({{M,M,0,M,M}}));

    t.reset();

    // Test lookup
    t.set(124, bv4);
    t.set(224, bv4, bvM);
    t.set(125, bv5);
    t.set(225, bv5, bvM);
    int l0 = t.lookup(bvL4);
    ASSERT_EQ(124, l0);
    int l1 = t.lookup(bvL4b);
    ASSERT_EQ(224, l1);

    // Test masked lookup
    int l2 = t.lookup(bvL5);
    ASSERT_EQ(125, l2);
    int l3 = t.lookup(bvL5b);
    ASSERT_EQ(225, l3);

    // Now set TCAM start point and try again
    int tcam_start_offset = 200;
    t.set_tcam_start(tcam_start_offset);
    int l4 = t.lookup(bvL4);
    ASSERT_EQ(224-tcam_start_offset, l4);
    int l5 = t.lookup(bvL4b);
    ASSERT_EQ(224-tcam_start_offset, l5);
  }


}
