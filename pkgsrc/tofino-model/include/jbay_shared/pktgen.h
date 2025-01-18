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

#ifndef _JBAY_SHARED_PKTGEN_H
#define _JBAY_SHARED_PKTGEN_H

#include <cstdint>
#include <iostream>
#include <array>
#include <cstring>
#include <map>
#include <vector>
#include <utility>
#include <map>
#include <atomic>
#include <common/rmt-assert.h>
#include <common/rmt-util.h>
#include <model_core/timer.h>
#include <memory>
#include <rmt-defs.h>
#include <pipe-object.h>
#include <rmt-object-manager.h>
#include <port.h>
#include <packet.h>
#include <pktgen-reg.h>
#include <packet-gen-metadata.h>

/*
  Things that are not implemented:
  - control register:
    - app_prio
    - app_stop_at_pkt_bndry
    - app_sel_cur_timestamp
  - all logging functions
  - priority functions
  - timestamp offsets
  - credits
  - deficit weighted round robin
  - PFC change events
  - event overflow count
  - swap_en (swapping bytes in buffer memory)
 */

namespace MODEL_CHIP_NAMESPACE {

class PacketEnqueuer;

class PktGen : public PipeObject {

  static constexpr int kMaxRecirculations = 1; // can only recirculate once on JBay

 public:
  PktGen (RmtObjectManager* om, PacketEnqueuer* pe, int pipe_index);
  virtual ~PktGen();

  // Can be called on a port that doesn't do recirculation, in which case
  //  it will return false and not modify the packet or packet pointer
  // For a port that does support recirculation, it can:
  //  return true, and a valid packet pointer -> recirculate
  //  return false and a valid packet pointer -> enqueue packet normally
  //  return false and a null packet pointer -> don't enque anything
  bool maybe_recirculate(Packet** r_packet);
  void stop();

  // Maybe trigger the Deparser/MAU event
  void maybe_trigger(PacketGenMetadata* m);

  void handle_port_down(uint16_t port);
  void handle_port_up(uint16_t port);

  // This is used to get a pointer to the port that generated packets come
  //   from - In Tofino it is 17, I think in JBay it will be 0.
  // used in src/shared/pktgen-app-reg.cpp
  uint16_t get_port_num() { return 0; }

  inline void set_test(bool val) { testing_ = val; }
  inline bool get_test() { return testing_; }

  // Only used so unit tests can intercept the enqueuer
  void set_enqueuer(PacketEnqueuer* pe) { pgen_reg_.set_enqueuer(pe); }

  uint64_t g_config_drop_cnt_;
  uint64_t g_config_recirc_drop_cnt_;
  uint64_t g_recirc_cnt_;
  uint64_t g_pgen_cnt_;
  uint64_t g_mxbar_cnt_;
  uint64_t g_drop_cnt_;
  uint64_t g_recirc_drop_cnt_;
  uint64_t g_triggers_;
  // RmtObjectManager* get_om() { return _om; }

 private:
  DISALLOW_COPY_AND_ASSIGN(PktGen);

  bool process_recirc(int channel, Packet** in_pkt, bool* recir_done);
  void update_recirc_packet(Packet* in_pkt);

  RmtObjectManager* om_;
  bool testing_;

  PktGenReg pgen_reg_;

};

};
#endif
