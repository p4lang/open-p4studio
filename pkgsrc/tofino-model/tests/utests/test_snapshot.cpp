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

  bool snapshot_print = false;

using namespace std;
using namespace MODEL_CHIP_NAMESPACE;



TEST(BFN_TEST_NAME(SnapshotTest),Basic) {
  if (snapshot_print) RMT_UT_LOG_INFO("test_snapshot_basic()\n");    
  int chip = 202; // Just the 2 stages
  int pipe = 0;
  int stage = 0;
  
  // Create our TestUtil class
  // Instantiating new TestUtil obj should free all existing
  // RmtObjectManagers (if this has not already occurred) then
  // recreate a RmtObjectManager just for chip
  TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
  //auto& mm_regs = RegisterUtils::ref_mau(pipe,stage).rams.match.merge;
  
  
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
  
  // Instantiate whole chip and fish out objmgr
  RmtObjectManager *om = tu.get_objmgr();
  ASSERT_TRUE(om != NULL);
  // Lookup this Pipe/Stage MAU and MAU_SNAPSHOT obj
  Mau *mau = om->mau_lookup(pipe, stage);
  ASSERT_TRUE(mau != NULL);
  // Setup stage default regs
  tu.set_stage_default_regs(0);
  tu.set_stage_default_regs(1);
  // Lookup some port
  Port *port = tu.port_get(16);
  ASSERT_TRUE(port != NULL);
  MauSnapshot *mausnap = mau->mau_snapshot();
  ASSERT_TRUE(mausnap != NULL);
  uint64_t mask32 = UINT64_C(0xFFFFFFFF);
  
  
  // Setup a Phv
  // Note, to cater for JBay, we MUST use Parser set_p functions
  // to program PHV words - this enables us to easily skip DarkPHV
  // words which Snapshot *cannot* match on
  Phv *phv_in = tu.phv_alloc();
  phv_in->set_ingress();
  phv_in->set_p(10, 0x12345678);
  phv_in->set_p(20, 0xABCDEFAB);
  
  // Setup snapshot regs using TestUtil helper funcs
  int snap_state = 1; // Armed 
  tu.set_phv_match(10, true, 0x12345678, 0xFFFFFFFF);
  tu.set_phv_capture(20, true);
  (void)tu.setup_snapshot(true, snap_state); // Ingress snapshot Armed
  
  // Send in a matching PHV using TestUtil::port_process_inbound
  (void)tu.port_process_inbound(port, phv_in);
  
  // Read back snapshot - check we got info from PHV we put in
  // Note, get_phv_capture_word maps the passed-in PHV word internally
  // so we just pass the same indexes we used when we did the set_p's
  snap_state = tu.setup_snapshot(true, -1);
  EXPECT_EQ(3, snap_state);  // Should be Full
  uint32_t word10 = static_cast<uint32_t>(tu.get_phv_capture_word(10) & mask32);
  uint32_t word20 = static_cast<uint32_t>(tu.get_phv_capture_word(20) & mask32);
  EXPECT_EQ(0x12345678u, word10);
  EXPECT_EQ(0xABCDEFABu, word20);
  
  
  // Schtum
  tu.finish_test();
  tu.quieten_log_flags();
}



