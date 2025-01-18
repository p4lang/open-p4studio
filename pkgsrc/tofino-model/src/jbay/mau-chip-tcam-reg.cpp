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

#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

MauChipTcamReg::MauChipTcamReg(int chipIndex, int pipeIndex, int mauIndex,
                               int rowIndex, int colIndex, MauTcam *tcam) :
    ctor_running_(true), tcam_(tcam),
    tcam_ghost_thread_en_(default_adapter(tcam_ghost_thread_en_,
                                          chipIndex,pipeIndex,mauIndex,colIndex,rowIndex,
                                          [this](){this->tcam_ghost_thread_en_write_callback();}))
{
  tcam_ghost_thread_en_.reset();
  ctor_running_ = false;
}
MauChipTcamReg::~MauChipTcamReg() { }

bool MauChipTcamReg::get_ghost() {
  return tcam_ghost_thread_en_.tcam_ghost_thread_en();
}

// On JBay higher-numbered cols, then higher-numbered rows are higher priority
// This corresponds to tcam_index_ value
int MauChipTcamReg::get_priority() {
  return tcam_->tcam_index();
}

void MauChipTcamReg::tcam_ghost_thread_en_write_callback() {
  if (ctor_running_) return;
  tcam_->mau()->tcam_config_changed();
}

}
