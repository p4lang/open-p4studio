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

#ifndef _SHARED_NXT_TAB_
#define _SHARED_NXT_TAB_

#include <rmt-defs.h>

namespace MODEL_CHIP_NAMESPACE {

  class NxtTab {

 private:
    static constexpr int  kLtOff = 0;
    static constexpr int  kLtBits = 4;
    static constexpr int  kLtMask = (1 << kLtBits) -1;
    static constexpr int  kLtMax = (1 << kLtBits);
    
    static constexpr int  kStOff = kLtOff + kLtBits;
    static constexpr int  kStBits = RmtDefs::kStageBits; // 4 or 5
    static constexpr int  kStMask = RmtDefs::kStageMask;
    static constexpr int  kStMax = (1 << kStBits);

    // Figure out absolute max val NxtTab can have for chip
    static constexpr int  kLogicalTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int  kStages = RmtDefs::kStagesMax;
    static constexpr int  kNxtTabMin = 0;
    static constexpr int  kNxtTabMax = ((kStages-1) << kStOff) | ((kLogicalTables-1) << kLtOff);

    static constexpr int  kNxtTabMask = (kStMask << kStOff) | (kLtMask << kLtOff);
    static constexpr int  kInvalOff   = kStOff + kStBits;
    
    // Use fixed value (0x1FF or 0x3FF) as the value indicating invalid next_table
    static constexpr int  kInvalNxtTab = (1 << kInvalOff) | kNxtTabMask;

    static_assert( (RmtDefs::kPipesMax <= RmtDefs::kPipesMaxEver), "Too many Pipes");
    static_assert( (RmtDefs::kStagesMax <= RmtDefs::kStagesMaxEver), "Too many MAUs");
    static_assert( (kStages <= kStMax), "Too many MAUs");
    static_assert( (kLtBits + kStBits < 16), "NxtTab must fit in uint16_t" );
    static_assert( (kLogicalTables <= kLtMax), "Too many tables");
    static_assert( (kInvalNxtTab > kNxtTabMax), "kInvalNxtTab should exceed kNxtTabMax" );


 public:
    static inline bool next_table_ok(int nxt_table_id) {
      return ((nxt_table_id >= kNxtTabMin) && (nxt_table_id <= kNxtTabMax));
    }
    static inline int which_table(int nxt_table_id) {
      return next_table_ok(nxt_table_id) ?(nxt_table_id >> kLtOff) & kLtMask :-1;
    }
    static inline int which_mau(int nxt_table_id) {
      return next_table_ok(nxt_table_id) ?(nxt_table_id >> kStOff) & kStMask :-1;
    }
    static inline int make_next_table(int mau, int table) {
      if ((mau < 0)   || (mau   >= kStages) ||
          (table < 0) || (table >= kLogicalTables)) return kInvalNxtTab;
      return ((mau & kStMask) << kStOff) | ((table & kLtMask) << kLtOff);
    }
    static inline int next_table_mask()  { return kNxtTabMask;  }
    static inline int inval_next_table() { return kInvalNxtTab; }
  };
  
}
#endif // _SHARED_NXT_TAB_
