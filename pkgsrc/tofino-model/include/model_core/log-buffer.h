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

#ifndef _MODEL_CORE_LOG_BUFFER_H_
#define _MODEL_CORE_LOG_BUFFER_H_

#include <common/rmt-util.h>

namespace model_core {

// XXX: LogBuffer encapsulates a char buffer of defined size and provides
// a method to append to that buffer without overflowing.
class LogBuffer {
 public:
  explicit LogBuffer(int size);
  LogBuffer(int size, const char* fmt, ...);
  ~LogBuffer();
  void Reset();
  bool Append(const char* fmt, ...);
  char* GetBuf();
  char* ExtractBuf();

 private:
    bool AppendV(const char* fmt, va_list args);
    DISALLOW_COPY_AND_ASSIGN(LogBuffer);
    int cursor_ = 0;
    int buf_size_;
    char *buf_;
};

#define LOG_APPEND_CHECK(buf_ptr, fmt, ...) do {     \
    if(buf_ptr) {                                    \
      buf_ptr->Append(fmt, __VA_ARGS__);             \
    }                                                \
} while (0)                                         

//For when only 1 argument is used, 
//usually a string with no formatting
#define LOG_APPEND_ONE(buf_ptr, log) do {            \
    if(buf_ptr) {                                    \
      buf_ptr->Append(log);                          \
    }                                                \
} while (0)                                         

}
#endif //_MODEL_CORE_LOG_BUFFER_H_
