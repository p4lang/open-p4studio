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
#include <pipe-object.h>

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

class BFN_TEST_NAME(PipeObject) : public BaseTest { };

TEST_F(BFN_TEST_NAME(PipeObject), TestGetLocalPortIndex) {
  ASSERT_EQ(1, GLOBAL_THROW_ON_ASSERT);  // sanity check
  int pipe_index = 0;
  PipeObject pipe_object(om_, pipe_index);
  // Test deliberately provokes RMT_ASSERT failures, so capture them to avoid
  // noise in test logs
  EXPECT_TRUE(nullptr != rmt_stdout_capture()->start());
  EXPECT_TRUE(nullptr != rmt_stderr_capture()->start());
  EXPECT_THROW(pipe_object.get_local_port_index(-1), std::runtime_error);
  EXPECT_THROW(pipe_object.get_local_port_index(72), std::runtime_error);

  for (int pipe_index = 0; pipe_index < 4; pipe_index++) {
    int pipe_base = 0x80 * pipe_index;
    PipeObject pipe_object(om_, pipe_index);
    for (int i = pipe_base; i < pipe_base + 72; i++) {
      int chan_index = -1;
      EXPECT_NO_THROW(chan_index = pipe_object.get_local_port_index(i));
      EXPECT_EQ(i - pipe_base, chan_index) << i;
    }
    int bad_pipe_base = 0x80 * ((pipe_index + 1) % 4);
    for (int i = bad_pipe_base; i < bad_pipe_base + 72; i++) {
      EXPECT_THROW(pipe_object.get_local_port_index(i), std::runtime_error);
    }
  }
  rmt_stdout_capture()->stop();
  rmt_stderr_capture()->stop();
  // check all lines start with "ERROR ASSERT:"
  // 2 + (4 * 72) = 290 expected rmt assert errors
  EXPECT_EQ(290, rmt_stdout_capture()->for_each_line_starts_with(
      "ERROR ASSERT:")) << rmt_stdout_capture()->dump_lines(200);
  EXPECT_EQ(290, rmt_stderr_capture()->for_each_line_starts_with(
      "ERROR ASSERT:")) << rmt_stderr_capture()->dump_lines(200);
}


TEST_F(BFN_TEST_NAME(PipeObject), TestRmtLog) {
  PipeObject test_obj(om_, 1);
  char long_str[800];
  for (int i=0; i<800; i++) long_str[i] = 48 + (i % 10);
  auto log_capture = rmt_logger_capture();

  // check logs filtered according to flags - set no flags
  test_obj.set_log_flags(UINT64_C(0));
  log_capture->start();
  test_obj.rmt_log_va(UINT64_C(0x1), "This is %s", "*very* bad");  // NB no new line
  test_obj.rmt_log_va(UINT64_C(0x2), "This is %s\n", "not so bad");
  test_obj.rmt_log_va(UINT64_C(0x8), "This is %s\n", "a warning");
  test_obj.rmt_log_va(UINT64_C(0x8000000000000000), "This is always logged");
  test_obj.rmt_log_va(UINT64_C(0x10), "This is %s %d\n", "fine!", 42);
  test_obj.rmt_log_va(UINT64_C(0x10), "%s", long_str);
  log_capture->stop();
  std::string expected = " This is always logged\n";
  EXPECT_EQ(expected, log_capture->dump_lines(std::string::npos, false));

  // check logs filtered according to flags - set FEW flags
  test_obj.set_log_flags(FEW);
  log_capture->clear();
  log_capture->start();
  test_obj.rmt_log_va(UINT64_C(0x1), "This is %s", "*very* bad");  // NB no new line
  test_obj.rmt_log_va(UINT64_C(0x2), "This is %s\n", "not so bad");
  test_obj.rmt_log_va(UINT64_C(0x8), "This is %s\n", "a warning");
  test_obj.rmt_log_va(UINT64_C(0x8000000000000000), "This is always logged");
  test_obj.rmt_log_va(UINT64_C(0x10), "This is %s %d\n", "fine!", 42);
  test_obj.rmt_log_va(UINT64_C(0x10), "%s", long_str);
  log_capture->stop();
  expected = "FATAL ERROR This is *very* bad\n"
             "ERROR This is not so bad\n"
             "WARN This is a warning\n"
             " This is always logged\n";
  EXPECT_EQ(expected, log_capture->dump_lines(std::string::npos, false));

  // set ALL flags
  test_obj.set_log_flags(ALL);
  log_capture->clear();
  log_capture->start();
  test_obj.rmt_log_va(UINT64_C(0x1), "");
  test_obj.rmt_log_va(UINT64_C(0x1), "This is %s", "*very* bad");  // NB no new line
  test_obj.rmt_log_va(UINT64_C(0x2), "This is %s\n", "not so bad");
  test_obj.rmt_log_va(UINT64_C(0x8), "This is %s\n", "a warning");
  test_obj.rmt_log_va(UINT64_C(0x8000000000000000), "This is always logged");
  test_obj.rmt_log_va(UINT64_C(0x10), "This is %s %d\n", "fine!", 42);
  test_obj.rmt_log_va(UINT64_C(0x10), "%s", long_str);
  log_capture->stop();
  expected = "FATAL ERROR \n"
             "FATAL ERROR This is *very* bad\n"
             "ERROR This is not so bad\n"
             "WARN This is a warning\n"
             " This is always logged\n"
             "INF This is fine! 42\n"
             "INF ";
  std::string long_line = long_str;
  long_line.resize(766);  // expect actual logged line to be truncated
  long_line += "\n";      // and terminated with a newline
  expected += long_line;
  EXPECT_EQ(expected, log_capture->dump_lines(std::string::npos, false));
}

