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

#ifndef _SHARED_INSTR_
#define _SHARED_INSTR_

#include <common/rmt-assert.h>
#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <rmt-object.h>
#include <rmt-types.h>
#include <rmt-log.h>
#include <common/rmt-util.h>
#include <bitvector.h>
#include <phv.h>
#include <model_core/log-buffer.h>

// instr.cpp used to use BH_GET and BH_SET with absolute values for MSB/LSB.
// This was fine when we just had Tofino. Not so great now we have both Tofino/JBay
// as the absolute values differ between the chips.
// So instr.cpp now uses OP_GET and OP_SET with values relative to kOpCodeShift.
// Hopefully this relative value will remain constant!
//
#define BH_FL(msb, lsb)     ((msb)-(lsb)+1)
#define BH_F2M(msb, lsb)    ((1 << BH_FL(msb,lsb)) - 1)
#define BH_SET(msb, lsb, v) (((v) & BH_F2M(msb,lsb)) << (lsb))
#define BH_GET(msb, lsb, v) (((v) >> (lsb)) & BH_F2M(msb,lsb))
#define OP_SET(msb, lsb, v) BH_SET(((msb)+Instr::kOpCodeShift),((lsb)+Instr::kOpCodeShift),(v))
#define OP_GET(msb, lsb, v) BH_GET(((msb)+Instr::kOpCodeShift),((lsb)+Instr::kOpCodeShift),(v))


namespace MODEL_CHIP_NAMESPACE {

  class Mau;

  class Instr : public RmtLogger {

 public:
    static bool kRelaxInstrFormatCheck; // Defined in rmt-config.cpp
    static bool kRelaxInstrMatchingGressCheck;
    static bool kRelaxEqNeqConfigCheck;
    static bool kInstrCondMoveIsUnconditional;
    static bool kInstrCondMuxIsCondMove;
    static bool kInstrInvalidateIsNop;
    static bool kInstrInvalidateIsErr;
    static bool kInstrPairDpfIsErr;
    static bool kInstrDataDepShiftSupported;
    static bool kInstrReverseSubtractSupported;

    static constexpr int      kGroups = 14;
    static constexpr int      kInstrsMax = Phv::kWordsMax;
    static constexpr int      kInstrAluGrpSize = MauDefs::kInstrAluGrpSize;
    static constexpr int      kGroupsCalculated = kInstrsMax / kInstrAluGrpSize;
    static constexpr uint32_t kOperand1OnlyAlus = MauDefs::kInstrOperand1OnlyAlus;
    static constexpr uint32_t kOperand2OnlyAlus = MauDefs::kInstrOperand2OnlyAlus;
    static_assert( (kInstrAluGrpSize <= 32), "ALU group size must be <= 32" );
    static_assert( (kGroups == kGroupsCalculated),
                   "Calculated/Stated number of ALU groups NOT the same" );

