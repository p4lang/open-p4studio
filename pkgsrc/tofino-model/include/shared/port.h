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

#ifndef _SHARED_PORT_
#define _SHARED_PORT_

#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include <chip.h>
#include <rmt-log.h>
#include <rmt-defs.h>
#include <rmt-object.h>
#include <parser-block.h>
#include <deparser-block.h>
#include <ipb.h>
#include <epb.h>
#include <mac.h>
#include <pipe.h>
#include <packet.h>
#include <eop.h>
#include <phv.h>
#include <register_includes/pgr_port_down_dis.h>
#include <chip.h>

namespace MODEL_CHIP_NAMESPACE {

  class Mirror;

  class Port : public RmtObject {

 private:
    // XXX: packet version should be 0 initially
    static constexpr uint8_t  kInitialVersion = 0;
    static constexpr uint64_t kInitialSpeed = UINT64_C(10000000000);
    static constexpr int      kMetaHdrWords = 2;
    static constexpr int      kMetaHdrRecircShift = 63;
    static constexpr int      kMetaHdrV0Shift = 61; // 1b pad before recirc
    static constexpr int      kMetaHdrV1Shift = 60;
    static constexpr int      kMetaHdrPiShift = 48; // 3b pad before versions
    static constexpr int      kMetaHdrTsShift = 0;

    // PortIndex = 9b
    // A) pipe[8:7] group|mac[6:2] chan[1:0] (Tofino)
    //   - mac values 0-15 are real MACs, 16-17 are CPU MACs etc
    //   - depending on MAC config might not use all mac_port vals
    //     (eg 100G just 0, 50G/40G 0/2, 10G 0/1/2/3)
    //
    // B) pipe[8:7] group|mac[6:3] chan[2:0] (JBay)
    // Note might only use 1b/0b of pipe depending on chip/SKU
    //
    // C) WIP is same as B above, *except*
    //   - externally only 36 ports which are mapped into 72 at
    //     ingress and back to 36 at egress
    //   - everything treated as 72 internally (so 3b chan ids)
    //   - BUT mac channel only 2b so get_mac_chan() needs >>1
    //   - see RmtDefs::{kIngressLocalPortShift|kEgressLocalPortShift|kMacChanShift}
    //
    static constexpr int kPipes = RmtDefs::kPipesMax;
    static constexpr int kChips = Chip::kMaxNumChipsInPackage;
    static constexpr int kParsers = RmtDefs::kParsers;
    static constexpr int kParserChannels = RmtDefs::kParserChannels;
    static constexpr int kPortGroupsPerPipe = RmtDefs::kPortGroupsPerPipe;


    // XXX: Rename kPortMac,kPortPort to kPortGroup,kPortChan
    static constexpr int kPortChanShift   = 0;
    static constexpr int kPortChanWidth  = RmtDefs::kPortChanWidth;
    static constexpr int kPortChanMask   = (1<<kPortChanWidth)-1;
    static constexpr int kPortGroupWidth = RmtDefs::kPortGroupWidth;
    static constexpr int kPortGroupShift = kPortChanShift + kPortChanWidth;
    static constexpr int kPortGroupMask  = (1<<kPortGroupWidth)-1;
    static constexpr int kPortPortShift  = 0;
    static constexpr int kPortPortWidth  = kPortChanWidth + kPortGroupWidth;
    static constexpr int kPortPortMask   = (1<<kPortPortWidth)-1;
    static constexpr int kPortPipeShift  = kPortGroupShift + kPortGroupWidth;
    static constexpr int kPortPipeWidth  = RmtDefs::kPortPipeWidth;
    static constexpr int kPortPipeMask   = (1<<kPortPipeWidth)-1;
    static constexpr int kPortDieShift   = kPortPipeShift + kPortPipeWidth;
    static constexpr int kPortDieWidth   = RmtDefs::kPortDieWidth;
    static constexpr int kPortDieMask    = (1<<kPortDieWidth)-1;

