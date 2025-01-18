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

#ifndef _SHARED_MAU_MEMORY_
#define _SHARED_MAU_MEMORY_

#include <memory>
#include <unordered_map>
#include <atomic>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <indirect-addressing.h>
#include <model_core/register_block.h>
#include <rmt-log.h>


namespace MODEL_CHIP_NAMESPACE {

  class Mau;
  class MauSram;
  
  class MauMemory : public model_core::RegisterBlockIndirect<RegisterCallback> {

 public:
    // All of these ints/bools defined in rmt-config.cpp
    static bool kAllowBadAddrTypeRead;
    static bool kAllowBadAddrTypeWrite;
    static bool kAllowBadMemTypePhysRead;
    static bool kAllowBadMemTypePhysWrite;
    static bool kAllowBadMemTypeVirtRead;
    static bool kAllowBadMemTypeVirtWrite;
    static bool kAllowBadPhysRead;
    static bool kAllowBadPhysWrite;
    static bool kAllowBadVirtRead;
    static bool kAllowBadVirtWrite;
    static bool kAllowBadSramRead;
    static bool kAllowBadSramWrite;
    static bool kAllowBadTcamRead;
    static bool kAllowBadTcamWrite;
    static bool kAllowBadMapramRead;
    static bool kAllowBadMapramWrite;
    static bool kKeepFullResStats;
    static bool kAccessFullResStats;
    static int  kStatsFullVAddrPbusShift;       
    static int  kStatsVAddrPbusShift;           
    static int  kMeterVAddrPbusShift;           
    static int  kIdletimeVAddrPbusShift;        
    static int  kSelectorStatefulVAddrPbusShift;
    static bool kStatsVAddrPbusReadBubbleEmulate;
    static bool kStatsVAddrPbusWriteBubbleEmulate;
    static bool kStatsVAddrSweepBubbleEmulate;    
    static bool kStatsVAddrDumpBubbleEmulate;     
    static bool kStatsVAddrDumpWordBubbleEmulate; 
    static bool kMeterVAddrPbusReadBubbleEmulate;
    static bool kMeterVAddrPbusWriteBubbleEmulate;
    static bool kMeterVAddrSweepBubbleEmulate;    
    static bool kIdletimeVAddrPbusReadBubbleEmulate; 
    static bool kIdletimeVAddrPbusWriteBubbleEmulate;
    static bool kIdletimeVAddrSweepBubbleEmulate;    
    static bool kIdletimeVAddrDumpBubbleEmulate;     
    static bool kIdletimeVAddrDumpWordBubbleEmulate; 
    static bool kSelectorStatefulVAddrPbusReadBubbleEmulate; 
    static bool kSelectorStatefulVAddrPbusWriteBubbleEmulate;
    static bool kSelectorStatefulVAddrSweepBubbleEmulate;
    static bool kRelaxVirtWriteDataCheck;
    

    static constexpr int  kSramColumns = MauDefs::kSramColumnsPerMau;
    static constexpr int  kSramRows = MauDefs::kSramRowsPerMau;
    static constexpr int  kSrams = MauDefs::kSramsPerMau;
    static constexpr int  kSramWidth = MauDefs::kSramWidth;
    static constexpr int  kSramAddressWidth  = MauDefs::kSramAddressWidth;
    static constexpr int  kSramEntries = 1<<kSramAddressWidth;
    static constexpr int  kMapramColumns = MauDefs::kMapramColumnsPerMau;
    static constexpr int  kMapramRows = MauDefs::kMapramRowsPerMau;
    static constexpr int  kMaprams =  MauDefs::kMapramsPerMau;
    static constexpr int  kMapramWidth = MauDefs::kMapramWidth;
    static constexpr int  kMapramAddressWidth  = MauDefs::kMapramAddressWidth;
    static constexpr int  kMapramEntries = 1<<kMapramAddressWidth;
    static constexpr int  kTcams = MauDefs::kTcamsPerMau;
    static constexpr int  kTcamRows = MauDefs::kTcamRowsPerMau;
    static constexpr int  kTcamColumns = MauDefs::kTcamColumnsPerMau;
    static constexpr int  kTcamAddressWidth  = MauDefs::kTcamAddressWidth;
    static constexpr int  kTcamEntries = 1<<kTcamAddressWidth;
    static constexpr int  kLogicalTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int  kLogicalRows = MauDefs::kLogicalRowsPerMau;
    static constexpr int  kDeferredRamRows = MauDefs::kDeferredRamRows;
    static constexpr int  kDeferredRamColumns = MauDefs::kDeferredRamColumns;
    static constexpr int  kDeferredRamEntries = MauDefs::kNumEopAddrsPerDeferredRamBank;
    static constexpr uint32_t kSramValidColumnMask = MauDefs::kSramValidColumnMask;
    static constexpr uint32_t kMapramValidColumnMask = MauDefs::kMapramValidColumnMask;
    static constexpr uint64_t kBadDataWord = MauDefs::kBadDataWord;
    static constexpr uint8_t  kMapramVpnUnoccupied = MauDefs::kMapramVpnUnoccupied;
    
    

