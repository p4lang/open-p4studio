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

#ifndef _UTESTS_TEST_CONFIG_
#define _UTESTS_TEST_CONFIG_

#include <cinttypes>
#include <utests/test_namespace.h>


namespace MODEL_CHIP_TEST_NAMESPACE {

  class TestConfig {

    static constexpr int kAbsMaxStages = 64;

 public:
    TestConfig();
    ~TestConfig();

 private:
    // Global vars controlling THROW_ERROR RMT_ASSERT macros etc.
    int GLOBAL_AbortOnError;
    int GLOBAL_ThrowOnError;
    int GLOBAL_ThrowOnAssert;

    // Do we support XXX
    bool Model_kAllowCb50;

    // Do phv_lookup/handle_eop/pbus_OP use a mutex to ensure exclusive access
    // Should only be set to false if ALL kXXXVAddrPbusBubbleEmulate booleans
    // are false OR if you are absolutely SURE only one thread at a time will
    // do PhvLookup, HandleEop, IndirectRead, IndirectWrite, SetTime etc.
    bool Mau_kMauUseMutex;

    // Do we honour DinPower features
    bool Mau_kMauDinPowerMode;
    // Do we reset unused MauLookupResult entries
    bool Mau_kResetUnusedLookupResults;
    // Do we lookup LTCAMs not associated with any LT
    bool Mau_kLookupUnusedLtcams;
    // Do we set next_table_pred
    bool Mau_kSetNextTablePred;
    // Relax checks that depend on info from preceding MAU
    bool Mau_kRelaxPrevStageCheck;

    // Allow selection of diff MAU features - see consts in mau.h
    uint32_t Mau_kMauFeatures[kAbsMaxStages];

    // Do InWord/OutWord/IndirectRead/IndirectWrite use a mutex to ensure
    // exclusive access, and should we InitChip the first time we call GetObjMgr
    bool Chip_kChipUseMutex;
    bool Chip_kInitOnVeryFirstAccess;
    bool Chip_kUseGlobalTimeIfZero;

    // Do we insist on good addr types, mem types in phys|virt addresses
    bool MauMemory_kAllowBadAddrTypeRead;
    bool MauMemory_kAllowBadAddrTypeWrite;
    bool MauMemory_kAllowBadMemTypePhysRead;
    bool MauMemory_kAllowBadMemTypePhysWrite;
    bool MauMemory_kAllowBadMemTypeVirtRead;
    bool MauMemory_kAllowBadMemTypeVirtWrite;

    // Do we insist on the phys|virt addresses having zeros in [29:21]|[29:27]
    bool MauMemory_kAllowBadPhysRead;
    bool MauMemory_kAllowBadPhysWrite;
    bool MauMemory_kAllowBadVirtRead;
    bool MauMemory_kAllowBadVirtWrite;

    // Do we allow bad sram/tcam/mapram reads/writes or throw an error
    bool MauMemory_kAllowBadSramRead;
    bool MauMemory_kAllowBadSramWrite;
    bool MauMemory_kAllowBadTcamRead;
    bool MauMemory_kAllowBadTcamWrite;
    bool MauMemory_kAllowBadMapramRead;
    bool MauMemory_kAllowBadMapramWrite;

    // Do we keep and allow access to Full Res Stats
    // Initial value dependent on mode - see rmt-features.h
    bool MauMemory_kKeepFullResStats;
    bool MauMemory_kAccessFullResStats;

    // What shifts to apply to PBus Virtual Addresses
    int MauMemory_kStatsFullVAddrPbusShift ;
    int MauMemory_kStatsVAddrPbusShift;
    int MauMemory_kMeterVAddrPbusShift;
    int MauMemory_kSelectorStatefulVAddrPbusShift;
    int MauMemory_kIdletimeVAddrPbusShift;

    // Do we use backend to do PBUS access (true) or just fake it (false)
    bool MauMemory_kStatsVAddrPbusReadBubbleEmulate;
    bool MauMemory_kStatsVAddrPbusWriteBubbleEmulate;
    bool MauMemory_kStatsVAddrSweepBubbleEmulate;
    bool MauMemory_kStatsVAddrDumpWordBubbleEmulate;

    bool MauMemory_kMeterVAddrPbusReadBubbleEmulate;
    bool MauMemory_kMeterVAddrPbusWriteBubbleEmulate;
    bool MauMemory_kMeterVAddrSweepBubbleEmulate;

    bool MauMemory_kIdletimeVAddrPbusReadBubbleEmulate;
    bool MauMemory_kIdletimeVAddrPbusWriteBubbleEmulate;
    bool MauMemory_kIdletimeVAddrSweepBubbleEmulate;
    bool MauMemory_kIdletimeVAddrDumpBubbleEmulate;
    bool MauMemory_kIdletimeVAddrDumpWordBubbleEmulate;

