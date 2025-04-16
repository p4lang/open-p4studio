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

#include <model_core/model.h>
#include <bitvector.h>
#include <rmt-defs.h>
#include <rmt-object-manager.h>
#include <mau.h>
#include <rmt-packet-coordinator.h>
#include <packet.h>
#include <mirror.h>
#include "input_xbar_util.h"
#include "tcam_row_vh_util.h"

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;

  bool     mirror_print = false;
  bool     mirror_stress_print = true;
  bool     mirror_stress_norm_print = false;
  bool     mirror_stress_debug = false;
  bool     mirror_stress_data_debug = false;
  uint32_t coal_baseid = RmtDefs::kMirrorNormalSessions - RmtDefs::kMirrorCoalSessions;
  uint8_t  jbay_seq = 0;


  bool test_check_coal_mirrored_pkt(uint32_t i, Packet *mpkt)
  {
    // check pkt len
    bool     jbay = RmtObject::is_jbay_or_later();
    uint32_t pkt_len = mpkt->len();

    std::array<uint8_t, 1500> buf;
    mpkt->get_buf((uint8_t *)&buf[0], 0, pkt_len);
    uint8_t num_samples = buf[jbay?1:0]; // JBay uses Byte1!
    if (num_samples == 0) { return false; }

    uint32_t header_len = (jbay) ?(8 + (i%3)*4) :(4 + (i%4)*4);
    uint32_t sample_len = (((i%3)==1) ?(64 + (i%4)*4) :80);
    uint32_t exp_len = header_len + (sample_len * num_samples) + 4; //4=FCS
    if (pkt_len != exp_len) { return false; }

    // check the header added by MB
    uint32_t tof_hdr0  = (num_samples << 24) | 0x00223344;
    uint32_t tof_hdr1  =                       0x55667788;
    uint32_t jbay_hdr0 = (num_samples << 16) | i;
    uint32_t jbay_hdr1 = (0x55667700 | jbay_seq++);
    std::array<uint8_t, 32> exp_header;
    uint32_t *bp = (uint32_t *)&exp_header[0];
    bp[0] =  __builtin_bswap32( (jbay) ?jbay_hdr0 :tof_hdr0 );
    bp[1] =  __builtin_bswap32( (jbay) ?jbay_hdr1 :tof_hdr1 );
    bp[2] =  __builtin_bswap32( 0x99AABBCC );
    bp[3] =  __builtin_bswap32( 0xDDEEFF00+(i%256) );
    //exp_header[jbay?1:0] = num_samples; // this is already checked so ok to use received val
    for (int b=0; b<(int)(header_len); b++) {
        if (buf[b] != exp_header[b]) {
            return false;
        }
    }
    // check payload from each sample
    //int sample = 0;
    bp = (uint32_t *)&buf[header_len];
    for (int b=0; b<num_samples; b++) {
        for (int s=0; s<(int)sample_len/4; s++) {
            if (bp[s] != (uint32_t)(i + (b*8))) {
                if (mirror_print) {
                    RMT_UT_LOG_INFO("Mirror pkt payload error at %d: %x != %x",
                              b+s, (i+s*8), bp[s]);
                }
                return false;
            }
        }
        bp += (sample_len/4);
    }
    return true;
  }
  void test_check_mirrored_pkt(uint32_t i, Packet *mpkt,
                               bool invert_i_h2)
  {
    // set up expectations...
    uint32_t egress_uc_port = i % 64;
    if (RmtObject::is_chip1()) egress_uc_port |= ((i % 4) << 9);  // add die id bits
    uint32_t icos  = 0x7 & ((invert_i_h2) ?(~(i % 8)) :(i % 8));
    uint32_t hash1 = (0x3AFCC00 + i) & 0x1FFF;
    uint32_t hash2 = 0x1FFF & ((invert_i_h2) ?(~((i * 727) + 929)) :((i * 727) + 929));
    uint32_t mgid1 = 0xAC00 + i;
    uint32_t mgid2 = 0xDC00 + i;
    //uint32_t max_pkt_len = ((i*64) % 1024) + 128;
    uint32_t version = i % 4;
    uint32_t cpu_needs_copy = ((i%16) >> 3) & 0x01;
    uint32_t cpu_cos = (i%16) & 0x07;
    uint32_t sid = mpkt->mirror_metadata()->mirror_id();

    // egress die id = i%4; my die id defaults to 0, so for unicast expect
    // tm_vec bit 0 set for egress die id 0 or 1, tm_vec bit 1 set for egress
    // die id 2 or 3;
    uint8_t expected_uc_tm_vec = ((i % 4) < 2) ? 0x1 : 0x2;
    // mirr session multicast tm vec is set to (i/10)%16
    uint8_t expected_mc_tm_vec = 0;
    expected_mc_tm_vec |= (((i/10) % 16) & 0x3) ? 0x1 : 0;
    expected_mc_tm_vec |= (((i/10) % 16) & 0xc) ? 0x2 : 0;
    // mirr c2c tm vec is set to (i/4)%16
    uint8_t expected_c2c_tm_vec = 0;
    if (((i%16) >> 3) & 0x1) { // c2c valid bit set
      expected_c2c_tm_vec |= (((i/4) % 16) & 0x3) ? 0x1 : 0;
      expected_c2c_tm_vec |= (((i/4) % 16) & 0xc) ? 0x2 : 0;
    }
    uint8_t expected_tm_vec = (expected_uc_tm_vec |
                               expected_mc_tm_vec |
                               expected_c2c_tm_vec);
    expected_tm_vec = RmtObject::is_chip1() ? expected_tm_vec : 0;

    I2QueueingMetadata *i2q = mpkt->i2qing_metadata();
    EXPECT_EQ(expected_tm_vec, i2q->tm_vec());
    EXPECT_EQ(egress_uc_port, i2q->egress_uc_port());
    EXPECT_EQ(version, i2q->version());
    EXPECT_EQ(i2q->icos(), icos);
    EXPECT_EQ(i2q->cpu_needs_copy(), cpu_needs_copy);
    // EXPECT_EQ(i2q->needs_mc_copy(),  needs_mc_copy);
    EXPECT_TRUE(!i2q->cpu_needs_copy() || i2q->cpu_cos() == cpu_cos);
    EXPECT_EQ(i2q->egress_uc_port(), egress_uc_port);
    EXPECT_TRUE(i2q->has_mgid1());
    EXPECT_TRUE(i2q->has_mgid2());
    EXPECT_EQ(i2q->mgid1(), mgid1);
    EXPECT_EQ(i2q->mgid2(), mgid2);
    EXPECT_EQ(i2q->hash1(), hash1);
    EXPECT_EQ(i2q->hash2(), hash2);
    // EXPECT_EQ(i2q->meter_color(), meter_color);
    uint16_t expected_pipe_mask = 0x1;
    EXPECT_EQ(expected_pipe_mask, i2q->pipe_mask());
    if (!::testing::Test::HasNonfatalFailure())
    {
        int pkt_len = mpkt->len();
        std::array<uint8_t, 1500> buf;
        mpkt->get_buf((uint8_t *)&buf[0], 0, pkt_len);
        if (RmtObject::is_jbayA0()) {
          // XXX: JBayA0: extra 4 bytes of FCS (zeros) are added to
          // original pkt len
          for (int b = (pkt_len - 4); b < pkt_len; b++) {
            if (buf[b] != 0u) {
              RMT_UT_LOG_INFO("Mirror pkt extra CRC error at %d: %x != %x",
                              b, buf[b], 0);
              ADD_FAILURE() << "Unexpected payload int " << buf[b] << " at " << b << "/" << pkt_len/4;
            }
          }
          pkt_len -= 4;
        }
        // check payload
        int *bp = (int *)&buf[0];
        for (int b=0; b<pkt_len/4; b++) {
            if (bp[b] != (int)sid) {
                if (mirror_print) {
                    RMT_UT_LOG_INFO("Mirror pkt payload error at %d: %x != %x",
                              b, sid, bp[b]);
                }
                ADD_FAILURE() << "Unexpected payload int " << bp[b] << " at " << b << "/" << pkt_len/4;
            }
        }
    }
  }

  bool is_coal_session(uint16_t sid) {
    return (sid & ~(RmtDefs::kMirrorCoalSessions - 1)) == coal_baseid;
  }


