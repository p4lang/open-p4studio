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

#include <parser.h>

namespace MODEL_CHIP_NAMESPACE {

const char *Parser::kCounterOpStrings[] = {
  "AddOnly", "PopStackAdd", "LoadImmed", "LoadFromCntrRam"
};


void Parser::set_counter_ctr_op4(int pi, uint8_t v) {
  RMT_ASSERT(0 && "op4 counter_ops not available");
}
void Parser::set_counter_ctr_op2(int pi, uint8_t v) {
  RMT_ASSERT( ((v & ~3) == 0) && "Bad op2 counter_op");
  ea_ram_->ctr_op(pi, v);
}
void Parser::set_counter_ctr_op(int pi, uint8_t v) {
  set_counter_ctr_op2(pi, v);
}

bool Parser::counter_stack_push(int pi) {
  return ((ea_ram_->ctr_stack_push(pi) & 1) == 1);
}
void Parser::set_counter_stack_push(int pi, bool tf) {
  ea_ram_->ctr_stack_push(pi, (tf)?1:0);
}

uint8_t Parser::pcnt_add_to_stack(int pi) {
  return true; // JBay always adds to stack
}
void Parser::set_pcnt_add_to_stack(int pi, uint8_t v) {
  // JBay always adds to stack so whinge if v == 0
  RMT_ASSERT( (v != 0) && "jbay::Parser::pcnt_add_to_stack called with v==0" );
}

const char *Parser::counter_ctr_op_str(uint8_t op) {
  return kCounterOpStrings[op & 3];
}
uint32_t Parser::counter_ctr_uops(int pi) {
  uint32_t uops = 0;
  switch (counter_ctr_op(pi)) {
    case kCounter2LoadFromCntrRam:
      uops = kCtrUopLoadSearchByte | kCtrUopCntrRamLateRotMask |
          kCtrUopApplyCntrRam |
          kCtrUopIncrImm0 | kCtrUopIncrStackAddrBug | kCtrUopClearZeroNeg;
      uops |= (counter_stack_push(pi)) ?kCtrUopPush :kCtrUopZeroise0;
      break;
    case kCounter2LoadImmediate:
      uops = kCtrUopLoadImm | kCtrUopIncrImm0 | kCtrUopIncrStackImm | kCtrUopSetZeroNeg;
      uops |= (counter_stack_push(pi)) ?kCtrUopPush :kCtrUopZeroise0;
      break;
    case kCounter2PopStackAdd:
      uops = kCtrUopPop | kCtrUopLoadImm |
          kCtrUopIncrImm0 | kCtrUopIncrStackImm | kCtrUopSetZeroNeg;
      break;
    case kCounter2AddOnly:
      uops = kCtrUopLoadImm | kCtrUopIncrImm0 | kCtrUopIncrStackImm | kCtrUopSetZeroNeg;
      break;
    default: RMT_ASSERT(0 && "jbay::Parser::counter_ctr_uops Bad UOP"); break;
  }
  return uops;
}

uint8_t Parser::action_ram_en(int pi) {
  return 3; // All banks enabled on JBay
}
void Parser::set_action_ram_en(int pi, uint8_t en) {
  // Silently ignore call on JBay
}

bool Parser::extract16_add_off(int pi, int i) {
  RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
  return ((act_ram_->phv_offset_add_dst(pi, i) & 0x1) == 0x1);
}
uint8_t Parser::extract16_src(int pi, int i, uint8_t mask) {
  RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
  return act_ram_->phv_src(pi, i) & mask;
}
uint8_t Parser::extract16_src_msb(int pi, int i, int bitpos) {
  RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
  return (act_ram_->phv_src(pi,i ) >> bitpos) & 0x1;
}
uint16_t Parser::extract16_dst_phv_raw(int pi, int i) {
  RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
  return act_ram_->phv_dst(pi, i);
}
void Parser::set_extract16_add_off(int pi, int i, bool tf) {
  RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
  act_ram_->phv_offset_add_dst(pi, i, tf ?0x1 :0x0);
}
void Parser::set_extract16_src(int pi, int i, uint8_t v) {
  RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
  act_ram_->phv_src(pi, i, v);
}
void Parser::set_extract16_dst_phv_raw(int pi, int i, uint16_t v) {
  RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
  act_ram_->phv_dst(pi, i, v);
}

uint8_t Parser::extract16_type(int pi, int i) {
  return act_ram_->extract_type(pi, i);
}

void Parser::set_extract16_type(int pi, int i, uint8_t extract_type) {
  RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
  switch (extract_type) {
    case ParserShared::kExtractTypeNone:
    case ParserShared::kExtractType8bToLSB:
    case ParserShared::kExtractType8bToMSB:
    case ParserShared::kExtractType16b:
      act_ram_->extract_type(pi, i, extract_type); break;
    default: RMT_ASSERT(0);
  }
}

void Parser::set_extract16_type_cnt(int pi, uint8_t cnt_16, uint8_t cnt_8_hi, uint8_t cnt_8_lo) {
  (void)pi;
  (void)cnt_16;
  (void)cnt_8_hi;
  (void)cnt_8_lo;
  RMT_ASSERT(false);
  // not implemented for jbay which uses explicit per-extractor types set using
  // set_extract16_type
}

void Parser::update_extract16_type_cnt(int pi, uint8_t extract_type, uint8_t val) {
  (void)pi;
  (void)extract_type;
  (void)val;
  // not implemented for jbay which uses explicit per-extractor types set using
  // set_extract16_type
}


// Funcs to handle immediate constants
uint16_t Parser::val_const(int pi, int i) {
  return act_ram_->val_const(pi,i);
}
bool Parser::val_const_rot(int pi, int i) {
  return ((act_ram_->val_const_rot(pi,i) & 1) == 1);
}
bool Parser::val_const_32b_bond(int pi) {
  return ((act_ram_->val_const_32b_bond(pi) & 1) == 1);
}
void Parser::set_val_const(int pi, int i, uint16_t v) {
  act_ram_->val_const(pi,i,v);
}
void Parser::set_val_const_rot(int pi, int i, bool tf) {
  act_ram_->val_const_rot(pi,i,tf?1:0);
}
void Parser::set_val_const_32b_bond(int pi, bool tf) {
  act_ram_->val_const_32b_bond(pi,tf?1:0);
}

uint32_t Parser::extract_immediate_consts(int pi, int i, int rot, uint32_t dflt) {
  RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
  // JBay: Only extractors 0-9 can use immediate constants - higher extractors
  // simply return 'dflt' input param (typically parser version) as an
  // immediate constant; WIP: all extractors can use immediate constants
  if (i >= RmtDefs::kParserNumImmConstExtractors) {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserExtract),
        "Parser::extract_immediate_consts(%d) extractor %d cannot use "
        "immediate constant, returning default val=%08x\n",
        pi, i, dflt);
    return dflt;
  }
  uint16_t v0 = act_ram_->val_const(pi, 0);
  uint16_t v1 = act_ram_->val_const(pi, 1);
  bool rot0 = (act_ram_->val_const_rot(pi, 0) == 1);
  bool rot1 = (act_ram_->val_const_rot(pi, 1) == 1);
  if (act_ram_->val_const_32b_bond(pi) == 0) {
    if (rot0) v0 = model_common::Util::rotr16(v0,rot);
    if (rot1) v1 = model_common::Util::rotr16(v1,rot);
    return (static_cast<uint32_t>(v1) << 16) | (static_cast<uint32_t>(v0) << 0);
  } else {
    uint32_t v32 = (static_cast<uint32_t>(v1) << 16) | (static_cast<uint32_t>(v0) << 0);
    // In bonded case rot0 only determines if rotr happens (7Feb2017)
    if (rot0) v32 = model_common::Util::rotr32(v32,rot);
    return v32;
  }
}

