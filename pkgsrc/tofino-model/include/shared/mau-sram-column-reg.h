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

#ifndef _SHARED_MAU_SRAM_COLUMN_REG_
#define _SHARED_MAU_SRAM_COLUMN_REG_

#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>

// Reg defs auto-generated from Semifore
#include <register_includes/match_to_logical_table_ixbar_outputmap_array2.h>
#include <register_includes/row_action_nxtable_bus_drive_array.h>
#include <register_includes/hitmap_output_map_array.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauSramColumn;

  class MauSramColumnReg : public MauObject {
    
 public:
    MauSramColumnReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                     int colIndex, MauSramColumn *mauSramColumn);
    virtual ~MauSramColumnReg();


    uint8_t  get_hitmap_output_map(uint8_t hit);
    uint8_t  get_nxtable_bus_drive(uint8_t row);
    // In get_logical_table_X funcs 'which' param 0=>ExactMatch 1=>TernaryMatch
    int      get_logical_table_for_row(uint8_t row, int result_bus, int which);
    uint16_t get_all_logical_tables_for_row(uint8_t row, int result_buses, int which);

 private:
    void    hitmap_write_callback(uint32_t i);
    void    nxtable_write_callback(uint32_t i);
    void    logical_table_write_callback(uint32_t i, uint32_t j);
    
 private:
    MauSramColumn                                                *mau_sram_column_;
    register_classes::HitmapOutputMapArray                        hitmap_output_map_array_; 
    register_classes::RowActionNxtableBusDriveArray               row_action_nxtable_bus_drive_array_;
    register_classes::MatchToLogicalTableIxbarOutputmapArray2     match_to_logical_table_ixbar_outputmap_array2_;
    //register_classes::MatchToLogicalTableIcxbarOutputmapArray2  match_to_logical_table_icxbar_outputmap_array2_;
  };
}
#endif // _SHARED_MAU_SRAM_COLUMN_REG_
