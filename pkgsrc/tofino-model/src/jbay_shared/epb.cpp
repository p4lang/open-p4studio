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

#include <register_adapters.h>
#include <epb.h>

namespace MODEL_CHIP_NAMESPACE {

  Epb::Epb(RmtObjectManager *om, int pipeIndex, int epbIndex)
      : EpbCommon(om, pipeIndex, epbIndex),
        epb_counters_ { {
           EpbCounters(chip_index(), pipeIndex, epbIndex, 0) ,
           EpbCounters(chip_index(), pipeIndex, epbIndex, 1) ,
           EpbCounters(chip_index(), pipeIndex, epbIndex, 2) ,
           EpbCounters(chip_index(), pipeIndex, epbIndex, 3) ,
           EpbCounters(chip_index(), pipeIndex, epbIndex, 4) ,
           EpbCounters(chip_index(), pipeIndex, epbIndex, 5) ,
           EpbCounters(chip_index(), pipeIndex, epbIndex, 6) ,
           EpbCounters(chip_index(), pipeIndex, epbIndex, 7) } },
        chnl_ctrl_ { {
          { epb_adapter(chnl_ctrl_[0], chip_index(), pipeIndex, epbIndex, 0) },
          { epb_adapter(chnl_ctrl_[1], chip_index(), pipeIndex, epbIndex, 1) },
          { epb_adapter(chnl_ctrl_[2], chip_index(), pipeIndex, epbIndex, 2) },
          { epb_adapter(chnl_ctrl_[3], chip_index(), pipeIndex, epbIndex, 3) },
          { epb_adapter(chnl_ctrl_[4], chip_index(), pipeIndex, epbIndex, 4) },
          { epb_adapter(chnl_ctrl_[5], chip_index(), pipeIndex, epbIndex, 5) },
          { epb_adapter(chnl_ctrl_[6], chip_index(), pipeIndex, epbIndex, 6) },
          { epb_adapter(chnl_ctrl_[7], chip_index(), pipeIndex, epbIndex, 7) } } },
        epb_parser_maxbyte_{ chip_index(), pipeIndex, epbIndex }
  {
    for (int i = 0; i < kChannelsPerEpb; i++) {
      epb_counters_[i].reset();
      chnl_ctrl_[i].reset();
    }
    epb_parser_maxbyte_.reset();
  }
  Epb::~Epb() {
  }

  uint16_t Epb::RemapPhyToLogical(uint16_t phy_port, int chan) {
    return phy_port;
  }

  EpbCounters* Epb::get_epb_counters(int epbChan) {
    return is_chan_valid(epbChan) ? &epb_counters_[epbChan] : nullptr;
  }

}
