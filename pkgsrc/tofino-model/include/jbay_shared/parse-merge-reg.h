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

#ifndef _JBAY_SHARED_PARSE_MERGE_REG_
#define _JBAY_SHARED_PARSE_MERGE_REG_

#include <phv.h>
#include <rmt-defs.h>
#include <register_adapters.h>
#include <model_core/spinlock.h>

// Reg defs auto-generated from Semifore
#include <register_includes/pmerge_upper_left_reg_phv_owner_127_0_mutable.h>
#include <register_includes/pmerge_upper_right_reg_phv_owner_255_128_mutable.h>
#include <register_includes/pmerge_upper_left_reg_phv_clr_on_wr_127_0_mutable.h>
#include <register_includes/pmerge_upper_right_reg_phv_clr_on_wr_255_128_mutable.h>
#include <register_includes/pmerge_lower_left_pps_reg_i_start_table_array_mutable.h>
#include <register_includes/pmerge_lower_right_pps_reg_e_start_table_array_mutable.h>
#include <register_includes/pmerge_lower_right_pps_reg_g_start_table_mutable.h>
#include <register_includes/pmerge_lower_right_pps_reg_tm_status_phv_mutable.h>
//#include <register_includes/prsr_reg_merge_rspec_phv_valid_mutable.h>
//#include <register_includes/prsr_reg_merge_rspec_mode_array_mutable.h>
//#include <register_includes/scratch_r_mutable.h>


namespace MODEL_CHIP_NAMESPACE {

  class ParseMergeReg {
    static constexpr int kTmStatusFifoSz = 16;
    static_assert( (kTmStatusFifoSz < 0xFF), "TmStatus ptrs too small");

 public:
    ParseMergeReg(int chipIndex, int pipeIndex) :
        spinlock_(), tm_status_rd_(0), tm_status_cnt_(0),
        prs_merge_phv_owner_127_0_(pipe_adapter(prs_merge_phv_owner_127_0_, chipIndex, pipeIndex)),
        prs_merge_phv_owner_255_128_(pipe_adapter(prs_merge_phv_owner_255_128_, chipIndex, pipeIndex)),
        prs_merge_phv_clr_on_wr_127_0_(pipe_adapter(prs_merge_phv_clr_on_wr_127_0_, chipIndex, pipeIndex)),
        prs_merge_phv_clr_on_wr_255_128_(pipe_adapter(prs_merge_phv_clr_on_wr_255_128_, chipIndex, pipeIndex)),
        prs_merge_i_start_tabs_(pipe_adapter(prs_merge_i_start_tabs_, chipIndex, pipeIndex)),
        prs_merge_e_start_tabs_(pipe_adapter(prs_merge_e_start_tabs_, chipIndex, pipeIndex)),
        prs_merge_g_start_tab_(pipe_adapter(prs_merge_g_start_tab_, chipIndex, pipeIndex)),
        prs_merge_pps_tm_status_phv_(pipe_adapter(prs_merge_pps_tm_status_phv_, chipIndex, pipeIndex))
        //prs_merge_phv_valid_(pipe_adapter(prs_merge_phv_valid_, chipIndex, pipeIndex)),
        //prs_merge_mode_(pipe_adapter(prs_merge_mode_, chipIndex, pipeIndex)),
        //scratch_(pipe_adapter(scratch_, chipIndex, pipeIndex))
    {
      prs_merge_phv_owner_127_0_.reset();
      prs_merge_phv_owner_255_128_.reset();
      prs_merge_phv_clr_on_wr_127_0_.reset();
      prs_merge_phv_clr_on_wr_255_128_.reset();
      prs_merge_i_start_tabs_.reset();
      prs_merge_e_start_tabs_.reset();
      prs_merge_g_start_tab_.reset();
      prs_merge_pps_tm_status_phv_.reset();
      //prs_merge_phv_valid_.reset();
      //prs_merge_mode_.reset();
      //scratch_.reset();
    }
    ~ParseMergeReg() { }

    inline bool is_port_valid(int port) {
      // check port index is in interval [0,71]
      return (port >= 0) && (port < RmtDefs::kPortsPerPipe);
    }

    inline uint8_t phv_owner(int off16) {
      RMT_ASSERT((off16 >= 0) && (off16 < 256));
      if (off16 < 128) {
        return prs_merge_phv_owner_127_0_.owner(off16);
      } else {
        return prs_merge_phv_owner_255_128_.owner(off16 - 128);
      }
    }
    inline void phv_set_owner(int off16, uint8_t val) {
      RMT_ASSERT((off16 >= 0) && (off16 < 256));
      if (off16 < 128) {
        prs_merge_phv_owner_127_0_.owner(off16, val & 1);
      } else {
        prs_merge_phv_owner_255_128_.owner(off16 - 128, val & 1);
      }
    }


