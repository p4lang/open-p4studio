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

#ifndef _SHARED_MAU_
#define _SHARED_MAU_

#include <string>
#include <cstdint>
#include <mutex>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <pipe.h>
#include <time-info.h>
#include <cache-id.h>
#include <phv.h>
#include <eop.h>
#include <teop.h>
#include <mau-info.h>
#include <mau-lookup-result.h>
#include <mau-input.h>
#include <mau-dependencies.h>
#include <mau-result-bus.h>
#include <mau-addr-dist.h>
#include <mau-instr-store.h>
#include <mau-op-handler.h>
#include <mau-memory.h>
#include <mau-moveregs.h>
#include <mau-logical-table.h>
#include <mau-logical-row.h>
#include <mau-sram-column.h>
#include <mau-sram-row.h>
#include <mau-sram.h>
#include <mau-mapram.h>
#include <mau-logical-tcam.h>
#include <mau-tcam-row.h>
#include <mau-tcam.h>
#include <mau-stash-column.h>
#include <mau-hash-distribution.h>
#include <mau-execute-step.h>
#include <mau-table-counters.h>
#include <mau-stateful-counters.h>
#include <mau-predication.h>
#include <mau-snapshot.h>
#include <mau-teop.h>
#include <mau-mpr-tcam-table.h>
#include <mau-io.h>

