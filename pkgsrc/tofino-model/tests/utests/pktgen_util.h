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

#ifndef _PKTGEN_UTIL_
#define _PKTGEN_UTIL_

#include <utests/test_util.h>
#include "test_pkt_q.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <rmt-defs.h>
#include <bitvector.h>
#include <port.h>
#include <packet.h>
#include <packet-enqueuer.h>


namespace MODEL_CHIP_TEST_NAMESPACE {

  struct PktgenRandAppConfig {
    // maskTypes is a mask of allowed types
    uint16_t  maskTypes;
    // maskFlags is a mask of allowed flags
    // setFlags is a set of flags to set unconditionally
    uint8_t   maskFlags, setFlags;
    // maskBits determines how often a recirc/may match occurs
    // (eg if 3 bits only masked, and assuming random packet data
    //  should match 1 packet in 8)
    uint8_t   maskBits;
    // maxBatches/maxPackets
    uint16_t  maxBatches;
    uint16_t  maxPackets;
    uint16_t  maxBatchesPackets; // max product of 2 above
    // minPeriod/maxPeriod effects min|max ipg|ibg and timer_cycles
    uint32_t  minPeriod;
    uint32_t  maxPeriod;

    // PER-APP-TYPE config
    uint8_t   numEachType[TestUtil::kPgenAppTypeMax+1];
    uint8_t   flagsMaskEachType[TestUtil::kPgenAppTypeMax+1];
    uint8_t   flagsSetEachType[TestUtil::kPgenAppTypeMax+1];
    uint8_t   flagsSetFirstAppOfType[TestUtil::kPgenAppTypeMax+1];
  };



  class PktgenRandApp {
 public:
    using BV128 = MODEL_CHIP_NAMESPACE::BitVector<128>;
    using BV72  = MODEL_CHIP_NAMESPACE::BitVector< 72>;
    using PKT   = MODEL_CHIP_NAMESPACE::Packet;

    static constexpr int      kNumAppsPerPipe  = 16;
    static constexpr int      kNumPortsPerPipe = 72;
    static constexpr int      kPktBufWords     = 1024;
    static constexpr int      kMeta1BufWords   = kNumPortsPerPipe;
    static constexpr uint64_t kAppFifoSize     = UINT64_C(4);

    PktgenRandApp(TestUtil *tu, int pipe, int app, uint64_t seed,
                  const struct PktgenRandAppConfig &cfg, uint8_t pgen_chans);
    ~PktgenRandApp();

    uint8_t        pgen_chans()            const { return pgen_chans_; }
    bool           enabled()               const { return enabled_; }
    uint8_t        type()                  const { return type_; }
    uint8_t        flags()                 const { return flags_; }
    uint8_t        chan()                  const { return chan_; }
    uint8_t        prio()                  const { return prio_; }
    uint16_t       payload_addr()          const { return payload_addr_; }
    uint16_t       payload_size()          const { return payload_size_; }
    uint8_t        ing_port()              const { return ing_port_; }
    bool           ing_inc()               const { return ing_inc_; }
    uint8_t        ing_wrap_port()         const { return ing_wrap_port_; }
    uint8_t        ing_port_pipe_id()      const { return ing_port_pipe_id_; }
    uint8_t        recirc_out_ebuf()       const { return recirc_out_ebuf_; }
    BV128          recirc_val_bv()         const { return recirc_val_bv_; }
    BV128          recirc_mask_bv()        const { return recirc_mask_bv_; }
    uint16_t       num_batches()           const { return num_batches_; }
    uint16_t       num_pkts()              const { return num_pkts_; }
    uint32_t       ibg_min()               const { return ibg_min_; }
    uint32_t       ibg_max()               const { return ibg_max_; }
    uint32_t       ipg_min()               const { return ipg_min_; }
    uint32_t       ipg_max()               const { return ipg_max_; }
    uint32_t       timer_cycles()          const { return timer_cycles_; }
    BV72           portdown_mask0()        const { return portdown_mask0_; }
    BV72           portdown_mask1()        const { return portdown_mask1_; }

    uint8_t        ibg_scaled_max()        const { return ibg_scaled_max_; }
    uint8_t        ibg_scale()             const { return ibg_scale_; }
    uint8_t        ipg_scaled_max()        const { return ipg_scaled_max_; }
    uint8_t        ipg_scale()             const { return ipg_scale_; }

