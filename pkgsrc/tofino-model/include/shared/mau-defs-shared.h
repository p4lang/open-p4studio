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

#ifndef _SHARED_MAU_DEFS_SHARED_
#define _SHARED_MAU_DEFS_SHARED_

#include <rmt-defs.h>

namespace MODEL_CHIP_NAMESPACE {

enum CntrType {
  kDisabled       ,
  kTableMiss      ,
  kTableHit       ,
  kGatewayMiss    ,
  kGatewayHit     ,
  kGatewayInhibit ,
  kUnconditional  ,  // stateful logging address (when predicated on and per-flow enabled)
  // new for JBay
  kGatewayInhibitEntry0,
  kGatewayInhibitEntry1,
  kGatewayInhibitEntry2,
  kGatewayInhibitEntry3,
  kGatewayInhibitMiss
};


  class MauDefsShared {
    // *** Chip-specific changes should be made in include/<chip>/rmt-defs.h *

 public:

    // Definition of ALU group's bit width
    static const int kAlu_WidthPerGroup[];

    enum BusTypeEnum {
      kExactMatchBus = 0,
      kTindBus
    };

    // Return this data val for accesses to invalid columns
    static constexpr uint64_t kBadDataWord = UINT64_C(0x0BAD0BAD0BAD0BAD);
    // Whether SRAM code uses spinlocks
    static constexpr bool   kSramUseSpinlocks = true;

    static constexpr int    kTablesMin = 16;
    static constexpr int    kTablesMax = 16;
    static constexpr int    kTableBits = 4;
    static constexpr int    kTableMask = (1<<kTableBits)-1;
    static constexpr int    kLogicalTablesPerMau = kTablesMax;
    static constexpr int    kSramsPerMau = 8 * 12;
    static constexpr int    kSramRowsPerMau = 8;
    static constexpr int    kSramRowFirstTOP = kSramRowsPerMau / 2;
    static constexpr int    kSramColumnsPerMau = kSramsPerMau / kSramRowsPerMau;
    static constexpr int    kSramColumnFirstRHS = kSramColumnsPerMau / 2;
    static constexpr int    kLogicalRowsPerPhysicalRow = 2;
    static constexpr int    kLogicalRowsPerMau = kSramRowsPerMau * kLogicalRowsPerPhysicalRow;
    static constexpr int    kLogicalRowFirstTop = kLogicalRowsPerMau / 2;
    static constexpr int    kLogicalColumnsPerMau = kSramsPerMau / kLogicalRowsPerMau;
    static constexpr int    kSramAddressWidth = 10;
    static constexpr int    kSramWidth = 128;
    static constexpr int    kMapramsPerMau = 8 * 12;
    static constexpr int    kMapramRowsPerMau = 8;
    static constexpr int    kMapramRowFirstTOP = kMapramRowsPerMau / 2;
    static constexpr int    kMapramColumnsPerMau = kMapramsPerMau / kMapramRowsPerMau;
    static constexpr int    kMapramAddressWidth = 10;
    static constexpr int    kMapramWidth = 11;
    static constexpr int    kSearchBusesPerRow = 2;
    static constexpr int    kTcamsPerMau = 24;
    static constexpr int    kTcamRowsPerMau = 12;
    static constexpr int    kTcamColumnsPerMau = kTcamsPerMau / kTcamRowsPerMau;
    static constexpr int    kTcamAddressWidth = 9;
    static constexpr int    kTcamWidth = 44;
    static constexpr int    kTcamSearchBusesPerRow = 2;
    static constexpr int    kLogicalTcamsPerMau = 8;
    static constexpr int    kHitBitsPerRow = 1;
    static constexpr int    kOutBitsPerRow = 1;
    static constexpr int    kMatchOutputBusesPerRow = 2;
    static constexpr int    kMatchOutputBusesPerMau = kMatchOutputBusesPerRow * kSramRowsPerMau;
    static constexpr int    kMatchOutputBusWidth = 83;
    static constexpr int    kDataBusWidth = 128;
    static constexpr int    kActionOutputBusesPerLogicalRow = 1;
    static constexpr int    kActionOutputBusWidth = kDataBusWidth;
    static constexpr int    kActionHVOutputBusWidth = 1024;
    static constexpr int    kTindOutputBusesPerRow = 2;
    static constexpr int    kTindOutputBusesPerMau = kTindOutputBusesPerRow * kSramRowsPerMau;
    static constexpr int    kTindOutputBusWidth = 64;
    static constexpr int    kTableResultMatchAddrPos = 64;
    static constexpr int    kTableResultMatchAddrWidth = 19;
    static constexpr int    kTableResultBusWidth = (kMatchOutputBusWidth > kTindOutputBusWidth) ?kMatchOutputBusWidth :kTindOutputBusWidth;
    static constexpr int    kTableResultBusLsbPadWidth = 5;
    static constexpr int    kInstrs = 32;

