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

#ifndef _JBAY_SHARED_RMT_DEFS_JBAY_SHARED_
#define _JBAY_SHARED_RMT_DEFS_JBAY_SHARED_

#include <model_core/model.h>
#include <rmt-defs-shared.h>

namespace MODEL_CHIP_NAMESPACE {

  class RmtDefsJbayShared : public RmtDefsShared {
 public:
    static constexpr uint8_t kChipType = model_core::ChipType::kJbay;

    static constexpr int     kPhvWordsMax                 = 280;
    static constexpr int     kPhvWordsMaxExtended         = 280;
    static constexpr int     kPhvWordsMaxUnmapped         = 224;
    static constexpr int     kPhvWordsMaxExtendedUnmapped = 224;
    static constexpr int     kPhvWordsPerGroupUnmapped    =  32;
    static constexpr int     kPhvWordsPerGroup            =  40;
    static int map_mau_phv_index(const int phv_index)       { return phv_index; }
    static int map_maumixbar_phv_index(const int mixbar_phv_index) {
      // See email 'Mau_datapath CSR mapping change' - 19 June 2018
      // - mixbar now programmed in blocks of 8 with [0-5]=NORM [6,7]=MOCHA
      // - so have to expand 2x8 into 1x16 to get [0-11]=NORM [12-15]=MOCHA
      // - and then call map_index_16_20 to open up holes [16-19]=DARK
      uint8_t map_2x8_to_16[16] = { 0,1,2,3,4,5,12,13,6,7,8,9,10,11,14,15 };
      return map_index_16_20( ((mixbar_phv_index/16)*16) + map_2x8_to_16[mixbar_phv_index%16] );
    }
    static int map_mausnap_phv_index(const int phv_index)   { return map_index_16_20(phv_index); }
    static int unmap_mausnap_phv_index(const int phv_index) { return map_index_20_16(phv_index); }
    static int map_prsr_phv_index(const int phv_index)      { return map_index_16_20(phv_index); }
    static int map_dprsr_phv_index(const int phv_index)     { return map_index_16_20(phv_index); }

    static constexpr int     kPipesMin = 4;
    static constexpr int     kPipesMax = 4;
    static constexpr int     kPipeBits = 2;
    static constexpr int     kPipeMask = (1<<kPipeBits)-1;
    static constexpr int     kStagesMin = 4;
    static constexpr int     kStagesMax = 20;
    static constexpr int     kStageBits = 5;
    static constexpr int     kStageMask = (1<<kStageBits)-1;
    static constexpr int     kParsers = 36;
    static constexpr int     kParserChannels = 2;
    static constexpr int     kPhysMacs = 8;
    static constexpr int     kPortGroupsPerPipe = 9;
    static constexpr int     kPortsPerPortGroup = 8;
    static constexpr int     kQidsPerPortGroup = 128;
    static constexpr int     kPortsPerPipe = kPortsPerPortGroup * kPortGroupsPerPipe;
    static constexpr int     kMacChannelMax = 8; // ComiraUmac4
    static constexpr int     kIpbs = 9;
    static constexpr int     kEpbs = 9;

    // Elements of port ids (assumes no gaps)
    static constexpr int     kPortChanWidth = 3;       // port[2:0]
    static constexpr int     kPortGroupWidth = 4;      // port[6:3]
    static constexpr int     kPortPipeWidth = 2;       // port[8:7]
    static constexpr int     kPortParserChanWidth = 1; // port[0:0]
    static constexpr int     kPortParserWidth = 6;     // port[6:1]
    static constexpr int     kPortIpbChanWidth = 3;    // port[2:0]
    static constexpr int     kPortIpbWidth = 4;        // port[6:3] 4-1 Parser-IPB
    static constexpr int     kPortEpbChanWidth = 3;    // port[2:0]
    static constexpr int     kPortEpbWidth = 4;        // port[6:3] 4-1 Parser-EPB
    static_assert( (kPortPipeWidth >= kPipeBits), "Insufficient pipe bitwidth in port ids");


    // We create 4 pipes in JBay and all have independent MAUs
    static int map_mau_pipe(int pipe, uint32_t enabled_pipes) { return pipe; }
    // And each has its own LearningFilter
    static int map_lfltr_pipe(int pipe, uint32_t enabled_pipes) { return pipe; }

    // IPB defns - kIpbMeta1Size now specialized for JBay/WIP
    static constexpr int     kChannelsPerIpb = 8;
    static constexpr int     kIpbHeaderSizeBytes = 32;
    static constexpr int     kIpbMeta0SizeBytes = 8;
    //static constexpr int   kIpbMeta1SizeBytes = 16;
    static constexpr int     kChlsPerIpb = kChannelsPerIpb;
    static_assert( (1<<kPortIpbChanWidth == kChannelsPerIpb),
                   "Expected IPB channels to use ALL Port IPB bits" );

