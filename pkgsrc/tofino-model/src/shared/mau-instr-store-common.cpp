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
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <register_adapters.h>
#include <phv.h>
#include <mau-instr-store-common.h>



namespace MODEL_CHIP_NAMESPACE {

MauInstrStoreCommon::MauInstrStoreCommon(int chipIndex, int pipeIndex, int mauIndex, Mau *mau)
    : mau_(mau), ingress_writes_(0), egress_writes_(0),
      instr_(new Instr(mau->get_object_manager(), pipeIndex, mauIndex, mau)),
      phv_ingress_thread_(default_adapter(phv_ingress_thread_,chipIndex,pipeIndex,mauIndex,
                          [this](uint32_t i,uint32_t j){this->phv_ingress_write_cb(i,j); })),
      phv_egress_thread_(default_adapter(phv_egress_thread_,chipIndex,pipeIndex,mauIndex,
                          [this](uint32_t i,uint32_t j){this->phv_egress_write_cb(i,j); })),
      phv_ingress_thread_imem_(default_adapter(phv_ingress_thread_imem_,chipIndex,pipeIndex,mauIndex)),
      phv_egress_thread_imem_(default_adapter(phv_egress_thread_imem_,chipIndex,pipeIndex,mauIndex)),
      //phv_ingress_thread_alu_(default_adapter(phv_ingress_thread_alu_,chipIndex,pipeIndex,mauIndex)),
      //phv_egress_thread_alu_(default_adapter(phv_egress_thread_alu_,chipIndex,pipeIndex,mauIndex)),
      actionmux_din_power_ctl_(default_adapter(actionmux_din_power_ctl_,chipIndex,pipeIndex,mauIndex,
                               [this](uint32_t i,uint32_t j){this->actionmux_en_write_cb(i,j); })),
      match_input_xbar_din_power_ctl_(default_adapter(match_input_xbar_din_power_ctl_,chipIndex,pipeIndex,mauIndex,
                                      [this](uint32_t i,uint32_t j){this->matchxbar_en_write_cb(i,j); })),
      imem_parity_ctl_(default_adapter(imem_parity_ctl_,chipIndex,pipeIndex,mauIndex)) {

  phv_ingress_thread_.reset();
  phv_egress_thread_.reset();
  phv_ingress_thread_imem_.reset();
  phv_egress_thread_imem_.reset();
  //phv_ingress_thread_alu_.reset(); // Now PER-CHIP
  //phv_egress_thread_alu_.reset();  // Now PER-CHIP
  actionmux_din_power_ctl_.reset();
  match_input_xbar_din_power_ctl_.reset();
  imem_parity_ctl_.reset();
  zeros_.fill_all_zeros();
}
MauInstrStoreCommon::~MauInstrStoreCommon() {
  delete instr_;
}

// Lookup tables to map half-group index to *WORD* offset in ingress/egress BitVector
// (the WORD size depends on Phv::kWordsMax (so on Tofino 8b, on Jbay 10b))
const int MauInstrStoreCommon::kWordOffsetsPerPhvHalfGroupLO[MauInstrStoreCommon::kPhvHalfGroups] = {
  0, 1, 2, 3,  8,  9, 10, 11, 16, 17, 18, 19, 20, 21
};
const int MauInstrStoreCommon::kWordOffsetsPerPhvHalfGroupHI[MauInstrStoreCommon::kPhvHalfGroups] = {
  4, 5, 6, 7, 12, 13, 14, 15, 22, 23, 24, 25, 26, 27
};

void MauInstrStoreCommon::phv_ingress_write_cb(uint32_t hilo, uint32_t which_half_group) {
  int bit_off = half_group_bit_offset(hilo, which_half_group);
  uint16_t val = phv_ingress_thread_.phv_ingress_thread(hilo, which_half_group);
  ingress_writes_++;
  ingress_.set_word(static_cast<uint64_t>(val), bit_off, kPhvHalfGroupWordSize);
}
void MauInstrStoreCommon::phv_egress_write_cb(uint32_t hilo, uint32_t which_half_group) {
  int bit_off = half_group_bit_offset(hilo, which_half_group);
  uint16_t val = phv_egress_thread_.phv_egress_thread(hilo, which_half_group);
  egress_writes_++;
  egress_.set_word(static_cast<uint64_t>(val), bit_off, kPhvHalfGroupWordSize);
}
void MauInstrStoreCommon::actionmux_en_write_cb(uint32_t hilo, uint32_t which_half_group) {
  int bit_off = half_group_bit_offset(hilo, which_half_group);
  uint16_t val = actionmux_din_power_ctl_.actionmux_din_power_ctl(hilo, which_half_group);
  actionmux_en_.set_word(static_cast<uint64_t>(val), bit_off, kPhvHalfGroupWordSize);
}
void MauInstrStoreCommon::matchxbar_en_write_cb(uint32_t hilo, uint32_t which_half_group) {
  int bit_off = half_group_bit_offset(hilo, which_half_group);
  uint16_t val = match_input_xbar_din_power_ctl_.match_input_xbar_din_power_ctl(
      hilo, which_half_group);
  matchxbar_en_.set_word(static_cast<uint64_t>(val), bit_off, kPhvHalfGroupWordSize);
}
void MauInstrStoreCommon::check_ingress_egress() {
  while (ingress_writes_ > 0) {
    int iwrites = ingress_writes_;
    for (int hilo = 0; hilo <= 1; hilo++) {
      for (int i = 0; i < kPhvHalfGroups; i++) {
        uint16_t valA = phv_ingress_thread_.phv_ingress_thread(hilo, i);
        uint16_t valB = phv_ingress_thread_imem_.phv_ingress_thread_imem(hilo, i);
        uint16_t valC = get_phv_ingress_thread_alu(hilo, i, valA); // PER-CHIP
        if ((valA != valB) || (valA != valC)) {
          bool relax = kRelaxThreadReplicationCheck;
          RMT_LOG_OBJ(mau_, RmtDebug::error(RmtDebug::kRmtDebugMauAlu,relax),
                      "Ingress thread NOT replicated correctly\n");
        }
      }
      ingress_writes_ -= iwrites;
    }
  }
  while (egress_writes_ > 0) {
    int ewrites = egress_writes_;
    for (int hilo = 0; hilo <= 1; hilo++) {
      for (int i = 0; i < kPhvHalfGroups; i++) {
        uint16_t valA = phv_egress_thread_.phv_egress_thread(hilo, i);
        uint16_t valB = phv_egress_thread_imem_.phv_egress_thread_imem(hilo, i);
        uint16_t valC = get_phv_egress_thread_alu(hilo, i, valA); // PER-CHIP
        if ((valA != valB) || (valA != valC)) {
          bool relax = kRelaxThreadReplicationCheck;
          RMT_LOG_OBJ(mau_, RmtDebug::error(RmtDebug::kRmtDebugMauAlu,relax),
                      "Egress thread NOT replicated correctly\n");
        }
      }
    }
    egress_writes_ -= ewrites;
  }
  if (ingress_.intersects_with(egress_)) {
    bool relax = kRelaxThreadOverlapCheck;
    RMT_LOG_OBJ(mau_, RmtDebug::error(RmtDebug::kRmtDebugMauAlu,relax),
                "Ingress/Egress thread overlap!\n");
    if (!relax) { THROW_ERROR(-2); } // For DV
  }
  if (ingress_.equals(zeros_) && egress_.equals(zeros_)) {
    RMT_LOG_OBJ(mau_, RmtDebug::verbose(), "Ingress/Egress thread both EMPTY!\n");
  }
}
void MauInstrStoreCommon::track_colours(uint32_t instrWord, uint32_t phvWord,
                                        uint32_t instr, int colour) {
  if (instr == 0) {
    colour0_[instrWord].clear_bit(phvWord);
    colour1_[instrWord].clear_bit(phvWord);
  } else if (colour == 0) {
    colour0_[instrWord].set_bit(phvWord);
    colour1_[instrWord].clear_bit(phvWord);
  } else {
    colour0_[instrWord].clear_bit(phvWord);
    colour1_[instrWord].set_bit(phvWord);
  }
}
uint8_t MauInstrStoreCommon::track_parity(uint32_t instrWord, uint32_t phvWord,
                                          uint32_t instr, uint8_t colour, uint8_t parity) {
  // Possibly calculate parity in H/W
  if ((imem_parity_ctl_.imem_parity_generate() & 0x1) != 0)
    parity = parity32( (colour << 28) | instr );
  // Store parity (might be passed-in or calculated)
  if (parity == 0x1)
    parity_[instrWord].set_bit(phvWord);
  else
    parity_[instrWord].clear_bit(phvWord);
  // Pass back out parity to write into register - may be masked off
  if ((imem_parity_ctl_.imem_parity_read_mask() & 0x1) != 0)
    parity = 0;
  return parity;
}

void MauInstrStoreCommon::instr_rw_callback(uint32_t instrWord, uint32_t phvWord,
                                            bool write) {
  RMT_ASSERT((instrWord < kInstrs) && (Phv::is_valid_phv(phvWord)));
  uint32_t instr;
  uint8_t color, r_parity;
  imem_read(instrWord, phvWord, &instr, &color, &r_parity); // PER-CHIP
  track_colours(instrWord, phvWord, instr, color);
  if (!write) r_parity = parity_[instrWord].get_bit(phvWord);
  uint8_t w_parity = track_parity(instrWord, phvWord, instr, color, r_parity);
  if (w_parity != r_parity)
    imem_write_parity(instrWord, phvWord, w_parity); // PER_CHIP
}


uint32_t MauInstrStoreCommon::instr_store_read(uint32_t instrWord, uint32_t phvWord) {
  RMT_ASSERT((instrWord < kInstrs) && (Phv::is_valid_phv(phvWord)));
  uint32_t instr = 0u;
  bool read_ok = imem_read(instrWord, phvWord, &instr, NULL, NULL); // PER-CHIP
  RMT_ASSERT(read_ok);
  return instr;
}
void MauInstrStoreCommon::instr_reset(Phv *phv) {
  if ((ingress_writes_ > 0) || (egress_writes_ > 0)) check_ingress_egress();
  instr_ops_.fill_all_zeros();
  instr_->reset();
  instr_reset_chip(phv);
}
void MauInstrStoreCommon::instr_add_simple_op(uint8_t op) {
  uint8_t colour = (op >> 0) & 0x1;
  uint8_t instrWord = (op >> 1) & 0x1F;
  uint8_t pfe = (op >> 6) & 0x1;
  uint8_t iegress = (op >> 7) & 0x1;
  RMT_ASSERT((pfe == 0x1) || (Address::kGlobalAddrEnable));
  RMT_ASSERT(instrWord < kInstrs);
  BitVector<kPhvWords> bv(UINT64_C(0));
  // Initialize based on colour
  if (colour == 0) bv.copy_from(colour0_[instrWord]);
  if (colour == 1) bv.copy_from(colour1_[instrWord]);
  // Mask off ingress/egress bits as selected
  if (iegress == 0) bv.mask(ingress_);
  if (iegress == 1) bv.mask(egress_);
  // See if this op intersects with already added ops
  bool check_for_clashes = instr_ops_.intersects_with(bv);
  // Or resulting bits into accumulator
  instr_ops_.or_with(bv);
  // And copy corresponding words into instr itself
  if (check_for_clashes) {
    for (int i = 0; i < kPhvWords; i++) {
      if (bv.bit_set(i)) {
        uint32_t oldop = instr_->get(i);
        uint32_t newop = instr_store_read(instrWord, i);
        instr_->set(i, newop);
        if ((oldop != 0u) && (newop != 0u) && (oldop != newop)) {
          bool relax = kRelaxInstrOverlapCheck;
          RMT_LOG_OBJ(mau_, RmtDebug::error(RmtDebug::kRmtDebugMauAlu,relax),
                      "Overlapping instructions added for ALU/PHV %d "
                      "(oldop=0x%x newop=0x%x)\n", i, oldop, newop);
          if (!relax) { THROW_ERROR(-2); } // For DV
        }
      }
    }
  } else {
    for (int i = 0; i < kPhvWords; i++) {
      if (bv.bit_set(i)) {
        instr_->set(i, instr_store_read(instrWord, i));
      }
    }
  }
}
Instr *MauInstrStoreCommon::instr_get() {
  if ((ingress_writes_ > 0) || (egress_writes_ > 0)) check_ingress_egress();
  return instr_;
}


}
