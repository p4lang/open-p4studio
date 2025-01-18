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

#ifndef _MODEL_CORE_RMT_DEBUG_
#define _MODEL_CORE_RMT_DEBUG_

#include <cinttypes>
#include <map>
#include <string>

// Hard-code DEBUG for the moment
#define RMT_DEBUG

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  enum rmt_log_types_ {
    RMT_LOG_TYPE_MODEL=0,
    RMT_LOG_TYPE_P4,
    RMT_LOG_TYPE_TOFINO,
    RMT_LOG_TYPE_PKT,
    MAX_RMT_LOG_TYPE
  };

#ifdef __cplusplus
}
#endif /* __cplusplus */

namespace model_core {

class RmtDebug {
 private:
  static const std::map<std::string, uint64_t> flags_map_;
  static const std::map<std::string, int> log_types_map_;

 public:
    RmtDebug()  { }
    ~RmtDebug() { }

    static constexpr bool is_rmt_debug_enabled() {
    #ifdef RMT_DEBUG
          return true;
    #else
          return false;
    #endif
        }

    static const uint64_t flags_for_string(std::string key);
    static const std::map<std::string, uint64_t> flags_map(std::string key="*");
    static const int log_type_for_string(std::string key);
    static const std::string log_string_for_type(int log_type);

    // Defs/funcs to turn generic debug flags into debug strings
    static constexpr int kDebugLevels = 8;
    static const char   *kDebugLevelStrings[];
    static inline int get_debug_level_max(uint64_t flags) {
      return __builtin_ffsll(flags) - 1;
    }
    static inline const char *get_debug_level_max_str(uint64_t flags) {
      int lvl = get_debug_level_max(flags);
      return ((lvl >= 0) && (lvl < kDebugLevels)) ?kDebugLevelStrings[lvl] :"";
    }

    // Funcs to splice log-level flags together with arbitrary other flags
    static inline uint64_t fatal(uint64_t f=UINT64_C(0))   { return f|kRmtDebugFatal; }
    static inline uint64_t error(uint64_t f=UINT64_C(0))   { return f|kRmtDebugError; }
    static inline uint64_t error2(uint64_t f=UINT64_C(0))  { return f|kRmtDebugError2; }
    static inline uint64_t warn(uint64_t f=UINT64_C(0))    { return f|kRmtDebugWarn; }
    static inline uint64_t info(uint64_t f=UINT64_C(0))    { return f|kRmtDebugInfo; }
    static inline uint64_t verbose(uint64_t f=UINT64_C(0)) { return f|kRmtDebugVerbose; }
    static inline uint64_t debug(uint64_t f=UINT64_C(0))   { return f|kRmtDebugDebug; }
    static inline uint64_t trace(uint64_t f=UINT64_C(0))   { return f|kRmtDebugTrace; }
    static inline uint64_t error(uint64_t f, bool relax) {
      return (relax) ?error2(f) :error(f);
    }
    static inline uint64_t error(bool relax) {
      return (relax) ?error2() :error();
    }

    static constexpr uint64_t ALL = UINT64_C(0xFFFFFFFFFFFFFFFF);
    static constexpr uint64_t NONE = UINT64_C(0);

    // Generic flags in lower 32 bits - keep these unique
    //
    static constexpr uint64_t kRmtDebugFatal     = UINT64_C(0x0000000000000001);
    static constexpr uint64_t kRmtDebugError     = UINT64_C(0x0000000000000002);
    static constexpr uint64_t kRmtDebugError2    = UINT64_C(0x0000000000000004);
    static constexpr uint64_t kRmtDebugWarn      = UINT64_C(0x0000000000000008);
    static constexpr uint64_t kRmtDebugInfo      = UINT64_C(0x0000000000000010);
    static constexpr uint64_t kRmtDebugVerbose   = UINT64_C(0x0000000000000020);
    static constexpr uint64_t kRmtDebugDebug     = UINT64_C(0x0000000000000040);
    static constexpr uint64_t kRmtDebugTrace     = UINT64_C(0x0000000000000080);
    // Composite flags
    //   Fatal+Error
    static constexpr uint64_t kRmtDebugBadStuff  = UINT64_C(0x0000000000000003);
    //   Basic logging level flags: Fatal ... Trace, and also Force
    static constexpr uint64_t kRmtDebugBasicMask = UINT64_C(0x80000000000000FF);

