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

#ifndef _SHARED_PHV_
#define _SHARED_PHV_

#include <common/rmt-assert.h>
#include <string>
#include <cstdint>
#include <cache-id.h>
#include <rmt-defs.h>
#include <rmt-types.h>
#include <rmt-log.h>
#include <rmt-object.h>
#include <bitvector.h>
#include <time-info.h>
#include <common/rmt-util.h>


namespace MODEL_CHIP_NAMESPACE {

  class PhvFactory;
  class Packet;
  class Teop;
  class PhvPipeData;

  class PacketInfo {
    static constexpr uint8_t kFlagsIng   = 0x1;
    static constexpr uint8_t kFlagsEgr   = 0x2;
    static constexpr uint8_t kFlagsGhost = 0x4;
    static constexpr uint8_t kFlagsValid = 0x8;

 public:
    PacketInfo()  {};
    ~PacketInfo() { packet_ = NULL; flags_ = 0; }

    inline Packet  *packet()    const { return packet_;  }
    inline uint8_t  version()   const { return version_; }
    inline bool     ing()       const { return ((flags_ & kFlagsIng)   != 0);  }
    inline bool     egr()       const { return ((flags_ & kFlagsEgr)   != 0);  }
    inline bool     ghost()     const { return ((flags_ & kFlagsGhost) != 0);  }
    inline bool     valid()     const { return ((flags_ & kFlagsValid) != 0);  }
    inline bool     dflt_ing()  const { return valid() && !ghost();            }
    inline bool     dflt_egr()  const { return valid();                        }
    inline bool     ingress()   const { return ing() || dflt_ing();            }
    inline bool     egress()    const { return egr() || dflt_egr();            }
    inline void set_ingress() { RMT_ASSERT(!egr()); flags_ |= (kFlagsIng  |kFlagsValid); }
    inline void set_egress()  { RMT_ASSERT(!ing()); flags_ |= (kFlagsEgr  |kFlagsValid); }
    inline void set_ghost()   { RMT_ASSERT(!egr()); flags_ |= (kFlagsGhost|kFlagsValid); }
    inline void set_valid()   {                     flags_ |=              kFlagsValid;  }
    inline void set_packet(Packet *packet) {
      if (packet == NULL) return;
      packet_ = packet;
      set_valid();
    }
    inline void set_version(uint8_t version) {
      version_ = version;
      set_valid();
    }
    inline void set_pktlen(uint16_t pktlen) {
      set_valid(); // Ignored
    }
    inline void set_eopnum(uint8_t eopnum) {
      eopnum_ = eopnum;
      set_valid();
    }
    inline void set_pkterr(uint32_t pkterr) {
      pkterr_ |= pkterr;
      set_valid();
    }
    inline uint16_t pktlen() const { return pktlen_; }
    inline uint8_t  eopnum() const { return eopnum_; }
    inline uint32_t pkterr() const { return pkterr_; }

    TimeInfoType time_info_ {};

 private:
    Packet  *packet_ = NULL;
    uint16_t pktlen_ = 0;
    uint8_t  eopnum_ = 0xFF;
    uint8_t  version_ = 0;
    uint32_t pkterr_ = 0u;
    uint8_t  flags_ = 0;
  };


  class Phv : private RmtObject, private RmtLogger {

 public:
    static constexpr uint32_t kFlagsSourceMau           = 0x00001u;
    static constexpr uint32_t kFlagsSourceParser        = 0x00002u;
    static constexpr uint32_t kFlagsSourceDeparser      = 0x00003u;
    static constexpr uint32_t kFlagsSourceMask          = 0x000FFu;
    static constexpr uint32_t kFlagsChkSource           = 0x00100u;
    static constexpr uint32_t kFlagsDoMap               = 0x00200u;
    static constexpr uint32_t kFlagsChkInRangeNorm      = 0x00400u;
    static constexpr uint32_t kFlagsChkInRangeTaga      = 0x00800u;
    static constexpr uint32_t kFlagsChkInRangeAny       = kFlagsChkInRangeNorm | kFlagsChkInRangeTaga;
    static constexpr uint32_t kFlagsAssertOnRangeError  = 0x01000u;
    static constexpr uint32_t kFlagsAssertOnSourceError = 0x02000u;
    static constexpr uint32_t kFlagsAssertOnAny         = kFlagsAssertOnRangeError | kFlagsAssertOnSourceError;
    static constexpr uint32_t kFlagsDefault             = kFlagsChkSource | kFlagsDoMap | kFlagsAssertOnSourceError;
    static constexpr uint32_t kFlagsMau                 = kFlagsSourceMau | kFlagsDefault | kFlagsChkInRangeNorm;
    static constexpr uint32_t kFlagsParser              = kFlagsSourceParser | kFlagsDefault | kFlagsChkInRangeAny;
    static constexpr uint32_t kFlagsDeparser            = kFlagsSourceDeparser | kFlagsDefault | kFlagsChkInRangeNorm;
    static constexpr uint32_t kFlagsDeparserExtended    = kFlagsSourceDeparser | kFlagsDefault | kFlagsChkInRangeAny;
    static constexpr uint32_t kFlagsExtended            = kFlagsChkInRangeAny;
    static constexpr uint32_t kFlagsMatchOnly           = 0x10000u;


