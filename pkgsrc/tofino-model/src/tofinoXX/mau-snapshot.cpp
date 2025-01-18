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

// MauSnapshot - Tofino/TofinoB0 snapshot handling code
// In shared/ because identical across these chips

#include <mau.h>
#include <register_adapters.h>
#include <mau-snapshot.h>

namespace MODEL_CHIP_NAMESPACE {

MauSnapshot::MauSnapshot(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau)
    : MauSnapshotCommon(om, pipeIndex, mauIndex, mau),
      mau_snapshot_capture_subword32b_hi_ { {
        { default_adapter(mau_snapshot_capture_subword32b_hi_[0], chip_index(),pipeIndex,mauIndex,0 )},
        { default_adapter(mau_snapshot_capture_subword32b_hi_[1], chip_index(),pipeIndex,mauIndex,1 )} } },
      mau_snapshot_capture_subword32b_lo_ { {
        { default_adapter(mau_snapshot_capture_subword32b_lo_[0], chip_index(),pipeIndex,mauIndex,0 )},
        { default_adapter(mau_snapshot_capture_subword32b_lo_[1], chip_index(),pipeIndex,mauIndex,1 )} } },
      mau_snapshot_capture_subword16b_    { {
        { default_adapter(mau_snapshot_capture_subword16b_[0], chip_index(),pipeIndex,mauIndex,0 )},
        { default_adapter(mau_snapshot_capture_subword16b_[1], chip_index(),pipeIndex,mauIndex,1 )} } },
      mau_snapshot_capture_subword8b_     { {
        { default_adapter(mau_snapshot_capture_subword8b_[0], chip_index(),pipeIndex,mauIndex,0 )},
        { default_adapter(mau_snapshot_capture_subword8b_[1], chip_index(),pipeIndex,mauIndex,1 )} } },
      mau_snapshot_datapath_capture_(default_adapter(mau_snapshot_datapath_capture_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_table_active_(default_adapter(mau_snapshot_table_active_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_next_table_out_(default_adapter(mau_snapshot_next_table_out_,chip_index(),pipeIndex,mauIndex))
{
  for (int i = 0; i <= 1; i++) {
    mau_snapshot_capture_subword32b_hi_[i].reset();
    mau_snapshot_capture_subword32b_lo_[i].reset();
    mau_snapshot_capture_subword16b_[i].reset();
    mau_snapshot_capture_subword8b_[i].reset();
  }
  mau_snapshot_datapath_capture_.reset();
  mau_snapshot_table_active_.reset();
  mau_snapshot_next_table_out_.reset();

  // Check no mapping func necessary
  RMT_ASSERT(kPhvWordsUnmapped == kPhvWords);
}
MauSnapshot::~MauSnapshot() {
}

bool MauSnapshot::is_thread_active(bool ingress, Phv *phv) {
  return (ingress) ?phv->ingress() :phv->egress();
}
void MauSnapshot::datapath_capture(bool ingress,
                                   uint8_t from_prev, uint8_t timed, uint8_t here,
                                   uint8_t error, uint8_t ing_pktver, uint8_t eg_pktver,
                                   uint8_t thread_active, uint8_t trigger_thread,
                                   uint8_t ghost_thread_active) {
  RMT_ASSERT(ghost_thread_active == 0);
  uint8_t ie = (ingress) ?0 :1;
  mau_snapshot_datapath_capture_.snapshot_from_prev_stage(ie, from_prev);
  mau_snapshot_datapath_capture_.timebased_snapshot_trigger(ie, timed);
  mau_snapshot_datapath_capture_.snapshot_from_this_stage(ie, here);
  // TODO: disappeared in regs_28059_mau_dev: where did it go??
  //mau_snapshot_datapath_capture_.snapshot_error(ie, error);
  mau_snapshot_datapath_capture_.snapshot_ingress_pktversion(ie, ing_pktver);
  mau_snapshot_datapath_capture_.snapshot_egress_pktversion(ie, eg_pktver);
  mau_snapshot_datapath_capture_.snapshot_thread_active(ie, thread_active);
  mau_snapshot_datapath_capture_.snapshot_trigger_thread(ie, trigger_thread);
}
void MauSnapshot::next_table_capture(bool ingress) {
  // Get handle on the output I/O object for this MAU
  // XXX - capture OUTPUT info
  MauIO *outIO = mau()->mau_io_output();
  if (ingress) {
    mau_snapshot_next_table_out_.
        mau_snapshot_next_table_out(0, outIO->ingress_nxt_tab());
  } else {
    mau_snapshot_next_table_out_.
        mau_snapshot_next_table_out(1, outIO->egress_nxt_tab());
  }
}
void MauSnapshot::per_chip_capture(bool ingress, Phv *phv) {
  MauDependencies *deps = mau()->mau_dependencies();
  uint16_t mask = (ingress) ?deps->lt_ingress() :deps->lt_egress();
  uint16_t v = 0, actives = 0;
  for (int lt = 0; lt < kLogicalTables; lt++) {
    MauLookupResult *res = mau()->mau_lookup_result(lt);
    if (res->active()) actives  |= (1<<lt);
  }
  // Preserve bits of other thread - only update bits for this thread
  v = mau_snapshot_table_active_.mau_snapshot_table_active();
  v = (v & ~mask) | (actives & mask);
  mau_snapshot_table_active_.mau_snapshot_table_active(v);
}


}
