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

#include <boost/algorithm/string.hpp>
#include <model_core/rmt-debug.h>

namespace model_core {

const char* RmtDebug::kDebugLevelStrings[] = {
  "FATAL ERROR", "ERROR", "error", "WARN", "INF", "V", "DBG", "TRC"
};

const std::map<std::string, uint64_t> RmtDebug::flags_map_ {
  // every kRmtDebug* constexpr has 0 added as a workaround to avoid them being
  // undefined when linking i.e. not odr-used
  {"Fatal", kRmtDebugFatal + UINT64_C(0)},
  {"Error", kRmtDebugError + UINT64_C(0)},
  {"Error2", kRmtDebugError2 + UINT64_C(0)},
  {"Warn", kRmtDebugWarn + UINT64_C(0)},
  {"Info", kRmtDebugInfo + UINT64_C(0)},
  {"Verbose", kRmtDebugVerbose + UINT64_C(0)},
  {"Debug", kRmtDebugDebug + UINT64_C(0)},
  {"Trace", kRmtDebugTrace + UINT64_C(0)},
  {"BadStuff", kRmtDebugBadStuff + UINT64_C(0)},
  {"BasicMask", kRmtDebugBasicMask + UINT64_C(0)},
  {"Create", kRmtDebugCreate + UINT64_C(0)},
  {"Delete", kRmtDebugDelete + UINT64_C(0)},
  {"Enter", kRmtDebugEnter + UINT64_C(0)},
  {"Exit", kRmtDebugExit + UINT64_C(0)},
  {"Read", kRmtDebugRead + UINT64_C(0)},
  {"Write", kRmtDebugWrite + UINT64_C(0)},
  {"Get", kRmtDebugGet + UINT64_C(0)},
  {"Set", kRmtDebugSet + UINT64_C(0)},
  {"Force", kRmtDebugForce + UINT64_C(0)},
//  {"ActionOutputHvXbar1", kRmtDebugActionOutputHvXbar1 + UINT64_C(0)},
//  {"ActionOutputHvXbar2", kRmtDebugActionOutputHvXbar2 + UINT64_C(0)},
//  {"ActionOutputHvXbar4", kRmtDebugActionOutputHvXbar4 + UINT64_C(0)},
//  {"ActionOutputHvXbar8", kRmtDebugActionOutputHvXbar8 + UINT64_C(0)},
//  {"BitVector1", kRmtDebugBitVector1 + UINT64_C(0)},
//  {"BitVector2", kRmtDebugBitVector2 + UINT64_C(0)},
//  {"BitVector4", kRmtDebugBitVector4 + UINT64_C(0)},
//  {"BitVector8", kRmtDebugBitVector8 + UINT64_C(0)},
//  {"CacheId1", kRmtDebugCacheId1 + UINT64_C(0)},
//  {"CacheId2", kRmtDebugCacheId2 + UINT64_C(0)},
//  {"CacheId4", kRmtDebugCacheId4 + UINT64_C(0)},
//  {"CacheId8", kRmtDebugCacheId8 + UINT64_C(0)},
//  {"Chip1", kRmtDebugChip1 + UINT64_C(0)},
//  {"Chip2", kRmtDebugChip2 + UINT64_C(0)},
//  {"Chip4", kRmtDebugChip4 + UINT64_C(0)},
//  {"Chip8", kRmtDebugChip8 + UINT64_C(0)},
  {"CsumEngCalcFastB0B1", kRmtDebugCsumEngCalcFastB0B1 + UINT64_C(0)},
  {"CsumEngCalcNoFastB0B1", kRmtDebugCsumEngCalcNoFastB0B1 + UINT64_C(0)},
  {"CsumEngCalcBefore", kRmtDebugCsumEngCalcBefore + UINT64_C(0)},
  {"CsumEngCalcSlow", kRmtDebugCsumEngCalcSlow + UINT64_C(0)},
  {"CsumEngCalcFin", kRmtDebugCsumEngCalcFin + UINT64_C(0)},
  {"CsumEngErr", kRmtDebugCsumEngErr + UINT64_C(0)},
//  {"CsumEng1", kRmtDebugCsumEng1 + UINT64_C(0)},
//  {"CsumEng2", kRmtDebugCsumEng2 + UINT64_C(0)},
//  {"CsumEng4", kRmtDebugCsumEng4 + UINT64_C(0)},
//  {"CsumEng8", kRmtDebugCsumEng8 + UINT64_C(0)},
  {"DeparserCsumChange", kRmtDebugDeparserCsumChange + UINT64_C(0)},
  {"DeparserCsumSummary", kRmtDebugDeparserCsumSummary + UINT64_C(0)},
  {"DeparserCsumNonZero", kRmtDebugDeparserCsumNonZero + UINT64_C(0)},
  {"DeparserCsumVerbose", kRmtDebugDeparserCsumVerbose + UINT64_C(0)},
  {"Deparser1", kRmtDebugDeparser1 + UINT64_C(0)},
  {"Deparser2", kRmtDebugDeparser2 + UINT64_C(0)},
  {"Deparser4", kRmtDebugDeparser4 + UINT64_C(0)},
  {"Deparser8", kRmtDebugDeparser8 + UINT64_C(0)},
  {"DeparserBlock1", kRmtDebugDeparserBlock1 + UINT64_C(0)},
  {"DeparserBlock2", kRmtDebugDeparserBlock2 + UINT64_C(0)},
  {"DeparserBlock4", kRmtDebugDeparserBlock4 + UINT64_C(0)},
  {"DeparserBlock8", kRmtDebugDeparserBlock8 + UINT64_C(0)},
//  {"HashAddressVhXbar1", kRmtDebugHashAddressVhXbar1 + UINT64_C(0)},
//  {"HashAddressVhXbar2", kRmtDebugHashAddressVhXbar2 + UINT64_C(0)},
//  {"HashAddressVhXbar4", kRmtDebugHashAddressVhXbar4 + UINT64_C(0)},
//  {"HashAddressVhXbar8", kRmtDebugHashAddressVhXbar8 + UINT64_C(0)},
//  {"HashAddressVhXbarWithReg1", kRmtDebugHashAddressVhXbarWithReg1 + UINT64_C(0)},
//  {"HashAddressVhXbarWithReg2", kRmtDebugHashAddressVhXbarWithReg2 + UINT64_C(0)},
//  {"HashAddressVhXbarWithReg4", kRmtDebugHashAddressVhXbarWithReg4 + UINT64_C(0)},
//  {"HashAddressVhXbarWithReg8", kRmtDebugHashAddressVhXbarWithReg8 + UINT64_C(0)},
//  {"IndirectAccessBlock1", kRmtDebugIndirectAccessBlock1 + UINT64_C(0)},
//  {"IndirectAccessBlock2", kRmtDebugIndirectAccessBlock2 + UINT64_C(0)},
//  {"IndirectAccessBlock4", kRmtDebugIndirectAccessBlock4 + UINT64_C(0)},
//  {"IndirectAccessBlock8", kRmtDebugIndirectAccessBlock8 + UINT64_C(0)},
//  {"InputControlledXbar1", kRmtDebugInputControlledXbar1 + UINT64_C(0)},
//  {"InputControlledXbar2", kRmtDebugInputControlledXbar2 + UINT64_C(0)},
//  {"InputControlledXbar4", kRmtDebugInputControlledXbar4 + UINT64_C(0)},
//  {"InputControlledXbar8", kRmtDebugInputControlledXbar8 + UINT64_C(0)},
  {"Instr1", kRmtDebugInstr1 + UINT64_C(0)},
//  {"Instr2", kRmtDebugInstr2 + UINT64_C(0)},
//  {"Instr4", kRmtDebugInstr4 + UINT64_C(0)},
//  {"Instr8", kRmtDebugInstr8 + UINT64_C(0)},
//  {"MatchDataInputVhXbar1", kRmtDebugMatchDataInputVhXbar1 + UINT64_C(0)},
//  {"MatchDataInputVhXbar2", kRmtDebugMatchDataInputVhXbar2 + UINT64_C(0)},
//  {"MatchDataInputVhXbar4", kRmtDebugMatchDataInputVhXbar4 + UINT64_C(0)},
//  {"MatchDataInputVhXbar8", kRmtDebugMatchDataInputVhXbar8 + UINT64_C(0)},
//  {"MatchDataInputVhXbarWithReg1", kRmtDebugMatchDataInputVhXbarWithReg1 + UINT64_C(0)},
//  {"MatchDataInputVhXbarWithReg2", kRmtDebugMatchDataInputVhXbarWithReg2 + UINT64_C(0)},
//  {"MatchDataInputVhXbarWithReg4", kRmtDebugMatchDataInputVhXbarWithReg4 + UINT64_C(0)},
//  {"MatchDataInputVhXbarWithReg8", kRmtDebugMatchDataInputVhXbarWithReg8 + UINT64_C(0)},
  {"MauLookup", kRmtDebugMauLookup + UINT64_C(0)},
  {"MauMatchDistrib", kRmtDebugMauMatchDistrib + UINT64_C(0)},
  {"MauFindActions", kRmtDebugMauFindActions + UINT64_C(0)},
  {"MauProcessAction", kRmtDebugMauProcessAction + UINT64_C(0)},
  {"MauDumpActionHVOutputBus", kRmtDebugMauDumpActionHVOutputBus + UINT64_C(0)},
//  {"Mau2", kRmtDebugMau2 + UINT64_C(0)},
//  {"Mau4", kRmtDebugMau4 + UINT64_C(0)},
//  {"Mau8", kRmtDebugMau8 + UINT64_C(0)},
//  {"Mau10", kRmtDebugMau10 + UINT64_C(0)},
  {"MauInput", kRmtDebugMauInput + UINT64_C(0)},
  {"MauAlu", kRmtDebugMauAlu + UINT64_C(0)},
  {"MauAddrDistOutputToBus", kRmtDebugMauAddrDistOutputToBus + UINT64_C(0)},
  {"MauAddrDistSweep", kRmtDebugMauAddrDistSweep + UINT64_C(0)},
  {"MauAddrDistVpn", kRmtDebugMauAddrDistVpn + UINT64_C(0)},
  {"MauAddrDistClaim", kRmtDebugMauAddrDistClaim + UINT64_C(0)},
  {"MauAddrDistAction", kRmtDebugMauAddrDistAction + UINT64_C(0)},
  {"MauAddrDistStats", kRmtDebugMauAddrDistStats + UINT64_C(0)},
  {"MauAddrDistMeter", kRmtDebugMauAddrDistMeter + UINT64_C(0)},
  {"MauAddrDistIdle", kRmtDebugMauAddrDistIdle + UINT64_C(0)},
  {"MauAddrDistHdr", kRmtDebugMauAddrDistHdr + UINT64_C(0)},
  {"MauAddrDistEop", kRmtDebugMauAddrDistEop + UINT64_C(0)},
  {"MauAddrDistColorWrite", kRmtDebugMauAddrDistColorWrite + UINT64_C(0)},
//  {"MauDependencies1", kRmtDebugMauDependencies1 + UINT64_C(0)},
//  {"MauDependencies2", kRmtDebugMauDependencies2 + UINT64_C(0)},
//  {"MauDependencies4", kRmtDebugMauDependencies4 + UINT64_C(0)},
//  {"MauDependencies8", kRmtDebugMauDependencies8 + UINT64_C(0)},
//  {"MauGatewayPayload1", kRmtDebugMauGatewayPayload1 + UINT64_C(0)},
//  {"MauGatewayPayload2", kRmtDebugMauGatewayPayload2 + UINT64_C(0)},
//  {"MauGatewayPayload4", kRmtDebugMauGatewayPayload4 + UINT64_C(0)},
//  {"MauGatewayPayload8", kRmtDebugMauGatewayPayload8 + UINT64_C(0)},
//  {"MauGatewayPayloadReg1", kRmtDebugMauGatewayPayloadReg1 + UINT64_C(0)},
//  {"MauGatewayPayloadReg2", kRmtDebugMauGatewayPayloadReg2 + UINT64_C(0)},
//  {"MauGatewayPayloadReg4", kRmtDebugMauGatewayPayloadReg4 + UINT64_C(0)},
//  {"MauGatewayPayloadReg8", kRmtDebugMauGatewayPayloadReg8 + UINT64_C(0)},
//  {"MauGatewayTable1", kRmtDebugMauGatewayTable1 + UINT64_C(0)},
//  {"MauGatewayTable2", kRmtDebugMauGatewayTable2 + UINT64_C(0)},
//  {"MauGatewayTable4", kRmtDebugMauGatewayTable4 + UINT64_C(0)},
//  {"MauGatewayTable8", kRmtDebugMauGatewayTable8 + UINT64_C(0)},
//  {"MauGatewayTableReg1", kRmtDebugMauGatewayTableReg1 + UINT64_C(0)},
//  {"MauGatewayTableReg2", kRmtDebugMauGatewayTableReg2 + UINT64_C(0)},
//  {"MauGatewayTableReg4", kRmtDebugMauGatewayTableReg4 + UINT64_C(0)},
//  {"MauGatewayTableReg8", kRmtDebugMauGatewayTableReg8 + UINT64_C(0)},
//  {"MauHashGenerator1", kRmtDebugMauHashGenerator1 + UINT64_C(0)},
//  {"MauHashGenerator2", kRmtDebugMauHashGenerator2 + UINT64_C(0)},
//  {"MauHashGenerator4", kRmtDebugMauHashGenerator4 + UINT64_C(0)},
//  {"MauHashGenerator8", kRmtDebugMauHashGenerator8 + UINT64_C(0)},
//  {"MauHashGeneratorWithReg1", kRmtDebugMauHashGeneratorWithReg1 + UINT64_C(0)},
//  {"MauHashGeneratorWithReg2", kRmtDebugMauHashGeneratorWithReg2 + UINT64_C(0)},
//  {"MauHashGeneratorWithReg4", kRmtDebugMauHashGeneratorWithReg4 + UINT64_C(0)},
//  {"MauHashGeneratorWithReg8", kRmtDebugMauHashGeneratorWithReg8 + UINT64_C(0)},
//  {"MauInput1", kRmtDebugMauInput1 + UINT64_C(0)},
//  {"MauInput2", kRmtDebugMauInput2 + UINT64_C(0)},
//  {"MauInput4", kRmtDebugMauInput4 + UINT64_C(0)},
//  {"MauInput8", kRmtDebugMauInput8 + UINT64_C(0)},
//  {"MauInputXbar1", kRmtDebugMauInputXbar1 + UINT64_C(0)},
//  {"MauInputXbar2", kRmtDebugMauInputXbar2 + UINT64_C(0)},
//  {"MauInputXbar4", kRmtDebugMauInputXbar4 + UINT64_C(0)},
//  {"MauInputXbar8", kRmtDebugMauInputXbar8 + UINT64_C(0)},
//  {"MauInputXbarWithReg1", kRmtDebugMauInputXbarWithReg1 + UINT64_C(0)},
//  {"MauInputXbarWithReg2", kRmtDebugMauInputXbarWithReg2 + UINT64_C(0)},
//  {"MauInputXbarWithReg4", kRmtDebugMauInputXbarWithReg4 + UINT64_C(0)},
//  {"MauInputXbarWithReg8", kRmtDebugMauInputXbarWithReg8 + UINT64_C(0)},
//  {"MauInstrStore1", kRmtDebugMauInstrStore1 + UINT64_C(0)},
//  {"MauInstrStore2", kRmtDebugMauInstrStore2 + UINT64_C(0)},
//  {"MauInstrStore4", kRmtDebugMauInstrStore4 + UINT64_C(0)},
//  {"MauInstrStore8", kRmtDebugMauInstrStore8 + UINT64_C(0)},
  {"MauLogicalRowFindAction", kRmtDebugMauLogicalRowFindAction + UINT64_C(0)},
  {"MauLogicalRowActionOutput", kRmtDebugMauLogicalRowActionOutput + UINT64_C(0)},
  {"MauLogicalRowHacktionOffset", kRmtDebugMauLogicalRowHacktionOffset + UINT64_C(0)},
  {"MauLogicalRowData", kRmtDebugMauLogicalRowData + UINT64_C(0)},
  {"MauLogicalRowAddr", kRmtDebugMauLogicalRowAddr + UINT64_C(0)},
//  {"MauLogicalRow4", kRmtDebugMauLogicalRow4 + UINT64_C(0)},
//  {"MauLogicalRow8", kRmtDebugMauLogicalRow8 + UINT64_C(0)},
//  {"MauLogicalRowReg1", kRmtDebugMauLogicalRowReg1 + UINT64_C(0)},
//  {"MauLogicalRowReg2", kRmtDebugMauLogicalRowReg2 + UINT64_C(0)},
//  {"MauLogicalRowReg4", kRmtDebugMauLogicalRowReg4 + UINT64_C(0)},
//  {"MauLogicalRowReg8", kRmtDebugMauLogicalRowReg8 + UINT64_C(0)},
  {"MauLogicalTableLookup", kRmtDebugMauLogicalTableLookup + UINT64_C(0)},
  {"MauLogicalTableFindActions", kRmtDebugMauLogicalTableFindActions + UINT64_C(0)},
  {"MauLogicalTableGetAddrs", kRmtDebugMauLogicalTableGetAddrs + UINT64_C(0)},
  {"MauLogicalTableSwizzle", kRmtDebugMauLogicalTableSwizzle + UINT64_C(0)},
  {"MauLogicalTableSetBus", kRmtDebugMauLogicalTableSetBus + UINT64_C(0)},
  {"MauLogicalTableGetBus", kRmtDebugMauLogicalTableGetBus + UINT64_C(0)},
//  {"MauLogicalTable4", kRmtDebugMauLogicalTable4 + UINT64_C(0)},
//  {"MauLogicalTable8", kRmtDebugMauLogicalTable8 + UINT64_C(0)},
//  {"MauLogicalTableReg1", kRmtDebugMauLogicalTableReg1 + UINT64_C(0)},
//  {"MauLogicalTableReg2", kRmtDebugMauLogicalTableReg2 + UINT64_C(0)},
//  {"MauLogicalTableReg4", kRmtDebugMauLogicalTableReg4 + UINT64_C(0)},
//  {"MauLogicalTableReg8", kRmtDebugMauLogicalTableReg8 + UINT64_C(0)},
//  {"MauLogicalTcamCol1", kRmtDebugMauLogicalTcamCol1 + UINT64_C(0)},
//  {"MauLogicalTcamCol2", kRmtDebugMauLogicalTcamCol2 + UINT64_C(0)},
//  {"MauLogicalTcamCol4", kRmtDebugMauLogicalTcamCol4 + UINT64_C(0)},
//  {"MauLogicalTcamCol8", kRmtDebugMauLogicalTcamCol8 + UINT64_C(0)},
  {"MauLogicalTcamLookup", kRmtDebugMauLogicalTcamLookup + UINT64_C(0)},
  {"MauLogicalTcamUpdate", kRmtDebugMauLogicalTcamUpdate + UINT64_C(0)},
  {"MauLogicalTcamUpdateAddTind", kRmtDebugMauLogicalTcamUpdateAddTind + UINT64_C(0)},
  {"MauLogicalTcamUpdateDelTind", kRmtDebugMauLogicalTcamUpdateDelTind + UINT64_C(0)},
  {"MauLogicalTcamFindTind", kRmtDebugMauLogicalTcamFindTind + UINT64_C(0)},
  {"MauLogicalTcamUpdateTableMap", kRmtDebugMauLogicalTcamUpdateTableMap + UINT64_C(0)},
  {"MauLogicalTcamAttachToTcam", kRmtDebugMauLogicalTcamAttachToTcam + UINT64_C(0)},
  {"MauLogicalTcamDetachFromTcam", kRmtDebugMauLogicalTcamDetachFromTcam + UINT64_C(0)},
  {"MauLogicalTcamFindChain", kRmtDebugMauLogicalTcamFindChain + UINT64_C(0)},
//  {"MauLogicalTcamReg1", kRmtDebugMauLogicalTcamReg1 + UINT64_C(0)},
//  {"MauLogicalTcamReg2", kRmtDebugMauLogicalTcamReg2 + UINT64_C(0)},
//  {"MauLogicalTcamReg4", kRmtDebugMauLogicalTcamReg4 + UINT64_C(0)},
//  {"MauLogicalTcamReg8", kRmtDebugMauLogicalTcamReg8 + UINT64_C(0)},
//  {"MauLookupResult1", kRmtDebugMauLookupResult1 + UINT64_C(0)},
//  {"MauLookupResult2", kRmtDebugMauLookupResult2 + UINT64_C(0)},
//  {"MauLookupResult4", kRmtDebugMauLookupResult4 + UINT64_C(0)},
//  {"MauLookupResult8", kRmtDebugMauLookupResult8 + UINT64_C(0)},
  {"MauMapramIdleSweep", kRmtDebugMauMapramIdleSweep + UINT64_C(0)},
  {"MauMapramIdleActive", kRmtDebugMauMapramIdleActive + UINT64_C(0)},
  {"MauMapramColorRead", kRmtDebugMauMapramColorRead + UINT64_C(0)},
  {"MauMapramColorWrite", kRmtDebugMauMapramColorWrite + UINT64_C(0)},
//  {"MauMemory1", kRmtDebugMauMemory1 + UINT64_C(0)},
//  {"MauMemory2", kRmtDebugMauMemory2 + UINT64_C(0)},
//  {"MauMemory4", kRmtDebugMauMemory4 + UINT64_C(0)},
//  {"MauMemory8", kRmtDebugMauMemory8 + UINT64_C(0)},
//  {"MauNotUsed1", kRmtDebugMauNotUsed1 + UINT64_C(0)},
//  {"MauNotUsed2", kRmtDebugMauNotUsed2 + UINT64_C(0)},
//  {"MauNotUsed4", kRmtDebugMauNotUsed4 + UINT64_C(0)},
//  {"MauNotUsed8", kRmtDebugMauNotUsed8 + UINT64_C(0)},
//  {"MauOpHandler1", kRmtDebugMauOpHandler1 + UINT64_C(0)},
//  {"MauOpHandler2", kRmtDebugMauOpHandler2 + UINT64_C(0)},
//  {"MauOpHandler4", kRmtDebugMauOpHandler4 + UINT64_C(0)},
//  {"MauOpHandler8", kRmtDebugMauOpHandler8 + UINT64_C(0)},
//  {"MauResultBus1", kRmtDebugMauResultBus1 + UINT64_C(0)},
//  {"MauResultBus2", kRmtDebugMauResultBus2 + UINT64_C(0)},
//  {"MauResultBus4", kRmtDebugMauResultBus4 + UINT64_C(0)},
//  {"MauResultBus8", kRmtDebugMauResultBus8 + UINT64_C(0)},
  {"MauSramLookup", kRmtDebugMauSramLookup + UINT64_C(0)},
  {"MauSramUpdate", kRmtDebugMauSramUpdate + UINT64_C(0)},
  {"MauSramMatchOutput", kRmtDebugMauSramMatchOutput + UINT64_C(0)},
  {"MauSramTindOutput", kRmtDebugMauSramTindOutput + UINT64_C(0)},
  {"MauSramStatsRead", kRmtDebugMauSramStatsRead + UINT64_C(0)},
  {"MauSramStatsWrite", kRmtDebugMauSramStatsWrite + UINT64_C(0)},
  {"MauSramMeterRead", kRmtDebugMauSramMeterRead + UINT64_C(0)},
  {"MauSramMeterWrite", kRmtDebugMauSramMeterWrite + UINT64_C(0)},
  {"MauSramActionRead", kRmtDebugMauSramActionRead + UINT64_C(0)},
  {"MauSramActionWrite", kRmtDebugMauSramActionWrite + UINT64_C(0)},
  {"MauSramSelectorRead", kRmtDebugMauSramSelectorRead + UINT64_C(0)},
  {"MauSramSelectorOutput", kRmtDebugMauSramSelectorOutput + UINT64_C(0)},
  {"MauSramStatefulRead", kRmtDebugMauSramStatefulRead + UINT64_C(0)},
  {"MauSramStatefulWrite", kRmtDebugMauSramStatefulWrite + UINT64_C(0)},
  {"MauSramColumnUpdateHitmap", kRmtDebugMauSramColumnUpdateHitmap + UINT64_C(0)},
  {"MauSramColumnUpdateLogtab", kRmtDebugMauSramColumnUpdateLogtab + UINT64_C(0)},
  {"MauSramColumnInnerHit", kRmtDebugMauSramColumnInnerHit + UINT64_C(0)},
  {"MauSramColumnOuterHit", kRmtDebugMauSramColumnOuterHit + UINT64_C(0)},
//  {"MauSramColumn1", kRmtDebugMauSramColumn1 + UINT64_C(0)},
//  {"MauSramColumn2", kRmtDebugMauSramColumn2 + UINT64_C(0)},
//  {"MauSramColumnReg1", kRmtDebugMauSramColumnReg1 + UINT64_C(0)},
//  {"MauSramColumnReg2", kRmtDebugMauSramColumnReg2 + UINT64_C(0)},
//  {"MauSramColumnReg4", kRmtDebugMauSramColumnReg4 + UINT64_C(0)},
//  {"MauSramColumnReg8", kRmtDebugMauSramColumnReg8 + UINT64_C(0)},
  {"MauSramRowSetMatchBus", kRmtDebugMauSramRowSetMatchBus + UINT64_C(0)},
  {"MauSramRowGetMatchBus", kRmtDebugMauSramRowGetMatchBus + UINT64_C(0)},
  {"MauSramRowSetTindBus", kRmtDebugMauSramRowSetTindBus + UINT64_C(0)},
  {"MauSramRowGetTindBus", kRmtDebugMauSramRowGetTindBus + UINT64_C(0)},
  {"MauSramRowSetHVBus", kRmtDebugMauSramRowSetHVBus + UINT64_C(0)},
  {"MauSramRowGetHVBus", kRmtDebugMauSramRowGetHVBus + UINT64_C(0)},
//  {"MauSramRow4", kRmtDebugMauSramRow4 + UINT64_C(0)},
//  {"MauSramRow8", kRmtDebugMauSramRow8 + UINT64_C(0)},
  {"MauStatsAluIncr", kRmtDebugMauStatsAluIncr + UINT64_C(0)},
  {"MauStatsAlu2", kRmtDebugMauStatsAlu2 + UINT64_C(0)},
  {"MauStatsAlu4", kRmtDebugMauStatsAlu4 + UINT64_C(0)},
  {"MauStatsAlu8", kRmtDebugMauStatsAlu8 + UINT64_C(0)},
  {"MauTcam_Tcam3Lookup", kRmtDebugMauTcam_Tcam3Lookup + UINT64_C(0)},
  {"MauTcam_Tcam3LookupDetail", kRmtDebugMauTcam_Tcam3LookupDetail + UINT64_C(0)},
  {"MauTcam_Tcam3DebugHit", kRmtDebugMauTcam_Tcam3DebugHit + UINT64_C(0)},
  {"MauTcam_Tcam3DebugMiss", kRmtDebugMauTcam_Tcam3DebugMiss + UINT64_C(0)},
  {"MauTcam_Tcam3DebugValid", kRmtDebugMauTcam_Tcam3DebugValid + UINT64_C(0)},
  {"MauTcam_Tcam3DebugMap", kRmtDebugMauTcam_Tcam3DebugMap + UINT64_C(0)},
  {"MauTcam_Tcam3_E", kRmtDebugMauTcam_Tcam3_E + UINT64_C(0)},
  {"MauTcam_Tcam3_F", kRmtDebugMauTcam_Tcam3_F + UINT64_C(0)},
  {"MauTcamLookup", kRmtDebugMauTcamLookup + UINT64_C(0)},
  {"MauTcamUpdate", kRmtDebugMauTcamUpdate + UINT64_C(0)},
//  {"MauTcam1", kRmtDebugMauTcam1 + UINT64_C(0)},
//  {"MauTcam2", kRmtDebugMauTcam2 + UINT64_C(0)},
//  {"MauTcam4", kRmtDebugMauTcam4 + UINT64_C(0)},
//  {"MauTcam8", kRmtDebugMauTcam8 + UINT64_C(0)},
//  {"MauTcamReg1", kRmtDebugMauTcamReg1 + UINT64_C(0)},
//  {"MauTcamReg2", kRmtDebugMauTcamReg2 + UINT64_C(0)},
//  {"MauTcamReg4", kRmtDebugMauTcamReg4 + UINT64_C(0)},
//  {"MauTcamReg8", kRmtDebugMauTcamReg8 + UINT64_C(0)},
//  {"MauTcamRow1", kRmtDebugMauTcamRow1 + UINT64_C(0)},
//  {"MauTcamRow2", kRmtDebugMauTcamRow2 + UINT64_C(0)},
//  {"MauTcamRow4", kRmtDebugMauTcamRow4 + UINT64_C(0)},
//  {"MauTcamRow8", kRmtDebugMauTcamRow8 + UINT64_C(0)},
//  {"Model1", kRmtDebugModel1 + UINT64_C(0)},
//  {"Model2", kRmtDebugModel2 + UINT64_C(0)},
//  {"Model4", kRmtDebugModel4 + UINT64_C(0)},
//  {"Model8", kRmtDebugModel8 + UINT64_C(0)},
//  {"Packet1", kRmtDebugPacket1 + UINT64_C(0)},
//  {"Packet2", kRmtDebugPacket2 + UINT64_C(0)},
//  {"Packet4", kRmtDebugPacket4 + UINT64_C(0)},
//  {"Packet8", kRmtDebugPacket8 + UINT64_C(0)},
//  {"PacketBuffer1", kRmtDebugPacketBuffer1 + UINT64_C(0)},
//  {"PacketBuffer2", kRmtDebugPacketBuffer2 + UINT64_C(0)},
//  {"PacketBuffer4", kRmtDebugPacketBuffer4 + UINT64_C(0)},
//  {"PacketBuffer8", kRmtDebugPacketBuffer8 + UINT64_C(0)},
  {"ParserParseLoop", kRmtDebugParserParseLoop + UINT64_C(0)},
  {"ParserParseMatch", kRmtDebugParserParseMatch + UINT64_C(0)},
  {"ParserParseNoMatch", kRmtDebugParserParseNoMatch + UINT64_C(0)},
  {"ParserParseOK", kRmtDebugParserParseOK + UINT64_C(0)},
  {"ParserParseError", kRmtDebugParserParseError + UINT64_C(0)},
  {"ParserParseErrorDetail", kRmtDebugParserParseErrorDetail + UINT64_C(0)},
  {"ParserExtract", kRmtDebugParserExtract + UINT64_C(0)},
  {"ParserExtractError", kRmtDebugParserExtractError + UINT64_C(0)},
  {"ParserExtractSoftError", kRmtDebugParserExtractSoftError + UINT64_C(0)},
  {"ParserError", kRmtDebugParserError + UINT64_C(0)},
  {"ParserPriority", kRmtDebugParserPriority + UINT64_C(0)},
  {"ParserVersion", kRmtDebugParserVersion + UINT64_C(0)},
  {"ParserClot", kRmtDebugParserClot + UINT64_C(0)},
  {"ParserCounterStack", kRmtDebugParserCounterStack + UINT64_C(0)},
  {"ParserMemory", kRmtDebugParserMemory + UINT64_C(0)},
  {"ParserHdrLen", kRmtDebugParserHdrLen + UINT64_C(0)},
//  {"ParserBlock1", kRmtDebugParserBlock1 + UINT64_C(0)},
//  {"ParserBlock2", kRmtDebugParserBlock2 + UINT64_C(0)},
//  {"ParserBlock4", kRmtDebugParserBlock4 + UINT64_C(0)},
//  {"ParserBlock8", kRmtDebugParserBlock8 + UINT64_C(0)},
  {"Phv1", kRmtDebugPhv1 + UINT64_C(0)},
//  {"Phv2", kRmtDebugPhv2 + UINT64_C(0)},
//  {"Phv4", kRmtDebugPhv4 + UINT64_C(0)},
//  {"Phv8", kRmtDebugPhv8 + UINT64_C(0)},
//  {"PhvFactory1", kRmtDebugPhvFactory1 + UINT64_C(0)},
//  {"PhvFactory2", kRmtDebugPhvFactory2 + UINT64_C(0)},
//  {"PhvFactory4", kRmtDebugPhvFactory4 + UINT64_C(0)},
//  {"PhvFactory8", kRmtDebugPhvFactory8 + UINT64_C(0)},
  {"PipeProcess", kRmtDebugPipeProcess + UINT64_C(0)},
  {"PipeRunMaus", kRmtDebugPipeRunMaus + UINT64_C(0)},
  {"PipeRunSingleMau", kRmtDebugPipeRunSingleMau + UINT64_C(0)},
//  {"Pipe8", kRmtDebugPipe8 + UINT64_C(0)},
//  {"Port1", kRmtDebugPort1 + UINT64_C(0)},
//  {"Port2", kRmtDebugPort2 + UINT64_C(0)},
//  {"Port4", kRmtDebugPort4 + UINT64_C(0)},
//  {"Port8", kRmtDebugPort8 + UINT64_C(0)},
//  {"Queueing1", kRmtDebugQueueing1 + UINT64_C(0)},
//  {"Queueing2", kRmtDebugQueueing2 + UINT64_C(0)},
//  {"Queueing4", kRmtDebugQueueing4 + UINT64_C(0)},
//  {"Queueing8", kRmtDebugQueueing8 + UINT64_C(0)},
//  {"RegisterBlock1", kRmtDebugRegisterBlock1 + UINT64_C(0)},
//  {"RegisterBlock2", kRmtDebugRegisterBlock2 + UINT64_C(0)},
//  {"RegisterBlock4", kRmtDebugRegisterBlock4 + UINT64_C(0)},
//  {"RegisterBlock8", kRmtDebugRegisterBlock8 + UINT64_C(0)},
//  {"RegisterBlockIndirect1", kRmtDebugRegisterBlockIndirect1 + UINT64_C(0)},
//  {"RegisterBlockIndirect2", kRmtDebugRegisterBlockIndirect2 + UINT64_C(0)},
//  {"RegisterBlockIndirect4", kRmtDebugRegisterBlockIndirect4 + UINT64_C(0)},
//  {"RegisterBlockIndirect8", kRmtDebugRegisterBlockIndirect8 + UINT64_C(0)},
//  {"RmtObjectManager1", kRmtDebugRmtObjectManager1 + UINT64_C(0)},
//  {"RmtObjectManager2", kRmtDebugRmtObjectManager2 + UINT64_C(0)},
//  {"RmtObjectManager4", kRmtDebugRmtObjectManager4 + UINT64_C(0)},
//  {"RmtObjectManager8", kRmtDebugRmtObjectManager8 + UINT64_C(0)},
//  {"RmtOpHandler1", kRmtDebugRmtOpHandler1 + UINT64_C(0)},
//  {"RmtOpHandler2", kRmtDebugRmtOpHandler2 + UINT64_C(0)},
//  {"RmtOpHandler4", kRmtDebugRmtOpHandler4 + UINT64_C(0)},
//  {"RmtOpHandler8", kRmtDebugRmtOpHandler8 + UINT64_C(0)},
  {"RmtSweeperIdleSetInterval", kRmtDebugRmtSweeperIdleSetInterval + UINT64_C(0)},
  {"RmtSweeperIdleLock", kRmtDebugRmtSweeperIdleLock + UINT64_C(0)},
  {"RmtSweeperIdleUnlock", kRmtDebugRmtSweeperIdleUnlock + UINT64_C(0)},
  {"RmtSweeperIdleSweep", kRmtDebugRmtSweeperIdleSweep + UINT64_C(0)},
  {"RmtSweeperIdleDump", kRmtDebugRmtSweeperIdleDump + UINT64_C(0)},
  {"RmtSweeperIdleDumpWord", kRmtDebugRmtSweeperIdleDumpWord + UINT64_C(0)},
  {"RmtSweeperStatsSetInterval", kRmtDebugRmtSweeperStatsSetInterval + UINT64_C(0)},
  {"RmtSweeperStatsLock", kRmtDebugRmtSweeperStatsLock + UINT64_C(0)},
  {"RmtSweeperStatsUnlock", kRmtDebugRmtSweeperStatsUnlock + UINT64_C(0)},
  {"RmtSweeperStatsSweep", kRmtDebugRmtSweeperStatsSweep + UINT64_C(0)},
  {"RmtSweeperStatsDump", kRmtDebugRmtSweeperStatsDump + UINT64_C(0)},
  {"RmtSweeperStatsDumpWord", kRmtDebugRmtSweeperStatsDumpWord + UINT64_C(0)},
  {"RmtSweeperBarrier", kRmtDebugRmtSweeperBarrier + UINT64_C(0)},
  {"RmtSweeperMeterSetInterval", kRmtDebugRmtSweeperMeterSetInterval + UINT64_C(0)},
//  {"Sram1", kRmtDebugSram1 + UINT64_C(0)},
//  {"Sram2", kRmtDebugSram2 + UINT64_C(0)},
//  {"Sram4", kRmtDebugSram4 + UINT64_C(0)},
//  {"Sram8", kRmtDebugSram8 + UINT64_C(0)},
//  {"Tcam1", kRmtDebugTcam1 + UINT64_C(0)},
//  {"Tcam2", kRmtDebugTcam2 + UINT64_C(0)},
//  {"Tcam4", kRmtDebugTcam4 + UINT64_C(0)},
//  {"Tcam8", kRmtDebugTcam8 + UINT64_C(0)},
//  {"TcamRowVhWithReg1", kRmtDebugTcamRowVhWithReg1 + UINT64_C(0)},
//  {"TcamRowVhWithReg2", kRmtDebugTcamRowVhWithReg2 + UINT64_C(0)},
//  {"TcamRowVhWithReg4", kRmtDebugTcamRowVhWithReg4 + UINT64_C(0)},
//  {"TcamRowVhWithReg8", kRmtDebugTcamRowVhWithReg8 + UINT64_C(0)},
  {"MauSelectorMatchCentral", kRmtDebugMauSelectorMatchCentral + UINT64_C(0)},
  {"MauSelectorAlu", kRmtDebugMauSelectorAlu + UINT64_C(0)},
  {"MauHashDistrib", kRmtDebugMauHashDistrib + UINT64_C(0)},
  {"MauStatefulAlu", kRmtDebugMauStatefulAlu + UINT64_C(0)},
  {"MauMeter", kRmtDebugMauMeter + UINT64_C(0)},
  {"MauLpfMeter", kRmtDebugMauLpfMeter + UINT64_C(0)},
};

const uint64_t RmtDebug::flags_for_string(std::string name) {
  uint64_t flags = UINT64_C(0);
  if (name.compare(0, 9, "kRmtDebug") == 0) name.erase(0, 9);
  if (name.empty()) return flags;
  try {
    // try exact match first
    flags = flags_map_.at(name);
  } catch (const std::out_of_range&) {
    // pattern match
    std::map<std::string, uint64_t> matches = flags_map(name);
    for (const auto& it : matches) {
      flags |= static_cast<uint64_t>(it.second);
    }
  }
  return flags;
}

const std::map<std::string, uint64_t> RmtDebug::flags_map(std::string name) {
  std::map<std::string, uint64_t> map;

  size_t pos = 0;
  size_t len = name.length();
  bool wildcard = false;
  std::string lower_name = boost::algorithm::to_lower_copy(name);
  if (!name.empty()) {
    if (lower_name.compare(0, 9, "krmtdebug") == 0) pos = 9;
    if (name.compare((len - 1), 1, "*") == 0) wildcard = true;
  }
  for(auto it : flags_map_) {
    std::string lower_key = boost::algorithm::to_lower_copy(it.first);
    if ((wildcard &&
         (lower_name.compare(
             pos, len - 1 - pos, lower_key, 0, len - 1 - pos) == 0)) ||
        (lower_name.compare(pos, len - pos, lower_key) == 0)) {
      map.insert(it);
    }
  }
  return map;
}

const std::map<std::string, int> RmtDebug::log_types_map_ {
  // NB ensure these keys are all lower case
  {"model", RMT_LOG_TYPE_MODEL},
  {"p4", RMT_LOG_TYPE_P4},
  {"tofino", RMT_LOG_TYPE_TOFINO},
  {"packet", RMT_LOG_TYPE_PKT}
};

const int RmtDebug::log_type_for_string(std::string name) {
  try {
    // try exact match first
    return log_types_map_.at(name);
  } catch (const std::out_of_range&) {
    std::string lower_name = boost::algorithm::to_lower_copy(name);
    for(auto it : log_types_map_) {
      if (lower_name.compare(it.first) == 0) {
        return it.second;
      }
    }
  }
  return -1;
}

const std::string RmtDebug::log_string_for_type(int log_type) {
  for(auto & it : log_types_map_) {
    if (it.second == log_type) {
      return it.first;
    }
  }
  return "unknown";
}

}
