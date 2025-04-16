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

#ifndef _SHARED_CLOT_
#define _SHARED_CLOT_

#include <common/rmt-assert.h>
#include <rmt-defs.h>


namespace MODEL_CHIP_NAMESPACE {

  class ClotEntry {

 public:
    static constexpr uint8_t kFlagValid               = 0x01;
    static constexpr uint8_t kFlagEmitted             = 0x04;
    static constexpr uint8_t kFlagHasPrevAdjacentClot = 0x08;
    static constexpr int     kMinLen =    1;
    static constexpr int     kMinOff = -255;
    static constexpr int     kMaxLen = RmtDefs::kClotMaxLen;
    static constexpr int     kMaxOff = RmtDefs::kClotMaxOffset;
    static constexpr int     kMaxLenPlusOff = RmtDefs::kClotMaxLenPlusOffset;
    static constexpr uint8_t kMaxTag = RmtDefs::kClotMaxTag;
    static constexpr uint8_t kBadTag = 0xFF;
    static_assert( (kBadTag > kMaxTag), "Clot bad tag must exceed Clot max tag" );
    static_assert( (kBadTag <= 0xFF),   "Clot bad tag must fit in uint8_t" );
    static_assert( (kMaxLen <= 32767),  "Clot MaxLen must fit in int16_t" );
    static_assert( (kMaxOff <= 32767),  "Clot MaxOff must fit in int16_t" );
    static_assert( (kMaxLenPlusOff <= 32767),  "Clot MaxLenPlusOff must fit in int16_t" );
    static bool is_valid_tag(uint8_t tag)     { return (tag <= kMaxTag); }
    static bool is_valid_length(int len)      { return ((len >= kMinLen) && (len <= kMaxLen)); }
    static bool is_valid_offset(int off)      { return ((off >= kMinOff) && (off <= kMaxOff)); }
    static bool is_valid_length_offset(int len, int off) {
      return (is_valid_length(len) && is_valid_offset(off) && ((len+off) <= kMaxLenPlusOff));
    }

    ClotEntry()   { reset(); }
    ~ClotEntry()  { reset(); }

    inline void reset() {
      flags_ = 0; tag_ = kBadTag;
      length_ = offset_ = 0;
      checksum_ = 0;
    }
    inline void     invalidate()               { flags_ &= ~kFlagValid; }
    inline bool     valid()              const { return ((flags_ & kFlagValid) != 0); }
    inline void     set_valid()                { flags_ |= kFlagValid; }
    inline bool     emitted()            const { return ((flags_ & kFlagEmitted) != 0); }
    inline void     set_emitted()              { flags_ |=  kFlagEmitted; }
    inline void     clear_emitted()            { flags_ &= ~kFlagEmitted; }
    inline bool     has_prev_adjacent()  const { return ((flags_ & kFlagHasPrevAdjacentClot) != 0); }
    inline void     set_has_prev_adjacent()    { flags_ |=  kFlagHasPrevAdjacentClot; }
    inline void     clear_has_prev_adjacent()  { flags_ &= ~kFlagHasPrevAdjacentClot; }

    inline uint8_t  tag()      const { return tag_; }
    inline int      length()   const { return static_cast<int>(length_); }
    inline int      offset()   const { return static_cast<int>(offset_); }
    inline uint16_t checksum() const { return checksum_; }

    inline int get_gap_before(int len2, int offset2) const {
      // If this clot finishes before [offset2,len2] return gap in between
      return (offset2 >= offset()+length()) ?offset2  - (offset() + length()) :-1;
    }
    inline int get_gap_after(int len2, int offset2) const {
      // If [offset2,len2] finishes before this clot return gap in between
      return (offset2+len2 <= offset()) ?offset() - (offset2  + len2) :-1;
    }
    inline int get_gap(int len2, int offset2) const {
      int gap = get_gap_before(len2, offset2);
      if (gap < 0) gap = get_gap_after(len2, offset2);
      return gap;
    }
    inline bool gap_ok(int len2, int offset2, int min_gap, bool allow_adjacent) const {
      if (min_gap < 0) return true; // Gap always ok if negative minGap
      int gap = get_gap(len2, offset2);
      if ((gap == 0) && allow_adjacent) return true;
      if (gap < min_gap) return false;
      return true;
    }
    inline bool is_immediately_after(int len2, int offset2) const {
      return (get_gap_after(len2, offset2) == 0);
    }
    inline bool is_immediately_after(const ClotEntry &c2) const {
      return is_immediately_after(c2.length(), c2.offset());
    }

