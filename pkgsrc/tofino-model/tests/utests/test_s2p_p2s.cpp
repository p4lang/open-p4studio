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
#include <utests/test_util.h>
#include <s2p.h>
#include <p2s.h>
#include <rmt-object-manager.h>
#include <mau-defs.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

class BFN_TEST_NAME(ConverterTest) : public BaseTest {
 public:
  void check_shadow_counter(
      volatile void *ctr_addr,
      volatile void *ctr_sample_addr,
      volatile void *ctr_time_addr,
      int width,
      const std::function<void(uint64_t)> &increment_func,
      std::string counter_name) {
    SCOPED_TRACE(counter_name);
    FakeRegister ctr(tu_, tu_->reg_ptr_to_addr(ctr_addr), width);
    FakeRegister ctr_sample(tu_, tu_->reg_ptr_to_addr(ctr_sample_addr), 1);
    FakeRegister ctr_time(tu_, tu_->reg_ptr_to_addr(ctr_time_addr), 48);
    uint64_t expected_time = om_->time_get_cycles();

    EXPECT_EQ(UINT64_C(0), ctr.read());  // sanity check
    om_->time_increment(MauDefs::kOneCyclePicosecs);
    ctr_sample.write(UINT64_C(0x1));  // trigger a sample
    EXPECT_EQ(UINT64_C(0), ctr.read());  // sanity check
    EXPECT_EQ(UINT64_C(0), ctr_sample.read());  // check sample reset
    EXPECT_EQ(++expected_time, ctr_time.read());  // check time

    if (nullptr != increment_func) {
      increment_func(UINT64_C(0xabba));
      EXPECT_EQ(UINT64_C(0), ctr.read());
      om_->time_increment(MauDefs::kOneCyclePicosecs);
      ctr_sample.write(UINT64_C(0x1));
      EXPECT_EQ(UINT64_C(0xabba), ctr.read());
      EXPECT_EQ(UINT64_C(0), ctr_sample.read());
      EXPECT_EQ(++expected_time, ctr_time.read());

      uint64_t max = UINT64_C(0xffffffffffffffff) >> (64 - width);
      increment_func(max - UINT64_C(0xabba));
      EXPECT_EQ(UINT64_C(0xabba), ctr.read());
      om_->time_increment(MauDefs::kOneCyclePicosecs);
      ctr_sample.write(UINT64_C(0x1));
      EXPECT_EQ(max, ctr.read());
      EXPECT_EQ(UINT64_C(0), ctr_sample.read());
      EXPECT_EQ(++expected_time, ctr_time.read());

      increment_func(1);
      EXPECT_EQ(max, ctr.read());
      om_->time_increment(MauDefs::kOneCyclePicosecs);
      ctr_sample.write(UINT64_C(0x1));
      EXPECT_EQ(UINT64_C(0), ctr.read());
      EXPECT_EQ(UINT64_C(0), ctr_sample.read());
      EXPECT_EQ(++expected_time, ctr_time.read());

      increment_func(1);
      EXPECT_EQ(UINT64_C(0), ctr.read());
      om_->time_increment(MauDefs::kOneCyclePicosecs);
      ctr_sample.write(UINT64_C(0x1));
      EXPECT_EQ(UINT64_C(1), ctr.read());
      EXPECT_EQ(UINT64_C(0), ctr_sample.read());
      EXPECT_EQ(++expected_time, ctr_time.read());
    }
  }
};


// S2P

class BFN_TEST_NAME(S2p): public BFN_TEST_NAME(ConverterTest) {};

TEST_F(BFN_TEST_NAME(S2p), CheckCounters) {
  for (int pipe_index = 0; pipe_index < tu_->kPipesMax; pipe_index++) {
    SCOPED_TRACE(pipe_index);
    S2p *s2p = om_->s2p_get(pipe_index);
    ASSERT_FALSE(nullptr == s2p);
    for (int chan_index = 0; chan_index < 72; chan_index++) {
      SCOPED_TRACE(chan_index);
      int port_index = (0x80 * pipe_index) + chan_index;
      check_shadow_counter(
          &RegisterUtils::addr_s2p(pipe_index)->pkt_ctr[chan_index],
          &RegisterUtils::addr_s2p(pipe_index)->ctr_sample,
          &RegisterUtils::addr_s2p(pipe_index)->ctr_time,
          36,
          [s2p, port_index](uint64_t amount) {
            s2p->increment_pkt_ctr(port_index, amount);
          },
          "pkt_ctr");
      check_shadow_counter(
          &RegisterUtils::addr_s2p(pipe_index)->byte_ctr[chan_index],
          &RegisterUtils::addr_s2p(pipe_index)->ctr_sample,
          &RegisterUtils::addr_s2p(pipe_index)->ctr_time,
          48,
          [s2p, port_index](uint64_t amount) {
            s2p->increment_byte_ctr(port_index, amount);
          },
          "byte_ctr");
    }
  }
}

TEST_F(BFN_TEST_NAME(S2p), CheckBadPortIndices) {
  ASSERT_EQ(1, GLOBAL_THROW_ON_ASSERT);  // sanity check
  S2p *s2p = om_->s2p_get(0);
  ASSERT_FALSE(nullptr == s2p);
  // Test deliberately provokes RMT_ASSERT failures, so capture them to avoid
  // noise in test logs
  EXPECT_TRUE(nullptr != rmt_stdout_capture()->start());
  EXPECT_TRUE(nullptr != rmt_stderr_capture()->start());
  EXPECT_NO_THROW(s2p->increment_pkt_ctr(1));  // sanity
  EXPECT_THROW(s2p->increment_pkt_ctr(-1), std::runtime_error);
  EXPECT_THROW(s2p->increment_pkt_ctr(72), std::runtime_error);
  EXPECT_THROW(s2p->increment_byte_ctr(-1), std::runtime_error);
  EXPECT_THROW(s2p->increment_byte_ctr(72), std::runtime_error);
  rmt_stdout_capture()->stop();
  rmt_stderr_capture()->stop();
  // check all lines start with "ERROR ASSERT:"
  EXPECT_EQ(4, rmt_stdout_capture()->for_each_line_starts_with(
      "ERROR ASSERT:")) << rmt_stdout_capture()->dump_lines(200);
  EXPECT_EQ(4, rmt_stderr_capture()->for_each_line_starts_with(
      "ERROR ASSERT:")) << rmt_stderr_capture()->dump_lines(200);
}