// Funcs to handle disable_partial_hdr_err - added in XXX
bool Parser:: disable_partial_hdr_err(int pi) {
  return act_ram_->disable_partial_hdr_err(pi);
}
void Parser::set_disable_partial_hdr_err(int pi, bool tf) {
  act_ram_->disable_partial_hdr_err(pi,tf?1:0);
}

// These offset/checksum/pri_upd funcs now need to be per-chip because of WIP ActionRAM reorganisation
bool Parser::offset_reset(int pi) {
  return ((act_ram_->dst_offset_rst(pi) & 0x1) == 0x1);
}
uint8_t Parser::offset_incr_val(int pi) {
  return act_ram_->dst_offset_inc(pi);
}
bool    Parser::checksum_enable(int pi, int i) {
  return ((act_ram_->csum_en(pi,i) & 0x1) == 0x1);
}
uint8_t Parser::checksum_ram_addr(int pi, int i) {
  return act_ram_->csum_addr(pi,i);
}
uint8_t Parser::pri_upd_type(int pi) {
  return (act_ram_->pri_upd_type(pi) & 0x1);
}
uint8_t Parser::pri_upd_src(int pi) {
  return (act_ram_->pri_upd_src(pi) & 0x1F);
}
uint8_t Parser::pri_upd_en_shr(int pi) {
  return (act_ram_->pri_upd_en_shr(pi) & 0xF);
}
uint8_t Parser::pri_upd_val_mask(int pi) {
  return (act_ram_->pri_upd_val_mask(pi) & 0x7);
}