    // Version
    static constexpr int    kVersionBits = 2;

    // Hash function
    static constexpr int    kHashInputBytes = 128;
    static constexpr int    kHashOutputWidth = 52;
    static constexpr int    kHashDistribSelectWidth = 48;
    static constexpr int    kHashletWidth = 16;
    static constexpr int    kHashDistribGroups = 6;
    static constexpr int    kHashGroups = 8;
    static constexpr int    kHashFirstStages = 16;
    // Top part of hash function used for masked equals and gateway tables
    static constexpr int    kMaskedEqualsWidth = 12;

    // Input xbar
    static constexpr int    kExactMatchBytes = 128;
    static constexpr int    kTernaryMatchBytes = 66;
    static constexpr int    kTotalMatchBytes = kExactMatchBytes + kTernaryMatchBytes;

    // TCAM array
    static constexpr int    kTernaryRowMatchBits = kTcamWidth;
    static constexpr int    kTernaryFirstByte = kExactMatchBytes;
    static constexpr int    kTernaryFirstBit = kTernaryFirstByte * 8;

    // Gateway tables
    static constexpr int    kGatewayTablesPerRow = 2;
    static constexpr int    kGatewayTableEntries = 4;
    static constexpr int    kGatewayTableExpressionAWidth = 24;
    static constexpr int    kGatewayTableHashExtractWidth = kMaskedEqualsWidth;
    static constexpr int    kGatewayTableExpressionBWidth = 32;
    static constexpr int    kGatewayPayloadsPerRow = 2;

    // Stash
    static constexpr int    kStashEntries = 4;

    // Idletime - for address distribution
    static constexpr int    kIdletimeBusesPerMau = 20;



    // DEFS ABOVE HERE WERE PREVIOUSLY IN RmtDefs
    static constexpr int kExactMatchInputBits = kExactMatchBytes * 8;
    static constexpr int kExactMatchValidBits = kExactMatchBytes;
    static constexpr int kTernaryMatchInputBits = kTernaryMatchBytes * 8;
    static constexpr int kTernaryMatchValidBits = kTernaryMatchBytes;
    static constexpr int kTotalMatchInputBits = kTotalMatchBytes * 8;
    static constexpr int kTotalMatchValidBits = kTotalMatchBytes;
    static constexpr int kNextTableWidth = 8;
    static constexpr int kMatchesPerSram = 5;
    static constexpr int kVpnsPerSram = 6;
    static constexpr int kVpnsPerMau = 512;
    static constexpr int kMaxStatsEntriesPerWord = 7; // Possible to use entries 0..6
    static constexpr int kNumBanksPerDeferredRam = 1;
    static constexpr int kNumEopAddrsPerDeferredRamBank = 160;
    static constexpr int kDeferredRamRows = 4;
    static constexpr int kDeferredRamColumns = kNumBanksPerDeferredRam;
    static constexpr int kNumEopAddrs = kNumBanksPerDeferredRam*kNumEopAddrsPerDeferredRamBank;

    // Sram column mask - now lost col0
    // Hyperdev transition 07092015 also lost col1!!
    static constexpr int kSramColumns = kSramColumnsPerMau;
    static constexpr int kSramValidAllColumnMask = (1 << kSramColumns) - 1;
    static constexpr int kSramValidColumnMask = kSramValidAllColumnMask & ~0x3;
    //static constexpr int kSramValidColumnMask = kSramValidAllColumnMask;