TEST_F(BFN_TEST_NAME(S2p), CheckLookup) {
  // check following:
  //   * get should always return an object
  //   * lookup after get should always return the *same* object
  //   * lookup for different pipe should return different object
  //   * after reset, lookups should return nullptr
  S2p *s2p = om_->s2p_get(0);
  ASSERT_FALSE(nullptr == s2p);
  S2p *s2p2 = om_->s2p_lookup(0);
  ASSERT_FALSE(nullptr == s2p2);
  ASSERT_TRUE(s2p == s2p2);
  S2p *s2p3 = om_->s2p_lookup(1);
  ASSERT_FALSE(nullptr == s2p3);
  ASSERT_FALSE(s2p == s2p3);
  GLOBAL_MODEL->Reset();
  S2p *s2p4 = om_->s2p_lookup(0);
  ASSERT_TRUE(nullptr == s2p4);
  S2p *s2p5 = om_->s2p_lookup(1);
  ASSERT_TRUE(nullptr == s2p5);
}


// P2S

class BFN_TEST_NAME(P2s): public BFN_TEST_NAME(ConverterTest) { };

TEST_F(BFN_TEST_NAME(P2s), CheckCounters) {
  for (int pipe_index = 0; pipe_index < tu_->kPipesMax; pipe_index++) {
    SCOPED_TRACE(pipe_index);
    P2s *p2s = om_->p2s_get(pipe_index);
    ASSERT_FALSE(nullptr == p2s);
    for (int chan_index = 0; chan_index < 72; chan_index++) {
      SCOPED_TRACE(chan_index);
      int port_index = (0x80 * pipe_index) + chan_index;
      check_shadow_counter(
          &RegisterUtils::addr_p2s(pipe_index)->pkt_ctr[chan_index],
          &RegisterUtils::addr_p2s(pipe_index)->ctr_sample,
          &RegisterUtils::addr_p2s(pipe_index)->ctr_time,
          36,
          [p2s, port_index](uint64_t amount) {
            p2s->increment_pkt_ctr(port_index, amount);
          },
          "pkt_ctr");
      check_shadow_counter(
          &RegisterUtils::addr_p2s(pipe_index)->byte_ctr[chan_index],
          &RegisterUtils::addr_p2s(pipe_index)->ctr_sample,
          &RegisterUtils::addr_p2s(pipe_index)->ctr_time,
          48,
          [p2s, port_index](uint64_t amount) {
            p2s->increment_byte_ctr(port_index, amount);
          },
          "byte_ctr");
    }
  }
}

TEST_F(BFN_TEST_NAME(P2s), CheckBadPortIndices) {
  ASSERT_EQ(1, GLOBAL_THROW_ON_ASSERT);  // sanity check
  P2s *p2s = om_->p2s_get(0);
  ASSERT_FALSE(nullptr == p2s);
  // Test deliberately provokes RMT_ASSERT failures, so capture them to avoid
  // noise in test logs
  EXPECT_TRUE(nullptr != rmt_stdout_capture()->start());
  EXPECT_TRUE(nullptr != rmt_stderr_capture()->start());
  EXPECT_NO_THROW(p2s->increment_pkt_ctr(1));  // sanity
  EXPECT_THROW(p2s->increment_pkt_ctr(-1), std::runtime_error);
  EXPECT_THROW(p2s->increment_pkt_ctr(72), std::runtime_error);
  EXPECT_THROW(p2s->increment_byte_ctr(-1), std::runtime_error);
  EXPECT_THROW(p2s->increment_byte_ctr(72), std::runtime_error);
  rmt_stdout_capture()->stop();
  rmt_stderr_capture()->stop();
  // check all lines start with "ERROR ASSERT:"
  EXPECT_EQ(4, rmt_stdout_capture()->for_each_line_starts_with(
      "ERROR ASSERT:")) << rmt_stdout_capture()->dump_lines(200);
  EXPECT_EQ(4, rmt_stderr_capture()->for_each_line_starts_with(
      "ERROR ASSERT:")) << rmt_stderr_capture()->dump_lines(200);
}

TEST_F(BFN_TEST_NAME(P2s), CheckLookup) {
  // check following:
  //   * get should always return an object
  //   * lookup after get should always return the *same* object
  //   * lookup for different pipe should return different object
  //   * after reset, lookups should return nullptr
  P2s *p2s = om_->p2s_get(0);
  ASSERT_FALSE(nullptr == p2s);
  P2s *p2s2 = om_->p2s_lookup(0);
  ASSERT_FALSE(nullptr == p2s2);
  ASSERT_TRUE(p2s == p2s2);
  P2s *p2s3 = om_->p2s_lookup(1);
  ASSERT_FALSE(nullptr == p2s3);
  ASSERT_FALSE(p2s == p2s3);
  GLOBAL_MODEL->Reset();
  P2s *p2s4 = om_->p2s_lookup(0);
  ASSERT_TRUE(nullptr == p2s4);
  P2s *p2s5 = om_->p2s_lookup(1);
  ASSERT_TRUE(nullptr == p2s5);
}


}
