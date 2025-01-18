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

#ifndef _SHARED_MAU_LOGICAL_TABLE_
#define _SHARED_MAU_LOGICAL_TABLE_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-object.h>
#include <mau-lookup-result.h>
#include <mau-result-bus.h>
#include <mau-addr-dist.h>
#include <mau-hash-distribution.h>
#include <mau-logical-table-reg.h>
#include <phv.h>
#include <eop.h>

namespace MODEL_CHIP_NAMESPACE {

  class MauExecuteState;
  class MauSelectorMatchCentral;
  class MauStatefulCounters;

  class MauLogicalTable : public MauObject {

 public:
    static bool kRelaxPairedLtcamErrors; // Defined in rmt-config.cpp
    static bool kRelaxHdrtimeMeterAddrColorCheck;
    static bool kRelaxHdrtimeMeterAddrNopCheck;

    static constexpr int  kType = RmtTypes::kRmtTypeMauLogicalTable;
    static constexpr bool kEvaluateAllDefault = RmtDefs::kEvaluateAllDefault;

    static constexpr int  kSramColumnsPerMau = MauDefs::kSramColumnsPerMau;
    static constexpr int  kLogicalTcamsPerMau = MauDefs::kLogicalTcamsPerMau;
    static constexpr int  kLogicalRows = MauDefs::kLogicalRowsPerMau;
    static constexpr int  kMatchOutputBusWidth = MauDefs::kMatchOutputBusWidth;
    static constexpr int  kTindOutputBusWidth = MauDefs::kTindOutputBusWidth;
    static constexpr int  kTableResultBusWidth = MauDefs::kTableResultBusWidth;
    static constexpr int  kTableResultBusLsbPadWidth = MauDefs::kTableResultBusLsbPadWidth;
    static constexpr int  kActionHVOutputBusWidth = MauDefs::kActionHVOutputBusWidth;
    static constexpr int  kTableResultMatchAddrPos = MauDefs::kTableResultMatchAddrPos;
    static constexpr int  kGatewayPayloadsPerRow = MauDefs::kGatewayPayloadsPerRow;
    static constexpr int  kSramRowsPerMau = MauDefs::kSramRowsPerMau;
    static constexpr int  kGatewayPayloads = kSramRowsPerMau * kGatewayPayloadsPerRow;

    static const int      kDescendingPriColIndexTab[kSramColumnsPerMau];
    static const uint32_t kSuppressColLookupOnHitMaskTab[kSramColumnsPerMau];

    MauLogicalTable(RmtObjectManager *om, int pipeIndex, int mauIndex, int tableIndex, Mau *mau);
    virtual ~MauLogicalTable();

    inline int table_index()  const { return table_index_; }

    inline void add_sram_column(int col_index)    { sram_columns_ |= (1<<col_index); }
    inline void remove_sram_column(int col_index) { sram_columns_ &= ~(1<<col_index); }
    inline bool active_sram_column(int col_index) { return ((sram_columns_ & (1<<col_index)) != 0); }

    inline uint32_t logical_tcams()            const { return logical_tcams_; }
    inline void add_logical_tcam(int ltcam_index)    { logical_tcams_ |= (1<<ltcam_index); }
    inline void remove_logical_tcam(int ltcam_index) { logical_tcams_ &= ~(1<<ltcam_index); }
    inline bool active_logical_tcam(int ltcam_index) { return ((logical_tcams_ & (1<<ltcam_index)) != 0); }

    void add_gateway_payload(MauDefs::BusTypeEnum bus,int payload_index) {
      int i = bus==MauDefs::kExactMatchBus ? 0 : 1;
      gateway_payloads_[i] |= (1<<payload_index);
    }
    void remove_gateway_payload(MauDefs::BusTypeEnum bus,int payload_index) {
      int i = bus==MauDefs::kExactMatchBus ? 0 : 1;
      gateway_payloads_[i] &= ~(1<<payload_index);
    }
    bool active_gateway_payload(MauDefs::BusTypeEnum bus,int payload_index) {
      int i = bus==MauDefs::kExactMatchBus ? 0 : 1;
      return ((gateway_payloads_[i] & (1<<payload_index)) != 0);
    }

    inline bool is_ingress() { return mau_deps_->is_ingress_lt(table_index_); }
    inline bool is_egress()  { return mau_deps_->is_egress_lt(table_index_); }
    inline bool is_active()  { return is_ingress() || is_egress(); }
    inline bool check_ingress_egress(bool ingress) {
      return ((ingress) ?is_ingress() :is_egress());
    }
    inline bool has_bitmask_ops() { return mau_instr_store_->has_bitmask_ops(table_index_); }


