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
#include "model_core/json-loader.h"
#include "filesystem_helper.h"

namespace rmt_utests {

using model_core::JSONLoader;

TEST(TestJSONLoader, MaxCharacters) {
  // use a sample context.json, but we don't care what content is for this test
  std::string filename = get_resource_file_path("parser-static-config-tofinoXX.json");
  // this should load ok...
  JSONLoader loader1(filename, "testfile", 100000000);
  ASSERT_TRUE(loader1.IsLoaded());
  // this should fail while loading due to cap on characters read
  JSONLoader loader2(filename, "testfile", 10);
  ASSERT_FALSE(loader2.IsLoaded());
}

TEST(TestJSONLoader, InvalidUTF8) {
  // XXX: Verify that JSONLoader configures its json parser to detect
  // invalid UTF8.
  // Uses invalid JSON examples documented at:
  // https://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt
  char tmp_file_name[] = "/tmp/p4_target_conf_XXXXXX";
  int err = mkstemp(tmp_file_name);
  ASSERT_GT(err, 0);
  FILE *file = fopen(tmp_file_name, "w");

  // ["ok"] is OK
  unsigned char buf1[] = {'[', '"', 0x6f, 0x6b, '"', ']'};
  file = fopen(tmp_file_name, "w");
  fwrite(buf1, sizeof(char), sizeof(buf1), file);
  fclose(file);
  JSONLoader loader1(tmp_file_name, "testfile", 1000);
  EXPECT_TRUE(loader1.IsLoaded());

  // 0xfe is invalid anywhere
  unsigned char buf2[] = {'[',  '"', 0xfe,  '"', ']'};
  file = fopen(tmp_file_name, "w");
  fwrite(buf2, sizeof(char), sizeof(buf2), file);
  fclose(file);
  JSONLoader loader2(tmp_file_name, "testfile", 1000);
  EXPECT_FALSE(loader2.IsLoaded());

  // 2 byte sequences may start with 0xdf...
  // 0xdf followed by a space is invalid
  unsigned char buf3[] = {'[',  '"', 0xdf,  ' ', '"', ']'};
  file = fopen(tmp_file_name, "w");
  fwrite(buf3, sizeof(char), sizeof(buf3), file);
  fclose(file);
  JSONLoader loader3(tmp_file_name, "testfile", 1000);
  EXPECT_FALSE(loader3.IsLoaded());

  // 0xdf followed by < 0x80 is invalid
  unsigned char buf4[] = {'[',  '"', 0xdf, 0x7f, '"', ']'};
  file = fopen(tmp_file_name, "w");
  fwrite(buf4, sizeof(char), sizeof(buf4), file);
  fclose(file);
  JSONLoader loader4(tmp_file_name, "testfile", 1000);
  EXPECT_FALSE(loader4.IsLoaded());

  // 0xdf followed by >= 0x80 is OK
  unsigned char buf5[] = {'[',  '"', 0xdf,  0x80, '"', ']'};
  file = fopen(tmp_file_name, "w");
  fwrite(buf5, sizeof(char), sizeof(buf5), file);
  fclose(file);
  JSONLoader loader5(tmp_file_name, "testfile", 1000);
  EXPECT_TRUE(loader5.IsLoaded());
}

}
