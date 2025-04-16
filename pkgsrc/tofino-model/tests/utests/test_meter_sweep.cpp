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
#include <chrono>

#include "meter_util.h"
#include "gtest.h"

#include <chip.h>
#include <rmt-object-manager.h>
#include <model_core/model.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

  bool msweep_print = true;
  bool msweep_print_lots = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;



  TEST(BFN_TEST_NAME(MeterSweepTest),SweepImmediate) {
    GLOBAL_MODEL->Reset();
    if (msweep_print) RMT_UT_LOG_INFO("test_msweep_sweep_immediate()\n");

    int chip = 7; // Full complement stages
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    MauMeterAlu::kRelaxThreadCheck = true;


    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Global vars
    const int n_iterations = 500;

    // Instantiate whole chip and fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    RmtSweeper *sweeper = om->sweeper_get();


    // Setup Stage0 SRAM[7,11] as a MeterSRAM tied to ALU 3 in Pipe0 & Pipe1
    // Use VPN=15 and LT=15
    int s = 0; int r = 7; int c = 11; int vpn = 15; int lt = vpn;
    int interval = 4;
    // interval=4 HolePos=4 (always 4 for Meter) VPN max/min=15 EN=1
    // (interval=X gives a sweep interval of 2^22+X cycles)
    int sctl = (interval << 17) | (4 << 13) | (vpn << 7) | (vpn << 1) | (1 << 0);


    // Setup initial value we'll install into MeterSRAM
    MeterEntry entry0;
    entry0.set_from_parameters(
        UINT64_C( 3400000000 ), // peak_rate_bits_per_second
        UINT64_C( 6 ),          // peak_burst_size milli seconds
        UINT64_C( 3300000000 ), // committed_rate_bits_per_second,
        UINT64_C( 5 ) );        // committed_burst_size milli seconds
    uint64_t data0 = entry0.get_data0();
    uint64_t data1 = entry0.get_data1();

    // On write, ~bits[21:11] used as write_mask so mask inverted VPN with 0x7FF
    // to keep bits[21:11] == 0 and ~bits[21:11] == 0x7FF thus writing all bits
    uint64_t vpn_write_val = ~(static_cast<uint64_t>(vpn)) & UINT64_C(0x7FF);

    for (int p = 0; p <= 1; p++) {
      // Setup LT 15 for ingress
      tu.table_config(p, s, lt, true);

      // Setup SRAM[7,11] - setup physically
      tu.rwram_config(p, s, r, c, TestUtil::kUnitramTypeMeter,
                      vpn, vpn, 0, lt, false, false); // Vpn0/Vpn1/Fmt/LT/Egr/UseDefRam

      // Setup meter_sweep_ctl
      auto& mau_base = MODEL_CHIP_NAMESPACE::RegisterUtils::ref_mau(p,s);
      auto& adist_regs = mau_base.rams.match.adrdist;
      auto a_meter_sweep_ctl = &adist_regs.meter_sweep_ctl[r/2];
      tu.OutWord((void*)a_meter_sweep_ctl, sctl);

      // Need to program up entries [0,1023] with initial vals - also maprams with VPN
      for (int i = 0; i < 1024; i++) {
        tu.sram_write(p, s, r, c, i, data0, data1);
        tu.mapram_write(p, s, r, c, i, vpn_write_val, UINT64_C(0));
      }
    }

    // Up debug
    //flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Now do lots of sweeps
    uint32_t n_errors = 0u;
    printf("Sweeping...(clock_rate=%" PRId64 ")\n", kClockRate);
    uint64_t T = UINT64_C(0);
    for (int i = 0; i < n_iterations; i++) {
      // Sweep - min delta - 800ps
      T += RmtSweeper::interval_to_psecs(0);
      sweeper->sweep(T);
    }

    // Now we go through the MeterSRAM [7,11] in Pipe0 Pipe1
    // and check the values are the same - and not the initial vals!
    // NOTE we do virtual reads here!
    printf("Checking...\n");
    for (int i = 0; i < 1024; i++) {
      uint32_t vaddr = (vpn << (10)) | (i << (0));
      uint64_t p0[2], p1[2];
      tu.rwram_read(0,0,TestUtil::kVirtMemTypeMeter,lt,vaddr,&p0[0],&p0[1]);
      tu.rwram_read(1,0,TestUtil::kVirtMemTypeMeter,lt,vaddr,&p1[0],&p1[1]);
      if ((p0[0] == data0) && (p0[1] == data1)) {
        printf("Pipe0: Index=%d still at initial vals!\n", i);
      }
      if ((p1[0] == data0) && (p1[1] == data1)) {
        printf("Pipe1: Index=%d still at initial vals!\n", i);
      }
      bool mismatch = ((p0[0] != p1[0]) || (p0[1] != p1[1]));
      if (mismatch || (i == 0)) {
        if (mismatch) n_errors++;
        printf("Pipe0/Pipe1 %s:  Pipe0 = 0x%" PRIx64 " %" PRIx64 " !!!!!!!!\n"
               "                 Pipe1 = 0x%" PRIx64 " %" PRIx64 " Index=%d\n",
               mismatch?"mismatch":"ok", p0[1], p0[0], p1[1], p1[0], i);
      }
    }
    printf("Checking...done\n");
    EXPECT_EQ(0u, n_errors);

    // Schtumm
    tu.finish_test();
    tu.quieten_log_flags();
  }


  TEST(BFN_TEST_NAME(MeterSweepTest),SweepBothAndCompare) {
    GLOBAL_MODEL->Reset();
    if (msweep_print) RMT_UT_LOG_INFO("test_msweep_sweep_both_and_compare()\n");

    int chip = 7; // Full complement stages
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    MauMeterAlu::kRelaxThreadCheck = true;
    MauAddrDist::kMeterSweepOnDemandPipe0 = true;
    Chip::kUseGlobalTimeIfZero = true;


    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


    // Global vars
    const int n_iterations = 500;

    // Instantiate whole chip and fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    RmtSweeper *sweeper = om->sweeper_get();


    // Setup Stage0 SRAM[7,11] as a MeterSRAM tied to ALU 3 in Pipe0 & Pipe1
    // Use VPN=15 and LT=15
    int s = 0; int r = 7; int c = 11; int vpn = 15; int lt = vpn;
    int interval = 4;
    // interval=4 HolePos=4 (always 4 for Meter) VPN max/min=15 EN=1
    // (interval=X gives a sweep interval of 2^22+X cycles)
    int sctl = (interval << 17) | (4 << 13) | (vpn << 7) | (vpn << 1) | (1 << 0);


    // Setup initial value we'll install into MeterSRAM
    MeterEntry entry0;
    entry0.set_from_parameters(
        UINT64_C( 3400000000 ), // peak_rate_bits_per_second
        UINT64_C( 6 ),          // peak_burst_size milli seconds
        UINT64_C( 3300000000 ), // committed_rate_bits_per_second,
        UINT64_C( 5 ) );        // committed_burst_size milli seconds
    uint64_t data0 = entry0.get_data0();
    uint64_t data1 = entry0.get_data1();

    // On write, ~bits[21:11] used as write_mask so mask inverted VPN with 0x7FF
    // to keep bits[21:11] == 0 and ~bits[21:11] == 0x7FF thus writing all bits
    uint64_t vpn_write_val = ~(static_cast<uint64_t>(vpn)) & UINT64_C(0x7FF);

    // Setup initial T
    uint64_t T = UINT64_C(123456789);
    sweeper->sweep(T);

    for (int p = 0; p <= 1; p++) {
      // Setup LT 15 for ingress
      tu.table_config(p, s, lt, true);

      // Setup SRAM[7,11] - setup physically
      tu.rwram_config(p, s, r, c, TestUtil::kUnitramTypeMeter,
                      vpn, vpn, 0, lt, false, false); // Vpn0/Vpn1/Fmt/LT/Egr/UseDefRam

      // Setup meter_sweep_ctl
      auto& mau_base = MODEL_CHIP_NAMESPACE::RegisterUtils::ref_mau(p,s);
      auto& adist_regs = mau_base.rams.match.adrdist;
      auto a_meter_sweep_ctl = &adist_regs.meter_sweep_ctl[r/2];
      tu.OutWord((void*)a_meter_sweep_ctl, sctl);

      // Need to program up entries [0,1023] with initial vals - also maprams with VPN
      for (int i = 0; i < 1024; i++) {
        tu.sram_write(p, s, r, c, i, data0, data1);
        tu.mapram_write(p, s, r, c, i, vpn_write_val, UINT64_C(0));
      }
      // Do physical reads to check data values as we expect
      for (int i = 0; i < 1024; i++) {
        uint64_t rd[2];
        tu.sram_read(p, s, r, c, i, &rd[0],&rd[1]);
        bool mismatch = ((rd[0] != data0) || (rd[1] != data1));
        if (mismatch || (i < 10) || ((i % 100) == 0)) {
          printf("PhysRD Pipe%d: Index[%4d] = 0x%" PRIx64 " %" PRIx64 " %s\n",
                 p, i, rd[1], rd[0], mismatch?"!!!!!!!!!!!!!!!!!!!!!":"");
        }
      }
      // Do virtual reads to check data values as we expect
      for (int i = 0; i < 1024; i++) {
        uint32_t vaddr = (vpn << (10)) | (i << (0)); // NO subwords bits on Meter CfgRd
        uint64_t rd[2];
        tu.rwram_read(p, s,TestUtil::kVirtMemTypeMeter,lt,vaddr,&rd[0],&rd[1]);
        bool mismatch = ((rd[0] != data0) || (rd[1] != data1));
        if (mismatch || (i < 10) || ((i % 100) == 0)) {
          printf("VirtRD Pipe%d: Index[%4d] = 0x%" PRIx64 " %" PRIx64 " %s\n",
                 p, i, rd[1], rd[0], mismatch?"!!!!!!!!!!!!!!!!!!!!!":"");
        }
      }
    }

    // Up debug
    //flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Now do lots of sweeps
    bool output_when_at_initial_value = false;
    uint32_t sweep_cnt = 0u, prev_sweep_cnt = 0u, n_errors = 0u;
    printf("Sweeping...(clock_rate=%" PRId64 ")\n", kClockRate);

    for (int it = 0; it < n_iterations; it++) {
      // Sweep - min delta - 800ps
      T += RmtSweeper::interval_to_psecs(0);
      sweeper->sweep(T);
      //printf("Iteration[%d] T=%" PRId64 "(%" PRIx64 ")\n", it, T, T);

      // See if a sweep has occurred
      sweep_cnt += sweeper->get_stage_sweep_cnt(s);
      if (prev_sweep_cnt < sweep_cnt) {
        prev_sweep_cnt = sweep_cnt;

        // Now we go through the MeterSRAM [7,11] in Pipe0 Pipe1
        // and check the values are the same
        printf("Iteration[%d] T=%" PRId64 "(%" PRIx64 ") SweepCnt=%d Checking...\n",
               it, T, T, sweep_cnt);

        // Track what words are still at initial value
        int minindex_at_initval[2] = {  99999,  99999 };
        int maxindex_at_initval[2] = { -99999, -99999 };

        // Go through all indices in MeterSRAM
        for (int i = 0; i < 1024; i++) {
          uint32_t vaddr = (vpn << (10)) | (i << (0));
          uint64_t prd[2][2], vrd[2][2];

          // Do virtual reads if index < iteration
          // These *should* match as a virtual read in Pipe0
          // should cause all sweeps to be caught up
          if (i <= it) {
            tu.rwram_read(0, s,TestUtil::kVirtMemTypeMeter,lt,vaddr,&vrd[0][0],&vrd[0][1]);
            tu.rwram_read(1, s,TestUtil::kVirtMemTypeMeter,lt,vaddr,&vrd[1][0],&vrd[1][1]);
            if ((vrd[0][0] != vrd[1][0]) || (vrd[0][1] != vrd[1][1])) {
              n_errors++;
              // Always squawk if we see Pipe0/Pipe1 data mismatch
              printf("Pipe0/1 VIRT mismatch:  Pipe0 = 0x%" PRIx64 " %" PRIx64 " !!!!!!!!\n"
                     "                        Pipe1 = 0x%" PRIx64 " %" PRIx64 " Index=%d\n",
                     vrd[0][1], vrd[0][0], vrd[1][1], vrd[1][0], i);
            }
            // Track if any entries still at initial val (or cycle back to it)
            for (int p = 0; p <= 1; p++) {
              if ((vrd[p][0] == data0) && (vrd[p][1] == data1)) {
                if (i < minindex_at_initval[p]) minindex_at_initval[p] = i;
                if (i > maxindex_at_initval[p]) maxindex_at_initval[p] = i;
              }
            }
            if (i == it) {
              // Otherwise occasionally output values read - use same index as iteration
              printf("  Pipe0[%d] and Pipe1[%d] = 0x%" PRIx64 " %" PRIx64 "\n",
                     i, i, vrd[0][1], vrd[0][0]);
            }

          } else {
            // Do physical reads in Pipe0 when index >= iteration
            // Physical reads don't provoke sweep catch up so these
            // should still be at initial programmed value
            tu.sram_read(0, s, r, c, i, &prd[0][0],&prd[0][1]);
            if ((prd[0][0] != data0) || (prd[0][1] != data1)) {
              n_errors++;
              // Always squawk if we see Pipe0/Pipe1 data mismatch
              printf("Pipe0/Data PHYS mismatch:  Pipe0 = 0x%" PRIx64 " %" PRIx64 " !!!!!!!!\n"
                     "                           Data  = 0x%" PRIx64 " %" PRIx64 " Index=%d\n",
                     prd[0][1], prd[0][0], data1, data0, i);
            }
          }

        } // for (int i = 0; i < 1024; i++)


        // Print out some debug showing which words are still at initial value in Pipe0/1
        if (output_when_at_initial_value) {
          for (int p = 0; p <= 1; p++) {
            if ((minindex_at_initval[p] < 99999) || (maxindex_at_initval[p] > -99999)) {
              printf("Iteration[%d] T=%" PRId64 " SweepCnt=%d AtInitVal[Pipe%d]=[%d,%d]\n",
                     it, T, sweep_cnt, p, minindex_at_initval[p], maxindex_at_initval[p]);
            }
          }
        }

      } // if (prev_sweep_cnt < sweep_cnt)

    } // for (int it = 0; it < n_iterations; it++)

    EXPECT_EQ(0u, n_errors);

    // Schtumm
    tu.finish_test();
    tu.quieten_log_flags();
  }



  TEST(BFN_TEST_NAME(MeterSweepTest),TimeSweepImmediate) {
    GLOBAL_MODEL->Reset();
    if (msweep_print) RMT_UT_LOG_INFO("test_msweep_time_sweep_immediate()\n");

    int chip = 7; // Full complement stages
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    MauMeterAlu::kRelaxThreadCheck = true;


    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Global vars
    const int n_iterations = 1600;

    // Instantiate whole chip and fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    RmtSweeper *sweeper = om->sweeper_get();


    // Setup Stage0 SRAM[7,11] as a MeterSRAM tied to ALU 3 in Pipe0 & Pipe1
    // Use VPN=15 and LT=15
    int p = 0; int s = 0; int r = 7; int c = 11; int vpn = 15; int lt = vpn;
    int interval = 4;
    // interval=4 HolePos=4 (always 4 for Meter) VPN max/min=15 EN=1
    // (interval=X gives a sweep interval of 2^22+X cycles)
    int sctl = (interval << 17) | (4 << 13) | (vpn << 7) | (vpn << 1) | (1 << 0);


    // Setup initial value we'll install into MeterSRAM
    MeterEntry entry0;
    entry0.set_from_parameters(
        UINT64_C( 3400000000 ), // peak_rate_bits_per_second
        UINT64_C( 6 ),          // peak_burst_size milli seconds
        UINT64_C( 3300000000 ), // committed_rate_bits_per_second,
        UINT64_C( 5 ) );        // committed_burst_size milli seconds
    uint64_t data0 = entry0.get_data0();
    uint64_t data1 = entry0.get_data1();

    // On write, ~bits[21:11] used as write_mask so mask inverted VPN with 0x7FF
    // to keep bits[21:11] == 0 and ~bits[21:11] == 0x7FF thus writing all bits
    uint64_t vpn_write_val = ~(static_cast<uint64_t>(vpn)) & UINT64_C(0x7FF);

    // Setup LT 15 for ingress
    tu.table_config(p, s, lt, true);

    // Setup SRAM[7,11] - setup physically
    tu.rwram_config(p, s, r, c, TestUtil::kUnitramTypeMeter,
                    vpn, vpn, 0, lt, false, false); // Vpn0/Vpn1/Fmt/LT/Egr/UseDefRam

    // Setup meter_sweep_ctl
    auto& mau_base = MODEL_CHIP_NAMESPACE::RegisterUtils::ref_mau(p,s);
    auto& adist_regs = mau_base.rams.match.adrdist;
    auto a_meter_sweep_ctl = &adist_regs.meter_sweep_ctl[r/2];
    tu.OutWord((void*)a_meter_sweep_ctl, sctl);

    // Need to program up entries [0,1023] with initial vals - also maprams with VPN
    for (int i = 0; i < 1024; i++) {
      tu.sram_write(p, s, r, c, i, data0, data1);
      tu.mapram_write(p, s, r, c, i, vpn_write_val, UINT64_C(0));
    }

    // Up debug
    //flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Now do lots of sweeps
    printf("Sweeping...(clock_rate=%" PRId64 ")\n", kClockRate);
    auto start = std::chrono::high_resolution_clock::now();
    uint64_t T = UINT64_C(0);
    for (int i = 0; i < n_iterations; i++) {
      // Sweep - min delta - 800ps
      T += RmtSweeper::interval_to_psecs(0);
      sweeper->sweep(T);
    }
    auto finish = std::chrono::high_resolution_clock::now();

    uint32_t n_sweeps = sweeper->get_stage_sweep_cnt(s);
    using std::chrono::microseconds;
    microseconds ms = std::chrono::duration_cast<microseconds>(finish - start);
    printf("Sweeping done immediately. %d sweeps in %" PRId64 " micros\n",
           n_sweeps, static_cast<uint64_t>(ms.count()));

    // Schtumm
    tu.finish_test();
    tu.quieten_log_flags();
  }



  TEST(BFN_TEST_NAME(MeterSweepTest),TimeSweepOnDemand) {
    GLOBAL_MODEL->Reset();
    if (msweep_print) RMT_UT_LOG_INFO("test_msweep_time_sweep_on_demand()\n");

    int chip = 7; // Full complement stages
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    MauMeterAlu::kRelaxThreadCheck = true;
    MauAddrDist::kMeterSweepOnDemandPipe0 = true;
    Chip::kUseGlobalTimeIfZero = true;


    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Global vars
    const int n_iterations = 1600;

    // Instantiate whole chip and fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    RmtSweeper *sweeper = om->sweeper_get();


    // Setup Stage0 SRAM[7,11] as a MeterSRAM tied to ALU 3 in Pipe0 & Pipe1
    // Use VPN=15 and LT=15
    int p = 0; int s = 0; int r = 7; int c = 11; int vpn = 15; int lt = vpn;
    int interval = 4;
    // interval=4 HolePos=4 (always 4 for Meter) VPN max/min=15 EN=1
    // (interval=X gives a sweep interval of 2^22+X cycles)
    int sctl = (interval << 17) | (4 << 13) | (vpn << 7) | (vpn << 1) | (1 << 0);


    // Setup initial value we'll install into MeterSRAM
    MeterEntry entry0;
    entry0.set_from_parameters(
        UINT64_C( 3400000000 ), // peak_rate_bits_per_second
        UINT64_C( 6 ),          // peak_burst_size milli seconds
        UINT64_C( 3300000000 ), // committed_rate_bits_per_second,
        UINT64_C( 5 ) );        // committed_burst_size milli seconds
    uint64_t data0 = entry0.get_data0();
    uint64_t data1 = entry0.get_data1();

    // On write, ~bits[21:11] used as write_mask so mask inverted VPN with 0x7FF
    // to keep bits[21:11] == 0 and ~bits[21:11] == 0x7FF thus writing all bits
    uint64_t vpn_write_val = ~(static_cast<uint64_t>(vpn)) & UINT64_C(0x7FF);

    // Setup LT 15 for ingress
    tu.table_config(p, s, lt, true);

    // Setup SRAM[7,11] - setup physically
    tu.rwram_config(p, s, r, c, TestUtil::kUnitramTypeMeter,
                    vpn, vpn, 0, lt, false, false); // Vpn0/Vpn1/Fmt/LT/Egr/UseDefRam

    // Setup meter_sweep_ctl
    auto& mau_base = MODEL_CHIP_NAMESPACE::RegisterUtils::ref_mau(p,s);
    auto& adist_regs = mau_base.rams.match.adrdist;
    auto a_meter_sweep_ctl = &adist_regs.meter_sweep_ctl[r/2];
    tu.OutWord((void*)a_meter_sweep_ctl, sctl);

    // Need to program up entries [0,1023] with initial vals - also maprams with VPN
    for (int i = 0; i < 1024; i++) {
      tu.sram_write(p, s, r, c, i, data0, data1);
      tu.mapram_write(p, s, r, c, i, vpn_write_val, UINT64_C(0));
    }

    // Up debug
    //flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Now do lots of sweeps
    // But here sweeps don't happen immediately !!!!!!!!!!!!!!!!
    printf("Sweeping...(clock_rate=%" PRId64 ")\n", kClockRate);
    auto start = std::chrono::high_resolution_clock::now();
    uint64_t T = UINT64_C(0);
    for (int i = 0; i < n_iterations; i++) {
      // Sweep - min delta - 800ps
      T += RmtSweeper::interval_to_psecs(0);
      sweeper->sweep(T);
    }
    // Sweeps happen on demand when entries are read !!!!!!!!!!!
    for (int i = 0; i < 1024; i++) {
      uint32_t vaddr = (vpn << (10)) | (i << (0));
      uint64_t p0[2];
      tu.rwram_read(0,0,TestUtil::kVirtMemTypeMeter,lt,vaddr,&p0[0],&p0[1]);
    }
    auto finish = std::chrono::high_resolution_clock::now();

    uint32_t n_sweeps = sweeper->get_stage_sweep_cnt(s);
    using std::chrono::microseconds;
    microseconds ms = std::chrono::duration_cast<microseconds>(finish - start);
    printf("Sweeping done on demand. %d sweeps in %" PRId64 " micros\n",
           n_sweeps, static_cast<uint64_t>(ms.count()));

    // Schtumm
    tu.finish_test();
    tu.quieten_log_flags();
  }



}
