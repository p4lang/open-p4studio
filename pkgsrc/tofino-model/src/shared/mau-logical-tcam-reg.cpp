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

#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-logical-tcam-reg.h>
#include <mau-logical-tcam.h>
#include <mau-sram-column-reg.h>
#include <mau-sram-column.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

  MauLogicalTcamReg::MauLogicalTcamReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                                       int ltcamIndex, MauLogicalTcam *mauLogicalTcam)
      : MauObject(om, pipeIndex, mauIndex, kType, ltcamIndex), mau_logical_tcam_(mauLogicalTcam),
        match_to_logical_table_ixbar_outputmap_(default_adapter(match_to_logical_table_ixbar_outputmap_,chip_index(), pipeIndex, mauIndex)),
        tcam_error_detect_enable_(default_adapter(tcam_error_detect_enable_,chip_index(), pipeIndex, mauIndex)),
        tcam_match_adr_shift_(default_adapter(tcam_match_adr_shift_,chip_index(), pipeIndex, mauIndex, ltcamIndex)),
        tcam_match_addr_to_physical_oxbar_outputmap_(default_adapter(tcam_match_addr_to_physical_oxbar_outputmap_,chip_index(), pipeIndex, mauIndex)),
        tcam_hit_to_logical_table_ixbar_outputmap_(default_adapter(tcam_hit_to_logical_table_ixbar_outputmap_,chip_index(), pipeIndex, mauIndex, ltcamIndex,
                                                  [this](){this->tcam_ixbar_write_callback();})) {

    static_assert( (kTindOutputBusesPerMau <= 16), "Tind buses must fit in a uint16_t");
    static_assert( ((kLogicalTcamsPerMau % 2) == 0), "Recode paired_ltcam to handle odd number ltcams");

    match_to_logical_table_ixbar_outputmap_.reset();
    tcam_error_detect_enable_.reset();
    tcam_match_adr_shift_.reset();
    tcam_match_addr_to_physical_oxbar_outputmap_.reset();
    tcam_hit_to_logical_table_ixbar_outputmap_.reset();
  }
  MauLogicalTcamReg::~MauLogicalTcamReg() {    
  }


  uint16_t MauLogicalTcamReg::get_tind_buses() {
    uint16_t tind_buses = 0;
    int ltcam = mau_logical_tcam_->ltcam_index();
    for (int tind_bus = 0; tind_bus < MauDefs::kTindOutputBusesPerMau; tind_bus++) {
      if ((tcam_match_addr_to_physical_oxbar_outputmap_.enabled_3bit_muxctl_enable(tind_bus)) &&
          (tcam_match_addr_to_physical_oxbar_outputmap_.enabled_3bit_muxctl_select(tind_bus) == ltcam))
        tind_buses |= (1<<tind_bus);
    }
    return tind_buses;
  }

  int MauLogicalTcamReg::get_logical_table() {
    if (!tcam_hit_to_logical_table_ixbar_outputmap_.enabled_4bit_muxctl_enable()) return -1;
    return tcam_hit_to_logical_table_ixbar_outputmap_.enabled_4bit_muxctl_select();
  }

  int MauLogicalTcamReg::paired_ltcam(bool ingress) {
    int ltcam = mau_logical_tcam_->ltcam_index();
    RMT_ASSERT((ltcam >= 0) && (ltcam < kLogicalTcamsPerMau));
    int ltclo = ltcam % (kLogicalTcamsPerMau/2);
    int ltchi = ltclo + (kLogicalTcamsPerMau/2);
    int ltoth = (ltcam == ltclo) ?ltchi :ltclo;
    bool en = (((tcam_error_detect_enable_.tcam_logical_channel_error_detect_enable() >> ltclo) & 1) == 1);
    bool ing = (((tcam_error_detect_enable_.tcam_logical_channel_thread() >> ltclo) & 1) == 0);
    // If ltcam enabled for error detect and right (ingress/egress) thread, return paired ltcam else -1
    return ((en) && (ing == ingress)) ?ltoth :-1;
  }

  int MauLogicalTcamReg::tcam_match_adr_shift() {
    return tcam_match_adr_shift_.tcam_match_adr_shift();
  }

  void MauLogicalTcamReg::tcam_ixbar_write_callback() {
    int old_logtab = mau_logical_tcam_->get_logical_table();
    int new_logtab = get_logical_table();
    if (old_logtab != new_logtab) {
      mau_logical_tcam_->update_logical_table(new_logtab);
    }
  }
}
