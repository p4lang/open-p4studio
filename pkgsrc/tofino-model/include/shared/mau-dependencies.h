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

#ifndef _SHARED_MAU_DEPENDENCIES_
#define _SHARED_MAU_DEPENDENCIES_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-delay.h>
#include <mau-chip-dependencies.h>

#include <register_includes/stage_concurrent_with_prev.h>
#include <register_includes/match_ie_input_mux_sel.h>
#include <register_includes/phv_fifo_enable.h>
//Removed in bfnregs 20150107_182406_7982_mau_dev
//#include <tofino/register_includes/final_output_delay.h>
#include <register_includes/predication_ctl.h>
#include <register_includes/action_output_delay.h>
#include <register_includes/cur_stage_dependency_on_prev.h>
#include <register_includes/next_stage_dependency_on_cur.h>
#include <register_includes/pipelength_added_stages.h>
#include <register_includes/deferred_eop_bus_delay_array.h>
#include <register_includes/logical_table_thread_array.h>
#include <register_includes/adr_dist_table_thread_array2.h>
#include <register_includes/exact_match_delay_thread_array.h>
#include <register_includes/exact_match_logical_result_delay.h>
#include <register_includes/exact_match_logical_result_en.h>
#include <register_includes/exact_match_phys_result_delay_array.h>
#include <register_includes/exact_match_phys_result_en_array.h>
#include <register_includes/exact_match_phys_result_thread_array.h>
#include <register_includes/adr_dist_pipe_delay_array2.h>
#include <register_includes/tind_bus_prop_array.h>
#include <register_includes/mau_hash_group_config.h>
#include <register_includes/meter_alu_group_action_ctl_array.h>
#include <register_includes/meter_alu_group_data_delay_ctl_array.h>


namespace MODEL_CHIP_NAMESPACE {

  class Mau;

  class MauDependencies01 {
 public:

    MauDependencies01(int chipIndex, int pipeIndex, int mauIndex,
                      int which, uint8_t ieflag, Mau *mau,
                      RegisterCallback& write_callback);
    ~MauDependencies01();

    static inline bool is_valid_dependency_mode(int mode) {
      return ((mode >= 0) && (mode < 32) &&
              (((MauDefs::valid_dependency_modes >> mode) & 1u) == 1u));
    }
    static inline int find_valid_dependency_mode(int start_mode) {
      if (start_mode >= 0) {
        for (int mode = start_mode; mode >= 0; mode--) {
          if (is_valid_dependency_mode(mode)) return mode;
        }
      }
      return -1;
    }
    inline uint32_t    stage_features()  const { return stage_features_; }
    inline uint32_t    depend_features() const { return dependency_features_; }
    inline uint32_t    series_features() const { return series_features_; }
    inline bool        ingress()         const { return ((ieflag_ & 1) == 1); }
    inline const char *iestr()           const { return (ingress()) ?"INGRESS" :"EGRESS"; }

 private:
    void update_dependency_features_this();
    void update_series_features_this(uint32_t features);
    void dump_delay_state_this(uint64_t flgs);
    void dump_delay_state_prev(uint64_t flgs, bool this_too);
    void dump_delay_state_next(uint64_t flgs, bool this_too);
    void find_series_features_this(uint32_t *features);
    void find_series_features_prev(uint32_t *features);
    void find_series_features_next(uint32_t *features);
    void distrib_series_features_this(uint32_t features);
    void distrib_series_features_prev(uint32_t features);
    void distrib_series_features_next(uint32_t features);

 public:
    void update_info_on_write();
    void dump_delay_state(uint64_t flgs);
    void find_series_features(uint32_t *features);
    void distrib_series_features(uint32_t features);
    void update_series_features();
    void update_stage_features(uint32_t features);
    bool report_bad_delay(const char *delaystr, uint8_t act, uint8_t exp,
                          bool allow_greater=false);
    bool report_bad_delay_prev(const char *delaystr, uint8_t act, uint8_t exp);
    bool check_pipelength_added_stages();
    bool check_start_table_fifo0();
    bool check_start_table_fifo1();
    bool check_action_output_delay();
    bool check_deferred_eop_bus_delay();
    uint8_t get_delay(int which_delay);
    uint8_t get_prev_post_pred_delay();


