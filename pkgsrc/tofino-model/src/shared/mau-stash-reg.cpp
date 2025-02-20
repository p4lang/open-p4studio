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

#include <string>
#include <common/rmt-assert.h>
#include <mau-stash-reg.h>
#include <mau-stash.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

MauStashReg::MauStashReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                         int rowIndex, MauStash *mauStash)
    : MauObject(om, pipeIndex, mauIndex), 

      stash_bank_enable_array_(default_adapter(stash_bank_enable_array_, chip_index(), pipeIndex, mauIndex, rowIndex )),
      stash_data_array2_(default_adapter(stash_data_array2_, chip_index(), pipeIndex, mauIndex, rowIndex,
                          [this](uint32_t a1,uint32_t a0){this->data_callback(a1,a0);})),
      stash_hashkey_data_array_(default_adapter(stash_hashkey_data_array_, chip_index(), pipeIndex, mauIndex, rowIndex )),
      stash_match_address_array_(default_adapter(stash_match_address_array_, chip_index(), pipeIndex, mauIndex, rowIndex )),
      stash_version_valid_array_(default_adapter(stash_version_valid_array_, chip_index(), pipeIndex, mauIndex, rowIndex )),
      stash_match_input_data_ctl_array_(default_adapter(stash_match_input_data_ctl_array_, chip_index(), pipeIndex, mauIndex, rowIndex )),
      stash_match_mask_array2_(default_adapter(stash_match_mask_array2_, chip_index(), pipeIndex, mauIndex, rowIndex,
                                [this](uint32_t a1,uint32_t a0){this->mask_callback(a1,a0);})),
      stash_match_result_bus_select_array_(default_adapter(stash_match_result_bus_select_array_, chip_index(), pipeIndex, mauIndex, rowIndex )),
      stash_bus_overload_bytemask_array_(default_adapter(stash_bus_overload_bytemask_array_, chip_index(), pipeIndex, mauIndex, rowIndex )),
      mau_stash_(mauStash)
{
  RMT_LOG_VERBOSE("MAU_STASH_REG::create\n");

  stash_bank_enable_array_.reset();
  stash_data_array2_.reset();
  stash_hashkey_data_array_.reset();
  stash_match_address_array_.reset();
  stash_version_valid_array_.reset();
  stash_match_input_data_ctl_array_.reset();
  stash_match_mask_array2_.reset();
  stash_match_result_bus_select_array_.reset();
  stash_bus_overload_bytemask_array_.reset();
}

MauStashReg::~MauStashReg()
{
  RMT_LOG_VERBOSE("MAU_STASH_REG::delete\n");
}

void MauStashReg::data_callback(uint32_t a1,uint32_t a0) {
  mau_stash_->entry_update(a1,a0,
                           stash_data_array2_.stash_data(a1,a0));
}
void MauStashReg::mask_callback(uint32_t a1,uint32_t a0) {
  mau_stash_->mask_update(a1,a0,
                          stash_match_mask_array2_.stash_match_mask(a1,a0));
}

// void MauStashReg::VersionValidUpdate(int index)
// {
//   mau_stash_->VersionValidUpdate(index);
// }

}