TEST(BFN_TEST_NAME(SnapshotTest),DualWordMatch) {
  if (snapshot_print) RMT_UT_LOG_INFO("test_snapshot_dual_word_match()\n");    
  int chip = 202; // Just the 2 stages
  int pipe = 0;
  int stage = 0;
  
  // Create our TestUtil class
  // Instantiating new TestUtil obj should free all existing
  // RmtObjectManagers (if this has not already occurred) then
  // recreate a RmtObjectManager just for chip
  TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
  //auto& mm_regs = RegisterUtils::ref_mau(pipe,stage).rams.match.merge;
  
  
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
  

  // Instantiate whole chip and fish out objmgr
  RmtObjectManager *om = tu.get_objmgr();
  ASSERT_TRUE(om != NULL);
  // Lookup this Pipe/Stage MAU and MAU_SNAPSHOT obj
  Mau *mau = om->mau_lookup(pipe, stage);
  ASSERT_TRUE(mau != NULL);
  // Setup stage default regs
  tu.set_stage_default_regs(0);
  tu.set_stage_default_regs(1);
  // Lookup some port
  Port *port = tu.port_get(16);
  ASSERT_TRUE(port != NULL);
  MauSnapshot *mausnap = mau->mau_snapshot();
  ASSERT_TRUE(mausnap != NULL);
  uint64_t mask32 = UINT64_C(0xFFFFFFFF);


  // Setup a Phv (use set_p again)
  Phv *phv_in = tu.phv_alloc();
  phv_in->set_ingress();
  phv_in->set_p(10, 0x12345678);
  phv_in->set_p(11, 0x87654321);
  phv_in->set_p(20, 0xABCDEFAB);
  

  // Setup snapshot regs using TestUtil helper funcs
  int snap_state = 1; // Armed 
  tu.set_phv_match(10, true, 0x12345678, 0xFFFFFFFF);
  tu.set_phv_match(11, true, 0x87654321, 0xFFFFFFFF);
  tu.set_phv_capture(20, true);  
  (void)tu.setup_snapshot(true, snap_state); // Ingress snapshot Armed
  
  // Send in a matching PHV using TestUtil::port_process_inbound
  (void)tu.port_process_inbound(port, phv_in);
  
  // Read back snapshot - check we got info from PHV we put in
  snap_state = tu.setup_snapshot(true, -1);
  EXPECT_EQ(3, snap_state);  // Should be Full
  uint32_t word10 = static_cast<uint32_t>(tu.get_phv_capture_word(10) & mask32);
  uint32_t word11 = static_cast<uint32_t>(tu.get_phv_capture_word(11) & mask32);
  uint32_t word20 = static_cast<uint32_t>(tu.get_phv_capture_word(20) & mask32);
  EXPECT_EQ(0x12345678u, word10);
  EXPECT_EQ(0x87654321u, word11);
  EXPECT_EQ(0xABCDEFABu, word20);
  
  
  // Schtum
  tu.finish_test();
  tu.quieten_log_flags();
}



TEST(BFN_TEST_NAME(SnapshotTest),FirstMatchCaptured) {
  if (snapshot_print) RMT_UT_LOG_INFO("test_snapshot_first_match_captured()\n");    
  int chip = 202; // Just the 2 stages
  int pipe = 0;
  int stage = 0;
  
  // Create our TestUtil class
  // Instantiating new TestUtil obj should free all existing
  // RmtObjectManagers (if this has not already occurred) then
  // recreate a RmtObjectManager just for chip
  TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
  //auto& mm_regs = RegisterUtils::ref_mau(pipe,stage).rams.match.merge;
  

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


  // Instantiate whole chip and fish out objmgr
  RmtObjectManager *om = tu.get_objmgr();
  ASSERT_TRUE(om != NULL);
  // Lookup this Pipe/Stage MAU and MAU_SNAPSHOT obj
  Mau *mau = om->mau_lookup(pipe, stage);
  ASSERT_TRUE(mau != NULL);
  // Setup stage default regs
  tu.set_stage_default_regs(0);
  tu.set_stage_default_regs(1);
  // Lookup some port
  Port *port = tu.port_get(16);
  ASSERT_TRUE(port != NULL);
  MauSnapshot *mausnap = mau->mau_snapshot();
  ASSERT_TRUE(mausnap != NULL);
  uint64_t mask32 = UINT64_C(0xFFFFFFFF);
  uint64_t mask16 = UINT64_C(0xFFFF);


  // Setup a Phv (use set_p again)
  Phv *phv_in = tu.phv_alloc();
  phv_in->set_ingress();
  phv_in->set_p(10, 0x12345678);
  phv_in->set_p(11, 0x87654321);
  phv_in->set_p(20, 0xABCDEFAB);
  
     
  
  // Setup snapshot regs using TestUtil helper funcs
  int snap_state = 1; // Armed 
  tu.set_phv_match(10, true, 0x12345678, 0xFFFFFFFF);
  tu.set_phv_match(11, true, 0x87654321, 0xFFFFFFFF);
  tu.set_phv_capture(20, true);
  // Capture when phv[200]=5
  tu.set_phv_match(200, true, 5, 0xFFFF);  
  tu.set_phv_capture(201, true);  
  (void)tu.setup_snapshot(true, snap_state); // Ingress snapshot Armed
  
  
  // Send in many PHVs using TestUtil::port_process_inbound
  // We send in lots of PHVs with phv[200]=5 (i=5,15,25,35...)
  // but we should only capture the first one where phv[201]=5 too
  for (int i = 0; i < 1000; i++) {
    phv_in->clobber_p(200, i % 10);
    phv_in->clobber_p(201, i);
    //printf("\nPHV#%d phv[200]=%d phv[201]=%d\n", i, phv_in->get_p(200), phv_in->get_p(201));
    Phv *phv_out = tu.port_process_inbound(port, phv_in);
    tu.phv_free(phv_out);
  }
  
  // Read back snapshot - check we got info from the PHV we expect (#5)
  snap_state = tu.setup_snapshot(true, -1);
  EXPECT_EQ(3, snap_state);  // Should be Full
  uint32_t word10  = static_cast<uint32_t>(tu.get_phv_capture_word(10)  & mask32);
  uint32_t word11  = static_cast<uint32_t>(tu.get_phv_capture_word(11)  & mask32);
  uint32_t word20  = static_cast<uint32_t>(tu.get_phv_capture_word(20)  & mask32);
  uint32_t word200 = static_cast<uint32_t>(tu.get_phv_capture_word(200) & mask16);
  uint32_t word201 = static_cast<uint32_t>(tu.get_phv_capture_word(201) & mask16);
  EXPECT_EQ(0x12345678u, word10);
  EXPECT_EQ(0x87654321u, word11);
  EXPECT_EQ(0xABCDEFABu, word20);
  EXPECT_EQ(5u, word200);
  EXPECT_EQ(5u, word201);
  
  
  
  // Schtum
  tu.finish_test();
  tu.quieten_log_flags();
}



