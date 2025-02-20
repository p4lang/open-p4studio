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
#include <mau-sram-column-reg.h>
#include <mau-sram-column.h>
#include <register_adapters.h>



namespace MODEL_CHIP_NAMESPACE {
  MauSramColumnReg::MauSramColumnReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                                     int colIndex, MauSramColumn *mauSramColumn)
      : MauObject(om, pipeIndex, mauIndex), mau_sram_column_(mauSramColumn),
        hitmap_output_map_array_(default_adapter(hitmap_output_map_array_,chip_index(), pipeIndex, mauIndex, colIndex,
                                 [this](uint32_t i){this->hitmap_write_callback(i);})),
        row_action_nxtable_bus_drive_array_(default_adapter(row_action_nxtable_bus_drive_array_,chip_index(), pipeIndex, mauIndex, colIndex,
                                            [this](uint32_t i){this->nxtable_write_callback(i);})),
        // Next two not really column regs but useful to use here
        match_to_logical_table_ixbar_outputmap_array2_(default_adapter(match_to_logical_table_ixbar_outputmap_array2_,chip_index(), pipeIndex, mauIndex,
                                                       [this](uint32_t i,uint32_t j){this->logical_table_write_callback(i,j);})) {
        //match_to_logical_table_icxbar_outputmap_array2_(chip_index(), pipeIndex, mauIndex,
        //[this](uint32_t i,uint32_t j){this->logical_table_write_callback(i,j);}) {

    hitmap_output_map_array_.reset();
    row_action_nxtable_bus_drive_array_.reset();
    match_to_logical_table_ixbar_outputmap_array2_.reset();
  }
  MauSramColumnReg::~MauSramColumnReg() {
  }

  void MauSramColumnReg::hitmap_write_callback(uint32_t i) {
    mau_sram_column_->maps_changed();
  }
  void MauSramColumnReg::nxtable_write_callback(uint32_t i) {
    mau_sram_column_->maps_changed();
  }
  void MauSramColumnReg::logical_table_write_callback(uint32_t i, uint32_t j) {
    mau_sram_column_->maps_changed();
  }

  uint8_t MauSramColumnReg::get_hitmap_output_map(uint8_t hit) {
    // Per-column
    if (!hitmap_output_map_array_.enabled_4bit_muxctl_enable(hit)) return 0xFF;
    return hitmap_output_map_array_.enabled_4bit_muxctl_select(hit);
  }
  uint8_t MauSramColumnReg::get_nxtable_bus_drive(uint8_t row) {
    // Per-column
    return row_action_nxtable_bus_drive_array_.row_action_nxtable_bus_drive(row);
  }
  int MauSramColumnReg::get_logical_table_for_row(uint8_t row, int result_buses, int which) {
    // The which param is used to select ExactMatch (0) or TernaryMatch (1)
    if ( !((result_buses == 1) || (result_buses == 2)) ) return -1; // This func only handles single bus
    uint8_t input = (row << 1) | (result_buses >> 1); // result_bus & 1 => LSB 0, result_bus & 2 => LSB 1
    if (!match_to_logical_table_ixbar_outputmap_array2_.enabled_4bit_muxctl_enable(which,input)) return -1;
    return match_to_logical_table_ixbar_outputmap_array2_.enabled_4bit_muxctl_select(which,input);
  }
  uint16_t MauSramColumnReg::get_all_logical_tables_for_row(uint8_t row, int result_buses, int which) {
    int table_bus0 = ((result_buses & 1) == 1) ?get_logical_table_for_row(row, 1, which) :-1;
    int table_bus1 = ((result_buses & 2) == 2) ?get_logical_table_for_row(row, 2, which) :-1;
    return ((table_bus0 >= 0) ?1<<table_bus0 :0) | ((table_bus1 >= 0) ?1<<table_bus1 :0);
  }
}
