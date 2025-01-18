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

// MauPredication - Jbay implementation

#include <mau.h>
#include <register_adapters.h>
#include <mau-lookup-result.h>
#include <mau-predication.h>

namespace MODEL_CHIP_NAMESPACE {

MauPredication::MauPredication(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau)
    : MauPredicationCommon(om, pipeIndex, mauIndex, mau), mau_(mau),
      lt_ingress_orig_(0), lt_counters_orig_(0),
      lt_ingress_(0), lt_egress_(0), lt_ghost_(0), lt_mpr_(0), lt_mpr_mask_(0),
      lt_counters_(0), lt_countable_(0), lt_lookupable_(0), lt_runnable_(0), lt_active_(0), lt_warn_(0),
      threads_active_(0),
      this_mau_ingress_match_dep_(false), this_mau_egress_match_dep_(false),
      next_mau_ingress_match_dep_(false), next_mau_egress_match_dep_(false),
      prev_mau_output_seq_(0u),
      curr_output_seq_(0u), curr_check_seq_(0u), pending_check_seq_(1u),
      mpr_always_run_(default_adapter(mpr_always_run_, chip_index(), pipeIndex, mauIndex, [this](){this->register_change_callback();})),
      mpr_glob_exec_lut_(default_adapter(mpr_glob_exec_lut_, chip_index(), pipeIndex, mauIndex, [this](uint32_t i){this->register_change_callback();})),
      mpr_long_brch_lut_(default_adapter(mpr_long_brch_lut_, chip_index(), pipeIndex, mauIndex, [this](uint32_t i){this->register_change_callback();})),
      mpr_next_table_lut_(default_adapter(mpr_next_table_lut_, chip_index(), pipeIndex, mauIndex, [this](uint32_t i, uint32_t j){this->register_change_callback();})),
      mpr_stage_id_(default_adapter(mpr_stage_id_, chip_index(), pipeIndex, mauIndex, [this](uint32_t i){this->register_change_callback();})),
      mpr_bus_dep_(default_adapter(mpr_bus_dep_, chip_index(), pipeIndex, mauIndex, [this](){this->register_change_callback();})),
      mpr_glob_exec_thread_(default_adapter(mpr_glob_exec_thread_, chip_index(), pipeIndex, mauIndex, [this](){this->register_change_callback();})),
      mpr_long_brch_thread_(default_adapter(mpr_long_brch_thread_, chip_index(), pipeIndex, mauIndex, [this](){this->register_change_callback();})),
      mpr_thread_delay_(default_adapter(mpr_thread_delay_, chip_index(), pipeIndex, mauIndex, [this](uint32_t i){this->register_change_callback();})),
      pred_always_run_(default_adapter(pred_always_run_, chip_index(), pipeIndex, mauIndex, [this](uint32_t i){this->register_change_callback();})),
      pred_glob_exec_thread_(default_adapter(pred_glob_exec_thread_, chip_index(), pipeIndex, mauIndex, [this](uint32_t i){this->register_change_callback();})),
      pred_map_glob_(default_adapter(pred_map_glob_, chip_index(), pipeIndex, mauIndex, [this](uint32_t i,uint32_t j){this->register_change_callback();})),
      pred_map_loca_(default_adapter(pred_map_loca_, chip_index(), pipeIndex, mauIndex, [this](uint32_t i,uint32_t j){this->register_change_callback();})),
      pred_miss_exec_(default_adapter(pred_miss_exec_, chip_index(), pipeIndex, mauIndex, [this](uint32_t i){this->register_change_callback();})),
      pred_miss_long_brch_(default_adapter(pred_miss_long_brch_, chip_index(), pipeIndex, mauIndex, [this](uint32_t i){this->register_change_callback();})),
      pred_long_brch_lt_src_(default_adapter(pred_long_brch_lt_src_, chip_index(), pipeIndex, mauIndex, [this](uint32_t i){this->register_change_callback();})),
      pred_long_brch_terminate_(default_adapter(pred_long_brch_terminate_, chip_index(), pipeIndex, mauIndex, [this](){this->register_change_callback();})),
      pred_long_brch_thread_(default_adapter(pred_long_brch_thread_, chip_index(), pipeIndex, mauIndex, [this](uint32_t i){this->register_change_callback();})),
      pred_ghost_thread_(default_adapter(pred_ghost_thread_, chip_index(), pipeIndex, mauIndex, [this](){this->register_change_callback();})),
      pred_is_a_brch_(default_adapter(pred_is_a_brch_, chip_index(), pipeIndex, mauIndex, [this](){this->register_change_callback();})),
      pred_stage_id_(default_adapter(pred_stage_id_, chip_index(), pipeIndex, mauIndex, [this](){this->register_change_callback();})),
      next_table_map_en_(default_adapter(next_table_map_en_,chip_index(), pipeIndex, mauIndex, [this](){this->register_change_callback();})),
      next_table_map_en_gateway_(default_adapter(next_table_map_en_gateway_,chip_index(), pipeIndex, mauIndex, [this](){this->register_change_callback();})),
      next_table_tcam_actionbit_map_en_(default_adapter(next_table_tcam_actionbit_map_en_,chip_index(), pipeIndex, mauIndex, [this](){this->register_change_callback();})),
      next_stage_dependency_on_cur_(default_adapter(next_stage_dependency_on_cur_,chip_index(),pipeIndex,mauIndex))
{
  global_exec_ = 0;
  long_branch_ = 0;
  for (int i = 0; i < kThreads; i++) {
    next_table_[i] = NxtTab::inval_next_table();
    thread_global_exec_[i] = 0;
    thread_long_branch_[i] = 0;
  }
  mpr_always_run_.reset();
  mpr_glob_exec_lut_.reset();
  mpr_long_brch_lut_.reset();
  mpr_next_table_lut_.reset();
  mpr_stage_id_.reset();
  mpr_bus_dep_.reset();
  mpr_glob_exec_thread_.reset();
  mpr_long_brch_thread_.reset();
  mpr_thread_delay_.reset();
  pred_always_run_.reset();
  pred_glob_exec_thread_.reset();
  pred_map_glob_.reset();
  pred_map_loca_.reset();
  pred_miss_exec_.reset();
  pred_miss_long_brch_.reset();
  pred_long_brch_lt_src_.reset();
  pred_long_brch_terminate_.reset();
  pred_long_brch_thread_.reset();
  pred_ghost_thread_.reset();
  pred_is_a_brch_.reset();
  pred_stage_id_.reset();
  next_table_map_en_.reset();
  next_table_map_en_gateway_.reset();
  next_table_tcam_actionbit_map_en_.reset();
  next_stage_dependency_on_cur_.reset();
}
MauPredication::~MauPredication() {
}

void MauPredication::io_print(MauIO *io, const char *iodir) {
  RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "MAU_PRED<%d> %s "
              "PRED: NxtTabs=[0x%x,0x%x,0x%x] GEX=0x%04x LBR=0x%02x "
              " MPR: NxtTabs=[0x%x,0x%x,0x%x] GEX=0x%04x LBR=0x%02x \n",
              mau_->mau_index(), iodir,
              io->ingress_nxt_tab(), io->egress_nxt_tab(), io->ghost_nxt_tab(),
              io->global_exec(), io->long_branch(),
              io->ingress_mpr_nxt_tab(), io->egress_mpr_nxt_tab(), io->ghost_mpr_nxt_tab(),
              io->mpr_global_exec(), io->mpr_long_branch());
}
void MauPredication::register_check_lt_subset(int thrd, int idx1, int idx2,
                                              uint16_t gress_lts, uint16_t reg_lts,
                                              const char *reg, bool warn, bool eq) {
  bool err = (eq) ?(gress_lts != reg_lts) :((gress_lts & reg_lts) != reg_lts);
  if (err) {
    RMT_LOG_OBJ(mau_, (warn) ?RmtDebug::warn() :RmtDebug::error(kRelaxPredicationCheck),
                "MAU_PRED<%d> LTs (0x%04x) in %s[%s][%d][%d] "
                "are NOT %s %s LTs (0x%04x)\n",
                mau_->mau_index(), reg_lts, reg, thrd_str(thrd), idx1, idx2,
                (eq) ?"identical to" :"subset of", thrd_str(thrd), gress_lts);
  }
}
void MauPredication::register_check_lbr_subset(int thrd, int idx1, int idx2,
                                               uint8_t lbrs1, uint8_t lbrs2,
                                               const char *reg, bool warn, bool eq) {
  bool err = (eq) ?(lbrs1 != lbrs2) :((lbrs1 & lbrs2) != lbrs2);
  if (err) {
    RMT_LOG_OBJ(mau_, (warn) ?RmtDebug::warn() :RmtDebug::error(kRelaxPredicationCheck),
                "MAU_PRED<%d> long_branch bits (0x%02x) in %s[%s][%d][%d] "
                "are NOT %s %s long_branch (0x%02x)\n",
                mau_->mau_index(), lbrs2, reg, thrd_str(thrd), idx1, idx2,
                (eq) ?"identical to" :"subset of", thrd_str(thrd), lbrs1);
  }
}
void MauPredication::register_change_callback() {
  pending_check_seq_++;
}
bool MauPredication::register_config_change() {
  spinlock();
  bool change = (curr_check_seq_ < pending_check_seq_);
  curr_check_seq_ = pending_check_seq_;
  spinunlock();
  return change;
}
void MauPredication::register_config_check() {

  // 1a. First of all some sanity checks re LT thread config
  //
  // Check ghost LTs are a subset of configured ingress LTs
  RMT_ASSERT((lt_ingress_orig_ & lt_ghost_) == lt_ghost_);
  // Then setup lt_ingress_ to be only non-ghost ingress LTs
  lt_ingress_ = lt_ingress_orig_ & (kLtAll & ~lt_ghost_);

  // 1b. Now check no tables cited in more than one gress
  RMT_ASSERT((lt_ingress_ & lt_egress_) == 0);
  RMT_ASSERT((lt_ingress_ & lt_ghost_) == 0);
  RMT_ASSERT((lt_egress_ & lt_ghost_) == 0);

  uint16_t                         all_lts = lt_ingress_ | lt_ghost_ | lt_egress_;
  uint16_t                         unused_lts = kLtAll & ~all_lts;
  std::array< uint16_t, kThreads > thread_lts =  { lt_ingress_, lt_egress_, lt_ghost_ };
  std::array< uint8_t,  kThreads > thread_lbrs = { 0, 0, 0 };
  std::array< uint8_t,  kThreads > thread_lbrs_in = { 0, 0, 0 };
  std::array< uint8_t,  kThreads > thread_lbrs_out = { 0, 0, 0 };
  std::array< uint16_t, kThreads > thread_gex_out = { 0, 0, 0 };
  bool                             do_throw = false;

  // 1c. Finally some sanity checks re LT counter config
  //
  // Setup lt_counters_ to be only used LTs (switch off LTs not in any gress)
  lt_counters_ = lt_counters_orig_ & all_lts;
  // Warn if any configured counter LTs are unused LTs
  if ((lt_counters_orig_ & unused_lts) != 0) {
    RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                "MAU_PRED<%d> Counter using invalid LT - ignoring "
                "(valid LTs=0x%04x, counter LTs=0x%04x, counter LTs used=0x%04x)\n",
                mau_->mau_index(), all_lts, lt_counters_orig_, lt_counters_);
  }

  // 2. Basic check stage equality registers set up properly
  // Also check programming of regs that should match next stage dependency
  // No longer check mpr_stage_id - there are scenarios when that may differ.
  // (see email thread "jbay pred_always_run/mpr_always_run registers" 12Dec2017)
  //
  //RMT_ASSERT(mpr_stage_id_.mpr_stage_id(*) == mau_->mau_index());
  RMT_ASSERT(pred_stage_id_.pred_stage_id() == mau_->mau_index());
  MauIO *outIO = mau_->mau_io_output();
  Mau *nextMau = outIO->mau();
  if (nextMau != NULL) {
    uint8_t mpr_bd_ing = (next_mau_ingress_match_dep_) ?0 :1;
    uint8_t mpr_bd_egr = (next_mau_egress_match_dep_) ?0 :1;
    if ((mpr_bd_ing != mpr_bus_dep_.mpr_bus_dep_ingress()) ||
        (mpr_bd_egr != mpr_bus_dep_.mpr_bus_dep_egress())) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxPredicationCheck),
                  "MAU_PRED<%d> mpr_bus_dep_ingress OR mpr_bus_dep_egress mismatch! "
                  "ingress(expected=%d actual=%d) egress(expected=%d actual=%d)\n",
                  mau_->mau_index(),
                  mpr_bd_ing, mpr_bus_dep_.mpr_bus_dep_ingress(),
                  mpr_bd_egr, mpr_bus_dep_.mpr_bus_dep_egress());
      do_throw = true;
    }
  }

  // 3. Now need to check config of more complex regs.
  // First of all need to figure out which bits of long_branch_ bus
  // are used for ingress, egress and ghost in this stage.
  // Note this is NOT simply pred_long_brch_thread(thrd) as that
  // may also define long_branch bits that pass through this stage
  // unused
  //
  uint8_t lbr_term = pred_long_brch_terminate_.pred_long_brch_terminate();
  for (int thrd = 0; thrd < kThreads; thrd++) {
    // Get LTs and pred_long_brch_thread setting for thread
    uint16_t lt_gress = thread_lts[thrd];
    uint8_t  lbr = pred_long_brch_thread_.pred_long_brch_thread(thrd);
    for (int lt = 0; lt < kTables; lt++) {
      if (((lt_gress >> lt) & 1) == 1) {
        // For each LT in gress see what lbr bit selects it
        uint8_t en = pred_long_brch_lt_src_.enabled_3bit_muxctl_enable(lt);
        uint8_t sel = pred_long_brch_lt_src_.enabled_3bit_muxctl_select(lt);
        if ((en == 1) && (((lbr >> sel) & 1) == 1))
          thread_lbrs[thrd] |= (1<<sel); // Set that bit in per-thread lbr array
      }
    }
    // Track what LBRs arrived per-thread
    thread_lbrs_in[thrd] = lbr;
    // Figure out what LBRs will exit this stage
    // - will augment these later using pred_map_glob.
    // The thread_gex_out and thread_lbrs_out will be available for
    // downstream MAU to sanity check its LTs against.
    thread_lbrs_out[thrd] = lbr & ~lbr_term & 0xFF;
  }

  // 4. Now we know which LT/LBR bits are input to which gress in this stage
  // we can check config of various thread mask regs
  //
  MauDependencies *deps = mau_->mau_dependencies();
  uint8_t ing_lbr_in = thread_lbrs_in[kThreadIngress] | thread_lbrs_in[kThreadGhost];
  uint8_t egr_lbr_in = thread_lbrs_in[kThreadEgress];

  for (int ie = 0; ((ie == 0) || (ie == 1)); ie++) {
    bool ingress = (ie == 0);
    bool next_match_dep = (ingress) ?next_mau_ingress_match_dep_ :next_mau_egress_match_dep_;
    bool delayed_exp = next_match_dep;
    bool delayed_act = (mpr_thread_delay_.mpr_thread_delay(ie) > 0);
    if (delayed_exp != delayed_act) {
      RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                  "MAU_PRED<%d> %s mpr_thread_delay %sactive - expected %sactive\n",
                  mau_->mau_index(), thrd_str(ie), delayed_exp?"":"in", delayed_act?"":"in");
    }
    uint8_t del_exp = 0;
    if ((next_match_dep) && (deps != NULL))
      del_exp = deps->get_delay(ingress, MauDelay::kPostPredication) - 4; // Used to be -3
    uint8_t del_act = mpr_thread_delay_.mpr_thread_delay(ie);
    if (del_exp != del_act) {
      RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                  "MAU_PRED<%d> %s mpr_thread_delay set to %d - expected %d\n",
                  mau_->mau_index(), thrd_str(ie), del_act, del_exp);
    }
  }
  uint8_t  mpr_lbr_act = mpr_long_brch_thread_.mpr_long_brch_thread();
  uint8_t  mpr_lbr_exp = egr_lbr_in & ~ing_lbr_in & 0xFF;
  uint16_t mpr_gex_act = mpr_glob_exec_thread_.mpr_glob_exec_thread();
  uint16_t mpr_gex_exp = lt_egress_ & ~(lt_ingress_|lt_ghost_) & kLtAll;
  //RMT_ASSERT((mpr_lbr_act == mpr_lbr_exp) && (mpr_gex_act == mpr_gex_exp));
  if ((mpr_lbr_act != mpr_lbr_exp) || (mpr_gex_act != mpr_gex_exp)) {
    RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxPredicationCheck),
                "MAU_PRED<%d> mpr_glob_exec_thread OR mpr_long_brch_thread "
                "mismatch! glob_exec(expected=0x%04x actual=0x%04x) "
                "long_brch(expected=0x%02x actual=0x%02x)\n", mau_->mau_index(),
                mpr_gex_exp, mpr_gex_act, mpr_lbr_exp, mpr_lbr_act);
    do_throw = true;
  }

  // 5. If this is MAU0 check some LTs have power!
  // Won't complain in DV_MODE - DV often drives non-0 initial vals into MAU0
  //
  if ((Mau::must_have_always_run(mau_->mau_index())) &&
      (mpr_always_run_.mpr_always_run() == 0)) {
    RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                "MAU_PRED<%d> mpr_always_run is 0 so no LTs can run!\n",
                mau_->mau_index());
  }

  // 6. Now we can check that the output glob_exec_ long_branch_ buses
  // from the *previous* stage for each gress match up with this stages
  // uses of glob_exec_ and long_branch_
  //
  Mau *prevMau = (mau_->mau_index() > 0) ?mau_->mau_previous() :NULL;
  if (prevMau != NULL) {
    MauPredication *prev_pred = prevMau->mau_predication();
    for (int thrd = 0; thrd < kThreads; thrd++) {
      bool ing = ((thrd == kThreadIngress) || (thrd == kThreadGhost));
      if ((ing && this_mau_ingress_match_dep_) || (!ing && this_mau_egress_match_dep_)) {
        // Only check prev_gex/prev_lbr if gress is match dependent
        uint16_t lt_gress = thread_lts[thrd];
        uint8_t lbr_gress = thread_lbrs[thrd]; // LBRs *used* in this stage
        uint16_t prev_gex = 0;
        uint8_t  prev_lbr = 0;
        prev_pred->thread_output_info(thrd, &prev_gex, &prev_lbr);
        // Check prev stage *per-gress* global_exec is a subset of this stage gress LTs
        register_check_lt_subset(thrd, -1, -1, lt_gress, prev_gex,
                                 "previous_stage_global_exec",
                                 Mau::kRelaxPrevStageCheck, false); // May just warn
        // Check this stage *per-gress* LBRs are a subset of prev stage per-gress long_branch
        register_check_lbr_subset(thrd, -1, -1, prev_lbr, lbr_gress,
                                  "LBRs_used_this_stage",
                                  Mau::kRelaxPrevStageCheck, false); // May just warn
      }
    }
  }

  // 7. Check mpr_always_run (use thrd=kThreads to produce thrd name ALL)
  // and verify pred_always_run ORs to be a subset of mpr_always_run
  //
  uint16_t mpr_run = mpr_always_run_.mpr_always_run();
  register_check_lt_subset(kThreads, -1, -1, all_lts, mpr_run, "mpr_always_run", false);
  uint16_t pred_run = 0;
  for (int thrd = 0; thrd < kThreads; thrd++) {
    pred_run |= pred_always_run_.pred_always_run(thrd);
  }
  if ((mpr_run & pred_run) != pred_run) {
    RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxPredicationCheck),
                "MAU_PRED<%d> LTs in mpr_always_run (0x%04x) should be "
                "superset of LTs in pred_always_run (0x%04x)"
                "[0x%04x,0x%04x,0x%04x]\n", mau_->mau_index(), mpr_run, pred_run,
                pred_always_run_.pred_always_run(kThreadIngress),
                pred_always_run_.pred_always_run(kThreadEgress),
                pred_always_run_.pred_always_run(kThreadGhost));
  }

  // 8. Now we can verify config of various mpr_ and pred_ regs
  // Mostly these indexed by LT but some by LBR bit
  //
  for (int thrd = 0; thrd < kThreads; thrd++) {
    uint16_t lt_gress = thread_lts[thrd];
    uint8_t lbr_gress = thread_lbrs[thrd]; // LBR bits *used* this stage

    // Check pred_always_run
    register_check_lt_subset(thrd, -1, -1, lt_gress,
                             pred_always_run_.pred_always_run(thrd),
                             "pred_always_run", false);
    // Check pred_glob_exec_thread
    register_check_lt_subset(thrd, -1, -1, lt_gress,
                             pred_glob_exec_thread_.pred_glob_exec_thread(thrd),
                             "pred_glob_exec_thread", false);
    // Check mpr_long_brch_lut
    for (int bit = 0; bit < 8; bit++) {
      if (((lbr_gress >> bit) & 1) == 1) {
        register_check_lt_subset(thrd, bit, -1, lt_gress,
                                 mpr_long_brch_lut_.mpr_long_brch_lut(bit),
                                 "mpr_long_brch_lut", false);
      }
    }
    // Check mpr_glob_exec_lut, mpr_next_table_lut, pred_miss_loca_exec
    for (int lt = 0; lt < kTables; lt++) {
      if (((lt_gress >> lt) & 1) == 1) {
        uint16_t mask_equal_or_below_lt = kLtAll & ~mask_above(lt);
        uint16_t loca_exec = pred_miss_exec_.pred_miss_loca_exec(lt) << 1;
        // XXX: Disable this check - incorrect - bits in mpr_glob_exec
        //            do *not* correspond to LTs
        //register_check_lt_subset(thrd, lt, -1, lt_gress,
        //                         mpr_glob_exec_lut_.mpr_glob_exec_lut(lt),
        //                         "mpr_glob_exec_lut", false);
        register_check_lt_subset(thrd, lt, -1, lt_gress,
                                 mpr_next_table_lut_.mpr_next_table_lut(thrd, lt),
                                 "mpr_next_table_lut", false);
        register_check_lt_subset(thrd, lt, -1, lt_gress, loca_exec,
                                 "pred_miss_loca_exec", false);
        if ((loca_exec & mask_equal_or_below_lt) != 0) {
          RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                      "MAU_PRED<%d> some LTs in pred_miss_loca_exec[%d]=0x%04x are "
                      "less than or equal to LT=%d and so are ignored\n",
                      mau_->mau_index(), lt, loca_exec, lt);
        }

        // Check pred_map_loca_exec, pred_map_loca_next_table
        // Also issues some warnings if spurious bits set
        // (only check if map is enabled or gateway enabled for LT)
        bool en = (((next_table_map_en_.next_table_map_en() >> lt) & 1) == 1);
        bool en_gw = (((next_table_map_en_gateway_.next_table_map_en_gateway() >> lt) & 1) == 1);
        bool en_abit = (((next_table_tcam_actionbit_map_en_.next_table_tcam_actionbit_map_en() >> lt) & 1) == 1);

        if (en || en_gw || en_abit) {
          int lim = (en || en_gw) ?8 :2; // Only check 2 entries if just used for actionbit
          uint32_t map_ok = 0u;

          for (int map = 0; map < lim; map++) {
            map_ok |= 1<<map; // At outset assume ok, clear down if error

            uint16_t loca_exec = pred_map_loca_.pred_map_loca_exec(lt, map) << 1;

            // Just warn if ONLY loca_exec lo-bits (<= LT) are not in correct gress
            // (we're not so bothered about lo-bits as they are ignored at runtime)
            bool warn = ((lt_gress & loca_exec & mask_above(lt)) == (loca_exec & mask_above(lt)));
            register_check_lt_subset(thrd, lt, map, lt_gress, loca_exec,
                                     "pred_map_loca_exec", warn);
            // Also warn if loca_exec has ANY LTs below this LT
            if ((loca_exec & mask_equal_or_below_lt) != 0) {
              RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                          "MAU_PRED<%d> some LTs in pred_map_loca_exec[%d][%d]=0x%04x are "
                          "less than or equal to LT=%d and so are ignored\n",
                          mau_->mau_index(), lt, map, loca_exec, lt);
            }
            if (!warn) map_ok &= ~(1<<map); // Some hi-bits NOT in gress so clear map_ok bit
            if (!warn) do_throw = true;

            uint16_t nxt_tab = pred_map_loca_.pred_map_loca_next_table(lt, map);

            int nxt_tab_mau = NxtTab::which_mau(nxt_tab);
            if ((nxt_tab_mau == pred_stage_id_.pred_stage_id()) &&
                (((pred_is_a_brch_.pred_is_a_brch() >> lt) & 1) == 1)) {

              int      nxt_tab_lt = NxtTab::which_table(nxt_tab);
              uint16_t lt_nt = 1<<nxt_tab_lt;
              bool     ok_nt = (((lt_gress & lt_nt) == lt_nt) && (nxt_tab_lt > lt));

              // Log an error if nxt_tab_lt NOT in correct gress (warn if reset value)
              register_check_lt_subset(thrd, lt, map, lt_gress, lt_nt,
                                       "pred_map_loca_next_table", (nxt_tab==0));
              // Log an error if nxt_tab_lt <= this LT (warn if reset value)
              if (nxt_tab_lt <= lt) {
                RMT_LOG_OBJ(mau_, (nxt_tab==0) ?RmtDebug::warn() :RmtDebug::error(kRelaxPredicationCheck),
                            "MAU_PRED<%d> NxtTabLT=%d in pred_map_loca_next_table[%d][%d] "
                            "is less than or equal to LT=%d! (NxtTab=0x%x)\n",
                            mau_->mau_index(), nxt_tab_lt, lt, map, lt, nxt_tab);
              }
              // nxt_tab_lt NOT in correct gress OR <= this LT so clear map_ok bit
              if (!ok_nt) map_ok &= ~(1<<map);
              if (!ok_nt && (nxt_tab!=0)) do_throw = true;
            }
          } // for (int map = 0; map < lim; map++)

          // Log an error if there are NO valid map entries for the LT
          // (there should be at least ONE given the map/gateway_map is enabled)
          if (map_ok == 0u) {
            RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxPredicationCheck),
                        "MAU_PRED<%d> pred_map_loca_next_table[%d][0..7] "
                        "NO valid map entries at all for map-enabled LT %d\n",
                        mau_->mau_index(), lt, lt);
            do_throw = true;
          }


          // Now go through pred_map_glob and track output
          // global_exec and long_branch usage for each gress.
          // We use local vars so we know if there's been a change from last time.
          //
          // Some unterminated long_branches might already be on in thread_lbrs_out.
          //
          // Ultimately we should end up with the maximal possible set of glob_exec
          // and long_brch values for all threads - these should not overlap
          //
          for (int map = 0; map < lim; map++) {
            thread_gex_out[thrd]  |= pred_map_glob_.pred_map_glob_exec(lt, map);
            thread_lbrs_out[thrd] |= pred_map_glob_.pred_map_long_brch(lt, map);
          }

        } // if (en || en_gw || en_abit)

        // Likewise for pred_miss_exec/pred_miss_long_brch
        thread_gex_out[thrd]  |= pred_miss_exec_.pred_miss_glob_exec(lt);
        thread_lbrs_out[thrd] |= pred_miss_long_brch_.pred_miss_long_brch(lt);

      } // if (((lt_gress >> lt) & 1) == 1)
    } // for (int lt = 0; lt < kTables; lt++)
  } // for (int thrd = 0; thrd < kThreads; thrd++)


  // 9. Check no overlaps in output usage of global_exec or long_branch
  //
  if (((thread_gex_out[kThreadIngress]  & thread_gex_out[kThreadEgress])  != 0) ||
      ((thread_gex_out[kThreadIngress]  & thread_gex_out[kThreadGhost])   != 0) ||
      ((thread_gex_out[kThreadEgress]   & thread_gex_out[kThreadGhost])   != 0) ||
      ((thread_lbrs_out[kThreadIngress] & thread_lbrs_out[kThreadEgress]) != 0) ||
      ((thread_lbrs_out[kThreadIngress] & thread_lbrs_out[kThreadGhost])  != 0) ||
      ((thread_lbrs_out[kThreadEgress]  & thread_lbrs_out[kThreadGhost])  != 0)) {
    RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxPredicationCheck),
                "MAU_PRED<%d> multiple threads using same LT in global_exec output "
                "or same BIT in long_branch output (global_exec=[0x%04x,0x%04x,0x%04x]) "
                "long_branch=[0x%02x,0x%02x,0x%02x])\n", mau_->mau_index(),
                thread_gex_out[kThreadIngress], thread_gex_out[kThreadEgress],
                thread_gex_out[kThreadGhost], thread_lbrs_out[kThreadIngress],
                thread_lbrs_out[kThreadEgress], thread_lbrs_out[kThreadGhost]);
    do_throw = true;
  }
  // Now see if these output values have changed from last time
  bool changed = false;
  for (int thrd = 0; thrd < kThreads; thrd++) {
    if (thread_gex_out[thrd]  != thread_global_exec_[thrd]) changed = true;
    if (thread_lbrs_out[thrd] != thread_long_branch_[thrd]) changed = true;
  }
  // If so update and tick our curr_output_seq value so downstream MAU can spot delta
  if (changed) {
    for (int thrd = 0; thrd < kThreads; thrd++) {
      thread_global_exec_[thrd] = thread_gex_out[thrd];
      thread_long_branch_[thrd] = thread_lbrs_out[thrd];
    }
    curr_output_seq_++;
  }

  // 10. Should now be able to further check output control registers
  // The configuration of the mpr_bus_dep register should be consistent
  // with the per-thread output usage of global_exec and long_branch
  // if the next stage is match dependent
  //
  if (nextMau != NULL) {
    // mpr_bus_dep register fields have action-dep bits switched on, hence invert
    uint8_t mpr_bd_mdep_lbrs = 0xFF & ~mpr_bus_dep_.mpr_bus_dep_long_brch();
    uint16_t mpr_bd_mdep_gex = kLtAll & ~mpr_bus_dep_.mpr_bus_dep_glob_exec();
    uint8_t mdep_lbrs = 0;
    uint16_t mdep_gex = 0;
    if ((next_mau_ingress_match_dep_) && (mpr_bus_dep_.mpr_bus_dep_ingress() == 0)) {
      mdep_lbrs |= ( thread_lbrs_out[kThreadIngress] | thread_lbrs_out[kThreadGhost] );
      mdep_gex  |= ( thread_gex_out[kThreadIngress]  | thread_gex_out[kThreadGhost] );
    }
    if ((next_mau_egress_match_dep_) && (mpr_bus_dep_.mpr_bus_dep_egress() == 0)) {
      mdep_lbrs |= thread_lbrs_out[kThreadEgress];
      mdep_gex  |= thread_gex_out[kThreadEgress];
    }
    // Check match dependent bits on mpr bus are a superset of ones we output
    if (((mpr_bd_mdep_lbrs & mdep_lbrs) != mdep_lbrs) || ((mpr_bd_mdep_gex & mdep_gex) != mdep_gex)) {
      RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                  "MAU_PRED<%d> match dependent bits in mpr_bus_dep_glob_exec AND mpr_bus_dep_long_brch "
                  "should be equal to or a superset of the ones output from this stage! "
                  "glob_exec(mpr mdep bits=0x%04x  expected bits set=0x%04x) "
                  "long_brch(mpr mdep bits=0x%02x  expected bits set=0x%02x)\n",
                  mau_->mau_index(), mpr_bd_mdep_gex, mdep_gex, mpr_bd_mdep_lbrs, mdep_lbrs);
    }
  }

  // If there was a serious error (and we're not relaxing) throw now
  if (do_throw && !kRelaxPredicationCheck) { THROW_ERROR(-2); } // For DV
}


