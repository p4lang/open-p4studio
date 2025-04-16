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

#ifndef _SHARED_ADDRESS_
#define _SHARED_ADDRESS_

#include <common/rmt-assert.h>
#include <mau-defs.h>
#include <rmt-defs.h>


namespace MODEL_CHIP_NAMESPACE {

  struct AddrType {
    static constexpr uint8_t  kNone     = 0x00;
    static constexpr uint8_t  kAction   = 0x01;
    static constexpr uint8_t  kStats    = 0x02;
    static constexpr uint8_t  kMeter    = 0x04;
    static constexpr uint8_t  kSelect   = 0x08;
    static constexpr uint8_t  kStateful = 0x10;
    static constexpr uint8_t  kIdle     = 0x20;
  };
  
  class Address  {
 public:
    static int  kStatsAddrFixShift;  // Defined in rmt-config.cpp
    static int  kStatsAddrSramShift; // Defined in rmt-config.cpp
    static bool kGlobalAddrEnable;   // Defined in rmt-config.cpp

    // Check the type of a direct address.
    static bool dir_addr_is_pipe(uint32_t a) {
      return RmtDefs::kPipeTypeValue == ((a >> RmtDefs::kDirPipeTypeShift) & RmtDefs::kPipeTypeMask);
    }
    static bool dir_addr_is_devsel(uint32_t a) {
      return RmtDefs::kDevSelTypeValue == ((a >> RmtDefs::kDirDevSelTypeShift) & RmtDefs::kDevSelTypeMask);
    }
    static bool dir_addr_is_mac(uint32_t a) {
      return RmtDefs::kMacTypeValue == ((a >> RmtDefs::kDirMacTypeShift) & RmtDefs::kMacTypeMask);
    }
    static bool dir_addr_is_serdes(uint32_t a) {
      return RmtDefs::kSerdesTypeValue == ((a >> RmtDefs::kDirSerdesTypeShift) & RmtDefs::kSerdesTypeMask);
    }

    // Pipe Address Helpers.
    static uint32_t dir_pipe_addr_get_pipe_id(uint32_t a) {
      return (a >> RmtDefs::kDirPipePipeIdShift) & RmtDefs::kPipePipeIdMask;
    }
    static uint32_t dir_pipe_addr_get_stage_id(uint32_t a) {
      return (a >> RmtDefs::kDirPipeStageIdShift) & RmtDefs::kPipeStageIdMask;
    }
    static uint32_t dir_pipe_addr_get_reg_addr(uint32_t a) {
      return (a >> RmtDefs::kDirPipeRegAddrShift) & RmtDefs::kPipeRegAddrMask;
    }
    static uint64_t dir_to_ind_pipe_reg(uint32_t a) {
      return (RmtDefs::kPipeTypeValue << RmtDefs::kDirPipeTypeShift) |
             (dir_pipe_addr_get_pipe_id(a) << RmtDefs::kDirPipePipeIdShift) |
             (dir_pipe_addr_get_stage_id(a) << RmtDefs::kDirPipeStageIdShift) |
             (dir_pipe_addr_get_reg_addr(a) << RmtDefs::kDirPipeRegAddrShift);
    }

    // DevSelect Address Helpers.
    static uint32_t dir_devsel_addr_get_dev_id(uint32_t a) {
      return (a >> RmtDefs::kDirDevSelDevIdShift) & RmtDefs::kDevSelDevIdMask;
    }
    static uint32_t dir_devsel_addr_get_reg_addr(uint32_t a) {
      return (a >> RmtDefs::kDirDevSelRegAddrShift) & RmtDefs::kDevSelRegAddrMask;
    }
    static uint64_t dir_to_ind_devsel_reg(uint32_t a) {
      return (RmtDefs::kDevSelTypeValue << RmtDefs::kDirDevSelTypeShift) |
             (dir_devsel_addr_get_dev_id(a) << RmtDefs::kDirDevSelDevIdShift) |
             (dir_devsel_addr_get_reg_addr(a) << RmtDefs::kDirDevSelRegAddrShift);
    }

    static bool ind_addr_is_pipe(uint64_t a) {
      return RmtDefs::kPipeTypeValue == ((a >> RmtDefs::kIndPipeTypeShift) & RmtDefs::kPipeTypeMask);
    }
    static bool ind_addr_is_devsel(uint64_t a) {
      return RmtDefs::kDevSelTypeValue == ((a >> RmtDefs::kIndDevSelTypeShift) & RmtDefs::kDevSelTypeMask);
    }
    static bool ind_addr_is_mac(uint64_t a) {
      return RmtDefs::kMacTypeValue == ((a >> RmtDefs::kIndMacTypeShift) & RmtDefs::kMacTypeMask);
    }
    static bool ind_addr_is_serdes(uint64_t a) {
      return RmtDefs::kSerdesTypeValue == ((a >> RmtDefs::kIndSerdesTypeShift) & RmtDefs::kSerdesTypeMask);
    }

    static bool ind_addr_is_pipe_reg(uint64_t a) {
      return ind_addr_is_pipe(a) &&
        RmtDefs::kIndPipeRegTypeValue == ((a >> RmtDefs::kIndPipeRegTypeShift) & RmtDefs::kIndPipeRegTypeMask);
    }
    static bool ind_addr_is_devsel_reg(uint64_t a) {
      return ind_addr_is_devsel(a) &&
        RmtDefs::kIndDevSelRegTypeValue == ((a >> RmtDefs::kIndDevSelRegTypeShift) & RmtDefs::kIndDevSelRegTypeMask);
    }
    static bool ind_addr_is_mac_reg(uint64_t a) {
      return ind_addr_is_mac(a) &&
        RmtDefs::kIndMacRegTypeValue == ((a >> RmtDefs::kIndMacRegTypeShift) & RmtDefs::kIndMacRegTypeMask);
    }
    static bool ind_addr_is_serdes_reg(uint64_t a) {
      return ind_addr_is_serdes(a) &&
        RmtDefs::kIndSerdesRegTypeValue == ((a >> RmtDefs::kIndSerdesRegTypeShift) & RmtDefs::kIndSerdesRegTypeMask);
    }
    static bool ind_addr_is_reg(uint64_t a) {
      return ind_addr_is_pipe_reg(a) || ind_addr_is_devsel_reg(a) ||
             ind_addr_is_mac_reg(a) || ind_addr_is_serdes_reg(a);
    }

