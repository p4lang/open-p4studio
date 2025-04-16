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
#include <memory>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <rmt-sweeper.h>
#include <model_core/model.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_NAMESPACE {


void TableOpCtl::lock_wait() {
  bool locked_by_me = false;
  while (!locked_by_me) {
    wait_stopped(); // Wait till no OPS running
    spinlock_.lock();
    if (notrunning()) {
      inc_lock(); // Take lock
      locked_by_me = true;
    }
    spinlock_.unlock();
  }
}
void TableOpCtl::unlock_signal() {
  if (unlocked()) return;
  bool signal = false;
  spinlock_.lock();
  if (locked()) {
    dec_lock(); // Release lock
    signal = unlocked();
  }
  spinlock_.unlock();
  if (signal) do_signal();
}
void TableOpCtl::op_start() {
  bool run_by_me = false;
  while (!run_by_me) {
    wait_unlocked(); // Wait till no lock held
    spinlock_.lock();
    if (unlocked()) {
      inc_run(); // Set running
      run_by_me = true;
    }
    spinlock_.unlock();
  }
}
void TableOpCtl::op_stop() {
  RMT_ASSERT(running());
  bool signal = false;
  spinlock_.lock();
  if (running()) {
    dec_run();
    signal = notrunning();
  }
  spinlock_.unlock();
  if (signal) do_signal();
}
bool TableOpCtl::is_locked_else_op_start() {
  spinlock_.lock();
  bool is_locked = locked();
  if (!is_locked) inc_run();
  spinlock_.unlock();
  return is_locked;
}


void TableInfo::idle_set_sweep_interval(RmtSweeper *sweeper, uint8_t interval) {
  // Always async so always Q
  Op op(Op::kOpIdleSetInterval, this, static_cast<uint64_t>(interval), 0);
  q_op_hi(sweeper, op);
}
void TableInfo::idle_lock(RmtSweeper *sweeper, uint16_t lock_id) {
  if (kSynchronousIdleOps) {
    idle_lock_wait(); // Wait for running dumps/sweeps to finish then take lock
    do_idle_lock(sweeper, lock_id);
  } else {
    Op op(Op::kOpIdleLock, this, static_cast<uint64_t>(lock_id), 0);
    q_op_hi(sweeper, op);
  }
}
void TableInfo::idle_unlock(RmtSweeper *sweeper, uint16_t lock_id) {
  if (kSynchronousIdleOps) {
    do_idle_unlock(sweeper, lock_id);
    idle_unlock_signal(); // Release lock allowing dumps/sweeps to run
  } else {
    Op op(Op::kOpIdleUnlock, this, static_cast<uint64_t>(lock_id), 0);
    q_op_hi(sweeper, op);
  }
}
void TableInfo::idle_sweep(RmtSweeper *sweeper, uint64_t picosecs) {
  // Always async so always Q - Q demux will ensure table not locked before running
  spinlock_.lock();
  bool q = (n_queued_idle_sweeps_ < kMaxQueuedIdleSweeps);
  if (q) n_queued_idle_sweeps_++;
  spinlock_.unlock();
  if (q) {
    Op op(Op::kOpIdleSweep, this, picosecs, 0);
    q_op_lo(sweeper, op);
  } else {
    RMT_LOG_OBJ(sweeper, RmtDebug::verbose(),
              "RmtSweeper[%d,%d,%d]::idle_sweep - too many sweeps queued!\n",
                pipe(), stage(), table());
  }
}
void TableInfo::idle_dump(RmtSweeper *sweeper, bool clear) {
  if (kSynchronousIdleOps) {
    idle_op_start(); // Wait till unlocked then mark as running
    do_idle_dump(sweeper, clear);
    idle_op_stop();  // Signal no longer running so lock may be acquired
  } else if (!kAllowSynchronousIdleDump || idle_locked() || any_ops_queued()) {
    Op op(Op::kOpIdleDump, this, UINT64_C(0), 0, clear);
    q_op_lo(sweeper, op);
  } else {
    do_idle_dump(sweeper, clear);
  }
}
void TableInfo::idle_dump_word(RmtSweeper *sweeper, uint32_t addr, bool clear) {
  if (kSynchronousIdleOps) {
    idle_op_start(); // Wait till unlocked then mark as running
    do_idle_dump_word(sweeper, addr, clear);
    idle_op_stop();  // Signal no longer running so lock may be acquired
  } else if (!kAllowSynchronousIdleDumpWord || idle_locked() || any_ops_queued()) {
    Op op(Op::kOpIdleDumpWord, this, UINT64_C(0), 0, clear);
    op.set_addr(addr, AddrType::kIdle);
    q_op_lo(sweeper, op);
  } else {
    do_idle_dump_word(sweeper, addr, clear);
  }
}

