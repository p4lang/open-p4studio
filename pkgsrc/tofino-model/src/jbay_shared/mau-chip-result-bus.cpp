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
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-op-handler.h>
#include <mau-result-bus.h>


namespace MODEL_CHIP_NAMESPACE {

void MauChipResultBus::atomic_mod_sram_go_rd_callback(int ie) {
  RMT_ASSERT((ie == 0) || (ie == 1));
}
void MauChipResultBus::atomic_mod_sram_go_wr_callback(int ie) {
  RMT_ASSERT((ie == 0) || (ie == 1));
  bool ingress = (ie == 0);
  MauOpHandler *op_handler = mau_->mau_op_handler();
  op_handler->atomic_mod_sram(ingress, ie, UINT64_C(0), UINT64_C(0), UINT64_C(0));
}

void MauChipResultBus::atomic_mod_tcam_go_rd_callback(int ie) {
  RMT_ASSERT((ie == 0) || (ie == 1));
}
void MauChipResultBus::atomic_mod_tcam_go_wr_callback(int ie) {
  RMT_ASSERT((ie == 0) || (ie == 1));
  MauOpHandler *op_handler = mau_->mau_op_handler();
  // TODO: XXX: pass gress?
  //bool ingress = (ie == 0);
  op_handler->flush_all_tcam_writeregs();
}

}
