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

#include <deparser-chip-reg.h>
#include <register_adapters.h>

// Deparser registers that are specific to jbay

namespace MODEL_CHIP_NAMESPACE {

DeparserChipReg::DeparserChipReg(int chip, int pipe) :
    pvt_r_array_{{
      {deparser_in_hdr_adapter(pvt_r_array_[0], chip, pipe, register_classes::DprsrPvtRArrayMutable::kTbl0 )},
      {deparser_in_hdr_adapter(pvt_r_array_[1], chip, pipe, register_classes::DprsrPvtRArrayMutable::kTbl1 )}  }},
    copy_to_cpu_pv_{ deparser_in_hdr_adapter(copy_to_cpu_pv_, chip, pipe) }
{
  Reset();
}

DeparserChipReg::~DeparserChipReg() {
}

void DeparserChipReg::Reset() {
  pvt_r_array_[0].reset();
  pvt_r_array_[1].reset();
  copy_to_cpu_pv_.reset();
}

// 5 bit field, one bit per pipe. Extra bit is for whether there is a 400G port in the group
uint16_t DeparserChipReg::get_multicast_pipe_vector(int table_num, int mgid) {
  RMT_ASSERT( table_num >= 0 && table_num < 2 );
  RMT_ASSERT( mgid >= 0 && mgid <= 0xffff );
  int subword  = mgid % kEntriesPerPvtWord;
  int word     = mgid / kEntriesPerPvtWord;
  return pvt_r_array_[table_num].entry(word, subword);
}

void DeparserChipReg::set_multicast_pipe_vector(int table_num, int mgid, uint16_t val) {
  RMT_ASSERT( table_num >= 0 && table_num < 2 );
  RMT_ASSERT( mgid >= 0 && mgid <= 0xffff );
  int subword  = mgid % kEntriesPerPvtWord;
  int word     = mgid / kEntriesPerPvtWord;
  val &= RmtDefs::kI2qMulticastPipeVectorMask;
  pvt_r_array_[table_num].entry(word, subword, static_cast<uint8_t>(val));
}

uint8_t DeparserChipReg::get_multicast_die_vector(int table_num, int mgid) {
  return 0;  // n/a for jbay
}

uint8_t DeparserChipReg::get_copy_to_cpu_pipe_vector() {
  return copy_to_cpu_pv_.pipe_vec();
}

uint8_t DeparserChipReg::get_copy_to_cpu_die_vector() {
  return 0;  // n/a for jbay
}

uint8_t DeparserChipReg::get_ingress_min_pkt_len(int slice) {
  return 0; // n/a for jbay
}
uint8_t DeparserChipReg::get_egress_min_pkt_len(int slice) {
  return 0; // n/a for jbay
}
uint8_t DeparserChipReg::get_min_pkt_len(int slice, bool ingress) {
  return GLOBAL_ZERO; // n/a for jbay
}


}
