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

#ifndef _JBAY_SHARED_PARSER_
#define _JBAY_SHARED_PARSER_

#include <parser-shared.h>
#include <parser-counter-stack.h>

#include <register_includes/prsr_reg_main_rspec_err_phv_cfg_array_mutable.h>
#include <register_includes/prsr_reg_main_rspec_port_rate_cfg_mutable.h>
#include <register_includes/prsr_reg_main_rspec_port_chnl_en_mutable.h>
#include <register_includes/prsr_reg_main_rspec_start_lookup_offsets_array_mutable.h>
#include <register_includes/prsr_reg_main_rspec_pkt_rx_cnt_array_mutable.h>
#include <register_includes/prsr_reg_main_rspec_pkt_tx_cnt_array_mutable.h>
//#include <register_includes/prsr_reg_main_rspec_narrow_to_wide_err_cnt_mutable.h>
//#include <register_includes/prsr_reg_main_rspec_dst_cont_err_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_pri_map_array_mutable.h>
#include <register_includes/prsr_reg_main_rspec_intr_stat_mutable.h>
// These registers are instantiated but are NOT used - however allows read/write
#include <register_includes/prsr_reg_main_rspec_intr_en0_mutable.h>
#include <register_includes/prsr_reg_main_rspec_intr_en1_mutable.h>
#include <register_includes/prsr_reg_main_rspec_intr_freeze_enable_mutable.h>
#include <register_includes/prsr_reg_main_rspec_intr_inj_mutable.h>
#include <register_includes/prsr_reg_main_rspec_aram_mbe_err_log_mutable.h>
#include <register_includes/prsr_reg_main_rspec_aram_sbe_err_log_mutable.h>
#include <register_includes/prsr_reg_main_rspec_ctr_range_err_log_mutable.h>
#include <register_includes/prsr_reg_main_rspec_ibuf_oflow_err_log_mutable.h>
#include <register_includes/prsr_reg_main_rspec_ibuf_uflow_err_log_mutable.h>
#include <register_includes/prsr_reg_main_rspec_multi_wr_err_log_mutable.h>
#include <register_includes/prsr_reg_main_rspec_no_tcam_match_err_log_mutable.h>
#include <register_includes/prsr_reg_main_rspec_op_fifo_oflow_err_log_mutable.h>
#include <register_includes/prsr_reg_main_rspec_op_fifo_uflow_err_log_mutable.h>
#include <register_includes/prsr_reg_main_rspec_partial_hdr_err_log_mutable.h>
#include <register_includes/prsr_reg_main_rspec_phv_owner_err_log_mutable.h>
#include <register_includes/prsr_reg_main_rspec_src_ext_err_log_mutable.h>
#include <register_includes/prsr_reg_main_rspec_tcam_par_err_log_mutable.h>
//#include <register_includes/prsr_reg_main_rspec_narrow_to_wide_err_log_mutable.h>
//#include <register_includes/prsr_reg_main_rspec_dst_cont_errlog_mutable.h>
#include <register_includes/prsr_reg_main_rspec_mem_ctrl_mutable.h>
#include <register_includes/prsr_reg_main_rspec_phv_clr_on_wr_mutable.h>

namespace MODEL_CHIP_NAMESPACE {

  class Parser : public ParserShared {

 private:
    void reset_this();
    void phv_clr_on_wr_validate();
    void phv_owner_validate();

 public:
    static const char *kCounterOpStrings[];
    static inline bool off16_ok(int off16) { return ((off16 >= 0) && (off16 < 256)); }

    Parser(RmtObjectManager *om, int pipeIndex, int prsIndex, int ioIndex,
           const ParserConfig &config);
    virtual ~Parser();
    void reset();
    void counter_reset();
    void not_implemented(const char *clazz);
    void memory_change_callback(uint8_t mem_type, uint8_t mem_inst, uint32_t mem_index);
    void memory_ctrl_callback();
    void phv_clr_on_wr_change_callback();
    void phv_owner_change_callback();
    void parse_start(Packet *pkt, Phv *phv);
    void parse_end_state(Packet *pkt, Phv *phv, int state_cnt, bool final);
    Phv *get_ghost_phv();
    int phv_increment(int phv, int incr);


    // Functions to handle varying top-level registers
    inline int parserChan(int ci)         { return (RmtDefs::get_parser_element(parser_index()) * 2) + (ci % 2); }
    // More complex derivation of ipb_chan on JBay
    inline int ipb_chan(int ci)           { return parserChan(ci); }

    inline uint8_t  port_rate_cfg(int ci) { return prs_main_port_rate_cfg_.chnl_rate(ci); }
    inline uint8_t  port_chnl_en(int ci)  { return prs_main_port_chnl_en_.enbl(); }
    // enabled() used to index 8 port_chnl_en bits prior to regs 45907_parde_jbay - see old func below
    //inline bool   enabled(int ci)       { return ((port_chnl_en(ci) & (1<<parserChan(ci))) != 0); }
    inline bool     enabled(int ci)       { return ((port_chnl_en(ci) & (1<<ci)) != 0); }
           uint8_t  hdr_len_adj();
           void set_hdr_len_adj(uint8_t v);
    inline uint16_t hdr_max_len()         { return get_hdr_len_adj()->max_len(); }
    inline uint8_t  mode()                { return 0; }
    inline uint8_t  mem_ctrl()            { return prs_main_mem_ctrl_.wr_addr(); }
    inline uint64_t rx_count(int ci)      { return prs_main_pkt_rx_cnt_.cnt(ci); }
    inline uint64_t tx_count(int ci)      { return prs_main_pkt_tx_cnt_.cnt(ci); }

