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
#include <common/rmt-assert.h>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-color-switchbox.h>
#include <nxt-tab.h>
#include <event.h>
#include <packet.h>
#include <model_core/rmt-phv-modification.h>

//#define USE_MAU_IGNORED_REGS_EMPTY
#ifdef USE_MAU_IGNORED_REGS_EMPTY
#include <mau-ignored-regs-empty.h>
#else
#include <mau-ignored-regs.h>
#endif

#if WITH_DEBUGGER
#include <libP4Debugger.h>
#endif

namespace MODEL_CHIP_NAMESPACE {

  Mau::Mau(RmtObjectManager *om, int pipeIndex, int mauIndex, const MauConfig& config)
      : MauObject(om, pipeIndex, mauIndex, kType),
        logical_tables_(), logical_rows_(),
        sram_columns_(), sram_rows_(), srams_(), maprams_(),
        logical_tcams_(), tcam_rows_(), tcams_(), mau_lock_(),
        evaluate_all_(kEvaluateAllDefault), dependencies_changed_(false),
        action_hv_output_bus_(UINT64_C(0)),
        lookup_results_(), vpn_sram_bitmaps_(),
        time_info_(),
        modify_match_(om,pipeIndex, mauIndex, this), //added phv instantiations
        modify_action_(om,pipeIndex, mauIndex, this),
        modify_output_(om,pipeIndex, mauIndex, this),
        mau_info_(om, pipeIndex, mauIndex, this),
        mau_stash_column_(om, pipeIndex, mauIndex, this),
        mau_input_(chip_index(), pipeIndex, mauIndex, this),
        mau_dependencies_(chip_index(), pipeIndex, mauIndex, this),
        mau_predication_(om, pipeIndex, mauIndex, this),
        mau_result_bus_(chip_index(), pipeIndex, mauIndex, this),
        mau_addr_dist_(chip_index(), pipeIndex, mauIndex, this),
        mau_hash_dist_(om, chip_index(), pipeIndex, mauIndex, this),
        mau_instr_store_(chip_index(), pipeIndex, mauIndex, this),
        mau_op_handler_(om, pipeIndex, mauIndex, this),
        mau_memory_(chip_index(), pipeIndex, mauIndex, this),
        mau_moveregs_ctl_(om, pipeIndex, mauIndex, this),
        mau_table_counters_(om, pipeIndex, mauIndex, this),
        mau_stateful_counters_(om, pipeIndex, mauIndex, this),
        mau_snapshot_(om, pipeIndex, mauIndex, this),
        mau_teop_(om, pipeIndex, mauIndex, this),
        mau_mpr_tcam_table_(chip_index(), pipeIndex, mauIndex, this),
        mau_chip_(chip_index(), pipeIndex, mauIndex, this),
        mau_ignored_regs_(NULL), mau_previous_(NULL),
        powered_ltcams_(0), inval_output_selector_alus_(0),
        data_oflo_rows_(0), egress_meter_alus_(0) {

    static_assert( (kSramRows * kSramColumns == kSrams),
                   "N srams per col * N srams per row MUST be same as total N srams");
    static_assert( (kTcamRows * kTcamColumns == kTcams),
                   "N tcams per col * N tcams per row MUST be same as total N tcams");
    static_assert( (kLogicalRows * kLogicalColumns == kSrams),
                   "N log.srams per col * N log.srams per row MUST be same as total N srams");
    // If sram/mapram config not 1-1 mau_init_srams/reset_resources/mau_free_all
    // will all need to change
    static_assert( ((kMapramRows == kSramRows) && (kMapramColumns == kSramColumns) &&
                    (kMapramRowFirstTOP == kSramRowFirstTOP)),
                   "N maprams & mapram layout MUST be same as N srams & sram layout");

    // Now CTOR mostly done install ourself into the MauObject
    set_mau(this);

    all_en_.fill_all_ones();
    for (int i = 0; i < kVpns; i++) vpn_sram_bitmaps_[i].fill_all_zeros();
    next_tables_[0] = -1;
    next_tables_[1] = -1;

    for (int i = 0; i < kLogicalTables; i++ )
      lookup_results_[i].init(this,&mau_result_bus_);

    time_info_.set_relative_time(UINT64_C(0));
    for (int i = 0; i < RmtDefs::kStagesMax; i++) {
      for (int j = 0; j < kNumMeterAlus; j++) time_info_.set_meter_tick_time(i, j, UINT64_C(0));
    }

    // Dynamically allocate mau_ignored_regs_ - if we include the MauIgnoredRegs object
    // within the Mau object, then we need to include mau-ignored-regs.h in mau.h and
    // that slows down everything!!!
    mau_ignored_regs_ = new MauIgnoredRegs(chip_index(), pipeIndex, mauIndex, this);

    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCreate), "MAU::create\n");
  }
  Mau::~Mau() {
    if (mau_ignored_regs_ != NULL) delete mau_ignored_regs_;
    for (auto step : match_only_steps_) delete(step);
    for (auto step : at_hdr_steps_)  delete(step);
    for (auto step : at_eop_steps_)  delete(step);
    for (auto step : at_teop_steps_) delete(step);
    for (auto step : pbus_rd_steps_) delete(step);
    for (auto step : pbus_wr_steps_) delete(step);
    for (auto step : sweep_steps_)   delete(step);
    for (auto step : stateful_clear_steps_) delete(step);

    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDelete), "MAU::delete\n");
  }


  // Only DV uses these find_ funcs any more
  int Mau::find_next_table(const bool ingress, int curr_table_id, int next_table_id) {
    RmtObjectManager *om = get_object_manager();
    // curr_table_id must NOT be from a later stage
    RMT_ASSERT((curr_table_id < 0) || (NxtTab::which_mau(curr_table_id) <= mau_index()));
    // A. Reset curr_table_id if for earlier stage.
    //    But complain and return inval_next_table() if next_table_id is for earlier stage
    //    (see Pat's email "small functional change for timing" 09 Aug 2015)
    if ((curr_table_id >= 0) && (NxtTab::which_mau(curr_table_id) < mau_index()))
      curr_table_id = -1;
    if ((next_table_id >= 0) && (NxtTab::which_mau(next_table_id) < mau_index())) {
      //next_table_id = -1;
      if ((curr_table_id >= 0) && (NxtTab::which_mau(next_table_id) >= 0)) {
        RMT_LOG(RmtDebug::error(),
                "Invalid stage %d (< this stage %d) in next_table %d (curr_table %d)\n",
                NxtTab::which_mau(next_table_id), mau_index(), next_table_id, curr_table_id);
      }
      return NxtTab::inval_next_table();
    }
    // B. If curr_table is very LAST table in this stage and either:
    // 1. next_table is not specified (-1) or
    // 2. next_table is specified and also in this stage
    // then bump next_table to next stage.
    // (if there is no next stage then we'll return inval_next_table() in C below)
    if (curr_table_id == NxtTab::make_next_table(mau_index(), kLogicalTables-1)) {
      if ((next_table_id < 0) || (NxtTab::which_mau(next_table_id) == mau_index())) {
        next_table_id = NxtTab::make_next_table(1+mau_index(), 0);
      }
    }
    // C. If next_table_id for later stage hand off processing to relevant MAU
    if ((next_table_id >= 0) && (NxtTab::which_mau(next_table_id) > mau_index())) {
      Mau *mau = om->mau_lookup(pipe_index(), NxtTab::which_mau(next_table_id));
      if (mau != NULL) {
        return mau->find_next_table(ingress, -1, next_table_id);
      } else {
        return NxtTab::inval_next_table();
      }
    }
    // D. Otherwise we look in this stage - try to start at curr_table_id+1
    // (Note curr_table can never be last table (15) because of logic in step B)
    if (next_table_id < 0) {
      if (curr_table_id < 0) {
        next_table_id = NxtTab::make_next_table(mau_index(), 0); // Start from 0
      } else {
        next_table_id = NxtTab::make_next_table(mau_index(), 1+NxtTab::which_table(curr_table_id));
      }
    } else {
      if ((curr_table_id >= 0) && (next_table_id <= curr_table_id)) {
        next_table_id = NxtTab::make_next_table(mau_index(), 1+NxtTab::which_table(curr_table_id));
      }
    }
    RMT_ASSERT(next_table_id >= 0);
    // E. Now we have some value next_table_id tick up through tables starting
    // from this point until we find a table that's valid for ingress/egress
    for (int ti = NxtTab::which_table(next_table_id); ti < kLogicalTables; ti++) {
      MauLogicalTable *table = logical_table_lookup(ti);
      if ((table != NULL) && (table->is_active()) && (table->check_ingress_egress(ingress))) {
        return NxtTab::make_next_table(mau_index(), ti);
      }
    }
    // F. If get here, still have NOT found a table, so drop through to look in
    // any available succeeding MAU
    for (int stage = 1+mau_index(); stage < om->num_stages(); stage++) {
      Mau *mau = om->mau_lookup(pipe_index(), stage);
      if (mau != NULL) return mau->find_next_table(ingress, -1, next_table_id);
    }
    return NxtTab::inval_next_table();
  }
  int Mau::find_first_table(const bool ingress) {
    return find_next_table(ingress, -1, -1);
  }


  void Mau::process_snap_start(MauExecuteState* state) {
    // Maybe capture a snapshot
    // Once we know whether there's been a snapshot, process_pred_start
    // (called as a later step) will know whether to evaluate ALL logical tables
    (void)mau_snapshot()->maybe_snapshot(state->match_phv_, state->action_phv_);
  }
  void Mau::process_snap_end(MauExecuteState* state) {
    if (snapshot_captured()) {
      // Finalize snapshot - captures output data and hit/miss/phys_hit_addr info
      mau_snapshot()->finalize_snapshot(state->next_ophv_,
                                        state->next_table_[0], state->next_table_[1]);
    }
  }
  void Mau::process_pred_start(MauExecuteState* state) {
    // Initialise predication - might need to take account of snapshot
    pred_set_evaluate_all(evaluate_all() || snapshot_captured());
    bool thread_active[MauPredicationCommon::kThreads] = { state->match_phv_->ingress(),
                                                           state->match_phv_->egress(),
                                                           state->match_phv_->ghost() };
    pred_start(thread_active);
  }
  void Mau::process_pred_end(MauExecuteState* state) {
    // Finish predication
    pred_end();
  }

  void Mau::process_lookup(bool ingress, MauExecuteState* state) {
    // New flavour lookup
    uint16_t lts_gress = pred_lt_info(ingress? Pred::kIngThread :Pred::kEgrThread);
    uint16_t new_match_lookups = 0, new_gwonly_lookups = 0;

    // Get first LT to lookup for gress
    // NOTE, not necessarily an active table (selected by nxt-tab),
    // just one that needs to be looked up in. It might be because
    // we need its result for snapshot, or maybe it has counters,
    // or maybe we're in DV mode looking up everything we can.
    //
    bool do_match_lookup = false;
    int pred_lt = pred_get_first_table(ingress, &do_match_lookup);
    while (pred_lt >= 0) {
      RMT_ASSERT(pred_lt < kLogicalTables);
      // pred_lt must be in correct gress and shouldn't repeat
      RMT_ASSERT(((lts_gress >> pred_lt) & 1) == 1);
      lts_gress &= ~(1<<pred_lt);

      if (do_match_lookup) new_match_lookups  |= 1<<pred_lt;
      else                 new_gwonly_lookups |= 1<<pred_lt;

      MauLogicalTable *table = logical_table_lookup(pred_lt);
      RMT_ASSERT((table != NULL) && (table->check_ingress_egress(ingress)));
      MauLookupResult& result = lookup_results_[pred_lt];
      result.reset();
      result.setup_lookup(state->match_phv_, pred_lt);

      // Do lookup - last param determines whether we lookup in SRAMs/TCAMs
      // (if false we only lookup in Gateways/Stashes)
      RMT_LOG(RmtDebug::verbose(),
              "MAU::process_lookup(%s %s) -> match lookup LT %d(0x%02x)\n",
              ingress?"ingress":"egress", do_match_lookup?"match":"gateway/stash",
              pred_lt, pred_lt);
      table->lookup_match(state->match_phv_, &result, ingress, do_match_lookup);

      // Extract the looked-up next-table - updates next_table_form internally
      uint16_t next_lt = result.extract_next_table(this, pred_lt);

      // Call find_next_table to figure out what nxt_tab should be
      if (kSetNextTablePred)
        result.set_next_table_pred(find_next_table(ingress, pred_lt, next_lt));

      if (!state->match_phv_->match_only()) {
        // Assuming counter is configured to count misses, we will count
        // a miss if no match lookup above because of power-saving.
        // (XXX)
        mau_table_counters()->maybe_increment_table_counter(pred_lt, result);
      }

      // Is pred_lt active?
      uint16_t lts_active = pred_lt_info(ingress? Pred::kIngActive :Pred::kEgrActive);
      bool active = (((lts_active >> pred_lt) & 1) == 1);
      // Tell predication about next_table found
      pred_set_next_table(pred_lt, result);
      // If no match lookup we mark result invalid (no addresses extracted)
      if (!do_match_lookup && !active) result.set_valid(false); else result.set_active(active);
      // Get next LT to lookup for gress
      pred_lt = pred_get_next_table(ingress, pred_lt, &do_match_lookup);

    } // while (pred_lt >= 0)

    if (Mau::kResetUnusedLookupResults) { // Only DV_MODE does this
      // Combine notrun LTs for this thread and unused LTs (not in any thread)
      // and initialise the MauLookupResult objects but mark them invalid
      // (we'll do unused ones twice on ingress and egress but hey).
      // Have to do this as DV wrapper calls extract funcs unconditionally.
      uint16_t lts_notrun_unused = lts_gress | pred_lt_info(Pred::kUnused);
      for (int ti = 0; ti < kLogicalTables; ti++) {
        if (((lts_notrun_unused >> ti) & 1) == 1) {
          MauLookupResult& result = lookup_results_[ti];
          result.reset();
          result.setup_lookup(state->match_phv_, ti);
          result.set_valid(false);
        }
      }
    }
    if (kLookupUnusedLtcams) {
      RMT_LOG(RmtDebug::warn(),
              "MAU::process_lookup: kLookupUnusedLtcams=true !!! "
              "Gateways that inhibit TCAM table lookups may see corrupted results\n");

      // Special switch to ensure we execute any un-run LTCAMs.
      //
      // But careful with usage as code currently does *not* discriminate
      // unrun LTCAMs from inhibited LTCAMs so G/W results may get corrupted.
      //
      // This can be necessary as a physical TCAM can drive many LTCAMs.
      // An LTCAM therefore may need to be run even though it is not
      // associated with any powered LogicalTable - so it gets run
      // as a side-effect of other LogicalTable/LTCAMs being run
      //
      // NB. The MauLogicalTcam code will only run physical TCAMs
      // that it should as it checks whether each physical TCAM is
      // driving some powered LTCAM before executing it.
      MauLookupResult dummy;
      for (int ltc = 0; ltc < kLogicalTcams; ltc++) {
        MauLogicalTcam *ltcam = logical_tcam_lookup(ltc);
        if ((ltcam != NULL) && (!ltcam->has_run())) {
          // Run for egress then ingress - should be harmless
          dummy.reset();
          ltcam->lookup_ternary_match(state->match_phv_, -1, &dummy, false, true);
          dummy.reset();
          ltcam->lookup_ternary_match(state->match_phv_, -1, &dummy, true, true);
        }
      }
    }
  }

  void Mau::tick_stateful_counters(MauExecuteState* state) {
    mau_stateful_counters_.tick_counters( lookup_results_, state);
  }
  void Mau::process_update_next_table(bool ingress, MauExecuteState* state) {
  }





  void Mau::execute(Phv *iphv, Phv *ophv, Phv** next_iphv, Phv** next_ophv ) {

    ABORT_IF_GLOBAL_ERROR; // Will abort if this func recalled post assert throw

    // Get handle on the I/O objects for this MAU
    MauIO *inIO = mau()->mau_io_input();
    MauIO *outIO = mau()->mau_io_output();
    RMT_ASSERT((inIO != NULL) && (outIO != NULL));

    // Maybe allocate and install teop (only on JBay)
    if (mau_teop_.teop_available() &&
        (iphv != NULL) && (ophv != NULL) && (iphv->egress())) {
      RMT_ASSERT(ophv->egress());
      RMT_ASSERT(iphv->teop() == ophv->teop()); // Could be NULL
      if (iphv->teop() == NULL) {
        Teop *teop = mau_teop_.teop_allocate();
        iphv->set_teop(teop);
        ophv->set_teop(teop);
      }
    }

    int ing_eopnum_iphv = (iphv != NULL) ?iphv->ingress_eopnum() :0xFF;
    int egr_eopnum_iphv = (iphv != NULL) ?iphv->egress_eopnum() :0xFF;
    int hash_iphv = (iphv != NULL) ?iphv->hash() :0xFFFFFFFF;
    int ing_eopnum_ophv = (ophv != NULL) ?ophv->ingress_eopnum() :0xFF;
    int egr_eopnum_ophv = (ophv != NULL) ?ophv->egress_eopnum() :0xFF;
    int hash_ophv = (ophv != NULL) ?ophv->hash() :0xFFFFFFFF;
    int ing_nxt_tab = inIO->ingress_nxt_tab();
    int egr_nxt_tab = inIO->egress_nxt_tab();
    RMT_LOG(RmtDebug::info(), "MAU::process_match "
            "iphv_hash=0x%08x ophv_hash=0x%08x "
            "ingress iphv_eop=%d,ophv_eop=%d,nxt_tab=%d  "
            "egress iphv_eop=%d,ophv_eop=%d,nxt_tab=%d\n",
            hash_iphv, hash_ophv,
            ing_eopnum_iphv, ing_eopnum_ophv, ing_nxt_tab,
            egr_eopnum_iphv, egr_eopnum_ophv, egr_nxt_tab);

    // Check time moves forward
    if (iphv != NULL) {
      if (iphv->is_before(time_info_,true) || iphv->is_before(time_info_,false)) {
        RMT_LOG(RmtDebug::warn(), "MAU::process_match "
                "Time gone backwards! T_mau=%" PRIu64 " T_iphv_ingress=%" PRIu64 " "
                "T_iphv_egress=%" PRIu64 "\n", time_info_.get_relative_time(),
                iphv->get_relative_time(true), iphv->get_relative_time(false));
      }
      else time_info_.set_from(iphv->get_time_info(true));
    }

    // In MAU0 ingress|egress_next_table are not available
    // Check via MAU feature func
    if (Mau::ignores_start_table(mau_index())) {
      ing_nxt_tab = NxtTab::make_next_table(mau_index(), 0);
      egr_nxt_tab = NxtTab::make_next_table(mau_index(), 0);
    }

    MauExecuteState state(iphv, ophv, ing_nxt_tab, egr_nxt_tab);

    // MAU0 has ophv/iphv (Tofino/JBay) wired up to 0 - but check via
    // MAU feature func to make this configurable.
    //
    // Ignore passed in ophv/iphv and use a zero_phv in its place
    // Caller will free ophv/iphv as normal - zero_phv freed on this func exit.
    Phv *zero_phv = NULL;
    if (Mau::ophv_is_zero(mau_index()) && (ophv != NULL)) {
      zero_phv = ophv->clone(); // Just hdr info, so data stays all 0
      ophv = zero_phv;
    } else if (Mau::iphv_is_zero(mau_index()) && (iphv != NULL)) {
      zero_phv = iphv->clone();
      iphv = zero_phv;
    }


    if (enabled()) {

      lock_resources();
      // Handle changes to dependencies
      if (dependencies_changed_) handle_dependencies_changed();

      try {

        next_tables_[0] = ing_nxt_tab;
        next_tables_[1] = egr_nxt_tab;
        state.match_phv_  = make_match_phv(iphv,ophv);
        state.action_phv_ = make_action_phv(iphv,ophv);

        if (state.match_phv_->match_only()) {
          // Only do front-end stuff (eg lookup)
          for (auto step : match_only_steps_) step->execute(&state);

          // Setup next_ophv_ in case caller looks at it
          state.next_ophv_ = state.action_phv_;

          mau_info_incr(MAU_N_MATCH_PHVS); // Count match_only PHVs

        } else {
          // All normal HDR steps
          for (auto step : at_hdr_steps_) step->execute(&state);

          mau_info_incr(MAU_N_PHVS); // Count normal PHVs
        }

        // Debug LOG MAU results -
        // Results for all tables are logged here
        RmtObjectManager *om = get_object_manager();
        for (int t=0; t<kLogicalTables; t++) {
          MauLogicalTable *table = logical_table_lookup(t);
          if (!table) {
            continue;
          }
          auto result = lookup_results_[t];
          bool ingress = logical_table_lookup(t)->is_ingress();
          // Try to get pkt_id for event logging
          uint64_t pkt_id = 0;
          if (ingress && ophv->ingress_packet()) {
            pkt_id = ophv->ingress_packet()->pkt_id();
          } else if (ophv->egress_packet()) {
            pkt_id = ophv->egress_packet()->pkt_id();
          }

          const char *dir_str = ingress ? "Ingress" : "Egress";
          Gress gress = ingress ? Gress::ingress : Gress::egress;
          std::list<std::pair<int, uint32_t>> empty_list;
          if (result.valid()) {
	    auto table_name = om->get_table_name(pipe_index(), s_index(), t);
            if (result.active()) {

              // Figure out if this was a stateful table - find op4 meter_addr
              int meter_alu = -1, stateful_instr = -1;
              uint32_t meter_addr3 = result.extract_meter_addr(this, t);
              constexpr bool sop_col_aware_becomes_sweep = is_jbay_or_later();
              uint32_t meter_addr = Address::meter_addr_map_op3en_to_op4(meter_addr3, 0,
                                                                         sop_col_aware_becomes_sweep);
              if (Address::meter_addr_op_enabled(meter_addr)) {
                // Result below will be -1 if meter_addr is not a stateful_addr
                stateful_instr = Address::meter_addr_stateful_instruction(meter_addr);
                meter_alu = mau_addr_dist_.get_meter_alu(t);
              }

              uint16_t nxt_tab = result.next_table();
              auto next_table_stage = NxtTab::which_mau(nxt_tab);
              auto next_table_table = NxtTab::which_table(nxt_tab);
              auto next_table_name = om->get_table_name(pipe_index(),
                                                        next_table_stage, next_table_table);
              bool match_table_is_inhibited = false;
              bool match_table_is_missing = false;

              if (table->gateway_is_enabled()) {
                std::string condition_name = om->get_gateway_condition_name(pipe_index(), s_index(), t);
                if (result.gatewayran()) {
                  RMT_P4_LOG_INFO("%s : Gateway table condition (%s) %s.\n",
                                      dir_str,
                                      condition_name.c_str(),
                                      result.gatewaymatch() ? "matched" : "not matched");
                }

                /**
                 * Gateways can be attached to tables. In the model we detect an attached
                 * table if there are any logical TCAMs or SRAM columns used, which incidates
                 * that there is an exact match table at this table address. However, this
                 * does not catch other table types (e.g. match_with_no_key, stateful, stats & meters).
                 * To detect attached non-match tables we can refer to context.json, which is optionally
                 * provided for logging (p4_name_lookup).
                 * For non-match tables, result.gatewayinhibit() changes meaning. If true, the gateway can
                 * provide the payload for the attached table, and the actions of the attached table (if any)
                 * will be executed.
                 */

                // If there is an attached match table at this table address
                if (table->gateway_has_attached_table()) {
                  // If the match table is inhibited and the results (action & statefulALU
                  // info) are provided by the gateway payload
                  if (result.gatewayinhibit()) {
                    match_table_is_inhibited = true;
                    RMT_P4_LOG_INFO("%s : Table %s is inhibited by a gateway condition\n",
                                        dir_str, table_name.c_str());
                    RMT_P4_LOG_INFO("%s : Gateway %s provide payload.\n",
                                        dir_str, result.gateway_payload_disabled() ? "did not" : "did");
                  } else {
                    RMT_P4_LOG_INFO("%s : Associated table %s is executed\n", dir_str, table_name.c_str());
                  }
                // If there is a non-match table attached at this table address
                // (note: depends on p4_name_lookup & context.json)
                } else if (om->get_gateway_has_attached_table(pipe_index(), s_index(), t)) {
                  match_table_is_missing = true;
                  RMT_P4_LOG_INFO("%s : Gateway attached to %s\n", dir_str, table_name.c_str());
                  // In the absence of a match table the gateway can provide the payload for the attached table
                  if (result.gatewayinhibit()) {
                    RMT_P4_LOG_INFO("%s : Associated table %s is executed\n", dir_str, table_name.c_str());
                    RMT_P4_LOG_INFO("%s : Gateway %s provide payload.\n",
                                        dir_str, result.gateway_payload_disabled() ? "did not" : "did")
                  } else {
                    RMT_P4_LOG_INFO("%s : Table %s is inhibited by a gateway condition\n",
                                        dir_str, table_name.c_str());
                  }
                // No attached table.
                } else {
                  match_table_is_missing = true;
                  if (result.gatewayinhibit() && !result.gateway_payload_disabled()) {
                    RMT_P4_LOG_INFO("%s : Gateway did provide payload.\n", dir_str);
                  }
                }
              }
              // If there is a match table which is not inhibited, print hit/miss & match key info
              if (!match_table_is_inhibited && !match_table_is_missing) {
                RMT_P4_LOG_INFO("%s : Table %s is %s\n", dir_str, table_name.c_str(),
                                    result.match() ? "hit" : "miss");
                RMT_P4_LOG_VERBOSE("Key:\n");
                rmt_log_P4_table_key(state.match_phv_, t, !ingress);
              }
              // Print action info
              if ((meter_alu >= 0) && (stateful_instr >= 0)) {
                RMT_P4_LOG_VERBOSE("Executed StatefulALU %d with instruction %d\n",
                                meter_alu, stateful_instr);
                log_stateful_alu(pkt_id, gress, t, meter_alu, stateful_instr);
              }
              uint32_t instr_raw_addr = result.extract_action_instr_raw_addr(this, t);
              if (Address::action_instr_enabled(instr_raw_addr)) {
                auto action_name = rmt_act_instr_to_name(t, instr_raw_addr);
                RMT_P4_LOG_VERBOSE("Execute%sAction: %s\n", result.match() ? " " : " Default ", action_name.c_str());
                int row = rmt_log_get_salu_row(t, action_name);
                rmt_log_p4_action_info(t, action_name,
                                       state.match_phv_/* input phv */,
                                       state.next_ophv_/* result phv */,
                                       (row >= 0 && row < 8) ?
                                       state.salu_log_valuelist_[row] : empty_list,
                                       result);
              }
              // TODO next_table_name may be invalid if result.match() == false
              RMT_P4_LOG_VERBOSE("Next Table = %s\n", next_table_name.c_str());
              auto stats_addr = result.extract_stats_addr(this, t);
              auto addr_consumed = table->mau_addr_dist()->addr_consumed(stats_addr);
              if (result.match()) {
                log_table_hit(pkt_id, gress, t, next_table_stage, next_table_table, instr_raw_addr, stats_addr, addr_consumed);
              }
            } else {
              RMT_P4_LOG_INFO("%s : Table %s is not active(inhibited/skipped)\n", dir_str,
                          table_name.c_str());
            }
          }
        }

      } catch (const std::runtime_error&) {
        // DV_MODE and RMT_ASSERT() or THROW_ERROR() called.
        // NULL these out so caller sees we had a serious error
        state.match_phv_ = NULL;
        state.next_ophv_ = NULL;
      }


      // Sanity check that all distibuted addresses have been used somewhere
      // XXX: do this before releasing lock
      if (!all_addrs_consumed()) mau_addr_dist()->addrs_leftover_check();

      unlock_resources();

      if ((state.match_phv_ != NULL) && (state.match_phv_->ieg())) {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauLookup),
                "MAU::process_match table_active=0x%s\n",
                table_active().to_string().c_str());
      }

      // State may contain evictions that need processing
      mau_addr_dist_.handle_evictions(state.evict_info_);
    }

    if (zero_phv != NULL) {
      RmtObjectManager *om = get_object_manager();
      RMT_ASSERT(om != NULL);
      // Should only get here if Mau::ophv_is_zero or Mau::iphv_is_zero
      RMT_ASSERT(Mau::ophv_is_zero(mau_index()) || Mau::iphv_is_zero(mau_index()));
      // action_phv should always be a clone (or NULL if error)
      RMT_ASSERT(state.action_phv_ != zero_phv);
      // match_phv might still be zero_phv in which case don't free
      if (state.match_phv_ != zero_phv) om->phv_delete(zero_phv);
    }

    *next_iphv = state.match_phv_;
    *next_ophv = state.next_ophv_;
  }

  void Mau::execute(Phv *iphv, Phv *ophv,
                    int *ingress_next_table, int *egress_next_table, int *ghost_next_table,
                    Phv** next_iphv, Phv** next_ophv ) {
    RMT_ASSERT((ingress_next_table != NULL) && (egress_next_table != NULL));
    RMT_ASSERT(ghost_next_table != NULL);

    // Get handle on the I/O objects for this MAU
    MauIO *inIO = mau()->mau_io_input();
    MauIO *outIO = mau()->mau_io_output();
    RMT_ASSERT((inIO != NULL) && (outIO != NULL));

    // On Tofino in MAU0 ingress|egress_next_table are not available
    // Check via MAU feature func
    if (Mau::ignores_start_table(mau_index())) {
      inIO->set_ingress_nxt_tab(NxtTab::make_next_table(mau_index(), 0));
      inIO->set_egress_nxt_tab(NxtTab::make_next_table(mau_index(), 0));
      inIO->set_ghost_nxt_tab(NxtTab::make_next_table(mau_index(), 0));
    } else {
      inIO->set_ingress_nxt_tab(*ingress_next_table);
      inIO->set_egress_nxt_tab(*egress_next_table);
      if (*ghost_next_table >= 0) inIO->set_ghost_nxt_tab(*ghost_next_table);
    }

    execute(iphv, ophv, next_iphv, next_ophv);

    *ingress_next_table = outIO->ingress_nxt_tab();
    *egress_next_table = outIO->egress_nxt_tab();
    *ghost_next_table = outIO->ghost_nxt_tab();
  }

  // This makes a new phv (free the returned match_phv when done)
  Phv *Mau::make_match_phv(Phv *iphv, Phv *ophv) {
    MauDependencies *deps = mau_dependencies();
    RMT_ASSERT (deps != NULL);
    Phv *match_phv;
    // Synthesize match_phv from iphv/ophv
    Phv *ingress_src_phv = deps->ingress_use_ophv_for_match() ?ophv :iphv;
    Phv *egress_src_phv = deps->egress_use_ophv_for_match() ?ophv :iphv;
    if (ingress_src_phv == egress_src_phv)
      match_phv = ingress_src_phv->clone();
    else
      match_phv = ophv->clone(); // Just pick ophv
    match_phv->copydata(ingress_src_phv, ingress_selector());
    match_phv->copyinfo(ingress_src_phv, true); // true => ingress
    match_phv->copydata(egress_src_phv, egress_selector());
    match_phv->copyinfo(egress_src_phv, false); // false => egress

   //adding call to apply modification method
    modify_match_.apply_modification(match_phv, RmtLogger::rmt_log_check(RmtDebug::verbose()));
    return match_phv;
  }
  // This makes a new phv (free the returned action_phv when done)
  Phv *Mau::make_action_phv(Phv *iphv, Phv *ophv) {
    MauDependencies *deps = mau_dependencies();
    RMT_ASSERT (deps != NULL);
    Phv *action_phv;
    // Synthesize action_phv from iphv/ophv
    Phv *ingress_src_phv = deps->ingress_use_ophv_for_action() ?ophv :iphv;
    Phv *egress_src_phv = deps->egress_use_ophv_for_action() ?ophv :iphv;
    if (ingress_src_phv == egress_src_phv)
      action_phv = ingress_src_phv->clone();
    else
      action_phv = ophv->clone(); // Just pick ophv
    action_phv->copydata(ingress_src_phv, ingress_selector());
    action_phv->copyinfo(ingress_src_phv, true); // true => ingress
    action_phv->copydata(egress_src_phv, egress_selector());
    action_phv->copyinfo(egress_src_phv, false); // false => egress

    if (deps->ingress_is_concurrent() || deps->egress_is_concurrent()) {
      action_phv->start_recording_written();
    }
    //adding call to apply modification method
    modify_action_.apply_modification(action_phv, RmtLogger::rmt_log_check(RmtDebug::verbose()));
    return action_phv;
  }
  // This makes a new phv (free the returned output_phv when done)
  Phv *Mau::make_output_phv(Phv *action_phv, Phv *ophv) {
    RmtObjectManager *om = get_object_manager();
    MauDependencies *deps = mau_dependencies();
    RMT_ASSERT((om != NULL) && (deps != NULL));
    Phv *output_phv;
    // Synthesize output_phv from action_phv/ophv
    Phv *ingress_src_phv = deps->ingress_is_concurrent() ?ophv :action_phv;
    Phv *egress_src_phv = deps->egress_is_concurrent() ?ophv :action_phv;
    if (ingress_src_phv == egress_src_phv)
      output_phv = ingress_src_phv->clone();
    else
      output_phv = action_phv->clone(); // Just pick action_phv
    output_phv->copydata(ingress_src_phv, ingress_selector());
    output_phv->copyinfo(ingress_src_phv, true); // true => ingress
    output_phv->copydata(egress_src_phv, egress_selector());
    output_phv->copyinfo(egress_src_phv, false); // false => egress

    if (deps->ingress_is_concurrent()) {
      // Overwrite with written,ingress words from action_phv
      RMT_ASSERT(action_phv->written_bv() != NULL);
      BitVector<kPhvWords> ingress_written;
      action_phv->written_bv()->extract_into(0, &ingress_written);
      ingress_written.mask(*ingress_selector());
      output_phv->copydata(action_phv, &ingress_written);
    }
    if (deps->egress_is_concurrent()) {
      // Overwrite with written,egress words from action_phv
      RMT_ASSERT(action_phv->written_bv() != NULL);
      BitVector<kPhvWords> egress_written;
      action_phv->written_bv()->extract_into(0, &egress_written);
      egress_written.mask(*egress_selector());
      output_phv->copydata(action_phv, &egress_written);
    }
    // Wrote unit-test ChipTest.NoPhvLeakage to confirm this does not leak PHVs
    om->phv_delete(action_phv);
    //adding call to apply modification method
    modify_output_.apply_modification(output_phv, RmtLogger::rmt_log_check(RmtDebug::verbose()));
    return output_phv;
  }



  void Mau::process_action(MauExecuteState* state) {
    if (!enabled()) return;
    //Phv* iphv = state->iphv_;
    Phv* ophv = state->ophv_;
    Phv* action_phv = state->action_phv_;

    RmtObjectManager *om = get_object_manager();
    MauInstrStore *instrStore = mau_instr_store();
    RMT_ASSERT ((om != NULL) && (instrStore != NULL));

    // Then tell the instrStore about all instructions we found
    instrStore->instr_reset(action_phv);
    for (int ti = 0; ti < kLogicalTables; ti++) {
      MauLookupResult& result = lookup_results_[ti];
      if (result.active()) {
        // Only process active (predicated) tables
        // Get instr_op - if not a bitmask op it will have ingress/egress bit at MSB
        uint32_t instr_op = result.extract_action_instr_addr(this,ti);
        if (instrStore->instr_add_op(ti, result.ingress(), instr_op)) {
          RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauProcessAction),
                  "MAU::process_action: Adding InstrOP=0x%02x (T=%d)\n", instr_op, ti);
        } else {
          RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauProcessAction),
                  "MAU::process_action: InstrOP=0x%02x (T=%d) DISABLED\n", instr_op, ti);
        }
      }
    }
    // And fish out the composite Instr formed
    Instr *instr = instrStore->instr_get();
    int phv0 = -1;
    for (int i = 0; i < Instr::kInstrsMax; i++) {
      uint32_t v = instr->get(i);
      if (v != 0u) {
        if (phv0 < 0) phv0 = i; // Track first instruction
        auto phv_name = om->get_phv_name(pipe_index(), s_index(), i);
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauProcessAction),
                "MAU::process_action: %s (phv%d) = instr%d (0x%08x)\n",
                phv_name.c_str(), i, v, v);
      }
    }
    // Print the action bus for debug if there are instructions.
    if (phv0 != -1) dump_action_hv_bus(action_hv_output_bus_);


    // We pass the action_hv_output_bus, the selected instr
    // and the new action_phv to the ActionEngine for processing!!
    if (Phv::is_valid_phv(phv0)) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauProcessAction),
              "MAU::process_action: Input1: phv%d=0x%08x\n", phv0, action_phv->get(phv0));
    }
    (void)alu_main(action_phv, instr, action_hv_output_bus_);
    if (Phv::is_valid_phv(phv0)) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauProcessAction),
              "MAU::process_action: Output1: phv%d=0x%08x\n", phv0, action_phv->get(phv0));
    }

    // Determine next_ophv
    state->next_ophv_ = make_output_phv(action_phv, ophv);
  }

  int Mau::alu_main(Phv *action_phv, Instr *instr,
                    const BitVector<kActionHVOutputBusWidth> &action_bus) {
    RmtObjectManager *om = get_object_manager();
    RMT_ASSERT(action_phv && instr && om);


    // Copy the PHV since the ALUs will execute sequentially rather than all in
    // parallel and any PHV fields used as instruction inputs must be the
    // unmodified verion.
    Phv *src_phv = action_phv->clone(true);

    // Zeroise words in src_phv NOT set in actionmux_en_selector
    // in order to emulate corresponfing Tofino DinPower feature
    if (kMauDinPowerMode) src_phv->maskdata(actionmux_en_selector());

    int error = instr->execute(src_phv, action_bus, action_phv);
    mau_info_incr(MAU_VLIW_ALU_EXECUTIONS, instr->get_execute_count());

#if WITH_DEBUGGER
    for (int i=0; i<Phv::kWordsMax; ++i) {
      if (src_phv->get_ignore_valid_bit(i) !=
          action_phv->get_ignore_valid_bit(i)) {
        register_handle_t h = RmtDefs::p4dHandle_phv(chip_index(), pipe_index(),
                                                     mau_index(), i);
        uint32_t x = action_phv->get_ignore_valid_bit(i);
        p4d_updateRegister(h, (unsigned char*)&x);
      }
    }
#endif
    om->phv_delete(src_phv);

    return error;

  }


  // Dump action_hv_output_bus
  void Mau::dump_action_hv_bus(const BitVector<kActionHVOutputBusWidth>& a_hv_bus) {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauDumpActionHVOutputBus),
            "MAU::dump_ac_hv_bus: ActBus Byte Idx                        "
            "1 1  1 1 1 1  1 1 1 1  2 2 2 2  2 2 2 2  2 2 3 3\n");
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauDumpActionHVOutputBus),
            "MAU::dump_ac_hv_bus: ActBus Byte Idx  0 1 2 3  4 5 6 7  8 9 "
            "0 1  2 3 4 5  6 7 8 9  0 1 2 3  4 5 6 7  8 9 0 1\n");
    for (int i=0; i<(kActionHVOutputBusWidth/32); i+=8) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauDumpActionHVOutputBus),
              "MAU::dump_ac_hv_bus: ActBus[%3d-%3d] %02x%02x%02x%02x "
              "%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x "
              "%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x\n",
              4*i, 4*(i+7)+3,
              (a_hv_bus.get_word((i+0)*32, 32) >>  0) & 0xFF,
              (a_hv_bus.get_word((i+0)*32, 32) >>  8) & 0xFF,
              (a_hv_bus.get_word((i+0)*32, 32) >> 16) & 0xFF,
              (a_hv_bus.get_word((i+0)*32, 32) >> 24) & 0xFF,
              (a_hv_bus.get_word((i+1)*32, 32) >>  0) & 0xFF,
              (a_hv_bus.get_word((i+1)*32, 32) >>  8) & 0xFF,
              (a_hv_bus.get_word((i+1)*32, 32) >> 16) & 0xFF,
              (a_hv_bus.get_word((i+1)*32, 32) >> 24) & 0xFF,
              (a_hv_bus.get_word((i+2)*32, 32) >>  0) & 0xFF,
              (a_hv_bus.get_word((i+2)*32, 32) >>  8) & 0xFF,
              (a_hv_bus.get_word((i+2)*32, 32) >> 16) & 0xFF,
              (a_hv_bus.get_word((i+2)*32, 32) >> 24) & 0xFF,
              (a_hv_bus.get_word((i+3)*32, 32) >>  0) & 0xFF,
              (a_hv_bus.get_word((i+3)*32, 32) >>  8) & 0xFF,
              (a_hv_bus.get_word((i+3)*32, 32) >> 16) & 0xFF,
              (a_hv_bus.get_word((i+3)*32, 32) >> 24) & 0xFF,
              (a_hv_bus.get_word((i+4)*32, 32) >>  0) & 0xFF,
              (a_hv_bus.get_word((i+4)*32, 32) >>  8) & 0xFF,
              (a_hv_bus.get_word((i+4)*32, 32) >> 16) & 0xFF,
              (a_hv_bus.get_word((i+4)*32, 32) >> 24) & 0xFF,
              (a_hv_bus.get_word((i+5)*32, 32) >>  0) & 0xFF,
              (a_hv_bus.get_word((i+5)*32, 32) >>  8) & 0xFF,
              (a_hv_bus.get_word((i+5)*32, 32) >> 16) & 0xFF,
              (a_hv_bus.get_word((i+5)*32, 32) >> 24) & 0xFF,
              (a_hv_bus.get_word((i+6)*32, 32) >>  0) & 0xFF,
              (a_hv_bus.get_word((i+6)*32, 32) >>  8) & 0xFF,
              (a_hv_bus.get_word((i+6)*32, 32) >> 16) & 0xFF,
              (a_hv_bus.get_word((i+6)*32, 32) >> 24) & 0xFF,
              (a_hv_bus.get_word((i+7)*32, 32) >>  0) & 0xFF,
              (a_hv_bus.get_word((i+7)*32, 32) >>  8) & 0xFF,
              (a_hv_bus.get_word((i+7)*32, 32) >> 16) & 0xFF,
              (a_hv_bus.get_word((i+7)*32, 32) >> 24) & 0xFF);
    }
  }


  // used only by test_actions
  void Mau::process_for_tests(Phv *phv, const Eop &eop) {
    // this does roughly what the old process_actions_stats_meters did
    MauExecuteState state(nullptr,nullptr,-1,-1);
    state.action_phv_ = phv;
    state.at_eop_ = true;
    state.op_ = kStateOpHandleEop;
    state.eop_ = eop;
    // Don't bother with locking here - only used by tests
    for (auto step : at_eop_steps_) step->execute(&state);
    // State may contain evictions that need processing
    mau_addr_dist_.handle_evictions(state.evict_info_);
  }

  // Handle EOP - push deferred stats/meter addresses
  int Mau::handle_eop(const Eop &eop) {
    RMT_LOG(RmtDebug::info(),
            "MAU::handle_eop ingress eopnum=%d   egress eopnum=%d\n",
            eop.ingress_eopnum(),eop.egress_eopnum());

    int ret = 0;
    ABORT_IF_GLOBAL_ERROR; // Will abort if this func recalled post assert throw

    // Now we can call physical row processing to
    // handle deferred stats/meter addresses
    if (eop.valid()) {

      // Check time moves forward
      if (eop.is_before(time_info_)) {
        RMT_LOG(RmtDebug::warn(), "MAU::handle_eop "
                "Time gone backwards! T_mau=%" PRIu64 " T_eop=%" PRIu64 "\n",
                time_info_.get_relative_time(), eop.get_relative_time());
      }
      else time_info_.set_from(eop.get_time_info());

      MauExecuteState state(nullptr,nullptr,-1,-1);
      state.action_phv_ = NULL;
      state.at_eop_ = true;
      state.eop_ = eop;
      state.op_ = kStateOpHandleEop;

      //printf("EOP\n"); fflush(stdout);
      lock_resources();
      try {
        for (auto step : at_eop_steps_) step->execute(&state);
      } catch (const std::runtime_error&) {
        // DV_MODE and RMT_ASSERT() or THROW_ERROR() called.
        // Return -1 so caller sees we had a serious error
        ret = -1;
      }
      mau_info_incr(MAU_N_EOPS);

      // Sanity check that all distibuted addresses have been used somewhere
      // XXX: do this before releasing lock
      if (!all_addrs_consumed()) mau_addr_dist()->addrs_leftover_check();

      unlock_resources();

      // State may contain evictions that need processing
      mau_addr_dist_.handle_evictions(state.evict_info_);
    }
    return ret;
  }

  // Handle TEOP - consume stats/meter addresses and
  // true pktlen/errors off TEOP bus
  int Mau::handle_dp_teop(const Teop &teop) {
    if (!teop.in_use()) return 0;

    RMT_LOG(RmtDebug::info(), "MAU::handle_dp_teop\n");
    int ret = 0;
    ABORT_IF_GLOBAL_ERROR; // Will abort if this func recalled post assert throw

    // Check time moves forward
    if (teop.is_before(time_info_)) {
      RMT_LOG(RmtDebug::warn(), "MAU::handle_dp_teop "
              "Time gone backwards! T_mau=%" PRIu64 " T_teop=%" PRIu64 "\n",
              time_info_.get_relative_time(), teop.get_relative_time());
    }
    else time_info_.set_from(teop.get_time_info());

    MauExecuteState state(nullptr,nullptr,-1,-1);
    state.action_phv_ = NULL;
    state.at_teop_ = true;
    state.teop_ = teop;
    state.op_ = kStateOpHandleTeop;

    lock_resources();
    try {
      for (auto step : at_teop_steps_) step->execute(&state);
    } catch (const std::runtime_error&) {
      // DV_MODE and RMT_ASSERT() or THROW_ERROR() called.
      // Return -1 so caller sees we had a serious error
      ret = -1;
    }
    mau_info_incr(MAU_N_TEOPS);
    unlock_resources();
    return ret;
  }
  int Mau::handle_dp_teop(Teop *teop) {
    RMT_ASSERT(teop != NULL);
    return handle_dp_teop(*teop);
  }


  // Handle PBUS read
  int Mau::pbus_read(uint8_t addrtype, int logical_table, uint32_t addr,
                     uint64_t* data0, uint64_t* data1,
                     bool clear, bool lock, uint64_t T) {

    // Addrop here is FINAL back-end op (eg in meter case op4 NOT op3:pfe)
    int addrop = (clear) ?Address::kAddrOpCfgRdClr :Address::kAddrOpCfgRd;
    MauExecuteState state(nullptr,nullptr,-1,-1);
    state.data_.fill_all_zeros();
    state.addr_ = Address::addr_make(addrtype, addr, addrop);
    state.addrtype_ = addrtype;
    state.logical_table_ = static_cast<uint8_t>(logical_table & 0xFF);
    state.op_ = StateOp::kStateOpPbusRd;
    state.relative_time_ = T;
    state.ret_ = 0; // If read succeeds this will be set to 1

    // printf("PBUS_READ<lt=%d,addr=0x%08x>\n", logical_table, addr); fflush(stdout);
    auto steps = (clear) ?pbus_wr_steps_ :pbus_rd_steps_;
    if (lock) lock_resources(); else reset_resources();
    for (auto step : steps) step->execute(&state);
    if (lock) unlock_resources();

    if (data0 != NULL) *data0 = state.data_.get_word(0);
    if (data1 != NULL) *data1 = state.data_.get_word(64);
    return static_cast<int>(state.ret_);
  }

  // Handle PBUS write OP - allows arbitrary addr with arbitrary OP
  int Mau::pbus_write_op(uint8_t addrtype, int logical_table, uint32_t addr,
                         uint64_t data0, uint64_t data1, bool lock, uint64_t T) {

    MauExecuteState state(nullptr,nullptr,-1,-1);
    state.data_.set_word(data0, 0);
    state.data_.set_word(data1, 64);
    state.addr_ = addr;
    state.addrtype_ = addrtype;
    state.logical_table_ = static_cast<uint8_t>(logical_table & 0xFF);
    state.op_ = StateOp::kStateOpPbusWr;
    state.relative_time_ = T;
    state.ret_ = 0;

    // printf("PBUS_WRITE_OP<lt=%d,addr=0x%08x>\n", logical_table, addr); fflush(stdout);
    if (lock) lock_resources(); else reset_resources();
    for (auto step : pbus_wr_steps_) step->execute(&state);
    if (lock) unlock_resources();
    return static_cast<int>(state.ret_);
  }
  // Handle PBUS write - calls pbus_write_op with OpCfgWr in addr
  int Mau::pbus_write(uint8_t addrtype, int logical_table, uint32_t addr,
                      uint64_t data0, uint64_t data1, bool lock, uint64_t T) {
    addr = Address::addr_make(addrtype, addr, Address::kAddrOpCfgWr);
    return pbus_write_op(addrtype, logical_table, addr, data0, data1, lock, T);
  }
  // Handle PBUS read/write - ONLY EVER used to copy data during move ops
  int Mau::pbus_read_write(uint8_t addrtype, int logical_table,
                           uint32_t addrRd, uint32_t addrWr, uint8_t flags, uint64_t T) {

    // Addrops here are FINAL back-end ops (eg in meter case op4 NOT op3:pfe)
    MauExecuteState state(nullptr,nullptr,-1,-1);
    state.data_.fill_all_zeros();
    state.addr_ = Address::addr_make(addrtype, addrRd, Address::kAddrOpCfgRd);
    state.addrtype_ = addrtype;
    state.logical_table_ = static_cast<uint8_t>(logical_table & 0xFF);
    state.op_ = StateOp::kStateOpPbusRd;
    state.relative_time_ = T;
    state.flags_ = flags;
    state.rw_raddr_ = state.addr_;
    state.ret_ = 0; // If read succeeds this will be set to 1

    reset_backend(); // Ignore flags for now
    for (auto step : pbus_rd_steps_) step->execute(&state);

    if (state.ret_ == 1) {
      // Leave state.rw_raddr_ alone
      state.addr_ = Address::addr_make(addrtype, addrWr, Address::kAddrOpCfgWr);
      state.op_ = StateOp::kStateOpPbusWr;
      state.ret_ = 0;

      reset_backend();
      for (auto step : pbus_wr_steps_) step->execute(&state);
    }

    reset_backend();
    return static_cast<int>(state.ret_);
  }

  // Handle PBUS sweep
  int Mau::do_sweep(uint8_t addrtype, int logical_table, uint32_t addr,
                    bool lock, uint64_t relativeT, uint64_t meterTickT) {
    MauExecuteState state(nullptr,nullptr,-1,-1);
    // Here we expect input addr to be already setup with correct OP4
    state.addr_ = addr;
    state.addrtype_ = addrtype;
    state.logical_table_ = static_cast<uint8_t>(logical_table & 0xFF);
    state.op_ = StateOp::kStateOpSweep;
    state.relative_time_ = relativeT;
    state.meter_tick_time_ = meterTickT;
    state.ret_ = 1; // 1 => true => idle

    //printf("DO_SWEEP<lt=%d,addr=0x%08x>\n", logical_table, addr); fflush(stdout);
    if (lock) lock_resources(); else reset_resources();
    for (auto step : sweep_steps_) step->execute(&state);
    if (lock) unlock_resources();

    return static_cast<int>(state.ret_);
  }

  // Handle Stateful Clear
  int Mau::do_stateful_clear(int logical_table, uint64_t T) {
    // Must be called with lock held
    MauExecuteState state(nullptr,nullptr,-1,-1);
    state.addr_ = 0u;
    state.addrtype_ = AddrType::kMeter;
    state.logical_table_ = static_cast<uint8_t>(logical_table & 0xFF);
    state.op_ = StateOp::kStateOpStatefulClear;
    state.relative_time_ = T;
    state.ret_ = 0;
    for (auto step : stateful_clear_steps_) step->execute(&state);
    return static_cast<int>(state.ret_);
  }

  // Check data_oflo_rows_
  // Should be no more than 6 contiguous rows so no more than 5 overflows
  void Mau::check_data_oflo_rows() {
    static_assert( (kMaxConsecDataOverflows < kSramRows),
		   "Max consec data overflows should be less than number sram rows");
    static_assert( (kMaxConsecDataOverflows < 8),
		   "Max consec data overflows should be less 8");
    uint8_t search = 0xFF >> (8-kMaxConsecDataOverflows);
    for (int i = 0; i <= 8-kMaxConsecDataOverflows; i++) {
      if ((data_oflo_rows_ & (search<<i)) == (search<<i)) {
	RMT_LOG(RmtDebug::error(MauSramRowReg::kRelaxOfloRdMuxCheck),
		"check_data_oflo_rows: Too many data oflo rows (0x%02x)\n",
		data_oflo_rows_);
	if (!MauSramRowReg::kRelaxOfloRdMuxCheck) { THROW_ERROR(-2); }
	break;
      }
    }
  }


  // Functions to create tables/tcams/srams etc


  MauLogicalTable *Mau::logical_table_get(int tableIndex) {
    RMT_ASSERT ((tableIndex >= 0) && (tableIndex < kLogicalTables));
    if ((tableIndex < 0) || (tableIndex >= kLogicalTables)) return NULL;
    MauLogicalTable *table = logical_table_lookup(tableIndex);
    if (table == NULL) {
      table = new MauLogicalTable(get_object_manager(),
                                  pipe_index(), mau_index(), tableIndex, this);
      logical_table_set(tableIndex, table);
      table->set_mau(this);
    }
    return table;
  }


  MauLogicalRow *Mau::logical_row_get(int logicalRowIndex) {
    RMT_ASSERT ((logicalRowIndex >= 0) && (logicalRowIndex < kLogicalRows));
    if ((logicalRowIndex < 0) || (logicalRowIndex >= kLogicalRows)) return NULL;
    MauLogicalRow *logrow = logical_row_lookup(logicalRowIndex);
    if (logrow == NULL) {
      int physrowIndex = physical_row_index(logicalRowIndex);
      logrow = new MauLogicalRow(get_object_manager(),
                                 pipe_index(), mau_index(), logicalRowIndex, this,
                                 physrowIndex, physical_row_which(logicalRowIndex));
      logical_row_set(logicalRowIndex, logrow);
      logrow->set_mau(this);
      // Link this logical row to the physical row it is part of
      MauSramRow *physrow = sram_row_lookup(physrowIndex);
      if (physrow != NULL) {
        logrow->set_physical_row(physrow);
        if (logicalRowIndex == logical_row_index_left(physrowIndex)) {
          physrow->set_logrow_left(logrow);
        } else if (logicalRowIndex == logical_row_index_right(physrowIndex)) {
          physrow->set_logrow_right(logrow);
        } else {
          RMT_ASSERT(0);
        }
      }
    }
    return logrow;
  }

  MauSramColumn *Mau::sram_column_get(int columnIndex) {
    RMT_ASSERT ((columnIndex >= 0) && (columnIndex < kSramColumns));
    if ((columnIndex < 0) || (columnIndex >= kSramColumns)) return NULL;
    MauSramColumn *column = sram_column_lookup(columnIndex);
    if (column == NULL) {
      column = new MauSramColumn(get_object_manager(),
                                 pipe_index(), mau_index(), columnIndex, this);
      sram_column_set(columnIndex, column);
      column->set_mau(this);
    }
    return column;
  }
  MauSramRow *Mau::sram_row_get(int rowIndex) {
    RMT_ASSERT ((rowIndex >= 0) && (rowIndex < kSramRows));
    if ((rowIndex < 0) || (rowIndex >= kSramRows)) return NULL;
    MauSramRow *row = sram_row_lookup(rowIndex);
    if (row == NULL) {
      row = new MauSramRow(get_object_manager(),
                           pipe_index(), mau_index(), rowIndex, this, mau_input());
      sram_row_set(rowIndex, row);
      row->set_mau(this);
      // Link this row to physical rows above/below if they exist
      if (rowIndex < kSramRows-1) {
        // rows increase from row 0 at bottom to row 7 at top
        // so rowAbove is numerically greater by 1
        MauSramRow *rowAbove = sram_row_lookup(rowIndex+1);
        if (rowAbove != NULL) {
          rowAbove->set_row_below(row);
          row->set_row_above(rowAbove);
        }
      }
      if (rowIndex > 0) {
        // rowBelow numerically less by 1
        MauSramRow *rowBelow = sram_row_lookup(rowIndex-1);
        if (rowBelow != NULL) {
          rowBelow->set_row_above(row);
          row->set_row_below(rowBelow);
        }
      }
      // Now link this row to left/right logical row objects
      MauLogicalRow *logrowLeft = logical_row_lookup(logical_row_index_left(rowIndex));
      if (logrowLeft != NULL) {
        row->set_logrow_left(logrowLeft);
        logrowLeft->set_physical_row(row);
      }
      MauLogicalRow *logrowRight = logical_row_lookup(logical_row_index_right(rowIndex));
      if (logrowRight != NULL) {
        row->set_logrow_right(logrowRight);
        logrowRight->set_physical_row(row);
      }

    }
    return row;
  }
  MauSram *Mau::sram_get(int rowIndex, int colIndex) {
    RMT_ASSERT ((rowIndex >= 0) && (rowIndex < kSramRows));
    RMT_ASSERT ((colIndex >= 0) && (colIndex < kSramColumns));
    if ((rowIndex < 0) || (rowIndex >= kSramRows)) return NULL;
    if ((colIndex < 0) || (colIndex >= kSramColumns)) return NULL;
    if ((kSramValidColumnMask & (1<<colIndex)) == 0) return NULL;
    MauSram *sram = sram_lookup(rowIndex, colIndex);
    if (sram == NULL) {
      sram = new MauSram(get_object_manager(),
                         pipe_index(), mau_index(), rowIndex, colIndex,
                         sram_array_index(rowIndex, colIndex), this);
      sram_set(rowIndex, colIndex, sram);
      // Fill in ptrs to row/col
      MauLogicalRow *logrow = logical_row_get(logical_row_index(rowIndex, colIndex));
      MauSramRow *row = sram_row_get(rowIndex);
      MauSramColumn *col = sram_column_get(colIndex);
      sram->set_logical_row(logrow);
      sram->set_row(row);
      sram->set_column(col);
      logrow->sram_set(logical_column_index(rowIndex, colIndex), sram);
      row->sram_set(colIndex, sram);
      col->sram_set(rowIndex, sram);
      MauMapram *mapram = mapram_lookup(rowIndex, colIndex);
      if (mapram != NULL) {
        // Link up to mapram if it exists
        sram->set_mapram(mapram);
        mapram->set_sram(sram);
      }
      sram->set_mau(this);
    }
    return sram;
  }


  MauMapram *Mau::mapram_get(int rowIndex, int colIndex) {
    RMT_ASSERT ((rowIndex >= 0) && (rowIndex < kMapramRows));
    RMT_ASSERT ((colIndex >= 0) && (colIndex < kMapramColumns));
    if ((rowIndex < 0) || (rowIndex >= kMapramRows)) return NULL;
    if ((colIndex < 0) || (colIndex >= kMapramColumns)) return NULL;
    // No maprams instantiated for cols 0-3 since regs_12544_mau_dev
    // No maprams instantiated for cols 4-5 since regs_13957_mau_dev
    if ((kMapramValidColumnMask & (1<<colIndex)) == 0) return NULL;
    MauMapram *mapram = mapram_lookup(rowIndex, colIndex);
    if (mapram == NULL) {
      mapram = new MauMapram(get_object_manager(),
                         pipe_index(), mau_index(), rowIndex, colIndex,
                         mapram_array_index(rowIndex, colIndex), this);
      mapram_set(rowIndex, colIndex, mapram);
      // Fill in ptrs to row/col
      MauLogicalRow *logrow = logical_row_get(logical_row_index(rowIndex, colIndex));
      mapram->set_logical_row(logrow);
      MauSram *sram = sram_lookup(rowIndex, colIndex);
      if (sram != NULL) {
        // Link up to sram if it exists
        mapram->set_sram(sram);
        sram->set_mapram(mapram);
      }
      mapram->set_mau(this);
    }
    return mapram;
  }


  MauLogicalTcam *Mau::logical_tcam_get(int ltcamIndex) {
    RMT_ASSERT ((ltcamIndex >= 0) && (ltcamIndex < kLogicalTcams));
    if ((ltcamIndex < 0) || (ltcamIndex >= kLogicalTcams)) return NULL;
    MauLogicalTcam *ltcam = logical_tcam_lookup(ltcamIndex);
    if (ltcam == NULL) {
      ltcam = new MauLogicalTcam(get_object_manager(),
                                 pipe_index(), mau_index(), ltcamIndex, this);
      logical_tcam_set(ltcamIndex, ltcam);
      ltcam->set_mau(this);
    }
    return ltcam;
  }

  MauTcamRow *Mau::tcam_row_get(int rowIndex) {
    RMT_ASSERT ((rowIndex >= 0) && (rowIndex < kTcamRows));
    RMT_ASSERT((rowIndex % 2) == 0); // Check we only ever create *even* rows
    if ((rowIndex < 0) || (rowIndex >= kTcamRows)) return NULL;
    MauTcamRow *row = tcam_row_lookup(rowIndex);
    if (row == NULL) {
      row = new MauTcamRow(get_object_manager(),
                           pipe_index(), mau_index(), rowIndex, this, mau_input());
      tcam_row_set(rowIndex, row);
      row->set_mau(this);
    }
    return row;
  }
  MauTcam *Mau::tcam_get(int rowIndex, int colIndex) {
    RMT_ASSERT ((rowIndex >= 0) && (rowIndex < kTcamRows));
    RMT_ASSERT ((colIndex >= 0) && (colIndex < kTcamColumns));
    if ((rowIndex < 0) || (rowIndex >= kTcamRows)) return NULL;
    if ((colIndex < 0) || (colIndex >= kTcamColumns)) return NULL;
    MauTcam *tcam = tcam_lookup(rowIndex, colIndex);
    if (tcam == NULL) {
      tcam = new MauTcam(get_object_manager(),
                         pipe_index(), mau_index(), rowIndex, colIndex,
                         tcam_array_index(rowIndex,colIndex), this);
      tcam_set(rowIndex, colIndex, tcam);
      // Fill in ptrs to row
      // !!!!!!!!!! NOTE !!!!!!!!!!
      // We only ever create *even* MauTcamRows - 2 rows worth of MauTcams
      // end up sharing a single MauTcamRow obj - this is because there are
      // half as many TcamRowXBars as TCAM rows.
      // Look at the Fig 6.28 in MAU  uArch doc.
      // The MauTcam object is set with a pointer to the correct *even* row.
      // All odd rows just stay NULL.
      //
      int evenRowIndex = (((rowIndex % 2) == 1) ?(rowIndex-1) :rowIndex);
      MauTcamRow *row = tcam_row_get(evenRowIndex);
      tcam->set_row(row);
      tcam->set_mau(this);
    }
    return tcam;
  }

  // Called from MauTcamReg when tcam_mode changes
  // Lets us track whether ingress/egress uses tcams
  void Mau::tcam_config_changed() {
    uint32_t i_ftrs = 0u, e_ftrs = 0u;
    for (int col = 0; col < kTcamColumns; col++) {
      for (int row = 0; row < kTcamRows; row++) {
        MauTcam *tcam = tcam_get(row, col);
        if (tcam != NULL) {
          if      (tcam->is_ingress()) i_ftrs |= MauDefs::kMauTcamPresent;
          else if (tcam->is_ghost())   i_ftrs |= MauDefs::kMauTcamPresent;
          else if (tcam->is_egress())  e_ftrs |= MauDefs::kMauTcamPresent;
        }
      }
    }
    set_tcam_dynamic_features(i_ftrs, e_ftrs);
  }


  MauIO *Mau::mau_io_input() {
    return get_object_manager()->mau_io_get(pipe_index(), mau_index());
  }
  MauIO *Mau::mau_io_output() {
    return get_object_manager()->mau_io_get(pipe_index(), 1+mau_index());
  }


  void Mau::mau_init_srams() {
    for (int col = 0; col < kSramColumns; col++) {
      for (int row = 0; row < kSramRows; row++) {
        (void)sram_get(row, col);
      }
    }
    for (int col = 0; col < kMapramColumns; col++) {
      for (int row = 0; row < kMapramRows; row++) {
        (void)mapram_get(row, col);
      }
    }
  }
  void Mau::mau_init_tcams() {
    for (int col = 0; col < kTcamColumns; col++) {
      for (int row = 0; row < kTcamRows; row++) {
        (void)tcam_get(row, col);
      }
    }
  }
  void Mau::mau_init_tables() {
    for (int tab = 0; tab < kLogicalTables; tab++) {
      (void)logical_table_get(tab);
    }
    for (int ltcam = 0; ltcam < kLogicalTcams; ltcam++) {
      (void)logical_tcam_get(ltcam);
    }
  }
  void Mau::mau_init_all() {
    mau_init_srams();
    mau_init_tcams();
    mau_init_tables();
  }


  void Mau::lock_resources() {
    if (kMauUseMutex) mau_lock_.lock();
    reset_resources();
  }
  void Mau::unlock_resources() {
    if (kMauUseMutex) mau_lock_.unlock();
  }
  void Mau::reset_backend() {
    for (int logrow = 0; logrow < kLogicalRows; logrow++) {
      MauLogicalRow *logicalRow = logical_row_lookup(logrow);
      if (logicalRow != NULL) logicalRow->reset_resources();
    }
    mau_addr_dist_.reset_resources();
    inval_output_selector_alus_ = 0;
  }
  void Mau::reset_resources() {
    for (int scol = 0; scol < kSramColumns; scol++) {
      MauSramColumn *sramColumn = sram_column_lookup(scol);
      if (sramColumn != NULL) sramColumn->reset_resources();
    }
    for (int srow = 0; srow < kSramRows; srow++) {
      MauSramRow *sramRow = sram_row_lookup(srow);
      if (sramRow != NULL) sramRow->reset_resources();
    }
    for (int scol = 0; scol < kSramColumns; scol++) {
      for (int srow = 0; srow < kSramRows; srow++) {
        MauSram *sram = sram_lookup(srow,scol);
        if (sram != NULL) sram->reset_resources();
      }
    }
    for (int mcol = 0; mcol < kMapramColumns; mcol++) {
      for (int mrow = 0; mrow < kMapramRows; mrow++) {
        MauMapram *mapram = mapram_lookup(mrow,mcol);
        if (mapram != NULL) mapram->reset_resources();
      }
    }
    for (int ltcam = 0; ltcam < kLogicalTcams; ltcam++) {
      MauLogicalTcam *logicalTcam = logical_tcam_lookup(ltcam);
      if (logicalTcam != NULL) logicalTcam->reset_resources();
    }
    for (int trow = 0; trow < kTcamRows; trow++) {
      MauTcamRow *tcamRow = tcam_row_lookup(trow);
      if (tcamRow != NULL) tcamRow->reset_resources();
    }
    for (int tab = 0; tab < kLogicalTables; tab++) {
      MauLogicalTable *logicalTable = logical_table_lookup(tab);
      if (logicalTable != NULL) logicalTable->reset_resources();
      lookup_results_[tab].invalidate();
    }
    mau_snapshot_.reset_resources();
    mau_stateful_counters_.reset_resources();

    powered_ltcams_ = 0; // Reset ltcam tracking var

    // XXX
    // Don't reset output now as fiddles with next MAU
    // which isn't locked.
    // Special case this within logic that uses MauIO
    // resources (eg Snapshot/Predication)
    //mau_io_output()->reset();

    action_hv_output_bus_.fill_all_zeros();
    // Reset 'backend' ie - MauAddrDist & MauLogicalRows
    reset_backend();
    // Do some checks
    check_data_oflo_rows();
  }
  void Mau::flush_color_queues() {
    for (int alu = 0; alu < kNumMeterAlus; alu++) {
      mau_addr_dist_.flush_queued_color_writes(alu);
    }
  }
  void Mau::flush_queues() {
    flush_color_queues();
  }

  bool Mau::evaluate_all(int tableIndex) {
    if ((tableIndex < 0) || (tableIndex >= kLogicalTables)) return false;
    MauLogicalTable *table = logical_table_lookup(tableIndex);
    return (table != NULL) ?table->evaluate_all() :false;
  }
  void Mau::set_evaluate_all(int tableIndex, bool tf) {
    if ((tableIndex < 0) || (tableIndex >= kLogicalTables)) return;
    MauLogicalTable *table = logical_table_lookup(tableIndex);
    if (table != NULL) table->set_evaluate_all(tf);
  }
  bool Mau::evaluate_all() {
    return evaluate_all_;
  }
  void Mau::set_evaluate_all(bool tf) {
    for (int tableIndex = 0; tableIndex < kLogicalTables; tableIndex++)
      set_evaluate_all(tableIndex, tf);
    evaluate_all_ = tf;
  }




  // Just for unit testing cleanup
  void Mau::mau_free_all() {
    for (int tab = 0; tab < kLogicalTables; tab++) {
      MauLogicalTable *table = logical_table_lookup(tab);
      if (table != NULL) delete table;
      logical_table_set(tab, NULL);
    }
    for (int ltc = 0; ltc < kLogicalTcams; ltc++) {
      MauLogicalTcam *ltcam = logical_tcam_lookup(ltc);
      if (ltcam != NULL) delete ltcam;
      logical_tcam_set(ltc, NULL);
    }
    for (int trow = 0; trow < kTcamRows; trow++) {
      MauTcamRow *row = tcam_row_lookup(trow);
      if (row != NULL) delete row;
      tcam_row_set(trow, NULL);
    }
    for (int col = 0; col < kTcamColumns; col++) {
      for (int row = 0; row < kTcamRows; row++) {
        MauTcam *tcam = tcam_lookup(row,col);
        if (tcam != NULL) delete tcam;
        tcam_set(row,col, NULL);
      }
    }
    for (int lrow = 0; lrow < kLogicalRows; lrow++) {
      MauLogicalRow *logrow = logical_row_lookup(lrow);
      if (logrow != NULL) delete logrow;
      logical_row_set(lrow, NULL);
    }
    for (int srow = 0; srow < kSramRows; srow++) {
      MauSramRow *row = sram_row_lookup(srow);
      if (row != NULL) delete row;
      sram_row_set(srow, NULL);
    }
    for (int scol = 0; scol < kSramColumns; scol++) {
      MauSramColumn *col = sram_column_lookup(scol);
      if (col != NULL) delete col;
      sram_column_set(scol, NULL);
    }
    for (int col = 0; col < kMapramColumns; col++) {
      for (int row = 0; row < kMapramRows; row++) {
        MauMapram *mapram = mapram_lookup(row,col);
        if (mapram != NULL) delete mapram;
        mapram_set(row,col, NULL);
      }
    }
    for (int col = 0; col < kSramColumns; col++) {
      for (int row = 0; row < kSramRows; row++) {
        MauSram *sram = sram_lookup(row,col);
        if (sram != NULL) delete sram;
        sram_set(row,col, NULL);
      }
    }
  }

