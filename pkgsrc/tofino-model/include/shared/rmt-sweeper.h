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

#ifndef _SHARED_RMT_SWEEPER_
#define _SHARED_RMT_SWEEPER_

#include <memory>
#include <list>
#include <condition_variable>
#include <rmt-defs.h>
#include <rmt-object.h>
#include <model_core/spinlock.h>

namespace MODEL_CHIP_NAMESPACE {

  class Op;
  class RmtSweeper;

  // Maintain info about move operations received
  // Associate each with the current lock_epoch to allow remapping of Op addrs
  //
  class MoveInfo {
 public:
    MoveInfo(uint8_t lockEpoch, uint8_t addrType, uint32_t fromAddr, uint32_t toAddr) {
      lock_epoch_ = lockEpoch;
      addr_type_ = addrType;
      from_addr_ = fromAddr;
      to_addr_ = toAddr;
    }
    ~MoveInfo() { }

    uint8_t   lock_epoch()  const { return lock_epoch_; }
    uint8_t   addrtype()    const { return addr_type_; }
    uint32_t  from_addr()   const { return from_addr_; }
    uint32_t  to_addr()     const { return to_addr_; }

 private:
    uint8_t   lock_epoch_;
    uint8_t   addr_type_;
    uint32_t  from_addr_;
    uint32_t  to_addr_;
  };



  // Lock/Wait control object for Stats/Idle
  // Counts number threads locking/running in table (one of these always zero)
  // Also maintains rolling 'epoch' which ticks on first lock (epoch 0 ==> unlocked)
  //
  class TableOpCtl {
 public:
    TableOpCtl() : cv_mutex_(), cv_(), run_cnt_(0), lock_cnt_(0), lock_epoch_(0) {
      spinlock_.clear();
    }
    ~TableOpCtl() { }

    inline void tick_epoch() {
      if (lock_epoch_ == 255) lock_epoch_ = 1;
      else lock_epoch_++;
    }
    inline void    reset_epoch()      { lock_epoch_ = 0; }
    inline uint8_t epoch()      const { return lock_epoch_; }
    inline bool    locked()     const { return (lock_cnt_ > 0); }
    inline bool    unlocked()   const { return (lock_cnt_ == 0); }
    inline void    inc_lock()         { lock_cnt_++; if (lock_cnt_ == 1) tick_epoch(); }
    inline void    dec_lock()         { lock_cnt_--; if (lock_cnt_ == 0) reset_epoch(); }
    inline bool    running()    const { return (run_cnt_ > 0); }
    inline bool    notrunning() const { return (run_cnt_ == 0); }
    inline void    inc_run()          { run_cnt_++; }
    inline void    dec_run()          { run_cnt_--; }
    // Next func called from TableInfo set_x_locked funcs so wrap in spinlock/spinunlock
    inline void    do_lock(bool tf)   {
      spinlock_.lock(); if (tf) inc_lock(); else dec_lock(); spinlock_.unlock();
    }

    void lock_wait();
    void unlock_signal();
    void op_start();
    void op_stop();
    bool is_locked_else_op_start();

 private:
    // Waiting/signalling prims
    void wait_stopped() {
      std::unique_lock<std::mutex> cv_lock(cv_mutex_);
      while (running()) cv_.wait(cv_lock);
    }
    void wait_unlocked() {
      std::unique_lock<std::mutex> cv_lock(cv_mutex_);
      while (locked()) cv_.wait(cv_lock);
    }
    void do_signal() {
      std::unique_lock<std::mutex> cv_lock(cv_mutex_);
      cv_.notify_all();
    }

 private:
    model_core::Spinlock     spinlock_;
    std::mutex               cv_mutex_;
    std::condition_variable  cv_;
    uint16_t                 run_cnt_;
    uint16_t                 lock_cnt_;
    uint8_t                  lock_epoch_;

  }; // TableOpCtl



