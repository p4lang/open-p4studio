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

#ifndef _SHARED_MAU_GATEWAY_PAYLOAD_
#define _SHARED_MAU_GATEWAY_PAYLOAD_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <mau-gateway-payload-reg.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauSramRow;

  class MauGatewayPayload : public MauObject {

    static constexpr int kMatchOutputBusWidth = MauDefs::kMatchOutputBusWidth;
    static constexpr int kTindOutputBusWidth = MauDefs::kTindOutputBusWidth;
    static constexpr int kAdrStartPos = kTindOutputBusWidth;
    static constexpr int kAdrWidth    = kMatchOutputBusWidth - kAdrStartPos;
    
 public:
    static bool kRelaxGatewayReplicationCheck; // Defined in rmt-config.cpp
    
    MauGatewayPayload(RmtObjectManager *om, int pipeIndex, int mauIndex,
                      int rowIndex, int which, Mau *mau, MauSramRow *row);
    ~MauGatewayPayload();

    void put_payload_on_bus();

    
    // called by MauGatewayPayloadReg when data entry changes
    void data_update(int index);
    void adr_update();
    void logical_table_update();

    uint32_t get_payload_data(int i) { return mau_gateway_payload_reg_.GetPayloadData(i); }
    uint32_t get_payload_adr()       { return mau_gateway_payload_reg_.GetPayloadAdr(); }
    bool     verify_replication()    { return mau_gateway_payload_reg_.VerifyReplication(); }

    bool get_inhibit_enabled(MauDefs::BusTypeEnum bus) {
      if ( bus == MauDefs::kTindBus ) 
        return mau_gateway_payload_reg_.GetInhibitEnabledTind();
      else
        return mau_gateway_payload_reg_.GetInhibitEnabledExact();
    }
    int  get_logical_table(MauDefs::BusTypeEnum bus) {
      if ( bus == MauDefs::kTindBus ) 
        return mau_gateway_payload_reg_.GetLogicalTableTind();
      else
        return mau_gateway_payload_reg_.GetLogicalTableExact();        
    }
    void get_result_busses_enabled(bool* exact_match,bool* tind) {
      mau_gateway_payload_reg_.GetResultBussesEnabled(exact_match,tind);
    }
    void get_disable(bool* exact_match,bool* tind) {
      mau_gateway_payload_reg_.GetDisable(exact_match,tind);
    }
    
 private:

    const int payload_index_;
    Mau* const mau_;
    MauSramRow* const row_;
    int which_;
    bool enabled_[2]{false,false};
    int logical_table_[2]{-1,-1}; // the power on reset will change this to 0
    BitVector< kMatchOutputBusWidth > payload_;
    MauGatewayPayloadReg mau_gateway_payload_reg_;
  };
}

#endif // ifndef _SHARED_MAU_GATEWAY_PAYLOAD_
