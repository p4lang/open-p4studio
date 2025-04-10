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

// EPB - TofinoXX specific code

#include <register_adapters.h>
#include <port.h>
#include <epb.h>
#include <common/rmt-util.h>

namespace MODEL_CHIP_NAMESPACE {

using namespace model_common;

EpbCounters::EpbCounters(Epb *epb, int epbChan) :
  epb_(epb), epbChan_(epbChan) {}

uint64_t EpbCounters::get_egr_pipe_count() {
  return epb_->egr_pipe_count_.egr_pipe_cnt(epbChan_);
};

void EpbCounters::set_egr_pipe_count(uint64_t val) {
  epb_->egr_pipe_count_.egr_pipe_cnt(epbChan_, val);
};

void EpbCounters::increment_egr_pipe_count() {
  set_egr_pipe_count(
      Util::increment_and_wrap(get_egr_pipe_count(), 64)
  );
}

uint64_t EpbCounters::get_egr_bypass_count() {
  return epb_->egr_bypass_count_.egr_byp_cnt(epbChan_);
};

void EpbCounters::set_egr_bypass_count(uint64_t val) {
  epb_->egr_bypass_count_.egr_byp_cnt(epbChan_, val);
};

void EpbCounters::increment_egr_bypass_count() {
  set_egr_bypass_count(
      Util::increment_and_wrap(get_egr_bypass_count(), 64)
  );
}


  Epb::Epb(RmtObjectManager *om, int pipeIndex, int epbIndex)
      : EpbCommon(om, pipeIndex, epbIndex),
        // glb_ver_(chip_index(),pipeIndex,epbIndex),
        threading_ctrl_(epb_adapter(threading_ctrl_,chip_index(),pipeIndex,epbIndex)),
        chnl_ctrl_(epb_adapter(chnl_ctrl_,chip_index(),pipeIndex,epbIndex)),
        egr_pipe_count_(epb_adapter(egr_pipe_count_, chip_index(), pipeIndex, epbIndex)),
        egr_bypass_count_(epb_adapter(egr_bypass_count_, chip_index(), pipeIndex, epbIndex)),
        epb_counters_ { {
           EpbCounters(this, 0) ,
           EpbCounters(this, 1) ,
           EpbCounters(this, 2) ,
           EpbCounters(this, 3) } } {

    //glb_ver_.reset(); // glb_ver disappeared in regs_9576_main !??!
    threading_ctrl_.reset();
    chnl_ctrl_.reset();
    egr_pipe_count_.reset();
    egr_bypass_count_.reset();
  }
  Epb::~Epb() {
  }

  uint16_t Epb::RemapPhyToLogical(uint16_t phy_port, int chan) {
    uint16_t pipeid_ovr = chnl_ctrl_.pipeid_ovr(chan);
    /* is pipe-id override enabled */
    if (!((pipeid_ovr >> 2) & 0x1)) {
      return phy_port;
    }
    uint16_t log_pipe = pipeid_ovr & 0x3;
    uint16_t phy_pipe = Port::get_pipe_num(phy_port);

    if (phy_pipe == log_pipe) {
      return phy_port;
    }
    uint16_t log_port = Port::make_port_index(log_pipe, phy_port);
    return log_port;
  }

  EpbCounters* Epb::get_epb_counters(int epbChan) {
    return is_chan_valid(epbChan) ? &epb_counters_[epbChan] : nullptr;
  }

}
