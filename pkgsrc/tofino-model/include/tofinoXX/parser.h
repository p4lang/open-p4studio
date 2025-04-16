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

#ifndef _TOFINOXX_PARSER_
#define _TOFINOXX_PARSER_

#include <parser-shared.h>
#include <parser-extractors-all.h>
#include <register_includes/prsr_reg_main_rspec_err_phv_cfg_mutable.h>
#include <register_includes/prsr_reg_main_rspec_enable_mutable.h>
#include <register_includes/prsr_reg_main_rspec_mode_mutable.h>
#include <register_includes/prsr_reg_main_rspec_dst_cont_err_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_intr_status_mutable.h>
// These registers are instantiated but are NOT used - however allows read/write
#include <register_includes/prsr_reg_main_rspec_intr_enable0_mutable.h>
#include <register_includes/prsr_reg_main_rspec_intr_enable1_mutable.h>
#include <register_includes/prsr_reg_main_rspec_intr_freeze_enable_mutable.h>
#include <register_includes/prsr_reg_main_rspec_intr_inject_mutable.h>
#include <register_includes/prsr_reg_main_rspec_dst_cont_errlog_mutable.h>
#include <register_includes/prsr_reg_main_rspec_aram_mbe_errlog_mutable.h>
#include <register_includes/prsr_reg_main_rspec_aram_sbe_errlog_mutable.h>
#include <register_includes/prsr_reg_main_rspec_ctr_range_errlog_mutable.h>
#include <register_includes/prsr_reg_main_rspec_ibuf_oflow_errlog_mutable.h>
#include <register_includes/prsr_reg_main_rspec_ibuf_uflow_errlog_mutable.h>
#include <register_includes/prsr_reg_main_rspec_multi_wr_errlog_mutable.h>
#include <register_includes/prsr_reg_main_rspec_no_tcam_match_errlog_mutable.h>
#include <register_includes/prsr_reg_main_rspec_op_fifo_oflow_errlog_mutable.h>
#include <register_includes/prsr_reg_main_rspec_op_fifo_uflow_errlog_mutable.h>
#include <register_includes/prsr_reg_main_rspec_partial_hdr_errlog_mutable.h>
#include <register_includes/prsr_reg_main_rspec_phv_owner_errlog_mutable.h>
#include <register_includes/prsr_reg_main_rspec_src_ext_errlog_mutable.h>
#include <register_includes/prsr_reg_main_rspec_tcam_par_errlog_mutable.h>
#include <register_includes/scratch_r_mutable.h>
#include <register_includes/prsr_reg_main_rspec_a_emp_thresh_mutable.h>


namespace MODEL_CHIP_NAMESPACE {

  class Parser : public ParserShared {

 private:
    // Define this static accessors to facilitate calling sub-constructors in parser CTOR
    static enum memory_classes::PrsrMlTcamRowArray::PrsrMemMainRspecEnum get_tcam_w01(int i) {
      if (i == 0) return memory_classes::PrsrMlTcamRowArray::PrsrMemMainRspecEnum::kMlTcamRowWord0;
      return memory_classes::PrsrMlTcamRowArray::PrsrMemMainRspecEnum::kMlTcamRowWord1;
    }
    void reset_this();

 public:
    static const char *kCounterOpStrings[];

    Parser(RmtObjectManager *om, int pipeIndex, int prsIndex, int ioIndex,
           const ParserConfig &config);
    virtual ~Parser();
    void reset();
    void not_implemented(const char *clazz);
    void memory_change_callback(uint8_t mem_type, uint8_t mem_inst, uint32_t mem_index);


    // Tofino wraps on increment
    inline int phv_increment(int phv, int incr) { return Phv::wrap_phv_p(phv, incr); }

    // Functions to handle varying top-level registers
            uint8_t  hdr_len_adj();
            void set_hdr_len_adj(uint8_t v);
    inline uint8_t  port_rate_cfg(int ci) { return 0u; }
    inline uint8_t  port_chnl_en(int ci)  { return 0; }
    inline bool     enabled(int ci)       { return ((prs_main_enable_.enable(ci) & 1) == 1); }
    inline uint8_t  mode() {
      uint8_t m = prs_main_mode_.mode();
      ParseMergeReg *prs_mrg = get_prs_merge();
      if (prs_mrg != NULL) RMT_ASSERT(m == prs_mrg->mode(parser_index()));
      return m;
    }
    inline void set_port_rate_cfg(int ci, uint8_t v) { }
    inline void set_port_chnl_en(int ci, uint8_t v)  { }
    inline void set_enabled(int ci, bool tf)         { prs_main_enable_.enable(ci, tf ?1 :0); }
    inline void set_mode(uint8_t v) {
      prs_main_mode_.mode(v);
      ParseMergeReg *prs_mrg = get_prs_merge();
      if (prs_mrg != NULL) prs_mrg->set_mode(parser_index(), v);
    }
    void inc_tx_count(int ci);


