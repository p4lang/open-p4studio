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

#include <model_core/model.h>
#include <bitvector.h>
#include <chip.h>
#include <rmt-object-manager.h>
#include <rmt-packet-coordinator.h>


extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

bool model_print = false;

using namespace std;
using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(ModelTest),ChipPackage) {
  using CTYP = model_core::ChipType;

  TestConfig tc(); // Just to save/restore static config changed below
  model_core::Model::kAllowCb50 = true; // XXX normally disabled - re-enable for this test

  GLOBAL_MODEL->Reset();
  GLOBAL_MODEL->DestroyAllChips();
  if (model_print) RMT_UT_LOG_INFO("test_model_chip_package()\n");
  model_core::Model *model = GLOBAL_MODEL.get();

  // Only create chip types for chips we're built with
  int typecnt = 0;
  int types[5] = { CTYP::kNone, CTYP::kNone, CTYP::kNone, CTYP::kNone, CTYP::kNone };
#ifdef MODEL_TOFINO
  types[typecnt++] = CTYP::kTofino;
#endif
#ifdef MODEL_TOFINOB0
  types[typecnt++] = CTYP::kTofinoB0;
#endif
#ifdef MODEL_JBAY
  types[typecnt++] = CTYP::kJbay;
#endif
#ifdef MODEL_JBAYB0
  types[typecnt++] = CTYP::kJbayB0;
#endif
  EXPECT_LT(0, typecnt);

  // Step through chip indices at diff spacing
  int steps[5] = { 1, 23, 41, 61, 83 };


  for (int pkgsz = 1; pkgsz <= 4; pkgsz++) { // Diff pkg sizes

    for (int type = 0; type < typecnt; type++) { // Of diff types

      for (int step = 0; step < 5; step++) { // Using diff chip index spacing

        // If we have many chip-types configured and we're testing non-trivial
        // package size, then every odd step we test non_identical chips in package
        bool non_identical = (typecnt > 1) && (pkgsz > 1) && ((step % 2) == 1);
        bool identical = !non_identical;
        int  chip_type = types[type];
        int  diff_type = (type > 0) ?types[0] :types[1];
        //printf("PKGSZ=%d CTYPE=%d STEP=%d\n", pkgsz, chip_type, steps[step]);

        // Create 1->4 chips of same type, chip_type
        // (unless non-identical where we make the type of chip[0] diff_type)
        int chips[4] = { -1, -1, -1, -1 };
        for (int c = 0; c < pkgsz; c++) {
          int ctyp = ((c == 0) && (non_identical)) ?diff_type :chip_type;
          int cidx = (c * steps[step]) % 200; // Keep in [0..199]

          bool crea_ok = model->CreateChip(cidx, ctyp);
          EXPECT_EQ(true, crea_ok);
          chips[c] = cidx;
        }

        // Set into a package - ok if all CBs or if trivial pkgsz 1
        bool pkgsz_ok   = ( (pkgsz == 1) || (pkgsz == 2) || (pkgsz == 4) );
        bool exp_set_ok = ( (identical && pkgsz_ok && (chip_type == CTYP::kRsvd0)) || (pkgsz == 1) );
        bool exp_is_pkg = (exp_set_ok && (pkgsz > 1));

        bool set_ok = model->SetPackage(chips[0], chips[1], chips[2], chips[3]);
        EXPECT_EQ(exp_set_ok, set_ok);

        // Check model thinks all chips are packaged/unpackaged as appropriate
        for (int c = 0; c < pkgsz; c++) {
          bool act_is_pkg = model->IsPackaged(chips[c]);
          EXPECT_EQ(exp_is_pkg, act_is_pkg);
        }
        bool is_pkg = exp_is_pkg;

        if (set_ok) {
          // Retrieve package if set and check what was setup
          for (int pos = 0; pos < pkgsz; pos++) {
            int c0 = chips[pos], c1 = -1, c2 = -1, c3 = -1;
            int sz = model->GetPackage(&c0, &c1, &c2, &c3);
            // Size should match
            EXPECT_EQ(sz, pkgsz);
            int chips2[4] = { c0, c1, c2, c3 };
            // Package should match irrespective of which chip we used as c0
            for (int i = 0; i < 4; i++) {
              EXPECT_EQ(chips[i], chips2[i]);
            }
          }
          // Given we set ok any attempt to delete a packaged chip should fail
          if (is_pkg) {
            for (int c = 0; c < pkgsz; c++) {
              bool del_ok = model->DestroyChip(chips[c]);
              EXPECT_FALSE(del_ok);
            }
          }
        }

        bool exp_unpkg_ok = is_pkg; // Can only unpackage if packaged
        bool unpkg_ok = model->UnPackage(chips[0], chips[1], chips[2], chips[3]);
        EXPECT_EQ(exp_unpkg_ok, unpkg_ok);

        // Check model now thinks all chips are unpackaged
        for (int c = 0; c < pkgsz; c++) {
          bool act_is_pkg = model->IsPackaged(chips[c]);
          EXPECT_FALSE(act_is_pkg);
        }

        // Check we can now delete all chips
        for (int c = 0; c < pkgsz; c++) {
          bool del_ok = model->DestroyChip(chips[c]);
          EXPECT_TRUE(del_ok);
        }

      } // for step
    } // for type
  } // for pkgsz
}


