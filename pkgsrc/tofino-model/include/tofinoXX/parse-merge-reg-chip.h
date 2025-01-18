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

#ifndef _TOFINOXX_PARSE_MERGE_REG_
#define _TOFINOXX_PARSE_MERGE_REG_

#include <phv.h>
#include <register_adapters.h>

// Reg defs auto-generated from Semifore
#include <register_includes/prsr_reg_merge_rspec_phv_owner_mutable.h>
#include <register_includes/prsr_reg_merge_rspec_phv_valid_mutable.h>
#include <register_includes/prsr_reg_merge_rspec_mode_array_mutable.h>
// This register is instantiated but is NOT used - however allows read/write
#include <register_includes/scratch_r_mutable.h>


namespace MODEL_CHIP_NAMESPACE {

  class ParseMergeReg {

 public:
    ParseMergeReg(int chipIndex, int pipeIndex) :
        start_tab_ingress_(0), start_tab_egress_(0), start_tab_ghost_(0),
        prs_merge_phv_owner_(pipe_adapter(prs_merge_phv_owner_, chipIndex, pipeIndex)),
        prs_merge_phv_valid_(pipe_adapter(prs_merge_phv_valid_, chipIndex, pipeIndex)),
        prs_merge_mode_(pipe_adapter(prs_merge_mode_, chipIndex, pipeIndex)),
        scratch_(pipe_adapter(scratch_, chipIndex, pipeIndex)) {

      prs_merge_phv_owner_.reset();
      prs_merge_phv_valid_.reset();
      prs_merge_mode_.reset();
      scratch_.reset();
    }
    ~ParseMergeReg() { }


    inline uint8_t phv_owner(int phv_word) {
      uint8_t val = 0;
      if (Phv::is_valid_norm_phv_p(phv_word))
        val = prs_merge_phv_owner_.owner(phv_word) & 0x1;
      else if (Phv::is_valid_taga_phv_p(phv_word))
        val = prs_merge_phv_owner_.t_owner(phv_word - k_phv::kTagalongStart) & 0x1;
      return val;
    }
    inline void phv_set_owner(int phv_word, uint8_t val) {
      if (Phv::is_valid_norm_phv_p(phv_word))
        prs_merge_phv_owner_.owner(phv_word, val & 0x1);
      else if (Phv::is_valid_taga_phv_p(phv_word))
        prs_merge_phv_owner_.t_owner(phv_word - k_phv::kTagalongStart, val & 0x1);
    }


    inline bool phv_clr_on_wr(int off16)              { return false; }
    inline void phv_set_clr_on_wr(int off16, bool tf) { }


    inline bool phv_init_valid(int phv_word) {
      if (Phv::is_valid_norm_phv_p(phv_word))
        return ((prs_merge_phv_valid_.vld(phv_word) & 0x1) == 0x1);
      else
        return false;
    }
    inline void phv_set_init_valid(int phv_word, bool tf) {
      if (Phv::is_valid_norm_phv_p(phv_word))
        prs_merge_phv_valid_.vld(phv_word, tf ?0x1 :0x0);
    }


    inline uint8_t mode(int prsrIndex) {
      return prs_merge_mode_.mode(prsrIndex);
    }
    inline void set_mode(int prsrIndex, uint8_t val) {
      prs_merge_mode_.mode(prsrIndex, val);
    }


    inline uint16_t phv_start_tab_ingress(int port) {
      return start_tab_ingress_;
    }
    inline void phv_set_start_tab_ingress(int port, int tab) {
      start_tab_ingress_ = tab; // port ignored on Tofino
    }
    inline void phv_set_start_tab_ingress(int tab) {
      start_tab_ingress_ = tab;
    }

    inline uint16_t phv_start_tab_egress(int port) {
      return start_tab_egress_;
    }
    inline void phv_set_start_tab_egress(int port, int tab) {
      start_tab_egress_ = tab; // port ignored on Tofino
    }
    inline void phv_set_start_tab_egress(int tab) {
      start_tab_egress_ = tab;
    }

    inline uint16_t phv_start_tab_ghost() {
      return start_tab_ghost_;
    }
    inline void phv_set_start_tab_ghost(int tab) {
      start_tab_ghost_ = tab;
    }

 private:
    int                                                  start_tab_ingress_;
    int                                                  start_tab_egress_;
    int                                                  start_tab_ghost_;
    register_classes::PrsrRegMergeRspecPhvOwnerMutable   prs_merge_phv_owner_;
    register_classes::PrsrRegMergeRspecPhvValidMutable   prs_merge_phv_valid_;
    register_classes::PrsrRegMergeRspecModeArrayMutable  prs_merge_mode_;
    // This register is instantiated but is NOT used - however allows read/write
    register_classes::ScratchRMutable                    scratch_;
  };

  typedef ParseMergeReg ParseMergeRegType;
}
#endif // _TOFINOXX_PARSE_MERGE_REG_
