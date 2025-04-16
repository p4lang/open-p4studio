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
#include <address.h>
#include <mau-logical-tcam.h>
#include <mau-logical-row.h>
#include <mau-sram.h>


namespace MODEL_CHIP_NAMESPACE {

  MauSram::MauSram(RmtObjectManager *om,
                   int pipeIndex, int mauIndex,
                   int rowIndex, int colIndex, int sramIndex, Mau *mau)
      : MauObject(om, pipeIndex, mauIndex, kType, rowIndex, colIndex, mau),
        row_index_(rowIndex), col_index_(colIndex), sram_index_(sramIndex),
        logrow_index_(mau->logical_row_index(rowIndex,colIndex)),
        logcol_index_(mau->logical_column_index(rowIndex,colIndex)),
        logical_tcam_(-1), logical_table_(-1), logical_tables_(0),
        hit_mask_(0), inhibit_hit_mask_(0), inhibit_hit_index_(0xFFFF), type_(0),
        pending_state_(kPendingStateReset), pending_index_(0),
        pending_data0_(UINT64_C(0)), pending_data1_(UINT64_C(0)),
        result_bus_(mau->mau_result_bus()),
        logrow_(NULL), row_(NULL), column_(NULL), mapram_(NULL),
        vpns_(), sram_(), masks_(),
        mau_sram_reg_(om, pipeIndex, mauIndex, rowIndex, colIndex, sramIndex, this)
  {
    RMT_ASSERT(sramIndex == mau->sram_array_index(rowIndex, colIndex));
    static_assert(kVpnsPerMau <= 0xFFFF, "vpn must fit in uint16_t");
    static_assert(kVpnsPerMau < 0xFFFF, "vpn 0xFFFF reserved for invalid");
    static_assert(kMaskEntries <= 8, "mask_enables returns uint8_t");
    for (int i = 0; i < kVpns; i++) vpns_[i] = 0xFFFF;
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCreate),
            "MauSram::create\n");
  }
  MauSram::~MauSram() {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDelete),
            "MauSram::delete\n");
    logrow_ = NULL;
    row_ = NULL;
    column_ = NULL;
    mapram_ = NULL;
  }


  void MauSram::update_vpns() {
    for (int i = 0; i < kVpns; i++) {
      uint16_t old_vpn = vpns_[i];
      uint16_t new_vpn = mau_sram_reg_.get_vpn(i);
      if (old_vpn != new_vpn) {
        Mau *mauobj = mau();
        RMT_ASSERT(mauobj != NULL);
        vpns_[i] = 0xFFFF;
        if (old_vpn < kVpnsPerMau)
          mauobj->vpn_remove_sram(old_vpn, this);
        if (new_vpn < kVpnsPerMau)
          mauobj->vpn_add_sram(new_vpn, this);
        vpns_[i] = new_vpn;
      }
    }
  }
  void MauSram::update(int new_ltcam, uint16_t new_logtabs, int new_type) {
    Mau *mauobj = mau();
    RMT_ASSERT(mauobj != NULL);
    int old_ltcam = logical_tcam_;
    int old_logtabs = logical_tables_;
    int old_type = type_;
    MauLogicalTcam *ltold = NULL;
    if (old_ltcam >= 0) ltold = mauobj->logical_tcam_lookup(old_ltcam);
    MauLogicalTcam *ltnew = NULL;
    if (new_ltcam >= 0) ltnew = mauobj->logical_tcam_lookup(new_ltcam);
    MauSramColumn *colold = NULL;
    if (MauDefs::is_match_type(old_type)) colold = column();
    MauSramColumn *colnew = NULL;
    if (MauDefs::is_match_type(new_type)) colnew = column();
    logical_tcam_ = new_ltcam;
    logical_table_ = mau_sram_reg_.get_logical_table();
    logical_tables_ = new_logtabs;
    type_ = new_type;
    if ((new_type != old_type) || (old_ltcam != new_ltcam)) {
      if ((colold != NULL) || (colnew != NULL) || (ltold != NULL) || (ltnew != NULL))
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramUpdate),
                "MauSram::update<%d>  new_ltcam=%d(%d) new_logtabs=0x%04x(0x%04x) new_type=%d(%d)\n",
                sram_index(), new_ltcam,old_ltcam, new_logtabs,old_logtabs, new_type,old_type);
      if (colold != NULL) colold->remove_match_sram(this, old_logtabs);
      if (ltold != NULL) ltold->remove_tind_sram(this, old_logtabs);
      if (colnew != NULL) colnew->add_match_sram(this, new_logtabs);
      if (ltnew != NULL) ltnew->add_tind_sram(this, new_logtabs);
    } else if (new_logtabs != old_logtabs) {
      if ((colnew != NULL) || (ltnew != NULL))
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramUpdate),
                "MauSram::update<%d>  new_ltcam=%d(%d) new_logtabs=0x%04x(0x%04x) new_type=%d(%d)\n",
                sram_index(), new_ltcam,old_ltcam, new_logtabs,old_logtabs, new_type,old_type);
      if (colnew != NULL) colnew->update_match_sram(this, new_logtabs, old_logtabs);
      if (ltnew != NULL) ltnew->update_tind_sram(this, new_logtabs, old_logtabs);
    }
  }
  void MauSram::update_logical_tables(uint16_t new_logtabs, uint16_t old_logtabs) {
    int lt = mau_sram_reg_.get_logical_table();
    if (new_logtabs == 0) {
      RMT_LOG(RmtDebug::warn(),
              "MauSram::update_logical_tables() - match_to_logical_table_ixbar==0 - "
              "will use unitram LT=%d instead\n", lt);
      new_logtabs |= (1<<lt);
    } else if (((new_logtabs >> lt) & 1) == 0) {
      RMT_LOG(RmtDebug::warn(),
              "MauSram::update_logical_tables() - logical table MISMATCH in unitram\n");
    }
    update(logical_tcam_, new_logtabs, type_);
  }


  bool MauSram::get_next_table(const int index, int which_next_table,
                               uint32_t *next_table) {
    if (next_table == NULL) return false;
    BitVector<kSramWidth> val;
    uint32_t bitpos;
    if ((mau_sram_reg_.get_next_table_bitpos(which_next_table, &bitpos)) &&
        (get(index, &val))) {
      uint64_t bv_word = val.get_word(bitpos, kNextTableWidth);
      *next_table = static_cast<uint32_t>(bv_word);
    } else {
      *next_table = 0u;
    }
    return true;
  }

  void MauSram::inhibit(const uint32_t inhibit_addr) {
    int vpn = static_cast<uint16_t>(Address::xm_match_addr_get_vpn(inhibit_addr));
    uint16_t index = static_cast<uint16_t>(Address::xm_match_addr_get_index(inhibit_addr));
    uint8_t  inhibits = 0;
    for (int i = 0; i < kMatchEntries; i++) {
      if (get_vpn(i) == vpn) inhibits |= 1<<i;
    }
    inhibit_hit_index_ = 0xFFFF;
    inhibit_hit_mask_ = inhibits;
    if (inhibits != 0) inhibit_hit_index_ = index;
  }
  void MauSram::uninhibit() {
    inhibit_hit_index_ = 0xFFFF;
    inhibit_hit_mask_ = 0;
  }
  bool MauSram::hit_inhibited(const int index, const int hit) {
    return ((index == static_cast<int>(inhibit_index())) &&
            ((inhibit_mask() & (1<<hit)) != 0));
  }

  int MauSram::lookup(const int index, BitVector<kSramWidth> &inval,
                      uint8_t *match_masks) {
    RMT_ASSERT(is_match_sram());
    int hitindex = -1;
    BitVector<kSramWidth> sramval, maskval;
    if (match_masks != NULL) *match_masks = 0;
    if (get(index, &sramval)) {
      for (int i = 0; i < kMaskEntries; i++) {
        if (((match_enables() & (1<<i)) != 0) &&
            (!hit_inhibited(index, i)) &&
            (get_mask(i, &maskval))) {

          bool hit = false;
          if (inval.masked_s0q1_s1q0_equals(sramval, maskval,
                                            s0q1_enable_,s1q0_enable_)) {
            hit = true;
            hitindex = index;
            hit_mask_ |= 1<<i; // Stash for probe
            if (match_masks != NULL) *match_masks |= 1<<i;
	    // don't break, need to check all so higher level code can
	    //  complain if there is more than one match
            //break;
          }
          if (rmt_log_check(RmtDebug::verbose())) {
            const char *hitmiss = (hit) ?"HIT" :"MISS";
            rmt_log_va(RmtDebug::verbose(),
                    "MauSram::lookup(index=%d,%s=%d) "
                    " INP= %016" PRIx64 " %016" PRIx64 " ...\n",
                    index, hitmiss, i,
                    inval.get_word(64), inval.get_word(0));
            rmt_log_va(RmtDebug::verbose(),
                    "MauSram::lookup(index=%d,%s=%d) "
                    "SRAM= %016" PRIx64 " %016" PRIx64 " ...\n",
                    index, hitmiss, i,
                    sramval.get_word(64), sramval.get_word(0));
            rmt_log_va(RmtDebug::verbose(),
                    "MauSram::lookup(index=%d,%s=%d) "
                    "MASK= %016" PRIx64 " %016" PRIx64 " ...\n",
                    index, hitmiss, i,
                    maskval.get_word(64), maskval.get_word(0));
            rmt_log_va(RmtDebug::verbose(),
                    "MauSram::lookup(index=%d,%s=%d) "
                    "S0Q1= %016" PRIx64 " %016" PRIx64 " ...\n",
                    index, hitmiss, i,
                    s0q1_enable_.get_word(64), s0q1_enable_.get_word(0));
            rmt_log_va(RmtDebug::verbose(),
                    "MauSram::lookup(index=%d,%s=%d) "
                    "S1Q0= %016" PRIx64 " %016" PRIx64 "\n",
                    index, hitmiss, i,
                    s1q0_enable_.get_word(64), s1q0_enable_.get_word(0));

          }
        }
      }
    }
    return hitindex;
  }

  int MauSram::lookup(Phv *phv, uint8_t *match_masks) {
    int hitindex = -1;
    int search_bus = mau_sram_reg_.get_search_bus();
    BitVector<kSramWidth> match_data;
    MauSramRow *rowobj = row();
    RMT_ASSERT (rowobj != NULL);
    if (match_masks != NULL) *match_masks = 0;
    hitindex = rowobj->get_hash_index(phv, col_index());
    if (hitindex >= 0) {
      rowobj->get_match_data(phv, search_bus, &match_data);
      hitindex = lookup(hitindex, match_data, match_masks);
    }
    return hitindex;
  }


  bool MauSram::lt_uses_xm_bus(int lt, int bus) {
    RMT_ASSERT((bus == 0) || (bus == 1));
    int which_bus = (row_index_ << 1) | (bus & 1);
    uint16_t bus_mask = static_cast<uint16_t>(1 << which_bus);
    return ((result_bus_->get_match_buses(lt) & bus_mask) != 0);
  }
  void MauSram::set_match_output(const int lt, const int index, const int which_entry) {
    BitVector<kSramWidth> sramVal;
    BitVector<kMatchOutputBusWidth> busVal;
    MauSramRow *rowobj = row();
    RMT_ASSERT (rowobj != NULL);
    if (get(index, &sramVal)) {
      bool output_to_bus = false;
      uint32_t matchAddr = make_match_address(index, which_entry);
      busVal.set_word(sramVal.get_word(0,64),0,64);
      busVal.set32(2, matchAddr);
      if (use_match_result_bus(0) && lt_uses_xm_bus(lt,0)) {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramMatchOutput),
                "MauSram::set_match_output(lt=%d,index=%d,entry=%d,bus=0) "
                "vpn=0x%02x matchAddr=0x%08x matchBus=0x%016" PRIx64 "\n",
                lt, index, which_entry, get_vpn(which_entry),
                matchAddr, busVal.get_word(0,64));
        rowobj->set_match_output_bus(0, busVal, col_index());
        output_to_bus = true;
      }
      if (use_match_result_bus(1) && lt_uses_xm_bus(lt,1)) {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramMatchOutput),
                "MauSram::set_match_output(lt=%d,index=%d,entry=%d,bus=1) "
                "vpn=0x%02x matchAddr=0x%08x matchBus=0x%016" PRIx64 "\n",
                lt, index, which_entry, get_vpn(which_entry),
                matchAddr, busVal.get_word(0,64));
        rowobj->set_match_output_bus(1, busVal, col_index());
        output_to_bus = true;
      }
      if (!output_to_bus) {
        RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugMauSramMatchOutput),
                "MauSram::set_match_output: NO output bus configured (LT=%d)\n",lt);
      }
    }
  }

  bool MauSram::lt_uses_tm_bus(int lt, int bus) {
    RMT_ASSERT((bus == 0) || (bus == 1));
    int which_bus = (row_index_ << 1) | (bus & 1);
    uint16_t bus_mask = static_cast<uint16_t>(1 << which_bus);
    return ((result_bus_->get_tind_buses(lt) & bus_mask) != 0);
  }
  void MauSram::set_tind_output(const int lt, const int index, const int which_word,
                                uint8_t *which_bus, uint8_t *next_tab) {
    RMT_ASSERT( which_word == 0 || which_word == 1 );
    BitVector<kSramWidth> sramVal;
    BitVector<kTindOutputBusWidth> busVal;
    MauSramRow *rowobj = row();
    RMT_ASSERT (rowobj != NULL);
    int bus = get_tind_result_bus();

    if (get(index, &sramVal)) {
      bool output_to_bus = false;
      busVal.set_word(sramVal.get_word(which_word*64,64),0,64);
      if (use_tind_result_bus(0) && lt_uses_tm_bus(lt,0)) {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramTindOutput),
                "MauSram::set_tind_output(lt=%d,index=%d,word=%d,bus=0) "
                "tindBus=0x%016" PRIx64 "\n",
                lt, index, which_word, busVal.get_word(0,64));
        rowobj->set_tind_output_bus(0, busVal, col_index());
        output_to_bus = true;
      }
      if (use_tind_result_bus(1) && lt_uses_tm_bus(lt,1)) {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramTindOutput),
                "MauSram::set_tind_output(lt=%d,index=%d,word=%d,bus=1) "
                "tindBus=0x%016" PRIx64 "\n",
                lt, index, which_word, busVal.get_word(0,64));
        rowobj->set_tind_output_bus(1, busVal, col_index());
        output_to_bus = true;
      }
      if (!output_to_bus) {
        RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugMauSramTindOutput),
                "MauSram::set_tind_output: NO output bus configured (LT=%d)\n",lt);
      }
    }
    if (which_bus != NULL) *which_bus  = static_cast<uint8_t>(bus);
    if (next_tab != NULL)  *next_tab = busVal.get_byte(0);
  }



  void MauSram::claim_addrs() {
    switch (get_ram_type()) {
      case MauDefs::kSramTypeStats:    claim_addrs_stats_sram();    break;
      case MauDefs::kSramTypeMeter:    claim_addrs_meter_sram();    break;
      case MauDefs::kSramTypeSelector: claim_addrs_selector_sram(); break;
      case MauDefs::kSramTypeStateful: claim_addrs_stateful_sram(); break;
    }
  }
  void MauSram::run_selector_read() {
    if (is_selector_sram()) run_read_selector_sram();
  }
  // These days action_read is deferred till *after* ALUs run
  // This allows SALU to mux Stateful Min/Max info into action address
  void MauSram::run_action_read() {
    if (is_action_sram()) run_read_action_sram();
  }
  void MauSram::run_read() {
    // NB run_read_selector_sram runs earlier
    switch (get_ram_type()) {
      //case MauDefs::kSramTypeAction:   run_read_action_sram();    break;
      case MauDefs::kSramTypeStats:    run_read_stats_sram();     break;
      case MauDefs::kSramTypeMeter:    run_read_meter_sram();     break;
      case MauDefs::kSramTypeStateful: run_read_stateful_sram();  break;
    }
  }
  void MauSram::run_write() {
    switch (get_ram_type()) {
      case MauDefs::kSramTypeStats:    run_write_stats_sram();    break;
      case MauDefs::kSramTypeMeter:    run_write_meter_sram();    break;
      case MauDefs::kSramTypeSelector: run_write_selector_sram(); break;
      case MauDefs::kSramTypeStateful: run_write_stateful_sram(); break;
    }
  }




  bool MauSram::run_read_action_sram() {
    // Get action address from logical row
    uint32_t actionAddr = action_addr();
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramActionRead),
            "MauSram::run_read_action_sram(addr=0x%08x)\n",actionAddr);
    if (!Address::action_addr_enabled(actionAddr)) return false;

    int shift = Address::action_addr_get_shift(actionAddr);
    int vpn = Address::action_addr_get_vpn(actionAddr);
    int vpn_cmp_mask = Address::action_addr_get_vpn_cmpmask(shift);

    // See if VPNs match - we don't compare tableIndex for
    // actionSRAMs as they may be shared across logical tables
    if ((vpn & vpn_cmp_mask) != (get_vpn(0) & vpn_cmp_mask)) return false;

    int index = Address::action_addr_get_index(actionAddr);
    int subword = Address::action_addr_get_subword(actionAddr, shift);
    uint8_t action_nbits = 4<<std::min(5,shift); // 8,16,32,64,128
    uint8_t action_offset = subword * action_nbits;
    // Sanity check - ignore any selector address muxed in - just base addr
    mau()->mau_addr_dist()->action_addr_consume(action_addr_base());
    // Keep tally
    mau()->mau_info_incr(MAU_ACTION_SRAMS_READ);

    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramActionRead),
            "MauSram::run_read_action_sram(vpn=%d,"
            "addr=0x%08x,shift=%d,sub=%d,nbits=%d,off=%d)\n",
            vpn,actionAddr,shift,subword,action_nbits,action_offset);

    BitVector<kSramWidth> sramVal;
    bool found = get(index, &sramVal);
    RMT_ASSERT(found);

    // Drive action data back onto logical row action data bus
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramActionRead),
            "MauSram::run_read_action_sram(index=%d) = "
            "0x%016" PRIx64 " 0x%016" PRIx64 "\n",
            index, sramVal.get_word(0), sramVal.get_word(64));
    output_rd_data(&sramVal);

    //if (output_action_subword()) {
    //  One ActionSRAM should output action subword so
    //  sram row code can subword select from it correctly
    //  NB. output_action_subword() now *always* true
    //  logrow_->set_action_rd_addr(&actionAddr);
    //}
    return true;
  }


  // Multiple R/W rams might possibly be able to contain the
  // distributed stats/meter/selector address - the sram that
  // currently possesses the address should claim it at a high
  // pri (and there should be only one). If the ram does NOT
  // contain the address but COULD accommodate it, it can claim
  // the address at a low pri - many rams might do this - one
  // will be selected deterministically.
  //
  int8_t MauSram::claim_pri(uint8_t addrtype, int vpn, uint16_t index) {
    // If we have addr (VPN match) claim at hi pri (1)
    // If we could hold addr (VPN unoccupied) claim at lo pri (0)
    // Otherwise return -128 which is guaranteed to never win
    int8_t retpri = -128;
    int mapram_vpn = mapram_->get_synth2port_vpn(index);
    if (mapram_vpn == vpn)                       retpri = 1;
    else if (mapram_vpn == kMapramVpnUnoccupied) retpri = 0;
    RMT_LOG(RmtDebug::verbose(),
            "Claim: mapram_vpn=%d addr_vpn=%d\n", mapram_vpn, vpn);
    return retpri;
  }

  bool MauSram::stats_claim(int pri) {
    return mau()->mau_addr_dist()->stats_alu_claim( get_alu_index(), logrow_index_, logcol_index_, pri );
  }

  bool MauSram::meter_claim(int pri) {
    return mau()->mau_addr_dist()->meter_alu_claim( get_alu_index(), logrow_index_, logcol_index_, pri );
  }

  void MauSram::claim_addrs_stats_sram() {
    if (mapram() == NULL) return;
    // Get stats address, VPN, index then see if we should claim addr
    uint32_t addr = stats_addr();
    if (!Address::stats_addr_op_enabled(addr)) return;
    uint16_t stats_index = Address::stats_addr_get_index(addr);
    int stats_vpn = Address::stats_addr_get_vpn(addr);
    int8_t pri = claim_pri(AddrType::kStats, stats_vpn, stats_index);
    // Always claim if we have an addr - Synth2Port code relies on
    // this to determine *entire* range of SRAM rows using ALU
    (void)stats_claim(pri);
  }
  bool MauSram::run_read_stats_sram() {
    uint32_t addr = stats_addr();
    if (!Address::stats_addr_op_enabled(addr)) return false;
    // Check whether claim succeeded but only if we have an addr
    if (!stats_claim(-128)) return false;

    uint16_t stats_index = Address::stats_addr_get_index(addr);
    int stats_vpn = Address::stats_addr_get_vpn(addr);
    int mapram_vpn = mapram_->get_synth2port_vpn(stats_index);
    // We should either already have addr or be 'empty' (ie vpn 0x3F)
    RMT_ASSERT((mapram_vpn == stats_vpn) || (mapram_vpn == kMapramVpnUnoccupied));
    logrow_->set_stats_logical_table(get_logical_table()); // For full-res-stats
    mau()->mau_addr_dist()->stats_addr_consume(addr); // Sanity check
    if (!Address::stats_addr_op_cfg(addr))
      mau()->mau_info_incr(MAU_STATS_SRAMS_READ); // Keep tally
    if (mapram_vpn == kMapramVpnUnoccupied) return false; // Bail if empty

    // If we have successfully claimed addr
    // then push corresponding entry onto bus
    BitVector<kSramWidth> sramVal(UINT64_C(0));
    bool found = get(stats_index, &sramVal);
    RMT_ASSERT(found);

    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramStatsRead),
            "MauSram::run_read_stats_sram(addr=0x%08x index=%d) = "
            "0x%016" PRIx64 " 0x%016" PRIx64 "\n",
            addr, stats_index, sramVal.get_word(0), sramVal.get_word(64));

    // Drive stats data back onto configured logical row data bus
    output_rd_data(&sramVal);
    return true;
  }

  bool MauSram::run_write_stats_sram() {
    // TODO: 2port: Currently don't implement synthetic 2-port SRAMs.
    // Whenever an SRAM is selected to output data onto a RD DATA BUS
    // **EXACTLY THE SAME** SRAM consumes the response data from the WR
    // DATA BUS

    uint32_t addr = stats_waddr();
    if (!Address::stats_addr_op_enabled(addr)) return false;
    // Check whether claim succeeded but only if we have an addr
    if (!stats_claim(-128)) return false;

    int stats_vpn = Address::stats_addr_get_vpn(addr);
    uint16_t stats_index = Address::stats_addr_get_index(addr);
    int mapram_vpn = mapram_->get_synth2port_vpn(stats_index);
    RMT_ASSERT((mapram_vpn == stats_vpn) || (mapram_vpn == kMapramVpnUnoccupied));
    if (!Address::stats_addr_op_cfg(addr))
      mau()->mau_info_incr(MAU_STATS_SRAMS_WRITTEN); // Keep tally

    // If index was previously empty update vpn into MAP ram
    if (mapram_vpn == kMapramVpnUnoccupied)
      mapram_->set_synth2port_vpn(stats_index, stats_vpn);

    // Read data off logical row stats WRITE data bus
    BitVector<kDataBusWidth> busVal;
    if (!input_wr_data(&busVal)) return false;

    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramStatsWrite),
            "MauSram::run_write_stats_sram(index=%d) = "
            "0x%016" PRIx64 " 0x%016" PRIx64 "\n",
            stats_index, busVal.get_word(0), busVal.get_word(64));

    // And write back to memory
    set(stats_index, busVal);
    return true;
  }



  void MauSram::claim_addrs_meter_sram() {
    if (mapram() == NULL) return;
    // Get meter address, VPN, index then see if we should claim addr
    uint32_t addr = meter_addr();
    if (!Address::meter_addr_op_enabled(addr)) return;
    uint16_t meter_index = Address::meter_addr_get_index(addr);
    int meter_vpn = Address::meter_addr_get_vpn(addr);
    int8_t pri = claim_pri(AddrType::kMeter, meter_vpn, meter_index);
    // Always claim if we have an addr - Synth2Port code relies on
    // this to determine *entire* range of SRAM rows using ALU
    (void)meter_claim(pri);
  }

  bool MauSram::run_read_meter_sram(const char *sramtype) {
    bool sramtype_meter = (*sramtype == 'm');
    uint32_t addr = meter_addr();
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramMeterRead),
            "MauSram::run_read_%s_sram(addr=0x%08x,subword=%d)\n",
            sramtype, addr, Address::meter_addr_get_subword(addr));

    if (!Address::meter_addr_op_enabled(addr)) return false;
    // Check whether claim succeeded but only if we have an addr
    if (!meter_claim(-128)) return false;

    uint16_t meter_index = Address::meter_addr_get_index(addr);
    int meter_vpn = Address::meter_addr_get_vpn(addr);
    int mapram_vpn = mapram_->get_synth2port_vpn(meter_index);
    // We should either already have addr or be 'empty' (ie vpn 0x3F)
    RMT_ASSERT((mapram_vpn == meter_vpn) || (mapram_vpn == kMapramVpnUnoccupied));
    mau()->mau_addr_dist()->meter_addr_consume(addr); // Sanity check
    if (sramtype_meter && !Address::meter_addr_op_cfg(addr))
      mau()->mau_info_incr(MAU_METER_SRAMS_READ); // Keep tally

    if (mapram_vpn == kMapramVpnUnoccupied) {
      bool stateful_logging = false;
      if (Address::meter_addr_is_stateful(addr))
        stateful_logging = mau()->stateful_counter_enabled(get_logical_table());

      // Selector/Stateful SRAMs probably shouldn't be empty - Meters definitely not
      if (!Address::meter_addr_op_sweep(addr) &&
          !Address::meter_addr_op_cfg(addr) &&
          !stateful_logging) {
        // But don't complain if Sweep/CfgRd/CfgWr or if Stateful *logging*
        RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugMauSramMeterRead, kRelaxSramEmptyCheck),
                "MauSram::run_read_%s_sram(addr=0x%08x index=%d) EMPTY/Uninitialized\n",
                sramtype, addr, meter_index);
      }
      return false; // Bail if empty
    }

    // If we have successfully claimed addr
    // then push corresponding entry onto bus
    BitVector<kSramWidth> sramVal(UINT64_C(0));
    bool found = get(meter_index, &sramVal);
    RMT_ASSERT(found);

    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramMeterRead),
            "MauSram::run_read_%s_sram(addr=0x%08x index=%d) = "
            "0x%016" PRIx64 " 0x%016" PRIx64 "\n",
            sramtype, addr, meter_index,
            sramVal.get_word(0), sramVal.get_word(64));

    // Drive meter data back onto configured logical row data bus
    output_rd_data(&sramVal);
    return !Address::meter_addr_op_cfg(addr);
  }

  bool MauSram::run_write_meter_sram(const char *sramtype) {
    bool sramtype_meter = (*sramtype == 'm');
    uint32_t addr = meter_waddr();
    if (!Address::meter_addr_op_enabled(addr)) return false;
    // Check whether claim succeeded but only if we have an addr
    if (!meter_claim(-128)) return false;

    int meter_vpn = Address::meter_addr_get_vpn(addr);
    uint16_t meter_index = Address::meter_addr_get_index(addr);
    int mapram_vpn = mapram_->get_synth2port_vpn(meter_index);
    RMT_ASSERT((mapram_vpn == meter_vpn) || (mapram_vpn == kMapramVpnUnoccupied));
    if (sramtype_meter && !Address::meter_addr_op_cfg(addr))
      mau()->mau_info_incr(MAU_METER_SRAMS_WRITTEN); // Keep tally

    // If index was previously empty update vpn into MAP ram
    // Note, Selector/Stateful SRAMs probably shouldn't be empty - Meters definitely not
    if (mapram_vpn == kMapramVpnUnoccupied)
      mapram_->set_synth2port_vpn(meter_index, meter_vpn);

    // Read data off logical row meter WRITE data bus
    // These days uses stats WRITE data bus instead
    BitVector<kDataBusWidth> wrBusVal;
    if (!input_wr_data(&wrBusVal)) return false;

    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramMeterWrite),
            "MauSram::run_write_%s_sram(index=%d) = "
            "0x%016" PRIx64 " 0x%016" PRIx64 "\n",
            sramtype, meter_index, wrBusVal.get_word(0), wrBusVal.get_word(64));