    inline void set_port_rate_cfg(int ci, uint8_t v) { prs_main_port_rate_cfg_.chnl_rate(ci, v); }
    inline void set_port_chnl_en(int ci, uint8_t v)  { prs_main_port_chnl_en_.enbl(v); }
    inline void set_enabled(int ci, bool tf)         {
      // set_enabled used to index 8 port_chnl_en bits prior to regs 45907_parde_jbay
      int  pchan = ci; //int  pchan = parserChan(ci);
      set_port_chnl_en(ci, (port_chnl_en(ci) & ~(1<<pchan)) | ((tf?1:0)<<pchan));
    }
    inline void set_hdr_max_len(uint16_t v)          { get_hdr_len_adj()->max_len(v); }
    inline void set_mode(uint8_t v)                  { }
    inline void set_mem_ctrl(uint8_t v)              { prs_main_mem_ctrl_.wr_addr(v); memory_ctrl_callback(); }
    inline void set_drop_count(int ci, uint64_t v)   { set_drop_count_raw(ci, v); }
    inline void set_rx_count(int ci, uint64_t v)     { prs_main_pkt_rx_cnt_.cnt(ci,v); }
    inline void set_tx_count(int ci, uint64_t v)     { prs_main_pkt_tx_cnt_.cnt(ci,v); }
    inline void inc_rx_count(int ci)                 { prs_main_pkt_rx_cnt_.cnt(ci, 1 + prs_main_pkt_rx_cnt_.cnt(ci)); }
    void inc_drop_count(int ci);
    void inc_tx_count(int ci);


    inline bool perr_phv_output(int chan, uint32_t err, Phv *phv) {
      // XXX: err_phv_cfg needs to be indexed by chan on JBay
      if (err == 0u) return true;            // No error whatsoever (=> NO drop)
      uint32_t nodrop_mask = kErrNoDropMask; // Certain errors (csum) unconditionally mean NO drop
      // Other errors can be enabled/disabled to determine whether they provoke drop or not
      // XXX: should ignore these (treat as 1) on egress
      bool egr = egress();
      if (egr || prs_main_err_phv_cfg_.no_tcam_match_err_en(chan)) nodrop_mask |= kErrNoTcamMatch;
      if (egr || prs_main_err_phv_cfg_.partial_hdr_err_en(chan))   nodrop_mask |= kErrPartialHdr;
      if (egr || prs_main_err_phv_cfg_.ctr_range_err_en(chan))     nodrop_mask |= kErrCtrRange;
      if (egr || prs_main_err_phv_cfg_.timeout_iter_err_en(chan))  nodrop_mask |= kErrTimeoutIter;
      if (egr || prs_main_err_phv_cfg_.timeout_cycle_err_en(chan)) nodrop_mask |= kErrTimeoutCycle;
      if (egr || prs_main_err_phv_cfg_.src_ext_err_en(chan))       nodrop_mask |= kErrSrcExt;
      //if (egr || prs_main_err_phv_cfg_.dst_cont_err_en())      nodrop_mask |= kErrDstCont;
      if (egr || prs_main_err_phv_cfg_.phv_owner_err_en(chan))     nodrop_mask |= kErrPhvOwner;
      if (egr || prs_main_err_phv_cfg_.multi_wr_err_en(chan))      nodrop_mask |= kErrMultiWr;
      //if (egr || prs_main_err_phv_cfg_.aram_sbe_en(chan))        nodrop_mask |= kErrARAMsbe;
      if (egr || prs_main_err_phv_cfg_.aram_mbe_en(chan))          nodrop_mask |= kErrARAMmbe;
      if (egr || prs_main_err_phv_cfg_.fcs_err_en(chan))           nodrop_mask |= kErrFcs;
      //if (egr || prs_main_err_phv_cfg_.csum_err_en())          nodrop_mask |= kErrCsum;
      //if (egr || prs_main_err_phv_cfg_.ibuf_oflow_err_en())    nodrop_mask |= kErrIbufOflow;
      //if (egr || prs_main_err_phv_cfg_.ibuf_uflow_err_en())    nodrop_mask |= kErrIbufUflow;
      //if (egr || prs_main_err_phv_cfg_.op_fifo_oflow_err_en()) nodrop_mask |= kErrOpFifoOflow;
      //if (egr || prs_main_err_phv_cfg_.op_fifo_uflow_err_en()) nodrop_mask |= kErrOpFifoUflow;
      //if (egr || prs_main_err_phv_cfg_.tcam_par_en())          nodrop_mask |= kErrTcamPar;
      //if (egr || prs_main_err_phv_cfg_.narrow_to_wide())       nodrop_mask |= kErrNarrowToWide;
      //if (egr || prs_main_err_phv_cfg_.csum_config_err_en())   nodrop_mask |= kErrCsumConfig ;
      err &= nodrop_mask;
      if (err == 0u) return false; // No 'no drop' errs so return false (=> DROP)
      // We know we're not dropping this PHV but what errors do we actually report in PHV
      err &= perr_phv_mask();      // By dflt perr_phv_mask_ = kErrDfltRepMask
      if (err == 0u) return true;  // But nothing to report (=> NO drop though)
      // Write errors to configured dst PHV word
      if (prs_main_err_phv_cfg_.en(chan))
        perr_phv_write(prs_main_err_phv_cfg_.dst(chan), err, phv);
      return true; // (=> NO drop)
    }
    inline void set_perr_phv_cfg(int chan, uint32_t cfg) {
      prs_main_err_phv_cfg_.write(chan, cfg);
    }
    inline void set_perr_phv_output(const int chan, const uint32_t err_flag, const int dst_word) {
      // for tests: set dst word to which parser errors will be written; if
      // dst_word maps to a 32b word then the lower 16b index is used
      uint32_t cfg = err_flag;
      int size, off16A, off16B, sz8_01;
      Phv::phv_index_to_off16_p(dst_word, &size, &off16A, &off16B, &sz8_01);
      cfg |= off16A << 24;
      cfg |= (1 << 23);  // enable bit
      set_perr_phv_cfg(chan, cfg);
    }
    inline void prs_reg_intr_status_err_set(uint32_t e) {
      if ((e & kErrNoTcamMatch) != 0u)  prs_main_intr_status_.no_tcam_match_err(0x1);
      if ((e & kErrPartialHdr) != 0u)   prs_main_intr_status_.partial_hdr_err(0x1);
      if ((e & kErrCtrRange) != 0u)     prs_main_intr_status_.ctr_range_err(0x1);
      if ((e & kErrTimeoutIter) != 0u)  prs_main_intr_status_.timeout_iter_err(0x1);
      if ((e & kErrTimeoutCycle) != 0u) prs_main_intr_status_.timeout_cycle_err(0x1);
      if ((e & kErrSrcExt) != 0u)       prs_main_intr_status_.src_ext_err(0x1);
      //if ((e & kErrDstCont) != 0u)      prs_main_intr_status_.dst_cont_err(0x1);
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
      //if ((e & kErrNarrowToWide) != 0u) prs_main_intr_status_.narrow_to_wide(0x1);
    }
    inline void prs_reg_err_cnt_inc(uint32_t e, int inc) {
      //if ((e & kErrDstCont) != 0u)    prs_main_dst_cont_err_cnt_.cnt( prs_main_dst_cont_err_cnt_.cnt() + inc);
      //if ((e & kErrNarrowToWide) != 0u) prs_main_narrow_to_wide_err_cnt_.cnt( prs_main_narrow_to_wide_err_cnt_.cnt() + inc);
    }
    inline uint32_t prs_map_sw_errs_to_hw_errs(uint32_t e) {
      return e & kErrJbayMask;
    }

