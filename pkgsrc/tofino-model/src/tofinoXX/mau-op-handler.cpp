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
#include <mau-op-handler.h>

namespace MODEL_CHIP_NAMESPACE {

MauOpHandler::MauOpHandler(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau)
    : MauOpHandlerCommon(om, pipeIndex, mauIndex, mau) {
}
MauOpHandler::~MauOpHandler() {
}

void MauOpHandler::instr_handle_perchip(int instr, int data_size,
                                        uint64_t data0, uint64_t data1, uint64_t T) {
  // On Tofino/TofinoB0 we *abuse* unused op[27:23] = 0x04 to mean set_meter_time
  // Also *any* instruction with op[27:21] = 0x0E is tcam_copy_word
  switch (instr >> 23) {
    case 0x3:
      switch (instr >> 21) {
        case 0xE: instr_tcam_copy_word(instr, 10, data_size, data0, data1, T); break;
      }
      break;
    case 0x4: set_meter_time(data0, data1, T); break;
  }
}

}
