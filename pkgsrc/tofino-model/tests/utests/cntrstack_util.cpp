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

#include <utests/test_util.h>

#include "cntrstack_util.h"

#undef  RMT_LOG
#define RMT_LOG(FLAGS,...) if (_do_printf_) printf(__VA_ARGS__);
#define FIX_BUGS

namespace MODEL_CHIP_TEST_NAMESPACE {

CntrstackUtil::CntrstackUtil(RmtObjectManager *om, int pipe, int parser_index)
  : MODEL_CHIP_NAMESPACE::Parser(om, pipe, parser_index, 0, RmtDefs::kParser_Config[parser_index]) {
    kRelaxExtractionCheck = true;
}
CntrstackUtil::~CntrstackUtil() {
}



void CntrstackUtil::counter_reset() {
  counter_ = counter_next_ = 0;
  counter_pending_ = counter_range_err_pending_ = false;
  cntrstack_reset();
}
// Note #ifndef FIX_BUGS (and without EXTRA EXTRA)
// this is the original Tofino/JBay counter_handle impl.
// The FIX_BUGS is to fix bugs in the original JBay
// implementation to match the new microOP impl
//
bool CntrstackUtil::counter_handle(int index,
                                      uint8_t f8_3, uint8_t f8_2,
                                      uint8_t f8_1, uint8_t f8_0, int *shift) {
  RMT_ASSERT(shift != nullptr);
  uint16_t f16 = static_cast<uint16_t>(f8_1 << 8) | static_cast<uint16_t>(f8_0 << 0);
  uint8_t  pcnt_addr;
  int8_t   pcnt_addr_int, incr, counter_prev;
  bool     incr_on_push = false;

  bool ok = true;
  if (counter_range_err_pending_) ok = false;
  if (index < 0) return ok; // Just harvest pending_err

  switch (counter_ctr_op(index)) {

    case kCounter2LoadFromCntrRam:
      // Use immediate value as addr into counter-init-ram

      // XXX: Mask addr (actually reduce addr mod kCounterInitEntries)
      // to match RTL behaviour - but report error if addr >= kCounterInitEntries
      pcnt_addr = counter_load_addr(index);
      if (pcnt_addr >= kCounterInitEntries) {
        pcnt_addr %= kCounterInitEntries;
        RMT_LOG(RmtDebug::error(kRelaxExtractionCheck),
                "ParserOld::counter_handle(index=%d) LoadFromCntrRam "
                "but addr %d exceeds CounterRam size %d so masking to %d\n",
                index, counter_load_addr(index), kCounterInitEntries, pcnt_addr);
        if (!kRelaxExtractionCheck) { THROW_ERROR(-2); }
      }
      if (pcnt_is_valid_counter(pcnt_addr, f8_3, f8_2, f16)) {

        pcnt_addr_int = static_cast<int8_t>(pcnt_addr);
#ifdef FIX_BUGS
        counter_prev = counter_;
        if (counter_pending_) counter_prev = counter_next_; // EXTRA EXTRA
#else
        counter_prev = counter_next_; // Always push prev val to stack
#endif
        // Counter pends for a cycle in this case
        counter_range_err_pending_ = false; counter_pending_ = true;
        // Ensure counter_ not eq_0, lt_0 for this cycle though
        counter_ = 127; // EXTRA EXTRA OLD LOGIC USED 1 **NOT** 127

        // Load with calculated value from CounterRam
        counter_next_ = pcnt_calculate_counter(pcnt_addr, f8_3, f8_2, f16);
        // Counter stack push NOT delayed (23/2/2017)
        // XXX    - push *previous* value of counter but
        // XXX-II - maybe add on counter *index* (if ctr_stack_upd_w_top)
#ifdef FIX_BUGS
        incr_on_push = cntrstack_maybe_push(index, counter_prev, 0); // EXTRA EXTRA
        cntrstack_increment(index, pcnt_addr_int); // EXTRA EXTRA
#else
        incr_on_push = cntrstack_maybe_push(index, counter_prev, pcnt_addr_int);
#endif
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserCounterStack),
                "ParserOld::counter_load_from_cntr_ram(index=%d,prev=%d,pushed=%d,counter=%d)\n",
                index, counter_prev,
                static_cast<int8_t>(counter_prev + ((incr_on_push)?pcnt_addr_int:0)),
                counter_next_);
      } else {
        counter_range_err_pending_ = true;
      }
      break;

    case kCounter2LoadImmediate:
#ifdef FIX_BUGS
      if (counter_pending_) counter_ = counter_next_; // EXTRA EXTRA
#endif
      counter_range_err_pending_ = false; counter_pending_ = false;
      counter_prev = counter_;
#ifdef FIX_BUGS
      // Load immediate value directly as new counter value
      counter_ = counter_load_imm_int(index);
      // XXX - push *previous* value of counter but
      // maybe add on new counter value (if ctr_stack_upd_w_top)
      incr_on_push = cntrstack_maybe_push(index, counter_prev, 0); // EXTRA EXTRA
      // Apply increment *to counter stack* ALSO EXTRA EXTRA
      cntrstack_increment(index, counter_); // EXTRA EXTRA
#else
      // Load immediate value directly as new counter value
      counter_ = counter_load_imm_int(index);
      // XXX - push *previous* value of counter but
      // maybe add on new counter value (if ctr_stack_upd_w_top)
      incr_on_push = cntrstack_maybe_push(index, counter_prev, counter_);
#endif
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserCounterStack),
              "ParserOld::counter_load_imm(index=%d,prev=%d,pushed=%d,counter=%d)\n",
              index, counter_prev,
              static_cast<int8_t>(counter_prev + ((incr_on_push)?counter_:0)),
              counter_);
      break;

    case kCounter2AddOnly:
      // Set counter_ from next value if there is one pending
      if (counter_pending_) counter_ = counter_next_;
      counter_range_err_pending_ = false; counter_pending_ = false;
      counter_prev = counter_;

      // Use immediate value directly as counter increment
      incr = counter_load_imm_int(index);
      // Apply increment *to counter stack*
      cntrstack_increment(index, incr);
      // Apply increment to counter itself
      counter_ += incr;
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserCounterStack),
              "ParserOld::counter_add_only(index=%d,prev=%d,incr=%d,counter=%d)\n",
              index, counter_prev, incr, counter_);
      break;

    case kCounter2PopStackAdd:
      // Set counter_ from next value if there is one pending
      if (counter_pending_) counter_ = counter_next_;
      counter_range_err_pending_ = false; counter_pending_ = false;

      // Pop counter stack (only on JBay, returns unmodified counter on Tofino)
      counter_prev = cntrstack_pop(index, counter_);
      // Use immediate value directly as counter increment
      incr = counter_load_imm_int(index);
      // Apply increment to popped counter value only
      counter_ = counter_prev + incr;
#ifdef FIX_BUGS
      // Apply increment *to counter stack* ALSO EXTRA EXTRA
      cntrstack_increment(index, incr); // EXTRA EXTRA
#endif
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserCounterStack),
              "ParserOld::counter_pop_stack(index=%d,popped=%d,incr=%d,counter=%d)\n",
              index, counter_prev, incr, counter_);
      break;

    default: RMT_ASSERT(0); break; // EXTRA EXTRA
  }
  return ok;
}


}