    inline bool perr_phv_output(int chan, uint32_t err, Phv *phv) {
      if (err == 0u) return true;            // No error whatsoever (=> NO drop)
      uint32_t nodrop_mask = kErrNoDropMask; // Certain errors (csum) unconditionally mean NO drop
      // Other errors can be enabled/disabled to determine whether they provoke drop or not
      // XXX: should ignore these (treat as 1) on egress
      bool egr = egress();
      if (egr || prs_main_err_phv_cfg_.no_tcam_match_err_en())   nodrop_mask |= kErrNoTcamMatch;
      if (egr || prs_main_err_phv_cfg_.partial_hdr_err_en())     nodrop_mask |= kErrPartialHdr;
      if (egr || prs_main_err_phv_cfg_.ctr_range_err_en())       nodrop_mask |= kErrCtrRange;
      if (egr || prs_main_err_phv_cfg_.timeout_iter_err_en())    nodrop_mask |= kErrTimeoutIter;
      if (egr || prs_main_err_phv_cfg_.timeout_cycle_err_en())   nodrop_mask |= kErrTimeoutCycle;
      if (egr || prs_main_err_phv_cfg_.src_ext_err_en())         nodrop_mask |= kErrSrcExt;
      if (egr || prs_main_err_phv_cfg_.dst_cont_err_en())        nodrop_mask |= kErrDstCont;
      if (egr || prs_main_err_phv_cfg_.phv_owner_err_en())       nodrop_mask |= kErrPhvOwner;
      if (egr || prs_main_err_phv_cfg_.multi_wr_err_en())        nodrop_mask |= kErrMultiWr;
      //if (egr || prs_main_err_phv_cfg_.aram_sbe_en())          nodrop_mask |= kErrARAMsbe;
      if (egr || prs_main_err_phv_cfg_.aram_mbe_en())            nodrop_mask |= kErrARAMmbe;
      if (egr || prs_main_err_phv_cfg_.fcs_err_en())             nodrop_mask |= kErrFcs;
      //if (egr || prs_main_err_phv_cfg_.csum_err_en())          nodrop_mask |= kErrCsum;
      //if (egr || prs_main_err_phv_cfg_.ibuf_oflow_err_en())    nodrop_mask |= kErrIbufOflow;
      //if (egr || prs_main_err_phv_cfg_.ibuf_uflow_err_en())    nodrop_mask |= kErrIbufUflow;
      //if (egr || prs_main_err_phv_cfg_.op_fifo_oflow_err_en()) nodrop_mask |= kErrOpFifoOflow;
      //if (egr || prs_main_err_phv_cfg_.op_fifo_uflow_err_en()) nodrop_mask |= kErrOpFifoUflow;
      //if (egr || prs_main_err_phv_cfg_.tcam_par_en())          nodrop_mask |= kErrTcamPar;
      //if (egr || prs_main_err_phv_cfg_.csum_config_err_en())   nodrop_mask |= kErrCsumConfig ;
      err &= nodrop_mask;
      if (err == 0u) return false; // No 'no drop' errs so return false (=> DROP)
      // We know we're not dropping this PHV but what errors do we actually report in PHV
      err &= perr_phv_mask();      // By dflt perr_phv_mask_ = kErrDfltRepMask
      if (err == 0u) return true;  // But nothing to report (=> NO drop though)
      // Write errors to configured dst PHV word
      perr_phv_write(prs_main_err_phv_cfg_.dst(), err, phv);
      return true; // (=> NO drop)
    }
    inline void set_perr_phv_cfg(int chan, uint32_t cfg) {
      prs_main_err_phv_cfg_.write(0, cfg);
    }
    inline void set_perr_phv_output(const int chan, const uint32_t err_flag, const int dst_word) {
      uint32_t cfg = err_flag;
      cfg |= dst_word << 23;
      set_perr_phv_cfg(chan, cfg);
    }

