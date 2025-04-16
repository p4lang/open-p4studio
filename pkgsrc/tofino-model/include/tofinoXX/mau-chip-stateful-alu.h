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

#ifndef _TOFINOXX_MAU_CHIP_STATEFUL_ALU_
#define _TOFINOXX_MAU_CHIP_STATEFUL_ALU_

// Chip specific Stateful ALU code - this is default version used for Tofino
//   and TofinoB0

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <bitvector.h>

// Reg defs auto-generated from Semifore
#include <register_includes/salu_instr_output_alu_array.h>
#include <register_includes/salu_instr_state_alu_array2.h> // passed in
#include <register_includes/stateful_ctl.h>                // passed in

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
  static constexpr int kExtraMSBits = 0; // Tofino has no Extra MSBits
  static constexpr int kOutMemHi = 0;
  static constexpr int kOutMemLo = 1;
  static constexpr int kOutPhvHi = 2;
  static constexpr int kOutPhvLo = 3;
  static constexpr int kOutAluHi = 4;
  static constexpr int kOutAluLo = 5;
  static constexpr int kOutCmp   = 6;
  static constexpr int kOutSAddr = 7;  // These are illegal on Tofino,
  static constexpr int kOutDiv   = 8;  //   so salu_output_asrc will
  static constexpr int kOutMod   = 9;  //   return -1 for anything
  static constexpr int kPreIncDec= 10; //   over 6

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
    RMT_ASSERT(alu==0);
    return out_reg_.salu_output_cmpfn(instr_addr);
  }
  int salu_output_asrc(int alu,int instr_addr) {
    RMT_ASSERT(alu==0);
    int asrc = out_reg_.salu_output_asrc(instr_addr);
    if (asrc>6) return -asrc; // Illegal op
    return asrc;
  }
  bool check_width(int w) {
    return (w==0 || w == 3 || w == 4 || w == 5);
  }
  // Select which output Alu's post mask predication input to use
  //  for the output action.
  // In Tofino it is alway 0 as there is only one output alu.
  int which_cmp_output( register_classes::StatefulCtl& stateful_ctl) {
    return 0;
  }
  uint32_t get_out_addr(register_classes::StatefulCtl& stateful_ctl, uint32_t addr) {
    RMT_ASSERT(0); // JBay only
    return 0;
  }

  uint32_t get_divider_output(register_classes::SaluInstrCommonArray& com_reg,
                              int instr, int64_t as, int64_t bs ) {
    RMT_ASSERT(0); // JBay only
    return 0;
  }
  uint32_t get_modulus_output(register_classes::SaluInstrCommonArray& com_reg,
                              int instr, int64_t as, int64_t bs ) {
    RMT_ASSERT(0); // JBay only
    return 0;
  }

  uint8_t get_regfile_addr( register_classes::SaluInstrCmpAluArray2& cmp_reg,
                            int instr_addr, int which_alu) {
    return 0x3 & cmp_reg.salu_cmp_const_src(instr_addr, which_alu);
  }
  bool get_cmp_asrc_mask_enable( register_classes::SaluInstrCmpAluArray2& cmp_reg,
                                 int instr_addr, int which_alu) {
    return GLOBAL_FALSE; // JBay only
  }
  bool get_cmp_bsrc_mask_enable( register_classes::SaluInstrCmpAluArray2& cmp_reg,
                                 int instr_addr, int which_alu) {
    return GLOBAL_FALSE; // JBay only
  }
  bool get_cmp_mask_input( register_classes::SaluInstrCmpAluArray2& cmp_reg,
                           int instr_addr, int which_alu) {
    return false; // JBay only
  }

  void run_tmatch(register_classes::SaluInstrCmpAluArray2& cmp_reg,
                  int which_cmp_alu,int instr, uint64_t a,uint64_t b,
                  uint64_t word_width_mask, // just for checking
                  bool *match, bool *learn) {
    RMT_ASSERT(GLOBAL_FALSE); // JBay only: no TMatch to tofino
  }

  bool calculate_cmp_with_sbus( register_classes::SaluInstrCmpAluArray2& cmp_reg,
                                register_classes::SaluInstrCommonArray& com_reg,
                                int instr_addr, int which_alu, bool cmp_in, bool sbus_in) {
    // no sbus on Tofino
    return cmp_in;
  }
  bool get_sbus(register_classes::SaluInstrCommonArray& com_reg, int alu_index, int instr,
                const StatefulBus& match_bus,
                const StatefulBus& learn_or_match_bus) {
    // no sbus on Tofino
    return false;
  }

  // minmax
  bool get_salu_minmax_enable(register_classes::SaluInstrCommonArray& com_reg,
                              int instr) {
    return GLOBAL_FALSE; // JBay only - always false for TofinoXX
  }
  bool get_salu_sbus_match_comb(register_classes::SaluInstrCommonArray& com_reg,
                           int instr) {
    return GLOBAL_FALSE; // JBay only
  }
  bool get_salu_sbus_learn_comb(register_classes::SaluInstrCommonArray& com_reg,
                           int instr) {
    return GLOBAL_FALSE; // JBay only
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
                        bool* mask_was_zero) {
    RMT_ASSERT(GLOBAL_FALSE); // JBay only
  }

  bool get_lmatch_adr_bit_enable(int alu,int instr_addr) {
    RMT_ASSERT(alu==0);
    return GLOBAL_FALSE; // JBay only
  }

  bool get_flyover_src_sel(int instr_addr,int alu_addr) {
    return false; // JBay only
  }

  void calculate_stateful_clear( register_classes::StatefulCtl& stateful_ctl,
                                 register_classes::SaluInstrStateAluArray2& state_reg,
                                 BitVector<kDataBusWidth> *data_out,
                                 BitVector<kActionOutputBusWidth> *action_out ) {
    RMT_ASSERT(GLOBAL_FALSE); // JBay only
  }

  bool uses_divide() {
    return GLOBAL_FALSE; // JBay only
  }

  static const char* nop1_addc_str() { return "Nop1"; }
  static const char* nop2_subc_str() { return "Nop2"; }

  // On Tofino is a Nop
  uint64_t nop1_addc(bool hilo, bool alu_1or2, int64_t a,int64_t b,int width, bool dual) {
    return 0;
  }
  // On Tofino is a Nop
  uint64_t nop2_subc(bool hilo, bool alu_1or2, int64_t a,int64_t b,int width, bool dual) {
    return 0;
  }
  void add(bool hilo, bool alu_1or2, int64_t a,int64_t b)  {} // nothing to do on Tofino
  void sub(bool hilo, bool alu_1or2, int64_t a,int64_t b)  {} // nothing to do on Tofino

  uint32_t get_reg_file_msbs( int addr ) {
    return 0; // JBay only
  }

 private:
  int alu_index_;
  register_classes::SaluInstrOutputAluArray out_reg_;

  // TODO: share the copy of this in mau-stateful-alu.h ??
  inline uint64_t sign_ext64(uint64_t val, int width) { return (64!=width && ((val >> (width-1)) & 1)) ? val | ~((UINT64_C(1) << width) - UINT64_C(1)) : val; }

};
}

#endif // _TOFINOXX_MAU_STATEFUL_ALU_