void Parser::set_offset_reset(int pi, bool tf) {
  act_ram_->dst_offset_rst(pi,tf ?0x1 :0x0);
}
void Parser::set_offset_incr_val(int pi, uint8_t v) {
  act_ram_->dst_offset_inc(pi,v);
}
void Parser::set_checksum_enable(int pi, int i, bool tf) {
  act_ram_->csum_en(pi,i,tf ?0x1 :0x0);
}
void Parser::set_checksum_ram_addr(int pi, int i, uint8_t v) {
  act_ram_->csum_addr(pi,i,v);
}
void Parser::set_pri_upd_type(int pi, uint8_t v) {
  act_ram_->pri_upd_type(pi,v);
}
void Parser::set_pri_upd_src(int pi, uint8_t v) {
  act_ram_->pri_upd_src(pi,v);
}
void Parser::set_pri_upd_en_shr(int pi, uint8_t v) {
  act_ram_->pri_upd_en_shr(pi,v);
}
void Parser::set_pri_upd_val_mask(int pi, uint8_t v) {
  act_ram_->pri_upd_val_mask(pi,v);
}


// Funcs to allow access to Version Update fields in ActionRAM
uint8_t Parser::ver_upd_type(int pi) {
  return (act_ram_->ver_upd_type(pi) & 0x1);
}
uint8_t Parser::ver_upd_src(int pi) {
  return (act_ram_->ver_upd_src(pi) & 0x1F);
}
uint8_t Parser::ver_upd_en_shr(int pi) {
  return (act_ram_->ver_upd_en_shr(pi) & 0xF);
}
uint8_t Parser::ver_upd_val_mask(int pi) {
  return (act_ram_->ver_upd_val_mask(pi) & 0x3);
}
void Parser::set_ver_upd_type(int pi, uint8_t v) {
  act_ram_->ver_upd_type(pi,v);
}
void Parser::set_ver_upd_src(int pi, uint8_t v) {
  act_ram_->ver_upd_src(pi,v);
}
void Parser::set_ver_upd_en_shr(int pi, uint8_t v) {
  act_ram_->ver_upd_en_shr(pi,v);
}
void Parser::set_ver_upd_val_mask(int pi, uint8_t v) {
  act_ram_->ver_upd_val_mask(pi,v);
}


