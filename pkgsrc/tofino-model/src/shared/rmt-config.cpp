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
#include <common/rmt-features.h>
#include <chip-features.h>
#include <chip.h>
#include <indirect_access_block.h>
#include <rmt-log.h>
#include <rmt-defs.h>
#include <address.h>
#include <mau-memory.h>
#include <mau-stats-alu.h>
#include <mau-sram-reg.h>
#include <mau-mapram-reg.h>
#include <rmt-sweeper.h>
#include <rmt-object-manager.h>
#include <mau-dependencies.h>
#include <mau-snapshot-common.h>
#include <queueing.h>
#include <parser.h>
#include <packet.h>
#include <phv-factory.h>
#include <action-output-hv-xbar.h>
#include <deparser.h>
#include <rmt-packet-coordinator.h>


namespace MODEL_CHIP_NAMESPACE {

  // Do phv_lookup/handle_eop/pbus_OP use a mutex to ensure exclusive access
  // Should only be set to false if ALL kXXXVAddrPbusBubbleEmulate booleans
  // are false OR if you are absolutely SURE only one thread at a time will
  // do PhvLookup, HandleEop, IndirectRead, IndirectWrite, SetTime etc.
  bool Mau::kMauUseMutex = true;

  // Do we honour Tofino DinPower features
  bool Mau::kMauDinPowerMode = true;
  // Do we reset unused MauLookupResult entries
  bool Mau::kResetUnusedLookupResults = RESET_UNUSED_LOOKUP_RESULTS;
  // Do we lookup LTCAMs not associated with any LT
  bool Mau::kLookupUnusedLtcams = LOOKUP_UNUSED_LTCAMS;
  // Do we set next_table_pred
  bool Mau::kSetNextTablePred = SET_NEXT_TABLE_PRED;
  // Relax checks that depend on info from preceding MAU
  bool Mau::kRelaxPrevStageCheck = false;

  // Allow selection of diff MAU features - see consts in mau.h
  // NB. Stages after stage6 are default initialised (to 0u)
  uint32_t Mau::kMauFeatures[RmtDefs::kStagesMax] = {
    MAU_FEATURES_0, 0u, 0u, 0u, 0u, 0u, MAU_FEATURES_6
  };

  // Do InWord/OutWord/IndirectRead/IndirectWrite use a mutex to ensure
  // exclusive access, and should we InitChip the first time we call GetObjMgr
  bool Chip::kChipUseMutex = CHIP_USE_MUTEX;
  bool Chip::kInitOnVeryFirstAccess = true;
  bool Chip::kUseGlobalTimeIfZero = CHIP_USE_GLOBAL_TIME_IF_ZERO;

  // Do we insist on good addr types, mem types in phys|virt addresses
  bool MauMemory::kAllowBadAddrTypeRead = false;
  bool MauMemory::kAllowBadAddrTypeWrite = false;
  bool MauMemory::kAllowBadMemTypePhysRead = false;
  bool MauMemory::kAllowBadMemTypePhysWrite = false;
  bool MauMemory::kAllowBadMemTypeVirtRead = false;
  bool MauMemory::kAllowBadMemTypeVirtWrite = false;

  // Do we insist on the phys|virt addresses having zeros in [29:21]|[29:27]
  bool MauMemory::kAllowBadPhysRead = false;
  bool MauMemory::kAllowBadPhysWrite = false;
  bool MauMemory::kAllowBadVirtRead = false;
  bool MauMemory::kAllowBadVirtWrite = false;

  // Do we allow bad sram/tcam/mapram reads/writes or throw an error
  bool MauMemory::kAllowBadSramRead = false;
  bool MauMemory::kAllowBadSramWrite = false;
  bool MauMemory::kAllowBadTcamRead = false;
  bool MauMemory::kAllowBadTcamWrite = false;
  bool MauMemory::kAllowBadMapramRead = false;
  bool MauMemory::kAllowBadMapramWrite = false;