#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)

int               model_test_base_asic;
std::mutex        model_test_mutex;
RmtObjectManager *model_test_obj_mgrs[4];
uint32_t          model_test_tx_tot_pkt_count;
uint32_t          model_test_tx_pkt_counts[4][4][72];
uint32_t          model_test_ing_pkt_counts[4][4][72];
void              model_test_tx_pkt_fn(int asic_id, int port, uint8_t *buf, int len) {
  // XXX: transmit func will have mapped port to external range [0..36] so we
  // must revert it back to the internal range [0..72]
  port = Port::port_map_inbound(port);
  RMT_ASSERT((asic_id >= model_test_base_asic) && (asic_id < model_test_base_asic + 4));
  // Have to use asic_id as each chip just transmits with its local pipe/port
  int c = asic_id - model_test_base_asic;
  RMT_ASSERT((c >= 0) && (c < 4));
  int p_c = Port::get_die_num(port);
  RMT_ASSERT((p_c >= 0) && (p_c < 4));
  int p = Port::get_pipe_num(port);
  RMT_ASSERT((p >= 0) && (p < 4));
  int p_port = Port::get_pipe_local_port_index(port);
  RMT_ASSERT((p_port >= 0) && (p_port < 72));

  std::unique_lock<std::mutex> lock(model_test_mutex);

  model_test_tx_pkt_counts[c][p][p_port]++;
  model_test_tx_tot_pkt_count++;

  // Parse apart RX buf using sscanf...
  uint32_t P_tot_pkt_cnt;
  int P_in_c, P_c, P_p, P_port;
  P_tot_pkt_cnt = P_in_c = P_c = P_p = P_port = 999999999;
  const char *cbuf = (char*)buf;
  sscanf(cbuf, "Pkt=%4d InC=%d C=%d P=%d Port=%d", &P_tot_pkt_cnt, &P_in_c, &P_c, &P_p, &P_port);

  // ... and check we see monotonic increasing pkt_cnt for packets from a given *ingress* port
  // (we use the same 3-p 71-port calculation that occurs at TX to infer where the packet ingressed)
  if (P_tot_pkt_cnt <= model_test_ing_pkt_counts[P_in_c][3-P_p][71-P_port]) {
    printf("PACKET REORDERED from [%d,%d,%2d]: cntMaxRX=%d cntThisRX=%d !!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",
           P_in_c, 3-P_p, 71-P_port, model_test_ing_pkt_counts[P_in_c][3-P_p][71-P_port], P_tot_pkt_cnt);
  } else {
    // Track max pkt_cnt from *ingress* port
    model_test_ing_pkt_counts[P_in_c][3-P_p][71-P_port] = P_tot_pkt_cnt;
  }

  if (model_test_tx_pkt_counts[c][p][p_port] > 4) {
    printf("PACKET EXCESS: model_test_tx_pkt_fn[p_c=%d] (%d,%3d[%d,%d,%2d]=%2d) [%s]\n",
           p_c, asic_id, port, c, p, p_port, model_test_tx_pkt_counts[c][p][p_port],
           reinterpret_cast<char*>(buf));
  }
}

