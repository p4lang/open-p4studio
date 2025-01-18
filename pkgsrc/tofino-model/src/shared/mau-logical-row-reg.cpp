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

#include <mau.h>
#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-logical-row-reg.h>
#include <mau-logical-row.h>
#include <register_adapters.h>



namespace MODEL_CHIP_NAMESPACE {
  MauLogicalRowReg::MauLogicalRowReg(RmtObjectManager *om,
                                     int pipeIndex, int mauIndex, int logrowIndex,
                                     Mau *mau, MauLogicalRow *mauLogicalRow,
                                     int physrowIndex, int physrowWhich) 
      : MauObject(om, pipeIndex, mauIndex, kType, logrowIndex, mau),
        mau_logical_row_(mauLogicalRow), logrowIndex_(logrowIndex),
        physrowIndex_(physrowIndex), physrowWhich_(physrowWhich), addr_mux_vals_(0xFFFF),
        selector_action_adr_fallback_homerow_(default_adapter(selector_action_adr_fallback_homerow_,chip_index(), pipeIndex, mauIndex, physrowIndex, 0)),
        selector_action_adr_fallback_oflo_(default_adapter(selector_action_adr_fallback_oflo_,chip_index(), pipeIndex, mauIndex, physrowIndex, 1)),
        adr_dist_oflo_xbar_(default_adapter(adr_dist_oflo_xbar_,chip_index(), pipeIndex, mauIndex, physrowIndex, physrowWhich)),
        adr_dist_oflo2_xbar_(default_adapter(adr_dist_oflo2_xbar_,chip_index(), pipeIndex, mauIndex, physrowIndex, physrowWhich)),
        ram_address_mux_ctl_array_(default_adapter(ram_address_mux_ctl_array_,
            chip_index(), pipeIndex, mauIndex, physrowIndex,
            [this](uint32_t i, uint32_t j){this->addr_mux_change_callback(i,j);}))
  {
    selector_action_adr_fallback_homerow_.reset();
    selector_action_adr_fallback_oflo_.reset();
    adr_dist_oflo_xbar_.reset();
    adr_dist_oflo2_xbar_.reset();
    ram_address_mux_ctl_array_.reset();
    addr_mux_vals_ = 0;
  }
  MauLogicalRowReg::~MauLogicalRowReg() {  }



  // On change go through all SRAMS and stash what addr_mux vals they use
  void MauLogicalRowReg::addr_mux_change_callback(uint32_t lr, uint32_t logcol) {
    if (addr_mux_vals_ == 0xFFFF) return;
    RMT_ASSERT((lr == 0) || (lr == 1));
    RMT_ASSERT(logcol < kLogicalColumns);
    if (physrowWhich_ != static_cast<int>(lr)) return;
    uint16_t addr_mux_vals = 0;
    for (int c = 0; c < kLogicalColumns; c++) {
      int addr_mux = ram_address_mux_ctl_array_.ram_unitram_adr_mux_select(lr,c);
      RMT_ASSERT((addr_mux >= 0) && (addr_mux <= 15));
      addr_mux_vals |= (1<<addr_mux);
    }
    addr_mux_vals_ = addr_mux_vals;
  }


  void MauLogicalRowReg::oflow_handle(bool ingress, uint32_t *addr, uint8_t addrtype,
                                      int lc, int pri) {
    MauAddrDist *addr_dist = mau_logical_row_->mau_addr_dist();
    RMT_ASSERT(addr_dist != NULL);
    if (addr != NULL) *addr = 0u;
    if ((adr_dist_oflo_xbar_.adr_dist_oflo_adr_xbar_enable() & 0x1) == 0) return;
    
    uint8_t idx = adr_dist_oflo_xbar_.adr_dist_oflo_adr_xbar_source_index();
    RMT_ASSERT(idx < MauDefs::kLogicalRowsPerMau/2);
    uint8_t sel = adr_dist_oflo_xbar_.adr_dist_oflo_adr_xbar_source_sel();
    RMT_ASSERT(sel < 4);
    bool this_row_in_top_half = Mau::top_sram(physrowIndex_,0);
    // Row in source_index refers to a row in the same half of the SRAM array
    // as us so we add on 8 if we're in top_half of SRAM array.
    int src_logrow = idx + ((this_row_in_top_half) ?MauDefs::kLogicalRowFirstTop :0);
    switch (sel) {
      case 0:
        // Action - can only get from higher numbered rows than us
        if (logrowIndex_ < src_logrow) {
          if (addr != NULL) *addr = addr_dist->action_addr(src_logrow, ingress, addrtype);
        }
        break;
      case 1:
        // Stats - can only get from higher numbered rows than us
        if (logrowIndex_ < src_logrow) {
          if (addr != NULL) *addr = addr_dist->stats_addr(src_logrow, ingress, addrtype);
        }
        break;
      case 2:
        // Meter - can only get from higher numbered rows than us
        if (logrowIndex_ < src_logrow) {
          if (addr != NULL) *addr = addr_dist->meter_addr(src_logrow, ingress, addrtype);
        }
        break;
      case 3:
        // Overflow
        if (!this_row_in_top_half) {
          if (addr != NULL) *addr = addr_dist->oflow_addr(ingress, addrtype);
        }
        break;
    }
  }
  void MauLogicalRowReg::oflow2_handle(bool ingress, uint32_t *addr, uint8_t addrtype,
                                       int lc, int pri) {
    MauAddrDist *addr_dist = mau_logical_row_->mau_addr_dist();
    RMT_ASSERT(addr_dist != NULL);
    if (addr != NULL) *addr = 0u;

    bool up = (adr_dist_oflo2_xbar_.adr_dist_oflo2_adr_xbar_overflow2_up_enable() == 1);
    bool dn = (adr_dist_oflo2_xbar_.adr_dist_oflo2_adr_xbar_overflow2_down_enable() == 1);
    // We don't currently insist on one being set but complain if both are
    if (up && dn) {
      RMT_LOG(RmtDebug::warn(),
              "Oflow2_up and oflow2_down BOTH selected on log row %d "
              "so using oflow2_up\n", logrowIndex_);
      dn = false;
    }
    if (up) {
      if (addr != NULL) *addr = addr_dist->oflow2_up_addr(ingress, addrtype);
    } else if (dn) {
      if (addr != NULL) *addr = addr_dist->oflow2_down_addr(ingress, addrtype);
    }
  }