    bool MauMemory_kSelectorStatefulVAddrPbusReadBubbleEmulate;
    bool MauMemory_kSelectorStatefulVAddrPbusWriteBubbleEmulate;
    bool MauMemory_kSelectorStatefulVAddrSweepBubbleEmulate;

    bool MauMemory_kRelaxVirtWriteDataCheck;

    // Do we zeroise data0/data1 passed to push/pop instructions
    // (and also zeroise data1 passed to run_stateful instruction)
    bool MauOpHandlerCommon_kZeroisePushPopData;
    bool MauOpHandlerCommon_kAllowAtomicWideBubbles;

    // Do we allow TCAM hits to trigger a movereg commit
    bool MauMoveregs_kAllowMoveregsCommitOnTcamHit;

    // Bottom 2 bits stats address not transmitted
    // so recreated (notionally) at SRAM
    int Address_kStatsAddrFixShift;
    int Address_kStatsAddrSramShift;

    // Override MSB in instr/action/stats/meter/idletime addresses
    // so these addresses always considered valid
    bool Address_kGlobalAddrEnable;

    // Relax certain consistency checks in MauLookupResult MatchMerge regs
    bool MauLookupResult_kRelaxLookupShiftPfePosCheck;
    bool MauLookupResult_kRelaxLookupShiftOpPosCheck;
    bool MauLookupResult_kRelaxPayloadShifterEnabledCheck;

    // Relax certain consistency checks in MauHashDistribution
    bool MauHashDistribution_kRelaxHashSelectorLenCheck;
    bool MauHashDistribution_kRelaxHashSelectorShiftCheck;

    // Relax consistency checks in predication logic
    bool MauPredicationCommon_kRelaxPredicationCheck;
    bool MauPredicationCommon_kAllowUnpoweredTablesToBecomeActive;
    bool MauPredicationCommon_kPowerTablesForInactiveThreads;

    // Get StatsALU to keep full-resolution statistics
    // Initial value dependent on mode - see rmt-features.h
    bool MauStatsAlu_kKeepFullResStats;
    // Do pkt/byte counters get capped or just overflow
    bool MauStatsAlu_kCapStats;

    // Relax Synth2port VPN checks
    bool MauSram_kRelaxSramVpnCheck;
    bool MauSram_kRelaxSramEmptyCheck;

    // Do we use data1 on mapram write to contain relative_time
    bool MauMapram_kMapramWriteData1HasTime;

    // Relax certain consistency checks for UnitRam registers
    bool MauSramReg_kRelaxSramIngressCheck;
    bool MauSramReg_kRelaxSramBitposCheck;

    // Relax certain consistency checks for MapRam registers
    bool MauMapramReg_kRelaxMapramIngressCheck;

    // Relax certain consistency checks in MauLogicalRow regs
    bool MauLogicalRow_kRelaxRowSelectorShiftCheck;
    bool MauLogicalRow_kRelaxDataMultiWriteCheck;
    bool MauLogicalRow_kRelaxAddrMultiWriteCheck;

    // Relax certain consistency checks in MauSramColumn logic
    bool MauSramColumn_kRelaxMultiHitCheck;
    bool MauSramColumn_kRelaxMultiColumnHitCheck;

    // Relax certain consistency checks in MauSram logic
    bool MauSramRow_kRelaxMatchBusMultiWriteCheck;
    bool MauSramRow_kRelaxTindBusMultiWriteCheck;

    // Relax certain consistency checks in MauSramRow regs
    // Initial value MemoryCoreSplit dependent on chip - see chip-features.h
    // Initial value RdMuxCheck dependent on mode - see rmt-features.h
    bool MauSramRowReg_kMemoryCoreSplit;
    bool MauSramRowReg_kRelaxOfloWrMuxCheck;
    bool MauSramRowReg_kRelaxOfloRdMuxCheck;
    bool MauSramRowReg_kRelaxSynth2PortFabricCheck;

    bool MauColorSwitchbox_kRelaxColorBusDiffAluCheck;

    bool MauHashDistributionRegs_kRelaxGroupEnableChecks;

    // Allow dump/dump_word OPs to occur synchronously on thread
    // that does Write of corresponding Instr
    bool TableInfo_kAllowSynchronousIdleDump;
    bool TableInfo_kAllowSynchronousIdleDumpWord;
    bool TableInfo_kAllowSynchronousStatsDump;
    bool TableInfo_kAllowSynchronousStatsDumpWord;
    // Disable sweep of idle table if it is completely idle
    bool TableInfo_kDisableIdleSweepOnTableIdle;
    // These next two take precedence over kAllowSynchronous flags above
    bool TableInfo_kSynchronousStatsOps;
    bool TableInfo_kSynchronousIdleOps;