  // Maintain info about each stats/idle table in system
  class TableInfo {
    static constexpr int kPipes = RmtDefs::kPipesMax;
    static constexpr int kStages = RmtDefs::kStagesMax;
    static constexpr int kTables = MauDefs::kLogicalTablesPerMau;
    static_assert( (kPipes < 256), "Pipe must fit in uint8_t");
    static_assert( (kStages < 256), "Stage must fit in uint8_t");
    static_assert( (kTables < 256), "Table must fit in uint8_t");
    static constexpr int kMaxQueuedIdleSweeps = 8;
    static constexpr int kMaxQueuedStatsSweeps = 8;
    static constexpr int kMaxQueuedMeterSweeps = 8;
    static_assert( (kMaxQueuedIdleSweeps < 256), "n_queued_idle_sweeps must fit in uint8_t");
    static_assert( (kMaxQueuedStatsSweeps < 256), "n_queued_stats_sweeps must fit in uint8_t");
    static_assert( (kMaxQueuedMeterSweeps < 256), "n_queued_meter_sweeps must fit in uint8_t");


    static constexpr uint8_t kIntervals = MauDefs::kMaxInterval;
    static constexpr uint8_t kTableInfoFlagsInitialized = 0x1;
    static constexpr uint8_t kTableInfoFlagsEnabled = 0x2;
    static constexpr uint8_t kTableInfoFlagsActive = 0x4;
    static constexpr uint8_t kTableInfoFlagsIdleLocked = 0x8;
    static constexpr uint8_t kTableInfoFlagsStatsLocked = 0x10;


 public:
    // Defined in rmt-config.cpp
    static bool kAllowSynchronousIdleDump;
    static bool kAllowSynchronousIdleDumpWord;
    static bool kAllowSynchronousStatsDump;
    static bool kAllowSynchronousStatsDumpWord;
    static bool kDisableIdleSweepOnTableIdle;
    static bool kSynchronousIdleOps;  // Takes precedence over kAllowSynchronousIdleX
    static bool kSynchronousStatsOps; // Takes precedence over kAllowSynchronousStatsX

    TableInfo() {
      reset();
      spinlock_.clear();
    }
    TableInfo(const TableInfo& other) = delete;  // XXX
    ~TableInfo() { flags_ = 0; clear_all_moves(); }

    void reset() {
      move_list_ = nullptr;
      n_queued_ops_ = 0;
      n_queued_idle_sweeps_ = 0;
      n_queued_stats_sweeps_ = 0;
      n_queued_meter_sweeps_ = 0;
      pipe_ = 0;
      stage_ = 0;
      table_ = 0;
      flags_ = 0;
      idle_interval_ = 0xFF;
      stats_interval_ = 0xFF;
      meter_interval_ = 0xFF;
    }

    void init(int pipe, int stage, int table) {
      RMT_ASSERT((pipe >= 0) && (pipe < kPipes));
      RMT_ASSERT((stage >= 0) && (stage < kStages));
      RMT_ASSERT((table >= 0) && (table < kTables));
      reset();
      pipe_ = static_cast<uint8_t>(pipe & 0xFF);
      stage_ = static_cast<uint8_t>(stage & 0xFF);
      table_ = static_cast<uint8_t>(table & 0xFF);
      flags_ = kTableInfoFlagsInitialized;
    }

    // Inline getters
    int  pipe()   const { return static_cast<int>(pipe_); }
    int  stage()  const { return static_cast<int>(stage_); }
    int  table()  const { return static_cast<int>(table_); }