    inline bool phv_clr_on_wr(int off16) {
      RMT_ASSERT((off16 >= 0) && (off16 < 256));
      if (off16 < 128) {
        return ((prs_merge_phv_clr_on_wr_127_0_.clr(off16) & 1) == 1);
      } else {
        return ((prs_merge_phv_clr_on_wr_255_128_.clr(off16 - 128) & 1) == 1);
      }
    }
    inline void phv_set_clr_on_wr(int off16, bool tf) {
      RMT_ASSERT((off16 >= 0) && (off16 < 256));
      if (off16 < 128) {
        prs_merge_phv_clr_on_wr_127_0_.clr(off16, tf?1:0);
      } else {
        prs_merge_phv_clr_on_wr_255_128_.clr(off16 - 128, tf?1:0);
      }
    }


    // No valid bits in JBay - so no mechanism for initialising PHV word as valid
    inline bool phv_init_valid(int off16) { return GLOBAL_FALSE; }
    inline void phv_set_init_valid(int off16, bool tf) { }


    inline uint16_t phv_start_tab_ingress(int port) {
      RMT_ASSERT(is_port_valid(port));
      return prs_merge_i_start_tabs_.table(port);
    }
    inline void phv_set_start_tab_ingress(int port, int tab) {
      RMT_ASSERT(is_port_valid(port));
      prs_merge_i_start_tabs_.table(port, tab);
    }
    inline void phv_set_start_tab_ingress(int tab) { // All ports
      for (int i = 0; i < RmtDefs::kPortsPerPipe; i++) phv_set_start_tab_ingress(i, tab);
    }

    inline uint16_t phv_start_tab_egress(int port) {
      RMT_ASSERT(is_port_valid(port));
      return prs_merge_e_start_tabs_.table(port);
    }
    inline void phv_set_start_tab_egress(int port, int tab) {
      RMT_ASSERT(is_port_valid(port));
      prs_merge_e_start_tabs_.table(port, tab);
    }
    inline void phv_set_start_tab_egress(int tab) { // All ports
      for (int i = 0; i < RmtDefs::kPortsPerPipe; i++) phv_set_start_tab_egress(i, tab);
    }

    inline uint16_t phv_start_tab_ghost() {
      return prs_merge_g_start_tab_.table();
    }
    inline void phv_set_start_tab_ghost(int tab) {
      prs_merge_g_start_tab_.table(tab);
    }

    // chip-specific functions
    virtual bool phv_get_tm_status_phv(int pipe, uint8_t *off16) = 0;
    virtual void phv_set_tm_status_phv(int pipe, uint8_t off16) = 0;
    virtual bool phv_get_tm_status_phv_sec(int pipe, uint8_t *off16, uint8_t *which_half) = 0;  // WIP only
    virtual void phv_set_tm_status_phv_sec(uint8_t off16, uint8_t which_half) = 0;  // WIP only
    // end chip-specific functions

    bool tm_status_input(uint32_t *val_msb, uint32_t *val_lsb);
    bool set_tm_status_input(uint32_t val_msb, uint32_t val_lsb=0);

 private:
    model_core::Spinlock                                               spinlock_;
    uint8_t                                                            tm_status_rd_;
    uint8_t                                                            tm_status_cnt_;
    // array of status values, each uint32_t[2] entry has tm status MSB value
    // in index 0 to be written to tm_status_phv; for WIP only, index 1 may
    // optionally hold tm status LSB value to be written to tm_status_phv_sec
    std::array< uint32_t[2], kTmStatusFifoSz >                         tm_status_;
    register_classes::PmergeUpperLeftRegPhvOwner_127_0Mutable          prs_merge_phv_owner_127_0_;
    register_classes::PmergeUpperRightRegPhvOwner_255_128Mutable       prs_merge_phv_owner_255_128_;
    register_classes::PmergeUpperLeftRegPhvClrOnWr_127_0Mutable        prs_merge_phv_clr_on_wr_127_0_;
    register_classes::PmergeUpperRightRegPhvClrOnWr_255_128Mutable     prs_merge_phv_clr_on_wr_255_128_;
    register_classes::PmergeLowerLeftPpsRegIStartTableArrayMutable     prs_merge_i_start_tabs_;
    register_classes::PmergeLowerRightPpsRegEStartTableArrayMutable    prs_merge_e_start_tabs_;
    register_classes::PmergeLowerRightPpsRegGStartTableMutable         prs_merge_g_start_tab_;
 protected:
    register_classes::PmergeLowerRightPpsRegTmStatusPhvMutable         prs_merge_pps_tm_status_phv_;
  };

}
#endif // _JBAY_SHARED_PARSE_MERGE_REG_
