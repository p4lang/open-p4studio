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

#ifndef _SHARED_MIRROR_H_
#define _SHARED_MIRROR_H_

#include <cstdint>
#include <array>
#include <vector>
#include <mutex>

#include <model_core/timer-manager.h>
#include <rmt-log.h>
#include <rmt-defs.h>
#include <pipe-object.h>
#include <packet.h>
#include <mirror-reg.h>


namespace MODEL_CHIP_NAMESPACE {

  class Mirror : public PipeObject {

 public:
    static constexpr int kSlices              = MirrorReg::kSlices;
    static constexpr int kNormalSessions      = MirrorReg::kNormalSessions;
    static constexpr int kCoalSessions        = MirrorReg::kCoalSessions;
    static constexpr int kCoalHeaderPktCntPos = MirrorReg::kCoalHeaderPktCntPos;
    static constexpr int kCoalHeaderMinSize   = MirrorReg::kCoalHeaderMinSize;
    static constexpr int kCoalHeaderMaxSize   = MirrorReg::kCoalHeaderMaxSize;
    static constexpr int kCoalMinSize         = MirrorReg::kCoalMinSize;
    static constexpr int kCoalMaxSize         = MirrorReg::kCoalMaxSize;
    static constexpr int kCoalSliceHdrSize    = MirrorReg::kCoalSliceHdrSize;
    static constexpr int kCoalSliceBodySize   = MirrorReg::kCoalSliceBodySize;
    static constexpr int kCoalSliceSize       = MirrorReg::kCoalSliceSize;


    Mirror(RmtObjectManager *om=0, int pipeIndex=0);
    virtual ~Mirror() {};

    Packet  *ProcessMirrorPacket(Packet *pkt, bool ingress);
    void     ProcessCoalTimer(uint64_t tid);
    uint32_t active_coal_sessions()       { return active_coal_sessions_; }
    void     set_coal_tx_enable(bool tf)  { coal_tx_enable_ = tf; }
    MirrorReg *mirror_regs() { return &mirror_regs_; }

 private:
    void     CoalPktTx(int slice, int coal_num, Packet *pkt=NULL);
    Packet  *CoalPktAdd(int slice, int coal_num, Packet *pkt);
    Packet  *CoalPktStart(int slice, int coal_num, Packet *pkt=NULL);
    Packet  *CoalPktClose(int slice, int coal_num);
    void MirrorPktSetI2Qmetadata(uint32_t sess_id,
                                 Packet *pkt,
                                 uint32_t version);
    // for chip specific metadata handling...
    void SetI2QmetadataChip(Packet *pkt,
                            MirrorSessionReg &sess);

 private:
    // keep a soft copy of timers (instead of modifying the registers)
    class CoalSessInfo {
   public:
      CoalSessInfo()  { clear(); }
      ~CoalSessInfo() { }
      void clear()    { pkt = NULL; remaining_time = 0; pkt_count = 0; }
      Packet   *pkt;
      uint32_t  remaining_time;
      uint32_t  pkt_count;
    };

    MirrorReg           mirror_regs_;          // per-chip mirror registers
    model_timer::Timer  coalescing_timer_;     // coalesce timer
    int32_t             active_coal_sessions_; // counter kept for utests
    bool                coal_tx_enable_;       // allow COAL timed pkt tx
    std::mutex                                                       coal_session_mutex_;
    std::array< std::array< CoalSessInfo,kCoalSessions >, kSlices >  coal_sessions_;

};

}
#endif // _SHARED_MIRROR_H_
