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

#ifndef _TOFINOXX_RMT_DEFS_TOFINOXX_
#define _TOFINOXX_RMT_DEFS_TOFINOXX_

#include <model_core/model.h>
#include <rmt-defs-shared.h>

namespace MODEL_CHIP_NAMESPACE {

  class RmtDefsTofinoXX : public RmtDefsShared {
 public:
    static constexpr int     kSkuDefault = 0;

    static constexpr int     kPhvWordsMax                 = 224;
    static constexpr int     kPhvWordsMaxExtended         = 368;
    static constexpr int     kPhvWordsMaxUnmapped         = 224;
    static constexpr int     kPhvWordsMaxExtendedUnmapped = 368;
    static constexpr int     kPhvWordsPerGroupUnmapped    =  32;
    static constexpr int     kPhvWordsPerGroup            =  32;
    static int map_mau_phv_index(const int phv_index)       { return phv_index; }
    static int map_maumixbar_phv_index(const int phv_index) { return phv_index; }
    static int map_mausnap_phv_index(const int phv_index)   { return phv_index; }
    static int unmap_mausnap_phv_index(const int phv_index) { return phv_index; }
    static int map_prsr_phv_index(const int phv_index)      { return phv_index; }
    static int map_dprsr_phv_index(const int phv_index)     { return phv_index; }

    static constexpr int     kSkuMin = 0;
    static constexpr int     kSkuMax = 34;
    static constexpr int     kPipesMin = 4;
    static constexpr int     kPipesMax = 4;
    static constexpr int     kPipeBits = 2;
    static constexpr int     kPipeMask = (1<<kPipeBits)-1;
    static constexpr int     kStagesMin = 12;
    static constexpr int     kStagesMax = 12;
    static constexpr int     kStageBits = 4;
    static constexpr int     kStageMask = (1<<kStageBits)-1;
    static constexpr int     kPresTotal = 4;
    static constexpr int     kParsers = 18;
    static constexpr int     kParserChannels = 4;
    static constexpr int     kPhysMacs = 16;
    static constexpr int     kPortGroupsPerPipe = 18;
    static constexpr int     kPortsPerPortGroup = 4;
    static constexpr int     kQidsPerPortGroup = 32;
    static constexpr int     kPortsPerPipe = kPortsPerPortGroup * kPortGroupsPerPipe;
    static constexpr int     kTmPortGroupsPerPipe = 18;
    static constexpr int     kTmPortsPerPortGroup = 4;
    static constexpr int     kTmPortsPerPipe = kTmPortsPerPortGroup * kTmPortGroupsPerPipe;
    static constexpr int     kMacChannelMax = 4; // ComiraUmac3
    static constexpr int     kIpbs = 18;
    static constexpr int     kEpbs = 18;
    static constexpr int     kTmPipesMax = 4;

    // Elements of port ids (assumes no gaps)
    static constexpr int     kPortChanWidth = 2;       // port[1:0]
    static constexpr int     kPortGroupWidth = 5;      // port[6:2]
    static constexpr int     kPortPipeWidth = 2;       // port[8:7]
    static constexpr int     kPortDieWidth = 0;        // n/a for tofinoXX
    static constexpr int     kPortParserChanWidth = 2; // port[1:0]
    static constexpr int     kPortParserWidth = 5;     // port[6:2]
    static constexpr int     kPortIpbChanWidth = 2;    // port[1:0] 1-1 Parser-IPB
    static constexpr int     kPortIpbWidth = 5;        // port[6:2] 1-1 Parser-IPB
    static constexpr int     kPortEpbChanWidth = 2;    // port[1:0] 1-1 Parser-EPB
    static constexpr int     kPortEpbWidth = 5;        // port[6:2] 1-1 Parser-EPB
    static constexpr int     kPortWidth = 9;           // port[8:0]
    static_assert( (kPortPipeWidth >= kPipeBits), "Insufficient pipe bitwidth in port ids");
    static constexpr int     kIngressLocalPortShift = 0; // these 5 n/a for Tofino
    static constexpr int     kPipeTmLocalPortShift = 0;
    static constexpr int     kTmPipeLocalPortShift = 0;
    static constexpr int     kEgressLocalPortShift = 0;
    static constexpr int     kMacChanShift = 0;

    // We create 4 pipes in TofinoXX and all have independent MAUs
    static int map_mau_pipe(int pipe, uint32_t enabled_pipes) { return pipe; }
    // And each has its own LearningFilter
    static int map_lfltr_pipe(int pipe, uint32_t enabled_pipes) { return pipe; }

