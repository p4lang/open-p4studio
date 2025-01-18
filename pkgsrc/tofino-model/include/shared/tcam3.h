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

#ifndef _SHARED_TCAM3_
#define _SHARED_TCAM3_

#include <iostream>
#include <string>
#include <array>
#include <atomic>
#include <rmt-defs.h>
#include <bitvector.h>
#include <pipe-object.h>
#include <model_core/spinlock.h>

namespace MODEL_CHIP_NAMESPACE {

  template <size_t N_ENTRIES, size_t WIDTH>
      class Tcam3 {

 private:
    static constexpr bool    kTcamAllowWrap           = false;
    static constexpr bool    kTcamUseSpinlocks        = RmtDefs::kTcamUseSpinlocks;
    static constexpr int     kTcamLocks               = (((N_ENTRIES/32)>2) ?(N_ENTRIES/32) :(2));
    static constexpr int     kTcamRangeMaxSeparation  = 8;
    static constexpr uint8_t kTcamFlagBoundary        = 0x01;
    static constexpr uint8_t kTcamFlagPayload0Mask    = 0x0F;
    static constexpr uint8_t kTcamFlagValid           = 0x10;
    static constexpr uint8_t kTcamDefaultPayload0     = kTcamFlagBoundary;
    static constexpr uint8_t kTcamDefaultPayload1     = 0x0;
    static constexpr uint8_t kBytemapConfigDflt       = 0x0;
    static constexpr uint8_t kBytemapConfigDirtcam2   = 0x1;
    static constexpr uint8_t kBytemapConfigDirtcam4Lo = 0x2;
    static constexpr uint8_t kBytemapConfigDirtcam4Hi = 0x3;

    inline int  pri_to_index(const int pri, int hd) const { return (1 + pri + hd) % N_ENTRIES; }
    inline int  index_to_pri(const int i, int hd)   const { return (N_ENTRIES-1 + i - hd) % N_ENTRIES; }
    inline int  handle_wrap(const int i)    const { return ((i < 0) ?N_ENTRIES-1+((i+1)%N_ENTRIES) :i%N_ENTRIES); }
    inline bool ok_pri(const unsigned int pri)       const { return ((pri <= N_ENTRIES-1));  }
    inline bool ok_index(const unsigned int index)   const { return ((index <= N_ENTRIES-1)); }

    inline bool has_flag(const int index, uint8_t flag) const { return flags_[index] & flag; }
    inline void set_flag(const int index, uint8_t flag)   { flags_[index] = flags_[index] | flag; }
    inline void clear_flag(const int index, uint8_t flag) { flags_[index] = flags_[index] & ~flag; }

    // Mapping funcs for DirtCAMs etc
    inline uint16_t map_byte_dflt(const uint8_t byte, const uint8_t bits) const {
      return ((static_cast<uint16_t>(byte) << 8) | static_cast<uint16_t>(~byte & 0xFF));
    }
    inline uint16_t map_byte_dirtcam2(const uint8_t byte, const uint8_t bits) const {
      // Take 2 bits at a time, one-hot encode
      uint16_t out = 0;
      for (int tbi = 0; tbi < 4; tbi++) {
        uint8_t tb = (byte >> (tbi+tbi)) & 0x3;
        switch (tb) {
          case 0: out |= (0x001 << (tbi+tbi)); break;
          case 1: out |= (0x002 << (tbi+tbi)); break;
          case 2: out |= (0x100 << (tbi+tbi)); break;
          case 3: out |= (0x200 << (tbi+tbi)); break;
        }
      }
      return out;
    }
    inline uint16_t map_byte_dirtcam4_lo(const uint8_t byte, const uint8_t bits) const {
      uint16_t r = (1 << (byte & 0xF));
      if (bits == 4) {
        // eg. for vbit mode 2: search0[3:0] = r[3:0] and search1[3:0]=r[7:4]
        return ((r & 0x00F0) << 4) | (r & 0x000F);
      }
      return r;
    }
    inline uint16_t map_byte_dirtcam4_hi(const uint8_t byte, const uint8_t bits) const {
      uint16_t r = (1 << (byte >> 4));
      if (bits == 4) {
        r = (1 << (byte & 0xF)); // Use lo-bits of byte in this case
        // eg. for vbit mode 3: search0[3:0] = r[11:8] and search1[3:0]=r[15:12]
        return ((r & 0xF000) >> 4) | ((r & 0x0F00) >> 8);
      }
      return r;
    }

