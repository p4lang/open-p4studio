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

#include <string>
#include "gtest.h"
#include <utests/test_namespace.h>
#include <utests/test_util.h>
#include <rmt-log.h>
#include <packet.h>

namespace MODEL_CHIP_NAMESPACE {
  std::string rmt_get_pkt_id_sig(Packet* pkt, bool use_sig);
}

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

class TestLogger : public RmtLogger {
 public:
  TestLogger(RmtObjectManager* om, int type_index, int pipe_index, int stage_index) :
      RmtLogger(om, type_index),
      pipe_index_(pipe_index),
      stage_index_(stage_index)
       { };
  int pipe_index() const override { return pipe_index_; }
  int s_index() const override { return stage_index_; }
  int rt_index() const override { return 3; }
  int c_index() const override { return 4; }
  // access protected RmtLogger method
  std::list<std::string> do_rmt_log_p4_salu_pretty_print(
      int table_index,
      std::string action_name,
      std::list<std::pair<int, uint32_t>>& salu_log_value_list) {
    return rmt_log_p4_salu_pretty_print(table_index,
                                        action_name,
                                        salu_log_value_list);
  }
 private:
  int pipe_index_;
  int stage_index_;
};

class BFN_TEST_NAME(TestRmtLog) : public BaseTest { };

TEST_F(BFN_TEST_NAME(TestRmtLog), ModelPrefix) {
  auto do_test = [this](int log_type) {
    int log_index_depth = 2;
    TestLogger* logger = new TestLogger(om_, log_index_depth, 1, 2);
    logger->set_log_type_flags(UINT64_C(0xffffffffffffffff), log_type);
    auto* log_capture = rmt_logger_capture();
    log_capture->clear();
    log_capture->start();
    RMT_TYPE_LOG_OBJ(logger, log_type, 1, "%s", "bonjour");
    log_capture->stop();
    delete logger;
  };

  auto* log_capture = rmt_logger_capture();
  int log_type = 0;  // 0 == model type logging
  // om with default prefix
  do_test(log_type);
  int line_count = log_capture->for_each_line_containing(
      "<202,1,2> FATAL ERROR bonjour", nullptr);
  EXPECT_EQ(1, line_count) << log_capture->dump_lines();

  om_->set_log_prefix("my prefix:");
  do_test(log_type);
  line_count = log_capture->for_each_line_containing(
      "my prefix: <202,1,2> FATAL ERROR bonjour", nullptr);
  EXPECT_EQ(1, line_count) << log_capture->dump_lines();

  log_type = 1;  // 1 == P4 logging
  do_test(log_type);
  line_count = log_capture->for_each_line_containing(
      ":-:-:<202,1,2>:bonjour", nullptr);
  EXPECT_EQ(1, line_count) << log_capture->dump_lines();
}


TEST_F(BFN_TEST_NAME(TestRmtLog), PacketSignature) {
  Packet* pkt1 = om_->pkt_create();
  const int pkt_size = 13;
  // use packet id rather than signature
  std::string actual = rmt_get_pkt_id_sig(pkt1, false);
  EXPECT_EQ("0x" + std::to_string(pkt1->pkt_id()) + ":", actual);

  // enable pkt sig in logging
  om_->set_log_pkt_signature(2, 8, true);
  // use signature, but signature not yet extracted
  actual = rmt_get_pkt_id_sig(pkt1, true);
  EXPECT_EQ("-:", actual);
  // extract signature (but no bytes in packet)
  pkt1->extract_signature();
  actual = rmt_get_pkt_id_sig(pkt1, true);
  EXPECT_EQ("-:", actual);
  // give packet some bytes
  uint8_t buf[pkt_size]{'h','e','l','l','o',' ','w','o','r','l','d', 0x7u, 0xdu};
  PacketBuffer* pkt_buf = new PacketBuffer(buf, pkt_size);  // deleted by Packet::destroy()
  pkt1->append(pkt_buf);
  pkt1->extract_signature();
  actual = rmt_get_pkt_id_sig(pkt1, true);
  EXPECT_EQ("0x6c6c6f20776f726c:", actual); // pkt_buf[2:8] "llo worl"
  // use id rather than extracted signature
  actual = rmt_get_pkt_id_sig(pkt1, false);
  EXPECT_EQ("0x" + std::to_string(pkt1->pkt_id()) + ":", actual);

  // use different byte range for signature
  om_->set_log_pkt_signature(0, 13, true);
  pkt1->extract_signature();
  actual = rmt_get_pkt_id_sig(pkt1, true);
  EXPECT_EQ("0x68656c6c6f20776f726c64070d:", actual); // pkt_buf[0:13] "hello world"7d

  om_->pkt_delete(pkt1);
}


TEST_F(BFN_TEST_NAME(TestRmtLog), P4SaluPrettyPrint) {
  TestLogger logger(om_, 4, 0, 0);
  int table_index = 1;
  std::string action_name = "MyAction";
  std::list<std::pair<int, uint32_t>> salu_log_value_list;
  for (int i = 0; i < RMT_LOG_SALU_OUTPUT_RESULT; i++) {
    salu_log_value_list.emplace_back(i, i);
  }
  std::list<std::string> actual = logger.do_rmt_log_p4_salu_pretty_print(
      table_index,
      action_name,
      salu_log_value_list);
  std::list<std::string> expected {
    "\t--- SALU Condition ---",
    "\t  Not supplied by program.",
    "\t    SALU ConditionLo: TRUE",
    "\t    SALU ConditionHi: TRUE",
    "\t--- SALU Update ---",
    "\t  None",
    "\t--- SALU Output ---",
    "\t    Output destination supplied by program",
    "\t---  SALU Register ---",
    "\t   Register Index: 0x1",
    "\t     Before stateful alu execution: 0x00000002",
    "\t     After stateful alu execution: 0x00000015"
  };
  auto it = actual.begin();
  for (const auto& ex : expected) {
    EXPECT_EQ(ex, *it);
    it++;
  }
}

}
