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
#include <mau-sram-reg.h>
#include <mau-sram.h>
#include <register_adapters.h>



namespace MODEL_CHIP_NAMESPACE {


  MauSramReg::MauSramReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                         int rowIndex, int colIndex, int sramIndex, MauSram *mauSram)
      : MauObject(om, pipeIndex, mauIndex, kType, rowIndex, colIndex),
        mau_sram_(mauSram),
        match_mask_array_(default_adapter(match_mask_array_,chip_index(), pipeIndex, mauIndex, rowIndex, colIndex,
                          [this](uint32_t i){this->match_mask_write_callback(i);})),
        match_bytemask_array_(default_adapter(match_bytemask_array_,chip_index(), pipeIndex, mauIndex, rowIndex, colIndex,
                              [this](uint32_t i){this->match_bytemask_write_callback(i);})),
        match_nexttable_bitpos_(default_adapter(match_nexttable_bitpos_,chip_index(), pipeIndex, mauIndex, rowIndex, colIndex)),
        match_nibble_s0q1_enable_(default_adapter(match_nibble_s0q1_enable_,chip_index(), pipeIndex, mauIndex, rowIndex, colIndex,
                                  [this](){this->match_nibble_s0q1_enable_callback();})),
        match_nibble_s1q0_enable_(default_adapter(match_nibble_s1q0_enable_,chip_index(), pipeIndex, mauIndex, rowIndex, colIndex,
                                  [this](){this->match_nibble_s1q0_enable_callback();})),
        ram_address_mux_ctl_(default_adapter(ram_address_mux_ctl_,chip_index(), pipeIndex, mauIndex,
                             rowIndex, (colIndex<6) ? 0 : 1 , colIndex % 6,
                             [this](){this->config_change_callback();})),
        unit_ram_ctl_(default_adapter(unit_ram_ctl_,chip_index(), pipeIndex, mauIndex, rowIndex, colIndex, nullptr,
                      [this](){this->config_change_callback();})),
        unitram_config_(default_adapter(unitram_config_,chip_index(), pipeIndex, mauIndex,
                        rowIndex, (colIndex<6) ? 0 : 1 , colIndex % 6,
                        [this](){this->config_change_callback();})),
        match_ram_vpn_(default_adapter(match_ram_vpn_,chip_index(), pipeIndex, mauIndex, rowIndex, colIndex,
                       [this](){this->match_ram_vpn_write_callback();})),
        row_action_nxtable_bus_drive_(default_adapter(row_action_nxtable_bus_drive_,chip_index(), pipeIndex, mauIndex, colIndex, rowIndex)),
        adr_dist_tind_adr_xbar_ctl_array_(nullptr),
        config_verify_(true), config_complain_(false),
        tind_buses_(0), unit_vpn_(0),
        match_mask_bv_(UINT64_C(0)) {

    unitram_config_.reset(); // Do this first so type is 0
    match_bytemask_array_.reset();
    match_mask_array_.reset();
    match_nexttable_bitpos_.reset();
    unit_ram_ctl_.reset();
    match_nibble_s0q1_enable_.reset();
    match_nibble_s1q0_enable_.reset();
    ram_address_mux_ctl_.reset();
    match_ram_vpn_.reset();
    row_action_nxtable_bus_drive_.reset();
    // Only LHS SRAMs have this next reg
    if (Mau::lhs_sram(rowIndex, colIndex)) {
      adr_dist_tind_adr_xbar_ctl_array_= default_adapter_new(adr_dist_tind_adr_xbar_ctl_array_,chip_index(), pipeIndex, mauIndex, rowIndex,
                                                             [this](uint32_t i){this->tind_xbar_write_callback();});
      adr_dist_tind_adr_xbar_ctl_array_->reset();
    }
  }
  MauSramReg::~MauSramReg() {
    if (adr_dist_tind_adr_xbar_ctl_array_ != NULL) {
      delete adr_dist_tind_adr_xbar_ctl_array_;
      adr_dist_tind_adr_xbar_ctl_array_ = NULL;
    }
  }


  void MauSramReg::match_mask_write_callback(uint32_t i) {
    // Update match_mask BitVector from register
    // NB. Since register update 190914 we INVERT these masks
    uint32_t reg_word = ~match_mask_array_.match_mask(i);
    match_mask_bv_.set32(i, reg_word);
    // Synthesize calls to bytemask callback so we recalc all bytemasks
    for (int i = 0; i < MauSram::kMaskEntries; i++) {
      match_bytemask_write_callback(i);
    }
    // Reverify config
    config_verify_ = true;
  }
  void MauSramReg::match_bytemask_write_callback(uint32_t i) {
    // Get bytemask from register, transform to BV,
    // mask with match_mask BitVector, update MauSram
    // NB. Since register update 190914 we INVERT these masks
    uint16_t bytes = ~match_bytemask_array_.mask_bytes_0_to_13(i);
    uint8_t nibbles = ~match_bytemask_array_.mask_nibbles_28_to_31(i);
    BitVector<MauDefs::kSramWidth> bv(UINT64_C(0));
    // First 14 bits of bytes reg control low bytes of mask
    if ((bytes & 0x0001) != 0) bv.set_byte(0xFF, 0);
    if ((bytes & 0x0002) != 0) bv.set_byte(0xFF, 1);
    if ((bytes & 0x0004) != 0) bv.set_byte(0xFF, 2);
    if ((bytes & 0x0008) != 0) bv.set_byte(0xFF, 3);
    if ((bytes & 0x0010) != 0) bv.set_byte(0xFF, 4);
    if ((bytes & 0x0020) != 0) bv.set_byte(0xFF, 5);
    if ((bytes & 0x0040) != 0) bv.set_byte(0xFF, 6);
    if ((bytes & 0x0080) != 0) bv.set_byte(0xFF, 7);
    if ((bytes & 0x0100) != 0) bv.set_byte(0xFF, 8);
    if ((bytes & 0x0200) != 0) bv.set_byte(0xFF, 9);
    if ((bytes & 0x0400) != 0) bv.set_byte(0xFF,10);
    if ((bytes & 0x0800) != 0) bv.set_byte(0xFF,11);
    if ((bytes & 0x1000) != 0) bv.set_byte(0xFF,12);
    if ((bytes & 0x2000) != 0) bv.set_byte(0xFF,13);
    // First 4 bits of nibbles reg control hi nibbles of mask
    switch (nibbles & 0x3) {
      case 0x1: bv.set_byte(0x0F,14); break;
      case 0x2: bv.set_byte(0xF0,14); break;
      case 0x3: bv.set_byte(0xFF,14); break;
    }
    switch (nibbles & 0xC) {
      case 0x4: bv.set_byte(0x0F,15); break;
      case 0x8: bv.set_byte(0xF0,15); break;
      case 0xC: bv.set_byte(0xFF,15); break;
    }
    // Finally mask with match_mask_
    bv.mask(match_mask_bv_);
    // And update MauSram mask i
    mau_sram_->set_mask(i,bv);
    // Reverify config
    config_verify_ = true;
  }

  void MauSramReg::match_ram_vpn_write_callback() {
    if (unitram_config_.unitram_type() == 0) return;
    mau_sram_->update_vpns();
    // Reverify config
    config_verify_ = true;
  }
  void MauSramReg::tind_xbar_write_callback() {
    if (unitram_config_.unitram_type() == 0) return;
    config_change_callback();
    // Reverify config
    config_verify_ = true;
  }
  void MauSramReg::config_change_callback() {
    if (unitram_config_.unitram_type() == 0) return;
    uint16_t old_unit_vpn = unit_vpn_;
    uint16_t new_unit_vpn = get_vpn(0);
    if (new_unit_vpn != old_unit_vpn) {
      unit_vpn_ = new_unit_vpn;
      mau_sram_->update_vpns();
    }
    int old_logtab = mau_sram_->get_logical_table();
    int old_tind_buses = tind_buses_;
    int old_ltcam = mau_sram_->get_logical_tcam();
    int old_type = mau_sram_->get_type();
    int new_logtab = get_logical_table();
    int new_tind_buses = get_tind_result_buses();
    int new_ltcam = get_logical_tcam();
    int new_type = get_ram_type_check();
    if (!MauDefs::is_tind_type(new_type)) new_ltcam = -1;

    // Update stored vals
    tind_buses_ = new_tind_buses;
    // Callout if there has been a change
    if ((new_logtab != old_logtab) ||
        (new_type != old_type) ||
        (new_ltcam != old_ltcam) ||
        (new_tind_buses != old_tind_buses)) {
      mau_sram_->update(new_ltcam, 1<<new_logtab, new_type);
    }
    // Reverify config
    config_verify_ = true;
  }
  void MauSramReg::match_nibble_s0q1_enable_callback() {
    mau_sram_->match_nibble_s0q1bar_enable_update(
                  match_nibble_s0q1_enable_.match_nibble_s0q1_enable());
    // Reverify config
    config_verify_ = true;
  }
  void MauSramReg::match_nibble_s1q0_enable_callback() {
    mau_sram_->match_nibble_s1q0bar_enable_update(
                  match_nibble_s1q0_enable_.match_nibble_s1q0_enable());
    // Reverify config
    config_verify_ = true;
  }


  bool MauSramReg::is_ingress() {
    bool relax = kRelaxSramIngressCheck;
    bool ingress = (unitram_config_.unitram_ingress() == 1);
    bool egress = (unitram_config_.unitram_egress() == 1);
    if (!ingress && !egress && relax) ingress = true;
    return ingress;
  }
  bool MauSramReg::is_egress() {
    bool egress = (unitram_config_.unitram_egress() == 1);
    return egress;
  }
  bool MauSramReg::is_active() {
    return (is_ingress() || is_egress());
  }
  bool MauSramReg::check_ingress_egress(bool ingress) {
    return ((ingress) ?is_ingress() :is_egress());
  }


  int MauSramReg::get_write_data_mux() {
    return static_cast<int>(unit_ram_ctl_.match_ram_write_data_mux_select() & 0x7);
  }
  int MauSramReg::get_read_data_mux() {
    return static_cast<int>(unit_ram_ctl_.match_ram_read_data_mux_select() & 0x7);
  }
  int MauSramReg::get_search_bus() {
    return static_cast<int>(unit_ram_ctl_.match_ram_matchdata_bus1_sel() & 0x1);
  }

  bool MauSramReg::is_match_sram() {
    // Since registers 190914 MUX no longer explicitly selects HASH
    // This is implicit if unitram_type == MATCH (1)
    //bool isMatch0 = isMuxSelectingHash;
    bool isMatch1 = (unitram_config_.unitram_type() == 1);
    // Match SRAM should EITHER contain overhead fields (and so
    // drive a result bus) OR contain match entries
    bool isMatch2 = (((unit_ram_ctl_.match_result_bus_select() & 0x3) != 0) ||
                     (unit_ram_ctl_.match_entry_enable() != 0));
    if (isMatch1 && isMatch2) return true;
    if (!isMatch1 && !isMatch2) return false;
    if (config_complain_)
      RMT_LOG(RmtDebug::warn(),
              "MauSramReg::is_match_sram() Inconsistent data wrt SRAM type\n");
    return isMatch1;
  }
  int MauSramReg::get_match_result_buses() {
    return static_cast<int>(unit_ram_ctl_.match_result_bus_select() & 0x3);
  }
  int MauSramReg::get_match_result_bus() {
    int buses = get_match_result_buses();
    return buses;
  }
  bool MauSramReg::use_match_result_bus(int bus) {
    return ((bus >=0) && (bus <=1) && ((get_match_result_buses() & (1<<bus)) != 0));
  }
  int MauSramReg::get_nxtab_result_buses() {
    return static_cast<int>(row_action_nxtable_bus_drive_.row_action_nxtable_bus_drive() & 0x3);
  }
  int MauSramReg::get_nxtab_result_bus() {
    int buses = get_nxtab_result_buses();
    return buses;
  }

  int MauSramReg::get_tind_addr_bus() {
    int ram_mux_sel = ram_address_mux_ctl_.ram_unitram_adr_mux_select();
    if ((ram_mux_sel == 2) || (ram_mux_sel == 3)) return ram_mux_sel - 2;
    return -1;
  }
  bool MauSramReg::is_tind_sram() {
    bool isMuxSelectingTind = (get_tind_addr_bus() >= 0);
    bool isMatch1 = isMuxSelectingTind;
    bool isMatch2 = ((unit_ram_ctl_.tind_result_bus_select() & 0x3) != 0);
    bool isMatch3 = (unitram_config_.unitram_type() == 6);
    if (isMatch1 && isMatch2 && isMatch3) return true;
    if (!isMatch1 && !isMatch2 && !isMatch3) return false;
    if (config_complain_)
      RMT_LOG(RmtDebug::warn(),
              "MauSramReg::is_tind_sram() Inconsistent data wrt TIND type %d %d %d\n",
              isMatch1, isMatch2, isMatch3);
    return isMatch1;

  }
  int MauSramReg::get_tind_result_buses() {
    return static_cast<int>(unit_ram_ctl_.tind_result_bus_select() & 0x3);
  }
  int MauSramReg::get_tind_result_bus() {
    int buses = get_tind_result_buses();
    return buses;
  }
  bool MauSramReg::use_tind_result_bus(int bus) {
    return ((bus >=0) && (bus <=1) && ((get_tind_result_buses() & (1<<bus)) != 0));
  }
  int MauSramReg::get_result_bus() {
    int bus = 0;
    if      (is_match_sram()) bus = get_match_result_bus();
    else if (is_tind_sram())  bus = get_tind_result_bus();
    if (bus == 0) return -1;
    return (mau_sram_->row_index() << 1) | ((bus >> 1) & 0x1);
  }
  int MauSramReg::get_nxtab_bus() {
    if (! (is_match_sram() || is_tind_sram()) ) return -1;
    int bus = get_nxtab_result_bus();
    if (bus == 0) return -1;
    return (mau_sram_->row_index() << 1) | ((bus >> 1) & 0x1);
  }

  bool MauSramReg::output_rd_data(BitVector<kDataBusWidth> *data) {
    MauLogicalRow *logrow = mau_sram_->logical_row();
    RMT_ASSERT(logrow != NULL);
    int rd_bus = (unit_ram_ctl_.match_ram_read_data_mux_select());
    switch (rd_bus) {
      case 0: logrow->set_stats_rd_data(data);  return true;
      case 1: logrow->set_stats_rd_data(data);  return true;
        //case 1: logrow->set_meter_rd_data(data);  return true;
      case 2: logrow->set_oflo_rd_data(data);   return true;
      case 3: logrow->set_oflo2_rd_data(data);  return true;
        // ActionRam now outputs action_rd_data *after* ALU
        // so we suppress multi_write_check by calling _nocheck
      case 4: logrow->set_action_rd_data_nocheck(data); return true;
      default: return false;
    }
  }
  // Just for debug - read back what we output
  bool MauSramReg::input_rd_data(BitVector<kDataBusWidth> *data) {
    MauLogicalRow *logrow = mau_sram_->logical_row();
    RMT_ASSERT(logrow != NULL);
    int rd_bus = (unit_ram_ctl_.match_ram_read_data_mux_select());
    switch (rd_bus) {
      case 0: logrow->stats_rd_data(data);  return true;
      case 1: logrow->stats_rd_data(data);  return true;
        //case 1: logrow->meter_rd_data(data);  return true;
      case 2: logrow->oflo_rd_data(data);   return true;
      case 3: logrow->oflo2_rd_data(data);  return true;
      case 4: logrow->action_rd_data(data); return true;
      default: return false;
    }
  }
  bool MauSramReg::input_wr_data(BitVector<kDataBusWidth> *data) {
    MauLogicalRow *logrow = mau_sram_->logical_row();
    RMT_ASSERT(logrow != NULL);
    int wr_bus = (unit_ram_ctl_.match_ram_write_data_mux_select());
    switch (wr_bus) {
      case 0: logrow->stats_wr_data(data);  return true;
      case 1: logrow->stats_wr_data(data);  return true;
        //case 1: logrow->meter_wr_data(data);  return true;
      case 2: logrow->oflo_wr_data(data);   return true;
      case 3: logrow->oflo2_wr_data(data);  return true;
      default: return false;
    }
  }

  int MauSramReg::get_addr_mux() {
    return static_cast<int>(ram_address_mux_ctl_.ram_unitram_adr_mux_select());
  }

  bool MauSramReg::is_action_sram() {
    // Prior to registers 190914 used select==5 => ACTION (now 1 and HASH has gone)
    uint8_t ram_addr_mux_sel = ram_address_mux_ctl_.ram_unitram_adr_mux_select();
    bool isMuxSelAction = (ram_addr_mux_sel == 1);
    bool isMuxSelOflow = (ram_addr_mux_sel == 4);
    bool isMuxSelSelector = (ram_addr_mux_sel == 6);
    bool isMuxSelSelectorOflow = (ram_addr_mux_sel == 7);
    bool isMuxSelSelectorOflow2 = (ram_addr_mux_sel == 8);
    if (isMuxSelOflow) {
      bool isOflowEn = (ram_address_mux_ctl_.ram_oflo_adr_mux_select_oflo() == 1);
      bool isOflow2En = (ram_address_mux_ctl_.ram_oflo_adr_mux_select_oflo2() == 1);
      if (!isOflowEn && !isOflow2En) {
        if (config_complain_)
          RMT_LOG(RmtDebug::verbose(), "MauSramReg::is_action_sram() "
                  "Oflow input selected but neither oflow/oflow2 enabled\n");
        isMuxSelOflow = false;
      }
    }
    bool isMatch1 = (isMuxSelAction || isMuxSelOflow ||
                     isMuxSelSelector || isMuxSelSelectorOflow ||
                     isMuxSelSelectorOflow2);
    bool isMatch2 = ((unitram_config_.unitram_type() == MauDefs::kSramTypeAction) ||
                     (unitram_config_.unitram_type() == MauDefs::kSramTypeSelector));
    if (isMatch1 && isMatch2) return true;
    if (!isMatch1 && !isMatch2) return false;
    if (config_complain_)
      RMT_LOG(RmtDebug::warn(),
              "MauSramReg::is_action_sram() Inconsistent data wrt ACTION sram type\n");
    return isMatch1;
  }

  uint32_t MauSramReg::action_addr() {
    MauLogicalRow *logrow = mau_sram_->logical_row();
    RMT_ASSERT(logrow != NULL);
    uint32_t addr = Address::invalid();
    if (!is_active()) return addr;

    uint8_t ram_addr_mux_sel = ram_address_mux_ctl_.ram_unitram_adr_mux_select();
    if (ram_addr_mux_sel == 1) {
      addr = 0u; logrow->action_addr(&addr, is_ingress());
    } else if (ram_addr_mux_sel == 4) {
      if (ram_address_mux_ctl_.ram_oflo_adr_mux_select_oflo() == 1) {
        addr = 0u; logrow->oflo_addr(&addr, is_ingress(), AddrType::kAction);
      } else if (ram_address_mux_ctl_.ram_oflo_adr_mux_select_oflo2() == 1) {
        addr = 0u; logrow->oflo2_addr(&addr, is_ingress(), AddrType::kAction);
      }
    } else if (ram_addr_mux_sel == 6) {
      addr = 0u; logrow->selector_addr(&addr, is_ingress());
    } else if (ram_addr_mux_sel == 7) {
      addr = 0u; logrow->selector_oflo_addr(&addr, is_ingress());
    } else if (ram_addr_mux_sel == 8) {
      addr = 0u; logrow->oflo_selector_addr(&addr, is_ingress(), AddrType::kAction);
    }
    return addr;
  }
  uint32_t MauSramReg::action_addr_base() {
    MauLogicalRow *logrow = mau_sram_->logical_row();
    RMT_ASSERT(logrow != NULL);
    uint32_t addr = Address::invalid();
    if (!is_active()) return addr;
    // Return action addr or oflo addr - no selector addr muxing
    uint8_t ram_addr_mux_sel = ram_address_mux_ctl_.ram_unitram_adr_mux_select();
    if ((ram_addr_mux_sel == 4) || (ram_addr_mux_sel == 8)) {
      addr = 0u; logrow->oflo_addr(&addr, is_ingress(), AddrType::kAction);
    } else {
      addr = 0u; logrow->action_addr(&addr, is_ingress());
    }
    return addr;
  }



  // These next 3 funcs contain code common to both stats and meters
  bool MauSramReg::oflo_check() {
    if (ram_address_mux_ctl_.ram_ofo_stats_mux_select_oflo() == 1) {
      if ((ram_address_mux_ctl_.ram_oflo_adr_mux_select_oflo() == 1) ||
          (ram_address_mux_ctl_.ram_oflo_adr_mux_select_oflo2() == 1)) {
        return true;
      } else {
        if (config_complain_)
          RMT_LOG(RmtDebug::verbose(), "MauSramReg::oflo_check() "
                  "Delayed oflow input selected but neither oflow/oflow2 enabled");
      }
    }
    return false;
  }
  uint32_t MauSramReg::oflo_addr(uint8_t addrtype) {
    MauLogicalRow *logrow = mau_sram_->logical_row();
    RMT_ASSERT(logrow != NULL);
    uint32_t addr = Address::invalid();
    if (!is_active()) return addr;

    if (ram_address_mux_ctl_.ram_ofo_stats_mux_select_oflo() == 1) {
      if (ram_address_mux_ctl_.ram_oflo_adr_mux_select_oflo() == 1) {
        addr = 0u; logrow->oflo_addr(&addr, is_ingress(), addrtype);
      } else if (ram_address_mux_ctl_.ram_oflo_adr_mux_select_oflo2() == 1) {
        addr = 0u; logrow->oflo2_addr(&addr, is_ingress(), addrtype);
      }
    }
    return addr;
  }

  bool MauSramReg::is_synth2port_sram() {
    // Synth2Port srams MUST have synth2port maprams and MUST have certain MUX programming
    MauMapram *map = mau_sram_->mapram();
    bool isMatch = ((map != NULL) && (map->is_synth2port_mapram()));
    bool radrMux = ((ram_address_mux_ctl_.synth2port_radr_mux_select_home_row() == 1) ||
                    (ram_address_mux_ctl_.synth2port_radr_mux_select_oflo() == 1));
    bool wadrMux = ((ram_address_mux_ctl_.map_ram_wadr_mux_enable() == 1) &&
                    (ram_address_mux_ctl_.map_ram_wadr_mux_select() == 1));
    // TODO: should check synth2port hbus_members etc. as well
    if ((isMatch) && (radrMux != wadrMux) && (config_complain_)) {
      RMT_LOG(RmtDebug::warn(),
              "MauSramReg::is_synth2port_sram() Inconsistent synth2port mux programming\n");
    }
    return isMatch;
  }

  bool MauSramReg::is_stats_sram() {
    bool isMatch1 = (unitram_config_.unitram_type() == MauDefs::kSramTypeStats);
    bool isMatch2 = is_synth2port_sram();
    bool isMatch3 = false;
    if (ram_address_mux_ctl_.ram_unitram_adr_mux_select() == 5) {
      if ((ram_address_mux_ctl_.ram_ofo_stats_mux_select_statsmeter() == 1) &&
          (ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_stats() == 1)) {
        isMatch3 = true;
      } else if (ram_address_mux_ctl_.ram_ofo_stats_mux_select_oflo() == 1) {
        isMatch3 = oflo_check();
      }
    }
    if (isMatch1 && isMatch2 && isMatch3) return true;
    if (!isMatch1 && !isMatch2 && !isMatch3) return false;
    if (config_complain_)
      RMT_LOG(RmtDebug::warn(),
              "MauSramReg::is_stats_sram() Inconsistent data wrt STATS sram type\n");
    return isMatch1;
  }
  uint32_t MauSramReg::stats_addr() {
    MauLogicalRow *logrow = mau_sram_->logical_row();
    RMT_ASSERT(logrow != NULL);
    uint32_t addr = Address::invalid();
    if (mau_sram_->mapram() == NULL) return addr;
    if (!is_active()) return addr;
    if (unitram_config_.unitram_type() != MauDefs::kSramTypeStats) return addr;

    if (ram_address_mux_ctl_.ram_unitram_adr_mux_select() == 5) {
      if ((ram_address_mux_ctl_.ram_ofo_stats_mux_select_statsmeter() == 1) &&
          (ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_stats() == 1)) {
        addr = 0u; logrow->stats_addr(&addr, is_ingress());
      } else if (ram_address_mux_ctl_.ram_ofo_stats_mux_select_oflo() == 1) {
        addr = oflo_addr(AddrType::kStats);
      }
    }
    return addr;
  }

  uint32_t MauSramReg::stats_waddr() {
    // TODO: 2port: Currently don't implement synthetic 2-port SRAMs.
    // Whenever an SRAM is selected to output data onto a RD DATA BUS
    // **EXACTLY THE SAME** SRAM consumes the response data from the WR DATA
    // BUS, so for now we can simply just call stats_addr() here
    return stats_addr();
  }


  bool MauSramReg::is_meter_sram() {
    bool isMatch1 = (unitram_config_.unitram_type() == MauDefs::kSramTypeMeter);
    bool isMatch2 = is_synth2port_sram();
    bool isMatch3 = false;
    if (ram_address_mux_ctl_.ram_unitram_adr_mux_select() == 5) {
      if ((ram_address_mux_ctl_.ram_ofo_stats_mux_select_statsmeter() == 1) &&
          (ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_meter() == 1)) {
        isMatch3 = true;
      } else if (ram_address_mux_ctl_.ram_ofo_stats_mux_select_oflo() == 1) {
        isMatch3 = oflo_check();
      }
    }
    if (isMatch1 && isMatch2 && isMatch3) return true;
    if (!isMatch1 && !isMatch2 && !isMatch3) return false;
    if (config_complain_)
      RMT_LOG(RmtDebug::warn(),
              "MauSramReg::is_meter_sram() Inconsistent data wrt METER sram type\n");
    return isMatch1;
  }
  uint32_t MauSramReg::meter_addr() {
    MauLogicalRow *logrow = mau_sram_->logical_row();
    RMT_ASSERT(logrow != NULL);
    uint32_t addr = Address::invalid();
    if (!is_active()) return addr;
    if ((unitram_config_.unitram_type() != MauDefs::kSramTypeMeter) &&
        (unitram_config_.unitram_type() != MauDefs::kSramTypeStateful) &&
        (unitram_config_.unitram_type() != MauDefs::kSramTypeSelector)) return addr;

    if (ram_address_mux_ctl_.ram_unitram_adr_mux_select() == 5) {
      if ((ram_address_mux_ctl_.ram_ofo_stats_mux_select_statsmeter() == 1) &&
          (ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_meter() == 1)) {
        addr = 0u; logrow->meter_addr(&addr, is_ingress());
      } else if (ram_address_mux_ctl_.ram_ofo_stats_mux_select_oflo() == 1) {
        addr = oflo_addr(AddrType::kMeter);
      }
    }
    return addr;
  }

  uint32_t MauSramReg::meter_waddr() {
    // TODO: 2port: Currently don't implement synthetic 2-port SRAMs.
    // Whenever an SRAM is selected to output data onto a RD DATA BUS
    // **EXACTLY THE SAME** SRAM consumes the response data from the WR DATA
    // BUS, so for now we can simply just call meter_addr() here
    return meter_addr();
  }


  bool MauSramReg::is_selector_sram() {
    bool isMatch1 = (unitram_config_.unitram_type() == MauDefs::kSramTypeSelector);
    bool isMatch2 = is_synth2port_sram();
    if (isMatch1 && isMatch2) return true;
    if (!isMatch1 && !isMatch2) return false;
    if (config_complain_)
      RMT_LOG(RmtDebug::verbose(),
              "MauSramReg::is_selector_sram() "
              "Inconsistent data wrt SELECTOR sram type\n");
    return isMatch1;
  }
  //uint32_t MauSramReg::selector_addr()     { return meter_addr(); }
  //uint32_t MauSramReg::selector_waddr()    { return meter_waddr(); }



  bool MauSramReg::is_stateful_sram() {
    bool isMatch1 = (unitram_config_.unitram_type() == MauDefs::kSramTypeStateful);
    bool isMatch2 = is_synth2port_sram();
    if (isMatch1 && isMatch2) return true;
    if (!isMatch1 && !isMatch2) return false;
    if (config_complain_)
      RMT_LOG(RmtDebug::warn(),
              "MauSramReg::is_stateful_sram() "
              "Inconsistent data wrt STATEFUL sram type\n");
    return isMatch1;
  }
  //uint32_t MauSramReg::stateful_addr()     { return meter_addr(); }
  //uint32_t MauSramReg::stateful_waddr()    { return meter_waddr(); }


  bool MauSramReg::is_selector_action_addr() {
    return ((ram_address_mux_ctl_.ram_unitram_adr_mux_select() == 6) ||
            (ram_address_mux_ctl_.ram_unitram_adr_mux_select() == 7));
  }

  bool MauSramReg::output_action_subword() {
    //return ((unitram_config_.unitram_action_subword_out_en() & 0x1) != 0);
    return true;
  }



  int MauSramReg::get_ram_type_check() {
    bool typematch = false;
    int type = unitram_config_.unitram_type();
    switch (type) {
      case MauDefs::kSramTypeInvalid:  typematch = false;              break;
      case MauDefs::kSramTypeMatch:    typematch = is_match_sram();    break;
      case MauDefs::kSramTypeAction:   typematch = is_action_sram();   break;
      case MauDefs::kSramTypeStats:    typematch = is_stats_sram();    break;
      case MauDefs::kSramTypeMeter:    typematch = is_meter_sram();    break;
      case MauDefs::kSramTypeStateful: typematch = is_stateful_sram(); break;
      case MauDefs::kSramTypeTind:     typematch = is_tind_sram();     break;
      case MauDefs::kSramTypeSelector: typematch = is_selector_sram(); break;
      default:                         typematch = false;              break;
    }
    if (!typematch) {
      if (type != 0)
        if (config_complain_)
          RMT_LOG(RmtDebug::warn(),
                  "MauSramReg::get_ram_type_check(%d) "
                  "SRAM doesn't match its stated type\n", type);
      bool type2 = MauDefs::kSramTypeInvalid;
      if (is_match_sram())    type2 = MauDefs::kSramTypeMatch;
      if (is_action_sram())   type2 = MauDefs::kSramTypeAction;
      if (is_stats_sram())    type2 = MauDefs::kSramTypeStats;
      if (is_meter_sram())    type2 = MauDefs::kSramTypeMeter;
      if (is_stateful_sram()) type2 = MauDefs::kSramTypeStateful;
      if (is_tind_sram())     type2 = MauDefs::kSramTypeTind;
      if (is_selector_sram()) type2 = MauDefs::kSramTypeSelector;
      type = MauDefs::kSramTypeInvalid;
      if (is_selector_sram()) type = MauDefs::kSramTypeSelector;
      if (is_tind_sram())     type = MauDefs::kSramTypeTind;
      if (is_stateful_sram()) type = MauDefs::kSramTypeStateful;
      if (is_meter_sram())    type = MauDefs::kSramTypeMeter;
      if (is_stats_sram())    type = MauDefs::kSramTypeStats;
      if (is_action_sram())   type = MauDefs::kSramTypeAction;
      if (is_match_sram())    type = MauDefs::kSramTypeMatch;
      if (type2 != type) {
        if (config_complain_)
          RMT_LOG(RmtDebug::error(),
                  "MauSramReg::get_ram_type_check(%d) "
                  "SRAM could be more than one type!\n", type);
      }
    }
    return type;
  }


  uint8_t MauSramReg::match_enables() {
    if (!is_match_sram()) return 0;
    // Enables moved from match_nexttable_bitpos -> unit_ram_ctl
    // NB. Match0 use to be always enabled - but now there are enables for all matches
    return unit_ram_ctl_.match_entry_enable();
  }
  bool MauSramReg::get_next_table_bitpos(const int which, uint32_t *bitpos) {
    RMT_ASSERT((which >= 0) && (which < MauSram::kMaskEntries));
    RMT_ASSERT(bitpos != NULL);
    // uArch fig6.60 suggests nxt-tab bitpos NOT gated by match-entry-enable
    //if ((match_enables() & (1<<which)) == 0) return false;
    uint8_t lim, pos;
    switch (which) {
      case 0:  *bitpos = 0u; return true;
      case 1:  lim =  7; pos = match_nexttable_bitpos_.match_next_table1_bitpos(); break;
      case 2:  lim = 15; pos = match_nexttable_bitpos_.match_next_table2_bitpos(); break;
      case 3:  lim = 23; pos = match_nexttable_bitpos_.match_next_table3_bitpos(); break;
      case 4:  lim = 31; pos = match_nexttable_bitpos_.match_next_table4_bitpos(); break;
      default: RMT_ASSERT(0); break;
    }
    if (pos > lim) {
      if (config_complain_)
        RMT_LOG(RmtDebug::error(kRelaxSramBitposCheck),
                "MauSramReg::get_next_table_bitpos(%d) Bitpos %d TOO big (lim=%d)\n",
                which, pos, lim);
      if (!kRelaxSramBitposCheck) { THROW_ERROR(-2); } // For DV
      return false;
    }
    *bitpos = static_cast<uint32_t>(1 + pos); // Add 1 for bitpos 1,2,3,4
    return true;
  }


  uint16_t MauSramReg::get_vpn(int which_vpn) {
    RMT_ASSERT((which_vpn >= 0) && (which_vpn <= MauSram::kVpns));
    if (is_match_sram()) {
      RMT_ASSERT((which_vpn >= 0) && (which_vpn <= kMatchEntries));
      // Get 3 bits defining VPN
      uint16_t vpn_lsb = (match_ram_vpn_.match_ram_vpn_lsbs() >> (which_vpn * 3)) & 0x7;
      // Use top-bit (bit2) to determine bits[8:2] - bits[1:0] are preserved
      if ((vpn_lsb & 0x4) == 0x4)
        return ((match_ram_vpn_.match_ram_vpn1() & 0x7F) << 2) | (vpn_lsb & 0x3);
      else
        return ((match_ram_vpn_.match_ram_vpn0() & 0x7F) << 2) | (vpn_lsb & 0x3);
    } else {
      return (which_vpn == 0) ?unitram_config_.unitram_vpn() :0xFFFF;
    }
  }
  int MauSramReg::get_logical_table() {
    int lt = unitram_config_.unitram_logical_table();
    if (is_match_sram()) {
      int lt2 = unit_ram_ctl_.match_ram_logical_table();
      if (lt != lt2) {
        if (config_complain_)
          RMT_LOG(RmtDebug::warn(),
                  "MauSramReg::get_logical_table() Inconsistent logical table config "
                  "(unitram_config=%d unit_ram_ctl=%d)\n", lt, lt2);
      }
    }
    return lt;
  }
  int MauSramReg::get_logical_tcam() {
    int ltcam = -1;
    int which = get_tind_addr_bus();
    if ((adr_dist_tind_adr_xbar_ctl_array_ != NULL) && ((which == 0) || (which == 1)) &&
        ((adr_dist_tind_adr_xbar_ctl_array_->enabled_3bit_muxctl_enable(which) & 0x1) != 0)) {
      ltcam = adr_dist_tind_adr_xbar_ctl_array_->enabled_3bit_muxctl_select(which) & 0x7;
    }
    return ltcam;
  }
  int MauSramReg::get_ram_type() {
    return unitram_config_.unitram_type();
  }

  bool MauSramReg::matches_type_vpn_table(const int type, const int vpn, const int table) {
    bool typematch = true;
    bool vpnmatch = true;
    bool tablematch = true;
    int n_vpns = 0;

    if (type >= 0) {
      typematch = false;
      switch (type) {
        case MauDefs::kSramTypeInvalid:  typematch = false;              n_vpns = 0; break;
        case MauDefs::kSramTypeMatch:    typematch = is_match_sram();    n_vpns = 5; break;
        case MauDefs::kSramTypeAction:   typematch = is_action_sram();   n_vpns = 1; break;
        case MauDefs::kSramTypeStats:    typematch = is_stats_sram();    n_vpns = 6; break;
        case MauDefs::kSramTypeMeter:    typematch = is_meter_sram();    n_vpns = 1; break;
        case MauDefs::kSramTypeStateful: typematch = is_stateful_sram(); n_vpns = 1; break;
        case MauDefs::kSramTypeTind:     typematch = is_tind_sram();     n_vpns = 1; break;
        case MauDefs::kSramTypeSelector: typematch = is_selector_sram(); n_vpns = 1; break;
        default:                         typematch = false;              n_vpns = 0; break;
      }
    }
    if ((vpn >= 0) && (n_vpns > 0)) {
      vpnmatch = false;
      for (int i = 0; i < n_vpns; i++) {
        if (get_vpn(i) == vpn) vpnmatch = true;
      }
    }
    if (table >= 0) {
      tablematch = false;
      if (get_logical_table() == table) tablematch = true;
    }

    return (typematch && vpnmatch && tablematch);
  }


  int MauSramReg::get_alu_type() {
    switch (get_ram_type()) {
      case MauDefs::kSramTypeStats:    return 0;
      case MauDefs::kSramTypeMeter:    return 1;
      case MauDefs::kSramTypeStateful: return 1;
      case MauDefs::kSramTypeSelector: return 1;
    }
    return -1;
  }
  int MauSramReg::get_alu_index() {
    // NEW CODE THAT USES INFO FROM LOGICAL ROW
    MauLogicalRow *logrow = mau_sram_->logical_row();
    int     alu = -1;
    int     home_alu, oflow_alu;
    uint8_t home_addrtype, oflow_addrtype;

    if (logrow->get_alus(&home_alu, &home_addrtype, NULL,
                         &oflow_alu, &oflow_addrtype, NULL)) {

      if (ram_address_mux_ctl_.ram_unitram_adr_mux_select() == 5) {
        if (ram_address_mux_ctl_.ram_ofo_stats_mux_select_statsmeter() == 1) {
          if (ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_meter() == 1) {
            RMT_ASSERT(ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_stats() == 0);
            RMT_ASSERT(home_alu >= 0);
            RMT_ASSERT(home_addrtype == AddrType::kMeter);
            RMT_ASSERT(get_alu_type() == 1); // Just to check RAM type
            alu = home_alu;
          }
          else if (ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_stats() == 1) {
            RMT_ASSERT(home_alu >= 0);
            RMT_ASSERT(home_addrtype == AddrType::kStats);
            RMT_ASSERT(get_alu_type() == 0); // Just to check RAM type
            alu = home_alu;
          }
        } else if (ram_address_mux_ctl_.ram_ofo_stats_mux_select_oflo() == 1) {
          if (oflow_alu >= 0) {
            if (oflow_addrtype == AddrType::kMeter) {
              RMT_ASSERT(get_alu_type() == 1); // Just to check RAM type
              alu = oflow_alu;
            } else if (oflow_addrtype == AddrType::kStats) {
              RMT_ASSERT(get_alu_type() == 0); // Just to check RAM type
              alu = oflow_alu;
            }
          }
        }
      }
    }
    return alu;
  }
  int MauSramReg::get_alu_logrow_index() {
    int alu_idx = get_alu_index();
    if (alu_idx < 0) return -1;
    switch (get_alu_type()) {
      case 0: return MauStatsAlu::get_stats_alu_logrow_index(alu_idx);
      case 1: return MauMeterAlu::get_meter_alu_logrow_index(alu_idx);
    }
    return -1;
  }
  int MauSramReg::get_alu_row_index() {
    int alu_logrow_idx = get_alu_logrow_index();
    return (alu_logrow_idx < 0) ?-1 :mau_sram_->mau()->physical_row_index(alu_logrow_idx);
  }
  MauSramRow *MauSramReg::get_alu_row() {
    int alu_row_idx = get_alu_row_index();
    return (alu_row_idx < 0) ?NULL: mau_sram_->mau()->sram_row_lookup(alu_row_idx);
  }


  void MauSramReg::reset_resources() {
    if (!config_verify_) return;
    // Check stuff out
    bool prev_complain = config_complain_;
    config_complain_ = true;
    //RMT_LOG(RmtDebug::warn(), "Re-verifying SRAM config...\n");
    (void)get_ram_type_check();
    (void)get_logical_table();
    //RMT_LOG(RmtDebug::warn(), "Re-verifying SRAM config...DONE\n");
    config_complain_ = prev_complain;
    config_verify_ = false;
  }

}
