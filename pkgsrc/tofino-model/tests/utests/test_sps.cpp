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

#include <mau-sbox.h>

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool sps_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;

  // TODO: add tests for Sps14 and Sps18 once those are enabled in mau-sbox.cpp
  TEST(BFN_TEST_NAME(Sps),Sps15) {
    if (sps_print) RMT_UT_LOG_INFO("test_sps_basic()\n");

    // These expected numbers came from the RTL and matched the results from MauSbox
    //   straight away, so they are probably right.
    MauSbox sbox;
    EXPECT_EQ( 0x1436,  sbox.sps15(0x2629) );
    EXPECT_EQ( 0x53b9,  sbox.sps15(0x1f1d) );
    EXPECT_EQ( 0x4f8b,  sbox.sps15(0x338c) );
    EXPECT_EQ( 0x2f3d,  sbox.sps15(0xfb29) );
  }

}




