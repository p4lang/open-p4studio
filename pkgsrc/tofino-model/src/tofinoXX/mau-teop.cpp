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

// MauTeop - Tofino/TofinoB0 code
// In shared/ because identical across these chips

#include <mau.h>
#include <register_adapters.h>
#include <mau-lookup-result.h>
#include <mau-teop.h>

namespace MODEL_CHIP_NAMESPACE {

MauTeop::MauTeop(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau)
    : MauTeopCommon(om, pipeIndex, mauIndex, mau), mau_(mau) {
}
MauTeop::~MauTeop() {
}


}