 private:
    static constexpr int  kPhvWordsMax                 = RmtDefs::kPhvWordsMax;
    static constexpr int  kPhvWordsMaxExtended         = RmtDefs::kPhvWordsMaxExtended;
    static constexpr int  kPhvWordsMaxUnmapped         = RmtDefs::kPhvWordsMaxUnmapped;
    static constexpr int  kPhvWordsMaxExtendedUnmapped = RmtDefs::kPhvWordsMaxExtendedUnmapped;
    static constexpr int  kPhvWordsPerGroup            = RmtDefs::kPhvWordsPerGroup;
    static constexpr int  kPhvWordsPerGroupUnmapped    = RmtDefs::kPhvWordsPerGroupUnmapped;
    static constexpr int  kGroups =
        (kPhvWordsMax + kPhvWordsPerGroup - 1) / kPhvWordsPerGroup;
    static constexpr int  kGroupsExtended =
        (kPhvWordsMaxExtended + kPhvWordsPerGroup - 1) / kPhvWordsPerGroup;


    // Fundamental accessors - private - limited checking.
    // These assume access has been checked and mapped
    //
    static inline bool base_basic_check_word(int phv_word) {
      return ((phv_word >= 0) && (phv_word < kWordsMaxExtended));
    }
    static inline int base_which_word(int phv_word) {
      RMT_ASSERT(base_basic_check_word(phv_word));
      return phv_word % kPhvWordsPerGroup;
    }
    static inline int base_which_group(int phv_word) {
      RMT_ASSERT(base_basic_check_word(phv_word));
      return phv_word / kPhvWordsPerGroup;
    }
    static inline uint32_t base_which_mask(const int phv_word) {
      if (!base_basic_check_word(phv_word)) return 0u;
      return RmtDefs::kPhv_MaskPerGroup[base_which_group(phv_word)];
    }
    static inline int base_which_width(const int phv_word) {
      if (!base_basic_check_word(phv_word)) return 0;
      return RmtDefs::kPhv_WidthPerGroup[base_which_group(phv_word)];
    }
    static inline int base_which_width_in_bytes(const int phv_word) {
      return base_which_width(phv_word) / 8;
    }
    static inline int base_which_mod(const int phv_word) {
      if (!base_basic_check_word(phv_word)) return 0;
      return RmtDefs::kPhv_ModulusPerGroup[base_which_group(phv_word)];
    }
    static inline bool base_is_valid_norm_phv(const int phv_word) {
      return ((phv_word >= 0) && (phv_word < kWordsMax) &&
              (base_which_mask(phv_word) != 0u));
    }
    static inline bool base_is_valid_taga_phv(const int phv_word) {
      return ((phv_word >= kWordsMax) && (phv_word < kWordsMaxExtended) &&
              (base_which_mask(phv_word) != 0u));
    }
    static inline bool base_is_valid_phv(const int phv_word) {
      return base_is_valid_norm_phv(phv_word) || base_is_valid_taga_phv(phv_word);
    }

