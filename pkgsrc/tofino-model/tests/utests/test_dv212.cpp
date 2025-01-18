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

// XXX -> test_dv212.cpp
// Ref Model has a Wide Tcam match bug (again).
// Miscompare 274e1, 1072e1

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

  bool dv212_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(Dv212Test),Packet1) {
    GLOBAL_MODEL->Reset();
    if (dv212_print) RMT_UT_LOG_INFO("test_dv212_packet1()\n");
    
    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = FEW;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Set 'vintage' of DV test to configure other global vars correctly
    tu.set_dv_test(212);
    // Change log prefix
    RmtObjectManager *om = tu.get_objmgr();
    om->string_map_lookup("SetLogPrefix","DV212");

    // Just to stop compiler complaining about unused vars
    //flags = ALL; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);    


    // PUT GENERATED OutWord/IndirectWrite calls HERE
    ASSERT_TRUE(tu.read_test_file("extracted_tests/test_dv212.test"));


    tu.finish_test();
    tu.quieten_log_flags();
}


}


    
// Summary: Ref Model has a Wide Tcam match bug (again)....
// 
// Just when you thought we were done..  Once more unto the breach, dear friends
// 
// Nice way to start off the weekend.
// 
// Fail Msg:
// # UVM_ERROR @1220000ps uvm_test_top.mau_env.dpath_sb>mau_datapath_sb.svh(3279) [SB_RTL] #274e1# LTCAM miscompare Hit-index for lrn=0 hi(rtl=0x61000 vs ref=0x616a0)
// # UVM_ERROR @4701600ps uvm_test_top.mau_env.dpath_sb>mau_datapath_sb.svh(3279) [SB_RTL] #1072e1# LTCAM miscompare Hit-index for lrn=0 hi(rtl=0x61000 vs ref=0x616a0)
// # UVM_ERROR :    2
// 
// All the fails are from the same source Phv that is driven multiple times into the MAU.
// 
// # INFO @3192900ps uvm_test_top.prev_pkt_ag.pkt_drv>mau_mau_pktheader_driver.svh(304) [DRV] EOP delay Eop#1072e1# itg=8 : <EOP id=1072  v=b10 rcr=b00 port_id={0x21,0x00} bytesize={0x00ff,0x0000}> dly=8 cycle=0
// 
// # INFO @4672800ps uvm_test_top.prev_pkt_ag.pkt_drv>mau_mau_pktheader_driver.svh(424) [DRV-PKT] get_and_drive_pkt() AF op=1580 {1072:1038}:  #1072e1#  : start by change id from 393 to 1580  <id#393#> ta=b10 ophvld=b10 err=b00 ver=b0000
// 
// 
// # Physical TCAM array:
// # How to read? {ltn, ltid, vpn, I/E, Searchbus}
// # 
// #               0                                1
// # row_11: { 1, 7,0x0e,E,S0}                                             
// # row_10: { 1, 7,0x0e,E,S0}                                             
// # row_ 9: { 1, 7,0x0e,E,S0}                                             
// # row_ 8: { 1, 7,0x0f,E,S0}                                             
// # row_ 7: { 1, 7,0x0f,E,S0}     row_ 7: { 0, 1,0x31,E,S1}               
// # row_ 6: { 1, 7,0x0f,E,S0}     row_ 6: { 0, 1,0x31,E,S1}               
// #                               row_ 5: { 0, 1,0x31,E,S1}               
// #                               row_ 4: { 0, 1,0x31,E,S1}               
// #                               row_ 3: { 0, 1,0x30,E,S1}               
// #                               row_ 2: { 0, 1,0x30,E,S1}               
// #                               row_ 1: { 0, 1,0x30,E,S1}               
// #                               row_ 0: { 0, 1,0x30,E,S1}               
// # 
// 
// 
// ===============================================================================
//  tu.OutWord(&mau_reg_map.tcams.tcam_match_adr_shift[0], 0x4); /* a=0x2040980   <<tcam_match_adr_shift[2:0]=3'h4>> */
//  tu.OutWord(&mau_reg_map.tcams.tcam_match_adr_shift[1], 0x4); /* a=0x2040984   <<tcam_match_adr_shift[2:0]=3'h4>> */
// 
// # UVM_ERROR @1220000ps uvm_test_top.mau_env.dpath_sb>mau_datapath_sb.svh(3279) [SB_RTL] #274e1# LTCAM miscompare Hit-index for lrn=0 hi(rtl=0x61000 vs ref=0x616a0)
// 
// Location in Logical Tcam 0 
// rtl=0x61000 >> 4 = 0x6100  vpn=0x30 idx=0x100 (256)
// ref=0x616a0 >> 4 = 0x616a  vpn=0x30 idx=0x16a (362)
// 
// ===============================================================================
// # INFO @ 853700ps uvm_test_top.prev_pkt_ag.pkt_drv>mau_mau_pktheader_driver.svh(304) [DRV] EOP delay Eop#274e1# itg=10 : <EOP id=274  v=b10 rcr=b00 port_id={0x1f,0x00} bytesize={0x012f,0x0000}> dly=10 cycle=0
// # INFO @1191200ps uvm_test_top.prev_pkt_ag.pkt_drv>mau_mau_pktheader_driver.svh(424) [DRV-PKT] get_and_drive_pkt() AF op=393 {274:250}:  #274e1#  : start  <id#393#> ta=b10 ophvld=b10 err=b00 ver=b0000
// #   next_table={0x00,0x00} itg=6
// # INFO @1191200ps uvm_test_top.prev_pkt_ag.pkt_drv>mau_mau_pktheader_driver.svh(480) [DRV-PKT] get_and_drive_pkt()   op=393 {274:250}:  #274e1#  : DONE  <id#393#> ta=b10 ophvld=b10 err=b00 ver=b0000  sttbl={0x00,0x00} 
// 
// # INFO @1198000ps uvm_test_top.mau_env.mauint_ag.mon>mau_match_action_monitor.svh(900) [MON] Saw txn ltcam (egr=1)  LgTcam id #274e1#  d[0]=0xe1000 pld=0 d[1]=0x9cee0 pld=0 d[2]=0xXxxxx pld=0 d[3]=0xXxxxx pld=0 d[4]=0xXxxxx pld=0 d[5]=0xXxxxx pld=0 d[6]=0xXxxxx pld=0 d[7]=0xXxxxx pld=0 
// 
// # UVM_ERROR @1220000ps uvm_test_top.mau_env.dpath_sb>mau_datapath_sb.svh(3279) [SB_RTL] #274e1# LTCAM miscompare Hit-index for lrn=0 hi(rtl=0x61000 vs ref=0x616a0)
// # LgTBL Checked refm ltid=1{MauLTcamRes  v=1 m=1 tm=1 ti=1 maddr=0x616a0 pay=0 }
// # LgTBL Checked refm ltid=7{MauLTcamRes  v=1 m=1 tm=1 ti=1 maddr=0x1cee0 pay=0 }
// # Checked Match-Action Internal  rtl #274e1# ltid=all{ Mau Match Action Op id=#274e1#  cycle(1494,1505,1512,1516)}
// #  for  Mau Match Action Op id=#274e1#  cycle(1494,1505,1512,1516)
// 
// 
// 
// 
// ===============================================================================
// ===============================================================================
// 
// At 1196400 ps, after Tcam col1 row3 match_out is stable.
// 
// mrdflag is stored MRD flag so '0' is distribute and '1' is stop
// 
// From Waves
// col1 row 0
// S0  03b_50e5_0063
// S1  fc4_af1a_ff9c
// matchout ----_0008_0800_0000_0000_0000_4000_0000_0000_0020_0000_0000_0000_0000_0000_0000_0200_0000_0000_0600_0240_0000_0000_0000_0000_0000_0000_1000_1000_0000_0000_0000
// mrdflag  ffff_0000_0000_ffff_ffff_0000_baba_5545_ffff_0400_0000_ffff_ffff_0000_2222_dfdd_0000_ffff_ffff_0020_0000_ffff_4404_bbfb_0000_ffff_ffff_0000_0000_ffff_5d55_a2ab
// mrdmatch ----_07ff_fff0_0000_0000_0000_c000_0000_0000_07ff_c000_0000_0000_0000_0000_0001_fffc_0000_0001_ffc1_ffff_0000_0000_0000_0000_0000_0001_ffef_ffe0_0000_0000_0000
// 
//                                                       0111                             X-- idx=0x100(256)
//                                                        Y-idx=0x16a(362)
// row1
// chainin  ----_07ff_fff0_0000_0000_0000_c000_0000_0000_07ff_c000_0000_0000_0000_0000_0001_fffc_0000_0001_ffc1_ffff_0000_0000_0000_0000_0000_0001_ffef_ffe0_0000_0000_0000
// S0  c9d_8031_a872
// S1  362_7fce_578d
// matchout ----_----_0010_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0100_0000_0000_0440_0240_0000_0000_0000_0000_0000_0000_1000_0000_0000_0000_0000
// mrdflag  ffff_0000_0000_ffff_ffff_0000_baba_5545_ffff_0400_0000_ffff_ffff_0000_2222_dfdd_0000_ffff_ffff_0020_0000_ffff_4404_bbfb_0000_ffff_ffff_0000_0000_ffff_5d55_a2ab
// mrdmatch ----_----_0fff_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_fffe_0000_0001_ffc1_ffff_0000_0000_0000_0000_0000_0001_ffe0_0000_0000_0000_0000
// 
// row2
// chainin  ----_----_0fff_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_fffe_0000_0001_ffc1_ffff_0000_0000_0000_0000_0000_0001_ffe0_0000_0000_0000_0000
// S0  3ff_ffd9_b3ec
// S1  c00_0026_4c13
// matchout ----_----_----_----_----_----_----_----_----_----_----_----_----_----_----_----_3200_0000_0000_0000_4040_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000
// mrdflag  ffff_0000_0000_ffff_ffff_0000_baba_5545_ffff_0400_0000_ffff_ffff_0000_2222_dfdd_0000_ffff_ffff_0020_0000_ffff_4404_bbfb_0000_ffff_ffff_0000_0000_ffff_5d55_a2ab
// mrdmatch ----_----_----_----_----_----_----_----_----_----_----_----_----_----_----_0001_fffc_0000_0000_003f_ffff_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000
// 
// row3
// chainin  ----_----_----_----_----_----_----_----_----_----_----_----_----_----_----_0001_fffc_0000_0000_003f_ffff_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000
// S0  cff_ffff_ff67
// S1  300_0000_0098
// matchout ----_----_----_----_----_----_----_----_----_----_----_----_----_----_----_----_3200_0000_0000_0000_4040_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000
// mrdflag  ffff_0000_0000_ffff_ffff_0000_baba_5545_ffff_0400_0000_ffff_ffff_0000_2222_dfdd_0000_ffff_ffff_0020_0000_ffff_4404_bbfb_0000_ffff_ffff_0000_0000_ffff_5d55_a2ab
// mrdmatch ----_----_----_----_----_----_----_----_----_----_----_----_----_----_----_---1_fffc_0000_0000_003f_ffff_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000
//    
//                                                                                        ^-- index 0x100 or 256
// 
// ===============================================================================
// ===============================================================================
// Tcam 1 programming around  Y-idx=0x16a(362)  +/- 8 entries
// 
// To pull the programmed lines from the DV logic.
// $ zgrep -e 'Tcam-Entry BYTE' -e 'done ADD_ENTRY tcam' -e 'start ADD_ENTRY tcam' -e 'mau_tcam.svh.220' -e 'mau_tcam.svh.229' run_dir/run.log.gz > run_dir/tcam_prog.log 
// $ grep -e  'done ADD_ENTRY tcam' run_dir/tcam_prog.log | grep -e tcam_array.tcam1_1 | sed -e 's/pipe0.stage0.mau_cfg.mau_array_cfg.tcam_array.//g' |  less
// 
// S0  c9d_8031_a872
// S1  362_7fce_578d
// 
// # INFO @  60200ps >mau_tcam.svh(195) [tcam1_1] done ADD_ENTRY tcam[ 1][ 1][367].word0=0x18c700639167, word1=0x0738ff9c6e9b, mask=0x000000000002  byte_mask=b111111
// # INFO @  60200ps >mau_tcam.svh(195) [tcam1_1] done ADD_ENTRY tcam[ 1][ 1][366].word0=0x193b0063aea5, word1=0x06c4ff9c515a, mask=0x000000000000  byte_mask=b111111
// # INFO @  60200ps >mau_tcam.svh(195) [tcam1_1] done ADD_ENTRY tcam[ 1][ 1][365].word0=0x195bc667d1ed, word1=0x06ffff9cffff, mask=0x005bc604d1ec  byte_mask=b111111
// # INFO @  60200ps >mau_tcam.svh(195) [tcam1_1] done ADD_ENTRY tcam[ 1][ 1][364].word0=0x19ffa067fff5, word1=0x068fffdc5f5e, mask=0x008fa0445f54  byte_mask=b111111
// # INFO @  60200ps >mau_tcam.svh(195) [tcam1_1] done ADD_ENTRY tcam[ 1][ 1][363].word0=0x1953627b9f6f, word1=0x06bfff9fefff, mask=0x0013621b8f6e  byte_mask=b111111
// 
// # INFO @  60200ps >mau_tcam.svh(195) [tcam1_1] done ADD_ENTRY tcam[ 1][ 1][361].word0=0x19ff927bd775, word1=0x07fbfffffd9b, mask=0x01fb927bd510  byte_mask=b111111
// # INFO @  60200ps >mau_tcam.svh(195) [tcam1_1] done ADD_ENTRY tcam[ 1][ 1][360].word0=0x18c70063a927, word1=0x0738ff9c56da, mask=0x000000000002  byte_mask=b111111
// # INFO @  60200ps >mau_tcam.svh(195) [tcam1_1] done ADD_ENTRY tcam[ 1][ 1][359].word0=0x193b00638f25, word1=0x06c4ff9c70da, mask=0x000000000000  byte_mask=b111111
// # INFO @  60200ps >mau_tcam.svh(195) [tcam1_1] done ADD_ENTRY tcam[ 1][ 1][358].word0=0x19df00e7aebf, word1=0x07feffbcfbdb, mask=0x01de00a4aa9a  byte_mask=b111111
// # INFO @  60200ps >mau_tcam.svh(195) [tcam1_1] done ADD_ENTRY tcam[ 1][ 1][357].word0=0x19bf7d73fb7d, word1=0x06f6ffdfdfde, mask=0x00b67d53db5c  byte_mask=b111111
// # INFO @  60200ps >mau_tcam.svh(195) [tcam1_1] done ADD_ENTRY tcam[ 1][ 1][356].word0=0x197b417faaed, word1=0x06effffefdfa, mask=0x006b417ea8e8  byte_mask=b111111
// # INFO @  60200ps >mau_tcam.svh(195) [tcam1_1] done ADD_ENTRY tcam[ 1][ 1][355].word0=0x19fbc86fffe7, word1=0x07dcfffc693f, mask=0x01d8c86c6926  byte_mask=b111111  << last MRD line
// 
// # INFO @  60200ps >mau_tcam.svh(195) [tcam1_1] done ADD_ENTRY tcam[ 1][ 1][354].word0=0x193b0063afa5, word1=0x06c4ff9c505b, mask=0x000000000000  byte_mask=b111111
// # INFO @  60200ps >mau_tcam.svh(195) [tcam1_1] done ADD_ENTRY tcam[ 1][ 1][353].word0=0x19db6073bff5, word1=0x06bfff9c7bfb, mask=0x009b60103bf0  byte_mask=b111111
// # INFO @  60200ps >mau_tcam.svh(195) [tcam1_1] done ADD_ENTRY tcam[ 1][ 1][352].word0=0x19430063a625, word1=0x07bcff9c59da, mask=0x010000000000  byte_mask=b111111
// 
// No match in the first byte .......
// 
// Ref Model messages - look in the run.log.gz for all of them.. had trouble following the back and forth.....
// 
// # INFO @1220000ps uvm_test_top.mau_env.dpath_sb>mau_datapath_sb.svh(1091) [SB_RTL] Send 0 sweeps to be executed by refmodel going to delete them from RTL sweep_q
// # INFO @1220000ps uvm_test_top.mau_env.dpath_sb>mau_datapath_sb.svh(2742) [SB_REF] #274e1# Next Table RefModel  i=0xff e=0x00, #actions= 6
// # INFO @1220000ps uvm_test_top.mau_env.dpath_sb>mau_datapath_sb.svh(2889) [SB_RTL] #274e1# match iPhv rtl PREV A vs rtl NEXT B 
// 
// ref_model_wrapper::mau_process_match( extract LTcam results start egr=1 ltcam_en=3
// <0,0,0> V MauLogicalTcam::lookup_ternary_match<0,T=0> Try TCAM(6,1) vpn=49[0x0031] W=1
// ......
// <0,0,4,1> V  TCAM3::tcam_match(2) s0=0x00000b3b50e50063 s1=0x000004c4af1aff9c {0} MISS
// <0,0,0> V MauLogicalTcam::lookup_wide_match<0> TCAM0=(6,1) NO CHAIN HIT
// <0,0,0> V MauLogicalTcam::lookup_ternary_match<0,T=0>=-1  {wide match}
// 
// <0,0,0> V MauLogicalTcam::lookup_ternary_match<0,T=0> Try TCAM(3,1) vpn=48[0x0030] W=1
// <0,0,0> WARN TCAM 2 in chain 0x000f must set tcam_match_out_enable
// <0,0,0> WARN TCAM 2 in chain 0x000f must set tcam_match_out_enable
// <0,0,0> V MauLogicalTcam::lookup_wide_match<0> TCAM0=(3,1) Head=511 Chain=0x000f.....
// <0,0,0> V MauLogicalTcam::lookup_wide_match<0> TCAM0=(3,1) RangeHI=511 RangeLO(pre)=0...
// <0,0,0> TRC MAU_TCAM_ROW::get_match_data row=0 even=FC4AF1AFF9C odd=3627FCE578D
// <0,0,0,1> V TCAM3::tcam_match(495)  w0=0x000000cff56dc0ff w1=0x00000f39aefaffdd {0} MISS
// ...
// <0,0,3,1> V TCAM3::tcam_match(357)  w0=0x00000cffffffff67 w1=0x0000039324ee36fa {0} HIT
// <0,0,3,1> V  TCAM3::tcam_match(357) s0=0x00000cffffffff67 s1=0x0000030000000098 {0} HIT
// <0,0,0> V MauLogicalTcam::lookup_wide_match<0> TCAM0=(3,1) Head=511 Chain=0x000f RangeHI=352 RangeLO(post)=345 ChainHitPri=362 DONE
// <0,0,0> V MauLogicalTcam::lookup_ternary_match<0,T=0>=362  {wide match}
// <0,0,0> V MauLogicalTcam::lookup_ternary_match<0,T=0>=362 (Shift=4 matchAddr=399008[0x000616a0])
// <0,0,0> V MauLogicalTcam::find_tind_sram(type=6,vpn=12) NO TINDS EXAMINED!!
// <0,0,0> V MauLogicalTcam::lookup_ternary_match<0,T=0> HitMiss=HIT (tot_hits=487 tot_misses=852)
// 
// 
// More info in the file: bug_template in run_dir
// 
// 
// Stream    : mau
// Version   : Change 35427 on 2016/05/02 by jrobinson@xwork_jrobinson '   Fix for two Pbus irritaors r'
// p4 opened : None
// p4 shelve#: None
// RERUNCMD  : runSim.py +test=mau_tcam_multi_phv_test_c -f knobs/tcam_2tbl_W2_4D1_8_cfg_ie.knobs -f knobs/seq_mixA_dcm1_rpt_tptr.knobs -f knobs/n_phvs_400_900.knobs -f knobs/phvattr_ie.knobs -f xpr op_pass.knobs -f questa.knobs -f knobs/cfg_rand_stgX_dep_dly.knobs -f knobs/pbus_seq0_memcsr_irritator_rand.knobs +UVM_VERBOSITY=UVM_HIGH +UVM_MAX_QUIT_COUNT=2 -f config/zdebug.knobs -f config/incr.dump.knobs  +sim.pre_run_do_file=/proj/tofino/dsrinu/rtf_dsrinu/verif/tb/block/mau/mau_xprop.do -f knobs/mau_vendor_mem.knobs +seed=6143604945495591599  --vcs_suppress_stdout
// 
// Seed      :  +seed=6143604945495591599
// Bug dir   : /scratch/dv//jira/TOF-<bug#>
// Ref model : MODEL_HOME=/tools/barefoot/sw/model/2016_04_29/model


