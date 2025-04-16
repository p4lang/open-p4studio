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

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <algorithm>
#include <random>
#include <common/bounded_queue.h>
#include <model_core/timer-manager.h>
#include <model_core/timer.h>
#include <pktgen-reg.h>
#include <pktgen-app-reg.h>
#include <register_adapters.h>
#include <mcn_test.h>
#include <rmt-log.h>

namespace MODEL_CHIP_NAMESPACE {

PktGenAppReg::PktGenAppReg(int chip, int pipe, RmtObjectManager* om, int app_id, PktGenReg* pgr) :
    PipeObject(om,pipe),
    pktgen_reg_(pgr),
    app_id_(app_id),
    enabled_(0),
    g_done_(false),
    started_(false),
    recir_done_(false),
    timed_events_timer_([this](uint64_t tid) { this->main_cb(tid); }),
    ing_p_(0),

    ibg_timer_([&](uint64_t tid) { this->batch_cb(); }),
    ipg_timer_([&](uint64_t tid) { this->packet_cb(); }),
    ibg_jitter_( ((app_id+47)*7) ),       /*rng seed*/
    ipg_jitter_( ((app_id+47)*7) + 999 ), /*rng seed*/
    p_count_(0),
    b_count_(0),
    pending_events_(0),

    ctrl_(default_adapter(ctrl_,chip, pipe, app_id, [this](){this->ctrl_callback();})),
    event_number_(default_adapter(event_number_,chip,pipe,app_id)),
    event_timer_(default_adapter(event_timer_,chip,pipe,app_id)),
    ingr_port_ctrl_(default_adapter(ingr_port_ctrl_,chip,pipe,app_id)),
    payload_ctrl_(default_adapter(payload_ctrl_,chip,pipe,app_id)),
    recir_match_mask_(default_adapter(recir_match_mask_,chip,pipe,app_id)),
    recir_match_value_(default_adapter(recir_match_value_,chip,pipe,app_id)),

    ibg_event_base_jitter_value_(default_adapter(ibg_event_base_jitter_value_,chip,pipe,app_id,
                                                register_classes::PgrAppEventBaseJitterValue::PgrAppRegRspecEnum::kEventIbgJitterBaseValue)),
    ibg_event_max_jitter_(default_adapter(ibg_event_max_jitter_,chip,pipe,app_id,
                                         register_classes::PgrAppEventMaxJitter::PgrAppRegRspecEnum::kEventMaxIbgJitter)),

    ibg_event_jitter_scaling_(default_adapter(ibg_event_jitter_scaling_,chip,pipe,app_id,
                                             register_classes::PgrAppEventJitterScaling::PgrAppRegRspecEnum::kEventIbgJitterScale)),

    ipg_event_base_jitter_value_(default_adapter(ipg_event_base_jitter_value_,chip,pipe,app_id,
                                                register_classes::PgrAppEventBaseJitterValue::PgrAppRegRspecEnum::kEventIpgJitterBaseValue)),
    ipg_event_max_jitter_(default_adapter(ipg_event_max_jitter_,chip,pipe,app_id,
                                         register_classes::PgrAppEventMaxJitter::PgrAppRegRspecEnum::kEventMaxIpgJitter)),

    ipg_event_jitter_scaling_(default_adapter(ipg_event_jitter_scaling_,chip,pipe,app_id,
                                             register_classes::PgrAppEventJitterScaling::PgrAppRegRspecEnum::kEventIpgJitterScale)),

    app_recirc_port_src_(default_adapter(app_recirc_port_src_,chip,pipe)),