    // EPB defns
    static constexpr int     kChannelsPerEpb = 8;
    static_assert( (1<<kPortEpbChanWidth == kChannelsPerEpb),
                   "Expected EPB channels to use ALL Port EPB bits" );

    // Parser defns
    static constexpr int     kParserCheckFlags = 0x0008;
    static constexpr int     kParserExtractFlags = 0x0007;
    static constexpr int     kParserExtract8bWordsPerCycle = 0;
    static constexpr int     kParserExtract16bWordsPerCycle = 20;
    static constexpr int     kParserExtract32bWordsPerCycle = 0;
    static constexpr int     kParserFirstByteInbuf2 = 48;
    static constexpr int     kParserChecksumEngines = 5;
    static constexpr int     kParserChecksumRams = 5;
    static constexpr int     kParserCounterStackSize = 3;
    static constexpr int     kParserGroupShift = 2; // Derive parser 'group' (ie IBP/EBP) from parser number
    static constexpr int     kParserElementMask = (1<<kParserGroupShift)-1;
    static int get_parser_group(int parser_num)   { return parser_num >> kParserGroupShift; }
    static int get_parser_element(int parser_num) { return parser_num & kParserElementMask; }
    static int get_parser_index(int grp, int elt) { return (grp << kParserGroupShift) | (elt & kParserElementMask); }

    // EgressBuf defns
    static constexpr int kEgressBufSlicesPerPipe = 4;
    static constexpr int kEgressBufEbuf100PerSlice = 1;
    static constexpr int kEgressBufEbuf400PerSlice = 2;
    static constexpr int kEgressBufChannelsPerEbuf100 = 2;
    static constexpr int kEgressBufChannelsPerEbuf400 = 8;


    // CLOT config - used by Parser and Deparser
    static constexpr int     kClotMinGap = 3; // -1 => overlaps allowed
    static constexpr int     kClotMaxLen = 64;
    static constexpr int     kClotMaxOffset = 383;
    static constexpr int     kClotMaxLenPlusOffset = 384;
    static constexpr uint8_t kClotMaxSimultaneousTags = 16;
    static constexpr int     kClotTagWidth = 6;
    static constexpr uint8_t kClotMaxTag = (1<<kClotTagWidth)-1;

    // Deparser defns
    static constexpr int     kDeparserSlicesPerPipe = 4;
    static constexpr int     kDeparserChannelsPerSlice = 18;

   static int get_deparser_slice(int local_channel_id) {
      // slice0 deals with 0,1 and 8-23  then slice1 2,3 and 24-39 and so on
      int slice= (local_channel_id<8) ?
          local_channel_id / 2 :
          (local_channel_id-8) / 16;
      RMT_ASSERT( slice>=0 && slice<kDeparserSlicesPerPipe );
      return slice;
    }

    static int get_deparser_channel_within_slice(int local_channel_id) {
      // Up to 8: local_channels: 0,2,4,6 map to slice's channel 0.
      //                          1,3,5,7 to 1
      //  Above 8 groups of 16 map to channels 2..17
      int ch = (local_channel_id<8) ?
          local_channel_id % 2 :
          (((local_channel_id-8) % 16) + 2);
      RMT_ASSERT( ch>=0 && ch<kDeparserChannelsPerSlice );
      return ch;
    }

    // I2Queueing definitions
    static constexpr uint8_t kI2qQidWidth = 7;
    static constexpr uint8_t kI2qQidMask = (0x1u << kI2qQidWidth) - 1;

    // Mirroring
    static constexpr int    kMirrorSlices         =   4;
    static constexpr int    kMirrorNormalSessions = 256;
    static constexpr int    kMirrorCoalSessions   =  16;

    // Learn filter
    static constexpr int    kLearnQuantumWidth = 384;
    static constexpr int    kLearnFilterNumBloomFilter = 2;
    static constexpr int    kLearnFilterNumHash = 4;
    static constexpr int    kLearnFilterHashIndexBits = 16;
    static constexpr int    kLearnFilterHashTableSize = (1<<kLearnFilterHashIndexBits);
    static constexpr int    kLearnFilterLqBufferSize = 4096;

    // Miscellaneous1: Reset control values and status masks
    static constexpr uint32_t kSoftResetCtrlMask     = 0x40000u; // Bit18
    static constexpr uint32_t kSoftResetCtrlVal      = 0x40000u;

    static constexpr uint32_t kSoftResetStatusMask   = 0x001Au;
    static constexpr uint32_t kSoftResetStatusValOk  = 0x0002u;
    static constexpr uint32_t kSoftResetStatusValErr = 0x0000u;

