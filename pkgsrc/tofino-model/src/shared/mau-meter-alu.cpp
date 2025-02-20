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
#include <mau-meter-alu.h>
#include <phv.h>
#include <address.h>
#include <register_adapters.h>

namespace MODEL_CHIP_NAMESPACE {

  const char *MauMeterAlu::kOp4Strings[] = {
    "NOP", "Color0/Salu0", "CfgRd", "Color1/Salu1", "CfgWr",
    "Color2/Salu2", "Sweep/SaluClr", "Color3/Salu3",
    "MoveregRd", "CfgSalu0", "MoveregWr", "CfgSalu1",
    "CfgSalu2", "CfgSalu3", "RSVD", "Selector"
  };

  MauMeterAlu::MauMeterAlu(RmtObjectManager *om,
                           int pipeIndex, int mauIndex, int logicalRowIndex,
                           Mau *mau, MauLogicalRow *mau_log_row,
                           int physicalRowIndex, int physicalRowWhich)
      : MauObject(om, pipeIndex, mauIndex, kType, logicalRowIndex, mau),
        mau_log_row_(mau_log_row),  mau_memory_(mau->mau_memory()),
        meter_(om, pipeIndex, mauIndex, logicalRowIndex,mau),
        lpf_meter_(om, pipeIndex, mauIndex, logicalRowIndex,mau),
        salu_(om, pipeIndex, mauIndex, logicalRowIndex,mau),
        logical_row_(logicalRowIndex), physical_row_which_(physicalRowWhich),
        mau_index_(mauIndex),
        alu_index_(get_meter_alu_regs_index(logicalRowIndex)), // 0 to 3
        meter_ctl_(default_adapter(meter_ctl_,chip_index(), pipeIndex, mauIndex, alu_index_,
                   [this](){this->ctl_write_callback(); })),
        red_value_ctl_(default_adapter(red_value_ctl_,chip_index(), pipeIndex, mauIndex, alu_index_)),
        stateful_ctl_(default_adapter(stateful_ctl_,chip_index(), pipeIndex, mauIndex, alu_index_,
                      [this](){this->ctl_write_callback(); })),
        selector_ctl_(default_adapter(selector_ctl_,chip_index(), pipeIndex, mauIndex, alu_index_,
                      [this](){this->ctl_write_callback(); })),
        meter_alu_thread_(default_adapter(meter_alu_thread_,chip_index(), pipeIndex, mauIndex)),
        meter_alu_group_action_ctl_(default_adapter(meter_alu_group_action_ctl_,chip_index(), pipeIndex, mauIndex, alu_index_)),
        meter_alu_group_phv_hash_mask_(default_adapter(meter_alu_group_phv_hash_mask_,chip_index(), pipeIndex, mauIndex,
                                                       [this](int a1,int a0){this->phv_hash_mask_write_callback(a1,a0); })),
        meter_alu_group_phv_hash_shift_(default_adapter(meter_alu_group_phv_hash_shift_,chip_index(), pipeIndex, mauIndex, alu_index_))
  {
    meter_ctl_.reset();
    red_value_ctl_.reset();
    stateful_ctl_.reset();
    selector_ctl_.reset();
    meter_alu_thread_.reset(); // Just for ingress/egress thread check
    meter_alu_group_action_ctl_.reset();
    meter_alu_group_phv_hash_mask_.reset();
    meter_alu_group_phv_hash_shift_.reset();
    ctor_finished_ = true;
  }
  MauMeterAlu::~MauMeterAlu() { }