    static constexpr int  kAddrTypeRegister         = 0;
    static constexpr int  kAddrTypeInstruction      = 1;
    static constexpr int  kAddrTypePhysicalMemory   = 2;
    static constexpr int  kAddrTypeVirtualMemory    = 3;
    // We only register for top 3/4 of lower 32-bits of PBUS address space
    // [ie from 0x40000000 to 0xFFFFFFFF] so the type fields are all 1 less
    // than the corresponding AddrType
    static constexpr int  kOffsetTypeInstruction    = kAddrTypeInstruction-1;
    static constexpr int  kOffsetTypePhysicalMemory = kAddrTypePhysicalMemory-1;
    static constexpr int  kOffsetTypeVirtualMemory  = kAddrTypeVirtualMemory-1;

    
    static constexpr int  kPhysMemTypeSRAM          = 0;
    static constexpr int  kPhysMemTypeMapRAM        = 1;
    static constexpr int  kPhysMemTypeDeferStatsRAM = 2;
    static constexpr int  kPhysMemTypeDeferMeterRAM = 3;
    static constexpr int  kPhysMemTypeTCAM          = 4;
    static constexpr int  kPhysMemTypePendingSRAM   = 7;

    static constexpr int  kVirtMemTypeStats            = 0;
    static constexpr int  kVirtMemTypeMeter            = 1;
    static constexpr int  kVirtMemTypeSelectorStateful = 2;
    static constexpr int  kVirtMemTypeSelector         = 2;
    static constexpr int  kVirtMemTypeStateful         = 2; 
    static constexpr int  kVirtMemTypeIdletime         = 3;


    static uint64_t make_instr_address(int pipe, int stage, int dataSize, int instr) {
      uint64_t addr = kPipeStartAddress;
      addr |= set_pipe(pipe);
      addr |= set_stage(stage);
      addr |= set_type(kAddrTypeInstruction);
      addr |= set_instr_data_size(dataSize);
      addr |= set_instr(instr);
      return addr;
    }
    static uint64_t make_phys_address(int pipe, int stage, int pMemType,
                                      int row, int col, int index) {
      uint64_t addr = kPipeStartAddress;
      addr |= set_pipe(pipe);
      addr |= set_stage(stage);
      addr |= set_type(kAddrTypePhysicalMemory);
      addr |= set_phys_mem_type(pMemType);
      addr |= set_phys_row(row);
      addr |= set_phys_col(col);
      addr |= set_phys_index(index);
      return addr;
    }
    static uint64_t make_virt_address(int pipe, int stage, int vMemType,
                                      int logtab, int vAddr) {
      uint64_t addr = kPipeStartAddress;
      addr |= set_pipe(pipe);
      addr |= set_stage(stage);
      addr |= set_type(kAddrTypeVirtualMemory);
      addr |= set_virt_mem_type(vMemType);
      addr |= set_virt_log_tab(logtab);
      addr |= set_virt_addr(vAddr);
      return addr;
    }

    

 private:
    // Register from 0x40000000 to 0xFFFFFFFF
    static constexpr uint64_t kPhysVirtBase     = kPipeStartAddress + UINT64_C(0x00040000000);
    static constexpr uint64_t kPhysVirtSize     = UINT64_C(0x000C0000000);
    static constexpr int      kPhysVirtShift    = 30;
    static constexpr int      kPhysVirtBits     = 1;
    static constexpr int      kPhysVirtMask     = (1<<kPhysVirtBits)-1;
    static constexpr int      kTypeShift        = 30;
    static constexpr int      kTypeBits         = 2;
    static constexpr int      kTypeMask         = (1<<kTypeBits)-1;

