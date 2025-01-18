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

// MauInstrStore - Jbay specific code

#include <mau.h>
#include <phv.h>
#include <register_adapters.h>
#include <mau-instr-store.h>


namespace MODEL_CHIP_NAMESPACE {


MauInstrStoreAluConfig::MauInstrStoreAluConfig() {
  std::array<int, kAluTypes> n_alus_of_type;
  for (int t = 0; t < kAluTypes; t++) {
    n_alus_of_type[t] = 0;
    for (int off = 0; off < kAlusPerGrp; off++)
      alu_type_off_to_index_[t][off] = 0xFF;
  }

  for (int alu_index = 0; alu_index < kAlusPerGrp; alu_index++) {
    uint8_t type = find_alu_type(alu_index);
    RMT_ASSERT(type < kAluTypes);
    uint8_t off = n_alus_of_type[type];
    RMT_ASSERT(off < kAlusPerGrp);
    n_alus_of_type[type]++;
    alu_index_to_type_off_[alu_index] = make_type_off(type, off);
    alu_type_off_to_index_[type][off] = static_cast<uint8_t>(alu_index);
  }
}
MauInstrStoreAluConfig::~MauInstrStoreAluConfig() {
}
uint8_t MauInstrStoreAluConfig::map_type_off_to_index(uint8_t type, uint8_t off) {
  RMT_ASSERT((type < kAluTypes) && (off < kAlusPerGrp));
  uint8_t index = alu_type_off_to_index_[type][off];
  RMT_ASSERT(index < kAlusPerGrp);
  return index;
}
void MauInstrStoreAluConfig::map_index_to_type_off(uint8_t index, uint8_t *type, uint8_t *off) {
  RMT_ASSERT((type != NULL) && (off != NULL));
  RMT_ASSERT(index < kAlusPerGrp);
  uint16_t type_off = alu_index_to_type_off_[index];
  *type = get_type(type_off);
  *off = get_off(type_off);
  RMT_ASSERT((*type < kAluTypes) && (*off < kAlusPerGrp));
}



MauInstrStoreAluConfig* MauInstrStore::static_alu_config_ = NULL;
MauInstrStoreAluConfig* MauInstrStore::static_alu_config_init() {
  if (static_alu_config_ == NULL) {
    MauInstrStoreAluConfig *alu_config = new MauInstrStoreAluConfig();
    static_alu_config_ = alu_config;
  }
  return static_alu_config_;
}
MauInstrStore::MauInstrStore(int chipIndex, int pipeIndex, int mauIndex, Mau *mau)
    : MauInstrStoreCommon(chipIndex, pipeIndex, mauIndex, mau),
      alu_config_(static_alu_config_init()),
      imem_subword8_(default_adapter(imem_subword8_,chipIndex,pipeIndex,mauIndex,
                     [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword8_rw_cb(kAluTypeNormal,i,j,k,l,true); },
                     [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword8_rw_cb(kAluTypeNormal,i,j,k,l,false); })),
      imem_subword16_(default_adapter(imem_subword16_,chipIndex,pipeIndex,mauIndex,
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword16_rw_cb(kAluTypeNormal,i,j,k,l,true); },
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword16_rw_cb(kAluTypeNormal,i,j,k,l,false); })),
      imem_subword32_(default_adapter(imem_subword32_,chipIndex,pipeIndex,mauIndex,
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword32_rw_cb(kAluTypeNormal,i,j,k,l,true); },
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword32_rw_cb(kAluTypeNormal,i,j,k,l,false); })),
      imem_mocha_subword8_(default_adapter(imem_mocha_subword8_,chipIndex,pipeIndex,mauIndex,
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword8_rw_cb(kAluTypeMocha,i,j,k,l,true); },
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword8_rw_cb(kAluTypeMocha,i,j,k,l,false); })),
      imem_mocha_subword16_(default_adapter(imem_mocha_subword16_,chipIndex,pipeIndex,mauIndex,
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword16_rw_cb(kAluTypeMocha,i,j,k,l,true); },
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword16_rw_cb(kAluTypeMocha,i,j,k,l,false); })),
      imem_mocha_subword32_(default_adapter(imem_mocha_subword32_,chipIndex,pipeIndex,mauIndex,
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword32_rw_cb(kAluTypeMocha,i,j,k,l,true); },
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword32_rw_cb(kAluTypeMocha,i,j,k,l,false); })),
      imem_dark_subword8_(default_adapter(imem_dark_subword8_,chipIndex,pipeIndex,mauIndex,
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword8_rw_cb(kAluTypeDark,i,j,k,l,true); },
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword8_rw_cb(kAluTypeDark,i,j,k,l,false); })),
      imem_dark_subword16_(default_adapter(imem_dark_subword16_,chipIndex,pipeIndex,mauIndex,
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword16_rw_cb(kAluTypeDark,i,j,k,l,true); },
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword16_rw_cb(kAluTypeDark,i,j,k,l,false); })),
      imem_dark_subword32_(default_adapter(imem_dark_subword32_,chipIndex,pipeIndex,mauIndex,
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword32_rw_cb(kAluTypeDark,i,j,k,l,true); },
                      [this](uint32_t i,uint32_t j, uint32_t k, uint32_t l){this->imem_subword32_rw_cb(kAluTypeDark,i,j,k,l,false); })),
      imem_table_addr_format_(default_adapter(imem_table_addr_format_,chipIndex,pipeIndex,mauIndex)),
      mau_action_instruction_adr_mode_(default_adapter(mau_action_instruction_adr_mode_,chipIndex,pipeIndex,mauIndex)),
      imem_table_selector_fallback_addr_(default_adapter(imem_table_selector_fallback_addr_,chipIndex,pipeIndex,mauIndex)),
      imem_table_selector_fallback_icxbar_(default_adapter(imem_table_selector_fallback_icxbar_,chipIndex,pipeIndex,mauIndex)),
      imem_word_read_override_(default_adapter(imem_word_read_override_,chipIndex,pipeIndex,mauIndex)),
      fallback_using_lts_(0)
{
  imem_subword8_.reset();
  imem_subword16_.reset();
  imem_subword32_.reset();
  imem_mocha_subword8_.reset();
  imem_mocha_subword16_.reset();
  imem_mocha_subword32_.reset();
  imem_dark_subword8_.reset();
  imem_dark_subword16_.reset();
  imem_dark_subword32_.reset();
  imem_table_addr_format_.reset();
  mau_action_instruction_adr_mode_.reset();
  imem_table_selector_fallback_addr_.reset();
  imem_table_selector_fallback_icxbar_.reset();
  imem_word_read_override_.reset();
  for (int i = 0; i < kLogicalTables; i++) ops_per_lt_[i] = 0;
}
MauInstrStore::~MauInstrStore() {
}

