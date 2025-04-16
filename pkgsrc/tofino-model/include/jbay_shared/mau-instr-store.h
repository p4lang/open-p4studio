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

#ifndef _JBAY_SHARED_MAU_INSTR_STORE_
#define _JBAY_SHARED_MAU_INSTR_STORE_

#include <mau-instr-store-common.h>
#include <instr.h>

#include <register_includes/imem_subword8_array4_mutable.h>
#include <register_includes/imem_subword16_array4_mutable.h>
#include <register_includes/imem_subword32_array4_mutable.h>
#include <register_includes/imem_mocha_subword8_array4_mutable.h>
#include <register_includes/imem_mocha_subword16_array4_mutable.h>
#include <register_includes/imem_mocha_subword32_array4_mutable.h>
#include <register_includes/imem_dark_subword8_array4_mutable.h>
#include <register_includes/imem_dark_subword16_array4_mutable.h>
#include <register_includes/imem_dark_subword32_array4_mutable.h>
#include <register_includes/imem_table_addr_format_array.h>
#include <register_includes/mau_action_instruction_adr_mode_array.h>
#include <register_includes/imem_table_selector_fallback_addr_array.h>
#include <register_includes/imem_table_selector_fallback_icxbar_ctl_array.h>
#include <register_includes/imem_word_read_override.h>


namespace MODEL_CHIP_NAMESPACE {


  class MauInstrStoreAluConfig {
 public:
    static constexpr uint8_t  kAluTypeNormal = 0;
    static constexpr uint8_t  kAluTypeMocha = 1;
    static constexpr uint8_t  kAluTypeDark = 2;
 private:
    static constexpr uint8_t  kAluTypes = 3;
    static constexpr int      kAlusPerGrp = MauDefs::kInstrAluGrpSize;
    static constexpr uint32_t kAllAlus = 0xFFFFFFFFu >> (32-kAlusPerGrp);
    static constexpr uint32_t kMochaAlus = MauDefs::kInstrOperand1OnlyAlus;
    static constexpr uint32_t kDarkAlus = MauDefs::kInstrOperand2OnlyAlus;
    static constexpr uint32_t kMochaOrDarkAlus = kMochaAlus | kDarkAlus;
    static constexpr uint32_t kNormalAlus = kAllAlus & ~kMochaOrDarkAlus;

    static_assert( (kAluTypes < 255), "AluType must fit in uint8_t");
    static_assert( (kAlusPerGrp < 255), "AlusPerGrp must fit in uint8_t");
    static_assert( (kNormalAlus != 0u), "No normal ALUs available");
    static_assert( ((kNormalAlus & kMochaAlus) == 0u),
                   "Normal ALUs and MochaALUs must not overlap");
    static_assert( ((kNormalAlus & kDarkAlus) == 0u),
                   "Normal ALUs and DarkALUs must not overlap");
    static_assert( ((kMochaAlus & kDarkAlus) == 0u),
                   "Mocha ALUs and DarkALUs must not overlap");
    static_assert( ((kNormalAlus | kMochaAlus | kDarkAlus) == kAllAlus),
                   "Undefined ALU type");

    static uint16_t make_type_off(uint8_t type, uint8_t off) {
      return (static_cast<uint16_t>(type) << 8) | (static_cast<uint16_t>(off) << 0);
    }
    static uint8_t get_type(uint16_t type_off) {
      return static_cast<uint8_t>((type_off >> 8) & 0xFF);
    }
    static uint8_t get_off(uint16_t type_off) {
      return static_cast<uint8_t>((type_off >> 0) & 0xFF);
    }
    static uint8_t find_alu_type(int alu_index) {
      if      ((kNormalAlus & (1<<alu_index)) != 0u) return kAluTypeNormal;
      else if ((kMochaAlus & (1<<alu_index)) != 0u)  return kAluTypeMocha;
      else if ((kDarkAlus & (1<<alu_index)) != 0u)   return kAluTypeDark;
      else RMT_ASSERT(0);
    }

