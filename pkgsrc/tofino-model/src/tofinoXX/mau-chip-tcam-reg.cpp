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

#include <mau.h>
#include <mau-tcam.h>
#include <mau-chip-tcam-reg.h>

namespace MODEL_CHIP_NAMESPACE {

MauChipTcamReg::MauChipTcamReg(int chipIndex, int pipeIndex, int mauIndex,
                               int rowIndex, int colIndex, MauTcam *tcam) : tcam_(tcam) {
}
MauChipTcamReg::~MauChipTcamReg() { }

// On Tofino higher-numbered cols, then higher-numbered rows are higher priority
// This corresponds to tcam_index_ value
// NB. Prior to TCAM CRB change was: (get_vpn() << kTcamIndexWidth) | tcam_->tcam_index();
int MauChipTcamReg::get_priority() {
  return tcam_->tcam_index();
}

}