    //
    // Instruction Memory Decode Help.  Apply shift before mask.
    //
    // TOFINO
    // Instructions are 26/23/20 bits wide for 32/16/8 bit ALUs. The 10 lsbs
    // are the "common bits" and the next 10 are opCode.  The lay out is like
    // this:
    // Name   Bits  Width Notes
    // ========================================================================
    // OpExt  28:26  3   Extended (S/W only) opcode
    // Misc   25:23  3   Additional operands for 32 bit ALUs
    // Misc   22:20  3   Additional operands for 16 and 32 bit ALUs
    // OpCode 19:10 10   See enum for values; variable length, msbs may be
    //                   additional operand data.
    // Format 10     1   1:Deposit-field, 0:all other instructions
    // Src1i  9      1   1:Src1 is ADIB, 0:Src1 is from PHV
    // Src1   8:4    5   If PHV && >15, immediate value of Src1-24
    // Src2   3:0    4   Always a PHV word (unless Deposit-field)
    //
    // JBAY
    // Instructions are 27/24/21 bits wide for 32/16/8 bit ALUs. The 11 lsbs
    // are the "common bits" and the next 10 are opCode.  The lay out is like
    // this:
    // Name   Bits  Width Notes
    // ========================================================================
    // OpExt  29:27  3   Extended (S/W only) opcode
    // Misc   26:24  3   Additional operands for 32 bit ALUs
    // Misc   23:21  3   Additional operands for 16 and 32 bit ALUs
    // OpCode 20:11 10   See enum for values; variable length, msbs may be
    //                   additional operand data.
    // Format 11     1   1:Deposit-field, 0:all other instructions
    // Src1i  10     1   1:Src1 is ADIB, 0:Src1 is from PHV
    // Src1   9:5    5   If PHV && >19, immediate value of Src1-24
    // Src2   4:0    5   Always a PHV word (unless Deposit-field)
    //
    // Fundamental values
    static constexpr int kS2Shift = 0;
    static constexpr int kS2Width = MauDefs::kInstrSrc2Width;
    static constexpr int kS1Width = MauDefs::kInstrSrc1Width;
    static constexpr int kS1TypeWidth = 1;
    static constexpr int kFmtWidth = 1;
    static constexpr int kOpExtraWidth = 9; // NOT including Fmt
    static constexpr int kOp16ExtraWidth = 3;
    static constexpr int kOp32ExtraWidth = 3;
    static constexpr int kOpExtendedWidth = 3;
    static constexpr int kDpfOpCodeWidth = 3;
    // These are offsets into the opcode field
    static constexpr int kDpfOpCodeExtraShift = 4;
    static constexpr int kLoadConstOpCodeExtraShift = 5;
    // Calculate widths/masks
    static constexpr int kS1TypeS1Width = kS1TypeWidth + kS1Width;
    static constexpr int kS1TypeS1WidthS2Width = kS1TypeWidth + kS1Width + kS2Width;
    static constexpr int kOpCodeWidth = kOpExtraWidth + kFmtWidth;
    static constexpr int kBitLen8  = kOpCodeWidth + kS1TypeS1WidthS2Width;
    static constexpr int kBitLen16 = kBitLen8 + kOp16ExtraWidth;
    static constexpr int kBitLen32 = kBitLen16 + kOp32ExtraWidth;
    static constexpr int kS2Mask = 0xFFFFFFFF >> (32-kS2Width);
    static constexpr int kS1Mask = 0xFFFFFFFF >> (32-kS1Width);
    static constexpr int kS1TypeMask = 0xFFFFFFFF >> (32-kS1TypeWidth);
    static constexpr int kS1TypeS1Mask = 0xFFFFFFFF >> (32-(kS1TypeWidth+kS1Width));
    static constexpr int kFmtMask = 0xFFFFFFFF >> (32-kFmtWidth);
    static constexpr int kOpCodeMask = 0xFFFFFFFF >> (32-kOpCodeWidth);
    static constexpr int kInstrMask8 = 0xFFFFFFFF >> (32-kBitLen8);
    static constexpr int kInstrMask16 = 0xFFFFFFFF >> (32-kBitLen16);
    static constexpr int kInstrMask32 = 0xFFFFFFFF >> (32-kBitLen32);
    static constexpr int kOpExtendedMask = 0xFFFFFFFF >> (32-kOpExtendedWidth);
    static constexpr int kDpfOpBitsMask = 0xFFFFFFFF >> (32-kDpfOpCodeWidth);
    // Calculate shifts
    static constexpr int kS1Shift = kS2Shift + kS2Width;
    static constexpr int kS1TypeShift = kS1Shift + kS1Width;
    static constexpr int kFmtShift = kS1TypeShift + kS1TypeWidth;
    static constexpr int kOpExtraShift = kFmtShift + kFmtWidth;
    static constexpr int kOp16ExtraShift = kOpExtraShift + kOpExtraWidth;
    static constexpr int kOp32ExtraShift = kOp16ExtraShift + kOp16ExtraWidth;
    static constexpr int kOpCodeShift = kFmtShift; // Same - Fmt is part of OpCode
    static constexpr int kOpExtendedShift = kOp32ExtraShift + kOp32ExtraWidth;
    static constexpr int kDpfOpBitsShift = kOpCodeShift + kDpfOpCodeExtraShift;
    // Constant sizes for LoadConst
    static constexpr int kConstSize32 = 21;
    static constexpr int kConstSize16 = 16;
    static constexpr int kConstSize8  = 8;
    // Field values
    static constexpr int kS1TypeRAM   = 1;
    static constexpr int kS1TypePHV   = 0;


    // Static funcs to discover whether an ALU is src1 or src2 only
    // (this is determined by PER-CHIP configuration vars in mau-defs.h)
    static bool isOperand1Enabled(const uint32_t op) {
      return (((op >> (kS1TypeWidth+kS1Width)) & 1) == 1);
    }
    static bool isOperand2Enabled(const uint32_t op) {
      return (((op >> (kS2Width)) & 1) == 1);
    }
    static bool isOperand1OnlyAlu(const int instr_word) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      return ((kOperand1OnlyAlus & (1u << (instr_word % kInstrAluGrpSize))) != 0u);
    }
    static bool isOperand2OnlyAlu(const int instr_word) {
      RMT_ASSERT((instr_word >= 0) && (instr_word < kInstrsMax));
      return ((kOperand2OnlyAlus & (1u << (instr_word % kInstrAluGrpSize))) != 0u);
    }
    static bool isOperandMuxOnlyAlu(const int instr_word) {
      return (isOperand1OnlyAlu(instr_word) || isOperand2OnlyAlu(instr_word));
    }
    static bool isOperandMuxOnlyAlu(const int instr_word_from, const int instr_word_to) {
      RMT_ASSERT(instr_word_from <= instr_word_to);
      for (int i = instr_word_from; i <= instr_word_to; i++) {
        if (isOperandMuxOnlyAlu(i)) return true;
      }
      return false;
    }

