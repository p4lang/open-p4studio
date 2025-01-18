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
#include <address.h>
#include <mau-chip-stateful-alu.h>
#include <mau-info.h>
#include <register_adapters.h>

// Chip specific Stateful ALU code - this is the version used for JBay

namespace MODEL_CHIP_NAMESPACE {

MauChipStatefulAlu::MauChipStatefulAlu(RmtObjectManager *om, int pipeIndex,
                                       int mauIndex, int logicalRowIndex,
                                       int alu_index, Mau *mau)
    : MauObject(om, pipeIndex, mauIndex, kType, logicalRowIndex, mau),
      ctor_running_(true), alu_index_(alu_index),
      com_reg_(default_adapter(com_reg_,chip_index(), pipeIndex, mauIndex, alu_index_,
                               [this](uint32_t i){this->com_reg_write_callback(i); })),
      out_reg_(default_adapter(out_reg_,chip_index(), pipeIndex, mauIndex, alu_index_)),
      instr_tmatch_alu_(default_adapter(instr_tmatch_alu_,chip_index(), pipeIndex, mauIndex, alu_index_)),
      tmatch_mask_(default_adapter(tmatch_mask_,chip_index(), pipeIndex, mauIndex, alu_index_)),
      stateful_clear_action_output_(default_adapter(stateful_clear_action_output_,chip_index(), pipeIndex, mauIndex, alu_index_)),
      instr2_state_alu_(default_adapter(instr2_state_alu_,chip_index(), pipeIndex, mauIndex, alu_index_)),
      const_regfile_msbs_(default_adapter(const_regfile_msbs_,chip_index(), pipeIndex, mauIndex, alu_index_))
{
  com_reg_.reset();
  out_reg_.reset();
  instr_tmatch_alu_.reset();
  tmatch_mask_.reset();
  stateful_clear_action_output_.reset();
  instr2_state_alu_.reset();
  const_regfile_msbs_.reset();
  reset_resources();
  ctor_running_ = false;
}

MauChipStatefulAlu::~MauChipStatefulAlu() { }

void MauChipStatefulAlu::reset_resources() {
  lo_add_ran_[0] = false;
  lo_sub_ran_[0] = false;
  carry_[0] = 0;
  lo_add_ran_[1] = false;
  lo_sub_ran_[1] = false;
  carry_[1] = 0;
}

void MauChipStatefulAlu::com_reg_write_callback(uint32_t instr) {
  if (ctor_running_) return;
  // Get ref to MeterAlu
  int logrow = MauMeterAlu::get_meter_alu_logrow_index(alu_index_);
  MauLogicalRow *logical_row = mau()->logical_row_lookup(logrow);
  RMT_ASSERT(logical_row != nullptr);
  MauMeterAlu *meter_alu = logical_row->mau_meter_alu();
  RMT_ASSERT(meter_alu != nullptr);
  // And upcall ctl_write_callback
  // This will allow MauMeterAlu code to detect any change in usage
  // of salu_divide and further upcall delay checking code
  meter_alu->ctl_write_callback();
}

void MauChipStatefulAlu::get_a(
    register_classes::SaluInstrStateAluArray2& state_reg,
    bool alu_1or2, uint64_t phv_hi,
    uint64_t phv_lo, uint64_t ram_hi, uint64_t ram_lo,
    uint64_t c,int width,uint64_t mask,
    int instr_addr,int alu_addr,
    bool is_run_stateful,
    uint64_t* a, int64_t* as) {

  // This function should only be called with up to 32 bit masks
  RMT_ASSERT( 0 == ( mask & UINT64_C( 0xFFFFFFFF00000000 )) );

  // In 64 and 128 bit cases need to flyover upper bits into "a"
  //  (note: not into the signed version "as"
  bool flyover = (width>=64);
  uint64_t flyover_mask = flyover ? UINT64_C( 0xFFFFFFFF00000000 ) : 0;

  // force width to be less than 32, as this path is only 32 bits wide apart from flyover
  width = width>32 ? 32 :  width;

  //  0 = memory lo
  //  1 = memory hi
  //  2 = phv lo  (alu2-hi only for symmetric divide/mod operations)
  //  3 = phv hi  (alu2-hi only for symmetric divide/mod operations)
  //  4 = constant
  switch ( state_reg.salu_asrc_input(instr_addr, alu_addr) ) {
    case 0:
      *a  = ram_lo & mask;
      *as = sign_ext64(*a, width);
      *a |= ram_lo & flyover_mask;
      break;
    case 1:
      *a  = ram_hi & mask;
      *as = sign_ext64(*a, width);
      *a |= ram_hi & flyover_mask;
      break;
    case 2:
      RMT_ASSERT( ! is_run_stateful ); // there is no phv for run stateful
      RMT_ASSERT( alu_addr == 2 ); // alu2-hi
      *a  = phv_lo & mask;
      *as = sign_ext64(*a, width);
      *a |= phv_lo & flyover_mask;
      break;
    case 3:
      RMT_ASSERT( ! is_run_stateful ); // there is no phv for run stateful
      RMT_ASSERT( alu_addr == 2 ); // alu2-hi
      *a  = phv_hi & mask;
      *as = sign_ext64(*a, width);
      *a |= phv_hi & flyover_mask;
      break;
    case 4:
      *a  = c;
      *as = c;
      // No flyover
      break;
    default:
      RMT_ASSERT(0);
      break;
  }
}

void MauChipStatefulAlu::get_b(
    register_classes::SaluInstrStateAluArray2& state_reg,
    bool hilo, bool alu_1or2, uint64_t phv_hi,
    uint64_t phv_lo, uint64_t ram_hi, uint64_t ram_lo,
    uint64_t c,int width,uint64_t mask,
    int instr_addr,int alu_addr,
    bool is_run_stateful,
    uint64_t* b, int64_t* bs) {
  // This function should only be called with up to 32 bit masks
  RMT_ASSERT( 0 == ( mask & UINT64_C( 0xFFFFFFFF00000000 )) );

  // In 64 and 128 bit cases need to flyover upper bits into "b"
  //  (note: not into the signed version "bs"
  bool flyover = (width>=64);
  uint64_t flyover_mask = flyover ? UINT64_C( 0xFFFFFFFF00000000 ) : 0;

  // force width to be less than 32, as this path is only 32 bits wide apart from flyover
  width = width>32 ? 32 :  width;

  switch ( state_reg.salu_bsrc_input(instr_addr, alu_addr) ) {
    // 0 = phv lo
    // 1 = phv hi
    // 2 = memory lo (alu2-hi only for symmetric divide/mod operations)
    // 3 = memory hi (alu2-hi only for symmetric divide/mod operations)
    // 4 = const
    case 0:
      RMT_ASSERT( ! is_run_stateful ); // there is no phv for run stateful
      *b  = phv_lo & mask;
      *bs = sign_ext64(*b, width);
      *b |= phv_lo & flyover_mask;
      break;
    case 1:
      RMT_ASSERT( ! is_run_stateful ); // there is no phv for run stateful
      *b  = phv_hi & mask;
      *bs = sign_ext64(*b, width);
      *b |= phv_hi & flyover_mask;
      break;
    case 2:
      RMT_ASSERT( alu_addr == 2 ); // alu2-hi
      *b  = ram_lo & mask;
      *bs = sign_ext64(*b, width);
      *b |= ram_lo & flyover_mask;
      break;
    case 3:
      RMT_ASSERT( alu_addr == 2 ); // alu2-hi
      *b  = ram_hi & mask;
      *bs = sign_ext64(*b, width);
      *b |= ram_hi & flyover_mask;
      break;
    case 4:
      *b  = c;
      *bs = c;
      // No flyover
     break;
  }
}

uint32_t MauChipStatefulAlu::get_divider_output(register_classes::SaluInstrCommonArray& com_reg,
                              int instr, int64_t as, int64_t bs ) {
  RMT_ASSERT( com_reg.salu_divide_enable(instr) );
  // operands always treated as 16b signed
  int16_t as16 = as & 0xFFFF;
  int16_t bs16 = bs & 0xFFFF;
  // Handle divide by zero case as Cadence signed divider: max +/- value (closest to +/- infinity)
  if (bs16 == 0) {
    return as16>=0 ? 0x7FFF : 0x8000;
  }
  // handle overflow case as Cadence signed divider component
  if ( as == 0x8000 && bs == -1 ) {
    // XXX: Return 0x8000 in this case - used to return 0x7ffff
    return 0x8000;
  }
  return  (as16 / bs16) & 0xFFFF;
}
uint32_t MauChipStatefulAlu::get_modulus_output(register_classes::SaluInstrCommonArray& com_reg,
                                                int instr, int64_t as, int64_t bs ) {
  RMT_ASSERT( com_reg.salu_divide_enable(instr) );
  // operands always treated as 16b signed
  int16_t as16 = as & 0xFFFF;
  int16_t bs16 = bs & 0xFFFF;
  // Handle divide by zero case as Cadence signed divider: returns A as modulus
  if (bs16 == 0) {
    return 0xffff & as16;
  }
  // XXX: removed special case for (MaxNeg / -1) which used to return 0xffff
  // XXX: remainder always 0 when divisor is -1/+1
  if ( (bs == 1) || (bs == -1) ) {
    return 0;
  }
  int16_t mod = as16 % bs16;
  // C++ (ISO 2011) % gives the same sign as the dividend
  // Cadence divider modulus has the sign of the divisor (same as common lisp!)
  // eg
  //  A %  B    C++11   lisp
  //  6 %  16 =  6        6
  //  6 % -16 =  6      -10
  // -6 %  16 = -6       10
  // -6 % -16 = -6       -6
  //
  // So when the sign of A and B is different we need to fix the result of
  //  the mod by adding B (unless the mod is 0)
  if (( (as & 0x8000) != (bs & 0x8000) ) && (mod!=0)) { // test sign bits
    mod = mod + bs;
  }
  return (mod & 0xffff);
}


void MauChipStatefulAlu::run_tmatch(register_classes::SaluInstrCmpAluArray2& cmp_reg,
                                    int which_cmp_alu,int instr, uint64_t a,uint64_t b,
                                    uint64_t word_width_mask, // just for checking
                                    bool *match, bool *learn) {

  RMT_ASSERT( which_cmp_alu < MauDefs::kStatefulAluTMatchAlus );

  bool vld = a & 1; // bit zero is vld bit
  *learn = instr_tmatch_alu_.salu_tmatch_vld_ctl(instr, which_cmp_alu) && (! vld );

  if ( ! cmp_reg.salu_cmp_tmatch_enable(instr,which_cmp_alu) ) {
    *match = false;
    return;
  }

  uint64_t mask0 = tmatch_mask_.tmatch_mask( which_cmp_alu, 0 );
  uint64_t mask1 = tmatch_mask_.tmatch_mask( which_cmp_alu, 1 );
  uint64_t tmatch_mask = (mask1<<32) | mask0;

  // check tmatch_mask does not unmask any bits beyond the
  //  word width mask
  // XXX: keep klocwork happy with || GLOBAL_FALSE
  RMT_ASSERT((0 == ((~tmatch_mask) & (~word_width_mask))) || GLOBAL_FALSE );

  // In JBay only the bottom 8 bits can be set
  uint64_t invert = instr_tmatch_alu_.salu_tmatch_invert(instr, which_cmp_alu);

  uint64_t result = ~ ( a ^ b ); // bitwise equality

  result ^= invert; // invert selected bits

  // tmatch_mask has a 1 for each bit that DOES NOT participate in the match
  result |= tmatch_mask;

  // All bits must be 1 for a match
  *match = (result == UINT64_C(0xFFFFFFFFFFFFFFFF));

}

bool MauChipStatefulAlu::get_sbus(register_classes::SaluInstrCommonArray& com_reg, int alu_index, int instr,
                                  const MauChipStatefulAlu::StatefulBus& match_bus,
                                  const MauChipStatefulAlu::StatefulBus& learn_or_match_bus) {

  uint32_t m_listen  = com_reg.salu_match_sbus_listen(instr);
  uint32_t lm_listen = com_reg.salu_lmatch_sbus_listen(instr);

  // Normally OR together inputs, this selects ANDing mode
  bool anding_mode = com_reg_.salu_sbus_in_comb(instr);

  // in ORing mode start false and set true if bits are set
  //   in ANDing mode start true and set false if bits are clear
  bool out = anding_mode ? true : false;
  char log_match_bus[kNumStatefulAlus];
  char log_lmatch_bus[kNumStatefulAlus];
  log_match_bus[kNumStatefulAlus-1]  = 0;
  log_lmatch_bus[kNumStatefulAlus-1] = 0;
  // registers contain 3 bits, one for each of the other ALUs, keep track with reg_bit
  for (int i=0,reg_bit=0;i<kNumStatefulAlus; ++i) {
    if ( i != alu_index ) { // not this ALU
      bool m_listen_bit  = (m_listen >>reg_bit)&1;
      bool lm_listen_bit = (lm_listen>>reg_bit)&1;
      if (anding_mode) {
        if ((!match_bus[i]) &&          m_listen_bit)  out = false;
        if ((!learn_or_match_bus[i]) && lm_listen_bit) out = false;
      }
      else { // ORing mode
        if (match_bus[i] &&          m_listen_bit)  out = true;
        if (learn_or_match_bus[i] && lm_listen_bit) out = true;
      }

      log_match_bus[kNumStatefulAlus-reg_bit-2]  = match_bus[i]          ? '1' : '0';
      log_lmatch_bus[kNumStatefulAlus-reg_bit-2] = learn_or_match_bus[i] ? '1' : '0';
      reg_bit++;
    }
  }
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "SBus Input: match=0b%s lmatch=0b%s\n",log_match_bus,log_lmatch_bus);
  return out;
}