  void MauMeterAlu::ctl_write_callback() {
    if (!ctor_finished_) return;
    uint32_t ftrs = 0u;
    // Keep copies of next 3 characteristics so we can spot change
    r_action_override_ = MauChipMeterAlu::get_right_action_override(meter_alu_group_action_ctl_);
    salu_uses_divide_ = salu_uses_divide();
    salu_uses_qlag_ = salu_uses_qlag(); // XXX: Infer stateful QLAG
    if (meter_ctl_.meter_enable())       ftrs |= MauDefs::kMauMeterAluMeterPresent;
    if (lpf_enabled_or_red_only_mode())  ftrs |= MauDefs::kMauMeterAluMeterLpfPresent;
    if (selector_ctl_.selector_enable()) ftrs |= MauDefs::kMauMeterAluSelectorPresent;
    if (stateful_ctl_.salu_enable())     ftrs |= MauDefs::kMauMeterAluStatefulPresent;
    if (salu_uses_divide_)               ftrs |= MauDefs::kMauMeterAluStatefulDivideUsed;
    if (salu_uses_qlag_)                 ftrs |= MauDefs::kMauMeterAluStatefulQlagUsed;
    if (r_action_override_)              ftrs |= MauDefs::kMauMeterAluRightActionOverrideUsed;
    bool egress = ((meter_ctl_.meter_alu_egress() & 0x1) == 0x1);
    uint32_t i_ftrs = (egress) ?0u :ftrs;
    uint32_t e_ftrs = (egress) ?ftrs :0u;
    mau()->set_meter_alu_dynamic_features(alu_index_, egress, i_ftrs, e_ftrs);
  }
  void MauMeterAlu::phv_hash_mask_write_callback(int a1, int a0) {
    static_assert( kStatefulMeterAluDataBits == 64 || kStatefulMeterAluDataBits == 128,
                   "kStatefulMeterAluDataBits must be 64 or 128");
    if (a1 == alu_index_) { // check if write to our alu
      if (kStatefulMeterAluDataBits == 128) {
        // update the correct word of the phv_hash_mask_ with the new value
        phv_hash_mask_.set32(a0, meter_alu_group_phv_hash_mask_.meter_alu_group_phv_hash_mask(a1,a0));
      }
      else {
        // case for Tofino dummy register, just set 64 bits
        if (a0<2) {
          phv_hash_mask_.set32(a0, meter_alu_group_phv_hash_mask_.meter_alu_group_phv_hash_mask(a1,a0));
        }
      }
    }
  }

  bool MauMeterAlu::salu_uses_qlag() {
    return (is_jbay_or_later() && stateful_ctl_.salu_enable() && !selector_ctl_.selector_enable() &&
            (mau_log_row_->consumes_sel_addr() || mau_log_row_->produces_sel_addr()));
  }
  bool MauMeterAlu::is_egress_alu() {
    bool egress = ((meter_ctl_.meter_alu_egress() & 0x1) == 0x1);
    if (kRelaxThreadCheck) return egress;
    uint8_t ing0 = meter_alu_thread_.meter_alu_thread_ingress(0);
    uint8_t egr0 = meter_alu_thread_.meter_alu_thread_egress(0);
    uint8_t ing1 = meter_alu_thread_.meter_alu_thread_ingress(1);
    uint8_t egr1 = meter_alu_thread_.meter_alu_thread_egress(1);
    // Thread registers should be replicated - check
    RMT_ASSERT((ing0 == ing1) && (egr0 == egr1));
    RMT_ASSERT((egr0 & (1<<alu_index_)) == ((egress) ?(1<<alu_index_) :0));
    RMT_ASSERT((ing0 & (1<<alu_index_)) == ((egress) ?0 :(1<<alu_index_)));
    return egress;
  }
  bool MauMeterAlu::is_ingress_alu() { return !is_egress_alu(); }

  // XXX: func to validate op passed to ALU
  bool MauMeterAlu::check_hdrtime_op4_ok(MauExecuteState *state, uint32_t valid_ops,
                                         uint32_t addr, int op4, const char *alu_type) {
    // XXX: only check ops at hdrtime; otherwise return ok
    if (state->op_ != StateOp::kStateOpPhvLookup) return true;
    bool ok = (((valid_ops >> op4) & 1) == 1);
    if (!ok) {
      RMT_LOG(RmtDebug::error(kRelaxOpCheck), "MeterOP %d (%s) NOT supported by %s (addr=0x%x)\n",
              op4, get_opstr(op4), alu_type, addr);
    }
    return ok;
  }

  int MauMeterAlu::get_packet_len(MauExecuteState *state) {
    if (state->at_eop_) {
      bool egress = is_egress_alu();
      if (!egress && state->eop_.ingress_valid()) return state->eop_.ingress_pktlen();
      if (egress  && state->eop_.egress_valid())  return state->eop_.egress_pktlen();
    } else if (state->at_teop_) {
      return state->teop_.byte_len();
    } else {
      RMT_ASSERT(0);
    }
    return 0;
  }


