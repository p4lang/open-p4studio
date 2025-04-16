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

#ifndef _SHARED_MAU_TCAM_
#define _SHARED_MAU_TCAM_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <mau-lookup-result.h>
#include <mau-tcam-reg.h>
#include <tcam3.h>
#include <phv.h>


namespace MODEL_CHIP_NAMESPACE {

  class Mau;
  class MauTcamRow;


  class MauTcam : public MauObject {
 public:
    static constexpr int      kType = RmtTypes::kRmtTypeMauTcam;
    static constexpr int      kTernaryMatchInputBits = MauDefs::kTernaryMatchInputBits;
    static constexpr int      kTernaryMatchValidBits = MauDefs::kTernaryMatchValidBits;

    static constexpr int      kTcamRowsPerMau = MauDefs::kTcamRowsPerMau;
    static constexpr int      kTcamMidRowUpper = kTcamRowsPerMau/2;
    static constexpr int      kTcamMidRowLower = kTcamMidRowUpper-1;
    static_assert( ((kTcamRowsPerMau % 2) == 0), "Logic assumes even number TCAM rows");

    static constexpr int      kLogicalTcamsPerMau = MauDefs::kLogicalTcamsPerMau;
    static constexpr int      kTcamWidth = MauDefs::kTcamWidth;
    static constexpr int      kTcamAddressWidth  = MauDefs::kTcamAddressWidth;
    static constexpr int      kTcamEntries = 1<<kTcamAddressWidth;
    static constexpr int      kTcamAddressMask = (1<<kTcamAddressWidth)-1;

    static constexpr int      kTcamMatchAddrWidth = MauDefs::kTableResultMatchAddrWidth; // 19
    static constexpr int      kTcamMatchAddrMask = (1<<kTcamMatchAddrWidth)-1;

    static constexpr int      kPayloadOffset = 0;
    static constexpr int      kPayloadBits = 1;
    static constexpr uint64_t kPayloadMask = (UINT64_C(1) << kPayloadBits) - 1;
    static constexpr int      kDataOffset = kPayloadOffset + kPayloadBits;
    static constexpr int      kDataBits = kTcamWidth;
    static constexpr uint64_t kDataMask = (UINT64_C(1) << kDataBits) - 1;
    static constexpr int      kParityOffset = kDataOffset + kDataBits;
    static constexpr int      kParityBits = 2;
    static constexpr uint64_t kParityMask = (UINT64_C(1) << kParityBits) - 1;
    static constexpr int      kPayloadOffsetWithinTcamPayload = 0;
    static constexpr int      kParityOffsetWithinTcamPayload = 2;

    // Static funcs to form and split apart read/write vals
    static inline uint64_t rval_make(uint64_t w, uint8_t p) {
      uint8_t  p_payload = (p >> kPayloadOffsetWithinTcamPayload) & kPayloadMask;
      uint8_t  p_parity = (p >> kParityOffsetWithinTcamPayload) & kParityMask;
      uint64_t rval_payload = static_cast<uint64_t>(p_payload) << kPayloadOffset;
      uint64_t rval_data = w << kDataOffset;
      uint64_t rval_parity = static_cast<uint64_t>(p_parity) << kParityOffset;
      return rval_parity | rval_data | rval_payload;
    }
    static inline uint64_t wval_split_data_payload(uint64_t wval, uint8_t *payload) {
      uint8_t wval_payload = static_cast<uint8_t>((wval >> kPayloadOffset) & kPayloadMask);
      uint8_t wval_parity = static_cast<uint8_t>((wval >> kParityOffset) & kParityMask);
      if (payload != NULL)
        *payload = (wval_payload << kPayloadOffsetWithinTcamPayload) |
            (wval_parity << kParityOffsetWithinTcamPayload);
      return (wval >> kDataOffset) & kDataMask;
    }



    MauTcam(RmtObjectManager *om, int pipeIndex, int mauIndex,
            int rowIndex, int colIndex, int tcamIndex, Mau *mau);
    virtual ~MauTcam();