    // Write 16b to a PHV off16
    void     phv_write_16b(Phv *phv, int phv_or_off, uint16_t val, ParseContext *context=nullptr);
    void     phv_write_16b(Phv *phv, int phv_or_off, uint16_t val, uint8_t extract_type, ParseContext *context=nullptr);
    uint32_t phv_check_write_16b(Phv *phv, int phv_or_off, uint16_t val, ParseContext *context=nullptr);
    uint32_t phv_check_write_16b(Phv *phv, int phv_or_off, uint16_t val, uint8_t extract_type, ParseContext *context=nullptr);

    // JBay specific func to actually write error flags to PHV word(s)
    void perr_phv_write(uint16_t dst, uint32_t err, Phv *phv);
    // JBay specific func to write TM status to PHV word(s)
    void tm_status_phv_write(uint32_t tm_status, Phv *phv, uint8_t word);
    void tm_status_phv_write_msb(uint32_t tm_status, Phv *phv);
    void tm_status_phv_write_lsb(uint32_t tm_status, Phv *phv);

    inline bool phv_get_tm_status_phv(int pipe, uint8_t *phv_or_off ) {
      ParseMergeReg *prs_merge = get_prs_merge();
      if (prs_merge == nullptr) return false;
      return prs_merge->phv_get_tm_status_phv(pipe, phv_or_off);
    }
    inline void phv_set_tm_status_phv(int pipe, uint8_t phv_or_off) {
      ParseMergeReg *prs_merge = get_prs_merge();
      if (prs_merge != nullptr) prs_merge->phv_set_tm_status_phv(pipe, phv_or_off);
    }
    inline bool phv_get_tm_status_phv_sec(int pipe, uint8_t *phv_or_off, uint8_t *which_half) {
      // WIP only
      ParseMergeReg *prs_merge = get_prs_merge();
      if (prs_merge == nullptr) return false;
      return prs_merge->phv_get_tm_status_phv_sec(pipe, phv_or_off, which_half);
    }
    inline void phv_set_tm_status_phv_sec(uint8_t phv_or_off, uint8_t which_half) {
      // WIP only
      // Note: phv_sec is only enabled if phv_pri is enabled which implies that
      // phv_set_tm_status_phv has been called and set pipe mask, so here we
      // just set phv_sec
      ParseMergeReg *prs_merge = get_prs_merge();
      if (prs_merge != nullptr) prs_merge->phv_set_tm_status_phv_sec(phv_or_off, which_half);
    }
    inline bool tm_status_input(uint32_t *val_msb, uint32_t *val_lsb=nullptr) {
      ParseMergeReg *prs_merge = get_prs_merge();
      if (prs_merge == nullptr) return false;
      return prs_merge->tm_status_input(val_msb, val_lsb);
    }
    inline bool set_tm_status_input(uint32_t val_msb, uint32_t val_lsb=0) {
      ParseMergeReg *prs_merge = get_prs_merge();
      if (prs_merge == nullptr) return false;
      return prs_merge->set_tm_status_input(val_msb, val_lsb);
    }


    // Function to handle initial match byte load (from start_lookup_offsets CSR)
    void inbuf0_maybe_get_initial(int chan, uint8_t *v8_3, uint8_t *v8_2, uint8_t *v8_1, uint8_t *v8_0);
    inline uint8_t  start_lookup_offsets(int ci, int bi)            { return prs_main_start_lookup_offsets_.offset(ci,bi); }
    inline void set_start_lookup_offsets(int ci, int bi, uint8_t v) { prs_main_start_lookup_offsets_.offset(ci,bi,v); }
    // XXX: fix incorrect sense of save_lookup below
    inline bool     save_lookup(int pi, int bi)                     { return (ea_ram_->sv_lookup_8(pi,bi) == 1); }
    inline void set_save_lookup(int pi, int bi, bool tf)            { ea_ram_->sv_lookup_8(pi,bi,tf?1:0); }
    // Functions to handle different capabilities of inbuf1 (NO scratch pad buffer)
    void inbuf1_setup(Packet *pkt);
    void inbuf1_maybe_fill(int pi, uint8_t v8_3, uint8_t v8_2, uint8_t v8_1, uint8_t v8_0, int state_cnt);
    //inline bool scratch_upd(int pi)              { RMT_ASSERT(0); return false; }
    //inline void set_scratch_upd(int pi, bool tf) { RMT_ASSERT(0); }
    // Functions to handle different capabilities of inbuf2 (TS/Version buffer)
    void inbuf2_setup(Packet *pkt);
    void inbuf2_update_consts(uint32_t v);


    // Counter Stack handling
    bool   cntrstack_push(int pi, int8_t cntr, bool propagate);
    bool   cntrstack_maybe_push(int pi, int8_t cntr, int8_t incr);
    void   cntrstack_increment(int pi, int8_t cntr);
    int8_t cntrstack_pop(int pi, int8_t cntr);
    void     cntrstack_reset() { ctr_stack_.reset(); }
    uint32_t cntrstack_hash() { return ctr_stack_.hash(); }
    int      cntrstack_peek(int8_t cntrs[], bool propagates[], int size) {
      return ctr_stack_.peek(cntrs, propagates, size);
    }
    uint64_t cntrstack_counter_data(int8_t curr_cntr) {
      return ctr_stack_.peek64(curr_cntr);
    }

