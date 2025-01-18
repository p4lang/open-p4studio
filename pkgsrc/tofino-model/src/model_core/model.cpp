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

// The tmodel-sw-version.h include not really needed.
// However the file is used by packaging scripts,
// and including it here means we'll spot if it's moved,
// and so we'll know to ***** update packaging *****
//
#include <model_core/tmodel-sw-version.h>
#include <model_core/model.h>
#include <model_core/event.h>
#include <model_core/rmt-phv-modification.h>

#if WITH_DEBUGGER
#include <libP4Debugger.h>
#endif

#ifdef MODEL_TOFINO
#undef MODEL_CHIP_NAMESPACE
#define MODEL_CHIP_NAMESPACE tofino
#undef _SHARED_CHIP__
#include <shared/chip.h>
#endif
#ifdef MODEL_TOFINOB0
#undef MODEL_CHIP_NAMESPACE
#define MODEL_CHIP_NAMESPACE tofinoB0
#undef _SHARED_CHIP__
#include <shared/chip.h>
#endif
#ifdef MODEL_JBAY
#undef MODEL_CHIP_NAMESPACE
#define MODEL_CHIP_NAMESPACE jbay
#undef _SHARED_CHIP__
#include <shared/chip.h>
#endif
#ifdef MODEL_JBAYB0
#undef MODEL_CHIP_NAMESPACE
#define MODEL_CHIP_NAMESPACE jbayB0
#undef _SHARED_CHIP__
#include <shared/chip.h>
#endif

#undef MODEL_CHIP_NAMESPACE