  // Do we keep and allow access to Full Res Stats
  // Initial value dependent on mode - see rmt-features.h
  bool MauMemory::kKeepFullResStats = KEEP_FULL_RES_STATS;
  bool MauMemory::kAccessFullResStats = ACCESS_FULL_RES_STATS;

  // What shifts to apply to PBus Virtual Addresses
  int MauMemory::kStatsFullVAddrPbusShift = 0;
  int MauMemory::kStatsVAddrPbusShift = 0;
  int MauMemory::kMeterVAddrPbusShift = -7; // No subword bits from PBus
  int MauMemory::kSelectorStatefulVAddrPbusShift = -2; // Only 5 from PBus, need 7
  int MauMemory::kIdletimeVAddrPbusShift = 0;

  // Do we use backend to do PBUS access (true) or just fake it (false)
  // Fake it by default for Idletime as otherwise way too slow
  bool MauMemory::kStatsVAddrPbusReadBubbleEmulate = true;
  bool MauMemory::kStatsVAddrPbusWriteBubbleEmulate = true;
  bool MauMemory::kStatsVAddrSweepBubbleEmulate = true;
  bool MauMemory::kStatsVAddrDumpWordBubbleEmulate = true;

  bool MauMemory::kMeterVAddrPbusReadBubbleEmulate = true;
  bool MauMemory::kMeterVAddrPbusWriteBubbleEmulate = true;
  bool MauMemory::kMeterVAddrSweepBubbleEmulate = true;

  bool MauMemory::kIdletimeVAddrPbusReadBubbleEmulate = true;
  bool MauMemory::kIdletimeVAddrPbusWriteBubbleEmulate = true;
  bool MauMemory::kIdletimeVAddrSweepBubbleEmulate = false;
  bool MauMemory::kIdletimeVAddrDumpBubbleEmulate = false;
  bool MauMemory::kIdletimeVAddrDumpWordBubbleEmulate = false;

  bool MauMemory::kSelectorStatefulVAddrPbusReadBubbleEmulate = true;
  bool MauMemory::kSelectorStatefulVAddrPbusWriteBubbleEmulate = true;
  bool MauMemory::kSelectorStatefulVAddrSweepBubbleEmulate = true;

  // Do we allow virt writes to pass non-0 data bits outside the ones
  // selected by the subword specified in the address - only relevant
  // for stats, selector/stateful and idletime virtual writes
  bool MauMemory::kRelaxVirtWriteDataCheck = false;

  // Do we zeroise data0/data1 passed to push/pop instructions
  // (and also zeroise data1 passed to run_stateful instruction)
  bool MauOpHandlerCommon::kZeroisePushPopData = ZEROISE_PUSH_POP_DATA;
  bool MauOpHandlerCommon::kAllowAtomicWideBubbles = true;

  // Do we allow TCAM hits to trigger a movereg commit
  bool MauMoveregs::kAllowMoveregsCommitOnTcamHit = true;

  // Bottom 2 bits stats address not transmitted
  // so recreated (notionally) at SRAM
  int Address::kStatsAddrFixShift = 0;
  int Address::kStatsAddrSramShift = -2;

  // Override MSB in instr/action/stats/meter/idletime addresses
  // so these addresses always considered valid
  bool Address::kGlobalAddrEnable = false;

  // Relax certain consistency checks in MauLookupResult MatchMerge regs
  bool MauLookupResult::kRelaxLookupShiftPfePosCheck = EXTRACT_CHECK_RELAX;
  bool MauLookupResult::kRelaxLookupShiftOpPosCheck = EXTRACT_CHECK_RELAX;
  bool MauLookupResult::kRelaxPayloadShifterEnabledCheck = false;

  // Relax certain consistency checks in MauHashDistribution
  bool MauHashDistribution::kRelaxHashSelectorLenCheck = false;
  bool MauHashDistribution::kRelaxHashSelectorShiftCheck = false;

