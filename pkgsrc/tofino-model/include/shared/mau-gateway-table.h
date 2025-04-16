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

#ifndef _SHARED_MAU_GATEWAY_TABLE_
#define _SHARED_MAU_GATEWAY_TABLE_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <phv.h>
#include <tcam3.h>
#include <mau-gateway-table-reg.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauSramRow;

  class MauGatewayTable : public MauObject {
    static constexpr int    kEntries       = MauDefs::kGatewayTableEntries;
    // The ExpressionA and ExpressionB terminology comes from the Tofino Architecture spec v1.8
    //  these correspond to variously:
    //  ExpressionA = data_entry, hash_entry, Boolean Match
    //  ExpressionB = match_data, match_entry, Vector Equality Match
    static constexpr int  kExpressionAWidth = MauDefs::kGatewayTableExpressionAWidth;
    static constexpr int  kExpressionBWidth = MauDefs::kGatewayTableExpressionBWidth;
    static constexpr int  kVersionWidth     = MauDefs::kVersionBits;

    static constexpr int  kWholeMatchDataWidth = MauDefs::kSramWidth;
    static constexpr int  kWholeHashWidth      = MauDefs::kHashOutputWidth;

    static constexpr int  kHashExtractWidth    = MauDefs::kGatewayTableHashExtractWidth;
    static constexpr int  kHashExtractStart    = kWholeHashWidth - kHashExtractWidth;

    // Position of fields in tcam (this concept only exists in the model
    //   so I can use one tcam3 for the whole gateway table)
    static constexpr int  kTcamWidth   = kExpressionAWidth + kVersionWidth + kExpressionBWidth;
    static constexpr int  kTcamExpressionAPos = 0;
    static constexpr int  kTcamExpressionBPos = kTcamExpressionAPos + kExpressionAWidth;
    static constexpr int  kTcamVersionPos     = kTcamExpressionBPos + kExpressionBWidth;

    
 public:
    MauGatewayTable(RmtObjectManager *om, int pipeIndex, int mauIndex,
                    int rowIndex, int tableIndex, Mau *mau, MauSramRow *row);
    ~MauGatewayTable();

    /** lookup the phv and return whether it hit and, if so, the hit index
    */
    void lookup(Phv *phv, bool *hit, int *hit_index);
    MauSramRow *row()  const { return row_; }
    
    // called by MauGatewayTableReg when data entry changes
    void VersionValidUpdate(int index);
    void DataUpdate(int index,int word);
    void MatchDataUpdate(int index,int word);

    uint8_t GetLogicalTable() { return mau_gateway_table_reg_.GetLogicalTable(); }

 private:
    uint64_t CalculateExpressionAData(Phv *phv);
    uint32_t CalculateExpressionBData(Phv *phv);
    void Calculate2bDirtCAM(uint64_t in,uint64_t *s0,uint64_t *s1);
    void Calculate4bDirtCAM(uint64_t in,uint64_t *s0,uint64_t *s1);

 private:
    Tcam3<kEntries, kTcamWidth > tcam_;
    MauGatewayTableReg mau_gateway_table_reg_;
    MauSramRow* row_;
    
  };
}

#endif // ifndef _SHARED_MAU_GATEWAY_TABLE_
