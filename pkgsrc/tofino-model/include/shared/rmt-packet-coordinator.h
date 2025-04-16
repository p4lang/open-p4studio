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

#ifndef _SHARED_RMT_PACKET_COORDINATOR_
#define _SHARED_RMT_PACKET_COORDINATOR_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <common/rmt.h>
#include <rmt-defs.h>
#include <rmt-object-manager.h>
#include <packet.h>
#include <packet-enqueuer.h>
#include <queueing.h>
#include <rmt-log.h>

namespace MODEL_CHIP_NAMESPACE {

// Defines signature for alternative pipe process function that may be
// installed for testing.
typedef std::function<void(Packet *, Packet *, Packet **, Packet **,
                       Packet **,
                       Packet **,
                       Packet **)> pipe_process_fn_t;


class PerPipeQueues {
  static constexpr int kMaxPktGenPktsPending = 25;

 public:
  PerPipeQueues(RmtObjectManager *om, int pipe) :
      om_(om), mutex_(), rx_queue_(), pgen_queue_(),
      n_in_tm_(0u), pipe_(pipe),
      pipe_active_(false), pgen_next_(false), tm_next_(false) {
  }
  ~PerPipeQueues() { }


  void snooze(std::unique_lock<std::mutex> *lock, int n_millis) {
    lock->unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(n_millis));
    lock->lock();
  }
  void enqueue_from_port(Packet *pkt) {
    std::unique_lock<std::mutex> lock(mutex_);
    rx_queue_.push_back(pkt);
  }
  void enqueue_from_resubmit(Packet *pkt) {
    std::unique_lock<std::mutex> lock(mutex_);
    rx_queue_.push_front(pkt);
  }
  void enqueue_from_pgen(Packet *pkt) {
    std::unique_lock<std::mutex> lock(mutex_);
    while (pgen_queue_.size() >= kMaxPktGenPktsPending) snooze(&lock, 1);
    pgen_queue_.push(pkt);
  }
  void enqueue_from_tm(Packet *pkt) {
    // Packet *not* queued but count kept
    std::unique_lock<std::mutex> lock(mutex_);
    n_in_tm_++;
  }
  void deactivate() {
    // Mark pipe inactive when done so another thread can get in
    std::unique_lock<std::mutex> lock(mutex_);
    RMT_ASSERT(pipe_active_);
    pipe_active_ = false;
  }
  bool dequeue(Packet **in_pkt, Packet **out_pkt);

 private:
  Packet *get_from_port();
  Packet *get_from_pgen();
  Packet *get_input_pkt();
  Packet *get_output_pkt();

  RmtObjectManager     *om_;
  std::mutex            mutex_;
  std::list<Packet*>    rx_queue_;
  std::queue<Packet*>   pgen_queue_;
  uint32_t              n_in_tm_;
  int                   pipe_;
  bool                  pipe_active_;
  bool                  pgen_next_;
  bool                  tm_next_;
}; // PerPipeQueues



class RmtPacketCoordinator : private RmtObject, public DefaultLogger, public PacketEnqueuer {
  static constexpr int kPipes = RmtDefs::kPipesMax;

 public:
  static bool kProcessGressesSeparately; // Defined in rmt-config.cpp

  RmtPacketCoordinator(RmtObjectManager *om);
  ~RmtPacketCoordinator();

  void set_num_threads(int n);
  void start();
  void stop();
  void resume();
  bool is_running() { return run_ && GLOBAL_TRUE; }
  void stop_running();
  void packet_arrived();
  void packets_processed(int n);
  bool is_idle();
  // Return 0 when it already was idle, 1 when needed to wait to be idle.
  int wait_for_idle();
  bool wait_for_packets(bool waiting, int thread_index);

  // from PacketEnqueuer used by PacketGen etc.
  void enqueue(uint16_t port_num, Packet*& pkt, bool pgen);
  // Used in Chip::PacketReceivePostBuf
  void enqueue(uint16_t port_num, uint8_t *buffer, int length);
  // Used from within packet_processing
  void enqueue_resubmit(Packet *pkt);
  // Called by TM
  void enqueue_from_tm(Packet *pkt, int pipe);
  void pkt_freed(Packet *pkt);

  void set_tx_fn(RmtPacketCoordinatorTxFn txfn)  { tx_function_ = txfn; }
  RmtPacketCoordinatorTxFn get_tx_fn()           { return tx_function_; }

  // install an alternative pipe process function for testing
  void set_pipe_process_fn(pipe_process_fn_t fn) { pipe_process_fn_= fn; }

 private:
  DISALLOW_COPY_AND_ASSIGN(RmtPacketCoordinator);

  void transmit(Packet *pkt);
  void s2p_process(Packet *pkt, bool mirrored);
  void p2s_process(Packet *pkt);
  void packet_process(int thread_index);
  void do_pipe_processing(int pipe_index, Packet *from_port, Packet *from_tm);

  int                          num_pipes_;   // Pipes avaiable
  int                          num_threads_; // Threads to use
  // Vector of PerPipeQueues
  std::vector<PerPipeQueues*>  pipe_queues_;
  // Threads to process packets
  std::vector<std::thread>     packet_processors_;
  // Flag to stop all running packet processing threads.
  bool                         run_ ;
  bool                         waiting_till_idle_;
  int                          num_threads_waiting_;
  uint32_t                     num_packets_pending_;

  std::mutex                   mutex_;
  std::condition_variable      thread_cv_;
  std::condition_variable      waiting_till_idle_cv_;

  // Function pointer to a transmit function, called when a packet is "sent out" of a port.
  RmtPacketCoordinatorTxFn     tx_function_;
  pipe_process_fn_t            pipe_process_fn_;
};

}

#endif
