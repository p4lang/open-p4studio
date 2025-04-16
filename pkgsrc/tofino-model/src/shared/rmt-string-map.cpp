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
#include <rmt-string-map.h>
#include <rmt-object-manager.h>
#include <chip-features.h>
#include <chip.h>
#include <address.h>
#include <mau-memory.h>
#include <mau-stats-alu.h>
#include <mau-sram-reg.h>
#include <mau-mapram-reg.h>
#include <rmt-sweeper.h>
#include <mau-meter-alu.h>
#include <mau-dependencies.h>
#include <instr.h>
#include <parser.h>
#include <packet.h>
#include <phv-factory.h>
#include <deparser.h>
#include <rmt-packet-coordinator.h>


namespace MODEL_CHIP_NAMESPACE {

  RmtStringMap::RmtStringMap(RmtObjectManager* om)
      : DefaultLogger(om,RmtTypes::kRmtTypeRmtStringMap), om_(om)
  {
    RMT_LOG(RmtDebug::verbose(), "RmtStringMap CREATE\n");
    init();
  }
  RmtStringMap::~RmtStringMap() {
    RMT_LOG(RmtDebug::verbose(), "RmtStringMap DELETE\n");
  }

int RmtStringMap::lookup(const char *key, const char *value) {
    try {
      return string_map_.at(std::string(key))(key,value);
    } catch (const std::exception&) {
      RMT_LOG(RmtDebug::verbose(), "Unable to map string \'%s\'\n", key);
      RMT_ASSERT(0);
    }
  }

std::vector<std::string> RmtStringMap::get_all_keys() {
  std::vector<std::string> keys;
  keys.reserve(string_map_.size());

  for(auto kv : string_map_) {
    keys.push_back(kv.first);
  }
  return keys;
}


void RmtStringMap::clear() {
  string_map_.clear();
}

int RmtStringMap::set_bool(bool& b, std::string value) {
  b = parse_bool_value( value );
  return  b ? 1 : 0;
}
bool RmtStringMap::parse_bool_value(std::string value) {
  if (value == "true")  return true;
  if (value == "false") return false;
  throw std::runtime_error("set_bool expected true|false, got " + value);
}
int RmtStringMap::set_uint64(uint64_t& v64, std::string value) {
  try {
    v64 = static_cast<uint64_t>(std::stol(value, nullptr, 0));
  } catch (const std::exception&) {
    throw std::runtime_error("set_uint64 expected uint64_t, got " + value);
  }
  return (v64 > UINT64_C(0));
}

void RmtStringMap::init() {
  using s_t = const std::string;
  string_map_ = {
    { "AbortOnError",                                  [](s_t& k, s_t& v){ GLOBAL_ABORT_ON_ERROR = parse_bool_value(v) ?1 :0; return GLOBAL_ABORT_ON_ERROR;  } },
    { "ThrowOnError",                                  [](s_t& k, s_t& v){ GLOBAL_THROW_ON_ERROR = parse_bool_value(v) ?1 :0; return GLOBAL_THROW_ON_ERROR;  } },
    { "ThrowOnAssert",                                 [](s_t& k, s_t& v){ GLOBAL_THROW_ON_ASSERT = parse_bool_value(v) ?1 :0; return GLOBAL_THROW_ON_ASSERT;  } },
    { "MauUseMutex",                                   [](s_t& k, s_t& v){ return set_bool(Mau::kMauUseMutex,v); } },
    { "MauDinPowerMode",                               [](s_t& k, s_t& v){ return set_bool(Mau::kMauDinPowerMode,v); } },
    { "ResetUnusedLookupResults",                      [](s_t& k, s_t& v){ return set_bool(Mau::kResetUnusedLookupResults,v); } },
    { "LookupUnusedLtcams",                            [](s_t& k, s_t& v){ return set_bool(Mau::kLookupUnusedLtcams,v); } },
    { "SetNextTablePred",                              [](s_t& k, s_t& v){ return set_bool(Mau::kSetNextTablePred,v); } },
    { "RelaxPrevStageCheck",                           [](s_t& k, s_t& v){ return set_bool(Mau::kRelaxPrevStageCheck,v); } },
    { "ChipUseMutex",                                  [](s_t& k, s_t& v){ return set_bool(Chip::kChipUseMutex,v); } },
    { "InitOnVeryFirstAccess",                         [](s_t& k, s_t& v){ return set_bool(Chip::kInitOnVeryFirstAccess,v); } },
    { "UseGlobalTimeIfZero",                           [](s_t& k, s_t& v){ return set_bool(Chip::kUseGlobalTimeIfZero,v); } },
    { "AllowBadAddrTypeRead",                          [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadAddrTypeRead,v); } },
    { "AllowBadAddrTypeWrite",                         [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadAddrTypeWrite,v); } },
    { "AllowBadMemTypePhysRead",                       [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadMemTypePhysRead,v); } },
    { "AllowBadMemTypePhysWrite",                      [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadMemTypePhysWrite,v); } },
    { "AllowBadMemTypeVirtRead",                       [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadMemTypeVirtRead,v); } },
    { "AllowBadMemTypeVirtWrite",                      [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadMemTypeVirtWrite,v); } },
    { "AllowBadPhysRead",                              [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadPhysRead,v); } },
    { "AllowBadPhysWrite",                             [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadPhysWrite,v); } },
    { "AllowBadVirtRead",                              [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadVirtRead,v); } },
    { "AllowBadVirtWrite",                             [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadVirtWrite,v); } },
    { "AllowBadSramRead",                              [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadSramRead,v); } },
    { "AllowBadSramWrite",                             [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadSramWrite,v); } },
    { "AllowBadTcamRead",                              [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadTcamRead,v); } },
    { "AllowBadTcamWrite",                             [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadTcamWrite,v); } },
    { "AllowBadMapramRead",                            [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadMapramRead,v); } },
    { "AllowBadMapramWrite",                           [](s_t& k, s_t& v){ return set_bool(MauMemory::kAllowBadMapramWrite,v); } },
    { "KeepFullResStats",                              [](s_t& k, s_t& v){ return set_bool(MauMemory::kKeepFullResStats,v); } },
    { "AccessFullResStats",                            [](s_t& k, s_t& v){ return set_bool(MauMemory::kAccessFullResStats,v); } },
    { "StatsVAddrPbusReadBubbleEmulate",               [](s_t& k, s_t& v){ return set_bool(MauMemory::kStatsVAddrPbusReadBubbleEmulate,v); } },
    { "StatsVAddrPbusWriteBubbleEmulate",              [](s_t& k, s_t& v){ return set_bool(MauMemory::kStatsVAddrPbusWriteBubbleEmulate,v); } },
    { "StatsVAddrSweepBubbleEmulate",                  [](s_t& k, s_t& v){ return set_bool(MauMemory::kStatsVAddrSweepBubbleEmulate,v); } },
    { "StatsVAddrDumpWordBubbleEmulate",               [](s_t& k, s_t& v){ return set_bool(MauMemory::kStatsVAddrDumpWordBubbleEmulate,v); } },
    { "MeterVAddrPbusReadBubbleEmulate",               [](s_t& k, s_t& v){ return set_bool(MauMemory::kMeterVAddrPbusReadBubbleEmulate,v); } },
    { "MeterVAddrPbusWriteBubbleEmulate",              [](s_t& k, s_t& v){ return set_bool(MauMemory::kMeterVAddrPbusWriteBubbleEmulate,v); } },
    { "MeterVAddrSweepBubbleEmulate",                  [](s_t& k, s_t& v){ return set_bool(MauMemory::kMeterVAddrSweepBubbleEmulate,v); } },
    { "IdletimeVAddrPbusReadBubbleEmulate",            [](s_t& k, s_t& v){ return set_bool(MauMemory::kIdletimeVAddrPbusReadBubbleEmulate,v); } },
    { "IdletimeVAddrPbusWriteBubbleEmulate",           [](s_t& k, s_t& v){ return set_bool(MauMemory::kIdletimeVAddrPbusWriteBubbleEmulate,v); } },
    { "IdletimeVAddrSweepBubbleEmulate",               [](s_t& k, s_t& v){ return set_bool(MauMemory::kIdletimeVAddrSweepBubbleEmulate,v); } },
    { "IdletimeVAddrDumpBubbleEmulate",                [](s_t& k, s_t& v){ return set_bool(MauMemory::kIdletimeVAddrDumpBubbleEmulate,v); } },
    { "IdletimeVAddrDumpWordBubbleEmulate",            [](s_t& k, s_t& v){ return set_bool(MauMemory::kIdletimeVAddrDumpWordBubbleEmulate,v); } },
    { "SelectorStatefulVAddrPbusReadBubbleEmulate",    [](s_t& k, s_t& v){ return set_bool(MauMemory::kSelectorStatefulVAddrPbusReadBubbleEmulate,v); } },
    { "SelectorStatefulVAddrPbusWriteBubbleEmulate",   [](s_t& k, s_t& v){ return set_bool(MauMemory::kSelectorStatefulVAddrPbusWriteBubbleEmulate,v); } },
    { "SelectorStatefulVAddrSweepBubbleEmulate",       [](s_t& k, s_t& v){ return set_bool(MauMemory::kSelectorStatefulVAddrSweepBubbleEmulate,v); } },
    { "RelaxVirtWriteDataCheck",                       [](s_t& k, s_t& v){ return set_bool(MauMemory::kRelaxVirtWriteDataCheck,v); } },
    { "ZeroisePushPopData",                            [](s_t& k, s_t& v){ return set_bool(MauOpHandlerCommon::kZeroisePushPopData,v); } },
    { "AllowAtomicWideBubbles",                        [](s_t& k, s_t& v){ return set_bool(MauOpHandlerCommon::kAllowAtomicWideBubbles,v); } },
    { "AllowMoveregsCommitOnTcamHit",                  [](s_t& k, s_t& v){ return set_bool(MauMoveregs::kAllowMoveregsCommitOnTcamHit,v); } },
    { "GlobalAddrEnable",                              [](s_t& k, s_t& v){ return set_bool(Address::kGlobalAddrEnable,v); } },
    { "RelaxLookupShiftPfePosCheck",                   [](s_t& k, s_t& v){ return set_bool(MauLookupResult::kRelaxLookupShiftPfePosCheck,v); } },
    { "RelaxLookupShiftOpPosCheck",                    [](s_t& k, s_t& v){ return set_bool(MauLookupResult::kRelaxLookupShiftOpPosCheck,v); } },
    { "RelaxPayloadShifterEnabledCheck",               [](s_t& k, s_t& v){ return set_bool(MauLookupResult::kRelaxPayloadShifterEnabledCheck,v); } },
    { "RelaxHashSelectorLenCheck",                     [](s_t& k, s_t& v){ return set_bool(MauHashDistribution::kRelaxHashSelectorLenCheck,v); } },
    { "RelaxHashSelectorShiftCheck",                   [](s_t& k, s_t& v){ return set_bool(MauHashDistribution::kRelaxHashSelectorShiftCheck,v); } },
    { "RelaxPredicationCheck",                         [](s_t& k, s_t& v){ return set_bool(MauPredicationCommon::kRelaxPredicationCheck,v); } },
    { "AllowUnpoweredTablesToBecomeActive",            [](s_t& k, s_t& v){ return set_bool(MauPredicationCommon::kAllowUnpoweredTablesToBecomeActive,v); } },
    { "PowerTablesForInactiveThreads",                 [](s_t& k, s_t& v){ return set_bool(MauPredicationCommon::kPowerTablesForInactiveThreads,v); } },
    { "KeepFullResStats",                              [](s_t& k, s_t& v){ return set_bool(MauStatsAlu::kKeepFullResStats,v); } },
    { "CapStats",                                      [](s_t& k, s_t& v){ return set_bool(MauStatsAlu::kCapStats,v); } },
    { "RelaxSramVpnCheck",                             [](s_t& k, s_t& v){ return set_bool(MauSram::kRelaxSramVpnCheck,v); } },
    { "RelaxSramEmptyCheck",                           [](s_t& k, s_t& v){ return set_bool(MauSram::kRelaxSramEmptyCheck,v); } },
    { "MapramWriteData1HasTime",                       [](s_t& k, s_t& v){ return set_bool(MauMapram::kMapramWriteData1HasTime,v); } },
    { "MapramColorWriteLatencyTeop",                   [](s_t& k, s_t& v){ return set_uint64(MauMapram::kMapramColorWriteLatencyTEOP,v); } },
    { "RelaxSramIngressCheck",                         [](s_t& k, s_t& v){ return set_bool(MauSramReg::kRelaxSramIngressCheck,v); } },
    { "RelaxSramBitposCheck",                          [](s_t& k, s_t& v){ return set_bool(MauSramReg::kRelaxSramBitposCheck,v); } },
    { "RelaxMapramIngressCheck",                       [](s_t& k, s_t& v){ return set_bool(MauMapramReg::kRelaxMapramIngressCheck,v); } },
    { "RelaxRowSelectorShiftCheck",                    [](s_t& k, s_t& v){ return set_bool(MauLogicalRow::kRelaxRowSelectorShiftCheck,v); } },
    { "RelaxDataMultiWriteCheck",                      [](s_t& k, s_t& v){ return set_bool(MauLogicalRow::kRelaxDataMultiWriteCheck,v); } },
    { "RelaxAddrMultiWriteCheck",                      [](s_t& k, s_t& v){ return set_bool(MauLogicalRow::kRelaxAddrMultiWriteCheck,v); } },
    { "RelaxMultiHitCheck",                            [](s_t& k, s_t& v){ return set_bool(MauSramColumn::kRelaxMultiHitCheck,v); } },
    { "RelaxMultiColumnHitCheck",                      [](s_t& k, s_t& v){ return set_bool(MauSramColumn::kRelaxMultiColumnHitCheck,v); } },
    { "RelaxMatchBusMultiWriteCheck",                  [](s_t& k, s_t& v){ return set_bool(MauSramRow::kRelaxMatchBusMultiWriteCheck,v); } },
    { "RelaxTindBusMultiWriteCheck",                   [](s_t& k, s_t& v){ return set_bool(MauSramRow::kRelaxTindBusMultiWriteCheck,v); } },
    { "MemoryCoreSplit",                               [](s_t& k, s_t& v){ return set_bool(MauSramRowReg::kMemoryCoreSplit,v); } },
    { "RelaxOfloWrMuxCheck",                           [](s_t& k, s_t& v){ return set_bool(MauSramRowReg::kRelaxOfloWrMuxCheck,v); } },
    { "RelaxOfloRdMuxCheck",                           [](s_t& k, s_t& v){ return set_bool(MauSramRowReg::kRelaxOfloRdMuxCheck,v); } },
    { "RelaxSynth2PortFabricCheck",                    [](s_t& k, s_t& v){ return set_bool(MauSramRowReg::kRelaxSynth2PortFabricCheck,v); } },
    { "RelaxColorBusDiffAluCheck",                     [](s_t& k, s_t& v){ return set_bool(MauColorSwitchbox::kRelaxColorBusDiffAluCheck,v); } },
    { "RelaxGroupEnableChecks",                        [](s_t& k, s_t& v){ return set_bool(MauHashDistributionRegs::kRelaxGroupEnableChecks,v); } },
    { "AllowSynchronousIdleDump",                      [](s_t& k, s_t& v){ return set_bool(TableInfo::kAllowSynchronousIdleDump,v); } },
    { "AllowSynchronousIdleDumpWord",                  [](s_t& k, s_t& v){ return set_bool(TableInfo::kAllowSynchronousIdleDumpWord,v); } },
    { "AllowSynchronousStatsDump",                     [](s_t& k, s_t& v){ return set_bool(TableInfo::kAllowSynchronousStatsDump,v); } },
    { "AllowSynchronousStatsDumpWord",                 [](s_t& k, s_t& v){ return set_bool(TableInfo::kAllowSynchronousStatsDumpWord,v); } },
    { "DisableIdleSweepOnTableIdle",                   [](s_t& k, s_t& v){ return set_bool(TableInfo::kDisableIdleSweepOnTableIdle,v); } },
    { "SynchronousIdleOps",                            [](s_t& k, s_t& v){ return set_bool(TableInfo::kSynchronousIdleOps,v); } },
    { "SynchronousStatsOps",                           [](s_t& k, s_t& v){ return set_bool(TableInfo::kSynchronousStatsOps,v); } },
    { "RelaxPrePfeAddrCheck",                          [](s_t& k, s_t& v){ return set_bool(MauAddrDist::kRelaxPrePfeAddrCheck,v); } },
    { "MeterSweepOnDemand",                            [](s_t& k, s_t& v){ return set_bool(MauAddrDist::kMeterSweepOnDemand,v); } },
    { "RelaxPacketActionAtHdrCheck",                   [](s_t& k, s_t& v){ return set_bool(MauAddrDist::kRelaxPacketActionAtHdrCheck,v); } },
    { "RelaxAllAddrsConsumedCheck",                    [](s_t& k, s_t& v){ return set_bool(MauAddrDist::kRelaxAllAddrsConsumedCheck,v); } },
    { "RelaxActionAddrsConsumedCheck",                 [](s_t& k, s_t& v){ return set_bool(MauAddrDist::kRelaxActionAddrsConsumedCheck,v); } },
    { "RelaxStatsAddrsConsumedCheck",                  [](s_t& k, s_t& v){ return set_bool(MauAddrDist::kRelaxStatsAddrsConsumedCheck,v); } },
    { "RelaxMeterAddrsConsumedCheck",                  [](s_t& k, s_t& v){ return set_bool(MauAddrDist::kRelaxMeterAddrsConsumedCheck,v); } },
    { "RelaxIdletimeAddrsConsumedCheck",               [](s_t& k, s_t& v){ return set_bool(MauAddrDist::kRelaxIdletimeAddrsConsumedCheck,v); } },
    { "RelaxSwPushPopInvalidErrors",                   [](s_t& k, s_t& v){ return set_bool(MauStatefulCounters::kRelaxSwPushPopInvalidErrors,v); } },
    { "RelaxSwPushPopOverflowUnderflowErrors",         [](s_t& k, s_t& v){ return set_bool(MauStatefulCounters::kRelaxSwPushPopOverflowUnderflowErrors,v); } },
    { "SynchronousStatefulCounterClear",               [](s_t& k, s_t& v){ return set_bool(MauStatefulCounters::kSynchronousStatefulCounterClear,v); } },
    { "StatefulCounterTickCheckTime",                  [](s_t& k, s_t& v){ return set_bool(MauStatefulCounters::kStatefulCounterTickCheckTime,v); } },
    { "RelaxDependencyCheck",                          [](s_t& k, s_t& v){ return set_bool(MauDependencies::kRelaxDependencyCheck,v); } },
    { "RelaxDelayCheck",                               [](s_t& k, s_t& v){ return set_bool(MauDependencies::kRelaxDelayCheck,v); } },
    { "RelaxReplicationCheck",                         [](s_t& k, s_t& v){ return set_bool(MauDependencies::kRelaxReplicationCheck,v); } },
    { "RelaxThreadCheck",                              [](s_t& k, s_t& v){ return set_bool(MauDependencies::kRelaxThreadCheck,v); } },
    { "RelaxMeterAluThreadCheck",                      [](s_t& k, s_t& v){ return set_bool(MauMeterAlu::kRelaxThreadCheck,v); } },
    { "RelaxMeterAluOpCheck",                          [](s_t& k, s_t& v){ return set_bool(MauMeterAlu::kRelaxOpCheck,v); } },
    { "RelaxMeterExponentCheck",                       [](s_t& k, s_t& v){ return set_bool(MauMeter::kRelaxExponentCheck,v); } },
    { "RelaxGatewayReplicationCheck",                  [](s_t& k, s_t& v){ return set_bool(MauGatewayPayload::kRelaxGatewayReplicationCheck,v); } },
    { "RelaxExtractionCheck",                          [](s_t& k, s_t& v){ return set_bool(ParserShared::kRelaxExtractionCheck,v); } },
    { "RelaxPreExtractionCheck",                       [](s_t& k, s_t& v){ return set_bool(ParserShared::kRelaxPreExtractionCheck,v); } },
    { "RelaxFinalParseCheck",                          [](s_t& k, s_t& v){ return set_bool(ParserShared::kRelaxFinalParseCheck,v); } },
    { "RelaxClotOverlapCheck",                         [](s_t& k, s_t& v){ return set_bool(Clot::kRelaxOverlapCheck,v); } },
    { "AllowAdjacentClot",                             [](s_t& k, s_t& v){ return set_bool(Clot::kAllowAdjacent,v); } },
    { "AllowDuplicateClot",                            [](s_t& k, s_t& v){ return set_bool(Clot::kAllowDuplicate,v); } },
    { "RelaxPairedLtcamErrors",                        [](s_t& k, s_t& v){ return set_bool(MauLogicalTable::kRelaxPairedLtcamErrors,v); } },
    { "RelaxHdrtimeMeterAddrNopCheck",                 [](s_t& k, s_t& v){ return set_bool(MauLogicalTable::kRelaxHdrtimeMeterAddrNopCheck,v); } },
    { "RelaxHdrtimeMeterAddrColorCheck",               [](s_t& k, s_t& v){ return set_bool(MauLogicalTable::kRelaxHdrtimeMeterAddrColorCheck,v); } },
    { "RelaxTcamCheck",                                [](s_t& k, s_t& v){ return set_bool(MauLogicalTcam::kRelaxTcamCheck,v); } },
    { "RelaxThreadReplicationCheck",                   [](s_t& k, s_t& v){ return set_bool(MauInstrStoreCommon::kRelaxThreadReplicationCheck,v); } },
    { "RelaxAddrFormatReplicationCheck",               [](s_t& k, s_t& v){ return set_bool(MauInstrStoreCommon::kRelaxAddrFormatReplicationCheck,v); } },
    { "RelaxThreadOverlapCheck",                       [](s_t& k, s_t& v){ return set_bool(MauInstrStoreCommon::kRelaxThreadOverlapCheck,v); } },
    { "RelaxInstrOverlapCheck",                        [](s_t& k, s_t& v){ return set_bool(MauInstrStoreCommon::kRelaxInstrOverlapCheck,v); } },
    { "RelaxInstrFormatCheck",                         [](s_t& k, s_t& v){ return set_bool(Instr::kRelaxInstrFormatCheck,v); } },
    { "RelaxInstrMatchingGressCheck",                  [](s_t& k, s_t& v){ return set_bool(Instr::kRelaxInstrMatchingGressCheck,v); } },
    { "RelaxEqNeqConfigCheck",                         [](s_t& k, s_t& v){ return set_bool(Instr::kRelaxEqNeqConfigCheck,v); } },
    { "InstrCondMoveIsUnconditional",                  [](s_t& k, s_t& v){ return set_bool(Instr::kInstrCondMoveIsUnconditional,v); } },
    { "InstrCondMuxIsCondMove",                        [](s_t& k, s_t& v){ return set_bool(Instr::kInstrCondMuxIsCondMove,v); } },
    { "InstrInvalidateIsNop",                          [](s_t& k, s_t& v){ return set_bool(Instr::kInstrInvalidateIsNop,v); } },
    { "InstrInvalidateIsErr",                          [](s_t& k, s_t& v){ return set_bool(Instr::kInstrInvalidateIsErr,v); } },
    { "InstrPairDpfIsErr",                             [](s_t& k, s_t& v){ return set_bool(Instr::kInstrPairDpfIsErr,v); } },
    { "InstrDataDepShiftSupported",                    [](s_t& k, s_t& v){ return set_bool(Instr::kInstrDataDepShiftSupported,v); } },
    { "InstrReverseSubtractSupported",                 [](s_t& k, s_t& v){ return set_bool(Instr::kInstrReverseSubtractSupported,v); } },
    { "PktInitTimeRandOnAlloc",                        [](s_t& k, s_t& v){ return set_bool(Packet::kPktInitTimeRandOnAlloc,v); } },
    { "PhvInitTimeRandOnAlloc",                        [](s_t& k, s_t& v){ return set_bool(PhvFactory::kPhvInitTimeRandOnAlloc,v); } },
    { "PhvInitAllValid",                               [](s_t& k, s_t& v){ return set_bool(PhvFactory::kPhvInitAllValid,v); } },
    { "SnapshotCompareValidBit",                       [](s_t& k, s_t& v){ return set_bool(MauSnapshotCommon::kSnapshotCompareValidBit,v); } },
    { "SnapshotCaptureValidBit",                       [](s_t& k, s_t& v){ return set_bool(MauSnapshotCommon::kSnapshotCaptureValidBit,v); } },
    { "SnapshotUsePhvTime",                            [](s_t& k, s_t& v){ return set_bool(MauSnapshotCommon::kSnapshotUsePhvTime,v); } },
    { "SnapshotMaskThreadFields",                      [](s_t& k, s_t& v){ return set_bool(MauSnapshotCommon::kSnapshotMaskThreadFields,v); } },
    { "BugActionDataBusMaskBeforeDup",                 [](s_t& k, s_t& v){ return set_bool(ActionOutputHvXbar::kBugActionDataBusMaskBeforeDup,v); } },
    { "RelaxSaluPredRsiCheck",                         [](s_t& k, s_t& v){ return set_bool(MauStatefulAlu::kRelaxSaluPredRsiCheck,v); } },
    { "MaxReturnsHighestIndex",                        [](s_t& k, s_t& v){ return set_bool(MauChipStatefulAlu::kMaxReturnsHighestIndex,v); } },
    { "MinReturnsHighestIndex",                        [](s_t& k, s_t& v){ return set_bool(MauChipStatefulAlu::kMinReturnsHighestIndex,v); } },
    { "RelaxDeparserChunkChecks",                      [](s_t& k, s_t& v){ return set_bool(Deparser::kRelaxDeparserChunkChecks,v); } },
    { "RelaxDeparserByteChecks",                       [](s_t& k, s_t& v){ return set_bool(Deparser::kRelaxDeparserByteChecks,v); } },
    { "RelaxDeparserGroupChecks",                      [](s_t& k, s_t& v){ return set_bool(Deparser::kRelaxDeparserGroupChecks,v); } },
    { "RelaxDeparserFdeChecks",                        [](s_t& k, s_t& v){ return set_bool(Deparser::kRelaxDeparserFdeChecks,v); } },
    { "RelaxDeparserClotChecks",                       [](s_t& k, s_t& v){ return set_bool(Deparser::kRelaxDeparserClotChecks,v); } },
    { "RelaxDeparserClotLenChecks",                    [](s_t& k, s_t& v){ return set_bool(Deparser::kRelaxDeparserClotLenChecks,v); } },
    { "ProcessGressesSeparately",                      [](s_t& k, s_t& v){ return set_bool(RmtPacketCoordinator::kProcessGressesSeparately,v); } },
    { "ParserPhvFillOtherGress",                       [](s_t& k, s_t& v){ return set_bool(ParserShared::kPhvFillOtherGress,v); } },

    //{ "",                                            [](s_t& k, s_t& v){ return set_bool(,v); } },

    // DV use MAU0 as if it is just like any other MAU, but everyone else needs MAU0 to do the special things that MAU0 does. Allow DV to switch on special features too
    { "SetMau0IsSpecial",                              [](s_t& k, s_t& v){ Mau::kMauFeatures[0] = parse_bool_value(v) ?MAU_FEATURES_0_SPECIAL :0; return Mau::kMauFeatures[0];  } },

    // example of using [this] to access the object manager
    { "SetChipFreeAllOnExit",                          [this](s_t& k, s_t& v){ om_->set_chip_free_all_on_exit( parse_bool_value(v) ); return 0; } },
    { "SetLogPrefix",                                  [this](s_t& k, s_t& v){ om_->set_log_prefix(v.c_str()); return 0; } },

    { "ZERO", [](s_t& k, s_t& v){return 0;} }
  };
}



}
