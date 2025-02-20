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

#ifndef _SHARED_MAU_SNAPSHOT_COMMON_
#define _SHARED_MAU_SNAPSHOT_COMMON_

#include <phv.h>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <mau-dependencies.h>

#include <register_includes/phv_egress_thread_array2.h>
#include <register_includes/phv_ingress_thread_array2.h>

#include <register_includes/mau_snapshot_match_subword16b_array2.h>
#include <register_includes/mau_snapshot_match_subword32b_hi_array2.h>
#include <register_includes/mau_snapshot_match_subword32b_lo_array2.h>
#include <register_includes/mau_snapshot_match_subword8b_array2.h>
#include <register_includes/mau_snapshot_config.h>
#include <register_includes/mau_snapshot_timestamp_trigger_hi.h>
#include <register_includes/mau_snapshot_timestamp_trigger_lo.h>

#include <register_includes/mau_snapshot_physical_exact_match_hit_address_array_mutable.h>
#include <register_includes/mau_snapshot_physical_tcam_hit_address_array_mutable.h>
#include <register_includes/mau_snapshot_gateway_table_inhibit_logical_mutable.h>
#include <register_includes/mau_snapshot_logical_table_hit_mutable.h>

#include <register_includes/mau_fsm_snapshot_cur_stateq_array_mutable.h>
#include <register_includes/mau_snapshot_timestamp_hi_mutable.h>
#include <register_includes/mau_snapshot_timestamp_lo_mutable.h>

#include <register_includes/exact_match_phys_result_en_array.h>
#include <register_includes/exact_match_phys_result_thread_array.h>
#include <register_includes/tind_bus_prop_array.h>
#include <register_includes/intr_status_mau_snapshot_mutable.h>



namespace MODEL_CHIP_NAMESPACE {


  class MauSnapshotCommon : public MauObject {
    static constexpr int  HI = 1;
    static constexpr int  LO = 0;
    static constexpr int  NEITHER = -1;
    static constexpr int  kType = RmtTypes::kRmtTypeMauSnapshot;
    static constexpr int  kMatchOutputBusesPerMau = MauDefs::kMatchOutputBusesPerMau;
    static constexpr int  kTindOutputBusesPerMau = MauDefs::kTindOutputBusesPerMau;
    static constexpr int  kPassive = 0;
    static constexpr int  kArmed = 1;
    static constexpr int  kTriggerHappy = 2;
    static constexpr int  kFull = 3;


 public:
    static bool kSnapshotCompareValidBit; // Defined in rmt-config.cpp
    static bool kSnapshotCaptureValidBit;
    static bool kSnapshotUsePhvTime;
    static bool kSnapshotMaskThreadFields;

    static constexpr int  kPhvWords = Phv::kWordsMax;
    static constexpr int  kPhvWordsUnmapped = Phv::kWordsMaxUnmapped;
    static constexpr int  kLogicalTables = MauDefs::kLogicalTablesPerMau;

