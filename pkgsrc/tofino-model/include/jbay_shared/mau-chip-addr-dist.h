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

#ifndef _JBAY_SHARED_MAU_CHIP_ADDR_DIST_
#define _JBAY_SHARED_MAU_CHIP_ADDR_DIST_

#include <register_adapters.h>

#include <register_includes/deferred_ram_ctl_array2.h>
#include <register_includes/deferred_oflo_ctl.h>
#include <register_includes/meter_color_logical_to_phys_icxbar_ctl_array.h>
#include <register_includes/mau_meter_alu_vpn_range_array.h>
#include <register_includes/mau_stats_alu_vpn_range_array.h>
#include <register_includes/meter_sweep_num_subwords_array.h>
#include <register_includes/meter_sweep_cmd_ovr_ctl_array.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauAddrDist;

  class MauChipAddrDist {

 public:
    MauChipAddrDist(int chipIndex, int pipeIndex, int mauIndex, Mau *mau, MauAddrDist *mad) :
        ctor_running_(true), mau_addr_dist_(mad), subw_shift_(), sweep_op4_(),
        deferred_ram_ctl_(default_adapter(deferred_ram_ctl_,chipIndex,pipeIndex,mauIndex)),
        deferred_oflo_ctl_(default_adapter(deferred_oflo_ctl_,chipIndex,pipeIndex,mauIndex)),
        meter_color_logical_to_phys_icxbar_(default_adapter(meter_color_logical_to_phys_icxbar_,chipIndex,pipeIndex,mauIndex)),
        mau_meter_alu_vpn_range_(default_adapter(mau_meter_alu_vpn_range_,chipIndex,pipeIndex,mauIndex)),
        mau_stats_alu_vpn_range_(default_adapter(mau_stats_alu_vpn_range_,chipIndex,pipeIndex,mauIndex)),
        meter_sweep_num_subwords_(default_adapter(meter_sweep_num_subwords_,chipIndex,pipeIndex,mauIndex,
                                                  [this](uint32_t i){this->sweep_write_callback(i); })),
        meter_sweep_cmd_ovr_ctl_(default_adapter(meter_sweep_cmd_ovr_ctl_,chipIndex,pipeIndex,mauIndex,
                                                 [this](uint32_t i){this->sweep_write_callback(i); }))
    {
      deferred_ram_ctl_.reset();
      deferred_oflo_ctl_.reset();
      meter_color_logical_to_phys_icxbar_.reset();
      mau_meter_alu_vpn_range_.reset();
      mau_stats_alu_vpn_range_.reset();
      meter_sweep_num_subwords_.reset();
      meter_sweep_cmd_ovr_ctl_.reset();
      for (uint32_t alu = 0; alu < MauDefs::kNumMeterAlus; alu++) {
        subw_shift_[alu] = 0;
        sweep_op4_[alu] = Address::kMeterOp4Sweep;
      }
      ctor_running_ = false;
    }
    ~MauChipAddrDist() { }

    bool    get_deferred_ram_en(int s_or_m, int idx)      { return deferred_ram_ctl_.deferred_ram_en(s_or_m, idx); }
    bool    get_deferred_ram_thread(int s_or_m, int idx)  { return deferred_ram_ctl_.deferred_ram_thread(s_or_m, idx); }
    uint8_t get_deferred_ram_err_ctl(int s_or_m, int idx) { return deferred_ram_ctl_.deferred_ram_err_ctl(s_or_m,idx); }
    uint8_t get_deferred_oflo_ctl()                       { return deferred_oflo_ctl_.deferred_oflo_ctl(); }

    uint32_t get_meter_color_alu_rows(int lt) {
      // and IC Xbar, so has a vector of 4 bits, one bit for each ALU, then it gets mapped to 16 bit rows
      return MauMeterAlu::map_meter_alus_to_rows(meter_color_logical_to_phys_icxbar_.meter_color_logical_to_phys_icxbar_ctl(lt));
    }

    bool vpn_range_check_meter(int lrow, uint32_t addr) {
      // If bad row or meter_addr not enabled bail with false
      if ( (((MauDefs::kMeterAluLogicalRows >> lrow) & 1) == 0) ||
           (!Address::meter_addr_op_enabled(addr)) ) return false;
      uint32_t alu = MauMeterAlu::get_meter_alu_regs_index(lrow);
      RMT_ASSERT(alu < MauDefs::kNumMeterAlus);
      if (mau_meter_alu_vpn_range_.meter_vpn_range_check_enable(alu) == 0) return true;
      int vpn = Address::meter_addr_get_vpn(addr);
      return ((vpn >= (mau_meter_alu_vpn_range_.meter_vpn_base(alu)  & Address::kMeterVpnMask)) &&
              (vpn <= (mau_meter_alu_vpn_range_.meter_vpn_limit(alu) & Address::kMeterVpnMask)));
    }
    bool vpn_range_check_stats(int lrow, uint32_t addr) {
      // If bad row or stats_addr not enabled bail with false
      if ( (((MauDefs::kStatsAluLogicalRows >> lrow) & 1) == 0) ||
           (!Address::stats_addr_op_enabled(addr)) ) return false;
      uint32_t alu = MauStatsAlu::get_stats_alu_regs_index(lrow);
      RMT_ASSERT(alu < MauDefs::kNumStatsAlus);
      if (mau_stats_alu_vpn_range_.stats_vpn_range_check_enable(alu) == 0) return true;
      int vpn = Address::stats_addr_get_vpn(addr);
      return ((vpn >= (mau_stats_alu_vpn_range_.stats_vpn_base(alu)  & Address::kStatsVpnMask)) &&
              (vpn <= (mau_stats_alu_vpn_range_.stats_vpn_limit(alu) & Address::kStatsVpnMask)));
    }

    int get_meter_sweep_subword_shift(int alu) {
      RMT_ASSERT((alu >= 0) && (alu < static_cast<int>(MauDefs::kNumMeterAlus)));
      uint8_t v = meter_sweep_num_subwords_.meter_sweep_num_subwords(alu);
      RMT_ASSERT((v <= 4) && "Invalid meter_sweep_num_subwords");
      return v;
    }
    int get_meter_sweep_subwords(int alu) {
      return 1 << get_meter_sweep_subword_shift(alu);
    }
    int get_meter_sweep_op4(int alu) {
      RMT_ASSERT((alu >= 0) && (alu < static_cast<int>(MauDefs::kNumMeterAlus)));
      if (meter_sweep_cmd_ovr_ctl_.meter_sweep_cmd_ovr_en(alu) == 0) return Address::kMeterOp4Sweep;
      return meter_sweep_cmd_ovr_ctl_.meter_sweep_cmd_ovr_enc(alu);
    }

   private:
    void sweep_write_callback(int alu);

    bool                                                    ctor_running_;
    MauAddrDist                                            *mau_addr_dist_;
    std::array< int, MauDefs::kNumMeterAlus >               subw_shift_;
    std::array< int, MauDefs::kNumMeterAlus >               sweep_op4_;
    register_classes::DeferredRamCtlArray2                  deferred_ram_ctl_;
    register_classes::DeferredOfloCtl                       deferred_oflo_ctl_;
    register_classes::MeterColorLogicalToPhysIcxbarCtlArray meter_color_logical_to_phys_icxbar_;
    register_classes::MauMeterAluVpnRangeArray              mau_meter_alu_vpn_range_;
    register_classes::MauStatsAluVpnRangeArray              mau_stats_alu_vpn_range_;
    register_classes::MeterSweepNumSubwordsArray            meter_sweep_num_subwords_;
    register_classes::MeterSweepCmdOvrCtlArray              meter_sweep_cmd_ovr_ctl_;
 };

}

#endif
