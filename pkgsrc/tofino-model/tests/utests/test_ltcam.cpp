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
#include <iostream>
#include <string>
#include <array>
#include <cassert>

#include "gtest.h"

#include "tcam_util.h"
#include <model_core/model.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

bool ltcam_print = true;
bool ltcam_print_lots = false;

using namespace std;
using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(LtcamTest),SingleTcam) {
  GLOBAL_MODEL->Reset();
  if (ltcam_print) RMT_UT_LOG_INFO("test_ltcam_single_tcam()\n");
  int           chip = 202; // 2 stages
  int           pipe = 0;
  TestUtil      tu(GLOBAL_MODEL.get(), chip, pipe);
  TcamArrayWrap tcam_array(&tu);

  tu.set_stage_default_regs(pipe, 0);
  tu.set_stage_default_regs(pipe, 1);

  // DEBUG setup....................
  uint64_t pipes, stages, types, rows_tabs, cols, flags;
  pipes = ONE; stages = HI;  types = ALL; rows_tabs = FEW; cols = TOP; flags = NON;
  pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
  // Just to stop compiler complaining about unused vars
  flags = FEW;
  tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
  tu.quieten_p4_log_flags(pipes);

  const uint64_t root_seed      = UINT64_C(0x1234567890);
  const int      num_iterations = 10;
  const int      num_lookups    = 20;

  // Flags for final (only) tcam
  uint32_t tcflags = TcamCtl::kMatchLoFinal|TcamCtl::kMatchHiFinal;
  uint8_t  ltcams = 1<<4; // Bitmask
  uint64_t seed = root_seed;
  int      num_mismatches = 0;
  int      debug = 2;

  if (debug == 0)
    tu.get_objmgr()->update_log_type_levels(pipes, ALL, RMT_LOG_TYPE_P4, ALL, UINT64_C(0));
  tcam_array.set_debug(debug);

  for (int iteration = 0; iteration < num_iterations; iteration++) {
    // Even iterations configure all LTCAMs as ingress, odd iterations all egress
    uint8_t ing_ltcams =  ((iteration % 2) == 0) ?0xFF :0x00;
    uint8_t egr_ltcams =  ((iteration % 2) == 0) ?0x00 :0xFF;

    for (int col = 0; col <= 1; col++) {

      for (int row = 0; row < 12; row++) {

        // Setup random w0/w1 config in LTCAM - just use *one* TCAM
        tcam_array.set_seed(seed);
        tcam_array.configure(ing_ltcams, egr_ltcams);
        tcam_array.configure_tcams(col,col,row,row,tcflags,ltcams,UINT64_C(0));
        tcam_array.install();

        for (int lookup = 0; lookup < num_lookups; lookup++) {
          // Even lookups lookup using ingress thread, odd use egress thread
          uint8_t thread = ((lookup % 2) == 0) ?TcamConsts::kIngress :TcamConsts::kEgress;

          uint64_t lookup_seed = tcam_array.get_lookup_seed();
          // Then do random data lookups in LTCAM comparing against RefModel
          if (!tcam_array.lookup_seed(thread, lookup_seed, debug)) {
            num_mismatches++;
            printf("TCAM[%d,%d] Lookup %d failed - retrying with logging on "
                   "(seed=0x%" PRIx64 ")\n", row, col, lookup, seed);
            tu.update_log_flags(pipes, stages, types, rows_tabs, cols, ALL, ALL);
            (void)tcam_array.lookup_seed(thread, lookup_seed, 8); // 9=>maximal debug
            tu.quieten_log_flags();
            break;
          }
        } // for (int lookup = 0; lookup < num_lookups; lookup++)

        seed = tu.mmix_rand64(seed);
      } // for (int row = 0; row < 12; row++)
    } // for (int col = 0; col <= 1; col++)

  } // for (int iteration = 0; iteration < num_iterations; iteration++)
  EXPECT_EQ(0, num_mismatches);
}