    static uint32_t ind_pipe_addr_get_pipe_id(uint64_t a) {
      return (a >> RmtDefs::kIndPipePipeIdShift) & RmtDefs::kPipePipeIdMask;
    }
    static uint32_t ind_pipe_addr_get_stage_id(uint64_t a) {
      return (a >> RmtDefs::kIndPipeStageIdShift) & RmtDefs::kPipeStageIdMask;
    }
    static uint32_t ind_pipe_addr_get_reg_addr(uint64_t a) {
      return (a >> RmtDefs::kIndPipeRegAddrShift) & RmtDefs::kPipeRegAddrMask;
    }
    static uint32_t ind_to_dir_pipe_reg(uint64_t a) {
      return (RmtDefs::kPipeTypeValue << RmtDefs::kDirPipeTypeShift) |
             (ind_pipe_addr_get_pipe_id(a) << RmtDefs::kDirPipePipeIdShift) |
             (ind_pipe_addr_get_stage_id(a) << RmtDefs::kDirPipeStageIdShift) |
             (ind_pipe_addr_get_reg_addr(a) << RmtDefs::kDirPipeRegAddrShift);
    }

    static uint32_t ind_devsel_addr_get_dev_id(uint64_t a) {
      return (a >> RmtDefs::kIndDevSelDevIdShift) & RmtDefs::kDevSelDevIdMask;
    }
    static uint32_t ind_devsel_addr_get_reg_addr(uint64_t a) {
      return (a >> RmtDefs::kIndDevSelRegAddrShift) & RmtDefs::kDevSelRegAddrMask;
    }
    static uint32_t ind_to_dir_devsel_reg(uint64_t a) {
      return (RmtDefs::kDevSelTypeValue << RmtDefs::kDirDevSelTypeShift) |
             (ind_devsel_addr_get_dev_id(a) << RmtDefs::kDirDevSelDevIdShift) |
             (ind_devsel_addr_get_reg_addr(a) << RmtDefs::kDirDevSelRegAddrShift);
    }

    static uint32_t ind_mac_addr_get_mac_id(uint64_t a) {
      return (a >> RmtDefs::kIndMacMacIdShift) & RmtDefs::kMacMacIdMask;
    }
    static uint32_t ind_mac_addr_get_reg_addr(uint64_t a) {
      return (a >> RmtDefs::kIndMacRegAddrShift) & RmtDefs::kMacRegAddrMask;
    }
    static uint32_t ind_to_dir_mac_reg(uint64_t a) {
      return (RmtDefs::kMacTypeValue << RmtDefs::kDirMacTypeShift) |
             (ind_devsel_addr_get_dev_id(a) << RmtDefs::kDirMacMacIdShift) |
             (ind_devsel_addr_get_reg_addr(a) << RmtDefs::kDirMacRegAddrShift);
    }

    static uint32_t ind_serdes_addr_get_reg_addr(uint64_t a) {
      return (a >> RmtDefs::kIndSerdesRegAddrShift) & RmtDefs::kSerdesRegAddrMask;
    }
    static uint32_t ind_to_dir_serdes_reg(uint64_t a) {
      return (RmtDefs::kSerdesTypeValue << RmtDefs::kDirSerdesTypeShift) |
             (ind_serdes_addr_get_reg_addr(a) << RmtDefs::kDirMacRegAddrShift);
    }

    static uint32_t ind_to_dir_reg_addr(uint64_t a) {
      RMT_ASSERT( ind_addr_is_reg(a) );
      if (ind_addr_is_pipe_reg(a)) return ind_to_dir_pipe_reg(a);
      if (ind_addr_is_devsel_reg(a)) return ind_to_dir_devsel_reg(a);
      if (ind_addr_is_mac_reg(a)) return ind_to_dir_mac_reg(a);
      if (ind_addr_is_serdes_reg(a)) return ind_to_dir_serdes_reg(a);
      RMT_ASSERT(0);
    }