void MauInstrStore::imem_subword8_rw_cb(uint8_t aluType, uint32_t aluHalf, uint32_t aluGrpInHalf,
                                        uint32_t aluWord, uint32_t instrWord, bool write) {
  RMT_ASSERT( ((aluHalf == 0) || (aluHalf == 1)) && (aluGrpInHalf < (kNumGroups8b/2)) );
  int aluGrp = (aluHalf * (kNumGroups8b/2)) + aluGrpInHalf;
  int phvWord8 = (aluGrp * kAlusPerGrp) + alu_config_->map_type_off_to_index(aluType, aluWord);
  int phvWord = Phv::map_word_rel8_to_abs(phvWord8);
  RMT_ASSERT((instrWord < kInstrs) && (Phv::is_valid_phv(phvWord)))
  instr_rw_callback(instrWord, phvWord, write);
}
void MauInstrStore::imem_subword16_rw_cb(uint8_t aluType, uint32_t aluHalf, uint32_t aluGrpInHalf,
                                         uint32_t aluWord, uint32_t instrWord, bool write) {
  RMT_ASSERT( ((aluHalf == 0) || (aluHalf == 1)) && (aluGrpInHalf < (kNumGroups16b/2)) );
  int aluGrp = (aluHalf * (kNumGroups16b/2)) + aluGrpInHalf;
  int phvWord16 = (aluGrp * kAlusPerGrp) + alu_config_->map_type_off_to_index(aluType, aluWord);
  int phvWord = Phv::map_word_rel16_to_abs(phvWord16);
  RMT_ASSERT((instrWord < kInstrs) && (Phv::is_valid_phv(phvWord)));
  instr_rw_callback(instrWord, phvWord, write);
}
void MauInstrStore::imem_subword32_rw_cb(uint8_t aluType, uint32_t aluHalf, uint32_t aluGrpInHalf,
                                         uint32_t aluWord, uint32_t instrWord, bool write) {
  RMT_ASSERT( ((aluHalf == 0) || (aluHalf == 1)) && (aluGrpInHalf < (kNumGroups32b/2)) );
  int aluGrp = (aluHalf * (kNumGroups32b/2)) + aluGrpInHalf;
  int phvWord32 = (aluGrp * kAlusPerGrp) + alu_config_->map_type_off_to_index(aluType, aluWord);
  int phvWord = Phv::map_word_rel32_to_abs(phvWord32);
  RMT_ASSERT((instrWord < kInstrs) && (Phv::is_valid_phv(phvWord)));
  instr_rw_callback(instrWord, phvWord, write);
}