    // Some helper funcs to make instructions
    static uint32_t make_load_const(const uint32_t val) {
      uint32_t lo_mask = (0xFFFFFFFFu >>(32-kOpCodeShift));
      uint32_t hi_mask = (0xFFFFFFFFu >>(32-kConstSize32)) & ~lo_mask;
      uint32_t lo_val = (val & lo_mask) << 0;
      uint32_t hi_val = (val & hi_mask) << kLoadConstOpCodeExtraShift;
      return hi_val | (0x8 << kOpCodeShift) | lo_val;
    }

    // Instruction Operand Source types
    static constexpr int kSrc1ImmediateMin = kInstrAluGrpSize;
    static constexpr int kSrc1ImmediateSubtract = MauDefs::kInstrSrc1Subtract;

    enum class InstrSrcType {
      kPhvField, // Source comes from a PHV group
      kPhvImmediate, // Source is an immediate value
      kADIBField, // Source comes from the Action Data Input Bus
      kUnknownInstrSrc // Only used for error cases
    };
    enum class InstrWidth {
      kInstrSz32,
      kInstrSz16,
      kInstrSz8,
      // XXX: undefined enumerator enables possibility of none of the
      // above so that sanity checking code *might* be reachable
      kInstrSzUndefined
    };
    enum class InstrType {
      kNop,
      kLoadConst,
      kFunnelShift,
      kShl,
      kShru,
      kShrs,
      kShlDataDep,  // JBay only
      kShruDataDep, // JBay only
      kShrsDataDep, // JBay only
      kPairDpf,
      kByteRotateMerge,
      kConditionalMux,
      kConditionalMove,
      kInvalidate,
      kBitMaskedSet,
      kAluSetZ,
      kAluNor,
      kAluAndCA,
      kAluNotA,
      kAluAndCB,
      kAluNotB,
      kAluXor,
      kAluNand,
      kAluAnd,
      kAluXNor,
      kAluB,
      kAluOrCB,
      kAluA,
      kAluOrCA,
      kAluOr,
      kAluSetHi,
      kGtEqU, // JBay only
      kGtEqS, // JBay only
      kLtU,   // JBay only
      kLtS,   // JBay only
      kLtEqU, // JBay only
      kLtEqS, // JBay only
      kGtU,   // JBay only
      kGtS,   // JBay only
      kEq,    // JBay only
      kNEq,   // JBay only
      kEq64,  // JBay only
      kNEq64, // JBay only
      kSAddU,
      kSAddS,
      kSSubU,
      kSSubS,
      kMinU,
      kMinS,
      kMaxU,
      kMaxS,
      kAdd,
      kAddC,
      kSub,
      kSubR,  // JBay only
      kSubC,
      kSubRC, // JBay only
      kDepositField,
      kInvalid,
      kOperand1, // JBay only - pseudo InstrType corresponding to use of operand1 MUX
      kOperand2  // JBay only - pseudo InstrType corresponding to use of operand2 MUX
    };


    Instr()
        : RmtLogger(nullptr,RmtTypes::kRmtTypeInstr),
          pipe_index_(0), mau_index_(0), mau_(NULL) {
      reset();
    };
    Instr(RmtObjectManager *om, int p_idx, int m_idx, Mau *mau)
        : RmtLogger(om, RmtTypes::kRmtTypeInstr),
          pipe_index_(p_idx), mau_index_(m_idx), mau_(mau) {
      reset();
    };
    ~Instr() { };

    inline void reset() {
      for (int i = 0; i < kInstrsMax; i++) words_[i] = 0u;
      deferred_alus_.fill_all_zeros();
      cout_.fill_all_zeros();
      equal_.fill_all_zeros(); notequal_.fill_all_zeros();
      execute_count_ = 0;
    }