 public:
    MauInstrStoreAluConfig();
    ~MauInstrStoreAluConfig();
    uint8_t map_type_off_to_index(uint8_t type, uint8_t off);
    void    map_index_to_type_off(uint8_t index, uint8_t *type, uint8_t *off);

 private:
    std::array< std::array<uint8_t, kAlusPerGrp>, kAluTypes >  alu_type_off_to_index_;
    std::array<uint16_t, kAlusPerGrp>                          alu_index_to_type_off_;
  };




  class MauInstrStore : public MauInstrStoreCommon {

 public:
    static MauInstrStoreAluConfig *static_alu_config_;
    static MauInstrStoreAluConfig *static_alu_config_init();

    static constexpr uint8_t  kAluTypeNormal = MauInstrStoreAluConfig::kAluTypeNormal;
    static constexpr uint8_t  kAluTypeMocha = MauInstrStoreAluConfig::kAluTypeMocha;
    static constexpr uint8_t  kAluTypeDark = MauInstrStoreAluConfig::kAluTypeDark;
    static constexpr int      kAlusPerGrp = MauDefs::kInstrAluGrpSize;
    static constexpr int      kPhvsPerGrp = RmtDefs::kPhvWordsPerGroup;
    static_assert( (kPhvsPerGrp >= kAlusPerGrp),
                   "Expecting PHVsPerGroup to meet or exceed ALUsPerGroup");
    static_assert( ((kPhvsPerGrp % kAlusPerGrp) == 0),
                   "Expecting PHVsPerGroup to be integer multiple of ALUsPerGroup");
    static constexpr int      kPhvAluMult = kPhvsPerGrp / kAlusPerGrp;
    static constexpr int      kNumGroups8b = 4;
    static constexpr int      kNumGroups16b = 6;
    static constexpr int      kNumGroups32b = 4;
    static_assert( (kNumGroups8b + kNumGroups16b + kNumGroups32b == 14),
                   "Expecting total number PHV Groups to be 14 (20 PHVs per group)" );
    static constexpr int      kLogicalTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int      kNumSelectorAlus = MauDefs::kNumMeterAlus;


    MauInstrStore(int chipIndex, int pipeIndex, int mauIndex, Mau *mau);
    virtual ~MauInstrStore();

 private:
    void imem_subword8_rw_cb(uint8_t aluType, uint32_t aluHalf, uint32_t aluGrpInHalf,
                             uint32_t aluWord, uint32_t instrWord, bool write);
    void imem_subword16_rw_cb(uint8_t aluType, uint32_t aluHalf, uint32_t aluGrpInHalf,
                              uint32_t aluWord, uint32_t instrWord, bool write);
    void imem_subword32_rw_cb(uint8_t aluType, uint32_t aluHalf, uint32_t aluGrpInHalf,
                              uint32_t aluWord, uint32_t instrWord, bool write);

 public:
    inline uint16_t get_phv_ingress_thread_alu(int hilo, int grp, uint16_t dflt) {
      return dflt;
    }
    inline uint16_t get_phv_egress_thread_alu(int hilo, int grp, uint16_t dflt) {
      return dflt;
    }
    bool imem_normal_read(uint32_t instrWord, uint32_t phvWord,
                          uint32_t *instr, uint8_t *color, uint8_t *parity);
    bool imem_mocha_read(uint32_t instrWord, uint32_t phvWord,
                         uint32_t *instr, uint8_t *color, uint8_t *parity);
    bool imem_dark_read(uint32_t instrWord, uint32_t phvWord,
                        uint32_t *instr, uint8_t *color, uint8_t *parity);
    bool imem_read(uint32_t instrWord, uint32_t phvWord,
                   uint32_t *instr, uint8_t *color, uint8_t *parity);