TEST(BFN_TEST_NAME(LtcamTest),TwoTcamsTwoGresses) {
  GLOBAL_MODEL->Reset();
  if (ltcam_print) RMT_UT_LOG_INFO("test_ltcam_two_tcams_two_gresses()\n");
  int           chip = 202; // 2 stages
  int           pipe = 0;
  TestUtil      tu(GLOBAL_MODEL.get(), chip, pipe);
  TcamArrayWrap tcam_array(&tu);

  tu.set_stage_default_regs(pipe, 0);
  tu.set_stage_default_regs(pipe, 1);

  // DEBUG setup....................
  uint64_t pipes, stages, types, rows_tabs, cols, flags;
  pipes = ONE; stages = HI;  types = ALL; rows_tabs = FEW; cols = TOP; flags = NON;
  pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
  // Just to stop compiler complaining about unused vars
  flags = FEW;
  tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
  tu.quieten_p4_log_flags(pipes);

  const uint64_t root_seed      = UINT64_C(0x1234567890);
  const int      num_iterations = 10;
  const int      num_lookups    = 20;

  // Flags for final (only) tcam
  uint32_t tcflags = TcamCtl::kMatchLoFinal|TcamCtl::kMatchHiFinal;
  uint8_t  ing_ltcams = 1<<4; // Bitmask
  uint8_t  egr_ltcams = 1<<3; // Bitmask
  uint64_t seed = root_seed;
  int      num_mismatches = 0;
  int      debug = 8;

  if (debug == 0)
    tu.get_objmgr()->update_log_type_levels(pipes, ALL, RMT_LOG_TYPE_P4, ALL, UINT64_C(0));
  tcam_array.set_debug(debug);

  for (int iteration = 0; iteration < num_iterations; iteration++) {

    for (int col = 0; col <= 1; col++) {

      for (int row = 0; row < 12; row++) {

        // Setup random w0/w1 config in LTCAM
        tcam_array.set_seed(seed);
        tcam_array.configure(ing_ltcams, egr_ltcams);
        // Setup 2 tcams. First X,Y using ing LTCAM. Second 1-X,11-Y using egr TCAM.
        tcam_array.configure_tcams(col,col,row,row,tcflags,
                                   ing_ltcams,UINT64_C(0));
        tcam_array.configure_tcams(1-col,1-col,11-row,11-row,tcflags,
                                   egr_ltcams,UINT64_C(0));
        tcam_array.install();

        for (int lookup = 0; lookup < num_lookups; lookup++) {
          uint64_t lookup_seed = tcam_array.get_lookup_seed();

          for (int ie = 0; ie < 4; ie++) {
            // Do 4 identical lookups 0=no thread, 1=ingress, 2=egress, 3=both
            uint8_t thread = static_cast<uint8_t>(ie);

            // Then do random data lookups in LTCAM comparing against RefModel
            if (!tcam_array.lookup_seed(thread, lookup_seed, debug)) {
              num_mismatches++;
              printf("TCAM[%d,%d] Lookup %d,%d failed - retrying with logging on "
                     "(seed=0x%" PRIx64 ") (lookup_seed=0x%" PRIx64 ")\n",
                     row, col, lookup, ie, seed, lookup_seed);
              tu.update_log_flags(pipes, stages, types, rows_tabs, cols, ALL, ALL);
              (void)tcam_array.lookup_seed(thread, lookup_seed, 8); // 9=>maximal debug
              tu.quieten_log_flags();
              break;
            }

          } // for (int ie = 0; ie < 4; ie++)
        } // for (int lookup = 0; lookup < num_lookups; lookup++)

        seed = tu.mmix_rand64(seed);
      } // for (int row = 0; row < 12; row++)
    } // for (int col = 0; col <= 1; col++)

  } // for (int iteration = 0; iteration < num_iterations; iteration++)
  EXPECT_EQ(0, num_mismatches);
}


