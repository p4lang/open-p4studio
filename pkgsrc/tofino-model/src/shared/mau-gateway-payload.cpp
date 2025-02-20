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

#include <mau.h>
#include <string>
#include <rmt-log.h>
#include <rmt-types.h>
#include <rmt-object-manager.h>
#include <mau-gateway-payload.h>
#include <mau-logical-table.h>
#include <mau-sram-row.h>

namespace MODEL_CHIP_NAMESPACE {

MauGatewayPayload::MauGatewayPayload(RmtObjectManager* om, int pipeIndex, int mauIndex,
                                     int rowIndex, int which,Mau* mau, MauSramRow* row)
    : MauObject(om, pipeIndex, mauIndex, RmtTypes::kRmtTypeMauGatewayPayload, rowIndex, mau),
      payload_index_((rowIndex*2)+which),
      mau_(mau),
      row_(row),
      which_(which),
      mau_gateway_payload_reg_(om,pipeIndex,mauIndex,rowIndex,which,this)
{
  RMT_LOG_VERBOSE("MAU_GATEWAY_PAYLOAD::create\n");
  payload_.fill_all_zeros();
}

MauGatewayPayload::~MauGatewayPayload()
{
  RMT_LOG_VERBOSE("MAU_GATEWAY_PAYLOAD::delete\n");
}

void MauGatewayPayload::put_payload_on_bus() {
  bool exact_match;
  bool tind;

  get_result_busses_enabled( &exact_match, &tind );

  // I don't think both busses should be driven
  RMT_ASSERT( ! ( exact_match && tind ) );
  // Verify replicated regs 
  if (!kRelaxGatewayReplicationCheck) RMT_ASSERT( verify_replication() );
  
  if (tind) {
    RMT_LOG(RmtDebug::verbose(),
            "MauGatewayPayload putting payload on tind bus=%d\n",which_);
    BitVector<kTindOutputBusWidth> bus;
    payload_.extract_into(0,&bus);
    row_->set_tind_output_bus(which_, bus, 999);
  }
  if (exact_match) {
    RMT_LOG(RmtDebug::verbose(),
            "MauGatewayPayload putting payload on match output bus=%d\n",which_);
    row_->set_match_output_bus(which_, payload_, 999);
  }

}

void MauGatewayPayload::data_update(int a0) {
  RMT_ASSERT( a0 < (kMatchOutputBusWidth/32) );
  payload_.set32(a0, get_payload_data(a0) );
}

void MauGatewayPayload::adr_update() {
  payload_.set_word(get_payload_adr(), kAdrStartPos, kAdrWidth );
}

void MauGatewayPayload::logical_table_update() {
  // when logical table changes, register with that logical table,
  //   so it is easy to find the payload from the logical table when needed

  for (int i=0;i<2;++i) {
    MauDefs::BusTypeEnum bus = i ? MauDefs::kExactMatchBus : MauDefs::kTindBus;
    int new_logical_table = get_logical_table(bus);
    if (enabled_[i]) {
      if ( logical_table_[i] != new_logical_table ) {
        MauLogicalTable* old_lt =  mau_->logical_table_lookup( logical_table_[i]  );
        RMT_ASSERT(old_lt!=nullptr);
        old_lt->remove_gateway_payload( bus, payload_index_ );
      }
    }
    enabled_[i] = get_inhibit_enabled(bus);
    logical_table_[i] = new_logical_table;
    RMT_LOG(RmtDebug::verbose(),
            "MauGatewayPayload bus=%d new_logical_table=%d enabled_=%d\n",i,new_logical_table,enabled_[i]);
    if (enabled_[i]) {
      MauLogicalTable* new_lt =  mau_->logical_table_lookup( logical_table_[i] );
      RMT_ASSERT(new_lt!=nullptr);
      new_lt->add_gateway_payload( bus, payload_index_ );
    }
  }
}



}