 public: // Make these all public to allow easy access
    //register_classes::FinalOutputDelay        final_output_delay_;
    register_classes::PredicationCtl            predication_ctl_;
    register_classes::ActionOutputDelay         action_output_delay_;
    register_classes::CurStageDependencyOnPrev  cur_stage_dependency_on_prev_;
    register_classes::NextStageDependencyOnCur  next_stage_dependency_on_cur_;
    register_classes::PipelengthAddedStages     pipelength_added_stages_;
    register_classes::DeferredEopBusDelay       deferred_eop_bus_delay_;
    RegisterCallback                            write_callback_;
    Mau                                        *mau_;
    MauDependencies01                          *prev_gress_deps_;
    MauDependencies01                          *next_gress_deps_;
    int                                         which_;
    uint8_t                                     ieflag_;
    bool                                        action_output_enabled_;
    bool                                        final_output_enabled_;
    bool                                        start_table_fifo0_enabled_;
    bool                                        start_table_fifo1_enabled_;
    bool                                        verify_table_thread_;
    uint16_t                                    table_thread_;
    uint8_t                                     cur_stage_dep_;
    uint8_t                                     next_stage_dep_;
    uint32_t                                    stage_features_;
    uint32_t                                    dependency_features_;
    uint32_t                                    series_features_;
  };


  class MauDependencies {
 public:
    static bool kRelaxDependencyCheck; // Defined in rmt-config.cpp
    static bool kRelaxDelayCheck;
    static bool kRelaxReplicationCheck;
    static bool kRelaxThreadCheck;

    static constexpr int kLogicalTables = MauDefs::kLogicalTablesPerMau;
    static constexpr int kNumMeterAlus  = MauDefs::kNumMeterAlus;

    MauDependencies(int chipIndex, int pipeIndex, int mauIndex, Mau *mau);
    ~MauDependencies();

    void link_deps();
    void set_prev_deps(MauDependencies *prev_deps);
    void set_next_deps(MauDependencies *next_deps);
    void check_config();

    inline MauDependencies01 *ingress()               { return &ingress_; }
    inline MauDependencies01 *egress()                { return &egress_; }
    inline MauDependencies01 *gress(int ie)           { return (ie==0) ?ingress() :egress(); }

    inline uint16_t logical_tables(int ie)            { return logical_tables(*gress(ie)); }
    inline uint16_t lt_ingress()                      { return logical_tables(ingress_); }
    inline uint16_t lt_egress()                       { return logical_tables(egress_); }
    inline bool     is_ingress_lt(int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      bool ing = (((lt_ingress() >> lt) & 1) == 1);
      bool egr = (((lt_egress()  >> lt) & 1) == 1);
      RMT_ASSERT( (!ing || !egr) && "LT in both gresses");
      return ing;
    }
    inline bool     is_egress_lt(int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      bool ing = (((lt_ingress() >> lt) & 1) == 1);
      bool egr = (((lt_egress()  >> lt) & 1) == 1);
      RMT_ASSERT( (!ing || !egr) && "LT in both gresses");
      return egr;
    }
    inline uint32_t dynamic_features(int ie)              { return dynamic_features(*gress(ie)); }
    inline void set_dynamic_features(int ie, uint32_t f)  { set_dynamic_features(*gress(ie), f); }

    inline bool get_right_action_override(int alu) {
      return chip_deps_.get_right_action_override(alu);
    }
    inline uint16_t get_ghost_table_thread() {
      return chip_deps_.get_ghost_table_thread();
    }
    inline uint16_t lt_ghost()                        { return get_ghost_table_thread(); }
    inline bool     is_ghost_lt(int lt) {
      RMT_ASSERT((lt >= 0) && (lt < kLogicalTables));
      bool ght = (((lt_ghost() >> lt) & 1) == 1);
      return ght;
    }