    // IPB defns
    static constexpr int     kChannelsPerIpb = 4;
    static constexpr int     kIpbHeaderSizeBytes = 16;
    static constexpr int     kIpbMeta0LogicalPortWidth = 9;
    static constexpr int     kIpbMeta0SizeBytes = 8;
    static constexpr int     kIpbMeta1SizeBytes = 8;
    static constexpr int     kIpbMeta1ResubSizeBytes = 8;
    static constexpr int     kChlsPerIpb = kChannelsPerIpb;
    static_assert( (1<<kPortIpbChanWidth == kChannelsPerIpb),
                   "Expected IPB channels to use ALL Port IPB bits" );

    // EPB defns
    static constexpr int     kChannelsPerEpb = 4;
    static_assert( (1<<kPortEpbChanWidth == kChannelsPerEpb),
                   "Expected EPB channels to use ALL Port EPB bits" );

    // Parser defns
    static constexpr int     kParserCheckFlags = 0x000F;
    static constexpr int     kParserExtractFlags = 0x0000;
    static constexpr int     kParserExtract8bWordsPerCycle = 4;
    static constexpr int     kParserExtract16bWordsPerCycle = 4;
    static constexpr int     kParserExtract32bWordsPerCycle = 4;
    static constexpr int     kParserFirstByteInbuf2 = 54;
    static constexpr int     kParserChecksumEngines = 2;
    static constexpr int     kParserChecksumRams = 2;
    static constexpr int     kParserCounterStackSize = 0;
    static constexpr int     kParserGroupShift = 0; // Derive parser 'group' (ie IBP/EBP) from parser number
    static int get_parser_group(int parser_num) { return parser_num >> kParserGroupShift; }

    // CLOT config - used by Parser and Deparser
    static constexpr int     kClotMinGap = -1;
    static constexpr int     kClotMaxLen = 0;
    static constexpr int     kClotMaxOffset = 0;
    static constexpr int     kClotMaxLenPlusOffset = 0;
    static constexpr uint8_t kClotMaxSimultaneousTags = 0;
    static constexpr int     kClotTagWidth = 0;
    static constexpr uint8_t kClotMaxTag = (1<<kClotTagWidth)-1;

    // Deparser metadata widths
    static constexpr int     kDeparserEgressUnicastPortWidth = kPortWidth;
    static constexpr int     kDeparserQidWidth = 5;
    static constexpr int     kDeparserXidL2Width = 9;
    static constexpr int     kDeparserMirrEpipePortWidth = kPortWidth;  // compatibility only
    static constexpr int     kDeparserPortVectorWidth = 4; // bits

    // I2Queueing definitions
    static constexpr uint8_t kI2qQidWidth = 5;
    static constexpr uint8_t kI2qQidMask = (1 << kI2qQidWidth) - 1;
    static constexpr uint8_t kI2qMulticastPipeVectorMask = 0xF;

    // Mirroring
    static constexpr int    kMirrorSlices         =    1;
    static constexpr int    kMirrorNormalSessions = 1024;
    static constexpr int    kMirrorCoalSessions   =    8;

    // Learn filter
    static constexpr int    kLearnQuantumWidth = 384;
    static constexpr int    kLearnFilterNumBloomFilter = 2;
    static constexpr int    kLearnFilterNumHash = 4;
    static constexpr int    kLearnFilterHashIndexBits = 14;
    static constexpr int    kLearnFilterHashTableSize = (1<<kLearnFilterHashIndexBits);
    static constexpr int    kLearnFilterLqBufferSize = 2048;

    // Miscellaneous1: Reset control values and status masks
    static constexpr uint32_t kSoftResetCtrlMask     = 0x0040u;
    static constexpr uint32_t kSoftResetCtrlVal      = 0x0040u;

    static constexpr uint32_t kSoftResetStatusMask   = 0x0039u;
    static constexpr uint32_t kSoftResetStatusValOk  = 0x0001u;
    static constexpr uint32_t kSoftResetStatusValErr = 0x0000u;

    // Packet Generator
    static constexpr int    kPktGen_P16 = 16;
    static constexpr int    kPktGen_P17 = 17;
    static constexpr int    kPktGenApps = 8;

    //
    // Address Manipulation
    //

