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

#ifndef _SHARED_MAU_STASH_
#define _SHARED_MAU_STASH_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <rmt-log.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <phv.h>
#include <mau-result-bus.h>
#include <mau-stash-reg.h>

namespace MODEL_CHIP_NAMESPACE {

  class Mau;
  class MauSramRow;

  class MauStash : public MauObject {
    static constexpr int kEntries             = MauDefs::kStashEntries;
    static constexpr int kVersionWidth        = MauDefs::kVersionBits;
    static constexpr int kMatchDataWidth      = MauDefs::kSramWidth;
    static constexpr int kAddressWidth        = MauDefs::kSramAddressWidth;
    static constexpr int kWholeHashWidth      = MauDefs::kHashOutputWidth;
    static constexpr int kMaskedEqualsWidth   = MauDefs::kMaskedEqualsWidth;
    static constexpr int kMaskedEqualsStart   = kWholeHashWidth - kMaskedEqualsWidth;
    static constexpr int kMatchOutputBusWidth = MauDefs::kMatchOutputBusWidth;

 public:
    MauStash(RmtObjectManager *om, int pipeIndex, int mauIndex,
                    int rowIndex, Mau *mau, MauSramRow *row);
    ~MauStash();

    /** lookup the phv in a stash half and return whether it hit and, if so, the hit mask
     *    mask will have one bit set per entry hit (as stash half has 4 entries all hit = 0xf)
     */
    void lookup(Phv *phv, int which_stash, uint32_t *hit_mask);
    MauSramRow *row()  const { return row_; }

    uint8_t get_logical_table(int which_stash) { 
      return mau_stash_reg_.get_logical_table(which_stash);
    }
    void inhibit(int which_stash, const uint32_t inhibit_addr) {
      RMT_ASSERT((which_stash == 0) || (which_stash == 1));
      inhibit_array_[which_stash] = inhibit_addr;
    }
    void uninhibit(int which_stash) {
      inhibit(which_stash, 0xFFFFFFFF);
    }
    bool hit_inhibited(int which_stash, const uint32_t hit_addr) {
      RMT_ASSERT((which_stash == 0) || (which_stash == 1));
      return (inhibit_array_[which_stash] == hit_addr);
    }


    // called by MauStashReg when data entry changes
    //void DataUpdate(int index,int word);

    void entry_update(uint32_t a1,uint32_t a0,uint32_t v);
    void mask_update(uint32_t a1,uint32_t a0,uint32_t v);

    void set_match_output(Phv *phv, const int which_bus, const int index, const int which_stash);

    bool get_match_result_bus_select(int which_stash,int which_bus) {
      return mau_stash_reg_.get_match_result_bus_select(which_stash,which_bus);
    }

 private:
    MauStashReg mau_stash_reg_;
    MauSramRow* row_;

    // These are one for each of the two Stashes in the row
    std::array<BitVector<MauDefs::kSramWidth>,2>  mask_array_{};
    std::array<uint32_t,2>                        inhibit_array_{};
    std::array<BitVector<MauDefs::kSramWidth>,kEntries> entries_{};

  };
}

#endif // ifndef _SHARED_MAU_STASH_
