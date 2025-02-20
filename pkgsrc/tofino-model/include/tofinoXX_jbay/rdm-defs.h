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

#ifndef __RDM_DEFS__
#define __RDM_DEFS__
#include <mem_utils.h>

namespace MODEL_CHIP_NAMESPACE {

class RdmDefs {
 public:
    static constexpr int kRdmWidth = 80;
    static constexpr int kRdmDepth = BFN_MEM_TM_PRE_RDM_SZ;
    static constexpr int kRdmNodeCount = 2*kRdmDepth;

    static constexpr int kType1Shift = 0;
    static constexpr int kType1Width = 4;
    static constexpr int kType2Shift = 40;
    static constexpr int kType2Width = 4;

    static constexpr int kL1RidL1NextShift = 4;
    static constexpr int kL1RidL1NextWidth = 20;
    static constexpr int kL1RidL2NextShift = 24;
    static constexpr int kL1RidL2NextWidth = 20;
    static constexpr int kL1RidRidShift    = 48;
    static constexpr int kL1RidRidWidth    = 16;
    static constexpr int kL1RidXidShift    = 64;
    static constexpr int kL1RidXidWidth    = 16;
    static constexpr int kL1RidEndL2NextShift = 4;
    static constexpr int kL1RidEndL2NextWidth = 20;
    static constexpr int kL1RidEndRidShift = 24;
    static constexpr int kL1RidEndRidWidth = 16;

    static constexpr int kL1EcmpPtrL1NextShift  = 4;
    static constexpr int kL1EcmpPtrL1NextWidth  = 20;
    static constexpr int kL1EcmpPtrVector0Shift = 24;
    static constexpr int kL1EcmpPtrVector0Width = 20;
    static constexpr int kL1EcmpPtrVector1Shift = 44;
    static constexpr int kL1EcmpPtrVector1Width = 20;
    static constexpr int kL1EcmpPtrXidShift     = 64;
    static constexpr int kL1EcmpPtrXidWidth     = 16;

    static constexpr int kL1EcmpVecL1BaseShift  = 4;
    static constexpr int kL1EcmpVecL1BaseWidth  = 20;
    static constexpr int kL1EcmpVecLengthShift  = 24;
    static constexpr int kL1EcmpVecLengthWidth  = 5;
    static constexpr int kL1EcmpVecVectorShift  = 32;
    static constexpr int kL1EcmpVecVectorWidth  = 32;

    static constexpr int kL2Port16PipeShift  = 4;
    static constexpr int kL2Port16PipeWidth  = 2;
    static constexpr int kL2Port16LastShift  = 7;
    static constexpr int kL2Port16LastWidth  = 1;
    static constexpr int kL2Port16SpvShift   = 24;
    static constexpr int kL2Port16SpvWidth   = 2;
    static constexpr int kL2Port16PortsShift = 8;
    static constexpr int kL2Port16PortsWidth = 16;

    static constexpr int kL2Port64PipeShift  = 4;
    static constexpr int kL2Port64PipeWidth  = 2;
    static constexpr int kL2Port64LastShift  = 7;
    static constexpr int kL2Port64LastWidth  = 1;
    static constexpr int kL2Port64SpvShift   = 72;
    static constexpr int kL2Port64SpvWidth   = 8;
    static constexpr int kL2Port64PortsShift = 8;
    static constexpr int kL2Port64PortsWidth = 64;

    static constexpr int kL2LagL2NextShift = 4;
    static constexpr int kL2LagL2NextWidth = 20;
    static constexpr int kL2LagLagIdShift  = 24;
    static constexpr int kL2LagLagIdWidth  = 8;

    enum class RdmNodeType {
      kReset=0,
      kInvalid=0,
      kL1Rid=1,
      kL1RidXid=2,
      kL1RidEnd=3,
      kL1EcmpPtr=5,
      kL1EcmpPtrXid=6,
      kL1EcmpVector=4,
      kL2Port16=8,
      kL2Port64=9,
      kL2Lag=12,
    };
};

}

#endif

