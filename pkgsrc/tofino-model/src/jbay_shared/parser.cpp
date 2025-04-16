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

// PARSER - Jbay specific code
#include <port.h>
#include <parser.h>
#include <clot.h>
#include <parser-block.h>
#include <rmt-object-manager.h>
#include <rmt-packet-coordinator.h>

namespace MODEL_CHIP_NAMESPACE {

Parser::Parser(RmtObjectManager *om, int pipeIndex, int prsIndex, int ioIndex,
               const ParserConfig &config)
    : ParserShared(om, pipeIndex, prsIndex, ioIndex, config),
      reset_running_(false), clr_on_wr_changed_(true), owner_changed_(true),
      prs_main_err_phv_cfg_(prsr_reg_adapter(prs_main_err_phv_cfg_, chip_index(), pipeIndex, ioIndex, prsIndex)),
      prs_main_port_rate_cfg_(prsr_reg_adapter(prs_main_port_rate_cfg_, chip_index(), pipeIndex, ioIndex, prsIndex)),
      prs_main_port_chnl_en_(prsr_reg_adapter(prs_main_port_chnl_en_, chip_index(), pipeIndex, ioIndex, prsIndex)),
      prs_main_start_lookup_offsets_(prsr_reg_adapter(prs_main_start_lookup_offsets_, chip_index(), pipeIndex, ioIndex, prsIndex)),
      prs_main_pri_map_(prsr_reg_adapter(prs_main_pri_map_, chip_index(), pipeIndex, ioIndex, prsIndex)),
      //prs_main_dst_cont_err_cnt_(prsr_reg_adapter(prs_main_dst_cont_err_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
      //prs_main_dst_cont_errlog_(prsr_reg_adapter(prs_main_dst_cont_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
      //                          [this](){this->not_implemented("prs_main_dst_cont_errlog_");})),
      prs_main_pkt_rx_cnt_(prsr_reg_adapter(prs_main_pkt_rx_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
      prs_main_pkt_tx_cnt_(prsr_reg_adapter(prs_main_pkt_tx_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
      //prs_main_narrow_to_wide_err_cnt_(prsr_reg_adapter(prs_main_narrow_to_wide_err_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
      prs_main_intr_status_(prsr_reg_adapter(prs_main_intr_status_, chip_index(), pipeIndex, ioIndex, prsIndex)),
      // These registers are instantiated but are NOT used - however allows read/write
      prs_main_intr_enable0_(prsr_reg_adapter(prs_main_intr_enable0_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                              [this](){this->not_implemented("prs_main_int_en0_");})),
      prs_main_intr_enable1_(prsr_reg_adapter(prs_main_intr_enable1_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                              [this](){this->not_implemented("prs_main_int_en1_");})),
      prs_main_intr_freeze_enable_(prsr_reg_adapter(prs_main_intr_freeze_enable_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                    [this](){this->not_implemented("prs_main_int_freeze_enable_");})),
      prs_main_intr_inject_(prsr_reg_adapter(prs_main_intr_inject_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                             [this](){this->not_implemented("prs_main_int_inj_");})),
      prs_main_aram_mbe_errlog_(prsr_reg_adapter(prs_main_aram_mbe_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                 [this](){this->not_implemented("prs_main_aram_mbe_err_log_");})),
      prs_main_aram_sbe_errlog_(prsr_reg_adapter(prs_main_aram_sbe_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                 [this](){this->not_implemented("prs_main_aram_sbe_err_log_");})),
      prs_main_ctr_range_errlog_(prsr_reg_adapter(prs_main_ctr_range_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                  [this](){this->not_implemented("prs_main_ctr_range_err_log_");})),
      prs_main_ibuf_oflow_errlog_(prsr_reg_adapter(prs_main_ibuf_oflow_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                   [this](){this->not_implemented("prs_main_ibuf_oflow_err_log_");})),
      prs_main_ibuf_uflow_errlog_(prsr_reg_adapter(prs_main_ibuf_uflow_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                   [this](){this->not_implemented("prs_main_ibuf_uflow_err_log_");})),
      prs_main_multi_wr_errlog_(prsr_reg_adapter(prs_main_multi_wr_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                 [this](){this->not_implemented("prs_main_multi_wr_err_log_");})),
      prs_main_no_tcam_match_errlog_(prsr_reg_adapter(prs_main_no_tcam_match_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                      [this](){this->not_implemented("prs_main_no_tcam_match_err_log_");})),
      prs_main_op_fifo_oflow_errlog_(prsr_reg_adapter(prs_main_op_fifo_oflow_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                      [this](){this->not_implemented("prs_main_op_fifo_oflow_err_log_");})),
      prs_main_op_fifo_uflow_errlog_(prsr_reg_adapter(prs_main_op_fifo_uflow_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                      [this](){this->not_implemented("prs_main_op_fifo_uflow_err_log_");})),
      prs_main_partial_hdr_errlog_(prsr_reg_adapter(prs_main_partial_hdr_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                    [this](){this->not_implemented("prs_main_partial_hdr_err_log_");})),
      prs_main_phv_owner_errlog_(prsr_reg_adapter(prs_main_phv_owner_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                  [this](){this->not_implemented("prs_main_phv_owner_err_log_");})),
      prs_main_src_ext_errlog_(prsr_reg_adapter(prs_main_src_ext_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                [this](){this->not_implemented("prs_main_src_ext_err_log_");})),
      prs_main_tcam_par_errlog_(prsr_reg_adapter(prs_main_tcam_par_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                 [this](){this->not_implemented("prs_main_tcam_par_err_log_");})),
      //prs_main_narrow_to_wide_errlog_(prsr_reg_adapter(prs_main_narrow_to_wide_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
      //                                [this](){this->not_implemented("prs_main_narrow_to_wide_err_log_");})),
      prs_main_mem_ctrl_(prsr_reg_adapter(prs_main_mem_ctrl_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                 [this](){this->memory_ctrl_callback();})),
      prs_main_phv_clr_on_wr_(prsr_reg_adapter(prs_main_phv_clr_on_wr_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                               [this](){this->phv_clr_on_wr_change_callback();})),
      checksum_engine_ { { { om,pipeIndex,ioIndex,prsIndex,0,0,this },
                           { om,pipeIndex,ioIndex,prsIndex,1,1,this },
                           { om,pipeIndex,ioIndex,prsIndex,2,2,this },
                           { om,pipeIndex,ioIndex,prsIndex,3,3,this },
                           { om,pipeIndex,ioIndex,prsIndex,4,4,this } } },
      prs_tcam_word0s_word1s_(prsr_mem_adapter(prs_tcam_word0s_word1s_,chip_index(), pipeIndex, ioIndex, prsIndex,
                                         [this](uint32_t i){this->memory_change_callback(ParserMemoryType::kTcam,0,i);})),
      ea_ram_(get_prs_ea_ram()), act_ram_(get_prs_act_ram()), ctr_init_ram_(get_prs_ctr_init_ram()),
      ctr_stack_(), written_(UINT64_C(0)), wr_history_ptr_(0),
      extracted_phv_(false), pending_shift_(0) {

    static_assert( (kLoadBytes==4),
                 "JBay Parser logic can only load 4 bytes from buffer");
    reset_this();
}
Parser::~Parser() {
}
void Parser::reset_this() {
  reset_running_ = true;
  prs_main_err_phv_cfg_.reset();
  prs_main_port_rate_cfg_.reset();
  prs_main_port_chnl_en_.reset();
  prs_main_start_lookup_offsets_.reset();
  prs_main_pri_map_.reset();
  //prs_main_dst_cont_err_cnt_.reset();
  prs_main_pkt_rx_cnt_.reset();
  prs_main_pkt_tx_cnt_.reset();
  //prs_main_narrow_to_wide_err_cnt_.reset();
  prs_main_intr_status_.reset();
  prs_main_intr_enable0_.reset();
  prs_main_intr_enable1_.reset();
  prs_main_intr_freeze_enable_.reset();
  prs_main_intr_inject_.reset();
  // These registers are instantiated but are NOT used - however allows read/write
  //prs_main_dst_cont_errlog_.reset();
  prs_main_aram_mbe_errlog_.reset();
  prs_main_aram_sbe_errlog_.reset();
  prs_main_ctr_range_errlog_.reset();
  prs_main_ibuf_oflow_errlog_.reset();
  prs_main_ibuf_uflow_errlog_.reset();
  prs_main_multi_wr_errlog_.reset();
  prs_main_no_tcam_match_errlog_.reset();
  prs_main_op_fifo_oflow_errlog_.reset();
  prs_main_op_fifo_uflow_errlog_.reset();
  prs_main_partial_hdr_errlog_.reset();
  prs_main_phv_owner_errlog_.reset();
  prs_main_src_ext_errlog_.reset();
  prs_main_tcam_par_errlog_.reset();
  //prs_main_narrow_to_wide_errlog_.reset();
  prs_main_mem_ctrl_.reset();
  prs_main_phv_clr_on_wr_.reset();

  memory_ctrl_prev_ = static_cast<uint8_t>(RmtDefs::get_parser_element(parser_index()));
  // Used to have to initialize mem_ctrl explicitly as reset value was variable
  // NOT needed once reset val = fixed value 0 (==> all parsers 0,1,2,3 listen to memory 0)
  //prs_main_mem_ctrl_.wr_addr(memory_ctrl_prev_);

  prs_tcam_word0s_word1s_.reset();

  // Reset subscriber array
  for (int i = 0; i < kMaxSubscribers; i++) subscribers_[i] = NULL;
  subscribed_to_ = this;
  for (int i = 0; i < kMaxMemorySize; i++) memory_change_ignore_[i] = 0;
  memory_write_warn_ = memory_repair_warn_ = 0;

  // Reset pending CLOT
  clot_reset();

  reset_running_ = false;

  // Fake a call to memory_ctrl_callback in case we need to subscribe to another Parser
  memory_ctrl_callback();
}
void Parser::reset() {
  // Call superclass reset and then self reset
  ParserShared::reset();
  reset_this();
}
void Parser::counter_reset() {
  // Call superclass counter_reset and then reset ctr_stack_
  ParserShared::counter_reset();
  ctr_stack_.reset();
}
void Parser::not_implemented(const char *clazz) {
  if (reset_running_) return;
  ParserShared::not_implemented(clazz);
}
void Parser::memory_change_callback(uint8_t mem_type, uint8_t mem_inst, uint32_t mem_index) {
  // If we're still running CTOR return
  if (reset_running_) return;
    // If we're ignoring callbacks for this mem_index/mem_type return
  if ((memory_change_ignore_[mem_index] & mem_type) != 0) return;

  Parser *sub = subscribed_to_;

  // Tell all Parsers snooping us that memory has changed.
  // This is their chance to grab the data that was just written
  int notifies = notify_subscribers(mem_type, mem_inst, mem_index);

  if (sub != this) {
    // If we are snooping another Parser and receive this callback
    // (indicating a direct write to our own memory) then we also need
    // to 'repair' the memory by reading the data from the Parser we're
    // snooping.

    // Fake a change to get memory 'repaired'
    // This fetches the overwritten line from the Parser we're subscribed_to
    notify_memory_change(sub, mem_type, mem_inst, mem_index);

    if (notifies == 0) {
      // WARN about direct write to this Parser
      // We're subscribed to another Parser and no one is subscribed to us
      // so NO writes should be targeted at this Parser - the RTL handles
      // this automatically as it won't even decode local addresses if the
      // mem_ctrl CSR is pointing elsewhere - in S/W however this situation
      // could arise if the driver directly writes to this Parser.
      if ((memory_write_warn_ & mem_type) == 0) {
        memory_write_warn_ |= mem_type; // Only warn first time
        RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugParserMemory),
                "Parser::memory_change_callback(Type=%d,Inst=%d,Index=%d) "
                "Unexpected write to Parser memory - this Parser(%d) is "
                "snooping writes to Parser(%d) so should NOT be directly "
                "programmed\n", mem_type, mem_inst, mem_index,
                parser_index(), sub->parser_index());
      }
    }
    // WARN that memory will be repaired
    if ((memory_repair_warn_ & mem_type) == 0) {
      memory_repair_warn_ |= mem_type; // Only warn first time
        RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugParserMemory),
                "Parser::memory_change_callback(Type=%d,Inst=%d,Index=%d) "
                "Direct write to Parser memory - this Parser(%d) must "
                "now repair its memory by reading from Parser(%d)\n",
                mem_type, mem_inst, mem_index,
                parser_index(), sub->parser_index());
    }

  } else {
    // We're not snooping any other Parser so it's correct for
    // writes to occur to this Parser.
    // We just need to update S/W TCAM if it's a TCAM write

    if (mem_type == ParserMemoryType::kTcam) {
      // Handle change to local TCAM
      prs_tcam_change(static_cast<uint32_t>(mem_index));
    }
  }
}
// memory_change_callback() and memory_ctrl_callback() to support XXX
void Parser::memory_ctrl_callback() {
  // If we're still running CTOR return
  if (reset_running_) return;

  // If no change in mem_ctrl return
  uint8_t prev_mem_ctrl = memory_ctrl_prev_;
  uint8_t curr_mem_ctrl = mem_ctrl(); // Read reg
  if (prev_mem_ctrl == curr_mem_ctrl) return;
  // Else update our stashed copy...
  memory_ctrl_prev_ = curr_mem_ctrl;

  // ...and figure out what Parser mem_ctrl refers to
  RmtObjectManager *om = get_object_manager();
  int this_prsr_index = parser_index();
  int this_prsr_group = RmtDefs::get_parser_group(this_prsr_index);
  int prev_prsr_elt = static_cast<int>(prev_mem_ctrl);
  int curr_prsr_elt = static_cast<int>(curr_mem_ctrl);
  int prev_prsr_index = RmtDefs::get_parser_index(this_prsr_group, prev_prsr_elt);
  int curr_prsr_index = RmtDefs::get_parser_index(this_prsr_group, curr_prsr_elt);

  // Lookup prev/curr Parser if not referring to ourself
  Parser *prev_parser = this;
  if (prev_prsr_index != this_prsr_index) {
    ParserBlock *prev_prsr_block = om->parser_lookup(pipe_index(), prev_prsr_index);
    RMT_ASSERT(prev_prsr_block != NULL);
    prev_parser = (ingress()) ?prev_prsr_block->ingress() :prev_prsr_block->egress();
  }
  Parser *curr_parser = this;
  if (curr_prsr_index != this_prsr_index) {
    ParserBlock *curr_prsr_block = om->parser_lookup(pipe_index(), curr_prsr_index);
    RMT_ASSERT(curr_prsr_block != NULL);
    curr_parser = (ingress()) ?curr_prsr_block->ingress() :curr_prsr_block->egress();
  }

  // Some sanity checks
  RMT_ASSERT(subscribed_to_ == prev_parser);
  RMT_ASSERT(prev_parser != curr_parser);

  // Now unsubscribe from prev and subscribe to curr
  subscribed_to_ = NULL;
  if (prev_parser != this) prev_parser->unsubscribe(this);
  subscribed_to_ = curr_parser;
  if (curr_parser != this) curr_parser->subscribe(this);
}

void Parser::phv_clr_on_wr_change_callback() {
  if (reset_running_) return;
  clr_on_wr_changed_ = true;
}
void Parser::phv_clr_on_wr_validate() {
  if (!clr_on_wr_changed_) return;
  clr_on_wr_changed_ = false;
  for (int i = 0; i < 256; i++) {
    bool clr_on_wr_here = (prs_main_phv_clr_on_wr_.clr(i) == 1);
    bool clr_on_wr_pmerge = phv_clr_on_wr(i);
    RMT_ASSERT( (clr_on_wr_here == clr_on_wr_pmerge) && "Mismatch in clr_on_wr config");
  }
}
void Parser::phv_owner_change_callback() {
  if (reset_running_) return;
  owner_changed_ = true;
  ParserShared::phv_owner_change_callback();
}
void Parser::phv_owner_validate() {
  if (!owner_changed_) return;
  owner_changed_ = false;
  register_classes::PrsrRegMainRspecPhvOwnerMutable *r = get_phv_owner();
  // Check that off16s corresponding to PHV32s have identical ownership
  for (int i = 0; i < Phv::phv_max_p(); i++) {
    int size, offA, offB, sz8_01;
    Phv::phv_index_to_off16_p(i, &size, &offA, &offB, &sz8_01);
    if (size == 32) {
      RMT_ASSERT(off16_ok(offA) && off16_ok(offB));
      RMT_ASSERT(r->owner(offA) == r->owner(offB));
    }
  }
}
void Parser::parse_start(Packet *pkt, Phv *phv) {
  RMT_ASSERT((pkt != NULL) && (phv != NULL));
  // Initialize written_ BV - can't use PHV itself on JBay
  written_.fill_all_zeros();
  // Reset history buffer of recent phv_off16s written to
  history_reset();
  // If PHV clr_on_wr changed validate now
  if (clr_on_wr_changed_) phv_clr_on_wr_validate();
  // If PHV ownership changed validate now
  if (owner_changed_) phv_owner_validate();
  // Reset memory_write/memory_repair warnings
  memory_write_warn_ = memory_repair_warn_ = 0;
  // XXX: Initialise var that tracks whether we extract to PHV
  extracted_phv_ = false;
  // XXX: WIP: init var used to track shift amounts that have not been
  // applied to orig_hdr_len
  pending_shift_ = 0;
  // XXX: reset pending CLOT for each packet
  clot_reset();

  if (ingress()) {
    // Ingress - maybe update TM status into PHV
    uint32_t val_msb = 0u, val_lsb = 0u;
    if (tm_status_input(&val_msb, &val_lsb)) {
      tm_status_phv_write_msb(val_msb, phv);
      tm_status_phv_write_lsb(val_lsb, phv);  // only WIP
    }
  } else {
    // Egress - push Q info from egress pkt into ParseMerge
    uint32_t q_dpth = pkt->qing2e_metadata()->ing_q_depth();
    uint32_t q_id = static_cast<uint32_t>(pkt->i2qing_metadata()->qid()); // Correct qid?
    uint32_t p_id = static_cast<uint32_t>(pipe_index()); // Egress pipe or pipe of inbound port?
    uint32_t tm_status_msb = 0, tm_status_lsb = 0;
    int dst_msb = 31;
    // shift in pipe id
    int pipe_width = RmtDefs::kPackagePipeBits;  // pipe id bits = 2 jbay, 4 WIP
    dst_msb -= pipe_width;
    uint32_t src_mask = (0x1u << pipe_width) - 1;
    tm_status_msb |= (p_id & src_mask) << dst_msb;
    // XXX: shift in 4 bit MAC id (port group)
    dst_msb -= RmtDefs::kPortGroupWidth;
    RMT_ASSERT(RmtDefs::kPortGroupWidth < dst_msb);
    int group_num = 0xf;  // use invalid mac id as default in case port not set
    Port *port = pkt->port();
    if (nullptr != port) {
      // NB get_group_num() applies kPortGroupMask
      group_num = Port::get_group_num(port->port_index());
    }
    tm_status_msb |= group_num << dst_msb;
    // XXX:  shift in 7 bit queue id
    dst_msb -= RmtDefs::kI2qQidWidth;
    RMT_ASSERT(RmtDefs::kI2qQidWidth < dst_msb);
    tm_status_msb |= (q_id & RmtDefs::kI2qQidMask) << dst_msb;
    // shift in q_dpth msbs
    // XXX: previously the LSB's of q_dpth were written to tm_status, now
    // we we write as many of q_dpth MSBs as there is space.
    int q_dpth_width = RmtDefs::kTmQueueOccupancyWidth;
    src_mask = (0x1u << q_dpth_width) - 1;
    RMT_ASSERT(q_dpth_width > dst_msb);
    tm_status_msb |= (q_dpth & src_mask) >> (q_dpth_width - dst_msb);
    // write q_dpth lsbs to tm_status_lsb (WIP only)
    src_mask = (0x1u << (q_dpth_width - dst_msb)) - 1;
    tm_status_lsb |= (q_dpth & src_mask);
    if (q_dpth > 0) set_tm_status_input(tm_status_msb, tm_status_lsb);
  }
}
void Parser::parse_end_state(Packet *pkt, Phv *phv, int state_cnt, bool final) {
  // XXX: Complain if extracted a CLOT before extracting a PHV
  bool extracted_clot = (pkt->clot() != NULL);
  if (extracted_clot && !extracted_phv_) {
    RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserClot),
            "Parser::parse_end_state(StateCnt=%d) "
            "CLOT extracted *before* any PHV extraction. This is NOT allowed\n", state_cnt);
    if (!kRelaxExtractionCheck) { THROW_ERROR(-2); }
  }
  if (final) clot_discard(-1, "Parse end so"); // Will WARN if CLOT still pending
}
Phv *Parser::get_ghost_phv() {
  Phv *ghost_phv = NULL;
  uint8_t dummy = 0;
  uint32_t val = 0u;
  if (ingress() && phv_get_tm_status_phv(pipe_index(), &dummy) && tm_status_input(&val)) {
    RmtObjectManager *objmgr = get_object_manager();
    RMT_ASSERT(objmgr != NULL);
    ghost_phv = objmgr->phv_create();
    RMT_ASSERT(ghost_phv != NULL);
    ghost_phv->set_pipe(pipe_index());
    // Write in
    tm_status_phv_write_msb(val, ghost_phv);
  }
  return ghost_phv;
}