// JUST USED BY UNIT TEST LOGIC
// Set ParseAction vals for some action index
// Shim function to allow use of a consistent set_action across ALL chips
//
// On JBay we map 8b/32b extracts to use 16b extractors
// We ONLY examine first 2 checksum params and first 4 extract params
// We leave first 4 (4 by default, n_disabled in general) extractors for use by
// checksumming so:
// - extractors 4  5  6  7 are used for 8b extracts
// - extractors 8  9 10 11 are used for 16b extracts
// - extractors 12/13 14/15 16/17 18/19 used for 32b extracts
//
// Can only cope with 32b worth of immediate data. As it stands
// unit-tests only ever use 4x8b immediate data so this is OK.
// Check if any 16b/32b immediate data requested and if so complain.
//
void Parser::set_action(int index,
                        bool _offset_reset, uint8_t _offset_incr_val,
                        bool _checksum_enable[], uint8_t _checksum_ram_addr[],
                        bool _extract8_src_imm_val[], bool _extract8_add_off[],
                        uint8_t _extract8_src[],  uint16_t _extract8_dst_phv[],
                        bool _extract16_src_imm_val[], bool _extract16_add_off[],
                        uint8_t _extract16_src[], uint16_t _extract16_dst_phv[],
                        bool _extract32_src_imm_val[], bool _extract32_add_off[],
                        uint8_t _extract32_src[], uint16_t _extract32_dst_phv[],
                        int _dst_phv_format, int n_disabled) {

  set_offset_reset(index, _offset_reset);
  set_offset_incr_val(index, _offset_incr_val);
  for (int i = 0; i < 2; i++) {
    set_checksum_enable(index, i, _checksum_enable[i]);
    set_checksum_ram_addr(index, i, _checksum_ram_addr[i]);
  }

  // Disable first n_disabled 16b extractors - reserved for checksumming
  int extractor = 0;
  for (int i = 0; i < n_disabled; i++) {
    if (extractor >= RmtDefs::kParserExtract16bWordsPerCycle) {
      RMT_ASSERT(0 && "max number of extractors exceeded");
    }
    set_extract16_dst_phv_by_phv(index, extractor, k_phv::kBadPhv);
    set_extract16_src_imm_val(index, extractor, false);
    set_extract16_add_off(index, extractor, false);
    set_extract16_src(index, extractor, 0);
    extractor++;
  }

  // Allow immediate value for 8b extracts
  uint32_t val_const = 0u;
  for (int i = 0; i < 4; i++) {
    if (extractor >= RmtDefs::kParserExtract16bWordsPerCycle) {
      if  (_extract8_dst_phv[i] == k_phv::kBadPhv) continue;
      RMT_ASSERT(0 && "max number of extractors exceeded");
    }
    if (_extract8_src_imm_val[i]) {
      // Replace with reference to appropriate byte in inbuf2
      val_const |= (_extract8_src[i] << ((3-i)*8));
      set_extract16_src(index, extractor, 60+i);
    } else {
      // This func assumes correct byte offsets specified initially
      // but will fix them up if their alignment 'doesn't work'
      // See later comment below
      set_extract16_src(index, extractor, _extract8_src[i]);
    }
    set_extract16_add_off(index, extractor, _extract8_add_off[i]);
    int phv = map_dst_phv(_extract8_dst_phv[i], _dst_phv_format);
    set_extract16_dst_phv_by_phv(index, extractor, phv);
    extractor++;
  }
  set_val_const(index, 0, static_cast<uint16_t>(val_const & 0xFFFF));
  set_val_const(index, 1, static_cast<uint16_t>(val_const >> 16));

  // Simple map for 16b extracts - but no immediate value allowed
  for (int i = 0; i < 4; i++) {
    if (extractor >= RmtDefs::kParserExtract16bWordsPerCycle) {
      if  (_extract16_dst_phv[i] == k_phv::kBadPhv) continue;
      RMT_ASSERT(0 && "max number of extractors exceeded");
    }
    RMT_ASSERT(!_extract16_src_imm_val[i]);
    set_extract16_src_imm_val(index, extractor, false);
    set_extract16_add_off(index, extractor, _extract16_add_off[i]);
    set_extract16_src(index, extractor, _extract16_src[i]);
    int phv = map_dst_phv(_extract16_dst_phv[i], _dst_phv_format);
    set_extract16_dst_phv_by_phv(index, extractor, phv);
    extractor++;
  }

  // Have to double up 16b extractors for 32b extracts - no imm val again
  for (int i = 0; i < 4; i++) {
    if ((extractor+1) >= RmtDefs::kParserExtract16bWordsPerCycle) {
      if  (_extract32_dst_phv[i] == k_phv::kBadPhv) continue;
      RMT_ASSERT(0 && "max number of extractors exceeded");
    }
    RMT_ASSERT(!_extract32_src_imm_val[i]);
    // Here we rely on 2 consecutive 16b extracts being allowed to target same 32b phv
    int phv = map_dst_phv(_extract32_dst_phv[i], _dst_phv_format);
    set_extract16_src_imm_val(index, extractor, false);
    set_extract16_add_off(index, extractor, _extract32_add_off[i]);
    set_extract16_src(index, extractor, _extract32_src[i]);
    set_extract16_dst_phv_by_phv(index, extractor, phv, 1); // First SRC is HI 16b
    extractor++;

    set_extract16_src_imm_val(index, extractor, false);
    set_extract16_add_off(index, extractor, _extract32_add_off[i]);
    set_extract16_src(index, extractor, _extract32_src[i] + 2);
    set_extract16_dst_phv_by_phv(index, extractor, phv, 0); // Second SRC is LO 16b
    extractor++;
  }
}

bool Parser::hdr_len_inc(int pi) {
  (void)pi;
  RMT_ASSERT(0);  // shouldn't be called for jbay
  return false;
}

void Parser::set_hdr_len_inc(int pi, bool tf) {
  (void)pi;
  (void)tf;
  // ignored for jbay
}

uint8_t Parser::hdr_len_inc_final_amt(int pi) {
  // for jbay an inc amount is only applied on the final inc, when
  // hdr_len_inc_stop is true
  return act_ram_->hdr_len_inc_final_amt(pi);
}

void Parser::set_hdr_len_inc_final_amt(int pi, uint8_t val) {
  act_ram_->hdr_len_inc_final_amt(pi, val);
}