TEST_F(BFN_TEST_NAME(PipeObject), TestRmtLogType) {
  // check that pipe, stage index prefix is added
  // type 3 == kRmtTypeDeparserBlock -> print depth 2
  PipeObject test_obj(om_, 1, 2, 3);
  auto log_capture = rmt_logger_capture();
  test_obj.set_log_flags(ALL);
  log_capture->start();
  test_obj.rmt_log_va(UINT64_C(0x8), "%s", "My message");
  log_capture->stop();
  std::string expected = "<202,1,2> WARN My message\n";
  EXPECT_EQ(expected, log_capture->dump_lines(std::string::npos, false));

  // type 32 == kRmtTypeMauSram -> print depth 4
  PipeObject test_obj_mau_sram(om_, 1, 2, 32, 3, 4);
  log_capture->clear();
  test_obj_mau_sram.set_log_flags(ALL);
  log_capture->start();
  test_obj_mau_sram.rmt_log_va(UINT64_C(0x8), "%s", "My message");
  log_capture->stop();
  expected = "<202,1,2,3,4> WARN My message\n";
  EXPECT_EQ(expected, log_capture->dump_lines(std::string::npos, false));
}

TEST_F(BFN_TEST_NAME(PipeObject), TestRmtLogP4Type) {
  PipeObject test_obj(om_, 1);
  char long_str[1000];
  for (int i=0; i<1000; i++) long_str[i] = 48 + (i % 10);
  auto log_capture = rmt_logger_capture();

  // create some packets to provide info for log prefixes
  EXPECT_FALSE(om_->log_use_pkt_sig());  // sanity check
  Packet *ig_pkt = om_->pkt_create();
  EXPECT_FALSE(nullptr == ig_pkt);
  ig_pkt->pkt_id(1);
  Packet *eg_pkt = om_->pkt_create();
  EXPECT_FALSE(nullptr == eg_pkt);
  eg_pkt->pkt_id(2);

  // check logs filtered according to flags - set restricted flags
  test_obj.set_log_type_flags(UINT64_C(0x21), RMT_LOG_TYPE_P4);
  log_capture->start();
  test_obj.rmt_log_va(1, UINT64_C(0x1), "");
  test_obj.rmt_log_va(1, UINT64_C(0x1), "I need a newline");
  test_obj.rmt_log_va(1, UINT64_C(0x20), "I have a newline\n");  // Verbose
  test_obj.rmt_log_va(1, UINT64_C(0x40), "I am ignored");  // Debug
  log_capture->stop();
  std::string expected = ":-:-:<202,1,->:\n"
                         ":-:-:<202,1,->:I need a newline\n"
                         "    :-:-:<202,1,->:I have a newline\n";
  EXPECT_EQ(expected, log_capture->dump_lines(std::string::npos, false));

  // check logs filtered according to flags ignores setting ALL flags on
  // *different* type
  test_obj.set_log_flags(ALL); // NB sets flags for different type of logging
  log_capture->clear();
  log_capture->start();
  test_obj.rmt_log_va(1, UINT64_C(0x1), "");
  test_obj.rmt_log_va(1, UINT64_C(0x1), "I need a newline");
  test_obj.rmt_log_va(1, UINT64_C(0x20), "I have a newline\n");  // Verbose
  test_obj.rmt_log_va(1, UINT64_C(0x40), "I am ignored");  // Debug
  log_capture->stop();
  expected = ":-:-:<202,1,->:\n"
             ":-:-:<202,1,->:I need a newline\n"
             "    :-:-:<202,1,->:I have a newline\n";
  EXPECT_EQ(expected, log_capture->dump_lines(std::string::npos, false));

  // set ALL flags, setup thread local packet info
  rmt_thd_info.ig_packet = ig_pkt;
  rmt_thd_info.eg_packet = eg_pkt;
  test_obj.set_log_type_flags(ALL, RMT_LOG_TYPE_P4);
  log_capture->clear();
  log_capture->start();
  test_obj.rmt_log_va(1, UINT64_C(0x1), "%s %s", "p4", "logging");
  test_obj.rmt_log_va(1, UINT64_C(0x40), "%s", long_str);  // Debug
  log_capture->stop();
  expected = ":0x1:0x2:<202,1,->:p4 logging\n";
  std::string long_line = long_str;
  long_line = "        :0x1:0x2:<202,1,->:";
  long_line += long_str;;
  long_line.resize(878);  // expect actual logged line to be truncated
  long_line += "\n";      // and terminated with a newline
  expected += long_line;
  EXPECT_EQ(expected, log_capture->dump_lines(std::string::npos, false));
  om_->pkt_delete(ig_pkt);
  om_->pkt_delete(eg_pkt);
}

