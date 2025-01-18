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
#include <mau-stateful-counters.h>
#include <register_adapters.h>



namespace MODEL_CHIP_NAMESPACE {

MauStatefulCounters::AddrNoStageId MauStatefulCounters::AddrWithStageId::remove_stage_id() const {
  return AddrNoStageId( (instr_part().addr_   >> kStatefulAddrStageBits) |
                        (counter_part().addr_ &  kCounterNoStageIdPartMask) );
}

MauStatefulCounters::MauStatefulCounters(RmtObjectManager *om,
                                         int pipeIndex, int mauIndex, Mau *mau)
    : MauObject(om, pipeIndex, mauIndex, kType, mau), ctor_running_(true),
      mau_stateful_log_counter_array_(default_adapter(mau_stateful_log_counter_array_,chip_index(),pipeIndex,mauIndex)),
      mau_stateful_log_counter_ctl_(default_adapter(mau_stateful_log_counter_ctl_,chip_index(),pipeIndex,mauIndex)),
      mau_stateful_log_counter_ctl2_(default_adapter(mau_stateful_log_counter_ctl2_,chip_index(),pipeIndex,mauIndex,
                                                     [this](int a0){this->stateful_ctl2_callback(a0);})),
      mau_stateful_log_counter_ctl3_(default_adapter(mau_stateful_log_counter_ctl3_,chip_index(),pipeIndex,mauIndex)),

