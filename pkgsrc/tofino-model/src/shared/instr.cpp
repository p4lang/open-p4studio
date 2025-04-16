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
#include <iostream>
#include <rmt-log.h>
#include <instr.h>

namespace MODEL_CHIP_NAMESPACE {

  void Instr::getOperand1Dbl(const int instr_word,
                             const BitVector<MauDefs::kActionHVOutputBusWidth> &adib,
                             uint32_t &hi, uint32_t &lo) {
    RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
    // Pair-DPF no longer applies to Bit-Masked-Set so assert.
    RMT_ASSERT(!isMerged(instr_word) && "Merged ALU not supported for double-width operand");

    unsigned s = getSrc1(instr_word);
    // Validate it is from ADIB.
    RMT_ASSERT(InstrSrcType::kADIBField == getSrc1Type(instr_word));
    unsigned s_lo = getSrc1(instr_word);
    int width = getByteWidth(instr_word);
    // Validate the index is within range.
    if (s_lo >= 32) {
      RMT_LOG(RmtDebug::error(kRelaxInstrFormatCheck),
              "Invalid instruction double operand 1. "
              "Op %d Instr %d [%#x]",
              s_lo, instr_word, get(instr_word));
      hi=0, lo=0;
      return;
    }

    // Get the operand in two steps, the upper bits are in the odd index of
    // the even/odd pair of ADIB fields pointed to by the src1 pointer. The
    // lower bits are in the index pointed to by src1 pointer.
    hi = getOperandADIB(width, s | 0x01, adib);
    lo = getOperandADIB(width, s, adib);

    // Mask off the operands to the appropriate width, 32-bit, 16-bit, or
    // 8-bit.
    hi &= (4!=width) ? ((1==width) ? 0xFF : 0xFFFF) : 0xFFFFFFFF;
    lo &= (4!=width) ? ((1==width) ? 0xFF : 0xFFFF) : 0xFFFFFFFF;
  }

  void Instr::getMergedOperands(const int instr_word, Phv *phv,
                                const BitVector<MauDefs::kActionHVOutputBusWidth> &adib,
                                int32_t &src1, int32_t &src2,
                                bool &error1, bool &error2) {
    RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
    if (isMerged(instr_word)) {
      int instr_odd = instr_word | 1;
      int instr_even = instr_odd ^ 1;
      uint32_t s1_hi = getOperand1(instr_odd, phv, adib, error1);
      uint32_t s2_hi = getOperand2(instr_odd, phv, error2);
      // Note that we don't check the valid bit for the even ALU's operands.
      bool x = false;
      // Don't allow operand validity of the odd ALU to affect the even's.
      uint32_t s1_lo = /*error1 ? 0 :*/ getOperand1(instr_even, phv, adib, x);
      uint32_t s2_lo = /*error2 ? 0 :*/ getOperand2(instr_even, phv, x);
      int shift = getBitWidth(instr_word);
      //src1 = !error1 ? (s1_hi << shift) | (s1_lo & ((1<<shift)-1)) : 0;
      //src2 = !error2 ? (s2_hi << shift) | (s2_lo & ((1<<shift)-1)) : 0;
      src1 = /*!error1 ?*/ (s1_hi << shift) | (s1_lo & ((1<<shift)-1)) /*: 0*/;
      src2 = /*!error2 ?*/ (s2_hi << shift) | (s2_lo & ((1<<shift)-1)) /*: 0*/;
    } else {
      src1 = getOperand1(instr_word, phv, adib, error1);
      src2 = getOperand2(instr_word, phv, error2);
    }
  }


  // NOTE:  Instr::getOpCode now in separate file
  // Either src/shared/instr-opcode.cpp (if Tofino/TofinoB0)
  // Or     src/jbay/instr-opcode.cpp   (if JBay)


  Instr::InstrType Instr::getOpExtended(const int instr_word, bool operandsOnly) {
    if (!operandsOnly) return getOpCode(instr_word);

    RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
    RMT_ASSERT (isOperandMuxOnlyAlu(instr_word));
    InstrType op = InstrType::kNop;
    // Special handling for ALUs that only have operand muxes.
    // May return pseudo InstrType kOperand1 or kOperand2
    // or kNop if the ALU is disabled for power-saving
    uint32_t op_word = get(instr_word);
    uint32_t s1 = getElt(instr_word); // S1Type=0 (PHV) at this point
    uint32_t s2 = s1;
    uint32_t extended_op_word = 0u;
    if (isOperand1OnlyAlu(instr_word) && isOperand1Enabled(op_word)) {
      s1 = (op_word & kS1TypeS1Mask); // Include S1Type from op_word
      op = InstrType::kOperand1;
      extended_op_word = 1 << kOpExtendedShift;
    } else if (isOperand2OnlyAlu(instr_word) && isOperand2Enabled(op_word)) {
      s2 = (op_word & kS2Mask);
      op = InstrType::kOperand2;
      extended_op_word = 2 << kOpExtendedShift;
    }
    if ((op == InstrType::kOperand1) || (op == InstrType::kOperand2)) {
      // All later code goes back to the words_ array so we
      // fixup instr_word to include correct s1 and s2 values.
      // If inspected, op will look like a kNop under kOpCodeMask
      // but like kOperand1/kOperand2 under kOpExtendedMask
      clobber(instr_word, extended_op_word | (s1 << kS1Shift) | (s2 << kS2Shift));
    }
    return op;
  }



