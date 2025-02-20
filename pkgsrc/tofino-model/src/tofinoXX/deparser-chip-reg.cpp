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

#include <deparser-chip-reg.h>
#include <register_adapters.h>

// Deparser registers that are specific to tofino

namespace MODEL_CHIP_NAMESPACE {

// Registers must use the correct adapter:
//  deparser_in_hdr_adapter() for registers in the Input and Header phase
//  deparser_out_adapter() for registers in the Output phase (not shared)

DeparserChipReg::DeparserChipReg(int chip, int pipe) :
    ipipe_remap_{deparser_out_adapter(ipipe_remap_, chip, pipe )},
    epipe_remap_{deparser_out_adapter(epipe_remap_, chip, pipe )}
{
  Reset();
}

DeparserChipReg::~DeparserChipReg() {
}

void DeparserChipReg::Reset() {
    ipipe_remap_.reset();
    epipe_remap_.reset();
}

}
