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

#include "test_wrapper.h"

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace std;
using namespace MODEL_CHIP_NAMESPACE;


// Utility func to call wrapper with PHV
Phv *wrapper_process_phv(ref_model_wrapper *wrapper, TestUtil& tu,
                         int pipe, int stage, Phv *phv, int port) {

  static uint32_t iphv_cycle = 0u, ophv_cycle = 0u;

  bool     ingress = phv->ingress();
  uint8_t  pktver = (ingress) ?phv->ingress_version() :phv->egress_version();
  uint8_t  hdrport = static_cast<uint8_t>(port & 0x7F);
  int      ing_nxt_tab, egr_nxt_tab, retval;
  uint32_t phvside[4], lpf_id, lpf_active, table_active, action_cnt;
  uint32_t snapshot_data;
  uint32_t mpr_result;
  uint32_t teop_result[4];
  uint64_t lpf_ts, lpf_rng;

  // Not quite sure how big these supposed to be so make them all 512
  uint32_t idata[512], ivalid[512], odata[512], ovalid[512];
  uint32_t nxt_idata[512], nxt_ivalid[512], nxt_odata[512], nxt_ovalid[512];
  uint32_t mresult[512], actdatabus[512], hash[512], gw[512];
  uint32_t ehit[512], emresbus[512], ltcamres[512], hashdist[512], aluio[512];

  // Init all vars and arrays
  ing_nxt_tab = egr_nxt_tab = retval = 0;
  phvside[0] = (hdrport << 8) | (pktver << 4) | (ingress ?0x0 :0x1);
  phvside[1] = 0; phvside[2] = 0; phvside[3] = 0;
  teop_result[0] = 0; teop_result[1]= 0 ; teop_result[2] = 0 ;
  teop_result[3] = 0;
  lpf_id = lpf_active = table_active = action_cnt = 0u;
  lpf_ts = lpf_rng = UINT64_C(0);
  snapshot_data = 0u;
  mpr_result=0u;

  for (int i = 0; i < 512; i++) {
    idata[i] = ivalid[i] = odata[i] = ovalid[i] = 0u;
    nxt_idata[i] = nxt_ivalid[i] = nxt_odata[i] = nxt_ovalid[i] = 0u;
    mresult[i] = actdatabus[i] = hash[i] = gw[i] = 0u;
    ehit[i] = emresbus[i] = ltcamres[i] = hashdist[i] = aluio[i] = 0u;
  }

  // Form iphv and ophv identically
  wrapper->extract_data_from_phv(phv, (phv->ingress() ?0x0 :0x1), idata, ivalid);
  wrapper->extract_data_from_phv(phv, (phv->ingress() ?0x0 :0x1), odata, ovalid);

  // Call process_match process_action via wrapper
  wrapper->mau_model_process_match_n_action(static_cast<uint32_t>(pipe),
                                            static_cast<uint32_t>(stage),
                                            nxt_idata, nxt_ivalid, nxt_odata, nxt_ovalid,
                                            mresult, actdatabus, hash, gw,
                                            ehit, emresbus, ltcamres,
                                            idata, ivalid, odata, ovalid,
                                            phvside, 0, &table_active,
                                            &ing_nxt_tab, &egr_nxt_tab, &action_cnt,
                                            hashdist, aluio,
                                            lpf_active, &lpf_ts, &lpf_rng, lpf_id,
                                            iphv_cycle, ophv_cycle, &snapshot_data, teop_result, &mpr_result,  &retval);
  // Spit out nxt_phv
  Phv *nxt_phv = tu.phv_alloc();
  nxt_phv = wrapper->create_phv_from_data((ingress ?0x0 :0x1), pktver, hdrport, 0,
                                          nxt_odata, nxt_ovalid);

  iphv_cycle++; ophv_cycle++; // Monotonically increase

  return nxt_phv;
}



TEST(BFN_TEST_NAME(WrapperTest),SnapshotViaWrapper) {

  GLOBAL_MODEL->Reset();
  int chip = 0; // wrapper hardcoded to use Chip 0 so don't change
  int pipe = 0;
  int stage = 0;
  int port = 16;

  // Next line resets/inits chip 0 creating new RmtObjectManager
  TestUtil tu(GLOBAL_MODEL.get(), chip);
  tu.set_dv_test(999);

  // Next line resets all chips/inits chip 0 again!
  // So now there's a new RmtObjectManager for chip 0
  ref_model_wrapper *wrapper = ref_model_wrapper::getInstance(true);
  ASSERT_TRUE(wrapper != NULL);

  // Tell TestUtil obj to get hold of new RmtObjectManager
  RmtObjectManager *om = tu.get_objmgr();
  ASSERT_TRUE(om != NULL);
  // Make things quieten down (means Wrapper log flags ignored)
  tu.update_log_flags(ALL, ALL, ALL, ALL, ALL, FEW, FEW);

  // Lookup the Pipe/Stage MAU and MAU_SNAPSHOT obj
  Mau *mau = om->mau_lookup(pipe, stage);
  ASSERT_TRUE(mau != NULL);
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
  // NB. Have to stop at 180 as that maps to 224 and currently
  // the wrapper only copies words 0-223
  for (unsigned int word = 0; word < 180; word++) {

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
      Phv *phv_tmp = wrapper_process_phv(wrapper, tu, pipe, stage, phv_FF, port);
      if (phv_tmp != phv_FF) tu.phv_free(phv_tmp);
    }
    // State should still be Armed
    EXPECT_EQ(1, tu.setup_snapshot(true, -1));

    // Now send in matching phv
    Phv *phv_out =  wrapper_process_phv(wrapper, tu, pipe, stage, phv_in, port);
    if (phv_out != phv_in) tu.phv_free(phv_out);

    // State should now be Full
    EXPECT_EQ(3, tu.setup_snapshot(true, -1));
    // And captured word should have value 'word'
    uint32_t wA = static_cast<uint32_t>(tu.get_phv_capture_word(word) & mask8);
    EXPECT_EQ(word, wA);

    // Send in more non-matching phv_FF
    for (int i = 0; i < 10; i++) {
      Phv *phv_tmp = wrapper_process_phv(wrapper, tu, pipe, stage, phv_FF, port);
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

  ref_model_wrapper::deleteInstance();
  tu.phv_free(phv_FF);
  tu.finish_test();
}

}



