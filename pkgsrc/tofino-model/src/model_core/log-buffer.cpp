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

#include <cstdarg>
#include <common/rmt-assert.h>
#include <common/rmt-util.h>
#include <model_core/log-buffer.h>

namespace model_core {

/**
 * Instantiates a new LogBuffer capable of holding up to size characters
 * (excluding the termination '\0' character). If size is <0 the buffer will
 * default to size 0.
 * @param size
 */
LogBuffer::LogBuffer(int size) :
    buf_size_((size > 0) ? (size + 1) : 1) {  // add space for the '\0'
  buf_ = (char*)malloc(buf_size_);
  RMT_ASSERT_NOT_NULL(buf_);
  Reset();
}

LogBuffer::LogBuffer(int size, const char* fmt, ...) :
LogBuffer(size) {
  va_list args;
  va_start(args, fmt);
  AppendV(fmt, args);
  va_end(args);
}

LogBuffer::~LogBuffer() {
  if (nullptr != buf_) free((void*)buf_);
  cursor_ = 0;
  buf_size_ = 0;
  buf_ = nullptr;
}

void LogBuffer::Reset() {
  cursor_ = 0;
  buf_[0] = '\0';
}

/**
 * Append given formatted string to the buffer. If the formatted string is
 * longer than the remaining space in the buffer then the string will be
 * truncated to fit into the remaining space. If the buffer is already full
 * then an error will be raised.
 * @param fmt A format statement
 * @param args Parameters for the format statement
 * @return true if the whole formatted statement was written to the buffer,
 *    false if the formatted statement was truncated.
 */
bool LogBuffer::AppendV(const char* fmt, va_list args) {
  int remaining = buf_size_ - cursor_;
  RMT_ASSERT(remaining > 0);
  int written = vsnprintf(buf_ + cursor_, remaining, fmt, args);
  cursor_ += written;
  return written < remaining;
}

bool LogBuffer::Append(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  bool res = AppendV(fmt, args);
  va_end(args);
  return res;
}


/**
 * @return a pointer to the char buffer
 */
char* LogBuffer::GetBuf() { return buf_; }

/**
 * @return a pointer to the char buffer
 *    and null it out. Caller frees
 */
char* LogBuffer::ExtractBuf() {
  char *buf = buf_;
  cursor_ = 0;
  buf_size_ = 0;
  buf_ = nullptr;
  return buf;
}

}
