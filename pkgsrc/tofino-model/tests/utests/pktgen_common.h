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

#ifndef _PKTGEN_COMMON_
#define _PKTGEN_COMMON_

#include <utests/test_util.h>
#include <iostream>
#include <vector>
#include <string>

#include "gtest.h"

#include <model_core/model.h>
#include <model_core/timer-manager.h>
#include <pktgen.h>
#include <pktgen-reg.h>
#include <rmt-object-manager.h>
#include <packet.h>
#include <rmt-packet-coordinator.h>
#include <port.h>
#include <crafter/Crafter.h>
#include <crafter/Utils/CrafterUtils.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

// Alternative pipe process function for testing packet gen - take packet from
// port and make it come out of the egress pipe where it can be recirculated
void install_fake_pipe_process_fn(RmtObjectManager *rmt,
                                  RmtPacketCoordinator *rc) {
  pipe_process_fn_t pipe_process_fn = [rmt](
      Packet *ingr_pkt,
      Packet *egr_pkt,
      Packet **queued_pkt,
      Packet **sent_pkt,
      Packet **resubmit_pkt,
      Packet **ing_mirror_pkt,
      Packet **egr_mirror_pkt
  ) {
    *queued_pkt = NULL;
    *sent_pkt = ingr_pkt;
    if (nullptr != ingr_pkt) {
      ingr_pkt->set_metadata_added(true);
      ingr_pkt->set_egress();
      ingr_pkt->set_port(
          rmt->port_get(ingr_pkt->i2qing_metadata()->egress_uc_port()));
    }
  };
  rc->set_pipe_process_fn(pipe_process_fn);
}

void tr_packet(int asic_id, int port, uint8_t* buf, int len) {
}

}
#endif // _PKTGEN_COMMON_