    static constexpr int kPortParserChanShift = 0;
    static constexpr int kPortParserChanWidth = RmtDefs::kPortParserChanWidth;
    static constexpr int kPortParserChanMask  = (1<<kPortParserChanWidth)-1;
    static constexpr int kPortParserWidth     = RmtDefs::kPortParserWidth;
    static constexpr int kPortParserShift     = kPortParserChanShift + kPortParserChanWidth;
    static constexpr int kPortParserMask      = (1<<kPortParserWidth)-1;

    static constexpr int kPortIpbChanShift = 0;
    static constexpr int kPortIpbChanWidth = RmtDefs::kPortIpbChanWidth;
    static constexpr int kPortIpbChanMask  = (1<<kPortIpbChanWidth)-1;
    static constexpr int kPortIpbWidth     = RmtDefs::kPortIpbWidth;
    static constexpr int kPortIpbShift     = kPortIpbChanShift + kPortIpbChanWidth;
    static constexpr int kPortIpbMask      = (1<<kPortIpbWidth)-1;

    static constexpr int kPortEpbChanShift = 0;
    static constexpr int kPortEpbChanWidth = RmtDefs::kPortEpbChanWidth;
    static constexpr int kPortEpbChanMask  = (1<<kPortEpbChanWidth)-1;
    static constexpr int kPortEpbWidth     = RmtDefs::kPortEpbWidth;
    static constexpr int kPortEpbShift     = kPortEpbChanShift + kPortEpbChanWidth;
    static constexpr int kPortEpbMask      = (1<<kPortEpbWidth)-1;

    static constexpr int kNonPortMask      = ((kPortDieMask << kPortDieShift) |
                                              (kPortPipeMask << kPortPipeShift));
    static constexpr int kNonPipeMask      = ((kPortDieMask << kPortDieShift) |
                                              (kPortGroupMask << kPortGroupShift) |
                                              (kPortChanMask << kPortChanShift));

   public:
    static uint64_t zeroVal;
    static constexpr int kPortChanMin = 0;
    static constexpr int kPortChanMax = (1<<kPortChanWidth)-1;
    static constexpr int kPortGroupMax = kPortGroupsPerPipe-1;
    static constexpr int kPortGroupMin = 0;
    static constexpr int kPortsPerPipe = RmtDefs::kPortsPerPipe;
    static constexpr int kPortsPerPipeMax = kPortsPerPipe + 1; // 72 for mirror

    static_assert( ((kPortGroupMax & kPortGroupMask) == kPortGroupMax), "Too many PortGroups");

    static inline int lr_shift(int v, int s)   { return (s < 0) ?(v << -s) :(v >> s); }
    static inline int get_die_num(int p)       { return (p >> kPortDieShift) & kPortDieMask; }
    static inline int get_pipe_num(int p)      { return (p >> kPortPipeShift) & kPortPipeMask; }
    static inline int get_port_num(int p)      { return (p >> kPortPortShift) & kPortPortMask; }
    static inline int get_group_num(int p)     { return (p >> kPortGroupShift) & kPortGroupMask; }
    static inline int get_chan_num(int p)      { return (p >> kPortChanShift) & kPortChanMask; }
    static inline int get_parser_num(int p)    { return (p >> kPortParserShift) & kPortParserMask; }
    static inline int get_parser_chan(int p)   { return (p >> kPortParserChanShift) & kPortParserChanMask; }
    static inline int get_ipb_num(int p)       { return (p >> kPortIpbShift) & kPortIpbMask; }
    static inline int get_ipb_chan(int p)      { return (p >> kPortIpbChanShift) & kPortIpbChanMask; }
    static inline int get_epb_num(int p)       { return (p >> kPortEpbShift) & kPortEpbMask; }
    static inline int get_epb_chan(int p)      { return (p >> kPortEpbChanShift) & kPortEpbChanMask; }
    // Add some aliases - mac corresponds to Port group and mac_chan to Port chan
    static inline int get_mac_num(int p)       { return get_group_num(p); }
    static inline int get_mac_chan(int p)      { return lr_shift( get_chan_num(p), RmtDefs::kMacChanShift ); }

    static inline int swap_port(int p, int new_port) {
      return (p & kNonPortMask) | ((new_port & kPortPortMask) << kPortPortShift);
    }

