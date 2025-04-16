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

#ifndef _UTESTS_TEST_RMT_PACKET_COORDINATOR_H
#define _UTESTS_TEST_RMT_PACKET_COORDINATOR_H

#include <utests/test_util.h>
#include <common/rmt.h>
#include <packet.h>
#include <pktgen.h>
#include <port.h>
#include <rmt-object-manager.h>
#include <rmt-packet-coordinator.h>
#include <register_includes/reg.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

// function to capture packets transmitted from egress pipeline
int test_rmt_pkt_coord_tx_pkts = 0;
void test_rmt_pkt_coord_tr_pkt(int asic_id, int port, uint8_t* buf, int len) {
  test_rmt_pkt_coord_tx_pkts++;
}

// provides chip-agnostic set up
class BFN_TEST_NAME(RmtPacketCoordinator) : public BaseTest {
 public:
  RmtPacketCoordinator* rmt_packet_coordinator_;

  void SetUp() override {
    set_pipe_index(0);
    BaseTest::SetUp();
    rmt_packet_coordinator_ = om_->packet_coordinator_get();
    rmt_packet_coordinator_->stop();  // initially stop rmt packet coordinator
    rmt_packet_coordinator_->set_tx_fn(test_rmt_pkt_coord_tr_pkt);
    om_->pktgen_lookup(pipe_index())->set_test(true);
    test_rmt_pkt_coord_tx_pkts = 0;
  }

  void TearDown() override {
    rmt_packet_coordinator_->set_tx_fn(nullptr);
    tu_->quieten_log_flags();
    BaseTest::TearDown();
  }

  void send_packets(
      int ingress_port,
      int egress_port,
      uint32_t n_pkts,
      uint64_t bypass_mode=0x0) {
    while (n_pkts) {
      Packet* pkt = om_->pkt_create("0123456789abcdef"); // 8 bytes
      pkt->set_ingress();
      pkt->set_port(om_->port_get(ingress_port));
      pkt->i2qing_metadata()->set_physical_ingress_port(ingress_port);
      pkt->i2qing_metadata()->set_egress_unicast_port(egress_port);
      pkt->i2qing_metadata()->set_bypass_egr_mode(bypass_mode);
      pkt->set_metadata_added(true);
      rmt_packet_coordinator_->enqueue(ingress_port, pkt, false);
      --n_pkts;
    }
  }

  void run_rmt_packet_coordinator(int expected_pkts_transmitted) {
    // run rmt_packet_coordinator_ until expected number of packets have been
    // captured by the tx function or a max number of sleeps have passed
    rmt_packet_coordinator_->start();
    uint32_t iter = 0;
    while ((test_rmt_pkt_coord_tx_pkts<expected_pkts_transmitted) && (iter<1500)) {
      std::this_thread::sleep_for(std::chrono::microseconds(500));
      ++iter;
    }
    rmt_packet_coordinator_->stop();
  }
};

}

#endif //_UTESTS_TEST_RMT_PACKET_COORDINATOR_H
