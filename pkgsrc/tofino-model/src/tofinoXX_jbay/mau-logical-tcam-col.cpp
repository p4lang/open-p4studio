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
#include <mau-logical-tcam-col.h>
#include <mau-logical-tcam.h>
#include <mau-sram.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

MauLogicalTcamCol::MauLogicalTcamCol(int chipIndex, int pipeIndex, int mauIndex,
                                       int ltcamIndex, int colIndex,
                                       MauLogicalTcam *mauLogicalTcam)
    : mau_logical_tcam_(mauLogicalTcam), col_index_(colIndex), curr_table_map_(0u),
      tcam_table_map_(default_adapter(tcam_table_map_,chipIndex, pipeIndex, mauIndex, colIndex, ltcamIndex,
                                      [this](){this->table_map_write_callback(); })) {
  tcam_table_map_.reset();
}
MauLogicalTcamCol::~MauLogicalTcamCol() {
}

void MauLogicalTcamCol::set_table_map(uint32_t new_table_map) {
  if (curr_table_map_ == new_table_map) return; // No change
  mau_logical_tcam_->tcam_table_map_updated(col_index_, new_table_map, curr_table_map_);
  curr_table_map_ = new_table_map;
}
void MauLogicalTcamCol::table_map_write_callback() {
  set_table_map( tcam_table_map_.tcam_table_map() );
}

}
