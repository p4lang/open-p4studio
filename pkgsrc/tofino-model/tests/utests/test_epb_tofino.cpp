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

#include <epb.h>
#include "test_ipb_common.h"

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

class BFN_TEST_NAME(EpbCountersTest) : public CounterTestBase {
 protected:
  Epb *test_epb_;
  void SetUp() override {
    CounterTestBase::SetUp();
    //instantiate an Epb to access its counters member
    test_epb_ = new Epb(tu_->get_objmgr(), pipe_index(), parseBufferIndex_);
  }

  void TearDown() override {
    delete test_epb_;
    CounterTestBase::TearDown();
  }

  void *get_counter_addr(int pipeIndex,
                         int parseBufferIndex,
                         int chanIndex,
                         std::string counter_name,
                         std::string counter_word) override {
    std::vector<PathElement> path{
        PathElement{"pipes", std::vector<int>{pipeIndex}},
        PathElement{"pmarb", boost::none},
        PathElement{"ebp18_reg", boost::none},
        PathElement{"egrNx_reg", std::vector<int>{parseBufferIndex}},
        PathElement{"epb_disp_port_regs", boost::none},
        PathElement{counter_name, std::vector<int>{chanIndex}},
        PathElement{counter_word, boost::none}
    };
    return tu_->lookup_register_map(path);
  }

};


TEST_F(BFN_TEST_NAME(EpbCountersTest), EpbCounters) {
  for (int chanIndex = 0; chanIndex < Epb::kChannelsPerEpb; chanIndex++) {
    uint32_t expected = test_counter(chanIndex,
                                     "egr_pipe_count",
                                     "egr_pipe_count_0_2");
    EXPECT_EQ(
        expected,
        test_epb_->get_epb_counters(chanIndex)->get_egr_pipe_count()) << "egr_pipe_count";
    expected = test_counter(chanIndex,
                            "egr_bypass_count",
                            "egr_bypass_count_0_2");
    EXPECT_EQ(
        expected,
        test_epb_->get_epb_counters(chanIndex)->get_egr_bypass_count()) << "egr_bypass_count";
  }
}

TEST_F(BFN_TEST_NAME(EpbCountersTest), EpbCounterIncrements) {
  auto *ctrs = test_epb_->get_epb_counters(0);
  ASSERT_FALSE(nullptr == ctrs);
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t { return ctrs->get_egr_pipe_count(); },
      [ctrs](uint64_t val) { ctrs->set_egr_pipe_count(val); },
      [ctrs]() { ctrs->increment_egr_pipe_count(); },
      UINT64_C(0xffffffffffffffff)
  );
}

}