      mau_stateful_log_counter_clear_(default_adapter(mau_stateful_log_counter_clear_,chip_index(),pipeIndex,mauIndex,
                                                      [this](){this->stateful_counter_clear_callback();})),
      mau_stateful_log_ctl_ixbar_map_(default_adapter(mau_stateful_log_ctl_ixbar_map_,chip_index(),pipeIndex,mauIndex,
                                                      [this](int a2,int a1,int a0){this->stateful_ixbar_map_callback(a2,a1,a0);})),
      mau_stateful_log_counter_oxbar_map_(default_adapter(mau_stateful_log_counter_oxbar_map_,chip_index(),pipeIndex,mauIndex)),
      mau_stateful_log_counter_logical_map_(default_adapter(mau_stateful_log_counter_logical_map_,chip_index(),pipeIndex,mauIndex)),
      mau_stateful_log_stage_vpn_offset_(default_adapter(mau_stateful_log_stage_vpn_offset_,chip_index(),pipeIndex,mauIndex)),
      mau_stateful_log_fifo_level_(default_adapter(mau_stateful_log_fifo_level_,chip_index(),pipeIndex,mauIndex)),
      mau_stateful_log_watermark_threshold_(default_adapter(mau_stateful_log_watermark_threshold_,chip_index(),pipeIndex,mauIndex)),
      mau_stateful_log_cur_watermark_(default_adapter(mau_stateful_log_cur_watermark_,chip_index(),pipeIndex,mauIndex)),
      meter_alu_adr_range_check_icxbar_map_(default_adapter(meter_alu_adr_range_check_icxbar_map_,chip_index(),pipeIndex,mauIndex)),
      mau_meter_alu_vpn_range_(default_adapter(mau_meter_alu_vpn_range_,chip_index(),pipeIndex,mauIndex)),
      meter_adr_shift_(default_adapter(meter_adr_shift_,chip_index(),pipeIndex,mauIndex)),
      stateful_instr_width_logical_(default_adapter(stateful_instr_width_logical_,chip_index(),pipeIndex,mauIndex)),
      adr_dist_table_thread_(default_adapter(adr_dist_table_thread_,chip_index(), pipeIndex, mauIndex,
                                             [this](uint32_t i, uint32_t j ){this->ad_thread_change_callback(i,j); }))
{
  mau_stateful_log_counter_array_.reset();
  mau_stateful_log_counter_ctl_.reset();
  mau_stateful_log_counter_ctl2_.reset();
  mau_stateful_log_counter_ctl3_.reset();
  mau_stateful_log_counter_clear_.reset();
  mau_stateful_log_ctl_ixbar_map_.reset();
  mau_stateful_log_counter_oxbar_map_.reset();
  mau_stateful_log_counter_logical_map_.reset();
  mau_stateful_log_stage_vpn_offset_.reset();
  mau_stateful_log_fifo_level_.reset();
  mau_stateful_log_watermark_threshold_.reset();
  mau_stateful_log_cur_watermark_.reset();
  meter_alu_adr_range_check_icxbar_map_.reset();
  mau_meter_alu_vpn_range_.reset();
  meter_adr_shift_.reset();
  stateful_instr_width_logical_.reset();
  adr_dist_table_thread_.reset();

  for (int i=0;i<kNumStatefulCounters;++i) {
    T_clear_next_[i] = kTmax;
    cntr_flags_[i] = 0;
  }
  ctor_running_ = false;
}
MauStatefulCounters::~MauStatefulCounters() {
}

bool MauStatefulCounters::stateful_counter_enabled(int lt) {
  // stateful enabled if either push or pop counter enabled
  return ((lt >= 0) && (lt < kLogicalTables) &&
          ((get_type_stateful_counter(lt,0) != CntrType::kDisabled) ||
           (get_type_stateful_counter(lt,1) != CntrType::kDisabled))
           );
}
bool MauStatefulCounters::stateful_counter_at_max(int counter) {
  //RMT_ASSERT(0);
  // This is used on Tofino fill in whether stateful counters are done for the *ALU*
  // It's called from intr_status_mau_ad callback
  // Not sure if anyone cares for JBay, so just disable assert for moment
  return false;
}
uint32_t MauStatefulCounters::maybe_increment_stateful_counter(uint32_t addr, int lt,
                                                               const MauLookupResult &res) {
  // Does nothing on JBay, increment is done in tick_counters
  //  and value is returned in get_counter_value_for_* functions
  return addr;
}


void MauStatefulCounters::tick_counters( const std::array<MauLookupResult,kLogicalTables> &lookup_results,
                                         MauExecuteState* state) {
  // Note MAU will have been locked at start PHV processing

  // Work out the time. From the correct gress of the phv. Insist identical if DV_MODE
  Phv* match_phv = state->match_phv_;
  RMT_ASSERT( match_phv != nullptr );
  uint64_t active_counters = 0;
  uint64_t Ting = 0u, Tegr = 0u, T = state->get_phv_time(&Ting,&Tegr);
  if (MauStatefulCounters::kStatefulCounterTickCheckTime) { // Only DV_MODE normally
    RMT_ASSERT((Ting == UINT64_C(0)) || (Tegr == UINT64_C(0)) || (Ting == Tegr));
  }

  // Figure out which counters could be active for clear by working out the gress of
  //   each counter, then looking at the gress(es) of the match phv.
  // Get handle on MauDependencies so we can figure out if LTs are ingress/egress
  MauDependencies *deps = mau()->mau_dependencies();
  std::array<bool,kNumStatefulCounters> counter_is_ingress{};
  std::array<bool,kNumStatefulCounters> counter_is_egress{};
  for (int lt = 0; lt < kLogicalTables; ++lt) {
    for (int ev = 0; ev < 2; ++ev) {
      // First map from logical table space into counter space
      uint32_t reg = mau_stateful_log_ctl_ixbar_map_.
          mau_stateful_log_ctl_ixbar_map(lt / 8,0,ev);
      uint8_t field = (reg >> ((lt % 8) * 3)) & 0x7;
      bool enabled = 0x4 & field;
      int lt_counter  = field & 0x3;
      if ( enabled ) {
        if (deps->is_ingress_lt(lt)) {
          counter_is_ingress[lt_counter] = true;
        }
        else {
          counter_is_egress[lt_counter] = true;
        }
      }
    }
  }


  for (int counter=0;counter<kNumStatefulCounters;++counter) {
    RMT_ASSERT( !( counter_is_ingress[counter] && counter_is_egress[counter] ) );

    if ( match_phv->ingress_ghost() && counter_is_ingress[counter] ) {
      active_counters |= UINT64_C(1) << counter;
    }
    if ( match_phv->egress() && counter_is_egress[counter] ) {
      active_counters |= UINT64_C(1) << counter;
    }
  }

  // Maybe replace all logic above with 'active_counters = counters_in_gress(gress_mask)'
  // For the moment leave it in, calc both ways, compare and warn if different
  uint8_t gress_mask = ((match_phv->ingress_ghost() ?1 :0) | (match_phv->egress() ?2 :0));
  uint64_t active_counters2 = counters_in_gress(gress_mask);
  if (active_counters != active_counters2) {
    RMT_LOG(RmtDebug::warn(),
            "MauStatefulCounters::tick_counters active_counter MISMATCH! (0x%x vs 0x%x)\n",
            active_counters, active_counters2);
  }

  // Clear all counters up to time T-1
  if (T > 0) continue_clear_till(active_counters,T-1);
  // Later logic this func will handle clearing at T

  for (int counter=0;counter<kNumStatefulCounters;++counter) {
    bool events[2] { false, false };
    // Go through all the logical tables and look for push and pop events for this counter
    for (int lt=0;lt<kLogicalTables;++lt) {
      const MauLookupResult &res = lookup_results[lt];
      if (res.valid() && res.active()) { // Only examine active results
        // TODO: get rid of const cast here : would have to propagate const down into MauLookupResult functions
        uint32_t meter_addr = const_cast<MauLookupResult&>(res).extract_meter_addr(mau(),lt);
        if (Address::meter_addr_enabled(meter_addr)) {
          for (int which_event=0;which_event<2;++which_event) {
            // First map from logical table space into counter space
            uint32_t regR = mau_stateful_log_ctl_ixbar_map_.mau_stateful_log_ctl_ixbar_map(lt / 8,0,which_event);
            uint32_t regL = mau_stateful_log_ctl_ixbar_map_.mau_stateful_log_ctl_ixbar_map(lt / 8,1,which_event);
            RMT_ASSERT(regR == regL); // there are right and left versions for physical reasons
            int field = (regR >> ((lt % 8) * 3)) & 0x7;
            bool enable = 0x4 & field;
            int lt_counter = field & 0x3;
            if (enable && (counter == lt_counter) && res.active() ) {
              if ( counter_event_happened( lt,which_event, res ) ) {
                // there can be more than one event per counter in DV!
                //RMT_ASSERT( events[0] == false && events[1] == false );
                events[which_event] = true;
                RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::tick_counters counter %d: %s event for LT %d\n",
                        counter,which_event?"pop":"push",lt);
              }
            }
          }
        }
      }
    }

    // Now do any push, pop or in-progress clear events for this counter
    if ( events[0] || events[1] || is_clear_counter_active(counter, T) ) {

      int counter_type = mau_stateful_log_counter_ctl2_.slog_counter_function(counter);

      switch ( counter_type ) {
        case 0: // disabled
          RMT_ASSERT(0); // should not have either a push or a pop to a disabled counter!
          break;
        case 1:  // stateful logging
          RMT_ASSERT( !events[1] ); // should not have a pop to a stateful logging counter
          if (events[0]) // only push events tick the logging counter
            (void)tick_logging(counter,T);
          break;
        case 2:  // FIFO
          // When DVing the RTL push and pop can happen at the same time
          //  in this case the RTL just ors together the counter outputs
          if (events[1])
            (void)pop_fifo(counter,events[0],T);
          if (events[0])
            (void)push_fifo(counter,events[1],T);
          break;
        case 3: // stack
          // When DVing the RTL push and pop can happen at the same time
          //  in this case the RTL just ors together the counter outputs
          if (events[1])
            (void)pop_stack(counter,events[0],T);
          else if (events[0])  // case where both push and pop are active is handled by pop_stack()
            (void)push_stack(counter,events[1],T);
          break;
        case 4: // bloom filter clear
          RMT_ASSERT( !events[1] ); // should not have a pop to a stateful logging counter
          // check gress on phv is correct, we might get two phv's for the same time from DV
          //  (one for ingress and one for egress) and we don't want to do this twice.
          if (( active_counters>>counter) & 1 ) {
            (void)tick_clear_counter(counter, T);  // Does nothing if not clearing
            // If we've had a push event, arrange to start clear at T+1
            if (events[0]) start_clear_at(counter, T+1);
          }
          break;
        default:
          RMT_ASSERT(0);
          break;
      }
    }
  }
  // Maybe loop doing all remaining clearing now (not if DV_MODE)
  maybe_synchronous_clear(false);
}