// XXX: Increment PHV but wrap in [0.255]
int Parser::phv_increment(int phv, int incr) {
  int phv_inc = phv + incr;
  if (!off16_ok(phv_inc)) {
    RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugParserExtract),
            "Parser::phv_increment: phv_off16 %d (%d+%d) is >= 256 so wrapped to %d\n",
            phv_inc, phv, incr, phv_inc % 256);
    phv_inc %= 256;
  }
  return phv_inc;
}


// Write 16b to a PHV off16
void Parser::phv_write_16b(Phv *phv, int phv_or_off, uint16_t val, uint8_t extract_type, ParseContext *context) {
  int size, phvWordA, phvWordB, sz32_01;
  Phv::phv_off16_to_index_p(phv_or_off, &size, &phvWordA, &phvWordB, &sz32_01);
  if (Phv::is_valid_phv_p(phvWordA)) {
    // XXX - Allow PHV container to be replaced in certain circumstances
    // rather than new data ORing in - this is configured in a ParseMerge CSR
    // but may be frustrated if multiple extracts occur to PHV container and
    // the extractions aren't sufficiently separated.
    bool want_clobber = phv_clr_on_wr(phv_or_off);
    bool do_clobber = (want_clobber) ?(!history_contains(phv_or_off)) :false;
    if (want_clobber && !do_clobber) {
      // WARN clobber not possible
      RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtract),
              "Parser::phv_write_16b() off %d was extracted to sometime in the last %d "
              "extractions so unable to unconditionally OVERWRITE (will OR instead)\n",
              phv_or_off, kWrHistorySize);
    }
    if (size == 8) {
      RMT_ASSERT(Phv::is_valid_phv_p(phvWordB));
      // 16b spread across 2 8b PHV words
      //
      // Initially I had version OLD below
      // So I fixed it to be NEW1 (after XXX)
      // But that was also wrong so now should be NEW2 (see XXX)
      // (though as it turns out NEW2 is the same as OLD for 8b)
      //
      // NB. If this logic changes MUST update logic in checksum-engine.cpp
      // too - search for refs to XXX !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      //
      //
      // Orig:
      // Assume we have a sequence of packet bytes P0 P1 P2 P3 ...
      // And we have a source/destination pair of src = P1, dst = X,
      // where X is an 8b container. The different extract types yield:
      //    PRSR_EXT_8B_LO: X+1 = P1
      //    PRSR_EXT_8B_HI: X   = P1
      //    PRSR_EXT_16B:   X   = P1, X+1 = P2
      //
      // Now:
      // Assume we have a sequence of packet bytes P0 P1 P2 P3 ...
      // And we have a source/destination pair of src = P1, dst = X,
      // where X is an 8b container. The different extract types yield:
      //    PRSR_EXT_8B_LO: X   = P1
      //    PRSR_EXT_8B_HI: X+1 = P1
      //    PRSR_EXT_16B:   X+1 = P1, X = P2
      //
      if (extract_type == kExtractType8bToLSB) {
        // phv->set_p(phvWordA, (val >> 8) & 0xFF); // OLD     X=P1
        //phv->set_p(phvWordB, (val >> 8) & 0xFF);  // NEW1  X+1=P1
        phv_set_p(phv, phvWordA, (val >> 8) & 0xFF, context, do_clobber);    // NEW2    X=P1
        // Maybe clobber phvWordB
        if (do_clobber) phv_set_p(phv, phvWordB, 0u, context, do_clobber);
      } else if (extract_type == kExtractType8bToMSB) {
        // phv->set_p(phvWordB, (val >> 8) & 0xFF); // OLD   X+1=P1
        //phv->set_p(phvWordA, (val >> 8) & 0xFF);  // NEW1    X=P1
        phv_set_p(phv, phvWordB, (val >> 8) & 0xFF, context, do_clobber);    // NEW2  X+1=P1
        // Maybe clobber phvWordA
        if (do_clobber) phv_set_p(phv, phvWordA, 0u, context, do_clobber);
      } else if (extract_type == kExtractType16b) {
        // phv->set_p(phvWordA, (val >> 8) & 0xFF); // OLD     X=P1
        // phv->set_p(phvWordB, (val >> 0) & 0xFF); // OLD   X+1=P2
        phv_set_p(phv, phvWordA, (val >> 0) & 0xFF, context, do_clobber);    // NEW2    X=P2
        phv_set_p(phv, phvWordB, (val >> 8) & 0xFF, context, do_clobber);    // NEW2  X+1=P1
      } else RMT_ASSERT(0);

    } else if (size == 16) {
      // XXX - check below was too strict
      // RMT_ASSERT(extract_type == kExtractType16b);

      // 16b in a single 16b PHV word

      // Handle kExtractType8b* specially (see XXX)
      if      (extract_type == kExtractType8bToLSB) val >>= 8;
      else if (extract_type == kExtractType8bToMSB) val  &= 0xFF00;

      phv_set_p(phv, phvWordA, val, context, do_clobber);
    } else {
      // XXX - check below was too strict
      // RMT_ASSERT(extract_type == kExtractType16b);
      RMT_ASSERT(size == 32);

      // 16b in either low or high 16b of a single 32b PHV word

      // Handle kExtractType8b* specially (see XXX)
      if      (extract_type == kExtractType8bToLSB) val >>= 8;
      else if (extract_type == kExtractType8bToMSB) val  &= 0xFF00;
      // Also contMSB used to be 0 before XXX
      int      contMSB = 1, contLSB = 1-contMSB;
      uint32_t val32 = static_cast<uint32_t>(val);

      // Fetch existing 32b
      uint32_t new_val = phv->get_p(phvWordA);
      // If clobbering need to clear HI or LO 16b
      if (do_clobber) {
        if      (sz32_01 == contMSB) new_val &= ~(0xFFFFu << 16); // Clear MSBs
        else if (sz32_01 == contLSB) new_val &= ~(0xFFFFu <<  0); // Clear LSBs
      }
      // OR in new 16b - if clobbering will have cleared the 16b above
      if      (sz32_01 == contMSB) new_val |= (val32 << 16); // Set MSBs
      else if (sz32_01 == contLSB) new_val |= (val32 <<  0); // Set LSBs
      else RMT_ASSERT(0);

      // And overwrite with new_val
      phv_set_p(phv, phvWordA, new_val, context, true);
    }
    // Track that we've written to this off16
    written_.set_bit(true, phv_or_off);
    // And write extraction into history
    history_write(phv_or_off);
  }
}
void Parser::phv_write_16b(Phv *phv, int phv_or_off, uint16_t val, ParseContext *context) {
  phv_write_16b(phv, phv_or_off, val, kExtractType16b, context);
}
// As above but returning error code
uint32_t Parser::phv_check_write_16b(Phv *phv, int phv_or_off, uint16_t val, uint8_t extract_type, ParseContext *context) {
  uint32_t err = 0u;
  if (phv_can_write(phv, phv_or_off)) {
    phv_write_16b(phv, phv_or_off, val, extract_type, context);
  } else {
    // Can't write - figure out error code
    if      (!phv_valid_16b(phv, phv_or_off)) err |= kErrDstCont;
    else if (!phv_owner_16b(phv, phv_or_off)) err |= kErrPhvOwner;
    else                                      err |= kErrMultiWr;
  }
  return err;
}
uint32_t Parser::phv_check_write_16b(Phv *phv, int phv_or_off, uint16_t val, ParseContext *context) {
  return phv_check_write_16b(phv, phv_or_off, val, kExtractType16b, context);
}

