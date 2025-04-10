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

#ifndef _JBAY_SHARED_MAU_CHIP_STATEFUL_ALU_
#define _JBAY_SHARED_MAU_CHIP_STATEFUL_ALU_

// Chip specific Stateful ALU code - this is the version used for JBay

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <bitvector.h>

// Reg defs auto-generated from Semifore
#include <register_includes/salu_instr_common_array.h>
#include <register_includes/salu_instr_output_alu_array2.h>
#include <register_includes/salu_instr_state_alu_array2.h> // passed in
#include <register_includes/salu_instr_tmatch_alu_array2.h>
#include <register_includes/tmatch_mask_array2.h>
#include <register_includes/stateful_clear_action_output.h>
#include <register_includes/salu_instr2_state_alu_array2.h>
#include <register_includes/salu_const_regfile_msbs_array.h>

namespace MODEL_CHIP_NAMESPACE {

class MauChipStatefulAlu : public MauObject {

  static constexpr int      kType = RmtTypes::kRmtTypeStatefulAlu;
  static constexpr int      kNumStatefulAlus = MauDefs::kNumStatefulAlus;
  using StatefulBus = std::array<bool,kNumStatefulAlus>;

  static constexpr int      kDataBusWidth = MauDefs::kDataBusWidth;
  static constexpr int      kStatefulMeterAluDataBits = MauDefs::kStatefulMeterAluDataBits;
  static constexpr int      kActionOutputBusWidth = MauDefs::kActionOutputBusWidth;

 public:
  static bool kMaxReturnsHighestIndex; // Defined in rmt-config.cpp
  static bool kMinReturnsHighestIndex; // Defined in rmt-config.cpp
  static constexpr int kExtraMSBits = 2;
  static constexpr int kOutMemHi = 1;
  static constexpr int kOutMemLo = 0;
  static constexpr int kOutPhvHi = 3;
  static constexpr int kOutPhvLo = 2;
  static constexpr int kOutAluHi = 5;
  static constexpr int kOutAluLo = 4;
  static constexpr int kOutCmp   = 6;
  static constexpr int kOutSAddr = 7;
  static constexpr int kOutDiv   = 8;
  static constexpr int kOutMod   = 9;
  static constexpr int kPreIncDec= 10;

  static constexpr int kAluBits = 2; // assuming 4 Salu

  MauChipStatefulAlu(RmtObjectManager *om, int pipeIndex, int mauIndex,
                     int logicalRowIndex, int alu_index, Mau *mau);
  ~MauChipStatefulAlu();

  void reset_resources();

  // as all chips operate on the state register (but using different fields),
  //  instantiate it in shared code and pass it in)
  void get_a(register_classes::SaluInstrStateAluArray2& state_reg,
             bool alu_1or2, uint64_t phv_hi,
             uint64_t phv_lo, uint64_t ram_hi, uint64_t ram_lo,
             uint64_t c,int width,uint64_t mask,
             int instr_addr,int alu_addr,
             bool is_run_stateful,
             uint64_t* a, int64_t* as);
  void get_b(register_classes::SaluInstrStateAluArray2& state_reg,
             bool hilo, bool alu_1or2, uint64_t phv_hi,
             uint64_t phv_lo, uint64_t ram_hi, uint64_t ram_lo,
             uint64_t c,int width,uint64_t mask,
             int instr_addr,int alu_addr,
             bool is_run_stateful,
             uint64_t* b, int64_t* bs);

  int salu_output_cmpfn(int alu,int instr_addr) {
    RMT_ASSERT(alu>=0 && alu<4);
    return out_reg_.salu_output_cmpfn(instr_addr,alu);
  }
  int salu_output_asrc(int alu,int instr_addr) {
    int asrc = out_reg_.salu_output_asrc(instr_addr,alu);
    return asrc;
  }
  bool check_width(int w) {
    return (w==0 || w == 3 || w == 4 || w == 5 || w == 6 || w == 7);
  }
  // Select which output Alu's post mask predication input to use
  //  for the output action.
  int which_cmp_output( register_classes::StatefulCtl& stateful_ctl) {
    return stateful_ctl.salu_output_pred_sel();
  }

  uint32_t get_out_addr(register_classes::StatefulCtl& stateful_ctl, uint32_t addr) {
    if ( stateful_ctl.salu_stage_id_enable() ) {
      uint32_t vaddr = Address::meter_addr_get_vaddr(addr);
      //uint32_t type  = Address::meter_addr_get_type(addr);
      uint32_t stage = stateful_ctl.salu_stage_id();
      constexpr int addr_width = Address::kMeterAddrWidth-Address::kMeterAddrTypeWidth-1;
      return (stage << addr_width) |
          vaddr;
    }
    else {
      return addr;
    }
  }

  uint32_t get_divider_output(register_classes::SaluInstrCommonArray& com_reg,
                              int instr, int64_t as, int64_t bs );
  uint32_t get_modulus_output(register_classes::SaluInstrCommonArray& com_reg,
                              int instr, int64_t as, int64_t bs );

  uint8_t get_regfile_addr( register_classes::SaluInstrCmpAluArray2& cmp_reg,
                            int instr_addr, int which_alu) {
    return cmp_reg.salu_cmp_regfile_adr(instr_addr, which_alu);
  }