TEST(BFN_TEST_NAME(SnapshotTest),TimedCapture) {
  if (snapshot_print) RMT_UT_LOG_INFO("test_snapshot_timed_capture()\n");    
  int chip = 202; // Just the 2 stages
  int pipe = 0;
  int stage = 0;
  
  // Create our TestUtil class
  // Instantiating new TestUtil obj should free all existing
  // RmtObjectManagers (if this has not already occurred) then
  // recreate a RmtObjectManager just for chip
  TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
  //auto& mm_regs = RegisterUtils::ref_mau(pipe,stage).rams.match.merge;
  

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


  // Instantiate whole chip and fish out objmgr
  RmtObjectManager *om = tu.get_objmgr();
  ASSERT_TRUE(om != NULL);
  // Lookup this Pipe/Stage MAU and MAU_SNAPSHOT obj
  Mau *mau = om->mau_lookup(pipe, stage);
  ASSERT_TRUE(mau != NULL);
  // Setup stage default regs
  tu.set_stage_default_regs(0);
  tu.set_stage_default_regs(1);
  // Lookup some port
  Port *port = tu.port_get(16);
  ASSERT_TRUE(port != NULL);
  MauSnapshot *mausnap = mau->mau_snapshot();
  ASSERT_TRUE(mausnap != NULL);
  uint64_t mask32 = UINT64_C(0xFFFFFFFF);
  uint64_t mask16 = UINT64_C(0xFFFF);


  // Setup a Phv for ingress (use set_p again)
  Phv *phv_ing = tu.phv_alloc();
  phv_ing->set_ingress();
  phv_ing->set_p(10, 0x12345678);
  phv_ing->set_p(11, 0x87654321);
  phv_ing->set_p(20, 0xABCDEFAB);
  phv_ing->set_p(200, 5);
  phv_ing->set_p(201, 5);
  // And a Phv for egress
  Phv *phv_egr = tu.phv_alloc();
  phv_egr->set_egress();
  
  
  // Setup snapshot regs using TestUtil helper funcs

  // First of all setup for timed capture of ingress PHV
  uint64_t ts = UINT64_C(123456789); // 123.45 usecs
  uint64_t ts_trigger = ts/MauDefs::kOneCyclePicosecs;
  tu.set_capture_timestamp(true, ts_trigger);

  // Now setup PHV words 10,11,20,200,201 in ingress thread
  // Map them first as set_phv_ingress_thread assumes mapped words
  tu.set_phv_ingress_egress(RmtDefs::map_mausnap_phv_index( 10), true);
  tu.set_phv_ingress_egress(RmtDefs::map_mausnap_phv_index( 11), true);
  tu.set_phv_ingress_egress(RmtDefs::map_mausnap_phv_index( 20), true);
  tu.set_phv_ingress_egress(RmtDefs::map_mausnap_phv_index(200), true);
  tu.set_phv_ingress_egress(RmtDefs::map_mausnap_phv_index(201), true);  

  // Finish setup for timed capture ingress thread
  int snap_state = 1; // Armed 
  (void)tu.setup_snapshot(true, snap_state); // Ingress snapshot Armed
  
  
  // Now send in many *egress* PHVs using TestUtil::port_process_inbound
  // moving on time as we go.
  // Should see capture_timestamp increase - but NO capture should occur
  uint64_t ts_old = UINT64_C(0), ts_new = UINT64_C(0);
  for (int i = 0; i < 200; i++) {
    om->time_increment(UINT64_C(1000000)); // Move time on 1 usec
    Phv *phv_out = tu.port_process_inbound(port, phv_egr);
    tu.phv_free(phv_out);

    ts_new = tu.get_capture_timestamp_now(true);
    EXPECT_GE(ts_new, ts_old);     // Time should not go backwards
    EXPECT_LE(ts_new, ts_trigger); // Time should not move past trigger
    //printf("TSold=%" PRId64 " TSnew=%" PRId64 "\n", ts_old, ts_new);
    ts_old = ts_new;
    snap_state = tu.setup_snapshot(true, -1);
    EXPECT_EQ(1, snap_state);  // Should be still Armed
  }

  // Now send in an ingress PHV - capture should happen
  om->time_increment(UINT64_C(1000000)); // Move time on 1 usec
  (void)tu.port_process_inbound(port, phv_ing);
  ts_new = tu.get_capture_timestamp_now(true);
  //printf("TSold=%" PRId64 " TSnew=%" PRId64 "\n", ts_old, ts_new);
  EXPECT_GE(ts_new, ts_old);     // Time should not go backwards
  EXPECT_LE(ts_new, ts_trigger); // Time should not move past trigger
  ts_old = ts_new;
  snap_state = tu.setup_snapshot(true, -1);
  EXPECT_EQ(3, snap_state);  // Should be Full

  // Finally put in more egress PHVs
  for (int j = 0; j < 200; j++) {
    om->time_increment(UINT64_C(1000000)); // Move time on 1 usec
    Phv *phv_out = tu.port_process_inbound(port, phv_egr);
    tu.phv_free(phv_out);

    ts_new = tu.get_capture_timestamp_now(true);
    EXPECT_GE(ts_new, ts_old);     // Time should not go backwards
    EXPECT_LE(ts_new, ts_trigger); // Time should not move past trigger
    //printf("TSold=%" PRId64 " TSnew=%" PRId64 "\n", ts_old, ts_new);
    ts_old = ts_new;
    snap_state = tu.setup_snapshot(true, -1);
    EXPECT_EQ(3, snap_state);  // Should be still Full
  }
  

  // And check capture words what we expect - should be stuff from ingress PHV
  uint32_t word10  = static_cast<uint32_t>(tu.get_phv_capture_word(10)  & mask32);
  uint32_t word11  = static_cast<uint32_t>(tu.get_phv_capture_word(11)  & mask32);
  uint32_t word20  = static_cast<uint32_t>(tu.get_phv_capture_word(20)  & mask32);
  uint32_t word200 = static_cast<uint32_t>(tu.get_phv_capture_word(200) & mask16);
  uint32_t word201 = static_cast<uint32_t>(tu.get_phv_capture_word(201) & mask16);
  EXPECT_EQ(0x12345678u, word10);
  EXPECT_EQ(0x87654321u, word11);
  EXPECT_EQ(0xABCDEFABu, word20);
  EXPECT_EQ(5u, word200);
  EXPECT_EQ(5u, word201);
  
  
  // Schtum
  tu.finish_test();
  tu.quieten_log_flags();
}



