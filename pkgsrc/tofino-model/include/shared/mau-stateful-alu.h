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

#ifndef _SHARED_MAU_STATEFUL_ALU_
#define _SHARED_MAU_STATEFUL_ALU_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <bitvector.h>

// Reg defs auto-generated from Semifore
#include <register_includes/salu_instr_common_array.h>
#include <register_includes/salu_const_regfile_array.h>
#include <register_includes/salu_instr_cmp_alu_array2.h>
#include <register_includes/salu_instr_state_alu_array2.h>
#include <register_includes/stateful_ctl.h>
#include <register_includes/salu_mathtable_array.h>
#include <register_includes/salu_mathunit_ctl.h>
#include <mau-chip-stateful-alu.h>
#include <model_core/log-buffer.h>

namespace MODEL_CHIP_NAMESPACE {

  class MauStatefulAlu : public MauObject {

    static constexpr int      kType = RmtTypes::kRmtTypeStatefulAlu;
    static constexpr int      kDataBusWidth = MauDefs::kDataBusWidth;
    static constexpr uint32_t kStatefulAluLogicalRows = MauDefs::kStatefulAluLogicalRows;
    static constexpr int      kActionOutputBusWidth = MauDefs::kActionOutputBusWidth;
    static constexpr int      kStatefulAluCmpAlus = MauDefs::kStatefulAluCmpAlus;
    static constexpr int      kStatefulAluTMatchAlus = MauDefs::kStatefulAluTMatchAlus;
    static constexpr int      kStatefulAluOutputAlus = MauDefs::kStatefulAluOutputAlus;
    static constexpr int      kStatefulMeterAluDataBits = MauDefs::kStatefulMeterAluDataBits;
    static constexpr int      kStatefulAluCmpConstWidth = MauDefs::kStatefulAluCmpConstWidth;
    static constexpr int      kNumStatefulAlus = MauDefs::kNumStatefulAlus;

    // Uarch doc v1.7.2 table 6.47 Stateful Comapre ALU ISA
    static constexpr int  kCmpAluOpEq    = 0b0000;
    static constexpr int  kCmpAluOpNe    = 0b0001;
    static constexpr int  kCmpAluOpDis   = 0b0010; // JBay only
    static constexpr int  kCmpAluOpGS    = 0b0100;
    static constexpr int  kCmpAluOpLES   = 0b0101;
    static constexpr int  kCmpAluOpGES   = 0b0110;
    static constexpr int  kCmpAluOpLS    = 0b0111;
    static constexpr int  kCmpAluOpGU    = 0b1000;
    static constexpr int  kCmpAluOpLEU   = 0b1001;
    static constexpr int  kCmpAluOpGEU   = 0b1010;
    static constexpr int  kCmpAluOpLU    = 0b1011;
    static constexpr int  kCmpAluOpGUUS  = 0b1100;
    static constexpr int  kCmpAluOpLEUUS = 0b1101;
    static constexpr int  kCmpAluOpGEUUS = 0b1110;
    static constexpr int  kCmpAluOpLUUS  = 0b1111;

