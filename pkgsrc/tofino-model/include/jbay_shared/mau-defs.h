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

#ifndef _JBAY_MAU_DEFS_
#define _JBAY_MAU_DEFS_

#include <mau-defs-shared.h>

namespace MODEL_CHIP_NAMESPACE {

  class MauDefs : public MauDefsShared {

 public:
    // MAU extractor definitions (instrs 1b wider to allow for bitmask ops)
    static constexpr int      kActionInstrWidth = 8;
    // MAU VLIW definitions
    static constexpr int      kInstrAluGrpSize = 20;
    static constexpr int      kInstrSrc1Subtract = 24;
    static constexpr int      kInstrSrc1Width = 5;
    static constexpr int      kInstrSrc2Width = 5;
    static constexpr uint32_t kInstrOperand1OnlyAlus = 0x0000F000u; // 11-15
    static constexpr uint32_t kInstrOperand2OnlyAlus = 0x000F0000u; // 16-19

    // Number of cycles for color to get from meter to actually written into into mapram
    static constexpr uint64_t kMapramColorWriteLatency     = 15;
    static constexpr uint64_t kMapramColorWriteLatencyEOP  = 15;
    static constexpr uint64_t kMapramColorWriteLatencyTEOP = 15;

    // The size of the data bus from input vh xbar to meter alus
    static constexpr int      kStatefulMeterAluDataBytes = 16;
    static constexpr int      kStatefulMeterAluDataBits  = kStatefulMeterAluDataBytes*8;
    // Two stateful events in JBay, one for Push, one for Pop.
    static constexpr int      kNStatefulEvents = 2;
    static constexpr int      kStatefulAluCmpAlus = 4;
    static constexpr int      kStatefulAluTMatchAlus = 2;
    static constexpr int      kStatefulAluOutputAlus = 4;
    static constexpr int      kStatefulAluCmpConstWidth = 6;
    static constexpr int      kStatefulAluUsesRandomNumber = 1;
    static constexpr int      kMeterOp4SaluClearValue = 0x6; // 4 bit opcode for JBay
    static constexpr bool     kMeterClearOveridesSweep = true; // as clear and sweep use same opcode

    // General MAU delay definitions (see uArch section 3)
    static constexpr int      kMauBaseDelay                              = 23;
    static constexpr int      kMauBasePredicationDelay                   = 13;
    static constexpr int      kMauTcamExtraDelay                         =  2;
    static constexpr int      kMauMeterAluSelectorExtraDelay             =  8;
    static constexpr int      kMauMeterAluMeterLpfExtraDelay             =  4;
    static constexpr int      kMauMeterAluStatefulExtraDelay             =  4;
    static constexpr int      kMauMeterAluStatefulDivideExtraDelay       =  2;
    static constexpr int      kMeterAluGroupDataDelayBase                = 16;

    // Configs specifically required for various delay configuration CSRs:
    //
    // 1. MeterAluGroupActDelay
    // Adb => ALU output goes direct to ADB (this one only applies to right_alu_action_delay)
    // Div => Some Stateful ALU in stage series using divide (DivEn==>ALU using divide too)
    // Sel => Selector present in stage series
    // Tack Adb/Div/Sel sufffix onto kMeterAluGroupActDelay_ to find correct delay
    static constexpr int      kMeterAluGroupActDelay                     =  0;
    static constexpr int      kMeterAluGroupActDelay_Adb                 =  2;
    static constexpr int      kMeterAluGroupActDelay_Div                 =  2;
    static constexpr int      kMeterAluGroupActDelay_DivEn               =  0;
    static constexpr int      kMeterAluGroupActDelay_Sel                 =  4;
    static constexpr int      kMeterAluGroupActDelay_Sel_Div             =  4; // uArch 0.9
    static constexpr int      kMeterAluGroupActDelay_Sel_DivEn           =  2;
    static constexpr int      kMeterAluGroupActDelay_Sel_Adb             =  6;
    // 2. DeferedEopBus
    static constexpr int      kDeferredEopBusInternalExtraDelay          =  2;
    static constexpr int      kDeferredEopBusOutputMatchDepExtraDelay    = -2;
    static constexpr int      kDeferredEopBusOutputActionDepDelay        =  1;
    // 3. StartTableFifo
    static constexpr int      kMauMatchDepStartTableFifo0ExtraDelayMau0  = -2;
    static constexpr int      kMauMatchDepStartTableFifo0ExtraDelayMauN  = -3;
    static constexpr int      kMauActionDepStartTableFifo0Delay          =  0;

    // Teop defs
    static constexpr int kNumTeopBuses = 4;

    // Bits correspond to valid indices in MauStatsAlu::kStatsAlu_Config table
    // (here formats 10,12,18,20,25,26 are valid - NO 3/6 entry modes)
    static constexpr uint32_t valid_stats_modes      = 0x06141400u;

    // Bits correspond to valid values for MapramConfig::idletime_bitwidth()
    // (here values 0,2,3 indicating widths 1,3,6 are valid - NO 2b idletime)
    static constexpr uint32_t valid_idletime_modes   = 0x0000000Du;

    // Bits correspond to valid values cur_stage_dep/next_stage_dep
    // (here values 0,1 indicating MatchDep/ActionDep are valid - NO concurrent)
    static constexpr uint32_t valid_dependency_modes = 0x00000003u;

    // RED per flow disable introduced in JBay
    static constexpr int kLpfRedDisableBit       = 62;


    MauDefs()          {}
    virtual ~MauDefs() {}

  };
}

#endif // _JBAY_MAU_DEFS_
