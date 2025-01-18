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

#ifndef _JBAYXX_MEM_UTILS_
#define _JBAYXX_MEM_UTILS_

#include <mem_utils_shared.h>

#define BFN_MEM_TM(y)     0x300000000000ull

#define BFN_MEM_PIPE_PKTGEN_BUFMEM_OFFSET(p)     BFN_MEM_PIPE(parde_pgr_mem_rspec_buffer_mem_word_address)
#define BFN_MEM_PIPE_PKTGEN_BUFMEM_ESZ(p)        BFN_MEM_PIPE(parde_pgr_mem_rspec_buffer_mem_word_array_element_size)
#define BFN_MEM_PIPE_PKTGEN_BUFMEM_CNT(p)        BFN_MEM_PIPE(parde_pgr_mem_rspec_buffer_mem_word_array_count)

#define BFN_MEM_PIPE_PKTGEN_PHASE0META_OFFSET(p) BFN_MEM_PIPE(parde_pgr_ph0_rspec_phase0_mem_word_address)
#define BFN_MEM_PIPE_PKTGEN_PHASE0META_ESZ(p)    BFN_MEM_PIPE(parde_pgr_ph0_rspec_phase0_mem_word_array_element_size)
#define BFN_MEM_PIPE_PKTGEN_PHASE0META_CNT(p)    BFN_MEM_PIPE(parde_pgr_ph0_rspec_phase0_mem_word_array_count)

#define BFN_MEM_TM_PRE_FIFO_ADDR          BFN_MEM_INVAL_MEM_ADDRESS
#define BFN_MEM_TM_PRE_FIFO_ESZ           BFN_MEM_INVAL_ESZ
#define BFN_MEM_TM_PRE_FIFO_CNT           1

#define BFN_MEM_TM_PRE_RDM_SZ             (512*1024)

namespace MODEL_CHIP_NAMESPACE {

  class MemUtils : public MemUtilsShared {