int MauStatefulCounters::increment_with_wrap(int counter, int value) {
  // need to increment and wrap from vpn_max to vpn_min, fixing up extra_bit
  int value_without_extra_bit = value & no_extra_bit_mask_[counter];

  int new_value;
  if ( value_without_extra_bit == max_value( counter ) ) {
    new_value = min_value( counter );
    // put back the extra_bit inverted as we have wrapped
    int extra_bit_inv = (~value)  & extra_bit_mask_[counter];
    new_value = new_value ^ extra_bit_inv;
  }
  else {
    new_value = value + 1;
  }
  return new_value;
}
int MauStatefulCounters::decrement_with_wrap(int counter, int value) {
  // need to decrement and wrap from vpn_min to vpn_max, fixing up extra_bit
  int value_without_extra_bit = value & no_extra_bit_mask_[counter];

  int new_value;
  if ( value_without_extra_bit == min_value( counter ) ) {
    new_value = max_value( counter );
    // put back the extra_bit inverted as we have wrapped
    int extra_bit_inv = (~value)  & extra_bit_mask_[counter];
    new_value = new_value ^ extra_bit_inv;
  }
  else {
    new_value = value - 1;
  }
  return new_value;
}

bool MauStatefulCounters::check_counter_value( int counter, int value ) {
  int masked_value = value & no_extra_bit_mask_[counter];
  return ( (masked_value >= min_value(counter)) && (masked_value <= max_value(counter)) );
}

bool MauStatefulCounters::tick_logging(int counter, uint64_t T) {
  bool ticked = false;
  const char *actions[] = { "ERROR", "tick_logging", "ERROR", "push_stack", "counter_clear" };

  spinlock();
  int v = mau_stateful_log_counter_array_.mau_stateful_log_counter(counter,0);
  RMT_ASSERT( check_counter_value( counter, v ));

  int counter_type = mau_stateful_log_counter_ctl2_.slog_counter_function(counter);
  RMT_ASSERT((counter_type >= 1) && (counter_type <= 4));
  // for RMT_LOG, as this function is used both for tick_logging and push_stack
  const char* action = actions[counter_type];

  set_counter_output_value(counter, v);
  if ( v == wrapped_max_value(counter) ) {
    set_output_overflow(counter);
    // XXX: ALL setting/clearing of Overflowed/Underflowed flags
    // disabled for now. But leave in place in case we need to allow
    // unconditional entry into MauStatefulCounter logic in these cases
    //
    //set_cntr_flag(counter, kFlagOverflowed);
    RMT_LOG(RmtDebug::verbose(),
            "MauStatefulCounters::%s T=%" PRId64 " counter %d: value=%06x overflow\n",
            action,T,counter,v);
  }
  else {
    // even though logging doesn't wrap, the counter does wrap to max+1 when full
    int new_v = increment_with_wrap(counter,v);
    mau_stateful_log_counter_array_.mau_stateful_log_counter(counter,0,new_v);
    //clr_cntr_flag(counter, kFlagOverflowed);
    ticked = true;
    update_levels_push(counter);
    update_thresholds_push(counter);
    RMT_LOG(RmtDebug::verbose(),
            "MauStatefulCounters::%s T=%" PRId64 " counter %d: value=%06x new_value=%06x\n",
            action,T,counter,v,new_v);
  }
  spinunlock();
  return ticked;
}
bool MauStatefulCounters::tick_clear_counter(int counter, uint64_t T) {

  // Is a start pending for this cycle - remember and if so, clear flag
  bool start_restart = is_clear_counter_start_pending(counter, T);
  if (start_restart) clr_clear_counter_start_pending(counter);
  bool ticked = false;

  while (true) {

    // Is counter running - if not we might be starting it now
    if (!is_clear_counter_running(counter, T) && start_restart) {
      // YES - update running flag
      set_clear_counter_running(counter);
    }
    if (!is_clear_counter_running(counter, T)) break; // Really not running, exit

    // Counter running so tick_logging
    ticked = tick_logging(counter, T);
    // Use overflow instruction as clear instruction
    set_output_overflow(counter);

    if (!ticked) {
      clr_clear_counter_running(counter); // Counter overflowed so stops running
      reset_stateful_counter(counter);    // Reset counter itself on overflow
      // XXX - now that clear counter has overflowed reset output addr to 0
      counter_output_[counter].reset();
      RMT_LOG( RmtDebug::verbose(),
               "MauStatefulCounters::tick_clear_counter %d: PostRESET counter_output=%08x\n",
               counter,counter_output_[counter].to_uint32());

      // But we may instantly restart
      if (start_restart) {
        // YES - loop and restart
        RMT_LOG(RmtDebug::verbose(),
                "MauStatefulCounters::counter_clear T=%" PRId64 " counter %d: "
                "Immediately restarting clear\n", T, counter);
      } else {
        // NO - counter stopped - exit
        T_clear_next_[counter] = kTmax;
        break;
      }
    } else {
      // Counter still running - advance time of next clear and exit
      T_clear_next_[counter] = T+1; // TIME MOVES ON HERE
      break;
    }

  }
  return ticked;
}
bool MauStatefulCounters::push_fifo(int counter, bool pop_also, uint64_t T) {
  bool pushed = false;
  spinlock();
  int v_push = mau_stateful_log_counter_array_.mau_stateful_log_counter(counter,0);
  int v_pop  = mau_stateful_log_counter_array_.mau_stateful_log_counter(counter,1);
  RMT_LOG( RmtDebug::verbose(), "MauStatefulCounters::push_fifo counter %d: v_push=%06x v_pop=%06x\n",counter,v_push,v_pop);
  RMT_ASSERT( check_counter_value( counter, v_push ));
  RMT_ASSERT( check_counter_value( counter, v_pop ));
  set_counter_output_value(counter, v_push);
  // Full test is if v_push and v_pop are the same except for the extra_bit
  // can't overflow if pushing at the same time (only happens in DV)
  if ( (v_push == ( v_pop ^ extra_bit_mask_[counter])) && !pop_also) {
    RMT_LOG( RmtDebug::verbose(), "MauStatefulCounters::push_fifo counter %d: overflow\n",counter);
    set_output_overflow(counter);
    //set_cntr_flag(counter, kFlagOverflowed);
  }
  else {
    int v = increment_with_wrap(counter,v_push);
    RMT_LOG( RmtDebug::verbose(), "MauStatefulCounters::push_fifo counter %d: new_v=%06x\n",counter,v);
    mau_stateful_log_counter_array_.mau_stateful_log_counter(counter,0,v);
    //clr_cntr_flag(counter, kFlagOverflowed);
    pushed = true;
    if (!pop_also) update_levels_push(counter);
    update_thresholds_push(counter);
  }
  spinunlock();
  return pushed;
}
bool MauStatefulCounters::pop_fifo(int counter, bool push_also, uint64_t T) {
  bool popped = false;
  spinlock();
  int v_push = mau_stateful_log_counter_array_.mau_stateful_log_counter(counter,0);
  int v_pop  = mau_stateful_log_counter_array_.mau_stateful_log_counter(counter,1);
  RMT_LOG( RmtDebug::verbose(), "MauStatefulCounters::pop_fifo counter %d: v_push=%06x v_pop=%06x%s\n",counter,v_push,v_pop,
           push_also ? " (push also)":"");
  RMT_ASSERT( check_counter_value( counter, v_push ));
  RMT_ASSERT( check_counter_value( counter, v_pop ));
  set_counter_output_value(counter, v_pop);
  // Empty test is if v_push and v_pop are the same including the extra_bit
  // can't underflow if pushing at the same time (only happens in DV)
  if ( (v_push == v_pop) && !push_also) {
    set_output_underflow(counter);
    //set_cntr_flag(counter, kFlagUnderflowed);
  }
  else {
    int v = increment_with_wrap(counter,v_pop);
    RMT_LOG( RmtDebug::verbose(), "MauStatefulCounters::pop_fifo counter %d: new_v=%06x\n",counter,v);
    mau_stateful_log_counter_array_.mau_stateful_log_counter(counter,1,v);
    //clr_cntr_flag(counter, kFlagUnderflowed);
    popped = true;
    if (!push_also) update_levels_pop(counter);
    update_thresholds_pop(counter);
  }
  spinunlock();
  return popped;
}
bool MauStatefulCounters::push_stack(int counter, bool pop_also, uint64_t T) {
  // only used when just pushing, so tick_logging code does not have to handle the case of both
  RMT_ASSERT( ! pop_also );
  // does the same thing as ticking the logging counter
  return tick_logging(counter, T);
}
bool MauStatefulCounters::pop_stack(int counter, bool push_also, uint64_t T) {
  bool popped = false;
  spinlock();
  int v = mau_stateful_log_counter_array_.mau_stateful_log_counter(counter,0);
  int new_v = v;
  RMT_LOG( RmtDebug::verbose(), "MauStatefulCounters::pop_stack counter %d: v=%06x\n",counter,v);
  RMT_ASSERT( check_counter_value( counter, v ));
  // the counter does not move if there is also a push
  if ( ! push_also ) {
    if ( v == min_value(counter) ) {
      set_output_underflow(counter);
      //set_cntr_flag(counter, kFlagUnderflowed);
      RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::pop_stack counter %d: value=%06x underflow\n",counter,v);
    }
    else {
      // can only wrap when full to one less than full
      new_v = decrement_with_wrap(counter,v);
      RMT_LOG( RmtDebug::verbose(), "MauStatefulCounters::pop_stack counter %d: new_v=%06x\n",counter,new_v);
      mau_stateful_log_counter_array_.mau_stateful_log_counter(counter,0,new_v);
      //clr_cntr_flag(counter, kFlagUnderflowed);
      popped = true;
      update_levels_pop(counter);
      update_thresholds_pop(counter);
    }
  }
  else {
    update_thresholds_pop(counter); // needs updating even if there is also a push
  }
  if (push_also) update_thresholds_push(counter);
  set_counter_output_value(counter, new_v); // note: uses post decrement value
  spinunlock();
  return popped;
}