    // Generic address OPs - *mostly* same across all AddrTypes
    static constexpr int      kAddrOpMin        = 0x0;
    static constexpr int      kAddrOpNop        = 0x0;
    static constexpr int      kAddrOpNormal     = 0x1;
    static constexpr int      kAddrOpCfgRd      = 0x2;
    static constexpr int      kAddrOpCfgRdClr   = 0x3;
    static constexpr int      kAddrOpCfgWr      = 0x4;
    static constexpr int      kAddrOpSweep      = 0x5;
    static constexpr int      kAddrOpMoveregRd  = 0x6;
    static constexpr int      kAddrOpMoveregWr  = 0x7;
    static constexpr int      kAddrOpMax        = 0x7;
    // StatsAddr OPs
    static constexpr int      kStatsOpNop        = 0x0;
    static constexpr int      kStatsOpStats      = 0x1;
    static constexpr int      kStatsOpCfgRd      = 0x2;
    static constexpr int      kStatsOpCfgRdClr   = 0x3;
    static constexpr int      kStatsOpCfgWr      = 0x4;
    static constexpr int      kStatsOpMoveregRd  = 0x5;
    static constexpr int      kStatsOpMoveregWr  = 0x6;
    static constexpr int      kStatsOpFsm        = 0x7;
    static constexpr int      kStatsOpMask       = 0x7;
    // MeterAddr OPs - 3b
    static constexpr int      kMeterOp3Nop             = 0x0;    
    static constexpr int      kMeterOp3LpfOrColorBlind = 0x2;
    static constexpr int      kMeterOp3ColorAware      = 0x6;
    static constexpr int      kMeterOp3Selector        = 0x4;
    static constexpr int      kMeterOp3SaluInst0       = 0x1; // => kMeterOp4SaluInst0 etc
    static constexpr int      kMeterOp3SaluInst1       = 0x3;
    static constexpr int      kMeterOp3SaluInst2       = 0x5;
    static constexpr int      kMeterOp3SaluInst3       = 0x7;
    static constexpr int      kMeterOp3Mask            = 0x7;
    // MeterAddr OPs - 4b
    static constexpr int      kMeterOp4Nop          = 0x0;
    static constexpr int      kMeterOp4CfgRd        = 0x2;
    static constexpr int      kMeterOp4CfgWr        = 0x4;
    static constexpr int      kMeterOp4Sweep        = 0x6;
    static constexpr int      kMeterOp4MoveregRd    = 0x8;
    static constexpr int      kMeterOp4MoveregWr    = 0xA;
    static constexpr int      kMeterOp4MeterColor0  = 0x1;
    static constexpr int      kMeterOp4MeterColor1  = 0x3;
    static constexpr int      kMeterOp4MeterColor2  = 0x5;
    static constexpr int      kMeterOp4MeterColor3  = 0x7;
    static constexpr int      kMeterOp4SaluInst0    = 0x1;
    static constexpr int      kMeterOp4SaluInst1    = 0x3;
    static constexpr int      kMeterOp4SaluInst2    = 0x5;
    static constexpr int      kMeterOp4SaluInst3    = 0x7;
    static constexpr int      kMeterOp4Selector     = 0xF;
    static constexpr int      kMeterOp4CfgSaluInst0 = 0x9;
    static constexpr int      kMeterOp4CfgSaluInst1 = 0xB;
    static constexpr int      kMeterOp4CfgSaluInst2 = 0xC;
    static constexpr int      kMeterOp4CfgSaluInst3 = 0xD;
    static constexpr int      kMeterOp4Reserved     = 0xE; // Used by meter_sweep_one_address
    static constexpr int      kMeterOp4Mask         = 0xF;
    static constexpr int      kMeterOp4SaluClear    = MauDefs::kMeterOp4SaluClearValue;
    
    // SelectorAddr/StatefulAddr OPs
    static constexpr int      kSelectorStatefulOpMask = 0xF;
    // IdletimeAddr OPs
    static constexpr int      kIdletimeOpNop        = 0x0;
    static constexpr int      kIdletimeOpMarkActive = 0x1;
    static constexpr int      kIdletimeOpCfgRd      = 0x2;
    static constexpr int      kIdletimeOpCfgRdClr   = 0x3;
    static constexpr int      kIdletimeOpCfgWr      = 0x4;
    static constexpr int      kIdletimeOpSweep      = 0x5;
    static constexpr int      kIdletimeOpMoveregRd  = 0x6;
    static constexpr int      kIdletimeOpMoveregWr  = 0x7;
    static constexpr int      kIdletimeOpMask       = 0x7;

    // Index,VPN widths and masks
    static constexpr int      kIndexWidth = MauDefs::kSramAddressWidth;
    static constexpr int      kIndexMask = (1<<kIndexWidth)-1;
    static constexpr int      kMapIndexWidth = MauDefs::kMapramAddressWidth;
    static constexpr int      kMapIndexMask = (1<<kMapIndexWidth)-1;
    static constexpr int      kXmMatchVpnWidth = 9;
    static constexpr int      kTmMatchVpnWidth = 6;
    static constexpr int      kActionVpnWidth = 7;
    static constexpr int      kStatsVpnWidth = MauDefs::kMapramVpnBits;
    static constexpr int      kMeterVpnWidth = MauDefs::kMapramVpnBits;
    static constexpr int      kIdletimeVpnWidth = 6;
    static constexpr int      kXmMatchVpnMask = (1<<kXmMatchVpnWidth)-1;
    static constexpr int      kActionVpnMask = (1<<kActionVpnWidth)-1;
    static constexpr int      kStatsVpnMask = (1<<kStatsVpnWidth)-1;
    static constexpr int      kMeterVpnMask = (1<<kMeterVpnWidth)-1;
    static constexpr int      kIdletimeVpnMask = (1<<kIdletimeVpnWidth)-1;
    static constexpr int      kMeterSubwordWidth = 7;
    static constexpr int      kMeterSubwordMask = (1<<kMeterSubwordWidth)-1;

