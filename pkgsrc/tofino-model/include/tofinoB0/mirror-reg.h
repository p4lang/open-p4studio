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

// MirrorReg - TofinoB0
// Separate implementation because of min_bcnt CSR

#ifndef _TOFINOB0_MIRROR_REG_
#define _TOFINOB0_MIRROR_REG_

#include <mirror-reg-tofinoXX.h>
#include <register_includes/mir_buf_regs_min_bcnt.h>

namespace tofinoB0 {

  class MirrorReg: public MirrorRegTofinoXX {

 public:
    MirrorReg(int chipIndex, int pipeIndex)
      : MirrorRegTofinoXX(chipIndex,pipeIndex),
        min_bcnt_(deparser_out_adapter(min_bcnt_,chipIndex,pipeIndex))
    {
      min_bcnt_.reset();
    }
    virtual ~MirrorReg() { }

    // COALESCING session CSRs - S is sliceID which is unused on TofinoB0
    inline uint16_t coal_abs_min_size(int S, int k)  {
      return static_cast<uint16_t>(min_bcnt_.min_bcnt());
    }


 private:
    register_classes::MirBufRegsMinBcnt   min_bcnt_;
  };

}
#endif // _TOFINOB0_MIRROR_REG_
