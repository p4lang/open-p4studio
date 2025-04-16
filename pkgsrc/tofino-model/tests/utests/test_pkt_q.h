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

#ifndef _TEST_PKT_Q_
#define _TEST_PKT_Q_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <rmt-defs.h>
#include <rmt-object-manager.h>
#include <port.h>
#include <ipb.h>
#include <packet.h>
#include <packet-enqueuer.h>


namespace MODEL_CHIP_TEST_NAMESPACE {

  // Wrap up Packet and associated info
  class PktInfo {
 public:
    PktInfo(MODEL_CHIP_NAMESPACE::Packet *pkt,
            uint16_t port, bool pgen, uint8_t src_pipe,
            uint16_t matches, uint64_t T=UINT64_C(0))
        : pkt_(pkt), num_(0u), port_(port), pgen_(pgen), src_pipe_(src_pipe),
          matches_(matches), T_(T) {

      if (T_ == UINT64_C(0)) model_timer::ModelTimerGetTime(T_);
      pkt_hash_ = pkt_->hash();
      pkt_len_ = pkt_->len();
      pkt_is_egress_ = pkt_->is_egress();
    }
    ~PktInfo() { pkt_ = NULL; }

    MODEL_CHIP_NAMESPACE::Packet *pkt()           const { return pkt_; }
    uint32_t                      pkt_hash()      const { return pkt_hash_; }
    uint16_t                      pkt_len()       const { return pkt_len_; }
    bool                          pkt_is_egress() const { return pkt_is_egress_; }
    uint32_t                      num()           const { return num_; }
    uint16_t                      port()          const { return port_; }
    bool                          pgen()          const { return pgen_; }
    uint8_t                       src_pipe()      const { return src_pipe_; }
    uint16_t                      matches()       const { return matches_; }
    uint64_t                      T()             const { return T_; }

    void set_num(uint32_t num) { num_ = num; }
    void set_pgen(bool pgen)   { pgen_ = pgen; }
    void print(const char *prefix) {
      int port_pipe = MODEL_CHIP_NAMESPACE::Port::get_pipe_num(port_);
      int port_local = MODEL_CHIP_NAMESPACE::Port::get_pipe_local_port_index(port_);
      printf("%s: %c Pkt %5d: T=%15" PRId64 " %s "
             "SrcPipe=%d Port=%2d[%d,%d] PktHash=0x%08x PktLen=%4d "
             "Matches[ ",
             prefix, pkt_is_egress_?'E':'I', num_, T_, pgen_?"PktGen":"Recirc",
             src_pipe_, port_, port_pipe, port_local, pkt_hash_, pkt_len_);
      for (int i = 0; i < 16; i++) {
        if (((matches() >> i) & 1) == 1) printf("%d ",i);
      }
      printf("]\n");
    }

 private:
    MODEL_CHIP_NAMESPACE::Packet *pkt_;
    uint32_t                      pkt_hash_;
    uint16_t                      pkt_len_;
    bool                          pkt_is_egress_;
    uint32_t                      num_;
    uint16_t                      port_;
    bool                          pgen_;
    uint8_t                       src_pipe_;
    uint16_t                      matches_;
    uint64_t                      T_;
  };


  // All pipes queue Packets to a SharedQ
  class SharedQ {
 public:
    SharedQ() : queue_(), num_(0u), cnt_(0u), debug_(false) { lock_.clear(); }
    ~SharedQ()                                              {                }

    uint32_t cnt()        const { return cnt_; }
    bool     debug()      const { return debug_; }
    void     set_debug(bool tf) { debug_ = tf; }

    // Locking prims
    void spinlock() {
      while (std::atomic_flag_test_and_set_explicit(&lock_,
                                                    std::memory_order_acquire))
        ; // spin until the lock is acquired
    }
    void spinunlock() {
      std::atomic_flag_clear_explicit(&lock_,
                                      std::memory_order_release);
    }
    void push_pktinfo(PktInfo *pi) {
      assert( pi != nullptr );
      spinlock();
      cnt_++;
      pi->set_num(num_++);
      queue_.push(pi);
      spinunlock();
      if (debug_) pi->print("  Q");
    }
    PktInfo* get_pktinfo() {
      PktInfo *pi = nullptr;
      spinlock();
      if (!queue_.empty()) {
        pi = queue_.front();
        queue_.pop();
      }
      spinunlock();
      if ((debug_) && (pi != nullptr)) pi->print("DeQ");
      return pi;
    }
    void free_pktinfo(PktInfo *pi) {
      if (pi == nullptr) return;
      spinlock();
      cnt_--;
      spinunlock();
      delete pi;
    }

 private:
    std::atomic_flag       lock_;
    std::queue<PktInfo*>   queue_;
    uint32_t               num_;
    uint32_t               cnt_;
    bool                   debug_;
  };