TEST_F(BFN_TEST_NAME(PipeObject), TestRmtLogChecks) {
  PipeObject test_obj(om_, 1);
  const uint64_t bit1 = 1;

  // p4 type flags - low order flags enabled by default, except for model type
  // which has configurable default flags
  uint64_t init_model_flags = test_obj.log_type_flags(RMT_LOG_TYPE_MODEL);
  EXPECT_EQ(init_model_flags, test_obj.log_flags());
  for (int test_bit = -1; test_bit < 64; test_bit++) {
    uint64_t flags = (test_bit < 0) ? UINT64_C(0) : bit1 << test_bit;
    if (((test_bit >= 0) && (test_bit < 8)) || (test_bit == 63)) {
      // basic mask, including force flag
      EXPECT_TRUE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_P4))
                  << test_bit;
      EXPECT_TRUE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_PKT))
                  << test_bit;
    } else {
      EXPECT_FALSE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_P4))
                   << test_bit;
      EXPECT_FALSE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_PKT))
                   << test_bit;
    }
    if ((flags & init_model_flags) || (test_bit == 63)) {
      EXPECT_TRUE(test_obj.rmt_log_check(flags)) << test_bit;
      EXPECT_TRUE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_MODEL))
                  << test_bit;
    } else {
      EXPECT_FALSE(test_obj.rmt_log_check(flags)) << test_bit;
      EXPECT_FALSE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_MODEL))
                   << test_bit;
    }
  }
  // check each bit enabled - model type
  for (int set_bit = -1; set_bit < 64; set_bit++) {
    uint64_t mask = (set_bit < 0) ? UINT64_C(0) : bit1 << set_bit;
    test_obj.set_log_flags(mask);
    for (int test_bit = -1; test_bit < 64; test_bit++) {
      uint64_t flags = (test_bit < 0) ? UINT64_C(0) : bit1 << test_bit;
      if ((mask && (test_bit == set_bit)) || (test_bit == 63)) {
        EXPECT_TRUE(test_obj.rmt_log_check(flags))
                    << test_bit << " " << set_bit;
      } else {
        EXPECT_FALSE(test_obj.rmt_log_check(flags))
                     << test_bit << " " << set_bit;
      }
    }
  }
  // repeat using explicit type methods for model type
  test_obj.set_log_type_flags(UINT64_C(0), RMT_LOG_TYPE_P4);  // clear p4 type
  for (int set_bit = -1; set_bit < 64; set_bit++) {
    uint64_t mask = (set_bit < 0) ? UINT64_C(0) : bit1 << set_bit;
    test_obj.set_log_type_flags(mask, RMT_LOG_TYPE_MODEL);
    for (int test_bit = -1; test_bit < 64; test_bit++) {
      uint64_t flags = (test_bit < 0) ? UINT64_C(0) : bit1 << test_bit;
      if (test_bit == 63) {
        // forced
        EXPECT_TRUE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_MODEL))
                    << test_bit << " " << set_bit;
        EXPECT_TRUE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_P4))
                    << test_bit << " " << set_bit;
      } else if (mask && (test_bit == set_bit)) {
        EXPECT_TRUE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_MODEL))
                    << test_bit << " " << set_bit;
        EXPECT_FALSE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_P4))
                     << test_bit << " " << set_bit;
      } else {
        EXPECT_FALSE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_MODEL))
                     << test_bit << " " << set_bit;
        EXPECT_FALSE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_P4))
                     << test_bit << " " << set_bit;
      }
    }
  }
  // repeat using explicit type methods for p4 type
  test_obj.set_log_type_flags(UINT64_C(0), RMT_LOG_TYPE_MODEL);
  for (int set_bit = -1; set_bit < 64; set_bit++) {
    uint64_t mask = (set_bit < 0) ? UINT64_C(0) : bit1 << set_bit;
    test_obj.set_log_type_flags(mask, RMT_LOG_TYPE_P4);
    for (int test_bit = -1; test_bit < 64; test_bit++) {
      uint64_t flags = (test_bit < 0) ? UINT64_C(0) : bit1 << test_bit;
      if (test_bit == 63) {
        // forced
        EXPECT_TRUE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_P4))
                    << test_bit << " " << set_bit;
        EXPECT_TRUE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_MODEL))
                    << test_bit << " " << set_bit;
      } else if (mask && (test_bit == set_bit)) {
        EXPECT_TRUE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_P4))
                    << test_bit << " " << set_bit;
        EXPECT_FALSE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_MODEL))
                     << test_bit << " " << set_bit;
      } else {
        EXPECT_FALSE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_P4))
                     << test_bit << " " << set_bit;
        EXPECT_FALSE(test_obj.rmt_log_type_check(flags, RMT_LOG_TYPE_MODEL))
                     << test_bit << " " << set_bit;
      }
    }
  }
}