uint16_t MauPredication::get_powered_always_gex_lbr(uint16_t active_thread_lts,
                                                    uint16_t glob_exec, uint8_t long_brch) {
  uint16_t always_run_lts = mpr_always_run_.mpr_always_run();
  uint16_t long_brch_lts = 0;
  for (int bit = 0; bit < 8; bit++) {
    if (((long_brch >> bit) & 1) == 1)
      long_brch_lts |= mpr_long_brch_lut_.mpr_long_brch_lut(bit);
  }
  uint16_t glob_exec_lts = 0;
  for (int lt = 0; lt < kTables; lt++) {
    if (((glob_exec >> lt) & 1) == 1)
      glob_exec_lts |= mpr_glob_exec_lut_.mpr_glob_exec_lut(lt);
  }
  uint16_t always_gex_lbr_lts = glob_exec_lts | long_brch_lts | always_run_lts;
  return always_gex_lbr_lts;
}
uint16_t MauPredication::get_powered_next_tab(int thrd, int start_tab) {
  RMT_ASSERT((thrd >= 0) && (thrd < kThreads));
  int this_mau = mpr_stage_id_.mpr_stage_id(thrd); // Used to check against mau_->mau_index()
  int start_mau = NxtTab::which_mau(start_tab);
  int start_lt = NxtTab::which_table(start_tab);
  bool here = (start_mau == this_mau); // nxt-tab ignored if earlier/later mau
  uint16_t next_tab_lts = (here) ?mpr_next_table_lut_.mpr_next_table_lut(thrd, start_lt) :0;
  return next_tab_lts;
}
uint16_t MauPredication::get_powered_mpr(int ing_start, int egr_start, int ght_start,
                                         uint16_t glob_exec, uint8_t long_brch) {
  uint16_t always_gex_lbr_lts = get_powered_always_gex_lbr(kLtAll, glob_exec, long_brch);
  uint16_t ing_next_tab_lts   = get_powered_next_tab(kThreadIngress, ing_start);
  uint16_t egr_next_tab_lts   = get_powered_next_tab(kThreadEgress, egr_start);
  uint16_t ght_next_tab_lts   = get_powered_next_tab(kThreadGhost, ght_start);
  uint16_t all_lts = always_gex_lbr_lts| ing_next_tab_lts| egr_next_tab_lts| ght_next_tab_lts;
  // XXX: post mpr_ gating change, just hand back all MPR inputs
  // (see thread XXX mpr_* gating - 23Feb2018)
  return all_lts;
}