TEST(BFN_TEST_NAME(LtcamTest),SingleChain) {
  GLOBAL_MODEL->Reset();
  if (ltcam_print) RMT_UT_LOG_INFO("test_ltcam_single_chain()\n");
  int           chip = 202; // 2 stages
  int           pipe = 0;
  TestUtil      tu(GLOBAL_MODEL.get(), chip, pipe);
  TcamArrayWrap tcam_array(&tu);

  tu.set_stage_default_regs(pipe, 0);
  tu.set_stage_default_regs(pipe, 1);

  // DEBUG setup....................
  uint64_t pipes, stages, types, rows_tabs, cols, flags;
  pipes = ONE; stages = HI;  types = ALL; rows_tabs = FEW; cols = TOP; flags = NON;
  pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
  // Just to stop compiler complaining about unused vars
  flags = FEW;
  tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
  tu.quieten_p4_log_flags(pipes);

  const uint64_t root_seed         = UINT64_C(0x1234567890);
  const uint64_t debug_seed        = UINT64_C(0x63fea7514b67abb1);
  const uint64_t debug_lookup_seed = UINT64_C(0x62354cda6226d1f3);
  const int      num_iterations    = 5;
  const int      num_lookups       = 20;

  uint8_t  ltcams = 1<<4; // Bitmask
  uint64_t seed = root_seed;
  int      num_mismatches = 0;
  int      debug = 0;

  if (debug == 0)
    tu.get_objmgr()->update_log_type_levels(pipes, ALL, RMT_LOG_TYPE_P4, ALL, UINT64_C(0));
  tcam_array.set_debug(debug);

  for (int iteration = 0; iteration < num_iterations; iteration++) {

    // Even iterations configure all LTCAMs as ingress, odd iterations all egress
    uint8_t ing_ltcams = ((iteration % 2) == 0) ?0xFF :0x00;
    uint8_t egr_ltcams = ((iteration % 2) == 0) ?0x00 :0xFF;

    for (int col = 0; col <= 1; col++) {

      for (int row_start = 0; row_start < 12; row_start++) {

        for (int chain_len = 1; chain_len <= 11; chain_len++) {

          int row_end = (row_start + chain_len) % 12;

          int row_hi = (row_start > row_end) ?row_start :row_end;
          int row_lo = (row_start < row_end) ?row_start :row_end;
          EXPECT_NE(row_start, row_end);

          // Set flags to indicate CHAIN OUT for all (non-final) TCAMs in chain.
          // Final *upper* half TCAM may get MATCH OUT later
          // Final *lower* half TCAM may get MATCH OUT later (unless chain
          //  crosses middle in which case it'll get CHAIN OUT instead).
          uint32_t tcflags = TcamCtl::kChainOutput;

          // If crossing middle should set final LO TCAM to CHAIN OUT too
          // (for now also set kChainHiFinal as RefModel expects TCAM[6] ChainOut set)
          if ((row_hi >= 6) && (row_lo <= 5))
            tcflags |= (TcamCtl::kChainLoFinal|TcamCtl::kMatchHiFinal|TcamCtl::kChainHiFinal);
          // If entirely bot half chain, final LO TCAM should MATCH OUT
          else if (row_hi <= 5)                   tcflags |= TcamCtl::kMatchLoFinal;
          // If entirely top half chain, final HI TCAM should MATCH OUT
          else if (row_lo >= 6)                   tcflags |= TcamCtl::kMatchHiFinal;

          // Setup random w0/w1 config in LTCAM
          tcam_array.set_seed(seed);
          tcam_array.configure(ing_ltcams, egr_ltcams);
          // Alternate use of ltcams as ingress/egress
          tcam_array.configure_tcams(col,col,row_start,row_end,tcflags,ltcams,UINT64_C(0));
          tcam_array.install();

          for (int lookup = 0; lookup < num_lookups; lookup++) {
            // Even lookups lookup using ingress thread, odd use egress thread
            uint8_t thread = ((lookup % 2) == 0) ?TcamConsts::kIngress :TcamConsts::kEgress;

            uint64_t lookup_seed = tcam_array.get_lookup_seed();
            int      lookup_debug = debug;

            // Maybe up debug
            if ((seed == debug_seed) && (lookup_seed == debug_lookup_seed)) lookup_debug = 8;

            // Then do random data lookups in LTCAM comparing against RefModel
            printf("TCAM[%d..%d,%d] Lookup %d (seed=0x%" PRIx64 ") (lookup_seed=0x%" PRIx64 ")\n",
                   row_start, row_end, col, lookup, seed, lookup_seed);

            if (!tcam_array.lookup_seed(thread, lookup_seed, lookup_debug)) {
              num_mismatches++;
              printf("TCAM[%d..%d,%d] Lookup %d failed - retrying with logging on "
                     "(chain_len=%d) (seed=0x%" PRIx64 ") (lookup_seed=0x%" PRIx64 ")\n",
                     row_start, row_end, col, lookup, chain_len, seed, lookup_seed);
              tu.update_log_flags(pipes, stages, types, rows_tabs, cols, ALL, ALL);
              (void)tcam_array.lookup_seed(thread, lookup_seed, 8); // 9=>maximal debug
              tu.quieten_log_flags();
              break;
            }
          } // for (int lookup = 0; lookup < num_lookups; lookup++)

          seed = tu.mmix_rand64(seed);

        } // for (int chain_len = 1; chain_len <= 11; chain_len++)
      } // for (int row_start = 0; row_start < 12; row_start++)
    } // for (int col = 0; col <= 1; col++)

  } // for (int iteration = 0; iteration < num_iterations; iteration++)
  EXPECT_EQ(0, num_mismatches);
}