  // Per-pipe obj passed to PktGen CTOR
  // On enqueue() callback, batches up args, adds src_pipe
  //   and enqueues to sharedQ (from TestPktQ object)
  //
  class TestPerPipePktQ : public MODEL_CHIP_NAMESPACE::PacketEnqueuer {
 public:
    TestPerPipePktQ(SharedQ *shared_q, int pipe)
        : om_(nullptr), ipb_(nullptr), shared_q_(shared_q), src_pipe_(pipe) {
    }
    ~TestPerPipePktQ() { }

    void set_ipb(MODEL_CHIP_NAMESPACE::Ipb *ipb) { ipb_ = ipb; }

    void print_hdr(MODEL_CHIP_NAMESPACE::Packet*& pkt, int orig_len) {
      if (!shared_q_->debug()) return;
      const bool jbay = MODEL_CHIP_NAMESPACE::RmtObject::is_jbayXX();
      uint64_t meta[4] = { UINT64_C(0) };
      uint8_t  buf[32] = { 0 };
      int cur_len = pkt->len();
      int got_len = pkt->get_buf(buf, 0, 32); // Always 32B metadata hdrs
      // On JBay: Zeros(8B) Meta0(8B) Meta1(16B)
      // On WIP:             Meta0(8B) Meta1(24B)
      assert( got_len == 32 );
      model_common::Util::fill_val(&meta[0],   8, buf,32,  0);
      model_common::Util::fill_val(&meta[1],   8, buf,32,  8);
      model_common::Util::fill_val(&meta[2],   8, buf,32, 16);
      model_common::Util::fill_val(&meta[3],   8, buf,32, 24);
      uint64_t hdrT = UINT64_C(0);
      if (pkt->is_generated()) hdrT = meta[jbay?1:0] & UINT64_C(0xFFFFFFFFFFFF);
      printf("TestPktQ: PgenPktQ (Len %d->%d) (HdrT=%" PRId64 ") (Hdr[0..3]"
             " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 ")\n",
             orig_len, cur_len, hdrT, meta[0], meta[1], meta[2], meta[3]);
    }
    void advance_ipb_time(uint64_t T) {
      if (om_ == nullptr) return;
      // Advance sweep time (used by IPB) to be same as model time
      // (this will be ahead of the time when PktGen decided to Q)
      uint64_t T_ipb = om_->time_get_cycles();
      assert( T >= T_ipb );
      if (T > T_ipb) om_->time_increment_cycles(T - T_ipb);
    }
    void add_ipb_header(MODEL_CHIP_NAMESPACE::Packet*& pkt, uint64_t T) {
      if (ipb_ == nullptr) return;
      // We now stash model time inside Packet so no need for this call
      //advance_ipb_time(T);
      pkt = ipb_->prepend_metadata_hdr(pkt, 0, 0);
    }

    // from PacketEnqueuer used by PacketGen etc.
    void enqueue(uint16_t port_num, MODEL_CHIP_NAMESPACE::Packet*& pkt, bool pgen) {
      int orig_len = pkt->len();
      uint64_t T = UINT64_C(0);
      model_timer::ModelTimerGetTime(T);
      // If we have IPB pointer and a PacketGen packet prepend an IPB header
      // - this currently used for WIP only
      // - note the chan is derived from the packet's ingr port
      if ((pgen) && (ipb_ != nullptr)) add_ipb_header(pkt, T);
      PktInfo *pi = new PktInfo(pkt, port_num, pgen, src_pipe_, 0, T);
      print_hdr(pkt, orig_len);
      shared_q_->push_pktinfo(pi);
    }