void TableInfo::stats_set_sweep_interval(RmtSweeper *sweeper, uint8_t interval) {
  // Always async so always Q
  Op op(Op::kOpStatsSetInterval, this, static_cast<uint64_t>(interval), 0);
  q_op_hi(sweeper, op);
}
void TableInfo::stats_lock(RmtSweeper *sweeper, uint16_t lock_id) {
  if (kSynchronousStatsOps) {
    stats_lock_wait(); // Wait for running dumps to finish then take lock
    do_stats_lock(sweeper, lock_id);
  } else {
    Op op(Op::kOpStatsLock, this, static_cast<uint64_t>(lock_id), 0);
    q_op_hi(sweeper, op);
  }
}
void TableInfo::stats_unlock(RmtSweeper *sweeper, uint16_t lock_id) {
  if (kSynchronousStatsOps) {
    do_stats_unlock(sweeper, lock_id);
    stats_unlock_signal(); // Release lock allowing dumps to run
  } else {
    Op op(Op::kOpStatsUnlock, this, static_cast<uint64_t>(lock_id), 0);
    q_op_hi(sweeper, op);
  }
}
void TableInfo::stats_sweep(RmtSweeper *sweeper, uint64_t picosecs) {
  // Always async so always Q - Q demux will ensure table not locked before running
  spinlock_.lock();
  bool q = (n_queued_stats_sweeps_ < kMaxQueuedStatsSweeps);
  if (q) n_queued_stats_sweeps_++;
  spinlock_.unlock();
  if (q) {
    Op op(Op::kOpStatsSweep, this, picosecs, 0);
    q_op_lo(sweeper, op);
  } else {
    RMT_LOG_OBJ(sweeper, RmtDebug::verbose(),
              "RmtSweeper[%d,%d,%d]::stats_sweep - too many sweeps queued!\n",
                pipe(), stage(), table());
  }
}
void TableInfo::stats_dump(RmtSweeper *sweeper) {
  if (kSynchronousStatsOps) {
    stats_op_start(); // Wait till unlocked then mark as running
    do_stats_dump(sweeper);
    stats_op_stop();  // Signal no longer running so lock may be acquired
  } else if (!kAllowSynchronousStatsDump || stats_locked() || any_ops_queued()) {
    Op op(Op::kOpStatsDump, this, UINT64_C(0), 0);
    q_op_lo(sweeper, op);
  } else {
    do_stats_dump(sweeper); // Call synchronously
  }
}
void TableInfo::stats_dump_word(RmtSweeper *sweeper, uint32_t addr) {
  if (kSynchronousStatsOps) {
    stats_op_start(); // Wait till unlocked then mark as running
    do_stats_dump_word(sweeper, addr);
    stats_op_stop();  // Signal no longer running so lock may be acquired
  } else if (!kAllowSynchronousStatsDumpWord || stats_locked() || any_ops_queued()) {
    Op op(Op::kOpStatsDumpWord, this, UINT64_C(0), 0);
    op.set_addr(addr, AddrType::kStats);
    q_op_lo(sweeper, op); // Call asynchronously
  } else {
    do_stats_dump_word(sweeper, addr); // Call synchronously
  }
}
void TableInfo::stats_evict_word(RmtSweeper *sweeper, uint32_t addr, uint64_t data) {
  // If table locked for stats or any outstanding queued OPs
  // then we must queue stats_evict_word Op
  if (stats_locked() || any_ops_queued()) {
    Op op(Op::kOpStatsEvictWord, this, data, 0);
    op.set_addr(addr, AddrType::kStats);
    q_op_lo(sweeper, op); // Call asynchronously
  } else {
    do_stats_evict_word(sweeper, addr, data); // Call synchronously
  }
}
void TableInfo::barrier(RmtSweeper *sweeper, uint16_t lock_id, uint8_t flags) {
  uint8_t without_stats_flags = flags & ~RmtSweeper::kFlagsBarrierStats;
  uint8_t without_idle_flags =  flags & ~RmtSweeper::kFlagsBarrierIdle;
  bool sync_stats = (kSynchronousStatsOps && ((flags & RmtSweeper::kFlagsBarrierStats) != 0));
  bool sync_idle  = (kSynchronousIdleOps  && ((flags & RmtSweeper::kFlagsBarrierIdle) != 0));
  if (sync_stats && sync_idle) {
    //stats_op_start(); idle_op_start();  // No need to wait for unlocked AFAIK
    do_barrier(sweeper, lock_id, flags);
    //stats_op_stop();  idle_op_stop();
  } else if (sync_stats) {
    //stats_op_start();
    do_barrier(sweeper, lock_id, without_idle_flags);
    //stats_op_stop();
    Op op(Op::kOpBarrier, this, static_cast<uint64_t>(lock_id), without_stats_flags);
    q_op_lo(sweeper, op);
  } else if (sync_idle) {
    //idle_op_start();
    do_barrier(sweeper, lock_id, without_stats_flags);
    //idle_op_stop();
    Op op(Op::kOpBarrier, this, static_cast<uint64_t>(lock_id), without_idle_flags);
    q_op_lo(sweeper, op);
  } else {
    Op op(Op::kOpBarrier, this, static_cast<uint64_t>(lock_id), flags);
    q_op_lo(sweeper, op);
  }
}
void TableInfo::meter_set_sweep_interval(RmtSweeper *sweeper, uint8_t interval) {
  // Always async so always Q
  Op op(Op::kOpMeterSetInterval, this, static_cast<uint64_t>(interval), 0);
  q_op_hi(sweeper, op);
}
void TableInfo::meter_sweep(RmtSweeper *sweeper, uint64_t picosecs) {
  // Always async so always Q
  spinlock_.lock();
  bool q = (n_queued_meter_sweeps_ < kMaxQueuedMeterSweeps);
  if (q) n_queued_meter_sweeps_++;
  spinlock_.unlock();
  if (q) {
    Op op(Op::kOpMeterSweep, this, picosecs, 0);
    q_op_lo(sweeper, op);
  } else {
    RMT_LOG_OBJ(sweeper, RmtDebug::verbose(),
              "RmtSweeper[%d,%d,%d]::meter_sweep - too many sweeps queued!\n",
                pipe(), stage(), table());
  }
}