uint16_t MauPredication::get_nxtab_mask(int thrd, bool active, uint16_t gress, int nxt_tab) {
  RMT_ASSERT((thrd >= 0) && (thrd < kThreads));

  int this_mau = pred_stage_id_.pred_stage_id(); // Used to check against mau_->mau_index();
  int nxt_mau = NxtTab::which_mau(nxt_tab);
  int nxt_lt = NxtTab::which_table(nxt_tab);
  // No tables run as a result of nxt-tab if earlier/later mau so return 0
  if (nxt_mau != this_mau) return 0;
  if (!is_bit_set(gress, nxt_lt)) {
    if (active) { // Only report error if thread is active
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxPredicationCheck),
                  "MAU_PRED<%d> %s nxt_table %d not in %s LTs (0x%04x)\n",
                  mau_->mau_index(), thrd_str(thrd), nxt_lt, thrd_str(thrd), gress);
    }
    return 0;
  }
  return 1<<nxt_lt; // Only ever returns single LT
}
uint16_t MauPredication::get_active_mask(int thrd, bool active, uint16_t gress,
                                         int start_tab, uint16_t glob_exec, uint8_t long_brch) {
  RMT_ASSERT((thrd >= 0) && (thrd < kThreads));
  uint16_t always_run_lts = pred_always_run_.pred_always_run(thrd);
  uint8_t  long_brch_mask = pred_long_brch_thread_.pred_long_brch_thread(thrd);
  uint8_t  long_brch_masked = long_brch & long_brch_mask;
  uint16_t long_brch_lts = 0;
  for (int lt = 0; lt < kTables; lt++) {
    uint8_t en = pred_long_brch_lt_src_.enabled_3bit_muxctl_enable(lt);
    uint8_t sel = pred_long_brch_lt_src_.enabled_3bit_muxctl_select(lt);
    if ((en == 1) && (((long_brch_masked >> sel) & 1) == 1))
      long_brch_lts |= 1<<lt;
  }
  uint16_t glob_exec_mask = pred_glob_exec_thread_.pred_glob_exec_thread(thrd);
  uint16_t glob_exec_lts = glob_exec & glob_exec_mask;
  uint16_t nxt_tab_lt = mask_first( get_nxtab_mask(thrd, active, gress, start_tab) );
  uint16_t all_lts = nxt_tab_lt | glob_exec_lts | long_brch_lts | always_run_lts;
  return all_lts & gress;
}

