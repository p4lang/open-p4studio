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


#include <cinttypes>
#include <rmt-log.h>
#include <indirect_access_block.h>
#include <model_core/model.h>
#include <chip.h>
#include <rmt-object-manager.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_NAMESPACE {

void IndirectAccessBlock::ReadCallback() {
  uint64_t address = (static_cast<uint64_t>(cpu_ind_addr_high_.ind_addr_high()) << 32) |
      cpu_ind_addr_low_.ind_addr_low();
  uint64_t data0,data1;
  // XXX: Initialise data0/1 to keep StaticAnalysis happy
  data0 = data1 = UINT64_C(0);
  GLOBAL_MODEL->IndirectRead(chip_,address,&data0,&data1);
  cpu_ind_data00_.ind_data0(data0 & 0xffffffff);
  cpu_ind_data01_.ind_data1(data0>>32);
  cpu_ind_data10_.ind_data2(data1 & 0xffffffff);
  cpu_ind_data11_.ind_data3(data1>>32);
}

void IndirectAccessBlock::WriteCallback() {
  if (ctor_running_) return;
  uint64_t address = (static_cast<uint64_t>(cpu_ind_addr_high_.ind_addr_high()) << 32) |
      cpu_ind_addr_low_.ind_addr_low();
  uint64_t data0 = (static_cast<uint64_t>(cpu_ind_data01_.ind_data1()) << 32) | cpu_ind_data00_.ind_data0();
  uint64_t data1 = (static_cast<uint64_t>(cpu_ind_data11_.ind_data3()) << 32) | cpu_ind_data10_.ind_data2();
  //printf("Indirect Write %016" PRIx64 " = %016" PRIx64 " %016" PRIx64 " \n", address,data0,data1);
  GLOBAL_MODEL->IndirectWrite(chip_,address,data0,data1);
}

}