bool MauChipStatefulAlu::calculate_cmp_with_sbus( register_classes::SaluInstrCmpAluArray2& cmp_reg,
                                                  register_classes::SaluInstrCommonArray& com_reg,
                                                  int instr_addr, int which_alu, bool cmp_in, bool sbus_in) {
  bool do_or  = cmp_reg.salu_cmp_sbus_or(instr_addr, which_alu);
  bool do_and = cmp_reg.salu_cmp_sbus_and(instr_addr, which_alu);
  bool invert = cmp_reg.salu_cmp_sbus_invert(instr_addr, which_alu);
  RMT_ASSERT( ! ( do_or && do_and ) ); // uArch says "using either an AND function or an OR function"

  // check that when when no bits are selected that we don't invert, and or or the sbus
  uint32_t m  = com_reg.salu_match_sbus_listen(instr_addr);
  uint32_t lm = com_reg.salu_lmatch_sbus_listen(instr_addr);
  RMT_ASSERT( ! ((m==0) && (lm==0) && (invert || do_and || do_or)) );

  bool out = cmp_in;

  bool sbus = invert ? !sbus_in : sbus_in;
  if (do_or)   out = out || sbus;
  if (do_and)  out = out && sbus;

  if ( m!=0 || lm!=0) {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "          Sbus: cmp_in=%d sbus_pre_invert=%d sbus_post_invert=%d out=%d\n",
            cmp_in?1:0,sbus_in?1:0,sbus?1:0,out?1:0);
  }
  return out;
}

