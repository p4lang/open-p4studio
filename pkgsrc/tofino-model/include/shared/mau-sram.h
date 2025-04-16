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

#ifndef _SHARED_MAU_SRAM_
#define _SHARED_MAU_SRAM_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <mau-sram-reg.h>
#include <address.h>
#include <sram.h>
#include <phv.h>


namespace MODEL_CHIP_NAMESPACE {

  class Mau;
  class MauResultBus;
  class MauLogicalRow;
  class MauSramRow;
  class MauSramColumn;
  class MauMapram;


  class MauSram : public MauObject {
 public:
    static bool kRelaxSramVpnCheck;   // Defined in rmt-config.cpp
    static bool kRelaxSramEmptyCheck; // Defined in rmt-config.cpp

    static constexpr int kType = RmtTypes::kRmtTypeMauSram;
    static constexpr int kExactMatchInputBits = MauDefs::kExactMatchInputBits;
    static constexpr int kExactMatchValidBits = MauDefs::kExactMatchValidBits;
    static constexpr int kNextTableWidth = MauDefs::kNextTableWidth;

    static constexpr int kSramWidth = MauDefs::kSramWidth;
    static constexpr int kSramAddressWidth  = MauDefs::kSramAddressWidth;
    static constexpr int kSramEntries = 1<<kSramAddressWidth;
    static constexpr int kSramColumns = MauDefs::kSramColumnsPerMau;
    static constexpr int kSramColumnFirstRHS = MauDefs::kSramColumnFirstRHS;
    static constexpr int kMatchEntries = MauDefs::kMatchesPerSram;
    static constexpr int kMaskEntries = MauDefs::kMatchesPerSram;
    static constexpr int kMatchOutputBusWidth = MauDefs::kMatchOutputBusWidth;
    static constexpr int kTindOutputBusWidth = MauDefs::kTindOutputBusWidth;
    static constexpr int kDataBusWidth = MauDefs::kDataBusWidth;
    static constexpr int kLogicalTcamsPerMau = MauDefs::kLogicalTcamsPerMau;
    static constexpr int kVpnsPerMau = MauDefs::kVpnsPerMau;
    static constexpr int kVpns = MauDefs::kVpnsPerSram;
    static constexpr uint8_t kMapramVpnUnoccupied = MauDefs::kMapramVpnUnoccupied;
    static_assert( (kMatchEntries <= 8), "Hit Mask/Inhibit Mask stored in uint8_t");
    static_assert( (kSramEntries < 0xFFFF), "Index 0xFFFF assumed to be invalid");


    MauSram(RmtObjectManager *om, int pipeIndex, int mauIndex,
            int rowIndex, int colIndex, int sramIndex, Mau *mau);
    virtual ~MauSram();

    inline int col_index()                     const { return col_index_; }
    inline MauSramColumn *column()             const { return column_; }
    inline void set_column(MauSramColumn *column)    { column_ = column; }
    inline int row_index()                     const { return row_index_; }
    inline MauSramRow *row()                   const { return row_; }
    inline void set_row(MauSramRow *row)             { row_ = row; }
    inline MauLogicalRow *logical_row()        const { return logrow_; }
    inline void set_logical_row(MauLogicalRow *row)  { logrow_ = row; }
    inline MauMapram *mapram()                 const { return mapram_; }
    inline void set_mapram(MauMapram *mapram)        { mapram_ = mapram; }
    inline int sram_index()                    const { return sram_index_; }
    inline int logrow_index()                  const { return logrow_index_; }
    inline int logcol_index()                  const { return logcol_index_; }
    inline uint8_t hit_mask()                  const { return hit_mask_; }
    inline uint8_t inhibit_mask()              const { return inhibit_hit_mask_; }
    inline uint16_t inhibit_index()            const { return inhibit_hit_index_; }
    inline int prio() const {
      // Col prio is hi->lo, col 0,1,2,3,4,5,11,10,9,8,7,6 (with prio 24->19, 11->6)
      if (col_index() < kSramColumnFirstRHS)
        return kSramColumns + kSramColumns - col_index();
      else
        return col_index();
    }

