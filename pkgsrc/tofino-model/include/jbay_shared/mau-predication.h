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

// MauPredication - JBay

#ifndef _JBAY_SHARED_MAU_PREDICATION_
#define _JBAY_SHARED_MAU_PREDICATION_

#include <mau-predication-common.h>

#include <register_includes/mpr_always_run.h>
#include <register_includes/mpr_glob_exec_thread.h>
#include <register_includes/mpr_glob_exec_lut_array.h>
#include <register_includes/mpr_long_brch_thread.h>
#include <register_includes/mpr_long_brch_lut_array.h>
#include <register_includes/mpr_next_table_lut_array2.h>
#include <register_includes/mpr_thread_delay_array.h>
#include <register_includes/mpr_bus_dep.h>
#include <register_includes/mpr_stage_id_array.h>

#include <register_includes/pred_always_run_array.h>
#include <register_includes/pred_glob_exec_thread_array.h>
#include <register_includes/pred_map_glob_array2.h>
#include <register_includes/pred_map_loca_array2.h>
#include <register_includes/pred_miss_exec_array.h>
#include <register_includes/pred_miss_long_brch_array.h>
#include <register_includes/pred_long_brch_lt_src_array.h>
#include <register_includes/pred_long_brch_terminate.h>
#include <register_includes/pred_long_brch_thread_array.h>
#include <register_includes/pred_ghost_thread.h>
#include <register_includes/pred_is_a_brch.h>
#include <register_includes/pred_stage_id.h>

#include <register_includes/next_table_map_en.h>
#include <register_includes/next_table_map_en_gateway.h>
#include <register_includes/next_table_tcam_actionbit_map_en.h>
#include <register_includes/next_stage_dependency_on_cur_array.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauPredication : public MauPredicationCommon {

 private:
    uint16_t get_powered_always_gex_lbr(uint16_t active_thread_lts,
                                        uint16_t glob_exec, uint8_t long_brch);
    uint16_t get_powered_next_tab(int thrd, int start_tab);
    uint16_t get_powered_mpr(int ing_start, int egr_start, int ght_start,
                             uint16_t glob_exec, uint8_t long_brch);
    uint16_t get_nxtab_mask(int thrd, bool active, uint16_t gress,
                            int nxt_tab);
    uint16_t get_active_mask(int thrd, bool active, uint16_t gress,
                             int start_tab, uint16_t glob_exec, uint8_t long_brch);

 public:
    MauPredication(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau);
    virtual ~MauPredication();

    void start(bool thread_active[]);
    void end();
    int  get_next_table(bool ingress, int curr_lt, bool *do_lookup);
    int  get_first_table(bool ingress, bool *do_lookup);
    void set_next_table(int lt, const MauLookupResult &result);
    uint16_t lt_info(uint16_t pred_sel);
    uint16_t lts_active();
    uint32_t output_seq();
    void     thread_output_info(int thrd, uint16_t *global_exec, uint8_t *long_branch);

 private:
    void  io_print(MauIO *io, const char *iodir);
    void  register_check_lt_subset(int thrd, int idx1, int idx2,
                                   uint16_t gress_lts, uint16_t reg_lts,
                                   const char *reg, bool warn, bool eq=false);
    void  register_check_lbr_subset(int thrd, int idx1, int idx2,
                                    uint8_t gress_lbrs, uint8_t reg_lbrs,
                                    const char *reg, bool warn, bool eq=false);
    void  register_change_callback();
    void  register_config_check();
    bool  register_config_change();
    bool  other_config_change();

 private:
    Mau       *mau_;
    uint16_t   lt_ingress_orig_;  // Original value ingress
    uint16_t   lt_counters_orig_; // Original value counters
    uint16_t   lt_ingress_;       // Ingress
    uint16_t   lt_egress_;        // Egress
    uint16_t   lt_ghost_;         // Ghost
    uint16_t   lt_mpr_;           // Powered LTs derived from MPR IOs + mpr_always_active
    uint16_t   lt_mpr_mask_;      // LTs in active gresses (OR of some/all ing_/egr_/ght_)
    uint16_t   lt_counters_;      // With counters
    uint16_t   lt_countable_;     // With counters - ON given thread_active
    uint16_t   lt_lookupable_;    // ON given thread active and ing/egr nxt_tab
    uint16_t   lt_runnable_;      // Lookupable OR countable
    uint16_t   lt_active_;        // Initial nxt_tab LT and all subsequent nxt_tab LTs
    uint16_t   lt_warn_;          // LTs to complain about
    uint8_t    threads_active_;

    bool                                       this_mau_ingress_match_dep_;
    bool                                       this_mau_egress_match_dep_;
    bool                                       next_mau_ingress_match_dep_;
    bool                                       next_mau_egress_match_dep_;
    uint32_t                                   prev_mau_output_seq_;

    uint32_t                                   curr_output_seq_;
    uint32_t                                   curr_check_seq_;
    uint32_t                                   pending_check_seq_;

    uint16_t                                   global_exec_; // Output value
    uint8_t                                    long_branch_; // Output value
    std::array< uint16_t, kThreads >           next_table_;  // Output nxt_tab vals
    std::array< uint16_t, kThreads >           thread_global_exec_; // Maximal output per-thread
    std::array< uint8_t,  kThreads >           thread_long_branch_; // Maximal output per-thread

    register_classes::MprAlwaysRun             mpr_always_run_;
    register_classes::MprGlobExecLutArray      mpr_glob_exec_lut_;
    register_classes::MprLongBrchLutArray      mpr_long_brch_lut_;
    register_classes::MprNextTableLutArray2    mpr_next_table_lut_;
    register_classes::MprStageIdArray          mpr_stage_id_;
    register_classes::MprBusDep                mpr_bus_dep_;
    register_classes::MprGlobExecThread        mpr_glob_exec_thread_;
    register_classes::MprLongBrchThread        mpr_long_brch_thread_;
    register_classes::MprThreadDelayArray      mpr_thread_delay_;

    register_classes::PredAlwaysRunArray       pred_always_run_;
    register_classes::PredGlobExecThreadArray  pred_glob_exec_thread_;
    register_classes::PredMapGlobArray2        pred_map_glob_;
    register_classes::PredMapLocaArray2        pred_map_loca_;
    register_classes::PredMissExecArray        pred_miss_exec_;
    register_classes::PredMissLongBrchArray    pred_miss_long_brch_;
    register_classes::PredLongBrchLtSrcArray   pred_long_brch_lt_src_;
    register_classes::PredLongBrchTerminate    pred_long_brch_terminate_;
    register_classes::PredLongBrchThreadArray  pred_long_brch_thread_;
    register_classes::PredGhostThread          pred_ghost_thread_;
    register_classes::PredIsABrch              pred_is_a_brch_;
    register_classes::PredStageId              pred_stage_id_;

    register_classes::NextTableMapEn           next_table_map_en_;
    register_classes::NextTableMapEnGateway    next_table_map_en_gateway_;
    register_classes::NextTableTcamActionbitMapEn   next_table_tcam_actionbit_map_en_;
    register_classes::NextStageDependencyOnCurArray next_stage_dependency_on_cur_;

  };

}

#endif // _JBAY_SHARED_MAU_PREDICATION_
