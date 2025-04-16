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

#ifndef _TEST_PARSER_ARBITER_
#define _TEST_PARSER_ARBITER_

#include <utests/test_util.h>
#include <parser-arbiter.h>
#include <port.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

/**
 * Provides generic test method for chip-specific subclasses
 */
class BFN_TEST_NAME(ParserArbiter) : public BaseTest {
 public:
  void SetUp() {
    set_pipe_index(0);
    BaseTest::SetUp();
  }

  template<typename REG>
  void check_egress_counters(REG *reg_addr) {
    ParserArbiter *parb = om_->parser_arbiter_lookup(pipe_index());
    // check that ParserArbiter has been instantiated for the pipe
    ASSERT_TRUE(nullptr != parb);

    tu_->check_counter(
        &reg_addr->e_eop_count,
        64,
        [parb]() { parb->increment_e_eop_count(1); },
        "e_eop_count");
    tu_->check_counter(
        &reg_addr->e_phv_count,
        64,
        [parb]() { parb->increment_e_phv_count(); },
        "e_phv_count");
  }

  template<typename REG>
  void check_ingress_counters(REG *reg_addr) {
    ParserArbiter *parb = om_->parser_arbiter_lookup(pipe_index());
    // check that ParserArbiter has been instantiated for the pipe
    ASSERT_TRUE(nullptr != parb);

    tu_->check_counter(
        &reg_addr->i_eop_count,
        64,
        [parb]() { parb->increment_i_eop_count(1); },
        "i_eop_count");
    tu_->check_counter(
        &reg_addr->i_norm_eop_count,
        64,
        [parb]() { parb->increment_i_norm_eop_count(1); },
        "i_norm_eop_count");
    tu_->check_counter(
        &reg_addr->i_norm_phv_count,
        64,
        [parb]() { parb->increment_i_norm_phv_count(); },
        "i_norm_phv_count");
    tu_->check_counter(
        &reg_addr->i_phv_count,
        64,
        [parb]() { parb->increment_i_phv_count(); },
        "i_phv_count");
    tu_->check_counter(
        &reg_addr->i_resub_eop_count,
        64,
        [parb]() { parb->increment_i_resub_eop_count(1); },
        "i_resub_eop_count");
    tu_->check_counter(
        &reg_addr->i_resub_phv_count,
        64,
        [parb]() { parb->increment_i_resub_phv_count(); },
        "i_resub_phv_count");
  }

