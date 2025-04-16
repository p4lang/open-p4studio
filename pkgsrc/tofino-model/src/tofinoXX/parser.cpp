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

// PARSER - TofinoXX specific code

#include <parser.h>
#include <rmt-object-manager.h>
#include <rmt-packet-coordinator.h>
#include <epb.h>

namespace MODEL_CHIP_NAMESPACE {

const char *Parser::kCounterOpStrings[] = {
  "AddOnly", "AddOnly", "LoadImmed", "LoadFromCntrRam"
};


Parser::Parser(RmtObjectManager *om, int pipeIndex, int prsIndex, int ioIndex,
               const ParserConfig &config)
    : ParserShared(om, pipeIndex, prsIndex, ioIndex, config), reset_running_(false),
      prs_main_err_phv_cfg_(prsr_reg_adapter(prs_main_err_phv_cfg_, chip_index(), pipeIndex, ioIndex, prsIndex)),
      prs_main_enable_(prsr_reg_adapter(prs_main_enable_, chip_index(), pipeIndex, ioIndex, prsIndex)),
      prs_main_mode_(prsr_reg_adapter(prs_main_mode_, chip_index(), pipeIndex, ioIndex, prsIndex)),

      prs_main_dst_cont_err_cnt_(prsr_reg_adapter(prs_main_dst_cont_err_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
      prs_main_intr_status_(prsr_reg_adapter(prs_main_intr_status_, chip_index(), pipeIndex, ioIndex, prsIndex)),
      // These registers are instantiated but are NOT used - however allows read/write
      prs_main_intr_enable0_(prsr_reg_adapter(prs_main_intr_enable0_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                              [this](){this->not_implemented("prs_main_intr_enable0_");})),
      prs_main_intr_enable1_(prsr_reg_adapter(prs_main_intr_enable1_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                              [this](){this->not_implemented("prs_main_intr_enable1_");})),
      prs_main_intr_freeze_enable_(prsr_reg_adapter(prs_main_intr_freeze_enable_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                    [this](){this->not_implemented("prs_main_intr_freeze_enable_");})),
      prs_main_intr_inject_(prsr_reg_adapter(prs_main_intr_inject_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                             [this](){this->not_implemented("prs_main_intr_inject_");})),
      prs_main_dst_cont_errlog_(prsr_reg_adapter(prs_main_dst_cont_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                [this](){this->not_implemented("prs_main_dst_cont_errlog_");})),
      prs_main_aram_mbe_errlog_(prsr_reg_adapter(prs_main_aram_mbe_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                 [this](){this->not_implemented("prs_main_aram_mbe_errlog_");})),
      prs_main_aram_sbe_errlog_(prsr_reg_adapter(prs_main_aram_sbe_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                 [this](){this->not_implemented("prs_main_aram_sbe_errlog_");})),
      prs_main_ctr_range_errlog_(prsr_reg_adapter(prs_main_ctr_range_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                  [this](){this->not_implemented("prs_main_ctr_range_errlog_");})),
      prs_main_ibuf_oflow_errlog_(prsr_reg_adapter(prs_main_ibuf_oflow_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                   [this](){this->not_implemented("prs_main_ibuf_oflow_errlog_");})),
      prs_main_ibuf_uflow_errlog_(prsr_reg_adapter(prs_main_ibuf_uflow_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                   [this](){this->not_implemented("prs_main_ibuf_uflow_errlog_");})),
      prs_main_multi_wr_errlog_(prsr_reg_adapter(prs_main_multi_wr_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                 [this](){this->not_implemented("prs_main_multi_wr_errlog_");})),
      prs_main_no_tcam_match_errlog_(prsr_reg_adapter(prs_main_no_tcam_match_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                      [this](){this->not_implemented("prs_main_no_tcam_match_errlog_");})),
      prs_main_op_fifo_oflow_errlog_(prsr_reg_adapter(prs_main_op_fifo_oflow_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                      [this](){this->not_implemented("prs_main_op_fifo_oflow_errlog_");})),
      prs_main_op_fifo_uflow_errlog_(prsr_reg_adapter(prs_main_op_fifo_uflow_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                      [this](){this->not_implemented("prs_main_op_fifo_uflow_errlog_");})),
      prs_main_partial_hdr_errlog_(prsr_reg_adapter(prs_main_partial_hdr_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                    [this](){this->not_implemented("prs_main_partial_hdr_errlog_");})),
      prs_main_phv_owner_errlog_(prsr_reg_adapter(prs_main_phv_owner_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                  [this](){this->not_implemented("prs_main_phv_owner_errlog_");})),
      prs_main_src_ext_errlog_(prsr_reg_adapter(prs_main_src_ext_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                [this](){this->not_implemented("prs_main_src_ext_errlog_");})),
      prs_main_tcam_par_errlog_(prsr_reg_adapter(prs_main_tcam_par_errlog_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                                 [this](){this->not_implemented("prs_main_tcam_par_errlog_");})),
      scratch_(prsr_reg_adapter(scratch_, chip_index(), pipeIndex, ioIndex, prsIndex)),
      prs_main_a_emp_thresh_(prsr_reg_adapter(prs_main_a_emp_thresh_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                              [this](){this->not_implemented("prs_main_a_emp_thresh_");})),
      checksum_engine_ { { { om,pipeIndex,ioIndex,prsIndex,0,0,this }, { om,pipeIndex,ioIndex,prsIndex,1,1,this } } },
      prs_tcam_word0s_(prsr_mem_adapter(prs_tcam_word0s_,chip_index(), pipeIndex, ioIndex, prsIndex, get_tcam_w01(0),
                                        [this](uint32_t i){this->memory_change_callback(ParserMemoryType::kTcam,0,i);})),
      prs_tcam_word1s_(prsr_mem_adapter(prs_tcam_word1s_,chip_index(), pipeIndex, ioIndex, prsIndex, get_tcam_w01(1),
                                        [this](uint32_t i){this->memory_change_callback(ParserMemoryType::kTcam,1,i);})),
      ea_ram_(get_prs_ea_ram()), act_ram_(get_prs_act_ram()), ctr_init_ram_(get_prs_ctr_init_ram()),
      xtrac_(om,this,get_prs_act_ram()) {

  static_assert( (kLoadBytes==4),
                 "Tofino Parser logic can only load 4 bytes from buffer");
  reset_this();
}
Parser::~Parser() {
}
void Parser::reset_this() {
  reset_running_ = true;
  prs_main_err_phv_cfg_.reset();
  prs_main_enable_.reset();
  prs_main_mode_.reset();
  prs_main_dst_cont_err_cnt_.reset();
  prs_main_intr_status_.reset();
  prs_main_intr_enable0_.reset();
  prs_main_intr_enable1_.reset();
  prs_main_intr_freeze_enable_.reset();
  prs_main_intr_inject_.reset();
  prs_main_dst_cont_errlog_.reset();
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
  scratch_.reset();
  prs_main_a_emp_thresh_.reset();
  prs_tcam_word0s_.reset();
  prs_tcam_word1s_.reset();
  reset_running_ = false;
}
void Parser::reset() {
  // Call superclass reset and then self reset
  ParserShared::reset();
  reset_this();
}
void Parser::not_implemented(const char *clazz) {
  if (reset_running_) return;
  ParserShared::not_implemented(clazz);
}
void Parser::memory_change_callback(uint8_t mem_type, uint8_t mem_inst, uint32_t mem_index) {
  if (reset_running_) return;
  // Only care about TCAM changes in TofinoXX
  if (mem_type == ParserMemoryType::kTcam) prs_tcam_change(mem_index);
}


// Function to handle initial match byte load (just returns zeros)
void Parser::inbuf0_maybe_get_initial(int chan,
                                      uint8_t *v8_3, uint8_t *v8_2,
                                      uint8_t *v8_1, uint8_t *v8_0) {
  RMT_ASSERT((v8_3 != NULL) && (v8_2 != NULL) && (v8_1 != NULL) && (v8_0 != NULL));
  *v8_3 = *v8_2 = *v8_1 = *v8_0 = 0;
}
// Functions to handle different capabilities of inbuf1 (scratch pad buffer)
void Parser::inbuf1_setup(Packet *pkt) {
  InputBuffer *inbuf1 = get_inbuf(1);
  inbuf1->reset();
  // For buf1 reads at pos 32+x/48+x corresponds to read of byte x
  // so set read_pos_mod to 16
  inbuf1->set_read_pos_mod(16);
  if (ingress()) {
    // In the case of an ingress parser we fill buf1 with 16 bytes from pkt
    // and pad out with zeros to len 17:
    // 1. to allow us to get 2 bytes from pos 47 or 63 (with 2nd byte 0)
    // 2. to handle case where pkt is shorter than 16 bytes
    int sz = inbuf1->fill(pkt, 0, 16);
    inbuf1->fill_zeros(17-sz);
  } else {
    // In the case of an egress parser we fill buf1 with 32 zeros
    inbuf1->fill_zeros(32);
  }
}
void Parser::inbuf1_maybe_fill(int pi, uint8_t v8_3, uint8_t v8_2, uint8_t v8_1, uint8_t v8_0,
                               int state_cnt) {
  // Tofino never dynamically refills inbuf1
}
// Function to handle different capabilities of inbuf2 (TS/Version buffer)
void Parser::inbuf2_setup(Packet *pkt) {
  InputBuffer *inbuf2 = get_inbuf(2);
  inbuf2->reset();

  // in case of egress, take delay computed from queueing.cpp into account
  uint64_t tstamp = timestamp_get();
  if (!ingress()) {
    tstamp += pkt->qing2e_metadata()->delay() - 1;
  }
  // For both ingress/egress parser we fill buf2 exactly with
  // a) 6 bytes of timestamp b) 4 bytes of version
  inbuf2->fill_val(tstamp, 6, false);
  inbuf2->fill_val(version_get(), 4, false);
  // And we set read_pos_mod to 54 - any read offset less than this will
  // cause an error on get() - any read offset >= 64 will also fail
  inbuf2->set_read_pos_mod(kFirstByteInbuf2); // 54
}

void Parser::inbuf2_update_consts(uint32_t v) {
  // Does nothing on TofinoXX
}

const char *Parser::counter_ctr_op_str(uint8_t op) {
  return kCounterOpStrings[op & 3];
}
uint32_t Parser::counter_ctr_uops(int pi) {
  uint32_t uops = 0;
  switch (counter_ctr_op(pi)) {
    case kCounter2LoadFromCntrRam:
      uops = kCtrUopZeroise0 |
          kCtrUopLoadSearchByte | kCtrUopCntrRamLateRotMask |
          kCtrUopApplyCntrRam | kCtrUopIncrImm0 | kCtrUopClearZeroNeg;
      break;
    case kCounter2LoadImmediate:
      uops = kCtrUopZeroise0 | kCtrUopLoadImm | kCtrUopIncrImm0 | kCtrUopSetZeroNeg;
      break;
    case kCounter2PopStackAdd:
      uops = kCtrUopLoadImm | kCtrUopIncrImm0 | kCtrUopSetZeroNeg;
      break;
    case kCounter2AddOnly:
      uops = kCtrUopLoadImm | kCtrUopIncrImm0 | kCtrUopSetZeroNeg;
      break;
    default: RMT_ASSERT(0 && "tofinoXX::Parser::counter_ctr_uops Bad UOP"); break;
  }
  return uops;
}

// These funcs allow the shared Parser code to behave as if it is capable
// of extracting 4 bytes from the data buffer (rather than 2 bytes and 1 short)
//
void Parser::get_lookup_offset(int pi, int n, uint8_t *off, bool *load, int *which_buf) {
  RMT_ASSERT((n >= 0) && (n < kLoadBytes));
  RMT_ASSERT((off != NULL) && (load != NULL) && (which_buf != NULL));
  uint8_t v, ld;
  if ((n == 2) || (n == 3)) {
    v = prs_ea_field8_N_lookup_offset(pi, n-2);
    ld = prs_ea_field8_N_load(pi, n-2);
  } else {
    v = prs_ea_field16_lookup_offset(pi);
    ld = prs_ea_field16_load(pi);
  }
  *load = ((ld & 0x1) == 0x1);
  *which_buf = (v >> kLoadOffsetWidth) & 0x1;
  *off = (v >> 0) & kLoadOffsetMask;
  // Assuming:
  // 1. old offset16 was offset of first/hi byte of 16bit val from pkt buffer
  // 2. offset16 now replaced by offset8[0] and offset8[1]
  // 3. and offset8[1] is offset of MSByte of 16bit val
  // 4. and offset8[0] is offset of LSByte of 16bit val
  // Then, offset8[1] == offset16 and offset8[0] == offset16+1
  if (n == 0) RMT_ASSERT(*off < kLoadOffsetMask);
  if (n == 0) *off = *off + 1;
}
uint8_t Parser::prs_ea_field8_N_lookup_offset(int pi, int n) {
  RMT_ASSERT((n == 0) || (n == 1)); // offset_8[2] and [3] not avail on Tofino
  return ea_ram_->lookup_offset_8(pi, n);
}
uint8_t Parser::prs_ea_field8_N_load(int pi, int n) {
  RMT_ASSERT((n == 0) || (n == 1)); // lookup_8[2] and [3] not avail on Tofino
  return ea_ram_->ld_lookup_8(pi, n);
}
uint8_t Parser::prs_ea_field16_lookup_offset(int pi) {
  return ea_ram_->lookup_offset_16(pi);
}
uint8_t Parser::prs_ea_field16_load(int pi) {
  return ea_ram_->ld_lookup_16(pi);
}

void Parser::set_lookup_offset(int pi, int n, uint8_t off, bool load, int which_buf) {
  RMT_ASSERT((n >= 0) && (n < kLoadBytes));
  RMT_ASSERT((which_buf == 0) || (which_buf == 1));
  if ((n == 2) || (n == 3)) {
    uint8_t v = ((which_buf & 0x1) << kLoadOffsetWidth) | ((off & kLoadOffsetMask) << 0);
    prs_ea_set_field8_N_lookup_offset(pi, n-2, v);
    prs_ea_set_field8_N_load(pi, n-2, (load) ?1 :0);
  } else {
    if (n == 0) RMT_ASSERT(off > 0);
    if (n == 0) off--;
    uint8_t v = ((which_buf & 0x1) << kLoadOffsetWidth) | ((off & kLoadOffsetMask) << 0);
    prs_ea_set_field16_lookup_offset(pi, v);
    prs_ea_set_field16_load(pi, (load) ?1 :0);
  }
}
void Parser::prs_ea_set_field8_N_lookup_offset(int pi, int n, uint8_t v) {
  RMT_ASSERT((n == 0) || (n == 1)); // offset_8[2] and [3] not avail on Tofino
  ea_ram_->lookup_offset_8(pi, n, v);
}
void Parser::prs_ea_set_field8_N_load(int pi, int n, uint8_t v) {
  RMT_ASSERT((n == 0) || (n == 1)); // lookup_8[2] and [3] not avail on Tofino
  ea_ram_->ld_lookup_8(pi, n, v);
}
void Parser::prs_ea_set_field16_lookup_offset(int pi, uint8_t v) {
  return ea_ram_->lookup_offset_16(pi, v);
}
void Parser::prs_ea_set_field16_load(int pi, uint8_t v) {
  ea_ram_->ld_lookup_16(pi, v);
}


// Set ParserEarlyAction vals for some action index
void Parser::set_early_action(int index, uint8_t _counter_load_imm,
                              bool _counter_load_src, bool _counter_load,
                              bool _done, uint8_t _shift_amount,
                              uint8_t _field8_1_lookup_offset, uint8_t _field8_0_lookup_offset,
                              uint8_t _field16_lookup_offset,
                              bool _load_field8_1, bool _load_field8_0, bool _load_field16,
                              uint8_t _next_state_mask, uint8_t _next_state) {
  // Call ParserShared func to initialise common fields
  set_early_action_shared(index, _counter_load_imm, _counter_load_src, _counter_load,
                          _done, _shift_amount, _next_state_mask, _next_state);
  set_field8_1_lookup_offset(index, _field8_1_lookup_offset);
  set_field8_0_lookup_offset(index, _field8_0_lookup_offset);
  set_field16_lookup_offset(index, _field16_lookup_offset);
  set_load_field8_1(index, _load_field8_1);
  set_load_field8_0(index, _load_field8_0);
  set_load_field16(index, _load_field16);
}
// Allow initialisation using 4x byte offsets but ONLY if certain conditions are met
void Parser::set_early_action(int index, uint8_t _counter_load_imm,
                              bool _counter_load_src, bool _counter_load,
                              bool _done, uint8_t _shift_amount,
                              uint8_t _field8_3_lookup_offset, uint8_t _field8_2_lookup_offset,
                              uint8_t _field8_1_lookup_offset, uint8_t _field8_0_lookup_offset,
                              bool _load_field8_3, bool _load_field8_2,
                              bool _load_field8_1, bool _load_field8_0,
                              uint8_t _next_state_mask, uint8_t _next_state) {
  RMT_ASSERT(_field8_0_lookup_offset == _field8_1_lookup_offset+1);
  RMT_ASSERT(_load_field8_0 == _load_field8_1);
  set_early_action(index, _counter_load_imm, _counter_load_src, _counter_load,
                   _field8_3_lookup_offset, _field8_2_lookup_offset,
                   _field8_1_lookup_offset,
                   _load_field8_3, _load_field8_2, _load_field8_1,
                   _done, _shift_amount, _next_state_mask, _next_state);
}



// Get TCAM entry out of regs
BitVector<44> Parser::prs_tcam_get_entry(memory_classes::PrsrMlTcamRowArray &prs, uint32_t i) {
  uint16_t v16 = prs.lookup_16(i);
  uint8_t v8_0 = prs.lookup_8(i,0);
  uint8_t v8_1 = prs.lookup_8(i,1);
  uint8_t state = prs.curr_state(i);
  bool cnt_eq_0 = ((prs.ctr_zero(i) & 0x1) == 0x1);
  bool cnt_lt_0 = ((prs.ctr_neg(i) & 0x1) == 0x1);
  bool ver0 = ((prs.ver_0(i) & 0x1) == 0x1);
  bool ver1 = ((prs.ver_1(i) & 0x1) == 0x1);
  return make_tcam_entry(ver1, ver0, cnt_eq_0, cnt_lt_0, state, v16, v8_1, v8_0);
}
// Handle TCAM change - copy changed row from regs -> s/w
void Parser::prs_tcam_change(uint32_t i) {
  if (reset_running_) return;
  BitVector<44> word0 = prs_tcam_get_entry(prs_tcam_word0s_, i);
  BitVector<44> word1 = prs_tcam_get_entry(prs_tcam_word1s_, i);
  set_tcam_word0_word1(i, word0, word1);
}


// Functions to handle different extraction capabilities
uint16_t Parser::map_dst_phv(uint16_t phv_in, int from_format) {
  if (from_format == model_core::ChipType::kTofino) {
    return phv_in;
  } else {
    RMT_ASSERT(0);
  }
}
bool Parser::extract_ok_phv(int phv_word) {
  if ((phv_word < 0) || (phv_word == k_phv::kBadPhv)) return false;
  return (phv_word < k_phv::kTagalongStart);
}
bool Parser::extract_ok_tphv(int phv_word) {
  if ((phv_word < 0) || (phv_word == k_phv::kBadPhv)) return false;
  return (phv_word >= k_phv::kTagalongStart);
}
int Parser::get_phv_to_write(int phv_word) {
  return (extract_ok_phv(phv_word)) ?phv_word :-1;
}
int Parser::get_tphv_to_write(int phv_word) {
  return (extract_ok_tphv(phv_word)) ?phv_word :-1;
}

// Function to apply further extraction constraint checks
void Parser::check_further_extract_constraints(uint32_t check_flags,
                                               int index, int which_extract,
                                               int phv_word, Phv *phv, int extract_sz,
                                               bool partial_hdr_err) {
  // Figure out for Tofino what extra checks, if any, need to be made
  int phv_sz = Phv::which_width(phv_word);
  if ((extract_sz == 8) && (phv_sz == 16)) {
    // 2x8->lohi16
    check_flags |= kCheckPrevNum8bitExtractsAlways2N;
  } else if ((extract_sz == 16) && (phv_sz == 32)) {
    // 2x16->lohi32
    check_flags |= kCheckPrevNum8bitExtractsAlways4N;
    check_flags |= kCheckPrevNum16bitExtractsAlways2N;
  } else if ((extract_sz == 8) && (phv_sz == 32)) {
    // 4x8->32
    check_flags |= kCheckPrevNum8bitExtractsAlways4N;
    check_flags |= kCheckPrevNum16bitExtractsAlways2N;
  }

  ExtractInfo *x = get_extract_info();
  if (x->n2w_8b_32b_recently_seen_ == true) {
    check_flags |= kCheckPrevNum16bitExtractsAlways2N;
  } else if (x->n2w_16b_32b_recently_seen_ == true) {
    check_flags |= kCheckPrevNum8bitExtractsAlways4N;
  }

  if (check_flags == 0u) return;

  // In the case of Tofino if we're attempting to perform a narrow extraction
  // to a wide PHV we need to check that previously the number 8b 16b extracts
  // have always been 2N (or sometimes 4N)
  // (we rely on common calling func telling us what to check)
  if ((check_flags & kCheckPrevNum8bitExtractsAlways4N) != 0u) {
    if (!x->prev_n_8b_extracts_always_4N_) {
      RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractSoftError),
              "ParserShared::inbuf_extract_%ld_to_%ld(%d,%d) phv=%d "
              "n_8b_extracts has not always been 4N previously (now=%d), n2w_xx_recently_seen=(%d, %d)\n",
              extract_sz, Phv::which_width(phv_word), index, which_extract, phv_word,
              x->n_8b_extracts_, x->n2w_8b_32b_recently_seen_, x->n2w_16b_32b_recently_seen_);
      if (!kRelaxExtractionCheck && !partial_hdr_err) { THROW_ERROR(-2); }
    }
  }
  if ((check_flags & kCheckPrevNum8bitExtractsAlways2N) != 0u) {
    if (!x->prev_n_8b_extracts_always_2N_) {
      RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractSoftError),
              "ParserShared::inbuf_extract_%ld_to_%zd(%d,%d) phv=%d "
              "n_8b_extracts has not always been 2N previously (now=%d), n2w_xx_recently_seen=(%d, %d)\n",
              extract_sz, Phv::which_width(phv_word), index, which_extract, phv_word,
              x->n_8b_extracts_, x->n2w_8b_32b_recently_seen_, x->n2w_16b_32b_recently_seen_);
      if (!kRelaxExtractionCheck && !partial_hdr_err) { THROW_ERROR(-2); }
    }
  }
  if ((check_flags & kCheckPrevNum16bitExtractsAlways2N) != 0u) {
    if (!x->prev_n_16b_extracts_always_2N_) {
      RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractSoftError),
              "ParserShared::inbuf_extract_%ld_to_%zd(%d,%d) phv=%d "
              "n_16b_extracts has not always been 2N previously (now=%d), n2w_xx_recently_seen=(%d, %d)\n",
              extract_sz, Phv::which_width(phv_word), index, which_extract, phv_word,
              x->n_16b_extracts_, x->n2w_8b_32b_recently_seen_, x->n2w_16b_32b_recently_seen_);
      if (!kRelaxExtractionCheck && !partial_hdr_err) { THROW_ERROR(-2); }
    }
  }
}