    // Mapram column mask (only RHS columns since regs_13957_mau_dev)
    static constexpr int kMapramColumns = kMapramColumnsPerMau;
    static constexpr int kMapramHalfColumns = kMapramColumns/2;
    static constexpr uint32_t kMapramHalfColumnsMask = (1u << kMapramHalfColumns) - 1;
    static constexpr uint32_t kMapramLhsColumns = kMapramHalfColumnsMask;
    static constexpr uint32_t kMapramRhsColumns = kMapramHalfColumnsMask << kMapramHalfColumns;
    static constexpr uint32_t kMapramValidColumnMask = kMapramRhsColumns;
    // Other Mapram defs
    static constexpr int      kMapramVpnBits = 6;
    static constexpr uint8_t  kMapramVpnMask = (1<<kMapramVpnBits)-1;
    static constexpr uint8_t  kMapramVpnMax  = (1<<kMapramVpnBits)-1;
    static constexpr uint8_t  kMapramVpnUnoccupied = kMapramVpnMax;

    // ALU logical row masks and limits
    static constexpr uint32_t kStatsAluLogicalRows = (1u<<13)|(1u<<9)|(1u<<5)|(1u<<1);
    static constexpr uint32_t kMeterAluLogicalRows = (1u<<15)|(1u<<11)|(1u<<7)|(1u<<3);
    static constexpr uint32_t kNumAlus = 4;
    static constexpr uint32_t kNumStatsAlus = kNumAlus;
    static constexpr uint32_t kNumMeterAlus = kNumAlus;
    static constexpr uint32_t kNumStatefulAlus = kNumMeterAlus;
    static constexpr uint32_t kSelectorAluInvalOutput = 0x80000000; // Force fallback addr
    static constexpr uint32_t kSelectorAluLogicalRows = kMeterAluLogicalRows;
    static constexpr uint32_t kStatefulAluLogicalRows = kMeterAluLogicalRows;
    static constexpr int      kMaxConsecDataOverflows = 5; // ALU can only use 6 rows of SRAMs so 5
    static constexpr uint32_t kNumStatefulCounters = 4;
    static constexpr uint32_t kNumColorBuses = 4;

    // MeterOPs allowed at hdrtime for each MeterALU variant (OP4 vals 0-15 defined in address.h)
    static constexpr uint32_t kMeterAluHdrtimeMeterOps = 0x00BEu;
    static constexpr uint32_t kMeterAluHdrtimeLpfOps   = 0x00BEu;
    static constexpr uint32_t kMeterAluHdrtimeSaluOps  = 0x3AFEu;
    static constexpr uint32_t kMeterAluHdrtimeSelOps   = 0x8014u;

    // MAU Sweeper defs
    static constexpr int       kOneTickShift     = 21;
    static constexpr uint64_t  kOneTickCycles    = UINT64_C(1) << kOneTickShift;
    static constexpr uint64_t  kOneCyclePicosecs = UINT64_C(1000000000000) / RmtDefs::kRmtClocksPerSec;
    static constexpr uint64_t  kOneTickPicosecs  = kOneTickCycles * kOneCyclePicosecs;
    static constexpr uint8_t   kMaxInterval = 31;  // 41.7 days
    static constexpr int       kTimestampBitPos  = 100; // Pos in Meter/Lpf word
    static constexpr int       kTimestampWidth   = 28;  // Width in Meter/Lpf word

    // SRAM Types
    static constexpr int kSramTypeInvalid  = 0;
    static constexpr int kSramTypeMin      = 1;
    static constexpr int kSramTypeMatch    = 1;
    static constexpr int kSramTypeAction   = 2;
    static constexpr int kSramTypeStats    = 3;
    static constexpr int kSramTypeMeter    = 4;
    static constexpr int kSramTypeStateful = 5;
    static constexpr int kSramTypeTind     = 6;
    static constexpr int kSramTypeSelector = 7;
    static constexpr int kSramTypeMax      = 7;

    // MapRAM Types
    static constexpr int kMapramTypeInvalid  = 0;
    static constexpr int kMapramTypeMin      = 1;
    static constexpr int kMapramTypeStats    = 1;
    static constexpr int kMapramTypeMeter    = 2;
    static constexpr int kMapramTypeStateful = 3;
    static constexpr int kMapramTypeIdletime = 4;
    static constexpr int kMapramTypeColor    = 5;
    static constexpr int kMapramTypeSelector = 6;
    static constexpr int kMapramTypeMax      = 6;

    // Row bus types
    static constexpr int kRowBusTypeInvalid  = 0;
    static constexpr int kRowBusTypeStats    = 1;
    static constexpr int kRowBusTypeMeter    = 2;
    static constexpr int kRowBusTypeAction   = 3;
    static constexpr int kRowBusTypeOflow    = 4;
    static constexpr int kRowBusTypeOflow2   = 5;

