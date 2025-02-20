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
#include <mau-snapshot-common.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {


MauSnapshotCommon::MauSnapshotCommon(RmtObjectManager *om,
                                     int pipeIndex, int mauIndex, Mau *mau)
    : MauObject(om, pipeIndex, mauIndex, kType, mau),
      phv_ingress_changed_(false), phv_egress_changed_(false), w0w1_changed_(false),
      ingress_vector_(UINT64_C(0)), egress_vector_(UINT64_C(0)),
      w0w1_change_vector_(UINT64_C(0)), w0w1_zero_vector_(UINT64_C(0)), w0w1_compare_vector_(UINT64_C(0)),
      ingress_zero_vector_(UINT64_C(0)), egress_zero_vector_(UINT64_C(0)),
      ingress_valid_vector_(UINT64_C(0)), egress_valid_vector_(UINT64_C(0)),
      TS_at_T_start_(UINT64_C(0)), T_start_(UINT64_C(0)),
      T_now_last_(UINT64_C(0xFFFFFFFFFFFFFFFF)),

      phv_ingress_thread_(default_adapter(phv_ingress_thread_,chip_index(),pipeIndex,mauIndex,
                          [this](uint32_t i,uint32_t j){this->phv_ingress_change_callback(i,j);})),
      phv_egress_thread_(default_adapter(phv_egress_thread_,chip_index(),pipeIndex,mauIndex,
                         [this](uint32_t i,uint32_t j){this->phv_egress_change_callback(i,j);})),

      mau_snapshot_match_subword32b_hi_(default_adapter(mau_snapshot_match_subword32b_hi_,chip_index(),pipeIndex,mauIndex,
                                        [this](uint32_t i,uint32_t j){this->match_subword_change_callback(i,j,32,HI);})),
      mau_snapshot_match_subword32b_lo_(default_adapter(mau_snapshot_match_subword32b_lo_,chip_index(),pipeIndex,mauIndex,
                                        [this](uint32_t i,uint32_t j){this->match_subword_change_callback(i,j,32,LO);})),
      mau_snapshot_match_subword16b_(default_adapter(mau_snapshot_match_subword16b_,chip_index(),pipeIndex,mauIndex,
                                     [this](uint32_t i,uint32_t j){this->match_subword_change_callback(i,j,16);})),
      mau_snapshot_match_subword8b_(default_adapter(mau_snapshot_match_subword8b_,chip_index(),pipeIndex,mauIndex,
                                    [this](uint32_t i,uint32_t j){this->match_subword_change_callback(i,j,8);})),
      mau_snapshot_config_(default_adapter(mau_snapshot_config_,chip_index(),pipeIndex,mauIndex, [this](){this->config_change_callback();})),
      mau_snapshot_timestamp_trigger_hi_(default_adapter(mau_snapshot_timestamp_trigger_hi_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_timestamp_trigger_lo_(default_adapter(mau_snapshot_timestamp_trigger_lo_,chip_index(),pipeIndex,mauIndex)),

      mau_snapshot_physical_exact_match_hit_address_(default_adapter(mau_snapshot_physical_exact_match_hit_address_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_physical_tcam_hit_address_(default_adapter(mau_snapshot_physical_tcam_hit_address_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_gateway_table_inhibit_logical_(default_adapter(mau_snapshot_gateway_table_inhibit_logical_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_logical_table_hit_(default_adapter(mau_snapshot_logical_table_hit_,chip_index(),pipeIndex,mauIndex)),

      mau_fsm_snapshot_cur_stateq_(default_adapter(mau_fsm_snapshot_cur_stateq_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_timestamp_hi_(default_adapter(mau_snapshot_timestamp_hi_,chip_index(),pipeIndex,mauIndex,
                                 [this](){this->timestamp_change_callback(HI);})),
      mau_snapshot_timestamp_lo_(default_adapter(mau_snapshot_timestamp_lo_,chip_index(),pipeIndex,mauIndex,
                                 [this](){this->timestamp_change_callback(LO);})),
      exact_match_phys_result_en_(default_adapter(exact_match_phys_result_en_,chip_index(), pipeIndex, mauIndex)),
      exact_match_phys_result_thread_(default_adapter(exact_match_phys_result_thread_,chip_index(), pipeIndex, mauIndex)),
      tind_bus_prop_(default_adapter(tind_bus_prop_,chip_index(), pipeIndex, mauIndex)),
      intr_status_mau_snapshot_(default_adapter(intr_status_mau_snapshot_,chip_index(),pipeIndex,mauIndex))
{
  for (int i = 0; i < kPhvWords; i++) w0_[i] = w1_[i] = UINT64_C(0);

  // Match automatically fails for unprogrammed (zero value) words
  if (kPhvWordsUnmapped == kPhvWords) {
    w0w1_zero_vector_.fill_all_ones();
  } else {
    // Here need to take account of the fact that DarkPHV words can NOT be
    // matched against so should not be treated as unprogrammed (zero value).
    // We only care whether NormalPHV MochaPHV words remain zero value.
    for (int j = 0; j < kPhvWordsUnmapped; j++)
      w0w1_zero_vector_.set_bit(RmtDefs::map_mausnap_phv_index(j));
  }
  w0w1_changed_ = true;

  phv_ingress_thread_.reset();
  phv_egress_thread_.reset();

  mau_snapshot_match_subword32b_hi_.reset();
  mau_snapshot_match_subword32b_lo_.reset();
  mau_snapshot_match_subword16b_.reset();
  mau_snapshot_match_subword8b_.reset();
  mau_snapshot_config_.reset();
  mau_snapshot_timestamp_trigger_hi_.reset();
  mau_snapshot_timestamp_trigger_lo_.reset();

  mau_snapshot_physical_exact_match_hit_address_.reset();
  mau_snapshot_physical_tcam_hit_address_.reset();
  mau_snapshot_gateway_table_inhibit_logical_.reset();
  mau_snapshot_logical_table_hit_.reset();

  mau_fsm_snapshot_cur_stateq_.reset();
  mau_snapshot_timestamp_hi_.reset();
  mau_snapshot_timestamp_lo_.reset();

  exact_match_phys_result_en_.reset();
  exact_match_phys_result_thread_.reset();
  tind_bus_prop_.reset();
  intr_status_mau_snapshot_.reset();

  snapshot_pending_[0] = snapshot_pending_[1] = false;
  snapshot_capture_[0] = snapshot_capture_[1] = false;
  timestamp_snapshot_enabled_[0] = timestamp_snapshot_enabled_[1] = false;
  T_now_last_ = UINT64_C(0); // CTOR done
}
MauSnapshotCommon::~MauSnapshotCommon() {
}


void MauSnapshotCommon::phv_ingress_change_callback(uint32_t i, uint32_t j) {
  phv_ingress_changed_ = true; // Remember ingress changed
}
void MauSnapshotCommon::phv_egress_change_callback(uint32_t i, uint32_t j) {
  phv_egress_changed_ = true; // Remember egress changed
}
void MauSnapshotCommon::match_subword_change_callback(uint32_t off, uint32_t j,
                                                      int size, int hilo) {
  RMT_ASSERT( ( ((size == 8) || (size == 16)) && (hilo == NEITHER) ) ||
              ( (size == 32) && ((hilo == LO) || (hilo == HI))) );
  if (T_now_last_ == UINT64_C(0xFFFFFFFFFFFFFFFF)) return; // In CTOR
  // Offset will be in [0-223] so need to call
  // unmapped version of sizeoffset_to_index
  int index = sizeoffset_to_index_unmapped(size, off);
  RMT_ASSERT(index < kPhvWords);
  spinlock();
  w0w1_changed_ = true;
  w0w1_change_vector_.set_bit(index); // Remember which subword changed
  spinunlock();
}
void MauSnapshotCommon::config_change_callback() {
  if (T_now_last_ == UINT64_C(0xFFFFFFFFFFFFFFFF)) return; // In CTOR

  bool old_timed_snap_ing = timestamp_snapshot_enabled_[0];
  bool old_timed_snap_egr = timestamp_snapshot_enabled_[1];
  bool new_timed_snap_ing = is_timestamp_snapshot_enabled(true);
  bool new_timed_snap_egr = is_timestamp_snapshot_enabled(false);

  if ((old_timed_snap_ing != new_timed_snap_ing) ||
      (old_timed_snap_egr != new_timed_snap_egr)) {

    // Record values so we can detect next change
    timestamp_snapshot_enabled_[0] = new_timed_snap_ing;
    timestamp_snapshot_enabled_[1] = new_timed_snap_egr;

    // If overall state changed from disabled->enabled update T_now_last_
    bool old_timed_snap = (old_timed_snap_ing || old_timed_snap_egr);
    bool new_timed_snap = (new_timed_snap_ing || new_timed_snap_egr);
    if (!old_timed_snap && new_timed_snap) {
      T_start_ = get_time_now(NULL); TS_at_T_start_ = get_timestamp();
    }
  }
}
void MauSnapshotCommon::timestamp_change_callback(int hilo) {
  RMT_ASSERT((hilo == HI) || (hilo == LO));
  if (T_now_last_ == UINT64_C(0xFFFFFFFFFFFFFFFF)) return; // In CTOR
  if (is_timestamp_snapshot_enabled()) {
      T_start_ = get_time_now(NULL); TS_at_T_start_ = get_timestamp();
  }
}


uint64_t MauSnapshotCommon::get_time_now(Phv *phv) {
  // Always get global time and nop time
  const char *srcusedstr[3] = { "global", "phv", "nop" };
  int      srcused  = 0; // => global
  uint64_t T_now_last_prev = T_now_last_;
  uint64_t T_glob = get_object_manager()->time_get_cycles();
  uint64_t T_nop = mau()->mau_op_handler()->get_last_instr_time();
  uint64_t T_phv = UINT64_C(0), T_delta = UINT64_C(0);
  uint64_t T_max = T_glob;

  if (kSnapshotUsePhvTime) {
    if (phv != NULL) {
      // Use relative_time from Phv - XXX
      uint64_t T_phv_ing = (phv->relative_time_valid(true))  ?phv->get_relative_time(true)  :UINT64_C(0);
      uint64_t T_phv_egr = (phv->relative_time_valid(false)) ?phv->get_relative_time(false) :UINT64_C(0);
      T_phv = (T_phv_egr > T_phv_ing) ?T_phv_egr :T_phv_ing;
      // Use maximum of Phv relative time and global time
      T_max   = (T_glob > T_phv) ?T_glob :T_phv;
      srcused = (T_glob > T_phv) ?0 :1;
    } else {
      // Use maximum of last NOP time and global time
      T_max   = (T_glob > T_nop) ?T_glob :T_nop;
      srcused = (T_glob > T_nop) ?0 :2;
    }
  }
  // Figure out delta and apply
  T_delta = (T_max > T_now_last_) ?(T_max - T_now_last_) :UINT64_C(0);
  T_now_last_ += T_delta;

  RMT_LOG_OBJ(mau(), RmtDebug::verbose(),
              "MAU_SNAPSHOT<%d> Tnow derived from %s time "
              "Tnow=%" PRId64 " Tprev=%" PRId64 ",Tdelta=%" PRId64 " "
              "[Tglob=%" PRId64 ",Tnop=%" PRId64 ",Tphv=%" PRId64 " Tmax=%" PRId64 "]\n",
              mau()->mau_index(), srcusedstr[srcused],
              T_now_last_, T_now_last_prev, T_delta,
              T_glob, T_nop, T_phv, T_max);
  return T_now_last_;
}
bool MauSnapshotCommon::timestamp_triggered_snapshot(bool ingress, Phv *phv, bool advance_timestamp) {
  if (!is_timestamp_snapshot_enabled(ingress)) return false;
  if (!advance_timestamp) return (get_timestamp() == get_trigger_timestamp());
  uint64_t T_now = get_time_now(phv);
  uint64_t TS_delta = (T_now > T_start_) ?(T_now - T_start_) :UINT64_C(0);
  uint64_t TS_new = TS_at_T_start_ + TS_delta;
  uint64_t TS_trigger = get_trigger_timestamp();
  uint64_t TS = (TS_new > TS_trigger) ?TS_trigger :TS_new;
  set_timestamp(TS);
  RMT_LOG_OBJ(mau(), RmtDebug::verbose(),
              "MAU_SNAPSHOT<%d> TS=%" PRId64 " TS_trigger=%" PRId64 " "
              "[Tnow=%" PRId64 ",Tstart=%" PRId64 ",TS_at_Tstart=%" PRId64 "]\n",
              mau()->mau_index(), TS, TS_trigger, T_now, T_start_, TS_at_T_start_);
  return (TS == TS_trigger);
}


void MauSnapshotCommon::harvest_phv_ingress_egress_thread_changes() {
  // While outstanding changes to ingress/egress thread copy
  // in up-to-date version
  if (phv_ingress_changed_) {
    phv_ingress_changed_ = false;
    ingress_vector_.copy_from(*mau()->ingress_selector());
  }
  if (phv_egress_changed_) {
    phv_egress_changed_ = false;
    egress_vector_.copy_from(*mau()->egress_selector());
  }
}
void MauSnapshotCommon::harvest_match_subword_changes() {
  // While we have changed match_subwords harvest into w0_ w1_
  // and update w0w1_compare_vector_

  bool copy_w0w1_changed = false;
  BitVector<kPhvWords> copy_w0w1_change_vector(UINT64_C(0));
  spinlock();
  if (w0w1_changed_) {
    copy_w0w1_changed = true;
    copy_w0w1_change_vector.copy_from(w0w1_change_vector_);
    w0w1_changed_ = false;
    w0w1_change_vector_.fill_all_zeros();
  }
  spinunlock();

  if (copy_w0w1_changed) {
    int bit = copy_w0w1_change_vector.get_first_bit_set();
    while (bit >= 0) {
      // The change vector only has bits set for normal
      // or mocha PHV words - we need to map to the
      // appropriate full PHV word (only diff on JBay)
      int mapbit = RmtDefs::map_mausnap_phv_index(bit);
      // Get latest values subwords
      uint64_t mask = get_mask(Phv::which_width(mapbit));
      w0_[mapbit] = get_match_subword(bit, 0) & mask;
      w1_[mapbit] = get_match_subword(bit, 1) & mask;

      // If w0 == w1 == ZERO then remember as that is
      // always a mismatch for any s0/s1 (apart from the
      // case s0 == s1 == 0, but that is impossible here
      // and provokes an assert in ternary_match_s0s1)
      //
      // If there are *ANY* bits set in the zero_vector
      // then match fails for *ALL* PHVs
      //
      if ((w0_[mapbit] == UINT64_C(0)) && (w1_[mapbit] == UINT64_C(0)))
        w0w1_zero_vector_.set_bit(mapbit);
      else
        w0w1_zero_vector_.clear_bit(mapbit);

      // If w0 == w1 == mask (ie all ONES in width) then
      // don't bother doing a compare as that is always
      // a match for any s0/s1
      if ((w0_[mapbit] == mask) && (w1_[mapbit] == mask))
        w0w1_compare_vector_.clear_bit(mapbit);
      else
        w0w1_compare_vector_.set_bit(mapbit);

      bit = copy_w0w1_change_vector.get_first_bit_set(bit);
    }
  }
}

void MauSnapshotCommon::recalc_ingress_egress_valid() {
  // AND ingress/egress vector with w0w1_zero_vector
  // to create ingress_zero/egress_zero
  ingress_zero_vector_.copy_from(ingress_vector_);
  ingress_zero_vector_.mask(w0w1_zero_vector_);
  egress_zero_vector_.copy_from(egress_vector_);
  egress_zero_vector_.mask(w0w1_zero_vector_);

  // AND ingress/egress vector with w0w1_compare_vector
  // to create ingress_valid/egress_valid
  ingress_valid_vector_.copy_from(ingress_vector_);
  ingress_valid_vector_.mask(w0w1_compare_vector_);
  egress_valid_vector_.copy_from(egress_vector_);
  egress_valid_vector_.mask(w0w1_compare_vector_);
}

bool MauSnapshotCommon::phv_match(Phv *phv, const BitVector<kPhvWords> &selector) {
  int i = selector.get_first_bit_set();
  while (i >= 0) {
    //uint64_t mask = get_mask(Phv::which_width(i));
    //uint64_t val = phv->get_including_valid_bit(i);
    //uint64_t s1 = val & mask, s0 = ~val & mask;
    if (!ternary_match(w0_[i], w1_[i],
                       phv->get_including_valid_bit(i), Phv::which_width(i))) {
      //printf("SnapshotMISS[%d] w0=0x%016" PRIx64 " w1=0x%016" PRIx64 "\n"
      //       "                 s0=0x%016" PRIx64 " s1=0x%016" PRIx64 " width=%d\n",
      //       i, w0_[i], w1_[i], s0, s1, Phv::which_width(i));
      return false;
    }
    i = selector.get_first_bit_set(i);
  }
  //printf("SnapshotHIT\n");
  return true;
}
void MauSnapshotCommon::phv_capture(Phv *phv, const BitVector<kPhvWords> &selector) {
  int i = selector.get_first_bit_set();
  while (i >= 0) {
    set_capture_subword(i, phv->get(i), phv->is_valid(i));
    i = selector.get_first_bit_set(i);
  }
}

void MauSnapshotCommon::physical_bus_hit_addr_capture(bool ingress) {
  // Setup a fake MauLookupResult - info we need is NOT table specific
  MauLookupResult res;
  res.init(mau(), mau()->mau_result_bus());

  uint8_t xm_bot_en = exact_match_phys_result_en_.exact_match_phys_result_en(0);
  uint8_t xm_top_en = exact_match_phys_result_en_.exact_match_phys_result_en(1);
  uint16_t xm_buses_en = (static_cast<uint16_t>(xm_top_en) << 8) |
      (static_cast<uint16_t>(xm_bot_en) << 0);
  uint8_t xm_bot_thread = exact_match_phys_result_thread_.exact_match_phys_result_thread(0);
  uint8_t xm_top_thread = exact_match_phys_result_thread_.exact_match_phys_result_thread(1);
  uint16_t xm_buses_thread = (static_cast<uint16_t>(xm_top_thread) << 8) |
      (static_cast<uint16_t>(xm_bot_thread) << 0);
  uint16_t xm_buses_egress = xm_buses_thread & xm_buses_en;
  uint16_t xm_buses_ingress = ~xm_buses_thread & xm_buses_en;
  uint16_t xm_buses_gress = (ingress) ?xm_buses_ingress :xm_buses_egress;
  uint8_t gress = (ingress) ?0 :1;

  for (int xmbus = 0; xmbus < kMatchOutputBusesPerMau; xmbus++) {
    if (((xm_buses_gress >> xmbus) & 1) == 1) {
      uint32_t v = res.get_match_addr(xmbus, 0); // 0=>XM
      mau_snapshot_physical_exact_match_hit_address_.
          mau_snapshot_physical_exact_match_hit_address(xmbus, v);
    }
  }
  for (int tmbus = 0; tmbus < kTindOutputBusesPerMau; tmbus++) {
    uint8_t ie = tind_bus_prop_.thread(tmbus);
    uint8_t en = tind_bus_prop_.enabled(tmbus);
    if ((en == 1) && (ie == gress)) {
      uint32_t v = res.get_match_addr(tmbus, 1); // 1=>TM
      mau_snapshot_physical_tcam_hit_address_.
          mau_snapshot_physical_tcam_hit_address(tmbus, v);
    }
  }
}
void MauSnapshotCommon::logical_table_info_capture(bool ingress) {
  MauDependencies *deps = mau()->mau_dependencies();
  uint16_t mask = (ingress) ?deps->lt_ingress() :deps->lt_egress();
  uint16_t v = 0, inhibits = 0, matches = 0, actives = 0;
  for (int lt = 0; lt < kLogicalTables; lt++) {
    MauLookupResult *res = mau()->mau_lookup_result(lt);
    if (res->gatewayinhibit()) inhibits |= (1<<lt);
    if (res->match())          matches  |= (1<<lt);
    if (res->active())         actives  |= (1<<lt);
  }
  // Preserve bits of other thread - only update bits for this thread
  v = mau_snapshot_gateway_table_inhibit_logical_.mau_snapshot_gateway_table_inhibit_logical();
  v = (v & ~mask) | (inhibits & mask);
  mau_snapshot_gateway_table_inhibit_logical_.
      mau_snapshot_gateway_table_inhibit_logical(v);

  v = mau_snapshot_logical_table_hit_.mau_snapshot_logical_table_hit();
  v = (v & ~mask) | (matches & mask);
  mau_snapshot_logical_table_hit_.mau_snapshot_logical_table_hit(v);
}


// PUBLIC funcs

void MauSnapshotCommon::reset_resources() {
  // Track whether we've captured a snapshot *this time*
  // To check whether there's outstanding snapshot state query FSM
  snapshot_capture_[0] = snapshot_capture_[1] = false;
  bool changes = false;
  while (phv_ingress_changed_ || phv_egress_changed_ || w0w1_changed_) {
    harvest_phv_ingress_egress_thread_changes();
    harvest_match_subword_changes();
    changes = true;
  }
  if (changes) recalc_ingress_egress_valid();
}


bool MauSnapshotCommon::phv_match_ingress(Phv *phv) {
  if (ingress_zero_vector_.get_first_bit_set() >= 0) return false;
  return phv_match(phv, ingress_valid_vector_);
}
bool MauSnapshotCommon::phv_match_egress(Phv *phv) {
  if (egress_zero_vector_.get_first_bit_set() >= 0) return false;
  return phv_match(phv, egress_valid_vector_);
}
bool MauSnapshotCommon::phv_match(bool ingress, Phv *phv) {
  return (ingress) ?phv_match_ingress(phv) :phv_match_egress(phv);
}


void MauSnapshotCommon::phv_capture_ingress(Phv *phv) {
  phv_capture(phv, ingress_vector_);
}
void MauSnapshotCommon::phv_capture_egress(Phv *phv) {
  phv_capture(phv, egress_vector_);
}
void MauSnapshotCommon::phv_capture(bool ingress, Phv *phv) {
  return (ingress) ?phv_capture_ingress(phv) :phv_capture_egress(phv);
}


bool MauSnapshotCommon::maybe_snapshot(Phv *match_phv, Phv *action_phv) {

  // Get handle on the I/O objects for this MAU
  MauIO *inIO = mau()->mau_io_input();
  MauIO *outIO = mau()->mau_io_output();
  outIO->reset_snap();

  // Vars for datapath_capture reg update
  uint8_t snapshot_from_prev = 0;
  uint8_t snapshot_match = 0;
  uint8_t snapshot_timed = 0;
  uint8_t snapshot_here = 0;
  uint8_t snapshot_error = 0;
  uint8_t snapshot_ing_pktver = 0;
  uint8_t snapshot_eg_pktver = 0;
  uint8_t snapshot_trigger_thread = 0;
  uint8_t snapshot_thread_active = 0 |
      ( ((match_phv->ingress()) ?1 :0) << 0 ) |
      ( ((match_phv->egress())  ?1 :0) << 1 );
  uint8_t snapshot_thread_active_ghost = (match_phv->ghost()) ?1 :0;
  bool    thread_active_ungated[2] = { match_phv->ingress() || match_phv->ghost(),
                                       match_phv->egress() };

  for (int ie = 0; ((ie == 0) || (ie == 1)); ie++) {

    bool ingress = (ie == 0);
    int  state = get_fsm(ingress);
    int  pktver = (ingress) ?match_phv->ingress_version() :match_phv->egress_version();
    bool thread_active = is_thread_active(ingress, match_phv);
    bool triggered_already = inIO->snapshot_triggered(ie);
    bool triggered_here = false;
    bool propagated_trigger = false;


    if ((state == kArmed) || (state == kTriggerHappy)) {
      // In Armed/TriggerHappy state we capture if there
      // is a PHV match or a timestamp triggered snapshot

      // Maybe advance timestamp time if timebased snapshot enabled
      (void)timestamp_triggered_snapshot(ingress, match_phv, true);

      if (thread_active) {
        // See if timebased snapshot triggered but don't advance timestamp
        // (final param false disables timestamp advance)
        bool timed = timestamp_triggered_snapshot(ingress, match_phv, false);
        bool match = phv_match(ingress, match_phv);

        triggered_here = (match || timed);
        snapshot_capture_[ie] = triggered_here;

        if (match)          snapshot_match = 1;
        if (timed)          snapshot_timed = 1;
        if (triggered_here) snapshot_here |= (1<<ie);
        if (ingress)        snapshot_ing_pktver = pktver;
        if (!ingress)       snapshot_eg_pktver = pktver;
        if (triggered_here) snapshot_trigger_thread |= (1<<ie);

        // If triggered_here we always propagate
        if (triggered_here) propagated_trigger = true;
      }
    }

    if ((state == kPassive) || (state == kTriggerHappy)) {
      // In Passive/TriggerHappy state we also capture if an earlier
      // stage has triggered a snapshot (ie triggered_already)

      // Note we use RAW thread_active vals in this case
      if (thread_active_ungated[ie]) {
        snapshot_capture_[ie] = triggered_here || triggered_already;

        if (triggered_already) snapshot_from_prev |= (1<<ie);
        if (ingress)           snapshot_ing_pktver = pktver;
        if (!ingress)          snapshot_eg_pktver = pktver;
        if (triggered_already) snapshot_trigger_thread |= (1<<ie); // XXX
      }

      // And in Passive/TriggerHappy we also propagate previous trigger
      if (triggered_already) propagated_trigger = true;
    }

    // Pass on propagated_trigger, this is TRUE
    // IF triggered_here (means state must have been Armed/TriggerHappy)
    // OR triggered_already and state Passive/TriggerHappy
    outIO->set_snapshot_triggered(ie, propagated_trigger);

  } // for


  if (snapshot_capture_[0] || snapshot_capture_[1]) {
    // If either capture triggered maybe do both
    // Note, this probably does not make sense for DV
    // as ingress/egress PHVs will not be same as RTL
    if (snapshot_both()) {
      snapshot_capture_[0] = snapshot_capture_[1] = true;
    }
    // Update ingress/egress specific state capture regs
    for (int ie = 0; ((ie == 0) || (ie == 1)); ie++) {
      bool ingress = (ie == 0);
      if (snapshot_capture_[ie]) {
        const char *statestr[4] = { "passive", "armed", "trigger_happy", "full" };
        const char *iestr[2] = { "ingress", "egress" };
        //phv_capture(ingress, action_phv); // Too soon (see XXX)
        datapath_capture(ingress,
                         snapshot_from_prev, snapshot_timed, snapshot_here,
                         snapshot_error, snapshot_ing_pktver, snapshot_eg_pktver,
                         snapshot_thread_active, snapshot_trigger_thread,
                         snapshot_thread_active_ghost);
        RMT_LOG_OBJ(mau(), RmtDebug::verbose(),
                    "MAU_SNAPSHOT<%d> %s FSM=%s "
                    "Prev=%d Here=%d Match=%d Timed=%d DPerr=%d "
                    "IngPktVer=%d EgrPktVer=%d ThreadActive=%d "
                    "TriggerThread=%d ThreadActiveGhost=%d\n",
                    mau()->mau_index(), iestr[ie], statestr[get_fsm(ingress)],
                    snapshot_from_prev, snapshot_here,
                    snapshot_match, snapshot_timed, snapshot_error,
                    snapshot_ing_pktver, snapshot_eg_pktver,
                    snapshot_thread_active, snapshot_trigger_thread,
                    snapshot_thread_active_ghost);
      }
    }
  }

  return (snapshot_capture_[0] || snapshot_capture_[1]);
}


void MauSnapshotCommon::finalize_snapshot(Phv *output_phv,
                                          int ingress_nxt_tab, int egress_nxt_tab) {
  if (snapshot_capture_[0] || snapshot_capture_[1]) {

    // Finally capture output data, update FSM and set interrupt
    for (int ie = 0; ((ie == 0) || (ie == 1)); ie++) {
      bool ingress = (ie == 0);
      if (snapshot_capture_[ie]) {

        // Capture nxt_tab, hit_addrs and various logical_table info
        next_table_capture(ingress);
        physical_bus_hit_addr_capture(ingress);
        logical_table_info_capture(ingress);
        per_chip_capture(ingress, output_phv);

        phv_capture(ingress, output_phv);

        set_fsm(ingress, kFull);
        set_interrupt(ingress);
      }
    }
    mau()->mau_info_incr(MAU_SNAPSHOTS_TAKEN);
  }
}


bool MauSnapshotCommon::snapshot_captured() {
  return (snapshot_capture_[0] || snapshot_capture_[1]);
}


}