    inline bool get(uint8_t *tag, int *len, int *off, uint16_t *csum) const {
      RMT_ASSERT((tag != nullptr) && (len != nullptr) && (off != nullptr) && (csum != nullptr));
      *tag = tag_;
      *len = length();
      *off = offset();
      *csum = checksum();
      return valid();
    }
    inline void set(uint8_t tag, int len, int off, uint16_t csum) {
      RMT_ASSERT(is_valid_tag(tag));
      RMT_ASSERT(is_valid_length_offset(len, off));
      tag_ = tag;
      length_ = static_cast<int16_t>(len);
      offset_ = static_cast<int16_t>(off);
      checksum_ = csum;
      set_valid();
    }

 private:
    uint8_t  flags_;
    uint8_t  tag_;
    int16_t  length_;
    int16_t  offset_;
    uint16_t checksum_;
  };



  class Clot {

 public:
    static bool kRelaxOverlapCheck; // Defined in rmt-config.cpp
    static bool kAllowAdjacent;
    static bool kAllowDuplicate;
    static bool kAllowRepeatEmit;

    static constexpr uint8_t kFlagEmitStarted            = 0x01;
    static constexpr uint8_t kFlagClearErrMask           = 0x0F;
    static constexpr uint8_t kFlagGetErrTagInvalid       = 0x10;
    static constexpr uint8_t kFlagGetErrPrevAdjUnemitted = 0x20;
    static constexpr uint8_t kFlagGetErrAlreadyEmitted   = 0x40;
    static constexpr int     kClotMinGap = RmtDefs::kClotMinGap;
    static constexpr int     kTagWidth = RmtDefs::kClotTagWidth;
    static constexpr int     kMaxSimulTags = static_cast<int>(RmtDefs::kClotMaxSimultaneousTags);
    static constexpr int     kClotArraySize = (kMaxSimulTags > 0) ?kMaxSimulTags :1;
    static constexpr int     kMaxLen = ClotEntry::kMaxLen;
    static constexpr int     kMaxOff = ClotEntry::kMaxOff;
    static constexpr int     kMaxLenPlusOff = ClotEntry::kMaxLenPlusOff;
    static constexpr uint8_t kMaxTag = ClotEntry::kMaxTag;
    static constexpr uint8_t kBadTag = ClotEntry::kBadTag;
    static constexpr uint8_t kBadIndex = 255;
    static_assert( (kBadTag > kMaxTag), "Clot bad tag must exceed Clot max tag" );
    static_assert( (kClotArraySize >= kMaxSimulTags),
                   "Clot array size must meet/exceed max number simultaneous clot tags" );
    static_assert( (kClotArraySize <= 255), "Clot array index must fit into uint8_t" );
    static_assert( (kClotArraySize < kBadIndex), "Need discriminated bad index val" );
    static_assert( (kMaxTag < 64), "In-use tags must fit into 64b bitmask" );
    static bool is_valid_tag(uint8_t tag) { return ClotEntry::is_valid_tag(tag); }
    static bool is_valid_index(uint8_t i) { return (i < kBadIndex); }

    Clot()  { reset(); }
    ~Clot() { reset(); }

    inline bool emit_started()                const { return ((flags_ & kFlagEmitStarted) != 0); }
    inline bool err_invalid_tag()             const { return ((flags_ & kFlagGetErrTagInvalid) != 0); }
    inline bool err_prev_adjacent_unemitted() const { return ((flags_ & kFlagGetErrPrevAdjUnemitted) != 0); }
    inline bool err_already_emitted()         const { return ((flags_ & kFlagGetErrAlreadyEmitted) != 0); }
    inline void err_clear()                         { flags_ &= kFlagClearErrMask; } // Clear on get/set
    inline bool is_ok_index(uint8_t i)        const { return is_valid_index(i) && (i < n_tags_set_); }

    inline const char *err_str() const {
      if (err_invalid_tag()) return "clot tag invalid";
      else if (err_prev_adjacent_unemitted()) return "previous adjacent clot entry exists but not yet emitted";
      else if (err_already_emitted()) return "clot entry already emitted";
      else return "ok";
    }

