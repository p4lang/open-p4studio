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

#ifndef _JBAY_SHARED_MIRROR_BUFFER_REG_
#define _JBAY_SHARED_MIRROR_BUFFER_REG_

#include <mirror-buffer-reg-common.h>
//#include <register_adapters.h>
//#include <register_includes/mir_buf_regs_min_bcnt.h>

namespace MODEL_CHIP_NAMESPACE {

  class MirrorRegs : public MirrorRegsCommon {
 public:
    MirrorRegs(int chip=0, int pipe=0, MirrorBuffer *mb=NULL);
    virtual ~MirrorRegs();

    // not sure if this in the JBay CSRs yet
    //inline uint8_t min_bcnt() { return min_bcnt_.min_bcnt(); }
    inline uint8_t min_bcnt() { return 0; }

 private:
    //register_classes::MirBufRegsMinBcnt  min_bcnt_;
  };
}

#endif // _JBAY_SHARED_MIRROR_BUFFER_REG_