// Output parse errors to a OFF16 format dst phv word determined by config
void Parser::perr_phv_write(uint16_t dst, uint32_t err, Phv *phv) {
  if ((err == 0u) || (phv == NULL)) return;
  // Note after next mapping step kErr* tokens may no longer match
  uint16_t hw_err = static_cast<uint16_t>(prs_map_sw_errs_to_hw_errs(err) & 0xFFFF);
  // PHV ownership checked for ERR container - but multi-wr ignored.
  RMT_ASSERT(phv_owner_16b(phv, dst));
  ParseContext context("emitted errors");
  context.extracted_value_ = hw_err;
  context.extracted_value_size_ = 2;
  phv_write_16b(phv, static_cast<int>(dst), hw_err, &context);
  // XXX: Track we extracted PHV
  extracted_phv_ = true;
}

// Output TM status to a OFF16 format INGRESS dst_phv word determined by config
void Parser::tm_status_phv_write_msb(uint32_t tm_status, Phv *phv) {
  uint8_t val = 0;
  // Might not be enabled at all or not enabled for pipe
  if (!phv_get_tm_status_phv(pipe_index(), &val)) return;
  int phv_or_off = static_cast<int>(val);
  RMT_ASSERT(ingress()); // Should be ingress Parser writing to ingress PHV words
  ParseContext context("emitted TM status");
  context.extracted_value_ = tm_status;
  context.extracted_value_size_ = 4;
  // PHV ownership/multi-wr both ignored for TM status write
  // Also note contMSB used to be 0 before XXX
  int contMSB = 1, contLSB = 1-contMSB;
  phv_write_16b(phv, phv_or_off + contMSB, tm_status >> 16, &context);
  phv_write_16b(phv, phv_or_off + contLSB, tm_status >>  0, &context);
  // XXX: tm_status write does NOT set extracted_phv_
  phv->set_ghost();
}