    // Setters/Getters to regularise handling of ctr_op (formerly ctr_load/ctr_ld_src)
    const char     *counter_ctr_op_str(uint8_t op);
    uint32_t        counter_ctr_uops(int pi);
    inline uint8_t  counter_ctr_op(int pi)                   { return ea_ram_->ctr_op(pi); }
    bool            counter_stack_push(int pi);
    inline bool     counter_stack_upd_w_top(int pi)          { return ((ea_ram_->ctr_stack_upd_w_top(pi) & 1) == 1); }
    void        set_counter_ctr_op4(int pi, uint8_t v);
    void        set_counter_ctr_op2(int pi, uint8_t v);
    void        set_counter_ctr_op(int pi, uint8_t v);
    void        set_counter_stack_push(int pi, bool tf);
    inline void set_counter_stack_upd_w_top(int pi, bool tf) { ea_ram_->ctr_stack_upd_w_top(pi, tf?1:0); }
    // JBay/WIP code should always call counter_ctr_op and set_counter_ctr_op
    inline bool     counter_load(int pi)                     { RMT_ASSERT(0); }
    inline bool     counter_load_src(int pi)                 { RMT_ASSERT(0); }
    inline void set_counter_load(int pi, bool tf)            { RMT_ASSERT(0); }
    inline void set_counter_load_src(int pi, bool tf)        { RMT_ASSERT(0); }

    // Get/Set what ActionRam banks are enabled - *all* enabled on JBay
    uint8_t         action_ram_en(int pi);
    void        set_action_ram_en(int pi, uint8_t en);

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

    // Pure Jbay variant that takes 2b _counter_ctr_op
    void set_early_action(int index, uint8_t _counter_load_imm,
                          uint8_t _counter_ctr_op, bool _done, uint8_t _shift_amount,
                          uint8_t _field8_3_lookup_offset, uint8_t _field8_2_lookup_offset,
                          uint8_t _field8_1_lookup_offset, uint8_t _field8_0_lookup_offset,
                          bool _load_field8_3, bool _load_field8_2,
                          bool _load_field8_1, bool _load_field8_0,
                          uint8_t _next_state_mask, uint8_t _next_state);
    // Also allow both Tofino style EarlyActionRAM initialisation
    void set_early_action(int index, uint8_t _counter_load_imm,
                          bool _counter_load_src, bool _counter_load,
                          bool _done, uint8_t _shift_amount,
                          uint8_t _field8_3_lookup_offset, uint8_t _field8_2_lookup_offset,
                          uint8_t _field8_1_lookup_offset, uint8_t _field8_0_lookup_offset,
                          bool _load_field8_3, bool _load_field8_2,
                          bool _load_field8_1, bool _load_field8_0,
                          uint8_t _next_state_mask, uint8_t _next_state);
    void set_early_action(int index, uint8_t _counter_load_imm,
                          bool _counter_load_src, bool _counter_load,
                          bool _done, uint8_t _shift_amount,
                          uint8_t _field8_1_lookup_offset, uint8_t _field8_0_lookup_offset,
                          uint8_t _field16_lookup_offset,
                          bool _load_field8_1, bool _load_field8_0, bool _load_field16,
                          uint8_t _next_state_mask, uint8_t _next_state);

    // Functions to handle different extraction capabilities
    bool extract_offset_check(uint8_t xt_type, int off, bool *allow_partial,
                              uint8_t buf_occ = kInputBufferSize) override {
      // JBay allows extraction from anywhere in the buffer, even beyond the buf_occ/buf_req amount
      int off0 = 0, siz = kInputBufferSize, off2 = kFirstByteInbuf2, max = kExtractSrcMax;
      if (allow_partial != NULL) *allow_partial = true;
      switch (xt_type) {
        case kExtractTypeNone:    return true;
        case kExtractType8bToLSB: return ((off >= off0) && (off <= siz-1)) || ((off >= off2) && (off <= max-1));
        case kExtractType8bToMSB: return ((off >= off0) && (off <= siz-1)) || ((off >= off2) && (off <= max-1));
        case kExtractType16b:     return ((off >= off0) && (off <= siz-2)) || ((off >= off2) && (off <= max-2));
        default: RMT_ASSERT(0);
      }
    }
    inline int extract8_first_non_checksum(int n_xt, int ck8, int ck16, int ck32) {
      RMT_ASSERT(0); return -1;
    }
           int extract16_first_non_checksum(int n_xt, int ck8, int ck16, int ck32);
    inline int extract32_first_non_checksum(int n_xt, int ck8, int ck16, int ck32) {
      RMT_ASSERT(0); return -1;
    }
    inline bool extract8_available(int i_xt, int n_xt, int ck8, int ck16, int ck32) {
      return false;
    }
           bool extract16_available(int i_xt, int n_xt, int ck8, int ck16, int ck32);
    inline bool extract32_available(int i_xt, int n_xt, int ck8, int ck16, int ck32) {
      return false;
    }
    inline uint8_t  extract8_type(int pi,int i)                   { RMT_ASSERT(0); }
    inline bool     extract8_src_imm_val(int pi,int i)            { RMT_ASSERT(0); }
    inline bool     extract8_rot_imm_val(int pi,int i)            { RMT_ASSERT(0); }
    inline bool     extract8_add_off(int pi, int i)               { RMT_ASSERT(0); }
    inline uint8_t  extract8_src(int pi, int i, uint8_t mask)     { RMT_ASSERT(0); }
    inline uint8_t  extract8_src_msb(int pi, int i, int bitpos)   { RMT_ASSERT(0); }
    inline uint16_t extract8_dst_phv(int pi, int i)               { RMT_ASSERT(0); }
           uint8_t  extract16_type(int pi,int i);
           bool     extract16_src_imm_val(int pi,int i);
           bool     extract16_rot_imm_val(int pi,int i);
           bool     extract16_add_off(int pi, int i);
           uint8_t  extract16_src(int pi, int i, uint8_t mask);
           uint8_t  extract16_src_msb(int pi, int i, int bitpos);
           uint16_t extract16_dst_phv_raw(int pi, int i);
    inline uint16_t extract16_dst_phv_by_off16(int pi, int i)     { return extract16_dst_phv_raw(pi, i); }
           uint16_t extract16_dst_phv_by_phv(int pi, int i);
    inline uint16_t extract16_dst_phv(int pi, int i)              { return extract16_dst_phv_by_off16(pi,i); }
    inline uint8_t  extract32_type(int pi,int i)                  { RMT_ASSERT(0); }
    inline bool     extract32_src_imm_val(int pi,int i)           { RMT_ASSERT(0); }
    inline bool     extract32_rot_imm_val(int pi,int i)           { RMT_ASSERT(0); }
    inline bool     extract32_add_off(int pi, int i)              { RMT_ASSERT(0); }
    inline uint8_t  extract32_src(int pi, int i, uint8_t mask)    { RMT_ASSERT(0); }
    inline uint8_t  extract32_src_msb(int pi, int i, int bitpos)  { RMT_ASSERT(0); }
    inline uint16_t extract32_dst_phv(int pi, int i)              { RMT_ASSERT(0); }
    inline uint8_t  extract8_src_const(int pi, int i, uint8_t m, int rot)  { RMT_ASSERT(0); }
    inline uint16_t extract16_src_const(int pi, int i, uint8_t m, int rot) { RMT_ASSERT(0); }
    inline uint32_t extract32_src_const(int pi, int i, uint8_t m, int rot) { RMT_ASSERT(0); }
    inline uint8_t  extract8_inbuf_shift(int pi, int i)            { RMT_ASSERT(0); }
           uint8_t  extract16_inbuf_shift(int pi, int i);
    inline uint8_t  extract32_inbuf_shift(int pi, int i)           { RMT_ASSERT(0); }
           uint32_t extract_immediate_consts(int pi, int i, int rot, uint32_t dflt);

