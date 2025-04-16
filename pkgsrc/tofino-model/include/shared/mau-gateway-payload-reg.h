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

#ifndef _SHARED_MAU_GATEWAY_PAYLOAD_REG_
#define _SHARED_MAU_GATEWAY_PAYLOAD_REG_

#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>

// Reg defs auto-generated from Semifore
#include <register_includes/gateway_payload_data.h>
#include <register_includes/gateway_payload_match_adr.h>
#include <register_includes/gateway_to_pbus_xbar_ctl.h>
#include <register_includes/gateway_payload_exact_pbus.h>
#include <register_includes/gateway_payload_tind_pbus.h>
#include <register_includes/gateway_payload_tind_disable.h>
#include <register_includes/gateway_payload_exact_disable.h>

namespace MODEL_CHIP_NAMESPACE {

class MauGatewayPayload;

class MauGatewayPayloadReg : public MauObject {

  static constexpr int    kEntries = 2; // to give 64 bits of payload data

  public:
    MauGatewayPayloadReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                         int rowIndex, int tableIndex, MauGatewayPayload* mau_gateway_payload);
    virtual ~MauGatewayPayloadReg();

    uint32_t GetPayloadData(int i) {
      RMT_ASSERT(i>=0 && i<2);
      return gateway_payload_data_array_[i].gateway_payload_data();
    }
    uint32_t GetPayloadAdr() {
      return gateway_payload_match_adr_.gateway_payload_match_adr();      
    }
    bool VerifyReplication() {
      bool same = true;
      if (gateway_payload_data_array_[0].gateway_payload_data() !=
          gw_replicate_data_array_[0].gateway_payload_data()) same = false;
      if (gateway_payload_data_array_[1].gateway_payload_data() !=
          gw_replicate_data_array_[1].gateway_payload_data()) same = false;
      if (gateway_payload_match_adr_.gateway_payload_match_adr() !=
          gw_replicate_match_adr_.gateway_payload_match_adr()) same = false;
      return same;
    }

  bool GetInhibitEnabledTind()  { return gateway_to_pbus_xbar_ctl_.tind_inhibit_enable(); }
  bool GetInhibitEnabledExact() { return gateway_to_pbus_xbar_ctl_.exact_inhibit_enable(); }

  int GetLogicalTableTind() { return gateway_to_pbus_xbar_ctl_.tind_logical_select(); }
  int GetLogicalTableExact() { return gateway_to_pbus_xbar_ctl_.exact_logical_select(); }

  void GetResultBussesEnabled(bool* exact_match,bool* tind) {

    int bitn = (which_ == 0) ? 0 : 1;

    *exact_match = 1 & (gateway_payload_exact_pbus_.gateway_payload_exact_pbus() >> bitn);
    *tind        = 1 & (gateway_payload_tind_pbus_.gateway_payload_tind_pbus()   >> bitn);
  }
 
  void GetDisable(bool* exact_match,bool* tind); // does not exist in Tofino, just returns false for both
  
 
    
 private:
    std::array<register_classes::GatewayPayloadData,2>  gateway_payload_data_array_;
    std::array<register_classes::GatewayPayloadData,2>  gw_replicate_data_array_;
    register_classes::GatewayPayloadMatchAdr            gateway_payload_match_adr_;
    register_classes::GatewayPayloadMatchAdr            gw_replicate_match_adr_;
    // TODO: gateway_to_pbus_xbar_ctl_ was renamed from gateway_hitmap_xbar_ctl
    //         check that nothing else needs to be changed
    register_classes::GatewayToPbusXbarCtl              gateway_to_pbus_xbar_ctl_;
    register_classes::GatewayPayloadExactPbus           gateway_payload_exact_pbus_;
    register_classes::GatewayPayloadTindPbus            gateway_payload_tind_pbus_;
    register_classes::GatewayPayloadExactDisable        gateway_payload_exact_disable_;
    register_classes::GatewayPayloadTindDisable         gateway_payload_tind_disable_;
    MauGatewayPayload* mau_gateway_payload_;
    int which_;

    void DataUpdate(int index);
    void AdrUpdate();
    void LogicalTableUpdate();
  };
}
#endif // _SHARED_MAU_GATEWAY_PAYLOAD_REG_
