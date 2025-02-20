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

#ifndef _JBAY_SHARED_MAU_SNAPSHOT_
#define _JBAY_SHARED_MAU_SNAPSHOT_

#include <mau-snapshot-common.h>

#include <register_includes/mau_snapshot_config.h>
#include <register_includes/mau_snapshot_capture_subword16b_array2_mutable.h>
#include <register_includes/mau_snapshot_capture_subword32b_hi_array2_mutable.h>
#include <register_includes/mau_snapshot_capture_subword32b_lo_array2_mutable.h>
#include <register_includes/mau_snapshot_capture_subword8b_array2_mutable.h>
#include <register_includes/mau_snapshot_datapath_capture_array_mutable.h>
#include <register_includes/mau_snapshot_table_active_array_mutable.h>
#include <register_includes/mau_snapshot_next_table_out_array_mutable.h>
#include <register_includes/mau_snapshot_global_exec_out_array_mutable.h>
#include <register_includes/mau_snapshot_long_branch_out_array_mutable.h>
#include <register_includes/mau_snapshot_mpr_next_table_out_array_mutable.h>
#include <register_includes/mau_snapshot_mpr_global_exec_out_array_mutable.h>
#include <register_includes/mau_snapshot_mpr_long_branch_out_array_mutable.h>
#include <register_includes/mau_snapshot_meter_adr_array_mutable.h>
#include <register_includes/mau_snapshot_imem_logical_read_adr_array_mutable.h>
#include <register_includes/mau_snapshot_imem_logical_selector_fallback_mutable.h>


namespace MODEL_CHIP_NAMESPACE {


class MauSnapshot: public MauSnapshotCommon {

