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
#include <mau-sram-row.h>


namespace MODEL_CHIP_NAMESPACE {

  MauSramRow::MauSramRow(RmtObjectManager *om,
                         int pipeIndex, int mauIndex, int rowIndex,
                         Mau *mau, MauInput *mau_input)
      : MauObject(om, pipeIndex, mauIndex, kType, rowIndex, mau),
        row_index_(rowIndex), srams_(),
        hash_address_(chip_index(),pipeIndex,mauIndex,rowIndex,mau_input->get_hash_generator()),
        action_output_hv_xbar_(chip_index(),pipeIndex,mauIndex,rowIndex),
        match_xbars_{ {
            { chip_index(),pipeIndex,mauIndex,rowIndex,0,mau_input->get_hash_generator() },
            { chip_index(),pipeIndex,mauIndex,rowIndex,1,mau_input->get_hash_generator() } } },
        gateway_tables_{ {
            { om,pipeIndex,mauIndex,rowIndex,0,mau,this },
            { om,pipeIndex,mauIndex,rowIndex,1,mau,this } } },
        gateway_payloads_{ {
            { om,pipeIndex,mauIndex,rowIndex,0,mau,this },
            { om,pipeIndex,mauIndex,rowIndex,1,mau,this } } },
        stash_( om,pipeIndex,mauIndex,rowIndex,mau,this ),
        row_above_(NULL), row_below_(NULL), logrow_left_(NULL), logrow_right_(NULL),
        mau_sram_row_reg_(om, pipeIndex, mauIndex, rowIndex, this),
        mau_color_switchbox_(om, pipeIndex, mauIndex, rowIndex, mau, this)  {

    static_assert(kSearchBuses == 2, "MauSramRow can only handle 2 search buses");
    static_assert(kGatewayTablesPerRow == 2, "MauSramRow can only handle 2 gateway tables");
    static_assert(kGatewayPayloadsPerRow == 2, "MauSramRow can only handle 2 gateway payloads");

    reset_resources();
    RMT_LOG_VERBOSE("MAU_SRAM_ROW::create\n");
  }
  MauSramRow::~MauSramRow() {
    RMT_LOG_VERBOSE("MAU_SRAM_ROW::delete\n");
    for (int i = 0; i < kSramColumns; i++) srams_[i] = NULL;
    row_above_ = NULL; row_below_ = NULL;
    logrow_left_ = NULL; logrow_right_ = NULL;
  }


  // Exact match priority for each SRAM column
  // Cols in prio order are 0,1,2,3,4,5,11,10,9,8,7,6
  // (start at prio 24 to match up with MauSram::prio)
  //
  // set_match_output_bus should be called for any given
  // PHV in descending col prio order (handled by logic
  // in MauLogicalTable::lookup_exact_match)
  //
  // 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11
  const int MauSramRow::kPriorityPerColumnTab[MauSramRow::kSramColumns] = {
    24, 23, 22, 21, 20, 19,  6,  7,  8,  9, 10, 11
  };
  // Priority group for each SRAM column
  // Columns 7 and 6 handled specially - hence group 99 - see comment
  // in set_match_output_bus
  //
  // 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11
  const int MauSramRow::kPriGroupPerColumnTab[MauSramRow::kSramColumns] = {
     2,  2,  2,  2,  2,  2, 99, 99,  1,  1,  1,  1
  };




  void MauSramRow::get_input(Phv *phv,
                             BitVector<kExactMatchInputBits> *input_bits,
                             BitVector<kExactMatchValidBits> *valid_bits) {
    Mau *mauobj = mau();
    RMT_ASSERT (mauobj != NULL);
    mauobj->get_exact_match_input(phv, input_bits, valid_bits);
  }

  int MauSramRow::get_hash_index(Phv *phv, int column) {
    int                          index = -1;
    bool                         addr_valid = false;
    MauHashGenerator::InputT     input_bits;
    MauHashGenerator::ValidT     valid_bits;
    BitVector<kSramAddressWidth> addr;

    get_input(phv, &input_bits, &valid_bits);
    hash_address_.CalculateHashAddress(column,
                                       input_bits, valid_bits,
                                       phv->cache_id(),
                                       &addr_valid,
                                       &addr);
    if (addr_valid) index = static_cast<int>(addr.get_word(0, kSramAddressWidth));
    return index;
  }