    inline uint16_t map_byte(const uint8_t config,
                             const uint8_t byte, const uint8_t bits) const {
      switch (config) {
        case kBytemapConfigDflt:       return map_byte_dflt(byte, bits);
        case kBytemapConfigDirtcam2:   return map_byte_dirtcam2(byte, bits);
        case kBytemapConfigDirtcam4Lo: return map_byte_dirtcam4_lo(byte, bits);
        case kBytemapConfigDirtcam4Hi: return map_byte_dirtcam4_hi(byte, bits);
        default:                       return map_byte_dflt(byte, bits);
      }
    }
    inline void map_search_bitvectors(const BitVector<WIDTH> &in_search1,
                                      BitVector<WIDTH> *search0,
                                      BitVector<WIDTH> *search1) const  {
      uint8_t orcnf = 0; // Track whether any non-zero bytemap config
      for (unsigned int i = 0; i*8 < WIDTH; i++) {
        uint8_t  cnf = bytemap_config_.get_byte(i);
        uint8_t  bits_in_byte = static_cast<uint8_t>(std::min((int)(WIDTH - i*8), 8));
        uint16_t out16 = map_byte(cnf, in_search1.get_byte(i), bits_in_byte);
        search0->set_byte(out16 & 0xFF, i);
        search1->set_byte(out16 >> 8, i);
        orcnf |= cnf;
      }
      got_nonzero_bytemap_config_ = (orcnf != 0);
    }
    inline void map_search_bitvectors_fast(const BitVector<WIDTH> &in_search1,
                                           BitVector<WIDTH> *search0,
                                           BitVector<WIDTH> *search1) const  {
      if (got_nonzero_bytemap_config_) {
        map_search_bitvectors(in_search1, search0, search1);
        uint64_t in1 = in_search1.get_word(0);
        uint64_t vs1 = search1->get_word(0);
        uint64_t vs0 = search0->get_word(0);
        RMT_LOG_OBJ(containing_obj_,
                    RmtDebug::verbose(RmtDebug::kRmtDebugMauTcam_Tcam3DebugMap),
                    "TCAM3::map()" " in1=0x%016" PRIx64
                    " s1=0x%016" PRIx64 " s0=0x%016" PRIx64 "\n", in1, vs1, vs0);
      } else {
        search1->copy_from(in_search1);
        search0->copy_from(in_search1);
        search0->invert();
      }
    }


    inline void set_valid(const int index, bool valid) {
      if (valid)
        set_flag(index, kTcamFlagValid);
      else
        clear_flag(index, kTcamFlagValid);
    }
    inline bool both_zero(const BitVector<WIDTH> &bv0,
                          const BitVector<WIDTH> &bv1) const {
      return (bv1.is_zero() && bv0.is_zero());
    }
    inline bool is_word0_word1_not00(const BitVector<WIDTH> &bv_word0,
                                     const BitVector<WIDTH> &bv_word1) const {
      return !both_zero(bv_word0, bv_word1);
    }
    inline bool is_srch0_srch1_00(const BitVector<WIDTH> &bv_srch0,
                                  const BitVector<WIDTH> &bv_srch1) const {
      return both_zero(bv_srch0, bv_srch1);
    }



    // Get/Set payload byte(s) - abuse lo-bits of flags_
    // field to store further payload info
    //
    inline uint8_t get_payload0(const int index) const {
      return (ok_index(index)) ?(flags_[index] & kTcamFlagPayload0Mask) :0;
    }
    inline uint8_t get_payload0_pri(const int pri, int head) const {
      return (ok_pri(pri)) ?get_payload0(pri_to_index(pri,head)) :0;
    }
    inline void set_payload0(const int index, uint8_t payload0) {
      if (!ok_index(index)) return;
      uint8_t flags = flags_[index];
      flags &= ~kTcamFlagPayload0Mask; // Clear existing payload
      flags |= (payload0 & kTcamFlagPayload0Mask); // OR in new
      flags_[index] = flags;
    }

