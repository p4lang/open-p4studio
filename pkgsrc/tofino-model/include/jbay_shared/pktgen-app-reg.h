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

#ifndef __JBAY_PKTGEN_APP_REG_H
#define __JBAY_PKTGEN_APP_REG_H

#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <register_includes/pgr_app_ctr48_batch_mutable.h>
#include <register_includes/pgr_app_ctr48_packet_mutable.h>
#include <register_includes/pgr_app_ctr48_trigger_mutable.h>
#include <register_includes/pgr_app_ctrl.h>

#include <register_includes/pgr_app_event_base_jitter_value.h>
#include <register_includes/pgr_app_event_max_jitter.h>
#include <register_includes/pgr_app_event_jitter_scaling.h>

#include <register_includes/pgr_app_event_number.h>
#include <register_includes/pgr_app_event_timer.h>
#include <register_includes/pgr_app_ingr_port_ctrl.h>
#include <register_includes/pgr_app_payload_ctrl.h>
#include <register_includes/pgr_app_recir_match_mask.h>
#include <register_includes/pgr_app_recir_match_value.h>
#include <register_includes/pgr_port_down_dis_mutable.h>
#include <register_includes/pgr_cfg_app_recirc_port_src.h>

#include <common/rmt-util.h>
#include <model_core/timer-manager.h>
#include <pipe-object.h>
#include <packet.h>
#include <pktgen.h>
#include <bitvector.h>

namespace MODEL_CHIP_NAMESPACE {


enum TRIGGER_TYPE_E {
  ONETIME          = 0,
  PERIODIC         = 1,
  PORT_DOWN        = 2,
  PACKET_RECIRC    = 3,
  DEPARSER         = 4,
  PFC_CHANGE_EVENT = 5,
  UNDEF            = 6
} ;


class PktGenJitter {
  using DistType = std::uniform_int_distribution<uint32_t>;
 public:
  PktGenJitter(uint32_t seed)
      : base_(0), scale_(0),
        generator_(seed),
        distribution_(new DistType(0,0xFFFFFFFF)) {
  }
  ~PktGenJitter() { }
  void set_params(uint32_t base, uint32_t max, int scale) {
    // the timers can't handle a value of zero
    if (base==0) {
      base = 1;
      if (max>0) {
        max--;
      }
    }
    distribution_.reset( new DistType(0,max) );
    base_  = base;
    scale_ = scale;
    //max_ = max;
  }

  uint32_t get_jittered() {
    uint32_t r = ((*distribution_)( generator_ )) << scale_;
    r += base_;
    if ( r < base_ ) r = 0xFFFFFFFF; // saturate at max value
    //printf("get_jittered: base=%d scale=%d max=%d -> %d\n",base_,scale_,max_,r);
    return r;
  }
 private:
  uint32_t base_;
  int scale_;
  //int max_;
  std::default_random_engine generator_;
  std::unique_ptr<DistType> distribution_;
};

class PktGenReg;

class PktGenAppReg : public PipeObject {

 public:
  static constexpr int kMatchWidth = RmtDefs::kPktGenMatchWidth;
  static constexpr int kEventFifoDepth = RmtDefs::kPktGenEventFifoDepth;

  static constexpr int kExtractedAddressShift = 0;
  static constexpr int kExtractedAddressMask  = 0x03ff;
  static constexpr int kExtractedSizeShift    = 10;
  static constexpr int kExtractedSizeMask     = 0x3fff;

  static constexpr int kIngressPortMask = (1 << (RmtDefs::kPortChanWidth + RmtDefs::kPortGroupWidth)) - 1;

  PktGenAppReg(int chip, int pipe_index, RmtObjectManager* om, int app_id, PktGenReg* pgen);

  ~PktGenAppReg();



  void set_packet_payload(uint8_t* buf, uint32_t sz);
  void handle_port_down(uint16_t port);
  void handle_recirc(uint32_t recir_header);
  void start_or_stop();
  void start();
  void stop();
  void gen_packet(int b_count, int p_count, const PktGenEventData& event_data);
  bool snoop(BitVector < kMatchWidth > val, bool recirculation, int channel);