    uint64_t       n_triggers_exp()        const { return n_triggers_exp_; }
    uint64_t       n_triggers_act()        const { return n_triggers_act_; }
    uint64_t       t_setup()               const { return t_setup_; }
    uint64_t       t_teardown()            const { return t_teardown_; }
    uint64_t       t_last_trigger()        const { return t_last_trigger_; }
    uint64_t       t_last_batch_trigger()  const { return t_last_batch_trigger_; }
    uint64_t       t_last_pkt_trigger()    const { return t_last_pkt_trigger_; }
    uint8_t        last_port()             const { return last_port_; }
    uint16_t       last_batch_num()        const { return last_batch_num_; }
    uint16_t       last_pkt_num()          const { return last_pkt_num_; }
    uint64_t       n_triggers_pending()    const {
      uint64_t n = UINT64_C(0);
      if (n_triggers_exp_ > n_triggers_act_) n = n_triggers_exp_ - n_triggers_act_;
      return n;
    }

    // Setters to track dynamic state as app runs
    void inc_n_triggers_exp()                    { n_triggers_exp_++; }
    void inc_n_triggers_act()                    { n_triggers_act_++; }
    void set_t_setup(uint64_t T)                 { t_setup_ = T; }
    void set_t_last_trigger(uint64_t T)          { t_last_trigger_ = T; }
    void set_t_last_batch_trigger(uint64_t T)    { t_last_batch_trigger_ = T; }
    void set_t_last_pkt_trigger(uint64_t T)      { t_last_pkt_trigger_ = T; }
    void set_last_port(uint8_t p)                { last_port_ = p; }
    void set_last_batch_num(uint16_t b)          { last_batch_num_ = b; }
    void set_last_pkt_num(uint16_t p)            { last_pkt_num_ = p; }

    void set_expected_port_down(int port)        { expected_port_downs_.set_bit(port); }
    void clear_expected_port_down(int port)      { expected_port_downs_.clear_bit(port); }
    bool get_expected_port_down(int port)        { return expected_port_downs_.bit_set(port); }
    // Utility methods
    bool pgen_chan_enabled();
    bool ebuf_linked(int ebuf);
    bool dprsr_triggered();
    bool recirc_triggered();
    bool recirc_triggered(int out_ebuf);
    bool pkt_triggered();
    bool timeout_triggered();
    bool fifo_full();
    bool key_matches(const BV128 &key);
    bool make_key_from_pkt(PKT *p, BV128 *bv);
    uint16_t make_pkt_hdr(uint8_t *hdrbuf,
                          uint16_t min_hdrlen, uint16_t max_hdrlen,
                          uint16_t pktlen);
    bool pkt_matches(PKT *p);
    uint64_t n_timeouts_expected();
    bool portdown_triggered(int port);
    int  header_len();
    void disable_configuration();

 private:
    bool     pick_enabled();
    uint8_t  pick_type();
    uint8_t  pick_flags();
    uint8_t  pick_chan();
    uint8_t  pick_prio();
    uint16_t pick_payload_addr(bool from_pkt);
    uint16_t pick_payload_size(uint16_t addr);
    uint8_t  pick_ing_port();
    bool     pick_ing_inc();
    uint8_t  pick_ing_wrap_port(uint8_t ing_port);
    uint8_t  pick_ing_port_pipe_id();
    uint8_t  pick_recirc_out_ebuf();
    BV128    pick_recirc_val();
    BV128    pick_recirc_mask(bool addrsize_from_pkt, uint16_t addr);
    uint16_t pick_num_batches(bool limit_batches);
    uint16_t pick_num_pkts(bool limit_pkts, uint16_t num_batches);
    uint32_t pick_ipg_min();
    uint32_t pick_ipg_max(uint32_t ipg_min);
    uint32_t pick_ibg_min(uint32_t ipg_max, uint16_t num_packets);
    uint32_t pick_ibg_max(uint32_t ipg_min, uint16_t num_packets);
    uint32_t pick_timer_cycles(uint32_t ipg_max, uint16_t num_packets,
                               uint32_t ibg_max, uint16_t num_batches);
    BV72     pick_portdown_mask(int which);

    void calculate_scaling(uint32_t max, uint32_t limit,
                           uint32_t *scaled_max, uint32_t *scale);
    void reset_configuration_state();
    void reset_dynamic_state();
    void pick_configuration_state();
    void program_reset();
    void program_configuration();
    void setup_configuration();