TEST_F(BFN_TEST_NAME(PipeObject), TestRmtLogFlagSetting) {
  PipeObject test_obj(om_, 1);
  uint64_t test_flags = UINT64_C(256);
  uint64_t pipes = UINT64_C(15);  // pipe 1
  uint64_t no_flags = UINT64_C(0);

  test_obj.set_log_flags(test_flags);
  uint64_t actual_flags = test_obj.log_flags();
  uint64_t expected_flags = test_flags;
  EXPECT_EQ(expected_flags, actual_flags);
  actual_flags = test_obj.log_type_flags(RMT_LOG_TYPE_MODEL);
  EXPECT_EQ(expected_flags, actual_flags);

  // reset and use type parameterised method
  test_obj.set_log_flags(no_flags);
  actual_flags = test_obj.log_flags();
  EXPECT_EQ(no_flags, actual_flags);  // sanity check
  test_obj.set_log_type_flags(test_flags, RMT_LOG_TYPE_MODEL);
  actual_flags = test_obj.log_flags();
  EXPECT_EQ(expected_flags, actual_flags);
  actual_flags = test_obj.log_type_flags(RMT_LOG_TYPE_MODEL);
  EXPECT_EQ(expected_flags, actual_flags);

  // reset and use rmt object manager method;
  // this always OR's with fixed flags
  expected_flags |= FIXED_LOG_FLAGS;
  test_obj.set_log_flags(no_flags);
  actual_flags = test_obj.log_flags();
  EXPECT_EQ(no_flags, actual_flags);  // sanity check
  om_->update_log_flags(pipes, ALL, ALL, ALL, ALL, test_flags, test_flags);
  actual_flags = test_obj.log_flags();
  EXPECT_EQ(expected_flags, actual_flags);
  actual_flags = test_obj.log_type_flags(RMT_LOG_TYPE_MODEL);
  EXPECT_EQ(expected_flags, actual_flags);

  // reset and use rmt object manager type parameterised method;
  // this doesn't OR with fixed flags
  expected_flags = test_flags;
  test_obj.set_log_flags(no_flags);
  actual_flags = test_obj.log_flags();
  EXPECT_EQ(no_flags, actual_flags);  // sanity check
  om_->update_log_type_levels(pipes, ALL, RMT_LOG_TYPE_MODEL, ALL, test_flags);
  actual_flags = test_obj.log_flags();
  EXPECT_EQ(expected_flags, actual_flags);
  actual_flags = test_obj.log_type_flags(RMT_LOG_TYPE_MODEL);
  EXPECT_EQ(expected_flags, actual_flags);

  // now for P4 type log flags...
  test_obj.set_log_type_flags(no_flags, RMT_LOG_TYPE_P4);
  actual_flags = test_obj.log_type_flags(RMT_LOG_TYPE_P4);
  EXPECT_EQ(no_flags, actual_flags);  // sanity check
  RmtLoggerCapture *log_capture = rmt_logger_capture();
  log_capture->start();
  om_->update_log_type_levels(pipes, ALL, RMT_LOG_TYPE_P4, ALL, test_flags);
  log_capture->stop();
  actual_flags = test_obj.log_type_flags(RMT_LOG_TYPE_P4);
  EXPECT_EQ(expected_flags, actual_flags);
  // expect warning when no context.json is loaded...
  int pipe_count = 0;
  auto line_checker = [&pipe_count](int line_num, size_t pos, std::string line) {
    char expected[100];
    snprintf(
        expected, sizeof(expected),
        "updating P4 logging flags for pipe %d but P4 names have not been loaded",
        pipe_count);
    EXPECT_FALSE(line.find(expected) == std::string::npos) << line;
    pipe_count++;
  };
  int line_count = log_capture->for_each_line_containing(
      "WARNING: updating P4 logging flags", line_checker);
  EXPECT_EQ(4, line_count);

  // ..even when disabling all log levels
  log_capture->clear();
  log_capture->start();
  om_->update_log_type_levels(pipes, ALL, RMT_LOG_TYPE_P4, ALL, NON);
  log_capture->stop();
  expected_flags = UINT64_C(0);
  actual_flags = test_obj.log_type_flags(RMT_LOG_TYPE_P4);
  EXPECT_EQ(expected_flags, actual_flags);
  // expect warning when no context.json is loaded...
  pipe_count = 0;
  line_count = log_capture->for_each_line_containing(
      "WARNING: updating P4 logging flags", line_checker);
  EXPECT_EQ(4, line_count);

  // ...but not if context json has been loaded
  load_context_json_file(0);
  load_context_json_file(1);
  log_capture->clear();
  log_capture->start();
  om_->update_log_type_levels(pipes, ALL, RMT_LOG_TYPE_P4, ALL, test_flags);
  log_capture->stop();
  expected_flags = test_flags;
  actual_flags = test_obj.log_type_flags(RMT_LOG_TYPE_P4);
  EXPECT_EQ(expected_flags, actual_flags);
  pipe_count = 2;  // expect no warning for pipes 0 and 1
  line_count = log_capture->for_each_line_containing(
      "WARNING: updating P4 logging flags", line_checker);
  EXPECT_EQ(2, line_count);

  // ...and only if flags are updated for pipe that has no p4 names loaded
  log_capture->clear();
  log_capture->start();
  pipes = UINT64_C(7);
  om_->update_log_type_levels(pipes, ALL, RMT_LOG_TYPE_P4, ALL, test_flags);
  log_capture->stop();
  expected_flags = test_flags;
  actual_flags = test_obj.log_type_flags(RMT_LOG_TYPE_P4);
  EXPECT_EQ(expected_flags, actual_flags);
  pipe_count = 2;  // expect no warning for pipes 0, 1 and 4
  line_count = log_capture->for_each_line_containing(
      "WARNING: updating P4 logging flags", line_checker);
  EXPECT_EQ(1, line_count);
}

}  // namespace
