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

#ifndef _SHARED_MAU_STATS_ALU_
#define _SHARED_MAU_STATS_ALU_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <phv.h>
#include <address.h>
#include <bitvector.h>
#include <mau-memory.h>

// Reg defs auto-generated from Semifore
#include <register_includes/statistics_ctl.h>
#include <register_includes/lrt_threshold_array.h>
#include <register_includes/lrt_update_interval_array.h>
#include <register_includes/mau_cfg_stats_alu_lt.h>


// Per-row STATS ALU

namespace MODEL_CHIP_NAMESPACE {

  class Mau;
  class MauExecuteState;
  class MauLogicalRow;

  // Struct to define config of a particular stats entry format
  struct MauStatsAluConfig {
    int8_t  format_index;
    int8_t  pkt_ctrs;
    int8_t  byte_ctrs;
    int8_t  n_entries;
    int8_t  pkt_field_width;
    int8_t  byte_field_width;
    uint8_t flags;
    int8_t  pkt_offsets[MauDefs::kMaxStatsEntriesPerWord];
    int8_t  byte_offsets[MauDefs::kMaxStatsEntriesPerWord];
  };
    
  
  class MauStatsAlu : public MauObject {

    static constexpr int      kType = RmtTypes::kRmtTypeMauStatsAlu;
    static constexpr int      kLogicalTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int      kLogicalRows = MauDefs::kLogicalRowsPerMau;
    static constexpr int      kLogicalColumns = MauDefs::kLogicalColumnsPerMau;
    static constexpr int      kDataBusWidth = MauDefs::kDataBusWidth;
    static constexpr int      kMaxEntriesPerWord = MauDefs::kMaxStatsEntriesPerWord;
    static constexpr int      kMaxStatsFormats = 32;
    static constexpr int      kStatsBytecountAdjustWidth = 14;
    static constexpr int      kStatsPktByteWidthDelta = 8;
    static constexpr int      kStatsLrtThresholdShift = 4;
    static constexpr uint64_t kStatsByteMult = UINT64_C(1);
    static constexpr uint64_t kStatsPktMult = kStatsByteMult << kStatsPktByteWidthDelta;
    static constexpr uint32_t kStatsAluLogicalRows = MauDefs::kStatsAluLogicalRows;
    // Defs for flags controlling stats format reserved values
    static constexpr uint8_t  kFlgRsvd0 = 0x1;
    static constexpr uint8_t  kFlgRsvdF = 0x2;
    static constexpr uint8_t  kFlgLRT   = 0x4;
    static constexpr uint8_t  kFlgDflt  = kFlgRsvd0|kFlgRsvdF|kFlgLRT;
    static inline bool zero_rsvd(int f) { return ((f & kFlgRsvd0) != 0); }
    static inline bool ones_rsvd(int f) { return ((f & kFlgRsvdF) != 0); }
    static inline bool lrt_avail(int f) { return ((f & kFlgLRT) != 0); }
   

 public:
    static bool kKeepFullResStats; // Defined in rmt-config.cpp
    static bool kCapStats;         // Defined in rmt-config.cpp

    // Defs for Stats notify messages
    static constexpr uint64_t kStatsFsmMsgType = UINT64_C(0);
    static constexpr int      kStatsFsmMsg_AddressOffset = 16;
    static constexpr int      kStatsFsmMsg_PipeStageTableOffset = 35;
    static constexpr int      kStatsFsmMsg_DataOffset = 64;
    static constexpr uint64_t kStatsLockAckMsgType = UINT64_C(1);
    static constexpr int      kStatsLockAckMsg_LockIdOffset = 16; 
    static constexpr int      kStatsLockAckMsg_PipeStageTableOffset = 35;
    static constexpr uint64_t kStatsDumpMsgType = UINT64_C(2);
    static constexpr int      kStatsDumpMsg_AddressOffset = 16;
    static constexpr int      kStatsDumpMsg_PipeStageTableOffset = 35;
    static constexpr int      kStatsDumpMsg_DataOffset = 64;

