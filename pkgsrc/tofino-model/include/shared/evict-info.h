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

#ifndef _SHARED_EVICT_INFO_
#define _SHARED_EVICT_INFO_

#include <common/rmt-assert.h>
#include <mau-defs.h>


namespace MODEL_CHIP_NAMESPACE {

  class EvictInfo {
    static constexpr int kNumStatsAlus = MauDefs::kNumStatsAlus;  

 public:
    EvictInfo()  {
      for (int i = 0; i < kNumStatsAlus; i++) {
        addr_vals_[i] = Address::invalid();
        data_vals_[i] = UINT64_C(0);
      }
    }
    ~EvictInfo() { }

    inline uint32_t addr(int alu) const {
      RMT_ASSERT((alu >= 0) && (alu < kNumStatsAlus));
      return addr_vals_[alu];
    }
    inline uint64_t data(int alu) const {
      RMT_ASSERT((alu >= 0) && (alu < kNumStatsAlus));
      return data_vals_[alu];
    }
    inline void set_evictinfo(const int alu, const uint32_t addr, const uint64_t data) {
      RMT_ASSERT((alu >= 0) && (alu < kNumStatsAlus));
      addr_vals_[alu] = addr;
      data_vals_[alu] = data;
    }

 private:
    std::array< uint32_t, kNumStatsAlus > addr_vals_;
    std::array< uint64_t, kNumStatsAlus > data_vals_;
  };
}

#endif // _SHARED_EVICT_INFO_