    inline uint8_t get_payload1(const int index) const {
      return (ok_index(index)) ?payloads_[index] :0;
    }
    inline uint8_t get_payload1_pri(const int pri, int head) const {
      return (ok_pri(pri)) ?get_payload1(pri_to_index(pri,head)) :0;
    }
    inline void set_payload1(const int index, uint8_t payload1) {
      if (!ok_index(index)) return;
      payloads_[index] = payload1;
    }

 public:
    Tcam3()              : containing_obj_(NULL) { reset(); }
    Tcam3(PipeObject *p) : containing_obj_(p)    { reset(); }
    ~Tcam3() { }


    inline void lock(const int index) const {
      if (!kTcamUseSpinlocks) return;
      spinlocks_[index % kTcamLocks].lock();
    }
    inline void unlock(const int index) const {
      if (!kTcamUseSpinlocks) return;
      spinlocks_[index % kTcamLocks].unlock();
    }

    inline int get_tcam_start() const { return head_; }
    inline void set_tcam_start(const int head_index) {
      if (!ok_index(head_index)) return;
      head_ = head_index;
      if ((kTcamAllowWrap) &&
          ((!is_boundary(head_)) || (!is_boundary(handle_wrap(head_-1))))) {
        RMT_LOG_OBJ(containing_obj_,RmtDebug::warn(),
                "TCAM3::set_tcam_start: Possible multi-range match across headptr boundary");
      }
      if (head_index != N_ENTRIES-1) {
        RMT_LOG_OBJ(containing_obj_,RmtDebug::warn(),
                    "TCAM3::set_tcam_start(%d) Variable TCAM start in use!",head_index);
      }
      RMT_ASSERT(index_to_pri(head_index,head_index) == N_ENTRIES-1);
    }
    inline void set_lookup_return_pri(const bool lookup_return_pri) {
      lookup_return_pri_ = lookup_return_pri;
    }
    inline void set_bytemap_config(const unsigned int byte_index, uint8_t config) {
      if (byte_index * 8 > WIDTH) return;
      if (config != 0) got_nonzero_bytemap_config_ = true;
      bytemap_config_.set_byte(config, byte_index);
    }

    inline bool is_valid(const int index) const {
      if (!ok_index(index)) return false;
      return has_flag(index, kTcamFlagValid);
    }

    // Get/Set boundary flag for multi-range matches
    // Notionally boundary is between entries so is_boundary(N)
    // says whether there is a boundary between entry N and N+1
    inline bool is_boundary(const int index) const {
      if (!ok_index(index)) return false;
      return has_flag(index, kTcamFlagBoundary);
    }
    inline bool is_boundary_above(const int index) const {
      if ((index == N_ENTRIES-1) && (!kTcamAllowWrap)) return true;
      return is_boundary(index);
    }
    inline bool is_boundary_below(const int index) const {
      if ((index == 0) && (!kTcamAllowWrap)) return true;
      return is_boundary(handle_wrap(index-1));
    }
    inline void set_boundary(const int index, bool boundary=true) {
      if (!ok_index(index)) return;
      if (boundary) {
        set_flag(index, kTcamFlagBoundary);
      } else {
        clear_flag(index, kTcamFlagBoundary);
      }
      if ((kTcamAllowWrap) && (!boundary) &&
          ((index == head_) || (index == handle_wrap(head_-1)))) {
        RMT_LOG_OBJ(containing_obj_,RmtDebug::warn(),
                "TCAM3::set_boundary: Possible multi-range match across headptr boundary");
      }
    }

    // Map TCAM pri to TCAM index (possibly using supplied head)
    inline int get_index(const int pri, int head=-1) const {
      if (head < 0) head = head_;
      int index = pri_to_index(pri,head);
      if (index != pri) {
        RMT_LOG_OBJ(containing_obj_,RmtDebug::verbose(),
                "TCAM3::get_index(%d)=%d Variable TCAM start in use!",pri,index);
      }
      return index;
    }

    // Map prev payload get/set to payload1
    inline uint8_t get_payload(const int index) const {
      return get_payload1(index);
    }
    inline uint8_t get_payload_pri(const int index, int head=-1) const {
      if (head < 0) head = head_;
      return get_payload1_pri(index, head);
    }
    inline void set_payload(const int index, uint8_t payload) {
      set_payload1(index, payload);
    }