// Function to apply checksum constraint checks
bool Parser::check_checksum_constraints(int match_index,
                                        int &wrote_8b, int &wrote_16b, int &wrote_32b) {
  bool ok = true;
  if ((wrote_8b > 0) && (extract8_type(match_index, 3) != kExtractType8b)) {
    RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractError),
            "Parser::check_checksum_constraints_8(%d,3) %d checksum write(s) to 8b PHV so "
            "extractor 3 MUST be active (po_action_row[%d].phv_8b_dst_3 must be valid)\n",
            match_index, wrote_8b, match_index);
    ok = false;
  }
  if ((wrote_16b > 0) && (extract16_type(match_index, 3) != kExtractType16b)) {
    RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractError),
            "Parser::check_checksum_constraints_16(%d,3) %d checksum write(s) to 16b PHV so "
            "extractor 3 MUST be active (po_action_row[%d].phv_16b_dst_3 must be valid)\n",
            match_index, wrote_16b, match_index);
    ok = false;
  }
  if ((wrote_32b > 0) && (extract32_type(match_index, 3) != kExtractType32b)) {
    RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractError),
            "Parser::check_checksum_constraints_32(%d,3) %d checksum write(s) to 32b PHV so "
            "extractor 3 MUST be active (po_action_row[%d].phv_32b_dst_3 must be valid)\n",
            match_index, wrote_32b, match_index);
    ok = false;
  }
  if (!ok && !kRelaxExtractionCheck) { THROW_ERROR(-2); }
  return ok;
}