  bool MauMeterAlu::run_alu_internal(MauExecuteState *state, bool meter_sweep) {
    uint64_t present_time = state->get_meter_tick_time(mau_index_,alu_index_,is_ingress_alu());
    BitVector<kDataBusWidth> data_in(UINT64_C(0));
    BitVector<kDataBusWidth> data_out(UINT64_C(0));
    uint32_t addr = 0u;

    BitVector <kActionOutputBusWidth> action_data_out{};

    get_input(&data_in, &addr);

    if (!Address::meter_addr_op_enabled(addr)) return false;

    int op4 = Address::meter_addr_op4(addr);
    uint8_t color_out = 0;

    RMT_LOG(RmtDebug::verbose(),
            "MauMeterALU: %saddr=0x%x time=%" PRId64 " data_in=0x%016" PRIx64 "_%016" PRIx64 "\n",
            meter_sweep ? "SWEEP " : "",
            addr,present_time,data_in.get_word(64),data_in.get_word(0));

    // can't have more than one ALU enabled at a time.
    RMT_ASSERT (((meter_ctl_.meter_enable()?1:0) + (lpf_enabled_or_red_only_mode()?1:0) +
             (stateful_ctl_.salu_enable()?1:0)) <= 1 );

    // Keep overall total meter ALU invoctaion
    mau()->mau_info_incr(MAU_METER_ALU_TOTAL_INVOCATIONS);

    if ( meter_ctl_.meter_enable() ) {

      // XXX: check op valid for MeterALU
      // XXX: only check at hdrtime
      (void)check_hdrtime_op4_ok(state, MauDefs::kMeterAluHdrtimeMeterOps,
                                 addr, op4, "MeterALU");

      int adjust_value = signextend<signed int,
                                    kMeterBytecountAdjustWidth >(
                                        meter_ctl_.meter_bytecount_adjust());
      bool is_byte_counter = meter_ctl_.meter_byte();
      int decrement = 0;
      uint64_t random_number = 0;

      if (!meter_sweep) {
        RMT_ASSERT( state->at_eop_ || state->at_teop_ ); // check being called on (t)EOP
        if (is_byte_counter) {
          decrement = get_packet_len(state) + adjust_value;
          // The adjusted bytecount is set to zero underflow occurs after
          //   adding stats_bytecount_adjust
          if (decrement < 0) {
            decrement=0;
          }
        }
        else {
          decrement = 1;
        }
        random_number = state->get_meter_random_value(mau_index_,alu_index_,is_ingress_alu());
      }


      int color_in  = Address::meter_addr_get_color(addr);
      if (color_in == -1) {
        RMT_ASSERT( meter_sweep ); // Should get a valid color unless it is a meter sweep
        color_in = 0;
      }

      RMT_LOG(RmtDebug::verbose(),
              "MauMeterALU: Standard meter (%s) adjust_value=%d decrement=%d color_in=%d\n",
              meter_ctl_.meter_byte() ? "byte" : "packet",
              adjust_value,decrement,color_in);

      RMT_ASSERT ( Address::meter_addr_is_meter( addr ) || meter_sweep );
      mau()->mau_info_incr(MAU_METER_ALU_NORMAL_INVOCATIONS);

      uint64_t relative_time = 0;
      state->get_relative_time( &relative_time,is_ingress_alu());

      meter_.calculate_output(
          present_time,
          relative_time,
          addr,
          is_byte_counter,
          meter_ctl_.meter_rng_enable(),
          random_number,
          meter_ctl_.meter_time_scale(),
          meter_sweep,
          &data_in,
          color_in,
          decrement,
          MauChipMeterAlu::get_meter_lpf_sat_ctl( meter_ctl_ ),
          &data_out,
          &color_out);
      has_run_=true;
    }

    if ( lpf_enabled_or_red_only_mode() ) {

      // XXX: check op valid for MeterLpfALU
      // XXX: only check at hdrtime
      (void)check_hdrtime_op4_ok(state, MauDefs::kMeterAluHdrtimeLpfOps,
                                 addr, op4, "MeterLpfALU");

      uint32_t D = 0;
      uint64_t random_number = 0;
      if ( ! meter_sweep ) {
        RMT_ASSERT( !state->at_eop_ && !state->at_teop_ ); // check not being called on (t)EOP
        D = get_alu_data(state->match_phv_);
        random_number = state->get_meter_random_value(mau_index_,alu_index_,is_ingress_alu());
      }
      RMT_LOG(RmtDebug::verbose(),
              "MauMeterALU: LPF meter D=0x%x random_number=0x%" PRIx64 "\n",D,random_number);
      if (meter_ctl_.red_enable()) {
        mau()->mau_info_incr(MAU_METER_ALU_LPF_RED_INVOCATIONS);
        RMT_LOG(RmtDebug::verbose(),
                "MauMeterALU: RED enabled, nodrop=%d drop=%d\n",
                red_value_ctl_.red_nodrop_value(),
                red_value_ctl_.red_drop_value());
      }
      RMT_ASSERT ( Address::meter_addr_is_meter( addr ) || meter_sweep );
      mau()->mau_info_incr(MAU_METER_ALU_LPF_INVOCATIONS);


      uint64_t relative_time = 0;
      state->get_relative_time( &relative_time,is_ingress_alu());

      uint32_t action_data_out_32b = 0;

      lpf_meter_.calculate_output(present_time, relative_time, addr,
                                  meter_sweep,
                                  meter_ctl_.red_enable(),
                                  red_only_mode(), // JBay only
                                  red_value_ctl_.red_nodrop_value(),
                                  red_value_ctl_.red_drop_value(),
                                  D,
                                  random_number,  // from LFSR
                                  meter_ctl_.meter_time_scale(),
                                  &data_in,
                                  MauChipMeterAlu::get_meter_lpf_sat_ctl( meter_ctl_ ),
                                  &data_out,
                                  &action_data_out_32b
                                  );
      action_data_out.set32(0,action_data_out_32b);
      has_run_=true;
    }

    if ( stateful_ctl_.salu_enable() ) {

      // XXX: check op valid for StatefulALU (might allow Selector OPs too)
      uint32_t ok_ops = MauDefs::kMeterAluHdrtimeSaluOps;
      if (selector_ctl_.selector_enable()) ok_ops |= MauDefs::kMeterAluHdrtimeSelOps;
      // XXX: only check at hdrtime
      (void)check_hdrtime_op4_ok(state, ok_ops, addr, op4, "StatefulALU");

      // Fake a callback if SALU divider usage or SALU QLAG config changes
      if ((salu_uses_divide() != salu_uses_divide_) || (salu_uses_qlag() != salu_uses_qlag_))
        ctl_write_callback();

      if ( Address::meter_addr_is_stateful( addr ) ) {
        RMT_ASSERT( !state->at_eop_ && !state->at_teop_ ); // check not being called on (t)EOP
        RMT_ASSERT( !meter_sweep );
        auto D = get_alu_data_wide(state->match_phv_);

        // If addr comes from a run_stateful_instruction then
        // override present_time with relative_time value from state
        // OR if this is jbay ALWAYS use relative_time
        if (Address::meter_addr_is_run_stateful(addr) || is_jbay_or_later())
          state->get_relative_time(&present_time, is_ingress_alu());

        // mau()->mau_info_incr(MAU_METER_ALU_SALU_INVOCATIONS); // count salu in calculate_output() so it works in standalone
        uint32_t sel_index = 0u; // Only used by min/max OP
        salu_.calculate_output(addr, D, &data_in, &data_out, &action_data_out,
                               present_time, is_ingress_alu(),
                               state->match_bus_,state->learn_or_match_bus_,
                               &sel_index);

        // Push sel_index into selector switchbox (logical row code will splice
        // the index into the action_addr, or if it is invalid, replace the whole
        // action_addr with the homerow/oflo_fallback_action_addr).
        mau_log_row_->set_selector_rd_addr(sel_index);
        // Also tell MAU if sel_index is invalid so it can use fallback IMEM addr
        bool sel_index_invalid = (sel_index == MauDefs::kSelectorAluInvalOutput);
        mau()->set_selector_alu_output_invalid(alu_index_, sel_index_invalid);

        auto p4_salu_value_list = salu_.get_p4_log_results();
        for (auto p4_salu_value = p4_salu_value_list.begin(); p4_salu_value != p4_salu_value_list.end(); p4_salu_value++) {
          state->salu_log_valuelist_[mau_log_row_->physical_row_index()].push_back((*p4_salu_value));
        }
        has_run_=true;
      }
    }

    set_output_action(action_data_out);
    // Update color write data bus - NB color_out is inhibited for sweep
    mau_log_row_->set_color_write_data(&color_out);

    set_output_data(&data_out);

    RMT_LOG(RmtDebug::verbose(),
            "MauMeterALU: data_out=0x%016" PRIx64 "_%016" PRIx64 " color_out=%d "
            "action_data_out=0x%016" PRIx64 "_%016" PRIx64 "\n",
            data_out.get_word(64),data_out.get_word(0),static_cast<int>(color_out),
            action_data_out.get_word(64),action_data_out.get_word(0) );

    return true;
  }

