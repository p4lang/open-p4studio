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

#ifndef _SHARED_INDIRECT_ACCESS_BLOCK__
#define _SHARED_INDIRECT_ACCESS_BLOCK__

#include <vector>
#include <model_core/register_block.h>

#include <register_includes/cpu_ind_data11_mutable.h>
#include <register_includes/cpu_ind_data00_mutable.h>
#include <register_includes/cpu_ind_addr_low.h>
#include <register_includes/cpu_ind_addr_high.h>
#include <register_includes/cpu_ind_data10_mutable.h>
#include <register_includes/cpu_ind_data01_mutable.h>

namespace MODEL_CHIP_NAMESPACE {

class IndirectAccessBlock {
public:
  IndirectAccessBlock(int chip) :
    ctor_running_(true),
    chip_(chip),
    cpu_ind_data00_(chip, [this](){ this->WriteCallback(); }, [this](){ this->ReadCallback(); } ),
    cpu_ind_data01_(chip),
    cpu_ind_data10_(chip),
    cpu_ind_data11_(chip),
    cpu_ind_addr_high_(chip),
    cpu_ind_addr_low_(chip)
  {
    cpu_ind_data01_.reset();
    cpu_ind_data10_.reset();
    cpu_ind_data11_.reset();
    cpu_ind_addr_high_.reset();
    cpu_ind_addr_low_.reset();
    // do this last as will trigger callbacks
    cpu_ind_data00_.reset();
    ctor_running_ = false;
  };
  ~IndirectAccessBlock() {};

  void ReadCallback();
  void WriteCallback();

private:
  bool ctor_running_;
  int chip_;
  register_classes::CpuIndData00Mutable   cpu_ind_data00_;
  register_classes::CpuIndData01Mutable   cpu_ind_data01_;
  register_classes::CpuIndData10Mutable   cpu_ind_data10_;
  register_classes::CpuIndData11Mutable   cpu_ind_data11_;
  register_classes::CpuIndAddrHigh        cpu_ind_addr_high_;
  register_classes::CpuIndAddrLow         cpu_ind_addr_low_;
};

}

#endif