    static constexpr uint64_t kRmtDebugCreate    = UINT64_C(0x0000000000000100);
    static constexpr uint64_t kRmtDebugDelete    = UINT64_C(0x0000000000000200);
    static constexpr uint64_t kRmtDebugEnter     = UINT64_C(0x0000000000000400);
    static constexpr uint64_t kRmtDebugExit      = UINT64_C(0x0000000000000800);
    static constexpr uint64_t kRmtDebugRead      = UINT64_C(0x0000000000001000);
    static constexpr uint64_t kRmtDebugWrite     = UINT64_C(0x0000000000002000);
    static constexpr uint64_t kRmtDebugGet       = UINT64_C(0x0000000000004000);
    static constexpr uint64_t kRmtDebugSet       = UINT64_C(0x0000000000008000);

    // Force flag - for messages that should always be unconditionally logged
    static constexpr uint64_t kRmtDebugForce     = UINT64_C(0x8000000000000000);

    // Specific flags in top 32 bits - these don't have to be unique
    //
//    static constexpr uint64_t kRmtDebugActionOutputHvXbar1          = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugActionOutputHvXbar2          = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugActionOutputHvXbar4          = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugActionOutputHvXbar8          = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugBitVector1                   = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugBitVector2                   = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugBitVector4                   = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugBitVector8                   = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugCacheId1                     = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugCacheId2                     = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugCacheId4                     = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugCacheId8                     = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugChip1                        = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugChip2                        = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugChip4                        = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugChip8                        = UINT64_C(0x0000000800000000);

    static constexpr uint64_t kRmtDebugCsumEngCalcFastB0B1          = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugCsumEngCalcNoFastB0B1        = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugCsumEngCalcBefore            = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugCsumEngCalcSlow              = UINT64_C(0x0000000800000000);
    static constexpr uint64_t kRmtDebugCsumEngCalcFin               = UINT64_C(0x0000001000000000);
    static constexpr uint64_t kRmtDebugCsumEngErr                   = UINT64_C(0x0000002000000000);
//    static constexpr uint64_t kRmtDebugCsumEng1                     = UINT64_C(0x0000010000000000);
//    static constexpr uint64_t kRmtDebugCsumEng2                     = UINT64_C(0x0000020000000000);
//    static constexpr uint64_t kRmtDebugCsumEng4                     = UINT64_C(0x0000040000000000);
//    static constexpr uint64_t kRmtDebugCsumEng8                     = UINT64_C(0x0000080000000000);

    static constexpr uint64_t kRmtDebugDeparserCsumChange           = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugDeparserCsumSummary          = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugDeparserCsumNonZero          = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugDeparserCsumVerbose          = UINT64_C(0x0000000800000000);
    static constexpr uint64_t kRmtDebugDeparser1                    = UINT64_C(0x0000001000000000);
    static constexpr uint64_t kRmtDebugDeparser2                    = UINT64_C(0x0000002000000000);
    static constexpr uint64_t kRmtDebugDeparser4                    = UINT64_C(0x0000004000000000);
    static constexpr uint64_t kRmtDebugDeparser8                    = UINT64_C(0x0000008000000000);

    static constexpr uint64_t kRmtDebugDeparserBlock1               = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugDeparserBlock2               = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugDeparserBlock4               = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugDeparserBlock8               = UINT64_C(0x0000000800000000);

//    static constexpr uint64_t kRmtDebugHashAddressVhXbar1           = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugHashAddressVhXbar2           = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugHashAddressVhXbar4           = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugHashAddressVhXbar8           = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugHashAddressVhXbarWithReg1    = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugHashAddressVhXbarWithReg2    = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugHashAddressVhXbarWithReg4    = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugHashAddressVhXbarWithReg8    = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugIndirectAccessBlock1         = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugIndirectAccessBlock2         = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugIndirectAccessBlock4         = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugIndirectAccessBlock8         = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugInputControlledXbar1         = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugInputControlledXbar2         = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugInputControlledXbar4         = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugInputControlledXbar8         = UINT64_C(0x0000000800000000);