bool TableInfo::op_stalled(RmtSweeper *sweeper, const Op &op) {
  RMT_ASSERT(this == op.tableinfo());
  switch (op.opcode()) {
    case Op::kOpNop:              return false;
    case Op::kOpIdleSetInterval:  return false;
    case Op::kOpIdleLock:         return false;
    case Op::kOpIdleUnlock:       return false;
    case Op::kOpIdleSweep:        return is_idle_locked_else_op_start();
    case Op::kOpIdleDump:         return idle_locked();
    case Op::kOpIdleDumpWord:     return idle_locked();
    case Op::kOpStatsSetInterval: return false;
    case Op::kOpStatsLock:        return false;
    case Op::kOpStatsUnlock:      return false;
    case Op::kOpStatsSweep:       return is_stats_locked_else_op_start();
    case Op::kOpStatsDump:        return stats_locked();
    case Op::kOpStatsDumpWord:    return stats_locked();
    case Op::kOpStatsEvictWord:   return stats_locked();
    case Op::kOpBarrier:          return false;
    case Op::kOpMeterSetInterval: return false;
    case Op::kOpMeterSweep:       return false;
    default: RMT_ASSERT(0); return false;
  }
}
void TableInfo::op_demux(RmtSweeper *sweeper, const Op &op) {
  RMT_ASSERT(this == op.tableinfo());
  uint32_t addr = remap_op_addr(op); // May do nothing
  switch (op.opcode()) {
    case Op::kOpNop:              break;
    case Op::kOpIdleSetInterval:  do_idle_set_sweep_interval(sweeper, op.arg8()); break;
    case Op::kOpIdleLock:         do_idle_lock(sweeper, op.arg16()); break;
    case Op::kOpIdleUnlock:       do_idle_unlock(sweeper, op.arg16()); break;
    case Op::kOpIdleSweep:        do_idle_sweep(sweeper, op.arg()); break;
    case Op::kOpIdleDump:         do_idle_dump(sweeper, op.tf()); break;
    case Op::kOpIdleDumpWord:     do_idle_dump_word(sweeper, addr, op.tf()); break;
    case Op::kOpStatsSetInterval: do_stats_set_sweep_interval(sweeper, op.arg8()); break;
    case Op::kOpStatsLock:        do_stats_lock(sweeper, op.arg16()); break;
    case Op::kOpStatsUnlock:      do_stats_unlock(sweeper, op.arg16()); break;
    case Op::kOpStatsSweep:       do_stats_sweep(sweeper, op.arg()); break;
    case Op::kOpStatsDump:        do_stats_dump(sweeper); break;
    case Op::kOpStatsDumpWord:    do_stats_dump_word(sweeper, addr); break;
    case Op::kOpStatsEvictWord:   do_stats_evict_word(sweeper, addr, op.arg()); break;
    case Op::kOpBarrier:          do_barrier(sweeper, op.arg16(), op.flags()); break;
    case Op::kOpMeterSetInterval: do_meter_set_sweep_interval(sweeper, op.arg8()); break;
    case Op::kOpMeterSweep:       do_meter_sweep(sweeper, op.arg()); break;
    default: RMT_ASSERT(0); break;
  }
  dec_ops_queued(); // Maintain count OPs per-table
  // If no more queued OPs then clear down moves
  if (!any_ops_queued()) clear_all_moves();

}
void TableInfo::sweep(RmtSweeper *sweeper, uint32_t interval_mask, uint64_t picosecs) {
  if ((idle_interval() < kIntervals) && ((interval_mask & (1u<<idle_interval())) != 0u))
    idle_sweep(sweeper, picosecs);
  if ((stats_interval() < kIntervals) && ((interval_mask & (1u<<stats_interval())) != 0u))
    stats_sweep(sweeper, picosecs);
  if ((meter_interval() < kIntervals) && ((interval_mask & (1u<<meter_interval())) != 0u))
    meter_sweep(sweeper, picosecs);
}


// Funcs to add movereg info to table, clear down movereg info, remap addrs in Ops
void TableInfo::add_move(uint8_t addrType, uint32_t fromAddr, uint32_t toAddr) {
  uint8_t lock_epoch = epoch(addrType);
  if (lock_epoch == 0) return; // Ignore moves if table not locked
  MoveInfo move(lock_epoch, addrType, fromAddr, toAddr);
  spinlock_.lock();
  if (move_list_ == NULL) move_list_ = new std::list<MoveInfo>;
  move_list_->push_back(move);
  spinlock_.unlock();
}
void TableInfo::clear_all_moves() {
  spinlock_.lock();
  if (move_list_ != NULL) delete move_list_;
  move_list_ = NULL;
  spinlock_.unlock();
}
uint32_t TableInfo::remap_op_addr(const Op &op) {
  uint32_t addr = op.addr();
  // Only remap Ops that queued because table was locked and that have addresses
  if ((op.lock_epoch() == 0) || (op.addrtype() == AddrType::kNone)) return addr;
  spinlock_.lock();
  if (move_list_ != NULL) {
    uint8_t op_epoch = op.lock_epoch(), op_addrtype = op.addrtype();
    std::list<MoveInfo>::iterator move = move_list_->begin();
    while (move != move_list_->end()) {
      if ((move->lock_epoch() == op_epoch) && (move->addrtype() == op_addrtype) &&
          (move->from_addr() == addr)) {
        addr = move->to_addr();
        break;
      }
      move++;
    }
  }
  spinlock_.unlock();
  return addr;
}



