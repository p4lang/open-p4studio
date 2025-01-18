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

#ifndef __JBAY_SHARED_PKTGEN_REG_H
#define __JBAY_SHARED_PKTGEN_REG_H

#include <register_includes/pgr_ebuf_port_ctrl_array.h>
#include <register_includes/pgr_eth_cpu_ctrl.h>
#include <register_includes/pgr_tbc_port_ctrl.h>
#include <register_includes/pgr_pgen_ctrl.h>

#include <register_includes/pgr_port_down_dis_mutable.h>
#include <register_includes/pgr_pgen_port_down_ctrl.h>
#include <register_includes/pgr_pgen_port_down_event_mask_array.h>
#include <register_includes/pgr_pgen_port_down_vec_clr.h>
#include <register_includes/pgr_pgen_retrigger_port_down.h>
#include <register_includes/pgr_ipb_port_ctrl.h>

#include <packet.h>
#include <rmt-defs.h>
#include <pipe-object.h>
#include <pktgen-mem.h>
#include <common/rmt-util.h>
#include <common/bounded_queue.h>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <queue>
#include <bitset>

namespace MODEL_CHIP_NAMESPACE {

class PktGen;
class PktGenAppReg;
class PktGenReg;
class PacketEnqueuer;

struct PktGenEventData {
  BitVector<RmtDefs::kPktGenMatchWidth> key{};
  int down_port=0;
  int addr=0;
  int length=0;
  void reset() {
    down_port=0;
    addr=0;
    length=0;
    key.fill_all_zeros();
  }
};

class PktGenReg : public PipeObject {

 public:

  static constexpr int kSleepTime = 1000;
  static constexpr int kMatchWidth   = RmtDefs::kPktGenMatchWidth;
  static constexpr int kChannelModes = 5;
  static constexpr int kChannelsPerEthernet = 4;
  static constexpr int kPortMax     = RmtDefs::kParsers * RmtDefs::kParserChannels;
  static_assert(kChannelsPerEthernet==4,"Next bit only works for 4 channels!");
  static constexpr uint8_t ch_mode_[kChannelModes][kChannelsPerEthernet] = {
    {0, 0, 0, 0},
    {0, 2, 0, 2},
    {0, 2, 0, 3},
    {0, 2, 1, 2},
    {0, 2, 1, 3}
  };
  static constexpr int kFcsLen = Packet::kPktFcsLen; // CRC length to pad generated packets by (4B)

  PktGenReg(int chip, int pipe, RmtObjectManager* om, PktGen* pgen, PacketEnqueuer* pe);
  //PktGenReg(int chip = 0, int pipe = 0, RmtObjectManager* om=nullptr, PktGen* pgen = nullptr);
  virtual ~PktGenReg(void);

  // Only used so unit tests can intercept the enqueuer
  void set_enqueuer(PacketEnqueuer* pe) { packet_enqueuer_ = pe; }

  std::array<int,kChannelsPerEthernet> get_eth_seq() {
    static_assert(kChannelsPerEthernet==4,"Only works for 4 channels!");
    auto seq =  eth_cpu_ctrl_.channel_seq();
    return {  seq & 3,
             (seq>>2) & 3,
             (seq>>4) & 3,
             (seq>>6) & 3 };
  }

  // test if ethernet channel is set up in sequences correctly
  bool check_eth_channel(int ch) {
    RMT_ASSERT( ch>=0 && ch<4 );
    // 0: 1 channel mode; with channel sequence 0,0,0,0
    // 1: 2 channel mode; with channel sequence 0,2,0,2
    // 2: 3 channel mode; with channel sequence 0,2,0,3
    // 3: 3 channel mode; with channel sequence 0,2,1,2
    // 4: 4 channel mode; with channel sequence 0,2,1,3
    int mode = eth_cpu_ctrl_.channel_mode();
    bool ch_valid_for_mode =
        ( mode==0  && (ch==0) )                   ||
        ( mode==1  && (ch==0 || ch==2) )          ||
        ( mode==2  && (ch==0 || ch==2 || ch==3) ) ||
        ( mode==3  && (ch==0 || ch==2 || ch==1) ) ||
        ( mode==4 );
    auto channel_seq = get_eth_seq();
    bool ch_in_seq =
        (ch==channel_seq[0]) ||
        (ch==channel_seq[1]) ||
        (ch==channel_seq[2]) ||
        (ch==channel_seq[3]);
    return ch_in_seq && ch_valid_for_mode;
  }

  // test if EBUF channel is available for recirculation
  void check_channel(int ch, bool* recirc, bool* drop);

  bool channel_enabled_for_packet_gen(int ch) {
    RMT_ASSERT( ch>=0 && ch <8 );
    return (ipb_port_ctrl_.pgen_channel_en() >> ch) & 1;
  }

  bool recirc_packet(int channel, Packet** packet);
  void port_down(uint16_t port);
  void port_up(uint16_t port);
  void stop(void);


  template<typename BUF_READER, int MATCH_WIDTH>
  void get_snoop_val(BUF_READER* buf_reader, BitVector<MATCH_WIDTH> *val) {
    constexpr int kBytesInKey = MATCH_WIDTH / 8;
    uint8_t buf[ kBytesInKey ];
    buf_reader->get_buf(buf, 0, kBytesInKey);

    for (unsigned int i = 0; i < kBytesInKey; i ++) {
      val->set_byte( buf[i], kBytesInKey-1-i ); // first byte is most significant
    }
  }

