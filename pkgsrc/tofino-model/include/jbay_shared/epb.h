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

#ifndef _JBAY_SHARED_EPB_
#define _JBAY_SHARED_EPB_

#include <epb-common.h>
#include <epb-counters.h>

#include <register_includes/epb_chnl_ctrl_mutable.h>
#include <register_includes/epb_parser_maxbyte.h>

namespace MODEL_CHIP_NAMESPACE {

class Epb : public EpbCommon {

 public:
    Epb(RmtObjectManager *om, int pipeIndex, int parseIndex);
    virtual ~Epb();

    // Some convenience funcs in case callers want a DIY header
    inline bool is_chan_valid(int chan) {
      return ((chan >= 0) && (chan < kChannelsPerEpb));
    }
    inline bool is_chan_enabled(int chan) {
      //return ((chan >= 0) && (chan <= 3) && ((chnl_ctrl_.chnl_ena(chan) & 0x1) != 0));
      // No more chnl_ena as of regs_43323_parde_jbay
      return is_chan_valid(chan);
    }
    inline void set_chan_enabled(int chan, bool enabled=true) {
      // no-op for jbay, chans always enabled
    }
    inline uint16_t get_ctrl_flags(int chan) {
      return (is_chan_enabled(chan)) ? chnl_ctrl_[chan].meta_opt() : 0;
    }
    inline void set_ctrl_flags(int chan, uint16_t flags) {
      if (is_chan_enabled(chan)) chnl_ctrl_[chan].meta_opt(flags);
    }
    inline uint32_t get_global_version(int chan) {
      //return (is_chan_enabled(chan)) ? glb_ver_.glb_ver(chan) : 0u;
      return 0u;
    }
    inline bool is_ctrl_flag_set(int chan, uint16_t flag) {
      return ((get_ctrl_flags(chan) & flag) != 0);
    }
    inline uint16_t get_packet_len(Packet *pkt) {
      return pkt->orig_ingress_pkt_len();
    }
    inline uint16_t get_padding(int metadata_size) {
      return metadata_size < 8            ? 8 - metadata_size
             : ((metadata_size % 4) == 0) ? 0
                                          : 4 - (metadata_size % 4);
    }
    inline uint16_t get_max_byte() {
      // CSR encodes number 16B chunks
      uint16_t v = epb_parser_maxbyte_.prsr_dph_max();
      RMT_ASSERT((v >= 4) && "JBayCB.EPB.prsr_dph_max must be >= 4");
      // NB. XXX, says driver should always program this to 1023
      //     which results in a returned max_byte value of 16368
      //     so suppress asserts below for JBay (also for WIP as code shared)
      // RMT_ASSERT(((v & 1) == 0) && "JBayCB.EPB.prsr_dph_max must be even");
      // RMT_ASSERT((v <= 32) && "JBayCB.EPB.prsr_dph_max must be <= 32");
      return v * 16;
    }

    /* Remap physical port to logical port */
    uint16_t RemapPhyToLogical(uint16_t phy_port, int chan);

    EpbCounters* get_epb_counters(int epbChan);


 private:
    std::array< EpbCounters, kChannelsPerEpb >                           epb_counters_;
    std::array< register_classes::EpbChnlCtrlMutable, kChannelsPerEpb >  chnl_ctrl_;
    register_classes::EpbParserMaxbyte                                   epb_parser_maxbyte_;
};

}

#endif // _JBAY_EPB_
