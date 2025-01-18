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

#ifndef _SHARED_MAU_INSTR_STORE_COMMON_
#define _SHARED_MAU_INSTR_STORE_COMMON_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <address.h>
#include <instr.h>

#include <register_includes/imem_parity_ctl.h>
#include <register_includes/phv_egress_thread_array2.h>
#include <register_includes/phv_ingress_thread_array2.h>
#include <register_includes/phv_egress_thread_imem_array2.h>
#include <register_includes/phv_ingress_thread_imem_array2.h>
//phv_ingress|egress_thread_alu_array2 removed in regs 55472_mau_dev_jbay
//so these now have to be in per-chip logic
//#include <register_includes/phv_egress_thread_alu_array2.h>
//#include <register_includes/phv_ingress_thread_alu_array2.h>
#include <register_includes/actionmux_din_power_ctl_array2.h>
#include <register_includes/match_input_xbar_din_power_ctl_array2.h>


namespace MODEL_CHIP_NAMESPACE {

  class Mau;
  class Phv;

  class MauInstrStoreCommon {
    static constexpr int kPhvWords = Phv::kWordsMax;
    static constexpr int kPhvGroups = Phv::kGroupsMax;
    static constexpr int kPhvHalfGroups = Phv::kGroupsMax * 2;

    static const int kWordOffsetsPerPhvHalfGroupLO[kPhvHalfGroups];
    static const int kWordOffsetsPerPhvHalfGroupHI[kPhvHalfGroups];

    // We have 2x (HI/LO) arrays of these word offsets which
    // means each offset indexes a WORD of bitwidth:
    static constexpr int kPhvHalfGroupWordSize = kPhvWords / (kPhvHalfGroups*2);
    static_assert( (kPhvHalfGroupWordSize <= 16), "HalfGroupWords must fit in a uint16_t");

    static int half_group_word_offset(uint32_t hilo, uint32_t which_half_group) {
      RMT_ASSERT((hilo == 0) || (hilo == 1));
      RMT_ASSERT(which_half_group < kPhvHalfGroups);
      if (hilo == 0)
        return kWordOffsetsPerPhvHalfGroupLO[which_half_group];
      else
        return kWordOffsetsPerPhvHalfGroupHI[which_half_group];
    }
    static int half_group_bit_offset(uint32_t hilo, uint32_t which_half_group) {
      return half_group_word_offset(hilo, which_half_group) * kPhvHalfGroupWordSize;
    }
    // from http://www-graphics.stanford.edu/~seander/bithacks.html#ParityParallel
    static uint8_t parity32( uint32_t v ) {
      v ^= v >> 16;
      v ^= v >> 8;
      v ^= v >> 4;
      v &= 0xf;
      return (0x6996 >> v) & 1;
    }

 public:
    static bool kRelaxThreadReplicationCheck; // Defined in rmt-config.cpp
    static bool kRelaxAddrFormatReplicationCheck;
    static bool kRelaxThreadOverlapCheck;
    static bool kRelaxInstrOverlapCheck;

    static constexpr int     kInstrs = MauDefs::kInstrs;
    static constexpr uint8_t kOpTypeNone    = 0;
    static constexpr uint8_t kOpTypeSimple  = 1;
    static constexpr uint8_t kOpTypeBitmask = 2;

    MauInstrStoreCommon(int chipIndex, int pipeIndex, int mauIndex, Mau *mau);
    MauInstrStoreCommon(const MauInstrStoreCommon& other) = delete;  // XXX
    virtual ~MauInstrStoreCommon();

 private:
    MauInstrStoreCommon& operator=(const MauInstrStoreCommon&){ return *this; } // XXX
    void phv_ingress_write_cb(uint32_t hilo, uint32_t which_half_group);
    void phv_egress_write_cb(uint32_t hilo, uint32_t which_half_group);
    void actionmux_en_write_cb(uint32_t hilo, uint32_t which_half_group);
    void matchxbar_en_write_cb(uint32_t hilo, uint32_t which_half_group);
    void check_ingress_egress();
    void track_colours(uint32_t instrWord, uint32_t phvWord8, uint32_t instr, int colour);
    uint8_t track_parity(uint32_t instrWord, uint32_t phvWord8,
                         uint32_t instr, uint8_t colour, uint8_t parity);