bool MauStatefulCounters::counter_event_happened(int lt, int which_event, const MauLookupResult& res ) {
  bool e = false;
  switch (get_type_stateful_counter(lt,which_event)) {
    case CntrType::kDisabled:       e = false;        break;
    case CntrType::kTableMiss:      e = !res.match(); break;
    case CntrType::kTableHit:       e = res.match();  break;
    case CntrType::kGatewayInhibit: e = (res.gatewayran() && res.gatewayinhibit()); break;
    case CntrType::kUnconditional:  e = true;         break;
    case CntrType::kGatewayInhibitEntry0:
      e = (res.gatewayran() && res.gatewayinhibit() && res.gatewayhitindex()==0); break;
    case CntrType::kGatewayInhibitEntry1:
      e = (res.gatewayran() && res.gatewayinhibit() && res.gatewayhitindex()==1); break;
    case CntrType::kGatewayInhibitEntry2:
      e = (res.gatewayran() && res.gatewayinhibit() && res.gatewayhitindex()==2); break;
    case CntrType::kGatewayInhibitEntry3:
      e = (res.gatewayran() && res.gatewayinhibit() && res.gatewayhitindex()==3); break;
    case CntrType::kGatewayInhibitMiss:
      e = (res.gatewayran() && res.gatewayinhibit() && res.gatewayhitindex()< 0); break;
    default: RMT_ASSERT(0); break;
  }
  return e;
}

void MauStatefulCounters::reset_resources() {
  for (int i=0;i<kNumStatefulCounters;++i) {
    counter_output_[i].reset();
  }
  for (int i=0;i<kLogicalTables;++i) {
    vpn_in_range_[i] = false;
  }
  for (int i=0;i<kNumAlus;++i) {
    final_addrs_[i] = 0u;
  }
}

