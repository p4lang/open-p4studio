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

// MauInstrStore - Tofino/TofinoB0
// In shared/ because identical across these chips

#ifndef _TOFINOXX_MAU_INSTR_STORE_
#define _TOFINOXX_MAU_INSTR_STORE_

#include <mau-instr-store-common.h>
#include <instr.h>

#include <register_includes/phv_egress_thread_alu_array2.h>
#include <register_includes/phv_ingress_thread_alu_array2.h>
#include <register_includes/imem_subword16_array2_mutable.h>
#include <register_includes/imem_subword32_array2_mutable.h>
#include <register_includes/imem_subword8_array2_mutable.h>

namespace MODEL_CHIP_NAMESPACE {

  class MauInstrStore : public MauInstrStoreCommon {

 public:
    MauInstrStore(int chipIndex, int pipeIndex, int mauIndex, Mau *mau);
    virtual ~MauInstrStore();

 private:
    void imem_subword8_rw_cb(uint32_t phvWord8, uint32_t instrWord, bool write);
    void imem_subword16_rw_cb(uint32_t phvWord16, uint32_t instrWord, bool write);
    void imem_subword32_rw_cb(uint32_t phvWord32, uint32_t instrWord, bool write);

 public:
    inline uint16_t get_phv_ingress_thread_alu(int hilo, int grp, uint16_t dflt) {
      return phv_ingress_thread_alu_.phv_ingress_thread_alu(hilo, grp);
    }
    inline uint16_t get_phv_egress_thread_alu(int hilo, int grp, uint16_t dflt) {
      return phv_egress_thread_alu_.phv_egress_thread_alu(hilo, grp);
    }
    bool imem_read(uint32_t instrWord, uint32_t phvWord,
                   uint32_t *instr, uint8_t *color, uint8_t *parity);
    bool imem_write(uint32_t instrWord, uint32_t phvWord,
                    uint32_t instr, uint8_t color, uint8_t parity);
    bool imem_write_parity(uint32_t instrWord, uint32_t phvWord, uint8_t parity);

 private:
    register_classes::PhvIngressThreadAluArray2   phv_ingress_thread_alu_;
    register_classes::PhvEgressThreadAluArray2    phv_egress_thread_alu_;
    register_classes::ImemSubword8Array2Mutable   imem_subword8_;
    register_classes::ImemSubword16Array2Mutable  imem_subword16_;
    register_classes::ImemSubword32Array2Mutable  imem_subword32_;
  };

}
#endif // _TOFINOXX_MAU_INSTR_STORE_