    // Uarch doc v1.7.2 table 6.48 Stateful State-Update ALU Arithmetic ISA
    static constexpr int kSAddU  = 0b0000;
    static constexpr int kSAddS  = 0b0001;
    static constexpr int kSSubU  = 0b0010;
    static constexpr int kSSubS  = 0b0011;
    static constexpr int kMinU   = 0b0100;
    static constexpr int kMinS   = 0b0101;
    static constexpr int kMaxU   = 0b0110;
    static constexpr int kMaxS   = 0b0111;
    static constexpr int kNop1_AddC = 0b1000; // Nop Tofino / AddC JBay
    static constexpr int kNop2_SubC = 0b1001; // Nop Tofino / SubC JBay
    static constexpr int kSSubRU = 0b1010;
    static constexpr int kSSubRS = 0b1011;
    static constexpr int kAdd    = 0b1100;
    static constexpr int kNop3   = 0b1101;
    static constexpr int kSub    = 0b1110;
    static constexpr int kSubR   = 0b1111;
    static inline const char* ath_op_str(int op) { switch (op) {
        case kSAddU:  return "SatAddU";
        case kSAddS:  return "SatAddS";
        case kSSubU:  return "SatSubU";
        case kSSubS:  return "SatSubS";
        case kMinU:   return "MinU";
        case kMinS:   return "MinS";
        case kMaxU:   return "MaxU";
        case kMaxS:   return "MaxS";
        case kNop1_AddC:   return MauChipStatefulAlu::nop1_addc_str();
        case kNop2_SubC:   return MauChipStatefulAlu::nop2_subc_str();
        case kSSubRU: return "SatSubRU";
        case kSSubRS: return "SatSubRS";
        case kAdd:    return "Add";
        case kNop3:   return "Nop3";
        case kSub:    return "Sub";
        case kSubR:   return "SubR";
        default:      return "Illegal Op Code";
    } }

    // Uarch doc v1.7.2 table 6.49 Stateful State-Update ALU Logical ISA
    static constexpr int kSetZ  = 0b0000;
    static constexpr int kNor   = 0b0001;
    static constexpr int kAndCA = 0b0010;
    static constexpr int kNotA  = 0b0011;
    static constexpr int kAndCB = 0b0100;
    static constexpr int kNotB  = 0b0101;
    static constexpr int kXor   = 0b0110;
    static constexpr int kNand  = 0b0111;
    static constexpr int kAnd   = 0b1000;
    static constexpr int kXnor  = 0b1001;
    static constexpr int kB     = 0b1010;
    static constexpr int kOrCA  = 0b1011;
    static constexpr int kA     = 0b1100;
    static constexpr int kOrCB  = 0b1101;
    static constexpr int kOr    = 0b1110;
    static constexpr int kSetHi = 0b1111;
    static inline const char* log_op_str(int op) { switch (op) {
        case kSetZ:  return "LO";
        case kNor:   return "NOR";
        case kAndCA: return "ANDCA";
        case kNotA:  return "CA";
        case kAndCB: return "ANDCB";
        case kNotB:  return "CB";
        case kXor:   return "XOR";
        case kNand:  return "NAND";
        case kAnd:   return "AND";
        case kXnor:  return "XNOR";
        case kB:     return "B";
        case kOrCA:  return "OrCA";
        case kA:     return "A";
        case kOrCB:  return "OrCB";
        case kOr:    return "OR";
        case kSetHi: return "HI";
        default:     return "Illegal Op Code";
    } }

    // Uarch doc v1.7.2 table 6.50 Stateful ALU Special Purpose 1b Instructions
    static constexpr int kSetBit    = 0b0000;
    static constexpr int kSetBitC   = 0b0001;
    static constexpr int kClrBit    = 0b0010;
    static constexpr int kClrBitC   = 0b0011;
    static constexpr int kRdBit     = 0b0100;
    static constexpr int kRdBitC    = 0b0101;
    static constexpr int kSetBitAt  = 0b0110;
    static constexpr int kSetBitAtC = 0b0111;
    static constexpr int kClrBitAt  = 0b1000;
    static constexpr int kClrBitAtC = 0b1001;
    static inline const char* onebit_op_str(int op) { switch (op) {
        case kSetBit:    return "SetBit";
        case kSetBitC:   return "SetBitC";
        case kClrBit:    return "ClrBit";
        case kClrBitC:   return "ClrBitC";
        case kRdBit:     return "RdBit";
        case kRdBitC:    return "RdBitC";
        case kSetBitAt:  return "SetBitAt";
        case kSetBitAtC: return "SetBitAtC";
        case kClrBitAt:  return "ClrBitAt";
        case kClrBitAtC: return "ClrBitAtC";
        default:         return "Illegal Op Code";
    } }

