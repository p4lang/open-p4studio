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
#include <mau-dependencies.h>
#include <register_adapters.h>



namespace MODEL_CHIP_NAMESPACE {

  MauDependencies01::MauDependencies01(int chipIndex, int pipeIndex, int mauIndex,
                                       int which, uint8_t ieflag, Mau *mau,
                                       RegisterCallback& write_callback)
      : //Removed in bfnregs 20150107_182406_7982_mau_dev
        //final_output_delay_(chipIndex,pipeIndex,mauIndex,which,0,write_callback),
        predication_ctl_(default_adapter(predication_ctl_,chipIndex,pipeIndex,mauIndex,which,write_callback)),
        action_output_delay_(default_adapter(action_output_delay_,chipIndex,pipeIndex,mauIndex,which,write_callback)),
        cur_stage_dependency_on_prev_(default_adapter(cur_stage_dependency_on_prev_,chipIndex,pipeIndex,mauIndex,which,write_callback)),
        next_stage_dependency_on_cur_(default_adapter(next_stage_dependency_on_cur_,chipIndex,pipeIndex,mauIndex,which,write_callback)),
        pipelength_added_stages_(default_adapter(pipelength_added_stages_,chipIndex,pipeIndex,mauIndex,which,write_callback)),
        deferred_eop_bus_delay_(default_adapter(deferred_eop_bus_delay_,chipIndex,pipeIndex,mauIndex,which,write_callback)),
        write_callback_(write_callback),
        mau_(mau), prev_gress_deps_(NULL), next_gress_deps_(NULL),
        which_(which), ieflag_(ieflag),
        action_output_enabled_(false), final_output_enabled_(false),
        start_table_fifo0_enabled_(false), start_table_fifo1_enabled_(false),
        verify_table_thread_(false), table_thread_(0),
        cur_stage_dep_(0), next_stage_dep_(0),
        stage_features_(0u), dependency_features_(0u), series_features_(0u) {

    update_dependency_features_this();
    update_series_features_this(0u);
    //final_output_delay_.reset();
    predication_ctl_.reset();
    action_output_delay_.reset();
    cur_stage_dependency_on_prev_.reset();
    next_stage_dependency_on_cur_.reset();
    pipelength_added_stages_.reset();
    deferred_eop_bus_delay_.reset();
  }
  MauDependencies01::~MauDependencies01() { }

  void MauDependencies01::update_info_on_write() {
    start_table_fifo0_enabled_ =
        ((predication_ctl_.start_table_fifo_enable() & 0x1) != 0);
    start_table_fifo1_enabled_ =
        ((predication_ctl_.start_table_fifo_enable() & 0x2) != 0);
    verify_table_thread_ = true;
    table_thread_ = predication_ctl_.table_thread();

    uint8_t cur_dep = cur_stage_dependency_on_prev_.cur_stage_dependency_on_prev();
    uint8_t nxt_dep = next_stage_dependency_on_cur_.next_stage_dependency_on_cur();
    RMT_ASSERT(is_valid_dependency_mode(cur_dep));
    RMT_ASSERT(is_valid_dependency_mode(nxt_dep));

    // If dependency config is unchanged we're done - bail out
    if ((cur_stage_dep_ == cur_dep) && (next_stage_dep_ == nxt_dep)) return;

    // Otherwise update dependency vars and dependency features
    cur_stage_dep_ = cur_dep;
    next_stage_dep_ = nxt_dep;
    update_dependency_features_this();

    // and re-evaluate series features
    update_series_features();
  }


  void MauDependencies01::update_dependency_features_this() {
    // Maintain dependency features separately from stage_features
    // as it does not make sense for these to be disseminated across a series
    uint32_t features = 0u;
    if (mau_->mau_index() == 0)    features |= MauDefs::kMauCurrStageIsMau0;
    if      (cur_stage_dep_ == 0)  features |= MauDefs::kMauCurrStageMatchDep;
    else if (cur_stage_dep_ == 1)  features |= MauDefs::kMauCurrStageActionDep;
    if      (next_stage_dep_ == 0) features |= MauDefs::kMauNextStageMatchDep;
    else if (next_stage_dep_ == 1) features |= MauDefs::kMauNextStageActionDep;
    dependency_features_ = features;
  }
  void MauDependencies01::update_series_features_this(uint32_t features) {
    series_features_ = features;
  }

  void MauDependencies01::dump_delay_state_this(uint64_t flgs) {
    int stage = mau_->mau_index();
    RMT_LOG_OBJ(mau_, flgs, "dump_delay_state: %sMAU<%02d,%s> Ftrs=0x%06x Dep=%1d "
                "Delays(Tot=%2d Pred=%2d Tcam=%2d<%2d> Sel=%2d<%2d> MLpf=%2d<%2d> Stfl=%2d<%2d> "
                "  series<stageRaw>)\n",
                (stage > 9) ?"" :" ", stage, iestr(), stage_features(), cur_stage_dep_,
                get_delay(MauDelay::kPipeLatency),    get_delay(MauDelay::kPredication),
                get_delay(MauDelay::kSeriesTcam),     get_delay(MauDelay::kTcamRaw),
                get_delay(MauDelay::kSeriesSelector), get_delay(MauDelay::kSelectorRaw),
                get_delay(MauDelay::kSeriesMeterLpf), get_delay(MauDelay::kMeterLpfRaw),
                get_delay(MauDelay::kSeriesStateful), get_delay(MauDelay::kStatefulRaw));
  }
  void MauDependencies01::dump_delay_state_prev(uint64_t flgs, bool this_too) {
    if (prev_gress_deps_ != NULL) prev_gress_deps_->dump_delay_state_prev(flgs, true);
    if (this_too) dump_delay_state_this(flgs);
  }
  void MauDependencies01::dump_delay_state_next(uint64_t flgs, bool this_too) {
    if (this_too) dump_delay_state_this(flgs);
    if (next_gress_deps_ != NULL) next_gress_deps_->dump_delay_state_next(flgs, true);
  }
  void MauDependencies01::dump_delay_state(uint64_t flgs) {
    dump_delay_state_prev(flgs, false);
    dump_delay_state_next(flgs, true);
  }

  void MauDependencies01::find_series_features_this(uint32_t *features) {
    *features |= stage_features_;
  }
  void MauDependencies01::find_series_features_prev(uint32_t *features) {
    // Only go prev (upstream) if SELF not match_dep
    if ((cur_stage_dep_ != 0) && (prev_gress_deps_ != NULL))
      prev_gress_deps_->find_series_features_prev(features);
    find_series_features_this(features);
  }
  void MauDependencies01::find_series_features_next(uint32_t *features) {
    find_series_features_this(features);
    // Only go next (downstream) if NEXT not match_dep
    if ((next_stage_dep_ != 0) && (next_gress_deps_ != NULL))
      next_gress_deps_->find_series_features_next(features);
  }
  void MauDependencies01::find_series_features(uint32_t *features) {
    find_series_features_prev(features);
    find_series_features_next(features);
  }

  void MauDependencies01::distrib_series_features_this(uint32_t features) {
    bool change = (series_features_ != features);
    update_series_features_this(features);
    if ((change) && (write_callback_)) write_callback_(); // Will call dependencies_changed()
  }
  void MauDependencies01::distrib_series_features_prev(uint32_t features) {
    // Only go prev (upstream) if SELF not match_dep
    if ((cur_stage_dep_ != 0) && (prev_gress_deps_ != NULL))
      prev_gress_deps_->distrib_series_features_prev(features);
    distrib_series_features_this(features);
  }
  void MauDependencies01::distrib_series_features_next(uint32_t features) {
    distrib_series_features_this(features);
    // Only go next (downstream) if NEXT not match_dep
    if ((next_stage_dep_ != 0) && (next_gress_deps_ != NULL))
      next_gress_deps_->distrib_series_features_next(features);
  }
  void MauDependencies01::distrib_series_features(uint32_t features) {
    distrib_series_features_prev(features);
    distrib_series_features_next(features);
  }


