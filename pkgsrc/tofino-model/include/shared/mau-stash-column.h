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

#ifndef _SHARED_MAU_STASH_COLUMN_
#define _SHARED_MAU_STASH_COLUMN_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-object.h>
#include <mau-lookup-result.h>
#include <mau-stash-column-reg.h>
#include <mau-stash.h>
#include <phv.h>


namespace MODEL_CHIP_NAMESPACE {
  
  class MauStashColumn : public MauObject {
    
 public:
    //static constexpr int  kType = RmtTypes::kRmtTypeMauStashColumn;
    static constexpr int  kStashRows = MauDefs::kSramRowsPerMau;
    static constexpr int  kLogicalTablesPerMau = MauDefs::kLogicalTablesPerMau;
    static constexpr int  kStashEntries        = MauDefs::kStashEntries;
    
    MauStashColumn(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    virtual ~MauStashColumn();

    inline MauStash *stash_lookup(int row)             { return stashes_[row]; }
    inline void     stash_set(int row, MauStash *stash) {
      RMT_ASSERT((row >= 0) && (row < kStashRows));
      stashes_[row] = stash;
    }

    bool lookup(Phv *phv, int logicalTableIndex, MauLookupResult *result);

    static bool HasOnlyOneBitSet (unsigned int x) {
      return ((x != 0) && !(x & (x - 1)));
    }

    bool BusIsInLogicalTable(int row,int bus,int logicalTableIndex) {
      uint8_t t;
      bool enabled;
      mau_stash_column_reg_.get_row_bus_logical_table(row,bus,&enabled,&t);
      if (enabled && (t == logicalTableIndex)) {
        return true;
      }
      return false;
    }

    void FindWhichStashesDriveBus(int row,int bus,bool* stash_0_drives_bus,bool* stash_1_drives_bus);

 private:
    std::array<MauStash*,kStashRows>            stashes_{};
    CacheId                                     lookup_cache_id_;
    MauLookupResult                             cached_result_;
    // maybe store this and keep it updated. TODO: decide
    //std::array<uint16_t,kLogicalTablesPerMau>   stash_rows_used_;
    MauStashColumnReg                            mau_stash_column_reg_;
  };
}
#endif // _SHARED_MAU_STASH_COLUMN_