    inline void prs_reg_intr_status_err_set(uint32_t e) {
      if ((e & kErrNoTcamMatch) != 0u)  prs_main_intr_status_.no_tcam_match_err(0x1);
      if ((e & kErrPartialHdr) != 0u)   prs_main_intr_status_.partial_hdr_err(0x1);
      if ((e & kErrCtrRange) != 0u)     prs_main_intr_status_.ctr_range_err(0x1);
      if ((e & kErrTimeoutIter) != 0u)  prs_main_intr_status_.timeout_iter_err(0x1);
      if ((e & kErrTimeoutCycle) != 0u) prs_main_intr_status_.timeout_cycle_err(0x1);
      if ((e & kErrSrcExt) != 0u)       prs_main_intr_status_.src_ext_err(0x1);
      if ((e & kErrDstCont) != 0u)      prs_main_intr_status_.dst_cont_err(0x1);
      if ((e & kErrPhvOwner) != 0u)     prs_main_intr_status_.phv_owner_err(0x1);
      if ((e & kErrMultiWr) != 0u)      prs_main_intr_status_.multi_wr_err(0x1);
      if ((e & kErrARAMsbe) != 0u)      prs_main_intr_status_.aram_sbe(0x1);
      if ((e & kErrARAMmbe) != 0u)      prs_main_intr_status_.aram_mbe(0x1);
      if ((e & kErrFcs) != 0u)          prs_main_intr_status_.fcs_err(0x1);
      if ((e & kErrCsum) != 0u)         prs_main_intr_status_.csum_err(0x1);
      if ((e & kErrIbufOflow) != 0u)    prs_main_intr_status_.ibuf_oflow_err(0x1);
      if ((e & kErrIbufUflow) != 0u)    prs_main_intr_status_.ibuf_uflow_err(0x1);
      if ((e & kErrOpFifoOflow) != 0u)  prs_main_intr_status_.op_fifo_oflow_err(0x1);
      if ((e & kErrOpFifoUflow) != 0u)  prs_main_intr_status_.op_fifo_uflow_err(0x1);
      if ((e & kErrTcamPar) != 0u)      prs_main_intr_status_.tcam_par_err(0x1);
    }
    inline void prs_reg_err_cnt_inc(uint32_t e, int inc) {
      if ((e & kErrDstCont) != 0u) prs_main_dst_cont_err_cnt_.cnt( prs_main_dst_cont_err_cnt_.cnt() + inc);
    }
    inline uint32_t prs_map_sw_errs_to_hw_errs(uint32_t e) {
      return e & kErrTofinoMask;
    }


    // Function to handle initial match byte load
    void inbuf0_maybe_get_initial(int chan, uint8_t *v8_3, uint8_t *v8_2, uint8_t *v8_1, uint8_t *v8_0);
    // Functions to handle different capabilities of inbuf1 (scratch pad buffer)
    void inbuf1_setup(Packet *pkt);
    void inbuf1_maybe_fill(int pi, uint8_t v8_3, uint8_t v8_2, uint8_t v8_1, uint8_t v8_0, int state_cnt);
    // Functions to handle different capabilities of inbuf2 (TS/Version buffer)
    void inbuf2_setup(Packet *pkt);
    void inbuf2_update_consts(uint32_t v);


    // Stub functions to handle absence of Counter Stack
    inline bool    cntrstack_push(int pi, int8_t cntr, bool propagate)    { return false; }
    inline bool    cntrstack_maybe_push(int pi, int8_t cntr, int8_t incr) { return false; }
    inline void    cntrstack_increment(int pi, int8_t cntr)               { }
    inline int8_t  cntrstack_pop(int pi, int8_t cntr)        { return cntr; }
    // Setters/Getters to regularise handling of ctr_load/ctr_ld_src
    const char    *counter_ctr_op_str(uint8_t op);
    uint32_t       counter_ctr_uops(int pi);
    inline uint8_t counter_ctr_op(int pi)                 { return ((ea_ram_->ctr_load(pi) & 1) << 1) | ((ea_ram_->ctr_ld_src(pi) & 1) << 0); }
    inline bool    counter_load(int pi)                   { return ((ea_ram_->ctr_load(pi) & 1) == 1); }
    inline bool    counter_load_src(int pi)               { return ((ea_ram_->ctr_ld_src(pi) & 1) == 1); }
    inline bool    counter_stack_push(int pi)             { RMT_ASSERT(0); return false; } // Just to allow linking
    inline bool    counter_stack_upd_w_top(int pi)        { RMT_ASSERT(0); return false; } // Just to allow linking
    inline void    set_counter_ctr_op(int pi, uint8_t v)  { ea_ram_->ctr_load(pi, (v>>1) & 1); ea_ram_->ctr_ld_src(pi, (v>>0) & 1); }
    inline void    set_counter_load(int pi, bool tf)      { ea_ram_->ctr_load(pi,tf ?1 :0); }
    inline void    set_counter_load_src(int pi, bool tf)  { ea_ram_->ctr_ld_src(pi,tf ?1 :0); }
    inline void    set_counter_stack_push(int pi, bool tf)      { } // Allow call but ignore to simplify utest logic
    inline void    set_counter_stack_upd_w_top(int pi, bool tf) { } // Allow call but ignore to simplify utest logic

