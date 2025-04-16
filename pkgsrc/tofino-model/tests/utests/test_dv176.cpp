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

// mau_stats1 -> test_dv176.cpp
// 
// Use this command to get fails:
// $ grep ',fail,' /scratch/dv/verif/regr/jrobinson_01_21__15_58_mau_stats1_testlist/jrobinson_01_21__15_58_mau_stats1_testlist_summary_report.txt | sed 'a\ ' | less
//
//
// Here are the ones to look at:
//
// block_mau/stats_tcm1tbl_D24_oflo_e2_basicA_0_2644142605046000820,fail,# UVM_ERROR @1136600ps
// uvm_test_top.mau_env.stats_env1>mau_block_stats_env.svh(121) [MAUSTATS] Miscompare entry a=0x200c0e03ffc entry=0x3ffc  Ref-d=0x0 != SB-d=0x5d f=2
//
// block_mau/stats_tcm1tbl_D3_22_oflo_e1_basicA_0_2644142605046000834,fail,# UVM_ERROR @1314000ps
// uvm_test_top.mau_env.stats_env1>mau_block_stats_env.svh(121) [MAUSTATS] Miscompare entry a=0x200c0605000 entry=0x5000  Ref-d=0x0 != SB-d=0x335df000000000000005e f=2
//
// block_mau/stats_tcm1tbl_D24_oflo_e1_basicA_0_2644142605046000852,fail,# UVM_ERROR @1139900ps
// uvm_test_top.mau_env.stats_env1>mau_block_stats_env.svh(121) [MAUSTATS] Miscompare entry a=0x200c1e01008 entry=0x1008  Ref-d=0x0 != SB-d=0x4d730000000000000008c f=2
//
// block_mau/stats_tcm2tbl_D12_oflo_e1_basicA_0_2644142605046000853,fail,# UVM_ERROR @1415100ps
// uvm_test_top.mau_env.stats_env1>mau_block_stats_env.svh(121) [MAUSTATS] Miscompare entry a=0x200c0206000 entry=0x6000  Ref-d=0x0 != SB-d=0x11b9f000000000000001a f=2


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

  bool dv176_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv176Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv176_print) RMT_UT_LOG_INFO("test_dv176_packet1()\n");
    
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
    tu.set_dv_test(176);    

    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);    



    RmtObjectManager *om = tu.get_objmgr();
    Mau *mau = om->mau_lookup(pipe, stage);
    (void)tu.port_get(16);
    ASSERT_TRUE(tu.read_test_file("extracted_tests/test_dv176.test"));


    int num_phvs = mau->mau_info_read("MAU_N_PHVS");
    int num_eops = mau->mau_info_read("MAU_N_EOPS");
    printf("N_PHVS=%d, N_EOPS=%d\n", num_phvs, num_eops);


    tu.quieten_log_flags();


    // Allow stats CFG READs to ALL LTs
    auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);
    tu.OutWord(&mau_reg_map.rams.match.adrdist.mau_ad_stats_virt_lt[0], 0xFFFFu); 
    tu.OutWord(&mau_reg_map.rams.match.adrdist.mau_ad_stats_virt_lt[1], 0xFFFFu); 
    tu.OutWord(&mau_reg_map.rams.match.adrdist.mau_ad_stats_virt_lt[2], 0xFFFFu);
    tu.OutWord(&mau_reg_map.rams.match.adrdist.mau_ad_stats_virt_lt[3], 0xFFFFu);     

    // Read back vals
    // We read back stats subword 0s to get full 128b
    // Then if non-zero we read a subword at a time (and get full-res vals)
    //
    uint64_t data0, data1, fdata0, fdata1, sdata0, sdata1;

    for (int logtab = 0; logtab <= 15; logtab += 1) {

      // Only read tables that have the stats_adr_icxbar setup
      uint32_t alus = tu.InWord(&mau_reg_map.rams.match.adrdist.adr_dist_stats_adr_icxbar_ctl[logtab]);
      if (alus == 0u) continue;       
      printf("\n");

      // First of all do genuine stats virtual address reads
      for (int index = 0; index < 65536; index += 8) {

        int sram_index = (index/8) % 1024;
        int sram_vpn = (index/8) / 1024;
        
        // Read vaddr for pipe0, stage0, STATS, logtab, vaddr
        uint32_t vaddr = index;
        uint64_t addr64 = TestUtil::make_virtual_address(0, 0, TestUtil::kVirtMemTypeStats, logtab, vaddr);

        data0 = UINT64_C(0); data1 = UINT64_C(0);
        tu.IndirectRead(addr64, &data0, &data1);

        if ((data0 != UINT64_C(0)) || (data1 != UINT64_C(0))) {
	  printf("\n");
          printf(" 128b: LT=%d VPN=%d Index=%d       SAddr64=0x%016" PRIx64 
                 " Data1=%016" PRIx64 " Data0=%016" PRIx64 "\n",
                 logtab, sram_vpn, sram_index, addr64, data1, data0);

          // Now read out stats vals (and full res vals) for each subword
          for (int subword = 0; subword < 7; subword++) {
            uint64_t saddr64 = addr64 | subword; 
            uint64_t faddr64 = saddr64 | (0x3 << 28);
            
            sdata0 = UINT64_C(0); sdata1 = UINT64_C(0);
            tu.IndirectRead(saddr64, &sdata0, &sdata1);

            fdata0 = UINT64_C(0); fdata1 = UINT64_C(0);
            tu.IndirectRead(faddr64, &fdata0, &fdata1);

            if ((sdata0 != UINT64_C(0)) || (sdata1 != UINT64_C(0)) ||
                (fdata0 != UINT64_C(0)) || (fdata1 != UINT64_C(0))) {
            
              printf("  Sub: LT=%d VPN=%d Index=%d Sub=%d SAddr64=0x%016" PRIx64 
                     " Data1=%016" PRIx64 " Data0=%016" PRIx64 
                     " PktCnt=%" PRId64 " ByteCnt=%" PRId64 "\n",
                     logtab, sram_vpn, sram_index, subword, saddr64,
                     sdata1, sdata0, fdata0, fdata1);
            }
          }
        }
      }
    }

    
    tu.finish_test();
    printf("N_PHVS=%d, N_EOPS=%d\n", num_phvs, num_eops);
}


}
    
