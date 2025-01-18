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

#ifndef _JBAY_SHARED_MAU_SRAM_ROW_CHIP_REG_
#define _JBAY_SHARED_MAU_SRAM_ROW_CHIP_REG_

#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <register_adapters.h>

#include <register_includes/synth2port_vpn_ctl.h>
#include <register_includes/atomic_mod_shadow_ram_status_left_array_mutable.h>
#include <register_includes/atomic_mod_shadow_ram_status_right_array_mutable.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauSramRowChipReg : public MauObject {
   public:
    static constexpr int kType = RmtTypes::kRmtTypeMauSramRowReg; // close enough

    MauSramRowChipReg(RmtObjectManager *om, int pipeIndex, int mauIndex, int rowIndex,
                      MauSramRow *mauSramRow);
    virtual ~MauSramRowChipReg();


    // TODO: vpn is now 9 bits in JBay to support multi-stage FIFOs
    //
    // TODO: reverted back to Tofino-style - 6b with array index removed
    bool synth2port_vpn_valid(int vpn) {
      // TODO: need to choose correct array element - NO - see email
      return ((vpn >= synth2port_vpn_ctl_.synth2port_vpn_base()) &&
              (vpn <= synth2port_vpn_ctl_.synth2port_vpn_limit()));
    }

    void shadow_ram_status_rd_cb(int row, int col);

   private:
    MauSramRow                                                 *mau_sram_row_;
    register_classes::Synth2portVpnCtl                          synth2port_vpn_ctl_;
    register_classes::AtomicModShadowRamStatusLeftArrayMutable  atomic_mod_shadow_ram_status_l_;
    register_classes::AtomicModShadowRamStatusRightArrayMutable atomic_mod_shadow_ram_status_r_;

  };
}

#endif
