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

#include <rmt-object-manager.h>
#include <model_core/model.h>
#include <rmt-packet-coordinator.h>
#include <port.h>
#include <pktgen.h>
#include <packet-gen-metadata.h>
#include <rmt-types.h>
#include <event.h>
#include <other-pipe-objects.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;
extern bool g_use_pcie_veth;

namespace MODEL_CHIP_NAMESPACE {

static uint32_t g_pkt_id = 0;

thread_local rmt_thd_info_t rmt_thd_info;   // thread specific context used by logging


// These next 4 functions private and *UNLOCKED* - only called from dequeue()
Packet *PerPipeQueues::get_from_port() {
  if (rx_queue_.empty()) return nullptr;
  Packet *p = rx_queue_.front();
  rx_queue_.pop_front();
  return p;
}
Packet *PerPipeQueues::get_from_pgen() {
  if (pgen_queue_.empty()) return nullptr;
  Packet *p = pgen_queue_.front();
  pgen_queue_.pop();
  return p;
}
Packet *PerPipeQueues::get_input_pkt() {
  Packet *p = nullptr;
  if (pgen_next_) {
    p = get_from_pgen();
    if (p == nullptr) p = get_from_port();
  } else {
    p = get_from_port();
    if (p == nullptr) p = get_from_pgen();
  }
  pgen_next_ = !pgen_next_;
  return p;
}
Packet *PerPipeQueues::get_output_pkt() {
  Queueing *qing = om_->queueing_get();
  RMT_ASSERT_NOT_NULL(qing);
  Packet *p = nullptr;
  qing->dequeue(pipe_, p);
  if (p != nullptr) RMT_ASSERT(n_in_tm_ > 0 && "TM packet missing");
  if (p != nullptr) n_in_tm_--;
  return p;
}

bool PerPipeQueues::dequeue(Packet **in_pkt, Packet **out_pkt) {
  RMT_ASSERT((in_pkt != nullptr) && (out_pkt != nullptr));

  std::unique_lock<std::mutex> lock(mutex_);
  // Bail if some other thread already using this pipe
  if (pipe_active_) return false;

  Packet *p_in = get_input_pkt();
  Packet *p_out = get_output_pkt();
  // Bail if nothing to do...
  if ((p_in == nullptr) && (p_out == nullptr)) return false;

  pipe_active_ = true; // ... else mark pipe active
  *in_pkt = p_in;      // and pass out packet(s)
  *out_pkt = p_out;
  return true;
}




RmtPacketCoordinator::RmtPacketCoordinator(RmtObjectManager *om)
    : RmtObject(om), DefaultLogger(om,0),
      num_pipes_(om->num_pipes()), num_threads_(1),
      run_(false), waiting_till_idle_(false), num_threads_waiting_(0),
      num_packets_pending_(0u), tx_function_(nullptr) {
  for (int i = 0; i < num_pipes_; i++) {
    pipe_queues_.push_back(new PerPipeQueues(om, i));
  }
}
RmtPacketCoordinator::~RmtPacketCoordinator() {
  stop();
  for (int i = 0; i < num_pipes_; i++) {
    PerPipeQueues *ppq = pipe_queues_.back();
    pipe_queues_.pop_back();
    delete ppq;
  }
}


void RmtPacketCoordinator::set_num_threads(int n_threads) {
  RMT_ASSERT(n_threads > 0);
  num_threads_ = n_threads;
}

void RmtPacketCoordinator::start() {
  // If not running, start the packet processing
  if (!is_running()) {
    run_ = true;
    for (int i = 0; i < num_threads_; ++i) {
      packet_processors_.push_back(std::thread(&RmtPacketCoordinator::packet_process, this, i));
    }
    printf("Created %d packet processing threads\n", num_threads_);
  }
}

void RmtPacketCoordinator::stop() {
  stop_running();
  // Repeat calls to stop running whilst threads waiting
  int n = 1;
  while (n > 0) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      n = num_threads_waiting_;
    }
    if (n > 0) {
      stop_running();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
  for (auto& thread : packet_processors_) {
    if (thread.joinable())
      thread.join();
  }
}

void RmtPacketCoordinator::resume() {
  RMT_P4_LOG_INFO("RmtPacketCoordinator::resume called unexpectedly !!!!!!!!!!!!!!!!!!!!!\n");
  if (!is_running()) return; // If not running do nothing
  // Wake up and look for packets
  std::unique_lock<std::mutex> lock(mutex_);
  thread_cv_.notify_all();
}


void RmtPacketCoordinator::stop_running() {
  std::unique_lock<std::mutex> lock(mutex_);
  run_ = false;
  if (waiting_till_idle_) waiting_till_idle_cv_.notify_all();
  thread_cv_.notify_all();
}
void RmtPacketCoordinator::packet_arrived() {
  std::unique_lock<std::mutex> lock(mutex_);
  num_packets_pending_++;
  // TODO: maybe suppress notify is packet's pipe already active
  if (num_threads_waiting_ > 0) thread_cv_.notify_one();
}
void RmtPacketCoordinator::packets_processed(int n) {
  std::unique_lock<std::mutex> lock(mutex_);
  RMT_ASSERT(n > 0);
  // XXX: disable this for the moment
  // RMT_ASSERT(num_packets_pending_ >= (unsigned)n);
  num_packets_pending_ -= n;
}
bool RmtPacketCoordinator::is_idle() {
  RmtObjectManager *om = get_object_manager();
  RMT_ASSERT_NOT_NULL(om);
  return ((num_threads_waiting_ == num_threads_) && (num_packets_pending_ == 0u) && om->pre_fifos_empty());
}
int RmtPacketCoordinator::wait_for_idle() {
  std::unique_lock<std::mutex> lock(mutex_);
  int ret = 0;
  if (!is_running()) return ret; // Stopping - bail
  int cnt = 0;
  while (!is_idle()) {
    ret = 1;
    if (cnt++ >= 999999) break; // for Klocwork
    if (!is_running()) break;
    waiting_till_idle_ = true;
    waiting_till_idle_cv_.wait(lock);
    waiting_till_idle_ = false;
  }
  RMT_ASSERT((cnt < 999999) && "Too long waiting for idle");
  return ret;
}
bool RmtPacketCoordinator::wait_for_packets(bool waiting, int thread_index) {
  bool was_waiting = waiting;
  {
    std::unique_lock<std::mutex> lock(mutex_);
    num_threads_waiting_++;

    if (thread_index == 0) {
      // Thread 0 only waits max 10s, then returns to check for new packets

      // Thread 0 might notify threads that are waiting for model to become idle
      if (waiting_till_idle_ && is_idle()) waiting_till_idle_cv_.notify_all();

      waiting = (is_running() && (num_packets_pending_ == 0u));
      if (was_waiting && waiting) {
        // If was previously waiting and should still be waiting, wait max 10s more
        (void)thread_cv_.wait_for(lock,std::chrono::seconds(10));
        // Then re-evaluate whether we should continue waiting
        waiting = (is_running() && (num_packets_pending_ == 0u));
      }
    } else {
      // Other threads wait till signalled
      thread_cv_.wait(lock);
      waiting = (is_running() && (num_packets_pending_ == 0u));
      was_waiting = waiting; // To prevent log below
    }

    num_threads_waiting_--;
  }
  // XXX: Only say stuff if non_waiting->waiting or waiting->non_waiting
  if (!was_waiting && waiting) {
    RMT_P4_LOG_INFO("Waiting for packets to process\n");
  }
  if (was_waiting && !waiting) {
    RMT_P4_LOG_INFO("Begin packet processing\n");
  }
  return waiting;
}


void RmtPacketCoordinator::enqueue(uint16_t port_num, uint8_t *buffer, int length) {
  RMT_ASSERT((port_num < RmtDefs::kPortsTotal) && "Bad port_num");
  RMT_ASSERT((buffer != nullptr) && "Null buffer passed to enqueue");
  RMT_ASSERT((length > 0) && "Zero length passed to enqueue");
  RmtObjectManager *om = get_object_manager();
  Port *port = om->port_get(port_num);
  Packet *pkt = om->pkt_create(buffer, length);
  int pipe = Port::get_pipe_num(port_num);
  RMT_ASSERT_NOT_NULL(om);
  RMT_ASSERT_NOT_NULL(port);
  RMT_ASSERT_NOT_NULL(pkt);
  RMT_ASSERT(((pipe >= 0) && (pipe < num_pipes_)) && "Bad port pipe number");
  pkt->set_port(port);
  pipe_queues_.at(pipe)->enqueue_from_port(pkt);
  packet_arrived(); // Signals waiting thread
}
void RmtPacketCoordinator::enqueue(uint16_t port_num, Packet*& pkt, bool pgen) {
  RMT_ASSERT((port_num < RmtDefs::kPortsTotal) && "Bad port_num");
  RmtObjectManager *om = get_object_manager();
  Port *port = om->port_get(port_num);
  int pipe = Port::get_pipe_num(port_num);
  RMT_ASSERT_NOT_NULL(om);
  RMT_ASSERT_NOT_NULL(port);
  RMT_ASSERT_NOT_NULL(pkt);
  RMT_ASSERT(((pipe >= 0) && (pipe < num_pipes_)) && "Bad port pipe number");
  pkt->set_port(port);
  if (pgen) {
    pipe_queues_.at(pipe)->enqueue_from_pgen(pkt);
  } else {
    pipe_queues_.at(pipe)->enqueue_from_port(pkt);
  }
  packet_arrived();
}
void RmtPacketCoordinator::enqueue_resubmit(Packet *pkt) {
  RMT_ASSERT_NOT_NULL(pkt);
  Port *port = pkt->port();
  RMT_ASSERT_NOT_NULL(port);
  int pipe = Port::get_pipe_num(port->port_index());
  RMT_ASSERT(((pipe >= 0) && (pipe < num_pipes_)) && "Bad port pipe number");
  pipe_queues_.at(pipe)->enqueue_from_resubmit(pkt);
  packet_arrived();
}
void RmtPacketCoordinator::enqueue_from_tm(Packet *pkt, int pipe) {
  RMT_ASSERT_NOT_NULL(pkt);
  // XXX: use passed in pipe; port object not set till later in p2s_process
  RMT_ASSERT(((pipe >= 0) && (pipe < num_pipes_)) && "Bad port pipe number");
  pipe_queues_.at(pipe)->enqueue_from_tm(pkt);
  packet_arrived();
}

void RmtPacketCoordinator::pkt_freed(Packet *pkt) {
  if (rmt_thd_info.ig_packet == pkt) rmt_thd_info.ig_packet = nullptr;
  if (rmt_thd_info.eg_packet == pkt) rmt_thd_info.eg_packet = nullptr;
}




void RmtPacketCoordinator::transmit(Packet *pkt) {
  uint16_t len = pkt->len();
  int port_num = pkt->qing2e_metadata()->egress_port();
  uint8_t *buf = new uint8_t[len];
  len = pkt->get_buf(buf, 0, len);

  uint32_t pipes_en = GLOBAL_MODEL->GetPipesEnabled(chip_index());
  if ((!g_use_pcie_veth) && RmtDefs::is_pcie_port(port_num, pipes_en)) {
    int cos = buf[0] & 7;
    int fcs_len = 4;
    // dru_rx_pkt_callback is MT-safe
    GLOBAL_MODEL->dru_rx_pkt_callback(chip_index(), buf, len - fcs_len, cos);
  } else if (tx_function_ != nullptr) {
    // XXX: WIP: map internal port view (72) to external port view (36)
    int external_port = Port::port_map_outbound(port_num);
    // tx_function must also be MT-safe
    tx_function_(chip_index(), external_port, buf, len);
  }
  delete [] buf;
}

void RmtPacketCoordinator::s2p_process(Packet *pkt, bool mirrored) {
  RMT_ASSERT_NOT_NULL(pkt);
  RmtObjectManager *om = get_object_manager();
  RMT_ASSERT_NOT_NULL(om);
  int pipe_index = pkt->port()->pipe_index();
  S2p *s2p = om->s2p_get(pipe_index);
  if (mirrored && is_jbayXX()) {
    // Mirrored packets *not* counted on JBay
  } else {
    int port_index = pkt->port()->port_index();
    s2p->increment_pkt_ctr(port_index);
    s2p->increment_byte_ctr(port_index, pkt->len());
  }
  if (is_jbay_or_later()) {
    // XXX: Map logical pipes to physical pipes (JBay only)
    // XXX: map pipe view of ports (72) to TM view (36) (WIP only)
    s2p->map_logical_to_physical(pkt, mirrored);
  }
}

void RmtPacketCoordinator::p2s_process(Packet *pkt) {
  RMT_ASSERT_NOT_NULL(pkt);
  RmtObjectManager *om = get_object_manager();
  RMT_ASSERT_NOT_NULL(om);
  if (is_chip1_or_later()) {
    // XXX: WIP: map TM view of ports (36) to pipe view (72)
    int tm_port_num = pkt->qing2e_metadata()->egress_port();
    int pipe_port_num = Port::port_map_tm_to_pipe(tm_port_num);
    pkt->qing2e_metadata()->set_egress_port(pipe_port_num);
  }
  // XXX: now find Port object
  int port_num = pkt->qing2e_metadata()->egress_port();
  Port *p = om->port_lookup(Port::get_die_local_port_index(port_num));
  RMT_ASSERT((p != nullptr) && "No Port object for egress port number");
  pkt->set_egress();
  pkt->set_port(p);
}

void RmtPacketCoordinator::packet_process(int thread_index) {
  std::array<uint64_t,kPipes> pkts_in = { UINT64_C(0) };
  std::array<uint64_t,kPipes> pkts_out= { UINT64_C(0) };
  uint64_t waits = UINT64_C(0);
  bool waiting = false;
  int last_pipe_processed = thread_index % num_pipes_;

  rmt_thd_info.ig_packet = nullptr;
  rmt_thd_info.eg_packet = nullptr;
  rmt_thd_info.pipe = -1;

  while (is_running()) {

    // Loop through pipes till we find a pipe that:
    // a) has got packets to process
    // b) is not already busy

    int n_pipes_processed = 0;
    int base_pipe = last_pipe_processed;

    for (int i = 1; i <= num_pipes_; i++) {
      // Will loop base+1,base+2,...<wrap>...,base
      int try_pipe = (base_pipe + i) % num_pipes_;

      Packet *from_port = nullptr, *from_tm = nullptr;

      // dequeue func below will only return true if one or more
      // packets available and pipe is *not* already active.
      // Whilst holding PerPipeQueue lock it marks the pipe as active.
      if (is_running() &&
          pipe_queues_.at(try_pipe)->dequeue(&from_port, &from_tm)) {

        int n_pkts = 0;
        if (from_port != nullptr) {
          ++n_pkts;
          ++pkts_in[try_pipe];
        }
        if (from_tm != nullptr) {
          ++n_pkts;
          ++pkts_out[try_pipe];
        }
        RMT_ASSERT(n_pkts > 0);

        if (waiting && (thread_index == 0)) {
          waiting = false;
          RMT_P4_LOG_INFO("Begin packet processing\n");
        }

        // Call main processing function
        if (kProcessGressesSeparately) {
          if (from_port != nullptr)
            do_pipe_processing(try_pipe, from_port, nullptr);
          if (from_tm != nullptr)
            do_pipe_processing(try_pipe, nullptr, from_tm);
        } else {
          do_pipe_processing(try_pipe, from_port, from_tm);
        }

        // Can now mark the pipe as inactive
        pipe_queues_.at(try_pipe)->deactivate();

        // And decrement number pending packets
        packets_processed(n_pkts);

        n_pipes_processed++;
        last_pipe_processed = try_pipe;
      }
    }

    // If we looped across *all* pipes but found nothing to do
    // then wait for packets to arrive. Note wait_for_packets
    // will only block if number packets arrived is unchanged
    if (n_pipes_processed == 0) {
      if (thread_index == 0) {
        RmtObjectManager *om = get_object_manager();
        RMT_ASSERT_NOT_NULL(om);
        om->flush_event_log();
      }
      waiting = wait_for_packets(waiting, thread_index);
      if (waiting) ++waits;
    }

  } // while (is_running())


  // Thread exiting - take lock so we can dump stats without interleaving on this chip
  std::unique_lock<std::mutex> lock(mutex_);

  uint64_t pkts_in_all = UINT64_C(0), pkts_out_all = UINT64_C(0);
  for (int i = 0; i < kPipes; i++) {
    if (pkts_in[i] + pkts_out[i] > UINT64_C(0)) { // Only print detail if >0
      RMT_P4_LOG_INFO("Chip=%d Thread=%d Pipe=%d: PktsIn=%" PRId64 " PktsOut=%" PRId64 "\n",
                      chip_index(), thread_index, i, pkts_in[i], pkts_out[i]);
    }
    pkts_in_all += pkts_in[i];
    pkts_out_all += pkts_out[i];
  }
  RMT_P4_LOG_INFO("Chip=%d Thread=%d Pipe=*: PktsIn=%" PRId64 " PktsOut=%" PRId64 " "
                  "PktsTOT=%" PRId64 "  Waits=%" PRId64 "\n",
                  chip_index(), thread_index, pkts_in_all, pkts_out_all,
                  pkts_in_all+pkts_out_all, waits);
}


void RmtPacketCoordinator::do_pipe_processing(int pipe_index, Packet *from_port, Packet *from_tm) {
  RmtObjectManager *om = get_object_manager();
  RMT_ASSERT_NOT_NULL(om);
  Queueing *qing = om->queueing_get();
  RMT_ASSERT_NOT_NULL(qing);
  RMT_ASSERT(((from_port != nullptr) || (from_tm != nullptr)) && "Port & TM packets both null");

  bool bypass = (from_tm != nullptr) ?from_tm->i2qing_metadata()->bypass_egr_mode() :false;
  rmt_thd_info.ig_packet = from_port;
  rmt_thd_info.eg_packet = (bypass) ?nullptr :from_tm;
  rmt_thd_info.pipe = pipe_index;
  Pipe *pipe = nullptr;

  if (from_port != nullptr) {
    RMT_ASSERT((from_port->port()->pipe_index() == pipe_index) && "InPort pkt pipe mismatch");
    pipe = from_port->port()->pipe();

    // Set the packet's version now that it is being "received" by the model.
    from_port->set_version(from_port->port()->version());
    from_port->pkt_id(++g_pkt_id);
    from_port->pkt_type(Packet::PKT_TYPE_NORM);
    // extract signature
    from_port->extract_signature();

    // Increment RX MAC counters
    Mac *in_mac = from_port->port()->mac();
    if (in_mac != nullptr)
      in_mac->increment_rx_counters(from_port->port()->mac_chan(), from_port, 0u);

    RMT_P4_LOG_INFO("========== Ingress Pkt from port %d (%d bytes)  ==========\n",
                    from_port->port()->port_index(), from_port->len());
    rmt_log_packet(from_port);
    om->log_info(std::string("Ingress packet id: ") + std::to_string(from_port->pkt_id()));
    om->log_packet(RmtTypes::kRmtTypePort, *from_port);
  }
  if (from_tm != nullptr) {
    // XXX: Call p2s_process to do any required pipe/port mapping and find egress Port object
    p2s_process(from_tm);

    RMT_ASSERT((from_tm->port()->pipe_index() == pipe_index) && "TM pkt pipe mismatch");
    RMT_ASSERT(((pipe == nullptr) || (pipe == from_tm->port()->pipe())) && "InPort TM pipe mismatch");
    pipe = from_tm->port()->pipe();

    P2s *p2s = om->p2s_get(from_tm->port()->pipe_index());
    int port_index = from_tm->port()->port_index();
    p2s->increment_pkt_ctr(port_index);
    p2s->increment_byte_ctr(port_index, from_tm->len());
    from_tm->set_metadata_added(false);

    RMT_P4_LOG_INFO("========== Egress Pkt from TM to port %d (tm %d, real %d bytes) ==========\n",
                    from_tm->port()->port_index(),
                    from_tm->qing2e_metadata()->len(), from_tm->len());
    rmt_log_packet(from_tm);
    om->log_packet(RmtTypes::kRmtTypeQueueing, *from_tm);
  }


  // Process the pair.
  Packet *ing_pipe_out = nullptr;
  Packet *ing_mirror_out = nullptr;
  Packet *egr_pipe_out = nullptr;
  Packet *egr_mirror_out = nullptr;
  Packet *resubmit_pkt = nullptr;
  PacketGenMetadata *packet_gen_metadata = nullptr; // not used on Tofino

  if (nullptr != pipe_process_fn_) {
    // for testing - call alternative pipe process function
    pipe_process_fn_(from_port, from_tm, &ing_pipe_out,
                     &egr_pipe_out, &resubmit_pkt,
                     &ing_mirror_out, &egr_mirror_out);
  } else {
    pipe->process(from_port, !bypass ? from_tm : nullptr, &ing_pipe_out,
                  &egr_pipe_out, &resubmit_pkt,
                  &ing_mirror_out, &egr_mirror_out, &packet_gen_metadata);
    if (nullptr != resubmit_pkt) {
      enqueue_resubmit(resubmit_pkt);
    }
    if (bypass) {
      egr_pipe_out = from_tm;
    }
  }

  if (packet_gen_metadata) {
    // send metadata to packetgen for the JBay Deparser/MAU trigger type
    RMT_ASSERT((pipe->pipe_index() == pipe_index) && "Pipe mismatch 3");
    PktGen* pgen = om->pktgen_lookup(pipe_index);
    RMT_ASSERT( pgen && "Bad pktgen obj");
    pgen->maybe_trigger(packet_gen_metadata);
    delete packet_gen_metadata;
  }

  // For now, the ingress pipeline always feeds the queueing block.
  if (ing_pipe_out) {
    // Decide if the packet should be dropped.
    if (!ing_pipe_out->i2qing_metadata()->is_egress_uc() &&
        !ing_pipe_out->i2qing_metadata()->cpu_needs_copy() &&
        !ing_pipe_out->i2qing_metadata()->has_mgid1() &&
        !ing_pipe_out->i2qing_metadata()->has_mgid2()) {
      om->pkt_delete(ing_pipe_out);
    } else {
      //RMT_P4_LOG_INFO("========== Enqueue Pkt to TM from port %d ==========\n",
      //                   ing_pipe_out->port()->port_index());
      //rmt_log_packet(ing_pipe_out);

      // Map log->phys pipe (JBay only) and tick counters (JBay/WIP)
      s2p_process(ing_pipe_out, false /* false=>not mirrored */);
      qing->enqueue_die(ing_pipe_out);
    }
  }
  if (ing_mirror_out) {
    // Map log->phys pipe (JBay only) and tick counters (WIP only)
    s2p_process(ing_mirror_out, true /* true=>mirrored */);
    qing->enqueue_die(ing_mirror_out);
  }
  // For now, the egress pipeline always feeds the ports.
  if (egr_pipe_out) {
    Port *out_port = egr_pipe_out->port();
    RMT_P4_LOG_INFO("========== Tx Pkt to port %d %s(%d bytes) ==========\n",
                    out_port->port_index(),
                    bypass ? "(Bypass Egress) " : "",
                    egr_pipe_out->len());
    rmt_log_packet(egr_pipe_out);
    om->log_packet(RmtTypes::kRmtTypePort, *egr_pipe_out);
    RMT_ASSERT((out_port->pipe_index() == pipe_index) && "OutPort pkt pipe mismatch");
    int port_index = out_port->port_index();
    EgressBuf *egress_buf = om->egress_buf_get(pipe_index);
    egress_buf->increment_mac_xmt_pkt(port_index);
    if (bypass) {
      egress_buf->increment_warp_rcv_pkt(port_index);
      auto *epb_counters = get_object_manager()->epb_counters_lookup(
          pipe_index,
          out_port->epb_index(),
          out_port->epb_chan());
      if (nullptr != epb_counters) epb_counters->increment_egr_bypass_count();
    } else {
      egress_buf->increment_dprsr_rcv_pkt(port_index);
    }

    PktGen* pgen = om->pktgen_lookup(pipe_index);
    RMT_ASSERT( pgen && "Bad pktgen obj 2");
    bool recirculate = pgen->maybe_recirculate(&egr_pipe_out);
    if (recirculate) {
      // recirculation is always done in packet gen now for consistency, so the packet
      //  pointer should always be nulled out
      RMT_ASSERT((egr_pipe_out == nullptr) && "Non-null recirc pkt from pgen");
      RMT_P4_LOG_INFO("========== Tx Pkt to port %d recirculated ==========\n",
                      port_index);
    }
    else {
      if (egr_pipe_out) {
        // Increment TX MAC counters
        Mac *out_mac = egr_pipe_out->port()->mac();
        if (out_mac != nullptr)
          out_mac->increment_tx_counters(egr_pipe_out->port()->mac_chan(),
                                         egr_pipe_out, 0u);
        transmit(egr_pipe_out);
        om->pkt_delete(egr_pipe_out);
      }
    }
  }
  if (egr_mirror_out) {
    // Map log->phys pipe (JBay only) and tick counters (WIP only)
    s2p_process(egr_mirror_out, true /* true=>mirrored */);
    qing->enqueue_die(egr_mirror_out);
  }

  rmt_thd_info.ig_packet = nullptr;
  rmt_thd_info.eg_packet = nullptr;
  rmt_thd_info.pipe = -1;
}


}
