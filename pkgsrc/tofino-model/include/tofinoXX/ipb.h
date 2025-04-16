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

#ifndef _TOFINOXX_IPB_
#define _TOFINOXX_IPB_

#include <ipb-common.h>

#include <register_includes/chnl_ctrl.h>
#include <register_includes/chnl_metadata_fix.h>
#include <register_includes/chnl_metadata_fix2.h>
#include <register_includes/chnl_recirc_discard_pkt.h>
#include <register_includes/chnl_recirc_received_pkt_mutable.h>
#include <register_includes/glb_parser_maxbyte.h>

namespace MODEL_CHIP_NAMESPACE {

class IpbCounters : public IpbCountersCommon {
 public:
  IpbCounters(int chipIndex, int pipeIndex, int ipbIndex, int chanIndex);
  void reset() override;
  virtual void increment_chnl_deparser_drop_pkt() override;
  virtual void increment_chnl_resubmit_received_pkt() override;

  register_classes::ChnlRecircDiscardPkt  chnl_recirc_discard_pkt_;
  register_classes::ChnlRecircReceivedPktMutable chnl_recirc_received_pkt_;
};

class Ipb : public IpbCommon {

 public:
    Ipb(RmtObjectManager *om, int pipeIndex, int ipbIndex);
    virtual ~Ipb();


    inline bool is_chan_enabled(int chan) {
      return ((chnl_ctrl_[chan].chnl_ena() & 1) == 1);
    }
    inline bool overwrite_meta1(Packet *packet, int chan) {
      return true;
    }
    inline uint8_t get_version(Packet *packet, int chan) {
      if (!is_chan_meta_enabled(chan)) return 0;
      return (metadata_fix2_[chan].chnl_meta_fix2() >> 12) & 0x3;
    }
    inline uint16_t get_logical_port(Packet *packet, int chan) {
      if (!is_chan_meta_enabled(chan)) return 0;
      return (metadata_fix2_[chan].chnl_meta_fix2() >> 0) & 0x1FF;
    }
    inline uint64_t get_meta1(Packet *packet, int chan, int word) {
      if (!is_chan_meta_enabled(chan)) return UINT64_C(0);
      if ( !((word >= 0) && (word <= kMeta1SizeWords)) ) return UINT64_C(0);
      return (metadata_fix_[chan].chnl_meta_fix());
    }
    inline uint32_t get_packet_flags(Packet *packet) {
      return 0u;
    }
    inline uint16_t get_max_byte(Packet *packet) {
      if (!is_meta_enabled()) return 0;
      // CSR encodes number 16B chunks
      uint16_t v = glb_parser_maxbyte_.prsr_dph_max();
      RMT_ASSERT((v >= 4) && "Tof.IPB.prsr_dph_max must be >= 4");
      return v * 16;
    }

    IpbCounters *get_ipb_counters(int ipbChan);

 private:
    std::array< IpbCounters,                        kChannelsPerIpb>   ipb_counters_;
    std::array< register_classes::ChnlCtrl,         kChannelsPerIpb >  chnl_ctrl_;
    std::array< register_classes::ChnlMetadataFix,  kChannelsPerIpb >  metadata_fix_;
    std::array< register_classes::ChnlMetadataFix2, kChannelsPerIpb >  metadata_fix2_;
    register_classes::GlbParserMaxbyte                                 glb_parser_maxbyte_;
};

}
#endif // _TOFINOXX_IPB_