  // Relax consistency checks in predication logic
  bool MauPredicationCommon::kRelaxPredicationCheck = false;
  bool MauPredicationCommon::kAllowUnpoweredTablesToBecomeActive = false;
  bool MauPredicationCommon::kPowerTablesForInactiveThreads = false;

  // Get StatsALU to keep full-resolution statistics
  // Initial value dependent on mode - see rmt-features.h
  bool MauStatsAlu::kKeepFullResStats = KEEP_FULL_RES_STATS;
  // Do pkt/byte counters get capped or just overflow
  bool MauStatsAlu::kCapStats = false;

  // Relax Synth2port VPN checks
  bool MauSram::kRelaxSramVpnCheck = false;
  bool MauSram::kRelaxSramEmptyCheck = false;

  // Do we use data1 on mapram write to contain relative_time
  bool MauMapram::kMapramWriteData1HasTime = MAPRAM_WRITE_DATA1_HAS_TIME;
  uint64_t MauMapram::kMapramColorWriteLatencyTEOP = MauDefs::kMapramColorWriteLatencyTEOP;

  // Relax certain consistency checks for UnitRam registers
  bool MauSramReg::kRelaxSramIngressCheck = false;
  bool MauSramReg::kRelaxSramBitposCheck = false;

  // Relax certain consistency checks for MapRam registers
  bool MauMapramReg::kRelaxMapramIngressCheck = false;

  // Relax certain consistency checks in MauLogicalRow regs
  bool MauLogicalRow::kRelaxRowSelectorShiftCheck = false;
  bool MauLogicalRow::kRelaxDataMultiWriteCheck = false;
  bool MauLogicalRow::kRelaxAddrMultiWriteCheck = false;

  // Relax certain consistency checks in MauSramColumn logic
  bool MauSramColumn::kRelaxMultiHitCheck = false;
  bool MauSramColumn::kRelaxMultiColumnHitCheck = false;

  // Relax certain consistency checks in MauSram logic
  bool MauSramRow::kRelaxMatchBusMultiWriteCheck = false;
  bool MauSramRow::kRelaxTindBusMultiWriteCheck = false;

  // Relax certain consistency checks in MauSramRow regs
  // Initial value MemoryCoreSplit dependent on chip - see chip-features.h
  // Initial value RdMuxCheck dependent on mode - see rmt-features.h
  bool MauSramRowReg::kMemoryCoreSplit = MEMORY_CORE_SPLIT;
  bool MauSramRowReg::kRelaxOfloWrMuxCheck = false;
  bool MauSramRowReg::kRelaxOfloRdMuxCheck = OFLO_ROW_CHECK_RELAX;
  bool MauSramRowReg::kRelaxSynth2PortFabricCheck = SYNTH2PORT_CHECK_RELAX;

  //  Relax certain consistency checks in MauColorSwitchbox
  bool MauColorSwitchbox::kRelaxColorBusDiffAluCheck = false;

  //  Relax certain consistency checks in MauHashDistributionRegs
  bool MauHashDistributionRegs::kRelaxGroupEnableChecks = false;

  // Allow dump/dump_word OPs to occur synchronously on thread
  // that does Write of corresponding Instr
  bool TableInfo::kAllowSynchronousIdleDump = false;
  bool TableInfo::kAllowSynchronousIdleDumpWord = false;
  bool TableInfo::kAllowSynchronousStatsDump = false;
  bool TableInfo::kAllowSynchronousStatsDumpWord = false;
  // Disable sweep of idle table if it is completely idle
  bool TableInfo::kDisableIdleSweepOnTableIdle = true;
  // These next two take precedence over kAllowSynchronous flags above
  bool TableInfo::kSynchronousIdleOps = true;
  bool TableInfo::kSynchronousStatsOps = true;

