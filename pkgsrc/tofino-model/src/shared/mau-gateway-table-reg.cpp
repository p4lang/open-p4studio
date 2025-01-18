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
#include <mau-gateway-table.h>
#include <mau-gateway-table-reg.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

MauGatewayTableReg::MauGatewayTableReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                                       int rowIndex, int tableIndex, MauGatewayTable *mauGatewayTable)
    : MauObject(om, pipeIndex, mauIndex, RmtTypes::kRmtTypeMauGatewayTableReg, rowIndex, tableIndex),
      gateway_table_data_(default_adapter(gateway_table_data_, chip_index(), pipeIndex, mauIndex, rowIndex, tableIndex,
                           [this](uint32_t a1,uint32_t a0){this->DataUpdate(a1,a0);})),
      gateway_table_match_data_(default_adapter(gateway_table_match_data_, chip_index(), pipeIndex, mauIndex, rowIndex, tableIndex,
                                 [this](uint32_t a1,uint32_t a0){this->MatchDataUpdate(a1,a0);})),
      gateway_table_ctl_(default_adapter(gateway_table_ctl_,chip_index(), pipeIndex, mauIndex, rowIndex, tableIndex)),
      gateway_table_match_data_xor_en_(default_adapter(gateway_table_match_data_xor_en_,chip_index(), pipeIndex, mauIndex, rowIndex, tableIndex )),
      gateway_table_vv_entry_array_(default_adapter(gateway_table_vv_entry_array_,chip_index(), pipeIndex, mauIndex, rowIndex, tableIndex,
                                    [this](uint32_t a0){this->VersionValidUpdate(a0);})),
      mau_gateway_table_(mauGatewayTable)
{
  RMT_LOG_VERBOSE("MAU_GATEWAY_TABLE_REG::create\n");

  gateway_table_data_.reset();
  gateway_table_match_data_.reset();
  gateway_table_ctl_.reset();
  gateway_table_match_data_xor_en_.reset();
  gateway_table_vv_entry_array_.reset();
}

MauGatewayTableReg::~MauGatewayTableReg()
{
  RMT_LOG_VERBOSE("MAU_GATEWAY_TABLE_REG::delete\n");
}

void MauGatewayTableReg::DataUpdate(int index,int word)
{
  mau_gateway_table_->DataUpdate(index,word);
}

void MauGatewayTableReg::MatchDataUpdate(int index,int word)
{
  mau_gateway_table_->MatchDataUpdate(index,word);
}

void MauGatewayTableReg::VersionValidUpdate(int index)
{
  mau_gateway_table_->VersionValidUpdate(index);
}

}
