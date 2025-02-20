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

#include <model_core/rmt-phv-modification.h>

#include <string>
#include <cinttypes>

namespace model_core {

    // RMT PHV MODIFICATION

    // Map strings xor|or|clr|and|XOR|OR|CLR|AND to ActionEnum tokens
    // No mapping: ActionEnum::kErr
    RmtPhvModification::ActionEnum RmtPhvModification::action_for_string(const std::string action) {
        if (action == "xor" || action == "XOR") return ActionEnum::kXor;
        if (action == "or"  || action == "OR")  return ActionEnum::kOr;
        if (action == "clr" || action == "CLR") return ActionEnum::kClr;
        if (action == "and" || action == "AND") return ActionEnum::kAnd;
        return ActionEnum::kErr;
    }

    // Map strings m|a|o|match|action|output|M|A|O|MATCH|ACTION|OUTPUT to ModifyEnum tokens
    // No mapping: ModifyEnum::kErr
    RmtPhvModification::ModifyEnum RmtPhvModification::phv_for_string(const std::string stage) {
        if (stage == "m" || stage == "match"  || stage == "M" || stage == "MATCH")  return ModifyEnum::kMatch;
        if (stage == "a" || stage == "action" || stage == "A" || stage == "ACTION") return ModifyEnum::kAction;
        if (stage == "o" || stage == "output" || stage == "O" || stage == "OUTPUT") return ModifyEnum::kOutput;
        return ModifyEnum::kErr;
    }
}