    // But now also allow get/set of both (note only 4-bits payload0)
    inline void get_payloads(const int index,
                             uint8_t *payload0, uint8_t *payload1) const {
      if (payload0 != NULL) *payload0 = get_payload0(index);
      if (payload1 != NULL) *payload1 = get_payload1(index);
    }
    inline void set_payloads(const int index,
                             uint8_t payload0, uint8_t payload1) {
      set_payload0(index, payload0);
      set_payload1(index, payload1);
    }


    // Get/Set word0/word1 stored
    inline void get(const int index,
                    BitVector<WIDTH> *bv_word0, BitVector<WIDTH> *bv_word1,
                    uint8_t *payload0, uint8_t *payload1) const {
      if (!ok_index(index)) return;
      lock(index);
      if (bv_word0 != NULL) *bv_word0 = word0s_[index];
      if (bv_word1 != NULL) *bv_word1 = word1s_[index];
      get_payloads(index, payload0, payload1);
      unlock(index);
    }
    inline void get(const int index,
                    BitVector<WIDTH> *bv_word0, BitVector<WIDTH> *bv_word1) const {
      get(index, bv_word0, bv_word1, NULL, NULL);
    }
    inline void get_pri(const int pri,
                        BitVector<WIDTH> *bv_word0, BitVector<WIDTH> *bv_word1, int head=-1) const {
      if (!ok_pri(pri)) return;
      if (head < 0) head = head_;
      get(pri_to_index(pri,head), bv_word0, bv_word1);
    }


    // The funcs that allow valid to be explicitly set have
    // been made private - these days they should only be used
    // internally
 private:
    // Unlocked versions - only used by tcam_writereg op
    inline void set_word0_word1_nolock(const int index,
                                       const BitVector<WIDTH> &bv_word0,
                                       const BitVector<WIDTH> &bv_word1,
                                       const uint8_t payload0,
                                       const uint8_t payload1,
                                       bool valid) {
      if (!ok_index(index)) return;
      word0s_[index] = bv_word0;
      word1s_[index] = bv_word1;
      set_payloads(index, payload0, payload1);
      set_valid(index, valid);
    }
 public:
    inline void set_word0_word1_nolock(const int index,
                                       const BitVector<WIDTH> &bv_word0,
                                       const BitVector<WIDTH> &bv_word1,
                                       const uint8_t payload0,
                                       const uint8_t payload1) {
      set_word0_word1_nolock(index, bv_word0, bv_word1,
                             payload0, payload1, true);
    }
    inline void set_word0_word1_nolock(const int index,
                                       const BitVector<WIDTH> &bv_word0,
                                       const BitVector<WIDTH> &bv_word1) {
      set_word0_word1_nolock(index, bv_word0, bv_word1,
                             kTcamDefaultPayload0, kTcamDefaultPayload1, true);
    }
 private:
    // Normal locked version - calls unlocked version
    inline void set_word0_word1(const int index,
                                const BitVector<WIDTH> &bv_word0,
                                const BitVector<WIDTH> &bv_word1,
                                const uint8_t payload0,
                                const uint8_t payload1,
                                bool valid) {
      if (!ok_index(index)) return;
      lock(index);
      set_word0_word1_nolock(index, bv_word0, bv_word1,
                             payload0, payload1, valid);
      unlock(index);
    }
 private:
    // Convenience versions that call normal locked version but
    // allow you to leave out payload vals, default valid to true
    inline void set_word0_word1(const int index,
                                const BitVector<WIDTH> &bv_word0,
                                const BitVector<WIDTH> &bv_word1,
                                bool valid) {
      set_word0_word1(index, bv_word0, bv_word1,
                      kTcamDefaultPayload0, kTcamDefaultPayload1, valid);
    }
 public:
    inline void set_word0_word1(const int index,
                                const BitVector<WIDTH> &bv_word0,
                                const BitVector<WIDTH> &bv_word1,
                                const uint8_t payload0,
                                const uint8_t payload1) {
      set_word0_word1(index, bv_word0, bv_word1, payload0, payload1, true);
    }
 public:
    // Even more convenience!
    inline void set_word0_word1(const int index,
                                const BitVector<WIDTH> &bv_word0,
                                const BitVector<WIDTH> &bv_word1) {
      if (!ok_index(index)) return;
      set_word0_word1(index, bv_word0, bv_word1,
                      kTcamDefaultPayload0, kTcamDefaultPayload1);
    }
 private:
    // Set word0/word1 using value/mask
    inline void set_value_mask(const int index,
                               const BitVector<WIDTH> &bv_value,
                               const BitVector<WIDTH> &bv_mask,
                               bool valid) {
      if (!ok_index(index)) return;
      BitVector<WIDTH> bv_word0(UINT64_C(0));
      BitVector<WIDTH> bv_word1(UINT64_C(0));
      BitVector<WIDTH> bv_inverted_mask(UINT64_C(0));
      // Create ~mask
      bv_inverted_mask.copy_from(bv_mask);
      bv_inverted_mask.invert();
      // Word1 = value & mask | ~mask
      bv_word1.copy_from(bv_value);
      bv_word1.mask(bv_mask);
      bv_word1.or_with(bv_inverted_mask);
      // Word0 = ~value & mask | ~mask
      bv_word0.copy_from(bv_value);
      bv_word0.invert();
      bv_word0.mask(bv_mask);
      bv_word0.or_with(bv_inverted_mask);
      // Call word0/word1 set
      set_word0_word1(index, bv_word0, bv_word1, valid);
    }
 public:
    inline void set_value_mask(const int index,
                               const BitVector<WIDTH> &bv_value,
                               const BitVector<WIDTH> &bv_mask) {
      set_value_mask(index, bv_value, bv_mask, true);
    }
    inline void set_value(const int index,
                          const BitVector<WIDTH> &bv_value) {
      if (!ok_index(index)) return;
      set_value_mask(index, bv_value, BitVector<WIDTH>().fill_all_ones());
    }


