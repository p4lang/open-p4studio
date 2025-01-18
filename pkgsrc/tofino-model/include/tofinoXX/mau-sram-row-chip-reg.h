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

#ifndef _TOFINOXX_MAU_SRAM_ROW_CHIP_REG_
#define _TOFINOXX_MAU_SRAM_ROW_CHIP_REG_

#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <bitvector.h>
#include <register_adapters.h>

#include <register_includes/synth2port_vpn_ctl.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauSramRowChipReg : public MauObject {
   public:
    static constexpr int kType = RmtTypes::kRmtTypeMauSramRowReg; // close enough

    MauSramRowChipReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                      int rowIndex, MauSramRow *mauSramRow) :
        MauObject(om, pipeIndex, mauIndex, kType, rowIndex),
        synth2port_vpn_ctl_(default_adapter(synth2port_vpn_ctl_,chip_index(),pipeIndex,mauIndex,rowIndex))
    {
      synth2port_vpn_ctl_.reset();
    }
    virtual ~MauSramRowChipReg() {}

    bool synth2port_vpn_valid(int vpn) {
      return ((vpn >= synth2port_vpn_ctl_.synth2port_vpn_base()) &&
              (vpn <= synth2port_vpn_ctl_.synth2port_vpn_limit()));
    }

   private:
    register_classes::Synth2portVpnCtl            synth2port_vpn_ctl_;

  };
}

#endif
