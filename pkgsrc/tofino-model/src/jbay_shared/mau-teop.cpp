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

// MauTeop - JBay specific implementation

#include <mau.h>
#include <register_adapters.h>
#include <mau-teop.h>

namespace MODEL_CHIP_NAMESPACE {

MauTeop::MauTeop(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau)
    : MauTeopCommon(om, pipeIndex, mauIndex, mau),
      mau_(mau), prev_mau_teop_(NULL), next_mau_teop_(NULL),
      min_linked_mau_index_(mau->mau_index()), // self initially
      adr_dist_meter_icxbar_(default_adapter(adr_dist_meter_icxbar_, chip_index(), pipeIndex, mauIndex,
                                             [this](uint32_t i){this->register_change_callback(i);})),
      adr_dist_stats_icxbar_(default_adapter(adr_dist_stats_icxbar_, chip_index(), pipeIndex, mauIndex,
                                             [this](uint32_t i){this->register_change_callback(i);})),
      mau_ad_meter_virt_lt_(default_adapter(mau_ad_meter_virt_lt_, chip_index(), pipeIndex, mauIndex,
                                             [this](uint32_t i){this->register_change_callback(i);})),
      mau_ad_stats_virt_lt_(default_adapter(mau_ad_stats_virt_lt_, chip_index(), pipeIndex, mauIndex,
                                             [this](uint32_t i){this->register_change_callback(i);})),
      deferred_ram_ctl_(default_adapter(deferred_ram_ctl_, chip_index(), pipeIndex, mauIndex)),
      meter_color_logical_to_phys_icxbar_(default_adapter(meter_color_logical_to_phys_icxbar_,chip_index(), pipeIndex, mauIndex)),