bool MauPredication::other_config_change() {
  MauIO *inIO = mau_->mau_io_input();
  MauIO *outIO = mau_->mau_io_output();
  RMT_ASSERT(mau_ == inIO->mau());
  Mau   *nextMau = outIO->mau();
  Mau   *prevMau = mau_->mau_previous();
  MauPredication   *prevMauPred = (prevMau != NULL) ?prevMau->mau_predication() :NULL;
  MauDependencies  *deps = mau_->mau_dependencies();
  MauTableCounters *cntrs = mau_->mau_table_counters();
  uint16_t lt_ing = deps->lt_ingress();
  uint16_t lt_egr = deps->lt_egress();
  uint16_t lt_ght = pred_ghost_thread_.pred_ghost_thread();
  uint16_t lt_cntrs = cntrs->lt_with_counters();
  uint32_t prev_out_seq = (prevMauPred != NULL) ?prevMauPred->output_seq() :0u;

  bool this_ing_match_dep = mau_->is_match_dependent(true);
  bool this_egr_match_dep = mau_->is_match_dependent(false);
  // Derive initial value of next_ing|egr_match_dep from this MAU's CSR
  bool next_ing_match_dep = (next_stage_dependency_on_cur_.next_stage_dependency_on_cur(0) == 0);
  bool next_egr_match_dep = (next_stage_dependency_on_cur_.next_stage_dependency_on_cur(1) == 0);
  // But if nextMAU actually exists...
  if (nextMau != NULL) {
    MauDependencies *nextDeps = nextMau->mau_dependencies();
    uint16_t next_lts = nextDeps->lt_ingress() | nextDeps->lt_egress();
    // ... and if it has some LTs (suggesting it has been configured)
    if (next_lts != 0) {
      // then override based on actual configuration of nextMau
      next_ing_match_dep = nextMau->is_match_dependent(true);
      next_egr_match_dep = nextMau->is_match_dependent(false);
    }
  }

  // Has anything changed that might require us to re-validate config
  bool changed = ((lt_ing != lt_ingress_orig_) || (lt_cntrs != lt_counters_orig_) ||
                  (lt_egr != lt_egress_) || (lt_ght != lt_ghost_) ||
                  (prev_out_seq != prev_mau_output_seq_) ||
                  (this_ing_match_dep != this_mau_ingress_match_dep_) ||
                  (this_egr_match_dep != this_mau_egress_match_dep_) ||
                  (next_ing_match_dep != next_mau_ingress_match_dep_) ||
                  (next_egr_match_dep != next_mau_egress_match_dep_));
  if (changed) {
    // Update member vars
    lt_ingress_orig_ = lt_ing;
    lt_counters_orig_ = lt_cntrs;
    // We may update lt_ingress_ and lt_counters_ in register_config_check()
    lt_egress_ = lt_egr;
    lt_ghost_ = lt_ght;
    prev_mau_output_seq_ = prev_out_seq;
    this_mau_ingress_match_dep_ = this_ing_match_dep;
    this_mau_egress_match_dep_ = this_egr_match_dep;
    next_mau_ingress_match_dep_ = next_ing_match_dep;
    next_mau_egress_match_dep_ = next_egr_match_dep;
  }
  return changed;
}

