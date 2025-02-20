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
#include <mau-table-counters.h>
#include <register_adapters.h>

namespace MODEL_CHIP_NAMESPACE {

MauTableCounters::MauTableCounters(RmtObjectManager *om,
                         int pipeIndex, int mauIndex, Mau *mau)
    : MauObject(om, pipeIndex, mauIndex, kType, mau), ctor_running_(true),
      logical_tables_with_tblcounters_(0), logical_tables_with_gwcounters_(0),
      mau_table_counter_array_(default_adapter(mau_table_counter_array_,chip_index(),pipeIndex,mauIndex)),
      mau_table_counter_ctl_(default_adapter(mau_table_counter_ctl_,chip_index(),pipeIndex,mauIndex,
                             [this](uint32_t i){this->table_counter_ctl_callback(i);})),
      mau_table_counter_clear_(default_adapter(mau_table_counter_clear_,chip_index(),pipeIndex,mauIndex,
                               [this](){this->table_counter_clear_callback();}))
{
  mau_table_counter_array_.reset();
  mau_table_counter_ctl_.reset();
  mau_table_counter_clear_.reset();
  for (int i = 0; i < kTables; i++) reset_table_cntr(i);
  ctor_running_ = false;
}
MauTableCounters::~MauTableCounters() {
}

void MauTableCounters::table_counter_ctl_callback(uint32_t i) {
  if (ctor_running_) return;
  uint16_t tblcntrs = 0, gwcntrs = 0;
  for (int lt = 0; lt < kTables; lt++) {
    switch (get_type_table_cntr(lt)) {
      case CntrType::kTableMiss:      tblcntrs |= (1<<lt); break;
      case CntrType::kTableHit:       tblcntrs |= (1<<lt); break;
      case CntrType::kGatewayMiss:    gwcntrs  |= (1<<lt); break;
      case CntrType::kGatewayHit:     gwcntrs  |= (1<<lt); break;
      case CntrType::kGatewayInhibit: gwcntrs  |= (1<<lt); break;
      default: break;
    }
  }
  logical_tables_with_tblcounters_ = tblcntrs;
  logical_tables_with_gwcounters_  = gwcntrs;
}
void MauTableCounters::table_counter_clear_callback() {
  if (ctor_running_) return;
  uint32_t reg = mau_table_counter_clear_.mau_table_counter_clear();
  for (int i = 0; i < kTables; i++) {
    if ((reg & (1u<<i)) != 0u) reset_table_cntr(i);
  }
}

uint16_t MauTableCounters::lt_with_tblcounters() {
  return logical_tables_with_tblcounters_;
}
uint16_t MauTableCounters::lt_with_gwcounters() {
  return logical_tables_with_gwcounters_;
}
uint16_t MauTableCounters::lt_with_counters() {
  return lt_with_tblcounters() | lt_with_gwcounters();
}
bool MauTableCounters::evaluate_table_counters() {
  return (lt_with_counters() != 0);
}

void MauTableCounters::maybe_increment_table_counter(int lt, const MauLookupResult &res) {
  RMT_ASSERT(res.logical_table() == lt);
  bool do_inc = false;
  switch (get_type_table_cntr(lt)) {
    case CntrType::kDisabled:       do_inc = false;        break;
    case CntrType::kTableMiss:      do_inc = !res.match(); break;
    case CntrType::kTableHit:       do_inc = res.match();  break;
    case CntrType::kGatewayMiss:    do_inc = (res.gatewayran() && !res.gatewaymatch());  break;
    case CntrType::kGatewayHit:     do_inc = (res.gatewayran() && res.gatewaymatch());   break;
    case CntrType::kGatewayInhibit: do_inc = (res.gatewayran() && res.gatewayinhibit()); break;
    default: RMT_ASSERT(0); break;
  }
  if (do_inc) incr_table_cntr(lt);
  if (do_inc) mau()->mau_info_incr(MAU_TABLE_COUNTERS_TICKED);    
}


}