  void MauDependencies01::update_series_features() {
    // Determine action/conc chain series features and update self/neighbours
    uint32_t features = 0u;
    find_series_features(&features);
    distrib_series_features(features);
  }
  void MauDependencies01::update_stage_features(uint32_t features) {
    // If nothing has changed we're done - bail out
    if (stage_features_ == features) return;
    // Else update
    stage_features_ = features;
    // ... and callback to trigger dependencies_changed
    if (write_callback_) write_callback_();
    // and re-evaluate series features (which could also callback)
    update_series_features();
  }


  bool MauDependencies01::report_bad_delay(const char *delaystr,
                                           uint8_t act, uint8_t exp, bool allow_greater) {
    if (act == exp) return true;
    uint64_t dump_flags = UINT64_C(0);
    if ((act > exp) && (allow_greater)) {
      dump_flags = RmtDebug::warn();
      RMT_LOG_OBJ(mau_, dump_flags,
                  "MAU<%d,%s> Excessive %s (Actual=%d ExpectedMin=%d Features=0x%x)\n",
                  mau_->mau_index(), iestr(), delaystr, act, exp, stage_features());
    } else {
      dump_flags = RmtDebug::error(MauDependencies::kRelaxDelayCheck);
      RMT_LOG_OBJ(mau_, dump_flags,
                  "MAU<%d,%s> Bad %s (Actual=%d Expected=%d Features=0x%x)\n",
                  mau_->mau_index(), iestr(), delaystr, act, exp, stage_features());
      if (!MauDependencies::kRelaxDelayCheck) { THROW_ERROR(-2); }
    }
    dump_delay_state(dump_flags);
    return false;
  }
  bool MauDependencies01::report_bad_delay_prev(const char *delaystr,
                                                uint8_t act, uint8_t exp) {
    if (act == exp) return true;
    bool error_relax = MauDependencies::kRelaxDelayCheck;
    bool warn = Mau::kRelaxPrevStageCheck;
    uint64_t dump_flags = (warn) ?RmtDebug::warn() :RmtDebug::error(error_relax);
    RMT_LOG_OBJ(mau_, dump_flags,
                "MAU<%d,%s> Bad %s (Actual=%d Expected=%d Features=0x%x)\n",
                mau_->mau_index(), iestr(), delaystr, act, exp, stage_features());
    if (!warn && !error_relax) { THROW_ERROR(-2); }
    dump_delay_state(dump_flags);
    return false;
  }
  bool MauDependencies01::check_pipelength_added_stages() {
    // Verify pipelength_added_stages
    // This can depend on the total added delay of earlier/later stages
    // (see uArch doc 3.1.2)
    uint8_t act = pipelength_added_stages_.pipelength_added_stages();
    uint8_t exp = MauDelay::total_added(series_features());
    if (act == exp) return true;
    // Final arg=true ==> allow act > exp
    return report_bad_delay("pipelength_added_stages", act, exp, true);
  }

  bool MauDependencies01::check_start_table_fifo0() {
    bool jbay = RmtObject::is_jbay_or_later();
    bool act_st_fifo0 = start_table_fifo0_enabled_;
    bool exp_st_fifo0 = !act_st_fifo0;

    if (cur_stage_dep_ == 0) { // MatchDep
      exp_st_fifo0 = true;
    } else if (cur_stage_dep_ == 1) { // ActionDep
      exp_st_fifo0 = (jbay) ?false :true;
    } else if (cur_stage_dep_ == 2) { // Concurrent
      exp_st_fifo0 = false;
    }
    if (act_st_fifo0 != exp_st_fifo0) {
      RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                  "MAU<%d,%s> Bad value start_table_fifo0_enabled_"
                  "(Actual=%d Expected=%d Chip=%s CurDep=%d Features=0x%x)\n",
                  mau_->mau_index(), iestr(), act_st_fifo0, exp_st_fifo0,
                  jbay?"JBay":"Tofino", cur_stage_dep_, stage_features());
    }
    if (cur_stage_dep_ == 2) return true;