    // ALU types
    static constexpr int kAluTypeInvalid  = 0;
    static constexpr int kAluTypeStats    = 1;
    static constexpr int kAluTypeMeter    = 2;
    static constexpr int kAluTypeStateful = 3;
    static constexpr int kAluTypeSelector = 4;

    // ALU dynamic features (determined by config)
    static constexpr uint32_t kMauMeterAluSelectorPresent         = 0x000001u;
    static constexpr uint32_t kMauMeterAluStatefulPresent         = 0x000002u;
    static constexpr uint32_t kMauMeterAluMeterLpfPresent         = 0x000004u;
    static constexpr uint32_t kMauMeterAluMeterPresent            = 0x000008u;
    // These next three introduced on JBay
    static constexpr uint32_t kMauMeterAluRightActionOverrideUsed = 0x000010u;
    static constexpr uint32_t kMauMeterAluStatefulDivideUsed      = 0x000040u;
    static constexpr uint32_t kMauMeterAluStatefulQlagUsed        = 0x000080u;

    // ... and MAU dynamic features (determined by config)
    static constexpr uint32_t kMauTcamPresent                     = 0x000100u;
    static constexpr uint32_t kMauTindPresent                     = 0x000200u;
    static constexpr uint32_t kMauWideSelectorPresent             = 0x001000u;

    // Temporary MAU dependency-related features only used on calls
    // to MauDelay::stage_* functions
    static constexpr uint32_t kMauCurrStageMatchDep               = 0x010000u;
    static constexpr uint32_t kMauCurrStageActionDep              = 0x020000u;
    static constexpr uint32_t kMauCurrStageIsMau0                 = 0x080000u;
    static constexpr uint32_t kMauNextStageMatchDep               = 0x100000u;
    static constexpr uint32_t kMauNextStageActionDep              = 0x200000u;


    // Some feature checking funcs
    static inline bool tcam_present(uint32_t features) {
      return (((features & kMauTcamPresent) != 0u) ||
              ((features & kMauTindPresent) != 0u) ||
              ((features & kMauWideSelectorPresent) != 0u));
    }
    static inline bool selector_present(uint32_t features) {
      return ((features & kMauMeterAluSelectorPresent) != 0u);
    }
    static inline bool meter_lpf_present(uint32_t features) {
      return ((features & kMauMeterAluMeterLpfPresent) != 0u);
    }
    static inline bool stateful_present(uint32_t features) {
      return ((features & kMauMeterAluStatefulPresent) != 0u);
    }
    static inline bool divide_used(uint32_t features) {
      return ((features & kMauMeterAluStatefulDivideUsed) != 0u);
    }
    static inline bool qlag_used(uint32_t features) {
      return ((features & kMauMeterAluStatefulQlagUsed) != 0u);
    }
    static inline bool right_action_override_used(uint32_t features) {
      return ((features & kMauMeterAluRightActionOverrideUsed) != 0u);
    }
    static inline bool stateful_divide(uint32_t features) {
      return stateful_present(features) && divide_used(features);
    }
    static inline bool stateful_qlag(uint32_t features) {
      return stateful_present(features) && qlag_used(features);
    }

    static inline bool curr_stage_match_dep(uint32_t features) {
      return ((features & kMauCurrStageMatchDep) != 0u);
    }
    static inline bool curr_stage_action_dep(uint32_t features) {
      return ((features & kMauCurrStageActionDep) != 0u);
    }
    static inline bool curr_stage_is_mau0(uint32_t features) {
      return ((features & kMauCurrStageIsMau0) != 0u);
    }
    static inline bool next_stage_match_dep(uint32_t features) {
      return ((features & kMauNextStageMatchDep) != 0u);
    }
    static inline bool next_stage_action_dep(uint32_t features) {
      return ((features & kMauNextStageActionDep) != 0u);
    }



    // MAU error codes which may be put into PHV
    static constexpr uint32_t kErrMauTcamErrorDetected = 0x00000001;
    // Parser/Deparser error codes which may be put into PHV



    MauDefsShared()  {}
    ~MauDefsShared() {}

    static constexpr bool is_match_type(int type) { return (type == kSramTypeMatch); }
    static constexpr bool is_tind_type(int type)  { return (type == kSramTypeTind); }
  };
}

#endif // _SHARED_MAU_DEFS_SHARED_