    inline void base_set_written(const int phv_word, bool tf) {
      if (!base_is_valid_phv(phv_word)) return;
      if (written_ != NULL) written_->set_bit(tf, phv_word);
    }
    inline void base_set_valid(const int phv_word, bool tf) {
      if (!base_is_valid_phv(phv_word)) return;
      valid_.set_bit(tf, phv_word);
    }
    inline bool base_is_valid(const int phv_word) const {
      if (!base_is_valid_phv(phv_word)) return false;
      return valid_.bit_set(phv_word);
    }
    inline bool base_was_written(const int phv_word) const {
      // Exploit written BV if one avail else fallback to base_is_valid()
      if (!base_is_valid_phv(phv_word)) return false;
      if (written_ != NULL) return written_->bit_set(phv_word);
      return base_is_valid(phv_word);
    }
    inline void base_set_error(const int phv_word, bool tf) {
      if (!base_is_valid_phv(phv_word)) return;
      if (error_ == NULL) error_.reset( new BitVector<kWordsMaxExtended>(UINT64_C(0)) );
      error_->set_bit(tf, phv_word);
      if (tf) base_set_valid(phv_word, false); // Invalidate on error
    }
    inline bool base_has_error(const int phv_word) const {
      if ((!base_is_valid_phv(phv_word)) || (error_ == NULL)) return false;
      return error_->bit_set(phv_word);
    }
    inline void base_set(const int phv_word, uint32_t val) {
      RMT_ASSERT(base_is_valid_phv(phv_word));
      if (!base_is_valid(phv_word)) words_[phv_word] = 0u;
      words_[phv_word] |= val & base_which_mask(phv_word);
      base_set_valid(phv_word, true);
      base_set_written(phv_word, true);
    }
    inline void base_clobber(const int phv_word, uint32_t val) {
      RMT_ASSERT(base_is_valid_phv(phv_word));
      words_[phv_word] = 0u;
      base_set(phv_word, val);
    }
    inline uint32_t base_get_ignore_valid_bit(const int phv_word) const {
      RMT_ASSERT(base_is_valid_phv(phv_word));
      return words_[phv_word] & base_which_mask(phv_word);
    }
    inline uint32_t base_get(const int phv_word) const {
      RMT_ASSERT(base_is_valid_phv(phv_word));
      if (!base_is_valid(phv_word)) return 0UL;
      return base_get_ignore_valid_bit(phv_word);
    }
    inline uint64_t base_get_including_valid_bit(const int phv_word) const {
      uint64_t w = static_cast<uint64_t>(base_get_ignore_valid_bit(phv_word));
      uint64_t v = (base_is_valid(phv_word)) ?UINT64_C(1) :UINT64_C(0);
      return (v << base_which_width(phv_word)) | w;
    }
    inline uint8_t base_get_byte(const int phv_word, int which_byte) const {
      RMT_ASSERT(base_is_valid_phv(phv_word));
      if (!base_is_valid(phv_word)) return 0;
      which_byte %= base_which_width_in_bytes(phv_word);
      return (uint8_t)((words_[phv_word] >> (which_byte * 8)) & 0xFF);
    }
    inline uint32_t base_get_field(const int phv_word, int start_bit, int end_bit) const {
      uint32_t val = base_get(phv_word);
      if (end_bit < 31) val &= (uint32_t)((1 << (end_bit+1)) - 1);
      val >>= start_bit;
      return val;
    }



    // Checked accessors - private - more extensive checking.
    // We check source, map index, check range, then call base_ funcs above
    // Exact checks applied depend on flags
    //
    static inline int do_map(const int phv_word, uint32_t flags) {
      if ((flags & kFlagsDoMap) == 0u) return phv_word;
      // Call PER-CHIP map function - specialized in RmtDefs
      switch (flags & kFlagsSourceMask) {
        case kFlagsSourceMau:      return RmtDefs::map_mau_phv_index(phv_word);
        case kFlagsSourceParser:   return RmtDefs::map_prsr_phv_index(phv_word);
        case kFlagsSourceDeparser: return RmtDefs::map_dprsr_phv_index(phv_word);
        default:                   return phv_word;
      }
    }
    static inline bool chk_range(const int phv_word, uint32_t flags) {
      if (((flags & kFlagsChkInRangeNorm) != 0u) &&
          (base_is_valid_norm_phv(phv_word))) return true;
      if (((flags & kFlagsChkInRangeTaga) != 0u) &&
          (base_is_valid_taga_phv(phv_word))) return true;
      return false;
    }
    static inline int chk_static(const int phv_word, uint32_t flags) {
      int mapped_word = -1;
      if (base_basic_check_word(phv_word)) {
        mapped_word = do_map(phv_word, flags);
        if (!chk_range(mapped_word, flags)) mapped_word = -1;
      }
      if ((flags & kFlagsAssertOnRangeError) != 0u) RMT_ASSERT(mapped_word >= 0);
      return mapped_word;
    }
    static inline int chk_which_word(const int phv_word, uint32_t flags) {
      return base_which_word(chk_static(phv_word, flags));
    }
    static inline int chk_which_group(const int phv_word, uint32_t flags) {
      return base_which_group(chk_static(phv_word, flags));
    }
    static inline int chk_which_mask(const int phv_word, uint32_t flags) {
      return base_which_mask(chk_static(phv_word, flags));
    }
    static inline int chk_which_mod(const int phv_word, uint32_t flags) {
      return base_which_mod(chk_static(phv_word, flags));
    }
    static inline int chk_which_width(const int phv_word, uint32_t flags) {
      return base_which_width(chk_static(phv_word, flags));
    }
    static inline int chk_which_width_in_bytes(const int phv_word, uint32_t flags) {
      return base_which_width_in_bytes(chk_static(phv_word, flags));
    }
    static inline bool chk_is_valid_norm_phv(const int phv_word, uint32_t flags) {
      return base_is_valid_norm_phv(chk_static(phv_word, flags));
    }
    static inline bool chk_is_valid_taga_phv(const int phv_word, uint32_t flags) {
      return base_is_valid_taga_phv(chk_static(phv_word, flags));
    }
    static inline bool chk_is_valid_phv(const int phv_word, uint32_t flags) {
      return base_is_valid_phv(chk_static(phv_word, flags));
    }

