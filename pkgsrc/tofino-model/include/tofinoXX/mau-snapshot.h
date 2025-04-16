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

// MauSnapshot - Tofino/TofinoB0
// In shared/ because identical across these chips

#ifndef _TOFINOXX_MAU_SNAPSHOT_
#define _TOFINOXX_MAU_SNAPSHOT_

#include <mau-snapshot-common.h>

#include <register_includes/mau_snapshot_capture_subword16b_array_mutable.h>
#include <register_includes/mau_snapshot_capture_subword32b_hi_array_mutable.h>
#include <register_includes/mau_snapshot_capture_subword32b_lo_array_mutable.h>
#include <register_includes/mau_snapshot_capture_subword8b_array_mutable.h>
#include <register_includes/mau_snapshot_datapath_capture_array_mutable.h>
#include <register_includes/mau_snapshot_table_active_mutable.h>
#include <register_includes/mau_snapshot_next_table_out_array_mutable.h>


namespace MODEL_CHIP_NAMESPACE {


class MauSnapshot: public MauSnapshotCommon {

public:
    MauSnapshot(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    virtual ~MauSnapshot();

    inline void set_capture_subword32(const int i, uint32_t subword, bool valid) {
      RMT_ASSERT((i >= 0) && (i < 64));
      if (!kSnapshotCaptureValidBit) valid = false;
      uint32_t hi = ((subword >> 16) & 0xFFFFu) | ((valid) ?(1u<<16) :0u);
      uint32_t lo = ((subword >>  0) & 0xFFFFu);
      mau_snapshot_capture_subword32b_hi_[i/32].mau_snapshot_capture_subword32b_hi(i%32,hi);
      mau_snapshot_capture_subword32b_lo_[i/32].mau_snapshot_capture_subword32b_lo(i%32,lo);
    }
    inline void set_capture_subword16(const int i, uint32_t subword, bool valid) {
      RMT_ASSERT((i >= 0) && (i < 96));
      if (!kSnapshotCaptureValidBit) valid = false;
      uint32_t v = ((subword >> 0) & 0xFFFFu) | ((valid) ?(1u<<16) :0u);
      mau_snapshot_capture_subword16b_[i/48].mau_snapshot_capture_subword16b(i%48,v);
    }
    inline void set_capture_subword8(const int i, uint32_t subword, bool valid) {
      RMT_ASSERT((i >= 0) && (i < 64));
      if (!kSnapshotCaptureValidBit) valid = false;
      uint16_t v = (static_cast<uint16_t>((subword >> 0) & 0xFFu)) | ((valid) ?(1<<8) :0);
      mau_snapshot_capture_subword8b_[i/32].mau_snapshot_capture_subword8b(i%32,v);
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
    // Read/Write registers for snapshot captured state
    std::array< register_classes::MauSnapshotCaptureSubword32bHiArrayMutable,2 >  mau_snapshot_capture_subword32b_hi_;
    std::array< register_classes::MauSnapshotCaptureSubword32bLoArrayMutable,2 >  mau_snapshot_capture_subword32b_lo_;
    std::array< register_classes::MauSnapshotCaptureSubword16bArrayMutable,2 >    mau_snapshot_capture_subword16b_;
    std::array< register_classes::MauSnapshotCaptureSubword8bArrayMutable,2 >     mau_snapshot_capture_subword8b_;
    register_classes::MauSnapshotDatapathCaptureArrayMutable                      mau_snapshot_datapath_capture_;
    register_classes::MauSnapshotTableActiveMutable                               mau_snapshot_table_active_;
    register_classes::MauSnapshotNextTableOutArrayMutable                         mau_snapshot_next_table_out_;

};



}
#endif // _TOFINOXX_MAU_SNAPSHOT_