    // Get/Set what ActionRam banks are enabled - *all* enabled on Tofino
    inline uint8_t  action_ram_en(int pi) { return 3; }
    inline void     set_action_ram_en(int pi, uint8_t en) { /* Silently ignore call on Tofino */ }

    // Setters/Getters to handle different lookup_offset/load config regs
    void    get_lookup_offset(int pi, int n, uint8_t *off, bool *load, int *which_buf);
    uint8_t prs_ea_field8_N_lookup_offset(int pi, int n);
    uint8_t prs_ea_field8_N_load(int pi, int n);
    uint8_t prs_ea_field16_lookup_offset(int pi);
    uint8_t prs_ea_field16_load(int pi);
    void    set_lookup_offset(int pi, int n, uint8_t off, bool load, int which_buf);
    void    prs_ea_set_field8_N_lookup_offset(int pi, int n, uint8_t v);
    void    prs_ea_set_field8_N_load(int pi, int n, uint8_t v);
    void    prs_ea_set_field16_lookup_offset(int pi, uint8_t v);
    void    prs_ea_set_field16_load(int pi, uint8_t v);

    // Allow Tofino style EarlyActionRAM initialisation
    void set_early_action(int index, uint8_t _counter_load_imm,
                          bool _counter_load_src, bool _counter_load,
                          bool _done, uint8_t _shift_amount,
                          uint8_t _field8_1_lookup_offset, uint8_t _field8_0_lookup_offset,
                          uint8_t _field16_lookup_offset,
                          bool _load_field8_1, bool _load_field8_0, bool _load_field16,
                          uint8_t _next_state_mask, uint8_t _next_state);
    void set_early_action(int index, uint8_t _counter_load_imm,
                          bool _counter_load_src, bool _counter_load,
                          bool _done, uint8_t _shift_amount,
                          uint8_t _field8_3_lookup_offset, uint8_t _field8_2_lookup_offset,
                          uint8_t _field8_1_lookup_offset, uint8_t _field8_0_lookup_offset,
                          bool _load_field8_3, bool _load_field8_2,
                          bool _load_field8_1, bool _load_field8_0,
                          uint8_t _next_state_mask, uint8_t _next_state);

