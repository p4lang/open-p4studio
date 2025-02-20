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

#include <utests/test_config.h>
#include <mau.h>
#include <chip.h>
#include <address.h>
#include <mau-memory.h>
#include <mau-lookup-result.h>
#include <mau-stats-alu.h>
#include <mau-sram.h>
#include <mau-sram-reg.h>
#include <mau-mapram-reg.h>
#include <mau-logical-row.h>
#include <mau-sram-row.h>
#include <mau-sram-row-reg.h>
#include <rmt-sweeper.h>
#include <mau-addr-dist.h>
#include <mau-dependencies.h>
#include <mau-op-handler.h>
#include <mau-instr-store-common.h>
#include <mau-snapshot-common.h>
#include <action-output-hv-xbar.h>
#include <instr.h>
#include <queueing.h>
#include <parser.h>
#include <packet.h>
#include <phv-factory.h>

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

TestConfig::TestConfig() {
  static_assert( (RmtDefs::kStagesMax <= kAbsMaxStages),
                 "kStagesMax exceeds TestConfig storage space" );

  // Stash all config vals
  GLOBAL_AbortOnError = GLOBAL_ABORT_ON_ERROR;
  GLOBAL_ThrowOnError = GLOBAL_THROW_ON_ERROR;
  GLOBAL_ThrowOnAssert = GLOBAL_THROW_ON_ASSERT;

  Model_kAllowCb50 = model_core::Model::kAllowCb50;

  Mau_kMauUseMutex = Mau::kMauUseMutex;
  Mau_kMauDinPowerMode = Mau::kMauDinPowerMode;
  Mau_kResetUnusedLookupResults = Mau::kResetUnusedLookupResults;
  Mau_kLookupUnusedLtcams = Mau::kLookupUnusedLtcams;
  Mau_kSetNextTablePred = Mau::kSetNextTablePred;
  Mau_kRelaxPrevStageCheck = Mau::kRelaxPrevStageCheck;
  for (int i = 0; i < RmtDefs::kStagesMax; i++) Mau_kMauFeatures[i] = Mau::kMauFeatures[i];

  Chip_kChipUseMutex = Chip::kChipUseMutex;
  Chip_kInitOnVeryFirstAccess = Chip::kInitOnVeryFirstAccess;
  Chip_kUseGlobalTimeIfZero = Chip::kUseGlobalTimeIfZero;

  MauMemory_kAllowBadAddrTypeRead = MauMemory::kAllowBadAddrTypeRead;
  MauMemory_kAllowBadAddrTypeWrite = MauMemory::kAllowBadAddrTypeWrite;
  MauMemory_kAllowBadMemTypePhysRead = MauMemory::kAllowBadMemTypePhysRead;
  MauMemory_kAllowBadMemTypePhysWrite = MauMemory::kAllowBadMemTypePhysWrite;
  MauMemory_kAllowBadMemTypeVirtRead = MauMemory::kAllowBadMemTypeVirtRead;
  MauMemory_kAllowBadMemTypeVirtWrite = MauMemory::kAllowBadMemTypeVirtWrite;
  MauMemory_kAllowBadPhysRead = MauMemory::kAllowBadPhysRead;
  MauMemory_kAllowBadPhysWrite = MauMemory::kAllowBadPhysWrite;
  MauMemory_kAllowBadVirtRead = MauMemory::kAllowBadVirtRead;
  MauMemory_kAllowBadVirtWrite = MauMemory::kAllowBadVirtWrite;
  MauMemory_kAllowBadSramRead = MauMemory::kAllowBadSramRead;
  MauMemory_kAllowBadSramWrite = MauMemory::kAllowBadSramWrite;
  MauMemory_kAllowBadTcamRead = MauMemory::kAllowBadTcamRead;
  MauMemory_kAllowBadTcamWrite = MauMemory::kAllowBadTcamWrite;
  MauMemory_kAllowBadMapramRead = MauMemory::kAllowBadMapramRead;
  MauMemory_kAllowBadMapramWrite = MauMemory::kAllowBadMapramWrite;
  MauMemory_kKeepFullResStats = MauMemory::kKeepFullResStats;
  MauMemory_kAccessFullResStats = MauMemory::kAccessFullResStats;

  MauMemory_kStatsFullVAddrPbusShift = MauMemory::kStatsFullVAddrPbusShift;
  MauMemory_kStatsVAddrPbusShift = MauMemory::kStatsVAddrPbusShift;
  MauMemory_kMeterVAddrPbusShift = MauMemory::kMeterVAddrPbusShift;
  MauMemory_kSelectorStatefulVAddrPbusShift = MauMemory::kSelectorStatefulVAddrPbusShift;
  MauMemory_kIdletimeVAddrPbusShift = MauMemory::kIdletimeVAddrPbusShift;

  MauMemory_kStatsVAddrPbusReadBubbleEmulate = MauMemory::kStatsVAddrPbusReadBubbleEmulate;
  MauMemory_kStatsVAddrPbusWriteBubbleEmulate = MauMemory::kStatsVAddrPbusWriteBubbleEmulate;
  MauMemory_kStatsVAddrSweepBubbleEmulate = MauMemory::kStatsVAddrSweepBubbleEmulate;
  MauMemory_kStatsVAddrDumpWordBubbleEmulate = MauMemory::kStatsVAddrDumpWordBubbleEmulate;

  MauMemory_kMeterVAddrPbusReadBubbleEmulate = MauMemory::kMeterVAddrPbusReadBubbleEmulate;
  MauMemory_kMeterVAddrPbusWriteBubbleEmulate = MauMemory::kMeterVAddrPbusWriteBubbleEmulate;
  MauMemory_kMeterVAddrSweepBubbleEmulate = MauMemory::kMeterVAddrSweepBubbleEmulate;

  MauMemory_kIdletimeVAddrPbusReadBubbleEmulate = MauMemory::kIdletimeVAddrPbusReadBubbleEmulate;
  MauMemory_kIdletimeVAddrPbusWriteBubbleEmulate = MauMemory::kIdletimeVAddrPbusWriteBubbleEmulate;
  MauMemory_kIdletimeVAddrSweepBubbleEmulate = MauMemory::kIdletimeVAddrSweepBubbleEmulate;
  MauMemory_kIdletimeVAddrDumpBubbleEmulate = MauMemory::kIdletimeVAddrDumpBubbleEmulate;
  MauMemory_kIdletimeVAddrDumpWordBubbleEmulate = MauMemory::kIdletimeVAddrDumpWordBubbleEmulate;

  MauMemory_kSelectorStatefulVAddrPbusReadBubbleEmulate = MauMemory::kSelectorStatefulVAddrPbusReadBubbleEmulate;
  MauMemory_kSelectorStatefulVAddrPbusWriteBubbleEmulate = MauMemory::kSelectorStatefulVAddrPbusWriteBubbleEmulate;
  MauMemory_kSelectorStatefulVAddrSweepBubbleEmulate = MauMemory::kSelectorStatefulVAddrSweepBubbleEmulate;

  MauMemory_kRelaxVirtWriteDataCheck = MauMemory::kRelaxVirtWriteDataCheck;

  MauOpHandlerCommon_kZeroisePushPopData = MauOpHandlerCommon::kZeroisePushPopData;
  MauOpHandlerCommon_kAllowAtomicWideBubbles = MauOpHandlerCommon::kAllowAtomicWideBubbles;

  MauMoveregs_kAllowMoveregsCommitOnTcamHit = MauMoveregs::kAllowMoveregsCommitOnTcamHit;

  Address_kStatsAddrFixShift = Address::kStatsAddrFixShift;
  Address_kStatsAddrSramShift = Address::kStatsAddrSramShift;

  Address_kGlobalAddrEnable = Address::kGlobalAddrEnable;

  MauLookupResult_kRelaxLookupShiftPfePosCheck = MauLookupResult::kRelaxLookupShiftPfePosCheck;
  MauLookupResult_kRelaxLookupShiftOpPosCheck = MauLookupResult::kRelaxLookupShiftOpPosCheck;
  MauLookupResult_kRelaxPayloadShifterEnabledCheck = MauLookupResult::kRelaxPayloadShifterEnabledCheck;

  MauHashDistribution_kRelaxHashSelectorLenCheck = MauHashDistribution::kRelaxHashSelectorLenCheck;
  MauHashDistribution_kRelaxHashSelectorShiftCheck = MauHashDistribution::kRelaxHashSelectorShiftCheck;

  MauPredicationCommon_kRelaxPredicationCheck = MauPredicationCommon::kRelaxPredicationCheck;
  MauPredicationCommon_kAllowUnpoweredTablesToBecomeActive = MauPredicationCommon::kAllowUnpoweredTablesToBecomeActive;
  MauPredicationCommon_kPowerTablesForInactiveThreads = MauPredicationCommon::kPowerTablesForInactiveThreads;

  MauStatsAlu_kKeepFullResStats = MauStatsAlu::kKeepFullResStats;
  MauStatsAlu_kCapStats = MauStatsAlu::kCapStats;

  MauSram_kRelaxSramVpnCheck = MauSram::kRelaxSramVpnCheck;
  MauSram_kRelaxSramEmptyCheck = MauSram::kRelaxSramEmptyCheck;

  MauMapram_kMapramWriteData1HasTime = MauMapram::kMapramWriteData1HasTime;

  MauSramReg_kRelaxSramIngressCheck = MauSramReg::kRelaxSramIngressCheck;
  MauSramReg_kRelaxSramBitposCheck = MauSramReg::kRelaxSramBitposCheck;

  MauMapramReg_kRelaxMapramIngressCheck = MauMapramReg::kRelaxMapramIngressCheck;

  MauLogicalRow_kRelaxRowSelectorShiftCheck = MauLogicalRow::kRelaxRowSelectorShiftCheck;
  MauLogicalRow_kRelaxDataMultiWriteCheck = MauLogicalRow::kRelaxDataMultiWriteCheck;
  MauLogicalRow_kRelaxAddrMultiWriteCheck = MauLogicalRow::kRelaxAddrMultiWriteCheck;

  MauSramColumn_kRelaxMultiHitCheck = MauSramColumn::kRelaxMultiHitCheck;
  MauSramColumn_kRelaxMultiColumnHitCheck = MauSramColumn::kRelaxMultiColumnHitCheck;

  MauSramRow_kRelaxMatchBusMultiWriteCheck = MauSramRow::kRelaxMatchBusMultiWriteCheck;
  MauSramRow_kRelaxTindBusMultiWriteCheck = MauSramRow::kRelaxTindBusMultiWriteCheck;

  MauSramRowReg_kMemoryCoreSplit = MauSramRowReg::kMemoryCoreSplit;
  MauSramRowReg_kRelaxOfloWrMuxCheck = MauSramRowReg::kRelaxOfloWrMuxCheck;
  MauSramRowReg_kRelaxOfloRdMuxCheck = MauSramRowReg::kRelaxOfloRdMuxCheck;
  MauSramRowReg_kRelaxSynth2PortFabricCheck = MauSramRowReg::kRelaxSynth2PortFabricCheck;

  MauColorSwitchbox_kRelaxColorBusDiffAluCheck = MauColorSwitchbox::kRelaxColorBusDiffAluCheck;

  MauHashDistributionRegs_kRelaxGroupEnableChecks =  MauHashDistributionRegs::kRelaxGroupEnableChecks;

  TableInfo_kAllowSynchronousIdleDump = TableInfo::kAllowSynchronousIdleDump;
  TableInfo_kAllowSynchronousIdleDumpWord = TableInfo::kAllowSynchronousIdleDumpWord;
  TableInfo_kAllowSynchronousStatsDump = TableInfo::kAllowSynchronousStatsDump;
  TableInfo_kAllowSynchronousStatsDumpWord = TableInfo::kAllowSynchronousStatsDumpWord;
  TableInfo_kDisableIdleSweepOnTableIdle = TableInfo::kDisableIdleSweepOnTableIdle;
  TableInfo_kSynchronousStatsOps = TableInfo::kSynchronousStatsOps;
  TableInfo_kSynchronousIdleOps = TableInfo::kSynchronousIdleOps;

  MauAddrDist_kRelaxPrePfeAddrCheck = MauAddrDist::kRelaxPrePfeAddrCheck;
  MauAddrDist_kMeterSweepOnDemand = MauAddrDist::kMeterSweepOnDemand;
  MauAddrDist_kMeterSweepOnDemandPipe0 = MauAddrDist::kMeterSweepOnDemandPipe0;
  MauAddrDist_kRelaxPacketActionAtHdrCheck = MauAddrDist::kRelaxPacketActionAtHdrCheck;
  MauAddrDist_kRelaxAllAddrsConsumedCheck = MauAddrDist::kRelaxAllAddrsConsumedCheck;
  MauAddrDist_kRelaxActionAddrsConsumedCheck = MauAddrDist::kRelaxActionAddrsConsumedCheck;
  MauAddrDist_kRelaxStatsAddrsConsumedCheck = MauAddrDist::kRelaxStatsAddrsConsumedCheck;
  MauAddrDist_kRelaxMeterAddrsConsumedCheck = MauAddrDist::kRelaxMeterAddrsConsumedCheck;
  MauAddrDist_kRelaxIdletimeAddrsConsumedCheck = MauAddrDist::kRelaxIdletimeAddrsConsumedCheck;

  MauStatefulCounters_kRelaxSwPushPopInvalidErrors = MauStatefulCounters::kRelaxSwPushPopInvalidErrors;
  MauStatefulCounters_kRelaxSwPushPopOverflowUnderflowErrors = MauStatefulCounters::kRelaxSwPushPopOverflowUnderflowErrors;
  MauStatefulCounters_kSynchronousStatefulCounterClear = MauStatefulCounters::kSynchronousStatefulCounterClear;
  MauStatefulCounters_kStatefulCounterTickCheckTime = MauStatefulCounters::kStatefulCounterTickCheckTime;

  MauDependencies_kRelaxDependencyCheck = MauDependencies::kRelaxDependencyCheck;
  MauDependencies_kRelaxDelayCheck = MauDependencies::kRelaxDelayCheck;
  MauDependencies_kRelaxReplicationCheck = MauDependencies::kRelaxReplicationCheck;
  MauDependencies_kRelaxThreadCheck = MauDependencies::kRelaxThreadCheck;

  MauMeterAlu_kRelaxThreadCheck = MauMeterAlu::kRelaxThreadCheck;
  MauMeterAlu_kRelaxOpCheck = MauMeterAlu::kRelaxOpCheck;
  MauMeter_kRelaxExponentCheck = MauMeter::kRelaxExponentCheck;

  MauGatewayPayload_kRelaxGatewayReplicationCheck = MauGatewayPayload::kRelaxGatewayReplicationCheck;

  ParserShared_kRelaxExtractionCheck = ParserShared::kRelaxExtractionCheck;
  ParserShared_kRelaxPreExtractionCheck = ParserShared::kRelaxPreExtractionCheck;
  ParserShared_kRelaxFinalParseCheck = ParserShared::kRelaxFinalParseCheck;
  Clot_kRelaxOverlapCheck = Clot::kRelaxOverlapCheck;
  Clot_kAllowAdjacent = Clot::kAllowAdjacent;
  Clot_kAllowDuplicate = Clot::kAllowDuplicate;

  MauLogicalTable_kRelaxPairedLtcamErrors = MauLogicalTable::kRelaxPairedLtcamErrors;
  MauLogicalTable_kRelaxHdrtimeMeterAddrColorCheck = MauLogicalTable::kRelaxHdrtimeMeterAddrColorCheck;
  MauLogicalTable_kRelaxHdrtimeMeterAddrNopCheck = MauLogicalTable::kRelaxHdrtimeMeterAddrNopCheck;

  MauLogicalTcam_kRelaxTcamCheck = MauLogicalTcam::kRelaxTcamCheck;

  MauInstrStoreCommon_kRelaxThreadReplicationCheck = MauInstrStoreCommon::kRelaxThreadReplicationCheck;
  MauInstrStoreCommon_kRelaxAddrFormatReplicationCheck = MauInstrStoreCommon::kRelaxAddrFormatReplicationCheck;
  MauInstrStoreCommon_kRelaxThreadOverlapCheck = MauInstrStoreCommon::kRelaxThreadOverlapCheck;
  MauInstrStoreCommon_kRelaxInstrOverlapCheck = MauInstrStoreCommon::kRelaxInstrOverlapCheck;

  Instr_kRelaxInstrFormatCheck = Instr::kRelaxInstrFormatCheck;
  Instr_kRelaxInstrMatchingGressCheck = Instr::kRelaxInstrMatchingGressCheck;
  Instr_kRelaxEqNeqConfigCheck = Instr::kRelaxEqNeqConfigCheck;

  Instr_kInstrCondMoveIsUnconditional = Instr::kInstrCondMoveIsUnconditional;
  Instr_kInstrCondMuxIsCondMove = Instr::kInstrCondMuxIsCondMove;
  Instr_kInstrInvalidateIsNop = Instr::kInstrInvalidateIsNop;
  Instr_kInstrInvalidateIsErr = Instr::kInstrInvalidateIsErr;
  Instr_kInstrPairDpfIsErr = Instr::kInstrPairDpfIsErr;
  Instr_kInstrDataDepShiftSupported = Instr::kInstrDataDepShiftSupported;
  Instr_kInstrReverseSubtractSupported = Instr::kInstrReverseSubtractSupported;

  Packet_kPktInitTimeRandOnAlloc = Packet::kPktInitTimeRandOnAlloc;
  PhvFactory_kPhvInitTimeRandOnAlloc = PhvFactory::kPhvInitTimeRandOnAlloc;
  PhvFactory_kPhvInitAllValid = PhvFactory::kPhvInitAllValid;

  MauSnapshotCommon_kSnapshotCompareValidBit = MauSnapshotCommon::kSnapshotCompareValidBit;
  MauSnapshotCommon_kSnapshotCaptureValidBit = MauSnapshotCommon::kSnapshotCaptureValidBit;
  MauSnapshotCommon_kSnapshotUsePhvTime = MauSnapshotCommon::kSnapshotUsePhvTime;
  MauSnapshotCommon_kSnapshotMaskThreadFields = MauSnapshotCommon::kSnapshotMaskThreadFields;

  ActionOutputHvXbar_kBugActionDataBusMaskBeforeDup = ActionOutputHvXbar::kBugActionDataBusMaskBeforeDup;

  MauStatefulAlu_kRelaxSaluPredRsiCheck = MauStatefulAlu::kRelaxSaluPredRsiCheck;

  MauChipStatefulAlu_kMaxReturnsHighestIndex = MauChipStatefulAlu::kMaxReturnsHighestIndex;
  MauChipStatefulAlu_kMinReturnsHighestIndex = MauChipStatefulAlu::kMinReturnsHighestIndex;

  Queueing_kAllowMissingDies = Queueing::kAllowMissingDies;
}