#if MCN_TEST(MODEL_CHIP_NAMESPACE,tofino) || MCN_TEST(MODEL_CHIP_NAMESPACE,tofinoB0)
  TEST(BFN_TEST_NAME(MirrorTest), UsingTofinoCSRs) {
    int chip = 0;
    int pipe = 0;
    int stage = 0;
    GLOBAL_MODEL->Reset();
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);

    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    Mirror *mb0 = om->mirror_lookup(pipe);  // port 0
    Mir_buf_all *regs0 = tu.mirror_buf_reg_get(pipe);

    // stop pkt coordinator so that any packets queued by mirroring logic
    // are not processed by pkt_coordinator
    om->packet_coordinator_get()->stop();

    // global enable
    uint32_t    csr = 0;
    setp_mir_buf_regs_glb_ctrl_ingr_ena(&csr, 1);
    setp_mir_buf_regs_glb_ctrl_egr_ena(&csr, 1);
    setp_mir_buf_regs_glb_ctrl_coalescing_ena(&csr, 1);
    // XXX negative mirroring is not turned on yet - TODO
    // setp_mir_buf_regs_glb_ctrl_neg_mirror_ena(&csr, 1);
    tu.OutWord(&regs0->mir_buf_regs.mir_glb_group.glb_ctrl, csr);

    // baseid for coal sessions
    csr = 0;
    setp_mir_buf_regs_coalescing_baseid_coal_sid(&csr, coal_baseid);
    tu.OutWord(&regs0->mir_buf_regs.mir_glb_group.coalescing_baseid, csr);
    // set timeout to 10
    csr = 0;
    setp_mir_buf_regs_coalescing_basetime_coal_basetime(&csr, 1);
    tu.OutWord(&regs0->mir_buf_regs.mir_glb_group.coalescing_basetime, csr);

    // sessionid %3 are setup as 0 = ingress, 1 = egress, 2 = disabled
    uint32_t meta0, meta1, meta2, meta3, meta4;
    uint32_t egress_port, icos, hash1, hash2, mgid1, mgid2;
    uint32_t mgid1_v, mgid2_v, c2c_v, c2c_cos, deflect;
    uint32_t max_pkt_len;
    bool ingress, egress;
    // Configuration
    if (mirror_print) {
        RMT_UT_LOG_INFO("Mirror session Config...\n");
    }
    for (int i=0; i<RmtDefs::kMirrorNormalSessions; i++) {
        csr = 0;
        meta0 = meta1 = meta2 = meta3 = meta4 = 0;
        egress_port = i % 64;
        icos = i % 8;
        hash1 = (0x3AFCC00 + i) & 0x1FFF;
        hash2 = ((i * 727) + 929) & 0x1FFF;
        mgid1 = 0xAC00 + i;
        mgid2 = 0xDC00 + i;
        mgid1_v = mgid2_v = 1;
        max_pkt_len = ((i*64) % 1024) + 128;
        c2c_v = ((i%16) >> 3) & 0x1;
        c2c_cos = (i%16) & 0x7;
        deflect = 0;

        // XXX make a struct of all metadata
        tu.mir_session_set_metadata(meta0, meta1, meta2, meta3, meta4,
                                 egress_port, icos,
                                 0x01,  /* pipe vector */
                                 0x03,  /* color */
                                 hash1, hash2,
                                 mgid1, mgid1_v, mgid2, mgid2_v,
                                 c2c_v, c2c_cos, deflect
                                 );

        tu.OutWord(&regs0->mir_buf_desc.norm_desc_grp[i].session_meta0, meta0);
        tu.OutWord(&regs0->mir_buf_desc.norm_desc_grp[i].session_meta1, meta1);
        tu.OutWord(&regs0->mir_buf_desc.norm_desc_grp[i].session_meta2, meta2);
        tu.OutWord(&regs0->mir_buf_desc.norm_desc_grp[i].session_meta3, meta3);
        tu.OutWord(&regs0->mir_buf_desc.norm_desc_grp[i].session_meta4, meta4);

        setp_mir_buf_desc_session_ctrl_norm_trunc_size(&csr, max_pkt_len);

        if (is_coal_session(i)) {
            switch(i%3) {
                case 0:
                case 1:
                    // egress en
                    setp_mir_buf_desc_session_ctrl_norm_egr_ena(&csr, 1);
                    break;
                case 2:
                    // disabled
                    break;
            }
        } else {
            switch (i%3) {
                case 0:
                    // ingress en
                    // XXX check for the coalescing ids.. or test error cases
                    setp_mir_buf_desc_session_ctrl_norm_ingr_ena(&csr, 1);
                    break;
                case 1:
                    // egress en
                    setp_mir_buf_desc_session_ctrl_norm_egr_ena(&csr, 1);
                    break;
                case 2:
                    // disabled
                    break;
            }
        }
        tu.OutWord(&regs0->mir_buf_desc.norm_desc_grp[i].session_ctrl, csr);
        csr = 0;
        // program coal descriptors
        if (is_coal_session(i)) {
            uint32_t csr0, csr1, ver;
            csr0 = csr1 = 0;
            ver = i%4;
            tu.OutWord(&regs0->mir_buf_regs.coal_desc_grp[i-coal_baseid].coal_pkt_header0,
                       0x11223344);
            tu.OutWord(&regs0->mir_buf_regs.coal_desc_grp[i-coal_baseid].coal_pkt_header1,
                       0x55667788);
            tu.OutWord(&regs0->mir_buf_regs.coal_desc_grp[i-coal_baseid].coal_pkt_header2,
                       0x99AABBCC);
            tu.OutWord(&regs0->mir_buf_regs.coal_desc_grp[i-coal_baseid].coal_pkt_header3,
                       0xDDEEFF00+(i%256));
            switch (i%3) {
            // with this logic coal idx 1,4,7 are using extract_len from dprsr
            // coal idx 2,5 are using extract_len from reg, 0,3,6 are disabled
                case 0:
                    setp_mir_buf_regs_coal_ctrl0_coal_ena(&csr0, 1);
                    setp_mir_buf_regs_coal_ctrl0_coal_minpkt_size(&csr0, 200);
                    // use coal len from deparser
                    setp_mir_buf_regs_coal_ctrl1_coal_sflow_type(&csr1, 1);
                    break;
                case 1:
                    setp_mir_buf_regs_coal_ctrl0_coal_ena(&csr0, 1);
                    setp_mir_buf_regs_coal_ctrl0_coal_minpkt_size(&csr0, 220);
                    // use coal len from the register
                    setp_mir_buf_regs_coal_ctrl1_coal_extract_length(&csr1, 64 + (i%4)*4);
                    break;
                case 2:
                    // disabled
                    break;
            }
            setp_mir_buf_regs_coal_ctrl0_coal_vid(&csr0, ver);
            setp_mir_buf_regs_coal_ctrl0_coal_timeout(&csr0, 1+(i%8));
            // coal pkt header len upto 16B (with 1B(num_samples)) = 128b
            // hw only allows for header len multiple of 4B (upto 16)
            setp_mir_buf_regs_coal_ctrl0_coal_pkthdr_length(&csr0, 4 + (i%4)*4);

            tu.OutWord(&regs0->mir_buf_regs.coal_desc_grp[i-coal_baseid].coal_ctrl0, csr0);
            tu.OutWord(&regs0->mir_buf_regs.coal_desc_grp[i-coal_baseid].coal_ctrl1, csr1);
            if (mirror_print) {
                RMT_UT_LOG_INFO("Coal config [%d]\n", i);
                RMT_UT_LOG_INFO("\tvid %d, minpkt %d, timeout %d, hdr_len %d, extract_len %d\n",
                        getp_mir_buf_regs_coal_ctrl0_coal_vid(&csr0),
                        getp_mir_buf_regs_coal_ctrl0_coal_minpkt_size(&csr0),
                        getp_mir_buf_regs_coal_ctrl0_coal_timeout(&csr0),
                        getp_mir_buf_regs_coal_ctrl0_coal_pkthdr_length(&csr0),
                        getp_mir_buf_regs_coal_ctrl1_coal_extract_length(&csr1));
            }
        }
    }
    model_timer::ModelTimerSetTime(0);  // init timer incase it is already used
    // Test
    if (mirror_print) {
        RMT_UT_LOG_INFO("Test Norm Mirror sessions ...\n");
    }
    for (int i=0; i<(int)coal_baseid; i++) {
        uint8_t base_pkt[1500];
        int *bp = (int *)&base_pkt[0];
        for (int b=0; b<(int)sizeof(base_pkt)/4; b++) {
            bp[b] = i;
        }

        max_pkt_len = ((i*64) % 1024) + 128 + 4;    // over the max allowed
        ingress = ((i%3) == 0);
        egress = ((i%3) == 1);

        // create a packet (1: pkt.len <= trunc len, 2: > trunc_len
        Packet *pkt = om->pkt_create(base_pkt, max_pkt_len);
        // set session_ids in the mirror_meta
        MirrorMetadata  *mirror_metadata = pkt->mirror_metadata();
        mirror_metadata->set_mirror_id(i);
        mirror_metadata->set_version(i%4);
        mirror_metadata->set_coal_len(0);

        Packet *mpkt = nullptr;
        // send the packet to mirror, check output
        mpkt = mb0->ProcessMirrorPacket(pkt, true/*ingress*/);
        // check pkt
        if (ingress) {
            ASSERT_TRUE(mpkt != nullptr);
            test_check_mirrored_pkt(i, mpkt, false);
        } else {
            // no mirrored pkt expected
            ASSERT_TRUE(mpkt == nullptr);
        }
        mpkt = mb0->ProcessMirrorPacket(pkt, false/*egress*/);
        if (egress) {
            ASSERT_TRUE(mpkt != nullptr);
            test_check_mirrored_pkt(i, mpkt, false);
        } else {
            // no mirrored pkt expected
            ASSERT_TRUE(mpkt == nullptr);
        }
        om->pkt_delete(pkt);
    }
    // Test coalescing sessions
    // send multiple packets (3) to reach coal pkt len
    for (int c=0; c<3; c++) {
        for (int i=coal_baseid; i<RmtDefs::kMirrorNormalSessions; i++) {
            uint8_t base_pkt[1500];
            int *bp = (int *)&base_pkt[0];
            for (int b=0; b<(int)sizeof(base_pkt)/4; b++) {
                bp[b] = i + c*8;
            }

            // XXX send some pkts smaller than extract size and some pkts over
            // the extract size
            max_pkt_len = ((i*64) % 1024) + 128 + 4;    // over the max allowed

            // create a packet (1: pkt.len <= trunc len, 2: > trunc_len
            Packet *pkt = om->pkt_create(base_pkt, max_pkt_len);
            // set session_ids in the mirror_meta
            MirrorMetadata  *mirror_metadata = pkt->mirror_metadata();
            mirror_metadata->set_mirror_id(i);
            mirror_metadata->set_version(i%4);
            mirror_metadata->set_coal_len(80/4);    // in terms of words

            Packet *mpkt = nullptr;
            // send the packet to mirror, check output
            mpkt = mb0->ProcessMirrorPacket(pkt, false/*egress*/);

            if (c < 2) {
                assert(mpkt == nullptr);
                EXPECT_EQ(mpkt, nullptr);
            } else {
                if ((i%3) == 0) {
                    ASSERT_TRUE(mpkt != nullptr);
                    EXPECT_EQ(test_check_coal_mirrored_pkt(i, mpkt), true);
                } else {
                  assert(mpkt == nullptr);
                  EXPECT_EQ(mpkt, nullptr);
                }
            }
            if (mpkt == nullptr) {
                om->pkt_delete(pkt);
            }

        }
        if (c < 2) {
            // check active coal sessions
            EXPECT_EQ(mb0->active_coal_sessions(), 5u);
        }
    }
    // half the sessions are still open
    EXPECT_EQ(mb0->active_coal_sessions(), 2u);
    // fire the timer for all
    model_timer::ModelTimerIncrement(10);
    EXPECT_EQ(mb0->active_coal_sessions(), 0u);

    // global disable mirroring
    // test a few packets
  }
#endif // if MCN_TEST(MODEL_CHIP_NAMESPACE,tofino|tofinoB0)


