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

#ifndef _SHARED_MAU_LOGICAL_TCAM_
#define _SHARED_MAU_LOGICAL_TCAM_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-object.h>
#include <mau-lookup-result.h>
#include <mau-tcam.h>
#include <mau-logical-tcam-col.h>
#include <mau-logical-tcam-reg.h>
#include <address.h>
#include <phv.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauSram;

  class MauLogicalTcam : public MauObject {
 public:
    static bool kRelaxTcamCheck; // Defined in rmt-config.cpp

    static constexpr int  kType = RmtTypes::kRmtTypeMauLogicalTcam;
    static constexpr int  kTcamsPerMau = MauDefs::kTcamsPerMau;
    static constexpr int  kSramsPerMau = MauDefs::kSramsPerMau;
    static constexpr int  kTcamColumnsPerMau = MauDefs::kTcamColumnsPerMau;
    static constexpr int  kTcamRowsPerMau = MauDefs::kTcamRowsPerMau;
    static constexpr int  kTcamRowsPerHalf = kTcamRowsPerMau/2;
    static constexpr int  kTcamWidth = MauDefs::kTcamWidth;
    static constexpr int  kTcamAddressWidth  = MauDefs::kTcamAddressWidth;
    static constexpr int  kTcamEntries = 1<<kTcamAddressWidth;
    static constexpr int  kLogicalTablesPerMau = MauDefs::kLogicalTablesPerMau;
    static constexpr int  kTindOutputBusesPerMau = MauDefs::kTindOutputBusesPerMau;
    static constexpr int  kTindOutputBusesPerRow = MauDefs::kTindOutputBusesPerRow;
    static constexpr int  kTindOutputBusWidth = MauDefs::kTindOutputBusWidth;

    static inline bool tcam_sort_func(MauTcam *t1, MauTcam *t2) {
      return (t1->get_priority() > t2->get_priority());
    }


    MauLogicalTcam(RmtObjectManager *om, int pipeIndex, int mauIndex, int ltcamIndex, Mau *mau);
    virtual ~MauLogicalTcam();

    inline int      ltcam_index()       const { return ltcam_index_; }
    inline int      get_logical_table() const { return logical_table_; }
    inline bool     ingress()           const { return ingress_; }
    inline bool     has_run()           const { return has_run_; }

    inline uint32_t table_map(int col) { return cols_[col]->table_map(); }
    inline void set_table_map(int col, uint32_t v) { cols_[col]->set_table_map(v); }

    inline uint16_t get_tind_buses () {
      return mau_logical_tcam_reg_.get_tind_buses();
    }

    inline int tcam_match_addr_shift() {
      return mau_logical_tcam_reg_.tcam_match_adr_shift();
    }
    inline int paired_ltcam(bool ingress) {
      return mau_logical_tcam_reg_.paired_ltcam(ingress);
    }

    // Allow reset/query of hit/miss tracking counters
    inline void reset_hit_miss_cnt() { hits_ = misses_ = 0u; }
    inline void get_hit_miss_cnt(uint32_t *hit, uint32_t *miss) {
      if (hit != NULL) *hit = hits_;
      if (miss != NULL) *miss = misses_;
    }

    bool lookup_ternary_match(Phv *match_phv, int logicalTableIndex, MauLookupResult *result,
                              bool ingress, bool evalAll);
    int  lookup_wide_match(Phv *match_phv, MauTcam *tcam0, MauLookupResult *result,
                           bool ingress, bool evalAll);
    bool find_tind_sram(uint32_t matchAddr, int logicalTableIndex, MauLookupResult *result,
                        bool evalAll);

    void tcam_table_map_updated(int col,
                                uint32_t new_table_map, uint32_t old_table_map);
    void tcam_config_changed(int row, int col);
    uint32_t tcam_find_chain(int endrow, int col);
    uint32_t tcam_find_chain_rest(int endrow, int col);

    void update_logical_table(int new_logtab);

    void remove_tind_logical_table(int sram, int logtab);
    void add_tind_logical_table(int sram, int logtab);
    void update_tind_logical_tables(int sram, uint16_t new_logtabs, uint16_t old_logtabs);
    void update_tind_sram(MauSram *mauSram, uint16_t new_logtabs, uint16_t old_logtabs);
    void add_tind_sram(MauSram *mauSram, uint16_t new_logtabs);
    void remove_tind_sram(MauSram *mauSram, uint16_t old_logtabs);

    bool tcam_check_gress_mode(int lt);

    void reset_resources();

    MauLogicalTcam *paired_ltcam_obj(bool ingress);
    int find_logical_table();


 private:
    void update_tcam_config_internal();
    uint32_t calculate_chain(Mau *mauobj, int col,
                             int start_row, int end_row, int my_row);
    bool sanity_check_chain_internal(Mau *mauobj, int col, int start_row, uint32_t chain);
    int lookup_chain_simple(Mau *mauobj, Phv *match_phv, int col,
                            int start_row, int end_row, uint32_t chain, int head,
                            BitVector<kTcamEntries> *hits);
    int lookup_chain_simple(Mau *mauobj, Phv *match_phv, int col, uint32_t chain, int head,
                            BitVector<kTcamEntries> *hits);
    int lookup_chain_simple(Mau *mauobj, Phv *match_phv, MauTcam *tcam,
                            BitVector<kTcamEntries> *hits);


    int                                                        ltcam_index_;
    uint32_t                                                   curr_seq_;
    uint32_t                                                   pending_seq_;
    CacheId                                                    lookup_cache_id_;
    bool                                                       lookup_cache_ingress_;
    MauLookupResult                                            cached_result_;
    bool                                                       ingress_;
    bool                                                       has_run_;
    int                                                        logical_table_;
    uint32_t                                                   hits_;
    uint32_t                                                   misses_;
    std::array< BitVector<kSramsPerMau>, kLogicalTablesPerMau> tind_srams_used_;
    std::array<MauLogicalTcamCol*,kTcamColumnsPerMau>          cols_;
    std::vector<MauTcam*>                                      tcams_;
    MauLogicalTcamReg                                          mau_logical_tcam_reg_;
  };
}
#endif // _SHARED_MAU_LOGICAL_TCAM_
