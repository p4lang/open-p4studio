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

#ifndef _SHARED_MAU_OP_HANDLER_COMMON_
#define _SHARED_MAU_OP_HANDLER_COMMON_

#include <memory>
#include <atomic>
#include <rmt-defs.h>
#include <mau-object.h>


namespace MODEL_CHIP_NAMESPACE {

  class Mau;

  class MauOpHandlerCommon : public MauObject {

 public:
    static constexpr int  kTcamsPerMau  = MauDefs::kTcamsPerMau;
    static constexpr int  kTcamRowsPerMau  = MauDefs::kTcamRowsPerMau;
    static constexpr int  kTcamColumnsPerMau  = MauDefs::kTcamColumnsPerMau;
    static constexpr int  kTcamAddressWidth  = MauDefs::kTcamAddressWidth;
    static constexpr int  kTcamEntries = 1<<kTcamAddressWidth;
    static constexpr int  kTcamWidth = MauDefs::kTcamWidth;
    static constexpr int  kSramRowsPerMau  = MauDefs::kSramRowsPerMau;
    static constexpr int  kSramColumnsPerMau  = MauDefs::kSramColumnsPerMau;
    static constexpr int  kLogicalTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int  kType = RmtTypes::kRmtTypeMauOpHandler;

    static bool kZeroisePushPopData; // Defined in rmt-config.cpp
    static bool kAllowAtomicWideBubbles;

    MauOpHandlerCommon(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    ~MauOpHandlerCommon();


    inline uint64_t get_last_instr_time()           { return last_instr_T_; }
    inline void     set_last_instr_time(uint64_t T) { last_instr_T_ = T;    }

    void flush_all_tcam_writeregs();
    void set_tcam_writereg(int mem, uint32_t address,
                           uint64_t data_0, uint64_t data_1, bool write_tcam=true);
    void set_tcam_writereg(int row, int col, uint32_t address,
                           uint64_t data_0, uint64_t data_1, bool write_tcam);

    void tcam_copy_word(int src_table_id, int dst_table_id, int num_tables,
                        int num_words, int adr_incr_dir,
                        uint32_t src_address, uint32_t dst_address);
    void tcam_copy_word(int src_col, int dst_col, int bot_row, int top_row,
                        int num_words, int adr_incr_dir,
                        uint32_t src_address, uint32_t dst_address, uint64_t T);
    void barrier_lock(int table, uint8_t lock_type, uint16_t lock_id);
    void dump_idle_word(int table, bool clear, uint32_t idle_addr);
    void dump_idle_table(int table, bool clear);
    void dump_stats_word(int table, uint32_t stats_addr);
    void dump_stats_table(int table);
    void run_stateful_instruction(int table, int instr, uint32_t addr,
                                  uint64_t data0, uint64_t data1, uint64_t T);
    void push_table_move_addr(int table, int s_addr,
                              uint64_t data0, uint64_t data1, uint64_t T);
    void pop_table_move_addr(int table, bool idle_init, bool stats_init_ones,
                             uint64_t data0, uint64_t data1, uint64_t T);


    void set_meter_time(uint64_t data0, uint64_t data1, uint64_t T);
    void extra_func_demux(int instr, int opsiz, int datasiz,
                          uint64_t data0, uint64_t data1, uint64_t T);



    // These not implemented initially
    //
    void set_memdata(int pipe, int stage, int mem, uint32_t address,
                     uint8_t data_size, uint64_t data_0, uint64_t data_1) { }

    void tcam_invalidate_word(int pipe, int stage,
                              int table_id, int num_tables,
                              uint32_t src_address) { }

    void ism_init(int pipe, int stage,
                  int table_id, uint32_t v_address,
                  bool init, bool srcloc_uses_idletime,
                  bool srcloc_uses_stats, bool unlock) { }

    // Couldn't find these - changed?
    //
    // (set_table_qualif_reg  pipe stage table_id dm_address ????
    // (set_internal_version  pipe stage table_id ????
    // (disable_table_qualif_reg ????



    void instr_pipering_inst01(int instr, int opsiz, int datasiz,
                               uint64_t data0, uint64_t data1, uint64_t T);
    void instr_push_table_move_adr(int instr, int opsiz, int datsiz,
                                   uint64_t data0, uint64_t data1, uint64_t T);
    void instr_pop_table_move_adr(int instr, int opsiz, int datsiz,
                                  uint64_t data0, uint64_t data1, uint64_t T);
    void instr_ism_init(int instr, int opsiz, int datsiz,
                        uint64_t data0, uint64_t data1, uint64_t T);
    void instr_nop(int instr, int opsiz, int datsiz,
                   uint64_t data0, uint64_t data1, uint64_t T);
    void instr_barrier_lock(int instr, int opsiz, int datsiz,
                            uint64_t data0, uint64_t data1, uint64_t T);
    void instr_dump_stats_word(int instr, int opsiz, int datsiz,
                               uint64_t data0, uint64_t data1, uint64_t T);
    void instr_dump_idle_word(int instr, int opsiz, int datsiz,
                              uint64_t data0, uint64_t data1, uint64_t T);
    void instr_set_tcam_writereg(int instr, int opsiz, int datsiz,
                                 uint64_t data0, uint64_t data1, uint64_t T);
    void instr_tcam_copy_word(int instr, int opsiz, int datsiz,
                              uint64_t data0, uint64_t data1, uint64_t T);
    void instr_run_stateful_instruction(int instr, int opsiz, int datsiz,
                                        uint64_t data0, uint64_t data1, uint64_t T);
    void instr_dump_idle_table(int instr, int opsiz, int datsiz,
                               uint64_t data0, uint64_t data1, uint64_t T);
    void instr_dump_stats_table(int instr, int opsiz, int datsiz,
                                uint64_t data0, uint64_t data1, uint64_t T);


    // Generic handler for PBUS instructions
    void instr_handle(int instr, int data_size, uint64_t data0, uint64_t data1, uint64_t T);

    // And PER-CHIP specialization
    virtual bool atomic_in_progress() { return false; }
    virtual void instr_handle_perchip(int instr, int data_size,
                                      uint64_t data0, uint64_t data1, uint64_t T) = 0;
    model_core::Spinlock spinlock_;

 private:
    Mau                 *mau_;
    uint64_t             last_instr_T_;
    uint64_t             tick_time_base_;
    uint64_t             tick_time_prev_;
  };
}

#endif // _SHARED_MAU_OP_HANDLER_
