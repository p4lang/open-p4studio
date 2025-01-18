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
#include <mau-mapram-reg.h>
#include <mau-mapram.h>
#include <register_adapters.h>



// An example to clarify vpn + vpn_member
//
// vpn=1111111
// vpn_members=1011
//
// makes the matching VPNs be:
//
// 1=111111100 (bot 2 bits = 00 because bit 0 selected in members)
// 2=111111101 (bot 2 bits = 01 because bit 1 selected)
// 3=111111111 (bot 2 bits = 11 because bit 3 selected)



namespace MODEL_CHIP_NAMESPACE {

  MauMapramReg::MauMapramReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                             int rowIndex, int colIndex, int mapramIndex, MauMapram *mauMapram)
      : MauObject(om, pipeIndex, mauIndex, kType, rowIndex, colIndex),
        mau_mapram_(mauMapram), mau_addr_dist_(mauMapram->mau()->mau_addr_dist()),
        row_index_(rowIndex), col_index_(colIndex),
        type_(MauDefs::kMapramTypeInvalid), bus_(-1), order_(0),
        // Since regs 13957_mau_dev maprams only on RHS so mapram_config
        // and mapram_ctl register arrays have lost a dimension
        mapram_config_(default_adapter(mapram_config_,chip_index(), pipeIndex, mauIndex, rowIndex,
                       colIndex % kMapramHalfColumns,
                       [this](){this->config_change_callback();})),
        //mapram_ctl_(chip_index(), pipeIndex, mauIndex, rowIndex,
        //            colIndex % kMapramHalfColumns,
        //            [this](){this->config_change_callback();}),
        ram_address_mux_ctl_(default_adapter(ram_address_mux_ctl_,chip_index(), pipeIndex, mauIndex,
                             rowIndex, (colIndex<kMapramHalfColumns) ? 0 : 1 ,
                             colIndex % kMapramHalfColumns,
                             [this](){this->config_change_callback();})),
        adr_dist_idletime_adr_xbar_ctl_(default_adapter(adr_dist_idletime_adr_xbar_ctl_,
            chip_index(), pipeIndex, mauIndex, rowIndex,
            colIndex % kMapramHalfColumns,
            [this](){this->idletime_xbar_write_callback();})),
        idletime_cfg_rd_clear_val_(default_adapter(idletime_cfg_rd_clear_val_,chip_index(), pipeIndex, mauIndex, rowIndex,
                                   colIndex % kMapramHalfColumns))

  {
    mapram_config_.reset();
    //mapram_ctl_.reset();
    ram_address_mux_ctl_.reset();
    adr_dist_idletime_adr_xbar_ctl_.reset();
    idletime_cfg_rd_clear_val_.reset();
  }
  MauMapramReg::~MauMapramReg() {
  }

  void MauMapramReg::notify_sram_row() {
    // Call MauSramRowReg code so it can update its info re Mapram config
    MauSramRow *sram_row = mau_mapram_->physical_row();
    MauSramRowReg *sram_row_reg = (sram_row != nullptr) ?sram_row->row_registers() :nullptr;
    if (sram_row_reg != nullptr) sram_row_reg->mapram_change_callback(col_index_);
  }
  void MauMapramReg::maybe_notify_sweeper() {
    bool wasIdletime = (type_ == MauDefs::kMapramTypeIdletime);
    int wasBus = (wasIdletime) ?bus_ :-1;
    type_ = get_mapram_type();
    bus_ = get_idle_bus();
    bool isIdletime = (type_ == MauDefs::kMapramTypeIdletime);
    if ((wasIdletime != isIdletime) || (wasBus != bus_))
      mau_addr_dist_->mapram_change_callback();
  }
  void MauMapramReg::maybe_check_valid_idletime() {
    if (mapram_config_.mapram_type() != MauDefs::kMapramTypeIdletime) return;
    if (!is_valid_idletime_mode()) {
        RMT_LOG(RmtDebug::error(), "MauMapramReg::maybe_check_valid_idletime() "
                "Invalid idletime bitwidth %d\n", idletime_bitwidth());
    }
  }
  void MauMapramReg::check_parity_ecc() {
    // Check parity/ECC not both specified
    if (has_parity() && has_ecc()) {
        RMT_LOG(RmtDebug::error(), "MauMapramReg::check_parity_ecc() "
                "Mapram configured for BOTH parity and ECC\n");
    }
    if (mapram_config_.mapram_type() != MauDefs::kMapramTypeIdletime) return;
    // If idletime mapram check idletime bit widths as expected for parity/ECC
    int idle_ram_width = idletime_width();
    if (has_parity() && ((idle_ram_width == 3) || (idle_ram_width == 6))) {
        RMT_LOG(RmtDebug::error(), "MauMapramReg::check_parity_ecc() "
                "%d-bit idletime mapram configured for parity!\n", idle_ram_width);
    }
    if (has_ecc() && ((idle_ram_width == 1) || (idle_ram_width == 2))) {
        RMT_LOG(RmtDebug::error(), "MauMapramReg::check_parity_ecc() "
                "%d-bit idletime mapram configured for ECC!\n", idle_ram_width);
    }
  }

  void MauMapramReg::config_change_callback() {
    if (mapram_config_.mapram_type() == 0) return;
    check_parity_ecc();
    maybe_check_valid_idletime();
    maybe_notify_sweeper();
    notify_sram_row();
  }
  void MauMapramReg::idletime_xbar_write_callback() {
    if (mapram_config_.mapram_type() == 0) return;
    maybe_notify_sweeper();
  }

  bool MauMapramReg::is_ingress() {
    bool relax = kRelaxMapramIngressCheck;
    bool ingress = (mapram_config_.mapram_ingress() == 1);
    bool egress = (mapram_config_.mapram_egress() == 1);
    if (!ingress && !egress && relax) ingress = true;
    return ingress;
  }
  bool MauMapramReg::is_egress() {
    //bool ingress = (mapram_config_.mapram_ingress() == 1);
    bool egress = (mapram_config_.mapram_egress() == 1);
    return egress;
  }
  bool MauMapramReg::is_active() {
    return (is_ingress() || is_egress());
  }
  bool MauMapramReg::check_ingress_egress(bool ingress) {
    return ((ingress) ?is_ingress() :is_egress());
  }


  int MauMapramReg::get_idle_bus() {
    if (!adr_dist_idletime_adr_xbar_ctl_.enabled_4bit_muxctl_enable()) return -1;
    int bus = adr_dist_idletime_adr_xbar_ctl_.enabled_4bit_muxctl_select();
    // Upto 10 buses to keep the 0-19 pattern (all used inc 9/19)
    // Each mapram can select from 0-9 in their half
    if ((bus < 0) || (bus > 9)) return -1;
    if (row_index_ >= kMapramRowFirstTOP) bus += 10;
    if ((mau_addr_dist_->get_idletime_logical_table(bus) != get_logical_table()) &&
        (mau_addr_dist_->get_idletime_logical_table(bus) >= 0)) {
      RMT_LOG(RmtDebug::warn(),
              "MauMapramReg::get_idle_bus(): Inconsistent data wrt logical table [%d vs %d]\n",
              mau_addr_dist_->get_idletime_logical_table(bus), get_logical_table());
    }
    return bus;
  }


  bool MauMapramReg::is_synth2port_mapram() {
    uint8_t type = mapram_config_.mapram_type();
    bool typeMatch = ((type == MauDefs::kMapramTypeStats) ||
                      (type == MauDefs::kMapramTypeMeter) ||
                      (type == MauDefs::kMapramTypeStateful) ||
                      (type == MauDefs::kMapramTypeSelector));
    bool radrMux = ((ram_address_mux_ctl_.synth2port_radr_mux_select_home_row() == 1) ||
                    (ram_address_mux_ctl_.synth2port_radr_mux_select_oflo() == 1));
    bool wadrMux = ((ram_address_mux_ctl_.map_ram_wadr_mux_enable() == 1) &&
                    (ram_address_mux_ctl_.map_ram_wadr_mux_select() == 1));
    return typeMatch && radrMux && wadrMux;
  }
  bool MauMapramReg::is_synth2port_home_mapram() {
    return (is_synth2port_mapram() &&
            (ram_address_mux_ctl_.synth2port_radr_mux_select_home_row() == 1));
  }
  bool MauMapramReg::is_synth2port_oflo_mapram() {
    return (is_synth2port_mapram() &&
            (ram_address_mux_ctl_.synth2port_radr_mux_select_oflo() == 1));
  }
  bool MauMapramReg::is_statistics_mapram() {
    // TODO: Add consistency checks and warnings
    return (mapram_config_.mapram_type() == MauDefs::kMapramTypeStats);
  }
  bool MauMapramReg::is_meter_mapram() {
    // TODO: Add consistency checks and warnings
    return (mapram_config_.mapram_type() == MauDefs::kMapramTypeMeter);
  }
  bool MauMapramReg::is_stateful_mapram() {
    // TODO: Add consistency checks and warnings
    return (mapram_config_.mapram_type() == MauDefs::kMapramTypeStateful);
  }
  bool MauMapramReg::is_idletime_mapram() {
    bool isMatch1 = (mapram_config_.mapram_type() == MauDefs::kMapramTypeIdletime);
    // isMatch2 might be true for colormap rams, bail out now: TODO: remove as I think it is not true!
    //if (mapram_config_.mapram_type() == MauDefs::kMapramTypeColor) return false;
    bool isMatch2 = (( mapram_radr_mux_select() == kMapramRAdrSMOflo ) &&
                     (ram_address_mux_ctl_.ram_ofo_stats_mux_select_statsmeter() == 1) &&
                     (ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_idlet() == 1));
    if (isMatch1 && isMatch2) return true;
    if (!isMatch1 && !isMatch2) return false;
    RMT_LOG(RmtDebug::warn(),
            "MauMapramReg::is_idletime_mapram(): Inconsistent data wrt mapram type\n");
    return isMatch1;
  }
  bool MauMapramReg::is_color_mapram() {
    // TODO: Add consistency checks and warnings
    return (mapram_config_.mapram_type() == MauDefs::kMapramTypeColor);
  }
  bool MauMapramReg::is_color_home_mapram() {
    return (is_color_mapram() && (mapram_config_.mapram_color_write_bus_select() == 0));
  }
  bool MauMapramReg::is_color_oflo_mapram() {
    return (is_color_mapram() && (mapram_config_.mapram_color_write_bus_select() == 1));
  }
  bool MauMapramReg::is_selector_mapram() {
    // TODO: Add consistency checks and warnings
    return (mapram_config_.mapram_type() == MauDefs::kMapramTypeSelector);
  }

  uint32_t MauMapramReg::idletime_addr() {
    // Look at xbar to find idletime bus we're connected to
    uint32_t addr = Address::invalid();
    if (!is_active()) return addr;
    if (mapram_config_.mapram_type() != MauDefs::kMapramTypeIdletime) return addr;
    int bus = get_idle_bus();
    if (bus < 0) return addr;
    if (( mapram_radr_mux_select() == kMapramRAdrSMOflo ) &&
        (ram_address_mux_ctl_.ram_ofo_stats_mux_select_statsmeter() == 1) &&
        (ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_idlet() == 1)) {
      addr = mau_addr_dist()->idle_addr(bus, is_ingress());
    }
    return addr;
  }

  uint32_t MauMapramReg::color_read_addr(uint8_t* addrtype /*=nullptr*/) {
    uint32_t addr = Address::invalid();
    if (addrtype) *addrtype = AddrType::kNone;
    if (!is_active()) return addr;
    if (mapram_config_.mapram_type() != MauDefs::kMapramTypeColor) return addr;
    MauLogicalRow* logrow = mau_mapram_->logical_row();
    RMT_ASSERT( logrow );

    if ( mapram_radr_mux_select() == kMapramRAdrColor ) {
      switch ( ram_ofo_stats_mux_select() ) {
        case kRamOfoStatsMuxNone:
          break;
        case kRamOfoStatsMuxOflo:
          if ( ram_address_mux_ctl_.ram_oflo_adr_mux_select_oflo() ) {
            addr = 0u; logrow->oflo_addr(&addr, is_ingress(), AddrType::kStats );
            if (addrtype) *addrtype = AddrType::kStats;
          } else {
            RMT_ASSERT(0);
          }
          break;
        case kRamOfoStatsMuxStatsMeter:
          if ( ram_stats_meter_adr_mux_select() == kRamStatsMeterAdrMuxStats ) {
            addr = 0u; logrow->stats_addr(&addr, is_ingress());
            if (addrtype) *addrtype = AddrType::kStats;
          }
          else if ( ram_stats_meter_adr_mux_select() == kRamStatsMeterAdrMuxIdlet ) {
            int bus = get_idle_bus();
            if (bus >= 0) {
              addr = mau_addr_dist()->idle_addr(bus, is_ingress());
              if (addrtype) *addrtype = AddrType::kIdle;
            }
          } else {
            RMT_ASSERT(0);
          }
          break;
        case kRamOfoStatsMuxInvalid:
          RMT_ASSERT(0);
          break;
      }
    }
    return addr;
  }
  uint32_t MauMapramReg::color_write_addr() {
    uint32_t addr = Address::invalid();
    if (!is_active()) return addr;
    if (mapram_config_.mapram_type() != MauDefs::kMapramTypeColor) return addr;
    if (ram_address_mux_ctl_.map_ram_wadr_mux_enable() == 0) return addr;
    if (ram_address_mux_ctl_.map_ram_wadr_mux_select() != 3) return addr;
    MauLogicalRow* logrow = mau_mapram_->logical_row();
    RMT_ASSERT( logrow );
    if (ram_address_mux_ctl_.synth2port_radr_mux_select_home_row() == 1) {
      addr = 0u; logrow->meter_rd_addr(&addr);
    } else if (ram_address_mux_ctl_.synth2port_radr_mux_select_oflo() == 1) {
      addr = 0u; logrow->oflo_rd_addr(&addr, AddrType::kMeter);
    }
    return addr;
  }
  uint8_t MauMapramReg::color_write_data() {
    uint8_t color=0;
    bool oflo = mapram_config_.mapram_color_write_bus_select();
    MauLogicalRow* logrow = mau_mapram_->logical_row();
    RMT_ASSERT( logrow );
    if ( oflo ) { // Oflo
      logrow->oflow_color_write_data(&color);
    }
    else { // Color write bus
      logrow->color_write_data(&color);
    }
    return color;
  }

  bool MauMapramReg::vpn_match(int vpn) {
    // mapram_vpn_members no longer exists!
    // Bits [8:2] must match mapram_vpn
    // Single bit corresponding to bits [1:0] must be set in vpn_members
    //return ((((vpn >> 2) & 0x3F) == (mapram_config_.mapram_vpn() & 0x3F)) &&
    //((mapram_config_.mapram_vpn_members() & (1 << (vpn & 0x3))) != 0));

    int mask = 0x3f;
    if (mapram_config_.mapram_type() == MauDefs::kMapramTypeColor) {
      mask = 0xf; // only 4 bits are used for vpn in color maprams
    }
    return ((vpn & mask) == (mapram_config_.mapram_vpn() & mask));
  }
  int MauMapramReg::get_logical_table() {
    return mapram_config_.mapram_logical_table();
  }
  int MauMapramReg::get_type() {
    return type_;
  }

  int MauMapramReg::get_vpn() {
    int vpn = mapram_config_.mapram_vpn() & 0x3F;
    return vpn;
  }
  int MauMapramReg::get_vpn_min() {
    return get_vpn(); // Just return vpn
    //if ((mapram_config_.mapram_vpn_members() & 0x1) != 0) return (vpn << 2) | 0x0;
    //if ((mapram_config_.mapram_vpn_members() & 0x2) != 0) return (vpn << 2) | 0x1;
    //if ((mapram_config_.mapram_vpn_members() & 0x4) != 0) return (vpn << 2) | 0x2;
    //if ((mapram_config_.mapram_vpn_members() & 0x8) != 0) return (vpn << 2) | 0x3;
    //return -1;
  }
  int MauMapramReg::get_vpn_max() {
    return get_vpn(); // Just return vpn
    //if ((mapram_config_.mapram_vpn_members() & 0x8) != 0) return (vpn << 2) | 0x3;
    //if ((mapram_config_.mapram_vpn_members() & 0x4) != 0) return (vpn << 2) | 0x2;
    //if ((mapram_config_.mapram_vpn_members() & 0x2) != 0) return (vpn << 2) | 0x1;
    //if ((mapram_config_.mapram_vpn_members() & 0x1) != 0) return (vpn << 2) | 0x0;
    //return -1;
  }

  uint8_t MauMapramReg::get_order() {
    return order_;
  }
  void MauMapramReg::set_order(uint8_t order) {
    order_ = order;
  }
  uint32_t MauMapramReg::get_priority() {
    uint8_t order = get_order();
    uint8_t vpn = static_cast<uint8_t>(get_vpn()  & 0xFF);
    uint8_t row = static_cast<uint8_t>(row_index_ & 0xFF);
    uint8_t col = static_cast<uint8_t>(col_index_ & 0xFF);
    return (order << 24) | (vpn << 16) | (row << 8) | (col << 0);
  }


  int MauMapramReg::get_mapram_type_check() {
    bool typematch = false;
    int type = mapram_config_.mapram_type();
    switch (type) {
      case MauDefs::kMapramTypeInvalid:   typematch = false;                  break;
      case MauDefs::kMapramTypeStats:     typematch = is_statistics_mapram(); break;
      case MauDefs::kMapramTypeMeter:     typematch = is_meter_mapram();      break;
      case MauDefs::kMapramTypeStateful:  typematch = is_stateful_mapram();   break;
      case MauDefs::kMapramTypeIdletime:  typematch = is_idletime_mapram();   break;
      case MauDefs::kMapramTypeColor:     typematch = is_color_mapram();      break;
      case MauDefs::kMapramTypeSelector:  typematch = is_selector_mapram();   break;
      default:                            typematch = false;                  break;
    }
    if (!typematch) {
      RMT_LOG(RmtDebug::warn(),
              "MauMapramReg::get_mapram_type_check(): MAPRAM doesn't match its stated type\n");
      bool type2 = MauDefs::kMapramTypeInvalid;
      if (is_statistics_mapram())   type2 = MauDefs::kMapramTypeStats;
      if (is_meter_mapram())        type2 = MauDefs::kMapramTypeMeter;
      if (is_stateful_mapram())     type2 = MauDefs::kMapramTypeStateful;
      if (is_idletime_mapram())     type2 = MauDefs::kMapramTypeIdletime;
      if (is_color_mapram())        type2 = MauDefs::kMapramTypeColor;
      if (is_selector_mapram())     type2 = MauDefs::kMapramTypeSelector;
      type = MauDefs::kMapramTypeInvalid;
      if (is_selector_mapram())     type = MauDefs::kMapramTypeSelector;
      if (is_color_mapram())        type = MauDefs::kMapramTypeColor;
      if (is_idletime_mapram())     type = MauDefs::kMapramTypeIdletime;
      if (is_stateful_mapram())     type = MauDefs::kMapramTypeStateful;
      if (is_meter_mapram())        type = MauDefs::kMapramTypeMeter;
      if (is_statistics_mapram())   type = MauDefs::kMapramTypeStats;
      if (type2 != type) {
        RMT_LOG(RmtDebug::warn(),
                "MauMapramReg::get_mapram_type_check(): MAPRAM could be more than one type!\n");
      }
    }
    return type;
  }
  int MauMapramReg::get_mapram_type() {
    return get_mapram_type_check();
  }

  int MauMapramReg::get_alu_type() {
    switch (get_mapram_type()) {
      case MauDefs::kMapramTypeStats:    return  0;
      case MauDefs::kMapramTypeMeter:    return  1;
      case MauDefs::kMapramTypeStateful: return  1;
      case MauDefs::kMapramTypeIdletime: return -1;
      case MauDefs::kMapramTypeColor:    return  1;
      case MauDefs::kMapramTypeSelector: return  1;
    }
    return -1;
  }

  int MauMapramReg::get_sram_alu_index() {
    switch (get_mapram_type()) {
      case MauDefs::kMapramTypeIdletime: return -1;
      case MauDefs::kMapramTypeColor:    return -1;
    }
    // Find SRAM corresponding to this MapRAM
    MauSram *sram = mau()->sram_lookup(row_index_, col_index_);
    return (sram != NULL) ?sram->get_alu_index() :-1;
  }
  int MauMapramReg::get_color_alu_index() {
    if (!is_color_mapram()) return -1;
    MauLogicalRow *logrow = mau_mapram_->logical_row();
    if (logrow == NULL) return -1;

    if (mapram_config_.mapram_color_write_bus_select() == 0) {
      // Using homerow
      return MauMeterAlu::get_meter_alu_regs_index(logrow->logical_row_index());
    } else {
      // Using oflow - query switchbox to find ALU
      MauSramRow *sramrow = logrow->physical_row();
      if (sramrow == NULL) return -1;
      MauSramRowReg *sramrowreg = sramrow->row_registers();
      if (sramrowreg == NULL) return -1;
      return sramrowreg->get_color_alu_index();
    }
  }
  int MauMapramReg::get_alu_index() {
    // NEW CODE THAT USES INFO FROM SRAM/COLOR SWITCHBOX
    int alu = -1;
    if (alu < 0) alu = get_sram_alu_index();
    if (alu < 0) alu = get_color_alu_index();
    return alu;
  }
  int MauMapramReg::get_meter_alu_index() {
    return (get_alu_type() == 1) ?get_alu_index() :-1;
  }


}