bool MauInstrStore::imem_normal_read(uint32_t instrWord, uint32_t phvWord,
                                     uint32_t *instr, uint8_t *color, uint8_t *parity) {
  if ((instrWord >= kInstrs) || (!Phv::is_valid_phv(phvWord))) return false;
  uint8_t aluIndex = static_cast<uint8_t>(phvWord % kAlusPerGrp);
  uint8_t type, off, col, par;
  alu_config_->map_index_to_type_off(aluIndex, &type, &off);
  if (type != kAluTypeNormal) return false;

  uint32_t ins, div, aluGrp, aluOff = static_cast<uint32_t>(off);
  int width = Phv::which_width(phvWord);
  switch (width) {
    case 8:
      aluGrp = Phv::map_word_abs_to_rel8(phvWord) / kAlusPerGrp; div = (kNumGroups8b/2);
      ins = imem_subword8_.imem_subword8_instr(aluGrp/div, aluGrp%div, aluOff, instrWord);
      col = imem_subword8_.imem_subword8_color(aluGrp/div, aluGrp%div, aluOff, instrWord);
      par = imem_subword8_.imem_subword8_parity(aluGrp/div, aluGrp%div, aluOff, instrWord);
      break;
    case 16:
      aluGrp = Phv::map_word_abs_to_rel16(phvWord) / kAlusPerGrp; div = (kNumGroups16b/2);
      ins = imem_subword16_.imem_subword16_instr(aluGrp/div, aluGrp%div, aluOff, instrWord);
      col = imem_subword16_.imem_subword16_color(aluGrp/div, aluGrp%div, aluOff, instrWord);
      par = imem_subword16_.imem_subword16_parity(aluGrp/div, aluGrp%div, aluOff, instrWord);
      break;
    case 32:
      aluGrp = Phv::map_word_abs_to_rel32(phvWord) / kAlusPerGrp; div = (kNumGroups32b/2);
      ins = imem_subword32_.imem_subword32_instr(aluGrp/div, aluGrp%div, aluOff, instrWord);
      col = imem_subword32_.imem_subword32_color(aluGrp/div, aluGrp%div, aluOff, instrWord);
      par = imem_subword32_.imem_subword32_parity(aluGrp/div, aluGrp%div, aluOff, instrWord);
      break;
    default: RMT_ASSERT(0);
  }
  if (instr != NULL) *instr = ins;
  if (color != NULL) *color = col;
  if (parity != NULL) *parity = par;
  return true;
}
bool MauInstrStore::imem_mocha_read(uint32_t instrWord, uint32_t phvWord,
                                    uint32_t *instr, uint8_t *color, uint8_t *parity) {
  if ((instrWord >= kInstrs) || (!Phv::is_valid_phv(phvWord))) return false;
  uint8_t aluIndex = static_cast<uint8_t>(phvWord % kAlusPerGrp);
  uint8_t type, off, col, par;
  alu_config_->map_index_to_type_off(aluIndex, &type, &off);
  if (type != kAluTypeMocha) return false;

  uint32_t ins, div, aluGrp,  aluOff = static_cast<uint32_t>(off);
  int width = Phv::which_width(phvWord);
  switch (width) {
    case 8:
      aluGrp = Phv::map_word_abs_to_rel8(phvWord) / kAlusPerGrp; div = (kNumGroups8b/2);
      ins = imem_mocha_subword8_.imem_mocha_subword_instr(aluGrp/div, aluGrp%div, aluOff, instrWord);
      col = imem_mocha_subword8_.imem_mocha_subword_color(aluGrp/div, aluGrp%div, aluOff, instrWord);
      par = imem_mocha_subword8_.imem_mocha_subword_parity(aluGrp/div, aluGrp%div, aluOff, instrWord);
      break;
    case 16:
      aluGrp = Phv::map_word_abs_to_rel16(phvWord) / kAlusPerGrp; div = (kNumGroups16b/2);
      ins = imem_mocha_subword16_.imem_mocha_subword_instr(aluGrp/div, aluGrp%div, aluOff, instrWord);
      col = imem_mocha_subword16_.imem_mocha_subword_color(aluGrp/div, aluGrp%div, aluOff, instrWord);
      par = imem_mocha_subword16_.imem_mocha_subword_parity(aluGrp/div, aluGrp%div, aluOff, instrWord);
      break;
    case 32:
      aluGrp = Phv::map_word_abs_to_rel32(phvWord) / kAlusPerGrp; div = (kNumGroups32b/2);
      ins = imem_mocha_subword32_.imem_mocha_subword_instr(aluGrp/div, aluGrp%div, aluOff, instrWord);
      col = imem_mocha_subword32_.imem_mocha_subword_color(aluGrp/div, aluGrp%div, aluOff, instrWord);
      par = imem_mocha_subword32_.imem_mocha_subword_parity(aluGrp/div, aluGrp%div, aluOff, instrWord);
      break;
    default: RMT_ASSERT(0);
  }
  if (instr != NULL) *instr = ins;
  if (color != NULL) *color = col;
  if (parity != NULL) *parity = par;
  return true;
}
bool MauInstrStore::imem_dark_read(uint32_t instrWord, uint32_t phvWord,
                                    uint32_t *instr, uint8_t *color, uint8_t *parity) {
  if ((instrWord >= kInstrs) || (!Phv::is_valid_phv(phvWord))) return false;
  uint8_t aluIndex = static_cast<uint8_t>(phvWord % kAlusPerGrp);
  uint8_t type, off, col, par;
  alu_config_->map_index_to_type_off(aluIndex, &type, &off);
  if (type != kAluTypeDark) return false;

  uint32_t ins, div, aluGrp, aluOff = static_cast<uint32_t>(off);
  int width = Phv::which_width(phvWord);
  switch (width) {
    case 8:
      aluGrp = Phv::map_word_abs_to_rel8(phvWord) / kAlusPerGrp; div = (kNumGroups8b/2);
      ins = imem_dark_subword8_.imem_dark_subword_instr(aluGrp/div, aluGrp%div, aluOff, instrWord);
      col = imem_dark_subword8_.imem_dark_subword_color(aluGrp/div, aluGrp%div, aluOff, instrWord);
      par = imem_dark_subword8_.imem_dark_subword_parity(aluGrp/div, aluGrp%div, aluOff, instrWord);
      break;
    case 16:
      aluGrp = Phv::map_word_abs_to_rel16(phvWord) / kAlusPerGrp; div = (kNumGroups16b/2);
      ins = imem_dark_subword16_.imem_dark_subword_instr(aluGrp/div, aluGrp%div, aluOff, instrWord);
      col = imem_dark_subword16_.imem_dark_subword_color(aluGrp/div, aluGrp%div, aluOff, instrWord);
      par = imem_dark_subword16_.imem_dark_subword_parity(aluGrp/div, aluGrp%div, aluOff, instrWord);
      break;
    case 32:
      aluGrp = Phv::map_word_abs_to_rel32(phvWord) / kAlusPerGrp; div = (kNumGroups32b/2);
      ins = imem_dark_subword32_.imem_dark_subword_instr(aluGrp/div, aluGrp%div, aluOff, instrWord);
      col = imem_dark_subword32_.imem_dark_subword_color(aluGrp/div, aluGrp%div, aluOff, instrWord);
      par = imem_dark_subword32_.imem_dark_subword_parity(aluGrp/div, aluGrp%div, aluOff, instrWord);
      break;
    default: RMT_ASSERT(0);
  }
  if (instr != NULL) *instr = ins;
  if (color != NULL) *color = col;
  if (parity != NULL) *parity = par;
  return true;
}
bool MauInstrStore::imem_read(uint32_t instrWord, uint32_t phvWord,
                              uint32_t *instr, uint8_t *color, uint8_t *parity) {
  if ((instrWord >= kInstrs) || (!Phv::is_valid_phv(phvWord))) return false;
  uint8_t aluIndex = static_cast<uint8_t>(phvWord % kAlusPerGrp);
  uint8_t type, off;
  alu_config_->map_index_to_type_off(aluIndex, &type, &off);
  switch (type) {
    case kAluTypeNormal: return imem_normal_read(instrWord, phvWord, instr, color, parity);
    case kAluTypeMocha:  return imem_mocha_read(instrWord, phvWord, instr, color, parity);
    case kAluTypeDark:   return imem_dark_read(instrWord, phvWord, instr, color, parity);
    default: RMT_ASSERT(0);
  }
}



