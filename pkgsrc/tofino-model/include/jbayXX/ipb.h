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

#ifndef _JBAYXX_IPB_
#define _JBAYXX_IPB_

#include <ipb-common.h>

#include <register_includes/glb_group_port_en.h>
#include <register_includes/chnl_ctrl.h>
#include <register_includes/chnl_meta.h>
#include <register_includes/chnl_resubmit_discard_pkt.h>
#include <register_includes/chnl_resubmit_received_pkt_mutable.h>
#include <register_includes/glb_parser_maxbyte.h>

namespace MODEL_CHIP_NAMESPACE {

class IpbCounters : public IpbCountersCommon {
 public:
  IpbCounters(int chipIndex, int pipeIndex, int ipbIndex, int chanIndex);
  void reset() override;
  virtual void increment_chnl_parser_send_pkt_err() override;
  virtual void increment_chnl_resubmit_received_pkt() override;

  register_classes::ChnlResubmitDiscardPkt  chnl_resubmit_discard_pkt_;
  register_classes::ChnlResubmitReceivedPktMutable chnl_resubmit_received_pkt_;
};

class Ipb : public IpbCommon {

    // Which Pkt flags get passed to Ipb::prepend_metadata_hdr
    // (individual flag defs in ipb-common.h)
    static constexpr uint32_t kFlagsPktFromPktGen = 0x00000007;
    static constexpr uint32_t kFlagsPktFromRecirc = 0x00000000;
    static constexpr uint32_t kFlagsPktFromOther  = 0x00000000;

 public:
    Ipb(RmtObjectManager *om, int pipeIndex, int ipbIndex);
    virtual ~Ipb();


    inline bool is_chan_enabled(int chan) {
      return (((port_en_.enbl() >> chan) & 1) == 1);
    }
    inline bool overwrite_meta1(Packet *packet, int chan) {
      // XXX: overwrite_meta1 is ignored for PktGen packets on JBay
      if (packet->is_generated()) return false;
      // Programmable on JBay, always false on other chips
      return (is_chan_meta_enabled(chan)) ?chnl_ctrl_[chan].overwrite_meta1() :false;;
    }
    inline uint8_t get_version(Packet *packet, int chan) {
      return (is_chan_meta_enabled(chan)) ?chnl_ctrl_[chan].version() :0;
    }
    inline uint16_t get_logical_port(Packet *packet, int chan) {
      return (is_chan_meta_enabled(chan)) ?chnl_ctrl_[chan].ingress_port() :0;
    }
    inline uint64_t get_meta1(Packet *packet, int chan, int word) {
      if (!is_chan_meta_enabled(chan)) return UINT64_C(0);
      if ( !((word >= 0) && (word < kMeta1SizeWords)) ) return UINT64_C(0);
      static_assert( kHeaderWordSizeBytes==8, "get_meta1 assumes kHeaderWordSizeBytes=8");
      static_assert( kMeta1SizeWords==2,      "get_meta1 assumes kMeta1SizeWords=2");
      if (word==0) {
        uint64_t v = chnl_meta_[chan].meta1_1();
        return (v<<32) | chnl_meta_[chan].meta1_0();
      } else {
        uint64_t v = chnl_meta_[chan].meta1_3();
        return (v<<32) | chnl_meta_[chan].meta1_2();
      }
    }
    inline uint32_t get_packet_flags(Packet *packet) {
      return (packet->is_generated()) ?kFlagsPktFromPktGen :kFlagsPktFromOther;
    }
    inline uint16_t get_max_byte(Packet *packet) {
      if (!is_meta_enabled()) return 0;
      // CSR encodes number 16B chunks
      uint16_t v = glb_parser_maxbyte_.prsr_dph_max();
      RMT_ASSERT((v >= 4) && "JBay.IPB.prsr_dph_max must be >= 4");
      // NB. XXX, says driver should always program this to 1023
      //     which results in a returned max_byte value of 16368
      //     so suppress asserts below
      // RMT_ASSERT(((v & 1) == 0) && "JBay.IPB.prsr_dph_max must be even");
      // RMT_ASSERT((v <= 32) && "JBay.IPB.prsr_dph_max must be <= 32");
      return v * 16;
    }

    IpbCounters *get_ipb_counters(int ipbChan);

 private:
    std::array< IpbCounters,                kChannelsPerIpb>   ipb_counters_;
    register_classes::GlbGroupPortEn                           port_en_;
    std::array< register_classes::ChnlCtrl, kChannelsPerIpb >  chnl_ctrl_;
    std::array< register_classes::ChnlMeta, kChannelsPerIpb >  chnl_meta_;
    register_classes::GlbParserMaxbyte                         glb_parser_maxbyte_;
};

}
#endif // _JBAYXX_IPB_