  uint32_t MauLogicalRowReg::oflow_addr(bool ingress, uint8_t addrtype) {
    uint32_t addr = 0u;
    oflow_handle(ingress, &addr, addrtype, -1, -1);
    return addr;
  }

  uint32_t MauLogicalRowReg::oflow2_addr(bool ingress, uint8_t addrtype) {
    uint32_t addr = 0u;
    (void)oflow2_handle(ingress, &addr, addrtype, -1, -1);
    return addr;
  }


  int MauLogicalRowReg::get_oflow_alu(uint8_t *addrtype, uint8_t *alutype) {
    if (addrtype != NULL) *addrtype = AddrType::kNone;
    if (alutype != NULL)  *alutype  = MauDefs::kAluTypeInvalid;

    // If oflo_xbar not enabled then no oflo ALU
    if ((adr_dist_oflo_xbar_.adr_dist_oflo_adr_xbar_enable() & 1) == 0) return -1;

    uint8_t src_idx = adr_dist_oflo_xbar_.adr_dist_oflo_adr_xbar_source_index();
    RMT_ASSERT(src_idx < MauDefs::kLogicalRowsPerMau/2);
    uint8_t sel = adr_dist_oflo_xbar_.adr_dist_oflo_adr_xbar_source_sel();
    RMT_ASSERT(sel < 4);
    bool this_row_in_top_half = Mau::top_sram(physrowIndex_,0);
    // Row in source_index refers to a row in the same half of the SRAM array
    // as us so we add on 8 if we're in top_half of SRAM array.
    int  src_logrow = src_idx + ((this_row_in_top_half) ?MauDefs::kLogicalRowFirstTop :0);
    switch (sel) {
      case 1:
        // Stats - can only get from higher numbered rows than us
        RMT_ASSERT((logrowIndex_ < src_logrow) &&
                   (((MauDefs::kStatsAluLogicalRows >> src_logrow) & 1) == 1));
        if (addrtype != NULL) *addrtype = AddrType::kStats;
        if (alutype != NULL)  *alutype  = MauDefs::kAluTypeStats;
        return MauStatsAlu::get_stats_alu_regs_index(src_logrow);

      case 2:
        // Meter - can only get from higher numbered rows than us
        RMT_ASSERT((logrowIndex_ < src_logrow) &&
                   (((MauDefs::kMeterAluLogicalRows >> src_logrow) & 1) == 1));
        if (addrtype != NULL) *addrtype = AddrType::kMeter;
        if (alutype != NULL)  *alutype  = MauDefs::kAluTypeMeter;
        return MauMeterAlu::get_meter_alu_regs_index(src_logrow);

      case 3:
        // Overflow - need to decode deferred_oflo_ctl to infer ALU
        RMT_ASSERT(!this_row_in_top_half);
        uint8_t oflo_ctl = mau()->mau_addr_dist()->get_oflo_ctl();
        for (int i = 0; i < 4; i++) {
          if (((oflo_ctl >> i) & 1) == 1) {
            if (addrtype != NULL)
              *addrtype = ((i%2)==0) ?AddrType::kStats :AddrType::kMeter;
            if (alutype != NULL)
              *alutype  = ((i%2)==0) ?MauDefs::kAluTypeStats :MauDefs::kAluTypeMeter;
            return 2+(i/2); // Always ALU in top half
          }
        }
    }
    return -1;
  }


}