TestConfig::~TestConfig() {
  // Restore all config vals
  GLOBAL_ABORT_ON_ERROR = GLOBAL_AbortOnError;
  GLOBAL_THROW_ON_ERROR = GLOBAL_ThrowOnError;
  GLOBAL_THROW_ON_ASSERT = GLOBAL_ThrowOnAssert;

  model_core::Model::kAllowCb50 = Model_kAllowCb50;

  Mau::kMauUseMutex = Mau_kMauUseMutex;
  Mau::kMauDinPowerMode = Mau_kMauDinPowerMode;
  Mau::kResetUnusedLookupResults = Mau_kResetUnusedLookupResults;
  Mau::kLookupUnusedLtcams = Mau_kLookupUnusedLtcams;
  Mau::kSetNextTablePred = Mau_kSetNextTablePred;
  Mau::kRelaxPrevStageCheck = Mau_kRelaxPrevStageCheck;
  for (int i = 0; i < RmtDefs::kStagesMax; i++) Mau::kMauFeatures[i] = Mau_kMauFeatures[i];

  Chip::kChipUseMutex = Chip_kChipUseMutex;
  Chip::kInitOnVeryFirstAccess = Chip_kInitOnVeryFirstAccess;
  Chip::kUseGlobalTimeIfZero = Chip_kUseGlobalTimeIfZero;

  MauMemory::kAllowBadAddrTypeRead = MauMemory_kAllowBadAddrTypeRead;
  MauMemory::kAllowBadAddrTypeWrite = MauMemory_kAllowBadAddrTypeWrite;
  MauMemory::kAllowBadMemTypePhysRead = MauMemory_kAllowBadMemTypePhysRead;
  MauMemory::kAllowBadMemTypePhysWrite = MauMemory_kAllowBadMemTypePhysWrite;
  MauMemory::kAllowBadMemTypeVirtRead = MauMemory_kAllowBadMemTypeVirtRead;
  MauMemory::kAllowBadMemTypeVirtWrite = MauMemory_kAllowBadMemTypeVirtWrite;
  MauMemory::kAllowBadPhysRead = MauMemory_kAllowBadPhysRead;
  MauMemory::kAllowBadPhysWrite = MauMemory_kAllowBadPhysWrite;
  MauMemory::kAllowBadVirtRead = MauMemory_kAllowBadVirtRead;
  MauMemory::kAllowBadVirtWrite = MauMemory_kAllowBadVirtWrite;
  MauMemory::kAllowBadSramRead = MauMemory_kAllowBadSramRead;
  MauMemory::kAllowBadSramWrite = MauMemory_kAllowBadSramWrite;
  MauMemory::kAllowBadTcamRead = MauMemory_kAllowBadTcamRead;
  MauMemory::kAllowBadTcamWrite = MauMemory_kAllowBadTcamWrite;
  MauMemory::kAllowBadMapramRead = MauMemory_kAllowBadMapramRead;
  MauMemory::kAllowBadMapramWrite = MauMemory_kAllowBadMapramWrite;
  MauMemory::kKeepFullResStats = MauMemory_kKeepFullResStats;
  MauMemory::kAccessFullResStats = MauMemory_kAccessFullResStats;

  MauMemory::kStatsFullVAddrPbusShift = MauMemory_kStatsFullVAddrPbusShift;
  MauMemory::kStatsVAddrPbusShift = MauMemory_kStatsVAddrPbusShift;
  MauMemory::kMeterVAddrPbusShift = MauMemory_kMeterVAddrPbusShift;
  MauMemory::kSelectorStatefulVAddrPbusShift = MauMemory_kSelectorStatefulVAddrPbusShift;
  MauMemory::kIdletimeVAddrPbusShift = MauMemory_kIdletimeVAddrPbusShift;

  MauMemory::kStatsVAddrPbusReadBubbleEmulate = MauMemory_kStatsVAddrPbusReadBubbleEmulate;
  MauMemory::kStatsVAddrPbusWriteBubbleEmulate = MauMemory_kStatsVAddrPbusWriteBubbleEmulate;
  MauMemory::kStatsVAddrSweepBubbleEmulate = MauMemory_kStatsVAddrSweepBubbleEmulate;
  MauMemory::kStatsVAddrDumpWordBubbleEmulate = MauMemory_kStatsVAddrDumpWordBubbleEmulate;

  MauMemory::kMeterVAddrPbusReadBubbleEmulate = MauMemory_kMeterVAddrPbusReadBubbleEmulate;
  MauMemory::kMeterVAddrPbusWriteBubbleEmulate = MauMemory_kMeterVAddrPbusWriteBubbleEmulate;
  MauMemory::kMeterVAddrSweepBubbleEmulate = MauMemory_kMeterVAddrSweepBubbleEmulate;

  MauMemory::kIdletimeVAddrPbusReadBubbleEmulate = MauMemory_kIdletimeVAddrPbusReadBubbleEmulate;
  MauMemory::kIdletimeVAddrPbusWriteBubbleEmulate = MauMemory_kIdletimeVAddrPbusWriteBubbleEmulate;
  MauMemory::kIdletimeVAddrSweepBubbleEmulate = MauMemory_kIdletimeVAddrSweepBubbleEmulate;
  MauMemory::kIdletimeVAddrDumpBubbleEmulate = MauMemory_kIdletimeVAddrDumpBubbleEmulate;
  MauMemory::kIdletimeVAddrDumpWordBubbleEmulate = MauMemory_kIdletimeVAddrDumpWordBubbleEmulate;

  MauMemory::kSelectorStatefulVAddrPbusReadBubbleEmulate = MauMemory_kSelectorStatefulVAddrPbusReadBubbleEmulate;
  MauMemory::kSelectorStatefulVAddrPbusWriteBubbleEmulate = MauMemory_kSelectorStatefulVAddrPbusWriteBubbleEmulate;
  MauMemory::kSelectorStatefulVAddrSweepBubbleEmulate = MauMemory_kSelectorStatefulVAddrSweepBubbleEmulate;

  MauMemory::kRelaxVirtWriteDataCheck = MauMemory_kRelaxVirtWriteDataCheck;

  MauOpHandlerCommon::kZeroisePushPopData = MauOpHandlerCommon_kZeroisePushPopData;
  MauOpHandlerCommon::kAllowAtomicWideBubbles = MauOpHandlerCommon_kAllowAtomicWideBubbles;

  MauMoveregs::kAllowMoveregsCommitOnTcamHit = MauMoveregs_kAllowMoveregsCommitOnTcamHit;

  Address::kStatsAddrFixShift = Address_kStatsAddrFixShift;
  Address::kStatsAddrSramShift = Address_kStatsAddrSramShift;

  Address::kGlobalAddrEnable = Address_kGlobalAddrEnable;

  MauLookupResult::kRelaxLookupShiftPfePosCheck = MauLookupResult_kRelaxLookupShiftPfePosCheck;
  MauLookupResult::kRelaxPayloadShifterEnabledCheck = MauLookupResult_kRelaxPayloadShifterEnabledCheck;
  MauLookupResult::kRelaxPayloadShifterEnabledCheck = MauLookupResult_kRelaxPayloadShifterEnabledCheck;

  MauHashDistribution::kRelaxHashSelectorLenCheck = MauHashDistribution_kRelaxHashSelectorLenCheck;
  MauHashDistribution::kRelaxHashSelectorShiftCheck = MauHashDistribution_kRelaxHashSelectorShiftCheck;

  MauPredicationCommon::kRelaxPredicationCheck = MauPredicationCommon_kRelaxPredicationCheck;
  MauPredicationCommon::kAllowUnpoweredTablesToBecomeActive = MauPredicationCommon_kAllowUnpoweredTablesToBecomeActive;
  MauPredicationCommon::kPowerTablesForInactiveThreads = MauPredicationCommon_kPowerTablesForInactiveThreads;

  MauStatsAlu::kKeepFullResStats = MauStatsAlu_kKeepFullResStats ;
  MauStatsAlu::kCapStats = MauStatsAlu_kCapStats;

  MauSram::kRelaxSramVpnCheck = MauSram_kRelaxSramVpnCheck;
  MauSram::kRelaxSramEmptyCheck = MauSram_kRelaxSramEmptyCheck;

  MauMapram::kMapramWriteData1HasTime = MauMapram_kMapramWriteData1HasTime;

  MauSramReg::kRelaxSramIngressCheck = MauSramReg_kRelaxSramIngressCheck;
  MauSramReg::kRelaxSramBitposCheck = MauSramReg_kRelaxSramBitposCheck;

  MauMapramReg::kRelaxMapramIngressCheck = MauMapramReg_kRelaxMapramIngressCheck;

  MauLogicalRow::kRelaxRowSelectorShiftCheck = MauLogicalRow_kRelaxRowSelectorShiftCheck;
  MauLogicalRow::kRelaxDataMultiWriteCheck = MauLogicalRow_kRelaxDataMultiWriteCheck;
  MauLogicalRow::kRelaxAddrMultiWriteCheck = MauLogicalRow_kRelaxAddrMultiWriteCheck;

  MauSramColumn::kRelaxMultiHitCheck = MauSramColumn_kRelaxMultiHitCheck;
  MauSramColumn::kRelaxMultiColumnHitCheck = MauSramColumn_kRelaxMultiColumnHitCheck;

  MauSramRow::kRelaxMatchBusMultiWriteCheck = MauSramRow_kRelaxMatchBusMultiWriteCheck;
  MauSramRow::kRelaxTindBusMultiWriteCheck = MauSramRow_kRelaxTindBusMultiWriteCheck;

  MauSramRowReg::kMemoryCoreSplit = MauSramRowReg_kMemoryCoreSplit;
  MauSramRowReg::kRelaxOfloWrMuxCheck = MauSramRowReg_kRelaxOfloWrMuxCheck;
  MauSramRowReg::kRelaxOfloRdMuxCheck = MauSramRowReg_kRelaxOfloRdMuxCheck;
  MauSramRowReg::kRelaxSynth2PortFabricCheck = MauSramRowReg_kRelaxSynth2PortFabricCheck;

  MauColorSwitchbox::kRelaxColorBusDiffAluCheck = MauColorSwitchbox_kRelaxColorBusDiffAluCheck;

  MauHashDistributionRegs::kRelaxGroupEnableChecks = MauHashDistributionRegs_kRelaxGroupEnableChecks;

  TableInfo::kAllowSynchronousIdleDump = TableInfo_kAllowSynchronousIdleDump;
  TableInfo::kAllowSynchronousIdleDumpWord = TableInfo_kAllowSynchronousIdleDumpWord;
  TableInfo::kAllowSynchronousStatsDump = TableInfo_kAllowSynchronousStatsDump;
  TableInfo::kAllowSynchronousStatsDumpWord = TableInfo_kAllowSynchronousStatsDumpWord;
  TableInfo::kDisableIdleSweepOnTableIdle = TableInfo_kDisableIdleSweepOnTableIdle;
  TableInfo::kSynchronousStatsOps = TableInfo_kSynchronousStatsOps;
  TableInfo::kSynchronousIdleOps = TableInfo_kSynchronousIdleOps;

  MauAddrDist::kRelaxPrePfeAddrCheck = MauAddrDist_kRelaxPrePfeAddrCheck;
  MauAddrDist::kMeterSweepOnDemand = MauAddrDist_kMeterSweepOnDemand;
  MauAddrDist::kMeterSweepOnDemandPipe0 = MauAddrDist_kMeterSweepOnDemandPipe0;
  MauAddrDist::kRelaxPacketActionAtHdrCheck = MauAddrDist_kRelaxPacketActionAtHdrCheck;
  MauAddrDist::kRelaxAllAddrsConsumedCheck = MauAddrDist_kRelaxAllAddrsConsumedCheck;
  MauAddrDist::kRelaxActionAddrsConsumedCheck = MauAddrDist_kRelaxActionAddrsConsumedCheck;
  MauAddrDist::kRelaxStatsAddrsConsumedCheck = MauAddrDist_kRelaxStatsAddrsConsumedCheck;
  MauAddrDist::kRelaxMeterAddrsConsumedCheck = MauAddrDist_kRelaxMeterAddrsConsumedCheck;
  MauAddrDist::kRelaxIdletimeAddrsConsumedCheck = MauAddrDist_kRelaxIdletimeAddrsConsumedCheck;

  MauStatefulCounters::kRelaxSwPushPopInvalidErrors = MauStatefulCounters_kRelaxSwPushPopInvalidErrors;
  MauStatefulCounters::kRelaxSwPushPopOverflowUnderflowErrors = MauStatefulCounters_kRelaxSwPushPopOverflowUnderflowErrors;
  MauStatefulCounters::kSynchronousStatefulCounterClear = MauStatefulCounters_kSynchronousStatefulCounterClear;
  MauStatefulCounters::kStatefulCounterTickCheckTime = MauStatefulCounters_kStatefulCounterTickCheckTime;

  MauDependencies::kRelaxDependencyCheck = MauDependencies_kRelaxDependencyCheck;
  MauDependencies::kRelaxDelayCheck = MauDependencies_kRelaxDelayCheck;
  MauDependencies::kRelaxReplicationCheck = MauDependencies_kRelaxReplicationCheck;
  MauDependencies::kRelaxThreadCheck = MauDependencies_kRelaxThreadCheck;

  MauMeterAlu::kRelaxThreadCheck = MauMeterAlu_kRelaxThreadCheck;
  MauMeterAlu::kRelaxOpCheck = MauMeterAlu_kRelaxOpCheck;
  MauMeter::kRelaxExponentCheck = MauMeter_kRelaxExponentCheck;

  MauGatewayPayload::kRelaxGatewayReplicationCheck = MauGatewayPayload_kRelaxGatewayReplicationCheck;

  ParserShared::kRelaxExtractionCheck = ParserShared_kRelaxExtractionCheck;
  ParserShared::kRelaxPreExtractionCheck = ParserShared_kRelaxPreExtractionCheck;
  ParserShared::kRelaxFinalParseCheck = ParserShared_kRelaxFinalParseCheck;
  Clot::kRelaxOverlapCheck = Clot_kRelaxOverlapCheck;
  Clot::kAllowAdjacent = Clot_kAllowAdjacent;
  Clot::kAllowDuplicate = Clot_kAllowDuplicate;

  MauLogicalTable::kRelaxPairedLtcamErrors = MauLogicalTable_kRelaxPairedLtcamErrors;
  MauLogicalTable::kRelaxHdrtimeMeterAddrColorCheck = MauLogicalTable_kRelaxHdrtimeMeterAddrColorCheck;
  MauLogicalTable::kRelaxHdrtimeMeterAddrNopCheck = MauLogicalTable_kRelaxHdrtimeMeterAddrNopCheck;

  MauLogicalTcam::kRelaxTcamCheck = MauLogicalTcam_kRelaxTcamCheck;

  MauInstrStoreCommon::kRelaxThreadReplicationCheck = MauInstrStoreCommon_kRelaxThreadReplicationCheck;
  MauInstrStoreCommon::kRelaxAddrFormatReplicationCheck = MauInstrStoreCommon_kRelaxAddrFormatReplicationCheck;
  MauInstrStoreCommon::kRelaxThreadOverlapCheck = MauInstrStoreCommon_kRelaxThreadOverlapCheck;
  MauInstrStoreCommon::kRelaxInstrOverlapCheck = MauInstrStoreCommon_kRelaxInstrOverlapCheck;

  Instr::kRelaxInstrFormatCheck = Instr_kRelaxInstrFormatCheck;
  Instr::kRelaxInstrMatchingGressCheck = Instr_kRelaxInstrMatchingGressCheck;
  Instr::kRelaxEqNeqConfigCheck = Instr_kRelaxEqNeqConfigCheck;

  Instr::kInstrCondMoveIsUnconditional = Instr_kInstrCondMoveIsUnconditional;
  Instr::kInstrCondMuxIsCondMove = Instr_kInstrCondMuxIsCondMove;
  Instr::kInstrInvalidateIsNop = Instr_kInstrInvalidateIsNop;
  Instr::kInstrInvalidateIsErr = Instr_kInstrInvalidateIsErr;
  Instr::kInstrPairDpfIsErr = Instr_kInstrPairDpfIsErr;
  Instr::kInstrDataDepShiftSupported = Instr_kInstrDataDepShiftSupported;
  Instr::kInstrReverseSubtractSupported = Instr_kInstrReverseSubtractSupported;

  Packet::kPktInitTimeRandOnAlloc = Packet_kPktInitTimeRandOnAlloc;
  PhvFactory::kPhvInitTimeRandOnAlloc = PhvFactory_kPhvInitTimeRandOnAlloc;
  PhvFactory::kPhvInitAllValid = PhvFactory_kPhvInitAllValid;

  MauSnapshotCommon::kSnapshotCompareValidBit = MauSnapshotCommon_kSnapshotCompareValidBit;
  MauSnapshotCommon::kSnapshotCaptureValidBit = MauSnapshotCommon_kSnapshotCaptureValidBit;
  MauSnapshotCommon::kSnapshotUsePhvTime = MauSnapshotCommon_kSnapshotUsePhvTime;
  MauSnapshotCommon::kSnapshotMaskThreadFields = MauSnapshotCommon_kSnapshotMaskThreadFields;

  ActionOutputHvXbar::kBugActionDataBusMaskBeforeDup = ActionOutputHvXbar_kBugActionDataBusMaskBeforeDup;

  MauStatefulAlu::kRelaxSaluPredRsiCheck = MauStatefulAlu_kRelaxSaluPredRsiCheck;

  MauChipStatefulAlu::kMaxReturnsHighestIndex = MauChipStatefulAlu_kMaxReturnsHighestIndex;
  MauChipStatefulAlu::kMinReturnsHighestIndex = MauChipStatefulAlu_kMinReturnsHighestIndex;

  Queueing::kAllowMissingDies = Queueing_kAllowMissingDies;
}

}
