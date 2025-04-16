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

#ifndef _SHARED_MIRROR_BUFFER_H_
#define _SHARED_MIRROR_BUFFER_H_

#include <cstdint>
#include <array>
#include <vector>
#include <mutex>
#include <model_core/timer-manager.h>
#include <rmt-log.h>
#include <rmt-defs.h>
#include <pipe-object.h>
#include <packet.h>
#include <mirror-buffer-reg.h>

namespace MODEL_CHIP_NAMESPACE {
class MirrorBuffer : public PipeObject {
    public:
        MirrorBuffer(RmtObjectManager *om=0, int pipeIndex=0);
        virtual ~MirrorBuffer() {};

        Packet  *ProcessMirrorPacket(Packet *pkt, bool ingress);
        void    ProcessCoalTimer(uint64_t tid);

        // for UT - see if any stat counters are there for this
        uint32_t active_coal_sessions()       { return active_coal_sessions_; }
        void     set_coal_tx_enable(bool tf)  { coal_tx_enable_ = tf; }

    private:
        uint16_t    CoalSessionId(uint8_t coal_idx) { return mirror_regs_.mirror_coal_idx_to_sid(coal_idx); }
        uint8_t     CoalIdx(uint16_t sid) { return mirror_regs_.mirror_coal_session_idx(sid); }
        void        CoalPktTx(uint8_t coal_idx) ;
        Packet      *CoalPktAdd(uint8_t coal_idx, Packet *pkt);
        Packet      *CoalPktStart(uint8_t coal_idx);
        Packet      *CoalPktClose(uint8_t coal_idx);
        void        MirrorPktSetI2Qmetadata(uint32_t sess_id, Packet *pkt, uint32_t version);


        // Mirroring buffer registers
        MirrorRegs              mirror_regs_;
        // coalescing timer
        model_timer::Timer      coalescing_timer_;

        bool    coal_tx_enable_;       // allow COAL timed pkt tx
        int32_t active_coal_sessions_ = 0;

        // keep a soft copy of timers (instead of modifying the registers)
        class coal_session_info {
            public:
            coal_session_info() {clear();}
            ~coal_session_info() {};
            void clear() {
                pkt = NULL;
                remaining_time = 0;
                pkt_count = 0;
            }
            Packet      *pkt;
            uint32_t    remaining_time;
            uint32_t    pkt_count;
        };

        std::mutex      coal_session_mutex_;
        std::array<coal_session_info, RmtDefs::kMirrorBufferCoalSessions>  coal_sessions_;
};

}
#endif // _SHARED_MIRROR_BUFFER_H_