    // At moment don't use enabled/active - logical tables convey this
    // info to sweeper by setting very large interval
    bool has_flag(uint8_t f) const { return ((flags_ & f) != 0); }
    bool enabled()           const { return has_flag(kTableInfoFlagsEnabled); }
    bool active()            const { return has_flag(kTableInfoFlagsActive); }
    //bool idle_locked()     const { return has_flag(kTableInfoFlagsIdleLocked); }
    //bool stats_locked()    const { return has_flag(kTableInfoFlagsStatsLocked); }
    bool    idle_locked()    const { return idle_op_ctl_.locked(); }
    bool    stats_locked()   const { return stats_op_ctl_.locked(); }
    uint8_t idle_interval()  const { return idle_interval_; }
    uint8_t stats_interval() const { return stats_interval_; }
    uint8_t meter_interval() const { return meter_interval_; }
    uint8_t idle_epoch()     const { return (idle_locked()) ?idle_op_ctl_.epoch() :0; }
    uint8_t stats_epoch()    const { return (stats_locked()) ?stats_op_ctl_.epoch() :0; }
    bool    any_ops_queued() const { return (n_queued_ops_ > 0); }
    uint8_t epoch(uint8_t addrType) const {
      if      (addrType == AddrType::kIdle)  return idle_epoch();
      else if (addrType == AddrType::kStats) return stats_epoch();
      return 0;
    }

    // ...and setters
    void inc_ops_queued() { spinlock_.lock(); n_queued_ops_++; spinlock_.unlock(); }
    void dec_ops_queued() { spinlock_.lock(); n_queued_ops_--; spinlock_.unlock(); }
    void set_flag(uint8_t flag, bool tf) {
      if (tf) flags_ |= flag; else flags_ &= ~flag;
    }
    void set_enabled(bool tf=true)         { set_flag(kTableInfoFlagsEnabled,tf); }
    void set_active(bool tf=true)          { set_flag(kTableInfoFlagsActive,tf); }
    //void set_idle_locked(bool tf=true)   { set_flag(kTableInfoFlagsIdleLocked,tf); }
    //void set_stats_locked(bool tf=true)  { set_flag(kTableInfoFlagsStatsLocked,tf); }
    void set_idle_locked(bool tf=true)     { idle_op_ctl_.do_lock(tf); }
    void set_stats_locked(bool tf=true)    { stats_op_ctl_.do_lock(tf); }
    void set_idle_interval(uint8_t intvl)  { idle_interval_ = intvl; }
    void set_stats_interval(uint8_t intvl) { stats_interval_ = intvl; }
    void set_meter_interval(uint8_t intvl) { meter_interval_ = intvl; }

    // Shims to op_ctl_ funcs
    void idle_lock_wait()                  { idle_op_ctl_.lock_wait(); }
    void idle_unlock_signal()              { idle_op_ctl_.unlock_signal(); }
    void idle_op_start()                   { idle_op_ctl_.op_start(); }
    void idle_op_stop()                    { idle_op_ctl_.op_stop(); }
    bool is_idle_locked_else_op_start()    { return idle_op_ctl_.is_locked_else_op_start(); }
    void stats_lock_wait()                 { stats_op_ctl_.lock_wait(); }
    void stats_unlock_signal()             { stats_op_ctl_.unlock_signal(); }
    void stats_op_start()                  { stats_op_ctl_.op_start(); }
    void stats_op_stop()                   { stats_op_ctl_.op_stop(); }
    bool is_stats_locked_else_op_start()   { return stats_op_ctl_.is_locked_else_op_start(); }


    // Wrapper funcs which queue the corresponding OP
    void idle_set_sweep_interval(RmtSweeper *sweeper, uint8_t interval);
    void idle_lock(RmtSweeper *sweeper, uint16_t lock_id);
    void idle_unlock(RmtSweeper *sweeper, uint16_t lock_id);
    void idle_sweep(RmtSweeper *sweeper, uint64_t picosecs);
    void idle_dump(RmtSweeper *sweeper, bool clear);
    void idle_dump_word(RmtSweeper *sweeper, uint32_t addr, bool clear);
    void stats_set_sweep_interval(RmtSweeper *sweeper, uint8_t interval);
    void stats_lock(RmtSweeper *sweeper, uint16_t lock_id);
    void stats_unlock(RmtSweeper *sweeper, uint16_t lock_id);
    void stats_sweep(RmtSweeper *sweeper, uint64_t picosecs);
    void stats_dump(RmtSweeper *sweeper);
    void stats_dump_word(RmtSweeper *sweeper, uint32_t addr);
    void stats_evict_word(RmtSweeper *sweeper, uint32_t addr, uint64_t data);
    void barrier(RmtSweeper *sweeper, uint16_t lock_id, uint8_t flags);
    void meter_set_sweep_interval(RmtSweeper *sweeper, uint8_t interval);
    void meter_sweep(RmtSweeper *sweeper, uint64_t picosecs);

