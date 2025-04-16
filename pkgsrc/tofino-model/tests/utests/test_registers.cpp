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
#include <random>
#include <map>

#include "gtest.h"

#include <bitvector.h>
#include <rmt-object.h>
#include <rmt-object-manager.h>
#include <model_core/model.h>
#include <register_includes/reg.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;

  bool registers_print = false;
  bool masked_mapram_write = !RmtObject::is_tofinoA0();

  TEST(BFN_TEST_NAME(RegisterTest),MapramReadWrite) {
    if (registers_print) RMT_UT_LOG_INFO("test_registers_mapram_read_write()\n");

    // Create our TestUtil class (and set it to use indirect regs)
    int chip = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip);
    tu.set_use_ind_regs(true);

    // Instantiate whole chip
    //tu.chip_init_all();
    tu.update_log_flags(ALL, ALL, ALL, ALL, ALL, FEW, ALL);
    //tu.update_log_flags(ALL, ALL, ALL, ALL, ALL, ALL, ALL);

    int pipe_last = RmtDefs::map_mau_pipe(RmtDefs::kPipesMax-1,0xFF);
    int stage_last = (chip > 200) ?chip-200-1 :RmtDefs::kStagesMax-1;
    std::default_random_engine generator;
    std::uniform_int_distribution<uint64_t> data_distribution;
    std::uniform_int_distribution<uint64_t> address_distribution(0,1023);
    std::uniform_int_distribution<uint64_t> pipe_distribution(0,pipe_last);
    std::uniform_int_distribution<uint64_t> stage_distribution(0,stage_last);
    std::uniform_int_distribution<uint64_t> row_distribution(0,7);
    std::uniform_int_distribution<uint64_t> column_distribution(6,11);

    for (int t=0;t<2;++t) { // first time write, second time read back
      generator.seed( unsigned(0xDAB0D1B0D0B0DEB0) );
      std::map<uint64_t,bool> written;
      for (int i=0;i<10000;++i) {
        uint64_t wdata0 = data_distribution(generator);
        uint64_t wdata1 = data_distribution(generator);
        int pipe  = pipe_distribution(generator);
        int stage = stage_distribution(generator);
        int r = row_distribution(generator);
        int c = column_distribution(generator);
        int addr = address_distribution(generator);

        // Ensure we do full writes 50% of the time when doing masked_mapram_write
        if ((masked_mapram_write) && ((i % 2) == 0)) wdata0 &= ~(UINT64_C(0x7FF) << 11);

        // work out address and avoid writing to the same one twice
        uint64_t a = tu.make_mapram_addr(pipe, stage, r, c, addr);
        if ( written.count(a) ) continue;
        written[a] = true;

        if (t==0) {
          if (registers_print) printf("  Write %d,%d,%d,%d,%d %" PRIx64 " %" PRIx64 "\n",pipe,stage,
                                      r,c,addr,wdata1,wdata0);
          tu.mapram_write(pipe,stage,r,c,addr, wdata0, wdata1);
        }
        else {
          uint64_t rdata0,rdata1;
          tu.mapram_read(pipe,stage,r,c,addr, &rdata0, &rdata1);
          if (registers_print) printf("  Read %d,%d,%d,%d,%d %" PRIx64 " %" PRIx64 "\n",pipe,stage,
                                      r,c,addr,rdata1,rdata0);
          // If we've been using TofinoB0 then physical mapram writes
          // will have been masked using ~wdata0[21:11]
          uint64_t mask = (masked_mapram_write) ?(((~wdata0) >> 11) & UINT64_C(0x7FF)) :UINT64_C(0x7FF);
          bool err = ((rdata0 != (wdata0 & mask)) || (rdata1 != NON));
          if ((err) || (i < 10) || ((i % 999) == 0)) {
            const char* errok = (err) ?"ERROR" :"OK";
            printf("%s  Addr %2d,%2d,%2d,%2d,%4d Wrote 0x%016" PRIx64 " (MaskedWrite 0x%016" PRIx64
                   ")  Read 0x%016" PRIx64 " Mask=0x%016" PRIx64 "\n",
                   errok, pipe,stage,r,c,addr, wdata0, (wdata0 & mask), rdata0, mask);
            if (err) {
              // Repeat read to allow debug
              tu.mapram_read(pipe,stage,r,c,addr, &rdata0, &rdata1);
            }
          }

          EXPECT_EQ( wdata0 & mask, rdata0);
          EXPECT_EQ( NON, rdata1);
        }
      }
    }
  }

  TEST(BFN_TEST_NAME(RegisterTest),SramReadWrite) {
    if (registers_print) RMT_UT_LOG_INFO("test_registers_sram_read_write()\n");

    // Create our TestUtil class (and set it to use indirect regs)
    int chip = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip);
    tu.set_use_ind_regs(true);

    // Instantiate whole chip
    //tu.chip_init_all();
    tu.update_log_flags(ALL, ALL, ALL, ALL, ALL, FEW, ALL);

    int pipe_last = RmtDefs::map_mau_pipe(RmtDefs::kPipesMax-1,0xFF);
    int stage_last = (chip > 200) ?chip-200-1 :RmtDefs::kStagesMax-1;
    std::default_random_engine generator;
    std::uniform_int_distribution<uint64_t> data_distribution;
    std::uniform_int_distribution<uint64_t> address_distribution(0,1023);
    std::uniform_int_distribution<uint64_t> pipe_distribution(0,pipe_last);
    std::uniform_int_distribution<uint64_t> stage_distribution(0,stage_last);
    std::uniform_int_distribution<uint64_t> row_distribution(0,7);
    std::uniform_int_distribution<uint64_t> column_distribution(2,11);

    for (int t=0;t<2;++t) { // first time write, second time read back
      generator.seed( unsigned(0xDAB0D1B0D0B0DEB0) );
      std::map<uint64_t,bool> written;
      for (int i=0;i<10000;++i) {
        uint64_t wdata0 = data_distribution(generator);
        uint64_t wdata1 = data_distribution(generator);
        int pipe  = pipe_distribution(generator);
        int stage = stage_distribution(generator);
        int r = row_distribution(generator);
        int c = column_distribution(generator);
        int addr = address_distribution(generator);

        // work out address and avoid writing to the same one twice
        uint64_t a = tu.make_sram_addr(pipe, stage, r, c, addr);
        if ( written.count(a) ) continue;
        written[a] = true;

        if (t==0) {
          if (registers_print) printf("  Write %d,%d,%d,%d,%d %" PRIx64 " %" PRIx64 "\n",pipe,stage,
                                      r,c,addr,wdata1,wdata0);
          tu.sram_write(pipe,stage,r,c,addr, wdata0, wdata1);
        }
        else {
          uint64_t rdata0,rdata1;
          tu.sram_read(pipe,stage,r,c,addr, &rdata0, &rdata1);
          if (registers_print) printf("  Read %d,%d,%d,%d,%d %" PRIx64 " %" PRIx64 "\n",pipe,stage,
                                      r,c,addr,rdata1,rdata0);
          EXPECT_EQ( wdata0, rdata0);
          EXPECT_EQ( wdata1, rdata1);
        }
      }
    }
  }

  TEST(BFN_TEST_NAME(RegisterTest),RegisterArrayReadWrite) {

    if (registers_print) RMT_UT_LOG_INFO("test_registers_register_array_read_write()\n");

    int chip = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip);
    tu.set_use_ind_regs(true);
    //tu.chip_init_all();

    int pipe_last = RmtDefs::map_mau_pipe(RmtDefs::kPipesMax-1,0xFF);
    int stage_last = (chip > 200) ?chip-200-1 :RmtDefs::kStagesMax-1;
    std::default_random_engine generator;
    std::uniform_int_distribution<uint32_t> data_distribution(0,0xffff);
    std::uniform_int_distribution<uint64_t> which_distribution(0,1);
    std::uniform_int_distribution<uint64_t> pipe_distribution(0,pipe_last);
    std::uniform_int_distribution<uint64_t> stage_distribution(0,stage_last);
    std::uniform_int_distribution<uint64_t> row_distribution(0,7);
    for (int t=0;t<2;++t) { // first time write, second time read back
      generator.seed( unsigned(0xDAB2D1B2D0B2DEB2) );
      std::map<volatile uint32_t*,bool> written;
      for (int i=0;i<100;++i) {
        uint32_t wdata = data_distribution(generator);
        int pipe  = pipe_distribution(generator);
        int stage = stage_distribution(generator);
        int r = row_distribution(generator);
        int which = which_distribution(generator);

        auto& ahv_xbar = RegisterUtils::ref_mau(pipe,stage).rams.array.row[r].action_hv_xbar;

        volatile uint32_t* addr = &(ahv_xbar.action_hv_ixbar_input_bytemask[which]);

        if ( written.count(addr) ) continue;
        written[addr] = true;

        if (t==0) {
          if (registers_print) printf("  Write %d,%d,%d,%d %x\n",pipe,stage,r,which,wdata);
          GLOBAL_MODEL->OutWord( chip,addr,wdata );
        }
        else {
          uint32_t rdata = GLOBAL_MODEL->InWord( chip, addr );
          if (registers_print) printf("  Read %d,%d,%d,%d %x\n",pipe,stage,
                                      r,which,rdata);
          EXPECT_EQ( wdata, rdata);
        }
      }
    }
  }



  TEST(BFN_TEST_NAME(RegisterTest),RegisterArray3ReadWrite) {

    if (registers_print) RMT_UT_LOG_INFO("test_registers_register_array3_read_write()\n");

    int chip = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip);
    tu.set_use_ind_regs(true);
    //tu.chip_init_all();

    int pipe_last = RmtDefs::map_mau_pipe(RmtDefs::kPipesMax-1,0xFF);
    int stage_last = (chip > 200) ?chip-200-1 :RmtDefs::kStagesMax-1;
    std::default_random_engine generator;
    std::uniform_int_distribution<uint32_t> data_distribution(0,0x1f);
    std::uniform_int_distribution<int> pipe_distribution(0,pipe_last);
    std::uniform_int_distribution<int> stage_distribution(0,stage_last);
    std::uniform_int_distribution<int> a2_distribution(0,1);
    std::uniform_int_distribution<int> a1_distribution(0,15);
    std::uniform_int_distribution<int> a0_distribution(0,7);

    for (int t=0;t<2;++t) { // first time write, second time read back
      generator.seed( unsigned(0xDAB3D1B3D0B3DEB3) );
      std::map<volatile uint32_t*,bool> written;
      for (int i=0;i<10000;++i) {
        uint32_t wdata = data_distribution(generator);
        int pipe  = pipe_distribution(generator);
        int stage = stage_distribution(generator);
        int a2 = a2_distribution(generator);
        int a1 = a1_distribution(generator);
        int a0 = a0_distribution(generator);

        //auto& a_validbit = RegisterUtils::ref_mau(pipe,stage).tcams.vh_data_xbar.tcam_validbit_xbar_ctl;
        //volatile uint32_t* addr = &(a_validbit[a2][a1][a0]);
        auto& a_ctl = RegisterUtils::ref_mau(pipe,stage).rams.array.row[0].exactmatch_row_vh_xbar_byteswizzle_ctl;
        volatile uint32_t* addr = &(a_ctl[a2][a1][a0]);

        if ( written.count(addr) ) continue;
        written[addr] = true;

        if (t==0) {
          if (registers_print) printf("  Write %d,%d,%d,%d,%d %x\n",pipe,stage,a2,a1,a0,wdata);
          // OR in some random junk when writing to make sure it's not writing somewhere wider
          GLOBAL_MODEL->OutWord( chip,addr,wdata | 0xABCDABE0 );
        }
        else {
          uint32_t rdata = GLOBAL_MODEL->InWord( chip, addr );
          if (registers_print) printf("  Read %d,%d,%d,%d,%d %x\n",pipe,stage,a2,a1,a0,rdata);
          EXPECT_EQ( wdata, rdata);
        }
      }
    }
  }

}