    MauSnapshotCommon(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    virtual ~MauSnapshotCommon();

    void reset_resources();
    bool phv_match_ingress(Phv *phv);
    bool phv_match_egress(Phv *phv);
    bool phv_match(bool ingress, Phv *phv);
    void phv_capture_ingress(Phv *phv);
    void phv_capture_egress(Phv *phv);
    void phv_capture(bool ingress, Phv *phv);
    bool maybe_snapshot(Phv *match_phv, Phv *action_phv);
    void finalize_snapshot(Phv *output_phv,
                           int ingress_nxt_tab, int egress_nxt_tab);
    bool snapshot_captured();


    inline uint64_t get_mask(const int width) {
      // Extra -1 in shift/mask calc determines whether we compare MSB valid bit
      int shift = (kSnapshotCompareValidBit) ?64-width-1 :64-width;
      return UINT64_C(0xFFFFFFFFFFFFFFFF) >> shift;
    }
    inline int sizeoffset_to_index(const int size, const int offset) {
      RMT_ASSERT((offset >= 0) && (offset < kPhvWords));
      switch (size) {
        case 32: return Phv::map_word_rel32_to_abs(offset);
        case 16: return Phv::map_word_rel16_to_abs(offset);
        case 8:  return Phv::map_word_rel8_to_abs(offset);
        default: RMT_ASSERT(0); break;
      }
    }
    inline void index_to_sizeoffset(const int index, int *size, int *offset) {
      RMT_ASSERT((size != NULL) && (offset != NULL));
      RMT_ASSERT((index >= 0) && (index < kPhvWords));
      switch (Phv::which_width(index)) {
        case 32: *size = 32; *offset = Phv::map_word_abs_to_rel32(index); return;
        case 16: *size = 16; *offset = Phv::map_word_abs_to_rel16(index); return;
        case 8:  *size =  8; *offset = Phv::map_word_abs_to_rel8(index);  return;
        default: RMT_ASSERT(0); break;
      }
    }

    // PER-CHIP implementation override
    virtual int sizeoffset_to_index_unmapped(const int size, const int offset) {
      return sizeoffset_to_index(size, offset);
    }
    virtual void index_to_sizeoffset_unmapped(const int index, int *size, int *offset) {
      index_to_sizeoffset(index, size, offset);
    }
    // PER-CHIP implementation
    virtual bool is_thread_active(bool ingress, Phv *phv) = 0;
    virtual void datapath_capture(bool ingress,
                                  uint8_t from_prev, uint8_t timed, uint8_t here,
                                  uint8_t error, uint8_t ing_pktver, uint8_t eg_pktver,
                                  uint8_t thread_active, uint8_t trigger_thread,
                                  uint8_t ghost_thread_active) = 0;
    virtual void set_capture_subword(const int index, uint32_t subword, bool valid) = 0;
    virtual void next_table_capture(bool ingress) = 0;
    virtual void per_chip_capture(bool ingress, Phv *phv) = 0;


 private:
    // Same func as in bitvector.h
    inline bool ternary_match_s0s1(const uint64_t w0, const uint64_t w1,
                                   const uint64_t s0, const uint64_t s1) {
      return ((((~w0) & s0 ) | ((~w1) & s1 )) == UINT64_C(0));
    }
    inline bool ternary_match(const uint64_t w0, const uint64_t w1,
                              const uint64_t s1, const int width) {
      // Extra -1 in shift/mask calc determines whether we compare MSB valid bit
      int shift = (kSnapshotCompareValidBit) ?64-width-1 :64-width;
      uint64_t mask = UINT64_C(0xFFFFFFFFFFFFFFFF) >> shift;
      return ternary_match_s0s1(w0, w1, (~s1 & mask), (s1 & mask));
    }

    inline uint64_t get_match_subword32(const int i, const int w01) {
      RMT_ASSERT((i >= 0) && (i < 64) && ((w01 == 0) || (w01 == 1)));
      // We extract valid bit but may not use it in compare (see kSnapshotCompareValidBit)
      uint32_t hi = mau_snapshot_match_subword32b_hi_.mau_snapshot_match_subword32b_hi(i,w01);
      uint16_t lo = mau_snapshot_match_subword32b_lo_.mau_snapshot_match_subword32b_lo(i,w01);
      return (static_cast<uint64_t>(hi & 0x1FFFFu) << 16) | (static_cast<uint64_t>(lo) << 0);
    }
    inline uint64_t get_match_subword16(const int i, const int w01) {
      RMT_ASSERT((i >= 0) && (i < 96) && ((w01 == 0) || (w01 == 1)));
      // We extract valid bit but may not use it in compare (see kSnapshotCompareValidBit)
      uint32_t v = mau_snapshot_match_subword16b_.mau_snapshot_match_subword16b(i,w01);
      return static_cast<uint64_t>(v & 0x1FFFFu);
    }
    inline uint64_t get_match_subword8(const int i, const int w01) {
      RMT_ASSERT((i >= 0) && (i < 64) && ((w01 == 0) || (w01 == 1)));
      // We extract valid bit but may not use it in compare (see kSnapshotCompareValidBit)
      uint16_t v = mau_snapshot_match_subword8b_.mau_snapshot_match_subword8b(i,w01);
      return static_cast<uint64_t>(v & 0x1FF);
    }
    inline uint64_t get_match_subword(const int index, const int w01) {
      RMT_ASSERT((index >= 0) && (index < kPhvWords));
      int size = 0, offset = -1;
      index_to_sizeoffset_unmapped(index, &size, &offset);
      switch (size) {
        case 32: return get_match_subword32(offset, w01);
        case 16: return get_match_subword16(offset, w01);
        case 8:  return get_match_subword8 (offset, w01);
        default: RMT_ASSERT(0); break;
      }
      return 0; // can theoretically get here if NDEBUG defined
    }

    inline int get_fsm(bool ingress) {
      uint8_t v = mau_fsm_snapshot_cur_stateq_.mau_fsm_snapshot_cur_stateq(ingress ?0 :1);
      return static_cast<int>(v);
    }
    inline void set_fsm(bool ingress, int state) {
      RMT_ASSERT((state == kPassive) || (state == kArmed) ||
                 (state == kTriggerHappy) || (state == kFull));
      uint8_t v = static_cast<uint8_t>(state & 0x3);
      mau_fsm_snapshot_cur_stateq_.mau_fsm_snapshot_cur_stateq(ingress ?0 :1, v);
    }

    inline uint64_t get_trigger_timestamp()  {
      uint16_t hi = mau_snapshot_timestamp_trigger_hi_.mau_snapshot_timestamp_trigger_hi();
      uint32_t lo = mau_snapshot_timestamp_trigger_lo_.mau_snapshot_timestamp_trigger_lo();
      return (static_cast<uint64_t>(hi) << 32) | (static_cast<uint64_t>(lo) << 0);
    }
    inline uint64_t get_timestamp()  {
      uint16_t hi = mau_snapshot_timestamp_hi_.mau_snapshot_timestamp_hi();
      uint32_t lo = mau_snapshot_timestamp_lo_.mau_snapshot_timestamp_lo();
      return (static_cast<uint64_t>(hi) << 32) | (static_cast<uint64_t>(lo) << 0);
    }
    inline void set_timestamp(uint64_t T)  {
      uint16_t hi = static_cast<uint16_t>((T >> 32) & 0xFFFF);
      uint32_t lo = static_cast<uint32_t>((T >>  0) & 0xFFFFFFFFu);
      mau_snapshot_timestamp_hi_.mau_snapshot_timestamp_hi(hi);
      mau_snapshot_timestamp_lo_.mau_snapshot_timestamp_lo(lo);
    }
    inline bool is_timestamp_snapshot_enabled() {
      return ((mau_snapshot_config_.timebased_snapshot_ingress_enable() == 1) ||
              (mau_snapshot_config_.timebased_snapshot_egress_enable() == 1));
    }
    inline bool is_timestamp_snapshot_enabled(bool ingress) {
      if (ingress)
        return (mau_snapshot_config_.timebased_snapshot_ingress_enable() == 1);
      else
        return (mau_snapshot_config_.timebased_snapshot_egress_enable() == 1);
    }

    inline bool snapshot_both() {
      return (mau_snapshot_config_.snapshot_both_threads() == 1);
    }

    inline bool is_pending_snapshot(bool ingress) {
      return snapshot_pending_[ingress ?0 :1];
    }
    inline void set_pending_snapshot(bool ingress, bool tf=true) {
      snapshot_pending_[ingress ?0 :1] = tf;
    }
    inline void reset_pending_snapshot(bool ingress) {
      set_pending_snapshot(ingress, false);
    }

    inline void set_interrupt(bool ingress) {
      uint8_t ie = (ingress) ?0 :1;
      uint8_t v = intr_status_mau_snapshot_.intr_snapshot_trigger();
      intr_status_mau_snapshot_.intr_snapshot_trigger(v | (1<<ie));
    }

    // Register write callbacks
    void phv_ingress_change_callback(uint32_t i, uint32_t j);
    void phv_egress_change_callback(uint32_t i, uint32_t j);
    void match_subword_change_callback(uint32_t i, uint32_t j, int size, int hilo=NEITHER);
    void config_change_callback();
    void timestamp_change_callback(int hilo);

    // Internal funcs
    uint64_t get_time_now(Phv *phv);
    bool timestamp_triggered_snapshot(bool ingress, Phv *phv, bool advance_timestamp);
    void harvest_phv_ingress_egress_thread_changes();
    void harvest_match_subword_changes();
    void recalc_ingress_egress_valid();
    bool phv_match(Phv *phv, const BitVector<kPhvWords> &selector);
    void phv_capture(Phv *phv, const BitVector<kPhvWords> &selector);
    void physical_bus_hit_addr_capture(bool ingress);
    void logical_table_info_capture(bool ingress);




    bool                                              phv_ingress_changed_;
    bool                                              phv_egress_changed_;
    bool                                              w0w1_changed_;
    BitVector<kPhvWords>                              ingress_vector_;
    BitVector<kPhvWords>                              egress_vector_;
    BitVector<kPhvWords>                              w0w1_change_vector_;
    BitVector<kPhvWords>                              w0w1_zero_vector_;
    BitVector<kPhvWords>                              w0w1_compare_vector_;
    BitVector<kPhvWords>                              ingress_zero_vector_;
    BitVector<kPhvWords>                              egress_zero_vector_;
    BitVector<kPhvWords>                              ingress_valid_vector_;
    BitVector<kPhvWords>                              egress_valid_vector_;
    uint64_t                                          w0_[kPhvWords];
    uint64_t                                          w1_[kPhvWords];
    uint64_t                                          TS_at_T_start_;
    uint64_t                                          T_start_;
    uint64_t                                          T_now_last_;
    bool                                              snapshot_pending_[2];
    bool                                              snapshot_capture_[2];
    bool                                              timestamp_snapshot_enabled_[2];
    // Significant non-snapshot registers
    register_classes::PhvIngressThreadArray2              phv_ingress_thread_;
    register_classes::PhvEgressThreadArray2               phv_egress_thread_;
    // Snapshot configuration registers
    register_classes::MauSnapshotMatchSubword32bHiArray2  mau_snapshot_match_subword32b_hi_;
    register_classes::MauSnapshotMatchSubword32bLoArray2  mau_snapshot_match_subword32b_lo_;
    register_classes::MauSnapshotMatchSubword16bArray2    mau_snapshot_match_subword16b_;
    register_classes::MauSnapshotMatchSubword8bArray2     mau_snapshot_match_subword8b_;
    register_classes::MauSnapshotConfig                   mau_snapshot_config_;
    register_classes::MauSnapshotTimestampTriggerHi       mau_snapshot_timestamp_trigger_hi_;
    register_classes::MauSnapshotTimestampTriggerLo       mau_snapshot_timestamp_trigger_lo_;
    register_classes::MauSnapshotPhysicalExactMatchHitAddressArrayMutable  mau_snapshot_physical_exact_match_hit_address_;
    register_classes::MauSnapshotPhysicalTcamHitAddressArrayMutable        mau_snapshot_physical_tcam_hit_address_;
    register_classes::MauSnapshotGatewayTableInhibitLogicalMutable         mau_snapshot_gateway_table_inhibit_logical_;
    register_classes::MauSnapshotLogicalTableHitMutable                    mau_snapshot_logical_table_hit_;
    // Read/Write registers describing snapshot capture status
    register_classes::MauFsmSnapshotCurStateqArrayMutable     mau_fsm_snapshot_cur_stateq_;
    register_classes::MauSnapshotTimestampHiMutable           mau_snapshot_timestamp_hi_;
    register_classes::MauSnapshotTimestampLoMutable           mau_snapshot_timestamp_lo_;
    register_classes::ExactMatchPhysResultEnArray             exact_match_phys_result_en_;
    register_classes::ExactMatchPhysResultThreadArray         exact_match_phys_result_thread_;
    register_classes::TindBusPropArray                        tind_bus_prop_;
    register_classes::IntrStatusMauSnapshotMutable            intr_status_mau_snapshot_;

  }; // MauSnapshot


}

#endif // _SHARED_MAU_SNAPSHOT_COMMON_
