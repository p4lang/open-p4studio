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

#ifndef _SHARED_MAU_SRAM_ROW_
#define _SHARED_MAU_SRAM_ROW_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <mau-sram.h>
#include <mau-sram-row-reg.h>
#include <match-data-input-vh-xbar-with-reg.h>
#include <hash-address-vh-xbar-with-reg.h>
#include <action-output-hv-xbar.h>
#include <mau-gateway-table.h>
#include <mau-gateway-payload.h>
#include <mau-stash.h>
#include <phv.h>
#include <mau-color-switchbox.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauExecuteState;
  class MauInput;


  class MauSramRow : public MauObject {

    static constexpr int kType = RmtTypes::kRmtTypeMauSramRow;
    static constexpr int kExactMatchInputBits = MauDefs::kExactMatchInputBits;
    static constexpr int kExactMatchValidBits = MauDefs::kExactMatchValidBits;

    static constexpr int kSearchBuses = MauDefs::kSearchBusesPerRow;
    static constexpr int kMatchOutputBuses = MauDefs::kMatchOutputBusesPerRow;
    static constexpr int kMatchOutputBusWidth = MauDefs::kMatchOutputBusWidth;
    static constexpr int kTindOutputBuses = MauDefs::kTindOutputBusesPerRow;
    static constexpr int kTindOutputBusWidth = MauDefs::kTindOutputBusWidth;
    static constexpr int kDataBusWidth = MauDefs::kDataBusWidth;
    static constexpr int kActionOutputBusWidth = MauDefs::kActionOutputBusWidth;
    static constexpr int kActionHVOutputBusWidth = MauDefs::kActionHVOutputBusWidth;
    static constexpr int kSramColumns = MauDefs::kSramColumnsPerMau;
    static constexpr int kSramWidth = MauDefs::kSramWidth;
    static constexpr int kSramAddressWidth  = MauDefs::kSramAddressWidth;
    static constexpr int kHashOutputWidth   = MauDefs::kHashOutputWidth;
    static constexpr int kGatewayTablesPerRow = MauDefs::kGatewayTablesPerRow;
    static constexpr int kGatewayPayloadsPerRow = MauDefs::kGatewayPayloadsPerRow;
    static constexpr int kStatefulMeterAluDataBits = MauDefs::kStatefulMeterAluDataBits;
    static const int kPriorityPerColumnTab[kSramColumns];
    static const int kPriGroupPerColumnTab[kSramColumns];


 public:
    static bool kRelaxMatchBusMultiWriteCheck; // Defined in rmt-config.cpp
    static bool kRelaxTindBusMultiWriteCheck;

    static int col_prio(int col) {
      return ((col >= 0) && (col < kSramColumns)) ?kPriorityPerColumnTab[col] :col;
    }
    static int col_prio_group(int col) {
      return ((col >= 0) && (col < kSramColumns)) ?kPriGroupPerColumnTab[col] :col;
    }



    MauSramRow(RmtObjectManager *om, int pipeIndex, int mauIndex, int rowIndex,
               Mau *mau, MauInput *mau_input);
    virtual ~MauSramRow();

    inline int      row_index()                const { return row_index_; }
    inline MauSram *sram_lookup(int col)             { return srams_[col]; }
    inline void     sram_set(int col, MauSram *sram) {
      RMT_ASSERT((col >= 0) && (col < kSramColumns));
      srams_[col] = sram;
    }

    inline MauSramRowReg *row_registers()  { return &mau_sram_row_reg_; }

    inline MauSramRow *row_above()  const { return row_above_; }
    inline void        set_row_above(MauSramRow *a) {
      row_above_ = a; mau_sram_row_reg_.set_row_above(a);
    }
    inline MauSramRow *row_below()  const { return row_below_; }
    inline void        set_row_below(MauSramRow *b) {
      row_below_ = b; mau_sram_row_reg_.set_row_below(b);
    }

    inline MauLogicalRow *logrow_left()  const { return logrow_left_; }
    inline void           set_logrow_left(MauLogicalRow *l)  {
      logrow_left_ = l; mau_sram_row_reg_.set_logrow_left(l);
    }
    inline MauLogicalRow *logrow_right() const { return logrow_right_; }
    inline void           set_logrow_right(MauLogicalRow *r) {
      logrow_right_ = r; mau_sram_row_reg_.set_logrow_right(r);
    }
    inline HashAddressVhXbarWithReg *hash_address_reg() {
      return (&hash_address_);
    }
    inline bool synth2port_vpn_valid(int vpn) {
      return mau_sram_row_reg_.synth2port_vpn_valid(vpn);
    }


    void get_input(Phv *phv,
                   BitVector<kExactMatchInputBits> *input_bits,
                   BitVector<kExactMatchValidBits> *valid_bits);
    int  get_hash_index(Phv *phv, int column);
    void get_match_data(Phv *phv, int search_bus, BitVector<kSramWidth> *match_data);
    void get_hash(Phv *phv, int search_bus, BitVector<kHashOutputWidth> *hash);
    void reset_resources();

    void set_match_output_bus(int which_bus, const BitVector<kMatchOutputBusWidth>& output, int col);
    bool get_match_output_bus(int which_bus, BitVector<kMatchOutputBusWidth> *output);
    uint64_t get_match_output_data(int which_bus);

    BitVector<kStatefulMeterAluDataBits> get_meter_stateful_selector_alu_data(Phv *phv, int which_bus);

    void get_gateway_table_result(Phv *phv, int which_table, bool* hit, int *hit_index);
    // informational for checking
    uint8_t get_gateway_table_logical_table(int which_table) {
      return gateway_tables_[which_table].GetLogicalTable();
    }

    void set_tind_output_bus(int which_bus, const BitVector<kTindOutputBusWidth>& output, int col);
    bool get_tind_output_bus(int which_bus, BitVector<kTindOutputBusWidth> *output);
    void set_tcam_match_addr(int which_bus, uint32_t match_addr);
    uint32_t get_tcam_match_addr(int which_bus);

    void put_gateway_payload_on_bus(int which) { gateway_payloads_[which].put_payload_on_bus(); }
    uint32_t get_payload_adr(int which) { return gateway_payloads_[which].get_payload_adr(); }
    void get_gateway_payload_result_busses_enabled(int which, bool* exact_match,bool* tind) {
      gateway_payloads_[which].get_result_busses_enabled(exact_match,tind);
    }
    void get_gateway_payload_disable(int which, bool* exact_match,bool* tind) {
      gateway_payloads_[which].get_disable(exact_match,tind);
    }

    MauStash* get_stash() { return &stash_; }

    void fetch_addresses();
    void srams_claim_addrs();
    void srams_run_read(MauExecuteState *state);
    void srams_run_color_mapram_read(MauExecuteState *state);
    void srams_run_selector_read(MauExecuteState *state);
    void srams_run_action_read(MauExecuteState *state);
    void run_selector_alu_with_state(MauExecuteState *state);
    void run_alus_with_state(MauExecuteState *state);
    void run_cmp_alus_with_state(MauExecuteState *state);
    void srams_run_write(MauExecuteState *state);
    void drive_action_output_hv();

    void set_output(const BitVector<kActionOutputBusWidth>& input_data,
                    uint32_t input_addr, int which_bus,
                    BitVector<kActionHVOutputBusWidth> *output);

    // expand 2 valid bits into the 4 bit format needed for exact match lookup
    uint8_t expand_valid_bits(uint8_t in) {
      return ( ( in & 3 ) | ( ((~in) & 3) << 2 ) );
    }

    void set_right_color_bus(uint8_t v, int mapram_col, int alu_index) {
      mau_color_switchbox_.set_right_color_bus(v, mapram_col, alu_index);
    }
    uint8_t get_right_color_bus() { return mau_color_switchbox_.get_right_color_bus(); }

    void set_right_color_overflow_bus(uint8_t v, int mapram_col, int alu_index) {
      mau_color_switchbox_.set_right_overflow_bus(v, mapram_col, alu_index);
    }
    uint8_t get_right_color_overflow_bus() { return mau_color_switchbox_.get_right_overflow_bus(); }

    void get_color_bus( int which_bus, uint8_t *output, bool *was_driven ) {
      mau_color_switchbox_.get_output_bus( which_bus, output, was_driven);
    }
    bool synth2port_fabric_check(bool is_stats, int alu, bool is_home_row, bool is_bottom, uint32_t ram_columns) {
      return mau_sram_row_reg_.synth2port_fabric_check(is_stats,alu,is_home_row,is_bottom,ram_columns);
    }

 private:
    int                                                              row_index_;
    std::array<MauSram*,kSramColumns>                                srams_;
    HashAddressVhXbarWithReg                                         hash_address_;
    ActionOutputHvXbar                                               action_output_hv_xbar_;
    std::array< MatchDataInputVhXbarWithReg, kSearchBuses>           match_xbars_;
    std::array< MauGatewayTable, kGatewayTablesPerRow>               gateway_tables_;
    std::array< MauGatewayPayload, kGatewayPayloadsPerRow>           gateway_payloads_;
    MauStash                                                         stash_;
    std::array< BitVector<kMatchOutputBusWidth>, kMatchOutputBuses>  match_output_buses_;
    std::array< bool, kMatchOutputBuses>                             match_output_bus_in_use_;
    std::array< int, kMatchOutputBuses>                              match_output_bus_col_;
    std::array< BitVector<kTindOutputBusWidth>, kTindOutputBuses>    tind_output_buses_;
    std::array< bool, kTindOutputBuses>                              tind_output_bus_in_use_;
    std::array< int, kTindOutputBuses>                               tind_output_bus_col_;
    std::array< uint32_t, kTindOutputBuses>                          tcam_match_addrs_;
    MauSramRow                                                      *row_above_;
    MauSramRow                                                      *row_below_;
    MauLogicalRow                                                   *logrow_left_;
    MauLogicalRow                                                   *logrow_right_;
    MauSramRowReg                                                    mau_sram_row_reg_;
    MauColorSwitchbox                                                mau_color_switchbox_;
  };
}
#endif // _SHARED_MAU_SRAM_ROW_