    inline void set_extract8_type(int pi, int i, uint8_t v)               { RMT_ASSERT(0); }
    inline void set_extract8_src_imm_val(int pi, int i, bool tf)          { RMT_ASSERT(0); }
    inline void set_extract8_rot_imm_val(int pi, int i, bool tf)          { RMT_ASSERT(0); }
    inline void set_extract8_add_off(int pi, int i, bool tf)              { RMT_ASSERT(0); }
    inline void set_extract8_src(int pi, int i, uint8_t v)                { RMT_ASSERT(0); }
    inline void set_extract8_dst_phv(int pi, int i, uint16_t v)           { RMT_ASSERT(0); }
           void set_extract16_type(int pi, int i, uint8_t extract_type);  // jbay
           void set_extract16_type_cnt(int pi, uint8_t cnt_16, uint8_t cnt_8_hi, uint8_t cnt_8_lo);  // WIP
           void update_extract16_type_cnt(int pi, uint8_t extract_type, uint8_t val);  // WIP
           void set_extract16_src_imm_val(int pi, int i, bool tf);
           void set_extract16_rot_imm_val(int pi, int i, bool tf);
           void set_extract16_add_off(int pi, int i, bool tf);
           void set_extract16_src(int pi, int i, uint8_t v);
           void set_extract16_dst_phv_raw(int pi, int i, uint16_t v);
    inline void set_extract16_dst_phv_by_off16(int pi, int i, uint16_t v) { set_extract16_dst_phv_raw(pi, i, v); }
           uint8_t set_extract16_dst_phv_by_phv(int pi, int i, uint16_t v, int which16);
           uint8_t set_extract16_dst_phv_by_phv(int pi, int i, uint16_t v)   { return set_extract16_dst_phv_by_phv(pi, i, v, 0); }
    inline void set_extract16_dst_phv(int pi, int i, uint16_t v)          { set_extract16_dst_phv_by_phv(pi, i, v); }
    inline void set_extract32_type(int pi, int i, uint8_t v)              { RMT_ASSERT(0); }
    inline void set_extract32_src_imm_val(int pi, int i, bool tf)         { RMT_ASSERT(0); }
    inline void set_extract32_rot_imm_val(int pi, int i, bool tf)         { RMT_ASSERT(0); }
    inline void set_extract32_add_off(int pi, int i, bool tf)             { RMT_ASSERT(0); }
    inline void set_extract32_src(int pi, int i, uint8_t v)               { RMT_ASSERT(0); }
    inline void set_extract32_dst_phv(int pi, int i, uint16_t v)          { RMT_ASSERT(0); }

    uint16_t map_dst_phv(uint16_t phv_in, int from_format);
    bool extract_ok_phv(int phv_word);
    bool extract_ok_tphv(int phv_word);
    int  get_phv_to_write(int phv_word);
    int  get_tphv_to_write(int phv_word);

    // Function to apply other early (pre-extraction) checks
    uint32_t other_early_error_checks(int match_index);

    // Function to apply further extraction constraint checks
    void check_further_extract_constraints(uint32_t check_flags,
                                           int index, int which_extract,
                                           int phv_word, Phv *phv, int extract_sz,
                                           bool partial_hdr_err);
    // Function to apply checksum constraint checks
    bool check_checksum_constraints(int match_index,
                                    int &wrote_8b, int &wrote_16b, int &wrote_32b);

    // Function to do JBay-specific 16b extract
    uint32_t perform_extract_16b(int index, int which_extract, const int word_tab[],
                                 int phv_off16, Phv *phv, uint16_t val,
                                 uint8_t extract_type,
                                 ParseContext *context);

    // These offset/checksum/pri_upd funcs now need to be per-chip because of WIP ActionRAM reorganisation
    bool    offset_reset(int pi);
    uint8_t offset_incr_val(int pi);
    bool    checksum_enable(int pi, int i);
    uint8_t checksum_ram_addr(int pi, int i);
    uint8_t pri_upd_type(int pi);
    uint8_t pri_upd_src(int pi);
    uint8_t pri_upd_en_shr(int pi);
    uint8_t pri_upd_val_mask(int pi);

    void set_offset_reset(int pi, bool tf);
    void set_offset_incr_val(int pi, uint8_t v);
    void set_checksum_enable(int pi, int i, bool tf);
    void set_checksum_ram_addr(int pi, int i, uint8_t v);
    void set_pri_upd_type(int pi, uint8_t v);
    void set_pri_upd_src(int pi, uint8_t v);
    void set_pri_upd_en_shr(int pi, uint8_t v);
    void set_pri_upd_val_mask(int pi, uint8_t v);

