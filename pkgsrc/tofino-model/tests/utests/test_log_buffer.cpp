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
#include <model_core/log-buffer.h>

namespace rmt_utests {

TEST(TestLogBuffer, Append) {
  model_core::LogBuffer buf(24);
  EXPECT_STREQ("", buf.GetBuf());
  bool actual = buf.Append("trains,");
  EXPECT_TRUE(actual);
  EXPECT_STREQ("trains,", buf.GetBuf());
  actual = buf.Append(" planes ");
  EXPECT_TRUE(actual);
  EXPECT_STREQ("trains, planes ", buf.GetBuf());
  actual = buf.Append("and automobiles");
  EXPECT_FALSE(actual);
  EXPECT_STREQ("trains, planes and autom", buf.GetBuf());
  int original = GLOBAL_THROW_ON_ASSERT;
  GLOBAL_THROW_ON_ASSERT = 1;
  ASSERT_THROW(buf.Append("out of bounds"), std::runtime_error);
  GLOBAL_THROW_ON_ASSERT = original;

  model_core::LogBuffer zero_buf(-2);
  EXPECT_STREQ("", zero_buf.GetBuf());
  GLOBAL_THROW_ON_ASSERT = 1;
  ASSERT_THROW(buf.Append("out of bounds"), std::runtime_error);
  GLOBAL_THROW_ON_ASSERT = original;
}

TEST(TestLogBuffer, ConstructWithFormat) {
  model_core::LogBuffer buf(24, "%s %d", "Apollo", 1);
  EXPECT_STREQ("Apollo 1", buf.GetBuf());
  bool actual = buf.Append("%d", 3);
  EXPECT_TRUE(actual);
  EXPECT_STREQ("Apollo 13", buf.GetBuf());

  // check truncation
  model_core::LogBuffer buf2(5, "%s %d", "Apollo", 13);
  EXPECT_STREQ("Apoll", buf2.GetBuf());
}
}
