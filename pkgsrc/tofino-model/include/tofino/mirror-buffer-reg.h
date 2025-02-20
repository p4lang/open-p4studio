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

#ifndef _TOFINO_MIRROR_BUFFER_REG_
#define _TOFINO_MIRROR_BUFFER_REG_

#include <mirror-buffer-reg-common.h>

namespace tofino {

  class MirrorRegs : public MirrorRegsCommon {    
 public:
    MirrorRegs(int chip=0, int pipe=0, MirrorBuffer *mb=NULL);
    virtual ~MirrorRegs();

    inline uint8_t min_bcnt() { return 0; }
  };
}

#endif // _TOFINO_MIRROR_BUFFER_REG_
