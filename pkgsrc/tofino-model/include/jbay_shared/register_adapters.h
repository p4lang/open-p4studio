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

#ifndef _JBAY_SHARED_REGISTER_ADAPTERS_
#define _JBAY_SHARED_REGISTER_ADAPTERS_
#include <utility>
#include <common/rmt-assert.h>
#include <rmt-defs.h>

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

// JBay has chip, learn filter number. But use pipe here? TODO check?
template<class REG,class... Ts> REG learning_filter_adapter(const REG& r,int chip, int pipe, Ts&& ... args) {
  return REG(chip, pipe, std::forward<Ts>(args)...);
}

template<class REG,class... Ts> REG packet_replication_engine_adapter(const REG& r,int chip, int pre_index, Ts&& ... args) {
  return REG(chip,pre_index,std::forward<Ts>(args)...);
}
//template<class REG,class... Ts> REG packet_replication_engine_adapter(const REG& r,int chip, Ts&& ... args) {
//  return REG(chip,std::forward<Ts>(args)...);
//}

template<class REG,class... Ts> REG tm_sch_adapter(const REG& r, int chip, int sch_ab_sel, int sch_pipe_index, Ts&& ... args) {
  auto sch_ab_enum = (sch_ab_sel == 0) ? REG::kTmSchaTop : REG::kTmSchbTop;
  return REG(chip, sch_ab_enum, sch_pipe_index, std::forward<Ts>(args)...);
}
// WIP version...
template<class REG,class... Ts> REG tm_sch_adapter(const REG& r, int chip, int sch_pipe_index, Ts&& ... args) {
  return REG(chip, sch_pipe_index, std::forward<Ts>(args)...);
}

// TODO: fix when we know how pmarb is sliced
template<class REG,class... Ts> REG default_mem_adapter(const REG& r,int chip, Ts&& ... args) {
  return REG(chip,std::forward<Ts>(args)...);
}

// Adapters just for Ingress/Egress Parsers
template<class REG,class... Ts> REG prsr_mem_adapter(const REG& r, int chip, int pipe, int ioIndex, int prsIndex, Ts&& ... args) {
  auto ing_egr_enum = (ioIndex==0) ?REG::kIPrsrMem :REG::kEPrsrMem;
  return REG(chip, pipe, ing_egr_enum, prsIndex, std::forward<Ts>(args)...);
}
template<class REG,class... Ts> REG prsr_reg_adapter(const REG& r, int chip, int pipe, int ioIndex, int prsIndex, Ts&& ... args) {
  auto ing_egr_enum = (ioIndex==0) ?REG::kIpbprsr4reg :REG::kEpbprsr4reg;
  int prsGrp = RmtDefs::get_parser_group(prsIndex);
  int prsElt = RmtDefs::get_parser_element(prsIndex);
  return REG(chip, pipe, ing_egr_enum, prsGrp, prsElt, std::forward<Ts>(args)...);
}

