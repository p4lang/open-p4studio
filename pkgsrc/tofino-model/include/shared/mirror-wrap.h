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

#ifndef _SHARED_MIRROR_WRAP_H_
#define _SHARED_MIRROR_WRAP_H_

#include <model_core/timer-manager.h>
#include <rmt-log.h>
#include <rmt-defs.h>
#include <pipe-object.h>
#include <packet.h>
#include <mirror-buffer.h>
#include <mirror.h>


namespace MODEL_CHIP_NAMESPACE {

  class MirrorWrap : public PipeObject {

 public:
    static constexpr uint8_t kFlagUseOld  = 0x1;
    static constexpr uint8_t kFlagUseNew  = 0x2;
    static constexpr uint8_t kFlagCompare = 0x4;

    MirrorWrap(RmtObjectManager *om=0, int pipeIndex=0);
    virtual ~MirrorWrap() {};

    Packet  *ProcessMirrorPacket(Packet *pkt, bool ingress);
    //void   ProcessCoalTimer(uint64_t tid);
    uint32_t active_coal_sessions();
    void     set_coal_tx_enable(bool tf);

 private:
    MirrorBuffer  mirror_old_;
    Mirror        mirror_new_;
    uint8_t       mode_;
};

}
#endif // _SHARED_MIRROR_WRAP_H_