    bool op_stalled(RmtSweeper *sweeper, const Op &op);
    void op_demux(RmtSweeper *sweeper, const Op &op);

    void sweep(RmtSweeper *sweeper, uint32_t interval_mask, uint64_t picosecs);

    // Funcs to add movereg info, clear it down, remap addrs in Ops
    void add_move(uint8_t addrType, uint32_t fromAddr, uint32_t toAddr);
    void clear_all_moves();
    uint32_t remap_op_addr(const Op &op);


 private:
    TableInfo& operator=(const TableInfo&){ return *this; } // XXX
    void q_op_hi(RmtSweeper *sweeper, const Op &op);
    void q_op_lo(RmtSweeper *sweeper, const Op &op);
    void idle_lock_ack(RmtSweeper *sweeper, uint16_t lock_id);
    void idle_unlock_ack(RmtSweeper *sweeper, uint16_t lock_id);
    void idle_barrier_ack(RmtSweeper *sweeper, uint16_t lock_id);
    void stats_lock_ack(RmtSweeper *sweeper, uint16_t lock_id);
    void stats_unlock_ack(RmtSweeper *sweeper, uint16_t lock_id);
    void stats_barrier_ack(RmtSweeper *sweeper, uint16_t lock_id);

    // Actual funcs which do the work
    void do_idle_set_sweep_interval(RmtSweeper *sweeper, uint8_t interval);
    void do_idle_lock(RmtSweeper *sweeper, uint16_t lock_id);
    void do_idle_unlock(RmtSweeper *sweeper, uint16_t lock_id);
    void do_idle_sweep(RmtSweeper *sweeper, uint64_t picosecs);
    void do_idle_dump(RmtSweeper *sweeper, bool clear);
    void do_idle_dump_word(RmtSweeper *sweeper, uint32_t addr, bool clear);
    void do_stats_set_sweep_interval(RmtSweeper *sweeper, uint8_t interval);
    void do_stats_lock(RmtSweeper *sweeper, uint16_t lock_id);
    void do_stats_unlock(RmtSweeper *sweeper, uint16_t lock_id);
    void do_stats_sweep(RmtSweeper *sweeper, uint64_t picosecs);
    void do_stats_dump(RmtSweeper *sweeper);
    void do_stats_dump_word(RmtSweeper *sweeper, uint32_t addr);
    void do_stats_evict_word(RmtSweeper *sweeper, uint32_t addr, uint64_t data);
    void do_barrier(RmtSweeper *sweeper, uint16_t lock_id, uint8_t flags);
    void do_meter_set_sweep_interval(RmtSweeper *sweeper, uint8_t interval);
    void do_meter_sweep(RmtSweeper *sweeper, uint64_t picosecs);


 private:
    model_core::Spinlock  spinlock_;
    std::list<MoveInfo>  *move_list_;
    TableOpCtl            idle_op_ctl_;
    TableOpCtl            stats_op_ctl_;
    uint16_t              n_queued_ops_;
    uint8_t               n_queued_idle_sweeps_;
    uint8_t               n_queued_stats_sweeps_;
    uint8_t               n_queued_meter_sweeps_;
    uint8_t               pipe_;
    uint8_t               stage_;
    uint8_t               table_;
    uint8_t               flags_;
    uint8_t               idle_interval_;
    uint8_t               stats_interval_;
    uint8_t               meter_interval_;

  }; // TableInfo