// Function to handle initial match byte load (uses start_lookup_offsets CSR)
void Parser::inbuf0_maybe_get_initial(int chan,
                                      uint8_t *v8_3, uint8_t *v8_2,
                                      uint8_t *v8_1, uint8_t *v8_0) {
  RMT_ASSERT((v8_3 != NULL) && (v8_2 != NULL) && (v8_1 != NULL) && (v8_0 != NULL));
  //*v8_3 = start_lookup_offsets(chan, 3) & kLoadOffsetMask;
  //*v8_2 = start_lookup_offsets(chan, 2) & kLoadOffsetMask;
  //*v8_1 = start_lookup_offsets(chan, 1) & kLoadOffsetMask;
  //*v8_0 = start_lookup_offsets(chan, 0) & kLoadOffsetMask;
  // XXX: Offsets must be LOOKED UP in inbuf0
  InputBuffer *inbuf0 = get_inbuf(0);
  inbuf0->get( start_lookup_offsets(chan, 3) & kLoadOffsetMask, v8_3 );
  inbuf0->get( start_lookup_offsets(chan, 2) & kLoadOffsetMask, v8_2 );
  inbuf0->get( start_lookup_offsets(chan, 1) & kLoadOffsetMask, v8_1 );
  inbuf0->get( start_lookup_offsets(chan, 0) & kLoadOffsetMask, v8_0 );
}
// Functions to handle different capabilities of inbuf1 (scratch pad buffer)
void Parser::inbuf1_setup(Packet *pkt) {
  // Initialize inbuf1
  InputBuffer *inbuf1 = get_inbuf(1);
  inbuf1->reset();
  // On JBay fill inbuf1 with upto 28B from packet and rest zeros
  int sz = inbuf1->fill(pkt, 0, 28);
  inbuf1->fill_zeros(32-sz);
}
void Parser::inbuf1_maybe_fill(int pi, uint8_t v8_3, uint8_t v8_2, uint8_t v8_1, uint8_t v8_0,
                               int state_cnt) {
  // NO scratch_upd on JBay so no dynamic refill of inbuf1 from inbuf0
  // if (!scratch_upd(pi)) return;
  // inbuf_reset(1);
  // inbuf_copy(1, 0, 32);
  //
  // BUT JBay can update any/all of last 4 bytes of inbuf1 from passed-in match bytes
  bool sv_unconditional = (state_cnt == 0); // Always save first time
  bool sv3 = save_lookup(pi,3) || sv_unconditional;
  bool sv2 = save_lookup(pi,2) || sv_unconditional;
  bool sv1 = save_lookup(pi,1) || sv_unconditional;
  bool sv0 = save_lookup(pi,0) || sv_unconditional;
  if (!sv3 && !sv2 && !sv1 && !sv0) return; // Bail - no save required
  InputBuffer *inbuf1 = get_inbuf(1);
  // If NOT overwriting fish out existing byte to put back
  if (!sv3) inbuf1->get(31-3, &v8_3);
  if (!sv2) inbuf1->get(31-2, &v8_2);
  if (!sv1) inbuf1->get(31-1, &v8_1);
  if (!sv0) inbuf1->get(31-0, &v8_0);
  inbuf1->rewind(4);
  inbuf1->fill_byte(v8_3);
  inbuf1->fill_byte(v8_2);
  inbuf1->fill_byte(v8_1);
  inbuf1->fill_byte(v8_0);
}
// Function to handle different capabilities of inbuf2 (TS/Version buffer)
void Parser::inbuf2_setup(Packet *pkt) {
  InputBuffer *inbuf2 = get_inbuf(2);
  inbuf2->reset();
  // For both ingress/egress parser we fill buf2 exactly with
  // a) 6 bytes of timestamp b) 4 bytes of version
  uint32_t ver = version_get();
  inbuf2->fill_val((uint16_t)0, 2, false);
  inbuf2->fill_val(timestamp_get(), 6, false);
  inbuf2->fill_val(ver, 4, false);
  inbuf2->fill_val(ver, 4, false);
  // Add extra 0 to allow 2 byte read at offset 63 - logic in
  // inbuf_extract_to_phv checks this is always a halfwidth read
  // so does not use byte 64
  inbuf2->fill_val((uint8_t)0, 1, false);
  // And we set read_pos_mod to 48 - any read offset less than this will
  // cause an error on get() - any read offset > 64 will also fail
  inbuf2->set_read_pos_mod(48);
}
void Parser::inbuf2_update_consts(uint32_t v) {
  // On JBay put passed-in const at end of buf2
  InputBuffer *inbuf2 = get_inbuf(2);
  inbuf2->rewind(5); // Rewind by 5
  inbuf2->fill_val(v, 4, false);
  inbuf2->fill_val((uint8_t)0, 1, false);
}

// Functions to handle JBay CounterStack
bool Parser::cntrstack_push(int pi, int8_t cntr, bool propagate_incr) {
  RMT_ASSERT(counter_stack_push(pi));
  bool ok = ctr_stack_.push(cntr, propagate_incr);
  if (!ok) {
    RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugParserCounterStack),
            "Parser::cntrstack_push(index=%d) CounterStack was already full! "
            "So bottom/oldest pushed value discarded\n", pi);
  }
  return propagate_incr;
}
bool Parser::cntrstack_maybe_push(int pi, int8_t cntr, int8_t incr) {
  if (!counter_stack_push(pi)) return false;
  bool propagate_incr = counter_stack_upd_w_top(pi);
  // XXX
  // if stack_upd_w_top set will need to add incr to value we're about to push
  if (propagate_incr) cntr += incr;
  bool ok = ctr_stack_.push(cntr, propagate_incr);
  if (!ok) {
    RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugParserCounterStack),
            "Parser::cntrstack_maybe_push(index=%d) CounterStack was already full! "
            "So bottom/oldest pushed value discarded\n", pi);
  }
  return propagate_incr;
}
void Parser::cntrstack_increment(int pi, int8_t cntr) {
  ctr_stack_.incr(cntr); // Counters just wrap
}
int8_t Parser::cntrstack_pop(int pi, int8_t cntr) {
  bool ok = ctr_stack_.pop(&cntr);
  if (!ok) {
    RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugParserCounterStack),
            "Parser::cntrstack_pop(index=%d) CounterStack was empty! "
            "So counter value zeroed\n", pi);
  }
  return cntr;
}

void Parser::get_lookup_offset(int pi, int n, uint8_t *off, bool *load, int *which_buf) {
  RMT_ASSERT((n >= 0) && (n < kLoadBytes));
  RMT_ASSERT((off != NULL) && (load != NULL) && (which_buf != NULL));
  uint8_t v = prs_ea_field8_N_lookup_offset(pi, n);
  uint8_t ld = prs_ea_field8_N_load(pi, n);
  *load = ((ld & 0x1) == 0x1);
  *which_buf = (v >> kLoadOffsetWidth) & 0x1;
  *off = (v >> 0) & kLoadOffsetMask;
}
uint8_t Parser::prs_ea_field8_N_lookup_offset(int pi, int n) {
  RMT_ASSERT((n >= 0) && (n < kLoadBytes));
  return ea_ram_->lookup_offset_8(pi, n);
}
uint8_t Parser::prs_ea_field8_N_load(int pi, int n) {
  RMT_ASSERT((n >= 0) && (n < kLoadBytes));
  return ea_ram_->ld_lookup_8(pi, n);
}
uint8_t Parser::prs_ea_field16_lookup_offset(int pi) {
  RMT_ASSERT(0); // offset_16 not avail on JBay
}
uint8_t Parser::prs_ea_field16_load(int pi) {
  RMT_ASSERT(0); // lookup_16 not avail on JBay
}

void Parser::set_lookup_offset(int pi, int n, uint8_t off, bool load, int which_buf) {
  RMT_ASSERT((n >= 0) && (n < kLoadBytes));
  uint8_t v = ((which_buf & 0x1) << kLoadOffsetWidth) | ((off & kLoadOffsetMask) << 0);
  prs_ea_set_field8_N_lookup_offset(pi, n, v);
  prs_ea_set_field8_N_load(pi, n, (load) ?1 :0);
}
void Parser::prs_ea_set_field8_N_lookup_offset(int pi, int n, uint8_t v) {
  RMT_ASSERT((n >= 0) && (n < kLoadBytes));
  ea_ram_->lookup_offset_8(pi, n, v);
}
void Parser::prs_ea_set_field8_N_load(int pi, int n, uint8_t v) {
  RMT_ASSERT((n >= 0) && (n < kLoadBytes));
  ea_ram_->ld_lookup_8(pi, n, v);
}
void Parser::prs_ea_set_field16_lookup_offset(int pi, uint8_t v) {
  RMT_ASSERT(0); // offset_16 not avail on JBay
}
void Parser::prs_ea_set_field16_load(int pi, uint8_t v) {
  RMT_ASSERT(0); // lookup_16 not avail on JBay
}



// Set ParserEarlyAction vals for some action index
void Parser::set_early_action(int index, uint8_t _counter_load_imm,
                              uint8_t _counter_ctr_op, bool _done, uint8_t _shift_amount,
                              uint8_t _field8_3_lookup_offset, uint8_t _field8_2_lookup_offset,
                              uint8_t _field8_1_lookup_offset, uint8_t _field8_0_lookup_offset,
                              bool _load_field8_3, bool _load_field8_2,
                              bool _load_field8_1, bool _load_field8_0,
                              uint8_t _next_state_mask, uint8_t _next_state) {
  // Call ParserShared func to initialise common fields
  // Here we call 2b variant of set_early_action_shared
  set_early_action_shared(index, _counter_load_imm, _counter_ctr_op,
                          _done, _shift_amount, _next_state_mask, _next_state);
  set_field8_3_lookup_offset(index, _field8_3_lookup_offset);
  set_field8_2_lookup_offset(index, _field8_2_lookup_offset);
  set_field8_1_lookup_offset(index, _field8_1_lookup_offset);
  set_field8_0_lookup_offset(index, _field8_0_lookup_offset);
  set_load_field8_3(index, _load_field8_3);
  set_load_field8_2(index, _load_field8_2);
  set_load_field8_1(index, _load_field8_1);
  set_load_field8_0(index, _load_field8_0);
}
void Parser::set_early_action(int index, uint8_t _counter_load_imm,
                              bool _counter_load_src, bool _counter_load,
                              bool _done, uint8_t _shift_amount,
                              uint8_t _field8_3_lookup_offset, uint8_t _field8_2_lookup_offset,
                              uint8_t _field8_1_lookup_offset, uint8_t _field8_0_lookup_offset,
                              bool _load_field8_3, bool _load_field8_2,
                              bool _load_field8_1, bool _load_field8_0,
                              uint8_t _next_state_mask, uint8_t _next_state) {
  // Figure out counter_ctr_op corresponding to counter_load/load_src and call func above
  uint8_t counter_ctr_op = (_counter_load?2:0) | (_counter_load_src?1:0);
  set_early_action(index, _counter_load_imm, counter_ctr_op,
                   _done, _shift_amount,
                   _field8_3_lookup_offset, _field8_2_lookup_offset,
                   _field8_1_lookup_offset, _field8_0_lookup_offset,
                   _load_field8_3, _load_field8_2,
                   _load_field8_1, _load_field8_0,
                   _next_state_mask, _next_state);
}
// Allow initialisation using 2x byte offsets and 1x short offset
void Parser::set_early_action(int index, uint8_t _counter_load_imm,
                              bool _counter_load_src, bool _counter_load,
                              bool _done, uint8_t _shift_amount,
                              uint8_t _field8_1_lookup_offset, uint8_t _field8_0_lookup_offset,
                              uint8_t _field16_lookup_offset,
                              bool _load_field8_1, bool _load_field8_0, bool _load_field16,
                              uint8_t _next_state_mask, uint8_t _next_state) {
  set_early_action(index, _counter_load_imm, _counter_load_src, _counter_load,
                   _done, _shift_amount,
                   _field8_1_lookup_offset, _field8_0_lookup_offset,
                   _field16_lookup_offset, 1 + _field16_lookup_offset,
                   _load_field8_1, _load_field8_0, _load_field16, _load_field16,
                   _next_state_mask, _next_state);
}

