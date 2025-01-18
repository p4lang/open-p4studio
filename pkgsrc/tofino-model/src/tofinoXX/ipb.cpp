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

// IPB - TofinoXX specific code

#include <register_adapters.h>
#include <ipb.h>
#include <common/rmt-util.h>

namespace MODEL_CHIP_NAMESPACE {

using namespace model_common;

IpbCounters::IpbCounters(int chipIndex, int pipeIndex, int ipbIndex, int chanIndex) :
  IpbCountersCommon(chipIndex, pipeIndex, ipbIndex, chanIndex),
  chnl_recirc_discard_pkt_(
      ipb_adapter(chnl_recirc_discard_pkt_, chipIndex, pipeIndex, ipbIndex, chanIndex)),
  chnl_recirc_received_pkt_(
      ipb_adapter(chnl_recirc_received_pkt_, chipIndex, pipeIndex, ipbIndex, chanIndex))
   { }

void IpbCounters::reset() {
  IpbCountersCommon::reset();
  chnl_recirc_discard_pkt_.reset();
  chnl_recirc_received_pkt_.reset();
}

void IpbCounters::increment_chnl_deparser_drop_pkt() {
  // override default implementation with 32 bit counter
  chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt(
      Util::increment_and_wrap(chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt(),
                               32));
}

void IpbCounters::increment_chnl_resubmit_received_pkt() {
  // XXX: chnl_recirc_received_pkt in Tofino IPB does in fact count
  // *resubmit* packets as per chnl_resubmit_received_pkt in JBay IPB.
  chnl_recirc_received_pkt_.chnl_recirc_received_pkt(
      Util::increment_and_wrap(chnl_recirc_received_pkt_.chnl_recirc_received_pkt(),
                               64)
  );
}

Ipb::Ipb(RmtObjectManager *om, int pipeIndex, int ipbIndex)
    : IpbCommon(om, pipeIndex, ipbIndex),
      ipb_counters_ { {
         IpbCounters(chip_index(), pipeIndex, ipbIndex, 0) ,
         IpbCounters(chip_index(), pipeIndex, ipbIndex, 1) ,
         IpbCounters(chip_index(), pipeIndex, ipbIndex, 2) ,
         IpbCounters(chip_index(), pipeIndex, ipbIndex, 3) } },
      chnl_ctrl_ { {
          { ipb_adapter(chnl_ctrl_[0], chip_index(), pipeIndex, ipbIndex, 0) },
          { ipb_adapter(chnl_ctrl_[1], chip_index(), pipeIndex, ipbIndex, 1) },
          { ipb_adapter(chnl_ctrl_[2], chip_index(), pipeIndex, ipbIndex, 2) },
          { ipb_adapter(chnl_ctrl_[3], chip_index(), pipeIndex, ipbIndex, 3) } } },
      metadata_fix_ { {
          { ipb_adapter(metadata_fix_[0], chip_index(), pipeIndex, ipbIndex, 0) },
          { ipb_adapter(metadata_fix_[1], chip_index(), pipeIndex, ipbIndex, 1) },
          { ipb_adapter(metadata_fix_[2], chip_index(), pipeIndex, ipbIndex, 2) },
          { ipb_adapter(metadata_fix_[3], chip_index(), pipeIndex, ipbIndex, 3) } } },
      metadata_fix2_ { {
          { ipb_adapter(metadata_fix2_[0], chip_index(), pipeIndex, ipbIndex, 0) },
          { ipb_adapter(metadata_fix2_[1], chip_index(), pipeIndex, ipbIndex, 1) },
          { ipb_adapter(metadata_fix2_[2], chip_index(), pipeIndex, ipbIndex, 2) },
          { ipb_adapter(metadata_fix2_[3], chip_index(), pipeIndex, ipbIndex, 3) } } },
      glb_parser_maxbyte_{ chip_index(), pipeIndex, ipbIndex }
{
  for (int i = 0; i < kChannelsPerIpb; i++) {
    ipb_counters_[i].reset();
    chnl_ctrl_[i].reset();
    metadata_fix_[i].reset();
    metadata_fix2_[i].reset();
  }
  glb_parser_maxbyte_.reset();
}
Ipb::~Ipb() {
}

IpbCounters* Ipb::get_ipb_counters(int ipbChan) {
  return is_chan_valid(ipbChan) ? &ipb_counters_[ipbChan] : nullptr;
}


}
