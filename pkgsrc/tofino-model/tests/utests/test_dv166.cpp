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

// XXX/dec16.pktx6 -> test_dv166.cpp
//  runSim.py +test=mau_tcam_multi_phv_test_c -f knobs/seq_statsA_br20_snd50_15_15_rpt5.knobs -f knobs/exact_1tblhit.knobs -f knobs/stats_em_common.knobs -f knobs/huffman_rand.knobs -f knobs/stats_pkt6.knobs -f knobs/stats_direct_addressing.knobs -f knobs/phvattr_ie.knobs -f knobs/n_phvs_12.knobs +*seq*.phv_loop_cnt=5 -f questa.knobs +uvm_test_top.mau_env.stats_env*.error_on_ref_model_stats_cmp=1 +uvm_test_top.mau_env.stats_env*.get_full_entry_ref_model=0 -f config/zdebug.knobs -f config/refmodel.debugall.knobs +seed=350944349114854440 +seed=350944349114854440
// Ref Model sticks on last Stats data when empty entries are read out at EOT
//
// For the first case of pkt x6. Looks like the CfgRd fields are stroed in the wrong locations.
// In this case subwords 3'h2 & 3'h4 are swapped.
// UVM_ERROR @ 236000ps uvm_test_top.mau_env.stats_env0>mau_block_stats_env.svh(90) [SB_REF]
//                      Miscompare entry a=0x200c04004e2 entry=0x4e2 Ref-d=0x4 != SB-d=0x0
// UVM_ERROR @ 236000ps uvm_test_top.mau_env.stats_env0>mau_block_stats_env.svh(90) [SB_REF]
//                      Miscompare entry a=0x200c04004e4 entry=0x4e4 Ref-d=0x0 != SB-d=0x4
// UVM_ERROR @ 236800ps uvm_test_top.mau_env.stats_env0>mau_block_stats_env.svh(90) [SB_REF]
//                      Miscompare entry a=0x200c040051a entry=0x51a Ref-d=0x18 != SB-d=0x0
// UVM_ERROR @ 236800ps uvm_test_top.mau_env.stats_env0>mau_block_stats_env.svh(90) [SB_REF]
//                      Miscompare entry a=0x200c040051c entry=0x51c Ref-d=0x0 != SB-d=0x18
// UVM_ERROR @ 238100ps uvm_test_top.mau_env.stats_env0>mau_block_stats_env.svh(90) [SB_REF]
//                      Miscompare entry a=0x200c0400572 entry=0x572 Ref-d=0x2 != SB-d=0x0
// UVM_ERROR @ 238200ps uvm_test_top.mau_env.stats_env0>mau_block_stats_env.svh(90) [SB_REF]
//                      Miscompare entry a=0x200c0400574 entry=0x574 Ref-d=0x0 != SB-d=0x2

#include <utests/test_util.h>
#include <iostream>
#include <string>
#include <array>
#include <cassert>

#include "gtest.h"

#include <bitvector.h>
#include <rmt-object-manager.h>
#include <model_core/model.h>
#include <mau.h>
#include <eop.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

  bool dv166_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv166Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv166_print) RMT_UT_LOG_INFO("test_dv166_packet1()\n");
    
    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Set 'vintage' of DV test to configure other global vars correctly
    tu.set_dv_test(166);    

    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


    // Get handles for ObjectManager, MAU, port
    // Setup a port on pipe 0
    // This also sets up basic config ingress parser and deparser
    RmtObjectManager *om = tu.get_objmgr();
    (void)om->mau_lookup(pipe, stage);
    (void)tu.port_get(16);
    // Uncomment below to up the debug output
    //flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

// PUT GENERATED OutWord/IndirectWrite calls HERE
    ASSERT_TRUE(tu.read_test_file("extracted_tests/test_dv166.test"));


    // Schtumm
    tu.quieten_log_flags();


    // Read back vals
    printf("Reading back stats subword 0s to get full 128b\n");
    printf("  If non-zero read a subword at a time\n");
    uint64_t data0, data1, fdata0, fdata1, sdata0, sdata1;
    int logtab = 2;

    // First of all do genuine stats virtual address reads
    for (int index = 0; index < 4096; index += 8) {

      // Read vaddr for pipe0, stage0, STATS, logtab=2, vaddr
      uint32_t vaddr = index;
      uint64_t addr64 = TestUtil::make_virtual_address(0, 0, TestUtil::kVirtMemTypeStats, logtab, vaddr);

      data0 = UINT64_C(0); data1 = UINT64_C(0);
      tu.IndirectRead(addr64, &data0, &data1);

      // The stats RAM uses format 0x56 which is a packet-only format 
      // having 6-entries per 128b. Bit 0x40 indicates egress stats ALU.
      if ((data0 != UINT64_C(0)) || (data1 != UINT64_C(0))) {
      	 printf(" 128b: LT=%d Index=%d Addr64=0x%016" PRIx64 
                " Data1=%016" PRIx64 " Data0=%016" PRIx64 "\n",
		logtab, index/8, addr64, data1, data0);

         // Now read out stats vals (and full res vals) for each subword
         for (int subword = 0; subword < 6; subword++) {
           uint64_t saddr64 = addr64 | subword; 
	   uint64_t faddr64 = saddr64 | (0x3 << 28);

	   sdata0 = UINT64_C(0); sdata1 = UINT64_C(0);
           tu.IndirectRead(saddr64, &sdata0, &sdata1);

	   fdata0 = UINT64_C(0); fdata1 = UINT64_C(0);
           tu.IndirectRead(faddr64, &fdata0, &fdata1);

           printf("  Sub: LT=%d Index=%d Sub=%d SAddr64=0x%016" PRIx64 
                " Data1=%016" PRIx64 " Data0=%016" PRIx64,
		logtab, index/8, subword, saddr64, sdata1, sdata0);
           printf(" PktCnt=%" PRId64, fdata0);
           if (fdata1 != UINT64_C(0)) printf(" ByteCnt=%" PRId64, fdata1);
	   printf("\n");
         }
      }
    }

    // Now read full-resolution stats - inc by 1 to go through all poss subwords
    printf("Reading back full-res stats\n");
    for (int index = 0; index < 4096; index += 1) {

      // Read vaddr for pipe0, stage0, STATS, logtab=2, vaddr
      uint32_t vaddr = index;
      uint64_t addr64 = TestUtil::make_virtual_address(0, 0, TestUtil::kVirtMemTypeStats, logtab, vaddr);
      // Abuse DataSize=0x3 to indicate full-res stats read
      uint64_t faddr64 = addr64 | (0x3 << 28);

      fdata0 = UINT64_C(0); fdata1 = UINT64_C(0);
      tu.IndirectRead(faddr64, &fdata0, &fdata1);

      // The full-res format keeps pkt-count in data0, byte-count in data1
      if ((fdata0 != UINT64_C(0)) || (fdata1 != UINT64_C(0))) {
        printf("FullRes: LT=%d Index=%d Sub=%d FAddr64=0x%016" PRIx64 
               " PktData=%" PRId64 " ByteData=%" PRId64 "\n",
               logtab, index/8, index%8, faddr64, fdata0, fdata1);
      }
    }
    

    tu.finish_test();
}


}
    