    inline bool chk_source(uint32_t flags) const {
      uint8_t src_flags = static_cast<uint8_t>(flags & kFlagsSourceMask);
      uint8_t src_phv   = source();
      bool src_ok = ((src_flags == 0) || (src_phv == 0) || (src_phv == src_flags));
      if ((flags & kFlagsAssertOnSourceError) != 0u) RMT_ASSERT(src_ok);
      return src_ok;
    }
    inline int chk(const int phv_word, uint32_t flags) const {
      int mapped_word = (chk_source(flags)) ?chk_static(phv_word, flags) :-1;
      return mapped_word;
    }
    inline void chk_set_written(const int phv_word, bool tf, uint32_t flags) {
      int mapped_word = chk(phv_word, flags);
      if (mapped_word >= 0) base_set_written(mapped_word, tf);
    }
    inline void chk_set_valid(const int phv_word, bool tf, uint32_t flags) {
      int mapped_word = chk(phv_word, flags);
      if (mapped_word >= 0) base_set_valid(mapped_word, tf);
    }
    inline bool chk_is_valid(const int phv_word, uint32_t flags) const {
      int mapped_word = chk(phv_word, flags);
      return (mapped_word >= 0) ?base_is_valid(mapped_word) :false;
    }
    inline bool chk_was_written(const int phv_word, uint32_t flags) const {
      int mapped_word = chk(phv_word, flags);
      return (mapped_word >= 0) ?base_was_written(mapped_word) :false;
    }
    inline void chk_set_error(const int phv_word, bool tf, uint32_t flags) {
      int mapped_word = chk(phv_word, flags);
      if (mapped_word >= 0) base_set_error(mapped_word, tf);
    }
    inline bool chk_has_error(const int phv_word, uint32_t flags) const {
      int mapped_word = chk(phv_word, flags);
      return (mapped_word >= 0) ?base_has_error(mapped_word) :false;
    }
    inline void chk_set(const int phv_word, uint32_t val, uint32_t flags) {
      int mapped_word = chk(phv_word, flags);
      if (mapped_word >= 0) base_set(mapped_word, val);
    }
    inline void chk_clobber(const int phv_word, uint32_t val, uint32_t flags) {
      int mapped_word = chk(phv_word, flags);
      if (mapped_word >= 0) base_clobber(mapped_word, val);
    }
    inline uint32_t chk_get_ignore_valid_bit(const int phv_word, uint32_t flags) const {
      int mapped_word = chk(phv_word, flags);
      return (mapped_word >= 0) ?base_get_ignore_valid_bit(mapped_word) :0u;
    }
    inline uint32_t chk_get(const int phv_word, uint32_t flags) const {
      int mapped_word = chk(phv_word, flags);
      return (mapped_word >= 0) ?base_get(mapped_word) :0u;
    }
    inline uint64_t chk_get_including_valid_bit(const int phv_word, uint32_t flags) const {
      int mapped_word = chk(phv_word, flags);
      return (mapped_word >= 0) ?base_get_including_valid_bit(mapped_word) :UINT64_C(0);
    }
    inline uint8_t chk_get_byte(const int phv_word, int which_byte, uint32_t flags) const {
      int mapped_word = chk(phv_word, flags);
      return (mapped_word >= 0) ?base_get_byte(mapped_word, which_byte) :0;
    }
    inline uint32_t chk_get_field(const int phv_word,
                                  int start_bit, int end_bit, uint32_t flags) const {
      int mapped_word = chk(phv_word, flags);
      return (mapped_word >= 0) ?base_get_field(mapped_word, start_bit, end_bit) :0u;
    }

    inline int s_index()    const { return 0; }
    inline int rt_index()   const { return 0; }
    inline int c_index()    const { return 0; }



 public:
    inline int pipe_index() const { return pipe_index_; }