TEST(BFN_TEST_NAME(SnapshotTest),CanCaptureOnAnyField) {
  if (snapshot_print) RMT_UT_LOG_INFO("test_snapshot_can_capture_on_any_field()\n");    
  int chip = 202; // Just the 2 stages
  int pipe = 0;
  int stage = 0;
  
  // Create our TestUtil class
  // Instantiating new TestUtil obj should free all existing
  // RmtObjectManagers (if this has not already occurred) then
  // recreate a RmtObjectManager just for chip
  TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
  //auto& mm_regs = RegisterUtils::ref_mau(pipe,stage).rams.match.merge;
  

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


  // Instantiate whole chip and fish out objmgr
  RmtObjectManager *om = tu.get_objmgr();
  ASSERT_TRUE(om != NULL);
  // Lookup this Pipe/Stage MAU and MAU_SNAPSHOT obj
  Mau *mau = om->mau_lookup(pipe, stage);
  ASSERT_TRUE(mau != NULL);
  // Setup stage default regs
  tu.set_stage_default_regs(0);
  tu.set_stage_default_regs(1);
  // Lookup some port
  Port *port = tu.port_get(16);
  ASSERT_TRUE(port != NULL);
  MauSnapshot *mausnap = mau->mau_snapshot();
  ASSERT_TRUE(mausnap != NULL);
  //uint64_t mask32 = UINT64_C(0xFFFFFFFF);
  //uint64_t mask16 = UINT64_C(0xFFFF);
  uint64_t mask8 = UINT64_C(0xFF);


  
  // Setup a phv that will never match as every word = 255
  Phv *phv_FF = tu.phv_alloc();
  phv_FF->set_ingress();
  for (int word = 0; word < 224; word++) phv_FF->set_p(word, 0xFFu);

  
  // Now loop setting up PHVs with a single word set
  // and then see if we can always capture using that word only
  for (unsigned int word = 0; word < 224; word++) {
    
    // Setup a Phv with just a single word set to value word
    Phv *phv_in = tu.phv_alloc();
    phv_in->set_ingress();
    phv_in->set_p(word, word);
  
    // Setup snapshot regs using TestUtil helper funcs
    // setting up to match just on field word having value 'word'
    tu.set_phv_match(word, true, word, 0xFFFFFFFF);
    (void)tu.setup_snapshot(true, 1); // Ingress snapshot Armed
  
    // Now send in non-matching phv_FF a few times
    for (int i = 0; i < 10; i++) {
      Phv *phv_tmp = tu.port_process_inbound(port, phv_FF);
      if (phv_tmp != phv_FF) tu.phv_free(phv_tmp);
    }
    // State should still be Armed
    EXPECT_EQ(1, tu.setup_snapshot(true, -1));

    // Now send in matching phv
    Phv *phv_out = tu.port_process_inbound(port, phv_in);    
    if (phv_out != phv_in) tu.phv_free(phv_out);

    // State should now be Full
    EXPECT_EQ(3, tu.setup_snapshot(true, -1));
    // And captured word should have value 'word'
    uint32_t wA = static_cast<uint32_t>(tu.get_phv_capture_word(word) & mask8);
    EXPECT_EQ(word, wA);
    
    // Send in more non-matching phv_FF 
    for (int i = 0; i < 10; i++) {
      Phv *phv_tmp = tu.port_process_inbound(port, phv_FF);
      if (phv_tmp != phv_FF) tu.phv_free(phv_tmp);
    }

    // State should still be Full
    EXPECT_EQ(3, tu.setup_snapshot(true, -1));
    // And captured word 'word' should still have value word
    uint32_t wB = static_cast<uint32_t>(tu.get_phv_capture_word(word) & mask8);
    EXPECT_EQ(word, wB);

    // Stop matching on word
    tu.set_phv_match(word, true, 0u, 0u);

    // Free original phv
    tu.phv_free(phv_in);    
    // Then loop

  } // for

  
  // Schtum
  tu.phv_free(phv_FF);    
  tu.finish_test();
  tu.quieten_log_flags();
}



