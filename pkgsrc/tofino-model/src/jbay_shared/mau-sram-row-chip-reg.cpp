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
#include <mau-sram.h>
#include <mau-sram-row-chip-reg.h>

namespace MODEL_CHIP_NAMESPACE {

MauSramRowChipReg::MauSramRowChipReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                                     int rowIndex, MauSramRow *mauSramRow) :
    MauObject(om, pipeIndex, mauIndex, kType, rowIndex), mau_sram_row_(mauSramRow),
    synth2port_vpn_ctl_(default_adapter(synth2port_vpn_ctl_,chip_index(),pipeIndex,mauIndex,rowIndex)),
    atomic_mod_shadow_ram_status_l_(default_adapter(atomic_mod_shadow_ram_status_l_,chip_index(),pipeIndex,mauIndex,rowIndex,
                                                    nullptr, [this,rowIndex](uint32_t i){this->shadow_ram_status_rd_cb(rowIndex,i + 0);})),
    atomic_mod_shadow_ram_status_r_(default_adapter(atomic_mod_shadow_ram_status_r_,chip_index(),pipeIndex,mauIndex,rowIndex,
                                                    nullptr, [this,rowIndex](uint32_t i){this->shadow_ram_status_rd_cb(rowIndex,i + 6);}))
{
  synth2port_vpn_ctl_.reset();
  atomic_mod_shadow_ram_status_l_.reset();
  atomic_mod_shadow_ram_status_r_.reset();
}
MauSramRowChipReg::~MauSramRowChipReg() {
}

void MauSramRowChipReg::shadow_ram_status_rd_cb(int row, int col) {
  MauSram *mau_sram = mau_sram_row_->sram_lookup(col);
  if (mau_sram == NULL) return;
  // Read pending state of relevant SRAM
  int  index = 0;
  bool pending = mau_sram->pending_index(&index);
  if (col >= 6) {
    atomic_mod_shadow_ram_status_r_.atomic_mod_shadow_ram_status_adr    (col - 6,index);
    atomic_mod_shadow_ram_status_r_.atomic_mod_shadow_ram_status_pending(col - 6,pending?1:0);
  } else {
    atomic_mod_shadow_ram_status_l_.atomic_mod_shadow_ram_status_adr    (col - 0,index);
    atomic_mod_shadow_ram_status_l_.atomic_mod_shadow_ram_status_pending(col - 0,pending?1:0);
  }
}

}