    // Functions to handle different extraction capabilities
    bool extract_offset_check(uint8_t xt_type, int off, bool *allow_partial, uint8_t buf_occ) override {
      int off0 = 0, siz = buf_occ, off2 = kFirstByteInbuf2, max = kExtractSrcMax;
      if (allow_partial != NULL) *allow_partial = false;
      switch (xt_type) {
        case kExtractTypeNone: return true;
        case kExtractType8b:   return ((off >= off0) && (off <= siz-1)) || ((off >= off2) && (off <= max-1));
        case kExtractType16b:  return ((off >= off0) && (off <= siz-2)) || ((off >= off2) && (off <= max-2));
        case kExtractType32b:  return ((off >= off0) && (off <= siz-4)) || ((off >= off2) && (off <= max-4));
        default: RMT_ASSERT(0);
      }
    }
    inline int extract8_first_non_checksum(int n_xt, int ck8, int ck16, int ck32) {
      return xtrac_.extract8_first_non_checksum(n_xt, ck8, ck16, ck32);
    }
    inline int extract16_first_non_checksum(int n_xt, int ck8, int ck16, int ck32) {
      return xtrac_.extract16_first_non_checksum(n_xt, ck8, ck16, ck32);
    }
    inline int extract32_first_non_checksum(int n_xt, int ck8, int ck16, int ck32) {
      return xtrac_.extract32_first_non_checksum(n_xt, ck8, ck16, ck32);
    }
    inline bool extract8_available(int i_xt, int n_xt, int ck8, int ck16, int ck32) {
      return xtrac_.extract8_available(i_xt, n_xt, ck8, ck16, ck32);
    }
    inline bool extract16_available(int i_xt, int n_xt, int ck8, int ck16, int ck32) {
      return xtrac_.extract16_available(i_xt, n_xt, ck8, ck16, ck32);
    }
    inline bool extract32_available(int i_xt, int n_xt, int ck8, int ck16, int ck32) {
      return xtrac_.extract32_available(i_xt, n_xt, ck8, ck16, ck32);
    }
    bool extract8_src_check_needed(int i_xt, int n_xt, int ck8, int ck16, int ck32) override {
      return xtrac_.extract8_available(i_xt, n_xt, 0, 0, 0);
    }
    bool extract16_src_check_needed(int i_xt, int n_xt, int ck8, int ck16, int ck32) override {
      return xtrac_.extract16_available(i_xt, n_xt, 0, 0, 0);
    }
    bool extract32_src_check_needed(int i_xt, int n_xt, int ck8, int ck16, int ck32) override {
      return xtrac_.extract32_available(i_xt, n_xt, 0, 0, 0);
    }
    inline bool     extract8_src_imm_val(int pi,int i)            { return xtrac_.extract8_src_imm_val(pi, i); }
    inline bool     extract8_rot_imm_val(int pi,int i)            { return xtrac_.extract8_rot_imm_val(pi,i); }
    inline bool     extract8_add_off(int pi, int i)               { return xtrac_.extract8_add_off(pi, i); }
    inline uint8_t  extract8_src(int pi, int i, uint8_t mask)     { return xtrac_.extract8_src(pi, i, mask); }
    inline uint8_t  extract8_src_msb(int pi, int i, int bitpos)   { return xtrac_.extract8_src_msb(pi, i, bitpos); }
    inline uint16_t extract8_dst_phv(int pi, int i)               { return xtrac_.extract8_dst_phv(pi, i); }
    inline uint8_t  extract8_type(int pi,int i)                   { return xtrac_.extract8_type(pi, i); }
    inline bool     extract16_src_imm_val(int pi,int i)           { return xtrac_.extract16_src_imm_val(pi,i); }
    inline bool     extract16_rot_imm_val(int pi,int i)           { return xtrac_.extract16_rot_imm_val(pi,i); }
    inline bool     extract16_add_off(int pi, int i)              { return xtrac_.extract16_add_off(pi, i); }
    inline uint8_t  extract16_src(int pi, int i, uint8_t mask)    { return xtrac_.extract16_src(pi, i, mask); }
    inline uint8_t  extract16_src_msb(int pi, int i, int bitpos)  { return xtrac_.extract16_src_msb(pi, i, bitpos); }
    inline uint16_t extract16_dst_phv(int pi, int i)              { return xtrac_.extract16_dst_phv(pi, i); }
    inline uint16_t extract16_dst_phv_by_phv(int pi, int i)       { return xtrac_.extract16_dst_phv(pi, i); }
    inline uint8_t  extract16_type(int pi,int i)                  { return xtrac_.extract16_type(pi, i); }
    inline bool     extract32_src_imm_val(int pi,int i)           { return xtrac_.extract32_src_imm_val(pi,i); }
    inline bool     extract32_rot_imm_val(int pi,int i)           { return xtrac_.extract32_rot_imm_val(pi,i); }
    inline bool     extract32_add_off(int pi, int i)              { return xtrac_.extract32_add_off(pi, i); }
    inline uint8_t  extract32_src(int pi, int i, uint8_t mask)    { return xtrac_.extract32_src(pi, i, mask); }
    inline uint8_t  extract32_src_msb(int pi, int i, int bitpos)  { return xtrac_.extract32_src_msb(pi, i, bitpos); }
    inline uint16_t extract32_dst_phv(int pi, int i)              { return xtrac_.extract32_dst_phv(pi, i); }
    inline uint8_t  extract32_type(int pi,int i)                  { return xtrac_.extract32_type(pi, i); }
    inline uint8_t  extract8_src_const(int pi, int i, uint8_t m, int rot)  { return xtrac_.extract8_src_const(pi, i, m, rot); }
    inline uint16_t extract16_src_const(int pi, int i, uint8_t m, int rot) { return xtrac_.extract16_src_const(pi, i, m, rot); }
    inline uint32_t extract32_src_const(int pi, int i, uint8_t m, int rot) { return xtrac_.extract32_src_const(pi, i, m, rot); }
    inline uint8_t  extract8_inbuf_shift(int pi, int i)            { return xtrac_.extract8_inbuf_shift(pi, i); }
    inline uint8_t  extract16_inbuf_shift(int pi, int i)           { return xtrac_.extract16_inbuf_shift(pi, i); }
    inline uint8_t  extract32_inbuf_shift(int pi, int i)           { return xtrac_.extract32_inbuf_shift(pi, i); }
    inline uint32_t extract_immediate_consts(int pi, int i, int rot, uint32_t dflt) {
      return xtrac_.extract_immediate_consts(pi, i, rot, dflt);
    }