  // Execute ONE instr_word
  int Instr::execute_one(int instr_word,
                         Phv *src_phv,
                         const BitVector<MauDefs::kActionHVOutputBusWidth> &adib,
                         Phv *phv_out,
                         model_core::LogBuffer* log_buf) {
    RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));
    RMT_ASSERT (src_phv && phv_out);
    int  error_cnt = 0;

    // Is this an operandsOnly ALU
    bool operandsOnly = isOperandMuxOnlyAlu(instr_word);

    // Determine if the ALU is merged and get the op code to execute.  Note
    // that when ALUs are merged the op code always comes from the even ALU.
    // An operandsOnly ALU will *always* use original instr_word (no merge)
    bool is_odd = instr_word & 1;
    bool merged = (operandsOnly) ?false :isMerged(instr_word);
    int  instr_even = (is_odd) ?instr_word-1 :instr_word;
    int  instr_merged = (merged) ?instr_even :instr_word;
    InstrType op = getOpExtended(instr_merged, operandsOnly);

    // - Get the two source operands from the instruction's common bits.
    //   Note that if the ALUs are merged the returned operands will be the
    //   combined operands.
    // - Only get the operands if the instruction uses them.
    // - The src1 operand is 64 bits as one instruction actually uses a 64
    //   bit operand.  It is also written into a 32-bit field for the rest
    //   of the instructions to use.
    int32_t src1 = 0;
    int32_t src2 = 0;
    bool error1 = false, error2 = false;
    if (InstrType::kNop != op && InstrType::kLoadConst != op) {
      getMergedOperands(instr_word, src_phv, adib, src1, src2,
                        error1, error2);
    }

    // Note that we are no longer checking the validity of source operands
    // (that is the valid bit of the PHV container).  Conditional Mux/Move
    // will still use the valid bits in the control flow of the move/mux but
    // invalid source containers will no longer convert operations to NOPs.
    if (InstrType::kConditionalMux != op && InstrType::kConditionalMove != op) {
      error1 = error2 = false;
    }

    // Get the instruction itself along with the width.  Note that merged
    // ALUs will will combine bits from two instructions to create one.
    uint32_t instr = getInstr(instr_word, merged);
    InstrWidth width = getWidth(instr_word, merged);

    // Initialize flags that the individual instructions will modify.
    bool noop = false;
    bool error = error1 || error2;
    bool invalidate = false;
    LOG_APPEND_CHECK(log_buf, "ALU[%3d] %s (0x%x)", instr_word, instr_op_name(op), words_[instr_word]);
    if (has_s1(op)) {
      int off = getSrc1(instr_word);
      if (InstrSrcType::kPhvField == getSrc1Type(instr_word)) {
        int s1 = off;
        int grp = instrWordToPhvGrp(instr_word);
        bool s1err = (s1 >= kInstrAluGrpSize);
        off = grp*kInstrAluGrpSize + ((s1err) ?0 :s1); // Use 0 on error
        if ((mau_ != NULL) &&
            (mau_->phv_get_gress(instr_word) != mau_->phv_get_gress(off))) {
          RMT_LOG_IF_NOT_NULL(log_buf, RmtDebug::error(
                  kRelaxInstrMatchingGressCheck), 
                  "%s uses src1 PHV[%3d] from opposite thread! (s1=%d,s1err=%d)\n", 
                  instr_op_name(op), off, s1, s1err);
          if (!kRelaxInstrMatchingGressCheck) { THROW_ERROR(-2); } // For DV
        }
        int s1_val = src_phv->get(off);
        LOG_APPEND_CHECK(log_buf, " Src1 PHV[%d]=0x%x (%d)%s", off, s1_val,
                s1_val , (error1 || s1err) ? "(Invalid)":"");
      } else if (InstrSrcType::kPhvImmediate == getSrc1Type(instr_word)) {
        LOG_APPEND_CHECK(log_buf, " Src1 Imm=%d", getOperandImmediate(off));
      } else {
        int adib_start=0, adib_end=0;
        if (InstrWidth::kInstrSz32 == width) {
          adib_start = 4*off;
          adib_end = adib_start+3;
        } else if (InstrWidth::kInstrSz16 == width) {
          adib_start = 32+2*off;
          adib_end = adib_start+1;
        } else {
          adib_start = off;
          adib_end = adib_start;
        }
        LOG_APPEND_CHECK(log_buf, " Src1 ADIB[%d:%d]=0x%x (%d)", adib_start, adib_end,
                getOperandADIB(getByteWidth(instr_word), off, adib),
                getOperandADIB(getByteWidth(instr_word), off, adib));
      }
    }
    if (has_s2(op)) {
      int s2 = getSrc2(instr_word);
      int grp = instrWordToPhvGrp(instr_word);
      bool s2err = (s2 >= kInstrAluGrpSize);
      int off = grp*kInstrAluGrpSize + ((s2err) ?0 :s2); // Use 0 on error
      if ((mau_ != NULL) &&
          (mau_->phv_get_gress(instr_word) != mau_->phv_get_gress(off))) {
        RMT_LOG_IF_NOT_NULL(log_buf, RmtDebug::error(
                kRelaxInstrMatchingGressCheck), 
                "%s uses src2 PHV[%3d] from opposite thread! (s2=%d,s2err=%d)\n", 
                instr_op_name(op), off, s2, s2err);
        if (!kRelaxInstrMatchingGressCheck) { THROW_ERROR(-2); } // For DV
      }
      LOG_APPEND_CHECK(log_buf, " Src2 PHV[%d]=0x%x (%d)%s", off, src_phv->get(off),
              src_phv->get(off), (error2 || s2err) ? "(Invalid)":"");
    }

    // Increment a count to track the number of non-NOP instructions seen.
    if (InstrType::kNop != op) {
      ++execute_count_;
      if (isOperandMuxOnlyAlu(instr_word)) {
        RMT_ASSERT((op == InstrType::kOperand1) || (op == InstrType::kOperand2));
      }
    }

    uint32_t ret = 0;
    switch (op) {
      case InstrType::kNop:
        noop = true;
        break;
      case InstrType::kLoadConst:
        error = false;
        ret = executeLoadConst(instr, width);
        break;
      case InstrType::kFunnelShift:
        ret = executeShiftFunnel(instr, width, src1, src2, log_buf);
        break;
      case InstrType::kShl:
        error = error2;
        ret = executeShiftLeft(instr, width, src2, log_buf);
        break;
      case InstrType::kShru:
        error = error2;
        ret = executeShiftRightUnsigned(instr, width, src2, log_buf);
        break;
      case InstrType::kShrs:
        error = error2;
        ret = executeShiftRightSigned(instr, width, src2, log_buf);
        break;
      case InstrType::kShlDataDep:
        error = error2;
        ret = executeShiftLeftDataDep(instr, width, src1, src2, log_buf);
        break;
      case InstrType::kShruDataDep:
        error = error2;
        ret = executeShiftRightUnsignedDataDep(instr, width, src1, src2, log_buf);
        break;
      case InstrType::kShrsDataDep:
        error = error2;
        ret = executeShiftRightSignedDataDep(instr, width, src1, src2, log_buf);
        break;
      case InstrType::kPairDpf:
        // Should only reach here for Pair-DPF executed in a 32-bit ALU or
        // for 16/8-bit ALUs which were incorrectly given a Pair-DPF
        // instruction.
        // Actual merged ALU cases would use the instruction from the adjacent
        // ALU.
        // NOTE, Pair-DPF not supported on some chips (eg JBay)
        noop = true;
        error = (kInstrPairDpfIsErr) ?true :false;
        if (error) {
          error_cnt++;
          RMT_LOG(RmtDebug::error(kRelaxInstrFormatCheck),
                  "Instr[%3d] Pair-DPF is NOT supported!", instr_word);
        }
        break;
      case InstrType::kByteRotateMerge:
        // Instruction does not exist for 8 bit ALUs.
        if (InstrWidth::kInstrSz8 == width) {
          noop = true;
          error = false;
        } else {
          ret = executeByteRotMerge(instr, width, src1, src2, log_buf);
        }
        break;
      case InstrType::kConditionalMux:
        if (!kInstrCondMuxIsCondMove) {
          ret = executeCondMux(instr, src1, src2, !error1, !error2,
                               phv_out->is_valid(merged ? instr_word+1 : instr_word),
                               error, noop, log_buf);
          break;
        }
        LOG_APPEND_ONE(log_buf, " CondMux->CondMove");

        // Here kInstrCondMuxIsCondMove = true
        // XXX: Duplicate kConditionalMove logic
        // - there is no single way of indicating 'fallthru'
        //   for all [gcc 4.8.3 ... gcc 9.3.0]
        if (kInstrCondMoveIsUnconditional) {
          LOG_APPEND_ONE(log_buf, " Unconditional");
          error = noop = invalidate = false;
          ret = src1;
        } else {
          ret = executeCondMove(instr, src1, !error1, !error2,
                                phv_out->is_valid(merged ? instr_word+1 : instr_word),
                                error, noop, invalidate, log_buf);
        }
        break;
      case InstrType::kConditionalMove:
        if (kInstrCondMoveIsUnconditional) {
          LOG_APPEND_ONE(log_buf, " Unconditional");
          error = noop = invalidate = false;
          ret = src1;
        } else {
          ret = executeCondMove(instr, src1, !error1, !error2,
                                phv_out->is_valid(merged ? instr_word+1 : instr_word),
                                error, noop, invalidate, log_buf);
        }
        break;
      case InstrType::kInvalidate:
        invalidate = (kInstrInvalidateIsNop || kInstrInvalidateIsErr) ?false :true;
        error = (kInstrInvalidateIsErr) ?true :false;
        if (error) {
          error_cnt++;
          RMT_LOG(RmtDebug::error(kRelaxInstrFormatCheck),
                  "Instr[%3d] Invalidate should not be used", instr_word);
        }
        break;
      case InstrType::kBitMaskedSet:
        {
          if (InstrSrcType::kADIBField != getSrc1Type(instr_word)) {
            // If src1 is not from the ADIB then Bit Masked Set will be a noop.
            noop = true;
            error = false;
            RMT_LOG(RmtDebug::error(kRelaxInstrFormatCheck),
                    "Instr[%3d] BitMaskedSet but Src1 not from ADIB",
                    instr_word);
            break;
          }
          if (merged) {
            if (InstrSrcType::kADIBField != getSrc1Type(is_odd ? instr_word-1 :
                                                        instr_word+1)) {
              // If src1 is not from the ADIB then Bit Masked Set will be a noop.
              noop = true;
              error = false;
              RMT_LOG(RmtDebug::error(kRelaxInstrFormatCheck),
                      "Instr[%3d] BitMaskedSet but Src1 not from ADIB",
                      instr_word);
              break;
            }
          }
          // This instruction needs a double width argument for src1 (which is
          // always from the ADIB for this instruction).  The extra bits come
          // from the adjacent entry of an even/odd pair.
          // For each ALU (when merged) get the extra bits from the ADIB.
          uint32_t mask = 0, masked_src = 0;
          getOperand1Dbl(instr_word, adib, mask, masked_src);
          ret = executeBitMaskedSet(instr, width, mask, masked_src, src2, log_buf);
          break;
        }
      case InstrType::kAluSetZ:
        error = false;
        ret = 0;
        break;
      case InstrType::kAluNor:
        ret = ~src1 & ~src2;
        break;
      case InstrType::kAluAndCA:
        ret = ~src1 & src2;
        break;
      case InstrType::kAluNotA:
        error = error1;
        ret = ~src1;
        break;
      case InstrType::kAluAndCB:
        ret = src1 & ~src2;
        break;
      case InstrType::kAluNotB:
        error = error2;
        ret = ~src2;
        break;
      case InstrType::kAluXor:
        ret = src1 ^ src2;
        break;
      case InstrType::kAluNand:
        ret = ~src1 | ~src2;
        break;
      case InstrType::kAluAnd:
        ret = src1 & src2;
        break;
      case InstrType::kAluXNor:
        ret = ~(src1 ^ src2);
        break;
      case InstrType::kAluB:
        error = error2;
        ret = src2;
        break;
      case InstrType::kAluOrCB:
        ret = src1 | ~src2;
        break;
      case InstrType::kAluA:
        error = error1;
        ret = src1;
        break;
      case InstrType::kAluOrCA:
        ret = ~src1 | src2;
        break;
      case InstrType::kAluOr:
        ret = src1 | src2;
        break;
      case InstrType::kAluSetHi:
        error = false;
        ret = 0xFFFFFFFF;
        break;
      case InstrType::kGtEqU:
      case InstrType::kLtU:
      case InstrType::kLtEqU:
      case InstrType::kGtU:
        ret = executeArithmeticCompareUnsigned(op, width, src1, src2);
        break;
      case InstrType::kGtEqS:
      case InstrType::kLtS:
      case InstrType::kLtEqS:
      case InstrType::kGtS:
        ret = executeArithmeticCompareSigned(op, width, src1, src2);
        break;
      case InstrType::kEq:
      case InstrType::kNEq:
        ret = executeArithmeticCompareEquality(instr_word, op, width, src1, src2);
        break;
      case InstrType::kEq64:
      case InstrType::kNEq64:
        if (is_odd || deferred_alus_.bit_set(instr_word)) {
          // If this ALU is:
          // 1. ODD
          // 2. EVEN but has previously been deferred
          // then just execute it now
          ret = executeArithmeticCompareEquality64(instr_word, op, width, src1, src2);
        } else {
          // EVEN ALU and NOT yet been deferred
          // We need to run the the ALU above (odd ALU) *before* we execute
          // this even ALU as it may be using signals the odd ALU outputs.
          // So we defer execution of this ALU till later
          deferred_alus_.set_bit(true, instr_word);
          --execute_count_; // Fixup
        }
        break;
      case InstrType::kSAddU:
        ret = executeSatAddUnsigned(width, src1, src2);
        break;
      case InstrType::kSAddS:
        ret = executeSatAddSigned(width, src1, src2);
        break;
      case InstrType::kSSubU:
        ret = executeSatSubUnsigned(width, src1, src2);
        break;
      case InstrType::kSSubS:
        ret = executeSatSubSigned(width, src1, src2);
        break;
      case InstrType::kMinU:
        ret = executeMinUnsigned(width, src1, src2);
        break;
      case InstrType::kMinS:
        ret = executeMinSigned(width, src1, src2);
        break;
      case InstrType::kMaxU:
        ret = executeMaxUnsigned(width, src1, src2);
        break;
      case InstrType::kMaxS:
        ret = executeMaxSigned(width, src1, src2);
        break;
      case InstrType::kAdd:
        ret = src1 + src2;
        if (!is_odd && InstrWidth::kInstrSz32 == getWidth(instr_word)) {
          // Even 32-bit ALUs will set the carry-in of the odd ALU above it.
          uint64_t x = src1 & 0xFFFFFFFF, y = src2 & 0xFFFFFFFF;
          int carry_out = ((x+y) & 0xFFFFFFFF00000000ULL) ? 1:0;
          setCout(instr_word, carry_out);
        }
        break;
      case InstrType::kAddC: {
        // Even number ALUs don't implement the carry-in and it is tied to
        // zero for add.
        int cin = 0;
        if (is_odd && InstrWidth::kInstrSz32 == getWidth(instr_word)) {
          cin = getCout(instr_word-1);
        }
        ret = src1 + src2 + cin;
        break; }
      case InstrType::kSub:
        ret = executeSub(instr_word, getWidth(instr_word), src1, src2);
        break;
      case InstrType::kSubR:
        ret = executeSubR(instr_word, width, src1, src2);
        break;
      case InstrType::kSubC:
        ret = executeSubC(instr_word, getWidth(instr_word), src1, src2);
        break;
      case InstrType::kSubRC:
        ret = executeSubRC(instr_word, width, src1, src2);
        break;
      case InstrType::kDepositField:
        ret = executeDepositField(instr, width, src1, src2, log_buf);
        break;
      case InstrType::kOperand1:
        RMT_ASSERT(isOperand1OnlyAlu(instr_word));
        RMT_ASSERT(!merged);
        ret = src1;
          break;
      case InstrType::kOperand2:
        RMT_ASSERT(isOperand2OnlyAlu(instr_word));
        RMT_ASSERT(!merged);
        ret = src2;
        break;
      default:
        break;
    }

    if (error) {
      //LOG_INFO("Instr[%3d] Error_cnt %d to %d", instr_word, error_cnt, error_cnt+1);
      ++error_cnt;
      RMT_LOG_IF_NOT_NULL(log_buf, RmtDebug::error2(
              RmtDebug::kRmtDebugInstr1), 
              "%s result NOP (error)\n", log_buf->GetBuf());
    } else if (invalidate) {
      phv_out->clobber(instr_word, 0);
      phv_out->set_valid(instr_word, false);
      if (merged) {
        phv_out->clobber(instr_word+1, 0);
        phv_out->set_valid(instr_word+1, false);
      }
      RMT_LOG_IF_NOT_NULL(log_buf, RmtDebug::verbose(
              RmtDebug::kRmtDebugInstr1), 
              "%s result InvalidateDest\n", log_buf->GetBuf());
    } else if (!noop) {
      if (merged) {
        if (InstrWidth::kInstrSz32 == width) {
          phv_out->clobber(instr_word, ret & 0xFFFF);
          phv_out->clobber(instr_word+1, (ret >> 16) & 0xFFFF);
          RMT_LOG_IF_NOT_NULL(log_buf, RmtDebug::verbose(
                  RmtDebug::kRmtDebugInstr1), "%s result merged 0x%x 0x%x\n", 
                  log_buf->GetBuf(), ret & 0xFFFF, (ret >> 16) & 0xFFFF);
        } else {
          phv_out->clobber(instr_word, ret & 0xFF);
          phv_out->clobber(instr_word+1, (ret >> 8) & 0xFF);
          RMT_LOG_IF_NOT_NULL(log_buf, RmtDebug::verbose(
                  RmtDebug::kRmtDebugInstr1), 
                  "%s result merged 0x%x 0x%x\n", 
                  log_buf->GetBuf(), ret & 0xFF, (ret >> 8) & 0xFF);
        }
      } else {
        phv_out->clobber(instr_word, ret);
        RMT_LOG_IF_NOT_NULL(log_buf, RmtDebug::verbose(
                RmtDebug::kRmtDebugInstr1), 
                "%s result 0x%x\n",  log_buf->GetBuf(), ret);
      }
    } else {
      if (op != InstrType::kNop) {
        RMT_LOG_IF_NOT_NULL(log_buf, RmtDebug::verbose(
                RmtDebug::kRmtDebugInstr1), 
                "%s result NOP\n", log_buf->GetBuf());
      }
    }
    return error_cnt;
  }

  // Execute ALL instr_words
  int Instr::execute(Phv *src_phv,
                     const BitVector<MauDefs::kActionHVOutputBusWidth> &adib,
                     Phv *phv_out) {
    execute_count_ = 0;
    error_alus_.fill_all_zeros();
    deferred_alus_.fill_all_zeros();

    bool quiet = !rmt_log_check(RmtDebug::verbose(RmtDebug::kRmtDebugInstr1));
    model_core::LogBuffer log_buf(256);
    model_core::LogBuffer* buf_ptr = (quiet) ?nullptr :&log_buf;

    int error_cnt = 0;

    // It is important to execute from zero up to max to ensure the carry out/
    // carry in is set correctly for the 32 bit AddC/SubC operations.
    //
    int instr_word;
    for (instr_word = 0; instr_word < kInstrsMax; ++instr_word) {
      log_buf.Reset();
      int ret = execute_one(instr_word, src_phv, adib, phv_out, buf_ptr);
      error_cnt += ret;
      if (ret > 0) error_alus_.set_bit(true, instr_word);
      if (isMerged(instr_word)) ++instr_word;
    }

    // Now special code to handle even EQ64/NEQ64 ALUs where the odd ALU needed
    // to run first - the even EQ64/NEQ64 ALUs will have been deferred till now
    //
    instr_word = deferred_alus_.get_first_bit_set();
    while (instr_word >= 0) {
      log_buf.Reset();
      int ret = execute_one(instr_word, src_phv, adib, phv_out, buf_ptr);
      error_cnt += ret;
      if (ret > 0) error_alus_.set_bit(true, instr_word);
      instr_word = deferred_alus_.get_first_bit_set(instr_word);
    }
    

    /**
    *  error_alus is a bitfield of which the corresponding bits get set 
    *  depending on the return value of execute_one(..., nullptr).
    *  We then loop through the bitfield and every bit that has been set
    *  corresponds to an alu that threw an error. This means that we need
    *  to run execute_one again on that alu, with a LogBuffer passed in
    *  instead of a nullptr. We do not need to check for the flags again
    *  as we know they are both false.
    */
    if (!quiet) {
      int temp = execute_count_;
      int instr_error = error_alus_.get_first_bit_set();
      while (instr_error >= 0) {
        log_buf.Reset();
        execute_one(instr_error, src_phv, adib, phv_out, &log_buf);
        instr_error = error_alus_.get_first_bit_set(instr_error);
      }
      execute_count_ = temp;
    }

    return error_cnt;
  }



  int Instr::executeLoadConst(const uint32_t instr, InstrWidth width) {
    int constant = 0;
    if (InstrWidth::kInstrSz32 == width) {
      uint32_t lo_mask = 0xFFFFFFFFu >> (32-kOpCodeShift); // ALL bits below opcode
      uint32_t op_lo = kLoadConstOpCodeExtraShift;         // Start rel to opcode
      uint32_t op_hi = op_lo + (kConstSize32 - kOpCodeShift);
      RMT_ASSERT(kConstSize32 > kOpCodeShift);
      constant = (OP_GET(op_hi,op_lo,instr) << kOpCodeShift) | (instr & lo_mask);
    } else if (InstrWidth::kInstrSz16 == width) {
      uint32_t lo_mask = 0xFFFFFFFFu >> (32-kOpCodeShift); // All bits below opcode
      uint32_t op_lo = kLoadConstOpCodeExtraShift;         // Start rel to opcode
      uint32_t op_hi = op_lo + (kConstSize16 - kOpCodeShift);
      RMT_ASSERT(kConstSize16 > kOpCodeShift);
      constant = (OP_GET(op_hi,op_lo,instr) << kOpCodeShift) | (instr & lo_mask);
    } else if (InstrWidth::kInstrSz8 == width) {
      uint32_t lo_mask = 0xFFFFFFFFu >> (32-kConstSize8);  // Just 8b typically
      RMT_ASSERT(kConstSize8 <= kOpCodeShift);
      constant = instr & lo_mask;
    } else {
      RMT_ASSERT(0);
    }
    return constant;
  }

  int Instr::executeShiftFunnel(const uint32_t instr, InstrWidth width,
                                uint32_t s1, uint32_t s2, model_core::LogBuffer* log_buf) {
    // Get the shift count.
    int shift = 0;
    if (InstrWidth::kInstrSz32 == width) {
      shift = OP_GET(10,6,instr);
    } else if (InstrWidth::kInstrSz16 == width) {
      shift = OP_GET(9,6,instr);
    } else if (InstrWidth::kInstrSz8 == width) {
      shift = OP_GET(8,6,instr);
    }

    // Combine the operands and shift.
    if (InstrWidth::kInstrSz32 == width) {
      uint64_t x = s1, y = s2;
      uint64_t z = x << 32 | y;
      uint64_t m = 0xFFFFFFFF;
      LOG_APPEND_CHECK(log_buf, " 0x%016" PRIx64 " >> %d", z, shift);
      return z >> shift & m;
    } else if (InstrWidth::kInstrSz16 == width) {
      uint32_t x = s1, y = s2;
      uint32_t z = x << 16 | y;
      uint32_t m = 0xFFFF;
      LOG_APPEND_CHECK(log_buf, " 0x%08x >> %d", z, shift);
      return z >> shift & m;
    } else if (InstrWidth::kInstrSz8 == width) {
      uint16_t x = s1, y = s2;
      uint16_t z = x << 8 | y;
      uint16_t m = 0xFF;
      LOG_APPEND_CHECK(log_buf, " 0x%04x >> %d", z, shift);
      return z >> shift & m;
    }
    RMT_ASSERT(0);
  }

  int Instr::executeShiftLeft(const uint32_t instr, InstrWidth width,
                              uint32_t s, model_core::LogBuffer* log_buf) {
    int shift = 0;
    if (InstrWidth::kInstrSz32 == width) {
      shift = OP_GET(10,6,instr);
      LOG_APPEND_CHECK(log_buf, " 0x%08x << %d", s, shift);
      return (s << shift) & 0xFFFFFFFF;
    } else if (InstrWidth::kInstrSz16 == width) {
      shift = OP_GET(9,6,instr);
      LOG_APPEND_CHECK(log_buf, " 0x%04x << %d", s, shift);
      return (s << shift) & 0x0000FFFF;
    } else if (InstrWidth::kInstrSz8 == width) {
      shift = OP_GET(8,6,instr);
      LOG_APPEND_CHECK(log_buf, " 0x%02x << %d", s, shift);
      return (s << shift) & 0x000000FF;
    }
    return 0;
  }
  unsigned Instr::executeShiftRightUnsigned(const uint32_t instr,
                                            InstrWidth width, uint32_t s,
                                            model_core::LogBuffer* log_buf) {
    int shift = 0;
    if (InstrWidth::kInstrSz32 == width) {
      shift = OP_GET(10,6,instr);
      LOG_APPEND_CHECK(log_buf, " 0x%08x >> %d", s, shift);
      return (s >> shift) & 0xFFFFFFFF;
    } else if (InstrWidth::kInstrSz16 == width) {
      shift = OP_GET(9,6,instr);
      LOG_APPEND_CHECK(log_buf, " 0x%04x >> %d", s, shift);
      return (s >> shift) & 0x0000FFFF;
    } else if (InstrWidth::kInstrSz8 == width) {
      shift = OP_GET(8,6,instr);
      LOG_APPEND_CHECK(log_buf, " 0x%02x >> %d", s, shift);
      return (s >> shift) & 0x000000FF;
    }
    return 0;
  }
  int Instr::executeShiftRightSigned(const uint32_t instr,
                                     InstrWidth width, int32_t s,
                                     model_core::LogBuffer* log_buf) {
    int shift = 0;
    if (InstrWidth::kInstrSz32 == width) {
      shift = OP_GET(10,6,instr);
      LOG_APPEND_CHECK(log_buf, " 0x%08x >> %d", s, shift);
      return (s >> shift) & 0xFFFFFFFF;
    } else if (InstrWidth::kInstrSz16 == width) {
      shift = OP_GET(9,6,instr);
      LOG_APPEND_CHECK(log_buf, " 0x%04x >> %d", s, shift);
      return ((int16_t)s >> shift) & 0xFFFF; // Type cast to sign extend while shifting
    } else if (InstrWidth::kInstrSz8 == width) {
      shift = OP_GET(8,6,instr);
      LOG_APPEND_CHECK(log_buf, " 0x%02x >> %d", s, shift);
      return ((int8_t)s >> shift) & 0xFF; // Type cast to sign extend while shifting
    }
    return 0;
  }

  int Instr::executeShiftLeftDataDep(const uint32_t instr, InstrWidth width,
                                     uint32_t src1, uint32_t src2, model_core::LogBuffer* log_buf) {
    // Call old SHL that gets shift from instr if data-dep shift not supported
    if (!kInstrDataDepShiftSupported) return executeShiftLeft(instr, width, src2, log_buf);
    // Otherwise we get shift from src1
    int shift = 0;
    if (InstrWidth::kInstrSz32 == width) {
      shift = src1 & 0x1F;
      LOG_APPEND_CHECK(log_buf, " 0x%08x << %d", src2, shift);
      return (src2 << shift) & 0xFFFFFFFF;
    } else if (InstrWidth::kInstrSz16 == width) {
      shift = src1 & 0xF;
      LOG_APPEND_CHECK(log_buf, " 0x%04x << %d", src2, shift);
      return (src2 << shift) & 0x0000FFFF;
    } else if (InstrWidth::kInstrSz8 == width) {
      shift = src1 & 0x7;
      LOG_APPEND_CHECK(log_buf, " 0x%02x << %d", src2, shift);
      return (src2 << shift) & 0x000000FF;
    }
    return 0;
  }
  unsigned Instr::executeShiftRightUnsignedDataDep(const uint32_t instr, InstrWidth width,
                                                   uint32_t src1, uint32_t src2, model_core::LogBuffer* log_buf) {
    // Call old SHRU that gets shift from instr if data-dep shift not supported
    if (!kInstrDataDepShiftSupported) return executeShiftRightUnsigned(instr, width, src2, log_buf);
    // Otherwise we get shift from src1
    int shift = 0;
    if (InstrWidth::kInstrSz32 == width) {
      shift = src1 & 0x1F;
      LOG_APPEND_CHECK(log_buf, " 0x%08x >> %d", src2, shift);
      return (src2 >> shift) & 0xFFFFFFFF;
    } else if (InstrWidth::kInstrSz16 == width) {
      shift = src1 & 0xF;
      LOG_APPEND_CHECK(log_buf, " 0x%04x >> %d", src2, shift);
      return (src2 >> shift) & 0x0000FFFF;
    } else if (InstrWidth::kInstrSz8 == width) {
      shift = src1 & 0x7;
      LOG_APPEND_CHECK(log_buf, " 0x%02x >> %d", src2, shift);
      return (src2 >> shift) & 0x000000FF;
    }
    return 0;
  }
  int Instr::executeShiftRightSignedDataDep(const uint32_t instr, InstrWidth width,
                                            uint32_t src1, int32_t src2, model_core::LogBuffer* log_buf) {
    // Call old SHRS that gets shift from instr if data-dep shift not supported
    if (!kInstrDataDepShiftSupported) return executeShiftRightSigned(instr, width, src2, log_buf);
    // Otherwise we get shift from src1
    int shift = 0;
    if (InstrWidth::kInstrSz32 == width) {
      shift = src1 & 0x1F;
      LOG_APPEND_CHECK(log_buf, " 0x%08x >> %d", src2, shift);
      return (src2 >> shift) & 0xFFFFFFFF;
    } else if (InstrWidth::kInstrSz16 == width) {
      shift = src1 & 0xF;
      LOG_APPEND_CHECK(log_buf, " 0x%04x >> %d", src2, shift);
      return ((int16_t)src2 >> shift) & 0xFFFF; // Type cast to sign extend while shifting
    } else if (InstrWidth::kInstrSz8 == width) {
      shift = src1 & 0x7;
      LOG_APPEND_CHECK(log_buf, " 0x%02x >> %d", src2, shift);
      return ((int8_t)src2 >> shift) & 0xFF; // Type cast to sign extend while shifting
    }
    return 0;
  }

  int Instr::executeByteRotMerge(const uint32_t instr, InstrWidth width,
                                 uint32_t s1, uint32_t s2, model_core::LogBuffer* log_buf) {
    uint8_t s1_cnt = 0, s2_cnt = 0, mask = 0;
    if (InstrWidth::kInstrSz32 == width) {
      s1_cnt = OP_GET(12,11,instr);
      s2_cnt = OP_GET(10,9,instr);
      mask   = OP_GET(7,4,instr);
      for (unsigned i=s1_cnt; i; --i) { s1 = (s1>>8) | (s1<<24); }
      for (unsigned i=s2_cnt; i; --i) { s2 = (s2>>8) | (s2<<24); }
      uint32_t x = ((mask & 0x8) ? (s1 & 0xFF000000) : (s2 & 0xFF000000)) |
                   ((mask & 0x4) ? (s1 & 0x00FF0000) : (s2 & 0x00FF0000)) |
                   ((mask & 0x2) ? (s1 & 0x0000FF00) : (s2 & 0x0000FF00)) |
                   ((mask & 0x1) ? (s1 & 0x000000FF) : (s2 & 0x000000FF));
      LOG_APPEND_CHECK(log_buf, " s1Rot=%d s2Rot=%d Msk=0x%x", s1_cnt, s2_cnt, mask);
      return x;
    } else {
      s1_cnt = OP_GET(11,11,instr);
      s2_cnt = OP_GET(9,9,instr);
      mask   = OP_GET(5,4,instr);
      s1 &= 0xFFFF;
      s2 &= 0xFFFF;
      for (int i=s1_cnt; i>0; --i) { s1 = (s1>>8) | (s1<<8); }
      for (int i=s2_cnt; i>0; --i) { s2 = (s2>>8) | (s2<<8); }
      uint16_t x = ((mask & 0x2) ? (s1 & 0xFF00) : (s2 & 0xFF00)) |
                   ((mask & 0x1) ? (s1 & 0x00FF) : (s2 & 0x00FF));
      LOG_APPEND_CHECK(log_buf, " s1Rot=%d s2Rot=%d Msk=0x%x", s1_cnt, s2_cnt, mask);
      return x;
    }
  }

  int Instr::executeBitMaskedSet(const uint32_t instr, InstrWidth width,
                                 uint32_t mask, uint32_t masked_src,
                                 uint32_t background, model_core::LogBuffer* log_buf) {
    if (InstrWidth::kInstrSz32 == width) {
      LOG_APPEND_CHECK(log_buf, " src 0x%x msk 0x%x", masked_src, mask);
      return (masked_src | (~mask & background)) & 0xFFFFFFFF;
    } else if (InstrWidth::kInstrSz16 == width) {
      mask &= 0xFFFF;
      masked_src &= 0xFFFF;
      background &= 0xFFFF;
      LOG_APPEND_CHECK(log_buf, " src 0x%x msk 0x%x", masked_src, mask);
      return (masked_src | (~mask & background)) & 0xFFFF;
    } else if (InstrWidth::kInstrSz8 == width) {
      mask &= 0xFF;
      masked_src &= 0xFF;
      background &= 0xFF;
      LOG_APPEND_CHECK(log_buf, " src 0x%x msk 0x%x", masked_src, mask);
      return (masked_src | (~mask & background)) & 0xFF;
    } else {
      RMT_ASSERT(0);
    }
    return 0;
  }

  int Instr::executeCondMove(const uint32_t instr,
                             uint32_t s1, bool s1_v, bool s2_v,
                             bool dst_v, bool &error, bool &noop,
                             bool &invalidate, model_core::LogBuffer* log_buf) {
    bool tst_s1 = OP_GET(9,9,instr);
    bool s2_pol = OP_GET(8,8,instr);
    bool tst_s2 = OP_GET(7,7,instr);
    bool dst_pol = OP_GET(6,6,instr);
    bool tst_dst = OP_GET(5,5,instr);

    // IF dest valid (optional check, reversible polarity
    // AND src2 valid (optional check, reversible polarity)
    // AND src1 valid (optional check)
    // THEN dest=src1
    // ELSE noop
    bool dst_cond = (((dst_pol == dst_v) && tst_dst) || !tst_dst);
    bool s2_cond  = (((s2_v == s2_pol) && tst_s2) || !tst_s2);
    bool s1_cond  = ((s1_v && tst_s1) || !tst_s1);
    if (dst_cond &&
        s2_cond &&
        s1_cond) {
      noop = false;
      // Note, invalid source operands no longer result in a NOP.
      error = false;//(tst_s1 && !s1_v) || (tst_s2 && (s2_pol != s2_v));
      invalidate = !error && !s1_v;
      LOG_APPEND_CHECK(log_buf, " Test1/2/D %d/%d/%d Pol2/D %d/%d",
              tst_s1, tst_s2, tst_dst, s2_pol, dst_pol);
      return s1;
    } else {
      noop = true;
      error = false;
      invalidate = false;
      LOG_APPEND_CHECK(log_buf, " Test1/2/D %d/%d/%d Pol2/D %d/%d",
              tst_s1, tst_s2, tst_dst, s2_pol, dst_pol);
      return 0;
    }
  }

  int Instr::executeCondMux(const uint32_t instr,
                             uint32_t s1, uint32_t s2, bool s1_v, bool s2_v,
                             bool dst_v, bool &error, bool &noop, model_core::LogBuffer* log_buf) {
    bool tst_s1 = true;//OP_GET(11,11,instr);
    bool dst_pol = OP_GET(6,6,instr);
    bool tst_dst = OP_GET(5,5,instr);

    // IF dest valid (optional check, reversible polarity
    //   IF src2 valid
    //     THEN dst = src2
    //   ELSE IF src1 valid (optional check)
    //     THEN dst = src1
    //   ELSE
    //     noop
    // ELSE noop
    if (((dst_pol == dst_v) && tst_dst) || !tst_dst) {
      if (s2_v) {
        noop = false;
        error = false;
        LOG_APPEND_CHECK(log_buf, " Test1/D %d/%d PolD %d",
                tst_s1, tst_dst, dst_pol);
        return s2;
      } else if ((s1_v && tst_s1) || !tst_s1) {
        // Note, invalid source operands no longer result in a NOP.
        noop = false;//!s1_v;
        error = false;//!s1_v;
        LOG_APPEND_CHECK(log_buf, " Test1/D %d/%d PolD %d",
                tst_s1, tst_dst, dst_pol);
        return s1_v ? s1 : 0;
      }
    }
    LOG_APPEND_CHECK(log_buf, " Test1/D %d/%d PolD %d",
            tst_s1, tst_dst, dst_pol);
    noop = true;
    error = false;
    return 0;
  }

  int Instr::executeArithmeticCompareUnsigned(InstrType op, InstrWidth width,
                                              uint32_t src1, uint32_t src2) {
    RMT_ASSERT (RmtObject::is_jbay_or_later());
    if (InstrWidth::kInstrSz32 == width) {
      switch (op) {
        case InstrType::kGtEqU: return ((uint32_t)src1 >= (uint32_t)src2) ?1 :0;
        case InstrType::kLtU:   return ((uint32_t)src1 <  (uint32_t)src2) ?1 :0;
        case InstrType::kLtEqU: return ((uint32_t)src1 <= (uint32_t)src2) ?1 :0;
        case InstrType::kGtU:   return ((uint32_t)src1 >  (uint32_t)src2) ?1 :0;
        default: RMT_ASSERT(0);
      }
    } else if (InstrWidth::kInstrSz16 == width) {
      switch (op) {
        case InstrType::kGtEqU: return ((uint16_t)src1 >= (uint16_t)src2) ?1 :0;
        case InstrType::kLtU:   return ((uint16_t)src1 <  (uint16_t)src2) ?1 :0;
        case InstrType::kLtEqU: return ((uint16_t)src1 <= (uint16_t)src2) ?1 :0;
        case InstrType::kGtU:   return ((uint16_t)src1 >  (uint16_t)src2) ?1 :0;
        default: RMT_ASSERT(0);
      }
    } else {
      switch (op) {
        case InstrType::kGtEqU: return ((uint8_t)src1 >= (uint8_t)src2) ?1 :0;
        case InstrType::kLtU:   return ((uint8_t)src1 <  (uint8_t)src2) ?1 :0;
        case InstrType::kLtEqU: return ((uint8_t)src1 <= (uint8_t)src2) ?1 :0;
        case InstrType::kGtU:   return ((uint8_t)src1 >  (uint8_t)src2) ?1 :0;
        default: RMT_ASSERT(0);
      }
    }
  }

  int Instr::executeArithmeticCompareSigned(InstrType op, InstrWidth width,
                                            int32_t src1, int32_t src2) {
    RMT_ASSERT (RmtObject::is_jbay_or_later());
    if (InstrWidth::kInstrSz32 == width) {
      switch (op) {
        case InstrType::kGtEqS: return ((int32_t)src1 >= (int32_t)src2) ?1 :0;
        case InstrType::kLtS:   return ((int32_t)src1 <  (int32_t)src2) ?1 :0;
        case InstrType::kLtEqS: return ((int32_t)src1 <= (int32_t)src2) ?1 :0;
        case InstrType::kGtS:   return ((int32_t)src1 >  (int32_t)src2) ?1 :0;
        default: RMT_ASSERT(0);
      }
    } else if (InstrWidth::kInstrSz16 == width) {
      switch (op) {
        case InstrType::kGtEqS: return ((int16_t)src1 >= (int16_t)src2) ?1 :0;
        case InstrType::kLtS:   return ((int16_t)src1 <  (int16_t)src2) ?1 :0;
        case InstrType::kLtEqS: return ((int16_t)src1 <= (int16_t)src2) ?1 :0;
        case InstrType::kGtS:   return ((int16_t)src1 >  (int16_t)src2) ?1 :0;
        default: RMT_ASSERT(0);
      }
    } else {
      switch (op) {
        case InstrType::kGtEqS: return ((int8_t)src1 >= (int8_t)src2) ?1 :0;
        case InstrType::kLtS:   return ((int8_t)src1 <  (int8_t)src2) ?1 :0;
        case InstrType::kLtEqS: return ((int8_t)src1 <= (int8_t)src2) ?1 :0;
        case InstrType::kGtS:   return ((int8_t)src1 >  (int8_t)src2) ?1 :0;
        default: RMT_ASSERT(0);
      }
    }
  }

  int Instr::executeArithmeticCompareEquality(int instr_word,
                                              InstrType op, InstrWidth width,
                                              uint32_t src1, uint32_t src2) {
    RMT_ASSERT (RmtObject::is_jbay_or_later());
    RMT_ASSERT ((op == InstrType::kEq) || (op == InstrType::kNEq));
    bool equal = false;
    if (InstrWidth::kInstrSz32 == width) {
      equal = ( (uint32_t)src1 == (uint32_t)src2 ); // 32b ALU
    } else if (InstrWidth::kInstrSz16 == width) {
      equal = ( (uint16_t)src1 == (uint16_t)src2 ); // 16b ALU
    } else if (InstrWidth::kInstrSz8 == width) {
      equal = (  (uint8_t)src1 ==  (uint8_t)src2 ); //  8b ALU
    } else {
      RMT_ASSERT(0);
    }
    switch (op) {
      case InstrType::kEq:  return  (equal) ?1 :0;
      case InstrType::kNEq: return (!equal) ?1 :0;
      default: RMT_ASSERT(0);
    }
  }

  int Instr::executeArithmeticCompareEquality64(int instr_word,
                                                InstrType op, InstrWidth width,
                                                uint32_t src1, uint32_t src2) {
    RMT_ASSERT (RmtObject::is_jbay_or_later());
    RMT_ASSERT ((op == InstrType::kEq64) || (op == InstrType::kNEq64));
    bool is_odd_alu = ((instr_word & 1) == 1);
    bool equal = false, eq2 = false, neq2 = false;
    if (InstrWidth::kInstrSz32 == width) {
      equal = ((uint32_t)src1 == (uint32_t)src2);
      if (is_odd_alu) {
        // ODD 32b ALU and kEq64/kNEq64 - drive eq/neq signals to EVEN ALU
        if      (op == InstrType::kEq64)  setEQout(instr_word, equal);
        else if (op == InstrType::kNEq64) setNEQout(instr_word, !equal);
        return 0; // But result always 0

      } else {
        // EVEN 32b ALU and kEq64/kNEq64
        // Further checks 1: check instr_word+1 is also a 32b ALU
        // (given tot num 32b ALUs is EVEN(64) this should NEVER happen)
        if (InstrWidth::kInstrSz32 != getWidth(instr_word+1, false)) {
          RMT_LOG(RmtDebug::error(kRelaxEqNeqConfigCheck),
                  "OP %s used on 32b ALU[%d] but next ALU[%d] "
                  "is NOT 32b\n", instr_op_name(op), instr_word, instr_word+1);
          if (!kRelaxEqNeqConfigCheck) { THROW_ERROR(-2); }
          return 0; // Result 0 in this (probably impossible) error case
        }
        // Further checks 2: check instr_word+1 is kEq64 or kNEq64 as appropriate
        InstrType expect_other_op = op, actual_other_op = getOpCode(instr_word+1);
        if (actual_other_op != expect_other_op) {
          RMT_LOG(RmtDebug::error(kRelaxEqNeqConfigCheck),
                  "OP %s used on 32b ALU[%d] but next ALU[%d] "
                  "is OP %s (*should* be OP %s)\n",
                  instr_op_name(op), instr_word, instr_word+1,
                  instr_op_name(actual_other_op),
                  instr_op_name(expect_other_op));
          if (!kRelaxEqNeqConfigCheck) { THROW_ERROR(-2); }
          return 0; // Result 0 in this error case
        }
        // EVEN 32b ALU and kEq64/kNEq64 - get eq/neq signals from ODD ALU above
        if      (op == InstrType::kEq64)  eq2  = getEQout(instr_word+1);
        else if (op == InstrType::kNEq64) neq2 = getNEQout(instr_word+1);
        // And return result using own eq/neq AND eq/neq from ODD ALU above
        if      (op == InstrType::kEq64)  return  (equal && eq2)  ?1 :0;
        else if (op == InstrType::kNEq64) return (!equal || neq2) ?1 :0;
        else return 0; // should not be reached
      }

    } else if (InstrWidth::kInstrSz16 == width) {
      // 16b ALU - so output 0
      RMT_LOG(RmtDebug::error(kRelaxEqNeqConfigCheck),
              "OP %s used on 16b ALU[%3d]\n", instr_op_name(op), instr_word);
      if (!kRelaxEqNeqConfigCheck) { THROW_ERROR(-2); }
      return 0;

    } else {
      // 8b ALU - so output 0
      RMT_ASSERT (InstrWidth::kInstrSz8 == width);
      RMT_LOG(RmtDebug::error(kRelaxEqNeqConfigCheck),
              "OP %s used on 8b ALU[%3d]\n", instr_op_name(op), instr_word);
      if (!kRelaxEqNeqConfigCheck) { THROW_ERROR(-2); }
      return 0;
    }
  }

  int Instr::executeSatAddUnsigned(InstrWidth width, uint32_t src1, uint32_t src2) {
    if (InstrWidth::kInstrSz32 == width) {
      uint64_t x = src1, y = src2;
      x += y;
      if (0xFFFFFFFFULL < x) {
        return 0xFFFFFFFF;
      } else {
        return x;
      }
    } else if (InstrWidth::kInstrSz16 == width) {
      uint32_t x = src1 & 0xFFFF, y = src2 & 0xFFFF;
      x += y;
      if (0xFFFFU < x) {
        return 0xFFFF;
      } else {
        return x;
      }
    } else {
      uint16_t x = src1 & 0xFF, y = src2 & 0xFF;
      x += y;
      if (0xFFU < x) {
        return 0xFF;
      } else {
        return x;
      }
    }
  }

  int Instr::executeSatAddSigned(InstrWidth width, uint32_t src1, uint32_t src2) {
    if (InstrWidth::kInstrSz32 == width) {
      int64_t x = (int32_t)src1, y = (int32_t)src2;
      x += y;
      if (x >  2147483647) x =  2147483647;
      if (x < -2147483648) x = -2147483648;
      return x & 0xFFFFFFFF;
    } else if (InstrWidth::kInstrSz16 == width) {
      int32_t x = (int16_t)src1, y = (int16_t)src2;
      x += y;
      if (x >  32767) x =  32767;
      if (x < -32768) x = -32768;
      return x & 0xFFFF;
    } else {
      int16_t x = (int8_t)src1, y = (int8_t)src2;
      x += y;
      if (x >  127) x =  127;
      if (x < -128) x = -128;
      return x & 0xFF;
    }
  }

  int Instr::executeSatSubUnsigned(InstrWidth width, uint32_t src1, uint32_t src2) {
    if (InstrWidth::kInstrSz32 == width) {
      uint32_t x = src1, y = src2;
      return (y >= x) ? 0 : x-y;
    } else if (InstrWidth::kInstrSz16 == width) {
      uint16_t x = src1, y = src2;
      return (y >= x) ? 0 : x-y;
    } else {
      uint8_t x = src1, y = src2;
      return (y >= x) ? 0 : x-y;
    }
  }

  int Instr::executeSatSubSigned(InstrWidth width, uint32_t src1, uint32_t src2) {
    if (InstrWidth::kInstrSz32 == width) {
      int64_t x = (int32_t)src1, y = (int32_t)src2;
      x -= y;
      if (x >  2147483647) x =  2147483647;
      if (x < -2147483648) x = -2147483648;
      return x & 0xFFFFFFFF;
    } else if (InstrWidth::kInstrSz16 == width) {
      int32_t x = (int16_t)src1, y = (int16_t)src2;
      x -= y;
      if (x >  32767) x =  32767;
      if (x < -32768) x = -32768;
      return x & 0xFFFF;
    } else {
      int16_t x = (int8_t)src1, y = (int8_t)src2;
      x -= y;
      if (x >  127) x =  127;
      if (x < -128) x = -128;
      return x & 0xFF;
    }
  }

  int Instr::executeMinUnsigned(InstrWidth width, uint32_t src1, uint32_t src2) {
    if (InstrWidth::kInstrSz32 == width) {
      return src1 < src2 ? src1:src2;
    } else if (InstrWidth::kInstrSz16 == width) {
      uint16_t x = src1, y = src2;
      return x < y ? x:y;
    } else {
      uint8_t x = src1, y = src2;
      return x < y ? x:y;
    }
  }

  int Instr::executeMinSigned(InstrWidth width, int32_t src1, int32_t src2) {
    if (InstrWidth::kInstrSz32 == width) {
      return src1 < src2 ? src1:src2;
    } else if (InstrWidth::kInstrSz16 == width) {
      return (int16_t)src1 < (int16_t)src2 ? src1:src2;
    } else {
      return (int8_t)src1 < (int8_t)src2 ? src1:src2;
    }
  }

  int Instr::executeMaxUnsigned(InstrWidth width, uint32_t src1, uint32_t src2) {
    if (InstrWidth::kInstrSz32 == width) {
      return src1 > src2 ? src1:src2;
    } else if (InstrWidth::kInstrSz16 == width) {
      uint16_t x = src1, y = src2;
      return x > y ? x:y;
    } else {
      uint8_t x = src1, y = src2;
      return x > y ? x:y;
    }
  }

  int Instr::executeMaxSigned(InstrWidth width, int32_t src1, int32_t src2) {
    if (InstrWidth::kInstrSz32 == width) {
      return src1 > src2 ? src1:src2;
    } else if (InstrWidth::kInstrSz16 == width) {
      return (int16_t)src1 > (int16_t)src2 ? src1:src2;
    } else {
      return (int8_t)src1 > (int8_t)src2 ? src1:src2;
    }
  }

  int Instr::executeSub(int instr_word, InstrWidth width, int32_t src1, int32_t src2) {
    bool is_odd = instr_word & 1;
    int ret = src1 - src2;
    if (!is_odd && InstrWidth::kInstrSz32 == width) {
      // Even 32-bit ALUs will set the carry-in of the odd ALU above it.
      uint64_t x = src1 & 0xFFFFFFFF, y = src2 & 0xFFFFFFFF;
      int carry_out = ((x-y) & 0xFFFFFFFF00000000ULL) ? 0:1;
      setCout(instr_word, carry_out);
    }
    return ret;
  }
  int Instr::executeSubR(int instr_word, InstrWidth width, int32_t src1, int32_t src2) {
    if (kInstrReverseSubtractSupported)
      return executeSub(instr_word, width, src2, src1); // Swap src1/src2
    else
      return executeSub(instr_word, width, src1, src2);
  }
  int Instr::executeSubC(int instr_word, InstrWidth width, int32_t src1, int32_t src2) {
    bool is_odd = instr_word & 1;
    // Even number ALUs don't implement SubC, they perform a normal Sub
    // so set cin to a default value which will offset the -1.
    int cin = 1;
    if (is_odd && InstrWidth::kInstrSz32 == width) {
      cin = getCout(instr_word-1);
    }
    return src1 - src2 - 1 + cin;
  }
  int Instr::executeSubRC(int instr_word, InstrWidth width, int32_t src1, int32_t src2) {
    if (kInstrReverseSubtractSupported)
      return executeSubC(instr_word, width, src2, src1); // Swap src1/src2
    else
      return executeSubC(instr_word, width, src1, src2);
  }

  int Instr::executeDepositField(const uint32_t instr, InstrWidth width,
                                 uint32_t s1, uint32_t s2, model_core::LogBuffer* log_buf) {
    int shift = 0, lsb = 0, msb = 0;
    if (InstrWidth::kInstrSz32 == width) {
      uint32_t src = s1, back = s2;
      shift = OP_GET(10,6,instr);
      lsb   = OP_GET(15,11,instr);
      msb   = OP_GET(5,1,instr);
      if (lsb > msb) {
        LOG_APPEND_CHECK(log_buf, " lsb %d greater than msb %d", lsb, msb);
        return back & 0xFFFFFFFF;
      }
      for (int i=0; i<shift; ++i) { // Rotate source data
        src = (src >> 1) | (src << 31);
      }
      if (31 == msb && 0 == lsb) {
        LOG_APPEND_CHECK(log_buf, " rightRot by %d == 0x%x, replace [%d:%d]",
                shift, src, msb, lsb);
        return src;
      }
      uint32_t x = BH_GET(msb,lsb,src); // Get bit field from source to insert into background
      uint32_t mask = ~BH_SET(msb,lsb,0xFFFFFFFF);
      LOG_APPEND_CHECK(log_buf, " rightRot by %d == 0x%x, replace [%d:%d]",
              shift, src, msb, lsb);
      return (back & mask) | BH_SET(msb,lsb,x);
    } else if (InstrWidth::kInstrSz16 == width) {
      uint16_t src = s1, back = s2;
      shift = OP_GET(9,6,instr);
      lsb   = (OP_GET(12,10,instr) << 1) | OP_GET(5,5,instr);
      msb   = OP_GET(4,1,instr);
      if (lsb > msb) {
        LOG_APPEND_CHECK(log_buf, " lsb %d greater than msb %d", lsb, msb);
        return back & 0xFFFF;
      }
      for (int i=0; i<shift; ++i) {
        src = (src >> 1) | (src << 15);
      }
      if (15 == msb && 0 == lsb) {
        LOG_APPEND_CHECK(log_buf, " rightRot by %d == 0x%x, replace [%d:%d]",
                shift, src, msb, lsb);
        return src;
      }
      uint32_t x = BH_GET(msb,lsb,src);
      uint32_t mask = ~BH_SET(msb,lsb,0x0000FFFF);
      LOG_APPEND_CHECK(log_buf, " rightRot by %d == 0x%x, replace [%d:%d]",
              shift, src, msb, lsb);
      return (back & mask) | BH_SET(msb,lsb,x);
    } else if (InstrWidth::kInstrSz8 == width) {
      uint8_t src = s1, back = s2;
      shift = OP_GET(8,6,instr);
      lsb   = (OP_GET(9,9,instr) << 2) | OP_GET(5,4,instr);
      msb   = OP_GET(3,1,instr);
      if (lsb > msb) {
        LOG_APPEND_CHECK(log_buf, " lsb %d greater than msb %d", lsb, msb);
        return back & 0xFF;
      }
      for (int i=0; i<shift; ++i) {
        src = (src >> 1) | (src << 7);
      }
      if (7 == msb && 0 == lsb) {
        LOG_APPEND_CHECK(log_buf, " rightRot by %d == 0x%x, replace [%d:%d]",
                shift, src, msb, lsb);
        return src;
      }
      uint32_t x = BH_GET(msb,lsb,src);
      uint32_t mask = ~BH_SET(msb,lsb,0x000000FF);
      LOG_APPEND_CHECK(log_buf, " rightRot by %d == 0x%x, replace [%d:%d]",
              shift, src, msb, lsb);
      return (back & mask) | BH_SET(msb,lsb,x);
    } else {
      RMT_ASSERT(0);
    }
  }

}
