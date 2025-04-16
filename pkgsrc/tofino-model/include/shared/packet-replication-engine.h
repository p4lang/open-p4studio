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

#ifndef _SHARED_PACKET_REPLICATION_ENGINE_
#define _SHARED_PACKET_REPLICATION_ENGINE_

#include <mutex>
#include <thread>
#include <array>
#include <condition_variable>

#include <rmt-object-manager.h>
#include <bitvector.h>
#include <pipe-object.h>
#include <queueing.h>
#include <packet-replication-engine-reg.h>
#include <chip.h>
#include <port.h>
#include <model_core/log-buffer.h>

namespace MODEL_CHIP_NAMESPACE {

  class Rdm;

  class PacketReplicationEngine : public PipeObject {

  public:
    static constexpr int kRxFifos = 4;
    static constexpr int kLocalPortWidth = RmtDefs::kTmPortsPerPipe;
    static constexpr int kDiePortWidth = kLocalPortWidth * RmtDefs::kPipesMax;
    static constexpr int kPortWidth = kLocalPortWidth * RmtDefs::kPresTotal;
    static constexpr int kLags = MemUtils::pre_lag_mem_size();
    static constexpr int kPmtSize = MemUtils::pre_pmt_mem_size();
    static constexpr int kLagArrSize = ((kPortWidth + 64 - 1) / 64);
    static constexpr int kLagLenMask = MemUtils::pre_lag_len_mask();

    enum class FifoMode {
      kFifoModeSP,
      kFifoModeDRR
    };

    PacketReplicationEngine(RmtObjectManager *om, int pipeIndex);
    ~PacketReplicationEngine() { stop(true); }

    bool ph_in(Packet *pkt, int cos);
    void start();
    void stop(bool right_now);

    inline int fifo_sz(int q) const { return reg_com()->fifo_q_size(q); }
    inline Rdm* rdm() { return get_object_manager()->rdm_get(); }
    inline int  bpt(int ver, int index) const { return reg_com()->bpt(ver, index); }
    inline void bpt(int ver, int index, int val) { reg_com()->write_bpt(ver, index, val); }
    inline uint32_t mit(int mgid) const { return reg_com()->mit(get_pre_pipe_index(), mgid); }
    inline void     mit(int mgid, uint32_t x) { reg_com()->write_mit(get_pre_pipe_index(), mgid, x); }
    void set_lag(int ver, int id, const BitVector<kPortWidth> &data);
    void set_lag_mbr(int ver, int id, int mbr, int val);
    const BitVector<kPortWidth>& lag(int ver, int id) const;
    void set_pmt_bit(int ver, int yid, int bit, int val);
    void write_pmt(int ver, int index, int segment, uint8_t upper, uint64_t lower);
    const BitVector<kPortWidth>& pmt(int ver, int id) const;
    BitVector<kPortWidth>& get_sw_mask(int ver);
    BitVector<kPortWidth>& get_hw_mask();
    inline void ctl_sw_mask(bool x) {use_sw_mask_ = x;}
    void ctl_hw_mask(bool x);
    inline bool use_sw_mask() const {return use_sw_mask_;}
    bool use_hw_mask();
    bool use_bpt();
    void ctl_bpt(bool x);
    void set_sw_mask_bit(int ver, bool x, int pos);
    void set_hw_mask_bit(bool x, int pos);
    void global_rid(uint16_t rid);
    inline int get_pre_pipe_index() const {return index_;}
    inline int get_chip_pipe_index() const {return (index_ & RmtDefs::kPipeMask);}

    void reset();
    void start_rdm_change_tracking();

    bool fifos_empty();
    bool enqueue(int fifo, Packet *pkt);

  private:
    void run();
    bool fifo_wait(bool wait);
    void one_pass();
    //bool enqueue(int fifo, Packet *pkt);
    void fifo_head(int fifo, Packet *&pkt);
    void fifo_pop(int fifo);
    size_t fifo_size(int fifo);
    bool fifo_ready(int fifo);
    int lag_hash(int ver, uint16_t hash, int id, uint16_t pkt_rid, uint16_t node_rid, int yid, model_core::LogBuffer& log_buf);
    bool walk_table(Packet *pkt, int max_copies);
    void generate_copies(Packet *pkt, uint16_t rid, uint64_t members, uint64_t mask);
    inline int make_port(int port) const { return make_port(chip_index_, get_chip_pipe_index(), port); }
    inline int make_port(int chip, int pipe, int port) const { return Port::make_chip_port_index(chip, pipe, port); }
    inline void make_indx(int port, int &pipe, int &idx) const { pipe = port >> 7; idx = port & 0x7F; }
    void get_lag_member(uint16_t hash, int ver, int id,
                        const BitVector<kPortWidth> &lag,
                        const BitVector<kPortWidth> &sw_mask,
                        const BitVector<kPortWidth> &hw_mask,
                        int &mbr_a, int &mbr_b, model_core::LogBuffer& lag_buf);
    inline bool is_port_local(int port) const { return (((chip_index_ << RmtDefs::kPortDieWidth) | get_chip_pipe_index()) ==
							((Port::get_die_num(port) << RmtDefs::kPortDieWidth) | Port::get_pipe_num(port))); }

    // FIXME - These should come from config registers
    FifoMode fifo_mode(int fifo);
    unsigned copies_per_pass();
    bool has_cpu_port();
    int get_cpu_port();
    int get_fifo_quantum(int fifo);
    int max_L1_nodes();
    int max_L2_nodes();
    bool is_pruned(int ver, int pkt_rid, int node_rid, int yid, int bit);
    uint16_t global_rid();
    bool generate_copy(int ver, int chip, int pipe, int port, Packet *pkt, int rid, bool rid_first);
    bool is_live(int ver, int pipe, int port);

    inline PacketReplicationEngineRegCom* reg_com() const { return get_object_manager()->pre_reg_com_get(); }

    int index_; // Which PRE this is (0 to 3).
    int chip_index_;

    // Input FIFO related.
    std::mutex                                                 rx_fifo_mutex_ = {};
    std::condition_variable                                    rx_fifo_cv_    = {};
    std::array<std::queue<Packet*>, kRxFifos>                  rx_fifo_;
    std::array<unsigned, kRxFifos>                             credit_ = {};

    // Tables

    // Port Masks.
    BitVector<kPortWidth>                                      sw_mask_ = {};
    BitVector<kPortWidth>                                      hw_mask_ = {};

    // Flags to control lookup behavior.
    bool                                                       use_sw_mask_ = true;

    // Thread Control.
    std::thread                                                worker_ = {};
    bool                                                       run_ = false;
    bool                                                       stopping_ = false;

    // To track the RDM-Change register.
    std::array<int, kRxFifos>                                  fifo_cnts_ = {};
    bool                                                       tracking_rdm_change_ = {};

    // Registers.
    PacketReplicationEngineRegPipe  reg_pipe_;
  };
}
#endif