void TableInfo::q_op_hi(RmtSweeper *sweeper, const Op &op) {
  inc_ops_queued(); // Maintain count OPs per-table
  sweeper->queue_op_hi(op);
}
void TableInfo::q_op_lo(RmtSweeper *sweeper, const Op &op) {
  inc_ops_queued(); // Maintain count OPs per-table
  sweeper->queue_op_lo(op);
}
void TableInfo::idle_lock_ack(RmtSweeper *sweeper, uint16_t lock_id) {
  RmtObjectManager *om = sweeper->get_object_manager();
  Mau *mau = om->mau_lookup(pipe(), stage());
  int p_dump = mau->pipe_dump_index();
  int s_dump = mau->mau_dump_index();
  // Idletime LOCK_ACK == type 1
  uint64_t msg = MauMapram::kIdletimeLockAckMsgType;
  int lock_id_off = MauMapram::kIdletimeLockAckMsg_LockIdOffset;
  int pst_off = MauMapram::kIdletimeLockAckMsg_PipeStageTableOffset;
  int ps_off = pst_off + MauDefs::kTableBits;
  int p_off = ps_off + RmtDefs::kStageBits;
  msg |= static_cast<uint64_t>(lock_id & 0xFFFF)              << lock_id_off;
  msg |= static_cast<uint64_t>(table() & MauDefs::kTableMask) << pst_off;
  msg |= static_cast<uint64_t>(s_dump & RmtDefs::kStageMask)  << ps_off;
  msg |= static_cast<uint64_t>(p_dump & RmtDefs::kPipeMask)   << p_off;
  sweeper->idle_notify(p_dump, s_dump, msg);
}
void TableInfo::idle_unlock_ack(RmtSweeper *sweeper, uint16_t lock_id) {
  idle_lock_ack(sweeper, lock_id); // Same as LOCK_ACK
}
void TableInfo::idle_barrier_ack(RmtSweeper *sweeper, uint16_t lock_id) {
  idle_lock_ack(sweeper, lock_id); // Same as LOCK_ACK
}
void TableInfo::stats_lock_ack(RmtSweeper *sweeper, uint16_t lock_id) {
  RmtObjectManager *om = sweeper->get_object_manager();
  Mau *mau = om->mau_lookup(pipe(), stage());
  int p_dump = mau->pipe_dump_index();
  int s_dump = mau->mau_dump_index();
  // Stats LOCK_ACK == type 1
  uint64_t msg = MauStatsAlu::kStatsLockAckMsgType;
  int lock_id_off = MauStatsAlu::kStatsLockAckMsg_LockIdOffset;
  int pst_off = MauStatsAlu::kStatsLockAckMsg_PipeStageTableOffset;
  int ps_off = pst_off + MauDefs::kTableBits;
  int p_off = ps_off + RmtDefs::kStageBits;
  msg |= static_cast<uint64_t>(lock_id & 0xFFFF)              << lock_id_off;
  msg |= static_cast<uint64_t>(table() & MauDefs::kTableMask) << pst_off;
  msg |= static_cast<uint64_t>(s_dump & RmtDefs::kStageMask)  << ps_off;
  msg |= static_cast<uint64_t>(p_dump & RmtDefs::kPipeMask)   << p_off;
  sweeper->stats_notify(p_dump, s_dump, msg, UINT64_C(0));
}
void TableInfo::stats_unlock_ack(RmtSweeper *sweeper, uint16_t lock_id) {
  stats_lock_ack(sweeper, lock_id); // Same as LOCK_ACK
}
void TableInfo::stats_barrier_ack(RmtSweeper *sweeper, uint16_t lock_id) {
  stats_lock_ack(sweeper, lock_id); // Same as LOCK_ACK
}

