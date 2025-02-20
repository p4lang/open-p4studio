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
#include <mau-selector-alu.h>
#include <mau-sbox.h>
#include <register_adapters.h>



namespace MODEL_CHIP_NAMESPACE {


MauSelectorAlu::MauSelectorAlu(RmtObjectManager  *om,
                               int                pipeIndex,
                               int                mauIndex,
                               int                log_row,
                               Mau               *mau,
                               MauLogicalRow     *logicalrow,
                               int                sram_row,
                               int                sram_which)
    : MauObject(om, pipeIndex, mauIndex, RmtTypes::kRmtTypeSelectorAlu, log_row, mau),
      logical_row_(logicalrow),
      mau_(mau),
      alu_index_(get_selector_alu_regs_index(log_row)),
      meter_alu_group_ctl_(default_adapter(meter_alu_group_ctl_,chip_index(), pipeIndex, mauIndex, alu_index_)),
      selector_alu_ctl_(default_adapter(selector_alu_ctl_,chip_index(), pipeIndex, mauIndex, alu_index_)),
      stateful_ctl_(default_adapter(stateful_ctl_,chip_index(), pipeIndex, mauIndex, alu_index_)),
      meter_alu_group_phv_hash_mask_(default_adapter(meter_alu_group_phv_hash_mask_,chip_index(), pipeIndex, mauIndex,
                                                     [this](int a1,int a0){this->phv_hash_mask_write_callback(a1,a0); })),
      meter_alu_group_phv_hash_shift_(default_adapter(meter_alu_group_phv_hash_shift_,chip_index(), pipeIndex, mauIndex, alu_index_))
{
  selector_alu_ctl_.reset();
  stateful_ctl_.reset();
  meter_alu_group_ctl_.reset();
  meter_alu_group_phv_hash_mask_.reset();
  meter_alu_group_phv_hash_shift_.reset();
}


void MauSelectorAlu::phv_hash_mask_write_callback(int a1, int a0) {
  if (a1 == alu_index_) { // check if write to our alu
    if (a0 == 0) {
      uint64_t v = meter_alu_group_phv_hash_mask_.meter_alu_group_phv_hash_mask(a1,a0);
      phv_hash_mask_ = (UINT64_C( 0xFFFFFFFF00000000 ) & phv_hash_mask_) | v;
    }
    else if (a0 == 1) {
      uint64_t v = meter_alu_group_phv_hash_mask_.meter_alu_group_phv_hash_mask(a1,a0);
      phv_hash_mask_ = (UINT64_C( 0x00000000FFFFFFFF ) & phv_hash_mask_) | (v<<32);
    }
  }
}
// Add new func to apply phv_hash_shift and phv_hash_mask
uint64_t MauSelectorAlu::get_alu_data(Phv *phv) {
  if (phv == NULL) return UINT64_C(0);
  uint8_t shift = meter_alu_group_phv_hash_shift_.meter_alu_group_phv_hash_shift();
  uint64_t inWord = logical_row_->get_meter_stateful_selector_alu_data(phv).get_word(0);
  // XXX: Need to special case shift >= 8 as undefined in C/C++
  uint64_t inWordShifted = (shift < 8) ?(inWord << (shift*8)) :UINT64_C(0);
  uint64_t outWord = inWordShifted & phv_hash_mask_;

#ifdef MAU_SELECTOR_ALU_SHOW_GET_ALU_DATA_PROCESSING
  printf("MauSelectorAlu::get_alu_data:              In=%08x %08x\n",
         static_cast<uint32_t>(inWord >> 32),
         static_cast<uint32_t>(inWord & UINT64_C(0xFFFFFFFF)));
  uint64_t im = inWord & phv_hash_mask_;
  printf("MauSelectorAlu::get_alu_data:        InMasked=%08x %08x\n",
         static_cast<uint32_t>(im >> 32),
         static_cast<uint32_t>(im & UINT64_C(0xFFFFFFFF)));
  uint64_t is = (shift < 8) ?(inWord << (shift*8)) :UINT64_C(0);
  printf("MauSelectorAlu::get_alu_data:       InShifted=%08x %08x\n",
         static_cast<uint32_t>(is >> 32),
         static_cast<uint32_t>(is & UINT64_C(0xFFFFFFFF)));
  uint64_t ism = is & phv_hash_mask_;
  printf("MauSelectorAlu::get_alu_data: InShiftedMasked=%08x %08x\n",
         static_cast<uint32_t>(ism >> 32),
         static_cast<uint32_t>(ism & UINT64_C(0xFFFFFFFF)));
  printf("MauSelectorAlu::get_alu_data: Shift=%d Mask=%08x %08x\n", shift,
         static_cast<uint32_t>(phv_hash_mask_ >> 32),
         static_cast<uint32_t>(phv_hash_mask_ & UINT64_C(0xFFFFFFFF)));
#endif
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
          "MauSelectorAlu::get_alu_data: IN=0x%016" PRIx64 " "
          "Shift=%d Mask=0x%016" PRIx64 "  OUT=0x%016" PRIx64 "\n",
          inWord, shift, phv_hash_mask_, outWord);
  return outWord;
}

