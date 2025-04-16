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

#ifndef _SHARED_INDIRECT_ADDRESSING_
#define _SHARED_INDIRECT_ADDRESSING_

#include <cinttypes>
#include <cstddef>  
#include <common/rmt-assert.h>
#include <rmt-defs.h>
#include <mem_utils.h>

namespace MODEL_CHIP_NAMESPACE {

  // for a number with 1 bit set in to get the bit down to bit 0: eg 0x40 -> 6
  constexpr int calculate_right_shift(const uint64_t addr) {
    return ((addr==0) || ((addr&1) && (addr!=1))) ?
        -9999 : // something went wrong, either no bits were set, or more than one
        (addr==1) ? 0 : 1 + calculate_right_shift(addr>>1);
  }

  static constexpr uint64_t kPipeStartAddress = BFN_MEM_PIPE(address) / 16;
  static constexpr uint64_t kPipeElementSize = BFN_MEM_PIPE_ESZ / 16;
  static constexpr int      kPipeArrayCount  = BFN_MEM_PIPE_CNT;
  static constexpr uint64_t kPipeTotalSize   = kPipeArrayCount * kPipeElementSize;
  static constexpr uint64_t kPipeEndAddress  = kPipeStartAddress + kPipeTotalSize - 1;

  static constexpr int      kIndPipeShift    = calculate_right_shift(kPipeElementSize);
  static_assert( (kIndPipeShift >= 0), "Something went wrong in calculate_right_shift!");
  // kIndPipeBits actually 3 on Tofino but PipeId[2] always 0 so just say width is 2 
  static constexpr int      kIndPipeBits     = 2;
  static_assert( (kIndPipeBits >= RmtDefs::kPipeBits),
                 "Insufficient pipe bits in indirect addresses!");
  static constexpr int      kIndPipeMask     = (1<<kIndPipeBits)-1;

  static constexpr uint64_t kStageElementSize = BFN_MEM_PIPE_MAU_ESZ / 16;
  static constexpr int      kStageArrayCount  = BFN_MEM_PIPE_MAU_CNT;
  static constexpr int      kIndStageShift    = calculate_right_shift(kStageElementSize);
  static_assert( (kIndStageShift >= 0), "Something went wrong in calculate_right_shift!");
  static_assert( (kIndPipeShift > kIndStageShift), "Invalid pipe/stage shifts detected");
  
  static constexpr int      kIndStageCalcBits = kIndPipeShift - kIndStageShift;
  static constexpr int      kIndStageDefnBits = RmtDefs::kStageBits;
  static constexpr int      kIndStageBits     = kIndStageDefnBits;

  static_assert( (kIndStageCalcBits == kIndStageDefnBits), "Calc/defn stage bits differ!");
  static_assert( (kIndStageBits >= kIndStageDefnBits),
                 "Insufficient stage bits in indirect addresses!");
  static constexpr int      kIndStageMask     = (1<<kIndStageBits)-1;

  inline bool indirect_is_in_pipe_space(uint64_t addr) {
    return (addr>kPipeStartAddress) && (addr<kPipeEndAddress);
  }
  inline int  indirect_get_pipe(uint64_t addr)      {
    return (addr >> kIndPipeShift) & kIndPipeMask;
  }
  inline int  indirect_get_stage(uint64_t addr)     {
    return (addr >> kIndStageShift) & kIndStageMask;
  }

}

#endif