    uint32_t stage_ftrs = stage_features() | depend_features(); // Include dep features
    uint8_t  act = predication_ctl_.start_table_fifo_delay0();
    uint8_t  exp = MauDelay::stage_start_table_fifo0(series_features(), stage_ftrs,
                                                     get_prev_post_pred_delay());
    if (act_st_fifo0 && (act == exp)) return true;
    return report_bad_delay_prev("start_table_fifo_delay0", act, exp);
  }
  bool MauDependencies01::check_start_table_fifo1() {
    bool    jbay = RmtObject::is_jbay_or_later();
    bool    st_fifo1 = start_table_fifo1_enabled_;
    uint8_t act = predication_ctl_.start_table_fifo_delay1();
    uint8_t exp = get_prev_post_pred_delay();
    if (jbay) {
      if (st_fifo1 || (act > 0)) {
        RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                    "MAU<%d,%s> Usage of start_table_fifo1_enabled_ "
                    "is deprecated on JBay - use mpr_ CSRs instead\n",
                    mau_->mau_index(), iestr());
      }
      return true;
    }
    if (cur_stage_dep_ != 0) return true; // MatchDep
    if (st_fifo1 && (act == exp)) return true;
    return report_bad_delay_prev("start_table_fifo_delay1", act, exp);
  }
  bool MauDependencies01::check_action_output_delay() {
    bool    action_output = action_output_enabled_;
    uint8_t exp = MauDelay::stage_action_output(series_features());
    uint8_t act = action_output_delay_.action_output_delay();
    if (cur_stage_dep_ == 1) return true; // ActionDep
    if (action_output && (act == exp)) return true;
    return report_bad_delay("action_output_delay", act, exp);
  }
  bool MauDependencies01::check_deferred_eop_bus_delay() {
    bool     deferred_eop_fifo_en = (deferred_eop_bus_delay_.eop_delay_fifo_en() == 1);
    if (!deferred_eop_fifo_en) return true;
    uint32_t stage_ftrs = stage_features() | depend_features(); // Include dep features
    uint8_t  actI = deferred_eop_bus_delay_.eop_internal_delay_fifo();
    uint8_t  expI = MauDelay::stage_deferred_eop_bus_internal(series_features(), stage_ftrs);
    if (actI != expI) report_bad_delay("deferred_eop_bus_delay_internal", actI, expI);

    uint8_t  actO = deferred_eop_bus_delay_.eop_output_delay_fifo();
    uint8_t  expO = MauDelay::stage_deferred_eop_bus_output(series_features(), stage_ftrs);
    if (actO != expO) report_bad_delay("deferred_eop_bus_delay_output", actO, expO);

    return ((actI == expI) && (actO == expO));
  }

  uint8_t MauDependencies01::get_delay(int which_delay) {
    int delay = MauDelay::get_delay(which_delay, series_features(), stage_features());
    RMT_ASSERT( (delay >= 0) && (delay <= 63) &&  "Bad MauDelay::get_delay");
    return delay;
  }
  uint8_t MauDependencies01::get_prev_post_pred_delay() {
    if ((cur_stage_dep_ != 0) || (prev_gress_deps_ == NULL)) return 0;
    return prev_gress_deps_->get_delay(MauDelay::kPostPredication);
  }






  MauDependencies::MauDependencies(int chipIndex, int pipeIndex, int mauIndex, Mau *mau)
      : mau_(mau), ctor_running_(true),
        stage_concurrent_with_prev_(default_adapter(stage_concurrent_with_prev_,chipIndex,pipeIndex,mauIndex, //0,
                                    [this](){this->stage_conc_write_callback(); })),
        match_ie_input_mux_sel_(default_adapter(match_ie_input_mux_sel_,chipIndex,pipeIndex,mauIndex,  //0,
                                [this](){this->input_mux_sel_write_callback(); })),
        phv_fifo_enable_(default_adapter(phv_fifo_enable_,chipIndex,pipeIndex,mauIndex,  //0,
                         [this](){this->fifo_enable_write_callback(); })),
        logical_table_thread_(default_adapter(logical_table_thread_,chipIndex, pipeIndex, mauIndex,
                              [this](uint32_t i){this->lt_thread_change_callback(i); })),
        adr_dist_table_thread_(default_adapter(adr_dist_table_thread_,chipIndex, pipeIndex, mauIndex,
                               [this](uint32_t i, uint32_t j ){this->ad_thread_change_callback(i,j); })),
        exact_match_delay_thread_(default_adapter(exact_match_delay_thread_,chipIndex, pipeIndex, mauIndex,
                              [this](uint32_t i){this->xm_delay_change_callback(i); })),
        exact_match_logical_result_delay_(default_adapter(exact_match_logical_result_delay_,chipIndex, pipeIndex, mauIndex,
                                          [this](){this->xm_bus_timing_change_callback(0); })),
        exact_match_logical_result_en_(default_adapter(exact_match_logical_result_en_,chipIndex, pipeIndex, mauIndex,
                                       [this](){this->xm_bus_timing_change_callback(0); })),
        exact_match_phys_result_delay_(default_adapter(exact_match_phys_result_delay_,chipIndex, pipeIndex, mauIndex,
                                       [this](uint32_t i){this->xm_bus_timing_change_callback(i); })),
        exact_match_phys_result_en_(default_adapter(exact_match_phys_result_en_,chipIndex, pipeIndex, mauIndex,
                                    [this](uint32_t i){this->xm_bus_timing_change_callback(i); })),
        exact_match_phys_result_thread_(default_adapter(exact_match_phys_result_thread_,chipIndex, pipeIndex, mauIndex,
                                        [this](uint32_t i){this->xm_bus_timing_change_callback(i); })),
        adr_dist_pipe_delay_(default_adapter(adr_dist_pipe_delay_,chipIndex, pipeIndex, mauIndex,
                             [this](uint32_t i, uint32_t j ){this->ad_pipedelay_change_callback(i,j); })),
        tind_bus_prop_(default_adapter(tind_bus_prop_,chipIndex, pipeIndex, mauIndex,
                       [this](uint32_t i){this->tind_bus_prop_change_callback(i); })),
        hash_group_config_(default_adapter(hash_group_config_,chipIndex, pipeIndex, mauIndex,
                       [this](){this->hash_group_config_change_callback(); })),
        meter_alu_group_action_ctl_(default_adapter(meter_alu_group_action_ctl_,chipIndex, pipeIndex, mauIndex,
                                    [this](uint32_t i){this->meter_alu_group_ctl_change_callback(i); })),
        meter_alu_group_data_delay_ctl_(default_adapter(meter_alu_group_data_delay_ctl_,chipIndex, pipeIndex, mauIndex,
                                        [this](uint32_t i){this->meter_alu_group_ctl_change_callback(i); })),
        chip_deps_(chipIndex, pipeIndex, mauIndex, mau),
        prev_deps_(NULL), next_deps_(NULL),
        ingress_(chipIndex, pipeIndex, mauIndex, 0, 0x1, mau,
                 [this](){this->ingress_reg_write_callback(); }),
        egress_(chipIndex, pipeIndex, mauIndex, 1, 0x2, mau,
                [this](){this->egress_reg_write_callback(); })
  {
    stage_concurrent_with_prev_.reset();
    match_ie_input_mux_sel_.reset();
    phv_fifo_enable_.reset();
    logical_table_thread_.reset();
    adr_dist_table_thread_.reset();
    exact_match_delay_thread_.reset();
    exact_match_logical_result_delay_.reset();
    exact_match_logical_result_en_.reset();
    exact_match_phys_result_delay_.reset();
    exact_match_phys_result_en_.reset();
    exact_match_phys_result_thread_.reset();
    adr_dist_pipe_delay_.reset();
    tind_bus_prop_.reset();
    hash_group_config_.reset();
    meter_alu_group_action_ctl_.reset();
    meter_alu_group_data_delay_ctl_.reset();
    ctor_running_ = false;
  }
  MauDependencies::~MauDependencies() { }



  void MauDependencies::link_deps() {
    Mau *prev_mau = mau_->mau_previous();
    set_prev_deps((prev_mau != NULL) ?prev_mau->mau_dependencies() :NULL);
  }
  void MauDependencies::set_prev_deps(MauDependencies *prev_deps) {
    if (prev_deps_ == prev_deps) return; // Bail if prev_deps already installed
    MauDependencies *prev_deps_0 = prev_deps_;
    prev_deps_ = prev_deps;
    if (prev_deps_0 != NULL) prev_deps_0->set_next_deps(NULL); // Unlink old prev from us
    ingress_.prev_gress_deps_ = (prev_deps != NULL) ?prev_deps->gress(ingress_.which_) :NULL;
    egress_.prev_gress_deps_ = (prev_deps != NULL) ?prev_deps->gress(egress_.which_) :NULL;
    if (prev_deps != NULL) prev_deps->set_next_deps(this);
  }
  void MauDependencies::set_next_deps(MauDependencies *next_deps) {
    if (next_deps_ == next_deps) return; // Bail if next_deps already installed
    MauDependencies *next_deps_0 = next_deps_;
    next_deps_ = next_deps;
    if (next_deps_0 != NULL) next_deps_0->set_prev_deps(NULL); // Unlink old next from us
    ingress_.next_gress_deps_ = (next_deps != NULL) ?next_deps->gress(ingress_.which_) :NULL;
    egress_.next_gress_deps_ = (next_deps != NULL) ?next_deps->gress(egress_.which_) :NULL;
    if (next_deps != NULL) next_deps->set_prev_deps(this);
  }


  void MauDependencies::stage_conc_write_callback() {
    if (ctor_running_) return;
    dependencies_changed();
  }
  void MauDependencies::input_mux_sel_write_callback() {
    if (ctor_running_) return;
    dependencies_changed();
  }
  void MauDependencies::fifo_enable_write_callback() {
    if (ctor_running_) return;
    egress_.action_output_enabled_ =
        ((phv_fifo_enable_.phv_fifo_egress_action_output_enable() & 0x1) != 0);
    ingress_.action_output_enabled_ =
        ((phv_fifo_enable_.phv_fifo_ingress_action_output_enable() & 0x1) != 0);
    egress_.final_output_enabled_ =
        ((phv_fifo_enable_.phv_fifo_egress_final_output_enable() & 0x1) != 0);
    ingress_.final_output_enabled_ =
        ((phv_fifo_enable_.phv_fifo_ingress_final_output_enable() & 0x1) != 0);
    dependencies_changed();
  }
  void MauDependencies::lt_thread_change_callback(uint32_t repl) {
    RMT_ASSERT((repl == 0) || (repl == 1) || (repl == 2));
    if (ctor_running_) return;
    ingress_.verify_table_thread_ = true;
    egress_.verify_table_thread_ = true;
    dependencies_changed();
  }
  void MauDependencies::ad_thread_change_callback(uint32_t ie, uint32_t repl) {
    RMT_ASSERT((ie == 0) || (ie == 1));
    RMT_ASSERT((repl == 0) || (repl == 1));
    if (ctor_running_) return;
    if (ie == 0) ingress_.verify_table_thread_ = true;
    if (ie == 1) egress_.verify_table_thread_ = true;
    dependencies_changed();
  }
  void MauDependencies::xm_delay_change_callback(uint32_t repl) {
    RMT_ASSERT((repl == 0) || (repl == 1) || (repl == 2));
    if (ctor_running_) return;
    ingress_.verify_table_thread_ = true;
    egress_.verify_table_thread_ = true;
    dependencies_changed();
  }
  void MauDependencies::xm_bus_timing_change_callback(uint32_t i) {
    if (ctor_running_) return;
    dependencies_changed();
  }
  void MauDependencies::ad_pipedelay_change_callback(uint32_t ie, uint32_t repl) {
    RMT_ASSERT((ie == 0) || (ie == 1));
    RMT_ASSERT((repl == 0) || (repl == 1));
    if (ctor_running_) return;
    if (ie == 0) ingress_.verify_table_thread_ = true;
    if (ie == 1) egress_.verify_table_thread_ = true;
    dependencies_changed();
  }
  void MauDependencies::tind_bus_prop_change_callback(uint32_t x) {
    RMT_ASSERT(x < MauDefs::kTindOutputBusesPerMau);
    if (ctor_running_) return;
    uint32_t i_ftrs = 0u, e_ftrs = 0u;
    for (int i = 0; i < MauDefs::kTindOutputBusesPerMau; i++) {
      uint8_t ie = tind_bus_prop_.thread(i);
      uint8_t en = tind_bus_prop_.enabled(i);
      if ((en == 1) && (ie == 0)) i_ftrs |= MauDefs::kMauTindPresent;
      if ((en == 1) && (ie == 1)) e_ftrs |= MauDefs::kMauTindPresent;
    }
    mau_->set_tind_dynamic_features(i_ftrs, e_ftrs);
  }
  void MauDependencies::hash_group_config_change_callback() {
    if (ctor_running_) return;
    uint32_t i_ftrs = 0u, e_ftrs = 0u;
    for (int i = 0; i < 6; i++) {
      uint8_t ie = (hash_group_config_.hash_group_egress() >> i) & 0x1;
      uint8_t en = (hash_group_config_.hash_group_enable() >> i) & 0x1;
      uint16_t ctl = (hash_group_config_.hash_group_ctl() >> (i+i)) & 0x3;
      // ctl==0 => selector_hash_mod ==> WideSel (see MikeF email 23/03/2016)
      if ((en == 1) && (ctl == 0) && (ie == 0)) i_ftrs |= MauDefs::kMauWideSelectorPresent;
      if ((en == 1) && (ctl == 0) && (ie == 1)) e_ftrs |= MauDefs::kMauWideSelectorPresent;
    }
    mau_->set_hash_dynamic_features(i_ftrs, e_ftrs);
  }
  void MauDependencies::meter_alu_group_ctl_change_callback(uint32_t alu) {
    if (ctor_running_) return;
    dependencies_changed();
  }
  void MauDependencies::ingress_reg_write_callback() {
    if (ctor_running_) return;
    ingress_.update_info_on_write();
    dependencies_changed();
  }
  void MauDependencies::egress_reg_write_callback() {
    if (ctor_running_) return;
    egress_.update_info_on_write();
    dependencies_changed();
  }
  void MauDependencies::dependencies_changed() {
   mau_->dependencies_changed();
  }


  uint16_t MauDependencies::check_lt_thread_replication(int ie) {
    RMT_ASSERT((ie == 0) || (ie == 1));
    uint16_t ing0 = logical_table_thread_.logical_table_thread_ingress(0);
    uint16_t ing1 = logical_table_thread_.logical_table_thread_ingress(1);
    uint16_t ing2 = logical_table_thread_.logical_table_thread_ingress(2);
    uint16_t egr0 = logical_table_thread_.logical_table_thread_egress(0);
    uint16_t egr1 = logical_table_thread_.logical_table_thread_egress(1);
    uint16_t egr2 = logical_table_thread_.logical_table_thread_egress(2);
    if ((ing0 == ing1) && (ing0 == ing2) && (egr0 == egr1) && (egr0 == egr2)) {
      return (ie == 0) ?ing0 :egr0; // All match return ingress/egress
    } else {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxReplicationCheck),
                  "MAU<%d> logical_table_thread reg NOT replicated correctly "
                  "Ingress=0x%04x,0x%04x,0x%04x Egress=0x%04x,0x%04x,0x%04x\n",
                  mau_->mau_index(), ing0, ing1, ing2, egr0, egr1, egr2);
      if (!kRelaxReplicationCheck) { THROW_ERROR(-2); }
      return 0;
    }
  }
  uint16_t MauDependencies::check_ad_thread_replication(int ie) {
    uint16_t ing0 = adr_dist_table_thread_.adr_dist_table_thread(0,0);
    uint16_t ing1 = adr_dist_table_thread_.adr_dist_table_thread(0,1);
    uint16_t egr0 = adr_dist_table_thread_.adr_dist_table_thread(1,0);
    uint16_t egr1 = adr_dist_table_thread_.adr_dist_table_thread(1,1);
    if ((ing0 == ing1) && (egr0 == egr1)) {
      return (ie == 0) ?ing0 :egr0; // All match return ingress/egress
    } else {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxReplicationCheck),
                  "MAU<%d> adr_dist_table_thread reg NOT replicated correctly "
                  "Ingress=0x%04x,0x%04x Egress=0x%04x,0x%04x\n",
                  mau_->mau_index(), ing0, ing1, egr0, egr1);
      if (!kRelaxReplicationCheck) { THROW_ERROR(-2); }
      return 0;
    }
  }
  void MauDependencies::check_xm_delay_replication(int ie) {
    RMT_ASSERT((ie == 0) || (ie == 1));
    uint8_t ing0 = (exact_match_delay_thread_.exact_match_delay_thread(0) >> 0) & 0x1;
    uint8_t ing1 = (exact_match_delay_thread_.exact_match_delay_thread(1) >> 0) & 0x1;
    uint8_t ing2 = (exact_match_delay_thread_.exact_match_delay_thread(2) >> 0) & 0x1;
    uint8_t egr0 = (exact_match_delay_thread_.exact_match_delay_thread(0) >> 1) & 0x1;
    uint8_t egr1 = (exact_match_delay_thread_.exact_match_delay_thread(1) >> 1) & 0x1;
    uint8_t egr2 = (exact_match_delay_thread_.exact_match_delay_thread(2) >> 1) & 0x1;
    if ((ing0 == ing1) && (ing0 == ing2) && (egr0 == egr1) && (egr0 == egr2)) {
      // All match
    } else {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxReplicationCheck),
                  "MAU<%d> xm_delay_thread reg NOT replicated correctly "
                  "Ingress=0x%01x,0x%01x,0x%01x Egress=0x%01x,0x%01x,0x%01x\n",
                  mau_->mau_index(), ing0, ing1, ing2, egr0, egr1, egr2);
      if (!kRelaxReplicationCheck) { THROW_ERROR(-2); }
    }
  }
  void MauDependencies::check_ad_pipedelay_replication(int ie) {
    uint8_t ing0 = adr_dist_pipe_delay_.adr_dist_pipe_delay(0,0);
    uint8_t ing1 = adr_dist_pipe_delay_.adr_dist_pipe_delay(0,1);
    uint8_t egr0 = adr_dist_pipe_delay_.adr_dist_pipe_delay(1,0);
    uint8_t egr1 = adr_dist_pipe_delay_.adr_dist_pipe_delay(1,1);
    if ((ing0 == ing1) && (egr0 == egr1)) {
      // All match
    } else {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxReplicationCheck),
                  "MAU<%d> adr_dist_pipe_delay reg NOT replicated correctly "
                  "Ingress=0x%01x,0x%01x Egress=0x%01x,0x%01x\n",
                  mau_->mau_index(), ing0, ing1, egr0, egr1);
      if (!kRelaxReplicationCheck) { THROW_ERROR(-2); }
    }
  }


  void MauDependencies::check_thread_config(int ie) {
    // On JBay, predication_ctl ingress_thread tables do NOT contain
    // ghost_thread tables so splice them in now if handling ingress.
    uint16_t ttp = (ie == 0) ?ingress_.table_thread_ :egress_.table_thread_;
    uint16_t ttg = (ie == 0) ?get_ghost_table_thread() :0; // Always 0 on Tofino
    uint16_t tt0 = ttp | ttg;
    uint16_t tt1 = check_lt_thread_replication(ie);
    uint16_t tt2 = check_ad_thread_replication(ie);
    check_xm_delay_replication(ie);
    check_ad_pipedelay_replication(ie);
    if (((ttp & ttg) != 0) || GLOBAL_FALSE) {  // XXX: use GLOBAL_FALSE to keep Klocwork happy
      // If we DO have ghost tables they should NOT be cited in predication_ctl
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxThreadCheck),
                  "MAU<%d,%s> Ghost tables should NOT be set in predication_ctl "
                  "table thread  (PredCtl[0]=0x%04x GhostTables=0x%04x)\n",
                  mau_->mau_index(), (ie==0) ?"INGRESS" :"EGRESS", ttp, ttg);
      if (!kRelaxThreadCheck) { THROW_ERROR(-2); }
    }
    if ((tt0 == tt1) && (tt0 == tt2)) {
      // All match - tt0 must be same as tt1/tt2
    } else {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxThreadCheck),
                  "MAU<%d,%s> TableThread regs NOT programmed consistently "
                  "(PredCtl=0x%04x LogicalTableThread=0x%04x AdrDistTableThread=0x%04x)\n",
                  mau_->mau_index(), (ie==0) ?"INGRESS" :"EGRESS", tt0, tt1, tt2);
      if (!kRelaxThreadCheck) { THROW_ERROR(-2); }
    }
  }
  void MauDependencies::check_thread_config() {
    if (ingress_.verify_table_thread_) check_thread_config(0);
    if (egress_.verify_table_thread_)  check_thread_config(1);
    ingress_.verify_table_thread_ = false;
    egress_.verify_table_thread_ = false;
  }
  void MauDependencies::check_config() {
    check_thread_config();
    check_dependencies(ingress_);
    check_dependencies(egress_);
    check_delays(ingress_);
    check_delays(egress_);
  }


  uint16_t MauDependencies::logical_tables(MauDependencies01& iegress) {
    // On JBay, predication_ctl ingress_thread tables do NOT contain
    // ghost_thread tables so splice them in now if handling ingress.
    // NB 1. get_ghost_table_thread() always 0 on Tofino
    // NB 2. MauPredication logic splits these apart again on JBay
    uint16_t ttp = iegress.table_thread_;
    uint16_t ttg = (iegress.ingress()) ?get_ghost_table_thread() :0;
    return ttp | ttg;
  }
  uint32_t MauDependencies::dynamic_features(MauDependencies01& iegress) {
    return iegress.stage_features();
  }
  void MauDependencies::set_dynamic_features(MauDependencies01& iegress, uint32_t features) {
    iegress.update_stage_features(features);
  }


  bool MauDependencies::report_bad_delay(MauDependencies01& iegress,
                                         const char *delaystr,
                                         uint8_t act, uint8_t exp,
                                         bool allow_greater) {
    return iegress.report_bad_delay(delaystr, act, exp, allow_greater);
  }
  bool MauDependencies::check_xm_bus_delay(MauDependencies01& iegress) {
    // Exact Match Delay Thread should be 1 (or 0) if series_tcam_delay >0 (or 0)
    bool ok = true;
    bool tcam_timing = MauDefs::tcam_present(iegress.series_features());
    uint8_t reg = exact_match_delay_thread_.exact_match_delay_thread(0);
    uint8_t act = reg & iegress.ieflag_;
    uint8_t exp = (tcam_timing) ?iegress.ieflag_ :0;
    if (act != exp) {
      ok = false;
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDelayCheck),
                  "MAU<%d,%s> xm_match_delay_thread (0x%x) programming "
                  "incorrect given tcam_timing=%d\n",
                  mau_->mau_index(), iegress.iestr(), reg, tcam_timing);
      if (!kRelaxDelayCheck) { THROW_ERROR(-2); }
    }
    // Now for each LT in gress (irrespective of whether enabled for XM result)
    // 1. XM result should be delayed (or not) if series_tcam_delay >0 (or 0)
    // NB. On JBay, predication_ctl ingress_thread tables do NOT contain
    //     ghost_thread tables so splice them in now if handling ingress
    uint16_t tt_pred  = iegress.table_thread_;
    uint16_t tt_ghost = (iegress.ingress()) ?get_ghost_table_thread() :0; // Always 0 on Tofino
    uint16_t tt_gress = tt_pred | tt_ghost;
    uint16_t tt_xm = exact_match_logical_result_en_.exact_match_logical_result_en();
    uint16_t tt_delayed = exact_match_logical_result_delay_.exact_match_logical_result_delay();
    uint16_t tt_gress_xm = tt_gress & tt_xm;
    uint16_t tt_error = 0;

    // The logic below used to only check *XM enabled* LTs in this gress.
    // However it turns out the MAU uses exact_match_logical_result_delay
    // to figure out whether to delay the output of Gateway LUTs too.
    //
    // So changed logic to check ALL LTs in gress (tt_gress) rather than
    // only XM-enabled LTs in gress (tt_gress_xm)
    //
    for (int lt = 0; lt < kLogicalTables; lt++) {
      uint16_t tt_this = 1<<lt;
      if ((tt_gress & tt_this) != 0) { // Check ALL LTs in this gress (see above)
        bool exp_delayed = tcam_timing;
        bool act_delayed = ((tt_delayed & tt_this) != 0);
        if (act_delayed != exp_delayed) tt_error |= tt_this;
      }
    }
    if (tt_error != 0) {
      ok = false;
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDelayCheck),
                  "MAU<%d,%s> xm_match_logical_result_delay/en (0x%04x/0x%04x) programming "
                  "incorrect for LTs=0x%04x given tcam_timing=%d (table_thread=0x%04x)\n",
                  mau_->mau_index(), iegress.iestr(), tt_delayed, tt_xm,
                  tt_error, tcam_timing, tt_gress);
    }

    // 2. And all enabled buses associated with LT should be delayed (or not)
    //
    // First of all figure our which XM buses are enabled and which are delayed
    // and which are in our gress
    uint8_t xm_bot_en = exact_match_phys_result_en_.exact_match_phys_result_en(0);
    uint8_t xm_top_en = exact_match_phys_result_en_.exact_match_phys_result_en(1);
    uint16_t xm_buses_en = (static_cast<uint16_t>(xm_top_en) << 8) |
        (static_cast<uint16_t>(xm_bot_en) << 0);
    uint8_t xm_bot_delayed = exact_match_phys_result_delay_.exact_match_phys_result_delay(0);
    uint8_t xm_top_delayed = exact_match_phys_result_delay_.exact_match_phys_result_delay(1);
    uint16_t xm_buses_delayed = (static_cast<uint16_t>(xm_top_delayed) << 8) |
        (static_cast<uint16_t>(xm_bot_delayed) << 0);
    uint8_t xm_bot_thread = exact_match_phys_result_thread_.exact_match_phys_result_thread(0);
    uint8_t xm_top_thread = exact_match_phys_result_thread_.exact_match_phys_result_thread(1);
    uint16_t xm_buses_thread = (static_cast<uint16_t>(xm_top_thread) << 8) |
        (static_cast<uint16_t>(xm_bot_thread) << 0);
    uint16_t xm_buses_egress = xm_buses_thread & xm_buses_en;
    uint16_t xm_buses_ingress = ~xm_buses_thread & xm_buses_en;
    uint16_t xm_buses_gress = iegress.ingress() ?xm_buses_ingress :xm_buses_egress;

    // Then for each LT find what buses it uses and see if they're all delayed/undelayed
    // and check that that matches up given tcam_timing (or not tcam_timing)
    for (int lt = 0; lt < kLogicalTables; lt++) {
      uint16_t tt_this = 1<<lt;
      if ((tt_gress_xm & tt_this) != 0) { // Only XM enabled LTs in this gress

        uint16_t xm_buses_lt_uses = mau_->mau_result_bus()->get_match_buses(lt);
        if (((xm_buses_lt_uses & xm_buses_ingress) != 0) &&
            ((xm_buses_lt_uses & xm_buses_egress) != 0)) {
          ok = false;
          RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDelayCheck),
                      "MAU<%d,%s> LT=%d uses buses (0x%04x) in both ingress and egress\n",
                      mau_->mau_index(), iegress.iestr(), lt, xm_buses_lt_uses);
        }
        uint16_t xm_buses_lt_delayed = xm_buses_lt_uses & xm_buses_delayed & xm_buses_gress;
        uint16_t xm_buses_lt_undelayed = xm_buses_lt_uses & ~xm_buses_delayed & xm_buses_gress;
        if (( tcam_timing && (xm_buses_lt_undelayed != 0)) ||
            (!tcam_timing && (xm_buses_lt_delayed != 0))) {
          ok = false;
          RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDelayCheck),
                      "MAU<%d,%s> xm_match_phys_result_delay/en/thread (0x%04x/0x%04x/0x%04x) "
                      "programming incorrect for LT=%d (uses xm buses 0x%04x["
                      "delayed=0x%04x,undelayed=0x%04x]) given tcam_timing=%d\n",
                      mau_->mau_index(), iegress.iestr(),
                      xm_buses_delayed, xm_buses_en, xm_buses_thread,
                      lt, xm_buses_lt_uses, xm_buses_lt_delayed, xm_buses_lt_undelayed,
                      tcam_timing);
        }
      }
    }
    return ok;
  }
  bool MauDependencies::check_adr_dist_pipe_delay(MauDependencies01& iegress) {
    uint8_t act = adr_dist_pipe_delay_.adr_dist_pipe_delay(iegress.which_,0);
    uint8_t exp = MauDelay::alu(iegress.series_features());
    if (act == exp) return true;
    return report_bad_delay(iegress, "adr_dist_pipe_delay", act, exp);
  }
  bool MauDependencies::check_meter_alu_group_action_ctl(MauDependencies01& iegress) {
    // XXX: fix x_alu_action_delay checks to take account of *series* selector_delay_
    uint32_t series_ftrs = iegress.series_features();
    bool ok = true;
    uint8_t act, exp;
    for (int alu = 0; alu < kNumMeterAlus; alu++) {
      int ie = (mau_->is_meter_alu_egress(alu)) ?1 :0;
      if (ie == iegress.which_) {
        uint32_t m_alu_ftrs = mau_->meter_alu_dynamic_features(alu);

        if (meter_alu_group_action_ctl_.right_alu_action_enable(alu) != 0) {
          exp = MauDelay::meter_alu_group_action_right(alu, series_ftrs, m_alu_ftrs);
          act = meter_alu_group_action_ctl_.right_alu_action_delay(alu);
          if (act != exp) {
            if (!report_bad_delay(iegress, "right_alu_action_delay", act, exp))
              ok = false;
          }
        }
        if (meter_alu_group_action_ctl_.left_alu_action_enable(alu) != 0) {
          exp = MauDelay::meter_alu_group_action_left(alu, series_ftrs, m_alu_ftrs);
          act = meter_alu_group_action_ctl_.left_alu_action_delay(alu);
          if (act != exp) {
            if (!report_bad_delay(iegress, "left_alu_action_delay", act, exp))
              ok = false;
          }
        }
      }
    }
    return ok;
  }
  bool MauDependencies::check_meter_alu_group_data_delay_ctl(MauDependencies01& iegress) {
    bool ok = true;
    uint8_t act, exp;
    for (int alu = 0; alu < kNumMeterAlus; alu++) {
      int ie = (mau_->is_meter_alu_egress(alu)) ?1 :0;
      if (ie == iegress.which_) {
        uint32_t m_alu_ftrs = mau_->meter_alu_dynamic_features(alu);
        exp = MauDelay::meter_alu_group_data(alu, iegress.series_features(), m_alu_ftrs);

        if (meter_alu_group_data_delay_ctl_.meter_alu_right_group_enable(alu) != 0) {
          act = meter_alu_group_data_delay_ctl_.meter_alu_right_group_delay(alu);
          if (act != exp) {
            if (!report_bad_delay(iegress, "meter_alu_right_group_delay", act, exp))
              ok = false;
          }
        }
        if (meter_alu_group_data_delay_ctl_.meter_alu_left_group_enable(alu) != 0) {
          act = meter_alu_group_data_delay_ctl_.meter_alu_left_group_delay(alu);
          if (act != exp) {
            if (!report_bad_delay(iegress, "meter_alu_left_group_delay", act, exp))
              ok = false;
          }
        }
      }
    }
    return ok;
  }

  bool MauDependencies::check_delays(MauDependencies01& iegress) {

    // Verify pipelength_added_stages
    // This can depend on the total added delay of earlier/later stages
    // (see uArch doc 3.1.2)
    bool b1 = iegress.check_pipelength_added_stages();

    // Verify start table fifo delays - first fifo0
    bool b2 = iegress.check_start_table_fifo0();

    // Verify start table fifo delays - now fifo1
    bool b3 = iegress.check_start_table_fifo1();

    // Verify action output delay
    bool b4 = iegress.check_action_output_delay();

    // Verify deferred_eop_bus_delay
    bool b5 = iegress.check_deferred_eop_bus_delay();

    // Verify XM bus delay
    // This can depend on the tcam_delay of earlier/later stages
    bool b6 = check_xm_bus_delay(iegress);

    // Verify adr_dist_pipe_delay
    bool b7 = check_adr_dist_pipe_delay(iegress);

    // Verify meter_alu_group_action_ctl for ALUs in our gress
    bool b8 = check_meter_alu_group_action_ctl(iegress);

    // Verify meter_alu_group_data_delay_ctl for ALUs in our gress
    // This can depend on the tcam_delay of earlier/later stages
    bool b9 = check_meter_alu_group_data_delay_ctl(iegress);

    return b1 && b2 && b3 && b4 && b5 && b6 && b7 && b8 && b9;
  }

  bool MauDependencies::prev_conc(MauDependencies01& iegress) {
    int     stage = mau_->mau_index();
    // If MAU0 return false - MAU0 always match dependent - but go via MAU feature func to make this configurable
    if (Mau::must_be_match_dependent(stage)) return false;
    bool    conc_dep = (iegress.cur_stage_dep_ == 2);
    bool    concurrent = ((stage_concurrent_with_prev_.stage_concurrent_with_prev() & iegress.ieflag_) != 0);
    bool    iphv = ((match_ie_input_mux_sel_.match_ie_input_mux_sel() & iegress.ieflag_) == 0); // Match takes iPhv
    if (Mau::must_use_iphv(stage) && !iphv) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDependencyCheck), "MAU<%d> *MUST* use iphv\n", stage);
      if (!kRelaxDependencyCheck) { THROW_ERROR(-2); }
    } else if (Mau::must_use_ophv(stage) && iphv) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDependencyCheck), "MAU<%d> *MUST* use ophv\n", stage);
      if (!kRelaxDependencyCheck) { THROW_ERROR(-2); }
    }

    bool    action_output = iegress.action_output_enabled_;
    uint8_t ao_delay = iegress.action_output_delay_.action_output_delay();
    bool    ok_ao_delay = ((ao_delay >= 17) && (ao_delay <= 30)); // Uarch doc section 3.1
    if (action_output && !ok_ao_delay) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDelayCheck),
                  "MAU<%d,%s> Bad action output delay %d (Expected=%d)\n",
                  stage, iegress.iestr(), ao_delay,
                  MauDelay::stage_action_output(iegress.series_features()));
      if (kRelaxDelayCheck) ok_ao_delay = true; else { THROW_ERROR(-2); }
    }
    bool    no_st_fifo0 = !iegress.start_table_fifo0_enabled_;
    if (conc_dep && concurrent && iphv && action_output && ok_ao_delay && no_st_fifo0) return true;
    if (conc_dep) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDependencyCheck), "Inconsistent %s prev-stage CONCURRENT settings\n", iegress.iestr());
      RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "OBSERVED: StageDep=%d Concurrent=%d iphv=%d act_out=%d ok_ao_delay=%d no_st_fifo0=%d\n",
                  iegress.cur_stage_dep_, concurrent, iphv, action_output, ok_ao_delay, no_st_fifo0);
      RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "EXPECTED: StageDep=2(CONC) Concurrent=1 iphv=1 act_out=1 ok_ao_delay=1 no_st_fifo0=1\n");
      if (!kRelaxDependencyCheck) { THROW_ERROR(-2); }
    }
    return conc_dep;
  }
  bool MauDependencies::prev_action_dep(MauDependencies01& iegress) {
    bool    jbay = RmtObject::is_jbay_or_later();
    int     stage = mau_->mau_index();
    // If MAU0 return false - MAU0 always match dependent - but go via MAU feature func to make this configurable
    if (Mau::must_be_match_dependent(stage)) return false;
    bool    act_dep = (iegress.cur_stage_dep_ == 1);
    bool    iphv = ((match_ie_input_mux_sel_.match_ie_input_mux_sel() & iegress.ieflag_) == 0); // Match takes iPhv
    if (Mau::must_use_iphv(stage) && !iphv) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDependencyCheck), "MAU<%d> *MUST* use iphv\n", stage);
      if (!kRelaxDependencyCheck) { THROW_ERROR(-2); }
    } else if (Mau::must_use_ophv(stage) && iphv) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDependencyCheck), "MAU<%d> *MUST* use ophv\n", stage);
      if (!kRelaxDependencyCheck) { THROW_ERROR(-2); }
    }

    bool    concurrent = ((stage_concurrent_with_prev_.stage_concurrent_with_prev() & iegress.ieflag_) != 0);
    bool    action_output = iegress.action_output_enabled_;
    bool    st_fifo0 = iegress.start_table_fifo0_enabled_;
    uint8_t st0_delay = iegress.predication_ctl_.start_table_fifo_delay0();
    bool    ok_st0_delay = (st0_delay == MauDefs::kMauActionDepStartTableFifo0Delay);
    bool    st_fifo0_ok = true;
    if ((act_dep && jbay && st_fifo0) || (act_dep && !jbay && !st_fifo0)) {
      st_fifo0_ok = false;
      RMT_LOG_OBJ(mau_, RmtDebug::warn(),
                  "MAU<%d,%s> Bad value start_table_fifo0_enabled_"
                  "(Actual=%d Expected=%d Chip=%s CurDep=1(action_dep))\n",
                  mau_->mau_index(), iegress.iestr(), st_fifo0, !st_fifo0, jbay?"JBay":"Tofino");
    }
    if (act_dep && st_fifo0 && !ok_st0_delay) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDelayCheck),
                  "MAU<%d,%s> Bad start table fifo0 delay %d\n", stage, iegress.iestr(), st0_delay);
      if (kRelaxDelayCheck) ok_st0_delay = true; else { THROW_ERROR(-2); }
    }
    if (act_dep && iphv && !concurrent && !action_output && st_fifo0_ok) return true;
    if (act_dep) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDependencyCheck), "Inconsistent %s prev-stage ACTION DEPENDENCY settings\n", iegress.iestr());
      RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "OBSERVED: StageDep=%d iphv=%d concurrent=%d act_dep=%d act_out=%d st_fifo0_ok=%d\n",
                  iegress.cur_stage_dep_, iphv, concurrent, act_dep, action_output, st_fifo0_ok);
      RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "EXPECTED: StageDep=1(ACT_DEP) iphv=1 concurrent=0 act_out=0 st_fifo0_ok=1\n");
      if (!kRelaxDependencyCheck) { THROW_ERROR(-2); }
    }
    return act_dep;
  }
  bool MauDependencies::prev_match_dep(MauDependencies01& iegress) {
    int     stage = mau_->mau_index();
    bool    match_dep = (iegress.cur_stage_dep_ == 0);
    // MAU0 always match dependent - but go via MAU feature func to make this configurable
    if (Mau::must_be_match_dependent(stage) && !match_dep) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDependencyCheck), "MAU<%d> *MUST* be match_dependent\n", stage);
      if (!kRelaxDependencyCheck) { THROW_ERROR(-2); }
    }
    bool    mux_sel_ok = true;
    bool    ophv = ((match_ie_input_mux_sel_.match_ie_input_mux_sel() & iegress.ieflag_) != 0);
    if (Mau::must_use_iphv(stage) && ophv) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDependencyCheck), "MAU<%d> *MUST* use iphv\n", stage);
      if (!kRelaxDependencyCheck) { THROW_ERROR(-2); }
      mux_sel_ok = false;
    } else if (Mau::must_use_ophv(stage) && !ophv) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDependencyCheck), "MAU<%d> *MUST* use ophv\n", stage);
      if (!kRelaxDependencyCheck) { THROW_ERROR(-2); }
      mux_sel_ok = false;
    }
    bool    concurrent = ((stage_concurrent_with_prev_.stage_concurrent_with_prev() & iegress.ieflag_) != 0);
    bool    action_output = iegress.action_output_enabled_;
    uint8_t ao_delay = iegress.action_output_delay_.action_output_delay();
    bool    ok_ao_delay = ((ao_delay >= 17) && (ao_delay <= 30)); // Uarch doc section 3.1
    if (action_output && !ok_ao_delay) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDelayCheck),
                  "MAU<%d,%s> Bad action output delay %d (Expected=%d)\n",
                  stage, iegress.iestr(), ao_delay,
                  MauDelay::stage_action_output(iegress.series_features()));
      if (kRelaxDelayCheck) ok_ao_delay = true; else { THROW_ERROR(-2); }
    }
    bool    st_fifo0 = iegress.start_table_fifo0_enabled_;
    uint8_t st0_delay = iegress.predication_ctl_.start_table_fifo_delay0();
    bool    ok_st0_delay = ((st0_delay >= 9) && (st0_delay <= 31)); // CSR doc
    if (match_dep && st_fifo0 && !ok_st0_delay) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDelayCheck),
                  "MAU<%d,%s> Bad start table fifo0 delay %d\n", stage, iegress.iestr(), st0_delay);
      if (kRelaxDelayCheck) ok_st0_delay = true; else { THROW_ERROR(-2); }
    }
    if (match_dep && mux_sel_ok && !concurrent && st_fifo0 && ok_st0_delay) return true;
    if (match_dep) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDependencyCheck), "Inconsistent %s prev-stage MATCH DEPENDENCY settings\n", iegress.iestr());
      RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "OBSERVED: StageDep=%d ophv=%d concurrent=%d st_fifo0=%d ok_st0_delay=%d (act_out=%d ok_ao_delay=%d)\n",
                  iegress.cur_stage_dep_, ophv, concurrent, st_fifo0, ok_st0_delay, action_output, ok_ao_delay);
      RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "EXPECTED: StageDep=0(MATCH_DEP) ophv=%d concurrent=0 st_fifo0=1 ok_st0_delay=1 (act_out=1 ok_ao_delay=1)\n",
                  (Mau::must_use_iphv(stage) ?0 :1));
      if (!kRelaxDependencyCheck) { THROW_ERROR(-2); }
    }
    return match_dep;
  }
  bool MauDependencies::check_dependencies(MauDependencies01& iegress) {
    // Use stated dependency to verify configuration
    if (iegress.prev_gress_deps_ != NULL) {
      if (iegress.prev_gress_deps_->next_stage_dep_ != iegress.cur_stage_dep_) {
        bool error_relax = MauDependencies::kRelaxDelayCheck;
        bool warn = Mau::kRelaxPrevStageCheck;
        RMT_LOG_OBJ(mau_, (warn) ?RmtDebug::warn() :RmtDebug::error(error_relax),
                    "MAU<%d-1,%s> next_stage_dep %d does NOT match MAU<%d,%s> cur_stage_dep\n",
                    mau_->mau_index(), iegress.iestr(),
                    iegress.prev_gress_deps_->next_stage_dep_, iegress.cur_stage_dep_,
                    iegress.iestr());
        if (!warn && !error_relax) { THROW_ERROR(-2); }
      }
    }
    switch (iegress.cur_stage_dep_) {
      case 0: return prev_match_dep(iegress);
      case 1: return prev_action_dep(iegress);
      case 2: return prev_conc(iegress);
    }
    return false;
  }
  bool MauDependencies::use_iphv(MauDependencies01& iegress) {
    //(void)check_dependencies(iegress);
    return ((match_ie_input_mux_sel_.match_ie_input_mux_sel() & iegress.ieflag_) == 0);
  }
  bool MauDependencies::use_ophv(MauDependencies01& iegress) {
    //(void)check_dependencies(iegress);
    return ((match_ie_input_mux_sel_.match_ie_input_mux_sel() & iegress.ieflag_) != 0);
  }
  bool MauDependencies::is_match_dependent(MauDependencies01& iegress) {
    //(void)check_dependencies(iegress);
    return (iegress.cur_stage_dep_ == 0);
  }
  bool MauDependencies::is_action_dependent(MauDependencies01& iegress) {
    //(void)check_dependencies(iegress);
    return (iegress.cur_stage_dep_ == 1);
  }
  bool MauDependencies::is_concurrent(MauDependencies01& iegress) {
    //(void)check_dependencies(iegress);
    return (iegress.cur_stage_dep_ == 2);
  }
  bool MauDependencies::use_iphv_for_match(MauDependencies01& iegress) {
    return use_iphv(iegress); // Input mux decides
  }
  bool MauDependencies::use_ophv_for_match(MauDependencies01& iegress) {
    return use_ophv(iegress); // Input mux decides
  }
  bool MauDependencies::use_iphv_for_action(MauDependencies01& iegress) {
    switch (iegress.cur_stage_dep_) {
      case 0: return use_iphv(iegress); // Input mux decides
      case 1: return false;             // NEVER when action dependent
      case 2: return use_iphv(iegress); // Input mux decides
      default: RMT_ASSERT(0);
    }
  }
  bool MauDependencies::use_ophv_for_action(MauDependencies01& iegress) {
    switch (iegress.cur_stage_dep_) {
      case 0: return use_ophv(iegress); // Input mux decides
      case 1: return true;              // ALWAYS when action dependent
      case 2: return use_ophv(iegress); // Input mux decides
      default: RMT_ASSERT(0);
    }
  }
  bool MauDependencies::start_table_avail(MauDependencies01& iegress) {
    // Should NEVER be called from JBay code
    RMT_ASSERT(!RmtObject::is_jbay_or_later());
    int     stage = mau_->mau_index();
    bool    st_fifo1 = iegress.start_table_fifo1_enabled_;
    uint8_t st_fifo1_delay = iegress.predication_ctl_.start_table_fifo_delay1();
    bool    ok_fifo1_delay = ((st_fifo1_delay >= 9) && (st_fifo1_delay <= 20)); // CSR
    bool    match_dep = prev_match_dep(iegress);
    // As of regs_25957_mau_dev start_table feature no longer available in MAU stage 0.
    // But go via MAU feature func though to make this configurable.
    if (match_dep && st_fifo1 && !ok_fifo1_delay) {
      RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDelayCheck),
                  "MAU<%d,%s> Bad start table fifo0 delay %d\n", stage, iegress.iestr(), st_fifo1_delay);
      if (kRelaxDelayCheck) ok_fifo1_delay = true; else { THROW_ERROR(-2); }
    }
    return Mau::uses_start_table(mau_->mau_index()) && match_dep && st_fifo1 && ok_fifo1_delay;
  }


  // NB final_output NOT used any more (see MikeF email 08/04/2015)
  // Not going to worry too much for now as these funcs not called any more
  bool MauDependencies::next_maybe_conc(MauDependencies01& iegress) {
    bool conc_dep = (iegress.next_stage_dep_ == 2);
    bool final_output = iegress.final_output_enabled_;
    if (conc_dep && !final_output) return true;
    if (!conc_dep && final_output) return false;
    RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDependencyCheck), "Inconsistent %s next-stage CONCURRENT settings\n", iegress.iestr());
    RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "OBSERVED: next_conc_dep=%d final_out_enabled=%d\n", conc_dep, iegress.final_output_enabled_);
    RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "EXPECTED: next_conc_dep=1 final_out_enabled=0\n");
    if (!kRelaxDependencyCheck) { THROW_ERROR(-2); }
    return conc_dep;
  }
  bool MauDependencies::next_maybe_action_dep(MauDependencies01& iegress) {
    bool    act_dep = (iegress.next_stage_dep_ == 1);
    bool    final_output = iegress.final_output_enabled_;
    uint8_t fifo_delay = 2; //iegress.final_output_delay_.final_output_delay();
    bool    ok_delays = ((kRelaxDelayCheck) || ((fifo_delay > 0) && (fifo_delay < 4))); //??
    if (act_dep && final_output && ok_delays) return true;
    if (!act_dep && !final_output && !ok_delays) return false;
    RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDependencyCheck), "Inconsistent %s next-stage ACTION DEPENDENCY settings\n", iegress.iestr());
    RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "OBSERVED: next_act_dep=%d final_out=%d ok_delays=%d\n", act_dep, final_output, ok_delays);
    RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "EXPECTED: next_act_dep=1 final_out=1 ok_delays=1\n");
    if (!kRelaxDependencyCheck) { THROW_ERROR(-2); }
    return act_dep;
  }
  bool MauDependencies::next_maybe_match_dep(MauDependencies01& iegress) {
    bool match_dep = (iegress.next_stage_dep_ == 0);
    bool final_output = iegress.final_output_enabled_;
    if (match_dep && !final_output) return true;
    if (!match_dep && final_output) return false;
    RMT_LOG_OBJ(mau_, RmtDebug::error(kRelaxDependencyCheck), "Inconsistent %s next-stage MATCH DEPENDENCY settings\n", iegress.iestr());
    RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "OBSERVED: next_match_dep=%d final_out_enabled=%d\n", match_dep, iegress.final_output_enabled_);
    RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "EXPECTED: next_match_dep=1 final_out_enabled=0\n");
    if (!kRelaxDependencyCheck) { THROW_ERROR(-2); }
    return match_dep;
  }

  uint8_t MauDependencies::get_delay(MauDependencies01& iegress, int which_delay) {
    return iegress.get_delay(which_delay);
  }


}