    // Funcs to allow access to Version Update fields in ActionRAM
    uint8_t ver_upd_type(int pi);
    uint8_t ver_upd_src(int pi);
    uint8_t ver_upd_en_shr(int pi);
    uint8_t ver_upd_val_mask(int pi);
    void set_ver_upd_type(int pi, uint8_t v);
    void set_ver_upd_src(int pi, uint8_t v);
    void set_ver_upd_en_shr(int pi, uint8_t v);
    void set_ver_upd_val_mask(int pi, uint8_t v);
    // Function to handle different capabilties wrt Version Update
    uint8_t update_version(int index, uint8_t curr_ver);
    // Function to setup Version Update regs
    void set_action_ver(int index,
                        uint8_t ver_upd_type, uint8_t ver_upd_src,
                        uint8_t ver_upd_en_shr, uint8_t ver_upd_val_mask);


    // Funcs to allow access to Priority Mapping register
    inline uint8_t  pri_map(int ci, int pri)            { return prs_main_pri_map_.pri(ci,pri) & 0x3; }
    inline void set_pri_map(int ci, int pri, uint8_t v) { prs_main_pri_map_.pri(ci,pri,v); }
    // Function to handle different capabilities wrt Priority Mapping
    uint8_t map_priority(int chan, uint8_t curr_pri);
    // Function to setup identity Priority Mapping
    void set_identity_priority_map();


    // Funcs to allow access to CLOT fields in ActionRAM
    // clot_tag_offset_add added in XXX
    // clot_len_mask/clot_len_add added in XXX (NB. clot_len_add only for Clot0)
    static constexpr int clot_tag_width = 6;
    static constexpr int clot_tag_mask = (1<<clot_tag_width)-1;
    static constexpr int clot_shift_mask = 0xF;
    static constexpr int clot_offset_mask = 0x1F;
    static constexpr int clot_length_width = 6;
    static constexpr int clot_length_max = ClotEntry::kMaxLen; // 64 not 63
    static constexpr int clot_length_mask = (1<<clot_length_width)-1;

    uint8_t clot_type(int pi, int ci);
    uint8_t clot_len_src(int pi, int ci);
    uint8_t clot_en_len_shr(int pi, int ci);
    uint8_t clot_len_mask(int pi, int ci);
    uint8_t clot_len_add(int pi, int ci);
    uint8_t clot_offset(int pi, int ci);
    uint8_t clot_tag(int pi, int ci);
    bool    clot_tag_offset_add(int pi, int ci);
    uint8_t clot_has_csum(int pi, int ci);
    void set_clot_type(int pi, int ci, uint8_t v);
    void set_clot_len_src(int pi, int ci, uint8_t v);
    void set_clot_en_len_shr(int pi, int ci, uint8_t v);
    void set_clot_len_mask(int pi, int ci, uint8_t v);
    void set_clot_len_add(int pi, int ci, int v);
    void set_clot_offset(int pi, int ci, uint8_t v);
    void set_clot_tag(int pi, int ci, uint8_t v);
    void set_clot_tag_offset_add(int pi, int ci, bool tf);
    void set_clot_has_csum(int pi, int ci, uint8_t v);
    // Function to setup CLOT regs
    void set_action_clot(int index, int clot_index,
                         uint8_t clot_type,
                         uint8_t clot_len_src, uint8_t clot_en_len_shr,
                         uint8_t clot_len_mask, uint8_t clot_len_add,
                         uint8_t clot_offset,
                         uint8_t clot_tag, bool clot_tag_offset_add,
                         uint8_t clot_has_csum);
    // CLOT helper funcs to extract checksums from ChecksumEngines etc
    bool clot_validate_checksum_engines(int index);
    bool clot_find_checksum(int index, uint8_t tag, uint16_t *csum);
    void clot_print(Clot *clot, int i, const char *argstr=NULL);
    void clot_print(Packet *packet, int i);
    void clot_print(Packet *packet);
    bool clot_write(int index, Packet *packet, int lot_index, int c_index,
                    uint8_t c_tag, int c_len, int c_off, uint16_t c_cksum);
    void clot_reset();
    void clot_discard(int index, const char *why);
    void clot_pend(int index, int c_index, uint8_t c_tag, int c_len, int c_off);
    bool clot_emit(int index, Packet *packet);
    // And to update CLOT within Packet
    int  clot_handle(int index, Packet *packet);


    // Funcs to handle immediate constants
    uint16_t val_const(int pi, int i);
    bool     val_const_rot(int pi, int i);
    bool     val_const_32b_bond(int pi);
    void set_val_const(int pi, int i, uint16_t v);
    void set_val_const_rot(int pi, int i, bool tf);
    void set_val_const_32b_bond(int pi, bool tf);
    //inline bool     extract_is_8b(int pi, int i) { return ((act_ram_->extract_is_8b(pi,i) & 1) == 1); }
    //inline void set_extract_is_8b(int pi, int i, bool tf) { act_ram_->extract_is_8b(pi,i,tf?1:0); }

    // chip-specific funcs to handle hdr_len updates (jbay or later)
    virtual void update_hdr_len(Packet *p, int index, uint32_t err_flags, uint8_t shift);
    bool hdr_len_inc(int pi);
    void set_hdr_len_inc(int pi, bool tf);
    uint8_t hdr_len_inc_final_amt(int pi);
    void set_hdr_len_inc_final_amt(int pi, uint8_t val);
    bool hdr_len_inc_stop(int pi);
    void set_hdr_len_inc_stop(int pi, bool tf);

    // Funcs to handle disable_partial_hdr_err - added in XXX
    bool disable_partial_hdr_err(int pi);
    void set_disable_partial_hdr_err(int pi, bool tf);

    bool partial_hdr_err_proc(int pi);
    void set_partial_hdr_err_proc(int pi, bool tf);


    // Shim function to allow consistent set_action across ALL chips
    void set_action(int index,
                    bool _offset_reset, uint8_t _offset_incr_val,
                    bool _checksum_enable[], uint8_t _checksum_ram_addr[],
                    bool _extract8_src_imm_val[], bool _extract8_add_off[],
                    uint8_t _extract8_src[],  uint16_t _extract8_dst_phv[],
                    bool _extract16_src_imm_val[], bool _extract16_add_off[],
                    uint8_t _extract16_src[], uint16_t _extract16_dst_phv[],
                    bool _extract32_src_imm_val[], bool _extract32_add_off[],
                    uint8_t _extract32_src[], uint16_t _extract32_dst_phv[],
                    int _dst_phv_format=model_core::ChipType::kTofino,
                    int disabled=4);