// This implements the logic on the top right hand side of the
//  uArch figure "Stateful Logging FIFO Push/Pop Counters",
//   starting at the "addr padding" block and going through to "To ALU"
uint32_t MauStatefulCounters::get_meter_addr( int alu, uint32_t meter_addr_in, uint64_t T ) {

  int  counter = 0, width = 0; // Default vals if no counter enabled
  bool enable = mau_stateful_log_counter_oxbar_map_.stateful_log_counter_oxbar_enable(alu);

  if (enable) {
    // Get real vals counter/width given counter enabled
    counter = mau_stateful_log_counter_oxbar_map_.stateful_log_counter_oxbar_ctl(alu);
    width = mau_stateful_log_counter_ctl2_.slog_instruction_width(counter);

    RMT_LOG(RmtDebug::verbose(),
            "MauStatefulCounters::get_meter_addr_for_alu %d: in=%08x counter=%d f=0x%x\n",
            alu, meter_addr_in, counter, cntr_flags_[counter]);
  } else {
    RMT_LOG(RmtDebug::verbose(),
            "MauStatefulCounters::get_meter_addr_for_alu %d: in=%08x\n", alu, meter_addr_in);
  }

  if (enable && is_clear_counter_active(counter) && (meter_addr_in != 0u)) {
    meter_addr_in = 0u; // Only squash if counter enabled and active
    const char *outstr = is_clear_counter_running(counter) ?"being cleared" :"clear pending";
    RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::get_meter_addr_for_alu %d: "
            "T=%" PRId64 " in=0 Input squashed as counter %d %s\n",alu,T,counter,outstr);
  }

  AddrNoStageId final_meter_addr(meter_addr_in);

  // from here on we are dealing with padded values as meter_addr_in is in this format

  AddrWithStageId oxbar_output{};
  if (enable) {
    RMT_LOG(RmtDebug::verbose(),
            "MauStatefulCounters::get_meter_addr_for_alu %d: counter=%d width=%d counter_output=%08x counter_output_padded=%08x\n",
            alu,counter,width,counter_output_[counter].to_uint32(),counter_output_[counter].pad(width).to_uint32());

    oxbar_output.or_in( counter_output_[counter].pad( width) );

    RMT_LOG(RmtDebug::verbose(),
            "MauStatefulCounters::get_meter_addr_for_alu %d: counter=%d width=%d oxbar_output=%08x\n",
            alu,counter,width,oxbar_output.to_uint32());
  }

  // the input to the meter addr shift always has the oxbar output's instr and counter part
  //  ored in (not the squash bit). The instr part will be zero unless there is an under/overflow.
  AddrWithStageId meter_addr_shift_input{};
  meter_addr_shift_input.or_in( oxbar_output.instr_part() );
  meter_addr_shift_input.or_in( oxbar_output.counter_part() );

  // If the instr!=0 or the squash bit is set remove the instr portion of the
  //  incoming data (final_meter_addr), otherwise OR in the whole value.
  // Note: have to add a zero stage id to final_meter_addr to make the width compatible
  bool squash = oxbar_output.squash_bit_non_zero();
  if ( squash || oxbar_output.instr_non_zero() ) {
    meter_addr_shift_input.or_in( final_meter_addr.add_stage_id().counter_part() );
  }
  else {
    meter_addr_shift_input.or_in( final_meter_addr.add_stage_id() );
  }

  // apply meter addr_shift to the counter part, the instr part does not get shifted
  AddrWithStageId meter_addr_shift_output( meter_addr_shift_input.instr_part() );
  int left_shift = meter_adr_shift( alu );
  meter_addr_shift_output.or_in( (meter_addr_shift_input.counter_part().shift_left( left_shift )).counter_part()  );

  RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::get_meter_addr_for_alu meter_adr_shift=%d unshifted_part=%08x shifted_part=%08x after_shift=%08x final=%08x\n",
          left_shift,
          meter_addr_shift_input.instr_part().to_uint32(),
          meter_addr_shift_input.counter_part().to_uint32(),
          (meter_addr_shift_input.counter_part().shift_left( left_shift )).counter_part().to_uint32(),
          meter_addr_shift_output.to_uint32());

  RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::get_meter_addr_for_alu %d: vpn=%d \n",alu,meter_addr_shift_output.get_vpn());

  // Do not set the in range bits for non-enabled address, but do set if for squashed
  //   addresses to keep the control flow cleaner for multi-stage fifos
  if ( (squash || meter_addr_shift_output.instr_non_zero()) &&
       vpn_in_range(alu, meter_addr_shift_output.get_vpn() ) ) {
    set_vpn_in_range_bit(alu);  // used later for immediate data
  }
  if ( meter_addr_shift_output.instr_non_zero() &&
       vpn_in_range(alu, meter_addr_shift_output.get_vpn() ) ) {
    AddrNoStageId output_addr( meter_addr_shift_output.remove_stage_id() );
    final_addrs_[alu] = output_addr.to_uint32(); // Stash for DV
    RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::get_meter_addr_for_alu %d: "
            "T=%" PRId64 " output=%08x\n",alu,T,output_addr.to_uint32() );
    return output_addr.to_uint32();
  }
  else {
    AddrNoStageId tmp_addr( meter_addr_shift_output.remove_stage_id() );
    final_addrs_[alu] = tmp_addr.to_uint32(); // Stash for DV
    RMT_LOG(RmtDebug::warn(), "MauStatefulCounters::get_meter_addr_for_alu %d: "
            "T=%" PRId64 " out of range, squashed\n",alu,T);
    return 0; // squash output if vpn out of range
  }
}

uint32_t MauStatefulCounters::get_immediate_data( int lt, uint32_t imm_data_in, uint32_t meter_addr_in ) {

  RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::get_immediate_data %d: imm_data_in=%08x meter_addr_in=%06x\n",lt,imm_data_in,meter_addr_in );

  AddrUnpadded out( 0 );

  if ( mau_stateful_log_counter_logical_map_.stateful_log_counter_logical_map_enable(lt) ) {

    int counter =   mau_stateful_log_counter_logical_map_.stateful_log_counter_logical_map_ctl(lt);

    RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::get_immediate_data counter%d out = %08x\n",
	    counter, counter_output_[counter].to_uint32() );

    AddrUnpadded oxbar_output( counter_output_[counter] );

    // If the instr!=0 and the squash bit are clear then use the instr part of
    //  the meter_addr_in, otherwise use the instr part from the counter
    out.or_in( AddrUnpadded( oxbar_output.counter_part() ) );
    RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::get_immediate_data with counter%d = %08x\n",
	    counter, out.to_uint32() );
    if ( oxbar_output.instr_or_squash_bit_non_zero() ) {
      out.or_in( oxbar_output.instr_part() );
      RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::get_immediate_data instr/squash with oxbar instr_part = %08x\n",
	      out.to_uint32() );
    }
    else {
      out.or_in_instr( Address::meter_addr_op4(meter_addr_in) );
      RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::get_immediate_data with op = %08x\n",
	      out.to_uint32() );
    }
    out.reduce_4b_instr_to_3b_instr();
    RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::get_immediate_data after reduce to 3b=%08x\n",
	    out.to_uint32() );
  }

  // Range bit is always ORed in
  out.or_in_range_bit( vpn_in_range_[lt] );
  RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::get_immediate_data after range_bit=%08x\n",
          out.to_uint32() );

  out.or_in( AddrUnpadded( imm_data_in ) );
  RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::get_immediate_data after imm_data_in or=%08x\n",
	  out.to_uint32() );

  // Note: VPN offset subtraction is always done
  int vpn_offset  = mau_stateful_log_stage_vpn_offset_.stateful_log_stage_vpn_offset(lt);
  int instr_width = stateful_instr_width_logical_.stateful_instr_width_logical(lt);

  out.subtract_vpn_offset( vpn_offset, instr_width );

  RMT_LOG(RmtDebug::verbose(), "MauStatefulCounters::get_immediate_data instr_width=%d vpn_offset=%d out=%08x\n",
          instr_width, vpn_offset, out.to_uint32() );

  return out.to_uint32();
}