    // We allocate space in this structure for kWordsMaxExtended words
    // (368) - this is NOT kGroupsExtended*kWordsPerGroup (12*32 = 384)
    // as currently the final group is size 16 not size 32 - doh!
    //
    // MAUs can ONLY access kWordsMax words
    //
    // Parsers/Deparsers can access kWordsMaxExtended words using special
    // funcs is_valid_x, get_x, set_x  etc.
    //
    static constexpr int  kWordsPerGroup    = kPhvWordsPerGroup;
    static constexpr int  kWordsMax         = kPhvWordsMax;
    static constexpr int  kWordsMaxExtended = kPhvWordsMaxExtended;
    static constexpr int  kWordsMaxUnmapped = kPhvWordsMaxUnmapped;
    static constexpr int  kGroupsMax        = kGroups;
    //static constexpr int  kWordsExtra = RmtDefs::kPhvWordsExtra;
    static_assert( (kWordsMaxExtended >= kWordsMax),
                   "Insufficient words in PHV" );

    // COMMON funcs
    static inline int make_word_mapped(int grp, int word) {
      RMT_ASSERT((grp >= 0) && (grp < kGroupsExtended));
      RMT_ASSERT((word >= 0) && (word < kPhvWordsPerGroup));
      return (grp * kPhvWordsPerGroup) + word;
    }
    // These all use *unmapped* size (so 32 not 40)
    static inline constexpr int make_word_const(int grp, int word) {
      return (grp * kPhvWordsPerGroupUnmapped) + word;
    }
    static inline int make_word_nocheck(int grp, int word) {
      return make_word_const(grp, word);
    }
    static inline int make_word_unmapped(int grp, int word) {
      RMT_ASSERT((grp >= 0) && (grp < kGroupsExtended));
      RMT_ASSERT((word >= 0) && (word < kPhvWordsPerGroupUnmapped));
      return make_word_nocheck(grp, word);
    }
    // Leave make_word in for the minute as so many things
    // use it - they all assume make_word is make_word_unmapped
    static inline int make_word(int grp, int word) {
      return make_word_unmapped(grp, word);
    }


    // These funcs exclusively for use of MAU
    // which is allowed to access all words up to kWordsMax
    // with no mapping
    //
    static int map_word_rel8_to_abs(int phv_word);
    static int map_word_rel16_to_abs(int phv_word);
    static int map_word_rel32_to_abs(int phv_word);
    static int map_group_rel8_to_abs(int group) ;
    static int map_group_rel16_to_abs(int group);
    static int map_group_rel32_to_abs(int group);
    static int map_word_abs_to_rel8(int phv_word);
    static int map_word_abs_to_rel16(int phv_word);
    static int map_word_abs_to_rel32(int phv_word);
    static int map_group_abs_to_rel8(int group) ;
    static int map_group_abs_to_rel16(int group);
    static int map_group_abs_to_rel32(int group);

    static int which_word(const int phv_word);
    static int which_group(const int phv_word);
    static int which_width(const int phv_word);
    static int which_width_in_bytes(const int phv_word);
    static int which_mod(const int phv_word);
    static bool is_valid_norm_phv(const int phv_word);
    static bool is_valid_taga_phv(const int phv_word);
    static bool is_valid_phv(const int phv_word);

    void set_written(const int phv_word, bool tf=true);
    void set_valid(const int phv_word, bool tf=true);
    bool is_valid(const int phv_word) const;
    bool was_written(const int phv_word) const;
    void set_error(const int phv_word, bool tf=true);
    bool has_error(const int phv_word) const;
    void set(const int phv_word, uint32_t val);
    void clobber(const int phv_word, uint32_t val);
    uint32_t get(const int phv_word) const;
    uint32_t get_ignore_valid_bit(const int phv_word) const;
    uint64_t get_including_valid_bit(const int phv_word) const;
    uint8_t get_byte(const int phv_word, int which_byte) const;
    uint32_t get_field(const int phv_word, int start_bit, int end_bit) const;



    // These funcs allow extended access to the Phv with NO mapping
    // (so ALL Phv words 0 - kWordsMaxExtended-1)
    // These days should only be used internally by phv.cpp and
    // by unit-test code
    //
    static int make_word_x(int grp, int word);
    static int which_word_x(const int phv_word);
    static int which_group_x(const int phv_word);
    static int which_width_x(const int phv_word);
    static int which_width_in_bytes_x(const int phv_word);
    static int which_mod_x(const int phv_word);
    static bool is_valid_norm_phv_x(const int phv_word);
    static bool is_valid_taga_phv_x(const int phv_word);
    static bool is_valid_phv_x(const int phv_word);
    void set_written_x(const int phv_word, bool tf=true);
    void set_valid_x(const int phv_word, bool tf=true);
    bool is_valid_x(const int phv_word) const;
    bool was_written_x(const int phv_word) const;
    void set_error_x(const int phv_word, bool tf=true);
    bool has_error_x(const int phv_word) const;
    void set_x(const int phv_word, uint32_t val);
    void clobber_x(const int phv_word, uint32_t val);
    uint32_t get_x(const int phv_word) const;
    uint32_t get_x_ignore_valid_bit(const int phv_word) const;
    uint32_t get_ignore_valid_bit_x(const int phv_word) const;
    uint64_t get_including_valid_bit_x(const int phv_word) const;
    uint8_t get_byte_x(const int phv_word, int which_byte) const;
    uint32_t get_field_x(const int phv_word, int start_bit, int end_bit) const;