void MauPredication::start(bool thread_active[]) {
  MauIO *inIO = mau_->mau_io_input();
  RMT_ASSERT(mau_ == inIO->mau());
  bool do_throw = false;
  bool changed = false;

  // Run register validation checks till changes stop
  while (other_config_change() || register_config_change()) {
    register_config_check();
    changed = true;
  }
  // After this all member vars should be setup properly

  bool ing_active = thread_active[kThreadIngress];
  bool egr_active = thread_active[kThreadEgress];
  bool ght_active = thread_active[kThreadGhost];
  threads_active_ = ((ing_active) ?(1<<kThreadIngress) :0) |
      ((egr_active) ?(1<<kThreadEgress) :0) | ((ght_active) ?(1<<kThreadGhost) :0);

  // All LTs, unused LTs, LTs in active threads (aka kMprMask/kActiveMask(
  uint16_t lt_all = lt_ingress_ | lt_egress_ | lt_ghost_;
  uint16_t lt_unused = kLtAll & ~lt_all;
  uint16_t lt_gress = ((ing_active) ?lt_ingress_ :0) |
      ((egr_active) ?lt_egress_ :0) | ((ght_active) ?lt_ghost_ :0);


  //uint8_t mode = (lt_counters_ != 0) ?kModeEvaluateAll :get_run_mode();
  uint8_t  mode = get_run_mode();

  // Find all LTs we can do full lookup in - ie powered LTs
  // This depends on ingress/egress/ghost mpr_nxt_tab, mpr_glob_exec,
  // mpr_long_branch I/O values and the mpr_always_run register config
  int      mpr_ing_nxt_tab = inIO->ingress_mpr_nxt_tab();
  int      mpr_egr_nxt_tab = inIO->egress_mpr_nxt_tab();
  int      mpr_ght_nxt_tab = inIO->ghost_mpr_nxt_tab();
  uint16_t mpr_glob_exec   = inIO->mpr_global_exec();
  uint8_t  mpr_long_branch = inIO->mpr_long_branch();

  uint16_t mpr_lts = get_powered_mpr(mpr_ing_nxt_tab, mpr_egr_nxt_tab, mpr_ght_nxt_tab,
                                     mpr_glob_exec, mpr_long_branch);
  uint16_t lt_lookup_all = mpr_lts & lt_gress;
  uint16_t lt_unpowered = kLtAll & ~lt_lookup_all;
  // Warn if no LTs in active threads at all can be looked up
  if ((lt_gress != 0) && (lt_lookup_all == 0)) {
    RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                "MAU_PRED<%d> No LTs powered for lookup!\n", mau_->mau_index());
  }

  // Say something if counter LTs are not available for full lookup
  // (ie use unpowered LTs). But restrict check to tblcounter LTs
  if ((lt_counters_ & lt_gress & lt_unpowered) != 0) {
    MauTableCounters *cntrs = mau_->mau_table_counters();
    uint16_t lt_counters_tblcntrs = lt_counters_ & cntrs->lt_with_tblcounters();
    if ((lt_counters_tblcntrs & lt_unpowered) != 0) {
      // Record unavailable tblcounter LTs and warn, if used, later
      if (changed) lt_warn_ = lt_counters_tblcntrs & lt_unpowered;
      RMT_LOG_OBJ(mau_, RmtDebug::info(),
                  "MAU_PRED<%d> TblCounter using power-saved LT - table lookup will "
                  "appear to always miss (powered LTs=0x%04x, tblcounter LTs=0x%04x)\n",
                  mau_->mau_index(), lt_lookup_all, lt_counters_tblcntrs);
    }
  }

  // Find initial set of LTs
  // This depends on ingress/egress/ghost nxt_tab, glob_exec,
  // long_branch I/O values and the pred_always_run register config
  // Note, only start values of ing/egr/ght nxt_tab included
  int      pred_ing_nxt_tab = inIO->ingress_nxt_tab();
  int      pred_egr_nxt_tab = inIO->egress_nxt_tab();
  int      pred_ght_nxt_tab = inIO->ghost_nxt_tab();
  uint16_t pred_glob_exec   = inIO->global_exec();
  uint8_t  pred_long_branch = inIO->long_branch();
  uint8_t  pred_long_branch_term = pred_long_brch_terminate_.pred_long_brch_terminate();

  uint16_t lt_active_ing = get_active_mask(kThreadIngress, ing_active, lt_ingress_,
                                           pred_ing_nxt_tab, pred_glob_exec, pred_long_branch);
  uint16_t lt_active_egr = get_active_mask(kThreadEgress, egr_active, lt_egress_,
                                           pred_egr_nxt_tab, pred_glob_exec, pred_long_branch);
  uint16_t lt_active_ght = get_active_mask(kThreadGhost, ght_active, lt_ghost_,
                                           pred_ght_nxt_tab, pred_glob_exec, pred_long_branch);
  uint16_t lt_active_all = lt_active_ing | lt_active_egr | lt_active_ght; // All threads

  // Complain if any of our active tables are not runnable - check for all threads
  if ((lt_lookup_all & lt_active_all & lt_gress) != (lt_active_all & lt_gress)) {
    bool warn = kAllowUnpoweredTablesToBecomeActive;
    RMT_LOG_OBJ(mau_, (warn) ?RmtDebug::warn() :RmtDebug::error(kRelaxPredicationCheck),
                "MAU_PRED<%d> Some initial predicated LTs are NOT powered so are unavailable "
                "for match lookup (predicated LTs=0x%04x)(powered LTs=0x%04x)\n",
                mau_->mau_index(), lt_active_all, lt_lookup_all);
    if (!warn) do_throw = true;
  }

  // Figure out countable/lookupable/active tables dependent on threads active
  lt_countable_ = lt_counters_ & lt_gress;

  // Now we know what LTs are available for our active threads
  // warn if we see LTs powered outside this set (XXX)
  // TODO:REMOVE: SHOULD NEVER HAPPEN post XXX mpr_* gating - 23Feb2018
  uint16_t lt_thread_inactive =  kLtAll & ~lt_gress;
  if ((lt_lookup_all & lt_thread_inactive) != 0) {
    bool warn = ((mau_->mau_index() == 0) && (kAllowUnpoweredTablesToBecomeActive));
    RMT_LOG_OBJ(mau_, (warn) ?RmtDebug::warn() :RmtDebug::error(kRelaxPredicationCheck),
                "MAU_PRED<%d> Some LTs NOT belonging to active threads are powered "
                "(ThreadsActive=%s)(LTs in active threads=0x%04x)(LTs powered=0x%04x)\n",
                mau_->mau_index(), thrdmask_str(threads_active_), lt_gress, lt_lookup_all);
  }

  // Store original set of LTs selected by MPR IOs and mpr_always_run
  lt_mpr_ = mpr_lts; // LTs from MPR
  lt_mpr_mask_ = lt_gress; // LTs in active threads
  lt_lookupable_ = lt_lookup_all; // mpr_lts & lt_gress
  RMT_ASSERT(!kPowerTablesForInactiveThreads);

  lt_active_ = ((ing_active) ?lt_active_ing :0) |
      ((egr_active) ?lt_active_egr :0) | ((ght_active) ?lt_active_ght :0);
  // Maybe fix up initial lt_active_ to only contain powered lts
  if (!kAllowUnpoweredTablesToBecomeActive) lt_active_ &= lt_lookupable_;

  // Find all LTs we should run - OR in counters as they run unconditionally
  switch (mode) {
    case kModeEvaluateAll:  lt_runnable_ = lt_lookupable_ | lt_countable_; break;
    case kModeShortCircuit: lt_runnable_ = lt_active_     | lt_countable_; break;
  }
  // Double-check we're not trying to use invalid LTs
  RMT_ASSERT((lt_runnable_ & lt_unused) == 0);



  // Finally set up initial output next_table_ vals
  next_table_[kThreadIngress] = pred_ing_nxt_tab;
  next_table_[kThreadEgress] = pred_egr_nxt_tab;
  next_table_[kThreadGhost] = pred_ght_nxt_tab;
  // and long_branch, global_exec vals (though some long branches may terminate here)
  long_branch_ = pred_long_branch & ~pred_long_branch_term & 0xFF;
  global_exec_ = 0;
  // These may all be modified by calls to set_next_table()
  // All of them will be output to next mau via MauIO in end() func

  io_print(inIO, "InputIO"); // Verbose info re inIO
  RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "MAU_PRED<%d>  "
              "ThreadsActive=%s[%c,%c,%c] LTs=[0x%04x,0x%04x,0x%04x] "
              "InitialActiveLTs=0x%04x[0x%04x,0x%04x,0x%04x] "
              "PoweredLTs=0x%04x[0x%04x,0x%04x,0x%04x] %s%s\n",
              mau_->mau_index(), thrdmask_str(threads_active_),
              (ing_active)?'T':'f', (egr_active)?'T':'f', (ght_active)?'T':'f',
              lt_ingress_, lt_egress_, lt_ghost_,
              lt_active_, lt_active_ing, lt_active_egr, lt_active_ght,
              lt_lookupable_,
              lt_lookupable_ & lt_ingress_, lt_lookupable_ & lt_egress_, lt_lookupable_ & lt_ghost_,
              ((lt_lookup_all & lt_thread_inactive) != 0)?"(LTs powered for inactive threads)":"",
              (kAllowUnpoweredTablesToBecomeActive)?"(Unpowered LTs CAN become active)":"");

  // If there was a serious error (and we're not relaxing) throw now
  if (do_throw && !kRelaxPredicationCheck) { THROW_ERROR(-2); } // For DV
}