TEST(BFN_TEST_NAME(LtcamTest),DebugOneChain) {
  GLOBAL_MODEL->Reset();
  if (ltcam_print) RMT_UT_LOG_INFO("test_ltcam_debug_one_chain()\n");
  int           chip = 202; // 2 stages
  int           pipe = 0;
  TestUtil      tu(GLOBAL_MODEL.get(), chip, pipe);
  TcamArrayWrap tcam_array(&tu);

  tu.set_stage_default_regs(pipe, 0);
  tu.set_stage_default_regs(pipe, 1);

  // DEBUG setup....................
  uint64_t pipes, stages, types, rows_tabs, cols, flags;
  pipes = ONE; stages = HI;  types = ALL; rows_tabs = FEW; cols = TOP; flags = NON;
  pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
  // Just to stop compiler complaining about unused vars
  flags = FEW;
  tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
  tu.quieten_p4_log_flags(pipes);

  const uint64_t iteration0_seed = UINT64_C(0x63fea7514b67abb1);
  const uint64_t lookup0_seed    = UINT64_C(0x62354cda6226d1f3);

  uint8_t  ltcams = 1<<4; // Bitmask
  uint64_t seed = iteration0_seed;
  uint64_t lookup_seed = lookup0_seed;
  int      num_mismatches = 0;
  int      debug = 8;

  if (debug == 0)
    tu.get_objmgr()->update_log_type_levels(pipes, ALL, RMT_LOG_TYPE_P4, ALL, UINT64_C(0));
  tcam_array.set_debug(debug);

  uint8_t ing_ltcams = 0xFF;
  uint8_t egr_ltcams = 0x00;

  int col = 0;
  int row_start = 1;
  int chain_len = 1;
  int lookup = 4;

  int row_end = (row_start + chain_len) % 12;

  int row_hi = (row_start > row_end) ?row_start :row_end;
  int row_lo = (row_start < row_end) ?row_start :row_end;
  EXPECT_NE(row_start, row_end);

  // Set flags to indicate CHAIN OUT for all (non-final) TCAMs in chain.
  // Final *upper* half TCAM may get MATCH OUT later
  // Final *lower* half TCAM may get MATCH OUT later (unless chain
  //  crosses middle in which case it'll get CHAIN OUT instead).
  uint32_t tcflags = TcamCtl::kChainOutput;

  // If crossing middle should set final LO TCAM to CHAIN OUT too
  // (for now also set kChainHiFinal as RefModel expects TCAM[6] ChainOut set)
  if ((row_hi >= 6) && (row_lo <= 5))
    tcflags |= (TcamCtl::kChainLoFinal|TcamCtl::kMatchHiFinal|TcamCtl::kChainHiFinal);
  // If entirely bot half chain, final LO TCAM should MATCH OUT
  else if (row_hi <= 5)                   tcflags |= TcamCtl::kMatchLoFinal;
  // If entirely top half chain, final HI TCAM should MATCH OUT
  else if (row_lo >= 6)                   tcflags |= TcamCtl::kMatchHiFinal;

  // Setup random w0/w1 config in LTCAM
  tcam_array.set_seed(seed);
  tcam_array.configure(ing_ltcams, egr_ltcams);
  // Alternate use of ltcams as ingress/egress
  tcam_array.configure_tcams(col,col,row_start,row_end,tcflags,ltcams,UINT64_C(0));
  tcam_array.install();

  // Even lookups lookup using ingress thread, odd use egress thread
  uint8_t thread = ((lookup % 2) == 0) ?TcamConsts::kIngress :TcamConsts::kEgress;

  // Then do random data lookups in LTCAM comparing against RefModel
  printf("TCAM[%d..%d,%d] Lookup %d (seed=0x%" PRIx64 ") (lookup_seed=0x%" PRIx64 ")\n",
         row_start, row_end, col, lookup, seed, lookup_seed);

  tu.update_log_flags(pipes, stages, types, rows_tabs, cols, ALL, ALL);

  if (!tcam_array.lookup_seed(thread, lookup_seed, debug)) {
    num_mismatches++;
    printf("TCAM[%d..%d,%d] Lookup %d failed - retrying with logging on "
           "(seed=0x%" PRIx64 ") (lookup_seed=0x%" PRIx64 ")\n",
           row_start, row_end, col, lookup, seed, lookup_seed);
    (void)tcam_array.lookup_seed(thread, lookup_seed, 8); // 9=>maximal debug
  }
  EXPECT_EQ(0, num_mismatches);
  tu.quieten_log_flags();
}