  // Async OPs to perform against table
  class Op {
 public:
    static constexpr uint8_t kOpNop = 0;
    static constexpr uint8_t kOpIdleSetInterval = 1;
    static constexpr uint8_t kOpIdleLock = 2;
    static constexpr uint8_t kOpIdleUnlock = 3;
    static constexpr uint8_t kOpIdleSweep = 4;
    static constexpr uint8_t kOpIdleDump = 5;
    static constexpr uint8_t kOpIdleDumpWord = 6;
    static constexpr uint8_t kOpStatsSetInterval = 7;
    static constexpr uint8_t kOpStatsLock = 8;
    static constexpr uint8_t kOpStatsUnlock = 9;
    static constexpr uint8_t kOpStatsSweep = 10;
    static constexpr uint8_t kOpStatsDump = 11;
    static constexpr uint8_t kOpStatsDumpWord = 12;
    static constexpr uint8_t kOpStatsEvictWord = 13;
    static constexpr uint8_t kOpBarrier = 14;
    static constexpr uint8_t kOpMeterSetInterval = 15;
    static constexpr uint8_t kOpMeterSweep = 16;
    static constexpr uint8_t kOpIdleMin = kOpIdleSetInterval;
    static constexpr uint8_t kOpIdleMax = kOpIdleDumpWord;
    static constexpr uint8_t kOpStatsMin = kOpStatsSetInterval;
    static constexpr uint8_t kOpStatsMax = kOpStatsEvictWord;
    static inline bool idle_op(uint8_t op) {
      return ((op >= kOpIdleMin) && (op <= kOpIdleMax));
    }
    static inline bool stats_op(uint8_t op) {
      return ((op >= kOpStatsMin) && (op <= kOpStatsMax));
    }

    Op(uint8_t opcode, TableInfo *ti, uint64_t arg, uint8_t flags, bool tf=true) {
      tableinfo_ = ti;
      opcode_ = opcode;
      arg_ = arg;
      flags_ = flags;
      tf_ = tf;
      addr_ = 0u;
      set_lock_epoch(opcode);
    }
    ~Op() { opcode_ = kOpNop; }

    TableInfo *tableinfo()  const { return tableinfo_; }
    uint8_t    opcode()     const { return opcode_; }
    uint8_t    flags()      const { return flags_; }
    bool       tf()         const { return tf_; }
    uint8_t    arg8()       const { return static_cast<uint8_t>(arg_ & 0xFF); }
    uint16_t   arg16()      const { return static_cast<uint16_t>(arg_ & 0xFFFF); }
    uint32_t   arg32()      const { return static_cast<uint32_t>(arg_ & 0xFFFFFFFF); }
    uint64_t   arg64()      const { return arg_; }
    uint64_t   arg()        const { return arg_; }
    uint32_t   addr()       const { return addr_; }
    uint8_t    addrtype()   const { return flags_; } // Abuse flags_ field for addrtype
    uint8_t    lock_epoch() const { return lock_epoch_; }

    void set_addr(uint32_t a, uint8_t t) { addr_ = a; flags_ = t; }
    void set_lock_epoch(uint8_t opcode)  {
      // Lock epoch will be 0 if table is unlocked
      if      (idle_op(opcode))  lock_epoch_ = tableinfo_->idle_epoch();
      else if (stats_op(opcode)) lock_epoch_ = tableinfo_->stats_epoch();
      else                       lock_epoch_ = 0;
    }


 private:
    TableInfo *tableinfo_;
    uint8_t    opcode_;
    uint8_t    flags_;
    uint8_t    lock_epoch_;
    bool       tf_;
    uint32_t   addr_;
    uint64_t   arg_;

  }; // Op




  // Main class - does timed table sweeps
  class RmtSweeper : public RmtObject, public DefaultLogger {