    inline int  col_index()               const { return col_index_; }
    inline int  row_index()               const { return row_index_; }
    inline MauTcamRow *row()              const { return row_; }
    inline void set_row(MauTcamRow *row)        { row_ = row; }
    inline int  tcam_index()              const { return tcam_index_; }
    inline int  get_search_bus()                { return mau_tcam_reg_.get_search_bus(); }
    inline bool get_chain_out()                 { return mau_tcam_reg_.get_chain_out(); }
    inline bool get_match_output_enable()       { return mau_tcam_reg_.get_match_output_enable(); }
    inline bool is_ingress()                    { return mau_tcam_reg_.get_ingress(); }
    inline bool is_egress()                     { return mau_tcam_reg_.get_egress(); }
    inline bool is_ghost()                      { return mau_tcam_reg_.get_ghost(); }
    inline uint8_t  get_vpn()                   { return mau_tcam_reg_.get_vpn(); }
    inline int  get_priority()                  { return mau_tcam_reg_.get_priority(); }
    inline int  get_logical_table()             { return mau_tcam_reg_.get_logical_table(); }

    inline bool drives_ltcam(int ltcam, uint8_t powered_ltcams) {
      return mau_tcam_reg_.drives_ltcam(ltcam, powered_ltcams);
    }
    inline bool get_ltcam_result_info(int ltcam, int *start_pos, int *n_entries, bool *bitmap) {
      return mau_tcam_reg_.get_ltcam_result_info(ltcam, start_pos, n_entries, bitmap);
    }
    inline bool bitmap_mode(int ltcam) {
      int start_pos, n_entries;
      bool bitmap;
      bool res = get_ltcam_result_info(ltcam, &start_pos, &n_entries, &bitmap);
      return res && bitmap;
    }
    inline bool multi_result_mode(int ltcam) {
      int start_pos, n_entries;
      bool bitmap;
      bool res = get_ltcam_result_info(ltcam, &start_pos, &n_entries, &bitmap);
      return res && (n_entries < kTcamEntries);
    }
    inline bool check_boundaries(int start, int entries) {
      return tcam_.is_boundary_below(start) && tcam_.is_boundary_above(start + entries - 1);
    }
    inline uint32_t compute_bitmap_result(int start, int entries, BitVector<kTcamEntries> *bv) {
      return mau_tcam_reg_.compute_bitmap_result(start, entries, bv);
    }
    inline uint32_t get_hit_address(int ltcam, int hit_index) {
      RMT_ASSERT(hit_index >= 0);
      return mau_tcam_reg_.get_hit_address(ltcam, static_cast<uint32_t>(hit_index));
    }
    inline bool wide_match()              const { return wide_match_; }
    inline void set_wide_match(bool tf)         { wide_match_ = tf; }



    inline void read(const int index, uint64_t *data0, uint64_t *data1, uint64_t T) const {
      BitVector<kTcamWidth> w1, w0;
      uint8_t p1 = 0, p0 = 0;
      if (get(index, &w0, &w1, &p0, &p1)) {
        if (data0 != NULL) *data0 = rval_make(w0.get_word(0), p0);
        if (data1 != NULL) *data1 = rval_make(w1.get_word(0), p1);
      }
    }
    inline void write(const int index, uint64_t data0, uint64_t data1, uint64_t T) {
      BitVector<kTcamWidth> w1, w0;
      uint8_t p1, p0;
      w0.set_word(wval_split_data_payload(data0, &p0), 0);
      w1.set_word(wval_split_data_payload(data1, &p1), 0);
      set(index, w0, w1, p0, p1);
    }
    inline bool get(const int index,
                    BitVector<kTcamWidth> *w0, BitVector<kTcamWidth> *w1,
                    uint8_t *payload0, uint8_t *payload1) const {
      if ((w0 == NULL) || (w1 == NULL)) return false;
      if ((index < 0) && (index >= kTcamEntries)) return false;
      tcam_.get(index, w0, w1, payload0, payload1);
      return true;
    }
    inline void set(const int index,
                    const BitVector<kTcamWidth> &w0, const BitVector<kTcamWidth> &w1,
                    const uint8_t payload0, const uint8_t payload1) {
      if ((index >= 0) && (index < kTcamEntries)) {
        tcam_.set_word0_word1(index, w0, w1, payload0, payload1);
      }
    }
    // Allow head to be specified in all lookup funcs (including ones that
    // lookup Phvs). This allows a consistent headptr to be used at all stages
    // of the lookup process irrespective of any concurrent modifications to
    // the head value. If head is left at -1 the current head value is used
    // which is fine for single lookups.
    //
    inline int lookup_index(const BitVector<kTcamWidth>& s1, int head=-1) const {
      return tcam_.tcam_lookup_index(s1, head);
    }
    inline int lookup_pri(const BitVector<kTcamWidth>& s1, int head=-1) const {
      return tcam_.tcam_lookup_pri(s1, head);
    }
    inline int lookup(const BitVector<kTcamWidth>& s1, int head=-1) const {
      return tcam_.tcam_lookup(s1, head);
    }
    inline int lookup(const BitVector<kTcamWidth>& s1,
                      const int hi_pri, const int lo_pri,
                      int head=-1, bool promote_hits=true) const {
      return tcam_.tcam_lookup(s1, hi_pri, lo_pri, head, promote_hits);
    }
    inline void find_range(const int hit_pri, int *hi_pri, int *lo_pri, int head=-1) const {
      tcam_.tcam_find_range(hit_pri, hi_pri, lo_pri, head);
    }
    inline void max_range(int *hi_pri, int *lo_pri) const {
      *hi_pri = kTcamEntries-1; *lo_pri = 0;
    }