TEST(BFN_TEST_NAME(LtcamTest),MultipleLogicalTcamsPerPhysicalTcam) {
  GLOBAL_MODEL->Reset();
  if (ltcam_print) RMT_UT_LOG_INFO("test_ltcam_multiple_logical_tcams_per_physical_tcam()\n");
  int           chip = 202; // 2 stages
  int           pipe = 0;
  TestUtil      tu(GLOBAL_MODEL.get(), chip, pipe);
  TcamArrayWrap tcam_array(&tu);

  tu.set_stage_default_regs(pipe, 0);
  tu.set_stage_default_regs(pipe, 1);

  // DEBUG setup....................
  int      debug = 0;
  uint64_t pipes, stages, types, rows_tabs, cols, flags;
  pipes = ONE; stages = HI;  types = ALL; rows_tabs = FEW; cols = TOP; flags = NON;
  pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
  // Just to stop compiler complaining about unused vars
  flags = FEW;

  tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
  tu.quieten_p4_log_flags(pipes);

  if (debug == 0)
    tu.get_objmgr()->update_log_type_levels(pipes, ALL, RMT_LOG_TYPE_P4, ALL, UINT64_C(0));
  tcam_array.set_debug(debug);

  const uint64_t root_seed      = UINT64_C(0x1234567890);
  const int      num_iterations = 50;
  const int      num_lookups    = 50;

  // Flags for final (only) tcam
  uint32_t tcflags = TcamCtl::kMatchLoFinal|TcamCtl::kMatchHiFinal;
  uint64_t seed = root_seed;
  int      total_hits = 0;
  int      num_mismatches = 0;

  for (int iteration = 0; iteration < num_iterations; iteration++) {
    // Even iterations configure all LTCAMs as ingress, odd iterations all egress
    uint8_t ing_ltcams =  ((iteration % 2) == 0) ?0xFF :0x00;
    uint8_t egr_ltcams =  ((iteration % 2) == 0) ?0x00 :0xFF;
    // Allow many LTCAMs in the physical TCAM on WIP
    // (NB On Tofino/JBay only highest numbered LTCAM will be used)
    uint8_t ltcams = static_cast<uint8_t>( ((iteration * 999329) + 999331) & 0xFF );
    int     col = iteration % 2;
    int     row = ((iteration * 997019) + 997031) % 12; // Jump about a bit

    // Setup random w0/w1 config in LTCAM - just use *one* TCAM
    tcam_array.set_seed(seed);
    tcam_array.configure(ing_ltcams, egr_ltcams);
    tcam_array.configure_tcams(col,col,row,row,tcflags,ltcams,UINT64_C(0));
    tcam_array.install();

    for (int lookup = 0; lookup < num_lookups; lookup++) {
      // Even lookups lookup using ingress thread, odd use egress thread
      uint8_t thread = ((lookup % 2) == 0) ?TcamConsts::kIngress :TcamConsts::kEgress;

      uint64_t lookup_seed = tcam_array.get_lookup_seed();
      // Then do random data lookups in LTCAM comparing against RefModel
      if (!tcam_array.lookup_seed(thread, lookup_seed, debug)) {
        num_mismatches++;
        printf("TCAM[%d,%d] Lookup %d failed - retrying with logging on "
               "(seed=0x%" PRIx64 ")\n", row, col, lookup, seed);
        tu.update_log_flags(pipes, stages, types, rows_tabs, cols, ALL, ALL);
        (void)tcam_array.lookup_seed(thread, lookup_seed, 8); // 9=>maximal debug
        tu.quieten_log_flags();
        break;
      }
    } // for (int lookup = 0; lookup < num_lookups; lookup++)

    seed = tu.mmix_rand64(seed);
    total_hits += tcam_array.total_hits();

  } // for (int iteration = 0; iteration < num_iterations; iteration++)
  EXPECT_EQ(0, num_mismatches);
  printf("Total LTCAM hits seen = %d\n", total_hits);
}