    static constexpr uint64_t  kOneTickCycles    = MauDefs::kOneTickCycles;
    static constexpr uint64_t  kOneCyclePicosecs = MauDefs::kOneCyclePicosecs;
    static constexpr uint64_t  kOneTickPicosecs  = MauDefs::kOneTickPicosecs;
    static constexpr uint8_t   kMaxInterval = MauDefs::kMaxInterval;
    static constexpr int       kPipes = RmtDefs::kPipesMax;
    static constexpr int       kStages = RmtDefs::kStagesMax;
    static constexpr int       kTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int       kIntervals = kMaxInterval;
    static_assert( (kPipes*kTables <= 64), "Pipe/Table mask must fit in uint64_t");
    static_assert( (kIntervals <= 32), "Interval mask must fit in uint32_t");

    // PT == pipe*16 + table  so  pipe = PT/16, and table = PT%16
    static inline int pt_get_pipe(int pt)  { return pt / kTables; }
    static inline int pt_get_table(int pt) { return pt % kTables; }
    static inline int make_pt(int pipe, int table) {
      RMT_ASSERT((pipe >= 0) && (pipe < kPipes));
      RMT_ASSERT((table >= 0) && (table < kTables));
      return (pipe * kTables) + table;
    }

    static inline int find_first(uint32_t v) { return __builtin_ffsl(v) - 1; }
    static inline int find_last(uint32_t v) {
      return (v == 0u) ?-1 :__builtin_clzl(1u) - __builtin_clzl(v);
    }

 public:
    static constexpr uint8_t kFlagsBarrierStats = 0x1;
    static constexpr uint8_t kFlagsBarrierIdle  = 0x2;
    static constexpr int     kOneTickShift      = MauDefs::kOneTickShift;

    static inline uint64_t cycles_to_psecs(uint64_t cycles) {
      return (cycles * kOneCyclePicosecs);
    }
    static inline uint64_t psecs_to_cycles(uint64_t psecs) {
      return (psecs / kOneCyclePicosecs);
    }
    static inline int psecs_to_ticks(uint64_t psecs) {
      return (psecs / kOneTickPicosecs);
    }
    static inline uint64_t ticks_to_psecs(int ticks) {
      return (ticks * kOneTickPicosecs);
    }
    static inline int interval_to_ticks(int intvl) {
      return (intvl < 0) ?0 :(1 << intvl);
    }
    static inline uint64_t interval_to_psecs(int intvl) {
      return ticks_to_psecs(interval_to_ticks(intvl));
    }
    static inline uint64_t interval_to_cycles(int intvl) {
      return static_cast<uint64_t>( interval_to_ticks(intvl) ) << kOneTickShift;
    }


 public:
    RmtSweeper(RmtObjectManager *om);
    virtual ~RmtSweeper();

    // Upcall DRU funcs
    bool idle_notify(int pipe, int stage, uint64_t msg0);
    bool stats_notify(int pipe, int stage, uint64_t msg0, uint64_t msg1);

    // Just called from tests
    uint32_t get_stage_sweep_cnt(int stage); // Just for testing
    void idle_set_stage_sweep_interval(int pipe, int stage, uint8_t interval);

    // Called from CPU instructions or from CPU register config
    void idle_set_sweep_interval(int pipe, int stage, int table, uint8_t interval);
    void idle_lock(int pipe, int stage, int table, uint16_t lock_id);
    void idle_unlock(int pipe, int stage, int table, uint16_t lock_id);
    void idle_sweep(int pipe, int stage, int table, uint64_t picosecs);
    void idle_dump(int pipe, int stage, int table, bool clear);
    void idle_dump_word(int pipe, int stage, int table, uint32_t addr, bool clear);
    void stats_set_sweep_interval(int pipe, int stage, int table, uint8_t interval);
    void stats_lock(int pipe, int stage, int table, uint16_t lock_id);
    void stats_unlock(int pipe, int stage, int table, uint16_t lock_id);
    void stats_sweep(int pipe, int stage, int table, uint64_t picosecs);
    void stats_dump(int pipe, int stage, int table);
    void stats_dump_word(int pipe, int stage, int table, uint32_t addr);
    void stats_evict_word(int pipe, int stage, int table, uint32_t addr, uint64_t data);
    void barrier(int pipe, int stage, int table, uint16_t lock_id, uint8_t flags);
    void meter_set_sweep_interval(int pipe, int stage, int table, uint8_t interval);
    void meter_sweep(int pipe, int stage, int table, uint64_t picosecs);