// Function to handle different capabilties wrt Version Update
uint8_t Parser::update_version(int index, uint8_t curr_ver) {
  return curr_ver;
}

// Function to handle different capabilities wrt Priority Mapping
uint8_t Parser::map_priority(int chan, uint8_t curr_pri) {
  return curr_pri & 0x3; // Emulate JBay/WIP 2b output pri
}
// Function to setup identity Priority Mapping - does nothing
void Parser::set_identity_priority_map() {
}

void Parser::inc_tx_count(int ci) {
  ParserShared::inc_tx_count(ci);
  // tofino specific counter
  if (!ingress()) {
    auto *epb_counters = get_object_manager()->epb_counters_lookup(pipe_index(),
                                                                   epb_index(ci),
                                                                   epb_chan(ci));
    if (nullptr != epb_counters) epb_counters->increment_egr_pipe_count();
  }
}

uint8_t Parser::hdr_len_adj() {
  return get_hdr_len_adj()->amt();
}

void Parser::set_hdr_len_adj(uint8_t v) {
  get_hdr_len_adj()->amt(v);
}

void Parser::update_hdr_len(Packet *p, int index, uint32_t err_flags, uint8_t shift) {
  RMT_ASSERT(!is_hdr_len_inc_stopped());
  p->set_orig_hdr_len(p->orig_hdr_len() + shift);
}

void Parser::phv_fill_other_gress(Phv* phv) {
  RMT_ASSERT(RmtPacketCoordinator::kProcessGressesSeparately);
  int gress = ingress() ? 0 : 1;
  register_classes::PrsrRegMainRspecPhvOwnerMutable *r = get_phv_owner();
  for (int i = 0; i < Phv::phv_max_p(); i++) {
    if (r->owner(i) != gress) {
      phv->set_p(i, 0xffffffff);
    }
  }
}


}