bool MauSelectorAlu::is_resilient_hash_mode()
{
  return (selector_alu_ctl_.resilient_hash_mode());
}

void  MauSelectorAlu::get_hash_buckets(Phv *phv)
{
  BitVector<kHashOutputWidth>     hash;
  MauSbox                         sbox;

  //hash.set_word( logical_row_->get_meter_stateful_selector_alu_data(phv).get_word(0) & phv_hash_mask_ ,0);
  hash.set_word( get_alu_data(phv), 0); // call new func that does byte-shift and mask

  // Break the hash into 3 buckets
  hash_bucket_array_[0] = hash.get_word(0, 14);
  hash_bucket_array_[1] = hash.get_word(14, 14);
  hash_bucket_array_[2] = hash.get_word(28, 14);
  if (selector_alu_ctl_.sps_nonlinear_hash_enable()) {
    // SPS14 enabled... Scramble hash
    hash_bucket_array_[0] = sbox.sps14(hash_bucket_array_[0]);
    hash_bucket_array_[1] = sbox.sps14(hash_bucket_array_[1]);
    hash_bucket_array_[2] = sbox.sps14(hash_bucket_array_[2]);
  }
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
          "after sps14 hash values = (0x%x, 0x%x, 0x%x)\n",
          hash_bucket_array_[0], hash_bucket_array_[1], hash_bucket_array_[2]);
}

/* Per SramRow ; After sram_row_lookup*/
void MauSelectorAlu::set_s_word_from_selector_ram(BitVector<kSramWidth>  *selector_word)
{
  s_word_.set_from(0, *selector_word);
  s_word_msb_ = s_word_.get_byte(15); // From 15th byte onwards; or from 120bit onwards
  // TODO: add an assert if top bit is set
  s_word_msb_ &= 0x7f; // rtl only uses 7 bits
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
          "Selector-Word MSByte = 0x%x\n", s_word_msb_);
}

// return either 0xFF or port vector bit position that is
// chosen for resilient hash.
// If returned 0xFF, caller is expected to perform PLAN-B hash
// and pick new bit.
uint8_t MauSelectorAlu::get_resilient_selection_index(Phv *phv, BitVector<kSramWidth>  *selector_word)
{
  RMT_ASSERT(s_word_msb_ != 0);

  uint8_t hash_enable = selector_alu_ctl_.resilient_hash_enable();
  get_hash_buckets(phv);

  hash_mod_array_[0] = hash_bucket_array_[0] % s_word_msb_;
  hash_mod_array_[1] = hash_bucket_array_[1] % s_word_msb_;
  hash_mod_array_[2] = hash_bucket_array_[2] % s_word_msb_;
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
          "Resilient hash: hash-enable=0x%x  3 hash-mod values = %d,  %d,  %d\n",
          hash_enable, hash_mod_array_[0], hash_mod_array_[1], hash_mod_array_[2]);

  // In priority order (First hash_mod_array being higher prio than 3rd )
  // check of aliveness of port-vector and return bit position as 7b value

  if (((hash_enable & 0x1) != 0) && (s_word_.get_bit(hash_mod_array_[0]))) {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
            "Resilient hash on first attempt succeeded\n");
    return (hash_mod_array_[0]);
  }
  if (((hash_enable & 0x2) != 0) && (s_word_.get_bit(hash_mod_array_[1]))) {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
            "Resilient hash on second attempt succeeded\n");
    return (hash_mod_array_[1]);
  }
  if (((hash_enable & 0x4) != 0) && (s_word_.get_bit(hash_mod_array_[2]))) {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
            "Resilient hash on third attempt succeeded\n");
    return (hash_mod_array_[2]);
  }
  return (0xFF);
}