  // Relax certain AddressDistribution consistency checks
  // Initial value AddrsConsumed checks dependent on mode - see rmt-features.h
  bool MauAddrDist::kRelaxPrePfeAddrCheck = PRE_PFE_ADDR_CHECK_RELAX;
  bool MauAddrDist::kMeterSweepOnDemand = METER_SWEEP_ON_DEMAND;
  bool MauAddrDist::kMeterSweepOnDemandPipe0 = false; // Only for unit-testing
  bool MauAddrDist::kRelaxPacketActionAtHdrCheck = false;
  bool MauAddrDist::kRelaxAllAddrsConsumedCheck = false;
  bool MauAddrDist::kRelaxActionAddrsConsumedCheck = ADDRS_CONSUMED_CHECK_RELAX;
  bool MauAddrDist::kRelaxStatsAddrsConsumedCheck = ADDRS_CONSUMED_CHECK_RELAX;
  bool MauAddrDist::kRelaxMeterAddrsConsumedCheck = ADDRS_CONSUMED_CHECK_RELAX;
  bool MauAddrDist::kRelaxIdletimeAddrsConsumedCheck = ADDRS_CONSUMED_CHECK_RELAX;

  // Relax StatefulCounter checks and whether we run StatefulClear synchronously
  bool MauStatefulCounters::kRelaxSwPushPopInvalidErrors = false;
  bool MauStatefulCounters::kRelaxSwPushPopOverflowUnderflowErrors = false;
  bool MauStatefulCounters::kSynchronousStatefulCounterClear = SYNC_SALU_CNTR_CLEAR;
  bool MauStatefulCounters::kStatefulCounterTickCheckTime = SALU_CNTR_TICK_CHECK_TIME;

  // Relax MAU0 specific config checks
  bool MauDependencies::kRelaxDependencyCheck = DEPENDENCY_CHECK_RELAX;
  bool MauDependencies::kRelaxDelayCheck = DELAY_CHECK_RELAX;
  bool MauDependencies::kRelaxReplicationCheck = false;
  bool MauDependencies::kRelaxThreadCheck = false;

  // Relax MeterALU consistency checks
  bool MauMeterAlu::kRelaxThreadCheck = false;
  bool MauMeterAlu::kRelaxOpCheck = false;
  bool MauMeter::kRelaxExponentCheck = METER_EXPONENT_CHECK_RELAX;

  // Relax Gateway consistency checks
  bool MauGatewayPayload::kRelaxGatewayReplicationCheck = false;

  // Relax Parser checks
  bool ParserShared::kRelaxExtractionCheck = PARSE_CHECK_RELAX;
  bool ParserShared::kRelaxPreExtractionCheck = true;
  bool ParserShared::kRelaxFinalParseCheck = false;
  bool Clot::kRelaxOverlapCheck = CLOT_OVERLAP_CHECK_RELAX;
  bool Clot::kAllowAdjacent = CLOT_ALLOW_ADJACENT;
  bool Clot::kAllowDuplicate = CLOT_ALLOW_DUPLICATE;
  bool Clot::kAllowRepeatEmit = CLOT_ALLOW_REPEAT_EMIT;

  // Relax LogicalTable checks
  bool MauLogicalTable::kRelaxPairedLtcamErrors = PAIRED_LTCAM_CHECK_RELAX;
  bool MauLogicalTable::kRelaxHdrtimeMeterAddrColorCheck = false;
  bool MauLogicalTable::kRelaxHdrtimeMeterAddrNopCheck = false;

  // Relax TCAM checks
  bool MauLogicalTcam::kRelaxTcamCheck = false;

  // Relax MauInstrStore/IMEM checks
  bool MauInstrStoreCommon::kRelaxThreadReplicationCheck = false;
  bool MauInstrStoreCommon::kRelaxAddrFormatReplicationCheck = false;
  bool MauInstrStoreCommon::kRelaxThreadOverlapCheck = false;
  bool MauInstrStoreCommon::kRelaxInstrOverlapCheck = false;

