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
#include <rmt-sweeper.h>
#include <mau-teop.h>
#include <mau-addr-dist.h>
#include <register_adapters.h>



  extern bool g_disable_meter_sweep;

namespace MODEL_CHIP_NAMESPACE {

  void AddrClaim::complain(int lr, int lc, int prio, Mau *maulog) {
    RMT_LOG_OBJ(maulog, RmtDebug::error(RmtDebug::kRmtDebugMauAddrDistClaim),
                "AddrClaim:: Identical priority(%d) address claim - (%d,%d) and (%d,%d)\n",
                prio, logrow_, logcol_, lr, lc);
  }
  bool AddrClaim::claim(int lr, int lc, int prio, Mau *maulog) {
    RMT_ASSERT((lr >= 0) && (lr < kLogicalRows));
    RMT_ASSERT((lc >= 0) && (lc < kLogicalColumns));
    RMT_ASSERT((prio >= -128) && (prio <= 127));
    RMT_LOG_OBJ(maulog, RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistClaim),
                "AddrClaim:: Claimants=%d CurrWinClaim=(%d,%d,%d)  NewTryClaim=(%d,%d,%d)\n",
                claimants_, logrow_, logcol_, prio_, lr, lc, prio);
    // keep track of all the rams that claim
    claimants_bitmap_[lr] |= 1<<lc;
    // Exit if identical logrow/logcol irrespective of prio - this
    // lets us check if claim succeeded without resynthesizing prio
    if ((claimants_ > 0) && (lr == logrow_) && (lc == logcol_)) return true;
    // Min prio claim never wins - can only ever be used to check if already won
    if (prio == -128) return false;
    bool winner = false;
    if ((claimants_ == 0) || (prio > prio_)) {
      // Higher prio always wins - also reset claimant count
      claimants_ = 0;
      winner = true;
    } else if (prio == prio_) {
      // Complain if we see duplicate *positive* priorities
      // Allows multiple claimers without complaint if prio <= 0
      if (prio > 0) complain(lr, lc, prio, maulog);
      // If matching pri, only replace if bigger logical row
      // or same logical row but bigger logical column
      winner = ((lr > logrow_) || ((lr == logrow_) && (lc > logcol_)));
    }
    claimants_++;
    if (winner) {
      logrow_ = static_cast<int8_t>(lr);
      logcol_ = static_cast<int8_t>(lc);
      prio_ = static_cast<int8_t>(prio);
    }
    return winner;
  }


  MauAddrDist::MauAddrDist(int chipIndex, int pipeIndex, int mauIndex, Mau *mau)
      : mau_(mau), sweeper_(NULL), mau_stateful_counters_(NULL), mau_teop_(NULL),
        mau_chip_addr_dist_(chipIndex,pipeIndex,mauIndex,mau,this),
        adr_dist_action_icxbar_(default_adapter(adr_dist_action_icxbar_,chipIndex,pipeIndex,mauIndex)),
        adr_dist_stats_icxbar_(default_adapter(adr_dist_stats_icxbar_,chipIndex,pipeIndex,mauIndex)),
        adr_dist_meter_icxbar_(default_adapter(adr_dist_meter_icxbar_,chipIndex,pipeIndex,mauIndex,
                               [this](uint32_t i){this->meter_icxbar_change_callback(i);})),
        adr_dist_idletime_oxbar_(default_adapter(adr_dist_idletime_oxbar_,chipIndex,pipeIndex,mauIndex)),
        packet_action_at_hdrtime_(default_adapter(packet_action_at_hdrtime_,chipIndex,pipeIndex,mauIndex)),
        // These 2 now in mau-chip-addr-dist.h
        //deferred_ram_ctl_(default_adapter(deferred_ram_ctl_,chipIndex,pipeIndex,mauIndex)),
        //deferred_oflo_ctl_(default_adapter(deferred_oflo_ctl_,chipIndex,pipeIndex,mauIndex)),
        meter_sweep_ctl_(default_adapter(meter_sweep_ctl_,chipIndex,pipeIndex,mauIndex,
                         [this](uint32_t i){this->meter_sweep_change_callback(i);})),
        idletime_sweep_ctl_(default_adapter(idletime_sweep_ctl_,chipIndex,pipeIndex,mauIndex,
                            [this](uint32_t i){this->idletime_sweep_change_callback(i);})),
        stats_lrt_fsm_sweep_size_(default_adapter(stats_lrt_fsm_sweep_size_,chipIndex,pipeIndex,mauIndex)),
        idletime_dump_ctl_(default_adapter(idletime_dump_ctl_,chipIndex,pipeIndex,mauIndex,
                           [this](uint32_t i){this->idletime_dump_change_callback(i);})),
        stats_dump_ctl_(default_adapter(stats_dump_ctl_,chipIndex,pipeIndex,mauIndex,
                        [this](uint32_t i){this->stats_dump_change_callback(i);})),
        oflo_adr_user_(default_adapter(oflo_adr_user_,chipIndex,pipeIndex,mauIndex)),
        stats_alu_lt_map_(default_adapter(stats_alu_lt_map_,chipIndex,pipeIndex,mauIndex)),
        intr_status_mau_ad_(default_adapter(intr_status_mau_ad_,chipIndex,pipeIndex,mauIndex,nullptr,
                            [this](){this->intr_status_mau_ad_read_callback(); })),
        //cfg_has_idle_(chipIndex,pipeIndex,mauIndex),
        //cfg_has_stats_(chipIndex,pipeIndex,mauIndex),
        //cfg_stats_are_direct_(chipIndex,pipeIndex,mauIndex),
        //cfg_meter_are_direct_(chipIndex,pipeIndex,mauIndex),
        oflow_addrtype_(0), oflow2_up_addrtype_(0), oflow2_down_addrtype_(0),
        oflow_addr_(0u), oflow2_up_addr_(0u), oflow2_down_addr_(0u),
        curr_seq_(0), pending_seq_(1), eop_map_(),
        stats_deferred_ram_rewrite_info_(), meter_deferred_ram_rewrite_info_(),
        idle_sweep_info_(), meter_sweep_info_(), meter_sweep_time_info_(),
        color_write_info_() {

    static_assert( (kLogicalRows <= 32), "AddrMask must fit in uint32_t");
    static_assert( (kLogicalColumns < 128), "Column must fit in int8_t");
    static_assert( (kIdletimeBuses <= 32), "Idletime buses must fit in uint32_t");
    static_assert( (kIdletimeBuses == kIdletimeEntries),
                   "nIdleBuses must match nIdleEntries");
    reset();
  }
  MauAddrDist::~MauAddrDist() {
    eop_map_.clear();
  }


  void MauAddrDist::reset() {
    for (int lt = 0; lt < kLogicalTables; lt++) {
      idle_sweep_info_[lt].reset(1); idle_sweep_info_[lt].set_logical_table(lt);
      idle_dump_info_[lt].reset(2);  idle_dump_info_[lt].set_logical_table(lt);
      stats_dump_info_[lt].reset(3); stats_dump_info_[lt].set_logical_table(lt);
    }
    for (int ma = 0; ma < kNumMeterAlus; ma++) {
      meter_sweep_info_[ma].reset(4); meter_sweep_info_[ma].set_alu(ma);
    }
    adr_dist_action_icxbar_.reset();
    adr_dist_stats_icxbar_.reset();
    adr_dist_meter_icxbar_.reset();
    adr_dist_idletime_oxbar_.reset();
    packet_action_at_hdrtime_.reset();
    // These 2 now in mau-chip-addr-dist.h
    //deferred_ram_ctl_.reset();
    //deferred_oflo_ctl_.reset();
    meter_sweep_ctl_.reset();
    idletime_sweep_ctl_.reset();
    stats_lrt_fsm_sweep_size_.reset();
    idletime_dump_ctl_.reset();
    stats_dump_ctl_.reset();
    oflo_adr_user_.reset();
    stats_alu_lt_map_.reset();
    intr_status_mau_ad_.reset();
    //cfg_has_idle_.reset();
    //cfg_has_stats_.reset();
    //cfg_stats_are_direct_.reset();
    //cfg_meter_are_direct_.reset();
    sweeper_ = mau_->get_object_manager()->sweeper_get();
    mau_stateful_counters_ = mau_->mau_stateful_counters();
    mau_teop_ = mau_->mau_teop();
    RMT_ASSERT(sweeper_ != NULL);
    RMT_ASSERT(mau_stateful_counters_ != NULL);
    RMT_ASSERT(mau_teop_ != NULL);
    reset_resources();
  }

  void MauAddrDist::intr_status_mau_ad_read_callback() {
    uint8_t mask = 0xFF >> (8-kNumMeterAlus);
    uint8_t log_done = intr_status_mau_ad_.stateful_log_done() & mask;
    for (int alu = 0; alu < kNumMeterAlus; alu++) {
      bool full = mau_stateful_counters_->stateful_counter_at_max(alu);
      if (full) log_done |= (1<<alu); else log_done &= ~(1<<alu);
    }
    intr_status_mau_ad_.stateful_log_done(log_done);
  }

  void MauAddrDist::reset_resources() {
    // Initialize addresses and address types
    oflow_addr_ = 0u;        oflow_addrtype_ = AddrType::kNone;
    oflow2_up_addr_ = 0u;    oflow2_up_addrtype_ = AddrType::kNone;
    oflow2_down_addr_ = 0u;  oflow2_down_addrtype_ = AddrType::kNone;
    for (int i = 0; i < kLogicalRows; i++) {
      action_addrs_[i] = 0u;
      stats_addrs_[i] = 0u;
      meter_addrs_[i] = 0u;
    }
    for (int j = 0; j < kIdletimeBuses; j++) {
      idletime_addrs_[j] = 0u;
    }
    // Set all addrtypes on all rows/buses as consumed
    action_addr_not_consumed_ = 0u;
    stats_addr_not_consumed_ = 0u;
    meter_addr_not_consumed_ = 0u;
    idletime_addr_not_consumed_ = 0u;

    // Reset src logical tables for all oflow/row buses
    oflow_src_logtab_ = 0xFF;
    oflow2_up_src_logtab_ = 0xFF;
    oflow2_down_src_logtab_ = 0xFF;
    for (int i = 0; i < kLogicalRows; i++) {
      action_src_logtabs_[i] = 0xFF;
      stats_src_logtabs_[i] = 0xFF;
      meter_src_logtabs_[i] = 0xFF;
    }
    for (int i = 0; i < kIdletimeBuses; i++) {
      idle_src_logtabs_[i] = 0xFF;
    }
    // Reset mask of ingress/egress tabs
    ingress_logtabs_ = 0u;
    egress_logtabs_ = 0u;
    // Reset mask of LTs per-row
    for (int i = 0; i < kLogicalRows; i++) {
      action_src_logtabmasks_[i] = 0u;
      stats_src_logtabmasks_[i] = 0u;
      meter_src_logtabmasks_[i] = 0u;
    }
    // Reset claims for all tables
    for (auto &ac : stats_addrclaims_) ac.reset();
    for (auto &ac : meter_addrclaims_) ac.reset();

    // Reset pre_pfe address arrays etc
    for (int i = 0; i < kLogicalRows; i++) {
      pre_pfe_en_action_addrs_[i] = 0u;
      pre_pfe_en_stats_addrs_[i] = 0u;
      pre_pfe_en_meter_addrs_[i] = 0u;
      pre_pfe_not_en_action_addrs_[i] = 0u;
      pre_pfe_not_en_stats_addrs_[i] = 0u;
      pre_pfe_not_en_meter_addrs_[i] = 0u;
    }
    for (int j = 0; j < kIdletimeBuses; j++) {
      pre_pfe_en_idletime_addrs_[j] = 0u;
      pre_pfe_not_en_idletime_addrs_[j] = 0u;
    }
    for (int i = 0; i < kLogicalRows; i++) {
      pre_pfe_action_src_logtabs_[i] = 0xFF;
      pre_pfe_stats_src_logtabs_[i] = 0xFF;
      pre_pfe_meter_src_logtabs_[i] = 0xFF;
    }
    for (int i = 0; i < kIdletimeBuses; i++) {
      pre_pfe_idle_src_logtabs_[i] = 0xFF;
    }
  }


  // Check addresses pre-PFE examination
  void MauAddrDist::pre_pfe_checks(std::array<uint32_t,kLogicalRows> *en_addr_array,
                                   std::array<uint32_t,kLogicalRows> *not_en_addr_array,
                                   std::array<uint8_t,kLogicalRows> *srctab_array,
                                   uint32_t adistMask, int lt, bool addr_en,
                                   uint32_t addr, uint8_t addr_type, const std::string& bus_name) {
    uint32_t logrows = adistMask & kAdistMaskHomeRows;
    for (int i = 0; i < kLogicalRows; i++) {
      if ((logrows & (1<<i)) != 0) {
        std::string bus_name_index = bus_name + "[" + std::to_string(i) + "]";
        if (addr_en) en_addr_array->at(i) |= addr; else not_en_addr_array->at(i) |= addr;
        pre_pfe_addr_en_noten_check(bus_name_index, en_addr_array->at(i), not_en_addr_array->at(i));
        // Row i driven from table lt - maintain first seen
        if (srctab_array->at(i) == 0xFF) srctab_array->at(i) = lt;
      }
    }
  }
  void MauAddrDist::pre_pfe_checks_action(int logical_table, bool ingress, uint32_t addr) {
    bool action_en = Address::action_addr_enabled(addr);
    uint32_t action_rows = get_adist_mask(&adr_dist_action_icxbar_, logical_table);
    pre_pfe_checks(&pre_pfe_en_action_addrs_, &pre_pfe_not_en_action_addrs_,
                   &pre_pfe_action_src_logtabs_, action_rows, logical_table, action_en,
                   addr, AddrType::kAction, "pre_pfe_action_addr");
  }
  void MauAddrDist::pre_pfe_checks_stats(int logical_table, bool ingress, uint32_t addr) {
    bool stats_en = Address::stats_addr_enabled(addr);
    uint32_t stats_rows = calculate_stats_rows(logical_table,ingress,0,When::kHdrTime);
    pre_pfe_checks(&pre_pfe_en_stats_addrs_, &pre_pfe_not_en_stats_addrs_,
                   &pre_pfe_stats_src_logtabs_, stats_rows, logical_table, stats_en,
                   addr, AddrType::kStats, "pre_pfe_stats_addr");
  }
  void MauAddrDist::pre_pfe_checks_meter(int logical_table, bool ingress, uint32_t addr) {
    // PFE checked *before* icxbar for Meter addresses on JBay so don't do this
    if (RmtObject::is_jbay_or_later()) return;
    bool meter_en = Address::meter_addr_enabled(addr);
    uint32_t meter_rows = calculate_meter_rows(logical_table,ingress,0,When::kHdrTime);
    pre_pfe_checks(&pre_pfe_en_meter_addrs_, &pre_pfe_not_en_meter_addrs_,
                   &pre_pfe_meter_src_logtabs_, meter_rows, logical_table, meter_en,
                   addr, AddrType::kMeter, "pre_pfe_meter_addr");
  }
  void MauAddrDist::pre_pfe_checks_idle(int logical_table, bool ingress, uint32_t addr) {
    //bool idle_en = Address::idletime_addr_enabled(addr);
  }


  // Check address within range for logical_row
  bool MauAddrDist::vpn_range_check(int logical_row, uint32_t addr, uint8_t addr_type,
                                    bool quiet) {
    bool ok1 = false;
    int vpn = -1;
    switch (addr_type) {
      case AddrType::kStats:
        if (!Address::stats_addr_op_enabled(addr)) return false;
        ok1 = vpn_range_check_stats(logical_row, addr);
        vpn = Address::stats_addr_get_vpn(addr);
        break;
      case AddrType::kMeter:
        if (!Address::meter_addr_op_enabled(addr)) return false;
        ok1 = vpn_range_check_meter(logical_row, addr);
        vpn = Address::meter_addr_get_vpn(addr);
        break;
      default:
        return true; // Bail if not checking this addr_type
    }

    // Check valid VPN for logical row
    //  So 1. within stats/meter vpn_range (checks on JBay, always ok on Tofino)
    // AND 2. within synth2port base/limit
    bool ok2 = mau_->logical_row_lookup(logical_row)->synth2port_vpn_valid(vpn);
    if (ok1 && ok2) return true;

    // Maybe bail saying nothing
    if (quiet) return false;

    // Range check failed - spit out an error/warning
    int sm_idx = 0, idx = -1;
    switch (addr_type) {
      case AddrType::kStats: idx = Address::stats_addr_get_index(addr); sm_idx = 2; break;
      case AddrType::kMeter: idx = Address::meter_addr_get_index(addr); sm_idx = 4; break;
      default: RMT_ASSERT(0); break;
    }
    int errstridx = ((ok1) ?0 :sm_idx) | ((ok2) ?0 :1); // Should be 1,2,3,4 or 5
    const char *errstrs[8] = { "", "Synth2Port", "StatsAluVpnRange", "Synth2Port+StatsAluVpnRange",
                              "MeterAluVpnRange", "Synth2Port+MeterAluVpnRange", "?!?", "!?!" };
    const char *addrstr = (sm_idx == 2) ?"STATS" :"METER";

    // XXX: allow 0x3FF abuse on JBay too
    bool quiet2  = ((RmtObject::is_tofinoXX() || RmtObject::is_jbayXX()) && (idx == 1023)); // 0x3FF sometimes (ab)used
    if (!quiet2) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(MauSram::kRelaxSramVpnCheck),
                  "MauAddrDist::vpn_range_check: Logrow=%d AddrType=%s,Addr=0x%x with "
                  "VPN=%d,index=%d NOT within %s Base/Limit - DISCARDING.\n",
                  logical_row, addrstr, addr, vpn, idx, errstrs[errstridx]);
    }
    return false;
  }
  uint32_t MauAddrDist::vpn_range_check_rows(uint32_t rows, uint32_t addr, uint8_t addr_type,
                                             bool quiet) {
    int      s_or_m = -1;
    uint32_t outrows = rows;
    switch (addr_type) {
      case AddrType::kStats: outrows &= kAdistMaskValidStatsRows; s_or_m = 0; break;
      case AddrType::kMeter: outrows &= kAdistMaskValidMeterRows; s_or_m = 1; break;
    }
    if (outrows == 0u) return 0u;
    for (int i = 0; i < kLogicalRows; i++) {
      if (((outrows >> i) & 1) == 1) {
        // Remove row if fails range check
        if (!vpn_range_check(i, addr, addr_type, quiet)) outrows &= ~(1u<<i);
      }
    }
    // If we were using oflow in the input rows maybe splice back into the output rows
    // Note param2 ingress is IGNORED in the call to deferred_ram_oflow so just pass true
    if ((rows & kAdistMaskOflow) != 0u) outrows = deferred_ram_oflow(outrows, true, s_or_m);
    return outrows;
  }
  uint32_t MauAddrDist::vpn_range_check_rows2(uint32_t rows, uint32_t addr, uint8_t addr_type) {
    if (rows == 0u) return 0u;
    // Check rows quietly - don't mind some rows rejecting address as long as one can accept
    uint32_t outrows = vpn_range_check_rows(rows, addr, addr_type, true);
    if (outrows != 0u) return outrows;
    // If we get here NO rows could accept address so check again noisily
    return vpn_range_check_rows(rows, addr, addr_type, false);
  }


  // Distribute address from LT to multiple rows
  void MauAddrDist::distribute_to_rows(std::array<uint32_t,kLogicalRows> *addr_array,
                                       std::array<uint8_t,kLogicalRows> *srctab_array,
                                       std::array<uint32_t,kLogicalRows> *srctabmask_array,
                                       uint32_t adistMask, int lt,
                                       uint32_t addr, uint8_t addr_type,
                                       const std::string& bus_name,
                                       bool overwrite_addr /*=false*/) {
    uint32_t logrows = adistMask & kAdistMaskHomeRows;
    for (int i = 0; i < kLogicalRows; i++) {
      if ((logrows & (1<<i)) != 0) {
        std::string bus_name_index = bus_name + "[" + std::to_string(i) + "]";
        if (overwrite_addr) addr_array->at(i) = 0u;
        addr_dist_debug(bus_name, lt, addr_array->at(i), addr);
        addr_multi_write_check(bus_name_index, addr_array->at(i), addr,
                               srctab_array->at(i), lt);
        addr_array->at(i) |= addr;
        srctabmask_array->at(i) |= lt;
        // Row i driven from table lt - maintain first seen
        if (srctab_array->at(i) == 0xFF) srctab_array->at(i) = lt;
      }
    }
  }

  // Remember whether logical table was used for ingress or egress
  void MauAddrDist::logical_table_track_ingress_egress(int logical_table, bool ingress) {
    if ((logical_table >= 0) && (logical_table < kLogicalTables)) {
      if (ingress) {
        RMT_ASSERT((egress_logtabs_ & (1u<<logical_table)) == 0u);
        ingress_logtabs_ |= (1u<<logical_table);
      } else {
        RMT_ASSERT((ingress_logtabs_ & (1u<<logical_table)) == 0u);
        egress_logtabs_ |= (1u<<logical_table);
      }
    }
  }


  // Do actual distribution - could be called EITHER at headertime OR at EOP
  void MauAddrDist::distrib_stats_addresses(int logical_table, bool ingress,
                                            uint32_t stats_addr, uint32_t stats_rows,
                                            bool overwrite_addr /*=false*/) {

    bool enabled_addr = Address::stats_addr_op_enabled(stats_addr);

    logical_table_track_ingress_egress(logical_table, ingress);
    if ((enabled_addr || overwrite_addr) && (stats_rows != 0u)) {
      distribute_to_rows(&stats_addrs_,
                         &stats_src_logtabs_, &stats_src_logtabmasks_,
                         stats_rows, logical_table,
                         stats_addr, AddrType::kStats,
                         "stats_addr", overwrite_addr);
      if (distribute_to_oflow(stats_rows, logical_table,
                              stats_addr, AddrType::kStats)) {
        if (!Address::stats_addr_op_cfg(stats_addr))
          mau_->mau_info_incr(MAU_OFLO_ADDRS_DISTRIBUTED);
      }
      if (enabled_addr)
        stats_addr_not_consumed_ |= stats_rows; // For sanity checking
      else if (overwrite_addr)
         stats_addr_not_consumed_ &= ~stats_rows;
    }
  }

  // Do actual distribution - could be called EITHER at headertime OR at EOP
  void MauAddrDist::distrib_meter_addresses(int logical_table, bool ingress,
                                            uint32_t meter_addr, uint32_t meter_rows,
                                            bool overwrite_addr /*=false*/) {
    bool enabled_addr = Address::meter_addr_op_enabled(meter_addr);

    logical_table_track_ingress_egress(logical_table, ingress);
    if ((enabled_addr || overwrite_addr) && (meter_rows != 0u)) {
      distribute_to_rows(&meter_addrs_,
                         &meter_src_logtabs_, &meter_src_logtabmasks_,
                         meter_rows, logical_table,
                         meter_addr, AddrType::kMeter, "meter_addr", overwrite_addr);
      if (distribute_to_oflow(meter_rows, logical_table,
                              meter_addr, AddrType::kMeter)) {
        if (!Address::meter_addr_op_cfg(meter_addr))
          mau_->mau_info_incr(MAU_OFLO_ADDRS_DISTRIBUTED);
      }
      if (enabled_addr)
        meter_addr_not_consumed_ |= meter_rows; // For sanity checking
      else if (overwrite_addr)
        meter_addr_not_consumed_ &= ~meter_rows;
    }
  }

  // ONLY ever run at headertime
  void MauAddrDist::distrib_action_addresses(int logical_table, bool ingress,
                                             uint32_t action_addr) {
    logical_table_track_ingress_egress(logical_table, ingress);

    if (Address::action_addr_enabled(action_addr)) {
      uint32_t act_rows = get_adist_mask(&adr_dist_action_icxbar_, logical_table);
      distribute_to_rows(&action_addrs_,
                         &action_src_logtabs_, &action_src_logtabmasks_,
                         act_rows, logical_table,
                         action_addr, AddrType::kAction, "action_addr");
      if (distribute_to_oflow(act_rows, logical_table,
                              action_addr, AddrType::kAction))
        mau_->mau_info_incr(MAU_OFLO_ADDRS_DISTRIBUTED);

      action_addr_not_consumed_ |= act_rows; // For sanity checking
    }
  }

  // ONLY ever run at headertime
  void MauAddrDist::distrib_idletime_addresses(int logical_table, bool ingress,
                                               uint32_t idletime_addr) {
    logical_table_track_ingress_egress(logical_table, ingress);

    if (!Address::idletime_addr_op_enabled(idletime_addr)) return;
    int op = Address::idletime_addr_op(idletime_addr);

    // Idletime addresses - oxbar so have to go through all possibilities
    for (int i = 0; i < kIdletimeBuses; i++) {
      if ((is_idletime_valid(i)) && (idletime_addr != 0u)) {
        int lt = get_idletime_entry(get_idle_oxbar_word(get_idletime_which_word(i)),
                                    get_idletime_which_entry(i));
        if (lt == logical_table) {
          if (idletime_addrs_[i] != 0u) {
            std::string bus_name_index = "idletime_addr[" + std::to_string(i) + "]";
            addr_multi_write_check(bus_name_index, idletime_addrs_[i], idletime_addr,
                                   idle_src_logtabs_[i], logical_table);
          }
          idletime_addrs_[i] |= idletime_addr;
          if (idle_src_logtabs_[i] == 0xFF) idle_src_logtabs_[i] = logical_table;

          // If this is a 'normal' address use then mark a hit
          if (op == Address::kIdletimeOpMarkActive) idle_sweep_info_[lt].idle_hit(this, lt);

	  idletime_addr_not_consumed_ |= (1u<<i); // For sanity checking
        }
      }
    }
  }

  // ONLY ever run at headertime - output addresses to Teop Bus
  void MauAddrDist::teop_output_stats_addr(Teop *teop, int lt, bool ingress,
                                           uint32_t stats_rows, uint32_t stats_addr) {
    for (int alu = 0; alu < kNumStatsAlus; alu++) {
      int row = MauStatsAlu::get_stats_alu_logrow_index(alu);
      if (((stats_rows >> row) & 1) == 1) {
        mau_teop_->teop_output_stats_addr(teop, lt, ingress, alu, stats_addr);
      }
    }
  }
  // ONLY ever run at headertime - output addresses to Teop Bus
  void MauAddrDist::teop_output_meter_addr(Teop *teop, int lt, bool ingress,
                                           uint32_t meter_rows, uint32_t meter_addr) {
    for (int alu = 0; alu < kNumMeterAlus; alu++) {
      int row = MauMeterAlu::get_meter_alu_logrow_index(alu);
      if (((meter_rows >> row) & 1) == 1) {
        mau_teop_->teop_output_meter_addr(teop, lt, ingress, alu, meter_addr);
      }
    }
  }


  // ONLY ever run at headertime
  // Per logical-table func to distribute extracted stats addresses to HdrTime rows/oflo
  bool MauAddrDist::distrib_stats_addresses(int logical_table, bool ingress,
                                            uint32_t stats_addr, Teop *teop) {
    if (!Address::stats_addr_op_enabled(stats_addr)) return false;

    // NOW HANDLE TEOP in defer_stats_addresses
    //teop_output_stats_addr(teop, logical_table, ingress, kStatsAluLogicalRows, stats_addr);

    uint32_t stats_rows = calculate_stats_rows(logical_table, ingress, 0, When::kHdrTime);
    if (stats_rows == 0u) return false;

    // Put onto HdrTime rows
    distrib_stats_addresses(logical_table, ingress, stats_addr, stats_rows);
    RMT_LOG_OBJ(mau_,RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistStats),
                "MauAddrDist::distrib_stats_addresses "
                "stats_addr=0x%x stats_rows=0x%x\n",stats_addr,stats_rows);
    return true;
  }
  // ONLY ever run at headertime
  // Per logical-table func to distribute extracted meter addresses to HdrTime rows/oflo
  bool MauAddrDist::distrib_meter_addresses(int logical_table, bool ingress,
                                            uint32_t meter_addr, Teop *teop, uint8_t color) {
    if (!Address::meter_addr_op_enabled(meter_addr)) return false;

    // NOW HANDLE TEOP in defer_meter_addresses
    // This func gets mapped ColorAware -> SaluClr addresses which don't got on TEOP
    // HdrTime address may also get put on TeopBus
    //teop_output_meter_addr(teop, logical_table, ingress, kMeterAluLogicalRows, meter_addr);

    uint32_t meter_rows = calculate_meter_rows(logical_table, ingress, 0, When::kHdrTime);
    if (meter_rows == 0u) return false;

    // Meter color should be 0 at HdrTime - only have color at EopTime/TeopTime
    //RMT_ASSERT((color == 0) && "Color invalid at HdrTime");

    // Put onto HdrTime rows
    distrib_meter_addresses(logical_table, ingress, meter_addr, meter_rows);
    RMT_LOG_OBJ(mau_,RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistMeter),
                "MauAddrDist::distrib_meter_addresses "
                "meter_addr=0x%x meter_rows=0x%x\n", meter_addr,meter_rows);
    return true;
  }

  // ONLY ever run at headertime - stashes address for distribution at EOP
  void MauAddrDist::defer_stats_addresses(int logical_table, bool ingress,
                                          uint32_t stats_addr,
                                          Teop *teop, uint8_t eop_num) {
    if (!Address::stats_addr_op_enabled(stats_addr)) return;
    if (!Address::stats_addr_op_deferrable(stats_addr)) return;

    // Deferred HdrTime address may also get put on TeopBus
    teop_output_stats_addr(teop, logical_table, ingress, kStatsAluLogicalRows, stats_addr);

    uint32_t stats_rows = calculate_stats_rows(logical_table,ingress,eop_num,When::kEopTime);
    if (stats_rows == 0u) return;
    if (!Eop::valid_eopnum(eop_num)) return;

    uint32_t curr_eop_stats_addr = 0u, rows = 0u;
    get_eop_stats_addr(logical_table, eop_num, &curr_eop_stats_addr, &rows);
    eop_free_check("stats_eop", curr_eop_stats_addr, eop_num);
    set_eop_stats_addr(logical_table, eop_num, stats_addr, stats_rows);

  }
  // ONLY ever run at headertime - stashes address for distribution at EOP
  void MauAddrDist::defer_meter_addresses(int logical_table, bool ingress,
                                          uint32_t meter_addr,
                                          Teop *teop, uint8_t eop_num) {
    if (!Address::meter_addr_op_enabled(meter_addr)) return;
    if (!Address::meter_addr_op_deferrable(meter_addr)) return;

    // Deferred HdrTime address may also get put on TeopBus
    teop_output_meter_addr(teop, logical_table, ingress, kMeterAluLogicalRows, meter_addr);

    uint32_t meter_rows = calculate_meter_rows(logical_table,ingress,eop_num,When::kEopTime);
    if (meter_rows == 0u) return;
    if (!Eop::valid_eopnum(eop_num)) return;

    // Since regs_24744_mau_dev there is a separate ixbar for meter_color
    // Complain if mismatch and zeroise color
    //uint32_t meter_color_rows = meter_rows;
    uint32_t meter_color_rows = get_meter_color_alu_rows(logical_table);
    uint32_t meter_addr_rows = meter_rows & kAdistMaskHomeRows;

    if (meter_color_rows != meter_addr_rows) {
      meter_addr = Address::meter_color_zeroise_color(meter_addr);
      RMT_LOG_OBJ(mau_, RmtDebug::error(RmtDebug::kRmtDebugMauAddrDistMeter),
                  "MauAddrDist: Meter Color/Addr row mismatch - color set to 0 "
                  "(ColorRows=0x%08x AddrRows=0x%08x) \n",
                  meter_color_rows, meter_addr_rows);
    }
    uint32_t curr_eop_meter_addr = 0u, rows = 0u;
    get_eop_meter_addr(logical_table, eop_num, &curr_eop_meter_addr, &rows);
    eop_free_check("meter_eop", curr_eop_meter_addr, eop_num);
    set_eop_meter_addr(logical_table, eop_num, meter_addr, meter_rows);
  }


  // At end of ALL logical-table headertime processing this function is called
  void MauAddrDist::finalize_stats_addresses(MauExecuteState *state) {
    uint32_t rows_todo = 0u;
    for (int i = 0; i < kLogicalRows; i++) {
      if (stats_addrs_[i] != 0u) rows_todo |= 1u<<i;
    }
    // Go through rows finding subsets with same addresses/LTs
    for (int i = 0; i < kLogicalRows; i++) {
      if (((rows_todo >> i) & 1) == 1) {
        uint32_t rows_in = 0u;
        uint32_t addr = stats_addrs_[i];
        uint32_t lts = stats_src_logtabmasks_[i];
        int  lt = stats_src_logtabs_[i];
        bool ingress = true;
        if ((lt >= 0) && (lt < kLogicalTables))
          ingress = (((ingress_logtabs_ >> lt) & 1) == 1);
        // Find all rows with same address driven by same LT(s)
        // (or maybe just same VPN from same LT???)
        for (int ii = i; ii < kLogicalRows; ii++) {
          if ( ( ((rows_todo >> ii) & 1) == 1 ) &&
               ( stats_addrs_[ii] == addr ) &&
               ( (lts == 0u) || ((stats_src_logtabmasks_[ii] & lts) != 0u) ) )
            rows_in |= 1u<<ii;
        }
        RMT_ASSERT(rows_in != 0u);
        rows_in = deferred_ram_oflow(rows_in, ingress, 0); // Maybe add oflow
        uint32_t rows_out = vpn_range_check_rows2(rows_in, addr, AddrType::kStats);
        // Send addr to whichever row(s) passed VPN range test, 0u elsewhere
        distrib_stats_addresses(lt, ingress, 0u, rows_in & ~rows_out, true);
        distrib_stats_addresses(lt, ingress, addr, rows_out, true);
        rows_todo &= ~rows_in; // Track all rows we've processed
      }
    }
  }

  // At end of ALL logical-table headertime processing this function is called to
  // splice stateful counters into meter addresses and leave on rows for HdrTime
  //
  void MauAddrDist::finalize_meter_addresses(MauExecuteState *state) {
    // On JBay may distribute a meter_address (without any LT) if counter
    // is overflowed or underflowed or if a counter_clear is active
    // (see XXX, XXX)

    std::array< uint32_t, kLogicalRows > addrs_per_row_;
    for (int i = 0; i < kLogicalRows; i++) addrs_per_row_[i] = 0u;
    uint32_t rows_todo = 0u;
    uint64_t Ting, Tegr, T = state->get_phv_time(&Ting,&Tegr);

    for (int alu = 0; alu < kNumMeterAlus; alu++) {
      bool unconditional = mau_stateful_counters_->alu_counter_flagged(alu);
      int row = MauMeterAlu::get_meter_alu_logrow_index(alu);
      uint32_t maddr_in = meter_addrs_[row];
      if ((maddr_in != 0u) || (unconditional)) {
        // Splice stateful counters into final meter_addr
        // NOTE, the muxed counter info does NOT go to the TEOP bus or DeferredRAM
        addrs_per_row_[row] = mau_stateful_counters_->get_meter_addr(alu, maddr_in, T);
        rows_todo |= 1u<<row;
      }
    }
    // Now go through rows finding subsets with same addresses/LTs
    for (int i = 0; i < kLogicalRows; i++) {
      if (((rows_todo >> i) & 1) == 1) {
        uint32_t rows_in = 0u;
        uint32_t addr = addrs_per_row_[i];
        uint32_t lts = meter_src_logtabmasks_[i];
        int  lt = meter_src_logtabs_[i];
        bool ingress = true; // Assume ingress if no LT
        if ((lt >= 0) && (lt < kLogicalTables))
          ingress = (((ingress_logtabs_ >> lt) & 1) == 1);
        // Find all rows with same address driven by same LT(s)
        // (or maybe just same VPN from same LT???)
        for (int ii = i; ii < kLogicalRows; ii++) {
          if ( ( ((rows_todo >> ii) & 1) == 1 ) &&
               ( addrs_per_row_[ii] == addr) &&
               ( (lts == 0u) || ((meter_src_logtabmasks_[ii] & lts) != 0u) ) )
            rows_in |= 1u<<ii;
        }
        RMT_ASSERT(rows_in != 0u);
        rows_in = deferred_ram_oflow(rows_in, ingress, 1); // Maybe add oflow
        uint32_t rows_out = vpn_range_check_rows2(rows_in, addr, AddrType::kMeter);
        // Send addr to whichever row(s) passed VPN range test, 0u elsewhere
        distrib_meter_addresses(lt, ingress, 0u, rows_in & ~rows_out, true);
        distrib_meter_addresses(lt, ingress, addr, rows_out, true);
        rows_todo &= ~rows_in; // Track all rows we've processed
      }
    }
  }

  // Func to distribute addresses from EOP (deferred RAM) at EOP time
  void MauAddrDist::distrib_eoptime_addresses(MauExecuteState *state) {
    const Eop &eop = state->eop_;

    for (int lt = 0; lt < Mau::kLogicalTables; lt++) {
      MauLogicalTable *table = mau_->logical_table_lookup(lt);

      if (table != NULL) {
        bool ing = (table->is_ingress() && eop.ingress_valid());
        bool egr = (table->is_egress() && eop.egress_valid());

        if (ing || egr) {
          uint8_t eop_num = (ing) ?eop.ingress_eopnum() :eop.egress_eopnum();
          bool eop_err = (ing) ?eop.ingress_eoperr() :eop.egress_eoperr();

          for (int sm = 0; ((sm == 0) || (sm == 1)); sm++) {
            const char *smstr[2] = { "stats", "meter" };
            uint8_t     addr_type = (sm == 0) ?AddrType::kStats :AddrType::kMeter;
            uint32_t    addr = 0u, rows = 0u;

            get_eop_addr(addr_type, lt, eop_num, &addr, &rows);
            if ((addr != 0u) || (rows != 0u))
              clear_eop_addr(addr_type, lt, eop_num);

            // See if any rows are squashed
            rows = deferred_ram_err_squash(rows, ing, sm, eop_err);
            // Now range check rows
            rows = vpn_range_check_rows2(rows, addr, addr_type);
            // We re-evaluate whether to use oflo here as config
            // could have changed between SOP,EOP
            rows = deferred_ram_oflow(rows, ing, sm);

            RMT_LOG_OBJ(mau_,RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistMeter),
                        "MauAddrDist::distrib_eoptime_addresses "
                        "%s_addr=0x%x %s_rows=0x%x\n",
                        smstr[sm], addr, smstr[sm], rows);
            if (sm == 0)
              distrib_stats_addresses(lt, ing, addr, rows);
            else
              distrib_meter_addresses(lt, ing, addr, rows);
          }
        }
      }
    }
  }

  // Func to distribute addresses from TEOP bus at TEOP time
  void MauAddrDist::distrib_teoptime_addresses(MauExecuteState *state) {
    const Teop &teop = state->teop_;
    uint32_t rows_todo = 0u;

    // Process METER
    std::array< uint32_t, kNumMeterAlus > maddrs_;
    for (int alu = 0; alu < kNumMeterAlus; alu++) maddrs_[alu] = 0u;
    std::array< int, kNumMeterAlus > mlts_;
    for (int alu = 0; alu < kNumMeterAlus; alu++) mlts_[alu] = -1;

    // Harvest all addresses destined for Meter ALUs
    for (int alu = 0; alu < kNumMeterAlus; alu++) {
      int mlt = -1;
      uint32_t maddr = 0u;
      if (mau_teop_->teop_input_meter_addr(teop, alu, &maddr, &mlt)) {
        RMT_ASSERT(mlt >= 0); // Should have LT if func returns true
        maddrs_[alu] = maddr; mlts_[alu] = mlt;
        int row = MauMeterAlu::get_meter_alu_logrow_index(alu);
        rows_todo |= 1u<<row;
      }
    }
    // Now batch addresses for VPN range check
    for (int alu = 0; alu < kNumMeterAlus; alu++) {
      int row = MauMeterAlu::get_meter_alu_logrow_index(alu);
      if (((rows_todo >> row) & 1) == 1) {
        uint32_t rows_in = 0u;
        uint32_t maddr = maddrs_[alu];
        int      mlt = mlts_[alu];
        for (int alu2 = alu; alu2 < kNumMeterAlus; alu2++) {
          int row2 = MauMeterAlu::get_meter_alu_logrow_index(alu2);
          if ( ( ((rows_todo >> row2) & 1) == 1 ) &&
               ( maddrs_[alu2] == maddr) && ( mlts_[alu2] == mlt ) )
            rows_in |= 1u<<row2;
        }
        RMT_ASSERT(rows_in != 0u);
        uint32_t rows_out = vpn_range_check_rows2(rows_in, maddr, AddrType::kMeter);
        // Send addr to whichever row(s) passed VPN range test, 0u elsewhere
        distrib_meter_addresses(mlt, false, 0u, rows_in & ~rows_out, true);
        distrib_meter_addresses(mlt, false, maddr, rows_out, true);
        rows_todo &= ~rows_in; // Track all rows we've processed
      }
    }

    // Process STATS
    rows_todo = 0u;
    std::array< uint32_t, kNumStatsAlus > saddrs_;
    for (int alu = 0; alu < kNumStatsAlus; alu++) saddrs_[alu] = 0u;
    std::array< int, kNumStatsAlus > slts_;
    for (int alu = 0; alu < kNumStatsAlus; alu++) slts_[alu] = -1;

    // Harvest all addresses destined for Stats ALUs
    for (int alu = 0; alu < kNumStatsAlus; alu++) {
      int slt = -1;
      uint32_t saddr = 0u;
      if (mau_teop_->teop_input_stats_addr(teop, alu, &saddr, &slt)) {
        RMT_ASSERT(slt >= 0); // Should have LT if func returns true
        saddrs_[alu] = saddr; slts_[alu] = slt;
        int row = MauStatsAlu::get_stats_alu_logrow_index(alu);
        rows_todo |= 1u<<row;
      }
    }
    // Then batch them for VPN range check
    for (int alu = 0; alu < kNumStatsAlus; alu++) {
      int row = MauStatsAlu::get_stats_alu_logrow_index(alu);
      if (((rows_todo >> row) & 1) == 1) {
        uint32_t rows_in = 0u;
        uint32_t saddr = saddrs_[alu];
        int      slt = slts_[alu];
        for (int alu2 = alu; alu2 < kNumStatsAlus; alu2++) {
          int row2 = MauStatsAlu::get_stats_alu_logrow_index(alu2);
          if ( ( ((rows_todo >> row2) & 1) == 1 ) &&
               ( saddrs_[alu2] == saddr) && ( slts_[alu2] == slt ) )
            rows_in |= 1u<<row2;
        }
        RMT_ASSERT(rows_in != 0u);
        uint32_t rows_out = vpn_range_check_rows2(rows_in, saddr, AddrType::kStats);
        // Send addr to whichever row(s) passed VPN range test, 0u elsewhere
        distrib_stats_addresses(slt, false, 0u, rows_in & ~rows_out, true);
        distrib_stats_addresses(slt, false, saddr, rows_out, true);
        rows_todo &= ~rows_in; // Track all rows we've processed
      }
    }
  }


  // Funcs to distribute stats/meter addresses to headertime,eoptime AND teoptime rows
  // These just called by PBUS_RD, PBUS_WR and SWEEP funcs
  void MauAddrDist::distrib_stats_addresses(int logical_table, bool ingress,
                                            uint32_t stats_addr, bool allow_eop) {
    uint32_t sop_stats_rows  = calculate_stats_rows(logical_table, ingress, 0, When::kHdrTime);
    uint32_t eop_stats_rows  = calculate_stats_rows(logical_table, ingress, 0, When::kEopTime);
    uint32_t teop_stats_rows = calculate_stats_rows(logical_table, ingress, 0, When::kTeopTime);
    uint32_t all_stats_rows  = sop_stats_rows | eop_stats_rows | teop_stats_rows;
    uint32_t use_stats_rows  = (allow_eop) ?all_stats_rows :sop_stats_rows;
    uint32_t checked_rows = vpn_range_check_rows2(use_stats_rows, stats_addr, AddrType::kStats);
    distrib_stats_addresses(logical_table, ingress, stats_addr, checked_rows);
  }
  void MauAddrDist::distrib_meter_addresses(int logical_table, bool ingress,
                                            uint32_t meter_addr, bool allow_eop) {
    uint32_t sop_meter_rows  = calculate_meter_rows(logical_table, ingress, 0, When::kHdrTime);
    uint32_t eop_meter_rows  = calculate_meter_rows(logical_table, ingress, 0, When::kEopTime);
    uint32_t teop_meter_rows = calculate_meter_rows(logical_table, ingress, 0, When::kTeopTime);
    uint32_t all_meter_rows  = sop_meter_rows | eop_meter_rows | teop_meter_rows;
    uint32_t use_meter_rows  = (allow_eop) ?all_meter_rows :sop_meter_rows;
    uint32_t checked_rows = vpn_range_check_rows2(use_meter_rows, meter_addr, AddrType::kMeter);
    distrib_meter_addresses(logical_table, ingress, meter_addr, checked_rows);
  }





  // Figure out what rows are enabled for HdrTime/EopTime/TeopTime stats/meter addresses
  void MauAddrDist::get_row_usage(uint32_t rows, bool ingress, int s_or_m,
                                  uint32_t *hdr_rows, uint32_t *eop_rows, uint32_t *teop_rows) {
    RMT_ASSERT((hdr_rows != NULL) || (eop_rows != NULL) || (teop_rows != NULL));
    uint32_t validrows = (s_or_m == 0) ?kAdistMaskValidStatsRows :kAdistMaskValidMeterRows;
    uint32_t inrows = (rows & kAdistMaskHomeRows); // Ignore any overflow
    uint32_t hdr_outrows = 0u, eop_outrows = 0u, teop_outrows = 0u;
    int arr_pos = 0; // Each valid homerow has an array element configuring it
    // Go through all valid homerows (not overflow)
    for (int i = 0; i < kLogicalRows; i++) {
      if ((validrows & (1u<<i)) != 0u) {
        if ((inrows & (1u<<i)) != 0u) {
          bool ing = ((get_deferred_ram_thread(s_or_m, arr_pos) & 1) == 0);
          bool dr_en = (get_deferred_ram_en(s_or_m, arr_pos) != 0);
          bool paah_en = (packet_action_at_hdrtime_.packet_action_at_headertime(s_or_m,arr_pos) == 1);
          bool teop_en = mau_teop_->teop_enabled(s_or_m, arr_pos);
          // Is row enabled for deferred_ram for correct thread (0=>ingress), for Teop or for HdrTime
          if ((dr_en) && (ingress == ing)) {
            eop_outrows |= (1u<<i);
            // If using deferred_ram_row MUST have BOTH paah/teop disabled for that ALU
            if (!kRelaxPacketActionAtHdrCheck) RMT_ASSERT(!paah_en && !teop_en);
          } else if (teop_en) {
            teop_outrows |= (1u<<i);
            // If using TEOP row MUST have BOTH paah/deferred_ram disabled for that ALU
            if (!kRelaxPacketActionAtHdrCheck) RMT_ASSERT(!paah_en && !dr_en);
          } else if (paah_en) {
            hdr_outrows |= (1u<<i);
            // If using HDR row MUST have BOTH deferred_ram/teop disabled for that ALU
            if (!kRelaxPacketActionAtHdrCheck) RMT_ASSERT(!dr_en && !teop_en);
          }
        }
        arr_pos++;
      }
    }
    RMT_LOG_OBJ(mau_, RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistEop),
                "MauAddrDist::get_row_usage(rows=0x%x,ie=%d,sm=%d) Hdr=0x%x Eop=0x%x Teop=0x%x\n",
                rows, ingress, s_or_m, hdr_outrows, eop_outrows, teop_outrows);
    if (hdr_rows != NULL)  *hdr_rows  = hdr_outrows;
    if (eop_rows != NULL)  *eop_rows  = eop_outrows;
    if (teop_rows != NULL) *teop_rows = teop_outrows;
  }
  // Figure out what rows are enabled for deferral of stats/meter addresses
  uint32_t MauAddrDist::deferred_ram_rows(uint32_t rows, bool ingress, int s_or_m) {
    uint32_t eop_rows = 0u;
    get_row_usage(rows, ingress, s_or_m, NULL, &eop_rows, NULL);
    return eop_rows;
  }
  // Figure out if any rows are squashed at EOP in presence/absence of error
  uint32_t MauAddrDist::deferred_ram_err_squash(uint32_t rows, bool ingress, int s_or_m, bool err) {
    uint32_t validrows = (s_or_m == 0) ?kAdistMaskValidStatsRows :kAdistMaskValidMeterRows;
    uint32_t inrows = (rows & kAdistMaskHomeRows); // Ignore any overflow
    uint32_t outrows = 0u;
    int arr_pos = 0; // Each valid homerow has an array element configuring it
    // Go through all valid homerows (not overflow)
    for (int i = 0; i < kLogicalRows; i++) {
      if ((validrows & (1u<<i)) != 0u) {
        if ((inrows & (1u<<i)) != 0u) {
          // Is row enabled in deferred_ram and for correct thread (0=>ingress)
          bool en = (get_deferred_ram_en(s_or_m, arr_pos) != 0);
          bool ing = ((get_deferred_ram_thread(s_or_m, arr_pos) & 1) == 0);
          // Is row squashed? Note err_ctl always 0 on Tofino
          uint8_t err_ctl = get_deferred_ram_err_ctl(s_or_m, arr_pos);
          bool squash = (err) ?(err_ctl == 2) :(err_ctl == 3);
          if ((en) && (ingress == ing) && (!squash)) {
            outrows |= (1u<<i); // Row still good
          }
        }
        arr_pos++;
      }
    }
    if (outrows != inrows) {
      RMT_LOG_OBJ(mau_, RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistEop),
                  "MauAddrDist::deferred_ram_err_squash(ie=%d,sm=%d) 0x%x\n",ingress,s_or_m,outrows);
    }
    return outrows;
  }
  // Figure out if overflow is enabled for output of stats/meter addresses
  // NOTE, these days this also governs whether oflow can be used at HeaderTime
  uint32_t MauAddrDist::deferred_ram_oflow(uint32_t rows, bool ingress, int s_or_m) {
    uint32_t validrowsEITHER = kAdistMaskValidStatsOflo|kAdistMaskValidMeterOflo;
    uint32_t validrows = (s_or_m == 0) ?kAdistMaskValidStatsOflo :kAdistMaskValidMeterOflo;
    uint32_t inrows = (rows & kAdistMaskHomeRows); // Ignore any overflow
    uint8_t oflow_rows_en = get_deferred_oflo_ctl();
    int bit_pos = 0; // Each valid homerow has a single bit configuring it
    // Go through all valid homerows (not overflow)
    for (int i = 0; i < kLogicalRows; i++) {
      if (((validrows & (1u<<i)) != 0u) && ((inrows & (1u<<i)) != 0u)) {
        // If row enabled for overflow return EXTRA oflow rows to use
        if ((oflow_rows_en & (1<<bit_pos)) != 0) return inrows | kAdistMaskOflow;
      }
      if ((validrowsEITHER & (1u<<i)) != 0u) bit_pos++;
    }
    return inrows;
  }


  uint32_t MauAddrDist::calculate_stats_rows(int logical_table, bool ingress,
                                             uint8_t eop_num, uint8_t when) {
    // Calculate all the rows this address goes to, then figure out which
    //  need to happen at header time, which at eop time and which at teop time
    uint32_t stats_dist_rows = get_stats_adist_mask(&adr_dist_stats_icxbar_, logical_table);
    // Whinge if invalid rows used
    invalid_row_used_check("Stats", stats_dist_rows, kAdistMaskValidStatsRows);
    // Remove invalid rows (ie Oflow, HomeRows without StatsALU)
    stats_dist_rows &= kAdistMaskValidStatsRows;

    // Now which of these are HDR/EOP/TEOP rows
    uint32_t stats_hdr_rows = 0u, stats_eop_rows = 0u, stats_teop_rows = 0u;
    get_row_usage(stats_dist_rows, ingress, 0,
                  &stats_hdr_rows, &stats_eop_rows, &stats_teop_rows);
    uint32_t stats_all_rows = stats_hdr_rows|stats_eop_rows|stats_teop_rows;
    RMT_ASSERT((stats_dist_rows & stats_all_rows) == stats_all_rows);

    switch (when) {
      case When::kHdrTime:
        // Maybe splice in oflo
        stats_hdr_rows = deferred_ram_oflow(stats_hdr_rows, ingress, 0);
        RMT_ASSERT((stats_hdr_rows == 0u) || (stats_eop_rows == 0u));
        return stats_hdr_rows;
      case When::kEopTime:
        // Maybe splice in oflo
        stats_eop_rows = deferred_ram_oflow(stats_eop_rows, ingress, 0);
        RMT_ASSERT((stats_hdr_rows == 0u) || (stats_eop_rows == 0u));
        // If calculating Eop rows check valid EopNum
        if (stats_eop_rows != 0u) RMT_ASSERT(Eop::valid_eopnum(eop_num));
        return stats_eop_rows;

      case When::kTeopTime: return stats_teop_rows;
      default:              return 0u;
    }
  }
  uint32_t MauAddrDist::calculate_meter_rows(int logical_table, bool ingress,
                                             uint8_t eop_num, uint8_t when) {
    // Calculate all the rows this address goes to, then figure out which
    //  need to happen at header time, which at eop time and which at teop time
    uint32_t meter_dist_rows = get_meter_adist_mask(&adr_dist_meter_icxbar_, logical_table);
    // Whinge if invalid rows used
    invalid_row_used_check("Meter", meter_dist_rows, kAdistMaskValidMeterRows);
    // Remove invalid rows (ie Oflow, HomeRows without MeterALU)
    meter_dist_rows &= kAdistMaskValidMeterRows;

    // Now which of these are HDR/EOP/TEOP rows
    uint32_t meter_hdr_rows = 0u, meter_eop_rows = 0u, meter_teop_rows = 0u;
    get_row_usage(meter_dist_rows, ingress, 1,
                  &meter_hdr_rows, &meter_eop_rows, &meter_teop_rows);
    uint32_t meter_all_rows = meter_hdr_rows|meter_eop_rows|meter_teop_rows;
    RMT_ASSERT((meter_dist_rows & meter_all_rows) == meter_all_rows);

    switch (when) {
      case When::kHdrTime:
        // Maybe splice in oflo
        meter_hdr_rows = deferred_ram_oflow(meter_hdr_rows, ingress, 1);
        RMT_ASSERT((meter_hdr_rows == 0u) || (meter_eop_rows == 0u));
        return meter_hdr_rows;
      case When::kEopTime:
        // Maybe splice in oflo
        meter_eop_rows = deferred_ram_oflow(meter_eop_rows, ingress, 1);
        RMT_ASSERT((meter_hdr_rows == 0u) || (meter_eop_rows == 0u));
        // If calculating Eop rows check valid EopNum
        if (meter_eop_rows != 0u) RMT_ASSERT(Eop::valid_eopnum(eop_num));
        return meter_eop_rows;

      case When::kTeopTime: return meter_teop_rows;
      default:              return 0u;
    }
  }


  // Allow addrs to be rewritten in/out of deferred ram (for movereg)
  void MauAddrDist::setup_rewrite_eop_addrs(uint8_t addrtype, int lt,
                                            int src, int dst, uint32_t mask) {
    RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
    if (addrtype == AddrType::kStats)
      stats_deferred_ram_rewrite_info_[lt].setup_rewrite_addrs(src, dst, mask);
    else if (addrtype == AddrType::kMeter)
      meter_deferred_ram_rewrite_info_[lt].setup_rewrite_addrs(src, dst, mask);
  }
  uint32_t MauAddrDist::rewrite_eop_addr(uint8_t addrtype, int lt, uint32_t addr) {
    RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
    if (addrtype == AddrType::kStats)
      return stats_deferred_ram_rewrite_info_[lt].rewrite_addr(addr);
    else if (addrtype == AddrType::kMeter)
      return meter_deferred_ram_rewrite_info_[lt].rewrite_addr(addr);
    else
      return addr;
  }

  // Funcs to store/retrieve eop addresses in/from an unordered_map
  bool MauAddrDist::get_eop_addr(uint8_t addrtype, int lt, uint8_t eop_num,
                                 uint32_t *addr, uint32_t *rows) {
    RMT_ASSERT((addr != NULL) && (rows != NULL));
    try {
      std::array<uint32_t,2> entry = eop_map_.at(make_map_key(addrtype,lt,eop_num));
      // Disable rewrite on output for now
      //*addr = rewrite_eop_addr(addrtype, lt, entry[0]);
      *addr = entry[0];
      *rows = entry[1];
      RMT_LOG_OBJ(mau_, RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistEop),
                  "MauAddrDist::get_eop_addr(0x%02X,%d,0x%02x) addr=0x%08x rows=0x%04x\n",
                  addrtype, lt, eop_num,*addr,*rows);
      return true;
    } catch (const std::exception&) {
      // Just means no value yet written for that addrtype/logical_table/eop_num
      return false;
    }
  }
  void MauAddrDist::set_eop_addr(uint8_t addrtype, int lt, uint8_t eop_num,
                                 uint32_t addr, uint32_t rows) {
    RMT_LOG_OBJ(mau_, RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistEop),
                "MauAddrDist::set_eop_addr(0x%02X,%d,0x%02x) addr=0x%08x rows=0x%04x\n",
                addrtype, lt, eop_num, addr, rows);
    std::array<uint32_t,2> entry = {addr, rows};
    // Disable rewrite on input for now
    //entry.at(0) = rewrite_eop_addr(addrtype, lt, addr);
    try {
      eop_map_.at(make_map_key(addrtype,lt,eop_num)) = entry;
    } catch (const std::exception&) {
      // Probably means no value yet written for that logical_table/addr - try emplace
      try {
        eop_map_.emplace(make_map_key(addrtype,lt,eop_num),entry);
      } catch (const std::exception&) {
        RMT_LOG_OBJ(mau_, RmtDebug::error(),
                    "MauAddrDist::set_eop_addr(0x%02X,%d,0x%02x) - Exception!\n",
                    addrtype, lt, eop_num);
      }
    }
  }
  void MauAddrDist::clear_eop_addr(uint8_t addrtype, int lt, uint8_t eop_num) {
    RMT_LOG_OBJ(mau_, RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistEop),
                "MauAddrDist::clear_eop_addr(0x%02X,%d,0x%02x)\n",
                addrtype, lt, eop_num);
    int erased = eop_map_.erase(make_map_key(addrtype,lt,eop_num));
    RMT_ASSERT( erased == 1 );
  }
  void MauAddrDist::update_eop_addr(uint8_t addrtype, int lt,
                                    int old_addr, int new_addr) {
    uint32_t mask; // NB ignore subword bits (bottom 7b) in meter addr compare
    if      (addrtype == AddrType::kStats) mask = Address::kStatsAddrAddrMask;
    else if (addrtype == AddrType::kMeter) mask = Address::kMeterAddrAddrMask & ~Address::kMeterSubwordMask;
    else                                   mask = 0xFFFFFFFFu;
    // Setup old->new rewrite (either addr < 0 prevents any rewrite)
    setup_rewrite_eop_addrs(addrtype, lt, old_addr, new_addr, mask);
    if ((old_addr < 0) || (new_addr < 0)) return;

    std::unordered_map< uint16_t, std::array<uint32_t,2> >::iterator it = eop_map_.begin();
    while (it != eop_map_.end()) {
      uint16_t key = it->first;
      std::array<uint32_t,2> & val = it->second;
      if ((map_key_get_addrtype(key) == addrtype) && (map_key_get_logtab(key) == lt) &&
          ((val.at(0) & mask) == (static_cast<uint32_t>(old_addr) & mask))) {
        val.at(0) = rewrite_eop_addr(addrtype, lt, val.at(0));
        RMT_LOG_OBJ(mau_, RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistEop),
                    "MauAddrDist::update_eop_addr eop=%d old_addr=%x addrtype=%d lt=%d mask=%x new_value=%x\n",
                    key&0xff,old_addr,addrtype,lt,mask,it->second.at(0));
      }
      ++it;
    }
  }


  // Need these next funcs to allow PBUS access
  uint32_t MauAddrDist::calculate_deferred_rows(uint8_t addrtype, int lt, uint8_t eop_num) {
    MauLogicalTable *ltab = mau_->logical_table_lookup(lt);
    if (ltab == NULL) return 0u;
    if (addrtype == AddrType::kStats)
      return calculate_stats_rows(lt, ltab->is_ingress(), eop_num, When::kEopTime);
    else if (addrtype == AddrType::kMeter)
      return calculate_meter_rows(lt, ltab->is_ingress(), eop_num, When::kEopTime);
    else
      return 0u;
  }
  bool MauAddrDist::row_read_eop_addr(uint8_t addrtype, int logrow, uint8_t eop_num,
                                      uint32_t *addr) {
    for (int lt = 0; lt < kLogicalTables; lt++) {
      uint32_t tmp_addr = 0u, actual_rows = 0u;
      uint32_t deferred_rows = calculate_deferred_rows(addrtype, lt, eop_num);
      if (((deferred_rows & (1<<logrow)) != 0u) &&
          (get_eop_addr(addrtype, lt, eop_num, &tmp_addr, &actual_rows)) &&
          ((actual_rows & (1<<logrow)) != 0u)) {
        // Return first addr found
        if (addr != NULL) *addr = tmp_addr;
        return true;
      }
    }
    return false;
  }
  void MauAddrDist::row_write_eop_addr(uint8_t addrtype, int logrow, uint8_t eop_num,
                                       uint32_t addr) {
    for (int lt = 0; lt < kLogicalTables; lt++) {
      uint32_t deferred_rows = calculate_deferred_rows(addrtype, lt, eop_num);
      if ((deferred_rows & (1<<logrow)) != 0u) {
        // Set addr for ALL tables that defer this logrow
        set_eop_addr(addrtype, lt, eop_num, addr, 1<<logrow);
      }
    }
  }



  // Queue/Dequeue/Update Color Writes per-MeterALU
  void MauAddrDist::queue_color_write(int alu, uint64_t relative_time, uint64_t wr_latency,
                                      MauMapram *mapram, uint32_t addr, uint8_t color) {
    RMT_ASSERT((alu >= 0) && (alu < kNumMeterAlus));
    RMT_ASSERT(mapram != NULL);
    uint64_t backT = UINT64_C(0);
    if (!color_write_info_[alu].empty())
      backT = color_write_info_[alu].back().relative_time_;
    if ((relative_time > UINT64_C(0)) && (relative_time < backT)) {
      RMT_LOG_OBJ(mau_,RmtDebug::warn(),
                  "MauAddrDist::queue_color_write RelativeTime=%" PRIu64
                  " < backT=%" PRIu64 "\n",
                  relative_time, backT);
    }
    //RMT_ASSERT(relative_time >= backT);
    ColorWriteEntry w;
    w.relative_time_ = relative_time;
    w.wr_latency_ = wr_latency;
    w.addr_ = addr;
    w.color_ = color;
    w.wr_mapram_row_ = mapram->row_index();
    w.wr_mapram_col_ = mapram->col_index();
    w.valid_ = true;
    color_write_info_[alu].push_back(w);
  }
  void MauAddrDist::dequeue_color_writes(int alu, uint64_t relative_time, MauMapram *mapram) {
    RMT_ASSERT((alu >= 0) && (alu < kNumMeterAlus));
    RMT_ASSERT(mapram != NULL);
    uint64_t frontT = UINT64_C(0);
    if (!color_write_info_[alu].empty())
      frontT = color_write_info_[alu].front().relative_time_;
    if (relative_time < frontT) {
      RMT_LOG_OBJ(mau_,RmtDebug::warn(), "MauAddrDist::dequeue_color_writes(alu=%d) "
                  "RelativeTime=%" PRIu64 " < frontT=%" PRIu64 "\n",
                  alu, relative_time, frontT);
    }

    // Loop down Q
    // Stop if we find a valid entry with a relative_time within the MapramColorWriteLatency
    // Otherwise, if entry is valid and has same VPN as mapram, update mapram with color
    std::deque<ColorWriteEntry>::iterator it = color_write_info_[alu].begin();

    while (it != color_write_info_[alu].end()) {
      if (it->valid_) {
        bool vpn_match = mapram->vpn_match(Address::color_addr_get_vpn(it->addr_, AddrType::kMeter));
        bool still_pending = (relative_time < it->relative_time_ + it->wr_latency_);
        bool deQ = (vpn_match && !still_pending);

        const char *s1 = (deQ) ?"DeQ" :"LeaveOnQ";
        const char *s2 = (relative_time < it->relative_time_) ?"!!!!!!!!!!" :"";
        RMT_LOG_OBJ(mau_, RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistColorWrite),
                    "MauAddrDist::dequeue_color_writes(alu=%d,addr=0x%x,color=%d) "
                    "time_now=%" PRIu64 " time_of_write=%" PRIu64 " "
                    "wr_latency=%" PRIu64 " match=%d pending=%d %s %s\n",
                    alu, it->addr_, it->color_, relative_time, it->relative_time_,
                    it->wr_latency_, vpn_match, still_pending, s1, s2);
        if (deQ) {
          // Update mapram with color and mark entry as invalid
          mapram->deferred_update_color(it->addr_, it->color_,
                                        relative_time, it->relative_time_);
          it->valid_ = false;
        }
      }
      it++;
    }
    // Loop down Q again - remove front entries that have been used (valid_ = false)
    while ((!color_write_info_[alu].empty()) && (!color_write_info_[alu].front().valid_)) {
      color_write_info_[alu].pop_front();
    }
  }
  void MauAddrDist::update_queued_color_writes(int alu, int old_addr, int new_addr) {
    RMT_ASSERT((alu >= 0) && (alu < kNumMeterAlus));
    if ((old_addr < 0) || (new_addr < 0)) return;
    uint32_t mask = Address::kMeterAddrAddrMask & ~Address::kMeterSubwordMask;
    // Loop down Q
    // Update any valid addrs if they match old_addr to be new_addr
    std::deque<ColorWriteEntry>::iterator it = color_write_info_[alu].begin();
    while (it != color_write_info_[alu].end()) {
      if (it->valid_) {
        if ((it->addr_ & mask) == (static_cast<uint32_t>(old_addr) & mask)) {
          // addr matches old_addr so rewrite to be new_addr
          uint32_t prev_addr = it->addr_;
          it->addr_ &= ~mask;
          it->addr_ |= static_cast<uint32_t>(new_addr) & mask;
          RMT_LOG_OBJ(mau_, RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistColorWrite),
                      "MauAddrDist::update_queued_color_writes  0x%x -> 0x%x\n",
                      prev_addr, it->addr_);
        }
      }
      it++;
    }
  }
  void MauAddrDist::flush_queued_color_writes(int alu) {
    RMT_ASSERT((alu >= 0) && (alu < kNumMeterAlus));
    for (int r = 0; r < kMapramRows; r++) {
      for (int c = 0; c < kMapramColumns; c++) {
        MauMapram *mapram = mau_->mapram_lookup(r, c);
        if ((mapram != NULL) && (mapram->is_color_mapram())) {
          int mapram_alu = mapram->get_meter_alu_index();
          if (mapram_alu == alu) {
            dequeue_color_writes(alu, UINT64_C(0xFFFFFFFFFFFFFFFF), mapram);
          }
        }
      }
    }
  }




  // Upcall sweeper so it knows to sweep us at the necessary intervals
  // Also called whenever adr_dist_meter_adr_icxbar changes
  // index param ignored
  void MauAddrDist::meter_sweeper_upcall(int index) {
    if (sweeper_ == NULL) return; // Not finished CTOR

    for (int ma = 0; ma < kNumMeterAlus; ma++) {
      uint8_t ma_mask = 1 << ma;

      // Find LT currently sweeping MeterALU - 0xFF => none
      int lt = static_cast<int>(meter_sweep_info_[ma].logical_table());

      if ((lt >= 0) && (lt < kLogicalTables)) {
        // SweepInfo for MeterALU has a LT so IS currently being swept.
        // Check MeterALU still enabled for sweep and check mapping LT->ALU still correct
        // (NB this is the raw mapping NOT rows)
        uint8_t lt_alus = adr_dist_meter_icxbar_.adr_dist_meter_adr_icxbar_ctl(lt);
        if ((!meter_sweep_info_[ma].enabled()) || ((lt_alus & ma_mask) == 0)) {
          // Sweep no longer enabled or LT no longer refers to MeterALU.
          // Stop LT sweep
          sweeper_->meter_set_sweep_interval(mau_->pipe_index(),
                                             mau_->mau_index(),
                                             lt, MauDefs::kMaxInterval);
          // And remove linkage to LT from SweepInfo
          lt = 0xFF;
          meter_sweep_info_[ma].set_logical_table(0xFF);
        }
      }
      if ( ! ((lt >= 0) && (lt < kLogicalTables)) ) {
        // SweepInfo for MeterALU has no LT so NOT currently being swept
        if (meter_sweep_info_[ma].enabled()) {
          // But MeterALU enabled for sweep. So find a LT with correct mapping
          for (int i = 0; i < kLogicalTables; i++) {
            uint8_t lt_alus = adr_dist_meter_icxbar_.adr_dist_meter_adr_icxbar_ctl(i);
            if ((lt_alus & ma_mask) != 0) {
              lt = i;
              meter_sweep_info_[ma].set_logical_table(static_cast<uint8_t>(i));
              break;
            }
          }
          // Might not find any such LT - barf?
          if ( ! ((lt >= 0) && (lt < kLogicalTables)) ) {
            RMT_LOG_OBJ(mau_,RmtDebug::warn(RmtDebug::kRmtDebugMauAddrDistSweep),
                        "MauAddrDist::meter_sweep_upcall() "
                        "Unable to find LT for MeterALU[%d]\n", ma);
          }
        }
      }
      if ((lt >= 0) && (lt < kLogicalTables)) {
        // Set sweep interval - if no change then ignored
        sweeper_->meter_set_sweep_interval(mau_->pipe_index(),
                                           mau_->mau_index(),
                                           lt, meter_sweep_info_[ma].sweep_interval());
      }

    } // for
  }


  // Find ALU associated with an LT
  int MauAddrDist::meter_lt_find_alu(int lt) {
    if ((lt < 0) || (lt >= kLogicalTables)) return -1;
    for (int ma = 0; ma < kNumMeterAlus; ma++) {
      if (static_cast<int>(meter_sweep_info_[ma].logical_table()) == lt)
        return ma;
    }
    return -1;
  }
  // Meter icxbar changed - maybe update RmtSweeper
  void MauAddrDist::meter_icxbar_change_callback(int lt) {
    meter_sweeper_upcall(-1);
  }
  // Some meter sweep interval or enabled/disabled state changed
  // so maybe update RmtSweeper using meter_sweeper_upcall
  void MauAddrDist::meter_sweep_change_callback(int alu) {
    if (sweeper_ == NULL) return;
    RmtObjectManager *om = mau_->get_object_manager();

    bool enabled = meter_sweep_ctl_.meter_sweep_en(alu);
    uint16_t interval = meter_sweep_ctl_.meter_sweep_interval(alu);
    uint8_t min_vpn = meter_sweep_ctl_.meter_sweep_offset(alu);
    uint8_t max_vpn = meter_sweep_ctl_.meter_sweep_size(alu);
    int8_t  hole_pos = -1;
    if (meter_sweep_ctl_.meter_sweep_remove_hole_en(alu) == 1) {
      hole_pos = meter_sweep_ctl_.meter_sweep_remove_hole_pos(alu);
    }

    // Configure both meter_sweep_info and meter_sweep_time_info
    // but we only enable one
    // Note, sweep implementation uses 2^21 as its basic interval
    // (because idle needs that) whereas meters have 2^22 as the
    //  basic interval hence the +1s below
    meter_sweep_info_[alu].set_sweep_interval(interval+1);
    meter_sweep_info_[alu].set_vpn_range(this, min_vpn, max_vpn, hole_pos);
    uint64_t vpn_valid_mask = meter_sweep_info_[alu].vpn_valid_mask();
    bool on_demand = kMeterSweepOnDemand || (kMeterSweepOnDemandPipe0 && (mau_->pipe_index() == 0));
    bool alu_enabled = mau_->is_meter_alu_meter_lpf(alu);
    uint8_t subw_shift = get_meter_sweep_subword_shift(alu);
    uint8_t sweep_op4 = get_meter_sweep_op4(alu);
    meter_sweep_time_info_[alu].config(on_demand, enabled, alu_enabled,
                                       interval+1, subw_shift, sweep_op4,
                                       vpn_valid_mask, om->time_get_cycles());

    meter_sweep_info_[alu].set_enabled(enabled && !on_demand);
    meter_sweeper_upcall(alu);
  }

  // Handle meter sweep for single *LogicalTable* - called by RmtSweeper
  void MauAddrDist::meter_sweep(int lt, uint64_t t_now_psecs) {
    RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
    int alu = meter_lt_find_alu(lt);
    if (g_disable_meter_sweep || (alu < 0) || (!meter_sweep_info_[alu].enabled())) return;
    mau_->mau_info_incr(MAU_METER_SWEEPS);

    int  huff_shift = 0;
    int  n_subwords = get_meter_sweep_subwords(alu);
    bool subwords_ok = (n_subwords == 1);
    int  sweep_op4 = get_meter_sweep_op4(alu);
    bool op_ok = (sweep_op4 == Address::kMeterOp4Sweep);
    bool is_stateful = mau_->is_meter_alu_stateful(alu);
    if (is_stateful) {
      // CSR below is [0,1,2,3,4] ==> num_subwords=[1,2,4,8,16], huff_shift=[7,6,5,4,3]
      huff_shift = Address::kMeterSubwordWidth - get_meter_sweep_subword_shift(alu);
      op_ok = ((sweep_op4 == Address::kMeterOp4SaluInst0) || (sweep_op4 == Address::kMeterOp4SaluInst1) ||
               (sweep_op4 == Address::kMeterOp4SaluInst2) || (sweep_op4 == Address::kMeterOp4SaluInst3) ||
               (sweep_op4 == Address::kMeterOp4Sweep) || (sweep_op4 == Address::kMeterOp4SaluClear));
      // NB. kMeterOp4SaluClear is synonym for kMeterOp4Sweep
      subwords_ok = true;
    }
    if (!op_ok || !subwords_ok) {
      RMT_LOG_OBJ(mau_,RmtDebug::warn(RmtDebug::kRmtDebugMauAddrDistSweep),
                  "MauAddrDist::meter_sweep() Unexpected SweepOP(%d) or SweepSubwords(%d) for ALU=%d\n",
                  sweep_op4, n_subwords, alu);
    }
    uint8_t n = 0;
    uint8_t vpn = meter_sweep_info_[alu].seq_to_vpn(n++);
    while (vpn != 0xFF) {
      mau_->lock_resources(); // Take mutex once for each sram
      for (int index = 0; index < kSramEntries; index++) {
        for (int subword = 0; subword < n_subwords; subword++) {
          // Note meter_addr_make2 will mask subword in 0x7F
          int subword2 = (is_stateful) ?(subword << huff_shift) :subword;
          uint32_t addr = Address::meter_addr_make2(vpn, index, subword2);
          uint32_t sweep_addr = Address::meter_addr_make(addr, sweep_op4);
          uint64_t sweep_cyc = RmtSweeper::psecs_to_cycles(t_now_psecs); // RelativeTime
          // We pass same time as relativeT and meterTickT here
          (void)mau_->do_sweep(AddrType::kMeter, lt, sweep_addr, false, sweep_cyc, sweep_cyc);
        }
      }
      mau_->unlock_resources();
      vpn = meter_sweep_info_[alu].seq_to_vpn(n++);
    }
  }
  // Handle meter sweep for *single address* within *ALU*
  // NOTE, also as a convenience takes cycle count not picosecs.
  // Negative return means no sweep performed and indicates error:
  // >=0 => sweep performed
  //  -1 => either sweep not enabled OR address vpn > max_vpn
  //  -2 => in hole
  //  -3 => address vpn < min_vpn
  //  -4 => config error (bad LT or min_vpn > max_vpn)
  //  -5 => param error (inval ALU/addr)
  int MauAddrDist::meter_sweep_one_address(int alu, uint32_t addr,
                                           uint64_t t_metertick_cycles,
                                           uint64_t t_relative_cycles) {
    // Check params
    if (sweeper_ == NULL) return -5;
    if ((alu < 0) || (alu >= kNumMeterAlus)) return -5;
    if (!meter_sweep_info_[alu].enabled()) return -1;

    // Check config
    uint8_t min_vpn = meter_sweep_info_[alu].min_vpn();
    uint8_t max_vpn = meter_sweep_info_[alu].max_vpn();
    int lt = static_cast<int>(meter_sweep_info_[alu].logical_table());
    if ((lt < 0) || (lt >= kLogicalTables)) return -4;
    if (min_vpn > max_vpn) return -4;

    uint8_t vpn = Address::meter_addr_get_vpn(addr);
    if (vpn > max_vpn) return -1;
    if (vpn < min_vpn) return -3;
    if (!meter_sweep_info_[alu].vpn_valid(vpn)) return -2;

    // Next line will overwrite any OP4 supplied in addr
    uint32_t sweep_addr = Address::meter_addr_make(addr, get_meter_sweep_op4(alu));
    mau_->mau_info_incr(MAU_METER_SINGLE_SWEEPS);

    // NOTE t_relative/t_metertick are SWAPPED for call to do_sweep !!!!!
    return mau_->do_sweep(AddrType::kMeter, lt, sweep_addr, true,
                          t_relative_cycles, t_metertick_cycles);
  }
  // Handle meter sweep for *single index/subword* within *ALU*
  // Index MSBs can be used to indicate VPN (which can be relative
  // to table min_vpn if relative_vpn=true)
  // NOTE, also as a convenience takes cycle count not picosecs.
  int MauAddrDist::meter_sweep_one_index(int alu, int index, int subword,
                                         uint64_t t_metertick_cycles,
                                         bool relative_vpn, uint64_t t_relative_cycles) {
    if ((index < 0) || (alu < 0) || (alu >= kNumMeterAlus)) return -5;
    if ((subword < 0) || (subword >= get_meter_sweep_subwords(alu))) return -5;
    uint8_t vpn = index >> MauDefs::kSramAddressWidth;
    if (vpn > SweepInfo::kVpnMax) return -5;
    //if ((meter_sweep_info_[alu].hole_pos() >= 0) && (!relative_vpn)) return -5;

    // Pass metertick_cycles for BOTH t_relative and t_metertick for backward
    // compatibility if t_relative optional param NOT set (still at default val)
    //
    // (WHY: State object only used to maintain one T param on sweep and returned
    //  it for BOTH get_relative_time() and get_meter_tick_time() calls.
    //  But the T value was always just used to carry metertick_time when called by DV.
    //  So now that there are two separate T values in State object on sweep, if
    //  relative_time is NOT set on this func call (so is still at its default value)
    //  we duplicate the value passed as metertick_time to be also relative_time
    //  just in case some code somewhere relies on relative_time value being set)
    //
    if (t_relative_cycles == UINT64_C(0xFFFFFFFFFFFFFFFF))
      t_relative_cycles = t_metertick_cycles;

    if (relative_vpn) vpn = meter_sweep_info_[alu].seq_to_vpn(vpn);
    if (mau_->is_meter_alu_stateful(alu)) {
      // CSR below is [0,1,2,3,4] ==> num_subwords=[1,2,4,8,16], huff_shift=[7,6,5,4,3]
      int huff_shift = Address::kMeterSubwordWidth - get_meter_sweep_subword_shift(alu);
      subword <<= huff_shift; // meter_addr_make2 masks subword in 0x7F
    }
    uint32_t addr = Address::meter_addr_make2(vpn, index, subword);
    return meter_sweep_one_address(alu, addr, t_metertick_cycles, t_relative_cycles);
  }
  // Same signature as meter_sweep_one_index above but NO optional arguments
  int MauAddrDist::meter_sweep_one_index_with_reltime(int alu, int index, int subword,
                                                      uint64_t t_metertick_cycles,
                                                      bool     relative_vpn,
                                                      uint64_t t_relative_cycles) {
    // Insist t_relative is not UINT64_MAX as that's taken to mean 'use t_metertick' above
    RMT_ASSERT(t_relative_cycles < UINT64_C(0xFFFFFFFFFFFFFFFF));
    return meter_sweep_one_index(alu, index, subword,
                                 t_metertick_cycles, relative_vpn, t_relative_cycles);
  }
  // Handle meter sweep for *single index* subword=0 within *ALU*
  int MauAddrDist::meter_sweep_one_index(int alu, int index,
                                         uint64_t t_metertick_cycles,
                                         bool     relative_vpn,
                                         uint64_t t_relative_cycles) {
    return meter_sweep_one_index(alu, index, 0,
                                 t_metertick_cycles, relative_vpn, t_relative_cycles);
  }




  // Upcall sweeper so it knows to sweep us at the necessary intervals
  void MauAddrDist::idletime_sweeper_upcall(int lt, bool idle) {
    if (sweeper_ == NULL) return; // Not finished CTOR
    uint8_t interval = idle_sweep_info_[lt].sweep_interval();
    if (!idle_sweep_info_[lt].enabled() || idle) interval = MauDefs::kMaxInterval;
    sweeper_->idle_set_sweep_interval(mau_->pipe_index(), mau_->mau_index(),
                                      lt, interval);
  }
  // Some table sweep interval or enabled/disabled state changed
  // so maybe update RmtSweeper using idletime_sweeper_upcall
  void MauAddrDist::idletime_sweep_change_callback(int lt) {
    uint8_t old_interval = idle_sweep_info_[lt].sweep_interval();
    bool    old_enabled = idle_sweep_info_[lt].enabled();
    uint8_t new_interval = idletime_sweep_ctl_.idletime_sweep_interval(lt);
    bool    new_enabled = idletime_sweep_ctl_.idletime_sweep_en(lt);
    uint8_t min_vpn = idletime_sweep_ctl_.idletime_sweep_offset(lt);
    uint8_t max_vpn = idletime_sweep_ctl_.idletime_sweep_size(lt);
    int8_t  hole_pos = -1;
    if (idletime_sweep_ctl_.idletime_sweep_remove_hole_en(lt) == 1) {
      hole_pos = idletime_sweep_ctl_.idletime_sweep_remove_hole_pos(lt);
    }
    idle_sweep_info_[lt].set_sweep_interval(new_interval);
    idle_sweep_info_[lt].set_enabled(new_enabled);
    // If range changes should recalculate mapram ordering
    if (idle_sweep_info_[lt].set_vpn_range(this, min_vpn, max_vpn, hole_pos))
      pending_seq_++;

    if ((new_interval != old_interval) || (new_enabled != old_enabled))
      idletime_sweeper_upcall(lt, false);
  }
  // Some dump state changed
  void MauAddrDist::idletime_dump_change_callback(int lt) {
    uint8_t min_vpn = idletime_dump_ctl_.idletime_dump_offset(lt);
    uint8_t max_vpn = idletime_dump_ctl_.idletime_dump_size(lt);
    int8_t  hole_pos = -1;
    if (idletime_dump_ctl_.idletime_dump_remove_hole_en(lt) == 1) {
      hole_pos = idletime_dump_ctl_.idletime_dump_remove_hole_pos(lt);
    }
    // If range changes should recalculate mapram ordering
    if (idle_dump_info_[lt].set_vpn_range(this, min_vpn, max_vpn, hole_pos))
      pending_seq_++;
  }

  // Whenever the idletime oxbar changes OR whenever there is some
  // change in an idletime MAP RAM config we need to rebuild the
  // mappings from Logical Tables to MAP RAMs
  //
  // For each Logical Table we keep a sorted list (sorted by VPN)
  // of all enabled idletime MAP RAMs that connect to an Idletime BUS
  // that is pushed to by the Logical Table
  //
  // We go through all MapRAMs (if they exist), find their bus and
  // the associated table then push the MAP RAM into a per logical
  // table Vector which we later sort by VPN
  //
  void MauAddrDist::idletime_oxbar_change_callback() {
    // Mappings between LogicalTables and Idletime buses changed
    pending_seq_++;
  }
  void MauAddrDist::mapram_change_callback() {
    // Some MAP RAM config has changed - we need to recalculate
    // the mappings from Logical Tables -> MAP RAMs
    pending_seq_++;
  }
  // Called from idletime_sweep to maybe update mapram mappings
  void MauAddrDist::update_mapram_mappings() {
    while (curr_seq_ < pending_seq_) {
      curr_seq_ = pending_seq_;

      // DON'T bother dynamically allocating/switching for now. This func
      // always called synchronously from idletime_sweep()/idletime_dump()
      for (int lt = 0; lt < kLogicalTables; lt++) {
        idle_sweep_info_[lt].clear(); idle_dump_info_[lt].clear();
      }
      // Go thru all maprams. If idletime maprams (and enabled)
      // figure out which logical table they belong to and add
      // to the corresponding sweep_info obj
      for (int r = 0; r < kMapramRows; r++) {
        for (int c = 0; c < kMapramColumns; c++) {
          MauMapram *mapram = mau_->mapram_lookup(r, c);
          if ((mapram != NULL) &&
              (mapram->is_idletime_mapram()) &&
              (mapram->idletime_enabled()) &&
              (mapram->is_valid_idletime_mode())) {
            int bus = mapram->idletime_bus();
            int logtab = (bus >= 0) ?get_idletime_logical_table(bus) :-1;
            if (logtab >= 0) {
              RMT_ASSERT(logtab < kLogicalTables);
              idle_sweep_info_[logtab].add_mapram(mapram);
              idle_dump_info_[logtab].add_mapram(mapram);
            }
          }
        }
      }
      // Now sort maprams for each table - lowest priority/VPN first
      // At the moment we allow sweep/dump sort order to differ
      for (int lt = 0; lt < kLogicalTables; lt++) {
        idle_sweep_info_[lt].sort_maprams(); idle_dump_info_[lt].sort_maprams();
      }
    }
  }


  // The functions below can operate in 2 different modes
  // detemined by the kXXVAddrPbusBubbleEmulate booleans.
  //
  // Mode 1: BubbleEmulate mode
  //   In this mode addresses are distributed as normal, but
  //   with OPs embedded in the address to cause the action
  //   to occur.
  //   This mode corresponds most closely to how the H/W works.
  //   The backend logic is shared between PHV lookups, sweeps,
  //   dumps etc and in S/W this is accomplished safely via the
  //   use of the mau_lock_ mutex.
  //   However this mode is probably less efficient than....
  //
  // Mode 2: Direct mode
  //   In this mode functions are called directly on underlying
  //   objects (eg idletime_sweep, idletime_dump) on maprams.
  //   Atomicity is handled within the func and multiple threads
  //   may simultaneously execute these without blocking.
  //   This mode is more efficient than BubbleEmulate mode but
  //   does not really correspond to how the H/W works.
  //

  // Send idletime notify msg
  void MauAddrDist::idletime_notify(int lt, uint32_t addr, uint64_t data) {
    if (sweeper_ == NULL) return; // Not finished CTOR
    uint64_t msg = MauMapram::kIdletimeDumpMsgType; // Idletime Dump == type 2
    msg |= static_cast<uint64_t>(data) << MauMapram::kIdletimeDumpMsg_DataOffset;
    int addr_off = MauMapram::kIdletimeDumpMsg_AddressOffset;
    int pst_off  = MauMapram::kIdletimeDumpMsg_PipeStageTableOffset;
    int ps_off = pst_off + MauDefs::kTableBits;
    int p_off = ps_off + RmtDefs::kStageBits;
    msg |= static_cast<uint64_t>(addr & Address::kIdletimeAddrAddrMask)        << addr_off;
    msg |= static_cast<uint64_t>(lt & MauDefs::kTableMask)                     << pst_off;
    msg |= static_cast<uint64_t>(mau_->mau_dump_index() & RmtDefs::kStageMask) << ps_off;
    msg |= static_cast<uint64_t>(mau_->pipe_dump_index() & RmtDefs::kPipeMask) << p_off;
    sweeper_->idle_notify(mau_->pipe_index(), mau_->mau_index(), msg);
  }

  // Handle idle sweep for single LogicalTable
  // NB. Notifies emit from within MauMapram code for the moment
  void MauAddrDist::idletime_sweep(int lt, uint64_t t_now_psecs) {
    if (!idle_sweep_info_[lt].enabled()) return;
    mau_->mau_info_incr(MAU_IDLETIME_SWEEPS);

    // Note, doing sweep using bubble for each address is *really*
    // slow, so normally use 'fast' mode in else section
    if (MauMemory::kIdletimeVAddrSweepBubbleEmulate) {
      bool now_idle = true;
      idle_sweep_info_[lt].idle_sweep_start(this, lt);
      uint8_t n = 0;
      uint8_t vpn = idle_sweep_info_[lt].seq_to_vpn(n++);
      while (vpn != 0xFF) {
        mau_->lock_resources(); // Take mutex once for each mapram
        for (int index = 0; index < kMapramEntries; index++) {
          uint32_t addr = Address::idletime_addr_make2(vpn, index, 0);
          uint32_t sweep_addr = Address::idletime_addr_make(addr, Address::kIdletimeOpSweep);
          uint64_t sweep_cyc = RmtSweeper::psecs_to_cycles(t_now_psecs); // RelativeTime
          int ret = mau_->do_sweep(AddrType::kIdle, lt, sweep_addr, false, sweep_cyc);
          if (ret == 0) now_idle = false;
        }
        mau_->unlock_resources();
        vpn = idle_sweep_info_[lt].seq_to_vpn(n++);
      }
      idle_sweep_info_[lt].idle_sweep_end(this, lt, now_idle); // May upcall sweeper
    } else {
      // Update mappings if any maprams OR oxbar OR min/max vpn changed
      update_mapram_mappings();
      // Then sweep table
      idle_sweep_info_[lt].idle_sweep(this, lt, t_now_psecs);
    }
  }
  // Handle idletime sweep for *single address*
  // NOTE, also as a convenience takes cycle count not picosecs.
  // Negative return means no sweep performed and indicates error:
  // >=0 => sweep performed
  //  -1 => either sweep not enabled OR address vpn > max_vpn
  //  -2 => in hole
  //  -3 => address vpn < min_vpn
  //  -4 => config error (min_vpn > max_vpn)
  //  -5 => param error (inval LT/addr)
  int MauAddrDist::idletime_sweep_one_address(int lt, uint32_t addr,
                                              uint64_t t_now_cycles) {
    // Check params
    if (sweeper_ == NULL) return -5;
    if ((lt < 0) || (lt >= kLogicalTables)) return -5;
    // Huffman decode to get shift - should be in [1,4]
    int shift = Address::idletime_addr_get_shift(addr);
    int subword = Address::idletime_addr_get_subword(addr, shift);
    if (subword >= Address::idletime_addr_get_nentries(addr, shift)) return -5;
    if (!idle_sweep_info_[lt].enabled()) return -1;

    // Check config
    uint8_t min_vpn = idle_sweep_info_[lt].min_vpn();
    uint8_t max_vpn = idle_sweep_info_[lt].max_vpn();
    if (min_vpn > max_vpn) return -4;

    uint8_t vpn = Address::idletime_addr_get_vpn(addr);
    if (vpn > max_vpn) return -1;
    if (vpn < min_vpn) return -3;
    if (!idle_sweep_info_[lt].vpn_valid(vpn)) return -2;

    uint32_t sweep_addr = Address::idletime_addr_make(addr, Address::kIdletimeOpSweep);

    mau_->mau_info_incr(MAU_IDLETIME_SINGLE_SWEEPS);
    return mau_->do_sweep(AddrType::kIdle, lt, sweep_addr, true, t_now_cycles);
  }
  // Handle idletime sweep for *single index*
  // Index MSBs can be used to indicate VPN (which can be relative
  // to table min_vpn if relative_vpn=true)
  // NOTE, subword MUST include Huffman bits
  // NOTE, also as a convenience takes cycle count not picosecs.
  int MauAddrDist::idletime_sweep_one_index(int lt, int index, int subword,
                                            uint64_t t_now_cycles,
                                            bool relative_vpn) {
    if ((index < 0) || (lt < 0) || (lt >= kLogicalTables)) return -5;
    uint8_t vpn = index >> MauDefs::kMapramAddressWidth;
    if (vpn > SweepInfo::kVpnMax) return -5;
    if (relative_vpn) vpn = idle_sweep_info_[lt].seq_to_vpn(vpn);
    uint32_t addr = Address::idletime_addr_make2(vpn, index, subword);
    return idletime_sweep_one_address(lt, addr, t_now_cycles);
  }


  // Handle idle dump for addr within LogicalTable
  void MauAddrDist::idletime_dump_word(int lt, uint32_t addr, bool clear) {
    //if ((cfg_has_idle_.mau_cfg_lt_has_idle() & (1<<lt)) == 0) return;
    uint8_t vpn = Address::idletime_addr_get_vpn(addr);
    if (!idle_dump_info_[lt].vpn_valid(vpn)) return;
    mau_->mau_info_incr(MAU_IDLETIME_DUMP_WORDS);

    if (MauMemory::kIdletimeVAddrDumpWordBubbleEmulate) {
      uint64_t data0 = UINT64_C(0), data1 = UINT64_C(0);
      int ret = mau_->pbus_read(AddrType::kIdle, lt, addr, &data0, &data1, clear, true);
      if (ret == 1) idletime_notify(lt, addr, data0);
    } else {
      // Update mappings if any maprams OR oxbar OR min/max vpn changed
      update_mapram_mappings();
      // Then dump addr within table
      idle_dump_info_[lt].idle_dump_word(addr, clear);
    }
  }
  // Handle idle dump for single LogicalTable
  void MauAddrDist::idletime_dump(int lt, bool clear) {
    //if ((cfg_has_idle_.mau_cfg_lt_has_idle() & (1<<lt)) == 0) return;
    mau_->mau_info_incr(MAU_IDLETIME_DUMPS);
    if (MauMemory::kIdletimeVAddrDumpBubbleEmulate) {
      uint8_t n = 0;
      uint8_t vpn = idle_dump_info_[lt].seq_to_vpn(n++);
      while (vpn != 0xFF) {
        mau_->lock_resources(); // Take mutex once for each mapram
        for (int index = 0; index < kMapramEntries; index++) {
          uint32_t addr = Address::idletime_addr_make2(vpn, index, 0);
          uint64_t data0 = UINT64_C(0), data1 = UINT64_C(0);
          int ret = mau_->pbus_read(AddrType::kIdle, lt, addr, &data0, &data1, clear, false);
          if (ret == 1) {
            mau_->unlock_resources(); // Release mutex whilst we notify
            idletime_notify(lt, addr, data0);
            mau_->lock_resources();
          }
        }
        mau_->unlock_resources();
        vpn = idle_dump_info_[lt].seq_to_vpn(n++);
      }
    } else {
      // Update mappings if any maprams OR oxbar OR min/max vpn changed
      update_mapram_mappings();
      // Then dump table
      idle_dump_info_[lt].idle_dump( clear);
    }
  }

  // Handle idle hit for single LogicalTable
  void MauAddrDist::idletime_hit(int lt) {
    idle_sweep_info_[lt].idle_hit(this, lt);
  }



  // Figure out stats format for a LogicalTable
  int MauAddrDist::stats_format(int lt) {
    // Deduce stats format from stats_dump_ctl
    bool dump_pkts = (stats_dump_ctl_.stats_dump_has_packets(lt) == 0x1);
    bool dump_bytes = (stats_dump_ctl_.stats_dump_has_bytes(lt) == 0x1);
    int  dump_entries = (stats_dump_ctl_.stats_dump_entries_per_word(lt) & 0x7);
    int  format = MauStatsAlu::get_stats_format(dump_pkts, dump_bytes, dump_entries);
    if (!MauStatsAlu::is_valid_stats_mode(format)) {
      RMT_LOG_OBJ(mau_,RmtDebug::error(),
                  "MauAddrDist: Invalid stats dump format %d\n", format);
    }
    return format;
  }

  // Send stats notify msg - either on dump or on evict
  void MauAddrDist::stats_notify(int lt, uint32_t addr, uint64_t data, bool dump) {
    if (sweeper_ == NULL) return; // Not finished CTOR
    uint64_t msg;
    int addr_off, pst_off, data_off;
    if (dump) {
      msg = MauStatsAlu::kStatsDumpMsgType; // Stats Dump == type 2
      addr_off = MauStatsAlu::kStatsDumpMsg_AddressOffset;
      pst_off  = MauStatsAlu::kStatsDumpMsg_PipeStageTableOffset;
      data_off = MauStatsAlu::kStatsDumpMsg_DataOffset;
    } else {
      msg = MauStatsAlu::kStatsFsmMsgType; // Stats Evict == type 0
      addr_off = MauStatsAlu::kStatsFsmMsg_AddressOffset;
      pst_off  = MauStatsAlu::kStatsFsmMsg_PipeStageTableOffset;
      data_off = MauStatsAlu::kStatsFsmMsg_DataOffset;
    }
    int ps_off = pst_off + MauDefs::kTableBits;
    int p_off = ps_off + RmtDefs::kStageBits;
    // Verify data offset - currently we assume is second 64-bit word
    RMT_ASSERT(data_off == 64);
    msg |= static_cast<uint64_t>(addr & Address::kStatsAddrAddrMask)           << addr_off;
    msg |= static_cast<uint64_t>(lt & MauDefs::kTableMask)                     << pst_off;
    msg |= static_cast<uint64_t>(mau_->mau_dump_index() & RmtDefs::kStageMask) << ps_off;
    msg |= static_cast<uint64_t>(mau_->pipe_dump_index() & RmtDefs::kPipeMask) << p_off;
    sweeper_->stats_notify(mau_->pipe_index(), mau_->mau_index(), msg, data);
  }

  // stats_dump_ctl has changed so recalc VPN range
  void MauAddrDist::stats_dump_change_callback(int lt) {
    uint8_t min_vpn = stats_dump_ctl_.stats_dump_offset(lt);
    uint8_t max_vpn = stats_dump_ctl_.stats_dump_size(lt);
    int8_t  hole_pos = -1;

    // In the stats_dump case there are NO holes, but there
    // ARE still hole registers (which are unused) so simply
    // ignore hole programming for dump - comment out lines below.
    //
    //if (stats_dump_ctl_.stats_dump_remove_hole_en(lt) == 1) {
    //   hole_pos = stats_dump_ctl_.stats_dump_remove_hole_pos(lt);
    //}
    stats_dump_info_[lt].set_vpn_range(this, min_vpn, max_vpn, hole_pos);
  }
  // Handle stats dump for addr within LogicalTable
  void MauAddrDist::stats_dump_word(int lt, uint32_t addr, bool dump) {
    //if ((cfg_has_stats_.mau_cfg_lt_has_stats() & (1<<lt)) == 0) return;
    uint64_t ZERO = UINT64_C(0);
    uint64_t data0 = ZERO, data1 = ZERO, dump_data1 = ZERO, dump_data2 = ZERO;
    uint16_t index = Address::stats_addr_get_index(addr);
    int subword = Address::stats_addr_get_subword(addr);
    // Deduce stats format from stats_dump_ctl - exit if invalid
    int format = stats_format(lt);
    if (!MauStatsAlu::is_valid_stats_mode(format)) return;

    //uint8_t vpn = Address::stats_addr_get_vpn(addr);
    //if (!stats_dump_info_[lt].vpn_valid(vpn)) return;
    mau_->mau_info_incr(MAU_STATS_DUMP_WORDS);

    // Maybe do work by directly driving back-end
    if (MauMemory::kStatsVAddrDumpWordBubbleEmulate) {
      int ret = mau_->pbus_read(AddrType::kStats, lt, addr, &data0, &data1, true, true);
      if (ret == 0) return;
    } else {
      MauSram *sram = mau_->mau_memory()->stats_virt_find_sram(lt, addr);
      if (sram == NULL) return;
      // Get subword mask corresponding to subword
      BitVector<kDataBusWidth> mask;
      if (!MauStatsAlu::get_subword_mask(format, subword, &mask)) return;
      // Atomically read and clear subword bits from SRAM entry
      //  lastarg==true ==> DONT clear if subword exactly matches subword mask
      sram->read_and_clear(index, &data0, &data1, mask, MauStatsAlu::is_ones_rsvd(format));
    }
    // Format data0/data1 into dump word(s)
    // Here lastarg==false means dump word(s) returned even for RSVD vals
    int n_dumps = MauStatsAlu::get_dump_data_word(format, subword, data0, data1,
                                                  &dump_data1, &dump_data2, false);
    if (n_dumps > 0) stats_notify(lt, addr, dump_data1, dump);
    if (n_dumps > 1) stats_notify(lt, addr+4, dump_data2, dump);
  }
  // Handle stats dump for single LogicalTable
  void MauAddrDist::stats_dump(int lt) {
    constexpr int max_subwords = MauDefs::kMaxStatsEntriesPerWord;
    //if ((cfg_has_stats_.mau_cfg_lt_has_stats() & (1<<lt)) == 0) return;
    mau_->mau_info_incr(MAU_STATS_DUMPS);

    // Deduce stats format from stats_dump_ctl - exit if invalid
    int format = stats_format(lt);
    if (!MauStatsAlu::is_valid_stats_mode(format)) return;
    bool ones_rsvd = MauStatsAlu::is_ones_rsvd(format);
    bool subword_valid[max_subwords];
    BitVector<kDataBusWidth> subword_mask[max_subwords];
    for (int s = 0; s < max_subwords; s++)
      subword_valid[s] = MauStatsAlu::get_subword_mask(format, s, &subword_mask[s]);
    uint64_t ZERO = UINT64_C(0);
    uint64_t data0, data1, dump_data1, dump_data2;

    uint8_t n = 0;
    uint8_t vpn = stats_dump_info_[lt].seq_to_vpn(n++);
    while (vpn != 0xFF) {
      mau_->lock_resources(); // Take mutex once per VPN
      // NB Must take mutex on non-bubble path too as
      // there is a risk of a race against packet lookup
      // post stats SRAM read but pre stats SRAM write.
      // If dump were to occur in the interval zeroisation
      // would be undone on SRAM write

      if (MauMemory::kStatsVAddrDumpWordBubbleEmulate) {
        for (int index = 0; index < kSramEntries; index++) {
          for (int sub = 0; sub < max_subwords; sub++) {
            if (subword_valid[sub]) {
              uint32_t addr = Address::stats_addr_make2(vpn, index, sub);
              data0 = ZERO; data1 = ZERO; dump_data1 = ZERO, dump_data2 = ZERO;
              if (mau_->pbus_read(AddrType::kStats, lt, addr, &data0, &data1,
                                  true, false) != 0) { // true,false ==> clear,nolock
                // Here lastarg==true means NO dump word(s) returned for RSVD vals
                int n_dumps = MauStatsAlu::get_dump_data_word(format, sub, data0, data1,
                                                              &dump_data1, &dump_data2, true);
                if (n_dumps > 0) stats_notify(lt, addr, dump_data1, true);
                if (n_dumps > 1) stats_notify(lt, addr+4, dump_data2, true);
              }
            }
          }
        }
      } else {
        for (int index = 0; index < kSramEntries; index++) {
          uint32_t addr0 = Address::stats_addr_make2(vpn, index, 0);
          MauSram *sram = mau_->mau_memory()->stats_virt_find_sram(lt, addr0);
          if (sram != NULL) {
            for (int sub = 0; sub < max_subwords; sub++) {
              if (subword_valid[sub]) {
                data0 = ZERO; data1 = ZERO; dump_data1 = ZERO, dump_data2 = ZERO;
                // Atomically read and clear subword bits from SRAM entry
                //  lastarg==true ==> DONT clear if subword exactly matches subword mask
                sram->read_and_clear(index, &data0, &data1, subword_mask[sub], ones_rsvd);
                // Here lastarg==true means NO dump word returned for RSVD vals
                int n_dumps = MauStatsAlu::get_dump_data_word(format, sub, data0, data1,
                                                              &dump_data1, &dump_data2, true);
                uint32_t addr = Address::stats_addr_make2(vpn, index, sub);
                if (n_dumps > 0) stats_notify(lt, addr, dump_data1, true);
                if (n_dumps > 1) stats_notify(lt, addr+4, dump_data2, true);
              }
            }
          }
        }
      }
      mau_->unlock_resources();
      vpn = stats_dump_info_[lt].seq_to_vpn(n++);
    } // while (vpn != 0xFF)
  }

  // Called post execute/handle_eop to handle stats LRT evictions
  // Calls sweeper as might need to Q if table lock held
  // But if table unlocked may result in immediate notify
  void MauAddrDist::handle_evictions(const EvictInfo &evictinfo) {
    if (sweeper_ == NULL) return; // Not finished CTOR
    for (int alu = 0; alu < kNumStatsAlus; alu++) {
      uint32_t addr = evictinfo.addr(alu);
      if (Address::stats_addr_op_enabled(addr)) {
        int lt = stats_alu_lt_map_.mau_cfg_stats_alu_lt(alu);
        if ((lt >= 0) && (lt < kLogicalTables)) {
          uint64_t data = evictinfo.data(alu);
          uint32_t mask = Address::kStatsAddrAddrMask;
          sweeper_->stats_evict_word(mau_->pipe_index(), mau_->mau_index(),
                                     lt, addr & mask, data);
        }
      }
    }
  }
  // Called by moveregs to update queued stats addresses
  void MauAddrDist::update_queued_addr(uint8_t addrtype, int lt,
                                       uint32_t old_addr, uint32_t new_addr) {
    if (sweeper_ == NULL) return; // Not finished CTOR
    uint32_t mask = 0xFFFFFFFFu;
    if (addrtype == AddrType::kStats)     mask = Address::kStatsAddrAddrMask;
    else if (addrtype == AddrType::kIdle) mask = Address::kIdletimeAddrAddrMask;
    else return;
    sweeper_->update_queued_addr(mau_->pipe_index(), mau_->mau_index(), lt,
                                 addrtype, old_addr & mask, new_addr & mask);
  }



  // Complain if attempt to access a bus that's not been driven by any logical table
  void MauAddrDist::bus_not_driven_complain(const std::string& bus_name, int row) const {
    if (row >= 0) {
      RMT_LOG_OBJ(mau_,RmtDebug::verbose(),
                  "MauAddrDist: Bus %s[%d] not driven\n", bus_name.c_str(), row);
    } else {
      RMT_LOG_OBJ(mau_,RmtDebug::verbose(),
                  "MauAddrDist: Bus %s not driven\n", bus_name.c_str());
    }
  }
  // Complain if access thread mismatch (eg ingress table accessed for egress)
  void MauAddrDist::ingress_egress_mismatch_complain(const std::string& bus_name, int row) const {
    if (row >= 0) {
      RMT_LOG_OBJ(mau_,RmtDebug::verbose(),
                  "MauAddrDist: Bus %s[%d] ingress/egress MISMATCH!\n", bus_name.c_str(), row);
    } else {
      RMT_LOG_OBJ(mau_,RmtDebug::verbose(),
                  "MauAddrDist: Bus %s ingress/egress MISMATCH!\n", bus_name.c_str());
    }
  }
  // Generic debug output
  void MauAddrDist::addr_dist_debug(const std::string& bus_name, int lt,
                                    uint32_t curr_val, uint32_t or_val) {
    RMT_LOG_OBJ(mau_,RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistOutputToBus),
                "MauAddrDist::output  LogicalTable %d outputs to BUS %s "
                "(initialVal=0x%08x,finalVal=0x%08x)\n",
                lt, bus_name.c_str(), curr_val, curr_val|or_val);
  }
  // Squawk if multiple addresses written to a bus and the
  // bits set in the non-PFE enabled addrs are not a subset of the
  // bits set in the PFE-enabled addrs
  void MauAddrDist::pre_pfe_addr_en_noten_check(const std::string& bus_name,
                                                uint32_t en_addr, uint32_t not_en_addr) {
    if ((en_addr != 0u) && (not_en_addr != 0u) && ((en_addr | not_en_addr) != en_addr))
      RMT_LOG_OBJ(mau_,RmtDebug::error(kRelaxPrePfeAddrCheck),
                  "MauAddrDist: AddrBus %s REUSED!!!! (non PFE-enabled bits set) "
                  " (enabledAddr=0x%08x,!enabledAddr=0x%08x)\n",
                  bus_name.c_str(), en_addr, not_en_addr);
  }
  // Squawk if multiple writers to a bus
  void MauAddrDist::addr_multi_write_check(const std::string& bus_name,
                                           uint32_t curr_val, uint32_t or_val,
                                           int curr_lt, int or_lt) {
    if ((curr_val != 0u) && (or_val != 0u) && (curr_val != or_val))
      RMT_LOG_OBJ(mau_,RmtDebug::warn(),
                  "MauAddrDist: Bus %s REUSED!!!! "
                  " (0x%08x(LT%d) | 0x%08x(LT%d) -> 0x%08x)\n",
                  bus_name.c_str(), curr_val, curr_lt, or_val, or_lt, curr_val|or_val);
  }
  // Squawk if multiple addrtypes written to a bus
  void MauAddrDist::addrtype_multi_write_check(const std::string& bus_name,
                                               uint8_t curr_typ, uint8_t or_typ) {
    if ((curr_typ != 0) && (or_typ != 0) && (curr_typ != or_typ))
      RMT_LOG_OBJ(mau_,RmtDebug::error(),
                "MauAddrDist: Bus %s REUSED for DIFF address types!!!! "
                "(initialType=0x%02x,finalType=0x%02x)\n",
                bus_name.c_str(), curr_typ, curr_typ|or_typ);
  }
  // Squawk if bus being used with a different addrtype from one written
  void MauAddrDist::addrtype_mismatch_check(const std::string& bus_name,
                                            uint8_t written_typ, uint8_t use_typ) const {
    if ((written_typ != 0) && (use_typ != 0) && (written_typ != use_typ))
      RMT_LOG_OBJ(mau_,RmtDebug::warn(),
                "MauAddrDist: Bus %s written but used with DIFF type!!!! "
                "(writtenType=0x%02x,useType=0x%02x)\n",
                bus_name.c_str(), written_typ, use_typ);
  }
  // Squawk if eop_entry still in use
  void MauAddrDist::eop_free_check(const char *eop_name, uint32_t eop_addr, uint8_t eop_num) {
    if (eop_addr == 0u) return;
    RMT_LOG_OBJ(mau_,RmtDebug::error(),
                "MauAddrDist: %s[%d] reused before addr 0x%08x distributed\n",
                eop_name, eop_num, eop_addr);
  }
  // Squawk if invalid homerow used
  void MauAddrDist::invalid_row_used_check(const std::string& bus_name,
                                           uint32_t rows_used, uint32_t rows_valid) {
    if ((rows_used & ~rows_valid) != 0u) {
      RMT_LOG_OBJ(mau_,RmtDebug::error(),
                  "MauAddrDist: Invalid %s row used 0x%08x (valid rows=0x%08x)\n",
                  bus_name.c_str(), rows_used, rows_valid);
    }
  }
  // Squawk if oflow2 used
  void MauAddrDist::oflow2_used_check(const std::string& bus_name, uint32_t val) {
    if ((kDisableOflow2) && (val != 0u)) {
      RMT_LOG_OBJ(mau_,RmtDebug::warn(),
                  "MauAddrDist: %s used! Oflow2 NO longer exists!\n", bus_name.c_str());
    }
  }
  // Squawk if oflow used and oflo_adr_user not set properly
  void MauAddrDist::oflow_used_check(const std::string& bus_name, uint8_t use_typ) {
    uint8_t reg = oflo_adr_user_.oflo_adr_user(0);
    if ((reg == 0) && (use_typ == AddrType::kAction)) return;
    if ((reg == 1) && (use_typ == AddrType::kStats)) return;
    if ((reg == 2) && (use_typ == AddrType::kMeter)) return;
    RMT_LOG_OBJ(mau_,RmtDebug::error(),
                "MauAddrDist: %s used for incorrect addrtype,%d"
                " - SHOULD be %d\n", bus_name.c_str(), use_typ, reg);
  }
  // Squawk if more than one ALU used
  void MauAddrDist::single_alu_check(const char *alu_name, uint32_t alus) {
    if (__builtin_popcountl(alus) > 1) {
      RMT_LOG_OBJ(mau_, (RmtObject::is_jbay_or_later()) ?RmtDebug::warn() :RmtDebug::error(),
                  "MauAddrDist: %s refers to more than 1 ALU (0x%08x)\n",
                  alu_name, alus);
    }
  }
  // Return whether an address was distributed/consumed
  bool MauAddrDist::addr_consumed(uint32_t addr) {
    // addr can only be distributed/consumed if PFE was set
    // Mask off op/PFE - so just compare vpn/index/subword
    int action_addr = Address::action_addr_get_vaddr(addr);
    int stats_addr = Address::stats_addr_get_vaddr(addr);
    int meter_addr = Address::meter_addr_get_vaddr(addr);
    int idletime_addr = Address::idletime_addr_get_vaddr(addr);
    bool found = false;
    for (int i = 0; i < kLogicalRows; i++) {
      if (Address::action_addr_get_vaddr(action_addrs_[i]) == action_addr) {
        found = true;
        if (((action_addr_not_consumed_ >> i) & 1u) == 1u) return false;
      }
      if (Address::stats_addr_get_vaddr(stats_addrs_[i]) == stats_addr) {
        found = true;
        if (((stats_addr_not_consumed_ >> i) & 1u) == 1u) return false;
      }
      if (Address::meter_addr_get_vaddr(meter_addrs_[i]) == meter_addr) {
        found = true;
        if (((meter_addr_not_consumed_ >> i) & 1u) == 1u) return false;
      }
    }
    for (int i = 0; i < kIdletimeBuses; i++) {
      if (Address::idletime_addr_get_vaddr(idletime_addrs_[i]) == idletime_addr) {
        found = true;
        if (((idletime_addr_not_consumed_ >> i) & 1u) == 1u) return false;
      }
    }
    // If here and we found address then it must have been consumed ok - return true
    // If here and we didn't find address then bad address/PFE unset - return false
    return found;
  }

  void MauAddrDist::addrs_leftover_check() {
    // Check for unconsumed addresses and throw an error if any addresses are
    // distributed but not used.
    // An error is NOT thrown if the unconsumed address has index 1023. This
    // caters for two special cases:
    // 1. For TofinoXX: There is a bug in snapshot that cannot distinguish if a
    // gateway is inhibiting a table. To work around this, the
    // gateway_payload_match_adr is configured to an invalid value with index
    // == 0x3FF (1023). So if we see this index on Tofino we DON'T complain the
    // address is unconsumed (XXX).
    // 2. XXX, XXX - This index may be used to disable a direct
    // counter/meter/register with no per-flow enable when a table is inhibited
    // by a gateway.
    const int special_case_index = 1023; // 0x3FF sometimes (ab)used
    if (action_addr_not_consumed_ != 0u) {
      for (int i = 0; i < kLogicalRows; i++) {
        if ((action_addr_not_consumed_ & (1u<<i)) != 0u) {
          int index = Address::action_addr_get_index(action_addrs_[i]);
          if (index != special_case_index) {
            bool relax = (kRelaxAllAddrsConsumedCheck || kRelaxActionAddrsConsumedCheck);
            RMT_LOG_OBJ(mau_, RmtDebug::error(relax),
                        "MauAddrDist: Action addr 0x%08x on row %d not consumed (index %d)\n",
                        action_addrs_[i], i, index);
            mau_->mau_info_incr(MAU_ACT_DATA_ADDRS_UNCONSUMED);
            if (!relax) { THROW_ERROR(-2); }
          }
        }
      }
    }
    if (stats_addr_not_consumed_ != 0u) {
      for (int i = 0; i < kLogicalRows; i++) {
        if ((stats_addr_not_consumed_ & (1u<<i)) != 0u) {
          int index = Address::stats_addr_get_index(stats_addrs_[i]);
          if (index != special_case_index) {
            bool relax = (kRelaxAllAddrsConsumedCheck || kRelaxStatsAddrsConsumedCheck);
            RMT_LOG_OBJ(mau_, RmtDebug::error(relax),
                        "MauAddrDist: Stats addr 0x%08x on row %d not consumed (index %d)\n",
                        stats_addrs_[i], i, index);
            mau_->mau_info_incr(MAU_STATS_ADDRS_UNCONSUMED);
            if (!relax) { THROW_ERROR(-2); }
          }
        }
      }
    }
    if (meter_addr_not_consumed_ != 0u) {
      for (int i = 0; i < kLogicalRows; i++) {
        if ((meter_addr_not_consumed_ & (1u<<i)) != 0u) {
          int index = Address::meter_addr_get_index(meter_addrs_[i]);
          if (index != special_case_index) {
            bool relax = (kRelaxAllAddrsConsumedCheck || kRelaxMeterAddrsConsumedCheck);
            RMT_LOG_OBJ(mau_, RmtDebug::error(relax),
                        "MauAddrDist: Meter addr 0x%08x on row %d not consumed (index %d)\n",
                        meter_addrs_[i], i, index);
            mau_->mau_info_incr(MAU_METER_ADDRS_UNCONSUMED);
            if (!relax) { THROW_ERROR(-2); }
          }
        }
      }
    }
    if (idletime_addr_not_consumed_ != 0u) {
      for (int i = 0; i < kIdletimeBuses; i++) {
        if ((idletime_addr_not_consumed_ & (1u<<i)) != 0u) {
          int index = Address::idletime_addr_get_index(idletime_addrs_[i]);
          if (index != special_case_index) {
            bool relax = (kRelaxAllAddrsConsumedCheck || kRelaxIdletimeAddrsConsumedCheck);
            RMT_LOG_OBJ(mau_, RmtDebug::error(relax),
                        "MauAddrDist: Idletime addr 0x%08x on bus %d not consumed (index %d)\n",
                        idletime_addrs_[i], i, index);
            mau_->mau_info_incr(MAU_IDLETIME_ADDRS_UNCONSUMED);
            if (!relax) { THROW_ERROR(-2); }
          }
        }
      }
    }
  }

  void MauAddrDist::meter_synth2port_fabric_check(int alu,int lr) {
    RMT_ASSERT( alu>=0 && alu<=kNumMeterAlus );
    synth2port_fabric_check(alu,meter_addrclaims_[alu],lr,false);
  }
  void MauAddrDist::stats_synth2port_fabric_check(int alu,int lr) {
    RMT_ASSERT( alu>=0 && alu<=kNumStatsAlus );
    synth2port_fabric_check(alu,stats_addrclaims_[alu],lr,true);
  }