TEST(BFN_TEST_NAME(SnapshotTest),MultiStage) {
  if (snapshot_print) RMT_UT_LOG_INFO("test_snapshot_multi_stage()\n");    
  int chip = 7; // ALL stages
  int pipe = 0;
  int stage = 0;
  
  // Create our TestUtil class
  // Instantiating new TestUtil obj should free all existing
  // RmtObjectManagers (if this has not already occurred) then
  // recreate a RmtObjectManager just for chip
  TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
  //auto& mm_regs = RegisterUtils::ref_mau(pipe,stage).rams.match.merge;
  

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


  // Instantiate whole chip and fish out objmgr
  RmtObjectManager *om = tu.get_objmgr();
  ASSERT_TRUE(om != NULL);
  // Setup stage default regs
  for (stage = 0; stage < TestUtil::kStagesMax; stage++) tu.set_stage_default_regs(stage);
  // Lookup some port
  Port *port = tu.port_get(16);
  ASSERT_TRUE(port != NULL);
  uint64_t mask32 = UINT64_C(0xFFFFFFFF);
  

  
  // Setup a Phv (use set_p again)
  Phv *phv_in = tu.phv_alloc();
  phv_in->set_ingress();
  phv_in->set_p(10, 0x12345678);
  phv_in->set_p(20, 0xABCDEFAB);

  
  // Setup snapshot regs using TestUtil helper funcs

  // Match on phv[10] - which means we capture phv[10] in Stage0
  // And also capture phv[20] in Stage0
  tu.set_phv_match(pipe, 0, 10, true, 0x12345678, 0xFFFFFFFF);
  tu.set_phv_capture(pipe, 0, 20, true);
  (void)tu.setup_snapshot(pipe, 0, true, 2); // Stage0 Ingress snapshot TriggerHappy

  for (stage = 1; stage <= 5; stage++) {
    // Capture phv[20] in Stages 1-5 - but passive capture triggered by Stage0 match
    // But we don't match on or add phv[10] to ingress thread, so should not be captured
    tu.set_phv_capture(pipe, stage, 20, true);
    (void)tu.setup_snapshot(pipe, stage, true, 0); // Stage1-5 Ingress snapshot Passive
  }
  
  // Send in a matching PHV using TestUtil::port_process_inbound
  (void)tu.port_process_inbound(port, phv_in);
  
  // Read back snapshot - check we got info from PHV we expect
  EXPECT_EQ(3, tu.setup_snapshot(pipe, 0, true, -1));  // Stage0 should be Full
  for (stage = 1; stage <= 5; stage++) {
    EXPECT_EQ(3, tu.setup_snapshot(pipe, stage, true, -1));  // Stage1-5 should be Full
  }
  uint32_t s0w10 = static_cast<uint32_t>(tu.get_phv_capture_word(pipe, 0, 10) & mask32);
  uint32_t s0w20 = static_cast<uint32_t>(tu.get_phv_capture_word(pipe, 0, 20) & mask32);
  EXPECT_EQ(0x12345678u, s0w10); // phv[10] captured
  EXPECT_EQ(0xABCDEFABu, s0w20); // phv[20] also captured
  for (stage = 1; stage <= 5; stage++) {
    auto& snapshot = RegisterUtils::ref_mau(pipe,stage).dp.snapshot_ctl;
    auto a_snap_dp = &snapshot.mau_snapshot_datapath_capture[0]; // Ingress
    uint32_t v_snap_dp = tu.InWord((void*)a_snap_dp);
    EXPECT_EQ((v_snap_dp & 0x1), 0x1u); //  snapshot_from_prev_stage
    EXPECT_EQ((v_snap_dp & 0x8), 0x0u); // !snapshot_from_this_stage
    uint32_t s1w10 = static_cast<uint32_t>(tu.get_phv_capture_word(pipe, stage, 10) & mask32);
    uint32_t s1w20 = static_cast<uint32_t>(tu.get_phv_capture_word(pipe, stage, 20) & mask32);
    EXPECT_EQ(0x00000000u, s1w10); // phv[10] NOT captured in stage1
    EXPECT_EQ(0xABCDEFABu, s1w20); // phv[20] captured ONLY
  }
  
  // Call finish_test - this'll show us snapshot info
  tu.finish_test(999);
  
  // Schtum
  tu.phv_free(phv_in);    
  tu.finish_test();
  tu.quieten_log_flags();
}



}
