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

#include <iomanip>
#include <iostream>
#include <model_core/register_block.h>
#include <rmt-object-manager.h>
#include <packet-enqueuer.h>
#include <pktgen.h>
#include <pktgen-reg.h>
#include <pktgen-app-reg.h>
#include <register_adapters.h>


using namespace std;
using namespace model_common;

namespace MODEL_CHIP_NAMESPACE {

constexpr uint8_t PktGenReg::ch_mode_[kChannelModes][kChannelsPerEthernet];


PktGenReg::PktGenReg(int chip, int pipe, RmtObjectManager* om, PktGen* pgen, PacketEnqueuer *pe)
  : PipeObject(om,pipe),
    pktgen_(pgen),
    om_(om),
    initialized_(false),
    pgen_buffer_(chip,pipe,"PGR Buffer Mem"),
    packet_enqueuer_(pe),
    curr_seq_ch_(0),
    main_thread_(nullptr),
    done_(false),
    pgen_phase0_meta_(chip,pipe,"PGR Phase0 Meta Mem"), // Unused on WIP
    // These queueing parameters were copied from Tofino, they don't seem to
    //  have any relation to the hardware.
    gen_q_{{  {25, 0, 20},{25, 0, 20},{25, 0, 20},{25, 0, 20},
              {25, 0, 20},{25, 0, 20},{25, 0, 20},{25, 0, 20}  }},
    recir_q_{{  {0, 20, 20}, {0, 20, 20}, {0, 20, 20}, {0, 20, 20},
                {0, 20, 20}, {0, 20, 20}, {0, 20, 20}, {0, 20, 20}  }},