    // From salu_instr_output_alu.salu_output_asrc CSR:
    static inline const char* out_op_str(int op) { switch (op) {
        case MauChipStatefulAlu::kOutMemHi : return "OutMemHi ";
        case MauChipStatefulAlu::kOutMemLo : return "OutMemLo ";
        case MauChipStatefulAlu::kOutPhvHi : return "OutPhvHi ";
        case MauChipStatefulAlu::kOutPhvLo : return "OutPhvLo ";
        case MauChipStatefulAlu::kOutAluHi : return "OutAluHi ";
        case MauChipStatefulAlu::kOutAluLo : return "OutAluLo ";
        case MauChipStatefulAlu::kOutCmp   : return "OutCmp   ";
        case MauChipStatefulAlu::kOutSAddr : return "OutSAddr ";
        case MauChipStatefulAlu::kOutDiv   : return "OutDiv   ";
        case MauChipStatefulAlu::kOutMod   : return "OutMod   ";
        case MauChipStatefulAlu::kPreIncDec: return "PreIncDec";
        default:        return "Illegal Op Code";
    } }

    static inline const char* cmp_op_str(int op) { switch (op) {
        case kCmpAluOpEq:    return "==";
        case kCmpAluOpNe:    return "!=";
        case kCmpAluOpDis:   return "Disabled";
        case kCmpAluOpGS:    return "S >";
        case kCmpAluOpLES:   return "S <=";
        case kCmpAluOpGES:   return "S >=";
        case kCmpAluOpLS:    return "S <";
        case kCmpAluOpGU:    return "U >";
        case kCmpAluOpLEU:   return "U <=";
        case kCmpAluOpGEU:   return "U >=";
        case kCmpAluOpLU:    return "U <";
        case kCmpAluOpGUUS:  return "UUS >";
        case kCmpAluOpLEUUS: return "UUS <=";
        case kCmpAluOpGEUUS: return "UUS >=";
        case kCmpAluOpLUUS:  return "UUS <";
        default:             return "Illegal Op Code";
    } }

    static constexpr int kOpTypeNone = 0;
    static constexpr int kOpTypeAth  = 1;
    static constexpr int kOpTypeLog  = 2;
    static constexpr int kOpType1Bit = 3;
    static constexpr int kOpTypeOut  = 4;
    static constexpr int kOpTypeCmp  = 5;
    static inline const int op_make(int optype, int op) {
      return ((optype << 8) | (op & 0xFF));
    }
    static inline const char* op_str(int op) { switch (op >> 8) {
        case kOpTypeAth:  return ath_op_str(op & 0xFF);
        case kOpTypeLog:  return log_op_str(op & 0xFF);
        case kOpType1Bit: return onebit_op_str(op & 0xFF);
        case kOpTypeOut:  return out_op_str(op & 0xFF);
        case kOpTypeCmp:  return cmp_op_str(op & 0xFF);
        default:          return "Illegal Op Code";
    } }


    static inline int get_stateful_alu_regs_index(int logrow) {
      int idx = __builtin_popcountl(kStatefulAluLogicalRows & ((1u<<(logrow+1))-1)) - 1;
      RMT_ASSERT(idx >= 0);
      return idx;
    }

 public:
    static bool kRelaxSaluPredRsiCheck; // Defined in rmt-config.cpp

    MauStatefulAlu(RmtObjectManager *om, int pipeIndex, int mauIndex,
                   int logicalRowIndex, Mau *mau);
    ~MauStatefulAlu();

    using StatefulBus = std::array<bool,kNumStatefulAlus>;

