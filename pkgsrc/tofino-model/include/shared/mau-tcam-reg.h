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

#ifndef _SHARED_MAU_TCAM_REG_
#define _SHARED_MAU_TCAM_REG_

#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <mau-chip-tcam-reg.h>

// Reg defs auto-generated from Semifore
//Removed in bfnregs 20150107_182406_7982_mau_dev
//#include <tofino/register_includes/tcam_priority_tailptr.h>
#include <register_includes/tcam_mode.h>
// Need this to figure out whether we should allow dirtcam on vbit
#include <register_includes/tcam_row_halfbyte_mux_ctl.h>




namespace MODEL_CHIP_NAMESPACE {

  class MauTcam;

  class MauTcamReg : public MauObject {
    static constexpr int kTcamWidth = MauDefs::kTcamWidth;
    static constexpr int kTcamBytes = (kTcamWidth+7)/8;
    static constexpr int kTcamIndexWidth = 6; // Tcam index in [0..63]
    static constexpr int kTcamAddressWidth  = MauDefs::kTcamAddressWidth;
    static constexpr int kTcamEntries = 1<<kTcamAddressWidth;

 public:
    MauTcamReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
               int rowIndex, int colIndex, int tcamIndex, MauTcam *mauTcam);
    virtual ~MauTcamReg();

    void reset();
    int  get_logical_table();
    int  get_search_bus();
    bool get_chain_out();
    bool get_match_output_enable();
    bool get_ingress();
    bool get_egress();
    bool get_ghost() { return mau_chip_tcam_reg_.get_ghost(); }
    uint8_t get_vpn();
    int  get_priority() { return mau_chip_tcam_reg_.get_priority(); }

    bool drives_ltcam(int ltcam, uint8_t powered_ltcams) {
      return mau_chip_tcam_reg_.drives_ltcam(ltcam, powered_ltcams);
    }
    bool get_ltcam_result_info(int ltcam, int *start_pos, int *n_entries, bool *bitmap) {
      return mau_chip_tcam_reg_.get_ltcam_result_info(ltcam, start_pos, n_entries, bitmap);
    }
    uint32_t compute_bitmap_result(int start, int entries, BitVector<kTcamEntries> *bv) {
      return mau_chip_tcam_reg_.compute_bitmap_result(start, entries, bv);
    }
    uint32_t get_hit_address(int ltcam, uint32_t hit_addr) {
      return mau_chip_tcam_reg_.get_hit_address(ltcam, get_vpn(), hit_addr);
    }

 private:
    //void tcam_tailptr_write_callback();
    void tcam_mode_write_callback();

    MauTcam                                *mau_tcam_;
    MauChipTcamReg                          mau_chip_tcam_reg_;
    int                                     row_;
    uint32_t                                prev_tcam_mode_;
    //register_classes::TcamPriorityTailptr tcam_priority_tailptr_;
    register_classes::TcamRowHalfbyteMuxCtl tcam_halfbyte_mux_ctl_;
    register_classes::TcamMode              tcam_mode_;
  };
}
#endif // _SHARED_MAU_TCAM_REG_