void TableInfo::do_idle_set_sweep_interval(RmtSweeper *sweeper, uint8_t intvl) {
  RMT_LOG_OBJ(sweeper,
              RmtDebug::verbose(RmtDebug::kRmtDebugRmtSweeperIdleSetInterval),
              "RmtSweeper[%d,%d,%d]::idle_set_sweep_interval(%d)\n",
              pipe(), stage(), table(), intvl);
  uint8_t prev_intvl = idle_interval();
  if (prev_intvl == intvl) return;
  // Big intervals used to indicate table is idle. These big intervals
  // effectively disable idle sweep (but can prevent this)
  if ((intvl >= kIntervals) && (!kDisableIdleSweepOnTableIdle)) return;

  // Set new interval in sweeper
  if ((intvl < kIntervals) &&
      (intvl != stats_interval()) && (intvl != meter_interval())) {
    sweeper->set_interval(pipe(), stage(), table(), intvl);
  }
  // Clear old interval in sweeper
  if ((prev_intvl < kIntervals) &&
      (prev_intvl != stats_interval()) && (prev_intvl != meter_interval())) {
    sweeper->clear_interval(pipe(), stage(), table(), prev_intvl);
  }
  // Update TableInfo
  set_idle_interval(intvl);
}
void TableInfo::do_idle_lock(RmtSweeper *sweeper, uint16_t lock_id) {
  // Lock TableInfo - may double lock if synchronous idle ops and just done idle_lock_wait()
  set_idle_locked(true);
  // Send lock ack
  idle_lock_ack(sweeper, lock_id);
  // Keep tally
  RmtObjectManager *om = sweeper->get_object_manager();
  Mau *mau = om->mau_lookup(pipe(), stage());
  mau->mau_info_incr(MAU_IDLETIME_LOCKS);
}
void TableInfo::do_idle_unlock(RmtSweeper *sweeper, uint16_t lock_id) {
  // Unlock TableInfo
  set_idle_locked(false);
  // Send lock ack
  idle_unlock_ack(sweeper, lock_id);
  // Keep tally
  RmtObjectManager *om = sweeper->get_object_manager();
  Mau *mau = om->mau_lookup(pipe(), stage());
  mau->mau_info_incr(MAU_IDLETIME_UNLOCKS);
}
void TableInfo::do_idle_sweep(RmtSweeper *sweeper, uint64_t picosecs) {
  spinlock_.lock();
  n_queued_idle_sweeps_--;
  spinlock_.unlock();
  //RMT_LOG_OBJ(sweeper, RmtDebug::kRmtDebugRmtSweeperIdleSweep,
  //            "RmtSweeper[%d,%d,%d]::idle_sweep(%16" PRId64 ")\n",
  //            pipe(), stage(), table(), picosecs);
  RmtObjectManager *om = sweeper->get_object_manager();
  Mau *mau = om->mau_lookup(pipe(), stage());
  MauAddrDist *mau_addr_dist = mau->mau_addr_dist();
  mau_addr_dist->idletime_sweep(table(), picosecs);
  // Sweeps always async
  // op_start called prior to op_demux() in op_stalled() - so just call op_stop()
  idle_op_stop();
}
void TableInfo::do_idle_dump(RmtSweeper *sweeper, bool clear) {
  RmtObjectManager *om = sweeper->get_object_manager();
  Mau *mau = om->mau_lookup(pipe(), stage());
  MauAddrDist *mau_addr_dist = mau->mau_addr_dist();
  mau_addr_dist->idletime_dump(table(), clear);
}
void TableInfo::do_idle_dump_word(RmtSweeper *sweeper, uint32_t addr, bool clear) {
  RmtObjectManager *om = sweeper->get_object_manager();
  Mau *mau = om->mau_lookup(pipe(), stage());
  MauAddrDist *mau_addr_dist = mau->mau_addr_dist();
  mau_addr_dist->idletime_dump_word(table(), addr, clear);
}
void TableInfo::do_stats_set_sweep_interval(RmtSweeper *sweeper, uint8_t intvl) {
  RMT_LOG_OBJ(sweeper,
              RmtDebug::verbose(RmtDebug::kRmtDebugRmtSweeperStatsSetInterval),
              "RmtSweeper[%d,%d,%d]::stats_set_sweep_interval(%d)\n",
              pipe(), stage(), table(), intvl);
  uint8_t prev_intvl = stats_interval();
  if (prev_intvl == intvl) return;
  // Set new interval in sweeper
  if ((intvl < kIntervals) &&
      (intvl != idle_interval()) && (intvl != meter_interval())) {
    sweeper->set_interval(pipe(), stage(), table(), intvl);
  }
  // Clear old interval in sweeper
  if ((prev_intvl < kIntervals) &&
      (prev_intvl != idle_interval()) && (prev_intvl != meter_interval())) {
    sweeper->clear_interval(pipe(), stage(), table(), prev_intvl);
  }
  // Update TableInfo
  set_stats_interval(intvl);
}
void TableInfo::do_stats_lock(RmtSweeper *sweeper, uint16_t lock_id) {
  // Lock TableInfo - may double lock if synchronous stats ops and just done stats_lock_wait()
  set_stats_locked(true);
  // Send lock ack
  stats_lock_ack(sweeper, lock_id);
  // Keep tally
  RmtObjectManager *om = sweeper->get_object_manager();
  Mau *mau = om->mau_lookup(pipe(), stage());
  mau->mau_info_incr(MAU_STATS_LOCKS);
}
void TableInfo::do_stats_unlock(RmtSweeper *sweeper, uint16_t lock_id) {
  // Unlock TableInfo
  set_stats_locked(false);
  // Send unlock ack
  stats_unlock_ack(sweeper, lock_id);
  // Keep tally
  RmtObjectManager *om = sweeper->get_object_manager();
  Mau *mau = om->mau_lookup(pipe(), stage());
  mau->mau_info_incr(MAU_STATS_UNLOCKS);
}
void TableInfo::do_stats_sweep(RmtSweeper *sweeper, uint64_t picosecs) {
  spinlock_.lock();
  n_queued_stats_sweeps_--;
  spinlock_.unlock();
  //RmtObjectManager *om = sweeper->get_object_manager();
  //Mau *mau = om->mau_lookup(pipe(), stage());
  //MauAddrDist *mau_addr_dist = mau->mau_addr_dist();
  //mau_addr_dist->stats_sweep(table(), picosecs);
  // Sweeps always async
  // op_start called prior to op_demux() in op_stalled() - so just call op_stop()
  stats_op_stop();
}
void TableInfo::do_stats_dump(RmtSweeper *sweeper) {
  RmtObjectManager *om = sweeper->get_object_manager();
  Mau *mau = om->mau_lookup(pipe(), stage());
  MauAddrDist *mau_addr_dist = mau->mau_addr_dist();
  mau_addr_dist->stats_dump(table());
}
void TableInfo::do_stats_dump_word(RmtSweeper *sweeper, uint32_t addr) {
  RmtObjectManager *om = sweeper->get_object_manager();
  Mau *mau = om->mau_lookup(pipe(), stage());
  MauAddrDist *mau_addr_dist = mau->mau_addr_dist();
  mau_addr_dist->stats_dump_word(table(), addr, true);
}
void TableInfo::do_stats_evict_word(RmtSweeper *sweeper, uint32_t addr, uint64_t data) {
  RmtObjectManager *om = sweeper->get_object_manager();
  Mau *mau = om->mau_lookup(pipe(), stage());
  MauAddrDist *mau_addr_dist = mau->mau_addr_dist();
  // Just notify - stats entry was cleared synchronously in ALU
  // NB didn't bother with op_start here as just a notify - so no need for op_stop
  mau_addr_dist->stats_notify(table(), addr, data, false);
}
void TableInfo::do_barrier(RmtSweeper *sweeper, uint16_t lock_id, uint8_t flags) {
  // Send stats ack and/or idle ack dependent on flags
  if ((flags & RmtSweeper::kFlagsBarrierStats) != 0) stats_barrier_ack(sweeper, lock_id);
  if ((flags & RmtSweeper::kFlagsBarrierIdle) != 0)  idle_barrier_ack(sweeper, lock_id);
  // Keep tally
  RmtObjectManager *om = sweeper->get_object_manager();
  Mau *mau = om->mau_lookup(pipe(), stage());
  mau->mau_info_incr(MAU_BARRIER_OPS);
}
void TableInfo::do_meter_set_sweep_interval(RmtSweeper *sweeper, uint8_t intvl) {
  RMT_LOG_OBJ(sweeper,
              RmtDebug::verbose(RmtDebug::kRmtDebugRmtSweeperMeterSetInterval),
              "RmtSweeper[%d,%d,%d]::meter_set_sweep_interval(%d)\n",
              pipe(), stage(), table(), intvl);
  uint8_t prev_intvl = meter_interval();
  if (prev_intvl == intvl) return;
  // Set new interval in sweeper
  if ((intvl < kIntervals) &&
      (intvl != idle_interval()) && (intvl != stats_interval())) {
    sweeper->set_interval(pipe(), stage(), table(), intvl);
  }
  // Clear old interval in sweeper
  if ((prev_intvl < kIntervals) &&
      (prev_intvl != idle_interval()) && (prev_intvl != stats_interval())) {
    sweeper->clear_interval(pipe(), stage(), table(), prev_intvl);
  }
  // Update TableInfo
  set_meter_interval(intvl);
}
void TableInfo::do_meter_sweep(RmtSweeper *sweeper, uint64_t picosecs) {
  spinlock_.lock();
  n_queued_meter_sweeps_--;
  spinlock_.unlock();
  RmtObjectManager *om = sweeper->get_object_manager();
  Mau *mau = om->mau_lookup(pipe(), stage());
  MauAddrDist *mau_addr_dist = mau->mau_addr_dist();
  mau_addr_dist->meter_sweep(table(), picosecs);
}




