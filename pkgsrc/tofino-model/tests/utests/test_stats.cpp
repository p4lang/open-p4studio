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
#include <packet.h>
#include <rmt-sweeper.h>
#include <model_core/rmt-dru-callback.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool stats_print = false;

using namespace std;
using namespace MODEL_CHIP_NAMESPACE;

uint32_t stats_callback_prev_addr = 0u;
uint32_t stats_callback_addr      = 0u;
uint64_t stats_callback_prev_msg1 = UINT64_C(0);
uint64_t stats_callback_msg1      = UINT64_C(0);
uint64_t stats_callback_bytes = UINT64_C(0);
uint64_t stats_callback_pkts  = UINT64_C(0);
uint64_t stats_callback_data  = UINT64_C(0);
int      stats_callback_logtab = -1;
int      stats_callback_vpn = -1;
int      stats_callback_index = -1;
int      stats_callbacks = 0;
int      stats_callback_pktcnt_width = 0;
void     stats_callback_print(int asic, uint8_t *stats_data, int len) {
  uint64_t msg0 = *(uint64_t*)stats_data;
  uint64_t msg1 = *(uint64_t*)(stats_data+8);
  uint32_t typ  = static_cast<uint32_t>((msg0 >>  0) & 0x3);        //  2 bits
  uint32_t addr = static_cast<uint32_t>((msg0 >> 16) & 0x1FFFFFFF); // 29 bits
  // Keep previous addr/msg1 - handy for checking 2xdump in 64b+64b format case
  stats_callback_prev_addr = stats_callback_addr;
  stats_callback_prev_msg1 = stats_callback_msg1;
  stats_callback_addr = addr;
  stats_callback_msg1 = msg1;
  int pipe   = (addr >> (4+4+6+10+3+0)) & 0x3;
  int stage  = (addr >>   (4+6+10+3+0)) & 0xF;
  int logtab = (addr >>     (6+10+3+0)) & 0xF;
  int vpn    = (addr >>       (10+3+0)) & 0x3F;
  int index  = (addr >>          (3+0)) & 0x3FF;
  int subw   = (addr >>            (0)) & 0x7;
  if ((typ == 0) || (typ == 2)) {
    stats_callbacks++;
    stats_callback_data = msg1;
    int      shift = (typ==0) ?28 :stats_callback_pktcnt_width;
    uint64_t mask  = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64-shift);
    uint64_t bytes = (msg1 >> shift);
    uint64_t pkts  = (msg1 & mask);
    if ((typ==2) && (stats_callback_pktcnt_width == 64)) {
      if (stats_callback_addr == stats_callback_prev_addr+4) {
        pkts = UINT64_C(0); bytes = msg1;
      } else {
        pkts = msg1; bytes = UINT64_C(0);
      }
    }
    printf("Stats%sCallback(type=%d,chip=%d,pipe=%d,stage=%d,"
           "lt=%d,vpn=%d,index=%d,sub=%d) = 0x%016" PRIx64 " "
           "[Pkts=%" PRId64 ",Bytes=%" PRId64 "]\n",
           (typ==0) ?"LRT" :"Dump",
           typ, asic, pipe, stage, logtab, vpn, index, subw, msg1, pkts, bytes);
    stats_callback_bytes += bytes;
    stats_callback_pkts  += pkts;
    stats_callback_logtab = logtab;
    stats_callback_vpn = vpn;
    stats_callback_index = index;
  } else {
    printf("Callback(type=%d,chip=%d,pipe=%d,stage=%d)\n", typ, asic, pipe, stage);
  }
}


  TEST(BFN_TEST_NAME(StatsTest),Basic) {
    if (stats_print) RMT_UT_LOG_INFO("test_stats_basic()\n");
    int chip = 202;
    int pipe = 0;
    int stage = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);

     // Register Stats Dump callback func with Model
     GLOBAL_MODEL.get()->
         register_callback_dru_lrt_update(stats_callback_print);


    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Switch on retention of/access to FullResStats
    MauStatsAlu::kKeepFullResStats = true;
    MauMemory::kKeepFullResStats = true;
    MauMemory::kAccessFullResStats = true;

    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Switch on fatal/error/warn messages - happens by default these days
    //tu.update_log_flags(0, 0, 0, 0, 0, UINT64_C(0x7), ALL);

    // Switch *ON* specific outputs
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeParser;
    //flags = RmtDebug::kRmtDebugParserParseLoop;
    //tu.update_log_flags(pipes, 0, types, 0, 0, flags, ALL);

    // Switch *OFF* certain output
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeMauTcam;
    //flags = RmtDebug::kRmtDebugMauTcam_Tcam3LookupDetail|RmtDebug::kRmtDebugMauTcam_Tcam3DebugMiss;
    //tu.update_log_flags(pipes, 0, types, 0, 0, 0, ~flags);

    // DEBUG setup done!!!!!!!!!!!!!!!!!!!!!!


    // Instantiate whole chip and fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    // Lookup this Pipe/Stage MAU and MAU_ADDR_DIST obj
    Mau *mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    MauAddrDist *adist = mau->mau_addr_dist();
    ASSERT_TRUE(adist != NULL);
    RmtSweeper *sweeper = om->sweeper_get();
    ASSERT_TRUE(sweeper != NULL);



    // Set all PHV ingress/egress threads for our 2 stages (chip9!)
    for (stage = 0; stage < 2; stage++) {
      tu.set_phv_range_all(stage, false);
      // Then set 1st 32 of the 64x32 64x8 to be ingress
      // and the 1st 48 of the 96x16 to be ingress
      tu.set_phv_ranges(stage, 32,0, 32,0, 32,16,0, true);
    }

    // Setup ingress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepAction, true);
    tu.set_dependency(1, TestUtil::kDepAction, true);
    // Setup egress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepConcurrent, false);
    tu.set_dependency(1, TestUtil::kDepConcurrent, false);

    // Setup single logical table for ingress in stage0
    tu.table_config(0, 0, true);    // stage0  table0  ingress

    // Then setup some STATS SRAMs
    // NB format 25 is Packet+Byte[1 entry,64b PKT,64b BYTE]
    // NB NO MAPRAMS IN cols 0-5 ANY MORE
    // NB StatsALUs only on logrows 13,9,5,1 (RHS of physrows 0,2,4,6)
    tu.rwram_config(0, 2, 6,                           // stage0 row2 col6
                    TestUtil::kUnitramTypeStats, 0, 0, // type vpn0=0 vpn1=0
                    25, 0, false, true);     // s_format=25 log_table=0 egress=F use_deferred=true
    tu.rwram_config(0, 2, 7,                           // stage0 row2 col7
                    TestUtil::kUnitramTypeStats, 0, 0, // type vpn0=0 vpn1=0
                    25, 0, false, true);     // s_format=25 log_table=0 egress=F use_deferred=true

    // Flush any OPs
    sweeper->sweep_increment(UINT64_C(0)); // Dequeues OPs regardless of T passed


    // UP debug now
    flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


    // Write initial val full-res counter (using stats addr we intend to use) but abusing DataSize field to contain 0x3
    uint64_t fwaddr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2);
    fwaddr |= (3 << 28);
    uint64_t VAL0 = UINT64_C(0x0012345678900000); // Initialize full-res stats to non-zero val
    tu.IndirectWrite(fwaddr, VAL0, VAL0);


    // Create a simple PHV (so we can set a
    // pkt length increment for byte counters)
    Eop eop;
    Phv *phv = tu.phv_alloc();
    int stats_vpn;
    int stats_index;
    uint32_t stats_addr;

    //Address::kGlobalAddrEnable = false;
    bool addr_enable = true;
    int pktlen = 100;
    int n_pkts = 10;

    // Now loop pushing packets in.....
    for (int i = 0; i < n_pkts; i++) {
      eop.set_ingress_eopinfo(pktlen, 0);

      if ((i % 2) == 0) stats_vpn = 7; else stats_vpn = 8;
      stats_index = 15;
      stats_addr = (stats_vpn << 15) | (stats_index << 5);

      // Now loop calling:
      //  MAU->reset_resources()
      //  MAU-ADDR_DIST->distrib_stats_addresses(), MAU->defer_stats_addresses()
      //  to distribute/defer stats addresses
      //  MAU->process_for_tests to get StatsALU to do its work
      //
      // NB actual stats address driven should NOT include bottom 2 LSBs as
      // these should always be zero and will be recreated at SRAM (hence >>2
      // in call the addresses below)
      // Also we switch on MSB (bit19) to indicate OP=stats
      uint32_t stats_dist_addr = (stats_addr>>2);
      if (addr_enable) stats_dist_addr |= (1u<<19);
      mau->reset_resources();

      // this is what update_addresses used to do
      // Use eopnum=0
      Teop *null_teop = NULL;
      adist->distrib_stats_addresses(0, true, stats_dist_addr, null_teop);
      adist->defer_stats_addresses(0, true, stats_dist_addr, null_teop, 0);

      mau->process_for_tests(phv, eop);
    }

    uint64_t data0, data1;
    uint64_t addr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats,
                                                   0, ((7<<15)|(15<<5))>>2);
    int type = (addr >> 30) & 0x3;

    // VPN7
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=7,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2, &data0, &data1);
    printf("1. Addr=%016" PRIx64 " Type=%d Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n",
           addr, type, data0, data1);
    if (Address::kGlobalAddrEnable || addr_enable) {
      // Half packets go to VPN 7 hence divide by 2
      EXPECT_EQ(data0, static_cast<uint64_t>(n_pkts/2));
      EXPECT_EQ(data1, static_cast<uint64_t>(pktlen*n_pkts/2));
    } else {
      EXPECT_EQ(data0, NON);
      EXPECT_EQ(data1, NON);
    }

    // Read full-res counter using similar addr but abusing DataSize field to contain 0x3
    uint64_t fdata0, fdata1;
    uint64_t faddr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2);
    faddr |= (3 << 28);
    //int ftype = (faddr >> 30) & 0x3;
    fdata0 = VAL0; fdata1 = VAL0; // NB initial val full-res counter may not be zero
    tu.IndirectRead(faddr, &fdata0, &fdata1);
    printf("1. FAdr=%016" PRIx64 " Type=%d FDat0=%016" PRIx64 " FDat1=%016" PRIx64 "\n",
           faddr, type, fdata0, fdata1);
    if (Address::kGlobalAddrEnable || addr_enable) {
      EXPECT_EQ(fdata0, static_cast<uint64_t>(VAL0 + (n_pkts/2)));
      EXPECT_EQ(fdata1, static_cast<uint64_t>(VAL0 + (pktlen*n_pkts/2)));
    } else {
      EXPECT_EQ(fdata0, VAL0);
      EXPECT_EQ(fdata1, VAL0);
    }

    // VPN8
    data0 = NON; data1 = NON;
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=8,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((8<<15)|(15<<5))>>2, &data0, &data1);
    printf("2. Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n", data0, data1);
    // Other half packets go to VPN 8 hence divide by 2
    if (Address::kGlobalAddrEnable || addr_enable) {
      EXPECT_EQ(data0, static_cast<uint64_t>(n_pkts/2));
      EXPECT_EQ(data1, static_cast<uint64_t>(pktlen*n_pkts/2));
    } else {
      EXPECT_EQ(data0, NON);
      EXPECT_EQ(data1, NON);
    }

    // Movereg test
    // Read addr(vpn=7,index=115) - expect to be 0
    // Then movereg 8,15->7,115 and initialise 8,15
    // Should see final val of 8,15 now in 7,115
    //
    // First off configure movereg to say direct stats and deferred stats
    //tu.OutWord(&mau_reg_map.rams.match.adrdist.movereg_ad_ctl[0], (1<<10)|(1<<6));
    // Configure LT 0 for Stats movereg
    tu.OutWord(&mau_reg_map.rams.match.adrdist.movereg_ad_direct[0], (1<<0));
    // Now configure for StatsALU[1] rather than LT[0]
    tu.OutWord(&mau_reg_map.rams.match.adrdist.movereg_stats_ctl[1], (1<<5)|(1<<3));
    // And finally config StatsALU->LT xbar to be enabled for LT[0] and StatsALU[1]
    tu.OutWord(&mau_reg_map.rams.match.adrdist.movereg_ad_stats_alu_to_logical_xbar_ctl[0], 0x5);
    data0 = NON; data1 = NON;
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=7,index=115) - expect 0
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(115<<5))>>2, &data0, &data1);
    printf("3. Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n", data0, data1);
    EXPECT_EQ(data0, NON);
    EXPECT_EQ(data1, NON);

    int instr_push_table_move_reg = 0, instr_pop_table_move_reg = 0;
    uint64_t push_addr = UINT64_C(0), pop_addr = UINT64_C(0);
    // Load 7,115
    instr_push_table_move_reg = 0x8000000|((0)<<20)|(   ( ((7)<<10) | 115 ) & 0x7ffff   );
    push_addr = TestUtil::make_instruction_address(pipe, 0, 0, instr_push_table_move_reg);
    tu.IndirectWrite(push_addr, UINT64_C(0), UINT64_C(0));
    // Load 8,15
    instr_push_table_move_reg = 0x8000000|((0)<<20)|(   ( ((8)<<10) |  15 ) & 0x7ffff   );
    push_addr = TestUtil::make_instruction_address(pipe, 0, 0, instr_push_table_move_reg);
    tu.IndirectWrite(push_addr, UINT64_C(0), UINT64_C(0));
    // Pop - should see 8,15 copied to 7,115
    instr_pop_table_move_reg  = 0x1e00000|((0)<<0);
    pop_addr = TestUtil::make_instruction_address(pipe, 0, 0, instr_pop_table_move_reg);
    tu.IndirectWrite(pop_addr, UINT64_C(0), UINT64_C(0));
    // Pop again - should see 8,15 initialised
    instr_pop_table_move_reg  = 0x1e00000|((0)<<0)|(1<<5);
    pop_addr = TestUtil::make_instruction_address(pipe, 0, 0, instr_pop_table_move_reg);
    tu.IndirectWrite(pop_addr, UINT64_C(0), UINT64_C(0));

    // Read vaddr for stage0, STATS, logtab0, addr(vpn=7,index=115) - expect npkts/2 etc
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(115<<5))>>2, &data0, &data1);
    printf("4. Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n", data0, data1);
    if (Address::kGlobalAddrEnable || addr_enable) {
      EXPECT_EQ(data0, static_cast<uint64_t>(n_pkts/2));
      EXPECT_EQ(data1, static_cast<uint64_t>(pktlen*n_pkts/2));
      printf("Stats entry MOVED from 8,15->7,115\n");
    }
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=8,index=15) - expect ALL_ONES
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((8<<15)|(15<<5))>>2, &data0, &data1);
    printf("5. Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n", data0, data1);
    if (Address::kGlobalAddrEnable || addr_enable) {
      EXPECT_EQ(data0, static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF));
      EXPECT_EQ(data1, static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF));
      printf("Stats entry 8,15 INITIALIZED\n");
    }

    // Finally trigger a stats dump - this will Q - need to call sweep to trigger
    // Since we're using 64b+64b format we will get 2x dump callbacks
    // After this pkt_cnt will be in stats_callback_prev_msg1
    //       and byte_cnt will be in stats_callback_msg1
    stats_callback_pktcnt_width = 64;
    stats_callback_prev_addr = stats_callback_addr = 0u;
    stats_callback_prev_msg1 = stats_callback_msg1 = UINT64_C(0);
    stats_callback_data = UINT64_C(0);
    stats_callbacks = 0;
    int dlt = 0;
    uint32_t daddr = ((7<<15)|(15<<5))>>2; // VPN 7, Index 15
    int instr_dump_stats_word = 0x1000000|((dlt & 0xF)<<19)|(daddr & 0x7FFFF);
    uint64_t iaddr = TestUtil::make_instruction_address(0,0,0,instr_dump_stats_word);
    int before_n_callbacks = stats_callbacks;
    tu.IndirectWrite(iaddr, UINT64_C(0), UINT64_C(0));
    sweeper->sweep_increment(UINT64_C(0)); // Dequeues OPs regardless of T passed
    int after_n_callbacks =  stats_callbacks;
    EXPECT_EQ(before_n_callbacks+2,after_n_callbacks);
    EXPECT_EQ(stats_callback_prev_msg1, static_cast<uint64_t>(n_pkts/2));
    EXPECT_EQ(stats_callback_msg1, static_cast<uint64_t>(pktlen*n_pkts/2));
    EXPECT_EQ(stats_callback_addr, 4+stats_callback_prev_addr);

    stats_callback_pktcnt_width = 0;
    // Free
    tu.phv_free(phv);
    // Schtum
    tu.finish_test();
    tu.quieten_log_flags();
  }


  TEST(BFN_TEST_NAME(StatsTest),WithLockedTable) {
    if (stats_print) RMT_UT_LOG_INFO("test_stats_with_locked_table()\n");
    int chip = 202;
    int pipe = 0;
    int stage = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);

     // Register Stats Dump callback func with Model
     GLOBAL_MODEL.get()->
         register_callback_dru_lrt_update(stats_callback_print);


    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Switch on retention of/access to FullResStats
    MauStatsAlu::kKeepFullResStats = true;
    MauMemory::kKeepFullResStats = true;
    MauMemory::kAccessFullResStats = true;
    // Switch off synchronous stats ops else dump post lock will just block
    TableInfo::kSynchronousStatsOps = false;


    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Switch on fatal/error/warn messages - happens by default these days
    //tu.update_log_flags(0, 0, 0, 0, 0, UINT64_C(0x7), ALL);

    // Switch *ON* specific outputs
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeParser;
    //flags = RmtDebug::kRmtDebugParserParseLoop;
    //tu.update_log_flags(pipes, 0, types, 0, 0, flags, ALL);

    // Switch *OFF* certain output
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeMauTcam;
    //flags = RmtDebug::kRmtDebugMauTcam_Tcam3LookupDetail|RmtDebug::kRmtDebugMauTcam_Tcam3DebugMiss;
    //tu.update_log_flags(pipes, 0, types, 0, 0, 0, ~flags);

    // DEBUG setup done!!!!!!!!!!!!!!!!!!!!!!


    // Instantiate whole chip and fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    // Lookup this Pipe/Stage MAU and MAU_ADDR_DIST obj
    Mau *mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    MauAddrDist *adist = mau->mau_addr_dist();
    ASSERT_TRUE(adist != NULL);
    RmtSweeper *sweeper = om->sweeper_get();
    ASSERT_TRUE(sweeper != NULL);



    // Set all PHV ingress/egress threads for our 2 stages (chip9!)
    for (stage = 0; stage < 2; stage++) {
      tu.set_phv_range_all(stage, false);
      // Then set 1st 32 of the 64x32 64x8 to be ingress
      // and the 1st 48 of the 96x16 to be ingress
      tu.set_phv_ranges(stage, 32,0, 32,0, 32,16,0, true);
    }

    // Setup ingress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepAction, true);
    tu.set_dependency(1, TestUtil::kDepAction, true);
    // Setup egress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepConcurrent, false);
    tu.set_dependency(1, TestUtil::kDepConcurrent, false);

    // Setup single logical table for ingress in stage0
    tu.table_config(0, 0, true);    // stage0  table0  ingress

    // Then setup some STATS SRAMs
    // NB format 25 is Packet+Byte[1 entry,64b PKT,64b BYTE]
    // NB NO MAPRAMS IN cols 0-5 ANY MORE
    // NB StatsALUs only on logrows 13,9,5,1 (RHS of physrows 0,2,4,6)
    tu.rwram_config(0, 2, 6,                           // stage0 row2 col6
                    TestUtil::kUnitramTypeStats, 7, 8, // type vpn0=7 vpn1=8 (used for dump cfg)
                    25, 0, false, true);     // s_format=25 log_table=0 egress=F use_deferred=true
    tu.rwram_config(0, 2, 7,                           // stage0 row2 col7
                    TestUtil::kUnitramTypeStats, 7, 8, // type vpn0=7 vpn1=8 (used for dump cfg)
                    25, 0, false, true);     // s_format=25 log_table=0 egress=F use_deferred=true

    // Flush any OPs
    sweeper->sweep_increment(UINT64_C(0)); // Dequeues OPs regardless of T passed


    // UP debug now
    flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);



    // Create a simple PHV (so we can set a
    // pkt length increment for byte counters)
    Eop eop;
    Phv *phv = tu.phv_alloc();
    int stats_vpn;
    int stats_index;
    uint32_t stats_addr;

    //Address::kGlobalAddrEnable = false;
    bool addr_enable = true;
    int pktlen = 100;
    int n_pkts = 10;

    // Now loop pushing packets in.....
    for (int i = 0; i < n_pkts; i++) {
      eop.set_ingress_eopinfo(pktlen, 0);

      if ((i % 2) == 0) stats_vpn = 7; else stats_vpn = 8;
      stats_index = 15;
      stats_addr = (stats_vpn << 15) | (stats_index << 5);

      // Now loop calling:
      //  MAU->reset_resources()
      //  MAU-ADDR_DIST->distrib_stats_addresses(), MAU->defer_stats_addresses()
      //  to distribute/defer stats addresses
      //  MAU->process_for_tests to get StatsALU to do its work
      //
      // NB actual stats address driven should NOT include bottom 2 LSBs as
      // these should always be zero and will be recreated at SRAM (hence >>2
      // in call the addresses below)
      // Also we switch on MSB (bit19) to indicate OP=stats
      uint32_t stats_dist_addr = (stats_addr>>2);
      if (addr_enable) stats_dist_addr |= (1u<<19);
      mau->reset_resources();

      // this is what update_addresses used to do
      // Use eopnum=0
      Teop *null_teop = NULL;
      adist->distrib_stats_addresses(0, true, stats_dist_addr, null_teop);
      adist->defer_stats_addresses(0, true, stats_dist_addr, null_teop, 0);

      mau->process_for_tests(phv, eop);
    }

    uint64_t data0, data1;
    uint64_t addr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats,
                                                   0, ((7<<15)|(15<<5))>>2);
    int type = (addr >> 30) & 0x3;

    // VPN7
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=7,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2, &data0, &data1);
    printf("1. Addr=%016" PRIx64 " Type=%d Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n",
           addr, type, data0, data1);
    if (Address::kGlobalAddrEnable || addr_enable) {
      // Half packets go to VPN 7 hence divide by 2
      EXPECT_EQ(data0, static_cast<uint64_t>(n_pkts/2));
      EXPECT_EQ(data1, static_cast<uint64_t>(pktlen*n_pkts/2));
    } else {
      EXPECT_EQ(data0, NON);
      EXPECT_EQ(data1, NON);
    }

    // Read full-res counter using similar addr but abusing DataSize field to contain 0x3
    uint64_t fdata0, fdata1;
    uint64_t faddr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2);
    faddr |= (3 << 28);
    //int ftype = (faddr >> 30) & 0x3;
    fdata0 = NON; fdata1 = NON;
    tu.IndirectRead(faddr, &fdata0, &fdata1);
    printf("1. FAdr=%016" PRIx64 " Type=%d FDat0=%016" PRIx64 " FDat1=%016" PRIx64 "\n",
           faddr, type, fdata0, fdata1);
    if (Address::kGlobalAddrEnable || addr_enable) {
      EXPECT_EQ(fdata0, static_cast<uint64_t>(n_pkts/2));
      EXPECT_EQ(fdata1, static_cast<uint64_t>(pktlen*n_pkts/2));
    } else {
      EXPECT_EQ(fdata0, NON);
      EXPECT_EQ(fdata1, NON);
    }

    // VPN8
    data0 = NON; data1 = NON;
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=8,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((8<<15)|(15<<5))>>2, &data0, &data1);
    printf("2. Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n", data0, data1);
    // Other half packets go to VPN 8 hence divide by 2
    if (Address::kGlobalAddrEnable || addr_enable) {
      EXPECT_EQ(data0, static_cast<uint64_t>(n_pkts/2));
      EXPECT_EQ(data1, static_cast<uint64_t>(pktlen*n_pkts/2));
    } else {
      EXPECT_EQ(data0, NON);
      EXPECT_EQ(data1, NON);
    }

    // Movereg test
    // Read addr(vpn=7,index=115) - expect to be 0
    // Then movereg 8,15->7,115 and initialise 8,15
    // Should see final val of 8,15 now in 7,115
    //
    // First off configure movereg to say direct stats and deferred stats
    //tu.OutWord(&mau_reg_map.rams.match.adrdist.movereg_ad_ctl[0], (1<<10)|(1<<6));
    // Configure LT 0 for Stats movereg
    tu.OutWord(&mau_reg_map.rams.match.adrdist.movereg_ad_direct[0], (1<<0));
    // Now configure for StatsALU[1] rather than LT[0]
    tu.OutWord(&mau_reg_map.rams.match.adrdist.movereg_stats_ctl[1], (1<<5)|(1<<3));
    // And finally config StatsALU->LT xbar to be enabled for LT[0] and StatsALU[1]
    tu.OutWord(&mau_reg_map.rams.match.adrdist.movereg_ad_stats_alu_to_logical_xbar_ctl[0], 0x5);
    data0 = NON; data1 = NON;
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=7,index=115) - expect 0
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(115<<5))>>2, &data0, &data1);
    printf("3. Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n", data0, data1);
    EXPECT_EQ(data0, NON);
    EXPECT_EQ(data1, NON);

    int instr_push_table_move_reg = 0, instr_pop_table_move_reg = 0;
    uint64_t push_addr = UINT64_C(0), pop_addr = UINT64_C(0);
    // Load 7,115
    instr_push_table_move_reg = 0x8000000|((0)<<20)|(   ( ((7)<<10) | 115 ) & 0x7ffff   );
    push_addr = TestUtil::make_instruction_address(pipe, 0, 0, instr_push_table_move_reg);
    tu.IndirectWrite(push_addr, UINT64_C(0), UINT64_C(0));
    // Load 8,15
    instr_push_table_move_reg = 0x8000000|((0)<<20)|(   ( ((8)<<10) |  15 ) & 0x7ffff   );
    push_addr = TestUtil::make_instruction_address(pipe, 0, 0, instr_push_table_move_reg);
    tu.IndirectWrite(push_addr, UINT64_C(0), UINT64_C(0));
    // Pop - should see 8,15 copied to 7,115
    instr_pop_table_move_reg  = 0x1e00000|((0)<<0);
    pop_addr = TestUtil::make_instruction_address(pipe, 0, 0, instr_pop_table_move_reg);
    tu.IndirectWrite(pop_addr, UINT64_C(0), UINT64_C(0));
    // Pop again - should see 8,15 initialised
    instr_pop_table_move_reg  = 0x1e00000|((0)<<0)|(1<<5);
    pop_addr = TestUtil::make_instruction_address(pipe, 0, 0, instr_pop_table_move_reg);
    tu.IndirectWrite(pop_addr, UINT64_C(0), UINT64_C(0));
    // We expect this op to result in a call to set_meter_time - check!
    //int instr_ism_init = 0x6000000; // ism_init(1<<26) | set_meter_time(4<<23)
    //uint64_t ism_addr = TestUtil::make_instruction_address(pipe, 0, 0, instr_ism_init);
    //tu.IndirectWrite(ism_addr, UINT64_C(0), UINT64_C(0));

    // Read vaddr for stage0, STATS, logtab0, addr(vpn=7,index=115) - expect npkts/2 etc
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(115<<5))>>2, &data0, &data1);
    printf("4. Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n", data0, data1);
    if (Address::kGlobalAddrEnable || addr_enable) {
      EXPECT_EQ(data0, static_cast<uint64_t>(n_pkts/2));
      EXPECT_EQ(data1, static_cast<uint64_t>(pktlen*n_pkts/2));
      printf("Stats entry MOVED from 8,15->7,115\n");
    }
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=8,index=15) - expect ALL_ONES
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((8<<15)|(15<<5))>>2, &data0, &data1);
    printf("5. Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n", data0, data1);
    if (Address::kGlobalAddrEnable || addr_enable) {
      EXPECT_EQ(data0, static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF));
      EXPECT_EQ(data1, static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF));
      printf("Stats entry 8,15 INITIALIZED\n");
    }


    // MOVEREG also moves full-res stats entries these days - so verify that too
    //
    // Read full-res addr for stage0, STATS, logtab0, addr(vpn=7,index=115) - expect npkts/2 etc
    faddr = 3<<28 | TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats,
                                                   0, ((7<<15)|(115<<5))>>2);
    tu.IndirectRead(faddr, &data0, &data1);
    printf("6. FullRes: Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n", data0, data1);
    if (Address::kGlobalAddrEnable || addr_enable) {
      EXPECT_EQ(data0, static_cast<uint64_t>(n_pkts/2));
      EXPECT_EQ(data1, static_cast<uint64_t>(pktlen*n_pkts/2));
    }
    // Read full-res addr for stage0, STATS, logtab0, addr(vpn=8,index=15) - expect ALL_ONES
    faddr = 3<<28 | TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats,
                                                   0, ((8<<15)|(15<<5))>>2);
    tu.IndirectRead(faddr, &data0, &data1);
    printf("7. FullRes: Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n", data0, data1);
    if (Address::kGlobalAddrEnable || addr_enable) {
      EXPECT_EQ(data0, static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF));
      EXPECT_EQ(data1, static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF));
    }


    // Test word/table dump in either BubbleMode/FastMode by setting this true/false
    MauMemory::kStatsVAddrDumpWordBubbleEmulate = true;


    // Now trigger a stats dump of VPN=7,Index=15 - this will Q - need to call sweep to trigger
    // This time also lock/unlock the table before/after dump - callback should be POST UNLOCK
    stats_callback_pktcnt_width = 64;
    stats_callback_data = UINT64_C(0);
    stats_callbacks = 0;
    int dlt = 0;
    uint32_t daddr = ((7<<15)|(15<<5))>>2; // VPN=7, Index=15
    int instr_dump_stats_word = 0x1000000|((dlt & 0xF)<<19)|(daddr & 0x7FFFF);
    int instr_dump_stats_tab  = 0x1F80000|((dlt & 0xF)<<0);
    int instr_barrier_lock    = 0x0800000|((dlt & 0xF)<<19)|(4<<16)|0xabcd; // 4=>StatsLock
    int instr_barrier_unlock  = 0x0800000|((dlt & 0xF)<<19)|(5<<16)|0xabcd; // 5=>StatsUnlock
    uint64_t dswaddr = TestUtil::make_instruction_address(0,0,0,instr_dump_stats_word);
    uint64_t dstaddr = TestUtil::make_instruction_address(0,0,0,instr_dump_stats_tab);
    uint64_t lockaddr   = TestUtil::make_instruction_address(0,0,0,instr_barrier_lock);
    uint64_t unlockaddr = TestUtil::make_instruction_address(0,0,0,instr_barrier_unlock);

    int before_n_callbacks = stats_callbacks;
    // Lock table 0
    tu.IndirectWrite(lockaddr, UINT64_C(0), UINT64_C(0));
    // Request stats dump word
    tu.IndirectWrite(dswaddr, UINT64_C(0), UINT64_C(0));
    // Sweep a couple of times - NOTHING should happen
    sweeper->sweep_increment(UINT64_C(0));
    sweeper->sweep_increment(UINT64_C(0));
    int after_n_callbacks =  stats_callbacks;
    EXPECT_EQ(before_n_callbacks,after_n_callbacks);
    // Now unlock table 0 - should now get callbacks
    tu.IndirectWrite(unlockaddr, UINT64_C(0), UINT64_C(0));
    // Sweep a couple of times
    // Should get 2x dump word callbacks at some point as using 64b+64b format
    // After this pkt_cnt will be in stats_callback_prev_msg1
    //       and byte_cnt will be in stats_callback_msg1
    stats_callback_prev_addr = stats_callback_addr = 0u;
    stats_callback_prev_msg1 = stats_callback_msg1 = UINT64_C(0);
    stats_callback_data = UINT64_C(0);
    stats_callbacks = 0;
    sweeper->sweep_increment(UINT64_C(0));
    sweeper->sweep_increment(UINT64_C(0));
    after_n_callbacks =  stats_callbacks;
    EXPECT_EQ(before_n_callbacks+2,after_n_callbacks);
    EXPECT_EQ(stats_callback_prev_msg1, static_cast<uint64_t>(n_pkts/2));
    EXPECT_EQ(stats_callback_msg1, static_cast<uint64_t>(pktlen*n_pkts/2));
    EXPECT_EQ(stats_callback_addr, 4+stats_callback_prev_addr);


    // Now loop doing stats dump of VPN=8,Index=15. Because this is ALL_ONES initially
    // we should see a callback with value ALL_ONES. But dumping ALL_ONES in 64b mode
    // causes the value to be reset to 0 so subsequent dump words should get back 0
    uint32_t daddr2 =  ((8<<15)|(15<<5))>>2; // VPN=8, Index=15
    int instr_dump_stats_word2 = 0x1000000|((dlt & 0xF)<<19)|(daddr2 & 0x7FFFF);
    uint64_t dswaddr2 = TestUtil::make_instruction_address(0,0,0,instr_dump_stats_word2);
    uint64_t expected = UINT64_C(0xFFFFFFFFFFFFFFFF);
    for (int i = 0; i < 100; i++) {
      stats_callbacks = 0;
      stats_callback_data = UINT64_C(0);
      stats_callback_vpn = -1;
      stats_callback_index = -1;
      // Request stats dump word and sweep to trigger
      tu.IndirectWrite(dswaddr2, UINT64_C(0), UINT64_C(0));
      sweeper->sweep_increment(UINT64_C(0));
      sweeper->sweep_increment(UINT64_C(0));
      EXPECT_EQ(stats_callbacks, 2);
      EXPECT_EQ(stats_callback_data, expected);
      //printf("stats_dump(%d) data=%016" PRIx64 "\n", i, stats_callback_data);
      EXPECT_EQ(stats_callback_vpn, 8);
      EXPECT_EQ(stats_callback_index, 15);
      expected = UINT64_C(0);
    }


    // Table dump generates a ton of output so quieten down beforehand
    tu.quieten_log_flags();
    // Reset callback info
    stats_callbacks = 0;
    stats_callback_data = UINT64_C(0);
    stats_callback_vpn = -1;
    stats_callback_index = -1;
    // Now trigger a stats TABLE dump - this will Q - need to call sweep to trigger
    tu.IndirectWrite(dstaddr, UINT64_C(0), UINT64_C(0));
    // Sweep a couple of times - should get dump callback for any non-zero
    // index - only expect to see one (VPN=7,Index=115)
    sweeper->sweep_increment(UINT64_C(0));
    sweeper->sweep_increment(UINT64_C(0));
    // Check callbacks etc
    EXPECT_EQ(stats_callbacks, 2);
    EXPECT_EQ(stats_callback_prev_msg1, static_cast<uint64_t>(n_pkts/2));
    EXPECT_EQ(stats_callback_msg1, static_cast<uint64_t>(pktlen*n_pkts/2));
    EXPECT_EQ(stats_callback_addr, 4+stats_callback_prev_addr);
    EXPECT_EQ(stats_callback_vpn, 7);
    EXPECT_EQ(stats_callback_index, 115);
    // Reset callback info and dump again - all previously dumped entries
    // should be zero so we should see no further callbacks
    stats_callbacks = 0;
    // Trigger final stats table dump
    tu.IndirectWrite(dstaddr, UINT64_C(0), UINT64_C(0));
    sweeper->sweep_increment(UINT64_C(0));
    sweeper->sweep_increment(UINT64_C(0));
    // Check no callbacks
    EXPECT_EQ(stats_callbacks, 0);


    stats_callback_pktcnt_width = 0;
    // Free
    tu.phv_free(phv);
    // Wrap up
    tu.finish_test();
    // Schtum
    tu.quieten_log_flags();
  }


  TEST(BFN_TEST_NAME(StatsTest),Lrt) {
    if (stats_print) RMT_UT_LOG_INFO("test_stats_lrt()\n");
    int chip = 202;
    int pipe = 0;
    int stage = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    //auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);

     // Register Stats Dump callback func with Model
     GLOBAL_MODEL.get()->
         register_callback_dru_lrt_update(stats_callback_print);


    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Switch on fatal/error/warn messages - happens by default these days
    //tu.update_log_flags(0, 0, 0, 0, 0, UINT64_C(0x7), ALL);

    // Switch *ON* specific outputs
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeParser;
    //flags = RmtDebug::kRmtDebugParserParseLoop;
    //tu.update_log_flags(pipes, 0, types, 0, 0, flags, ALL);

    // Switch *OFF* certain output
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeMauTcam;
    //flags = RmtDebug::kRmtDebugMauTcam_Tcam3LookupDetail|RmtDebug::kRmtDebugMauTcam_Tcam3DebugMiss;
    //tu.update_log_flags(pipes, 0, types, 0, 0, 0, ~flags);

    // DEBUG setup done!!!!!!!!!!!!!!!!!!!!!!


    // Instantiate whole chip and fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    // Lookup this Pipe/Stage MAU and MAU_ADDR_DIST obj
    Mau *mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    MauAddrDist *adist = mau->mau_addr_dist();
    ASSERT_TRUE(adist != NULL);
    RmtSweeper *sweeper = om->sweeper_get();
    ASSERT_TRUE(sweeper != NULL);



    // Set all PHV ingress/egress threads for our 2 stages (chip9!)
    for (stage = 0; stage < 2; stage++) {
      tu.set_phv_range_all(stage, false);
      // Then set 1st 32 of the 64x32 64x8 to be ingress
      // and the 1st 48 of the 96x16 to be ingress
      tu.set_phv_ranges(stage, 32,0, 32,0, 32,16,0, true);
    }

    // Setup ingress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepAction, true);
    tu.set_dependency(1, TestUtil::kDepAction, true);
    // Setup egress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepConcurrent, false);
    tu.set_dependency(1, TestUtil::kDepConcurrent, false);

    // Setup single logical table for ingress in stage0
    tu.table_config(0, 0, true);    // stage0  table0  ingress

    // Then setup some STATS SRAMs
    // NB format 25 is Packet+Byte[2 entries,28b PKT,26b BYTE]
    // NB NO MAPRAMS IN cols 0-5 ANY MORE
    // NB StatsALUs only on logrows 13,9,5,1 (RHS of physrows 0,2,4,6)
    int lrt_thresh = 0xA00; // 10*256 = 2560 bytes
    tu.rwram_config(0, 2, 6,                           // stage0 row2 col6
                    TestUtil::kUnitramTypeStats, 0, 0, // type vpn0=0 vpn1=0
                    lrt_thresh|26, 0, false, true);    // s_format=26 log_table=0 egress=F use_deferred=true
    tu.rwram_config(0, 2, 7,                           // stage0 row2 col7
                    TestUtil::kUnitramTypeStats, 0, 0, // type vpn0=0 vpn1=0
                    lrt_thresh|26, 0, false, true);    // s_format=26 log_table=0 egress=F use_deferred=true

    // Flush any OPs
    sweeper->sweep_increment(UINT64_C(0)); // Dequeues OPs regardless of T passed


    // UP debug now
    flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


    // Get ready for LRT callbacks
    stats_callback_bytes = UINT64_C(0);
    stats_callback_pkts  = UINT64_C(0);
    stats_callback_data  = UINT64_C(0);
    stats_callbacks = 0;


    // Create a simple PHV (so we can set a
    // pkt length increment for byte counters)
    Eop eop;
    Phv *phv = tu.phv_alloc();
    int stats_vpn;
    int stats_index;
    uint32_t stats_addr;

    int pktlen = 500;
    int n_pkts = 40;

    // Now loop pushing packets in.....
    // Expect to see LRT callback after around 26 packets
    for (int i = 0; i < n_pkts; i++) {
      eop.set_ingress_eopinfo(pktlen, 0);

      stats_vpn = 7;
      stats_index = 15;
      stats_addr = (stats_vpn << 15) | (stats_index << 5);

      // Now loop calling:
      //  MAU->reset_resources()
      //  MAU-ADDR_DIST->distrib_stats_addresses(), MAU->defer_stats_addresses()
      //  to distribute/defer stats addresses
      //  MAU->process_for_tests to get StatsALU to do its work
      //
      // NB actual stats address driven should NOT include bottom 2 LSBs as
      // these should always be zero and will be recreated at SRAM (hence >>2
      // in call the addresses below)
      // Also we switch on MSB (bit19) to indicate OP=stats
      uint32_t stats_dist_addr = (stats_addr>>2);
      stats_dist_addr |= (1u<<19);
      mau->reset_resources();

      // this is what update_addresses used to do
      // Use eopnum=0
      Teop *null_teop = NULL;
      adist->distrib_stats_addresses(0, true, stats_dist_addr, null_teop);
      adist->defer_stats_addresses(0, true, stats_dist_addr, null_teop, 0);

      mau->process_for_tests(phv, eop);
    }

    uint64_t data0, data1;
    uint64_t addr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats,
                                                   0, ((7<<15)|(15<<5))>>2);
    int type = (addr >> 30) & 0x3;

    // Just VPN7
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=7,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2, &data0, &data1);
    printf("1. Addr=%016" PRIx64 " Type=%d Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n",
           addr, type, data0, data1);

    uint64_t bytes = (data0 >> 28);
    uint64_t pkts  = (data0 & 0xFFFFFFF);
    EXPECT_EQ(stats_callback_pkts  + pkts,  static_cast<uint64_t>(n_pkts));
    EXPECT_EQ(stats_callback_bytes + bytes, static_cast<uint64_t>(pktlen*n_pkts));


    // Free
    tu.phv_free(phv);
    // Schtum
    tu.finish_test();
    tu.quieten_log_flags();
  }


  TEST(BFN_TEST_NAME(StatsTest),LrtWithLockedTable) {
    if (stats_print) RMT_UT_LOG_INFO("test_stats_lrt_with_locked_table()\n");
    int chip = 202;
    int pipe = 0;
    int stage = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    //auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);

     // Register Stats Dump callback func with Model
     GLOBAL_MODEL.get()->
         register_callback_dru_lrt_update(stats_callback_print);


    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Switch off synchronous stats ops else dump post lock will just block
    TableInfo::kSynchronousStatsOps = false;

    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Switch on fatal/error/warn messages - happens by default these days
    //tu.update_log_flags(0, 0, 0, 0, 0, UINT64_C(0x7), ALL);

    // Switch *ON* specific outputs
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeParser;
    //flags = RmtDebug::kRmtDebugParserParseLoop;
    //tu.update_log_flags(pipes, 0, types, 0, 0, flags, ALL);

    // Switch *OFF* certain output
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeMauTcam;
    //flags = RmtDebug::kRmtDebugMauTcam_Tcam3LookupDetail|RmtDebug::kRmtDebugMauTcam_Tcam3DebugMiss;
    //tu.update_log_flags(pipes, 0, types, 0, 0, 0, ~flags);

    // DEBUG setup done!!!!!!!!!!!!!!!!!!!!!!


    // Instantiate whole chip and fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    // Lookup this Pipe/Stage MAU and MAU_ADDR_DIST obj
    Mau *mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    MauAddrDist *adist = mau->mau_addr_dist();
    ASSERT_TRUE(adist != NULL);
    RmtSweeper *sweeper = om->sweeper_get();
    ASSERT_TRUE(sweeper != NULL);



    // Set all PHV ingress/egress threads for our 2 stages (chip9!)
    for (stage = 0; stage < 2; stage++) {
      tu.set_phv_range_all(stage, false);
      // Then set 1st 32 of the 64x32 64x8 to be ingress
      // and the 1st 48 of the 96x16 to be ingress
      tu.set_phv_ranges(stage, 32,0, 32,0, 32,16,0, true);
    }

    // Setup ingress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepAction, true);
    tu.set_dependency(1, TestUtil::kDepAction, true);
    // Setup egress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepConcurrent, false);
    tu.set_dependency(1, TestUtil::kDepConcurrent, false);

    // Setup single logical table for ingress in stage0
    tu.table_config(0, 0, true);    // stage0  table0  ingress

    // Then setup some STATS SRAMs
    // NB format 25 is Packet+Byte[2 entries,28b PKT,26b BYTE]
    // NB NO MAPRAMS IN cols 0-5 ANY MORE
    // NB StatsALUs only on logrows 13,9,5,1 (RHS of physrows 0,2,4,6)
    int lrt_thresh = 0xA00; // 10*256 = 2560 bytes
    tu.rwram_config(0, 2, 6,                           // stage0 row2 col6
                    TestUtil::kUnitramTypeStats, 0, 0, // type vpn0=0 vpn1=0
                    lrt_thresh|26, 0, false, true);    // s_format=26 log_table=0 egress=F use_deferred=true
    tu.rwram_config(0, 2, 7,                           // stage0 row2 col7
                    TestUtil::kUnitramTypeStats, 0, 0, // type vpn0=0 vpn1=0
                    lrt_thresh|26, 0, false, true);    // s_format=26 log_table=0 egress=F use_deferred=true

    // Flush any OPs
    sweeper->sweep_increment(UINT64_C(0)); // Dequeues OPs regardless of T passed


    // UP debug now
    flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Get ready for LRT callbacks
    stats_callback_bytes = UINT64_C(0);
    stats_callback_pkts  = UINT64_C(0);
    stats_callback_data  = UINT64_C(0);
    stats_callbacks = 0;

    // LOCK table this time
    int dlt = 0;
    int instr_barrier_lock    = 0x0800000|((dlt & 0xF)<<19)|(4<<16)|0xabcd; // 4=>StatsLock
    int instr_barrier_unlock  = 0x0800000|((dlt & 0xF)<<19)|(5<<16)|0xabcd; // 5=>StatsUnlock
    uint64_t lockaddr   = TestUtil::make_instruction_address(0,0,0,instr_barrier_lock);
    uint64_t unlockaddr = TestUtil::make_instruction_address(0,0,0,instr_barrier_unlock);
    // Lock table 0 - will see ACK - ignore
    tu.IndirectWrite(lockaddr, UINT64_C(0), UINT64_C(0));
    sweeper->sweep_increment(UINT64_C(0));
    sweeper->sweep_increment(UINT64_C(0));
    int before_n_callbacks = stats_callbacks;

    // Create a simple PHV (so we can set a
    // pkt length increment for byte counters)
    Eop eop;
    Phv *phv = tu.phv_alloc();
    int stats_vpn;
    int stats_index;
    uint32_t stats_addr;

    int pktlen = 500;
    int n_pkts = 40;

    // Now loop pushing packets in.....
    // Expect to see LRT callback after around 26 packets
    for (int i = 0; i < n_pkts; i++) {
      eop.set_ingress_eopinfo(pktlen, 0);

      stats_vpn = 7;
      stats_index = 15;
      stats_addr = (stats_vpn << 15) | (stats_index << 5);

      // Now loop calling:
      //  MAU->reset_resources()
      //  MAU-ADDR_DIST->distrib_stats_addresses(), MAU->defer_stats_addresses()
      //  to distribute/defer stats addresses
      //  MAU->process_for_tests to get StatsALU to do its work
      //
      // NB actual stats address driven should NOT include bottom 2 LSBs as
      // these should always be zero and will be recreated at SRAM (hence >>2
      // in call the addresses below)
      // Also we switch on MSB (bit19) to indicate OP=stats
      uint32_t stats_dist_addr = (stats_addr>>2);
      stats_dist_addr |= (1u<<19);
      mau->reset_resources();

      // this is what update_addresses used to do
      // Use eopnum=0
      Teop *null_teop = NULL;
      adist->distrib_stats_addresses(0, true, stats_dist_addr, null_teop);
      adist->defer_stats_addresses(0, true, stats_dist_addr, null_teop, 0);

      mau->process_for_tests(phv, eop);
    }

    // NO callbacks should have occurred during loop above since table was locked
    int after_n_callbacks =  stats_callbacks;
    EXPECT_EQ(before_n_callbacks,after_n_callbacks);

    // Now unlock table 0 and sweep - should get ALL LRT callbacks
    tu.IndirectWrite(unlockaddr, UINT64_C(0), UINT64_C(0));
    sweeper->sweep_increment(UINT64_C(0));
    sweeper->sweep_increment(UINT64_C(0));
    after_n_callbacks =  stats_callbacks;
    EXPECT_LT(before_n_callbacks,after_n_callbacks);


    // And data should match up like before
    uint64_t data0, data1;
    uint64_t addr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats,
                                                   0, ((7<<15)|(15<<5))>>2);
    int type = (addr >> 30) & 0x3;

    // Just VPN7
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=7,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2, &data0, &data1);
    printf("1. Addr=%016" PRIx64 " Type=%d Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n",
           addr, type, data0, data1);

    uint64_t bytes = (data0 >> 28);
    uint64_t pkts  = (data0 & 0xFFFFFFF);
    EXPECT_EQ(stats_callback_pkts  + pkts,  static_cast<uint64_t>(n_pkts));
    EXPECT_EQ(stats_callback_bytes + bytes, static_cast<uint64_t>(pktlen*n_pkts));


    // Free
    tu.phv_free(phv);
    // Schtum
    tu.finish_test();
    tu.quieten_log_flags();
  }


  TEST(BFN_TEST_NAME(StatsTest),LrtWithLockedTableAndMovereg) {
    if (stats_print) RMT_UT_LOG_INFO("test_stats_lrt_with_locked_table_and_movereg()\n");
    int chip = 202;
    int pipe = 0;
    int stage = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    //auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);

     // Register Stats Dump callback func with Model
     GLOBAL_MODEL.get()->
         register_callback_dru_lrt_update(stats_callback_print);


    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Switch off synchronous stats ops else dump post lock will just block
    TableInfo::kSynchronousStatsOps = false;

    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Switch on fatal/error/warn messages - happens by default these days
    //tu.update_log_flags(0, 0, 0, 0, 0, UINT64_C(0x7), ALL);

    // Switch *ON* specific outputs
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeParser;
    //flags = RmtDebug::kRmtDebugParserParseLoop;
    //tu.update_log_flags(pipes, 0, types, 0, 0, flags, ALL);

    // Switch *OFF* certain output
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeMauTcam;
    //flags = RmtDebug::kRmtDebugMauTcam_Tcam3LookupDetail|RmtDebug::kRmtDebugMauTcam_Tcam3DebugMiss;
    //tu.update_log_flags(pipes, 0, types, 0, 0, 0, ~flags);

    // DEBUG setup done!!!!!!!!!!!!!!!!!!!!!!


    // Instantiate whole chip and fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    // Lookup this Pipe/Stage MAU and MAU_ADDR_DIST obj
    Mau *mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    MauAddrDist *adist = mau->mau_addr_dist();
    ASSERT_TRUE(adist != NULL);
    RmtSweeper *sweeper = om->sweeper_get();
    ASSERT_TRUE(sweeper != NULL);



    // Set all PHV ingress/egress threads for our 2 stages (chip9!)
    for (stage = 0; stage < 2; stage++) {
      tu.set_phv_range_all(stage, false);
      // Then set 1st 32 of the 64x32 64x8 to be ingress
      // and the 1st 48 of the 96x16 to be ingress
      tu.set_phv_ranges(stage, 32,0, 32,0, 32,16,0, true);
    }

    // Setup ingress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepAction, true);
    tu.set_dependency(1, TestUtil::kDepAction, true);
    // Setup egress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepConcurrent, false);
    tu.set_dependency(1, TestUtil::kDepConcurrent, false);

    // Setup single logical table for ingress in stage0
    tu.table_config(0, 0, true);    // stage0  table0  ingress

    // Then setup some STATS SRAMs
    // NB format 25 is Packet+Byte[2 entries,28b PKT,26b BYTE]
    // NB NO MAPRAMS IN cols 0-5 ANY MORE
    // NB StatsALUs only on logrows 13,9,5,1 (RHS of physrows 0,2,4,6)
    int lrt_thresh = 0xA00; // 10*256 = 2560 bytes
    tu.rwram_config(0, 2, 6,                           // stage0 row2 col6
                    TestUtil::kUnitramTypeStats, 0, 0, // type vpn0=0 vpn1=0
                    lrt_thresh|26, 0, false, true);    // s_format=26 log_table=0 egress=F use_deferred=true
    tu.rwram_config(0, 2, 7,                           // stage0 row2 col7
                    TestUtil::kUnitramTypeStats, 0, 0, // type vpn0=0 vpn1=0
                    lrt_thresh|26, 0, false, true);    // s_format=26 log_table=0 egress=F use_deferred=true

    // Flush any OPs
    sweeper->sweep_increment(UINT64_C(0)); // Dequeues OPs regardless of T passed


    // UP debug now
    flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Get ready for LRT callbacks
    stats_callback_bytes = UINT64_C(0);
    stats_callback_pkts  = UINT64_C(0);
    stats_callback_data  = UINT64_C(0);
    stats_callbacks = 0;

    // LOCK table this time
    int dlt = 0;
    int instr_barrier_lock    = 0x0800000|((dlt & 0xF)<<19)|(4<<16)|0xabcd; // 4=>StatsLock
    int instr_barrier_unlock  = 0x0800000|((dlt & 0xF)<<19)|(5<<16)|0xabcd; // 5=>StatsUnlock
    uint64_t lockaddr   = TestUtil::make_instruction_address(0,0,0,instr_barrier_lock);
    uint64_t unlockaddr = TestUtil::make_instruction_address(0,0,0,instr_barrier_unlock);
    // Lock table 0 - will see ACK - ignore
    tu.IndirectWrite(lockaddr, UINT64_C(0), UINT64_C(0));
    sweeper->sweep_increment(UINT64_C(0));
    sweeper->sweep_increment(UINT64_C(0));
    int before_n_callbacks = stats_callbacks;


    // Create a simple PHV (so we can set a
    // pkt length increment for byte counters)
    Eop eop;
    Phv *phv = tu.phv_alloc();
    int stats_index = 15;
    uint32_t stats_addr = (7 << 15) | (stats_index << 5);
    uint32_t stats_addr_new = (8 << 15) | (stats_index << 5);
    int pktlen = 500;
    int n_pkts = 40;

    // Now generate a movereg - just directly call MAD
    // This tells RmtSweeper to remap any evict addresses (vpn7->vpn8)
    adist->update_queued_addr(2, 0, stats_addr>>2, stats_addr_new>>2);

    // Now loop pushing packets in.....
    // We use OLD addresses, but will expect all callbacks to be with NEW address
    // Expect to see LRT callback after around 26 packets
    for (int i = 0; i < n_pkts; i++) {
      eop.set_ingress_eopinfo(pktlen, 0);

      // Now loop calling:
      //  MAU->reset_resources()
      //  MAU-ADDR_DIST->distrib_stats_addresses(), MAU->defer_stats_addresses()
      //  to distribute/defer stats addresses
      //  MAU->process_for_tests to get StatsALU to do its work
      //
      // NB actual stats address driven should NOT include bottom 2 LSBs as
      // these should always be zero and will be recreated at SRAM (hence >>2
      // in call the addresses below)
      // Also we switch on MSB (bit19) to indicate OP=stats
      uint32_t stats_dist_addr = (stats_addr>>2);
      stats_dist_addr |= (1u<<19);
      mau->reset_resources();

      // this is what update_addresses used to do
      // Use eopnum=0
      Teop *null_teop = NULL;
      adist->distrib_stats_addresses(0, true, stats_dist_addr, null_teop);
      adist->defer_stats_addresses(0, true, stats_dist_addr, null_teop, 0);

      mau->process_for_tests(phv, eop);
    }

    // NO callbacks should have occurred during loop above since table was locked
    int after_n_callbacks =  stats_callbacks;
    EXPECT_EQ(before_n_callbacks,after_n_callbacks);

    // Now unlock table 0 and sweep - should get ALL LRT callbacks
    tu.IndirectWrite(unlockaddr, UINT64_C(0), UINT64_C(0));
    sweeper->sweep_increment(UINT64_C(0));
    sweeper->sweep_increment(UINT64_C(0));
    after_n_callbacks =  stats_callbacks;
    EXPECT_LT(before_n_callbacks,after_n_callbacks);


    // And data should match up like before
    uint64_t data0, data1;
    uint64_t addr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats,
                                                   0, ((7<<15)|(15<<5))>>2);
    int type = (addr >> 30) & 0x3;

    // Just VPN7
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=7,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2, &data0, &data1);
    printf("1. Addr=%016" PRIx64 " Type=%d Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n",
           addr, type, data0, data1);

    uint64_t bytes = (data0 >> 28);
    uint64_t pkts  = (data0 & 0xFFFFFFF);
    EXPECT_EQ(stats_callback_pkts  + pkts,  static_cast<uint64_t>(n_pkts));
    EXPECT_EQ(stats_callback_bytes + bytes, static_cast<uint64_t>(pktlen*n_pkts));
    EXPECT_EQ(stats_callback_index, 15);
    EXPECT_EQ(stats_callback_vpn, 8);

    // Free
    tu.phv_free(phv);
    // Schtum
    tu.finish_test();
    tu.quieten_log_flags();
  }


  TEST(BFN_TEST_NAME(StatsTest),ReadWriteAll) {
    if (stats_print) RMT_UT_LOG_INFO("test_stats_read_write_all()\n");
    int chip = 202;
    int pipe = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, 0);

    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = ONE; flags = HI;
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
    Mau *mau = om->mau_lookup(pipe, 0);
    ASSERT_TRUE(mau != NULL);

    // Decide which SRAM type to use for read/write testing
    // and configure row offsets, address shifts appropriately
    // This allows us to test Meter/Selector read writes as well as Stats
    int ramtype = TestUtil::kUnitramTypeStats;

    int memtype = -1; // Virt address memtype corresponding to ramtype
    int shift   = -1; // Virt address shift
    int roff    = -1; // Row offset to take account of ALU placement
    int subword =  0;
    int format  =  0;
    switch (ramtype) {
      case TestUtil::kUnitramTypeSelector: case TestUtil::kUnitramTypeStateful:
        memtype = TestUtil::kVirtMemTypeSelectorStateful; shift = 5; roff = 1; subword = 0xF; break;
      case TestUtil::kUnitramTypeMeter:
        memtype = TestUtil::kVirtMemTypeMeter; shift = 0; roff = 1; break;
      case TestUtil::kUnitramTypeStats:
        memtype = TestUtil::kVirtMemTypeStats; shift = 3; roff = 0; format = 25; break;
      default: assert(0); break;
    }

    // Setup a simple config
    for (int stage = 0; stage < 2; stage++) {
      // Set all PHV ingress/egress threads for our 2 stages (chip9!)
      tu.set_phv_range_all(stage, false);
      // Then set 1st 32 of the 64x32 64x8 to be ingress
      // and the 1st 48 of the 96x16 to be ingress
      tu.set_phv_ranges(stage, 32,0, 32,0, 32,16,0, true);

      // Setup ingress dependencies for stages
      tu.set_dependency(stage, TestUtil::kDepAction, true);
      // Setup egress dependencies for stages
      tu.set_dependency(stage, TestUtil::kDepConcurrent, false);

      // Setup 4 ingress logical tables per stage (tables 0,2,4,6 or 1,3,5,7)
      for (int table = 0; table < 8; table+=2) {
        tu.table_config(stage, table+roff, true);
      }

      // Then setup some SRAMs on rows 0,2,4,6 (or 1,3,5,7)
      // such that all the SRAMs on row X are associated with table X.
      // NB1 Only use RHS cols as this is a Virt RW test so we need MapRAMs
      // NB2 format 25 is STATS format (Packet+Byte[1 entry,64b PKT,64b BYTE])
      for (int row = 0; row < 8; row+=2) {
        for (int col = 6; col < 12; col++) {
          tu.rwram_config(stage, row+roff, col, ramtype, 0, 0, // VPNs
                          format, row+roff, false, true);  // Logtab# == Row#
        }
      }
    }

    // Now we can do some Virtual Reads/Writes
    // Randomly use bubble-mode read or write
    std::default_random_engine generator;
    std::uniform_int_distribution<uint64_t> data_distribution;
    std::uniform_int_distribution<uint64_t> rdbubble_distribution(0,1);
    std::uniform_int_distribution<uint64_t> wrbubble_distribution(0,1);
    std::uniform_int_distribution<uint64_t> stage_distribution(0,1);
    std::uniform_int_distribution<uint64_t> logtab_distribution(0,3);
    std::uniform_int_distribution<uint64_t> vpn_distribution(6,11);
    std::uniform_int_distribution<uint64_t> index_distribution(0,1023);

    // Stash how we currently read/write so we can restore later
    bool prevBubbleReadStats = MauMemory::kStatsVAddrPbusReadBubbleEmulate;
    bool prevBubbleWriteStats = MauMemory::kStatsVAddrPbusWriteBubbleEmulate;
    bool prevBubbleReadMeter = MauMemory::kMeterVAddrPbusReadBubbleEmulate;
    bool prevBubbleWriteMeter = MauMemory::kMeterVAddrPbusWriteBubbleEmulate;
    bool prevBubbleReadSelStat = MauMemory::kSelectorStatefulVAddrPbusReadBubbleEmulate;
    bool prevBubbleWriteSelStat = MauMemory::kSelectorStatefulVAddrPbusWriteBubbleEmulate;
    // Do we randomly chop and change access mechanism for each access?
    bool randomise_wr_access_mech = true;
    bool randomise_rd_access_mech = true;
    // Configure default access mechanism
    MauMemory::kStatsVAddrPbusWriteBubbleEmulate = true;
    MauMemory::kMeterVAddrPbusWriteBubbleEmulate = true;
    MauMemory::kSelectorStatefulVAddrPbusWriteBubbleEmulate = true;
    MauMemory::kStatsVAddrPbusReadBubbleEmulate = true;
    MauMemory::kMeterVAddrPbusReadBubbleEmulate = true;
    MauMemory::kSelectorStatefulVAddrPbusReadBubbleEmulate = true;

    //flags = ALL; // Up logging
    //tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    for (int t = 0; t < 2; ++t) { // first time write, second time read back
      generator.seed( unsigned(0xDAB0D1B0D0B0DEB0) );
      std::map<uint64_t,bool> written;
      for (int i = 0; i < 10000; ++i) {
        bool rdbubble = (rdbubble_distribution(generator) == 1);
        bool wrbubble = (wrbubble_distribution(generator) == 1);
        uint64_t wdata0 = data_distribution(generator);
        uint64_t wdata1 = data_distribution(generator);
        int stage = stage_distribution(generator);
        // Logtab will be 0,2,4,6 or 1,3,5,7
        int logtab = roff + (2*logtab_distribution(generator));
        int vpn = vpn_distribution(generator);
        int index = index_distribution(generator);
        int addr = (vpn << (10+shift)) | (index << shift) | subword;

        // work out address and avoid writing to the same one twice
        uint64_t a = tu.make_virt_addr(pipe,stage,memtype,logtab,addr);
        if ( written.count(a) ) continue;
        written[a] = true;

        if (t==0) {
          if (stats_print)
            printf("  Write %d,%d,%d,LT=%d,VPN=%d,Index=%d,Sub=%d %" PRIx64 " %" PRIx64 "\n",
                   pipe,stage,memtype,logtab,vpn,index,subword, wdata1,wdata0);
          if (randomise_wr_access_mech) {
            MauMemory::kStatsVAddrPbusWriteBubbleEmulate = wrbubble;
            MauMemory::kMeterVAddrPbusWriteBubbleEmulate = wrbubble;
            MauMemory::kSelectorStatefulVAddrPbusWriteBubbleEmulate = wrbubble;
          }
          tu.rwram_write(pipe,stage,memtype,logtab,addr, wdata0, wdata1);
        }
        else {
          uint64_t rdata0,rdata1;
          if (randomise_rd_access_mech) {
            MauMemory::kStatsVAddrPbusReadBubbleEmulate = rdbubble;
            MauMemory::kMeterVAddrPbusReadBubbleEmulate = rdbubble;
            MauMemory::kSelectorStatefulVAddrPbusReadBubbleEmulate = rdbubble;
          }
          tu.rwram_read(pipe,stage,memtype,logtab,addr, &rdata0, &rdata1);
          if (stats_print)
            printf("  Read %d,%d,%d,LT=%d,VPN=%d,Index=%d,Sub=%d %" PRIx64 " %" PRIx64 "\n",
                   pipe,stage,memtype,logtab,vpn,index,subword, rdata1,rdata0);
          EXPECT_EQ( wdata0, rdata0);
          EXPECT_EQ( wdata1, rdata1);
        }
      }
    }
    // Restore original config
    MauMemory::kStatsVAddrPbusReadBubbleEmulate = prevBubbleReadStats;
    MauMemory::kStatsVAddrPbusWriteBubbleEmulate = prevBubbleWriteStats;
    MauMemory::kMeterVAddrPbusReadBubbleEmulate = prevBubbleReadMeter;
    MauMemory::kMeterVAddrPbusWriteBubbleEmulate = prevBubbleWriteMeter;
    MauMemory::kSelectorStatefulVAddrPbusReadBubbleEmulate = prevBubbleReadSelStat;
    MauMemory::kSelectorStatefulVAddrPbusWriteBubbleEmulate = prevBubbleWriteSelStat;
  }


  TEST(BFN_TEST_NAME(StatsTest),Eop) {
    if (stats_print) RMT_UT_LOG_INFO("test_stats_eop()\n");
    int chip = 202;
    int pipe = 0;
    int stage = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    //auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);


    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = ONE; flags = HI;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Switch on retention of/access to FullResStats
    MauStatsAlu::kKeepFullResStats = true;
    MauMemory::kKeepFullResStats = true;
    MauMemory::kAccessFullResStats = true;

    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Switch on fatal/error/warn messages - happens by default these days
    //tu.update_log_flags(0, 0, 0, 0, 0, UINT64_C(0x7), ALL);

    // Switch *ON* specific outputs
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeParser;
    //flags = RmtDebug::kRmtDebugParserParseLoop;
    //tu.update_log_flags(pipes, 0, types, 0, 0, flags, ALL);

    // Switch *OFF* certain output
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeMauTcam;
    //flags = RmtDebug::kRmtDebugMauTcam_Tcam3LookupDetail|RmtDebug::kRmtDebugMauTcam_Tcam3DebugMiss;
    //tu.update_log_flags(pipes, 0, types, 0, 0, 0, ~flags);

    // DEBUG setup done!!!!!!!!!!!!!!!!!!!!!!


    // Instantiate whole chip and fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    // Lookup this Pipe/Stage MAU and MAU_ADDR_DIST obj
    Mau *mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    MauAddrDist *adist = mau->mau_addr_dist();
    ASSERT_TRUE(adist != NULL);



    // Set all PHV ingress/egress threads for our 2 stages (chip9!)
    for (stage = 0; stage < 2; stage++) {
      tu.set_phv_range_all(stage, false);
      // Then set 1st 32 of the 64x32 64x8 to be ingress
      // and the 1st 48 of the 96x16 to be ingress
      tu.set_phv_ranges(stage, 32,0, 32,0, 32,16,0, true);
    }

    // Setup ingress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepAction, true);
    tu.set_dependency(1, TestUtil::kDepAction, true);
    // Setup egress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepConcurrent, false);
    tu.set_dependency(1, TestUtil::kDepConcurrent, false);

    // Setup single logical table for ingress in stage0
    tu.table_config(0, 0, true);    // stage0  table0  ingress

    // Then setup some STATS SRAMs
    // NB format 25 is Packet+Byte[1 entry,64b PKT,64b BYTE]
    // Want to test eop/deferred rams here so MUST USE ODD LOGICAL ROW (ie col>=6)
    // And must be an odd logical row on which we have a StatsALU (phys=2,log=5)
    tu.rwram_config(0, 2, 6,                           // stage0 row2 col6
                    TestUtil::kUnitramTypeStats, 0, 0, // type vpn0=0 vpn1=0
                    25, 0, false, true);     // s_format=25 log_table=0 egress=F  use_deferred=true
    tu.rwram_config(0, 2, 7,                           // stage0 row2 col7
                    TestUtil::kUnitramTypeStats, 0, 0, // type vpn0=0 vpn1=0
                    25, 0, false, true);     // s_format=25 log_table=0 egress=F  use_deferred=true


    // UP debug now
    flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


    // Create a simple PHV (so we can set a
    // pkt length increment for byte counters)
    Phv *phv = tu.phv_alloc();
    int stats_vpn;
    int stats_index;
    uint32_t stats_addr;

    //Address::kGlobalAddrEnable = false;
    bool addr_enable = true;
    int pktlen = 100;
    int n_pkts = 10; // KEEP EVEN!!!!
    uint8_t eop_num_bad = 22<<1 | 0; // Port 22, no resubmit
    uint8_t eop_num_ok = 1<<1 | 0;   // Port 1, no resubmit
    Eop eop;

    // Now loop pushing packets in.....
    for (int i = 0; i < n_pkts; i++) {
      if ((i % 2) == 0) stats_vpn = 7; else stats_vpn = 8;
      stats_index = 15;
      stats_addr = (stats_vpn << 15) | (stats_index << 5);

      // Now loop calling:
      //  MAU->reset_resources()
      //  MAU-ADDR_DIST->update_addresses to defer a fake STATS address
      //  MAU->handle_eop() to do deferred STATS processing
      //
      // NB actual stats address driven should NOT include bottom 2 LSBs as
      // these should always be zero and will be recreated at SRAM (hence >>2
      // in call the addresses below)
      // Also we switch on MSB (bit19) to indicate OP=stats
      uint32_t stats_dist_addr = (stats_addr>>2);
      if (addr_enable) stats_dist_addr |= (1u<<19);
      mau->reset_resources();
      printf("adist->distrib_stats_addresses: Stats should be deferred !!!!!!!!!!\n");

      // this is what update_addresses used to do
      Teop *null_teop = NULL;
      adist->distrib_stats_addresses(0, true, stats_dist_addr, null_teop);
      adist->defer_stats_addresses(0, true, stats_dist_addr, null_teop, eop_num_ok);

      //Don't call process_actions_stats_meters - handled by handle_eop later
      //mau->process_actions_stats_meters(phv);

      // Setup BAD EOP and call - should have no effect
      eop.set_ingress_eopinfo(pktlen, eop_num_bad);
      printf("mau->handle_eop: Stats processing for bad EOP should occur now !!!!!!!!!\n");
      printf("mau->handle_eop:       and should have no effect               !!!!!!!!!\n");
      mau->handle_eop(eop);

      // Setup good EOP and call - note not really deferring significantly here!
      eop.set_ingress_eopinfo(pktlen, eop_num_ok);
      printf("mau->handle_eop: Stats processing for good EOP should occur now !!!!!!!!!\n");
      mau->handle_eop(eop);
    }

    uint64_t data0, data1;
    uint64_t addr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats,
                                                   0, ((7<<15)|(15<<5))>>2);
    int type = (addr >> 30) & 0x3;

    // VPN7
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=7,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2, &data0, &data1);
    printf("1. Addr=%016" PRIx64 " Type=%d Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n",
           addr, type, data0, data1);
    if (Address::kGlobalAddrEnable || addr_enable) {
      // Half packets go to VPN 7 hence divide by 2
      EXPECT_EQ(data0, static_cast<uint64_t>(n_pkts/2));
      EXPECT_EQ(data1, static_cast<uint64_t>(pktlen*n_pkts/2));
    } else {
      EXPECT_EQ(data0, NON);
      EXPECT_EQ(data1, NON);
    }

    // Read full-res counter using similar addr but abusing DataSize field to contain 0x3
    uint64_t fdata0, fdata1;
    uint64_t faddr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2);
    faddr |= (3 << 28);
    //int ftype = (faddr >> 30) & 0x3;
    fdata0 = NON; fdata1 = NON;
    tu.IndirectRead(faddr, &fdata0, &fdata1);
    printf("1. FAdr=%016" PRIx64 " Type=%d FDat0=%016" PRIx64 " FDat1=%016" PRIx64 "\n",
           faddr, type, fdata0, fdata1);
    if (Address::kGlobalAddrEnable || addr_enable) {
      EXPECT_EQ(fdata0, static_cast<uint64_t>(n_pkts/2));
      EXPECT_EQ(fdata1, static_cast<uint64_t>(pktlen*n_pkts/2));
    } else {
      EXPECT_EQ(fdata0, NON);
      EXPECT_EQ(fdata1, NON);
    }

    // VPN8
    data0 = NON; data1 = NON;
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=8,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((8<<15)|(15<<5))>>2, &data0, &data1);
    printf("2. Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n", data0, data1);
    // Other half packets go to VPN 8 hence divide by 2
    if (Address::kGlobalAddrEnable || addr_enable) {
      EXPECT_EQ(data0, static_cast<uint64_t>(n_pkts/2));
      EXPECT_EQ(data1, static_cast<uint64_t>(pktlen*n_pkts/2));
    } else {
      EXPECT_EQ(data0, NON);
      EXPECT_EQ(data1, NON);
    }
    // Do a PBUS write to see what happens!
    tu.rwram_write(0, TestUtil::kVirtMemTypeStats, 0, ((8<<15)|(15<<5))>>2, data0, data1);

    // Free
    tu.phv_free(phv);
    // Schtum
    tu.finish_test();
    tu.quieten_log_flags();
  }


  TEST(BFN_TEST_NAME(StatsTest),EopPipe) {
    if (stats_print) RMT_UT_LOG_INFO("test_stats_eop_pipe()\n");
    int chip = 202;
    int pipe = 0;
    int stage = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    //auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);


    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = ONE; flags = HI;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Switch on retention of/access to FullResStats
    MauStatsAlu::kKeepFullResStats = true;
    MauMemory::kKeepFullResStats = true;
    MauMemory::kAccessFullResStats = true;

    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Switch on fatal/error/warn messages - happens by default these days
    //tu.update_log_flags(0, 0, 0, 0, 0, UINT64_C(0x7), ALL);

    // Switch *ON* specific outputs
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeParser;
    //flags = RmtDebug::kRmtDebugParserParseLoop;
    //tu.update_log_flags(pipes, 0, types, 0, 0, flags, ALL);

    // Switch *OFF* certain output
    //pipes = UINT64_C(1) << pipe;
    //types = UINT64_C(1) << RmtTypes::kRmtTypeMauTcam;
    //flags = RmtDebug::kRmtDebugMauTcam_Tcam3LookupDetail|RmtDebug::kRmtDebugMauTcam_Tcam3DebugMiss;
    //tu.update_log_flags(pipes, 0, types, 0, 0, 0, ~flags);

    // DEBUG setup done!!!!!!!!!!!!!!!!!!!!!!


    // Instantiate whole chip and fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    // Lookup this Pipe/Stage MAU and MAU_ADDR_DIST obj
    Mau *mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    MauAddrDist *adist = mau->mau_addr_dist();
    ASSERT_TRUE(adist != NULL);


    // Set all PHV ingress/egress threads for our 2 stages (chip9!)
    for (stage = 0; stage < 2; stage++) {
      tu.set_phv_range_all(stage, false);
      // Then set 1st 32 of the 64x32 64x8 to be ingress
      // and the 1st 48 of the 96x16 to be ingress
      tu.set_phv_ranges(stage, 32,0, 32,0, 32,16,0, true);
    }

    // Setup ingress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepAction, true);
    tu.set_dependency(1, TestUtil::kDepAction, true);
    // Setup egress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepConcurrent, false);
    tu.set_dependency(1, TestUtil::kDepConcurrent, false);

    // Setup single logical table for ingress in stage0
    tu.table_config(0, 0, true);    // stage0  table0  ingress

    // Then setup some STATS SRAMs
    // NB format 25 is Packet+Byte[1 entry,64b PKT,64b BYTE]
    // Want to test eop/deferred rams here so MUST USE ODD LOGICAL ROW (ie col>=6)
    // And must be an odd logical row on which we have a StatsALU (phys=2,log=5)
    tu.rwram_config(0, 2, 6,                           // stage0 row2 col6
                    TestUtil::kUnitramTypeStats, 0, 0, // type vpn0=0 vpn1=0
                    25, 0, false, true);     // s_format=25 log_table=0 egress=F  use_deferred=true
    tu.rwram_config(0, 2, 7,                           // stage0 row2 col7
                    TestUtil::kUnitramTypeStats, 0, 0, // type vpn0=0 vpn1=0
                    25, 0, false, true);     // s_format=25 log_table=0 egress=F use_deferred=true


    // UP debug now
    flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


    // Create a simple PHV (so we can set a
    // pkt length increment for byte counters)
    Phv *phv = tu.phv_alloc();
    int stats_vpn;
    int stats_index;
    uint32_t stats_addr;

    // Switch on/off global address enable if testing
    // MSB-based address enable
    //Address::kGlobalAddrEnable = false;
    bool addr_enable = true;
    int pktlen = 100;
    int n_pkts = 10; // KEEP EVEN and <160
    uint64_t data0, data1, addr;
    int type;
    Eop eop;

    // Now loop pushing packets in.....
    printf("Pushing packets in - all stats processing should be deferred !!!!!!!!!!\n");
    for (int i = 0; i < n_pkts; i++) {
      if ((i % 2) == 0) stats_vpn = 7; else stats_vpn = 8;
      stats_index = 15;
      stats_addr = (stats_vpn << 15) | (stats_index << 5);

      //
      // NB actual stats address driven should NOT include bottom 2 LSBs as
      // these should always be zero and will be recreated at SRAM (hence >>2
      // in call the addresses below)
      // Also we switch on MSB (bit19) to indicate OP=stats
      uint32_t stats_dist_addr = (stats_addr>>2);
      if (addr_enable) stats_dist_addr |= (1u<<19);
      mau->reset_resources();
      uint8_t eop_num = i<<1 | 0; // Pkt i comes from port i

      // this is what update_addresses used to do
      Teop *null_teop = NULL;
      adist->distrib_stats_addresses(0, true, stats_dist_addr, null_teop);
      adist->defer_stats_addresses(0, true, stats_dist_addr, null_teop, eop_num);

    }


    // INITIALLY expect all stats to have 0 val
    printf("Finished pushing packets in - stats processing was deferred !!!!!!!!!!\n");
    addr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats,
                                          0, ((7<<15)|(15<<5))>>2);
    type = (addr >> 30) & 0x3;

    // VPN7
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=7,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2, &data0, &data1);
    printf("1. Addr=%016" PRIx64 " Type=%d Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n",
           addr, type, data0, data1);
    EXPECT_EQ(data0, NON);
    EXPECT_EQ(data1, NON);

    uint64_t fdata0, fdata1, faddr;
    int ftype;
    // Read full-res counter using similar addr but abusing DataSize field to contain 0x3
    faddr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2);
    faddr |= (3 << 28);
    ftype = (faddr >> 30) & 0x3;
    fdata0 = NON; fdata1 = NON;
    tu.IndirectRead(faddr, &fdata0, &fdata1);
    printf("1. FAdr=%016" PRIx64 " Type=%d FDat0=%016" PRIx64 " FDat1=%016" PRIx64 "\n",
           faddr, ftype, fdata0, fdata1);
    EXPECT_EQ(fdata0, NON);
    EXPECT_EQ(fdata1, NON);

    // VPN8
    data0 = NON; data1 = NON;
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=8,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((8<<15)|(15<<5))>>2, &data0, &data1);
    printf("2. Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n", data0, data1);
    EXPECT_EQ(data0, NON);
    EXPECT_EQ(data1, NON);



    // Now loop doing EOP processing for packets already pushed in
    printf("Pushing EOPs in - stats processing should happen now !!!!!!!!!!\n");
    for (int i = 0; i < n_pkts; i++) {
      uint8_t eop_num = i<<1 | 0; // Pkt i comes from port i
      eop.set_ingress_eopinfo(pktlen, eop_num);
      mau->handle_eop(eop);
    }


    // NOW stats should have expected vals
    printf("Stats processing should have happened !!!!!!!!!!\n");
    addr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats,
                                          0, ((7<<15)|(15<<5))>>2);
    type = (addr >> 30) & 0x3;
    // VPN7
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=7,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2, &data0, &data1);
    printf("1. Addr=%016" PRIx64 " Type=%d Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n",
           addr, type, data0, data1);
    if (Address::kGlobalAddrEnable || addr_enable) {
      // Half packets go to VPN 7 hence divide by 2
      EXPECT_EQ(data0, static_cast<uint64_t>(n_pkts/2));
      EXPECT_EQ(data1, static_cast<uint64_t>(pktlen*n_pkts/2));
    } else {
      EXPECT_EQ(data0, NON);
      EXPECT_EQ(data1, NON);
    }

    // Read full-res counter using similar addr but abusing DataSize field to contain 0x3
    faddr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2);
    faddr |= (3 << 28);
    ftype = (faddr >> 30) & 0x3;
    fdata0 = NON; fdata1 = NON;
    tu.IndirectRead(faddr, &fdata0, &fdata1);
    printf("1. FAdr=%016" PRIx64 " Type=%d FDat0=%016" PRIx64 " FDat1=%016" PRIx64 "\n",
           faddr, ftype, fdata0, fdata1);
    if (Address::kGlobalAddrEnable || addr_enable) {
      EXPECT_EQ(fdata0, static_cast<uint64_t>(n_pkts/2));
      EXPECT_EQ(fdata1, static_cast<uint64_t>(pktlen*n_pkts/2));
    } else {
      EXPECT_EQ(fdata0, NON);
      EXPECT_EQ(fdata1, NON);
    }

    // VPN8
    data0 = NON; data1 = NON;
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=8,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((8<<15)|(15<<5))>>2, &data0, &data1);
    printf("2. Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n", data0, data1);
    // Other half packets go to VPN 8 hence divide by 2
    if (Address::kGlobalAddrEnable || addr_enable) {
      EXPECT_EQ(data0, static_cast<uint64_t>(n_pkts/2));
      EXPECT_EQ(data1, static_cast<uint64_t>(pktlen*n_pkts/2));
    } else {
      EXPECT_EQ(data0, NON);
      EXPECT_EQ(data1, NON);
    }



    // Free
    tu.phv_free(phv);
    // Schtum
    tu.finish_test();
    tu.quieten_log_flags();
  }