  // handles recirculation packets
  void recirc_snoop(Packet* packet, int channel);
  // handles ingress deparser metadata
  void deparser_snoop(PacketBuffer* pb, int addr, int len);

  int get_gen_pipe() { return pgen_ctrl_.pipe_id(); }

  Packet* make_packet_from_buffer(int addr, int size, int channel);

  void get_phase0_metadata(const int index,const int size, uint8_t *phase0_metadata);

  uint8_t pktgen_channel_enable_mask() {
    return ipb_port_ctrl_.pgen_channel_en();
  }
  uint8_t recirc_trigger_channel_enable_mask() {
    return ipb_port_ctrl_.recir_channel_en();
  }

  uint64_t get_next_ev_num() { return ++ev_num_; }

  bool enqueue_generated_packet( int ch, Packet* p );
  void port_down_set_sent(int port);
  // returns the next port that is triggered but not yet sent, or -1 if none
  int get_next_port_down_unsent(int port, int which_mask);

  void update_port_down_registers_for_event(int port,int which_mask);

  bool port_down_event_mask(int port, int which_mask) {
    RMT_ASSERT( port>=0 && port <= kPortMax);
    RMT_ASSERT( which_mask>=0 && which_mask <= 1);
    return port_down_event_mask_.mask( which_mask, map_port_to_vector_bit(port));
  }

  PktGen* pktgen_; // this is only used publicly for old test mode stuff

 private:
  RmtObjectManager* om_;
  bool initialized_;
  // PktGen Shared Buffer
  PktGenMem<
    BFN_MEM_PIPE_PKTGEN_BUFMEM_CNT(0),
    BFN_MEM_PIPE_PKTGEN_BUFMEM_ESZ(0),
    BFN_MEM_PIPE_PKTGEN_BUFMEM_OFFSET(0)> pgen_buffer_;

  PacketEnqueuer* packet_enqueuer_;
  //uint8_t ch_tdm[RmtDefs::kChlsPerIpb];
  uint8_t curr_seq_ch_;

  std::array<PktGenAppReg*, RmtDefs::kPktGenApps> pgen_app_;

  std::thread* main_thread_;
  bool done_;

  std::mutex port_down_mutex_;
  // port_down_vector is the same as in the hardware - ie, it is set
  //  when the port goes down and cleared when the event is scheduled
  // there is one for each mask
  std::array<BitVector< kPortMax >,2> port_down_vector_{};
  BitVector< kPortMax > port_down_sent_{};
  BitVector< kPortMax > port_down_dis_before_write_{};
  // ports that are down at the moment
  BitVector< kPortMax > ports_currently_down_{};

  // PktGen Phase0 Metadata memory - unused on WIP
  PktGenMem<
    BFN_MEM_PIPE_PKTGEN_PHASE0META_CNT(0),
    BFN_MEM_PIPE_PKTGEN_PHASE0META_ESZ(0),
    BFN_MEM_PIPE_PKTGEN_PHASE0META_OFFSET(0)> pgen_phase0_meta_;

  using PacketQueue =  model_common::BoundedQueue<Packet*>;
  std::array<PacketQueue,RmtDefs::kChlsPerIpb> gen_q_;
  std::array<PacketQueue,RmtDefs::kChlsPerIpb> recir_q_;

  std::atomic<uint64_t> ev_num_{0}; // locally unique event number

  register_classes::PgrTbcPortCtrl       tbc_port_ctrl_;
  register_classes::PgrEthCpuCtrl        eth_cpu_ctrl_;
  register_classes::PgrEbufPortCtrlArray ebuf_port_ctrl_;

  register_classes::PgrPgenCtrl           pgen_ctrl_;

  register_classes::PgrPortDownDisMutable         port_down_dis_;
  register_classes::PgrPgenPortDownCtrl           port_down_ctrl_;
  register_classes::PgrPgenPortDownEventMaskArray port_down_event_mask_;
  register_classes::PgrPgenPortDownVecClr         port_down_vec_clr_;    // only app_disable() implemented
  register_classes::PgrPgenRetriggerPortDown      retrigger_port_down_;
  register_classes::PgrIpbPortCtrl                ipb_port_ctrl_;

  std::mutex reg_mutex;

  // The ethernet port is ports 2-5, but uses port down vector bits 4-7 on pipe 0
  int map_port_to_vector_bit( int port ) {
    if ( (pipe_index() == 0) && (port >=2) && (port<=5) ) {
      return port + 2;
    }
    return port;
  }
  int map_vector_bit_to_port( int vector_bit ) {
    if ( (pipe_index() == 0) && (vector_bit >=4) && (vector_bit<=7) ) {
      return vector_bit - 2;
    }
    return vector_bit;
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(PktGenReg);
  bool last_retrigger_en_{false};
  bool last_port_down_vec_clr_en_{false};

  void start(void);

  void start_or_stop(void);

  void port_down_dis_callback();
  void retrigger_callback();
  void port_down_vec_clr_callback();

  void reset(void);
  uint8_t get_seq_ch(void);
  uint8_t get_tdm_ch(void);

  void main_proc();

  void port_down_internal(uint16_t port);

}; // class PktGenReg

}; // namespace MODEL_CHIP_NAMESPACE {

#endif
