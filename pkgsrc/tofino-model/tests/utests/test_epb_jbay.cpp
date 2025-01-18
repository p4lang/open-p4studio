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
    // Given an integer channel index, it's hard to get to a particular channel
    // group in reg.h because the channel groups are not held in an array.
    // Instead, use register_map.h and construct the path element for the
    // channel group from the given index.
    std::string chan = "chan" + std::to_string(chanIndex) + "_group";
    std::vector<PathElement> path{
        PathElement{"pipes", std::vector<int>{pipeIndex}},
        PathElement{"pardereg", boost::none},
        PathElement{"pgstnreg", boost::none},
        PathElement{"epbprsr4reg", std::vector<int>{parseBufferIndex}},
        PathElement{"epbreg", boost::none},
        PathElement{chan, boost::none},
        PathElement{counter_name, boost::none},
        PathElement{counter_word, boost::none}
    };
    return tu_->lookup_register_map(path);
  }

  void test_all_counters(int chanIndex) {
    test_counter(
        chanIndex,
        "chnl_parser_send_pkt",
        "chnl_parser_send_pkt_0_3",
        test_epb_->get_epb_counters(chanIndex)->chnl_parser_send_pkt_);
    test_counter(
        chanIndex,
        "chnl_deparser_send_pkt",
        "chnl_deparser_send_pkt_0_2",
        test_epb_->get_epb_counters(chanIndex)->chnl_deparser_send_pkt_);
    test_counter(
        chanIndex,
        "chnl_warp_send_pkt",
        "chnl_warp_send_pkt_0_2",
        test_epb_->get_epb_counters(chanIndex)->chnl_warp_send_pkt_);
    test_counter(
        chanIndex,
        "chnl_p2s_received_pkt",
        "chnl_p2s_received_pkt_0_2",
        test_epb_->get_epb_counters(chanIndex)->chnl_p2s_received_pkt_);
  }
};


TEST_F(BFN_TEST_NAME(EpbCountersTest), EpbCounters) {
  for (int chanIndex = 0; chanIndex < Epb::kChannelsPerEpb; chanIndex++) {
    test_all_counters(chanIndex);
  }
}

TEST_F(BFN_TEST_NAME(EpbCountersTest), EpbCounterIncrements) {
  auto *ctrs = test_epb_->get_epb_counters(0);
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t {
        return ctrs->chnl_parser_send_pkt_.epb_chnl_parser_send_pkt(); },
      [ctrs](uint64_t val) {
        ctrs->chnl_parser_send_pkt_.epb_chnl_parser_send_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_parser_send_pkt(); },
      UINT64_C(0xfffffffff)
  );
#ifdef MODEL_CHIP_CB_OR_LATER
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t {
        return ctrs->chnl_parser_send_err_pkt_.err_pkt(); },
      [ctrs](uint64_t val) {
        ctrs->chnl_parser_send_err_pkt_.err_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_parser_send_pkt_err(); },
      UINT64_C(0xfffffffff)
  );
#else
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t {
        return ctrs->chnl_parser_send_pkt_.err_pkt(); },
      [ctrs](uint64_t val) {
        ctrs->chnl_parser_send_pkt_.err_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_parser_send_pkt_err(); },
      UINT64_C(0xfffffffff)
  );
#endif
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t {
        return ctrs->chnl_deparser_send_pkt_.epb_chnl_deparser_send_pkt(); },
      [ctrs](uint64_t val) {
        ctrs->chnl_deparser_send_pkt_.epb_chnl_deparser_send_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_deparser_send_pkt(); },
      UINT64_C(0xfffffffff)
  );
}

}