    inline void reset() {
      flags_ = 0;
      last_emitted_index_ = kBadIndex;
      min_off_ = ClotEntry::kMinOff; max_off_ = ClotEntry::kMaxOff;
      n_tags_used_ = n_tags_set_ = 0;
      tags_in_use_ = UINT64_C(0);
      for (size_t i = 0; i <  kClotArraySize; i++) { clots_[i].reset(); }
    }
    inline void reset_emit() {
      err_clear(); // Clear down error flags
      last_emitted_index_ = kBadIndex; // Forget all emissions
      for (size_t i = 0; i <  kClotArraySize; i++) { clots_[i].clear_emitted(); }
    }
    // Maintain extra indexes to allow checks that prev adjacent ClotEntries are emitted
    inline void reset_indexes() {
      for (size_t i = 0; i <  kClotArraySize; i++) {
        clots_[i].clear_has_prev_adjacent();
        index_to_prev_adjacent_index_map_[i] = kBadIndex;
      }
      for (size_t i = 0; i <  kMaxTag+1; i++) {
        tag_to_index_map_[i] = kBadIndex;
      }
    }
    inline void build_indexes() {
      reset_indexes();
      int n = n_tags_set();
      for (int i = 0; i < n; i++) {
        if (clots_[i].valid()) {
          RMT_ASSERT(is_valid_tag(clots_[i].tag()));
          tag_to_index_map_[clots_[i].tag()] = i;
          for (int j = 0; j < n; j++) {
            if (clots_[j].valid() && clots_[i].is_immediately_after(clots_[j])) {
              RMT_ASSERT(!clots_[i].has_prev_adjacent());
              clots_[i].set_has_prev_adjacent();
              index_to_prev_adjacent_index_map_[i] = j;
            }
          }
        }
      }
    }
    inline bool check_any_prev_adjacent_was_last_emitted(uint8_t tag) {
      bool ok = true;
      RMT_ASSERT(is_valid_tag(tag));
      uint8_t index = tag_to_index_map_[tag];
      RMT_ASSERT(is_ok_index(index));

      if (clots_[index].has_prev_adjacent()) {
        uint8_t prev_adjacent_index = index_to_prev_adjacent_index_map_[index];
        RMT_ASSERT(is_ok_index(prev_adjacent_index));
        RMT_ASSERT(clots_[prev_adjacent_index].valid());
        // If prev adjacent ClotEntry NOT the prev one emitted then
        // we can't emit this one - Deparser constraint - XXX
        if (last_emitted_index_ != prev_adjacent_index) {
          ok = false;
          flags_ |= kFlagGetErrPrevAdjUnemitted;
        }
      }
      if (ok) last_emitted_index_ = index; // Remember this clot emitted
      return ok;
    }
    inline void clear_last_emitted() {
      last_emitted_index_ = kBadIndex;
    }


    inline bool is_valid_length(int len)  { return ClotEntry::is_valid_length(len); }
    inline bool is_valid_offset(int off)  { return ((off >= min_off_) && (off <= max_off_)); }
    inline bool is_valid_length_offset(int len, int off) {
      return is_valid_length(len) && is_valid_offset(off) && ((len+off) <= (max_off_+1));
    }
    inline int16_t min_offset()     const { return min_off_; }
    inline int16_t max_offset()     const { return max_off_; }

    inline uint16_t read_length(int length) {
      RMT_ASSERT(is_valid_length(length) && (length > 0));
      // NB !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      // ONLY in this single case do we fake out the ACTUAL bits stored in the
      // real RTL CLOT object where ClotLens 1-64 are actually stored as 0-63
      // This was a change that happened in XXX - also in parser.cpp
      // NB !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      return static_cast<uint16_t>(length-1);
    }
    inline uint16_t read_offset(int offset) {
      RMT_ASSERT(is_valid_offset(offset));
      return (offset < 0) ?static_cast<uint16_t>(offset) & 0x1FF :static_cast<uint16_t>(offset);
    }
    inline uint16_t get_length(int length) {
      RMT_ASSERT(is_valid_length(length));
      return static_cast<uint16_t>(length);
    }
    inline uint16_t get_offset(int offset) {
      RMT_ASSERT(is_valid_offset(offset));
      return (offset < 0) ?static_cast<uint16_t>(offset) & 0x1FF :static_cast<uint16_t>(offset);
    }

    inline void set_min_max_offset(int min_off, int max_off) {
      RMT_ASSERT(min_off >= ClotEntry::kMinOff);
      RMT_ASSERT(max_off <= ClotEntry::kMaxOff);
      RMT_ASSERT(min_off <= max_off);
      min_off_ = min_off;
      max_off_ = max_off;
    }

    inline int  n_tags_set()  const { return n_tags_set_; }
    inline int  n_tags_used() const { return n_tags_used_; }
    inline bool is_full()     const { return (n_tags_set_ >= kMaxSimulTags); }

