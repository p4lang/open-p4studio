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

#ifndef _SHARED_MAU_SRAM_COLUMN_
#define _SHARED_MAU_SRAM_COLUMN_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-object.h>
#include <mau-lookup-result.h>
#include <mau-sram-column-reg.h>
#include <mau-sram.h>
#include <phv.h>


namespace MODEL_CHIP_NAMESPACE {
  
  class MauSramColumn : public MauObject {
    
 public:
    static bool kRelaxMultiHitCheck; // Defined in rmt-config.cpp
    static bool kRelaxMultiColumnHitCheck;

    static constexpr int  kType = RmtTypes::kRmtTypeMauSramColumn;
    static constexpr bool kEvaluateAllDefault = RmtDefs::kEvaluateAllDefault;
    static constexpr int  kSramRows = MauDefs::kSramRowsPerMau;
    static constexpr int  kLogicalTablesPerMau = MauDefs::kLogicalTablesPerMau;
    static constexpr int  kHitBits = MauDefs::kHitBitsPerRow;
    static constexpr int  kHits = 1 << kHitBits;
    static constexpr int  kHitMask = (1 << kHitBits) - 1;
    static constexpr int  kOutBits = MauDefs::kOutBitsPerRow;
    static constexpr int  kOuts = 1 << kOutBits;
    static constexpr int  kOutMask = (1 << kOutBits) - 1;
    static constexpr int  kHitmapInputs  = MauDefs::kSramRowsPerMau * kHits;
    static constexpr int  kHitmapOutputs = MauDefs::kSramRowsPerMau * kOuts;
    
    MauSramColumn(RmtObjectManager *om, int pipeIndex, int mauIndex, int columnIndex, Mau *mau);
    virtual ~MauSramColumn();

    inline int      column_index()             const { return column_index_; }
    inline MauSram *sram_lookup(int row)             { return srams_[row]; }
    inline void     sram_set(int row, MauSram *sram) {
      RMT_ASSERT((row >= 0) && (row < kSramRows));
      srams_[row] = sram;
    }

    inline uint8_t  hit_make(uint8_t row, uint8_t which) const {
      return (row << kHitBits) | (which & kHitMask);
    }
    inline uint8_t  hit_get_row(uint8_t hit)    const { return hit >> kHitBits; }
    inline uint8_t  hit_get_which(uint8_t hit)  const { return hit & kHitMask; }
    inline uint8_t  out_make(uint8_t row, uint8_t which) const {
      return (row << kOutBits) | (which & kOutMask);
    }
    inline uint8_t  out_get_row(uint8_t out)    const { return out >> kOutBits; }
    inline uint8_t  out_get_which(uint8_t out)  const { return out & kOutMask; }
    
    inline bool hit_check(uint8_t mask, uint8_t hit)  const {
      return ((mask & (1<<hit)) != 0);
    }

    inline bool evaluate_all(int tableIndex)   const { return evaluate_all_[tableIndex]; }
    inline void set_evaluate_all(int tableIndex, bool tf) {
      RMT_ASSERT((tableIndex >= 0) && (tableIndex < MauDefs::kLogicalTablesPerMau));
      evaluate_all_[tableIndex] = tf;
    }

        
    bool lookup(Phv *phv, int logicalTableIndex, MauLookupResult *result);
    void reset_resources();

    void remove_logical_table(int row, int logtab);
    void add_logical_table(int row, int logtab);
    void update_logical_tables(int row, uint16_t new_logtabs, uint16_t old_logtabs);
    void update_match_sram(MauSram *sram, uint16_t new_logtabs, uint16_t old_logtabs);
    void add_match_sram(MauSram *sram, uint16_t new_logtabs);
    void remove_match_sram(MauSram *sram, uint16_t old_logtabs);

    int      get_logical_table_for_row(uint8_t row, int result_bus, int which);
    uint16_t get_all_logical_tables_for_row(uint8_t row, int result_buses, int which);
    uint16_t get_all_logical_tables_for_row_nxtab(uint8_t row, int result_buses, int which);
    uint16_t get_all_xm_logical_tables_for_row(uint8_t row);
    uint16_t get_all_tm_logical_tables_for_row(uint8_t row);

    void update_sram_logical_table_mappings();
    void update_maps();
    void maps_changed();

    
 private:
    void update_hitmap();
    void update_tablemap();
    void handle_xtra_hits(int hit_row, int hit_index, int which_hit,
                          MauLookupResult *result);
    bool handle_inner_hit(int lt, int sram_row, int hit_index, int which_hit,
                          std::array<uint32_t,kHitmapOutputs>& local_reverse_hitmap,
                          MauLookupResult *result);
    bool handle_outer_hit(int lt, int sram_row, int hit_index, int which_hit,
                          std::array<uint32_t,kHitmapOutputs>& local_reverse_hitmap,
                          MauLookupResult *result);
    bool lookup_internal(Phv *phv, int logicalTableIndex, MauLookupResult *result);

 private:
    int                                         column_index_;
    std::array<MauSram*,kSramRows>              srams_;    
    int                                         curr_map_seq_;
    int                                         pending_map_seq_;
    CacheId                                     lookup_cache_id_;
    MauLookupResult                             cached_result_;
    std::array<bool,kLogicalTablesPerMau>       evaluate_all_;
    std::array<uint16_t,kLogicalTablesPerMau>   sram_rows_used_;
    std::array<uint32_t,kHitmapOutputs>         reverse_hitmap_;
    std::array<uint8_t,kHitmapInputs>           hitmap_xbar_;
    MauSramColumnReg                            mau_sram_column_reg_;
  };
}
#endif // _SHARED_MAU_SRAM_COLUMN_