    // Packet Generator
    static constexpr int    kPktGenApps = 16;
    static constexpr int    kPktGenMatchWidth = 128;
    static constexpr int    kPktGenEventFifoDepth = 5;
    //
    // Address Manipulation
    //

    // Shifts and masks to get the address type from an address.
    static constexpr int      kIndPipeTypeShift = 41;
    static constexpr int      kDirPipeTypeShift = 26;
    static constexpr uint64_t kPipeTypeMask  = 1;
    static constexpr uint64_t kPipeTypeValue = 1;
    static constexpr int      kIndDevSelTypeShift = 39;
    static constexpr int      kDirDevSelTypeShift = 24;
    static constexpr uint64_t kDevSelTypeMask  = 7;
    static constexpr uint64_t kDevSelTypeValue = 0;
    static constexpr int      kIndMacTypeShift = 39;
    static constexpr int      kDirMacTypeShift = 24;
    static constexpr uint64_t kMacTypeMask  = 7;
    static constexpr uint64_t kMacTypeValue = 2;
    static constexpr int      kIndSerdesTypeShift = 39;
    static constexpr int      kDirSerdesTypeShift = 24;
    static constexpr uint64_t kSerdesTypeMask  = 7;
    static constexpr uint64_t kSerdesTypeValue = 3;
    // Shifts and masks to check if an indirect address is a register or not.
    static constexpr int      kIndPipeRegTypeShift = 30;
    static constexpr uint64_t kIndPipeRegTypeMask  = 3;
    static constexpr uint64_t kIndPipeRegTypeValue = 0;
    static constexpr int      kIndDevSelRegTypeShift = 19; //33:19 zero
    static constexpr uint64_t kIndDevSelRegTypeMask  = 0x7FFF;
    static constexpr uint64_t kIndDevSelRegTypeValue = 0;
    static constexpr int      kIndMacRegTypeShift = 18; // 31:18 zero
    static constexpr uint64_t kIndMacRegTypeMask  = 0x3FFF;
    static constexpr uint64_t kIndMacRegTypeValue = 0;
    static constexpr int      kIndSerdesRegTypeShift = 24; //38:24 zero
    static constexpr uint64_t kIndSerdesRegTypeMask  = 0x7FFF;
    static constexpr uint64_t kIndSerdesRegTypeValue = 0;
    // Various extracts for pipe addresses.
    static constexpr int      kIndPipePipeIdShift = 39;
    static constexpr int      kDirPipePipeIdShift = 24;
    static constexpr uint64_t kPipePipeIdMask  = 3;
    static constexpr int      kIndPipeStageIdShift = 34;
    static constexpr int      kDirPipeStageIdShift = 19;
    static constexpr uint64_t kPipeStageIdMask  = 0x1F;
    static constexpr int      kIndPipeRegAddrShift = 0;
    static constexpr int      kDirPipeRegAddrShift = 0;
    static constexpr uint64_t kPipeRegAddrMask  = 0x7FFFF;
    // Various extracts for device-select addresses.
    static constexpr int      kIndDevSelDevIdShift = 34;
    static constexpr int      kDirDevSelDevIdShift = 19;
    static constexpr uint64_t kDevSelDevIdMask  = 0x1F;
    static constexpr int      kIndDevSelRegAddrShift = 0;
    static constexpr int      kDirDevSelRegAddrShift = 0;
    static constexpr uint64_t kDevSelRegAddrMask  = 0x7FFFF;
    // Various extracts for mac addresses.
    static constexpr int      kIndMacMacIdShift = 32;
    static constexpr int      kDirMacMacIdShift = 18;
    static constexpr int      kMacMacIdMask = 0x3F;
    static constexpr int      kIndMacRegAddrShift = 0;
    static constexpr int      kDirMacRegAddrShift = 0;
    static constexpr int      kMacRegAddrMask = 0x3FFFF;
    // Various extracts for serdes addresses.
    static constexpr int      kIndSerdesRegAddrShift = 0;
    static constexpr int      kDirSerdesRegAddrShift = 0;
    static constexpr int      kSerdesRegAddrMask = 0xFFFFFF;
    // Clock speed
    static constexpr uint64_t kRmtClocksPerSec = UINT64_C(1000000000); // 1.00GHz
    // PCIe Port
    static bool is_pcie_port(int port, uint32_t pipes_enabled) {
      (void)pipes_enabled;
      /* The PCIe port is always pipe 0 port 0. */
      return port == 0;
    }

    RmtDefsJbayShared()          {}
    virtual ~RmtDefsJbayShared() {}

  };
}

#endif // _JBAY_SHARED_RMT_DEFS_JBAY_SHARED_