    // Address widths *including* PerFlowEnable bit
    // InstrWidths are 8b on JBay, 7b on Tofino|TofinoB0
    static constexpr int      kImmDataWidth       = 32;
    static constexpr int      kActionInstrWidth   = MauDefs::kActionInstrWidth;
    static constexpr int      kActionAddrWidth    = 23;
    static constexpr int      kStatsAddrWidth     = 20;
    static constexpr int      kMeterAddrWidth     = 27;
    static constexpr int      kMeterAddrTypeWidth =  3;
    static constexpr int      kIdletimeAddrWidth  = 21;
    static constexpr int      kNxtTabWidth        =  8;
    static constexpr int      kSelectorLenWidth   =  8;
    // PFE positions within address if applicable (normally MSB)
    // (NB kActionInstrPfePos is bit 6 *even on JBay*)
    static constexpr int      kImmDataPfePos      = 0xFF;
    static constexpr int      kActionInstrPfePos  =  6;
    static constexpr int      kActionAddrPfePos   = kActionAddrWidth-1;
    static constexpr int      kStatsAddrPfePos    = kStatsAddrWidth-1;
    static constexpr int      kMeterAddrPfePos    = kMeterAddrWidth-kMeterAddrTypeWidth-1;
    static constexpr int      kIdletimeAddrPfePos = kIdletimeAddrWidth-1;
    static constexpr int      kNxtTabPfePos       = 0xFF;
    static constexpr int      kSelectorLenPfePos  = 0xFF;
    // OP positions within address (only used for Meters)
    static constexpr int      kImmDataOpPos       = 0xFF;
    static constexpr int      kActionInstrOpPos   = 0xFF;
    static constexpr int      kActionAddrOpPos    = 0xFF;
    static constexpr int      kStatsAddrOpPos     = 0xFF;
    static constexpr int      kMeterAddrOpPos     = kMeterAddrWidth-kMeterAddrTypeWidth;
    static constexpr int      kIdletimeAddrOpPos  = 0xFF;
    static constexpr int      kNxtTabOpPos        = 0xFF;
    static constexpr int      kSelectorLenOpPos   = 0xFF;
    
    static constexpr uint32_t kActionInstrEnabledMask = 1u<<kActionInstrPfePos;
    static constexpr uint32_t kActionAddrEnabledMask = 1u<<kActionAddrPfePos;
    static constexpr uint32_t kActionAddrAddrMask = (1u<<(kActionAddrWidth-1))-1u;
    static constexpr uint32_t kStatsAddrEnabledMask = 1u<<kStatsAddrPfePos;
    static constexpr uint32_t kStatsAddrAddrMask = (1u<<(kStatsAddrWidth-1))-1u;
    static constexpr uint32_t kMeterAddrEnabledMask = 1u<<kMeterAddrPfePos;
    static constexpr uint32_t kMeterAddrAddrMask = (1u<<(kMeterAddrWidth-kMeterAddrTypeWidth-1))-1u;
    static constexpr uint32_t kMeterAddrTypeMask = (1u<<kMeterAddrTypeWidth)-1u;
    static constexpr uint32_t kIdletimeAddrEnabledMask = 1u<<kIdletimeAddrPfePos;
    static constexpr uint32_t kIdletimeAddrAddrMask = (1u<<(kIdletimeAddrWidth-1))-1u;
    static constexpr uint32_t kInvalid = 0u;
    static inline uint32_t    invalid()                    { return kInvalid; }
    static inline bool        isInvalid(uint32_t a)        { return (a == kInvalid); }
    static inline bool        isValid(uint32_t a)          { return (a != kInvalid); }
    static inline uint32_t    addrShift(uint32_t a, int s) { return (s<0) ?(a<<-s) :(a>>s); }


    // Funcs to decode XM MATCH addresses
    static inline int xm_match_addr_get_vpn(uint32_t addr) {
      return (addr >> kIndexWidth) & kXmMatchVpnMask;
    }
    static inline int xm_match_addr_get_index(uint32_t addr) {
      return (addr >> 0) & kIndexMask;
    }
    static inline int xm_match_addr_make(uint16_t vpn, uint16_t index) {
      return ((vpn & kXmMatchVpnMask) << kIndexWidth) | ((index & kIndexMask) << 0);
    }

    
    // Funcs to decode TCAM MATCH addresses
    static inline int tcam_match_addr_get_subword(uint32_t addr,int shft=0) {
      return (addr & 0x1F) >> shft;
    }
    static inline int tcam_match_addr_get_vpn(uint32_t addr,int shft=0) {
      return (addr >> (kIndexWidth+5)) & ((1<<shft)-1);
    }
    static inline int tcam_match_addr_get_index(uint32_t addr) {
      return (addr >> 5) & kIndexMask;
    }
    static int tcam_match_addr_get_which(uint32_t addr) {
      return (addr >> 4) & 0x1;
    }

    
    // Funcs to decode ACTION INSTR addresses
    static inline bool action_instr_enabled(uint32_t addr) {
      return (((addr & kActionInstrEnabledMask) != 0u) || (kGlobalAddrEnable));
    }    


    // Funcs to decode ACTION addresses
    static inline bool action_addr_enabled(uint32_t addr) {
      return (((addr & kActionAddrEnabledMask) != 0u) || (kGlobalAddrEnable));
    }    
    