uint8_t MauSelectorAlu::get_planB_resilient_selection_index(Phv *phv, BitVector<kSramWidth>  *selector_word)
{
  RMT_ASSERT(s_word_msb_ != 0);

  // PlanB hash calculation Algo
  BitVector<kHashOutputWidth>     hash;
  MauSbox                         sbox;

  //hash.set_word(logical_row_->get_meter_stateful_selector_alu_data(phv).get_word(0) & phv_hash_mask_ ,0);
  hash.set_word( get_alu_data(phv), 0); // call new func that does byte-shift and mask

  uint32_t   scramble_18b_hash = (hash.get_word(42, 9) << 9) | (hash.get_word(5,9));
  if (selector_alu_ctl_.sps_nonlinear_hash_enable()) {
    scramble_18b_hash = sbox.sps18(scramble_18b_hash);
  }
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
          "PlanB resilient-hash. 18b-hash = 0x%x\n", scramble_18b_hash);

  /* odd and even permuted selector word will be in
   * permute_selector_word
   */
  BitVector<kSramWidth>   permute_selector_word_odd;
  BitVector<kSramWidth>   permute_selector_word_even;
  BitVector<kSramWidth>   copy_selector_word;
  copy_selector_word.set_from(0, *selector_word);
  permute_selector_word_odd.fill_all_zeros();
  permute_selector_word_even.fill_all_zeros();

  /* Step 2; Permute odd postioned bits */
  /* since bit-pos (which is odd-number) XOR {18b_scramble_hash[11:6],1}, results in
   * lsb bit = 0, means new position for the bit will be in even bi position.
   */
  uint8_t new_bit_pos, permute_hash, bit_at_i;
  permute_hash = (scramble_18b_hash & 0xfc0) >> 6;
  permute_hash = (permute_hash << 1);
  for (int i = 1; i < 120; i+=2) {
    new_bit_pos = i ^ permute_hash;
    bit_at_i = copy_selector_word.get_bit(i);
    if (bit_at_i) {
      permute_selector_word_odd.set_bit(new_bit_pos);
    }
  }
  /* Step 3 ; Permute even postioned bits */
  permute_hash = (scramble_18b_hash & 0x3f);
  permute_hash = (permute_hash << 1);
  for (int i = 0; i < 120; i+=2) {
    new_bit_pos = i ^ permute_hash;
    bit_at_i = copy_selector_word.get_bit(i);
    if (bit_at_i) {
      permute_selector_word_even.set_bit(new_bit_pos);
    }
  }

  permute_selector_word_even.or_with(permute_selector_word_odd);

  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
          "Permute Odd + Permute Even 128b selector ram word = "
          "0x%016" PRIx64 " 0x%016" PRIx64 "\n",
           permute_selector_word_even.get_word(0),
           permute_selector_word_even.get_word(64));

  if (permute_selector_word_even.is_zero()) {
    return 0xFF;
  }

  /* Step 4 */
  /* Priority circular encode */
  /* Priority encode = Find MSB bit position and clear out
   * all bits from msb-1 to 0 bit
   */
  uint8_t cir_prio_encode_val = 0;

  /* Make it circular priority encoder... using 0..63 as index obtained from scramble_18b_hash */
  /* Logic as shared with me by HW team */
  /* 1.  Beginning with bit location specified by hashbit[17:12], search for the
   *     leading 1 in the permuted 128b selector_word.  If one is found then output
   *     its encoded bit position (a 1 in bit position 0 outputs a value of 0).
   *     So you're searching from position hashbit[17:12] down to 0.
   * 2.  If a 1 is not found in the previous step, then just do a normal priority-encode of
   *     the 128b permuted selector_word beginning with bit 127.
   *      This effectively gives position hashbit[17:12] the highest priority.
   */

  int i = 0; bool found = false;
  uint8_t hd_ptr = (scramble_18b_hash & 0x3f000) >> 12;
  for (i = hd_ptr; i >= 0; i--) {
    if (permute_selector_word_even.get_bit(i)) {
      cir_prio_encode_val = i;
      found = true;
      break;
    }
  }
  if (!found) {
    /* search using head ptr did NOT yield high bit. Use priority_encode_val */
    for (i = 127; i >= 0; i--) {
      if (permute_selector_word_even.get_bit(i)) {
        cir_prio_encode_val = i;
        break;
      }
    }
  }
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
          "Head ptr for circular prio encoding = 0x%x; "
          "Encoded value before reverse map = 0x%x \n", hd_ptr,
          cir_prio_encode_val);

  /* Step 5 : reverse map */

  if (cir_prio_encode_val & 0x1) {
    permute_hash = (scramble_18b_hash & 0xfc0) >> 6;
    permute_hash = (permute_hash << 1);
    cir_prio_encode_val = cir_prio_encode_val ^ permute_hash;
  } else {
    permute_hash = (scramble_18b_hash & 0x3f);
    permute_hash = (permute_hash << 1);
    cir_prio_encode_val = cir_prio_encode_val ^ permute_hash;
  }
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
          "Final Plan-B selection-index = 0x%x\n", cir_prio_encode_val);

  return (cir_prio_encode_val);
}