#ifdef DEBUG_SRAM_WRITE
    BitVector<kDataBusWidth> rdBusVal;
    input_rd_data(&rdBusVal);
    if ((*sramtype != 'm') && (!wrBusVal.equals(rdBusVal))) {
      int wr_mux = mau_sram_reg_.get_write_data_mux();
      int rd_mux = mau_sram_reg_.get_read_data_mux();
      printf("MauSram::run_write_%s_sram(index=%d) WR<%d>= "
             "0x%016" PRIx64 " 0x%016" PRIx64 " RD<%d>= "
             "0x%016" PRIx64 " 0x%016" PRIx64 " RD/WR BUS MISMATCH "
             "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",
             sramtype, meter_index, wr_mux,
             wrBusVal.get_word(0), wrBusVal.get_word(64), rd_mux,
             rdBusVal.get_word(0), rdBusVal.get_word(64));
    }
#endif

    // And write back to memory
    set(meter_index, wrBusVal);
    return !Address::meter_addr_op_cfg(addr);
  }


  void MauSram::claim_addrs_selector_sram() {
    claim_addrs_meter_sram(); // Call meter version
  }
  bool MauSram::run_read_selector_sram() {
    bool ret = run_read_meter_sram("selector"); // Call meter version
    if (ret) mau()->mau_info_incr(MAU_SELECTOR_SRAMS_READ);
    return ret;
  }
  bool MauSram::run_write_selector_sram() {
    bool ret = run_write_meter_sram("selector"); // Call meter version
    if (ret) mau()->mau_info_incr(MAU_SELECTOR_SRAMS_WRITTEN);
    return ret;
  }

  void MauSram::claim_addrs_stateful_sram() {
    claim_addrs_meter_sram(); // Call meter version
  }
  bool MauSram::run_read_stateful_sram() {
    bool ret = run_read_meter_sram("stateful"); // Call meter version
    if (ret) mau()->mau_info_incr(MAU_STATEFUL_SRAMS_READ);
    return ret;
  }
  bool MauSram::run_write_stateful_sram() {
    bool ret = run_write_meter_sram("stateful"); // Call meter version
    if (ret) mau()->mau_info_incr(MAU_STATEFUL_SRAMS_WRITTEN);
    return ret;
  }


  void MauSram::match_nibble_s1q0bar_enable_update(uint32_t new_value) {
    match_nibble_sXqXbar_enable_update(new_value, &s1q0_enable_);
  }
  void MauSram::match_nibble_s0q1bar_enable_update(uint32_t new_value) {
    match_nibble_sXqXbar_enable_update(new_value, &s0q1_enable_);
  }
  void MauSram::match_nibble_sXqXbar_enable_update(uint32_t new_value,
                                                 BitVector<MauDefs::kSramWidth>* bv) {
    static_assert( MauDefs::kSramWidth == 128,
                   "match_nibble_sXqXbar_enable_update assumes 128bits");
    bv->fill_all_zeros();
    for (int i=0;i<32;++i) {
      if ( (new_value >> i) & 1 ) {
        for (int b=0; b<4;++b) {
          bv->set_bit( (i*4) + b );
        }
      }
    }
  }

  void MauSram::reset_resources() {
    hit_mask_ = 0;
    mau_sram_reg_.reset_resources();
  }


  // Next set of funcs to handle atomic SRAM update writereg
  // Allows atomic set across >1 SRAM

  bool MauSram::pending_index(int *index) {
    if (index != NULL) *index = static_cast<int>(pending_index_);
    return ((pending_state_ == kPendingStateReady) || (pending_state_ == kPendingStateLocked));
  }
  bool MauSram::pending_get(const int index, uint64_t *data0, uint64_t *data1, uint64_t T) {
    if (index < 0) return false;
    bool got_pending = false;
    spinlock();
    got_pending = ((pending_state_ != kPendingStateReset) && (pending_index_ == index));
    if (got_pending) {
      if (data0 != NULL) *data0 = pending_data0_;
      if (data1 != NULL) *data1 = pending_data1_;
    }
    spinunlock();
    return got_pending;
  }
  void MauSram::pending_set(const int index, uint64_t data0, uint64_t data1, uint64_t T) {
    if (index < 0) return;
    if (is_match_sram() || is_tind_sram() || is_action_sram()) {
      // All good - this is what we expect
    } else {
      // Error - can't do pending writes if not Match/Tind/Action SRAM
      const char *sramtypes[8] = { "Unused", "Match", "Action", "Stats",
                                  "Meter", "Stateful", "Tind", "Selector" };
      RMT_LOG(RmtDebug::error(),
              "MauSram::pending_set(index=%d) %s SRAM type invalid for pending write\n",
              index, sramtypes[get_ram_type() & 7]);
      return;
    }
    if ((pending_state_ == kPendingStateLocked) || (pending_state_ == kPendingStateWritten)) {
      // Error - can't allow pending writes if in process of committing
      const char *states[8] = { "Reset", "Ready", "Locked", "Written", "Done", "!", "!", "!" };
      RMT_LOG(RmtDebug::error(),
              "MauSram::pending_set(index=%d) New pending write ignored in state %s\n",
              index, states[pending_state_ & 7]);
      return;
    }
    spinlock();
    pending_state_ = kPendingStateReady;
    pending_index_ = index;
    pending_data0_ = data0;
    pending_data1_ = data1;
    spinunlock();
  }
  bool MauSram::pending_lock() {
    bool locked = false;
    spinlock();
    if (pending_state_ == kPendingStateReady) {
      sram_.lock(pending_index_);
      locked = true;
      pending_state_ = kPendingStateLocked;
    }
    spinunlock();
    return locked;
  }
  void MauSram::pending_flush() {
    spinlock();
    if (pending_state_ == kPendingStateLocked) {
      BitVector<kSramWidth> val;
      val.set_word(pending_data0_, 0);
      val.set_word(pending_data1_, 64);
      sram_.set_nolock(pending_index_, val);
      pending_state_ = kPendingStateWritten;
    }
    spinunlock();
  }
  void MauSram::pending_unlock() {
    spinlock();
    if (pending_state_ == kPendingStateWritten) {
      sram_.unlock(pending_index_);
      pending_state_ = kPendingStateDone;
    }
    spinunlock();
  }
  void MauSram::pending_unset() {
    spinlock();
    pending_state_ = kPendingStateReset;
    pending_index_ = 0;
    spinunlock();
  }


}
