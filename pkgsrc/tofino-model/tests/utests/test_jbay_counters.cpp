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
#include "cmp_helper.h"

#include <bitvector.h>
#include <rmt-object-manager.h>
#include <model_core/model.h>
#include <mau.h>
#include <mau-execute-step.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool counter_print = false;

using namespace std;
using namespace MODEL_CHIP_NAMESPACE;


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
     uint64_t ONE = UINT64_C(1);
     uint64_t HI  = UINT64_C(0xFFFFFFFF00000000);
     uint64_t ALL = UINT64_C(0xFFFFFFFFFFFFFFFF);
     uint64_t FEW = UINT64_C(0xF); // FatalErrorWarn
     uint64_t NON = UINT64_C(0);
     uint64_t TOP = UINT64_C(1) << 63;
     uint64_t pipes, stages, types, rows_tabs, cols, flags;
     pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;     
     pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
     tu.set_debug(false);
     tu.set_evaluate_all(true, false); // Don't test evaluateAll
     tu.set_free_on_exit(true);

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
     MauLookupResult hit[16], miss[16];
     for (int i = 0; i < 16; i++) {
       hit[i].init(mau,mau->mau_result_bus());
       miss[i].init(mau,mau->mau_result_bus());
       hit[i].reset();
       miss[i].reset();
       if (i<4) {
         hit[i].set_active(true);
         hit[i].set_match(true);
         hit[i].set_logical_table(i);
         hit[i].set_match_buses( 0x0001 );
         miss[i].reset();
         miss[i].set_active(true);
         miss[i].set_match(false);
         miss[i].set_logical_table(i);
       }
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
     //tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

     
     // Inject hit/miss events into all 4 tables
     // Even logical tables should increment on hit
     // Odd logical tables should increment on miss
     //
     uint32_t counter_last[] = { 0u, 0u, 0u, 0u };
     int clear_interval[] = { 797, 997, 691, 887 };
     auto a_table_clr = &mm_regs.mau_table_counter_clear;

     for (int tries = 0; tries < 99999; tries++) {
       
       for (int lt = 0; lt < 4; lt++) {

         auto a_table_counter = &mm_regs.mau_table_counter[lt][0];
         uint32_t counter_before, counter_after;

         // Check stashed counter hasn't changed from how we left it
         counter_before = tu.InWord((void*)a_table_counter);
         EXPECT_EQ(counter_last[lt], counter_before);
         
         // From time to time clear a counter
         // Only table specified should be affected
         if ((tries > 1000) && ((tries % clear_interval[lt]) == 0)) {
           tu.OutWord(a_table_clr, (1u << lt));
           for (int lt2 = 0; lt2 < 4; lt2++) {
             uint32_t counter_exp = (lt2 == lt) ?0u :counter_last[lt2];
             uint32_t counter_act = tu.InWord((void*)&mm_regs.mau_table_counter[lt2][0]);
             EXPECT_EQ(counter_exp, counter_act);
           }
         }
         
         // Send in HIT
         counter_before = tu.InWord((void*)a_table_counter);
         mau_table_counters->maybe_increment_table_counter(lt, hit[lt]);
         counter_after = tu.InWord((void*)a_table_counter);

         //printf(" HIT%d: LT=%d CounterBefore=0x%08x\n", tries, lt, counter_before);
         //printf(" HIT%d: LT=%d  CounterAfter=0x%08x\n", tries, lt, counter_after);

         if ((lt % 2) == 0) {
           // Table counting hits so should see increment
           EXPECT_PRED_FORMAT2(CmpHelperIntHex , counter_before + 1, counter_after);
         } else {
           // Table counting misses so should be no change
           EXPECT_PRED_FORMAT2(CmpHelperIntHex , counter_before, counter_after);
         }
         
         // Send in MISS
         counter_before = tu.InWord((void*)a_table_counter);
         mau_table_counters->maybe_increment_table_counter(lt, miss[lt]);
         counter_after = tu.InWord((void*)a_table_counter);
         
         //printf("MISS%d: LT=%d CounterBefore=0x%08x\n", tries, lt, counter_before);
         //printf("MISS%d: LT=%d  CounterAfter=0x%08x\n", tries, lt, counter_after);         
         
         if ((lt % 2) == 1) {
           // Table counting misses so should see increment
           EXPECT_PRED_FORMAT2(CmpHelperIntHex , counter_before + 1, counter_after);
         } else {
           // Table counting hits so should be no change
           EXPECT_PRED_FORMAT2(CmpHelperIntHex , counter_before , counter_after);
         }

         // Stash counter
         counter_last[lt] = counter_after;
         
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
     uint64_t ONE = UINT64_C(1);
     uint64_t HI  = UINT64_C(0xFFFFFFFF00000000);
     uint64_t ALL = UINT64_C(0xFFFFFFFFFFFFFFFF);
     uint64_t FEW = UINT64_C(0xF); // FatalErrorWarn
     uint64_t NON = UINT64_C(0);
     uint64_t TOP = UINT64_C(1) << 63;
     uint64_t pipes, stages, types, rows_tabs, cols, flags;
     pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;     
     pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
     tu.set_debug(false);
     tu.set_evaluate_all(true, false); // Don't test evaluateAll
     tu.set_free_on_exit(true);

     // Just to stop compiler complaining about unused vars
     flags = FEW;
     tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

     // Instantiate whole chip and fish out objmgr
     RmtObjectManager *om = tu.get_objmgr();
     ASSERT_TRUE(om != NULL);
     // Lookup this Pipe/Stage MAU and MAU_COUNTERS obj
     Mau *mau = om->mau_lookup(pipe, stage);
     ASSERT_TRUE(mau != NULL);
     std::array<MauLookupResult,16> hit{};
     std::array<MauLookupResult,16> miss{};
     for (int i = 0; i < 16; i++) {
       hit[i].init(mau,mau->mau_result_bus());
       miss[i].init(mau,mau->mau_result_bus());
       hit[i].reset();
       miss[i].reset();
       if (i<4) {
         hit[i].set_active(true);
         hit[i].set_match(true);
         hit[i].set_logical_table(i);
         hit[i].set_match_buses( 0x0001 );
         miss[i].set_active(true);
         miss[i].set_match(false);
         miss[i].set_logical_table(i);
       }
     }

     // Set up mask/default vals so that the PFE enables are always
     //  set for all hits and misses - needed so the counters count.
     uint32_t data_mask = 0x800000;
     uint32_t data_dflt = 0x800000;
     uint32_t data_miss = 0x800000;
     for (int bus_num=0;bus_num<16;++bus_num) {
       for (int xm_tm = 0; xm_tm <= 1; xm_tm++) {
         auto a_d_mask = &mm_regs.mau_meter_adr_mask[xm_tm][bus_num];
         tu.OutWord((void*)a_d_mask, data_mask);
         auto a_d_dflt = &mm_regs.mau_meter_adr_default[xm_tm][bus_num];
         tu.OutWord((void*)a_d_dflt, data_dflt);
       }
     }
     for (int log_table=0;log_table<16;++log_table) {
       // Miss val
       auto a_d_miss = &mm_regs.mau_meter_adr_miss_value[log_table];
       tu.OutWord((void*)a_d_miss, data_miss);
     }
      
     
     // Use counter_shift 4 so bottom 4-bits always same in/out
     // Means 16-bit instructions so 8 possible subwords
     int counter_shift = 4;
     int vpn_min[4];
     int vpn_max[4];
     MauStatefulCounters *mau_stateful_counters = mau->mau_stateful_counters();
     ASSERT_TRUE(mau_stateful_counters != NULL);
     // Setup tables and stateful counters
     for (int lt = 0; lt < 4; lt ++) {
       // Cnt HITs for even tables, MISSes for odd
       int cnt_what = ((lt % 2) == 0) ?2 :1;
       // 1-1 mapping LT->COUNTER
       int counter = lt; 
       // Use VPN ranges [8-15],[16-23],[24-31],[32-39] for LTs 0,1,2,3
       // Should make bits [5:3] of VPN portion of addr match LT+1
       vpn_min[lt] = (lt+1)*8;
       vpn_max[lt] = ((lt+2)*8)-1;
       
       // Setup 4 logical table for ingress in both stages
       tu.table_config(lt, true);    // tableN ingress
       
       // Setup stateful counters for logical tables
       // Have 1-1 mapping from LT 0-3 to ALU 0-3
       tu.stateful_cntr_config(lt, counter, cnt_what,
                               counter_shift, vpn_min[lt], vpn_max[lt]);
     }

     // UP debug now
     //flags = ALL;
     //tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

     
     // Inject hit/miss events into all 4 tables
     // Even logical tables should increment on hit
     // Odd logical tables should increment on miss
     // Returned addresses should match shifted value in stateful_log counter reg
     // Bits [5:3] of VPN portion of addr should match LT+1
     //
     // TODO: uncomment this and handle the two extra tries, first one wraps the counter, 
     //    last one counter does not change as it overflows - (tries < 65536) code below does not work!
     const int try_when_wrap_expected = 65535;
     for (int tries = 0; tries < try_when_wrap_expected + 10; tries++) { // overflows as well
       // as we are not really running the mau, have to reset resources manually
       mau->reset_resources();
       
       // send in hits to see even counters go up
       uint32_t counter_before[4], counter_after[4], addr_in, addr_out[4];
       for (unsigned int counter = 0; counter < 4; counter++) {
         auto a_stateful_log_counter = &mm_regs.mau_stateful_log_counter[counter][0];
         counter_before[counter] = tu.InWord((void*)a_stateful_log_counter);
       }

       MauExecuteState state(om->phv_create(),om->phv_create(),0,0); // need to fake state out too
       state.match_phv_ = om->phv_create();
       mau_stateful_counters->tick_counters( hit, &state );

       for (unsigned int counter = 0; counter < 4; counter++) {
         auto a_stateful_log_counter = &mm_regs.mau_stateful_log_counter[counter][0];
         counter_after[counter] = tu.InWord((void*)a_stateful_log_counter);

         bool expect_inc = ((counter % 2) == 0); // hit counter
         if (expect_inc) {
           if ( tries < try_when_wrap_expected) {
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , counter_before[counter] + 1, counter_after[counter]);
           }
           else {
             // overflow bit set and vpn_min
             int alu=counter; // note 1:1 mapping
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , (1<<(counter_shift+18)) | (vpn_min[alu]<<(counter_shift+9)),
                                                    counter_after[counter]);
           }
         }
         else {
           EXPECT_PRED_FORMAT2(CmpHelperIntHex , counter_before[counter], counter_after[counter]);
         }
       }

       for (int alu=0;alu<4;++alu) {
         addr_in = (0x7 << 23) | alu; // enabled address

         addr_out[alu] = mau_stateful_counters->get_meter_addr(alu, addr_in, UINT64_C(0));
         // direct mapping. TODO change this to spot errors using the wrong one (also need to change setup
         //   function to make this possible)
         int counter = alu; 
         bool expect_inc = ((counter % 2) == 0); // hit counter
         if (expect_inc) {
           if ( tries < (try_when_wrap_expected+1)) { // uses counter_before, so wrap occurs one later
             // TODO: 7fff..  should probably be related to counter_shift
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , addr_in | ((counter_before[counter]<<counter_shift) & 0x7fffff) , addr_out[alu] );
           }
           else {
             // vpn_min check depends on lt==counter==alu.  Overflow squashes oring in of addr_in.
             //uint32_t squashed_addr = addr_in & ((1 << 23)-1);
             //EXPECT_PRED_FORMAT2(CmpHelperIntHex , squashed_addr | (vpn_min[alu]<<(counter_shift+13)) , addr_out[alu] );
             // After the fix for XXX the squashed address has no instruction, so the VPN range check fails
             //  and the address is zeroed out
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , 0 , addr_out[alu] );
           }
         }
         else {
           EXPECT_PRED_FORMAT2(CmpHelperIntHex , addr_in , addr_out[alu] );
         }
       }
       
       // now send in misses to see odd counters go up
       for (unsigned int counter = 0; counter < 4; counter++) {
         auto a_stateful_log_counter = &mm_regs.mau_stateful_log_counter[counter][0];
         counter_before[counter] = tu.InWord((void*)a_stateful_log_counter);
       }

       mau->reset_resources();
       
       mau_stateful_counters->tick_counters( miss, &state );

       for (unsigned int counter = 0; counter < 4; counter++) {
         auto a_stateful_log_counter = &mm_regs.mau_stateful_log_counter[counter][0];
         counter_after[counter] = tu.InWord((void*)a_stateful_log_counter);

         bool expect_inc = ((counter % 2) == 1); // miss counter
         if (expect_inc) {
           if ( tries < try_when_wrap_expected) {
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , counter_before[counter] + 1, counter_after[counter]);
           }
           else {
             // overflow bit set and vpn_min
             int alu=counter; // note 1:1 mapping
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , (1<<(counter_shift+18)) | (vpn_min[alu]<<(counter_shift+9)),
                                                    counter_after[counter]);
           }
         }
         else {
           EXPECT_PRED_FORMAT2(CmpHelperIntHex , counter_before[counter], counter_after[counter]);
         }
       }


       for (int alu=0;alu<4;++alu) {
         addr_in = (0x7 << 23) | alu; // enabled address
         
         addr_out[alu] = mau_stateful_counters->get_meter_addr(alu, addr_in, UINT64_C(0));
         int counter = alu; // direct mapping. TODO change this to spot errors using the wrong one
         
         bool expect_inc = ((counter % 2) == 1); // miss counter
         
         if (expect_inc) {
           if ( tries < (try_when_wrap_expected+1)) { // uses counter_before, so wrap occurs one later
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , addr_in | ((counter_before[counter]<<counter_shift) & 0x7fffff) , addr_out[alu] );
           }
           else {
             // vpn_min check depends on lt==counter==alu. Overflow squashes oring in of addr_in.
             //uint32_t squashed_addr = addr_in & ((1 << 23)-1);
             //EXPECT_PRED_FORMAT2(CmpHelperIntHex , squashed_addr | (vpn_min[alu]<<(counter_shift+13)) , addr_out[alu] );
             // After the fix for XXX the squashed address has no instruction, so the VPN range check fails
             //  and the address is zeroed out
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , 0 , addr_out[alu] );
           }
         }
         else {
           EXPECT_PRED_FORMAT2(CmpHelperIntHex , addr_in , addr_out[alu] );
         }
       }

       for (int lt = 0; lt < 16; lt++) {
         uint32_t imm_data_in  = 0;
         uint32_t imm_data_out = 0;

         // TODO: test overflow using logical meter ctl
         // TODO: test in range bit
         // TODO: test VPN subtraction
         if (lt >= 4) {
           imm_data_out = mau_stateful_counters->get_immediate_data(lt, imm_data_in, 0 );
           EXPECT_PRED_FORMAT2(CmpHelperIntHex , imm_data_in , imm_data_out );
         }
         else {
           int alu=lt;
           int counter=lt;
           imm_data_out = mau_stateful_counters->get_immediate_data(lt, imm_data_in, addr_out[alu] );
           int exp_op = ((addr_out[alu] >> 23) & 0x7) << 23;
           //printf("counter_before %08x\n",counter_before[counter]);
           bool expect_inc = ((counter % 2) == 1); // miss counter
           if (expect_inc) {
             if ((counter_before[counter] >> 22) & 0x1f) { // TODO: why is this 22, not 23?
               // instr or squash bit, zero out the op from the meter_addr
               exp_op=0;
             }
             // as not a lot is programmed up here the output should just be the low part of the counter
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , exp_op | (counter_before[counter] & ((1<<(counter_shift+17))-1)) , imm_data_out );
           }
           else {
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , exp_op | imm_data_in , imm_data_out );
           }

         }
       }



     }
       

     // Schtum
     tu.finish_test();
     tu.quieten_log_flags();
   }


