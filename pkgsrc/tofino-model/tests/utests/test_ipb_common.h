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

#ifndef MODEL_TEST_IPB_COMMON_H
#define MODEL_TEST_IPB_COMMON_H

#include "gtest.h"
#include <utests/register_mapper.h>
#include <utests/test_util.h>
#include <register_includes/register_map.h>
#include <rmt-object-manager.h>
#include <ipb.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

// Base class providing some generic counter tests
class CounterTestBase : public BaseTest {
 protected:
  int parseBufferIndex_ = 0;
  int stage_ = 0;
  uint32_t data_ = 0xabba;

  virtual void SetUp() override;

  virtual void TearDown() override;

  virtual void *get_counter_addr(int pipeIndex,
                                 int parseBufferIndex,
                                 int chanIndex,
                                 std::string counter_name,
                                 std::string counter_word) = 0;

  uint32_t test_counter(const int chanIndex,
                        const std::string &counter_name,
                        const std::string &counter_word);

  void test_counter(const int chanIndex,
                    const std::string &counter_name,
                    const std::string &counter_word,
                    model_core::RegisterBlockBase &reg);

  template<typename T>
  void test_wrapping_counter(const std::function<T()> &getter,
                             const std::function<void(T)> &setter,
                             const std::function<void()> &incrementer,
                             const uint64_t max) {
    incrementer();  // increment from zero
    ASSERT_EQ(1u, getter());
    setter(max - 1);
    incrementer();  // increment to max
    ASSERT_EQ(max, getter());
    incrementer();  // wrap to zero
    ASSERT_EQ(0u, getter());
    incrementer();  // increment from zero
    ASSERT_EQ(1u, getter());
  }
};


class IpbCountersCommonTest : public CounterTestBase {
 protected:
  Ipb *test_ipb_;

  virtual void SetUp() override;

  virtual void TearDown() override;

  virtual void test_all_counters(int chanIndex);
};

}

#endif //MODEL_TEST_IPB_COMMON_H