    ctr48_batch_(default_adapter(ctr48_batch_,chip,pipe,app_id)),
    ctr48_packet_(default_adapter(ctr48_packet_,chip,pipe,app_id)),
    ctr48_trigger_(default_adapter(ctr48_trigger_,chip,pipe,app_id))
{
  reset();
}

PktGenAppReg::~PktGenAppReg() {
  stop();
}

void PktGenAppReg::new_batch() {

  if (g_done_) return;

  // should not get called if all batches have been sent
  //RMT_ASSERT(b_count_.load() < batches_per_event());
  RMT_LOG_VERBOSE( "PktGenAppReg: APP:%d_%d: new batch\n", pipe_index(), app_id_);

  ibg_timer_.stop();

  p_count_ = 0;
  new_packet();
}


void PktGenAppReg::new_packet() {
  if (g_done_) return;

  gen_packet(b_count_,p_count_,current_event_data_);
  p_count_ ++;

  if (p_count_.load() == packets_per_batch()) {
    // last packet in batch
    b_count_++;
    if (b_count_.load() == batches_per_event()) {
      // last batch from this event
      bool run_new_batch=false;
      {
        // check for pending events, if there are any reset the batch stuff and trigger and new batch
        std::lock_guard<std::mutex> lock(pending_events_mutex_);
        if (g_done_) return;
        pending_events_--;
        RMT_LOG_VERBOSE( "PktGenAppReg: APP:%d_%d: done with event, pending_events %d\n", pipe_index(), app_id_, pending_events_);
        RMT_ASSERT( pending_events_ >= 0 );
        if (pending_events_ > 0) {
          RMT_LOG_VERBOSE( "PktGenAppReg: APP:%d_%d: pending_events %d\n", pipe_index(), app_id_, pending_events_);
          b_count_=0;
          current_event_data_ = event_data_.front();
          event_data_.pop();
          run_new_batch=true;
        }
      }
      bool is_port_down = ctrl_.app_type() == TRIGGER_TYPE_E::PORT_DOWN;
      if ( is_port_down ) {
        RMT_ASSERT( pending_events_ == 0 ); // port down events are not queued
        uint32_t down_port = Port::get_pipe_local_port_index( current_event_data_.down_port );
        pktgen_reg_->port_down_set_sent( down_port );
        int next_port_down = pktgen_reg_->get_next_port_down_unsent( down_port, get_port_down_mask_sel() );
        if (next_port_down>=0) {
          RMT_LOG_VERBOSE( "PktGenAppReg: APP:%d_%d: next port %d TRIGGERED\n", pipe_index(), app_id_, next_port_down);
          do_port_down_event(next_port_down);
        }
      }
      if (run_new_batch) {
        new_batch();
      }
      return;
    }
    else {
      // not last batch from event - restart the inter batch timer now as the time is relative
      //   to the end of the last batch of the batch
      uint64_t batch_timeout = ibg_jitter_.get_jittered();
      uint64_t tm;
      model_timer::ModelTimerGetTime(tm);
      RMT_LOG_VERBOSE( "PktGenAppReg: CLK:%" PRIu64 " APP:%d_%d: batch timer %d\n", tm, pipe_index(), app_id_, batch_timeout);
      ibg_timer_.run(batch_timeout, model_timer::Timer::Once);
      return;
    }
  }
  else {
    ipg_timer_.stop();
    uint64_t packet_timeout = ipg_jitter_.get_jittered();
    RMT_LOG_VERBOSE( "PktGenAppReg: APP:%d_%d: packet timer %d\n", pipe_index(), app_id_, packet_timeout);
    ipg_timer_.run(packet_timeout, model_timer::Timer::Once);
  }

}

void PktGenAppReg::packet_cb() {
  if (g_done_) return;
  RMT_LOG_VERBOSE( "PktGenAppReg: APP:%d_%d: packet_cb\n", pipe_index(), app_id_);
  new_packet();
}

void PktGenAppReg::batch_cb() {
  if (g_done_) return;
  RMT_LOG_VERBOSE( "PktGenAppReg: APP:%d_%d: batch_cb\n", pipe_index(), app_id_);
  new_batch();
}

void PktGenAppReg::run_packet_timers(const PktGenEventData& event_data) {
  if (g_done_) return;

  bool is_port_down = ctrl_.app_type() == TRIGGER_TYPE_E::PORT_DOWN;

  bool run_new_batch=false;
  {
    std::lock_guard<std::mutex> lock(pending_events_mutex_);
    if ( pending_events_ == 0 ) {
      run_new_batch = true;
      current_event_data_ = event_data;
      pending_events_=1;
    }
    else {
      // Port down events are not queued here, we have to monitor which
      //  have sent in PktGenReg
      if (! is_port_down ) {
        if (pending_events_ < kEventFifoDepth) {
          event_data_.push( event_data );
          pending_events_++;
        }
        else {
          RMT_LOG_VERBOSE( "PktGenAppReg: APP:%d_%d: DROPPING event\n", pipe_index(), app_id_);
        }
      }
      else {
        RMT_LOG_VERBOSE( "PktGenAppReg: APP:%d_%d: port down event while pending=%d\n", pipe_index(), app_id_, pending_events_);
      }
    }
    RMT_LOG_VERBOSE( "PktGenAppReg: APP:%d_%d: start pending_events %d\n", pipe_index(), app_id_, pending_events_);
  }

  if (run_new_batch) {
    if ( is_port_down ) {
      uint32_t down_port = Port::get_pipe_local_port_index( current_event_data_.down_port );
      pktgen_reg_->update_port_down_registers_for_event(down_port,get_port_down_mask_sel());
      inc_counter(ctr48_trigger_);
    }

    ibg_timer_.stop();
    ipg_timer_.stop();
    b_count_ = 0;
    p_count_ = 0;

    // set the RNG parameters from the registers if Inter Batch Gap
    //   and Inter Packet Gap
    set_ibg_params( ibg_event_base_jitter_value_.value(),
                    ibg_event_max_jitter_.value(),
                    ibg_event_jitter_scaling_.value() );
    set_ipg_params( ipg_event_base_jitter_value_.value(),
                    ipg_event_max_jitter_.value(),
                    ipg_event_jitter_scaling_.value() );
    b_count_ = 0;
    new_batch();
  }
}

void PktGenAppReg::gen_packet(int b_count, int p_count,const PktGenEventData& event_data) {

  // get payload
  int size,addr;
  if ( payload_ctrl_.app_recirc_extract() ) {
    // from the recir packet
    RMT_ASSERT( ctrl_.app_type() == TRIGGER_TYPE_E::PACKET_RECIRC );
    int extract_location = payload_ctrl_.app_payload_addr();
    RMT_ASSERT((extract_location>=0) && (extract_location<=12));
    uint64_t extracted = event_data.key.get_word( (13-extract_location)*8, 24 );
    addr = (extracted>>kExtractedAddressShift) & kExtractedAddressMask;
    size = (extracted>>kExtractedSizeShift)    & kExtractedSizeMask;
  }
  else if ( ctrl_.app_type() == TRIGGER_TYPE_E::DEPARSER ) {
    // get the deparser metadata, which was stashed in the event data
    //  when the trigger happened
    addr = event_data.addr;
    size = event_data.length;
  }
  else {
    addr = payload_ctrl_.app_payload_addr();
    size = payload_ctrl_.app_payload_size();
  }
  Packet* e_packet = pktgen_reg_->make_packet_from_buffer(addr,size,ctrl_.app_chnl());

  // Reset the port number to the configured base at the start of each batch.
  if ( p_count == 0) {
    ing_p_ = ingr_port_ctrl_.app_ingr_port();
    inc_counter(ctr48_batch_);
  }

  inc_counter(ctr48_packet_);

  // Used in META0
  // In test mode only it is used in i2qing_metadata()->set_egress_unicast_port()
  int  pipe_and_port = (ingr_port_ctrl_.app_ingr_port_pipe_id()<<7) | ing_p_;

  std::vector<uint8_t> p_header;

  uint64_t timestamp = UINT64_C(0);
  model_timer::ModelTimerGetTime(timestamp);
  e_packet->set_generated_T(timestamp);
  //printf("PktGenAppReg: pipe_and_port=0x%x(%d) ts=%" PRId64 "\n", pipe_and_port, pipe_and_port, timestamp);

  if (RmtObject::is_jbayXX()) {
    // Only add Meta0/Meta1 on JBay - not on WIP - IPB does it on WIP

    // Add the meta0 header
    uint64_t meta0[2];
    meta0[0] = 0;
    meta0[1] = (static_cast<uint64_t>(pipe_and_port) << 48) | (timestamp & UINT64_C( 0xFFFFFFFFFFFF ));
    for (int i=0;i<2;++i) {
      for (int j=56;j>=0;j-=8) { // msb first
        p_header.push_back( (meta0[i] >> j) & 0xFF );
      }
    }

    // This is Meta1
    constexpr int kPhase0EntrySize=16;
    uint8_t phase0_metadata[kPhase0EntrySize];
    pktgen_reg_->get_phase0_metadata(ing_p_, kPhase0EntrySize, phase0_metadata);

    // Copy the phase0 data in the sram to the generated packet.
    for (int i = kPhase0EntrySize - 1; i >= 0; --i) {
      p_header.push_back(phase0_metadata[i]);
    }

    RMT_LOG_VERBOSE("PktGenAppReg: APP:%d_%d: packet to port %d ts=%" PRId64 " b=%d/%d p=%d/%d\n", pipe_index(),app_id_, pipe_and_port, timestamp,
                  b_count+1, batches_per_event(), p_count+1, packets_per_batch() );
    RMT_LOG_VERBOSE("PktGenAppReg: APP:%d_%d: meta1 = %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x%02x%02x%02x%02x\n",pipe_index(), app_id_,
                    phase0_metadata[0],phase0_metadata[1],phase0_metadata[2],phase0_metadata[3],
                    phase0_metadata[4],phase0_metadata[5],phase0_metadata[6],phase0_metadata[7],
                    phase0_metadata[8],phase0_metadata[9],phase0_metadata[10],phase0_metadata[11],
                    phase0_metadata[12],phase0_metadata[13],phase0_metadata[14],phase0_metadata[15]);
  } else {
    // WIP or after
    RMT_LOG_VERBOSE("PktGenAppReg: APP:%d_%d: packet to port %d ts=%" PRId64 " b=%d/%d p=%d/%d (Meta0/Meta1 added later by IPB)\n",
                    pipe_index(),app_id_, pipe_and_port, timestamp, b_count+1, batches_per_event(), p_count+1, packets_per_batch() );
  }

  int pkt_gen_hdr_start= p_header.size();
  for (int i=0;i<6;++i) { // all pkt_gen headers are at least 6 bytes
      p_header.push_back( 0 );
  }
  p_header[pkt_gen_hdr_start+5] = (p_count) & 0xFF;
  p_header[pkt_gen_hdr_start+4] = (p_count >> 8) & 0xFF;
  switch(ctrl_.app_type()) {
    case TRIGGER_TYPE_E::ONETIME:
    case TRIGGER_TYPE_E::PERIODIC:
      p_header[pkt_gen_hdr_start+3] = (b_count) & 0xFF;
      p_header[pkt_gen_hdr_start+2] = (b_count >> 8) & 0xFF;
      p_header[pkt_gen_hdr_start+1] = 0;
      break;
    case TRIGGER_TYPE_E::PORT_DOWN:
      {
        // Note get pipe from common regs in this case only
        auto gen_pipe = pktgen_reg_->get_gen_pipe();
        uint32_t down_port = Port::get_pipe_local_port_index( event_data.down_port);
        p_header[pkt_gen_hdr_start+3] = ((gen_pipe & 1) << 7) | down_port; // bottom bit of pipe, port
        p_header[pkt_gen_hdr_start+2] = ((gen_pipe & 2) >> 1); // top bit of pipe
        p_header[pkt_gen_hdr_start+1] = 0;
      }
      break;


    case TRIGGER_TYPE_E::PACKET_RECIRC:
    case TRIGGER_TYPE_E::DEPARSER:
      p_header[pkt_gen_hdr_start+3] = (b_count)&0xFF;
      p_header[pkt_gen_hdr_start+2] = (b_count >> 8)&0XFF;
      if (! ctrl_.app_nokey() ) {
        constexpr int kBytesInKey = kMatchWidth / 8;
        for (int i = kBytesInKey-1; i >= 0 ; i-- ) { // first byte is most significant
          uint8_t b = event_data.key.get_byte( i );
          p_header.push_back( b );
        }
      }
      break;
    case TRIGGER_TYPE_E::PFC_CHANGE_EVENT:
      // PFC isn't supported in the model as we don't have a proper TM
      RMT_LOG_WARN("PktGenAppReg: APP:%d_%d: PFC event not supported\n",pipe_index(), app_id_ );
      // 4 and 5 don't get set for PFC, they are zero in this one case
      p_header[pkt_gen_hdr_start+5] = 0;
      p_header[pkt_gen_hdr_start+4] = 0;
      break;
  }

  static_assert( RmtDefs::kPktGenApps == 16 , "header byte construction assumes 16 apps" );
  p_header[pkt_gen_hdr_start+0] = (app_id_ & 0xf) | (ingr_port_ctrl_.app_ingr_port_pipe_id() << 4);
  auto p_header_pb = new PacketBuffer(p_header.data(), p_header.size());
  e_packet->prepend(p_header_pb);
  if (pktgen_reg_->pktgen_->get_test()) {
    e_packet->i2qing_metadata()->set_egress_unicast_port(pipe_and_port);
  }
  // Physical ingress port is set to the generated port
  e_packet->i2qing_metadata()->set_physical_ingress_port(pipe_and_port);

  if ( ingr_port_ctrl_.app_ingr_port_inc() ) {
    // This will send a packet to wrap port, I checked this is correct.
    if (ing_p_ == ingr_port_ctrl_.app_ingr_port_wrap()) {
      ing_p_ = ingr_port_ctrl_.app_ingr_port();
    }
    else {
      // XXX: WIP: no odd ports so increment by 2
      const uint16_t inc_val = is_chip1() ?2 :1;
      ing_p_ = (ing_p_ + inc_val) & kIngressPortMask;
    }
  }

  RMT_LOG_VERBOSE("PktGenAppReg: APP:%d_%d: ch=%d PktHash=0x%08x\n",pipe_index(), app_id_, ctrl_.app_chnl(), e_packet->hash() );

  if (! pktgen_reg_->enqueue_generated_packet( ctrl_.app_chnl(), std::move(e_packet) )) {
    delete(e_packet);
    e_packet = nullptr;
  }
}

bool PktGenAppReg::snoop(BitVector<kMatchWidth> val, bool recirculation, int channel) {

  if (!enabled_) return false;

  if ( recirculation ) {
    if (ctrl_.app_type() != TRIGGER_TYPE_E::PACKET_RECIRC)
      return false;
    // check recirc trigger is set for this channel
    if ( 0 == ((pktgen_reg_->recirc_trigger_channel_enable_mask() >> channel) & 1))
      return false;
    // check if we are configured to snoop on this ebuf
    int ebuf = channel / 2;
    if (get_app_recirc_ebuf_src() != ebuf) {
      return false;
    }
  }
  else {
    if (ctrl_.app_type() != TRIGGER_TYPE_E::DEPARSER)
      return false;
  }
  BitVector < kMatchWidth > pkt_masked_key;
  BitVector < kMatchWidth > app_masked_key;
  pkt_masked_key.copy_from( val );
  // register fields are called recir_* but are also for deparser events
  pkt_masked_key.or_with( recir_match_mask_.recir_match_mask() );  // this could be done at app setup time? Or in a callback.
  app_masked_key.copy_from( recir_match_value_.recir_match_value() );
  app_masked_key.or_with( recir_match_mask_.recir_match_mask() );

  if ( pkt_masked_key.equals(app_masked_key) ) {
    RMT_LOG_VERBOSE("PktGenAppReg: APP:%d_%d: snoop %s hit pkt_val=0x%016" PRIx64 "%016" PRIx64
                                                       " val=0x%016" PRIx64 "%016" PRIx64
                                                      " mask=0x%016" PRIx64 "%016" PRIx64
                    "\n", pipe_index(),app_id_, recirculation ? "recirc":"deparser",
                    val.get_word(64),val.get_word(0),
                    recir_match_value_.recir_match_value().get_word(64),recir_match_value_.recir_match_value().get_word(0),
                    recir_match_mask_.recir_match_mask().get_word(64),recir_match_mask_.recir_match_mask().get_word(0)     );

    return true;
  }
  else {
    RMT_LOG_VERBOSE("PktGenAppReg: APP:%d_%d: snoop %s MISS pkt_val=0x%016" PRIx64 "%016" PRIx64
                                                       " val=0x%016" PRIx64 "%016" PRIx64
                                                      " mask=0x%016" PRIx64 "%016" PRIx64
                    "\n", pipe_index(),app_id_, recirculation ? "recirc":"deparser",
                    val.get_word(64),val.get_word(0),
                    recir_match_value_.recir_match_value().get_word(64),recir_match_value_.recir_match_value().get_word(0),
                    recir_match_mask_.recir_match_mask().get_word(64),recir_match_mask_.recir_match_mask().get_word(0)     );

    return false;
  }
}


void PktGenAppReg::main_cb(uint64_t tid) {
  if (g_done_) return;

  uint64_t tm;
  model_timer::ModelTimerGetTime(tm);

  RMT_LOG_VERBOSE("PktGenAppReg: CLK:%" PRIu64 " APP:%d_%d:main_cb\n", tm, pipe_index(),app_id_);
  do_event({});
}


void PktGenAppReg::start() {
  RMT_LOG_VERBOSE("PktGenAppReg: APP:%d_%d: start()\n", pipe_index(), app_id_ );
  if (!started_) {
    if((ctrl_.app_type() == TRIGGER_TYPE_E::ONETIME) ||
       (ctrl_.app_type() == TRIGGER_TYPE_E::PERIODIC)) {
      run_timed_events_timer();
    }
    g_done_ = false;
    started_ = true;

    retrigger_port_down();
  }
}


void PktGenAppReg::stop() {
  RMT_LOG_VERBOSE("PktGenAppReg: APP:%d_%d: stop()\n", pipe_index(), app_id_ );
  std::lock_guard<std::mutex> lock(pending_events_mutex_);
  g_done_ = true;
  started_ = false;

  switch(ctrl_.app_type()) {
    case TRIGGER_TYPE_E::ONETIME:
    case TRIGGER_TYPE_E::PERIODIC:
      timed_events_timer_.stop();
      break;
    case TRIGGER_TYPE_E::PORT_DOWN:
    case TRIGGER_TYPE_E::PACKET_RECIRC:
    case TRIGGER_TYPE_E::DEPARSER:
      // done straight away, so nothing to stop
      break;
  }

  // stop packet timers
  ibg_timer_.stop();
  ipg_timer_.stop();
  b_count_ = 0;
  p_count_ = 0;

  pending_events_ = 0;
  current_event_data_.reset();
  // clear the event_data_ queue by swapping with empty queue
  std::queue<PktGenEventData> empty;
  std::swap( event_data_, empty );

}



void PktGenAppReg::reset() {
  ctrl_.reset();
  event_number_.reset();
  event_timer_.reset();
  ingr_port_ctrl_.reset();
  payload_ctrl_.reset();
  recir_match_mask_.reset();
  recir_match_value_.reset();
  ibg_event_base_jitter_value_.reset();
  ibg_event_max_jitter_.reset();
  ibg_event_jitter_scaling_.reset();
  ipg_event_base_jitter_value_.reset();
  ipg_event_max_jitter_.reset();
  ipg_event_jitter_scaling_.reset();
  app_recirc_port_src_.reset();

  ctr48_batch_.reset();
  ctr48_packet_.reset();
  ctr48_trigger_.reset();
}

void PktGenAppReg::retrigger_port_down() {
  if (ctrl_.app_type() == TRIGGER_TYPE_E::PORT_DOWN) {
    int next_port_down = pktgen_reg_->get_next_port_down_unsent( 0, get_port_down_mask_sel() );
    RMT_LOG_VERBOSE("PktGenAppReg: %d_%d retrigger_port_down mask=%d next=%d\n",pipe_index(),app_id_,
                    get_port_down_mask_sel(), next_port_down);
    if (next_port_down>=0) {
      do_port_down_event(next_port_down);
    }
  }
}


void PktGenAppReg::do_port_down_event(const int port) {
  PktGenEventData event_data{};
  event_data.down_port = port;
  do_event(event_data);
}

void PktGenAppReg::do_event(const PktGenEventData& event_data) {
  if (g_done_ || !started_) return;
  uint64_t ev_num = pktgen_reg_->get_next_ev_num();
  uint64_t tm;

  RMT_LOG_VERBOSE("PktGenAppReg: APP:%d_%d: do_event %" PRId64 " app_type=%d\n", pipe_index(), app_id_, ev_num, ctrl_.app_type() );

  pktgen_reg_->pktgen_->g_triggers_++;
  if (ctrl_.app_type() != TRIGGER_TYPE_E::PORT_DOWN) {
    inc_counter(ctr48_trigger_);
  }
  model_timer::ModelTimerGetTime(tm);

  run_packet_timers(event_data);

  // Schedule next event if this is periodic
  if (!g_done_ && (ctrl_.app_type() == TRIGGER_TYPE_E::PERIODIC)) {
    run_timed_events_timer();
  }

}

void PktGenAppReg::run_timed_events_timer() {
  uint32_t timer_count = event_timer_.timer_count();
  if (timer_count == 0) timer_count=1;
  timed_events_timer_.stop();
  timed_events_timer_.run(timer_count, model_timer::Timer::Once);
}

void PktGenAppReg::ctrl_callback() {
  start_or_stop();
}

void PktGenAppReg::start_or_stop() {
  enabled_ = ctrl_.app_en();
  int ch = ctrl_.app_chnl();
  // Note that this only checks the channel is enabled (in ipb_port_ctrl_.pgen_channel_en)
  //  when app is enabled, because csr file says pgen_channel_en must be programmed
  //  before any app is enabled
  RMT_LOG_VERBOSE("PktGenAppReg: APP:%d_%d: start_or_stop %senabled : ch=%d %senabled\n", pipe_index(), app_id_,
                  enabled_?"":"not ", ch, pktgen_reg_->channel_enabled_for_packet_gen(ch)?"":"not ");
  if (enabled_ && pktgen_reg_->channel_enabled_for_packet_gen(ch)) {
    start();
  }
  else {
    stop();
  }
}



} // namespace MODEL_CHIP_NAMESPACE