    inline void set_extract8_src_imm_val(int pi, int i, bool tf)  { xtrac_.set_extract8_src_imm_val(pi, i, tf); }
    inline void set_extract8_rot_imm_val(int pi, int i, bool tf)  { xtrac_.set_extract8_rot_imm_val(pi, i, tf); }
    inline void set_extract8_add_off(int pi, int i, bool tf)      { xtrac_.set_extract8_add_off(pi, i, tf); }
    inline void set_extract8_src(int pi, int i, uint8_t v)        { xtrac_.set_extract8_src(pi, i, v); }
    inline void set_extract8_dst_phv(int pi, int i, uint16_t v)   { xtrac_.set_extract8_dst_phv(pi, i, v); }
    inline void set_extract8_type(int pi, int i, uint8_t v)       { xtrac_.set_extract8_type(pi, i, v); }
    inline void set_extract16_src_imm_val(int pi, int i, bool tf) { xtrac_.set_extract16_src_imm_val(pi, i, tf); }
    inline void set_extract16_rot_imm_val(int pi, int i, bool tf) { xtrac_.set_extract16_rot_imm_val(pi, i, tf); }
    inline void set_extract16_add_off(int pi, int i, bool tf)     { xtrac_.set_extract16_add_off(pi, i, tf); }
    inline void set_extract16_src(int pi, int i, uint8_t v)       { xtrac_.set_extract16_src(pi, i,  v); }
    inline void set_extract16_dst_phv(int pi, int i, uint16_t v)  { xtrac_.set_extract16_dst_phv(pi, i, v); }
    inline uint8_t set_extract16_dst_phv_by_phv(int pi, int i, uint16_t v)  { xtrac_.set_extract16_dst_phv(pi, i, v); return 0; }
    inline void set_extract16_type(int pi, int i, uint8_t v)      { xtrac_.set_extract16_type(pi, i, v); }
    inline void set_extract32_src_imm_val(int pi, int i, bool tf) { xtrac_.set_extract32_src_imm_val(pi, i, tf); }
    inline void set_extract32_rot_imm_val(int pi, int i, bool tf) { xtrac_.set_extract32_rot_imm_val(pi, i, tf); }
    inline void set_extract32_add_off(int pi, int i, bool tf)     { xtrac_.set_extract32_add_off(pi, i, tf); }
    inline void set_extract32_src(int pi, int i, uint8_t v)       { xtrac_.set_extract32_src(pi, i, v); }
    inline void set_extract32_dst_phv(int pi, int i, uint16_t v)  { xtrac_.set_extract32_dst_phv(pi, i, v); }
    inline void set_extract32_type(int pi, int i, uint8_t v)      { xtrac_.set_extract32_type(pi, i, v); }

    // These offset/checksum/pri_upd funcs now need to be per-chip because of WIP act_ram reorganisation
    inline bool    offset_reset(int pi)                           { return ((act_ram_->dst_offset_rst(pi) & 0x1) == 0x1); }
    inline uint8_t offset_incr_val(int pi)                        { return act_ram_->dst_offset_inc(pi); }
    inline bool    checksum_enable(int pi, int i)                 { return ((act_ram_->csum_en(pi,i) & 0x1) == 0x1); }
    inline uint8_t checksum_ram_addr(int pi, int i)               { return act_ram_->csum_addr(pi,i); }
    inline uint8_t pri_upd_type(int pi)                           { return (act_ram_->pri_upd_type(pi) & 0x1); }
    inline uint8_t pri_upd_src(int pi)                            { return (act_ram_->pri_upd_src(pi) & 0x1F); }
    inline uint8_t pri_upd_en_shr(int pi)                         { return (act_ram_->pri_upd_en_shr(pi) & 0xF); }
    inline uint8_t pri_upd_val_mask(int pi)                       { return (act_ram_->pri_upd_val_mask(pi) & 0x7); }

    inline void set_offset_reset(int pi, bool tf)                 { act_ram_->dst_offset_rst(pi,tf ?0x1 :0x0); }
    inline void set_offset_incr_val(int pi, uint8_t v)            { act_ram_->dst_offset_inc(pi,v); }
    inline void set_checksum_enable(int pi, int i, bool tf)       { act_ram_->csum_en(pi,i,tf ?0x1 :0x0); }
    inline void set_checksum_ram_addr(int pi, int i, uint8_t v)   { act_ram_->csum_addr(pi,i,v); }
    inline void set_pri_upd_type(int pi, uint8_t v)               { act_ram_->pri_upd_type(pi,v); }
    inline void set_pri_upd_src(int pi, uint8_t v)                { act_ram_->pri_upd_src(pi,v); }
    inline void set_pri_upd_en_shr(int pi, uint8_t v)             { act_ram_->pri_upd_en_shr(pi,v); }
    inline void set_pri_upd_val_mask(int pi, uint8_t v)           { act_ram_->pri_upd_val_mask(pi,v); }


    uint16_t map_dst_phv(uint16_t phv_in, int from_format);
    bool extract_ok_phv(int phv_word);
    bool extract_ok_tphv(int phv_word);
    int  get_phv_to_write(int phv_word);
    int  get_tphv_to_write(int phv_word);

    // Function to apply further extraction constraint checks
    void check_further_extract_constraints(uint32_t check_flags,
                                           int index, int which_extract,
                                           int phv_word, Phv *phv, int extract_sz,
                                           bool partial_hdr_err);
    // Function to apply checksum constraint checks
    bool check_checksum_constraints(int match_index,
                                    int &wrote_8b, int &wrote_16b, int &wrote_32b);

