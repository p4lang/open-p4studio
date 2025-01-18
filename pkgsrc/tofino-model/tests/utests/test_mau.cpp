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
#include <register_includes/reg.h>
#include <bitvector.h>
#include <rmt-object-manager.h>
#include <model_core/model.h>
#include <mau.h>
#include <port.h>
#include <packet.h>
#include <ipb.h>
#include "input_xbar_util.h"
#include "tcam_row_vh_util.h"

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;
extern int GLOBAL_RMT_ERROR;


namespace MODEL_CHIP_TEST_NAMESPACE {

  bool mau_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


  TEST(BFN_TEST_NAME(MauTest),SramTcamReadWrite0) {
    GLOBAL_MODEL->Reset();
    if (mau_print) RMT_UT_LOG_INFO("test_mau_sram_tcam_read_write0()\n");

    int chip = 202;
    int p = 0;
    int s = 0;

    // Instantiate whole chip - chip 9 so only 2 MAUs per pipe
    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, p, s);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    int ms = TestUtil::kPhysMemTypeSRAM;
    int mt = TestUtil::kPhysMemTypeTCAM;
    int r = 0;
    int sc = 2;
    int tc = 1;
    int i = 0;
    // Make address for Pipe0,Stage0,SRAM(0,0),Index0
    uint64_t saddr = TestUtil::make_physical_address(p,s,ms,r,sc,i);
    uint64_t taddr = TestUtil::make_physical_address(p,s,mt,r,tc,i);
    uint64_t wdata0 = UINT64_C(0xF0F0F0F0F0F0F0F0);
    uint64_t wdata1 = UINT64_C(0xF1F1F1F1F1F1F1F1);
    RMT_UT_LOG_INFO("saddr =%016" PRIx64 " taddr =%016" PRIx64 "\n",
                saddr, taddr);

    // Output an entry to Pipe0,Stage0,SRAM(0,0),Index0
    GLOBAL_MODEL->IndirectWrite(chip, saddr, wdata0, wdata1);
    // Output an entry to Pipe0,Stage0,TCAM(0,0),Index0
    GLOBAL_MODEL->IndirectWrite(chip, taddr, wdata0, wdata1);

    // Lookup this Pipe/Stage/SRAM/TCAM
    Mau *mau = om->mau_lookup(p,s);
    ASSERT_TRUE(mau != NULL);
    MauSram *sram = mau->sram_lookup(r,sc);
    ASSERT_TRUE(sram != NULL);
    MauTcam *tcam = mau->tcam_lookup(r,tc);
    ASSERT_TRUE(tcam != NULL);

    // Read back data directly from SRAM
    uint64_t rdata0;
    uint64_t rdata1;
    rdata0 = UINT64_C(0);
    rdata1 = UINT64_C(0);
    sram->read(i, &rdata0, &rdata1, UINT64_C(0));
    EXPECT_EQ(rdata0,wdata0);
    EXPECT_EQ(rdata1,wdata1);
    RMT_UT_LOG_INFO("wdata0=%016" PRIx64 " wdata1=%016" PRIx64 "\n",
                wdata0, wdata1);
    RMT_UT_LOG_INFO("rdata0=%016" PRIx64 " rdata1=%016" PRIx64 "\n",
                rdata0, rdata1);

    // Read back data indirectly from sram
    uint64_t irdata0;
    uint64_t irdata1;
    irdata0 = UINT64_C(0);
    irdata1 = UINT64_C(0);
    GLOBAL_MODEL->IndirectRead(chip, saddr, &irdata0, &irdata1);
    EXPECT_EQ(irdata0,wdata0);
    EXPECT_EQ(irdata1,wdata1);
    RMT_UT_LOG_INFO("wdata0=%016" PRIx64 " wdata1=%016" PRIx64 "\n",
                wdata0, wdata1);
    RMT_UT_LOG_INFO("irdata0=%016" PRIx64 " irdata1=%016" PRIx64 "\n",
                irdata0, irdata1);

    // Read back data directly from TCAM - only get bot 47 bits
    rdata0 = UINT64_C(1);
    rdata1 = UINT64_C(1);
    tcam->read(i, &rdata0, &rdata1, UINT64_C(0));
    EXPECT_EQ(rdata0,wdata0 & UINT64_C(0x7FFFFFFFFFFF));
    EXPECT_EQ(rdata1,wdata1 & UINT64_C(0x7FFFFFFFFFFF));
    RMT_UT_LOG_INFO("wdata0=%016" PRIx64 " wdata1=%016" PRIx64 "\n",
                wdata0, wdata1);
    RMT_UT_LOG_INFO("rdata0=%016" PRIx64 " rdata1=%016" PRIx64 "\n",
                rdata0, rdata1);