    inline bool ingress_prev_conc()                   { return prev_conc(ingress_); }
    inline bool ingress_prev_action_dependent()       { return prev_action_dep(ingress_); }
    inline bool ingress_prev_match_dependent()        { return prev_match_dep(ingress_); }
    inline bool ingress_check_dependencies()          { return check_dependencies(ingress_); }
    inline bool ingress_is_match_dependent()          { return is_match_dependent(ingress_); }
    inline bool ingress_is_action_dependent()         { return is_action_dependent(ingress_); }
    inline bool ingress_is_concurrent()               { return is_concurrent(ingress_); }
    inline bool ingress_use_iphv_for_match()          { return use_iphv_for_match(ingress_); }
    inline bool ingress_use_ophv_for_match()          { return use_ophv_for_match(ingress_); }
    inline bool ingress_use_iphv_for_action()         { return use_iphv_for_action(ingress_); }
    inline bool ingress_use_ophv_for_action()         { return use_ophv_for_action(ingress_); }
    inline bool egress_prev_conc()                    { return prev_conc(egress_); }
    inline bool egress_prev_action_dependent()        { return prev_action_dep(egress_); }
    inline bool egress_prev_match_dependent()         { return prev_match_dep(egress_); }
    inline bool egress_check_dependencies()           { return check_dependencies(egress_); }
    inline bool egress_is_match_dependent()           { return is_match_dependent(egress_); }
    inline bool egress_is_action_dependent()          { return is_action_dependent(egress_); }
    inline bool egress_is_concurrent()                { return is_concurrent(egress_); }
    inline bool egress_use_iphv_for_match()           { return use_iphv_for_match(egress_); }
    inline bool egress_use_ophv_for_match()           { return use_ophv_for_match(egress_); }
    inline bool egress_use_iphv_for_action()          { return use_iphv_for_action(egress_); }
    inline bool egress_use_ophv_for_action()          { return use_ophv_for_action(egress_); }
    inline bool start_table_avail(bool ingress)       { return start_table_avail(ingress ?ingress_ :egress_); }
    inline bool ingress_next_maybe_conc()             { return next_maybe_conc(ingress_); }
    inline bool ingress_next_maybe_action_dependent() { return next_maybe_action_dep(ingress_); }
    inline bool ingress_next_maybe_match_dependent()  { return next_maybe_match_dep(ingress_); }
    inline bool egress_next_maybe_conc()              { return next_maybe_conc(egress_); }
    inline bool egress_next_maybe_action_dependent()  { return next_maybe_action_dep(egress_); }
    inline bool egress_next_maybe_match_dependent()   { return next_maybe_match_dep(egress_); }
    inline bool prev_concurrent(bool ingress)         { return prev_conc(ingress ?ingress_ :egress_); }
    inline bool prev_action_dependent(bool ingress)   { return prev_action_dep(ingress ?ingress_ :egress_); }
    inline bool prev_match_dependent(bool ingress)    { return prev_match_dep(ingress ?ingress_ :egress_); }
    inline bool is_match_dependent(bool ingress)      { return is_match_dependent(ingress ?ingress_ :egress_); }
    inline bool is_action_dependent(bool ingress)     { return is_action_dependent(ingress ?ingress_ :egress_); }
    inline bool is_concurrent(bool ingress)           { return is_concurrent(ingress ?ingress_ :egress_); }
    inline uint8_t get_delay(bool ingress, int which) { return get_delay(ingress ?ingress_ :egress_, which); }