bool MauInstrStore::imem_normal_write(uint32_t instrWord, uint32_t phvWord,
                                      uint32_t instr, uint8_t color, uint8_t parity,
                                      bool write_parity_only) {
  if ((instrWord >= kInstrs) || (!Phv::is_valid_phv(phvWord))) return false;
  uint8_t aluIndex = static_cast<uint8_t>(phvWord % kAlusPerGrp);
  uint8_t type, off;
  alu_config_->map_index_to_type_off(aluIndex, &type, &off);
  if (type != kAluTypeNormal) return false;

  uint32_t div, aluGrp, aluOff = static_cast<uint32_t>(off);
  int width = Phv::which_width(phvWord);
  switch (width) {
    case 8:
      aluGrp = Phv::map_word_abs_to_rel8(phvWord) / kAlusPerGrp; div = (kNumGroups8b/2);
      imem_subword8_.imem_subword8_parity(aluGrp/div, aluGrp%div, aluOff, instrWord, parity);
      if (write_parity_only) break;
      imem_subword8_.imem_subword8_color(aluGrp/div, aluGrp%div, aluOff, instrWord, color);
      imem_subword8_.imem_subword8_instr(aluGrp/div, aluGrp%div, aluOff, instrWord, instr);
      break;
    case 16:
      aluGrp = Phv::map_word_abs_to_rel16(phvWord) / kAlusPerGrp; div = (kNumGroups16b/2);
      imem_subword16_.imem_subword16_parity(aluGrp/div, aluGrp%div, aluOff, instrWord, parity);
      if (write_parity_only) break;
      imem_subword16_.imem_subword16_color(aluGrp/div, aluGrp%div, aluOff, instrWord, color);
      imem_subword16_.imem_subword16_instr(aluGrp/div, aluGrp%div, aluOff, instrWord, instr);
      break;
    case 32:
      aluGrp = Phv::map_word_abs_to_rel32(phvWord) / kAlusPerGrp; div = (kNumGroups32b/2);
      imem_subword32_.imem_subword32_parity(aluGrp/div, aluGrp%div, aluOff, instrWord, parity);
      if (write_parity_only) break;
      imem_subword32_.imem_subword32_color(aluGrp/div, aluGrp%div, aluOff, instrWord, color);
      imem_subword32_.imem_subword32_instr(aluGrp/div, aluGrp%div, aluOff, instrWord, instr);
      break;
    default: RMT_ASSERT(0);
  }
  return true;
}
bool MauInstrStore::imem_mocha_write(uint32_t instrWord, uint32_t phvWord,
                                     uint32_t instr, uint8_t color, uint8_t parity,
                                     bool write_parity_only) {
  if ((instrWord >= kInstrs) || (!Phv::is_valid_phv(phvWord))) return false;
  uint8_t aluIndex = static_cast<uint8_t>(phvWord % kAlusPerGrp);
  uint8_t type, off;
  alu_config_->map_index_to_type_off(aluIndex, &type, &off);
  if (type != kAluTypeMocha) return false;

  uint32_t div, aluGrp, aluOff = static_cast<uint32_t>(off);
  int width = Phv::which_width(phvWord);
  switch (width) {
    case 8:
      aluGrp = Phv::map_word_abs_to_rel8(phvWord) / kAlusPerGrp; div = (kNumGroups8b/2);
      imem_mocha_subword8_.imem_mocha_subword_parity(aluGrp/div, aluGrp%div, aluOff, instrWord, parity);
      if (write_parity_only) break;
      imem_mocha_subword8_.imem_mocha_subword_color(aluGrp/div, aluGrp%div, aluOff, instrWord, color);
      imem_mocha_subword8_.imem_mocha_subword_instr(aluGrp/div, aluGrp%div, aluOff, instrWord, instr);
      break;
    case 16:
      aluGrp = Phv::map_word_abs_to_rel16(phvWord) / kAlusPerGrp; div = (kNumGroups16b/2);
      imem_mocha_subword16_.imem_mocha_subword_parity(aluGrp/div, aluGrp%div, aluOff, instrWord, parity);
      if (write_parity_only) break;
      imem_mocha_subword16_.imem_mocha_subword_color(aluGrp/div, aluGrp%div, aluOff, instrWord, color);
      imem_mocha_subword16_.imem_mocha_subword_instr(aluGrp/div, aluGrp%div, aluOff, instrWord, instr);
      break;
    case 32:
      aluGrp = Phv::map_word_abs_to_rel32(phvWord) / kAlusPerGrp; div = (kNumGroups32b/2);
      imem_mocha_subword32_.imem_mocha_subword_parity(aluGrp/div, aluGrp%div, aluOff, instrWord, parity);
      if (write_parity_only) break;
      imem_mocha_subword32_.imem_mocha_subword_color(aluGrp/div, aluGrp%div, aluOff, instrWord, color);
      imem_mocha_subword32_.imem_mocha_subword_instr(aluGrp/div, aluGrp%div, aluOff, instrWord, instr);
      break;
    default: RMT_ASSERT(0);
  }
  return true;
}
bool MauInstrStore::imem_dark_write(uint32_t instrWord, uint32_t phvWord,
                                    uint32_t instr, uint8_t color, uint8_t parity,
                                    bool write_parity_only) {
  if ((instrWord >= kInstrs) || (!Phv::is_valid_phv(phvWord))) return false;
  uint8_t aluIndex = static_cast<uint8_t>(phvWord % kAlusPerGrp);
  uint8_t type, off;
  alu_config_->map_index_to_type_off(aluIndex, &type, &off);
  if (type != kAluTypeDark) return false;

  uint32_t div, aluGrp, aluOff = static_cast<uint32_t>(off);
  int width = Phv::which_width(phvWord);
  switch (width) {
    case 8:
      aluGrp = Phv::map_word_abs_to_rel8(phvWord) / kAlusPerGrp; div = (kNumGroups8b/2);
      imem_dark_subword8_.imem_dark_subword_parity(aluGrp/div, aluGrp%div, aluOff, instrWord, parity);
      if (write_parity_only) break;
      imem_dark_subword8_.imem_dark_subword_color(aluGrp/div, aluGrp%div, aluOff, instrWord, color);
      imem_dark_subword8_.imem_dark_subword_instr(aluGrp/div, aluGrp%div, aluOff, instrWord, instr);
      break;
    case 16:
      aluGrp = Phv::map_word_abs_to_rel16(phvWord) / kAlusPerGrp; div = (kNumGroups16b/2);
      imem_dark_subword16_.imem_dark_subword_parity(aluGrp/div, aluGrp%div, aluOff, instrWord, parity);
      if (write_parity_only) break;
      imem_dark_subword16_.imem_dark_subword_color(aluGrp/div, aluGrp%div, aluOff, instrWord, color);
      imem_dark_subword16_.imem_dark_subword_instr(aluGrp/div, aluGrp%div, aluOff, instrWord, instr);
      break;
    case 32:
      aluGrp = Phv::map_word_abs_to_rel32(phvWord) / kAlusPerGrp; div = (kNumGroups32b/2);
      imem_dark_subword32_.imem_dark_subword_parity(aluGrp/div, aluGrp%div, aluOff, instrWord, parity);
      if (write_parity_only) break;
      imem_dark_subword32_.imem_dark_subword_color(aluGrp/div, aluGrp%div, aluOff, instrWord, color);
      imem_dark_subword32_.imem_dark_subword_instr(aluGrp/div, aluGrp%div, aluOff, instrWord, instr);
      break;
    default: RMT_ASSERT(0);
  }
  return true;
}
bool MauInstrStore::imem_write(uint32_t instrWord, uint32_t phvWord,
                               uint32_t instr, uint8_t color, uint8_t parity,
                               bool write_parity_only) {
  if ((instrWord >= kInstrs) || (!Phv::is_valid_phv(phvWord))) return false;
  uint8_t aluIndex = static_cast<uint8_t>(phvWord % kAlusPerGrp);
  uint8_t type, off;
  alu_config_->map_index_to_type_off(aluIndex, &type, &off);
  switch (type) {
    case kAluTypeNormal:
      return imem_normal_write(instrWord, phvWord, instr, color, parity, write_parity_only);
    case kAluTypeMocha:
      return imem_mocha_write(instrWord, phvWord, instr, color, parity, write_parity_only);
    case kAluTypeDark:
      return imem_dark_write(instrWord, phvWord, instr, color, parity, write_parity_only);
    default: RMT_ASSERT(0);
  }
}


