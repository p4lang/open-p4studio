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

  int do_break() {
    return 0;
  }

  bool virt_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


  TEST(BFN_TEST_NAME(VirtTest),StatsReadWriteAll) {
    if (virt_print) RMT_UT_LOG_INFO("test_virt_stats_read_write_all()\n");
    int chip = 202;
    int pipe = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, 0);

    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ONE; stages = HI; types = ALL; rows_tabs = FEW; cols = TOP; flags = NON;
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
          if (virt_print)
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
          if (virt_print)
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




  TEST(BFN_TEST_NAME(VirtTest),MeterReadWriteAll) {
    if (virt_print) RMT_UT_LOG_INFO("test_virt_meter_read_write_all()\n");
    int chip = 202;
    int pipe = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, 0);

    MauMeterAlu::kRelaxThreadCheck = true;

    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ONE; stages = HI; types = ALL; rows_tabs = FEW; cols = TOP; flags = NON;
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
    int ramtype = TestUtil::kUnitramTypeMeter;

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
          if (virt_print)
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
          if (virt_print)
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



  TEST(BFN_TEST_NAME(VirtTest),SelectorReadWriteAll) {
    if (virt_print) RMT_UT_LOG_INFO("test_virt_selector_read_write_all()\n");
    int chip = 202;
    int pipe = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, 0);

    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ONE; stages = HI; types = ALL; rows_tabs = FEW; cols = TOP; flags = NON;
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
    int ramtype = TestUtil::kUnitramTypeSelector;

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
                          format, row+roff, false, false);  // Logtab# == Row#
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
          if (virt_print)
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
          if (virt_print)
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


  TEST(BFN_TEST_NAME(VirtTest),StatefulReadWriteAll) {
    if (virt_print) RMT_UT_LOG_INFO("test_virt_stateful_read_write_all()\n");
    int chip = 202;
    int pipe = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, 0);

    MauMeterAlu::kRelaxThreadCheck = true;

    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ONE; stages = HI; types = ALL; rows_tabs = FEW; cols = TOP; flags = NON;
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
    int ramtype = TestUtil::kUnitramTypeStateful;

    int memtype = -1; // Virt address memtype corresponding to ramtype
    int shift   = -1; // Virt address shift
    int roff    = -1; // Row offset to take account of ALU placement
    int format  =  0;
    switch (ramtype) {
      case TestUtil::kUnitramTypeSelector: case TestUtil::kUnitramTypeStateful:
        memtype = TestUtil::kVirtMemTypeSelectorStateful; shift = 5; roff = 1; break;
        //case TestUtil::kUnitramTypeMeter:
        //memtype = TestUtil::kVirtMemTypeMeter; shift = 0; roff = 1; break;
        //case TestUtil::kUnitramTypeStats:
        //memtype = TestUtil::kVirtMemTypeStats; shift = 3; roff = 0; format = 25; break;
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
      for (int row = 0; row < 8; row+=2) {
        for (int col = 6; col < 12; col++) {
          tu.rwram_config(stage, row+roff, col, ramtype, 0, 0, // VPNs
                          format, row+roff, false, false);  // Logtab# == Row#
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
    // Only allow VPNs 6->11 as we only have maprams 6->11
    std::uniform_int_distribution<uint64_t> vpn_distribution(6,11);
    std::uniform_int_distribution<uint64_t> index_distribution(0,1023);
    std::uniform_int_distribution<uint64_t> subword_distribution(0,15);

    // Do we randomly chop and change access mechanism for each access?
    bool randomise_wr_access_mech = true;
    bool randomise_rd_access_mech = true;

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
        int lt03 = logtab_distribution(generator);
        int logtab = roff + lt03 + lt03;  // Logtab will be 0,2,4,6 or 1,3,5,7
        int vpn = vpn_distribution(generator);
        int index = index_distribution(generator);
        int subword = subword_distribution(generator);

        // Use lt03 to decide subword width and the fundamental subword_mask0
        int subword_width = (1<<lt03)*8; // LT 0=8b,1=16b,2=32b,3=64b
        uint64_t subword_mask0 = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64-subword_width);
        // And the actual subword bits used - chop off MSBs
        int shifted_subword = (subword << (1+lt03)) & 0x1F;
        // Update the subword to reflect this and make debug comprehensible
        subword = shifted_subword >> (1+lt03);

        // Use shifted_subword to determine whether we use r/wdata0 or r/wdata1
        int which01 = shifted_subword >> 4;
        // And the offset within r/wdata0 or r/wdata1
        int offset = (shifted_subword & 0xF) >> (1+lt03);
        // And so the particular subword_mask
        uint64_t subword_mask = subword_mask0 << (offset*subword_width);

        // Now setup address including huffman bits that convey width
        int addr = (vpn << (10+shift)) | (index << shift) | shifted_subword | ((1<<lt03)-1);

        // work out address and avoid writing to the same one twice
        uint64_t a = tu.make_virt_addr(pipe,stage,memtype,logtab,addr);
        if ( written.count(a) ) continue;
        written[a] = true;

        if (t==0) {
          // MASK otherwise model complains
          wdata0 = (which01 == 0) ?(wdata0 & subword_mask) :(UINT64_C(0));
          wdata1 = (which01 == 1) ?(wdata1 & subword_mask) :(UINT64_C(0));

          if (virt_print)
            printf("  Write %d,%d,%d,LT=%d,VPN=%d,Index=%d,Sub=%d Addr=0x%08x "
                   "%" PRIx64 " %" PRIx64 "\n",
                   pipe,stage,memtype,logtab,vpn,index,subword,addr, wdata1,wdata0);
          if (randomise_wr_access_mech) {
            MauMemory::kStatsVAddrPbusWriteBubbleEmulate = wrbubble;
            MauMemory::kMeterVAddrPbusWriteBubbleEmulate = wrbubble;
            MauMemory::kSelectorStatefulVAddrPbusWriteBubbleEmulate = wrbubble;
          }
          // NOTE: We *used to* write UNMASKED wdata0/wdata1 - the Model itself
          // used to only use the correct subword bits - the rest was left as junk
          // to verify the correct subset was being used and that the junk bits
          // weren't overwriting other subwords.
          // But THESE DAYS we write MASKED data0/data1 otherwise model complains
          tu.rwram_write(pipe,stage,memtype,logtab,addr, wdata0, wdata1);
        }
        if (t==1) {
          uint64_t rdata0,rdata1;
          if (randomise_rd_access_mech) {
            MauMemory::kStatsVAddrPbusReadBubbleEmulate = rdbubble;
            MauMemory::kMeterVAddrPbusReadBubbleEmulate = rdbubble;
            MauMemory::kSelectorStatefulVAddrPbusReadBubbleEmulate = rdbubble;
          }
          tu.rwram_read(pipe,stage,memtype,logtab,addr, &rdata0, &rdata1);

          if (virt_print)
            printf("  Read %d,%d,%d,LT=%d,VPN=%d,Index=%d,Sub=%d Addr=0x%08x "
                   "%" PRIx64 " %" PRIx64 "\n",
                   pipe,stage,memtype,logtab,vpn,index,subword,addr, rdata1,rdata0);

          // Do mask/zero and print on mismatch
          uint64_t tmp_wdata0 = (which01 == 0) ?(wdata0 & subword_mask) :(UINT64_C(0));
          uint64_t tmp_wdata1 = (which01 == 1) ?(wdata1 & subword_mask) :(UINT64_C(0));
          uint64_t tmp_rdata0 = (which01 == 0) ?(rdata0 & subword_mask) :(UINT64_C(0));
          uint64_t tmp_rdata1 = (which01 == 1) ?(rdata1 & subword_mask) :(UINT64_C(0));
          if ((tmp_wdata0 != tmp_rdata0) || (tmp_wdata1 != tmp_rdata1)) {
            printf("ERROR Wrote %d,%d,%d,LT=%d,VPN=%d,Index=%d,Sub=%d Addr=0x%08x "
                   "%016" PRIx64 " %016" PRIx64 " (%016" PRIx64 " %016" PRIx64 ")\n",
                   pipe,stage,memtype,logtab,vpn,index,subword,addr, wdata1,wdata0, tmp_wdata1,tmp_wdata0);
            printf("ERROR  Read %d,%d,%d,LT=%d,VPN=%d,Index=%d,Sub=%d Addr=0x%08x "
                   "%016" PRIx64 " %016" PRIx64 " (%016" PRIx64 " %016" PRIx64 ")\n",
                   pipe,stage,memtype,logtab,vpn,index,subword,addr, rdata1,rdata0, tmp_rdata1,tmp_rdata0);
          }

          // NOW we mask/zero wdata/rdata so we only compare correct subword
          wdata0 = (which01 == 0) ?(wdata0 & subword_mask) :(UINT64_C(0));
          wdata1 = (which01 == 1) ?(wdata1 & subword_mask) :(UINT64_C(0));
          rdata0 = (which01 == 0) ?(rdata0 & subword_mask) :(UINT64_C(0));
          rdata1 = (which01 == 1) ?(rdata1 & subword_mask) :(UINT64_C(0));
          EXPECT_EQ( wdata0, rdata0 );
          EXPECT_EQ( wdata1, rdata1 );
        }
      }
    }
  }




}