uint32_t MauStatefulCounters::get_final_addr(int alu) {
  // For DV, return addr before final VPN squash
  return ((alu >= 0) && (alu < kNumAlus)) ?final_addrs_[alu] :0u;
}



void MauStatefulCounters::refresh_lt_cntr_gress() {
  std::array<uint8_t, 2> local_cntrs_in_gress{};
  // Figure out counters in each gress using ixbar map
  for (int lt = 0; lt < kLogicalTables; ++lt) {
    for (int lr = 0; lr < 2; ++lr) {
      for (int ev = 0; ev < 2; ++ev) {
        // First map from logical table space into counter space
        uint32_t reg = mau_stateful_log_ctl_ixbar_map_.mau_stateful_log_ctl_ixbar_map(lt / 8,lr,ev);
        uint8_t field = (reg >> ((lt % 8) * 3)) & 0x7;
        bool en   = (((field >> 2) & 1) == 1);
        int  cntr = field & 0x3;
        if (en) {
          for (int gress = 0; gress <= 1; ++gress) {
            if ((((lts_in_gress_[  gress] >> lt) & 1) == 1) &&
                (((lts_in_gress_[1-gress] >> lt) & 1) == 0)) {
              // Track cntr gress but only if LT in a single gress
              local_cntrs_in_gress[gress] |= 1<<cntr;
            }
          }
        }
      }
    }
  }
  RMT_ASSERT( ((local_cntrs_in_gress[0] & local_cntrs_in_gress[1]) == 0) && "StatefulCounter in both gresses");

  // Set member vars atomically
  spinlock();
  cntrs_in_gress_[0] = local_cntrs_in_gress[0];
  cntrs_in_gress_[1] = local_cntrs_in_gress[1];
  spinunlock();
}
uint8_t MauStatefulCounters::counters_in_gress(uint8_t gress_mask) {
  std::array<uint8_t, 2> local_cntrs_in_gress{};
  spinlock();
  local_cntrs_in_gress[0] = cntrs_in_gress_[0];
  local_cntrs_in_gress[1] = cntrs_in_gress_[1];
  spinunlock();
  // If doing synchronous clears (ie SW_MODE) return all counters else just ones in gress
  if (kSynchronousStatefulCounterClear) return local_cntrs_in_gress[0] | local_cntrs_in_gress[1];
  else if ((gress_mask & 3) == 3) return local_cntrs_in_gress[0]  | local_cntrs_in_gress[1]; // Ing/Egr
  else if ((gress_mask & 2) == 2) return local_cntrs_in_gress[1]; // Egr only
  else if ((gress_mask & 1) == 1) return local_cntrs_in_gress[0]; // Ing only
  else return 0;
}
uint8_t MauStatefulCounters::counters_in_cntr_gress(int cntr) {
  RMT_ASSERT((cntr >= 0) && (cntr < kNumStatefulCounters));
  std::array<uint8_t, 2> local_cntrs_in_gress{};
  spinlock();
  local_cntrs_in_gress[0] = cntrs_in_gress_[0];
  local_cntrs_in_gress[1] = cntrs_in_gress_[1];
  spinunlock();
  // If doing synchronous clears (ie SW_MODE) return all counters else just ones in gress
  if (kSynchronousStatefulCounterClear) return local_cntrs_in_gress[0] | local_cntrs_in_gress[1];
  else if (((local_cntrs_in_gress[0] >> cntr) & 1) == 1) return local_cntrs_in_gress[0];
  else if (((local_cntrs_in_gress[1] >> cntr) & 1) == 1) return local_cntrs_in_gress[1];
  else return 0;
}
uint8_t MauStatefulCounters::counters_in_lt_gress(int lt) {
  RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
  std::array<uint8_t, 2> local_cntrs_in_gress{};
  spinlock();
  local_cntrs_in_gress[0] = cntrs_in_gress_[0];
  local_cntrs_in_gress[1] = cntrs_in_gress_[1];
  spinunlock();
  // If doing synchronous clears (ie SW_MODE) return all counters else just ones in gress
  if (kSynchronousStatefulCounterClear) return local_cntrs_in_gress[0] | local_cntrs_in_gress[1];
  else if (((lts_in_gress_[0] >> lt) & 1) == 1) return local_cntrs_in_gress[0];
  else if (((lts_in_gress_[1] >> lt) & 1) == 1) return local_cntrs_in_gress[1];
  else return 0;
}

void MauStatefulCounters::stateful_ctl2_callback(uint32_t counter) {
  int width = mau_stateful_log_counter_ctl2_.slog_instruction_width(counter);
  RMT_ASSERT( width < 5 );

  word_portion_width_[counter] = kSramAddressWidth + kMaxSubwordBits - width;
  int extra_bit_pos            = word_portion_width_[counter] + kStatefulAddrVpnBits;
  extra_bit_mask_[counter]     = 1 << extra_bit_pos;
  no_extra_bit_mask_[counter]  = (1 << extra_bit_pos) - 1;
  word_portion_mask_[counter]  = (1 << word_portion_width_[counter]) - 1 ;

}
void MauStatefulCounters::stateful_counter_clear_callback() {
  uint32_t reg = mau_stateful_log_counter_clear_.mau_stateful_log_counter_clear();
  for (int i = 0; i < kNumAlus; i++) {
    if ((reg & (1u<<i)) != 0u) reset_stateful_counter(i);
  }
}
void MauStatefulCounters::stateful_ixbar_map_callback(uint32_t g01, uint32_t lr, uint32_t pushpop) {
  if (ctor_running_) return;
  refresh_lt_cntr_gress();
}
void MauStatefulCounters::ad_thread_change_callback(uint32_t ie, uint32_t repl) {
  if (ctor_running_) return;
  RMT_ASSERT((ie == 0) || (ie == 1));
  RMT_ASSERT((repl == 0) || (repl == 1));
  uint16_t repl0 = adr_dist_table_thread_.adr_dist_table_thread(ie,0);
  uint16_t repl1 = adr_dist_table_thread_.adr_dist_table_thread(ie,1);
  if (repl0 != repl1) return; // Wait till stable
  lts_in_gress_[ie] = repl0;
  refresh_lt_cntr_gress();
}



