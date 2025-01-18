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

// Global vars - C++

#include <cstdio>
#include <cassert>
#include <stdexcept>
#include <common/rmt-features.h>


// Global vars - C linkage
extern "C" {


int GLOBAL_RMT_ERROR = 0;

// These globals control the ABORT_ERROR, THROW_ERROR
// and RMT_ASSERT macros defined in rmt-assert.h
//
int GLOBAL_ABORT_ON_ERROR  = FEATURE_ABORT_ON_ERROR;
int GLOBAL_THROW_ON_ERROR  = FEATURE_THROW_ON_ERROR;
int GLOBAL_THROW_ON_ASSERT = FEATURE_THROW_ON_ASSERT;
int GLOBAL_SHUTDOWN        = 0;

FILE *RMT_STDOUT = stdout;
FILE *RMT_STDERR = stderr;

// XXX: Klocwork static analysis flags unreachable code where
// conditional branches cannot be executed due to chip-specific implementation
// e.g. a particular chip may always return false for some register setting,
// where other chips return a dynamic value. To avoid these issues being
// flagged, we define GLOBAL_FALSE and GLOBAL_TRUE in this compilation unit and
// then use them in condition statements to trick the static analysis tool into
// thinking that the branch might be executed. However, these values should
// never change.
bool GLOBAL_FALSE = false;
bool GLOBAL_TRUE = true;
// ditto - used to persuade klocwork that 0 might not always be 0
int GLOBAL_ZERO = 0;
void* GLOBAL_NULLPTR = nullptr;

// These can be called direct - or from macros in rmt-assert.h
//
void rmt_throw_error_fn(int err, const char *file, int line, const char *func) {
  if (err == 0) return;
  if (GLOBAL_SHUTDOWN != 0) return;
  char buf[512];
  const char *str = (GLOBAL_THROW_ON_ERROR != 1) ?" WOULD have been" :"";
  snprintf(buf, 512,
           "ERROR: %s:%d (%s): error %d%s thrown.",
           file, line, func, err, str);
  fprintf(RMT_STDOUT, "%s\n", buf);
  fprintf(RMT_STDERR, "%s\n", buf);
  fflush(RMT_STDOUT);
  fflush(RMT_STDERR);
  if (GLOBAL_THROW_ON_ERROR != 1) return;
  GLOBAL_RMT_ERROR = err;
  throw std::runtime_error(buf);
}

void rmt_throw_assert_fn(bool expr, const char *file, int line, const char *func,
                         const char *exprstr) {
  if (expr) return;
  if (GLOBAL_SHUTDOWN != 0) return;
  char buf[512];
  fflush(RMT_STDOUT);
  snprintf(buf, 512,
           "ERROR ASSERT: %s:%d (%s): assert condition '%s' failed.",
           file, line, func, exprstr);
  fprintf(RMT_STDOUT, "%s\n", buf);
  fprintf(RMT_STDERR, "%s\n", buf);
  fflush(RMT_STDOUT);
  fflush(RMT_STDERR);
  if (GLOBAL_THROW_ON_ASSERT != 1) return;
  GLOBAL_RMT_ERROR = -1;
  throw std::runtime_error(buf);
}

void rmt_throw_nullptr_error_fn(const char *file,
                                int line,
                                const char *func,
                                const char *param) {
  char buf[512];
  snprintf(buf, 512,
           "ERROR (NULLPTR): %s:%d (%s): '%s' is null.",
           file, line, func, param);
  throw std::runtime_error(buf);
}

/**
 * Set the file to which rmt_throw_* functions will print stdout messages.
 * For testing purposes.
 *
 * @param file_ptr a file ptr; if nullptr then subsequent messages will be
 *        printed to stdout.
 */
void rmt_set_stdout(FILE *file_ptr) {
  (nullptr == file_ptr) ? RMT_STDOUT = stdout : RMT_STDOUT = file_ptr;
}

/**
 * Set the file to which rmt_throw_* functions will print stderr messages.
 * For testing purposes.
 *
 * @param file_ptr a file ptr; if nullptr then subsequent messages will be
 *        printed to stderr.
 */
void rmt_set_stderr(FILE *file_ptr) {
  (nullptr == file_ptr) ? RMT_STDERR = stderr : RMT_STDERR = file_ptr;
}

}
