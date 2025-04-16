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
#include <common/rmt-util.h>

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace model_common;

TEST(BFN_TEST_NAME(RmtUtil), TestIncrementAndWrap) {
  // 32 bit wide counter, uint32_t
  EXPECT_EQ(UINT32_C(1), Util::increment_and_wrap(UINT32_C(0), 32));
  EXPECT_EQ(UINT32_C(2000), Util::increment_and_wrap(UINT32_C(1999), 32));
  EXPECT_EQ(UINT32_C(0xffffffff), Util::increment_and_wrap(UINT32_C(0xfffffffe), 32));
  EXPECT_EQ(UINT32_C(0x0), Util::increment_and_wrap(UINT32_C(0xffffffff), 32));
  EXPECT_EQ(UINT32_C(0x1), Util::increment_and_wrap(UINT32_C(0x3), 32, UINT32_C(0xfffffffe)));

  // 1 bit wide counter, uint64_t
  EXPECT_EQ(UINT64_C(1), Util::increment_and_wrap(UINT64_C(0), 1));
  EXPECT_EQ(UINT64_C(0), Util::increment_and_wrap(UINT64_C(1), 1));
  EXPECT_EQ(UINT64_C(1), Util::increment_and_wrap(UINT64_C(0), 1));
  EXPECT_EQ(UINT64_C(1), Util::increment_and_wrap(UINT64_C(1), 1, UINT64_C(2)));

  // 32 bit wide counter, uint64_t
  EXPECT_EQ(UINT64_C(1), Util::increment_and_wrap(UINT64_C(0), 32));
  EXPECT_EQ(UINT64_C(2000), Util::increment_and_wrap(UINT64_C(1999), 32));
  EXPECT_EQ(UINT64_C(0xffffffff), Util::increment_and_wrap(UINT64_C(0xfffffffe), 32));
  EXPECT_EQ(UINT64_C(0x0), Util::increment_and_wrap(UINT64_C(0xffffffff), 32));
  EXPECT_EQ(UINT64_C(0x2), Util::increment_and_wrap(UINT64_C(0x3), 32, UINT64_C(0xffffffff)));
  EXPECT_EQ(UINT64_C(0x3), Util::increment_and_wrap(UINT64_C(0x3), 32, UINT64_C(0x100000000)));

  // 48 bit wide counter, uint64_t
  EXPECT_EQ(UINT64_C(1), Util::increment_and_wrap(UINT64_C(0), 48));
  EXPECT_EQ(UINT64_C(2000), Util::increment_and_wrap(UINT64_C(1999), 48));
  EXPECT_EQ(UINT64_C(0xffffffffffff), Util::increment_and_wrap(UINT64_C(0xfffffffffffe), 64));
  EXPECT_EQ(UINT64_C(0x0), Util::increment_and_wrap(UINT64_C(0xffffffffffff), 48));
  EXPECT_EQ(UINT64_C(0x1), Util::increment_and_wrap(UINT64_C(0x4), 48, UINT64_C(0xfffffffffffd)));
  EXPECT_EQ(UINT64_C(0x1), Util::increment_and_wrap(UINT64_C(0x4), 48, UINT64_C(0x1fffffffffffd)));

  // 64 bit wide counter, uint64_t
  EXPECT_EQ(UINT64_C(1), Util::increment_and_wrap(UINT64_C(0), 64));
  EXPECT_EQ(UINT64_C(2000), Util::increment_and_wrap(UINT64_C(1999), 64));
  EXPECT_EQ(UINT64_C(0xffffffffffffffff), Util::increment_and_wrap(UINT64_C(0xfffffffffffffffe), 64));
  EXPECT_EQ(UINT64_C(0x0), Util::increment_and_wrap(UINT64_C(0xffffffffffffffff), 64));
  EXPECT_EQ(UINT64_C(0x1), Util::increment_and_wrap(UINT64_C(0x4), 64, UINT64_C(0xfffffffffffffffd)));
}

}
