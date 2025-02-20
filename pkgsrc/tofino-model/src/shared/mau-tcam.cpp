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
#include <mau-logical-tcam.h>
#include <mau-tcam.h>
#include <mau-tcam-row.h>


namespace MODEL_CHIP_NAMESPACE {

  MauTcam::MauTcam(RmtObjectManager *om,
                   int pipeIndex, int mauIndex,
                   int rowIndex, int colIndex, int tcamIndex, Mau *mau)
      : MauObject(om, pipeIndex, mauIndex, kType, rowIndex, colIndex, mau),
        row_index_(rowIndex), col_index_(colIndex), tcam_index_(tcamIndex),
        ltcams_(0u), wide_match_(false),
        pending_locked_(false), pending_index_(-1),
        pending_data0_(UINT64_C(0)), pending_data1_(UINT64_C(0)),
        row_(NULL), tcam_(this),
        mau_tcam_reg_(om, pipeIndex, mauIndex, rowIndex, colIndex, tcamIndex, this)
  {
    static_assert( (kLogicalTcamsPerMau <= 32),
                   "Logical TCAM bitmask is uint32_t");
    RMT_ASSERT(tcamIndex == mau->tcam_array_index(rowIndex, colIndex));

    // At this point tcam_ has been initialised and reset
    // and so, by default, would return indexes.
    // That's no good for MAU TCAMs - we want priority values
    // returned - so we switch to that mode now.
    tcam_.set_lookup_return_pri(true);

    // We also set_tcam_start to N_ENTRIES-1
    tcam_.set_tcam_start(kTcamEntries-1);

    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCreate),
            "MAU_TCAM<%d,%d>::create\n", rowIndex, colIndex);
  }
  MauTcam::~MauTcam() {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDelete),
            "MAU_TCAM<%d,%d>::delete\n", row_index_, col_index_);
    row_ = NULL;
  }


  uint32_t MauTcam::make_match_address(const uint32_t hit_addr, int shift) {
    uint32_t shifted_addr = hit_addr << shift;
    // XXX: apply mask to result
    uint32_t masked_addr = shifted_addr & kTcamMatchAddrMask;
    if (shifted_addr != masked_addr) {
      // Can only happen on WIP when producing a 16b bitmask result
      // and using a TcamMatchAddrShift >= 4
      // XXX - warn so people know bits getting lost
      RMT_LOG_WARN("MAU_TCAM<%d,%d> TcamMatchAdrShift(%d) too big! Address bits lost "
                   "OriginalAddr=0x%x MaskedAddr=0x%x\n", row_index_, col_index_,
                   shift, shifted_addr, masked_addr);
    }
    return masked_addr;
  }

  void MauTcam::get_match_data(Phv *phv, BitVector<kTcamWidth> *match_data) {
    int search_bus = get_search_bus();
    // Need to get even/odd results from mau-tcam-row.h
    // determined by whether our row_index is even/odd
    int which_data = row_index_ % 2;
    MauTcamRow *rowobj = row();
    RMT_ASSERT (rowobj != NULL);
    rowobj->get_match_data(phv, search_bus, which_data, match_data);
  }

  int MauTcam::lookup_index(Phv *phv, int head) {
    BitVector<kTcamWidth> match_data;
    get_match_data(phv, &match_data);
    return lookup_index(match_data, head);
  }
  int MauTcam::lookup_pri(Phv *phv, int head) {
    BitVector<kTcamWidth> match_data;
    get_match_data(phv, &match_data);
    return lookup_pri(match_data, head);
  }
  int MauTcam::lookup(Phv *phv, int head) {
    BitVector<kTcamWidth> match_data;
    get_match_data(phv, &match_data);
    // Configuration of underlying TCAM3 decides whether this is
    // a lookup_index or lookup_pri - so lookup_pri given reset(true)
    return lookup(match_data, head);
  }
  int MauTcam::lookup(Phv *phv, const int hi_pri, const int lo_pri,
                      int head, bool promote_hits) {
    BitVector<kTcamWidth> match_data;
    get_match_data(phv, &match_data);
    // Configuration of underlying TCAM3 decides whether this is
    // a lookup_index or lookup_pri - so lookup_pri given reset(true)
    return lookup(match_data, hi_pri, lo_pri, head, promote_hits);
  }
  int MauTcam::lookup(Phv *phv, const BitVector<kTcamEntries>& hits_in,
                      BitVector<kTcamEntries> *hits_out, int head) {
    BitVector<kTcamWidth> match_data;
    get_match_data(phv, &match_data);
    return lookup(match_data, hits_in, hits_out, head);
  }

  void MauTcam::update_config() {
    // Called when tcam_mode changes
    Mau *mauobj = mau();
    RMT_ASSERT (mauobj != NULL);
    for (int ltc = 0; ltc < kLogicalTcamsPerMau; ltc++) {
      if ((ltcams_ & (1<<ltc)) != 0) {
        MauLogicalTcam *ltcam = mauobj->logical_tcam_get(ltc);
        if (ltcam != NULL) {
          ltcam->tcam_config_changed(row_index(), col_index());
        }
      }
    }
  }

  // Next set of funcs to handle TCAM writereg
  // Allows atomic set across >1 TCAM

  bool MauTcam::pending_get(int *index, uint64_t *data0, uint64_t *data1) {
    bool got_pending = false;
    spinlock();
    if (index != NULL) *index = pending_index_;
    if (data0 != NULL) *data0 = pending_data0_;
    if (data1 != NULL) *data1 = pending_data1_;
    got_pending = (pending_index_ >= 0);
    spinunlock();
    return got_pending;
  }
  void MauTcam::pending_set(const int index, uint64_t data0, uint64_t data1) {
    if (index < 0) return;
    BitVector<kTcamWidth> w0;
    BitVector<kTcamWidth> w1;
    spinlock();
    pending_index_ = index;
    pending_data0_ = data0;
    pending_data1_ = data1;
    spinunlock();
  }
  bool MauTcam::pending_lock() {
    bool locked = false;
    spinlock();
    if ((!pending_locked_) && (pending_index_ >= 0)) {
      locked = true;
      pending_locked_ = true;
      tcam_.lock(pending_index_);
    }
    spinunlock();
    return locked;
  }
  void MauTcam::pending_flush() {
    spinlock();
    if ((pending_index_ >= 0) && (pending_locked_)) {
      BitVector<kTcamWidth> w0, w1;
      uint8_t p0, p1;
      w0.set_word(wval_split_data_payload(pending_data0_, &p0), 0);
      w1.set_word(wval_split_data_payload(pending_data1_, &p1), 0);
      tcam_.set_word0_word1_nolock(pending_index_, w0, w1, p0, p1);
    }
    spinunlock();
  }
  void MauTcam::pending_unlock(bool unset) {
    spinlock();
    if ((pending_locked_) && (pending_index_ >= 0)) {
      tcam_.unlock(pending_index_);
      pending_locked_ = false;
      if (unset) pending_index_ = -1;
    }
    spinunlock();
  }
  void MauTcam::pending_unset() {
    spinlock();
    pending_index_ = -1;
    spinunlock();
  }

}
