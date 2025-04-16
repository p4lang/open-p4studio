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

#ifndef _SHARED_RMT_DEFS_SHARED_
#define _SHARED_RMT_DEFS_SHARED_

#if WITH_DEBUGGER
#include <libP4Debugger.h>
#endif


namespace MODEL_CHIP_NAMESPACE {

  struct PortConfig {
    uint64_t  speed_bits_per_sec;
    uint32_t  hdr_word0;
    uint32_t  hdr_word1;
    bool      enabled;
    uint16_t  logical_port_index;
    int       pipe_index;
    int       mac_index;
    int       mac_chan;
    int       parser_index;
    int       parser_chan;
    int       ipb_index;
    int       epb_index;
    int       ipb_chan;
    int       epb_chan;
  };
  struct ParserConfig {
    int       dummy;
  };
  struct PipeConfig {
    int       dummy;
  };
  struct MauConfig {
    int       dummy;
  };




  class RmtDefsShared {
    // *** Chip-specific changes should be made in include/<chip>/rmt-defs.h ***

 public:
    static const uint32_t       kPhv_MaskPerGroup[];
    static const int            kPhv_WidthPerGroup[];
    static const int            kPhv_ModulusPerGroup[];
    static const int            kPhv_Map_Rel8_Abs[];
    static const int            kPhv_Map_Rel16_Abs[];
    static const int            kPhv_Map_Rel32_Abs[];
    static const int            kPhv_Map_Abs_Rel8[];
    static const int            kPhv_Map_Abs_Rel16[];
    static const int            kPhv_Map_Abs_Rel32[];
    static const PortConfig     kPort_Config_Default[];
    static const ParserConfig   kParser_Config[];
    static const PipeConfig     kPipe_Config[];
    static const MauConfig      kMau_Config[];


    // Whether ALL tables are evaluated by default
    static constexpr bool   kEvaluateAllDefault = false;
    // Whether TCAM code uses spinlocks
    // (keep this here as Parser uses TCAMs too)
    static constexpr bool   kTcamUseSpinlocks = true;

    // Some index mapping functions
    static int map_index_identity(const int i) { return i; }
    static int map_index_16_20(const int i)    { return ( ((i / 16) * 20) + (i % 16) ); }
    static int map_index_20_16(const int i)    { return ( ((i / 20) * 16) + (i % 20) ); }

    // Pipe/Parser/MAU hi-level defs - DONT OVERRIDE THESE
    static constexpr int    kPipesMaxEver = 4;
    static constexpr int    kStagesMaxEver = 32;
    // Other Pipe/Parser/MAU hi-level defs
    static constexpr int    kParserPriorities = 8;
    static constexpr int    kParserInputBufferSize = 32;
    static constexpr int    kParserStates = 256;
    static constexpr int    kParserMaxStates = kParserStates * 3;
    static constexpr int    kParserCounterInitAddrWidth = 4;
    static constexpr int    kParserCounterInitEntries = 1 << kParserCounterInitAddrWidth;
    static constexpr int    kParserCounterInitAddrMask = kParserCounterInitEntries - 1;
    static constexpr int    kParserChecksumEntries = 32;
    static constexpr int    kPortsTotal = 512; // Only ports 0-69 128-197 256-325 384-453 enabled typically

    static constexpr int    kTmQueueOccupancyWidth = 19;
    static constexpr int    kTmQueueOccupancyMask = (1 << kTmQueueOccupancyWidth) - 1;
    static constexpr int    kTmQueueMaxPackets = 512;
    static constexpr int    kTmMaxPackets = 8192;

    // Packet Generator
    static constexpr int    kPktGenPortsPerPipe = 2;
    static constexpr int    kDefaultRecirCount = 2;
    static constexpr int    kPktGenApps = 8;
    static constexpr int    kPktGenTriggers = 4;
    static constexpr int    kPktGenBufSz = 16384;
    static constexpr int    kPktGenRecirEntries = 32;
    static constexpr int    kPktGenPgenEntries = 8;
    static constexpr int    kMacPortsPerPipe = 16;
    static constexpr int    kMacCPUPort = 64;
    static constexpr int    kNormalType   = 0;
    static constexpr int    kRecircType = 1;
    static constexpr int    kPGenType   = 2;
    static constexpr unsigned int kIpbSz = 32768;

    // Deparser metadata widths
    static constexpr int    kDeparserCopyToCpuCosWidth = 3;
    static constexpr int    kDeparserDropCtlWidth = 3;
    static constexpr int    kDeparserHashLAGECMPMcastWidth = 13;
    static constexpr int    kDeparserIcosWidth = 3;
    static constexpr int    kDeparserMeterColorWidth = 2;
    static constexpr int    kDeparserEgressMulticastGroupWidth = 16;
    static constexpr int    kDeparserRidWidth = 16;
    static constexpr int    kDeparserXidWidth = 16;
    static constexpr int    kDeparserYidWidth = 9;
    static constexpr int    kDeparserEcosWidth = 3;

    // This is here because in tofino the metadata is 16 bits, but
    //  the table was halved, so only 32768 will be used
    static constexpr int    kDeparserMulticastGroupIds = 32768;

    // Mirroring buffers
    static constexpr int    kMirrorBufferCoalSessions = 8;
    static constexpr int    kMirrorBufferSessions = 1024;
    static constexpr int    kMirrorBufferCoalHeaderBytes = 16;
    static constexpr int    kMirrorsPerPacket = 0;

    // Bridge metadata
    static constexpr int    kBridgeMetadataBytes = 0;



#if WITH_DEBUGGER
    static constexpr int      kP4dChipShift = 56;
    static constexpr int      kP4dTypeShift = 52;
    static constexpr uint64_t kP4dTypePipe  = 0;
    static constexpr int      kP4dPipeIdShift = 50;
    static constexpr int      kP4dPipeStageShift = 46;
    static constexpr int      kP4dPipeTypeShift = 42;
    static constexpr uint64_t kP4dPipeTypePhv   = 0;
    static constexpr int      kP4dPipePhvIdxShift = 0;
    static register_handle_t  p4dHandle_phv(uint64_t chip, uint64_t pipe, uint64_t stage, uint64_t word) {
                                  return (chip << kP4dChipShift) |
                                         (kP4dTypePipe << kP4dTypeShift) |
                                         (pipe << kP4dPipeIdShift) |
                                         (stage << kP4dPipeStageShift) |
                                         (kP4dPipeTypePhv << kP4dPipeTypeShift) |
                                         (word << kP4dPipePhvIdxShift); }
#endif // WITH_DEBUGGER


    RmtDefsShared()          { }
    virtual ~RmtDefsShared() { }

  };
}

#endif // _SHARED_RMT_DEFS_SHARED_