    static inline int action_addr_get_shift(uint32_t addr) {
      if ((addr & 0x1) == 0x0) return 1;        // Data-size 8 bits
      else if ((addr & 0x03) == 0x1) return 2;  // Data-size 16 bits
      else if ((addr & 0x07) == 0x3) return 3;  // Data-size 32 bits
      else if ((addr & 0x0F) == 0x7) return 4;  // Data-size 64 bits
      else if ((addr & 0x1F) == 0xF) return 5;  // Data-size 128 bits
      else {                                    // Use VPN Huffman bits
        if (((addr >> (kIndexWidth+5)) & 0x1) == 0x0)      return 6; // Data-size 256 bits
        else if (((addr >> (kIndexWidth+5)) & 0x3) == 0x1) return 7; // Data-size 512 bits
        else return 8;                                               // Data-size 1024 bits
      }
    }
    static inline int action_addr_get_subword(uint32_t addr,int shft=0) {
      return (addr & 0x1F) >> shft;
    }
    static inline int action_addr_get_vpn_cmpmask(int shft=0) {
      return (shft <= 5) ?kActionVpnMask :kActionVpnMask & (kActionVpnMask << (shft-5));
    }
    static inline int action_addr_get_vaddr(uint32_t addr) {
      return (addr >> 0) & kActionAddrAddrMask;
    }    
    static inline int action_addr_get_vpn(uint32_t addr) {
      return (addr >> (kIndexWidth+5)) & kActionVpnMask;
    }
    static inline int action_addr_get_index(uint32_t addr) {
      return (addr >> 5) & kIndexMask;
    }


    
    // Funcs to decode STATS addresses
    static inline bool stats_addr_enabled(uint32_t addr) {
      return (((addr & kStatsAddrEnabledMask) != 0u) || (kGlobalAddrEnable));
    }
    static inline int stats_addr_op(uint32_t addr) {
      // OP where PFE was
      return (addr >> (kStatsAddrWidth-1)) & kStatsOpMask;
    }
    static inline int stats_addr_make(uint32_t addr, int op) {
      // OP replaces PFE
      return ((op & kStatsOpMask) << (kStatsAddrWidth-1)) | (addr & kStatsAddrAddrMask);
    }
    static inline int stats_addr_map_en_to_op(uint32_t addr) {
      int op = (stats_addr_enabled(addr)) ?kStatsOpStats :kStatsOpNop;
      return stats_addr_make(addr, op);
    }
    static inline bool stats_addr_op_enabled(uint32_t addr) {
      return (stats_addr_op(addr) != kStatsOpNop);
    }
    static inline bool stats_addr_op_cfg(uint32_t addr) {
      int op = stats_addr_op(addr);
      return ((op == kStatsOpCfgRd) || (op == kStatsOpCfgRdClr) || (op == kStatsOpCfgWr));
    }
    static inline bool stats_addr_op_deferrable(uint32_t addr) {
      return (stats_addr_op(addr) == kStatsOpStats);
    }
    static inline int stats_addr_get_vaddr(uint32_t addr) {
      return (addr >> 0) & kStatsAddrAddrMask;
    }
    static inline int stats_addr_get_vpn(uint32_t addr) {
      return (addr >> (kIndexWidth+5+kStatsAddrSramShift)) & kStatsVpnMask;
    }
    static inline int stats_addr_get_index(uint32_t addr) {
      return (addr >> (5+kStatsAddrSramShift)) & kIndexMask;
    }
    static inline int stats_addr_get_subword(uint32_t addr,int shft=0) {
      return (addr & ( (1u << (5+kStatsAddrSramShift)) -1 )) >> shft;
    }
    static inline int stats_addr_make2(int vpn, int index, int subword) {
      return ((vpn & kStatsVpnMask) << (kIndexWidth+5+kStatsAddrSramShift)) |
          ((index & kIndexMask) << (5+kStatsAddrSramShift)) |
          ((subword & ( (1u << (5+kStatsAddrSramShift)) -1 )) << 0);
    }
    


 
    // Funcs to decode METER addresses
    static inline bool meter_addr_enabled(uint32_t addr) {
      return (((addr & kMeterAddrEnabledMask) != 0u) || (kGlobalAddrEnable));      
    }
    static inline int meter_addr_op3(uint32_t addr) {
      // OP3 above PFE, so OP3:PFE:ADDR
      return (addr >> (kMeterAddrWidth-kMeterAddrTypeWidth)) & kMeterOp3Mask;
    }
    static inline int meter_addr_op4(uint32_t addr) {
      // OP4 replaces PFE, so OP4:ADDR
      return (addr >> (kMeterAddrWidth-kMeterAddrTypeWidth-1)) & kMeterOp4Mask;
    }
    static inline int meter_addr_make(uint32_t addr, int op4) {
      // OP4 replaces PFE, so OP4:ADDR
      return ((op4 & kMeterOp4Mask) << (kMeterAddrWidth-kMeterAddrTypeWidth-1)) |
             (addr & kMeterAddrAddrMask);
    }        
    static inline uint8_t meter_color_get_color_op4(uint8_t colour) {
      return kMeterOp4MeterColor0 | ((colour & 0x3) << 1);
    }
    static inline int meter_color_op4_get_color(uint8_t colour_op4) {
      switch (colour_op4) {
        case kMeterOp4MeterColor0: case kMeterOp4MeterColor1:
        case kMeterOp4MeterColor2: case kMeterOp4MeterColor3:
	  return (colour_op4 >> 1) & 0x3;
        default: return -1;
      }
    }
    static inline int meter_color_zeroise_color(uint32_t addr) {
      return meter_addr_make(addr, kMeterOp4MeterColor0);
    }
    static inline int meter_addr_map_op3en_to_op4(uint32_t addr, uint8_t colour,
                                                  bool col_aware_becomes_sweep) {
      int op4 = kMeterOp4Nop;
      if (meter_addr_enabled(addr)) { 
        int colour_op4 = meter_color_get_color_op4(colour);
        int col_blind_op4 = colour_op4;
        int col_aware_op4 = (col_aware_becomes_sweep) ?kMeterOp4Sweep :colour_op4;
        switch (meter_addr_op3(addr)) {
	  case kMeterOp3Nop:       op4 = kMeterOp4Nop;        break;
          case kMeterOp3LpfOrColorBlind: op4 = col_blind_op4; break;
          case kMeterOp3ColorAware:      op4 = col_aware_op4; break;
          case kMeterOp3Selector:  op4 = kMeterOp4Selector;   break;
          case kMeterOp3SaluInst0: op4 = kMeterOp4SaluInst0;  break;          
          case kMeterOp3SaluInst1: op4 = kMeterOp4SaluInst1;  break;
          case kMeterOp3SaluInst2: op4 = kMeterOp4SaluInst2;  break;
          case kMeterOp3SaluInst3: op4 = kMeterOp4SaluInst3;  break;
          default: RMT_ASSERT(0); break;
        }
      }
      return meter_addr_make(addr, op4);
    }    
    static inline bool meter_addr_op_enabled(uint32_t addr) {
      return (meter_addr_op4(addr) != kMeterOp4Nop);  
    }
    static inline bool meter_addr_op_sweep(uint32_t addr) {
      return (meter_addr_op4(addr) == kMeterOp4Sweep);  
    }
    static inline bool meter_addr_op_cfg(uint32_t addr) {
      int op = meter_addr_op4(addr);
      return ((op == kMeterOp4CfgRd) || (op == kMeterOp4CfgWr));
    }    
    static inline bool meter_addr_is_meter(uint32_t addr) {
      switch (meter_addr_op4(addr)) {
        case kMeterOp4MeterColor0: case kMeterOp4MeterColor1:
        case kMeterOp4MeterColor2: case kMeterOp4MeterColor3:
          return true;
        default: return false;
      }
    }
    static inline bool meter_addr_is_stateful(uint32_t addr) {
      switch (meter_addr_op4(addr)) {
        case kMeterOp4SaluInst0: case kMeterOp4SaluInst1:
        case kMeterOp4SaluInst2: case kMeterOp4SaluInst3:
        case kMeterOp4CfgSaluInst0: case kMeterOp4CfgSaluInst1:
        case kMeterOp4CfgSaluInst2: case kMeterOp4CfgSaluInst3:
        case kMeterOp4SaluClear:
          return true;
        default: return false;
      }
    }
    static inline bool meter_addr_is_run_stateful(uint32_t addr) {
      switch (meter_addr_op4(addr)) {
        case kMeterOp4CfgSaluInst0: case kMeterOp4CfgSaluInst1:
        case kMeterOp4CfgSaluInst2: case kMeterOp4CfgSaluInst3:
          return true;
        default: return false;
      }
    }
    static inline int meter_addr_stateful_instruction(uint32_t addr) {
      switch (meter_addr_op4(addr)) {
        case kMeterOp4SaluInst0: case kMeterOp4CfgSaluInst0: return 0;
        case kMeterOp4SaluInst1: case kMeterOp4CfgSaluInst1: return 1;
        case kMeterOp4SaluInst2: case kMeterOp4CfgSaluInst2: return 2;
        case kMeterOp4SaluInst3: case kMeterOp4CfgSaluInst3: return 3;
        default: return -1;
      }
    }
    static inline bool meter_addr_is_stateful_clear(uint32_t addr) {
      return meter_addr_op4(addr) == kMeterOp4SaluClear;
    }
    static inline bool meter_addr_op_deferrable(uint32_t addr) {
      return meter_addr_is_meter(addr);
    }
    static inline int meter_addr_get_color(uint32_t addr) {
      return meter_color_op4_get_color(meter_addr_op4(addr));
    }
    static inline int meter_addr_get_type(uint32_t addr) {
      return meter_addr_op4(addr);
    }
    static inline int meter_addr_get_vaddr(uint32_t addr) {
      return (addr >> 0) & kMeterAddrAddrMask;
    }    
    static inline int meter_addr_get_vpn(uint32_t addr) {
      return (addr >> (kIndexWidth+7)) & kMeterVpnMask;
    }
    static inline int meter_addr_get_index(uint32_t addr) {
      return (addr >> 7) & kIndexMask;
    }
    static inline int meter_addr_get_subword(uint32_t addr) {
      return (addr >> 0) & kMeterSubwordMask;
    }
    static inline int meter_addr_get_subword_huffman(uint32_t addr) {
      return stateful_addr_get_subword(addr,stateful_addr_get_shift(addr));
    }
    static inline int meter_addr_get_word_addr(uint32_t addr) {
      return (addr >> 0) & kMeterAddrAddrMask & ~kMeterSubwordMask;
    }
    static inline int meter_addr_make2(int vpn, int index, int subword) {
      return (((vpn & kMeterVpnMask) << (kIndexWidth+kMeterSubwordWidth+0)) |
	      ((index & kIndexMask) << (kMeterSubwordWidth+0)) |
	      ((subword & kMeterSubwordMask) << 0));
    }
    static inline int meter_addr_make2_huffman(int vpn, int index, int subword, int shift) {
      const uint8_t huff[8] = { 0, 0, 0x1, 0x3, 0x7, 0xF, 0, 0 }; // Shift in [1..5]
      return (((vpn & kMeterVpnMask) << (kIndexWidth+kMeterSubwordWidth+0)) |
	      ((index & kIndexMask) << (kMeterSubwordWidth+0)) |
	      ( (((subword << shift) & 0x1F) | huff[shift & 7])  <<  2 ));
    }


    
    // Funcs to decode SELECTOR addresses - just for Huffman decode
    static inline int selector_addr_get_shift(uint32_t addr) {
      addr >>= 2; // Huffman is in bits [6:2]
      if ((addr & 0x1) == 0x0) return 1;       // 16   8-bit subwords
      else if ((addr & 0x03) == 0x1) return 2; //  8  16-bit subwords
      else if ((addr & 0x07) == 0x3) return 3; //  4  32-bit subwords
      else if ((addr & 0x0F) == 0x7) return 4; //  2  64-bit subwords
      else if ((addr & 0x1F) == 0xF) return 5; //  1 128-bit subword
      else return 0; // Invalid
    }
    static inline int selector_addr_get_width(uint32_t addr, int shft) {
      return ((shft >= 1) && (shft <= 5)) ?(4 << shft) :0;
    }
    static inline int selector_addr_get_nentries(uint32_t addr, int shft) {
      return ((shft >= 1) && (shft <= 5)) ?(32 >> shft) :0;
    }
    static inline int selector_addr_get_subword(uint32_t addr, int shft) {
      return ((addr >> 2) & 0x1F) >> shft;
    }
    static inline int selector_addr_get_offset(uint32_t addr, int shft) {
      int subword = selector_addr_get_subword(addr, shft);
      int nentries = selector_addr_get_nentries(addr, shft);
      int width = selector_addr_get_width(addr, shft);
      return ((width > 0) && (subword < nentries)) ?subword*width :-1;
    }



