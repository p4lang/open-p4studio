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

#include <phv-modification.h>

#include <string>
#include <cinttypes>
#include <phv.h>
#include <mau-object.h>
#include <model_core/rmt-phv-modification.h>

namespace MODEL_CHIP_NAMESPACE {

    // PHV MODIFICATION

    PhvModification::PhvModification(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau) : MauObject(om, pipeIndex, mauIndex, 0, mau){}
    PhvModification::~PhvModification() {}

    // Dynamically allocates a new XOR/OR/CLR Phv determined by <action>
    // Initialises all words within it to 0 and sets Phv word <index> to <value>
    int PhvModification::set_modification(model_core::RmtPhvModification::ActionEnum action, int index, uint32_t value) {
        if (index < 0 || index > Phv::kWordsMaxExtended) return -6;
        switch(action) {
            case model_core::RmtPhvModification::ActionEnum::kXor:
                set_xor_phv(index, value); return 0;
            case model_core::RmtPhvModification::ActionEnum::kOr:
                set_or_phv(index, value); return 0;
            case model_core::RmtPhvModification::ActionEnum::kClr:
                set_clr_phv(index, value); return 0;
            default:
                break;
        }
        return -5;
    }

    // Calls Phv bitwise operations for any non-null CLR/OR/XOR phv object 
    // Modifies the passed in phv
    void PhvModification::apply_modification(Phv *p, bool verbose) {
        RMT_ASSERT_NOT_NULL(p);
        if(get_clr_phv() != nullptr) p->bit_clr(get_clr_phv(), verbose);
        if(get_or_phv()  != nullptr) p->bit_or(get_or_phv(), verbose);
        if(get_xor_phv() != nullptr) p->bit_xor(get_xor_phv(), verbose);
    }
}
