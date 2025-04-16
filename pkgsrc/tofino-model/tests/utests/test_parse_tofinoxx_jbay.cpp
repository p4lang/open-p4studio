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

#include "test_parse.h"
#include "cntrstack_util.h"
#include <parser-static-config.h>

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;


TEST_F(MCN_CONCAT(BFN_TEST_NAME(ParseTest), UsingConfigBase), CounterStackOldVsNew) {

  // Loop many times verifying that the new CounterStack implementation
  // gives back the same results as the old CounterStack implementation
  // and that that gives back the same results as the built-in impl

  const int      num_iterations  = 50000;
  const int      debug_mod       = 100;
  const uint64_t root_rand       = UINT64_C(0xDAB0D1B0D0B0DEB0);
  const uint64_t debug_rand      = UINT64_C(0); // 0==>use root_rand
  const bool     DEBUG_ITER      = true;
  const bool     CHECK_ERRS      = true;

  parser_->set_log_flags(UINT64_C(0));

  CntrstackUtil cntrstackA(om_, pipe_index(), parser_->parser_index());
  cntrstackA.set_log_flags(UINT64_C(0));

  // Loop doing many iterations
  //
  uint32_t err_cnt = 0u;
  uint64_t fail_rand = UINT64_C(0);
  uint64_t iter_rand = root_rand;

  for (int iteration = 0; iteration < num_iterations; iteration++) {
    int shift_0, shift_a;
    uint32_t hash_0, hash_a;
    int8_t ctr_0, ctr_a, nxt_0, nxt_a;
    bool pnd_0, pnd_a, err_pnd_0, err_pnd_a;
    bool dmod = ((iteration % debug_mod) == 0);

    // Reset cntrstack impls
    parser_->counter_reset();
    cntrstackA.counter_reset();

    iter_rand = tu_->mmix_rand64(iter_rand); // Maybe force iter_rand
    if ((iteration == 0) && (debug_rand != UINT64_C(0))) iter_rand = debug_rand;
    if (DEBUG_ITER && dmod) printf("ITER[%4d] Seed=%016" PRIx64 " Errors=%d "
                                   "[FirstFailSeed=%016" PRIx64 "]\n",
                                   iteration, iter_rand, err_cnt, fail_rand);
    uint64_t loop_rand = iter_rand;

    // Setup 16 random CounterInit entries...
    for (int ci = 0; ci < RmtDefs::kParserCounterInitEntries; ci++) {
      // Always add_to_stack on JBay
      uint8_t a2s = RmtObject::is_jbayXX() ?1 :static_cast<uint8_t>(loop_rand & 1);
      loop_rand = tu_->mmix_rand64(loop_rand);
      parser_->set_counter_init(ci, // Program random add/maskW/rotate/max/source
                                static_cast<uint8_t>((loop_rand >>  8) & 0xFF),
                                static_cast<uint8_t>((loop_rand >> 16) & 0x07),
                                static_cast<uint8_t>((loop_rand >> 24) & 0x07),
                                static_cast<uint8_t>((loop_rand >> 32) & 0xFF),
                                static_cast<uint8_t>((loop_rand >> 40) & 0x03),
                                a2s);
      cntrstackA.set_counter_init(ci, // Program random add/maskW/rotate/max/source
                                  static_cast<uint8_t>((loop_rand >>  8) & 0xFF),
                                  static_cast<uint8_t>((loop_rand >> 16) & 0x07),
                                  static_cast<uint8_t>((loop_rand >> 24) & 0x07),
                                  static_cast<uint8_t>((loop_rand >> 32) & 0xFF),
                                  static_cast<uint8_t>((loop_rand >> 40) & 0x03),
                                  a2s);
    }
    // And 256 random EarlyActionRam entries (at least the counter relevant bits)
    for (int eai = 0; eai < RmtDefs::kParserStates; eai++) {
      loop_rand = tu_->mmix_rand64(loop_rand);
      parser_->set_counter_ctr_op(eai, static_cast<uint8_t>((loop_rand >> 8) & 0x3));
      parser_->set_counter_load_addr(eai, static_cast<uint8_t>((loop_rand >> 16) & 0xFF));
      parser_->set_counter_stack_upd_w_top(eai, ((loop_rand & 1) == 1));
      // Only set up push separately on JBay (included in op on WIP)
      if (RmtObject::is_jbayXX())
        parser_->set_counter_stack_push(eai, static_cast<uint8_t>((loop_rand & 2) == 2));

      cntrstackA.set_counter_ctr_op(eai, static_cast<uint8_t>((loop_rand >> 8) & 0x3));
      cntrstackA.set_counter_load_addr(eai, static_cast<uint8_t>((loop_rand >> 16) & 0xFF));
      cntrstackA.set_counter_stack_upd_w_top(eai, ((loop_rand & 1) == 1));
      // Only set up push separately on JBay (included in op on WIP)
      if (RmtObject::is_jbayXX())
        cntrstackA.set_counter_stack_push(eai, static_cast<uint8_t>((loop_rand & 2) == 2));
    }

    // Then do 256 calls to counter_handle on 0/a/b implementations
    // (using all randomly programmed entries above)
    for (int eai = 0; eai < RmtDefs::kParserStates; eai++) {
      loop_rand = tu_->mmix_rand64(loop_rand);
      uint8_t f8[4] = { static_cast<uint8_t>((loop_rand >>  0) & 0xFF),
                        static_cast<uint8_t>((loop_rand >>  8) & 0xFF),
                        static_cast<uint8_t>((loop_rand >> 16) & 0xFF),
                        static_cast<uint8_t>((loop_rand >> 24) & 0xFF) };
      int shift = static_cast<int>( (loop_rand >> 32) & 0xFF);
      shift_0 = shift_a =  shift;
      // Run our different implementations
      parser_->counter_handle(eai, f8[3], f8[2], f8[1], f8[0], &shift_0);
      cntrstackA.counter_handle(eai, f8[3], f8[2], f8[1], f8[0], &shift_a);

      // And check state of 0/a/b counter
      parser_->counter_info_get(&ctr_0, &nxt_0, &pnd_0, &err_pnd_0);
      cntrstackA.counter_info_get(&ctr_a, &nxt_a, &pnd_a, &err_pnd_a);
      hash_0 = parser_->cntrstack_hash();
      hash_a = cntrstackA.cntrstack_hash();

      bool mismatch = ((ctr_a != ctr_0) || (nxt_a != nxt_0) || (hash_a != hash_0));
      if ((mismatch) || (iter_rand == debug_rand)) {
        if ((mismatch) && (fail_rand == UINT64_C(0))) fail_rand = iter_rand;
        const char *ops[4] = { "Add ","Pop ","LdI ","LdCR" };
        const char *op = ops[parser_->counter_ctr_op(eai) & 3];
        printf("ITER[%4d] Seed=%016" PRIx64 " nCycles=%2d  OP=%s ctrA=%d,ctr0=%d "
               "nxtA=%d,nxt0=%d hashA=0x%08x,hash0=0x%08x  %s [FirstFailSeed=%016" PRIx64 "]\n",
               iteration, iter_rand, eai, op, ctr_a, ctr_0, nxt_a, nxt_0, hash_a, hash_0,
               (mismatch) ?"MISMATCH" :"ok", fail_rand);
        if (mismatch) {
          err_cnt++;
          break;
        }
      }
    }

    // Final check state of 0/a/b counter/CounterStack agree
    parser_->counter_info_get(&ctr_0, &nxt_0, &pnd_0, &err_pnd_0);
    cntrstackA.counter_info_get(&ctr_a, &nxt_a, &pnd_a, &err_pnd_a);
    hash_0 = parser_->cntrstack_hash();
    hash_a = cntrstackA.cntrstack_hash();
    if (CHECK_ERRS) {
      EXPECT_EQ(ctr_a, ctr_0);
      EXPECT_EQ(nxt_a, nxt_0);
      EXPECT_EQ(pnd_a, pnd_0);
      EXPECT_EQ(err_pnd_a, err_pnd_0);
      EXPECT_EQ(hash_a, hash_0);
      EXPECT_EQ(shift_a, shift_0);
    }
    EXPECT_EQ(0u, err_cnt);

  }  // for (int iteration = 0; iteration < num_iterations; iteration++)

}


}