  void MauSramRow::get_match_data(Phv *phv, int search_bus,
                                  BitVector<kSramWidth> *match_data) {
    RMT_ASSERT((search_bus >= 0) && (search_bus < kSearchBuses));
    MatchDataInputVhXbarWithReg *match_xbar = &match_xbars_[search_bus];
    BitVector<kExactMatchInputBits>  input_bits;
    BitVector<kExactMatchValidBits>  valid_bits;
    BitVector<MatchDataInputVhXbarWithReg::kVersionDataWidth> ingress_version_bits;
    BitVector<MatchDataInputVhXbarWithReg::kVersionDataWidth> egress_version_bits;

    get_input(phv, &input_bits, &valid_bits);

    ingress_version_bits.set_byte(expand_valid_bits(phv->ingress_version()), 0);
    egress_version_bits.set_byte(expand_valid_bits(phv->egress_version()), 0);

    match_xbar->CalculateSearchData(input_bits, valid_bits,
                                    ingress_version_bits, egress_version_bits,
                                    phv->cache_id(), match_data);
  }

  void MauSramRow::get_hash(Phv *phv, int which,
                            BitVector<kHashOutputWidth> *hash) {
    MauHashGenerator::InputT     input_bits;
    MauHashGenerator::ValidT     valid_bits;
    get_input(phv, &input_bits, &valid_bits);
    hash_address_.CalculateHash(input_bits, valid_bits,
                                which,
                                phv->cache_id(),
                                hash);
  }

  void MauSramRow::reset_resources() {
    mau_color_switchbox_.reset_resources();
    for (int i = 0; i < kMatchOutputBuses; i++) {
      match_output_bus_in_use_[i] = false;
      match_output_bus_col_[i] = 0;
    }
    for (int j = 0; j < kTindOutputBuses; j++) {
      tind_output_bus_in_use_[j] = false;
      tind_output_bus_col_[j] = 0;
      tcam_match_addrs_[j] = 0u;
    }
  }

