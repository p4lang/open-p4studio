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

// MauInstrStore - Tofino/TofinoB0 IMEM handling code

#include <register_adapters.h>
#include <mau-instr-store.h>

namespace MODEL_CHIP_NAMESPACE {

MauInstrStore::MauInstrStore(int chipIndex, int pipeIndex, int mauIndex, Mau *mau)
    : MauInstrStoreCommon(chipIndex, pipeIndex, mauIndex, mau),
      phv_ingress_thread_alu_(default_adapter(phv_ingress_thread_alu_,chipIndex,pipeIndex,mauIndex)),
      phv_egress_thread_alu_(default_adapter(phv_egress_thread_alu_,chipIndex,pipeIndex,mauIndex)),
      imem_subword8_(default_adapter(imem_subword8_,chipIndex,pipeIndex,mauIndex,
                     [this](uint32_t i,uint32_t j){this->imem_subword8_rw_cb(i,j,true); },
                     [this](uint32_t i,uint32_t j){this->imem_subword8_rw_cb(i,j,false); })),
      imem_subword16_(default_adapter(imem_subword16_,chipIndex,pipeIndex,mauIndex,
                     [this](uint32_t i,uint32_t j){this->imem_subword16_rw_cb(i,j,true); },
                     [this](uint32_t i,uint32_t j){this->imem_subword16_rw_cb(i,j,false); })),
      imem_subword32_(default_adapter(imem_subword32_,chipIndex,pipeIndex,mauIndex,
                     [this](uint32_t i,uint32_t j){this->imem_subword32_rw_cb(i,j,true); },
                     [this](uint32_t i,uint32_t j){this->imem_subword32_rw_cb(i,j,false); }))
{
  phv_ingress_thread_alu_.reset();
  phv_egress_thread_alu_.reset();
  imem_subword8_.reset();
  imem_subword16_.reset();
  imem_subword32_.reset();
}
MauInstrStore::~MauInstrStore() {
}

void MauInstrStore::imem_subword8_rw_cb(uint32_t phvWord8, uint32_t instrWord, bool write) {
  RMT_ASSERT(instrWord < kInstrs);
  int phvWord = Phv::map_word_rel8_to_abs(phvWord8);
  RMT_ASSERT(Phv::is_valid_phv(phvWord));
  instr_rw_callback(instrWord, phvWord, write);
}
void MauInstrStore::imem_subword16_rw_cb(uint32_t phvWord16, uint32_t instrWord, bool write) {
  RMT_ASSERT(instrWord < kInstrs);
  int phvWord = Phv::map_word_rel16_to_abs(phvWord16);
  RMT_ASSERT(Phv::is_valid_phv(phvWord));
  instr_rw_callback(instrWord, phvWord, write);
}
void MauInstrStore::imem_subword32_rw_cb(uint32_t phvWord32, uint32_t instrWord, bool write) {
  RMT_ASSERT(instrWord < kInstrs);
  int phvWord = Phv::map_word_rel32_to_abs(phvWord32);
  RMT_ASSERT(Phv::is_valid_phv(phvWord));
  instr_rw_callback(instrWord, phvWord, write);
}


bool MauInstrStore::imem_read(uint32_t instrWord, uint32_t phvWord,
                              uint32_t *instr, uint8_t *color, uint8_t *parity) {
  if ((instrWord > kInstrs) || (!Phv::is_valid_phv(phvWord))) return false;
  uint32_t ins;
  uint8_t col, par;
  int off = -1;
  int width = Phv::which_width(phvWord);
  switch (width) {
    case 8:
      off = Phv::map_word_abs_to_rel8(phvWord);
      ins = imem_subword8_.imem_subword8_instr(off, instrWord);
      col = imem_subword8_.imem_subword8_color(off, instrWord);
      par = imem_subword8_.imem_subword8_parity(off, instrWord);
      break;
    case 16:
      off = Phv::map_word_abs_to_rel16(phvWord);
      ins = imem_subword16_.imem_subword16_instr(off, instrWord);
      col = imem_subword16_.imem_subword16_color(off, instrWord);
      par = imem_subword16_.imem_subword16_parity(off, instrWord);
      break;
    case 32:
      off = Phv::map_word_abs_to_rel32(phvWord);
      ins = imem_subword32_.imem_subword32_instr(off, instrWord);
      col = imem_subword32_.imem_subword32_color(off, instrWord);
      par = imem_subword32_.imem_subword32_parity(off, instrWord);
      break;
  }
  if (off >= 0) {
    if (instr != NULL) *instr = ins;
    if (color != NULL) *color = col;
    if (parity != NULL) *parity = par;
  }
  return (off >= 0);
}

bool MauInstrStore::imem_write(uint32_t instrWord, uint32_t phvWord,
                               uint32_t instr, uint8_t color, uint8_t parity) {
  if ((instrWord > kInstrs) || (!Phv::is_valid_phv(phvWord))) return false;
  int off = -1;
  int width = Phv::which_width(phvWord);
  switch (width) {
    case 8:
      off = Phv::map_word_abs_to_rel8(phvWord);
      imem_subword8_.imem_subword8_instr(off, instrWord, instr);
      imem_subword8_.imem_subword8_color(off, instrWord, color);
      imem_subword8_.imem_subword8_parity(off, instrWord, parity);
      break;
    case 16:
      off = Phv::map_word_abs_to_rel16(phvWord);
      imem_subword16_.imem_subword16_instr(off, instrWord, instr);
      imem_subword16_.imem_subword16_color(off, instrWord, color);
      imem_subword16_.imem_subword16_parity(off, instrWord, parity);
      break;
    case 32:
      off = Phv::map_word_abs_to_rel32(phvWord);
      imem_subword32_.imem_subword32_instr(off, instrWord, instr);
      imem_subword32_.imem_subword32_color(off, instrWord, color);
      imem_subword32_.imem_subword32_parity(off, instrWord, parity);
      break;
  }
  return (off >= 0);
}

bool MauInstrStore::imem_write_parity(uint32_t instrWord, uint32_t phvWord, uint8_t parity) {
  if ((instrWord > kInstrs) || (!Phv::is_valid_phv(phvWord))) return false;
  int off = -1;
  int width = Phv::which_width(phvWord);
  switch (width) {
    case 8:
      off = Phv::map_word_abs_to_rel8(phvWord);
      imem_subword8_.imem_subword8_parity(off, instrWord, parity);
      break;
    case 16:
      off = Phv::map_word_abs_to_rel16(phvWord);
      imem_subword16_.imem_subword16_parity(off, instrWord, parity);
      break;
    case 32:
      off = Phv::map_word_abs_to_rel32(phvWord);
      imem_subword32_.imem_subword32_parity(off, instrWord, parity);
      break;
  }
  return (off >= 0);
}

}