    static inline int swap_pipe(int p, int new_pipe) {
      return (p & kNonPipeMask) | ((new_pipe & kPortPipeMask) << kPortPipeShift);
    }

    static inline bool is_valid_chip_num(int chip) {
      return (chip >= 0) && (chip < kChips);
    }

    static inline bool is_valid_pipe_num(int pipe) {
      return (pipe >= 0) && (pipe < kPipes);
    }

    static inline bool is_valid_group_num(int group) {
      return (group >= kPortGroupMin) && (group <= kPortGroupMax);
    }

    static inline bool is_valid_chan_num(int chan) {
      return (chan >= kPortChanMin) && (chan <= kPortChanMax);
    }

    static inline bool is_valid_pipe_local_port_index(int port) {
      int group = get_group_num(port);
      int chan = get_chan_num(port);
      return (is_valid_group_num(group) && is_valid_chan_num(chan));
    }

    static inline int make_port_index(int pipe, int group, int chan) {
      RMT_ASSERT(is_valid_pipe_num(pipe));
      RMT_ASSERT(is_valid_group_num(group));
      RMT_ASSERT(is_valid_chan_num(chan));
      return (((pipe & kPortPipeMask) << kPortPipeShift) |
              ((group & kPortGroupMask) << kPortGroupShift) |
              ((chan & kPortChanMask) << kPortChanShift));
    }

    static inline int make_chip_port_index(int chip, int pipe, int port) {
      int group, chan;
      RMT_ASSERT(is_valid_chip_num(chip));
      RMT_ASSERT(is_valid_pipe_num(pipe));
      group = get_group_num(port);
      chan = get_chan_num(port);
      RMT_ASSERT(is_valid_group_num(group));
      RMT_ASSERT(is_valid_chan_num(chan));
      return (((chip & kPortDieMask) << kPortDieShift) |
	      ((pipe & kPortPipeMask) << kPortPipeShift) |
              ((group & kPortGroupMask) << kPortGroupShift) |
              ((chan & kPortChanMask) << kPortChanShift));
    }

    static inline int make_port_index(int pipe, int local_port) {
      int group = get_group_num(local_port);
      int chan = get_chan_num(local_port);
      return make_port_index(pipe, group, chan);
    }
    static inline int get_pipe_local_port_index(int port) {
      int group = get_group_num(port);
      int chan = get_chan_num(port);
      return make_port_index(0, group, chan);
    }
    static inline int get_die_local_port_index(int port) {
      int pipe = get_pipe_num(port);
      int group = get_group_num(port);
      int chan = get_chan_num(port);
      return make_port_index(pipe, group, chan);
    }
    /**
     * Add given die index to given port index. Any existing die index will be
     * replaced.
     * @param port_index
     * @param die_index
     * @return port index with die index prepended
     */
    static inline int add_die_index(int port_index, int die_index) {
      int die_local_port = get_die_local_port_index(port_index);
      return die_local_port | ((die_index & kPortDieMask) << kPortDieShift);
    }


    // XXX: WIP: allow external view of ports (36 per pipe) to be
    // mapped into internal view of ports (72 per pipe), and vice-versa
    //
    static inline int port_map(int port, int shift) {
      return swap_port(port, lr_shift( get_port_num(port), shift ) );
    }
    static inline int port_map_inbound(int port) {
      return port_map(port, RmtDefs::kIngressLocalPortShift);
    }
    static inline int port_map_pipe_to_tm(int port) {
      return port_map(port, RmtDefs::kPipeTmLocalPortShift);
    }
    static inline int port_map_tm_to_pipe(int port) {
      return port_map(port, RmtDefs::kTmPipeLocalPortShift);
    }
    static inline int port_map_outbound(int port) {
      return port_map(port, RmtDefs::kEgressLocalPortShift);
    }



    Port(RmtObjectManager *om, int portIndex);
    Port(RmtObjectManager *om, int portIndex, int macIndex, int macChan);
    virtual ~Port();
    void reset();

