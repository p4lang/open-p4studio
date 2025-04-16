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

#include <mau.h>
#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-tcam-reg.h>
#include <mau-tcam.h>
#include <register_adapters.h>



namespace MODEL_CHIP_NAMESPACE {
  MauTcamReg::MauTcamReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                         int rowIndex, int colIndex, int tcamIndex,
                         MauTcam *mauTcam)
      : MauObject(om, pipeIndex, mauIndex),
        mau_tcam_(mauTcam),
        mau_chip_tcam_reg_(chip_index(),pipeIndex,mauIndex,rowIndex,colIndex,mauTcam),
        row_(rowIndex), prev_tcam_mode_(0xFFFFFFFF),
        //Removed in bfnregs 20150107_182406_7982_mau_dev
        //tcam_priority_tailptr_(chip_index(), pipeIndex, mauIndex, colIndex, rowIndex,
        //                       [this](){this->tcam_tailptr_write_callback();}),
        // Since TCAM CRB, search_bus === col so can simply instantiate
        // halfbyte_mux_ctl corresponding to our col,row
        tcam_halfbyte_mux_ctl_(default_adapter(tcam_halfbyte_mux_ctl_,chip_index(),
                                               pipeIndex, mauIndex, colIndex, rowIndex,
                                               [this](){this->tcam_mode_write_callback();})),
        tcam_mode_(default_adapter(tcam_mode_,chip_index(),
                                   pipeIndex, mauIndex, colIndex, rowIndex,
                                   [this](){this->tcam_mode_write_callback();})) {
    static_assert( (kTcamBytes == 6), "Not enough dirtcam control bits");
    reset();
    prev_tcam_mode_ = 0u;
  }
  MauTcamReg::~MauTcamReg() {
  }
  void MauTcamReg::reset() {
    //tcam_priority_tailptr_.reset();
    tcam_halfbyte_mux_ctl_.reset();
    tcam_mode_.reset();
  }


//void MauTcamReg::tcam_tailptr_write_callback() {
//    uint16_t tailptr = tcam_priority_tailptr_.tcam_priority_tailptr();
//    uint16_t head = (tailptr==0) ? (kTcamEntries-1) : (tailptr-1);
//    // tailptr N means tailptr is entry N-1
//    mau_tcam_->set_tcam_start(head);
//}

  void MauTcamReg::tcam_mode_write_callback() {
    if (prev_tcam_mode_ == 0xFFFFFFFF) return;
    // tcam_data1_select unused since TCAM CRB changes - so set d = 0u
    uint32_t d = 0u; //static_cast<uint32_t>(tcam_mode_.tcam_data1_select() & 0x1);
    uint32_t c = static_cast<uint32_t>(tcam_mode_.tcam_chain_out_enable() & 0x1);
    uint32_t i = static_cast<uint32_t>(tcam_mode_.tcam_ingress() & 0x1);
    uint32_t e = static_cast<uint32_t>(tcam_mode_.tcam_egress() & 0x1);
    uint32_t m = static_cast<uint32_t>(tcam_mode_.tcam_match_output_enable() & 0x1);
    uint32_t v = static_cast<uint32_t>(tcam_mode_.tcam_vpn() & 0x3F);
    uint32_t log_table = static_cast<uint32_t>(tcam_mode_.tcam_logical_table() & 0xF);
    uint32_t data_dirtcam_mode = (tcam_mode_.tcam_data_dirtcam_mode() & 0x3FF);
    uint32_t vbit_dirtcam_mode = (tcam_mode_.tcam_vbit_dirtcam_mode() & 0x3);
    if (tcam_halfbyte_mux_ctl_.tcam_row_halfbyte_mux_ctl_enable() == 0x1) {
      uint8_t sel = tcam_halfbyte_mux_ctl_.tcam_row_halfbyte_mux_ctl_select();
      if ((sel == 2) || (sel == 3)) {
        // Halfbyte muxctl is selecting valid bit or valid/version
        // In this case vbit dirtcam mux is bypassed so warn if used and ignore
        if (vbit_dirtcam_mode != 0u) {
          RMT_LOG_OBJ(mau_tcam_, RmtDebug::warn(),
                      "Ignoring tcam_vbit_dirtcam_mode - version/valid in use!\n");
        }
        vbit_dirtcam_mode = 0u;
      }
    }
    uint32_t dirtcam_mode = (vbit_dirtcam_mode << 10) | (data_dirtcam_mode);
    uint32_t new_mode = 0u;
    new_mode |= (log_table << 23) | (dirtcam_mode << 0);
    new_mode |= (v<<17) | (m<<16) | (e<<15) | (i<<14) | (c<<13) | (d<<12);
    RMT_ASSERT(new_mode != 0xFFFFFFFF);

    if (new_mode != prev_tcam_mode_) {
      // Each 2 bits of composite dirtcam_mode control a byte of TCAM
      for (int byte = 0; byte < kTcamBytes; byte++) {
        mau_tcam_->set_bytemap_config(byte, static_cast<uint8_t>(dirtcam_mode & 0x3));
        dirtcam_mode >>= 2;
      }
      mau_tcam_->update_config();
      prev_tcam_mode_ = new_mode;
      // Also call up into MAU so it knows a TCAM is in use
      mau_tcam_->mau()->tcam_config_changed();
    }
  }

  int MauTcamReg::get_logical_table() {
    return static_cast<int>(tcam_mode_.tcam_logical_table());
  }
  int MauTcamReg::get_search_bus() {
    // No more tcam_data1_select since TCAM CRB changes
    // Now search bus simply determined by column
    //return static_cast<int>(tcam_mode_.tcam_data1_select() & 0x1);
    int col = mau_tcam_->col_index();
    RMT_ASSERT((col == 0) || (col == 1));
    return col;
  }
  bool MauTcamReg::get_chain_out() {
    return ((tcam_mode_.tcam_chain_out_enable() & 0x1) != 0);
  }
  bool MauTcamReg::get_match_output_enable() {
    return ((tcam_mode_.tcam_match_output_enable() & 0x1) != 0);
  }
  bool MauTcamReg::get_ingress() {
    return ((tcam_mode_.tcam_ingress() & 0x1) != 0);
  }
  bool MauTcamReg::get_egress() {
    return ((tcam_mode_.tcam_egress() & 0x1) != 0);
  }
  uint8_t MauTcamReg::get_vpn() {
    return tcam_mode_.tcam_vpn();
  }
  // Since WIP get_priority() func is per-chip
  //
  //int MauTcamReg::get_priority() {
  //  Since Tofino TCAM CRB changes, priority simply determined by
  //  physical ordering NOT by VPN
  //  //return (get_vpn() << kTcamIndexWidth) | mau_tcam_->tcam_index();
  //  return mau_tcam_->tcam_index();
  //}


}
