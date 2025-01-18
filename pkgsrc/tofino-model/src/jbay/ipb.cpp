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

// IPB - Jbay specific code

#include <register_adapters.h>
#include <ipb.h>
#include <common/rmt-util.h>

namespace MODEL_CHIP_NAMESPACE {

using namespace model_common;

IpbCounters::IpbCounters(int chipIndex, int pipeIndex, int ipbIndex, int chanIndex) :
  IpbCountersCommon(chipIndex, pipeIndex, ipbIndex, chanIndex),
  chnl_resubmit_discard_pkt_(
      ipb_adapter(chnl_resubmit_discard_pkt_, chipIndex, pipeIndex, ipbIndex, chanIndex)),
  chnl_resubmit_received_pkt_(
      ipb_adapter(chnl_resubmit_received_pkt_, chipIndex, pipeIndex, ipbIndex, chanIndex))
   {
  // 36 bit wide counters
  wrap_counter_size_ = 36;
 }

void IpbCounters::reset() {
  IpbCountersCommon::reset();
  chnl_resubmit_discard_pkt_.reset();
  chnl_resubmit_received_pkt_.reset();
}

void IpbCounters::increment_chnl_parser_send_pkt_err() {
  chnl_parser_send_pkt_.err_pkt(
      Util::increment_and_wrap(chnl_parser_send_pkt_.err_pkt(),
                               wrap_counter_size_));
}

void IpbCounters::increment_chnl_resubmit_received_pkt() {
  chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt(
      Util::increment_and_wrap(chnl_resubmit_received_pkt_.chnl_resubmit_received_pkt(),
                               wrap_counter_size_)
  );
}

Ipb::Ipb(RmtObjectManager *om, int pipeIndex, int ipbIndex)
    : IpbCommon(om, pipeIndex, ipbIndex),
      ipb_counters_ { {
         IpbCounters(chip_index(), pipeIndex, ipbIndex, 0) ,
         IpbCounters(chip_index(), pipeIndex, ipbIndex, 1) ,
         IpbCounters(chip_index(), pipeIndex, ipbIndex, 2) ,
         IpbCounters(chip_index(), pipeIndex, ipbIndex, 3) ,
         IpbCounters(chip_index(), pipeIndex, ipbIndex, 4) ,
         IpbCounters(chip_index(), pipeIndex, ipbIndex, 5) ,
         IpbCounters(chip_index(), pipeIndex, ipbIndex, 6) ,
         IpbCounters(chip_index(), pipeIndex, ipbIndex, 7) } },
      port_en_(default_adapter(port_en_, chip_index(), pipeIndex, ipbIndex)),
      chnl_ctrl_ { {
          { ipb_adapter(chnl_ctrl_[0], chip_index(), pipeIndex, ipbIndex, 0) },
          { ipb_adapter(chnl_ctrl_[1], chip_index(), pipeIndex, ipbIndex, 1) },
          { ipb_adapter(chnl_ctrl_[2], chip_index(), pipeIndex, ipbIndex, 2) },
          { ipb_adapter(chnl_ctrl_[3], chip_index(), pipeIndex, ipbIndex, 3) },
          { ipb_adapter(chnl_ctrl_[4], chip_index(), pipeIndex, ipbIndex, 4) },
          { ipb_adapter(chnl_ctrl_[5], chip_index(), pipeIndex, ipbIndex, 5) },
          { ipb_adapter(chnl_ctrl_[6], chip_index(), pipeIndex, ipbIndex, 6) },
          { ipb_adapter(chnl_ctrl_[7], chip_index(), pipeIndex, ipbIndex, 7) } } },
      chnl_meta_ { {
          { ipb_adapter(chnl_meta_[0], chip_index(), pipeIndex, ipbIndex, 0) },
          { ipb_adapter(chnl_meta_[1], chip_index(), pipeIndex, ipbIndex, 1) },
          { ipb_adapter(chnl_meta_[2], chip_index(), pipeIndex, ipbIndex, 2) },
          { ipb_adapter(chnl_meta_[3], chip_index(), pipeIndex, ipbIndex, 3) },
          { ipb_adapter(chnl_meta_[4], chip_index(), pipeIndex, ipbIndex, 4) },
          { ipb_adapter(chnl_meta_[5], chip_index(), pipeIndex, ipbIndex, 5) },
          { ipb_adapter(chnl_meta_[6], chip_index(), pipeIndex, ipbIndex, 6) },
          { ipb_adapter(chnl_meta_[7], chip_index(), pipeIndex, ipbIndex, 7) } } },
      glb_parser_maxbyte_{ chip_index(), pipeIndex, ipbIndex }

  {
  port_en_.reset();
  for (int i = 0; i < kChannelsPerIpb; i++) {
    ipb_counters_[i].reset();
    chnl_ctrl_[i].reset();
    chnl_meta_[i].reset();
  }
  glb_parser_maxbyte_.reset();
}
Ipb::~Ipb() {
}

IpbCounters* Ipb::get_ipb_counters(int ipbChan) {
  return is_chan_valid(ipbChan) ? &ipb_counters_[ipbChan] : nullptr;
}

}
