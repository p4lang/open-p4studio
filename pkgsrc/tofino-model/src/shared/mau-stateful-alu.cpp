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
#include <bitset>
#include <limits>
#include <rmt-log.h>
#include <address.h>
#include <mau-stateful-alu.h>
#include <mau-info.h>
#include <register_adapters.h>



namespace MODEL_CHIP_NAMESPACE {

  MauStatefulAlu::MauStatefulAlu(RmtObjectManager *om, int pipeIndex,
                                 int mauIndex, int logicalRowIndex, Mau *mau)
      : MauObject(om, pipeIndex, mauIndex, kType, logicalRowIndex, mau),
        alu_index_(get_stateful_alu_regs_index(logicalRowIndex)),
        chip_salu_(om, pipeIndex, mauIndex, logicalRowIndex, alu_index_, mau),
        reg_file_(default_adapter(reg_file_,chip_index(), pipeIndex, mauIndex, alu_index_)),
        cmp_reg_(default_adapter(cmp_reg_,chip_index(), pipeIndex, mauIndex, alu_index_)),
        com_reg_(default_adapter(com_reg_,chip_index(), pipeIndex, mauIndex, alu_index_)),
        state_reg_(default_adapter(state_reg_,chip_index(), pipeIndex, mauIndex, alu_index_)),
        stateful_ctl_(default_adapter(stateful_ctl_,chip_index(), pipeIndex, mauIndex, alu_index_)),
        math_ctl_(default_adapter(math_ctl_,chip_index(), pipeIndex, mauIndex, alu_index_)),
        math_tbl_(default_adapter(math_tbl_,chip_index(), pipeIndex, mauIndex, alu_index_))
  {
    reg_file_.reset();
    cmp_reg_.reset();
    com_reg_.reset();
    state_reg_.reset();
    stateful_ctl_.reset();
    math_ctl_.reset();
    math_tbl_.reset();
    reset_alu_state();

    // Cmp alus are numbered in JBay, but have names in Tofino
    for (int which_alu=0;which_alu<kStatefulAluCmpAlus;++which_alu) {
      if (kStatefulAluCmpAlus == 2) {
        snprintf(cmp_name_[which_alu],3, "%s", which_alu ? "Hi":"Lo");
      }
      else {
        snprintf(cmp_name_[which_alu],3, "%d", which_alu );
      }
    }

  }
  MauStatefulAlu::~MauStatefulAlu() { }



  void MauStatefulAlu::get_width_and_mask(int instr_addr) {
    RMT_ASSERT(0 <= instr_addr && 4 > instr_addr);

    int w = com_reg_.salu_datasize(instr_addr);
    // Enforce max width of 32 for Tofino, and 128 for JBay
    RMT_ASSERT(chip_salu_.check_width(w));
    decoded_inputs_.real_width = 1 << w; // this could be 128 in JBay
    // For nearly all purposes we need to restrict the width to 64
    decoded_inputs_.width = decoded_inputs_.real_width>64 ? 64 : decoded_inputs_.real_width;
    decoded_inputs_.mask = ( 1 == decoded_inputs_.width) ? UINT64_C(0x1) :
            ( 8 == decoded_inputs_.width) ? UINT64_C(0xFF) :
            (16 == decoded_inputs_.width) ? UINT64_C(0xFFFF) :
            (32 == decoded_inputs_.width) ? UINT64_C(0xFFFFFFFF) :
                                            UINT64_C(0xFFFFFFFFFFFFFFFF); // for JBay only

  }

uint64_t MauStatefulAlu::get_cmp_constant(bool use_reg_file,
                                          int instr_addr,
                                          int width,
                                          int which_alu,
                                          uint64_t mask,
                                          model_core::LogBuffer& log_buf,
                                          const char *const constant_type) {
  uint64_t c=0;
  if (use_reg_file) {
    if ( width != 1 ) { // c=0 for 1b ops
      uint8_t reg_file_addr = chip_salu_.get_regfile_addr(cmp_reg_,instr_addr,which_alu);
      c = reg_file_.salu_const_regfile(reg_file_addr);
      uint64_t extra_msbs = chip_salu_.get_reg_file_msbs( reg_file_addr );
      c |= (extra_msbs << 32);
      if (is_jbay_or_later()) {
        RMT_ASSERT(MauChipStatefulAlu::kExtraMSBits == 2);
        // JBay, fix up the mask to add the two extra bits
        mask = ( 1 == width) ? UINT64_C(0x1) :
            ( 8 == width) ? UINT64_C(0x3FF) :
            (16 == width) ? UINT64_C(0x3FFFF) :
            (32 == width) ? UINT64_C(0x3FFFFFFFF) :
            UINT64_C(0xFFFFFFFFFFFFFFFF);
      } else {
        RMT_ASSERT(MauChipStatefulAlu::kExtraMSBits == 0);
      }
      c &= mask;

      // For 64 bit wide instructions the constant is sign extended same as 32bits
      c = sign_ext64(c, (width==64 ? 32 : width) + MauChipStatefulAlu::kExtraMSBits );

      // Print used to be done before sign extension, and this code assumes extension from
      //   width bits (for 8,16,32) but now extension is more complicated do it afterwards.
      //sprintf(lg, "%s C%s RegFile %d", lg, constant_type,
      //        width==8 ? (int8_t)c : width==16 ? (int16_t)c : (int32_t)c);
      log_buf.Append(" C%s RegFile %" PRId64, constant_type, c );

    }
  } else {
    c = cmp_reg_.salu_cmp_const_src(instr_addr, which_alu);
    const uint8_t sign_bit_mask = 1 << ( kStatefulAluCmpConstWidth - 1 );
    const uint8_t const_mask    = (1 << kStatefulAluCmpConstWidth) - 1;
    log_buf.Append(" C%s Immediate %d", constant_type,
            c & sign_bit_mask ? (int8_t)(((~const_mask) | c)&0xFF) : (int)(c & const_mask));
    c = sign_ext64(c, kStatefulAluCmpConstWidth);
  }
  return c;
}


void MauStatefulAlu::decode_inputs( uint32_t addr, uint64_t present_time,
                                      BitVector<MauStatefulAlu::kStatefulMeterAluDataBits> phv,
                                      BitVector<kDataBusWidth> *data_in ) {
    // Cache results - when the whole MAU is running this will get done as a result of running the cmp_alu's
    //   early, which we JBay needs to work out the TMatch bus. In the standalone DV environment this will
    //   get called when calculate_output gets called as before.
    if (decoded_inputs_.valid) return;

    if ( Address::meter_addr_is_stateful_clear(addr) ) {
      decoded_inputs_.valid = true;
      decoded_inputs_.stateful_clear = true;
      return;
    }
    decoded_inputs_.stateful_clear = false;

    // Decode address to get the instruction
    decoded_inputs_.instr_addr = Address::meter_addr_stateful_instruction(addr);
    RMT_ASSERT(0 <= decoded_inputs_.instr_addr && 4 > decoded_inputs_.instr_addr);
    decoded_inputs_.is_run_stateful = Address::meter_addr_is_run_stateful(addr);

    // Get operand data width.
    decoded_inputs_.width = 0;
    decoded_inputs_.mask = 0;
    get_width_and_mask(decoded_inputs_.instr_addr);

    // Get instruction width (single or double)
    decoded_inputs_.dbl = com_reg_.salu_op_dual(decoded_inputs_.instr_addr);
    // can't have double instructions with 128 bit width
    RMT_ASSERT( ! ( decoded_inputs_.dbl && (decoded_inputs_.real_width == 128)) );

    // Extract PHV operands based on width (Tofino at most 32 bits, JBay up to 64 bits)
    decoded_inputs_.phv_lo = (decoded_inputs_.width==1) ? 0 : phv.get_word(0,decoded_inputs_.width);
    decoded_inputs_.phv_hi = (decoded_inputs_.width==1) ? 0 : phv.get_word(decoded_inputs_.width,decoded_inputs_.width);

    decoded_inputs_.min_max_enable = chip_salu_.get_salu_minmax_enable(com_reg_,decoded_inputs_.instr_addr);

    // Extract RAM operands based on width and subword (these addresses are not huffman encoded)
    // Note that for single width cases, it is possible that ram_hi is taken
    // from a bit position larger than 128.  In this case the value is zero.
    decoded_inputs_.subword=0;
    decoded_inputs_.subword_width = decoded_inputs_.dbl ? 2*decoded_inputs_.real_width : decoded_inputs_.real_width;
    switch (decoded_inputs_.subword_width) {
      case  1: decoded_inputs_.subword= addr & 0x7F;     break;
      case  8: decoded_inputs_.subword= (addr>>3) & 0xF; break;
      case 16: decoded_inputs_.subword= (addr>>4) & 0x7; break;
      case 32: decoded_inputs_.subword= (addr>>5) & 0x3; break;
      case 64: decoded_inputs_.subword= (addr>>6) & 0x1; break;
      case 128: decoded_inputs_.subword=0; break;
    }

    // Work out flags to spot weird forwarding cases
    decoded_inputs_.word_addr = Address::meter_addr_get_word_addr(addr);
    decoded_inputs_.same_word_as_last_cycle = (present_time == (last_time_+1)) && (decoded_inputs_.word_addr == last_word_addr_);
    decoded_inputs_.same_subword_as_last_cycle = decoded_inputs_.same_word_as_last_cycle && (decoded_inputs_.subword == last_subword_);

    // Detect the case where the last instruction was min_max and this one is not min_max and the same word
    //  is being accessed. Strictly speaking this only applies when there is a post inc/dec, but if there isn't
    //  a post inc/dec then the ram word will not be changed and using the old one makes no difference)
    bool non_min_max_after_min_max_same_word = last_was_min_max_ && ( ! decoded_inputs_.min_max_enable ) &&
                                                  decoded_inputs_.same_word_as_last_cycle;

    // In Tofino the max width is 32, so the top 32 bits of these values will always be zero
    // In JBay max width is 64, so ram_hi and ram_lo could be getting 64 bit values
    if ( !non_min_max_after_min_max_same_word ) { // normal case
      decoded_inputs_.ram_lo = data_in->get_word(decoded_inputs_.subword*decoded_inputs_.subword_width, decoded_inputs_.width);
      decoded_inputs_.ram_hi = (decoded_inputs_.width==1) ? 0 : data_in->get_word(decoded_inputs_.subword*decoded_inputs_.subword_width+decoded_inputs_.width, decoded_inputs_.width);
    }
    else { // non_min_max_after_min_max_same_word case
      // In the case where there is a non-min_max instruction after a min_max instruction, everything*
      //  in the stateful ALU sees the previous ram data. This means we can do the change here. In the
      //  math unit forwarding case the old data is only seen by the math unit, so it has to be done
      //  separately using math_ram_lo and math_ram_hi
      // (* actually the minmax unit sees the new ram data, but as this is a non-min_max instruction
      //   it doesn't matter)
      if ( last_address_moved_ ) {
        // This case is for movereg, I'm not sure it is needed here, but it was in the math unit case
        decoded_inputs_.ram_lo = last_ram_lo;
        decoded_inputs_.ram_hi = last_ram_hi;
      }
      else {
        decoded_inputs_.ram_lo = last_data_in.get_word(decoded_inputs_.subword*decoded_inputs_.subword_width, decoded_inputs_.width);
        decoded_inputs_.ram_hi = (decoded_inputs_.width==1) ? 0 : last_data_in.get_word(decoded_inputs_.subword*decoded_inputs_.subword_width+decoded_inputs_.width, decoded_inputs_.width);
      }
    }

    decoded_inputs_.cmp_ram_lo = 0;
    decoded_inputs_.cmp_ram_hi = 0;
    if ( decoded_inputs_.width != 1) {
      decoded_inputs_.cmp_ram_lo = decoded_inputs_.ram_lo;
      decoded_inputs_.cmp_ram_hi = decoded_inputs_.ram_hi;
    }

    decoded_inputs_.valid=true;
  }