    static constexpr int      kInstrShift       = 0;
    static constexpr int      kInstrBits        = 28;
    static constexpr int      kInstrMask        = (1<<kInstrBits)-1;
    static constexpr int      kDataSizeShift    = kInstrShift+kInstrBits; // 28
    static constexpr int      kDataSizeBits     = 2;
    static constexpr int      kDataSizeMask     = (1<<kDataSizeBits)-1;

    static constexpr int      kPhysIndexShift   = 0;
    static constexpr int      kPhysIndexBits    = 10;
    static constexpr int      kPhysIndexMask    = (1<<kPhysIndexBits)-1;
    static constexpr int      kPhysColShift     = kPhysIndexShift+kPhysIndexBits; // 10
    static constexpr int      kPhysColBits      = 4;
    static constexpr int      kPhysColMask      = (1<<kPhysColBits)-1;
    static constexpr int      kPhysRowShift     = kPhysColShift+kPhysColBits; // 14
    static constexpr int      kPhysRowBits      = 4;
    static constexpr int      kPhysRowMask      = (1<<kPhysRowBits)-1;
    static constexpr int      kPhysMemTypeShift = kPhysRowShift+kPhysRowBits; // 18
    static constexpr int      kPhysMemTypeBits  = 3;
    static constexpr int      kPhysMemTypeMask  = (1<<kPhysMemTypeBits)-1;
    static constexpr int      kPhysZerosShift   = kPhysMemTypeShift+kPhysMemTypeBits; // 21
    static constexpr int      kPhysZerosBits    = 9;
    static constexpr int      kPhysZerosMask    = (1<<kPhysZerosBits)-1;

    static constexpr int      kVirtVAddrShift   = 0;
    static constexpr int      kVirtVAddrBits    = 21;
    static constexpr int      kVirtVAddrMask    = (1<<kVirtVAddrBits)-1;
    static constexpr int      kVirtLogTabShift  = kVirtVAddrShift+kVirtVAddrBits; // 21
    static constexpr int      kVirtLogTabBits   = 4;
    static constexpr int      kVirtLogTabMask   = (1<<kVirtLogTabBits)-1;
    static constexpr int      kVirtMemTypeShift = kVirtLogTabShift+kVirtLogTabBits; // 25
    static constexpr int      kVirtMemTypeBits  = 2;
    static constexpr int      kVirtMemTypeMask  = (1<<kVirtMemTypeBits)-1;
    static constexpr int      kVirtZerosShift   = kVirtMemTypeShift+kVirtMemTypeBits; // 27
    static constexpr int      kVirtZerosBits    = 3;
    static constexpr int      kVirtZerosMask    = (1<<kVirtZerosBits)-1;



    

    static bool is_reg(uint64_t addr)            { return (get_type(addr) == kAddrTypeRegister); }
    static bool is_instr(uint64_t addr)          { return (get_type(addr) == kAddrTypeInstruction); }
    static bool is_phys(uint64_t addr)           { return (get_type(addr) == kAddrTypePhysicalMemory); }
    static bool is_virt(uint64_t addr)           { return (get_type(addr) == kAddrTypeVirtualMemory); }

    static int  get_type(uint64_t addr)          { return (addr >> kTypeShift) & kTypeMask; }
    static int  get_data_size(uint64_t addr)     { return (addr >> kDataSizeShift) & kDataSizeMask; }
    
    static int  get_instr(uint64_t addr)         { return (addr >> kInstrShift) & kInstrMask; }

    static int  get_phys_zeros(uint64_t addr)    { return (addr >> kPhysZerosShift) & kPhysZerosMask; }
    static int  get_phys_mem_type(uint64_t addr) { return (addr >> kPhysMemTypeShift) & kPhysMemTypeMask; }
    static int  get_phys_row(uint64_t addr)      { return (addr >> kPhysRowShift) & kPhysRowMask; }
    static int  get_phys_col(uint64_t addr)      { return (addr >> kPhysColShift) & kPhysColMask; }
    static int  get_phys_index(uint64_t addr)    { return (addr >> kPhysIndexShift) & kPhysIndexMask; }
    static int  get_virt_zeros(uint64_t addr)    { return (addr >> kVirtZerosShift) & kVirtZerosMask; }
    static int  get_virt_mem_type(uint64_t addr) { return (addr >> kVirtMemTypeShift) & kVirtMemTypeMask; }
    static int  get_virt_log_tab(uint64_t addr)  { return (addr >> kVirtLogTabShift) & kVirtLogTabMask; }
    static int  get_virt_addr(uint64_t addr)     { return (addr >> kVirtVAddrShift) & kVirtVAddrMask; }
    