    // New funcs that lookup/find_range for whole BitVector of priorities
    inline int lookup(const BitVector<kTcamWidth>& s1,
                      const BitVector<kTcamEntries>& hits_in,
                      BitVector<kTcamEntries> *hits_out, int head=-1) const {
      return tcam_.tcam_lookup(s1, hits_in, hits_out, head);
    }
    inline void find_range(const BitVector<kTcamEntries>& hits_in,
                           BitVector<kTcamEntries> *hits_out, int head=-1) const {
      return tcam_.tcam_find_range(hits_in, hits_out, head);
    }



    inline int get_index(const int pri, int head=-1) const { return tcam_.get_index(pri, head); }
    inline int get_tcam_start() const { return tcam_.get_tcam_start(); }
    inline void set_tcam_start(const int head_index) {
      tcam_.set_tcam_start(head_index);
    }
    inline void set_bytemap_config(const int byte_index, uint8_t config) {
      tcam_.set_bytemap_config(byte_index, config);
    }
    inline void set_boundary(const int index, bool boundary=true) {
      tcam_.set_boundary(index, boundary);
    }
    inline void set_payload(const int index, uint8_t payload) {
      tcam_.set_payload(index, payload);
    }
    inline uint8_t get_payload(const int index) {
      return tcam_.get_payload(index);
    }
    inline uint8_t get_payload_pri(const int index, int head=-1) {
      return tcam_.get_payload_pri(index, head);
    }




    inline bool check_ingress_egress(bool ingress) {
      return ((ingress) ?is_ingress() || is_ghost() :is_egress());
    }
    inline void attach_ltcam(int ltcamIndex) { ltcams_ |=  (1u<<ltcamIndex); }
    inline void detach_ltcam(int ltcamIndex) { ltcams_ &= ~(1u<<ltcamIndex); }


    uint32_t make_match_address(const uint32_t hit_addr, int shift=0);
    void get_match_data(Phv *phv, BitVector<kTcamWidth> *match_data);
    int  lookup_index(Phv *phv, int head=-1);
    int  lookup_pri(Phv *phv, int head=-1);
    int  lookup(Phv *phv, int head=-1);
    int  lookup(Phv *phv, const int hi_pri, const int lo_pri,
                int head=-1, bool promote_hits=true);
    int  lookup(Phv *phv, const BitVector<kTcamEntries>& hits_in,
                BitVector<kTcamEntries> *hits_out, int head=-1);

    void update_config();
    bool pending_get(int *index, uint64_t *data0, uint64_t *data1);
    void pending_set(const int index, uint64_t data0, uint64_t data1);
    bool pending_lock(); // Returns true if managed to lock
    void pending_flush();
    void pending_unlock(bool unset=true);
    void pending_unset();



 private:
    int                             row_index_;
    int                             col_index_;
    int                             tcam_index_;
    uint32_t                        ltcams_;
    bool                            wide_match_;
    bool                            pending_locked_;
    int                             pending_index_;
    uint64_t                        pending_data0_;
    uint64_t                        pending_data1_;
    MauTcamRow                     *row_;
    Tcam3<kTcamEntries,kTcamWidth>  tcam_;
    MauTcamReg                      mau_tcam_reg_;
  };
}
#endif // _SHARED_MAU_TCAM_
