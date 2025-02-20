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
#include <rmt-object-manager.h>
#include <rmt-sweeper.h>
#include <mau-op-handler-common.h>


namespace MODEL_CHIP_NAMESPACE {

MauOpHandlerCommon::MauOpHandlerCommon(RmtObjectManager *om,
                                         int pipeIndex, int mauIndex, Mau *mau)
    : MauObject(om, pipeIndex, mauIndex, kType, mau),
      spinlock_(), mau_(mau),
      last_instr_T_(UINT64_C(0)),
      tick_time_base_(UINT64_C(0)), tick_time_prev_(UINT64_C(0)) {
  static_assert( (kTcamsPerMau <= 64), "TCAM bitmask must fit in uint64_t");
}
MauOpHandlerCommon::~MauOpHandlerCommon() {
}

void MauOpHandlerCommon::flush_all_tcam_writeregs() {
  spinlock_.lock();
  uint64_t tcam_bitmask = UINT64_C(0);
  // Go through all TCAMs locking
  // (We'll only successfully lock if the TCAM has a pending write)
  for (int tc = 0; tc < kTcamsPerMau; tc++) {
    MauTcam *tcam = mau_->tcam_lookup(tc);
    bool locked = false;
    if (tcam != NULL) locked = tcam->pending_lock();
    if (locked) tcam_bitmask |= (UINT64_C(1)<<tc);
  }
  // Now that all pending TCAMs are locked
  // go through them flushing pending writes
  for (int tc = 0; tc < kTcamsPerMau; tc++) {
    if ((tcam_bitmask & (UINT64_C(1)<<tc)) != 0) {
      MauTcam *tcam = mau_->tcam_lookup(tc);
      RMT_ASSERT(tcam != NULL);
      tcam->pending_flush();
    }
  }
  // Once we've flushed all pending writes
  // go through all TCAMs and unlock
  for (int tc = 0; tc < kTcamsPerMau; tc++) {
    if ((tcam_bitmask & (1<<tc)) != 0) {
      MauTcam *tcam = mau_->tcam_lookup(tc);
      RMT_ASSERT(tcam != NULL);
      tcam->pending_unlock(true);
    }
  }
  spinlock_.unlock();
}


void MauOpHandlerCommon::set_tcam_writereg(int mem, uint32_t address,
                                           uint64_t data_0, uint64_t data_1,
                                           bool write_tcam) {
  if ((mem < 0) || (mem >= kTcamsPerMau)) return;
  if (address >= kTcamEntries) return;
  int col = mem / kTcamRowsPerMau;
  int row = mem % kTcamRowsPerMau;
  MauTcam *mauTcam = mau_->tcam_get(row, col);
  RMT_ASSERT (mauTcam != NULL);
  mauTcam->pending_set(address, data_0, data_1);
  if (write_tcam) flush_all_tcam_writeregs();
}
void MauOpHandlerCommon::set_tcam_writereg(int row, int col, uint32_t address,
                                           uint64_t data_0, uint64_t data_1,
                                           bool write_tcam) {
  set_tcam_writereg((col*kTcamRowsPerMau)+row, address, data_0, data_1, write_tcam);
}


void MauOpHandlerCommon::tcam_copy_word(int src_table_id, int dst_table_id,
                                        int num_tables,
                                        int num_words, int adr_incr_dir,
                                        uint32_t src_address, uint32_t dst_address) {
  if ((src_table_id < 0) || (src_table_id >= kTcamsPerMau)) return;
  if (src_address >= kTcamEntries) return;
  if ((dst_table_id < 0) || (dst_table_id >= kTcamsPerMau)) return;
  if (dst_address >= kTcamEntries) return;
  if (!((adr_incr_dir == -1) || (adr_incr_dir == 1))) return;

  for (int w = 0; w < num_words; w++) {

    int src_index = src_address + (adr_incr_dir * w);
    int dst_index = dst_address + (adr_incr_dir * w);
    if ((src_index >= 0) && (src_index < kTcamEntries) &&
        (dst_index >= 0) && (dst_index < kTcamEntries)) {

      for (int t = 0; t < num_tables; t++) {
        int src_col = (src_table_id + t) / kTcamRowsPerMau;
        int src_row = (src_table_id + t) % kTcamRowsPerMau;
        int dst_col = (dst_table_id + t) / kTcamRowsPerMau;
        int dst_row = (dst_table_id + t) % kTcamRowsPerMau;
        MauTcam *src_tcam = mau_->tcam_get(src_row, src_col);
        MauTcam *dst_tcam = mau_->tcam_get(dst_row, dst_col);
        if ((src_tcam != NULL) && (dst_tcam != NULL)) {
          // Read src tcam and write value into dst pending writereg
          uint64_t data0 = 0;
          uint64_t data1 = 0;
          src_tcam->read(src_index, &data0, &data1, UINT64_C(0));
          dst_tcam->pending_set(dst_index, data0, data1);
        }
      }
      // Then flush written TCAMs out atomically
      flush_all_tcam_writeregs();
    }
  }
}
void MauOpHandlerCommon::tcam_copy_word(int src_col, int dst_col,
                                        int bot_row, int top_row,
                                        int num_words, int adr_incr_dir,
                                        uint32_t src_address, uint32_t dst_address,
                                        uint64_t T) {
  if ((src_col < 0) || (src_col >= kTcamColumnsPerMau)) return;
  if (src_address >= kTcamEntries) return;
  if ((dst_col < 0) || (dst_col >= kTcamColumnsPerMau)) return;
  if (dst_address >= kTcamEntries) return;
  if (!((adr_incr_dir == -1) || (adr_incr_dir == 1))) return;
  if ((src_col == dst_col) && (src_address == dst_address)) return;
  if (num_words <= 0) return;

  // Increase src/dst addr by kTcamEntries to handle negative adr_incr_dir,
  // catering for case where we wrap around bottom of TCAM
  int src_addr = static_cast<int>(src_address + kTcamEntries);
  int dst_addr = static_cast<int>(dst_address + kTcamEntries);

  for (int w = 0; w < num_words; w++) {
    int src_index = (src_addr + (adr_incr_dir * w)) % kTcamEntries;
    int dst_index = (dst_addr + (adr_incr_dir * w)) % kTcamEntries;

    for (int row = bot_row; row <= top_row; row++) {
      MauTcam *src_tcam = mau_->tcam_get(row, src_col);
      MauTcam *dst_tcam = mau_->tcam_get(row, dst_col);
      if ((src_tcam != NULL) && (dst_tcam != NULL)) {
        // Read src tcam and write value into dst pending writereg
        uint64_t data0 = 0;
        uint64_t data1 = 0;
        src_tcam->read(src_index, &data0, &data1, T);
        dst_tcam->pending_set(dst_index, data0, data1);
      }
    }
    // Then flush written TCAMs out atomically
    flush_all_tcam_writeregs();
  }
}
void MauOpHandlerCommon::barrier_lock(int table, uint8_t lock_type, uint16_t lock_id) {
  if ((table < 0) || (table >= kLogicalTables)) return;
  RmtObjectManager *om = mau_->get_object_manager();
  RmtSweeper *sweeper = om->sweeper_get();
  int pipe = mau_->pipe_index();
  int stage = mau_->mau_index();
  uint8_t bflags = 0;
  bool barrier = false;
  bool stats_lock = false,    idle_lock = false;
  bool stats_unlock = false,  idle_unlock = false;

  // This seems a needlessly redundant encoding!
  switch (lock_type) {
    case 0: barrier = true; bflags |= RmtSweeper::kFlagsBarrierStats; break;
    case 1: barrier = true; bflags |= RmtSweeper::kFlagsBarrierIdle;  break;
    case 2: stats_lock = true;   idle_lock = true;   break;
    case 3: stats_unlock = true; idle_unlock = true; break;
    case 4: stats_lock = true;   break;
    case 5: stats_unlock = true; break;
    case 6: idle_lock = true;    break;
    case 7: idle_unlock = true;  break;
  }
  if (barrier)      sweeper->barrier(pipe, stage, table, lock_id, bflags);
  if (stats_lock)   sweeper->stats_lock(pipe, stage, table, lock_id);
  if (idle_lock)    sweeper->idle_lock(pipe, stage, table, lock_id);
  if (stats_unlock) sweeper->stats_unlock(pipe, stage, table, lock_id);
  if (idle_unlock)  sweeper->idle_unlock(pipe, stage, table, lock_id);
}
void MauOpHandlerCommon::dump_idle_word(int table, bool clear, uint32_t idle_addr) {
  if ((table < 0) || (table >= kLogicalTables)) return;
  RmtObjectManager *om = mau_->get_object_manager();
  RmtSweeper *sweeper = om->sweeper_get();
  int pipe = mau_->pipe_index();
  int stage = mau_->mau_index();
  sweeper->idle_dump_word(pipe, stage, table, idle_addr, clear);
}
void MauOpHandlerCommon::dump_idle_table(int table, bool clear) {
  if ((table < 0) || (table >= kLogicalTables)) return;
  RmtObjectManager *om = mau_->get_object_manager();
  RmtSweeper *sweeper = om->sweeper_get();
  int pipe = mau_->pipe_index();
  int stage = mau_->mau_index();
  sweeper->idle_dump(pipe, stage, table, clear);
}
void MauOpHandlerCommon::dump_stats_word(int table, uint32_t stats_addr) {
  if ((table < 0) || (table >= kLogicalTables)) return;
  RmtObjectManager *om = mau_->get_object_manager();
  RmtSweeper *sweeper = om->sweeper_get();
  int pipe = mau_->pipe_index();
  int stage = mau_->mau_index();
  sweeper->stats_dump_word(pipe, stage, table, stats_addr);
}
void MauOpHandlerCommon::dump_stats_table(int table) {
  if ((table < 0) || (table >= kLogicalTables)) return;
  RmtObjectManager *om = mau_->get_object_manager();
  RmtSweeper *sweeper = om->sweeper_get();
  int pipe = mau_->pipe_index();
  int stage = mau_->mau_index();
  sweeper->stats_dump(pipe, stage, table);
}
void MauOpHandlerCommon::run_stateful_instruction(int table, int instr, uint32_t addr,
                                                  uint64_t data0, uint64_t data1,
                                                  uint64_t T) {
  int op = Address::kMeterOp4Nop;
  switch (instr) {
    case 0x0: op = Address::kMeterOp4CfgSaluInst0; break;
    case 0x1: op = Address::kMeterOp4CfgSaluInst1; break;
    case 0x2: op = Address::kMeterOp4CfgSaluInst2; break;
    case 0x3: op = Address::kMeterOp4CfgSaluInst3; break;
    default: RMT_ASSERT(0); break;
  }
  uint32_t meter_addr = Address::meter_addr_make(addr, op);
  // TODO:T: Temporary logic till DV swaps over to using explicit T value
  // NB. Data0 carries meter_addr in this instr so time in *data1* (or T)
  uint64_t Tdata = (kZeroisePushPopData) ?UINT64_C(0) :data1;
  uint64_t Trsi = (T > Tdata) ?T :Tdata;
  // Maybe move on stateful time - primarily for Bloom clear
  if (mau_->mau_stateful_counters()->is_counter_for_lt_being_cleared(table))
    mau_->mau_stateful_counters()->advance_time(table, Trsi, "RunStatefulInstruction", meter_addr);

  // Call pbus_write_op to deliver stateful instr (encoded in address) to Stateful ALU
  mau_->pbus_write_op(AddrType::kStateful, table, meter_addr,
                      UINT64_C(0), UINT64_C(0), true, Trsi);
}
void MauOpHandlerCommon::push_table_move_addr(int table, int s_addr,
                                              uint64_t data0, uint64_t data1,
                                              uint64_t T) {
  MauMoveregsCtl *movereg_ctl = mau_->mau_moveregs_ctl();
  // TODO:T: Temporary logic till DV swaps over to using explicit T value
  uint64_t Tdata = (kZeroisePushPopData) ?UINT64_C(0) :data0;
  uint64_t Tpush = (T > Tdata) ?T :Tdata;
  movereg_ctl->push_table_move_addr(table, s_addr, Tpush);
}
void MauOpHandlerCommon::pop_table_move_addr(int table,
                                             bool idle_init, bool stats_init_ones,
                                             uint64_t data0, uint64_t data1,
                                             uint64_t T) {
  MauMoveregsCtl *movereg_ctl = mau_->mau_moveregs_ctl();
  // TODO:T: Temporary logic till DV swaps over to using explicit T value
  uint64_t Tdata = (kZeroisePushPopData) ?UINT64_C(0) :data0;
  uint64_t Tpop = (T > Tdata) ?T :Tdata;
  movereg_ctl->pop_table_move_addr(table, idle_init, stats_init_ones, Tpop);
}
void MauOpHandlerCommon::set_meter_time(uint64_t data0, uint64_t data1, uint64_t T) {
  uint64_t tick_time = data0;
  if (tick_time < tick_time_prev_) {
    // Time gone backward - complain and fix
    // (add 1000 ticks extra so we advance past any queue delay)
    tick_time_base_ += tick_time_prev_;
    tick_time_base_ += UINT64_C(1000);
    RMT_LOG_OBJ(mau_, RmtDebug::error(),
                "MauOpHandlerCommon::instr_set_meter_time: Time gone backwards! "
                "time_prev=%" PRIu64 " time_now=%" PRIu64 " flushing color Qs\n",
                // " time_fixed=%" PRIu64 "\n",
                tick_time_prev_, tick_time);
    // Rather than futzing with T just flush color queues in ALL MAUs this pipe
    if (mau_->mau_index() == 0) {
      RmtObjectManager *om = mau_->get_object_manager();
      for (int stage = 0; stage < RmtDefs::kStagesMax; stage++) {
        Mau *mau = om->mau_lookup(mau_->pipe_index(), stage);
        if (mau != NULL) mau->flush_color_queues();
      }
    }
  }
  tick_time_prev_ = tick_time;
  //tick_time += tick_time_base_; // tick_time_base_==0 unless time gone backwards
  RmtObjectManager *om = mau_->get_object_manager();
  Pipe*  pipe = om->pipe_get(mau_->pipe_index(), -1);
  pipe->setup_time_info(tick_time);
}

void MauOpHandlerCommon::extra_func_demux(int instr, int opsiz, int datasiz,
                                          uint64_t data0, uint64_t data1,
                                          uint64_t T) {
  // Exploit unused op ism_init
  RMT_ASSERT(((instr >> 26) & 3) == 1);
  uint8_t extra_op = (instr >> 23) & 7;
  switch (extra_op) {
    case 4: set_meter_time(data0, data1, T); break;
  }
  return;
}



void MauOpHandlerCommon::instr_pipering_inst01(int instr, int opsiz, int datasiz,
                                               uint64_t data0, uint64_t data1,
                                               uint64_t T) {
  RMT_ASSERT(!atomic_in_progress());
  // Don't abuse this func - used by drivers
}
void MauOpHandlerCommon::instr_push_table_move_adr(int instr, int opsiz, int datsiz,
                                                   uint64_t data0, uint64_t data1,
                                                   uint64_t T) {
  RMT_ASSERT(!atomic_in_progress());
  uint8_t table = static_cast<uint8_t>((instr >> 20) & 0xF);
  int s_addr = ((instr >> 0) & 0x7FFFF);
  push_table_move_addr(table, s_addr, data0, data1, T);
}
void MauOpHandlerCommon::instr_pop_table_move_adr(int instr, int opsiz, int datsiz,
                                                  uint64_t data0, uint64_t data1,
                                                  uint64_t T) {
  RMT_ASSERT(!atomic_in_progress());
  uint8_t table = static_cast<uint8_t>((instr >> 0) & 0xF);
  bool idle_init = (((instr >> 4) & 1) == 1);
  bool stats_init_ones = (((instr >> 5) & 1) == 1);
  pop_table_move_addr(table, idle_init, stats_init_ones, data0, data1, T);
}
void MauOpHandlerCommon::instr_ism_init(int instr, int opsiz, int datsiz,
                                        uint64_t data0, uint64_t data1,
                                        uint64_t T) {
  RMT_ASSERT(!atomic_in_progress());
  // Maybe abuse this func to do other stuff
  extra_func_demux(instr, opsiz, datsiz, data0, data1, T);
}
void MauOpHandlerCommon::instr_nop(int instr, int opsiz, int datsiz,
                                   uint64_t data0, uint64_t data1,
                                   uint64_t T) {
  RMT_ASSERT(!atomic_in_progress());
  // Don't abuse - always a NOP
  set_last_instr_time(T); // But allow side-effect of updating a stashed time val
}
void MauOpHandlerCommon::instr_barrier_lock(int instr, int opsiz, int datsiz,
                                            uint64_t data0, uint64_t data1,
                                            uint64_t T) {
  RMT_ASSERT(!atomic_in_progress());
  uint8_t table = static_cast<uint8_t>((instr >> 19) & 0xF);
  uint8_t lock_type = static_cast<uint8_t>((instr >> 16) & 0x7);
  uint16_t lock_id = static_cast<uint16_t>((instr >> 0) & 0xFFFF);
  barrier_lock(table, lock_type, lock_id);
}
void MauOpHandlerCommon::instr_dump_stats_word(int instr, int opsiz, int datsiz,
                                               uint64_t data0, uint64_t data1,
                                               uint64_t T) {
  RMT_ASSERT(!atomic_in_progress());
  uint8_t table = static_cast<uint8_t>((instr >> 19) & 0xF);
  uint32_t stats_addr = ((instr >> 0) & 0x7FFFF);
  dump_stats_word(table, stats_addr);
}
void MauOpHandlerCommon::instr_dump_idle_word(int instr, int opsiz, int datsiz,
                                              uint64_t data0, uint64_t data1,
                                              uint64_t T) {
  RMT_ASSERT(!atomic_in_progress());
  uint8_t table = static_cast<uint8_t>((instr >> 17) & 0xF);
  bool clear = (((instr >> 16) & 0x1) == 0x1);
  uint32_t idle_addr = ((instr >> 0) & 0xFFFF);
  dump_idle_word(table, clear, idle_addr);
}
void MauOpHandlerCommon::instr_set_tcam_writereg(int instr, int opsiz, int datsiz,
                                                 uint64_t data0, uint64_t data1,
                                                 uint64_t T) {
  RMT_ASSERT(!atomic_in_progress());
  int row = (instr >> 11) & 0xF;
  int col = (instr >> 10) & 0x1;
  int write_tcam = (instr >> 9) & 0x1;
  int address = (instr >> 0) & 0x1FF;
  set_tcam_writereg(row, col, address, data0, data1, write_tcam);
}
void MauOpHandlerCommon::instr_tcam_copy_word(int instr, int opsiz, int datsiz,
                                              uint64_t data0, uint64_t data1,
                                              uint64_t T) {
  RMT_ASSERT(!atomic_in_progress());
  int adr_incr_dir = (((instr >> 9) & 0x1) == 0x1) ?1 :-1;
  int num_words = (instr >> 0) & 0x1FF;
  int src_col = static_cast<int>((data0 >> 27) & UINT64_C(0x1));
  int dst_col = static_cast<int>((data0 >> 26) & UINT64_C(0x1));
  int bot_row = static_cast<int>((data0 >> 22) & UINT64_C(0xF));
  int top_row = static_cast<int>((data0 >> 18) & UINT64_C(0xF));
  uint32_t src_addr = static_cast<int>((data0 >> 9) & UINT64_C(0x1FF));
  uint32_t dst_addr = static_cast<int>((data0 >> 0) & UINT64_C(0x1FF));
  tcam_copy_word(src_col, dst_col, bot_row, top_row,
                 num_words, adr_incr_dir, src_addr, dst_addr, T);
}
void MauOpHandlerCommon::instr_run_stateful_instruction(int instr, int opsiz, int datsiz,
                                                        uint64_t data0, uint64_t data1,
                                                        uint64_t T) {
  RMT_ASSERT(!atomic_in_progress());
  uint8_t table = static_cast<uint8_t>((instr >> 2) & 0xF);
  uint8_t stateful_instr = static_cast<uint8_t>((instr >> 0) & 0x3);
  uint32_t stateful_addr = static_cast<uint32_t>(data0 & 0x7FFFFF);
  run_stateful_instruction(table, stateful_instr, stateful_addr, data0, data1, T);
}
void MauOpHandlerCommon::instr_dump_idle_table(int instr, int opsiz, int datsiz,
                                               uint64_t data0, uint64_t data1,
                                               uint64_t T) {
  RMT_ASSERT(!atomic_in_progress());
  uint8_t table = static_cast<uint8_t>((instr >> 0) & 0xF);
  bool clear = (((instr >> 4) & 0x1) == 0x1);
  dump_idle_table(table, clear);
}
void MauOpHandlerCommon::instr_dump_stats_table(int instr, int opsiz, int datsiz,
                                                uint64_t data0, uint64_t data1,
                                                uint64_t T) {
  RMT_ASSERT(!atomic_in_progress());
  uint8_t table = static_cast<uint8_t>((instr >> 0) & 0xF);
  dump_stats_table(table);
}


void MauOpHandlerCommon::instr_handle(int instr, int data_size,
                                      uint64_t data0, uint64_t data1, uint64_t T) {
  RMT_ASSERT((instr >> 28) == 0);
  switch (instr >> 26) {
    case 0x3: instr_pipering_inst01(instr, 1, data_size, data0, data1, T); break;
    case 0x2: instr_push_table_move_adr(instr, 26, data_size, data0, data1, T); break;
    case 0x1: instr_ism_init(instr, 26, data_size, data0, data1, T); break;
    case 0x0:
      switch (instr >> 23) {
        case 0x0: instr_nop(instr, 0, data_size, data0, data1, T); break;
        case 0x1: instr_barrier_lock(instr, 23, data_size, data0, data1, T); break;
        case 0x2: instr_dump_stats_word(instr, 23, data_size, data0, data1, T); break;
        case 0x3:
          switch (instr >> 21) {
            case 0xC: instr_dump_idle_word(instr, 21, data_size, data0, data1, T); break;
            case 0xD: instr_set_tcam_writereg(instr, 15, data_size, data0, data1, T); break;
            case 0xE: instr_handle_perchip(instr, data_size, data0, data1, T); break;
            case 0xF:
              switch (instr >> 19) {
                case 0x3C: instr_pop_table_move_adr(instr, 6, data_size, data0, data1, T); break;
                case 0x3D: instr_run_stateful_instruction(instr, 6, data_size, data0, data1, T); break;
                case 0x3E: instr_dump_idle_table(instr, 5, data_size, data0, data1, T); break;
                case 0x3F: instr_dump_stats_table(instr, 4, data_size, data0, data1, T); break;
              }
              break;
          }
          break;
        case 0x4: case 0x5: case 0x6: case 0x7:
          instr_handle_perchip(instr, data_size, data0, data1, T); break;
      }
      break;
  }
}

}
