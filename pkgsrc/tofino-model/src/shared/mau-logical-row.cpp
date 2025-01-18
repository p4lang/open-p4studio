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
#include <mau-logical-row.h>


namespace MODEL_CHIP_NAMESPACE {

  MauLogicalRow::MauLogicalRow(RmtObjectManager *om,
                               int pipeIndex, int mauIndex, int logicalRowIndex,
                               Mau *mau, int physicalRowIndex, int physicalRowWhich)
      : MauObject(om, pipeIndex, mauIndex, kType, logicalRowIndex, mau),
        logical_row_index_(logicalRowIndex),
        physical_row_index_(physicalRowIndex),
        physical_row_which_(physicalRowWhich),
        srams_(), action_output_buses_(), action_output_bus_in_use_(),
        action_rd_addr_(0u), stats_rd_addr_(0u), meter_rd_addr_(0u),
        oflo_rd_addr_(0u), oflo2_rd_addr_(0u), selector_rd_addr_(0u),
        stats_wr_addr_(0u), meter_wr_addr_(0u), color_write_data_(0),
        color_write_data_was_set_(false),
        stats_logical_table_(0xFF), action_rd_data_(UINT64_C(0)),
        stats_rd_data_(UINT64_C(0)), stats_wr_data_(UINT64_C(0)),
        meter_rd_data_(UINT64_C(0)), meter_wr_data_(UINT64_C(0)),
        oflo_rd_data_(UINT64_C(0)), oflo2_rd_data_(UINT64_C(0)),
        mau_addr_dist_(mau->mau_addr_dist()), physical_row_(NULL),
        mau_stats_alu_(NULL), mau_meter_alu_(NULL), mau_selector_alu_(NULL),
        mau_logical_row_reg_(om, pipeIndex, mauIndex, logicalRowIndex, mau, this,
                             physicalRowIndex, physicalRowWhich) {

    if ((kStatsAluLogicalRows & (1u<<logicalRowIndex)) != 0u) {
      mau_stats_alu_ = new MauStatsAlu(om, pipeIndex, mauIndex,
                                       logicalRowIndex, mau, this,
                                       physicalRowIndex, physicalRowWhich);
      RMT_ASSERT(mau_stats_alu_ != NULL);
    }
    if ((kMeterAluLogicalRows & (1u<<logicalRowIndex)) != 0u) {
      mau_meter_alu_ = new MauMeterAlu(om, pipeIndex, mauIndex,
                                       logicalRowIndex, mau, this,
                                       physicalRowIndex, physicalRowWhich);
      RMT_ASSERT(mau_meter_alu_ != NULL);
    }
    if ((kSelectorAluLogicalRows & (1u<<logicalRowIndex)) != 0u) {
      mau_selector_alu_ = new MauSelectorAlu(om, pipeIndex, mauIndex,
                                             logicalRowIndex, mau, this,
                                             physicalRowIndex, physicalRowWhich);
      RMT_ASSERT(mau_selector_alu_ != NULL);
    }

    reset_resources();
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCreate),
            "MauLogicalRow::create\n");
  }
  MauLogicalRow::~MauLogicalRow() {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDelete),
            "MauLogicalRow::delete\n");

    if (mau_stats_alu_ != NULL) delete mau_stats_alu_;
    mau_stats_alu_ = NULL;
    if (mau_meter_alu_ != NULL) delete mau_meter_alu_;
    mau_meter_alu_ = NULL;
    if (mau_selector_alu_ != NULL) delete mau_selector_alu_;
    mau_selector_alu_ = NULL;

    for (int i = 0; i < kLogicalColumns; i++) srams_[i] = NULL;
    physical_row_ = NULL;
  }



  // SELECTOR stuff
  // Per-row selector ALU output - raw 7bit value
  void MauLogicalRow::selector_addr_raw(uint32_t *addr) {
    // Just an alias for selector_rd_addr()
    // Gets value set by call to set_selector_rd_addr()
    selector_rd_addr(addr);
  }
  void MauLogicalRow::selector_oflo_addr_raw(uint32_t *addr) {
    // Call Selector RamFabric muxes to get selector_rd_addr() from another row
    RMT_ASSERT((physical_row_ != NULL) && (physical_row_->row_registers() != NULL));
    if (physical_row_which_ == 0)
      physical_row_->row_registers()->l_oflo_adr(addr);
    else
      physical_row_->row_registers()->r_oflo_adr(addr);
  }
  void MauLogicalRow::selector_oflo2_addr_raw(uint32_t *addr) {
    // Call Selector RamFabric muxes to get selector_rd_addr() from another row
    RMT_ASSERT((physical_row_ != NULL) && (physical_row_->row_registers() != NULL));
    if (physical_row_which_ == 0)
      physical_row_->row_registers()->l_oflo2_adr(addr);
    else
      physical_row_->row_registers()->r_oflo2_adr(addr);
  }

  // Per-row selector shift (used by selector_X_index|addr funcs)
  // Used to shift the SelectorALU selector_index to a postion *above*
  // the action address huffman bits - so value 0 is invalid as there
  // is always at least 1 Huffman bit
  int MauLogicalRow::selector_addr_shift() {
    RMT_ASSERT((physical_row_ != NULL) && (physical_row_->row_registers() != NULL));
    if (physical_row_which_ == 0)
      return physical_row_->row_registers()->selector_adr_shift_l();
    else
      return physical_row_->row_registers()->selector_adr_shift_r();
  }
  int MauLogicalRow::selector_oflo_addr_shift() {
    RMT_ASSERT((physical_row_ != NULL) && (physical_row_->row_registers() != NULL));
    if (physical_row_which_ == 0)
      return physical_row_->row_registers()->selector_oflo_adr_shift_l();
    else
      return physical_row_->row_registers()->selector_oflo_adr_shift_r();
  }
  int MauLogicalRow::selector_oflo2_addr_shift() {
    RMT_ASSERT((physical_row_ != NULL) && (physical_row_->row_registers() != NULL));
    if (physical_row_which_ == 0)
      return physical_row_->row_registers()->selector_oflo2_adr_shift_l();
    else
      return physical_row_->row_registers()->selector_oflo2_adr_shift_r();
  }

  // Per-row selector ALU index - shifted selector ALU output
  void MauLogicalRow::selector_addr_index(uint32_t *addr) {
    // Get fresh addr here as we need to shift it before OR
    uint32_t sel_addr = 0u;
    selector_addr_raw(&sel_addr);
    if (sel_addr == MauDefs::kSelectorAluInvalOutput) {
      // Signal from SelectorALU to use fallback addr
      if (physical_row_which_ == 0) {
        RMT_LOG(RmtDebug::error(),
                "MauLogicalRow::selector_addr_index() "
                " HomeRow fallback addr NOT available on LHS)\n");
        THROW_ERROR(-2);
      }
      uint32_t fb_mask = Address::kActionAddrAddrMask;
      uint32_t fb_addr = mau_logical_row_reg_.get_homerow_fallback_action_addr();
      *addr = (*addr & ~fb_mask) | (fb_addr & fb_mask);
    } else {
      int shift = selector_addr_shift();
      if ((sel_addr != 0u) && ((shift < 1) || (shift > 5)) &&
          ((get_addr_mux_vals() & (1<<6)) != 0)) { // XXX: check sel_index in-use
        RMT_LOG(RmtDebug::error(kRelaxRowSelectorShiftCheck),
                "MauLogicalRow::selector_addr_index(shift=%d => INVALID)\n", shift);
        if (!kRelaxRowSelectorShiftCheck) { THROW_ERROR(-2); }
      }
      *addr |= (sel_addr << shift);
    }
  }
  void MauLogicalRow::selector_oflo_addr_index(uint32_t *addr) {
    uint32_t sel_oflo_addr = 0u;
    selector_oflo_addr_raw(&sel_oflo_addr);
    if (sel_oflo_addr == MauDefs::kSelectorAluInvalOutput) {
      // Signal from SelectorALU to use fallback addr
      uint32_t fb_mask = Address::kActionAddrAddrMask;
      uint32_t fb_addr = mau_logical_row_reg_.get_oflo_fallback_action_addr();
      *addr = (*addr & ~fb_mask) | (fb_addr & fb_mask);
    } else {
      int shift = selector_oflo_addr_shift();
      if ((sel_oflo_addr != 0u) && ((shift < 1) || (shift > 5)) &&
          ((get_addr_mux_vals() & (1<<7)) != 0)) { // XXX: check sel_index in-use
        RMT_LOG(RmtDebug::error(kRelaxRowSelectorShiftCheck),
                "MauLogicalRow::selector_oflo_addr_index(shift=%d => INVALID)\n", shift);
        if (!kRelaxRowSelectorShiftCheck) { THROW_ERROR(-2); }
      }
      *addr |= (sel_oflo_addr << shift);
    }
  }
  void MauLogicalRow::selector_oflo2_addr_index(uint32_t *addr) {
    uint32_t sel_oflo2_addr = 0u;
    selector_oflo2_addr_raw(&sel_oflo2_addr);
    int shift = selector_oflo2_addr_shift();
    if ((sel_oflo2_addr != 0u) && ((shift < 1) || (shift > 5))) {
      RMT_LOG(RmtDebug::error(kRelaxRowSelectorShiftCheck),
              "MauLogicalRow::selector_oflo2_addr_index(shift=%d => INVALID)\n", shift);
      if (!kRelaxRowSelectorShiftCheck) { THROW_ERROR(-2); }
    }
    *addr |= (sel_oflo2_addr << shift);
  }



  // READ addresses from AddressDistribution - for oflow/oflow2
  // addresses we call into logical row register code as we may
  // be configured to get oflo/oflo2 from other action/stats/meter
  // rows or from oflow/oflo2_up/oflo2_dn
  // NB. Purpose of read is to check ingress/egress addrtype validity
  // ALL addresses now read in advance in one fell swoop in fetch_addresses()
  // THESE FUNCS EXCLUSIVELY CALLED FROM MauSram code
  //
  void MauLogicalRow::action_addr(uint32_t *addr, bool ingress) {
    if (rmt_log_check(RmtDebug::kRmtDebugMauLogicalRowAddr)) {
      uint32_t sel_addr = 0u, sel_addr_raw = 0u, sel_addr_shft = 0u;
      uint32_t sel_oflo_addr = 0u, sel_oflo_addr_raw = 0u, sel_oflo_addr_shft = 0u;
      selector_addr_raw(&sel_addr_raw);
      sel_addr_shft = selector_addr_shift();
      selector_addr_index(&sel_addr);
      selector_oflo_addr_raw(&sel_oflo_addr_raw);
      sel_oflo_addr_shft = selector_oflo_addr_shift();
      selector_oflo_addr_index(&sel_oflo_addr);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalRowAddr),
              "MauLogicalRow::action_addr(actionA=0x%08x "
              "SelA=0x%02x[0x%02x,%d] SelOfloA=0x%02x[0x%02x,%d])\n",
              action_rd_addr_, sel_addr, sel_addr_raw, sel_addr_shft,
              sel_oflo_addr, sel_oflo_addr_raw, sel_oflo_addr_shft);
    }
    if (!Address::action_addr_enabled(mau_addr_dist()->action_addr(logical_row_index(), ingress)))
      *addr = Address::invalid();
    else
      *addr |= action_rd_addr_;
  }
  void MauLogicalRow::stats_addr(uint32_t *addr, bool ingress) {
    if (!Address::stats_addr_op_enabled(mau_addr_dist()->stats_addr(logical_row_index(), ingress)))
      *addr = Address::invalid();
    else
      *addr |= stats_rd_addr_;
  }
  void MauLogicalRow::meter_addr(uint32_t *addr, bool ingress) {
    uint32_t tmpaddr = 0u;
    if (is_jbay_or_later()) {
      // JBay StatefulCounters can distribute meter_addresses without LT - XXX
      tmpaddr = mau_addr_dist()->meter_addr_nocheck(logical_row_index());
    } else {
      tmpaddr = mau_addr_dist()->meter_addr(logical_row_index(), ingress);
    }
    if (!Address::meter_addr_op_enabled(tmpaddr))
      *addr = Address::invalid();
    else
      *addr |= meter_rd_addr_;
  }
  void MauLogicalRow::oflo_addr(uint32_t *addr, bool ingress, uint8_t addrtype) {
    if (rmt_log_check(RmtDebug::kRmtDebugMauLogicalRowAddr)) {
      uint32_t sel_addr = 0u, sel_addr_raw = 0u, sel_addr_shft = 0u;
      uint32_t sel_oflo_addr = 0u, sel_oflo_addr_raw = 0u, sel_oflo_addr_shft = 0u;
      selector_addr_raw(&sel_addr_raw);
      sel_addr_shft = selector_addr_shift();
      if (addrtype == AddrType::kAction) selector_addr_index(&sel_addr);
      selector_oflo_addr_raw(&sel_oflo_addr_raw);
      sel_oflo_addr_shft = selector_oflo_addr_shift();
      if (addrtype == AddrType::kAction) selector_oflo_addr_index(&sel_oflo_addr);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalRowAddr),
              "MauLogicalRow::oflo_addr(ofloA=0x%08x "
              "SelA=0x%02x[0x%02x,%d] SelOfloA=0x%02x[0x%02x,%d])\n",
              oflo_rd_addr_, sel_addr, sel_addr_raw, sel_addr_shft,
              sel_oflo_addr, sel_oflo_addr_raw, sel_oflo_addr_shft);
    }
    uint32_t a = mau_logical_row_reg_.oflow_addr(ingress, addrtype);
    bool en = false;
    switch (addrtype) {
      case AddrType::kAction: en = Address::action_addr_enabled(a);   break;
      case AddrType::kStats:  en = Address::stats_addr_op_enabled(a); break;
      case AddrType::kMeter:  en = Address::meter_addr_op_enabled(a); break;
    }
    if (en)
      *addr |= oflo_rd_addr_;
    else
      *addr = Address::invalid();
  }
  void MauLogicalRow::oflo2_addr(uint32_t *addr, bool ingress, uint8_t addrtype) {
    uint32_t a = mau_logical_row_reg_.oflow2_addr(ingress, addrtype);
    bool en = false;
    switch (addrtype) {
      case AddrType::kAction: en = Address::action_addr_enabled(a);   break;
      case AddrType::kStats:  en = Address::stats_addr_op_enabled(a); break;
      case AddrType::kMeter:  en = Address::meter_addr_op_enabled(a); break;
    }
    if (en)
      *addr |= oflo2_rd_addr_;
    else
      *addr = Address::invalid();
  }
  // Selector addresses are action addresses from AddressDistribution ORed
  // with selector indices from SelectorALUs on this row or other rows
  // (apart from oflo_selector_addr whichs is the oflow address ORed with
  // selector oflo)
  //
  void MauLogicalRow::selector_addr(uint32_t *addr, bool ingress) {
    action_addr(addr, ingress);
    selector_addr_index(addr); // ORs in shifted selector ALU output
  }
  void MauLogicalRow::selector_oflo_addr(uint32_t *addr, bool ingress) {
    action_addr(addr, ingress);
    selector_oflo_addr_index(addr); // ORs in shifted oflow selector ALU output
  }
  void MauLogicalRow::oflo_selector_addr(uint32_t *addr, bool ingress, uint8_t addrtype) {
    oflo_addr(addr, ingress, addrtype);
    selector_oflo_addr_index(addr); // ORs in shifted oflow selector ALU output
  }
  // THIS ONE NOT USED - RamAddressMux=8 now selects func above
  void MauLogicalRow::selector_oflo2_addr(uint32_t *addr, bool ingress) {
    action_addr(addr, ingress);
    selector_oflo2_addr_index(addr); // ORs in shifted oflow2 selector ALU output
  }




  // WRITE addresses - we need to call into physical row code as
  // write addresses might come from rows above/below etc depending
  // on the config of various WADR muxes
  // THESE FUNCS EXCLUSIVELY CALLED FROM MauSram code
  //
  void MauLogicalRow::stats_waddr(uint32_t *waddr) {
    RMT_ASSERT((physical_row_ != NULL) && (physical_row_->row_registers() != NULL));
    if (physical_row_which_ == 0)
      physical_row_->row_registers()->l_stats_wadr(waddr);
    else
      physical_row_->row_registers()->r_stats_wadr(waddr);
  }
  void MauLogicalRow::meter_waddr(uint32_t *waddr) {
    RMT_ASSERT((physical_row_ != NULL) && (physical_row_->row_registers() != NULL));
    if (physical_row_which_ == 0)
      physical_row_->row_registers()->l_meter_wadr(waddr);
    else
      physical_row_->row_registers()->r_meter_wadr(waddr);
  }
  void MauLogicalRow::oflow_waddr(uint32_t *waddr) {
    RMT_ASSERT((physical_row_ != NULL) && (physical_row_->row_registers() != NULL));
    if (physical_row_which_ == 0)
      physical_row_->row_registers()->l_oflo_wadr(waddr);
    else
      physical_row_->row_registers()->r_oflo_wadr(waddr);
  }
  void MauLogicalRow::oflow2_waddr(uint32_t *waddr) {
    RMT_ASSERT((physical_row_ != NULL) && (physical_row_->row_registers() != NULL));
    if (physical_row_which_ == 0)
      physical_row_->row_registers()->l_oflo2_wadr(waddr);
    else
      physical_row_->row_registers()->r_oflo2_wadr(waddr);
  }
  void MauLogicalRow::oflow_color_write_data(uint8_t *color) {
    RMT_ASSERT((physical_row_ != NULL) && (physical_row_->row_registers() != NULL));
    if (physical_row_which_ == 0)
      *color = 0; // there is no color bus on the left
    else
      physical_row_->row_registers()->r_oflo_color_write(color);
  }

  // ADDRESS/DATA accessor funcs called from physical row RamFabric code
  // based on configuration of RamFabric muxes
  // THESE FUNCS EXCLUSIVELY CALLED FROM MauSramRow code
  //
  void MauLogicalRow::addr_multi_write_check(const char *bus_name,
                                             uint32_t *addrA, uint32_t addrB) {
    RMT_ASSERT(addrA != NULL);
    if (Address::isInvalid(addrB)) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalRowAddr),
              "MauLogicalRow: %s address not driven\n", bus_name);
    } else if ((*addrA != 0u) && (addrB != 0u) && (*addrA != addrB)) {
      // Complain if we're ORing multiple vals into addr - probably an error
      RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugMauLogicalRowAddr),
              "MauLogicalRow: %s multiple drivers == 0x%08x|0x%08x\n",
              bus_name, *addrA, addrB);
      if (!kRelaxAddrMultiWriteCheck) { THROW_ERROR(-2); }
    }
  }


  // SETTER for selector_rd_addr_ called by SelectorALU
  void MauLogicalRow::set_selector_rd_addr(uint32_t addr) {
    selector_rd_addr_ = addr;
  }
  // THESE NOT USED NOW
  void MauLogicalRow::set_action_rd_addr(uint32_t *addr) { RMT_ASSERT(0); }
  void MauLogicalRow::set_stats_rd_addr(uint32_t *addr)  { RMT_ASSERT(0); }


  // ADDRESS accessor funcs first
  // Here we ignore INVALID addresses - still complain though
  //
  void MauLogicalRow::action_rd_addr(uint32_t *addr) {
    addr_multi_write_check("action_rd_addr", addr, action_rd_addr_);
    // Don't OR in invalid
    if (!Address::action_addr_enabled(action_rd_addr_)) return;
    *addr |= action_rd_addr_;
  }
  void MauLogicalRow::stats_rd_addr(uint32_t *addr) {
    addr_multi_write_check("stats_rd_addr", addr, stats_rd_addr_);
    // Don't OR in invalid
    if (!Address::stats_addr_op_enabled(stats_rd_addr_)) return;
    *addr |= stats_rd_addr_;
  }
  void MauLogicalRow::meter_rd_addr(uint32_t *addr) {
    addr_multi_write_check("meter_rd_addr", addr, meter_rd_addr_);
    // Don't OR in invalid
    if (!Address::meter_addr_op_enabled(meter_rd_addr_)) return;
    *addr |= meter_rd_addr_;
  }
  void MauLogicalRow::oflo_rd_addr(uint32_t *addr, uint8_t addrtype) {
    addr_multi_write_check("oflo_rd_addr", addr, oflo_rd_addr_);
    // Don't OR in non-enabled address
    uint32_t a = oflo_rd_addr_;
    bool en = false;
    switch (addrtype) {
      case AddrType::kAction: en = Address::action_addr_enabled(a);   break;
      case AddrType::kStats:  en = Address::stats_addr_op_enabled(a); break;
      case AddrType::kMeter:  en = Address::meter_addr_op_enabled(a); break;
    }
    if (!en) return;
    *addr |= oflo_rd_addr_;
  }
  void MauLogicalRow::oflo2_rd_addr(uint32_t *addr, uint8_t addrtype) {
    addr_multi_write_check("oflo2_rd_addr", addr, oflo2_rd_addr_);
    // Don't OR in non-enabled address
    uint32_t a = oflo2_rd_addr_;
    bool en = false;
    switch (addrtype) {
      case AddrType::kAction: en = Address::action_addr_enabled(a);   break;
      case AddrType::kStats:  en = Address::stats_addr_op_enabled(a); break;
      case AddrType::kMeter:  en = Address::meter_addr_op_enabled(a); break;
    }
    if (!en) return;
    *addr |= oflo2_rd_addr_;
  }
  void MauLogicalRow::selector_rd_addr(uint32_t *addr) {
    addr_multi_write_check("selector_rd_addr", addr, selector_rd_addr_);
    // Selector addr just an offset - no enabled flag
    *addr |= selector_rd_addr_;
  }
  // Action address used to select action subwords for ActionHV bus.
  // Selector indices *may* be ORed in depending on ram_address_mux
  // config of SRAMs on row
  void MauLogicalRow::action_sel_rd_addr(uint32_t *addr) {
    if (!Address::action_addr_enabled(action_rd_addr_)) return;
    uint16_t addr_mux_vals = get_addr_mux_vals();
    uint32_t act_sel_addr = action_rd_addr_; // Always get action_rd_addr_
    // But only OR in selector indices if mux 6 or 7 used on row
    if ((addr_mux_vals & (1<<6)) != 0) selector_addr_index(&act_sel_addr);
    if ((addr_mux_vals & (1<<7)) != 0) selector_oflo_addr_index(&act_sel_addr);
    addr_multi_write_check("action_sel_rd_addr", addr, act_sel_addr);
    *addr |= act_sel_addr;
  }


  // And WRITE ADDRESS accessors - ONLY stats and meter write addresses.
  // And for any logical row should be same as read addresses (although
  // ALUs may overwrite, typically to be AddrInval)
  //
  void MauLogicalRow::stats_wr_addr(uint32_t *addr) {
    addr_multi_write_check("stats_wr_addr", addr, stats_wr_addr_);
    *addr |= stats_wr_addr_;
  }
  void MauLogicalRow::meter_wr_addr(uint32_t *addr) {
    addr_multi_write_check("meter_wr_addr", addr, meter_wr_addr_);
    *addr |= meter_wr_addr_;
  }
  // SETTERs
  void MauLogicalRow::set_stats_wr_addr(uint32_t *addr) {
    stats_wr_addr_ = *addr;
  }
  void MauLogicalRow::set_meter_wr_addr(uint32_t *addr) {
    meter_wr_addr_ = *addr;
  }
  // Oflo/Oflo2 call back into RamFabric to fetch wrAddr from correct row
  void MauLogicalRow::oflo_wr_addr(uint32_t *addr) {
    RMT_ASSERT((physical_row_ != NULL) && (physical_row_->row_registers() != NULL));
    if (physical_row_which_ == 0)
      physical_row_->row_registers()->l_oflo_wadr(addr);
    else
      physical_row_->row_registers()->r_oflo_wadr(addr);
  }
  void MauLogicalRow::oflo2_wr_addr(uint32_t *addr) {
    RMT_ASSERT((physical_row_ != NULL) && (physical_row_->row_registers() != NULL));
    if (physical_row_which_ == 0)
      physical_row_->row_registers()->l_oflo2_wadr(addr);
    else
      physical_row_->row_registers()->r_oflo2_wadr(addr);
  }

  void MauLogicalRow::color_write_data(uint8_t* data) {
    *data |= color_write_data_;
  }
  bool MauLogicalRow::color_write_data_was_set() {
    return color_write_data_was_set_;
  }
  void MauLogicalRow::set_color_write_data(uint8_t* data) {
    color_write_data_ = *data;
    color_write_data_was_set_ = true;
  }



  void MauLogicalRow::data_multi_write_check(const char *bus_name,
                                             BitVector<kDataBusWidth> *dataA,
                                             const BitVector<kDataBusWidth>& dataB) {
    // Complain if we're ORing multiple vals into data - probably an error
    RMT_ASSERT(dataA != NULL);
    if ((!dataA->is_zero()) && (!dataB.is_zero()) && (!dataA->equals(dataB))) {
      RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugMauLogicalRowData),
              "MauLogicalRow: %s == 0x%s|0x%s\n",
              bus_name, dataA->to_string().c_str(), dataB.to_string().c_str());
      if (!kRelaxDataMultiWriteCheck) { THROW_ERROR(-2); }
    }
  }

  // Now DATA accessors - also funcs to set DATA buses - GETTERs first
  void MauLogicalRow::action_rd_data(BitVector<kDataBusWidth> *data) {
    data_multi_write_check("action_rd_data", data, action_rd_data_);
    data->or_with(action_rd_data_);
  }
  void MauLogicalRow::stats_rd_data(BitVector<kDataBusWidth> *data) {
    data_multi_write_check("stats_rd_data", data, stats_rd_data_);
    data->or_with(stats_rd_data_);
  }
  void MauLogicalRow::stats_alu_rd_data(BitVector<kDataBusWidth> *data) {
    // get the data possibly from overflow row
    uint32_t dummy_addr = 0u; // this will contain the addr on the row the data come from
    if (physical_row_which_ == 0)
      physical_row_->row_registers()->l_stats_alu(data, &dummy_addr);
    else
      physical_row_->row_registers()->r_stats_alu(data, &dummy_addr);
  }
  // get_selector_alu_input_data() is used by DV to get the input for selectors instead of
  //   stats_alu_rd_data (which is used for all other alus) because selectors have strange
  //   forwarding behaviour so the input must be got from the selector alu itself
  void MauLogicalRow::get_selector_alu_input_data(BitVector<kDataBusWidth> *data) {
    if (mau_selector_alu_) {
      mau_selector_alu_->get_input_data(data);
    } else {
      data->fill_all_zeros();
    }
  }
  // Similarly Meter ALUs (at least LPF ones) have wacky forwarding behaviour
  void MauLogicalRow::get_meter_alu_input_data(BitVector<kDataBusWidth> *data) {
    if (mau_meter_alu_) {
      mau_meter_alu_->get_input_data(data);
    } else {
      data->fill_all_zeros();
    }
  }
  void MauLogicalRow::stats_wr_data(BitVector<kDataBusWidth> *data) {
    data_multi_write_check("stats_wr_data", data, stats_wr_data_);
    data->or_with(stats_wr_data_);
  }
  void MauLogicalRow::meter_rd_data(BitVector<kDataBusWidth> *data) {
    data_multi_write_check("meter_rd_data", data, meter_rd_data_);
    data->or_with(meter_rd_data_);
  }
  void MauLogicalRow::meter_wr_data(BitVector<kDataBusWidth> *data) {
    data_multi_write_check("meter_wr_data", data, meter_wr_data_);
    data->or_with(meter_wr_data_);
  }
  void MauLogicalRow::oflo_rd_data(BitVector<kDataBusWidth> *data) {
    data_multi_write_check("oflo_rd_data", data, oflo_rd_data_);
    data->or_with(oflo_rd_data_);
  }
  void MauLogicalRow::oflo2_rd_data(BitVector<kDataBusWidth> *data) {
    data_multi_write_check("oflo2_rd_data", data, oflo2_rd_data_);
    data->or_with(oflo2_rd_data_);
  }
  // Oflo/Oflo2 call back into RamFabric to fetch wrData from correct row
  void MauLogicalRow::oflo_wr_data(BitVector<kDataBusWidth> *data) {
    RMT_ASSERT((physical_row_ != NULL) && (physical_row_->row_registers() != NULL));
    uint32_t addr = 0u; // Not used
    if (physical_row_which_ == 0)
      physical_row_->row_registers()->l_oflo_wr(data, &addr, 0);
    else
      physical_row_->row_registers()->r_oflo_wr(data, &addr, 0);
  }
  void MauLogicalRow::oflo2_wr_data(BitVector<kDataBusWidth> *data) {
    RMT_ASSERT((physical_row_ != NULL) && (physical_row_->row_registers() != NULL));
    uint32_t addr = 0u; // Not used
    if (physical_row_which_ == 0)
      physical_row_->row_registers()->l_oflo2_wr(data, &addr, 0);
    else
      physical_row_->row_registers()->r_oflo2_wr(data, &addr, 0);
  }

  // Now SETTERs - mixture - set_X_rd funcs called from MauSram code
  // set_X_wr funcs called from ALUs within MauLogicalRow
  //
  void MauLogicalRow::set_action_rd_data_nocheck(BitVector<kDataBusWidth> *data) {
    action_rd_data_.or_with(*data);
  }
  void MauLogicalRow::set_action_rd_data(BitVector<kDataBusWidth> *data) {
    data_multi_write_check("set_action_rd_data", data, action_rd_data_);
    action_rd_data_.or_with(*data);
  }
  void MauLogicalRow::set_stats_rd_data(BitVector<kDataBusWidth> *data) {
    data_multi_write_check("set_stats_rd_data", data, stats_rd_data_);
    stats_rd_data_.or_with(*data);
  }
  void MauLogicalRow::set_stats_wr_data(BitVector<kDataBusWidth> *data) {
    data_multi_write_check("set_stats_wr_data", data, stats_wr_data_);
    stats_wr_data_.or_with(*data);
  }
  void MauLogicalRow::set_meter_rd_data(BitVector<kDataBusWidth> *data) {
    data_multi_write_check("set_meter_rd_data", data, meter_rd_data_);
    meter_rd_data_.or_with(*data);
  }
  void MauLogicalRow::set_meter_wr_data(BitVector<kDataBusWidth> *data) {
    data_multi_write_check("set_meter_wr_data", data, meter_wr_data_);
    meter_wr_data_.or_with(*data);
  }
  void MauLogicalRow::set_oflo_rd_data(BitVector<kDataBusWidth> *data) {
    data_multi_write_check("set_oflo_rd_data", data, oflo_rd_data_);
    oflo_rd_data_.or_with(*data);
  }
  void MauLogicalRow::set_oflo2_rd_data(BitVector<kDataBusWidth> *data) {
    data_multi_write_check("set_oflo2_rd_data", data, oflo2_rd_data_);
    oflo2_rd_data_.or_with(*data);
  }

  // Only used when we catch_up_sweeps
  void MauLogicalRow::clear_stats_rd_data() {
    stats_rd_data_.fill_all_zeros();
  }
  void MauLogicalRow::clear_stats_wr_data() {
    stats_wr_data_.fill_all_zeros();
  }


  bool MauLogicalRow::run_selector_alu_with_state(MauExecuteState *state) {
    if (mau_selector_alu_ != NULL)
      mau_selector_alu_->run_alu_with_state(state);
    return true;
  }
  bool MauLogicalRow::run_alus_with_state(MauExecuteState *state) {
    // No ALUs on LHS since regs_12544_mau_dev
    // Alternating ALUs on RHS since regs_13957_mau_dev
    bool stats_ok = (mau_stats_alu_ != NULL) ?mau_stats_alu_->run_alu_with_state(state) :true;
    bool meter_ok = (mau_meter_alu_ != NULL) ?mau_meter_alu_->run_alu_with_state(state) :true;
    return stats_ok && meter_ok;
  }
  bool MauLogicalRow::run_cmp_alus_with_state(MauExecuteState *state) {
    return (mau_meter_alu_ != NULL) ?mau_meter_alu_->run_cmp_alu_with_state(state) :true;
  }

  // Fetch all addresses up front in one fell swoop
  void MauLogicalRow::fetch_addresses() {
    // Using AddrType::kNone bypasses ingress/egress validity checks in MauAddrDist
    action_rd_addr_ = mau_addr_dist()->action_addr(logical_row_index(), true,
                                                   AddrType::kNone);
    stats_rd_addr_ = mau_addr_dist()->stats_addr(logical_row_index(), true,
                                                 AddrType::kNone);
    if (is_jbay_or_later()) {
      // JBay StatefulCounters can distribute meter_addresses without LT - XXX
      meter_rd_addr_ = mau_addr_dist()->meter_addr_nocheck(logical_row_index());
    } else {
      meter_rd_addr_ = mau_addr_dist()->meter_addr(logical_row_index(), true,
                                                   AddrType::kNone);
    }
    oflo_rd_addr_ = mau_logical_row_reg_.oflow_addr(true, AddrType::kNone);
    oflo2_rd_addr_ = mau_logical_row_reg_.oflow2_addr(true, AddrType::kNone);

    // Only output info if have an enabled addr
    // (don't know what type of addr on oflo so just check if non-zero)
    if ((Address::action_addr_enabled(action_rd_addr_)) ||
        (Address::stats_addr_op_enabled(stats_rd_addr_)) ||
        (Address::meter_addr_op_enabled(meter_rd_addr_)) || (oflo_rd_addr_ != 0u))
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalRowAddr),
              "MauLogicalRow::fetch_addresses(A=0x%08x S=0x%08x M=0x%08x O=0x%08x)\n",
              action_rd_addr_, stats_rd_addr_, meter_rd_addr_, oflo_rd_addr_);
  }

  // Cleanup all buses
  void MauLogicalRow::reset_resources() {
    for (int i = 0; i < kActionOutputBuses; i++) action_output_bus_in_use_[i] = false;
    action_rd_data_.fill_all_zeros();
    stats_rd_data_.fill_all_zeros(); stats_wr_data_.fill_all_zeros();
    meter_rd_data_.fill_all_zeros(); meter_wr_data_.fill_all_zeros();
    oflo_rd_data_.fill_all_zeros(); oflo2_rd_data_.fill_all_zeros();
    action_rd_addr_ = Address::invalid();
    stats_rd_addr_ = Address::invalid();
    meter_rd_addr_ = Address::invalid();
    oflo_rd_addr_ = Address::invalid();
    oflo2_rd_addr_ = Address::invalid();
    selector_rd_addr_ = Address::invalid();
    stats_wr_addr_ = Address::invalid();
    meter_wr_addr_ = Address::invalid();
    stats_logical_table_ = 0xFF;
    color_write_data_was_set_ = false;

    if (mau_stats_alu_) mau_stats_alu_->reset_resources();
    if (mau_meter_alu_) mau_meter_alu_->reset_resources();
    if (mau_selector_alu_) mau_selector_alu_->reset_resources();
  }

}