    //static inline bool selector_addr_enabled(uint32_t addr) {
    //return meter_addr_enabled(addr);
    //}
    //static inline int selector_addr_get_vpn(uint32_t addr) {
    //return meter_addr_get_vpn(addr);
    //}
    //static inline int selector_addr_get_index(uint32_t addr) {
    //return meter_addr_get_index(addr);
    //}

    // Funcs to decode STATEFUL addresses
    //static inline bool stateful_addr_enabled(uint32_t addr) {
    //return meter_addr_enabled(addr);
    //}
    //static inline int stateful_addr_get_vpn(uint32_t addr) {
    //return meter_addr_get_vpn(addr);
    //}
    //static inline int stateful_addr_get_index(uint32_t addr) {
    //return meter_addr_get_index(addr);
    //}
    // These currently IDENTICAL to SELECTOR funcs
    static inline int stateful_addr_get_shift(uint32_t addr) {
      return selector_addr_get_shift(addr);
    }
    static inline int stateful_addr_get_width(uint32_t addr, int shft) {
      return selector_addr_get_width(addr, shft);
    }
    static inline int stateful_addr_get_nentries(uint32_t addr, int shft) {
      return selector_addr_get_nentries(addr, shft);
    }
    static inline int stateful_addr_get_subword(uint32_t addr, int shft) {
      return selector_addr_get_subword(addr, shft);
    }
    static inline int stateful_addr_get_offset(uint32_t addr, int shft) {
      return selector_addr_get_offset(addr, shft);
    }
    
    
    