    void calculate_output(uint32_t addr, BitVector<kStatefulMeterAluDataBits> phv,
                          BitVector<kDataBusWidth> *data_in,
                          BitVector<kDataBusWidth> *data_out,
                          BitVector<kActionOutputBusWidth> *action_out,
                          uint64_t present_time,
                          bool ingress,
                          StatefulBus match_bus,
                          StatefulBus learn_or_match_bus,
                          uint32_t *sel_index);
    void calculate_output(uint32_t addr, BitVector<kStatefulMeterAluDataBits> phv,
                          BitVector<kDataBusWidth> *data_in,
                          BitVector<kDataBusWidth> *data_out,
                          BitVector<kActionOutputBusWidth> *action_out,
                          uint64_t present_time,
                          bool ingress,
                          StatefulBus match_bus,
                          StatefulBus learn_or_match_bus) {
      uint32_t sel_index = 0u;
      calculate_output(addr, phv, data_in, data_out, action_out, present_time,
                       ingress, match_bus, learn_or_match_bus, &sel_index);
    }
    void calculate_cmp_alu(uint32_t addr, uint64_t present_time,
                           BitVector<kStatefulMeterAluDataBits> phv,
                           BitVector<kDataBusWidth> *data_in);

    // this is needed by calculate_cmp_alu, but as DV does not call that
    //  use this to set it before calling either calculate_cmp_alu or calculate_output
    void set_random_number_value( uint64_t value ) {
      random_number_.valid = true;
      random_number_.value = value;
    }

    void set_check_total_correct(bool v) { check_total_correct_ = v; }

    void update_addresses(int old_addr, int new_addr);
    void update_last_config_write_data( BitVector<kDataBusWidth>& data) {
      last_config_write_data_.copy_from( data );
      RMT_LOG(RmtDebug::verbose(), "MauStatefulALU: update_last_config_write_data new_value = 0x%016" PRIx64 "_%016" PRIx64 "\n",
              last_config_write_data_.get_word(64),last_config_write_data_.get_word(0));
    }

    void reset_resources();

    bool get_match_output();
    bool get_learn_output();

    bool get_minmax_mask_was_zero() {
      return minmax_mask_was_zero_;
    }
    int get_minmax_index() {
      return minmax_index_;
    }

    bool uses_divide() {
      return chip_salu_.uses_divide();
    }