  bool is_port_down_trigger_type() {
    return enabled_ && (ctrl_.app_type() == TRIGGER_TYPE_E::PORT_DOWN);
  }

  void retrigger_port_down();

  void do_event( const PktGenEventData& event_data);
  void do_port_down_event(const int port);

  int  get_port_down_mask_sel() { return ctrl_.app_port_down_mask_sel(); }

  // returns an ebuf number that this app listens top
  int get_app_recirc_ebuf_src() { return app_recirc_port_src_.app_recirc_src( app_id_ ); }

  int batches_per_event() { return event_number_.batch_num() + 1; }
  int packets_per_batch() { return event_number_.packet_num() + 1; }

 private:
  DISALLOW_COPY_AND_ASSIGN(PktGenAppReg);
  PktGenReg* pktgen_reg_;
  uint8_t app_id_;
  uint8_t enabled_;
  bool g_done_;
  bool started_;
  std::atomic<bool>recir_done_;
  model_timer::Timer timed_events_timer_;
  uint16_t ing_p_;

  model_timer::Timer ibg_timer_;
  model_timer::Timer ipg_timer_;
  PktGenJitter       ibg_jitter_;
  PktGenJitter       ipg_jitter_;
  std::atomic<uint16_t> p_count_;
  std::atomic<uint16_t> b_count_;
  int pending_events_;
  std::queue <PktGenEventData> event_data_;
  std::mutex pending_events_mutex_;
  PktGenEventData current_event_data_;

  register_classes::PgrAppCtrl              ctrl_;
  register_classes::PgrAppEventNumber       event_number_;
  register_classes::PgrAppEventTimer        event_timer_;
  register_classes::PgrAppIngrPortCtrl      ingr_port_ctrl_;
  register_classes::PgrAppPayloadCtrl       payload_ctrl_;
  register_classes::PgrAppRecirMatchMask    recir_match_mask_;
  register_classes::PgrAppRecirMatchValue   recir_match_value_;

  register_classes::PgrAppEventBaseJitterValue ibg_event_base_jitter_value_;
  register_classes::PgrAppEventMaxJitter       ibg_event_max_jitter_;
  register_classes::PgrAppEventJitterScaling   ibg_event_jitter_scaling_;
  register_classes::PgrAppEventBaseJitterValue ipg_event_base_jitter_value_;
  register_classes::PgrAppEventMaxJitter       ipg_event_max_jitter_;
  register_classes::PgrAppEventJitterScaling   ipg_event_jitter_scaling_;
  // pgr_cfg_app_recirc_port_sr is in the common space, but contains app specific configuration
  register_classes::PgrCfgAppRecircPortSrc     app_recirc_port_src_;

  register_classes::PgrAppCtr48BatchMutable         ctr48_batch_;
  register_classes::PgrAppCtr48PacketMutable        ctr48_packet_;
  register_classes::PgrAppCtr48TriggerMutable       ctr48_trigger_;


  void ctrl_callback();
  void reset();
  void port_down(uint16_t port);
  void cpu_down();
  bool port_down_ev();
  bool recir_match_ev();
  bool recir_match_packet();
  void clear_ports();
  void run_timed_events_timer();
  void main_cb(uint64_t tid);

  void packet_cb();
  void batch_cb();

  void run_packet_timers(const PktGenEventData& event_data);
  void stop_packet_timers();

  void set_ibg_params(uint32_t base, uint32_t max, int scale) {
    ibg_jitter_.set_params(base,max,scale);
  }
  void set_ipg_params(uint32_t base, uint32_t max, int scale) {
    ipg_jitter_.set_params(base,max,scale);
  }
  void new_batch();
  void new_packet();

  template <typename T> void inc_counter(T& counter) {
    counter.ctr48(  counter.ctr48() + 1 );
  }


};






};

#endif
