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

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "gtest.h"
#include <model_core/model.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

using namespace std;

int gtest_main(int argc, char **argv) {

  std::string test_list_file_name = "";
  for (int i=0; i< argc; i++) {
    if((strncmp(argv[i], "--test-list-file", 16) == 0)||
       (strncmp(argv[i], "--test_list_file", 16) == 0)) {
      if (i == (argc - 1)) {
        std::cout << "ERROR: --test-list-file requires a file" << std::endl;
        return 1;
      } else {
        test_list_file_name = argv[i+1];
        break;
      }
    }
  }

  model_core::Model *host = new model_core::Model(256,NULL);
  GLOBAL_MODEL.reset(host);
  host->Reset(); // Clears down subscribers etc
  printf("In utest.cpp:gtest_main()\n");
  //::testing::GTEST_FLAG(throw_on_failure) = true;
  ::testing::InitGoogleTest(&argc, argv);

  int ret = 0;
  if (!test_list_file_name.empty()) {
    // read tests from file:
    // * expect each line in file to specify a test filter e.g. 'MyTest.Stuff'
    // * execute each in turn
    // * rudimentary commenting is supported:
    //   - '//' or '#' at start of line to comment a line
    //   - '/*' ('*/') at start of lines to start(end) a block comment
    std::ifstream test_list_file(test_list_file_name);
    if (test_list_file.fail()) {
      std::cout << "ERROR: Failed to open test_list_file "
                << test_list_file_name << std::endl;
      return 1;
    }
    std::cout << "Running tests from file "
              << test_list_file_name << std::endl;
    std::string line;
    bool block_comment = false;
    while (std::getline(test_list_file, line)) {
      // note: KISS - comment delimiters must be at start of line
      if (line.find("//") == 0) continue;
      if (line.find("#") == 0) continue;
      if (line.find("/*") == 0) {
        block_comment = true;
        continue;
      }
      if (line.find("*/") == 0) {
        block_comment = false;
        continue;
      }
      if (block_comment) continue;
      ::testing::GTEST_FLAG(filter) = line.c_str();
      int err = RUN_ALL_TESTS();
      if (err != 0) ret = err;
    }
  } else {
   ret = RUN_ALL_TESTS();
  }
  return ret;
}



// Not sure if the funcs below are needed any more
int common_cpp_f(int x, int &y)
{
    cout << "Common core C++ file" << endl;
    y = x;
    return y;
}


#ifdef __cplusplus
extern "C" {
#endif

int common_c_gtest_main(int argc, char **argv)
{
  return gtest_main(argc, argv);
}

int common_c_f(int x, int *y)
{
  return common_cpp_f(x, *y);
}

// Define these next two funcs to satisfy linker
// Original signature
// unsigned long long sknobs_get_value(char *name, unsigned long long defaultValue) {
// Signature to match BFN modifications (so can dynamically link 'model-team' build .so with DV model
unsigned long long sknobs_get_value_te(const char *name, unsigned long long defaultValue, int type, const char* /* src_file */, int /* src_line */) {
  if      (strncmp(name, "ref_model_wrapper.log_level", 27) == 0)         return 2ull;
  else if (strncmp(name, "ref_model_wrapper.model_debug_flags", 35) == 0) return 0xFF9FFFFFFF1Full;
  else if (strncmp(name, "ref_model_wrapper.model_all_flags", 33) == 0)   return 0ull;
  else if (strncmp(name, "n_stages", 8) == 0)                             return 2ull;
  else                                                                    return defaultValue;
}
int sknobs_exists(char *name) {
  return 0;
}

#ifdef __cplusplus
}
#endif
