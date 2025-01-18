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

#ifndef _JBAY_DEPARSER_PERF_COUNT_ADAPTER_SHARED_H_
#define _JBAY_DEPARSER_PERF_COUNT_ADAPTER_SHARED_H_


namespace MODEL_CHIP_NAMESPACE {

// wrapper for real registers to hide difference between jbay and WIP perf
// counter time registers: jbay uses different register types for pkt and byte
// time snapshots
template<typename REG>
class DeparserPerfTimeAdapter {
 public:
  // register_adapters expects this class to have the gress enum so define here
  static const typename REG::DprsrRegRspecEnum kHoE = REG::kHoE;
  static const typename REG::DprsrRegRspecEnum kHoI = REG::kHoI;

  DeparserPerfTimeAdapter(
      int chipNumber,
      int index_pipe_addrmap,
      typename REG::DprsrRegRspecEnum selector_dprsr_reg_rspec,
      int index_dprsr_ho_e,
      RegisterCallback &write_callback = nullptr,
      RegisterCallback &read_callback = nullptr)
      : reg_(chipNumber,
             index_pipe_addrmap,
             selector_dprsr_reg_rspec,
             index_dprsr_ho_e,
             write_callback,
             read_callback) {}

  uint64_t count() { return reg_.count(); }
  void count(const uint64_t &v) { reg_.count(v); }
  void reset() { reg_.reset(); }

 private:
  REG reg_;
};

}

#endif //_JBAY_DEPARSER_PERF_COUNT_ADAPTER_SHARED_H_
