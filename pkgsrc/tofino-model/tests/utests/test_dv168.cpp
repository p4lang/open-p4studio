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

// XXX/dec17.bytex4 -> test_dv168.cpp
// Run with new registers in /scratch/dv/jira/XXX_dec28.bytex4.run_dir
//   runSim.py +test=mau_tcam_multi_phv_test_c -f knobs/seq_mixA_allhit.knobs -f knobs/tcam_1tbl_W2_3_cfg_ie_idlet.knobs -f knobs/tcam_common.knobs -f knobs/huffman128.knobs -f knobs/stats_bytes4.knobs -f knobs/tind_stats_max.knobs -f knobs/stats_indirect_addressing.knobs -f knobs/phvattr_ie.knobs -f knobs/n_phvs_3.knobs +*seq*.phv_loop_cnt=10 -f questa.knobs +uvm_test_top.mau_env.stats_env*.error_on_ref_model_stats_cmp=1 +uvm_test_top.mau_env.stats_env*.get_full_entry_ref_model=0 +seed=350944349114854470 -f config/zdebug.knobs -f config/refmodel.debugall.knobs +uvm_test_top.mau_env.stats_env*.log_level=debug +seed=350944349114854470
// Ref Model sticks on last Stats data when empty entries are read out at EOT
//
// Here is the next one.  byte x4 case. 
// The byte count does not seem to be stored cumulatively. (Looks like Ref Model is overwriting the count each time).
//
// Cut the case down to 3 phv and sent each 10 times.  But due to the magic of randomization,
// the stats table is egress and only one egress phv was generated. (Gave me a bit a panic for
// a while as was expecting three different stats address and only got one!).
//
// Error:
// # UVM_ERROR @ 423800ps uvm_test_top.mau_env.stats_env0>mau_block_stats_env.svh(90) [SB_REF] Miscompare entry a=0x200c1800114 entry=0x114  
// # Ref-d=0x121a0000000000000000 != 
// # SB-d=0xb5040000000000000000 f=0
//
// Stats Mon Ops:
// $ zgrep stats_mon run_dir/run.log.gz | grep STATS_ACTIVE | sed -e 's/uvm_test_top.mau_env.stats_env0.stats_ag.mon>mau_stats_monitor.svh//g'  > run_dir/stats_mon.log 
// # INFO @  64800ps (251) [MON] saw Stats pkt id=1# STATS_ACTIVE  a[22:19]=1 a=0x00114 eop=0x121a din=0x00000000000000000000000000000000 e=0 dout=0x000000000000121a0000000000000000 e=0
// # INFO @  90400ps (251) [MON] saw Stats pkt id=2# STATS_ACTIVE  a[22:19]=1 a=0x00114 eop=0x121a din=0x000000000000121a0000000000000000 e=0 dout=0x00000000000024340000000000000000 e=0
// # INFO @ 116000ps (251) [MON] saw Stats pkt id=3# STATS_ACTIVE  a[22:19]=1 a=0x00114 eop=0x121a din=0x00000000000024340000000000000000 e=0 dout=0x000000000000364e0000000000000000 e=0
// # INFO @ 140800ps (251) [MON] saw Stats pkt id=4# STATS_ACTIVE  a[22:19]=1 a=0x00114 eop=0x121a din=0x000000000000364e0000000000000000 e=0 dout=0x00000000000048680000000000000000 e=0
// # INFO @ 166400ps (251) [MON] saw Stats pkt id=5# STATS_ACTIVE  a[22:19]=1 a=0x00114 eop=0x121a din=0x00000000000048680000000000000000 e=0 dout=0x0000000000005a820000000000000000 e=0
// # INFO @ 193600ps (251) [MON] saw Stats pkt id=6# STATS_ACTIVE  a[22:19]=1 a=0x00114 eop=0x121a din=0x0000000000005a820000000000000000 e=0 dout=0x0000000000006c9c0000000000000000 e=0
// # INFO @ 219200ps (251) [MON] saw Stats pkt id=7# STATS_ACTIVE  a[22:19]=1 a=0x00114 eop=0x121a din=0x0000000000006c9c0000000000000000 e=0 dout=0x0000000000007eb60000000000000000 e=0
// # INFO @ 243200ps (251) [MON] saw Stats pkt id=8# STATS_ACTIVE  a[22:19]=1 a=0x00114 eop=0x121a din=0x0000000000007eb60000000000000000 e=0 dout=0x00000000000090d00000000000000000 e=0
// # INFO @ 270400ps (251) [MON] saw Stats pkt id=9# STATS_ACTIVE  a[22:19]=1 a=0x00114 eop=0x121a din=0x00000000000090d00000000000000000 e=0 dout=0x000000000000a2ea0000000000000000 e=0
// # INFO @ 296000ps (251) [MON] saw Stats pkt id=10# STATS_ACTIVE a[22:19]=1 a=0x00114 eop=0x121a din=0x000000000000a2ea0000000000000000 e=0 dout=0x000000000000b5040000000000000000 e=0
//

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

  bool dv168_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv168Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv168_print) RMT_UT_LOG_INFO("test_dv168_packet1()\n");
    
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
    tu.set_dv_test(168);    

    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);    
    
    
    ASSERT_TRUE(tu.read_test_file("extracted_tests/test_dv168.test"));



    // Schtumm
    tu.quieten_log_flags();


    // Read back vals
    printf("Reading back stats subword 0s to get full 128b\n");
    printf("  If non-zero read a subword at a time\n");
    uint64_t data0, data1, fdata0, fdata1, sdata0, sdata1;
    int logtab = 12;

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
    