    static uint64_t set_pipe(int pipe)           { return static_cast<uint64_t>(pipe & kIndPipeMask) << kIndPipeShift; }
    static uint64_t set_stage(int stage)         { return static_cast<uint64_t>(stage & kIndStageMask) << kIndStageShift; }
    static uint64_t set_type(int type)           { return static_cast<uint64_t>(type & kTypeMask) << kTypeShift; }
    static uint64_t set_instr_data_size(int ds)  { return static_cast<uint64_t>(ds & kDataSizeMask) << kDataSizeShift; }
    static uint64_t set_instr(int instr)         { return static_cast<uint64_t>(instr & kInstrMask) << kInstrShift; }
    static uint64_t set_phys_virt(int pv)        { return static_cast<uint64_t>(pv & kPhysVirtMask) << kPhysVirtShift; }
    static uint64_t set_phys_mem_type(int mt)    { return static_cast<uint64_t>(mt & kPhysMemTypeMask) << kPhysMemTypeShift; }
    static uint64_t set_phys_row(int row)        { return static_cast<uint64_t>(row & kPhysRowMask) << kPhysRowShift; }
    static uint64_t set_phys_col(int col)        { return static_cast<uint64_t>(col & kPhysColMask) << kPhysColShift; }
    static uint64_t set_phys_index(int index)    { return static_cast<uint64_t>(index & kPhysIndexMask) << kPhysIndexShift; }
    static uint64_t set_virt_mem_type(int mt)    { return static_cast<uint64_t>(mt & kVirtMemTypeMask) << kVirtMemTypeShift; }
    static uint64_t set_virt_log_tab(int lt)     { return static_cast<uint64_t>(lt & kVirtLogTabMask) << kVirtLogTabShift; }
    static uint64_t set_virt_addr(int addr)      { return static_cast<uint64_t>(addr & kVirtVAddrMask) << kVirtVAddrShift; }
    

    static uint64_t get_pipe_stage_base(int pipe, int stage) {
      uint64_t addr = kPhysVirtBase;
      addr |= set_pipe(pipe);
      addr |= set_stage(stage);
      return addr;
    }

