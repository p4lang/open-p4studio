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

// MauOpHandler - JBay specific code
#include <mau.h>
#include <rmt-log.h>
#include <register_adapters.h>
#include <rmt-object-manager.h>
#include <chip.h>
#include <mau-atomic-writes.h>
#include <mau-op-handler.h>

namespace MODEL_CHIP_NAMESPACE {

MauOpHandler::MauOpHandler(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau)
    : MauOpHandlerCommon(om, pipeIndex, mauIndex, mau), ctor_running_(true),
      atomic_state_(kAtomicStateReset), atomic_thread_(false), atomic_writes_(nullptr),
      atomic_mod_sram_go_pending_(default_adapter(atomic_mod_sram_go_pending_,
                                                  chip_index(), pipeIndex, mauIndex,
                                                  [this](uint32_t i){this->atomic_mod_sram_go_cb(i);})),
      atomic_mod_tcam_go_(default_adapter(atomic_mod_tcam_go_,
                                          chip_index(), pipeIndex, mauIndex,
                                          [this](){this->atomic_mod_tcam_go_cb();}))
{
  atomic_mod_sram_go_pending_.reset();
  atomic_mod_tcam_go_.reset();
  ctor_running_ = false;
}
MauOpHandler::~MauOpHandler() {
  if (atomic_writes_ != nullptr) delete atomic_writes_;
  atomic_writes_ = nullptr;
}


void MauOpHandler::push_pop_stateful(bool push, uint8_t cntr, uint32_t incr,
                                     uint64_t data0, uint64_t data1, uint64_t T) {
  MauStatefulCounters *stfl = mau()->mau_stateful_counters();
  // TODO:T: Temporary logic till DV swaps over to using explicit T value
  uint64_t Tdata = (kZeroisePushPopData) ?UINT64_C(0) :data0;
  uint64_t Tpps = (T > Tdata) ?T :Tdata;
  stfl->push_pop_stateful_instr(push, static_cast<int>(cntr), incr, Tpps);
}
void MauOpHandler::instr_push_pop_stateful(int instr, int opsiz, int datasiz,
                                           uint64_t data0, uint64_t data1, uint64_t T) {
  RMT_ASSERT(!atomic_in_progress());
  bool     push = (((instr >> 24) & 1) == 1);
  uint8_t  cntr = static_cast<uint8_t> ((instr >> 20) & 0xF);
  uint32_t incr = static_cast<uint32_t>((instr >>  0) & 0xFFFFF) + 1; // NOTE +1 !!!!!
  push_pop_stateful(push, cntr, incr, data0, data1, T);
}


void MauOpHandler::atomic_mod_sram(bool ingress, uint8_t flags,
                                   uint64_t data0, uint64_t data1, uint64_t T) {

  std::array< std::array< bool, kSramColumnsPerMau>, kSramRowsPerMau > locked;
  spinlock_.lock();

  // Go through all SRAMs for gress, locking any that are pending
  // (We'll only successfully lock if the SRAM has a pending write)
  for (int row = 0; row < kSramRowsPerMau; row++) {
    for (int col = 0; col < kSramColumnsPerMau; col++) {
      locked[row][col] = false;
      MauSram *sram = mau()->sram_lookup(row,col);
      if ((sram != NULL) && (sram->check_ingress_egress(ingress))) {
        locked[row][col] = sram->pending_lock();
      }
    }
  }
  // Now that all pending SRAMs are locked
  // go through them flushing pending writes
  for (int row = 0; row < kSramRowsPerMau; row++) {
    for (int col = 0; col < kSramColumnsPerMau; col++) {
      if (locked[row][col]) {
        MauSram *sram = mau()->sram_lookup(row,col);
        RMT_ASSERT(sram != NULL);
        sram->pending_flush();
      }
    }
  }
  // Once we've flushed all pending writes
  // go through all SRAMs and unlock
  for (int row = 0; row < kSramRowsPerMau; row++) {
    for (int col = 0; col < kSramColumnsPerMau; col++) {
      if (locked[row][col]) {
        MauSram *sram = mau()->sram_lookup(row,col);
        RMT_ASSERT(sram != NULL);
        sram->pending_unlock();
      }
    }
  }
  spinlock_.unlock();
}
void MauOpHandler::instr_atomic_mod_sram(int instr, int opsiz, int datasiz,
                                         uint64_t data0, uint64_t data1, uint64_t T) {
  RMT_ASSERT(!atomic_in_progress());
  uint8_t flags = static_cast<uint8_t>((instr >> 0) & 0xFF);
  bool ingress = atomic_mod_ingress(flags);
  atomic_mod_sram(ingress, flags, data0, data1, T);
}


void MauOpHandler::dispatch_atomic_writes(bool ingress) {
  if (atomic_thread_ != ingress) return; // Thread must match
  if (atomic_state_ != kAtomicStateEnding) return; // Must have seen END
  atomic_writes_->buffer_writes(false); // Need to let CSR writes through!
  int n = atomic_writes_->n_csr_writes();
  RMT_LOG(RmtDebug::verbose(),
          "MauOpHandler::dispatch_atomic_writes(%s) N_CSRs=%d START...\n",
          ingress?"ingress":"egress", n);
  RmtObjectManager *om = get_object_manager();
  Chip *chip = om->chip();
  for (int i = 0; i < n; i++) {
    uint32_t addr = 0u, data = 0u;
    if (atomic_writes_->get_write(i, &addr, &data)) {
      RMT_LOG(RmtDebug::verbose(),
              "MauOpHandler::dispatch_atomic_writes(%s) I=%d Addr=0x%x Data=0x%08x\n",
              ingress?"ingress":"egress", i, addr, data);
      chip->OutWord(addr, data);
    }
  }
  RMT_LOG(RmtDebug::verbose(),
          "MauOpHandler::dispatch_atomic_writes(%s) N_CSRs=%d ...END\n",
          ingress?"ingress":"egress", n);
  atomic_state_ = kAtomicStateEnded;
  atomic_writes_->reset(); // Reset buffer
}
void MauOpHandler::dispatch_atomic_writes_this_mau(bool ingress) {
  // NOTE, no need to lock out Chip::OutWord|IndirectWrite here
  // because on JBay default is to take per-chip mutex before
  // processing InWord/OutWord/IndirectRead/IndirectWrite.
  //
  // This means this thread is already holding mutex exclusively
  // so any other thread will be blocked and unable to get in.
  //
  // OTHERWISE would need to implement funcs to do something like:
  //  RmtObjectManager *om = get_object_manager();
  //  Chip *chip = om->chip();
  //  Chip::set_max_accessors(1);Chip::wait_for_accessors(1)
  //  Chip::set_max_accessors(-1); // AT END
  mau()->lock_resources();
  dispatch_atomic_writes(ingress);
  mau()->unlock_resources();
}
void MauOpHandler::dispatch_atomic_writes_all_maus(bool ingress) {
  // See comment above about blocking other Chip::OutWord|IndirectWrite
  RmtObjectManager *om = get_object_manager();
  for (int i = 0; i < RmtDefs::kStagesMax; i++) {
    Mau *mau = om->mau_lookup(pipe_index(), i);
    if (mau != NULL) mau->lock_resources();
  }
  for (int i = 0; i < RmtDefs::kStagesMax; i++) {
    Mau *mau = om->mau_lookup(pipe_index(), i);
    if (mau != NULL) {
      MauOpHandler *oph = mau->mau_op_handler();
      oph->dispatch_atomic_writes(ingress);
    }
  }
  for (int i = 0; i < RmtDefs::kStagesMax; i++) {
    Mau *mau = om->mau_lookup(pipe_index(), i);
    if (mau != NULL) mau->unlock_resources();
  }
}
void MauOpHandler::atomic_mod_csr(bool ingress, uint8_t flags,
                                  uint64_t data0, uint64_t data1, uint64_t T) {
  const char *states[4] = { "Reset", "Begun", "Ending", "Ended" };

  // If no MauAtomicWrite obj yet setup do so now
  // This handles capturing and buffering CSR writes made to this MAU
  if (atomic_writes_ == nullptr) {
    atomic_writes_ = new MauAtomicWrites(chip_index(), pipe_index(), mau_index(),
                                         [this](){this->atomic_write_cb();},
                                         [this](){this->atomic_read_cb();});
  }
  if (atomic_mod_begin(flags)) {
    RMT_ASSERT(!atomic_in_progress());
    // State must be RESET or ENDED
    switch (atomic_state_) {
      case kAtomicStateReset: case kAtomicStateEnded:
        atomic_state_ = kAtomicStateBegun;
        atomic_thread_ = ingress; // only care about thread on BEGIN
        atomic_writes_->buffer_writes(true); // Buffer CSR writes
        break;
      default:
        RMT_LOG(RmtDebug::error(), "MauOpHandler::atomic_mod_csr(BEGIN) but state=%s\n",
                states[atomic_state_ % 4]);
        break;
    }
  }
  if (atomic_mod_end(flags)) {
    // State must be BEGUN
    if (atomic_state_ == kAtomicStateBegun) {
      // Check gress matches one used on BEGIN - error if not
      if (atomic_thread_ != ingress) {
        const char *iestr[2] = { "ingress", "egress" };
        RMT_LOG(RmtDebug::error(), "MauOpHandler::atomic_mod_csr(END) "
                "END called on %s but BEGIN called on %s\n",
                iestr[ingress?0:1], iestr[atomic_thread_?0:1]);
      }
      atomic_state_ = kAtomicStateEnding; // Move on FSM
      if (kAllowAtomicWideBubbles) {
        // WIDE IS supported
        if (atomic_mod_wide_bubble(flags)) {
          // END and WIDE - dispatch ALL MAU buffered CSR writes
          dispatch_atomic_writes_all_maus(atomic_thread_);
        } else {
          // END and not WIDE - just leave this MAU marked ENDING
          // At some point another MAU will get END WIDE and will
          // dispatch atomic writes for this MAU
        }
      } else {
        // WIDE NOT supported - WARN - dispatch this MAU buffered CSR writes
        if (atomic_mod_wide_bubble(flags)) {
          RMT_LOG(RmtDebug::warn(), "MauOpHandler::atomic_mod_csr(END) "
                  "atomic_mod_csr WIDE flag NOT supported - IGNORING\n");
        }
        dispatch_atomic_writes_this_mau(atomic_thread_);
      }
    } else {
      RMT_LOG(RmtDebug::error(), "MauOpHandler::atomic_mod_csr(END) but state=%s\n",
              states[atomic_state_ % 4]);
    }
  }
}
void MauOpHandler::instr_atomic_mod_csr(int instr, int opsiz, int datasiz,
                                        uint64_t data0, uint64_t data1, uint64_t T) {
  // Don't RMT_ASSERT(!atomic_in_progress()) here as atomic_mod_CSR(END) *is* allowed
  uint8_t flags = static_cast<uint8_t>((instr >> 0) & 0xFF);
  bool ingress = atomic_mod_ingress(flags);
  atomic_mod_csr(ingress, flags, data0, data1, T);
}




// Demux to appropriate per-chip func
void MauOpHandler::instr_handle_perchip(int instr, int data_size,
                                        uint64_t data0, uint64_t data1, uint64_t T) {
  // On JBay op[27:25] = 0x1 is for push_pop_stateful (so [27:23]=0x4/0x5/0x6/0x7)
  // ONLY instruction with op[27:21]=0xE & op[20:19]=0x0 is tcam_copy_word (so [27:19]=0x38)
  // Instructions with op[27:19]=0x39/0x3A are atomic_mod_sram/atomic_mod_csr respectively
  switch (instr >> 23) {
    case 0x3:
      switch (instr >> 21) {
        case 0xE:
          switch (instr >> 19) {
            case 0x38: instr_tcam_copy_word(instr, 10, data_size, data0, data1, T); break;
            case 0x39: instr_atomic_mod_sram(instr, 1, data_size, data0, data1, T); break;
            case 0x3A: instr_atomic_mod_csr(instr,  3, data_size, data0, data1, T); break;
          }
          break;
      }
      break;
    case 0x4: case 0x5: case 0x6: case 0x7:
      instr_push_pop_stateful(instr, 25, data_size, data0, data1, T); break;
  }
}



void MauOpHandler::atomic_mod_sram_go_cb(int ie) {
  if (ctor_running_) return;
  RMT_ASSERT((ie == 0) || (ie == 1));
  bool ingress = (ie == 0);
  if (atomic_mod_sram_go_pending_.atomic_mod_sram_go_pending(ie) == 1) {
    // Flush all pending SRAM writes
    atomic_mod_sram(ingress, 0, UINT64_C(0), UINT64_C(0), UINT64_C(0));
    // Clear CSR to show done
    atomic_mod_sram_go_pending_.atomic_mod_sram_go_pending(ie, 0);
  }
}

void MauOpHandler::atomic_mod_tcam_go_cb() {
  if (ctor_running_) return;
  if (atomic_mod_tcam_go_.atomic_mod_tcam_go() == 1) {
    // Flush all pending TCAM writes
    flush_all_tcam_writeregs();
    // Clear CSR to show done
    atomic_mod_tcam_go_.atomic_mod_tcam_go(0);
  }
}

void MauOpHandler::atomic_read_cb() {
  RMT_ASSERT(!atomic_in_progress()); // Prob OK to assert
  if (atomic_in_progress()) {
    RMT_LOG(RmtDebug::error(),
            "MauOpHandler::atomic_read_cb: Possible invalid CSR read "
            "Atomic OP atomic_mod_csr still in progress\n");
  }
}
void MauOpHandler::atomic_write_cb() {
  if (atomic_writes_->overflowed()) {
    RMT_LOG(RmtDebug::error(),
            "MauOpHandler::atomic_write_cb: TOO MANY CSR writes "
            "(can buffer %d CSR writes but %d written\n",
            atomic_writes_->n_csr_writes(),
            atomic_writes_->n_writes());
  } else {
    uint32_t addr = 0u, data = 0u;
    if (atomic_writes_->get_write(-1, &addr, &data)) {
      RMT_LOG(RmtDebug::verbose(),
              "MauOpHandler::atomic_write_cb: Addr=0x%x Data=0x%08x\n",
              addr, data);
    }
  }
}


}