    static constexpr uint64_t kRmtDebugInstr1                       = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugInstr2                       = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugInstr4                       = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugInstr8                       = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMatchDataInputVhXbar1        = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMatchDataInputVhXbar2        = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMatchDataInputVhXbar4        = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMatchDataInputVhXbar8        = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMatchDataInputVhXbarWithReg1 = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMatchDataInputVhXbarWithReg2 = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMatchDataInputVhXbarWithReg4 = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMatchDataInputVhXbarWithReg8 = UINT64_C(0x0000000800000000);

    static constexpr uint64_t kRmtDebugMauLookup                    = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugMauMatchDistrib              = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugMauFindActions               = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugMauProcessAction             = UINT64_C(0x0000000800000000);
    static constexpr uint64_t kRmtDebugMauDumpActionHVOutputBus     = UINT64_C(0x0000001000000000);
//    static constexpr uint64_t kRmtDebugMau2                         = UINT64_C(0x0000002000000000);
//    static constexpr uint64_t kRmtDebugMau4                         = UINT64_C(0x0000004000000000);
//    static constexpr uint64_t kRmtDebugMau8                         = UINT64_C(0x0000008000000000);
//    static constexpr uint64_t kRmtDebugMau10                        = UINT64_C(0x0000010000000000);
    static constexpr uint64_t kRmtDebugMauInput                     = UINT64_C(0x0000100000000000);
    static constexpr uint64_t kRmtDebugMauAlu                       = UINT64_C(0x0000200000000000);

    static constexpr uint64_t kRmtDebugMauAddrDistOutputToBus       = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugMauAddrDistSweep             = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugMauAddrDistVpn               = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugMauAddrDistClaim             = UINT64_C(0x0000000800000000);
    static constexpr uint64_t kRmtDebugMauAddrDistAction            = UINT64_C(0x0000001000000000);
    static constexpr uint64_t kRmtDebugMauAddrDistStats             = UINT64_C(0x0000002000000000);
    static constexpr uint64_t kRmtDebugMauAddrDistMeter             = UINT64_C(0x0000004000000000);
    static constexpr uint64_t kRmtDebugMauAddrDistIdle              = UINT64_C(0x0000008000000000);
    static constexpr uint64_t kRmtDebugMauAddrDistHdr               = UINT64_C(0x0000010000000000);
    static constexpr uint64_t kRmtDebugMauAddrDistEop               = UINT64_C(0x0000020000000000);
    static constexpr uint64_t kRmtDebugMauAddrDistColorWrite        = UINT64_C(0x0000040000000000);

//    static constexpr uint64_t kRmtDebugMauDependencies1             = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauDependencies2             = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauDependencies4             = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauDependencies8             = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMauGatewayPayload1           = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauGatewayPayload2           = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauGatewayPayload4           = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauGatewayPayload8           = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMauGatewayPayloadReg1        = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauGatewayPayloadReg2        = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauGatewayPayloadReg4        = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauGatewayPayloadReg8        = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMauGatewayTable1             = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauGatewayTable2             = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauGatewayTable4             = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauGatewayTable8             = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMauGatewayTableReg1          = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauGatewayTableReg2          = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauGatewayTableReg4          = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauGatewayTableReg8          = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMauHashGenerator1            = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauHashGenerator2            = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauHashGenerator4            = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauHashGenerator8            = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMauHashGeneratorWithReg1     = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauHashGeneratorWithReg2     = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauHashGeneratorWithReg4     = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauHashGeneratorWithReg8     = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMauInput1                    = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauInput2                    = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauInput4                    = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauInput8                    = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMauInputXbar1                = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauInputXbar2                = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauInputXbar4                = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauInputXbar8                = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMauInputXbarWithReg1         = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauInputXbarWithReg2         = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauInputXbarWithReg4         = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauInputXbarWithReg8         = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMauInstrStore1               = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauInstrStore2               = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauInstrStore4               = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauInstrStore8               = UINT64_C(0x0000000800000000);

