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

// MauPredication - Tofino/TofinoB0 code
// In shared/ because identical across these chips

#include <mau.h>
#include <register_adapters.h>
#include <mau-lookup-result.h>
#include <mau-predication.h>

namespace MODEL_CHIP_NAMESPACE {

MauPredication::MauPredication(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau)
    : MauPredicationCommon(om, pipeIndex, mauIndex, mau), mau_(mau),
      lt_ingress_(0), lt_egress_(0), lt_counters_(0),
      lt_countable_(0), lt_lookupable_(0), lt_runnable_(0),
      lt_active_(0), lt_warn_(0)
{
  for (int i = 0; i < kThreads; i++) next_table_[i] = NxtTab::inval_next_table();
}
MauPredication::~MauPredication() {
}

uint16_t MauPredication::get_lookup_mask(bool ing, int start_tab) {
  MauDependencies *deps = mau_->mau_dependencies();
  uint16_t lt_gress = ing ?deps->lt_ingress() :deps->lt_egress();
  int this_mau = mau_->mau_index();
  int start_mau = NxtTab::which_mau(start_tab);
  int start_lt = NxtTab::which_table(start_tab);
  // No tables can run if earlier mau so return 0
  if  (start_mau < this_mau) return 0;
  if ((start_mau > this_mau) && (deps->start_table_avail(ing))) return 0;
  // Later MAU (and start_table available) ==> no tables run, so returned 0 above
  // Later MAU (and start_table unavailable) ==> run all tables (set start_lt = 0)
  //  This MAU (and start_table unavailable) ==> run all tables (set start_lt = 0)
  //  This MAU (and start_table available) ==> honour start_lt
  // If start_lt IS in this stage (>0) check in correct gress
  // (Note start_tab hard-wired to be 0 for Stage0 and we can't insist LT 0
  //  is in both gresses so we don't warn if this_mau == 0)
  if ((start_mau == this_mau) && (this_mau > 0) && (!is_bit_set(lt_gress, start_lt))) {
    const char *iestr[2] = { "ingress", "egress" };
    RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                "MAU_PRED<%d> %s start_table %d not in %s LTs (0x%04x)\n",
                mau_->mau_index(), iestr[ing?0:1], start_lt, iestr[ing?0:1], lt_gress);
  }
  if (!deps->start_table_avail(ing)) start_lt = 0;
  // Only tables greater than or equal to start_tab LT can run
  return mask_equal_or_above(start_lt);
}
uint16_t MauPredication::get_active_mask(bool ing, int nxt_tab) {
  int this_mau = mau_->mau_index();
  int nxt_mau = NxtTab::which_mau(nxt_tab);
  int nxt_lt = NxtTab::which_table(nxt_tab);
  if (nxt_mau != this_mau) return 0; // No tables can be active
  // Only tables greater than or equal to nxt_tab LT can be active
  return mask_equal_or_above(nxt_lt);
}