    // Functions to handle different capabilities wrt Counter Masking
    inline uint8_t  pcnt_mask_width(int pi)             {
      for (uint8_t width = 0; width < 8; width++) { if (pcnt_calc_mask(width) == pcnt_mask(pi)) return width; }
      RMT_ASSERT(0);
    }
    inline uint8_t  pcnt_mask(int pi)                   { return ctr_init_ram_->mask_8(pi); }
    inline void set_pcnt_mask_width(int pi, uint8_t v)  { set_pcnt_mask(pi, pcnt_calc_mask(v)); }
    inline void set_pcnt_mask(int pi, uint8_t v)        { ctr_init_ram_->mask_8(pi,v); }

    // Implementation of these varies across JBay/WIP
    uint8_t         pcnt_add_to_stack(int pi);
    void       set_pcnt_add_to_stack(int pi, uint8_t v);


    // Functions to handle NORMAL PHV owner/no_multi_wr fields
    // (Note in JBay case the value w is an phv_off16)
    inline uint8_t prs_reg_phv_owner_norm_get(register_classes::PrsrRegMainRspecPhvOwnerMutable *r, int w) {
      return (off16_ok(w)) ?r->owner(w) & 1 :0;
    }
    inline void prs_reg_phv_owner_norm_set(register_classes::PrsrRegMainRspecPhvOwnerMutable *r, int w, uint8_t v) {
      if (off16_ok(w)) r->owner(w, v & 1);
    }
    inline uint8_t prs_reg_no_multi_wr_norm_get(register_classes::PrsrRegMainRspecNoMultiWrMutable *r, int w) {
      return (off16_ok(w)) ?r->nmw(w) & 1 :0;
    }
    inline void prs_reg_no_multi_wr_norm_set(register_classes::PrsrRegMainRspecNoMultiWrMutable *r, int w, uint8_t v) {
      if (off16_ok(w)) r->nmw(w, v & 1);
    }
    // Functions to NOT handle TAGALONG PHV owner/no_multi_wr fields - assert on get/set
    inline uint8_t prs_reg_phv_owner_taga_get(register_classes::PrsrRegMainRspecPhvOwnerMutable *r, int w) {
      RMT_ASSERT(0);
    }
    inline void prs_reg_phv_owner_taga_set(register_classes::PrsrRegMainRspecPhvOwnerMutable *r, int w, uint8_t v) {
      RMT_ASSERT(0);
    }
    inline uint8_t prs_reg_no_multi_wr_taga_get(register_classes::PrsrRegMainRspecNoMultiWrMutable *r, int w) {
      RMT_ASSERT(0);
    }
    inline void prs_reg_no_multi_wr_taga_set(register_classes::PrsrRegMainRspecNoMultiWrMutable *r, int w, uint8_t v) {
      RMT_ASSERT(0);
    }


    // Functions to handle whether PHV writes are allowed
    inline bool phv_can_write(Phv *phv, int phv_off16) {
      return ((off16_ok(phv_off16)) && (phv_owner(phv_off16)) &&
              (phv_multi_write(phv_off16) || !written_.bit_set(phv_off16)));
    }
    inline bool phv_can_write_16b(Phv *phv, int phv_off16) {
      return phv_can_write(phv, phv_off16);
    }
    inline bool phv_owner_16b(Phv *phv, int phv_off16) {
      return phv_owner(phv_off16);
    }
    inline bool phv_valid_16b(Phv *phv, int phv_off16) {
      return off16_ok(phv_off16);
    }


    // Fill other gress PHV contianers with junk
    void phv_fill_other_gress(Phv* phv);

    // Access checksum engines
    inline ChecksumEngine *get_checksum_engine(int ci) { return &checksum_engine_[ci]; }


    // Access tcam row array - JBay has a single array holding both word0 and word1
    inline memory_classes::PrsrMlTcamRowArrayMutable *get_prs_word0s() { RMT_ASSERT(0); }
    inline memory_classes::PrsrMlTcamRowArrayMutable *get_prs_word1s() { RMT_ASSERT(0); }
    inline memory_classes::PrsrMlTcamRowArrayMutable *get_prs_word0s_word1s() {
      return &prs_tcam_word0s_word1s_;
    }
    // Code to extract TCAM vals from row array into BitVector to program s/w TCAM
    BitVector<44> prs_tcam_get_entry(memory_classes::PrsrMlTcamRowArrayMutable &prs,
                                     uint32_t i, uint32_t which_word);
    // Called on TCAM row array change - called from memory_change_callback()
    void prs_tcam_change(uint32_t n);



    // Maintain a history buffer of recent phv_off16s written to
    // XXX/XXX - this was eliminated in XXX
#ifdef CLR_ON_WR_HISTORY
    static constexpr uint8_t kWrHistorySize = 9;

    inline void history_write(int phv_off16) {
      RMT_ASSERT((phv_off16 >= 0) && (phv_off16 <= 0xFF));
      wr_history_[wr_history_ptr_ % kWrHistorySize] = static_cast<uint8_t>(phv_off16 & 0xFF);
      wr_history_ptr_++; // Increment ptr, but wrap when we get to twice size (so at 18 wrap to 9)
      if (wr_history_ptr_ == kWrHistorySize + kWrHistorySize) wr_history_ptr_ = kWrHistorySize;
    }
    inline bool history_contains(int phv_off16) {
      RMT_ASSERT((phv_off16 >= 0) && (phv_off16 <= 0xFF));
      uint8_t lim = (wr_history_ptr_ < kWrHistorySize) ?wr_history_ptr_ :kWrHistorySize;
      for (int i = 0; i < lim; i++) {
        if (wr_history_[i] == static_cast<uint8_t>(phv_off16 & 0xFF)) return true;
      }
      return false;
    }
#else
    static constexpr uint8_t kWrHistorySize = 1; // Just for compilation purposes

    inline void history_write(int phv_off16)    { return; }
    inline bool history_contains(int phv_off16) { return false; }
#endif
    inline void history_reset() { wr_history_ptr_ = 0; }


    // Keep track of other Parsers that subscribe to us.
    // Used as a way for other Parsers to get copies of our memory writes
    static constexpr int kMaxSubscribers = 4;
    static constexpr int kMaxMemorySize = kStates;

