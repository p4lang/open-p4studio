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

#ifndef _SHARED_MAU_GATEWAY_TABLE_REG_
#define _SHARED_MAU_GATEWAY_TABLE_REG_

#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>

// Reg defs auto-generated from Semifore
#include <register_includes/gateway_table_ctl.h>
#include <register_includes/gateway_table_data_entry_array2.h>
#include <register_includes/gateway_table_entry_matchdata_array2.h>
#include <register_includes/gateway_table_matchdata_xor_en.h>
#include <register_includes/gateway_table_vv_entry_array.h>

namespace MODEL_CHIP_NAMESPACE {

class MauGatewayTable;

class MauGatewayTableReg : public MauObject {

  static constexpr int    kEntries = MauDefs::kGatewayTableEntries;

  public:
    MauGatewayTableReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                       int rowIndex, int tableIndex, MauGatewayTable *mauGatewayTable);
    virtual ~MauGatewayTableReg();

    uint64_t GetDataEntryWord0( int entry ) {
      RMT_ASSERT( entry>=0 && entry<kEntries );
      return gateway_table_data_.gateway_table_data_entry(entry,0);
    }
    uint64_t GetDataEntryWord1( int entry ) {
      RMT_ASSERT( entry>=0 && entry<kEntries );
      return gateway_table_data_.gateway_table_data_entry(entry,1);
    }
    uint8_t GetVersionValid0( int entry ) {
      RMT_ASSERT( entry>=0 && entry<kEntries );
      return gateway_table_vv_entry_array_.gateway_table_entry_versionvalid0(entry);
    }
    uint8_t GetVersionValid1( int entry ) {
      RMT_ASSERT( entry>=0 && entry<kEntries );
      return gateway_table_vv_entry_array_.gateway_table_entry_versionvalid1(entry);
    }

    uint8_t GetData0Select() { return gateway_table_ctl_.gateway_table_input_data0_select(); }
    uint8_t GetData1Select() { return gateway_table_ctl_.gateway_table_input_data1_select(); }
    uint8_t GetHash0Select() { return gateway_table_ctl_.gateway_table_input_hash0_select(); }
    uint8_t GetHash1Select() { return gateway_table_ctl_.gateway_table_input_hash1_select(); }
    uint8_t GetLogicalTable() { return gateway_table_ctl_.gateway_table_logical_table(); }
    enum Mode { kNormal, k2bDirtCAM, k4bDirtCAM, kUnknown };
    Mode GetMode()         {
      switch (gateway_table_ctl_.gateway_table_mode()) {
        case 0:  return kNormal;
        case 1:  return k2bDirtCAM;
        case 2:  return k4bDirtCAM;
        default: return kUnknown;
      }
    }
    uint8_t GetEgressThread() { return gateway_table_ctl_.gateway_table_thread();  }
    uint32_t GetMatchData(int index,int word) { return gateway_table_match_data_.gateway_table_entry_matchdata(index,word); }

    uint32_t GetXorEnable() { return gateway_table_match_data_xor_en_.gateway_table_matchdata_xor_en(); }
    
 private:
    register_classes::GatewayTableDataEntryArray2 gateway_table_data_;
    register_classes::GatewayTableEntryMatchdataArray2 gateway_table_match_data_;
    register_classes::GatewayTableCtl          gateway_table_ctl_;
    register_classes::GatewayTableMatchdataXorEn gateway_table_match_data_xor_en_;
    register_classes::GatewayTableVvEntryArray   gateway_table_vv_entry_array_;

    MauGatewayTable                       *mau_gateway_table_;

    void DataUpdate(int index,int word);
    void MatchDataUpdate(int index,int word);
    void VersionValidUpdate(int index);
    
  };
}
#endif // _SHARED_MAU_GATEWAY_TABLE_REG_