    std::list<std::pair<int, uint32_t>> get_p4_log_results() {
      std::pair <int, uint32_t> width(RMT_LOG_SALU_WIDTH, p4_log_op_width_);
      std::pair <int, uint32_t> index(RMT_LOG_SALU_INDEX, p4_log_index_);
      std::pair <int, uint32_t> register_lo(RMT_LOG_SALU_REGISTER_LO, p4_log_register_lo_);
      std::pair <int, uint32_t> register_hi(RMT_LOG_SALU_REGISTER_HI, p4_log_register_hi_);
      std::pair <int, uint32_t> phv_lo(RMT_LOG_SALU_PHV_LO, p4_log_phv_lo_);
      std::pair <int, uint32_t> phv_hi(RMT_LOG_SALU_PHV_HI, p4_log_phv_hi_);
      std::pair <int, uint32_t> cond_input_ram_hi(RMT_LOG_SALU_COND_INPUT_RAM_HI, p4_log_cond_input_ram_hi_);
      std::pair <int, uint32_t> cond_input_ram_lo(RMT_LOG_SALU_COND_INPUT_RAM_LO, p4_log_cond_input_ram_lo_);
      std::pair <int, uint32_t> cond_input_phv_hi(RMT_LOG_SALU_COND_INPUT_PHV_HI, p4_log_cond_input_phv_hi_);
      std::pair <int, uint32_t> cond_input_phv_lo(RMT_LOG_SALU_COND_INPUT_PHV_LO, p4_log_cond_input_phv_lo_);
      std::pair <int, uint32_t> cond_output_lo(RMT_LOG_SALU_COND_OUTPUT_LO, p4_log_cond_output_lo_);
      std::pair <int, uint32_t> cond_output_hi(RMT_LOG_SALU_COND_OUTPUT_HI, p4_log_cond_output_hi_);
      std::pair <int, uint32_t> update_lo_1_pred(RMT_LOG_SALU_UPDATE_LO_1_PRED, p4_log_update_lo_1_predicate_);
      std::pair <int, uint32_t> update_lo_1_result(RMT_LOG_SALU_UPDATE_LO_1_RESULT, p4_log_update_lo_1_result_);
      std::pair <int, uint32_t> update_lo_2_pred(RMT_LOG_SALU_UPDATE_LO_2_PRED, p4_log_update_lo_2_predicate_);
      std::pair <int, uint32_t> update_lo_2_result(RMT_LOG_SALU_UPDATE_LO_2_RESULT, p4_log_update_lo_2_result_);
      std::pair <int, uint32_t> update_hi_1_pred(RMT_LOG_SALU_UPDATE_HI_1_PRED, p4_log_update_hi_1_predicate_);
      std::pair <int, uint32_t> update_hi_1_result(RMT_LOG_SALU_UPDATE_HI_1_RESULT, p4_log_update_hi_1_result_);
      std::pair <int, uint32_t> update_hi_2_pred(RMT_LOG_SALU_UPDATE_HI_2_PRED, p4_log_update_hi_2_predicate_);
      std::pair <int, uint32_t> update_hi_2_result(RMT_LOG_SALU_UPDATE_HI_2_RESULT, p4_log_update_hi_2_result_);
      std::pair <int, uint32_t> new_ram_hi(RMT_LOG_SALU_NEW_RAM_HI, p4_log_new_ram_hi_);
      std::pair <int, uint32_t> new_ram_lo(RMT_LOG_SALU_NEW_RAM_LO, p4_log_new_ram_lo_);
      std::pair <int, uint32_t> output_pred(RMT_LOG_SALU_OUTPUT_PRED, p4_log_output_predicate_);
      std::pair <int, uint32_t> output_result(RMT_LOG_SALU_OUTPUT_RESULT, p4_log_output_result_);

      p4_log_value_list_.clear(); // Clear any previous items in the list because
                                  // prior salu execution results need to cleared.
                                  // they are already logged.
      p4_log_value_list_.push_back(width);
      p4_log_value_list_.push_back(index);
      p4_log_value_list_.push_back(register_lo);
      p4_log_value_list_.push_back(register_hi);
      p4_log_value_list_.push_back(phv_lo);
      p4_log_value_list_.push_back(phv_hi);
      p4_log_value_list_.push_back(cond_input_ram_hi);
      p4_log_value_list_.push_back(cond_input_ram_lo);
      p4_log_value_list_.push_back(cond_input_phv_hi);
      p4_log_value_list_.push_back(cond_input_phv_lo);
      p4_log_value_list_.push_back(cond_output_lo);
      p4_log_value_list_.push_back(cond_output_hi);
      p4_log_value_list_.push_back(update_lo_1_pred);
      p4_log_value_list_.push_back(update_lo_1_result);
      p4_log_value_list_.push_back(update_lo_2_pred);
      p4_log_value_list_.push_back(update_lo_2_result);
      p4_log_value_list_.push_back(update_hi_1_pred);
      p4_log_value_list_.push_back(update_hi_1_result);
      p4_log_value_list_.push_back(update_hi_2_pred);
      p4_log_value_list_.push_back(update_hi_2_result);
      p4_log_value_list_.push_back(new_ram_lo);
      p4_log_value_list_.push_back(new_ram_hi);
      p4_log_value_list_.push_back(output_pred);
      p4_log_value_list_.push_back(output_result);

      return (p4_log_value_list_);
    }


 private:
    inline uint32_t sign_ext(uint32_t val, int width) { return (32!=width && ((val >> (width-1)) & 1)) ? val | ~((1u << width) - 1u) : val; }
    inline uint64_t sign_ext64(uint64_t val, int width) { return (64!=width && ((val >> (width-1)) & 1)) ? val | ~((UINT64_C(1) << width) - UINT64_C(1)) : val; }
    int decode_meter_type_to_instr(int meter_type);
    void get_width_and_mask(int instr_addr);
    bool cmp_op_is_signed(int op) { return kCmpAluOpGS == op || kCmpAluOpLES == op || kCmpAluOpGES == op || kCmpAluOpLS == op; }
    bool cmp_op_is_unsigned(int op) { return kCmpAluOpGU == op || kCmpAluOpLEU == op || kCmpAluOpGEU == op || kCmpAluOpLU == op; }