BitVector<Mau::kLogicalTables> Mau::table_active() {
  BitVector<kLogicalTables> r{};
  r.set_word(static_cast<uint64_t>(pred_lts_active()), 0, kLogicalTables);
  return r;
}

BitVector<Mau::kHashOutputWidth> Mau::get_hash_output(Phv *phv, int group) {
  BitVector<kExactMatchInputBits> input_bits;
  BitVector<kExactMatchValidBits> valid_bits;
  BitVector<kHashOutputWidth>  hash;

  get_exact_match_input(phv, &input_bits, &valid_bits);

  auto mau_hash_generator = mau_input_.get_hash_generator();
  RMT_ASSERT (mau_hash_generator);
  mau_hash_generator->CalculateOutput(input_bits,
                                      valid_bits,
                                      group,
                                      phv->cache_id(),
                                      &hash);
  return hash;
}

void Mau::get_color_bus( int which_bus, uint8_t *output, bool report_error_if_not_driven ) {
  constexpr int n_busses = static_cast<int>(MauDefs::kNumMeterAlus);
  RMT_ASSERT( which_bus < n_busses ); // there is one color bus per ALU

  // first half of busses come from bottom half
  int start_row = which_bus < (n_busses/2) ? 0 : (kSramRows/2);
  int end_row   = start_row + (kSramRows/2) - 1;

  bool was_driven = false;

  // Or in the outputs from all the rows in the relevant half
  int which_half_bus = which_bus % (n_busses/2);
  for (int row=start_row; row <= end_row; ++row) {
    sram_rows_[row]->get_color_bus(  which_half_bus, output, &was_driven );
  }
  if ( which_bus >= (n_busses/2) ) {
    // check if this color bus is configured to have overflow ored in
    //  (the bottom 2 busses will never return true)
    if ( mau_result_bus_.get_color_read_oflo_enable( which_bus ) ) {
      // or in all the overflow busses from the bottom half (doesn't overflow from top)
      for (int row=0; row <= (kSramRows/2) - 1; ++row) {
        sram_rows_[row]->get_color_bus(  MauColorSwitchbox::kOverflowIndex , output, &was_driven );
      }
    }

  }
  if ( !was_driven && report_error_if_not_driven ) {
    RMT_LOG(RmtDebug::error(), "Mau::get_color_bus: Nothing drove bus %d\n",which_bus);
  }


}

//adding definition for public method that calls PhvModification method
int Mau::set_phv_modification(model_core::RmtPhvModification::ModifyEnum which, 
                              model_core::RmtPhvModification::ActionEnum action, 
                               int index, uint32_t value){
  switch(which){
    case model_core::RmtPhvModification::ModifyEnum::kMatch:
      return modify_match_.set_modification(action, index, value);
    case model_core::RmtPhvModification::ModifyEnum::kAction:
      return modify_action_.set_modification(action, index, value);
    case model_core::RmtPhvModification::ModifyEnum::kOutput:
      return modify_output_.set_modification(action, index, value);
    case model_core::RmtPhvModification::ModifyEnum::kErr:
      break;
  }
  return -4;
}

}
