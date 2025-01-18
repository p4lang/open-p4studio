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

#include <rmt-log.h>
#include <instr.h>

namespace MODEL_CHIP_NAMESPACE {

  Instr::InstrType Instr::getOpCode(const int instr_word) {
    struct InstrOpCodeId {
      enum InstrType id;
      uint32_t val;
      uint32_t mask;
    } odt[] = {{InstrType::kOperand1   , 0x10000, 0x7000F}, // Pseudo InstrType => use operand1 MUX
               {InstrType::kOperand2   , 0x20000, 0x7000F}, // Pseudo InstrType => use operand2 MUX
               {InstrType::kNop            , 0x000, 0x00F},
               {InstrType::kLoadConst      , 0x008, 0x00F},
               {InstrType::kFunnelShift    , 0x004, 0x01F},
               {InstrType::kShl            , 0x00C, 0x03F},
               {InstrType::kShru           , 0x014, 0x03F},
               {InstrType::kShrs           , 0x01C, 0x03F},
               {InstrType::kShlDataDep     , 0x02C, 0x03F}, // New on JBay
               {InstrType::kShruDataDep    , 0x034, 0x03F}, // New on JBay
               {InstrType::kShrsDataDep    , 0x03C, 0x03F}, // New on JBay
               {InstrType::kPairDpf        , 0x002, 0x00F},
               {InstrType::kByteRotateMerge, 0x00A, 0x00F},
               //{InstrType::kConditionalMux,0x006, 0x01F}, // Combined with kConditionalMove on JBay
               {InstrType::kConditionalMove, 0x006, 0x00F},
               //{InstrType::kInvalidate    ,0x00E, 0x03F}, // Gone on JBay
               {InstrType::kBitMaskedSet   , 0x00E, 0x03F}, // NB. Changed - was 0x2E/0x3F on Tofino
               {InstrType::kAluSetZ        , 0x01E, 0x3FF},
               {InstrType::kAluNor         , 0x05E, 0x3FF},
               {InstrType::kAluAndCA       , 0x09E, 0x3FF},
               {InstrType::kAluNotA        , 0x0DE, 0x3FF},
               {InstrType::kAluAndCB       , 0x11E, 0x3FF},
               {InstrType::kAluNotB        , 0x15E, 0x3FF},
               {InstrType::kAluXor         , 0x19E, 0x3FF},
               {InstrType::kAluNand        , 0x1DE, 0x3FF},
               {InstrType::kAluAnd         , 0x21E, 0x3FF},
               {InstrType::kAluXNor        , 0x25E, 0x3FF},
               {InstrType::kAluB           , 0x29E, 0x3FF},
               {InstrType::kAluOrCA        , 0x2DE, 0x3FF},
               {InstrType::kAluA           , 0x31E, 0x3FF},
               {InstrType::kAluOrCB        , 0x35E, 0x3FF},
               {InstrType::kAluOr          , 0x39E, 0x3FF},
               {InstrType::kAluSetHi       , 0x3DE, 0x3FF},
               {InstrType::kGtEqU          , 0x02E, 0x3FF}, // New on JBay
               {InstrType::kGtEqS          , 0x06E, 0x3FF}, // New on JBay
               {InstrType::kLtU            , 0x0AE, 0x3FF}, // New on JBay
               {InstrType::kLtS            , 0x0EE, 0x3FF}, // New on JBay
               {InstrType::kLtEqU          , 0x12E, 0x3FF}, // New on JBay
               {InstrType::kLtEqS          , 0x16E, 0x3FF}, // New on JBay
               {InstrType::kGtU            , 0x1AE, 0x3FF}, // New on JBay
               {InstrType::kGtS            , 0x1EE, 0x3FF}, // New on JBay
               {InstrType::kEq             , 0x22E, 0x2FF}, // New on JBay
               {InstrType::kNEq            , 0x2AE, 0x2FF}, // New on JBay
               {InstrType::kEq64           , 0x26E, 0x2FF}, // New on JBay
               {InstrType::kNEq64          , 0x2EE, 0x2FF}, // New on JBay
               {InstrType::kSAddU          , 0x03E, 0x3FF},
               {InstrType::kSAddS          , 0x07E, 0x3FF},
               {InstrType::kSSubU          , 0x0BE, 0x3FF},
               {InstrType::kSSubS          , 0x0FE, 0x3FF},
               {InstrType::kMinU           , 0x13E, 0x3FF},
               {InstrType::kMinS           , 0x17E, 0x3FF},
               {InstrType::kMaxU           , 0x1BE, 0x3FF},
               {InstrType::kMaxS           , 0x1FE, 0x3FF},
               {InstrType::kAdd            , 0x23E, 0x3BF},
               {InstrType::kAddC           , 0x2BE, 0x3BF},
               {InstrType::kSub            , 0x33E, 0x3FF},
               {InstrType::kSubR           , 0x37E, 0x3FF}, // New on JBay
               {InstrType::kSubC           , 0x3BE, 0x3FF},
               {InstrType::kSubRC          , 0x3FE, 0x3FF}, // New on JBay
               {InstrType::kDepositField   , 0x001, 0x001},
               {InstrType::kInvalid        , 0x000, 0x000} /* Catch all */
              };
    RMT_ASSERT ((instr_word >= 0) && (instr_word < kInstrsMax));

    int s = (kOpExtendedShift - kOpCodeShift);
    int opcode = (((get(instr_word) >> kOpCodeShift) & kOpCodeMask) << 0) |
                 (((get(instr_word) >> kOpExtendedShift) & kOpExtendedMask) << s);
    int i = 0;
    for (; ; ++i) {
      if ((opcode & odt[i].mask) == (odt[i].val & odt[i].mask)) {
        break;
      }
    }
    if (InstrType::kInvalid == odt[i].id) {
      RMT_LOG(RmtDebug::error(kRelaxInstrFormatCheck),
              "Invalid Instruction Memory OpCode (0x%x) Instruction %d"
              " (0x%x) at %s:%d", opcode, instr_word, get(instr_word),
              __PRETTY_FUNCTION__, __LINE__);
    }
    return odt[i].id;
  }

}