    void subscribe(Parser *other_parser);
    void unsubscribe(Parser *other_parser);
    int  notify_subscribers(uint8_t src_mem_type, uint8_t src_mem_inst, uint32_t src_mem_index);
    void notify_memory_change_tcam(Parser *src_parser, uint8_t src_mem_inst, uint32_t i);
    void notify_memory_change_early_action(Parser *src_parser, uint8_t src_mem_inst, uint32_t i);
    void notify_memory_change_action(Parser *src_parser, uint8_t src_mem_inst, uint32_t i);
    void notify_memory_change_counter(Parser *src_parser, uint8_t src_mem_inst, uint32_t i);
    void notify_memory_change_checksum(Parser *src_parser, uint8_t src_mem_inst, uint32_t i);
    void notify_memory_change_sw_tcam(Parser *src_parser, uint8_t src_mem_inst, uint32_t i);
    bool notify_memory_change(Parser *src_parser,
                              uint8_t src_mem_type, uint8_t src_mem_inst, uint32_t src_mem_index);


 private:
    bool                                                             reset_running_;
    bool                                                             clr_on_wr_changed_;
    bool                                                             owner_changed_;
    register_classes::PrsrRegMainRspecErrPhvCfgArrayMutable          prs_main_err_phv_cfg_;
    register_classes::PrsrRegMainRspecPortRateCfgMutable             prs_main_port_rate_cfg_;
    register_classes::PrsrRegMainRspecPortChnlEnMutable              prs_main_port_chnl_en_;
    register_classes::PrsrRegMainRspecStartLookupOffsetsArrayMutable prs_main_start_lookup_offsets_;
    register_classes::PrsrRegMainRspecPriMapArrayMutable             prs_main_pri_map_;
    //register_classes::PrsrRegMainRspecDstContErrCntMutable         prs_main_dst_cont_err_cnt_;
    //register_classes::PrsrRegMainRspecDstContErrlogMutable         prs_main_dst_cont_errlog_;
    register_classes::PrsrRegMainRspecPktRxCntArrayMutable           prs_main_pkt_rx_cnt_;
    register_classes::PrsrRegMainRspecPktTxCntArrayMutable           prs_main_pkt_tx_cnt_;
    //register_classes::PrsrRegMainRspecNarrowToWideErrCntMutable    prs_main_narrow_to_wide_err_cnt_;
    register_classes::PrsrRegMainRspecIntrStatMutable                prs_main_intr_status_;
    // These registers are instantiated but are NOT used - however allows read/write
    register_classes::PrsrRegMainRspecIntrEn0Mutable                 prs_main_intr_enable0_;
    register_classes::PrsrRegMainRspecIntrEn1Mutable                 prs_main_intr_enable1_;
    register_classes::PrsrRegMainRspecIntrFreezeEnableMutable        prs_main_intr_freeze_enable_;
    register_classes::PrsrRegMainRspecIntrInjMutable                 prs_main_intr_inject_;
    register_classes::PrsrRegMainRspecAramMbeErrLogMutable           prs_main_aram_mbe_errlog_;
    register_classes::PrsrRegMainRspecAramSbeErrLogMutable           prs_main_aram_sbe_errlog_;
    register_classes::PrsrRegMainRspecCtrRangeErrLogMutable          prs_main_ctr_range_errlog_;
    register_classes::PrsrRegMainRspecIbufOflowErrLogMutable         prs_main_ibuf_oflow_errlog_;
    register_classes::PrsrRegMainRspecIbufUflowErrLogMutable         prs_main_ibuf_uflow_errlog_;
    register_classes::PrsrRegMainRspecMultiWrErrLogMutable           prs_main_multi_wr_errlog_;
    register_classes::PrsrRegMainRspecNoTcamMatchErrLogMutable       prs_main_no_tcam_match_errlog_;
    register_classes::PrsrRegMainRspecOpFifoOflowErrLogMutable       prs_main_op_fifo_oflow_errlog_;
    register_classes::PrsrRegMainRspecOpFifoUflowErrLogMutable       prs_main_op_fifo_uflow_errlog_;
    register_classes::PrsrRegMainRspecPartialHdrErrLogMutable        prs_main_partial_hdr_errlog_;
    register_classes::PrsrRegMainRspecPhvOwnerErrLogMutable          prs_main_phv_owner_errlog_;
    register_classes::PrsrRegMainRspecSrcExtErrLogMutable            prs_main_src_ext_errlog_;
    register_classes::PrsrRegMainRspecTcamParErrLogMutable           prs_main_tcam_par_errlog_;
    //register_classes::PrsrRegMainRspecNarrowToWideErrLogMutable    prs_main_narrow_to_wide_errlog_;
    register_classes::PrsrRegMainRspecMemCtrlMutable                 prs_main_mem_ctrl_;
    register_classes::PrsrRegMainRspecPhvClrOnWrMutable              prs_main_phv_clr_on_wr_;

    std::array<ChecksumEngine,kChecksumEngines>                      checksum_engine_;
    memory_classes::PrsrMlTcamRowArrayMutable                        prs_tcam_word0s_word1s_;
    memory_classes::PrsrMlEaRowArrayMutable                         *ea_ram_;
    memory_classes::PrsrPoActionRowArrayMutable                     *act_ram_;
    memory_classes::PrsrMlCtrInitRamMArrayMutable                   *ctr_init_ram_;
    ParserCounterStack< kCounterStackSize >                          ctr_stack_;
    BitVector<256>                                                   written_;
    std::array< uint8_t, kWrHistorySize >                            wr_history_;
    uint8_t                                                          wr_history_ptr_;
    uint8_t                                                          memory_write_warn_;
    uint8_t                                                          memory_repair_warn_;
    uint8_t                                                          memory_ctrl_prev_;
    std::array< uint8_t, kMaxMemorySize >                            memory_change_ignore_;
    std::array< Parser*, kMaxSubscribers >                           subscribers_;
    Parser                                                          *subscribed_to_;
    int                                                              pending_index_;
    int                                                              pending_c_index_;
    int                                                              pending_c_len_;
    int                                                              pending_c_off_;
    uint8_t                                                          pending_c_tag_;
    bool                                                             pending_clot_;
    bool                                                             extracted_phv_;
    uint8_t                                                          pending_shift_;
  };
}

#endif // _JBAY_SHARED_PARSER_
