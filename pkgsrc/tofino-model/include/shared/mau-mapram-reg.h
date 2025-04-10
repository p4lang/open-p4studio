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

#ifndef _SHARED_MAU_MAPRAM_REG_
#define _SHARED_MAU_MAPRAM_REG_

#include <cstdint>
#include <bitvector.h>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>

// Reg defs auto-generated from Semifore
#include <register_includes/ram_address_mux_ctl.h>
#include <register_includes/mapram_config.h>
//#include <tofino/register_includes/mapram_ctl.h>
#include <register_includes/adr_dist_idletime_adr_xbar_ctl.h>
#include <register_includes/idletime_cfg_rd_clear_val.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauMapram;
  class MauAddrDist;

  class MauMapramReg : public MauObject {
 public:
    // Defined in rmt-config.cpp
    static bool kRelaxMapramIngressCheck;

    static constexpr int kType = RmtTypes::kRmtTypeMauMapramReg;
    static constexpr int kMapramRowFirstTOP = MauDefs::kMapramRowFirstTOP;
    static constexpr int kMapramHalfColumns = MauDefs::kMapramHalfColumns;
    static constexpr uint8_t kMapramVpnMax  = MauDefs::kMapramVpnMax;

    MauMapramReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                 int rowIndex, int colIndex, int mapramIndex,
                 MauMapram *mauMapram);
    virtual ~MauMapramReg();

    // Access broadcast idletime buses
    inline MauAddrDist *mau_addr_dist() const { return mau_addr_dist_; }

    // IDLETIME info
    // Harmonise naming for idletime info
    inline bool idletime_notify_enabled() {
      return (mapram_config_.idletime_disable_notification() == 0);
    }
    inline int idletime_bus() {
      return get_idle_bus();
    }
    inline int idletime_bitwidth() {
      return mapram_config_.idletime_bitwidth() & 0x3;
    }
    inline bool is_valid_idletime_mode() {
      return (((MauDefs::valid_idletime_modes >> idletime_bitwidth()) & 1u) == 1u);
    }
    inline int idletime_type() { // 0,1,2,3
      return idletime_bitwidth();
    }
    inline int idletime_width() { // 1,2,3,6
      switch (idletime_bitwidth()) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 3: return 6;
        default: RMT_ASSERT(0);
      }
    }
    inline int idletime_nentries() { // 8,4,2,1
      switch (idletime_bitwidth()) {
        case 0: return 8;
        case 1: return 4;
        case 2: return 2;
        case 3: return 1;
        default: RMT_ASSERT(0);
      }
    }
    inline int idletime_offset(int subword) {
      RMT_ASSERT((subword >= 0) && (subword < idletime_nentries()));
      return subword * idletime_width();
    }
    inline int idletime_dump_width() { // NA,2,4,6
      switch (idletime_bitwidth()) {
        case 1: return 2;
        case 2: return 4;
        case 3: return 6;
        default: RMT_ASSERT(0);
      }
    }
    inline int idletime_dump_offset(int subword) {
      RMT_ASSERT((subword >= 0) && (subword < idletime_nentries()));
      return subword * idletime_dump_width();
    }
    inline bool idletime_2way() {
      return (mapram_config_.two_way_idletime_notification() == 1);
    }
    inline bool idletime_perflow() {
      return (mapram_config_.per_flow_idletime() == 1);
    }
    inline uint8_t idletime_rd_clear_val() {
      return idletime_cfg_rd_clear_val_.idletime_cfg_rd_clear_val();
    }

    enum MapramColorBusDrive { kMapramNoColorBus, kMapramColorBus, kMapramOverflowColorBus };
    MapramColorBusDrive color_bus_drive() {
      switch ( mapram_config_.mapram_color_bus_select() ) {
        case 0: return kMapramNoColorBus;
        case 1: return kMapramColorBus;
        case 2: return kMapramOverflowColorBus;
        default:
          RMT_ASSERT(0);
          return kMapramNoColorBus;
      }
    }

    enum MapramRAdrMuxSelect { kMapramRAdrNone, kMapramRAdrColor, kMapramRAdrSMOflo, kMapramRAdrInvalid };
    MapramRAdrMuxSelect mapram_radr_mux_select() {
      // one hot mux, so check for invalid / not driven cases too
      if ( ram_address_mux_ctl_.map_ram_radr_mux_select_color() ) {
        if ( ram_address_mux_ctl_.map_ram_radr_mux_select_smoflo() )
          return kMapramRAdrInvalid;
        else
          return kMapramRAdrColor;
      }
      else {
        if ( ram_address_mux_ctl_.map_ram_radr_mux_select_smoflo() )
          return kMapramRAdrSMOflo;
        else
          return kMapramRAdrNone;
      }
    }

    enum RamOfoStatsMuxSelect { kRamOfoStatsMuxNone, kRamOfoStatsMuxOflo, kRamOfoStatsMuxStatsMeter,
                                kRamOfoStatsMuxInvalid };
    RamOfoStatsMuxSelect ram_ofo_stats_mux_select() {
      // one hot mux, so check for invalid / not driven cases too
      if ( ram_address_mux_ctl_.ram_ofo_stats_mux_select_oflo() ) {
        if ( ram_address_mux_ctl_.ram_ofo_stats_mux_select_statsmeter() )
          return kRamOfoStatsMuxInvalid;
        else
          return kRamOfoStatsMuxOflo;
      }
      else {
        if ( ram_address_mux_ctl_.ram_ofo_stats_mux_select_statsmeter() )
          return kRamOfoStatsMuxStatsMeter;
        else
          return kRamOfoStatsMuxNone;
      }
    }

    // TODO: diagram shows a Sys input, how is this selected?
    enum RamStatsMeterAdrMuxSelect { kRamStatsMeterAdrMuxNone, kRamStatsMeterAdrMuxStats,
                                     kRamStatsMeterAdrMuxMeter, kRamStatsMeterAdrMuxIdlet,
                                     kRamStatsMeterAdrMuxInvalid };
    RamStatsMeterAdrMuxSelect ram_stats_meter_adr_mux_select() {
      if ( (!ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_stats()) &&
           (!ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_meter()) &&
           (!ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_idlet()) ) {
        return kRamStatsMeterAdrMuxNone;
      }
      else if ( ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_stats() &&
           (!ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_meter()) &&
           (!ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_idlet()) ) {
        return kRamStatsMeterAdrMuxStats;
      }
      else if ( ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_meter() &&
           (!ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_stats()) &&
           (!ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_idlet()) ) {
        return kRamStatsMeterAdrMuxMeter;
      }
      else if ( ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_idlet() &&
           (!ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_stats()) &&
           (!ram_address_mux_ctl_.ram_stats_meter_adr_mux_select_meter()) ) {
        return kRamStatsMeterAdrMuxIdlet;
      }
      else {
        return kRamStatsMeterAdrMuxInvalid;
      }
    }

    // Is the mapram configured for parity or ECC
    inline bool has_parity() {
      return ((mapram_config_.mapram_parity_generate() == 1) ||
              (mapram_config_.mapram_parity_check() == 1));
    }
    inline bool has_ecc() {
      return ((mapram_config_.mapram_ecc_generate() == 1) ||
              (mapram_config_.mapram_ecc_check() == 1));
    }

    // GENERIC info
    inline int vpn_limit() { return kMapramVpnMax; } // Was mapram_ctl_.mapram_vpn_limit();


    bool is_ingress();
    bool is_egress();
    bool is_active();
    bool check_ingress_egress(bool ingress);
    int get_idle_bus();
    bool is_synth2port_mapram();
    bool is_synth2port_home_mapram();
    bool is_synth2port_oflo_mapram();
    bool is_statistics_mapram();
    bool is_meter_mapram();
    bool is_stateful_mapram();
    bool is_idletime_mapram();
    bool is_color_mapram();
    bool is_color_home_mapram();
    bool is_color_oflo_mapram();
    bool is_selector_mapram();
    uint32_t idletime_addr();
    uint32_t color_read_addr(uint8_t* addrtype = nullptr);
    uint32_t color_write_addr();
    uint8_t color_write_data();
    bool vpn_match(int vpn);
    int get_logical_table();
    int get_type();
    int get_vpn();
    int get_vpn_min();
    int get_vpn_max();
    uint8_t get_order();
    void set_order(uint8_t order);
    uint32_t get_priority();
    int get_mapram_type_check();
    int get_mapram_type();
    int get_alu_type();
    int get_sram_alu_index();
    int get_color_alu_index();
    int get_alu_index();
    int get_meter_alu_index();


 private:
    void notify_sram_row();
    void maybe_notify_sweeper();
    void maybe_check_valid_idletime();
    void check_parity_ecc();
    void config_change_callback();
    void idletime_xbar_write_callback();


 private:
    MauMapram                               *mau_mapram_;
    MauAddrDist                             *mau_addr_dist_;
    int                                      row_index_;
    int                                      col_index_;
    int                                      type_;
    int                                      bus_;
    uint8_t                                  order_;
    register_classes::MapramConfig               mapram_config_;
    //register_classes::MapramCtl                mapram_ctl_;
    register_classes::RamAddressMuxCtl           ram_address_mux_ctl_;
    register_classes::AdrDistIdletimeAdrXbarCtl  adr_dist_idletime_adr_xbar_ctl_;
    register_classes::IdletimeCfgRdClearVal      idletime_cfg_rd_clear_val_;

  };
}
#endif // _SHARED_MAU_MAPRAM_REG_