bool MauInstrStore::imem_write(uint32_t instrWord, uint32_t phvWord,
                               uint32_t instr, uint8_t color, uint8_t parity) {
  return imem_write(instrWord, phvWord, instr, color, parity, false);
}
bool MauInstrStore::imem_write_parity(uint32_t instrWord, uint32_t phvWord, uint8_t parity) {
  return imem_write(instrWord, phvWord, 0u, 0, parity, true);
}


void MauInstrStore::instr_reset_chip(Phv *phv) {
  // Reset Snapshot info
  fallback_using_lts_ = 0;
  for (int i = 0; i < kLogicalTables; i++) ops_per_lt_[i] = 0;

  // If configured add unconditional op (instrWord=31(0x1F),color=1)
  // provided relevant thread is active
  if ((phv->ingress() || phv->ghost()) &&
      (imem_word_read_override_.imem_word_read_override_ingress() == 1)) {
    instr_add_simple_op((0<<7) | (1<<6) | (0x1F<<1) | (1<<0));
  }
  if ((phv->egress()) &&
      (imem_word_read_override_.imem_word_read_override_egress() == 1)) {
    instr_add_simple_op((1<<7) | (1<<6) | (0x1F<<1) | (1<<0));
  }
}
void MauInstrStore::instr_add_bitmask_op(uint8_t iegress, uint8_t pfe,
                                         uint8_t op_base, uint8_t op_bitmask,
                                         uint8_t color) {
  uint8_t op0 = ((iegress & 1) << 7) | ((pfe & 1) << 6) | (0 << 1) | ((color & 1) << 0);
  for (int i = 0; i < 8; i++) {
    if (((op_bitmask >> i) & 1) == 1) {
      uint8_t instrWord = op_base + i;
      RMT_ASSERT(instrWord < kInstrs);
      uint8_t op = op0 | ((instrWord & 0x1F) << 1);
      instr_add_simple_op(op);
    }
  }
}
bool MauInstrStore::instr_fallback(int logical_table) {
  // Should we use fallback instr or not - LT might not care
  RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
  for (int alu = 0; alu < kNumSelectorAlus; alu++) {
    if (mau()->is_selector_alu_output_invalid(alu)) {
      uint16_t lts_that_care = fallback_ctl(alu);
      if ((lts_that_care & (1<<logical_table)) != 0) return true;
    }
  }
  return false;
}
bool MauInstrStore::instr_add_op(int logical_table, bool ingress, uint8_t op) {
  RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
  uint16_t lt_mask = 1<<logical_table;
  uint8_t  iegress = ingress ?0 :1;
  bool     en = true;

  if (instr_fallback(logical_table)) {
    fallback_using_lts_ |= lt_mask; // For Snapshot

    bool fallback_bitmask_ops = has_fallback_bitmask_ops(logical_table);
    if (fallback_bitmask_ops) {
      uint8_t pfe = 1; // Bitmask op implies PFE=1
      uint8_t base =  fallback_bitmap_base(logical_table);
      uint8_t bitmask = fallback_addr(logical_table);
      uint8_t color = fallback_bitmap_color(logical_table);
      ops_per_lt_[logical_table] = bitmask; // Store bitmask for Snapshot
      instr_add_bitmask_op(iegress, pfe, base, bitmask, color);
    } else {
      //uint8_t pfe = 1; // Encoded op is always enabled
      //uint8_t op = fallback_addr(logical_table) & ((0x1F<<1)|(1<<0)); // Just lower 6b
      //ops_per_lt_[logical_table] = op | (pfe<<6); // Store op + pfe=1 for Snapshot
      //op |= (iegress<<7) | (pfe<<6); // but OR in iegress and PFE=1
      //instr_add_simple_op(op);

      // See email "IMEM Fallback Address" 28Jun2018 - fallback addr DOES contain PFE!
      uint8_t op = fallback_addr(logical_table) & 0x7F;
      ops_per_lt_[logical_table] = op; // Store op for Snapshot
      // OR in iegress for instr_add_simple_op
      if (Address::action_instr_enabled(op)) instr_add_simple_op(op | (iegress<<7));
    }
  } else {
    fallback_using_lts_ &= ~lt_mask; // For Snapshot

    bool bitmask_ops = has_bitmask_ops(logical_table);
    if (bitmask_ops) {
      uint8_t pfe = 1; // Bit set in bitmask implies PFE=1
      uint8_t base =  bitmap_base(logical_table);
      uint8_t bitmask = op;
      uint8_t color = bitmap_color(logical_table);
      ops_per_lt_[logical_table] = bitmask; // Store bitmask for Snapshot
      instr_add_bitmask_op(iegress, pfe, base, bitmask, color);
    } else if (Address::action_instr_enabled(op)) {
      ops_per_lt_[logical_table] = op & ~(1<<7); // Store op[6:0] for Snapshot
      uint8_t op2 = (ingress) ?(op & ~(1<<7)) :(op | (1<<7));
      RMT_ASSERT(op == op2); // Check iegress was set during extract_action_instr_addr
      instr_add_simple_op(op);
    } else {
      ops_per_lt_[logical_table] = 0;
      en = false;
    }
  }
  return en; // true if OP(s) successfully added
}
bool MauInstrStore::has_bitmask_ops(int logical_table) {
  RMT_ASSERT((logical_table >= 0) && (logical_table < kLogicalTables));
  uint16_t act_ins_mode0 = mau_action_instruction_adr_mode_.mau_action_instruction_adr_mode(0);
  uint16_t act_ins_mode1 = mau_action_instruction_adr_mode_.mau_action_instruction_adr_mode(1);
  if (act_ins_mode0 != act_ins_mode1) {
    bool relax = kRelaxAddrFormatReplicationCheck;
    RMT_LOG_OBJ(mau(), RmtDebug::error(RmtDebug::kRmtDebugMauAlu,relax),
                "has_bitmask_ops: mau_action_instruction_adr_mode "
                "NOT replicated correctly ([0]=0x%04x [1]=0x%04x)\n",
                act_ins_mode0, act_ins_mode1);
    if (!relax) { THROW_ERROR(-2); } // For DV
  }
  bool imem_fmt = (((imem_table_addr_format_.addr_format(logical_table)) & 1) == 1);
  bool act_ins_fmt = (((act_ins_mode0 >> logical_table) & 1) == 1);
  if (imem_fmt != act_ins_fmt) {
    bool relax = kRelaxAddrFormatReplicationCheck;
    RMT_LOG_OBJ(mau(), RmtDebug::error(RmtDebug::kRmtDebugMauAlu,relax),
                "has_bitmask_ops: imem_table_addr_format and mau_action_instruction_adr_mode "
                "do NOT match for LT %d (imem_table_addr_format=%d mau_act_ins_adr_mode=%d)\n",
                logical_table, imem_fmt, act_ins_fmt);
    if (!relax) { THROW_ERROR(-2); } // For DV
  }
  return imem_fmt;
}




}
