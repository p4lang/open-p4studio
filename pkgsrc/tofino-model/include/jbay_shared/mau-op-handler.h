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

#ifndef _JBAY_SHARED_MAU_OP_HANDLER_
#define _JBAY_SHARED_MAU_OP_HANDLER_

#include <memory>
#include <atomic>
#include <rmt-defs.h>
#include <common/rmt-util.h>
#include <mau-op-handler-common.h>

#include <register_includes/atomic_mod_sram_go_pending_array_mutable.h>
#include <register_includes/atomic_mod_tcam_go_mutable.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauAtomicWrites;

  class MauOpHandler : public MauOpHandlerCommon {

 public:
    static constexpr int kNumAtomicCsrWrites = 64;

    MauOpHandler(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    virtual ~MauOpHandler();


    void push_pop_stateful(bool push, uint8_t cntr, uint32_t incr,
                           uint64_t data0, uint64_t data1, uint64_t T);
    void instr_push_pop_stateful(int instr, int opsiz, int datasiz,
                                 uint64_t data0, uint64_t data1, uint64_t T);

    // Some static funcs to decode atomic instruction flags
    static inline bool atomic_mod_wide_bubble(uint8_t f)    { return ((f & 4) == 4); }
    static inline bool atomic_mod_narrow_bubble(uint8_t f)  { return ((f & 4) == 0); }
    static inline bool atomic_mod_end(uint8_t f)            { return ((f & 2) == 2); }
    static inline bool atomic_mod_begin(uint8_t f)          { return ((f & 2) == 0); }
    static inline bool atomic_mod_egress(uint8_t f)         { return ((f & 1) == 1); }
    static inline bool atomic_mod_ingress(uint8_t f)        { return ((f & 1) == 0); }

    void atomic_mod_sram(bool ingress, uint8_t flags,
                         uint64_t data0, uint64_t data1, uint64_t T);
    void instr_atomic_mod_sram(int instr, int opsiz, int datasiz,
                               uint64_t data0, uint64_t data1, uint64_t T);

    // Atomic Write FSM
    static constexpr uint8_t kAtomicStateReset  = 0; // Initial state
    static constexpr uint8_t kAtomicStateBegun  = 1; // BEGIN called CSR writes captured
    static constexpr uint8_t kAtomicStateEnding = 2; // END called, replaying writes
    static constexpr uint8_t kAtomicStateEnded  = 3; // END called, write replay done

    void dispatch_atomic_writes(bool ingress);
    void dispatch_atomic_writes_this_mau(bool ingress);
    void dispatch_atomic_writes_all_maus(bool ingress);
    void atomic_mod_csr(bool ingress, uint8_t flags,
                        uint64_t data0, uint64_t data1, uint64_t T);
    void instr_atomic_mod_csr(int instr, int opsiz, int datasiz,
                              uint64_t data0, uint64_t data1, uint64_t T);

    inline bool atomic_in_progress() { return (atomic_state_ == kAtomicStateBegun); }
    void instr_handle_perchip(int instr, int data_size,
                              uint64_t data0, uint64_t data1, uint64_t T);

 private:
    DISALLOW_COPY_AND_ASSIGN(MauOpHandler);
    void atomic_mod_sram_go_cb(int ie);
    void atomic_mod_tcam_go_cb();
    void atomic_read_cb();
    void atomic_write_cb();

 private:
    bool                                                   ctor_running_;
    uint8_t                                                atomic_state_;
    bool                                                   atomic_thread_;
    MauAtomicWrites                                       *atomic_writes_;
    register_classes::AtomicModSramGoPendingArrayMutable   atomic_mod_sram_go_pending_;
    register_classes::AtomicModTcamGoMutable               atomic_mod_tcam_go_;
  };

}

#endif // _JBAY_SHARED_MAU_OP_HANDLER_