  bool get_cmp_asrc_mask_enable( register_classes::SaluInstrCmpAluArray2& cmp_reg,
                                 int instr_addr, int which_alu) {
    return cmp_reg.salu_cmp_asrc_mask_enable(instr_addr, which_alu);
  }
  bool get_cmp_bsrc_mask_enable( register_classes::SaluInstrCmpAluArray2& cmp_reg,
                                 int instr_addr, int which_alu) {
    return cmp_reg.salu_cmp_bsrc_mask_enable(instr_addr, which_alu);
  }
  bool get_cmp_mask_input( register_classes::SaluInstrCmpAluArray2& cmp_reg,
                           int instr_addr, int which_alu) {
    return cmp_reg.salu_cmp_mask_input(instr_addr, which_alu);
  }

  void run_tmatch(register_classes::SaluInstrCmpAluArray2& cmp_reg,
                  int which_cmp_alu,int instr, uint64_t a,uint64_t b,
                  uint64_t word_width_mask, // just for checking
                  bool *match, bool *learn);

  bool calculate_cmp_with_sbus( register_classes::SaluInstrCmpAluArray2& cmp_reg,
                                register_classes::SaluInstrCommonArray& com_reg,
                                int instr_addr, int which_alu, bool cmp_in, bool sbus_in);

  bool get_sbus(register_classes::SaluInstrCommonArray& com_reg, int alu_index, int instr,
                const StatefulBus& match_bus,
                const StatefulBus& learn_or_match_bus);

  // minmax
  bool get_salu_minmax_enable(register_classes::SaluInstrCommonArray& com_reg,
                              int instr) {
    return com_reg.salu_minmax_enable(instr);
  }
  bool get_salu_sbus_match_comb(register_classes::SaluInstrCommonArray& com_reg,
                           int instr) {
    return com_reg.salu_sbus_match_comb(instr);
  }
  bool get_salu_sbus_learn_comb(register_classes::SaluInstrCommonArray& com_reg,
                           int instr) {
    return com_reg.salu_sbus_learn_comb(instr);
  }

  void calculate_minmax(register_classes::SaluInstrCommonArray& com_reg,
                        register_classes::SaluInstrStateAluArray2& state_reg,
                        register_classes::SaluInstrCmpAluArray2& cmp_reg,
                        register_classes::SaluConstRegfileArray& reg_file,
                        int instr,
                        bool same_word_as_last_cycle, bool last_was_min_max,
                        const BitVector<MauChipStatefulAlu::kDataBusWidth>& last_data_in,
                        const BitVector<MauChipStatefulAlu::kDataBusWidth>& last_data_out,
                        BitVector<kStatefulMeterAluDataBits> *phv,
                        BitVector<kDataBusWidth> *data_in,
                        BitVector<kDataBusWidth> *data_out,
                        uint64_t* minmax_out_lo, uint64_t* minmax_out_hi,
                        uint32_t *pre_inc_dec_value,
                        bool* mask_was_zero);


  bool get_lmatch_adr_bit_enable(int alu,int instr_addr) {
    return out_reg_.salu_lmatch_adr_bit_enable(instr_addr,alu);
  }
  // XXX: salu_pred_disable has been removed
  //bool get_pred_disable(int alu,int instr_addr) {
  //  return out_reg_.salu_pred_disable(instr_addr,alu);
  //}

  void calculate_stateful_clear( register_classes::StatefulCtl& stateful_ctl,
                                 register_classes::SaluInstrStateAluArray2& state_reg,
                                 BitVector<kDataBusWidth> *data_out,
                                 BitVector<kActionOutputBusWidth> *action_out );

  bool uses_divide() {
    for (int i = 0; i < 4; i++) {
      if ((com_reg_.salu_divide_enable(i) & 1) == 1) return true;
    }
    return false;
  }

  bool get_flyover_src_sel(int instr_addr,int alu_addr) {
    return instr2_state_alu_.salu_flyover_src_sel(instr_addr,alu_addr);
  }

  static const char* nop1_addc_str() { return "AddC"; }
  static const char* nop2_subc_str() { return "SubC"; }
  // On JBay is an AddC
  uint64_t nop1_addc(bool hilo, bool alu_1or2, int64_t a,int64_t b,int width, bool dual);
  // On JBay is an SubC
  uint64_t nop2_subc(bool hilo, bool alu_1or2, int64_t a,int64_t b,int width, bool dual);
  void add(bool hilo, bool alu_1or2, int64_t a,int64_t b);
  void sub(bool hilo, bool alu_1or2, int64_t a,int64_t b);

  uint32_t get_reg_file_msbs( int addr ) {
    return const_regfile_msbs_.salu_const_regfile_msbs(addr);
  }

 private:
  bool                                        ctor_running_;
  int                                         alu_index_;
  register_classes::SaluInstrCommonArray      com_reg_;
  register_classes::SaluInstrOutputAluArray2  out_reg_;
  register_classes::SaluInstrTmatchAluArray2  instr_tmatch_alu_;
  register_classes::TmatchMaskArray2          tmatch_mask_;
  register_classes::StatefulClearActionOutput stateful_clear_action_output_;
  register_classes::SaluInstr2StateAluArray2  instr2_state_alu_;
  register_classes::SaluConstRegfileMsbsArray const_regfile_msbs_;

  // For AddC and SubC instructions
  bool lo_add_ran_[2];
  bool lo_sub_ran_[2];
  int  carry_[2];

  // TODO: share the copy of this in mau-stateful-alu.h ??
  inline uint64_t sign_ext64(uint64_t val, int width) { return (64!=width && ((val >> (width-1)) & 1)) ? val | ~((UINT64_C(1) << width) - UINT64_C(1)) : val; }

  void com_reg_write_callback(uint32_t instr);

};


}

#endif // _JBAY_SHARED_MAU_CHIP_STATEFUL_ALU_