    inline const char* instr_op_name(InstrType op) {
      switch (op) {
        case InstrType::kNop:
          return "Nop";
        case InstrType::kLoadConst:
          return "LoadConst";
        case InstrType::kFunnelShift:
          return "FunnelShift";
        case InstrType::kShl:
          return "Shl";
        case InstrType::kShru:
          return "Shru";
        case InstrType::kShrs:
          return "Shrs";
        case InstrType::kShlDataDep:
          return "ShlDataDep";
        case InstrType::kShruDataDep:
          return "ShruDataDep";
        case InstrType::kShrsDataDep:
          return "ShrsDataDep";
        case InstrType::kPairDpf:
          return "PairDpf";
        case InstrType::kByteRotateMerge:
          return "ByteRotateMerge";
        case InstrType::kConditionalMux:
          return "ConditionalMux";
        case InstrType::kConditionalMove:
          return "ConditionalMove";
        case InstrType::kInvalidate:
          return "Invalidate";
        case InstrType::kBitMaskedSet:
          return "BitMaskedSet";
        case InstrType::kAluSetZ:
          return "SetZ";
        case InstrType::kAluNor:
          return "Nor";
        case InstrType::kAluAndCA:
          return "AndCA";
        case InstrType::kAluNotA:
          return "NotA";
        case InstrType::kAluAndCB:
          return "AndCB";
        case InstrType::kAluNotB:
          return "NotB";
        case InstrType::kAluXor:
          return "Xor";
        case InstrType::kAluNand:
          return "NAnd";
        case InstrType::kAluAnd:
          return "And";
        case InstrType::kAluXNor:
          return "XNor";
        case InstrType::kAluB:
          return "B";
        case InstrType::kAluOrCB:
          return "CB";
        case InstrType::kAluA:
          return "A";
        case InstrType::kAluOrCA:
          return "OrCA";
        case InstrType::kAluOr:
          return "Or";
        case InstrType::kAluSetHi:
          return "SetHi";
        case InstrType::kGtEqU:
          return "GtEqU";
        case InstrType::kGtEqS:
          return "GtEqS";
        case InstrType::kLtU:
          return "LtU";
        case InstrType::kLtS:
          return "LtS";
        case InstrType::kLtEqU:
          return "LtEqU";
        case InstrType::kLtEqS:
          return "LtEqS";
        case InstrType::kGtU:
          return "GtU";
        case InstrType::kGtS:
          return "GtS";
        case InstrType::kEq:
          return "Eq";
        case InstrType::kNEq:
          return "NEq";
        case InstrType::kEq64:
          return "Eq64";
        case InstrType::kNEq64:
          return "NEq64";
        case InstrType::kSAddU:
          return "AddU";
        case InstrType::kSAddS:
          return "AddS";
        case InstrType::kSSubU:
          return "SubU";
        case InstrType::kSSubS:
          return "SubS";
        case InstrType::kMinU:
          return "MinU";
        case InstrType::kMinS:
          return "MinS";
        case InstrType::kMaxU:
          return "MaxU";
        case InstrType::kMaxS:
          return "MaxS";
        case InstrType::kAdd:
          return "Add";
        case InstrType::kAddC:
          return "AddC";
        case InstrType::kSub:
          return "Sub";
        case InstrType::kSubR:
          return "SubR";
        case InstrType::kSubC:
          return "SubC";
        case InstrType::kSubRC:
          return "SubRC";
        case InstrType::kDepositField:
          return "DepositField";
        case InstrType::kInvalid:
          return "Invalid";
        case InstrType::kOperand1:
          return "Operand1";
        case InstrType::kOperand2:
          return "Operand2";
        default:
          return "Invalid";
      }
    }
    inline void set(const int instr_word, uint32_t val) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      words_[instr_word] |= val;
    }
    inline void clobber(const int instr_word, uint32_t val) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      words_[instr_word] = val;
    }
    inline uint32_t get(const int instr_word) const {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      return words_[instr_word];
    }


    // Handy functions to pick fields from a single instr uint32_t.
    inline int instrGetSrc1(const uint32_t instr) {
      return (instr >> kS1Shift) & kS1Mask;
    }
    inline int instrGetSrc2(const uint32_t instr) {
      return (instr >> kS2Shift) & kS2Mask;
    }
    inline int instrGetSrc1TypeField(const uint32_t instr) {
      return (instr >> kS1TypeShift) & kS1TypeMask;
    }
    inline int instrGetFormatTypeField(const uint32_t instr) {
      return (instr >> kFmtShift) & kFmtMask;
    }
    inline enum InstrSrcType instrGetSrc1Type(const uint32_t instr) {
      if (kS1TypeRAM == instrGetSrc1TypeField(instr)) {
        return InstrSrcType::kADIBField;
      } else if (kS1TypePHV == instrGetSrc1TypeField(instr) &&
                 kSrc1ImmediateMin > instrGetSrc1(instr)) {
        return InstrSrcType::kPhvField;
      } else if (kS1TypePHV == instrGetSrc1TypeField(instr) &&
                 kSrc1ImmediateMin <= instrGetSrc1(instr)) {
        return InstrSrcType::kPhvImmediate;
      } else {
          // Since we're only checking a single bit there is no way we could
          // land here, but just incase the source type field expands to a
          // wider field in the future we'll catch it.
        RMT_ASSERT(0 && "Unhandled instruction operand type");
        return InstrSrcType::kUnknownInstrSrc;
      }
    }
    // Handy functions to pick fields from the common instruction array
    inline int getSrc1(const int instr_word) {
      return instrGetSrc1(get(instr_word));
    }
    inline int getSrc2(const int instr_word) {
      return instrGetSrc2(get(instr_word));
    }
    inline int getSrc1TypeField(const int instr_word) {
      return instrGetSrc1TypeField(get(instr_word));
    }
    inline int getFormatTypeField(const int instr_word) {
      return instrGetFormatTypeField(get(instr_word));
    }
    inline enum InstrSrcType getSrc1Type(const int instr_word) {
      return instrGetSrc1Type(get(instr_word));
    }


    // Handy functions related to ALU groups.
    inline int getGrp(const int instr_word) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      return instr_word / kInstrAluGrpSize;
    }
    inline int getElt(const int instr_word) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      return instr_word % kInstrAluGrpSize;
    }
    inline int instrWordToPhvGrp(const int instr_word) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      // ALU Instruction groups map one-to-one to PHV groups
      return getGrp(instr_word);
    }
    inline int getBitWidth(const int instr_word) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      return MauDefs::kAlu_WidthPerGroup[getGrp(instr_word)];
    }
    inline int getByteWidth(const int instr_word) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      return getBitWidth(instr_word)/8;
    }
    inline InstrWidth getWidth(const int instr_word) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      int bit_width = getBitWidth(instr_word);
      InstrWidth ret = InstrWidth::kInstrSz32;
      if (32 == bit_width) {
          ret = InstrWidth::kInstrSz32;
      } else if (16 == bit_width) {
          ret = InstrWidth::kInstrSz16;
      } else if (8 == bit_width) {
          ret = InstrWidth::kInstrSz8;
      } else {
          RMT_ASSERT (0 && "Invalid instruction width");
      }
      return ret;
    }
    inline InstrWidth getWidth(const int instr_word, bool merged) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      int bit_width = getBitWidth(instr_word);
      InstrWidth ret = InstrWidth::kInstrSz32;
      if (32 == bit_width) {
          RMT_ASSERT(!merged);
          ret = InstrWidth::kInstrSz32;
      } else if (16 == bit_width) {
          ret = merged ? InstrWidth::kInstrSz32 : InstrWidth::kInstrSz16;
      } else if (8 == bit_width) {
          ret = merged ? InstrWidth::kInstrSz16 : InstrWidth::kInstrSz8;
      } else {
          RMT_ASSERT (0 && "Invalid instruction width");
      }
      return ret;
    }
    inline uint32_t getInstr(const int instr_word, bool merged) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      uint32_t ret = 0u; // NOP
      int instr_odd = instr_word | 1;
      int instr_even = instr_odd ^ 1;
      int32_t hi = !merged ? 0 :
          get(instr_odd) >> kDpfOpBitsShift & kDpfOpBitsMask;
      int32_t lo = get(merged ? instr_even : instr_word);

      if (InstrWidth::kInstrSz32 == getWidth(instr_word)) {
        ret = lo & kInstrMask32;
      } else if (InstrWidth::kInstrSz16 == getWidth(instr_word)) {
        ret = (lo & kInstrMask16) | hi << kBitLen16;
      } else if (InstrWidth::kInstrSz8  == getWidth(instr_word)) {
        ret = (lo & kInstrMask8) | hi << kBitLen8;
      }
      return ret;
    }


    inline bool supportsDpf(InstrType op) {
      switch (op) {
        case InstrType::kNop:
        case InstrType::kLoadConst:
          return false;
        case InstrType::kFunnelShift:
        case InstrType::kShl:
        case InstrType::kShru:
        case InstrType::kShrs:
        case InstrType::kShlDataDep:
        case InstrType::kShruDataDep:
        case InstrType::kShrsDataDep:
          return true;
        case InstrType::kPairDpf:
          return false;
        case InstrType::kByteRotateMerge:
          return true;
        case InstrType::kConditionalMux:
        case InstrType::kConditionalMove:
        case InstrType::kInvalidate:
        case InstrType::kBitMaskedSet:
        case InstrType::kAluSetZ:
        case InstrType::kAluNor:
        case InstrType::kAluAndCA:
        case InstrType::kAluNotA:
        case InstrType::kAluAndCB:
        case InstrType::kAluNotB:
        case InstrType::kAluXor:
        case InstrType::kAluNand:
        case InstrType::kAluAnd:
        case InstrType::kAluXNor:
        case InstrType::kAluB:
        case InstrType::kAluOrCB:
        case InstrType::kAluA:
        case InstrType::kAluOrCA:
        case InstrType::kAluOr:
        case InstrType::kAluSetHi:
        case InstrType::kEq64:
        case InstrType::kNEq64:
          return false;
        case InstrType::kGtEqU:
        case InstrType::kGtEqS:
        case InstrType::kLtU:
        case InstrType::kLtS:
        case InstrType::kLtEqU:
        case InstrType::kLtEqS:
        case InstrType::kGtU:
        case InstrType::kGtS:
        case InstrType::kEq:
        case InstrType::kNEq:
        case InstrType::kSAddU:
        case InstrType::kSAddS:
        case InstrType::kSSubU:
        case InstrType::kSSubS:
        case InstrType::kMinU:
        case InstrType::kMinS:
        case InstrType::kMaxU:
        case InstrType::kMaxS:
        case InstrType::kAdd:
        case InstrType::kAddC:
        case InstrType::kSub:
        case InstrType::kSubR:
        case InstrType::kSubC:
        case InstrType::kSubRC:
        case InstrType::kDepositField:
          return true;
        case InstrType::kInvalid:
        case InstrType::kOperand1:
        case InstrType::kOperand2:
        default:
          return false;
      }
    }

    // Handy functions to get an instruction's operands either from the PHV,
    // from the Action Data Input Bus, or from the instruction itself.
    inline uint32_t getOperand1(const int instr_word, Phv *phv,
                           const BitVector<MauDefs::kActionHVOutputBusWidth> &adib,
                           bool &error) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      RMT_ASSERT (phv);
      unsigned s = getSrc1(instr_word);
      uint32_t s1 = 0;
      error = false;
      if (InstrSrcType::kPhvField == getSrc1Type(instr_word) &&
          kSrc1ImmediateMin > s) {
        s1 = getOperandPhv(s, instrWordToPhvGrp(instr_word), phv, error);
        if (error) {
          return 0;
        }
        return s1 & 0xFFFFFFFF;
      }
      if (InstrSrcType::kPhvImmediate == getSrc1Type(instr_word)) {
        s1 = getOperandImmediate(getSrc1(instr_word));
        return s1 & 0xFFFFFFFF;
      }
      if (InstrSrcType::kADIBField == getSrc1Type(instr_word) &&
          (MauDefs::kActionHVOutputBusWidth/(8*sizeof(uint32_t)) > s)) {
        s1 = getOperandADIB(getByteWidth(instr_word),
                                         s,
                                         adib);
        return s1;
      }
      RMT_LOG(RmtDebug::error(kRelaxInstrFormatCheck),
              "Invalid instruction operand 1 (0x%x), instruction %d=0x%x",
              s, instr_word, get(instr_word));
      return 0;
    }
    inline int getOperand2(const int instr_word, Phv *phv, bool &error) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      RMT_ASSERT (phv);
      int s = getSrc2(instr_word);
      int s2 = getOperandPhv(s, instrWordToPhvGrp(instr_word), phv, error);
      if (error) {
        return 0;
      }
      return s2;
    }
    inline bool isMerged(const int instr_word) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      // Certain chips (eg JBay) do not support Pair-DPF
      if (kInstrPairDpfIsErr)
        return false;
      // 32-bit ALUs cannot be merged.
      if (InstrWidth::kInstrSz32 == getWidth(instr_word))
        return false;
      // Operand MUX only ALUs cannot be merged
      if (isOperandMuxOnlyAlu(instr_word))
        return false;
      // The odd instruction must be Pair-DPF to be merged.
      if (InstrType::kPairDpf != getOpCode(instr_word | 1))
        return false;
      // The even instruction must support Pair-DPF to be merged.
      return supportsDpf(getOpCode(instr_word & 0xFFFFFFFE));
    }
    inline bool has_s1(InstrType op) const {
      if ((op == InstrType::kShlDataDep) ||
          (op == InstrType::kShruDataDep) ||
          (op == InstrType::kShrsDataDep))
        return kInstrDataDepShiftSupported;
      else
        return op != InstrType::kNop &&
               op != InstrType::kLoadConst &&
               op != InstrType::kShl &&
               op != InstrType::kShru &&
               op != InstrType::kShrs &&
               op != InstrType::kInvalidate &&
               op != InstrType::kAluSetZ &&
               op != InstrType::kAluNotB &&
               op != InstrType::kAluB &&
               op != InstrType::kAluSetHi &&
               op != InstrType::kOperand2;
    }
    inline bool has_s2(InstrType op) const {
      return op != InstrType::kNop &&
             op != InstrType::kLoadConst &&
             op != InstrType::kInvalidate &&
             op != InstrType::kAluSetZ &&
             op != InstrType::kAluNotA &&
             op != InstrType::kAluA &&
             op != InstrType::kAluSetHi &&
             op != InstrType::kOperand1;
    }

    void getOperand1Dbl(const int instr_word,
                        const BitVector<MauDefs::kActionHVOutputBusWidth> &adib,
                        uint32_t &hi, uint32_t &lo);
    uint64_t getOperand1Dbl(const int instr_word,
                            const BitVector<MauDefs::kActionHVOutputBusWidth> &adib);
    void getMergedOperands(const int instr_word, Phv *phv,
                           const BitVector<MauDefs::kActionHVOutputBusWidth> &adib,
                           int32_t &src1, int32_t &src2,
                           bool &error1, bool &error2);
    Instr::InstrType getOpCode(const int instr_word);
    Instr::InstrType getOpExtended(const int instr_word, bool operandsOnly);
    int execute(Phv *src_phv,
                const BitVector<MauDefs::kActionHVOutputBusWidth> &adib,
                Phv *phv_out);
    int execute_one(int instr_word,
                    Phv *src_phv,
                    const BitVector<MauDefs::kActionHVOutputBusWidth> &adib,
                    Phv *phv_out,
                    model_core::LogBuffer* log_buf);
    int executeLoadConst(const uint32_t instr, InstrWidth width);
    int executeShiftFunnel(const uint32_t instr, InstrWidth width,
                           uint32_t s1, uint32_t s2, model_core::LogBuffer* log_buf);
    int executeShiftLeft(const uint32_t instr, InstrWidth width,
                         uint32_t s, model_core::LogBuffer* log_buf);
    unsigned executeShiftRightUnsigned(const uint32_t instr,
                                       InstrWidth width, uint32_t s, model_core::LogBuffer* log_buf);
    int executeShiftRightSigned(const uint32_t instr,
                                InstrWidth width, int32_t s, model_core::LogBuffer* log_buf);
    int executeShiftLeftDataDep(const uint32_t instr, InstrWidth width,
                                uint32_t src1, uint32_t src2, model_core::LogBuffer* log_buf);
    unsigned executeShiftRightUnsignedDataDep(const uint32_t instr, InstrWidth width,
                                              uint32_t src1, uint32_t src2, model_core::LogBuffer* log_buf);
    int executeShiftRightSignedDataDep(const uint32_t instr, InstrWidth width,
                                       uint32_t src1, int32_t src2, model_core::LogBuffer* log_buf);
    int executeByteRotMerge(const uint32_t instr, InstrWidth width,
                            uint32_t s1, uint32_t s2, model_core::LogBuffer* log_buf);
    int executeBitMaskedSet(const uint32_t instr, InstrWidth width,
                            uint32_t mask, uint32_t masked_src,
                            uint32_t s2, model_core::LogBuffer* log_buf);
    int executeCondMove(const uint32_t instr,
                        uint32_t s1, bool s1_v, bool s2_v,
                        bool dst_v, bool &error, bool &noop, bool &invalidate,
                        model_core::LogBuffer* log_buf);
    int executeCondMux(const uint32_t instr,
                       uint32_t s1, uint32_t s2, bool s1_v, bool s2_v,
                       bool dst_v, bool &error, bool &noop, model_core::LogBuffer* log_buf);
    int executeArithmeticCompareUnsigned(InstrType op, InstrWidth width,
                                         uint32_t src1, uint32_t src2);
    int executeArithmeticCompareSigned(InstrType op, InstrWidth width,
                                       int32_t src1, int32_t src2);
    int executeArithmeticCompareEquality(int instr_word, InstrType op, InstrWidth width,
                                         uint32_t src1, uint32_t src2);
    int executeArithmeticCompareEquality64(int instr_word, InstrType op, InstrWidth width,
                                           uint32_t src1, uint32_t src2);
    int executeSatAddUnsigned(InstrWidth width, uint32_t src1, uint32_t src2);
    int executeSatAddSigned(InstrWidth width, uint32_t src1, uint32_t src2);
    int executeSatSubUnsigned(InstrWidth width, uint32_t src1, uint32_t src2);
    int executeSatSubSigned(InstrWidth width, uint32_t src1, uint32_t src2);
    int executeMinUnsigned(InstrWidth width, uint32_t src1, uint32_t src2);
    int executeMinSigned(InstrWidth width, int32_t src1, int32_t src2);
    int executeMaxUnsigned(InstrWidth width, uint32_t src1, uint32_t src2);
    int executeMaxSigned(InstrWidth width, int32_t src1, int32_t src2);
    int executeSub(int instr_word, InstrWidth width, int32_t src1, int32_t src2);
    int executeSubR(int instr_word, InstrWidth width, int32_t src1, int32_t src2);
    int executeSubC(int instr_word, InstrWidth width, int32_t src1, int32_t src2);
    int executeSubRC(int instr_word, InstrWidth width, int32_t src1, int32_t src2);
    int executeDepositField(const uint32_t instr, InstrWidth width,
                            uint32_t s1, uint32_t s2, model_core::LogBuffer* log_buf);
    inline int get_execute_count() { return execute_count_; }

 private:
    DISALLOW_COPY_AND_ASSIGN(Instr);

    int getOperandImmediate(const int src) {
      RMT_ASSERT(kSrc1ImmediateMin <= src && kS1Mask >= src);
      return src - kSrc1ImmediateSubtract;
    }
    int getOperandPhv(const int src, const int grp, Phv *phv, bool &error) {
      RMT_ASSERT((Phv::kWordsMax / Phv::kGroupsMax) > src);
      RMT_ASSERT(kInstrAluGrpSize > src); // Possible on JBay as grpSize=20 but src field=5b
      RMT_ASSERT(kGroups > grp);
      RMT_ASSERT(phv);
      error = !phv->is_valid(kInstrAluGrpSize*grp + src);
      return phv->get(kInstrAluGrpSize*grp + src);
    }
    int getOperandADIB(const int width, const int src,
                       const BitVector<MauDefs::kActionHVOutputBusWidth> &adib) {
      RMT_ASSERT(1 == width || 2 == width || 4 == width);
      RMT_ASSERT(32 > src);
      int offset = (2 == width ? 32 : 0) + width * src;
      int val = 0;
      for (int i=width-1; i>=0; --i) {
        val = (val << 8) | adib.get_byte(offset+i);
      }
      return val;
    }
    uint64_t getDoubleOperandADIB(const int width, const int src,
                                  const BitVector<MauDefs::kActionHVOutputBusWidth> &adib) {
      RMT_ASSERT(1 == width || 2 == width || 4 == width);
      RMT_ASSERT(32 > src);
      int offset = (2 == width ? 32 : 0) + width * src;
      uint64_t val = 0;
      for (int i=2*width-1; i>=0; --i) {
        val = (val << 8) | adib.get_byte(offset+i);
      }
      return val;
    }

    inline void setCout(const int instr_word, int val) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      RMT_ASSERT (InstrWidth::kInstrSz32 == getWidth(instr_word));
      RMT_ASSERT (!(instr_word & 1));
      RMT_ASSERT (val == 0 || val == 1);
      cout_.set_bit( (val==1), instr_word);
    }
    inline int getCout(const int instr_word) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      RMT_ASSERT (InstrWidth::kInstrSz32 == getWidth(instr_word));
      RMT_ASSERT (!(instr_word & 1));
      return cout_.bit_set(instr_word) ?1 :0;
    }
    inline void setEQout(const int instr_word, bool equal) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      RMT_ASSERT (InstrWidth::kInstrSz32 == getWidth(instr_word));
      RMT_ASSERT (instr_word & 1);
      equal_.set_bit(equal, instr_word);
    }
    inline bool getEQout(const int instr_word) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      RMT_ASSERT (InstrWidth::kInstrSz32 == getWidth(instr_word));
      RMT_ASSERT (instr_word & 1);
      return equal_.bit_set(instr_word);
    }
    inline void setNEQout(const int instr_word, bool equal) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      RMT_ASSERT (InstrWidth::kInstrSz32 == getWidth(instr_word));
      RMT_ASSERT (instr_word & 1);
      notequal_.set_bit(equal, instr_word);
    }
    inline bool getNEQout(const int instr_word) {
      RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
      RMT_ASSERT (InstrWidth::kInstrSz32 == getWidth(instr_word));
      RMT_ASSERT (instr_word & 1);
      return notequal_.bit_set(instr_word);
    }

    uint32_t              words_[kInstrsMax];
    BitVector<kInstrsMax> error_alus_;
    BitVector<kInstrsMax> deferred_alus_;
    BitVector<kInstrsMax> cout_;     // Carry-out of EVEN 32b ALUs
    BitVector<kInstrsMax> equal_;    // Equality signal out of ODD 32b ALUs
    BitVector<kInstrsMax> notequal_; // Inequality signal out of ODD 32b ALUs
    int                   execute_count_;
    int                   pipe_index_;
    int                   mau_index_ ;
    Mau                  *mau_;

    int pipe_index() const { return pipe_index_; }
    int s_index()    const { return mau_index_; }
    int rt_index()   const { return -1; }
    int c_index()    const { return -1; }
  };
}
#endif // _SHARED_INSTR_