    // Funcs to decode IDLETIME addresses
    static inline bool idletime_addr_enabled(uint32_t addr) {
      return (((addr & kIdletimeAddrEnabledMask) != 0u) || (kGlobalAddrEnable));      
    }
    static inline int idletime_addr_op(uint32_t addr) {
      // OP where PFE was
      return (addr >> (kIdletimeAddrWidth-1)) & kIdletimeOpMask;
    }
    static inline int idletime_addr_make(uint32_t addr, int op) {
      // OP replaces PFE
      return ((op & kIdletimeOpMask) << (kIdletimeAddrWidth-1)) |
             (addr & kIdletimeAddrAddrMask);
    }
    static inline int idletime_addr_map_en_to_op(uint32_t addr) {
      int op = (idletime_addr_enabled(addr)) ?kIdletimeOpMarkActive :kIdletimeOpNop;
      return idletime_addr_make(addr, op);
    }    
    static inline bool idletime_addr_op_enabled(uint32_t addr) {
      return (idletime_addr_op(addr) != kIdletimeOpNop);
    }
    static inline bool idletime_addr_op_cfg(uint32_t addr) {
      int op = idletime_addr_op(addr);
      return ((op == kIdletimeOpCfgRd) || (op == kIdletimeOpCfgRdClr) || (op == kIdletimeOpCfgWr));
    }    
    static inline int idletime_addr_get_vaddr(uint32_t addr) {
      return (addr >> 0) & kIdletimeAddrAddrMask;
    }
    static inline int idletime_addr_get_shift(uint32_t addr) {
      if ((addr & 0x1) == 0x0) return 1;       // 8 1-bit subwords
      else if ((addr & 0x03) == 0x1) return 2; // 4 2-bit subwords
      else if ((addr & 0x07) == 0x3) return 3; // 2 3-bit subwords
      else if ((addr & 0x0F) == 0x7) return 4; // 1 6-bit subwords
      else return 0; // Invalid
    }
    static inline int idletime_addr_get_width(uint32_t addr, int shft) {
      switch (shft) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return 6;
        default: return 0;
      }
    }
    static inline int idletime_addr_get_nentries(uint32_t addr, int shft) {
      switch (shft) {
        case 1: return 8;
        case 2: return 4;
        case 3: return 2;
        case 4: return 1;
        default: return 0;
      }
    }
    static inline int idletime_addr_get_subword(uint32_t addr, int shft) {
      return (addr & 0xF) >> shft;
    }
    static inline int idletime_addr_get_offset(uint32_t addr, int shft) {
      int subword = idletime_addr_get_subword(addr, shft);
      int nentries = idletime_addr_get_nentries(addr, shft);
      int width = idletime_addr_get_width(addr, shft);
      return ((width > 0) && (subword < nentries)) ?subword*width :-1;
    }
    static inline int idletime_addr_get_vpn(uint32_t addr) {
      return (addr >> (kMapIndexWidth+4)) & kIdletimeVpnMask;
    }
    static inline int idletime_addr_get_index(uint32_t addr) {
      return (addr >> 4) & kMapIndexMask;
    }
    static inline int idletime_addr_make2(int vpn, int index, int subword) {
      return (((vpn & kIdletimeVpnMask) << (kMapIndexWidth+4)) |
	      ((index & kMapIndexMask) << 4) | ((subword & 0xF) << 0));
    }
    

    static inline int color_addr_get_shift(uint32_t addr, uint8_t addrtype) {
      switch ( addrtype ) {
        case AddrType::kMeter:  return 7;
        case AddrType::kStats:  return 3;
        case AddrType::kIdle:   return 4;
      }
      return 0; // should never happen, but there are no other asserts in this file.
    }
    static inline int color_addr_get_subword(uint32_t addr, uint8_t addrtype ) {
      return ( addr >> color_addr_get_shift(addr,addrtype) ) & 0x3;
    }
    static inline int color_addr_get_index(uint32_t addr,uint8_t addrtype) {
      return ( addr >> (color_addr_get_shift(addr,addrtype) + 2 )) & kMapIndexMask;
    }
    // ternary color mapram addresses only have 3 bits of vpn, I am told it is ok to use the
    //   same 4 bit mask as for exact match
    static inline int color_addr_get_vpn(uint32_t addr, uint8_t addrtype) {
      return ( addr >> (color_addr_get_shift(addr,addrtype) + 2 + 10)) & 0xf;
    }



