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

// MauTeop - JBay specific implementation

#ifndef _JBAY_SHARED_MAU_TEOP_
#define _JBAY_SHARED_MAU_TEOP_

#include <mau-teop-common.h>

#include <register_includes/adr_dist_stats_adr_icxbar_ctl_array.h>
#include <register_includes/adr_dist_meter_adr_icxbar_ctl_array.h>
#include <register_includes/mau_ad_stats_virt_lt_array.h>
#include <register_includes/mau_ad_meter_virt_lt_array.h>
#include <register_includes/deferred_ram_ctl_array2.h>
#include <register_includes/meter_color_logical_to_phys_icxbar_ctl_array.h>

#include <register_includes/dp_teop_meter_ctl_array.h>
#include <register_includes/dp_teop_stats_ctl_array.h>
#include <register_includes/teop_to_meter_adr_oxbar_ctl_array.h>
#include <register_includes/teop_to_stats_adr_oxbar_ctl_array.h>
#include <register_includes/meter_ctl_teop_en.h>
#include <register_includes/statistics_ctl_teop_en.h>
#include <register_includes/meter_to_teop_adr_oxbar_ctl_array.h>
#include <register_includes/stats_to_teop_adr_oxbar_ctl_array.h>
#include <register_includes/teop_bus_ctl_array.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauTeop : public MauTeopCommon {

 public:
    static constexpr int kTables       = MauDefs::kLogicalTablesPerMau;
    static constexpr int kAlus         = MauDefs::kNumAlus;
    static constexpr int kNumMeterAlus = MauDefs::kNumMeterAlus;
    static constexpr int kNumStatsAlus = MauDefs::kNumStatsAlus;
    static constexpr int kNumTeopBuses = MauDefs::kNumTeopBuses;
    static_assert( (kNumTeopBuses < 8), "Teop bus bitmask must fit in uint8_t" );


    MauTeop(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    virtual ~MauTeop();


    // These not part of MauTeopCommon interface at moment
    uint8_t teop_buses_at_hdrtime(int lt);
    uint8_t teop_buses_at_teoptime(int lt);
    uint8_t teop_buses_at_hdrtime_cached(int lt);
    uint8_t teop_buses_at_teoptime_cached(int lt);
    uint8_t teop_buses_at_hdrtime();
    uint8_t teop_buses_at_teoptime();
    uint8_t teop_buses_at_hdrtime_cached();
    uint8_t teop_buses_at_teoptime_cached();
    uint8_t teop_buses_at_hdrtime_in_earlier_stage();
    uint8_t teop_buses_at_teoptime_in_earlier_stage();
    uint8_t teop_buses_at_hdrtime_in_earlier_stage_cached();
    uint8_t teop_buses_at_teoptime_in_earlier_stage_cached();
    uint8_t teop_delay_required(int bus);
    bool    teop_delay_check(int bus);


    // These *are* part of MauTeopCommon interface
    bool  teop_available();
    Teop *teop_allocate();
    void  teop_free(Teop *teop);
    bool  teop_being_used(int lt);
    bool  teop_being_used();
    bool  teop_enabled(int s_or_m, int alu);
    bool  teop_input_meter_addr(const Teop &teop, int alu,
                                uint32_t *meter_addr, int *lt);
    bool  teop_input_stats_addr(const Teop &teop, int alu,
                                uint32_t *stats_addr, int *lt);
    void  teop_output_meter_addr(Teop *teop, int lt, bool ingress,
                                 int alu, uint32_t meter_addr);
    void  teop_output_stats_addr(Teop *teop, int lt, bool ingress,
                                 int alu, uint32_t stats_addr);

 private:
    void    set_prev_teop(MauTeop *prev_teop);
    void    set_next_teop(MauTeop *next_teop);
    uint8_t link_teop();
    void    register_change_callback(int lt_alu_bus);

 private:
    Mau                                                     *mau_;
    MauTeop                                                 *prev_mau_teop_;
    MauTeop                                                 *next_mau_teop_;
    uint8_t                                                  min_linked_mau_index_;
    std::array< uint8_t, kTables >                           teoptime_buses_;
    std::array< uint8_t, kTables >                           hdrtime_buses_;
    uint8_t                                                  teoptime_buses_here_;
    uint8_t                                                  hdrtime_buses_here_;
    uint8_t                                                  teoptime_buses_earlier_;
    uint8_t                                                  hdrtime_buses_earlier_;

    register_classes::AdrDistMeterAdrIcxbarCtlArray          adr_dist_meter_icxbar_;
    register_classes::AdrDistStatsAdrIcxbarCtlArray          adr_dist_stats_icxbar_;
    register_classes::MauAdMeterVirtLtArray                  mau_ad_meter_virt_lt_;
    register_classes::MauAdStatsVirtLtArray                  mau_ad_stats_virt_lt_;
    register_classes::DeferredRamCtlArray2                   deferred_ram_ctl_;
    register_classes::MeterColorLogicalToPhysIcxbarCtlArray  meter_color_logical_to_phys_icxbar_;

    register_classes::DpTeopMeterCtlArray                    dp_teop_meter_ctl_;
    register_classes::DpTeopStatsCtlArray                    dp_teop_stats_ctl_;
    register_classes::TeopToMeterAdrOxbarCtlArray            teop_to_meter_adr_oxbar_ctl_;
    register_classes::TeopToStatsAdrOxbarCtlArray            teop_to_stats_adr_oxbar_ctl_;

    std::array< register_classes::MeterCtlTeopEn*,      kNumMeterAlus >  meter_ctl_teop_en_;
    std::array< register_classes::StatisticsCtlTeopEn*, kNumStatsAlus >  statistics_ctl_teop_en_;

    register_classes::MeterToTeopAdrOxbarCtlArray            meter_to_teop_adr_oxbar_ctl_;
    register_classes::StatsToTeopAdrOxbarCtlArray            stats_to_teop_adr_oxbar_ctl_;
    register_classes::TeopBusCtlArray                        teop_bus_ctl_;

  };

}

#endif // _JBAY_SHARED_MAU_TEOP_
