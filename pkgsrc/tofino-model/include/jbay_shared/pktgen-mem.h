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

#ifndef __JBAY_SHARED_PKTGEN_MEM_H
#define __JBAY_SHARED_PKTGEN_MEM_H

#include <indirect-addressing.h>
#include <mutex>

namespace MODEL_CHIP_NAMESPACE {


///////////////////
// Buffer memory
template <uint64_t ENTRIES, uint64_t ELEMENT_SIZE, uint64_t ADDRESS_OFFSET>
class PktGenMem : public model_core::RegisterBlockIndirect<RegisterCallback> {

 public:
  PktGenMem(int chip, int pipe, const char* name) :
      RegisterBlockIndirect(chip,
                            ((pipe*BFN_MEM_PIPE_ESZ) + ADDRESS_OFFSET) >> 4, // Convert to word address
                            ENTRIES,
                            false,
                            nullptr,
                            nullptr,
                            name),
      buf_{}
  {
  }

  // Return into a linearized array
  void get_val(uint8_t* buf, uint16_t st_offset, uint16_t n_entries) {
    uint8_t index = 0;
    uint16_t o_index = 0;
    RMT_ASSERT(st_offset < ENTRIES);
    //RMT_ASSERT((n_entries % ELEMENT_SIZE) == 0);
    std::lock_guard<std::mutex> lck(mutex_);
    while (n_entries) {
      if (index == ELEMENT_SIZE) {
        index = 0;
        st_offset ++;
      }
      buf[o_index++] = buf_[st_offset][index];
      --n_entries;
      index++;
    }
  }

 private:
  bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
    uint8_t index = 0;
    RMT_ASSERT(offset < ENTRIES);
    std::lock_guard<std::mutex> lck(mutex_);
    while (index < ELEMENT_SIZE) {
      if (index < 8) {
        buf_[offset][index] = data0 & 0xFFu;
        data0 >>= 8u;
      } else {
        buf_[offset][index] = data1 & 0xFFu;
        data1 >>= 8u;
      }
      index ++;
    }
    return true;
  }

  bool read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const {
    int8_t index = ELEMENT_SIZE - 1;
    RMT_ASSERT(offset < ENTRIES);
    *data0 = 0;
    *data1 = 0;
    while (index >= 0) {
      if (index < 8) {
        *data0 <<= 8u;
        *data0 |= static_cast<uint64_t>(buf_[offset][index]);
      } else {
        *data1 <<= 8u;
        *data1 |= static_cast<uint64_t>(buf_[offset][index]);
      }
      index --;
    }
    return true;
  }

  std::string to_string(bool print_zeros, std::string indent_string) const { return ""; }
  std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const { return ""; }
  std::mutex mutex_;
  uint8_t buf_[ENTRIES][ELEMENT_SIZE];
};



}; // namespace MODEL_CHIP_NAMESPACE {

#endif