// extractor helper methods
// What is the first extractor that's NOT used for checksumming
int Parser::extract16_first_non_checksum(int n_xt, int ck8, int ck16, int ck32) {
  return ck8 + ck16 + ck32;
}
// Checksum extraction uses up low-numbered extractors
bool Parser::extract16_available(int i_xt, int n_xt, int ck8, int ck16, int ck32) {
  // These days 32b checksums just use 1x16b extractor
  int n_16b_xt_used = ck8 + ck16 + ck32;
  return ((i_xt >= n_16b_xt_used) && (i_xt < n_xt));
}


uint8_t  Parser::extract16_inbuf_shift(int pi, int i)          {
  //TODO:JBAY:256x16 fixup
  //return ((act_ram_->extract_is_8b(pi,i) & 1) == 1) ?8 :0;
  return 0;
}
bool Parser::extract16_src_imm_val(int pi, int i)  {
  RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
  // Always return false - IMM constants handled differently on JBay
  return false;
}
bool Parser::extract16_rot_imm_val(int pi, int i)  {
  RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
  // Always return false - IMM constants handled differently on JBay
  return false;
}
void Parser::set_extract16_src_imm_val(int pi, int i, bool tf)  {
  RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
  // no-op for jbay
}
void Parser::set_extract16_rot_imm_val(int pi, int i, bool tf)  {
  RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
  // no-op for jbay
}


// Get TCAM entry out of regs
BitVector<44> Parser::prs_tcam_get_entry(memory_classes::PrsrMlTcamRowArrayMutable &prs,
                                         uint32_t i, uint32_t which_word) {
  if (which_word == 0) {
    uint8_t v8_3 = prs.w0_lookup_8(i,3);
    uint8_t v8_2 = prs.w0_lookup_8(i,2);
    uint8_t v8_1 = prs.w0_lookup_8(i,1);
    uint8_t v8_0 = prs.w0_lookup_8(i,0);
    uint8_t state = prs.w0_curr_state(i);
    bool cnt_eq_0 = ((prs.w0_ctr_zero(i) & 0x1) == 0x1);
    bool cnt_lt_0 = ((prs.w0_ctr_neg(i) & 0x1) == 0x1);
    bool ver0 = ((prs.w0_ver_0(i) & 0x1) == 0x1);
    bool ver1 = ((prs.w0_ver_1(i) & 0x1) == 0x1);
    return make_tcam_entry(ver1, ver0, cnt_eq_0, cnt_lt_0, state, v8_3, v8_2, v8_1, v8_0);
  } else if (which_word == 1) {
    uint8_t v8_3 = prs.w1_lookup_8(i,3);
    uint8_t v8_2 = prs.w1_lookup_8(i,2);
    uint8_t v8_1 = prs.w1_lookup_8(i,1);
    uint8_t v8_0 = prs.w1_lookup_8(i,0);
    uint8_t state = prs.w1_curr_state(i);
    bool cnt_eq_0 = ((prs.w1_ctr_zero(i) & 0x1) == 0x1);
    bool cnt_lt_0 = ((prs.w1_ctr_neg(i) & 0x1) == 0x1);
    bool ver0 = ((prs.w1_ver_0(i) & 0x1) == 0x1);
    bool ver1 = ((prs.w1_ver_1(i) & 0x1) == 0x1);
    return make_tcam_entry(ver1, ver0, cnt_eq_0, cnt_lt_0, state, v8_3, v8_2, v8_1, v8_0);
  } else {
    RMT_ASSERT(0);
  }
}
// Handle TCAM change - copy changed row from regs -> s/w
void Parser::prs_tcam_change(uint32_t i) {
  BitVector<44> word0 = prs_tcam_get_entry(prs_tcam_word0s_word1s_, i, 0);
  BitVector<44> word1 = prs_tcam_get_entry(prs_tcam_word0s_word1s_, i, 1);
  set_tcam_word0_word1(i, word0, word1);
}


// JUST USED BY UNIT TEST LOGIC
// Functions to handle mapping OFF16 format dst_phv to PHV format dst_phv
uint16_t Parser::extract16_dst_phv_by_phv(int pi, int i) {
  int off16 = extract16_dst_phv_by_off16(pi, i);
  uint8_t ext_type = extract16_type(pi, i);
  if (ext_type == kExtractTypeNone) return k_phv::kBadPhv;
  RMT_ASSERT(off16_ok(off16));
  int size, phvWordA, phvWordB, sz32_01;
  Phv::phv_off16_to_index_p(off16, &size, &phvWordA, &phvWordB, &sz32_01);
  RMT_ASSERT((phvWordA >= 0) && (phvWordA < Phv::phv_max_p()));
  if (size == 8) {
    RMT_ASSERT((phvWordB >= 0) && (phvWordB < Phv::phv_max_p()));
    if (ext_type == kExtractType8bToLSB) return static_cast<uint16_t>(phvWordA);
    if (ext_type == kExtractType8bToMSB) return static_cast<uint16_t>(phvWordB);
    RMT_ASSERT(0);
  } else {
    RMT_ASSERT((size == 16) || (size == 32));
    if (ext_type == kExtractType16b) return static_cast<uint16_t>(phvWordA);
    RMT_ASSERT(0);
  }
}
// JUST USED BY UNIT TEST LOGIC
uint8_t Parser::set_extract16_dst_phv_by_phv(int pi, int i, uint16_t v, int which16) {
  uint8_t type = kExtractTypeNone;
  if (v < Phv::phv_max_p()) {
    int size, offA, offB, sz8_01;
    Phv::phv_index_to_off16_p(v, &size, &offA, &offB, &sz8_01);
    RMT_ASSERT(off16_ok(offA));
    if (size == 32) {
      RMT_ASSERT(off16_ok(offB));
      RMT_ASSERT((which16 == 0) || (which16 == 1));
      set_extract16_dst_phv_by_off16(pi, i, (which16==0) ?offA :offB);
      type = kExtractType16b;
    } else if (size == 8) {
      RMT_ASSERT(which16 == 0);
      set_extract16_dst_phv_by_off16(pi, i, offA);
      if      (sz8_01 == 0) type = kExtractType8bToLSB;
      else if (sz8_01 == 1) type = kExtractType8bToMSB;
      else RMT_ASSERT(0);
    } else {
      RMT_ASSERT(size == 16);
      RMT_ASSERT(which16 == 0);
      set_extract16_dst_phv_by_off16(pi, i, offA);
      type = kExtractType16b;
    }
  }
  set_extract16_type(pi, i, type);
  return type;
}



// Functions to handle different extraction capabilities
uint16_t Parser::map_dst_phv(uint16_t phv_in, int from_format) {
  if (phv_in == k_phv::kBadPhv) return phv_in;
  RMT_ASSERT(phv_in < Phv::phv_max_extended_p());
  return phv_in;
}
bool Parser::extract_ok_phv(int phv_off16) {
  if (phv_off16 == k_phv::kBadPhv) return false;
  return (off16_ok(phv_off16));
}
bool Parser::extract_ok_tphv(int phv_off16) {
  return false; // No TPHV on JBay
}
int Parser::get_phv_to_write(int phv_off16) {
  return (extract_ok_phv(phv_off16)) ?phv_off16 :-1;
}
int Parser::get_tphv_to_write(int phv_off16) {
  return -1; // No TPHV on JBay
}

// Function to apply other early (pre-extraction) checks
uint32_t Parser::other_early_error_checks(int match_index) {
  static_assert( ((kExtractMax_8b == 0) && (kExtractMax_32b == 0)),
                 "jbay::Parser::other_early_error_checks assumes only 16b extractors");
  bool relax = kRelaxPreExtractionCheck;
  BitVector<256> would_write(UINT64_C(0)); // Track writes this cycle
  uint32_t oth_errs = 0u;
  for (int i = 0; i < kExtractMax_16b; i++) {
    uint8_t xt_type = extract16_type(match_index, i);
    if (xt_type != kExtractTypeNone) {
      uint8_t  src_off = extract16_src(match_index, i, kExtractSrcMask);
      uint16_t dst_phv_off16 = extract16_dst_phv_by_off16(match_index, i);
      uint32_t new_errs = 0u;
      // XXX: Might need to apply offset_adjust_
      if (extract16_add_off(match_index, i))
        dst_phv_off16 = phv_increment(dst_phv_off16, offset_adjust());
      // JBay allows extraction anywhere in the buffer, even beyond buf_occ
      if (!extract_offset_check(xt_type, src_off, NULL)) new_errs |= kErrSrcExt;
      if (!phv_owner(dst_phv_off16))                     new_errs |= kErrPhvOwner;
      // Only need to check what's written/would be written if NOT multi_write
      if (!phv_multi_write(dst_phv_off16)) {
        if (written_.bit_set(dst_phv_off16) || would_write.bit_set(dst_phv_off16))
          new_errs |= kErrMultiWr;
        would_write.set_bit(true, dst_phv_off16);
      }
      if (new_errs != 0u) {
        RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractError,relax),
                "Parser::other_early_error_checks(%d) ERRORS=0x%04x[%s] "
                "Extractor=%d SrcOff=%d DstOff16=%d\n",
                match_index, new_errs, errstr_get(new_errs), i, src_off, dst_phv_off16);
        oth_errs |= new_errs;
      }
    }
  }
  // If final cycle (or errors already) also check residual PHV word - XXX
  if (done(match_index) || (oth_errs != 0u)) {
    for (int ci = 0; ci < kChecksumEngines; ci++) {
      ChecksumEngine *ceng = get_checksum_engine(ci);
      if (checksum_enable(match_index, ci)) {
        int pi = checksum_ram_addr(match_index, ci);
        RMT_ASSERT(pi < kChecksumEntries);
        uint16_t dst_phv_off16;
        if (ceng->get_residual_dst_phv_word(pi, &dst_phv_off16)) {
          uint32_t new_errs = 0u;
          if (!phv_owner(dst_phv_off16)) new_errs |= kErrPhvOwner;
          if (!phv_multi_write(dst_phv_off16)) {
            if (written_.bit_set(dst_phv_off16) || would_write.bit_set(dst_phv_off16))
              new_errs |= kErrMultiWr;
            would_write.set_bit(true, dst_phv_off16);
          }
          if (new_errs != 0u) {
            RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractError,relax),
                    "Parser::other_early_error_checks(%d) ERRORS=0x%04x[%s] "
                    "ChecksumEngine=%d ChecksumRamAddr=%d DstOff16=%d\n",
                    match_index, new_errs, errstr_get(new_errs), ci, pi, dst_phv_off16);
            oth_errs |= new_errs;
          }
        }
      }
    }
  }
  return oth_errs;
}

