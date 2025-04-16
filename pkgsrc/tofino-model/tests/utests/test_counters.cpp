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

#include <bitvector.h>
#include <rmt-object-manager.h>
#include <model_core/model.h>
#include <mau.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool counter_print = false;

using namespace std;
using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(CountersTest),CheckVpnHoleHandling) {
     if (counter_print) RMT_UT_LOG_INFO("test_counter_check_vpn_hole_handling()\n");    
     int chip = 202; // Just the 2 stages
     int pipe = 0;
     int stage = 0;

     // Create our TestUtil class
     // Instantiating new TestUtil obj should free all existing
     // RmtObjectManagers (if this has not already occurred) then
     // recreate a RmtObjectManager just for chip
     TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);

     // DEBUG setup....................
     uint64_t pipes, stages, types, rows_tabs, cols, flags;
     pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;     
     pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
     tu.set_debug(false);
     tu.set_evaluate_all(true, false); // Don't test evaluateAll
     tu.set_free_on_exit(true);
     // Set some random config flag here
     // Then check its gone back to its default setting in next Counters test
     MauMemory::kAllowBadMapramWrite = true;

     // Just to stop compiler complaining about unused vars
     flags = FEW;
     tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

     // Instantiate whole chip and fish out objmgr
     RmtObjectManager *om = tu.get_objmgr();
     ASSERT_TRUE(om != NULL);
     // Lookup this Pipe/Stage MAU and MAU_COUNTERS obj
     Mau *mau = om->mau_lookup(pipe, stage);
     ASSERT_TRUE(mau != NULL);
     MauTableCounters *mau_table_counters = mau->mau_table_counters();
     ASSERT_TRUE(mau_table_counters != NULL);


     // Test out one-liner to make 32b bitmask from 4b bytemask
     uint32_t masktab[] = { 0x00000000, 0x000000FF, 0x0000FF00, 0x0000FFFF,
                            0x00FF0000, 0x00FF00FF, 0x00FFFF00, 0x00FFFFFF,
                            0xFF000000, 0xFF0000FF, 0xFF00FF00, 0xFF00FFFF,
                            0xFFFF0000, 0xFFFF00FF, 0xFFFFFF00, 0xFFFFFFFF };
     uint32_t mask, ff = 255u;
     for (uint8_t bm = 0; bm < 16; bm++) {
       mask = (bm&8)*(ff<<21) + (bm&4)*(ff<<14) + (bm&2)*(ff<<7) + (bm&1)*(ff<<0);
       //printf("masktab[%d]=0x%08x calculated=0x%08x\n", bm, masktab[bm], mask);
       EXPECT_EQ(mask, masktab[bm]);
     }

 

     // Check the holevpn_incr func behaves as expected
     uint8_t val_in, val_out;

     // In the absence of a hole should do nothing
     val_in = 0;
     for (int i = 0; i < 63; i++) {
       val_out = SweepInfo::holevpn_incr(val_in, -1);
       if (counter_print)
         printf("i=%d hole=-1 output=%d (0x%02x) input=%d (0x%02x)\n",
                i, val_out, val_out, val_in, val_in);
       EXPECT_EQ(val_out,val_in+1);
       val_in = val_out;
     }

     // With a hole behaves a little differently - hole increments first
     val_in = 11;
     for (int i = 0; i < 63; i++) {
       val_out = SweepInfo::holevpn_incr(val_in, 4);
       if (counter_print)
         printf("i=%d hole=4 output=%d (0x%02x) input=%d (0x%02x)\n",
                i, val_out, val_out, val_in, val_in);
       
       val_in = val_out;
     }
     EXPECT_EQ(27, SweepInfo::holevpn_incr(11, 4));
     EXPECT_EQ(43, SweepInfo::holevpn_incr(27, 4));
     EXPECT_EQ(12, SweepInfo::holevpn_incr(43, 4));

     val_in = 16;
     for (int i = 0; i < 20; i++) {
       val_out = SweepInfo::holevpn_incr(val_in, 2);
       if (counter_print)
         printf("i=%d hole=2 output=%d (0x%02x) input=%d (0x%02x)\n",
                i, val_out, val_out, val_in, val_in);
       val_in = val_out;
     }
     EXPECT_EQ(20, SweepInfo::holevpn_incr(16, 2));
     EXPECT_EQ(24, SweepInfo::holevpn_incr(20, 2));
     EXPECT_EQ(17, SweepInfo::holevpn_incr(24, 2));
     EXPECT_EQ(21, SweepInfo::holevpn_incr(17, 2));
     EXPECT_EQ(25, SweepInfo::holevpn_incr(21, 2));

     EXPECT_EQ(48, SweepInfo::holevpn_incr(43, 2));
     
     // Schtum
     tu.finish_test();
     tu.quieten_log_flags();
   }


