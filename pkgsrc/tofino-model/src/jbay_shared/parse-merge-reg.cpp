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

#include <parse-merge-reg.h>

namespace MODEL_CHIP_NAMESPACE {

bool ParseMergeReg::tm_status_input(uint32_t *val_msb, uint32_t *val_lsb) {
  // val_lsb is only relevant for WIP
  if (val_msb == nullptr) return false;
  bool ok = false;
  spinlock_.lock();
  if (tm_status_cnt_ > 0) {
    uint32_t tmpval = tm_status_[tm_status_rd_][0];
    if (nullptr != val_lsb) *val_lsb = tm_status_[tm_status_rd_][1];
    if ((tmpval & 0x80000000) == 0u) {
      // Update ping-pong MSB val_msb to 1
      tm_status_[tm_status_rd_][0] = tmpval | 0x80000000;
      // Leave cnt/rd pointer as they are
    } else {
      // Decrement cnt/Increment rd
      tm_status_cnt_--;
      tm_status_rd_++;
      // And maybe wrap rd
      if (tm_status_rd_ == kTmStatusFifoSz) tm_status_rd_ = 0;
    }
    *val_msb = tmpval;
    ok = true;
  }
  spinlock_.unlock();
  return ok;
}

bool ParseMergeReg::set_tm_status_input(uint32_t val_msb, uint32_t val_lsb) {
  // val_lsb is only relevant for WIP
  bool ok = false;
  spinlock_.lock();
  if (tm_status_cnt_ < kTmStatusFifoSz) {
    // Buffer val_msb with ping-pong MSB val_msb == 0
    int buf_index = (tm_status_rd_ + tm_status_cnt_) % kTmStatusFifoSz;
    tm_status_[buf_index][0] = (val_msb & 0x7FFFFFFF);
    tm_status_[buf_index][1] = val_lsb;
    tm_status_cnt_++;
    ok = true;
  }
  spinlock_.unlock();
  return ok;
}

}