    // Function to handle different capabilties wrt Version Update
    uint8_t update_version(int index, uint8_t curr_ver);

    // Function to handle different capabilities wrt Priority Mapping
    uint8_t map_priority(int chan, uint8_t curr_pri);
    // Function to setup identity Priority Mapping
    void set_identity_priority_map();

    // Functions to handle different capabilities wrt Counter Masking
    inline uint8_t  pcnt_mask_width(int pi)             { return ctr_init_ram_->mask(pi); }
    inline uint8_t  pcnt_mask(int pi)                   { return pcnt_calc_mask(pcnt_mask_width(pi)); }
    inline void set_pcnt_mask_width(int pi, uint8_t v)  { ctr_init_ram_->mask(pi,v); }
    inline void set_pcnt_mask(int pi, uint8_t v)        {
      uint8_t width;
      for (width = 0; width < 8; width++) { if (pcnt_calc_mask(width) == v) break; }
      RMT_ASSERT(width < 8);
      set_pcnt_mask_width(pi, width);
    }
    // Functions to handle CounterRAM add_to_stack - specialized as does not exist on Tofino (only WIP)
    inline uint8_t  pcnt_add_to_stack(int pi)             { return false; }
    inline void set_pcnt_add_to_stack(int pi, uint8_t v)  { } // Allow call but ignore to simplify utest logic



    // Functions to handle NORMAL PHV owner/no_multi_wr fields
    inline uint8_t prs_reg_phv_owner_norm_get(register_classes::PrsrRegMainRspecPhvOwnerMutable *r, int w) {
      return Phv::is_valid_norm_phv_p(w) ?r->owner(w) & 1 :0;
    }
    inline void prs_reg_phv_owner_norm_set(register_classes::PrsrRegMainRspecPhvOwnerMutable *r, int w, uint8_t v) {
      if (Phv::is_valid_norm_phv_p(w)) r->owner(w, v & 1);
    }
    inline uint8_t prs_reg_no_multi_wr_norm_get(register_classes::PrsrRegMainRspecNoMultiWrMutable *r, int w) {
      return Phv::is_valid_norm_phv_p(w) ?r->nmw(w) & 1 :0;
    }
    inline void prs_reg_no_multi_wr_norm_set(register_classes::PrsrRegMainRspecNoMultiWrMutable *r, int w, uint8_t v) {
      if (Phv::is_valid_norm_phv_p(w)) r->nmw(w, v & 1);
    }
    // Functions to handle TAGALONG PHV owner/no_multi_wr fields
    inline uint8_t prs_reg_phv_owner_taga_get(register_classes::PrsrRegMainRspecPhvOwnerMutable *r, int w) {
      return Phv::is_valid_taga_phv_p(w) ?r->t_owner(w - k_phv::kTagalongStart) & 1 :0;
    }
    inline void prs_reg_phv_owner_taga_set(register_classes::PrsrRegMainRspecPhvOwnerMutable *r, int w, uint8_t v) {
      if (Phv::is_valid_taga_phv_p(w)) r->t_owner(w - k_phv::kTagalongStart, v & 1);
    }
    inline uint8_t prs_reg_no_multi_wr_taga_get(register_classes::PrsrRegMainRspecNoMultiWrMutable *r, int w) {
      return Phv::is_valid_taga_phv_p(w) ?r->t_nmw(w - k_phv::kTagalongStart) & 1 :0;
    }
    inline void prs_reg_no_multi_wr_taga_set(register_classes::PrsrRegMainRspecNoMultiWrMutable *r, int w, uint8_t v) {
      if (Phv::is_valid_taga_phv_p(w)) r->t_nmw(w - k_phv::kTagalongStart, v & 1);
    }


    // Functions to handle whether PHV writes are allowed
    inline bool phv_can_write(Phv *phv, int phv_or_off) {
      return ((Phv::is_valid_phv_x(phv_or_off)) && (phv_owner(phv_or_off)) &&
              (phv_multi_write(phv_or_off) || !phv->was_written_x(phv_or_off)));
    }
    inline bool phv_can_write_16b(Phv *phv, int phv_or_off) {
      if (Phv::which_width_p(phv_or_off) == 8)
        return phv_can_write(phv, phv_or_off) && phv_can_write(phv, phv_or_off+1);
      else
        return phv_can_write(phv, phv_or_off);
    }
    inline bool phv_owner_16b(Phv *phv, int phv_or_off) {
      if (Phv::which_width_p(phv_or_off) == 8)
        return phv_owner(phv_or_off) && phv_owner(phv_or_off+1);
      else
        return phv_owner(phv_or_off);
    }
    inline bool phv_valid_16b(Phv *phv, int phv_or_off) {
      if (Phv::which_width_p(phv_or_off) == 8)
        return Phv::is_valid_phv_x(phv_or_off) && Phv::is_valid_phv_x(phv_or_off+1);
      else
        return Phv::is_valid_phv_x(phv_or_off);
    }