    static constexpr uint64_t kRmtDebugMauLogicalRowFindAction      = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugMauLogicalRowActionOutput    = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugMauLogicalRowHacktionOffset  = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugMauLogicalRowData            = UINT64_C(0x0000001000000000);
    static constexpr uint64_t kRmtDebugMauLogicalRowAddr            = UINT64_C(0x0000002000000000);
//    static constexpr uint64_t kRmtDebugMauLogicalRow4               = UINT64_C(0x0000004000000000);
//    static constexpr uint64_t kRmtDebugMauLogicalRow8               = UINT64_C(0x0000008000000000);
//
//    static constexpr uint64_t kRmtDebugMauLogicalRowReg1            = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauLogicalRowReg2            = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauLogicalRowReg4            = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauLogicalRowReg8            = UINT64_C(0x0000000800000000);

    static constexpr uint64_t kRmtDebugMauLogicalTableLookup        = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugMauLogicalTableFindActions   = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugMauLogicalTableGetAddrs      = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugMauLogicalTableSwizzle       = UINT64_C(0x000000080000000);
    static constexpr uint64_t kRmtDebugMauLogicalTableSetBus        = UINT64_C(0x0000001000000000);
    static constexpr uint64_t kRmtDebugMauLogicalTableGetBus        = UINT64_C(0x0000002000000000);
//    static constexpr uint64_t kRmtDebugMauLogicalTable4             = UINT64_C(0x0000004000000000);
//    static constexpr uint64_t kRmtDebugMauLogicalTable8             = UINT64_C(0x0000008000000000);

//    static constexpr uint64_t kRmtDebugMauLogicalTableReg1          = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauLogicalTableReg2          = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauLogicalTableReg4          = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauLogicalTableReg8          = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMauLogicalTcamCol1           = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauLogicalTcamCol2           = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauLogicalTcamCol4           = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauLogicalTcamCol8           = UINT64_C(0x0000000800000000);

    static constexpr uint64_t kRmtDebugMauLogicalTcamLookup         = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugMauLogicalTcamUpdate         = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugMauLogicalTcamUpdateAddTind  = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugMauLogicalTcamUpdateDelTind  = UINT64_C(0x0000000800000000);
    static constexpr uint64_t kRmtDebugMauLogicalTcamFindTind       = UINT64_C(0x0000001000000000);
    static constexpr uint64_t kRmtDebugMauLogicalTcamUpdateTableMap = UINT64_C(0x0000002000000000);
    static constexpr uint64_t kRmtDebugMauLogicalTcamAttachToTcam   = UINT64_C(0x0000004000000000);
    static constexpr uint64_t kRmtDebugMauLogicalTcamDetachFromTcam = UINT64_C(0x0000008000000000);
    static constexpr uint64_t kRmtDebugMauLogicalTcamFindChain      = UINT64_C(0x0000010000000000);

//    static constexpr uint64_t kRmtDebugMauLogicalTcamReg1           = UINT64_C(0x0000010000000000);
//    static constexpr uint64_t kRmtDebugMauLogicalTcamReg2           = UINT64_C(0x0000020000000000);
//    static constexpr uint64_t kRmtDebugMauLogicalTcamReg4           = UINT64_C(0x0000040000000000);
//    static constexpr uint64_t kRmtDebugMauLogicalTcamReg8           = UINT64_C(0x0000080000000000);
//
//    static constexpr uint64_t kRmtDebugMauLookupResult1             = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauLookupResult2             = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauLookupResult4             = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauLookupResult8             = UINT64_C(0x0000000800000000);