    // Free everything up
    om->chip_free_all();
  }

  TEST(BFN_TEST_NAME(MauTest),SramTcamReadWrite) {
    GLOBAL_MODEL->Reset();
    if (mau_print) RMT_UT_LOG_INFO("test_mau_sram_tcam_read_write()\n");

    // Create our TestUtil class
    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    // Write some data to a SRAM/TCAM
    int r = 0;
    int sc = 2;
    int tc = 1;
    int i = 0;
    uint64_t wdata0 = UINT64_C(0xF0F0F0F0F0F0F0F0);
    uint64_t wdata1 = UINT64_C(0xF1F1F1F1F1F1F1F1);

    tu.sram_write(r,sc,i, wdata0, wdata1);
    // Only writes 44 bits - but adds 0 LSB - so 45 total
    tu.tcam_write(r,tc,i, wdata0, wdata1);

    // Lookup this Pipe/Stage/SRAM/TCAM
    Mau *mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    MauSram *sram = mau->sram_lookup(r,sc);
    ASSERT_TRUE(sram != NULL);
    MauTcam *tcam = mau->tcam_lookup(r,tc);
    ASSERT_TRUE(tcam != NULL);

    // Read back data directly from SRAM
    uint64_t rdata0;
    uint64_t rdata1;
    rdata0 = UINT64_C(0);
    rdata1 = UINT64_C(0);
    sram->read(i, &rdata0, &rdata1, UINT64_C(0));
    EXPECT_EQ(rdata0,wdata0);
    EXPECT_EQ(rdata1,wdata1);
    RMT_UT_LOG_INFO("SRAM: wdata0=%016" PRIx64 " wdata1=%016" PRIx64 "\n",
           wdata0, wdata1);
    RMT_UT_LOG_INFO("SRAM: rdata0=%016" PRIx64 " rdata1=%016" PRIx64 "\n",
           rdata0, rdata1);

    // Read back data directly from TCAM - only get bot 45 bits
    // and we have to shift the data we read right by 1 to remove
    // payload bit, so we compare 44
    rdata0 = UINT64_C(1);
    rdata1 = UINT64_C(1);
    tcam->read(i, &rdata0, &rdata1, UINT64_C(0));
    rdata0 >>= 1;
    rdata1 >>= 1;
    EXPECT_EQ(rdata0,wdata0 & UINT64_C(0xFFFFFFFFFFF));
    EXPECT_EQ(rdata1,wdata1 & UINT64_C(0xFFFFFFFFFFF));
    RMT_UT_LOG_INFO("TCAM: wdata0=%016" PRIx64 " wdata1=%016" PRIx64 "\n",
           wdata0, wdata1);
    RMT_UT_LOG_INFO("TCAM: rdata0=%016" PRIx64 " rdata1=%016" PRIx64 "\n",
           rdata0, rdata1);

    // Free everything up
    om->chip_free_all();
  }

  TEST(BFN_TEST_NAME(MauTest),SramTcamReadWriteIndirect) {
    GLOBAL_MODEL->Reset();
    if (mau_print) RMT_UT_LOG_INFO("test_mau_sram_tcam_read_write_indirect()\n");

    // Create our TestUtil class (and set it to use indirect regs)
    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_use_ind_regs(true);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    // Write some data to a SRAM/TCAM
    int r = 0;
    int sc = 2;
    int tc = 1;
    int i = 0;
    uint64_t wdata0 = UINT64_C(0xF0F0F0F0F0F0F0F0);
    uint64_t wdata1 = UINT64_C(0xF1F1F1F1F1F1F1F1);

    tu.sram_write(r,sc,i, wdata0, wdata1);
    // Only writes 44 bits - but adds 0 LSB - so 45 total
    tu.tcam_write(r,tc,i, wdata0, wdata1);

    // Lookup this Pipe/Stage/SRAM/TCAM
    Mau *mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    MauSram *sram = mau->sram_lookup(r,sc);
    ASSERT_TRUE(sram != NULL);
    MauTcam *tcam = mau->tcam_lookup(r,tc);
    ASSERT_TRUE(tcam != NULL);

    // Read back data directly from SRAM
    uint64_t rdata0;
    uint64_t rdata1;
    rdata0 = UINT64_C(0);
    rdata1 = UINT64_C(0);
    sram->read(i, &rdata0, &rdata1, UINT64_C(0));
    EXPECT_EQ(rdata0,wdata0);
    EXPECT_EQ(rdata1,wdata1);
    RMT_UT_LOG_INFO("wdata0=%016" PRIx64 " wdata1=%016" PRIx64 "\n",
                wdata0, wdata1);
    RMT_UT_LOG_INFO("rdata0=%016" PRIx64 " rdata1=%016" PRIx64 "\n",
                rdata0, rdata1);

    // Read back data directly from TCAM - only get bot 45 bits
    // and we have to shift the data we read right by 1 to remove
    // payload bit, so we compare 44
    rdata0 = UINT64_C(1);
    rdata1 = UINT64_C(1);
    tcam->read(i, &rdata0, &rdata1, UINT64_C(0));
    rdata0 >>= 1;
    rdata1 >>= 1;
    EXPECT_EQ(rdata0,wdata0 & UINT64_C(0xFFFFFFFFFFF));
    EXPECT_EQ(rdata1,wdata1 & UINT64_C(0xFFFFFFFFFFF));
    RMT_UT_LOG_INFO("wdata0=%016" PRIx64 " wdata1=%016" PRIx64 "\n",
                wdata0, wdata1);
    RMT_UT_LOG_INFO("rdata0=%016" PRIx64 " rdata1=%016" PRIx64 "\n",
                rdata0, rdata1);
  }

  TEST(BFN_TEST_NAME(MauTest),TcamReadWriteWithParity) {
    GLOBAL_MODEL->Reset();
    if (mau_print) RMT_UT_LOG_INFO("test_mau_tcam_read_write_with_parity()\n");

    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    // Create our TestUtil class
    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_free_on_exit(true);
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    // Write some data to a TCAM
    // We'll be doing a raw write so make sure bottom bit of w0
    // is SET else we could end up doing range searches
    uint64_t wdata0 = UINT64_C(0xFFFFFF0F0F0F0F0F);
    uint64_t wdata1 = UINT64_C(0xFFFFFF0F0F0F0F0F);

    // Raw write - unconstrained by TestUtil
    // Underlying code should only write 47 bits though
    tu.tcam_write_raw(pipe, stage, 4,0,511, wdata0, wdata1);

    // Lookup this Pipe/Stage/SRAM/TCAM
    Mau *mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    MauTcam *tcam = mau->tcam_lookup(4,0);
    ASSERT_TRUE(tcam != NULL);

    // Read back data directly from TCAM - should get 47 bits
    uint64_t rdata0 = UINT64_C(1);
    uint64_t rdata1 = UINT64_C(1);
    tcam->read(511, &rdata0, &rdata1, UINT64_C(0));
    EXPECT_EQ(rdata0,wdata0 & UINT64_C(0x7FFFFFFFFFFF));
    EXPECT_EQ(rdata1,wdata1 & UINT64_C(0x7FFFFFFFFFFF));
    RMT_UT_LOG_INFO("TCAM[511]: wdata0=%016" PRIx64 " wdata1=%016" PRIx64 "\n",
           wdata0, wdata1);
    RMT_UT_LOG_INFO("TCAM[511]: rdata0=%016" PRIx64 " rdata1=%016" PRIx64 "\n",
           rdata0, rdata1);

    // Check haven't broken lookup
    // Write a specific value/mask which we try and match against when we lookup(bv)
    // Pad out value/mask with parity/payload bits - shouldn't affect match
    // REMEMBER to shift value/mask left by 1!!!!!!!!
    // REMEMBER to set bit0 of w0 to 1 else we'll do ranged searches
    tu.set_debug(false);
    uint64_t value = UINT64_C(0x123456789AB);
    uint64_t mask  = UINT64_C(0xFFFFFF00000);
    uint64_t w1 = (UINT64_C(0x3) << 45) |  (((value & mask) | ~mask) << 1) | (UINT64_C(0x1) << 0);
    uint64_t w0 = (UINT64_C(0x3) << 45) | (((~value & mask) | ~mask) << 1) | (UINT64_C(0x1) << 0);
    RMT_UT_LOG_INFO("TCAM_write_raw: w0=%016" PRIx64 " w1=%016" PRIx64 "\n", w0, w1);
    tu.tcam_write_raw(pipe, stage, 4,0,510, w0,w1);

    // Read back
    tcam->read(510, &rdata0, &rdata1, UINT64_C(0));
    EXPECT_EQ(rdata0,w0 & UINT64_C(0x7FFFFFFFFFFF));
    EXPECT_EQ(rdata1,w1 & UINT64_C(0x7FFFFFFFFFFF));
    RMT_UT_LOG_INFO("TCAM[510]: wdata0=%016" PRIx64 " wdata1=%016" PRIx64 "\n",
           w0, w1);
    RMT_UT_LOG_INFO("TCAM[510]: rdata0=%016" PRIx64 " rdata1=%016" PRIx64 "\n",
           rdata0, rdata1);


    // Now lookup - expect index AND pri to be 510
    // because TCAM should be in initial reset state
    // with HEADPTR=0 (=> tcam_start=511)
    BitVector<44> bv(UINT64_C(0x12345600000));
    //tcam->set_log_flags(UINT64_C(0xFFFFFFFFFFFFFFFF));
    int tcam_start = tcam->get_tcam_start();
    RMT_UT_LOG_INFO("TCAM start = %d\n", tcam_start);

    int hitindex = tcam->lookup_index(bv);
    RMT_UT_LOG_INFO("BV lookup hitindex=%d\n", hitindex);
    EXPECT_EQ(510, hitindex);
    RMT_UT_LOG_INFO("TCAM: lookup_index(0x12345600000)=%d\n", hitindex);

    int hitpri = tcam->lookup_pri(bv);
    RMT_UT_LOG_INFO("BV lookup hitpri=%d\n", hitpri);
    EXPECT_EQ(510, hitpri);
    RMT_UT_LOG_INFO("TCAM: lookup_pri(0x12345600000)=%d\n", hitpri);

    // Quieten down!
    tu.finish_test();
    tu.quieten_log_flags();
  }

  TEST(BFN_TEST_NAME(MauTest),TcamReadWriteUsingWriteReg) {
    GLOBAL_MODEL->Reset();
    if (mau_print) RMT_UT_LOG_INFO("test_mau_tcam_read_write_using_writereg()\n");

    // Create our TestUtil class
    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);

    // Fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    // Write some data to a TCAM
    uint64_t wdata0 = UINT64_C(0xFFFFFFFFFFFEDCBA);
    uint64_t wdata1 = UINT64_C(0xFFFFFFFFFFFEDCBA);
    uint64_t rdata0 = UINT64_C(1);
    uint64_t rdata1 = UINT64_C(1);

    // Use write_reg - unconstrained by TestUtil
    // Underlying code should only write 47 bits though
    tu.tcam_write_with_writereg(pipe, stage, 11,1, 511, wdata0, wdata1);
    RMT_UT_LOG_INFO("TCAM before: wdata0=%016" PRIx64 " wdata1=%016" PRIx64 "\n",
                wdata0, wdata1);
    RMT_UT_LOG_INFO("TCAM before: rdata0=%016" PRIx64 " rdata1=%016" PRIx64 "\n",
                rdata0, rdata1);

    // Read back using raw PipeBusRead
    tu.tcam_read_raw(pipe, stage, 11,1, 511, &rdata0,&rdata1);
    RMT_UT_LOG_INFO("TCAM after: wdata0=%016" PRIx64 " wdata1=%016" PRIx64 "\n",
                wdata0, wdata1);
    RMT_UT_LOG_INFO("TCAM after: rdata0=%016" PRIx64 " rdata1=%016" PRIx64 "\n",
                rdata0, rdata1);
    EXPECT_EQ(rdata0,wdata0 & UINT64_C(0x7FFFFFFEDCBA));
    EXPECT_EQ(rdata1,wdata1 & UINT64_C(0x7FFFFFFEDCBA));

    // Now lookup this Pipe/Stage/SRAM/TCAM
    Mau *mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    MauTcam *tcam = mau->tcam_lookup(11,1);
    ASSERT_TRUE(tcam != NULL);
    // And read back data directly from TCAM - should get 47 bits again
    tcam->read(511, &rdata0, &rdata1, UINT64_C(0));
    EXPECT_EQ(rdata0,wdata0 & UINT64_C(0x7FFFFFFEDCBA));
    EXPECT_EQ(rdata1,wdata1 & UINT64_C(0x7FFFFFFEDCBA));
    RMT_UT_LOG_INFO("TCAM: wdata0=%016" PRIx64 " wdata1=%016" PRIx64 "\n",
                wdata0, wdata1);
    RMT_UT_LOG_INFO("TCAM: rdata0=%016" PRIx64 " rdata1=%016" PRIx64 "\n",
                rdata0, rdata1);
  }

  TEST(BFN_TEST_NAME(MauTest),ReadWriteTcamVhXbar) {
    GLOBAL_MODEL->Reset();
    if (mau_print) RMT_UT_LOG_INFO("test_mau_read_write_tcam_vh_xbar()\n");

    //$ grep 4040380 design/build/rtl/tofino.vh
    //define TOFINO_PIPES_MAU_TCAMS_VH_DATA_XBAR_TCAM_ROW_OUTPUT_CTL_ADDRESS 28'h4040380
    //define TOFINO_PIPES_MAU_TCAMS_VH_DATA_XBAR_TCAM_ROW_OUTPUT_CTL_BYTE_ADDRESS 28'h4040380

    // Create our TestUtil class - chip 7 so full complement stages
    int chip = 7;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    // Read/Write all tcam_row_halfbyte_mux_ctl regs
    auto& tcam_xbars = RegisterUtils::ref_mau(pipe,stage).tcams.vh_data_xbar;
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 12; j++) {
        auto a_mux_ctl = &tcam_xbars.tcam_row_halfbyte_mux_ctl[i][j];
        uint32_t v_mux_ctl = tu.InWord((void*)a_mux_ctl);
        uint32_t w_mux_ctl = (~v_mux_ctl & 0xF);
        uint32_t r_mux_ctl;
        // Invert, re-read, check we get back inverted version
        tu.OutWord((void*)a_mux_ctl, w_mux_ctl, "tcam_row_halfbyte_mux_ctl");
        r_mux_ctl = tu.InWord((void*)a_mux_ctl);
        EXPECT_EQ(r_mux_ctl, w_mux_ctl);
        // Put things back way they were
        tu.OutWord((void*)a_mux_ctl, v_mux_ctl, "tcam_row_halfbyte_mux_ctl");
        r_mux_ctl = tu.InWord((void*)a_mux_ctl);
        EXPECT_EQ(r_mux_ctl, v_mux_ctl);
      }
    }
    // Write all tcam_row_output_ctl regs
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 12; j++) {
        auto a_row_ctl = &tcam_xbars.tcam_row_output_ctl[i][j];
        uint32_t v_row_ctl = tu.InWord((void*)a_row_ctl);
        uint32_t w_row_ctl = (~v_row_ctl & 0x1F);
        uint32_t r_row_ctl;
        // Invert, re-read, check we get back inverted version
        // NOTE this reg unimplemented so should see printf output saying
        // "Read/Write of TcamRowOutputCtlArray2 register not supported"
        tu.OutWord((void*)a_row_ctl, w_row_ctl, "tcam_row_output_ctl");
        r_row_ctl = tu.InWord((void*)a_row_ctl);
        EXPECT_EQ(r_row_ctl, w_row_ctl);
        // Put things back way they were
        tu.OutWord((void*)a_row_ctl, v_row_ctl, "tcam_row_output_ctl");
        r_row_ctl = tu.InWord((void*)a_row_ctl);
        EXPECT_EQ(r_row_ctl, v_row_ctl);
      }
    }
  }

  TEST(BFN_TEST_NAME(MauTest),TcamLookupPacket) {
    GLOBAL_MODEL->Reset();
    if (mau_print) RMT_UT_LOG_INFO("test_mau_tcam_lookup_packet()\n");

    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    int lt0 = 0, lt5 = 5;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_free_on_exit(true);

    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Addrs enabled by default - and MeterOP==NOP so relax check
    Address::kGlobalAddrEnable = true;
    MauLogicalTable::kRelaxHdrtimeMeterAddrNopCheck = true;
    // Uses PFE within LSB zero-padding - relax checks
    MauLookupResult::kRelaxLookupShiftPfePosCheck = true;
    MauLookupResult::kRelaxLookupShiftOpPosCheck = true;

    // Switch on fatal/error/warn messages - happens by default these days
    //tu.update_log_flags(0, 0, 0, 0, 0, FEW, ALL);

    // Switch on all parse loop output
    pipes = UINT64_C(1) << pipe;
    types = UINT64_C(1) << RmtTypes::kRmtTypeParser;
    flags = RmtDebug::kRmtDebugParserParseLoop;
    //tu.update_log_flags(pipes, 0, types, 0, 0, flags, ALL);

    // Switch on TCAM more_detail output
    pipes = UINT64_C(1) << pipe;
    types = UINT64_C(1) << RmtTypes::kRmtTypeMauTcam;
    flags = RmtDebug::kRmtDebugMauTcam_Tcam3LookupDetail;
    //tu.update_log_flags(pipes, 0, types, 0, 0, flags, ALL);

    // this turns on debugging in the tcam row so you can see what the search bus ends up with
    // constexpr uint64_t one = 1;
    //tu.update_log_flags( pipes, one << 0, one << RmtTypes::kRmtTypeMauTcamRow,
    //                     one << lt0 /* rows */, 0 /* cols */, UINT64_C(0xff), ALL );

    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

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
    //
    tu.table_config(0, lt0, true);     // stage0 table0  ingress
    tu.table_config(0, lt5, true);     // stage0 table5  ingress
    // Setup LT default regs
    tu.set_table_default_regs(0, lt0); // stage0 table0
    tu.set_table_default_regs(0, lt5); // stage0 table5


    // Setup this logical table 0 and physical result bus 6
    // to have certan shift/mask/dflt/miss vals
    //
    // Typically shift/mask/dflt configured on physical result bus
    // and miss configured on table
    //
    // NxtTab miss=5  Instr miss=0x6  Data miss=0xABABABAB
    // Instr miss will turn into 0xC once LSB set to ingress=0
    //   which translates to opindex=3,colour=0,ingress=0
    //
    // NB - the +5 is to shift off the 00000s tacked onto the LSB
    tu.physbus_config(0,    lt0,  6,       // stage0  table0  physResultBus6
                      0,    0xFF, 0, lt5,  // nxt_tab shft/mask/dflt/miss (shift ignored)
                      16,   0xFF, 0, 0x6,  // instr   shft/mask/dflt/miss (no pad for instr addr)
                      32+5, 0xFFFFFFFF, 0, 0xABABABAB); // action_addr

    // Now setup a single logical tcam (4) to refer to this table 0
    tu.ltcam_config(0, 4, lt0, 4, (uint8_t)4); // stage0 ltcam4 table0 physbus4 matchAddrShift

    // Setup stage default regs (need to do this after dependency
    // config so mpr_bus_dep can get setup properly);
    tu.set_stage_default_regs(0);
    tu.set_stage_default_regs(1);


    // Then setup a TCAM(4,0) within this logical TCAM
    // We use vpn now in match_addr NOT tcam_index so for now set vpn to be same (4)
    tu.tcam_config(0, 4,   0,       // stage0 row4 col0,
                   0, lt0, 4, 4,    // inbus0 table0 ltcam4 vpn4
                   true, false, true, 1); // ingress chain output head

    tu.set_debug(false);
    //RMT_UT_LOG_INFO("MauTest::Adding TIND 3,3\n");
    // Then setup a TIND SRAM - has to be on row 3 to drive physical result bus 6
    tu.sram_config(0, 3, 3,                          // stage0 row3 col3
                   TestUtil::kUnitramTypeTind, 0, 0, // type tind_addr_bus tind_out_bus
                   lt0, 4, 1, 0);                    // table0 ltcam4 vpn0 vpn1
    uint64_t result = UINT64_C(0x000000FF); // Set bot 8 bits (nxt_tab) to 0xFF
    // Fill all entries with default results - instr=0(0),nxt_tab=0xFF
    for (int i = 0; i < 1024; i++) tu.sram_write(0, 3, 3, i, result, result);
    // Set entry 253 to real results we want - instr=0x4(0x8 post add LSB 0),nxt_tab=7
    // Instr 0x4(0x8) translates to opindex=2,colour=0(,ingress=0)
    result = UINT64_C(0x0000DCBA00040005);
    tu.sram_write(0, 3, 3, 253, result, result);

    // TODO: add a packet that does not match DA_LO_32 = 22AABBCC and then
    //   trigger a different thing. Wildcard rule is at 505, so use Tind 252

    // Setup opindex=2 to load an immediate constant into PHV Phv::make_word(4,0)
    // (This is the DA hi 16 bit field and we set it to be 0xabcd)
    int val = 0xabcd;
    uint32_t instr_OLD = ((val & 0x00F000) << 5) | (8 << 12) | (val & 0xFFF);
    //uint32_t instr_NEW = ((val & (0x1F<<11)) << 5) | (8 << 11) | (val & (0x7FF<<0));
    uint32_t instr_NEWER = ((val & (0x3F<<10)) << 5) | (8 << 10) | (val & (0x3FF<<0));
    uint32_t instr_make = Instr::make_load_const(static_cast<uint32_t>(val));
    EXPECT_EQ(instr_NEWER, RM_B4_16(instr_OLD));
    uint32_t instr = instr_make;
    tu.imem_config(0, Phv::make_word_mapped(4,0), 2, 0, instr);



    // Construct a packet - DA,SA,SIP,DIP,Proto,SPort,DPort
    Packet *p_in = tu.packet_make("08:00:22:AA:BB:CC", "08:00:11:DD:EE:FF",
                                  "10.17.34.51", "10.68.85.102",
                                  TestUtil::kProtoTCP, 0x1188, 0x1199);
    // Get a port - one associated with pipe 0
    // This also sets up basic config ingress parser and deparser
    int port_num = 16;
    int port_pipe = Port::get_pipe_num(port_num);
    int ipb_num = Port::get_ipb_num(port_num);
    int prsr_num = Port::get_parser_num(port_num);
    assert( pipe == port_pipe ); // Sanity check
    Port *port = tu.port_get(port_num);

    // XXX disabling the ingress buffer logic - this is causing some problem
    // temp disable ingress buffer for this test
    Ipb *ib = om->ipb_lookup(pipe, ipb_num);
    ib->set_meta_enabled(false);
    ib->set_rx_enabled(true);
    Parser *prsr = om->parser_lookup(pipe, prsr_num)->ingress();
    prsr->set_hdr_len_adj(0);


    // Parse the packet to get Phv
    Phv *phv_in = tu.port_parse(port, p_in);
    RMT_UT_LOG_INFO("MauTest::InPkt=%p [DA=%04X%08X]\n", p_in,
           phv_in->get_p(Phv::make_word(4,0)), phv_in->get_p(Phv::make_word(0,0)));


    // Route DA_LO_32 (Phv::make_word(0,0) == phv word 0) to 4 ternary bytes numbered 134 and up
    input_xbar_util::set_32_bit_word_src(chip,pipe,0 /*stage*/,
                                         134 /* start output byte */,
                                         true /*enable*/,
                                         0 /*which_phv_word*/ );
    // set row vh xbar to grab the second group of 5 ternary bytes (134 and up) and put it on the search bus for the row
    tcam_row_vh_util::set_input_src_simple( chip, pipe,0 /*stage*/, 4 /*row*/, 0 /*bus*/ ,
                                            true  /* enable */,
                                            1 /*main_src*/,
                                            0 /*extra_byte_src*/,
                                            0 /*extra_nibble*/);

    // And write TCAM entries within TCAM(4,0)
    //
    // First off some random vals we hope don't match anything
    // Payload0 has to be 1 now to indicate a boundary
    tu.tcam_write_value_mask(0, 4, 0, 511,         // stage0 row4 col0 index511
                             0x9999, 0x9999, 1,0); // value mask payload0/1

    // Then a specific value/mask which we try and match against when we lookup(bv)
    // Payload0 has to be 1 now to indicate a boundary
    uint64_t value = UINT64_C(0xABCD0000);
    uint64_t mask = UINT64_C(0xFFFF0000);
    tu.tcam_write_value_mask(0, 4, 0, 510,       // stage0 row4 col0 index510
                             value, mask, 1,0);  // value mask payload0/1

    // Some vals we read/write to check data getting back and forth ok
    tu.tcam_write(0, 4, 0, 509,  // stage0 row4 col0 index509
                  0x19191919, 0x19191919);
    tu.tcam_write(0, 4, 0, 508,  // stage0 row4 col0 index508
                  0x28282828, 0x28282828);

    // this entry is set to match the DA_LO_32
    // Payload0 has to be 1 now to indicate a boundary
    tu.tcam_write_value_mask(0, 4, 0, 507,       // stage0 row4 col0 index506
                             0x22AABBCC, 0xFFFFFFFF, 1,0);  // value mask payload0/1

    // filler entry - not expected to match in this test
    tu.tcam_write(0, 4, 0, 506,  // stage0 row4 col0 index507
                  0xFFFFFFFF, 0xFFFFFFFF);

    // And a catch all entry which should match everything
    // Payload0 has to be 1 now to indicate a boundary
    tu.tcam_write_value_mask(0, 4, 0, 505,   // stage0 row4 col0 index511
                             0x0, 0x0, 1,0); // value mask payload0/1

    // Check the data got there OK - use tcam ref and fish out 508/509
    // Shift data we read right by 1 to get rid of payload bit
    Mau *mau = om->mau_lookup(0, 0);  // Lookup pipe0 stage0
    MauTcam *tcam = mau->tcam_lookup(4,0);

    uint64_t r0, r1;
    tcam->read(509, &r0, &r1, UINT64_C(0));
    EXPECT_EQ((r0 >> 1) & UINT64_C(0XFFFFFFFF), static_cast<uint64_t>(0x19191919));
    EXPECT_EQ((r1 >> 1) & UINT64_C(0XFFFFFFFF), static_cast<uint64_t>(0x19191919));
    tcam->read(508, &r0, &r1, UINT64_C(0));
    EXPECT_EQ((r0 >> 1) & UINT64_C(0XFFFFFFFF), static_cast<uint64_t>(0x28282828));
    EXPECT_EQ((r1 >> 1) & UINT64_C(0XFFFFFFFF), static_cast<uint64_t>(0x28282828));

    // Now check r1/r0 for entry 510, which we added as value/mask matches.
    //
    // We'll be reading direct from TCAM here which has a payload bit in LSB
    // so shift right by 1 again
    //int shift = TestUtil::kTcamPayloadBits;
    uint64_t r1_expected = (((value & mask) | ~mask) & UINT64_C(0xFFFFFFFFFFF));
    uint64_t r0_expected = (((~value & mask) | ~mask) & UINT64_C(0xFFFFFFFFFFF));
    tcam->read(510, &r0, &r1, UINT64_C(0));
    // r1 should be value & mask | ~mask
    EXPECT_EQ((r1 >> 1), r1_expected);
    RMT_UT_LOG_INFO("r1=0x%016" PRIx64 " expected=0x%016" PRIx64 "\n",
                r1, r1_expected);
    // r0 should be ~value & mask | ~mask
    EXPECT_EQ((r0 >> 1), r0_expected);
    RMT_UT_LOG_INFO("r0=0x%016" PRIx64 " expected=0x%016" PRIx64 "\n",
                r0, r0_expected);
    // Now do the lookups
    // This should match entry 510
    BitVector<44> bv(UINT64_C(0xABCD1234));
    int hitindex1 = tcam->lookup_index(bv);
    RMT_UT_LOG_INFO("BV lookup hitindex=%d\n", hitindex1);
    EXPECT_EQ(hitindex1, 510);
    // PHV lookup will match 507 (the 22AABBCC entry)
    int hitindex2 = tcam->lookup_index(phv_in);
    RMT_UT_LOG_INFO("PHV lookup hitindex=%d\n", hitindex2);
    EXPECT_EQ(hitindex2, 507);


    // Send packet. We should see:
    // 1. Logical table lookup 0
    // 2. Logical TCAM lookup 4
    // 3. TCAM lookup 4,0
    // Should get a wild-card match in TCAM and an instruction
    // overwriting the first 2 bytes of DA
    // Also attempt to snapshot capture modified word(4,0)

    // Set all words used for ingress to be an unconditional snapshot match
    for (stage = 0; stage < 2; stage++) {
      for (int w = 0; w < 32; w++) {
        tu.set_phv_match(stage, Phv::make_word(0,w), true, 0u, 0u);
        tu.set_phv_match(stage, Phv::make_word(2,w), true, 0u, 0u);
        tu.set_phv_match(stage, Phv::make_word(4,w), true, 0u, 0u);
      }
      for (int w = 0; w < 16; w++) {
        tu.set_phv_match(stage, Phv::make_word(5,w), true, 0u, 0u);
      }
    }
    int snap_state = 1; // Armed
    tu.setup_snapshot(0, true, snap_state); // Stage0 ingress snapshot armed
    tu.setup_snapshot(1, true, snap_state); // Stage1 ingress snapshot armed

    Packet *p_out = tu.port_process_inbound(port, p_in);
    // Parse the output packet to get output Phv
    Phv *phv_out = tu.port_parse(port, p_out);
    assert(phv_out);
    RMT_UT_LOG_INFO("MauTest::OutPkt=%p [DA=%04X%08X]\n", p_out,
           phv_out->get_p(Phv::make_word(4,0)), phv_out->get_p(Phv::make_word(0,0)));
    EXPECT_EQ(0xabcdu, phv_out->get_p(Phv::make_word(4,0)));

    // Check snapshot fired and saw modified word(4,0) with value 0xabcd
    int snap_word = Phv::make_word(4,0);
    snap_state = tu.setup_snapshot(0, true, -1);
    EXPECT_EQ(3, snap_state); // Should be full
    snap_state = tu.setup_snapshot(1, true, -1);
    EXPECT_EQ(3, snap_state); // Should be full
    uint32_t s0_snap40 = static_cast<uint32_t>(tu.get_phv_capture_word(0, snap_word) & 0xFFFF);
    uint32_t s1_snap40 = static_cast<uint32_t>(tu.get_phv_capture_word(1, snap_word) & 0xFFFF);
    EXPECT_EQ(0xabcdu, s1_snap40);
    EXPECT_EQ(0xabcdu, s0_snap40);

    // Free packet(s)
    if ((p_out != NULL) && (p_out != p_in)) tu.packet_free(p_out);
    tu.packet_free(p_in);


    // Schtumm
    tu.finish_test();
    tu.quieten_log_flags();
  }


  TEST(BFN_TEST_NAME(MauTest),WideTcamLookupPacket) {
    GLOBAL_MODEL->Reset();
    if (mau_print) RMT_UT_LOG_INFO("test_mau_wide_tcam_lookup_packet()\n");
    bool jbay = RmtObject::is_jbay_or_later();
    bool has_tagalong = !jbay;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    int lt0 = 0, lt5 = 5;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip

    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);

    // Addrs enabled by default - and MeterOP==NOP so relax check
    Address::kGlobalAddrEnable = true;
    MauLogicalTable::kRelaxHdrtimeMeterAddrNopCheck = true;
    // Uses PFE within LSB zero-padding - relax check
    MauLookupResult::kRelaxLookupShiftPfePosCheck = true;
    MauLookupResult::kRelaxLookupShiftOpPosCheck = true;


    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    tu.set_debug(false);
    tu.set_free_on_exit(true);
    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, FEW);

    // Switch on fatal/error/warn messages - happens by default these days
    //tu.update_log_flags(0, 0, 0, 0, 0, UINT64_C(0x7), ALL);

    // Switch on all parse loop output
    pipes = UINT64_C(1) << pipe;
    types = UINT64_C(1) << RmtTypes::kRmtTypeParser;
    flags = RmtDebug::kRmtDebugParserParseLoop;
    //tu.update_log_flags(pipes, 0, types, 0, 0, flags, ALL);

    // Switch *OFF* TCAM more_detail output
    pipes = UINT64_C(1) << pipe;
    types = UINT64_C(1) << RmtTypes::kRmtTypeMauTcam;
    flags = RmtDebug::kRmtDebugMauTcam_Tcam3LookupDetail|RmtDebug::kRmtDebugMauTcam_Tcam3DebugMiss;
    //tu.update_log_flags(pipes, 0, types, 0, 0, 0, ~flags);

    // this turns on debugging in the tcam row so you can see what the search bus ends up with
    // constexpr uint64_t one = 1;
    //tu.update_log_flags( pipes, one << 0, one << RmtTypes::kRmtTypeMauTcamRow,
    //                     one << lt0 /* rows */, 0 /* cols */, UINT64_C(0xff), ALL );
    // DEBUG setup done!!!!!!!!!!!!!!!!!!!!!!

    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);


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
    tu.table_config(0, lt0, true);    // stage0  table0  ingress
    tu.table_config(0, lt5, true);     // stage0 table5  ingress
    // Setup LT default regs
    tu.set_table_default_regs(0, lt0); // stage0 table0
    tu.set_table_default_regs(0, lt5); // stage0 table5


    // Setup this logical table 0 and physical result bus 14
    // to have certain shift/mask/dflt/miss vals
    //
    // Typically shift/mask/dflt configured on physical result bus
    // and miss configured on table
    //
    // NxtTab miss=5  Instr miss=0x6  Data miss=0xABABABAB
    // Instr miss will turn into 0xC once LSB set to ingress=0
    //   which translates to opindex=3,colour=0,ingress=0
    //
    // NB - the +5 is to shift off the 00000s tacked onto the LSB
    tu.physbus_config(0,    lt0, 14,       // stage0  table0  physResultBus14
                      0,    0xFF, 0, lt5,  // nxt_tab shft/mask/dflt/miss (shift ignored)
                      16,   0xFF, 0, 0x6,  // instr   shft/mask/dflt/miss (instrAddr = no pad)
                      32+5, 0xFFFFFFFF, 0, 0xABABABAB); // action_addr (inc 5 bits pad)

    // Now setup a single logical tcam (7) to refer to this table
    tu.ltcam_config(0, 7, lt0, 14, (uint8_t)4); // stage0 ltcam7 table0 physbus14 matchAddrShift

    // Setup stage default regs (need to do this after dependency
    // config so mpr_bus_dep can get setup properly);
    tu.set_stage_default_regs(0);
    tu.set_stage_default_regs(1);


    // Then setup some TCAMs within this logical TCAM (11,0) (10,0) (9,0)
    // chained together to do a wide match. Lowest TCAM in this case has output=true
    //
    // We use tcam_index in match_addr so set vpn to be same (15,14,13)
    tu.tcam_config(0, 11,  0,      // stage0 row11 col0,
                   0, lt0, 7, 15,  // inbus0 table0 ltcam7 vpn15
                   true, true, false, 0); // ingress chain output head
    tu.tcam_config(0, 10,  0,      // stage0 row10 col0,
                   0, lt0, 7, 14,  // inbus0 table0 ltcam7 vpn14
                   true, true, false, 0); // ingress chain output head
    tu.tcam_config(0, 9, 0,        // stage0 row9 col0,
                   0, lt0, 7, 13,  // inbus0 table0 ltcam7 vpn13
                   true, false, true, 0); // ingress chain output head

    tu.set_debug(false);
    //RMT_UT_LOG_INFO("MauTest::Adding TIND 7,0\n");
    // Then setup a TIND SRAM - has to be on row 7 to drive physical result bus 14
    tu.sram_config(0, 7, 2,                          // stage0 row7 col2
                   TestUtil::kUnitramTypeTind, 0, 0, // type tind_addr_bus tind_out_bus
                   lt0, 7, 6, 0);                    // table0 ltcam7 vpn0=6 vpn1=0
    uint64_t result = UINT64_C(0x000000FF); // Set bot 8 bits (nxt_tab) to 0xFF
    // Fill all entries with default results - instr=0(0),nxt_tab=0xFF
    for (int i = 0; i < 1024; i++) tu.sram_write(0, 7, 2, i, result, result);
    // Set entry 222 to real results we want - instr=0x4(0x8 post add LSB 0),nxt_tab=16
    // Instr 0x4(0x8) translates to opindex=2,colour=0(ingress=0)
    result = UINT64_C(0x0000DCBA00040010);
    tu.sram_write(0, 7, 2, 222, result, result);



    // Setup opindex=2 to load an immediate constant into PHV Phv::make_word(4,0)
    // (This is the DA hi 16 bit field and we set it to be 0xFEDA)
    int val = 0xFEDA;
    //uint32_t instr = ((val & (0x1F<<11)) << 5) | (8 << 11) | (val & (0x7FF<<0));
    uint32_t instr_make = Instr::make_load_const(static_cast<uint32_t>(val));
    tu.imem_config(0, Phv::make_word_mapped(4,0), 2, 0, instr_make);

    // And write zero TCAM entries within TCAMs (15,0) (14,0) (13,0)
    for (int i = 0; i < TestUtil::kTcamMaxEntries; i++) {
      tu.tcam_zero(0, 11, 0, i); // stage0 row11 col0 index=i
      tu.tcam_zero(0, 10, 0, i); // stage0 row10 col0 index=i
      tu.tcam_zero(0,  9, 0, i); // stage0 row9  col0 index=i
    }
    // Now write a pattern of wildcards into our TCAMs
    // TCAM(11,0) has wildcard at 464,454,444,434,424,414
    // TCAM(10,0) has wildcard at     454,444,434,424
    // TCAM( 9,0) has wildcard at         444,434
    // Only common match is 444 so we should get wide hit at 444
    tu.tcam_write_value_mask(0, 11, 0, 464, 0x0, 0x0, 0,0); // stage0 row11 col0 index464
    tu.tcam_write_value_mask(0, 11, 0, 454, 0x0, 0x0, 0,0); // stage0 row11 col0 index454
    tu.tcam_write_value_mask(0, 11, 0, 444, 0x0, 0x0, 0,0); // stage0 row11 col0 index444
    tu.tcam_write_value_mask(0, 11, 0, 434, 0x0, 0x0, 0,0); // stage0 row11 col0 index434
    tu.tcam_write_value_mask(0, 11, 0, 424, 0x0, 0x0, 0,0); // stage0 row11 col0 index424
    tu.tcam_write_value_mask(0, 11, 0, 414, 0x0, 0x0, 0,0); // stage0 row11 col0 index414
    tu.tcam_write_value_mask(0, 10, 0, 454, 0x0, 0x0, 0,0); // stage0 row10 col0 index454
    tu.tcam_write_value_mask(0, 10, 0, 444, 0x0, 0x0, 0,0); // stage0 row10 col0 index444
    tu.tcam_write_value_mask(0, 10, 0, 434, 0x0, 0x0, 0,0); // stage0 row10 col0 index434
    tu.tcam_write_value_mask(0, 10, 0, 424, 0x0, 0x0, 0,0); // stage0 row10 col0 index424
    tu.tcam_write_value_mask(0,  9, 0, 444, 0x0, 0x0, 0,0); // stage0 row 9 col0 index444
    tu.tcam_write_value_mask(0,  9, 0, 434, 0x0, 0x0, 0,0); // stage0 row 9 col0 index434

    // Also write a match entry into TCAM(11,0) index=222 with wildcards in TCAM(10,0)
    // and TCAM(9,0) - should match DA_LO_32
    // Payload0 has to be 1 now to indicate a boundary
    tu.tcam_write_value_mask(0, 11, 0, 222,   // stage0 row11 col0 index222
                             0x22AABBCC, 0xFFFFFFFF, 1,0);  // value mask payload0/1
    tu.tcam_write_value_mask(0, 10, 0, 222,   // stage0 row10 col0 index222
                             0x0, 0x0, 1,0);  // value mask payload0/1
    tu.tcam_write_value_mask(0,  9, 0, 222,   // stage0 row9  col0 index222
                             0x0, 0x0, 1,0);  // value mask payload0/1


    // Route DA_LO_32 (Phv::make_word(0,0) == phv word 0) to 4 ternary bytes
    // numbered 134 and up
    input_xbar_util::set_32_bit_word_src(chip,pipe,0 /*stage*/,
                                         134 /* start output byte */,
                                         true /*enable*/,
                                         0 /*which_phv_word*/ );
    // set row vh xbar to grab the second group of 5 ternary bytes (134 and up)
    // and put it on the search bus for the row
    tcam_row_vh_util::set_input_src_simple( chip, pipe,0 /*stage*/, 4 /*row*/, 0 /*bus*/ ,
                                            true  /* enable */,
                                            1 /*main_src*/,
                                            0 /*extra_byte_src*/,
                                            0 /*extra_nibble*/);




    // TODO: add a packet that does not match DA_LO_32 = 22AABBCC

    // Configure learning, at the moment the parameters here don't
    //  make much difference as most of the stuff is hard coded in learn_config
    // TODO: update when learn_config() is updated
    int learn_phvs[48];
    tu.learn_config(0,    // pipe
                    true, // valid
                    // phv currently makes no difference as all entries programmed the same
                    //  but must be valid
                    Phv::make_word(4,0),    // phv_for_table_index
                    // these parameters currently ignored, all entries are
                    //  programmed to pick up the SA
                    0, // table_entry_index
                    0, // length
                    learn_phvs);


    // Switch off debug
    tu.set_debug(false);
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, NON, NON);
    tu.update_log_flags(pipes, stages, ONE<<53, rows_tabs, cols, HI, HI);

    tu.zero_dru_learn_callback_count();
    // Init learning filter to send notification for each new mac
    // this is done, by setting the timeout register value to 0
    // model code pushed the LQ immidiatly when timeout is 0
    // also clear the buffer right-away
    tu.lfltr_config(pipe, false, 1);
    if (!jbay) {
      EXPECT_EQ(0u, tu.deparser_get_learn_counter(pipe));  // sanity check
    }
    // Loop a few times so we can look for memory leaks etc
    for (int loop = 0; loop < 200; loop++) {

      printf("."); fflush(stdout);
      if ((loop % 100) == 0) {
        // Periodically dump out RmtObjectManager stats so we can see
        // if we're leaking any Packets/Phvs
        printf("\n");
        om->dump_stats();
      }

      // Construct a packet - DA,SA,SIP,DIP,Proto,SPort,DPort
      Packet *p_in = tu.packet_make("08:00:22:AA:BB:CC", "08:00:11:DD:EE:FF",
                                    "10.17.34.51", "10.68.85.102",
                                    TestUtil::kProtoTCP, 0x1188, 0x1199);
      // Get a port - one associated with pipe 0
      // This also sets up basic config ingress parser and deparser
      int port_num = 16;
      int port_pipe = Port::get_pipe_num(port_num);
      int ipb_num = Port::get_ipb_num(port_num);
      int prsr_num = Port::get_parser_num(port_num);
      assert( pipe == port_pipe ); // Sanity check
      Port *port = tu.port_get(port_num);

      // XXX disabling the ingress buffer logic - this is causing some problem
      // temp disable ingress buffer for this test
      Ipb *ib = om->ipb_lookup(pipe, ipb_num);
      ib->set_meta_enabled(false);
      ib->set_rx_enabled(true);
      Parser *prsr = om->parser_lookup(pipe, prsr_num)->ingress();
      prsr->set_hdr_len_adj(0);

      // Parse the packet to get Phv
      Phv *phv_in = tu.port_parse(port, p_in);
      EXPECT_LT(0, p_in->orig_hdr_len());
      RMT_UT_LOG_INFO("MauTest::InPkt=%p [DA=%04X%08X]\n", p_in,
                  phv_in->get_p(Phv::make_word(4,0)), phv_in->get_p(Phv::make_word(0,0)));



      // Send packet. We should see:
      // 1. Logical table lookup 0
      // 2. Logical TCAM lookup 7
      // 3. TCAM lookup (11,0) (10,0) (9,0)
      //    NB. This SHOULD iterate because:
      // TRY   a. Will hit at (11,0,464) but miss on (10,0,464)
      // RETRY b. Will hit at (11,0,454) then hit on (10,0,454) but miss on (9,0,454)
      // RETRY c. Will hit at (11,0,444) then hit on (10,0,444) then hit on (9,0,444)
      //
      // So SHOULD finally get a wild-card match at TCAM index 444
      // Then we should get a TIND lookup at index 222
      // Where we should find an instruction that overwrites the first 2 bytes of DA
      //
      Packet *p_out = tu.port_process_inbound(port, p_in);

      // Perform same process but using PHVs not Packets
      // Also add some tagalong PHV words and check they're preserved
      if (has_tagalong) {
        phv_in->set_x(Phv::make_word(8,8), 0x1234ABCD);
        phv_in->set_x(Phv::make_word(9,9), 0xCD);
        phv_in->set_x(Phv::make_word(10,10), 0xABCD);
        phv_in->set_x(Phv::make_word(11,11), 0x5678);
      }
      Phv *phv_out = tu.port_process_inbound(port, phv_in);
      if (has_tagalong) {
        EXPECT_EQ(0x1234ABCDu, phv_out->get_x(Phv::make_word(8,8)));
        EXPECT_EQ(0xCDu, phv_out->get_x(Phv::make_word(9,9)));
        EXPECT_EQ(0xABCDu, phv_out->get_x(Phv::make_word(10,10)));
        EXPECT_EQ(0x5678u, phv_out->get_x(Phv::make_word(11,11)));
        //printf("Phv::make_word(11,11) = 0x%08x\n", phv_out->get_x(Phv::make_word(11,11)));
      }
      //phv_out->print("phv_out", true);


      RMT_UT_LOG_INFO("MauTest::OutPkt=%p [DA=%04X%08X]\n", p_out,
                  phv_out->get_p(Phv::make_word(4,0)), phv_out->get_p(Phv::make_word(0,0)));
      //EXPECT_EQ(0xFEDAu, phv_out->get_p(Phv::make_word(4,0)));


      // Free packet(s) and Phv(s)
      if ((p_out != NULL) && (p_out != p_in)) tu.packet_free(p_out);
      tu.packet_free(p_in);
      if ((phv_out != NULL) && (phv_out != phv_in)) tu.phv_free(phv_out);
      tu.phv_free(phv_in);

    }
    // With bloom filter logic implemented.. there is only one callback as there is only 1
    // unique LQ and push timer is set to 1
    if (!jbay) {
      EXPECT_EQ(1, tu.dru_learn_callback_count() );
      EXPECT_EQ(1u, tu.deparser_get_learn_counter(pipe));
    }

    printf("\n");
    om->dump_stats();

    // Schtumm
    tu.finish_test();
    tu.quieten_log_flags();
  }



  TEST(BFN_TEST_NAME(MauTest),WideTcamLookupPacketCrossingMiddle) {
    GLOBAL_MODEL->Reset();

    if (mau_print) RMT_UT_LOG_INFO("test_mau_wide_tcam_lookup_packet_crossing_middle()\n");
    bool jbay = RmtObject::is_jbay_or_later();
    bool has_tagalong = !jbay;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    int lt0 = 0, lt5 = 5;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);

    // Addrs enabled by default - and MeterOP==NOP so relax check
    Address::kGlobalAddrEnable = true;
    MauLogicalTable::kRelaxHdrtimeMeterAddrNopCheck = true;
    // Relax MauLookupResult pfe_pos check
    MauLookupResult::kRelaxLookupShiftPfePosCheck = true;
    MauLookupResult::kRelaxLookupShiftOpPosCheck = true;


    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    tu.set_debug(false);
    tu.set_free_on_exit(true);
    // Just to stop compiler complaining about unused vars
    //flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Switch on fatal/error/warn messages - happens by default these days
    //tu.update_log_flags(0, 0, 0, 0, 0, UINT64_C(0x7), ALL);

    // Switch on all parse loop output
    pipes = UINT64_C(1) << pipe;
    types = UINT64_C(1) << RmtTypes::kRmtTypeParser;
    flags = RmtDebug::kRmtDebugParserParseLoop;
    //tu.update_log_flags(pipes, 0, types, 0, 0, flags, ALL);

    // Switch *OFF* TCAM more_detail output
    pipes = UINT64_C(1) << pipe;
    types = UINT64_C(1) << RmtTypes::kRmtTypeMauTcam;
    flags = RmtDebug::kRmtDebugMauTcam_Tcam3LookupDetail|RmtDebug::kRmtDebugMauTcam_Tcam3DebugMiss;
    //tu.update_log_flags(pipes, 0, types, 0, 0, 0, ~flags);

    // this turns on debugging in the tcam row so you can see what the search bus ends up with
    // constexpr uint64_t one = 1;
    //tu.update_log_flags( pipes, one << 0, one << RmtTypes::kRmtTypeMauTcamRow,
    //                     one << lt0 /* rows */, 0 /* cols */, UINT64_C(0xff), ALL );
    // DEBUG setup done!!!!!!!!!!!!!!!!!!!!!!

    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);


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
    tu.table_config(0, lt0, true);    // stage0  table0  ingress
    tu.table_config(0, lt5, true);     // stage0 table5  ingress
    // Setup LT default regs
    tu.set_table_default_regs(0, lt0); // stage0 table0
    tu.set_table_default_regs(0, lt5); // stage0 table5


    // Setup this logical table 15 and physical result bus 14
    // to have certain shift/mask/dflt/miss vals
    //
    // Typically shift/mask/dflt configured on physical result bus
    // and miss configured on table
    //
    // NxtTab miss=5  Instr miss=0x6  Data miss=0xABABABAB
    // Instr miss will turn into 0xC once LSB set to ingress=0
    //   which translates to opindex=3,colour=0,ingress=0
    //
    // NB - the +5 is to shift off the 00000s tacked onto the LSB
    tu.physbus_config(0,    lt0, 14,       // stage0  table0  physResultBus14
                      0,    0xFF, 0, lt5,  // nxt_tab shft/mask/dflt/miss (shift ignored)
                      16,   0xFF, 0, 0x6,  // instr   shft/mask/dflt/miss (instrAddr = no pad)
                      32+5, 0xFFFFFFFF, 0, 0xABABABAB); // action_addr (inc 5 bits pad)

    // Now setup a single logical tcam (7) to refer to this table
    tu.ltcam_config(0, 7, lt0, 14, (uint8_t)4); // stage0 ltcam7 table0 physbus14 matchAddrShift

    // Setup stage default regs (need to do this after dependency
    // config so mpr_bus_dep can get setup properly);
    tu.set_stage_default_regs(0);
    tu.set_stage_default_regs(1);


    // Then setup some TCAMs within this logical TCAM (11,0) (10,0) (9,0)
    // chained together to do a wide match. Lowest TCAM in this case has output=true
    //
    // Set all vpns to be same (13)
    tu.tcam_config(0,   7, 0,      // stage0 row7 col0,
                   0, lt0, 7, 13,  // inbus0 table0 ltcam7 vpn13
                   true, true, false, 0); // ingress chain output head
    tu.tcam_config(0,   6, 0,      // stage0 row6 col0,
                   0, lt0, 7, 13,  // inbus0 table0 ltcam7 vpn13
                   true, true, true, 0); // ingress chain output head
    tu.tcam_config(0,   5, 0,      // stage0 row5 col0,
                   0, lt0, 7, 13,  // inbus0 table0 ltcam7 vpn13
                   true, true, true, 0); // ingress chain output head
    tu.tcam_config(0,   4, 0,      // stage0 row5 col0,
                   0, lt0, 7, 13,  // inbus0 table0 ltcam7 vpn13
                   true, true, false, 0); // ingress chain output head

    tu.set_debug(false);
    // Then setup a TIND SRAM - has to be on row 7 to drive physical result bus 14
    tu.sram_config(0, 7, 2,                          // stage0 row7 col2
                   TestUtil::kUnitramTypeTind, 0, 0, // type tind_addr_bus tind_out_bus
                   lt0, 7, 6, 0);                    // table0 ltcam7 vpn0=6 vpn1=0
    uint64_t result = UINT64_C(0x000000FF); // Set bot 8 bits (nxt_tab) to 0xFF
    // Fill all entries with default results - instr=0(0),nxt_tab=0xFF
    for (int i = 0; i < 1024; i++) tu.sram_write(0, 7, 2, i, result, result);
    // Set entry 222 to real results we want - instr=0x4(0x8 post add LSB 0),nxt_tab=16
    // Instr 0x4(0x8) translates to opindex=2,colour=0(ingress=0)
    result = UINT64_C(0x0000DCBA00040010);
    tu.sram_write(0, 7, 2, 222, result, result);


    // Setup opindex=2 to load an immediate constant into PHV Phv::make_word(4,0)
    // (This is the DA hi 16 bit field and we set it to be 0xFEDA)
    int val = 0xFEDA;
    //uint32_t instr = ((val & (0x1F<<11)) << 5) | (8 << 11) | (val & (0x7FF<<0));
    uint32_t instr_make = Instr::make_load_const(static_cast<uint32_t>(val));
    tu.imem_config(0, Phv::make_word(4,0), 2, 0, instr_make);

    // And write zero TCAM entries within TCAMs (7,0) (6,0) (5,0) (4,0)
    for (int i = 0; i < TestUtil::kTcamMaxEntries; i++) {
      tu.tcam_zero(0, 7, 0, i); // stage0 row7 col0 index=i
      tu.tcam_zero(0, 6, 0, i); // stage0 row6 col0 index=i
      tu.tcam_zero(0, 5, 0, i); // stage0 row5 col0 index=i
      tu.tcam_zero(0, 4, 0, i); // stage0 row4 col0 index=i
    }

    // Now write a pattern of wildcards into our TCAMs
    // TCAM(5,0) has wildcard match hit at 444
    tu.tcam_write_value_mask(0, 5, 0, 444, 0x0, 0x0, 1,0); // stage0 row5 col0 index444
    // TCAM(4,0) has wildcard match hit at 444
    tu.tcam_write_value_mask(0, 4, 0, 444, 0x0, 0x0, 1,0); // stage0 row5 col0 index434
    // So at end of bottom TCAMs hit is 444

    // TCAM(7,0) has NO HIT at 443 but a wildcard match hit at 442.
    // In TCAM(7,0) 442 has no boundary set so hit 'spreads' to 443.
    tu.tcam_write           (0, 7, 0, 443, 0x0, 0x0, 1,0); // stage0 row5 col0 index444 - MISS
    tu.tcam_write_value_mask(0, 7, 0, 442, 0x0, 0x0, 0,0); // stage0 row5 col0 index434
    // TCAM(6,0) has NO HIT at 444 but a wildcard match hit at 443.
    // In TCAM(6,0) 443 has no boundary bit set so hit 'spreads' to 444.
    tu.tcam_write           (0, 6, 0, 444, 0x0, 0x0, 1,0); // stage0 row5 col0 index444 - MISS
    tu.tcam_write_value_mask(0, 6, 0, 443, 0x0, 0x0, 0,0); // stage0 row5 col0 index434
    // At end of top TCAMs hit is also 444
    // Therefore overall wide hit should be 444.


    //tu.tcam_write_value_mask(0, 7, 0, 464, 0x0, 0x0, 1,0); // stage0 row7 col0 index464
    //tu.tcam_write_value_mask(0, 7, 0, 454, 0x0, 0x0, 1,0); // stage0 row7 col0 index454
    //tu.tcam_write_value_mask(0, 7, 0, 444, 0x0, 0x0, 1,0); // stage0 row7 col0 index444
    //tu.tcam_write_value_mask(0, 7, 0, 434, 0x0, 0x0, 1,0); // stage0 row7 col0 index434
    //tu.tcam_write_value_mask(0, 7, 0, 424, 0x0, 0x0, 1,0); // stage0 row7 col0 index424
    //tu.tcam_write_value_mask(0, 7, 0, 414, 0x0, 0x0, 1,0); // stage0 row7 col0 index414
    //tu.tcam_write_value_mask(0, 6, 0, 454, 0x0, 0x0, 1,0); // stage0 row6 col0 index454
    //tu.tcam_write_value_mask(0, 6, 0, 444, 0x0, 0x0, 1,0); // stage0 row6 col0 index444
    //tu.tcam_write_value_mask(0, 6, 0, 434, 0x0, 0x0, 1,0); // stage0 row6 col0 index434
    //tu.tcam_write_value_mask(0, 6, 0, 424, 0x0, 0x0, 1,0); // stage0 row6 col0 index424



    // Switch off debug
    tu.set_debug(false);
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, NON, NON);
    tu.update_log_flags(pipes, stages, ONE<<53, rows_tabs, cols, HI, HI);
    flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


    // Loop a few times
    for (int loop = 0; loop < 1; loop++) {

      printf("."); fflush(stdout);
      if ((loop % 100) == 0) {
        // Periodically dump out RmtObjectManager stats so we can see
        // if we're leaking any Packets/Phvs
        printf("\n");
        om->dump_stats();
      }
      // Construct a packet - DA,SA,SIP,DIP,Proto,SPort,DPort
      Packet *p_in = tu.packet_make("08:00:22:AA:BB:CC", "08:00:11:DD:EE:FF",
                                    "10.17.34.51", "10.68.85.102",
                                    TestUtil::kProtoTCP, 0x1188, 0x1199);

      // Get a port - one associated with pipe 0
      // This also sets up basic config ingress parser and deparser
      int port_num = 16;
      int port_pipe = Port::get_pipe_num(port_num);
      int ipb_num = Port::get_ipb_num(port_num);
      int prsr_num = Port::get_parser_num(port_num);
      assert( pipe == port_pipe ); // Sanity check
      Port *port = tu.port_get(port_num);

      // XXX disabling the ingress buffer logic - this is causing some problem
      // temp disable ingress buffer for this test
      Ipb *ib = om->ipb_lookup(pipe, ipb_num);
      ib->set_meta_enabled(false);
      ib->set_rx_enabled(true);
      Parser *prsr = om->parser_lookup(pipe, prsr_num)->ingress();
      prsr->set_hdr_len_adj(0);


      // Parse the packet to get Phv
      Phv *phv_in = tu.port_parse(port, p_in);
      EXPECT_LT(0, p_in->orig_hdr_len());
      RMT_UT_LOG_INFO("MauTest::InPkt=%p [DA=%04X%08X]\n", p_in,
                  phv_in->get_p(Phv::make_word(4,0)), phv_in->get_p(Phv::make_word(0,0)));


      // Send packet. We should see:
      // 1. Logical table lookup 15
      // 2. Logical TCAM lookup 7
      // 3. TCAM lookup (11,0) (10,0) (9,0)
      //    NB. This SHOULD iterate because:
      // TRY   a. Will hit at (11,0,464) but miss on (10,0,464)
      // RETRY b. Will hit at (11,0,454) then hit on (10,0,454) but miss on (9,0,454)
      // RETRY c. Will hit at (11,0,444) then hit on (10,0,444) then hit on (9,0,444)
      //
      // So SHOULD finally get a wild-card match at TCAM index 444
      // Then we should get a TIND lookup at index 222
      // Where we should find an instruction that overwrites the first 2 bytes of DA
      //
      Packet *p_out = NULL; // tu.port_process_inbound(port, p_in);

      // Perform same process but using PHVs not Packets
      // Also add some tagalong PHV words and check they're preserved
      if (has_tagalong) {
        phv_in->set_x(Phv::make_word(8,8), 0x1234ABCDu);
        phv_in->set_x(Phv::make_word(9,9), 0xCDu);
        phv_in->set_x(Phv::make_word(10,10), 0xABCDu);
        phv_in->set_x(Phv::make_word(11,11), 0x5678u);
      }
      Phv *phv_out = tu.port_process_inbound(port, phv_in);
      if (has_tagalong) {
        EXPECT_EQ(0x1234ABCDu, phv_out->get_x(Phv::make_word(8,8)));
        EXPECT_EQ(0xCDu, phv_out->get_x(Phv::make_word(9,9)));
        EXPECT_EQ(0xABCDu, phv_out->get_x(Phv::make_word(10,10)));
        EXPECT_EQ(0x5678u, phv_out->get_x(Phv::make_word(11,11)));
        //printf("Phv::make_word(11,11) = 0x%08x\n", phv_out->get_x(Phv::make_word(11,11)));
      }
      //phv_out->print("phv_out", true);


      RMT_UT_LOG_INFO("MauTest::OutPkt=%p [DA=%04X%08X]\n", p_out,
                  phv_out->get_p(Phv::make_word(4,0)), phv_out->get_p(Phv::make_word(0,0)));
      //EXPECT_EQ(0xFEDAu, phv_out->get_p(Phv::make_word(4,0)));


      // Free packet(s) and Phv(s)
      if ((p_out != NULL) && (p_out != p_in)) tu.packet_free(p_out);
      tu.packet_free(p_in);
      if ((phv_out != NULL) && (phv_out != phv_in)) tu.phv_free(phv_out);
      tu.phv_free(phv_in);

    }

    printf("\n");
    om->dump_stats();

    // Schtumm
    tu.finish_test();
    tu.quieten_log_flags();
  }



  TEST(BFN_TEST_NAME(MauTest),AddrCalc) {
    GLOBAL_MODEL->Reset();
    if (mau_print) RMT_UT_LOG_INFO("test_mau_addr_calc()\n");
    int chip = 202;
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

    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    int pipe_last = RmtDefs::map_mau_pipe(RmtDefs::kPipesMax-1,0xFF);
    int stage_last = RmtDefs::kStagesMax-1;
    for (int p = 0; p <= pipe_last; p++) {
      for (int s = 0; s <= stage_last; s++) {
        for (int r = 0; r < 8; r++) {
          for (int c = 2; c < 12; c++) {
            for (int i = 0; i <= 999; i += 111) {
              for (int typ = 0; typ < 5; typ++) {

                // Work out addr by hand - the 2<<30 means PhysMem
                uint64_t local_addr = UINT64_C(0x20000000000);
                local_addr |= static_cast<uint64_t>(p) << kIndPipeShift;
                local_addr |= static_cast<uint64_t>(s) << kIndStageShift;
                local_addr |= static_cast<uint64_t>(2) << 30;
                local_addr |= (typ << 18) | (r << 14) | (c << 10) | i;

                // Call model code to figure out addr
                uint64_t model_addr = MauMemory::make_phys_address(p,s,typ,r,c,i);

                // Call TestUtil code to figure out addr
                // t_addr gets some basic error checking so
                // don't bother with comparision if val returned is -1
                uint64_t tu_addr = TestUtil::make_physical_address(p,s,typ,r,c,i);
                uint64_t tu_addr2 = UINT64_C(0xFFFFFFFFFFFFFFFF);

                if (tu_addr != UINT64_C(0xFFFFFFFFFFFFFFFF)) {
                  // Call type-specific TestUtil funcs too
                  switch (typ) {
                    case 0: tu_addr2 = tu.make_sram_addr(p,s,r,c,i);   break;
                    case 1: tu_addr2 = tu.make_mapram_addr(p,s,r,c,i); break;
                    case 4: tu_addr2 = tu.make_tcam_addr(p,s,r,c,i);   break;
                  }
                  if (tu_addr2 != UINT64_C(0xFFFFFFFFFFFFFFFF)) {
                    EXPECT_EQ(tu_addr,tu_addr2);
                    EXPECT_EQ(local_addr,tu_addr2);
                  }
                  EXPECT_EQ(local_addr,tu_addr);
                }
                EXPECT_EQ(local_addr,model_addr);
                // Dump out vars
                if (false && (p == 0) && (s == 0)) {
                  RMT_UT_LOG_INFO("LocalAddr=%012" PRIx64 "\n", local_addr);
                  RMT_UT_LOG_INFO("ModelAddr=%012" PRIx64 "\n", model_addr);
                  if (tu_addr != UINT64_C(0xFFFFFFFFFFFFFFFF))
                    RMT_UT_LOG_INFO(" TestAddr=%012" PRIx64 "\n", tu_addr);
                  if (tu_addr2 != UINT64_C(0xFFFFFFFFFFFFFFFF))
                    RMT_UT_LOG_INFO("TestAddr2=%012" PRIx64 "\n", tu_addr2);
                }
              }
            }
          }
        }
      }
    }
    // Now virtual
    for (int p = 0; p <= pipe_last; p++) {
      for (int s = 0; s <= stage_last; s++) {
        for (int lt = 0; lt < 16; lt++) {
          for (int addr = 0; addr <= 999999; addr += 111111) {
            for (int typ = 0; typ < 4; typ++) {

              // Work out addr by hand - the 3<<30 means VirtMem
              uint64_t local_addr = UINT64_C(0x20000000000);
              local_addr |= static_cast<uint64_t>(p) << kIndPipeShift;
              local_addr |= static_cast<uint64_t>(s) << kIndStageShift;
              local_addr |= static_cast<uint64_t>(3) << 30;
              local_addr |= (typ << 25) | (lt << 21) | addr;

              // Call model code to figure out addr
              uint64_t model_addr = MauMemory::make_virt_address(p,s,typ,lt,addr);

              // Call TestUtil code to figure out addr
              // tu_addr gets some basic error checking so
              // don't bother with comparision if val returned is -1
              uint64_t tu_addr = TestUtil::make_virtual_address(p,s,typ,lt,addr);

              if (tu_addr != UINT64_C(0xFFFFFFFFFFFFFFFF)) {
                EXPECT_EQ(local_addr,tu_addr);
              }
              EXPECT_EQ(local_addr,model_addr);
            }
          }
        }
      }
    }
    tu.finish_test();
    tu.quieten_log_flags();
  }


  TEST(BFN_TEST_NAME(MauTest),TriggerAssert) {
    if (mau_print) RMT_UT_LOG_INFO("test_mau_trigger_assert()\n");

    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    int lt0 = 0, lt5 = 5;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_free_on_exit(true);
    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    GLOBAL_THROW_ON_ERROR = 1;
    Address::kGlobalAddrEnable = true;
    // Uses PFE within LSB zero-padding - relax check
    // Don't relax any more - use thrown error instead of assert
    MauLookupResult::kRelaxLookupShiftPfePosCheck = false;


    // Instantiate whole chip and fish out objmgr
    //tu.chip_init_all();
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

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
    //
    tu.table_config(0, lt0, true);     // stage0 table0  ingress
    tu.table_config(0, lt5, true);     // stage0 table5  ingress
    // Setup LT default regs
    tu.set_table_default_regs(0, lt0); // stage0 table0
    tu.set_table_default_regs(0, lt5); // stage0 table5


    // Setup this logical table 4 and physical result bus 6
    // to have certan shift/mask/dflt/miss vals
    //
    // Typically shift/mask/dflt configured on physical result bus
    // and miss configured on table
    //
    // NxtTab miss=5  Instr miss=0x6  Data miss=0xABABABAB
    // Instr miss will turn into 0xC once LSB set to ingress=0
    //   which translates to opindex=3,colour=0,ingress=0
    //
    // NB - the +5 is to shift off the 00000s tacked onto the LSB
    tu.physbus_config(0,    lt0,  6,       // stage0  table0  physResultBus6
                      0,    0xFF, 0, lt5,  // nxt_tab shft/mask/dflt/miss (shift ignored)
                      16,   0xFF, 0, 0x6,  // instr   shft/mask/dflt/miss (no pad for instr addr)
                      32+5, 0xFFFFFFFF, 0, 0xABABABAB); // action_addr

    // Now setup a single logical tcam (4) to refer to this table 0
    tu.ltcam_config(0, 4, lt0, 4, (uint8_t)4); // stage0 ltcam4 table0 physbus4 matchAddrShift

    // Setup stage default regs (need to do this after dependency
    // config so mpr_bus_dep can get setup properly);
    tu.set_stage_default_regs(0);
    tu.set_stage_default_regs(1);

    // Then setup a TCAM(4,0) within this logical TCAM
    // We use vpn now in match_addr NOT tcam_index so for now set vpn to be same (4)
    tu.tcam_config(0, 4,   0,       // stage0 row4 col0,
                   0, lt0, 4, 4,    // inbus0 table0 ltcam4 vpn4
                   true, false, true, 1); // ingress chain output head

    tu.set_debug(false);
    //RMT_UT_LOG_INFO("MauTest::Adding TIND 3,3\n");
    // Then setup a TIND SRAM - has to be on row 3 to drive physical result bus 6
    tu.sram_config(0, 3, 3,                          // stage0 row3 col3
                   TestUtil::kUnitramTypeTind, 0, 0, // type tind_addr_bus tind_out_bus
                   lt0, 4, 1, 0);                    // table0 ltcam4 vpn0 vpn1
    uint64_t result = UINT64_C(0x000000FF); // Set bot 8 bits (nxt_tab) to 0xFF
    // Fill all entries with default results - instr=0(0),nxt_tab=0xFF
    for (int i = 0; i < 1024; i++) tu.sram_write(0, 3, 3, i, result, result);
    // Set entry 253 to real results we want - instr=0x4(0x8 post add LSB 0),nxt_tab=7
    // Instr 0x4(0x8) translates to opindex=2,colour=0(,ingress=0)
    result = UINT64_C(0x0000DCBA00040005);
    tu.sram_write(0, 3, 3, 253, result, result);


    // Setup a broken OP - should trigger an assert when we do lookup
    // NB. No longer asserts since regs_29733_mau_dev
    // Make do with a thrown error caused by bad ShiftPfePos
    uint32_t instr = 0x6BE8F1C;
    tu.imem_config(0, Phv::make_word(4,0), 2, 0, instr);



    // Construct a packet - DA,SA,SIP,DIP,Proto,SPort,DPort
    Packet *p_in = tu.packet_make("08:00:22:AA:BB:CC", "08:00:11:DD:EE:FF",
                                  "10.17.34.51", "10.68.85.102",
                                  TestUtil::kProtoTCP, 0x1188, 0x1199);
    // Get a port - one associated with pipe 0
    // This also sets up basic config ingress parser and deparser
    int port_num = 16;
    int port_pipe = Port::get_pipe_num(port_num);
    int ipb_num = Port::get_ipb_num(port_num);
    int prsr_num = Port::get_parser_num(port_num);
    assert( pipe == port_pipe ); // Sanity check
    Port *port = tu.port_get(port_num);

    // XXX disabling the ingress buffer logic - this is causing some problem
    // temp disable ingress buffer for this test
    Ipb *ib = om->ipb_lookup(pipe, ipb_num);
    ib->set_meta_enabled(false);
    ib->set_rx_enabled(true);
    Parser *prsr = om->parser_lookup(pipe, prsr_num)->ingress();
    prsr->set_hdr_len_adj(0);

    // Parse the packet to get Phv
    Phv *phv_in = tu.port_parse(port, p_in);
    RMT_UT_LOG_INFO("MauTest::InPkt=%p [DA=%04X%08X]\n", p_in,
           phv_in->get_p(Phv::make_word(4,0)), phv_in->get_p(Phv::make_word(0,0)));


    // Route DA_LO_32 (Phv::make_word(0,0) == phv word 0) to 4 ternary bytes numbered 134 and up
    input_xbar_util::set_32_bit_word_src(chip,pipe,0 /*stage*/,
                                         134 /* start output byte */,
                                         true /*enable*/,
                                         0 /*which_phv_word*/ );
    // set row vh xbar to grab the second group of 5 ternary bytes (134 and up) and put it on the search bus for the row
    tcam_row_vh_util::set_input_src_simple( chip, pipe,0 /*stage*/, 4 /*row*/, 0 /*bus*/ ,
                                            true  /* enable */,
                                            1 /*main_src*/,
                                            0 /*extra_byte_src*/,
                                            0 /*extra_nibble*/);

    // And write TCAM entries within TCAM(4,0)
    //
    // First off some random vals we hope don't match anything
    // Payload0 has to be 1 now to indicate a boundary
    tu.tcam_write_value_mask(0, 4, 0, 511,         // stage0 row4 col0 index511
                             0x9999, 0x9999, 1,0); // value mask payload0/1

    // Then a specific value/mask which we try and match against when we lookup(bv)
    // Payload0 has to be 1 now to indicate a boundary
    uint64_t value = UINT64_C(0xABCD0000);
    uint64_t mask = UINT64_C(0xFFFF0000);
    tu.tcam_write_value_mask(0, 4, 0, 510,       // stage0 row4 col0 index510
                             value, mask, 1,0);  // value mask payload0/1

    // Some vals we read/write to check data getting back and forth ok
    tu.tcam_write(0, 4, 0, 509,  // stage0 row4 col0 index509
                  0x19191919, 0x19191919);
    tu.tcam_write(0, 4, 0, 508,  // stage0 row4 col0 index508
                  0x28282828, 0x28282828);

    // this entry is set to match the DA_LO_32
    // Payload0 has to be 1 now to indicate a boundary
    tu.tcam_write_value_mask(0, 4, 0, 507,       // stage0 row4 col0 index506
                             0x22AABBCC, 0xFFFFFFFF, 1,0);  // value mask payload0/1

    // filler entry - not expected to match in this test
    tu.tcam_write(0, 4, 0, 506,  // stage0 row4 col0 index507
                  0xFFFFFFFF, 0xFFFFFFFF);

    // And a catch all entry which should match everything
    // Payload0 has to be 1 now to indicate a boundary
    tu.tcam_write_value_mask(0, 4, 0, 505,   // stage0 row4 col0 index511
                             0x0, 0x0, 1,0); // value mask payload0/1

    // Check the data got there OK - use tcam ref and fish out 508/509
    // Shift data we read right by 1 to get rid of payload bit
    Mau *mau = om->mau_lookup(0, 0);  // Lookup pipe0 stage0
    MauTcam *tcam = mau->tcam_lookup(4,0);

    uint64_t r0, r1;
    tcam->read(509, &r0, &r1, UINT64_C(0));
    EXPECT_EQ((r0 >> 1) & UINT64_C(0XFFFFFFFF), static_cast<uint64_t>(0x19191919));
    EXPECT_EQ((r1 >> 1) & UINT64_C(0XFFFFFFFF), static_cast<uint64_t>(0x19191919));
    tcam->read(508, &r0, &r1, UINT64_C(0));
    EXPECT_EQ((r0 >> 1) & UINT64_C(0XFFFFFFFF), static_cast<uint64_t>(0x28282828));
    EXPECT_EQ((r1 >> 1) & UINT64_C(0XFFFFFFFF), static_cast<uint64_t>(0x28282828));

    // Now check r1/r0 for entry 510, which we added as value/mask matches.
    //
    // We'll be reading direct from TCAM here which has a payload bit in LSB
    // so shift right by 1 again
    //int shift = TestUtil::kTcamPayloadBits;
    uint64_t r1_expected = (((value & mask) | ~mask) & UINT64_C(0xFFFFFFFFFFF));
    uint64_t r0_expected = (((~value & mask) | ~mask) & UINT64_C(0xFFFFFFFFFFF));
    tcam->read(510, &r0, &r1, UINT64_C(0));
    // r1 should be value & mask | ~mask
    EXPECT_EQ((r1 >> 1), r1_expected);
    RMT_UT_LOG_INFO("r1=0x%016" PRIx64 " expected=0x%016" PRIx64 "\n",
                r1, r1_expected);
    // r0 should be ~value & mask | ~mask
    EXPECT_EQ((r0 >> 1), r0_expected);
    RMT_UT_LOG_INFO("r0=0x%016" PRIx64 " expected=0x%016" PRIx64 "\n",
                r0, r0_expected);
    // Now do the lookups
    // This should match entry 510
    BitVector<44> bv(UINT64_C(0xABCD1234));
    int hitindex1 = tcam->lookup_index(bv);
    RMT_UT_LOG_INFO("BV lookup hitindex=%d\n", hitindex1);
    EXPECT_EQ(hitindex1, 510);
    // PHV lookup will match 507 (the 22AABBCC entry)
    int hitindex2 = tcam->lookup_index(phv_in);
    RMT_UT_LOG_INFO("PHV lookup hitindex=%d\n", hitindex2);
    EXPECT_EQ(hitindex2, 507);


    // Spew debug now
    flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Send PHV into MAU using process_match
    // Because we used a broken instruction we expect an ASSERT
    // which process_match turns into a NULL return. So we expect phv_out==NULL
    int ing_nxt_tab = 0, eg_nxt_tab = 0;
    Phv *phv_out = NULL;
    phv_out = mau->process_match(phv_in, phv_in, &ing_nxt_tab, &eg_nxt_tab);
    EXPECT_TRUE((phv_out==NULL));
    EXPECT_TRUE((GLOBAL_RMT_ERROR < 0));
    // Reset GLOBAL_RMT_ERROR or next call into RefModel will abort()
    GLOBAL_RMT_ERROR = 0;


    // Free packet(s)
    tu.packet_free(p_in);
    // Free PHVs
    tu.phv_free(phv_in);
    tu.phv_free(phv_out);

    tu.finish_test();
    tu.quieten_log_flags();
  }


  TEST(BFN_TEST_NAME(MauTest),DelayChecking) {
    GLOBAL_MODEL->Reset();
    if (mau_print) RMT_UT_LOG_INFO("test_mau_delay_checking()\n");

    const bool     jbay = RmtObject::is_jbay_or_later();
    // Consts for delay values
    const int      DELAY_base = MODEL_CHIP_NAMESPACE::MauDefs::kMauBaseDelay;
    const int      DELAY_pred = MODEL_CHIP_NAMESPACE::MauDefs::kMauBasePredicationDelay;
    const int      DELAY_tcam = MODEL_CHIP_NAMESPACE::MauDefs::kMauTcamExtraDelay;
    const int      DELAY_sel  = MODEL_CHIP_NAMESPACE::MauDefs::kMauMeterAluSelectorExtraDelay;
    const int      DELAY_mlpf = MODEL_CHIP_NAMESPACE::MauDefs::kMauMeterAluMeterLpfExtraDelay;
    const int      DELAY_stfl = MODEL_CHIP_NAMESPACE::MauDefs::kMauMeterAluStatefulExtraDelay;
    const int      DELAY_sdiv = MODEL_CHIP_NAMESPACE::MauDefs::kMauMeterAluStatefulDivideExtraDelay;
    // Consts for flags to pass to mau->set_X_dynamic_features()
    const uint32_t wide_sel   = MODEL_CHIP_NAMESPACE::MauDefs::kMauWideSelectorPresent;
    const uint32_t got_tcam   = MODEL_CHIP_NAMESPACE::MauDefs::kMauTcamPresent;
    const uint32_t got_tind   = MODEL_CHIP_NAMESPACE::MauDefs::kMauTindPresent;
    const uint32_t alu_sel    = MODEL_CHIP_NAMESPACE::MauDefs::kMauMeterAluSelectorPresent;
    const uint32_t alu_mlpf   = MODEL_CHIP_NAMESPACE::MauDefs::kMauMeterAluMeterLpfPresent;
    const uint32_t alu_stfl   = MODEL_CHIP_NAMESPACE::MauDefs::kMauMeterAluStatefulPresent;
    const uint32_t alu_ract   = MODEL_CHIP_NAMESPACE::MauDefs::kMauMeterAluRightActionOverrideUsed;
    const uint32_t alu_sdiv   = MODEL_CHIP_NAMESPACE::MauDefs::kMauMeterAluStatefulDivideUsed;
    const uint32_t alu_sqlag  = MODEL_CHIP_NAMESPACE::MauDefs::kMauMeterAluStatefulQlagUsed;
    // Consts for tokens to pass to mau->mau_dependencies->get_delay()
    const int      kPredD     = MODEL_CHIP_NAMESPACE::MauDelay::kPredication;
    const int      kPipeD     = MODEL_CHIP_NAMESPACE::MauDelay::kPipeLatency;

    int chip = 0;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    RmtObjectManager *om = tu.get_objmgr();

    // Find MAUs
    Mau *mau[12];
    for (int s = 0; s < 12; s++) mau[s] = om->mau_lookup(pipe,s);

    // First setup basic deps
    // MAU[0] MAU[6] match-dep - they have to be (on Tofino at least)
    // MAU[1-5] MAU[7-11] action-dep
    // MAU[0-5] MAU[6-11] should form 2 *independent* series for delay checks
    //
    // In each MAU: ALU0=RActionOverride, ALU1=Selector ALU2=MeterLpf ALU3=Stateful
    int mdep = 0, adep = 1;
    for (int s = 0; s < 12; s++) {
      int dep = ((s == 0) || (s == 6)) ?mdep :adep;
      tu.set_dependency(pipe, s, dep, true);
    }

    const int      num_iterations  = 9999;
    const int      debug_mod       = 100;
    const int      iteration_mod   = 21;
    const uint64_t root_rand       = UINT64_C(0xDAB0D1B0D0B0DEB0);
    const uint64_t debug_rand      = UINT64_C(0); // 0==>use root_rand
    const bool     DEBUG_ITER      = false;
    const bool     DEBUG_DELAY     = true;

    // Loop doing many iterations
    //
    uint64_t iter_rand = root_rand;
    for (int iteration = 0; iteration < num_iterations; iteration++) {
      bool dmod = ((iteration % debug_mod) == 0);
      bool imod = ((iteration % iteration_mod) == 0);

      iter_rand = tu.mmix_rand64(iter_rand); // Maybe force iter_rand
      if ((iteration == 0) && (debug_rand != UINT64_C(0))) iter_rand = debug_rand;

      if (DEBUG_ITER && dmod) printf("ITER[%4d] Seed=%016" PRIx64 "\n",
                                     iteration, iter_rand);

      // Program up random features in first 12 MAUs
      uint32_t ftrs05 = 0u; // Remember MAU[0..5] features

      // Loop randomly setting MAU[0..11] ingress(wlog) features
      //
      uint64_t loop_rand = iter_rand;
      for (int s = 0; s < 12; s++) {
        loop_rand = tu.mmix_rand64(loop_rand);
        uint32_t mask = 0u; // Try and keep mask sparse
        if ((loop_rand % UINT64_C( 2)) == 0) mask |= got_tcam;
        if ((loop_rand % UINT64_C( 4)) == 0) mask |= got_tind;
        if ((loop_rand % UINT64_C( 8)) == 0) mask |= wide_sel;
        if ((loop_rand % UINT64_C( 3)) == 0) mask |= alu_stfl;
        if ((loop_rand % UINT64_C( 6)) == 0) mask |= ( (jbay) ?alu_sdiv :0 );  // Only on JBay
        if ((loop_rand % UINT64_C( 9)) == 0) mask |= ( (jbay) ?alu_sqlag :0 ); // Only on JBay
        if ((loop_rand % UINT64_C( 5)) == 0) mask |= alu_sel;
        if ((loop_rand % UINT64_C( 7)) == 0) mask |= alu_mlpf;
        if ((loop_rand % UINT64_C(11)) == 0) mask |= ( (jbay) ?alu_ract :0 );  // Only on JBay
        loop_rand = tu.mmix_rand64(loop_rand);
        uint32_t ftrsS = static_cast<uint32_t>(loop_rand & static_cast<uint32_t>(mask));

        // No extra features in MAU[0..5] every imod'th go
        // but continue to set features in MOD[6..11] - this
        // checks the two series of stages are independent
        if ((s < 6) && imod) ftrsS = 0u;

        // Call mau func to set features - these should be propagated
        // to MAUs within the same series (so MAU[0..5] MAU[6..11])
        mau[s]->set_hash_dynamic_features(ftrsS & wide_sel, 0u);
        mau[s]->set_tcam_dynamic_features(ftrsS & got_tcam, 0u);
        mau[s]->set_tind_dynamic_features(ftrsS & got_tind, 0u);
        mau[s]->set_meter_alu_dynamic_features(0, false, ftrsS & alu_ract, 0u);
        mau[s]->set_meter_alu_dynamic_features(1, false, ftrsS & alu_sel, 0u);
        mau[s]->set_meter_alu_dynamic_features(2, false, ftrsS & alu_mlpf, 0u);
        // If StatefulDivideUsed/StatefulQlag feature set, ensure StatefulPresent also set
        if ((ftrsS & (alu_sdiv|alu_sqlag)) != 0u) ftrsS |= alu_stfl;
        mau[s]->set_meter_alu_dynamic_features(3, false,
                                               ftrsS & (alu_stfl|alu_sdiv|alu_sqlag), 0u);

        // Remember features added to MAU[0..5]
        if (s < 6) ftrs05 |= ftrsS;

      } // for (int s = 0; s < 12; s++) {


      // Now we can check series delays - first work out what they should be
      // which depends on the features in use (see uArch docs section 3.2)
      int d_tcam = 0, d_sel = 0, d_mlpf = 0, d_stfl = 0, d_alu = 0;
      if (((ftrs05 & got_tcam) != 0u) || ((ftrs05 & got_tind) != 0u) ||
          ((ftrs05 & wide_sel) != 0u)) {
        d_tcam = DELAY_tcam;
      }
      if (((ftrs05 & alu_sel) != 0u) || ((ftrs05 & alu_sqlag) != 0u))
        d_sel = DELAY_sel;

      if ((ftrs05 & alu_mlpf) != 0u) d_mlpf = DELAY_mlpf;

      if ((ftrs05 & alu_stfl) != 0u) {
        d_stfl = DELAY_stfl;
        if ((ftrs05 & alu_sdiv) != 0u) d_stfl += DELAY_sdiv;
      }

      // Apply precedence as described in uArch 3.2
      if                  (d_sel  > 0)  d_mlpf = d_stfl = 0;
      else if (( jbay) && (d_stfl > 0)) d_mlpf = 0;
      else if ((!jbay) && (d_mlpf > 0)) d_stfl = 0;
      d_alu = d_sel + d_mlpf + d_stfl;
      if (DEBUG_DELAY && dmod) printf("ITER[%4d] Seed=%016" PRIx64 " "
                                      "ftrs=0x%04x d_tcam=%d d_alu=%d\n",
                                      iteration, iter_rand, ftrs05, d_tcam, d_alu);

      // Values calculated above should match ACTUAL series delays given features
      int series_pred_act = MauDelay::predication(ftrs05);
      int series_pipe_act = MauDelay::pipe_latency(ftrs05);
      EXPECT_EQ(DELAY_pred + d_tcam, series_pred_act);
      EXPECT_EQ(DELAY_base + d_tcam + d_alu, series_pipe_act);

      // Now go through ALL of MAU[0..5] and check ACTUAL MAU delays match too
      for (int s = 0; s < 6; s++) {
        int mau_pred_act = mau[s]->mau_dependencies()->get_delay(true, kPredD);
        int mau_pipe_act = mau[s]->mau_dependencies()->get_delay(true, kPipeD);
        EXPECT_EQ(series_pred_act, mau_pred_act);
        EXPECT_EQ(series_pipe_act, mau_pipe_act);

        // Then for each ALU check the MeterAluGroupLeft|RightActionDelay as expected
        for (int alu = 0; alu < 4; alu++) {
          uint32_t ALU_ftrs = mau[s]->meter_alu_dynamic_features(alu);
          bool selector_present = ((ftrs05 & alu_sel) != 0u);
          bool stateful_divide = ((ftrs05 & alu_sdiv) != 0u);
          bool divide_enabled = ((ALU_ftrs & alu_sdiv) != 0u);
          bool right_action_override_used = ((ALU_ftrs & alu_ract) != 0u);
          int  exp_l;

          // From email thread "Precedence of delay config" 14Jan2019 MikeF said:
          //
          // selector_present ? ( (stateful_divide & divide_enabled*)    ? 2 : 4) :
          //                    ( (stateful_divide & !divide_enabled * ) ? 2 : 0)
          //
          // This should end up in uArch 0.10
          //
          if (selector_present) {
            if (stateful_divide &&  divide_enabled) exp_l = 2; else exp_l = 4;
          } else {
            if (stateful_divide && !divide_enabled) exp_l = 2; else exp_l = 0;
          }
          int exp_r = exp_l;
          if (right_action_override_used) {
            if (selector_present) exp_r = 6; else exp_r = 2;
          }

          int act_l = MauDelay::meter_alu_group_action_left(alu, ftrs05, ALU_ftrs);
          int act_r = MauDelay::meter_alu_group_action_right(alu, ftrs05, ALU_ftrs);
          EXPECT_EQ(act_l, exp_l);
          EXPECT_EQ(act_r, exp_r);
        }
      }

#ifdef MODEL_CHIP_JBAY_OR_LATER
      if ((mau[3]->meter_alu_dynamic_features(3) & alu_sdiv) == 0u) {
        // Only do this if MAU[3].MALU[3] *not* using stateful divide
        //
        // Manually setup stateful divide by writing salu_instr_common
        // - this should trigger callbacks that cause features to be
        //   redetermined and delays reevaluated in MAU[0..5]
        auto& mau_base = MODEL_CHIP_NAMESPACE::RegisterUtils::ref_mau(pipe,3);
        auto& stfl = mau_base.rams.map_alu.meter_group[3].stateful;
        auto  a_stfl_ctl = &stfl.stateful_ctl; // Must enable this too
        auto  a_salu_instr_com = &stfl.salu_instr_common[3];
        uint32_t v0_stfl_ctl = tu.InWord((void*)a_stfl_ctl);
        uint32_t v0_salu_instr_com = tu.InWord((void*)a_salu_instr_com);
        uint32_t v_stfl_ctl = v0_stfl_ctl;
        uint32_t v_salu_instr_com = v0_salu_instr_com;
        setp_stateful_ctl_salu_enable(&v_stfl_ctl, 1);
        setp_salu_instr_common_salu_divide_enable(&v_salu_instr_com, 1);
        tu.OutWord((void*)a_stfl_ctl, v_stfl_ctl);
        tu.OutWord((void*)a_salu_instr_com, v_salu_instr_com);

        // Should now see MAU[3].MALU[3] using stateful_divide
        EXPECT_NE(0u, (mau[3]->meter_alu_dynamic_features(3) & alu_sdiv));

        // Now reset to how things were
        tu.OutWord((void*)a_stfl_ctl, v0_stfl_ctl);
        tu.OutWord((void*)a_salu_instr_com, v0_salu_instr_com);
      }
#endif
      iter_rand = loop_rand;

    } // for (int iteration = 0; iteration < num_iterations; iteration++) {
  }