    // These funcs also exclusively for use of PARSER
    // (replacements for _x funcs)
    //
    static int phv_max_p();
    static int phv_max_extended_p();
    static int wrap_phv_p(const int phv_word, const int incr);
    static int make_word_p(int grp, int word);
    static int which_word_p(const int phv_word);
    static int which_group_p(const int phv_word);
    static int which_width_p(const int phv_word);
    static int which_width_in_bytes_p(const int phv_word);
    static bool is_valid_norm_phv_p(const int phv_word);
    static bool is_valid_taga_phv_p(const int phv_word);
    static bool is_valid_phv_p(const int phv_word);
    bool is_valid_p(const int phv_word) const;
    void set_valid_p(const int phv_word, bool tf=true);
    bool was_written_p(const int phv_word) const;
    void set_p(const int phv_word, uint32_t val);
    void clobber_p(const int phv_word, uint32_t val);
    uint32_t get_p(const int phv_word) const;
    uint32_t get_field_p(const int phv_word, int start_bit, int end_bit) const;

    // These only used by JBay Parser to map PHV words to 256x16b offset and vice-versa
    static void phv_index_to_off16_p(int phvWord, int *size, int *off16A, int *off16B, int *sz8_01);
    static void phv_off16_to_index_p(int off16, int *size, int *phvWordA, int *phvWordB, int *sz32_01);


    // These funcs exclusively for use of DEPARSER
    //
    static int phv_max_d();
    static int phv_max_extended_d();
    static int make_word_d(int grp, int word);
    static int which_word_d(const int phv_word);
    static int which_group_d(const int phv_word);
    static int which_width_d(const int phv_word);
    static int which_width_in_bytes_d(const int phv_word);
    static bool is_valid_norm_phv_d(const int phv_word);
    static bool is_valid_taga_phv_d(const int phv_word);
    static bool is_valid_phv_d(const int phv_word);
    bool is_valid_d(const int phv_word) const;
    void set_d(const int phv_word, uint32_t val);
    void clobber_d(const int phv_word, uint32_t val);
    uint32_t get_d(const int phv_word) const;
    uint32_t get_ignore_valid_bit_d(const int phv_word) const;

    // These funcs also exclusively for use of DEPARSER
    // but for the cases where the Deparser needs to
    // access words up to kWordsMaxExtended
    //
    static int phv_max_dx();
    static int phv_max_extended_dx();
    static int which_width_dx(const int phv_word);
    static int which_width_in_bytes_dx(const int phv_word);
    static bool is_valid_norm_phv_dx(const int phv_word);
    static bool is_valid_taga_phv_dx(const int phv_word);
    static bool is_valid_phv_dx(const int phv_word);
    bool is_valid_dx(const int phv_word) const;
    void set_dx(const int phv_word, uint32_t val);
    void clobber_dx(const int phv_word, uint32_t val);
    uint32_t get_dx(const int phv_word) const;
    uint32_t get_ignore_valid_bit_dx(const int phv_word) const;


