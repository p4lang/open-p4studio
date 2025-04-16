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
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-stateful-counters.h>
#include <register_adapters.h>



namespace MODEL_CHIP_NAMESPACE {


MauStatefulCounters::MauStatefulCounters(RmtObjectManager *om,
                                         int pipeIndex, int mauIndex, Mau *mau)
    : MauObject(om, pipeIndex, mauIndex, kType, mau),
      stateful_cntr_hole_en_(0),
      alu_vpn_change_(),
      mau_stateful_log_counter_array_(default_adapter(mau_stateful_log_counter_array_,chip_index(),pipeIndex,mauIndex)),
      mau_stateful_log_counter_ctl_(default_adapter(mau_stateful_log_counter_ctl_,chip_index(),pipeIndex,mauIndex)),
      mau_stateful_log_counter_clear_(default_adapter(mau_stateful_log_counter_clear_,chip_index(),pipeIndex,mauIndex,
                                      [this](){this->stateful_counter_clear_callback();})),
      mau_stateful_log_vpn_offset_(default_adapter(mau_stateful_log_vpn_offset_,chip_index(),pipeIndex,mauIndex,
                                   [this](uint32_t v){this->stateful_counter_vpn_callback(v);})),
      mau_stateful_log_vpn_limit_(default_adapter(mau_stateful_log_vpn_limit_,chip_index(),pipeIndex,mauIndex,
                                  [this](uint32_t w){this->stateful_counter_vpn_callback(w);})),
      mau_stateful_log_vpn_hole_en_(default_adapter(mau_stateful_log_vpn_hole_en_,chip_index(),pipeIndex,mauIndex,
                                    [this](){this->stateful_counter_vpn_en_callback();})),
      mau_stateful_log_instruction_width_(default_adapter(mau_stateful_log_instruction_width_,chip_index(),pipeIndex,mauIndex)),
      mau_stateful_log_ctl_ixbar_map_(default_adapter(mau_stateful_log_ctl_ixbar_map_,chip_index(),pipeIndex,mauIndex))
{
  mau_stateful_log_counter_array_.reset();
  mau_stateful_log_counter_ctl_.reset();
  mau_stateful_log_counter_clear_.reset();
  mau_stateful_log_vpn_offset_.reset();
  mau_stateful_log_vpn_limit_.reset();
  mau_stateful_log_vpn_hole_en_.reset();
  mau_stateful_log_instruction_width_.reset();
  mau_stateful_log_ctl_ixbar_map_.reset();

  // reset_stateful_cntr now only called from
  // mau_stateful_log_counter_clear_ callback
  //for (int j = 0; j < kNumAlus; j++) reset_stateful_cntr(j);
}
MauStatefulCounters::~MauStatefulCounters() {
}

bool MauStatefulCounters::stateful_counter_enabled(int lt) {
  return ((lt >= 0) && (lt < kLogicalTables) &&
          (get_type_stateful_cntr(lt) != CntrType::kDisabled));
}
bool MauStatefulCounters::stateful_counter_at_max(int alu) {
  uint32_t max_v = max_stateful_cntr(alu);
  uint32_t v = mau_stateful_log_counter_array_.mau_stateful_log_counter(alu,0);
  return (v == max_v);
}
uint32_t MauStatefulCounters::maybe_increment_stateful_counter(uint32_t addr, int lt,
                                                       const MauLookupResult &res) {
  RMT_ASSERT(res.logical_table() == lt);
  // If table NOT predicated on or addr NOT PFE-enabled addr return
  if (res.inactive() || !Address::meter_addr_op_enabled(addr)) return addr;
  // Check VPN config (if it has changed)
  maybe_check_vpn_range(lt);
  // Maybe increment counter (depends on counter type and res)
  bool do_inc = false;
  switch (get_type_stateful_cntr(lt)) {
    case CntrType::kDisabled:       do_inc = false;        break;
    case CntrType::kTableMiss:      do_inc = !res.match(); break;
    case CntrType::kTableHit:       do_inc = res.match();  break;
    case CntrType::kGatewayInhibit: do_inc = (res.gatewayran() && res.gatewayinhibit()); break;
    case CntrType::kUnconditional:  do_inc = true;         break;
    default: RMT_ASSERT(0); break;
  }
  // OR old counter into addr
  addr |= get_stateful_cntr_for_lt(lt);
  // Then do increment if required
  if (do_inc) incr_stateful_cntr(lt);
  if (do_inc) mau()->mau_info_incr(MAU_STATEFUL_LOG_COUNTERS_TICKED);
  return addr;
}



void MauStatefulCounters::stateful_counter_clear_callback() {
  uint32_t reg = mau_stateful_log_counter_clear_.mau_stateful_log_counter_clear();
  for (int i = 0; i < kNumAlus; i++) {
    if ((reg & (1u<<i)) != 0u) reset_stateful_cntr(i);
  }
}


void MauStatefulCounters::maybe_check_vpn_range(int lt) {
  int alu = which_stateful_alu(lt);
  if (alu < 0) return;
  bool vpn_change = false;
  spinlock();
  if (alu_vpn_change_[alu]) {
    vpn_change = true;
    alu_vpn_change_[alu] = false;
  }
  spinunlock();
  if (!vpn_change) return;
  // Get raw values vpn_offset, vpn_limit, hole_pos and check them
  uint8_t min_vpn = static_cast<uint8_t>(get_vpn_offset(alu));
  uint8_t max_vpn = static_cast<uint8_t>(get_vpn_limit(alu));
  int hole_pos = get_hole_pos(alu);
  if (!SweepInfo::holevpn_check(min_vpn, max_vpn, hole_pos)) {
    RMT_LOG_OBJ(mau(), RmtDebug::error(),
                "MauStatefulCounters::check_vpn_range: Invalid VPN min/max/hole config - "
                "(ALU=%d min_vpn=%d(0x%02x) max_vpn=%d(0x%02x) holepos=%d)\n",
                alu, min_vpn, min_vpn, max_vpn, max_vpn, hole_pos);
    THROW_ERROR(-2); // For DV
  }
}
void MauStatefulCounters::stateful_counter_vpn_callback(uint32_t alu) {
  RMT_ASSERT(alu < kNumAlus);
  spinlock();
  alu_vpn_change_[alu] = true;
  spinunlock();
}
void MauStatefulCounters::stateful_counter_vpn_en_callback() {
  uint8_t old_hole_en = stateful_cntr_hole_en_;
  uint8_t new_hole_en = mau_stateful_log_vpn_hole_en_.mau_stateful_log_vpn_hole_en();
  stateful_cntr_hole_en_ = new_hole_en;
  spinlock();
  for (int i = 0; i < kNumAlus; i++) {
    if ((old_hole_en & (1u<<i)) != (new_hole_en & (1u<<i))) alu_vpn_change_[i] = true;
  }
  spinunlock();
}




}