void set_mirror_tm_vec(TestUtil *tu, int pipe, int sess, uint8_t tm_vec) {
#if MCN_TEST(MODEL_CHIP_NAMESPACE, rsvd0)
  assert((pipe >= 0) && (pipe < RmtDefs::kPipesMax));
  assert((sess >= 0) && (sess < RmtDefs::kMirrorNormalSessions));
  auto mir = RegisterUtils::addr_mirbuf(pipe);
  uint32_t w3 = tu->InWord((void*)&mir->mirror.s2p_regs.sess_entry_word3);
  setp_mirr_s2p_sess_cfg_word3_r_tm_vec_f(&w3, tm_vec);
  tu->OutWord(&mir->mirror.s2p_regs.sess_entry_word3, w3);
  // Trigger copy into shadow RAM
  tu->OutWord(&mir->mirror.s2p_sess.tbl0[sess], 1);
#endif
}

  TEST(BFN_TEST_NAME(MirrorTest), UsingGenericShim) {
    bool jbay = RmtObject::is_jbay_or_later();
    int chip = 0;
    int pipe = 0;
    int stage = 0;
    GLOBAL_MODEL->Reset();
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    Queueing::kAllowMissingDies = true;
    jbay_seq = 0;

    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    Port *port0 = tu.port_get(0);
    Mirror *mb0 = om->mirror_lookup(pipe);  // port 0
    unsigned int n_coal_sessions = 0;

    // mb0->set_log_flags(0xFF);

    // stop pkt coordinator so that any packets queued by mirroring logic
    // are not processed by pkt_coordinator
    om->packet_coordinator_get()->stop();


    // global enable - note ingress coalesce NOT supported on Tofino
    // so just don't use ingress coalesce in test
    tu.set_mirror_global(pipe,
                         true, true,       // ing_en, egr_en
                         false, true,      // coal_ing_en, coal_egr_en
                         coal_baseid, 2u); // Use 2 as 1==>disable on JBay

    // sessionid %3 are setup as 0 = ingress, 1 = egress, 2 = disabled
    uint32_t egress_die_id, egress_port, icos, hash1, hash2, mgid1, mgid2;
    uint32_t mgid1_v, mgid2_v, c2c_v, c2c_cos, deflect;
    uint32_t max_pkt_len;
    bool ingress, egress;
    // Configuration
    if (mirror_print) {
        RMT_UT_LOG_INFO("Mirror session Config...\n");
    }
    for (int i=0; i<RmtDefs::kMirrorNormalSessions; i++) {
      egress_die_id = i % 4;
      egress_port = i % 64;
      icos = i % 8;
      hash1 = (0x3AFCC00 + i) & 0x1FFF;
      hash2 = ((i * 727) + 929) & 0x1FFF;
      mgid1 = 0xAC00 + i;
      mgid2 = 0xDC00 + i;
      mgid1_v = mgid2_v = 1;
      max_pkt_len = ((i*64) % 1024) + 128 + i%2;
      c2c_v = ((i%16) >> 3) & 0x1;
      c2c_cos = (i%16) & 0x7;
      deflect = 0;

      tu.set_mirror_meta(pipe, i,
                         egress_port | (egress_die_id << 9),  // egr_port
                         1,                       // egr_port_valid
                         0, icos,                 // eport_qid(?) ,icos,
                         0x1, 0x3,                // pipe_mask,color
                         hash1, hash2,
                         mgid1, mgid1_v,
                         mgid2, mgid2_v,
                         c2c_v, c2c_cos,
                         0u, 0u, 0u, 0u, 0u,      // xid,yid,rid,egress_bypass,yid_tbl_sel
                         deflect);
      set_mirror_tm_vec(&tu, pipe, i, (i/10)%16);  // WIP only
      bool norm_ing_en = false;
      bool norm_egr_en = false;
      bool is_coal = false;

      if (is_coal_session(i)) {
        is_coal = true;
        switch(i%3) {
          case 0:
          case 1:
            // egress en
            norm_egr_en = true;
            break;
          case 2:
            // disabled
            break;
        }
      } else {
        switch (i%3) {
          case 0:
            // ingress en
            // XXX check for the coalescing ids.. or test error cases
            norm_ing_en = true;
            break;
          case 1:
            // egress en
            norm_egr_en = true;
            break;
          case 2:
            // disabled
            break;
        }
      }
      tu.set_mirror_norm_session(pipe, 0, i,               // pipe,slice,norm_sess
                                 norm_ing_en, norm_egr_en,
                                 is_coal, i-coal_baseid,   // tell session if coal
                                 0, 64,                    // pri, max_entries
                                 max_pkt_len);
      // program coal descriptors
      if (is_coal_session(i)) {

        bool     coal_en = false;
        uint16_t coal_pkt_hdr_len = (jbay) ?(8+(i%3)*4) :(4 + (i%4)*4);
        uint16_t coal_minpkt_size = 0;
        uint16_t coal_extract_len = 0;
        bool     coal_len_from_inp = false;
        uint32_t coal_timeout = 1+(i%8);

        uint32_t coal_ver = i%4;
        uint32_t coal_pri = 0;
        uint32_t hdr0 = (jbay) ?0u         :0x11223344; // CompilerFlag | 0 | SessID on JBay
        uint32_t hdr1 = (jbay) ?0x55667700 :0x55667788; // Seqnum on JBay
        uint32_t hdr2 = 0x99AABBCC;
        uint32_t hdr3 = 0xDDEEFF00+(i%256);

        switch (i%3) {
          // with this logic coal idx 1,4,7 are using extract_len from dprsr
          // coal idx 2,5 are using extract_len from reg, 0,3,6 are disabled
          case 0:
            coal_en = true;
            coal_minpkt_size = 200;
            coal_len_from_inp = true;
            break;
          case 1:
            coal_en = true;
            coal_minpkt_size = 220;
            coal_len_from_inp = false;
            coal_extract_len = 64 + (i%4)*4;
            break;
          case 2:
            // disabled
            break;
        }
        if (coal_en) n_coal_sessions++;
        tu.set_mirror_coal_session(pipe, 0, i-coal_baseid,
                                   coal_en, coal_ver, coal_pri,
                                   coal_pkt_hdr_len, coal_minpkt_size,
                                   coal_extract_len,
                                   coal_len_from_inp, true, // tofino mode
                                   hdr0, hdr1, hdr2, hdr3, coal_timeout);

        if (mirror_print) {
          RMT_UT_LOG_INFO("Coal config [%d]\n", i);
          RMT_UT_LOG_INFO("\tvid %d, minpkt %d, timeout %d, hdr_len %d, extract_len %d\n",
                          coal_ver, coal_minpkt_size, coal_timeout, coal_pkt_hdr_len,
                          coal_extract_len);
        }
      }
    }
    model_timer::ModelTimerSetTime(0);  // init timer incase it is already used
    // Test
    if (mirror_print) {
        RMT_UT_LOG_INFO("Test Norm Mirror sessions ...\n");
    }
    for (int i=0; i<(int)coal_baseid; i++) {
      SCOPED_TRACE(i);
      // vary c2c die vec in mirror as test iterates
      mb0->mirror_regs()->set_copy_to_cpu_die_vec((i/4)%16);  // WIP only
      uint8_t base_pkt[1500];
      int *bp = (int *)&base_pkt[0];
      for (int b=0; b<(int)sizeof(base_pkt)/4; b++) {
        bp[b] = i;
      }

      max_pkt_len = ((i*64) % 1024) + 128 + i%2;
      ingress = ((i%3) == 0);
      egress = ((i%3) == 1);
      uint32_t in_pkt_len = max_pkt_len + (i%13) - 4; // In [max-4,max+8]

      // create a packet (1: pkt.len <= trunc len, 2: > trunc_len
      Packet *pkt = om->pkt_create(base_pkt, in_pkt_len);
      pkt->set_port(port0);

      // set session_ids in the mirror_meta
      MirrorMetadata  *mirror_metadata = pkt->mirror_metadata();
      mirror_metadata->set_mirror_id(i);
      mirror_metadata->set_version(i%4);
      mirror_metadata->set_coal_len(0);

      Packet *mpkt = nullptr;
      // send the packet to mirror, check output

      Packet *ingress_mpkt = mb0->ProcessMirrorPacket(pkt, true/*ingress*/);
      // check pkt
      if (ingress) {
        SCOPED_TRACE("ingress");
        ASSERT_TRUE(ingress_mpkt != nullptr);
        mpkt = ingress_mpkt;
        test_check_mirrored_pkt(i, mpkt, false);
      } else {
        // no mirrored pkt expected
        ASSERT_TRUE(ingress_mpkt == nullptr);
      }
      Packet *egress_mpkt = mb0->ProcessMirrorPacket(pkt, false/*egress*/);
      if (egress) {
        SCOPED_TRACE("egress");
        ASSERT_TRUE(egress_mpkt != nullptr);
        mpkt = egress_mpkt;
      } else {
        // no mirrored pkt expected
        ASSERT_TRUE(egress_mpkt == nullptr);
      }

      if (mpkt != nullptr) {
        const int fcs_len = 4;
        // Packets from deparser to mirror include 4B FCS; mirror preserves
        // the 4B FCS so packets from mirror to deparser (Tofino) or S2P
        // (Jbay) include 4B for FCS. (This is because inbound packets from
        // Ports have the FCS appended already and so it saves the Deparser
        // having to discriminate between packets from Ports and
        // truncated/generated packets from Mirror (or from PktGen). Note, in
        // the RTL, it *is* the Deparser that adds the FCS.)
        // The model mirror logic therefore allows an extra 4B for FCS, in
        // addition to the programmed max mirror packet size, when deciding if
        // mirror packets should be truncated...

        // Line below WAS just max_pkt_len += fcs_len;
        // BUT since XXX, for JBayB0/WIP/later trunc size *includes* 4B FCS already
        // so only add in on TofinoXX, JBayA0
        if (RmtObject::is_tofinoXX() || RmtObject::is_jbayA0()) max_pkt_len += fcs_len;

        // Also for jbay or later the truncation size is rounded down to a 4B boundary
        if (jbay) max_pkt_len &= ~0x3u;
        bool truncated = in_pkt_len > max_pkt_len;
        test_check_mirrored_pkt(i, mpkt, false);
        uint32_t mpkt_len = mpkt->len();
        uint32_t expected = truncated ? max_pkt_len : in_pkt_len;
        // XXX:: JbayA0 adds an extra 4B FCS
        if (RmtObject::is_jbayA0()) expected += 4;
        if (mirror_print)
          printf("NormalMirror: InLen=%d TruncSz=%d ExpectSz=%d OutLen=%d\n",
                 in_pkt_len, max_pkt_len, expected, mpkt_len);
        EXPECT_EQ(expected, mpkt_len);
      }

      om->pkt_delete(pkt);
    }

    // Test coalescing sessions
    // send multiple packets (3) to reach coal pkt len
    bool min_pkt_size_supported = !jbay;
    for (int c=0; c<3; c++) {
      SCOPED_TRACE(c);
      for (int i=coal_baseid; i<RmtDefs::kMirrorNormalSessions; i++) {
        SCOPED_TRACE(i);
        uint8_t base_pkt[1500];
        int *bp = (int *)&base_pkt[0];
        for (int b=0; b<(int)sizeof(base_pkt)/4; b++) {
          bp[b] = i + c*8;
        }

        // XXX send some pkts smaller than extract size and some pkts over
        // the extract size
        max_pkt_len = ((i*64) % 1024) + 128;
        uint32_t in_pkt_len = max_pkt_len + 4; // over the max allowed

        // create a packet (1: pkt.len <= trunc len, 2: > trunc_len
        Packet *pkt = om->pkt_create(base_pkt, in_pkt_len);
        pkt->set_port(port0);

        // set session_ids in the mirror_meta
        MirrorMetadata  *mirror_metadata = pkt->mirror_metadata();
        mirror_metadata->set_mirror_id(i);
        mirror_metadata->set_version(i%4);
        mirror_metadata->set_coal_len(80/4);    // in terms of words

        Packet *mpkt = nullptr;
        // send the packet to mirror, check output
        mpkt = mb0->ProcessMirrorPacket(pkt, false/*egress*/);
        //printf("*************** NewCoalSess=%d NCoalSessions=%d "
        //       "ActiveCoalSessions=%d c=%d ***************\n",
        //       i, n_coal_sessions, mb0->active_coal_sessions(), c);

        EXPECT_GT(n_coal_sessions, 0u);
        if (mpkt != nullptr) n_coal_sessions--;

        if (c < 2) {
          if (min_pkt_size_supported) {
            EXPECT_EQ(mpkt, nullptr);
          }
        } else {
          if ((i%3) == 0) {
            if (mpkt != nullptr) {
              EXPECT_EQ(test_check_coal_mirrored_pkt(i, mpkt), true);
            }
          } else {
            if (min_pkt_size_supported) {
              EXPECT_EQ(mpkt, nullptr);
            }
          }
        }
        if (mpkt == nullptr) {
          om->pkt_delete(pkt);
        }

      }
      if (c < 2) {
        // check active coal sessions
        EXPECT_EQ(mb0->active_coal_sessions(), n_coal_sessions);
      }
    }
    // some sessions are still open
    EXPECT_GT(mb0->active_coal_sessions(), 0u);
    // fire the timer a couple of times
    model_timer::ModelTimerIncrement(10);
    model_timer::ModelTimerIncrement(10);
    EXPECT_EQ(mb0->active_coal_sessions(), 0u);

    // global disable mirroring
    // test a few packets
  }