uint8_t MauSelectorAlu::get_fair_hash_selection_index(Phv *phv, BitVector<kSramWidth> *selector_word)
{
  RMT_ASSERT(s_word_msb_ != 0);

  // 1. C = Count all active ports or high bits in port-vector (selector_word[0:119])
  // 2. Note position of all active ports
  // 3. selection-index = hash-value MOD C
  // 4. For the computed selection-index, find the actual bit position in selector_word

  uint64_t    word_hi = selector_word->get_word(64, 56); // Upper 8 bits are not port-vector
  uint64_t    word_lo = selector_word->get_word(0, 64);
  uint8_t     array[120] = {0}; // Stores bit position.
  uint8_t     k = 0;
  uint64_t    mask = 1;

  // On bottom 64 bits
  for (int i = 0; i < 64; i++) {
    if (mask & word_lo) {
      array[k++] = i;
    }
    mask = mask << 1;
  }
  // On higher 56bits
  mask = 1;
  for (int i = 0; i < 56; i++) {
    if (mask & word_hi) {
      array[k++] = i + 64;
    }
    mask = mask << 1;
  }
  // pick the right hash-bucket MOD k
  get_hash_buckets(phv);
  // out of 3x14b hash, pick one that is configured.
  uint8_t bucket = selector_alu_ctl_.selector_fair_hash_select();
  RMT_ASSERT(bucket < 3);
  uint16_t hash = hash_bucket_array_[bucket];

  if (k != s_word_msb_) {
    RMT_LOG(RmtDebug::error(),
            "Fair hash mode but bitcount(%d) does NOT match MSB(%d)"
            " - using MSB\n", k, s_word_msb_);
    THROW_ERROR(-3);
  }
  uint8_t hashmod = hash % s_word_msb_;
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
          "Fair hash chosen = %d, configure-hash-bucket 0/1/2 = %d, "
          "up port-count = %d, hash-mod = %d\n",
          hash, bucket, s_word_msb_, hashmod);

  return (array[hashmod]);
}



// At the end of the successful execution of this function,
// selection-index that maps to a live port is computed
void
MauSelectorAlu::run_alu_with_phv(Phv *phv, BitVector<kSramWidth> *selector_word)
{
  // If no valid selection_index, signal this using invalid
  // selection_index_output (0x80000000)
  uint32_t selection_index_output = kSelectorAluInvalOutput;
  uint8_t  selection_index = 0xFF;

  set_s_word_from_selector_ram(selector_word);

  // Don't do anything unles MSB of data is non zero
  if (s_word_msb_ != 0) {
    if (is_resilient_hash_mode()) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
              " Selector Hash Mode = Resilient\n");
      selection_index = get_resilient_selection_index(phv, selector_word);
      // force planB for unit-testing purposes...
      //selection_index = 0xFF;
      if (selection_index == 0xFF) {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
                " Attempting Plan-B resilient Hash \n");
        // All 3 hash attempts to find active port failed. Triger plan B hash
        selection_index = get_planB_resilient_selection_index(phv, selector_word);
      }
    } else {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
              " Selector Hash Mode = Non-Resilient/Fair Hash \n");
      selection_index = get_fair_hash_selection_index(phv, selector_word);
    }
  }

  // Update selection_index_output if we have a valid selection_index
  if (selection_index < 0xFF)
    selection_index_output = static_cast<uint32_t>(selection_index);

  // Store selector_index as "selector_rd_addr_" in home-row/same row as
  // its selector alu. This is needed when both Selector-Alu and action-ram are on same row
  logical_row_->set_selector_rd_addr(selection_index_output);

  // Notify mau whether selection output invalid as may determine
  // whether certain fallback addresses are used
  mau_->set_selector_alu_output_invalid(alu_index_, is_inval_output(selection_index_output));

  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
          " Selection index = 0x%x\n", selection_index);
}