  bool MauMeterAlu::run_alu_with_state(MauExecuteState *state) {

    bool action_overide = MauChipMeterAlu::get_right_action_override( meter_alu_group_action_ctl_ );
    // Fake a callback if right_action_override usage changes
    if (action_overide != r_action_override_) ctl_write_callback();

    if ( action_overide && state->match_phv_ ) {
      // JBay only
      RMT_ASSERT( !lpf_enabled_or_red_only_mode() && !stateful_ctl_.salu_enable() );
      // have to use set_from so that this compiles for Tofino where the widths differ
      BitVector<kActionOutputBusWidth> D{};
      D.set_from( 0, get_alu_data_wide(state->match_phv_) );
      set_output_action_nocheck( D );
    }

    if (!meter_ctl_.meter_enable() && !lpf_enabled_or_red_only_mode() &&
        !stateful_ctl_.salu_enable()) return false;

    BitVector<kDataBusWidth> data(UINT64_C(0));
    uint32_t addr = 0u;

    get_input(&data, &addr);

    if (!Address::meter_addr_op_enabled(addr)) return false;

    mau()->mau_addr_dist()->meter_synth2port_fabric_check(alu_index_,logical_row_);

    int op = Address::meter_addr_op4(addr);


    if (op == Address::kMeterOp4CfgRd) {
      // Maybe catch-up sweeps - if sweeps caught up get_input again
      if (catch_up_sweeps(state)) get_input(&data, &addr);

      state->data_.copy_from(data);
      state->ret_ = 1;
      RMT_LOG(RmtDebug::verbose(),
              "MauMeterALU: CfgRd addr=0x%x data=0x%016" PRIx64 "_%016" PRIx64 "\n",
              addr,data.get_word(64),data.get_word(0));

    } else if (op == Address::kMeterOp4CfgWr) {
      uint8_t color_out = MauMeter::kMeterColorInhibit; // NO color update by default
      bool doing_movereg = Address::meter_addr_op_enabled(state->rw_raddr_);
      if (stateful_ctl_.salu_enable()) {
        // STATEFUL so will be writing particular subword
        int wr_shift = Address::stateful_addr_get_shift(addr);
        int wr_width = Address::stateful_addr_get_width(addr, wr_shift);
        int wr_offset = Address::stateful_addr_get_offset(addr, wr_shift);
        int rd_width = wr_width;
        int rd_offset = wr_offset;
        if (doing_movereg) {
          // If movereg, valid read addr will be in state->rw_raddr_
          int rd_shift = Address::stateful_addr_get_shift(state->rw_raddr_);
          rd_width = Address::stateful_addr_get_width(state->rw_raddr_, rd_shift);
          rd_offset = Address::stateful_addr_get_offset(state->rw_raddr_, rd_shift);
          RMT_ASSERT(wr_width == rd_width); // Widths must match if movereg
        }
        if (64 < wr_width) {
          RMT_ASSERT(wr_width == 128);
          data.copy_from(state->data_);
        } else {
          uint64_t word = state->data_.get_word(rd_offset, rd_width);
          data.set_word(word, wr_offset, wr_width);
        }
        salu_.update_last_config_write_data( data );

        RMT_LOG(RmtDebug::verbose(),
                "MauMeterALU: CfgWr %snew_data=0x%016" PRIx64 "_%016" PRIx64 " rd_offset=%d wr_offset=%d\n",doing_movereg?"doing_movereg ":"",
                data.get_word(64),data.get_word(0), rd_offset, wr_offset);

      } else if (lpf_enabled_or_red_only_mode()) {
        // LPF so need to update VOld
        uint64_t T = UINT64_C(0);
        state->get_relative_time(&T, is_ingress_alu());
        lpf_meter_.update_cache_config_write(T, addr, &data, &(state->data_));
        data.copy_from(state->data_);
      } else {
        // Meter also needs to update cache (TODO: share code with LPF?)
        uint64_t T = UINT64_C(0);
        state->get_relative_time(&T, is_ingress_alu());
        meter_.update_cache_config_write(T, addr, &data, &(state->data_));
        // METER so write all 128 bits
        // Only extract and write color if doing movereg
        data.copy_from(state->data_);
        if (doing_movereg) meter_.get_curr_color(&data, &color_out);
      }
      mau_log_row_->set_color_write_data(&color_out);
      set_output_data(&data);

      // No catch-up sweep required on CfgWr - but update last_sweep_t
      update_sweep_time(state);

    } else if ((op == Address::kMeterOp4Sweep) &&
               ( lpf_enabled_or_red_only_mode() ||
                 meter_ctl_.meter_enable() ||
                 ! MauDefs::kMeterClearOveridesSweep ) ) {
      // On JBay meter clear and sweep use the same opcode, so
      //   we need to check the enables above to spot a sweep
      // On Tofino there is no clear, so kMeterClearOveridesSweep
      //  is false and the enables are not checked.

      // Maybe catch-up sweeps - run_alu_internal will get_input again
      (void)catch_up_sweeps(state);
      bool sweep_rtn = run_alu_internal(state, true /* meter sweep */);
      update_sweep_time(state);
      return sweep_rtn;

    } else {

      // Maybe catch-up sweeps - run_alu_internal will get_input again
      (void)catch_up_sweeps(state);
      bool run_rtn = run_alu_internal(state, false /* not meter sweep */);
      if (state->op_ == StateOp::kStateOpSweep) update_sweep_time(state);
      return run_rtn;
    }
    return true;
  }