    static inline int get_stats_alu_regs_index(int logrow) {
      int idx =  __builtin_popcountl(kStatsAluLogicalRows & ((1u<<(logrow+1))-1)) - 1;
      RMT_ASSERT(idx >= 0);
      return idx;
    }
    static inline int get_stats_alu_logrow_index(int regs_index) {
      int n_logrows_seen_with_stats_alu = 0; 
      for (int logrow = 0; logrow < kLogicalRows; logrow++) {
        if ((kStatsAluLogicalRows & (1u<<logrow)) != 0u) {
          if (n_logrows_seen_with_stats_alu == regs_index) return logrow;
          n_logrows_seen_with_stats_alu++;
        }
      }
      return -1;
    }
    static inline uint32_t map_stats_alus_to_rows(uint32_t alus, int *regs_index=NULL) {
      uint32_t rows = 0u;
      int n_logrows_seen_with_stats_alu = 0; 
      for (int logrow = 0; logrow < kLogicalRows; logrow++) {
        if ((kStatsAluLogicalRows & (1u<<logrow)) != 0u) {
          // Found an row with a StatsALU - check if alu present in passed-in param
          if ((alus & (1<<n_logrows_seen_with_stats_alu)) != 0u) {
            if (regs_index != NULL) *regs_index = n_logrows_seen_with_stats_alu;
            rows |= (1<<logrow);
          }
          n_logrows_seen_with_stats_alu++;
        }
      }
      return rows;
    }
    
    // Map pkts/bytes/nentries to format in [0,31]
    static inline int get_stats_format(bool pkts, bool bytes, int nentries) {
      return ( ((pkts) ?1<<4 :0) | ((bytes) ?1<<3 :0) | (nentries & 0x7) );
    }
    // Check if stats format is valid
    static inline bool is_valid_stats_mode(int which_format) {
      RMT_ASSERT((which_format >= 0) && (which_format < kMaxStatsFormats));
      return (((MauDefs::valid_stats_modes >> which_format) & 1u) == 1u);
    }
    // Get pkt/byte off/nbits for a subword in some particular stats format
    static bool get_stats_format_info(int which_format, int subword,
                                      int *pkt_nbits, int *pkt_off,
                                      int *byte_nbits, int *byte_off, int *flags);
    // See if all-ones is a RSVD value for format
    static bool is_ones_rsvd(int which_format);
    // See if subword is valid for format
    static bool is_subword_valid(int which_format, int subword);
    
    // Get mask corresponding to subword given some particular stats format
    static bool get_subword_mask(int which_format, int subword,
                                 BitVector<kDataBusWidth> *mask);
    // Extract subword bits from input_data and return in dump1 dump2 using format
    // Return value is how many dump words we need to send - 0, 1 or 2
    static int get_dump_data_word(int which_format, int subword,
                                  const BitVector<kDataBusWidth> &input_data,
                                  uint64_t *dump1, uint64_t *dump2, bool no_dump_if_rsvd);
    // As above but input_data in 2 64b params    
    static int get_dump_data_word(int which_format, int subword,
                                  uint64_t input_data0, uint64_t input_data1,
                                  uint64_t *dump1, uint64_t *dump2, bool no_dump_if_rsvd);
    // Format pkt_cnt/byte_cnt into LRT evict data word
    static uint64_t get_lrt_evict_data_word(int pkt_nbits, int pkt_off,
                                            int byte_nbits, int byte_off,
                                            uint64_t pkt_cnt, uint64_t byte_cnt);


    // Defined in mau-stats-alu.cpp
    static const MauStatsAluConfig kStatsAlu_Config[kMaxStatsFormats];
    

 public:
    MauStatsAlu(RmtObjectManager *om, int pipeIndex, int mauIndex, int logicalRowIndex,
                Mau *mau, MauLogicalRow *mau_log_row,
                int physicalRowIndex, int physicalRowWhich);
    ~MauStatsAlu();
    

    void get_input(BitVector<kDataBusWidth> *data, uint32_t *addr);
    void get_output(BitVector<kDataBusWidth> *data, uint32_t *addr);
    void reset_resources();
    bool run_alu_with_state(MauExecuteState *state);

 private:
    void stats_ctl_change_callback();
    int  get_stats_format_this();
    bool is_lrt_enabled();
    uint64_t get_lrt_threshold();
    bool get_subentry_info(int subentry,
                           int *pkt_nbits, int *pkt_off,
                           int *byte_nbits, int *byte_off);
    int  get_packet_len(MauExecuteState *state);
    void set_output_data(BitVector<kDataBusWidth> *data);
    void set_output_addr(uint32_t *addr);
    void set_output(BitVector<kDataBusWidth> *data, uint32_t *addr);
    void keep_full_res_stats(uint32_t addr, int pkt_inc, int byte_inc);
    uint64_t get_wr_cnt(MauExecuteState *state, bool pkt,
                        int format, int nbits, int off);
    
 private:
    MauLogicalRow                        *mau_log_row_;
    MauMemory                            *mau_memory_;
    int                                   logical_row_;
    int                                   alu_index_;
    register_classes::StatisticsCtl           statistics_ctl_;
    register_classes::LrtThresholdArray       lrt_threshold_;
    register_classes::LrtUpdateIntervalArray  lrt_update_interval_;
    register_classes::MauCfgStatsAluLt        stats_alu_lt_;    
    bool has_run_=false;
  };
}
#endif // _SHARED_MAU_STATS_ALU_