void MauPredication::end() {
  MauIO *inIO = mau_->mau_io_input();
  MauIO *outIO = mau_->mau_io_output();
  outIO->reset_pred();
  outIO->set_ingress_nxt_tab(next_table_[kThreadIngress]);
  outIO->set_egress_nxt_tab(next_table_[kThreadEgress]);
  outIO->set_ghost_nxt_tab(next_table_[kThreadGhost]);
  outIO->set_long_branch(long_branch_);
  outIO->set_global_exec(global_exec_);

  // Either copy MPR buses in->out (action dep case)
  // or duplicate PRED outputs to MPR buses (match dep case)
  if (mpr_bus_dep_.mpr_bus_dep_ingress() == 0) {
    outIO->set_ingress_mpr_nxt_tab(next_table_[kThreadIngress]);
    outIO->set_ghost_mpr_nxt_tab(next_table_[kThreadGhost]);
  } else {
    outIO->set_ingress_mpr_nxt_tab(inIO->ingress_mpr_nxt_tab());
    outIO->set_ghost_mpr_nxt_tab(inIO->ghost_mpr_nxt_tab());
  }
  if (mpr_bus_dep_.mpr_bus_dep_egress() == 0) {
    outIO->set_egress_mpr_nxt_tab(next_table_[kThreadEgress]);
  } else {
    outIO->set_egress_mpr_nxt_tab(inIO->egress_mpr_nxt_tab());
  }

  // Handle long_branch
  uint8_t lbr_action_dep_mask = mpr_bus_dep_.mpr_bus_dep_long_brch();
  uint8_t lbr_match_dep_mask =  0xFF & ~lbr_action_dep_mask;
  uint8_t lbr_action_dep_out = inIO->mpr_long_branch() & lbr_action_dep_mask;
  uint8_t lbr_match_dep_out  = long_branch_ & lbr_match_dep_mask;
  outIO->set_mpr_long_branch(lbr_match_dep_out | lbr_action_dep_out);

  // Handle global_exec
  uint16_t gex_action_dep_mask = mpr_bus_dep_.mpr_bus_dep_glob_exec();
  uint16_t gex_match_dep_mask = kLtAll & ~gex_action_dep_mask;
  uint16_t gex_action_dep_out = inIO->mpr_global_exec() & gex_action_dep_mask;
  uint16_t gex_match_dep_out  = global_exec_ & gex_match_dep_mask;
  outIO->set_mpr_global_exec(gex_match_dep_out | gex_action_dep_out);

  //lt_countable_ = lt_lookupable_ = lt_runnable_ = 0; // Maybe cleanup
  // Leave lt_{ingress_,egress_,ghost_,counters_,warn_,active_} for examination
  for (int i = 0; i < kThreads; i++) next_table_[i] = NxtTab::inval_next_table();
  long_branch_ = global_exec_ = 0;

  io_print(outIO, "OutputIO"); // Verbose info re outIO
  RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "MAU_PRED<%d>  "
              "ThreadsActive=%s LTs=[0x%04x,0x%04x,0x%04x] FinalActiveLTs=0x%04x\n",
              mau_->mau_index(), thrdmask_str(threads_active_),
              lt_ingress_, lt_egress_, lt_ghost_, lt_active_);
}

