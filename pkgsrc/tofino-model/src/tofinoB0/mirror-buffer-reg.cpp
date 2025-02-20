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

// MIRROR-BUFFER-REG - TofinoB0 specific code

#include <mirror-buffer-reg.h>

namespace tofinoB0 {

MirrorRegs::MirrorRegs(int chip, int pipe, MirrorBuffer *mb)
    : MirrorRegsCommon(chip, pipe, mb),
      min_bcnt_(deparser_out_adapter(min_bcnt_,chip, pipe)) {
  
  min_bcnt_.reset();
}
MirrorRegs::~MirrorRegs() {
}




} 
