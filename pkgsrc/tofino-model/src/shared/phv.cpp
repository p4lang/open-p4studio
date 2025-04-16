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

#include <phv.h>

#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <packet.h>
#include <teop.h>
#include <phv-pipe-data.h>
#include <phv-factory.h>
#include <p4-name-lookup.h>


namespace MODEL_CHIP_NAMESPACE {


Phv::Phv(RmtObjectManager *om, PhvFactory *pf)
    : RmtObject(om), RmtLogger(om,RmtTypes::kRmtTypePhv),
      pf_(pf), teop_(nullptr), pipe_data_(nullptr),
      valid_(UINT64_C(0)), error_(nullptr), written_(nullptr),
      flags_(0u), pipe_index_(0), source_(0) {

  // phv_index_to_off16_p and phv_off16_to_index_p funcs assume
  // a particular layout of PHV words - check as expected
  RMT_ASSERT(RmtDefsShared::kPhv_WidthPerGroup[0] == 32);
  RMT_ASSERT(RmtDefsShared::kPhv_WidthPerGroup[1] == 32);
  RMT_ASSERT(RmtDefsShared::kPhv_WidthPerGroup[2] ==  8);
  RMT_ASSERT(RmtDefsShared::kPhv_WidthPerGroup[3] ==  8);
  RMT_ASSERT(RmtDefsShared::kPhv_WidthPerGroup[4] == 16);
  RMT_ASSERT(RmtDefsShared::kPhv_WidthPerGroup[5] == 16);
  RMT_ASSERT(RmtDefsShared::kPhv_WidthPerGroup[6] == 16);

  for (int i = 0; i < kWordsMaxExtended; i++) words_[i] = 0u;
}
Phv::Phv(RmtObjectManager *om) : Phv(om,NULL) { }

Phv::~Phv() {
  RMT_LOG_VERBOSE("Destroying PHV\n");
  pf_ = nullptr;
  teop_ = nullptr;
  pipe_data_ = nullptr;
}


// Only used by JBay Parser to map PHV words to 256x16b offset and vice-versa
void Phv::phv_index_to_off16_p(int phvWord, int *size,
                               int *off16A, int *off16B, int *sz8_01) {
  RMT_ASSERT((phvWord >= 0) && (phvWord < kWordsMax));
  RMT_ASSERT((size != NULL) && (off16A != NULL));
  const unsigned sz32 = sizeof(uint32_t), sz16 = sizeof(uint16_t), sz8 = sizeof(uint8_t);
  const int n32 = 32+32, n8 = 32+32; // n16 = 32+32+32;
  if (phvWord >= n32+n8) {
    *size = 16; // 16b PHV word
    *off16A = ( n32*sz32  + n8*sz8  + (( phvWord-(n32+n8) )*sz16) ) / sz16;
    if (off16B != NULL) *off16B = -1;
    if (sz8_01 != NULL) *sz8_01 = 99999;
  } else if (phvWord >= n32) {
    *size = 8;  // 8b PHV word
    *off16A = ( n32*sz32  + ( phvWord-n32 )*sz8 ) / sz16;
    if (off16B != NULL) *off16B = -1;
    if (sz8_01 != NULL) *sz8_01 = (phvWord-n32)%sz16;
  } else {
    *size = 32; // 32b PHV word
    *off16A = ( 0 + (phvWord-0)*sz32 ) / sz16;
    if (off16B != NULL) {
      *off16B = *off16A + 1;
      RMT_ASSERT(*off16B < 256);
    }
    if (sz8_01 != NULL) *sz8_01 = -99999;
  }
  RMT_ASSERT((*off16A >= 0) && (*off16A < 256));
}
void Phv::phv_off16_to_index_p(int off16, int *size,
                               int *phvWordA, int *phvWordB, int *sz32_01) {
  RMT_ASSERT((off16 >= 0) && (off16 < 256));
  RMT_ASSERT((size != NULL) && (phvWordA != NULL));
  const unsigned sz32 = sizeof(uint32_t), sz16 = sizeof(uint16_t), sz8 = sizeof(uint8_t);
  const int n32 = 32+32, n8 = 32+32; // n16 = 32+32+32;
  if (off16*sz16 >= n32*sz32 + n8*sz8) {
    *size = 16; // 16b PHV word
    *phvWordA = ( (off16*sz16 - n32*sz32 - n8*sz8) / sz16 )  + (n32+n8);
    if (phvWordB != NULL) *phvWordB = -1;
    if (sz32_01 != NULL)  *sz32_01 = 99999;
  } else if (off16*sz16 >= n32*sz32) {
    *size = 8;  // 8b PHV word
    *phvWordA = ( (off16*sz16 - n32*sz32) / sz8 ) + n32;
    if (phvWordB != NULL) {
      *phvWordB = *phvWordA + 1;
      RMT_ASSERT((*phvWordB < kWordsMax));
    }
    if (sz32_01 != NULL)  *sz32_01 = -99999;
  } else {
    *size = 32; // 32b PHV word
    *phvWordA = ( off16*sz16/sz32 ) + 0;
    if (phvWordB != NULL) *phvWordB = -1;
    if (sz32_01 != NULL)  *sz32_01 = off16%sz16;
  }
  RMT_ASSERT((*phvWordA >= 0) && (*phvWordA < kWordsMax));
}


void Phv::set_all_valid() {
  valid_.fill_all_ones();
}


Phv *Phv::clone_internal(int n_words) {
  Phv *clone_phv = get_object_manager()->phv_create();
  clone_phv->source_ = source_;
  clone_phv->flags_ = flags_;
  clone_phv->set_pipe(pipe_index());
  // Just copy teop ptr - NULLed on free
  clone_phv->teop_ = teop_;
  // Just copy pipe_data ptr - NULLed on free
  clone_phv->pipe_data_ = pipe_data_;
  // Ignore PhvFactory, duplicate cache_id
  clone_phv->cache_id_.SetFrom(cache_id_);
  // OK to copy PacketInfo - note freeing PHV/PacketInfo does not free packet
  clone_phv->ingress_ = ingress_;
  clone_phv->egress_ = egress_;
  if (n_words > 0) {
    // Copy *all* words - but recreate valid/error
    for (int i = 0; i < n_words; i++) {
      clone_phv->words_[i] = 0u;
      if (Phv::is_valid_phv_x(i)) {
        clone_phv->set_x(i, get_x(i));
        clone_phv->set_valid_x(i, is_valid_x(i));
        if (has_error_x(i)) clone_phv->set_error_x(i, true);
      }
    }
  }
  return clone_phv;
}
Phv *Phv::clone(bool copydata) {
  return clone_internal((copydata) ?kWordsMax :0);
}
Phv *Phv::clone_x(bool copydata) {
  return clone_internal((copydata) ?kWordsMaxExtended :0);
}


void Phv::copyinfo(Phv *phv, bool ingress_info) {
  if (ingress_info) {
    ingress_ = phv->ingress_;
  } else {
    egress_ = phv->egress_;
    teop_ = phv->teop_;
  }
}
void Phv::copydata(Phv *phv, BitVector<kWordsMax> *selector) {
  for (int i = 0; i < kWordsMax; i++) {
    // Copy all selected words - but recreate valid/error
    if (selector->bit_set(i)) {
      words_[i] = 0u;
      set(i, phv->get(i));
      set_valid(i, phv->is_valid(i));
      if (phv->has_error(i)) set_error(i, true); // Copy errors?
    }
  }
}
void Phv::copydata_x(Phv *phv, BitVector<kWordsMaxExtended> *selector) {
  for (int i = 0; i < kWordsMaxExtended; i++) {
    // Copy all selected words - but recreate valid/error
    if (Phv::is_valid_phv_x(i) && selector->bit_set(i)) {
      words_[i] = 0u;
      set_x(i, phv->get_x(i));
      set_valid_x(i, phv->is_valid_x(i));
      if (phv->has_error_x(i)) set_error_x(i, true); // Copy errors?
    }
  }
}

void Phv::merge_phvs(Phv *iphv, Phv *ephv) {
  RMT_ASSERT(iphv);
  RMT_ASSERT(ephv);
  RMT_ASSERT(iphv->pipe_index_ == ephv->pipe_index_);

  copyinfo(iphv, true);
  copyinfo(ephv, false);
  for (int i=0; i<kWordsMaxExtended; ++i) {
    words_[i] = iphv->words_[i] | ephv->words_[i];
  }

  valid_.or_with(iphv->valid_);
  valid_.or_with(ephv->valid_);
  if ((iphv->error_ != NULL) || (ephv->error_ != NULL)) {
    set_error(0); // So to create error_ if it doesn't yet exist
    if (iphv->error_ != NULL) error_->or_with(*iphv->error_);
    if (ephv->error_ != NULL) error_->or_with(*ephv->error_);
  }
  flags_ = iphv->flags_ | ephv->flags_;
  pipe_index_ = iphv->pipe_index_;
}

void Phv::maskdata(BitVector<kWordsMax> *selector) {
  // If bit NOT set in selector then zeroise PHV word - preserve valid/error
  for (int i = 0; i < kWordsMax; i++) {
    if (!selector->bit_set(i)) words_[i] = 0u;
  }
}

uint32_t Phv::hash() {
  // Only hash *valid* words
  uint32_t hashval = 0u;
  for (int i = 0; i < kWordsMaxExtended; i++) {
    if (Phv::is_valid_x(i)) hashval ^= get_x(i);
  }
  return hashval;
}

bool Phv::equals(Phv *other) {
  if (this == other) return true;
  // Don't compare PhvFactory, Teop, CacheId or written_ BitVector
  if (!valid_.equals(other->valid_)) return false;
  if ((error_ == nullptr) && (other->error_ != nullptr)) return false;
  if ((error_ != nullptr) && (other->error_ == nullptr)) return false;
  if ((error_ != nullptr) && (other->error_ != nullptr) &&
      (!error_->equals(*other->error_))) return false;
  for (int i = 0; i < kWordsMaxExtended; i++) {
    if (Phv::is_valid_phv_x(i)) {
      if (get_x(i) != other->get_x(i)) return false;
    }
  }
  // Insist ingress/egress status matches for equality
  if (ingress_.valid() != other->ingress_.valid()) return false;
  if (egress_.valid() != other->egress_.valid()) return false;
  return true;
}
bool Phv::identical(Phv *other) {
  if (this == other) return true;
  if (!equals(other)) return false;
  // Check contents of packet refs and versions
  if (ingress_.version() != other->ingress_.version()) return false;
  if (ingress_.packet() != other->ingress_.packet()) return false;
  if (egress_.version() != other->egress_.version()) return false;
  if (egress_.packet() != other->egress_.packet()) return false;
  // Also check contents invalid words ignoring masks
  for (int i = 0; i < kWordsMaxExtended; i++) {
    if (words_[i] != other->words_[i]) return false;
  }
  return true;
}
bool Phv::indistinguishable(Phv *other) {
  if (this == other) return true;
  if (!identical(other)) return false;
  // Finally check mutable state like pf/teop/cache_id/written vec
  if (pf_ != other->pf_) return false;
  if (teop_ != other->teop_) return false;
  if (pipe_data_ != other->pipe_data_) return false;
  if (!cache_id_.Equals(other->cache_id_)) return false;
  if ((written_ == nullptr) && (other->written_ != nullptr)) return false;
  if ((written_ != nullptr) && (other->written_ == nullptr)) return false;
  if ((written_ != nullptr) && (other->written_ != nullptr) &&
      (!written_->equals(*other->written_))) return false;
  return true;
}



void Phv::set_phv_by_byte(int byte, uint8_t byte_value) {
  // Currently there are 2x32 2x8 3x16 1x0 1x32 1x8 2x16 groups
  int group = 0;
  int byte_offset = byte;
  // First off find out which group this byte is in
  while (group < kGroupsExtended) {
    // WidthPerGroup array is *bitwidth* so divide by 8
    int bytes_per_word = which_width_in_bytes(group);
    RMT_ASSERT(bytes_per_word <= 4);
    int bytes_this_group = bytes_per_word * kWordsPerGroup;
    RMT_ASSERT(bytes_this_group <= 128);
    // XXX: Add (bytes_this_group > 0) to keep StaticAnalysis happy
    if ((bytes_this_group > 0) && (byte_offset < bytes_this_group)) {
      // We've found the group this byte is in - now which word
      // is it within the group, and which byte within that
      int word_within_group = byte_offset / bytes_per_word;
      RMT_ASSERT(word_within_group < kWordsPerGroup);
      int byte_within_word = byte_offset % bytes_per_word;
      RMT_ASSERT(byte_within_word < 4);
      int shift = byte_within_word * 8;
      uint32_t mask = ~(0xFFu << shift);
      uint32_t val = static_cast<uint32_t>(byte_value) << shift;
      int which_word = make_word_mapped(group, word_within_group);
      clobber(which_word, (get(which_word) & mask) | val);
      break;
    } else {
      byte_offset -= bytes_this_group;
      group++;
    }
  }
}
void Phv::set_packet(Packet *packet) {
  if (packet->is_egress())
    egress_.set_packet(packet);
  else
    ingress_.set_packet(packet);
}
void Phv::set_teop(Teop *teop) {
  teop_ = teop;
}
void Phv::set_cache_id() {
  if (pf_ != NULL) cache_id_.SetFrom(pf_->phv_cache_id());
}
void Phv::print(const char *s, bool force_log, uint32_t flags) const {
  const uint64_t log_flags = force_log ? RmtDebug::kRmtDebugForce
                                       : RmtDebug::verbose(RmtDebug::kRmtDebugPhv1);
  if (!rmt_log_check(log_flags)) return;

  RmtObjectManager *om = get_object_manager();
  const P4NameLookup *p4_name_lookup = nullptr;
  if (nullptr != om) p4_name_lookup = &(om->p4_name_lookup(pipe_index()));
  std::string phv_name = "Unknown";  // default name to log if P4NameLookup unavailable

  if (s != NULL) {
    RMT_LOG(log_flags, "%s\n", s);
  }
  int prev_group = 0;
  for (int i = 0; i < kWordsMaxExtended; i++) {
    int ii = chk_static(i, flags); // Map PHV index
    if (base_basic_check_word(ii)) {
      int group = base_which_group(ii);
      int word = base_which_word(ii);
      int width = base_which_width(ii);
      uint32_t val = base_get(ii);
      if (nullptr != p4_name_lookup) phv_name = p4_name_lookup->GetFieldName(s_index(), ii);
      if (group != prev_group) RMT_LOG(log_flags, "\n");
      RMT_LOG(log_flags,
              "%3d(mapped=%3d) [%2d,%2d]<%d> = %*u(0x%0*x) %s\n",
              i, ii, group, word, width,
              (width > 8) ? (width)/3 : 3, val,
              width/4, val,
              phv_name.c_str());
      if (nullptr != p4_name_lookup) {
        const P4PhvContainer *container = p4_name_lookup->GetPhvContainerSafe(s_index(), ii);
        if (nullptr != container) {
          for (const auto& it : container->GetPovHeaders()) {
            P4PovHeader *pov_header = it.second;
            int pov_bit_val = (val >> pov_header->GetBitIndex()) & 0x1;
            RMT_LOG(log_flags, "    %3d  POV bit %2d = %1d  %s\n",
                    i, pov_header->GetBitIndex(), pov_bit_val, pov_header->GetHeaderName().c_str());
          }
        }
      }
      prev_group = group;
    }
  }
}

void Phv::print_x(const char *s, bool force_log) const { print(s, force_log, kFlagsExtended); }
void Phv::print_p(const char *s, bool force_log) const { print(s, force_log, kFlagsParser);   }
void Phv::print_d(const char *s, bool force_log) const { print(s, force_log, kFlagsDeparserExtended); }
void Phv::print(const char *s, bool force_log)   const { print_x(s, force_log); }
void Phv::print(const char *s)                     const { print(s, false);   }
void Phv::print()                                  const { print(NULL);       }



// These funcs exclusively for use of MAU
// which is allowed to access all words up to kWordsMax with no mapping
//
int Phv::map_word_rel8_to_abs(int phv_word) {
  return make_word_mapped(RmtDefs::kPhv_Map_Rel8_Abs[chk_which_group(phv_word, kFlagsMau)],
                          chk_which_word(phv_word, kFlagsMau));
}
int Phv::map_word_rel16_to_abs(int phv_word) {
  return make_word_mapped(RmtDefs::kPhv_Map_Rel16_Abs[chk_which_group(phv_word, kFlagsMau)],
                          chk_which_word(phv_word, kFlagsMau));
}
int Phv::map_word_rel32_to_abs(int phv_word) {
  return make_word_mapped(RmtDefs::kPhv_Map_Rel32_Abs[chk_which_group(phv_word, kFlagsMau)],
                          chk_which_word(phv_word, kFlagsMau));
}
int Phv::map_group_rel8_to_abs(int group)  {
  return RmtDefs::kPhv_Map_Rel8_Abs[group];
}
int Phv::map_group_rel16_to_abs(int group) {
  return RmtDefs::kPhv_Map_Rel16_Abs[group];
}
int Phv::map_group_rel32_to_abs(int group) {
  return RmtDefs::kPhv_Map_Rel32_Abs[group];
}
int Phv::map_word_abs_to_rel8(int phv_word) {
  return make_word_mapped(RmtDefs::kPhv_Map_Abs_Rel8[chk_which_group(phv_word, kFlagsMau)],
                          chk_which_word(phv_word, kFlagsMau));
}
int Phv::map_word_abs_to_rel16(int phv_word) {
  return make_word_mapped(RmtDefs::kPhv_Map_Abs_Rel16[chk_which_group(phv_word, kFlagsMau)],
                          chk_which_word(phv_word, kFlagsMau));
}
int Phv::map_word_abs_to_rel32(int phv_word) {
  return make_word_mapped(RmtDefs::kPhv_Map_Abs_Rel32[chk_which_group(phv_word, kFlagsMau)],
                          chk_which_word(phv_word, kFlagsMau));
}
int Phv::map_group_abs_to_rel8(int group)  {
  return RmtDefs::kPhv_Map_Abs_Rel8[group];
}
int Phv::map_group_abs_to_rel16(int group) {
  return RmtDefs::kPhv_Map_Abs_Rel16[group];
}
int Phv::map_group_abs_to_rel32(int group) {
  return RmtDefs::kPhv_Map_Abs_Rel32[group];
}

int Phv::which_word(const int phv_word) {
  return chk_which_word(phv_word, kFlagsMau);
}
int Phv::which_group(const int phv_word) {
  return chk_which_group(phv_word, kFlagsMau);
}
int Phv::which_width(const int phv_word) {
  return chk_which_width(phv_word, kFlagsMau);
}
int Phv::which_width_in_bytes(const int phv_word) {
  return chk_which_width_in_bytes(phv_word, kFlagsMau);
}
int Phv::which_mod(const int phv_word) {
  return chk_which_mod(phv_word, kFlagsMau);
}
bool Phv::is_valid_norm_phv(const int phv_word) {
  return chk_is_valid_norm_phv(phv_word, kFlagsMau);
}
bool Phv::is_valid_taga_phv(const int phv_word) {
  return chk_is_valid_taga_phv(phv_word, kFlagsMau);
}
bool Phv::is_valid_phv(const int phv_word) {
  return chk_is_valid_phv(phv_word, kFlagsMau);
}

void Phv::set_written(const int phv_word, bool tf) {
  chk_set_written(phv_word, tf, kFlagsMau|kFlagsAssertOnAny);
}
void Phv::set_valid(const int phv_word, bool tf) {
  chk_set_valid(phv_word, tf, kFlagsMau|kFlagsAssertOnAny);
}
bool Phv::is_valid(const int phv_word) const {
  return chk_is_valid(phv_word, kFlagsMau);
}
bool Phv::was_written(const int phv_word) const {
  return chk_was_written(phv_word, kFlagsMau|kFlagsAssertOnAny);
}
void Phv::set_error(const int phv_word, bool tf) {
  chk_set_error(phv_word, tf, kFlagsMau|kFlagsAssertOnAny);
}
bool Phv::has_error(const int phv_word) const {
  return chk_has_error(phv_word, kFlagsMau|kFlagsAssertOnAny);
}
void Phv::set(const int phv_word, uint32_t val) {
  chk_set(phv_word, val, kFlagsMau|kFlagsAssertOnAny);
}
void Phv::clobber(const int phv_word, uint32_t val) {
  chk_clobber(phv_word, val, kFlagsMau|kFlagsAssertOnAny);
}
uint32_t Phv::get(const int phv_word) const {
  return chk_get(phv_word, kFlagsMau|kFlagsAssertOnAny);
}
uint32_t Phv::get_ignore_valid_bit(const int phv_word) const {
  return chk_get_ignore_valid_bit(phv_word, kFlagsMau|kFlagsAssertOnAny);
}
uint64_t Phv::get_including_valid_bit(const int phv_word) const {
  return chk_get_including_valid_bit(phv_word, kFlagsMau|kFlagsAssertOnAny);
}
uint8_t Phv::get_byte(const int phv_word, int which_byte) const {
  return chk_get_byte(phv_word, which_byte, kFlagsMau|kFlagsAssertOnAny);
}
uint32_t Phv::get_field(const int phv_word, int start_bit, int end_bit) const {
  return chk_get_field(phv_word, start_bit, end_bit, kFlagsMau|kFlagsAssertOnAny);
}



// These funcs used (almost) exclusively internally by phv.cpp
// for copying/cloning entire PHV. Also by unit-test logic
//
int Phv::make_word_x(int grp, int word) {
  return make_word_mapped(grp, word);
}
int Phv::which_word_x(const int phv_word) {
  return chk_which_word(phv_word, kFlagsExtended);
}
int Phv::which_group_x(const int phv_word) {
  return chk_which_group(phv_word, kFlagsExtended);
}
int Phv::which_width_x(const int phv_word) {
  return chk_which_width(phv_word, kFlagsExtended);
}
int Phv::which_width_in_bytes_x(const int phv_word) {
  return chk_which_width_in_bytes(phv_word, kFlagsExtended);
}
int Phv::which_mod_x(const int phv_word) {
  return chk_which_mod(phv_word, kFlagsExtended);
}
bool Phv::is_valid_norm_phv_x(const int phv_word) {
  return chk_is_valid_norm_phv(phv_word, kFlagsExtended);
}
bool Phv::is_valid_taga_phv_x(const int phv_word) {
  return chk_is_valid_taga_phv(phv_word, kFlagsExtended);
}
bool Phv::is_valid_phv_x(const int phv_word) {
  return chk_is_valid_phv(phv_word, kFlagsExtended);
}

void Phv::set_written_x(const int phv_word, bool tf) {
  chk_set_written(phv_word, tf, kFlagsExtended);
}
void Phv::set_valid_x(const int phv_word, bool tf) {
  chk_set_valid(phv_word, tf, kFlagsExtended);
}
bool Phv::is_valid_x(const int phv_word) const {
  return chk_is_valid(phv_word, kFlagsExtended);
}
bool Phv::was_written_x(const int phv_word) const {
  return chk_was_written(phv_word, kFlagsExtended);
}
void Phv::set_error_x(const int phv_word, bool tf) {
  chk_set_error(phv_word, tf, kFlagsExtended);
}
bool Phv::has_error_x(const int phv_word) const {
  return chk_has_error(phv_word, kFlagsExtended);
}
void Phv::set_x(const int phv_word, uint32_t val) {
  chk_set(phv_word, val, kFlagsExtended);
}
void Phv::clobber_x(const int phv_word, uint32_t val) {
  chk_clobber(phv_word, val, kFlagsExtended);
}
uint32_t Phv::get_x(const int phv_word) const {
  return chk_get(phv_word, kFlagsExtended);
}
uint32_t Phv::get_x_ignore_valid_bit(const int phv_word) const {
  return chk_get_ignore_valid_bit(phv_word, kFlagsExtended);
}
uint32_t Phv::get_ignore_valid_bit_x(const int phv_word) const {
  return chk_get_ignore_valid_bit(phv_word, kFlagsExtended);
}
uint64_t Phv::get_including_valid_bit_x(const int phv_word) const {
  return chk_get_including_valid_bit(phv_word, kFlagsExtended);
}
uint8_t Phv::get_byte_x(const int phv_word, int which_byte) const {
  return chk_get_byte(phv_word, which_byte, kFlagsExtended);
}
uint32_t Phv::get_field_x(const int phv_word, int start_bit, int end_bit) const {
  return chk_get_field(phv_word, start_bit, end_bit, kFlagsExtended);
}



// These funcs exclusively for use of PARSER
//
int Phv::phv_max_p() {
  int unmapped_max = kPhvWordsMaxUnmapped;
  int mapped_max = RmtDefs::map_prsr_phv_index(unmapped_max);
  RMT_ASSERT(mapped_max == kPhvWordsMax);
  return unmapped_max;
}
int Phv::phv_max_extended_p() {
  int unmapped_max = kPhvWordsMaxExtendedUnmapped;
  int mapped_max = RmtDefs::map_prsr_phv_index(unmapped_max);
  RMT_ASSERT(mapped_max == kPhvWordsMaxExtended);
  return unmapped_max;
}
int Phv::wrap_phv_p(const int phv_word, const int incr) {
  // Need to map phv_word so we get correct group/modulus
  int mapped_word = chk_static(phv_word, kFlagsParser);
  RMT_ASSERT(mapped_word >= 0);
  int grp = base_which_group(mapped_word);
  int mod = base_which_mod(mapped_word);
  // Wrap original phv_word
  int word = (mod > 0) ? (phv_word + incr) % mod : (phv_word + incr);
  int wrapped_word = make_word_unmapped(grp, word);
  // Recheck mapping still ok but return unmapped word
  RMT_ASSERT(chk_static(wrapped_word, kFlagsParser) >= 0);
  return wrapped_word;
}
int Phv::make_word_p(int grp, int word) {
  // Parser make_word creates unmapped words
  return make_word_unmapped(grp, word);
}
int Phv::which_word_p(const int phv_word) {
  return chk_which_word(phv_word, kFlagsParser);
}
int Phv::which_group_p(const int phv_word) {
  return chk_which_group(phv_word, kFlagsParser);
}
int Phv::which_width_p(const int phv_word) {
  return chk_which_width(phv_word, kFlagsParser);
}
int Phv::which_width_in_bytes_p(const int phv_word) {
  return chk_which_width_in_bytes(phv_word, kFlagsParser);
}
bool Phv::is_valid_norm_phv_p(const int phv_word) {
  return chk_is_valid_norm_phv(phv_word, kFlagsParser);
}
bool Phv::is_valid_taga_phv_p(const int phv_word) {
  return chk_is_valid_taga_phv(phv_word, kFlagsParser);
}
bool Phv::is_valid_phv_p(const int phv_word) {
  return chk_is_valid_phv(phv_word, kFlagsParser);
}

void Phv::set_valid_p(const int phv_word, bool tf) {
  return chk_set_valid(phv_word, tf, kFlagsParser);
}
bool Phv::is_valid_p(const int phv_word) const {
  return chk_is_valid(phv_word, kFlagsParser);
}
bool Phv::was_written_p(const int phv_word) const {
  return chk_was_written(phv_word, kFlagsParser|kFlagsAssertOnAny);
}
void Phv::set_p(const int phv_word, uint32_t val) {
  chk_set(phv_word, val, kFlagsParser|kFlagsAssertOnAny);
}
void Phv::clobber_p(const int phv_word, uint32_t val) {
  chk_clobber(phv_word, val, kFlagsParser|kFlagsAssertOnAny);
}
uint32_t Phv::get_p(const int phv_word) const {
  return chk_get(phv_word, kFlagsParser|kFlagsAssertOnAny);
}
uint32_t Phv::get_field_p(const int phv_word, int start_bit, int end_bit) const {
  // don't use chk() to avoid chk_source() because phv_word
  // from context.json uses parser addressing universally
  int mapped_word = chk_static(phv_word, kFlagsParser | kFlagsAssertOnAny);
  return (mapped_word >= 0) ? base_get_field(mapped_word, start_bit, end_bit) : 0u;
}



// These funcs exclusively for use of DEPARSER
// The _d variants typically check the phv_word < kWordsMax,
// however we allow the which_width funcs and is_valid_norm|taga
// funcs to see all phv_word < kWordsMaxExtended
//
int Phv::phv_max_d() {
  int unmapped_max = kPhvWordsMaxUnmapped;
  int mapped_max = RmtDefs::map_dprsr_phv_index(unmapped_max);
  RMT_ASSERT(mapped_max == kPhvWordsMax);
  return unmapped_max;
}
int Phv::phv_max_extended_d() {
  int unmapped_max = kPhvWordsMaxExtendedUnmapped;
  int mapped_max = RmtDefs::map_dprsr_phv_index(unmapped_max);
  RMT_ASSERT(mapped_max == kPhvWordsMaxExtended);
  return unmapped_max;
}
int Phv::make_word_d(int grp, int word) {
  // Deparser make_word creates unmapped words
  return make_word_unmapped(grp, word);
}
int Phv::which_word_d(const int phv_word) {
  return chk_which_word(phv_word, kFlagsDeparserExtended);
}
int Phv::which_group_d(const int phv_word) {
  return chk_which_group(phv_word, kFlagsDeparserExtended);
}
int Phv::which_width_d(const int phv_word) {
  return chk_which_width(phv_word, kFlagsDeparserExtended);
}
int Phv::which_width_in_bytes_d(const int phv_word) {
  return chk_which_width_in_bytes(phv_word, kFlagsDeparserExtended);
}
bool Phv::is_valid_norm_phv_d(const int phv_word) {
  return chk_is_valid_norm_phv(phv_word, kFlagsDeparserExtended);
}
bool Phv::is_valid_taga_phv_d(const int phv_word) {
  return chk_is_valid_taga_phv(phv_word, kFlagsDeparserExtended);
}
bool Phv::is_valid_phv_d(const int phv_word) {
  return chk_is_valid_phv(phv_word, kFlagsDeparserExtended);
}

bool Phv::is_valid_d(const int phv_word) const {
  return chk_is_valid(phv_word, kFlagsDeparser);
}
void Phv::set_d(const int phv_word, uint32_t val) {
  chk_set(phv_word, val, kFlagsDeparser|kFlagsAssertOnAny);
}
void Phv::clobber_d(const int phv_word, uint32_t val) {
  chk_clobber(phv_word, val, kFlagsDeparser|kFlagsAssertOnAny);
}
uint32_t Phv::get_d(const int phv_word) const {
  return chk_get(phv_word, kFlagsDeparser|kFlagsAssertOnAny);
}
uint32_t Phv::get_ignore_valid_bit_d(const int phv_word) const {
  return chk_get_ignore_valid_bit(phv_word, kFlagsDeparser|kFlagsAssertOnAny);
}



// These funcs also exclusively for use of DEPARSER
// (the _dx variants check the phv_word < kWordsMaxExtended)
//
int Phv::phv_max_dx() {
  int unmapped_max = kPhvWordsMaxUnmapped;
  int mapped_max = RmtDefs::map_prsr_phv_index(unmapped_max);
  RMT_ASSERT(mapped_max == kPhvWordsMax);
  return unmapped_max;
}
int Phv::phv_max_extended_dx() {
  int unmapped_max = kPhvWordsMaxExtendedUnmapped;
  int mapped_max = RmtDefs::map_dprsr_phv_index(unmapped_max);
  RMT_ASSERT(mapped_max == kPhvWordsMaxExtended);
  return unmapped_max;
}
int Phv::which_width_dx(const int phv_word) {
  return chk_which_width(phv_word, kFlagsDeparserExtended);
}
int Phv::which_width_in_bytes_dx(const int phv_word) {
  return chk_which_width_in_bytes(phv_word, kFlagsDeparserExtended);
}
bool Phv::is_valid_norm_phv_dx(const int phv_word) {
  return chk_is_valid_norm_phv(phv_word, kFlagsDeparserExtended);
}
bool Phv::is_valid_taga_phv_dx(const int phv_word) {
  return chk_is_valid_taga_phv(phv_word, kFlagsDeparserExtended);
}
bool Phv::is_valid_phv_dx(const int phv_word) {
  return chk_is_valid_phv(phv_word, kFlagsDeparserExtended);
}

bool Phv::is_valid_dx(const int phv_word) const {
  return chk_is_valid(phv_word, kFlagsDeparserExtended);
}
void Phv::set_dx(const int phv_word, uint32_t val) {
  chk_set(phv_word, val, kFlagsDeparserExtended|kFlagsAssertOnAny);
}
void Phv::clobber_dx(const int phv_word, uint32_t val) {
  chk_clobber(phv_word, val, kFlagsDeparserExtended|kFlagsAssertOnAny);
}
uint32_t Phv::get_dx(const int phv_word) const {
  return chk_get(phv_word, kFlagsDeparserExtended|kFlagsAssertOnAny);
}
uint32_t Phv::get_ignore_valid_bit_dx(const int phv_word) const {
  return chk_get_ignore_valid_bit(phv_word, kFlagsDeparserExtended|kFlagsAssertOnAny);
}


// Funcs to allow DV to get/set cached PhvPipeData
// This data can be used as a replacement for calculated data
//  at various points in MAU processing (eg MauInputXbar)
//
PhvPipeData *Phv::pipe_data() {
 if (pipe_data_ == nullptr) pipe_data_ = new PhvPipeData(pipe_index_); // Alloc on demand
 return pipe_data_;
}
void Phv::set_pipe_data(int mau, int what_data, int bit_offset, uint64_t data, int width) {
  pipe_data()->set_pipe_data(mau, what_data, bit_offset, data, width);
}
uint64_t Phv::get_pipe_data(int mau, int what_data, int bit_offset, int width) {
  if (pipe_data_ == nullptr) return UINT64_C(0);
  return pipe_data()->get_pipe_data(mau, what_data, bit_offset, width);
}
void Phv::set_pipe_data_ctrl(int mau, int what_data, uint8_t ctrl) {
  pipe_data()->set_pipe_data_ctrl(mau, what_data, ctrl);
}
uint8_t Phv::get_pipe_data_ctrl(int mau, int what_data) {
  if (pipe_data_ == nullptr) return PhvDataCtrl::kCalcOnly; // Default is calculate data
  return pipe_data()->get_pipe_data_ctrl(mau, what_data);
}
void Phv::free_pipe_data() {
  // DV should call this func once they are finished with a PHV
  if (pipe_data_ != nullptr) delete pipe_data_;
  pipe_data_ = nullptr;
}
bool Phv::has_pipe_data() {
  return (pipe_data_ != nullptr);
}

// Convenience func to store tcam_match_addr
void Phv::set_pipe_data_tcam_match_addr(int mau, int ltcam,
                                        uint32_t match_addr, uint8_t hit, uint8_t action_bit) {
  pipe_data()->set_pipe_data_tcam_match_addr(mau, ltcam, match_addr, hit, action_bit);
}
void Phv::get_pipe_data_tcam_match_addr(int mau, int ltcam,
                                        uint32_t *match_addr, uint8_t *hit, uint8_t *action_bit) {
  pipe_data()->get_pipe_data_tcam_match_addr(mau, ltcam, match_addr, hit, action_bit);
}

// Funcs to add a 'stuck-bit' feature to the model
// Use bitwise operations (xor, or, clear) to modify the phv
void Phv::bit_xor(Phv *xor_phv, bool verbose) {
  uint32_t oldval{}, newval{};
  for (unsigned i = 0; i < kWordsMaxExtended; i++) {
    oldval  = get_x(i);
    clobber_x(i, get_x(i) ^ xor_phv->get_x(i)); // Do the XOR operation for the index
    newval = get_x(i);
    if (verbose && (oldval != newval))
      RMT_LOG_VERBOSE("XOR[%u]: %x --> %x\n", i, oldval, newval);
  }
}
void Phv::bit_or(Phv *or_phv, bool verbose) {
  uint32_t oldval{}, newval{};
  for (unsigned i = 0; i < kWordsMaxExtended; i++) {
    oldval = get_x(i);
    set_x(i, or_phv->get_x(i));
    newval = get_x(i);
    if (verbose && (oldval != newval))
      RMT_LOG_VERBOSE("OR[%u]: %x --> %x\n", i, oldval, newval);
  }
}
void Phv::bit_clr(Phv *clr_phv, bool verbose) {
  uint32_t oldval{}, newval{};
  for (unsigned i = 0; i < kWordsMaxExtended; i++) {
    oldval  = get_x(i);
    clobber_x(i, get_x(i) & ~clr_phv->get_x(i)); // Do the CLR operation for the index
    newval = get_x(i);
    if (verbose && (oldval != newval))
      RMT_LOG_VERBOSE("CLR[%u]: %x --> %x\n", i, oldval, newval);
  }
}

}