RmtSweeper::RmtSweeper(RmtObjectManager *om)
    : RmtObject(om), DefaultLogger(om, RmtTypes::kRmtTypeRmtSweeper),
      spinlock_(),
      stage_sweep_cnt_tab_(),
      table_info_(), pipe_table_map_(),
      global_interval_mask_(0u), per_stage_interval_masks_(),
      tick_last_(0u), tick_now_(0u),
      tick_last_psecs_(UINT64_C(0)) {
  // Setup OP lists
  ops_hi_ = new std::list<Op>;
  ops_lo_ = new std::list<Op>;
  // Setup all tableinfos
  for (int pipe = 0; pipe < kPipes; pipe++) {
    for (int stage = 0; stage < kStages; stage++) {
      for (int table = 0; table < kTables; table++) {
        table_info_[pipe][stage][table].init(pipe,stage,table);
      }
    }
  }
}
RmtSweeper::~RmtSweeper() {
  delete ops_hi_;
  delete ops_lo_;
}

uint32_t RmtSweeper::get_stage_sweep_cnt(int stage) {
  RMT_ASSERT((stage >= 0) && (stage < kStages));
  uint32_t cnt = stage_sweep_cnt_tab_[stage];
  stage_sweep_cnt_tab_[stage] = 0u;
  return cnt;
}
bool RmtSweeper::idle_notify(int pipe, int stage, uint64_t msg0) {
  RMT_ASSERT((pipe >= 0) && (pipe < kPipes));
  RMT_ASSERT((stage >= 0) && (stage < kStages));
  GLOBAL_MODEL->dru_idle_update_callback(chip_index(), (uint8_t*)&msg0, 8);
  return true;
}
bool RmtSweeper::stats_notify(int pipe, int stage, uint64_t msg0, uint64_t msg1) {
  uint64_t message[2];
  RMT_ASSERT((pipe >= 0) && (pipe < kPipes));
  RMT_ASSERT((stage >= 0) && (stage < kStages));
  message[0] = msg0; message[1] = msg1;
  GLOBAL_MODEL->dru_lrt_update_callback(chip_index(), (uint8_t*)&message, 16);
  return true;
}