    bool run_cmp(int which_alu);
    uint64_t run_state_lo_1(uint64_t phv_hi, uint64_t phv_lo, uint64_t ram_hi,
                            uint64_t ram_lo, int instr_addr);
    uint64_t run_state_lo_2(uint64_t phv_hi, uint64_t phv_lo, uint64_t ram_hi,
                            uint64_t ram_lo,
                            uint32_t math_ram_hi, uint32_t math_ram_lo,
                            int instr_addr);
    uint64_t run_state_hi_1(uint64_t phv_hi, uint64_t phv_lo, uint64_t ram_hi,
                            uint64_t ram_lo, int instr_addr);
    uint64_t run_state_hi_2(uint64_t phv_hi, uint64_t phv_lo, uint64_t ram_hi,
                            uint64_t ram_lo, int instr_addr, int64_t *as_out, int64_t *bs_out);
    uint64_t run_state(bool hilo, bool alu_1or2, uint64_t phv_hi,
                       uint64_t phv_lo, uint64_t ram_hi, uint64_t ram_lo,
                       uint32_t math_ram_hi, uint32_t math_ram_lo,
                       int instr_addr, int64_t *as_out, int64_t *bs_out);
    uint32_t run_math(uint64_t phv_hi, uint64_t phv_lo,
                      uint32_t math_ram_hi, uint32_t math_ram_lo, int instr_addr);
    bool run_state_one_bit(int addr, int instr_addr,
                           BitVector<kDataBusWidth> *data_in,
                           BitVector<kDataBusWidth> *data_out);
    bool check_bit_pos(int bit_pos) {
      if (bit_pos < 120) {
        return true;
      } else {
        if ( check_total_correct_ ) {
          RMT_ASSERT(0);
        }
        return false;
      }
    }

    void reset_alu_state() {
      cmp_ops_[0] = cmp_ops_[1] = cmp_ops_[2] = cmp_ops_[3] = 0;
      state_ops_[0] = state_ops_[1] = state_ops_[2] = state_ops_[3] = 0;
      for (int out_alu=0;out_alu<kStatefulAluOutputAlus;++out_alu) {
        out_op_[out_alu] = 0;
      }
      math_input_data_ = UINT64_C(0);
      math_in_shift_ = 0;
      math_table_data_ = 0;
      math_out_shift_ = 0;
      math_unit_executed_ = false;
      math_output_data_ = 0u;
    }
    uint64_t get_cmp_constant(bool use_reg_file,
                              int instr_addr,
                              int width,
                              int which_alu,
                              uint64_t mask,
                              model_core::LogBuffer& log_buf,
                              const char *const constant_type = "" /* for logging */);

    void decode_inputs( uint32_t addr, uint64_t present_time ,
                        BitVector<kStatefulMeterAluDataBits> phv,
                        BitVector<kDataBusWidth> *data_in );

 private:
    int                                       alu_index_;
    MauChipStatefulAlu                        chip_salu_;
    register_classes::SaluConstRegfileArray   reg_file_;
    register_classes::SaluInstrCmpAluArray2   cmp_reg_;
    register_classes::SaluInstrCommonArray    com_reg_;
    register_classes::SaluInstrStateAluArray2 state_reg_;
    register_classes::StatefulCtl             stateful_ctl_;
    register_classes::SaluMathunitCtl         math_ctl_;
    register_classes::SaluMathtableArray      math_tbl_;