// ******************** TOFINO only tests ********************

#if MCN_TEST(MODEL_CHIP_NAMESPACE,tofino)

  TEST(BFN_TEST_NAME(MauTest),RandomReadWrite0) {
    GLOBAL_MODEL->Reset();
    if (mau_print) RMT_UT_LOG_INFO("test_mau_random_read_write0()\n");

    int chip = 202;
    int p = 0;
    int s = 0;

    // Instantiate whole chip - chip 202 so only 2 MAUs per pipe
    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, p, s);
    tu.set_debug(true);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);

    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    // Up debug
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Output an entry to addr
    uint64_t addr = UINT64_C(0x0000021c8003c000);
    uint64_t wdata0 = UINT64_C(0xF);
    uint64_t wdata1 = UINT64_C(0x1);
    GLOBAL_MODEL->IndirectWrite(chip, addr, wdata0, wdata1);

    // Read back data directly
    uint64_t rdata0;
    uint64_t rdata1;
    rdata0 = UINT64_C(0);
    rdata1 = UINT64_C(0);
    GLOBAL_MODEL->IndirectRead(chip, addr, &rdata0, &rdata1);

    RMT_UT_LOG_INFO("W0=%016" PRIx64 " W1=%016" PRIx64 "\n", wdata0, wdata1);
    RMT_UT_LOG_INFO("R0=%016" PRIx64 " R1=%016" PRIx64 "\n", rdata0, rdata1);
    EXPECT_EQ(rdata0,wdata0);
    EXPECT_EQ(rdata1,wdata1);


    // Schtum
    tu.finish_test();
    tu.quieten_log_flags();

    // Free everything up
    om->chip_free_all();
  }
#endif

}