    // Add generic OP onto addresses transforming to address-specific OP
    //
    // Nop, CfgRd, CfgWr are always 0,2,4.  CgfRdClr does not exist for Meters.
    // Sweep varies: StatsOpFsm (7), MeterOp4Sweep (6) or IdletimeOpSweep (5)
    // Here we're making an address from scratch so we don't check addr_enabled()
    //
    static inline uint32_t addr_make(uint8_t addrtype, uint32_t addr, int addrop) {

      // Check everything as we expect
      static_assert( (kAddrOpCfgRd == kStatsOpCfgRd), "Non generic STATS RD op");
      static_assert( (kAddrOpCfgRd == kMeterOp4CfgRd), "Non generic METER RD op");
      static_assert( (kAddrOpCfgRd == kIdletimeOpCfgRd), "Non generic IDLE RD op");

      static_assert( (kAddrOpCfgRdClr == kStatsOpCfgRdClr),
                     "Non generic STATS RDCLR op");
      static_assert( (kAddrOpCfgRdClr == kIdletimeOpCfgRdClr),
                     "Non generic IDLE RDCLR op");

      static_assert( (kAddrOpCfgWr == kStatsOpCfgWr), "Non generic STATS WR op");
      static_assert( (kAddrOpCfgWr == kMeterOp4CfgWr), "Non generic METER WR op");
      static_assert( (kAddrOpCfgWr == kIdletimeOpCfgWr), "Non generic IDLE WR op");

      if ((addrop < kAddrOpMin) || (addrop > kAddrOpMax)) return addr;
      switch (addrtype) {

        case AddrType::kStats:
          if (addrop == kAddrOpSweep) addrop = kStatsOpFsm;
          return stats_addr_make(addr, addrop);
          
        case AddrType::kMeter: case AddrType::kSelect: case AddrType::kStateful:
          if (addrop == kAddrOpCfgRdClr) addrop = kMeterOp4CfgRd;
          if (addrop == kAddrOpSweep) addrop = kMeterOp4Sweep;
          return meter_addr_make(addr, addrop);
          
        case AddrType::kIdle:
          if (addrop == kAddrOpSweep) addrop = kIdletimeOpSweep;
          return idletime_addr_make(addr, addrop);
          
        default: return addr;
      }
    }

    static inline uint32_t addr_op_enabled(uint8_t addrtype, uint32_t addr) {
      switch (addrtype) {
        case AddrType::kAction:   return action_addr_enabled(addr);
        case AddrType::kStats:    return stats_addr_op_enabled(addr);
        case AddrType::kMeter:    return meter_addr_op_enabled(addr);
        case AddrType::kSelect:   return meter_addr_op_enabled(addr);
        case AddrType::kStateful: return meter_addr_op_enabled(addr);
        case AddrType::kIdle:     return idletime_addr_enabled(addr);
        default:                  return false;
      }
    }

    static inline int addr_op(uint8_t addrtype, uint32_t addr) {
      switch (addrtype) {
        case AddrType::kStats:
          switch (stats_addr_op(addr)) {
            case kStatsOpStats:     return kAddrOpNormal;
            case kStatsOpCfgRd:     return kAddrOpCfgRd;
            case kStatsOpCfgWr:     return kAddrOpCfgWr;
            case kStatsOpMoveregRd: return kAddrOpMoveregRd;
            case kStatsOpMoveregWr: return kAddrOpMoveregWr;
            case kStatsOpFsm:       return kAddrOpSweep;
            default:                return kAddrOpNop;
          }
        case AddrType::kMeter: case AddrType::kSelect: case AddrType::kStateful:
          switch (meter_addr_op4(addr)) {
            case kMeterOp4MeterColor0: case kMeterOp4MeterColor1:
            case kMeterOp4MeterColor2: case kMeterOp4MeterColor3:
              return kAddrOpNormal;
            case kMeterOp4CfgRd:     return kAddrOpCfgRd;
            case kMeterOp4CfgWr:     return kAddrOpCfgWr;
            case kMeterOp4MoveregRd: return kAddrOpMoveregRd;
            case kMeterOp4MoveregWr: return kAddrOpMoveregWr;
            case kMeterOp4Sweep:     return kAddrOpSweep;
            default:                 return kAddrOpNop;
          }
        case AddrType::kIdle:
          switch (idletime_addr_op(addr)) {
            case kIdletimeOpMarkActive: return kAddrOpNormal;
            case kIdletimeOpCfgRd:      return kAddrOpCfgRd;
            case kIdletimeOpCfgWr:      return kAddrOpCfgWr;
            case kIdletimeOpMoveregRd:  return kAddrOpMoveregRd;
            case kIdletimeOpMoveregWr:  return kAddrOpMoveregWr;
            case kIdletimeOpSweep:      return kAddrOpSweep;
            default:                    return kAddrOpNop;
          }
        default: return kAddrOpNop;
      }
    }

    static inline uint32_t addr_vaddr(uint8_t addrtype, uint32_t addr) {
      switch (addrtype) {
        case AddrType::kStats:
          return stats_addr_get_vaddr(addr);
        case AddrType::kMeter: case AddrType::kSelect: case AddrType::kStateful:
          return meter_addr_get_vaddr(addr);
        case AddrType::kIdle:
          return idletime_addr_get_vaddr(addr);
        default:
          return Address::invalid();
      }
    }

  };


}
#endif // _SHARED_ADDRESS_
