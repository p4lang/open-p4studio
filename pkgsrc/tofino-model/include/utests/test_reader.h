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

#ifndef _UTESTS_TEST_READER_
#define _UTESTS_TEST_READER_
#include "test_namespace.h"
#include <boost/filesystem/path.hpp>

namespace MODEL_CHIP_TEST_NAMESPACE {

struct TestReaderGrammarFileIteratorReaderActions;
struct ReaderActions;

class TestReader {
 public:
  TestReader();
  ~TestReader();

  bool read_file(const char* file_name);
  bool read_dir(const char* dir_name);
  ReaderActions* get_action();
  void set_pipe(int pipe);
  void set_stage(int stage);
 private:
  bool read_file(boost::filesystem::path p);
  //TestReaderGrammar<iterator_type,ReaderActions> *s;
  TestReaderGrammarFileIteratorReaderActions *s;
};

}


#endif // _UTESTS_TEST_READER_
