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

#ifndef _TOFINOXX_MAU_DEFS_
#define _TOFINOXX_MAU_DEFS_

#include <mau-defs-shared.h>

namespace MODEL_CHIP_NAMESPACE {

  class MauDefs : public MauDefsShared {

 public:
    // MAU extractor definitions
    static constexpr int      kActionInstrWidth = 7;
    // MAU VLIW definitions
    static constexpr int      kInstrAluGrpSize = 16;
    static constexpr int      kInstrSrc1Subtract = 24;
    static constexpr int      kInstrSrc1Width = 5;
    static constexpr int      kInstrSrc2Width = 4;
    static constexpr uint32_t kInstrOperand1OnlyAlus = 0x00000000u;
    static constexpr uint32_t kInstrOperand2OnlyAlus = 0x00000000u;

    // Number of cycles for color to get from meter to actually written into into mapram
    static constexpr uint64_t kMapramColorWriteLatency     = 16;
    static constexpr uint64_t kMapramColorWriteLatencyEOP  = 16;
    static constexpr uint64_t kMapramColorWriteLatencyTEOP = 16;

    // The size of the data bus from input vh xbar to meter alus
    static constexpr int      kStatefulMeterAluDataBytes = 8;
    static constexpr int      kStatefulMeterAluDataBits  = kStatefulMeterAluDataBytes*8;

    // Only one stateful event on Tofino (used to Stateful logging)
    static constexpr int      kNStatefulEvents = 1;
    static constexpr int      kStatefulAluCmpAlus = 2;
    static constexpr int      kStatefulAluTMatchAlus = 0;
    static constexpr int      kStatefulAluOutputAlus = 1;
    static constexpr int      kStatefulAluCmpConstWidth = 4;
    static constexpr int      kStatefulAluUsesRandomNumber = 0;
    static constexpr int      kMeterOp4SaluClearValue = 999;    // there is no clear in tofino
    static constexpr bool     kMeterClearOveridesSweep = false; // there is no clear in tofino

    // General MAU delay definitions (see uArch section 3)
    static constexpr int      kMauBaseDelay                              = 20;
    static constexpr int      kMauBasePredicationDelay                   = 11;
    static constexpr int      kMauTcamExtraDelay                         =  2;
    static constexpr int      kMauMeterAluSelectorExtraDelay             =  8;
    static constexpr int      kMauMeterAluMeterLpfExtraDelay             =  4;
    static constexpr int      kMauMeterAluStatefulExtraDelay             =  4;
    static constexpr int      kMauMeterAluStatefulDivideExtraDelay       =  0;
    static constexpr int      kMeterAluGroupDataDelayBase                = 14;

    // Configs specifically required for various delay configuration CSRs:
    //
    // 1. MeterAluGroupActDelay
    // Adb => ALU output goes direct to ADB (this one only applies to right_alu_action_delay)
    // Div => Stateful ALU using divide in stage series (can not happen on TofinoXX)
    // Sel => Selector present in stage series
    // Tack Adb/Div/Sel sufffix onto kMeterAluGroupActDelay_ to find correct delay
    // NOTE: Adb/Div only defined to allow linking - these features *not available* on TofinoXX
    // (value 99 used just in case)
    static constexpr int      kMeterAluGroupActDelay                     =  0;
    static constexpr int      kMeterAluGroupActDelay_Adb                 = 99;
    static constexpr int      kMeterAluGroupActDelay_Div                 = 99;
    static constexpr int      kMeterAluGroupActDelay_DivEn               = 99;
    static constexpr int      kMeterAluGroupActDelay_Sel                 =  4;
    static constexpr int      kMeterAluGroupActDelay_Sel_Div             = 99;
    static constexpr int      kMeterAluGroupActDelay_Sel_DivEn           = 99;
    static constexpr int      kMeterAluGroupActDelay_Sel_Adb             = 99;
    // 2. DeferedEopBus
    static constexpr int      kDeferredEopBusInternalExtraDelay          =  3;
    static constexpr int      kDeferredEopBusOutputMatchDepExtraDelay    = -1;
    static constexpr int      kDeferredEopBusOutputActionDepDelay        =  1;
    // 3. StartTableFifo
    static constexpr int      kMauMatchDepStartTableFifo0ExtraDelayMau0  = -1;
    static constexpr int      kMauMatchDepStartTableFifo0ExtraDelayMauN  = -1;
    static constexpr int      kMauActionDepStartTableFifo0Delay          =  1;

    // Teop defs
    static constexpr int kNumTeopBuses = 4;

    // Bits correspond to valid indices in MauStatsAlu::kStatsAlu_Config table
    // (here formats 10,12,18,20,22,25,26,27 are valid)
    static constexpr uint32_t valid_stats_modes      = 0x0E541400u;

    // Bits correspond to valid values for MapramConfig::idletime_bitwidth()
    // (here values 0,1,2,3 indicating widths 1,2,3,6 are valid)
    static constexpr uint32_t valid_idletime_modes   = 0x0000000Fu;

    // Bits correspond to valid values cur_stage_dep/next_stage_dep
    // (here values 0,1,2 indicating MatchDep/ActionDep/Concurrent are valid)
    static constexpr uint32_t valid_dependency_modes = 0x00000007u;


    MauDefs()          {}
    virtual ~MauDefs() {}

  };
}

#endif // _TOFINOXX_MAU_DEFS_