    inline int             port_index()         const { return port_index_; }
    inline bool            enabled()            const { return enabled_; }
    inline uint8_t         version()            const { return version_; }
    inline Mac            *mac()                const { return mac_; }
    inline ParserBlock    *parser()             const { return parser_; }
    inline Pipe           *pipe()               const { return pipe_; }
    inline DeparserBlock  *deparser()           const { return deparser_; }

    inline void            set_version(uint8_t v)                 { version_ = v; }
    inline void            set_mac(Mac *mac)                      { mac_ = mac; }
    inline void            set_parser(ParserBlock *parser)        { parser_ = parser; }
    inline void            set_pipe(Pipe *pipe)                   { pipe_ = pipe; }
    inline void            set_deparser(DeparserBlock *deparser)  { deparser_ = deparser; }
    inline void            set_ipb(Ipb *ib)                       { ipb_ = ib; }
    inline void            set_epb(Epb *eb)                       { epb_ = eb; }
    inline void            set_mirror(Mirror *mr)                 { mirror_ = mr; }

    inline uint16_t        logical_port_index() const { return config_.logical_port_index; }
    inline uint64_t        speed()              const { return config_.speed_bits_per_sec; }
    inline uint32_t        hdr_word0()          const { return config_.hdr_word0; }
    inline uint32_t        hdr_word1()          const { return config_.hdr_word1; }
    inline int             pipe_index()         const { return config_.pipe_index; }
    inline int             mac_index()          const { return config_.mac_index; }
    inline int             parser_index()       const { return config_.parser_index; }
    inline int             ipb_index()          const { return config_.ipb_index; }
    inline int             epb_index()          const { return config_.epb_index; }
    inline int             mac_chan()           const { return config_.mac_chan; }
    inline int             parser_chan()        const { return config_.parser_chan; }
    inline int             ipb_chan()           const { return config_.ipb_chan; }
    inline int             epb_chan()           const { return config_.epb_chan; }

    // Easy access to ingress/egress parser/epb/deparser for some port
    inline Parser         *ingress_parser()   { return (parser_ != NULL) ? parser_->ingress() : NULL; }
    inline Parser         *egress_parser()    { return (parser_ != NULL) ? parser_->egress() : NULL; }
    inline Deparser       *ingress_deparser() { return (deparser_ != NULL) ? deparser_->ingress() : NULL; }
    inline Deparser       *egress_deparser()  { return (deparser_ != NULL) ? deparser_->egress() : NULL; }
    inline Ipb            *ipb()              { return ipb_;}
    inline Epb            *epb()              { return epb_; }
    inline Mirror         *mirror()           { return mirror_;}



    // Parse packet to PHV
    Phv *parse(Packet *packet);
    // Deparse PHV to packet
    Packet *deparse(Phv *phv);
    // Run PHV through MAUs
    Phv *matchaction(Phv *phv);
    // Run PHV/oPHV through MAUs (just for DV)
    Phv *matchaction2(Phv *phv, Phv *ophv);
    // Run packet through Parse then MAUs - no deparse
    Phv *parse_matchaction(Packet *packet);
    // Handle EOPs
    void handle_eop(const Eop &eop);

    // Process packet through ingress or egress Parser/MAUs/Deparser
    Packet *process_inbound(Packet *packet, uint64_t& recirc=zeroVal);
    Packet *process_outbound(Packet *packet);
    // Packet *process_recirc(Packet *packet, uint64_t& recirc=zeroVal);
    // Process packet in through ingress Parser/MAU/Deparser
    // and then out through egress Parser/MAU/Deparser
    Packet *process(Packet *packet, uint64_t& recirc=zeroVal);


 private:
    int                              port_index_;
    bool                             enabled_;
    uint8_t                          version_;
    Mac                             *mac_;
    ParserBlock                     *parser_;
    Pipe                            *pipe_;
    DeparserBlock                   *deparser_;
    Ipb                             *ipb_;
    Epb                             *epb_;
    Mirror                          *mirror_;
    PortConfig                       config_;
    register_classes::PgrPortDownDis port_en;

  };
}
#endif // _SHARED_PORT_
