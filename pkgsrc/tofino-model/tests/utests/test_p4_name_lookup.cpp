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
#include <stdio.h>
#include <cstdio>
#include "filesystem_helper.h"
#include "utests/test_namespace.h"
#include "model_core/p4_target_conf.h"
#include "model_core/version-string.h"
#include <p4-name-lookup.h>
#include <utests/gtest.h>
#include <mau.h>

namespace MODEL_CHIP_TEST_NAMESPACE {

class CaptureCStream {
 public:
  CaptureCStream(std::ostream *stream) : stream_(stream), original_buf_(nullptr) {}
  ~CaptureCStream() { stop(); }

  void start() {
    if (nullptr == original_buf_) {
      replacement_stream_.str("");
      replacement_stream_.clear();
      stream_->flush();
      original_buf_ = stream_->rdbuf();
      stream_->rdbuf(replacement_stream_.rdbuf());
    }
  }
  void stop() {
    if (nullptr != original_buf_) {
      stream_->rdbuf(original_buf_);
      original_buf_ = nullptr;
    }
  }
  std::string buffer() {
    return replacement_stream_.str();
  }

 private:
  std::ostream *stream_;
  std::stringstream replacement_stream_;
  std::streambuf *original_buf_;
};


class BFN_TEST_NAME(P4NameLookupFixture) : public BaseTest { };

TEST_F(BFN_TEST_NAME(P4NameLookupFixture), GetMatchFields) {
  using namespace MODEL_CHIP_NAMESPACE;
  // create a dummy PHV and load with some arbitrary values
  Phv phv(om_);
  phv.set_x(0, 0xa1a19898);
  phv.set_x(1, 0xb2b27676);
  phv.set_x(2, 0xc3c35454);
  phv.set_x(3, 0xd4d43232);
  phv.set_x(12, 0x00001234);
  phv.set_x(13, 0x00004321);
  phv.set_x(15, 0x0000000a);  // POV - set bits 1 and 3
  phv.set_x(195, 0x0000abba);  // egress_field
  // phv.print("test phv", true, Phv::kFlagsParser );

  std::string context_file_name = get_resource_file_path(
      "parser-static-config-tofinoXX.json");
  P4NameLookup p4_name_lookup(context_file_name);

  // NB sample context.json only has phv for stage 0 so use only that stage...
  std::list<std::string> res = p4_name_lookup.GetMatchFields(0, 1, phv, false);
  std::list<std::string> expected_0 {
    "\tWarning: 'foo' not found in context.json for stage 0 ingress\n",  // 'name' and 'global_name' swapped.
    "\t--validity_check--my_metadata.ip4_dst = 0x1\n",  // POV bit 3
    "\t--validity_check--my_metadata.ip4_src = 0x1\n",  // POV bit 1
    "\tsplit_header[7:0] = 0x34\n",
    "\tsplit_header[15:8] = 0x21\n"
  };
  // XXX: expect res to be sorted
  EXPECT_EQ(expected_0, res);

  res = p4_name_lookup.GetMatchFields(0, 2, phv, false);
  std::list<std::string> expected_1 = {
    "\t--validity_check--my_metadata.ip4_dst = 0x1\n",  // POV bit 3
    "\t--validity_check--my_metadata.sa_lo_32 = 0x0\n"  // POV bit 2
  };
  EXPECT_EQ(expected_1, res);

  // check graceful handling of non-existent table
  res = p4_name_lookup.GetMatchFields(0, 99, phv, false);
  std::list<std::string> expected_2 {
    "Could not find table in context (stage: 0, table 99)"
  };
  EXPECT_EQ(expected_2, res);

  // check graceful handling of non-existent phv for stage 2 table 1
  res = p4_name_lookup.GetMatchFields(2, 1, phv, false);
  std::list<std::string> expected_3 {
    "\tWarning: 'no_phv_alloc_for_stage_2' not found in context.json for stage 2 ingress\n"
  };
  EXPECT_EQ(expected_3, res);

  // check egress
  res = p4_name_lookup.GetMatchFields(0, 3, phv, true);
  std::list<std::string> expected_4 = {
    "\tegress_field[15:0] = 0xABBA\n"
  };
  EXPECT_EQ(expected_4, res);

  // check different stage 1
  res = p4_name_lookup.GetMatchFields(1, 1, phv, false);
  std::list<std::string> expected_5 = {
    "\tphv_1.stage_1[31:0] = 0xB2B27676\n"
  };
  EXPECT_EQ(expected_5, res);
}


TEST(BFN_TEST_NAME(P4NameLookup), ParseConfFile) {
  CaptureCStream cout_capture(&std::cout);
  CaptureCStream cerr_capture(&std::cerr);

  // no file
  cout_capture.start();
  cerr_capture.start();
  P4TargetConf p4_target_conf1(nullptr);
  cout_capture.stop();
  cerr_capture.stop();
  std::string expected_cout = "";
  std::string expected_cerr = "";
  EXPECT_EQ(expected_cout, cout_capture.buffer());
  EXPECT_EQ(expected_cerr, cerr_capture.buffer());
  EXPECT_FALSE(p4_target_conf1.IsLoaded());

  // non-existent file
  cout_capture.start();
  cerr_capture.start();
  P4TargetConf p4_target_conf2("file_does_not_exist");
  cout_capture.stop();
  cerr_capture.stop();
  expected_cout = "Opening p4 target config file 'file_does_not_exist' ...\n";
  expected_cerr = "ERROR: Failed to open p4 target config file\n";
  EXPECT_EQ(expected_cout, cout_capture.buffer());
  EXPECT_EQ(expected_cerr, cerr_capture.buffer());
  EXPECT_FALSE(p4_target_conf2.IsLoaded());

  // make a temp file into which we'll write some json
  char tmp_file_name[] = "/tmp/p4_target_conf_XXXXXX";
  int err = mkstemp(tmp_file_name);
  ASSERT_GT(err, 0);

  // invalid json
  FILE *file = fopen(tmp_file_name, "w");
  fprintf(file, "invalid json");
  fclose(file);
  cout_capture.start();
  cerr_capture.start();
  P4TargetConf p4_target_conf3(tmp_file_name);
  cout_capture.stop();
  cerr_capture.stop();
  expected_cout = "Opening p4 target config file '";
  expected_cout += tmp_file_name;
  expected_cout += "' ...\n";
  expected_cerr = "ERROR: Failed parsing p4 target config file at offset 0: Invalid value.\n";
  EXPECT_EQ(expected_cout, cout_capture.buffer());
  EXPECT_EQ(expected_cerr, cerr_capture.buffer());
  EXPECT_FALSE(p4_target_conf3.IsLoaded());
  remove(tmp_file_name);

  // ok json
  file = fopen(tmp_file_name, "w");
  fprintf(file, "[{\"foo\":\"bar\"}]");
  fclose(file);
  cout_capture.start();
  cerr_capture.start();
  P4TargetConf p4_target_conf4(tmp_file_name);
  cout_capture.stop();
  cerr_capture.stop();
  expected_cout = "Opening p4 target config file '";
  expected_cout += tmp_file_name;
  expected_cout += "' ...\nLoaded p4 target config file\n";
  expected_cerr = "";
  EXPECT_EQ(expected_cout, cout_capture.buffer());
  EXPECT_EQ(expected_cerr, cerr_capture.buffer());
  EXPECT_TRUE(p4_target_conf4.IsLoaded());
  remove(tmp_file_name);

  // invalid json
  file = fopen(tmp_file_name, "w");
  fprintf(file, "[{\"foo\":\"bar\"},]");
  fclose(file);
  cout_capture.start();
  cerr_capture.start();
  P4TargetConf p4_target_conf5(tmp_file_name);
  cout_capture.stop();
  cerr_capture.stop();
  expected_cout = "Opening p4 target config file '";
  expected_cout += tmp_file_name;
  expected_cout += "' ...\n";
  expected_cerr = "ERROR: Failed parsing p4 target config file at offset 15: Invalid value.\n";
  EXPECT_EQ(expected_cout, cout_capture.buffer());
  EXPECT_EQ(expected_cerr, cerr_capture.buffer());
  EXPECT_FALSE(p4_target_conf5.IsLoaded());
  remove(tmp_file_name);
}

TEST(BFN_TEST_NAME(P4NameLookup), ParseConfFileSizeLimit) {
  // verify that the number of characters that will be read from a conf file is
  // constrained.

  // make a temp file into which we'll write some json
  char tmp_file_name[] = "/tmp/p4_target_conf_XXXXXX";
  int err = mkstemp(tmp_file_name);
  ASSERT_GT(err, 0);
  CaptureCStream cout_capture(&std::cout);
  CaptureCStream cerr_capture(&std::cerr);

  // file size at limit i.e. 1000000 characters
  FILE *file = fopen(tmp_file_name, "w");
  fprintf(file, "[");
  for (int i = 1; i < 999985; i+=12) {
    fprintf(file, "{\"x\" : \"y\"},");
  }
  fprintf(file, "{\"size\": \"ok\"}]");
  fclose(file);
  cout_capture.start();
  cerr_capture.start();
  P4TargetConf p4_target_conf1(tmp_file_name);
  cout_capture.stop();
  cerr_capture.stop();
  std::string expected_cout = "Opening p4 target config file '";
  expected_cout += tmp_file_name;
  expected_cout += "' ...\nLoaded p4 target config file\n";
  std::string expected_cerr = "";
  EXPECT_EQ(expected_cout, cout_capture.buffer());
  EXPECT_EQ(expected_cerr, cerr_capture.buffer());
  EXPECT_TRUE(p4_target_conf1.IsLoaded());
  remove(tmp_file_name);

  // file size just beyond limit - one extra char
  file = fopen(tmp_file_name, "w");
  fprintf(file, "[");
  for (int i = 1; i < 999985; i+=12) {
    fprintf(file, "{\"x\" : \"y\"},");
  }
  fprintf(file, "{\"size\": \"bad\"}]");
  fclose(file);
  cout_capture.start();
  cerr_capture.start();
  P4TargetConf p4_target_conf2(tmp_file_name);
  cout_capture.stop();
  cerr_capture.stop();
  expected_cout = "Opening p4 target config file '";
  expected_cout += tmp_file_name;
  expected_cout += "' ...\n";
  expected_cerr = "ERROR: Failed parsing p4 target config file: Maximum character count exceeded\n";
  EXPECT_EQ(expected_cout, cout_capture.buffer());
  EXPECT_EQ(expected_cerr, cerr_capture.buffer());
  EXPECT_FALSE(p4_target_conf2.IsLoaded());
  remove(tmp_file_name);
}

TEST(BFN_TEST_NAME(P4NameLookup), LookupInConfFileMulti) {
  std::string conf_file_name = get_resource_file_path(
      "sample-conf-file-multi.json");
  P4TargetConf p4_target_conf(conf_file_name.c_str());
  EXPECT_EQ("path/to/context.json", p4_target_conf.GetContext(0, 0));
  EXPECT_EQ("path/to/context.json", p4_target_conf.GetContext(0, 1));
  EXPECT_EQ("path/to/context.json", p4_target_conf.GetContext(0, 2));
  EXPECT_EQ("path/to/context.json", p4_target_conf.GetContext(0, 3));
  EXPECT_EQ("path/to/contextx.json", p4_target_conf.GetContext(1, 0));
  EXPECT_EQ("path/to/contexty.json", p4_target_conf.GetContext(1, 1));
  EXPECT_EQ("path/to/contexty.json", p4_target_conf.GetContext(1, 2));
  EXPECT_EQ("path/to/contextz.json", p4_target_conf.GetContext(1, 3));
}

TEST(BFN_TEST_NAME(P4NameLookup), LookupInConfFileMultiNoPipeScope) {
  // XXX: use context array with no pipe-scope as default for any pipe;
  // verify that if a pipe_scope is found referring to a subset of pipe indexes
  // then that will be respected for those pipe indexes
  std::string conf_file_name = get_resource_file_path(
      "sample-conf-file-multi.json");
  P4TargetConf p4_target_conf(conf_file_name.c_str());
  EXPECT_EQ("path/to/context-no-pipe-scope.json", p4_target_conf.GetContext(2, 0));
  EXPECT_EQ("path/to/context-pipe-scope-1-3.json", p4_target_conf.GetContext(2, 1));
  EXPECT_EQ("path/to/context-no-pipe-scope.json", p4_target_conf.GetContext(2, 2));
  EXPECT_EQ("path/to/context-pipe-scope-1-3.json", p4_target_conf.GetContext(2, 3));
}

TEST(BFN_TEST_NAME(P4NameLookup), LookupInConfFileMultiMissingElement) {
  // XXX: verify improved logging of errors
  std::string conf_file_name = get_resource_file_path(
      "sample-conf-file-multi.json");
  P4TargetConf p4_target_conf(conf_file_name.c_str());
  EXPECT_THROW({
    try {
      p4_target_conf.GetContext(3, 0);
    } catch (std::invalid_argument& ia) {
      EXPECT_STREQ("Trying p4_devices: "
                   "Could not find key 'context' in JSON object. "
                   "Trying p4_program_list: "
                   "Could not find key 'p4_program_list' in JSON object.",
                   ia.what());
      throw;
    }
  },
  std::invalid_argument);
}

TEST(BFN_TEST_NAME(P4NameLookup), LookupInConfFile) {
  std::string conf_file_name = get_resource_file_path(
      "sample-conf-file.json");
  P4TargetConf p4_target_conf(conf_file_name.c_str());
  EXPECT_EQ("path/to/context.json", p4_target_conf.GetContext(0, 0));
  EXPECT_EQ("path/to/context.json", p4_target_conf.GetContext(0, 1));
  EXPECT_EQ("path/to/context.json", p4_target_conf.GetContext(0, 2));
  EXPECT_EQ("path/to/context.json", p4_target_conf.GetContext(0, 3));
}

TEST(BFN_TEST_NAME(P4NameLookup), ParseContextFile) {
  CaptureCStream cout_capture(&std::cout);
  CaptureCStream cerr_capture(&std::cerr);

  // empty file name
  cout_capture.start();
  cerr_capture.start();
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup1("");
  cout_capture.stop();
  cerr_capture.stop();
  std::string expected_cout = "";
  std::string expected_cerr = "";
  EXPECT_EQ(expected_cout, cout_capture.buffer());
  EXPECT_EQ(expected_cerr, cerr_capture.buffer());
  EXPECT_FALSE(p4_name_lookup1.IsLoaded());

  // non-existent file
  cout_capture.start();
  cerr_capture.start();
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup2("file_does_not_exist");
  cout_capture.stop();
  cerr_capture.stop();
  expected_cout = "Opening context file 'file_does_not_exist' ...\n";
  expected_cerr = "ERROR: Failed to open context file\n";
  EXPECT_EQ(expected_cout, cout_capture.buffer());
  EXPECT_EQ(expected_cerr, cerr_capture.buffer());
  EXPECT_FALSE(p4_name_lookup2.IsLoaded());

  // make a temp file into which we'll write some json
  char tmp_file_name[] = "/tmp/p4_name_lookup_XXXXXX";
  int err = mkstemp(tmp_file_name);
  std::string tmp_file_name_str(tmp_file_name);
  ASSERT_GT(err, 0);

  // invalid json
  FILE *file = fopen(tmp_file_name, "w");
  fprintf(file, "invalid json");
  fclose(file);
  cout_capture.start();
  cerr_capture.start();
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup3(tmp_file_name_str);
  cout_capture.stop();
  cerr_capture.stop();
  expected_cout = "Opening context file '";
  expected_cout += tmp_file_name;
  expected_cout += "' ...\n";
  expected_cerr = "ERROR: Failed parsing context file at offset 0: Invalid value.\n";
  EXPECT_EQ(expected_cout, cout_capture.buffer());
  EXPECT_EQ(expected_cerr, cerr_capture.buffer());
  EXPECT_FALSE(p4_name_lookup3.IsLoaded());
  remove(tmp_file_name);

  // valid json, but not object at root
  file = fopen(tmp_file_name, "w");
  fprintf(file, "[{\"foo\":\"bar\"}]");
  fclose(file);
  cout_capture.start();
  cerr_capture.start();
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup4(tmp_file_name_str);
  cout_capture.stop();
  cerr_capture.stop();
  expected_cout = "Opening context file '";
  expected_cout += tmp_file_name;
  expected_cout += "' ...\n";
  expected_cerr = "ERROR: Invalid context. Expected object at root.\n";
  EXPECT_EQ(expected_cout, cout_capture.buffer());
  EXPECT_EQ(expected_cerr, cerr_capture.buffer());
  EXPECT_FALSE(p4_name_lookup4.IsLoaded());
  remove(tmp_file_name);

  // ok json
  file = fopen(tmp_file_name, "w");
  fprintf(file, "{\"foo\":\"bar\", \"fruits\":[\"apple\", \"kiwi\"]}");
  fclose(file);
  cout_capture.start();
  cerr_capture.start();
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup5(tmp_file_name_str);
  cout_capture.stop();
  cerr_capture.stop();
  expected_cout = "Opening context file '";
  expected_cout += tmp_file_name;
  expected_cout += "' ...\nLoaded context file\n";
  expected_cerr = "";
  EXPECT_EQ(expected_cout, cout_capture.buffer());
  EXPECT_EQ(expected_cerr, cerr_capture.buffer());
  EXPECT_TRUE(p4_name_lookup5.IsLoaded());
  remove(tmp_file_name);
}

TEST(BFN_TEST_NAME(P4NameLookup), ContextGetSchemaVersion) {
  // make a temp file into which we'll write some json
  char tmp_file_name[] = "/tmp/p4_name_lookup_XXXXXX";
  int err = mkstemp(tmp_file_name);
  std::string tmp_file_name_str(tmp_file_name);
  ASSERT_GT(err, 0);

  FILE *file = fopen(tmp_file_name, "w");
  fprintf(file, "{ \"schema_version\" : \"1.6.1\", \"other\": \"stuff\" }");
  fclose(file);
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup1(tmp_file_name_str);
  std::string actual = p4_name_lookup1.GetSchemaVersion();
  EXPECT_EQ("1.6.1", actual);
  remove(tmp_file_name);

  // check Version class
  model_core::VersionString v_1_6_0("1.6.0");
  model_core::VersionString v_1_6_1("1.6.1");
  model_core::VersionString v_2_0_0("2.0.0");
  model_core::VersionString v_actual(actual);
  EXPECT_TRUE(v_actual == v_1_6_1);
  EXPECT_TRUE(v_actual != v_1_6_0);
  EXPECT_FALSE(v_actual < v_1_6_0);
  EXPECT_FALSE(v_1_6_0 == v_actual);
  EXPECT_TRUE(v_actual > v_1_6_0);
  EXPECT_TRUE(v_2_0_0 > v_actual);
  EXPECT_STREQ(v_actual.str().c_str(), "1.6.1");
  EXPECT_THROW(model_core::VersionString v("invalid"), std::invalid_argument);

  // missing schema_version
  file = fopen(tmp_file_name, "w");
  fprintf(file, "{ \"other\": \"stuff\" }");
  fclose(file);
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup2(tmp_file_name_str);
  EXPECT_THROW(p4_name_lookup2.GetSchemaVersion(), std::invalid_argument);
  remove(tmp_file_name);
}

TEST(BFN_TEST_NAME(P4NameLookup), LookupParserState) {
  std::string conf_file_name = get_resource_file_path(
      "parser-static-config-tofinoXX.json");
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup(conf_file_name);
  // lookup ingress state for parser 0
  EXPECT_EQ("my_state_0", p4_name_lookup.GetParserStateName(0, false, 0));
  // lookup ingress state for parser 1
  EXPECT_EQ("my_state_1", p4_name_lookup.GetParserStateName(1, false, 1));
  // lookup egress state for parser 0
  EXPECT_EQ("Error : Unknown-State", p4_name_lookup.GetParserStateName(0, true, 0));
}

TEST(BFN_TEST_NAME(P4NameLookup), LookupMultiParserState) {
  std::string conf_file_name = get_resource_file_path(
      "parser-static-config-multi-parser-tofinoXX.json");
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup(conf_file_name);
  // lookup ingress state for parser 0
  EXPECT_EQ("start", p4_name_lookup.GetParserStateName(0, false, 2));
  // lookup ingress state for parser 1
  EXPECT_EQ("parse_ipv4", p4_name_lookup.GetParserStateName(1, false, 4));
  // lookup egress state for parser 0
  EXPECT_EQ("start", p4_name_lookup.GetParserStateName(0, true, 1));
  // lookup unknown state
  EXPECT_EQ("Error : Unknown-State", p4_name_lookup.GetParserStateName(0, false, 99));
  // lookup unknown parser
  EXPECT_EQ("Error: Could not find states for parser id 17",
            p4_name_lookup.GetParserStateName(17, false, 1));
}

TEST(BFN_TEST_NAME(P4NameLookup), GetFieldName) {
  std::string conf_file_name = get_resource_file_path(
      "parser-static-config-tofinoXX.json");
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup(conf_file_name);
  EXPECT_EQ("I [my_metadata.sa_lo_32[31:0]]", p4_name_lookup.GetFieldName(0, 1));
  // repeat to flush out any bugs in caching...
  EXPECT_EQ("I [my_metadata.sa_lo_32[31:0]]", p4_name_lookup.GetFieldName(0, 1));
  EXPECT_EQ("E [egress_field[15:0]]", p4_name_lookup.GetFieldName(0, 195));
  EXPECT_EQ("E [egress_field[15:0]]", p4_name_lookup.GetFieldName(0, 195));
}

TEST_F(BFN_TEST_NAME(P4NameLookupFixture), GetValidHeaderNames) {
  using namespace MODEL_CHIP_NAMESPACE;
  Phv phv(om_);
  phv.set_p(15, 0xa); // set POV bits 1 and 3

  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup_empty("");
  std::list<std::string> actual = p4_name_lookup_empty.GetValidHeaderNames(false, phv);;
  std::list<std::string> expected_1 = { };
  EXPECT_EQ(expected_1, actual);

  std::string conf_file_name = get_resource_file_path(
      "parser-static-config-tofinoXX.json");
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup(conf_file_name);
  actual = p4_name_lookup.GetValidHeaderNames(false, phv);
  std::list<std::string> expected_2 = {
      "Header my_metadata.ip4_dst is valid\n",
      "Header my_metadata.ip4_src is valid\n"
  };
  expected_2.sort();
  actual.sort();
  EXPECT_EQ(expected_2, actual);
}

TEST_F(BFN_TEST_NAME(P4NameLookupFixture), GetTableName) {
  using namespace MODEL_CHIP_NAMESPACE;
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup_empty("");
  std::string actual = p4_name_lookup_empty.GetTableName(0, 1);
  EXPECT_EQ("JSON object not found.", actual);

  std::string conf_file_name = get_resource_file_path(
      "parser-static-config-tofinoXX.json");
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup(conf_file_name);
  actual = p4_name_lookup.GetTableName(0, 1);
  EXPECT_EQ("table_0_1", actual);
  actual = p4_name_lookup.GetTableName(0, 2);
  EXPECT_EQ("table_0_2", actual);
  actual = p4_name_lookup.GetTableName(1, 1);
  EXPECT_EQ("table_1_1", actual);
  actual = p4_name_lookup.GetTableName(2, 1);
  EXPECT_EQ("table_2_1", actual);
  actual = p4_name_lookup.GetTableName(99, 99);
  EXPECT_EQ("P4 table not valid - Could not find table in context (stage: 99, table 99)", actual);
  actual = p4_name_lookup.GetTableName(-1, -1);
  EXPECT_EQ("--END_OF_PIPELINE--", actual);
}

TEST_F(BFN_TEST_NAME(P4NameLookupFixture), GetGatewayConditionName) {
  using namespace MODEL_CHIP_NAMESPACE;
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup_empty("");
  std::string actual = p4_name_lookup_empty.GetGatewayConditionName(0, 4);
  EXPECT_EQ("JSON object not found.", actual);

  std::string conf_file_name = get_resource_file_path(
      "parser-static-config-tofinoXX.json");
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup(conf_file_name);
  actual = p4_name_lookup.GetGatewayConditionName(0, 4);
  EXPECT_EQ("the_condition", actual);
  actual = p4_name_lookup.GetGatewayConditionName(0, 2);
  EXPECT_EQ("Could not find table in context (stage: 0, table 2)", actual);
}

TEST_F(BFN_TEST_NAME(P4NameLookupFixture), GetGatewayHasAttachedTable) {
  using namespace MODEL_CHIP_NAMESPACE;
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup_empty("");
  bool actual = p4_name_lookup_empty.GetGatewayHasAttachedTable(0, 4);
  EXPECT_FALSE(actual);

  std::string conf_file_name = get_resource_file_path(
      "parser-static-config-tofinoXX.json");
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup(conf_file_name);
  actual = p4_name_lookup.GetGatewayHasAttachedTable(0, 4);
  EXPECT_TRUE(actual);
  actual = p4_name_lookup.GetGatewayHasAttachedTable(0, 2);
  EXPECT_FALSE(actual);
}

TEST_F(BFN_TEST_NAME(P4NameLookupFixture), GetActionName) {
  using namespace MODEL_CHIP_NAMESPACE;
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup_empty("");
  std::string actual = p4_name_lookup_empty.GetActionName(0, 1, 1);
  EXPECT_EQ("JSON object not found.", actual);

  std::string conf_file_name = get_resource_file_path(
      "parser-static-config-tofinoXX.json");
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup(conf_file_name);
  actual = p4_name_lookup.GetActionName(0, 1, 1);
  EXPECT_EQ("action_name_0_1_1", actual);
  actual = p4_name_lookup.GetActionName(0, 1, 2);
  EXPECT_EQ("action_name_0_1_2", actual);
  actual = p4_name_lookup.GetActionName(0, 2, 7);
  EXPECT_EQ("action_name_0_2_7", actual);
  // no action
  actual = p4_name_lookup.GetActionName(0, 2, 8);
  EXPECT_EQ("No-Action(PFE=0)", actual);
  // not valid instr_addr
  actual = p4_name_lookup.GetActionName(0, 2, 0x60);
  EXPECT_EQ("ERROR:Invalid-Action", actual);
  // not a match table...
  actual = p4_name_lookup.GetActionName(0, 4, 4);
  EXPECT_EQ("Could not find table in context (stage: 0, table 4)", actual);
}

TEST_F(BFN_TEST_NAME(P4NameLookupFixture), GetStflTablePhysicalRow) {
  using namespace MODEL_CHIP_NAMESPACE;
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup_empty("");
  int actual = p4_name_lookup_empty.GetStflTablePhysicalRow(0, 1, "action_1");
  EXPECT_EQ(-1, actual);

  std::string conf_file_name = get_resource_file_path(
      "parser-static-config-tofinoXX.json");
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup(conf_file_name);
  actual = p4_name_lookup.GetStflTablePhysicalRow(0, 1, "action_1");
  EXPECT_EQ(5, actual); // 2 * 2(meter_alu_index) + 1
  actual = p4_name_lookup.GetStflTablePhysicalRow(0, 1, "action_99");
  EXPECT_EQ(-1, actual);
  actual = p4_name_lookup.GetStflTablePhysicalRow(0, 99, "action_1");
  EXPECT_EQ(-1, actual);
}

TEST_F(BFN_TEST_NAME(P4NameLookupFixture), GetActionInfo) {
  using namespace MODEL_CHIP_NAMESPACE;
  Phv iphv(om_);
  iphv.set_x(0, 0xa);
  iphv.set_x(14, 0xb);
  iphv.set_x(195, 0xc);  // egress field
  Phv ephv(om_);
  ephv.set_x(0, 0xb);

  Mau *mau = om_->mau_lookup(pipe_index(), 0);
  ASSERT_TRUE(mau != NULL);
  MauLookupResult mau_lookup_result;
  mau_lookup_result.init(mau, mau->mau_result_bus());
  mau_lookup_result.reset();
  mau_lookup_result.set_active(true);
  mau_lookup_result.set_match(true);
  mau_lookup_result.set_logical_table(1);
  mau_lookup_result.set_match_buses( 0x0001 );

  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup_empty("");
  std::list<std::string> actual = p4_name_lookup_empty.GetActionInfo(
      0, 1, "action_1", iphv, ephv, mau_lookup_result);
  EXPECT_EQ(std::list<std::string> {}, actual);

  std::string conf_file_name = get_resource_file_path(
      "parser-static-config-tofinoXX.json");
  MODEL_CHIP_NAMESPACE::P4NameLookup p4_name_lookup(conf_file_name);
  actual = p4_name_lookup.GetActionInfo(
      0, 1, "action_1", iphv, ephv, mau_lookup_result);
  std::list<std::string> expected = {
      "Action Results:\n",
      "\t----- primitive_1 -----\n",
      "\tOperation:\n",
      "\tadd\n",
      "\tDestination:\n",
      "\tWarning: 'eg_intr_md.pkt_length' not found in context.json for stage 1 ingress\n",
      "\tSource 1:\n",
      "\tpkt_len_adjust=action_param\n",
      "\tSource 2:\n",
      "\tjunk_word_14_a[7:0] = 0xB\n",
      "\tSource 3:\n",
      "\tWarning: 'egress_field' not found in context.json for stage 0 ingress\n",
      "\t----- primitive_2 -----\n",
      "\tOperation:\n",
      "\tadd\n",
      "\tDestination:\n",
      "\ttable_0_1_stateful=stateful\n"
  };

  EXPECT_EQ(expected, actual);

  // check looking up *egress* phv src's; abuse the existing MAU lookup result
  // for this purpose
  mau_lookup_result.set_ingress(false);
  actual = p4_name_lookup.GetActionInfo(
      0, 1, "action_1", iphv, ephv, mau_lookup_result);
  std::list<std::string> expected_egress = {
      "Action Results:\n",
      "\t----- primitive_1 -----\n",
      "\tOperation:\n",
      "\tadd\n",
      "\tDestination:\n",
      "\tWarning: 'eg_intr_md.pkt_length' not found in context.json for stage 1 egress\n",
      "\tSource 1:\n",
      "\tpkt_len_adjust=action_param\n",
      "\tSource 2:\n",
      "\tWarning: 'junk_word_14_a' not found in context.json for stage 0 egress\n",
      "\tSource 3:\n",
      "\tegress_field[15:0] = 0xC\n",
      "\t----- primitive_2 -----\n",
      "\tOperation:\n",
      "\tadd\n",
      "\tDestination:\n",
      "\ttable_0_1_stateful=stateful\n"
  };
  EXPECT_EQ(expected_egress, actual);

}

}