    // Lookup in tcam
    inline bool tcam_match(const BitVector<WIDTH>& s0,
                           const BitVector<WIDTH>& s1,
                           const int index, const bool quiet=GLOBAL_FALSE) const {
      lock(index);
      bool hit = ((is_valid(index)) &&
                  (BitVector<WIDTH>::tcam_compare(word0s_[index], word1s_[index], s0, s1)));
      unlock(index);
      if (quiet) return hit;

      uint64_t dbgflg = UINT64_C(0);
      if (hit) {
        dbgflg = RmtDebug::verbose(RmtDebug::kRmtDebugMauTcam_Tcam3DebugHit);
        RMT_LOG_OBJ(containing_obj_,
                    RmtDebug::verbose(RmtDebug::kRmtDebugMauTcam_Tcam3LookupDetail),
                    "TCAM3::tcam_match(%d)=%d {%d}\n", index, hit,
                    has_flag(index, kTcamFlagBoundary));
      } else if (is_valid(index) && !both_zero(word0s_[index],word1s_[index])) {
        dbgflg = RmtDebug::verbose(RmtDebug::kRmtDebugMauTcam_Tcam3DebugMiss);
      }

      if (dbgflg != UINT64_C(0)) {
        const char *dbgstr = (hit) ?"HIT" :"MISS";
        uint64_t vw0 = word0s_[index].get_word(0);
        uint64_t vw1 = word1s_[index].get_word(0);
        uint64_t vs0 = s0.get_word(0);
        uint64_t vs1 = s1.get_word(0);
        RMT_LOG_OBJ(containing_obj_, dbgflg,
                    "TCAM3::tcam_match(%d)  w0=0x%016" PRIx64 " w1=0x%016" PRIx64 " {%d} %s\n",
                    index,vw0,vw1,has_flag(index,kTcamFlagBoundary),dbgstr);
        RMT_LOG_OBJ(containing_obj_, dbgflg,
                    " TCAM3::tcam_match(%d) s0=0x%016" PRIx64 " s1=0x%016" PRIx64 " {%d} %s\n",
                    index,vs0,vs1,has_flag(index,kTcamFlagBoundary),dbgstr);
      }
      return hit;
    }
    inline int tcam_lookup(const BitVector<WIDTH>& s0,
                           const BitVector<WIDTH>& s1,
                           const int hi_pri, const int lo_pri,
                           int head=-1, bool promote_hits=true) const {
      if ((!ok_pri(hi_pri)) || (!ok_pri(lo_pri))) return -1;
      if (lo_pri > hi_pri) return -1;
      if (head < 0) head = head_;
      int try_pri;

      if (promote_hits) {
        // May have a match from above the [hi,lo] range
        // Check hi_pri for a match but give up if boundary above
        try_pri = hi_pri;
        while (try_pri - hi_pri < kTcamRangeMaxSeparation) {
          // NB is_boundary_above takes care of !kTcamAllowWrap
          int index = pri_to_index(handle_wrap(try_pri),head);
          if (tcam_match(s0, s1, index, (try_pri == hi_pri))) {
            // Any match above returns hi_pri
            return ((lookup_return_pri_) ?hi_pri :pri_to_index(hi_pri,head));
          }
          if (is_boundary_above(index)) break;
          try_pri++;
        }
      }
      // Now look for matches within [hi,lo]
      for (int pri = hi_pri; pri >= lo_pri; pri--) {
        int index = pri_to_index(pri,head);
        if (tcam_match(s0, s1, index)) {
          // HIT - maybe return actual HIT pri - no promotion
          if (!promote_hits) return ((lookup_return_pri_) ?pri :pri_to_index(pri,head));
          // HIT - see if we can extend to a higher pri (but not beyond hi_pri)
          int higher_pri = hi_pri;
          tcam_find_range(pri, &higher_pri, NULL, head);
          return ((lookup_return_pri_) ?higher_pri :pri_to_index(higher_pri,head));
        }
      }
      if (promote_hits) {
        // May have a match from below the [hi,lo] range
        // Check lo_pri for a match but give up if boundary below
        try_pri = lo_pri;
        while (lo_pri - try_pri < kTcamRangeMaxSeparation) {
          // NB is_boundary_below takes care of !kTcamAllowWrap
          int index = pri_to_index(handle_wrap(try_pri),head);
          if (tcam_match(s0, s1, index, (try_pri == lo_pri))) {
            // Any match below returns lo_pri
            return ((lookup_return_pri_) ?lo_pri :pri_to_index(lo_pri,head));
          }
          if (is_boundary_below(index)) break;
          try_pri--;
        }
      }
      return -1;
    }
    inline int tcam_lookup(const BitVector<WIDTH>& s0,
                           const BitVector<WIDTH>& s1, int head=-1) const {
      return tcam_lookup(s0, s1, N_ENTRIES-1, 0, head);
    }
    inline int tcam_lookup(const BitVector<WIDTH>& in_search1,
                           const int hi_pri, const int lo_pri,
                           int head=-1, bool promote_hits=true) const {
      BitVector<WIDTH> search0(UINT64_C(0));
      BitVector<WIDTH> search1(UINT64_C(0));
      map_search_bitvectors_fast(in_search1, &search0, &search1);
      return tcam_lookup(search0, search1, hi_pri, lo_pri, head, promote_hits);
    }
    inline int tcam_lookup(const BitVector<WIDTH>& s1, int head=-1) const {
      return tcam_lookup(s1, N_ENTRIES-1, 0, head);
    }
    // Return index regardless of setting of lookup_return_pri_
    inline int tcam_lookup_index(const BitVector<WIDTH>& s1, int head=-1) const {
      if (head < 0) head = head_;
      int retval = tcam_lookup(s1, N_ENTRIES-1, 0, head);
      if (retval < 0) return retval;
      return (lookup_return_pri_) ?pri_to_index(retval,head) :retval;
    }
    // Return pri regardless of setting of lookup_return_pri_
    inline int tcam_lookup_pri(const BitVector<WIDTH>& s1, int head=-1) const {
      if (head < 0) head = head_;
      int retval = tcam_lookup(s1, N_ENTRIES-1, 0, head);
      if (retval < 0) return retval;
      return (lookup_return_pri_) ?retval :index_to_pri(retval,head);
    }