    inline void read(const int index, uint64_t *data0, uint64_t *data1, uint64_t T) const {
      BitVector<kSramWidth> val;
      if (get(index, &val)) {
        if (data0 != NULL) *data0 = val.get_word(0);
        if (data1 != NULL) *data1 = val.get_word(64);
      }
    }
    inline void read_and_clear(const int index, uint64_t *data0, uint64_t *data1,
                               const BitVector<kSramWidth> &clear_mask,
                               bool unless_exactly_equals_mask) {
      BitVector<kSramWidth> val;
      if (get_and_clear(index, &val, clear_mask, unless_exactly_equals_mask)) {
        if (data0 != NULL) *data0 = val.get_word(0);
        if (data1 != NULL) *data1 = val.get_word(64);
      }
    }
    inline void write(const int index, uint64_t data0, uint64_t data1, uint64_t T) {
      BitVector<kSramWidth> val;
      val.set_word(data0, 0);
      val.set_word(data1, 64);
      set(index, val);
    }
    inline void write_masked(const int index, uint64_t data0, uint64_t data1,
                             const BitVector<kSramWidth> &mask) {
      BitVector<kSramWidth> val;
      val.set_word(data0, 0);
      val.set_word(data1, 64);
      set_masked(index, val, mask);
    }

    inline bool get(const int index, BitVector<kSramWidth> *val) const {
      if ((val == NULL) || (index < 0) || (index >= kSramEntries)) return false;
      return sram_.get(index, val);
    }
    inline bool get_and_clear(const int index, BitVector<kSramWidth> *val,
                              const BitVector<kSramWidth> &clear_mask,
                              bool unless_exactly_equals_mask) {
      if ((val == NULL) || (index < 0) || (index >= kSramEntries)) return false;
      return sram_.get_and_clear(index, val, clear_mask, unless_exactly_equals_mask);
    }
    inline void set(const int index, const BitVector<kSramWidth> &val) {
      if ((index < 0) || (index >= kSramEntries)) return;
      sram_.set(index, val);
    }
    inline void set_masked(const int index, const BitVector<kSramWidth> &val,
                           const BitVector<kSramWidth> &mask) {
      if ((index < 0) || (index >= kSramEntries)) return;
      sram_.set_masked(index, val, mask);
    }

    inline bool get_mask(const int index, BitVector<kSramWidth> *val) const {
      if (val == NULL) return false;
      return ((index >= 0) && (index < kMaskEntries)) ?masks_.get(index, val) :false;
    }
    inline void set_mask(const int index, const BitVector<kSramWidth> &val) {
      if ((index >= 0) && (index < kMaskEntries)) masks_.set(index, val);
    }

    inline void set_logical_tcam(const int new_logical_tcam) {
      if (new_logical_tcam != mau_sram_reg_.get_logical_tcam()) {
          RMT_LOG_VERBOSE("SRAM <-> LogicalTcam MISMATCH");
      }
      update(new_logical_tcam, logical_table_, type_);
    }
    inline int get_logical_tcam() { return logical_tcam_; }

    inline void OLD_UNUSED_set_logical_table(const int new_logical_table) {
      if (new_logical_table !=  mau_sram_reg_.get_logical_table()) {
        RMT_LOG_VERBOSE("SRAM <-> LogicalTable MISMATCH");
      }
      logical_table_ = new_logical_table;
    }
    inline int get_logical_table()  { return logical_table_; }

    inline void set_logical_tables(uint16_t new_logical_tables) {
      //if (new_logical_tables != column_->get_all_xm_logical_tables_for_row(row_index_)) {
      //    RMT_LOG_VERBOSE("SRAM <-> LogicalTables MISMATCH");
      //}
      update_logical_tables(new_logical_tables, logical_tables_);
    }
    inline uint32_t get_logical_tables() { return logical_tables_; }

    inline void set_type(const int new_type) {
      if (new_type != mau_sram_reg_.get_ram_type()) {
          RMT_LOG_VERBOSE("SRAM <-> Type MISMATCH");
      }
      update(logical_tcam_, logical_table_, new_type);
    }
    inline int get_type() { return type_; }