 private:
    TestUtil                      *tu_;
    int                            pipe_;
    int                            app_;
    uint64_t                       seed_;
    uint64_t                       seed2_;
    const PktgenRandAppConfig      cfg_;
    // These all determined using seed_ and cfg_
    uint8_t                        pgen_chans_;
    bool                           enabled_;
    uint8_t                        type_;
    uint8_t                        flags_;
    uint8_t                        chan_;
    uint8_t                        prio_;
    uint16_t                       payload_addr_;
    uint16_t                       payload_size_;
    uint8_t                        ing_port_;
    bool                           ing_inc_;
    uint8_t                        ing_wrap_port_;
    uint8_t                        ing_port_pipe_id_;
    uint8_t                        recirc_out_ebuf_;
    BV128                          recirc_val_bv_;
    BV128                          recirc_mask_bv_;
    uint16_t                       num_batches_;
    uint16_t                       num_pkts_;
    uint32_t                       ibg_min_;
    uint32_t                       ibg_max_;
    uint32_t                       ipg_min_;
    uint32_t                       ipg_max_;
    uint32_t                       timer_cycles_;
    BV72                           portdown_mask0_;
    BV72                           portdown_mask1_;
    // These calculated from ibg|ipg max
    uint8_t                        ipg_scaled_max_;
    uint8_t                        ipg_scale_;
    uint8_t                        ibg_scaled_max_;
    uint8_t                        ibg_scale_;
    // These dynamically set as needed
    uint64_t                       n_triggers_exp_;
    uint64_t                       n_triggers_act_;
    uint64_t                       t_setup_;
    uint64_t                       t_teardown_;
    uint64_t                       t_last_trigger_;
    uint64_t                       t_last_batch_trigger_;
    uint64_t                       t_last_pkt_trigger_;
    uint16_t                       last_port_;
    uint16_t                       last_batch_num_;
    uint16_t                       last_pkt_num_;
    BV72                           expected_port_downs_;
  };





  class PktgenRandAppMgr {
 public:
    using PKT = MODEL_CHIP_NAMESPACE::Packet;

    static constexpr int kNumAppsPerPipe  = PktgenRandApp::kNumAppsPerPipe;
    static constexpr int kNumPortsPerPipe = PktgenRandApp::kNumPortsPerPipe;
    static constexpr int kPktBufWords     = PktgenRandApp::kPktBufWords;
    static constexpr int kMeta1BufWords   = PktgenRandApp::kMeta1BufWords;

    PktgenRandAppMgr(TestUtil *tu, int pipe, uint64_t seed,
                     struct PktgenRandAppConfig *cfg);
    ~PktgenRandAppMgr();


    uint64_t app_seed(int app)                  const { return app_seed_[app]; }
    uint8_t  last_app()                         const { return last_app_; }
    void     set_app_seed(int app, uint64_t seed)     { app_seed_[app] = seed; }
    void     set_last_app(uint8_t app)                { last_app_ = app; }

    PktgenRandApp *get_app(int app) {
      assert((app >= 0) && (app < kNumAppsPerPipe));
      return apps_[app];
    }

    uint8_t pick_type(int app, uint8_t mask);
    void setup_apps();
    void disable_apps();
    void teardown_apps();
    void dump_apps();
    void setup_bufs();
    void teardown_bufs();
    void setup_ports(uint8_t flags);
    void teardown_ports();
    bool chan_for_pktgen(uint8_t chan);
    bool port_for_pktgen(uint16_t port);
    bool chan_for_recirc(uint8_t chan);
    bool port_for_recirc(uint16_t port);
    int  port_get_ebuf(uint16_t port);
    int  check_app_generated_pkt(PKT *p, PktInfo *pi, bool *periodic);



 private:
    TestUtil                                      *tu_;
    int                                            pipe_;
    uint64_t                                       seed_;
    PktgenRandAppConfig                           *cfg_;

    uint64_t                                       port_seed_;
    // Note 2x 64b words for each PktBuf/Meta1Buf entry
    std::array< uint64_t, kPktBufWords*2   >       pktbuf_vals_;
    std::array< uint64_t, kMeta1BufWords*2 >       meta1buf_vals_;
    std::array< uint64_t, kNumAppsPerPipe  >       app_seed_;

    uint8_t                                        tbc_chans_;
    uint8_t                                        ethcpu_chans_;
    uint8_t                                        ebuf_chans_;
    uint8_t                                        recirc_chans_;
    uint8_t                                        pktgen_chans_;

    uint8_t                                        num_each_type_[TestUtil::kPgenAppTypeMax+1];
    std::array< PktgenRandApp*, kNumAppsPerPipe >  apps_;
    uint8_t                                        last_app_;
  };


}

#endif /*  _PKTGEN_UTIL_ */
