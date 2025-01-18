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

#include "test_ipb_common.h"

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

class BFN_TEST_NAME(IpbCountersTest) : public IpbCountersCommonTest {
 protected:
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
        PathElement{"ipbprsr4reg", std::vector<int>{parseBufferIndex}},
        PathElement{"ipbreg", boost::none},
        PathElement{chan, boost::none},
        PathElement{counter_name, boost::none},
        PathElement{counter_word, boost::none}
    };
    return tu_->lookup_register_map(path);
  }

  void test_all_counters(int chanIndex) override {
    IpbCountersCommonTest::test_all_counters(chanIndex);

    test_counter(chanIndex,
                 "chnl_resubmit_discard_pkt",
                 "chnl_resubmit_discard_pkt_0_2",
                 test_ipb_->get_ipb_counters(chanIndex)->chnl_resubmit_discard_pkt_);
    test_counter(chanIndex,
                 "chnl_resubmit_received_pkt",
                 "chnl_resubmit_received_pkt_0_2",
                 test_ipb_->get_ipb_counters(chanIndex)->chnl_resubmit_received_pkt_);
  }

};


TEST_F(BFN_TEST_NAME(IpbCountersTest), IpbCounters) {
  for (int chanIndex = 0; chanIndex < Ipb::kChannelsPerIpb; chanIndex++) {
    test_all_counters(chanIndex);
  }
}

TEST_F(BFN_TEST_NAME(IpbCountersTest), IpbCounterIncrements) {
  auto *ctrs = test_ipb_->get_ipb_counters(0);
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t { return ctrs->chnl_parser_send_pkt_.chnl_parser_send_pkt(); },
      [ctrs](uint64_t val) { ctrs->chnl_parser_send_pkt_.chnl_parser_send_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_parser_send_pkt(); },
        UINT64_C(0xfffffffff)
  );
#ifdef MODEL_CHIP_CB_OR_LATER
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t { return ctrs->chnl_parser_send_err_pkt_.err_pkt(); },
      [ctrs](uint64_t val) { ctrs->chnl_parser_send_err_pkt_.err_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_parser_send_pkt_err(); },
      UINT64_C(0xfffffffff)
  );
#else
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t { return ctrs->chnl_parser_send_pkt_.err_pkt(); },
      [ctrs](uint64_t val) { ctrs->chnl_parser_send_pkt_.err_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_parser_send_pkt_err(); },
      UINT64_C(0xfffffffff)
  );
#endif
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t { return ctrs->chnl_deparser_send_pkt_.chnl_deparser_send_pkt(); },
      [ctrs](uint64_t val) { ctrs->chnl_deparser_send_pkt_.chnl_deparser_send_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_deparser_send_pkt(); },
      UINT64_C(0xfffffffff)
  );
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t { return ctrs->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt(); },
      [ctrs](uint64_t val) { ctrs->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_deparser_drop_pkt(); },
      UINT64_C(0xfffffffff)
  );
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t { return ctrs->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt(); },
      [ctrs](uint64_t val) { ctrs->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_resubmit_received_pkt(); },
      UINT64_C(0xfffffffff)
  );
  test_wrapping_counter<uint64_t>(
      [ctrs]() -> uint64_t { return ctrs->chnl_macs_received_pkt_.chnl_macs_received_pkt(); },
      [ctrs](uint64_t val) { ctrs->chnl_macs_received_pkt_.chnl_macs_received_pkt(val); },
      [ctrs]() { ctrs->increment_chnl_macs_received_pkt(); },
      UINT64_C(0xfffffffff)
  );
}

// test chnl_resubmit_received_pkt - specific to jbay
TEST_F(BFN_TEST_NAME(IpbCountersTest), TestResubmitCounter) {
  auto call_insert_metadata =
      [this](int channel, bool resubmit, bool meta_enabled = true) {
    test_ipb_->set_meta_enabled(meta_enabled);
    // create a packet
    int base_len = 24;
    uint8_t base_pkt[base_len];
    for (int i = 0; i < base_len; i++) {
      base_pkt[i] = 'A' + i;
    }
    Packet *pkt = tu_->get_objmgr()->pkt_create(base_pkt, sizeof(base_pkt));
    if (resubmit) pkt->mark_for_resubmit();
    test_ipb_->insert_metadata(pkt, channel, resubmit);
    if (meta_enabled) base_len += 32;  // kHeaderSizeBytes prepended
    EXPECT_EQ(pkt->len(), base_len);
    tu_->get_objmgr()->pkt_delete(pkt);
  };

  auto *ipb_counters_0 = test_ipb_->get_ipb_counters(0);
  auto *ipb_counters_3 = test_ipb_->get_ipb_counters(3);
  // sanity checks...
  EXPECT_EQ(0u, ipb_counters_0->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  EXPECT_EQ(0u, ipb_counters_3->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());

  // make calls on varying channel, check counter increments on correct channel
  call_insert_metadata(0, false);
  EXPECT_EQ(0u, ipb_counters_0->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  EXPECT_EQ(0u, ipb_counters_3->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  call_insert_metadata(0, true);  // resubmit chan 0
  EXPECT_EQ(1u, ipb_counters_0->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  EXPECT_EQ(0u, ipb_counters_3->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  call_insert_metadata(0, true);  // resubmit chan 0
  EXPECT_EQ(2u, ipb_counters_0->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  EXPECT_EQ(0u, ipb_counters_3->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  call_insert_metadata(3, true);  // resubmit chan 3
  EXPECT_EQ(2u, ipb_counters_0->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  EXPECT_EQ(1u, ipb_counters_3->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  call_insert_metadata(0, true);  // resubmit chan 0
  EXPECT_EQ(3u, ipb_counters_0->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  EXPECT_EQ(1u, ipb_counters_3->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  call_insert_metadata(0, false);
  EXPECT_EQ(3u, ipb_counters_0->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  EXPECT_EQ(1u, ipb_counters_3->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  call_insert_metadata(3, true, false);  // resubmit chan 3, meta_enabled false
  EXPECT_EQ(3u, ipb_counters_0->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  EXPECT_EQ(1u, ipb_counters_3->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  call_insert_metadata(3, true);  // resubmit chan 3
  EXPECT_EQ(3u, ipb_counters_0->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  EXPECT_EQ(2u, ipb_counters_3->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());

  // set counters to their max value (36 bits for jbay) and check they wrap
  ipb_counters_0->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt(UINT64_C(0xfffffffff));
  ipb_counters_3->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt(UINT64_C(0xfffffffff));
  EXPECT_EQ(UINT64_C(0xfffffffff), ipb_counters_0->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  EXPECT_EQ(UINT64_C(0xfffffffff), ipb_counters_3->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  call_insert_metadata(0, true);  // resubmit chan 3
  EXPECT_EQ(0u, ipb_counters_0->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  EXPECT_EQ(UINT64_C(0xfffffffff), ipb_counters_3->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  call_insert_metadata(3, true);  // resubmit chan 3
  EXPECT_EQ(0u, ipb_counters_0->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
  EXPECT_EQ(0u, ipb_counters_3->chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt());
}

}