    // OTHER funcs
    inline uint8_t     source()          const { return source_; }
    inline PhvFactory *pf()                    { return pf_; }
    inline Teop       *teop()                  { return teop_; }
    inline CacheId&    cache_id()              { return cache_id_; }
    inline bool        match_only()            { return ((flags_ & kFlagsMatchOnly) != 0u); }
    inline bool        egress()          const { return egress_.egress(); }
    inline Packet     *egress_packet()   const { return egress_.packet(); }
    inline uint8_t     egress_version()  const { return egress_.version(); }
    inline uint8_t     egress_eopnum()         { return egress_.eopnum(); }
    inline uint16_t    egress_pktlen()         { return egress_.pktlen(); }
    inline uint32_t    egress_pkterr()   const { return egress_.pkterr(); }
    inline bool        ingress()         const { return ingress_.ingress(); }
    inline bool        ghost()           const { return ingress_.ghost(); }
    inline bool        ingress_ghost()   const { return ingress() || ghost(); }
    inline bool        ieg()             const { return ingress() || egress() || ghost(); }
    inline Packet     *ingress_packet()  const { return ingress_.packet(); }
    inline uint8_t     ingress_version() const { return ingress_.version(); }
    inline uint8_t     ingress_eopnum()        { return ingress_.eopnum(); }
    inline uint16_t    ingress_pktlen()        { return ingress_.pktlen(); }
    inline uint32_t    ingress_pkterr() const  { return ingress_.pkterr(); }
    // Default to ingress packet/ingress version
    inline Packet     *packet()                { return ingress_packet(); }
    inline uint8_t     version()         const { return ingress_version(); }
    inline void        set_source(uint8_t s)   { source_ = s; }
    inline void        set_egress()            { egress_.set_egress(); }
    inline void        set_ingress()           { ingress_.set_ingress(); }
    inline void        set_ghost()             { ingress_.set_ghost(); }
    inline void set_countable(bool countable, bool ingress=true) {
      // TODO: Remove once DV stop using
      printf("\n\nCall to set_countable will be REMOVED - use set_match_only\n\n");
    }
    inline void set_match_only(bool match_only) {
      if (match_only) flags_ |= kFlagsMatchOnly; else flags_ &= ~kFlagsMatchOnly;
    }
    inline void set_version(uint8_t version, bool ingress_version=true) {
      if (ingress_version) ingress_.set_version(version);
      else                 egress_.set_version(version);
    }
    inline void set_pktlen(uint16_t pktlen, bool ingress_pktlen=true) {
      if (ingress_pktlen) ingress_.set_pktlen(pktlen);
      else                egress_.set_pktlen(pktlen);
    }
    inline void set_eopnum(uint8_t eopnum, bool ingress_eopnum=true) {
      if (ingress_eopnum) ingress_.set_eopnum(eopnum);
      else                egress_.set_eopnum(eopnum);
    }
    inline void set_pkterr(uint32_t pkterr, bool ingress_pkterr=true) {
      if (ingress_pkterr) ingress_.set_pkterr(pkterr);
      else                egress_.set_pkterr(pkterr);
    }

    inline BitVector<kWordsMaxExtended>  *written_bv()  { return written_.get(); }
    inline void start_recording_written() {
      written_.reset( new BitVector<kWordsMaxExtended>(UINT64_C(0)) );
    }
    inline void destroy() {
      error_.reset( NULL );
      written_.reset( NULL );
    }


    Phv(RmtObjectManager *om, PhvFactory *pf);
    Phv(RmtObjectManager *om);
    ~Phv();

    void set_all_valid();
    Phv *clone(bool copydata=false);
    Phv *clone_x(bool copydata=false);
    void copyinfo(Phv *phv, bool ingress_info);
    void copydata(Phv *phv, BitVector<kWordsMax> *selector);
    void copydata_x(Phv *phv, BitVector<kWordsMaxExtended> *selector);
    void merge_phvs(Phv *iphv, Phv *ephv);
    uint32_t hash();
    void maskdata(BitVector<kWordsMax> *selector);
    bool equals(Phv *other);
    bool identical(Phv *other);
    bool indistinguishable(Phv *other);

    void set_phv_by_byte(int byte, uint8_t byte_value);
    void set_packet(Packet *packet);
    void set_teop(Teop *teop);
    void set_cache_id();

    void print(const char *s, bool force_log, uint32_t flags) const;
    void print_x(const char *s, bool force_log) const;
    void print_p(const char *s, bool force_log) const;
    void print_d(const char *s, bool force_log) const;
    void print(const char *s, bool force_log) const;
    void print(const char *s) const;
    void print() const;

    // access to time_info
    // use set_mau_tick_time and set_mau_random_value to set
    //  specific values to match the RTL simulation
    // Note: these functions set both ingress and egress words to the same value, this is
    //  usually what you want for one phv. However when the match_phv is made from
    //  the iphv and the ophv it might end up with different values for time for
    //  ingress and egress.
    void set_meter_tick_time(int mau_index,int meter_index,uint64_t tick_time) {
      ingress_.time_info_.set_meter_tick_time(mau_index,meter_index,tick_time);
      egress_.time_info_.set_meter_tick_time(mau_index,meter_index,tick_time);
    }
    void set_meter_random_value(int mau_index,int meter_index,uint64_t random_value) {
      ingress_.time_info_.set_meter_random_value(mau_index,meter_index,random_value);
      egress_.time_info_.set_meter_random_value(mau_index,meter_index,random_value);
    }
    void set_immediate_data_random_value(int mau_index,uint64_t random_value) {
      ingress_.time_info_.set_immediate_data_random_value(mau_index,random_value);
      egress_.time_info_.set_immediate_data_random_value(mau_index,random_value);
    }
    // use this to initialise to plausible values, first_tick_time is
    //  used as the tick time for the first mau and the seed for the
    //  random numbers (it doesn't bother to do the random accurately)
    void setup_time_info(uint64_t first_tick_time) {
      ingress_.time_info_.set_all(first_tick_time);
      egress_.time_info_.set_all(first_tick_time);
    }

