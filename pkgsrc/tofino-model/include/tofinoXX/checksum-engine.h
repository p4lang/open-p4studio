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

#ifndef _TOFINOXX_CHECKSUM_ENGINE_
#define _TOFINOXX_CHECKSUM_ENGINE_

#include <checksum-engine-shared.h>

namespace MODEL_CHIP_NAMESPACE {

  class ChecksumEngine : public ChecksumEngineShared {

 public:
    // Define a static accessor to simplify calling register constructor in checksum-engine-shared.cpp CTOR
    static enum memory_classes::PrsrPoCsumCtrlRowArrayMutable::PrsrMemMainRspecEnum
        get_csum_r01234(int i) {
      switch (i) {
        case 0: return memory_classes::PrsrPoCsumCtrlRowArrayMutable::PrsrMemMainRspecEnum::kPoCsumCtrl_0Row;
        case 1: return memory_classes::PrsrPoCsumCtrlRowArrayMutable::PrsrMemMainRspecEnum::kPoCsumCtrl_1Row;
        default: RMT_ASSERT(0);
      }
    }

    ChecksumEngine(RmtObjectManager *om, int pipeIndex, int ioIndex, int prsIndex,
                   int engineIndex, int ramIndex, Parser *parser);
    virtual ~ChecksumEngine();

  };

}

#endif // _TOFINOXX_CHECKSUM_ENGINE_
