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

#ifndef _TOFINOXX_MAU_CHIP_TCAM_REG_
#define _TOFINOXX_MAU_CHIP_TCAM_REG_

#include <rmt-defs.h>
#include <mau-defs.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauTcam;

  class MauChipTcamReg {

 public:
    static constexpr int      kTcamAddressWidth  = MauDefs::kTcamAddressWidth;
    static constexpr int      kTcamEntries = 1<<kTcamAddressWidth;
    static constexpr uint32_t kTcamAddressMask = (1u<<kTcamAddressWidth)-1;

    MauChipTcamReg(int chipIndex, int pipeIndex, int mauIndex,
                   int rowIndex, int colIndex, MauTcam *tcam);
    ~MauChipTcamReg();

    bool get_ghost()  { return false; }

    int  get_priority();

    bool drives_ltcam(int ltcam, uint8_t powered_ltcams) {
      return (((powered_ltcams >> ltcam) & 1) == 1);
    }
    bool get_ltcam_result_info(int ltcam, int *start_pos, int *n_entries, bool *bitmap) {
      RMT_ASSERT((start_pos != nullptr) && (n_entries != nullptr) && (bitmap != nullptr));
      *start_pos = 0;
      *n_entries = 1 << MauDefs::kTcamAddressWidth;
      *bitmap = GLOBAL_FALSE;
      return false; // False => no multi-LTCAM capability
    }
    uint32_t compute_bitmap_result(int start, int entries, BitVector<kTcamEntries> *bv) {
      RMT_ASSERT(0);
      return 0u;
    }
    uint32_t get_hit_address(int ltcam, uint8_t vpn, uint32_t hit_addr) {
      return (static_cast<uint32_t>(vpn) << kTcamAddressWidth) | (hit_addr & kTcamAddressMask);
    }

   private:
    MauTcam *tcam_;
  };
}

#endif // _TOFINOXX_MAU_CHIP_TCAM_REG_