    inline bool check_ingress_egress(bool ingress) {
      return mau_sram_reg_.check_ingress_egress(ingress);
    }
    inline uint8_t match_enables() {
      return mau_sram_reg_.match_enables();
    }
    inline bool is_match_sram() {
      return mau_sram_reg_.is_match_sram();
    }
    inline int get_match_result_bus() {
      return mau_sram_reg_.get_match_result_bus();
    }
    inline int get_match_result_buses() {
      return mau_sram_reg_.get_match_result_buses();
    }
    inline int use_match_result_bus(int bus) {
      return mau_sram_reg_.use_match_result_bus(bus);
    }
    inline bool is_tind_sram() {
      return mau_sram_reg_.is_tind_sram();
    }
    inline int get_tind_result_bus() {
      return mau_sram_reg_.get_tind_result_bus();
    }
    inline int get_tind_result_buses() {
      return mau_sram_reg_.get_tind_result_buses();
    }
    inline int use_tind_result_bus(int bus) {
      return mau_sram_reg_.use_tind_result_bus(bus);
    }
    inline int get_nxtab_result_bus() {
      return mau_sram_reg_.get_nxtab_result_bus();
    }
    inline int get_nxtab_result_buses() {
      return mau_sram_reg_.get_nxtab_result_buses();
    }

    inline int get_result_bus() {
      return mau_sram_reg_.get_result_bus();
    }
    inline int get_nxtab_bus() {
      return mau_sram_reg_.get_nxtab_bus();
    }

    bool lt_uses_xm_bus(int lt, int bus);
    bool lt_uses_tm_bus(int lt, int bus);

    inline bool output_rd_data(BitVector<kDataBusWidth> *data) {
      return mau_sram_reg_.output_rd_data(data);
    }
    inline bool input_rd_data(BitVector<kDataBusWidth> *data) {
      return mau_sram_reg_.input_rd_data(data);
    }
    inline bool input_wr_data(BitVector<kDataBusWidth> *data) {
      return mau_sram_reg_.input_wr_data(data);
    }
    inline int get_addr_mux() {
      return mau_sram_reg_.get_addr_mux();
    }

    inline bool is_action_sram()     { return mau_sram_reg_.is_action_sram(); }
    inline uint32_t action_addr()    { return mau_sram_reg_.action_addr(); }
    inline uint32_t action_addr_base()    { return mau_sram_reg_.action_addr_base(); }
    inline bool is_selector_action_addr() { return mau_sram_reg_.is_selector_action_addr(); }

    inline bool is_stats_sram()      { return mau_sram_reg_.is_stats_sram(); }
    inline uint32_t stats_addr()     { return mau_sram_reg_.stats_addr(); }
    inline uint32_t stats_waddr()    { return mau_sram_reg_.stats_waddr(); }

    inline bool is_meter_sram()      { return mau_sram_reg_.is_meter_sram(); }
    inline uint32_t meter_addr()     { return mau_sram_reg_.meter_addr(); }
    inline uint32_t meter_waddr()    { return mau_sram_reg_.meter_waddr(); }

    inline bool is_selector_sram()      { return mau_sram_reg_.is_selector_sram(); }
    //inline uint32_t selector_addr()     { return mau_sram_reg_.selector_addr(); }
    //inline uint32_t selector_waddr()    { return mau_sram_reg_.selector_waddr(); }

    inline bool is_stateful_sram()      { return mau_sram_reg_.is_stateful_sram(); }
    //inline uint32_t stateful_addr()     { return mau_sram_reg_.stateful_addr(); }
    //inline uint32_t stateful_waddr()    { return mau_sram_reg_.stateful_waddr(); }


    inline int get_ram_type() {
      return mau_sram_reg_.get_ram_type();
    }
    inline uint16_t get_vpn(int which_vpn) {
      return mau_sram_reg_.get_vpn(which_vpn);
    }
    inline bool matches_type(int type) {
      return mau_sram_reg_.matches_type_vpn_table(type, -1, -1);
    }
    inline bool matches_type_vpn(int type, int vpn) {
      return mau_sram_reg_.matches_type_vpn_table(type, vpn, -1);
    }
    inline bool matches_type_vpn_table(int type, int vpn, int table) {
      return mau_sram_reg_.matches_type_vpn_table(type, vpn, table);
    }

    inline uint32_t make_match_address(const int match_index, const int match_entry) {
      RMT_ASSERT((match_index >> 10) == 0);  // match addr is 19 bits - vpn<9>index<10>
      uint16_t index = static_cast<uint16_t>(match_index);
      uint16_t vpn = get_vpn(match_entry);
      return static_cast<uint32_t>(Address::xm_match_addr_make(vpn, index));
    }