  void MauSramRow::set_match_output_bus(int which_bus,
                                       const BitVector<kMatchOutputBusWidth>& output,
                                       int col) {
    RMT_ASSERT((which_bus >= 0) && (which_bus < kMatchOutputBuses));
    int prio = col_prio(col);
    int prio_grp = col_prio_group(col);
    RMT_ASSERT((prio > 0) && (prio_grp > 0));

    // Still squawk if bus reused - but hi prio beats lo prio
    if (match_output_bus_in_use_[which_bus]) {
      int prev_col = match_output_bus_col_[which_bus];
      int prev_prio = col_prio(prev_col);
      int prev_prio_grp = col_prio_group(prev_col);
      RMT_ASSERT((prev_col > 0) && (prev_prio > 0) && (prev_prio_grp > 0));

      if (prio_grp < prev_prio_grp) {
        //RMT_ASSERT(prio_grp <= 2);

        // If multiple cols with diff prio_grp values push results
        // then they are ORed together - we track latest col
        //
        // NB Cols 7 and 6 in prio_grp 99 are handled specially
        // and are never ORed with earlier result
        // Col 7/6 results only become visible if very first hit on row
        match_output_bus_col_[which_bus] = col;
        match_output_buses_[which_bus].or_with(output);
        RMT_LOG(RmtDebug::error(kRelaxMatchBusMultiWriteCheck),
                "MauSramRow::set_match_output_bus(R=%d,bus=%d) WRITTENx2! "
                "prev_col=%d new_col=%d bus=%s [NB 888=>STASH 999=>GW]\n",
                row_index(), which_bus, prev_col, col,
                match_output_buses_[which_bus].to_string().c_str());
        if (!kRelaxMatchBusMultiWriteCheck) { THROW_ERROR(-2); }

      } else if (prio > prev_prio) {
        // This should NOT happen - column lookup ordering should
        // ensure that higher pri cols occur first
        match_output_bus_col_[which_bus] = col;
        match_output_buses_[which_bus].set_from(0, output);
        RMT_LOG(RmtDebug::error(kRelaxMatchBusMultiWriteCheck),
                "MauSramRow::set_match_output_bus(R=%d,bus=%d): OVERWRITTEN! "
                "prev_col=%d new_col=%d bus=%s [NB 888=>STASH 999=>GW]\n",
                row_index(), which_bus, prev_col, col,
                match_output_buses_[which_bus].to_string().c_str());
        if (!kRelaxMatchBusMultiWriteCheck) { THROW_ERROR(-2); }

      } else {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramRowSetMatchBus),
                "MauSramRow::set_match_output_bus(R=%d,bus=%d) IGNORED "
                "prev_col=%d new_col=%d\n", row_index(), which_bus, prev_col, col);
      }
    } else {
      // Very first hit for row
      RMT_ASSERT(match_output_bus_col_[which_bus] == 0);
      match_output_bus_in_use_[which_bus] = true;
      match_output_bus_col_[which_bus] = col;
      match_output_buses_[which_bus].set_from(0, output);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramRowSetMatchBus),
              "MauSramRow::set_match_output_bus(R=%d,bus=%d) WRITTEN Col=%d MatchBus=%s\n",
              row_index(), which_bus, col,
              match_output_buses_[which_bus].to_string().c_str());
    }
  }
  bool MauSramRow::get_match_output_bus(int which_bus,
                                        BitVector<kMatchOutputBusWidth> *output) {
    RMT_ASSERT((which_bus >= 0) && (which_bus < kMatchOutputBuses) && (output != NULL));
    if (match_output_bus_in_use_[which_bus]) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramRowGetMatchBus),
              "MauSramRow::get_match_output_bus(R=%d,bus=%d) matchBus=%s\n",
              row_index(), which_bus,
              match_output_buses_[which_bus].to_string().c_str());
      output->set_from(0, match_output_buses_[which_bus]);
      return true;
    } else {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramRowGetMatchBus),
              "MauSramRow::get_match_output_bus(R=%d,bus=%d) Match Output Bus NOT DRIVEN!\n",
              row_index(), which_bus);
      output->fill_all_zeros();
      return false;
    }
  }
  uint64_t MauSramRow::get_match_output_data(int which_bus) {
    RMT_ASSERT((which_bus >= 0) && (which_bus < kMatchOutputBuses));
    uint64_t data;
    if (match_output_bus_in_use_[which_bus]) {
      data = match_output_buses_[which_bus].get_word(0);
    } else {
      RMT_LOG_VERBOSE("Match Output Bus NOT driven");
      data = UINT64_C(0);
    }
    return data;
  }

  BitVector<MauSramRow::kStatefulMeterAluDataBits> MauSramRow::get_meter_stateful_selector_alu_data(Phv *phv, int which_bus) {
     RMT_ASSERT((which_bus >= 0) && (which_bus < 2));

     BitVector<kExactMatchInputBits>  input_bits;
     BitVector<kExactMatchValidBits>  valid_bits;
     BitVector<kHashOutputWidth> hash;

     get_input(phv, &input_bits, &valid_bits);

     // get the part from the exact match hash address vh xbar
     hash_address_.CalculateHash(input_bits, valid_bits,
                                which_bus + 2,
                                phv->cache_id(),
                                &hash);
     BitVector<MauDefs::kStatefulMeterAluDataBits> data{};
     data.set_word( hash.get_word(0), 0 );

     // OR in the part from the Exact match row vh xbar
     data.or_with( match_xbars_[which_bus].CalculateStatefulMeterAluData( input_bits ));
     return data;
   }


  void MauSramRow::get_gateway_table_result(Phv *phv, int which_table,
                                            bool* hit, int *hit_index) {
    RMT_ASSERT( which_table < kGatewayTablesPerRow );
    return gateway_tables_[which_table].lookup(phv,hit,hit_index);
  }


  void MauSramRow::set_tind_output_bus(int which_bus,
                                       const BitVector<kTindOutputBusWidth>& output,
                                       int col) {
    RMT_ASSERT((which_bus >= 0) && (which_bus < kTindOutputBuses));
    int prio = col_prio(col);

    // Still squawk if bus reused - but hi prio beats lo prio
    if (tind_output_bus_in_use_[which_bus]) {
      int prev_col = tind_output_bus_col_[which_bus];
      int prev_prio = col_prio(prev_col);
      RMT_ASSERT((prev_col > 0) && (prev_prio > 0));

      if (prio > prev_prio) {
        // This should NOT happen - srams are maintained in tind_srams_used bitmap
        // in colmajor order so higher-pri cols should appear before lower-pri cols
        // (ie colN before colN+1)
        tind_output_bus_col_[which_bus] = col;
        tind_output_buses_[which_bus].set_from(0, output);
        RMT_LOG(RmtDebug::error(kRelaxTindBusMultiWriteCheck),
                "MauSramRow::set_tind_output_bus(R=%d,bus=%d): OVERWRITTEN! "
                "prev_col=%d new_col=%d [999=>GW]\n",
                row_index(), which_bus, prev_col, col);
        if (!kRelaxTindBusMultiWriteCheck) { THROW_ERROR(-2); }

      } else {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramRowSetTindBus),
                "MauSramRow::set_tind_output_bus(R=%d,bus=%d) IGNORED "
                "prev_col=%d new_col=%d\n", row_index(), which_bus, prev_col, col);
      }
    } else {
      RMT_ASSERT(tind_output_bus_col_[which_bus] == 0);
      tind_output_bus_in_use_[which_bus] = true;
      tind_output_bus_col_[which_bus] = col;
      tind_output_buses_[which_bus].set_from(0, output);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramRowSetTindBus),
              "MauSramRow::set_tind_output_bus(R=%d,bus=%d) WRITTEN Col=%d TindBus=%s)\n",
              row_index(), which_bus, col,
              tind_output_buses_[which_bus].to_string().c_str());
    }
  }
  bool MauSramRow::get_tind_output_bus(int which_bus,
                                       BitVector<kTindOutputBusWidth> *output) {
    RMT_ASSERT((which_bus >= 0) && (which_bus < kTindOutputBuses) && (output != NULL));
    if (tind_output_bus_in_use_[which_bus]) {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramRowSetTindBus),
            "MauSramRow::get_tind_output_bus(R=%d,bus=%d) tindBus=%s\n",
            row_index(), which_bus,
            tind_output_buses_[which_bus].to_string().c_str());
      output->set_from(0, tind_output_buses_[which_bus]);
      return true;
    } else {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramRowGetTindBus),
              "MauSramRow::get_tind_output_bus(R=%d,bus=%d) Tind Output Bus NOT DRIVEN!\n",
              row_index(), which_bus);
      output->fill_all_zeros();
      return false;
    }
  }


  void MauSramRow::set_tcam_match_addr(int which_bus, uint32_t match_addr) {
    RMT_ASSERT((which_bus >= 0) && (which_bus < kTindOutputBuses));
    tcam_match_addrs_[which_bus] |= match_addr;
  }
  uint32_t MauSramRow::get_tcam_match_addr(int which_bus) {
    RMT_ASSERT((which_bus >= 0) && (which_bus < kTindOutputBuses));
    return tcam_match_addrs_[which_bus];
  }
  // fetch_addresses must be called between distributing and using an address
  //     so, it has to be called between distributing idletime and stats and doing the
  //     color ram read. And then it has to be called again after distributing the
  //     resulting meter address and before claim_addrs. It could be split up
  //     to only do certain addresses in each pass, but for now just do them all
  void MauSramRow::fetch_addresses() {
    RMT_ASSERT((logrow_left_ != NULL) && (logrow_right_ != NULL));
    logrow_left_->fetch_addresses();
    logrow_right_->fetch_addresses();
  }

  void MauSramRow::srams_claim_addrs() {
    RMT_ASSERT((logrow_left_ != NULL) && (logrow_right_ != NULL));
    for (int i = 0; i < kSramColumns; i++) {
      MauSram *sram = srams_[i];
      if (sram != NULL) sram->claim_addrs();
    }
  }
  void MauSramRow::srams_run_read(MauExecuteState *state) {
    for (int i = 0; i < kSramColumns; i++) {
      MauSram *sram = srams_[i];
      if (sram != NULL) {
        sram->run_read();
        MauMapram *mapram = sram->mapram();
        if (mapram != NULL) mapram->run_read(state);
      }
    }
  }
  void MauSramRow::srams_run_color_mapram_read(MauExecuteState *state) {
    for (int i = 0; i < kSramColumns; i++) {
      MauSram *sram = srams_[i];
      if (sram != NULL) {
        MauMapram *mapram = sram->mapram();
        if (mapram != NULL) mapram->run_read_color(state);
      }
    }
  }
  void MauSramRow::srams_run_selector_read(MauExecuteState *state) {
    for (int i = 0; i < kSramColumns; i++) {
      MauSram *sram = srams_[i];
      if (sram != NULL) sram->run_selector_read();
    }
  }
  void MauSramRow::srams_run_action_read(MauExecuteState *state) {
    for (int i = 0; i < kSramColumns; i++) {
      MauSram *sram = srams_[i];
      if (sram != NULL) sram->run_action_read();
    }
  }
  void MauSramRow::run_selector_alu_with_state(MauExecuteState *state) {
    RMT_ASSERT((logrow_left_ != NULL) && (logrow_right_ != NULL));
    //logrow_left_->run_selector_alu_with_state(state);
    logrow_right_->run_selector_alu_with_state(state);
  }
  void MauSramRow::run_alus_with_state(MauExecuteState *state) {
    RMT_ASSERT((logrow_left_ != NULL) && (logrow_right_ != NULL));
    //logrow_left_->run_alus_with_state(state);
    logrow_right_->run_alus_with_state(state);
  }
  void MauSramRow::run_cmp_alus_with_state(MauExecuteState *state) {
    RMT_ASSERT((logrow_left_ != NULL) && (logrow_right_ != NULL));
    //logrow_left_->run_cmp_alus_with_state(state);
    logrow_right_->run_cmp_alus_with_state(state);
  }
  void MauSramRow::srams_run_write(MauExecuteState *state) {
    for (int i = 0; i < kSramColumns; i++) {
      MauSram *sram = srams_[i];
      if (sram != NULL) {
        sram->run_write();
        MauMapram *mapram = sram->mapram();
        if (mapram != NULL) mapram->run_write(state);
      }
    }
  }
  void MauSramRow::drive_action_output_hv() {
    RMT_ASSERT(mau() != NULL);

    // Init Data/Addr to 0 - values ORed in
    BitVector<kDataBusWidth> lhs_action_data(UINT64_C(0));
    uint32_t lhs_action_addr = 0u;
    mau_sram_row_reg_.r_l_action(&lhs_action_data, &lhs_action_addr);
    lhs_action_addr = 0u; // Should only use addr from own logrow
    logrow_left_->action_sel_rd_addr(&lhs_action_addr);
    if (lhs_action_addr != 0u) { // Schtum if 0
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramRowSetHVBus),
              "MauSramRow::drive_act_out_hv(R=%d,LHS) addr=0x%08x d=%s\n",
              row_index(), lhs_action_addr, lhs_action_data.to_string().c_str());
    }
    set_output(lhs_action_data, lhs_action_addr, 0, mau()->action_hv_output_bus());

    // Init Data/Addr to 0 - values ORed in
    BitVector<kDataBusWidth> rhs_action_data(UINT64_C(0));
    uint32_t rhs_action_addr = 0u;
    mau_sram_row_reg_.r_action(&rhs_action_data, &rhs_action_addr);
    rhs_action_addr = 0u; // Should only use addr from own logrow
    logrow_right_->action_sel_rd_addr(&rhs_action_addr);

    if (rhs_action_addr != 0u) { // Schtum if 0
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramRowSetHVBus),
              "MauSramRow::drive_act_out_hv(R=%d,RHS) addr=0x%08x d=%s\n",
              row_index(), rhs_action_addr, rhs_action_data.to_string().c_str());
    }
    set_output(rhs_action_data, rhs_action_addr, 1, mau()->action_hv_output_bus());

  }


  void MauSramRow::set_output(const BitVector<kActionOutputBusWidth>& input_data,
                              uint32_t input_addr, int which_bus,
                              BitVector<kActionHVOutputBusWidth> *output) {
    RMT_ASSERT(((which_bus == 0) || (which_bus == 1))  &&  (output != NULL));

    int shift, subword;
    uint8_t nbits, offset;

    if ( 0 == mau_sram_row_reg_.action_hv_xbar_disable_ram_adr( which_bus ? true /*rhs*/ :false    ) ) {
      shift = Address::action_addr_get_shift(input_addr);
      subword = Address::action_addr_get_subword(input_addr, shift);
      nbits = static_cast<uint8_t>(4<<std::min(5,shift)); // 8,16,32,64,128
      offset = static_cast<uint8_t>(subword) * nbits;
    }
    else {
      // if action hv xbar disable ram adr is on then just use the word as is (lower bits must
      //   be zero to allow the meter output to or in.
      shift = subword = offset = 0;
      nbits = 128;
    }
    if ((input_addr != 0u) || !input_data.is_zero()) { // Schtum if 0
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSramRowSetHVBus),
              "MauSramRow::set_output(R=%d,bus=%d) inAddr=0x%08x,"
              "shift=%d,subword=%d,offset=%d,nbits=%d inData=%s\n",
              row_index(), which_bus, input_addr, shift, subword, offset, nbits,
              input_data.to_string().c_str());
      mau()->mau_info_incr(MAU_ACTION_HV_UPDATES);
    }
    if (nbits <= 64) {
      BitVector<kActionOutputBusWidth> copy_data(UINT64_C(0));
      // Loop below shifts input_data into copy_data starting from offset
      int off = 0;
      while (off + offset < kActionOutputBusWidth) {
        copy_data.set_word(input_data.get_word(off + offset), off);
        off += 64;
      }
      // Duplication now happens with CalculateOutput
      //if ((nbits == 8) || (nbits == 16) || (nbits == 32)) {
      // Duplicate byte/halfword/word into pos 1 as well as 0
      // copy_data.set_word(copy_data.get_word(0,nbits), nbits, nbits);
      //}
      action_output_hv_xbar_.CalculateOutput(copy_data, which_bus, nbits, output);
    } else {
      action_output_hv_xbar_.CalculateOutput(input_data, which_bus, nbits, output);
    }
  }

}
