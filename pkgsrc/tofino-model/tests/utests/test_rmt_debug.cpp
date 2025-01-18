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

#include "gtest.h"
#include <utests/test_namespace.h>
#include <rmt-debug.h>

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

TEST(BFN_TEST_NAME(TestRmtDebug),FlagsForString) {
  EXPECT_EQ(UINT64_C(0), RmtDebug::flags_for_string(""));
  EXPECT_EQ(UINT64_C(0), RmtDebug::flags_for_string("bogus"));
  EXPECT_EQ(UINT64_C(0), RmtDebug::flags_for_string("MauSelector"));

  EXPECT_EQ(UINT64_C(1), RmtDebug::flags_for_string("Fatal"));
  EXPECT_EQ(UINT64_C(1), RmtDebug::flags_for_string("fatal"));
  EXPECT_EQ(UINT64_C(1), RmtDebug::flags_for_string("faTal*"));
  EXPECT_EQ(UINT64_C(1), RmtDebug::flags_for_string("FATA*"));
  EXPECT_EQ(UINT64_C(1), RmtDebug::flags_for_string("kRmtDebugFata*"));

  EXPECT_EQ(UINT64_C(0x000000c000000000), RmtDebug::flags_for_string("MauSelector*"));

  EXPECT_EQ(UINT64_C(0x80007fff8000ffff), RmtDebug::flags_for_string("*"));
  EXPECT_EQ(UINT64_C(0x80007fff8000ffff), RmtDebug::flags_for_string("kRmtDebug*"));
}

TEST(BFN_TEST_NAME(TestRmtDebug),Strings) {
  std::map<std::string, uint64_t> expected0 {};
  EXPECT_EQ(expected0, RmtDebug::flags_map("MauSelector"));
  EXPECT_EQ(expected0, RmtDebug::flags_map("mauSelector"));
  EXPECT_EQ(expected0, RmtDebug::flags_map(""));

  std::map<std::string, uint64_t> expected1 {
    {"MauSelectorAlu", UINT64_C(0x0000008000000000)},
  };
  EXPECT_EQ(expected1, RmtDebug::flags_map("MauSelectorAlu"));
  EXPECT_EQ(expected1, RmtDebug::flags_map("MauSelectorAlu*"));
  EXPECT_EQ(expected1, RmtDebug::flags_map("mauselectorAlu"));

  std::map<std::string, uint64_t> expected2 {
    {"MauSelectorMatchCentral", UINT64_C(0x0000004000000000)},
    {"MauSelectorAlu", UINT64_C(0x0000008000000000)},
  };
  EXPECT_EQ(expected2, RmtDebug::flags_map("MauSelector*"));
  EXPECT_EQ(expected2, RmtDebug::flags_map("kRmtDebugMauSelector*"));
  EXPECT_EQ(expected2, RmtDebug::flags_map("MAUSELECTOR*"));
  EXPECT_EQ(expected2, RmtDebug::flags_map("krmtdebugMauSelector*"));

  EXPECT_EQ(158u, RmtDebug::flags_map().size());
  EXPECT_EQ(158u, RmtDebug::flags_map("*").size());
  EXPECT_EQ(158u, RmtDebug::flags_map("kRmtDebug*").size());
}

TEST(BFN_TEST_NAME(TestRmtDebug),LogTypeForString) {
  EXPECT_EQ(-1, RmtDebug::log_type_for_string(""));
  EXPECT_EQ(-1, RmtDebug::log_type_for_string("bogus"));

  EXPECT_EQ(0, RmtDebug::log_type_for_string("model"));
  EXPECT_EQ(0, RmtDebug::log_type_for_string("Model"));

  EXPECT_EQ(1, RmtDebug::log_type_for_string("p4"));
  EXPECT_EQ(1, RmtDebug::log_type_for_string("P4"));

  EXPECT_EQ(3, RmtDebug::log_type_for_string("packet"));
  EXPECT_EQ(3, RmtDebug::log_type_for_string("PaCkeT"));
}

TEST(BFN_TEST_NAME(TestRmtDebug),LogStringForType) {
  EXPECT_EQ("model", RmtDebug::log_string_for_type(0));
  EXPECT_EQ("p4", RmtDebug::log_string_for_type(1));
  EXPECT_EQ("tofino", RmtDebug::log_string_for_type(2));
  EXPECT_EQ("packet", RmtDebug::log_string_for_type(3));
  EXPECT_EQ("unknown", RmtDebug::log_string_for_type(4));
  EXPECT_EQ("unknown", RmtDebug::log_string_for_type(-1));
}

}