TEST(BFN_TEST_NAME(LtcamTest),DeepTcams) {
  GLOBAL_MODEL->Reset();
  if (ltcam_print) RMT_UT_LOG_INFO("test_ltcam_deep_tcams()\n");
  int           chip = 202; // 2 stages
  int           pipe = 0;
  TestUtil      tu(GLOBAL_MODEL.get(), chip, pipe);
  TcamArrayWrap tcam_array(&tu);

  tu.set_stage_default_regs(pipe, 0);
  tu.set_stage_default_regs(pipe, 1);

  // DEBUG setup....................
  int      debug = 3;
  uint64_t pipes, stages, types, rows_tabs, cols, flags;
  pipes = ONE; stages = HI;  types = ALL; rows_tabs = FEW; cols = TOP; flags = NON;
  pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
  // Just to stop compiler complaining about unused vars
  flags = FEW;

  tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
  tu.quieten_p4_log_flags(pipes);

  if (debug == 0)
    tu.get_objmgr()->update_log_type_levels(pipes, ALL, RMT_LOG_TYPE_P4, ALL, UINT64_C(0));
  tcam_array.set_debug(debug);

  const uint64_t root_seed      = UINT64_C(0x1234567890);
  const int      num_iterations = 50;
  const int      num_lookups    = 50;

  // Flags for final (only) tcam
  uint32_t tcflags = TcamCtl::kMatchLoFinal|TcamCtl::kMatchHiFinal;
  uint64_t seed = root_seed;
  int      total_hits = 0;
  int      num_mismatches = 0;

  for (int iteration = 0; iteration < num_iterations; iteration++) {
    // Even iterations configure all LTCAMs as ingress, odd iterations all egress
    uint8_t ing_ltcams =  ((iteration % 2) == 0) ?0xFF :0x00;
    uint8_t egr_ltcams =  ((iteration % 2) == 0) ?0x00 :0xFF;
    uint8_t ltc_pair[2];
    uint8_t ltcam0 = tu.xrandrange(seed,iteration,0,7); // Pick one
    uint8_t ltcam1 = 7 - ltcam0;
    bool    cb_emulate_tof_jbay_prio = false; // *WILL* mismatch RefModel if true
    bool    cb_emulate_tof_jbay = false; // Emulate 1 LTCAM per TCAM, NoBitmap, Result0
    bool    tof_jbay_emulate_cb = false; // Emulate no actionbit

    ltc_pair[0] = 1<<ltcam0; ltc_pair[1] = 1<<ltcam1;
    // If cb_emulate_tof_jbay disable multiple LTCAMs per physical TCAM
    if (RmtObject::is_chip1() && !cb_emulate_tof_jbay) {
      // If WIP (and not emulating) we can handle many LTCAMs at once
      ltc_pair[0] = tu.xrand8(seed,iteration); ltc_pair[1] = 0xFF & ~ltcam0;
    }
    // Set flags below to constrain WIP to behave like Tof/JBay
    // Note WIP tcam_array will NOT perfectly match Tofino/JBay tcam_array
    // as the WIP implementation prioritizes row results differently.
    // (Set kStrictRowPrio flag to see perfect match - but RefModel lookup WILL fail)
    if (cb_emulate_tof_jbay) {
      tcflags |= TcamCtl::kNoBitmap;   // Constrain WIP to not use TCAM Bitmaps
      tcflags |= TcamCtl::kUseResult0; // Constrain WIP to only use TCAM PhysResult 0
    }
    // Set flag below to constrain Tof/JBay to behave like WIP
    if (tof_jbay_emulate_cb) {
      tcflags |= TcamCtl::kNoAbit; // Constrain Tof/JBay to not use actionbit
    }
    // NB. If you tell WIP tcam_array to do StrictRowPrio it WILL mismatch
    // the RefModel implementation - only intended to check the WIP tcam_array
    // behaves in the same manner as the Tofino/JBay tcam_array
    if (cb_emulate_tof_jbay_prio) {
      tcflags |= TcamCtl::kStrictRowPrio;
    }

    // Setup random w0/w1 config in LTCAM
    // We alternate use of LTCAM pair across TCAM rows/cols
    tcam_array.set_seed(seed);
    tcam_array.configure(ing_ltcams, egr_ltcams);
    for (int col = 0; col <= 1; col++) {
      for (int row = 0; row < 12; row++) {
        uint8_t ltc_here = ltc_pair[ (col + row) % 2 ];
        tcam_array.configure_tcams(col,col,row,row,tcflags,ltc_here,UINT64_C(0));
      }
    }
    tcam_array.install();

    for (int lookup = 0; lookup < num_lookups; lookup++) {
      // Even lookups lookup using ingress thread, odd use egress thread
      uint8_t thread = ((lookup % 2) == 0) ?TcamConsts::kIngress :TcamConsts::kEgress;

      uint64_t lookup_seed = tcam_array.get_lookup_seed();
      // Then do random data lookups in LTCAM comparing against RefModel
      printf("TCAM(%d) Lookup %d (seed=0x%" PRIx64 ") (lookup_seed=0x%" PRIx64 ")\n",
               iteration, lookup, seed, lookup_seed);
      if (!tcam_array.lookup_seed(thread, lookup_seed, debug)) {
        if (RmtObject::is_chip1() && ((tcflags & TcamCtl::kStrictRowPrio) != 0u)) {
          printf("TCAM(%d) Lookup %d failed - EXPECTED given StrictRowPrio mode "
               "(seed=0x%" PRIx64 ") (lookup_seed=0x%" PRIx64 ")\n",
               iteration, lookup, seed, lookup_seed);
        } else {
          num_mismatches++;
          printf("TCAM(%d) Lookup %d failed - retrying with logging on "
                 "(seed=0x%" PRIx64 ") (lookup_seed=0x%" PRIx64 ")\n",
                 iteration, lookup, seed, lookup_seed);
          tu.update_log_flags(pipes, stages, types, rows_tabs, cols, ALL, ALL);
          (void)tcam_array.lookup_seed(thread, lookup_seed, 8); // 9=>maximal debug
          tu.quieten_log_flags();
          break;
        }
      }
    } // for (int lookup = 0; lookup < num_lookups; lookup++)

    seed = tu.mmix_rand64(seed);
    total_hits += tcam_array.total_hits();

  } // for (int iteration = 0; iteration < num_iterations; iteration++)
  EXPECT_EQ(0, num_mismatches);
  printf("Total LTCAM hits seen = %d\n", total_hits);
}