    static constexpr uint64_t kRmtDebugMauMapramIdleSweep           = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugMauMapramIdleActive          = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugMauMapramColorRead           = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugMauMapramColorWrite          = UINT64_C(0x0000000800000000);

//    static constexpr uint64_t kRmtDebugMauMemory1                   = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauMemory2                   = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauMemory4                   = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauMemory8                   = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMauNotUsed1                  = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauNotUsed2                  = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauNotUsed4                  = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauNotUsed8                  = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMauOpHandler1                = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauOpHandler2                = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauOpHandler4                = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauOpHandler8                = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMauResultBus1                = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauResultBus2                = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauResultBus4                = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauResultBus8                = UINT64_C(0x0000000800000000);

    static constexpr uint64_t kRmtDebugMauSramLookup                = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugMauSramUpdate                = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugMauSramMatchOutput           = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugMauSramTindOutput            = UINT64_C(0x0000000800000000);
    static constexpr uint64_t kRmtDebugMauSramStatsRead             = UINT64_C(0x0000001000000000);
    static constexpr uint64_t kRmtDebugMauSramStatsWrite            = UINT64_C(0x0000002000000000);
    static constexpr uint64_t kRmtDebugMauSramMeterRead             = UINT64_C(0x0000004000000000);
    static constexpr uint64_t kRmtDebugMauSramMeterWrite            = UINT64_C(0x0000008000000000);
    static constexpr uint64_t kRmtDebugMauSramActionRead            = UINT64_C(0x0000010000000000);
    static constexpr uint64_t kRmtDebugMauSramActionWrite           = UINT64_C(0x0000020000000000);
    static constexpr uint64_t kRmtDebugMauSramSelectorRead          = UINT64_C(0x0000040000000000);
    static constexpr uint64_t kRmtDebugMauSramSelectorOutput        = UINT64_C(0x0000080000000000);
    static constexpr uint64_t kRmtDebugMauSramStatefulRead          = UINT64_C(0x0000100000000000);
    static constexpr uint64_t kRmtDebugMauSramStatefulWrite         = UINT64_C(0x0000200000000000);

    static constexpr uint64_t kRmtDebugMauSramColumnUpdateHitmap    = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugMauSramColumnUpdateLogtab    = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugMauSramColumnInnerHit        = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugMauSramColumnOuterHit        = UINT64_C(0x0000000800000000);
//    static constexpr uint64_t kRmtDebugMauSramColumn1               = UINT64_C(0x0000001000000000);
//    static constexpr uint64_t kRmtDebugMauSramColumn2               = UINT64_C(0x0000002000000000);
//
//    static constexpr uint64_t kRmtDebugMauSramColumnReg1            = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauSramColumnReg2            = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauSramColumnReg4            = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauSramColumnReg8            = UINT64_C(0x0000000800000000);

    static constexpr uint64_t kRmtDebugMauSramRowSetMatchBus        = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugMauSramRowGetMatchBus        = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugMauSramRowSetTindBus         = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugMauSramRowGetTindBus         = UINT64_C(0x0000000800000000);
    static constexpr uint64_t kRmtDebugMauSramRowSetHVBus           = UINT64_C(0x0000001000000000);
    static constexpr uint64_t kRmtDebugMauSramRowGetHVBus           = UINT64_C(0x0000002000000000);
//    static constexpr uint64_t kRmtDebugMauSramRow4                  = UINT64_C(0x0000004000000000);
//    static constexpr uint64_t kRmtDebugMauSramRow8                  = UINT64_C(0x0000008000000000);

    static constexpr uint64_t kRmtDebugMauStatsAluIncr              = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugMauStatsAlu2                 = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugMauStatsAlu4                 = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugMauStatsAlu8                 = UINT64_C(0x0000000800000000);

