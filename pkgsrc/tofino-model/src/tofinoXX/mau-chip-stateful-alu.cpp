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
#include <address.h>
#include <mau-chip-stateful-alu.h>
#include <mau-info.h>
#include <register_adapters.h>

// Chip specific Stateful ALU code - this is default version used for Tofino
//   and TofinoB0

namespace MODEL_CHIP_NAMESPACE {

MauChipStatefulAlu::MauChipStatefulAlu(RmtObjectManager *om, int pipeIndex,
                                       int mauIndex, int logicalRowIndex,
                                       int alu_index, Mau *mau)
    : MauObject(om, pipeIndex, mauIndex, kType, logicalRowIndex, mau),
      alu_index_(alu_index),
      out_reg_(default_adapter(out_reg_,chip_index(), pipeIndex, mauIndex, alu_index_))
{
  out_reg_.reset();
  reset_resources();
}

MauChipStatefulAlu::~MauChipStatefulAlu() { }

void MauChipStatefulAlu::reset_resources() { }

void MauChipStatefulAlu::get_a(
    register_classes::SaluInstrStateAluArray2& state_reg,
    bool alu_1or2, uint64_t phv_hi,
    uint64_t phv_lo, uint64_t ram_hi, uint64_t ram_lo,
    uint64_t c,int width,uint64_t mask,
    int instr_addr,int alu_addr,
    bool is_run_stateful,
    uint64_t* a, int64_t* as) {
  // These inputs are all 32bits
  ram_hi &= 0xFFFFFFFF;
  ram_lo &= 0xFFFFFFFF;
  bool a_is_mem = state_reg.salu_asrc_memory(instr_addr, alu_addr);
  if (a_is_mem) {
    bool ram_hilo = state_reg.salu_asrc_memory_index(instr_addr, alu_addr);
    *a = (ram_hilo ? ram_hi : ram_lo) & mask;
    *as = sign_ext64(*a, width);
  } else {
    *a = c;
    *as = c;
  }
}

void MauChipStatefulAlu::get_b(
    register_classes::SaluInstrStateAluArray2& state_reg,
    bool hilo, bool alu_1or2, uint64_t phv_hi,
    uint64_t phv_lo, uint64_t ram_hi, uint64_t ram_lo,
    uint64_t c,int width,uint64_t mask,
    int instr_addr,int alu_addr,
    bool is_run_stateful,
    uint64_t* b, int64_t* bs) {
    // These inputs are all 32bits
    phv_hi &= 0xFFFFFFFF;
    phv_lo &= 0xFFFFFFFF;
    bool b_is_phv = state_reg.salu_bsrc_phv(instr_addr, alu_addr);

    if (b_is_phv) {
      RMT_ASSERT( ! is_run_stateful ); // there is no phv for run stateful
      bool phv_hilo = state_reg.salu_bsrc_phv_index(instr_addr, alu_addr);
      *b = (phv_hilo ? phv_hi : phv_lo) & mask;
      *bs = sign_ext64(*b, width);
    } else {
      *b = c;
      *bs = c;
    }
}

}