// Only for testing - used to sweep at stage granularity - so to keep tests
// happy just set_sweep_interval for single table, table 15
void RmtSweeper::idle_set_stage_sweep_interval(int pipe, int stage, uint8_t interval) {
  idle_set_sweep_interval(pipe, stage, kTables-1, interval);
}
void RmtSweeper::idle_set_sweep_interval(int pipe, int stage, int table, uint8_t interval) {
  RMT_ASSERT(table_info_[pipe][stage][table].pipe() == pipe);
  RMT_ASSERT(table_info_[pipe][stage][table].stage() == stage);
  RMT_ASSERT(table_info_[pipe][stage][table].table() == table);
  table_info_[pipe][stage][table].idle_set_sweep_interval(this, interval);
}
void RmtSweeper::idle_lock(int pipe, int stage, int table, uint16_t lock_id) {
  table_info_[pipe][stage][table].idle_lock(this, lock_id);
}
void RmtSweeper::idle_unlock(int pipe, int stage, int table, uint16_t lock_id) {
  table_info_[pipe][stage][table].idle_unlock(this, lock_id);
}
void RmtSweeper::idle_sweep(int pipe, int stage, int table, uint64_t picosecs) {
  table_info_[pipe][stage][table].idle_sweep(this, picosecs);
}
void RmtSweeper::idle_dump(int pipe, int stage, int table, bool clear) {
  table_info_[pipe][stage][table].idle_dump(this, clear);
}
void RmtSweeper::idle_dump_word(int pipe, int stage, int table, uint32_t addr, bool clear) {
  table_info_[pipe][stage][table].idle_dump_word(this, addr, clear);
}
void RmtSweeper::stats_set_sweep_interval(int pipe, int stage, int table, uint8_t interval) {
  table_info_[pipe][stage][table].stats_set_sweep_interval(this, interval);
}
void RmtSweeper::stats_lock(int pipe, int stage, int table, uint16_t lock_id) {
  table_info_[pipe][stage][table].stats_lock(this, lock_id);
}
void RmtSweeper::stats_unlock(int pipe, int stage, int table, uint16_t lock_id) {
  table_info_[pipe][stage][table].stats_unlock(this, lock_id);
}
void RmtSweeper::stats_sweep(int pipe, int stage, int table, uint64_t picosecs) {
  table_info_[pipe][stage][table].stats_sweep(this, picosecs);
}
void RmtSweeper::stats_dump(int pipe, int stage, int table) {
  table_info_[pipe][stage][table].stats_dump(this);
}
void RmtSweeper::stats_dump_word(int pipe, int stage, int table, uint32_t addr) {
  table_info_[pipe][stage][table].stats_dump_word(this, addr);
}
void RmtSweeper::stats_evict_word(int pipe, int stage, int table, uint32_t addr, uint64_t data) {
  table_info_[pipe][stage][table].stats_evict_word(this, addr, data);
}
void RmtSweeper::barrier(int pipe, int stage, int table, uint16_t lock_id, uint8_t flags) {
  table_info_[pipe][stage][table].barrier(this, lock_id, flags);
}
void RmtSweeper::meter_set_sweep_interval(int pipe, int stage, int table, uint8_t interval) {
  table_info_[pipe][stage][table].meter_set_sweep_interval(this, interval);
}
void RmtSweeper::meter_sweep(int pipe, int stage, int table, uint64_t picosecs) {
  table_info_[pipe][stage][table].meter_sweep(this, picosecs);
}


void RmtSweeper::sweep(int pipe, int stage, int table, uint32_t interval_mask,
                       uint64_t t_now_psecs) {
  table_info_[pipe][stage][table].sweep(this, interval_mask, t_now_psecs);
}


void RmtSweeper::sweep(uint64_t t_now_psecs) {
  // Attempt to process all queued OPs
  dequeue_hi(); dequeue_lo();

  RMT_ASSERT(t_now_psecs >= tick_last_psecs_);
  if (t_now_psecs == tick_last_psecs_) return;
  // Remember when we got to
  tick_last_psecs_ = t_now_psecs;

  // Convert t_now_psecs to ticks
  tick_now_ = psecs_to_ticks(t_now_psecs);
  RMT_ASSERT(tick_now_ >= tick_last_);
  if (tick_now_ == tick_last_) return;
  // Keep tick_last_ in tmp var for while loop
  uint32_t tick_last = tick_last_;
  // Set tick_last_ = tick_now_ in case we return early
  tick_last_ = tick_now_;

  // Find first/last intervals in global_interval_mask_
  int first_intvl = find_first_interval();
  if (first_intvl < 0) return;
  int last_intvl = find_last_interval();
  RMT_ASSERT(last_intvl >= first_intvl);
  // No need to tick by just 1 if first_intvl > 0
  uint32_t tick_inc = interval_to_ticks(first_intvl);
  uint32_t tick_nxt = std::min(tick_now_, tick_last + tick_inc);

  while (tick_nxt <= tick_now_) {
    uint64_t tick_nxt_psecs = ticks_to_psecs(tick_nxt);

    // Sweep stage0, then stage1 etc etc, but only the pipes/tables
    // selected by intervals_to_sweep
    for (int stage = 0; stage < kStages; stage++) {

      // Do high pri OP processing - stuff like lock/unlock etc
      dequeue_hi();

      // The flipped bits from tick_last -> tick_nxt
      // indicate the intervals that need to be swept (hence XOR)
      // but also mask off and ignore empty intervals
      uint32_t intervals_to_sweep = (tick_last ^ tick_nxt) & per_stage_interval_masks_[stage];

      uint64_t pt_sweep = UINT64_C(0);
      for (int intvl = first_intvl; intvl <= last_intvl; intvl++) {
        // For each selected interval, for each stage,
        // accumulate the pipes/tables to sweep
        if ((intervals_to_sweep & (1u<<intvl)) != 0u) {
          pt_sweep |= pipe_table_map_[intvl][stage];
        }
      }
      // Then for all selected pipes/tables run the table sweep
      if (pt_sweep != UINT64_C(0)) {
        for (int pt = 0; pt < kPipes*kTables; pt++) {
          if ((pt_sweep & (UINT64_C(1) << pt)) != UINT64_C(0)) {
            int pipe = pt_get_pipe(pt);
            int table = pt_get_table(pt);

            stage_sweep_cnt_tab_[stage]++; // For testing
            // Sweep table (idles/stats/meters)
            sweep(pipe, stage, table, intervals_to_sweep, tick_nxt_psecs);
          }
        }
      }

    } // for stage

    // Then advance our tmp tick_last
    tick_last = tick_nxt;

    // Recalc tick_inc here - hipri processing might have
    // modified global_interval_mask_
    first_intvl = find_first_interval();
    if (first_intvl < 0) break;
    last_intvl = find_last_interval();
    RMT_ASSERT(last_intvl >= first_intvl);
    tick_inc = interval_to_ticks(first_intvl);
    tick_nxt = tick_last + tick_inc;

  } // while tick_nxt

  // Attempt to process all queued OPs, again.
  dequeue_hi(); dequeue_lo();
}


