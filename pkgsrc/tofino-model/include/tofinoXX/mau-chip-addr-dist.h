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

#ifndef _TOFINOXX_MAU_CHIP_ADDR_DIST_
#define _TOFINOXX_MAU_CHIP_ADDR_DIST_

#include <rmt-defs.h>
#include <mau-defs.h>
#include <register_adapters.h>

#include <register_includes/deferred_ram_ctl_array2.h>
#include <register_includes/deferred_oflo_ctl.h>
#include <register_includes/meter_color_logical_to_phys_ixbar_ctl_array.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauChipAddrDist {
   public:
    MauChipAddrDist(int chipIndex, int pipeIndex, int mauIndex, Mau *mau, MauAddrDist *mad) :
        deferred_ram_ctl_(default_adapter(deferred_ram_ctl_,chipIndex,pipeIndex,mauIndex)),
        deferred_oflo_ctl_(default_adapter(deferred_oflo_ctl_,chipIndex,pipeIndex,mauIndex)),
        meter_color_logical_to_phys_ixbar_(default_adapter(meter_color_logical_to_phys_ixbar_,chipIndex,pipeIndex,mauIndex))
    {
      deferred_ram_ctl_.reset();
      deferred_oflo_ctl_.reset();
      meter_color_logical_to_phys_ixbar_.reset();
    }
    ~MauChipAddrDist() { }

    bool    get_deferred_ram_en(int s_or_m, int idx)      { return deferred_ram_ctl_.deferred_ram_en(s_or_m, idx); }
    bool    get_deferred_ram_thread(int s_or_m, int idx)  { return deferred_ram_ctl_.deferred_ram_thread(s_or_m, idx); }
    uint8_t get_deferred_ram_err_ctl(int s_or_m, int idx) { return 0; } // 0 on Tofino
    uint8_t get_deferred_oflo_ctl()                       { return deferred_oflo_ctl_.deferred_oflo_ctl(); }

    int get_meter_color_alu(int lt) {
      if (meter_color_logical_to_phys_ixbar_.enabled_2bit_muxctl_enable(lt) == 0) return -1;
      return meter_color_logical_to_phys_ixbar_.enabled_2bit_muxctl_select(lt);
    }
    uint32_t get_meter_color_alu_rows(int lt) {
      int alu = get_meter_color_alu(lt);
      return (alu >= 0) ?MauMeterAlu::map_meter_alus_to_rows(1u<<alu) :0u;
    }
    bool vpn_range_check_meter(int lrow, uint32_t addr) { return true; }
    bool vpn_range_check_stats(int lrow, uint32_t addr) { return true; }
    int  get_meter_sweep_subword_shift(int alu)         { return 0; }
    int  get_meter_sweep_subwords(int alu)              { return meter_sweep_subwords; }
    int meter_sweep_subwords = 1;
    int  get_meter_sweep_op4(int alu)                   { return Address::kMeterOp4Sweep; }



   private:
    register_classes::DeferredRamCtlArray2                 deferred_ram_ctl_;
    register_classes::DeferredOfloCtl                      deferred_oflo_ctl_;
    register_classes::MeterColorLogicalToPhysIxbarCtlArray meter_color_logical_to_phys_ixbar_;
  };
}

#endif