  static constexpr int  kIngress = MauPredicationCommon::kThreadIngress;
  static constexpr int  kEgress = MauPredicationCommon::kThreadEgress;
  static constexpr int  kGhost = MauPredicationCommon::kThreadGhost;

public:
    MauSnapshot(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    virtual ~MauSnapshot();

    inline int sizeoffset_to_index_unmapped(const int size, const int offset) {
      RMT_ASSERT((offset >= 0) && (offset < kPhvWordsUnmapped));
      int off_mapped = RmtDefs::map_mausnap_phv_index(offset);
      int index_mapped = sizeoffset_to_index(size, off_mapped);
      RMT_ASSERT((off_mapped < kPhvWords) && (index_mapped < kPhvWords));
      int index = RmtDefs::unmap_mausnap_phv_index(index_mapped);
      RMT_ASSERT((index >= 0) && (index < kPhvWordsUnmapped));
      return index;
    }
    inline void index_to_sizeoffset_unmapped(const int index, int *size, int *offset) {
      RMT_ASSERT((index >= 0) && (index < kPhvWordsUnmapped));
      int index_mapped = RmtDefs::map_mausnap_phv_index(index);
      RMT_ASSERT((index_mapped >= 0) && (index < kPhvWords));
      int off_mapped = -1;
      index_to_sizeoffset(index_mapped, size, &off_mapped);
      RMT_ASSERT((off_mapped >= 0) && (off_mapped < kPhvWords));
      int off = RmtDefs::unmap_mausnap_phv_index(off_mapped);
      RMT_ASSERT((off >= 0) && (off < kPhvWordsUnmapped));
      *offset = off;
    }

    inline void set_capture_subword32(const int i, uint32_t subword, bool valid) {
      RMT_ASSERT((i >= 0) && (i < 80));
      if (!kSnapshotCaptureValidBit) valid = false;
      uint32_t hi = ((subword >> 16) & 0xFFFFu) | ((valid) ?(1u<<16) :0u);
      uint32_t lo = ((subword >>  0) & 0xFFFFu);
      int half = i/40, phv_in_half = i%40;
      int alu_grp = phv_in_half/20, phv_in_grp = phv_in_half%20;
      mau_snapshot_capture_subword32b_hi_[half].mau_snapshot_capture_subword32b_hi(phv_in_grp,alu_grp,hi);
      mau_snapshot_capture_subword32b_lo_[half].mau_snapshot_capture_subword32b_lo(phv_in_grp,alu_grp,lo);
    }
    inline void set_capture_subword16(const int i, uint32_t subword, bool valid) {
      RMT_ASSERT((i >= 0) && (i < 120));
      if (!kSnapshotCaptureValidBit) valid = false;
      uint32_t v = ((subword >> 0) & 0xFFFFu) | ((valid) ?(1u<<16) :0u);
      int half = i/60, phv_in_half = i%60;
      int alu_grp = phv_in_half/20, phv_in_grp = phv_in_half%20;
      mau_snapshot_capture_subword16b_[half].mau_snapshot_capture_subword16b(phv_in_grp,alu_grp,v);
    }
    inline void set_capture_subword8(const int i, uint32_t subword, bool valid) {
      RMT_ASSERT((i >= 0) && (i < 80));
      if (!kSnapshotCaptureValidBit) valid = false;
      uint16_t v = (static_cast<uint16_t>((subword >> 0) & 0xFFu)) | ((valid) ?(1<<8) :0);
      int half = i/40, phv_in_half = i%40;
      int alu_grp = phv_in_half/20, phv_in_grp = phv_in_half%20;
      mau_snapshot_capture_subword8b_[half].mau_snapshot_capture_subword8b(phv_in_grp,alu_grp,v);
    }
    inline void set_capture_subword(const int index, uint32_t subword, bool valid) {
      RMT_ASSERT((index >= 0) && (index < kPhvWords));
      int size = 0, offset = -1;
      index_to_sizeoffset(index, &size, &offset);
      switch (size) {
        case 32: return set_capture_subword32(offset, subword, valid);
        case 16: return set_capture_subword16(offset, subword, valid);
        case 8:  return set_capture_subword8 (offset, subword, valid);
        default: RMT_ASSERT(0); break;
      }
    }

    bool is_thread_active(bool ingress, Phv *phv);
    void datapath_capture(bool ingress,
                          uint8_t from_prev, uint8_t timed, uint8_t here,
                          uint8_t error, uint8_t ing_pktver, uint8_t eg_pktver,
                          uint8_t thread_active, uint8_t trigger_thread,
                          uint8_t ghost_thread_active);
    void next_table_capture(bool ingress);
    void per_chip_capture(bool ingress, Phv *phv);


private:
    // Internal funcs
    void table_active_capture(bool ingress);
    void global_exec_capture(bool ingress);
    void long_branch_capture(bool ingress);
    void mpr_next_table_capture(bool ingress);
    void mpr_global_exec_capture(bool ingress);
    void mpr_long_branch_capture(bool ingress);
    void meter_adr_capture(bool ingress);
    void imem_logical_read_adr_capture(bool ingress);
    void imem_logical_selector_fallback_capture(bool ingress);


    register_classes::MauSnapshotConfig                                            mau_snapshot_config_;
    // Read/Write registers for snapshot captured state
    std::array< register_classes::MauSnapshotCaptureSubword32bHiArray2Mutable,2 >  mau_snapshot_capture_subword32b_hi_;
    std::array< register_classes::MauSnapshotCaptureSubword32bLoArray2Mutable,2 >  mau_snapshot_capture_subword32b_lo_;
    std::array< register_classes::MauSnapshotCaptureSubword16bArray2Mutable,2 >    mau_snapshot_capture_subword16b_;
    std::array< register_classes::MauSnapshotCaptureSubword8bArray2Mutable,2 >     mau_snapshot_capture_subword8b_;
    register_classes::MauSnapshotDatapathCaptureArrayMutable                       mau_snapshot_datapath_capture_;
    register_classes::MauSnapshotTableActiveArrayMutable                           mau_snapshot_table_active_;
    register_classes::MauSnapshotNextTableOutArrayMutable                          mau_snapshot_next_table_out_;
    register_classes::MauSnapshotGlobalExecOutArrayMutable                         mau_snapshot_global_exec_out_;
    register_classes::MauSnapshotLongBranchOutArrayMutable                         mau_snapshot_long_branch_out_;
    register_classes::MauSnapshotMprNextTableOutArrayMutable                       mau_snapshot_mpr_next_table_out_;
    register_classes::MauSnapshotMprGlobalExecOutArrayMutable                      mau_snapshot_mpr_global_exec_out_;
    register_classes::MauSnapshotMprLongBranchOutArrayMutable                      mau_snapshot_mpr_long_branch_out_;
    register_classes::MauSnapshotMeterAdrArrayMutable                              mau_snapshot_meter_adr_;
    register_classes::MauSnapshotImemLogicalReadAdrArrayMutable                    mau_snapshot_imem_logical_read_adr_;
    register_classes::MauSnapshotImemLogicalSelectorFallbackMutable                mau_snapshot_imem_logical_selector_fallback_;
};



}
#endif // _JBAY_SHARED_MAU_SNAPSHOT_