    // Find range of entries that hit would be distributed to around hit pos
    // but never exceed passed-in [hi_pri,lo_pri]
    inline void tcam_find_range(const int hit_pri,
                                int *hi_pri, int *lo_pri, int head=-1) const {
      if (!ok_pri(hit_pri)) return;
      if (head < 0) head = head_;

      if (hi_pri != NULL) {
        int hi_inp = *hi_pri;
        *hi_pri = hit_pri;
        int try_pri = hit_pri;
        while ((try_pri <= hi_inp) &&
               (try_pri - hit_pri < kTcamRangeMaxSeparation)) {
          int index = pri_to_index(try_pri,head);
          *hi_pri = try_pri;
          if (is_boundary_above(index)) break;
          if (try_pri == N_ENTRIES-1) break; // Don't wrap pri hi->lo
          try_pri++;
        }
      }
      if (lo_pri != NULL) {
        int lo_inp = *lo_pri;
        *lo_pri = hit_pri;
        int try_pri = hit_pri;
        while ((try_pri >= lo_inp) &&
               (hit_pri - try_pri < kTcamRangeMaxSeparation)) {
          int index = pri_to_index(try_pri,head);
          *lo_pri = try_pri;
          if (is_boundary_below(index)) break;
          if (try_pri == 0) break; // Don't wrap pri lo->hi
          try_pri--;
        }
      }
      return;
    }