    static uint32_t make_map_key(int logical_table, uint32_t addr) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      uint32_t memtype = static_cast<uint32_t>(kVirtMemTypeStats & kVirtMemTypeMask) << kVirtMemTypeShift;
      uint32_t logtab = static_cast<uint32_t>(logical_table & kVirtLogTabMask) << kVirtLogTabShift;
      uint32_t vaddr = (addr & static_cast<uint32_t>(kVirtVAddrMask)) << kVirtVAddrShift;
      return memtype|logtab|vaddr;
    }

    
 public:
    MauMemory(int chipIndex, int pipeIndex, int stageIndex, Mau *mau) 
        : RegisterBlockIndirect(chipIndex, get_pipe_stage_base(pipeIndex, stageIndex),
                                            kPhysVirtSize, false, 0, 0, "MauMemory"),
        mau_(mau), full_res_stats_() {
    }
    virtual ~MauMemory() {
      full_res_stats_.clear();
    }

    
    bool read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const {
      int type = get_type(offset);
      switch (type) {
        case kOffsetTypeInstruction:    instr_read(offset, data0, data1, T); break;
        case kOffsetTypePhysicalMemory: phys_read(offset, data0, data1, T);  break;
        case kOffsetTypeVirtualMemory:  virt_read(offset, data0, data1, T);  break;
        default: bad_addrtype_read(offset, data0, data1, T); break;
      }
      return true;
    }
    bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
      int type = get_type(offset);
      switch (type) {
        case kOffsetTypeInstruction:    instr_write(offset, data0, data1, T); break;
        case kOffsetTypePhysicalMemory: phys_write(offset, data0, data1, T);  break;
        case kOffsetTypeVirtualMemory:  virt_write(offset, data0, data1, T);  break;
        default: bad_addrtype_write(offset, data0, data1, T); break;
      }
      return true;
    }

    
    // INSTRUCTION
    void instr_read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const;
    void instr_write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
    
    // PHYSICAL 
    void phys_read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const;
    void phys_write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
    
    // VIRTUAL
    void virt_read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const;
    void virt_write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);


    // Just for DV - access to full_res_stats map so they can iterate over it
    std::unordered_map<uint32_t, std::array<uint64_t,2>>* full_res_stats() { return &full_res_stats_; }



    std::string to_string(bool print_zeros = false, std::string indent_string = "") const {
      // this is meant to print the whole of the RegisterIndirectBlock, 
      // it probably doesn't make sense to implement it here as printing
      // the whole thing would be too much
      return "";
    }
    std::string to_string(uint64_t offset,bool print_zeros = false, std::string indent_string = "") const {
      return "mau-memory to_string at offset - not implemented!\n";
    }

    
 private:
    void bad_addrtype_read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const;
    void bad_addrtype_write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);

    void sram_read(int row, int col, int index, uint64_t* data0, uint64_t* data1, uint64_t T) const;
    void sram_write(int row, int col, int index, uint64_t data0, uint64_t data1, uint64_t T);
    void sram_pending_get(int row, int col, int index, uint64_t* data0, uint64_t* data1, uint64_t T) const;
    void sram_pending_set(int row, int col, int index, uint64_t data0, uint64_t data1, uint64_t T);
    void tcam_read(int row, int col, int index, uint64_t* data0, uint64_t* data1, uint64_t T) const;
    void tcam_write(int row, int col, int index, uint64_t data0, uint64_t data1, uint64_t T);
    void mapram_read(int row, int col, int index, uint64_t* data0, uint64_t *data1, uint64_t T) const;
    void mapram_write(int row, int col, int index, uint64_t data0, uint64_t data1, uint64_t T);
    void deferred_stats_ram_read(int row, int col, int index, uint64_t* data0, uint64_t *data1, uint64_t T) const;
    void deferred_stats_ram_write(int row, int col, int index, uint64_t data0, uint64_t data1, uint64_t T);
    void deferred_meter_ram_read(int row, int col, int index, uint64_t* data0, uint64_t *data1, uint64_t T) const;
    void deferred_meter_ram_write(int row, int col, int index, uint64_t data0, uint64_t data1, uint64_t T);
    void bad_memtype_phys_read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const;
    void bad_memtype_phys_write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
                                
    bool check_sram_write_data(uint64_t data0, uint64_t data1, const BitVector<kSramWidth>& mask);
    bool check_mapram_write_data(uint64_t data, uint64_t mask);
    MauSram *virt_find_sram(int sramtype, int logical_table,
                            uint8_t addrtype, uint32_t addr,
                            uint8_t vpn, uint16_t index,
                            bool create, int sramtype2=0) const;    
    void stats_virt_read(int logical_table, uint32_t addr, uint64_t* data0, uint64_t *data1, uint64_t T) const;
    void stats_virt_write(int logical_table, uint32_t addr, uint64_t data0, uint64_t data1, uint64_t T);
    void meter_virt_read(int logical_table, uint32_t addr, uint64_t* data0, uint64_t *data1, uint64_t T) const;
    void meter_virt_write(int logical_table, uint32_t addr, uint64_t data0, uint64_t data1, uint64_t T);
    void selector_stateful_virt_read(int logical_table, uint32_t addr, uint64_t* data0, uint64_t *data1, uint64_t T) const;
    void selector_stateful_virt_write(int logical_table, uint32_t addr, uint64_t data0, uint64_t data1, uint64_t T);
    void idletime_virt_read(int logical_table, uint32_t addr, uint64_t* data0, uint64_t *data1, uint64_t T) const;
    void idletime_virt_write(int logical_table, uint32_t addr, uint64_t data0, uint64_t data1, uint64_t T);
    void bad_memtype_virt_read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const;
    void bad_memtype_virt_write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
    

 public:
    MauSram *stats_virt_find_sram(int logical_table, uint32_t addr, bool create=false) const;
    MauSram *meter_virt_find_sram(int logical_table, uint32_t addr, bool create=false) const;
    MauSram *selector_stateful_virt_find_sram(int logical_table, uint32_t addr, bool create=false) const;
    void stats_virt_read_full(int logical_table, uint32_t addr, uint64_t* data0, uint64_t *data1, uint64_t T=UINT64_C(0)) const;
    void stats_virt_write_full(int logical_table, uint32_t addr, uint64_t data0, uint64_t data1, uint64_t T=UINT64_C(0));

 private:
    Mau                                                    *mau_;
    std::unordered_map< uint32_t, std::array<uint64_t,2> >  full_res_stats_;
  };

}
#endif // _SHARED_MAU_MEMORY_


