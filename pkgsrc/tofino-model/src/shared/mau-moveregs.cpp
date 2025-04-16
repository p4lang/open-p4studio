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
#include <mau-moveregs.h>
#include <register_adapters.h>



namespace MODEL_CHIP_NAMESPACE {


void MauMoveregs::push_table_move_addr(const int addr, const uint64_t relative_time) {
  if (!move_ok()) return;
  mau()->mau_info_incr(MAU_MOVEREGS_PUSH);
  relative_time_ = relative_time;
  shift_and_load(addr);
  inhibit();
  commit();
  prepare();
  update_addresses();
}
void MauMoveregs::pop_table_move_addr(const bool idle_init,
                                      const bool stats_init_ones,
                                      const uint64_t relative_time) {
  if (!move_ok()) return;
  mau()->mau_info_incr(MAU_MOVEREGS_POP);
  relative_time_ = relative_time;
  shift_and_load(-1);
  initialize(idle_init, stats_init_ones);
  inhibit();
  commit();
  prepare();
  update_addresses();
}

bool MauMoveregs::move_ok() {
  bool ok = true;
  if (mau()->mau_teop()->teop_being_used(lt_)) {
    RMT_LOG_OBJ(mau(), RmtDebug::warn(),
                "MauMoveregs::move_ok - Move NOT allowed "
                "for LT %d - table is using TEOP bus\n", lt_);
    ok = false;
  }
  return ok;
}
void MauMoveregs::shift_and_load(const int addr) {
  mau()->lock_resources();
  oldD_addr_ = D_addr_; D_addr_ = S_addr_; S_addr_ = addr;
  mau()->unlock_resources();

  //uint8_t sz = reg_meter_size();
  //printf("MOVEREG: MeterALU: load S=0x%08x D=0x%08x oldD=0x%08x (sz=%d)\n",
  // get_meter_addr(S_addr_,sz), get_meter_addr(D_addr_,sz),
  // get_meter_addr(oldD_addr_,sz), sz);

  if (addr < 0) return; // No addr check on pop_table_move_addr

  if (reg_direct_meter()) {
    int addr_unmasked = get_meter_addr(addr, reg_meter_size(), false);
    int addr_masked   = get_meter_addr(addr, reg_meter_size(), true);
    if (addr_unmasked != addr_masked) {
      RMT_LOG_OBJ(mau(), RmtDebug::warn(),
                  "MauMoveregs::push_table_move_addr - Invalid meter addr 0x%x pushed "
                  "(unmasked=0x%x masked=0x%x)\n",
                  addr, addr_unmasked, addr_masked);
    }
    uint32_t miss_addr = mau()->mau_result_bus()->get_meter_addr_miss(lt_, 99);
    uint32_t mask = Address::kMeterAddrAddrMask & ~Address::kMeterSubwordMask;
    if (Address::meter_addr_is_stateful(miss_addr))
      mask = Address::kMeterAddrAddrMask & ~0x3;
    if ((Address::meter_addr_enabled(miss_addr)) &&
        ((miss_addr & mask) == (static_cast<uint32_t>(addr_unmasked) & mask))) {
      RMT_LOG_OBJ(mau(), RmtDebug::error(),
                  "MauMoveregs::push_table_move_addr - "
                  "meter addr 0x%x is MISS addr for LT %d\n", addr_unmasked, lt_);
    }
  }
  if (reg_direct_stats()) {
    bool tcam = reg_stats_from_tcam();
    int addr_unmasked = get_stats_addr(addr, reg_stats_size(), tcam, false);
    int addr_masked   = get_stats_addr(addr, reg_stats_size(), tcam, true);
    if (addr_unmasked != addr_masked) {
      RMT_LOG_OBJ(mau(), RmtDebug::warn(),
                  "MauMoveregs::push_table_move_addr - Invalid stats addr 0x%x pushed "
                  "(unmasked=0x%x masked=0x%x)\n",
                  addr, addr_unmasked, addr_masked);
    }
    uint32_t miss_addr = mau()->mau_result_bus()->get_stats_addr_miss(lt_, 99);
    uint32_t mask = Address::kStatsAddrAddrMask;
    if ((Address::stats_addr_enabled(miss_addr)) &&
        ((miss_addr & mask) == (static_cast<uint32_t>(addr_unmasked) & mask))) {
      RMT_LOG_OBJ(mau(), RmtDebug::error(),
                  "MauMoveregs::push_table_move_addr - "
                  "stats addr 0x%x is MISS addr for LT %d\n", addr_unmasked, lt_);
    }
  }
  if (reg_direct_idle()) {
    int addr_unmasked = get_idle_addr(addr, reg_idle_size(), false);
    int addr_masked   = get_idle_addr(addr, reg_idle_size(), true);
    if (addr_unmasked != addr_masked) {
      RMT_LOG_OBJ(mau(), RmtDebug::warn(),
                  "MauMoveregs::push_table_move_addr - Invalid idle addr 0x%x pushed "
                  "(unmasked=0x%x masked=0x%x)\n",
                  addr, addr_unmasked, addr_masked);
    }
    uint32_t miss_addr = mau()->mau_result_bus()->get_idletime_addr_miss(lt_, 99);
    uint32_t mask = Address::kIdletimeAddrAddrMask;
    if ((Address::idletime_addr_enabled(miss_addr)) &&
        ((miss_addr & mask) == (static_cast<uint32_t>(addr_unmasked) & mask))) {
      RMT_LOG_OBJ(mau(), RmtDebug::error(),
                  "MauMoveregs::push_table_move_addr - "
                  "idletime addr 0x%x is MISS addr for LT %d\n", addr_unmasked, lt_);
    }
  }
}
void MauMoveregs::initialize(const bool idle_init, const bool stats_init_ones) {
  mau()->lock_resources();
  if ((oldD_addr_ >= 0) && (D_addr_ < 0) && (S_addr_ < 0)) {
    // Do initialize oldD_addr_ using passed-in args - only stats/idle
    if (reg_direct_stats()) {
      bool tcam = reg_stats_from_tcam();
      uint32_t saddr = get_stats_addr(oldD_addr_, reg_stats_size(), tcam);
      uint64_t sdata0 = (stats_init_ones) ?~UINT64_C(0) :UINT64_C(0);
      uint64_t sdata1 = (stats_init_ones) ?~UINT64_C(0) :UINT64_C(0);
      mau()->pbus_write(AddrType::kStats, lt_, saddr, sdata0, sdata1, false, relative_time_);
      // Initialize full-res stats
      mau()->mau_memory()->stats_virt_write_full(lt_, saddr, sdata0, sdata1, relative_time_);
    }
    if (reg_direct_idle()) {
      uint32_t iaddr = get_idle_addr(oldD_addr_, reg_idle_size());
      int shift = Address::idletime_addr_get_shift(iaddr);
      int off = Address::idletime_addr_get_offset(iaddr, shift);
      int idle_initval = get_idle_initval(idle_init, reg_idle_size());
      uint64_t idata0 = static_cast<uint64_t>(idle_initval) << off;
      uint64_t idata1 = ~UINT64_C(0);
      mau()->pbus_write(AddrType::kIdle, lt_, iaddr, idata0, idata1, false, relative_time_);
    }
  }
  mau()->unlock_resources();
}
void MauMoveregs::inhibit() {
  if (reg_tcam_only()) return; // Don't bother if table only uses TCAMs
  if ((D_addr_ >= 0) || (oldD_addr_ >= 0)) {
    for (int row = 0; row < kSramRows; row++) {
      // Inhibit SRAM XM hits
      for (int col = 0; col < kSramColumns; col++) {
        MauSram *sram = mau()->sram_lookup(row,col);
        if ((sram != NULL) && (sram->is_match_sram()) &&
            (sram->get_logical_table() == lt_)) {
          if (D_addr_ >= 0) {
	    sram->inhibit(D_addr_);
          } else if (oldD_addr_ >= 0) {
	    sram->uninhibit();
	  }
        }
      }
      // Inhibit STASH hits
      MauSramRow *sram_row = mau()->sram_row_lookup(row);
      if (sram_row != NULL) {
	MauStash *stash = sram_row->get_stash();
	if (stash != NULL) {
	  for (int which_stash = 0; which_stash < 2; which_stash++) {
	    if (stash->get_logical_table(which_stash) == lt_) {
	      if (D_addr_ >= 0) {
		stash->inhibit(which_stash, D_addr_);
	      } else if (oldD_addr_ >= 0) {
		stash->uninhibit(which_stash);
	      }
	    }
	  }
	}
      }
    }
  }
}
void MauMoveregs::prepare() {
  // Don't think we need to do anything here
}



void MauMoveregs::maybe_commit(const int tcam_hit_addr) {
  // Note this func called from within Logical TCAM lookup
  // so MAU lock should be already held when called

  // See if anything to do - if not return immediately
  if ((tcam_hit_addr < 0) || (S_addr_ < 0) || (D_addr_ < 0)) return;
  if ((tcam_hit_addr != S_addr_) && (tcam_hit_addr != D_addr_)) return;
  if (!kAllowMoveregsCommitOnTcamHit) return;
  if (!move_ok()) return;

  if ((tcam_hit_addr == D_addr_) && (last_commit_to_ != D_addr_)) {
    // If we get hit on address in D then commit S->D
    // (if we've not already done so)
    RMT_LOG_OBJ(mau(), RmtDebug::kRmtDebugVerbose,
                "MauMoveregs::maybe_commit - TCAM hit on D so "
                "committing S -> D (0x%x -> 0x%x) for LT %d\n",
                S_addr_, D_addr_, lt_);
    commit(S_addr_, D_addr_, relative_time_);
    update_addresses(S_addr_, D_addr_);
  }
  else if ((tcam_hit_addr == S_addr_) &&
           (last_commit_from_ == S_addr_) &&
           (last_commit_to_ == D_addr_)) {
    // If we get hit on address in S *after* we have
    // committed S->D then complain
    RMT_LOG_OBJ(mau(), RmtDebug::error(),
                "MauMoveregs::maybe_commit - TCAM hit on S after "
                "committing S -> D (0x%x -> 0x%x) for LT %d\n",
                S_addr_, D_addr_, lt_);
  }
}
void MauMoveregs::commit() {
  mau()->lock_resources();
  commit(D_addr_, oldD_addr_, relative_time_);
  mau()->unlock_resources();
}
void MauMoveregs::commit(int fromAddr, int toAddr, uint64_t T) {
  if ((last_commit_from_ == fromAddr) && (last_commit_to_ == toAddr) &&
      (fromAddr >= 0) && (toAddr >= 0)) {
    // Do nothing if we see duplicate calls to commit
    last_commit_from_ = -1; last_commit_to_ = -1;
    RMT_LOG_OBJ(mau(), RmtDebug::kRmtDebugVerbose,
                "MauMoveregs::commit S -> D (0x%x -> 0x%x) SUPPRESSED for LT %d\n",
                fromAddr, toAddr, lt_);
  } else {
    // Do the commit and then stash last commit from/to
    do_commit(fromAddr, toAddr, T);
    last_commit_from_ = fromAddr; last_commit_to_ = toAddr;
  }
}
void MauMoveregs::do_commit(int fromAddr, int toAddr, uint64_t T) {
  if ((fromAddr >= 0) && (toAddr >= 0)) {
    MauAddrDist *mad = mau()->mau_addr_dist();
    // No need to lock on pbus_read_write and we also only reset backend
    int flags = Mau::kFlagsResetBackend;
    if (reg_direct_meter()) {
      RMT_ASSERT(mad->get_meter_alu(lt_) == reg_meter_lt_to_alu());
      // Can't have direct stats/idle AND stats/idle for meter color
      RMT_ASSERT( ! (reg_direct_stats() && reg_stats_as_mc()) );
      RMT_ASSERT( ! (reg_direct_idle()  && reg_idle_as_mc())  );
      if (reg_stats_as_mc()) flags |= static_cast<int>(MauExecuteState::kFlagsDistribMeterAddrOnStatsBus);
      if (reg_idle_as_mc())  flags |= static_cast<int>(MauExecuteState::kFlagsDistribMeterAddrOnIdleBus);
      uint32_t from_maddr = get_meter_addr(fromAddr, reg_meter_size());
      uint32_t to_maddr = get_meter_addr(toAddr, reg_meter_size());
      RMT_LOG_OBJ(mau(), RmtDebug::kRmtDebugVerbose,
		  "MauMoveregs::do_commit MeterALU: 0x%08x(0x%x) to 0x%08x(0x%x) (flags=0x%02x)\n",
                  from_maddr, fromAddr, to_maddr, toAddr, flags);
      mau()->pbus_read_write(AddrType::kMeter, lt_, from_maddr, to_maddr, flags, T);
    }
    if (reg_direct_stats()) {
      RMT_ASSERT(mad->get_stats_alu(lt_) == reg_stats_lt_to_alu());
      bool tcam = reg_stats_from_tcam();
      uint32_t from_saddr = get_stats_addr(fromAddr, reg_stats_size(), tcam);
      uint32_t to_saddr = get_stats_addr(toAddr, reg_stats_size(), tcam);
      RMT_LOG_OBJ(mau(), RmtDebug::kRmtDebugVerbose,
		  "MauMoveregs::do_commit StatsALU: 0x%08x(0x%x) to 0x%08x(0x%x)\n",
                  from_saddr, fromAddr, to_saddr, toAddr);
      mau()->pbus_read_write(AddrType::kStats, lt_, from_saddr, to_saddr, flags, T);
      // Update full-res stats
      MauMemory *mem = mau()->mau_memory();
      uint64_t sdata0 = UINT64_C(0), sdata1 = UINT64_C(0);
      mem->stats_virt_read_full(lt_, from_saddr, &sdata0, &sdata1, T);
      mem->stats_virt_write_full(lt_, to_saddr, sdata0, sdata1, T);
    }
    if (reg_direct_idle()) {
      uint32_t from_iaddr = get_idle_addr(fromAddr, reg_idle_size());
      uint32_t to_iaddr = get_idle_addr(toAddr, reg_idle_size());
      RMT_LOG_OBJ(mau(), RmtDebug::kRmtDebugVerbose,
		  "MauMoveregs::do_commit IDLE: 0x%08x(0x%x) to 0x%08x(0x%x)\n",
                  from_iaddr, fromAddr, to_iaddr, toAddr);
      mau()->pbus_read_write(AddrType::kIdle, lt_, from_iaddr, to_iaddr, flags, T);
    }
  }
}
void MauMoveregs::update_addresses() {
  mau()->lock_resources();
  update_addresses(D_addr_, oldD_addr_);
  mau()->unlock_resources();
}
void MauMoveregs::update_addresses(int fromAddr, int toAddr) {
  if ((last_upd_from_ == fromAddr) && (last_upd_to_ == toAddr)) {
    // Do nothing if we see duplicate calls to update_addresses
    last_upd_from_ = -1; last_upd_to_ = -1;
  } else {
    // Do the updates and then stash last update from/to
    do_update_addresses(fromAddr, toAddr, kFlagsUpdAll);
    last_upd_from_ = fromAddr; last_upd_to_ = toAddr;
  }
}
// Allow these update_X funcs for backward compatibility
void MauMoveregs::update_deferred_ram() {
  mau()->lock_resources();
  do_update_addresses(D_addr_, oldD_addr_, kFlagsUpdDefRam);
  mau()->unlock_resources();
}
void MauMoveregs::update_queued_color_writes() {
  mau()->lock_resources();
  do_update_addresses(D_addr_, oldD_addr_, kFlagsUpdColWrs);
  mau()->unlock_resources();
}
void MauMoveregs::update_queued_evictions() {
  mau()->lock_resources();
  do_update_addresses(D_addr_, oldD_addr_, kFlagsUpdEvicts);
  mau()->unlock_resources();
}
void MauMoveregs::update_meter() {
  mau()->lock_resources();
  do_update_addresses(D_addr_, oldD_addr_, kFlagsUpdMtrAlu);
  mau()->unlock_resources();
}
void MauMoveregs::do_update_addresses(int fromAddr, int toAddr, uint8_t flags) {
  // Figure out what if anything we're going to update
  bool dir_stats = reg_direct_stats(), def_stats = reg_stats_deferred();
  bool dir_meter = reg_direct_meter(), def_meter = reg_meter_deferred();
  bool col_meter = reg_meter_color();
  bool upd_evicts    = (((flags & kFlagsUpdEvicts) != 0) && dir_stats);
  bool upd_def_stats = (((flags & kFlagsUpdDefRam) != 0) && dir_stats && def_stats);
  bool upd_def_meter = (((flags & kFlagsUpdDefRam) != 0) && dir_meter && def_meter);
  bool upd_col_meter = (((flags & kFlagsUpdColWrs) != 0) && dir_meter && col_meter);
  bool upd_alu_meter = (((flags & kFlagsUpdMtrAlu) != 0) && dir_meter);
  bool upd_stats = (upd_evicts || upd_def_stats);
  bool upd_meter = (upd_def_meter || upd_col_meter || upd_alu_meter);

  if (upd_stats || upd_meter) {
    MauAddrDist *mad = mau()->mau_addr_dist();

    int from_stats = -1, to_stats = -1;
    if (upd_stats) {
      bool tc = reg_stats_from_tcam();
      from_stats = (fromAddr < 0) ?fromAddr :get_stats_addr(fromAddr, reg_stats_size(), tc);
      to_stats = (toAddr < 0) ?toAddr :get_stats_addr(toAddr, reg_stats_size(), tc);
    }
    int from_meter = -1, to_meter = -1;
    if (upd_meter) {
      from_meter = (fromAddr < 0) ?fromAddr :get_meter_addr(fromAddr, reg_meter_size());
      to_meter = (toAddr < 0) ?toAddr :get_meter_addr(toAddr, reg_meter_size());
    }

    if (upd_def_meter) {
      RMT_LOG_OBJ(mau(), RmtDebug::kRmtDebugVerbose,
                  "MauMoveregs::do_update_addresses "
                  "MeterALU: update EOPs from 0x%08x to 0x%08x\n", from_meter, to_meter);
      mad->update_eop_addr(AddrType::kMeter, lt_, from_meter, to_meter);
    }
    if (upd_def_stats) {
      mad->update_eop_addr(AddrType::kStats, lt_, from_stats, to_stats);
    }
    if ((upd_evicts) && (from_stats >= 0) && (to_stats >= 0)) {
      mad->update_queued_addr(AddrType::kStats, lt_, from_stats, to_stats);
    }
    if (upd_col_meter) {
      RMT_ASSERT(mad->get_meter_alu(lt_) == reg_meter_lt_to_alu());
      mad->update_queued_color_writes(reg_meter_lt_to_alu(), from_meter, to_meter);
    }
    if (upd_alu_meter) {
      int alu_index = reg_meter_lt_to_alu();
      int log_row_index = MauMeterAlu::get_meter_alu_logrow_index( alu_index );
      MauLogicalRow* log_row = mau()->logical_row_lookup(log_row_index);
      RMT_ASSERT( log_row );
      MauMeterAlu* meter_alu = log_row->mau_meter_alu();
      RMT_ASSERT( meter_alu );
      meter_alu->update_addresses(from_meter, to_meter);
    }
  }
}

// MauMoveregsCtl contains many MauMoveregs so needs to be declared
// second which means these funcs can't be inline in header file
//
Mau *MauMoveregs::mau()                 { return ctl()->mau(); }
uint8_t MauMoveregs::reg_meter_size()   { return ctl()->reg_meter_size(lt_); }
uint8_t MauMoveregs::reg_stats_size()   { return ctl()->reg_stats_size(lt_); }
uint8_t MauMoveregs::reg_idle_size()    { return ctl()->reg_idle_size(lt_); }
bool MauMoveregs::reg_meter_deferred()  { return ctl()->reg_meter_deferred(lt_); }
bool MauMoveregs::reg_stats_deferred()  { return ctl()->reg_stats_deferred(lt_); }
bool MauMoveregs::reg_stats_from_tcam() { return ctl()->reg_stats_from_tcam(lt_); }
bool MauMoveregs::reg_meter_color()     { return ctl()->reg_meter_color(lt_); }
bool MauMoveregs::reg_stats_as_mc()     { return ctl()->reg_stats_as_mc(lt_); }
bool MauMoveregs::reg_idle_as_mc()      { return ctl()->reg_idle_as_mc(lt_); }
bool MauMoveregs::reg_direct_meter()    { return ctl()->reg_direct_meter(lt_); }
bool MauMoveregs::reg_direct_stats()    { return ctl()->reg_direct_stats(lt_); }
bool MauMoveregs::reg_direct_idle()     { return ctl()->reg_direct_idle(lt_); }
uint8_t MauMoveregs::reg_idle_2way_en() { return ctl()->reg_idle_2way_en(lt_); }
uint8_t MauMoveregs::reg_idle_mode()    { return ctl()->reg_idle_mode(lt_); }
bool MauMoveregs::reg_tcam_only()       { return ctl()->reg_tcam_only(lt_); }
int MauMoveregs::reg_stats_lt_to_alu()  { return ctl()->reg_stats_lt_to_alu(lt_); }
int MauMoveregs::reg_meter_lt_to_alu()  { return ctl()->reg_meter_lt_to_alu(lt_); }








MauMoveregsCtl::MauMoveregsCtl(RmtObjectManager *om,
                               int pipeIndex, int mauIndex, Mau *mau)
    : MauObject(om, pipeIndex, mauIndex, kType, mau),
      moveregs_tab_(),
      movereg_ad_direct_arr_(default_adapter(movereg_ad_direct_arr_,chip_index(), pipeIndex, mauIndex)),
      movereg_stats_ctl_arr_(default_adapter(movereg_stats_ctl_arr_,chip_index(), pipeIndex, mauIndex)),
      movereg_meter_ctl_arr_(default_adapter(movereg_meter_ctl_arr_,chip_index(), pipeIndex, mauIndex)),
      movereg_idle_ctl_arr_(default_adapter(movereg_idle_ctl_arr_,chip_index(), pipeIndex, mauIndex)),
      movereg_ad_stats_alu_to_lt_(default_adapter(movereg_ad_stats_alu_to_lt_,chip_index(), pipeIndex, mauIndex)),
      movereg_ad_meter_alu_to_lt_(default_adapter(movereg_ad_meter_alu_to_lt_,chip_index(), pipeIndex, mauIndex)),
      movereg_idle_pop_ctl_(default_adapter(movereg_idle_pop_ctl_,chip_index(), pipeIndex, mauIndex)),
      mau_cfg_movereg_tcam_only_(default_adapter(mau_cfg_movereg_tcam_only_,chip_index(), pipeIndex, mauIndex))
{
  for (int i = 0; i < kTables; i++) moveregs_tab_[i].init(this, i);
  movereg_ad_direct_arr_.reset();
  movereg_stats_ctl_arr_.reset();
  movereg_meter_ctl_arr_.reset();
  movereg_idle_ctl_arr_.reset();
  movereg_ad_stats_alu_to_lt_.reset();
  movereg_ad_meter_alu_to_lt_.reset();
  movereg_idle_pop_ctl_.reset();
  mau_cfg_movereg_tcam_only_.reset();
}
MauMoveregsCtl::~MauMoveregsCtl() {
}

MauMoveregs *MauMoveregsCtl::get_moveregs(const int lt) {
  return ((lt >= 0) && (lt < kTables)) ?&moveregs_tab_[lt] :NULL;
}
void MauMoveregsCtl::push_table_move_addr(const int lt,
                                          const int addr,
                                          const uint64_t relative_time) {
  MauMoveregs *moveregs = get_moveregs(lt);
  if (moveregs != NULL) moveregs->push_table_move_addr(addr, relative_time);
}
void MauMoveregsCtl::pop_table_move_addr(const int lt,
                                         const bool idle_init,
                                         const bool stats_init_ones,
                                         const uint64_t relative_time) {
  MauMoveregs *moveregs = get_moveregs(lt);
  if (moveregs != NULL) moveregs->pop_table_move_addr(idle_init,
                                                      stats_init_ones,
                                                      relative_time);
}


}