    static constexpr uint64_t kRmtDebugMauTcam_Tcam3Lookup          = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugMauTcam_Tcam3LookupDetail    = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugMauTcam_Tcam3DebugHit        = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugMauTcam_Tcam3DebugMiss       = UINT64_C(0x0000000800000000);
    static constexpr uint64_t kRmtDebugMauTcam_Tcam3DebugValid      = UINT64_C(0x0000001000000000);
    static constexpr uint64_t kRmtDebugMauTcam_Tcam3DebugMap        = UINT64_C(0x0000002000000000);
    static constexpr uint64_t kRmtDebugMauTcam_Tcam3_E              = UINT64_C(0x0000004000000000);
    static constexpr uint64_t kRmtDebugMauTcam_Tcam3_F              = UINT64_C(0x0000008000000000);
    static constexpr uint64_t kRmtDebugMauTcamLookup                = UINT64_C(0x0000010000000000);
    static constexpr uint64_t kRmtDebugMauTcamUpdate                = UINT64_C(0x0000020000000000);
//    static constexpr uint64_t kRmtDebugMauTcam1                     = UINT64_C(0x0000100000000000);
//    static constexpr uint64_t kRmtDebugMauTcam2                     = UINT64_C(0x0000200000000000);
//    static constexpr uint64_t kRmtDebugMauTcam4                     = UINT64_C(0x0000400000000000);
//    static constexpr uint64_t kRmtDebugMauTcam8                     = UINT64_C(0x0000800000000000);

//    static constexpr uint64_t kRmtDebugMauTcamReg1                  = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauTcamReg2                  = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauTcamReg4                  = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauTcamReg8                  = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugMauTcamRow1                  = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugMauTcamRow2                  = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugMauTcamRow4                  = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugMauTcamRow8                  = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugModel1                       = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugModel2                       = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugModel4                       = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugModel8                       = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugPacket1                      = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugPacket2                      = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugPacket4                      = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugPacket8                      = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugPacketBuffer1                = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugPacketBuffer2                = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugPacketBuffer4                = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugPacketBuffer8                = UINT64_C(0x0000000800000000);

    static constexpr uint64_t kRmtDebugParserParseLoop              = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugParserParseMatch             = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugParserParseNoMatch           = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugParserParseOK                = UINT64_C(0x0000000800000000);
    static constexpr uint64_t kRmtDebugParserParseError             = UINT64_C(0x0000001000000000);
    static constexpr uint64_t kRmtDebugParserParseErrorDetail       = UINT64_C(0x0000002000000000);
    static constexpr uint64_t kRmtDebugParserExtract                = UINT64_C(0x0000004000000000);
    static constexpr uint64_t kRmtDebugParserExtractError           = UINT64_C(0x0000008000000000);
    static constexpr uint64_t kRmtDebugParserExtractSoftError       = UINT64_C(0x0000010000000000);
    static constexpr uint64_t kRmtDebugParserError                  = UINT64_C(0x000001B000000000);
    static constexpr uint64_t kRmtDebugParserPriority               = UINT64_C(0x0000020000000000);
    static constexpr uint64_t kRmtDebugParserVersion                = UINT64_C(0x0000040000000000);
    static constexpr uint64_t kRmtDebugParserClot                   = UINT64_C(0x0000080000000000);
    static constexpr uint64_t kRmtDebugParserCounterStack           = UINT64_C(0x0000100000000000);
    static constexpr uint64_t kRmtDebugParserMemory                 = UINT64_C(0x0000200000000000);
    static constexpr uint64_t kRmtDebugParserHdrLen                 = UINT64_C(0x0000400000000000);

//    static constexpr uint64_t kRmtDebugParserBlock1                 = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugParserBlock2                 = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugParserBlock4                 = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugParserBlock8                 = UINT64_C(0x0000000800000000);

    static constexpr uint64_t kRmtDebugPhv1                         = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugPhv2                         = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugPhv4                         = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugPhv8                         = UINT64_C(0x0000000800000000);

//    static constexpr uint64_t kRmtDebugPhvFactory1                  = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugPhvFactory2                  = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugPhvFactory4                  = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugPhvFactory8                  = UINT64_C(0x0000000800000000);

