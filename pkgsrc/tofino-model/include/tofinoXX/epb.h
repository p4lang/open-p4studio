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

#ifndef _TOFINOXX_EPB_
#define _TOFINOXX_EPB_

#include <epb-common.h>

// glb_ver disappeared in regs_9576_main !??!
//#include <register_includes/epb_prsr_port_regs_glb_ver_array.h>
#include <register_includes/epb_prsr_port_regs_threading_ctrl.h>
#include <register_includes/epb_prsr_port_regs_chnl_ctrl_array_mutable.h>
#include <register_includes/epb_disp_port_regs_egr_pipe_count_array_mutable.h>
#include <register_includes/epb_disp_port_regs_egr_bypass_count_array_mutable.h>

namespace MODEL_CHIP_NAMESPACE {

class Epb;

// Provides methods to access a specific EPB channel's counters
class EpbCounters {
 public:
  EpbCounters(Epb *epb, int epbChan);

  uint64_t get_egr_pipe_count();
  void set_egr_pipe_count(uint64_t val);
  void increment_egr_pipe_count();

  uint64_t get_egr_bypass_count();
  void set_egr_bypass_count(uint64_t val);
  void increment_egr_bypass_count();

 private:
  Epb *epb_;
  int epbChan_;
};


class Epb : public EpbCommon {

 public:
    Epb(RmtObjectManager *om, int pipeIndex, int parseIndex);
    virtual ~Epb();

    // Some convenience funcs in case callers want a DIY header
    inline bool is_chan_valid(int chan) {
      return (chan >= 0) && (chan <= kChannelsPerEpb);
    }
    inline bool is_chan_enabled(int chan) {
      return (is_chan_valid(chan) && ((chnl_ctrl_.chnl_ena(chan) & 0x1) != 0));
    }
    inline void set_chan_enabled(int chan, bool enabled=true) {
      if (is_chan_valid(chan)) chnl_ctrl_.chnl_ena(chan, enabled ? 1 : 0);
    }
    inline uint16_t get_ctrl_flags(int chan) {
      return (is_chan_enabled(chan)) ? chnl_ctrl_.meta_opt(chan) : 0;
    }
    inline void set_ctrl_flags(int chan, uint16_t flags) {
      if (is_chan_enabled(chan)) chnl_ctrl_.meta_opt(chan, flags);
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
    inline uint16_t get_max_byte() {
      // CSR encodes number 16B chunks
      uint16_t v = threading_ctrl_.prsr_dph_max();
      RMT_ASSERT((v >= 4) && "Tof.EPB.prsr_dph_max must be >= 4");
      // XXX/5341: In Tofino, EPB transmits dph_max + 1 words, hence the (v + 1) term
      return (v + 1) * 16;
    }


    /* Remap physical port to logical port */
    uint16_t RemapPhyToLogical(uint16_t phy_port, int chan);

    EpbCounters *get_epb_counters(int epbChan);

 private:
    //register_classes::EpbPortRegsGlbVerArray        glb_ver_;
    register_classes::EpbPrsrPortRegsThreadingCtrl              threading_ctrl_;
    register_classes::EpbPrsrPortRegsChnlCtrlArrayMutable       chnl_ctrl_;
    register_classes::EpbDispPortRegsEgrPipeCountArrayMutable   egr_pipe_count_;
    register_classes::EpbDispPortRegsEgrBypassCountArrayMutable egr_bypass_count_;
    std::array<EpbCounters, kChannelsPerEpb> epb_counters_;
    friend class EpbCounters;

};
}

#endif // _TOFINOXX_EPB_