TEST(BFN_TEST_NAME(CountersTest),TableCounters) {
     if (counter_print) RMT_UT_LOG_INFO("test_counter_table_counters()\n");    
     int chip = 202; // Just the 2 stages
     int pipe = 0;
     int stage = 0;

     // Create our TestUtil class
     // Instantiating new TestUtil obj should free all existing
     // RmtObjectManagers (if this has not already occurred) then
     // recreate a RmtObjectManager just for chip
     TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
     auto& mm_regs = RegisterUtils::ref_mau(pipe,stage).rams.match.merge;


     // DEBUG setup....................
     uint64_t pipes, stages, types, rows_tabs, cols, flags;
     pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;     
     pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
     tu.set_debug(false);
     tu.set_evaluate_all(true, false); // Don't test evaluateAll
     tu.set_free_on_exit(true);
     // Check flag we set in previous Counters test has been reset to default
     EXPECT_EQ(MauMemory::kAllowBadMapramWrite, false);

     // Just to stop compiler complaining about unused vars
     flags = FEW;
     tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

     // Instantiate whole chip and fish out objmgr
     RmtObjectManager *om = tu.get_objmgr();
     ASSERT_TRUE(om != NULL);
     // Lookup this Pipe/Stage MAU and MAU_COUNTERS obj
     Mau *mau = om->mau_lookup(pipe, stage);
     ASSERT_TRUE(mau != NULL);
     MauTableCounters *mau_table_counters = mau->mau_table_counters();
     ASSERT_TRUE(mau_table_counters != NULL);
     MauLookupResult hit[4], miss[4];
     for (int i = 0; i < 4; i++) {
       hit[i].reset();
       hit[i].set_active(true);
       hit[i].set_match(true);
       hit[i].set_logical_table(i);
       miss[i].reset();
       miss[i].set_active(true);
       miss[i].set_match(false);
       miss[i].set_logical_table(i);
     }
     
     // Setup tables and table counters
     for (int lt = 0; lt < 4; lt ++) {
       // Cnt HITs for even tables, MISSes for odd
       int cnt_what = ((lt % 2) == 0) ?2 :1;
       
       // Setup 4 logical table for ingress in both stages
       tu.table_config(lt, true);    // tableN ingress
       
       // Setup tablecounters for logical tables 0-3
       tu.table_cntr_config(lt, cnt_what);
     }

     // UP debug now
     //flags = ALL;
     tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

     
     // Inject hit/miss events into all 4 tables
     // Even logical tables should increment on hit
     // Odd logical tables should increment on miss
     //
     uint32_t cntr_last[] = { 0u, 0u, 0u, 0u };
     int clear_interval[] = { 797, 997, 691, 887 };
     auto a_table_clr = &mm_regs.mau_table_counter_clear;

     for (int tries = 0; tries < 99999; tries++) {
       
       for (int lt = 0; lt < 4; lt++) {

         auto a_table_cntr = &mm_regs.mau_table_counter[lt][0];
         uint32_t cntr_before, cntr_after;

         // Check stashed counter hasn't changed from how we left it
         cntr_before = tu.InWord((void*)a_table_cntr);
         EXPECT_EQ(cntr_last[lt], cntr_before);
         
         // From time to time clear a counter
         // Only table specified should be affected
         if ((tries > 1000) && ((tries % clear_interval[lt]) == 0)) {
           tu.OutWord(a_table_clr, (1u << lt));
           for (int lt2 = 0; lt2 < 4; lt2++) {
             uint32_t cntr_exp = (lt2 == lt) ?0u :cntr_last[lt2];
             uint32_t cntr_act = tu.InWord((void*)&mm_regs.mau_table_counter[lt2][0]);
             EXPECT_EQ(cntr_exp, cntr_act);
           }
         }
         
         // Send in HIT
         cntr_before = tu.InWord((void*)a_table_cntr);
         mau_table_counters->maybe_increment_table_counter(lt, hit[lt]);
         cntr_after = tu.InWord((void*)a_table_cntr);

         //printf(" HIT%d: LT=%d CntrBefore=0x%08x\n", tries, lt, cntr_before);
         //printf(" HIT%d: LT=%d  CntrAfter=0x%08x\n", tries, lt, cntr_after);

         if ((lt % 2) == 0) {
           // Table counting hits so should see increment
           EXPECT_EQ(cntr_after, cntr_before + 1);
         } else {
           // Table counting misses so should be no change
           EXPECT_EQ(cntr_after, cntr_before);
         }
         
         // Send in MISS
         cntr_before = tu.InWord((void*)a_table_cntr);
         mau_table_counters->maybe_increment_table_counter(lt, miss[lt]);
         cntr_after = tu.InWord((void*)a_table_cntr);
         
         //printf("MISS%d: LT=%d CntrBefore=0x%08x\n", tries, lt, cntr_before);
         //printf("MISS%d: LT=%d  CntrAfter=0x%08x\n", tries, lt, cntr_after);         
         
         if ((lt % 2) == 1) {
           // Table counting misses so should see increment
           EXPECT_EQ(cntr_after, cntr_before + 1);
         } else {
           // Table counting hits so should be no change
           EXPECT_EQ(cntr_after, cntr_before);
         }

         // Stash counter
         cntr_last[lt] = cntr_after;
         
       } // for lt

     } // for tries


     // Schtum
     tu.finish_test();
     tu.quieten_log_flags();
   }     