#include <mau-chip.h>
#include <phv-modification.h>
#include <model_core/rmt-phv-modification.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauIgnoredRegs;
  class MauExecuteState;


  // Little class to track dynamic features per-thread
  class MauDynamicFeatures {
    static constexpr int  kNumMeterAlus = MauDefs::kNumMeterAlus;
 public:
    MauDynamicFeatures()  { reset(); }
    ~MauDynamicFeatures() { }
    void reset()          {
      dynamic_features_ = 0u;
      hash_dynamic_features_ = 0u;
      tcam_dynamic_features_ = 0u;
      tind_dynamic_features_ = 0u;
      for (int i = 0; i < kNumMeterAlus; i++) meter_alu_dynamic_features_[i] = 0u;
    }
    uint32_t meter_alu_dynamic_features(int alu) {
      RMT_ASSERT((alu >= 0) && (alu < kNumMeterAlus));
      return meter_alu_dynamic_features_[alu];
    }
    uint32_t dynamic_features() const { return dynamic_features_; }

    void coalesce_dynamic_features() {
      uint32_t f = 0u;
      f |= hash_dynamic_features_;
      f |= tcam_dynamic_features_;
      f |= tind_dynamic_features_;
      for (int i = 0; i < kNumMeterAlus; i++) f |= meter_alu_dynamic_features_[i];
      dynamic_features_ = f;
    }
    void set_hash_dynamic_features(uint32_t dynamic_features) {
      hash_dynamic_features_ = dynamic_features;
      coalesce_dynamic_features();
    }
    void set_tcam_dynamic_features(uint32_t dynamic_features) {
      tcam_dynamic_features_ = dynamic_features;
      coalesce_dynamic_features();
    }
    void set_tind_dynamic_features(uint32_t dynamic_features) {
      tind_dynamic_features_ = dynamic_features;
      coalesce_dynamic_features();
    }
    void set_meter_alu_dynamic_features(int alu, uint32_t dynamic_features) {
      RMT_ASSERT((alu >= 0) && (alu < kNumMeterAlus));
      meter_alu_dynamic_features_[alu] = dynamic_features;
      coalesce_dynamic_features();
    }
 private:
    uint32_t                            dynamic_features_;
    uint32_t                            hash_dynamic_features_;
    uint32_t                            tcam_dynamic_features_;
    uint32_t                            tind_dynamic_features_;
    std::array<uint32_t,kNumMeterAlus>  meter_alu_dynamic_features_;
  };



  class Mau : public MauObject {
 public:
    // These defined in rmt-config.cpp
    static bool     kMauUseMutex;
    static bool     kMauDinPowerMode;
    static bool     kResetUnusedLookupResults;
    static bool     kLookupUnusedLtcams;
    static bool     kSetNextTablePred;
    static bool     kRelaxPrevStageCheck;
    static uint32_t kMauFeatures[RmtDefs::kStagesMax];


    static constexpr int  kType = RmtTypes::kRmtTypeMau;
    static constexpr int  kExactMatchInputBits = MauDefs::kExactMatchInputBits;
    static constexpr int  kExactMatchValidBits = MauDefs::kExactMatchValidBits;
    static constexpr int  kTernaryMatchInputBits = MauDefs::kTernaryMatchInputBits;
    static constexpr int  kTernaryMatchValidBits = MauDefs::kTernaryMatchValidBits;
    static constexpr int  kTotalMatchInputBits = MauDefs::kTotalMatchInputBits;
    static constexpr int  kTotalMatchValidBits = MauDefs::kTotalMatchValidBits;

    static constexpr int  kActionHVOutputBusWidth = MauDefs::kActionHVOutputBusWidth;
    static constexpr int  kMatchOutputBusWidth = MauDefs::kMatchOutputBusWidth;
    static constexpr int  kTindOutputBusWidth = MauDefs::kTindOutputBusWidth;
    static constexpr int  kHashOutputWidth = MauDefs::kHashOutputWidth;
    static constexpr int  kLogicalTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int  kLogicalColumns = MauDefs::kLogicalColumnsPerMau;
    static constexpr int  kLogicalRows = MauDefs::kLogicalRowsPerMau;
    static constexpr int  kLogicalRowsPerPhysicalRow = MauDefs::kLogicalRowsPerPhysicalRow;
    static constexpr int  kSramColumnFirstRHS = MauDefs::kSramColumnFirstRHS;
    static constexpr int  kSramColumns = MauDefs::kSramColumnsPerMau;
    static constexpr int  kSramValidColumnMask = MauDefs::kSramValidColumnMask;
    static constexpr int  kSramRowFirstTOP = MauDefs::kSramRowFirstTOP;
    static constexpr int  kSramRows = MauDefs::kSramRowsPerMau;
    static constexpr int  kSrams = MauDefs::kSramsPerMau;
    static constexpr int  kMapramColumns = MauDefs::kMapramColumnsPerMau;
    // No maprams instantiated for cols 0-3 since regs_12544_mau_dev
    // No maprams instantiated for cols 4-6 since regs_13957_mau_dev - now only on RHS
    static constexpr int  kMapramValidColumnMask = MauDefs::kMapramValidColumnMask;
    static constexpr int  kMapramRowFirstTOP = MauDefs::kMapramRowFirstTOP;
    static constexpr int  kMapramRows = MauDefs::kMapramRowsPerMau;
    static constexpr int  kMaprams = MauDefs::kMapramsPerMau;
    static constexpr int  kTcams = MauDefs::kTcamsPerMau;
    static constexpr int  kTcamRows = MauDefs::kTcamRowsPerMau;
    static constexpr int  kTcamColumns = MauDefs::kTcamColumnsPerMau;
    static constexpr int  kLogicalTcams = MauDefs::kLogicalTcamsPerMau;
    static constexpr bool kEvaluateAllDefault = RmtDefs::kEvaluateAllDefault;
    static constexpr int  kVpns = MauDefs::kVpnsPerMau;
    static constexpr int  kNumMeterAlus = MauDefs::kNumMeterAlus;
    static constexpr int  kPhvWords = Phv::kWordsMax;
    static constexpr int  kMaxConsecDataOverflows = MauDefs::kMaxConsecDataOverflows;


    // Static consts/funcs to allow query of per-MAU features
    static constexpr uint32_t kMauFeature_OphvIsZero           = 0x01u;
    static constexpr uint32_t kMauFeature_MustUseIphv          = 0x02u;
    static constexpr uint32_t kMauFeature_MustBeMatchDependent = 0x04u;
    static constexpr uint32_t kMauFeature_IgnoresStartTable    = 0x08u;
    static constexpr uint32_t kMauFeature_IphvIsZero           = 0x10u;
    static constexpr uint32_t kMauFeature_MustUseOphv          = 0x20u;
    static constexpr uint32_t kMauFeature_MustHaveAlwaysRun    = 0x40u;

    static inline bool ophv_is_zero(int mau_index) {
      RMT_ASSERT((mau_index >= 0) && (mau_index < RmtDefs::kStagesMax));
      return kMauFeatures[mau_index] & kMauFeature_OphvIsZero;
    }
    static inline bool must_use_iphv(int mau_index) {
      RMT_ASSERT((mau_index >= 0) && (mau_index < RmtDefs::kStagesMax));
      return kMauFeatures[mau_index] & kMauFeature_MustUseIphv;
    }
    static inline bool must_be_match_dependent(int mau_index) {
      RMT_ASSERT((mau_index >= 0) && (mau_index < RmtDefs::kStagesMax));
      return kMauFeatures[mau_index] & kMauFeature_MustBeMatchDependent;
    }
    static inline bool ignores_start_table(int mau_index) {
      RMT_ASSERT((mau_index >= 0) && (mau_index < RmtDefs::kStagesMax));
      return kMauFeatures[mau_index] & kMauFeature_IgnoresStartTable;
    }
    static inline bool uses_start_table(int mau_index) {
      return !ignores_start_table(mau_index);
    }
    static inline bool iphv_is_zero(int mau_index) {
      RMT_ASSERT((mau_index >= 0) && (mau_index < RmtDefs::kStagesMax));
      return kMauFeatures[mau_index] & kMauFeature_IphvIsZero;
    }
    static inline bool must_use_ophv(int mau_index) {
      RMT_ASSERT((mau_index >= 0) && (mau_index < RmtDefs::kStagesMax));
      return kMauFeatures[mau_index] & kMauFeature_MustUseOphv;
    }
    static inline bool must_have_always_run(int mau_index) {
      RMT_ASSERT((mau_index >= 0) && (mau_index < RmtDefs::kStagesMax));
      return kMauFeatures[mau_index] & kMauFeature_MustHaveAlwaysRun;
    }


    static inline bool lhs_sram(int row, int col) {
      return (col < kSramColumnFirstRHS);
    }
    static inline bool rhs_sram(int row, int col) {
      return (!lhs_sram(row,col));
    }
    static inline bool bot_sram(int row, int col) {
      return (row < kSramRowFirstTOP);
    }
    static inline bool top_sram(int row, int col) {
      return (!bot_sram(row,col));
    }
    static inline int logical_row_index_left(int phys_row) {
      static_assert( (kLogicalRowsPerPhysicalRow == 2),
                     "Code assumes 2 logRows per physRow");
      RMT_ASSERT((phys_row >= 0) && (phys_row < kSramRows));
      return (phys_row * kLogicalRowsPerPhysicalRow);
    }
    static inline int logical_row_index_right(int phys_row) {
      return (logical_row_index_left(phys_row) + 1);
    }


    // Funcs to allow sram index calc in rowmajor and colmajor order.
    // We use rowmajor by default but swap over to colmajor for tind sram bitmap to
    // ensure colN tinds are handled before (and hence have higher pri) than colN+1
    static inline int sram_array_index_rowmajor(int row, int col) {
      return (row * kSramColumns) + col;
    }
    static inline int sram_array_index_rowmajor_get_row(int index) {
      return (index / kSramColumns);
    }
    static inline int sram_array_index_rowmajor_get_col(int index) {
      return (index % kSramColumns);
    }
    static inline int sram_array_index_colmajor(int row, int col) {
      return (col * kSramRows) + row;
    }
    static inline int sram_array_index_colmajor_get_col(int index) {
      return (index / kSramRows);
    }
    static inline int sram_array_index_colmajor_get_row(int index) {
      return (index % kSramRows);
    }
    static inline int sram_array_index_remap_rowmajor_to_colmajor(int index) {
      RMT_ASSERT ((index >= 0) && (index <= kSrams));
      int row = sram_array_index_rowmajor_get_row(index);
      int col = sram_array_index_rowmajor_get_col(index);
      RMT_ASSERT ((row >= 0) && (row < kSramRows));
      RMT_ASSERT ((col >= 0) && (col < kSramColumns));
      int c_index = sram_array_index_colmajor(row, col);
      RMT_ASSERT ((c_index >= 0) && (c_index <= kSrams));
      return c_index;
    }
    static inline int sram_array_index_remap_colmajor_to_rowmajor(int index) {
      RMT_ASSERT ((index >= 0) && (index <= kSrams));
      int row = sram_array_index_colmajor_get_row(index);
      int col = sram_array_index_colmajor_get_col(index);
      RMT_ASSERT ((row >= 0) && (row < kSramRows));
      RMT_ASSERT ((col >= 0) && (col < kSramColumns));
      int r_index = sram_array_index_rowmajor(row, col);
      RMT_ASSERT ((r_index >= 0) && (r_index <= kSrams));
      return r_index;
    }



    Mau(RmtObjectManager *om, int pipeIndex, int mauIndex, const MauConfig &config);
    virtual ~Mau();


    // Read/write/increment status info associated with MAU
    inline MauInfo *mau_info() { return &mau_info_; }
    // This func called internally to increments status counters
    inline void     mau_info_incr(int i, int inc=1) {
      mau_info_.incr(i, inc);
    }
    // Expect this one to be called by DV to retrieve counters
    inline uint32_t mau_info_read(const char *s, bool reset=false) {
      return mau_info_.read_name(s, reset);
    }
    // Or DV can call this to get whole set of counters
    inline void mau_info_read(uint32_t *array, int array_size, bool reset=false) {
      return mau_info_.read(array, array_size, reset);
    }
    // Or this to get all counters plus counter names
    inline void mau_info_read(uint32_t *array, int array_size, const char **name_array,
                              bool reset=false) {
      return mau_info_.read(array, array_size, name_array, reset);
    }



    inline MauInput *mau_input() { return &mau_input_; }
    inline void get_exact_match_input(Phv *phv,
                                      BitVector<kExactMatchInputBits> *input_bits,
                                      BitVector<kExactMatchValidBits> *valid_bits) {
      mau_input_.get_exact_match_input(phv, input_bits, valid_bits);
    }
    inline void get_ternary_match_input(Phv *phv,
                                        BitVector<kTernaryMatchInputBits> *input_bits,
                                        BitVector<kTernaryMatchValidBits> *valid_bits) {
      mau_input_.get_ternary_match_input(phv, input_bits, valid_bits);
    }
    inline void get_total_match_input(Phv *phv,
                                      BitVector<kTotalMatchInputBits> *input_bits,
                                      BitVector<kTotalMatchValidBits> *valid_bits) {
      mau_input_.get_total_match_input(phv, input_bits, valid_bits);
    }


    inline MauLookupResult *mau_lookup_result(int lt) {
      return ((lt >= 0) && (lt < kLogicalTables)) ? &lookup_results_[lt] :NULL;
    }


    inline MauDependencies *mau_dependencies() { return &mau_dependencies_; }
    inline void link_mau_dependencies()        { return mau_dependencies_.link_deps(); }
    inline void check_dependencies()           { return mau_dependencies_.check_config(); }
    inline bool is_match_dependent(bool ing)   { return mau_dependencies_.is_match_dependent(ing); }
    inline bool is_action_dependent(bool ing)  { return mau_dependencies_.is_action_dependent(ing); }
    inline bool start_table_avail(bool ing)    { return mau_dependencies_.start_table_avail(ing); }
    inline int  start_table(bool ingress) {
      return (start_table_avail(ingress)) ?next_tables_[ingress ?0 :1] :-1;
    }
    inline void report_dynamic_features() {
      for (int ie = 0; ie <= 1; ie++) {
        uint32_t curr_f = mau_dependencies_.dynamic_features(ie);
        uint32_t new_f = features_[ie].dynamic_features();
        if (new_f != curr_f) mau_dependencies_.set_dynamic_features(ie, new_f);
      }
    }

    inline void dependencies_changed() {
      dependencies_changed_ = true;
      // Suppress deltas until we have a pipe (ctor finished)
      if (pipe() != NULL) pipe()->dependencies_changed();
    }
    inline void handle_dependencies_changed() {
      if (dependencies_changed_) check_dependencies();
      dependencies_changed_ = false;
    }


    inline MauPredication *mau_predication()   { return &mau_predication_; }
    inline void pred_set_evaluate_all(bool tf) { mau_predication_.set_evaluate_all(tf); }
    inline void pred_start(bool thrd_active[]) { mau_predication_.start(thrd_active); }
    inline void pred_end()                     { mau_predication_.end(); }
    inline int  pred_get_next_table(bool ingress, int curr_lt, bool *do_lookup) {
      return mau_predication_.get_next_table(ingress, curr_lt, do_lookup);
    }
    inline int  pred_get_first_table(bool ingress, bool *do_lookup) {
      return mau_predication_.get_first_table(ingress, do_lookup);
    }
    inline void pred_set_next_table(int lt, const MauLookupResult &result) {
      mau_predication_.set_next_table(lt, result);
    }
    inline uint16_t pred_lt_info(uint16_t f)  { return mau_predication_.lt_info(f); }
    inline uint16_t pred_lts_active()         { return mau_predication_.lts_active(); }


    inline MauResultBus *mau_result_bus() { return &mau_result_bus_; }
    inline int get_logical_table_for_nxtab_bus(int exact_or_tcam, int nxtab_bus) {
      return mau_result_bus_.get_logical_table_for_nxtab_bus(exact_or_tcam, nxtab_bus);
    }

    inline MauAddrDist *mau_addr_dist() { return &mau_addr_dist_; }
    inline bool all_addrs_consumed() { return mau_addr_dist_.all_addrs_consumed(); }

    inline MauHashDistribution *mau_hash_dist() { return &mau_hash_dist_; }

    inline MauInstrStore *mau_instr_store() { return &mau_instr_store_; }
    inline BitVector<kPhvWords> *ingress_selector() {
      return mau_instr_store_.ingress();
    }
    inline BitVector<kPhvWords> *egress_selector() {
      return mau_instr_store_.egress();
    }
    inline BitVector<kPhvWords> *actionmux_en_selector() {
      return kMauDinPowerMode ?mau_instr_store_.actionmux_en() :&all_en_;
    }
    inline BitVector<kPhvWords> *matchxbar_en_selector() {
      return kMauDinPowerMode ?mau_instr_store_.matchxbar_en() :&all_en_;
    }
    inline int phv_get_gress(int phvWord) { return mau_instr_store_.phv_get_gress(phvWord); }

    inline MauOpHandler *mau_op_handler() { return &mau_op_handler_; }
    inline bool atomic_in_progress() { return mau_op_handler_.atomic_in_progress(); }

    inline MauMemory *mau_memory() { return &mau_memory_; }

    inline MauMoveregsCtl *mau_moveregs_ctl() { return &mau_moveregs_ctl_; }
    inline MauMoveregs *mau_moveregs(const int lt) {
      return mau_moveregs_ctl()->get_moveregs(lt);
    }

    inline MauTableCounters *mau_table_counters() { return &mau_table_counters_; }
    inline MauStatefulCounters *mau_stateful_counters() { return &mau_stateful_counters_; }
    inline bool evaluate_table_counters() { return mau_table_counters_.evaluate_table_counters(); }
    inline bool stateful_counter_enabled(int lt) {
      return mau_stateful_counters_.stateful_counter_enabled(lt);
    }
    inline uint32_t get_meter_addr_pre_vpn_squash(int alu) { // For DV
      return mau_stateful_counters_.get_final_addr(alu);
    }

    inline MauSnapshot *mau_snapshot() { return &mau_snapshot_; }
    inline bool snapshot_captured() { return mau_snapshot_.snapshot_captured(); }

    inline MauTeop *mau_teop() { return &mau_teop_; }

    // CHIP-SPECIFIC logic
    inline int  pipe_dump_index()  { return mau_chip_.pipe_dump_index(); }
    inline int  mau_dump_index()   { return mau_chip_.mau_dump_index();  }



    inline bool is_selector_alu_output_invalid(uint8_t alu) {
      return ((inval_output_selector_alus_ & (1<<alu)) != 0);
    }
    inline void set_selector_alu_output_invalid(uint8_t alu, bool inval) {
      inval_output_selector_alus_ &= ~(1<<alu);            // clear flag
      inval_output_selector_alus_ |= (inval) ?(1<<alu) :0; // maybe set flag
    }

    void check_data_oflo_rows();
    inline uint8_t get_data_oflo_rows() { return data_oflo_rows_; }
    inline void set_data_oflo(uint8_t row, uint8_t oflo) {
      RMT_ASSERT(row < kSramRows);
      spinlock();
      data_oflo_rows_ &= ~(1<<row);         // Clear oflo for row
      data_oflo_rows_ |= (oflo & 0x1)<<row; // Maybe set oflo for row
      spinunlock();
    }

    inline void set_hash_dynamic_features(uint32_t i_ftrs, uint32_t e_ftrs) {
      features_[0].set_hash_dynamic_features(i_ftrs);
      features_[1].set_hash_dynamic_features(e_ftrs);
      report_dynamic_features();
    }
    inline void set_tcam_dynamic_features(uint32_t i_ftrs, uint32_t e_ftrs) {
      features_[0].set_tcam_dynamic_features(i_ftrs);
      features_[1].set_tcam_dynamic_features(e_ftrs);
      report_dynamic_features();
    }
    inline void set_tind_dynamic_features(uint32_t i_ftrs, uint32_t e_ftrs) {
      features_[0].set_tind_dynamic_features(i_ftrs);
      features_[1].set_tind_dynamic_features(e_ftrs);
      report_dynamic_features();
    }
    inline void set_meter_alu_dynamic_features(int alu, bool egress, uint32_t i_ftrs, uint32_t e_ftrs) {
      egress_meter_alus_ &= ~(1<<alu);             // clear flags
      egress_meter_alus_ |= (egress) ?(1<<alu) :0; // maybe set flags
      features_[0].set_meter_alu_dynamic_features(alu, i_ftrs);
      features_[1].set_meter_alu_dynamic_features(alu, e_ftrs);
      report_dynamic_features();
    }
    inline bool is_meter_alu_egress(int alu) {
      return ((egress_meter_alus_ & (1<<alu)) != 0);
    }
    inline uint32_t meter_alu_dynamic_features(int alu) {
      int i = (is_meter_alu_egress(alu)) ?1 :0;
      return features_[i].meter_alu_dynamic_features(alu);
    }
    inline bool is_meter_alu_meter_lpf(int alu) {
      uint32_t mftrs = MauDefs::kMauMeterAluMeterPresent| MauDefs::kMauMeterAluMeterLpfPresent;
      return ((meter_alu_dynamic_features(alu) & mftrs) != 0u);
    }
    inline bool is_meter_alu_stateful(int alu) {
      return ((meter_alu_dynamic_features(alu) & MauDefs::kMauMeterAluStatefulPresent) != 0u);
    }
    void tcam_config_changed();

    Mau *mau_previous()             { return mau_previous_; }
    void set_mau_previous(Mau *mau) { mau_previous_ = mau; link_mau_dependencies(); }


    MauIO *mau_io_input();
    MauIO *mau_io_output();

    // Convenience funcs to set up PRED/MPR nxt-tabs,global_exec and long_branch
    void set_pred(uint16_t global_exec, uint8_t long_branch) {
      mau_io_input()->set_pred(global_exec, long_branch);
    }
    void set_pred(int i_nxt, int e_nxt, int g_nxt, uint16_t global_exec, uint8_t long_branch) {
      mau_io_input()->set_pred(i_nxt, e_nxt, g_nxt, global_exec, long_branch);
    }
    void set_mpr(uint16_t global_exec, uint8_t long_branch) {
      mau_io_input()->set_mpr(global_exec, long_branch);
    }
    void set_mpr(int i_nxt, int e_nxt, int g_nxt, uint16_t global_exec, uint8_t long_branch) {
      mau_io_input()->set_mpr(i_nxt, e_nxt, g_nxt, global_exec, long_branch);
    }



    inline MauLogicalTable *logical_table_lookup(int tableIndex) {
      RMT_ASSERT ((tableIndex >= 0) && (tableIndex < kLogicalTables));
      return logical_tables_[tableIndex];
    }
    inline void logical_table_set(int tableIndex, MauLogicalTable *table) {
      RMT_ASSERT ((tableIndex >= 0) && (tableIndex < kLogicalTables));
      logical_tables_[tableIndex] = table;
    }
    MauLogicalTable *logical_table_get(int tableIndex);

    inline MauLogicalRow *logical_row_lookup(int logicalRowIndex) {
      RMT_ASSERT ((logicalRowIndex >= 0) && (logicalRowIndex < kLogicalRows));
      return logical_rows_[logicalRowIndex];
    }
    inline void logical_row_set(int logicalRowIndex, MauLogicalRow *logrow) {
      RMT_ASSERT ((logicalRowIndex >= 0) && (logicalRowIndex < kLogicalRows));
      logical_rows_[logicalRowIndex] = logrow;
    }
    MauLogicalRow *logical_row_get(int logicalRowIndex);


    inline MauSramColumn *sram_column_lookup(int columnIndex) {
      RMT_ASSERT ((columnIndex >= 0) && (columnIndex < kSramColumns));
      return sram_columns_[columnIndex];
    }
    inline void sram_column_set(int columnIndex, MauSramColumn *column) {
      RMT_ASSERT ((columnIndex >= 0) && (columnIndex < kSramColumns));
      sram_columns_[columnIndex] = column;
    }
    MauSramColumn *sram_column_get(int columnIndex);

    inline MauSramRow *sram_row_lookup(int rowIndex) {
      RMT_ASSERT ((rowIndex >= 0) && (rowIndex < kSramRows));
      return sram_rows_[rowIndex];
    }
    inline void sram_row_set(int rowIndex, MauSramRow *row) {
      RMT_ASSERT ((rowIndex >= 0) && (rowIndex < kSramRows));
      sram_rows_[rowIndex] = row;
    }
    MauSramRow *sram_row_get(int rowIndex);
    MauSramRow *sram_row_above_central() { return sram_rows_[ (kSramRows/2) ];     }
    MauSramRow *sram_row_below_central() { return sram_rows_[ (kSramRows/2) - 1 ]; }

    // half the color busses come from above match central and half from below.
    void get_color_bus( int which_bus, uint8_t *output, bool report_error_if_not_driven );



    inline int sram_array_index(int row, int col) {
      RMT_ASSERT ((row >= 0) && (row < kSramRows));
      RMT_ASSERT ((col >= 0) && (col < kSramColumns));
      return sram_array_index_rowmajor(row, col);
    }
    inline int mapram_array_index(int row, int col) {
      RMT_ASSERT ((row >= 0) && (row < kMapramRows));
      RMT_ASSERT ((col >= 0) && (col < kMapramColumns));
      return (row * kMapramColumns) + col;
    }
    inline int tcam_array_index(int row, int col) {
      RMT_ASSERT ((row >= 0) && (row < kTcamRows));
      RMT_ASSERT ((col >= 0) && (col < kTcamColumns));
      return row + (col * kTcamRows);
    }
    inline int logical_row_index(int phys_row, int phys_col) {
      return sram_array_index(phys_row, phys_col) / kLogicalColumns;
    }
    inline int logical_column_index(int phys_row, int phys_col) {
      return sram_array_index(phys_row, phys_col) % kLogicalColumns;
    }
    inline int physical_row_index(int log_row) {
      RMT_ASSERT ((log_row >= 0) && (log_row < kLogicalRows));
      return (log_row / kLogicalRowsPerPhysicalRow);
    }
    inline int physical_row_which(int log_row) {
      RMT_ASSERT ((log_row >= 0) && (log_row < kLogicalRows));
      return (log_row % kLogicalRowsPerPhysicalRow);
    }


    inline MauSram *sram_lookup(int sramIndex) {
      RMT_ASSERT ((sramIndex >= 0) && (sramIndex < kSrams));
      return srams_[sramIndex];
    }
    inline void sram_set(int sramIndex, MauSram *sram) {
      RMT_ASSERT ((sramIndex >= 0) && (sramIndex < kSrams));
      srams_[sramIndex] = sram;
    }
    inline MauSram *sram_lookup(int rowIndex, int colIndex) {
      return sram_lookup(sram_array_index(rowIndex, colIndex));
    }
    inline void sram_set(int rowIndex, int colIndex, MauSram *sram) {
      sram_set(sram_array_index(rowIndex, colIndex), sram);
    }
    MauSram *sram_get(int rowIndex, int colIndex);


    inline MauMapram *mapram_lookup(int mapramIndex) {
      RMT_ASSERT ((mapramIndex >= 0) && (mapramIndex < kMaprams));
      return maprams_[mapramIndex];
    }
    inline void mapram_set(int mapramIndex, MauMapram *mapram) {
      RMT_ASSERT ((mapramIndex >= 0) && (mapramIndex < kMaprams));
      maprams_[mapramIndex] = mapram;
    }
    inline MauMapram *mapram_lookup(int rowIndex, int colIndex) {
      return mapram_lookup(mapram_array_index(rowIndex, colIndex));
    }
    inline void mapram_set(int rowIndex, int colIndex, MauMapram *mapram) {
      mapram_set(mapram_array_index(rowIndex, colIndex), mapram);
    }
    MauMapram *mapram_get(int rowIndex, int colIndex);


    inline MauLogicalTcam *logical_tcam_lookup(int ltcamIndex) {
      RMT_ASSERT ((ltcamIndex >= 0) && (ltcamIndex < kLogicalTcams));
      return logical_tcams_[ltcamIndex];
    }
    inline void logical_tcam_set(int ltcamIndex, MauLogicalTcam *ltcam) {
      RMT_ASSERT ((ltcamIndex >= 0) && (ltcamIndex < kLogicalTcams));
      logical_tcams_[ltcamIndex] = ltcam;
    }
    MauLogicalTcam *logical_tcam_get(int ltcamIndex);

    inline MauTcamRow *tcam_row_lookup(int rowIndex) {
      RMT_ASSERT ((rowIndex >= 0) && (rowIndex < kTcamRows));
      return tcam_rows_[rowIndex];
    }
    inline void tcam_row_set(int rowIndex, MauTcamRow *row) {
      RMT_ASSERT ((rowIndex >= 0) && (rowIndex < kTcamRows));
      tcam_rows_[rowIndex] = row;
    }
    MauTcamRow *tcam_row_get(int rowIndex);


    inline MauTcam *tcam_lookup(int tcamIndex) {
      RMT_ASSERT ((tcamIndex >= 0) && (tcamIndex < kTcams));
      return tcams_[tcamIndex];
    }
    inline void tcam_set(int tcamIndex, MauTcam *tcam) {
      RMT_ASSERT ((tcamIndex >= 0) && (tcamIndex < kTcams));
      tcams_[tcamIndex] = tcam;
    }
    inline MauTcam *tcam_lookup(int rowIndex, int colIndex) {
      return tcam_lookup(tcam_array_index(rowIndex, colIndex));
    }
    inline void tcam_set(int rowIndex, int colIndex, MauTcam *tcam) {
      tcam_set(tcam_array_index(rowIndex, colIndex), tcam);
    }
    MauTcam *tcam_get(int rowIndex, int colIndex);

    MauStashColumn *stash_column_get() { return &mau_stash_column_; }

    inline void vpn_add_sram(int vpn, MauSram *mauSram) {
      int sramIndex = mauSram->sram_index();
      RMT_ASSERT((sramIndex >= 0) && (sramIndex < kSrams));
      RMT_ASSERT((vpn >= 0) && (vpn <= kVpns));
      vpn_sram_bitmaps_[vpn].set_bit(sramIndex);
    }
    inline void vpn_remove_sram(int vpn, MauSram *mauSram) {
      int sramIndex = mauSram->sram_index();
      RMT_ASSERT((sramIndex >= 0) && (sramIndex < kSrams));
      RMT_ASSERT((vpn >= 0) && (vpn <= kVpns));
      vpn_sram_bitmaps_[vpn].clear_bit(sramIndex);
    }
    inline MauSram *vpn_get_next_sram(int vpn, MauSram *prevSram=NULL) {
      MauSram *nextSram = NULL;
      int prevIndex = -1;
      if (prevSram != NULL) prevIndex = prevSram->sram_index();
      RMT_ASSERT((prevIndex >= -1) && (prevIndex < kSrams));
      int nextIndex = vpn_sram_bitmaps_[vpn].get_first_bit_set(prevIndex);
      if ((nextIndex >= 0) && (nextIndex < kSrams)) {
        nextSram = sram_lookup(nextIndex);
        RMT_ASSERT(nextSram != NULL);
      }
      return nextSram;
    }
    inline MauSram *vpn_get_sram(int vpn) { return vpn_get_next_sram(vpn); }

    bool evaluate_all(int tableIndex);
    void set_evaluate_all(int tableIndex, bool tf);
    bool evaluate_all();
    void set_evaluate_all(bool tf);

    int  find_next_table(const bool ingress, int curr_table_id, int next_table_id);
    int  find_first_table(const bool ingress);
    void execute(Phv *iphv, Phv *ophv, Phv **next_iphv, Phv** next_ophv);
    void execute(Phv *iphv, Phv *ophv,
                 int *ingress_next_table, int *egress_next_table, int *ghost_next_table,
                 Phv** next_iphv, Phv** next_ophv);
    void process_action(MauExecuteState* state);
    int  alu_main(Phv *action_phv, Instr *instr,
                  const BitVector<kActionHVOutputBusWidth> &action_bus);
    void dump_action_hv_bus(const BitVector<kActionHVOutputBusWidth>& a_hv_bus);
    int  handle_eop(const Eop &eop);
    int  handle_dp_teop(const Teop &teop);
    int  handle_dp_teop(Teop *teop);
    int  pbus_read(uint8_t addrtype, int logical_table, uint32_t addr,
                   uint64_t* data0, uint64_t* data1, bool clear, bool lock, uint64_t T=UINT64_C(0));
    int  pbus_write_op(uint8_t addrtype, int logical_table, uint32_t addr,
                       uint64_t data0, uint64_t data1, bool lock, uint64_t T);
    int  pbus_write(uint8_t addrtype, int logical_table, uint32_t addr,
                    uint64_t data0, uint64_t data1, bool lock, uint64_t T);

    static constexpr uint8_t kFlagsDistribRsvd  = 0x0F; // See mau-execute-step.h
    static constexpr uint8_t kFlagsLock         = 0x10;
    static constexpr uint8_t kFlagsResetBackend = 0x20;
    static constexpr uint8_t kFlagsResetAll     = 0x40;
    int  pbus_read_write(uint8_t addrtype, int logical_table,
                         uint32_t addrRd, uint32_t addrWr, uint8_t flags, uint64_t T);
    int  do_sweep(uint8_t addrtype, int logical_table, uint32_t addr,
                  bool lock, uint64_t relativeT, uint64_t meterTickT=UINT64_C(0));
    int  do_stateful_clear(int logical_table, uint64_t T);

    void mau_init_srams();
    void mau_init_tcams();
    void mau_init_tables();
    void mau_init_all();
    void mau_free_all();
    void lock_resources();
    void unlock_resources();
    void reset_backend();
    void reset_resources();
    void flush_color_queues();
    void flush_queues();

    BitVector<kActionHVOutputBusWidth>* action_hv_output_bus() { return &action_hv_output_bus_; }
    BitVector<kLogicalTables> table_active();
    BitVector<kHashOutputWidth> get_hash_output(Phv *phv,int group);

    // Only used by DV. TODO: get them to change to calling execute()
    Phv *process_match2(Phv *iphv, Phv *ophv,
                        int *ingress_next_table, int *egress_next_table, int *ghost_next_table) {
      Phv* next_iphv = NULL; Phv* next_ophv = NULL;
      if (iphv != NULL)      iphv->set_source(Phv::kFlagsSourceMau);
      if (ophv != NULL)      ophv->set_source(Phv::kFlagsSourceMau);
      execute(iphv, ophv, ingress_next_table, egress_next_table, ghost_next_table, &next_iphv, &next_ophv);
      if (iphv != NULL)      iphv->set_source(0);
      if (ophv != NULL)      ophv->set_source(0);
      if (next_iphv != NULL) next_iphv->set_source(0);
      if (next_ophv != NULL) next_ophv->set_source(0);
      stashed_next_ophv_ = next_ophv;
      return next_iphv;
    }
    Phv *process_match(Phv *iphv, Phv *ophv,
                       int *ingress_next_table, int *egress_next_table, int *ghost_next_table) {
      return process_match2(iphv, ophv, ingress_next_table, egress_next_table, ghost_next_table);
    }
    Phv *process_match(Phv *iphv, Phv *ophv,
                       int *ingress_next_table, int *egress_next_table) {
      int ghost_next_table = -1;
      return process_match2(iphv, ophv, ingress_next_table, egress_next_table, &ghost_next_table);
    }
    // Only used by DV. TODO: get them to change to calling execute()
    Phv *process_action(Phv *iphv, Phv *ophv) {
      return stashed_next_ophv_;
    }

    // used only by test_actions test_stats
    void process_for_tests(Phv *phv, const Eop &eop);

    //public method for set_phv_modification
    int set_phv_modification(model_core::RmtPhvModification::ModifyEnum which, 
                            model_core::RmtPhvModification::ActionEnum action, 
                            int index, uint32_t value);

    // this hackiness is for DV to get the selector length to pass to
    // the hash distribution functions
    uint32_t get_selector_length( int logtab ) {
      return lookup_results_[logtab].get_selector_length(logtab);
    }
    uint32_t get_selector_action_address(Phv *phv, int logtab) {
      return lookup_results_[logtab].get_selector_action_address(phv, logtab);
    }
    uint32_t get_selector_address(Phv *phv, int logtab) {
      return lookup_results_[logtab].get_selector_address(phv, logtab);
    }

    // Another interface for DV to find out what LTCAMs are powered
    inline uint8_t get_powered_ltcams() {
      if (powered_ltcams_ != 0) return powered_ltcams_; // Already calculated
      // Calculate and cache - this var reset each PHV see reset_resources()
      powered_ltcams_ = mau_mpr_tcam_table_.get_powered_ltcams(pred_lt_info(Pred::kLookupable));
      return powered_ltcams_;
    }



    // make these available for DV
    Phv* make_match_phv(Phv *iphv, Phv *ophv);
    Phv* make_action_phv(Phv *iphv, Phv *ophv);
    Phv* make_output_phv(Phv *action_phv, Phv *ophv);


 private:
    void process_snap_start(MauExecuteState* state);
    void process_snap_end(MauExecuteState* state);
    void process_pred_start(MauExecuteState* state);
    void process_pred_end(MauExecuteState* state);
    void process_lookup(bool ingress, MauExecuteState* state);
    void process_update_next_table(bool ingress, MauExecuteState* state);
    void tick_stateful_counters(MauExecuteState* state);

 private:
    std::array<MauLogicalTable*,kLogicalTables> logical_tables_;
    std::array<MauLogicalRow*,kLogicalRows>     logical_rows_;
    std::array<MauSramColumn*,kSramColumns>     sram_columns_;
    std::array<MauSramRow*,kSramRows>           sram_rows_;
    std::array<MauSram*,kSrams>                 srams_;
    std::array<MauMapram*,kMaprams>             maprams_;
    std::array<MauLogicalTcam*,kLogicalTcams>   logical_tcams_;
    std::array<MauTcamRow*,kTcamRows>           tcam_rows_;
    std::array<MauTcam*,kTcams>                 tcams_;
    std::mutex                                  mau_lock_;
    bool                                        evaluate_all_;
    bool                                        dependencies_changed_;
    BitVector<kPhvWords>                        all_en_;
    BitVector<kActionHVOutputBusWidth>          action_hv_output_bus_;
    std::array<MauLookupResult,kLogicalTables>  lookup_results_;
    std::array<BitVector<kSrams>,kVpns>         vpn_sram_bitmaps_;
    TimeInfoType                                time_info_;

  
    PhvModification                             modify_match_; //instance variables for PHV modifications
    PhvModification                             modify_action_;
    PhvModification                             modify_output_;
    MauInfo                                     mau_info_;
    MauStashColumn                              mau_stash_column_;
    MauInput                                    mau_input_;
    MauDependencies                             mau_dependencies_;
    MauPredication                              mau_predication_;
    MauResultBus                                mau_result_bus_;
    MauAddrDist                                 mau_addr_dist_;
    MauHashDistribution                         mau_hash_dist_;
    MauInstrStore                               mau_instr_store_;
    MauOpHandler                                mau_op_handler_;
    MauMemory                                   mau_memory_;
    MauMoveregsCtl                              mau_moveregs_ctl_;
    MauTableCounters                            mau_table_counters_;
    MauStatefulCounters                         mau_stateful_counters_;
    MauSnapshot                                 mau_snapshot_;
    MauTeop                                     mau_teop_;
    MauMprTcamTable                             mau_mpr_tcam_table_;
    MauChip                                     mau_chip_;
    MauIgnoredRegs                             *mau_ignored_regs_;
    Mau                                        *mau_previous_;

    uint8_t                                     powered_ltcams_;
    uint8_t                                     inval_output_selector_alus_;
    uint8_t                                     data_oflo_rows_;
    uint8_t                                     egress_meter_alus_;
    std::array<MauDynamicFeatures,2>            features_;    // for ingress and egress
    std::array<int,2>                           next_tables_; // for ingress and egress
    Phv* stashed_next_ophv_=nullptr; // only needed for DV's process_action()  TODO: remove when possible


    std::vector< MauExecuteStep* > match_only_steps_{
      new MauExecuteStepMauFunc("Predication Start",this,&Mau::process_pred_start,kOnlyAtHeaderTime),
      new MauExecuteStepMauInEgFunc("Lookup",this,&Mau::process_lookup,kOnlyAtHeaderTime),
      new MauExecuteStepMauFunc("Predication End",this,&Mau::process_pred_end,kOnlyAtHeaderTime),
      };

    std::vector< MauExecuteStep* > at_hdr_steps_{
      new MauExecuteStepMauFunc("Snapshot Start",this,&Mau::process_snap_start,kOnlyAtHeaderTime),
      new MauExecuteStepMauFunc("Predication Start",this,&Mau::process_pred_start,kOnlyAtHeaderTime),
      new MauExecuteStepMauInEgFunc("Lookup",this,&Mau::process_lookup,kOnlyAtHeaderTime),
      new MauExecuteStepMauFunc("Tick Stateful Counters",this,&Mau::tick_stateful_counters,kOnlyAtHeaderTime),
      new MauExecuteStepRunTables("Dist act data addr",this,&MauLogicalTable::distrib_action_data_address,kOnlyAtHeaderTime),
      new MauExecuteStepRunTables("Dist idletime addr",this,&MauLogicalTable::distrib_idletime_address,kOnlyAtHeaderTime),
      new MauExecuteStepRunTables("Dist stats addr",this,&MauLogicalTable::distrib_stats_address_at_hdr,kOnlyAtHeaderTime),
      new MauExecuteStepMauAddrDistFunc("Final dist stats addrs",this,&MauAddrDist::finalize_stats_addresses,kOnlyAtHeaderTime),

      // have to do fetch addrs before color mapram read (and again after dist meters)
      new MauExecuteStepRunRows("Fetch Addrs",this,&MauSramRow::fetch_addresses,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Color mapram read",this,&MauSramRow::srams_run_color_mapram_read,kOnlyAtHeaderTime),

      // have to do meter address distribution before output_immediate_data to calculate the in_range bit for stateful counters
      new MauExecuteStepRunTables("Dist meter",this,&MauLogicalTable::distrib_meter_address_at_hdr,kOnlyAtHeaderTime),
      new MauExecuteStepMauAddrDistFunc("Final dist meter addrs",this,&MauAddrDist::finalize_meter_addresses,kOnlyAtHeaderTime),
      new MauExecuteStepRunTables("Imm data",this,&MauLogicalTable::output_immediate_data,kOnlyAtHeaderTime),

      new MauExecuteStepMauInEgFunc("Update Next Table",this,&Mau::process_update_next_table,kOnlyAtHeaderTime),
      new MauExecuteStepMauFunc("Predication End",this,&Mau::process_pred_end,kOnlyAtHeaderTime),

      new MauExecuteStepRunRows("Fetch Addrs",this,&MauSramRow::fetch_addresses,kAtHeaderAndEopTime),
      new MauExecuteStepRunRows("Claim Addrs",this,&MauSramRow::srams_claim_addrs,kAtHeaderAndEopTime),

      new MauExecuteStepRunRowsWithState("Selector Read",this,&MauSramRow::srams_run_selector_read,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Selector ALU",this,&MauSramRow::run_selector_alu_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Other SRAMs",this,&MauSramRow::srams_run_read,kAtHeaderAndEopTime),
      // have to run all the Stateful ALU's Compare ALUs to calculate the TMatch bus outputs before running the rest of Stateful ALUs
      new MauExecuteStepRunRowsWithState("Run Stateful Cmp ALUs",this,&MauSramRow::run_cmp_alus_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Run ALUs",this,&MauSramRow::run_alus_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Action Read",this,&MauSramRow::srams_run_action_read,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Run Write",this,&MauSramRow::srams_run_write,kAtHeaderAndEopTime),
      new MauExecuteStepRunRows("Drive Action HV",this,&MauSramRow::drive_action_output_hv,kAtHeaderAndEopTime),
      new MauExecuteStepMauFunc("Process Action",this,&Mau::process_action,kOnlyAtHeaderTime),
      new MauExecuteStepMauFunc("Snapshot End",this,&Mau::process_snap_end,kOnlyAtHeaderTime),
      };

    std::vector< MauExecuteStep* > at_eop_steps_{
      new MauExecuteStepRunRows("Fetch Addrs",this,&MauSramRow::fetch_addresses,kAtHeaderAndEopTime),

      new MauExecuteStepMauAddrDistFunc("EOP Dist Addrs",this,&MauAddrDist::distrib_eoptime_addresses,kOnlyAtEopTime),
          //new MauExecuteStepRunTablesEop("EOP Dist stats addr",this,&MauLogicalTable::distrib_stats_address_at_eop,kOnlyAtEopTime),
          //new MauExecuteStepRunTablesEop("EOP Dist meter addr",this,&MauLogicalTable::distrib_meter_address_at_eop,kOnlyAtEopTime),

      new MauExecuteStepRunRows("Fetch Addrs",this,&MauSramRow::fetch_addresses,kAtHeaderAndEopTime),
      new MauExecuteStepRunRows("Claim Addrs",this,&MauSramRow::srams_claim_addrs,kAtHeaderAndEopTime),

      new MauExecuteStepRunRowsWithState("Other SRAMs",this,&MauSramRow::srams_run_read,kAtHeaderAndEopTime),
      // have to run all the Stateful ALU's Compare ALUs to calculate the TMatch bus outputs before running the rest of Stateful ALUs
      new MauExecuteStepRunRowsWithState("Run Stateful Cmp ALUs",this,&MauSramRow::run_cmp_alus_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Run ALUs",this,&MauSramRow::run_alus_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Action Read",this,&MauSramRow::srams_run_action_read,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Run Write",this,&MauSramRow::srams_run_write,kAtHeaderAndEopTime),
      };

    std::vector< MauExecuteStep* > at_teop_steps_{
      new MauExecuteStepRunRows("Fetch Addrs",this,&MauSramRow::fetch_addresses,kAtHeaderAndEopTime),
      new MauExecuteStepMauAddrDistFunc("TEOP Dist Addrs",this,&MauAddrDist::distrib_teoptime_addresses,kOnlyAtTeopTime),

      new MauExecuteStepRunRows("Fetch Addrs",this,&MauSramRow::fetch_addresses,kAtHeaderAndEopTime),
      new MauExecuteStepRunRows("Claim Addrs",this,&MauSramRow::srams_claim_addrs,kAtHeaderAndEopTime),

      new MauExecuteStepRunRowsWithState("Other SRAMs",this,&MauSramRow::srams_run_read,kAtHeaderAndEopTime),
      // have to run all the Stateful ALU's Compare ALUs to calculate the TMatch bus outputs before running the rest of Stateful ALUs
      new MauExecuteStepRunRowsWithState("Run Stateful Cmp ALUs",this,&MauSramRow::run_cmp_alus_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Run ALUs",this,&MauSramRow::run_alus_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Action Read",this,&MauSramRow::srams_run_action_read,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Run Write",this,&MauSramRow::srams_run_write,kAtHeaderAndEopTime),
      };

    std::vector< MauExecuteStep* > pbus_rd_steps_{
      new MauExecuteStepTableFunc("Distrib PBUS Addr Early",this,&MauLogicalTable::distrib_pbus_address_early,kAtHeaderAndEopTime),
      new MauExecuteStepRunRows("Fetch Addrs",this,&MauSramRow::fetch_addresses,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Color mapram read",this,&MauSramRow::srams_run_color_mapram_read,kOnlyAtHeaderTime),
      new MauExecuteStepTableFunc("Distrib PBUS Addr",this,&MauLogicalTable::distrib_pbus_address,kAtHeaderAndEopTime),
      new MauExecuteStepRunRows("Fetch Addrs",this,&MauSramRow::fetch_addresses,kAtHeaderAndEopTime),
      new MauExecuteStepRunRows("Claim Addrs",this,&MauSramRow::srams_claim_addrs,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Selector Read",this,&MauSramRow::srams_run_selector_read,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Selector ALU",this,&MauSramRow::run_selector_alu_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Other SRAMs",this,&MauSramRow::srams_run_read,kAtHeaderAndEopTime),
      // have to run all the Stateful ALU's Compare ALUs to calculate the TMatch bus outputs before running the rest of Stateful ALUs
      new MauExecuteStepRunRowsWithState("Run Stateful Cmp ALUs",this,&MauSramRow::run_cmp_alus_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Run ALUs",this,&MauSramRow::run_alus_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Action Read",this,&MauSramRow::srams_run_action_read,kAtHeaderAndEopTime),
      };

    std::vector< MauExecuteStep* > pbus_wr_steps_{
      new MauExecuteStepTableFunc("Distrib PBUS Addr",this,&MauLogicalTable::distrib_pbus_address,kAtHeaderAndEopTime),
      new MauExecuteStepRunRows("Fetch Addrs",this,&MauSramRow::fetch_addresses,kAtHeaderAndEopTime),
      new MauExecuteStepRunRows("Claim Addrs",this,&MauSramRow::srams_claim_addrs,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Selector Read",this,&MauSramRow::srams_run_selector_read,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Selector ALU",this,&MauSramRow::run_selector_alu_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Other SRAMs",this,&MauSramRow::srams_run_read,kAtHeaderAndEopTime),
      // have to run all the Stateful ALU's Compare ALUs to calculate the TMatch bus outputs before running the rest of Stateful ALUs
      new MauExecuteStepRunRowsWithState("Run Stateful Cmp ALUs",this,&MauSramRow::run_cmp_alus_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Run ALUs",this,&MauSramRow::run_alus_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Action Read",this,&MauSramRow::srams_run_action_read,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Run Write",this,&MauSramRow::srams_run_write,kAtHeaderAndEopTime),
      };

    std::vector< MauExecuteStep* > sweep_steps_{
      new MauExecuteStepTableFunc("Distrib PBUS Addr",this,&MauLogicalTable::distrib_pbus_address,kAtHeaderAndEopTime),
      new MauExecuteStepRunRows("Fetch Addrs",this,&MauSramRow::fetch_addresses,kAtHeaderAndEopTime),
      new MauExecuteStepRunRows("Claim Addrs",this,&MauSramRow::srams_claim_addrs,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Selector Read",this,&MauSramRow::srams_run_selector_read,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Selector ALU",this,&MauSramRow::run_selector_alu_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Other SRAMs",this,&MauSramRow::srams_run_read,kAtHeaderAndEopTime),
      // have to run all the Stateful ALU's Compare ALUs to calculate the TMatch bus outputs before running the rest of Stateful ALUs
      new MauExecuteStepRunRowsWithState("Run Stateful Cmp ALUs",this,&MauSramRow::run_cmp_alus_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Run ALUs",this,&MauSramRow::run_alus_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Action Read",this,&MauSramRow::srams_run_action_read,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Run Write",this,&MauSramRow::srams_run_write,kAtHeaderAndEopTime),
      };

    std::vector< MauExecuteStep* > stateful_clear_steps_{
      new MauExecuteStepRunRows("Fetch Addrs",this,&MauSramRow::fetch_addresses,kAtHeaderAndEopTime),
      new MauExecuteStepRunRows("Claim Addrs",this,&MauSramRow::srams_claim_addrs,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Other SRAMs",this,&MauSramRow::srams_run_read,kAtHeaderAndEopTime),
      // have to run all the Stateful ALU's Compare ALUs to calculate the TMatch bus outputs before running the rest of Stateful ALUs
      new MauExecuteStepRunRowsWithState("Run Stateful Cmp ALUs",this,&MauSramRow::run_cmp_alus_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Run ALUs",this,&MauSramRow::run_alus_with_state,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Action Read",this,&MauSramRow::srams_run_action_read,kAtHeaderAndEopTime),
      new MauExecuteStepRunRowsWithState("Run Write",this,&MauSramRow::srams_run_write,kAtHeaderAndEopTime),
      };

  };
}
#endif // _SHARED_MAU_
