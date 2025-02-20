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
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-stash-column-reg.h>
#include <mau-stash-column.h>
#include <register_adapters.h>



namespace MODEL_CHIP_NAMESPACE {
MauStashColumnReg::MauStashColumnReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                                     MauStashColumn *mauStashColumn)
    : MauObject(om, pipeIndex, mauIndex),
      stash_hitmap_output_map_array2_(default_adapter(stash_hitmap_output_map_array2_,chip_index(), pipeIndex, mauIndex )),
      stash_next_table_lut_array2_(default_adapter(stash_next_table_lut_array2_,chip_index(), pipeIndex, mauIndex )),
      stash_row_nxtable_bus_drive_array2_(default_adapter(stash_row_nxtable_bus_drive_array2_,chip_index(), pipeIndex, mauIndex )),
      match_to_logical_table_ixbar_outputmap_array2_(default_adapter(match_to_logical_table_ixbar_outputmap_array2_,chip_index(), pipeIndex, mauIndex ))
{
  stash_hitmap_output_map_array2_.reset();
  stash_next_table_lut_array2_.reset();
  stash_row_nxtable_bus_drive_array2_.reset();
  match_to_logical_table_ixbar_outputmap_array2_.reset();
}

MauStashColumnReg::~MauStashColumnReg() {
}

}