TEST(BFN_TEST_NAME(CountersTest),StatefulLog) {
     if (counter_print) RMT_UT_LOG_INFO("test_counter_stateful_log()\n");    
     int chip = 202; // Just the 2 stages
     int pipe = 0;
     int stage = 0;

     // Create our TestUtil class
     // Instantiating new TestUtil obj should free all existing
     // RmtObjectManagers (if this has not already occurred) then
     // recreate a RmtObjectManager just for chip
     TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
     auto& mm_regs = RegisterUtils::ref_mau(pipe,stage).rams.match.merge;


     // DEBUG setup....................
     uint64_t pipes, stages, types, rows_tabs, cols, flags;
     pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;     
     pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
     tu.set_debug(false);
     tu.set_evaluate_all(true, false); // Don't test evaluateAll
     tu.set_free_on_exit(true);
     // Check flag we set in previous Counters test has been reset to default
     EXPECT_EQ(MauMemory::kAllowBadMapramWrite, false);

     // Just to stop compiler complaining about unused vars
     flags = FEW;
     tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

     // Instantiate whole chip and fish out objmgr
     RmtObjectManager *om = tu.get_objmgr();
     ASSERT_TRUE(om != NULL);
     // Lookup this Pipe/Stage MAU and MAU_COUNTERS obj
     Mau *mau = om->mau_lookup(pipe, stage);
     ASSERT_TRUE(mau != NULL);
     MauTableCounters *mau_table_counters = mau->mau_table_counters();
     ASSERT_TRUE(mau_table_counters != NULL);
     MauLookupResult hit[4], miss[4];
     for (int i = 0; i < 4; i++) {
       hit[i].reset();
       hit[i].set_active(true);
       hit[i].set_match(true);
       hit[i].set_logical_table(i);
       miss[i].reset();
       miss[i].set_active(true);
       miss[i].set_match(false);
       miss[i].set_logical_table(i);
     }
     
     MauStatefulCounters *mau_stateful_counters = mau->mau_stateful_counters();
     ASSERT_TRUE(mau_stateful_counters != NULL);
     // Setup tables and stateful counters
     for (int lt = 0; lt < 4; lt ++) {
       // Cnt HITs for even tables, MISSes for odd
       int cnt_what = ((lt % 2) == 0) ?2 :1;
       // 1-1 mapping LT->ALU
       int alu = lt; 
       // Use cntr_shift 4 so bottom 4-bits always same in/out
       // Means 16-bit instructions so 8 possible subwords
       int cntr_shift = 4;
       // Use VPN ranges [8-15],[16-23],[24-31],[32-39] for LTs 0,1,2,3
       // Should make bits [5:3] of VPN portion of addr match LT+1
       int vpn_min = (lt+1)*8;
       int vpn_max = ((lt+2)*8)-1;
       
       // Setup 4 logical table for ingress in both stages
       tu.table_config(lt, true);    // tableN ingress
       
       // Setup stateful counters for logical tables
       // Have 1-1 mapping from LT 0-3 to ALU 0-3
       tu.stateful_cntr_config(lt, alu, cnt_what,
                               cntr_shift, vpn_min, vpn_max);
     }

     // UP debug now
     //flags = ALL;
     tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

     
     // Inject hit/miss events into all 4 tables
     // Even logical tables should increment on hit
     // Odd logical tables should increment on miss
     // Returned addresses should match shifted value in stateful_log cntr reg
     // Bits [5:3] of VPN portion of addr should match LT+1
     //
     for (int tries = 0; tries < 65535; tries++) {
       
       for (unsigned int lt = 0; lt < 4; lt++) {

         int alu = lt;
         auto a_stateful_log_cntr = &mm_regs.mau_stateful_log_counter[alu][0];
         uint32_t cntr_before, cntr_after, addr_in, addr_out;
         
         // Send in HIT
         addr_in = (0x7 << 23) | lt; // Needs to be enabled addr
         cntr_before = tu.InWord((void*)a_stateful_log_cntr);
         addr_out = mau_stateful_counters->maybe_increment_stateful_counter(addr_in, lt, hit[lt]);
         cntr_after = tu.InWord((void*)a_stateful_log_cntr);

         // Don't check anything on try0 as VPN maybe not yet ORed in           
         if (tries > 0) {
           //printf(" HIT%d: LT=%d   AddrIn=0x%08x CntrBefore=0x%08x\n", tries, lt, addr_in, cntr_before);
           //printf(" HIT%d: LT=%d  AddrOut=0x%08x  CntrAfter=0x%08x\n", tries, lt, addr_out, cntr_after);
           //printf(" HIT%d: LT=%d VPN[5:3]=0x%08x  LT+1=%d\n", tries, lt, (addr_out >> 20) & 0x7, lt+1);
           // Bottom 4-bits should stay same because shifting by 4
           EXPECT_EQ(addr_out & 0xFu, lt);
           // Value of counter shifted by 4 should match addr part of address
           EXPECT_EQ(addr_out & 0x7FFFF0u, cntr_before << 4);
           // Bits [5:3] of VPN should match table+1
           EXPECT_EQ((addr_out >> 20) & 0x7u, lt+1);
           if ((lt % 2) == 0) {
             // Table counting hits so should see increment
             EXPECT_EQ(cntr_after, cntr_before + 1);
           } else {
             // Table counting misses so should be no change
             EXPECT_EQ(cntr_after, cntr_before);
           }
         }
         
         // Send in MISS
         addr_in = (0x7 << 23) | lt; // Needs to be enabled addr
         cntr_before = tu.InWord((void*)a_stateful_log_cntr);
         addr_out = mau_stateful_counters->maybe_increment_stateful_counter(addr_in, lt, miss[lt]);
         cntr_after = tu.InWord((void*)a_stateful_log_cntr);
         
         // Don't check anything on try0 as VPN maybe not yet ORed in           
         if (tries > 0) {
           //printf("MISS%d: LT=%d   AddrIn=0x%08x CntrBefore=0x%08x\n", tries, lt, addr_in, cntr_before);
           //printf("MISS%d: LT=%d  AddrOut=0x%08x  CntrAfter=0x%08x\n", tries, lt, addr_out, cntr_after);         
           //printf("MISS%d: LT=%d VPN[5:3]=0x%08x  LT+1=%d\n", tries, lt, (addr_out >> 20) & 0x7, lt+1);
           // Bottom 4-bits should stay same because shifting by 4
           EXPECT_EQ(addr_out & 0xF, lt);
           // Value of counter shifted by 4 should match addr part of address
           EXPECT_EQ(addr_out & 0x7FFFF0, cntr_before << 4);
           // Bits [5:3] of VPN should match table+1
           EXPECT_EQ((addr_out >> 20) & 0x7, lt+1);
           if ((lt % 2) == 1) {
             // Table counting misses so should see increment
             EXPECT_EQ(cntr_after, cntr_before + 1);
           } else {
             // Table counting hits so should be no change
             EXPECT_EQ(cntr_after, cntr_before);
           }
         }
       }
     }


     // Schtum
     tu.finish_test();
     tu.quieten_log_flags();
   }
}