  // Relax VLIW Instr checks
  bool Instr::kRelaxInstrFormatCheck = INSTR_FORMAT_CHECK_RELAX;
  bool Instr::kRelaxInstrMatchingGressCheck = false;
  bool Instr::kRelaxEqNeqConfigCheck = INSTR_CONFIG_CHECK_RELAX;

  // Other VLIW op config
  bool Instr::kInstrCondMoveIsUnconditional = INSTR_COND_MOVE_IS_UNCONDITIONAL;
  bool Instr::kInstrCondMuxIsCondMove = INSTR_COND_MUX_IS_COND_MOVE;
  bool Instr::kInstrInvalidateIsNop = INSTR_INVALIDATE_IS_NOP;
  bool Instr::kInstrInvalidateIsErr = INSTR_INVALIDATE_IS_ERR;
  bool Instr::kInstrPairDpfIsErr = INSTR_PAIRDPF_IS_ERR;
  bool Instr::kInstrDataDepShiftSupported = INSTR_DATADEP_SHIFT_SUPPORTED;
  bool Instr::kInstrReverseSubtractSupported = INSTR_REVERSE_SUBTRACT_SUPPORTED;

  // Control whether we initialise Time/Rand info on allocation Packets/Phvs
  bool Packet::kPktInitTimeRandOnAlloc = PKT_INIT_TIMERAND_ON_ALLOC;
  bool PhvFactory::kPhvInitTimeRandOnAlloc = PHV_INIT_TIMERAND_ON_ALLOC;
  // Control whether PHVs are initially created with all words marked valid
  bool PhvFactory::kPhvInitAllValid = PHV_INIT_ALL_VALID;

  // Snapshot configuration (do we take account of PHV valid bit on compare/capture)
  bool MauSnapshotCommon::kSnapshotCompareValidBit = SNAPSHOT_COMPARE_VALID_BIT;
  bool MauSnapshotCommon::kSnapshotCaptureValidBit = SNAPSHOT_CAPTURE_VALID_BIT;
  bool MauSnapshotCommon::kSnapshotUsePhvTime = SNAPSHOT_USE_PHV_TIME;
  bool MauSnapshotCommon::kSnapshotMaskThreadFields = SNAPSHOT_MASK_THREAD_FIELDS;

  // Flags to turn on certain buggy behaviours
  bool ActionOutputHvXbar::kBugActionDataBusMaskBeforeDup = BUG_ACTION_DATA_BUS_MASK_BEFORE_DUP;

  // Do we assert on RunStatefulInstructon to predicated *ON* Stateful ALU
  bool MauStatefulAlu::kRelaxSaluPredRsiCheck = SALU_PRED_RSI_CHECK_RELAX;

  // Flags to determine which index is returned for equal values in JBay SALU Max and Min instructions
  bool MauChipStatefulAlu::kMaxReturnsHighestIndex = true;
  bool MauChipStatefulAlu::kMinReturnsHighestIndex = true;

  // JBay Deparser checks relaxing
  bool Deparser::kRelaxDeparserChunkChecks    = false;
  bool Deparser::kRelaxDeparserByteChecks     = false;
  bool Deparser::kRelaxDeparserGroupChecks    = false;
  bool Deparser::kRelaxDeparserFdeChecks      = false;
  bool Deparser::kRelaxDeparserClotChecks     = false;
  bool Deparser::kRelaxDeparserClotLenChecks  = RELAX_DEPARSER_CLOT_LEN_CHECKS;

  // Relax Queueing checks
  bool Queueing::kAllowMissingDies = false;

  // Gress execution
  bool RmtPacketCoordinator::kProcessGressesSeparately = false;

  // Fill other gress with non-zero values during parsing
  // (Requires gress execution to be separated.)
  bool ParserShared::kPhvFillOtherGress = false;
}
