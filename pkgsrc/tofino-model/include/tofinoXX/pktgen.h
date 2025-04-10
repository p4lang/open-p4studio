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

#ifndef _TOFINOXX_PKTGEN_H
#define _TOFINOXX_PKTGEN_H

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


namespace MODEL_CHIP_NAMESPACE {


class PktGenReg;
class PacketEnqueuer;

class PktGen : public PipeObject {

 public:

  PktGen (RmtObjectManager* om, PacketEnqueuer* pe, int pipe_id);

  // Can be called on a port that doesn't do recirculation, in which case
  //  it will return false and not modify the packet or packet pointer
  // For a port that does support recirculation, it can:
  //  return true, and a valid packet pointer -> recirculate
  //  return false and a valid packet pointer -> enqueue packet normally
  //  return false and a null packet pointer -> don't enque anything
  bool maybe_recirculate(Packet** r_packet);
  void stop(void);

  void maybe_trigger(PacketGenMetadata* metadata) { /* do nothing on Tofino */ }

  uint32_t get_def_recir_count(void) { return def_recir_cnt_; }
  //void set_def_recir_count(uint32_t cnt) { def_recir_cnt_ = cnt; }
  void handle_port_down(uint16_t port);
  void handle_port_up(uint16_t port);

  // PktGenReg* get_regs(void) { return _pgen_reg; }
  // used in src/shared/pktgen-app-reg.cpp - will always be 17
  uint16_t get_port_num(void) { return RmtDefs::kPktGen_P17; }

  // Dtor
  virtual ~ PktGen ();

  // inline void set_tdm(bool val) { tdm_en_ = val; }
  inline bool get_tdm(void) { return tdm_en_; }
  inline void set_test(bool val) { testing_ = val; }
  inline bool get_test() { return testing_; }
  // inline void set_limited_buffer(bool val) { limited_buffer_ = val; }
  // inline bool get_limited_buffer(void) { return limited_buffer_; }

  uint64_t g_config_drop_cnt_;
  uint64_t g_config_recirc_drop_cnt_;
  uint64_t g_recirc_cnt_;
  uint64_t g_pgen_cnt_;
  uint64_t g_mxbar_cnt_;
  uint64_t g_drop_cnt_;
  uint64_t g_recirc_drop_cnt_;
  uint64_t g_triggers_;
  // RmtObjectManager* get_om() { return _om; }
  void update_recirc_packet(Packet* in_pkt);


 private:
  PktGen& operator=(const PktGen& other)=delete;
  PktGen(const PktGen& other)=delete;
  bool process_recirc(int which, Packet** in_pkt, bool* recir_done);

  RmtObjectManager* om_;
  uint16_t m_pipe;
  bool tdm_en_;
  bool testing_;


  std::atomic <unsigned int> avail_buffer_;
  //bool limited_buffer_;

  uint32_t def_recir_cnt_;
  std::array<PktGenReg*,2> pgen_reg_;

};

};
#endif