int MauPredication::get_next_table(bool ingress, int curr_lt, bool *do_lookup) {
  RMT_ASSERT((curr_lt >= -1) && (curr_lt < kTables) && (do_lookup != NULL));
  uint16_t lt_gress = ingress ?lt_ingress_|lt_ghost_ :lt_egress_;
  RMT_ASSERT((curr_lt == -1) || (is_bit_set(lt_gress, curr_lt)));
  // Find first table *greater than* curr_lt that's runnable and in correct gress
  int next_lt = find_first( mask_above(curr_lt) & lt_gress & lt_runnable_ );
  bool lookupable = is_bit_set(lt_lookupable_, next_lt); // Can we do full-lookup?
  if ((next_lt >= 0) && (!lookupable) && (is_bit_set(lt_warn_, next_lt))) {
    lt_warn_ &= ~(1<<next_lt); // Only warn on first use
    RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                "MAU_PRED<%d> get_next_table(%s,%d)=%d but LT %d is unpowered so "
                "unavailable for table lookup (will appear to always MISS)\n",
                mau_->mau_index(), thrd_str(ingress?0:1), curr_lt, next_lt, next_lt);
  }
  *do_lookup = lookupable;
  return next_lt;
}
int MauPredication::get_first_table(bool ingress, bool *do_lookup) {
  return get_next_table(ingress, -1, do_lookup);
}

