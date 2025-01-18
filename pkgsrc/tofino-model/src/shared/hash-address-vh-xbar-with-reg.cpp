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

#include <rmt-log.h>
#include <hash-address-vh-xbar-with-reg.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

HashAddressVhXbarWithReg::HashAddressVhXbarWithReg (
    int chipNumber, int pipe, int mau, int row,
    MauHashGeneratorWithReg* mau_hash_generator )
    : mau_hash_generator_(mau_hash_generator),
      mem_xbar_ctl_array_(default_adapter(mem_xbar_ctl_array_, chipNumber, pipe, mau, row,
                           [this](uint32_t a0){this->MemXbarCtrlArrayCallback(a0);} )),
      row_xbar_ctl_array_(default_adapter(row_xbar_ctl_array_, chipNumber, pipe, mau, row,
                           [this](uint32_t a0){this->RowXbarCtrlArrayCallback(a0);} )),
      bank_enable_array_(default_adapter(bank_enable_array_, chipNumber, pipe, mau, row,
                          [this](uint32_t a0){this->BankEnableArrayCallback(a0);} )),
      alu_hashdata_bytemask_(default_adapter(alu_hashdata_bytemask_, chipNumber, pipe, mau, row,
                              [this](){this->AluHashdataBytemaskCallback();} ))
{
  mem_xbar_ctl_array_.reset();
  row_xbar_ctl_array_.reset();
  bank_enable_array_.reset();
  alu_hashdata_bytemask_.reset();
}

}