    void sweep(int pipe, int stage, int table, uint32_t interval_mask, uint64_t t_now_psecs);
    // Called by timer code in RmtObjectManager
    void sweep(uint64_t t_now_psecs);
    void sweep_increment(uint64_t t_inc_psecs);
    void sweep_increment_cycles(uint64_t cycles);
    uint64_t get_time_now();
    uint64_t get_cycles_now();

    // These upcalled from TableInfo funcs
    void queue_op_hi(const Op &op);
    void queue_op_lo(const Op &op);
    bool queue_check_hi(TableInfo *ti);

    // This called by moveregs
    void update_queued_addr(int pipe, int stage, int table,
                            uint8_t addrtype, uint32_t from_addr, uint32_t to_addr);


    inline void set_interval(int pipe, int stage, int table, uint8_t interval) {
      RMT_ASSERT((pipe >= 0) && (pipe < kPipes));
      RMT_ASSERT((stage >= 0) && (stage < kStages));
      RMT_ASSERT((table >= 0) && (table < kTables));
      RMT_ASSERT(interval < kIntervals);
      pipe_table_map_[interval][stage] |= UINT64_C(1) << make_pt(pipe, table);
      global_interval_mask_ |= (1u << interval);
      per_stage_interval_masks_[stage] |= (1u << interval);
    }
    inline void clear_interval(int pipe, int stage, int table, uint8_t interval) {
      RMT_ASSERT((pipe >= 0) && (pipe < kPipes));
      RMT_ASSERT((stage >= 0) && (stage < kStages));
      RMT_ASSERT((table >= 0) && (table < kTables));
      RMT_ASSERT(interval < kIntervals);
      pipe_table_map_[interval][stage] &= ~(UINT64_C(1) << make_pt(pipe, table));
      if (pipe_table_map_[interval][stage] == UINT64_C(0)) {
        per_stage_interval_masks_[stage] &= ~(1u << interval);
        // Might be able to clear interval entirely - recalc global_interval_mask
        uint32_t tmp_mask = 0u;
        for (int s = 0; s < kStages; s++) tmp_mask |= per_stage_interval_masks_[s];
        global_interval_mask_ = tmp_mask;
      }
    }

 private:
    DISALLOW_COPY_AND_ASSIGN(RmtSweeper); // XXX
    // Convenience
    inline int find_first_interval() { return find_first(global_interval_mask_); }
    inline int find_last_interval()  { return find_last(global_interval_mask_); }

    // Dequeue and demux ops_hi_ and ops_lo_
    void dequeue_hi();
    void dequeue_lo();



 private:
    model_core::Spinlock  spinlock_;

    // Just for testing - count how many times stages get swept
    std::array< uint32_t, kStages>  stage_sweep_cnt_tab_;

    // Array of TableInfos - one for every pipe/stage/table
    std::array< std::array< std::array<TableInfo,kTables >,kStages >,kPipes >  table_info_;
    // 2x doubly-linked lists of async OPs - one hipri one lopri
    std::list<Op>  *ops_hi_, *ops_lo_;

    // Per-interval per-stage we keep a uint64_t PT to track what pipes/tables are active
    // at that interval granularity in that stage
    std::array< std::array<uint64_t,kStages>, kIntervals >   pipe_table_map_;

    // And interval masks to quickly figure out which intervals have stages with
    // non-0 pipe_table_map_ entries (and thus which entries in the pipe_table_map_
    // we need to examine). There's one global one that applies across all stages
    // and also an array of per-stage masks
    uint32_t                        global_interval_mask_;
    std::array< uint32_t, kStages>  per_stage_interval_masks_;

    uint32_t  tick_last_;
    uint32_t  tick_now_;

    uint64_t  tick_last_psecs_;

  }; // RmtSweeper



}

#endif // _SHARED_RMT_SWEEPER_
