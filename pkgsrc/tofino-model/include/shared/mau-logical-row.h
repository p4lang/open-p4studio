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

#ifndef _SHARED_MAU_LOGICAL_ROW_
#define _SHARED_MAU_LOGICAL_ROW_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <mau-sram.h>
#include <mau-logical-row-reg.h>
#include <mau-stats-alu.h>
#include <mau-meter-alu.h>
#include <mau-selector-alu.h>
#include <phv.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauExecuteState;
  class MauAddrDist;
  class MauSramRow;

  class MauLogicalRow : public MauObject {

    static constexpr int kType = RmtTypes::kRmtTypeMauLogicalRow;
    static constexpr int kLogicalColumns = MauDefs::kLogicalColumnsPerMau;
    static constexpr int kLogicalTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int kActionHVOutputBusWidth = MauDefs::kActionHVOutputBusWidth;
    static constexpr int kActionOutputBuses = MauDefs::kActionOutputBusesPerLogicalRow;
    static constexpr int kActionOutputBusWidth = MauDefs::kActionOutputBusWidth;
    static constexpr int kDataBusWidth = MauDefs::kDataBusWidth;
    static constexpr int kSramWidth = MauDefs::kSramWidth;

    static constexpr uint32_t kStatsAluLogicalRows = MauDefs::kStatsAluLogicalRows;
    static constexpr uint32_t kMeterAluLogicalRows = MauDefs::kMeterAluLogicalRows;
    static constexpr uint32_t kSelectorAluLogicalRows = MauDefs::kSelectorAluLogicalRows;
    static constexpr uint32_t kStatefulAluLogicalRows = MauDefs::kStatefulAluLogicalRows;
    static constexpr int kStatefulMeterAluDataBits = MauDefs::kStatefulMeterAluDataBits;

 public:
    static bool kRelaxRowSelectorShiftCheck; // All these defined in rmt-config.cpp
    static bool kRelaxDataMultiWriteCheck;
    static bool kRelaxAddrMultiWriteCheck;



    MauLogicalRow(RmtObjectManager *om, int pipeIndex, int mauIndex,
                  int logicalRowIndex, Mau *mau, int physicalRowIndex, int physicalRowWhich);
    virtual ~MauLogicalRow();

    inline int      row_index()                   const { return logical_row_index_; }
    inline int      logical_row_index()           const { return logical_row_index_; }
    inline int      physical_row_index()          const { return physical_row_index_; }
    inline MauSram *sram_lookup(int logcol)             { return srams_[logcol]; }
    inline void     sram_set(int logcol, MauSram *sram) {
      RMT_ASSERT((logcol >= 0) && (logcol < kLogicalColumns));
      srams_[logcol] = sram;
    }
    inline MauSramRow  *physical_row()                  const { return physical_row_; }
    inline void         set_physical_row(MauSramRow *physRow) { physical_row_ = physRow; }

    inline MauStatsAlu      *mau_stats_alu()     { return mau_stats_alu_; }
    inline MauMeterAlu      *mau_meter_alu()     { return mau_meter_alu_; }
    inline MauSelectorAlu   *mau_selector_alu()  { return mau_selector_alu_; }
    inline MauLogicalRowReg *logrow_registers()  { return &mau_logical_row_reg_; }

    inline uint32_t          get_addr_mux_vals() { return mau_logical_row_reg_.get_addr_mux_vals(); }
    inline uint32_t          selector_rd_addr()  { return selector_rd_addr_; }

    inline bool              consumes_sel_addr() { return ((get_addr_mux_vals() & (1u<<6)) != 0u); }
    inline bool              produces_sel_addr() {
      if ((physical_row_ == NULL) || (physical_row_->row_registers() == NULL)) return false;
      return physical_row_->row_registers()->produces_sel_addr();
    }



    // Stash logical table of stats SRAM hit on logical row
    inline int stats_logical_table() const {
      return (stats_logical_table_ < kLogicalTables) ?static_cast<int>(stats_logical_table_) :-1;
    }
    inline void set_stats_logical_table(int lt) {
      stats_logical_table_ = ((lt >= 0) && (lt < kLogicalTables)) ?static_cast<uint8_t>(lt) :0xFF;
    }

    // Access per-logicalRow action/stats/meter/oflow addr/data buses
    inline MauAddrDist *mau_addr_dist() const { return mau_addr_dist_; }

    // SELECTOR stuff
    // Per-row selector ALU output - raw 7bit value
    // Either from this row (selector_addr_raw) or
    // overflowed from another row (selector_oflo|oflo2_addr_raw)
    void selector_addr_raw(uint32_t *addr);
    void selector_oflo_addr_raw(uint32_t *addr);
    void selector_oflo2_addr_raw(uint32_t *addr);
    // Per-row selector shift (used by selector_X_index|addr funcs)
    int selector_addr_shift();
    int selector_oflo_addr_shift();
    int selector_oflo2_addr_shift();
    // Per-row selector ALU index - shifted selector ALU output
    void selector_addr_index(uint32_t *addr);
    void selector_oflo_addr_index(uint32_t *addr);
    void selector_oflo2_addr_index(uint32_t *addr);

    // ADDRs (for MauSram)
    void action_addr(uint32_t *addr, bool ingress);
    void stats_addr(uint32_t *addr, bool ingress);
    void meter_addr(uint32_t *addr, bool ingress);
    void oflo_addr(uint32_t *addr, bool ingress, uint8_t addrtype);
    void oflo2_addr(uint32_t *addr, bool ingress, uint8_t addrtype);
    void selector_addr(uint32_t *addr, bool ingress);
    void selector_oflo_addr(uint32_t *addr, bool ingress);
    void oflo_selector_addr(uint32_t *addr, bool ingress, uint8_t addrtype);
    void selector_oflo2_addr(uint32_t *addr, bool ingress);
    // WADDRs (for MauSram)
    void stats_waddr(uint32_t *waddr);
    void meter_waddr(uint32_t *waddr);
    void oflow_waddr(uint32_t *waddr);
    void oflow2_waddr(uint32_t *waddr);

    void oflow_color_write_data(uint8_t *color);


    // ADDRs/wADDRs (for MauSramRow)
    void addr_multi_write_check(const char *bus_name, uint32_t *addrA, uint32_t addrB);
    void set_selector_rd_addr(uint32_t addr);
    // Neither these 2 used any more
    void set_action_rd_addr(uint32_t *addr);
    void set_stats_rd_addr(uint32_t *addr);

    void action_rd_addr(uint32_t *addr);
    void stats_rd_addr(uint32_t *addr);
    void meter_rd_addr(uint32_t *addr);
    void oflo_rd_addr(uint32_t *addr, uint8_t addrtype);
    void oflo2_rd_addr(uint32_t *addr, uint8_t addrtype);
    void selector_rd_addr(uint32_t *addr);
    void action_sel_rd_addr(uint32_t *addr);

    void stats_wr_addr(uint32_t *addr);
    void meter_wr_addr(uint32_t *addr);
    void set_stats_wr_addr(uint32_t *addr);
    void set_meter_wr_addr(uint32_t *addr);
    void oflo_wr_addr(uint32_t *addr);
    void oflo2_wr_addr(uint32_t *addr);

    void color_write_data(uint8_t* data);
    void set_color_write_data(uint8_t* data);
    bool color_write_data_was_set();

    // DATA/wrDATA (for MauSramRow)
    void data_multi_write_check(const char *bus_name,
                                BitVector<kDataBusWidth> *dataA,
                                const BitVector<kDataBusWidth>& dataB);
    void action_rd_data(BitVector<kDataBusWidth> *data);
    void stats_rd_data(BitVector<kDataBusWidth> *data);
    void stats_alu_rd_data(BitVector<kDataBusWidth> *data); // also for meters
    // get_selector_alu_input_data() is used by DV to get the input for selectors instead of
    //   stats_alu_rd_data (which is used for all other alus) because selectors have strange
    //   forwarding behaviour so the input must be got from the selector alu itself
    void get_selector_alu_input_data(BitVector<kDataBusWidth> *data);
    // And now meter ALUs have strange forwarding behaviour hence this func
    void get_meter_alu_input_data(BitVector<kDataBusWidth> *data);
    void stats_wr_data(BitVector<kDataBusWidth> *data);
    void meter_rd_data(BitVector<kDataBusWidth> *data);
    void meter_wr_data(BitVector<kDataBusWidth> *data);
    void oflo_rd_data(BitVector<kDataBusWidth> *data);
    void oflo2_rd_data(BitVector<kDataBusWidth> *data);
    void oflo_wr_data(BitVector<kDataBusWidth> *data);
    void oflo2_wr_data(BitVector<kDataBusWidth> *data);
    // DATA setters (from MauSram or from MauLogicalRow ALUs)
    void set_action_rd_data_nocheck(BitVector<kDataBusWidth> *data);
    void set_action_rd_data(BitVector<kDataBusWidth> *data);
    void set_stats_rd_data(BitVector<kDataBusWidth> *data);
    void set_stats_wr_data(BitVector<kDataBusWidth> *data);
    void set_meter_rd_data(BitVector<kDataBusWidth> *data);
    void set_meter_wr_data(BitVector<kDataBusWidth> *data);
    void set_oflo_rd_data(BitVector<kDataBusWidth> *data);
    void set_oflo2_rd_data(BitVector<kDataBusWidth> *data);
    void clear_stats_rd_data();
    void clear_stats_wr_data();


    // Cleanup between PHVs
    void reset_resources();
    // Run ALUs etc
    bool run_alus_with_phv(Phv *phv);
    bool run_cmp_alus_with_phv(Phv *phv);
    bool run_selector_alu_with_state(MauExecuteState *state);
    bool run_alus_with_state(MauExecuteState *state);
    bool run_cmp_alus_with_state(MauExecuteState *state);
    void fetch_addresses();


    BitVector<kStatefulMeterAluDataBits> get_meter_stateful_selector_alu_data(Phv *phv) {
      return physical_row_->get_meter_stateful_selector_alu_data(phv, physical_row_which_);
    }
    bool synth2port_vpn_valid(int vpn) {
      RMT_ASSERT(physical_row_which_ == 1); // Must be RHS logrow
      return physical_row_->synth2port_vpn_valid(vpn);
    }
    int get_home_alu(uint8_t *addrtype, uint8_t *alutype) {
      // Func assumes only one of Stats|Meter ALU per-row
      // (also we ignore SelectorALU as that's just a MeterALU really)
      if (mau_stats_alu_ != NULL) {
        RMT_ASSERT((physical_row_which_ == 1) && (mau_meter_alu_ == NULL));
        if (addrtype != NULL) *addrtype = AddrType::kStats;
        if (alutype != NULL)  *alutype  = MauDefs::kAluTypeStats;
        return MauStatsAlu::get_stats_alu_regs_index(logical_row_index_);
      } else if (mau_meter_alu_ != NULL) {
        RMT_ASSERT((physical_row_which_ == 1) && (mau_stats_alu_ == NULL));
        if (addrtype != NULL) *addrtype = AddrType::kMeter;
        if (alutype != NULL)  *alutype  = MauDefs::kAluTypeMeter;
        return MauMeterAlu::get_meter_alu_regs_index(logical_row_index_);
      } else {
        if (addrtype != NULL) *addrtype = AddrType::kNone;
        if (alutype != NULL)  *alutype  = MauDefs::kAluTypeInvalid;
        return -1;
      }
    }
    bool get_alus(int *home_alu, uint8_t *home_addrtype, uint8_t *home_alutype,
                  int *oflow_alu, uint8_t *oflow_addrtype, uint8_t *oflow_alutype) {
      RMT_ASSERT((home_alu != NULL) && (oflow_alu != NULL));
      *home_alu  = get_home_alu(home_addrtype, home_alutype);
      *oflow_alu = mau_logical_row_reg_.get_oflow_alu(oflow_addrtype, oflow_alutype);
      return ((*home_alu >= 0) || (*oflow_alu >= 0));
    }



 private:
    DISALLOW_COPY_AND_ASSIGN(MauLogicalRow);
    void addr_mux_change_callback(uint32_t l_or_r, uint32_t logcol_index);


 private:
    int                                                               logical_row_index_;
    int                                                               physical_row_index_;
    int                                                               physical_row_which_;
    std::array<MauSram*,kLogicalColumns>                              srams_;
    std::array< BitVector<kActionOutputBusWidth>, kActionOutputBuses> action_output_buses_;
    std::array< bool, kActionOutputBuses>                             action_output_bus_in_use_;

    uint32_t                                                          action_rd_addr_;
    uint32_t                                                          stats_rd_addr_;
    uint32_t                                                          meter_rd_addr_;
    uint32_t                                                          oflo_rd_addr_;
    uint32_t                                                          oflo2_rd_addr_;
    uint32_t                                                          selector_rd_addr_;

    uint32_t                                                          stats_wr_addr_;
    uint32_t                                                          meter_wr_addr_;
    uint8_t                                                           color_write_data_;
    bool                                                              color_write_data_was_set_;
    uint8_t                                                           stats_logical_table_;

    BitVector<kDataBusWidth>                                          action_rd_data_;
    BitVector<kDataBusWidth>                                          stats_rd_data_;
    BitVector<kDataBusWidth>                                          stats_wr_data_;
    BitVector<kDataBusWidth>                                          meter_rd_data_;
    BitVector<kDataBusWidth>                                          meter_wr_data_;
    BitVector<kDataBusWidth>                                          oflo_rd_data_;
    BitVector<kDataBusWidth>                                          oflo2_rd_data_;

    MauAddrDist                                                      *mau_addr_dist_;
    MauSramRow                                                       *physical_row_;
    MauStatsAlu                                                      *mau_stats_alu_;
    MauMeterAlu                                                      *mau_meter_alu_;
    MauSelectorAlu                                                   *mau_selector_alu_;
    MauLogicalRowReg                                                  mau_logical_row_reg_;
  };
}
#endif // _SHARED_MAU_LOGICAL_ROW_