  void MauStatefulAlu::calculate_cmp_alu(uint32_t addr,uint64_t present_time,
                                            BitVector<MauStatefulAlu::kStatefulMeterAluDataBits> phv,
                                            BitVector<kDataBusWidth> *data_in
                                            ) {
    decode_inputs( addr, present_time, phv, data_in);

    if ( decoded_inputs_.stateful_clear ) {
      return;  // cmp alus don't do anything for stateful clear instructions
    }
    for (int alu=0;alu<kStatefulAluCmpAlus;++alu) {
      (void) run_cmp(alu);
    }
  }

  void MauStatefulAlu::calculate_output(uint32_t addr,
                                        BitVector<MauStatefulAlu::kStatefulMeterAluDataBits> phv,
                                        BitVector<kDataBusWidth> *data_in,
                                        BitVector<kDataBusWidth> *data_out,
                                        BitVector<kActionOutputBusWidth> *action_out,
                                        uint64_t present_time, bool ingress,
                                        MauStatefulAlu::StatefulBus match_bus,
                                        MauStatefulAlu::StatefulBus learn_or_match_bus,
                                        uint32_t *sel_index) {
    reset_alu_state();
    bool ram_changed = false;

    mau()->mau_info_incr(MAU_METER_ALU_SALU_INVOCATIONS);

    // When whole MAU runs this decode_inputs will have already been done and cached.
    //   But in the standalone DV environment it will do its work now.
    decode_inputs( addr, present_time, phv, data_in);

    if ( decoded_inputs_.stateful_clear ) {
      chip_salu_.calculate_stateful_clear( stateful_ctl_, state_reg_, data_out, action_out );
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),
              "StatefulALU CLEAR action_out=%08" PRIx64 " d_out=0x%016" PRIx64 "%016" PRIx64 "\n",
              action_out->get_word(0,32), data_out->get_word(64), data_out->get_word(0));
      return;
    }

    // TODO handle wider PHV in Jbay
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),
            "StatefulALU Instr=%d %d-bits %s-width Addr 0x%08x Phv 0x%016" PRIx64
            " Ram 0x%016" PRIx64 "_%016" PRIx64 "\n", decoded_inputs_.instr_addr, decoded_inputs_.real_width,
            decoded_inputs_.dbl?"Double":"Single", addr, phv.get_word(0), data_in->get_word(64),
            data_in->get_word(0));


    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),
            "StatefulALU RAM Hi/LO %#" PRIx64 "/%#" PRIx64 "  PHV Hi/Lo %#" PRIx64 "/%#" PRIx64 " subword %d\n",
            decoded_inputs_.ram_hi, decoded_inputs_.ram_lo, decoded_inputs_.phv_hi, decoded_inputs_.phv_lo, decoded_inputs_.subword);

    /////////////////////////////////////////////
    // Handle weird math unit forwarding start

    RMT_LOG(RmtDebug::verbose(),"StatefulALU same_subword_as_last_cycle=%s same_word_as_last_cycle=%s last_address_moved=%s\n",decoded_inputs_.same_subword_as_last_cycle?"true":"false",
            decoded_inputs_.same_word_as_last_cycle?"true":"false",last_address_moved_?"true":"false");

    // Math unit takes 2 cycles, so when operating on the same subword as last cycle the math unit uses the
    //  data in from the previous cycle (ie, not the updated value which the model has written back
    //  to the RAM in zero time)
    // The old data is only seen by the math unit, so it has to be done separately from the other case
    //  in decode_inputs
    // See long comment in update_addresses for why last_address_moved uses last_ram_lo and last_ram_hi.
    // The second case (same_word_as_last_cycle) will pick up the non-movereg case where the math unit is
    //  accessing the same subword by getting the old data from last_data_in. It will also pick up the
    //  more general case where there is an access to a subword of a different width that overlaps with
    //  the first subword and the math unit sees the old data again. Of course if the two subwords do
    //  not overlap then the subword will not have been updated anyway and it won't matter that we
    //  have used last_data_in rather than the current data in.
    //  This second case does not need to handle movereg as we don't support movereg between subwords
    //  of different widths.
    // See XXX and XXX for the history.
    uint32_t math_ram_lo = decoded_inputs_.ram_lo & 0xFFFFFFFF;
    uint32_t math_ram_hi = decoded_inputs_.ram_hi & 0xFFFFFFFF;
    if ( decoded_inputs_.same_subword_as_last_cycle && last_address_moved_) {
        math_ram_lo = last_ram_lo & 0xFFFFFFFF;
        math_ram_hi = last_ram_hi & 0xFFFFFFFF;

        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),
                "StatefulALU MATH_RAM Hi/LO  %#x/%#x (same movereg subword as last cycle)\n",
                math_ram_hi, math_ram_lo);
    }
    else if (decoded_inputs_.same_word_as_last_cycle) {
        math_ram_lo = last_data_in.get_word(decoded_inputs_.subword*decoded_inputs_.subword_width, decoded_inputs_.width);
        math_ram_hi = (decoded_inputs_.width==1) ? 0 : last_data_in.get_word(decoded_inputs_.subword*decoded_inputs_.subword_width+decoded_inputs_.width, decoded_inputs_.width);

        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),
                "StatefulALU MATH_RAM Hi/LO %#x/%#x (same word as last cycle)\n",
                math_ram_hi, math_ram_lo);
    }

    // update forwarding variables
    last_address_moved_ = false;
    last_word_addr_ = decoded_inputs_.word_addr;
    last_subword_ = decoded_inputs_.subword;

    last_time_ = present_time;

    // Handle weird math unit forwarding end
    /////////////////////////////////////////////


    bool sbus = chip_salu_.get_sbus(com_reg_, alu_index_, decoded_inputs_.instr_addr,
                                    match_bus,learn_or_match_bus);

    // In Tofino there are two cmp ALUs called  cmp-lo and cmp-hi which correspont to 0 and 1.
    //  In JBay the cmp ALUs are numbered.
    bool cmp_alu_out[kStatefulAluCmpAlus];
    int cmp=0; // One bit for each cmp alu output
    for (int alu=0;alu<kStatefulAluCmpAlus;++alu) {
      // When whole MAU runs run_cmp will have already been done and cached
      //   But in the standalone DV environment it will do its work now.
      bool cmp_alu_out_pre_sbus = run_cmp(alu);
      // OR in the TMatch match signal
      cmp_alu_out_pre_sbus      |= cmp_alu_out_[alu].tmatch_match;

      cmp_alu_out[alu] = chip_salu_.calculate_cmp_with_sbus( cmp_reg_, com_reg_, decoded_inputs_.instr_addr,
                                                             alu, cmp_alu_out_pre_sbus, sbus );
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "Cmp-ALU-%s After TMatch=%s, After SBus=%s\n",cmp_name_[alu],
              cmp_alu_out_pre_sbus?"true":"false",
              cmp_alu_out[alu]?"true":"false");
      if (cmp_alu_out[alu]) cmp |= 1<<alu;
    }
    RMT_ASSERT(cmp>=0 && cmp < (1<<kStatefulAluCmpAlus));
    // Convert the compare ALU result to a 1 hot encoded value.
    int cmp_alu_output = 1 << cmp;

    // In one bit mode the State ALUs run a bit differently...  Only Lo-0 runs
    // and it operates on the entire 128-bit RAM word.
    uint64_t state_lo1 = 0, state_lo1_raw = 0;
    uint64_t state_lo2 = 0, state_lo2_raw = 0;
    uint64_t state_hi1 = 0, state_hi1_raw = 0;
    uint64_t state_hi2 = 0, state_hi2_raw = 0;

    int64_t alu2_hi_as = 0; // operands might be needed for divide operation
    int64_t alu2_hi_bs = 0;
    if (1 == decoded_inputs_.width) {
      state_lo1_raw = run_state_one_bit(addr, decoded_inputs_.instr_addr, data_in, data_out);
    } else {
      // Execute alu1/2-hi/lo
      //  note: lo must be done before hi for carry propagation to work on JBay
      state_lo1_raw = run_state_lo_1(decoded_inputs_.phv_hi, decoded_inputs_.phv_lo, decoded_inputs_.ram_hi, decoded_inputs_.ram_lo, decoded_inputs_.instr_addr);
      state_lo2_raw = run_state_lo_2(decoded_inputs_.phv_hi, decoded_inputs_.phv_lo, decoded_inputs_.ram_hi, decoded_inputs_.ram_lo, math_ram_hi, math_ram_lo, decoded_inputs_.instr_addr);
      state_hi1_raw = run_state_hi_1(decoded_inputs_.phv_hi, decoded_inputs_.phv_lo, decoded_inputs_.ram_hi, decoded_inputs_.ram_lo, decoded_inputs_.instr_addr);
      state_hi2_raw = run_state_hi_2(decoded_inputs_.phv_hi, decoded_inputs_.phv_lo, decoded_inputs_.ram_hi, decoded_inputs_.ram_lo, decoded_inputs_.instr_addr,&alu2_hi_as,&alu2_hi_bs);
    }

    // Predicate the state ALUs.  If the compare ALU's output anded with the
    // predication mask is zero, the state ALU output will be disabled (predicated
    // off), if it is non-zero the state ALU output will be enabled (predicated on).
    bool pred_lo1_off = !(cmp_alu_output & state_reg_.salu_pred(decoded_inputs_.instr_addr, 1));
    bool pred_lo2_off = !(cmp_alu_output & state_reg_.salu_pred(decoded_inputs_.instr_addr, 0));
    bool pred_hi1_off = !(cmp_alu_output & state_reg_.salu_pred(decoded_inputs_.instr_addr, 3));
    bool pred_hi2_off = !(cmp_alu_output & state_reg_.salu_pred(decoded_inputs_.instr_addr, 2));

    state_lo1 = ((decoded_inputs_.width == 1) || (!pred_lo1_off)) ? state_lo1_raw : 0;
    state_lo2 = !pred_lo2_off ? state_lo2_raw : 0;
    state_hi1 = !pred_hi1_off ? state_hi1_raw : 0;
    state_hi2 = !pred_hi2_off ? state_hi2_raw : 0;

    // The 1 and 2 halves are ORed together.
    uint64_t state_lo = state_lo1 | state_lo2;
    uint64_t state_hi = state_hi1 | state_hi2;

    // Now do the min/max operation (JBay only)
    uint32_t minmax_pre_inc_dec_value=0;
    if ( decoded_inputs_.min_max_enable  ) {
      // Check settings are correct for minmax
      for (int state_alu=0;state_alu<4;++state_alu) {
        // must be an arith operation and a NOP
        RMT_ASSERT( state_reg_.salu_arith(decoded_inputs_.instr_addr, state_alu) );
        int op = state_reg_.salu_op(decoded_inputs_.instr_addr, state_alu);
        RMT_ASSERT( op == kNop3 );
        // For minmax all ALUs must be predicated on all the time by seeing salu_pred to all ones
        constexpr uint32_t pred_all_ones = (1<<(1<<kStatefulAluCmpAlus))-1;
        RMT_ASSERT( pred_all_ones == state_reg_.salu_pred(decoded_inputs_.instr_addr, state_alu));
      }
      RMT_ASSERT( decoded_inputs_.real_width == 128 );
      RMT_ASSERT( ! decoded_inputs_.dbl );
      // state alus must be programmed to output 0
      //RMT_ASSERT( (state_lo == 0) && (state_hi == 0) );
      // TODO: put back previous assert that doesn't mask?
      RMT_ASSERT( ((state_lo & UINT64_C(0xFFFFFFFF)) == 0) && ((state_hi & UINT64_C(0xFFFFFFFF)) == 0) );

      chip_salu_.calculate_minmax( com_reg_, state_reg_, cmp_reg_, reg_file_,
                                   decoded_inputs_.instr_addr,
                                   decoded_inputs_.same_word_as_last_cycle, last_was_min_max_, last_data_in, last_data_out,
                                   &phv, data_in, data_out,
                                   &state_lo, &state_hi, &minmax_pre_inc_dec_value, &minmax_mask_was_zero_);
      minmax_index_ = state_hi;
      *sel_index = minmax_mask_was_zero_ ?MauDefs::kSelectorAluInvalOutput :minmax_index_;
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"MinMax index: %" PRIx64 "%s%s\n",state_hi,
              decoded_inputs_.same_word_as_last_cycle?" same word as last cycle":"", last_was_min_max_?", last was min/max":"");
      last_was_min_max_ = true;
    }
    else {
      last_was_min_max_ = false;
    }
    // Update the RAM word with the output of the state ALUs:
    // - min_max prevents all other updates
    // - If this was a 1b operation then the entire RAM word is updated.
    // - Only if this was a double width operation will the state_hi update the
    //   RAM word.
    // - If both hi and lo are predicated then the RAM word is not updated.
    //
    if ( decoded_inputs_.min_max_enable ) {
      // any needed updates to data_out are done in calculate_minmax()
      ram_changed = ! data_out->equals( *data_in );
    }
    else if (1 == decoded_inputs_.width) {
      // The run_state_one_bit function already set the new RAM word.
      // However, if the ALU is predicated off it should be reset back to the
      // original value.
      if (pred_lo1_off) {
        // data_out->copy_from(*data_in);  // Actually 1b instructions always modify I think
      }
      ram_changed = true;

    } else {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),
              "StatefulALU S-ALU-1-Lo Predicated %s (cmp-out 0x%x, mask 0x%x)\n",
              pred_lo1_off ? "off":"on", cmp_alu_output,
              state_reg_.salu_pred(decoded_inputs_.instr_addr, 1));
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),
              "StatefulALU S-ALU-2-Lo Predicated %s (cmp-out 0x%x, mask 0x%x)\n",
              pred_lo2_off ? "off":"on", cmp_alu_output,
              state_reg_.salu_pred(decoded_inputs_.instr_addr, 0));
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),
              "StatefulALU S-ALU-1-Hi Predicated %s (cmp-out 0x%x, mask 0x%x)\n",
              pred_hi1_off ? "off":"on", cmp_alu_output,
              state_reg_.salu_pred(decoded_inputs_.instr_addr, 3));
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),
              "StatefulALU S-ALU-2-Hi Predicated %s (cmp-out 0x%x, mask 0x%x)\n",
              pred_hi2_off ? "off":"on", cmp_alu_output,
              state_reg_.salu_pred(decoded_inputs_.instr_addr, 2));
      data_out->copy_from(*data_in);
      // this used to set at most 32 bits, but now with flyover bits on JBay it might
      //   set up to 64 bits, which is what decoded_inputs_.width is limtted to.
      int set_width = decoded_inputs_.width;
      if ((decoded_inputs_.dbl || (decoded_inputs_.real_width==128)) && (!pred_hi1_off || !pred_hi2_off)) {
        // Double width operation and ALU-Hi is not predicated off, update the
        // upper half of the RAM word.
        data_out->set_word(state_hi, decoded_inputs_.subword*decoded_inputs_.subword_width+decoded_inputs_.width, set_width);
        ram_changed = true;
      }
      if (!pred_lo1_off || !pred_lo2_off) {
        // Single or double width operation and ALU-Lo is not predicated off,
        // update the lower half of the RAM word.
        data_out->set_word(state_lo, decoded_inputs_.subword*decoded_inputs_.subword_width, set_width);
        ram_changed = true;
      }
    }

    // Extract the RAM hi and low subwords again now that they have been
    // modified as might be needed by the output ALU.
    // - Note that if this is single width, then the RAM hi subword should
    //   actually be taken from the ALU result instead since it is not written
    //   back to memory.
    // - Note that if this is a 1-bit operation, then the 1-bit output of
    //   state-alu-1-lo should be used as the result.
    uint64_t new_ram_hi = 0, new_ram_lo = 0;
    if ( decoded_inputs_.min_max_enable ) {
      // These values don't quite mean the same thing in min_max. new_ram_lo
      //   feeds into the kOutAluLo case below and new_ram_hi feeds into kOutAluHi.
      // In minmax these cases are meant to produce:
      //  kOutAluLo : the post inc/dec value from min max which is in state_lo
      //  kOutAluHi : the selected index which is in state_hi
      new_ram_lo = state_lo;
      new_ram_hi = state_hi;
    }
    else if (1 != decoded_inputs_.width) {
      if (pred_lo1_off && pred_lo2_off) {
        // if both predicated off original ram contents used
        new_ram_lo = data_in->get_word(decoded_inputs_.subword*decoded_inputs_.subword_width, decoded_inputs_.width);
      }
      else {
        new_ram_lo = data_out->get_word(decoded_inputs_.subword*decoded_inputs_.subword_width, decoded_inputs_.width);
      }
      if (pred_hi1_off && pred_hi2_off) {
        // if both predicated off original ram contents used
        new_ram_hi = data_in->get_word(decoded_inputs_.subword*decoded_inputs_.subword_width+decoded_inputs_.width, decoded_inputs_.width);
      }
      else {
        if (!decoded_inputs_.dbl) {
          new_ram_hi = state_hi;
        } else {
          new_ram_hi = data_out->get_word(decoded_inputs_.subword*decoded_inputs_.subword_width+decoded_inputs_.width, decoded_inputs_.width);
        }
      }
    }
    else {
      new_ram_hi = 0;
      new_ram_lo = state_lo;
      // 1-Bit ops can only give a single bit output.
      RMT_ASSERT(0 == new_ram_lo || 1 == new_ram_lo);
    }

    bool div_or_mod_op = false; // for logging
    uint32_t out_alu_action_out[kStatefulAluOutputAlus]; // for logging
    const char* log_lmatch_adr[4]{"","","",""};          // for logging

    // Set action_out and data_out based on predication using cmp results
    // Note that kOutMemHi/Lo return the original RAM word.
    // Note that kOutAluHi/Lo return the modified RAM word.
    // Special case for kOutAluHi in single-width mode???
    bool pred_output_on[kStatefulAluCmpAlus] = {false};
    int output_alu_op[kStatefulAluCmpAlus];
    int cmp_output[kStatefulAluCmpAlus];
    int selected_cmp_output;
    // work out all the masked outputs as JBay could use any to form action out
    for (int out_alu=0;out_alu<kStatefulAluOutputAlus;++out_alu) {
      cmp_output[out_alu] = cmp_alu_output & chip_salu_.salu_output_cmpfn(out_alu,decoded_inputs_.instr_addr);
    }
    for (int out_alu=0;out_alu<kStatefulAluOutputAlus;++out_alu) {
      pred_output_on[out_alu] = ( cmp_output[out_alu] != 0 );
      output_alu_op[out_alu] = chip_salu_.salu_output_asrc(out_alu,decoded_inputs_.instr_addr);
      out_op_[out_alu] = op_make(kOpTypeOut, output_alu_op[out_alu]);

      if (!MauStatefulAlu::kRelaxSaluPredRsiCheck) {
        // Typically only do this check in DV_MODE
        // check output alu is not predicated on for pbus run stateful instruction
        RMT_ASSERT( ! ( decoded_inputs_.is_run_stateful && pred_output_on[out_alu] ));
      }

      uint32_t action_out_32b=0;
      bool use_bottom_64b = out_alu==0 || out_alu==1;
      uint32_t lmatch_adr = 0;
      switch (output_alu_op[out_alu]) {
        case MauChipStatefulAlu::kOutMemHi:
          if (use_bottom_64b) {
            action_out_32b = pred_output_on[out_alu] ? decoded_inputs_.ram_hi & 0xFFFFFFFF: 0;
          }
          else {
            RMT_ASSERT( (!pred_output_on[out_alu]) || (decoded_inputs_.dbl && decoded_inputs_.width==64));
            action_out_32b = pred_output_on[out_alu] ? data_in->get_word(64+32,32) : 0;
          }
          break;
        case MauChipStatefulAlu::kOutMemLo:
          if (use_bottom_64b) {
            action_out_32b = pred_output_on[out_alu] ? decoded_inputs_.ram_lo & 0xFFFFFFFF: 0;
          }
          else {
            RMT_ASSERT( (!pred_output_on[out_alu]) || (decoded_inputs_.width==64));
            action_out_32b = pred_output_on[out_alu] ? data_in->get_word(32,32) : 0;
          }
          break;
        case MauChipStatefulAlu::kOutPhvHi:
          if (use_bottom_64b) {
            action_out_32b = pred_output_on[out_alu] ? decoded_inputs_.phv_hi & 0xFFFFFFFF : 0;
          }
          else {
            RMT_ASSERT( (!pred_output_on[out_alu]) || (decoded_inputs_.dbl && decoded_inputs_.width==64));
            action_out_32b = pred_output_on[out_alu] ? phv.get_word(64+32,32) : 0;
          }
          break;
        case MauChipStatefulAlu::kOutPhvLo:
          if (use_bottom_64b) {
            action_out_32b = pred_output_on[out_alu] ? decoded_inputs_.phv_lo & 0xFFFFFFFF : 0;
          }
          else {
            RMT_ASSERT( (!pred_output_on[out_alu]) || (decoded_inputs_.width==64));
            action_out_32b = pred_output_on[out_alu] ? phv.get_word(32,32) : 0;
          }
          break;
        case MauChipStatefulAlu::kOutAluHi:
          if (use_bottom_64b) {
            action_out_32b = pred_output_on[out_alu] ? new_ram_hi : 0;
          }
          else {
            // TODO is an assert like those above needed?
            action_out_32b = pred_output_on[out_alu] ? (new_ram_hi >> 32) : 0; // flyover bits
          }
          break;
        case MauChipStatefulAlu::kOutAluLo:
          if (use_bottom_64b) {
            action_out_32b = pred_output_on[out_alu] ? new_ram_lo : 0;
          }
          else {
            // TODO is an assert like those above needed?
            action_out_32b = pred_output_on[out_alu] ? (new_ram_lo >> 32) : 0; // flyover bits
          }
          break;
        case MauChipStatefulAlu::kOutCmp:
          // In tofino there is only one cmp_output, but in JBay the cmp_output
          //   selected is chosen by a register field
          selected_cmp_output = cmp_output[ chip_salu_.which_cmp_output( stateful_ctl_ ) ];
          action_out_32b = ((selected_cmp_output!=0) << stateful_ctl_.salu_output_pred_comb_shift()) |
              ( (stateful_ctl_.salu_output_pred_shift() == 7) ? 0  :
                ( selected_cmp_output << (4* (1 + stateful_ctl_.salu_output_pred_shift()))));
          break;
        case MauChipStatefulAlu::kOutSAddr: // JBay only
          if ( chip_salu_.get_lmatch_adr_bit_enable(out_alu,decoded_inputs_.instr_addr ) ) {
            RMT_ASSERT( decoded_inputs_.real_width != 1 );   // can't be 1 bit wide
            RMT_ASSERT( decoded_inputs_.real_width != 128 ); // can't be 128 bits wide
            RMT_ASSERT( decoded_inputs_.dbl );               // must be double
            // XXX: use salu_mathtable[0]<<15:0>> for predication for all ALUs in this case
            //  (everything else uses salu_output_cmpfn to calculate per ALU cmp_output[], see above)
            bool single_bit = (0 != (cmp_alu_output & math_tbl_.salu_mathtable(0)));
            log_lmatch_adr[out_alu] = single_bit ? " lmatch_adr=1" : " lmatch_adr=0";
            if ( single_bit ) {
              lmatch_adr = 1 << com_reg_.salu_datasize(decoded_inputs_.instr_addr);
            }
          }
          action_out_32b = pred_output_on[out_alu] ? chip_salu_.get_out_addr(stateful_ctl_,addr) | lmatch_adr : 0;
          break;
        case MauChipStatefulAlu::kOutDiv: // JBay only
          action_out_32b = pred_output_on[out_alu] ? chip_salu_.get_divider_output(com_reg_,decoded_inputs_.instr_addr,alu2_hi_as,alu2_hi_bs) : 0;
          div_or_mod_op=true;
          break;
        case MauChipStatefulAlu::kOutMod: // JBay only
          action_out_32b = pred_output_on[out_alu] ? chip_salu_.get_modulus_output(com_reg_,decoded_inputs_.instr_addr,alu2_hi_as,alu2_hi_bs) : 0;
          div_or_mod_op=true;
          break;
        case MauChipStatefulAlu::kPreIncDec: // JBay only
          action_out_32b = pred_output_on[out_alu] ? minmax_pre_inc_dec_value : 0;
          break;
        default:
          RMT_LOG_WARN("Invalid output op %d\n",-output_alu_op[out_alu]);
          break;
      }
      // set the correct 32b word in the action output depending on the alu number
      int which_action_word;
      switch (out_alu) {
        case 0: which_action_word = 0; break;
        case 1: which_action_word = 2; break;
        case 2: which_action_word = 1; break;
        case 3: which_action_word = 3; break;
      }
      action_out->set32(which_action_word,action_out_32b);
      out_alu_action_out[out_alu]=action_out_32b; // for logging
    }

    /* ---- Following values are saved for SALU P4 level Logging ----- */
    p4_log_op_width_ = decoded_inputs_.subword_width; // salu operation width = 1b, 8b, 16b, 32b, 64b
    // Save entry index (ram index shifted + subword index), input ram value and input phv to salu
    switch (decoded_inputs_.subword_width) {
      case  1:    p4_log_index_ = addr;     break;
      case  8:    p4_log_index_ = addr>>3;  break;
      case  16:   p4_log_index_ = addr>>4;  break;
      case  32:   p4_log_index_ = addr>>5;  break;
      case  64:   p4_log_index_ = addr>>6;  break;
      case  128:  p4_log_index_ = addr>>7;  break;
    }
    p4_log_index_ &= 0x7FFFF;  // 19 bit mask

    p4_log_register_lo_ = decoded_inputs_.ram_lo;
    p4_log_register_hi_ = decoded_inputs_.ram_hi;
    p4_log_phv_lo_ = decoded_inputs_.phv_lo;
    p4_log_phv_hi_ = decoded_inputs_.phv_hi;
    // Save input fed to comparator.
    p4_log_cond_input_ram_hi_ = decoded_inputs_.cmp_ram_hi;
    p4_log_cond_input_ram_lo_ = decoded_inputs_.cmp_ram_lo;
    p4_log_cond_input_phv_hi_ = decoded_inputs_.phv_hi;
    p4_log_cond_input_phv_lo_ = decoded_inputs_.phv_lo;
    // Save result of condition_hi/lo. // TODO: JBay has 4 cmp alus
    //  if you do need to use masks like this to recover the other
    //  bits then bit2 is 0xF0F0F0F0 and bit3 is 0xFF00FF00
    p4_log_cond_output_lo_ = (cmp_alu_output & 0xAAAAAAAA)!=0;
    p4_log_cond_output_hi_ = (cmp_alu_output & 0xCCCCCCCC)!=0;
    // Save update_1/2 predicate and update_1/2 result
    p4_log_update_lo_1_predicate_ = (decoded_inputs_.width==1) ? true : !pred_lo1_off;
    p4_log_update_lo_1_result_ = state_lo1;
    p4_log_update_lo_2_predicate_ = !pred_lo2_off;
    p4_log_update_lo_2_result_ = state_lo2;
    p4_log_update_hi_1_predicate_ = !pred_hi1_off;
    p4_log_update_hi_1_result_ = state_hi1;
    p4_log_update_hi_2_predicate_ = !pred_hi2_off;
    p4_log_update_hi_2_result_ = state_hi2;
    // Save modified ram value
    p4_log_new_ram_hi_= new_ram_hi; // TODO: change? In JBay minmax, this is the value of the state_hi alu output
    p4_log_new_ram_lo_= new_ram_lo; // TODO: change? In JBay minmax, this is the value of the state_lo alu output
    // Save ALU predicate condition result and value. // TODO: JBay has 4 output ALUs
    p4_log_output_predicate_ = pred_output_on[0];
    // TODO: In JBay action_out can be 128b, does this need changing?
    p4_log_output_result_ = action_out->get_word(0,32); // For now not p4 logging source of
                                         // ALU output -- output_alu_op
    /* ---- P4 level logging end ----- */


    last_data_in.copy_from( *data_in );
    last_data_out.copy_from( *data_out );
    last_ram_lo = decoded_inputs_.ram_lo;
    last_ram_hi = decoded_inputs_.ram_hi;

    if (div_or_mod_op) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),
              "StatefulALU div_in_a==0x%016" PRIx64 " div_in_b=0x%016" PRIx64 "\n",alu2_hi_as,alu2_hi_bs);
    }
    for (int out_alu=0;out_alu<kStatefulAluOutputAlus;++out_alu) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),
              "StatefulALU out_op%s=%s cmp_alu_out=%x cmp_fn=%x %s%s"
              " action_out=0x%08x d_out=0x%016" PRIx64 "%016" PRIx64 "\n",
              (kStatefulAluOutputAlus==1)?"": std::to_string(out_alu).c_str(),
              out_op_str(output_alu_op[out_alu]),cmp_alu_output, chip_salu_.salu_output_cmpfn(out_alu,decoded_inputs_.instr_addr),
              pred_output_on[out_alu] ? "ON":"OFF",
              log_lmatch_adr[out_alu],
              out_alu_action_out[out_alu],
              data_out->get_word(64), data_out->get_word(0));

    }

    // P4 log that StatefulALU executed
    const char *dir_str = ingress ? "Ingress" : "Egress";
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"%s : Running StatefulALU %d with instruction %d  on index %d\n",
                       dir_str, alu_index_, decoded_inputs_.instr_addr, Address::meter_addr_get_index(addr));
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"RAM INPUT: 0x%016" PRIx64 " 0x%016" PRIx64 "\n",
                    data_in->get_word(64), data_in->get_word(0));

    // P4 log CONDITION input information
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"COND INPUT :  PHV Lo/RAM Lo %#" PRIx64 "/%#" PRIx64 "  PHV Hi/RAM Hi %#" PRIx64 "/%#" PRIx64 "\n",
            decoded_inputs_.phv_lo, decoded_inputs_.cmp_ram_lo,
            decoded_inputs_.phv_hi, decoded_inputs_.cmp_ram_hi);
    if (kStatefulAluCmpAlus == 2) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"COND OUTPUT: Condition Lo (%s) is %s  Condition Hi (%s) is %s\n",
              op_str(cmp_ops_[0]), (cmp_alu_out[0] ?"true" :"false"),
              op_str(cmp_ops_[1]), (cmp_alu_out[1] ?"true" :"false"));
    }
    else {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"COND OUTPUT: %s\n",
              std::bitset<kStatefulAluCmpAlus>( cmp ).to_string().c_str());
    }
    // P4 log STATE UPDATE input information
    if (1 == decoded_inputs_.width) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"UPDATE INPUT: RAM 0x%016" PRIx64 " 0x%016" PRIx64 "\n",
                      data_in->get_word(64), data_in->get_word(0));
    } else if ((math_ram_hi == decoded_inputs_.ram_hi) && (math_ram_lo == decoded_inputs_.ram_lo)) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"UPDATE INPUT: PHV Lo/RAM Lo %#" PRIx64 "/%#" PRIx64 "  PHV Hi/RAM Hi %#" PRIx64 "/%#" PRIx64 "\n",
                      decoded_inputs_.phv_lo, decoded_inputs_.ram_lo, decoded_inputs_.phv_hi, decoded_inputs_.ram_hi);
    } else {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"UPDATE INPUT: PHV Lo/RAM Lo/MathRAM Lo %" PRIx64 "/%" PRIx64 "/%#x  "
                      "PHV Hi/RAM Hi/MathRAM Hi %" PRIx64 "/%" PRIx64 "/%#x\n",
                      decoded_inputs_.phv_lo, decoded_inputs_.ram_lo, math_ram_lo, decoded_inputs_.phv_hi, decoded_inputs_.ram_hi, math_ram_hi);
    }

    // P4 log STATE UPDATE output information
    if (1 == decoded_inputs_.width) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"UPDATE_lo_1: Predicated on always  op %s  value "
                      "0x%016" PRIx64 " 0x%016" PRIx64 "\n",
                      op_str(state_ops_[1]), data_out->get_word(64), data_out->get_word(0));
    } else {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"UPDATE_lo_1: Predicated %s  op %s  value %" PRIx64 "\n",
                      (pred_lo1_off ?"off" :"on"), op_str(state_ops_[1]), state_lo1_raw);
      if (math_unit_executed_) { // Get further info if MathUnit executed
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"UPDATE_lo_2: MathUnit: Input=%09" PRIx64 " InShift=%d "
                        "TableData=%02x OutShift=%d Output=%08x\n",
                        math_input_data_, math_in_shift_, math_table_data_,
                        math_out_shift_, math_output_data_);
      }
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"UPDATE_lo_2: Predicated %s  op %s  value %" PRIx64 "\n",
                      (pred_lo2_off ?"off" :"on"), op_str(state_ops_[0]), state_lo2_raw);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"UPDATE_hi_1: Predicated %s  op %s  value %" PRIx64 "\n",
                      (pred_hi1_off ?"off" :"on"), op_str(state_ops_[3]), state_hi1_raw);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"UPDATE_hi_2: Predicated %s  op %s  value %" PRIx64 "\n",
                      (pred_hi2_off ?"off" :"on"), op_str(state_ops_[2]), state_hi2_raw);
    }

    // P4 log RAM output and ACTION BUS output information
    if (ram_changed) {
      if (! decoded_inputs_.min_max_enable )
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"RAM OUTPUT: RAM Lo %#" PRIx64 "  RAM Hi %#" PRIx64 "\n", new_ram_lo, new_ram_hi);
      else
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"MINMAX STATE OUT: Lo %" PRIx64 "  Hi %" PRIx64 "\n", new_ram_lo, new_ram_hi);
    }
    // TODO: JBay has 4 Output ALUs
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu),"ACTION OUTPUT: Predicated %s  op %s  value 0x%016" PRIx64 "_%016" PRIx64 "\n",
	    (pred_output_on[0] ?"on" :"off"), op_str(out_op_[0]),
	    action_out->get_word(64), action_out->get_word(0));
  }


  bool MauStatefulAlu::run_cmp(int which_alu) {

    RMT_ASSERT( which_alu>=0 && which_alu<kStatefulAluCmpAlus);

    // Cache results - when the whole MAU is running the cmp_alu's run early, which we JBay needs to
    //    work out the TMatch bus. In the standalone DV environment this will run when called from
    //    calculate_output as before.
    if (cmp_alu_out_[which_alu].valid) {
      return cmp_alu_out_[which_alu].cmp_out;
    }
    static constexpr int log_msg_size = 1024;
    model_core::LogBuffer log_buf(log_msg_size);
    int op = cmp_reg_.salu_cmp_opcode(decoded_inputs_.instr_addr, which_alu);
    bool signed_op = cmp_op_is_signed(op);
    bool out_of_range = false;


    log_buf.Append("Cmp-ALU-%s", cmp_name_[which_alu] );

    // Only JBay will return mask_asrc or bsrc true
    bool mask_asrc = chip_salu_.get_cmp_asrc_mask_enable(cmp_reg_,decoded_inputs_.instr_addr,which_alu);
    bool mask_bsrc = chip_salu_.get_cmp_bsrc_mask_enable(cmp_reg_,decoded_inputs_.instr_addr,which_alu);
    uint64_t cmp_mask = 0;
    if (mask_asrc || mask_bsrc) {
      bool mask_from_reg_file = chip_salu_.get_cmp_mask_input(cmp_reg_,decoded_inputs_.instr_addr,which_alu);
      // Note: top 32 bits are always set as these bits do not really have the mask applied to them
      cmp_mask = get_cmp_constant(mask_from_reg_file, decoded_inputs_.instr_addr, decoded_inputs_.width, which_alu,decoded_inputs_.mask, log_buf,"mask")
          | UINT64_C( 0xFFFFFFFF00000000) ;
    }

    uint64_t LHS = 0;
    uint64_t RHS = 0;
    uint64_t a = 0, A=0;
    bool a_is_neg = cmp_reg_.salu_cmp_asrc_sign(decoded_inputs_.instr_addr, which_alu);
    bool a_enabled = cmp_reg_.salu_cmp_asrc_enable(decoded_inputs_.instr_addr, which_alu);
    bool a_hilo = cmp_reg_.salu_cmp_asrc_input(decoded_inputs_.instr_addr, which_alu);
    a = a_enabled ? (a_hilo ? decoded_inputs_.cmp_ram_hi : decoded_inputs_.cmp_ram_lo) : 0;
    a &= decoded_inputs_.mask;
    if (mask_asrc) {
      a &= cmp_mask;
    }
    uint64_t a_for_tmatch = a;
    a &= 0xFFFFFFFF; // now truncate to 32 bits
    A = a;
    if (signed_op) { // Sign extend to 35 bits
      A = sign_ext64(a, decoded_inputs_.width);
      a_for_tmatch = sign_ext64(a_for_tmatch, decoded_inputs_.width);
      a = sign_ext64(a, decoded_inputs_.width) & UINT64_C(0x7FFFFFFFF);
    }
    if (a_is_neg) { // If negative, take 2's compliment of the value.
      RHS += A;
      A = ~A + 1;
      a = (~a + 1) & UINT64_C(0x7FFFFFFFF);
    } else {
      LHS += A;
    }

    log_buf.Append("A(%s %s%s): %#x", a_enabled ? "Enabled":"Disabled",
            a_hilo ? "RamHi":"RamLo", a_is_neg ? " Negated":"", (uint32_t)a);

    uint64_t b = 0, B = 0;
    bool b_is_neg = cmp_reg_.salu_cmp_bsrc_sign(decoded_inputs_.instr_addr, which_alu);
    bool b_enabled = cmp_reg_.salu_cmp_bsrc_enable(decoded_inputs_.instr_addr, which_alu);
    int  b_src = cmp_reg_.salu_cmp_bsrc_input(decoded_inputs_.instr_addr, which_alu);
    if ( b_enabled ) {
      switch (b_src) {
        case 0:
          b = decoded_inputs_.phv_lo;
          break;
        case 1:
          b = decoded_inputs_.phv_hi;
          break;
        case 2:
          RMT_ASSERT(MauDefs::kStatefulAluUsesRandomNumber); // JBay can use RNG
          RMT_ASSERT( random_number_.valid );
          b = 0xFFFFFFFF & random_number_.value;
          break;
        default:
          RMT_ASSERT(0);
          break;
      }
    }
    else {
      b = 0;
    }
    b &= decoded_inputs_.mask;
    if (mask_bsrc) {
      b &= cmp_mask;
    }
    uint64_t b_for_tmatch = b;
    b &= 0xFFFFFFFF; // now truncate to 32 bits
    B = b;
    if (signed_op) { // Sign extend to 35 bits
      B = sign_ext64(b, decoded_inputs_.width);
      b_for_tmatch = sign_ext64(b_for_tmatch, decoded_inputs_.width);
      b = sign_ext64(b, decoded_inputs_.width) & UINT64_C(0x7FFFFFFFF);
    }
    if (b_is_neg) { // If negative, take 2's compliment of the value.
      RHS += B;
      B = ~B + 1;
      b = (~b + 1) & UINT64_C(0x7FFFFFFFF);
    } else {
      LHS += B;
    }

    log_buf.Append(" B(%s %s%s): %#x", b_enabled ? "Enabled":"Disabled",
            b_src==2 ? "RNG" : b_src ? "PhvHi":"PhvLo", b_is_neg ? " Negated":"", (uint32_t)b);


    // Now we have a and b we can run the Ternary Match
    if (which_alu<kStatefulAluTMatchAlus) {
      chip_salu_.run_tmatch(cmp_reg_,which_alu, decoded_inputs_.instr_addr,
                            a_for_tmatch,b_for_tmatch,
                            decoded_inputs_.mask,
                            &cmp_alu_out_[which_alu].tmatch_match,
                            &cmp_alu_out_[which_alu].tmatch_learn );
      log_buf.Append(" (TMatch m=%d l=%d)", cmp_alu_out_[which_alu].tmatch_match,
              cmp_alu_out_[which_alu].tmatch_learn);
    }

    bool use_reg_file  = cmp_reg_.salu_cmp_regfile_const(decoded_inputs_.instr_addr, which_alu);
    uint64_t C = get_cmp_constant(use_reg_file, decoded_inputs_.instr_addr, decoded_inputs_.width, which_alu,decoded_inputs_.mask, log_buf,"");
    uint64_t c = C & UINT64_C(0x7FFFFFFFF);

    if ((int64_t)C < 0) {
      RHS += -1 * (int64_t)C;
    } else {
      LHS += C;
      if (cmp_op_is_unsigned(op)) {
          // The P4 author expresses the condition as A+B op C and the compiler
          // converts it to A+B+C op 0 by programming the two's compliment of C.
          // If C is a positive number then the P4 author wanted to use a
          // negative value for C which is not legal for unsigned operations.
          out_of_range = true;
      }
    }

    int     width = decoded_inputs_.width;
    uint64_t mask = decoded_inputs_.mask;
    // force width and mask to be less than 32, as this path is only 32 bits wide
    width = width>32 ? 32 :  width;
    mask &= UINT64_C(0xFFFFFFFF);


    int32_t UUS_LHS = A+B;
    int32_t UUS_RHS = C * -1;
    // UUS operations only work if the difference between A and B is within the
    // range of positive signed numbers for the instruction width.  UUS operations
    // also only work when the right hand side is positive.
    out_of_range = out_of_range || (UUS_LHS & ~(mask >> 1)) || (UUS_RHS < 0);
    // New UUS check for JBay Extra MSbits: Check C is within the range of 32 bit signed number and not positive
    out_of_range = out_of_range || (C>0) || (static_cast<int64_t>(C) < static_cast<int64_t>(std::numeric_limits<int32_t>::min()));

    // c is always sign extended, but for unsigned operations
    //  the resulting number must be treated as unsigned
    if (!signed_op)  c &= UINT64_C(0x7FFFFFFFF);
    uint64_t x = (a+b+c) & UINT64_C(0x7FFFFFFFF);
    // Use bit 31, 15, or 7 depending on the instruction width.  In case of a
    // 1 bit instruction width always use a value of 0 for the sign bit, this
    // is to match up with what the RTL does in this error case.
    int sign_bit = width != 1 ? (x >> (width-1)) & 1 : 0;
    int bit34 = (x >> 34) & 1; // Overflow 2

    // UUS instructions only check the lower 32/16/8 bits of result to determine
    // if the value is zero or not.  All other instructions will use the full
    // 35 bit value.  Note that 1-bit UUS compares will always see a zero.
    if (op == kCmpAluOpGUUS ||
        op == kCmpAluOpGEUUS ||
        op == kCmpAluOpLEUUS ||
        op == kCmpAluOpLUUS) {
      if (1 == width) {
        x = 0;
      } else {
        x = x & mask;
      }
    }

    log_buf.Append(" %" PRIi64 "+%" PRIi64 "+%" PRIi64
            " %s 0 (%#x, SignBit %d Overflow2 %d)",
            a, b, c, cmp_op_str(op), (uint32_t)x,
            sign_bit, bit34);
    cmp_ops_[which_alu] = op_make(kOpTypeCmp, op);

// TODO: remove T_RMT_ASSERT and replace with RMT_ASSERT when sure things are working
#define T_RMT_ASSERT(X) if (!( X )) RMT_LOG_ERROR(__FILE__  ":ERROR line %d ret(%d) != ret2(%d)\n",__LINE__,ret,ret2)

    bool ret = false, ret2 = false;
    switch (op) {
      case kCmpAluOpEq:
          ret = 0 == x;
          ret2 = LHS == RHS;
          T_RMT_ASSERT(ret == ret2);
          break;
      case kCmpAluOpNe:
          ret =  0 != x;
          ret2 =  LHS != RHS;
          T_RMT_ASSERT(ret == ret2);
          break;
      case kCmpAluOpDis: // Disabled, JBay only
          // TODO: assert if Tofino
          ret =  ret2 = false;
          T_RMT_ASSERT(ret == ret2);
          break;
      case kCmpAluOpGS:
          ret =  0 == bit34 && x != 0;
          ret2 = (int64_t)LHS > (int64_t)RHS;
          T_RMT_ASSERT(ret == ret2);
          break;
      case kCmpAluOpLES:
          ret =  0 == x || 1 == bit34;
          ret2 = (int64_t)LHS <= (int64_t)RHS;
          T_RMT_ASSERT(ret == ret2);
          break;
      case kCmpAluOpGES:
          ret =  0 == x || 0 == bit34;
          ret2 = (int64_t)LHS >= (int64_t)RHS;
          T_RMT_ASSERT(ret == ret2);
          break;
      case kCmpAluOpLS:
          ret =  1 == bit34 && x != 0;
          ret2 = (int64_t)LHS < (int64_t)RHS;
          T_RMT_ASSERT(ret == ret2);
          break;
      case kCmpAluOpGU:
          ret =  0 == bit34 && x != 0;
          ret2 = LHS > RHS;
          T_RMT_ASSERT( out_of_range || (ret == ret2));
          break;
      case kCmpAluOpLEU:
          ret =  0 == x || 1 == bit34;
          ret2 = LHS <= RHS;
          T_RMT_ASSERT( out_of_range || (ret == ret2));
          break;
      case kCmpAluOpGEU:
          ret =  0 == x || 0 == bit34;
          ret2 = LHS >= RHS;
          T_RMT_ASSERT( out_of_range || (ret == ret2));
          break;
      case kCmpAluOpLU:
          ret =  1 == bit34 && x != 0;
          ret2 = LHS < RHS;
          T_RMT_ASSERT( out_of_range || (ret == ret2));
          break;
      case kCmpAluOpGUUS:
          ret =  0 == sign_bit && x != 0;
          ret2 = UUS_LHS > UUS_RHS;
          T_RMT_ASSERT( out_of_range || (width==1) || (ret == ret2));
          break;
      case kCmpAluOpLEUUS:
          ret =  0 == x || 1 == sign_bit;
          ret2 = UUS_LHS <= UUS_RHS;
          T_RMT_ASSERT( out_of_range || (width==1) || (ret == ret2));
          break;
      case kCmpAluOpGEUUS:
          ret =  0 == x || 0 == sign_bit;
          ret2 = UUS_LHS >= UUS_RHS;
          T_RMT_ASSERT( out_of_range || (width==1) || (ret == ret2));
          break;
      case kCmpAluOpLUUS:
          ret =  1 == sign_bit && x != 0;
          ret2 = UUS_LHS < UUS_RHS;
          T_RMT_ASSERT( out_of_range || (width==1) || (ret == ret2));
          break;
      default:
        RMT_ASSERT(0);
    }
    log_buf.Append(" is %s", ret ? "true":"false");
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "%s\n", log_buf.GetBuf());

    cmp_alu_out_[which_alu].cmp_out = ret;
    cmp_alu_out_[which_alu].valid   = true;

    return cmp_alu_out_[which_alu].cmp_out;
  }

  uint64_t MauStatefulAlu::run_state_lo_1(uint64_t phv_hi, uint64_t phv_lo,
                                          uint64_t ram_hi, uint64_t ram_lo,
                                          int instr_addr) {
    return run_state(0, 0, phv_hi, phv_lo, ram_hi, ram_lo, 0, 0, instr_addr,nullptr,nullptr);
  }
  uint64_t MauStatefulAlu::run_state_lo_2(uint64_t phv_hi, uint64_t phv_lo,
                                          uint64_t ram_hi, uint64_t ram_lo,
                                          uint32_t math_ram_hi, uint32_t math_ram_lo,
                                          int instr_addr) {
    return run_state(0, 1, phv_hi, phv_lo, ram_hi, ram_lo, math_ram_hi, math_ram_lo, instr_addr,nullptr,nullptr);
  }
  uint64_t MauStatefulAlu::run_state_hi_1(uint64_t phv_hi, uint64_t phv_lo,
                                          uint64_t ram_hi, uint64_t ram_lo,
                                          int instr_addr) {
    return run_state(1, 0, phv_hi, phv_lo, ram_hi, ram_lo, 0, 0, instr_addr,nullptr,nullptr);
  }
  uint64_t MauStatefulAlu::run_state_hi_2(uint64_t phv_hi, uint64_t phv_lo,
                                          uint64_t ram_hi, uint64_t ram_lo,
                                          int instr_addr, int64_t *as_out, int64_t *bs_out) {
    return run_state(1, 1, phv_hi, phv_lo, ram_hi, ram_lo, 0, 0, instr_addr, as_out, bs_out);
  }

  uint64_t MauStatefulAlu::run_state(bool hilo, bool alu_1or2, uint64_t phv_hi,
                                     uint64_t phv_lo, uint64_t ram_hi, uint64_t ram_lo,
                                     uint32_t math_ram_hi, uint32_t math_ram_lo,
                                     int instr_addr, int64_t *as_out, int64_t *bs_out) {

    int alu_addr = 2*hilo + !alu_1or2;

    int     width = decoded_inputs_.width;
    uint64_t mask = decoded_inputs_.mask;
    // force width and mask to be less than 32, as this path is only 32 bits wide
    width = width>32 ? 32 :  width;
    mask &= UINT64_C(0xFFFFFFFF);

    // One bit operations are special cased outside this function.  They
    // cannot be handled here.
    RMT_ASSERT(1 != width);

    static constexpr int log_msg_size = 1024;
    model_core::LogBuffer log_buf(log_msg_size);
    log_buf.Append("S-ALU-%d-%s", alu_1or2 ? 2:1, hilo ? "Hi":"Lo");

    uint64_t c = 0;
    bool const_is_from_reg_file = state_reg_.salu_regfile_const(instr_addr, alu_addr);
    if (const_is_from_reg_file) {
      int reg_file_addr = 3 & state_reg_.salu_const_src(instr_addr, alu_addr);
      c = reg_file_.salu_const_regfile(reg_file_addr) & mask;
      c = sign_ext64(c, width);
    } else {
      c = state_reg_.salu_const_src(instr_addr, alu_addr);
      c = sign_ext64(c, 4);
    }

    uint64_t b = 0;
    int64_t bs = 0;
    // ALU-2-lo might be using the math unit rather than the PHV source
    log_buf.Append(" (%d,%d,%d)",alu_1or2 ,hilo , com_reg_.salu_alu2_lo_bsrc_math(instr_addr));
    if (!hilo && alu_1or2 && com_reg_.salu_alu2_lo_bsrc_math(instr_addr)) {
      b = run_math(phv_hi, phv_lo, math_ram_hi, math_ram_lo, instr_addr) & mask;
      bs = sign_ext64(b, width);
    } else {
      // separated out as register fields are different in JBay
      chip_salu_.get_b(state_reg_,hilo,alu_1or2,phv_hi, phv_lo, ram_hi, ram_lo,
                       c,decoded_inputs_.width,mask,instr_addr,alu_addr,
                       decoded_inputs_.is_run_stateful, &b,&bs );
    }
    if (bs_out) *bs_out = bs;

    uint64_t a = 0;
    int64_t as = 0;
    // separated out as register fields are different in JBay
    chip_salu_.get_a(state_reg_,alu_1or2,phv_hi, phv_lo, ram_hi, ram_lo,c,
                     decoded_inputs_.width,mask,instr_addr,alu_addr,
                     decoded_inputs_.is_run_stateful,&a,&as );
    if (as_out) *as_out = as;

    bool flyover_from_a = chip_salu_.get_flyover_src_sel(instr_addr,alu_addr);
    uint64_t flyover = (decoded_inputs_.width >= 64) ?
                         (flyover_from_a ? a : b ) & UINT64_C( 0xFFFFFFFF00000000 ) :
                         0;

    // remove the flyover and any other bits that might be set outside of the width
    a &= mask;
    b &= mask;

    int op = state_reg_.salu_op(instr_addr, alu_addr);
    bool arithmetic_or_logical = state_reg_.salu_arith(instr_addr, alu_addr);

    uint64_t ret = 0;
    int64_t s_ret = 0;
    int64_t s_max = ( 8 == width) ? 0x7F :
                    (16 == width) ? 0x7FFF : 0x7FFFFFFF;
    int64_t s_min = ( 8 == width) ? 0x80 :
                    (16 == width) ? 0x8000 : 0x80000000;
    uint64_t u_max = ( 8 == width) ? 0xFF :
                    (16 == width) ? 0xFFFF : 0xFFFFFFFF;
    s_min = sign_ext64(s_min, width);
    // for unsigned operations "a" and "b" must be masked in case they came from
    // c, which could be a negative signed extended constant which
    // needs to be treated as unsigned in unsigned operations.
    uint64_t am = mask & a;
    uint64_t bm = mask & b;
    if (arithmetic_or_logical) {
      uint32_t a32 = a, b32 = b;
      log_buf.Append(" 0x%08x %s 0x%08x", a32, ath_op_str(op), b32);
      state_ops_[alu_addr] = op_make(kOpTypeAth, op);
      switch (op) {
        case kSAddU:
          ret = am + bm;
          if (ret > u_max)
            ret = u_max;
          break;
        case kSAddS:
          s_ret = as + bs;
          if (s_ret < s_min)
            ret = s_min  & mask;
          else if (s_ret > s_max)
            ret = s_max;
          else
            ret = s_ret & mask;
          break;
        case kSSubU:
          ret = (bm > am) ? 0 : ((am - bm) & mask);
          break;
        case kSSubS:
          s_ret = as - bs;
          if (s_ret < s_min)
            ret = s_min & mask;
          else if (s_ret > s_max)
            ret = s_max;
          else
            ret = s_ret & mask;
          break;
        case kMinU:
          ret = am < bm ? am:bm;
          break;
        case kMinS:
          s_ret = as < bs ? as:bs;
          ret = s_ret & mask;
          break;
        case kMaxU:
          ret = am > bm ? am:bm;
          break;
        case kMaxS:
          s_ret = as > bs ? as:bs;
          ret = s_ret & mask;
          break;
        case kNop1_AddC:
          ret = chip_salu_.nop1_addc(hilo,alu_1or2,as,bs,decoded_inputs_.real_width,decoded_inputs_.dbl);
          break;
        case kNop2_SubC:
          ret = chip_salu_.nop2_subc(hilo,alu_1or2,as,bs,decoded_inputs_.real_width,decoded_inputs_.dbl);
          break;
        case kSSubRU:
          ret = (am > bm) ? 0 : ((bm - am) & mask);
          break;
        case kSSubRS:
          s_ret = bs - as;
          if (s_ret < s_min)
            ret = s_min & mask;
          else if (s_ret > s_max)
            ret = s_max;
          else
            ret = s_ret & mask;
          break;
        case kAdd:
          chip_salu_.add(hilo,alu_1or2,as,bs);
          ret = (as + bs) & mask;
          break;
        case kNop3:
          ret = 0;
          flyover = 0; // nop zeros flyover too
          break;
        case kSub:
          chip_salu_.sub(hilo,alu_1or2,as,bs);
          ret = (as - bs) & mask;
          break;
        case kSubR:
          ret = (bs - as) & mask;
          break;
        default:
          RMT_ASSERT(0);
      }
    } else {
      uint32_t a32 = a, b32 = b;
      log_buf.Append(" 0x%08x %s 0x%08x", a32, log_op_str(op), b32);
      state_ops_[alu_addr] = op_make(kOpTypeLog, op);
      switch (op) {
        case kSetZ:
          ret = 0;
          break;
        case kNor:
          ret = (~a & ~b) & mask;
          break;
        case kAndCA:
          ret = (~a & b) & mask;
          break;
        case kNotA:
          ret = ~a & mask;
          break;
        case kAndCB:
          ret = (a & ~b) & mask;
          break;
        case kNotB:
          ret = ~b & mask;
          break;
        case kXor:
          ret = (a ^ b) & mask;
          break;
        case kNand:
          ret = (~a | ~b) & mask;
          break;
        case kAnd:
          ret = (a & b) & mask;
          break;
        case kXnor:
          ret = ~(a ^ b) & mask;
          break;
        case kB:
          ret = b & mask;
          break;
        case kOrCA:
          ret = (~a | b) & mask;
          break;
        case kA:
          ret = a & mask;
          break;
        case kOrCB:
          ret = (a | ~b) & mask;
          break;
        case kOr:
          ret = (a | b) & mask;
          break;
        case kSetHi:
          ret = mask;
          break;
        default:
          RMT_ASSERT(0);
      }
    }
    log_buf.Append(" == 0x%08x", (uint32_t)(ret & 0xFFFFFFFF));
    if (flyover) log_buf.Append(" (flyover=0x%08x)", (uint32_t)((flyover>>32) & 0xFFFFFFFF));
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "%s\n", log_buf.GetBuf());
    return ret | flyover;
  }

  bool MauStatefulAlu::run_state_one_bit(int addr, int instr_addr,
                                         BitVector<kDataBusWidth> *data_in,
                                         BitVector<kDataBusWidth> *data_out) {
    // TODO: this test does not work on JBay because register field has changed,
    //  I could add a chip specific function to check this.
    // The ALU index is hardcoded to 1 because only alu-1-lo can run 1b ops.
    //bool a_is_mem = state_reg_.salu_asrc_memory(instr_addr, 1);
    // For 1b operations, the ALU must be setup such that A is from the RAM
    // word rather than a constant.  Why? Seems to be a hardware constraint
    // since these ALU operations don't really use the A/B sources, they just
    // modify the entire 128b RAM word.
    //RMT_ASSERT(a_is_mem);
    static constexpr int log_msg_size = 1024;
    model_core::LogBuffer log_buf(log_msg_size);
    log_buf.Append("S-ALU-1-lo 1 bit");

    data_out->copy_from(*data_in);

    // The bit to modify is indexed by the 7 lower bits of address.
    int bit_pos = addr & 0x7F;
    bool bit_val = 0;

    // Incase of the AT (Adjust Total) instructions, the top byte of the
    // 128-bit word contains a count of the number of 1 bits.  This will
    // need to be updated if the count changes.
    uint8_t total = data_out->get_byte(15);
    total &= 0x7F;

    // The ALU index is hardcoded to 1 because only alu-1-lo can run 1b ops.
    int op = state_reg_.salu_op(instr_addr, 1);
    log_buf.Append(" bit %d", bit_pos);
    state_ops_[1] = op_make(kOpType1Bit, op);
    switch (op) {
      case kSetBit:
        log_buf.Append(" SetBit");
        bit_val = data_out->get_bit(bit_pos);
        data_out->set_bit(1, bit_pos);
        break;
      case kSetBitC:
        log_buf.Append(" SetBitC");
        bit_val = !data_out->get_bit(bit_pos);
        data_out->set_bit(1, bit_pos);
        break;
      case kClrBit:
        log_buf.Append(" ClrBit");
        bit_val = data_out->get_bit(bit_pos);
        data_out->set_bit(0, bit_pos);
        break;
      case kClrBitC:
        log_buf.Append(" ClrBitC");
        bit_val = !data_out->get_bit(bit_pos);
        data_out->set_bit(0, bit_pos);
        break;
      case kRdBit:
        log_buf.Append(" RdBit");
        bit_val = data_out->get_bit(bit_pos);
        break;
      case kRdBitC:
        log_buf.Append(" RdBitC");
        bit_val = !data_out->get_bit(bit_pos);
        break;
      case kSetBitAt:
        log_buf.Append(" SetBitAt");
	if (total > 120) total = 120;
	bit_val = data_out->get_bit(bit_pos);
	if (check_bit_pos(bit_pos)) {
	  data_out->set_bit(1, bit_pos);
	  if (total >= 120) {
	    total = 120;
	  } else {
	    total += bit_val ? 0:1;
	  }
	}
        break;
      case kSetBitAtC:
        log_buf.Append(" SetBitAtC");
	if (total > 120) total = 120;
	bit_val = !data_out->get_bit(bit_pos);
	if (check_bit_pos(bit_pos)) {
	  data_out->set_bit(1, bit_pos);
	  if (total >= 120) {
	    total = 120;
	  } else {
	    total += !bit_val ? 0:1;
	  }
	}
        break;
      case kClrBitAt:
        log_buf.Append(" ClrBitAt");
	if (total > 120) total = 120;
	bit_val = data_out->get_bit(bit_pos);
	if (check_bit_pos(bit_pos)) {
	  data_out->set_bit(0, bit_pos);
	  if (0 != total) {
	    total -= bit_val ? 1:0;
	  }
	}
        break;
      case kClrBitAtC:
        log_buf.Append(" ClrBitAtC");
	if (total > 120) total = 120;
	bit_val = !data_out->get_bit(bit_pos);
	if (check_bit_pos(bit_pos)) {
	  data_out->set_bit(0, bit_pos);
	  if (0 != total) {
	    total -= !bit_val ? 1:0;
	  }
	}
        break;
      default:
        RMT_ASSERT(0);
    }
    // Write the total back into the top byte of the ram word if it was
    // adjusted.
    if (kSetBitAt == op || kSetBitAtC == op || kClrBitAt == op || kClrBitAtC == op) {
      // We don't want to check the total.  If the stateful table is used as a
      // stateful selection table (hence we are in 1 bit mode doing AT
      // instructions) then in resilient mode it is likely that the total does
      // not reflect the current number of set bits in the word, rather it
      // represents the number of valid members in the selection group.  The
      // state of those members could be up or down and if there is even a
      // single down member in the group the total will not represent the
      // number of set bits.  Instead we can verify that the number of set bits
      // does not exceed the total.
      uint8_t old_top_byte = data_out->get_byte(15);
      if ( check_total_correct_ ) {
        data_out->set_byte(0, 15);
        RMT_ASSERT(data_out->popcount() <= total);
      }
      total &= 0x7f; // counter is only 7 bits
      log_buf.Append(" NewTot %d",total);
      uint8_t new_top_byte = total;
      // Put the top bit back, so model matches when misprogrammed (see XXX)
      if (is_jbay_or_later()) {
        new_top_byte |= ( old_top_byte & 0x80 );
      }
      data_out->set_byte(new_top_byte, 15);
    }

    log_buf.Append(" bit_val == %d",bit_val);
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "%s\n", log_buf.GetBuf());

    return bit_val;
  }

  uint32_t MauStatefulAlu::run_math(uint64_t phv_hi, uint64_t phv_lo,
                                    uint32_t math_ram_hi, uint32_t math_ram_lo,
                                    int instr_addr) {
    // These inputs are all 32bits, even in JBay
    phv_hi &= 0xFFFFFFFF;
    phv_lo &= 0xFFFFFFFF;

    int input_src = com_reg_.salu_alu2_lo_math_src(instr_addr);
    uint32_t input;
    switch (input_src) {
      case 0:
        input = phv_lo;
        break;
      case 1:
        input = phv_hi;
        break;
      case 2:
        input = math_ram_lo;
        break;
      case 3:
        input = math_ram_hi;
        break;
      default:
        RMT_ASSERT(0);
        return 0;
    }

    //
    // Step 1, Compute the shift value for the math table data.
    //

    // 1a) Find leading ones position.
    int l1p = 0;
    if (!input) {
    } else {
      l1p =  31 - __builtin_clz (input);
    }
    RMT_ASSERT(l1p <= 31 && l1p >= 0);

    // 1b) Apply the salu_mathunit_exponent_shift configuration. Note that the
    //     leading 1s position may be rounded up here and the rounded value
    //     will be used in the math table lookup (in step 2).
    int l1p_shifted = 0;
    switch (math_ctl_.salu_mathunit_exponent_shift()) {
      case 0:
        l1p_shifted = l1p;
        RMT_ASSERT(l1p_shifted <= 31 && l1p_shifted >= 0);
        break;
      case 1:
        l1p_shifted = l1p << 1;
        RMT_ASSERT(l1p_shifted <= 62 && l1p_shifted >= 0);
        break;
      case 2:
        // Round up to the next even value for leading 1s postion.
        l1p += (l1p & 1) ? 1 : 0;
        l1p_shifted = l1p >> 1;
        RMT_ASSERT(l1p_shifted <= 16 && l1p_shifted >= 0);
        break;
      default:
        RMT_ASSERT(0);
        return 0;
    }

    // 1c) Apply the salu_mathunit_exponent_invert configuration.
    int l1p_shifted_inverted = 0;
    if (math_ctl_.salu_mathunit_exponent_invert()) {
      l1p_shifted_inverted = l1p_shifted * -1;
    } else {
      l1p_shifted_inverted = l1p_shifted;
    }
    RMT_ASSERT(l1p_shifted_inverted <= 62 && l1p_shifted_inverted >= -62);

    // 1d) Apply the salu_mathunit_output_scale configuration.  A signed 6-bit
    //     value added to the modified leading 1s postion.
    //     Note that at this point l1p_shifted_inverted has been sign extended
    //     from 6 bits to 32 bits.
    int scale_raw = math_ctl_.salu_mathunit_output_scale();
    // Sign extend the signed 6 bit value in the config register to a 32 bit
    // signed integer.
    int scale = (scale_raw & 0x20) ? 0xFFFFFFC0 | scale_raw : scale_raw;
    RMT_ASSERT(scale <= 31 && scale >= -32);
    // Both l1p_shift_inverted and scale are signed ints so simply add them
    // together.
    int final_shift = l1p_shifted_inverted + scale;
    RMT_ASSERT(final_shift <= (62+31) && final_shift >= (-62-32));

    // The range of the final shift is [-32, +31], ensure it is in that range.
    //  copy the range reducing behaviour of the hardware
    if (final_shift < -32) {
      final_shift = -32;
    } else if (final_shift > 31) {
      final_shift = 32;
    }


    //
    // Step 2, Look up the data from the math table.
    //
    uint64_t input_shifted = (static_cast<uint64_t>(input) << 3) & UINT64_C( 0x7FFFFFFFF );
    int tbl_index = (input_shifted >> l1p) & 0xF;
    int tbl_index_word = tbl_index >> 2;
    int tbl_index_byte = tbl_index & 0x3;
    uint32_t tbl_data = math_tbl_.salu_mathtable(tbl_index_word);
    tbl_data = (tbl_data >> (8*tbl_index_byte)) & 0xFF;

    //
    // Step 3, Apply the shift value to the extracted table data
    //

    uint32_t output = 0;
    if (final_shift>-32 && final_shift<32) { // output zero in all other cases
      if (final_shift < 0) {
	// Negative value, right shift
	int right_shift = -1 * final_shift;
	output = tbl_data >> right_shift;
      } else {
	// Positive value, left shift
	output = tbl_data << final_shift;
      }
    }

    // Stash key vals
    math_input_data_ = input_shifted;
    math_in_shift_ = l1p;
    math_table_data_ = tbl_data;
    math_out_shift_ = final_shift;
    math_output_data_ = output;
    math_unit_executed_ = true;

    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "Mathunit: in=%08x out=%08x\n", input, output);
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "Mathunit: l1p:   raw=%-2d shifted=%-2d inverted=%-2d\n", l1p,l1p_shifted,l1p_shifted_inverted);
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "Mathunit: final_shift=%d\n", final_shift);
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "Mathunit: scale: raw=0x%02x final=%d\n", scale_raw, scale);
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "Mathunit: input_shifted=%09" PRIx64 "\n", input_shifted);
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "Mathunit: tbl_index=%d word=%d byte=%d\n", tbl_index,tbl_index_word,tbl_index_byte);
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "Mathunit: tbl_data=%02x\n", tbl_data);
    return output;
  }

  void MauStatefulAlu::update_addresses(int old_addr, int new_addr) {
    if ((old_addr < 0) || (new_addr < 0)) return;

    // these addresses are huffman encoded (whereas normal salu addresses are not, they get the width from the instruction)
    int old_word_addr = Address::meter_addr_get_word_addr(old_addr);
    int old_subword   = Address::meter_addr_get_subword_huffman(old_addr);

    int new_word_addr = Address::meter_addr_get_word_addr(new_addr);
    int new_subword   = Address::meter_addr_get_subword_huffman(new_addr);

    RMT_LOG(RmtDebug::verbose(),
            "MauStatefulALU: update_addresses old=%x (%x,%x) new=%x last=(%x,%x)\n",
            old_addr, old_word_addr, old_subword, new_addr, last_word_addr_, last_subword_);


    //  To handle forwarding limitations the SALU keeps track of the last address and data used.
    //   Then if the current address matches the last address, the math unit actually uses the last data
    //   rather than the new data from the ram. To handle movereg, if the D (new) word_addr matches the
    //   last_word_addr we fix up the stored last_data_in by copying in the data that was written to D
    //   as part of the movereg (last_config_write_data_).
    //   last_config_write_data_ data is stored during the config write that updates D in an earlier
    //   part of the movereg.
    if (new_word_addr == last_word_addr_) {
      int old_width = Address::stateful_addr_get_width(old_addr,Address::stateful_addr_get_shift(old_addr));
      int new_width = Address::stateful_addr_get_width(new_addr,Address::stateful_addr_get_shift(new_addr));
      RMT_ASSERT( old_width == new_width );
      RMT_ASSERT( old_width != 0 );
      int width = old_width;

      RMT_LOG(RmtDebug::verbose(), "MauMeterALU: update_addresses updating last_data_in width=%d old_value = 0x%016" PRIx64 "_%016" PRIx64 "\n",
              width,last_data_in.get_word(64),last_data_in.get_word(0));
      if ( width <= 64 ) {
        uint64_t subword = last_config_write_data_.get_word( width * new_subword, width );
        last_data_in.set_word(subword, width * new_subword, width );
        RMT_LOG(RmtDebug::verbose(), "MauMeterALU: update_addresses updating last_data_in subword %d->%d subword = 0x%0*" PRIx64 "\n",
                old_subword,new_subword,width/4,subword);
      }
      else {
        RMT_ASSERT( width == 128 );
        last_data_in.copy_from( last_config_write_data_ );
      }
      RMT_LOG(RmtDebug::verbose(), "MauMeterALU: update_addresses updated last_data_in new_value = 0x%016" PRIx64 "_%016" PRIx64 "\n",
              last_data_in.get_word(64),last_data_in.get_word(0));
    }

    // If the old subword is the same as the last subword then we need to update the last address stored
    //  to be the new_addr. So, if the next cycle is an access to new_addr we will use the special forwarding
    //  case. Note: in this case we don't fix up last_data_in to be correct, instead we set the last_address_moved flag
    //  which tells the forwarding logic to use the old math unit inputs rather than extracting from last_data_in.
    if ((old_word_addr == last_word_addr_) && (old_subword == last_subword_)) {
      last_address_moved_ = true;
      last_word_addr_ = new_word_addr;
      last_subword_   = new_subword;
      RMT_LOG(RmtDebug::verbose(), "MauMeterALU: update_addresses updated last to (%x,%x)\n",
              last_word_addr_,last_subword_);
    }
  }

  void MauStatefulAlu::reset_resources() {
    chip_salu_.reset_resources();
    decoded_inputs_.valid=false;
    for (int i=0;i<kStatefulAluCmpAlus;++i) {
      cmp_alu_out_[i].valid=false;
    }
    random_number_.valid = false;
    minmax_mask_was_zero_=false;
    minmax_index_=0;
  }
  bool MauStatefulAlu::get_match_output() {
    RMT_ASSERT (decoded_inputs_.valid) ;

    if ( decoded_inputs_.stateful_clear ) {
      return false;
    }

    RMT_ASSERT (cmp_alu_out_[0].valid) ;
    RMT_ASSERT (cmp_alu_out_[1].valid) ;

    bool match0 = cmp_alu_out_[0].tmatch_match;
    bool match1 = cmp_alu_out_[1].tmatch_match;

    bool sbus_match_comb = chip_salu_.get_salu_sbus_match_comb(com_reg_,decoded_inputs_.instr_addr);

    if ( sbus_match_comb ) {
      return match1 && match0;
    }
    else {
      return match1 || match0;
    }
  }
  bool MauStatefulAlu::get_learn_output() {
    RMT_ASSERT (decoded_inputs_.valid) ;

    if ( decoded_inputs_.stateful_clear ) {
      return false;
    }

    RMT_ASSERT (cmp_alu_out_[0].valid) ;
    RMT_ASSERT (cmp_alu_out_[1].valid) ;

    bool learn0 = cmp_alu_out_[0].tmatch_learn;
    bool learn1 = cmp_alu_out_[1].tmatch_learn;

    bool sbus_learn_comb = chip_salu_.get_salu_sbus_learn_comb(com_reg_,decoded_inputs_.instr_addr);

    if ( sbus_learn_comb ) {
      return learn1 && learn0;
    }
    else {
      return learn1 || learn0;
    }
  }


}