// ******************** JBAY only tests ********************

#ifdef MODEL_CHIP_JBAY_OR_LATER

   TEST(BFN_TEST_NAME(StatsTest),Teop) {
    if (stats_print) RMT_UT_LOG_INFO("test_stats_teop()\n");
    int chip = 202;
    int pipe = 0;
    int stage = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    //auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);


    // DEBUG setup....................
    uint64_t ONE = UINT64_C(1);
    uint64_t HI  = UINT64_C(0xFFFFFFFF00000000);
    uint64_t ALL = UINT64_C(0xFFFFFFFFFFFFFFFF);
    uint64_t FEW = UINT64_C(0xF); // FatalErrorWarn
    uint64_t NON = UINT64_C(0);
    uint64_t TOP = UINT64_C(1) << 63;
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = ONE; flags = HI;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Switch on retention of/access to FullResStats
    MauStatsAlu::kKeepFullResStats = true;
    MauMemory::kKeepFullResStats = true;
    MauMemory::kAccessFullResStats = true;
    // Relax PacketAction check to avoid spurious MISMATCH logs
    MauAddrDist::kRelaxPacketActionAtHdrCheck = true;

    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Instantiate whole chip and fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    // Lookup this Pipe/Stage MAU and MAU_ADDR_DIST obj
    Mau *mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    MauAddrDist *adist = mau->mau_addr_dist();
    ASSERT_TRUE(adist != NULL);


    // Set all PHV ingress/egress threads for our 2 stages (chip9!)
    for (stage = 0; stage < 2; stage++) {
      tu.set_phv_range_all(stage, false);
      // Then set 1st 32 of the 64x32 64x8 to be ingress
      // and the 1st 48 of the 96x16 to be ingress
      tu.set_phv_ranges(stage, 32,0, 32,0, 32,16,0, true);
    }

    // Setup ingress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepAction, true);
    tu.set_dependency(1, TestUtil::kDepAction, true);
    // Setup egress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepConcurrent, false);
    tu.set_dependency(1, TestUtil::kDepConcurrent, false);

    // Setup single logical table for egress in stage0/stage1
    tu.table_config(0,  0, false);    // stage0  table0   egress
    tu.table_config(1, 15, false);    // stage0  table15  egress

    // Then setup some STATS SRAMs
    // NB format 25 is Packet+Byte[1 entry,64b PKT,64b BYTE]
    // Want to test teop/deferred rams here so MUST USE ODD LOGICAL ROW (ie col>=6)
    // And must be an odd logical row on which we have a StatsALU (phys=2,log=5)
    tu.rwram_config(0, 2, 6,                           // stage0 row2 col6
                    TestUtil::kUnitramTypeStats, 0, 0, // type vpn0=0 vpn1=0
                    25, 0, true, false);     // s_format=25 log_table=0 egress=T use_deferred=F
    tu.rwram_config(0, 2, 7,                           // stage0 row2 col7
                    TestUtil::kUnitramTypeStats, 0, 0, // type vpn0=0 vpn1=0
                    25, 0, true, false);     // s_format=25 log_table=0 egress=T use_deferred=F

    // Now we setup the TEOP xbars
    // Setup LT 0 to use StatsALU 1 (on logrow 5) outputting/inputting from TEOP bus 3
    tu.set_teop_regs(0,  0, 1, 3); // stage0 table0  alu1 bus3
    // Setup LT 15 to use StatsALU 3 in stage1 outputting/inputting from TEOP bus 0
    tu.set_teop_regs(1, 15, 3, 0); // stage1 table15 alu3 bus0


    // UP debug now
    flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


    // Create a simple PHV (so we can set a
    // pkt length increment for byte counters)
    Phv *phv = tu.phv_alloc();
    int stats_vpn;
    int stats_index;
    uint32_t stats_addr;

    int pktlen = 100;
    int n_pkts = 10; // KEEP EVEN and <160
    uint64_t data0, data1, addr;
    int type;
    Teop teop;

    // Now loop pushing packets in.....
    printf("Pushing packets in - all stats processing should be deferred !!!!!!!!!!\n");
    for (int i = 0; i < n_pkts; i++) {
      if ((i % 2) == 0) stats_vpn = 7; else stats_vpn = 8;
      stats_index = 15;
      stats_addr = (stats_vpn << 15) | (stats_index << 5);

      // NB actual stats address driven should NOT include bottom 2 LSBs as
      // these should always be zero and will be recreated at SRAM (hence >>2
      // in call the addresses below)
      // Also we switch on MSB (bit19) to indicate OP=stats
      uint32_t stats_dist_addr = (1u<<19) | (stats_addr>>2);

      // Reset MAU and TEOP and distrib address
      mau->reset_resources();
      teop.reset();
      //adist->distrib_stats_addresses(0, false, stats_dist_addr, &teop);
      adist->defer_stats_addresses(0, false, stats_dist_addr, &teop, 0);

      // If we've setup regs right should see stats_addr (18b of it) on teop bus 3
      EXPECT_TRUE(teop.stats_en(3));
      EXPECT_EQ(teop.addr(3), stats_addr>>3);
    }


    // INITIALLY expect all stats to have 0 val
    printf("Finished pushing packets in - stats processing was deferred !!!!!!!!!!\n");
    addr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats,
                                          0, ((7<<15)|(15<<5))>>2);
    type = (addr >> 30) & 0x3;

    // VPN7
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=7,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2, &data0, &data1);
    printf("1. Addr=%016" PRIx64 " Type=%d Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n",
           addr, type, data0, data1);
    EXPECT_EQ(data0, NON);
    EXPECT_EQ(data1, NON);

    uint64_t fdata0, fdata1, faddr;
    int ftype;
    // Read full-res counter using similar addr but abusing DataSize field to contain 0x3
    faddr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2);
    faddr |= (3 << 28);
    ftype = (faddr >> 30) & 0x3;
    fdata0 = NON; fdata1 = NON;
    tu.IndirectRead(faddr, &fdata0, &fdata1);
    printf("1. FAdr=%016" PRIx64 " Type=%d FDat0=%016" PRIx64 " FDat1=%016" PRIx64 "\n",
           faddr, ftype, fdata0, fdata1);
    EXPECT_EQ(fdata0, NON);
    EXPECT_EQ(fdata1, NON);

    // VPN8
    data0 = NON; data1 = NON;
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=8,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((8<<15)|(15<<5))>>2, &data0, &data1);
    printf("2. Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n", data0, data1);
    EXPECT_EQ(data0, NON);
    EXPECT_EQ(data1, NON);


    // Now loop doing TEOP processing for packets already pushed in
    printf("Pushing TEOPs in - stats processing should happen now !!!!!!!!!!\n");
    for (int i = 0; i < n_pkts; i++) {
      // Recreate stats addr from scratch
      if ((i % 2) == 0) stats_vpn = 7; else stats_vpn = 8;
      stats_index = 15;
      stats_addr = (stats_vpn << 15) | (stats_index << 5);

      // Recreate original TEOP but now add in pktlen too
      teop.reset();
      teop.set_addr(3, stats_addr>>3, true, false); // StatsEn=true
      teop.set_byte_len(pktlen);

      // Call MAU back to handle - ALU 1 should extract pktlen from TEOP
      mau->handle_dp_teop(teop);
    }

    // And NOW stats should have expected vals
    printf("Stats processing should have happened !!!!!!!!!!\n");
    addr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats,
                                          0, ((7<<15)|(15<<5))>>2);
    type = (addr >> 30) & 0x3;
    // VPN7
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=7,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2, &data0, &data1);
    printf("1. Addr=%016" PRIx64 " Type=%d Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n",
           addr, type, data0, data1);
    // Half packets go to VPN 7 hence divide by 2
    EXPECT_EQ(data0, static_cast<uint64_t>(n_pkts/2));
    EXPECT_EQ(data1, static_cast<uint64_t>(pktlen*n_pkts/2));

    // Read full-res counter using similar addr but abusing DataSize field to contain 0x3
    faddr = TestUtil::make_virtual_address(pipe, 0, TestUtil::kVirtMemTypeStats, 0, ((7<<15)|(15<<5))>>2);
    faddr |= (3 << 28);
    ftype = (faddr >> 30) & 0x3;
    fdata0 = NON; fdata1 = NON;
    tu.IndirectRead(faddr, &fdata0, &fdata1);
    printf("1. FAdr=%016" PRIx64 " Type=%d FDat0=%016" PRIx64 " FDat1=%016" PRIx64 "\n",
           faddr, ftype, fdata0, fdata1);
    EXPECT_EQ(fdata0, static_cast<uint64_t>(n_pkts/2));
    EXPECT_EQ(fdata1, static_cast<uint64_t>(pktlen*n_pkts/2));

    // VPN8
    data0 = NON; data1 = NON;
    // Read vaddr for stage0, STATS, logtab0, addr(vpn=8,index=15)
    tu.rwram_read(0, TestUtil::kVirtMemTypeStats, 0, ((8<<15)|(15<<5))>>2, &data0, &data1);
    printf("2. Data0=%016" PRIx64 " Data1=%016" PRIx64 "\n", data0, data1);
    // Other half packets go to VPN 8 hence divide by 2
    EXPECT_EQ(data0, static_cast<uint64_t>(n_pkts/2));
    EXPECT_EQ(data1, static_cast<uint64_t>(pktlen*n_pkts/2));

    // Free
    tu.phv_free(phv);
    // Schtum
    tu.finish_test();
    tu.quieten_log_flags();
  }
#endif


}
