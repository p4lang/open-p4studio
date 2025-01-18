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

#include "egress-buf.h"

#include "common/ebuf_reg_array.h"
#include "common/rmt-assert.h"
#include "register_includes/ebuf100_chnl_pktnum_mutable.h"
#include "register_includes/ebuf400_chnl_pktnum_mutable.h"

namespace MODEL_CHIP_NAMESPACE {

EgressBuf::EgressBuf(RmtObjectManager *om, int pipeIndex) :
  PipeObject(om, pipeIndex),
  ebuf100_chan_groups_(model_common::EBufRegArray<
                          EgressBufChanGroupImpl<register_classes::Ebuf100ChnlPktnumMutable>,
                          RmtDefs::kEgressBufSlicesPerPipe,
                          RmtDefs::kEgressBufEbuf100PerSlice,
                          RmtDefs::kEgressBufChannelsPerEbuf100>
                       ::create(chip_index(), pipeIndex)),
  ebuf400_chan_groups_(model_common::EBufRegArray<
                          EgressBufChanGroupImpl<register_classes::Ebuf400ChnlPktnumMutable>,
                          RmtDefs::kEgressBufSlicesPerPipe,
                          RmtDefs::kEgressBufEbuf400PerSlice,
                          RmtDefs::kEgressBufChannelsPerEbuf400>
                       ::create(chip_index(), pipeIndex)) {
  reset();
}

void EgressBuf::reset() {
  for (int i = 0; i < kEbuf400ChannelsPerPipe; i++) {
    ebuf400_chan_groups_[i].reset();
  }
  for (int i = 0; i < kEbuf100ChannelsPerPipe; i++) {
    ebuf100_chan_groups_[i].reset();
  }
}

EgressBufChanGroup *EgressBuf::get_chan_group(int portIndex) {
  // make sure we have pipe relative port index
  int chanIndex = get_local_port_index(portIndex);
  // klocwork thinks this method always returns nullptr so use GLOBAL_NULLPTR
  // to mitigate for that
  EgressBufChanGroup *chan_group = static_cast<EgressBufChanGroup*>(GLOBAL_NULLPTR);
  if (chanIndex < kEbuf100ChannelsPerPipe) {
    chan_group = &ebuf100_chan_groups_[chanIndex];
  } else {
    chanIndex -= kEbuf100ChannelsPerPipe;
    if (chanIndex < kEbuf400ChannelsPerPipe) {
      chan_group = &ebuf400_chan_groups_[chanIndex];
    }
  }
  return chan_group;
}

void EgressBuf::increment_dprsr_rcv_pkt(int portIndex) {
  EgressBufChanGroup *chan_group = get_chan_group(portIndex);
  RMT_ASSERT_NOT_NULL(chan_group);
  chan_group->increment_dprsr_rcv_pkt();
}

void EgressBuf::increment_warp_rcv_pkt(int portIndex) {
  EgressBufChanGroup *chan_group = get_chan_group(portIndex);
  RMT_ASSERT_NOT_NULL(chan_group);
  chan_group->increment_warp_rcv_pkt();
}

void EgressBuf::increment_mac_xmt_pkt(int portIndex) {
  EgressBufChanGroup *chan_group = get_chan_group(portIndex);
  RMT_ASSERT_NOT_NULL(chan_group);
  chan_group->increment_mac_xmt_pkt();
}

}  // namespace MODEL_CHIP_NAMESPACE