  bool MauMeterAlu::run_cmp_alu_with_state(MauExecuteState *state) {
    BitVector<kDataBusWidth> data_in(UINT64_C(0));
    uint32_t addr = 0u;

    get_input(&data_in, &addr);

    if (!Address::meter_addr_op_enabled(addr)) return false;

    if ( stateful_ctl_.salu_enable() && Address::meter_addr_is_stateful( addr ) ) {
      RMT_ASSERT( ! state->at_eop_ ); // check not being called on EOP
      auto D = get_alu_data_wide(state->match_phv_);

      // send the random number generator value to the SALU
      uint64_t random_number = state->get_meter_random_value(mau_index_,alu_index_,is_ingress_alu());
      salu_.set_random_number_value( random_number );

      uint64_t present_time=0;
      state->get_relative_time(&present_time, is_ingress_alu());

      salu_.calculate_cmp_alu(addr, present_time, D, &data_in);

      bool match = salu_.get_match_output();
      bool learn = salu_.get_learn_output();
      state->match_bus_[ alu_index_ ]          = match;
      state->learn_or_match_bus_[ alu_index_ ] = learn; // || match; TODO: rename, now it is just a learn bus
    }
    return true;
  }


  void MauMeterAlu::get_input(BitVector<kDataBusWidth> *data, uint32_t *addr) {
    // get the input address staight from the meter_addr on this row
    *addr = 0;
    mau_log_row_->meter_rd_addr( addr );
    data->fill_all_zeros();
    if (sweeping()) {
      sweep_get_input( data );
    } else {
      mau_log_row_->stats_alu_rd_data( data );
    }
  }
  BitVector<MauMeterAlu::kStatefulMeterAluDataBits> MauMeterAlu::get_alu_data_wide(Phv* phv) {
    if (phv) {
      auto r = mau_log_row_->get_meter_stateful_selector_alu_data(phv);
      int shift = meter_alu_group_phv_hash_shift_.meter_alu_group_phv_hash_shift();
      r.byte_shift_left(shift);
      r.mask( phv_hash_mask_ );
      return r;
    }
    else {
      return {};
    }
  }
  void MauMeterAlu::get_input_data(BitVector<kDataBusWidth> *data) {
    uint32_t addr;
    get_input(data, &addr);
    // If non-enabled addr just return whatever data get_input() gave us
    if (!Address::meter_addr_op_enabled(addr)) return;

    // Mostly in the logic below we do nothing so just use whatever
    // data the call to get_input() gave us. The only case where
    // we override currently is LPF which might have forwarded
    // data we don't know about. In which case the call to
    // lpf_meter_.get_input_data() will overwrite data provided
    // by get_input()
    // Now extended to meter as well

    if      (meter_ctl_.meter_enable())      { meter_.get_input_data(data);     }
    else if (lpf_enabled_or_red_only_mode()) { lpf_meter_.get_input_data(data); }
    else if (stateful_ctl_.salu_enable())    {                                  }
    else                                     {                                  }
  }