    uint64_t get_meter_tick_time(int mau_index, int meter_index, bool ingress) {
      return ingress ? ingress_.time_info_.get_meter_tick_time(mau_index,meter_index) :
          egress_.time_info_.get_meter_tick_time(mau_index,meter_index);
    }
    uint64_t get_meter_random_value(int mau_index, int meter_index, bool ingress) {
      return ingress ? ingress_.time_info_.get_meter_random_value(mau_index,meter_index) :
          egress_.time_info_.get_meter_random_value(mau_index,meter_index);

    }
    uint64_t get_immediate_data_random_value(int mau_index,bool ingress) {
      return ingress ? ingress_.time_info_.get_immediate_data_random_value(mau_index) :
          egress_.time_info_.get_immediate_data_random_value(mau_index);
    }
    void set_time_info_from(const TimeInfoType& other) {
      ingress_.time_info_.set_from(other);
      egress_.time_info_.set_from(other);
    }
    const TimeInfoType &get_time_info(bool ingress) const {
      return ingress ? ingress_.time_info_ : egress_.time_info_;
    }
    bool is_before(TimeInfoType& other, bool ingress) const {
      return ingress ? ingress_.time_info_.is_before(other) : egress_.time_info_.is_before(other);
    }


    void set_relative_time(uint64_t relative_time) {
      ingress_.time_info_.set_relative_time(relative_time);
      egress_.time_info_.set_relative_time(relative_time);
    }
    bool relative_time_valid(bool ingress)   const {
      return ingress ? ingress_.time_info_.relative_time_valid() :
           egress_.time_info_.relative_time_valid();
    }
    uint64_t get_relative_time(bool ingress) const {
      return ingress ? ingress_.time_info_.get_relative_time() :
          egress_.time_info_.get_relative_time();
    }

    // Some DV specific funcs
    inline uint8_t thread_active() {
      return ((ingress() ?0x1 :0x0) | (egress() ?0x2 :0x0));
    }

    inline void set_pipe(int pipe_index) {
      pipe_index_ = pipe_index;
      refresh_log_flags();
    }

    // Funcs to allow DV to get/set cached PhvPipeData
    PhvPipeData *pipe_data();
    void     set_pipe_data(int mau, int what_data, int bit_offset, uint64_t data, int width);
    uint64_t get_pipe_data(int mau, int what_data, int bit_offset, int width);
    void     set_pipe_data_ctrl(int mau, int what_data, uint8_t ctrl);
    uint8_t  get_pipe_data_ctrl(int mau, int what_data);
    void     free_pipe_data();
    bool     has_pipe_data();
    void     set_pipe_data_tcam_match_addr(int mau, int ltcam,
                                           uint32_t match_addr, uint8_t hit, uint8_t action_bit);
    void     get_pipe_data_tcam_match_addr(int mau, int ltcam,
                                           uint32_t *match_addr, uint8_t *hit, uint8_t *action_bit);

    // Funcs to add a 'stuck-bit' feature to the model
    // Use bitwise operations (xor, or, clear) to modify the phv
    void bit_xor(Phv *xor_phv, bool verbose);
    void bit_or(Phv *or_phv, bool verbose);
    void bit_clr(Phv *clr_phv, bool verbose);

 private:
    Phv *clone_internal(int n_words);
    DISALLOW_COPY_AND_ASSIGN(Phv);

    PhvFactory                                      *pf_;
    Teop                                            *teop_;
    PhvPipeData                                     *pipe_data_;
    CacheId                                          cache_id_;
    PacketInfo                                       ingress_;
    PacketInfo                                       egress_;
    uint32_t                                         words_[kWordsMaxExtended];
    BitVector<kWordsMaxExtended>                     valid_;
    std::unique_ptr< BitVector<kWordsMaxExtended> >  error_;
    std::unique_ptr< BitVector<kWordsMaxExtended> >  written_;
    uint32_t                                         flags_;
    int                                              pipe_index_;
    uint8_t                                          source_;
  };


  // Provide convenient aliases
  namespace k_phv {
    static constexpr int kBadPhv        = Phv::make_word_const(15,31);
    static constexpr int kTagalongStart = Phv::make_word_const(8,0);
  }

}
#endif // _SHARED_PHV_