 public:
    static uint64_t pre_mit_mem_address(int i) {
      if (i < 0 || i > 3) return BFN_MEM_INVAL_MEM_ADDRESS;
      return BFN_MEM_TOP(tm_tm_pre_pre_pipe_mem_mit_mem_word_address) +
             i * BFN_MEM_TOP(tm_tm_pre_pre_pipe_mem_array_element_size);
    }
    static constexpr uint64_t pre_mit_mem_size() {
      return 16*1024;
    }
    static constexpr uint64_t pre_pbt_mem_size() {
      return 288;
    }
    static constexpr  uint64_t pre_lit_np_mem_size() {
      return 256;
    }
    static constexpr uint64_t pre_lit_mem_size() {
      return 256;
    }
    static constexpr uint64_t pre_lit_entry_lower_mask() {
      return 0xffffffffffffffff;
    }
    static constexpr uint64_t pre_lit_entry_upper_mask() {
      return 0xff;
    }
    static constexpr uint64_t pre_pmt_mem_size() {
      return 288;
    }
    static constexpr uint64_t pre_lag_mem_size() {
      return 256;
    }
    static constexpr uint64_t pre_lag_mem_entry_size() {
      return 288;
    }
    static constexpr uint64_t pre_yids() {
      return 288;
    }
    static constexpr uint32_t pre_pbt_entry_mask() {
      return 0x1ff;
    }
    static constexpr uint32_t pre_lag_len_mask() {
      return 0x1ff;
    }
    static constexpr uint32_t pre_mit_mem_cnt() {
      return 4;
    }
    static constexpr uint32_t pre_pbt_mem_cnt() {
      return 2;
    }
    static constexpr uint32_t pre_lag_np_mem_cnt() {
      return 2;
    }
    static constexpr uint32_t pre_lag_mem_cnt() {
      return 4;
    }
    static constexpr uint32_t pre_pmt_mem_cnt() {
      return 4;
    }
    static constexpr uint64_t pre_pmt_entry_lower_mask() {
      return 0xffffffffffffffff;
    }
    static constexpr uint64_t pre_pmt_entry_upper_mask() {
      return 0xff;
    }
    static constexpr uint32_t pre_pbt_mem_max_val() {
      return 287;
    }
    static int pre_pbt_mem_entry_port_val(int pbt_entry) {
      return (pbt_entry % 72);
    }
    static int pre_pbt_mem_entry_pipe_val(int pbt_entry) {
      return (pbt_entry / 72);
    }
    static int pre_pbt_mem_entry_chip_val(int pbt_entry) {
      return 0;
    }
    static constexpr uint64_t pre_rdm_l2port64_lower_mask() {
      return 0xffffffffffffffff;
    }
    static constexpr uint64_t pre_rdm_l2port64_upper_mask() {
      return 0xff;
    }
    static uint64_t pktgen_mem_address(const unsigned int p) {
      return pipe_mem_address(p, BFN_MEM_PIPE(parde_pgr_mem_rspec_address));
    }
    static uint64_t pktgen_buffer_mem_word_array_count(unsigned int p) {
      return BFN_MEM_PIPE(parde_pgr_mem_rspec_buffer_mem_word_array_count);
    }
    static uint64_t pktgen_buffer_mem_word_array_element_size(unsigned int p) {
      return BFN_MEM_PIPE(parde_pgr_mem_rspec_buffer_mem_word_array_element_size);
    }
    static uint64_t pktgen_ph0_address(const unsigned int p) {
      return pipe_mem_address(p, BFN_MEM_PIPE(parde_pgr_ph0_rspec_address));
    }
    static uint64_t pktgen_buffer_ph0_word_array_count(unsigned int p) {
      return BFN_MEM_PIPE(parde_pgr_ph0_rspec_phase0_mem_word_array_count);
    }
    static uint64_t pktgen_buffer_ph0_word_array_element_size(unsigned int p) {
      return BFN_MEM_PIPE(parde_pgr_ph0_rspec_phase0_mem_word_array_element_size);
    }
    static constexpr bool pktgen_buffer_ph0_is_byte_swapped = false; // XXX
    static uint64_t pre_lit_mem_address(int i, int word) {
      switch (i) {
      case 0:
        switch (word) {
          case 0: return BFN_MEM_TM_PRE(lit0_bm_mem_word0_address);
          case 1: return BFN_MEM_TM_PRE(lit0_bm_mem_word1_address);
          case 2: return BFN_MEM_TM_PRE(lit0_bm_mem_word2_address);
          case 3: return BFN_MEM_TM_PRE(lit0_bm_mem_word3_address);
          default: return BFN_MEM_INVAL_MEM_ADDRESS;
        }
      case 1:
        switch (word) {
          case 0: return BFN_MEM_TM_PRE(lit1_bm_mem_word0_address);
          case 1: return BFN_MEM_TM_PRE(lit1_bm_mem_word1_address);
          case 2: return BFN_MEM_TM_PRE(lit1_bm_mem_word2_address);
          case 3: return BFN_MEM_TM_PRE(lit1_bm_mem_word3_address);
          default: return BFN_MEM_INVAL_MEM_ADDRESS;
        }
      default: return BFN_MEM_INVAL_MEM_ADDRESS;
      }
    }

    static uint64_t pre_pmt_mem_address(int i, int word) {
      switch (i) {
      case 0:
        switch (word) {
          case 0: return BFN_MEM_TM_PRE(pmt0_mem_word0_address);
          case 1: return BFN_MEM_TM_PRE(pmt0_mem_word1_address);
          case 2: return BFN_MEM_TM_PRE(pmt0_mem_word2_address);
          case 3: return BFN_MEM_TM_PRE(pmt0_mem_word3_address);
          default: return BFN_MEM_INVAL_MEM_ADDRESS;
        }
      case 1:
        switch (word) {
          case 0: return BFN_MEM_TM_PRE(pmt1_mem_word0_address);
          case 1: return BFN_MEM_TM_PRE(pmt1_mem_word1_address);
          case 2: return BFN_MEM_TM_PRE(pmt1_mem_word2_address);
          case 3: return BFN_MEM_TM_PRE(pmt1_mem_word3_address);
          default: return BFN_MEM_INVAL_MEM_ADDRESS;
        }
      default: return BFN_MEM_INVAL_MEM_ADDRESS;
      }
    }

  };
  }

#endif