bool MauStatefulCounters::is_counter_for_lt_being_cleared(int lt) {
  for (int ev = 0; ev < 2; ev++) {
    uint32_t reg = mau_stateful_log_ctl_ixbar_map_.mau_stateful_log_ctl_ixbar_map(lt / 8,0,ev);
    uint8_t field = (reg >> ((lt % 8) * 3)) & 0x7;
    bool enabled = 0x4 & field;
    int counter  = field & 0x3;
    int counter_type = mau_stateful_log_counter_ctl2_.slog_counter_function(counter);
    if ((enabled) && (counter_type == 4)) {
      RMT_LOG(RmtDebug::verbose(),
              "MauStatefulCounters::is_counter_for_lt_being_cleared() LT=%d counter=%d f=0x%x\n",
              lt, counter, cntr_flags_[counter]);
    }
    if ((enabled) && (counter_type == 4) && (is_clear_counter_active(counter)))
      return true;
  }
  return false;
}
void MauStatefulCounters::continue_clear_till(uint64_t active_counters, uint64_t T_stop) {
  // Find tickable SCCs
  // Find T_clear_next_min of all tickable SCCs
  // Maintain array of associated enabled ALUs for each tickable SCC
  // Maintain array of LTs for each tickable SCC
  //
  // Set T = T_next_clear_min
  // Loop T while T <= T_stop
  //    If SCCs is tickable
  //      tick_clear_counter(counter, T)
  //      add ALUs associated with this SCC to ALU_distrib
  //    If ALU_distrib != 0
  //      reset_backend
  //      get addrs for each ALU onto row
  //      distribute via recursive steps

  // Get handle on MauDependencies so we can figure out if LTs are ingress/egress
  MauDependencies *deps = mau()->mau_dependencies();

  static_assert( (kNumAlus <= 8), "ALU bitmask must fit in uint8_t");
  static_assert( (kLogicalTables <= 16), "LT bitmask must fit in uint16_t");
  std::array< uint8_t,  kNumStatefulCounters> alus_by_counter;
  std::array< uint16_t, kNumStatefulCounters> lts_by_counter;
  std::array< uint16_t, kNumAlus>             lts_by_alu;
  uint64_t T_clear_next_min = kTmax;

  for (int counter = 0; counter < kNumStatefulCounters; ++counter) {
    int counter_type = mau_stateful_log_counter_ctl2_.slog_counter_function(counter);
    // Find all active Stateful Clear Counters (SCCs)
    if ((counter_type == 4) && (is_clear_counter_active(counter)) && ((active_counters>>counter)&1)) {
      // Maintain minimum value of T_clear_next_
      if (T_clear_next_[counter] < T_clear_next_min)
        T_clear_next_min = T_clear_next_[counter];
    }
  }
  // Bail if nothing to clear
  if (T_clear_next_min == kTmax) return;


  // Find out what ALUs get clear for each counter
  for (int counter = 0; counter < kNumStatefulCounters; ++counter) {
    alus_by_counter[counter] = 0;
    lts_by_counter[counter] = 0;

    int counter_type = mau_stateful_log_counter_ctl2_.slog_counter_function(counter);
    // Find all active Stateful Clear Counters (SCCs) again
    if ((counter_type == 4) && (is_clear_counter_active(counter)) && ((active_counters>>counter)&1)) {
      // Find out which ALUs are associated with this SCC
      for (int alu = 0; alu < kNumAlus; ++alu) {
        if ((mau_stateful_log_counter_oxbar_map_.stateful_log_counter_oxbar_enable(alu) == 1) &&
            (mau_stateful_log_counter_oxbar_map_.stateful_log_counter_oxbar_ctl(alu) == counter)) {
          alus_by_counter[counter] |= 1<<alu; // OR in ALUs
        }
      }
      // Find out which LTs are associated with this SCC
      for (int lt = 0; lt < kLogicalTables; ++lt) {
        for (int ev = 0; ev < 2; ++ev) {
          // First map from logical table space into counter space
          uint32_t reg = mau_stateful_log_ctl_ixbar_map_.
              mau_stateful_log_ctl_ixbar_map(lt / 8,0,ev);
          uint8_t field = (reg >> ((lt % 8) * 3)) & 0x7;
          bool enabled = 0x4 & field;
          int lt_counter  = field & 0x3;
          if ((enabled) && (counter == lt_counter)) {
            lts_by_counter[counter] |= 1<<lt;
          }
        }
      }
    }
  }

  // So at least one counter to clear
  // Now we cycle sending clears till all counters overflow (or we hit T_stop)
  uint64_t T = T_clear_next_min;
  RMT_LOG(RmtDebug::verbose(),
          "MauStatefulCounters::continue_clear_till START T=%" PRId64 "\n", T);

  while (T <= T_stop) {

    int counters_active = 0;
    int counters_ticked = 0; // Stop when no counters get ticked (all overflowed)
    uint8_t alu_distrib = 0; // Track what ALUs to distribute to (and LTs)
    for (int alu = 0; alu < kNumAlus; ++alu) lts_by_alu[alu] = 0;

    for (int counter = 0; counter < kNumStatefulCounters; ++counter) {
      int counter_type = mau_stateful_log_counter_ctl2_.slog_counter_function(counter);
      // Find all tickable SCCs once more - now also check active at T
      if ((counter_type == 4) && (is_clear_counter_active(counter, T)) && ((active_counters>>counter)&1)) {

        counters_active++;

        // Tick SCC
        if (tick_clear_counter(counter, T)) counters_ticked++;

        uint8_t alus = alus_by_counter[counter];
        // Add ALUs associated with this SCC to alu_distrib
        alu_distrib |= alus;
        // And for each ALU track associated LTs
        for (int alu = 0; alu < kNumAlus; ++alu) {
          if (((alus >> alu) & 1) == 1)
            lts_by_alu[alu] |= lts_by_counter[counter];
        }
      }
    }

    if ((alu_distrib != 0) && (counters_ticked > 0)) {
      // At least one ALU needs to see a ticked value
      int lt0 = -1;

      for (int alu = 0; alu < kNumAlus; ++alu) {
        // Use *first* LT in list for ALU
        int lt = __builtin_ffs(lts_by_alu[alu]) - 1;

        if ((((alu_distrib >> alu) & 1) == 1) && (lt >= 0)) {
          // Pick first LT for call to do_stateful_clear
          if (lt0 < 0) lt0 = lt;
          // Call get_meter_addr to splice in ticked value
          uint32_t maddr = get_meter_addr(alu, 0u, T);
          // Place addr onto correct row for distribution
          uint32_t mrow = 1u << MauMeterAlu::get_meter_alu_logrow_index(alu);
          RMT_LOG(RmtDebug::verbose(),
                  "MauStatefulCounters::continue_clear_till T=%" PRId64 " "
                  "Addr=0x%08x Alu=%d LT=%d\n", T, maddr, alu, lt);
          mau()->mau_addr_dist()->distrib_meter_addresses(lt, deps->is_ingress_lt(lt),
                                                          maddr, mrow);
        }
      }
      // Call correct steps to get addr distributed
      if (lt0 >= 0) mau()->do_stateful_clear(lt0, T);

      // Must reset backend each time
      mau()->reset_backend();
    }

    // If any counters were active must reset them too
    if (counters_active > 0) reset_resources();

    // If no counters ticked this cycle bail out - we're done!
    if (counters_ticked == 0) break;


    T++;
  } // while (T <= T_stop)

  RMT_LOG(RmtDebug::verbose(),
          "MauStatefulCounters::continue_clear_till STOP T=%" PRId64 "\n", T_stop);
}