void MauPredication::set_next_table(int lt, const MauLookupResult &result) {
  uint16_t nxt_tab_mapped = result.next_table_form();
  uint16_t nxt_tab_dflt_mask = result.next_table_mask();
  // Bit of sanity checking
  RMT_ASSERT((lt >= 0) && (lt < kTables));
  RMT_ASSERT(is_bit_set(lt_runnable_, lt)); // ThreadActive/Powered/Countable LTs
  bool ing = is_bit_set(lt_ingress_, lt);
  bool egr = is_bit_set(lt_egress_, lt);
  bool ght = is_bit_set(lt_ghost_, lt);

  // Was table just run active? If not return. We only honour nxt_tab for active tables.
  if (!is_bit_set(lt_active_, lt)) return;

  // If table just run is unpowered insist result is a miss
  if (!is_bit_set(lt_lookupable_, lt)) RMT_ASSERT(result.miss());

  // Figure out thread and what LTs are set in thread gress
  int thrd = -1;
  uint16_t lt_gress = 0;
  if (ing) {
    thrd = kThreadIngress; lt_gress = lt_ingress_;
  } else if (egr) {
    thrd = kThreadEgress; lt_gress = lt_egress_;
  } else if (ght) {
    thrd = kThreadGhost; lt_gress = lt_ghost_;
  }

  // Update next_table but only if this LT is a branch
  if (is_bit_set(pred_is_a_brch_.pred_is_a_brch(), lt))
    next_table_[thrd] = nxt_tab_mapped;

  // Update global_exec_, long_branch_ and lt_active/lt_runnable_ (local_exec equiv)
  // if right combination of match/en inhibit/en_gw
  // NB loca_exec doesn't have a bit for LT 0 hence the <<1 below
  bool en = (((next_table_map_en_.next_table_map_en() >> lt) & 1) == 1);
  bool en_gw = (((next_table_map_en_gateway_.next_table_map_en_gateway() >> lt) & 1) == 1);
  bool en_abit = (((next_table_tcam_actionbit_map_en_.next_table_tcam_actionbit_map_en() >> lt) & 1) == 1);
  int map_entry = -1;
  uint16_t local_exec = 0;
  if ((result.match() && !result.gatewayinhibit() && (en || en_abit) ) || (result.gatewayinhibit() && en_gw)) {
    map_entry = nxt_tab_dflt_mask % 8;
    global_exec_ |= pred_map_glob_.pred_map_glob_exec(lt, map_entry);
    local_exec    = pred_map_loca_.pred_map_loca_exec(lt, map_entry) << 1;
    long_branch_ |= pred_map_glob_.pred_map_long_brch(lt, map_entry);
    // Double check we mapped nxt_tab_dflt_mask -> nxt_tab_mapped correctly
    if (is_bit_set(pred_is_a_brch_.pred_is_a_brch(), lt)) {
      RMT_ASSERT(nxt_tab_mapped == pred_map_loca_.pred_map_loca_next_table(lt, map_entry));
    }
  } else if (result.miss()) {
    global_exec_ |= pred_miss_exec_.pred_miss_glob_exec(lt);
    local_exec    = pred_miss_exec_.pred_miss_loca_exec(lt) << 1;
    long_branch_ |= pred_miss_long_brch_.pred_miss_long_brch(lt);
  }
  local_exec &= mask_above(lt); // Fixup
  // Check local exec only contains powered LTs - maybe fix
  if ((lt_lookupable_ & local_exec) != local_exec) {
    uint16_t local_exec_prev = local_exec;
    if (!kAllowUnpoweredTablesToBecomeActive) local_exec &= lt_lookupable_;
    bool warn = kAllowUnpoweredTablesToBecomeActive;
    RMT_LOG_OBJ(mau_, (warn) ?RmtDebug::warn() :RmtDebug::error(kRelaxPredicationCheck),
                "MAU_PRED<%d> some LTs in loca_exec[%d][%d]=0x%04x are "
                "NOT powered (powered %s LTs=0x%04x) (using loca_exec=0x%04x)\n",
                mau_->mau_index(), lt, map_entry, local_exec_prev, thrd_str(thrd),
                lt_gress & lt_lookupable_, local_exec);
  }
  // Check local exec only contains LTs in same thread as input lt - fix - maybe bail
  if ((lt_gress & local_exec) != local_exec) {
    uint16_t local_exec_prev = local_exec;
    local_exec &= lt_gress;
    RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxPredicationCheck),
                "MAU_PRED<%d> some LTs in loca_exec[%d][%d]=0x%04x are "
                "NOT in same gress (%s) as LT=%d (using loca_exec=0x%04x)\n",
                mau_->mau_index(), lt, map_entry, local_exec_prev, thrd_str(thrd), lt,
                local_exec);
    if (!kRelaxPredicationCheck) { THROW_ERROR(-2); } // For DV
  }
  // Update lt_active_ with local_exec vals (can include unpowered LTs)
  lt_active_   |= local_exec;
  lt_runnable_ |= local_exec; // Only needed for ShortCircuit mode

  // Is pred_is_a_brch unset - if so we finish now - this LTs
  // nxt-tab won't get added to lt_active_ - so it'll only be
  // run if it was in local_exec
  if (!is_bit_set(pred_is_a_brch_.pred_is_a_brch(), lt)) return;

  // Is nxt_tab in different stage - if so return
  // (see Pat's email "small functional change for timing" 09 Aug 2015)
  int this_mau = mau_->mau_index();
  int nxt_tab_mau = NxtTab::which_mau(nxt_tab_mapped);
  if (nxt_tab_mau != this_mau) return;

  // So pointing at another LT in *this* stage
  // Figure out what LTs in this stage could be active next tables
  int nxt_tab_lt = NxtTab::which_table(nxt_tab_mapped);
  RMT_ASSERT((nxt_tab_lt >= 0) && (nxt_tab_lt < kTables));
  uint16_t lt_nxt_avail = mask_above(lt) & lt_gress;
  if (!kAllowUnpoweredTablesToBecomeActive) lt_nxt_avail &= lt_lookupable_;

  // Warn/Error if NxtTab unpowered
  if (!is_bit_set(lt_lookupable_, nxt_tab_lt)) {
    bool warn = kAllowUnpoweredTablesToBecomeActive;
    RMT_LOG_OBJ(mau_, (warn) ?RmtDebug::warn() :RmtDebug::error(kRelaxPredicationCheck),
                "MAU_PRED<%d> NxtTabLT=%d is NOT powered so will always MISS on "
                "match lookup (powered %s LTs=0x%04x)(NxtTab=0x%x)(thisLT=%d)\n",
                mau_->mau_index(), nxt_tab_lt, thrd_str(thrd),
                lt_gress & lt_lookupable_, nxt_tab_mapped, lt);
  }
  // TODO:JBAY:STRIP these and ref to lt_nxt_avail2 below
  //uint16_t lt_nxt_avail2 = lt_nxt_avail & mask_above(nxt_tab_lt);

  bool gotnxt = is_bit_set(lt_nxt_avail, nxt_tab_lt);
  if (!gotnxt) {
    // nxt_tab_lt not available - why?
    if (!is_bit_set(lt_gress, nxt_tab_lt)) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxPredicationCheck),
                  "MAU_PRED<%d> NxtTabLT=%d is invalid (NxtTab=0x%x)(thisLT=%d)"
                  "(%sLTs=0x%04x)\n", mau_->mau_index(), nxt_tab_lt,
                  nxt_tab_mapped, lt, thrd_str(thrd), lt_gress);
      if (!kRelaxPredicationCheck) { THROW_ERROR(-2); } // For DV
    }
    return;
    //
    // TODO:JBAY:STRIP
    // NOTE, JBay doesn't search on for a runnable LT so this logic is
    // inapplicable - leave in but commented out for the moment
    //
    // Given nxt_tab_lt is not in the potential active set, find a nxt_tab_lt
    // that is, but search *above* the one we specified in nxt_tab
    //nxt_tab_lt = find_first(lt_nxt_avail2);
    //gotnxt = is_bit_set(lt_nxt_avail2, nxt_tab_lt);
    //if (!gotnxt) {
    //   RMT_LOG_OBJ(mau_, RmtDebug::warn(),
    //               "MAU_PRED<%d> NxtTabLT=%d unavailable and no further %s LTs "
    //               "available for match in this stage (powered %s LTs=0x%04x)"
    //               "(NxtTab=0x%x)(thisLT=%d)\n",
    //               mau_->mau_index(), NxtTab::which_table(nxt_tab_mapped),
    //               thrd_str(thrd), thrd_str(thrd),
    //               lt_gress & lt_lookupable_, nxt_tab_mapped, lt);
    // // If no further possible active tables in this stage let next stage find next table
    //   return;
    // }
    //
  }
  // Found a next table in this stage so update lt_active_/lt_runnable_ again
  lt_active_   |= (1<<nxt_tab_lt);
  lt_runnable_ |= (1<<nxt_tab_lt); // Only needed for ShortCircuit mode
  return;
}

uint16_t MauPredication::lt_info(uint16_t pred_sel) {
  if (pred_sel == 0) return 0;
  uint16_t lts = kLtAll;
  if ((pred_sel & Pred::kIngress) != 0)    lts &=   lt_ingress_;
  if ((pred_sel & Pred::kGhost) != 0)      lts &=   lt_ghost_;
  if ((pred_sel & Pred::kEgress) != 0)     lts &=   lt_egress_;
  if ((pred_sel & Pred::kCounters) != 0)   lts &=   lt_counters_;
  if ((pred_sel & Pred::kCountable) != 0)  lts &=   lt_countable_;
  if ((pred_sel & Pred::kLookupable) != 0) lts &=   lt_lookupable_;
  if ((pred_sel & Pred::kRunnable) != 0)   lts &=   lt_runnable_;
  if ((pred_sel & Pred::kActive) != 0)     lts &=   lt_active_;
  if ((pred_sel & Pred::kWarn) != 0)       lts &=   lt_warn_;
  if ((pred_sel & Pred::kUsed) != 0)       lts &=  (lt_ingress_|lt_egress_|lt_ghost_);
  if ((pred_sel & Pred::kUnused) != 0)     lts &= ~(lt_ingress_|lt_egress_|lt_ghost_);
  if ((pred_sel & Pred::kUnpowered) != 0)  lts &= ~(lt_lookupable_);
  if ((pred_sel & Pred::kIngThread) != 0)  lts &=  (lt_ingress_|lt_ghost_);
  if ((pred_sel & Pred::kEgrThread) != 0)  lts &=  (lt_egress_);
  if ((pred_sel & Pred::kMpr) != 0)        lts &=   lt_mpr_;
  if ((pred_sel & Pred::kMprMask) != 0)    lts &=   lt_mpr_mask_;
  return lts;
}
uint16_t MauPredication::lts_active() {
  return lt_info(Pred::kActive);
}
uint32_t MauPredication::output_seq() {
  return curr_output_seq_;
}
void MauPredication::thread_output_info(int thrd,
                                        uint16_t *global_exec, uint8_t *long_branch) {
  RMT_ASSERT((thrd >= 0) && (thrd < kThreads));
  RMT_ASSERT((global_exec != NULL) && (long_branch != NULL));
  *global_exec = thread_global_exec_[thrd];
  *long_branch = thread_long_branch_[thrd];
}


}