void MauPredication::start(bool thread_active[]) {
  MauIO *inIO = mau_->mau_io_input();
  bool ingress_active = thread_active[kThreadIngress];
  bool egress_active = thread_active[kThreadEgress];
  int ing_nxt_tab = inIO->ingress_nxt_tab();
  int egr_nxt_tab = inIO->egress_nxt_tab();
  next_table_[kThreadIngress] = ing_nxt_tab;
  next_table_[kThreadEgress] = egr_nxt_tab;

  MauDependencies *deps = mau_->mau_dependencies();
  uint16_t lt_ing = deps->lt_ingress();
  uint16_t lt_egr = deps->lt_egress();
  MauTableCounters *cntrs = mau_->mau_table_counters();
  uint16_t lt_cntrs = cntrs->lt_with_counters();
  bool changed = ((lt_ing != lt_ingress_) || (lt_egr != lt_egress_) ||
                 (lt_cntrs != lt_counters_));
  lt_ingress_ = lt_ing;
  lt_egress_ = lt_egr;
  lt_counters_ = lt_cntrs;
  // Check no tables cited as both ingress and egress
  RMT_ASSERT((lt_ingress_ & lt_egress_) == 0);

  // PRED:TESTING: use kModeEvaluateAll if there are table counters
  //uint8_t mode = (lt_counters_ != 0) ?kModeEvaluateAll :get_run_mode();
  uint8_t mode = get_run_mode();

  uint16_t lt_all = lt_ingress_ | lt_egress_;
  uint16_t lt_unused = kLtAll & ~lt_all;
  // Warn if counter LTs are unused LTs
  if ((lt_counters_ & lt_unused) != 0) {
    RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                "MAU_PRED<%d> Counter using invalid LT - IGNORING "
                "(valid LTs=0x%04x, counter LTs=0x%04x, counter LTs used=0x%04x)\n",
                mau_->mau_index(), lt_all, lt_counters_, lt_counters_ & lt_all);
    lt_counters_ &= lt_all; // Switch off LTs not in either gress
  }
  // Work out total set of tables active dependent on threads active
  uint16_t lt_gress = ((ingress_active) ?lt_ingress_ :0) | ((egress_active) ?lt_egress_ :0);
  lt_countable_ = lt_counters_ & lt_gress;

  // Find all LTs we can do full lookup in - ie powered LTs
  // This depends on ingress/egress nxt_tab and whether start_table is available for gress
  uint16_t lt_lookup_ing = get_lookup_mask(true, ing_nxt_tab) & lt_ingress_;
  uint16_t lt_lookup_egr = get_lookup_mask(false, egr_nxt_tab) & lt_egress_;
  lt_lookupable_ = ((ingress_active) ?lt_lookup_ing :0) | ((egress_active) ?lt_lookup_egr :0);
  uint16_t lt_unpowered = kLtAll & ~lt_lookupable_;

  // Say something if countable LTs are not available for full lookup
  // (ie use unpowered LTs). But restrict check to tblcounter LTs
  if ((lt_countable_ & lt_unpowered) != 0) {
    uint16_t lt_countable_tblcntrs = lt_countable_ & cntrs->lt_with_tblcounters();
    if ((lt_countable_tblcntrs & lt_unpowered) != 0) {
      // Record unavailable tblcounter LTs and warn, if used, later
      if (changed) lt_warn_ = lt_countable_tblcntrs & lt_unpowered;
      RMT_LOG_OBJ(mau_, RmtDebug::info(),
                  "MAU_PRED<%d> TblCounter using power-saved LT - table lookup will "
                  "appear to always miss (powered LTs=0x%04x, tblcountable LTs=0x%04x)\n",
                  mau_->mau_index(), lt_lookupable_, lt_countable_tblcntrs);
    }
  }

  // Find initial set of active LTs (at most one ingress and one egress)
  // This depends on ingress/egress nxt_tab again and set of LTs powered for gress
  uint16_t lt_active_ing = mask_first(get_active_mask(true, ing_nxt_tab) & lt_lookup_ing);
  uint16_t lt_active_egr = mask_first(get_active_mask(false, egr_nxt_tab) & lt_lookup_egr);
  lt_active_ = ((ingress_active) ?lt_active_ing :0) | ((egress_active) ?lt_active_egr :0);

  // Find all LTs we should run - OR in counters as they run unconditionally
  // (in short-circuit mode we ONLY run active LTs and counter LTs but we
  // splice in new active LTs as we discover them)
  switch (mode) {
    case kModeEvaluateAll:  lt_runnable_ = lt_lookupable_ | lt_countable_; break;
    case kModeShortCircuit: lt_runnable_ = lt_active_     | lt_countable_; break;
  }
  // Double-check we're not trying to use invalid LTs
  RMT_ASSERT((lt_runnable_ & lt_unused) == 0);
}
void MauPredication::end() {
  MauIO *outIO = mau_->mau_io_output();
  outIO->reset_pred();
  outIO->set_ingress_nxt_tab(next_table_[kThreadIngress]);
  outIO->set_egress_nxt_tab(next_table_[kThreadEgress]);
  // Cleanup but leave lt_{ingress_,egress_,counters_,warn_,active_} for examination
  lt_countable_ = lt_lookupable_ = lt_runnable_ = 0;
  for (int i = 0; i < kThreads; i++) next_table_[i] = NxtTab::inval_next_table();
}
int MauPredication::get_next_table(bool ingress, int curr_lt, bool *do_lookup) {
  RMT_ASSERT((curr_lt >= -1) && (curr_lt < kTables) && (do_lookup != NULL));
  uint16_t lt_gress = ingress ?lt_ingress_ :lt_egress_;
  RMT_ASSERT((curr_lt == -1) || (is_bit_set(lt_gress, curr_lt)));
  // Find first table *greater than* curr_lt that's runnable and in correct gress
  int next_lt = find_first( mask_above(curr_lt) & lt_gress & lt_runnable_ );
  bool lookupable = is_bit_set(lt_lookupable_, next_lt); // Can we do full-lookup?
  if ((next_lt >= 0) && (!lookupable)) {
    if (is_bit_set(lt_warn_, next_lt)) {
      const char *iestr[2] = { "ingress", "egress" };
      lt_warn_ &= ~(1<<next_lt); // Only warn on first use
      RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                  "MAU_PRED<%d> get_next_table(%s,%d) Power-saved LT %d using "
                  "table counter - table lookup will appear to always miss\n",
                  mau_->mau_index(), iestr[ingress?0:1], curr_lt, next_lt);
    }
  }
  *do_lookup = lookupable;
  return next_lt;
}
int MauPredication::get_first_table(bool ingress, bool *do_lookup) {
  return get_next_table(ingress, -1, do_lookup);
}
void MauPredication::set_next_table(int lt, const MauLookupResult &result) {

  uint16_t nxt_tab_mapped = result.next_table_form();
  // Bit of sanity checking
  RMT_ASSERT((lt >= 0) && (lt < kTables));
  RMT_ASSERT(is_bit_set(lt_runnable_, lt));
  bool ingress = is_bit_set(lt_ingress_, lt);
  bool egress  = is_bit_set(lt_egress_, lt);
  RMT_ASSERT(ingress == !egress);

  // Was table just run active? If not return. We only honour nxt_tab for active tables.
  if (!is_bit_set(lt_active_, lt)) return;

  // Is nxt_tab in different stage - if so return
  // (see Pat's email "small functional change for timing" 09 Aug 2015)
  int this_mau = mau_->mau_index();
  int nxt_tab_mau = NxtTab::which_mau(nxt_tab_mapped);
  next_table_[ingress ?0 :1] = nxt_tab_mapped;
  if (nxt_tab_mau != this_mau) return;

  // So hoping to activate another LT in *this* stage
  // Figure out what LTs in this stage could be active next tables
  int nxt_tab_lt = NxtTab::which_table(nxt_tab_mapped);
  RMT_ASSERT((nxt_tab_lt >= 0) && (nxt_tab_lt < kTables));
  uint16_t lt_gress = ingress ?lt_ingress_ :lt_egress_;
  uint16_t lt_nxt_avail = mask_above(lt) & lt_gress & lt_lookupable_;

  bool gotnxt = is_bit_set(lt_nxt_avail, nxt_tab_lt);
  if (!gotnxt) {
    // nxt_tab_lt not available - why?
    const char *iestr[2] = { "ingress", "egress" };
    uint16_t lt_nxt_avail2 = lt_nxt_avail & mask_above(nxt_tab_lt);

    // Warn if nxt_tab_lt is before this lt, in different gress or unpowered
    if (nxt_tab_lt <= lt) {
      RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                  "MAU_PRED<%d> NxtTabLT=%d is <= LT=%d! (NxtTab=0x%x)\n",
                  mau_->mau_index(), nxt_tab_lt, lt, nxt_tab_mapped);
      lt_nxt_avail2 = lt_nxt_avail; // Make sure we look for LTs above this lt
    }
    if (!is_bit_set(lt_gress, nxt_tab_lt)) {
      RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                  "MAU_PRED<%d> NxtTabLT=%d is not in same gress(%s) as LT=%d! "
                  "(NxtTab=0x%x)\n", mau_->mau_index(), nxt_tab_lt,
                  iestr[ingress?0:1], lt, nxt_tab_mapped);
    }
    if (!is_bit_set(lt_lookupable_, nxt_tab_lt)) {
      RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                  "MAU_PRED<%d> NxtTabLT=%d is NOT powered so is unavailable for "
                  "match lookup (powered %s LTs=0x%04x)(NxtTab=0x%x)(thisLT=%d)\n",
                  mau_->mau_index(), nxt_tab_lt, iestr[ingress?0:1],
                  lt_gress & lt_lookupable_, nxt_tab_mapped, lt);
    }
    // Given nxt_tab_lt is not in the potential active set, find a nxt_tab_lt
    // that is, but search *above* the one we specified in nxt_tab
    nxt_tab_lt = find_first(lt_nxt_avail2);
    gotnxt = is_bit_set(lt_nxt_avail2, nxt_tab_lt);
    if (!gotnxt) {
      RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                  "MAU_PRED<%d> NxtTabLT=%d unavailable and no further %s LTs "
                  "available for match in this stage (powered %s LTs=0x%04x)"
                  "(NxtTab=0x%x)(thisLT=%d)\n",
                  mau_->mau_index(), NxtTab::which_table(nxt_tab_mapped),
                  iestr[ingress?0:1], iestr[ingress?0:1],
                  lt_gress & lt_lookupable_, nxt_tab_mapped, lt);
      // If no further possible active tables in this stage let next stage find next table
      // PRED:TESTING: hmm not sure - leave original value in place for now
      // next_table_[ingress ?0 :1] = NxtTab::make_next_table(this_mau+1,0);
      return;
    }
  }
  // Found a next table in this stage so update lt_active_/lt_runnable_
  lt_active_ |= (1<<nxt_tab_lt);
  lt_runnable_ |= (1<<nxt_tab_lt); // Only needed for ShortCircuit mode
  return;
}
uint16_t MauPredication::lt_info(uint16_t pred_sel) {
  if (pred_sel == 0) return 0;
  uint16_t lts = kLtAll;
  if ((pred_sel & Pred::kIngress) != 0)    lts &= lt_ingress_;
  if ((pred_sel & Pred::kEgress) != 0)     lts &= lt_egress_;
  if ((pred_sel & Pred::kCounters) != 0)   lts &= lt_counters_;
  if ((pred_sel & Pred::kCountable) != 0)  lts &= lt_countable_;
  if ((pred_sel & Pred::kLookupable) != 0) lts &= lt_lookupable_;
  if ((pred_sel & Pred::kRunnable) != 0)   lts &= lt_runnable_;
  if ((pred_sel & Pred::kActive) != 0)     lts &= lt_active_;
  if ((pred_sel & Pred::kWarn) != 0)       lts &= lt_warn_;
  if ((pred_sel & Pred::kUsed) != 0)       lts &=  (lt_ingress_|lt_egress_);
  if ((pred_sel & Pred::kUnused) != 0)     lts &= ~(lt_ingress_|lt_egress_);
  if ((pred_sel & Pred::kUnpowered) != 0)  lts &= ~(lt_lookupable_);
  if ((pred_sel & Pred::kIngThread) != 0)  lts &=  (lt_ingress_);
  if ((pred_sel & Pred::kEgrThread) != 0)  lts &=  (lt_egress_);
  return lts;
}
uint16_t MauPredication::lts_active() {
  return lt_info(Pred::kActive);
}


}
