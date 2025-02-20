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

#ifndef _SHARED_MEM_UTILS_SHARED_
#define _SHARED_MEM_UTILS_SHARED_

#include <mcn_test.h>
#include <common/rmt-assert.h>

namespace bfn_mem {
  #include <register_includes/mem.h>
}

#if MCN_TEST(MODEL_CHIP_NAMESPACE, tofino) || MCN_TEST(MODEL_CHIP_NAMESPACE, tofinoB0)
#define BFN_MEM_TOP(y)    BFN_CONCAT(bfn_mem::pipe_top_level_,y)
#define BFN_MEM_PIPE(y)   BFN_CONCAT(bfn_mem::pipe_top_level_pipes_,y)
#define BFN_MEM_TM_PRE(y) BFN_CONCAT(bfn_mem::pipe_top_level_tm_pre_,y)

#elif MCN_TEST(MODEL_CHIP_NAMESPACE, jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE, jbayB0)
#define BFN_MEM_TOP(y)    BFN_CONCAT(bfn_mem::jbay_mem_,y)
#define BFN_MEM_PIPE(y)   BFN_CONCAT(bfn_mem::jbay_mem_pipes_,y)
#define BFN_MEM_TM_PRE(y) BFN_CONCAT(bfn_mem::jbay_mem_tm_tm_pre_pre_common_mem_,y)


#else
static_assert(false, "Unexpected MODEL_CHIP_NAMESPACE");
#endif


#define BFN_MEM_INVAL_MEM_ADDRESS         0x300000000000ull
#define BFN_MEM_INVAL_ESZ                 0x10000000ull

#define BFN_MEM_PIPE_ESZ                  BFN_MEM_PIPE(array_element_size)
#define BFN_MEM_PIPE_CNT                  BFN_MEM_PIPE(array_count)
#define BFN_MEM_PIPE_MAU_ESZ              BFN_MEM_PIPE(mau_array_element_size)
#define BFN_MEM_PIPE_MAU_CNT              BFN_MEM_PIPE(mau_array_count)

namespace MODEL_CHIP_NAMESPACE {

class MemUtilsShared {

 public:
  static uint64_t pipe_mem_address(const unsigned int p, uint64_t addr=UINT64_C(0)) {
    // XXX: add GLOBAL_ZERO so that klocwork thinks the min might be >0
    RMT_ASSERT((p >= (BFN_MEM_PIPE(array_index_min)) + GLOBAL_ZERO) && (p <= BFN_MEM_PIPE(array_index_max)));
    if (addr == UINT64_C(0)) addr += BFN_MEM_PIPE(address);
    addr += BFN_MEM_PIPE(array_element_size) * p;
    return addr;
  }

};
}

#endif