    inline bool output_action_subword() {
      return mau_sram_reg_.output_action_subword();
    }

    inline MauSramRow *get_alu_row()  { return mau_sram_reg_.get_alu_row(); }
    inline int get_alu_index()        { return mau_sram_reg_.get_alu_index(); }
    inline int get_alu_logrow_index() { return mau_sram_reg_.get_alu_logrow_index(); }

    void update_vpns();
    void update(int new_ltcam, uint16_t new_logtabs, int new_type);
    void update_logical_tables(uint16_t new_logtabs, uint16_t old_logtabs);
    bool get_next_table(const int index, int which_next_table, uint32_t *next_table);
    void inhibit(const uint32_t inhibit_addr);
    void uninhibit();
    bool hit_inhibited(const int index, const int hit);
    int  lookup(const int index, BitVector<kSramWidth> &inval, uint8_t *match_masks);
    int  lookup(Phv *phv, uint8_t *match_masks);
    void set_match_output(const int lt, const int index, const int which_entry);
    void set_tind_output(const int lt, const int index, const int which_word,
                         uint8_t *which_bus, uint8_t *next_tab);

    void claim_addrs();
    void run_read();
    void run_selector_read();
    void run_action_read();
    void run_write();
    int8_t claim_pri(uint8_t addrtype, int vpn, uint16_t index);
    bool stats_claim(int pri);
    bool meter_claim(int pri);
    void claim_addrs_stats_sram();
    void claim_addrs_meter_sram();
    void claim_addrs_selector_sram();
    void claim_addrs_stateful_sram();

    bool run_read_action_sram();
    bool run_read_stats_sram();
    bool run_read_meter_sram(const char *sramtype="meter");
    bool run_read_selector_sram();
    bool run_read_stateful_sram();
    bool run_write_stats_sram();
    bool run_write_meter_sram(const char *sramtype="meter");
    bool run_write_selector_sram();
    bool run_write_stateful_sram();


    void match_nibble_s1q0bar_enable_update(uint32_t new_value);
    void match_nibble_s0q1bar_enable_update(uint32_t new_value);
    // also used by stashes
    static void match_nibble_sXqXbar_enable_update(uint32_t new_value,
                                                   BitVector<MauDefs::kSramWidth>* bv);
    void reset_resources();

    // Pending Write FSM
    static constexpr uint8_t kPendingStateReset   = 0; // Unlocked
    static constexpr uint8_t kPendingStateReady   = 1; // Index+Data written
    static constexpr uint8_t kPendingStateLocked  = 2; // Locked and about to commit
    static constexpr uint8_t kPendingStateWritten = 3; // Committed
    static constexpr uint8_t kPendingStateDone    = 4; // Unlocked

    bool pending_index(int *index); // Return true if index IS pending
    bool pending_get(const int index, uint64_t *data0, uint64_t *data1, uint64_t T);
    void pending_set(const int index, uint64_t data0, uint64_t data1, uint64_t T);
    bool pending_lock(); // Returns true if managed to lock
    void pending_flush();
    void pending_unlock();
    void pending_unset();


 private:
    int                             row_index_;
    int                             col_index_;
    int                             sram_index_;
    int                             logrow_index_;
    int                             logcol_index_;
    int                             logical_tcam_;
    int                             logical_table_;
    uint16_t                        logical_tables_;
    uint8_t                         hit_mask_;
    uint8_t                         inhibit_hit_mask_;
    uint16_t                        inhibit_hit_index_;
    int                             type_;
    uint8_t                         pending_state_;
    uint16_t                        pending_index_;
    uint64_t                        pending_data0_;
    uint64_t                        pending_data1_;
    MauResultBus                   *result_bus_;
    MauLogicalRow                  *logrow_;
    MauSramRow                     *row_;
    MauSramColumn                  *column_;
    MauMapram                      *mapram_;
    std::array<uint16_t,kVpns>      vpns_;
    Sram<kSramEntries,kSramWidth>   sram_;
    Sram<kMaskEntries,kSramWidth>   masks_;
    MauSramReg                      mau_sram_reg_;
    BitVector<MauDefs::kSramWidth>  s0q1_enable_{};
    BitVector<MauDefs::kSramWidth>  s1q0_enable_{};
  };
}
#endif // _SHARED_MAU_SRAM_