    inline bool read_signed(int index, uint8_t *tag,
                            int *length, int *offset, uint16_t *checksum) {
      if ((index < 0) || index >= n_tags_set_) return false;
      return clots_[index].get(tag, length, offset, checksum);
    }
    inline bool read(int index, uint8_t *tag,
                     uint16_t *length, uint16_t *offset, uint16_t *checksum) {
      int len, off;
      bool ok = read_signed(index, tag, &len, &off, checksum);
      if ((ok) && (length != nullptr)) *length = read_length(len);
      if ((ok) && (offset != nullptr)) *offset = read_offset(off);
      return ok;
    }

    inline bool contains(uint8_t tag) {
      if (!is_valid_tag(tag) || (n_tags_set_ == 0)) return false;
      return (((tags_in_use_ >> tag) & 1) == 1);
    }
    inline bool get_first(uint8_t tag, int *length, int *offset, uint16_t *checksum) {
      RMT_ASSERT((length != nullptr) && (offset != nullptr) && (checksum != nullptr));
      err_clear();
      if (!emit_started()) build_indexes(); // Build indexes on first get op
      flags_ |= kFlagEmitStarted;
      if (contains(tag)) {
        // Go through all clots set
        // Return *first* one that matches input tag
        for (int i = 0; i < n_tags_set_; i++) {
          uint8_t entry_tag = kBadTag;
          if ((clots_[i].get(&entry_tag, length, offset, checksum)) &&
              (entry_tag == tag)) {
            // XXX: Error if clot entry already emitted
            if (clots_[i].emitted() && !kAllowRepeatEmit) {
              flags_ |= kFlagGetErrAlreadyEmitted; // Clot entry re-emit disallowed
              return false;
            } else {
              clots_[i].set_emitted();
              return true;
            }
          }
        }
      }
      flags_ |= kFlagGetErrTagInvalid; // Bad tag or missing tag
      return false;
    }
    inline bool get_signed(uint8_t tag, int *length, int *offset, uint16_t *checksum) {
      return get_first(tag, length, offset, checksum);
    }
    inline bool get(uint8_t tag, uint16_t *length, uint16_t *offset, uint16_t *checksum,
                    bool do_adj_check = true) {
      int len, off;
      bool ok = get_signed(tag, &len, &off, checksum);
      if (ok) {
        *length = get_length(len);
        *offset = get_offset(off);
        ok = !do_adj_check || check_any_prev_adjacent_was_last_emitted(tag);
      }
      return ok;
    }
    inline bool contains_overlap(uint8_t tag, int length, int offset) {
      if (kRelaxOverlapCheck) return false; // XXX: Overlap relaxation for ParserDV
      if (kClotMinGap < 0) return false; // Overlaps allowed if negative gap
      for (uint8_t i = 0; i < n_tags_set_; i++) {
        uint8_t entry_tag = kBadTag;
        int dummy_len, dummy_off;
        uint16_t dummy_csum;
        if ((clots_[i].get(&entry_tag, &dummy_len, &dummy_off, &dummy_csum)) &&
            (entry_tag != tag) &&
            (!clots_[i].gap_ok(length, offset, kClotMinGap, kAllowAdjacent)))
          return true;
      }
      return false;
    }
    inline bool set(uint8_t tag, int length, int offset, uint16_t checksum) {
      err_clear();
      // Return false if bad tag, dup tag (maybe), len/off too big or too many CLOTs set
      if ((!is_valid_tag(tag)) ||
          (!is_valid_length_offset(length, offset)) ||
          (contains_overlap(tag, length, offset)) ||
          (is_full()) ||
          (contains(tag) && !kAllowDuplicate)) return false;
      if (!contains(tag)) n_tags_used_++; // Unique tags used
      clots_[n_tags_set_].set(tag, length, offset, checksum);
      tags_in_use_ |= UINT64_C(1) << tag;
      n_tags_set_++; // Total tags used including duplicates
      RMT_ASSERT(n_tags_set_ >= n_tags_used_);

      if (emit_started()) build_indexes(); // Rebuild indexes on subsequent set
      return true;
    }


 private:
    uint8_t                                  flags_;
    uint8_t                                  last_emitted_index_;
    int16_t                                  min_off_;
    int16_t                                  max_off_;
    int                                      n_tags_used_;
    int                                      n_tags_set_;
    uint64_t                                 tags_in_use_;
    std::array< ClotEntry, kClotArraySize >  clots_;
    std::array< uint8_t,   kClotArraySize >  index_to_prev_adjacent_index_map_;
    std::array< uint8_t,   kMaxTag+1 >       tag_to_index_map_;
  };


}

#endif // _SHARED_CLOT_
