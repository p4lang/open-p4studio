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

#ifndef _SHARED_MAU_STASH_COLUMN_REG_
#define _SHARED_MAU_STASH_COLUMN_REG_

#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>

// Reg defs auto-generated from Semifore
#include <register_includes/stash_hitmap_output_map_array2.h>
#include <register_includes/stash_next_table_lut_array2.h>
#include <register_includes/stash_row_nxtable_bus_drive_array2.h>
#include <register_includes/match_to_logical_table_ixbar_outputmap_array2.h>


namespace MODEL_CHIP_NAMESPACE {

class MauStashColumn;

class MauStashColumnReg : public MauObject {
    
 public:
    MauStashColumnReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                      MauStashColumn *mauStashColumn);
    virtual ~MauStashColumnReg();

    void  get_hitmap_destination_row(int stash_row,int stash_which,bool *enabled,int *destination_row) {
      RMT_ASSERT( stash_row >= 0 && stash_row < MauDefs::kSramRowsPerMau );
      RMT_ASSERT( stash_which >= 0 && stash_which < 2 );
      *destination_row = stash_hitmap_output_map_array2_.enabled_3bit_muxctl_select(stash_which,stash_row);
      *enabled = stash_hitmap_output_map_array2_.enabled_3bit_muxctl_enable(stash_which,stash_row);
    }
    uint8_t get_next_table(int stash_row, int stash_which,int entry) {
      RMT_ASSERT( stash_row >=0 && stash_row < MauDefs::kSramRowsPerMau );
      RMT_ASSERT( stash_which >= 0 && stash_which < 2 );
      RMT_ASSERT( entry>=0 && entry < (MauDefs::kStashEntries / 2) );
      uint32_t w = stash_next_table_lut_array2_.stash_next_table_lut(stash_which,stash_row);
      return (( w >> (entry*8)) & 0xff);
    }
    void get_next_table_bus_drive(int stash_row,int stash_which,bool *bus0,bool *bus1) {
      RMT_ASSERT( stash_row >= 0 && stash_row < MauDefs::kSramRowsPerMau );
      RMT_ASSERT( stash_which >= 0 && stash_which < 2 );
      uint32_t w = stash_row_nxtable_bus_drive_array2_.stash_row_nxtable_bus_drive(stash_which,stash_row);
      *bus0 = w & 1;
      *bus1 = (w>>1) & 1;
    }

    void get_row_bus_logical_table(int row,int which_bus,bool *enabled,uint8_t *logical_table) {
      RMT_ASSERT( row >=0 && row < MauDefs::kSramRowsPerMau );
      RMT_ASSERT( which_bus >= 0 && which_bus < 2 );
      *enabled = match_to_logical_table_ixbar_outputmap_array2_.
          enabled_4bit_muxctl_enable(0,(row*2)+which_bus);
      *logical_table = match_to_logical_table_ixbar_outputmap_array2_.
          enabled_4bit_muxctl_select(0,(row*2)+which_bus);
    }

    
 private:
    register_classes::StashHitmapOutputMapArray2                stash_hitmap_output_map_array2_;
    register_classes::StashNextTableLutArray2                   stash_next_table_lut_array2_; 
    register_classes::StashRowNxtableBusDriveArray2             stash_row_nxtable_bus_drive_array2_;
    register_classes::MatchToLogicalTableIxbarOutputmapArray2   match_to_logical_table_ixbar_outputmap_array2_;
  };
}
#endif // _SHARED_MAU_STASH_COLUMN_REG_