    // Now lookup a whole BitVector of priorities returning a new BitVector
    inline int tcam_lookup(const BitVector<WIDTH>& s0,
                           const BitVector<WIDTH>& s1,
                           const BitVector<N_ENTRIES>& hits_in,
                           BitVector<N_ENTRIES> *hits_out, int head=-1) const {
      RMT_ASSERT(hits_out != NULL);
      if (head < 0) head = head_;
      int n_hits = 0;
      int pri = hits_in.get_first_bit_set(-1);
      while (pri >= 0) {
        if (tcam_match(s0, s1, pri_to_index(pri,head))) {
          hits_out->set_bit(pri);
          n_hits++;
        }
        pri = hits_in.get_first_bit_set(pri);
      }
      return n_hits;
    }
    // As above but automatically generate s0/s1 from in_search1
    inline int tcam_lookup(const BitVector<WIDTH>& in_search1,
                           const BitVector<N_ENTRIES>& hits_in,
                           BitVector<N_ENTRIES> *hits_out, int head=-1) const {
      BitVector<WIDTH> search0(UINT64_C(0));
      BitVector<WIDTH> search1(UINT64_C(0));
      map_search_bitvectors_fast(in_search1, &search0, &search1);
      return tcam_lookup(search0, search1, hits_in, hits_out, head);
    }
    // And find range around hits in BitVector returning new BitVector
    inline void tcam_find_range(const BitVector<N_ENTRIES>& hits_in,
                                BitVector<N_ENTRIES> *hits_out, int head=-1) const {
      RMT_ASSERT(hits_out != NULL);
      if (head < 0) head = head_;
      int pri = hits_in.get_first_bit_set(-1);
      while (pri >= 0) {
        int hi_pri = N_ENTRIES;
        int lo_pri = 0;
        tcam_find_range(pri, &hi_pri, &lo_pri, head);
        for (int i = lo_pri; i <= hi_pri; i++) hits_out->set_bit(i);
        pri = hits_in.get_first_bit_set(pri);
      }
    }

    inline void print(const int index) const {
      RMT_LOG_OBJ(containing_obj_, RmtDebug::verbose(),
                  "%s\n",word0s_[index].to_string().c_str());
      RMT_LOG_OBJ(containing_obj_, RmtDebug::verbose(),
                  "%s\n",word1s_[index].to_string().c_str());
    }

    inline void reset(const bool lookup_return_pri=false) {
      static_assert( (kTcamRangeMaxSeparation > 0),
                     "kTcamRangeMaxSeparation too small");
      head_ = 0;
      lookup_return_pri_ = lookup_return_pri;
      got_nonzero_bytemap_config_ = false;
      bytemap_config_.fill_all_zeros();
      for (unsigned int index = 0; index < N_ENTRIES; index++) {
        flags_[index] = 0;
        payloads_[index] = 0;
        word0s_[index].fill_all_zeros();
        word1s_[index].fill_all_zeros();
      }
      for (int lock = 0; lock < kTcamLocks; lock++) {
        spinlocks_[lock].clear();
      }
    }


 private:
    PipeObject                                      *containing_obj_;
    int                                              head_;
    bool                                             lookup_return_pri_;
    mutable bool                                     got_nonzero_bytemap_config_;
    BitVector<WIDTH+8>                               bytemap_config_;
    mutable std::array<
        model_core::Spinlock,kTcamLocks>             spinlocks_ { };
    std::array<uint8_t,N_ENTRIES>                    flags_ { };
    std::array<uint8_t,N_ENTRIES>                    payloads_ { };
    std::array<BitVector<WIDTH>,N_ENTRIES>           word0s_ { };
    std::array<BitVector<WIDTH>,N_ENTRIES>           word1s_ { };
  };
}

#endif // _SHARED_TCAM3_