TEST(BFN_TEST_NAME(LtcamTest),MultipleLogicalTcamsWithChaining) {
  GLOBAL_MODEL->Reset();
  if (ltcam_print) RMT_UT_LOG_INFO("test_ltcam_multiple_logical_tcams_with_chaining()\n");
  int           chip = 202; // 2 stages
  int           pipe = 0;
  TestUtil      tu(GLOBAL_MODEL.get(), chip, pipe);
  TcamArrayWrap tcam_array(&tu);

  tu.set_stage_default_regs(pipe, 0);
  tu.set_stage_default_regs(pipe, 1);

  // DEBUG setup....................
  int      debug = 0;
  uint64_t pipes, stages, types, rows_tabs, cols, flags;
  pipes = ONE; stages = HI;  types = ALL; rows_tabs = FEW; cols = TOP; flags = NON;
  pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
  // Just to stop compiler complaining about unused vars
  flags = FEW;
  tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);
  tu.quieten_p4_log_flags(pipes);

  if (debug == 0)
    tu.get_objmgr()->update_log_type_levels(pipes, ALL, RMT_LOG_TYPE_P4, ALL, UINT64_C(0));
  tcam_array.set_debug(debug);

  const uint64_t root_seed      = UINT64_C(0x1234567890);
  const int      num_iterations = 50;
  const int      num_lookups    = 50;

  // Flags for final (only) tcam
  uint64_t seed = root_seed;
  int      total_hits = 0;
  int      num_mismatches = 0;

  for (int iteration = 0; iteration < num_iterations; iteration++) {
    // Even iterations configure all LTCAMs as ingress, odd iterations all egress
    uint8_t ing_ltcams =  ((iteration % 2) == 0) ?0xFF :0x00;
    uint8_t egr_ltcams =  ((iteration % 2) == 0) ?0x00 :0xFF;
    // Allow many LTCAMs in the physical TCAM on WIP
    // (NB On Tofino/JBay only highest numbered LTCAM will be used)
    uint8_t ltcams = static_cast<uint8_t>( ((iteration * 999329) + 999331) & 0xFF );
    int     col = iteration % 2;
    int     row_start = ((iteration * 998027) + 997029) % 12; // Jump about a bit
    int     chain_len = (((iteration * 997019) + 997031) % 11) + 1;
    int     row_end = (row_start + chain_len) % 12;
    int     row_hi = (row_start > row_end) ?row_start :row_end;
    int     row_lo = (row_start < row_end) ?row_start :row_end;
    EXPECT_NE(row_start, row_end);

    // Set flags to indicate CHAIN OUT for all (non-final) TCAMs in chain.
    // Final *upper* half TCAM may get MATCH OUT later
    // Final *lower* half TCAM may get MATCH OUT later (unless chain
    //  crosses middle in which case it'll get CHAIN OUT instead).
    uint32_t tcflags = TcamCtl::kChainOutput;

    // If crossing middle should set final LO TCAM to CHAIN OUT too
    // (for now also set kChainHiFinal as RefModel expects TCAM[6] ChainOut set)
    if ((row_hi >= 6) && (row_lo <= 5))
      tcflags |= (TcamCtl::kChainLoFinal|TcamCtl::kMatchHiFinal|TcamCtl::kChainHiFinal);
    // If entirely bot half chain, final LO TCAM should MATCH OUT
    else if (row_hi <= 5)                   tcflags |= TcamCtl::kMatchLoFinal;
    // If entirely top half chain, final HI TCAM should MATCH OUT
    else if (row_lo >= 6)                   tcflags |= TcamCtl::kMatchHiFinal;

    // Setup random w0/w1 config in LTCAM - just use *one* TCAM
    tcam_array.set_seed(seed);
    tcam_array.configure(ing_ltcams, egr_ltcams);
    tcam_array.configure_tcams(col,col,row_start,row_end,tcflags,ltcams,UINT64_C(0));
    tcam_array.install();

    for (int lookup = 0; lookup < num_lookups; lookup++) {
      // Even lookups lookup using ingress thread, odd use egress thread
      uint8_t thread = ((lookup % 2) == 0) ?TcamConsts::kIngress :TcamConsts::kEgress;

      uint64_t lookup_seed = tcam_array.get_lookup_seed();
      // Then do random data lookups in LTCAM comparing against RefModel
      if (!tcam_array.lookup_seed(thread, lookup_seed, debug)) {
        num_mismatches++;
        printf("TCAM[%d..%d,%d] Lookup %d failed - retrying with logging on "
               "(seed=0x%" PRIx64 ") (lookup_seed=0x%" PRIx64 ")\n",
               row_start, row_end, col, lookup, seed, lookup_seed);
        tu.update_log_flags(pipes, stages, types, rows_tabs, cols, ALL, ALL);
        (void)tcam_array.lookup_seed(thread, lookup_seed, 8); // 9=>maximal debug
        tu.quieten_log_flags();
        break;
      }
    } // for (int lookup = 0; lookup < num_lookups; lookup++)

    seed = tu.mmix_rand64(seed);
    total_hits += tcam_array.total_hits();

  } // for (int iteration = 0; iteration < num_iterations; iteration++)
  EXPECT_EQ(0, num_mismatches);
  printf("Total LTCAM hits seen = %d\n", total_hits);
}





}