    static constexpr uint64_t kRmtDebugPipeProcess                  = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugPipeRunMaus                  = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugPipeRunSingleMau             = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugPipe8                        = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugPort1                        = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugPort2                        = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugPort4                        = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugPort8                        = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugQueueing1                    = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugQueueing2                    = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugQueueing4                    = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugQueueing8                    = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugRegisterBlock1               = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugRegisterBlock2               = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugRegisterBlock4               = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugRegisterBlock8               = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugRegisterBlockIndirect1       = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugRegisterBlockIndirect2       = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugRegisterBlockIndirect4       = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugRegisterBlockIndirect8       = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugRmtObjectManager1            = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugRmtObjectManager2            = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugRmtObjectManager4            = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugRmtObjectManager8            = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugRmtOpHandler1                = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugRmtOpHandler2                = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugRmtOpHandler4                = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugRmtOpHandler8                = UINT64_C(0x0000000800000000);

    static constexpr uint64_t kRmtDebugRmtSweeperIdleSetInterval    = UINT64_C(0x0000000100000000);
    static constexpr uint64_t kRmtDebugRmtSweeperIdleLock           = UINT64_C(0x0000000200000000);
    static constexpr uint64_t kRmtDebugRmtSweeperIdleUnlock         = UINT64_C(0x0000000400000000);
    static constexpr uint64_t kRmtDebugRmtSweeperIdleSweep          = UINT64_C(0x0000000800000000);
    static constexpr uint64_t kRmtDebugRmtSweeperIdleDump           = UINT64_C(0x0000001000000000);
    static constexpr uint64_t kRmtDebugRmtSweeperIdleDumpWord       = UINT64_C(0x0000002000000000);
    static constexpr uint64_t kRmtDebugRmtSweeperStatsSetInterval   = UINT64_C(0x0000004000000000);
    static constexpr uint64_t kRmtDebugRmtSweeperStatsLock          = UINT64_C(0x0000008000000000);
    static constexpr uint64_t kRmtDebugRmtSweeperStatsUnlock        = UINT64_C(0x0000010000000000);
    static constexpr uint64_t kRmtDebugRmtSweeperStatsSweep         = UINT64_C(0x0000020000000000);
    static constexpr uint64_t kRmtDebugRmtSweeperStatsDump          = UINT64_C(0x0000040000000000);
    static constexpr uint64_t kRmtDebugRmtSweeperStatsDumpWord      = UINT64_C(0x0000080000000000);
    static constexpr uint64_t kRmtDebugRmtSweeperBarrier            = UINT64_C(0x0000100000000000);
    static constexpr uint64_t kRmtDebugRmtSweeperMeterSetInterval   = UINT64_C(0x0000200000000000);

//    static constexpr uint64_t kRmtDebugSram1                        = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugSram2                        = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugSram4                        = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugSram8                        = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugTcam1                        = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugTcam2                        = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugTcam4                        = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugTcam8                        = UINT64_C(0x0000000800000000);
//
//    static constexpr uint64_t kRmtDebugTcamRowVhWithReg1            = UINT64_C(0x0000000100000000);
//    static constexpr uint64_t kRmtDebugTcamRowVhWithReg2            = UINT64_C(0x0000000200000000);
//    static constexpr uint64_t kRmtDebugTcamRowVhWithReg4            = UINT64_C(0x0000000400000000);
//    static constexpr uint64_t kRmtDebugTcamRowVhWithReg8            = UINT64_C(0x0000000800000000);

    static constexpr uint64_t kRmtDebugMauSelectorMatchCentral      = UINT64_C(0x0000004000000000);
    static constexpr uint64_t kRmtDebugMauSelectorAlu               = UINT64_C(0x0000008000000000);

    static constexpr uint64_t kRmtDebugMauHashDistrib               = UINT64_C(0x0000010000000000);
    static constexpr uint64_t kRmtDebugMauStatefulAlu               = UINT64_C(0x0000020000000000);

    static constexpr uint64_t kRmtDebugMauMeter                     = UINT64_C(0x0000040000000000);
    static constexpr uint64_t kRmtDebugMauLpfMeter                  = UINT64_C(0x0000080000000000);
  };
}

#endif // _MODEL_CORE_RMT_DEBUG_