TEST(BFN_TEST_NAME(CountersTest),StatefulClear) {
     if (counter_print) RMT_UT_LOG_INFO("test_counter_stateful_clear()\n");    
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
     uint64_t ONE = UINT64_C(1);
     uint64_t HI  = UINT64_C(0xFFFFFFFF00000000);
     uint64_t ALL = UINT64_C(0xFFFFFFFFFFFFFFFF);
     uint64_t FEW = UINT64_C(0xF); // FatalErrorWarn
     uint64_t NON = UINT64_C(0);
     uint64_t TOP = UINT64_C(1) << 63;
     uint64_t pipes, stages, types, rows_tabs, cols, flags;
     pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;     
     pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
     tu.set_debug(false);
     tu.set_evaluate_all(true, false); // Don't test evaluateAll
     tu.set_free_on_exit(true);

     // Just to stop compiler complaining about unused vars
     flags = FEW;
     tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

     // Instantiate whole chip and fish out objmgr
     RmtObjectManager *om = tu.get_objmgr();
     ASSERT_TRUE(om != NULL);
     // Lookup this Pipe/Stage MAU and MAU_COUNTERS obj
     Mau *mau = om->mau_lookup(pipe, stage);
     ASSERT_TRUE(mau != NULL);
     std::array<MauLookupResult,16> hit{};
     std::array<MauLookupResult,16> miss{};
     for (int i = 0; i < 16; i++) {
       hit[i].init(mau,mau->mau_result_bus());
       miss[i].init(mau,mau->mau_result_bus());
       hit[i].reset();
       miss[i].reset();
       if (i<4) {
         hit[i].set_active(true);
         hit[i].set_match(true);
         hit[i].set_logical_table(i);
         hit[i].set_match_buses( 0x0001 );
         miss[i].set_active(true);
         miss[i].set_match(false);
         miss[i].set_logical_table(i);
       }
     }

     // Set up mask/default vals so that the PFE enables are always
     //  set for all hits and misses - needed so the counters count.
     uint32_t data_mask = 0x800000;
     uint32_t data_dflt = 0x800000;
     uint32_t data_miss = 0x800000;
     for (int bus_num=0;bus_num<16;++bus_num) {
       for (int xm_tm = 0; xm_tm <= 1; xm_tm++) {
         auto a_d_mask = &mm_regs.mau_meter_adr_mask[xm_tm][bus_num];
         tu.OutWord((void*)a_d_mask, data_mask);
         auto a_d_dflt = &mm_regs.mau_meter_adr_default[xm_tm][bus_num];
         tu.OutWord((void*)a_d_dflt, data_dflt);
       }
     }
     for (int log_table=0;log_table<16;++log_table) {
       // Miss val
       auto a_d_miss = &mm_regs.mau_meter_adr_miss_value[log_table];
       tu.OutWord((void*)a_d_miss, data_miss);
     }
      
     
     // Use counter_shift 4 so bottom 4-bits always same in/out
     // Means 16-bit instructions so 8 possible subwords
     int counter_shift = 4;
     int vpn_min[4];
     int vpn_max[4];
     MauStatefulCounters *mau_stateful_counters = mau->mau_stateful_counters();
     ASSERT_TRUE(mau_stateful_counters != NULL);
     // Setup tables and stateful counters
     for (int lt = 0; lt < 4; lt ++) {
       // Cnt HITs for even tables, MISSes for odd
       int cnt_what = ((lt % 2) == 0) ?2 :1;
       // 1-1 mapping LT->COUNTER
       int counter = lt; 
       // Use VPN ranges [8-15],[16-23],[24-31],[32-39] for LTs 0,1,2,3
       // Should make bits [5:3] of VPN portion of addr match LT+1
       vpn_min[lt] = (lt+1)*8;
       vpn_max[lt] = ((lt+2)*8)-1;
       
       // Setup 4 logical table for ingress in both stages
       tu.table_config(lt, true);    // tableN ingress
       
       // Setup stateful counters for logical tables
       // Have 1-1 mapping from LT 0-3 to ALU 0-3
       tu.stateful_cntr_config(lt, counter, cnt_what,
                               counter_shift, vpn_min[lt], vpn_max[lt]);
       // Setup each counter to be a StatefulClear counter
       tu.set_stateful_clear_instruction_regs(mm_regs, counter, counter_shift-3,
                                              vpn_min[lt], vpn_max[lt] );

     }

     // UP debug now
     flags = ALL;
     tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

     // Try and kick off a sequence of clears for LT=x/COUNTER=x (1-1 mapping)
     mau_stateful_counters->push_pop_stateful_instr(true, 0, 0u, UINT64_C(1018) );
     mau_stateful_counters->push_pop_stateful_instr(true, 0, 0u, UINT64_C(1029) );

     mau_stateful_counters->push_pop_stateful_instr(true, 1, 0u, UINT64_C(1138) );
     mau_stateful_counters->push_pop_stateful_instr(true, 1, 0u, UINT64_C(1149) );

     mau_stateful_counters->push_pop_stateful_instr(true, 2, 0u, UINT64_C(1258) );
     mau_stateful_counters->push_pop_stateful_instr(true, 2, 0u, UINT64_C(1269) );

     mau_stateful_counters->push_pop_stateful_instr(true, 3, 0u, UINT64_C(1378) );
     mau_stateful_counters->push_pop_stateful_instr(true, 3, 0u, UINT64_C(1389) );
     
     
#ifdef NOT_YET     
     // Inject hit/miss events into all 4 tables
     // Even logical tables should increment on hit
     // Odd logical tables should increment on miss
     // Returned addresses should match shifted value in stateful_log counter reg
     // Bits [5:3] of VPN portion of addr should match LT+1
     //
     // TODO: uncomment this and handle the two extra tries, first one wraps the counter, 
     //    last one counter does not change as it overflows - (tries < 65536) code below does not work!
     const int try_when_wrap_expected = 65535;
     for (int tries = 0; tries < try_when_wrap_expected + 10; tries++) { // overflows as well
       // as we are not really running the mau, have to reset resources manually
       mau->reset_resources();
       
       // send in hits to see even counters go up
       uint32_t counter_before[4], counter_after[4], addr_in, addr_out[4];
       for (unsigned int counter = 0; counter < 4; counter++) {
         auto a_stateful_log_counter = &mm_regs.mau_stateful_log_counter[counter][0];
         counter_before[counter] = tu.InWord((void*)a_stateful_log_counter);
       }
       
       MauExecuteState state(om->phv_create(),om->phv_create(),0,0); // need to fake state out too
       state.match_phv_ = om->phv_create();
       mau_stateful_counters->tick_counters( hit, &state );

       for (unsigned int counter = 0; counter < 4; counter++) {
         auto a_stateful_log_counter = &mm_regs.mau_stateful_log_counter[counter][0];
         counter_after[counter] = tu.InWord((void*)a_stateful_log_counter);

         bool expect_inc = ((counter % 2) == 0); // hit counter
         if (expect_inc) {
           if ( tries < try_when_wrap_expected) {
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , counter_before[counter] + 1, counter_after[counter]);
           }
           else {
             // overflow bit set and vpn_min
             int alu=counter; // note 1:1 mapping
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , (1<<(counter_shift+18)) | (vpn_min[alu]<<(counter_shift+9)),
                                                    counter_after[counter]);
           }
         }
         else {
           EXPECT_PRED_FORMAT2(CmpHelperIntHex , counter_before[counter], counter_after[counter]);
         }
       }

       for (int alu=0;alu<4;++alu) {
         addr_in = (0x7 << 23) | alu; // enabled address

         addr_out[alu] = mau_stateful_counters->get_meter_addr(alu, addr_in, UINT64_C(0));
         // direct mapping. TODO change this to spot errors using the wrong one (also need to change setup
         //   function to make this possible)
         int counter = alu; 
         bool expect_inc = ((counter % 2) == 0); // hit counter
         if (expect_inc) {
           if ( tries < (try_when_wrap_expected+1)) { // uses counter_before, so wrap occurs one later
             // TODO: 7fff..  should probably be related to counter_shift
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , addr_in | ((counter_before[counter]<<counter_shift) & 0x7fffff) , addr_out[alu] );
           }
           else {
             // vpn_min check depends on lt==counter==alu.  Overflow squashes oring in of addr_in.
             uint32_t squashed_addr = addr_in & ((1 << 23)-1);
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , squashed_addr | (vpn_min[alu]<<(counter_shift+13)) , addr_out[alu] );
           }
         }
         else {
           EXPECT_PRED_FORMAT2(CmpHelperIntHex , addr_in , addr_out[alu] );
         }
       }
       
       // now send in misses to see odd counters go up
       for (unsigned int counter = 0; counter < 4; counter++) {
         auto a_stateful_log_counter = &mm_regs.mau_stateful_log_counter[counter][0];
         counter_before[counter] = tu.InWord((void*)a_stateful_log_counter);
       }

       mau->reset_resources();
       
       MauExecuteState state(om->phv_create(),om->phv_create(),0,0); // need to fake state out too
       state.match_phv_ = om->phv_create();
       mau_stateful_counters->tick_counters( miss, &state );

       for (unsigned int counter = 0; counter < 4; counter++) {
         auto a_stateful_log_counter = &mm_regs.mau_stateful_log_counter[counter][0];
         counter_after[counter] = tu.InWord((void*)a_stateful_log_counter);

         bool expect_inc = ((counter % 2) == 1); // miss counter
         if (expect_inc) {
           if ( tries < try_when_wrap_expected) {
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , counter_before[counter] + 1, counter_after[counter]);
           }
           else {
             // overflow bit set and vpn_min
             int alu=counter; // note 1:1 mapping
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , (1<<(counter_shift+18)) | (vpn_min[alu]<<(counter_shift+9)),
                                                    counter_after[counter]);
           }
         }
         else {
           EXPECT_PRED_FORMAT2(CmpHelperIntHex , counter_before[counter], counter_after[counter]);
         }
       }


       for (int alu=0;alu<4;++alu) {
         addr_in = (0x7 << 23) | alu; // enabled address
         
         addr_out[alu] = mau_stateful_counters->get_meter_addr(alu, addr_in, UINT64_C(0));
         int counter = alu; // direct mapping. TODO change this to spot errors using the wrong one
         
         bool expect_inc = ((counter % 2) == 1); // miss counter
         
         if (expect_inc) {
           if ( tries < (try_when_wrap_expected+1)) { // uses counter_before, so wrap occurs one later
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , addr_in | ((counter_before[counter]<<counter_shift) & 0x7fffff) , addr_out[alu] );
           }
           else {
             // vpn_min check depends on lt==counter==alu. Overflow squashes oring in of addr_in.
             uint32_t squashed_addr = addr_in & ((1 << 23)-1);
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , squashed_addr | (vpn_min[alu]<<(counter_shift+13)) , addr_out[alu] );
           }
         }
         else {
           EXPECT_PRED_FORMAT2(CmpHelperIntHex , addr_in , addr_out[alu] );
         }
       }

       for (int lt = 0; lt < 16; lt++) {
         uint32_t imm_data_in  = 0;
         uint32_t imm_data_out = 0;

         // TODO: test overflow using logical meter ctl
         // TODO: test in range bit
         // TODO: test VPN subtraction
         if (lt >= 4) {
           imm_data_out = mau_stateful_counters->get_immediate_data(lt, imm_data_in, 0 );
           EXPECT_PRED_FORMAT2(CmpHelperIntHex , imm_data_in , imm_data_out );
         }
         else {
           int alu=lt;
           int counter=lt;
           imm_data_out = mau_stateful_counters->get_immediate_data(lt, imm_data_in, addr_out[alu] );
           int exp_op = ((addr_out[alu] >> 23) & 0x7) << 23;
           //printf("counter_before %08x\n",counter_before[counter]);
           bool expect_inc = ((counter % 2) == 1); // miss counter
           if (expect_inc) {
             if ((counter_before[counter] >> 22) & 0x1f) { // TODO: why is this 22, not 23?
               // instr or squash bit, zero out the op from the meter_addr
               exp_op=0;
             }
             // as not a lot is programmed up here the output should just be the low part of the counter
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , exp_op | (counter_before[counter] & ((1<<(counter_shift+17))-1)) , imm_data_out );
           }
           else {
             EXPECT_PRED_FORMAT2(CmpHelperIntHex , exp_op | imm_data_in , imm_data_out );
           }

         }
       }



     }
#endif // NOT_YET
       

     // Schtum
     tu.finish_test();
     tu.quieten_log_flags();
   }


}
