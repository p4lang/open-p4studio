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

#ifndef _JBAYXX_DEPARSER_CHIP_REG_
#define _JBAYXX_DEPARSER_CHIP_REG_

#include <register_includes/dprsr_pvt_r_array_mutable.h>
#include <register_includes/dprsr_copy_to_c_p_u_p_v_r.h>

// Deparser registers that are specific to jbay

namespace MODEL_CHIP_NAMESPACE {

class DeparserChipReg {

 public:
  DeparserChipReg(int chip, int pipe);
  virtual ~DeparserChipReg();
  void Reset();
  uint16_t get_multicast_pipe_vector(int table_num, int mgid);
  void set_multicast_pipe_vector(int table_num, int mgid, uint16_t val);
  uint8_t get_multicast_die_vector(int table_num, int mgid);
  uint8_t get_copy_to_cpu_pipe_vector();
  uint8_t get_copy_to_cpu_die_vector();
  uint8_t get_ingress_min_pkt_len(int slice);
  uint8_t get_egress_min_pkt_len(int slice);
  uint8_t get_min_pkt_len(int slice, bool ingress);


  static constexpr int kEntriesPerPvtWord = 4;

 private:
  std::array<register_classes::DprsrPvtRArrayMutable, 2> pvt_r_array_;     // tbl0, tbl1
  register_classes::DprsrCopyToCPUPVR             copy_to_cpu_pv_;

};
}

#endif // _JBAYXX_DEPARSER_CHIP_REG_