  template<typename I_REG, typename E_REG>
  void test_increment(I_REG *i_reg_addr, E_REG *e_reg_addr) {
    tu_->set_stage_default_regs(0);
    tu_->set_stage_default_regs(1);
    Port *port = tu_->port_get(16);
    ASSERT_TRUE(nullptr != port);
    ASSERT_EQ(pipe_index(), port->pipe_index());

    // create some 64 byte packets
    const char *pktstr =
        "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A44556611881199"
        "0000000000000000000000000000000000000000000000000000";
    Packet *pkt1 = om_->pkt_create(pktstr);
    Packet *pkt2 = om_->pkt_create(pktstr);
    Packet *pkt3 = om_->pkt_create(pktstr);
    ASSERT_TRUE(nullptr != pkt1);
    ASSERT_TRUE(nullptr != pkt2);
    ASSERT_TRUE(nullptr != pkt3);

    ParserArbiter *parb = om_->parser_arbiter_get(port->pipe_index());
    ASSERT_TRUE(nullptr != parb);
    FakeRegister i_phv_count = FakeRegister(
        tu_, tu_->reg_ptr_to_addr(&i_reg_addr->i_phv_count), 64);
    FakeRegister i_norm_phv_count = FakeRegister(
        tu_, tu_->reg_ptr_to_addr(&i_reg_addr->i_norm_phv_count), 64);
    FakeRegister i_resub_phv_count = FakeRegister(
        tu_, tu_->reg_ptr_to_addr(&i_reg_addr->i_resub_phv_count), 64);
    FakeRegister e_phv_count = FakeRegister(
        tu_, tu_->reg_ptr_to_addr(&e_reg_addr->e_phv_count), 64);
    FakeRegister i_eop_count = FakeRegister(
        tu_, tu_->reg_ptr_to_addr(&i_reg_addr->i_eop_count), 64);
    FakeRegister i_norm_eop_count = FakeRegister(
        tu_, tu_->reg_ptr_to_addr(&i_reg_addr->i_norm_eop_count), 64);
    FakeRegister i_resub_eop_count = FakeRegister(
        tu_, tu_->reg_ptr_to_addr(&i_reg_addr->i_resub_eop_count), 64);
    FakeRegister e_eop_count = FakeRegister(
        tu_, tu_->reg_ptr_to_addr(&e_reg_addr->e_eop_count), 64);
    // sanity checks...
    EXPECT_EQ(UINT64_C(0), i_phv_count.read());
    EXPECT_EQ(UINT64_C(0), i_norm_phv_count.read());
    EXPECT_EQ(UINT64_C(0), i_resub_phv_count.read());
    EXPECT_EQ(UINT64_C(0), e_phv_count.read());
    EXPECT_EQ(UINT64_C(0), i_eop_count.read());
    EXPECT_EQ(UINT64_C(0), i_norm_eop_count.read());
    EXPECT_EQ(UINT64_C(0), i_resub_eop_count.read());
    EXPECT_EQ(UINT64_C(0), e_eop_count.read());

    EXPECT_TRUE(port->parser()->ingress()->enabled(port->parser_chan()));
    port->ipb()->set_rx_enabled(true);
    // process an ingress packet
    (void)tu_->port_process_inbound(port, pkt1);
    EXPECT_EQ(UINT64_C(1), i_phv_count.read());
    EXPECT_EQ(UINT64_C(1), i_norm_phv_count.read());
    EXPECT_EQ(UINT64_C(0), i_resub_phv_count.read());
    EXPECT_EQ(UINT64_C(0), e_phv_count.read());
    EXPECT_EQ(UINT64_C(64), i_eop_count.read());
    EXPECT_EQ(UINT64_C(64), i_norm_eop_count.read());
    EXPECT_EQ(UINT64_C(0), i_resub_eop_count.read());
    EXPECT_EQ(UINT64_C(0), e_eop_count.read());
    if (om_->pkt_last_deleted() != pkt1) om_->pkt_delete(pkt1);

    // process an ingress resubmit packet
    pkt2->mark_for_resubmit();
    (void)tu_->port_process_inbound(port, pkt2);
    EXPECT_EQ(UINT64_C(2), i_phv_count.read());
    EXPECT_EQ(UINT64_C(1), i_norm_phv_count.read());
    EXPECT_EQ(UINT64_C(1), i_resub_phv_count.read());
    EXPECT_EQ(UINT64_C(0), e_phv_count.read());
    EXPECT_EQ(UINT64_C(128), i_eop_count.read());
    EXPECT_EQ(UINT64_C(64), i_norm_eop_count.read());
    EXPECT_EQ(UINT64_C(64), i_resub_eop_count.read());
    EXPECT_EQ(UINT64_C(0), e_eop_count.read());
    if (om_->pkt_last_deleted() != pkt2) om_->pkt_delete(pkt2);

    // process an egress packet
    // explicitly enable egress parser chan (required for Tofino)
    port->parser()->egress()->set_channel(port->parser_chan(), true, 0);
    // arrange for some metadata to be prepended to the packet by epb
    port->epb()->set_ctrl_flags(port->epb_chan(), EpbCommon::kFlagIngressQueueTimestamp);
    EXPECT_TRUE(port->parser()->egress()->enabled(port->parser_chan()));
    // arrange for some metadata to be prepended to the packet by epb
    port->epb()->set_chan_enabled(port->epb_chan());
    port->epb()->set_ctrl_flags(
        port->epb_chan(),
        EpbCommon::kFlagIngressQueueTimestamp | EpbCommon::kFlagLength);
    // set hdr_len_adj to match metadata len:
    //     2 (egress port) + 4 (ingress queue timestamp) + 2 (length) = 8
    port->parser()->egress()->set_hdr_len_adj(8);
    // XXX: Disable writing errors on egress (err dst PHV word is ingress-owned)
    port->parser()->egress()->set_perr_phv_cfg(0, 0u);

    int orig_pkt3_len = pkt3->len();
    // N.B. egress eop pktlen is taken from qing2e_metadata
    pkt3->qing2e_metadata()->set_len(orig_pkt3_len);
    pkt3->qing2e_metadata()->set_ing_q_ts(0xabcd1234);
    (void)tu_->port_process_outbound(port, pkt3);
    EXPECT_EQ(UINT64_C(2), i_phv_count.read());
    EXPECT_EQ(UINT64_C(1), i_norm_phv_count.read());
    EXPECT_EQ(UINT64_C(1), i_resub_phv_count.read());
    EXPECT_EQ(UINT64_C(1), e_phv_count.read());
    EXPECT_EQ(UINT64_C(128), i_eop_count.read());
    EXPECT_EQ(UINT64_C(64), i_norm_eop_count.read());
    EXPECT_EQ(UINT64_C(64), i_resub_eop_count.read());
    EXPECT_EQ(UINT64_C(64), e_eop_count.read());
    ASSERT_TRUE(nullptr != pkt3);
    // verify that metadata prepended to packet by epb has been trimmed off
    EXPECT_EQ(orig_pkt3_len, pkt3->len());
    if (om_->pkt_last_deleted() != pkt3) om_->pkt_delete(pkt3);
}

};

TEST_F(BFN_TEST_NAME(ParserArbiter), CheckLookup) {
  // check following:
  //   * get should always return an object
  //   * lookup after get should always return the *same* object
  //   * lookup for different pipe should return different object
  //   * after reset, lookups should return nullptr
  ParserArbiter *parser_arbiter = om_->parser_arbiter_get(0);
  ASSERT_FALSE(nullptr == parser_arbiter);
  ParserArbiter *parser_arbiter2 = om_->parser_arbiter_lookup(0);
  ASSERT_FALSE(nullptr == parser_arbiter2);
  ASSERT_TRUE(parser_arbiter == parser_arbiter2);
  ParserArbiter *parser_arbiter3 = om_->parser_arbiter_lookup(1);
  ASSERT_FALSE(nullptr == parser_arbiter3);
  ASSERT_FALSE(parser_arbiter == parser_arbiter3);
  GLOBAL_MODEL->Reset();
  ParserArbiter *parser_arbiter4 = om_->parser_arbiter_lookup(0);
  ASSERT_TRUE(nullptr == parser_arbiter4);
  ParserArbiter *parser_arbiter5 = om_->parser_arbiter_lookup(1);
  ASSERT_TRUE(nullptr == parser_arbiter5);
}

}
#endif // _TEST_PARSER_ARBITER_
