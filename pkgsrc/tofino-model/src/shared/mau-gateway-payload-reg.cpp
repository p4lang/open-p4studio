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
#include <rmt-types.h>
#include <rmt-object-manager.h>
#include <mau-gateway-payload.h>
#include <mau-gateway-payload-reg.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

MauGatewayPayloadReg::MauGatewayPayloadReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                                       int rowIndex, int which, MauGatewayPayload *mauGatewayPayload)
    : MauObject(om, pipeIndex, mauIndex, RmtTypes::kRmtTypeMauGatewayPayloadReg, rowIndex),

  gateway_payload_data_array_{
  { {default_adapter(gateway_payload_data_array_[0],chip_index(), pipeIndex, mauIndex, rowIndex, which, 0, 0,
                 [this](){this->DataUpdate(0);} )},
    {default_adapter(gateway_payload_data_array_[1],chip_index(), pipeIndex, mauIndex, rowIndex, which, 1, 0,
                 [this](){this->DataUpdate(1);} )}
         }},
  gw_replicate_data_array_{
    { {default_adapter(gw_replicate_data_array_[0],chip_index(), pipeIndex, mauIndex, rowIndex, which, 0, 1 )},
      {default_adapter(gw_replicate_data_array_[1],chip_index(), pipeIndex, mauIndex, rowIndex, which, 1, 1 )}
         }},
  
  gateway_payload_match_adr_(default_adapter(gateway_payload_match_adr_, chip_index(), pipeIndex, mauIndex, rowIndex, which, 0, 
                              [this](){this->AdrUpdate();} )),
  gw_replicate_match_adr_(default_adapter(gw_replicate_match_adr_, chip_index(), pipeIndex, mauIndex, rowIndex, which, 1)),
  
  gateway_to_pbus_xbar_ctl_(default_adapter(gateway_to_pbus_xbar_ctl_,chip_index(), pipeIndex, mauIndex, rowIndex*2 + which,
                               [this](){this->LogicalTableUpdate();})),
  gateway_payload_exact_pbus_(default_adapter(gateway_payload_exact_pbus_,chip_index(), pipeIndex, mauIndex, rowIndex )),
  gateway_payload_tind_pbus_(default_adapter(gateway_payload_tind_pbus_,chip_index(), pipeIndex, mauIndex, rowIndex )),
  gateway_payload_exact_disable_(default_adapter(gateway_payload_exact_disable_,chip_index(), pipeIndex, mauIndex, rowIndex )),
  gateway_payload_tind_disable_(default_adapter(gateway_payload_tind_disable_,chip_index(), pipeIndex, mauIndex, rowIndex )),
  mau_gateway_payload_(mauGatewayPayload),
  which_(which)    
{
  RMT_LOG_VERBOSE("MAU_GATEWAY_PAYLOAD_REG::create\n");
  gateway_payload_data_array_[0].reset();
  gateway_payload_data_array_[1].reset();
  gw_replicate_data_array_[0].reset();
  gw_replicate_data_array_[1].reset();
  
  gateway_payload_match_adr_.reset();
  gw_replicate_match_adr_.reset();

  gateway_to_pbus_xbar_ctl_.reset();
  
  // TODO_0709 added exact/tind 
  gateway_payload_exact_pbus_.reset();
  gateway_payload_tind_pbus_.reset();

  gateway_payload_exact_disable_.reset();
  gateway_payload_tind_disable_.reset();
}

MauGatewayPayloadReg::~MauGatewayPayloadReg()
{
  RMT_LOG_VERBOSE("MAU_GATEWAY_PAYLOAD_REG::delete\n");
}

void MauGatewayPayloadReg::DataUpdate(int index) {
  mau_gateway_payload_->data_update(index);
}

void MauGatewayPayloadReg::AdrUpdate() {
  mau_gateway_payload_->adr_update();
}

void MauGatewayPayloadReg::LogicalTableUpdate() {
  mau_gateway_payload_->logical_table_update();
}


}