    tbc_port_ctrl_(default_adapter(tbc_port_ctrl_,chip,pipe,[this](){this->start_or_stop();})),
    eth_cpu_ctrl_(default_adapter(eth_cpu_ctrl_,chip,pipe,[this](){this->start_or_stop();})),
    ebuf_port_ctrl_(default_adapter(ebuf_port_ctrl_,chip,pipe,[this](uint32_t a0){this->start_or_stop();})),
    pgen_ctrl_(default_adapter(pgen_ctrl_,chip,pipe)),
    port_down_dis_(default_adapter(port_down_dis_,chip,pipe, [this](){this->port_down_dis_callback();})),
    port_down_ctrl_(default_adapter(port_down_ctrl_,chip,pipe)),
    port_down_event_mask_(default_adapter(port_down_event_mask_,chip,pipe)),
    port_down_vec_clr_(default_adapter(port_down_vec_clr_,chip,pipe, [this](){this->port_down_vec_clr_callback();})),
    retrigger_port_down_(default_adapter(retrigger_port_down_,chip,pipe , [this](){this->retrigger_callback();} )),
    ipb_port_ctrl_(default_adapter(ipb_port_ctrl_,chip,pipe,[this](){this->start_or_stop();}))
{
  for (unsigned int i = 0; i < pgen_app_.size(); i ++) {
    pgen_app_[i] = new PktGenAppReg(chip, pipe, om_, i, this);
  }
  reset();
  initialized_=true;
}


PktGenReg::~PktGenReg() {
    stop();
    for (unsigned int i = 0; i < pgen_app_.size(); i ++) {
        delete pgen_app_[i];
        pgen_app_[i] = nullptr;
    }
}

void PktGenReg::check_channel(int ch, bool* recirc, bool* drop) {
  RMT_ASSERT( ch>=0 );

  int eth_ch = ch - 2; // channel within ethernet

  if (( ch == 0 ) && tbc_port_ctrl_.port_en()) {
    *recirc = false;
    *drop   = false;
  }
  else if ((eth_ch>=0) && (eth_ch<=3) && eth_cpu_ctrl_.port_en() &&
           (( eth_cpu_ctrl_.channel_en() >> eth_ch ) & 1 )) {
    // enqueue on Ethernet mac
    RMT_ASSERT( check_eth_channel(eth_ch) );
    *recirc = false;
    *drop   = false;
  }
  else if ( ch<8 ) {
    int ebuf = ch/2;
    if ( ebuf_port_ctrl_.port_en( ebuf ) ) {
      // port enabled for recirc
      int e_ch = ch%2;
      if ((ebuf_port_ctrl_.channel_en( ebuf ) >> e_ch) & 1 ) {
        // recirculate
        *recirc = true;
        *drop =  false;
      }
      else {
        // drop
        *recirc = false;
        *drop   = true;
      }
    }
    else {
      // port not enabled for recirc, enqueue on mac
      *recirc = false;
      *drop   = false;
    }
  }
  else {
    // port not in range 0-7 just enqueue on mac
    *recirc = false;
    *drop   = false;
  }
}

bool PktGenReg::recirc_packet(int channel, Packet** packet) {

  if (!recir_q_[channel].enqueue(std::move(*packet))) {
    return false;
  }

  *packet = nullptr;
  return true;
}


void PktGenReg::reset(void) {
  tbc_port_ctrl_.reset();
  ebuf_port_ctrl_.reset();
  eth_cpu_ctrl_.reset();
  pgen_ctrl_.reset();
  port_down_dis_before_write_.fill_all_ones(); // port_down_dis resets to all ones
  port_down_dis_.reset();
  port_down_ctrl_.reset();
  port_down_event_mask_.reset();
  port_down_vec_clr_.reset();
  retrigger_port_down_.reset();
  ipb_port_ctrl_.reset();
  port_down_vector_[0].fill_all_zeros();
  port_down_vector_[1].fill_all_zeros();
  port_down_sent_.fill_all_zeros();
  ports_currently_down_.fill_all_zeros();
}

uint8_t PktGenReg::get_tdm_ch(void) {
  // nothing ever calls pktgen->set_tdm(), so this will never get called!
  RMT_ASSERT(0);
  //uint64_t c_time;
  //model_timer::ModelTimerGetTime(c_time);
  //uint8_t ch_idx = (c_time % RmtDefs::kChlsPerIpb);
  // Now get the actual channel
//  uint8_t m_channel = /*((pg_port16ctrl.channel_seq())>>(ch_idx << 1))&*/0x3;
//  return m_channel;
}

void PktGenReg::port_down_internal(uint16_t port) {
  // port_down lock is held by caller

  int mb = map_port_to_vector_bit(port);

  if (port_down_dis_.set(mb)) {
    RMT_LOG_VERBOSE("PktGenReg: %d portdown port %d disabled\n",pipe_index(),port);
    return;
  }

  // find if there is a port down app enabled that uses a
  //  mask that has this port's bit set
  // made more complicated because don't want to assert if
  //  both masks set, but no apps use either
  constexpr int kNumMasks = 2;
  int which_mask = -1;
  int which_app = -1;
  for (int m=0;m<kNumMasks;++m) {
    if ( port_down_event_mask( port, m ) ) {
      if (which_app==-1) {
        which_mask = m;
        RMT_LOG_VERBOSE("PktGenReg: mask %d setting bit for port %d\n",which_mask,port);
        port_down_vector_[which_mask].set_bit(mb);
      }
      for (uint i=0; i<pgen_app_.size(); ++i) {
        if ( pgen_app_[i]->is_port_down_trigger_type() &&
             (m == pgen_app_[i]->get_port_down_mask_sel())) {
          RMT_ASSERT( which_app == -1 ); // only one app handles a port
          which_app = i;
        }
      }
    }
  }

  if (which_app >= 0) {
    RMT_LOG_VERBOSE("PktGenReg: triggering app %d\n",which_app);
    pgen_app_[which_app]->do_port_down_event(port);
  }
  else {
    // If app_disable is set we will drop triggers that happen when there
    //   is no app enabled (to be backwards compatible with Tofino) so model this
    //   by clearing the bit.
    if ( port_down_vec_clr_.app_disable() ) {
      RMT_LOG_VERBOSE("PktGenReg: app_disable mode and no apps enabled, clearing bit %d\n",mb);
      port_down_vector_[which_mask].clear_bit(mb);
    }
  }
}

void PktGenReg::port_down(uint16_t port) {
  // Handle only if this port belongs to your pipe

  uint16_t pipe_id = Port::get_pipe_num( port );
  RMT_LOG_VERBOSE("PktGenReg: %d portdown trigger pipe %d port %d\n",pipe_index(),pipe_id, Port::get_pipe_local_port_index(port) );
  if (pipe_index() != pipe_id) {
    return;
  }
  port = Port::get_pipe_local_port_index(port);
  RMT_ASSERT( port < kPortMax );

  {
    std::lock_guard<std::mutex> lock(port_down_mutex_);

    ports_currently_down_.set_bit(port);

    // Check if port down events are enabled for this port
    //   for TBC or Ethernet ports check their own registers, otherwise
    //   use port_down_event_mask later using per-app selector
    // Only for pipe 0
    if ( pipe_index() == 0 ) {
      if (( port == 0 ) && tbc_port_ctrl_.port_en()) {
        // Note: tbc_port_en is 4 bits because there are 4 channels in the hardware,
        //  but there is only port 0 in the model.
        if ( 0 == port_down_ctrl_.tbc_port_en() ) {
          RMT_LOG_VERBOSE("PktGenReg: tbc_port_en not enabled\n");
          return;
        }
      }
      else if ((port>=2) && (port<=5) && eth_cpu_ctrl_.port_en()) {
        int eth_ch = port - 2;
        if ( 0 == ((port_down_ctrl_.eth_cpu_port_en() >> eth_ch) & 1) ) {
          RMT_LOG_VERBOSE("PktGenReg: eth_ch %d not enabled\n",eth_ch);
          return;
        }
      }
    }
    port_down_internal( port );
  }

}

void PktGenReg::port_up(uint16_t port) {
  // Handle only if this port belongs to your pipe

  uint16_t pipe_id = Port::get_pipe_num( port );
  RMT_LOG_VERBOSE("PktGenReg: %d portup trigger pipe %d port %d\n",pipe_index(),pipe_id, Port::get_pipe_local_port_index( port ));
  if (pipe_index() != pipe_id) {
    return;
  }
  port = Port::get_pipe_local_port_index( port );
  RMT_ASSERT( port < kPortMax );

  {
    std::lock_guard<std::mutex> lock(port_down_mutex_);
    ports_currently_down_.clear_bit(port);
  }
}

Packet* PktGenReg::make_packet_from_buffer(int addr, int size, int channel) {
  Packet* e_packet = nullptr;

  static_assert( kFcsLen>=0 , "kFcsLen can not be negative" );
  RMT_ASSERT( size >= GLOBAL_ZERO );
  if ((size + kFcsLen) > 0) {
    // packet length is padded by fcslen as a CRC must be added, doesn't matter
    //  what the value of the CRC is as nothing checks it, but initialise to zero
    std::unique_ptr<uint8_t[]> buf( new uint8_t[size+kFcsLen]{} );
    pgen_buffer_.get_val(buf.get(), addr, size);
    e_packet = om_->pkt_create(buf.get(), size+kFcsLen);
  }
  else {
    e_packet = om_->pkt_create();
  }

  // Port is set to 0 + channel
  uint16_t port = pktgen_->get_port_num();
  uint16_t i_port = ( pipe_index() << 7) | ((port << 2) | channel);
  e_packet->set_port(om_->port_get(i_port));
  e_packet->set_ingress();
  e_packet->set_generated(true);

  return e_packet;
}

void PktGenReg::get_phase0_metadata(const int index,const int size, uint8_t *phase0_metadata) {
  // This func unused on WIP
  pgen_phase0_meta_.get_val(phase0_metadata,index,size);
}

void PktGenReg::recirc_snoop(Packet* packet, int channel) {

  BitVector < kMatchWidth > val {};
  get_snoop_val(packet,&val);

  for (unsigned int i = 0; i < pgen_app_.size(); i ++) {
    if (pgen_app_[i]->snoop( val, true, channel ) ) {
      PktGenEventData event_data{};
      event_data.key.copy_from(val);
      pgen_app_[i]->do_event(event_data);
    }
  }
}

void PktGenReg::deparser_snoop(PacketBuffer* pb,int addr, int len) {

  RMT_ASSERT(pb);
  BitVector < kMatchWidth > val {};
  get_snoop_val(pb,&val);

  for (unsigned int i = 0; i < pgen_app_.size(); i ++) {
    if (pgen_app_[i]->snoop( val, false, -1 ) ) {
      PktGenEventData event_data{};
      event_data.addr    = addr;
      event_data.length  = len;
      event_data.key.copy_from(val);
      pgen_app_[i]->do_event(event_data);
    }
  }
}



void PktGenReg::start() {
  curr_seq_ch_ = 0;
  done_ = false;

  std::unique_lock<std::mutex> lock(reg_mutex);
  if (main_thread_ == nullptr) {
    main_thread_ = new std::thread(&PktGenReg::main_proc, this);
    RMT_LOG_VERBOSE("PktGenReg: MAIN_THREAD:START\n");
  }
  for (unsigned int i = 0; i < pgen_app_.size(); i++) {
    pgen_app_[i]->start_or_stop();
  }
}
void PktGenReg::stop(void) {

  std::unique_lock<std::mutex> lock(reg_mutex);
  for (unsigned int i = 0; i < pgen_app_.size(); i ++) {
    pgen_app_[i]->stop();
  }

  done_ = true;

  if (main_thread_ != nullptr) {
    if (main_thread_->joinable()) {
      main_thread_->join();
      RMT_LOG_VERBOSE("PktGenReg: MAIN_THREAD:STOP\n");
    }
    delete(main_thread_);
    main_thread_ = nullptr;
  }
}

void PktGenReg::start_or_stop() {
  if (!initialized_) return;

  bool s=false;

  // work out if any channel is enabled for pkt gen or recirc
  for (int ch=0;ch<8;++ch) {
    if ( channel_enabled_for_packet_gen(ch) ) {
      s=true;
    }
    else {
      int eth_ch = ch-2;
      if (( ch == 0 ) && tbc_port_ctrl_.port_en()) {
        // not pkt gen / recirc
      }
      else if ((eth_ch>=0) && (eth_ch<=3) && eth_cpu_ctrl_.port_en() &&
               (( eth_cpu_ctrl_.channel_en() >> eth_ch ) & 1 )) {
        // not pkt gen / recirc
      }
      else {
        int ebuf = ch/2;
        int e_ch = ch%2;
        if ( ebuf_port_ctrl_.port_en( ebuf ) &&
             ((ebuf_port_ctrl_.channel_en( ebuf ) >> e_ch) & 1 )) {
          s = true;
          break;
        }
      }
    }
  }

  if (s) {
    start();
  }
  else {
    stop();
  }
}


/*
 * Main process, just alternates between
 * recirc packet & pktgen packet
 * Packet gen has priority
 */
void PktGenReg::main_proc() {
  Packet* g_packet = nullptr;
  Packet* r_packet = nullptr;
  while (!done_) {
    done_ = done_ || GLOBAL_FALSE;  // XXX: Use GLOBAL_FALSE to keep klocwork happy
    const int sleep_time = kSleepTime;
    std::this_thread::sleep_for(std::chrono::microseconds(sleep_time));
    uint8_t curr_ch = curr_seq_ch_;
    // increment the channel (mimics old more complex code)
    curr_seq_ch_ = ((curr_seq_ch_ + 1)%RmtDefs::kChlsPerIpb);

    // Note: no channel enables are checked here, they are checked before the
    //  packets are queued
    if(gen_q_[curr_ch].dequeue(g_packet)) {
      RMT_LOG_VERBOSE("PktGenReg: DQ:G\n");
      packet_enqueuer_->enqueue(g_packet->port()->port_index(), g_packet, true);
      pktgen_->g_pgen_cnt_++;
      g_packet = nullptr;
    }
    else if (recir_q_[curr_ch].dequeue(r_packet)) {
      RMT_LOG_VERBOSE("PktGenReg: DQ:R\n");
      packet_enqueuer_->enqueue(r_packet->port()->port_index(), r_packet, false);
      r_packet = nullptr;
    }
  }
}


bool PktGenReg::enqueue_generated_packet( int ch, Packet* p ) {
  if ( gen_q_[ch].enqueue( p ) ) {
    return true;
  }
  else {
    pktgen_->g_drop_cnt_++;
    return false;
  }
}

void PktGenReg::update_port_down_registers_for_event(int port, int which_mask) {
    int nb = map_port_to_vector_bit(port);
    port_down_vector_[which_mask].clear_bit(nb);
    // HW writes 1
    RMT_LOG_VERBOSE("PktGenReg: %d portdown port %d setting disable\n",pipe_index(),port);
    port_down_dis_.set(nb, 0x1);
    port_down_dis_before_write_.set_bit( nb ); // keep a copy to spot changes
}


int PktGenReg::get_next_port_down_unsent(int port, int which_mask) {
  for (int i=0;i<kPortMax;++i) {
    int next_port = (port + i) % kPortMax;
    int nb = map_port_to_vector_bit(next_port);

    if ( port_down_vector_[which_mask].bit_set(nb) && ! port_down_sent_.bit_set(nb) ) {
      return next_port;
    }
  }
  return -1;
}

void PktGenReg::port_down_set_sent(int port) {
  // TODO: can't lock here because do_event is called with the lock held above
  //  and ends up calling this
  //std::lock_guard<std::mutex> lock(port_down_mutex_);
  int mb = map_port_to_vector_bit(port);
  RMT_LOG_VERBOSE("PktGenReg: setting port_down_sent bit %d\n",mb);
  port_down_sent_.set_bit(mb);
}


// Callback is called after bits have been cleared in the mask (it is a write 1 to clear register)
//  so we need to track the state before the write so we can spot what changed
void PktGenReg::port_down_dis_callback() {
  RMT_LOG_VERBOSE("PktGenReg:port_down_dis_callback\n");
  std::lock_guard<std::mutex> lock(port_down_mutex_);

  for (unsigned int i = 0; i < kPortMax; i ++) {
    bool reg_bit = port_down_dis_.set(i);
    bool before_write = port_down_dis_before_write_.bit_set(i);

    if ( reg_bit != before_write ) {
      RMT_ASSERT( before_write ); // write can only change 1 to 0
      RMT_LOG_VERBOSE("PktGenReg: clearing port_down_triggered bit %d\n",i);
      port_down_dis_before_write_.clear_bit(i);
      port_down_sent_.clear_bit(i);
    }
  }
}

void PktGenReg::retrigger_callback() {
  RMT_LOG_VERBOSE("PktGenReg:retrigger_callback en=%d all_down_port=%d\n",retrigger_port_down_.en(),retrigger_port_down_.all_down_port());
  // spot the rising edge on en
  if (retrigger_port_down_.en() && !last_retrigger_en_) {

    std::lock_guard<std::mutex> lock(port_down_mutex_);

    // clear all the disable bits
    for (unsigned int i = 0; i < kPortMax; i ++) {
        port_down_dis_.set(i,0);         // clear the disable bit (field is called "set"!)
        port_down_dis_before_write_.clear_bit(i);
    }
    if ( retrigger_port_down_.all_down_port() ) {
      // in this case clear the sent bits so everything gets resent
      RMT_LOG_VERBOSE("PktGenReg: clearing port_down_sent\n");
      for (unsigned int i = 0; i < kPortMax; i ++) {
        port_down_sent_.clear_bit(i);
      }
    }
    // now trigger all ports currently down
    for (unsigned int i = 0; i < kPortMax; i ++) {
      if ( ports_currently_down_.bit_set( i ) ) {
        RMT_LOG_VERBOSE("PktGenReg:  setting triggered port %d\n",i);
        port_down_internal( i );
      }
    }
  }
  last_retrigger_en_ = retrigger_port_down_.en();
}

void PktGenReg::port_down_vec_clr_callback() {
  RMT_LOG_VERBOSE("PktGenReg:port_down_vec_clr_callback pipe=%d, en=%d\n",
                  pipe_index(),port_down_vec_clr_.en());

  bool positive_edge = port_down_vec_clr_.en() && !last_port_down_vec_clr_en_;

  if (positive_edge) {
    RMT_LOG_VERBOSE("PktGenReg:port_down_vec_clr_callback clearing port_down_vector\n");
    port_down_vector_[0].fill_all_zeros();
    port_down_vector_[1].fill_all_zeros();
    if ( port_down_vec_clr_.set_sent() ) {
      RMT_LOG_VERBOSE("PktGenReg:port_down_vec_clr_callback setting all sent\n");
      port_down_sent_.fill_all_ones();
    }
  }
  last_port_down_vec_clr_en_ = port_down_vec_clr_.en();
}


} // namespace
