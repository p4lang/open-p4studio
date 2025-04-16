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

#ifndef _SHARED_MAU_TABLE_COUNTERS_
#define _SHARED_MAU_TABLE_COUNTERS_

#include <vector>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <mau-addr-dist.h>
#include <mau-lookup-result.h>

#include <register_includes/mau_table_counter_array2_mutable.h>
#include <register_includes/mau_table_counter_ctl_array.h>
#include <register_includes/mau_table_counter_clear_mutable.h>

namespace MODEL_CHIP_NAMESPACE {

  class MauTableCounters : public MauObject {
    static constexpr int      kType = RmtTypes::kRmtTypeMauTableCounters;
    static constexpr int      kTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int      kNumAlus = MauDefs::kNumAlus;
    static constexpr int      kSramAddressWidth  = MauDefs::kSramAddressWidth;
    static constexpr int      kSramEntries = 1<<kSramAddressWidth;
    static constexpr uint32_t kTableCntrMask = 0xFFFFFFFFu;
    static_assert( (kTables <= 16), "LTMask must fit in uint16_t");


 public:
    MauTableCounters(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    ~MauTableCounters();

    uint16_t lt_with_tblcounters();
    uint16_t lt_with_gwcounters();
    uint16_t lt_with_counters();
    bool evaluate_table_counters();
    void maybe_increment_table_counter(int lt, const MauLookupResult &res);


 private:
    // Register write callbacks
    void table_counter_ctl_callback(uint32_t i);
    void table_counter_clear_callback();


    // Zeroise MAU table counter
    inline void reset_table_cntr(int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kTables));
      spinlock();
      mau_table_counter_array_.mau_table_counter(lt,0,0u);
      spinunlock();
    }
    // Increment MAU table counter
    inline void incr_table_cntr(int lt, int inc=1) {
      RMT_ASSERT((lt >= 0) && (lt < kTables));
      spinlock();
      uint32_t v = mau_table_counter_array_.mau_table_counter(lt,0);
      v += inc;
      v &= kTableCntrMask;
      mau_table_counter_array_.mau_table_counter(lt,0,v);
      spinunlock();
    }
    // Get type of MAU table counter
    CntrType get_type_table_cntr(int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kTables));
      uint32_t reg = mau_table_counter_ctl_.mau_table_counter_ctl(lt / 8);
      uint32_t ctl = (reg >> ((lt % 8) * 3)) & 0x7;
      switch (ctl) {
        case 0: case 6: case 7: return CntrType::kDisabled;
        case 1: return CntrType::kTableMiss;
        case 2: return CntrType::kTableHit;
        case 3: return CntrType::kGatewayMiss;
        case 4: return CntrType::kGatewayHit;
        case 5: return CntrType::kGatewayInhibit;
        default: RMT_ASSERT(0); break;
      }
    }

 private:
    bool                                                  ctor_running_;
    uint16_t                                              logical_tables_with_tblcounters_;
    uint16_t                                              logical_tables_with_gwcounters_;
    register_classes::MauTableCounterArray2Mutable        mau_table_counter_array_;
    register_classes::MauTableCounterCtlArray             mau_table_counter_ctl_;
    register_classes::MauTableCounterClearMutable         mau_table_counter_clear_;
  }; // MauCounters

}

#endif // _SHARED_MAU_COUNTERS_