      dp_teop_meter_ctl_(default_adapter(dp_teop_meter_ctl_, chip_index(), pipeIndex, mauIndex)),
      dp_teop_stats_ctl_(default_adapter(dp_teop_stats_ctl_, chip_index(), pipeIndex, mauIndex)),
      teop_to_meter_adr_oxbar_ctl_(default_adapter(teop_to_meter_adr_oxbar_ctl_, chip_index(), pipeIndex, mauIndex,
                                                   [this](uint32_t i){this->register_change_callback(i);})),
      teop_to_stats_adr_oxbar_ctl_(default_adapter(teop_to_stats_adr_oxbar_ctl_, chip_index(), pipeIndex, mauIndex,
                                                   [this](uint32_t i){this->register_change_callback(i);})),
      meter_to_teop_adr_oxbar_ctl_(default_adapter(meter_to_teop_adr_oxbar_ctl_, chip_index(), pipeIndex, mauIndex,
                                                   [this](uint32_t i){this->register_change_callback(i);})),
      stats_to_teop_adr_oxbar_ctl_(default_adapter(stats_to_teop_adr_oxbar_ctl_, chip_index(), pipeIndex, mauIndex,
                                                   [this](uint32_t i){this->register_change_callback(i);})),
      teop_bus_ctl_(default_adapter(teop_bus_ctl_, chip_index(), pipeIndex, mauIndex))
{
  // Invalidate ALL cached bus values
  for (int lt = 0; lt < kTables; lt++) {
    teoptime_buses_[lt] = hdrtime_buses_[lt] = 0xFF;
  }
  teoptime_buses_here_ = hdrtime_buses_here_ = 0xFF;
  teoptime_buses_earlier_ = hdrtime_buses_earlier_ = 0xFF;

  adr_dist_meter_icxbar_.reset();
  adr_dist_stats_icxbar_.reset();
  mau_ad_meter_virt_lt_.reset();
  mau_ad_stats_virt_lt_.reset();
  deferred_ram_ctl_.reset();
  meter_color_logical_to_phys_icxbar_.reset();

  dp_teop_meter_ctl_.reset();
  dp_teop_stats_ctl_.reset();
  teop_to_meter_adr_oxbar_ctl_.reset();
  teop_to_stats_adr_oxbar_ctl_.reset();
  meter_to_teop_adr_oxbar_ctl_.reset();
  stats_to_teop_adr_oxbar_ctl_.reset();
  teop_bus_ctl_.reset();

  for (int alu = 0; alu < kNumMeterAlus; alu++) {
    meter_ctl_teop_en_[alu] = default_adapter_new(meter_ctl_teop_en_[alu],
                                                  chip_index(), pipeIndex, mauIndex, alu);
    meter_ctl_teop_en_[alu]->reset();
  }
  for (int alu = 0; alu < kNumStatsAlus; alu++) {
    statistics_ctl_teop_en_[alu] = default_adapter_new(statistics_ctl_teop_en_[alu],
                                                       chip_index(), pipeIndex, mauIndex, alu);
    statistics_ctl_teop_en_[alu]->reset();
  }
}
MauTeop::~MauTeop() {
  for (int alu = 0; alu < kNumMeterAlus; alu++) {
    if (meter_ctl_teop_en_[alu] != NULL) delete meter_ctl_teop_en_[alu];
    meter_ctl_teop_en_[alu] = NULL;
  }
  for (int alu = 0; alu < kNumStatsAlus; alu++) {
    if (statistics_ctl_teop_en_[alu] != NULL) delete statistics_ctl_teop_en_[alu];
    statistics_ctl_teop_en_[alu] = NULL;
  }
}


void MauTeop::set_prev_teop(MauTeop *prev_teop) {
  if (prev_mau_teop_ == prev_teop) return; // Bail if already done
  MauTeop *prev_teop_0 = prev_mau_teop_;
  prev_mau_teop_ = prev_teop;
  if (prev_teop_0 != NULL) prev_teop_0->set_next_teop(NULL);
  if (prev_teop != NULL) prev_teop->set_next_teop(this);
}
void MauTeop::set_next_teop(MauTeop *next_teop) {
  if (next_mau_teop_ == next_teop) return; // Bail if already done
  MauTeop *next_teop_0 = next_mau_teop_;
  next_mau_teop_ = next_teop;
  if (next_teop_0 != NULL) next_teop_0->set_prev_teop(NULL);
  if (next_teop != NULL) next_teop->set_prev_teop(this);
}
uint8_t MauTeop::link_teop() {
  if (min_linked_mau_index_ == 0) return 0; // Bail if fully linked
  Mau *prev_mau = mau_->mau_previous();
  set_prev_teop((prev_mau != NULL) ?prev_mau->mau_teop() :NULL);
  if (prev_mau_teop_ == NULL) return min_linked_mau_index_;
  uint8_t new_min = prev_mau_teop_->link_teop();
  if (min_linked_mau_index_ > new_min) min_linked_mau_index_ = new_min;
  return min_linked_mau_index_;
}
void MauTeop::register_change_callback(int lt_alu_bus) {
  if (lt_alu_bus >= 0) {
    // Clear this stage cached bus values
    for (int lt = 0; lt < kTables; lt++) {
      hdrtime_buses_[lt] = teoptime_buses_[lt] = 0xFF;
    }
    hdrtime_buses_here_ = teoptime_buses_here_ = 0xFF;
  } else {
    // Clear earlier stage cached bus values
    hdrtime_buses_earlier_ = teoptime_buses_earlier_ = 0xFF;
  }
  // Call downstream MauTeop to tell it registers have changed
  if (next_mau_teop_ != NULL) next_mau_teop_->register_change_callback(-1);
}




uint8_t MauTeop::teop_buses_at_hdrtime(int lt) {
  RMT_ASSERT((lt >= 0) && (lt < kTables));
  uint8_t  buses = 0;
  uint32_t m_alus = adr_dist_meter_icxbar_.adr_dist_meter_adr_icxbar_ctl(lt);
  uint32_t s_alus = adr_dist_stats_icxbar_.adr_dist_stats_adr_icxbar_ctl(lt);

  // Look in adr_dist_meter_icxbar[LT] and adr_dist_stats_icxbar[LT] for ALUs
  // If any of these ALUs output to TEOP BUS return true
  //
  for (int bus = 0; bus < kNumTeopBuses; bus++) {
    if (meter_to_teop_adr_oxbar_ctl_.enabled_2bit_muxctl_enable(bus) == 1) {
      int alu = meter_to_teop_adr_oxbar_ctl_.enabled_2bit_muxctl_select(bus);
      if (((m_alus >> alu) & 1) == 1) buses |= 1<<bus;
    }
    if (stats_to_teop_adr_oxbar_ctl_.enabled_2bit_muxctl_enable(bus) == 1) {
      int alu = stats_to_teop_adr_oxbar_ctl_.enabled_2bit_muxctl_select(bus);
      if (((s_alus >> alu) & 1) == 1) buses |= 1<<bus;
    }
  }
  return buses;
}
uint8_t MauTeop::teop_buses_at_teoptime(int lt) {
  RMT_ASSERT((lt >= 0) && (lt < kTables));
  uint8_t  buses = 0;
  // If any ALUs at all input from TEOP BUS then
  //  If any of these ALUs have lt in mau_ad_stats|meter_virt_lt[ALU] return true
  //
  for (int alu = 0; alu < kNumMeterAlus; alu++) {
    if (teop_to_meter_adr_oxbar_ctl_.enabled_2bit_muxctl_enable(alu) == 1) {
      uint8_t  bus = teop_to_meter_adr_oxbar_ctl_.enabled_2bit_muxctl_select(alu);
      uint32_t m_lts = mau_ad_meter_virt_lt_.mau_ad_meter_virt_lt(alu);
      if (((m_lts >> lt) & 1) == 1) buses |= 1<<bus;
    }
  }
  for (int alu = 0; alu < kNumStatsAlus; alu++) {
    if (teop_to_stats_adr_oxbar_ctl_.enabled_2bit_muxctl_enable(alu) == 1) {
      uint8_t  bus = teop_to_stats_adr_oxbar_ctl_.enabled_2bit_muxctl_select(alu);
      uint32_t s_lts = mau_ad_stats_virt_lt_.mau_ad_stats_virt_lt(alu);
      if (((s_lts >> lt) & 1) == 1) buses |= 1<<bus;
    }
  }
  return buses;
}



// Maybe return cached values
uint8_t MauTeop::teop_buses_at_hdrtime_cached(int lt) {
  if (hdrtime_buses_[lt] != 0xFF) return hdrtime_buses_[lt];
  hdrtime_buses_[lt] = teop_buses_at_hdrtime(lt);
  return hdrtime_buses_[lt];
}
uint8_t MauTeop::teop_buses_at_teoptime_cached(int lt) {
  if (teoptime_buses_[lt] != 0xFF) return teoptime_buses_[lt];
  teoptime_buses_[lt] = teop_buses_at_teoptime(lt);
  return teoptime_buses_[lt];
}
uint8_t MauTeop::teop_buses_at_hdrtime() {
  uint8_t buses = 0;
  for (int lt = 0; lt < kTables; lt++) buses |= teop_buses_at_hdrtime_cached(lt);
  return buses;
}
uint8_t MauTeop::teop_buses_at_teoptime() {
  uint8_t buses = 0;
  for (int lt = 0; lt < kTables; lt++) buses |= teop_buses_at_teoptime_cached(lt);
  return buses;
}
uint8_t MauTeop::teop_buses_at_hdrtime_cached() {
  if (hdrtime_buses_here_ != 0xFF) return hdrtime_buses_here_;
  hdrtime_buses_here_ = teop_buses_at_hdrtime();
  return hdrtime_buses_here_;
}
uint8_t MauTeop::teop_buses_at_teoptime_cached() {
  if (teoptime_buses_here_ != 0xFF) return teoptime_buses_here_;
  teoptime_buses_here_ = teop_buses_at_teoptime();
  return teoptime_buses_here_;
}
uint8_t MauTeop::teop_buses_at_hdrtime_in_earlier_stage() {
  if (prev_mau_teop_ == NULL) return 0;
  return (prev_mau_teop_->teop_buses_at_hdrtime_cached() |
          prev_mau_teop_->teop_buses_at_hdrtime_in_earlier_stage());
}
uint8_t MauTeop::teop_buses_at_teoptime_in_earlier_stage() {
  if (prev_mau_teop_ == NULL) return 0;
  return (prev_mau_teop_->teop_buses_at_teoptime_cached() |
          prev_mau_teop_->teop_buses_at_teoptime_in_earlier_stage());
}
uint8_t MauTeop::teop_buses_at_hdrtime_in_earlier_stage_cached() {
  if (hdrtime_buses_earlier_ != 0xFF) return hdrtime_buses_earlier_;
  hdrtime_buses_earlier_ = teop_buses_at_hdrtime_in_earlier_stage();
  return hdrtime_buses_earlier_;
}
uint8_t MauTeop::teop_buses_at_teoptime_in_earlier_stage_cached() {
  if (teoptime_buses_earlier_ != 0xFF) return teoptime_buses_earlier_;
  teoptime_buses_earlier_ = teop_buses_at_teoptime_in_earlier_stage();
  return teoptime_buses_earlier_;
}




uint8_t MauTeop::teop_delay_required(int bus) {
  MauDependencies *deps = mau_->mau_dependencies();
  bool mdep = deps->egress_is_match_dependent();

  (void)link_teop(); // Link to upstream TEOPs if not yet done so

  if ( mdep && (((teop_buses_at_hdrtime_in_earlier_stage_cached() >> bus) & 1) == 1) ) {
    uint8_t pipe_latency = deps->get_delay(false, MauDelay::kPipeLatency); // false=>egress
    return (pipe_latency >= 4) ?pipe_latency - 4 :99;
  }
  else if (((teop_buses_at_hdrtime_cached() >> bus) & 1) == 1) {
    uint8_t pipe_latency = deps->get_delay(false, MauDelay::kPipeLatency); // false=>egress
    uint8_t pred_delay = deps->get_delay(false, MauDelay::kPredication);
    RMT_ASSERT((pipe_latency > pred_delay) && "Bad pipe latency OR pred_delay");
    return (pipe_latency >= pred_delay + 7) ?pipe_latency - pred_delay - 7 :99;
  }
  return 0;
}
bool MauTeop::teop_delay_check(int bus) {
  uint8_t del_exp = teop_delay_required(bus);
  uint8_t del_act = 0;
  if (teop_bus_ctl_.teop_bus_ctl_delay_en(bus) == 1)
    del_act = teop_bus_ctl_.teop_bus_ctl_delay(bus);
  if (del_act != del_exp) {
    RMT_LOG_OBJ(mau_, RmtDebug::error(MauDependencies::kRelaxDelayCheck),
                "TeopDelayCheck<%d> TeopBus[%d] delay set to "
                "%d - expected %d (en=%d)"
                "(TeopHere=0x%x)(TeopEarlier=0x%x)\n",
                mau_->mau_index(), bus, del_act, del_exp,
                teop_bus_ctl_.teop_bus_ctl_delay_en(bus),
                teop_buses_at_hdrtime_cached(),
                teop_buses_at_hdrtime_in_earlier_stage_cached());
  }
  return (del_act == del_exp);
}



bool MauTeop::teop_available() {
  return true;
}
Teop *MauTeop::teop_allocate() {
  return new Teop();
}
void MauTeop::teop_free(Teop *teop) {
  if (teop != NULL) delete teop;
}
bool MauTeop::teop_being_used(int lt) {
  return ((teop_buses_at_hdrtime_cached(lt) != 0) ||
          (teop_buses_at_teoptime_cached(lt) != 0));
}
bool MauTeop::teop_being_used() {
  return ((teop_buses_at_hdrtime_cached() != 0) ||
          (teop_buses_at_teoptime_cached() != 0));
}
bool MauTeop::teop_enabled(int s_or_m, int alu) {
  if (s_or_m == 0) {
    RMT_ASSERT((alu >= 0) && (alu < kAlus) && (alu < kNumStatsAlus));
    return (dp_teop_stats_ctl_.dp_teop_stats_ctl_rx_en(alu) == 1);
  } else if (s_or_m == 1) {
    RMT_ASSERT((alu >= 0) && (alu < kAlus) && (alu < kNumMeterAlus));
    return (dp_teop_meter_ctl_.dp_teop_meter_ctl_rx_en(alu) == 1);
  } else {
    RMT_ASSERT(0);
  }
}
bool MauTeop::teop_input_meter_addr(const Teop &teop, int alu,
                                    uint32_t *meter_addr, int *lt) {
  RMT_ASSERT(teop.teop_time());
  RMT_ASSERT((alu >= 0) && (alu < kAlus) && (alu < kNumMeterAlus));
  RMT_ASSERT(meter_addr != NULL);

  // Look at teop_to_meter_adr_oxbar_ctl[ALU] to see if listening to a TEOP bus (rx_en)
  // Insist meter ALU getting address from TEOP
  // Insist meter_ctl_teop_en[ALU] is set
  // Insist at least one LT appears in mau_ad_meter_virt_lt[ALU]
  // Insist deferred_ram_ctl[meter,ALU] is 0
  // Check that teop bus is enabled with a meter addr
  // If listening for meter extract addr possibly shifting (rx_shift=0,1,2)
  // Count/Squash handling
  //   Figure out what errors in teop - None,FCS,Trunc
  //   switch (dp_teop_meter_ctl.dp_teop_meter_ctl_err) and count appropriately
  // If counting
  //   *meter_addr = (teop.addr() << 7) >> rx_shift;
  //   get color from TEOP, make into OP4 and OR into meter_addr with meter_addr_make
  //   return true
  //
  uint32_t meter_lts = mau_ad_meter_virt_lt_.mau_ad_meter_virt_lt(alu);
  if (teop_to_meter_adr_oxbar_ctl_.enabled_2bit_muxctl_enable(alu) == 0) return false;
  if (dp_teop_meter_ctl_.dp_teop_meter_ctl_rx_en(alu) == 0) return false;
  if (meter_ctl_teop_en_[alu]->meter_ctl_teop_en() == 0) return false;

  int bus = teop_to_meter_adr_oxbar_ctl_.enabled_2bit_muxctl_select(alu);
  if (!teop.meter_en(bus)) return false;

  if (meter_lts == 0) {
    RMT_LOG_OBJ(mau_, RmtDebug::error(),
                "TeopInputMeter<%d> MeterALU %d has no LTs in mau_ad_meter_virt_lt\n",
                mau_->mau_index(), alu);
    return false;
  }
  if (deferred_ram_ctl_.deferred_ram_en(1,alu) == 1) {
    RMT_LOG_OBJ(mau_, RmtDebug::error(),
                "TeopInputMeter<%d> MeterALU %d configured for EOP and TEOP!\n",
                mau_->mau_index(), alu);
    return false;
  }
  int shift = dp_teop_meter_ctl_.dp_teop_meter_ctl_rx_shift(alu);
  RMT_ASSERT((shift <= 2) && "Bad dp_teop_meter_ctl_rx_shift");

  bool use_addr = false;
  switch (dp_teop_meter_ctl_.dp_teop_meter_ctl_err(alu)) {
    case 0:  case 3:  case 7:                      use_addr = true; break;
    case 1:  use_addr = ( teop.error_fcs() && !teop.error_trunc()); break;
    case 2:  use_addr = (!teop.error_fcs() &&  teop.error_trunc()); break;
    case 4:  use_addr = (!teop.error_fcs() && !teop.error_trunc()); break;
    case 5:  use_addr = (!teop.error_fcs()                       ); break;
    case 6:  use_addr = (                     !teop.error_trunc()); break;
    default: RMT_ASSERT(0);
  }
  if (!use_addr) return false;

  uint8_t  m_op4  = Address::meter_color_get_color_op4(teop.color(bus));
  uint32_t m_addr = (teop.addr(bus) << 7) >> shift;
  *meter_addr = Address::meter_addr_make(m_addr, m_op4);
  if (lt != NULL) *lt = __builtin_ffs(meter_lts) - 1;
  return true;
}
bool MauTeop::teop_input_stats_addr(const Teop &teop, int alu,
                                    uint32_t *stats_addr, int *lt) {
  RMT_ASSERT(teop.teop_time());
  RMT_ASSERT((alu >= 0) && (alu < kAlus) && (alu < kNumStatsAlus));
  RMT_ASSERT(stats_addr != NULL);

  // Look at teop_to_stats_adr_oxbar_ctl[ALU] to see if listening to a TEOP bus (rx_en)
  // Insist stats ALU getting address from TEOP
  // Insist statistics_ctl_teop_en[ALU] is set
  // Insist at least one LT appears in mau_ad_stats_virt_lt[ALU]
  // Check that teop bus is enabled with a stats addr
  // Insist deferred_ram_ctl[stats,ALU] is 0
  // If listening for stats extract addr possibly shifting (rx_shift=0,1,2,3,4)
  // Count/Squash handling
  //   Figure out what errors in teop - None,FCS,Trunc
  //   switch (dp_teop_stats_ctl.dp_teop_stats_ctl_err) and count appropriately
  // If counting
  //   *stats_addr = (teop.addr() << 3) >> rx_shift;
  //   return true
  //
  uint32_t stats_lts = mau_ad_stats_virt_lt_.mau_ad_stats_virt_lt(alu);
  if (teop_to_stats_adr_oxbar_ctl_.enabled_2bit_muxctl_enable(alu) == 0) return false;
  if (dp_teop_stats_ctl_.dp_teop_stats_ctl_rx_en(alu) == 0) return false;
  if (statistics_ctl_teop_en_[alu]->statistics_ctl_teop_en() == 0) return false;

  int bus = teop_to_stats_adr_oxbar_ctl_.enabled_2bit_muxctl_select(alu);
  if (!teop.stats_en(bus)) return false;

  if (stats_lts == 0) {
    RMT_LOG_OBJ(mau_, RmtDebug::error(),
                "TeopInputStats<%d> StatsALU %d has no LTs in mau_ad_stats_virt_lt\n",
                mau_->mau_index(), alu);
    return false;
  }
  if (deferred_ram_ctl_.deferred_ram_en(0,alu) == 1) {
    RMT_LOG_OBJ(mau_, RmtDebug::error(),
                "TeopInputStats<%d> StatsALU %d configured for EOP and TEOP!\n",
                mau_->mau_index(), alu);
    return false;
  }
  int shift = dp_teop_stats_ctl_.dp_teop_stats_ctl_rx_shift(alu);
  RMT_ASSERT((shift <= 4) && "Bad dp_teop_stats_ctl_rx_shift");

  bool use_addr = false;
  switch (dp_teop_stats_ctl_.dp_teop_stats_ctl_err(alu)) {
    case 0:  case 3:  case 7:                      use_addr = true; break;
    case 1:  use_addr = ( teop.error_fcs() && !teop.error_trunc()); break;
    case 2:  use_addr = (!teop.error_fcs() &&  teop.error_trunc()); break;
    case 4:  use_addr = (!teop.error_fcs() && !teop.error_trunc()); break;
    case 5:  use_addr = (!teop.error_fcs()                       ); break;
    case 6:  use_addr = (                     !teop.error_trunc()); break;
    default: RMT_ASSERT(0);
  }
  if (!use_addr) return false;

  uint32_t s_addr = (teop.addr(bus) << 3) >> shift;
  *stats_addr = Address::stats_addr_make(s_addr, Address::kStatsOpStats);
  if (lt != NULL) *lt = __builtin_ffs(stats_lts) - 1;
  return true;
}
void MauTeop::teop_output_meter_addr(Teop *teop, int lt, bool ingress,
                                     int alu, uint32_t meter_addr) {
  if (teop == NULL) return;
  RMT_ASSERT(teop->hdr_time());
  RMT_ASSERT((lt >= 0) && (lt < kTables));
  RMT_ASSERT((alu >= 0) && (alu < kAlus) && (alu < kNumMeterAlus));
  if (ingress) return; // No such thing as ingress tEOP
  if (!Address::meter_addr_op_enabled(meter_addr)) return;
  if (!Address::meter_addr_op_deferrable(meter_addr)) return;

  // If ALU not set in adr_dist_meter_icxbar[LT] return
  // For each TEOP_BUS in 0-3
  //    Look at meter_to_teop_adr_oxbar_ctl[TEOP_BUS] to see if outputting from ALU
  //    If so
  //       Check if teop[TEOP_BUS] already in use - if so ERROR
  //         (note there can be multiple ALU users in stage but NOT at same time)
  //       Color - get color from meterOP OP4 - but maybe zeroise
  //         X=meter_color_logical_to_phys_icxbar_.meter_color_logical_to_phys_icxbar_ctl(lt);
  //         if (((X >> alu) & 1) != 1) color=0 - WARN??
  //       Overwrite teop[TEOP_BUS] with meter_addr
  //         (teop_bus_ctl determines whether we set stats_en/meter_en or BOTH)
  //       teop->set_meter_addr((meter_addr >> 7) & 0x3FFFF, stats_too, color)
  //         (stats_too determined by teop_bus_ctl_ I think)

  if (((adr_dist_meter_icxbar_.adr_dist_meter_adr_icxbar_ctl(lt) >> alu) & 1) == 0) return;

  bool zeroise_color = (((meter_color_logical_to_phys_icxbar_.
                          meter_color_logical_to_phys_icxbar_ctl(lt) >> alu) & 1) == 0);

  for (int bus = 0; bus < kNumTeopBuses; bus++) {
    if ((meter_to_teop_adr_oxbar_ctl_.enabled_2bit_muxctl_enable(bus) == 1) &&
        (meter_to_teop_adr_oxbar_ctl_.enabled_2bit_muxctl_select(bus) == alu)) {

      if (!teop->ok_to_use(bus, mau_->mau_index(), lt, AddrType::kMeter)) {
        RMT_LOG_OBJ(mau_, RmtDebug::error(),
                    "TeopOutputMeter<%d> TeopBus[%d] already in use! "
                    "(used by MAU=%d,LT=%d for AddrType=%d)\n",
                    mau_->mau_index(), bus, teop->stage(bus), teop->lt(bus),
                    teop->addrtype(bus));
      } else {
        teop_delay_check(bus);

        bool stats_en = (teop_bus_ctl_.teop_bus_ctl_stats_en(bus) == 1);
        bool meter_en = (teop_bus_ctl_.teop_bus_ctl_meter_en(bus) == 1);
        uint8_t m_op4 = Address::meter_addr_op4(meter_addr);
        int     color = Address::meter_color_op4_get_color(m_op4);
        if ((color < 0) || (zeroise_color)) color = 0;
        RMT_ASSERT((stats_en || meter_en) &&
                   "One of teop_bus_ctl_stats|meter_en should be set for in-use bus");

        uint32_t bus_addr = (meter_addr & Address::kMeterAddrAddrMask) >> 7;
        teop->set_addr(bus, bus_addr, stats_en, meter_en, color);
        // Next func just for debugging clashes
        teop->set_stage_lt(bus, mau_->mau_index(), lt, AddrType::kMeter);
      }
    }
  }
}
void MauTeop::teop_output_stats_addr(Teop *teop, int lt, bool ingress,
                                     int alu, uint32_t stats_addr) {
  if (teop == NULL) return;
  RMT_ASSERT(teop->hdr_time());
  RMT_ASSERT((lt >= 0) && (lt < kTables));
  RMT_ASSERT((alu >= 0) && (alu < kAlus) && (alu < kNumStatsAlus));
  if (ingress) return; // No such thing as ingress tEOP
  if (!Address::stats_addr_op_enabled(stats_addr)) return;
  if (!Address::stats_addr_op_deferrable(stats_addr)) return;

  // If ALU not set in adr_dist_stats_icxbar[LT] return
  // For each TEOP_BUS in 0-3
  //    Look at stats_to_teop_adr_oxbar_ctl[TEOP_BUS] to see if outputting from ALU
  //    If so
  //       Check if teop[TEOP_BUS] already in use - if so ERROR
  //         (note there can be multiple ALU users in stage but NOT at same time)
  //       Overwrite teop[TEOP_BUS] with stats_addr
  //         (teop_bus_ctl determines whether we set stats_en/meter_en or BOTH)
  //       teop->set_stats_addr((stats_addr >> 1) & 0x3FFFF);
  //         (meter_too determined by teop_bus_ctl_ I think)

  if (((adr_dist_stats_icxbar_.adr_dist_stats_adr_icxbar_ctl(lt) >> alu) & 1) == 0) return;

  for (int bus = 0; bus < kNumTeopBuses; bus++) {
    if ((stats_to_teop_adr_oxbar_ctl_.enabled_2bit_muxctl_enable(bus) == 1) &&
        (stats_to_teop_adr_oxbar_ctl_.enabled_2bit_muxctl_select(bus) == alu)) {

      if (!teop->ok_to_use(bus, mau_->mau_index(), lt, AddrType::kStats)) {
        RMT_LOG_OBJ(mau_, RmtDebug::error(),
                    "TeopOutputStats<%d> TeopBus[%d] already in use! "
                    "(used by MAU=%d,LT=%d for AddrType=%d)\n",
                    mau_->mau_index(), bus, teop->stage(bus), teop->lt(bus),
                    teop->addrtype(bus));
      } else {
        teop_delay_check(bus);

        bool stats_en = (teop_bus_ctl_.teop_bus_ctl_stats_en(bus) == 1);
        bool meter_en = (teop_bus_ctl_.teop_bus_ctl_meter_en(bus) == 1);
        RMT_ASSERT((stats_en || meter_en) &&
                   "One of teop_bus_ctl_stats|meter_en should be set for in-use bus");

        uint32_t bus_addr = (stats_addr & Address::kStatsAddrAddrMask) >> 1;
        teop->set_addr(bus, bus_addr, stats_en, meter_en);
        // Next func just for debugging clashes
        teop->set_stage_lt(bus, mau_->mau_index(), lt, AddrType::kStats);
      }
    }
  }
}


}