    // These are just for Logging (P4?)
    int      cmp_ops_[4];
    int      state_ops_[4];
    int      out_op_[kStatefulAluOutputAlus];
    uint64_t math_input_data_;
    uint8_t  math_in_shift_;
    uint8_t  math_table_data_;
    int8_t   math_out_shift_;
    bool     math_unit_executed_;
    uint32_t math_output_data_;

    bool check_total_correct_ = true;
    bool last_address_moved_  = false;
    uint32_t last_address_old_value_ = 0;
    BitVector<kDataBusWidth> last_config_write_data_{ UINT64_C(0) };
    BitVector<kDataBusWidth> last_data_in{ UINT64_C(0) };
    BitVector<kDataBusWidth> last_data_out{ UINT64_C(0) };
    uint64_t last_ram_lo=0;
    uint64_t last_ram_hi=0;
    int last_word_addr_=0;
    int last_subword_=0;
    uint64_t last_time_=0;
    bool last_was_min_max_=false;
    bool     cmp_alus_have_run_ = false;
    struct {
      uint64_t value;
      bool valid = false;
    } random_number_;
    bool minmax_mask_was_zero_=false;
    int minmax_index_=0;

    char cmp_name_[4][3] = {}; // for logging

    struct {
      bool valid = false;
      int instr_addr;
      int width;      // will be 64 for 128
      int real_width; // will be 128 for 128
      uint64_t mask;
      bool dbl;
      // In Tofino the max width=32, so top 32bits of all these
      //   uint64_t's will be zero
      uint64_t phv_lo;
      uint64_t phv_hi;
      uint64_t ram_lo;
      uint64_t ram_hi;
      uint64_t cmp_ram_lo;
      uint64_t cmp_ram_hi;
      int word_addr;
      int subword;
      int subword_width;
      bool same_word_as_last_cycle=false;
      bool same_subword_as_last_cycle=false;
      bool stateful_clear = false;
      bool is_run_stateful = false;
      bool min_max_enable = false;
    } decoded_inputs_ ;

    // TMatch in cmp structure: can't have its own array because there are none on Tofino
    static_assert( kStatefulAluTMatchAlus <= kStatefulAluCmpAlus ,
                   "Code assumes kStatefulAluTMatchAlus <= kStatefulAluCmpAlus" );
    struct {
      bool valid = false;
      bool cmp_out=false;
      bool tmatch_match=false;
      bool tmatch_learn=false;
    } cmp_alu_out_[ kStatefulAluCmpAlus ];

    // input and result of SALU operation are saved
    // for P4 logging.
    int       p4_log_op_width_{0};
    uint32_t  p4_log_index_{0};
    uint32_t  p4_log_register_lo_{0};
    uint32_t  p4_log_register_hi_{0};
    uint32_t  p4_log_phv_lo_{0};
    uint32_t  p4_log_phv_hi_{0};
    uint32_t  p4_log_cond_input_ram_hi_{0};
    uint32_t  p4_log_cond_input_ram_lo_{0};
    uint32_t  p4_log_cond_input_phv_hi_{0};
    uint32_t  p4_log_cond_input_phv_lo_{0};
    bool      p4_log_cond_output_lo_{false};
    bool      p4_log_cond_output_hi_{false};
    bool      p4_log_update_lo_1_predicate_{false};
    uint32_t  p4_log_update_lo_1_result_{0};
    bool      p4_log_update_lo_2_predicate_{false};
    uint32_t  p4_log_update_lo_2_result_{0};
    bool      p4_log_update_hi_1_predicate_{false};
    uint32_t  p4_log_update_hi_1_result_{0};
    bool      p4_log_update_hi_2_predicate_{false};
    uint32_t  p4_log_update_hi_2_result_{0};
    uint32_t  p4_log_new_ram_hi_{0};
    uint32_t  p4_log_new_ram_lo_{0};
    bool      p4_log_output_predicate_{false};
    uint32_t  p4_log_output_result_{0};
    std::list<std::pair<int, uint32_t>> p4_log_value_list_;
  };
}

#endif // _SHARED_MAU_STATEFUL_ALU_
