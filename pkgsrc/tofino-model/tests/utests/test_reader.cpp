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

#include <utests/test_reader.h>
#include <utests/reader_actions.h>
#include <utests/test_reader_grammar.h>
// for finding the file
#include <utests/filesystem_helper.h>
#include <boost/filesystem/operations.hpp>
#include <regex>
//#include <boost/predef.h> // use this if we ever upgrade to boost 1.55 or later

namespace MODEL_CHIP_TEST_NAMESPACE {


typedef boost::spirit::istream_iterator iterator_type;

struct TestReaderGrammarFileIteratorReaderActions : TestReaderGrammar<iterator_type,ReaderActions>
{
};


TestReader::TestReader() {
    s = new TestReaderGrammarFileIteratorReaderActions;
}
TestReader::~TestReader() {
  delete s;
}

bool TestReader::read_file(boost::filesystem::path p) {
  CommentSkipper<iterator_type> skipper;
  // open file, disable skipping of whitespace
  std::ifstream in(p.c_str());
  if (in.bad()) {
    printf("Test file %s could not be opened\n",p.c_str());
    return false;
  }
  in.unsetf(std::ios::skipws);

  // wrap istream into iterator
  iterator_type iter(in);
  iterator_type end;

  return phrase_parse(iter, end, *s, skipper );
}

bool TestReader::read_file(const char* file_name) {
  std::string exe_path = get_executable_dir();

  boost::filesystem::path p( exe_path );
  p /= file_name;

  return read_file(p);
}

// reads all files matching *.test in dir.
// @return true if dir_name is a directory and all files matching *.test
//         are successfully read, false otherwise.
bool TestReader::read_dir(const char* dir_name) {
  std::string exe_path = get_executable_dir();
  bool res = true;

  using namespace std;
  using namespace boost::filesystem;
  boost::filesystem::path pd( exe_path );
  pd /= dir_name;

  const std::regex filter( ".*\\.test" );

  try {
    if (exists(pd) && is_directory(pd)) {
      directory_iterator it(pd), eod;

      BOOST_FOREACH(path const &pf, std::make_pair(it, eod)) {
        if(is_regular_file(pf) && std::regex_match(pf.string(), filter)) {
          printf("Reading test file %s\n",pf.c_str());
          res &= read_file(pf);
          s->action.clear();
          s->action.reset();
        }
      }
    } else {
      res = false;
    }
  }

  catch (const filesystem_error& ex)
  {
    cout << ex.what() << '\n';
  }
  return res;
}

ReaderActions* TestReader::get_action() {
  return &s->action;
}

void TestReader::set_pipe(int pipe) {
  s->action.set_pipe(pipe);
}
void TestReader::set_stage(int stage) {
  s->action.set_stage(stage);
}

}