    // Shifts and masks to get the address type from an address.
    static constexpr int      kIndPipeTypeShift = 41;
    static constexpr int      kDirPipeTypeShift = 25;
    static constexpr uint64_t kPipeTypeMask  = 1;
    static constexpr uint64_t kPipeTypeValue = 1;
    static constexpr int      kIndDevSelTypeShift = 39;
    static constexpr int      kDirDevSelTypeShift = 23;
    static constexpr uint64_t kDevSelTypeMask  = 7;
    static constexpr uint64_t kDevSelTypeValue = 0;
    static constexpr int      kIndMacTypeShift = 39;
    static constexpr int      kDirMacTypeShift = 23;
    static constexpr uint64_t kMacTypeMask  = 7;
    static constexpr uint64_t kMacTypeValue = 2;
    static constexpr int      kIndSerdesTypeShift = 39;
    static constexpr int      kDirSerdesTypeShift = 23;
    static constexpr uint64_t kSerdesTypeMask  = 7;
    static constexpr uint64_t kSerdesTypeValue = 3;
    // Shifts and masks to check if an indirect address is a register or not.
    static constexpr int      kIndPipeRegTypeShift = 30;
    static constexpr uint64_t kIndPipeRegTypeMask  = 3;
    static constexpr uint64_t kIndPipeRegTypeValue = 0;
    static constexpr int      kIndDevSelRegTypeShift = 18; //33:18 zero
    static constexpr uint64_t kIndDevSelRegTypeMask  = 0xFFFF;
    static constexpr uint64_t kIndDevSelRegTypeValue = 0;
    static constexpr int      kIndMacRegTypeShift = 17; // 31:17 zero
    static constexpr uint64_t kIndMacRegTypeMask  = 0x7FFF;
    static constexpr uint64_t kIndMacRegTypeValue = 0;
    static constexpr int      kIndSerdesRegTypeShift = 22; //37:22 zero
    static constexpr uint64_t kIndSerdesRegTypeMask  = 0xFFFF;
    static constexpr uint64_t kIndSerdesRegTypeValue = 0;
    // Various extracts for pipe addresses.
    static constexpr int      kIndPipePipeIdShift = 37;
    static constexpr int      kDirPipePipeIdShift = 23;
    static constexpr uint64_t kPipePipeIdMask  = 3;
    static constexpr int      kIndPipeStageIdShift = 33;
    static constexpr int      kDirPipeStageIdShift = 19;
    static constexpr uint64_t kPipeStageIdMask  = 0xF;
    static constexpr int      kIndPipeRegAddrShift = 0;
    static constexpr int      kDirPipeRegAddrShift = 0;
    static constexpr uint64_t kPipeRegAddrMask  = 0x7FFFF;
    // Various extracts for device-select addresses.
    static constexpr int      kIndDevSelDevIdShift = 34;
    static constexpr int      kDirDevSelDevIdShift = 18;
    static constexpr uint64_t kDevSelDevIdMask  = 0x1F;
    static constexpr int      kIndDevSelRegAddrShift = 0;
    static constexpr int      kDirDevSelRegAddrShift = 0;
    static constexpr uint64_t kDevSelRegAddrMask  = 0x3FFFF;
    // Various extracts for mac addresses.
    static constexpr int      kIndMacMacIdShift = 32;
    static constexpr int      kDirMacMacIdShift = 17;
    static constexpr int      kMacMacIdMask = 0x3F;
    static constexpr int      kIndMacRegAddrShift = 0;
    static constexpr int      kDirMacRegAddrShift = 0;
    static constexpr int      kMacRegAddrMask = 0x1FFFF;
    // Various extracts for serdes addresses.
    static constexpr int      kIndSerdesRegAddrShift = 0;
    static constexpr int      kDirSerdesRegAddrShift = 0;
    static constexpr int      kSerdesRegAddrMask = 0x3FFFFF;
    // Clock speed
    static constexpr uint64_t kRmtClocksPerSec = UINT64_C(1250000000); // 1.25GHz
    // PCIe Port
    static bool is_pcie_port(int port, uint32_t pipes_enabled) {
      int local_port_bit_sz = kPortChanWidth + kPortGroupWidth;
      int local_port_bit_mask = (1u << local_port_bit_sz) - 1;
      int pipe_local_port = port & local_port_bit_mask;
      int pipe = port >> local_port_bit_sz;
      /* Local port number is always 64. */
      if (pipe_local_port != 64)
        return false;
      /* PCIe port would be on pipe 2 if pipe 2 is enabled but when pipe 2 is
       * disabled the PCIe port moves to pipe 3. */
      if (pipes_enabled & 4) {
        /* Pipe 2 is enabled. */
        return pipe == 2;
      } else if (pipes_enabled & 8) {
        /* Pipe 2 is disabled and pipe 3 is enabled. */
        return pipe == 3;
      }
      return false;
    }

    RmtDefsTofinoXX()          {}
    virtual ~RmtDefsTofinoXX() {}

  };
}

#endif // _TOFINOXX_RMT_DEFS_TOFINOXX_