// Function to apply further extraction constraint checks
void Parser::check_further_extract_constraints(uint32_t check_flags,
                                               int index, int which_extract,
                                               int phv_off16, Phv *phv, int extract_sz,
                                               bool partial_hdr_err) {
  // Check we have valid off16 on JBay
  if (!off16_ok(phv_off16)) {
    RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractSoftError),
            "Parser::inbuf_extract_%ld_to_%ld(%d,%d) INVALID off16=%d\n",
            extract_sz, extract_sz, index, which_extract, phv_off16);
    if (!kRelaxExtractionCheck) { THROW_ERROR(-2); }
  }
  // Do a special can-write check for JBay
  if (!phv_can_write(phv, phv_off16)) {
    const char *ownstr = phv_owner(phv_off16) ?"" :"not owner";
    const char *nmwstr = phv_multi_write(phv_off16) ?"" :"not multi-writable";
    RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractSoftError),
            "Parser::inbuf_extract_%ld_to_%ld(%d,%d) off16=%d "
            "CAN NOT WRITE<%d> %d  %s %s\n",
            extract_sz, extract_sz,
            index, which_extract, phv_off16, io_index(), phv_off16,
            ownstr, nmwstr);
    // XXX: don't throw in this scenario - overzealous
    // if (!kRelaxExtractionCheck) { THROW_ERROR(-2); }
  }
}
// Function to apply checksum constraint checks
bool Parser::check_checksum_constraints(int match_index,
                                        int &wrote_8b, int &wrote_16b, int &wrote_32b) {
  static_assert( ((kExtractMax_8b == 0) && (kExtractMax_32b == 0)),
                 "jbay::Parser::check_checksum_constraints assumes only 16b extractors");
  static_assert( (kExtractMax_16b <= 32), "Extractor bitmasks must fit in uint32_t");
  bool  ok = true;
  uint32_t xtrac_all    = 0xFFFFFFFFu >> (32-kExtractMax_16b);
  uint32_t xtrac_active = 0u; // Extractors active/used (not kExtractTypeNone)
  uint32_t xtrac_clr    = 0u; // Inactive extractors where dst container is clr_on_wr

  for (int i = 0; i < kExtractMax_16b; i++) {
    // What extractors are actually configured to be used
    if (extract16_type(match_index, i) != kExtractTypeNone) {
      xtrac_active |= 1u<<i;
    } else {
      // Extractor NOT used - but track whether configured dst container is clr_on_wr
      uint16_t off16 = extract16_dst_phv_by_off16(match_index, i);
      if (phv_clr_on_wr(off16)) xtrac_clr |= 1u<<i;
    }
  }
  RMT_ASSERT((xtrac_active & xtrac_clr) == 0u);

  int first_non_checksum = extract16_first_non_checksum(kExtractMax_16b,
                                                        wrote_8b, wrote_16b, wrote_32b);
  int last_used = (xtrac_active == 0u) ?-1 :__builtin_clzl(1u) - __builtin_clzl(xtrac_active);
  uint32_t xtrac_range = (last_used >= 0) ?0xFFFFFFFFu >> (32 - (last_used + 1)) :0u;
  uint32_t xtrac_holes = xtrac_range & ~xtrac_active;
  uint32_t xtrac_cksum = (first_non_checksum > 0) ?0xFFFFFFFFu >> (32 - first_non_checksum) :0u;

  // ERROR if checksum write but none of extractors[max_cksum...19] active
  if (first_non_checksum > 0) {
    RMT_ASSERT((wrote_8b > 0) || (wrote_16b > 0) || (wrote_32b > 0));
    // Make mask containing all extractors above & including max checksumming extractor
    uint32_t xtrac_check = xtrac_all & ~(xtrac_cksum >> 1);
    if ((xtrac_check & xtrac_active) == 0u) {
      RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractError),
              "Parser::check_checksum_constraints(%d) %d 16b checksum write(s) required "
              "but NO extractors >= %d active\n", match_index,
              first_non_checksum, first_non_checksum-1);
      ok = false;
    }
  }
  // ERROR if 'holes' in usage of active extractors and PHV containers are clr_on_wr
  if ((xtrac_holes & xtrac_clr) != 0u) {
    RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractError),
            "Parser::check_checksum_constraints(%d) Some unused extractors are "
            "configured for clr_on_wr - this will cause these containers to be zeroed ",
            "(Unused=0x%08x ClrOnWr=0x%08x 16bChecksumWrites=%d)\n",
            match_index, xtrac_holes, xtrac_clr, first_non_checksum);
    ok = false;
  }
  // WARN if not all checksumming extractors active
  if ((ok) && ((xtrac_cksum & xtrac_active) != xtrac_cksum)) {
    RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugParserExtract),
            "Parser::check_checksum_constraints(%d) Some checksumming extractors "
            "NOT marked active. Any extractor used for checksumming should have an "
            "active dummy extract. (Active=0x%08x Checksumming=0x%08x 16bChecksumWrites=%d)\n",
            match_index, xtrac_active, xtrac_cksum, first_non_checksum);
  }
  // WARN if any other 'holes' in usage of active extractors - inefficient
  if ((ok) && (xtrac_holes & ~xtrac_cksum) != 0u) {
    RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugParserExtract),
            "Parser::check_checksum_constraints(%d) Non-contiguous extractors used. "
            "This is inefficient. (Unused=0x%08x Active=0x%08x 16bChecksumWrites=%d)\n",
            match_index, xtrac_holes, xtrac_active, first_non_checksum);
  }
  if (!ok && !kRelaxExtractionCheck) { THROW_ERROR(-2); }
  return ok;
}

// Function to do JBay-specific 16b extract
uint32_t Parser::perform_extract_16b(int index, int which_extract, const int word_tab[],
                                     int phv_off16, Phv *phv, uint16_t val,
                                     uint8_t extract_type,
                                     ParseContext *context) {
  uint32_t err = phv_check_write_16b(phv, phv_off16, val, extract_type, context);
  // XXX: Track we extracted PHV
  if (err == 0u) extracted_phv_ = true;
  return err;
}


// Function to handle different capabilties wrt Version Update
uint8_t Parser::update_version(int index, uint8_t curr_ver) {
  // JBay can update pkt version from action ram or packet
  uint8_t ret_ver = curr_ver;
  uint8_t upd_type = ver_upd_type(index);
  if (upd_type == 0) {
    // If ver update enabled use val_mask as literal value
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserVersion),
            "Parser::update_version(index=%d,curr_ver=%d,UpdType=0)\n",
            index, curr_ver);
    if ((ver_upd_en_shr(index) & 0x1) != 0) {
      ret_ver = ver_upd_val_mask(index);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugExit),
              "Parser::update_version(index=%d,curr_ver=%d,UpdType=0 - ret_ver=%d)\n",
              index, curr_ver, ret_ver);
    }
  } else if (upd_type == 1) {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserVersion),
            "Parser::update_version(index=%d,curr_ver=%d,UpdType=1)\n",
            index, curr_ver);
    // Get 16-bit value from packet - only ever from buffer 0
    uint16_t v16 = 0;
    inbuf_get(ver_upd_src(index), &v16, true, true, 0);
    // _Always_ update the version: data missing from the buffer
    // is replaced with zeros in RTL.
    // Shift and mask and use bottom 2-bits as ver
    v16 >>= ver_upd_en_shr(index);
    v16 &= static_cast<uint16_t>(ver_upd_val_mask(index));
    ret_ver = static_cast<uint8_t>(v16 & 0x3);
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserVersion),
            "Parser::update_version(index=%d,curr_ver=%d,UpdType=1,"
            "V16=0x%04x - ret_ver=%d)\n",
            index, curr_ver, v16, ret_ver);
  }
  return ret_ver;
}
void Parser::set_action_ver(int index,
                            uint8_t ver_upd_type, uint8_t ver_upd_src,
                            uint8_t ver_upd_en_shr, uint8_t ver_upd_val_mask) {
  set_ver_upd_type(index, ver_upd_type);
  set_ver_upd_src(index, ver_upd_src);
  set_ver_upd_en_shr(index, ver_upd_en_shr);
  set_ver_upd_val_mask(index, ver_upd_val_mask);
}

// Function to handle different capabilities wrt Priority Mapping
uint8_t Parser::map_priority(int chan, uint8_t curr_pri) {
  uint8_t mapped_pri = pri_map(chan, curr_pri);
  if (mapped_pri != curr_pri) {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserPriority),
            "Parser::map_priority(chan=%d,curr_pri=%d,mapped_pri=%d)\n",
            chan, curr_pri, mapped_pri);
  }
  return mapped_pri;
}
// Function to setup identity Priority Mapping - programs map regs
void Parser::set_identity_priority_map() {
  for (int chan = 0; chan < kChannels; chan++) {
    for (int pri = 0; pri < kPriorities; pri++) set_pri_map(chan,pri,pri);
  }
}