 public:
    inline Mau                  *mau()           { return mau_; }
    inline BitVector<kPhvWords> *ingress()       { return &ingress_; }
    inline BitVector<kPhvWords> *egress()        { return &egress_; }
    inline BitVector<kPhvWords> *actionmux_en()  { return &actionmux_en_; }
    inline BitVector<kPhvWords> *matchxbar_en()  { return &matchxbar_en_; }
    inline int phv_get_gress(int phvWord) {
      RMT_ASSERT((phvWord >= 0) && (phvWord < kPhvWords));
      if      (ingress_.bit_set(phvWord)) return  0;
      else if ( egress_.bit_set(phvWord)) return  1;
      else                                return -1;
    }

    // PER-CHIP implementation
    virtual uint16_t get_phv_ingress_thread_alu(int hilo, int grp, uint16_t dflt) = 0;
    virtual uint16_t get_phv_egress_thread_alu(int hilo, int grp, uint16_t dflt) = 0;

    virtual bool imem_read(uint32_t instrWord, uint32_t phvWord,
                           uint32_t *instr, uint8_t *color, uint8_t *parity) = 0;
    virtual bool imem_write(uint32_t instrWord, uint32_t phvWord,
                            uint32_t instr, uint8_t color, uint8_t parity) = 0;
    virtual bool imem_write_parity(uint32_t instrWord, uint32_t phvWord,
                                   uint8_t parity) = 0;
    virtual void instr_reset_chip(Phv *phv) { }
    virtual bool instr_add_op(int logical_table, bool ingress, uint8_t op) {
      if (!Address::action_instr_enabled(op)) return false;
      uint8_t op2 = (ingress) ?(op & ~(1<<7)) :(op | (1<<7));
      RMT_ASSERT(op == op2);   // Check iegress set during extract_action_instr_addr
      instr_add_simple_op(op); // Only simple ops by default
      return true;
    }
    virtual bool has_bitmask_ops(int logical_table) {
      return false; // Only simple ops by default
    }

    void instr_rw_callback(uint32_t instrWord, uint32_t phvWord, bool write);
    uint32_t instr_store_read(uint32_t instrWord, uint32_t phvWord);
    void instr_reset(Phv *phv);
    void instr_add_simple_op(uint8_t op);
    Instr *instr_get();


 private:
    Mau                                              *mau_;
    int                                               ingress_writes_;
    int                                               egress_writes_;
    Instr                                            *instr_;
    BitVector<kPhvWords>                              instr_ops_;
    BitVector<kPhvWords>                              zeros_;
    BitVector<kPhvWords>                              ingress_;
    BitVector<kPhvWords>                              egress_;
    BitVector<kPhvWords>                              actionmux_en_;
    BitVector<kPhvWords>                              matchxbar_en_;
    std::array< BitVector<kPhvWords>, kInstrs >       parity_;
    std::array< BitVector<kPhvWords>, kInstrs >       colour0_;
    std::array< BitVector<kPhvWords>, kInstrs >       colour1_;
    register_classes::PhvIngressThreadArray2          phv_ingress_thread_;
    register_classes::PhvEgressThreadArray2           phv_egress_thread_;
    register_classes::PhvIngressThreadImemArray2      phv_ingress_thread_imem_;
    register_classes::PhvEgressThreadImemArray2       phv_egress_thread_imem_;
    //register_classes::PhvIngressThreadAluArray2     phv_ingress_thread_alu_;
    //register_classes::PhvEgressThreadAluArray2      phv_egress_thread_alu_;
    register_classes::ActionmuxDinPowerCtlArray2      actionmux_din_power_ctl_;
    register_classes::MatchInputXbarDinPowerCtlArray2 match_input_xbar_din_power_ctl_;
    register_classes::ImemParityCtl                   imem_parity_ctl_;
  };
}
#endif // _SHARED_MAU_INSTR_STORE_COMMON_

