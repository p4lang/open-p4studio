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
#include "port.h"

namespace MODEL_CHIP_TEST_NAMESPACE {

void CounterTestBase::SetUp() {
  set_pipe_index(0);
  BaseTest::SetUp();
  //  tu_->update_log_flags(ALL, ALL, ALL, ALL, ALL, ALL, ALL);
}

void CounterTestBase::TearDown() {
  tu_->quieten_log_flags();
  BaseTest::TearDown();
}

uint32_t CounterTestBase::test_counter(const int chanIndex,
                                       const std::string &counter_name,
                                       const std::string &counter_word) {
  void *ctr_addr = get_counter_addr(pipe_index(),
                                    parseBufferIndex_,
                                    chanIndex,
                                    counter_name,
                                    counter_word);
  // check that counter exists
  uint32_t val = tu_->InWord(ctr_addr);
  EXPECT_EQ(0u, val) << counter_name;
  // write to counter and read back
  tu_->OutWord(ctr_addr, data_);
  val = tu_->InWord(ctr_addr);
  EXPECT_EQ(data_, val) << counter_name;
  uint32_t expected = data_;
  data_ += 0x101;
  return expected;
}

void CounterTestBase::test_counter(const int chanIndex,
                                   const std::string &counter_name,
                                   const std::string &counter_word,
                                   model_core::RegisterBlockBase &reg) {
  uint32_t expected = test_counter(chanIndex, counter_name, counter_word);
  // check counter reference is correct
  uint32_t actual;
  reg.read(0, &actual);
  EXPECT_EQ(expected, actual) << counter_name;
}


void IpbCountersCommonTest::SetUp() {
  CounterTestBase::SetUp();
  //instantiate an Ipb to access its counters member
  test_ipb_ = new Ipb(tu_->get_objmgr(), pipe_index(), parseBufferIndex_);
}

void IpbCountersCommonTest::TearDown() {
  delete test_ipb_;
  CounterTestBase::TearDown();
}

void IpbCountersCommonTest::test_all_counters(int chanIndex) {
  test_counter(chanIndex,
               "chnl_wsch_discard_pkt",
               "chnl_wsch_discard_pkt_0_2",
               test_ipb_->get_ipb_counters(chanIndex)->chnl_wsch_discard_pkt_);

  test_counter(chanIndex,
               "chnl_wsch_trunc_pkt",
               "chnl_wsch_trunc_pkt_0_2",
               test_ipb_->get_ipb_counters(chanIndex)->chnl_wsch_trunc_pkt_);

  test_counter(chanIndex,
               "chnl_parser_discard_pkt",
               "chnl_parser_discard_pkt_0_2",
               test_ipb_->get_ipb_counters(chanIndex)->chnl_parser_discard_pkt_);

  test_counter(chanIndex,
               "chnl_parser_send_pkt",
               "chnl_parser_send_pkt_0_3",
               test_ipb_->get_ipb_counters(chanIndex)->chnl_parser_send_pkt_);

  test_counter(chanIndex,
               "chnl_deparser_send_pkt",
               "chnl_deparser_send_pkt_0_2",
               test_ipb_->get_ipb_counters(chanIndex)->chnl_deparser_send_pkt_);

  test_counter(chanIndex,
               "chnl_deparser_drop_pkt",
               "chnl_deparser_drop_pkt_0_2",
               test_ipb_->get_ipb_counters(chanIndex)->chnl_deparser_drop_pkt_);

  test_counter(chanIndex,
               "chnl_macs_received_pkt",
               "chnl_macs_received_pkt_0_3",
               test_ipb_->get_ipb_counters(chanIndex)->chnl_macs_received_pkt_);
}

TEST(BFN_TEST_NAME(Ipb),IpbCountersInc) {
  bool jbay = RmtObject::is_jbay_or_later();
  int chip = 202; // 2 stages
  TestUtil tu(GLOBAL_MODEL.get(), chip, 0, 0);
  uint64_t pipes, stages, types, rows_tabs, cols, flags;
  pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = FEW;
  tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

  RmtObjectManager *om = tu.get_objmgr();
  ASSERT_TRUE(nullptr != om);
  tu.set_stage_default_regs(0);
  tu.set_stage_default_regs(1);
  Port *port = tu.port_get(16);
  ASSERT_TRUE(nullptr != port);

  const char *pktstr =
      "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A44556611881199"
      "0000000000000000000000000000000000000000000000000000";
  Packet *pkt1 = om->pkt_create(pktstr);
  Packet *pkt2 = om->pkt_create(pktstr);
  ASSERT_TRUE(nullptr != pkt1);
  ASSERT_TRUE(nullptr != pkt2);

  Ipb *ipb = om->ipb_lookup(port->pipe_index(), port->ipb_index());
  ASSERT_TRUE(nullptr != ipb);
  auto *ipb_counters = ipb->get_ipb_counters(port->ipb_chan());
  ASSERT_TRUE(nullptr != ipb_counters);
  // sanity checks...
  EXPECT_EQ(0u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
  EXPECT_EQ(0u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
  EXPECT_EQ(0u, ipb_counters->chnl_macs_received_pkt_.chnl_macs_received_pkt());
  EXPECT_EQ(0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());

  // check counters increment when chan is enabled
  ipb->set_rx_enabled(true);
  (void)tu.port_process_inbound(port, pkt1);
  EXPECT_EQ(1u, ipb_counters->chnl_macs_received_pkt_.chnl_macs_received_pkt());
  EXPECT_EQ(1u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
  EXPECT_EQ(1u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
  // for Tofino tu.port_get() sets up deparser with a default egress port, so
  // packet is *not* dropped, but there is no default egress port for Jbay so
  // packet *is* dropped.
  EXPECT_EQ(jbay ? 1u : 0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());
  if (om->pkt_last_deleted() != pkt1) {
    om->pkt_delete(pkt1); // Free pkt ourself
  }

  // check counters do not increment when chan is disabled
  ipb->set_rx_enabled(false);
  (void)tu.port_process_inbound(port, pkt2);
  EXPECT_EQ(1u, ipb_counters->chnl_macs_received_pkt_.chnl_macs_received_pkt());
  EXPECT_EQ(1u, ipb_counters->chnl_parser_send_pkt_.chnl_parser_send_pkt());
  EXPECT_EQ(1u, ipb_counters->chnl_deparser_send_pkt_.chnl_deparser_send_pkt());
  EXPECT_EQ(jbay ? 1u : 0u, ipb_counters->chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt());
  if (om->pkt_last_deleted() != pkt2) {
    om->pkt_delete(pkt2); // Free pkt ourself
  }

  tu.quieten_log_flags();
}

}