// Function to setup CLOT regs
void Parser::set_action_clot(int index, int clot_index,
                             uint8_t clot_type,
                             uint8_t clot_len_src, uint8_t clot_en_len_shr,
                             uint8_t clot_len_mask, uint8_t clot_len_add,
                             uint8_t clot_offset,
                             uint8_t clot_tag, bool clot_tag_offset_add,
                             uint8_t clot_has_csum) {
  set_clot_type(index, clot_index, clot_type);
  set_clot_en_len_shr(index, clot_index, clot_en_len_shr);
  set_clot_len_src(index, clot_index, clot_len_src);
  set_clot_len_mask(index, clot_index, clot_len_mask);
  // NB ClotLenAdd only supported by Clot0
  RMT_ASSERT((clot_index == 0) || (clot_len_add == 0));
  set_clot_len_add(index, clot_index, clot_len_add);
  set_clot_offset(index, clot_index, clot_offset);
  set_clot_tag(index, clot_index, clot_tag);
  set_clot_tag_offset_add(index, clot_index, clot_tag_offset_add);
  set_clot_has_csum(index, clot_index, clot_has_csum);
}

// CLOT helper funcs to extract checksums from ChecksumEngines etc
bool Parser::clot_validate_checksum_engines(int index) {
  bool ok = true;

  // Record CLOT engines that still have incomplete checksums
  uint16_t which_engines = 0;
  int      n_incomplete_clot_checksums = 0;
  for (int eng_index = 0; eng_index < kChecksumEngines; eng_index++) {

    // Here we check ALL ChecksumEngines even those NOT enabled this cycle.)
    //
    ChecksumEngine *cksmeng = get_checksum_engine(eng_index);
    if (cksmeng->is_clot_type() && cksmeng->started()) {
      // Engine must be 2,3,4 and be CLOT and still be running
      // (fixes spurious warning)
      which_engines |= (1 << eng_index);
      n_incomplete_clot_checksums++;
    }
  }
  if (n_incomplete_clot_checksums > 1) {
    RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserClot),
            "Parser::clot_validate_checksum_engines(index=%d) "
            "Too many incomplete CLOT checksums(%d) (engines=0x%02x)\n",
            index, n_incomplete_clot_checksums, which_engines);
    ok = false;
  }
  return ok;
}
bool Parser::clot_find_checksum(int index, uint8_t tag, uint16_t *csum) {
  RMT_ASSERT(csum != NULL);
  for (int eng_index = 0; eng_index < kChecksumEngines; eng_index++) {

    // Here we only look for tag in ENABLED ChecksumEngines
    //
    uint8_t pcksm_index = checksum_ram_addr(index, eng_index);
    if ((checksum_enable(index, eng_index)) && (pcksm_index < kChecksumEntries)) {
      ChecksumEngine *cksmeng = get_checksum_engine(eng_index);
      uint8_t  eng_tag = ClotEntry::kBadTag;
      uint16_t eng_csum = 0;
      if (cksmeng->get_clot_tag_checksum(pcksm_index, &eng_tag, &eng_csum)) {
        // Must have CLOT engine that has finished to get here.
        // Furthermore MUST have finished this cycle
        // as get_clot_tag_checksum also checks pcksm_final() (HDR_END flag)
        if (!Clot::is_valid_tag(eng_tag)) {
          RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugParserClot),
                  "Parser::clot_find_checksum(index=%d,engine=%d,ramIndex=%d) "
                  "Bad ChecksumEngine CLOT tag %d!\n",
                  index, eng_index, pcksm_index, eng_tag);
        }
        if (tag == eng_tag) {
          RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserClot),
                  "Parser::clot_find_checksum(index=%d,engine=%d,ramIndex=%d) "
                  "Checksum found for CLOT tag %d = 0x%04x\n",
                  index, eng_index, pcksm_index, eng_tag, eng_csum);
          *csum = eng_csum;
          return true; // Use *first* one we find
        }
      }
    }
  }
  return false;
}
// Clot print added - improvement XXX
void Parser::clot_print(Clot *clot, int i, const char *argstr) {
  if (clot == NULL) return;
  const char *str = (argstr != NULL) ?argstr :"";
  if (i < 0) i = clot->n_tags_set() + i;
  uint8_t  tag;
  uint16_t len, off, csum;
  bool valid = clot->read(i, &tag, &len, &off, &csum);
  if (valid) {
    int  actlen, actoff;
    bool valid2 = clot->read_signed(i, &tag, &actlen, &actoff, &csum);
    RMT_ASSERT(valid2);
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserClot),
            "CLOT[%2d]: Tag=%2d Off=%3d Len=%2d CSum=0x%04x (RealOff=%3d,RealLen=%2d) %s\n",
            i, tag, off, len, csum, actoff, actlen, str);
  }
}
void Parser::clot_print(Packet *packet, int i) {
  Clot *clot = packet->clot();
  if (clot == NULL) return;
  clot_print(clot, i);
}
void Parser::clot_print(Packet *packet) {
  Clot *clot = packet->clot();
  if (clot == NULL) return;
  for (int i = 0; i < Clot::kMaxSimulTags; i++) clot_print(clot, i);
}
bool Parser::clot_write(int index, Packet *packet, int lot_index, int c_index,
                        uint8_t c_tag, int c_len, int c_off, uint16_t c_cksum) {
  bool ok = true;
  Clot *clot = packet->clot();
  if (clot == NULL) {
    // Allocate and install Clot if not done yet
    clot = new Clot();
    packet->set_clot(clot);
    RMT_ASSERT(clot != NULL);
    clot->set_min_max_offset(0, Clot::kMaxOff);
  }
  if (clot->contains(c_tag)) {
    RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserClot),
            "Parser::clot_write(index=%d[%d],clotIndex=%d) "
            "Tag %d reused!\n", index, lot_index, c_index, c_tag);
  }
  if (!clot->set(c_tag, c_len, c_off, c_cksum)) {
    ok = false;
    // Problem - figure out what and complain
    if (!clot->is_valid_tag(c_tag)) {
      RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserClot),
              "Parser::clot_write(index=%d[%d],clotIndex=%d) "
              "Bad ActionRam CLOT tag %d!\n",
              index, lot_index, c_index, c_tag);
    }
    if (!clot->is_valid_length_offset(c_len, c_off)) {
      RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserClot),
              "Parser::clot_write(index=%d[%d],clotIndex=%d) "
              "Bad length(%d) or offset(%d) or length+offset(%d) "
              "(Packet min/max offset %d/%d)\n", index,
              lot_index, c_index, c_len, c_off, c_len+c_off,
              clot->min_offset(), clot->max_offset());
    }
    // XXX: this next check may be relaxed by Parser DV
    if (clot->contains_overlap(c_tag, c_len, c_off)) {
      RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserClot),
              "Parser::clot_write(index=%d[%d],clotIndex=%d) "
              "CLOT has pre-existing entry that overlaps with "
              "length(%d) offset(%d) tag(%d) (Min inter-clot gap is %d)\n",
              index, lot_index, c_index, c_len, c_off, c_tag,
              Clot::kClotMinGap);
    }
    if (clot->is_full()) {
      RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserClot),
              "Parser::clot_write(index=%d[%d],clotIndex=%d) "
              "CLOT is full, no room for tag %d\n",
              index, lot_index, c_index, c_tag);
    }
    if (clot->contains(c_tag) && !Clot::kAllowDuplicate) {
      RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserClot),
              "Parser::clot_write(index=%d[%d],clotIndex=%d) "
              "CLOT already contains tag %d and duplicates disallowed\n",
              index, lot_index, c_index, c_tag);
    }
  } else {
    clot_print(packet, -1); // Print what we just set
  }
  return ok;
}
void Parser::clot_reset() {
  pending_clot_ = false; // Invalidate pending
  pending_index_ = pending_c_index_ = pending_c_len_ = pending_c_off_ = -1;
  pending_c_tag_ = ClotEntry::kBadTag;
}
void Parser::clot_discard(int index, const char *why) {
  if (pending_clot_) {
    RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugParserClot),
            "Parser::clot_discard(index=%d) %s "
            "partial_clot DISCARDED  PrevIndex=%d PrevCIndex=%d "
            "PrevTag=%d PrevLen=%d PrevOffset=%d\n", index, why,
            pending_index_, pending_c_index_, pending_c_tag_,
            pending_c_len_, pending_c_off_);
  }
  clot_reset();
}
void Parser::clot_pend(int index, int c_index, uint8_t c_tag, int c_len, int c_off) {
  if (pending_clot_) clot_discard(index, "New pending clot so previous");

  pending_index_ = index;
  pending_c_index_ = c_index;
  pending_c_len_ = c_len;
  pending_c_off_ = c_off;
  pending_c_tag_ = c_tag;
  pending_clot_ = true;
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserClot),
          "Parser::clot_pend(index=%d,clotIndex=%d) "
          "partial_clot WRITTEN  Index=%d CIndex=%d\n",
          index, c_index, pending_index_, pending_c_index_);
  // Call clot_print on fake Clot to get consistent output format
  Clot tmp_clot;
  tmp_clot.set(c_tag, c_len, c_off, 0x0000);
  clot_print(&tmp_clot, -1, "[PENDING]");
}
bool Parser::clot_emit(int index, Packet *packet) {
  bool ok = true;
  uint16_t eng_csum = 0;

  // Maybe emit 1 CLOT tuple using cached pending clot info if we find checksum
  if (pending_clot_) {
    if (clot_find_checksum(index, pending_c_tag_, &eng_csum)) {
      // Flush cached LOT info + checksum we just found
      if (!clot_write(index, packet, pending_index_, pending_c_index_,
                      pending_c_tag_, pending_c_len_, pending_c_off_, eng_csum))
        ok = false;
      // Checksum found - reset CLOT - invalidates pending
      clot_reset();
    } else {
      // XXX: clot continues to pend till *any* later cycle - so comment out below
      // clot_discard(index, "No checksum found so"); // Discard if no csum next cycle
    }
  }
  // Emit up to 2 more CLOT tuples maybe using checksums from CLOT ChecksumEngines
  for (int c_index = 0; ((c_index == 0) || (c_index == 1)); c_index++) {
    bool     c_enabled = false;
    uint8_t  c_type = clot_type(index, c_index);
    uint16_t c_len = 0;
    if ((c_type == 0) && ((clot_en_len_shr(index, c_index) & 1) != 0)) {
      // XXX - ActionRam ClotLenSrc value is ClotLen-1
      c_len = clot_len_src(index, c_index);
      c_len += 1;
      c_enabled = true;
    } else if (c_type == 1) {
      // XXX - Packet ClotLenSrc value is actual ClotLen (NOT ClotLen-1)
      inbuf_get(clot_len_src(index, c_index), &c_len, true, true, 0);
      c_len >>= clot_en_len_shr(index, c_index);
      // XXX - Apply configurable mask/add to Packet ClotLen
      // (NB JBay only supports ADD for Clot0. Clot1 always has ADD of 0)
      c_len &= static_cast<uint16_t>(clot_len_mask(index, c_index));
      c_len += clot_len_add(index, c_index);
      // Clot can store 64 so only reduce mod 64 if >64
      if (c_len > clot_length_max) c_len %= clot_length_max;
      c_enabled = true;
    }
    if (c_enabled) {
      uint16_t c_cksum = 0;
      uint8_t  c_tag = clot_tag(index, c_index);
      int      c_off = clot_offset(index, c_index) + inbuf_getmaxpos() - hdr_len_adj();

      if (clot_tag_offset_add(index, c_index)) {
        // XXX - Might increment tag using dst_offset counter
        // in a similar way to which we increment PHV container.
        c_tag = (c_tag + offset_adjust()) & clot_tag_mask;
      }

      bool ok_cksum = true; // Assume we don't need checksum
      if (clot_has_csum(index, c_index))
        ok_cksum = clot_find_checksum(index, c_tag, &c_cksum); // Check if we have one

      if (ok_cksum) {
        // No checksum needed or we found one this cycle.
        // Write clot immediately so lot_index == index
        if (!clot_write(index, packet, index, c_index, c_tag, c_len, c_off, c_cksum))
          ok = false;
      } else {
        // Checksum needed but none found - pend clot info till later cycle
        clot_pend(index, c_index, c_tag, c_len, c_off);
      }
    }
  }
  return ok;
}
// Outer wrapper function to handle CLOT processing - called unconditionally
int Parser::clot_handle(int index, Packet *packet) {
  int err = 0;
  // Validate ChecksumEngines running ok
  if (!clot_validate_checksum_engines(index)) err |= kErrCsumConfig;
  // Now emit any CLOTs possibly including checksum from above
  if (!clot_emit(index, packet)) err |= kErrCsumConfig;
  if ((err != 0) && !kRelaxExtractionCheck) { THROW_ERROR(-2); }
  return err;
}