    bool imem_normal_write(uint32_t instrWord, uint32_t phvWord,
                           uint32_t instr, uint8_t color, uint8_t parity,
                           bool write_parity_only=false);
    bool imem_mocha_write(uint32_t instrWord, uint32_t phvWord,
                          uint32_t instr, uint8_t color, uint8_t parity,
                          bool write_parity_only=false);
    bool imem_dark_write(uint32_t instrWord, uint32_t phvWord,
                         uint32_t instr, uint8_t color, uint8_t parity,
                         bool write_parity_only=false);
    bool imem_write(uint32_t instrWord, uint32_t phvWord,
                    uint32_t instr, uint8_t color, uint8_t parity,
                    bool write_parity_only);
    bool imem_write(uint32_t instrWord, uint32_t phvWord,
                    uint32_t instr, uint8_t color, uint8_t parity);
    bool imem_write_parity(uint32_t instrWord, uint32_t phvWord,
                           uint8_t parity);

    void instr_reset_chip(Phv *phv);
    void instr_add_bitmask_op(uint8_t iegress, uint8_t pfe,
                              uint8_t op_base, uint8_t op_bitmask, uint8_t color);
    bool instr_fallback(int logical_table);
    bool instr_add_op(int logical_table, bool ingress, uint8_t op);
    bool has_bitmask_ops(int logical_table);

    // These next two only used by Snapshot logic
    inline uint16_t get_fallback_using_lts() {
      return fallback_using_lts_;
    }
    inline uint8_t get_op_for_lt(int logical_table) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      return ops_per_lt_[logical_table];
    }


    inline uint8_t bitmap_base(int logical_table) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      // 0=>0 1=>8 2=>16 3=>24
      return (imem_table_addr_format_.bitmap_range(logical_table) * 8);
    }
    inline uint8_t bitmap_color(int logical_table) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      return imem_table_addr_format_.bitmap_color(logical_table);
    }

    inline bool has_fallback_bitmask_ops(int logical_table) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      return ((imem_table_selector_fallback_addr_.fallback_addr_format(logical_table) & 1) == 1);
    }
    inline uint8_t fallback_bitmap_base(int logical_table) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      // 0=>0 1=>8 2=>16 3=>24
      return (imem_table_selector_fallback_addr_.fallback_bitmap_range(logical_table) * 8);
    }
    inline uint8_t fallback_bitmap_color(int logical_table) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      return imem_table_selector_fallback_addr_.fallback_bitmap_color(logical_table);
    }
    inline uint8_t fallback_addr(int logical_table) {
      RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
      return imem_table_selector_fallback_addr_.fallback_addr(logical_table);
    }
    inline uint16_t fallback_ctl(int alu) {
      RMT_ASSERT((alu >= 0) && (alu < kNumSelectorAlus));
      return imem_table_selector_fallback_icxbar_.imem_table_selector_fallback_icxbar_ctl(alu);
    }


 private:
    MauInstrStoreAluConfig                                   *alu_config_;
    register_classes::ImemSubword8Array4Mutable               imem_subword8_;
    register_classes::ImemSubword16Array4Mutable              imem_subword16_;
    register_classes::ImemSubword32Array4Mutable              imem_subword32_;
    register_classes::ImemMochaSubword8Array4Mutable          imem_mocha_subword8_;
    register_classes::ImemMochaSubword16Array4Mutable         imem_mocha_subword16_;
    register_classes::ImemMochaSubword32Array4Mutable         imem_mocha_subword32_;
    register_classes::ImemDarkSubword8Array4Mutable           imem_dark_subword8_;
    register_classes::ImemDarkSubword16Array4Mutable          imem_dark_subword16_;
    register_classes::ImemDarkSubword32Array4Mutable          imem_dark_subword32_;
    register_classes::ImemTableAddrFormatArray                imem_table_addr_format_;
    register_classes::MauActionInstructionAdrModeArray        mau_action_instruction_adr_mode_;
    register_classes::ImemTableSelectorFallbackAddrArray      imem_table_selector_fallback_addr_;
    register_classes::ImemTableSelectorFallbackIcxbarCtlArray imem_table_selector_fallback_icxbar_;
    register_classes::ImemWordReadOverride                    imem_word_read_override_;
    uint16_t                                                  fallback_using_lts_;
    std::array< uint8_t, kLogicalTables >                     ops_per_lt_;
  };
}
#endif // _JBAY_SHARED_MAU_INSTR_STORE_