namespace model_core {

ModelInterface* ModelInterface::GetGlobalModel() {
  return GLOBAL_MODEL.get();
}

bool Model::kAllowCb50 = false;
// bool Model::kAllowFtr100 = true;  // FTR now always 100T

// On creation of model, make a capture class if a certain option is selected (boolean val)
Model::Model(int chips, const char* event_log) :
    chips_(chips), chip_p_(chips),
    en_0bad_mode_(1), model_logger_(this),
    spinlock_(), cb_in_progress_(false), cb_q_(),
    events_(event_log ? new EventLog_t(event_log) : nullptr),
    capture_(nullptr)
{
  model_logger_.set_console_logging(true);
#if WITH_DEBUGGER
  p4d_initialize("Harlyn ASIC Model", "", P4D_RECORD_TRACE|P4D_NON_INTERACTIVE);
#endif
}

Model::~Model() {
  DestroyAllChips(); // Reset and Destroy all chips
}


void Model::Log(int chip, int pipe, const char *buffer) {
  (void)model_logger_.log(chip, pipe, buffer);
}
void Model::SetLogDir(const char *log_dir) {
  model_logger_.set_log_dir(log_dir);
}
const char *Model::GetLogDir() const {
  return model_logger_.log_dir();
}
void Model::ProcessConfigFlags(uint32_t flags) {
    if ((flags & RMT_FLAGS_THREAD_PER_PIPE) != 0u) {
      model_logger_.set_per_chip_logging(true);
      model_logger_.set_per_pipe_logging(true);
  } else if ((flags & RMT_FLAGS_MULTI_CHIP) != 0u ) {
      model_logger_.set_per_chip_logging(true);
  }
}

void Model::SetTrace(unsigned chip, unsigned pipe, bool enable) {
  if (enable )
    std::ignore = trace_.emplace(chip, pipe);
  else
    std::ignore = trace_.erase({chip, pipe});
};

bool Model::TraceEnabled(unsigned chip, unsigned pipe) const {
  return trace_.find({chip, pipe}) != trace_.end();
}

uint8_t Model::GetDefaultType() {
  // XXX: Restructed func to try and keep StaticAnalysis happy
  // Default type is 'oldest' configured chip
#ifdef MODEL_TOFINO
  return ChipType::kTofino;
#endif
#ifdef MODEL_TOFINOB0
  return ChipType::kTofinoB0;
#endif
#ifdef MODEL_JBAY
  return ChipType::kJbay;
#endif
#ifdef MODEL_JBAYB0
  return ChipType::kJbayB0;
#endif
  return ChipType::kNone;
}

void Model::StartCapture() {
  const char *log_dir = GetLogDir();
  capture_.reset((log_dir == nullptr) ? new Capture() : new Capture(log_dir));
}

bool Model::CreateChip(int chip, uint8_t type) {
  if ((chip < 0) || (chip >= chips_)) return false;
  if (chip_p_[chip]) return false;
  if (type == ChipType::kDefault) type = GetDefaultType();
  ChipInterface *c = NULL;
  switch (type) {
#ifdef MODEL_TOFINO
    case ChipType::kTofino:   c = new tofino::Chip(chip);   break;
#endif
#ifdef MODEL_TOFINOB0
    case ChipType::kTofinoB0: c = new tofinoB0::Chip(chip); break;
#endif
#ifdef MODEL_JBAY
    case ChipType::kJbay:     c = new jbay::Chip(chip);     break;
#endif
#ifdef MODEL_JBAYB0
    case ChipType::kJbayB0:   c = new jbayB0::Chip(chip);   break;
#endif
    default:  return false;
  }
  RMT_ASSERT(c != NULL);
  chip_p_[chip].reset(c);
  RMT_ASSERT(GetType(chip) == type);
  ResetChip(chip);
  chip_p_[chip]->set_0bad_mode(en_0bad_mode_);
  // By default put chip into a trivial, size 1 package
  chip_p_[chip]->SetPackage(chip_p_[chip].get(), nullptr, nullptr, nullptr);
  log_event(EventChipLifetime<EventWriter>(0, chip, type));
  return true;
}

bool Model::DestroyChip(int chip) {
  if (!IsChipValid(chip)) return false;
  if (IsPackaged(chip)) return false; // Must be unpackaged to Destroy
  // Next line results in Chip::~Chip() which does Chip::DestroyChip()
  chip_p_[chip].reset(NULL);
  log_event(EventChipLifetime<EventWriter>(0, chip, 0, true));
  return true;
}

// Returns package size (1 if unpackaged; 2,3,4 if packaged; <0 on error)
int Model::GetPackage(int *chip0, int *chip1, int *chip2, int *chip3) {
  int *cinp[4] = { chip0, chip1, chip2, chip3 };
  int  maxpkgsz = 0;
  for (int i = 0; i < 4; i++) {
    if (cinp[i] == nullptr) break;
    maxpkgsz = i+1;
  }
  if (maxpkgsz == 0) return -1; // Error1: no ptrs passed in

  int cin[4] = { -1, -1, -1, -1 };
  for (int i = 0; i < 4; i++) {
    if (cinp[i] != nullptr) cin[i] = *cinp[i];
  }
  // For all valid indices insist chips are valid/created
  int valid0 = -1;
  for (int i = 0; i < 4; i++) {
    if (IsChipIndexValid(cin[i])) {
      if (!IsChipValid(cin[i])) return -3; // Error3: invalid chip
      if (valid0 < 0) valid0 = i; // Track first valid index
    }
  }
  if (valid0 < 0) return -2; // Error2: no valid chip indices

  ChipInterface *cif0, *cif1, *cif2, *cif3;
  // Get chip package of first valid index
  chip_p_[ cin[valid0] ]->GetPackage(&cif0, &cif1, &cif2, &cif3);
  ChipInterface *ciftab[4] = { cif0, cif1, cif2, cif3 };

  // Now loop through all valid chip indices and check match
  for (int i = 0; i < 4; i++ ) {
    if (IsChipIndexValid( cin[i] )) {
      RMT_ASSERT(IsChipValid( cin[i] ));
      chip_p_[ cin[i] ]->GetPackage(&cif0, &cif1, &cif2, &cif3);
      if ((cif0 != ciftab[0]) || (cif1 != ciftab[1]) ||
          (cif2 != ciftab[2]) || (cif3 != ciftab[3]))
        return -4; // Error4: mismatching chip packages
    }
  }
  // All valid indices have same chip package defn.
  // Now map valid ptrs back to chip indices
  int cout[4] = { -1, -1, -1, -1 };
  for (int n = 0; n < chips_; n++) {
    if (chip_p_[n].get() != nullptr) {
      for (int i = 0; i < 4; i++) {
        if (chip_p_[n].get() == ciftab[i]) cout[i] = n;
      }
    }
  }
  // Should now be able to work out pkgsize
  int pkgsz = 0;
  for (int i = 0; i < 4; i++) {
    if (cout[i] < 0) break;
    pkgsz = i+1;
  }
  RMT_ASSERT(pkgsz >= 1);

  // Args must be big enough to accommodate entire package
  if (pkgsz > maxpkgsz) return -5; // Error5: args too small for package

  // Then return to caller
  if (chip0 != nullptr) *chip0 = cout[0];
  if (chip1 != nullptr) *chip1 = cout[1];
  if (chip2 != nullptr) *chip2 = cout[2];
  if (chip3 != nullptr) *chip3 = cout[3];
  return pkgsz;
}
bool Model::IsPackaged(int chip) {
  int chip0 = chip, dummy1 = -1, dummy2 = -1, dummy3 = -1;
  int pkgsz = GetPackage(&chip0, &dummy1, &dummy2, &dummy3);
  return (pkgsz > 1);
}
bool Model::SetPackage(int chip0, int chip1, int chip2, int chip3) {
  int cin[4] = { chip0, chip1, chip2, chip3 };
  ChipInterface *ciftab[4] = { nullptr, nullptr, nullptr, nullptr };
  int pkgsz = 0;
  uint8_t type0 = GetType(cin[0]);

  // Check all chips are created (allow chip -1 to mean none)
  // Check all chips currently unpackaged
  // Check all chips are same type
  //
  for (int i = 0; i < 4; i++) {
    if (!IsChipIndexValid(cin[i])) break; // Stop when see a -1
    if (!IsChipValid(cin[i]))      return false;
    if (IsPackaged(cin[i]))        return false;
    if (GetType(cin[i]) != type0)  return false;
    ciftab[i] = chip_p_[ cin[i] ].get();
    RMT_ASSERT(ciftab[i] != nullptr);
    for (int j = 0; j < i; j++) {
      if (ciftab[j] == ciftab[i]) return false; // No dups
    }
    pkgsz = i+1;
  }
  bool ok_pkg_size = false;
  switch (type0) {
    case ChipType::kRsvd3:
      ok_pkg_size = (pkgsz == 1); // || ((pkgsz == 2) && kAllowFtr100);
      break;
    case ChipType::kRsvd0:
      ok_pkg_size = ((pkgsz == 1) || (pkgsz == 2) || ((pkgsz == 4) && kAllowCb50));
      break;
    case ChipType::kJbayA0: case ChipType::kJbayB0:
    case ChipType::kTofinoA0: case ChipType::kTofinoB0:
      ok_pkg_size = (pkgsz == 1);
      break;
  }
  if (!ok_pkg_size) return false;

  // Call per-chip SetPackage to tie all chips together
  for (int i = 0; i < pkgsz; i++ ) {
    ciftab[i]->SetPackage(ciftab[0], ciftab[1], ciftab[2], ciftab[3]);
  }
  return true;
}
bool Model::UnPackage(int chip0, int chip1, int chip2, int chip3) {
  int cin0 = chip0, cin1 = chip1, cin2 = chip2, cin3 = chip3;
  int cina[4] = { cin0, cin1, cin2, cin3 };
  int exp_pkgsz = 0;
  for (int i = 0; i < 4; i++) {
    if (!IsChipValid(cina[i])) break;
    exp_pkgsz = i+1;
  }
  if (exp_pkgsz == 0) return false;
  int act_pkgsz = GetPackage(&cin0, &cin1, &cin2, &cin3);
  if (act_pkgsz <= 1) return false; // Error or unpackaged
  if (act_pkgsz != exp_pkgsz) return false; // Mismatched size
  int cinb[4] = { cin0, cin1, cin2, cin3 };

  for (int i = 0; i < act_pkgsz; i++) {
    if (cina[i] != cinb[i]) return false; // Package must match exactly
  }
  for (int i = 0; i < act_pkgsz; i++) {
    RMT_ASSERT(IsChipValid( cinb[i] ));
    ChipInterface *cif = chip_p_[ cinb[i] ].get();
    cif->SetPackage(cif, nullptr, nullptr, nullptr);
  }
  return true;
}
bool Model::UnPackage(int chip) {
  // Figure out package from single chip then call 'proper' UnPackage
  int chip0 = chip, chip1 = -1, chip2 = -1, chip3 = -1;
  int pkgsz = GetPackage(&chip0, &chip1, &chip2, &chip3);
  return (pkgsz <= 1) ?false :UnPackage(chip0, chip1, chip2, chip3);
}
void Model::UnPackageAll() {
  for (int i = 0; i < chips_; i++) {
    int chip0 = i, chip1 = -1, chip2 = -1, chip3 = -1;
    if (GetPackage(&chip0, &chip1, &chip2, &chip3) > 1) {
      (void)UnPackage(chip0, chip1, chip2, chip3);
    }
  }
}


void Model::set_0bad_mode(int en) {
  en_0bad_mode_ = en;
  for (int c = 0; c < chips_; ++c) {
    if (chip_p_[c]) chip_p_[c]->set_0bad_mode(en);
  }
}


void Model::Reset() {
  for (int i = 0; i < chips_; i++) ResetChip(i);
}
void Model::DestroyAllChips() {
  UnPackageAll();
  for (int i = 0; i < chips_; i++) (void)DestroyChip(i);
  flush_event_log();
  events_.reset(NULL);
  model_logger_.reset();
}


#ifdef MODEL_TOFINOB0
tofinoB0::RmtObjectManager *Model::GetTofinoB0ObjectManager(int chip) {
  ChipInterface *chip_if = GetChip(chip);
  if (chip_if == NULL) return NULL;
  tofinoB0::Chip *chip_impl = dynamic_cast<tofinoB0::Chip*>(chip_if);
  if (chip_impl == NULL) return NULL;
  return chip_impl->GetObjectManager();
}
void Model::GetObjectManager(int chip, tofinoB0::RmtObjectManager **objmgr) {
  if (objmgr != NULL) *objmgr = GetTofinoB0ObjectManager(chip);
}
#endif
#ifdef MODEL_TOFINO
tofino::RmtObjectManager *Model::GetTofinoObjectManager(int chip) {
  ChipInterface *chip_if = GetChip(chip);
  if (chip_if == NULL) return NULL;
  tofino::Chip *chip_impl = dynamic_cast<tofino::Chip*>(chip_if);
  if (chip_impl == NULL) return NULL;
  return chip_impl->GetObjectManager();
}
void Model::GetObjectManager(int chip, tofino::RmtObjectManager **objmgr) {
  if (objmgr != NULL) *objmgr = GetTofinoObjectManager(chip);
}
// Temp accessor just for ref_model_wrapper
tofino::RmtObjectManager *Model::GetObjectManager(int chip) {
  return GetTofinoObjectManager(chip);
}
#endif
#ifdef MODEL_JBAY
jbay::RmtObjectManager *Model::GetJbayObjectManager(int chip) {
  ChipInterface *chip_if = GetChip(chip);
  if (chip_if == NULL) return NULL;
  jbay::Chip *chip_impl = dynamic_cast<jbay::Chip*>(chip_if);
  if (chip_impl == NULL) return NULL;
  return chip_impl->GetObjectManager();
}
void Model::GetObjectManager(int chip, jbay::RmtObjectManager **objmgr) {
  if (objmgr != NULL) *objmgr = GetJbayObjectManager(chip);
}
#endif
#ifdef MODEL_JBAYB0
jbayB0::RmtObjectManager *Model::GetJbayB0ObjectManager(int chip) {
  ChipInterface *chip_if = GetChip(chip);
  if (chip_if == NULL) return NULL;
  jbayB0::Chip *chip_impl = dynamic_cast<jbayB0::Chip*>(chip_if);
  if (chip_impl == NULL) return NULL;
  return chip_impl->GetObjectManager();
}
void Model::GetObjectManager(int chip, jbayB0::RmtObjectManager **objmgr) {
  if (objmgr != NULL) *objmgr = GetJbayB0ObjectManager(chip);
}
#endif



// Funcs that actually perform callback of Low Level Driver func
void Model::do_dru_diag_event_callback(int asic, uint8_t *diag_data, int len) {
  if (lld_callbacks_.dru_diag_event_) {
    lld_callbacks_.dru_diag_event_(asic, diag_data, len);
  }
}
void Model::do_dru_idle_update_callback(int asic, uint8_t *idle_timeout_data, int len) {
  if (lld_callbacks_.dru_idle_update_) {
    lld_callbacks_.dru_idle_update_(asic, idle_timeout_data, len);
  }
}
void Model::do_dru_lrt_update_callback(int asic, uint8_t *lrt_stat_data, int len) {
  if (lld_callbacks_.dru_lrt_update_) {
    lld_callbacks_.dru_lrt_update_(asic, lrt_stat_data, len);
  }
}
void Model::do_dru_rx_pkt_callback(int asic, uint8_t *pkt, int len, int cos) {
  if (lld_callbacks_.dru_rx_pkt_) {
    lld_callbacks_.dru_rx_pkt_(asic, pkt, len, cos);
  }
}
void Model::do_dru_learn_callback(int asic, uint8_t *learn_filter_data, int len, int pipe_nbr) {
  if (lld_callbacks_.dru_learn_) {
    lld_callbacks_.dru_learn_(asic, learn_filter_data, len, pipe_nbr);
  }
}

// Demux to underlying Low Level Driver funcs
void Model::do_callback(int which_cb,
                        int asic, uint8_t *data, int len, int arg) {
  switch(which_cb) {
    case CallbackOp::kCallbackOpDruDiagEvent:  do_dru_diag_event_callback(asic, data, len);   break;
    case CallbackOp::kCallbackOpDruIdleUpdate: do_dru_idle_update_callback(asic, data, len);  break;
    case CallbackOp::kCallbackOpDruLrtUpdate:  do_dru_lrt_update_callback(asic, data, len);   break;
    case CallbackOp::kCallbackOpDruRxPkt:      do_dru_rx_pkt_callback(asic, data, len, arg);  break;
    case CallbackOp::kCallbackOpDruLearn:      do_dru_learn_callback(asic, data, len, arg);   break;
    default: RMT_ASSERT(0);
  }
}

// Q args for async callback or callback immediately ourself
void Model::q_or_callback(int which_cb,
                          int asic, uint8_t *data, int len, int arg) {
  bool do_dispatch = false;
  bool do_immediate = false;
  bool do_queue = false;

  spinlock_.lock();
  uint32_t asic_epoch = EpochChip(asic);
  if (asic_epoch != 0u) {
    // Ignore callback if chip not initialised yet

    if (cb_in_progress_) {
      // Some other thread is already dispatching callbacks
      // so just queue an op for it to handle
      do_queue = true;
    } else {
      // Nobody yet dispatching callbacks - so I'll do it.
      // Set cb_in_progress_ to indicate that
      cb_in_progress_ = true;
      do_dispatch = true;
      if (cb_q_.empty()) {
        // Don't even need to Q this op - avoids copying data
        do_immediate = true;
        // Will still always try and dispatch queued ops
        // as ops could arrive post unlock
      } else {
        // Callback op Q non-empty - so must Q this op
        do_queue = true;
      }
    }

    if (do_queue) {
      CallbackOp cb_op(which_cb, asic, asic_epoch, data, len, arg);
      cb_q_.push_back(cb_op);
    }

  }
  spinlock_.unlock();


  if (do_immediate) {
    // Immediate callback using func args
    do_callback(which_cb, asic, data, len, arg);
  }
  if (do_dispatch) {
    // Dequeue any queued ops and dispatch callbacks
    // Finish when callback op Q is empty
    bool done = false;
    while (!done) {
      int op, asic, len, arg;
      uint32_t cb_epoch;
      uint8_t *data;
      spinlock_.lock();
      std::list<CallbackOp>::iterator it = cb_q_.begin();
      if (it != cb_q_.end()) {
        // Fish out params for dispatch
        op = it->op();
        asic = it->asic(); cb_epoch = it->epoch();
        data = it->swapdata(); len = it->len();
        arg = it->arg();
        cb_q_.pop_front();
        asic_epoch = EpochChip(asic);
        // Release lock..
        spinlock_.unlock();
        // ...and do callback if chip epoch still same
        if (cb_epoch == asic_epoch) {
          do_callback(op, asic, data, len, arg);
        }
        // and then free up data
        if (data != NULL) free((void*)data);
      } else {
        done = true;
        // All done so update cb_in_progress_ to
        // indicate we're finished...
        cb_in_progress_ = false;
        // ...and then release lock
        spinlock_.unlock();
      }
    }
  }
}

// Public funcs to callback Low Level Driver - callback may occur immediately
// or possibly queue and get dispatched by another thread
void Model::dru_diag_event_callback(int asic, uint8_t *diag_data, int len) {
  q_or_callback(CallbackOp::kCallbackOpDruDiagEvent, asic, diag_data, len);
}
void Model::dru_idle_update_callback(int asic, uint8_t *idle_timeout_data, int len) {
  q_or_callback(CallbackOp::kCallbackOpDruIdleUpdate, asic, idle_timeout_data, len);
}
void Model::dru_lrt_update_callback(int asic, uint8_t *lrt_stat_data, int len) {
  q_or_callback(CallbackOp::kCallbackOpDruLrtUpdate, asic, lrt_stat_data, len);
}
void Model::dru_rx_pkt_callback(int asic, uint8_t *pkt, int len, int cos) {
  q_or_callback(CallbackOp::kCallbackOpDruRxPkt, asic, pkt, len, cos);
}
void Model::dru_learn_callback(int asic, uint8_t *learn_filter_data, int len, int pipe_nbr) {
  q_or_callback(CallbackOp::kCallbackOpDruLearn, asic, learn_filter_data, len, pipe_nbr);
}

void Model::log_event(const Event<EventWriter>& event) {
  if (events_) events_->Add(event);
}
void Model::log_message(const uint64_t time, const Severity severity, const std::string& message) {
  if (events_) events_->Add(EventMessage<EventWriter>(time, severity, message));
}
void Model::flush_event_log() {
  if (events_) events_->Flush();
}

int Model::SetPhvModification(int chip, int pipe, int stage, RmtPhvModification::ModifyEnum which,
                              RmtPhvModification::ActionEnum action, int index, uint32_t value) {
  ChipInterface *c = GetChip(chip);
  return (c != nullptr) ? c->SetPhvModification(pipe, stage, which, action, index, value) : -2;
}

}