void Parser::subscribe(Parser *other_parser) {
  RMT_ASSERT((other_parser != NULL) && (other_parser != this));
  int emptyPos = -1;
  for (int i = 0; i < kMaxSubscribers; i++) {
    if  (subscribers_[i] == other_parser) return;
    if ((subscribers_[i] == NULL) && (emptyPos < 0)) emptyPos = i;
  }
  RMT_ASSERT(emptyPos >= 0);
  subscribers_[emptyPos] = other_parser;
}
void Parser::unsubscribe(Parser *other_parser) {
  RMT_ASSERT((other_parser != NULL) && (other_parser != this));
  for (int i = 0; i < kMaxSubscribers; i++) {
    if  (subscribers_[i] == other_parser) subscribers_[i] = NULL;
  }
}
int Parser::notify_subscribers(uint8_t src_mem_type, uint8_t src_mem_inst, uint32_t src_mem_index) {
  int notifies = 0;
  for (int i = 0; i < kMaxSubscribers; i++) {
    if (subscribers_[i] != NULL) {
      if (subscribers_[i]->notify_memory_change(this, src_mem_type, src_mem_inst, src_mem_index)) {
        notifies++;
      }
    }
  }
  return notifies;
}


void Parser::notify_memory_change_tcam(Parser *src_parser, uint8_t src_mem_inst, uint32_t i) {
  RMT_ASSERT((src_parser != NULL) && (src_mem_inst == 0));
  memory_classes::PrsrMlTcamRowArrayMutable *src = src_parser->get_prs_word0s_word1s();
  memory_classes::PrsrMlTcamRowArrayMutable *dst = get_prs_word0s_word1s();
  // Copy data at index i in src_parser TCAM into this parser TCAM
  uint64_t data0 = UINT64_C(0), data1 = UINT64_C(0), T = UINT64_C(0);
  src->read(i, &data0, &data1, T);
  dst->write(i, data0, data1, T);
  // Now also update S/W TCAM entry
  prs_tcam_change(i);
}
void Parser::notify_memory_change_early_action(Parser *src_parser, uint8_t src_mem_inst, uint32_t i) {
  RMT_ASSERT((src_parser != NULL) && (src_mem_inst == 0));
  memory_classes::PrsrMlEaRowArrayMutable *src = src_parser->get_prs_ea_ram();
  memory_classes::PrsrMlEaRowArrayMutable *dst = get_prs_ea_ram();
  // Copy data at index i in src_parser earlyActionRAM into this parser earlyActionRAM
  uint64_t data0 = UINT64_C(0), data1 = UINT64_C(0), T = UINT64_C(0);
  src->read(i, &data0, &data1, T);
  dst->write(i, data0, data1, T);
}
void Parser::notify_memory_change_action(Parser *src_parser, uint8_t src_mem_inst, uint32_t i) {
  RMT_ASSERT((src_parser != NULL) && (src_mem_inst == 0));
  memory_classes::PrsrPoActionRowArrayMutable *src = src_parser->get_prs_act_ram();
  memory_classes::PrsrPoActionRowArrayMutable *dst = get_prs_act_ram();
  // Copy data at index i in src_parser actionRAM into this parser actionRAM
  for (int j = 0; j < 4; j++) {
    uint64_t data0 = UINT64_C(0), data1 = UINT64_C(0), T = UINT64_C(0);
    src->read((i*4) + j, &data0, &data1, T);
    dst->write((i*4) + j, data0, data1, T);
  }
}
void Parser::notify_memory_change_counter(Parser *src_parser, uint8_t src_mem_inst, uint32_t i) {
  RMT_ASSERT((src_parser != NULL) && (src_mem_inst == 0));
  memory_classes::PrsrMlCtrInitRamMArrayMutable *src = src_parser->get_prs_ctr_init_ram();
  memory_classes::PrsrMlCtrInitRamMArrayMutable *dst = get_prs_ctr_init_ram();
  // Copy data at index i in src_parser counterRAM into this parser counterRAM
  uint64_t data0 = UINT64_C(0), data1 = UINT64_C(0), T = UINT64_C(0);
  src->read(i, &data0, &data1, T);
  dst->write(i, data0, data1, T);
}
void Parser::notify_memory_change_checksum(Parser *src_parser, uint8_t src_mem_inst, uint32_t i) {
  ChecksumEngine *src = src_parser->get_checksum_engine(src_mem_inst);
  ChecksumEngine *dst = get_checksum_engine(src_mem_inst);
  RMT_ASSERT((src != NULL) && (dst != NULL));
  // Hand off processing to relevant ChecksumEngine
  dst->notify_memory_change_checksum(src, src_mem_inst, i);
}
void Parser::notify_memory_change_sw_tcam(Parser *src_parser, uint8_t src_mem_inst, uint32_t i) {
  RMT_ASSERT((src_parser != NULL) && (src_mem_inst == 0));
  // Copy data at index i in src_parser S/W TCAM into this parser S/W TCAM
  BitVector<44> word0(UINT64_C(0)), word1(UINT64_C(0));
  src_parser->get_tcam_word0_word1(i, &word0, &word1);
  set_tcam_word0_word1(i, word0, word1);
}


bool Parser::notify_memory_change(Parser *src_parser,
                                  uint8_t src_mem_type, uint8_t src_mem_inst, uint32_t src_mem_index) {
  if (src_parser != subscribed_to_) return false;

  // Barf if already handling a memory change
  RMT_ASSERT((memory_change_ignore_[src_mem_index] & src_mem_type) == 0);
  // Start ignoring memory changes for memory we're about to modify
  memory_change_ignore_[src_mem_index] |= src_mem_type;

  switch (src_mem_type) {
    case ParserMemoryType::kTcam:        notify_memory_change_tcam(src_parser, src_mem_inst, src_mem_index);         break;
    case ParserMemoryType::kEarlyAction: notify_memory_change_early_action(src_parser, src_mem_inst, src_mem_index); break;
    case ParserMemoryType::kAction:      notify_memory_change_action(src_parser, src_mem_inst, src_mem_index);       break;
    case ParserMemoryType::kCounter:     notify_memory_change_counter(src_parser, src_mem_inst, src_mem_index);      break;
    case ParserMemoryType::kChecksum:    notify_memory_change_checksum(src_parser, src_mem_inst, src_mem_index);     break;
    case ParserMemoryType::kSwTcam:      notify_memory_change_sw_tcam(src_parser, src_mem_inst, src_mem_index);      break;
    default: RMT_ASSERT(0);
  }
  // Stop ignoring memory changes for memory we just modified
  memory_change_ignore_[src_mem_index] &= ~src_mem_type;
  return true;
}

void Parser::inc_tx_count(int ci) {
  ParserShared::inc_tx_count(ci);
  // jbay specific counters
  prs_main_pkt_tx_cnt_.cnt(ci, 1 + prs_main_pkt_tx_cnt_.cnt(ci));
  if (!ingress()) {
    auto *epb_counters = get_object_manager()->epb_counters_lookup(pipe_index(),
                                                                   epb_index(ci),
                                                                   epb_chan(ci));
    if (nullptr != epb_counters) epb_counters->increment_chnl_parser_send_pkt();
  }
}

void Parser::inc_drop_count(int ci) {
  ParserShared::inc_drop_count(ci);
  // jbay specific counters
  if (!ingress()) {
    auto *epb_counters = get_object_manager()->epb_counters_lookup(pipe_index(),
                                                                   epb_index(ci),
                                                                   epb_chan(ci));
    if (nullptr != epb_counters) epb_counters->increment_chnl_parser_send_pkt_err();
  }
}

uint8_t Parser::hdr_len_adj() {
  return get_hdr_len_adj()->amt();
}

void Parser::set_hdr_len_adj(uint8_t v) {
  get_hdr_len_adj()->amt(v);
}

void Parser::phv_fill_other_gress(Phv* phv) {
  RMT_ASSERT(RmtPacketCoordinator::kProcessGressesSeparately);
  int gress = ingress() ? 0 : 1;
  register_classes::PrsrRegMainRspecPhvOwnerMutable *r = get_phv_owner();
  for (int i = 0; i < Phv::phv_max_p(); i++) {
    int size, offA, offB, sz8_01;
    Phv::phv_index_to_off16_p(i, &size, &offA, &offB, &sz8_01);
    if (r->owner(offA) != gress || (size == 32 && r->owner(offB) != gress)) {
      phv->set_p(i, 0xffffffff);
    }
  }
}

}
