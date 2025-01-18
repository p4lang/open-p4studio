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
#include <string>
#include <exception>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-memory.h>


namespace MODEL_CHIP_NAMESPACE {

    // TOP-LEVEL ACCESS
    void MauMemory::bad_addrtype_read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const {
      RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead, kAllowBadAddrTypeRead),
                  "MauMemory::bad_addrtype_read(0x%016" PRIx64 ") - "
                  "Invalid address type %d\n", offset, get_type(offset));
      if (data0 != NULL) *data0 = kBadDataWord;
      if (data1 != NULL) *data1 = kBadDataWord;
      mau_->mau_info_incr(MAU_BAD_CFG_READS);
      if (!kAllowBadAddrTypeRead) { THROW_ERROR(-3); } // For DV

    }
    void MauMemory::bad_addrtype_write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
      RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead,kAllowBadAddrTypeWrite),
                  "MauMemory::bad_addrtype_write(0x%016" PRIx64 ") - "
                  "Invalid address type %d\n", offset, get_type(offset));
      mau_->mau_info_incr(MAU_BAD_CFG_WRITES);
      if (!kAllowBadAddrTypeWrite) { THROW_ERROR(-3); } // For DV
    }



    // INSTRUCTIONS
    void MauMemory::instr_read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const {
      //T = UINT64_C(0); // Zeroise_T
      // Instructions should always be a WRITE
      //RMT_ASSERT(0);
      RMT_LOG_OBJ(mau_,RmtDebug::warn(RmtDebug::kRmtDebugRead),
                  "MauMemory::instr_read(0x%" PRIx64 "): UNEXPECTED instr_read OP!!\n", offset);
    }
    void MauMemory::instr_write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
      //T = UINT64_C(0); // Zeroise_T
      MauOpHandler *op_handler = mau_->mau_op_handler();
      RMT_ASSERT(op_handler != NULL);
      int data_size = get_data_size(offset);
      int instr = get_instr(offset);
      op_handler->instr_handle(instr, data_size, data0, data1, T);
      mau_->mau_info_incr(MAU_INSTR_WRITES);
    }


    // PHYSICAL MEMORY ACCESS
    void MauMemory::phys_read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const {
      RMT_ASSERT(!mau_->atomic_in_progress());
      //T = UINT64_C(0); // Zeroise_T
      int mem_type = get_phys_mem_type(offset);
      int row = get_phys_row(offset);
      int col = get_phys_col(offset);
      int index = get_phys_index(offset);
      int zeros = get_phys_zeros(offset);
      if (zeros != 0) {
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead,kAllowBadPhysRead),
                    "MauMemory::phys_read(0x%016" PRIx64 ") - "
                    "Address[%d:%d] not zero\n",
                    offset, kPhysZerosShift+kPhysZerosBits-1, kPhysZerosShift);
        if (data0 != NULL) *data0 = kBadDataWord;
        if (data1 != NULL) *data1 = kBadDataWord;
        if (!kAllowBadPhysRead) { THROW_ERROR(-3); } // For DV
      } else {
        switch (mem_type) {
          case kPhysMemTypeSRAM:          sram_read(row, col, index, data0, data1, T);         break;
          //case kPhysMemTypePendingSRAM:   sram_pending_get(row, col, index, data0, data1, T);  break;
          case kPhysMemTypeTCAM:          tcam_read(row, col, index, data0, data1, T);         break;
          case kPhysMemTypeMapRAM:        mapram_read(row, col, index, data0, data1, T);       break;
          case kPhysMemTypeDeferStatsRAM: deferred_stats_ram_read(row, col, index, data0, data1, T); break;
          case kPhysMemTypeDeferMeterRAM: deferred_meter_ram_read(row, col, index, data0, data1, T); break;
          default: bad_memtype_phys_read(offset, data0, data1, T); break;
        }
      }
    }
    void MauMemory::phys_write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
      RMT_ASSERT(!mau_->atomic_in_progress());
      //T = UINT64_C(0); // Zeroise_T
      int mem_type = get_phys_mem_type(offset);
      int row = get_phys_row(offset);
      int col = get_phys_col(offset);
      int index = get_phys_index(offset);
      int zeros = get_phys_zeros(offset);
      if (zeros != 0) {
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite,kAllowBadPhysWrite),
                    "MauMemory::phys_write(0x%016" PRIx64 ") - "
                    "Address[%d:%d] not zero\n",
                    offset, kPhysZerosShift+kPhysZerosBits-1, kPhysZerosShift);
        if (!kAllowBadPhysWrite) { THROW_ERROR(-3); } // For DV
      } else {
        switch (mem_type) {
          case kPhysMemTypeSRAM:          sram_write(row, col, index, data0, data1, T);        break;
          case kPhysMemTypePendingSRAM:   sram_pending_set(row, col, index, data0, data1, T);  break;
          case kPhysMemTypeTCAM:          tcam_write(row, col, index, data0, data1, T);        break;
          case kPhysMemTypeMapRAM:        mapram_write(row, col, index, data0, data1, T);      break;
          case kPhysMemTypeDeferStatsRAM: deferred_stats_ram_write(row, col, index, data0, data1, T); break;
          case kPhysMemTypeDeferMeterRAM: deferred_meter_ram_write(row, col, index, data0, data1, T); break;
          default: bad_memtype_phys_write(offset, data0, data1, T); break;
        }
      }
    }

    void MauMemory::sram_read(int row, int col, int index,
                              uint64_t* data0, uint64_t* data1, uint64_t T) const {
      MauSram *sram = NULL;
      if ((row >= 0) && (row < kSramRows) &&
          (col >= 0) && (col < kSramColumns) &&
          (index >= 0) && (index < kSramEntries)) {
        sram = mau_->sram_lookup(row,col);
        if (sram != NULL) sram->read(index, data0, data1, T);
      }
      if (sram == NULL) {
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead,kAllowBadSramRead),
                    "MauMemory::sram_read(%d,%d,%d) - "
                    "Invalid row|col|index\n", row, col, index);
        if (data0 != NULL) *data0 = kBadDataWord;
        if (data1 != NULL) *data1 = kBadDataWord;
        mau_->mau_info_incr(MAU_BAD_CFG_READS);
        if (!kAllowBadSramRead) { THROW_ERROR(-3); } // For DV
      } else {
        mau_->mau_info_incr(MAU_SRAM_CFG_READS);
      }
    }
    void MauMemory::sram_write(int row, int col, int index,
                               uint64_t data0, uint64_t data1, uint64_t T) {
      MauSram *sram = NULL;
      if ((row >= 0) && (row < kSramRows) &&
          (col >= 0) && (col < kSramColumns) &&
          (index >= 0) && (index < kSramEntries)) {
        sram = mau_->sram_lookup(row,col);
        if (sram != NULL) sram->write(index, data0, data1, T);
      }
      if (sram == NULL) {
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite,kAllowBadSramWrite),
                    "MauMemory::sram_write(%d,%d,%d) - IGNORED - "
                    "Invalid row|col|index\n", row, col, index);
        mau_->mau_info_incr(MAU_BAD_CFG_WRITES);
        if (!kAllowBadSramWrite) { THROW_ERROR(-3); } // For DV
      } else {
        mau_->mau_info_incr(MAU_SRAM_CFG_WRITES);
      }
    }

    void MauMemory::sram_pending_get(int row, int col, int index,
                                     uint64_t* data0, uint64_t* data1, uint64_t T) const {
      if (!RmtObject::is_jbay_or_later()) {
        uint64_t offset = set_phys_mem_type(kPhysMemTypePendingSRAM) |
            set_phys_row(row) | set_phys_col(col) | set_phys_index(index);
        bad_addrtype_read(offset, data0, data1, T);
      } else {
        MauSram *sram = NULL;
        if ((row >= 0) && (row < kSramRows) &&
            (col >= 0) && (col < kSramColumns) &&
            (index >= 0) && (index < kSramEntries)) {
          sram = mau_->sram_lookup(row,col);
          if (sram != NULL) {
            if (!sram->pending_get(index, data0, data1, T)) {
              if (data0 != NULL) *data0 = kBadDataWord;
              if (data1 != NULL) *data1 = kBadDataWord;
            }
          }
        }
        if (sram == NULL) {
          RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead,kAllowBadSramRead),
                      "MauMemory::sram_pending_get(%d,%d,%d) - "
                      "Invalid row|col|index\n", row, col, index);
          if (data0 != NULL) *data0 = kBadDataWord;
          if (data1 != NULL) *data1 = kBadDataWord;
          mau_->mau_info_incr(MAU_BAD_CFG_READS);
          if (!kAllowBadSramRead) { THROW_ERROR(-3); } // For DV
        } else {
          mau_->mau_info_incr(MAU_SRAM_CFG_READS);
        }
      }
    }
    void MauMemory::sram_pending_set(int row, int col, int index,
                                     uint64_t data0, uint64_t data1, uint64_t T) {
      if (!RmtObject::is_jbay_or_later()) {
        uint64_t offset = set_phys_mem_type(kPhysMemTypePendingSRAM) |
            set_phys_row(row) | set_phys_col(col) | set_phys_index(index);
        bad_addrtype_write(offset, data0, data1, T);
      } else {
        MauSram *sram = NULL;
        if ((row >= 0) && (row < kSramRows) &&
            (col >= 0) && (col < kSramColumns) &&
            (index >= 0) && (index < kSramEntries)) {
          sram = mau_->sram_lookup(row,col);
          if (sram != NULL) sram->pending_set(index, data0, data1, T);
        }
        if (sram == NULL) {
          RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite,kAllowBadSramWrite),
                      "MauMemory::sram_pending_set(%d,%d,%d) - IGNORED - "
                      "Invalid row|col|index\n", row, col, index);
          mau_->mau_info_incr(MAU_BAD_CFG_WRITES);
          if (!kAllowBadSramWrite) { THROW_ERROR(-3); } // For DV
        } else {
          mau_->mau_info_incr(MAU_SRAM_CFG_WRITES);
        }
      }
    }

    void MauMemory::tcam_read(int row, int col, int index,
                              uint64_t* data0, uint64_t* data1, uint64_t T) const {
      MauTcam *tcam = NULL;
      if ((row >= 0) && (row < kTcamRows) &&
          (col >= 0) && (col < kTcamColumns) &&
          (index >= 0) && (index < kTcamEntries)) {
        tcam = mau_->tcam_lookup(row,col);
        if (tcam != NULL) tcam->read(index, data0, data1, T);
      }
      if (tcam == NULL) {
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead,kAllowBadTcamRead),
                    "MauMemory::tcam_read(%d,%d,%d) - "
                    "Invalid row|col|index\n", row, col, index);
        if (data0 != NULL) *data0 = kBadDataWord;
        if (data1 != NULL) *data1 = kBadDataWord;
        mau_->mau_info_incr(MAU_BAD_CFG_READS);
        if (!kAllowBadTcamRead) { THROW_ERROR(-3); } // For DV
      } else {
        mau_->mau_info_incr(MAU_TCAM_CFG_READS);
      }
    }
    void MauMemory::tcam_write(int row, int col, int index,
                               uint64_t data0, uint64_t data1, uint64_t T) {
      MauTcam *tcam = NULL;
      if ((row >= 0) && (row < kTcamRows) &&
          (col >= 0) && (col < kTcamColumns) &&
          (index >= 0) && (index < kTcamEntries)) {
        tcam = mau_->tcam_lookup(row,col);
        if (tcam != NULL) tcam->write(index, data0, data1, T);
      }
      if (tcam == NULL) {
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite,kAllowBadTcamWrite),
                    "MauMemory::tcam_write(%d,%d,%d) - IGNORED - "
                    "Invalid row|col|index\n", row, col, index);
        mau_->mau_info_incr(MAU_BAD_CFG_READS);
        if (!kAllowBadTcamWrite) { THROW_ERROR(-3); } // For DV
      } else {
        mau_->mau_info_incr(MAU_TCAM_CFG_WRITES);
      }
    }

    void MauMemory::mapram_read(int row, int col, int index,
                                uint64_t* data0, uint64_t* data1, uint64_t T) const {
      MauMapram *mapram = NULL;
      if ((row >= 0) && (row < kMapramRows) &&
          (col >= 0) && (col < kMapramColumns) &&
          (index >= 0) && (index < kMapramEntries)) {
        mapram = mau_->mapram_lookup(row,col);
        if (mapram != NULL) mapram->read(index, data0, data1, T);
      }
      if (mapram == NULL) {
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead,kAllowBadMapramRead),
                    "MauMemory::mapram_read(%d,%d,%d) - "
                    "Invalid row|col|index\n", row, col, index);
        if (data0 != NULL) *data0 = kBadDataWord;
        if (data1 != NULL) *data1 = kBadDataWord;
        mau_->mau_info_incr(MAU_BAD_CFG_READS);
        if (!kAllowBadMapramRead) { THROW_ERROR(-3); } // For DV
      } else {
        mau_->mau_info_incr(MAU_MAPRAM_CFG_READS);
      }
    }
    void MauMemory::mapram_write(int row, int col, int index,
                                 uint64_t data0, uint64_t data1, uint64_t T) {
      MauMapram *mapram = NULL;
      if ((row >= 0) && (row < kMapramRows) &&
          (col >= 0) && (col < kMapramColumns) &&
          (index >= 0) && (index < kMapramEntries)) {
        mapram = mau_->mapram_lookup(row,col);
        if (mapram != NULL) mapram->write(index, data0, data1, T);
      }
      if (mapram == NULL) {
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite,kAllowBadMapramWrite),
                    "MauMemory::mapram_write(%d,%d,%d) - IGNORED - "
                    "Invalid row|col|index\n", row, col, index);
        mau_->mau_info_incr(MAU_BAD_CFG_READS);
        if (!kAllowBadMapramWrite) { THROW_ERROR(-3); } // For DV
      } else {
        mau_->mau_info_incr(MAU_MAPRAM_CFG_WRITES);
      }
    }

    void MauMemory::deferred_stats_ram_read(int row, int col, int index,
                                            uint64_t* data0, uint64_t *data1, uint64_t T) const {
      // Here row is [0,3] to select which stats deferred ram and
      // col should be 0 as there is only 1 bank in each deferred ram.
      // Entries should be in [0,160)

      // Map stats row select [0,3] to logical row [0,15]
      int logrow = MauStatsAlu::get_stats_alu_logrow_index(row);
      if ((row >= 0) && (row < kDeferredRamRows) && (logrow >= 0) &&
          (col >= 0) && (col < kDeferredRamColumns) &&
          (index >= 0) && (index < kDeferredRamEntries)) {
        MauAddrDist *addr_dist = mau_->mau_addr_dist();
        RMT_ASSERT(addr_dist != NULL);
        uint8_t eop_num = (col * kDeferredRamEntries) + index;
        uint32_t stats_addr = 0u;
        if (addr_dist->row_read_eop_stats_addr(logrow, eop_num, &stats_addr)) {
          if (data0 != NULL) *data0 = static_cast<uint64_t>(stats_addr);
          mau_->mau_info_incr(MAU_DEFRAM_CFG_READS);
        }
      } else {
        mau_->mau_info_incr(MAU_BAD_CFG_READS);
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead),
                    "MauMemory::deferred_stats_ram_read(%d,%d,%d) - IGNORED "
                    "- Invalid row|col|index\n", row, col, index);
      }
    }
    void MauMemory::deferred_stats_ram_write(int row, int col, int index,
                                             uint64_t data0, uint64_t data1, uint64_t T) {
      // Map stats row select [0,3] to logical row [0,15]
      int logrow = MauStatsAlu::get_stats_alu_logrow_index(row);
      if ((row >= 0) && (row < kDeferredRamRows) && (logrow >= 0) &&
          (col >= 0) && (col < kDeferredRamColumns) &&
          (index >= 0) && (index < kDeferredRamEntries)) {
        MauAddrDist *addr_dist = mau_->mau_addr_dist();
        RMT_ASSERT(addr_dist != NULL);
        uint8_t eop_num = (col * kDeferredRamEntries) + index;
        uint32_t stats_addr = static_cast<uint32_t>(data0 & 0xFFFFFFFF);
        addr_dist->row_write_eop_stats_addr(logrow, eop_num, stats_addr);
        mau_->mau_info_incr(MAU_DEFRAM_CFG_WRITES);
      } else {
        mau_->mau_info_incr(MAU_BAD_CFG_WRITES);
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite),
                    "MauMemory::deferred_stats_ram_write(%d,%d,%d) - IGNORED "
                    "- Invalid row|col|index\n", row, col, index);
      }
    }
    void MauMemory::deferred_meter_ram_read(int row, int col, int index,
                                            uint64_t* data0, uint64_t *data1, uint64_t T) const {
      // Here row is [0,3] to select which meter deferred ram and
      // col should be 0 as there is only 1 bank in each deferred ram.
      // Entries should be in [0,160)

      // Map meter row select [0,3] to logical row [0,15]
      int logrow = MauMeterAlu::get_meter_alu_logrow_index(row);
      if ((row >= 0) && (row < kDeferredRamRows) && (logrow >= 0) &&
          (col >= 0) && (col < kDeferredRamColumns) &&
          (index >= 0) && (index < kDeferredRamEntries)) {
        MauAddrDist *addr_dist = mau_->mau_addr_dist();
        RMT_ASSERT(addr_dist != NULL);
        uint8_t eop_num = (col * kDeferredRamEntries) + index;
        uint32_t meter_addr = 0u;
        if (addr_dist->row_read_eop_meter_addr(logrow, eop_num, &meter_addr)) {
          if (data0 != NULL) *data0 = static_cast<uint64_t>(meter_addr);
          mau_->mau_info_incr(MAU_DEFRAM_CFG_READS);
        }
      } else {
        mau_->mau_info_incr(MAU_BAD_CFG_READS);
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead),
                    "MauMemory::deferred_meter_ram_read(%d,%d,%d) - IGNORED "
                    "- Invalid row|col|index\n", row, col, index);
      }
    }
    void MauMemory::deferred_meter_ram_write(int row, int col, int index,
                                             uint64_t data0, uint64_t data1, uint64_t T) {
      // Map meter row select [0,3] to logical row [0,15]
      int logrow = MauMeterAlu::get_meter_alu_logrow_index(row);
      if ((row >= 0) && (row < kDeferredRamRows) && (logrow >= 0) &&
          (col >= 0) && (col < kDeferredRamColumns) &&
          (index >= 0) && (index < kDeferredRamEntries)) {
        MauAddrDist *addr_dist = mau_->mau_addr_dist();
        RMT_ASSERT(addr_dist != NULL);
        uint8_t eop_num = (col * kDeferredRamEntries) + index;
        uint32_t meter_addr = static_cast<uint32_t>(data0 & 0xFFFFFFFF);
        addr_dist->row_write_eop_meter_addr(logrow, eop_num, meter_addr);
        mau_->mau_info_incr(MAU_DEFRAM_CFG_WRITES);
      } else {
        mau_->mau_info_incr(MAU_BAD_CFG_WRITES);
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite),
                    "MauMemory::deferred_meter_ram_write(%d,%d,%d) - IGNORED "
                    "- Invalid row|col|index\n", row, col, index);
      }
    }
    void MauMemory::bad_memtype_phys_read(uint64_t offset,
                                          uint64_t* data0, uint64_t* data1, uint64_t T) const {
      RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead,kAllowBadMemTypePhysRead),
                  "MauMemory::bad_memtype_phys_read(0x%016" PRIx64 ") - "
                  "Invalid mem type %d\n", offset, get_phys_mem_type(offset));
      if (data0 != NULL) *data0 = kBadDataWord;
      if (data1 != NULL) *data1 = kBadDataWord;
      mau_->mau_info_incr(MAU_BAD_CFG_READS);
      if (!kAllowBadMemTypePhysRead) { THROW_ERROR(-3); } // For DV
    }
    void MauMemory::bad_memtype_phys_write(uint64_t offset,
                                           uint64_t data0, uint64_t data1, uint64_t T) {
      RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite,kAllowBadMemTypePhysWrite),
                  "MauMemory::bad_memtype_phys_write(0x%016" PRIx64 ") - "
                  "Invalid mem type %d\n", offset, get_phys_mem_type(offset));
      mau_->mau_info_incr(MAU_BAD_CFG_WRITES);
      if (!kAllowBadMemTypePhysWrite) { THROW_ERROR(-3); } // For DV
    }




    // VIRTUAL MEMORY ACCESS
    bool MauMemory::check_sram_write_data(uint64_t data0, uint64_t data1,
                                          const BitVector<kSramWidth>& mask) {
      if (kRelaxVirtWriteDataCheck) return true;
      // Check whether bits in Data0/Data1 NOT covered by mask are all 0s
      return (((data0 & ~mask.get_word( 0)) == UINT64_C(0)) &&
              ((data1 & ~mask.get_word(64)) == UINT64_C(0)));
    }
    bool MauMemory::check_mapram_write_data(uint64_t data, uint64_t mask) {
      if (kRelaxVirtWriteDataCheck) return true;
      // Limit mask for check to physical width of mapram
      uint64_t check_mask = ~mask & (UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64-kMapramWidth));
      return ((data & check_mask) == UINT64_C(0));
    }

    void MauMemory::virt_read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const {
      RMT_ASSERT(!mau_->atomic_in_progress());
      //T = UINT64_C(0); // Zeroise_T
      int data_size = get_data_size(offset);
      int mem_type = get_virt_mem_type(offset);
      int ltab = get_virt_log_tab(offset);
      uint32_t addr = static_cast<uint32_t>(get_virt_addr(offset));
      int zeros = get_virt_zeros(offset);
      bool zeros_ok = (zeros == 0);

      if (kAccessFullResStats) {
        // Not allowed in RTL_MODE (when comparing against Emulator say)
        // Special case to handle abuse of data_size field to indicate full-res stats
        if (mem_type == kVirtMemTypeStats) zeros_ok = ((zeros == 0) || (zeros == 0x3<<1));
      }
      if (!zeros_ok) {
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead,kAllowBadVirtRead),
                    "MauMemory::virt_read(0x%016" PRIx64 ") - "
                    "Address[%d:%d] not zero\n",
                    offset, kVirtZerosShift+kVirtZerosBits-1, kVirtZerosShift);
        if (data0 != NULL) *data0 = kBadDataWord;
        if (data1 != NULL) *data1 = kBadDataWord;
        if (!kAllowBadVirtRead) { THROW_ERROR(-3); } // For DV
      } else {
        switch (mem_type) {
          case kVirtMemTypeStats:
            // (Ab)use data_size field to indicate a full-resolution stats read
            if (data_size == 0x3)
              stats_virt_read_full(ltab, addr, data0, data1, T);
            else
              stats_virt_read(ltab, addr, data0, data1, T);
            break;
          case kVirtMemTypeMeter:            meter_virt_read(ltab, addr, data0, data1, T); break;
          case kVirtMemTypeSelectorStateful: selector_stateful_virt_read(ltab, addr, data0, data1, T); break;
          case kVirtMemTypeIdletime:         idletime_virt_read(ltab, addr, data0, data1, T); break;
          default: bad_memtype_virt_read(offset, data0, data1, T); break;
        }
      }
    }
    void MauMemory::virt_write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
      RMT_ASSERT(!mau_->atomic_in_progress());
      int data_size = get_data_size(offset);
      int mem_type = get_virt_mem_type(offset);
      int ltab = get_virt_log_tab(offset);
      uint32_t addr = static_cast<uint32_t>(get_virt_addr(offset));
      int zeros = get_virt_zeros(offset);
      bool zeros_ok = (zeros == 0);
      //if (mem_type != kVirtMemTypeSelectorStateful) T = UINT64_C(0); // Zeroise_T unless SelectorStateful

      if (kAccessFullResStats) {
        // Not allowed in RTL_MODE (when comparing against Emulator say)
        // Special case to handle abuse of data_size field to indicate full-res stats
        if (mem_type == kVirtMemTypeStats) zeros_ok = ((zeros == 0) || (zeros == 0x3<<1));
      }
      if (!zeros_ok) {
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite,kAllowBadVirtWrite),
                    "MauMemory::virt_write(0x%016" PRIx64 ") - "
                    "Address[%d:%d] not zero\n",
                    offset, kVirtZerosShift+kVirtZerosBits-1, kVirtZerosShift);
        if (!kAllowBadVirtWrite) { THROW_ERROR(-3); } // For DV
      } else {
        switch (mem_type) {
          case kVirtMemTypeStats:
            // (Ab)use data_size field to indicate a full-resolution stats write
            if (data_size == 0x3)
              stats_virt_write_full(ltab, addr, data0, data1, T);
            else
              stats_virt_write(ltab, addr, data0, data1, T);
            break;
          case kVirtMemTypeMeter:            meter_virt_write(ltab, addr, data0, data1, T); break;
          case kVirtMemTypeSelectorStateful: selector_stateful_virt_write(ltab, addr, data0, data1, T); break;
          case kVirtMemTypeIdletime:         idletime_virt_write(ltab, addr, data0, data1, T); break;
          default: bad_memtype_virt_write(offset, data0, data1, T); break;
        }
      }
    }

    MauSram *MauMemory::virt_find_sram(int sramtype, int logical_table,
                                       uint8_t addrtype, uint32_t addr,
                                       uint8_t vpn, uint16_t index,
                                       bool create, int sramtype2) const {
      MauSram *sram0 = NULL;
      // Add some fake OP to passed-in addr so vpn_range_check is happy
      uint32_t addr2 = Address::addr_make(addrtype, addr, Address::kAddrOpCfgRd);
      if ((sramtype2 < MauDefs::kSramTypeMin) || (sramtype2 > MauDefs::kSramTypeMax))
        sramtype2 = sramtype;
      if ((logical_table >= 0) && (logical_table < kLogicalTables)) {
        MauAddrDist *adist = mau_->mau_addr_dist();
        // Search through Srams/Maprams for VPN match
        for (int row = 0; row < kSramRows; row++) {
          for (int col = 0; col < kSramColumns; col++) {
            MauSram *sram = mau_->sram_lookup(row,col);
            if ((sram != NULL) && (sram->get_logical_table() == logical_table) &&
                ((sram->get_ram_type() == sramtype) || (sram->get_ram_type() == sramtype2))) {
              MauMapram *mapram = sram->mapram();
              if (mapram != NULL) {
                uint8_t mapram_vpn = mapram->get_synth2port_vpn(index);
                if (mapram_vpn == vpn) {
                  int arow = sram->get_alu_logrow_index();
                  bool ok = (arow >= 0) ?adist->vpn_range_check(arow, addr2, addrtype, false) :false;
                  if (ok) return sram;
                } else if ((sram0 == NULL) && (create) &&
                           (mapram_vpn == kMapramVpnUnoccupied)) {
                  int arow = sram->get_alu_logrow_index();
                  bool ok = (arow >= 0) ?adist->vpn_range_check(arow, addr2, addrtype, true) :false;
                  if (ok) sram0 = sram; // Store first ok sram that has slot empty
                }
              }
            }
          }
        }
      }
      if (sram0 != NULL) {
        MauMapram *mapram0 = sram0->mapram();
        if (mapram0 != NULL) {
          mapram0->set_synth2port_vpn(index, vpn);
          return sram0;
        }
      }
      return NULL;
    }
    MauSram *MauMemory::stats_virt_find_sram(int logical_table, uint32_t addr, bool create) const {
      return virt_find_sram(MauDefs::kSramTypeStats, logical_table, AddrType::kStats, addr,
                            Address::stats_addr_get_vpn(addr),
                            Address::stats_addr_get_index(addr), create);
    }
    void MauMemory::stats_virt_read(int logical_table, uint32_t addr,
                                    uint64_t* data0, uint64_t* data1, uint64_t T) const {

      bool ok_lt = ((logical_table >= 0) && (logical_table < kLogicalTables));
      if ((ok_lt) && (mau_->mau_result_bus()->stats_virt_allow(logical_table))) {

        addr = Address::addrShift(addr, kStatsVAddrPbusShift);
        if (kStatsVAddrPbusReadBubbleEmulate) {
          mau_->pbus_read(AddrType::kStats, logical_table, addr, data0, data1, false, true, T);
          mau_->mau_info_incr(MAU_STATS_VIRT_READS);
        } else {
          // TODO: LOCK_RESOURCES here
          MauSram *sram = stats_virt_find_sram(logical_table, addr);
          if (sram != NULL) {
            uint16_t stats_index = Address::stats_addr_get_index(addr);
            sram->read(stats_index, data0, data1, T);
            mau_->mau_info_incr(MAU_STATS_VIRT_READS);
          }
          else mau_->mau_info_incr(MAU_BAD_VIRT_READS);
        }
      } else {
        mau_->mau_info_incr(MAU_BAD_VIRT_READS);
        const char *errstr = (ok_lt) ?"stats virt access not enabled" :"invalid";
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead),
                    "MauMemory::stats_virt_read(%d,0x%08x) - logical table %s\n",
                    logical_table, addr, errstr);
      }
    }
    void MauMemory::stats_virt_write(int logical_table, uint32_t addr,
                                     uint64_t data0, uint64_t data1, uint64_t T) {

      bool ok_lt = ((logical_table >= 0) && (logical_table < kLogicalTables));
      if ((ok_lt) && (mau_->mau_result_bus()->stats_virt_allow(logical_table))) {

        addr = Address::addrShift(addr, kStatsVAddrPbusShift);

        uint16_t stats_index = Address::stats_addr_get_index(addr);
        int stats_subword = Address::stats_addr_get_subword(addr);
        // Deduce stats format
        MauAddrDist *addr_dist = mau_->mau_addr_dist();
        RMT_ASSERT(addr_dist != NULL);
        int stats_format = addr_dist->stats_format(logical_table);
        BitVector<kSramWidth> mask(UINT64_C(0));
        // Use StatsALU to generate a subword write mask
        bool ok_mask = MauStatsAlu::get_subword_mask(stats_format, stats_subword, &mask);

        if ((ok_mask) && (check_sram_write_data(data0, data1, mask))) {

          if (kStatsVAddrPbusWriteBubbleEmulate) {
            mau_->pbus_write(AddrType::kStats, logical_table, addr, data0, data1, true, T);
            mau_->mau_info_incr(MAU_STATS_VIRT_WRITES);
          } else {
            // TODO: LOCK_RESOURCES here
            MauSram *sram = stats_virt_find_sram(logical_table, addr, true);
            if (sram != NULL) {
              // Do masked write to update SRAM data atomically
              sram->write_masked(stats_index, data0, data1, mask);
              mau_->mau_info_incr(MAU_STATS_VIRT_WRITES);
            }
            else mau_->mau_info_incr(MAU_BAD_VIRT_WRITES);
          }

        } else {
          mau_->mau_info_incr(MAU_BAD_VIRT_WRITES);
          RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite),
                      "MauMemory::stats_virt_write(%d,0x%08x) - "
                      "bad subword %d OR data0/data1 not 0 outside mask %s\n",
                      logical_table, addr, stats_subword, mask.to_string().c_str());
        }
      } else {
        mau_->mau_info_incr(MAU_BAD_VIRT_WRITES);
        const char *errstr = (ok_lt) ?"stats virt access not enabled" :"invalid";
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite),
                    "MauMemory::stats_virt_write(%d,0x%08x) - logical table %s\n",
                    logical_table, addr, errstr);
      }
    }



    MauSram *MauMemory::meter_virt_find_sram(int logical_table, uint32_t addr,
                                             bool create) const {
      return virt_find_sram(MauDefs::kSramTypeMeter, logical_table, AddrType::kMeter, addr,
                            Address::meter_addr_get_vpn(addr),
                            Address::meter_addr_get_index(addr), create);
    }
    void MauMemory::meter_virt_read(int logical_table, uint32_t addr,
                                    uint64_t* data0, uint64_t* data1, uint64_t T) const {

      bool ok_lt = ((logical_table >= 0) && (logical_table < kLogicalTables));
      if ((ok_lt) && (mau_->mau_result_bus()->meter_virt_allow(logical_table))) {

        addr = Address::addrShift(addr, kMeterVAddrPbusShift);
        if (kMeterVAddrPbusReadBubbleEmulate) {
          mau_->pbus_read(AddrType::kMeter, logical_table, addr, data0, data1, false, true, T);
          mau_->mau_info_incr(MAU_METER_VIRT_READS);
        } else {
          MauSram *sram = meter_virt_find_sram(logical_table, addr);
          if (sram != NULL) {
            uint16_t meter_index = Address::meter_addr_get_index(addr);
            // Read back all 128-bits
            sram->read(meter_index, data0, data1, T);
            mau_->mau_info_incr(MAU_METER_VIRT_READS);
          }
          else mau_->mau_info_incr(MAU_BAD_VIRT_READS);
        }
      } else {
        mau_->mau_info_incr(MAU_BAD_VIRT_READS);
        const char *errstr = (ok_lt) ?"meter virt access not enabled" :"invalid";
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead),
                    "MauMemory::meter_virt_read(%d,0x%08x) - logical table %s\n",
                    logical_table, addr, errstr);
      }
    }

    void MauMemory::meter_virt_write(int logical_table, uint32_t addr,
                                     uint64_t data0, uint64_t data1, uint64_t T) {

      bool ok_lt = ((logical_table >= 0) && (logical_table < kLogicalTables));
      if ((ok_lt) && (mau_->mau_result_bus()->meter_virt_allow(logical_table))) {

        addr = Address::addrShift(addr, kMeterVAddrPbusShift);
        if (kMeterVAddrPbusWriteBubbleEmulate) {
          mau_->pbus_write(AddrType::kMeter, logical_table, addr, data0, data1, true, T);
          mau_->mau_info_incr(MAU_METER_VIRT_WRITES);
        } else {
          // No subword in meter addr so write all 128 bits
          MauSram *sram = meter_virt_find_sram(logical_table, addr, true);
          if (sram != NULL) {
            uint16_t meter_index = Address::meter_addr_get_index(addr);
            sram->write(meter_index, data0, data1, T);
            mau_->mau_info_incr(MAU_METER_VIRT_WRITES);
          }
          else mau_->mau_info_incr(MAU_BAD_VIRT_WRITES);
        }
      } else {
        mau_->mau_info_incr(MAU_BAD_VIRT_WRITES);
        const char *errstr = (ok_lt) ?"meter virt access not enabled" :"invalid";
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite),
                    "MauMemory::meter_virt_write(%d,0x%08x) - logical table %s\n",
                    logical_table, addr, errstr);
      }
    }


    MauSram *MauMemory::selector_stateful_virt_find_sram(int logical_table, uint32_t addr,
                                                         bool create) const {
      // TODO: VIRT: how to discriminate selector/stateful
      return virt_find_sram(MauDefs::kSramTypeSelector, logical_table, AddrType::kMeter, addr,
                            Address::meter_addr_get_vpn(addr),
                            Address::meter_addr_get_index(addr), create,
                            MauDefs::kSramTypeStateful);
    }
    void MauMemory::selector_stateful_virt_read(int logical_table, uint32_t addr,
                                                uint64_t* data0, uint64_t *data1, uint64_t T) const {

      bool ok_lt = ((logical_table >= 0) && (logical_table < kLogicalTables));
      if ((ok_lt) && (mau_->mau_result_bus()->meter_virt_allow(logical_table))) {

        addr = Address::addrShift(addr, kSelectorStatefulVAddrPbusShift);
        // Maybe move on stateful time - primarily for Bloom clear
        if (mau_->mau_stateful_counters()->is_counter_for_lt_being_cleared(logical_table))
          mau_->mau_stateful_counters()->advance_time(logical_table, T, "StatefulVirtualRead", addr);

        if (kSelectorStatefulVAddrPbusReadBubbleEmulate) {
          mau_->pbus_read(AddrType::kMeter, logical_table, addr, data0, data1, false, true, T);
          mau_->mau_info_incr(MAU_SELECTOR_STATEFUL_VIRT_READS);
        } else {
          MauSram *sram = selector_stateful_virt_find_sram(logical_table, addr);
          if (sram != NULL) {
            uint8_t vpn = Address::meter_addr_get_vpn(addr);
            uint16_t index = Address::meter_addr_get_index(addr);
            // Read back all 128-bits
            sram->read(index, data0, data1, T);
            mau_->mau_info_incr(MAU_SELECTOR_STATEFUL_VIRT_READS);
            RMT_LOG_OBJ(mau_,RmtDebug::verbose(RmtDebug::kRmtDebugRead),
                        "MauMemory:: selector_stateful_virt_read[%d,%d] "
                        "(ltab=%d,vpn=%d,index=%d) = "
                        "0x%016" PRIx64 " 0x%016" PRIx64 " \n",
                        sram->row_index(), sram->col_index(),
                        logical_table, vpn, index, *data0, *data1);
          }
          else mau_->mau_info_incr(MAU_BAD_VIRT_READS);
        }
      } else {
        mau_->mau_info_incr(MAU_BAD_VIRT_READS);
        const char *errstr = (ok_lt) ?"meter virt access not enabled" :"invalid";
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead),
                    "MauMemory::selector_stateful_virt_read(%d,0x%08x) - logical table %s\n",
                    logical_table, addr, errstr);
      }
    }
    void MauMemory::selector_stateful_virt_write(int logical_table, uint32_t addr,
                                                 uint64_t data0, uint64_t data1, uint64_t T) {

      bool ok_lt = ((logical_table >= 0) && (logical_table < kLogicalTables));
      if ((ok_lt) && (mau_->mau_result_bus()->meter_virt_allow(logical_table))) {

        addr = Address::addrShift(addr, kSelectorStatefulVAddrPbusShift);

        uint8_t vpn = Address::meter_addr_get_vpn(addr);
        uint16_t index = Address::meter_addr_get_index(addr);
        int shift = Address::selector_addr_get_shift(addr);
        int width = Address::selector_addr_get_width(addr, shift);
        BitVector<kSramWidth> mask(UINT64_C(0));
        if (width < kSramWidth) {
          int offset = Address::selector_addr_get_offset(addr, shift);
          uint64_t ONES = UINT64_C(0xFFFFFFFFFFFFFFFF);
          mask.set_word(ONES, offset, width);
        } else {
          mask.fill_all_ones();
        }

        if (check_sram_write_data(data0, data1, mask)) {
          // Maybe move on stateful time - primarily for Bloom clear
          if (mau_->mau_stateful_counters()->is_counter_for_lt_being_cleared(logical_table))
            mau_->mau_stateful_counters()->advance_time(logical_table, T, "StatefulVirtualWrite", addr);

          if (kSelectorStatefulVAddrPbusWriteBubbleEmulate) {
            mau_->pbus_write(AddrType::kMeter, logical_table, addr, data0, data1, true, T);
            mau_->mau_info_incr(MAU_SELECTOR_STATEFUL_VIRT_WRITES);
          } else {
            MauSram *sram = selector_stateful_virt_find_sram(logical_table, addr, true);
            if (sram != NULL) {
              RMT_LOG_OBJ(mau_,RmtDebug::verbose(RmtDebug::kRmtDebugWrite),
                          "MauMemory::selector_stateful_virt_write[%d,%d] "
                          "(ltab=%d,vpn=%d,index=%d) = "
                          "0x%016" PRIx64 " 0x%016" PRIx64 " \n",
                          sram->row_index(), sram->col_index(),
                          logical_table, vpn, index, data0, data1);
              if (width < kSramWidth) {
                sram->write_masked(index, data0, data1, mask);
              } else {
                sram->write(index, data0, data1, T);
              }
              mau_->mau_info_incr(MAU_SELECTOR_STATEFUL_VIRT_WRITES);
            }
            else mau_->mau_info_incr(MAU_BAD_VIRT_WRITES);
          }

        } else {
          mau_->mau_info_incr(MAU_BAD_VIRT_WRITES);
          RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite),
                      "MauMemory::selector_stateful_virt_write(%d,0x%08x) - "
                      "data0/data1 not 0 outside mask %s\n",
                      logical_table, addr, mask.to_string().c_str());
        }
      } else {
        mau_->mau_info_incr(MAU_BAD_VIRT_WRITES);
        const char *errstr = (ok_lt) ?"meter virt access not enabled" :"invalid";
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite),
                    "MauMemory::selector_stateful_virt_write(%d,0x%08x) - logical table %s\n",
                    logical_table, addr, errstr);
      }
    }



    void MauMemory::idletime_virt_read(int logical_table, uint32_t addr,
                                       uint64_t* data0, uint64_t *data1, uint64_t T) const {
      if ((logical_table >= 0) && (logical_table < kLogicalTables)) {
        addr = Address::addrShift(addr, kIdletimeVAddrPbusShift);
        if (kIdletimeVAddrPbusReadBubbleEmulate) {
          mau_->pbus_read(AddrType::kIdle, logical_table, addr, data0, data1, false, true, T);
          mau_->mau_info_incr(MAU_IDLETIME_VIRT_READS);
        } else {
          // Extract VPN/index from address
          //int shift = Address::idletime_addr_get_shift(addr);
          uint8_t idle_vpn = Address::idletime_addr_get_vpn(addr);
          uint16_t idle_index = Address::idletime_addr_get_index(addr);
          // Search through Maprams for Ltab,VPN match
          for (int row = 0; row < kMapramRows; row++) {
            for (int col = 0; col < kMapramColumns; col++) {
              MauMapram *mapram = mau_->mapram_lookup(row,col);
              if ((mapram != NULL) && (mapram->is_idletime_mapram()) &&
                  (logical_table == mapram->get_logical_table()) &&
                  (idle_vpn == mapram->get_vpn()) &&
                  (idle_index < kMapramEntries)) {
                // Read whole word
                mapram->read(idle_index, data0, data1, T);
                mau_->mau_info_incr(MAU_IDLETIME_VIRT_READS);
                break;
              }
            }
          }
        }
      } else {
        mau_->mau_info_incr(MAU_BAD_VIRT_READS);
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead),
                    "MauMemory::idletime_virt_read(%d,0x%08x) - Invalid ltab\n",
                    logical_table, addr);
      }
    }
    void MauMemory::idletime_virt_write(int logical_table, uint32_t addr,
                                        uint64_t data0, uint64_t data1, uint64_t T) {
      if ((logical_table >= 0) && (logical_table < kLogicalTables)) {
        bool upcall_sweeper = false;
        addr = Address::addrShift(addr, kIdletimeVAddrPbusShift);

        // Extract VPN/index from address
        int shift = Address::idletime_addr_get_shift(addr);
        uint8_t idle_vpn = Address::idletime_addr_get_vpn(addr);
        uint16_t idle_index = Address::idletime_addr_get_index(addr);
        int width = Address::idletime_addr_get_width(addr, shift);
        int offset = Address::idletime_addr_get_offset(addr, shift);
        bool width_match = true;

        // Search through Maprams for Ltab,VPN match
        // (this lets us work out what correct write width should be)
        MauMapram *mapram = NULL;
        for (int row = 0; row < kMapramRows; row++) {
          for (int col = 0; col < kMapramColumns; col++) {
            mapram = mau_->mapram_lookup(row,col);
            if ((mapram != NULL) && (mapram->is_idletime_mapram()) &&
                (logical_table == mapram->get_logical_table()) &&
                (idle_vpn == mapram->get_vpn()) &&
                (idle_index < kMapramEntries)) {
              // Found - does address entry width match mapram entry
              // width? (if true, occasionally updates width to write)
              uint32_t tmpAddr = Address::addr_make(AddrType::kIdle, addr,
                                                    Address::kAddrOpCfgWr);
              width_match = mapram->idletime_check_width(tmpAddr, &width);
              break;
            }
          }
        }

        // Setup a mask to check for extra bits set in data0/data1
        BitVector<kSramWidth> mask(UINT64_C(0));
        mask.set_word(UINT64_C(0xFFFFFFFFFFFFFFFF), offset, width);

        // Check and warn if ANY extra bits set outside mask
        if (!check_sram_write_data(data0, data1, mask)) {
          RMT_LOG_OBJ(mau_,RmtDebug::warn(RmtDebug::kRmtDebugWrite),
                      "MauMemory::idletime_virt_write(%d,0x%08x) - "
                      "data0/data1 (0x%016" PRIx64 "/0x%016" PRIx64 ") "
                      "not 0 outside mask0/1 (0x%016" PRIx64 "/0x%016" PRIx64 ") "
                      "[shift=%d vpn=%d index=%d width=%d offset=%d]\n",
                      logical_table, addr, data0, data1,
                      mask.get_word(0), mask.get_word(64),
                      shift, idle_vpn, idle_index, width, offset);
        }

        // Do nothing if we had a mismatch in the address/mapram entry widths
        //  (ERROR will already have been reported by idletime_check_width)
        // Check and ERROR if extra bits set outside mask within kMapramWidth
        if ((width_match) && (check_mapram_write_data(data0, mask.get_word(0)))) {

          if (kIdletimeVAddrPbusWriteBubbleEmulate) {
            mau_->pbus_write(AddrType::kIdle, logical_table, addr, data0, data1, true, T);
            upcall_sweeper = true;
          } else if (mapram != NULL) {
            // Write subword determined from address
            // NB. S/W will 'know' correct offset/width and so
            // data0 will have subword bits in correct position. So
            // we need to shift and mask data0!
            uint64_t subword = (data0 >> offset) & ((1<<width)-1);
            mapram->set_word(idle_index, offset, width, subword);
            upcall_sweeper = true;
          }
          if (upcall_sweeper) {
            mau_->mau_info_incr(MAU_IDLETIME_VIRT_WRITES);
            // Upcall sweeper in case the logical table was
            // completely idle and sweeping was deactivated.
            // This write could mean table needs sweeping again
            MauAddrDist *addr_dist = mau_->mau_addr_dist();
            RMT_ASSERT(addr_dist != NULL);
            addr_dist->idletime_hit(logical_table);
          } else {
            mau_->mau_info_incr(MAU_BAD_VIRT_WRITES);
          }

        } else {
          mau_->mau_info_incr(MAU_BAD_VIRT_WRITES);
          if (width_match) {
            RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite),
                        "MauMemory::idletime_virt_write(%d,0x%08x) - "
                        "data0 (0x%016" PRIx64 ") not 0 outside mask "
                        "(0x%016" PRIx64 ") (mask limited to mapram width %d) "
                        "[shift=%d vpn=%d index=%d width=%d offset=%d]\n",
                        logical_table, addr, data0, mask.get_word(0),
                        kMapramWidth, shift, idle_vpn, idle_index, width, offset);
          }
        }

      } else {
        mau_->mau_info_incr(MAU_BAD_VIRT_WRITES);
        RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite),
                    "MauMemory::idletime_virt_write(%d,0x%08x) - Invalid ltab\n",
                    logical_table, addr);
      }
    }


    void MauMemory::stats_virt_read_full(int logical_table, uint32_t addr,
                                         uint64_t* data0, uint64_t *data1, uint64_t T) const {
      if (!kKeepFullResStats) {
        // Can only be called from StatsALU/Moveregs in this mode - just return 0
        if (data0 != NULL) *data0 = UINT64_C(0);
        if (data1 != NULL) *data1 = UINT64_C(0);
      } else {
        if ((logical_table >= 0) && (logical_table < kLogicalTables)) {
          addr = Address::addrShift(addr, kStatsFullVAddrPbusShift);

          // Here we allow access to full-resolution stats counters
          try {
            std::array<uint64_t,2> entry = full_res_stats_.at(make_map_key(logical_table,addr));
            if (data0 != NULL) *data0 = entry.at(0);
            if (data1 != NULL) *data1 = entry.at(1);
          } catch (const std::exception&) {
            // Just means no value yet written for that logical_table/addr
          }
        } else {
          RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead),
                      "MauMemory::stats_virt_read_full(%d,0x%08x) - Invalid ltab\n",
                      logical_table, addr);
        }
      }
    }

    void MauMemory::stats_virt_write_full(int logical_table, uint32_t addr,
                                          uint64_t data0, uint64_t data1, uint64_t T) {
      if (!kKeepFullResStats) {
        // Do nothing - just ignore write
      } else {
        if ((logical_table >= 0) && (logical_table < kLogicalTables)) {
          addr = Address::addrShift(addr, kStatsFullVAddrPbusShift);

          std::array<uint64_t,2> entry = {data0, data1};
          try {
            full_res_stats_.at(make_map_key(logical_table,addr)) = entry;
          } catch (const std::exception&) {
            // Probably means no value yet written for that logical_table/addr - try emplace
            try {
              full_res_stats_.emplace(make_map_key(logical_table,addr),entry);
            } catch (const std::exception&) {
              RMT_LOG_OBJ(mau_, RmtDebug::error(),
                          "MauMemory::stats_virt_write_full(%d,0x%08x) - Exception!\n",
                          logical_table, addr);
            }
          }
        } else {
          RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite),
                      "MauMemory::stats_virt_write_full(%d,0x%08x) - Invalid ltab\n",
                      logical_table, addr);
        }
      }
    }


    void MauMemory::bad_memtype_virt_read(uint64_t offset,
                                          uint64_t* data0, uint64_t* data1, uint64_t T) const {
      RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugRead,kAllowBadMemTypeVirtRead),
                  "MauMemory::bad_memtype_virt_read(0x%016" PRIx64 ") - "
                  "Invalid mem type %d\n", offset, get_virt_mem_type(offset));
      if (data0 != NULL) *data0 = kBadDataWord;
      if (data1 != NULL) *data1 = kBadDataWord;
      mau_->mau_info_incr(MAU_BAD_VIRT_READS);
      if (!kAllowBadMemTypeVirtRead) { THROW_ERROR(-3); } // For DV

    }
    void MauMemory::bad_memtype_virt_write(uint64_t offset,
                                           uint64_t data0, uint64_t data1, uint64_t T) {
      RMT_LOG_OBJ(mau_,RmtDebug::error(RmtDebug::kRmtDebugWrite,kAllowBadMemTypeVirtWrite),
                  "MauMemory::bad_memtype_virt_write(0x%016" PRIx64 ") - "
                  "Invalid mem type %d\n", offset, get_virt_mem_type(offset));
      mau_->mau_info_incr(MAU_BAD_VIRT_WRITES);
      if (!kAllowBadMemTypeVirtWrite) { THROW_ERROR(-3); } // For DV
    }



}
