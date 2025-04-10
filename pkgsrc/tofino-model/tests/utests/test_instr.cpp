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

#include <utests/test_util.h>
#include <iostream>
#include <string>
#include <array>
#include <cassert>

#include "gtest.h"

#include <rmt-object-manager.h>
#include <instr.h>
#include <phv-factory.h>
#include <mau.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

#ifdef MODEL_CHIP_JBAY_OR_LATER

//TODO:JBAY Should switch over to macro below once
//TODO:JBAY mau-defs.h is fixed up to have Src2 width 5
#define INS_COMMON_BITS(src1i_, src1_, src2_) \
  (BH_SET(10,10,src1i_) | BH_SET(9,5,src1_) | BH_SET(4,0,src2_))

// OLD MACRO
//#define INS_COMMON_BITS(src1i_, src1_, src2_)
//  (BH_SET(9,9,src1i_) | BH_SET(8,4,src1_) | BH_SET(3,0,src2_))
#define JBAY (true)
#else
#define INS_COMMON_BITS(src1i_, src1_, src2_) \
  (BH_SET(9,9,src1i_) | BH_SET(8,4,src1_) | BH_SET(3,0,src2_))
#define JBAY (false)
#endif
  bool jbay = JBAY;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


  TEST(BFN_TEST_NAME(InstrTest),Basic) {
    Instr instrs;

    // Ensure reset clears.
    instrs.reset();
    for (int i=0; i<Instr::kInstrsMax; ++i) {ASSERT_TRUE(!instrs.get(i));}

    // Ensure set sets.
    for (uint32_t i=0; i<Instr::kInstrsMax; ++i) {
      instrs.set(i, ~i);
    }
    for (uint32_t i=0; i<Instr::kInstrsMax; ++i) {
      ASSERT_TRUE(~i == instrs.get(i));
    }

    // Ensure masking to the instruction length works.
    for (uint32_t i=0; i<Instr::kInstrsMax; ++i) {
      if (Instr::InstrWidth::kInstrSz32 == instrs.getWidth(i)) {
        ASSERT_TRUE((~i & Instr::kInstrMask32) == (Instr::kInstrMask32 & instrs.get(i)));
      } else if (Instr::InstrWidth::kInstrSz16 == instrs.getWidth(i)) {
        ASSERT_TRUE((~i & Instr::kInstrMask16) == (Instr::kInstrMask16 & instrs.get(i)));
      } else if (Instr::InstrWidth::kInstrSz8  == instrs.getWidth(i)) {
        ASSERT_TRUE((~i & Instr::kInstrMask8) == (Instr::kInstrMask8 & instrs.get(i)));
      } else {
        ASSERT_TRUE(false);
      }
    }

    // Ensure width detection is correct.
    int G = Instr::kInstrAluGrpSize;
    for (int i=0; i<Instr::kInstrsMax; ++i) {
      ASSERT_TRUE(((i <  (4*G)) &&
                   (Instr::InstrWidth::kInstrSz32 == instrs.getWidth(i))) ||
                  ((i > ((4*G)-1)) && (i < (8*G)) &&
                   (Instr::InstrWidth::kInstrSz8  == instrs.getWidth(i))) ||
                  ((i > ((8*G)-1)) && (i < (14*G)) &&
                   (Instr::InstrWidth::kInstrSz16 == instrs.getWidth(i))));
    }
  }


  TEST(BFN_TEST_NAME(InstrTest),OpCode) {
    Instr instrs;
    // For every instruction location...(not s1/s2 operand-mux-only ALUs though)
    for (int i=0; i<Instr::kInstrsMax; ++i) {
      if (Instr::isOperandMuxOnlyAlu(i)) continue;
      // For every 10 bit opcode possible...
      for (uint32_t val=0; val<=0x3FF; ++val) {
        uint32_t op_bits = 0;
        int s = Instr::kOpCodeShift;
        // NOP
        instrs.reset();
        op_bits = (val & 0x3F0) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kNop == instrs.getOpCode(i));
        // Load Const
        instrs.reset();
        op_bits = ((val & 0x3F0) | 0x008) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kLoadConst == instrs.getOpCode(i));
        // Funnel Shift
        instrs.reset();
        op_bits = ((val & 0x3E0) | 0x004) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kFunnelShift == instrs.getOpCode(i));

        uint32_t sh_mask = (jbay) ?0x3C0 :0x3E0;
        // SHL
        instrs.reset();
        op_bits = ((val & sh_mask) | 0x00C) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kShl == instrs.getOpCode(i));
        // SHRu
        instrs.reset();
        op_bits = ((val & sh_mask) | 0x014) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kShru == instrs.getOpCode(i));
        // SHRs
        instrs.reset();
        op_bits = ((val & sh_mask) | 0x01C) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kShrs == instrs.getOpCode(i));
        if (jbay) {
          // SHL (data-dependent)
          instrs.reset();
          op_bits = ((val & 0x3C0) | 0x02C) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kShlDataDep == instrs.getOpCode(i));
          // SHRu (data-dependent)
          instrs.reset();
          op_bits = ((val & 0x3C0) | 0x034) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kShruDataDep == instrs.getOpCode(i));
          // SHRs (data-dependent)
          instrs.reset();
          op_bits = ((val & 0x3C0) | 0x03C) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kShrsDataDep == instrs.getOpCode(i));
        }
        // Pair-DPF
        instrs.reset();
        op_bits = ((val & 0x3F0) | 0x002) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kPairDpf == instrs.getOpCode(i));
        // Byte Rotate Merge
        instrs.reset();
        op_bits = ((val & 0x3F0) | 0x00A) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kByteRotateMerge == instrs.getOpCode(i));
        // Conditional Mux
        instrs.reset();
        op_bits = ((val & 0x3E0) | 0x006) << s;
        instrs.set(i, op_bits);
        if (jbay) {
          // Conditional MUX combined into Conditional MOVE on JBay
          ASSERT_TRUE(Instr::InstrType::kConditionalMove == instrs.getOpCode(i));
        } else {
          ASSERT_TRUE(Instr::InstrType::kConditionalMux == instrs.getOpCode(i));
        }
        // Conditional Move
        instrs.reset();
        op_bits = ((val & 0x3E0) | 0x016) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kConditionalMove == instrs.getOpCode(i));
        // Invalidate
        instrs.reset();
        op_bits = ((val & 0x3C0) | 0x00E) << s;
        instrs.set(i, op_bits);
        if (jbay) {
          // No Invalidate on JBay - should show up as BitMaskedSet instead
          ASSERT_TRUE(Instr::InstrType::kBitMaskedSet == instrs.getOpCode(i));
        } else {
          ASSERT_TRUE(Instr::InstrType::kInvalidate == instrs.getOpCode(i));
        }
        // Bit Masked Set
        instrs.reset();
        int opcode = (jbay) ?0x00E :0x02E; // Diff opcode on JBay
        op_bits = ((val & 0x3C0) | opcode) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kBitMaskedSet == instrs.getOpCode(i));
        // ALU Set Zero
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x01E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluSetZ == instrs.getOpCode(i));
        // ALU NOR
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x05E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluNor == instrs.getOpCode(i));
        // ALU And !A
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x09E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluAndCA == instrs.getOpCode(i));
        // ALU !A
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x0DE) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluNotA == instrs.getOpCode(i));
        // ALU And !B
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x11E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluAndCB == instrs.getOpCode(i));
        // ALU  !B
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x15E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluNotB == instrs.getOpCode(i));
        // ALU XOR
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x19E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluXor == instrs.getOpCode(i));
        // ALU NAND
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x1DE) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluNand == instrs.getOpCode(i));
        // ALU AND
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x21E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluAnd == instrs.getOpCode(i));
        // ALU XNOR
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x25E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluXNor == instrs.getOpCode(i));
        // ALU B
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x29E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluB == instrs.getOpCode(i));
        // ALU OR !B
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x2DE) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluOrCA == instrs.getOpCode(i));
        // ALU A
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x31E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluA == instrs.getOpCode(i));
        // ALU OR !A
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x35E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluOrCB == instrs.getOpCode(i));
        // ALU OR
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x39E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluOr == instrs.getOpCode(i));
        // ALU Set High
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x3DE) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAluSetHi == instrs.getOpCode(i));
        // Arithmetic Compare OPS only on JBay
        if (jbay) {
          // Greater Than or Equal Unsigned
          instrs.reset();
          op_bits = ((val & 0xC00) | 0x02E) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kGtEqU == instrs.getOpCode(i));
          // Greater Than or Equal Signed
          instrs.reset();
          op_bits = ((val & 0xC00) | 0x06E) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kGtEqS == instrs.getOpCode(i));
          // Less Than Unsigned
          instrs.reset();
          op_bits = ((val & 0xC00) | 0x0AE) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kLtU == instrs.getOpCode(i));
          // Less Than Signed
          instrs.reset();
          op_bits = ((val & 0xC00) | 0x0EE) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kLtS == instrs.getOpCode(i));
          // Less Than or Equal Unsigned
          instrs.reset();
          op_bits = ((val & 0xC00) | 0x12E) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kLtEqU == instrs.getOpCode(i));
          // Less Than or Equal Signed
          instrs.reset();
          op_bits = ((val & 0xC00) | 0x16E) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kLtEqS == instrs.getOpCode(i));
          // Greater Than Unsigned
          instrs.reset();
          op_bits = ((val & 0xC00) | 0x1AE) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kGtU == instrs.getOpCode(i));
          // Greater Than Signed
          instrs.reset();
          op_bits = ((val & 0xC00) | 0x1EE) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kGtS == instrs.getOpCode(i));
          // Equal
          instrs.reset();
          op_bits = ((val & 0xC00) | 0x22E) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kEq == instrs.getOpCode(i));
          // Not Equal
          instrs.reset();
          op_bits = ((val & 0xC00) | 0x2AE) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kNEq == instrs.getOpCode(i));
          // Equal 64b
          instrs.reset();
          op_bits = ((val & 0xC00) | 0x26E) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kEq64 == instrs.getOpCode(i));
          // Not Equal 64b
          instrs.reset();
          op_bits = ((val & 0xC00) | 0x2EE) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kNEq64 == instrs.getOpCode(i));
        }
        // Saturating Add Unsigned
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x03E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kSAddU == instrs.getOpCode(i));
        // Saturating Add Signed
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x07E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kSAddS == instrs.getOpCode(i));
        // Saturating Sub Unsigned
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x0BE) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kSSubU == instrs.getOpCode(i));
        // Saturating Sub Signed
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x0FE) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kSSubS == instrs.getOpCode(i));
        // Min Unsigned
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x13E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kMinU == instrs.getOpCode(i));
        // Min Signed
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x17E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kMinS == instrs.getOpCode(i));
        // Max Unsigned
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x1BE) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kMaxU == instrs.getOpCode(i));
        // Max Signed
        instrs.reset();
        op_bits = ((val & 0xC00) | 0x1FE) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kMaxS == instrs.getOpCode(i));
        // Add
        instrs.reset();
        op_bits = ((val & 0xC40) | 0x23E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAdd == instrs.getOpCode(i));
        // Add-Carry
        instrs.reset();
        op_bits = ((val & 0xC40) | 0x2BE) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kAddC == instrs.getOpCode(i));

        uint32_t sub_mask = (jbay) ?0xC00 :0xC40;
        // Sub
        instrs.reset();
        op_bits = ((val & sub_mask) | 0x33E) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kSub == instrs.getOpCode(i));
        if (jbay) {
          // Sub Reverse
          instrs.reset();
          op_bits = ((val & 0xC00) | 0x37E) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kSubR == instrs.getOpCode(i));
        }
        // Sub-Carry
        instrs.reset();
        op_bits = ((val & sub_mask) | 0x3BE) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kSubC == instrs.getOpCode(i));
        if (jbay) {
          // Sub-Carry Reverse
          instrs.reset();
          op_bits = ((val & 0xC00) | 0x3FE) << s;
          instrs.set(i, op_bits);
          ASSERT_TRUE(Instr::InstrType::kSubRC == instrs.getOpCode(i));
        }
        // Deposit Field
        instrs.reset();
        op_bits = ((val & 0x3FE) | 0x001) << s;
        instrs.set(i, op_bits);
        ASSERT_TRUE(Instr::InstrType::kDepositField == instrs.getOpCode(i));
      }
    }
  }


  TEST(BFN_TEST_NAME(InstrTest),OpCodeMuxOnly) {
    Instr instrs;
    // For every instruction location...
    for (int i=0; i<Instr::kInstrsMax; ++i) {
      // For every 10 bit value possible (unshifted)
      for (uint32_t op=0; op<=0x3FF; ++op) {
        instrs.reset();
        instrs.set(i, op);
        if (Instr::isOperand1OnlyAlu(i)) {
          if (Instr::isOperand1Enabled(op)) {
            ASSERT_TRUE(Instr::InstrType::kOperand1 == instrs.getOpExtended(i,true));
            ASSERT_TRUE(Instr::InstrType::kOperand1 == instrs.getOpCode(i));
          } else {
            ASSERT_TRUE(Instr::InstrType::kNop == instrs.getOpExtended(i,true));
            ASSERT_TRUE(Instr::InstrType::kNop == instrs.getOpCode(i));
          }
        } else if (Instr::isOperand2OnlyAlu(i)) {
          if (Instr::isOperand2Enabled(op)) {
            ASSERT_TRUE(Instr::InstrType::kOperand2 == instrs.getOpExtended(i,true));
            ASSERT_TRUE(Instr::InstrType::kOperand2 == instrs.getOpCode(i));
          } else {
            ASSERT_TRUE(Instr::InstrType::kNop == instrs.getOpExtended(i,true));
            ASSERT_TRUE(Instr::InstrType::kNop == instrs.getOpCode(i));
          }
        } else {
          // Should definitely NOT be kOperand1 or kOperand2
          ASSERT_FALSE(Instr::InstrType::kOperand1 == instrs.getOpExtended(i,false));
          ASSERT_FALSE(Instr::InstrType::kOperand2 == instrs.getOpExtended(i,false));
          ASSERT_FALSE(Instr::InstrType::kOperand1 == instrs.getOpCode(i));
          ASSERT_FALSE(Instr::InstrType::kOperand2 == instrs.getOpCode(i));
        }
      }
    }
  }

  // Helper function for the operand test.
  uint32_t computeOp1(int instr_idx, bool src1i, int src1,
                 BitVector<MauDefs::kActionHVOutputBusWidth> &adib, Phv *phv,
                 bool &error) {
    int G = Instr::kInstrAluGrpSize;
    if (src1i) { // From ADIB
      error = false;
      if (instr_idx < (G*4)) {
        return (adib.get_byte(4*src1+3) << 24) |
               (adib.get_byte(4*src1+2) << 16) |
               (adib.get_byte(4*src1+1) << 8) |
               adib.get_byte(4*src1);
      } else if (instr_idx < (G*8)) {
        return adib.get_byte(src1);
      } else {
        return (adib.get_byte(32 + 2*src1+1) << 8) |
               adib.get_byte(32 + 2*src1);
      }
    } else if (src1 >= G) { // Immediate
      error = false;
      return src1 - 24;
    } else { // From PHV
      error = !phv->is_valid((instr_idx/G)*G + (src1%G));
      return phv->get((instr_idx/G)*G + (src1%G));
    }
    return 0;
  }
  // Helper function for the operand test.
  uint32_t computeOp2(int instr_idx, int src2, Phv *phv, bool &error) {
    int G = Instr::kInstrAluGrpSize;
    error = !phv->is_valid((instr_idx/G)*G + (src2%G));
    return phv->get((instr_idx/G)*G + (src2%G));
  }


  TEST(BFN_TEST_NAME(InstrTest),Operand) {
    Instr instrs;
    // Get a dummy PHV and fill it with a fixed pattern.
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    for (int i=0; i<Phv::kWordsMax; ++i) {
      if (32 == phv->which_width(i)) {
        phv->set(i, -1*i);
      } else if (8 == phv->which_width(i)) {
        phv->set(i, 0x80 | i);
      } else {
        phv->set(i, (i << 8) | i);
      }
      //RMT_UT_LOG_INFO("PHV[%3d] = %d", i, phv->get(i));
    }
    //for (int i=1; i<Phv::kWordsMax; i+=2) phv->set_error(i);

    // Get a dummy bit vector and fill it with a fixed pattern.
    std::array<uint8_t,128> pattern;
    for (int i=0; i<128; ++i) {
      pattern[i] = 128 - i;
    }
    BitVector<MauDefs::kActionHVOutputBusWidth> adib(pattern);

    //
    // Basic Operands
    //

    // For every instruction location...
    for (int ins=0; ins<Instr::kInstrsMax; ++ins) {
      if (Instr::isOperandMuxOnlyAlu(ins)) continue;
      // For each combination of Src1 type, Src1 value, and Src2 value...
      for (int i=0; i<=0x1; ++i) {
        for (int j=0; j<32; ++j) {
          for (int k=0; k<=0xF; ++k) {
            // Build the instruction common bits.
            uint32_t val = INS_COMMON_BITS(i, j, k);
            instrs.reset();
            instrs.set(ins, val);
            bool error_x = false, error_y = false;
            uint32_t x = instrs.getOperand1(ins, phv, adib, error_x);
            uint32_t y = instrs.getOperand2(ins, phv, error_y);

            // Validate.
            bool error_x_val = false, error_y_val = false;
            uint32_t x_val = computeOp1(ins, i, j, adib, phv, error_x_val);
            uint32_t y_val = computeOp2(ins, k, phv, error_y_val);
            ASSERT_TRUE(error_x_val == error_x);
            ASSERT_TRUE(error_y_val == error_y);
            ASSERT_TRUE((!error_x && x == x_val) || (error_x && x == 0));
            ASSERT_TRUE((!error_y && y == y_val) || (error_y && y == 0));
          }
        }
      }
    }

    //
    // Merged Operands
    //
    if (Instr::kInstrPairDpfIsErr) return;

#if I_AM_VERY_PATIENT
    // For every instruction location...
    for (int ins=0; ins<Instr::kInstrsMax; ins+=2) {
      if (Instr::isOperandMuxOnlyAlu(ins)) continue;
      // For each combination of Src1 type, Src1 value, and Src2 value on the
      // even ALU
      for (int i=0; i<=0x1; ++i) {
        for (int j=0; j<32; ++j) {
          for (int k=0; k<=0xF; ++k) {
            // For each combination of Src1 type, Src1 value, and Src2 value on
            // the odd ALU
            for (int I=0; I<=0x1; ++I) {
              for (int J=0; (I && J<40) || (!I && J<=63); ++J) {
                for (int K=0; K<=0xF; ++K) {
#else
    // For an instruction pair within each size group
    for (int iii=3; iii<Instr::kGroups; iii+=3) {
      int ins = iii * Instr::kInstrAluGrpSize;
      if (Instr::isOperandMuxOnlyAlu(ins)) continue; // Just in case
      // For some combinations of Src1 type, Src1 value, and Src2 value on the
      // even ALU
      for (int i=0; i<=0x1; ++i) {
        for (int j=0; j<32; ++j) {
          for (int k=0; k<=0xF; k+=5) {
            // For some combinations of Src1 type, Src1 value, and Src2 value on
            // the odd ALU
            for (int I=0; I<=0x1; ++I) {
              for (int J=0; J<32; J+=2) {
                for (int K=0; K<=0xF; K+=6) {
#endif
                  instrs.reset();
                  // Build the instruction common bits.  Include an opcode for
                  // the odd ALU so that they will merge.
                  uint32_t val_even = (4 << Instr::kOpCodeShift) | INS_COMMON_BITS(i, j, k);
                  instrs.set(ins, val_even);
                  uint32_t val_odd  = (0x2 << Instr::kOpCodeShift) | INS_COMMON_BITS(I, J, K);
                  instrs.set(ins+1, val_odd);
                  if (iii < 4) {
                    // 32 Bit ALUs have no special behavior when merged.
                    // Verify each ALU in the pair independently.
                    for (int pair=0; pair<2; ++pair) {
                      bool error_x = false, error_y = false;
                      int32_t x = 0, y = 0;
                      instrs.getMergedOperands(ins+pair, phv, adib, x, y, error_x, error_y);
                      bool error_x_val = false, error_y_val = false;
                      int32_t x_val = computeOp1(ins+pair, pair ? I:i, pair ? J:j, adib, phv, error_x_val);
                      int32_t y_val = computeOp2(ins+pair, pair ? K:k, phv, error_y_val);
                      ASSERT_TRUE(error_x_val == error_x);
                      ASSERT_TRUE(error_y_val == error_y);
                      ASSERT_TRUE((!error_x && x == x_val) || (error_x && x == 0));
                      ASSERT_TRUE((!error_y && y == y_val) || (error_y && y == 0));
                    }
                  } else {
                    // For merged 8-bit ALUs verify them as a single 16 bit ALU.
                    // For merged 16 bit ALUs, verify them as a single 32 bit ALU.
                    bool error_x = false, error_y = false;
                    int32_t x = 0, y = 0;
                    instrs.getMergedOperands(ins, phv, adib, x, y, error_x, error_y);

                    bool error_x_val = false, error_y_val = false;
                    int32_t x_val = computeOp1(ins+1, I, J, adib, phv, error_x_val);
                    int32_t y_val = computeOp2(ins+1, K, phv, error_y_val);
                    int shift = (iii < 8) ? 8 : 16;
                    if (!error_x_val) {
                      x_val = (x_val << shift) | (computeOp1(ins, i, j, adib, phv, error_x_val) & ((1<<shift)-1));
                    }
                    if (!error_y_val) {
                      y_val = (y_val << shift) | (computeOp2(ins, k, phv, error_y_val) & ((1<<shift)-1));
                    }
                    ASSERT_TRUE(error_x_val == error_x);
                    ASSERT_TRUE(error_y_val == error_y);
                    if (!((!error_x && x == x_val) || (error_x && x == 0))) {
                      printf("error_x %d, x 0x%x, x_expected 0x%x\n", error_x, x, x_val);
                    }
                    ASSERT_TRUE((!error_x && x == x_val) || (error_x && x == 0));
                    ASSERT_TRUE((!error_y && y == y_val) || (error_y && y == 0));
                  }
                }
              }
            }
          }
        }
      }
    }
    om->phv_delete(phv);
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),LoadConst) {
    Instr instrs;
    // Get a dummy PHV
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();

    // Get a dummy bit vector.
    std::array<uint8_t,128> pattern{};
    BitVector<MauDefs::kActionHVOutputBusWidth> adib(pattern);

    uint32_t v[] = {~0u, 0x3FFFFF, 0xFFFF, 0xFF, 0xF, 0, 0x12345};
    instrs.reset();
    for (int iii=0; iii<Instr::kGroups; iii+=2) {
      int ins = iii * Instr::kInstrAluGrpSize;
      for (unsigned j=0; j<sizeof(v)/sizeof(v[0]); ++j) {
        uint32_t c = 0;
        // To allow for arb size v - don't bother trying if no proper ALU i+j
        if (Instr::isOperandMuxOnlyAlu(ins+j)) continue;
        Instr::InstrWidth w = instrs.getWidth(ins+j);
        int opbits = Instr::kOpCodeShift;
        uint32_t botmask = 0xFFFFFFFFu >> (32-opbits);
        if (Instr::InstrWidth::kInstrSz32 == w) {
          int topbits = 21-opbits;
          uint32_t topmask = 0xFFFFFFFF >> (32-topbits);
          c = (8 << opbits) | (v[j] & botmask) | (((v[j] >> opbits) & topmask) << (opbits+5));
        } else if (Instr::InstrWidth::kInstrSz16 == w) {
          int topbits = 16-opbits;
          uint32_t topmask = 0xFFFFFFFF >> (32-topbits);
          c = (8 << opbits) | (v[j] & botmask) | (((v[j] >> opbits) & topmask) << (opbits+5));
        } else if (Instr::InstrWidth::kInstrSz8 == w) {
          c = (8 << opbits) | (v[j] & 0x0FF);
        }
        instrs.set(ins+j, c);
      }
    }

    uint32_t ret = instrs.execute(phv, adib, phv);
    ASSERT_FALSE(ret);

    for (int iii=0; iii<Instr::kGroups; iii+=2) {
      int ins = iii * Instr::kInstrAluGrpSize;
      for (unsigned j=0; j<sizeof(v)/sizeof(v[0]); ++j) {
        if (Instr::isOperandMuxOnlyAlu(ins+j)) continue;
        Instr::InstrWidth w = instrs.getWidth(ins+j);
        ret = phv->get(ins+j);
        if (Instr::InstrWidth::kInstrSz32 == w) {
          ASSERT_TRUE((v[j] & 0x1FFFFF) == ret);
        } else if (Instr::InstrWidth::kInstrSz16 == w) {
          ASSERT_TRUE((v[j] & 0xFFFF) == ret);
        } else if (Instr::InstrWidth::kInstrSz8 == w) {
          ASSERT_TRUE((v[j] & 0xFF) == ret);
        } else {
          ASSERT_TRUE(0);
        }
      }
    }
    delete phv;
  }


  TEST(BFN_TEST_NAME(InstrTest),FunnelShift) {
    Instr instrs;
    // Get a dummy PHV
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    for (int i=0; i<Phv::kWordsMax; ++i) {
      if (32 == phv->which_width(i)) {
        phv->set(i, -1*i);
      } else if (8 == phv->which_width(i)) {
        phv->set(i, 0x80 | i);
      } else {
        phv->set(i, (i << 8) | i);
      }
    }

    // Get a dummy bit vector and fill it with a fixed pattern.
    std::array<uint8_t,128> pattern;
    for (int i=0; i<128; ++i) {
      pattern[i] = 128 - i;
    }
    BitVector<MauDefs::kActionHVOutputBusWidth> adib(pattern);

    // For each combination of Src1 type, Src1 value, and Src2 value on the
    // even ALU
    for (int i=0; i<=0x1; ++i) {
      for (int j=0; j<32; ++j) {
        for (int k=0; k<=0xF; ++k) {
          // For each shift value
          for (int shift=0; shift<32; ++shift) {
            // Build the instruction.
            instrs.reset();
            // For instructions in each group
            for (int iii=0; iii<Instr::kGroups; iii+=2) {
              int ins = iii * Instr::kInstrAluGrpSize;
              uint32_t c = (OP_SET(4,0,4)) | INS_COMMON_BITS(i, j, k);
              if (Instr::InstrWidth::kInstrSz32 == instrs.getWidth(ins)) {
                c |= OP_SET(10,6,shift);
              } else if (Instr::InstrWidth::kInstrSz16 == instrs.getWidth(ins)) {
                c |= OP_SET(9,6,shift%16);
              } else {
                c |= OP_SET(8,6,shift%8);
              }
              instrs.set(ins, c);
            }

            // Execute the instructions.
            Phv *phv_out = om->phv_create();
            uint32_t ret = instrs.execute(phv, adib, phv_out);
            ASSERT_FALSE(ret);

            for (int iii=0; iii<Instr::kGroups; iii+=2) {
              int ins = iii * Instr::kInstrAluGrpSize;
              uint32_t val = 0;
              bool had_error = true;
              if (Instr::InstrWidth::kInstrSz32 == instrs.getWidth(ins)) {
                uint64_t v = computeOp1(ins, i, j, adib, phv, had_error);
                v = (v << 32) | computeOp2(ins, k, phv, had_error);
                val = (v >> shift) & 0xFFFFFFFF;
              } else if (Instr::InstrWidth::kInstrSz16 == instrs.getWidth(ins)) {
                uint32_t v = computeOp1(ins, i, j, adib, phv, had_error);
                v = (v << 16) | computeOp2(ins, k, phv, had_error);
                val = (v >> (shift%16)) & 0x0000FFFF;
              } else {
                uint16_t v = computeOp1(ins, i, j, adib, phv, had_error);
                v = (v << 8) | computeOp2(ins, k, phv, had_error);
                val = (v >> (shift%8)) & 0x000000FF;
              }
              ret = phv_out->get(ins);
              ASSERT_FALSE(had_error);
              EXPECT_EQ(ret, val);
              ASSERT_TRUE(ret == val);
            }
            delete phv_out;
          }
        }
      }
    }
    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),ShiftLeft) {
    Instr instrs;
    // Get a dummy PHV
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    for (int i=0; i<Phv::kWordsMax; ++i) {
      if (32 == phv->which_width(i)) {
        phv->set(i, -1*i);
      } else if (8 == phv->which_width(i)) {
        phv->set(i, 0x80 | i);
      } else {
        phv->set(i, (i << 8) | i);
      }
    }

    // Get a dummy bit vector and fill it with a fixed pattern.
    std::array<uint8_t,128> pattern;
    for (int i=0; i<128; ++i) {
      pattern[i] = 128 - i;
    }
    BitVector<MauDefs::kActionHVOutputBusWidth> adib(pattern);

    // For each value of source 2 operand.
    for (int k=0; k<=0xF; ++k) {
      // For each shift value
      for (int shift=0; shift<32; ++shift) {
        // Build the instruction.
        instrs.reset();
        // For instructions in each group
        for (int iii=0; iii<Instr::kGroups; iii+=2) {
          int ins = iii * Instr::kInstrAluGrpSize;
          uint32_t c = (OP_SET(4,0,0xC)) | INS_COMMON_BITS(0, 0, k);
          if (Instr::InstrWidth::kInstrSz32 == instrs.getWidth(ins)) {
            c |= OP_SET(10,6,shift);
          } else if (Instr::InstrWidth::kInstrSz16 == instrs.getWidth(ins)) {
            c |= OP_SET(9,6,shift%16);
          } else {
            c |= OP_SET(8,6,shift%8);
          }
          instrs.set(ins, c);
        }

        // Execute the instructions.
        Phv *phv_out = om->phv_create();
        uint32_t ret = instrs.execute(phv, adib, phv_out);
        ASSERT_FALSE(ret);

        for (int iii=0; iii<Instr::kGroups; iii+=2) {
          int ins = iii * Instr::kInstrAluGrpSize;
          uint32_t val = 0;
          bool had_error = true;
          if (Instr::InstrWidth::kInstrSz32 == instrs.getWidth(ins)) {
            uint32_t v = computeOp2(ins, k, phv, had_error);
            val = (v << shift) & 0xFFFFFFFF;
          } else if (Instr::InstrWidth::kInstrSz16 == instrs.getWidth(ins)) {
            uint16_t v = computeOp2(ins, k, phv, had_error);
            val = (v << (shift%16)) & 0x0000FFFF;
          } else {
            uint8_t v = computeOp2(ins, k, phv, had_error);
            val = (v << (shift%8)) & 0x000000FF;
          }
          ret = phv_out->get(ins);
          ASSERT_FALSE(had_error);
          EXPECT_EQ(ret, val);
          ASSERT_TRUE(ret == val);
        }
        delete phv_out;
      }
    }
    delete phv;
    delete om;
  }

  TEST(BFN_TEST_NAME(InstrTest),ShiftRightUnsigned) {
    Instr instrs;
    // Get a dummy PHV
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    for (int i=0; i<Phv::kWordsMax; ++i) {
      if (32 == phv->which_width(i)) {
        phv->set(i, -1*i);
      } else if (8 == phv->which_width(i)) {
        phv->set(i, 0x80 | i);
      } else {
        phv->set(i, (i << 8) | i);
      }
    }

    // Get a dummy bit vector and fill it with a fixed pattern.
    std::array<uint8_t,128> pattern;
    for (int i=0; i<128; ++i) {
      pattern[i] = 128 - i;
    }
    BitVector<MauDefs::kActionHVOutputBusWidth> adib(pattern);

    // For each value of source 2 operand.
    for (int k=0; k<=0xF; ++k) {
      // For each shift value
      for (int shift=0; shift<32; ++shift) {
        // Build the instruction.
        instrs.reset();
        // For instructions in each group
        for (int iii=0; iii<Instr::kGroups; iii+=2) {
          int ins = iii * Instr::kInstrAluGrpSize;
          uint32_t c = (OP_SET(4,0,0x14)) | INS_COMMON_BITS(0, 0, k);
          if (Instr::InstrWidth::kInstrSz32 == instrs.getWidth(ins)) {
            c |= OP_SET(10,6,shift);
          } else if (Instr::InstrWidth::kInstrSz16 == instrs.getWidth(ins)) {
            c |= OP_SET(9,6,shift%16);
          } else {
            c |= OP_SET(8,6,shift%8);
          }
          instrs.set(ins, c);
        }

        // Execute the instructions.
        Phv *phv_out = om->phv_create();
        uint32_t ret = instrs.execute(phv, adib, phv_out);
        ASSERT_FALSE(ret);

        for (int iii=0; iii<Instr::kGroups; iii+=2) {
          int ins = iii * Instr::kInstrAluGrpSize;
          uint32_t val = 0;
          bool had_error = true;
          if (Instr::InstrWidth::kInstrSz32 == instrs.getWidth(ins)) {
            uint32_t v = computeOp2(ins, k, phv, had_error);
            val = (v >> shift) & 0xFFFFFFFF;
          } else if (Instr::InstrWidth::kInstrSz16 == instrs.getWidth(ins)) {
            uint16_t v = computeOp2(ins, k, phv, had_error);
            val = (v >> (shift%16)) & 0x0000FFFF;
          } else {
            uint8_t v = computeOp2(ins, k, phv, had_error);
            val = (v >> (shift%8)) & 0x000000FF;
          }
          ret = phv_out->get(ins);
          ASSERT_FALSE(had_error);
          EXPECT_EQ(ret, val);
          ASSERT_TRUE(ret == val);
        }
        delete phv_out;
      }
    }
    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),ShiftRightSigned) {
    Instr instrs;
    // Get a dummy PHV
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    for (int i=0; i<Phv::kWordsMax; ++i) {
      if (32 == phv->which_width(i)) {
        phv->set(i, -1*i);
      } else if (8 == phv->which_width(i)) {
        phv->set(i, 0x80 | i);
      } else {
        phv->set(i, (i << 8) | i);
      }
    }

    // Get a dummy bit vector and fill it with a fixed pattern.
    std::array<uint8_t,128> pattern;
    for (int i=0; i<128; ++i) {
      pattern[i] = 128 - i;
    }
    BitVector<MauDefs::kActionHVOutputBusWidth> adib(pattern);

    // For each value of source 2 operand.
    for (int k=0; k<=0xF; ++k) {
      // For each shift value
      for (int shift=0; shift<32; ++shift) {
        // Build the instruction.
        instrs.reset();
        // For instructions in each group
        for (int iii=0; iii<Instr::kGroups; iii+=2) {
          int ins = iii * Instr::kInstrAluGrpSize;
          uint32_t c = (OP_SET(4,0,0x1C)) | INS_COMMON_BITS(0, 0, k);
          if (Instr::InstrWidth::kInstrSz32 == instrs.getWidth(ins)) {
            c |= OP_SET(10,6,shift);
          } else if (Instr::InstrWidth::kInstrSz16 == instrs.getWidth(ins)) {
            c |= OP_SET(9,6,shift%16);
          } else {
            c |= OP_SET(8,6,shift%8);
          }
          instrs.set(ins, c);
        }

        // Execute the instructions.
        Phv *phv_out = om->phv_create();
        uint32_t ret = instrs.execute(phv, adib, phv_out);
        ASSERT_FALSE(ret);

        for (int iii=0; iii<Instr::kGroups; iii+=2) {
          int ins = iii * Instr::kInstrAluGrpSize;
          uint32_t val = 0;
          bool had_error = true;
          if (Instr::InstrWidth::kInstrSz32 == instrs.getWidth(ins)) {
            int32_t v = computeOp2(ins, k, phv, had_error);
            val = (v >> shift) & 0xFFFFFFFF;
          } else if (Instr::InstrWidth::kInstrSz16 == instrs.getWidth(ins)) {
            int16_t v = computeOp2(ins, k, phv, had_error);
            val = (v >> (shift%16)) & 0x0000FFFF;
          } else {
            int8_t v = computeOp2(ins, k, phv, had_error);
            val = (v >> (shift%8)) & 0x000000FF;
          }
          ret = phv_out->get(ins);
          ASSERT_FALSE(had_error);
          EXPECT_EQ(ret, val);
          ASSERT_TRUE(ret == val);
        }
        delete phv_out;
      }
    }
    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),ByteRotateMerge) {
    Instr instrs;
    // Get a dummy PHV
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    for (int i=0; i<Phv::kWordsMax; ++i) {
      if (32 == phv->which_width(i)) {
        phv->set(i, -1*i);
      } else if (8 == phv->which_width(i)) {
        phv->set(i, 0x80 | i);
      } else {
        phv->set(i, (i << 8) | i);
      }
    }

    // Get a dummy bit vector and fill it with a fixed pattern.
    std::array<uint8_t,128> pattern;
    for (int i=0; i<128; ++i) {
      pattern[i] = 128 - i;
    }
    BitVector<MauDefs::kActionHVOutputBusWidth> adib(pattern);

    // For some combinations of src1, src2, rotate counts, and mask.
    for (int i=0; i<=0x1; ++i) {
      for (int j=0; j<32; j+=2) {
        for (int k=0; k<=0xF; k+=3) {
          // For each shift value
          for (int ops=0; ops<256; ++ops) {
            // Build the instruction.
            instrs.reset();
            // For instructions in each group
            for (int iii=0; iii<Instr::kGroups &&
                     (iii < 4 || (iii >= 4 && !(ops & 0xF0)));
                 iii+=2) {
              int ins = iii * Instr::kInstrAluGrpSize;

              uint32_t c = (OP_SET(3,0,0xA)) | INS_COMMON_BITS(i, j, k);
              if (Instr::InstrWidth::kInstrSz32 == instrs.getWidth(ins)) {
                unsigned byte_mask = ops & 0xF;
                unsigned rot1 = (ops >> 4) & 0x3, rot2 = (ops >> 6) & 0x3;
                c |= OP_SET(7,4,byte_mask);
                c |= OP_SET(10,9,rot2);
                c |= OP_SET(12,11,rot1);
              } else if (Instr::InstrWidth::kInstrSz16 == instrs.getWidth(ins)) {
                unsigned byte_mask = ops & 0x3;
                unsigned rot1 = (ops >> 2) & 0x1, rot2 = (ops >> 3) & 0x1;
                c |= OP_SET(5,4,byte_mask);
                c |= OP_SET(9,9,rot2);
                c |= OP_SET(11,11,rot1);
              } else {
                // No 8-bit verion of the instruction.
              }
              instrs.set(ins, c);
            }

            // Execute the instructions.
            Phv *phv_out = om->phv_create();
            uint32_t ret = instrs.execute(phv, adib, phv_out);
            ASSERT_FALSE(ret);

            for (int iii=0; iii<Instr::kGroups &&
                     (iii < 4 || (iii >= 4 && !(ops & 0xF0)));
                 iii+=2) {
              int ins = iii * Instr::kInstrAluGrpSize;

              uint32_t val = 0;
              bool had_error = true;
              if (Instr::InstrWidth::kInstrSz32 == instrs.getWidth(ins)) {
                uint32_t x = computeOp1(ins, i, j, adib, phv, had_error);
                ASSERT_FALSE(had_error);
                uint32_t y = computeOp2(ins, k, phv, had_error);
                ASSERT_FALSE(had_error);
                uint8_t b10 = x & 0xFF;
                uint8_t b11 = (x >> 8) & 0xFF;
                uint8_t b12 = (x >> 16) & 0xFF;
                uint8_t b13 = (x >> 24) & 0xFF;
                uint8_t rot1 = (ops >> 4) & 0x3;
                uint8_t b1[4] = {};
                if (0 == rot1) {
                  b1[0] = b10; b1[1] = b11; b1[2] = b12; b1[3] = b13;
                } else if (1 == rot1) {
                  b1[0] = b11; b1[1] = b12; b1[2] = b13; b1[3] = b10;
                } else if (2 == rot1) {
                  b1[0] = b12; b1[1] = b13; b1[2] = b10; b1[3] = b11;
                } else if (3 == rot1) {
                  b1[0] = b13; b1[1] = b10; b1[2] = b11; b1[3] = b12;
                }
                uint8_t b20 = y & 0xFF;
                uint8_t b21 = (y >> 8) & 0xFF;
                uint8_t b22 = (y >> 16) & 0xFF;
                uint8_t b23 = (y >> 24) & 0xFF;
                uint8_t rot2 = (ops >> 6) & 0x3;
                uint8_t b2[4] = {};
                if (0 == rot2) {
                  b2[0] = b20; b2[1] = b21; b2[2] = b22; b2[3] = b23;
                } else if (1 == rot2) {
                  b2[0] = b21; b2[1] = b22; b2[2] = b23; b2[3] = b20;
                } else if (2 == rot2) {
                  b2[0] = b22; b2[1] = b23; b2[2] = b20; b2[3] = b21;
                } else if (3 == rot2) {
                  b2[0] = b23; b2[1] = b20; b2[2] = b21; b2[3] = b22;
                }
                val = (ops & 0x8 ? b1[3] : b2[3]) << 24 |
                      (ops & 0x4 ? b1[2] : b2[2]) << 16 |
                      (ops & 0x2 ? b1[1] : b2[1]) << 8  |
                      (ops & 0x1 ? b1[0] : b2[0]);
              } else if (Instr::InstrWidth::kInstrSz16 == instrs.getWidth(ins)) {
                uint16_t x = computeOp1(ins, i, j, adib, phv, had_error);
                ASSERT_FALSE(had_error);
                uint16_t y = computeOp2(ins, k, phv, had_error);
                ASSERT_FALSE(had_error);
                uint8_t b10 = x & 0xFF;
                uint8_t b11 = (x >> 8) & 0xFF;
                uint8_t rot1 = (ops >> 2) & 0x1;
                uint8_t b1[2] = {};
                if (0 == rot1) {
                  b1[0] = b10; b1[1] = b11;
                } else if (1 == rot1) {
                  b1[0] = b11; b1[1] = b10;
                }
                uint8_t b20 = y & 0xFF;
                uint8_t b21 = (y >> 8) & 0xFF;
                uint8_t rot2 = (ops >> 3) & 0x1;
                uint8_t b2[2] = {};
                if (0 == rot2) {
                  b2[0] = b20; b2[1] = b21;
                } else if (1 == rot2) {
                  b2[0] = b21; b2[1] = b20;
                }
                val = (ops & 0x2 ? b1[1] : b2[1]) << 8 |
                      (ops & 0x1 ? b1[0] : b2[0]);
              } else {
                val = 0;
                had_error = false;
              }
              ret = phv_out->get(ins);
              ASSERT_FALSE(had_error);
              EXPECT_EQ(ret, val);
              ASSERT_TRUE(ret == val);
            }
            delete phv_out;
          }
        }
      }
    }
    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),ConditionalMuxMove) {
    Instr instrs;
    // Construct a PHV where in the first two fields of each width are valid and
    // the rest of the PHV is invalid.  This PHV will only be used for source
    // operands.
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    BitVector<MauDefs::kActionHVOutputBusWidth> adib;
    for (int ppp=0; ppp<12; ppp+=4) {
      int phv_grp = ppp * Instr::kInstrAluGrpSize;
      phv->set(phv_grp+0, 20);
      phv->set(phv_grp+1, 10);
    }

    // Currently this test only tests the case where *BOTH*
    // kInstrCondMuxIsCondMove and kInstrCondMoveIsUnconditional are true or both false
    // (this reflects the default JBay/Tofino configurations)
    ASSERT_TRUE(Instr::kInstrCondMuxIsCondMove == Instr::kInstrCondMoveIsUnconditional);

    for (int test_dst=0; test_dst<=1; ++test_dst) {
      for (int dst_pol=0; dst_pol<=1; ++dst_pol) {
        for (int test_s1=0; test_s1<=1; ++test_s1) {
          for (int test_s2=0; test_s2<=1; ++test_s2) {
            for (int s2_pol=0; s2_pol<=1; ++s2_pol) {
              instrs.reset();
              for (int iii=0; iii<12; iii+=4) {
                int ins = iii * Instr::kInstrAluGrpSize;

#define COND_MOV_INS_BITS (OP_SET(9,9,test_s1) | OP_SET(8,8,s2_pol)  | \
                           OP_SET(7,7,test_s2) | OP_SET(6,6,dst_pol) | \
                           OP_SET(5,5,test_dst) | OP_SET(4,0,0x16))
#define COND_MUX_INS_BITS (OP_SET(6,6,dst_pol) | \
                           OP_SET(5,5,test_dst) | OP_SET(4,0,0x06))
                instrs.set(ins+ 0, COND_MOV_INS_BITS | INS_COMMON_BITS(0,1,0));
                instrs.set(ins+ 1, COND_MOV_INS_BITS | INS_COMMON_BITS(0,1,15));
                instrs.set(ins+ 2, COND_MOV_INS_BITS | INS_COMMON_BITS(0,14,0));
                instrs.set(ins+ 3, COND_MOV_INS_BITS | INS_COMMON_BITS(0,14,15));

                instrs.set(ins+ 4, COND_MUX_INS_BITS | INS_COMMON_BITS(0,1,0));
                instrs.set(ins+ 5, COND_MUX_INS_BITS | INS_COMMON_BITS(0,1,15));
                instrs.set(ins+ 6, COND_MUX_INS_BITS | INS_COMMON_BITS(0,14,0));
                instrs.set(ins+ 7, COND_MUX_INS_BITS | INS_COMMON_BITS(0,14,15));
#undef COND_MOV_MV_INS_BITS
#undef COND_MUX_MV_INS_BITS
              }

              // Construct a PHV where in the first 16 fields of each width
              // are valid and the rest of the PHV is invalid.  This PHV will
              // only be used as the ALU destination.
              Phv *phv_out = om->phv_create();
              for (int ppp=0; ppp<12; ppp+=4) {
                int phv_grp = ppp * Instr::kInstrAluGrpSize;
                for (int i=0; i<16; ++i) {
                  phv_out->set(phv_grp+i, 100);
                }
              }

              // Execute the instructions
              uint32_t ret = instrs.execute(phv, adib, phv_out);
              uint32_t error_cnt = 0;

              for (int iii=0; iii<12; iii+=4) {
                int ins = iii * Instr::kInstrAluGrpSize;

                // Validate the results.
                uint32_t val = 0;
                bool wrote = false, error = false;
                // Mov, S1-val,   s2-val,   dst-val
                wrote = (!test_dst || (test_dst && dst_pol)) ? true : false;
                wrote = !wrote ? false : (!test_s2 || (test_s2 && s2_pol));
                error = false;
                val = wrote ? 10 : 100;
                if (Instr::kInstrCondMoveIsUnconditional) { error = false; val = 10; }
                ASSERT_TRUE(val == phv_out->get(ins+0));
                error_cnt += error ? 1:0;
                // Mov, S1-val,   s2-inval, dst-val
                wrote = (!test_dst || (test_dst && dst_pol)) ? true : false;
                wrote = !wrote ? false : (!test_s2 || (test_s2 &&  !s2_pol));
                error = false;
                val = wrote ? 10 : 100;
                if (Instr::kInstrCondMoveIsUnconditional) { error = false; val = 10; }
                ASSERT_TRUE(val == phv_out->get(ins+1));
                error_cnt += error ? 1:0;
                // Mov, S1-inval,  s2-val,   dst-val
                wrote = (!test_dst || (test_dst && dst_pol)) ? true : false;
                wrote = !wrote ? false : (!test_s2 || (test_s2 && s2_pol));
                wrote = !wrote ? false : !test_s1;
                error = test_s1 ? wrote : false;
                val =  (!wrote || error) ? 100 : 0;
                if (Instr::kInstrCondMoveIsUnconditional) { error = false; val = 0; }
                ASSERT_TRUE(val == phv_out->get(ins+2));
                error_cnt += error ? 1:0;
                // Mov, S1-inval,  s2-inval, dst-val
                wrote = (!test_dst || (test_dst && dst_pol)) ? true : false;
                wrote = !wrote ? false : (!test_s2 || (test_s2 &&  !s2_pol));
                wrote = !wrote ? false : !test_s1;
                error = test_s1 ? wrote : false;
                val =  (!wrote || error) ? 100 : 0;
                if (Instr::kInstrCondMoveIsUnconditional) { error = false; val = 0; }
                ASSERT_TRUE(val == phv_out->get(ins+3));
                error_cnt += error ? 1:0;

                // Mux, S1-val,    s2-val,   dst-val
                wrote = (!test_dst || (test_dst && dst_pol)) ? true : false;
                wrote = !wrote ? false : true;
                error = false;
                val = wrote ? 20 : 100;
                if (Instr::kInstrCondMuxIsCondMove && Instr::kInstrCondMoveIsUnconditional) { error = false; val = 10; }
                ASSERT_TRUE(val == phv_out->get(ins+4));
                error_cnt += error ? 1:0;
                // Mux, S1-val,    s2-inval, dst-val
                wrote = (!test_dst || (test_dst && dst_pol)) ? true : false;
                wrote = !wrote ? false : true;
                error = false;
                val = wrote ? 10 : 100;
                if (Instr::kInstrCondMuxIsCondMove && Instr::kInstrCondMoveIsUnconditional) { error = false; val = 10; }
                ASSERT_TRUE(val == phv_out->get(ins+5));
                error_cnt += error ? 1:0;
                // Mux, S1-inval,  s2-val,   dst-val
                wrote = (!test_dst || (test_dst && dst_pol)) ? true : false;
                wrote = !wrote ? false : true;
                error = false;
                val = wrote ? 20 : 100;
                if (Instr::kInstrCondMuxIsCondMove && Instr::kInstrCondMoveIsUnconditional) { error = false; val = 0; }
                ASSERT_TRUE(val == phv_out->get(ins+6));
                error_cnt += error ? 1:0;
                // Mux, S1-inval,  s2-inval, dst-val
                wrote = (!test_dst || (test_dst && dst_pol)) ? true : false;
                wrote = !wrote ? false : !true;//test_s1;
                error = wrote;
                val = 100;
                if (Instr::kInstrCondMuxIsCondMove && Instr::kInstrCondMoveIsUnconditional) { error = false; val = 0; }
                ASSERT_TRUE(val == phv_out->get(ins+7));
                error_cnt += error ? 1:0;
              }
              if (ret != error_cnt) {
                RMT_UT_LOG_ERROR("ErrorCnt ALU %d != ErrorCntUT %d (TestDst %d, DstPol %d, TestS1 %d, TestS2 %d, S2Pol %d)", ret, error_cnt, test_dst, dst_pol, test_s1, test_s2, s2_pol);
              }
              ASSERT_TRUE(ret == error_cnt);

              delete phv_out;
            }
          }
        }
      }
    }
    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),Invalidate) {
    if (Instr::kInstrInvalidateIsNop || Instr::kInstrInvalidateIsErr) return;

    Instr instrs;
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    BitVector<MauDefs::kActionHVOutputBusWidth> adib;

    // Make everything valid in the PHV.
    for (int i=0; i<Phv::kWordsMax; ++i) {
      phv->set(i, i);
    }

    unsigned int G = Instr::kInstrAluGrpSize;
    unsigned int i0 = ( 0 * G) + 11;
    unsigned int i1 = (12 * G) +  8;
    unsigned int i2 = ( 6 * G) +  4;
    unsigned int i3 = (13 * G) +  2;
    unsigned int i4 = (13 * G) +  3;
    unsigned int i5 = ( 7 * G) +  8;
    unsigned int i6 = ( 7 * G) +  9;

    instrs.reset();
    // Invalidate a 32-bit, 16-bit and 8 bit field.
    instrs.set(i0, OP_SET(9,0,0x00E));
    instrs.set(i1, OP_SET(9,0,0x00E));
    instrs.set(i2, OP_SET(9,0,0x00E));

    // Invalidate using merged 8-bit and merged 16-bit ALUs.
    instrs.set(i3, OP_SET(9,0,0x00E));
    instrs.set(i4, OP_SET(9,0,0x002));
    instrs.set(i5, OP_SET(9,0,0x00E));
    instrs.set(i6, OP_SET(9,0,0x002));

    // ensure that the invalidated fields are marked as written.
    phv->start_recording_written();

    uint32_t ret = instrs.execute(phv, adib, phv);
    ASSERT_FALSE(ret);

    for (unsigned i=0; i < Phv::kWordsMax; ++i) {
      if (i0 == i || i1 == i || i2 == i ||
          i3 == i || i5 == i ) {
        ASSERT_FALSE(phv->is_valid(i));
        ASSERT_TRUE(phv->written_bv()->get_bit(i));
      } else {
        ASSERT_TRUE(phv->is_valid(i));
        ASSERT_FALSE(phv->written_bv()->get_bit(i));
        ASSERT_TRUE(i == phv->get(i));
      }
    }

    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),BitMaskedSet) {
    Instr instrs;
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    unsigned int G = Instr::kInstrAluGrpSize;
    int opcode = (jbay) ?0x00E :0x02E; // Diff opcode on JBay

    // Get a dummy bit vector and fill it with a fixed pattern.
    std::array<uint8_t,128> bv;

    // 32-bit ALU, Src=0x89ABCDEF, Mask=0xFFFFFFFF, Bkgrnd=0x12345678
    // 32-bit ALU, Src=0x89ABCDEF, Mask=0xFFFFFFFF, Bkgrnd=0x33333333
    bv[3]=0x89 & 0xFF; bv[2]=0xAB & 0xFF; bv[1]=0xCD & 0xFF; bv[0]=0xEF & 0xFF;
    bv[7]=0xFF; bv[6]=0xFF; bv[5]=0xFF; bv[4]=0xFF;
    phv->set(0, 0x12345678);
    phv->set(1, 0x33333333);
    // 32-bit ALU, Src=0x89ABCDEF, Mask=0x55555555, Bkgrnd=0x12345678
    // 32-bit ALU, Src=0x89ABCDEF, Mask=0x55555555, Bkgrnd=0x33333333
    bv[11]=0x89 & 0x55; bv[10]=0xAB & 0x55; bv[9]=0xCD & 0x55; bv[8]=0xEF & 0x55;
    bv[15]=0x55; bv[14]=0x55; bv[13]=0x55; bv[12]=0x55;
    phv->set(2, 0x12345678);
    phv->set(3, 0x33333333);

    // 8-bit ALU, Src=0xAB, Mask=0x0F, Bkgrnd=0x12
    // 8-bit ALU, Src=0xAB, Mask=0x0F, Bkgrnd=0x34
    bv[16]=0xAB & 0x0F;
    bv[17]=0x0F;
    phv->set((G*4)+0, 0x12);
    phv->set((G*4)+1, 0x34);
    // 8-bit ALU, Src=0xAB, Mask=0xF0, Bkgrnd=0x56
    // 8-bit ALU, Src=0xAB, Mask=0xF0, Bkgrnd=0x78
    bv[18]=0xAB & 0xF0;
    bv[19]=0xF0;
    phv->set((G*4)+2, 0x56);
    phv->set((G*4)+3, 0x78);

    // 16-bit ALU, Src=0xABCD, Mask=0x0F0F, Bkgrnd=0x1234
    // 16-bit ALU, Src=0xABCD, Mask=0x0F0F, Bkgrnd=0x8F8F
    bv[33]=0xAB & 0x0F; bv[32]=0xCD & 0x0F;
    bv[35]=0x0F; bv[34]=0x0F;
    phv->set((G*8)+0, 0x1234);
    phv->set((G*8)+1, 0x8F8F);
    // 16-bit ALU, Src=0xABCD, Mask=0xF0F0, Bkgrnd=0x5678
    // 16-bit ALU, Src=0xABCD, Mask=0xF0F0, Bkgrnd=0xFEDC
    bv[37]=0xAB & 0xF0; bv[36]=0xCD & 0xF0;
    bv[39]=0xF0; bv[38]=0xF0;
    phv->set((G*8)+2, 0x5678);
    phv->set((G*8)+3, 0xFEDC);

    // Merged 8-bit ALU, Src=0xABCD, Mask=0xF00F, Bkgrnd=0x1234
    //bv[36]=0xCD & 0x0F;
    //bv[37]=0xAB & 0xF0;
    //bv[38]=0x0F;
    //bv[39]=0xF0;
    //phv->set(96, 0x34);
    //phv->set(97, 0x12);

    // Merged 16-bit ALU, Src=0x89ABCDEF, Mask=0xAAAA5555, Bkgrnd=0xF0FF55AA
    // 41,40 == 96
    // 105,104 ==128
    //bv[113] = 0xCD & 0x55; bv[112] = 0xEF & 0x55;
    //bv[115] = 0x89 & 0xAA; bv[114] = 0xAB & 0xAA;
    //bv[119] = 0xAA; bv[118] = 0xAA; bv[117] = 0x55; bv[116] = 0x55;
    //phv->set(160, 0x55AA);
    //phv->set(161, 0xF0FF);

    instrs.reset();
    instrs.set(0, OP_SET(9,0,opcode) | INS_COMMON_BITS(1, 0, 0));
    instrs.set(1, OP_SET(9,0,opcode) | INS_COMMON_BITS(1, 1, 1));
    instrs.set(2, OP_SET(9,0,opcode) | INS_COMMON_BITS(1, 2, 0));
    instrs.set(3, OP_SET(9,0,opcode) | INS_COMMON_BITS(1, 2, 1));

    instrs.set((G*4)+0, OP_SET(9,0,opcode) | INS_COMMON_BITS(1, 16, 0));
    instrs.set((G*4)+1, OP_SET(9,0,opcode) | INS_COMMON_BITS(1, 16, 1));
    instrs.set((G*4)+2, OP_SET(9,0,opcode) | INS_COMMON_BITS(1, 18, 2));
    instrs.set((G*4)+3, OP_SET(9,0,opcode) | INS_COMMON_BITS(1, 18, 3));

    instrs.set((G*8)+0, OP_SET(9,0,opcode) | INS_COMMON_BITS(1, 0, 0));
    instrs.set((G*8)+1, OP_SET(9,0,opcode) | INS_COMMON_BITS(1, 0, 1));
    instrs.set((G*8)+2, OP_SET(9,0,opcode) | INS_COMMON_BITS(1, 2, 2));
    instrs.set((G*8)+3, OP_SET(9,0,opcode) | INS_COMMON_BITS(1, 2, 3));

    if (!Instr::kInstrPairDpfIsErr) {
      // More merging
      instrs.set((G*11)+ 9, OP_SET(9,0,opcode) | INS_COMMON_BITS(0, 31, 0));
      instrs.set((G*11)+10, OP_SET(9,0,0x02)   | INS_COMMON_BITS(0, 31, 1));
      instrs.set((G*11)+11, OP_SET(9,0,opcode) | INS_COMMON_BITS(0, 31, 0));
    }


    Phv *phv_out = om->phv_create();
    phv_out->start_recording_written();

    BitVector<MauDefs::kActionHVOutputBusWidth> adib(bv);
    uint32_t ret = instrs.execute(phv, adib, phv_out);
    ASSERT_FALSE(ret);

    uint32_t val = 0, alu = 0, s,m,b,i;

    s = 0x89ABCDEF; m = 0xFFFFFFFF; b = 0x12345678; i = 0;
    alu = phv_out->get(i);
    val = (s & m) | (b & ~m);
    ASSERT_TRUE(phv_out->is_valid(i));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i));
    ASSERT_TRUE(val == alu);
    s = 0xFFFFFFFF; m = 0xFFFFFFFF; b = 0x33333333; i = 1;
    alu = phv_out->get(i);
    val = (s & m) | (b & ~m);
    ASSERT_TRUE(phv_out->is_valid(i));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i));
    ASSERT_TRUE(val == alu);
    s = 0x89ABCDEF; m = 0x55555555; b = 0x12345678; i = 2;
    alu = phv_out->get(i);
    val = (s & m) | (b & ~m);
    ASSERT_TRUE(phv_out->is_valid(i));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i));
    ASSERT_TRUE(val == alu);
    s = 0x89ABCDEF; m = 0x55555555; b = 0x33333333; i = 3;
    alu = phv_out->get(i);
    val = (s & m) | (b & ~m);
    ASSERT_TRUE(phv_out->is_valid(i));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i));
    ASSERT_TRUE(val == alu);

    s = 0xAB; m = 0x0F; b = 0x12; i = (G*4)+0;
    alu = phv_out->get(i);
    val = (s & m) | (b & ~m);
    ASSERT_TRUE(phv_out->is_valid(i));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i));
    ASSERT_TRUE(val == alu);
    s = 0xAB; m = 0x0F; b = 0x34; i = (G*4)+1;
    alu = phv_out->get(i);
    val = (s & m) | (b & ~m);
    ASSERT_TRUE(phv_out->is_valid(i));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i));
    ASSERT_TRUE(val == alu);
    s = 0xAB; m = 0xF0; b = 0x56; i = (G*4)+2;
    alu = phv_out->get(i);
    val = (s & m) | (b & ~m);
    ASSERT_TRUE(phv_out->is_valid(i));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i));
    ASSERT_TRUE(val == alu);
    s = 0xAB; m = 0xF0; b = 0x78; i = (G*4)+3;
    alu = phv_out->get(i);
    val = (s & m) | (b & ~m);
    ASSERT_TRUE(phv_out->is_valid(i));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i));
    ASSERT_TRUE(val == alu);

    s = 0xABCD; m = 0x0F0F; b = 0x1234; i = (G*8)+0;
    alu = phv_out->get(i);
    val = (s & m) | (b & ~m);
    ASSERT_TRUE(phv_out->is_valid(i));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i));
    ASSERT_TRUE(val == alu);
    s = 0xABCD; m = 0x0F0F; b = 0x8F8F; i = (G*8)+1;
    alu = phv_out->get(i);
    val = (s & m) | (b & ~m);
    ASSERT_TRUE(phv_out->is_valid(i));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i));
    ASSERT_TRUE(val == alu);
    s = 0xABCD; m = 0xF0F0; b = 0x5678; i = (G*8)+2;
    alu = phv_out->get(i);
    val = (s & m) | (b & ~m);
    ASSERT_TRUE(phv_out->is_valid(i));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i));
    ASSERT_TRUE(val == alu);
    s = 0xABCD; m = 0xF0F0; b = 0xFEDC; i = (G*8)+3;
    alu = phv_out->get(i);
    val = (s & m) | (b & ~m);
    ASSERT_TRUE(phv_out->is_valid(i));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i));
    ASSERT_TRUE(val == alu);

    if (!Instr::kInstrPairDpfIsErr) {
      // Check merging
      for (i=((G*11)+9); i<=((G*11)+11); ++i) {
        if (!PhvFactory::kPhvInitAllValid) {
          ASSERT_FALSE(phv_out->is_valid(i));
        }
        ASSERT_FALSE(phv_out->written_bv()->get_bit(i));
      }
    }

    delete phv_out;
    delete phv;
    delete om;
  }


  // TODO - This is a pretty dumb unit test as it's implementation of the
  // logical operations is almost identical to what it is supposed to be
  // verifying.  Maybe instead just replace it with some checks against hard
  // coded values; for example, set src1=1 and src2=2 then verify OR is 3.
  TEST(BFN_TEST_NAME(InstrTest),Logicals) {
    Instr instrs;
    // Get a dummy PHV
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    int G = Instr::kInstrAluGrpSize;

    for (int i=0; i<Phv::kWordsMax; ++i) {
      if (32 == phv->which_width(i)) {
        phv->set(i, -1*i);
      } else if (8 == phv->which_width(i)) {
        phv->set(i, 0x80 | i);
      } else {
        phv->set(i, (i << 8) | i);
      }
    }

    // Get a dummy bit vector and fill it with a fixed pattern.
    std::array<uint8_t,128> pattern;
    for (int i=0; i<128; ++i) {
      pattern[i] = 128 - i;
    }
    BitVector<MauDefs::kActionHVOutputBusWidth> adib(pattern);

    // For all combinations of src1 and src2.
    for (int i=0; i<=0x1; ++i) {
      for (int j=0; j<32; ++j) {
        for (int k=0; k<G; ++k) {
          // Build the instruction.
          instrs.reset();
          for (int ins=0; ins<Instr::kInstrsMax; ++ins) {
            if (Instr::isOperandMuxOnlyAlu(ins)) continue;
            uint32_t c = OP_SET(9,6,ins%16) | OP_SET(5,0,0x1E) | INS_COMMON_BITS(i, j, k);
            instrs.set(ins, c);
          }

          // Execute the instructions
          Phv *phv_out = om->phv_create();
          phv_out->start_recording_written();

          uint32_t ret = instrs.execute(phv, adib, phv_out);
          ASSERT_FALSE(ret);

          // Verify the results.
          for (int ins=0; ins<Instr::kInstrsMax; ++ins) {
            if (Instr::isOperandMuxOnlyAlu(ins)) continue;
            // Get the operands from PHV/ADIB.
            bool had_error = true;
            uint32_t src1 = computeOp1(ins, i, j, adib, phv, had_error);
            ASSERT_FALSE(had_error);
            uint32_t src2 = computeOp2(ins, k, phv, had_error);
            ASSERT_FALSE(had_error);
            uint32_t val = 0;
            switch (ins % 16) {
              case 0:
                val = 0;
                break;
              case 1:
                val = ~(src1|src2);
                break;
              case 2:
                val = ~src1 & src2;
                break;
              case 3:
                val = ~src1;
                break;
              case 4:
                val = src1 & ~src2;
                break;
              case 5:
                val = ~src2;
                break;
              case 6:
                val = src1 ^ src2;
                break;
              case 7:
                val = ~(src1 & src2);
                break;
              case 8:
                val = src1 & src2;
                break;
              case 9:
                val = ~(src1 ^ src2);
                break;
              case 10:
                val = src2;
                break;
              case 11:
                val = ~src1 | src2;
                break;
              case 12:
                val = src1;
                break;
              case 13:
                val = src1 | ~src2;
                break;
              case 14:
                val = src1 | src2;
                break;
              case 15:
                val = ~0;
                break;
            }
            Instr::InstrWidth w = instrs.getWidth(ins);
            if (Instr::InstrWidth::kInstrSz16 == w) {
              val &= 0xFFFF;
            } else if (Instr::InstrWidth::kInstrSz8 == w) {
              val &= 0xFF;
            }

            // Get the ALU's result.
            uint32_t alu = phv_out->get(ins);
            ASSERT_TRUE(phv_out->is_valid(ins));
            ASSERT_TRUE(phv_out->written_bv()->get_bit(ins));
            ASSERT_TRUE(val == alu);
          }
          delete phv_out;
        }
      }
    }
    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),ArithmeticComparesBasic) {
    if (!jbay) return; // Only on JBay
    int G = Instr::kInstrAluGrpSize;
    RMT_ASSERT(G == 20); RMT_ASSERT(Phv::kWordsPerGroup == 40);

    RmtObjectManager *om = new RmtObjectManager();
    Instr instrs;
    Phv *phv = om->phv_create();

    // Special vals where X<X+1
    uint32_t vals32[] = { (uint32_t)-2147483648, (uint32_t)-32, (uint32_t)-1,
                          0, 1, 32, 2147483647, 2147483648, 3000000000, 4294967295 };
    uint16_t vals16[] = { (uint16_t)-32768, (uint16_t)-16, (uint16_t)-1,
                          0, 1, 16, 32767, 32768, 40000, 65535 };
    uint8_t  vals8[]  = { (uint8_t)-128, (uint8_t)-8, (uint8_t)-1,
                          0, 1, 8, 127, 128, 200, 255 };
    // Want ALUs 8/9 to do OP=9 (eq64) and ALUs 10/11 to do OP=11 (neq64)
    uint8_t  op_map32[] = { 0,1,2,3,4,5,6,7,9,9,11,11 };
    // Don't do eq64 and neq64 on 16b 8b ALUs - not allowed - error
    uint8_t  op_map[]   = { 0,1,2,3,4,5,6,7,8,10,8,10 };

    // Fill PHV with vals above
    for (int i = 0; i < Phv::kWordsMax; i++) {
      if (32 == phv->which_width(i)) {
        phv->set(i, vals32[i % 10]);
      } else if (8 == phv->which_width(i)) {
        phv->set(i, vals8[i % 10]);
      } else {
        phv->set(i, vals16[i % 10]);
      }
    }
    // From val arrays above we use signed vals 0-6 OR unsigned vals 3-9
    // We set up both src1/src2 to be in 0-6 or in 3-9 depending on signed/unsigned OP
    // Given values programmed into PHV, comparison result should
    //    be completely determined by i OP j
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 7; j++) {
        // Build instr
        instrs.reset();
        for (int ins = 0; ins < Instr::kInstrsMax; ins++) {
          if (Instr::isOperandMuxOnlyAlu(ins)) continue;
          // Which ALU is this within ALU group
          int g = ins % G;
          // MODs here should be redundant
          uint8_t op = (phv->which_width(ins) == 32) ?op_map32[g % 12] :op_map[g % 12];
          // OPs 1,3,5,7 are signed so use vals 0-6 - other OPs use vals 3-9
          bool signed_op = ((op==1) || (op==3) || (op==5) || (op==7));
          uint8_t inc = (signed_op) ?0 :3;
          // Set up src1/src2 to be in 0-6 or in 3-9
          // But if doing 64b ops, sometimes make src1/src2 the same on odd ALUs
          int j2 = ( ((op==9)||(op==11)) && ((ins%2)==1) && (((ins/10)%2)==1) ) ?i :j;
          uint32_t c = OP_SET(9,6,op) | OP_SET(5,0,0x2E) | INS_COMMON_BITS(0, i+inc, j2+inc);
          instrs.set(ins, c);
        }

        // Execute the instructions
        Phv *phv_out = om->phv_create();
        BitVector<MauDefs::kActionHVOutputBusWidth> adib(UINT64_C(0));
        uint32_t ret = instrs.execute(phv, adib, phv_out);
        ASSERT_FALSE(ret);

        // Verify the results - determined by i OP j
        uint8_t op, prevevenop, evenop = 99;
        for (int ins = 0; ins < Instr::kInstrsMax; ins++) {
          if (Instr::isOperandMuxOnlyAlu(ins)) continue;
          bool cmp = false;
          int g = ins % G;
          prevevenop = evenop;

          op = (phv->which_width(ins) == 32) ?op_map32[g % 12] :op_map[g % 12];
          evenop = ((ins % 2) == 0) ?op :99;
          switch (op) {
            case  0: cmp = (i >= j); break;
            case  1: cmp = (i >= j); break;
            case  2: cmp = (i <  j); break;
            case  3: cmp = (i <  j); break;
            case  4: cmp = (i <= j); break;
            case  5: cmp = (i <= j); break;
            case  6: cmp = (i >  j); break;
            case  7: cmp = (i >  j); break;
            case  8: cmp = (i == j);                          break; //EQ
            case  9: cmp = (prevevenop==9)  ?false :(i == j); break; //EQ64
            case 10: cmp = (i != j);                          break; //NEQ
            case 11: cmp = (prevevenop==11) ?false :(i != j); break; //NEQ64
          }
          uint32_t ours = (cmp) ?1u :0u;
          // Get the ALU's result.
          uint32_t alus = phv_out->get(ins);
          if (ours != alus) {
            printf("i=%d j=%d ins=%d width=%d g=%d op=%d ours=%d alus=%d\n",
                   i, j, ins, Phv::which_width(ins), g, op, ours, alus);
          }
          ASSERT_TRUE(ours == alus);
        }
        delete phv_out;
      }
    }
    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),ArithmeticComparesExtended) {
    if (!jbay) return; // Only on JBay

    Instr instrs;
    // Get a dummy PHV
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    int G = Instr::kInstrAluGrpSize;

    for (int i=0; i<Phv::kWordsMax; ++i) {
      if (32 == phv->which_width(i)) {
        phv->set(i, -1*i);
      } else if (8 == phv->which_width(i)) {
        phv->set(i, 0x80 | i);
      } else {
        phv->set(i, (i << 8) | i);
      }
    }
    // Get a dummy bit vector and fill it with a fixed pattern.
    std::array<uint8_t,128> pattern;
    for (int i=0; i<128; ++i) {
      pattern[i] = 128 - i;
    }
    BitVector<MauDefs::kActionHVOutputBusWidth> adib(pattern);

    // For all combinations of src1 and src2.
    for (int i=0; i<=0x1; ++i) {
      for (int j=0; j<32; ++j) {
        for (int k=0; k<G; ++k) {
          // Build the instruction.
          instrs.reset();
          for (int ins=0; ins<Instr::kInstrsMax; ++ins) {
            int op = (ins % 9);
            if (Instr::isOperandMuxOnlyAlu(ins)) continue;
            uint32_t c = OP_SET(9,6,op) | OP_SET(5,0,0x2E) | INS_COMMON_BITS(i, j, k);
            instrs.set(ins, c);
          }

          // Execute the instructions
          Phv *phv_out = om->phv_create();
          phv_out->start_recording_written();

          uint32_t ret = instrs.execute(phv, adib, phv_out);
          ASSERT_FALSE(ret);

          // Verify the results.
          for (int ins=0; ins<Instr::kInstrsMax; ++ins) {
            if (Instr::isOperandMuxOnlyAlu(ins)) continue;
            // Get the operands from PHV/ADIB.
            bool had_error = true;
            uint32_t src1 = computeOp1(ins, i, j, adib, phv, had_error);
            ASSERT_FALSE(had_error);
            uint32_t src2 = computeOp2(ins, k, phv, had_error);
            ASSERT_FALSE(had_error);

            uint32_t s1_uint32 = static_cast<uint32_t>(src1);
            uint32_t s2_uint32 = static_cast<uint32_t>(src2);
            uint16_t s1_uint16 = static_cast<uint16_t>(src1);
            uint16_t s2_uint16 = static_cast<uint16_t>(src2);
            uint8_t  s1_uint8  = static_cast<uint8_t>(src1);
            uint8_t  s2_uint8  = static_cast<uint16_t>(src2);
            int32_t  s1_int32 = static_cast<int32_t>(src1);
            int32_t  s2_int32 = static_cast<int32_t>(src2);
            int16_t  s1_int16 = static_cast<int16_t>(src1);
            int16_t  s2_int16 = static_cast<int16_t>(src2);
            int8_t   s1_int8  = static_cast<int8_t>(src1);
            int8_t   s2_int8  = static_cast<int16_t>(src2);

            bool     cmp = false;
            switch (instrs.getWidth(ins)) {
              case Instr::InstrWidth::kInstrSz32:
                switch (ins % 9) {
                  case 0: cmp = (s1_uint32 >= s2_uint32); break;
                  case 1: cmp = ( s1_int32 >= s2_int32);  break;
                  case 2: cmp = (s1_uint32 <  s2_uint32); break;
                  case 3: cmp = ( s1_int32 <  s2_int32);  break;
                  case 4: cmp = (s1_uint32 <= s2_uint32); break;
                  case 5: cmp = ( s1_int32 <= s2_int32);  break;
                  case 6: cmp = (s1_uint32 >  s2_uint32); break;
                  case 7: cmp = ( s1_int32 >  s2_int32);  break;
                  case 8: cmp = (s1_uint32 == s2_uint32); break;
                }
                break;
              case Instr::InstrWidth::kInstrSz16:
                switch (ins % 9) {
                  case 0: cmp = (s1_uint16 >= s2_uint16); break;
                  case 1: cmp = ( s1_int16 >= s2_int16);  break;
                  case 2: cmp = (s1_uint16 <  s2_uint16); break;
                  case 3: cmp = ( s1_int16 <  s2_int16);  break;
                  case 4: cmp = (s1_uint16 <= s2_uint16); break;
                  case 5: cmp = ( s1_int16 <= s2_int16);  break;
                  case 6: cmp = (s1_uint16 >  s2_uint16); break;
                  case 7: cmp = ( s1_int16 >  s2_int16);  break;
                  case 8: cmp = (s1_uint16 == s2_uint16); break;
                }
                break;
              case Instr::InstrWidth::kInstrSz8:
                switch (ins % 9) {
                  case 0: cmp = (s1_uint8 >= s2_uint8); break;
                  case 1: cmp = ( s1_int8 >= s2_int8);  break;
                  case 2: cmp = (s1_uint8 <  s2_uint8); break;
                  case 3: cmp = ( s1_int8 <  s2_int8);  break;
                  case 4: cmp = (s1_uint8 <= s2_uint8); break;
                  case 5: cmp = ( s1_int8 <= s2_int8);  break;
                  case 6: cmp = (s1_uint8 >  s2_uint8); break;
                  case 7: cmp = ( s1_int8 >  s2_int8);  break;
                  case 8: cmp = (s1_uint8 == s2_uint8); break;
                }
                break;
              case Instr::InstrWidth::kInstrSzUndefined:
                FAIL() << "Unexpected Instr::InstrWidth::kInstrSzUndefined";
                break;
            }
            uint32_t val = (cmp) ?1 :0;
            // Get the ALU's result.
            uint32_t alu = phv_out->get(ins);
            ASSERT_TRUE(phv_out->is_valid(ins));
            ASSERT_TRUE(phv_out->written_bv()->get_bit(ins));
            ASSERT_TRUE(val == alu);
          }
          delete phv_out;
        }
      }
    }
    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),ArithmeticsSaturatingAddUnsigned) {
    Instr instrs;

    // Get a dummy PHV
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    int G = Instr::kInstrAluGrpSize;

    // Get a dummy bit vector
    BitVector<MauDefs::kActionHVOutputBusWidth> adib;

    phv->set(0+0, 0xFFFFFFFF); // Max unsigned
    phv->set(0+1, 0x78787878);
    phv->set(0+2, 0x12345678);
    phv->set(0+3, 0x01234567);
    phv->set(0+4, 0x00000000); // Min unsigned
    phv->set(0+5, 0x80000000); // Min signed
    phv->set(0+6, 0x7FFFFFFF); // Max signed

    phv->set((G*4)+0, 0xFF);
    phv->set((G*4)+1, 0x78);
    phv->set((G*4)+2, 0x12);
    phv->set((G*4)+3, 0x01);
    phv->set((G*4)+4, 0x00);
    phv->set((G*4)+5, 0x80);
    phv->set((G*4)+6, 0x7F);
    phv->set((G*6)+1, 0xFF); phv->set((G*6)+0, 0xFF);
    phv->set((G*6)+3, 0x78); phv->set((G*6)+2, 0x78);
    phv->set((G*6)+5, 0x12); phv->set((G*6)+4, 0x34);
    phv->set((G*6)+7, 0x01); phv->set((G*6)+6, 0x23);
    phv->set((G*6)+9, 0x00); phv->set((G*6)+8, 0x00);
    phv->set((G*6)+11,0x80); phv->set((G*6)+10,0x00);
    phv->set((G*6)+13,0x7F); phv->set((G*6)+12,0xFF);

    phv->set((G*8)+0, 0xFFFF);
    phv->set((G*8)+1, 0x7878);
    phv->set((G*8)+2, 0x1234);
    phv->set((G*8)+3, 0x0123);
    phv->set((G*8)+4, 0x0000);
    phv->set((G*8)+5, 0x8000);
    phv->set((G*8)+6, 0x7FFF);
    phv->set((G*10)+1, 0xFFFF); phv->set((G*10)+0, 0xFFFF);
    phv->set((G*10)+3, 0x7878); phv->set((G*10)+2, 0x7878);
    phv->set((G*10)+5, 0x1234); phv->set((G*10)+4, 0x5678);
    phv->set((G*10)+7, 0x0123); phv->set((G*10)+6, 0x4567);
    phv->set((G*10)+9, 0x0000); phv->set((G*10)+8, 0x0000);
    phv->set((G*10)+11,0x8000); phv->set((G*10)+10,0x0000);
    phv->set((G*10)+13,0x7FFF); phv->set((G*10)+12,0xFFFF);

    //
    // Saturating Unsigned Add.
    //
    // Clear the instructions
    instrs.reset();
    uint16_t op = 0x03E;
    for (int iii=0; iii<=8; iii+=4) {
      int ins = iii * G;

      // 0xFFFFFFFF + 0xFFFFFFFF, 0xFF + 0xFF, 0xFFFF+0xFFFF
      instrs.set(ins+0, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,0));
      // 0xFFFFFFFF + 0, 0xFF + 0, 0xFFFF + 0
      instrs.set(ins+1, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,4));
      // 0 + 0xFFFFFFFF, 0 + 0xFF, 0 + 0xFFFF
      instrs.set(ins+2, OP_SET(9,0,op) | INS_COMMON_BITS(0,4,0));
      // 0x78787878 + 0x01234567, 0x78 + 0x01, 0x7878 + 0x0123
      instrs.set(ins+3, OP_SET(9,0,op) | INS_COMMON_BITS(0,1,3));
      // 0x12345678 + 0, 0x12 + 0, 0x1234 + 0
      instrs.set(ins+4, OP_SET(9,0,op) | INS_COMMON_BITS(0,2,4));
      // 0 + 0x12345678, 0 + 0x12, 0 + 0x1234
      instrs.set(ins+5, OP_SET(9,0,op) | INS_COMMON_BITS(0,4,2));
      // 0xFFFFFFFF + 0x12345678, 0xFF + 0x12, 0xFFFF + 0x1234
      instrs.set(ins+6, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,2));
      // 0x80000000 + 0x7FFFFFFF, 0x800 + 0x7F, 0x8000 + 0x7FFF
      instrs.set(ins+7, OP_SET(9,0,op) | INS_COMMON_BITS(0,5,6));
      // 0x7FFFFFFF + 0x80000000, 0x7F + 0x80, 0x7FFF + 0x8000
      instrs.set(ins+8, OP_SET(9,0,op) | INS_COMMON_BITS(0,6,5));
      // 0x01234567 + 0x7FFFFFFF, 0x01 + 0x7F, 0x0167 + 0x7FFF
      instrs.set(ins+9, OP_SET(9,0,op) | INS_COMMON_BITS(0,3,6));
    }
    for (int iii=6; iii<=10; iii+=4) {
      int ins = iii * G;

      // Merged ALUs.
      if (Instr::kInstrPairDpfIsErr) continue;

      // 0xFFFFFFFF + 0xFFFFFFFF, 0xFFFF+0xFFFF
      instrs.set(ins+0, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,0));
      instrs.set(ins+1, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,1));
      // 0xFFFFFFFF + 0, 0xFFFF + 0
      instrs.set(ins+2, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,8));
      instrs.set(ins+3, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,9));
      // 0 + 0xFFFFFFFF, 0 + 0xFFFF
      instrs.set(ins+4, OP_SET(9,0,op) | INS_COMMON_BITS(0,8,0));
      instrs.set(ins+5, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,9,1));
      // 0x78787878 + 0x01234567, 0x7878 + 0x0123
      instrs.set(ins+6, OP_SET(9,0,op) | INS_COMMON_BITS(0,2,6));
      instrs.set(ins+7, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,3,7));
      // 0x12345678 + 0, 0x1234 + 0
      instrs.set(ins+8, OP_SET(9,0,op) | INS_COMMON_BITS(0,4,8));
      instrs.set(ins+9, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,5,9));
      // 0 + 0x12345678, 0 + 0x1234
      instrs.set(ins+10,OP_SET(9,0,op) | INS_COMMON_BITS(0,8,4));
      instrs.set(ins+11,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,9,5));
      if (!Instr::isOperandMuxOnlyAlu(12,15)) {
        // 0xFFFFFFFF + 0x12345678, 0xFFFF + 0x1234
        instrs.set(ins+12,OP_SET(9,0,op) | INS_COMMON_BITS(0,0,4));
        instrs.set(ins+13,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,5));
        // 0x80000000 + 0x7FFFFFFF, 0x8000 + 0x7FFF
        instrs.set(ins+14,OP_SET(9,0,op) | INS_COMMON_BITS(0,10,12));
        instrs.set(ins+15,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,11,13));
      }
    }

    // Execute the instructions
    Phv *phv_out = om->phv_create();
    phv_out->start_recording_written();
    uint32_t error_cnt = instrs.execute(phv, adib, phv_out);
    ASSERT_FALSE(error_cnt);

    // Validate the results.
    for (int iii=0; iii<=8; iii+=4) {
      int ins = iii * G;
      uint32_t alu = 0, val = 0;

      // 0xFFFFFFFF + 0xFFFFFFFF, 0xFF + 0xFF, 0xFFFF+0xFFFF
      val = (ins < (G*4)) ? 0xFFFFFFFF : (ins < (G*8)) ? 0xFF : 0xFFFF;
      alu = phv_out->get(ins + 0);
      ASSERT_TRUE(phv_out->is_valid(ins + 0));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 0));
      ASSERT_TRUE(val == alu);

      // 0xFFFFFFFF + 0, 0xFF + 0, 0xFFFF + 0
      val = (ins < (G*4)) ? 0xFFFFFFFF : (ins < (G*8)) ? 0xFF : 0xFFFF;
      alu = phv_out->get(ins + 1);
      ASSERT_TRUE(phv_out->is_valid(ins + 1));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 1));
      ASSERT_TRUE(val == alu);

      // 0 + 0xFFFFFFFF, 0 + 0xFF, 0 + 0xFFFF
      val = (ins < (G*4)) ? 0xFFFFFFFF : (ins < (G*8)) ? 0xFF : 0xFFFF;
      alu = phv_out->get(ins + 2);
      ASSERT_TRUE(phv_out->is_valid(ins + 2));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 2));
      ASSERT_TRUE(val == alu);

      // 0x78787878 + 0x01234567, 0x78 + 0x01, 0x7878 + 0x0123
      val = (ins < (G*4)) ? 0x799BBDDF : (ins < (G*8)) ? 0x79 : 0x799B;
      alu = phv_out->get(ins + 3);
      ASSERT_TRUE(phv_out->is_valid(ins + 3));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 3));
      ASSERT_TRUE(val == alu);

      // 0x12345678 + 0, 0x12 + 0, 0x1234 + 0
      val = (ins < (G*4)) ? 0x12345678 : (ins < (G*8)) ? 0x12 : 0x1234;
      alu = phv_out->get(ins + 4);
      ASSERT_TRUE(phv_out->is_valid(ins + 4));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 4));
      ASSERT_TRUE(val == alu);

      // 0 + 0x12345678, 0 + 0x12, 0 + 0x1234
      val = (ins < (G*4)) ? 0x12345678 : (ins < (G*8)) ? 0x12 : 0x1234;
      alu = phv_out->get(ins + 5);
      ASSERT_TRUE(phv_out->is_valid(ins + 5));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 5));
      ASSERT_TRUE(val == alu);

      // 0xFFFFFFFF + 0x12345678, 0xFF + 0x12, 0xFFFF + 0x1234
      val = (ins < (G*4)) ? 0xFFFFFFFF : (ins < (G*8)) ? 0xFF : 0xFFFF;
      alu = phv_out->get(ins + 6);
      ASSERT_TRUE(phv_out->is_valid(ins + 6));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 6));
      ASSERT_TRUE(val == alu);

      // 0x80000000 + 0x7FFFFFFF, 0x80 + 0x7F, 0x8000 + 0x7FFF
      val = (ins < (G*4)) ? 0xFFFFFFFF : (ins < (G*8)) ? 0xFF : 0xFFFF;
      alu = phv_out->get(ins + 7);
      ASSERT_TRUE(phv_out->is_valid(ins + 7));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 7));
      ASSERT_TRUE(val == alu);

      // 0x7FFFFFFF + 0x80000000, 0x7F + 0x80, 0x7FFF + 0x8000
      val = (ins < (G*4)) ? 0xFFFFFFFF : (ins < (G*8)) ? 0xFF : 0xFFFF;
      alu = phv_out->get(ins + 8);
      ASSERT_TRUE(phv_out->is_valid(ins + 8));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 8));
      ASSERT_TRUE(val == alu);

      // 0x01234567 + 0x7FFFFFFF, 0x01 + 0x7F, 0x0123 + 0x7FFF
      val = (ins < (G*4)) ? 0x81234566 : (ins < (G*8)) ? 0x80 : 0x8122;
      alu = phv_out->get(ins + 9);
      ASSERT_TRUE(phv_out->is_valid(ins + 9));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 9));
      ASSERT_TRUE(val == alu);
    }

    // Validate the results for the merged ALUs.
    for (int iii=6; iii<=10; iii+=4) {
      int ins = iii * G;

      if (Instr::kInstrPairDpfIsErr) continue;

      uint32_t alu = 0, val = 0;
      int shift = ins < (G*8) ? 8 : 16;
      // 0xFFFFFFFF + 0xFFFFFFFF, 0xFFFF+0xFFFF
      val = (ins < (G*8)) ? 0xFFFF : 0xFFFFFFFF;
      alu = phv_out->get(ins + 1);
      alu = (alu << shift) | phv_out->get(ins + 0);
      ASSERT_TRUE(phv_out->is_valid(ins + 1) && phv_out->is_valid(ins + 0));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 1) &&
                  phv_out->written_bv()->get_bit(ins + 0));
      ASSERT_TRUE(val == alu);
      // 0xFFFFFFFF + 0, 0xFFFF + 0
      val = (ins < (G*8)) ? 0xFFFF : 0xFFFFFFFF;
      alu = phv_out->get(ins + 3);
      alu = (alu << shift) | phv_out->get(ins + 2);
      ASSERT_TRUE(phv_out->is_valid(ins + 3) && phv_out->is_valid(ins + 2));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 3) &&
                  phv_out->written_bv()->get_bit(ins + 2));
      ASSERT_TRUE(val == alu);
      // 0 + 0xFFFFFFFF, 0 + 0xFFFF
      val = (ins < (G*8)) ? 0xFFFF : 0xFFFFFFFF;
      alu = phv_out->get(ins + 5);
      alu = (alu << shift) | phv_out->get(ins + 4);
      ASSERT_TRUE(phv_out->is_valid(ins + 5) && phv_out->is_valid(ins + 4));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 5) &&
                  phv_out->written_bv()->get_bit(ins + 4));
      ASSERT_TRUE(val == alu);
      // 0x78787878 + 0x01234567, 0x7878 + 0x0123
      val = (ins < (G*8)) ? 0x799B : 0x799BBDDF;
      alu = phv_out->get(ins + 7);
      alu = (alu << shift) | phv_out->get(ins + 6);
      ASSERT_TRUE(phv_out->is_valid(ins + 7) && phv_out->is_valid(ins + 6));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 7) &&
                  phv_out->written_bv()->get_bit(ins + 6));
      ASSERT_TRUE(val == alu);
      // 0x12345678 + 0, 0x1234 + 0
      val = (ins < (G*8)) ? 0x1234 : 0x12345678;
      alu = phv_out->get(ins + 9);
      alu = (alu << shift) | phv_out->get(ins + 8);
      ASSERT_TRUE(phv_out->is_valid(ins + 9) && phv_out->is_valid(ins + 8));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 9) &&
                  phv_out->written_bv()->get_bit(ins + 8));
      ASSERT_TRUE(val == alu);
      // 0 + 0x12345678, 0 + 0x1234
      val = (ins < (G*8)) ? 0x1234 : 0x12345678;
      alu = phv_out->get(ins + 11);
      alu = (alu << shift) | phv_out->get(ins + 10);
      ASSERT_TRUE(phv_out->is_valid(ins + 11) && phv_out->is_valid(ins + 10));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 11) &&
                  phv_out->written_bv()->get_bit(ins + 10));
      ASSERT_TRUE(val == alu);
      if (!Instr::isOperandMuxOnlyAlu(12,15)) {
        // 0xFFFFFFFF + 0x12345678, 0xFFFF + 0x1234
        val = (ins < (G*8)) ? 0xFFFF : 0xFFFFFFFF;
        alu = phv_out->get(ins + 13);
        alu = (alu << shift) | phv_out->get(ins + 12);
        ASSERT_TRUE(phv_out->is_valid(ins + 13) && phv_out->is_valid(ins + 12));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 13) &&
                    phv_out->written_bv()->get_bit(ins + 12));
        ASSERT_TRUE(val == alu);
        // 0x80000000 + 0x7FFFFFFF, 0x8000 + 0x7FFF
        val = (ins < (G*8)) ? 0xFFFF : 0xFFFFFFFF;
        alu = phv_out->get(ins + 15);
        alu = (alu << shift) | phv_out->get(ins + 14);
        ASSERT_TRUE(phv_out->is_valid(ins + 15) && phv_out->is_valid(ins + 14));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 15) &&
                    phv_out->written_bv()->get_bit(ins + 14));
        ASSERT_TRUE(val == alu);
      }
    }
    delete phv_out;

    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),ArithmeticsSaturatingAddSigned) {
    Instr instrs;

    // Get a dummy PHV
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    int G = Instr::kInstrAluGrpSize;

    // Get a dummy bit vector
    BitVector<MauDefs::kActionHVOutputBusWidth> adib;

    phv->set(0+0, 0xFFFFFFFF); // Max unsigned
    phv->set(0+1, 0x78787878);
    phv->set(0+2, 0x12345678);
    phv->set(0+3, 0x01234567);
    phv->set(0+4, 0x00000000); // Min unsigned
    phv->set(0+5, 0x80000000); // Min signed
    phv->set(0+6, 0x7FFFFFFF); // Max signed

    phv->set((G*4)+0, 0xFF);
    phv->set((G*4)+1, 0x78);
    phv->set((G*4)+2, 0x12);
    phv->set((G*4)+3, 0x01);
    phv->set((G*4)+4, 0x00);
    phv->set((G*4)+5, 0x80);
    phv->set((G*4)+6, 0x7F);
    phv->set((G*6)+1, 0xFF); phv->set((G*6)+0, 0xFF);
    phv->set((G*6)+3, 0x78); phv->set((G*6)+2, 0x78);
    phv->set((G*6)+5, 0x12); phv->set((G*6)+4, 0x34);
    phv->set((G*6)+7, 0x01); phv->set((G*6)+6, 0x23);
    phv->set((G*6)+9, 0x00); phv->set((G*6)+8, 0x00);
    phv->set((G*6)+11,0x80); phv->set((G*6)+10,0x00);
    phv->set((G*6)+13,0x7F); phv->set((G*6)+12,0xFF);

    phv->set((G*8)+0, 0xFFFF);
    phv->set((G*8)+1, 0x7878);
    phv->set((G*8)+2, 0x1234);
    phv->set((G*8)+3, 0x0123);
    phv->set((G*8)+4, 0x0000);
    phv->set((G*8)+5, 0x8000);
    phv->set((G*8)+6, 0x7FFF);
    phv->set((G*10)+1, 0xFFFF); phv->set((G*10)+0, 0xFFFF);
    phv->set((G*10)+3, 0x7878); phv->set((G*10)+2, 0x7878);
    phv->set((G*10)+5, 0x1234); phv->set((G*10)+4, 0x5678);
    phv->set((G*10)+7, 0x0123); phv->set((G*10)+6, 0x4567);
    phv->set((G*10)+9, 0x0000); phv->set((G*10)+8, 0x0000);
    phv->set((G*10)+11,0x8000); phv->set((G*10)+10,0x0000);
    phv->set((G*10)+13,0x7FFF); phv->set((G*10)+12,0xFFFF);

    //
    // Saturating Signed Add.
    //
    // Clear the instructions
    instrs.reset();
    uint16_t op = 0x07E;
    for (int iii=0; iii<=8; iii+=4) {
      int ins = iii * G;

      // 0xFFFFFFFF + 0xFFFFFFFF, 0xFF + 0xFF, 0xFFFF+0xFFFF
      instrs.set(ins+0, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,0));
      // 0xFFFFFFFF + 0, 0xFF + 0, 0xFFFF + 0
      instrs.set(ins+1, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,4));
      // 0 + 0xFFFFFFFF, 0 + 0xFF, 0 + 0xFFFF
      instrs.set(ins+2, OP_SET(9,0,op) | INS_COMMON_BITS(0,4,0));
      // 0x78787878 + 0x01234567, 0x78 + 0x01, 0x7878 + 0x0123
      instrs.set(ins+3, OP_SET(9,0,op) | INS_COMMON_BITS(0,1,3));
      // 0x12345678 + 0, 0x12 + 0, 0x1234 + 0
      instrs.set(ins+4, OP_SET(9,0,op) | INS_COMMON_BITS(0,2,4));
      // 0 + 0x12345678, 0 + 0x12, 0 + 0x1234
      instrs.set(ins+5, OP_SET(9,0,op) | INS_COMMON_BITS(0,4,2));
      // 0xFFFFFFFF + 0x12345678, 0xFF + 0x12, 0xFFFF + 0x1234
      instrs.set(ins+6, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,2));
      // 0x80000000 + 0x7FFFFFFF, 0x800 + 0x7F, 0x8000 + 0x7FFF
      instrs.set(ins+7, OP_SET(9,0,op) | INS_COMMON_BITS(0,5,6));
      // 0x7FFFFFFF + 0x80000000, 0x7F + 0x80, 0x7FFF + 0x8000
      instrs.set(ins+8, OP_SET(9,0,op) | INS_COMMON_BITS(0,6,5));
      // 0x01234567 + 0x7FFFFFFF, 0x01 + 0x7F, 0x0167 + 0x7FFF
      instrs.set(ins+9, OP_SET(9,0,op) | INS_COMMON_BITS(0,3,6));
    }
    for (int iii=6; iii<=10; iii+=4) {
      int ins = iii * G;

      // Merged ALUs.
      if (Instr::kInstrPairDpfIsErr) continue;

      // 0xFFFFFFFF + 0xFFFFFFFF, 0xFFFF+0xFFFF
      instrs.set(ins+0, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,0));
      instrs.set(ins+1, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,1));
      // 0xFFFFFFFF + 0, 0xFFFF + 0
      instrs.set(ins+2, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,8));
      instrs.set(ins+3, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,9));
      // 0 + 0xFFFFFFFF, 0 + 0xFFFF
      instrs.set(ins+4, OP_SET(9,0,op) | INS_COMMON_BITS(0,8,0));
      instrs.set(ins+5, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,9,1));
      // 0x78787878 + 0x01234567, 0x7878 + 0x0123
      instrs.set(ins+6, OP_SET(9,0,op) | INS_COMMON_BITS(0,2,6));
      instrs.set(ins+7, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,3,7));
      // 0x12345678 + 0, 0x1234 + 0
      instrs.set(ins+8, OP_SET(9,0,op) | INS_COMMON_BITS(0,4,8));
      instrs.set(ins+9, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,5,9));
      // 0 + 0x12345678, 0 + 0x1234
      instrs.set(ins+10,OP_SET(9,0,op) | INS_COMMON_BITS(0,8,4));
      instrs.set(ins+11,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,9,5));
      if (!Instr::isOperandMuxOnlyAlu(12,15)) {
        // 0xFFFFFFFF + 0x12345678, 0xFFFF + 0x1234
        instrs.set(ins+12,OP_SET(9,0,op) | INS_COMMON_BITS(0,0,4));
        instrs.set(ins+13,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,5));
        // 0x80000000 + 0x7FFFFFFF, 0x8000 + 0x7FFF
        instrs.set(ins+14,OP_SET(9,0,op) | INS_COMMON_BITS(0,10,12));
        instrs.set(ins+15,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,11,13));
      }
    }

    // Execute the instructions
    Phv *phv_out = om->phv_create();
    phv_out->start_recording_written();
    uint32_t error_cnt = instrs.execute(phv, adib, phv_out);
    ASSERT_FALSE(error_cnt);

    // Validate the results.
    for (int iii=0; iii<=8; iii+=4) {
      int ins = iii * G;
      uint32_t alu = 0, val = 0;

      // 0xFFFFFFFF + 0xFFFFFFFF, 0xFF + 0xFF, 0xFFFF+0xFFFF
      val = (ins < (G*4)) ? 0xFFFFFFFE : (ins < (G*8)) ? 0xFE : 0xFFFE;
      alu = phv_out->get(ins + 0);
      ASSERT_TRUE(phv_out->is_valid(ins + 0));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 0));
      ASSERT_TRUE(val == alu);

      // 0xFFFFFFFF + 0, 0xFF + 0, 0xFFFF + 0
      val = (ins < (G*4)) ? 0xFFFFFFFF : (ins < (G*8)) ? 0xFF : 0xFFFF;
      alu = phv_out->get(ins + 1);
      ASSERT_TRUE(phv_out->is_valid(ins + 1));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 1));
      ASSERT_TRUE(val == alu);

      // 0 + 0xFFFFFFFF, 0 + 0xFF, 0 + 0xFFFF
      val = (ins < (G*4)) ? 0xFFFFFFFF : (ins < (G*8)) ? 0xFF : 0xFFFF;
      alu = phv_out->get(ins + 2);
      ASSERT_TRUE(phv_out->is_valid(ins + 2));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 2));
      ASSERT_TRUE(val == alu);

      // 0x78787878 + 0x01234567, 0x78 + 0x01, 0x7878 + 0x0123
      val = (ins < (G*4)) ? 0x799BBDDF : (ins < (G*8)) ? 0x79 : 0x799B;
      alu = phv_out->get(ins + 3);
      ASSERT_TRUE(phv_out->is_valid(ins + 3));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 3));
      ASSERT_TRUE(val == alu);

      // 0x12345678 + 0, 0x12 + 0, 0x1234 + 0
      val = (ins < (G*4)) ? 0x12345678 : (ins < (G*8)) ? 0x12 : 0x1234;
      alu = phv_out->get(ins + 4);
      ASSERT_TRUE(phv_out->is_valid(ins + 4));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 4));
      ASSERT_TRUE(val == alu);

      // 0 + 0x12345678, 0 + 0x12, 0 + 0x1234
      val = (ins < (G*4)) ? 0x12345678 : (ins < (G*8)) ? 0x12 : 0x1234;
      alu = phv_out->get(ins + 5);
      ASSERT_TRUE(phv_out->is_valid(ins + 5));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 5));
      ASSERT_TRUE(val == alu);

      // 0xFFFFFFFF + 0x12345678, 0xFF + 0x12, 0xFFFF + 0x1234
      val = (ins < (G*4)) ? 0x12345677 : (ins < (G*8)) ? 0x11 : 0x1233;
      alu = phv_out->get(ins + 6);
      ASSERT_TRUE(phv_out->is_valid(ins + 6));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 6));
      ASSERT_TRUE(val == alu);

      // 0x80000000 + 0x7FFFFFFF, 0x80 + 0x7F, 0x8000 + 0x7FFF
      val = (ins < (G*4)) ? 0xFFFFFFFF : (ins < (G*8)) ? 0xFF : 0xFFFF;
      alu = phv_out->get(ins + 7);
      ASSERT_TRUE(phv_out->is_valid(ins + 7));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 7));
      ASSERT_TRUE(val == alu);

      // 0x7FFFFFFF + 0x80000000, 0x7F + 0x80, 0x7FFF + 0x8000
      val = (ins < (G*4)) ? 0xFFFFFFFF : (ins < (G*8)) ? 0xFF : 0xFFFF;
      alu = phv_out->get(ins + 8);
      ASSERT_TRUE(phv_out->is_valid(ins + 8));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 8));
      ASSERT_TRUE(val == alu);

      // 0x01234567 + 0x7FFFFFFF, 0x01 + 0x7F, 0x0123 + 0x7FFF
      val = (ins < (G*4)) ? 0x7FFFFFFF : (ins < (G*8)) ? 0x7F : 0x7FFF;
      alu = phv_out->get(ins + 9);
      ASSERT_TRUE(phv_out->is_valid(ins + 9));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 9));
      ASSERT_TRUE(val == alu);
    }

    // Validate the results for the merged ALUs.
    for (int iii=6; iii<=10; iii+=4) {
      int ins = iii * G;

      if (Instr::kInstrPairDpfIsErr) continue;

      uint32_t alu = 0, val = 0;
      int shift = (ins < (G*8)) ? 8 : 16;
      // 0xFFFFFFFF + 0xFFFFFFFF, 0xFFFF+0xFFFF
      val = (ins < (G*8)) ? 0xFFFE : 0xFFFFFFFE;
      alu = phv_out->get(ins + 1);
      alu = (alu << shift) | phv_out->get(ins + 0);
      ASSERT_TRUE(phv_out->is_valid(ins + 1) && phv_out->is_valid(ins + 0));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 1) &&
                  phv_out->written_bv()->get_bit(ins + 0));
      ASSERT_TRUE(val == alu);
      // 0xFFFFFFFF + 0, 0xFFFF + 0
      val = (ins < (G*8)) ? 0xFFFF : 0xFFFFFFFF;
      alu = phv_out->get(ins + 3);
      alu = (alu << shift) | phv_out->get(ins + 2);
      ASSERT_TRUE(phv_out->is_valid(ins + 3) && phv_out->is_valid(ins + 2));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 3) &&
                  phv_out->written_bv()->get_bit(ins + 2));
      if (val != alu) {
        RMT_UT_LOG_INFO("ins %d ALU %#x (%d) UT %#x (%d)", ins, alu, alu, val, val);
      }
      ASSERT_TRUE(val == alu);
      // 0 + 0xFFFFFFFF, 0 + 0xFFFF
      val = (ins < (G*8)) ? 0xFFFF : 0xFFFFFFFF;
      alu = phv_out->get(ins + 5);
      alu = (alu << shift) | phv_out->get(ins + 4);
      ASSERT_TRUE(phv_out->is_valid(ins + 5) && phv_out->is_valid(ins + 4));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 5) &&
                  phv_out->written_bv()->get_bit(ins + 4));
      ASSERT_TRUE(val == alu);
      // 0x78787878 + 0x01234567, 0x7878 + 0x0123
      val = (ins < (G*8)) ? 0x799B : 0x799BBDDF;
      alu = phv_out->get(ins + 7);
      alu = (alu << shift) | phv_out->get(ins + 6);
      ASSERT_TRUE(phv_out->is_valid(ins + 7) && phv_out->is_valid(ins + 6));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 7) &&
                  phv_out->written_bv()->get_bit(ins + 6));
      ASSERT_TRUE(val == alu);
      // 0x12345678 + 0, 0x1234 + 0
      val = (ins < (G*8)) ? 0x1234 : 0x12345678;
      alu = phv_out->get(ins + 9);
      alu = (alu << shift) | phv_out->get(ins + 8);
      ASSERT_TRUE(phv_out->is_valid(ins + 9) && phv_out->is_valid(ins + 8));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 9) &&
                  phv_out->written_bv()->get_bit(ins + 8));
      ASSERT_TRUE(val == alu);
      // 0 + 0x12345678, 0 + 0x1234
      val = (ins < (G*8)) ? 0x1234 : 0x12345678;
      alu = phv_out->get(ins + 11);
      alu = (alu << shift) | phv_out->get(ins + 10);
      ASSERT_TRUE(phv_out->is_valid(ins + 11) && phv_out->is_valid(ins + 10));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 11) &&
                  phv_out->written_bv()->get_bit(ins + 10));
      ASSERT_TRUE(val == alu);
      if (!Instr::isOperandMuxOnlyAlu(12,15)) {
        // 0xFFFFFFFF + 0x12345678, 0xFFFF + 0x1234
        val = (ins < (G*8)) ? 0x1233 : 0x12345677;
        alu = phv_out->get(ins + 13);
        alu = (alu << shift) | phv_out->get(ins + 12);
        ASSERT_TRUE(phv_out->is_valid(ins + 13) && phv_out->is_valid(ins + 12));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 13) &&
                    phv_out->written_bv()->get_bit(ins + 12));
        ASSERT_TRUE(val == alu);
        // 0x80000000 + 0x7FFFFFFF, 0x8000 + 0x7FFF
        val = (ins < (G*8)) ? 0xFFFF : 0xFFFFFFFF;
        alu = phv_out->get(ins + 15);
        alu = (alu << shift) | phv_out->get(ins + 14);
        ASSERT_TRUE(phv_out->is_valid(ins + 15) && phv_out->is_valid(ins + 14));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 15) &&
                    phv_out->written_bv()->get_bit(ins + 14));
        ASSERT_TRUE(val == alu);
      }
    }
    delete phv_out;

    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),ArithmeticsSaturatingSubUnsigned) {
    Instr instrs;

    // Get a dummy PHV
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    int G = Instr::kInstrAluGrpSize;

    // Get a dummy bit vector
    BitVector<MauDefs::kActionHVOutputBusWidth> adib;

    phv->set(0+0, 0xFFFFFFFF); // Max unsigned
    phv->set(0+1, 0x78787878);
    phv->set(0+2, 0x12345678);
    phv->set(0+3, 0x01234567);
    phv->set(0+4, 0x00000000); // Min unsigned
    phv->set(0+5, 0x80000000); // Min signed
    phv->set(0+6, 0x7FFFFFFF); // Max signed

    phv->set((G*4)+0, 0xFF);
    phv->set((G*4)+1, 0x78);
    phv->set((G*4)+2, 0x12);
    phv->set((G*4)+3, 0x01);
    phv->set((G*4)+4, 0x00);
    phv->set((G*4)+5, 0x80);
    phv->set((G*4)+6, 0x7F);
    phv->set((G*6)+1, 0xFF); phv->set((G*6)+0, 0xFF);
    phv->set((G*6)+3, 0x78); phv->set((G*6)+2, 0x78);
    phv->set((G*6)+5, 0x12); phv->set((G*6)+4, 0x34);
    phv->set((G*6)+7, 0x01); phv->set((G*6)+6, 0x23);
    phv->set((G*6)+9, 0x00); phv->set((G*6)+8, 0x00);
    phv->set((G*6)+11,0x80); phv->set((G*6)+10,0x00);
    phv->set((G*6)+13,0x7F); phv->set((G*6)+12,0xFF);

    phv->set((G*8)+0, 0xFFFF);
    phv->set((G*8)+1, 0x7878);
    phv->set((G*8)+2, 0x1234);
    phv->set((G*8)+3, 0x0123);
    phv->set((G*8)+4, 0x0000);
    phv->set((G*8)+5, 0x8000);
    phv->set((G*8)+6, 0x7FFF);
    phv->set((G*10)+1, 0xFFFF); phv->set((G*10)+0, 0xFFFF);
    phv->set((G*10)+3, 0x7878); phv->set((G*10)+2, 0x7878);
    phv->set((G*10)+5, 0x1234); phv->set((G*10)+4, 0x5678);
    phv->set((G*10)+7, 0x0123); phv->set((G*10)+6, 0x4567);
    phv->set((G*10)+9, 0x0000); phv->set((G*10)+8, 0x0000);
    phv->set((G*10)+11,0x8000); phv->set((G*10)+10,0x0000);
    phv->set((G*10)+13,0x7FFF); phv->set((G*10)+12,0xFFFF);

    //
    // Saturating Unsigned Sub.
    //
    // Clear the instructions
    instrs.reset();
    uint16_t op = 0x0BE;
    for (int iii=0; iii<=8; iii+=4) {
      int ins = iii * G;

      // 0xFFFFFFFF - 0xFFFFFFFF, 0xFF 0 0xFF, 0xFFFF00xFFFF
      instrs.set(ins+0, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,0));
      // 0xFFFFFFFF - 0, 0xFF - 0, 0xFFFF - 0
      instrs.set(ins+1, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,4));
      // 0 - 0xFFFFFFFF, 0 - 0xFF, 0 - 0xFFFF
      instrs.set(ins+2, OP_SET(9,0,op) | INS_COMMON_BITS(0,4,0));
      // 0x78787878 - 0x01234567, 0x78 - 0x01, 0x7878 - 0x0123
      instrs.set(ins+3, OP_SET(9,0,op) | INS_COMMON_BITS(0,1,3));
      // 0x12345678 - 0, 0x12 - 0, 0x1234 - 0
      instrs.set(ins+4, OP_SET(9,0,op) | INS_COMMON_BITS(0,2,4));
      // 0 - 0x12345678, 0 - 0x12, 0 - 0x1234
      instrs.set(ins+5, OP_SET(9,0,op) | INS_COMMON_BITS(0,4,2));
      // 0xFFFFFFFF - 0x12345678, 0xFF - 0x12, 0xFFFF - 0x1234
      instrs.set(ins+6, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,2));
      // 0x80000000 - 0x7FFFFFFF, 0x800 - 0x7F, 0x8000 - 0x7FFF
      instrs.set(ins+7, OP_SET(9,0,op) | INS_COMMON_BITS(0,5,6));
      // 0x7FFFFFFF - 0x80000000, 0x7F - 0x80, 0x7FFF - 0x8000
      instrs.set(ins+8, OP_SET(9,0,op) | INS_COMMON_BITS(0,6,5));
      // 0x01234567 - 0x7FFFFFFF, 0x01 - 0x7F, 0x0167 - 0x7FFF
      instrs.set(ins+9, OP_SET(9,0,op) | INS_COMMON_BITS(0,3,6));
    }
    for (int i=6; i<=10; i+=4) {
      int ins = i * G;

      // Merged ALUs.
      if (Instr::kInstrPairDpfIsErr) continue;

      // 0xFFFFFFFF - 0xFFFFFFFF, 0xFFFF - 0xFFFF
      instrs.set(ins+0, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,0));
      instrs.set(ins+1, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,1));
      // 0xFFFFFFFF - 0, 0xFFFF - 0
      instrs.set(ins+2, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,8));
      instrs.set(ins+3, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,9));
      // 0 - 0xFFFFFFFF, 0 - 0xFFFF
      instrs.set(ins+4, OP_SET(9,0,op) | INS_COMMON_BITS(0,8,0));
      instrs.set(ins+5, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,9,1));
      // 0x78787878 - 0x01234567, 0x7878 - 0x0123
      instrs.set(ins+6, OP_SET(9,0,op) | INS_COMMON_BITS(0,2,6));
      instrs.set(ins+7, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,3,7));
      // 0x12345678 - 0, 0x1234 - 0
      instrs.set(ins+8, OP_SET(9,0,op) | INS_COMMON_BITS(0,4,8));
      instrs.set(ins+9, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,5,9));
      // 0 - 0x12345678, 0 - 0x1234
      instrs.set(ins+10,OP_SET(9,0,op) | INS_COMMON_BITS(0,8,4));
      instrs.set(ins+11,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,9,5));
      if (!Instr::isOperandMuxOnlyAlu(12,19)) {
        // 0xFFFFFFFF - 0x12345678, 0xFFFF - 0x1234
        instrs.set(ins+12,OP_SET(9,0,op) | INS_COMMON_BITS(0,0,4));
        instrs.set(ins+13,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,5));
        // 0x80000000 - 0x7FFFFFFF, 0x8000 - 0x7FFF
        instrs.set(ins+14,OP_SET(9,0,op) | INS_COMMON_BITS(0,10,12));
        instrs.set(ins+15,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,11,13));
        // 0x7FFFFFFF - 0x80000000, 0x7FFF - 0x8000
        instrs.set(ins+16,OP_SET(9,0,op) | INS_COMMON_BITS(0,12,10));
        instrs.set(ins+17,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,13,11));
        // 0x01234567 - 0x7FFFFFFF, 0x0167 - 0x7FFF
        instrs.set(ins+18,OP_SET(9,0,op) | INS_COMMON_BITS(0,6,12));
        instrs.set(ins+19,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,7,13));
      }
    }

    // Execute the instructions
    Phv *phv_out = om->phv_create();
    phv_out->start_recording_written();
    uint32_t error_cnt = instrs.execute(phv, adib, phv_out);
    ASSERT_FALSE(error_cnt);

    // Validate the results.
    for (int iii=0; iii<=8; iii+=4) {
      int ins = iii * G;
      uint32_t alu = 0, val = 0;

      // 0xFFFFFFFF - 0xFFFFFFFF, 0xFF - 0xFF, 0xFFFF+0xFFFF
      val = (ins < (G*4)) ? 0x00000000 : (ins < (G*8)) ? 0x00 : 0x0000;
      alu = phv_out->get(ins + 0);
      ASSERT_TRUE(phv_out->is_valid(ins + 0));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 0));
      ASSERT_TRUE(val == alu);

      // 0xFFFFFFFF - 0, 0xFF - 0, 0xFFFF - 0
      val = (ins < (G*4)) ? 0xFFFFFFFF : (ins < (G*8)) ? 0xFF : 0xFFFF;
      alu = phv_out->get(ins + 1);
      ASSERT_TRUE(phv_out->is_valid(ins + 1));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 1));
      ASSERT_TRUE(val == alu);

      // 0 - 0xFFFFFFFF, 0 - 0xFF, 0 - 0xFFFF
      val = (ins < (G*4)) ? 0x00000000 : (ins < (G*8)) ? 0x00 : 0x0000;
      alu = phv_out->get(ins + 2);
      ASSERT_TRUE(phv_out->is_valid(ins + 2));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 2));
      ASSERT_TRUE(val == alu);

      // 0x78787878 - 0x01234567, 0x78 - 0x01, 0x7878 - 0x0123
      val = (ins < (G*4)) ? 0x77553311 : (ins < (G*8)) ? 0x77 : 0x7755;
      alu = phv_out->get(ins + 3);
      ASSERT_TRUE(phv_out->is_valid(ins + 3));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 3));
      ASSERT_TRUE(val == alu);

      // 0x12345678 - 0, 0x12 - 0, 0x1234 - 0
      val = (ins < (G*4)) ? 0x12345678 : (ins < (G*8)) ? 0x12 : 0x1234;
      alu = phv_out->get(ins + 4);
      ASSERT_TRUE(phv_out->is_valid(ins + 4));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 4));
      ASSERT_TRUE(val == alu);

      // 0 - 0x12345678, 0 - 0x12, 0 - 0x1234
      val = (ins < (G*4)) ? 0x00000000 : (ins < (G*8)) ? 0x00 : 0x0000;
      alu = phv_out->get(ins + 5);
      ASSERT_TRUE(phv_out->is_valid(ins + 5));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 5));
      ASSERT_TRUE(val == alu);

      // 0xFFFFFFFF - 0x12345678, 0xFF - 0x12, 0xFFFF - 0x1234
      val = (ins < (G*4)) ? 0xEDCBA987 : (ins < (G*8)) ? 0xED : 0xEDCB;
      alu = phv_out->get(ins + 6);
      ASSERT_TRUE(phv_out->is_valid(ins + 6));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 6));
      ASSERT_TRUE(val == alu);

      // 0x80000000 - 0x7FFFFFFF, 0x80 - 0x7F, 0x8000 - 0x7FFF
      val = (ins < (G*4)) ? 0x00000001 : (ins < (G*8)) ? 0x01 : 0x0001;
      alu = phv_out->get(ins + 7);
      ASSERT_TRUE(phv_out->is_valid(ins + 7));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 7));
      ASSERT_TRUE(val == alu);

      // 0x7FFFFFFF - 0x80000000, 0x7F - 0x80, 0x7FFF - 0x8000
      val = (ins < (G*4)) ? 0x00000000 : (ins < (G*8)) ? 0x00 : 0x0000;
      alu = phv_out->get(ins + 8);
      ASSERT_TRUE(phv_out->is_valid(ins + 8));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 8));
      ASSERT_TRUE(val == alu);

      // 0x01234567 - 0x7FFFFFFF, 0x01 - 0x7F, 0x0123 - 0x7FFF
      val = (ins < (G*4)) ? 0x00000000 : (ins < (G*8)) ? 0x00 : 0x0000;
      alu = phv_out->get(ins + 9);
      ASSERT_TRUE(phv_out->is_valid(ins + 9));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 9));
      ASSERT_TRUE(val == alu);
    }

    // Validate the results for the merged ALUs.
    for (int iii=6; iii<=10; iii+=4) {
      int ins = iii * G;

      if (Instr::kInstrPairDpfIsErr) continue;

      uint32_t alu = 0, val = 0;
      int shift = (ins < (G*8)) ? 8 : 16;
      // 0xFFFFFFFF - 0xFFFFFFFF, 0xFFFF - 0xFFFF
      val = (ins < (G*8)) ? 0x0000 : 0x00000000;
      alu = phv_out->get(ins + 1);
      alu = (alu << shift) | phv_out->get(ins + 0);
      ASSERT_TRUE(phv_out->is_valid(ins + 1) && phv_out->is_valid(ins + 0));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 1) &&
                  phv_out->written_bv()->get_bit(ins + 0));
      ASSERT_TRUE(val == alu);
      // 0xFFFFFFFF - 0, 0xFFFF - 0
      val = (ins < (G*8)) ? 0xFFFF : 0xFFFFFFFF;
      alu = phv_out->get(ins + 3);
      alu = (alu << shift) | phv_out->get(ins + 2);
      ASSERT_TRUE(phv_out->is_valid(ins + 3) && phv_out->is_valid(ins + 2));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 3) &&
                  phv_out->written_bv()->get_bit(ins + 2));
      ASSERT_TRUE(val == alu);
      // 0 - 0xFFFFFFFF, 0 - 0xFFFF
      val = (ins < (G*8)) ? 0x0000 : 0x00000000;
      alu = phv_out->get(ins + 5);
      alu = (alu << shift) | phv_out->get(ins + 4);
      ASSERT_TRUE(phv_out->is_valid(ins + 5) && phv_out->is_valid(ins + 4));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 5) &&
                  phv_out->written_bv()->get_bit(ins + 4));
      ASSERT_TRUE(val == alu);
      // 0x78787878 - 0x01234567, 0x7878 - 0x0123
      val = (ins < (G*8)) ? 0x7755 : 0x77553311;
      alu = phv_out->get(ins + 7);
      alu = (alu << shift) | phv_out->get(ins + 6);
      ASSERT_TRUE(phv_out->is_valid(ins + 7) && phv_out->is_valid(ins + 6));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 7) &&
                  phv_out->written_bv()->get_bit(ins + 6));
      ASSERT_TRUE(val == alu);
      // 0x12345678 - 0, 0x1234 - 0
      val = (ins < (G*8)) ? 0x1234 : 0x12345678;
      alu = phv_out->get(ins + 9);
      alu = (alu << shift) | phv_out->get(ins + 8);
      ASSERT_TRUE(phv_out->is_valid(ins + 9) && phv_out->is_valid(ins + 8));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 9) &&
                  phv_out->written_bv()->get_bit(ins + 8));
      ASSERT_TRUE(val == alu);
      // 0 - 0x12345678, 0 - 0x1234
      val = (ins < (G*8)) ? 0x0000 : 0x00000000;
      alu = phv_out->get(ins + 11);
      alu = (alu << shift) | phv_out->get(ins + 10);
      ASSERT_TRUE(phv_out->is_valid(ins + 11) && phv_out->is_valid(ins + 10));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 11) &&
                  phv_out->written_bv()->get_bit(ins + 10));
      ASSERT_TRUE(val == alu);
      if (!Instr::isOperandMuxOnlyAlu(12,19)) {
        // 0xFFFFFFFF - 0x12345678, 0xFFFF - 0x1234
        val = (ins < (G*8)) ? 0xEDCB : 0xEDCBA987;
        alu = phv_out->get(ins + 13);
        alu = (alu << shift) | phv_out->get(ins + 12);
        ASSERT_TRUE(phv_out->is_valid(ins + 13) && phv_out->is_valid(ins + 12));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 13) &&
                    phv_out->written_bv()->get_bit(ins + 12));
        ASSERT_TRUE(val == alu);
        // 0x80000000 - 0x7FFFFFFF, 0x8000 - 0x7FFF
        val = (ins < (G*8)) ? 0x0001 : 0x00000001;
        alu = phv_out->get(ins + 15);
        alu = (alu << shift) | phv_out->get(ins + 14);
        ASSERT_TRUE(phv_out->is_valid(ins + 15) && phv_out->is_valid(ins + 14));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 15) &&
                    phv_out->written_bv()->get_bit(ins + 14));
        ASSERT_TRUE(val == alu);
        // 0x7FFFFFFF - 0x80000000, 0x7FFF - 0x8000
        val = (ins < (G*8)) ? 0x0000 : 0x00000000;
        alu = phv_out->get(ins + 17);
        alu = (alu << shift) | phv_out->get(ins + 16);
        ASSERT_TRUE(phv_out->is_valid(ins + 17) && phv_out->is_valid(ins + 16));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 17) &&
                    phv_out->written_bv()->get_bit(ins + 16));
        ASSERT_TRUE(val == alu);
        // 0x01234567 - 0x7FFFFFFF, 0x0167 - 0x7FFF
        val = (ins < (G*8)) ? 0x0000 : 0x00000000;
        alu = phv_out->get(ins + 19);
        alu = (alu << shift) | phv_out->get(ins + 18);
        ASSERT_TRUE(phv_out->is_valid(ins + 19) && phv_out->is_valid(ins + 18));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 19) &&
                    phv_out->written_bv()->get_bit(ins + 18));
        ASSERT_TRUE(val == alu);
      }
    }
    delete phv_out;
    delete phv;

    phv = om->phv_create();
    phv->set(114, 0xF9);
    int ins = (7 * G) + 10;
    instrs.reset();
    instrs.set(ins, OP_SET(9,0,op) | INS_COMMON_BITS(0,32,2));
    phv_out = om->phv_create();
    phv_out->start_recording_written();
    error_cnt = instrs.execute(phv, adib, phv_out);
    ASSERT_FALSE(error_cnt);
    EXPECT_EQ(1u, phv_out->written_bv()->get_bit(ins));
    EXPECT_EQ(0u, phv_out->get(ins));

    delete phv_out;
    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),ArithmeticsSaturatingSubSigned) {
    Instr instrs;

    // Get a dummy PHV
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    int G = Instr::kInstrAluGrpSize;

    // Get a dummy bit vector
    BitVector<MauDefs::kActionHVOutputBusWidth> adib;

    phv->set(0+0, 0xFFFFFFFF); // Max unsigned
    phv->set(0+1, 0x78787878);
    phv->set(0+2, 0x12345678);
    phv->set(0+3, 0x01234567);
    phv->set(0+4, 0x00000000); // Min unsigned
    phv->set(0+5, 0x80000000); // Min signed
    phv->set(0+6, 0x7FFFFFFF); // Max signed

    phv->set((G*4)+0, 0xFF);
    phv->set((G*4)+1, 0x78);
    phv->set((G*4)+2, 0x12);
    phv->set((G*4)+3, 0x01);
    phv->set((G*4)+4, 0x00);
    phv->set((G*4)+5, 0x80);
    phv->set((G*4)+6, 0x7F);
    phv->set((G*6)+1, 0xFF); phv->set((G*6)+0, 0xFF);
    phv->set((G*6)+3, 0x78); phv->set((G*6)+2, 0x78);
    phv->set((G*6)+5, 0x12); phv->set((G*6)+4, 0x34);
    phv->set((G*6)+7, 0x01); phv->set((G*6)+6, 0x23);
    phv->set((G*6)+9, 0x00); phv->set((G*6)+8, 0x00);
    phv->set((G*6)+11,0x80); phv->set((G*6)+10,0x00);
    phv->set((G*6)+13,0x7F); phv->set((G*6)+12,0xFF);

    phv->set((G*8)+0, 0xFFFF);
    phv->set((G*8)+1, 0x7878);
    phv->set((G*8)+2, 0x1234);
    phv->set((G*8)+3, 0x0123);
    phv->set((G*8)+4, 0x0000);
    phv->set((G*8)+5, 0x8000);
    phv->set((G*8)+6, 0x7FFF);
    phv->set((G*10)+1, 0xFFFF); phv->set((G*10)+0, 0xFFFF);
    phv->set((G*10)+3, 0x7878); phv->set((G*10)+2, 0x7878);
    phv->set((G*10)+5, 0x1234); phv->set((G*10)+4, 0x5678);
    phv->set((G*10)+7, 0x0123); phv->set((G*10)+6, 0x4567);
    phv->set((G*10)+9, 0x0000); phv->set((G*10)+8, 0x0000);
    phv->set((G*10)+11,0x8000); phv->set((G*10)+10,0x0000);
    phv->set((G*10)+13,0x7FFF); phv->set((G*10)+12,0xFFFF);

    //
    // Saturating Signed Sub.
    //
    // Clear the instructions
    instrs.reset();
    uint16_t op = 0x0FE;
    for (int iii=0; iii<=8; iii+=4) {
      int ins = iii * G;

      // 0xFFFFFFFF - 0xFFFFFFFF, 0xFF 0 0xFF, 0xFFFF00xFFFF
      instrs.set(ins+0, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,0));
      // 0xFFFFFFFF - 0, 0xFF - 0, 0xFFFF - 0
      instrs.set(ins+1, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,4));
      // 0 - 0xFFFFFFFF, 0 - 0xFF, 0 - 0xFFFF
      instrs.set(ins+2, OP_SET(9,0,op) | INS_COMMON_BITS(0,4,0));
      // 0x78787878 - 0x01234567, 0x78 - 0x01, 0x7878 - 0x0123
      instrs.set(ins+3, OP_SET(9,0,op) | INS_COMMON_BITS(0,1,3));
      // 0x12345678 - 0, 0x12 - 0, 0x1234 - 0
      instrs.set(ins+4, OP_SET(9,0,op) | INS_COMMON_BITS(0,2,4));
      // 0 - 0x12345678, 0 - 0x12, 0 - 0x1234
      instrs.set(ins+5, OP_SET(9,0,op) | INS_COMMON_BITS(0,4,2));
      // 0xFFFFFFFF - 0x12345678, 0xFF - 0x12, 0xFFFF - 0x1234
      instrs.set(ins+6, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,2));
      // 0x80000000 - 0x7FFFFFFF, 0x800 - 0x7F, 0x8000 - 0x7FFF
      instrs.set(ins+7, OP_SET(9,0,op) | INS_COMMON_BITS(0,5,6));
      // 0x7FFFFFFF - 0x80000000, 0x7F - 0x80, 0x7FFF - 0x8000
      instrs.set(ins+8, OP_SET(9,0,op) | INS_COMMON_BITS(0,6,5));
      // 0x01234567 - 0x7FFFFFFF, 0x01 - 0x7F, 0x0167 - 0x7FFF
      instrs.set(ins+9, OP_SET(9,0,op) | INS_COMMON_BITS(0,3,6));
    }
    for (int iii=6; iii<=10; iii+=4) {
      int ins = iii * G;

      // Merged ALUs.
      if (Instr::kInstrPairDpfIsErr) continue;

      // 0xFFFFFFFF - 0xFFFFFFFF, 0xFFFF - 0xFFFF
      instrs.set(ins+0, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,0));
      instrs.set(ins+1, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,1));
      // 0xFFFFFFFF - 0, 0xFFFF - 0
      instrs.set(ins+2, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,8));
      instrs.set(ins+3, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,9));
      // 0 - 0xFFFFFFFF, 0 - 0xFFFF
      instrs.set(ins+4, OP_SET(9,0,op) | INS_COMMON_BITS(0,8,0));
      instrs.set(ins+5, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,9,1));
      // 0x78787878 - 0x01234567, 0x7878 - 0x0123
      instrs.set(ins+6, OP_SET(9,0,op) | INS_COMMON_BITS(0,2,6));
      instrs.set(ins+7, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,3,7));
      // 0x12345678 - 0, 0x1234 - 0
      instrs.set(ins+8, OP_SET(9,0,op) | INS_COMMON_BITS(0,4,8));
      instrs.set(ins+9, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,5,9));
      // 0 - 0x12345678, 0 - 0x1234
      instrs.set(ins+10,OP_SET(9,0,op) | INS_COMMON_BITS(0,8,4));
      instrs.set(ins+11,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,9,5));
      if (!Instr::isOperandMuxOnlyAlu(12,15)) {
        // 0xFFFFFFFF - 0x12345678, 0xFFFF - 0x1234
        instrs.set(ins+12,OP_SET(9,0,op) | INS_COMMON_BITS(0,0,4));
        instrs.set(ins+13,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,5));
        // 0x80000000 - 0x7FFFFFFF, 0x8000 - 0x7FFF
        instrs.set(ins+14,OP_SET(9,0,op) | INS_COMMON_BITS(0,10,12));
        instrs.set(ins+15,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,11,13));
      }
    }

    // Execute the instructions
    Phv *phv_out = om->phv_create();
    phv_out->start_recording_written();
    uint32_t error_cnt = instrs.execute(phv, adib, phv_out);
    ASSERT_FALSE(error_cnt);

    // Validate the results.
    for (int iii=0; iii<=8; iii+=4) {
      int ins = iii * G;
      uint32_t alu = 0, val = 0;

      // 0xFFFFFFFF - 0xFFFFFFFF, 0xFF - 0xFF, 0xFFFF+0xFFFF
      val = (ins < (G*4)) ? 0x00000000 : (ins < (G*8)) ? 0x00 : 0x0000;
      alu = phv_out->get(ins + 0);
      ASSERT_TRUE(phv_out->is_valid(ins + 0));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 0));
      ASSERT_TRUE(val == alu);

      // 0xFFFFFFFF - 0, 0xFF - 0, 0xFFFF - 0
      val = (ins < (G*4)) ? 0xFFFFFFFF : (ins < (G*8)) ? 0xFF : 0xFFFF;
      alu = phv_out->get(ins + 1);
      ASSERT_TRUE(phv_out->is_valid(ins + 1));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 1));
      ASSERT_TRUE(val == alu);

      // 0 - 0xFFFFFFFF, 0 - 0xFF, 0 - 0xFFFF
      val = (ins < (G*4)) ? 0x00000001 : (ins < (G*8)) ? 0x01 : 0x0001;
      alu = phv_out->get(ins + 2);
      ASSERT_TRUE(phv_out->is_valid(ins + 2));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 2));
      ASSERT_TRUE(val == alu);

      // 0x78787878 - 0x01234567, 0x78 - 0x01, 0x7878 - 0x0123
      val = (ins < (G*4)) ? 0x77553311 : (ins < (G*8)) ? 0x77 : 0x7755;
      alu = phv_out->get(ins + 3);
      ASSERT_TRUE(phv_out->is_valid(ins + 3));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 3));
      ASSERT_TRUE(val == alu);

      // 0x12345678 - 0, 0x12 - 0, 0x1234 - 0
      val = (ins < (G*4)) ? 0x12345678 : (ins < (G*8)) ? 0x12 : 0x1234;
      alu = phv_out->get(ins + 4);
      ASSERT_TRUE(phv_out->is_valid(ins + 4));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 4));
      ASSERT_TRUE(val == alu);

      // 0 - 0x12345678, 0 - 0x12, 0 - 0x1234
      val = (ins < (G*4)) ? 0xEDCBA988 : (ins < (G*8)) ? 0xEE : 0xEDCC;
      alu = phv_out->get(ins + 5);
      ASSERT_TRUE(phv_out->is_valid(ins + 5));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 5));
      ASSERT_TRUE(val == alu);

      // 0xFFFFFFFF - 0x12345678, 0xFF - 0x12, 0xFFFF - 0x1234
      val = (ins < (G*4)) ? 0xEDCBA987 : (ins < (G*8)) ? 0xED : 0xEDCB;
      alu = phv_out->get(ins + 6);
      ASSERT_TRUE(phv_out->is_valid(ins + 6));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 6));
      ASSERT_TRUE(val == alu);

      // 0x80000000 - 0x7FFFFFFF, 0x80 - 0x7F, 0x8000 - 0x7FFF
      val = (ins < (G*4)) ? 0x80000000 : (ins < (G*8)) ? 0x80 : 0x8000;
      alu = phv_out->get(ins + 7);
      ASSERT_TRUE(phv_out->is_valid(ins + 7));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 7));
      ASSERT_TRUE(val == alu);

      // 0x7FFFFFFF - 0x80000000, 0x7F - 0x80, 0x7FFF - 0x8000
      val = (ins < (G*4)) ? 0x7FFFFFFF : (ins < (G*8)) ? 0x7F : 0x7FFF;
      alu = phv_out->get(ins + 8);
      ASSERT_TRUE(phv_out->is_valid(ins + 8));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 8));
      ASSERT_TRUE(val == alu);

      // 0x01234567 - 0x7FFFFFFF, 0x01 - 0x7F, 0x0123 - 0x7FFF
      val = (ins < (G*4)) ? 0x81234568 : (ins < (G*8)) ? 0x82 : 0x8124;
      alu = phv_out->get(ins + 9);
      ASSERT_TRUE(phv_out->is_valid(ins + 9));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 9));
      ASSERT_TRUE(val == alu);
    }

    // Validate the results for the merged ALUs.
    for (int iii=6; iii<=10; iii+=4) {
      int ins = iii * G;

      if (Instr::kInstrPairDpfIsErr) continue;

      uint32_t alu = 0, val = 0;
      int shift = (ins < (G*8)) ? 8 : 16;
      // 0xFFFFFFFF - 0xFFFFFFFF, 0xFFFF - 0xFFFF
      val = (ins < (G*8)) ? 0x0000 : 0x00000000;
      alu = phv_out->get(ins + 1);
      alu = (alu << shift) | phv_out->get(ins + 0);
      ASSERT_TRUE(phv_out->is_valid(ins + 1) && phv_out->is_valid(ins + 0));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 1) &&
                  phv_out->written_bv()->get_bit(ins + 0));
      ASSERT_TRUE(val == alu);
      // 0xFFFFFFFF - 0, 0xFFFF - 0
      val = (ins < (G*8)) ? 0xFFFF : 0xFFFFFFFF;
      alu = phv_out->get(ins + 3);
      alu = (alu << shift) | phv_out->get(ins + 2);
      ASSERT_TRUE(phv_out->is_valid(ins + 3) && phv_out->is_valid(ins + 2));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 3) &&
                  phv_out->written_bv()->get_bit(ins + 2));
      ASSERT_TRUE(val == alu);
      // 0 - 0xFFFFFFFF, 0 - 0xFFFF
      val = (ins < (G*8)) ? 0x0001 : 0x00000001;
      alu = phv_out->get(ins + 5);
      alu = (alu << shift) | phv_out->get(ins + 4);
      ASSERT_TRUE(phv_out->is_valid(ins + 5) && phv_out->is_valid(ins + 4));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 5) &&
                  phv_out->written_bv()->get_bit(ins + 4));
      ASSERT_TRUE(val == alu);
      // 0x78787878 - 0x01234567, 0x7878 - 0x0123
      val = (ins < (G*8)) ? 0x7755 : 0x77553311;
      alu = phv_out->get(ins + 7);
      alu = (alu << shift) | phv_out->get(ins + 6);
      ASSERT_TRUE(phv_out->is_valid(ins + 7) && phv_out->is_valid(ins + 6));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 7) &&
                  phv_out->written_bv()->get_bit(ins + 6));
      ASSERT_TRUE(val == alu);
      // 0x12345678 - 0, 0x1234 - 0
      val = (ins < (G*8)) ? 0x1234 : 0x12345678;
      alu = phv_out->get(ins + 9);
      alu = (alu << shift) | phv_out->get(ins + 8);
      ASSERT_TRUE(phv_out->is_valid(ins + 9) && phv_out->is_valid(ins + 8));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 9) &&
                  phv_out->written_bv()->get_bit(ins + 8));
      ASSERT_TRUE(val == alu);
      // 0 - 0x12345678, 0 - 0x1234
      val = (ins < (G*8)) ? 0xEDCC : 0xEDCBA988;
      alu = phv_out->get(ins + 11);
      alu = (alu << shift) | phv_out->get(ins + 10);
      ASSERT_TRUE(phv_out->is_valid(ins + 11) && phv_out->is_valid(ins + 10));
      ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 11) &&
                  phv_out->written_bv()->get_bit(ins + 10));
      ASSERT_TRUE(val == alu);
      if (!Instr::isOperandMuxOnlyAlu(12,15)) {
        // 0xFFFFFFFF - 0x12345678, 0xFFFF - 0x1234
        val = (ins < (G*8)) ? 0xEDCB : 0xEDCBA987;
        alu = phv_out->get(ins + 13);
        alu = (alu << shift) | phv_out->get(ins + 12);
        ASSERT_TRUE(phv_out->is_valid(ins + 13) && phv_out->is_valid(ins + 12));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 13) &&
                    phv_out->written_bv()->get_bit(ins + 12));
        ASSERT_TRUE(val == alu);
        // 0x80000000 - 0x7FFFFFFF, 0x8000 - 0x7FFF
        val = (ins < (G*8)) ? 0x8000 : 0x80000000;
        alu = phv_out->get(ins + 15);
        alu = (alu << shift) | phv_out->get(ins + 14);
        ASSERT_TRUE(phv_out->is_valid(ins + 15) && phv_out->is_valid(ins + 14));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 15) &&
                    phv_out->written_bv()->get_bit(ins + 14));
        ASSERT_TRUE(val == alu);
      }
    }
    delete phv_out;
    delete phv;

    Phv *phv_res;

    phv_res = om->phv_create();
    phv = om->phv_create();
    if (!Instr::kInstrPairDpfIsErr) {
      // More merging
      phv->set((G*4)+0, 0x70);
      phv->set((G*4)+5, 0x46);
      phv->set((G*4)+9, 0x14);
      instrs.reset();
      instrs.set((G*4)+10, OP_SET(9,0,0x0FE) | INS_COMMON_BITS(0,24,0x0));
      instrs.set((G*4)+11, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,24,0x5));
      error_cnt = instrs.execute(phv, adib, phv_res);
      ASSERT_FALSE(error_cnt);
      int16_t x = 0 - 0x4670;
      ASSERT_TRUE(phv_res->get((G*4)+10) == (uint16_t)(x & 0xFF));
      ASSERT_TRUE(phv_res->get((G*4)+11) == (uint16_t)((x >> 8) & 0xFF));
    }
    delete phv_res;
    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),ArithmeticsMinMax) {
    Instr instrs;

    // Get a dummy PHV
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    int G = Instr::kInstrAluGrpSize;

    // Get a dummy bit vector
    BitVector<MauDefs::kActionHVOutputBusWidth> adib;

    phv->set(0+0, 0xFFFFFFFF); // Max unsigned
    phv->set(0+1, 0x12345678);
    phv->set(0+2, 0x00000000); // Min unsigned
    phv->set(0+3, 0x80000000); // Min signed
    phv->set(0+4, 0x7FFFFFFF); // Max signed
    phv->set(0+5, 0x01234567);

    phv->set((G*4)+0, 0xFF);
    phv->set((G*4)+1, 0x12);
    phv->set((G*4)+2, 0x00);
    phv->set((G*4)+3, 0x80);
    phv->set((G*4)+4, 0x7F);
    phv->set((G*4)+5, 0x01);
    phv->set((G*6)+1, 0xFF); phv->set((G*6)+0, 0xFF);
    phv->set((G*6)+3, 0x12); phv->set((G*6)+2, 0x34);
    phv->set((G*6)+5, 0x00); phv->set((G*6)+4, 0x00);
    phv->set((G*6)+7, 0x80); phv->set((G*6)+6, 0x00);
    phv->set((G*6)+9, 0x7F); phv->set((G*6)+8, 0xFF);
    phv->set((G*6)+11,0x01); phv->set((G*6)+10,0x23);

    phv->set((G*8)+0, 0xFFFF);
    phv->set((G*8)+1, 0x1234);
    phv->set((G*8)+2, 0x0000);
    phv->set((G*8)+3, 0x8000);
    phv->set((G*8)+4, 0x7FFF);
    phv->set((G*8)+5, 0x0123);
    phv->set((G*10)+1, 0xFFFF); phv->set((G*10)+0, 0xFFFF);
    phv->set((G*10)+3, 0x1234); phv->set((G*10)+2, 0x5678);
    phv->set((G*10)+5, 0x0000); phv->set((G*10)+4, 0x0000);
    phv->set((G*10)+7, 0x8000); phv->set((G*10)+6, 0x0000);
    phv->set((G*10)+9, 0x7FFF); phv->set((G*10)+8, 0xFFFF);
    phv->set((G*10)+11,0x0123); phv->set((G*10)+10,0x4567);

    for (int i=0; i<4; ++i) { // 0..3==minU, minS, maxU, maxS
      // Clear the instructions
      instrs.reset();
      uint16_t op = 0x13E + i*0x40;

      for (int iii=0; iii<=8; iii+=4) {
        int ins = iii * G;

        // 0xFFFFFFFF,0x12345678, 0xFF,0x12, 0xFFFF,0x1234
        instrs.set(ins+0, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,1));
        // 0xFFFFFFFF,0x00000000, 0xFF,0x00, 0xFFFF,0x0000
        instrs.set(ins+1, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,2));
        // 0xFFFFFFFF,0x80000000, 0xFF,0x80, 0xFFFF,0x8000
        instrs.set(ins+2, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,3));
        // 0xFFFFFFFF,0x7FFFFFFF, 0xFF,0x7F, 0xFFFF,0x7FFF
        instrs.set(ins+3, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,4));
        // 0x12345678,0x00000000, 0x12,0x00, 0x1234,0x0000
        instrs.set(ins+4, OP_SET(9,0,op) | INS_COMMON_BITS(0,1,2));
        // 0x12345678,0x80000000, 0x12,0x80, 0x1234,0x8000
        instrs.set(ins+5, OP_SET(9,0,op) | INS_COMMON_BITS(0,1,3));
        // 0x12345678,0x7FFFFFFF, 0x12,0x7F, 0x1234,0x7FFF
        instrs.set(ins+6, OP_SET(9,0,op) | INS_COMMON_BITS(0,1,4));
        // 0x00000000,0x80000000, 0x00,0x80, 0x0000,0x8000
        instrs.set(ins+7, OP_SET(9,0,op) | INS_COMMON_BITS(0,2,3));
        // 0x00000000,0x7FFFFFFF, 0x00,0x7F, 0x0000,0x7FFF
        instrs.set(ins+8, OP_SET(9,0,op) | INS_COMMON_BITS(0,2,4));
        // 0x80000000,0x7FFFFFFF, 0x80,0x7F, 0x8000,0x7FFF
        instrs.set(ins+9, OP_SET(9,0,op) | INS_COMMON_BITS(0,3,4));
        // 0x01234567,0x12345678, 0x01,0x12, 0x0123,0x1234
        instrs.set(ins+10,OP_SET(9,0,op) | INS_COMMON_BITS(0,5,1));
      }
      for (int iii=6; iii<=10; iii+=4) {
        int ins = iii * G;

        // Merged ALUs.
        if (Instr::kInstrPairDpfIsErr) continue;

        // 0xFFFFFFFF,0x12345678, 0xFFFF,0x1234
        instrs.set(ins+0, OP_SET(9,0,op)    | INS_COMMON_BITS(0,0,2));
        instrs.set(ins+1, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,3));
        // 0xFFFFFFFF,0x00000000, 0xFFFF,0x0000
        instrs.set(ins+2, OP_SET(9,0,op)    | INS_COMMON_BITS(0,0,4));
        instrs.set(ins+3, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,5));
        // 0xFFFFFFFF,0x80000000, 0xFFFF,0x8000
        instrs.set(ins+4, OP_SET(9,0,op)    | INS_COMMON_BITS(0,0,6));
        instrs.set(ins+5, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,7));
        // 0xFFFFFFFF,0x7FFFFFFF, 0xFFFF,0x7FFF
        instrs.set(ins+6, OP_SET(9,0,op)    | INS_COMMON_BITS(0,0,8));
        instrs.set(ins+7, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,9));
        // 0x12345678,0x00000000, 0x1234,0x0000
        instrs.set(ins+8, OP_SET(9,0,op)    | INS_COMMON_BITS(0,2,4));
        instrs.set(ins+9, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,3,5));
        // 0x12345678,0x80000000, 0x1234,0x8000
        instrs.set(ins+10,OP_SET(9,0,op)    | INS_COMMON_BITS(0,2,6));
        instrs.set(ins+11,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,3,7));
        if (!Instr::isOperandMuxOnlyAlu(12,15)) {
          // 0x12345678,0x7FFFFFFF, 0x1234,0x7FFF
          instrs.set(ins+12,OP_SET(9,0,op)    | INS_COMMON_BITS(0,2,8));
          instrs.set(ins+13,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,3,9));
          // 0x00000000,0x80000000, 0x0000,0x8000
          instrs.set(ins+14,OP_SET(9,0,op)    | INS_COMMON_BITS(0,4,6));
          instrs.set(ins+15,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,5,7));
        }
      }

      // Execute the instructions
      Phv *phv_out = om->phv_create();
      phv_out->start_recording_written();
      uint32_t error_cnt = instrs.execute(phv, adib, phv_out);
      ASSERT_FALSE(error_cnt);

      // Validate the results.
      for (int iii=0; iii<=8; iii+=4) {
        int ins = iii * G;
        uint32_t alu = 0, val = 0;

        // 0xFFFFFFFF,0x12345678, 0xFF,0x12, 0xFFFF,0x1234
        if (ins < (G*4)) {
          val = 0==i ? 0x12345678 : 1==i ? 0xFFFFFFFF :
                2==i ? 0xFFFFFFFF :        0x12345678;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x12 : 1==i ? 0xFF :
                2==i ? 0xFF :        0x12;
        } else {
          val = 0==i ? 0x1234 : 1==i ? 0xFFFF :
                2==i ? 0xFFFF :        0x1234;
        }
        alu = phv_out->get(ins + 0);
        ASSERT_TRUE(phv_out->is_valid(ins + 0));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 0));
        ASSERT_TRUE(val == alu);

        // 0xFFFFFFFF,0x00000000, 0xFF,0x00, 0xFFFF,0x0000
        if (ins < (G*4)) {
          val = 0==i ? 0x00000000 : 1==i ? 0xFFFFFFFF :
                2==i ? 0xFFFFFFFF :        0x00000000;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x00 : 1==i ? 0xFF :
                2==i ? 0xFF :        0x00;
        } else {
          val = 0==i ? 0x0000 : 1==i ? 0xFFFF :
                2==i ? 0xFFFF :        0x0000;
        }
        alu = phv_out->get(ins + 1);
        ASSERT_TRUE(phv_out->is_valid(ins + 1));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 1));
        ASSERT_TRUE(val == alu);

        // 0xFFFFFFFF,0x80000000, 0xFF,0x80, 0xFFFF,0x8000
        if (ins < (G*4)) {
          val = 0==i ? 0x80000000 : 1==i ? 0x80000000 :
                2==i ? 0xFFFFFFFF :        0xFFFFFFFF;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x80 : 1==i ? 0x80 :
                2==i ? 0xFF :        0xFF;
        } else {
          val = 0==i ? 0x8000 : 1==i ? 0x8000 :
                2==i ? 0xFFFF :        0xFFFF;
        }
        alu = phv_out->get(ins + 2);
        ASSERT_TRUE(phv_out->is_valid(ins + 2));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 2));
        ASSERT_TRUE(val == alu);

        // 0xFFFFFFFF,0x7FFFFFFF, 0xFF,0x7F, 0xFFFF,0x7FFF
        if (ins < (G*4)) {
          val = 0==i ? 0x7FFFFFFF : 1==i ? 0xFFFFFFFF :
                2==i ? 0xFFFFFFFF :        0x7FFFFFFF;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x7F : 1==i ? 0xFF :
                2==i ? 0xFF :        0x7F;
        } else {
          val = 0==i ? 0x7FFF : 1==i ? 0xFFFF :
                2==i ? 0xFFFF :        0x7FFF;
        }
        alu = phv_out->get(ins + 3);
        ASSERT_TRUE(phv_out->is_valid(ins + 3));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 3));
        ASSERT_TRUE(val == alu);

        // 0x12345678,0x00000000, 0x12,0x00, 0x1234,0x0000
        if (ins < (G*4)) {
          val = 0==i ? 0x00000000 : 1==i ? 0x00000000 :
                2==i ? 0x12345678 :        0x12345678;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x00 : 1==i ? 0x00 :
                2==i ? 0x12 :        0x12;
        } else {
          val = 0==i ? 0x0000 : 1==i ? 0x0000 :
                2==i ? 0x1234 :        0x1234;
        }
        alu = phv_out->get(ins + 4);
        ASSERT_TRUE(phv_out->is_valid(ins + 4));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 4));
        ASSERT_TRUE(val == alu);

        // 0x12345678,0x80000000, 0x12,0x80, 0x1234,0x8000
        if (ins < (G*4)) {
          val = 0==i ? 0x12345678 : 1==i ? 0x80000000 :
                2==i ? 0x80000000 :        0x12345678;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x12 : 1==i ? 0x80 :
                2==i ? 0x80 :        0x12;
        } else {
          val = 0==i ? 0x1234 : 1==i ? 0x8000 :
                2==i ? 0x8000 :        0x1234;
        }
        alu = phv_out->get(ins + 5);
        ASSERT_TRUE(phv_out->is_valid(ins + 5));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 5));
        ASSERT_TRUE(val == alu);

        // 0x12345678,0x7FFFFFFF, 0x12,0x7F, 0x1234,0x7FFF
        if (ins < (G*4)) {
          val = 0==i ? 0x12345678 : 1==i ? 0x12345678 :
                2==i ? 0x7FFFFFFF :        0x7FFFFFFF;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x12 : 1==i ? 0x12 :
                2==i ? 0x7F :        0x7F;
        } else {
          val = 0==i ? 0x1234 : 1==i ? 0x1234 :
                2==i ? 0x7FFF :        0x7FFF;
        }
        alu = phv_out->get(ins + 6);
        ASSERT_TRUE(phv_out->is_valid(ins + 6));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 6));
        ASSERT_TRUE(val == alu);

        // 0x00000000,0x80000000, 0x00,0x80, 0x0000,0x8000
        if (ins < (G*4)) {
          val = 0==i ? 0x00000000 : 1==i ? 0x80000000 :
                2==i ? 0x80000000 :        0x00000000;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x00 : 1==i ? 0x80 :
                2==i ? 0x80 :        0x00;
        } else {
          val = 0==i ? 0x0000 : 1==i ? 0x8000 :
                2==i ? 0x8000 :        0x0000;
        }
        alu = phv_out->get(ins + 7);
        ASSERT_TRUE(phv_out->is_valid(ins + 7));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 7));
        ASSERT_TRUE(val == alu);

        // 0x00000000,0x7FFFFFFF, 0x00,0x7F, 0x0000,0x7FFF
        if (ins < (G*4)) {
          val = 0==i ? 0x00000000 : 1==i ? 0x00000000 :
                2==i ? 0x7FFFFFFF :        0x7FFFFFFF;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x00 : 1==i ? 0x00 :
                2==i ? 0x7F :        0x7F;
        } else {
          val = 0==i ? 0x0000 : 1==i ? 0x0000 :
                2==i ? 0x7FFF :        0x7FFF;
        }
        alu = phv_out->get(ins + 8);
        ASSERT_TRUE(phv_out->is_valid(ins + 8));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 8));
        ASSERT_TRUE(val == alu);

        // 0x80000000,0x7FFFFFFF, 0x80,0x7F, 0x8000,0x7FFF
        if (ins < (G*4)) {
          val = 0==i ? 0x7FFFFFFF : 1==i ? 0x80000000 :
                2==i ? 0x80000000 :        0x7FFFFFFF;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x7F : 1==i ? 0x80 :
                2==i ? 0x80 :        0x7F;
        } else {
          val = 0==i ? 0x7FFF : 1==i ? 0x8000 :
                2==i ? 0x8000 :        0x7FFF;
        }
        alu = phv_out->get(ins + 9);
        ASSERT_TRUE(phv_out->is_valid(ins + 9));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 9));
        ASSERT_TRUE(val == alu);

        // 0x01234567,0x12345678, 0x01,0x12, 0x0123,0x1234
        if (ins < (G*4)) {
          val = 0==i ? 0x01234567 : 1==i ? 0x01234567 :
                2==i ? 0x12345678 :        0x12345678;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x01 : 1==i ? 0x01 :
                2==i ? 0x12 :        0x12;
        } else {
          val = 0==i ? 0x0123 : 1==i ? 0x0123 :
                2==i ? 0x1234 :        0x1234;
        }
        alu = phv_out->get(ins + 10);
        ASSERT_TRUE(phv_out->is_valid(ins + 10));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 10));
        ASSERT_TRUE(val == alu);
      }

      // Validate the results for the merged ALUs.
      for (int iii=6; iii<=10; iii+=4) {
        int ins = iii * G;

        if (Instr::kInstrPairDpfIsErr) continue;

        uint32_t alu = 0, val = 0;
        int shift = (ins < (G*8)) ? 8 : 16;
        // 0xFFFFFFFF,0x12345678, 0xFFFF,0x1234
        if (ins >= (G*8)) {
          val = 0==i ? 0x12345678 : 1==i ? 0xFFFFFFFF :
                2==i ? 0xFFFFFFFF :        0x12345678;
        } else {
          val = 0==i ? 0x1234 : 1==i ? 0xFFFF :
                2==i ? 0xFFFF :        0x1234;
        }
        alu = phv_out->get(ins + 1);
        alu = (alu << shift) | phv_out->get(ins + 0);
        ASSERT_TRUE(phv_out->is_valid(ins + 1) && phv_out->is_valid(ins + 0));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 1) &&
                    phv_out->written_bv()->get_bit(ins + 0));
        ASSERT_TRUE(val == alu);
        // 0xFFFFFFFF,0x00000000, 0xFFFF,0x0000
        if (ins >= (G*8)) {
          val = 0==i ? 0x00000000 : 1==i ? 0xFFFFFFFF :
                2==i ? 0xFFFFFFFF :        0x00000000;
        } else {
          val = 0==i ? 0x0000 : 1==i ? 0xFFFF :
                2==i ? 0xFFFF :        0x0000;
        }
        alu = phv_out->get(ins + 3);
        alu = (alu << shift) | phv_out->get(ins + 2);
        ASSERT_TRUE(phv_out->is_valid(ins + 3) && phv_out->is_valid(ins + 2));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 3) &&
                    phv_out->written_bv()->get_bit(ins + 2));
        ASSERT_TRUE(val == alu);
        // 0xFFFFFFFF,0x80000000, 0xFFFF,0x8000
        if (ins >= (G*8)) {
          val = 0==i ? 0x80000000 : 1==i ? 0x80000000 :
                2==i ? 0xFFFFFFFF :        0xFFFFFFFF;
        } else {
          val = 0==i ? 0x8000 : 1==i ? 0x8000 :
                2==i ? 0xFFFF :        0xFFFF;
        }
        alu = phv_out->get(ins + 5);
        alu = (alu << shift) | phv_out->get(ins + 4);
        ASSERT_TRUE(phv_out->is_valid(ins + 5) && phv_out->is_valid(ins + 4));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 5) &&
                    phv_out->written_bv()->get_bit(ins + 4));
        ASSERT_TRUE(val == alu);
        // 0xFFFFFFFF,0x7FFFFFFF, 0xFFFF,0x7FFF
        if (ins >=(G*8)) {
          val = 0==i ? 0x7FFFFFFF : 1==i ? 0xFFFFFFFF :
                2==i ? 0xFFFFFFFF :        0x7FFFFFFF;
        } else {
          val = 0==i ? 0x7FFF : 1==i ? 0xFFFF :
                2==i ? 0xFFFF :        0x7FFF;
        }
        alu = phv_out->get(ins + 7);
        alu = (alu << shift) | phv_out->get(ins + 6);
        ASSERT_TRUE(phv_out->is_valid(ins + 7) && phv_out->is_valid(ins + 6));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 7) &&
                    phv_out->written_bv()->get_bit(ins + 6));
        ASSERT_TRUE(val == alu);
        // 0x12345678,0x00000000, 0x1234,0x0000
        if (ins >= (G*8)) {
          val = 0==i ? 0x00000000 : 1==i ? 0x00000000 :
                2==i ? 0x12345678 :        0x12345678;
        } else {
          val = 0==i ? 0x0000 : 1==i ? 0x0000 :
                2==i ? 0x1234 :        0x1234;
        }
        alu = phv_out->get(ins + 9);
        alu = (alu << shift) | phv_out->get(ins + 8);
        ASSERT_TRUE(phv_out->is_valid(ins + 9) && phv_out->is_valid(ins + 8));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 9) &&
                    phv_out->written_bv()->get_bit(ins + 8));
        ASSERT_TRUE(val == alu);
        // 0x12345678,0x80000000, 0x1234,0x8000
        if (ins >= (G*8)) {
          val = 0==i ? 0x12345678 : 1==i ? 0x80000000 :
                2==i ? 0x80000000 :        0x12345678;
        } else {
          val = 0==i ? 0x1234 : 1==i ? 0x8000 :
                2==i ? 0x8000 :        0x1234;
        }
        alu = phv_out->get(ins + 11);
        alu = (alu << shift) | phv_out->get(ins + 10);
        ASSERT_TRUE(phv_out->is_valid(ins + 11) && phv_out->is_valid(ins + 10));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 11) &&
                    phv_out->written_bv()->get_bit(ins + 10));
        ASSERT_TRUE(val == alu);
        if (!Instr::isOperandMuxOnlyAlu(12,15)) {
          // 0x12345678,0x7FFFFFFF, 0x1234,0x7FFF
          if (ins >= (G*8)) {
            val = 0==i ? 0x12345678 : 1==i ? 0x12345678 :
                  2==i ? 0x7FFFFFFF :        0x7FFFFFFF;
          } else {
            val = 0==i ? 0x1234 : 1==i ? 0x1234 :
                  2==i ? 0x7FFF :        0x7FFF;
          }
          alu = phv_out->get(ins + 13);
          alu = (alu << shift) | phv_out->get(ins + 12);
          ASSERT_TRUE(phv_out->is_valid(ins + 13) && phv_out->is_valid(ins + 12));
          ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 13) &&
                      phv_out->written_bv()->get_bit(ins + 12));
          ASSERT_TRUE(val == alu);
          // 0x00000000,0x80000000, 0x0000,0x8000
          if (ins >= (G*8)) {
            val = 0==i ? 0x00000000 : 1==i ? 0x80000000 :
                  2==i ? 0x80000000 :        0x00000000;
          } else {
            val = 0==i ? 0x0000 : 1==i ? 0x8000 :
                  2==i ? 0x8000 :        0x0000;
          }
          alu = phv_out->get(ins + 15);
          alu = (alu << shift) | phv_out->get(ins + 14);
          ASSERT_TRUE(phv_out->is_valid(ins + 15) && phv_out->is_valid(ins + 14));
          ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 15) &&
                      phv_out->written_bv()->get_bit(ins + 14));
          ASSERT_TRUE(val == alu);
        }
      }

      delete phv_out;
    }
    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),ArithmeticsAddSub) {
    Instr instrs;

    // Get a dummy PHV
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    int G = Instr::kInstrAluGrpSize;

    // Get a dummy bit vector
    BitVector<MauDefs::kActionHVOutputBusWidth> adib;

    phv->set(0+0, 0xFFFFFFFF); // Max unsigned
    phv->set(0+1, 0x12345678);
    phv->set(0+2, 0x00000000); // Min unsigned
    phv->set(0+3, 0x80000000); // Min signed
    phv->set(0+4, 0x7FFFFFFF); // Max signed
    phv->set(0+5, 0x01234567);

    phv->set((G*4)+0, 0xFF);
    phv->set((G*4)+1, 0x12);
    phv->set((G*4)+2, 0x00);
    phv->set((G*4)+3, 0x80);
    phv->set((G*4)+4, 0x7F);
    phv->set((G*4)+5, 0x01);
    phv->set((G*6)+1, 0xFF); phv->set((G*6)+0, 0xFF);
    phv->set((G*6)+3, 0x12); phv->set((G*6)+2, 0x34);
    phv->set((G*6)+5, 0x00); phv->set((G*6)+4, 0x00);
    phv->set((G*6)+7, 0x80); phv->set((G*6)+6, 0x00);
    phv->set((G*6)+9, 0x7F); phv->set((G*6)+8, 0xFF);
    phv->set((G*6)+11,0x01); phv->set((G*6)+10,0x23);

    phv->set((G*8)+0, 0xFFFF);
    phv->set((G*8)+1, 0x1234);
    phv->set((G*8)+2, 0x0000);
    phv->set((G*8)+3, 0x8000);
    phv->set((G*8)+4, 0x7FFF);
    phv->set((G*8)+5, 0x0123);
    phv->set((G*10)+1, 0xFFFF); phv->set((G*10)+0, 0xFFFF);
    phv->set((G*10)+3, 0x1234); phv->set((G*10)+2, 0x5678);
    phv->set((G*10)+5, 0x0000); phv->set((G*10)+4, 0x0000);
    phv->set((G*10)+7, 0x8000); phv->set((G*10)+6, 0x0000);
    phv->set((G*10)+9, 0x7FFF); phv->set((G*10)+8, 0xFFFF);
    phv->set((G*10)+11,0x0123); phv->set((G*10)+10,0x4567);

    for (int i=0; i<4; ++i) { // 0..3==Add, AddC, Sub, SubC
      // Clear the instructions
      instrs.reset();
      uint16_t op = 0x23E + i*0x80;
      for (int iii=0; iii<=8; iii+=4) {
        int ins = iii * G;

        // 0xFFFFFFFF,0x12345678, 0xFF,0x12, 0xFFFF,0x1234
        instrs.set(ins+0, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,1));
        // 0xFFFFFFFF,0x00000000, 0xFF,0x00, 0xFFFF,0x0000
        instrs.set(ins+1, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,2));
        // 0xFFFFFFFF,0x80000000, 0xFF,0x80, 0xFFFF,0x8000
        instrs.set(ins+2, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,3));
        // 0xFFFFFFFF,0x7FFFFFFF, 0xFF,0x7F, 0xFFFF,0x7FFF
        instrs.set(ins+3, OP_SET(9,0,op) | INS_COMMON_BITS(0,0,4));
        // 0x12345678,0x00000000, 0x12,0x00, 0x1234,0x0000
        instrs.set(ins+4, OP_SET(9,0,op) | INS_COMMON_BITS(0,1,2));
        // 0x12345678,0x80000000, 0x12,0x80, 0x1234,0x8000
        instrs.set(ins+5, OP_SET(9,0,op) | INS_COMMON_BITS(0,1,3));
        // 0x12345678,0x7FFFFFFF, 0x12,0x7F, 0x1234,0x7FFF
        instrs.set(ins+6, OP_SET(9,0,op) | INS_COMMON_BITS(0,1,4));
        // 0x00000000,0x80000000, 0x00,0x80, 0x0000,0x8000
        instrs.set(ins+7, OP_SET(9,0,op) | INS_COMMON_BITS(0,2,3));
        // 0x00000000,0x7FFFFFFF, 0x00,0x7F, 0x0000,0x7FFF
        instrs.set(ins+8, OP_SET(9,0,op) | INS_COMMON_BITS(0,2,4));
        // 0x80000000,0x7FFFFFFF, 0x80,0x7F, 0x8000,0x7FFF
        instrs.set(ins+9, OP_SET(9,0,op) | INS_COMMON_BITS(0,3,4));
        // 0x01234567,0x12345678, 0x01,0x12, 0x0123,0x1234
        instrs.set(ins+10,OP_SET(9,0,op) | INS_COMMON_BITS(0,5,1));
      }
      for (int iii=6; iii<=10; iii+=4) {
        int ins = iii * G;

        // Merged ALUs.
        if (Instr::kInstrPairDpfIsErr) continue;

        // 0xFFFFFFFF,0x12345678, 0xFFFF,0x1234
        instrs.set(ins+0, OP_SET(9,0,op)    | INS_COMMON_BITS(0,0,2));
        instrs.set(ins+1, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,3));
        // 0xFFFFFFFF,0x00000000, 0xFFFF,0x0000
        instrs.set(ins+2, OP_SET(9,0,op)    | INS_COMMON_BITS(0,0,4));
        instrs.set(ins+3, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,5));
        // 0xFFFFFFFF,0x80000000, 0xFFFF,0x8000
        instrs.set(ins+4, OP_SET(9,0,op)    | INS_COMMON_BITS(0,0,6));
        instrs.set(ins+5, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,7));
        // 0xFFFFFFFF,0x7FFFFFFF, 0xFFFF,0x7FFF
        instrs.set(ins+6, OP_SET(9,0,op)    | INS_COMMON_BITS(0,0,8));
        instrs.set(ins+7, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,1,9));
        // 0x12345678,0x00000000, 0x1234,0x0000
        instrs.set(ins+8, OP_SET(9,0,op)    | INS_COMMON_BITS(0,2,4));
        instrs.set(ins+9, OP_SET(9,0,0x002) | INS_COMMON_BITS(0,3,5));
        // 0x12345678,0x80000000, 0x1234,0x8000
        instrs.set(ins+10,OP_SET(9,0,op)    | INS_COMMON_BITS(0,2,6));
        instrs.set(ins+11,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,3,7));
        if (!Instr::isOperandMuxOnlyAlu(12,15)) {
          // 0x12345678,0x7FFFFFFF, 0x1234,0x7FFF
          instrs.set(ins+12,OP_SET(9,0,op)    | INS_COMMON_BITS(0,2,8));
          instrs.set(ins+13,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,3,9));
          // 0x00000000,0x80000000, 0x0000,0x8000
          instrs.set(ins+14,OP_SET(9,0,op)    | INS_COMMON_BITS(0,4,6));
          instrs.set(ins+15,OP_SET(9,0,0x002) | INS_COMMON_BITS(0,5,7));
        }
      }

      // Execute the instructions
      Phv *phv_out = om->phv_create();
      phv_out->start_recording_written();
      uint32_t error_cnt = instrs.execute(phv, adib, phv_out);
      ASSERT_FALSE(error_cnt);

      // Validate the results.
      for (int iii=0; iii<=8; iii+=4) {
        int ins = iii * G;
        uint32_t alu = 0, val = 0;

        // 0xFFFFFFFF,0x12345678, 0xFF,0x12, 0xFFFF,0x1234
        if (ins < (G*4)) {
          val = 0==i ? 0x12345677 : 1==i ? 0x12345677 :
                2==i ? 0xEDCBA987 :        0xEDCBA987;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x11 : 1==i ? 0x11 :
                2==i ? 0xED :        0xED;
        } else {
          val = 0==i ? 0x1233 : 1==i ? 0x1233 :
                2==i ? 0xEDCB :        0xEDCB;
        }
        alu = phv_out->get(ins + 0);
        ASSERT_TRUE(phv_out->is_valid(ins + 0));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 0));
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);

        // 0xFFFFFFFF,0x00000000, 0xFF,0x00, 0xFFFF,0x0000
        if (ins < (G*4)) {
          val = 0==i ? 0xFFFFFFFF : 1==i ? 0xFFFFFFFF :
                2==i ? 0xFFFFFFFF :        0xFFFFFFFE;
        } else if (ins < (G*8)) {
          val = 0==i ? 0xFF : 1==i ? 0xFF :
                2==i ? 0xFF :        0xFF;
        } else {
          val = 0==i ? 0xFFFF : 1==i ? 0xFFFF :
                2==i ? 0xFFFF :        0xFFFF;
        }
        alu = phv_out->get(ins + 1);
        ASSERT_TRUE(phv_out->is_valid(ins + 1));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 1));
        if (val != alu) {
          RMT_UT_LOG_INFO("ins[%d] (%s) alu %#x val %#x", ins+1, i == 0 ? "Add " :
                                                          i == 1 ? "AddC" :
                                                          i == 2 ? "Sub " : "SubC",
                                                          alu, val);
        }
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);

        // 0xFFFFFFFF,0x80000000, 0xFF,0x80, 0xFFFF,0x8000
        if (ins < (G*4)) {
          val = 0==i ? 0x7FFFFFFF : 1==i ? 0x7FFFFFFF :
                2==i ? 0x7FFFFFFF :        0x7FFFFFFF;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x7F : 1==i ? 0x7F :
                2==i ? 0x7F :        0x7F;
        } else {
          val = 0==i ? 0x7FFF : 1==i ? 0x7FFF :
                2==i ? 0x7FFF :        0x7FFF;
        }
        alu = phv_out->get(ins + 2);
        ASSERT_TRUE(phv_out->is_valid(ins + 2));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 2));
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);

        // 0xFFFFFFFF,0x7FFFFFFF, 0xFF,0x7F, 0xFFFF,0x7FFF
        if (ins < (G*4)) {
          val = 0==i ? 0x7FFFFFFE : 1==i ? 0x7FFFFFFE :
                2==i ? 0x80000000 :        0x7FFFFFFF;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x7E : 1==i ? 0x7E :
                2==i ? 0x80 :        0x80;
        } else {
          val = 0==i ? 0x7FFE : 1==i ? 0x7FFE :
                2==i ? 0x8000 :        0x8000;
        }
        alu = phv_out->get(ins + 3);
        ASSERT_TRUE(phv_out->is_valid(ins + 3));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 3));
        if (val != alu) {
          RMT_UT_LOG_INFO("ins[%d] (%s) alu %#x val %#x", ins+1, i == 0 ? "Add " :
                                                          i == 1 ? "AddC" :
                                                          i == 2 ? "Sub " : "SubC",
                                                          alu, val);
        }
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);

        // 0x12345678,0x00000000, 0x12,0x00, 0x1234,0x0000
        if (ins < (G*4)) {
          val = 0==i ? 0x12345678 : 1==i ? 0x12345678 :
                2==i ? 0x12345678 :        0x12345678;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x12 : 1==i ? 0x12 :
                2==i ? 0x12 :        0x12;
        } else {
          val = 0==i ? 0x1234 : 1==i ? 0x1234 :
                2==i ? 0x1234 :        0x1234;
        }
        alu = phv_out->get(ins + 4);
        ASSERT_TRUE(phv_out->is_valid(ins + 4));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 4));
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);

        // 0x12345678,0x80000000, 0x12,0x80, 0x1234,0x8000
        if (ins < (G*4)) {
          val = 0==i ? 0x92345678 : 1==i ? 0x92345678 :
                2==i ? 0x92345678 :        0x92345677;
        } else if (ins < (G*8)) {
          // 0x12 - 0x80 == 0001_0010 - 1000_0000
          val = 0==i ? 0x92 : 1==i ? 0x92 :
                2==i ? 0x92 :        0x92;
        } else {
          val = 0==i ? 0x9234 : 1==i ? 0x9234 :
                2==i ? 0x9234 :        0x9234;
        }
        alu = phv_out->get(ins + 5);
        ASSERT_TRUE(phv_out->is_valid(ins + 5));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 5));
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);

        // 0x12345678,0x7FFFFFFF, 0x12,0x7F, 0x1234,0x7FFF
        if (ins < (G*4)) {
          val = 0==i ? 0x92345677 : 1==i ? 0x92345677 :
                2==i ? 0x92345679 :        0x92345679;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x91 : 1==i ? 0x91 :
                2==i ? 0x93 :        0x93;
        } else {
          val = 0==i ? 0x9233 : 1==i ? 0x9233 :
                2==i ? 0x9235 :        0x9235;
        }
        alu = phv_out->get(ins + 6);
        ASSERT_TRUE(phv_out->is_valid(ins + 6));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 6));
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);

        // 0x00000000,0x80000000, 0x00,0x80, 0x0000,0x8000
        if (ins < (G*4)) {
          val = 0==i ? 0x80000000 : 1==i ? 0x80000000 :
                2==i ? 0x80000000 :        0x7FFFFFFF;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x80 : 1==i ? 0x80 :
                2==i ? 0x80 :        0x80;
        } else {
          val = 0==i ? 0x8000 : 1==i ? 0x8000 :
                2==i ? 0x8000 :        0x8000;
        }
        alu = phv_out->get(ins + 7);
        ASSERT_TRUE(phv_out->is_valid(ins + 7));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 7));
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);

        // 0x00000000,0x7FFFFFFF, 0x00,0x7F, 0x0000,0x7FFF
        if (ins < (G*4)) {
          val = 0==i ? 0x7FFFFFFF : 1==i ? 0x7FFFFFFF :
                2==i ? 0x80000001 :        0x80000001;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x7F : 1==i ? 0x7F :
                2==i ? 0x81 :        0x81;
        } else {
          val = 0==i ? 0x7FFF : 1==i ? 0x7FFF :
                2==i ? 0x8001 :        0x8001;
        }
        alu = phv_out->get(ins + 8);
        ASSERT_TRUE(phv_out->is_valid(ins + 8));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 8));
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);

        // 0x80000000,0x7FFFFFFF, 0x80,0x7F, 0x8000,0x7FFF
        if (ins < (G*4)) {
          val = 0==i ? 0xFFFFFFFF : 1==i ? 0xFFFFFFFF :
                2==i ? 0x00000001 :        0x00000000;
        } else if (ins < (G*8)) {
          val = 0==i ? 0xFF : 1==i ? 0xFF :
                2==i ? 0x01 :        0x01;
        } else {
          val = 0==i ? 0xFFFF : 1==i ? 0xFFFF :
                2==i ? 0x0001 :        0x0001;
        }
        alu = phv_out->get(ins + 9);
        ASSERT_TRUE(phv_out->is_valid(ins + 9));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 9));
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);

        // 0x01234567,0x12345678, 0x01,0x12, 0x0123,0x1234
        if (ins < (G*4)) {
          val = 0==i ? 0x13579bdf : 1==i ? 0x13579bdf :
                2==i ? 0xEEEEEEEF :        0xEEEEEEEF;
        } else if (ins < (G*8)) {
          val = 0==i ? 0x13 : 1==i ? 0x13 :
                2==i ? 0xEF :        0xEF;
        } else {
          val = 0==i ? 0x1357 : 1==i ? 0x1357 :
                2==i ? 0xEEEF :        0xEEEF;
        }
        alu = phv_out->get(ins + 10);
        ASSERT_TRUE(phv_out->is_valid(ins + 10));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 10));
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);
      }

      // Validate the results for the merged ALUs.
      for (int iii=6; iii<=10; iii+=4) {
        int ins = iii * G;

        if (Instr::kInstrPairDpfIsErr) continue;

        uint32_t alu = 0, val = 0;
        int shift = (ins < (G*8)) ? 8 : 16;
        // 0xFFFFFFFF,0x12345678, 0xFFFF,0x1234
        if (ins >= (G*8)) {
          val = 0==i ? 0x12345677 : 1==i ? 0x12345677 :
                2==i ? 0xEDCBA987 :        0xEDCBA987;
        } else {
          val = 0==i ? 0x1233 : 1==i ? 0x1233 :
                2==i ? 0xEDCB :        0xEDCB;
        }
        alu = phv_out->get(ins + 1);
        alu = (alu << shift) | phv_out->get(ins + 0);
        ASSERT_TRUE(phv_out->is_valid(ins + 1) && phv_out->is_valid(ins + 0));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 1) &&
                    phv_out->written_bv()->get_bit(ins + 0));
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);
        // 0xFFFFFFFF,0x00000000, 0xFFFF,0x0000
        if (ins >= (G*8)) {
          val = 0==i ? 0xFFFFFFFF : 1==i ? 0xFFFFFFFF :
                2==i ? 0xFFFFFFFF :        0xFFFFFFFF;
        } else {
          val = 0==i ? 0xFFFF : 1==i ? 0xFFFF :
                2==i ? 0xFFFF :        0xFFFF;
        }
        alu = phv_out->get(ins + 3);
        alu = (alu << shift) | phv_out->get(ins + 2);
        ASSERT_TRUE(phv_out->is_valid(ins + 3) && phv_out->is_valid(ins + 2));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 3) &&
                    phv_out->written_bv()->get_bit(ins + 2));
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);
        // 0xFFFFFFFF,0x80000000, 0xFFFF,0x8000
        if (ins >= (G*8)) {
          val = 0==i ? 0x7FFFFFFF : 1==i ? 0x7FFFFFFF :
                2==i ? 0x7FFFFFFF :        0x7FFFFFFF;
        } else {
          val = 0==i ? 0x7FFF : 1==i ? 0x7FFF :
                2==i ? 0x7FFF :        0x7FFF;
        }
        alu = phv_out->get(ins + 5);
        alu = (alu << shift) | phv_out->get(ins + 4);
        ASSERT_TRUE(phv_out->is_valid(ins + 5) && phv_out->is_valid(ins + 4));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 5) &&
                    phv_out->written_bv()->get_bit(ins + 4));
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);
        // 0xFFFFFFFF,0x7FFFFFFF, 0xFFFF,0x7FFF
        if (ins >=(G*8)) {
          val = 0==i ? 0x7FFFFFFE : 1==i ? 0x7FFFFFFE :
                2==i ? 0x80000000 :        0x80000000;
        } else {
          val = 0==i ? 0x7FFE : 1==i ? 0x7FFE :
                2==i ? 0x8000 :        0x8000;
        }
        alu = phv_out->get(ins + 7);
        alu = (alu << shift) | phv_out->get(ins + 6);
        ASSERT_TRUE(phv_out->is_valid(ins + 7) && phv_out->is_valid(ins + 6));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 7) &&
                    phv_out->written_bv()->get_bit(ins + 6));
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);
        // 0x12345678,0x00000000, 0x1234,0x0000
        if (ins >= (G*8)) {
          val = 0==i ? 0x12345678 : 1==i ? 0x12345678 :
                2==i ? 0x12345678 :        0x12345678;
        } else {
          val = 0==i ? 0x1234 : 1==i ? 0x1234 :
                2==i ? 0x1234 :        0x1234;
        }
        alu = phv_out->get(ins + 9);
        alu = (alu << shift) | phv_out->get(ins + 8);
        ASSERT_TRUE(phv_out->is_valid(ins + 9) && phv_out->is_valid(ins + 8));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 9) &&
                    phv_out->written_bv()->get_bit(ins + 8));
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);
        // 0x12345678,0x80000000, 0x1234,0x8000
        if (ins >= (G*8)) {
          val = 0==i ? 0x92345678 : 1==i ? 0x92345678 :
                2==i ? 0x92345678 :        0x92345678;
        } else {
          val = 0==i ? 0x9234 : 1==i ? 0x9234 :
                2==i ? 0x9234 :        0x9234;
        }
        alu = phv_out->get(ins + 11);
        alu = (alu << shift) | phv_out->get(ins + 10);
        ASSERT_TRUE(phv_out->is_valid(ins + 11) && phv_out->is_valid(ins + 10));
        ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 11) &&
                    phv_out->written_bv()->get_bit(ins + 10));
        EXPECT_EQ(val, alu);
        //ASSERT_TRUE(val == alu);
        if (!Instr::isOperandMuxOnlyAlu(12,15)) {
          // 0x12345678,0x7FFFFFFF, 0x1234,0x7FFF
          if (ins >= (G*8)) {
            val = 0==i ? 0x92345677 : 1==i ? 0x92345677 :
                  2==i ? 0x92345679 :        0x92345679;
          } else {
            val = 0==i ? 0x9233 : 1==i ? 0x9233 :
                  2==i ? 0x9235 :        0x9235;
          }
          alu = phv_out->get(ins + 13);
          alu = (alu << shift) | phv_out->get(ins + 12);
          ASSERT_TRUE(phv_out->is_valid(ins + 13) && phv_out->is_valid(ins + 12));
          ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 13) &&
                      phv_out->written_bv()->get_bit(ins + 12));
          EXPECT_EQ(val, alu);
          //ASSERT_TRUE(val == alu);
          // 0x00000000,0x80000000, 0x0000,0x8000
          if (ins >= (G*8)) {
            val = 0==i ? 0x80000000 : 1==i ? 0x80000000 :
                  2==i ? 0x80000000 :        0x80000000;
          } else {
            val = 0==i ? 0x8000 : 1==i ? 0x8000 :
                  2==i ? 0x8000 :        0x8000;
          }
          alu = phv_out->get(ins + 15);
          alu = (alu << shift) | phv_out->get(ins + 14);
          ASSERT_TRUE(phv_out->is_valid(ins + 15) && phv_out->is_valid(ins + 14));
          ASSERT_TRUE(phv_out->written_bv()->get_bit(ins + 15) &&
                      phv_out->written_bv()->get_bit(ins + 14));
          EXPECT_EQ(val, alu);
          //ASSERT_TRUE(val == alu);
        }
      }
      delete phv_out;
    }
    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),DepositField) {
    /* Src1 - Source
     * Src2 - Background
     * R - Right rotate of Source (Src1)
     * L - lsb of background
     * H - msb of background
     */
    Instr instrs;
    RmtObjectManager *om = new RmtObjectManager();
    Phv *phv = om->phv_create();
    int G = Instr::kInstrAluGrpSize;

    std::array<uint8_t,128> bv;
    bv[32+2*24+0] = 0x34;
    bv[32+2*24+1] = 0x12;
    bv[32+2*25+0] = 0xFF;
    bv[32+2*25+1] = 0xFF;
    bv[32+2*26+0] = 0;
    bv[32+2*26+1] = 0;
    BitVector<MauDefs::kActionHVOutputBusWidth> adib(bv);

    phv->set(0, 0x00000000);
    phv->set(1, 0xFFFFFFFF);
    phv->set(2, 0x789ABCDE);
    phv->set(3, 0x12345678);
    phv->set((G*4)+0, 0x00);
    phv->set((G*4)+1, 0xFF);
    phv->set((G*4)+2, 0x78);
    phv->set((G*4)+3, 0x12);
    phv->set((G*8)+0, 0x0000);
    phv->set((G*8)+1, 0xFFFF);
    phv->set((G*8)+2, 0x789A);
    phv->set((G*8)+3, 0x1234);
    phv->set((G*8)+14, 0xFFFF); //was 142
    phv->set((G*8)+15, 0x0000); //was 143

    // 31:0, r31
    instrs.set(0, OP_SET(15,11, 0) | OP_SET(10,6,31)| OP_SET(5,1,31) | OP_SET(0,0,1) | INS_COMMON_BITS(0,1,0));
    // 30:1, r0
    instrs.set(1, OP_SET(15,11, 1) | OP_SET(10,6,0) | OP_SET(5,1,30) | OP_SET(0,0,1) | INS_COMMON_BITS(0,1,0));
    // 16:16, r0
    instrs.set(2, OP_SET(15,11,16) | OP_SET(10,6,0) | OP_SET(5,1,16) | OP_SET(0,0,1) | INS_COMMON_BITS(0,1,0));
    // 23:12, r4
    instrs.set(3, OP_SET(15,11,12) | OP_SET(10,6,4) | OP_SET(5,1,23) | OP_SET(0,0,1) | INS_COMMON_BITS(0,2,3));

    // 7:0, r7
    instrs.set((G*4)+0, OP_SET(9,9,0)|OP_SET(8,6,7)|OP_SET(5,4,0)|OP_SET(3,1,7)|OP_SET(0,0,1)|INS_COMMON_BITS(0,1,0));
    // 6:1, r0
    instrs.set((G*4)+1, OP_SET(9,9,0)|OP_SET(8,6,0)|OP_SET(5,4,1)|OP_SET(3,1,6)|OP_SET(0,0,1)|INS_COMMON_BITS(0,1,0));
    // 6:6, r6
    instrs.set((G*4)+2, OP_SET(9,9,1)|OP_SET(8,6,6)|OP_SET(5,4,2)|OP_SET(3,1,6)|OP_SET(0,0,1)|INS_COMMON_BITS(0,1,0));
    // 3:0, r4
    instrs.set((G*4)+3, OP_SET(9,9,0)|OP_SET(8,6,4)|OP_SET(5,4,0)|OP_SET(3,1,3)|OP_SET(0,0,1)|INS_COMMON_BITS(0,2,3));

    // 15:0, r15
    instrs.set((G*8)+0, OP_SET(12,10,0)|OP_SET(9,6,15)|OP_SET(5,5,0)|OP_SET(4,1,15)|OP_SET(0,0,1)|INS_COMMON_BITS(0,1,0));
    // 14:1, r0
    instrs.set((G*8)+1, OP_SET(12,10,0)|OP_SET(9,6,0)|OP_SET(5,5,1)|OP_SET(4,1,14)|OP_SET(0,0,1)|INS_COMMON_BITS(0,1,0));
    // 12:12, r0
    instrs.set((G*8)+2, OP_SET(12,10,6)|OP_SET(9,6,0)|OP_SET(5,5,0)|OP_SET(4,1,12)|OP_SET(0,0,1)|INS_COMMON_BITS(0,1,0));
    // 12:8, r4
    instrs.set((G*8)+3, OP_SET(12,10,4)|OP_SET(9,6,4)|OP_SET(5,5,0)|OP_SET(4,1,12)|OP_SET(0,0,1)|INS_COMMON_BITS(0,2,3));
    // 15:2, r14
    instrs.set((G*8)+4, OP_SET(12,10,1)|OP_SET(9,6,14)|OP_SET(5,5,0)|OP_SET(4,1,15)|OP_SET(0,0,1)|INS_COMMON_BITS(1,25,15));
    instrs.set((G*8)+5, OP_SET(12,10,1)|OP_SET(9,6,14)|OP_SET(5,5,0)|OP_SET(4,1,15)|OP_SET(0,0,1)|INS_COMMON_BITS(1,26,14));
    instrs.set((G*8)+6, OP_SET(12,10,1)|OP_SET(9,6,15)|OP_SET(5,5,0)|OP_SET(4,1,15)|OP_SET(0,0,1)|INS_COMMON_BITS(1,24,15));


    Phv *phv_out = om->phv_create();
    phv_out->start_recording_written();

    uint32_t ret = instrs.execute(phv, adib, phv_out);
    ASSERT_FALSE(ret);
    uint32_t alu = 0, val = 0;

    ASSERT_TRUE(phv_out->is_valid(0));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(0));
    alu = phv_out->get(0);
    val = 0xFFFFFFFF;
    ASSERT_TRUE(val == alu);
    ASSERT_TRUE(phv_out->is_valid(1));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(1));
    alu = phv_out->get(1);
    val = 0x7FFFFFFE;
    ASSERT_TRUE(val == alu);
    ASSERT_TRUE(phv_out->is_valid(2));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(2));
    alu = phv_out->get(2);
    val = 0x00010000;
    ASSERT_TRUE(val == alu);
    ASSERT_TRUE(phv_out->is_valid(3));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(3));
    alu = phv_out->get(3);
    val = 0x1289A678;
    ASSERT_TRUE(val == alu);

    int i40 = (G*4)+0;
    ASSERT_TRUE(phv_out->is_valid(i40));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i40));
    alu = phv_out->get(i40);
    val = 0xFF;
    ASSERT_TRUE(val == alu);
    int i41 = (G*4)+1;
    ASSERT_TRUE(phv_out->is_valid(i41));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i41));
    alu = phv_out->get(i41);
    val = 0x7E;
    ASSERT_TRUE(val == alu);
    int i42 = (G*4)+2;
    ASSERT_TRUE(phv_out->is_valid(i42));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i42));
    alu = phv_out->get(i42);
    val = 0x40;
    ASSERT_TRUE(val == alu);
    int i43 = (G*4)+3;
    ASSERT_TRUE(phv_out->is_valid(i43));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i43));
    alu = phv_out->get(i43);
    val = 0x17;
    ASSERT_TRUE(val == alu);

    int i80 = (G*8)+0;
    ASSERT_TRUE(phv_out->is_valid(i80));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i80));
    alu = phv_out->get(i80);
    val = 0xFFFF;
    ASSERT_TRUE(val == alu);
    int i81 = (G*8)+1;
    ASSERT_TRUE(phv_out->is_valid(i81));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i81));
    alu = phv_out->get(i81);
    val = 0x7FFE;
    ASSERT_TRUE(val == alu);
    int i82 = (G*8)+2;
    ASSERT_TRUE(phv_out->is_valid(i82));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i82));
    alu = phv_out->get(i82);
    val = 0x1000;
    ASSERT_TRUE(val == alu);
    int i83 = (G*8)+3;
    ASSERT_TRUE(phv_out->is_valid(i83));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i83));
    alu = phv_out->get(i83);
    val = 0x0734;
    ASSERT_TRUE(val == alu);
    int i84 = (G*8)+4;
    ASSERT_TRUE(phv_out->is_valid(i84));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i84));
    alu = phv_out->get(i84);
    val = 0xFFFC;
    EXPECT_EQ(val, alu);
    ASSERT_TRUE(val == alu);
    int i85 = (G*8)+5;
    ASSERT_TRUE(phv_out->is_valid(i85));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i85));
    alu = phv_out->get(i85);
    val = 0x0003;
    EXPECT_EQ(val, alu);
    ASSERT_TRUE(val == alu);
    int i86 = (G*8)+6;
    ASSERT_TRUE(phv_out->is_valid(i86));
    ASSERT_TRUE(phv_out->written_bv()->get_bit(i86));
    alu = phv_out->get(i86);
    val = 0x2468;
    EXPECT_EQ(val, alu);
    ASSERT_TRUE(val == alu);

    delete phv_out;
    delete phv;
    delete om;
  }


  TEST(BFN_TEST_NAME(InstrTest),ReadWriteImem) {
    int chip = 202; // Just the 2 stages
    int pipe = 0;
    int stage = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);


    // DEBUG setup....................
    uint64_t ONE = UINT64_C(1);
    uint64_t HI  = UINT64_C(0xFFFFFFFF00000000);
    uint64_t ALL = UINT64_C(0xFFFFFFFFFFFFFFFF);
    uint64_t FEW = UINT64_C(0xF); // FatalErrorWarn
    uint64_t NON = UINT64_C(0);
    uint64_t TOP = UINT64_C(1) << 63;
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Instantiate whole chip and fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    // Lookup this Pipe/Stage MAU and MAU_INSTR_STORE obj
    Mau *mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    MauInstrStore *mauInstrStore = mau->mau_instr_store();
    ASSERT_TRUE(mauInstrStore != NULL);
    int G = Phv::kWordsPerGroup;

    // Setup random number gen
    std::default_random_engine generator;
    std::uniform_int_distribution<uint64_t> data_distribution;
    generator.seed( 0xDAB0D1B0D0B0DEB0 );

    // Go through all instruction words and all phv words (up to 280 on JBay)
    for (int ins = 0; ins < MauInstrStore::kInstrs; ins++) {
      for (int phv = 0; phv < Phv::kWordsMax; phv++) {

        int width = Phv::which_width(phv);
        int base = 0; // Base to convert abs phv to relative phv offset
        int instrWidth = 0;
        switch (width) {
          case 32: instrWidth = Instr::kBitLen32; base = 0;       break;
          case 16: instrWidth = Instr::kBitLen16; base = G+G+G+G; break;
          case 8:  instrWidth = Instr::kBitLen8;  base = G+G;     break;
          default: RMT_ASSERT(0);
        }
        if (jbay) {
          if      (Instr::isOperand1OnlyAlu(phv)) instrWidth = 7; // Mocha
          else if (Instr::isOperand2OnlyAlu(phv)) instrWidth = 6; // Dark
        }
        volatile uint32_t *a_imem;
        switch (width) {
          case 32: a_imem = RegisterUtils::addr_imem32(pipe,stage,ins,phv-base); break;
          case 16: a_imem = RegisterUtils::addr_imem16(pipe,stage,ins,phv-base); break;
          case 8:  a_imem = RegisterUtils::addr_imem8 (pipe,stage,ins,phv-base); break;
          default: RMT_ASSERT(0);
        }

        // Write a few random numbers and read back
        for (int iterations = 0; iterations < 50; iterations++) {
          uint64_t data64 = data_distribution(generator); // RAND data
          uint32_t data32 = static_cast<uint32_t>((data64 & 0xFFFFFFFFu) ^ (data64>>32));
          uint32_t instr = data32; // Write whole 32b
          uint8_t  color  = static_cast<uint8_t>((data32 >> (instrWidth+0)) & 1);
          uint8_t  parity = static_cast<uint8_t>((data32 >> (instrWidth+1)) & 1);
          // Write whole 32b of data32 as instr - should be masked by reg code
          mauInstrStore->imem_write(ins, phv, instr, color, parity);
          // Now read back and check identical
          uint32_t mask = 0xFFFFFFFFu >> (32-1-1-instrWidth); //Parity,Color,Instr
          uint32_t v_written = data32 & mask;
          uint32_t v_imem = tu.InWord((void*)a_imem);
          EXPECT_EQ(v_written, v_imem);
          //printf("Ins=%d Phv=%d(%d,%d) Wrote=0x%08x(0x%08x) Read=0x%08x %s\n",
          //       ins, phv, Phv::which_group(phv), Phv::which_word(phv),
          //       data32, v_written, v_imem,
          //       (v_written != v_imem) ?" MISMATCH !!!!!!!!!!!!!!!" :"");
        }
      }
    }

  }

  TEST(BFN_TEST_NAME(InstrTest),SpeedTest) {
    int num_runs = 1000;
    Instr instrs;
    RmtObjectManager *om = new RmtObjectManager();

    //Initially turn flag off
    om->update_log_flags(ALL,ALL,ALL,ALL,ALL,NON,~RmtDebug::kRmtDebugInstr1);
    //Flag should be initially off
    ASSERT_FALSE(instrs.rmt_log_check(RmtDebug::verbose(RmtDebug::kRmtDebugInstr1)));

    //Create a test phv
    Phv *phv = om->phv_create();

    //dummy BitVector
    BitVector<MauDefs::kActionHVOutputBusWidth> adib;

    /**FIRST RUN THROUGH WITH FLAGS OFF*/

    //Start timer
    auto start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < num_runs; i++) {
      uint32_t ret = instrs.execute(phv, adib, phv);
      ASSERT_FALSE(ret); //Assert no errors
    }

    //End timer
    auto end = std::chrono::high_resolution_clock::now();
    // Getting number of milliseconds as a double.
    std::chrono::duration<double, std::milli> new_time = end - start;

    /**SECOND RUN THROUGH WITH FLAGS ON*/

    //Turn flag on
    instrs.set_log_flags(RmtDebug::kRmtDebugInstr1);
    //Flag should be on
    ASSERT_TRUE(instrs.rmt_log_check(RmtDebug::verbose(RmtDebug::kRmtDebugInstr1)));

    //Start Timer
    start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < num_runs; i++) {
      uint32_t ret = instrs.execute(phv, adib, phv);
      ASSERT_FALSE(ret);
    }

    //End Timer
    end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> old_time = end - start;

    EXPECT_TRUE(new_time.count() < old_time.count())
    << "Old time: " << old_time.count() << "ms\n"
    << "New time: " << new_time.count() << "ms\n";
  }
}