 private:
    void stage_conc_write_callback();
    void input_mux_sel_write_callback();
    void fifo_enable_write_callback();
    void lt_thread_change_callback(uint32_t repl);
    void ad_thread_change_callback(uint32_t ie, uint32_t repl);
    void xm_delay_change_callback(uint32_t repl);
    void xm_bus_timing_change_callback(uint32_t i);
    void ad_pipedelay_change_callback(uint32_t ie, uint32_t repl);
    void tind_bus_prop_change_callback(uint32_t i);
    void hash_group_config_change_callback();
    void meter_alu_group_ctl_change_callback(uint32_t alu);
    void ingress_reg_write_callback();
    void egress_reg_write_callback();
    void dependencies_changed();
    uint16_t check_lt_thread_replication(int ie);
    uint16_t check_ad_thread_replication(int ie);
    void check_xm_delay_replication(int ie);
    void check_ad_pipedelay_replication(int ie);
    void check_thread_config(int ie);
    void check_thread_config();
    uint16_t logical_tables(MauDependencies01& iegress);
    uint32_t dynamic_features(MauDependencies01& iegress);
    void set_dynamic_features(MauDependencies01& iegress, uint32_t features);
    bool report_bad_delay(MauDependencies01& iegress,
                          const char *delaystr, uint8_t act, uint8_t exp,
                          bool allow_greater=false);
    bool check_xm_bus_delay(MauDependencies01& iegress);
    bool check_adr_dist_pipe_delay(MauDependencies01& iegress);
    bool check_meter_alu_group_action_ctl(MauDependencies01& iegress);
    bool check_meter_alu_group_data_delay_ctl(MauDependencies01& iegress);
    bool check_delays(MauDependencies01& iegress);
    bool prev_conc(MauDependencies01& iegress);
    bool prev_action_dep(MauDependencies01& iegress);
    bool prev_match_dep(MauDependencies01& iegress);
    bool check_dependencies(MauDependencies01& iegress);
    bool is_match_dependent(MauDependencies01& iegress);
    bool is_action_dependent(MauDependencies01& iegress);
    bool is_concurrent(MauDependencies01& iegress);
    bool use_iphv(MauDependencies01& iegress);
    bool use_ophv(MauDependencies01& iegress);
    bool use_iphv_for_match(MauDependencies01& iegress);
    bool use_ophv_for_match(MauDependencies01& iegress);
    bool use_iphv_for_action(MauDependencies01& iegress);
    bool use_ophv_for_action(MauDependencies01& iegress);
    bool start_table_avail(MauDependencies01& iegress);
    bool next_maybe_conc(MauDependencies01& iegress);
    bool next_maybe_action_dep(MauDependencies01& iegress);
    bool next_maybe_match_dep(MauDependencies01& iegress);
    uint8_t get_delay(MauDependencies01& iegress, int which_delay);

    Mau                                              *mau_;
    bool                                              ctor_running_;
    register_classes::StageConcurrentWithPrev         stage_concurrent_with_prev_;
    register_classes::MatchIeInputMuxSel              match_ie_input_mux_sel_;
    register_classes::PhvFifoEnable                   phv_fifo_enable_;
    register_classes::LogicalTableThreadArray         logical_table_thread_;
    register_classes::AdrDistTableThreadArray2        adr_dist_table_thread_;
    register_classes::ExactMatchDelayThreadArray      exact_match_delay_thread_;
    register_classes::ExactMatchLogicalResultDelay    exact_match_logical_result_delay_;
    register_classes::ExactMatchLogicalResultEn       exact_match_logical_result_en_;
    register_classes::ExactMatchPhysResultDelayArray  exact_match_phys_result_delay_;
    register_classes::ExactMatchPhysResultEnArray     exact_match_phys_result_en_;
    register_classes::ExactMatchPhysResultThreadArray exact_match_phys_result_thread_;
    register_classes::AdrDistPipeDelayArray2          adr_dist_pipe_delay_;
    register_classes::TindBusPropArray                tind_bus_prop_;
    register_classes::MauHashGroupConfig              hash_group_config_;
    register_classes::MeterAluGroupActionCtlArray     meter_alu_group_action_ctl_;
    register_classes::MeterAluGroupDataDelayCtlArray  meter_alu_group_data_delay_ctl_;

    MauChipDependencies                               chip_deps_;
    MauDependencies                                  *prev_deps_;
    MauDependencies                                  *next_deps_;
    MauDependencies01                                 ingress_;
    MauDependencies01                                 egress_;
  };
}
#endif // _SHARED_MAU_DEPENDENCIES_