void MauChipStatefulAlu::calculate_minmax(register_classes::SaluInstrCommonArray& com_reg,
                                          register_classes::SaluInstrStateAluArray2& state_reg,
                                          register_classes::SaluInstrCmpAluArray2& cmp_reg,
                                          register_classes::SaluConstRegfileArray& reg_file,
                                          int instr,
                                          bool same_word_as_last_cycle, bool last_was_min_max,
                                          const BitVector<MauChipStatefulAlu::kDataBusWidth>& last_data_in,
                                          const BitVector<MauChipStatefulAlu::kDataBusWidth>& last_data_out,
                                          BitVector<MauChipStatefulAlu::kStatefulMeterAluDataBits> *phv,
                                          BitVector<MauChipStatefulAlu::kDataBusWidth> *data_in,
                                          BitVector<MauChipStatefulAlu::kDataBusWidth> *data_out,
                                          uint64_t* minmax_out_lo, uint64_t* minmax_out_hi,
                                          uint32_t *pre_inc_dec_value,
                                          bool* mask_was_zero){

  bool special_case_for_lack_of_forwarding = same_word_as_last_cycle && !last_was_min_max;

  // 0 = 8b  min, 1 = 8b  max, 2 = 16b min, 3 = 16b max
  int ctl = com_reg.salu_minmax_ctl(instr);
  bool width16 = ctl & 2;
  bool is_max  = ctl & 1;

  // 1 = mask comes from 32b Registerfile CMP ALU3 registerfile address. 0 = from phv
  bool mask_ctl = com_reg.salu_minmax_mask_ctl(instr);
  uint32_t mask = 0;
  int reg_file_addr = -1;
  if (mask_ctl) {
    reg_file_addr = 3 & cmp_reg.salu_cmp_regfile_adr(instr, 3 /*always from ALU3's*/);
    mask = reg_file.salu_const_regfile(reg_file_addr);
  }
  else {
    mask = phv->get_word(0);
  }

  // 8 bits of mask needed for the eight 16 bit numbers
  // or 16 bits of mask needed for the sixteen 8 bit numbers
  mask &= width16 ? 0xFF : 0xFFFF;

  *mask_was_zero = ( mask == 0 ); // if mask==0 RTL can generate an interrupt


  const BitVector<MauChipStatefulAlu::kDataBusWidth> *minmax_src;
  bool minmax_from_phv = com_reg.salu_minmax_src_sel(instr);

  if ( minmax_from_phv ) {
    minmax_src = phv;
  }
  else {
      if ( ! special_case_for_lack_of_forwarding ) { // normal case
        minmax_src = data_in;
      }
      else {
        // If the this word address is the same as the last cycle, we
        //  have to use the last data in the hardware does not
        //  forward minmax correctly
        minmax_src = &last_data_in;
      }
  }

  int mask_width = width16 ? 8 : 16;
  int data_width = width16 ? 16 : 8;
  int selected_index = 0;
  uint32_t selected_value = 0;
  bool first_value = true;
  for (int i=0;i<mask_width;++i) {
    if ( (mask>>i) & 1 ) {
      uint32_t v = minmax_src->get_word( i * data_width, data_width );
      // Note: for equal values there are config values to determine
      //   whether it returns the highest or lowest index.
      if ( first_value ||
           // MAX
           (  is_max  &&
              ( kMaxReturnsHighestIndex ? ( v >= selected_value ) : ( v > selected_value ) ) ) ||
           // MIN
           (  (!is_max)  &&
              ( kMinReturnsHighestIndex ? ( v <= selected_value ) : ( v < selected_value ) ) ) ) {
        selected_index = i;
        selected_value = v;
        first_value = false;
      }
    }
  }
  *pre_inc_dec_value = selected_value;
  *minmax_out_lo = selected_value;
  *minmax_out_hi = selected_index;

  data_out->copy_from(*data_in);

  // now do post incr/decr
  bool postinc = com_reg.salu_minmax_postinc_enable(instr);
  bool postdec = com_reg.salu_minmax_postdec_enable(instr);

  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "MinMax%s width=%d found %s with mask=0x%x from %s : selected index=%d value=0x%x %s\n",
          postinc||postdec?(postinc?" postinc":" postdec"):"",
          data_width,is_max?"Max":"Min",mask,
          reg_file_addr==-1? "phv" : ("regfile " + boost::lexical_cast<std::string>(reg_file_addr)).c_str(),
          selected_index,selected_value,special_case_for_lack_of_forwarding?" (special case)":"");


  if (postinc || postdec) {
    bool postmod_value_from_phv = com_reg.salu_minmax_postmod_value_ctl(instr);
    int mod_value = 0;
    if ( postmod_value_from_phv ) {
      // 1 = postmod value comes from PHV[23:16] for 8b minmax or PHV[15:8] for 16b minmax when salu_minmax_mask_ctl==0.
      //     postmod value comes from PHV[7:0] when salu_minmax_mask_ctl==1
      mod_value = phv->get_byte( mask_ctl ? 0 : (width16 ? 1 : 2) );
    }
    else {
      // 0 = postmod value comes from 32b Registerfile bits 23:16 for 8b minmax and bits 15:8 for 16b minmax when salu_minmax_mask_ctl==1.
      //     postmod value comes from registerfile bits 7:0 when salu_minmax_mask_ctl==0.
      //    CMP ALU3 registerfile address (salu_cmp_regfile_adr) is overloaded for this purpose to save hardware";
      int addr = cmp_reg.salu_cmp_regfile_adr(instr, 3 /*always from ALU3's*/);
      uint32_t r = reg_file.salu_const_regfile(addr);
      mod_value = 0xFF & ( (!mask_ctl) ? r : (width16 ? (r>>8) : (r>>16)) );
    }
    int value = selected_value;
    if (mask) { // only do this if the mask has at least one bit set
      if (postinc) {
        value += mod_value;
        int max_value = width16 ? 0xFFFF : 0xFF;
        if (value > max_value) value = max_value;
      }
      else {
        value -= mod_value;
        if (value < 0) value=0;
      }
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "MinMax post%s mod=0x%x new_value=0x%x\n",postinc?"inc":"dec",mod_value,value);
      // Only change the data word if the minmax source was the from the data word
      if ( ! minmax_from_phv ) {
        data_out->set_word( value, selected_index * data_width, data_width );
      }
    }
    else {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "MinMax post%s mask=0, not %srementing\n",postinc?"inc":"dec",postinc?"inc":"dec");
    }
    // note: the output that ends up as output_asrc is not affected by special_case_for_lack_of_forwarding
    *minmax_out_lo = value;
    if ( special_case_for_lack_of_forwarding ) {
      // If the this word address is the same as the last cycle, we
      //  have to use the last data out as the hardware squashes the
      //  output of minmax to avoid overwriting the updated value.
      //  This overwrite would happen because we have used the
      //  last data in for the input to minmax above
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatefulAlu), "MinMax outputting last data out\n");
      data_out->copy_from(last_data_out);
    }
  }

}