void
MauSelectorAlu::run_alu_with_state(MauExecuteState *state) {

  // handle the data in / lack of forwarding first so get_input_data() always returns correct value

  uint32_t addr = 0u;
  logical_row_->meter_rd_addr(&addr);

  BitVector<kSramWidth> data(UINT64_C(0));
  if (Address::meter_addr_op_enabled(addr)) {
    logical_row_->stats_alu_rd_data(&data);
  }

  uint64_t current_time;
  uint32_t const addr_mask = Address::kMeterAddrAddrMask & (~ 0x7f); // to mask off bottom 7 bits
  bool current_time_valid = state->get_relative_time(&current_time, selector_alu_ctl_.selector_thread() ? false : true );
  if ( current_time_valid &&
       (current_time == (1 + last_relative_time_)) &&
       Address::meter_addr_op_enabled( last_address_ ) &&
       Address::meter_addr_op_enabled( addr ) &&
       (( addr & addr_mask ) == ( last_address_ & addr_mask ))) {
    // ALU is being used one tick later with the same word address, so
    // use the last data value. Usually this will be the same as the
    // current data value (so there's no harm in using the last
    // value). However if the data has been modified by a stateful alu
    // in the previous cycle we need to use the last data because this
    // new data is not forwarded properly.
    data_used_.copy_from( last_data_in_ );
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
            "Selector using last word "
            "0x%016" PRIx64 "%016" PRIx64 " (time=%" PRIi64 " last=%" PRIi64 "| addr=%x last=%x mask=%x)\n",
            data_used_.get_word(64),
            data_used_.get_word(0),
            current_time, last_relative_time_,
            addr, last_address_,addr_mask );
  }
  else {
    data_used_.copy_from( data );
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauSelectorAlu),
            "Selector using current word "
            "0x%016" PRIx64 "%016" PRIx64 " (time=%" PRIi64 " last=%" PRIi64 "| addr=%x last=%x)\n",
            data_used_.get_word(64),
            data_used_.get_word(0),
            current_time, last_relative_time_,
            addr, last_address_ );
  }
  // calls without current_time_valid are handle eop, so don't update
  //  TODO: why is function being called on eop anyway?
  if (current_time_valid) {
    last_relative_time_=current_time;
    last_address_= addr;
    last_data_in_.copy_from( data );
  }

  if (selector_alu_ctl_.selector_enable() == 0) return;
  if (!Address::meter_addr_op_enabled(addr)) return;

  int op = Address::meter_addr_op4(addr);
  // XXX: Error if extracted meter addr is a sweep addr
  // XXX: Complain if ANY invalid MeterOP4 for SelectorALU (might allow SALU OPs)
  uint32_t ok_ops = MauDefs::kMeterAluHdrtimeSelOps;
  if (stateful_ctl_.salu_enable()) ok_ops |= MauDefs::kMeterAluHdrtimeSaluOps;
  // XXX: only check at hdrtime
  if ((state->op_ == StateOp::kStateOpPhvLookup) && (((ok_ops >> op) & 1) == 0)) {
    RMT_LOG(RmtDebug::error(MauMeterAlu::kRelaxOpCheck),
            "MeterOP %d (%s) NOT supported by SelectorALU (addr=0x%x)\n",
            op, MauMeterAlu::get_opstr(op), addr);
  }

  int wr_shift = Address::selector_addr_get_shift(addr);
  int wr_width = Address::selector_addr_get_width(addr, wr_shift);
  int wr_offset = Address::selector_addr_get_offset(addr, wr_shift);
  int rd_width = wr_width;
  int rd_offset = wr_offset;

  switch (op) {
    case Address::kMeterOp4CfgRd:
      state->data_.copy_from(data);
      state->ret_ = 1;
      break;
    case Address::kMeterOp4CfgWr:
      if (Address::meter_addr_op_enabled(state->rw_raddr_)) {
        // If movereg, valid read addr will be in state->rw_raddr_
        int rd_shift = Address::selector_addr_get_shift(state->rw_raddr_);
        rd_width = Address::selector_addr_get_width(state->rw_raddr_, rd_shift);
        rd_offset = Address::selector_addr_get_offset(state->rw_raddr_, rd_shift);
        RMT_ASSERT(wr_width == rd_width); // Widths must match if movereg
      }
      if (wr_width < kSramWidth) {
        RMT_ASSERT((wr_width > 0) && (wr_width <= 64));
        uint64_t word = state->data_.get_word(rd_offset, rd_width);
        data.set_word(word, wr_offset, wr_width);
      } else {
        data.copy_from(state->data_);
      }
      logical_row_->set_stats_wr_data(&data);
      break;
    case Address::kMeterOp4Selector:
      if (state->match_phv_ != NULL) run_alu_with_phv(state->match_phv_,&data_used_);
      // note: write is of 'data', not data_used_ because synth2port logic makes sure
      //  that old stuff is not written to the rams
      logical_row_->set_stats_wr_data(&data);
      break;
  }
}


void MauSelectorAlu::get_input_data(BitVector<kSramWidth> *data) {
  data->copy_from(data_used_);
}

}