void MauStatefulCounters::start_clear_at(int counter, uint64_t T) {
  RMT_LOG(RmtDebug::verbose(),
          "MauStatefulCounters::start_clear_at: Will start clearing "
          "counter %d at T=%" PRId64 "\n", counter, T);
  if (is_clear_counter_running(counter, T)) {
    // A clear whilst already clearing maybe ignored - but WARN
    RMT_LOG(RmtDebug::warn(),
            "MauStatefulCounters::start_clear_at: "
            "Counter %d is already being cleared - new clear will be "
            "ignored unless counter overflows at exactly T=%" PRId64 "\n",
            counter, T);
    RMT_ASSERT(T == T_clear_next_[counter]);
  }
  T_clear_next_[counter] = T;
  set_clear_counter_start_pending(counter);
}
void MauStatefulCounters::maybe_synchronous_clear(bool lock) {
  // In DV_MODE PHV arrival triggers clearing
  // In SW_MODE clearing occurs synchronously
  if (MauStatefulCounters::kSynchronousStatefulCounterClear) {
    // Lock only when called from push_pop_stateful_instr
    if (lock) mau()->lock_resources();
    continue_clear_till((1<<kNumStatefulCounters)-1,kTmax); // Clear all counters
    if (lock) mau()->unlock_resources();
  }
}

void MauStatefulCounters::push_pop_stateful_instr(bool push, int counter,
                                                  uint32_t incr, uint64_t T) {
  uint8_t active_cntrs = counters_in_cntr_gress(counter);
  RMT_LOG(RmtDebug::verbose(),
          "MauStatefulCounters::push_pop_stateful_instr: Push=%d "
          "Counter=%d Incr=%d T=%" PRId64 ": ActiveCounters=0x%x\n",
          push, counter, incr, T, active_cntrs);
  mau()->lock_resources(); // Get MAU all to ourself

  // Clear active counters up to time T-1
  if (T > 0) continue_clear_till(active_cntrs, T-1);
  // Later logic this func will handle clearing at T

  int  counter_type = mau_stateful_log_counter_ctl2_.slog_counter_function(counter);
  bool pop = !push;
  bool invalid = false, overflow = false, underflow = false;

  switch ( counter_type ) {
    case 0: // disabled
      invalid = true;
      break;
    case 1:  // stateful logging
      if (push) { // only push events tick the logging counter
        for (uint32_t i = 0; i < incr; i++)
          (void)tick_logging(counter, T);
      }
      else invalid = true;
      break;
    case 2: // FIFO
      if (pop) {
        for (uint32_t i = 0; i < incr; i++) {
          if (!pop_fifo(counter,false,T)) underflow = true;
        }
      } else if (push) {
        for (uint32_t i = 0; i < incr; i++) {
          if (!push_fifo(counter,false,T)) overflow = true;
        }
      }
      break;
    case 3: // Stack
      if (pop) {
        for (uint32_t i = 0; i < incr; i++) {
          if (!pop_stack(counter,false,T)) underflow = true;
        }
      } else if (push) {
        for (uint32_t i = 0; i < incr; i++) {
          if (!push_stack(counter,false,T)) overflow = true;
        }
      }
      break;
    case 4: // Bloom filter clear - handle later
      if (pop) invalid = true;
      break;
    default:
      invalid = true;
      break;
  }

  // Now that other counter types have been updated with this
  // instruction at time T, we move on all clear counters to T.
  continue_clear_till(active_cntrs, T);

  // And if this op was a push to one of the clear counters we
  // handle it now, arranging to start clearing at T+1
  if ((counter_type == 4) && (push)) {
    start_clear_at(counter, T+1);
  }


  // Report errors
  const char *opstr = (push) ?"PUSH" :"POP";
  const char *types[] = {
    "disabled", "logging", "fifo", "stack", "clear", "inval5", "inval6", "inval7"
  };
  if (invalid) {
    RMT_LOG(RmtDebug::error(kRelaxSwPushPopInvalidErrors),
            "MauStatefulCounters::push_pop_stateful_instr "
            "Counter %d of type %d(%s) is INVALID for %s operation (T=%" PRId64 ")\n",
            counter, counter_type, types[counter_type], opstr, T);
    if (!kRelaxSwPushPopInvalidErrors) { THROW_ERROR(-2); } // For DV
  }
  if (overflow || underflow) {
    const char *errstr = (overflow) ?"overflowed" :"underflowed";
    RMT_LOG(RmtDebug::error(kRelaxSwPushPopOverflowUnderflowErrors),
            "MauStatefulCounters::push_pop_stateful_instr "
            "Counter %d of type %d(%s) %s on %s operation (T=%" PRId64 ")\n",
            counter, counter_type, types[counter_type], errstr, opstr, T);
    //if (!kRelaxSwPushPopOverflowUnderflowErrors) { THROW_ERROR(-2); } // For DV
  }

  // Maybe loop doing all remaining clearing now (not if DV_MODE)
  maybe_synchronous_clear(false);

  mau()->unlock_resources();
}


void MauStatefulCounters::advance_time(int lt, uint64_t T, const char *why_str, uint32_t why_val) {
  uint8_t active_cntrs = counters_in_lt_gress(lt);
  RMT_LOG(RmtDebug::verbose(),
          "MauStatefulCounters::advance_time: LT=%d T=%" PRId64 " %s 0x%x: ActiveCntrs=0x%x\n",
          lt, T, why_str, why_val, active_cntrs);
  mau()->lock_resources(); // Get MAU all to ourself

  // Clear all counters up to time T
  continue_clear_till(active_cntrs, T);

  mau()->unlock_resources();
}


}