#ifdef MODEL_CHIP_JBAY_OR_LATER
  TEST(BFN_TEST_NAME(MirrorTest), ConfigFromMirrorMetadata) {

    // Basically the same test as UseGenericShim above.
    // But now we configure I2QMetadata to be set from MirrorMetadata
    // We put 'normal' icos/hash2 cfg into Session CSRs
    // - but every 5th session we configure use of MirrorMetadata
    // We put inverted icos/hash2 into MirrorMetadata
    // - so packets into session 5/10/15 etc should get inverted vals

    ASSERT_TRUE(RmtObject::is_jbay_or_later());
    bool jbay = true;
    int chip = 0;
    int pipe = 0;
    int stage = 0;
    GLOBAL_MODEL->Reset();
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    Queueing::kAllowMissingDies = true;
    jbay_seq = 0;

    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    Port *port0 = tu.port_get(0);
    Mirror *mb0 = om->mirror_lookup(pipe);  // port 0
    unsigned int n_coal_sessions = 0;

    // stop pkt coordinator so that any packets queued by mirroring logic
    // are not processed by pkt_coordinator
    om->packet_coordinator_get()->stop();


    // global enable - note ingress coalesce NOT supported on Tofino
    // so just don't use ingress coalesce in test
    tu.set_mirror_global(pipe,
                         true, true,       // ing_en, egr_en
                         false, true,      // coal_ing_en, coal_egr_en
                         coal_baseid, 2u); // Use 2 as 1==>disable on JBay

    // sessionid %3 are setup as 0 = ingress, 1 = egress, 2 = disabled
    uint32_t egress_die_id, egress_port, icos, hash1, hash2, mgid1, mgid2;
    uint32_t mgid1_v, mgid2_v, c2c_v, c2c_cos, deflect;
    uint32_t max_pkt_len;
    bool ingress, egress;
    // Configuration
    if (mirror_print) {
        RMT_UT_LOG_INFO("Mirror session Config...\n");
    }
    for (int i=0; i<RmtDefs::kMirrorNormalSessions; i++) {
      bool mm_inverted_icos_hash2 = (((i % 5) == 0) && (i < (int)coal_baseid));

      egress_die_id = i % 4;
      egress_port = i % 64;
      icos = i % 8;
      hash1 = (0x3AFCC00 + i) & 0x1FFF;
      hash2 = ((i * 727) + 929) & 0x1FFF;
      mgid1 = 0xAC00 + i;
      mgid2 = 0xDC00 + i;
      mgid1_v = mgid2_v = 1;
      max_pkt_len = ((i*64) % 1024) + 128;
      c2c_v = ((i%16) >> 3) & 0x1;
      c2c_cos = (i%16) & 0x7;
      deflect = 0;

      tu.set_mirror_meta(pipe, i,
                         egress_port | (egress_die_id << 9),  // egr_port
                         1,                       // egr_port_valid
                         0, icos,                 // eport_qid(?) ,icos,
                         0x1, 0x3,                // pipe_mask,color
                         hash1, hash2,
                         mgid1, mgid1_v,
                         mgid2, mgid2_v,
                         c2c_v, c2c_cos,
                         0u, 0u, 0u, 0u, 0u,      // xid,yid,rid,egress_bypass,yid_tbl_sel
                         deflect);
      set_mirror_tm_vec(&tu, pipe, i, (i/10)%16);  // WIP only
      tu.set_mirror_meta_cfg(pipe, i,
                             mm_inverted_icos_hash2,      // icos_cfg maybe true
                             mm_inverted_icos_hash2,      // hash_cfg maybe true
                             false, false, false, false); // all rest false

      bool norm_ing_en = false;
      bool norm_egr_en = false;
      bool is_coal = false;

      if (is_coal_session(i)) {
        is_coal = true;
        switch(i%3) {
          case 0:
          case 1:
            // egress en
            norm_egr_en = true;
            break;
          case 2:
            // disabled
            break;
        }
      } else {
        switch (i%3) {
          case 0:
            // ingress en
            // XXX check for the coalescing ids.. or test error cases
            norm_ing_en = true;
            break;
          case 1:
            // egress en
            norm_egr_en = true;
            break;
          case 2:
            // disabled
            break;
        }
      }
      tu.set_mirror_norm_session(pipe, 0, i,               // pipe,slice,norm_sess
                                 norm_ing_en, norm_egr_en,
                                 is_coal, i-coal_baseid,   // tell session if coal
                                 0, 64,                    // pri, max_entries
                                 max_pkt_len);
      // program coal descriptors
      if (is_coal_session(i)) {

        bool     coal_en = false;
        uint16_t coal_pkt_hdr_len = (jbay) ?(8+(i%3)*4) :(4 + (i%4)*4);
        uint16_t coal_minpkt_size = 0;
        uint16_t coal_extract_len = 0;
        bool     coal_len_from_inp = false;
        uint32_t coal_timeout = 1+(i%8);

        uint32_t coal_ver = i%4;
        uint32_t coal_pri = 0;
        uint32_t hdr0 = (jbay) ?0u         :0x11223344; // CompilerFlag | 0 | SessID on JBay
        uint32_t hdr1 = (jbay) ?0x55667700 :0x55667788; // Seqnum on JBay
        uint32_t hdr2 = 0x99AABBCC;
        uint32_t hdr3 = 0xDDEEFF00+(i%256);

        switch (i%3) {
          // with this logic coal idx 1,4,7 are using extract_len from dprsr
          // coal idx 2,5 are using extract_len from reg, 0,3,6 are disabled
          case 0:
            coal_en = true;
            coal_minpkt_size = 200;
            coal_len_from_inp = true;
            break;
          case 1:
            coal_en = true;
            coal_minpkt_size = 220;
            coal_len_from_inp = false;
            coal_extract_len = 64 + (i%4)*4;
            break;
          case 2:
            // disabled
            break;
        }
        if (coal_en) n_coal_sessions++;
        tu.set_mirror_coal_session(pipe, 0, i-coal_baseid,
                                   coal_en, coal_ver, coal_pri,
                                   coal_pkt_hdr_len, coal_minpkt_size,
                                   coal_extract_len,
                                   coal_len_from_inp, true, // tofino mode
                                   hdr0, hdr1, hdr2, hdr3, coal_timeout);

        if (mirror_print) {
          RMT_UT_LOG_INFO("Coal config [%d]\n", i);
          RMT_UT_LOG_INFO("\tvid %d, minpkt %d, timeout %d, hdr_len %d, extract_len %d\n",
                          coal_ver, coal_minpkt_size, coal_timeout, coal_pkt_hdr_len,
                          coal_extract_len);
        }
      }
    }
    model_timer::ModelTimerSetTime(0);  // init timer incase it is already used
    // Test
    if (mirror_print) {
        RMT_UT_LOG_INFO("Test Norm Mirror sessions ...\n");
    }
    for (int i=0; i<(int)coal_baseid; i++) {
      // vary c2c die vec in mirror as test iterates
      mb0->mirror_regs()->set_copy_to_cpu_die_vec((i/4)%16);  // WIP only

      bool mm_inverted_icos_hash2 = (((i % 5) == 0) && (i < (int)coal_baseid));

      uint8_t base_pkt[1500];
      int *bp = (int *)&base_pkt[0];
      for (int b=0; b<(int)sizeof(base_pkt)/4; b++) {
        bp[b] = i;
      }

      max_pkt_len = ((i*64) % 1024) + 128 + 4;    // over the max allowed
      ingress = ((i%3) == 0);
      egress = ((i%3) == 1);

      // create a packet (1: pkt.len <= trunc len, 2: > trunc_len
      Packet *pkt = om->pkt_create(base_pkt, max_pkt_len);
      pkt->set_port(port0);

      // set session_ids in the mirror_meta
      MirrorMetadata  *mirror_metadata = pkt->mirror_metadata();
      mirror_metadata->set_mirror_id(i);
      mirror_metadata->set_version(i%4);
      mirror_metadata->set_coal_len(0);
      // maybe set inverted icos/hash2 into mirror_metadata
      if (mm_inverted_icos_hash2) {
        uint8_t  icos  = (~(i % 8)) & 0x7;
        uint16_t hash2 = (~((i * 727) + 929)) & 0x1FFF;
        mirror_metadata->set_mirr_hash(hash2);
        mirror_metadata->set_mirr_icos(icos);
      }

      Packet *mpkt = nullptr;
      // send the packet to mirror, check output
      mpkt = mb0->ProcessMirrorPacket(pkt, true/*ingress*/);
      // check pkt
      if (ingress) {
        ASSERT_TRUE(mpkt != nullptr);
        test_check_mirrored_pkt(i, mpkt, mm_inverted_icos_hash2);
      } else {
        // no mirrored pkt expected
        ASSERT_TRUE(mpkt == nullptr);
      }
      mpkt = mb0->ProcessMirrorPacket(pkt, false/*egress*/);
      if (egress) {
        ASSERT_TRUE(mpkt != nullptr);
        test_check_mirrored_pkt(i, mpkt, mm_inverted_icos_hash2);
      } else {
        // no mirrored pkt expected
        ASSERT_TRUE(mpkt == nullptr);
      }
      om->pkt_delete(pkt);
    }

    // Test coalescing sessions
    // send multiple packets (3) to reach coal pkt len
    bool min_pkt_size_supported = !jbay;
    for (int c=0; c<3; c++) {

      for (int i=coal_baseid; i<RmtDefs::kMirrorNormalSessions; i++) {

        uint8_t base_pkt[1500];
        int *bp = (int *)&base_pkt[0];
        for (int b=0; b<(int)sizeof(base_pkt)/4; b++) {
          bp[b] = i + c*8;
        }

        // XXX send some pkts smaller than extract size and some pkts over
        // the extract size
        max_pkt_len = ((i*64) % 1024) + 128 + 4;    // over the max allowed

        // create a packet (1: pkt.len <= trunc len, 2: > trunc_len
        Packet *pkt = om->pkt_create(base_pkt, max_pkt_len);
        pkt->set_port(port0);

        // set session_ids in the mirror_meta
        MirrorMetadata  *mirror_metadata = pkt->mirror_metadata();
        mirror_metadata->set_mirror_id(i);
        mirror_metadata->set_version(i%4);
        mirror_metadata->set_coal_len(80/4);    // in terms of words

        Packet *mpkt = nullptr;
        // send the packet to mirror, check output
        mpkt = mb0->ProcessMirrorPacket(pkt, false/*egress*/);
        //printf("*************** NewCoalSess=%d NCoalSessions=%d "
        //       "ActiveCoalSessions=%d c=%d ***************\n",
        //       i, n_coal_sessions, mb0->active_coal_sessions(), c);

        EXPECT_GT(n_coal_sessions, 0u);
        if (mpkt != nullptr) n_coal_sessions--;

        if (c < 2) {
          if (min_pkt_size_supported) {
            EXPECT_EQ(mpkt, nullptr);
          }
        } else {
          if ((i%3) == 0) {
            if (mpkt != nullptr) {
              EXPECT_EQ(test_check_coal_mirrored_pkt(i, mpkt), true);
            }
          } else {
            if (min_pkt_size_supported) {
              EXPECT_EQ(mpkt, nullptr);
            }
          }
        }
        if (mpkt == nullptr) {
          om->pkt_delete(pkt);
        }

      }
      if (c < 2) {
        // check active coal sessions
        EXPECT_EQ(mb0->active_coal_sessions(), n_coal_sessions);
      }
    }
    // some sessions are still open
    EXPECT_GT(mb0->active_coal_sessions(), 0u);
    // fire the timer a couple of times
    model_timer::ModelTimerIncrement(10);
    model_timer::ModelTimerIncrement(10);
    EXPECT_EQ(mb0->active_coal_sessions(), 0u);

    // global disable mirroring
    // test a few packets
  }



  TEST(BFN_TEST_NAME(MirrorTest), CoalSliceStress) {

    // Purpose of this test is to stress test the JBay
    // COALESCE slicing mechanism. Strategy is:
    //
    // TestUtil::set_mirror_global(coal_basetime==1 ==> disable coal timeout)
    // Enable everything else
    //
    // Loop a lot
    //   Each time setup a new set of 256 mirror sessions
    //     Use a fixed mapping of session -> function
    //     So for sess S S[7:6]=slice S[5]=valid S[4]=Norm(0)/Coal
    //                   S[3]=Ingress/Egress and S[3:0]=coalID(if COAL)
    //   Each session gets a random seed that determines mirr/coal cfg
    //     eg Random trunc_size/extract_len
    //     Limit MAU/Sess CFG random (coal_extract_len/hash1/hash2 ??)
    //     Store config seed in SessCfgSeed[Sess]
    //     Program up config using TestUtil::set_mirror funcs and TU.xrand helpers
    //
    //   Loop for a lot of packets
    //     Each packet gets a new random seed
    //       Seed determines Port and Sess
    //         Enforce slice is SAME in Port and Sess
    //         Install Port object - lookup using om->port_lookup()
    //         Lookup PortSessTab[Port][Sess] to find PktNum (0-63)
    //           Avoid PktNum that's the same as Port[15:8]
    //         Insist any previous usage of PktNum is all done - not still coalescing!
    //       Seed determines packet MirrorMeta
    //       Seed determines packet length
    //         But if using coal_len==0 (whole pkt) limit length to MirrCfg.trunc_len
    //       Seed determines random packet payload (but fix Byte0/4/8 etc == PktNum)
    //       Store packet seed in PortSess.packet_seeds[PktNum]
    //
    //     Call ProcessMirrorPacket
    //       If we get a packet back (not always)
    //         Get Sess from PktHdrBytes[2]&[3]
    //           Lookup sess_seed from SessCfgSeed[Sess]
    //           Recalculate required config vars from sess_seed
    //         PacketLen should be in range TruncLenF(SessSeed),TruncLenF(SessSeed) + 176
    //         Loop through all slices in Packet
    //           Get Port from Slice[0]&[1]
    //           Loop through all words in Slice
    //             Get PktNum from word[0]. Should stay constant through all slice
    //             Lookup PortSess
    //                Is PktNum expected for PortSess??
    //                If first slice
    //                   check I2Q Meta fields matches input packet MirrorMeta
    //                   (regenerate from PortSess.packet_seeds[PktNum])
    //                   copy PortSess.packet_seeds[PktNum] into curr_seed
    //                Check all words in packet match ITER(curr_pkt_seed)
    //                Should hit a new SliceHdr after Slice[2] (rounded to 4B) bytes
    //                Total number of bytes extracted should be
    //                   Min PktLenF(PktSeed),extract_len (0==>WholePacket tho)
    //
    //    Once all packets done
    //      Mirror.set_coal_tx_enable(false)
    //      Call MirrorTimer a few times to empty timer queues

    const int pkts_per_portsess = 191; // MUST be <256

    struct PortSess {
      uint64_t next_pkt_num;
      uint64_t packet_seeds[pkts_per_portsess];
      uint64_t packet_data_last[pkts_per_portsess];
      uint16_t packet_byte_next[pkts_per_portsess];
    };

    ASSERT_TRUE(RmtObject::is_jbay_or_later());
    int chip = 0, pipe = 0, stage = 0;
    GLOBAL_MODEL->Reset();
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    Mirror *mb0 = om->mirror_lookup(pipe);  // port 0
    mb0->set_coal_tx_enable(false); // Set up timer TX to just free pkt
    // Disable P4 logging - too verbose
    uint64_t ALL = UINT64_C(0xFFFFFFFFFFFFFFFF);
    om->update_log_type_levels(ALL, ALL, RMT_LOG_TYPE_P4, ALL, UINT64_C(0));

    // Stop pkt coordinator so that any packets queued by mirroring logic
    // are not processed by pkt_coordinator
    om->packet_coordinator_get()->stop();


    // Define vars to maintain config etc
    //
    const int  num_ports_per_pipe        =     72;
    const int  min_packet_size           =     16;
    const int  max_packet_size           =   9104; // For mirror packets
    const int  min_trunc_size            =     64; // but should always get at least 188
    const int  max_trunc_size            =  16383; // 14b (but will get capped to 16368)
    const int  fcs_size = 4;
    int        num_sessions              = RmtDefs::kMirrorNormalSessions;
    int        num_iterations            =     50;
    int        num_packets_per_iteration =   5000;
    int        tot_packets               = num_iterations * num_packets_per_iteration;
    int        print_modulus             = (tot_packets <= 100000000) ?100 :1000;
    uint64_t   debug_iteration           = UINT64_C(0);  // 0=>rand seed each iteration
    uint64_t   debug_on_packet           = UINT64_C(0);  // 0=>none
    int        debug_slice               =     -1; // -1=>all slices
    int        debug_coal_num            =     -1; // -1=>all coal_nums
    RMT_ASSERT(num_sessions >= RmtDefs::kMirrorCoalSessions);

    int      ErrCnt = 0;
    uint8_t  PktBuf[max_packet_size];

    uint64_t SessCfgSeed[RmtDefs::kMirrorNormalSessions];
    for (int sess = 0; sess < RmtDefs::kMirrorNormalSessions; sess++) SessCfgSeed[sess] = UINT64_C(0);

    std::array< std::array< PortSess*, RmtDefs::kMirrorNormalSessions >, num_ports_per_pipe >  PortSessTab;
    for (int port = 0; port < num_ports_per_pipe; port++) {
      for (int sess = 0; sess < RmtDefs::kMirrorNormalSessions; sess++) {
        PortSessTab[port][sess] = nullptr;
      }
    }



    // Global enable - enable everything
    //
    tu.set_mirror_global(pipe,
                         true, true,  // ing_en, egr_en
                         true, true,  // coal_ing_en, coal_egr_en
                         0, 1u);      // 0,1 used as is otherwise arg-1 stored


    // Setup main random number generation
    //
    std::default_random_engine main_generator;
    std::uniform_int_distribution<uint64_t> loop_seed;
    main_generator.seed( unsigned(0xDAB0D1B0D0B0DEB0) );


    // LOOP FOR LOTS OF ITERATIONS
    //
    for (int iteration = 0; iteration < num_iterations; iteration++) {

      // Setup random number generation for this iteration
      //
      std::default_random_engine loop_generator;
      std::uniform_int_distribution<uint64_t> config_seed;
      std::uniform_int_distribution<uint64_t> packet_seed;
      uint64_t iteration_seed = loop_seed(main_generator);
      if (debug_iteration != UINT64_C(0)) iteration_seed = debug_iteration;
      loop_generator.seed( iteration_seed );

      // 0. GLOBAL CONFIG
      //
      // min_pkt_size - causes mirror to pad
      int min_pkt_len = (RmtObject::is_chip1()) ?((iteration % 48) + 16) :0;
      mb0->mirror_regs()->set_min_pkt_len(min_pkt_len);
      if (min_pkt_len != 0) min_pkt_len -= fcs_size;


      // 1. DISABLE/RESET ALL SESSIONS
      //
      // First off per-slice config
      //
      for (int sess = 0; sess < num_sessions; sess++) {
        SessCfgSeed[sess] = UINT64_C(0);
      }
      for (int slice = 0; slice < RmtDefs::kMirrorSlices; slice++) {
        for (int sess = 0; sess < num_sessions; sess++) {
          tu.set_mirror_norm_session(pipe, slice, sess,
                                     false, false, false, 0, 0, 0, 0);
        }
        for (int sess = 0; sess < RmtDefs::kMirrorCoalSessions; sess++) {
          tu.set_mirror_coal_session(pipe, slice, sess, false,
                                     0, 0, 8, 0, 0, false, false,
                                     0u, 0u, 0u, 0u, 0u);
        }
      }
      // Then session config
      for (int sess = 0; sess < num_sessions; sess++) {
        tu.set_mirror_meta(pipe, sess,
                           0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
                           0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u);
        tu.set_mirror_meta_cfg(pipe, sess,
                               false, false, false, false, false, false);
      }


      // 2. RANDOM CONFIG ALL SESSIONS
      //
      // Structure sessId as follows:
      //   sess S S[7:6]=slice S[5]=valid S[4]=Norm(0)/Coal
      //          S[3]=Ingress/Egress and S[3:0]=coalID(if COAL)
      // So only need to setup valid sessIds
      // And only need to do configure of relevant slice
      //
      for (int sess = 0; sess < num_sessions; sess++) {
        int  slice    =   (sess >> 6) & 3;
        bool invalid  = (((sess >> 5) & 1) == 0);
        bool ing_en   = (((sess >> 3) & 1) == 0), egr_en = !ing_en;
        bool norm_en  = (((sess >> 4) & 1) == 0), coal_en = !norm_en;
        int  coal_num =   (sess >> 0) & 0xF;
        if (invalid) continue;

        // Generate and stash per-config rand val
        uint64_t cfg_seed = config_seed(loop_generator);
        SessCfgSeed[sess] = cfg_seed;

        // Calculate what value we can use for extract_len given our trunc_size.
        // We don't ever want to fill more than one COAL packet as this utest
        // relies on getting packets back synchronously from ProcessMirrorPacket
        // and doesn't cope with them being asynchronously transmitted
        // Also we only track pkts_per_portsess packets. So we MUST extract enough bytes
        // that we never get more samples than this in a single coal packet!

        int  trunc_size = tu.xrandrange(cfg_seed, 5000, min_trunc_size, max_trunc_size);
        trunc_size = (trunc_size / 4) * 4; // Round down to 4B boundary on JBay
        int  pkt_hdr_len = 8 + (4 * tu.xrandrange(cfg_seed, 5001, 0, 2)); // 8,12,16
        int  totsize_headers = pkt_hdr_len + 4 + (4 * (trunc_size / 180));
        int  maxsize_payload = trunc_size - totsize_headers;
        // extract_len(sample_pkt_len) is in 32b words hence divide by 4 (with max 255)
        // to calculate. But multiply by 4 for call to set_mirror_coal_session.
        int  cap_extract_wlen = std::min(255, maxsize_payload/4);
        int  floor_extract_wlen = ((maxsize_payload / pkts_per_portsess)/4);
        RMT_ASSERT(floor_extract_wlen < 255);
        int  extract_len = 4 * tu.xrandrange(cfg_seed, 5006,
                                             floor_extract_wlen, cap_extract_wlen);
        // Allow a special case of extract_len == 0 as this means 'whole packet'.
        if (extract_len == 4*floor_extract_wlen) extract_len = 0;
        bool len_from_inp = tu.xrandbool(cfg_seed, 5009);

        // tu.set_mirror_coal_session *insists* hdr vals 0 unless Slice0
        uint32_t hdr0 = (slice == 0) ?tu.xrand32(cfg_seed, 5010) :0u;
        uint32_t hdr1 = (slice == 0) ?tu.xrand32(cfg_seed, 5011) :0u;
        uint32_t hdr2 = (slice == 0) ?tu.xrand32(cfg_seed, 5012) :0u;
        uint32_t hdr3 = (slice == 0) ?tu.xrand32(cfg_seed, 5013) :0u;

        tu.set_mirror_norm_session(pipe, slice, sess,
                                   ing_en, egr_en, coal_en,
                                   coal_num,                     // coal_num
                                   tu.xrand8(cfg_seed, 5002),    // pri
                                   tu.xrand8(cfg_seed, 5003),    // max_n
                                   trunc_size);                  // trunc_size
        if (coal_en) { // Only configure coalescing if session[4:4] set
          tu.set_mirror_coal_session(pipe, slice, coal_num,
                                     true,                       // enabled
                                     tu.xrand8(cfg_seed, 5004),  // ver
                                     tu.xrand8(cfg_seed, 5002),  // pri (as above)
                                     pkt_hdr_len,                // pkt_hdr_len
                                     0,                          // min_pkt_size (unused)
                                     extract_len,                // extract_len (in bytes)
                                     len_from_inp,               // len_from_inp
                                     false,                      // tofino_mode=false
                                     hdr0,                       // hdr0 (only if Slice0)
                                     hdr1,                       // hdr1 (  "   "   "   )
                                     hdr2,                       // hdr2 (  "   "   "   )
                                     hdr3,                       // hdr3 (  "   "   "   )
                                     0u);                        // coal_timeout
        }
        // NB. *MUST* set one of egr_port_vld/mgid1_vld/mgid2_vld/c2c_vld
        // else packet will NOT be mirrored.
        // For now we set egr_port_vld=true and epipe_cfg=false (to use sess_cfg)
        tu.set_mirror_meta(pipe, sess,
                           tu.xrand32(cfg_seed, 5015),           // egr_port
                           true,                                 // egr_port_vld
                           tu.xrand32(cfg_seed, 5017),           // eport_qid
                           tu.xrand8( cfg_seed, 5018, 3),        // icos
                           tu.xrand32(cfg_seed, 5019),           // pipe_mask
                           tu.xrand32(cfg_seed, 5020),           // color
                           tu.xrand16(cfg_seed, 5021),           // hash1
                           tu.xrand16(cfg_seed, 5022, 13),       // hash2
                           tu.xrand16(cfg_seed, 5023),           // mgid1
                           tu.xrand32(cfg_seed, 5024),           // mgid1_vld
                           tu.xrand16(cfg_seed, 5025),           // mgid2
                           tu.xrand32(cfg_seed, 5026),           // mgid2_vld
                           tu.xrand32(cfg_seed, 5027),           // c2c_vld
                           tu.xrand32(cfg_seed, 5028),           // c2c_cos
                           tu.xrand16(cfg_seed, 5029),           // xid
                           tu.xrand16(cfg_seed, 5030, 9),        // yid
                           tu.xrand16(cfg_seed, 5031),           // rid
                           tu.xrand32(cfg_seed, 5032),           // egress_bypass
                           tu.xrand32(cfg_seed, 5033),           // yid_tbl_sel
                           tu.xrand32(cfg_seed, 5034));          // deflect
        tu.set_mirror_meta_cfg(pipe, sess,
                               tu.xrandbool(cfg_seed, 5035),     // hash_cfg
                               tu.xrandbool(cfg_seed, 5036),     // icos_cfg
                               tu.xrandbool(cfg_seed, 5037),     // dod_cfg
                               tu.xrandbool(cfg_seed, 5038),     // c2c_cfg
                               tu.xrandbool(cfg_seed, 5039),     // mc_cfg
                               false);                           // epipe_cfg

      } // for (int sess = 0; sess < kMaxCoalSessions; sess++)


      // If we're triggering debug on a particular packet switch off initially
      if (debug_on_packet != UINT64_C(0)) {
        mirror_stress_debug = mirror_stress_data_debug = false;
      }


      // 3. LOOP FOR LOTS OF PACKETS
      //
      for (int packet = 0; packet < num_packets_per_iteration; packet++) {

        // Vars we're going to use a lot
        uint32_t w32 = 0u;
        uint64_t w64 = UINT64_C(0);
        int      pos = 0;

        // 3a. CREATE A NEW PACKET
        //
        // Generate per-packet seed val
        // Will use this to seed the packet data word generators
        uint64_t pkt_rand = packet_seed(loop_generator);

        // Maybe switch debug ON (will stay on then forever)
        if (pkt_rand == debug_on_packet) {
          mirror_stress_debug = mirror_stress_data_debug = true;
        }

        // Now synthesize other values from our per-packet seed
        int port = tu.xrandrange(pkt_rand, 5102, 0, num_ports_per_pipe-1);
        Port *port_obj = om->port_lookup(port);
        RMT_ASSERT(port_obj != nullptr);
        int slice = RmtDefs::get_deparser_slice(port); // Mirror uses same
        int sess_tmp = tu.xrandrange(pkt_rand, 5103, 0, num_sessions-1);
        // Clear randomised slice bits, set *port* slice instead. Set valid bit
        int sess = (sess_tmp & 0x3F) | ((slice & 3) << 6) | (1 << 5);
        int coal_num = (sess >> 0) & 0xF;
        // Extract whether ingress/egress/norm/coal etc
        bool ingress = (((sess >> 3) & 1) == 0); //egress = !ingress;
        bool norm_en = (((sess >> 4) & 1) == 0), coal_en = !norm_en;
        bool invalid = (((sess >> 5) & 1) == 0);

        // Do we do debug prints
        bool debug_sess = (((debug_slice < 0) || (debug_slice == slice)) &&
                           ((debug_coal_num < 0) || (debug_coal_num == coal_num)));


        // Look up what config seed we used for this sess and
        // recalculate cfg vals we need to mirror packets
        uint64_t cfg_seed = SessCfgSeed[sess];

        // And calculate variables to let us work out a packet length etc
        int  trunc_size = tu.xrandrange(cfg_seed, 5000, min_trunc_size, max_trunc_size);
        trunc_size = (trunc_size / 4) * 4; // Round down to 4B boundary on JBay
        int  pkt_hdr_len = 8 + (4 * tu.xrandrange(cfg_seed, 5001, 0, 2)); // 8,12,16
        int  totsize_headers = pkt_hdr_len + 4 + (4 * (trunc_size / 180));
        int  maxsize_payload = trunc_size - totsize_headers;
        int  cap_extract_wlen = std::min(255, maxsize_payload/4);
        // We only track pkts_per_portsess packets. So we MUST extract enough bytes
        // that we never get more samples than this in a single coal packet!
        int  floor_extract_wlen = ((maxsize_payload / pkts_per_portsess)/4);
        RMT_ASSERT(floor_extract_wlen < 255);
        int  extract_len = 4 * tu.xrandrange(cfg_seed, 5006,
                                             floor_extract_wlen, cap_extract_wlen);
        // Allow a special case of extract_len == 0 as this means 'whole packet'.
        if (extract_len == 4*floor_extract_wlen) extract_len = 0;
        bool len_from_inp = tu.xrandbool(cfg_seed, 5009);

        // Work out what length Packet can be
        //
        // If extract_len == 0 or MirrorMetadata coal_len == 0 then
        // we're doing whole packet extract.
        //
        // In this case we limit packet size to be maxsize_payload
        // which is determined from trunc_size and size taken up by slice hdrs
        // (we ensure it is at least min_packet_size)
        //
        // mirr_coal_wlen is in 32b words with max 255 (cap_extract_wlen is x32b already)
        int  mirr_coal_wlen = tu.xrandrange(pkt_rand, 5104,
                                            floor_extract_wlen, cap_extract_wlen);
        // Allow mirr_coal_wlen == 0 as this means 'whole packet'.
        if (mirr_coal_wlen == floor_extract_wlen) mirr_coal_wlen = 0;
        bool whole_packet = (len_from_inp) ?(mirr_coal_wlen == 0) :(extract_len == 0);
        int  cap_packet_size = max_packet_size;
        if (whole_packet && (maxsize_payload < max_packet_size)) {
          cap_packet_size = std::max(min_packet_size, maxsize_payload);
        }
        int pkt_len = tu.xrandrange(pkt_rand, 5101, min_packet_size, cap_packet_size);


        // Do some printing from time to time
        if ((mirror_stress_print) &&
            ((tot_packets <= 100) || ((packet % print_modulus) == 0))) {
          printf("Iter[%d] Packet[%d]  Port=%d,Slice=%d,Sess=%d,CoalNum=%d  "
                 "PktLen=%d,PktRandSeed=%016" PRIx64 " IterSeed=%16" PRIx64 " ErrCnt=%d\n",
                 iteration, packet, port, slice, sess, coal_num,
                 pkt_len, pkt_rand, iteration_seed, ErrCnt);
        }


        // Now we have port/sess we can lookup PortSess object (or create one)
        PortSess *port_sess = PortSessTab[port][sess];
        if (port_sess == nullptr) {
          port_sess = new PortSess();
          port_sess->next_pkt_num = UINT64_C(0);
          for (int i = 0; i < pkts_per_portsess; i++) port_sess->packet_seeds[i] = UINT64_C(0);
          for (int i = 0; i < pkts_per_portsess; i++) port_sess->packet_data_last[i] = UINT64_C(0);
          for (int i = 0; i < pkts_per_portsess; i++) port_sess->packet_byte_next[i] = 0;
          PortSessTab[port][sess] = port_sess;
        }
        // Get pkt_num to use.
        int pkt_num = port_sess->next_pkt_num++ % pkts_per_portsess;
        port_sess->packet_seeds[pkt_num] = pkt_rand;
        port_sess->packet_data_last[pkt_num] = pkt_rand;
        port_sess->packet_byte_next[pkt_num] = 0;
        uint16_t cfg_hash2 = tu.xrand16(pkt_rand, 5022, 13);
        uint16_t mirr_hash = tu.xrand16(pkt_rand, 5107, 13);


        if (debug_sess && mirror_stress_debug) {
          printf("COAL_STRESS<NewPkt>: Slice%d/%d [%d,%d] PktNum=%d "
                 "PktLen=%d TruncLen=%d,ExtractLen=%d,CoalLen=%d,FromINP=%d "
                 "PktRandSeed=%016" PRIx64 " CfgHash2=%d MirrMetaHash=%d  %s\n",
                 slice,coal_num, port,sess, pkt_num,pkt_len,
                 trunc_size, extract_len, mirr_coal_wlen*4, len_from_inp,
                 pkt_rand, cfg_hash2, mirr_hash, coal_en ?"" :"!!!!! NOT COAL PKT !!!!!");
        }


        w64 = pkt_rand; // Seed pkt_data from pkt_rand
        // Fill in our single fixed buf with new data up to that length
        int bufsz = ((pkt_len+3)/4)*4; // Round up to 4B
        RMT_ASSERT(bufsz <= max_packet_size);
        uint64_t mask64 = UINT64_C(0xFFFFFFFF) << ((bufsz - pkt_len) * 8);
        uint32_t mask32 = static_cast<uint32_t>(mask64 & UINT64_C(0xFFFFFFFF));

        // Loop installing data
        pos = 0;
        while (pos < pkt_len) {
          //uint64_t w64_prev = w64;

          // Fill with random data 32b at a time using Knuth's MMIX 64b linear congruence
          w64 = (w64 * UINT64_C(6364136223846793005)) + UINT64_C(1442695040888963407);
          w32 = static_cast<uint32_t>((w64 >> 32) ^ (w64 & UINT64_C(0xFFFFFFFF)));
          w32 = (w32 & 0x00FFFFFF) | (pkt_num << 24); // But put pkt_num in MSB
          uint32_t mask = (pos + 4 > pkt_len) ?mask32 :0xFFFFFFFFu;
          w32 &= mask; // Mask off bytes outside pkt_len

          if (((pos % 176) <= 12) && (debug_sess) && (mirror_stress_data_debug)) {
            printf("COAL_STRESS<NewPktData>: Slice%d/%d [%d,%d] PktNum=%d "
                   "PktRandSeed=%016" PRIx64 " W64=%016" PRIx64 " "
                   "Data[%d]=0x%08x Mask=0x%08x\n",
                   slice, coal_num, port, sess, pkt_num, pkt_rand,
                   w64, pos/4, w32, mask);
          }
          pos = model_common::Util::fill_buf(PktBuf, bufsz, pos, 4, w32);
        }


        // Now setup Packet itself and fill-in MirrorMetadata
        Packet *in_pkt = om->pkt_create(PktBuf, pkt_len);
        if (ingress) in_pkt->set_ingress(); else in_pkt->set_egress();
        in_pkt->set_port(port_obj);

        MirrorMetadata *mm = in_pkt->mirror_metadata();
        mm->set_mirror_id(sess);
        mm->set_coal_len(mirr_coal_wlen); // In 32b words
        mm->set_version(tu.xrand8(pkt_rand, 5105, 2));
        mm->set_mirr_io_sel(tu.xrand8(pkt_rand, 5106, 1));
        mm->set_mirr_hash(mirr_hash);
        mm->set_mirr_mc_ctrl(tu.xrand8(pkt_rand, 5108, 1));
        mm->set_mirr_c2c_ctrl(tu.xrand8(pkt_rand, 5109, 1));
        mm->set_mirr_sel(tu.xrand8(pkt_rand, 5110, 1));
        //mm->set_mirr_epipe_port(tu.xrand16(pkt_rand, 5111, 9));
        mm->set_mirr_qid(tu.xrand8(pkt_rand, 5112, 7));
        mm->set_mirr_dond_ctrl(tu.xrandbool(pkt_rand, 5113));
        mm->set_mirr_icos(tu.xrand8(pkt_rand, 5114, 3));



        // 3b. NOW CALL MIRROR LOGIC TO MIRROR PACKET

        int in_pkt_len = in_pkt->len();
        Packet *out_pkt = mb0->ProcessMirrorPacket(in_pkt, ingress);
        if (invalid) {
          EXPECT_EQ(out_pkt, nullptr);
        }

        if (out_pkt == nullptr) {
          // If Mirror logic returns NULL we must free in_pkt ourself
          om->pkt_delete(in_pkt);
          in_pkt = nullptr;
        } else {

          I2QueueingMetadata *out_i2q = out_pkt->i2qing_metadata();
          int out_pkt_len = out_pkt->len();
          EXPECT_FALSE(invalid);
          EXPECT_GE(out_pkt_len, 4); // Absolute min
          out_pkt_len -= fcs_size;   // Discount FCS 4B in in/out pkts
          in_pkt_len -= fcs_size;
          // XXX: On JBayB0, WIP and later trunc_size now *includes* FCS so remove
          if (!RmtObject::is_jbayA0()) trunc_size -= fcs_size;

          if (norm_en) {
            // Check simple mirrored packet
            // Should be same packet but maybe truncated
            EXPECT_EQ(in_pkt, out_pkt);
            if (mirror_stress_debug || mirror_stress_norm_print)
              printf("COAL_STRESS:NormalMirror: InLen=%d MinLen=%d TruncSz=%d OutLen=%d FCS=%dB %s\n",
                     in_pkt_len, min_pkt_len, trunc_size, out_pkt_len, fcs_size,
                     ((in_pkt_len < min_pkt_len) || (in_pkt_len > trunc_size)) ?"!!!!!!!!!!!!!!!" :"");
            if (RmtObject::is_jbayA0()) {
              // XXX: JBayA0 erroneously adds 4byte FCS to mirror packets
              if (in_pkt_len < min_pkt_len) {
                EXPECT_EQ(out_pkt_len, min_pkt_len + 4); // min_pkt_len only configurable on WIP
              } else if (in_pkt_len > trunc_size) {
                EXPECT_EQ(out_pkt_len, trunc_size + 4);
              } else {
                EXPECT_LE(out_pkt_len, trunc_size + 4);
              }
            }
            else {
              if (in_pkt_len < min_pkt_len) {
                EXPECT_EQ(out_pkt_len, min_pkt_len); // min_pkt_len only configurable on WIP
              } else if (in_pkt_len > trunc_size) {
                EXPECT_EQ(out_pkt_len, trunc_size);
              } else {
                EXPECT_LE(out_pkt_len, trunc_size);
              }
            }
            om->pkt_delete(in_pkt);
            in_pkt = nullptr;

          } else if (coal_en) {
            int fillsz = 0;

            // Could be getting COAL packet that includes slices
            // from earlier packets - time to check
            uint8_t coal_hdr[16];
            fillsz = out_pkt->get_buf(coal_hdr, 0, 16);
            EXPECT_EQ(fillsz, 16);
            uint32_t hdr0 = 0u, hdr1 = 0u, hdr2 = 0u, hdr3 = 0u;
            pos = 0;
            pos = model_common::Util::fill_val(&hdr0, 4, coal_hdr, 16, pos);
            pos = model_common::Util::fill_val(&hdr1, 4, coal_hdr, 16, pos);
            pos = model_common::Util::fill_val(&hdr2, 4, coal_hdr, 16, pos);
            pos = model_common::Util::fill_val(&hdr3, 4, coal_hdr, 16, pos);

            int out_sess = hdr0 & 0xFFFF;
            EXPECT_LT(out_sess, num_sessions);
            int out_sess0 = (out_sess >> 0) & 0x3F; // Get sess for Slice0
            int out_slice = (out_sess >> 6) & 0x3;
            int out_coal_num = (out_sess >> 0) & 0xF;
            uint64_t out_cfg = SessCfgSeed[out_sess];

            // hdr0,hdr1,hdr2,hdr3 NOT per-slice so use Slice0 out_cfg for these
            uint64_t out_cfg0 = SessCfgSeed[out_sess0]; // Get out_cfg for Slice0

            // Re-synthesize cfg vals now we know seed of config we're checking
            int  out_trunc_size = tu.xrandrange(out_cfg, 5000, min_trunc_size,max_trunc_size);
            out_trunc_size = (out_trunc_size / 4) * 4; // Round down to 4B boundary on JBay
            int  out_pkt_hdr_len = 8 + (4 * tu.xrandrange(out_cfg, 5001, 0, 2)); // 8,12,16
            int  out_totsize_headers = out_pkt_hdr_len + 4 + (4 * (out_trunc_size / 180));
            int  out_maxsize_payload = out_trunc_size - out_totsize_headers;
            int  out_cap_extract_wlen = std::min(255, out_maxsize_payload/4);
            int  out_floor_extract_wlen = ((out_maxsize_payload / pkts_per_portsess)/4);
            RMT_ASSERT(out_floor_extract_wlen < 255);
            int  out_extract_len = 4 * tu.xrandrange(out_cfg, 5006,
                                                     out_floor_extract_wlen,
                                                     out_cap_extract_wlen);
            // Allow a special case of out_extract_len == 0 as this means 'whole packet'.
            if (out_extract_len == 4*out_floor_extract_wlen) out_extract_len = 0;
            bool out_len_from_inp = tu.xrandbool(out_cfg, 5009);

            if (out_pkt_len > out_trunc_size + 180) {
              printf("COAL_STRESS: Slice%d/%d PktLen=%d TruncSize=%d\n",
                     out_slice, out_coal_num, out_pkt_len, out_trunc_size);
              ErrCnt++;
            }
            if ((hdr0 >> 24) != (tu.xrand32(out_cfg0, 5010) & 0xFF)) {
              printf("COAL_STRESS: Slice%d/%d GotCompFlag=0x%02x ExpectedCompFlag=0x%02x "
                     "(RawPktHdr0=0x%08x RawCfgHdr0=0x%08x)\n", out_slice, out_coal_num,
                     hdr0 >> 24, tu.xrand32(out_cfg0,5010) & 0xFF,
                     hdr0, tu.xrand32(out_cfg0,5010));
              ErrCnt++;
            }


            // Check COAL pkt len as expected for session
            //
            // Should be less than trunc_size + 180
            EXPECT_LE(out_pkt_len, out_trunc_size + 180);

            // Hdr0 (upper byte in packet, bottom byte of config word),
            // Hdr2 (maybe), Hdr3 (maybe) should be as we set them up to be.
            int  slice_pos_in_pkt = 8;
            EXPECT_EQ(hdr0 >> 24, tu.xrand32(out_cfg0, 5010) & 0xFF);
            if (out_pkt_hdr_len > 8) {
              EXPECT_EQ(hdr2, tu.xrand32(out_cfg0, 5012));
              slice_pos_in_pkt = 12;
            }
            if (out_pkt_hdr_len > 12) {
              EXPECT_EQ(hdr3, tu.xrand32(out_cfg0, 5013));
              slice_pos_in_pkt = 16;
            }
            EXPECT_LE(slice_pos_in_pkt, out_pkt_len);


            // Loop through all slices checking
            int slice_num = 0;
            while (slice_pos_in_pkt < out_pkt_len) {

              // First of all read in SliceHdr and first data word of Slice
              uint8_t slice_buf[180]; // Big enough for whole slice
              fillsz = out_pkt->get_buf(slice_buf, slice_pos_in_pkt, 4 + 4);
              EXPECT_EQ(fillsz, 8);
              uint32_t slice_hdr = 0u, slice_data0 = 0u;
              pos = 0;
              pos = model_common::Util::fill_val(&slice_hdr, 4, slice_buf, 4 + 4, pos);
              pos = model_common::Util::fill_val(&slice_data0, 4, slice_buf, 4 + 4, pos);

              // Check data in slice hdr and first slice data word
              // Get port/len out of slice hdr
              // Get pkt_num  out of first slice data word
              int  out_port    =   (slice_hdr   >> 16) & 0xFFFF;
              int  out_len     =   (slice_hdr   >>  8) & 0x00FF;
              //bool out_start_f = (((slice_hd  >>  7) & 1) == 1);
              bool out_end_f   = (((slice_hdr   >>  6) & 1) == 1);
              int  out_pkt_num =   (slice_data0 >> 24) & 0x00FF;
              // Do we do debug prints
              bool out_debug_sess = (((debug_slice < 0) || (debug_slice == out_slice)) &&
                                ((debug_coal_num < 0) || (debug_coal_num == out_coal_num)));


              EXPECT_LE(out_pkt_num, pkts_per_portsess);
              EXPECT_LE(out_len, 176);
              if (!out_end_f) {
                EXPECT_EQ(out_len, 176);
              }

              // Lookup Port/Sess for this packet - should exist!
              PortSess *out_port_sess = PortSessTab[out_port][out_sess];
              ASSERT_TRUE(out_port_sess != nullptr);
              uint64_t out_pkt_rand = out_port_sess->packet_seeds[out_pkt_num];


              // Initialise expected packet len using relevant cfg/rand vals
              // (might be whole packet len, sess_extract_len OR mirr_meta coal_len).
              // Read current len, seen to date, out of PortSess
              //
              int  pkt_mirr_coal_wlen = tu.xrandrange(out_pkt_rand, 5104,
                                                      out_floor_extract_wlen,
                                                      out_cap_extract_wlen);
              // Allow pkt_mirr_coal_wlen == 0 as this means 'whole packet'.
              if (pkt_mirr_coal_wlen == out_floor_extract_wlen) pkt_mirr_coal_wlen = 0;

              bool pkt_whole_packet;
              int  pkt_extract_len;
              if (out_len_from_inp) {
                pkt_whole_packet = (pkt_mirr_coal_wlen == 0);
                pkt_extract_len = 4 * pkt_mirr_coal_wlen;
              } else {
                pkt_whole_packet = (out_extract_len == 0);
                pkt_extract_len = out_extract_len;
              }
              int  pkt_cap_packet_size = max_packet_size;
              if (pkt_whole_packet && (out_maxsize_payload < max_packet_size)) {
                pkt_cap_packet_size = std::max(min_packet_size, out_maxsize_payload);
              }

              int exp_inp_pkt_len = tu.xrandrange(out_pkt_rand, 5101,
                                                  min_packet_size, pkt_cap_packet_size);
              int exp_out_pkt_len = std::min(pkt_extract_len, exp_inp_pkt_len);
              if (pkt_whole_packet) exp_out_pkt_len = exp_inp_pkt_len;
              int got_so_far_pkt_len = out_port_sess->packet_byte_next[out_pkt_num];


              // Initialise w64 to let us check data words in slice
              w64 = out_port_sess->packet_data_last[out_pkt_num];


              // More printing
              if (out_debug_sess && mirror_stress_debug) {
                printf("COAL_STRESS<Start>: Slice%d/%d [%d,%d] PktNum=%d "
                       "PktRandSeed=%016" PRIx64 " (WhichSlice=%d) "
                       "ExpOutLen=%d(ExtractLen=%d,CoalLen=%d,FromINP=%d,PktLen=%d) "
                       "GotLen=%d \n",
                       out_slice, out_coal_num, out_port, out_sess, out_pkt_num,
                       out_pkt_rand, slice_num,
                       exp_out_pkt_len, out_extract_len, pkt_mirr_coal_wlen*4, pkt_whole_packet,
                       exp_inp_pkt_len, got_so_far_pkt_len);
              }


              // Now we know slice size we can read whole slice into buffer to check
              int  data_sz  = 4 + out_len;           // Includes slice hdr, NOT rounded
              int  slice_sz = 4 + ((out_len+3)/4)*4; // Includes slice hdr, round up to 4B
              uint64_t mask64 = UINT64_C(0xFFFFFFFF) << ((slice_sz - data_sz) * 8);
              uint32_t mask32 = static_cast<uint32_t>(mask64 & UINT64_C(0xFFFFFFFF));

              fillsz = out_pkt->get_buf(slice_buf, slice_pos_in_pkt, slice_sz);
              EXPECT_EQ(fillsz, slice_sz);

              // Loop checking data as we expect
              pos = 4; // Get past slice hdr
              while (pos < slice_sz) {
                //uint64_t w64_prev = w64;
                uint32_t data0 = 0u;
                int pos2 = model_common::Util::fill_val(&data0, 4, slice_buf, slice_sz, pos);
                uint32_t mask = (pos2 > data_sz) ?mask32 :0xFFFFFFFFu;
                data0 &= mask;

                // Work out what *next* data word *should* be
                w64 = (w64 * UINT64_C(6364136223846793005)) + UINT64_C(1442695040888963407);
                w32 = static_cast<uint32_t>((w64 >> 32) ^ (w64 & UINT64_C(0xFFFFFFFF)));
                w32 = (w32 & 0x00FFFFFF) | (out_pkt_num << 24); // Always pkt_num in MSB
                w32 &= mask;

                // Verify data word is what we expect
                if (((data0 != w32) && (pos <= 32)) ||
                    ((out_debug_sess) && (mirror_stress_data_debug) && (pos <= 12))) {
                  printf("COAL_STRESS<Chk>: Slice%d/%d [%d,%d] PktLen=%d "
                         "Slice(Num=%d,Sz=%d<%d>,PktPos=%d) Word(PktPos=%d,SlicePos=%d) "
                         "W64=%016" PRIx64 " "
                         "Exp[%d]=0x%08x Got[%d]=0x%08x Mask=0x%08x %s\n",
                         out_slice, out_coal_num, out_port, out_sess,
                         out_pkt_len, slice_num, data_sz, slice_sz,
                         slice_pos_in_pkt, slice_pos_in_pkt + pos, pos-4,
                         w64, (pos-4)/4, w32, (pos-4)/4, data0, mask,
                         (data0 != w32) ?"!!!!!!!!!!" :"");
                  if (data0 != w32) ErrCnt++;
                }
                EXPECT_EQ(data0, w32);
                pos = pos2;
              }


              // Increment len we read out of PortSess
              got_so_far_pkt_len += out_len;


              // FINAL CHECKS
              //
              // got_so_far_len should always be <= exp_pkt_len
              // Unless END flag set in which case should be same
              bool bad_len = ((got_so_far_pkt_len > exp_out_pkt_len) ||
                              ((out_end_f) && (got_so_far_pkt_len != exp_out_pkt_len)));
              if (bad_len) ErrCnt++;
              if (out_debug_sess && mirror_stress_debug) {
                printf("COAL_STRESS<End>: Slice%d/%d [%d,%d] PktNum=%d "
                       "PktRandSeed=%016" PRIx64 " (WhichSlice=%d) "
                       "ExpOutLen=%d(ExtractLen=%d,CoalLen=%d,FromINP=%d,PktLen=%d) "
                       "GotLen=%d %s\n",
                       out_slice, out_coal_num, out_port, out_sess, out_pkt_num,
                       out_pkt_rand, slice_num,
                       exp_out_pkt_len, out_extract_len, pkt_mirr_coal_wlen*4, pkt_whole_packet,
                       exp_inp_pkt_len, got_so_far_pkt_len, bad_len ?"!!!!!!!!!!" :"");
              }
              EXPECT_LE(got_so_far_pkt_len, exp_out_pkt_len);
              if (out_end_f) {
                EXPECT_EQ(got_so_far_pkt_len, exp_out_pkt_len);
              }

              // If first slice check info in coal_pkt I2Q meta is from orig_pkt MIRR meta
              if (slice_num == 0) {
                // Check a few vars that always come from session cfg
                uint16_t cfg_xid   = tu.xrand16(out_cfg, 5029);
                uint16_t cfg_yid   = tu.xrand16(out_cfg, 5030, 9);
                uint16_t cfg_rid   = tu.xrand16(out_cfg, 5031);
                uint16_t cfg_hash2 = tu.xrand16(out_cfg, 5022, 13);
                uint8_t  cfg_icos  = tu.xrand8 (out_cfg, 5018, 3);
                uint16_t pkt_hash  = tu.xrand16(out_pkt_rand, 5107, 13);
                uint8_t  pkt_icos  = tu.xrand8 (out_pkt_rand, 5114, 3);

                if ((cfg_xid != out_i2q->xid()) ||
                    (cfg_yid != out_i2q->yid()) ||
                    (cfg_rid != out_i2q->irid())) ErrCnt++;
                EXPECT_EQ(cfg_xid,  out_i2q->xid());
                EXPECT_EQ(cfg_yid, out_i2q->yid());
                EXPECT_EQ(cfg_rid, out_i2q->irid());

                // Check a couple of vars that could either come
                // from session cfg or from mirror metadata
                if (tu.xrandbool(out_cfg, 5035)) {
                  if (pkt_hash != out_i2q->hash2()) ErrCnt++;
                  EXPECT_EQ(pkt_hash, out_i2q->hash2());
                } else {
                  if (cfg_hash2 != out_i2q->hash2()) ErrCnt++;
                  EXPECT_EQ(cfg_hash2, out_i2q->hash2());
                }
                if (tu.xrandbool(out_cfg, 5036)) {
                  if (pkt_icos != out_i2q->icos()) ErrCnt++;
                  EXPECT_EQ(pkt_icos, out_i2q->icos());
                } else {
                  if (cfg_icos != out_i2q->icos()) ErrCnt++;
                  EXPECT_EQ(cfg_icos, out_i2q->icos());
                }
              }

              // Track curr pos/ and last w64 val in out_port_sess for out_pkt
              // We pick these up next time we see a slice of this packet
              out_port_sess->packet_data_last[out_pkt_num] = w64;
              out_port_sess->packet_byte_next[out_pkt_num] = got_so_far_pkt_len;

              slice_pos_in_pkt += slice_sz;
              slice_num++;

            } // while (slice_pos_in_pkt < out_pkt_len)




            // Mirror code itself deletes in_pkt
            delete out_pkt; out_pkt = nullptr;

          } // else if (coal_en)

        } // if (out_pkt != nullptr)


      } // for (int packet = 0; packet < num_packets_per_iteration; packet++)


      // Fire timer a few times to drain queues
      uint32_t incr = 1;
      while ((incr < 999999999u) && (mb0->active_coal_sessions() > 0)) {
        model_timer::ModelTimerIncrement(incr);
        incr += incr;
      }
      EXPECT_EQ(mb0->active_coal_sessions(), 0u);

      //om->dump_stats(); // So we can track memory usage

    } // for (int iteration = 0; iteration < num_iterations; iteration++)

  }

#endif // if MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,WIP)




}