void MauAddrDist::synth2port_fabric_check(int alu, const AddrClaim& claim,int lr,bool is_stats) {
    RMT_ASSERT( lr & 1 ); // logical row must be on rhs as this is where all alus are
    bool relax = false;

    // TODO: remove this, it is just in for debug info
    //for (int r = 0; r < kLogicalRows; ++r) RMT_LOG_OBJ(mau_,RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistMeter),"MauAddrDist:synth2port_fabric_check log_row %d claim_bm=0x%02x\n",r,claim.get_claimant_columns(r));


    // check there are no claiming srams above the alu
    for (int r = kLogicalRows - 1; r > lr; --r) {
      if ( claim.get_claimant_columns(r) != 0 ) {
        RMT_LOG_OBJ(mau_,RmtDebug::error(relax),
                    "MauAddrDist:synth2port_fabric_check ram on log_row %d above alu on log_row %d\n",
                    r, lr);
      }
    }
    int top_row = lr;
    int bottom_row = lr;
    for (int r = top_row; r >= 0; --r) {
      if ( claim.get_claimant_columns(r) != 0 ) {
        bottom_row = r;
      }
    }

    RMT_LOG_OBJ(mau_,RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistMeter),"MauAddrDist:synth2port_fabric_check %s alu=%d | top_log_row %d | bottom_log_row %d\n",is_stats?"stats":"meter",alu,top_row, bottom_row);


    for (int r = top_row; r >= bottom_row; r -= kLogicalRowsPerPhysicalRow) {
      int phys_row = mau_->physical_row_index(r);
      MauSramRow* row = mau_->sram_row_lookup(phys_row);
      RMT_ASSERT(row);
      RMT_LOG_OBJ(mau_,RmtDebug::verbose(RmtDebug::kRmtDebugMauAddrDistMeter),"MauAddrDist:synth2port_fabric_check r=%d phys_row=%d\n",r,phys_row);
      row->synth2port_fabric_check( is_stats,alu,
                                    r == top_row,
                                    r == bottom_row,
                                    claim.get_claimant_columns(r) );
    }
  }


  bool SweepInfo::set_vpn_range(MauAddrDist *mad,
                                uint8_t min_vpn, uint8_t max_vpn, int8_t hole_pos) {
    RMT_ASSERT((min_vpn != 0xFF) && (max_vpn != 0xFF));
    if ((min_vpn == min_vpn_) && (max_vpn == max_vpn_) && (hole_pos == hole_pos_))
      return false; // Nothing changed so bail

    // Check config is sane
    if (!holevpn_check(min_vpn, max_vpn, hole_pos)) {
      RMT_LOG_OBJ(mad->mau(), RmtDebug::error(RmtDebug::kRmtDebugMauAddrDistVpn),
                  "SweepInfo::set_vpn_range: Invalid VPN min/max/hole config - "
                  "(TYP=%d LT=%d ALU=%d min_vpn=%d(0x%02x) max_vpn=%d(0x%02x) holepos=%d)\n",
                  type_, logical_table_, alu_, min_vpn, min_vpn, max_vpn, max_vpn, hole_pos);
      THROW_ERROR(-2); // For DV
    }

    vpn_valid_mask_ = UINT64_C(0);
    for (uint8_t i = 0; i < kVpnMax; i++) {
      seq_to_vpn_[i] = 0xFF; vpn_to_seq_[i] = 0xFF;
    }
    min_vpn_ = 0xFF; max_vpn_ = 0xFF; hole_pos_ = -1;
    uint8_t n = 0;
    if (hole_pos >= 0) {
      min_vpn = holevpn_init_min(min_vpn, hole_pos);
      uint8_t vpn = min_vpn;
      while (true) {
        RMT_ASSERT(vpn >= min_vpn);
        seq_to_vpn_[n] = vpn; vpn_to_seq_[vpn] = n; n++;
        vpn_valid_mask_ |= (UINT64_C(1) << vpn);
        if (vpn == max_vpn) break; // Stop once we hit max_vpn
        vpn = holevpn_incr(vpn, hole_pos);
      }
    } else {
      // Trivial sequence [0]=min_vpn [1]=min_vpn+1 etc etc
      for (uint8_t vpn = min_vpn; vpn <= max_vpn; vpn++) {
        seq_to_vpn_[n] = vpn; vpn_to_seq_[vpn] = n; n++;
        vpn_valid_mask_ |= (UINT64_C(1) << vpn);
      }
    }
    min_vpn_ = min_vpn; max_vpn_ = max_vpn; hole_pos_ = hole_pos;
    return true;
  }


  // Clear mapram ordering
  void SweepInfo::maprams_clear_ordering() {
    auto it = maprams_.begin();
    while (it != maprams_.end()) {
      MauMapram *mapram = *it;
      mapram->set_order(0);
      ++it;
    }
  }
  // Order maprams according to vpn_to_seq
  void SweepInfo::maprams_set_ordering() {
    auto it = maprams_.begin();
    while (it != maprams_.end()) {
      MauMapram *mapram = *it;
      int mapram_vpn = mapram->get_vpn();
      int order = (vpn_valid(mapram_vpn)) ?vpn_to_seq(mapram_vpn) :0xFF;
      mapram->set_order(order);
      ++it;
    }
  }


  // Handle idle dump for SINGLE LogicalTable
  void SweepInfo::idle_dump(bool clear) {
    // Call idletime_dump_all in each MAP RAM
    // stopping if we get to end of address-space
    auto it = maprams_.begin();
    while (it != maprams_.end()) {
      MauMapram *mapram = *it;
      int mapram_vpn = mapram->get_vpn();
      if (vpn_valid(mapram_vpn)) mapram->idletime_dump_all(clear);
      ++it;
    }
  }

  // Handle idle dump for single addr within single LogicalTable
  void SweepInfo::idle_dump_word(uint32_t addr, bool clear) {
    // Set OP to be cfg_rd or cfg_rd_clr - not strictly necessary
    // since we call idletime_dump_one direct here
    int op = (clear) ?Address::kIdletimeOpCfgRdClr :Address::kIdletimeOpCfgRd;
    addr = Address::idletime_addr_make(addr, op);

    // Call idletime_dump_one on first MAP RAM
    // that claims to handle address
    auto it = maprams_.begin();
    while (it != maprams_.end()) {
      MauMapram *mapram = *it;
      int mapram_vpn = mapram->get_vpn();
      if ((vpn_valid(mapram_vpn)) && (mapram->idletime_handles_addr(addr))) {
        mapram->idletime_dump_one(Address::idletime_addr_get_index(addr), clear);
        break;
      }
      ++it;
    }
  }

  // Handle idle sweep for SINGLE LogicalTable
  void SweepInfo::idle_sweep(MauAddrDist *mad, int lt, uint64_t t_now_psecs) {
    // Table not enabled OR empty vpn range so return
    if ((!enabled_) || (min_vpn_ == 0xFF) || (max_vpn_ == 0xFF)) return;

    idle_sweep_start(mad, lt);

    // Now call idletime_sweep_all in each MAP RAM
    // stopping if we get to end of address-space for table
    bool now_idle = true;
    auto it = maprams_.begin();
    while (it != maprams_.end()) {
      MauMapram *mapram = *it;
      RMT_ASSERT(mapram != NULL);
      int mapram_vpn = mapram->get_vpn();
      if (vpn_valid(mapram_vpn)) {
        if (!mapram->idletime_sweep_all()) now_idle = false;
      }
      ++it;
    }

    // Maybe upcall to tell higher level code we're idle
    idle_sweep_end(mad, lt, now_idle);
  }


  // Handle hit in logical table
  // If table was previously idle upcall to tell higher level
  // code we're active again
  void SweepInfo::idle_hit(MauAddrDist *mad, int lt) {
    spinlock_.lock();
    bool was_idle = idle_;
    hit_ = true;
    idle_ = false;
    if (was_idle) mad->idletime_sweeper_upcall(lt, false);
    spinlock_.unlock();
  }


  // Lets higher level code trigger sweeper upcall
  void SweepInfo::idle_sweep_start(MauAddrDist *mad, int lt) {
    spinlock_.lock();
    hit_ = false;
    spinlock_.unlock();
  }
  void SweepInfo::idle_sweep_end(MauAddrDist *mad, int lt, bool now_idle) {
    if (!now_idle) return;
    //printf("IDLE_SWEEP_END - hit=%d - may upcall for table %d\n", hit_, lt);
    spinlock_.lock();
    if (!hit_) {
      idle_ = true;
      mad->idletime_sweeper_upcall(lt, true);
    }
    spinlock_.unlock();
  }


}