  uint32_t MauMeterAlu::get_alu_data(Phv* phv) {
    auto bus_data = get_alu_data_wide(phv);
    return bus_data.get_word(0,32); // get least significant 32 bits
  }
  void MauMeterAlu::set_output_data(BitVector<kDataBusWidth> *data) {
    if (sweeping()) {
      sweep_set_output(data);
    } else {
      // there only is a stats data bus, no meter data bus any more
      mau_log_row_->set_stats_wr_data(data);
    }
  }
  void MauMeterAlu::set_output_action_nocheck(BitVector<kActionOutputBusWidth>& action) {
    last_action_.copy_from(action);
    // Don't check for multiple writers here
    mau_log_row_->set_action_rd_data_nocheck(&action);
  }
  void MauMeterAlu::set_output_action(BitVector<kActionOutputBusWidth>& action) {
    last_action_.copy_from(action); // make sure last_action_ is updated even if enable is not set
    bool action_overide = MauChipMeterAlu::get_right_action_override( meter_alu_group_action_ctl_ );
    // only OR the results into the ADB if the enable is set
    if ( meter_alu_group_action_ctl_.right_alu_action_enable() &&
         ! action_overide ) {
      set_output_action_nocheck(action);
    }
  }

  void MauMeterAlu::get_output(BitVector<kDataBusWidth> *data, uint32_t *addr,BitVector<kActionOutputBusWidth> *action) {
    if (has_run_) {
      // there only is a stats data bus, no meter data bus any more
      mau_log_row_->stats_wr_data(data);
      mau_log_row_->stats_wr_addr(addr);
      if (action) action->copy_from( last_action_ );
    }
    else {
      data->fill_all_zeros();
      *addr = 0;
      if (action) action->fill_all_zeros();
    }
  }
  void MauMeterAlu::reset_resources() {
    has_run_=false;
    salu_.reset_resources();
  }