bool Parser::hdr_len_inc_stop(int pi) {
  return act_ram_->hdr_len_inc_stop(pi) == 1u;
}

void Parser::set_hdr_len_inc_stop(int pi, bool tf) {
  act_ram_->hdr_len_inc_stop(pi, tf ? 1 : 0);
}

void Parser::update_hdr_len(Packet *p, int index, uint32_t err_flags, uint8_t shift) {
  bool stop = hdr_len_inc_stop(index);
  if (is_hdr_len_inc_stopped()) {
    if (stop) {
      // Warn if we see stop action more than once
      RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugParserParseLoop),
              "Parser::update_hdr_len(index=%d) "
              "Ignored repeated use hdr_len_inc_stop\n",
              index);
    }
    // hdr_len inc has been previously stopped so exit without applying inc
    return;
  }
  if (stop) {
    set_hdr_len_inc_stopped(true);  // will only be set true once
    shift = hdr_len_inc_final_amt(index);
  }
  p->set_orig_hdr_len(p->orig_hdr_len() + shift);
}

void Parser::tm_status_phv_write_lsb(uint32_t tm_status, Phv *phv) {
  (void)tm_status;
  (void)phv;
  // no-op for jbay which does not have phv_sec for tm status LSB
}

bool Parser::partial_hdr_err_proc(int pi) {
  (void)pi;
  return false;
  // no-op for jbay
}
void Parser::set_partial_hdr_err_proc(int pi, bool tf) {
  (void)pi;
  (void)tf;
  // no-op for jbay
}


// Funcs to allow access to CLOT fields in ActionRAM
uint8_t Parser::clot_type(int pi, int ci) {
  return (act_ram_->clot_type(pi,ci) & 1);
}
uint8_t Parser::clot_len_src(int pi, int ci) {
  return (act_ram_->clot_len_src(pi,ci) & clot_length_mask);
}
uint8_t Parser::clot_en_len_shr(int pi, int ci) {
  return (act_ram_->clot_en_len_shr(pi,ci) & clot_shift_mask);
}
uint8_t Parser::clot_len_mask(int pi, int ci) {
  return (act_ram_->clot_len_mask(pi,ci) & clot_length_mask);
}
uint8_t Parser::clot_len_add(int pi, int ci) {
  return (ci == 0) ?(act_ram_->clot_len_add(pi) & clot_length_mask) :0;
}
uint8_t Parser::clot_offset(int pi, int ci) {
  return (act_ram_->clot_offset(pi,ci) & clot_offset_mask);
}
uint8_t Parser::clot_tag(int pi, int ci) {
  return (act_ram_->clot_tag(pi,ci) & clot_tag_mask);
}
bool    Parser::clot_tag_offset_add(int pi, int ci) {
  return ((act_ram_->clot_tag_offset_add(pi,ci) & 1) == 1);
}
uint8_t Parser::clot_has_csum(int pi, int ci) {
  return (act_ram_->clot_has_csum(pi,ci) & 1);
}
void Parser::set_clot_type(int pi, int ci, uint8_t v) {
  act_ram_->clot_type(pi,ci,v & 1);
}
void Parser::set_clot_len_src(int pi, int ci, uint8_t v) {
  act_ram_->clot_len_src(pi,ci,v & clot_length_mask);
}
void Parser::set_clot_en_len_shr(int pi, int ci, uint8_t v) {
  act_ram_->clot_en_len_shr(pi,ci,v & clot_shift_mask);
}
void Parser::set_clot_len_mask(int pi, int ci, uint8_t v) {
  act_ram_->clot_len_mask(pi,ci,v & clot_length_mask);
}
void Parser::set_clot_len_add(int pi, int ci, int v) {
  if (ci == 0) act_ram_->clot_len_add(pi, v & clot_length_mask);
}
void Parser::set_clot_offset(int pi, int ci, uint8_t v) {
  act_ram_->clot_offset(pi,ci,v & clot_offset_mask);
}
void Parser::set_clot_tag(int pi, int ci, uint8_t v) {
  act_ram_->clot_tag(pi,ci,v & clot_tag_mask);
}
void Parser::set_clot_tag_offset_add(int pi, int ci, bool tf) {
  act_ram_->clot_tag_offset_add(pi,ci,tf?1:0);
}
void Parser::set_clot_has_csum(int pi, int ci, uint8_t v) {
  act_ram_->clot_has_csum(pi,ci,v & 1);
}


}