 private:
    MODEL_CHIP_NAMESPACE::RmtObjectManager  *om_;
    MODEL_CHIP_NAMESPACE::Ipb               *ipb_;
    SharedQ                                 *shared_q_;
    uint8_t                                  src_pipe_;
  };



  // Singleton Test obj to access SharedQ
  class TestPktQ {
    static uint64_t make_pkt_key(uint8_t type, uint16_t port, uint16_t len, uint32_t hash) {
      return (static_cast<uint64_t>(hash & 0xFFFFFFFFu) << (0))        |
             (static_cast<uint64_t>(len  & 0x3FFF)      << (0+32))     |
             (static_cast<uint64_t>(port & 0x1FF)       << (0+32+14))  |
             (static_cast<uint64_t>(type & 0xFF)        << (0+32+14+9));
    }

 public:
    TestPktQ()
        : om_(nullptr), shared_q_(), pending_packets_(),
          per_pipe_ { { { &shared_q_,0 }, { &shared_q_,1 },
                        { &shared_q_,2 }, { &shared_q_,3 } } }   {
      for (int type = 0; type < 256; type++) n_pending_packets_[type] = 0;
      micros_per_tick_ = UINT64_C(0);
    }
    ~TestPktQ() {
      pending_packets_.clear();
    }

    // This is the object we need to pass to PktGen CTOR
    TestPerPipePktQ* get_per_pipe_q(int pipe) {
      return ((pipe >= 0) && (pipe < 4)) ? &per_pipe_[pipe] : nullptr;
    }
    uint32_t  cnt_pktinfo()            { return shared_q_.cnt();            }
    void     push_pktinfo(PktInfo *pi) {        shared_q_.push_pktinfo(pi); }
    PktInfo*  get_pktinfo()            { return shared_q_.get_pktinfo();    }
    void     free_pktinfo(PktInfo *pi) {        shared_q_.free_pktinfo(pi); }
    int       get_times(uint64_t *ts1, uint64_t *ts2) {
      // Return model time and sweep time (if we have an OM ptr)
      assert( (ts1 != nullptr) && (ts2 != nullptr) );
      *ts1 = UINT64_C(0); model_timer::ModelTimerGetTime(*ts1);
      *ts2 = (om_ != nullptr) ?om_->time_get_cycles() :UINT64_C(0);
      return (om_ != nullptr) ?2 :1;
    }
    void     check_time() {
      uint64_t ts1, ts2; // If have both times check the same
      if ((get_times(&ts1, &ts2) < 2) || (ts1 == ts2)) return;
      printf("TestPktQ: ts1=%" PRId64 " ts2=%" PRId64 " (delta=%" PRId64 ")\n",
             ts1, ts2, ts1-ts2);
    }
    void     sync_time() {
      uint64_t ts1, ts2; // If have both times sync'em up
      if (get_times(&ts1, &ts2) < 2) return;
      if      (ts1 > ts2) model_timer::ModelTimerIncrement(ts1 - ts2);
      else if (ts2 > ts1) om_->time_increment_cycles(ts2 - ts1);
    }
    void     wait(uint64_t Twait) {
      model_timer::ModelTimerIncrement(Twait);
      if (micros_per_tick_ > UINT64_C(0))
        std::this_thread::sleep_for(std::chrono::microseconds(Twait*micros_per_tick_));
    }
    PktInfo* wait_pktinfo(uint64_t Twait, uint64_t *Tactual) {
      uint64_t Tdelta = UINT64_C(8);
      uint64_t Tstart = UINT64_C(0);
      model_timer::ModelTimerGetTime(Tstart);
      uint64_t Tnow = Tstart;
      uint64_t Tend = Tnow + Twait;
      PktInfo *pi = get_pktinfo();
      while ((Tnow < Tend) && (pi == nullptr)) {
        uint64_t Tinc = (Tnow + Tdelta > Tend) ?(Tend - Tnow) :Tdelta;
        wait(Tinc);
        model_timer::ModelTimerGetTime(Tnow);
        Tdelta += Tdelta;
        pi = get_pktinfo();
      }
      if (Tactual != nullptr) *Tactual = Tnow - Tstart;
      return pi;
    }

    void handle_pending_packet(uint8_t type, uint16_t port,
                               uint16_t len, uint32_t hash, int32_t delta) {
      uint64_t key = make_pkt_key(type, port, len, hash);
      int cnt = 0;
      try {
        cnt = pending_packets_.at(key);
      } catch (const std::exception& e) { }
      try {
        pending_packets_.emplace(key, cnt + delta);
        n_pending_packets_[type] += delta;
        assert( n_pending_packets_[type] >= 0 );
      } catch (const std::exception& e) { }
    }
    void add_pending_packet(uint8_t type, uint16_t port, uint16_t len, uint32_t hash) {
      handle_pending_packet(type, port, len, hash, +1);
    }
    void add_pending_packet(uint8_t type, PktInfo *pi) {
      assert( pi->pkt() != nullptr );
      add_pending_packet(type, pi->port(), pi->pkt()->len(), pi->pkt()->hash());
    }
    void remove_pending_packet(uint8_t type, uint16_t port, uint16_t len, uint32_t hash) {
      handle_pending_packet(type, port, len, hash, -1);
    }
    void remove_pending_packet(uint8_t type, PktInfo *pi) {
      assert( pi->pkt() != nullptr );
      remove_pending_packet(type, pi->port(), pi->pkt()->len(), pi->pkt()->hash());
    }
    int count_pending_packets(uint8_t type) {
      return n_pending_packets_[type];
    }

    void set_micros_per_tick(uint64_t m_per_t) { micros_per_tick_ = m_per_t; }
    void set_debug(bool tf)                    { shared_q_.set_debug(tf); }

 private:
    static constexpr int num_pipes = MODEL_CHIP_NAMESPACE::RmtDefs::kPipesMax;
    static_assert( ((num_pipes > 0) && (num_pipes <= 4)), "Incorrect num pipes" );

    MODEL_CHIP_NAMESPACE::RmtObjectManager   *om_;
    SharedQ                                   shared_q_;
    std::unordered_map< uint64_t, int >       pending_packets_;
    std::array< int, 256 >                    n_pending_packets_;
    std::array< TestPerPipePktQ, num_pipes >  per_pipe_;
    uint64_t                                  micros_per_tick_;
  };

}

#endif /* _TEST_PKT_Q_ */