void RmtSweeper::sweep_increment(uint64_t t_inc_psecs) {
  if (t_inc_psecs == 0) {
    // This is something to do with the meter tests, which call runner
    //  with --time-disable which means time does not get advanced,
    //  but rmt_time_increment(0) gets called instead.
    // Can't call sweep because this can cause problems if another
    //  thread is advancing time, so just do what sweep() would have
    //  done when called with an zero increment.
    dequeue_hi(); dequeue_lo();
  } else {
    sweep(tick_last_psecs_ + t_inc_psecs);
  }
}
void RmtSweeper::sweep_increment_cycles(uint64_t cycles) {
  sweep_increment( cycles_to_psecs(cycles) );
}


uint64_t RmtSweeper::get_time_now() {
  return tick_last_psecs_;
}
uint64_t RmtSweeper::get_cycles_now() {
  return tick_last_psecs_ / kOneCyclePicosecs;
}

// Add OPs to the hipri Q
void RmtSweeper::queue_op_hi(const Op &op) {
  spinlock_.lock();
  ops_hi_->push_back(op);
  spinlock_.unlock();
}
// And the lopri Q
void RmtSweeper::queue_op_lo(const Op &op) {
  spinlock_.lock();
  ops_lo_->push_back(op);
  spinlock_.unlock();
}

// Check the hipri Q for ops relating to table
bool RmtSweeper::queue_check_hi(TableInfo *ti) {
  bool found_op = false;
  spinlock_.lock();
  std::list<Op>::iterator opit = ops_hi_->begin();
  while (opit != ops_hi_->end()) {
    if (opit->tableinfo() == ti) {
      found_op = true;
      break;
    }
    opit++;
  }
  spinlock_.unlock();
  return found_op;
}

// Process hipri - always demux every OP
void RmtSweeper::dequeue_hi() {
  std::list<Op> *local_ops = NULL;
  spinlock_.lock();
  local_ops = ops_hi_;
  if (local_ops->empty()) {
    // Do nothing - exit
    spinlock_.unlock();
    return;
  } else {
    // Swap and continue
    ops_hi_ = new std::list<Op>;
    spinlock_.unlock();
  }
  while (true) {
    std::list<Op>::iterator opit = local_ops->begin();
    if (opit == local_ops->end()) break;
    opit->tableinfo()->op_demux(this, *opit);
    local_ops->pop_front();
  }
  delete local_ops;
}

// Process lopri - only demux non-stalled OPs
// ie OPs that are not blocked by a table lock
// eg idle_dump is blocked by the idle lock
void RmtSweeper::dequeue_lo() {
  if (ops_lo_->empty()) return;

  std::list<Op> *stalled_ops = new std::list<Op>;
  std::list<Op> *ready_ops = new std::list<Op>;
  std::list<Op> *local_ops = NULL;
  while (true) {

    spinlock_.lock();
    local_ops = ops_lo_;
    if (local_ops->empty()) {
      // Swap with stalled OPs
      ops_lo_ = stalled_ops;
      spinlock_.unlock();
      delete local_ops;
      break;

    } else {
      // Swap with empty list
      ops_lo_ = new std::list<Op>;
      spinlock_.unlock();
      while (true) {
        std::list<Op>::iterator opit = local_ops->begin();
        if (opit == local_ops->end()) break;
        if (opit->tableinfo()->op_stalled(this, *opit)) {
          stalled_ops->push_back(*opit);
        } else {
          ready_ops->push_back(*opit);
        }
        local_ops->pop_front();
      }
      delete local_ops;
    }
  }
  // Process ready OPs
  while (true) {
    std::list<Op>::iterator opit = ready_ops->begin();
    if (opit == ready_ops->end()) break;
    opit->tableinfo()->op_demux(this, *opit);
    ready_ops->pop_front();
  }
  delete ready_ops;
}

// Add movereg info to table
void RmtSweeper::update_queued_addr(int pipe, int stage, int table,
                                    uint8_t addrtype, uint32_t from_addr, uint32_t to_addr) {
  table_info_[pipe][stage][table].add_move(addrtype, from_addr, to_addr);
}


}