  void MauMeterAlu::update_addresses(int old_addr, int new_addr) {
    if ((old_addr < 0) || (new_addr < 0)) return;

    SweepTimeInfo *sweep_time_info = mau()->mau_addr_dist()->get_sweep_time_info(alu_index_);
    sweep_time_info->update_addresses(Address::meter_addr_get_vaddr(old_addr),
                                      Address::meter_addr_get_vaddr(new_addr));
    salu_.update_addresses(old_addr,new_addr);
    lpf_meter_.update_cache(old_addr,new_addr);
    meter_.update_cache(old_addr,new_addr);
  }

  bool MauMeterAlu::catch_up_sweeps(MauExecuteState *state) {
    // OnDemand sweep typically only on in SW_MODE (switchable via kMeterSweepOnDemand).
    // When OnDemand sweep active MeterSweeps are deferred and only occur when
    // someone tries to CfgRd a Meter word or access it for PHV/EOP/TEOP.
    // This is much more efficient (~50x) than sweeping all the time.

    SweepTimeInfo *sweep_time_info = mau()->mau_addr_dist()->get_sweep_time_info(alu_index_);
    if (!sweep_time_info->on_demand_used()) return false;

    uint64_t access_T = UINT64_C(0);
    state->get_relative_time(&access_T, is_ingress_alu());
    // We catch-up to a couple of cycles ago - don't want to mess with forwarding
    if (access_T >= UINT64_C(2)) access_T -= UINT64_C(2); else access_T = UINT64_C(0);

    uint32_t vaddr = Address::meter_addr_get_vaddr(state->addr_); // Remove OP
    uint64_t prev_sweep_T = UINT64_C(0);
    uint64_t next_sweep_T = sweep_time_info->get_next_sweep_t(vaddr);
    if (next_sweep_T > access_T) return false;

    SweepConfigEntry *cnf = sweep_time_info->get_config(vaddr);
    RMT_ASSERT(cnf != nullptr);
    bool salu_enabled = (stateful_ctl_.salu_enable() == 1);
    int vpn = Address::meter_addr_get_vpn(vaddr);
    int index = Address::meter_addr_get_index(vaddr);
    int shift = (salu_enabled) ?cnf->huffman_shift() :0;
    int n_subwords = cnf->n_subwords();
    RMT_ASSERT(salu_enabled || (n_subwords == 1)); // Only ever >1 subwords if SALU
    int sweep_op4 = cnf->sweep_op4();
    bool is_sweep = ((sweep_op4 == Address::kMeterOp4Sweep) &&
                     (cnf->alu_enabled() || !MauDefs::kMeterClearOveridesSweep));

    BitVector<kDataBusWidth> data;
    BitVector<kDataBusWidth> ctl_mask;
    ctl_mask.fill_all_ones();
    // Allow *timestamp* to vary if !salu_enabled - top bits of word [100..127] - create mask
    if (!salu_enabled) ctl_mask.set_word(UINT64_C(0),
                                         MauDefs::kTimestampBitPos, MauDefs::kTimestampWidth);

    // Fetch initial value of stats_alu_rd_data and start sweep using that
    data.fill_all_zeros();
    mau_log_row_->stats_alu_rd_data(&data);
    sweep_start(data);
    // Now that sweep data input buffer has been filled with initial val stats_alu_rd_data
    // sweeping proceeds consuming a sweep data input buffer and producing a sweep data output
    // buffer. The buffers get swapped on each sweep cycle the output from cycle N becoming
    // the input to cycle N+1.
    // Once the output remains unchanged wrt input then sweeping can terminate.
    // The final sweep output gets used exactly one more time on the next get_input call
    //  thus mimicking the input that would have come from SRAM had regular sweeping been used.

    while (next_sweep_T <= access_T) {

      bool ctl_changed = false;

      for (int subword = 0; subword < n_subwords; subword++) {
        MauExecuteState sweep_state(nullptr, nullptr, -1, -1);

        uint32_t vaddr2 = Address::meter_addr_make2(vpn, index, subword << shift);
        sweep_state.addr_ = Address::meter_addr_make(vaddr2, sweep_op4);
        sweep_state.addrtype_ = AddrType::kMeter;
        sweep_state.op_ = StateOp::kStateOpSweep;
        sweep_state.relative_time_ = next_sweep_T;
        sweep_state.meter_tick_time_ = next_sweep_T;
        sweep_state.ret_ = 1;

        // run_alu_internal calls sweep_get_input/sweep_set_output internally
        run_alu_internal(&sweep_state, is_sweep);

        // See if sweep input/output differ under mask
        if (sweep_data_changed(ctl_mask)) ctl_changed = true;

        sweep_continue();

      } // for (int subword = 0; subword < n_subwords; subword++)

      //printf("CATCH_UP_SWEEPS(Cnt=%d,T=%" PRId64 "(%" PRIx64 ") "
      //       "lastT=%" PRId64 "(%" PRIx64 ") CtlChanged=%c\n",
      //       cnt, next_sweep_T, next_sweep_T, access_T, access_T,
      //       ctl_changed?'T':'F'); cnt++;

      if (!ctl_changed) {
        // Nothing changed - so break out early - no point in further sweeps
        //
        // But pretend we've swept all the way to sweep time preceding access_T
        prev_sweep_T = sweep_time_info->get_prev_sweep_t(vaddr, access_T);
        if (!salu_enabled) {
          // If not SALU, update last sweep output to have prev_sweep_T as timestamp
          sweep_update_output(prev_sweep_T >> meter_ctl_.meter_time_scale(),
                              MauDefs::kTimestampBitPos, MauDefs::kTimestampWidth);
        }
        break;
      }

      prev_sweep_T = next_sweep_T;
      next_sweep_T = sweep_time_info->get_next_sweep_t(vaddr);

    } // while

    sweep_time_info->set_last_sweep_t(vaddr, prev_sweep_T);
    sweep_stop();

    return true;
  }

  void MauMeterAlu::update_sweep_time(MauExecuteState *state) {
    SweepTimeInfo *sweep_time_info = mau()->mau_addr_dist()->get_sweep_time_info(alu_index_);
    uint64_t access_T = UINT64_C(0);
    state->get_relative_time(&access_T, is_ingress_alu());
    sweep_time_info->set_last_sweep_t(Address::meter_addr_get_vaddr(state->addr_), access_T);
  }

}