void MauChipStatefulAlu::calculate_stateful_clear( register_classes::StatefulCtl& stateful_ctl,
                                                   register_classes::SaluInstrStateAluArray2& state_reg,
                                                   BitVector<kDataBusWidth> *data_out,
                                                   BitVector<kActionOutputBusWidth> *action_out ) {
  action_out->fill_all_zeros();
  action_out->set32( 0, stateful_clear_action_output_.stateful_clear_action_output() );

  if ( stateful_ctl.salu_clear_value_ctl() ) {
    // 128 bits of data_out are stored in 4 words of salu_instr_state_alu register
    for (int w=0;w<4;w++) {
      constexpr int instr = 3; // it uses instr 3 to store the 32 bit value
      // need to concatenate all the fields in salu_instr_state_alu to get the 32 bit value
      //  this is code adapted from read() in salu_instr_state_alu.h
      uint32_t data=0;
      data |= (state_reg.salu_const_src(instr, w) & 0xf);
      data |= ((state_reg.salu_regfile_const(instr, w) & 0x1) << 4);
      data |= ((state_reg.salu_bsrc_input(instr, w) & 0x7) << 5);
      data |= ((state_reg.salu_asrc_input(instr, w) & 0x7) << 8);
      data |= ((state_reg.salu_op(instr, w) & 0xf) << 11);
      data |= ((state_reg.salu_arith(instr, w) & 0x1) << 15);
      data |= (state_reg.salu_pred(instr, w) << 16);
      data_out->set32( w, data );
    }
  }
  else {
    data_out->fill_all_zeros();
  }

}
// TODO: move into .h file when stable
// On JBay is an AddC
uint64_t MauChipStatefulAlu::nop1_addc(bool hilo, bool alu_1or2, int64_t a,int64_t b,int width, bool dual) {
  RMT_ASSERT( hilo ); // only allowed on alu-hi
  RMT_ASSERT( width >= 32 && dual );
  int which = alu_1or2 ? 1:0;
  RMT_ASSERT( lo_add_ran_[which] ); // must be paired with add running on alu-lo
  return a + b + carry_[which];
}
// On JBay is an SubC
uint64_t MauChipStatefulAlu::nop2_subc(bool hilo, bool alu_1or2, int64_t a,int64_t b,int width, bool dual) {
  RMT_ASSERT( hilo ); // only allowed on alu-hi
  RMT_ASSERT( width >= 32 && dual );
  int which = alu_1or2 ? 1:0;
  RMT_ASSERT( lo_sub_ran_[which] ); // must be paired with sub running on alu-lo
  return a - b - 1 + carry_[which];
}


void MauChipStatefulAlu::add(bool hilo, bool alu_1or2, int64_t a,int64_t b) {
  if (hilo == false) { // lo
    int which = alu_1or2 ? 1:0;
    lo_add_ran_[ which ] = true;
    carry_[which]     = (  UINT64_C(0x100000000 ) &
                         ((UINT64_C(0x0FFFFFFFF) & a) +
                          (UINT64_C(0x0FFFFFFFF) & b) )) ? 1:0;
  }
}

void MauChipStatefulAlu::sub(bool hilo, bool alu_1or2, int64_t a,int64_t b) {
  if (hilo == false) { // lo
    int which = alu_1or2 ? 1:0;
    lo_sub_ran_[ which ] = true;
    carry_[which]     = (  UINT64_C(0x100000000 ) &
                         ((UINT64_C(0x0FFFFFFFF) & a) -
                          (UINT64_C(0x0FFFFFFFF) & b) )) ? 1:0;
  }
}

}
