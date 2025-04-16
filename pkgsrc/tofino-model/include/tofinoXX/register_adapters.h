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

#ifndef _TOFINOXX_REGISTER_ADAPTERS_
#define _TOFINOXX_REGISTER_ADAPTERS_
#include <utility>
#include <common/rmt-assert.h>

namespace MODEL_CHIP_NAMESPACE {

// r is only used to work out the type of register to build, the const& is to
//  suppress uninitialized varaible warnings
template<class REG,class... Ts> REG default_adapter(const REG& r,Ts&& ... args) {
  return REG(std::forward<Ts>(args)...);
}

// Make a register using new, needed because gcc5 was not doing NRVO in this case
//  see register_block.h for more details
// In theory all the adapters should have _new versions, but only this one is used
//  in the current code.
template<class REG,class... Ts> REG* default_adapter_new(const REG* r,Ts&& ... args) {
  return new REG(std::forward<Ts>(args)...);
}


template<class REG,class... Ts> REG learning_filter_adapter(const REG& r,int chip, int pipe, Ts&& ... args) {
  switch (pipe) {
    case 0: return REG(chip, REG::DvslAddrmapEnum::kLfltr0, std::forward<Ts>(args)...);
    case 1: return REG(chip, REG::DvslAddrmapEnum::kLfltr1, std::forward<Ts>(args)...);
    case 2: return REG(chip, REG::DvslAddrmapEnum::kLfltr2, std::forward<Ts>(args)...);
    case 3: return REG(chip, REG::DvslAddrmapEnum::kLfltr3, std::forward<Ts>(args)...);
    default: RMT_ASSERT(0);
  }
}

template<class REG,class... Ts> REG packet_replication_engine_adapter(const REG& r,int chip, Ts&& ... args) {
  return REG(chip,std::forward<Ts>(args)...);
}

// this is a temporary hack because I didn't remove the pipe from the trestles memory hierarchy
template<class REG,class... Ts> REG default_mem_adapter(const REG& r,int chip, Ts&& ... args) {
  return REG(chip,std::forward<Ts>(args)...);
}

// Adapters just for Ingress/Egress Parsers
template<class REG,class... Ts> REG prsr_mem_adapter(const REG& r, int chip, int pipe, int index, Ts&& ... args) {
  auto ing_egr_enum = (index==0) ?REG::kIPrsr :REG::kEPrsr;
  return REG(chip, pipe, ing_egr_enum, std::forward<Ts>(args)...);
}
template<class REG,class... Ts> REG prsr_reg_adapter(const REG& r, int chip, int pipe, int index, Ts&& ... args) {
  auto prsr_enum = (index==0) ?REG::kIbp18Reg :REG::kEbp18Reg;
  return REG(chip, pipe, prsr_enum, std::forward<Ts>(args)...);
}

template<class REG,class... Ts> REG pipe_adapter(const REG& r,Ts&& ... args) {
  return REG(std::forward<Ts>(args)...);
}
template<class REG,class... Ts> REG epb_adapter(const REG& r,Ts&& ... args) {
  return REG(std::forward<Ts>(args)...);
}
template<class REG,class... Ts> REG ipb_adapter(const REG& r, int chip, int pipe, int ipbIndex, int ipbChan, Ts&& ... args) {
  switch (ipbChan) {
    case 0: return REG(chip, pipe, ipbIndex, REG::IngBufRegsEnum::kChan0Group, std::forward<Ts>(args)...);
    case 1: return REG(chip, pipe, ipbIndex, REG::IngBufRegsEnum::kChan1Group, std::forward<Ts>(args)...);
    case 2: return REG(chip, pipe, ipbIndex, REG::IngBufRegsEnum::kChan2Group, std::forward<Ts>(args)...);
    case 3: return REG(chip, pipe, ipbIndex, REG::IngBufRegsEnum::kChan3Group, std::forward<Ts>(args)...);
    default: RMT_ASSERT(0);
  }
}

// This still in use by pktgen etc
template<class REG,class... Ts> REG parser_adapter(const REG& r,Ts&& ... args) {
  return REG(std::forward<Ts>(args)...);
}

template<class REG,class... Ts> REG deparser_in_hdr_adapter(const REG& r,Ts&& ... args) {
  return REG(std::forward<Ts>(args)...);
}

template<class REG,class... Ts> REG deparser_out_adapter(const REG& r,Ts&& ... args) {
  return REG(std::forward<Ts>(args)...);
}


template<class REG,class... Ts> REG* misc_regs_dbg_rst_adapter_new(const REG* r, int chip, int which, Ts&& ... args) {
  switch (which) {
    case 0: return new REG(chip, REG::MiscRegsEnum::kDbgRst0, std::forward<Ts>(args)...);
    case 1: return new REG(chip, REG::MiscRegsEnum::kDbgRst1, std::forward<Ts>(args)...);
    default: RMT_ASSERT(0);
  }
}

};

#endif