TEST(BFN_TEST_NAME(ModelTest),MultiChipPacketRouting) {
  // Replace pipe process function with one that skips all pipe processing.
  // There should be no ingress packets and egress packets should just be sent
  pipe_process_fn_t pipe_process_fn = [](Packet *ingr_pkt,
                                         Packet *egr_pkt,
                                         Packet **queued_pkt,
                                         Packet **sent_pkt,
                                         Packet **resubmit_pkt,
                                         Packet **ing_mirror_pkt,
                                         Packet **egr_mirror_pkt
  ) {
    *queued_pkt = ingr_pkt;
    if (nullptr != ingr_pkt) {
      EXPECT_TRUE(ingr_pkt->is_ingress());
    }
    *sent_pkt = egr_pkt;
    if (nullptr != egr_pkt) {
      EXPECT_TRUE(egr_pkt->i2qing_metadata()->is_egress_uc());
      uint16_t port = egr_pkt->i2qing_metadata()->egress_uc_port();
      int c = Port::get_die_num(port);
      int local_port = Port::get_die_local_port_index(port);
      egr_pkt->set_egress();
      egr_pkt->set_metadata_added(true);
      egr_pkt->set_port(model_test_obj_mgrs[c]->port_lookup(local_port));
    }
  };

  TestConfig tc(); // Just to save/restore static config changed below
  model_core::Model::kAllowCb50 = true; // XXX normally disabled - re-enable for this test

  GLOBAL_MODEL->Reset();
  GLOBAL_MODEL->DestroyAllChips();
  if (model_print) RMT_UT_LOG_INFO("test_model_multi_chip_packet_routing()\n");
  model_core::Model *model = GLOBAL_MODEL.get();

  TestUtil *tus[4];
  Chip *chp[4];
  RmtPacketCoordinator *pcs[4];
  uint32_t local_counts[4][4][72]; // 4 chips of 4 pipes of 72 ports (just lo 32b)

  // Reset global counters and local counters
  model_test_base_asic = 210;
  model_test_tx_tot_pkt_count = 1u; // Avoid 0 to simplify reordering check
  for (int c = 0; c < 4; c++) {
    for (int p = 0; p < 4; p++) {
      for (int port = 0; port < 72; port++) {
        model_test_tx_pkt_counts[c][p][port] = 0u;
        model_test_ing_pkt_counts[c][p][port] = 0u;
        local_counts[c][p][port] = 0u;
      }
    }
  }
  // Create chips via TestUtil class to give us convenience funcs
  for (int c = 0; c < 4; c++) {
    tus[c] = new TestUtil(model, model_test_base_asic + c, 0, 0);
    model_test_obj_mgrs[c] = tus[c]->get_objmgr();
    chp[c] = model_test_obj_mgrs[c]->chip();
    pcs[c] = model_test_obj_mgrs[c]->packet_coordinator_get(); // N threads set to 1
    pcs[c]->stop(); // Initially stop
    pcs[c]->set_pipe_process_fn(pipe_process_fn);
    pcs[c]->set_tx_fn(model_test_tx_pkt_fn);
  }
  // Now package up all chips
  bool pkg_ok = model->SetPackage(model_test_base_asic + 0, model_test_base_asic + 1,
                                  model_test_base_asic + 2, model_test_base_asic + 3);
  EXPECT_TRUE(pkg_ok);

  for (int c = 0; c < 4; c++) pcs[c]->set_num_threads(c+1); // Bump N threads
  for (int c = 0; c < 4; c++) pcs[c]->start(); // Start all PacketCoordinators

  // Now create packets destined for all output ports
  // Introduce packets on each chip in turn routing to all other outputs
  char cbuf[64];
  uint8_t ubuf[64];
  uint32_t tot_pkt_count = 1u; // Avoid 0 to simplify reordering check
  uint32_t idle_count = 0u;
  int port_inc = 2; // XXX: WIP: odd ports unused on WIP so avoid

  for (int in_c = 0; in_c < 4; in_c++) {
    for (int c = 0; c < 4; c++) {
      for (int p = 0; p < 4; p++) {
        // Avoid low ports - often PCIe->CPU so don't get tx_fn callback
        for (int port = 8; port < 72; port += port_inc) {
          sprintf(cbuf, "Pkt=%4d InC=%d C=%d P=%d Port=%d        ",
                  tot_pkt_count, in_c, c, p, port);
          memcpy((void*)ubuf, (void*)cbuf, (size_t)32);
          tot_pkt_count++;
          Packet *pkt = model_test_obj_mgrs[in_c]->pkt_create(ubuf,32);
          I2QueueingMetadata *i2q = pkt->i2qing_metadata();
          // Setup full port info
          bool local = ((chp[in_c]->GetMyDieId() == static_cast<uint32_t>(c)) ||
                        (chp[in_c]->GetReadDieId() == static_cast<uint32_t>(c)));
          bool remote = ((chp[in_c]->GetWriteDieId() == static_cast<uint32_t>(c)) ||
                         (chp[in_c]->GetDiagonalDieId() == static_cast<uint32_t>(c)));
          EXPECT_TRUE(local || remote);
          uint8_t  tmv = (local?1:0 ) | (remote?2:0);
          uint16_t ing_port = (in_c << 9) | ((3-p) << 7) | (72-port_inc-port);
          uint16_t egr_port = (c << 9)    | (p << 7)     | (port);
          uint16_t ing_local_port = Port::get_die_local_port_index(ing_port);

          // printf("MCPR: Packet[%4d] IngPort=0x%x EgrPort=0x%x\n", tot_pkt_count, ing_port, egr_port);
          EXPECT_NE(3, tmv);
          if ((port % 8) == 0) tmv = 3; // Occasionally set both local/remote
          i2q->set_tm_vec(tmv);
          i2q->set_physical_ingress_port(ing_port);
          i2q->set_egress_unicast_port(egr_port);

          pkt->set_ingress();
          pkt->set_metadata_added(true);
          pkt->set_port(model_test_obj_mgrs[c]->port_lookup(ing_local_port));

          pcs[in_c]->enqueue(ing_local_port, pkt, false);
          if (pcs[in_c]->is_idle()) idle_count++;

          // Count anticipated packet outputs per-chip/pipe/port
          local_counts[c][p][port]++;

          std::this_thread::sleep_for(std::chrono::microseconds(1000));
        } // for port
      } // for p
    } // for c
  } // for in_c

  // Sleep a while longer - but bail if tot counts match/exceed(err)
  // printf("MCPR: Sleeping a while....\n"); fflush(stdout);
  for (int iter = 0; iter < 5000; iter++) {
    if (model_test_tx_tot_pkt_count >= tot_pkt_count) break;
    std::this_thread::sleep_for(std::chrono::microseconds(1000));
  }
  // printf("MCPR: Stopping PacketCoordinators....\n"); fflush(stdout);
  for (int c = 0; c < 4; c++) pcs[c]->stop(); // Stop all PacketCoordinators

  // Check local packet counts match global counts
  for (int c = 0; c < 4; c++) {
    for (int p = 0; p < 4; p++) {
      for (int port = 0; port < 72; port++) {
        if (local_counts[c][p][port] != model_test_tx_pkt_counts[c][p][port]) {
          printf("PACKET COUNT MISMATCH: SEND[%d,%d,%2d]=%2d XMIT[%d,%d,%2d]=%2d !!!!!!!!!!!!\n",
                 c, p, port, local_counts[c][p][port],
                 c, p, port, model_test_tx_pkt_counts[c][p][port]);
        }
        EXPECT_EQ(local_counts[c][p][port], model_test_tx_pkt_counts[c][p][port]);
      }
    }
  }

  // Error if model reports idle immediately post enqueue more than 1 time in 500
  printf("TotPktCount=%d IdleCount=%d\n", tot_pkt_count, idle_count);
  EXPECT_LT(idle_count, tot_pkt_count/500);

  // Unpackage all chips
  // printf("MCPR: Unpackaging chips....\n"); fflush(stdout);
  model->UnPackage(model_test_base_asic + 0, model_test_base_asic + 1,
                   model_test_base_asic + 2, model_test_base_asic + 3);

  // Delete tus and teardown all chips
  for (int c = 0; c < 4; c++) {
    pcs[c]->set_tx_fn(nullptr);
    pcs[c]->set_pipe_process_fn(nullptr);
    pcs[c] = nullptr;
    chp[c] = nullptr;
    model_test_obj_mgrs[c] = nullptr;
    tus[c]->quieten_log_flags();
    delete tus[c];
    tus[c] = nullptr;
  }

  // printf("MCPR: Calling MODEL->Reset()\n"); fflush(stdout);
  GLOBAL_MODEL->Reset();
  // printf("MCPR: Calling MODEL->DestroyAllChips()\n"); fflush(stdout);
  GLOBAL_MODEL->DestroyAllChips();
  // printf("MCPR: DONE!\n"); fflush(stdout);
}

#endif

}