    // Relax certain AddressDistribution consistency checks
    // Initial value checks dependent on mode - see rmt-features.h
    bool MauAddrDist_kRelaxPrePfeAddrCheck;
    bool MauAddrDist_kMeterSweepOnDemand;
    bool MauAddrDist_kMeterSweepOnDemandPipe0;
    bool MauAddrDist_kRelaxPacketActionAtHdrCheck;
    bool MauAddrDist_kRelaxAllAddrsConsumedCheck;
    bool MauAddrDist_kRelaxActionAddrsConsumedCheck;
    bool MauAddrDist_kRelaxStatsAddrsConsumedCheck;
    bool MauAddrDist_kRelaxMeterAddrsConsumedCheck;
    bool MauAddrDist_kRelaxIdletimeAddrsConsumedCheck;

    // Relax StatefulCounter checks and whether we run StatefulClear synchronously
    bool MauStatefulCounters_kRelaxSwPushPopInvalidErrors;
    bool MauStatefulCounters_kRelaxSwPushPopOverflowUnderflowErrors;
    bool MauStatefulCounters_kSynchronousStatefulCounterClear;
    bool MauStatefulCounters_kStatefulCounterTickCheckTime;

    // Relax MAU0 specific config checks
    bool MauDependencies_kRelaxDependencyCheck;
    bool MauDependencies_kRelaxDelayCheck;
    bool MauDependencies_kRelaxReplicationCheck;
    bool MauDependencies_kRelaxThreadCheck;

    // Relax MeterALU consistency checks
    bool MauMeterAlu_kRelaxThreadCheck;
    bool MauMeterAlu_kRelaxOpCheck;
    bool MauMeter_kRelaxExponentCheck;

    // Relax Gateway consistency checks
    bool MauGatewayPayload_kRelaxGatewayReplicationCheck;

    // Relax Parser checks
    bool ParserShared_kRelaxExtractionCheck;
    bool ParserShared_kRelaxPreExtractionCheck;
    bool ParserShared_kRelaxFinalParseCheck;
    bool Clot_kRelaxOverlapCheck;
    bool Clot_kAllowAdjacent;
    bool Clot_kAllowDuplicate;

    // Relax LogicalTable checks
    bool MauLogicalTable_kRelaxPairedLtcamErrors;
    bool MauLogicalTable_kRelaxHdrtimeMeterAddrColorCheck;
    bool MauLogicalTable_kRelaxHdrtimeMeterAddrNopCheck;

    // Relax TCAM checks
    bool MauLogicalTcam_kRelaxTcamCheck;

    // Relax MauInstrStore/IMEM checks
    bool MauInstrStoreCommon_kRelaxAddrFormatReplicationCheck;
    bool MauInstrStoreCommon_kRelaxThreadReplicationCheck;
    bool MauInstrStoreCommon_kRelaxThreadOverlapCheck;
    bool MauInstrStoreCommon_kRelaxInstrOverlapCheck;

    // Relax VLIW Instr checks
    bool Instr_kRelaxInstrFormatCheck;
    bool Instr_kRelaxInstrMatchingGressCheck;
    bool Instr_kRelaxEqNeqConfigCheck;

    // Other VLIW op config
    bool Instr_kInstrCondMoveIsUnconditional;
    bool Instr_kInstrCondMuxIsCondMove;
    bool Instr_kInstrInvalidateIsNop;
    bool Instr_kInstrInvalidateIsErr;
    bool Instr_kInstrPairDpfIsErr;
    bool Instr_kInstrDataDepShiftSupported;
    bool Instr_kInstrReverseSubtractSupported;

    // Control whether we initialise Time/Rand info on allocation Packets/Phvs
    bool Packet_kPktInitTimeRandOnAlloc;
    bool PhvFactory_kPhvInitTimeRandOnAlloc;
    bool PhvFactory_kPhvInitAllValid;

    // Snapshot configuration (do we take account of PHV valid bit on compare/capture)
    bool MauSnapshotCommon_kSnapshotCompareValidBit;
    bool MauSnapshotCommon_kSnapshotCaptureValidBit;
    bool MauSnapshotCommon_kSnapshotUsePhvTime;
    bool MauSnapshotCommon_kSnapshotMaskThreadFields;

    // Flags to turn on certain buggy behaviours
    bool ActionOutputHvXbar_kBugActionDataBusMaskBeforeDup;

    // Do we assert on RunStatefulInstructon to predicated *ON* Stateful ALU
    bool MauStatefulAlu_kRelaxSaluPredRsiCheck;

    // Flags to determine which index is returned for equal values in JBay SALU Max and Min instructions
    bool MauChipStatefulAlu_kMaxReturnsHighestIndex;
    bool MauChipStatefulAlu_kMinReturnsHighestIndex;

    // Relax Queueing checks
    bool Queueing_kAllowMissingDies;
  };


}
#endif // _UTESTS_TEST_CONFIG_