    // Access checksum engines
    inline ChecksumEngine *get_checksum_engine(int ci) { return &checksum_engine_[ci]; }

    // Access tcam row arrays
    inline memory_classes::PrsrMlTcamRowArray *get_prs_word0s() { return &prs_tcam_word0s_; }
    inline memory_classes::PrsrMlTcamRowArray *get_prs_word1s() { return &prs_tcam_word1s_; }
    // Code to extract TCAM vals from row array into BitVector to program s/w TCAM
    BitVector<44> prs_tcam_get_entry(memory_classes::PrsrMlTcamRowArray &prs, uint32_t i);
    // Called on TCAM row array change - called from memory_change_callback
    void prs_tcam_change(uint32_t n);

    // tofinoxx-specific funcs to handle hdr_len update
    virtual void update_hdr_len(Packet *p, int index, uint32_t err_flags, uint8_t shift);

    // Fill other gress PHV containers with junk
    void phv_fill_other_gress(Phv* phv);

 private:
    bool                                                        reset_running_;
    register_classes::PrsrRegMainRspecErrPhvCfgMutable          prs_main_err_phv_cfg_;
    register_classes::PrsrRegMainRspecEnableMutable             prs_main_enable_;
    register_classes::PrsrRegMainRspecModeMutable               prs_main_mode_;
    register_classes::PrsrRegMainRspecDstContErrCntMutable      prs_main_dst_cont_err_cnt_;
    register_classes::PrsrRegMainRspecIntrStatusMutable         prs_main_intr_status_;
    register_classes::PrsrRegMainRspecIntrEnable0Mutable        prs_main_intr_enable0_;
    register_classes::PrsrRegMainRspecIntrEnable1Mutable        prs_main_intr_enable1_;
    register_classes::PrsrRegMainRspecIntrFreezeEnableMutable   prs_main_intr_freeze_enable_;
    register_classes::PrsrRegMainRspecIntrInjectMutable         prs_main_intr_inject_;
    register_classes::PrsrRegMainRspecDstContErrlogMutable      prs_main_dst_cont_errlog_;
    register_classes::PrsrRegMainRspecAramMbeErrlogMutable      prs_main_aram_mbe_errlog_;
    register_classes::PrsrRegMainRspecAramSbeErrlogMutable      prs_main_aram_sbe_errlog_;
    register_classes::PrsrRegMainRspecCtrRangeErrlogMutable     prs_main_ctr_range_errlog_;
    register_classes::PrsrRegMainRspecIbufOflowErrlogMutable    prs_main_ibuf_oflow_errlog_;
    register_classes::PrsrRegMainRspecIbufUflowErrlogMutable    prs_main_ibuf_uflow_errlog_;
    register_classes::PrsrRegMainRspecMultiWrErrlogMutable      prs_main_multi_wr_errlog_;
    register_classes::PrsrRegMainRspecNoTcamMatchErrlogMutable  prs_main_no_tcam_match_errlog_;
    register_classes::PrsrRegMainRspecOpFifoOflowErrlogMutable  prs_main_op_fifo_oflow_errlog_;
    register_classes::PrsrRegMainRspecOpFifoUflowErrlogMutable  prs_main_op_fifo_uflow_errlog_;
    register_classes::PrsrRegMainRspecPartialHdrErrlogMutable   prs_main_partial_hdr_errlog_;
    register_classes::PrsrRegMainRspecPhvOwnerErrlogMutable     prs_main_phv_owner_errlog_;
    register_classes::PrsrRegMainRspecSrcExtErrlogMutable       prs_main_src_ext_errlog_;
    register_classes::PrsrRegMainRspecTcamParErrlogMutable      prs_main_tcam_par_errlog_;
    register_classes::ScratchRMutable                           scratch_;
    register_classes::PrsrRegMainRspecAEmpThreshMutable         prs_main_a_emp_thresh_;

    std::array<ChecksumEngine,kChecksumEngines>                 checksum_engine_;
    memory_classes::PrsrMlTcamRowArray                          prs_tcam_word0s_;
    memory_classes::PrsrMlTcamRowArray                          prs_tcam_word1s_;
    memory_classes::PrsrMlEaRowArrayMutable                    *ea_ram_;
    memory_classes::PrsrPoActionRowArrayMutable                *act_ram_;
    memory_classes::PrsrMlCtrInitRamMArrayMutable              *ctr_init_ram_;
    ParserExtractorsAll                                         xtrac_;

  };
}

#endif // _TOFINOXX_PARSER_