    inline uint16_t get_action_logical_rows() {
      return mau_logical_table_reg_.get_action_logical_rows();
    }
    inline bool get_action_overflow() {
      return mau_logical_table_reg_.get_action_overflow();
    }
    inline bool get_action_overflow2_up() {
      return mau_logical_table_reg_.get_action_overflow2_up();
    }
    inline bool get_action_overflow2_down() {
      return mau_logical_table_reg_.get_action_overflow2_down();
    }
    inline uint8_t get_action_overflow_buses() {
      return mau_logical_table_reg_.get_action_overflow_buses();
    }
    inline uint8_t get_tind_ram_data_size() {
      return mau_logical_table_reg_.get_tind_ram_data_size();
    }
    int tcam_match_addr_get_subword(uint32_t addr,int shft=0) {
      return (addr & 0x1F) >> shft;
    }
    void get_tind_offset_nbits(uint32_t matchAddr, int tind_bus, uint8_t *off, uint8_t *nbits);


    bool gateway_is_enabled() { return mau_logical_table_reg_.gateway_is_enabled(); }
    int gateway_table_row() { return mau_logical_table_reg_.gateway_table_row(); }
    int gateway_table_which() { return mau_logical_table_reg_.gateway_table_which(); }
    uint16_t gateway_get_next_table(bool hit,int hit_index) {
      return mau_logical_table_reg_.gateway_get_next_table(hit,hit_index);
    }
    bool gateway_get_inhibit(bool hit,int hit_index) {
      return mau_logical_table_reg_.gateway_get_inhibit(hit,hit_index);
    }
    bool gateway_has_attached_table() {
      return logical_tcams_ != 0 || sram_columns_ != 0;
    }

    inline MauAddrDist* mau_addr_dist() const { return mau_addr_dist_; }

    inline bool evaluate_all()  const { return evaluate_all_; }
    void set_evaluate_all(bool tf);

    bool lookup_match(Phv *match_phv, MauLookupResult *result, bool ingress,
                      bool sram_tcam=true);
    bool lookup_gateway(Phv *match_phv, MauLookupResult *result,bool ingress);
    bool lookup_exact_match(Phv *match_phv, MauLookupResult *result, bool ingress);
    void lookup_paired_ltcam(Phv *match_phv, MauLookupResult *result, bool ingress,
                             int ltc_index, int paired_ltc_index);
    bool lookup_ternary_match(Phv *match_phv, MauLookupResult *result, bool ingress);
    bool lookup_stash(Phv *match_phv, MauLookupResult *result, bool ingress);
    uint8_t  packet_color(Phv *match_phv, MauLookupResult *result);
    void output_immediate_data(Phv *match_phv, MauLookupResult *result, bool ingress);
    void distrib_action_data_address(Phv *match_phv, MauLookupResult *result, bool ingress);
    void distrib_idletime_address(Phv *match_phv, MauLookupResult *result, bool ingress);
    void distrib_stats_address_at_hdr(Phv *match_phv, MauLookupResult *result, bool ingress);
    void distrib_meter_address_at_hdr(Phv *match_phv, MauLookupResult *result, bool ingress);
    void distrib_stats_address_at_eop(const Eop &eop);
    void distrib_meter_address_at_eop(const Eop &eop);
    void distrib_pbus_address_early(MauExecuteState *state);
    void distrib_pbus_address(MauExecuteState *state);


    void action_hv_bus_output_imm_data_byte(int tab, int which_byte, uint8_t byte_val);
    void action_hv_bus_output_imm_data(int tab, uint32_t imm_data);

    void extract_addrs(MauLookupResult *result,
                       const BitVector<kTableResultBusWidth>& data,
                       uint16_t *next_table, uint32_t *imm_data,
                       uint32_t *act_data_addr, uint8_t *act_instr_addr,
                       uint32_t *stats_addr, uint32_t *meter_addr,
                       uint32_t *idletime_addr, uint32_t *selector_len=NULL);

    void reset_resources();


    void get_table_result_bus(BitVector<kTableResultBusWidth> *output) {
      // This is redundant, left in for DV convenience
    }

 private:

    int                                     table_index_;
    static_assert( kSramColumnsPerMau < 32, "can only handled 32 columns");
    uint32_t                                sram_columns_;
    static_assert( kLogicalTcamsPerMau < 32, "can only handled 32 tcams");
    uint32_t                                logical_tcams_;
    static_assert( kGatewayPayloads < 32, "can only handled 32 gateway payloads");
    uint32_t                                gateway_payloads_[2]{};
    bool                                    evaluate_all_;
    MauResultBus                           *mau_result_bus_;
    MauDependencies                        *mau_deps_;
    MauAddrDist                            *mau_addr_dist_;
    MauHashDistribution                    *mau_hash_dist_;
    MauInstrStore                          *mau_instr_store_;
    MauStatefulCounters                    *mau_stateful_counters_;
    MauLogicalTableReg                      mau_logical_table_reg_;
    Mau                                    *mau_;
  };
}
#endif // _SHARED_MAU_LOGICAL_TABLE_