// TODO: fix when we know how pmarb is sliced
template<class REG,class... Ts> REG pipe_adapter(const REG& r, int chip, int pipe, Ts&& ... args) {
  return REG(chip, pipe, std::forward<Ts>(args)...);
}
template<class REG,class... Ts> REG epb_adapter(const REG& r, int chip, int pipe, int epbIndex, int epbChan, Ts&& ... args) {
  switch (epbChan) {
    case 0: return REG(chip, pipe, epbIndex, REG::EpbRspecEnum::kChan0Group, std::forward<Ts>(args)...);
    case 1: return REG(chip, pipe, epbIndex, REG::EpbRspecEnum::kChan1Group, std::forward<Ts>(args)...);
    case 2: return REG(chip, pipe, epbIndex, REG::EpbRspecEnum::kChan2Group, std::forward<Ts>(args)...);
    case 3: return REG(chip, pipe, epbIndex, REG::EpbRspecEnum::kChan3Group, std::forward<Ts>(args)...);
    case 4: return REG(chip, pipe, epbIndex, REG::EpbRspecEnum::kChan4Group, std::forward<Ts>(args)...);
    case 5: return REG(chip, pipe, epbIndex, REG::EpbRspecEnum::kChan5Group, std::forward<Ts>(args)...);
    case 6: return REG(chip, pipe, epbIndex, REG::EpbRspecEnum::kChan6Group, std::forward<Ts>(args)...);
    case 7: return REG(chip, pipe, epbIndex, REG::EpbRspecEnum::kChan7Group, std::forward<Ts>(args)...);
    default: RMT_ASSERT(0);
  }
}
template<class REG,class... Ts> REG ipb_adapter(const REG& r, int chip, int pipe, int ipbIndex, int ipbChan, Ts&& ... args) {
  switch (ipbChan) {
    case 0: return REG(chip, pipe, ipbIndex, REG::IpbRspecEnum::kChan0Group, std::forward<Ts>(args)...);
    case 1: return REG(chip, pipe, ipbIndex, REG::IpbRspecEnum::kChan1Group, std::forward<Ts>(args)...);
    case 2: return REG(chip, pipe, ipbIndex, REG::IpbRspecEnum::kChan2Group, std::forward<Ts>(args)...);
    case 3: return REG(chip, pipe, ipbIndex, REG::IpbRspecEnum::kChan3Group, std::forward<Ts>(args)...);
    case 4: return REG(chip, pipe, ipbIndex, REG::IpbRspecEnum::kChan4Group, std::forward<Ts>(args)...);
    case 5: return REG(chip, pipe, ipbIndex, REG::IpbRspecEnum::kChan5Group, std::forward<Ts>(args)...);
    case 6: return REG(chip, pipe, ipbIndex, REG::IpbRspecEnum::kChan6Group, std::forward<Ts>(args)...);
    case 7: return REG(chip, pipe, ipbIndex, REG::IpbRspecEnum::kChan7Group, std::forward<Ts>(args)...);
    default: RMT_ASSERT(0);
  }
}

// This still in use by pktgen etc
// TODO: fix when we know how pmarb is sliced
template<class REG,class... Ts> REG parser_adapter(const REG& r, int chip, int pipe, Ts&& ... args) {
  return REG(chip, pipe, std::forward<Ts>(args)...);
}

// TODO: fix when we know how deparsers are sliced
template<class REG,class... Ts> REG deparser_in_hdr_adapter(const REG& r,int chip, int pipe, Ts&& ... args) {
  return REG(chip, pipe, std::forward<Ts>(args)...);
}
// for slices with ingress/egress selector
template<class REG,class... Ts> REG deparser_in_hdr_adapter(const REG& r,int chip, int pipe, int slice, bool egress,
                                                            Ts&& ... args) {
  return REG(chip, pipe, egress ? REG::kHoE : REG::kHoI , slice,
             std::forward<Ts>(args)...);
}

// for registers in dprsr_input_non_pp_ing_and_egr_g group
template<class REG,class... Ts> REG deparser_in_non_pp_adapter(const REG& r,int chip, int pipe, bool egress,
                                                               Ts&& ... args) {
  return REG(chip, pipe, egress ? REG::kEgr : REG::kIngr ,
             std::forward<Ts>(args)...);
}
// this looks the same as deparser_in_hdr_adapter !
template<class REG,class... Ts> REG deparser_slice_r_adapter(const REG& r,int chip, int pipe, int slice, bool egress,
                                                            Ts&& ... args) {
  return REG(chip, pipe, egress ? REG::kHoE : REG::kHoI , slice,
             std::forward<Ts>(args)...);
}

// TODO: fix when we know how deparsers are sliced
template<class REG,class... Ts> REG deparser_out_adapter(const REG& r,int chip, int pipe, Ts&& ... args) {
  return REG(chip, pipe, std::forward<Ts>(args)...);
}


template<class REG,class... Ts> REG* misc_regs_dbg_rst_adapter_new(const REG* r, int chip, int which, Ts&& ... args) {
  return new REG(chip, std::forward<Ts>(args)...);
}


};

#endif
