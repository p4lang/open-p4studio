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
#include <mau-logical-tcam.h>
#include <mau-logical-row.h>
#include <mau-mapram.h>
#include <mau-meter.h>


namespace MODEL_CHIP_NAMESPACE {

  MauMapram::MauMapram(RmtObjectManager *om,
                       int pipeIndex, int mauIndex,
                       int rowIndex, int colIndex, int mapramIndex, Mau *mau)
      : MauObject(om, pipeIndex, mauIndex, kType, rowIndex, colIndex, mau),
        row_index_(rowIndex), col_index_(colIndex), mapram_index_(mapramIndex),
        logcol_index_(mau->logical_column_index(rowIndex,colIndex)),
        lhs_(mau->lhs_sram(rowIndex,colIndex)),
        sweep_index_(0), sweep_subword_(0), notify_error_(false), idle_(false),
        //row_(NULL), column_(NULL),
        logrow_(NULL), sram_(NULL), mapram_(),
        mau_mapram_reg_(om, pipeIndex, mauIndex, rowIndex, colIndex, mapramIndex, this)
  {
    RMT_ASSERT(mapramIndex == mau->mapram_array_index(rowIndex, colIndex));
    // Since regs 13957_mau_dev maprams only on RHS
    RMT_ASSERT((colIndex >= (kMapramColumns/2)) && (colIndex < kMapramColumns));
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCreate),
            "MauMapram::create\n");
  }
  MauMapram::~MauMapram() {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDelete),
            "MauMapram::delete\n");
    logrow_ = NULL;
    sram_ = NULL;
  }


  // Figure out write mask for Mapram writes - chip-specific func
  // (see src/<chip>/mau-mapram.cpp)
  //
  // uint64_t MauMapram::get_write_mask(uint64_t data0, uint64_t data1);


  void MauMapram::reset_resources() {
  }

  // Keep tally of reads/writes
  void MauMapram::idletime_tally(bool read, bool written) {
    if (read)    mau()->mau_info_incr(MAU_IDLETIME_MAPRAMS_READ);
    if (written) mau()->mau_info_incr(MAU_IDLETIME_MAPRAMS_WRITTEN);
  }

  // Send a notification that a word became active/idle
  void MauMapram::idletime_notify(int index, uint64_t msg, int addr_off) {
    if (!idletime_notify_enabled()) return;
    Mau *mauobj = mau();
    RMT_ASSERT(mauobj != NULL);
    RmtObjectManager *om = mauobj->get_object_manager();
    RMT_ASSERT(om != NULL);
    RmtSweeper *sweeper = om->sweeper_get();
    RMT_ASSERT(sweeper != NULL);    
    int pipe = mauobj->pipe_dump_index();
    int stage = mauobj->mau_dump_index();
    int logtab = get_logical_table(); // From mapram reg
    int vpn = get_vpn();
    int vpn_off  = addr_off + 10;
    int pst_off  = vpn_off + 6;
    int ps_off = pst_off + MauDefs::kTableBits;
    int p_off = ps_off + RmtDefs::kStageBits;    
    msg |= static_cast<uint64_t>(index & 0x3FF)                << addr_off;
    msg |= static_cast<uint64_t>(vpn & 0x3F)                   << vpn_off;
    msg |= static_cast<uint64_t>(logtab & MauDefs::kTableMask) << pst_off;
    msg |= static_cast<uint64_t>(stage & RmtDefs::kStageMask)  << ps_off;
    msg |= static_cast<uint64_t>(pipe & RmtDefs::kPipeMask)    << p_off;
    // Call RMT-level idle notifier here - returns true if OK
    notify_error_ = !sweeper->idle_notify(pipe, stage, msg);
  }

  // Send a notification that a word became active/idle
  void MauMapram::idletime_fsm_notify(int index, int subword, bool active, bool idle) {
    RMT_ASSERT((index >= 0) && (index < kMapramEntries));
    RMT_ASSERT((subword >= 0) && (subword < 4));
    uint64_t msg = kIdletimeFsmMsgType; // Idletime FSM == type 0
    // Idle/Active bits in dump msg DO NOT occupy same pos as their subword's position
    // in mapram entry - so we use idletime_dump_offset here NOT idletime_offset
    int ia_off   = kIdletimeFsmMsg_IdleActiveOffset + idletime_dump_offset(subword);
    msg |= static_cast<uint64_t>(((active ?1 :0) << 1) | ((idle ?1 :0) << 0)) << ia_off;
    idletime_notify(index, msg, kIdletimeFsmMsg_AddressOffset);
  }
  // Dump a word
  void MauMapram::idletime_dump_notify(int index, uint8_t data) {
    RMT_ASSERT((index >= 0) && (index < kMapramEntries));
    uint64_t msg = kIdletimeDumpMsgType; // Idletime DUMP == type 2
    msg |= static_cast<uint64_t>(data) << kIdletimeDumpMsg_DataOffset;
    idletime_notify(index, msg, kIdletimeDumpMsg_AddressOffset);
  }

  // Handle sweep of single subword maybe notifying and return whether word was already idle
  bool MauMapram::idletime_sweep(int index, int subword, int off, int nbits, bool twoway,
                                 uint64_t max_val, uint64_t incr_eq0, uint64_t incr_ne0) {
    uint64_t orig_val = mapram_.incr_word(index, off, nbits, max_val, incr_eq0, incr_ne0);
    bool was_idle = (orig_val == max_val);
    bool is_idle = (orig_val + incr_ne0 == max_val);
    bool is_active = ((twoway) && (orig_val == UINT64_C(0)));

    uint64_t new_val = mapram_.get_word(index, off, nbits);
    if (is_active || is_idle || (orig_val != new_val))
      RMT_LOG(RmtDebug::kRmtDebugMauMapramIdleSweep,
	      "MauMapram::idletime_sweep(p=%d,s=%d,r=%d,c=%d,i=%d,sub=%d ORIG=%d->NOW=%d)\n",
	      pipe_index(), mau_index(), row_index_, col_index_, index, subword,
	      (int)(orig_val & 0x3FF), (int)(new_val & 0x3FF));
    if (is_active || is_idle) idletime_fsm_notify(index, subword, is_active, is_idle);
    idletime_tally(true, (orig_val != new_val));
    return was_idle;
  }

  // Handle sweep of single subword
  bool MauMapram::idletime_sweep_one(int index, int subword) {
    int off = idletime_offset(subword);
    int nbits = idletime_width();
    // No sweep in 1b mode
    if (nbits == 1) return true;
    bool perflow = idletime_perflow();
    bool twoway = idletime_2way();
    // In 2b mode bail if both perflow/twoway - invalid
    if ((nbits == 2) && (twoway) && (perflow)) return true;
    uint64_t ONE = UINT64_C(1);
    // maxval is 2^N-2 if perflow enabled otherwise 2^N-1
    uint64_t maxval = (perflow) ?((ONE << nbits) - ONE - ONE) :((ONE << nbits) - ONE);
    // Val 0 gets incremented 0->2 in twoway mode
    uint64_t incrEQ0 = (twoway) ?ONE+ONE :ONE;
    return idletime_sweep(index, subword, off, nbits, twoway, maxval, incrEQ0, ONE);
  }
  // Handle sweep of whole word
  bool MauMapram::idletime_sweep_word(int index) {
    bool is_idle = true;
    for (int sub = 0; sub < idletime_nentries(); sub++) {
      if (!idletime_sweep_one(index, sub)) is_idle = false;
    }
    return is_idle;
  }

  // Handle sweep on an entire MapRAM - return whether whole mapRAM now idle
  bool MauMapram::idletime_sweep_all() {
    if ((!is_idletime_mapram()) || (!idletime_enabled())) return true;
    if (!is_valid_idletime_mode()) return true;    
    
    // Calculate ONCE all values which stay fixed for whole mapRAM
    // Note1: No sweep in 1b mode - treat as if completely idle
    // Note2: In 2b mode both perflow/twoway is invalid - treat as if completely idle
    int nbits = idletime_width();
    if (nbits == 1) return true; 
    bool perflow = idletime_perflow();
    bool twoway = idletime_2way();
    if ((nbits == 2) && (twoway) && (perflow)) return true;
    
    bool is_idle = true;
    uint64_t ONE = UINT64_C(1);
    // maxval is 2^N-2 if perflow enabled otherwise 2^N-1
    uint64_t maxval = (perflow) ?((ONE << nbits) - ONE - ONE) :((ONE << nbits) - ONE);
    // Val 0 gets incremented 0->2 in twoway mode
    uint64_t incrEQ0 = (twoway) ?ONE+ONE :ONE;

    // Now go through all indexes and all subwords doing sweep
    // We pick up where we left off. If we scan through whole
    // mapRAM successfully we should leave this func with the
    // sweep_index_ sweep_subword_ values both back at 0.
    //
    int nSubwords = idletime_nentries();
    while (sweep_index_ < kMapramEntries) {
      while (sweep_subword_ < nSubwords) {
        int off = idletime_offset(sweep_subword_);
        if (!idletime_sweep(sweep_index_, sweep_subword_, off, nbits,
                            twoway, maxval, incrEQ0, ONE))
          is_idle = false;
        // If we get a notify error we bail out - will restart on next call
        if (notify_error_) return false;
        sweep_subword_++;
      }
      sweep_subword_ = 0;
      sweep_index_++;
    }
    sweep_index_ = 0;
    return is_idle;
  }

  // Handle hit on single subword
  void MauMapram::idletime_active_one(int index, int subword) {
    int off = idletime_offset(subword);
    int nbits = idletime_width();
    bool perflow = idletime_perflow();
    bool twoway = idletime_2way();
    // In 2b mode bail if both perflow/twoway - invalid
    if ((nbits == 2) && (twoway) && (perflow)) return;

    uint64_t ZERO = UINT64_C(0);
    uint64_t ONE = UINT64_C(1);
    // Read current val
    uint64_t inval = mapram_.get_word(index, off, nbits);
    // Set satval to a huge number if perflow enabled so vals 63/7/3 do not get reset
    uint64_t satval = (perflow) ?((ONE << nbits) - ONE) :UINT64_C(999999999);
    // If 2b/3b/6b mode and perflow and val saturated we're done - bail out
    if ((nbits > 1) && (perflow) && (inval == satval)) return;

    // maxval is 2^N-2 if perflow enabled otherwise 2^N-1
    uint64_t maxval = (perflow) ?((ONE << nbits) - ONE - ONE) :((ONE << nbits) - ONE);
    // In twoway mode we reset to 1 (if val>0 and val<maxval) else 0
    uint64_t resetval = (twoway) ?ONE :ZERO;
    uint64_t outval = ZERO;

    if (nbits == 1)
      mapram_.set_word(index, off, nbits, ONE); // 1b mode always resets to 1
    else
      outval = mapram_.reset_word(index, off, nbits, ZERO, maxval, resetval);
    RMT_LOG(RmtDebug::kRmtDebugMauMapramIdleActive,
	    "MauMapram::idletime_active(p=%d,s=%d,r=%d,c=%d,i=%d,sub=%d "
	    "BUS=%d,MAX=%d,RST=%d,perflow=%d,2way=%d) IN=%d OUT=%d\n",
	    pipe_index(), mau_index(), row_index_, col_index_, index, subword,
	    idletime_bus(), (int)(maxval & 0x3FF), (int)(resetval & 0x3FF),
	    perflow, twoway, (int)(inval & 0x3FF), (int)(outval & 0x3FF));
    idletime_tally(true, (inval != outval));
  }


  // Handle read of single word 
  uint64_t MauMapram::idletime_read_word(int index, bool clear) {
    uint64_t word = UINT64_C(0);
    int width = idletime_width()*idletime_nentries();
    RMT_ASSERT(width <= 8);
    if (clear) {
      // Reset to configured clear_val
      uint64_t cfg_val = static_cast<uint64_t>(idletime_rd_clear_val());
      word = mapram_.swap_word(index, 0, width, cfg_val);
      idletime_tally(true, true);
    } else {
      // No reset - leave data as is - just read
      word = mapram_.get_word(index, 0, width);
      idletime_tally(false, false); // Dont tally cfgRd
    }
    return word;
  }

  // Figure out whether the width in an addr matches the width of
  // entries in this idletime mapram (occasionally update width
  // to reflect larger width available if op is CfgWr)
  bool MauMapram::idletime_check_width(uint32_t addr, int *width) {
    int idle_addr_shift = Address::idletime_addr_get_shift(addr);
    int idle_addr_nents = Address::idletime_addr_get_nentries(addr, idle_addr_shift);
    int idle_addr_op = Address::idletime_addr_op(addr);
    
    if ((idle_addr_op == Address::kIdletimeOpCfgWr) && (idle_addr_nents == 1)) {
      // Address is a CfgWr of 1 entry - maybe update width to 8 dependent on 
      // parity/ECC (allows single-write whole-word initialization)
      // And if neither parity nor ECC set width 8 assumed.
      int idle_addr_width = Address::idletime_addr_get_width(addr, idle_addr_shift);
      RMT_ASSERT(idle_addr_width == 6);
      int ret_width = 8;
      if      (has_parity())  ret_width = 8;
      else if (has_ecc())     ret_width = 6;
      if (width != NULL) *width = ret_width; // Update passed-in width
      return true;
    } else {
      // Check number entries matches in address and ram if not CfgWr 
      // (or CfgWr and >1 ent)
      int idle_ram_nents = idletime_nentries();
      if (idle_addr_nents != idle_ram_nents) {
        RMT_LOG(RmtDebug::error(),
                "MauMapram::idletime_check_width(0x%x) "
                "Address #entries=%d does not equal Mapram #entries=%d\n",
                addr, idle_addr_nents, idle_ram_nents);
      }
      return (idle_addr_nents == idle_ram_nents); // Leave passed-in width as it was
    }
  }

  // Handle write of single subword - handles rd_subword and
  // wr_subword being different - but that only happens during moveregs
  void MauMapram::idletime_write_one(int index, int rd_subword, int wr_subword,
                                     int width, uint64_t data) {
    int rd_off = idletime_offset(rd_subword);
    int wr_off = idletime_offset(wr_subword);
    // Note S/W will knows correct offset/width and so data
    // word will have subword bits in correct position.
    // So we need to shift and mask before calling set_word
    uint64_t subdata = (data >> rd_off) & ((1<<width)-1);
    mapram_.set_word(index, wr_off, width, subdata);
    idletime_tally(false, false); // Dont tally cfgWr
  }


  // Handle dump of single word
  void MauMapram::idletime_dump_one(int index, bool clear) {
    uint64_t data = idletime_read_word(index, clear);
    idletime_dump_notify(index, static_cast<uint8_t>(data & 0xFF));
  }
  // Handle dump of an entire MapRAM
  void MauMapram::idletime_dump_all(bool clear) {
    if ((!is_idletime_mapram()) || (!idletime_enabled())) return;
    if (!is_valid_idletime_mode()) return;    
    for (int index = 0; index < kMapramEntries; index++) {
      idletime_dump_one(index, clear);
    }
  }

  // See if this idletime MapRAM handles this addr
  bool MauMapram::idletime_handles_addr(uint32_t addr) {
    if (!idletime_enabled()) return false;
    if (!is_valid_idletime_mode()) return false;
    if (!Address::idletime_addr_op_enabled(addr)) return false;
    // Huffman decode to get shift - should be in [1,4]
    int shift = Address::idletime_addr_get_shift(addr);
    if ((shift < 1) || (shift > 4)) return false;
    // Check selected subword in range
    int subword = Address::idletime_addr_get_subword(addr, shift);
    if (subword >= idletime_nentries()) return false;
    // Check VPN matches
    int vpn = Address::idletime_addr_get_vpn(addr);
    if (!vpn_match(vpn)) return false;
    return true;
  }

  void MauMapram::idletime_handle(MauExecuteState *state, uint32_t addr) {
    if (!idletime_handles_addr(addr)) return;

    mau()->mau_addr_dist()->idletime_addr_consume(addr); // Sanity check

    int shift = Address::idletime_addr_get_shift(addr);
    int width = Address::idletime_addr_get_width(addr, shift);
    int wr_subword = Address::idletime_addr_get_subword(addr, shift);
    //int vpn = Address::idletime_addr_get_vpn(addr);
    int index = Address::idletime_addr_get_index(addr);
    int op = Address::idletime_addr_op(addr);
    bool width_match = true;
    bool clear = false;

    // If we have a valid addr in the info_ field then we're probably doing
    // moveregs and the info_addr should be used to figure out a rd_subword
    // within the state_data which might be different to the wr_subword.
    int rd_subword = wr_subword;
    if ((state->rw_format_ > 0) && (Address::idletime_addr_op_enabled(state->rw_raddr_))) {
      RMT_ASSERT(state->rw_format_ == idletime_width()); // Insist width same for rd/wr
      int rd_shift = Address::idletime_addr_get_shift(state->rw_raddr_);
      rd_subword = Address::idletime_addr_get_subword(state->rw_raddr_, rd_shift);
    }
    
    switch (op) {
      case Address::kIdletimeOpMarkActive:
	// Will log an error if #entries in addr/mapram mismatch
	if (idletime_check_width(addr, NULL)) 
	  idletime_active_one(index, wr_subword);
        break;
      case Address::kIdletimeOpSweep:
        state->ret_ = (idletime_sweep_word(index)) ?1 :0;
        break;
      case Address::kIdletimeOpCfgRdClr: clear = true; // FALLTHROUGH
      case Address::kIdletimeOpCfgRd:      
        state->data_.set_word(idletime_read_word(index, clear), 0);
        state->ret_ = 1;
        // Stash width in case doing movereg read/write 
        state->rw_format_ = idletime_width(); 
        break;
      case Address::kIdletimeOpCfgWr:
        width_match = idletime_check_width(addr, &width);
	// Mismatches should have been caught in MauMemory::idletime_virt_write
        RMT_ASSERT(width_match); 
        idletime_write_one(index, rd_subword, wr_subword,
                           width, state->data_.get_word(0));
        break;
    }
  }


  // Do CfgRd op on read path as there might not be a write phase
  void MauMapram::run_read_idletime(MauExecuteState *state) {
    uint32_t addr = idletime_addr();
    if (Address::idletime_addr_op(addr) == Address::kIdletimeOpCfgRd)
      idletime_handle(state, addr);
  }
  // Do all other Ops on write now 
  void MauMapram::run_write_idletime(MauExecuteState *state) {
    uint32_t addr = idletime_addr();
    if (Address::idletime_addr_op(addr) != Address::kIdletimeOpCfgRd)
      idletime_handle(state, addr);
  }

  // this is called from MauSramRow::srams_run_color_mapram_read
  void MauMapram::run_read_color(MauExecuteState *state) {
    if (!is_color_mapram()) return;
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMapramColorRead),
            "MauMapram::run_read_color\n");

    uint8_t addr_type;
    uint32_t addr = color_read_addr(&addr_type);
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMapramColorRead),
            "MauMapram::run_read_color addr=0x%x (type=%d)\n", addr, addr_type);
    if (addr_type == AddrType::kNone) return;
    // Temporarily reverse any stats bus/idle bus shift before checking meter_op
    int mtr_shift = Address::color_addr_get_shift(addr, AddrType::kMeter);
    int bus_shift = Address::color_addr_get_shift(addr, addr_type);
    if (!Address::meter_addr_op_enabled(Address::addrShift(addr, bus_shift-mtr_shift)))
      return;

    int subword = Address::color_addr_get_subword(addr,addr_type);
    RMT_ASSERT( subword >= 0 && subword < 4 );
    int vpn = Address::color_addr_get_vpn(addr,addr_type);
    int index = Address::color_addr_get_index(addr,addr_type);

    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMapramColorRead),
            "MauMapram::run_read_color addr=0x%x vpn=0x%x index=%d vpn_match=%d\n",
            addr,vpn,index,vpn_match(vpn));

    // Check VPN matches
    if (!vpn_match(vpn)) return;
    
    process_pending_color_writes(state);

    // If address ok mark up as consumed on both stats rows and idletime buses
    mau()->mau_addr_dist()->idletime_addr_consume(addr); 
    mau()->mau_addr_dist()->stats_addr_consume(addr);
    mau()->mau_info_incr(MAU_COLOR_MAPRAMS_READ); // Keep tally

    
    uint64_t ram_data = mapram_.get_word(index,0,kMapramWidth);
    uint8_t color = kMapramColorEntryMask & (ram_data >> (subword*kMapramColorEntryWidth));
    auto color_bus_drive = mau_mapram_reg_.color_bus_drive();
    if ( color_bus_drive != MauMapramReg::kMapramNoColorBus ) {
      RMT_ASSERT(!lhs_ && "Mapram on LHS of unitram array");
      if ( color_bus_drive == MauMapramReg::kMapramColorBus ) {
        physical_row_->set_right_color_bus(color, col_index_, get_color_alu_index());
      }
      if ( color_bus_drive == MauMapramReg::kMapramOverflowColorBus ) {
        physical_row_->set_right_color_overflow_bus(color, col_index_, get_color_alu_index());
      }
    }
    // Also stash copies of color into state    
    if (state->op_ == kStateOpPbusRd) state->rd_color_ = color;
    state->color_ = color; 
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMapramColorRead),
	    "MauMapram::run_read_color addr=0x%x vpn=0x%x index=%d subword=%d "
            "ram_data=%" PRIx64 " color=%d color_bus_drive=%d\n",
            addr,vpn,index,subword,ram_data,color,color_bus_drive);
  }

  void MauMapram::run_write_color(MauExecuteState *state) {
    if (!is_color_mapram()) return;

    uint32_t addr = mau_mapram_reg_.color_write_addr();
    if (!Address::meter_addr_op_enabled(addr)) return;

    uint8_t color_data = mau_mapram_reg_.color_write_data();
    uint8_t color = MauMeter::color_extract( color_data );
    bool    color_inhibit = MauMeter::color_inhibit( color_data );
    if (color_inhibit) return;
    
    uint8_t addr_type = AddrType::kMeter;
    int subword = Address::color_addr_get_subword(addr,addr_type);
    RMT_ASSERT( subword >= 0 && subword < 4 );
    int vpn = Address::color_addr_get_vpn(addr,addr_type);
    int index = Address::color_addr_get_index(addr,addr_type);

    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMapramColorWrite),
           "MauMapram::run_write_color addr=0x%x vpn=0x%x index=%d vpn_match=%d\n",
           addr,vpn,index,vpn_match(vpn));

    if (state->op_ == kStateOpSweep) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMapramColorWrite),
	      "MauMapram::run_write_color sweep so skipping write\n");
      return;
    }
	      
    
    // Check VPN matches
    if (!vpn_match(vpn)) return;

    
    // Normally we EITHER do an immediate commit OR queue a deferred write
    //  of the bus color (calculated from 128b data) depending on whether
    //  there's a relative_time value in state obj
    // Only exception (one day) is movereg where we may immediately commit
    //  SRC color read from the SRC address mapram (kept in state obj)
    //  and ALSO queue the bus color
    uint64_t relative_time = UINT64_C(0);
    bool doing_movereg = Address::meter_addr_op_enabled(state->rw_raddr_);
    bool do_q_bus_color = (state->get_relative_time(&relative_time,is_ingress()));
    bool do_imm_commit_color = !do_q_bus_color || doing_movereg;
    // TEMP? However for now we don't Q the bus color when doing movereg
    if (doing_movereg) do_q_bus_color = false;
    
    if (do_imm_commit_color) {
      // do an immediate write of imm_color (movereg src color OR bus color)
      uint8_t imm_color = (doing_movereg) ?state->rd_color_ :color;
      update_color(addr, imm_color);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMapramColorWrite),
              "MauMapram::run_write_color IMM addr=0x%x vpn=%d index=%d subword=%d "
              "color=%d %s\n", addr, vpn, index, subword, imm_color,
              (doing_movereg) ?"MOVEREG" :"");
    }
    if (do_q_bus_color) {
      // queue deferred write of q_color (bus color) to per-ALU Q
      uint8_t wr_latency;
      if      (state->at_teop_) wr_latency = kMapramColorWriteLatencyTEOP;
      else if (state->at_eop_)  wr_latency = MauDefs::kMapramColorWriteLatencyEOP;
      else                      wr_latency = MauDefs::kMapramColorWriteLatency;
      uint8_t q_color = color;
      mau()->mau_addr_dist()->queue_color_write(get_meter_alu_index(),
                                                relative_time, wr_latency,
                                                this, addr, q_color);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMapramColorWrite),
              "MauMapram::run_write_color QUEUE addr=0x%x vpn=%d index=%d subword=%d "
              "color=%d time_now=%" PRIu64 " time_of_write=%" PRIu64 " %s\n",
              addr, vpn, index, subword, q_color, relative_time, relative_time,
              (doing_movereg) ?"MOVEREG" :"");
    }
  }


  void MauMapram::process_pending_color_writes(uint64_t relative_time) {
    // Maybe triggers callback of deferred_update_color for this mapram
    if (!is_color_mapram() || (get_meter_alu_index() < 0)) return;
    mau()->mau_addr_dist()->dequeue_color_writes(get_meter_alu_index(),
                                                 relative_time, this);
  }
  void MauMapram::process_pending_color_writes(MauExecuteState* state) {
    if (state->op_ == kStateOpPbusRd) return; // No Q flush if CfgRd    
    uint64_t relative_time = UINT64_C(0);
    if ((!state->get_relative_time(&relative_time,is_ingress())) || (relative_time == UINT64_C(0)))
      relative_time = UINT64_C(0xFFFFFFFFFFFFFFFF);
    // NB. If no time specified this will empty whole Q of entries
    process_pending_color_writes(relative_time);
  }
  void MauMapram::process_pending_color_writes() {
    uint64_t relative_time = get_object_manager()->time_get_cycles();
    process_pending_color_writes(relative_time);
  }
  void MauMapram::update_color(uint32_t addr, uint8_t color) {
    int index = Address::color_addr_get_index(addr, AddrType::kMeter);
    int subword = Address::color_addr_get_subword(addr, AddrType::kMeter);
    int offset = subword * kMapramColorEntryWidth;
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMapramColorWrite),
	    "MauMapram::update_color addr=0x%08x index=%d offset=%d color=%d\n",
            addr, index, offset, color);
    mapram_.set_word(index, offset, kMapramColorEntryWidth, color);
    mau()->mau_info_incr(MAU_COLOR_MAPRAMS_WRITTEN); // Keep tally
  }
  // This func called back from MauAddrDist::dequeue_color_writes
  void MauMapram::deferred_update_color(uint32_t addr, uint8_t color,
                                        uint64_t time_now, uint64_t time_of_write) {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMapramColorWrite),
	    "MauMapram::deferred_update_color DeQUEUE addr=0x%08x color=%d "
            "time_now=%" PRIu64 " time_of_write=%" PRIu64 "\n",
            addr, color, time_now, time_of_write);
    update_color(addr, color);
  }


  void MauMapram::run_read(MauExecuteState *state) {
    RMT_ASSERT(logrow_ != NULL);
    if (is_statistics_mapram()) {
    } else if (is_meter_mapram()) {
    } else if (is_stateful_mapram()) {
    } else if (is_idletime_mapram()) {
      run_read_idletime(state);
    } else if (is_color_mapram()) {
    } else if (is_selector_mapram()) {
    }
  }
  void MauMapram::run_write(MauExecuteState *state) {
    RMT_ASSERT(logrow_ != NULL);
    if (is_statistics_mapram()) {
    } else if (is_meter_mapram()) {
    } else if (is_stateful_mapram()) {
    } else if (is_idletime_mapram()) {
      run_write_idletime(state);
    } else if (is_color_mapram()) {
      run_write_color(state);
    } else if (is_selector_mapram()) {
    }
  }


}
