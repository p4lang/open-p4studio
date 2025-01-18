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

#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <packet-pipe-data.h>
#include <parser.h>
#include <register_adapters.h>
#include <ipb.h>
#include <event.h>

namespace MODEL_CHIP_NAMESPACE {

  // INPUT BUFFER

  // Fill input buffer from a packet (starting at packet position ppos)
  // at position determined by input buffer write_bytes_ advancing write_bytes_
  int InputBuffer::fill(Packet *p, int ppos, int nbytes) {
    RMT_ASSERT ((wbytes_ >= rbytes_) && (nbytes >= 0));
    int inbytes1 = 0, inbytes2 = 0;
    int space = kInputBufferSize - (wbytes_ - rbytes_);
    if ((space > 0) && (nbytes > 0)) {
      // Work out where to start and bytes/space available
      int wpos = wbytes_ % kInputBufferSize;
      int pbytes1 = std::min( std::min(space,nbytes), kInputBufferSize - wpos);
      inbytes1 = p->get_buf(&input_buffer_[wpos], ppos, pbytes1);
      space -= inbytes1;
      nbytes -= inbytes1;
      wbytes_ += inbytes1;
      if ((pbytes1 == inbytes1) && (space > 0) && (nbytes > 0)) {
        // More bytes probably available in packet - any space left at start buf
        int rpos = rbytes_ % kInputBufferSize;
        int pbytes2 = std::min( std::min(space,nbytes), rpos);
        inbytes2 = p->get_buf(&input_buffer_[0], ppos + pbytes1, pbytes2);
        wbytes_ += inbytes2;
      }
    }
    return inbytes1 + inbytes2;
  }
  // Fill a single byte in input buffer
  int InputBuffer::fill_byte(uint8_t byte) {
    RMT_ASSERT (wbytes_ >= rbytes_);
    int space = kInputBufferSize - (wbytes_ - rbytes_);
    if (space <= 0) return 0;
    input_buffer_[wbytes_ % kInputBufferSize] = byte;
    wbytes_ += 1;
    return 1;
  }
  // Fill input buffer with nbytes zeros
  int InputBuffer::fill_zeros(int nzeros)  {
    int cnt = 0;
    for (int i = 0; i < nzeros; i++) cnt += fill_byte(0);
    return cnt;
  }
  // Rewind input buffer by nbytes
  void InputBuffer::rewind(int nbytes) {
    RMT_ASSERT (wbytes_ >= nbytes);
    wbytes_ -= nbytes;
  }



  // PARSER Error Strings - XXX
  //
  const char *ParserShared::kErrorStrings[] = {
    "NoTcamMatch", "PartialHdr", "CtrRange", "TimeoutIter",
    "TimeoutCycle", "SrcExt",  "DstCont", "PhvOwner",
    "MultiWr", "AramSbe", "AramMbe", "Fcs",
    "Csum", "IbufOflow", "IbufUflow", "OpFifoOflow",
    "OpFifoUflow", "TcamPar", "NarrowToWide", "E19", "CsumConfig",
    "E21", "E22", "E23", "E24", "E25", "E26", "E27", "E28", "E29", "E30", "E31"
  };


  // PARSER
  //
  ParserShared::ParserShared(RmtObjectManager *om, int pipeIndex, int prsIndex, int ioIndex,
                             const ParserConfig &config)
      : PipeObject(om, pipeIndex, prsIndex, kType),
        // prs_common_rspec_(prs_common_regs) - No longer exists as of regs_7217_parser-split
        prs_merge_reg_(NULL),
        prs_main_hdr_len_adj_(prsr_reg_adapter(prs_main_hdr_len_adj_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        //prs_main_intr_(prsr_reg_adapter(prs_main_intr_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_max_iter_(prsr_reg_adapter(prs_main_max_iter_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_no_multi_wr_(prsr_reg_adapter(prs_main_no_multi_wr_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_phv_owner_(prsr_reg_adapter(prs_main_phv_owner_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                              [this](){this->phv_owner_change_callback();})),
        prs_main_pri_start_(prsr_reg_adapter(prs_main_pri_start_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_pri_thresh_(prsr_reg_adapter(prs_main_pri_thresh_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_start_state_(prsr_reg_adapter(prs_main_start_state_, chip_index(), pipeIndex, ioIndex, prsIndex)),

        prs_main_hdr_byte_cnt_(prsr_reg_adapter(prs_main_hdr_byte_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_idle_cnt_(prsr_reg_adapter(prs_main_idle_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_pkt_drop_cnt_(prsr_reg_adapter(prs_main_pkt_drop_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),

        prs_main_aram_mbe_cnt_(prsr_reg_adapter(prs_main_aram_mbe_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_aram_sbe_cnt_(prsr_reg_adapter(prs_main_aram_sbe_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_csum_err_cnt_(prsr_reg_adapter(prs_main_csum_err_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_ctr_range_err_cnt_(prsr_reg_adapter(prs_main_ctr_range_err_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_fcs_err_cnt_(prsr_reg_adapter(prs_main_fcs_err_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_multi_wr_err_cnt_(prsr_reg_adapter(prs_main_multi_wr_err_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_no_tcam_match_err_cnt_(prsr_reg_adapter(prs_main_no_tcam_match_err_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_op_fifo_full_cnt_(prsr_reg_adapter(prs_main_op_fifo_full_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_op_fifo_full_stall_cnt_(prsr_reg_adapter(prs_main_op_fifo_full_stall_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_partial_hdr_err_cnt_(prsr_reg_adapter(prs_main_partial_hdr_err_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_phv_owner_err_cnt_(prsr_reg_adapter(prs_main_phv_owner_err_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_src_ext_err_cnt_(prsr_reg_adapter(prs_main_src_ext_err_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_tcam_par_err_cnt_(prsr_reg_adapter(prs_main_tcam_par_err_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_timeout_cycle_err_cnt_(prsr_reg_adapter(prs_main_timeout_cycle_err_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),
        prs_main_timeout_iter_err_cnt_(prsr_reg_adapter(prs_main_timeout_iter_err_cnt_, chip_index(), pipeIndex, ioIndex, prsIndex)),

        // These registers are instantiated but are NOT used - however allows read/write
        prs_main_debug_ctrl_(prsr_reg_adapter(prs_main_debug_ctrl_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                              [this](){this->not_implemented("prs_main_debug_ctrl_");})),
        prs_main_ecc_(prsr_reg_adapter(prs_main_ecc_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                       [this](){this->not_implemented("prs_main_ecc_");})),
        prs_main_max_cycle_(prsr_reg_adapter(prs_main_max_cycle_, chip_index(), pipeIndex, ioIndex, prsIndex,
                                             [this](){this->not_implemented("prs_main_max_cycle_");})),

        prs_action_ram_(prsr_mem_adapter(prs_action_ram_,chip_index(), pipeIndex, ioIndex, prsIndex,
                                         [this](uint32_t i){this->memory_change_callback(ParserMemoryType::kAction,0,i);})),
        prs_ctr_init_ram_(prsr_mem_adapter(prs_ctr_init_ram_,chip_index(), pipeIndex, ioIndex, prsIndex,
                                           [this](uint32_t i){this->memory_change_callback(ParserMemoryType::kCounter,0,i);})),
        prs_ea_ram_(prsr_mem_adapter(prs_ea_ram_,chip_index(), pipeIndex, ioIndex, prsIndex,
                                     [this](uint32_t i){this->memory_change_callback(ParserMemoryType::kEarlyAction,0,i);})),

        tcam_match_(this),
        pipe_index_(pipeIndex), parser_index_(prsIndex), io_index_(ioIndex),
        last_err_flags_(0), extract_info_(), errstr_buf_(kErrStrBufSize) {

    static_assert((kChecksumEngines == kChecksumRams),
                  "Expect 1-1 mapping between checksum ENGINEs and RAMs");

    // Now call reset properly
    reset_this();
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCreate),
            "ParserShared::create<%d,%d> = %p\n", pipeIndex, prsIndex, this);
  }
  ParserShared::~ParserShared() {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDelete),
            "ParserShared::delete<%d,%d> %p\n", pipe_index_, parser_index_, this);
  }
  void ParserShared::reset_this() {
    // Reset parser
    reset_running_ = true; // Switch off callback handling
    //prs_common_rspec_->reset(); // No longer exists as of regs_7217_parser-split

    //TODO_RM: prs_main_err_phv_cfg_.reset();
    prs_main_hdr_len_adj_.reset();
    //prs_main_intr_.reset();
    prs_main_max_iter_.reset();
    prs_main_no_multi_wr_.reset();
    prs_main_phv_owner_.reset();
    prs_main_pri_start_.reset();
    prs_main_pri_thresh_.reset();
    prs_main_start_state_.reset();

    prs_main_hdr_byte_cnt_.reset();
    prs_main_idle_cnt_.reset();
    prs_main_pkt_drop_cnt_.reset();

    prs_main_aram_mbe_cnt_.reset();
    prs_main_aram_sbe_cnt_.reset();
    prs_main_csum_err_cnt_.reset();
    prs_main_ctr_range_err_cnt_.reset();
    prs_main_fcs_err_cnt_.reset();
    prs_main_multi_wr_err_cnt_.reset();
    prs_main_no_tcam_match_err_cnt_.reset();
    prs_main_op_fifo_full_cnt_.reset();
    prs_main_op_fifo_full_stall_cnt_.reset();
    prs_main_partial_hdr_err_cnt_.reset();
    prs_main_phv_owner_err_cnt_.reset();
    prs_main_src_ext_err_cnt_.reset();
    prs_main_tcam_par_err_cnt_.reset();
    prs_main_timeout_iter_err_cnt_.reset();
    prs_main_timeout_cycle_err_cnt_.reset();

    // These registers are instantiated but are NOT used - however allows read/write
    prs_main_debug_ctrl_.reset();
    prs_main_ecc_.reset();
    prs_main_max_cycle_.reset();

    prs_ea_ram_.reset();
    prs_action_ram_.reset();
    prs_ctr_init_ram_.reset();

    tcam_match_.reset();
    tcam_match_.set_tcam_start(kStates-1);
    inbuf_reset();
    counter_reset();
    offset_reset();
    //checksum_reset(); Now happens during ChecksumEngine CTOR
    for (int ci = 0; ci < kChannels; ci++) congested_[ci] = false;

    reset_running_ = false; // OK to handle callbacks again
  }
  void ParserShared::reset() {
    reset_this();
  }
  // Report that register is not implemented
  void ParserShared::not_implemented(const char *clazz) {
    if (reset_running_) return;
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCreate),
            "ParserShared::WriteCB(%s) Register NOT implemented\n", clazz);
    //printf("Register %s not implemented\n", clazz);
  }
  // Called whenever a Parser memory is written - typically overridden in subclass
  void ParserShared::memory_change_callback(uint8_t mem_type, uint8_t mem_inst, uint32_t mem_index) {
    if (reset_running_) return;
  }
  // Called when PHV ownership changes
  void ParserShared::phv_owner_change_callback() {
  }


  // Set Mode of parser etc
  void ParserShared::set_parser_mode(uint8_t _mode, uint16_t _max_iter) {
    // For now set self (ingress) up as owner of all PHV words
    for (int i = 0; i < Phv::phv_max_extended_p(); i++) phv_set_owner(i);
    set_mode(_mode);
    set_max_iter(_max_iter);
  }

  // Set ParserChannel config
  void ParserShared::set_channel(int chan, bool _enabled, uint8_t _start_state) {
    set_enabled(chan, _enabled);
    set_start_state(chan, _start_state);
    set_hdr_byte_count(chan, UINT64_C(0));
    set_idle_count(chan, UINT64_C(0));
    set_drop_count(chan, UINT64_C(0));
    set_rx_count(chan, UINT64_C(0));
    set_tx_count(chan, UINT64_C(0));
  }
  void ParserShared::set_channel_congested(int chan, bool _congested) {
    if ((chan >= 0) && (chan < kChannels)) congested_[chan] = _congested;
  }

  // Set word0/word1 BitVectors for some TCAM index
  void ParserShared::get_tcam_word0_word1(int index, BitVector<44> *word0, BitVector<44> *word1) {
    tcam_match_.get(index, word0, word1);
  }
  // Set word0/word1 BitVectors for some TCAM index
  void ParserShared::set_tcam_word0_word1(int index, const BitVector<44> &word0, const BitVector<44> &word1) {
    // valid_tcam_entry defined as taking value then mask so pass word1 as value
    // 28/11/14: No longer allow Parser to dictate validity of TCAM entry - all
    // combinations of ver_1/ver_0 valid (26/11)
    //tcam_match_.set_word0_word1(index, word0, word1, valid_tcam_entry(word1,word0));
    // TODO: remove from tcam3.h versions of set_word0_word1 that allow valid to be specified
    tcam_match_.set_word0_word1(index, word0, word1);
  }
  // Set value/mask BitVectors for some TCAM index
  void ParserShared::set_tcam_value_mask(int index, const BitVector<44> &value, const BitVector<44> &mask) {
    //tcam_match_.set_value_mask(index, value, mask, valid_tcam_entry(value,mask));
    tcam_match_.set_value_mask(index, value, mask);
  }
  void ParserShared::set_tcam_match(int index, const BitVector<44> &value, const BitVector<44> &mask) {
    set_tcam_value_mask(index, value, mask);
  }


  // Set common ParserEarlyAction vals for some action index
  void ParserShared::set_early_action_shared(int index, uint8_t _counter_load_imm,
                                             bool _counter_load_src, bool _counter_load,
                                             bool _done, uint8_t _shift_amount,
                                             uint8_t _next_state_mask, uint8_t _next_state) {
    RMT_ASSERT(!RmtObject::is_jbay_or_later());
    set_counter_load_imm(index, _counter_load_imm);
    set_counter_load_src(index, _counter_load_src);
    set_counter_load(index, _counter_load);
    set_done(index, _done);
    set_shift_amount(index, _shift_amount);
    set_buf_req(index, _shift_amount);
    set_next_state_mask(index, _next_state_mask);
    set_next_state(index, _next_state);
  }
  // set_early_action_shared variant that accepts 2b counter_ctr_op field
  void ParserShared::set_early_action_shared(int index, uint8_t _counter_load_imm,
                                             uint8_t _counter_ctr_op,
                                             bool _done, uint8_t _shift_amount,
                                             uint8_t _next_state_mask, uint8_t _next_state) {
    set_counter_load_imm(index, _counter_load_imm);
    set_counter_ctr_op(index, _counter_ctr_op);
    set_done(index, _done);
    set_shift_amount(index, _shift_amount);
    set_buf_req(index, _shift_amount);
    set_next_state_mask(index, _next_state_mask);
    set_next_state(index, _next_state);
  }


  // Set ParseAction vals for some action index
  void ParserShared::set_action(int index,
                                bool _offset_reset, uint8_t _offset_incr_val,
                                bool _checksum_enable[], uint8_t _checksum_ram_addr[],
                                bool _extract8_src_imm_val[], bool _extract8_add_off[],
                                uint8_t _extract8_src[],  uint16_t _extract8_dst_phv[],
                                bool _extract16_src_imm_val[], bool _extract16_add_off[],
                                uint8_t _extract16_src[], uint16_t _extract16_dst_phv[],
                                bool _extract32_src_imm_val[], bool _extract32_add_off[],
                                uint8_t _extract32_src[], uint16_t _extract32_dst_phv[],
                                int _dst_phv_format, int disabled) {

    set_offset_reset(index, _offset_reset);
    set_offset_incr_val(index, _offset_incr_val);
    for (int i = 0; i < kChecksumEngines; i++) {
      set_checksum_enable(index, i, _checksum_enable[i]);
      set_checksum_ram_addr(index, i, _checksum_ram_addr[i]);
    }
    // TODO: Looks like only a subset of these can be configured
    // to support immediate constants (2x8 1x16 1x32). Don't know
    // which yet. Should we suppress/report on config or treat
    // as runtime error??
    for (int i = 0; i < kExtractMax_8b; i++) {
      set_extract8_src_imm_val(index, i, _extract8_src_imm_val[i]);
      set_extract8_add_off(index, i, _extract8_add_off[i]);
      set_extract8_src(index, i, _extract8_src[i]);
      set_extract8_dst_phv(index, i, map_dst_phv(_extract8_dst_phv[i], _dst_phv_format));
    }
    for (int i = 0; i < kExtractMax_16b; i++) {
      set_extract16_src_imm_val(index, i, _extract16_src_imm_val[i]);
      set_extract16_add_off(index, i, _extract16_add_off[i]);
      set_extract16_src(index, i, _extract16_src[i]);
      set_extract16_dst_phv(index, i, map_dst_phv(_extract16_dst_phv[i], _dst_phv_format));
    }
    for (int i = 0; i < kExtractMax_32b; i++) {
      set_extract32_src_imm_val(index, i, _extract32_src_imm_val[i]);
      set_extract32_add_off(index, i, _extract32_add_off[i]);
      set_extract32_src(index, i, _extract32_src[i]);
      set_extract32_dst_phv(index, i, map_dst_phv(_extract32_dst_phv[i], _dst_phv_format));
    }
  }

  // Set EXTRA ParseAction vals for some action index
  // Didn't want to change set_action API so added new funcs to
  // set things like rot_imm_val and priority
  void ParserShared::set_action_extra(int index,
                                      bool _extract8_rot_imm_val[],
                                      bool _extract16_rot_imm_val[],
                                      bool _extract32_rot_imm_val[]) {
    for (int i = 0; i < kExtractMax_8b; i++) {
      set_extract8_rot_imm_val(index, i, _extract8_rot_imm_val[i]);
    }
    for (int i = 0; i < kExtractMax_16b; i++) {
      set_extract16_rot_imm_val(index, i, _extract16_rot_imm_val[i]);
    }
    for (int i = 0; i < kExtractMax_32b; i++) {
      set_extract32_rot_imm_val(index, i, _extract32_rot_imm_val[i]);
    }
  }
  void ParserShared::set_action_pri(int index,
                                    uint8_t pri_upd_type, uint8_t pri_upd_src,
                                    uint8_t pri_upd_en_shr, uint8_t pri_upd_val_mask) {
    set_pri_upd_type(index, pri_upd_type);
    set_pri_upd_src(index, pri_upd_src);
    set_pri_upd_en_shr(index, pri_upd_en_shr);
    set_pri_upd_val_mask(index, pri_upd_val_mask);
  }
  void ParserShared::set_action_ver(int index,
                                    uint8_t ver_upd_type, uint8_t ver_upd_src,
                                    uint8_t ver_upd_en_shr, uint8_t ver_upd_val_mask) {
    // Not all chips can update version
    // so expect this to be overridden in PER-CHIP subclass
    RMT_ASSERT(0);
  }
  void ParserShared::set_action_clot(int index, int clot_index,
                                     uint8_t clot_type,
                                     uint8_t clot_len_src, uint8_t clot_en_len_shr,
                                     uint8_t clot_len_mask, uint8_t clot_len_add,
                                     uint8_t clot_offset,
                                     uint8_t clot_tag, bool clot_tag_offset_add,
                                     uint8_t clot_has_csum) {
    // Not all chips do CLOT so expect this to be overridden in PER-CHIP subclass
    RMT_ASSERT(0);
  }




  // Set ParserCounterInit vals for some counter_index
  void ParserShared::set_counter_init(int counter_index,
                                      uint8_t _add, uint8_t _mask_width, uint8_t _rotate,
                                      uint8_t _max, uint8_t _source, uint8_t _add_to_stack) {
    set_pcnt_add(counter_index, _add);
    set_pcnt_mask_width(counter_index, _mask_width);
    set_pcnt_rotate(counter_index, _rotate);
    set_pcnt_max(counter_index, _max);
    set_pcnt_source(counter_index, _source);
    set_pcnt_add_to_stack(counter_index, _add_to_stack);
  }

  // Set ParserChecksum vals for some checksum_index
  // regs_5794_main:
  // - now have N checksum RAMs so need to add checksum_ram_index to this func
  void ParserShared::set_checksum(int checksum_ram_index, int checksum_index,
                                  uint16_t _shift_left, uint32_t _swap, uint32_t _mask,
                                  bool _rotate, uint16_t _add,
                                  uint8_t _dst_bit_final_pos, bool _final,
                                  uint16_t _dst_phv, bool _dst_update,
                                  bool _residual, bool _start, uint32_t mul2) {
    ChecksumEngine *cksmeng = get_checksum_engine(checksum_ram_index);
    if (cksmeng != NULL) {
      cksmeng->set_checksum(checksum_index,
                            _shift_left, _swap, _mask, _rotate,_add,
                            _dst_bit_final_pos, _final,
                            _dst_phv, _dst_update, _residual, _start, mul2);
    }
  }
  // regs_5794_main:
  // - now have N checksum RAMs so add this wrapper to write identical config to all
  void ParserShared::set_checksum(int checksum_index,
                                  uint16_t _shift_left, uint32_t _swap, uint32_t _mask,
                                  bool _rotate, uint16_t _add,
                                  uint8_t _dst_bit_final_pos, bool _final,
                                  uint16_t _dst_phv, bool _dst_update,
                                  bool _residual, bool _start) {
    for (int i = 0; i < kChecksumEngines; i++) {
      set_checksum(i, checksum_index,
                  _shift_left, _swap, _mask, _rotate,_add,
                  _dst_bit_final_pos, _final,
                  _dst_phv, _dst_update, _residual, _start);
    }
  }


  // Write 16b to a PHV, handle splitting across 2x 8b words
  void ParserShared::phv_write_16b(Phv *phv, int phv_or_off, uint16_t val, ParseContext *context) {
    if (Phv::is_valid_phv_p(phv_or_off)) {
      int width = Phv::which_width_p(phv_or_off);
      if (width == 8) {
        phv_set_p(phv, phv_or_off, (val >> 8) & 0xFF, context, false);
        if (Phv::is_valid_phv_p(phv_or_off + 1))
          phv_set_p(phv, phv_or_off + 1, (val >> 0) & 0xFF, context, false);
      } else {
        RMT_ASSERT((width == 16) || (width == 32));
        phv_set_p(phv, phv_or_off, val, context, false);
      }
    }
  }
  // As above but returning error code
  uint32_t ParserShared::phv_check_write_16b(Phv *phv, int phv_or_off, uint16_t val, ParseContext *context) {
    uint32_t err = 0u;
    if (phv_can_write(phv, phv_or_off)) {
      phv_write_16b(phv, phv_or_off, val, context);
    } else {
      // Can't write - figure out error code
      if      (!phv_valid_16b(phv, phv_or_off)) err |= kErrDstCont;
      else if (!phv_owner_16b(phv, phv_or_off)) err |= kErrPhvOwner;
      else                                      err |= kErrMultiWr;
    }
    return err;
  }
  // Output parse errors to a PHV word determined by config
  void ParserShared::perr_phv_write(uint16_t dst, uint32_t err, Phv *phv) {
    if ((err == 0u) || (phv == NULL)) return;
    // Note after next mapping step kErr* tokens may no longer match
    uint16_t hw_err = static_cast<uint16_t>(prs_map_sw_errs_to_hw_errs(err) & 0xFFFF);

    uint16_t raw_dst_phv_word = dst;
    int      dst_phv_word[2]; // Dual extract for Trestles!
    dst_phv_word[0] = get_phv_to_write(raw_dst_phv_word);
    dst_phv_word[1] = get_tphv_to_write(raw_dst_phv_word);

    ParseContext context("emitted errors");
    context.extracted_value_ = hw_err;
    context.extracted_value_size_ = 2;
    for (int i = 0; i < 2; i++) {
      // Call phv_write_16 to do writes
      // (Note original logic didn't check +1 was valid!)
      phv_write_16b(phv, dst_phv_word[i], hw_err, &context);
    }
  }

  template <typename T>
  void ParserShared::check_extract_constraints(int index, int which_extract,
                                               const int word_tab[],
                                               int phv_word, Phv *phv, T val,
                                               uint8_t extract_type,
                                               int ck8, int ck16, int ck32,
                                               bool partial_hdr_err) {
    uint32_t check = kCheckFlags;

    // In all cases check we have a valid phv word
    if (((check & kCheckValidity) != 0) && (!Phv::is_valid_phv_p(phv_word))) {
      RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractSoftError),
              "ParserShared::inbuf_extract_%zu_to_%d(%d,%d) INVALID phv=%d\n",
              sizeof(T)*8, Phv::which_width_p(phv_word),
              index, which_extract, phv_word);
      if (!kRelaxExtractionCheck) { THROW_ERROR(-2); }
    }
    // In all cases check can write phv word
    if (((check & kCheckCanWrite) != 0) && (!phv_can_write(phv, phv_word))) {
      const char *ownstr = phv_owner(phv_word) ?"" :"not owner";
      const char *nmwstr = phv_multi_write(phv_word) ?"" :"not multi-writable";
      RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractSoftError),
              "ParserShared::inbuf_extract_%zu_to_%d(%d,%d) phv=%d "
              "CAN NOT WRITE<%d> %d  %s %s\n",
              sizeof(T)*8, Phv::which_width_p(phv_word),
              index, which_extract, phv_word, io_index_, phv_word,
              ownstr, nmwstr);
      // XXX: don't throw in this scenario - overzealous
      // if (!kRelaxExtractionCheck) { THROW_ERROR(-2); }
    }

    // Special checks for mismatched widths - extractor width != phv width
    if (((check & kCheckMismatchedWidths) != 0) &&
        (Phv::which_width_p(phv_word) != sizeof(T)*8)) {

      if (sizeof(T)*8 == 2 * Phv::which_width_p(phv_word)) {
        // 32->2x16 or 16->2x8 and NOT JBay halfwidth extract
        // 1. PHV must be even
        // 2. Both PHV and PHV+1 must be writable (already checked one though)
        if ((phv_word % 2) != 0) {
          RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractSoftError),
                  "ParserShared::inbuf_extract_%zu_to_%d(%d,%d) phv=%d "
                  "MUST specify *EVEN* PHV word when extracting to pair\n",
                  sizeof(T)*8, Phv::which_width_p(phv_word),
                  index, which_extract, phv_word);
          if (!kRelaxExtractionCheck && !partial_hdr_err) { THROW_ERROR(-2); }
        }
        int phv_word0 = phv_word & ~0x1; // Make even
        int phv_word1 = phv_word0 + 1;
        int phv_word_other = (phv_word == phv_word0) ?phv_word1 :phv_word0;
        if (!phv_can_write(phv, phv_word_other)) {
          const char *ownstr = phv_owner(phv_word_other) ?"" :"not owner";
          const char *nmwstr = phv_multi_write(phv_word_other) ?"" :"not multi-writable";
          RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractSoftError),
                  "ParserShared::inbuf_extract_%zu_to_%d(%d,%d) phv=%d "
                  "PAIRED PHV CAN NOT WRITE<%d> %d  %s %s\n",
                  sizeof(T)*8, Phv::which_width_p(phv_word_other),
                  index, which_extract, phv_word_other, io_index_, phv_word_other,
                  ownstr, nmwstr);
          // XXX: don't throw in this scenario - overzealous
          // if (!kRelaxExtractionCheck) { THROW_ERROR(-2); }
        }

      } else if (sizeof(T)*8 * 2 == Phv::which_width_p(phv_word)) {
        // 16->lohi32 or 8->lohi16
        // 1. PHV must be paired [0,1] or [2,3]
        //
        // XXX: checksum validation results are inserted _ahead_ of extractions.
        // For example, a single 16b checksum verification result will occupy
        // slot 0, pushing extractor 0 back to slot 1, etc. The valid pairing
        // is [1,2] because these are pushed to slots [2,3].
        int ck_offset = sizeof(T) * 8 == 8 ? ck8 : ck16;
        int i_paired = (((which_extract + ck_offset) % 2) == 0) ?(which_extract+1) :(which_extract-1);
        if (i_paired < 0 || i_paired >= kExtractPhvWordsPerCycle) {
          RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractSoftError),
                  "ParserShared::inbuf_extract_%zu_to_%d(%d,%d) "
                  "extractor=%d paired_extractor=%d - extractor index out of bounds\n",
                  sizeof(T)*8, Phv::which_width_p(phv_word),
                  index, which_extract, which_extract, i_paired);
          if (!kRelaxExtractionCheck) { THROW_ERROR(-2); }
        } else {
          int phv_word_paired = word_tab[i_paired];
          if (phv_word != phv_word_paired) {
            RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractSoftError),
                    "ParserShared::inbuf_extract_%zu_to_%d(%d,%d) phv=%d paired_phv=%d "
                    "extractor%d=%d extractor%d=%d - MUST refer to same PHV word\n",
                    sizeof(T)*8, Phv::which_width_p(phv_word),
                    index, which_extract, phv_word, phv_word_paired,
                    which_extract, phv_word, i_paired, phv_word_paired);
            if (!kRelaxExtractionCheck && !partial_hdr_err) { THROW_ERROR(-2); }
          }
        }

      } else if (sizeof(T)*8 * 4 == Phv::which_width_p(phv_word)) {
        // 8->octetIn32
        // 1. Surrounding group of 4 PHV words must be identical
        bool all_same = true;
        int i_start = which_extract - (which_extract % 4);
        for (int i = i_start; i < i_start + 4; i++) {
          if (phv_word != word_tab[i]) all_same = false;
        }
        if (!all_same) {
          RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractSoftError),
                  "ParserShared::inbuf_extract_%zu_to_%d(%d,%d) phv=%d "
                  "extractors[%d..%d] MUST all refer to same PHV word\n",
                  sizeof(T)*8, Phv::which_width_p(phv_word),
                  index, which_extract, phv_word, i_start, i_start+3);
          if (!kRelaxExtractionCheck && !partial_hdr_err) { THROW_ERROR(-2); }
        }

      } else {
        // Some other extractSize -> phvSize config - error
        RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractSoftError),
                "ParserShared::inbuf_extract_%zu_to_%d(%d,%d) phv=%d "
                "WIDTH MISMATCH (extractSz=%zu,phvSz=%d)\n",
                sizeof(T)*8, Phv::which_width_p(phv_word),
                index, which_extract, phv_word,
                sizeof(T)*8, Phv::which_width_p(phv_word));
        if (!kRelaxExtractionCheck) { THROW_ERROR(-2); }
      }
    }

    if (((check & kCheckFurther) != 0) && (is_jbay_or_later())) {
      // Apply PER-CHIP constraint checks - do them now if JBay/WIP
      check_further_extract_constraints(check, index, which_extract,
                                        phv_word, phv, sizeof(T)*8,
                                        partial_hdr_err);
    }
  }

  template <typename T>
  void ParserShared::check_extract_constraints_2(int index, int which_extract,
                                                 const int word_tab[],
                                                 int phv_word, Phv *phv, T val,
                                                 uint8_t extract_type,
                                                 int ck8, int ck16, int ck32,
                                                 bool partial_hdr_err) {
    uint32_t check = kCheckFlags;
    if (((check & kCheckFurther) != 0) && (is_tofinoXX())) {
      // Apply PER-CHIP constraint checks - do them now if Tofino
      // EG on Tofino checking num 8b/16b prev extractions is 4N/2N
      //
      // XXX: these used to occur *BEFORE* perform_extract
      // but they must happen AFTER to take account of any narrow
      // wide extractions that have occurred this cycle
      check_further_extract_constraints(check, index, which_extract,
                                        phv_word, phv, sizeof(T)*8,
                                        partial_hdr_err);
    }
  }

  template <typename T>
  uint32_t  ParserShared::perform_extract(int index, int which_extract,
                                          const int word_tab[],
                                          int phv_word, Phv *phv, T val,
                                          uint8_t extract_type,
                                          ParseContext *context,
                                          int ck8, int ck16, int ck32) {
    uint32_t xtrac = kExtractFlags;
    if (((xtrac & kExtract8bSpecial) != 0) && (sizeof(T)*8 == 8)) {
      return perform_extract_8b(index, which_extract, word_tab, phv_word,
                                phv, val, extract_type, context);
    } else if (((xtrac & kExtract16bSpecial) != 0) && (sizeof(T)*8 == 16)) {
      return perform_extract_16b(index, which_extract, word_tab, phv_word,
                                 phv, val, extract_type, context);
    } else if (((xtrac & kExtract32bSpecial) != 0) && (sizeof(T)*8 == 32)) {
      return perform_extract_32b(index, which_extract, word_tab, phv_word,
                                 phv, val, extract_type, context);
    } else {
      uint32_t errs = 0u;
      // GRG: H/W ECO to allow wide extractions to narrow fields (30 Jan 2016)
      // No dest addr checking, ownership/multiwrite checking only if
      // extraction and container sizes match
      //if ((Phv::is_valid_phv_p(phv_word)) &&
      //    (Phv::which_width_p(phv_word) == sizeof(T)*8) &&
      //    (phv_can_write(phv, phv_word))) {
      if  ((Phv::which_width_p(phv_word) != sizeof(T)*8) ||
           ((Phv::which_width_p(phv_word) == sizeof(T)*8) &&
            (phv_can_write(phv, phv_word)))) {

        if (Phv::which_width_p(phv_word) == sizeof(T)*8) {
          phv_set_p(phv, phv_word, (uint32_t)val, context, false); // OR

        } else if (sizeof(T)*8 == 2 * Phv::which_width_p(phv_word)) {
          int phv_width = Phv::which_width_p(phv_word);
          uint32_t mask = (1 << phv_width) - 1;
          phv_set_p(phv, phv_word & 0x1fe, (uint32_t)(val >> phv_width), context, false); // OR
          phv_set_p(phv, (phv_word & 0x1fe) + 1, (uint32_t)(val) & mask, context, false); // OR

        } else if (sizeof(T)*8 * 2 == Phv::which_width_p(phv_word)) {
          // XXX: checksum validation results are inserted _ahead_ of extractions.
          // For example, a single 16b checksum verification result will occupy
          // slot 0, pushing extractor 0 back to slot 1, etc.
          int ck_offset = sizeof(T) * 8 == 8 ? ck8 : ck16;
          uint32_t wide_val = (uint32_t)val << (sizeof(T) * 8 * (1 - ((which_extract + ck_offset) % 2)));
          phv_set_p(phv, phv_word, (uint32_t)wide_val, context, false); // OR

        } else if (sizeof(T)*8 * 4 == Phv::which_width_p(phv_word)) {
          // The odd alignment here is due to HW arbiter implementation
          uint32_t wide_val = (uint32_t)val << (sizeof(T) * 8 * (3 - ((which_extract + 2) % 4)));
          phv_set_p(phv, phv_word, (uint32_t)wide_val, context, false); // OR
        } else {
          // 32b->8b extraction NOT currently allowed
        }
        // Keep track of how many 8b/16b extractions performed
        if      (sizeof(T)*8 ==  8) extract_info_.n_8b_extracts_++;
        else if (sizeof(T)*8 == 16) extract_info_.n_16b_extracts_++;


      } else {
        // Handle different error cases
        //
        // Could do with different error codes here to discriminate
        // someone trying to write an invalid PHV (eg 224-255 or >367)
        // from someone writing multiple times to a PHV word that they
        // don't own, or one that can not be written to more than once
        //
        // GRG: ECO (1/30/16)(30 Jan 2016)
        //if (!Phv::is_valid_phv_p(phv_word)) {
        //  RMT_LOG(RmtDebug::kRmtDebugParserExtractError,
        //          "ParserShared::inbuf_extract_%ld(%d,%d) phv=%d+%d val=%08x - "
        //          "INVALID PHV\n",
        //          sizeof(T)*8, index, i, phv_word, incr, val);
        //  errs |= kErrDstCont;
        //}
        // GRG: ECO (1/30/16)(30 Jan 2016)
        if ((Phv::which_width_p(phv_word) == sizeof(T)*8) &&
            !phv_can_write(phv, phv_word)) {
          RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractError),
                  "ParserShared::inbuf_extract_%ld(%d,%d) phv=%d val=%08x - "
                  "CAN NOT WRITE\n",
                  sizeof(T)*8, index, which_extract, phv_word, val);
          RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractError),
                  "ParserShared::inbuf_extract_%ld(%d,%d) io_index=%d owner=%d nmw=%d\n",
                  sizeof(T)*8, index, which_extract, io_index_,
                  phv_owner_val(phv_word), phv_multi_write_val(phv_word));
          // Figure out what err flags to set
          // 1. Maybe can't write because not owner,
          // 2. Or maybe because multi-write not set
          if (!phv_owner(phv_word))
            errs |= kErrPhvOwner;
          else if (phv->is_valid_p(phv_word) && !phv_multi_write(phv_word))
            errs |= kErrMultiWr;
        }
        // GRG: ECO (1/30/2016)(30 Jan 2016)
        if ((sizeof(T)*8 == 8) &&
            (Phv::which_width_p(phv_word) != sizeof(T)*8)) {
          RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserExtractError),
                  "ParserShared::inbuf_extract_%ld(%d,%d) phv=%d val=%08x - "
                  "WIDTH MISMATCH (valSz=%d,phvSz=%d)\n",
                  sizeof(T)*8, index, which_extract, phv_word, val,
                  sizeof(T)*8, Phv::which_width_p(phv_word));
          errs |= kErrDstCont;
        }
      }
      return errs;
    }
  }

  // XXX: Tofino: Mimic worse case parser arbiter fifos occupancy in order to handle
  // n2w extraction constraints properly.
  // Arbiter fifo occupancy read (decrement).
  void ParserShared::arbiter_fifo_occ_read(uint32_t occ_8b, uint32_t occ_16b, uint32_t occ_32b) {
    // Mimic the reads from the arbiter fifo which get occupied (piled up) because twice as many
    // writes compared to read can happens at each states for the 16b and 32b  extractions fifos.
    // Ignoring the cases where 2 x 16b values can be places on a 32b bus, or when 2 x 8b values
    // can be placed on a 16b bus when considering worst-case FIFO occupancy behavior.
    // The read priority ordering is:
    // Phase 1: 32-bit bus arbitration   Phase 2: 16-bit bus arbitration  Phase 3: 8-bit bus arbitration
    // Priority:                         Priority:                        Priority:
    //  1 - 32b         32b              1 - 16b       16b                1 - 8b    8b    8b    8b
    //  2 - 32b         {16b 16b}        2 - 32b--------->                2 - 8b    8b    16b----->
    //  3 - 32b         ---              3 - 16b       {8b 8b}            3 - 16b----->   16b----->
    //  4 - {16b 16b}   {16b 16b}        4 - {8b 8b}   {8b 8b}            4 - 32b----------------->
    //  5 - {16b 16b}   ---              5 - 16b       ---                5 - 8b    8b    8b    ---
    //  6 - ---         ---              6 - {8b 8b}   ---                6 - 8b    ---   16b----->
    //                                   7 - ---       ---                7 - 8b    8b    ---   ---
    //                                                                    8 - 16b----->   ---   ---
    //                                                                    9 - 8b    ---   ---   ---
    //                                                                   10 - ---   ---   ---   ---

    // Reads to 2 x 32b buses
    if (occ_32b < 2)
      occ_32b = 0;
    else
      occ_32b -= 2;

    // Reads to 2 x 16b buses
    if (occ_16b > 0) {
      if (occ_16b < 2)
        occ_16b = 0;
      else
        occ_16b -= 2;
    }
    else if (occ_32b > 0)
      occ_32b--;

    // Reads to 4 x 8b buses
    if (occ_8b >= 4)
      occ_8b -= 4;
    else if (occ_8b >= 2 && occ_16b > 0) {
      occ_8b -= 2;
      occ_16b--;
    }
    else if (occ_16b >= 2)
      occ_16b -= 2;
    else if (occ_32b > 0)
      occ_32b--;
    else {
      if (occ_8b == 3)
        occ_8b = 0;
      else {
        occ_8b = 0;   // either 0 or 1 8b entry, so read it
        occ_16b = 0;  // either 0 or 1 16b entry, so read it
      }
    }
  }

  // XXX: Tofino: Mimic worse case parser arbiter fifos occupancy in order to
  // handle n2w extraction constraints properly.
  // Currently the following constraints are checked before and at the current
  // state (XXX) where narrow-to-wide extractions are done.
  // But need also to check the states after the last narrow-to-wide extraction as
  // something in the states after can interfer with the bus packing.
  // This is done by minic every state up to the last narrow-to-wide state and calculate
  // the FIFO occupancy after each state assuming that nothing is read from the FIFO.
  // Then start reading when reaching the full threshold and then apply the normal
  // narrow-to-wide constraints at all states (i.e., must be 0/2/4 x 16b extracts +
  // 0/4 x 8b extracts per state), until FIFO n2w count is 0 (empty).
  template <typename T>
  void ParserShared::check_arbiter_fifo_occ(const int word_tab[], int n_xt, T val) {
    uint32_t check = kCheckFlags;
    if (((check & kCheckFurther) != 0) && (is_tofinoXX())) {
      ExtractInfo &x = extract_info_;

      // Accumulate worse-case arbiter fifo occupancy.
      if      (sizeof(T)*8 ==  8) extract_info_.n_8b_wc_fifo_occ_ += n_xt;
      else if (sizeof(T)*8 == 16) extract_info_.n_16b_wc_fifo_occ_ += n_xt;
      else if (sizeof(T)*8 == 32) extract_info_.n_32b_wc_fifo_occ_ += n_xt;

      for (int i = 0; i < n_xt; i++) {
        if ((sizeof(T)*8 * 2 == Phv::which_width_p(word_tab[i])) ||
            (sizeof(T)*8 * 4 == Phv::which_width_p(word_tab[i]))) {
          // Narrow-to-wide extraction.
          // Only care about 8b -> 32b and 16b -> 32b extracts for future state padding.
          // (8b -> 16b narrow-to-wide extracts are not impacted by future states.)
          if (sizeof(T)*8 == 8) {
            if (Phv::which_width_p(word_tab[i]) == 32) {
              x.n2w_8b_32b_recently_seen_ = true;
              x.n2w_8b_in_curr_state_ = true;
            }
          }
          else if (sizeof(T)*8 == 16) {
            x.n2w_16b_32b_recently_seen_ = true;
            x.n2w_16b_in_curr_state_ = true;
          }
        }
      }

      // Decrement outstanding pad state counts
      if (n_xt) {
        if      (sizeof(T)*8 ==  8 && x.n_8b_pad_states_needed_)
          x.n_8b_pad_states_needed_--;
        else if (sizeof(T)*8 == 16 && x.n_16b_pad_states_needed_)
          x.n_16b_pad_states_needed_--;
      }
    }
  }

  // Read immediate constant from extract8/16/32 src field
  // possibly right rotating (by offset_adjust_ if offset_rot_imm flag set)
  template <typename T> bool ParserShared::imm_const_get(int index, int which, T *val, int shift) {
    static_assert(std::is_integral<T>::value, "only integral types allowed" );
    static_assert(!std::is_same< T, bool >::value, "bool not allowed");
    // All extracts below use mask 0xFF
    bool imm_const_got = true;
    if (sizeof(T)*8 == 8)
      *val = extract8_src_const(index, which, kExtractSrcImmConstMask, shift);
    else if (sizeof(T)*8 == 16)
      *val = extract16_src_const(index, which, kExtractSrcImmConstMask, shift);
    else if (sizeof(T)*8 == 32)
      *val = extract32_src_const(index, which, kExtractSrcImmConstMask, shift);
    else imm_const_got = false;
    return imm_const_got;
  }

  void ParserShared::log_phv_set_p(Phv *phv,
                                   int word,
                                   uint32_t val,
                                   ParseContext *context,
                                   bool clobber) {
    uint64_t log_flag = RmtDebug::kRmtDebugVerbose;
    if (context && rmt_log_type_check(log_flag, RMT_LOG_TYPE_P4)) {
      int mapped_word = RmtDefs::map_prsr_phv_index(word);
      std::string name = get_object_manager()->get_phv_name(pipe_index(), 0 /* stage 0 */ , mapped_word);
      if (nullptr == context->preamble_) {
        RMT_TYPE_LOG(
            RMT_LOG_TYPE_P4, log_flag,
            "%s Parser state '%s': extracted %*s%0*x, %s PHV word %3d to value %*s%0*x %s\n",
            gress_str(),
            rmt_log_parser_state(parser_index(), egress(), context->state_).c_str(),
            2 * (4 - context->extracted_value_size_) + 2,
            "0x",
            2 * context->extracted_value_size_,
            context->extracted_value_,
            clobber ? " setting" : "updating",
            word,
            2 * (4 - Phv::which_width_in_bytes_p(word)) + 2,
            "0x",
            2 * Phv::which_width_in_bytes_p(word),
            phv->get_p(word),
            name.c_str()
        );
      } else {
        RMT_TYPE_LOG(
            RMT_LOG_TYPE_P4, log_flag,
            "%s Parser %s 0x%0*x, %s PHV word %3d to value 0x%0*x %s\n",
            gress_str(),
            context->preamble_,
            2 * context->extracted_value_size_,
            context->extracted_value_,
            clobber ? " setting" : "updating",
            word,
            2 * Phv::which_width_in_bytes_p(word),
            phv->get_p(word),
            name.c_str()
        );
      }
    }
  }
  void ParserShared::phv_set_p(Phv *phv, int word, uint32_t val, ParseContext *context, bool clobber) {
    if (clobber) {
      phv->clobber_p(word, val);
    } else {
      phv->set_p(word, val);
    }
    log_phv_set_p(phv, word, val, context, clobber);
  }

  // Use indexed extract_src offset to get byte/short/int from inbuf
  // then use indexed extract_dst_phv to write val to correct word in PHV
  template <typename T> uint32_t ParserShared::inbuf_extract_to_phv(int index, Phv *phv, const uint64_t pkt_id, T val,
                                                                    int n_xt, int ck8,
                                                                    int ck16, int ck32,
                                                                    ParseContext *context,
                                                                    bool partial_hdr_err) {
    RMT_ASSERT(n_xt <= kExtractPhvWordsPerCycle);
    static_assert(std::is_integral<T>::value, "only integral types allowed" );
    static_assert(!std::is_same< T, bool >::value, "bool not allowed");
    auto om = get_object_manager();
    uint32_t errs = 0u;
    uint32_t version = version_get();
    uint32_t value_tab[kExtractPhvWordsPerCycle]; // Size at max
    int      norm_tab[kExtractPhvWordsPerCycle+3];
    int      taga_tab[kExtractPhvWordsPerCycle+3];
    uint8_t  extract_type_tab[kExtractPhvWordsPerCycle+3];
    uint8_t  xt_type;
    bool     xt_available;
    int      phv_or_off, norm_word, taga_word;
    uint8_t  buf_occ = buf_req(index);
    int      xt_cnt = 0;

    if (n_xt == 0) return errs; // Bail if no extractors of this size

    // STEP0: Initialize (+3 is to simplify paired/quad checking)
    for (int i = 0; i < kExtractPhvWordsPerCycle+3; i++) {
      norm_tab[i] = -1; taga_tab[i] = -1; extract_type_tab[i] = kExtractTypeNone;
    }

    if      (sizeof(T)*8 ==  8) xt_cnt = ck8;
    else if (sizeof(T)*8 == 16) xt_cnt = ck16;
    else if (sizeof(T)*8 == 32) xt_cnt = ck32;

    // STEP1: Figure out all PHV words we'll be extracting to
    for (int i = 0; i < n_xt; i++) {
      value_tab[i] = 0;
      norm_tab[i] = -1;
      taga_tab[i] = -1;
      extract_type_tab[i] = kExtractTypeNone;

      xt_type = extract_type(sizeof(T)*8, index, i);
      // extractors used for cksum are not considered to be available
      xt_available = extract_available(sizeof(T)*8, i, n_xt, ck8, ck16, ck32);
      // XXX: Tofino: extractors activated for checksum validation results
      // must have their sources checked, even though the extraction is discareded.
      bool xt_src_check_needed = extract_src_check_needed(sizeof(T)*8, i, n_xt, ck8, ck16, ck32);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserExtract),
              "ParserShared::inbuf_extract_%ld(%d,%d) extract_type=%d "
              "(%savailable, source check %sneeded)\n",
              sizeof(T)*8, index, i, xt_type, xt_available ? "" : "not ",
              xt_src_check_needed ? "" : "not ");

      if ((xt_available || xt_src_check_needed) && (xt_type != kExtractTypeNone)) {
        bool got_src = false;
        val = 0;
        if (extract_src_imm_val(sizeof(T)*8, index, i)) {
          int shift = extract_rot_imm_val(sizeof(T)*8, index, i) ? offset_adjust_ : 0;
          got_src = imm_const_get(index, i, &val, shift);
          RMT_ASSERT(got_src);
          if (xt_available)
            RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserExtract),
                    "ParserShared::inbuf_extract_%ld(%d,%d) imm_shift=%d val=%08x\n",
                    sizeof(T)*8, index, i, shift, val);
        } else {
          // Update inbuf2 with immediate constants - only does anything on
          // JBay or later. If immediate constant extraction not supported (JBay
          // extractors 10-19) use version instead.
          inbuf2_update_consts(extract_immediate_consts(index, i, offset_adjust_, version));

          // If src MSB set use buf 2 (version/timestamp) otherwise normal buf 0
          // Pass MSB bitpos (5) to extract_src_msb
          // NB If MSB set we'll be using buf2 and we expect offset to be in:
          // - Tof/Tre: [54,63] (bytes [54,59]=Timestamp [60-63]=Version)
          // - JBay:    [48,63] (bytes [48,49]=0 [50,55]=Timestamp [56-59]=Version [60-63]=IMM
          // NOTE: start offset 54/48 handled within inbuf_get using read_mod_pos
          int which_buf = (extract_src_msb(sizeof(T)*8, index, i, kExtractSrcMSB) == 1) ?2 :0;
          int off = extract_src(sizeof(T)*8, index, i, kExtractSrcMask);

          // part of PartialHdrFixes
          // Check offset via PER-CHIP func
          // Certain chips (JBay) zeroise data if extract goes beyond buffer occupancy
          bool allow_partial = false; // On JBay may become true
          got_src = extract_offset_check(xt_type, off, &allow_partial, buf_occ);
          if (got_src) {
            got_src = inbuf_get(off, &val, true, allow_partial, which_buf);
          }
          if (!got_src) errs |= kErrSrcExt;
          RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserExtract),
                  "ParserShared::inbuf_extract_%ld(%d,%d) Buf=%d off=%d extract_type=%d\n",
                  sizeof(T)*8, index, i, which_buf, off, xt_type);
        }

        if (xt_available) {
          if (!got_src) {
            RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserExtractError),
                    "ParserShared::inbuf_extract_%ld(%d,%d) val=%08x - NO SRC (errs=0x%04x)\n",
                    sizeof(T)*8, index, i, val, errs);
          } else {
            phv_or_off = extract_dst_phv(sizeof(T)*8, index, i);
            int incr = extract_add_off(sizeof(T)*8, index, i) ? offset_adjust_ : 0;

            norm_word = get_phv_to_write(phv_or_off);
            if (norm_word >= 0) {
              int norm_orig = norm_word;
              norm_word = phv_increment(norm_word, incr); // Maybe wrap in group (not JBay)
              RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserExtract),
                      "ParserShared::inbuf_extract_norm_%ld(%d,%d) phv=%d (%d+%d) val=%08x\n",
                      sizeof(T)*8, index, i, norm_word, norm_orig, incr, val);
            }
            taga_word = get_tphv_to_write(phv_or_off); // Gets dealiased word
            if (taga_word >= 0) {
              int taga_orig = taga_word;
              taga_word = phv_increment(taga_word, incr); // Maybe wrap in group
              RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserExtract),
                      "ParserShared::inbuf_extract_taga_%ld(%d,%d) phv=%d (%d+%d) val=%08x\n",
                      sizeof(T)*8, index, i, taga_word, taga_orig, incr, val);
            }
            norm_tab[i]  = norm_word;                  // Stash normal phv_word to use,
            taga_tab[i]  = taga_word;                  // stash tagalong phv_word to use
            value_tab[i] = static_cast<uint32_t>(val); // and stash extracted value
            extract_type_tab[i] = xt_type;             // (also what extract_type)
            xt_cnt++;                                  // Count the extract
          }
        }
      }
    }
    // STEP2: Perform extra constraint checks
    for (int i = 0; i < n_xt; i++) {
      // NB norm_word/taga_word will only be >=0 if extract_available
      val = static_cast<T>(value_tab[i]);
      norm_word = norm_tab[i];
      xt_type = extract_type_tab[i];
      if (norm_word >= 0)
        check_extract_constraints(index, i, norm_tab, norm_word, phv, val, xt_type, ck8, ck16, ck32,
                partial_hdr_err);
      taga_word = taga_tab[i];
      if (taga_word >= 0)
        check_extract_constraints(index, i, taga_tab, taga_word, phv, val, xt_type, ck8, ck16, ck32,
                partial_hdr_err);
    }
    // STEP3: Finally perform extractions
    for (int i = 0; i < n_xt; i++) {
      // NB norm_word/taga_word will only be >=0 if extract_available
      val = static_cast<T>(value_tab[i]);
      norm_word = norm_tab[i];
      xt_type = extract_type_tab[i];
      Gress gress = ingress() ? Gress::ingress : Gress::egress;
      context->extracted_value_ = (uint32_t)val;
      context->extracted_value_size_ = sizeof(T);
      if (norm_word >= 0) {
        errs |= static_cast<uint32_t>(perform_extract(index, i, norm_tab, norm_word, phv, val, xt_type,
                    context, ck8, ck16, ck32));
        om->log_parser_extract(pkt_id, pipe_index_, gress, norm_word, +val, false);
      }
      taga_word = taga_tab[i];
      if (taga_word >= 0) {
        errs |= static_cast<uint32_t>(perform_extract(index, i, taga_tab, taga_word, phv, val, xt_type,
                    context, ck8, ck16, ck32));
        om->log_parser_extract(pkt_id, pipe_index_, gress, taga_word, +val, true);
      }
    }
    if (is_tofinoXX()) {
      // XXX: Tofino: Check arbiter fifo occupancy.
      check_arbiter_fifo_occ(norm_tab, xt_cnt, val);
    }

    // STEP4: Perform further final constraint checks
    inbuf_extract_final_checks(index, phv, -1);
    for (int i = 0; i < n_xt; i++) {
      // NB norm_word/taga_word will only be >=0 if extract_available
      val = static_cast<T>(value_tab[i]);
      norm_word = norm_tab[i];
      xt_type = extract_type_tab[i];
      if (norm_word >= 0)
        check_extract_constraints_2(index, i, norm_tab, norm_word, phv, val, xt_type, ck8, ck16, ck32,
                partial_hdr_err);
      taga_word = taga_tab[i];
      if (taga_word >= 0)
        check_extract_constraints_2(index, i, taga_tab, taga_word, phv, val, xt_type, ck8, ck16, ck32,
                partial_hdr_err);
    }
    return errs;
  }

  uint32_t ParserShared::handle_extract(int index, Phv *phv, const uint64_t pkt_id,
                                        int ck8, int ck16, int ck32, ParseContext *context,
                                        bool partial_hdr_err) {
    ExtractInfo &x = extract_info_;
    uint32_t xt_errs = 0u;
    // XXX: initialise x.n_{8b|16b}_extracts_ based on number checksum verification writes that have occurred
    x.n_8b_extracts_ += ck8;
    xt_errs |= inbuf_extract_to_phv(index, phv, pkt_id, (uint8_t)0, kExtractMax_8b,
                                    ck8, ck16, ck32, context, partial_hdr_err);
    x.n_16b_extracts_ += ck16;
    xt_errs |= inbuf_extract_to_phv(index, phv, pkt_id, (uint16_t)0, kExtractMax_16b,
                                    ck8, ck16, ck32, context, partial_hdr_err);
    // We don't track n_32b_extracts
    xt_errs |= inbuf_extract_to_phv(index, phv, pkt_id, (uint32_t)0, kExtractMax_32b,
                                    ck8, ck16, ck32, context, partial_hdr_err);
    return xt_errs;
  }
  void ParserShared::inbuf_extract_final_checks(int index, Phv *phv, int extract_cycle) {
    // Remember whether n 8b/16b extracts always 2N at end of all extractions, and
    // whether n 8b extractions has always been 4N (this only an issue and only ever checked on Tofino1)
    ExtractInfo &x = extract_info_;
    if ((x.n_8b_extracts_ % 4) != 0)  x.prev_n_8b_extracts_always_4N_ = false;
    if ((x.n_8b_extracts_ % 2) != 0)  x.prev_n_8b_extracts_always_2N_ = false;
    if ((x.n_16b_extracts_ % 2) != 0) x.prev_n_16b_extracts_always_2N_ = false;
  }
  void ParserShared::extract_counter_reset() {
    ExtractInfo &x = extract_info_;
    x.n_8b_extracts_ = 0;
    x.n_16b_extracts_ = 0;
    x.prev_n_8b_extracts_always_4N_ = true;
    x.prev_n_8b_extracts_always_2N_ = true;
    x.prev_n_16b_extracts_always_2N_ = true;
    x.n_8b_wc_fifo_occ_ = 0;
    x.n_16b_wc_fifo_occ_ = 0;
    x.n_32b_wc_fifo_occ_ = 0;
    x.n_8b_pad_states_needed_ = 0;
    x.n_16b_pad_states_needed_ = 0;
    x.n2w_8b_32b_recently_seen_ = false;
    x.n2w_16b_32b_recently_seen_ = false;
    x.n2w_8b_in_curr_state_ = false;
    x.n2w_16b_in_curr_state_ = false;
  }

  char *ParserShared::errstr_get(uint32_t err_flags) {
    bool first_time = true;
    errstr_buf_.Reset();
    for (int err = 0; err < 32; err++) {
      if (((err_flags >> err) & 1) == 1) {
        bool ok;
        if (first_time) {
          ok = errstr_buf_.Append("%s", kErrorStrings[err]);
          first_time = false;
        } else {
          ok = errstr_buf_.Append(",%s", kErrorStrings[err]);
        }
        RMT_ASSERT(ok && "Parser errstr unexpectedly truncated");
      }
    }
    return errstr_buf_.GetBuf();
  }

  uint32_t ParserShared::version_get() {
    uint32_t v = version_;
    if (v == 0u) {
      RmtObjectManager *om = get_object_manager();
      if (om != NULL) v = om->version_get();
    }
    return v;
  }
  uint64_t ParserShared::timestamp_get() {
    uint64_t ts = timestamp_;
    if (ts == UINT64_C(0)) {
      RmtObjectManager *om = get_object_manager();
      if (om != NULL) ts = om->timestamp_get();
    }
    return ts;
  }
  // Setup 3 inbufs (0=normal pkt, 1=first 16/32 bytes pkt, 2=version/timestamp)
  void ParserShared::inbuf_setup(Packet *pkt) {
    static_assert( (InputBuffer::kInputBufferSize == 32),
                   "inbuf_setup assumes input buffer sizes are 32 bytes");

    // Reset all input buffers
    inbufs_[0].reset();

    // Inbuf1/Inbuf2 setup varies PER-CHIP
    inbuf1_setup(pkt);
    inbuf2_setup(pkt);
  }
  // Force PHV valid signals specified in the phv_valid register.
  void ParserShared::phv_valid_force(Phv *phv) {
    // No valid bits in JBay
    // So no mechanism for initialising PHV word as valid
    if (is_jbay_or_later()) return; // So return immediately
    int phv_word = 0;
    while (Phv::is_valid_phv_p(phv_word)) {
      if (phv_init_valid(phv_word) && phv_owner(phv_word)) {
        // XXX: Just set word valid (not written)
        phv->set_valid_p(phv_word, true);
      }
      phv_word++;
    }
  }
  void ParserShared::counter_reset() {
    counter_ = 0;
    counter_next_ = 0;
    counter_pending_ = false;
    counter_range_err_pending_ = false;
  }
  // Fixup counter_ to reflect info from EarlyActionRAM
  // XXX: Support shift based on packet data
  // XXX: Support for WIP enhancements
  // XXX: Add some logging
  //
  bool ParserShared::counter_handle(int index,
                                    uint8_t f8_3, uint8_t f8_2,
                                    uint8_t f8_1, uint8_t f8_0, int *shift) {
    RMT_ASSERT(shift != nullptr);

    bool ok = !counter_range_err_pending_;
    // XXX: Check index before deref in RMT_LOG below
    if (index < 0) return ok; // Just harvest pending_err

    RMT_LOG(RmtDebug::verbose(), "ParserShared::counter_handle(index=%d) OP=%s "
            "UOPS=0x%02x F8[0..3]=[0x%02x,0x%02x,0x%02x,0x%02x] "
            "\n>>>>>START:  CTR=%d  NXT=%d %s %s  SHIFT=%d  CtrStack=0x%" PRIx64 "\n",
            index, counter_ctr_op_str(counter_ctr_op(index)), counter_ctr_uops(index),
            f8_0, f8_1, f8_2, f8_3,
            counter_, counter_next_,
            counter_pending_?"PEND":"", counter_range_err_pending_?"ERR_PEND":"",
            *shift, cntrstack_counter_data(counter_));

    uint16_t f16 = static_cast<uint16_t>(f8_1 << 8) | static_cast<uint16_t>(f8_0 << 0);
    uint32_t uops = counter_ctr_uops(index);
    uint8_t  pkt_imm_val = 0, imm_val;
    int8_t   imm_val_int = 0;
    uint8_t  pcnt_addr = 0;

    counter_range_err_pending_ = false;
    if ((uops & kCtrUopLoadPktBuf) != 0) {
      RMT_ASSERT(RmtObject::is_chip1_or_later());
      // Use bits[7:3] from CTR_AMT_IDX as byte pos to get source byte from input buffer
      inbuf_get(buf_byte_offset(index), &pkt_imm_val);
    }
    if ((uops & kCtrUopUsesCounterRam) != 0) {
      pcnt_addr = counter_load_addr_lsbs(index);
      if ((uops & kCtrUopLoadSearchByte) != 0) {
        // CTR_AMT_IDX bits[7:4] should be 0
        if (counter_load_addr_msbs(index) != 0) {
          RMT_LOG(RmtDebug::error(kRelaxExtractionCheck),
                  "ParserShared::counter_handle(index=%d) LoadFromCntrRam "
                  "but addr %d exceeds CounterRam size %d so masking to %d\n",
                  index, counter_load_addr(index), kCounterInitEntries, pcnt_addr);
          if (!kRelaxExtractionCheck) { THROW_ERROR(-2); }
        }
        // Use one of 4x TCAM search bytes as source byte
        pkt_imm_val = pcnt_choose_field(pcnt_addr, f8_3, f8_2, f16);
      }
      // Check source byte is valid - may need to apply rot/mask now (eg on WIP)
      if ((uops & kCtrUopCntrRamEarlyRotMask) != 0)
        pkt_imm_val = pcnt_rotmask_counter_from_field(pcnt_addr, pkt_imm_val);
      if (!pcnt_is_valid_counter(pcnt_addr, pkt_imm_val))
        counter_range_err_pending_ = true;
    }
    if (counter_range_err_pending_) return ok;


    // Load counter if it was written in prev cycle
    if (counter_pending_) counter_ = counter_next_;
    counter_pending_ = false; // counter_next_ = 0;


    if ((uops & kCtrUopPop) != 0) {
      counter_ = cntrstack_pop(index, counter_);
    }
    if ((uops & kCtrUopPush) != 0) {
      (void)cntrstack_push(index, counter_, counter_stack_upd_w_top(index));
      counter_ = 0;
    }
    if ((uops & kCtrUopZeroise0) != 0) {
      counter_ = 0;
    }
    if ((uops & kCtrUopLoadImm) != 0) {
      imm_val = counter_load_imm(index);
      imm_val_int = static_cast<int8_t>( imm_val );
    }
    if ((uops & kCtrUopLoadSearchByte) != 0) {
      imm_val_int = static_cast<int8_t>( pkt_imm_val );
    }
    if ((uops & kCtrUopLoadPktBuf) != 0) {
      RMT_ASSERT(RmtObject::is_chip1_or_later());
      if ((uops & kCtrUopUsesCounterRam) != 0) {
        // Rot/Mask applied earlier on WIP (see kCtrUopCntrRamEarlyRotMask)
      } else if ((uops & kCtrUopSetShift) != 0) {
        // Rot/Mask determined by CTR_AMT_IDX[2:0] and SHIFT_AMT
        pkt_imm_val = buf_byte_shift(index, pkt_imm_val);
      } else {
        // Rot/Mask determined by CTR_AMT_IDX[2:0] and LOOKUP_OFFSET[3]
        pkt_imm_val = buf_byte_norm(index, pkt_imm_val);
      }
      imm_val_int = static_cast<int8_t>( pkt_imm_val );
    }
    if ((uops & kCtrUopApplyCntrRam) != 0) {
      RMT_ASSERT((uops & kCtrUopLoad) != 0);
      // Calculate imm_val - may need to apply rot/mask now (eg on Tof/JBay)
      if ((uops & kCtrUopCntrRamLateRotMask) != 0)
        pkt_imm_val = pcnt_rotmask_counter_from_field(pcnt_addr, pkt_imm_val);
      // But always add
      imm_val_int = pcnt_addto_counter_from_field(pcnt_addr, pkt_imm_val);
    }
    if ((uops & kCtrUopIncrImm0) != 0) {
      counter_ += imm_val_int;
    }
    if ((uops & kCtrUopIncrStackImm) != 0) {
      cntrstack_increment(index, imm_val_int);
    }
    if ((uops & kCtrUopIncrStackAddrBug) != 0) {
      RMT_ASSERT(RmtObject::is_jbayXX());
      // JBay buggy behaviour - CntrRam *addr* used as incr
      RMT_ASSERT((uops & kCtrUopApplyCntrRam) != 0);
      cntrstack_increment(index, static_cast<int8_t>(pcnt_addr));
    }
    if ((uops & kCtrUopIncrStackByAddMaybe) != 0) {
      RMT_ASSERT(RmtObject::is_chip1_or_later());
      RMT_ASSERT((uops & kCtrUopApplyCntrRam) != 0);
      if (pcnt_add_to_stack(pcnt_addr))
        cntrstack_increment(index, static_cast<int8_t>(pcnt_add(pcnt_addr)));
    }
    if ((uops & kCtrUopSetShift) != 0) {
      RMT_ASSERT(RmtObject::is_chip1_or_later());
      RMT_ASSERT((uops & kCtrUopLoadPktBuf) != 0);
      uint8_t s = pkt_imm_val % 32;
      if (s == 0) s = 32;
      counter_ = ( static_cast<int8_t>(pkt_imm_val) - static_cast<int8_t>(s) ) / 32;
      *shift = static_cast<int>(s);
    }
    if ((uops & kCtrUopSetZeroNeg) != 0) {
      RMT_ASSERT((uops & kCtrUopApplyCntrRam) == 0);
      // Do nothing
    }
    if ((uops & kCtrUopClearZeroNeg) != 0) {
      RMT_ASSERT((uops & kCtrUopApplyCntrRam) != 0);
      counter_next_ = counter_;
      counter_ = 127; // So CTR_ZERO/CTR_NEG == 0
      counter_pending_ = true;
    }

    RMT_LOG(RmtDebug::verbose(), "ParserShared::counter_handle(index=%d) OP=%s "
            "\n>>>>>  END:  CTR=%d  NXT=%d %s %s  SHIFT=%d  CtrStack=0x%" PRIx64 "\n",
            index, counter_ctr_op_str(counter_ctr_op(index)), counter_, counter_next_,
            counter_pending_?"PEND":"", counter_range_err_pending_?"ERR_PEND":"",
            *shift, cntrstack_counter_data(counter_));
    return ok;
  }
  bool ParserShared::counter_handle(int index, int state_cnt,
                                    uint8_t f8_3, uint8_t f8_2,
                                    uint8_t f8_1, uint8_t f8_0, int *shift) {
    // XXX: on JBay, values passed to Counter logic are incorrect
    // and have value 0 at SOP (start of packet) when state_cnt == 0
    if (is_jbayXX() && (state_cnt == 0))
      f8_3 = f8_2 = f8_1 = f8_0 = 0;
    return counter_handle(index, f8_3, f8_2, f8_1, f8_0, shift);
  }

  void ParserShared::offset_reset() {
    offset_adjust_ = 0;
  }
  // Fixup offset_adjust_ to reflect info from ActionRAM
  void ParserShared::offset_fixup(int index) {
    if (offset_reset(index)) offset_adjust_ = 0;
    offset_adjust_ += offset_incr_val(index);
  }

  // Code to handle checksumming
  void ParserShared::checksum_reset() {
    for (int ci = 0; ci < kChecksumEngines; ci++)
      cksmeng_reset_checksum(ci);
  }
  void ParserShared::checksum_start(Phv *phv) {
    for (int ci = 0; ci < kChecksumEngines; ci++)
      cksmeng_start_checksum(ci, phv);
  }
  // Finish residual checksumming
  int ParserShared::checksum_stop(Packet *packet, Phv *phv, int pkt_furthest_pos) {
    int err = 0;
    for (int ci = 0; ci < kChecksumEngines; ci++) {
      if (cksmeng_(ci)->residual_active()) {
        uint16_t dst_phv_word = cksmeng_(ci)->dst_phv_word();
        if (phv_can_write_16b(phv, dst_phv_word)) {
          err |= cksmeng_stop_checksum(ci, packet, pkt_furthest_pos);
        } else {
          if      (!phv_valid_16b(phv, dst_phv_word)) err |= kErrDstCont;
          else if (!phv_owner_16b(phv, dst_phv_word)) err |= kErrPhvOwner;
          else                                        err |= kErrMultiWr;
        }
      }
    }
    return err;
  }
  int ParserShared::checksum_handle(int match_index, int &wrote_8b, int &wrote_16b,
                                    int &wrote_32b, int &err_cnt, int shift_from_pkt) {
    int err = 0;
    for (int ci = 0; ci < kChecksumEngines; ci++) {
      if (checksum_enable(match_index, ci)) {
        uint8_t pcksm_index = checksum_ram_addr(match_index, ci);
        if (pcksm_index < kChecksumEntries) {
          uint32_t err_curr = cksmeng_do_checksum(match_index, ci, pcksm_index,
                                                  wrote_8b, wrote_16b, wrote_32b,
                                                  shift_from_pkt);
          err |= err_curr;
          if ((err_curr & kErrCsum) != 0) err_cnt++;
        } else {
          err |= kErrCsumConfig;
        }
      }
    }
    // Check checksumming constraints
    // Tofino needs extractor 3 to be enabled in ActionRam if a checksum validation occurs
    if (!check_checksum_constraints(match_index, wrote_8b, wrote_16b, wrote_32b)) {
      err |= kErrCsumConfig;
    }
    return err;
  }
  // Update pkt priority from action ram or packet
  uint8_t ParserShared::update_priority(int index, uint8_t curr_pri) {
    uint8_t ret_pri = curr_pri;
    uint8_t upd_type = pri_upd_type(index);
    if (upd_type == 0) {
      // If pri update enabled use val_mask as literal value
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserPriority),
              "ParserShared::update_prio(index=%d,curr_pri=%d,UpdType=0)\n",
              index, curr_pri);
      if ((pri_upd_en_shr(index) & 0x1) != 0) {
        ret_pri = pri_upd_val_mask(index);
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugExit),
                "ParserShared::update_prio(index=%d,curr_pri=%d,UpdType=0 - ret_pri=%d)\n",
                index, curr_pri, ret_pri);
      }
    } else if (upd_type == 1) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserPriority),
              "ParserShared::update_prio(index=%d,curr_pri=%d,UpdType=1)\n",
              index, curr_pri);
      // get 16-bit value from packet
      uint16_t v16 = 0;
      inbuf_get(pri_upd_src(index), &v16, true, true, 0);
      // _Always_ update the priority: data missing from the buffer is replaced
      // with zeros in RTL.
      // Shift and mask and use bottom 3-bits as pri
      v16 >>= pri_upd_en_shr(index);
      v16 &= static_cast<uint16_t>(pri_upd_val_mask(index));
      ret_pri = static_cast<uint8_t>(v16 & 0x7);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserPriority),
              "ParserShared::update_prio(index=%d,curr_pri=%d,UpdType=1,V16=0x%04x - ret_pri=%d)\n",
              index, curr_pri, v16, ret_pri);
    }
    return ret_pri;
  }


  void ParserShared::error_debug(uint32_t err_flags_orig, uint32_t err_flags_final) {
    if ((err_flags_orig & kErrNoTcamMatch) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(no_tcam_match_err)\n");
    }
    if ((err_flags_orig & kErrPartialHdr) != 0) {
      const char *prefix = ((err_flags_final & kErrPartialHdr) == 0) ?"DISABLED " :"";
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "%sPKT_ERR(partial_hdr_err)\n", prefix);
    }
    if ((err_flags_orig & kErrCtrRange) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(ctr_range_err)\n");
    }
    if ((err_flags_orig & kErrTimeoutIter) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(timeout_iter_err)\n");
    }
    if ((err_flags_orig & kErrTimeoutCycle) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(timeout_cycle_err)\n");
    }
    if ((err_flags_orig & kErrSrcExt) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(src_ext_err)\n");
    }
    if ((err_flags_orig & kErrDstCont) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(dst_cont_err)\n");
    }
    if ((err_flags_orig & kErrPhvOwner) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(phv_owner_err)\n");
    }
    if ((err_flags_orig & kErrMultiWr) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(multi_write_err)\n");
    }
    if ((err_flags_orig & kErrARAMsbe) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(aram_sbe)\n");
    }
    if ((err_flags_orig & kErrARAMmbe) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(aram_mbe)\n");
    }
    if ((err_flags_orig & kErrFcs) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(fcs_err)\n");
    }
    if ((err_flags_orig & kErrCsum) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(csum_err)\n");
    }
    if ((err_flags_orig & kErrIbufOflow) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(ibuf_oflow)\n");
    }
    if ((err_flags_orig & kErrIbufUflow) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(ibuf_uflow)\n");
    }
    if ((err_flags_orig & kErrOpFifoOflow) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(op_fifo_oflow)\n");
    }
    if ((err_flags_orig & kErrOpFifoUflow) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(op_fifo_uflow)\n");
    }
    if ((err_flags_orig & kErrTcamPar) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(tcam_par)\n");
    }
    if ((err_flags_orig & kErrNarrowToWide) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(narrow_to_wide)\n");
    }
    if ((err_flags_orig & kErrCsumConfig) != 0) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseErrorDetail),
              "PKT_ERR(csum_config_err)\n");
    }
  }

  void ParserShared::adjust_hdr_len(Packet *p, int chan) {
    // Calculate the header length and update the total header byte count.
    //  - This should be updated regardless of whether the packet is dropped.
    // Note we apply an adjustment hdr_len_adj()
    // And we cap to a hdr_max_len() (programmable on JBay)
    uint16_t hdr_len = p->orig_hdr_len();
    uint16_t hdr_len_orig = hdr_len;
    hdr_len = (hdr_len > hdr_len_adj()) ? hdr_len - hdr_len_adj() : 0;
    hdr_len = (hdr_len > hdr_max_len()) ? hdr_max_len() : hdr_len;
    p->set_orig_hdr_len(hdr_len);
    set_hdr_len_inc_stopped(true); // Track we've updated hdr_len so we don't do this again
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserHdrLen),
            "ParserShared::adjust_hdr_len(chan=%d) hdr_len_adj=%d, hdr_max_len=%d, hdr_len adjusted from %d to %d\n",
            chan, hdr_len_adj(), hdr_max_len(), hdr_len_orig, hdr_len);
  }

  void ParserShared::inc_drop_count(int ci) {
    prs_main_pkt_drop_cnt_.cnt(ci, 1 + prs_main_pkt_drop_cnt_.cnt(ci));
    if (ingress()) {
      auto *counters = get_object_manager()->ipb_counters_lookup(
          pipe_index(), ipb_index(ci), ipb_chan(ci));
      if (nullptr != counters) counters->increment_chnl_parser_send_pkt_err();
    }
  }
  void ParserShared::inc_tx_count(int ci) {
    if (ingress()) {
      auto *counters = get_object_manager()->ipb_counters_lookup(
          pipe_index(), ipb_index(ci), ipb_chan(ci));
      if (nullptr != counters) counters->increment_chnl_parser_send_pkt();
    }
  }

  void ParserShared::maybe_stash_counters(Packet *p, int cycle, int8_t curr_counter) {
    // Maybe stash away cntr values into PacketPipeData
    // Cycle0 goes in bits[63:0], Cycle1 in bits[127:64] etc (upto 256 cycles)
    // See description in parser-counter-stack.h for format of 64b
    //
    uint8_t prsr_cntr_ctl = p->get_pipe_data_ctrl(ingress(), parser_index(),
                                                  PacketData::kPrsrCntr);
    if (!PacketDataCtrl::do_store(prsr_cntr_ctl)) return;

    p->set_pipe_data(ingress(), parser_index(), PacketData::kPrsrCntr,
                     64 * cycle, cntrstack_counter_data(curr_counter), 64);
  }

  // Parse packet using TCAM+actions extracting packet fields
  // to configured PHV fields
  // Allow reuse of existing PHV if passed one
  Phv *ParserShared::parse(Packet *p, int chan, Phv *prev_phv) {
    // TODO: reset & parser mode logic
    last_err_flags_ = 0;
    if ((p == NULL) || (chan >= kChannels) || (!enabled(chan))) return NULL;

    Gress   gress = ingress() ?Gress::ingress :Gress::egress;
    bool    partial_hdr_err_disabled = false, partial_header_err_proc = false;
    uint8_t pkt_pri = pri_start(chan);
    uint8_t state;
    uint8_t v8_3 = 0, v8_2 = 0, v8_1 = 0, v8_0 = 0;
    int ppos = 0, state_cnt = -1, shift = 0, shift_from_pkt = -1;
    uint8_t pkt_ver = p->version();
    bool ver0 = p->is_version_0();
    bool ver1 = p->is_version_1();
    bool finished = false;
    uint32_t err_flags = 0u;
    RmtObjectManager *objmgr = get_object_manager();
    RMT_ASSERT(objmgr != NULL);
    Phv *phv = (prev_phv != NULL) ?prev_phv :objmgr->phv_create();
    RMT_ASSERT(phv != NULL);
    phv->set_source(Phv::kFlagsSourceParser);
    p->set_orig_hdr_len(0);
    objmgr->log_packet(kType, *p);

    // XXX: figure out how many packet bytes can be examined.
    // parsable_len is the amount of the packet passed on by IPB/EPB
    // taking account of the programming of IPB/EPB prsr_dph_max CSR
    // and the IBP/EBP header len
    uint16_t pkt_len = p->len();
    uint16_t pkt_parsable_len = p->parsable_len();
    uint16_t pkt_avail_len = (pkt_len < pkt_parsable_len) ?pkt_len :pkt_parsable_len;

    // Fill the other gress with non-zero values
    // Helps show up some errors related to ownership
    if (kPhvFillOtherGress)
      phv_fill_other_gress(phv);

    parse_start(p, phv);

    // Setup buf2 with version/timestamp & if ingress setup buf1 with 16-bytes of packet,
    inbuf_setup(p);
    extract_counter_reset();
    counter_reset();
    offset_reset();
    hdr_len_inc_stopped_reset();
    checksum_start(phv);

    ppos += inbuf_fill(p, ppos); // Fill normal input buf (buf0) from packet
    inbuf0_maybe_get_initial(chan, &v8_3, &v8_2, &v8_1, &v8_0); // Get inital vals (just JBay)
    state = start_state(chan);   // Start off in initial state

    ParseContext context;
    while (!finished) {
      context.state_ = state;
      uint32_t cntr_errs, oth_errs, csum_errs, clot_errs, extract_errs;

      state_cnt++; // Tick state iteration count
      RMT_ASSERT( (state_cnt <= 0xFFFF) && "Too many iterations!" );

      // Many break statements within while loop so easiest to
      // call parse_end_state:
      // a) here (for previous state_cnt) AND
      // b) after while loop exits.
      // NB here arg4==false ==> NOT final state
      if (state_cnt > 0) parse_end_state(p, phv, state_cnt-1, false);

      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseLoop),
              "ParserShared::parse(state=%d,v8_3=%02x,v8_2=%02x,"
              "v8_1=%02x,v8_0=%02x,cnt=%d,pri=%d)%s%s%s%s\n",
              state, v8_3, v8_2, v8_1, v8_0, state_cnt, pkt_pri,
              ver1?" V1":"",ver0?" V0":"",(counter_>0)?" GT0":"",(counter_<0)?" LT0":"");
      RMT_P4_LOG_INFO("%s Parser state '%s'\n", gress_str(),
                      rmt_log_parser_state(parser_index(), egress(), state).c_str());
      objmgr->log_parser_state(p->pkt_id(), pipe_index(), gress, state);


      //
      // For ordering of error checks see email thread 1Mar2018
      // "Harlyn parser: what happens to payload offset when a parsing error occurs"
      //

      if (state_cnt >= max_iter()) {
        // ERROR(config) - too many states - loop?
        err_flags |= kErrTimeoutIter;
        break;
      }

      // Now call parse_start_state
      parse_start_state(p, phv, state_cnt);


      // Disabled this log for XXX
      //RMT_P4_LOG_VERBOSE(
      //    "ParserShared::key:state=%d, v8_3=%02x, v8_2=%02x, v8_1=%02x, v8_0=%02x "
      //    " cnt=%d, ver1=%d, ver0=%d\n",
      //    state, v8_3, v8_2, v8_1, v8_0, counter_, ver1, ver0);

      // Lookup state + packet bytes
      BitVector<44> bvLookup = make_tcam_entry(ver1, ver0,
                                               (counter_ == 0), (counter_ < 0),
                                               state, v8_3, v8_2, v8_1, v8_0);
      int index = tcam_match_.tcam_lookup(bvLookup);
      context.index_ = index;

      // Fetch shift early - it may be overidden on WIP
      shift = (index < 0) ?0 :shift_amount(index);
      shift_from_pkt = -1; // XXX: initialise every parse cycle

      // Do counter handling - pass match index - also pass fields
      // we looked up in case we're using them to initialize counter
      //
      // Just *record* cntr_errs at this point.
      // They will be reported alongside any NoTcamMatch/PartialHdr error
      //
      cntr_errs = (counter_handle(index, state_cnt,
                                  v8_3, v8_2, v8_1, v8_0,
                                  &shift_from_pkt)) ?0u :kErrCtrRange;
      err_flags |= cntr_errs;
      if (shift_from_pkt >= 0) shift = shift_from_pkt;

      // Maybe stash away cntr values (inc cntrstack) into PacketPipeData
      maybe_stash_counters(p, state_cnt, counter_);

      if (index < 0) {
        // ERROR(config) - should always be a matching state
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseNoMatch),
                "ParserShared::parse(state=%d,cnt=%d) NO MATCH!!!!!!!\n",
                state, state_cnt);
        err_flags |= kErrNoTcamMatch;
        RMT_LOG(RmtDebug::verbose(), "%s\n",
                bvLookup.to_string().c_str());
        break;
      }
      objmgr->log_parser_tcam_match(p->pkt_id(), pipe_index_, gress, index,
                                    bvLookup.to_string());


      // Complain if shift_amount exceeds buf_req (part of PartialHdrFixes)
      if (shift > buf_req(index)) {
        RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserParseError),
                "ParserShared::parse(state=%d,cnt=%d) "
                "index=%d ShiftAmt(%d) exceeds BufReq(%d)\n",
                state, state_cnt, index, shift, buf_req(index));
      }

      if (inbuf_getpos(buf_req(index)) > pkt_avail_len) {
        err_flags |= kErrPartialHdr;
        // XXX - record whether Partial Header Error disabled in this state
        partial_hdr_err_disabled = disable_partial_hdr_err(index);
        partial_header_err_proc = partial_hdr_err_proc(index);  // WIP only
        // Break Partial Header Error and JBay (part of PartialHdrFixes)
        // XXX: when disable_partial_hdr_err is set defer break until
        // checksum is done
        if (is_jbay_or_later() &&
            !(partial_hdr_err_disabled || partial_header_err_proc)) break;
      }

      // If cntr_errs occurred earlier but they were NOT reported
      // alongside a NoTcamMatch or PartialHdr error we break now
      if (cntr_errs != 0u) break;

      // Check for any other errors that may prevent extraction occurring
      // eg SrcExt/PhvOwner/MultiWr (currently only break now on JBay)
      oth_errs = other_early_error_checks(index);
      err_flags |= oth_errs;
      if (oth_errs != 0u) {
        if (is_jbay_or_later()) break;
      }

      // Do checksum calc now - defer offset_adjust_ calc
      // till end of loop
      int csum_ext_cnt_8b = 0, csum_ext_cnt_16b = 0, csum_ext_cnt_32b = 0;
      int err_cnt = 0;
      csum_errs = checksum_handle(index,
                                  csum_ext_cnt_8b, csum_ext_cnt_16b, csum_ext_cnt_32b,
                                  err_cnt, shift_from_pkt);
      err_flags |= csum_errs;
      // At VS request (03/10/14) continue on checksum error
      //if ((err_flags & kErrCsum) != 0) break;
      // Increment the checksum counter (multiple errors may be generated per cycle)
      inc_cntr(err_cnt, prs_main_csum_err_cnt_);
      // XXX: when partial hdr error occurs and disable_partial_hdr_err
      // is set break after checksum
      if (is_jbay_or_later() &&
          partial_hdr_err_disabled &&
          !partial_header_err_proc) break;

      //
      // From here on we will be updating pkt offset etc so
      // spit out some debug
      //
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseMatch),
              "ParserShared::parse(state=%d,cnt=%d) MATCH index=%d OffsetAdj=%d Cntr=%d\n",
              state, state_cnt, index, offset_adjust_, counter_);

      // Use indexed offsets to fill in v8_3, v8_2, v8_1, v8_0
      //
      // NB 1. get_lookup_offset allows us to behave as if we're always capable
      //       of extracting 4 bytes from the data buffer (rather than 2 bytes
      //       and 1 short) irrespective of PER-CHIP capabilties
      //
      // NB 2. MSB is used to select which buf (buf0/buf1) to get bytes from
      //       (get_lookup_offset returns MSB in v_which_buf)
      //       *** TOFINO ***
      //       If MSB set we get bytes from buf1 which is fixed copy of 1st 16 bytes pkt
      //       If MSB set we expect to be getting bytes in range [32,47] or [48,63].
      //          (buf1 has read_pos_mod set to 16 so we'll mod it into range)
      //       Since buf1 only stores 16-bytes pkt ONCE 2-byte reads [47-48] [63-64]
      //          automaticaly get the second byte 0
      //
      // NB 3. NO reinit - use vals from previous iteration if load not set
      //       v8_3 = 0; v8_2 = 0; v8_1 = 0; v8_0 = 0;
      //
      // Always retrieve the lookup fields, even when there is insufficient
      // data in the buffer. The RTL only compares buf_req when identifying
      // partial header errors.
      //
      int      v_which_buf;
      bool     v_load;
      uint8_t  v_off;

      get_lookup_offset(index, 0, &v_off, &v_load, &v_which_buf);
      inbuf_get(v_off, &v8_0, v_load, false, v_which_buf);

      get_lookup_offset(index, 1, &v_off, &v_load, &v_which_buf);
      inbuf_get(v_off, &v8_1, v_load, false, v_which_buf);

      get_lookup_offset(index, 2, &v_off, &v_load, &v_which_buf);
      inbuf_get(v_off, &v8_2, v_load, false, v_which_buf);

      get_lookup_offset(index, 3, &v_off, &v_load, &v_which_buf);
      inbuf_get(v_off, &v8_3, v_load, false, v_which_buf);


      // Now we go on to extract words to PHV since all inbuf_gets
      // above SUCCEEDED. From now on we also DO update pkt offset.

      // MATCH index used to get vals from EarlyActionRAM

      state = next_state(index, state);
      //shift = shift_amount(index); // Now before counter_handle()
      finished = done(index);

      // Possibly re-fill inbuf1 (scratch pad buffer) from inbuf0 OR
      // using some/all of our match bytes (JBay-only feature)
      inbuf1_maybe_fill(index, v8_3, v8_2, v8_1, v8_0, state_cnt);

      // MATCH index used to get vals from ActionRAM

      // Update packet priority using ActionRAM
      // This could read another 16-bits from packet
      pkt_pri = update_priority(index, pkt_pri);

      // Update pkt_ver using ActionRAM (JBay only)
      // Could also read another 16-bits from packet
      pkt_ver = update_version(index, pkt_ver);

      // Possibly update CLOT info within packet (JBay only)
      clot_errs = clot_handle(index, p);
      err_flags |= clot_errs;
      if (clot_errs != 0u) err_cnt++;

      if (is_tofinoXX()) {
        // XXX: Tofino: Mimic worse case parser arbiter fifos occupancy in
        // order to handle n2w extraction constraints properly.
        //
        // Update FIFO occupancies _before_ the extract, since extracts can't
        // occur when FIFOs are full.
        ExtractInfo &x = extract_info_;
        const uint32_t FULL_THRESH = kExtractArbFifoSize - kExtractMax_8b_16b_32b;

        // Decrement (read) arbiter FIFOs occupancy if any of the fifos has reached
        // the FULL threshold (FIFO size - max number of bytes extracted per state).
        // Decrement both the worst case (wc) and the narrow-to-wide (n2w) counts.
        // There are only one set of FIFOs. In the worst case, we only read those when
        // we hit a full condition in one of the FIFOs.
        if ((x.n_8b_wc_fifo_occ_ > FULL_THRESH) ||
            (x.n_16b_wc_fifo_occ_ > FULL_THRESH) ||
            (x.n_32b_wc_fifo_occ_ > FULL_THRESH)) {
          arbiter_fifo_occ_read(x.n_8b_wc_fifo_occ_, x.n_16b_wc_fifo_occ_, x.n_32b_wc_fifo_occ_);
        }

        // Verify whether the outstanding pad state counts are met
        //
        // Padding requirements:
        //  8b -> 32b n2w extracts : pad 16b extracts
        //  16b -> 32b n2w extracts : pad 8b extracts
        if (x.n_8b_pad_states_needed_ == 0)
            x.n2w_16b_32b_recently_seen_ = false;
        if (x.n_16b_pad_states_needed_ == 0)
            x.n2w_8b_32b_recently_seen_ = false;

        x.n2w_8b_in_curr_state_ = false;
        x.n2w_16b_in_curr_state_ = false;
      }

      // Use indexed vals to extract 8/16/32 bit words to phv
      extract_errs = handle_extract(index, phv, p->pkt_id(), csum_ext_cnt_8b,
                                    csum_ext_cnt_16b, csum_ext_cnt_32b, &context,
                                    (err_flags & kErrPartialHdr) != 0);
      // XXX: These final checks now occurs within handle_extract
      //inbuf_extract_final_checks(index, phv, state_cnt);

      if (is_tofinoXX()) {
        ExtractInfo &x = extract_info_;

        if (x.n2w_8b_in_curr_state_ || x.n2w_16b_in_curr_state_) {
          // Calculate number of padded states needed to guarantee that the
          // narrow-to-wide extract will be correctly read from the FIFO. (Read
          // in one cycle, and placed on the same buses.)
          //
          // A narrow-to-wide extract with 8b extractors requires padding on
          // the 16b buses, and an narrow-to-wide extract with 16b buses
          // requires padding on the 8b buses.
          //
          // No ceiling needed because occupancies should be integer multiples of pad size.
          if (x.n2w_8b_in_curr_state_)  x.n_16b_pad_states_needed_ = x.n_8b_wc_fifo_occ_  / 4;
          if (x.n2w_16b_in_curr_state_) x.n_8b_pad_states_needed_  = x.n_16b_wc_fifo_occ_ / 2;
        }
      }

      if (extract_errs != 0u) {
        // ERROR(pkt) - couldn't extract bytes to write to PHV
        // OR multi-write/phv owner/dst cont error
        err_flags |= extract_errs;
        // DONT break now - advance shift then break (see XXX)
      }

      // ERROR(pkt) - not enough bytes to satisfy shift
      // TODO: is this always an error
      // Removed as part of PartialHdrFixes
      //if (!shift_ok) err_flags |= kErrPartialHdr;

      // Calculate the header length and update the total header byte count.
      // NB. On JBay hdr_len_inc may stop at an arbitrary point
      //     in which case this function perform the hdr_len update immediately
      //     On Tofino the hdr_len update *only* occurs when parse is complete
      update_hdr_len(p, index, err_flags, shift);

      // Apply shift (even if there were extract errors)
      // XXX: should occur *after* update_hdr_len
      bool shift_ok = (inbuf_shift(shift) == shift);

      if (shift_ok) {
        // Break now if there were any extract errors or critical checksum errors
        if ((extract_errs != 0u) || ((csum_errs & ~kErrCsum) != 0u)) break;
        // Otherwise refill buf and continue
        ppos += inbuf_fill(p, ppos);
      } else {
        // Break now if there was a shift error
        // So a partial_hdr detected on shift causes a break prior to offset_fixup
        break;
      }

      // Now do offset adjustment
      offset_fixup(index);

      // Break on partial header error
      // (will have done so earlier if JBay)
      if ((err_flags & kErrPartialHdr) != 0u) break;

    } // while (!finished)

    // NB arg4==true ==> FINAL iteration
    parse_end_state(p, phv, state_cnt, true);


    // XXX: priority mapping is unconditional regardless of error
    //
    // Map pkt priority (JBay/WIP only)
    pkt_pri = map_priority(chan, pkt_pri);
    // Update packet priority
    p->set_priority(pkt_pri);

    // Always do these steps on Tofino, but only if NO error on JBay
    // (see email thread "Harlyn parser: what happens ..." 1 Mar 18)
    //
    // Calculate the header length and update the total header byte count.
    // XXX   - PartialHdrErr still visible at this point
    // XXX - call unconditionally - hdr_len must be updated
    if ((!is_chip1_or_later()) &&
        (!is_hdr_len_inc_stopped()) &&
        ((err_flags & kErrPartialHdr) != 0u)) {
      // RTL marks the entire packet as header in case of partial header error.
      // XXX: actually only the pkt chunk passed on by IPB/EPB (+ hdrs)
      p->set_orig_hdr_len(pkt_avail_len);
    }

    // XXX: cksum validation error does not preclude other actions
    uint32_t filtered_error_flags = err_flags & (~kErrCsum);
    if (is_tofinoXX() ||
        (filtered_error_flags == 0) ||
        ((filtered_error_flags == kErrPartialHdr) && partial_header_err_proc) ||
        ((err_flags == kErrPartialHdr) && partial_hdr_err_disabled)) {
      // We do this last as it (ab)uses inbuf so messes up getmaxpos()
      // XXX: Prior to WIP, residual checksum calculation
      // incorrectly continued to end of headers even when header length
      // increments had been stopped. With WIP, the residual should be
      // calculated up to and including any final amount added to the header
      // length when header length increments stop.
      int pkt_furthest_pos = is_chip1_or_later() ? p->orig_hdr_len() : inbuf_getmaxpos();
      err_flags |= checksum_stop(p, phv, pkt_furthest_pos);
    }

    // At this point pkt orig_hdr_len includes metadata prepended to the packet
    // by IPB; this metadata is not visible to the deparser, so orig_hdr_len
    // must be adjusted downwards by hdr_len_adj.
    // XXX: adjust orig_hdr_len *after* automatic residual checksum calc
    // has completed because WIP uses the pkt orig_hdr_len to determine the
    // position in the parser buffer at which residual calculation should
    // finish.
    // [ metadata ][ packet                                           ]
    // <---------------- orig_hdr_len -------------------->
    adjust_hdr_len(p, chan);
    // [ metadata ][ packet                                           ]
    //             <---- orig_hdr_len -------------------->
    inc_hdr_byte_count(chan, p->orig_hdr_len());
    inc_rx_count(chan);

    // Force valid bigs
    phv_valid_force(phv);


    // TODO: interrupt handling??
    // Packet ERRORS possible on:
    // a) PHV field extract fail - byte/short/int not avail
    // b) match field extract fail
    // c) unable to shift configured amount
    // d) FCS err/CSUM err
    //
    // Config ERRORS possible on:
    // e) failure to find match in TCAM
    // f) too many cycles within parser
    // g) MEM err


    // Stash err_flags now - we might be disabling some errors
    uint32_t err_flags_orig = err_flags;

    if (err_flags != 0u) {
      if (partial_hdr_err_disabled) err_flags &= ~kErrPartialHdr;
      // XXX - Partial Header Errors debugged, counted, and
      // go into IntrStatus but do NOT get output into PHV.
      // (so PHV might not get dropped here)

      error_debug(err_flags_orig, err_flags);
      perr_count(err_flags_orig);
      perr_interrupt_output(err_flags_orig);

      // XXX - only ever drop on ingress
      if ((err_flags != 0u) &&
          (!perr_phv_output(chan, err_flags, phv)) &&
          (ingress())) {
        // If not setup to stash error in Phv, and this is
        // ingress parser then assume we're to drop it.
        // Parser never drops on egress.
        objmgr->phv_delete(phv);
        phv = NULL;
      }
    }

    if ((p->qing2e_metadata()->ing_congested() || congested_[chan]) &&
        (ingress()) && (pkt_pri < pri_thresh(chan)) && (phv != NULL)) {
      // If packet experienced congestion OR
      // channel has been set globally as congested
      // and this is ingress parser and the pkt_pri
      // is less than the threshold value for this chan
      // then drop the packet. Parser never drops on egress.
      objmgr->phv_delete(phv);
      phv = NULL;
    }

    const char *xtrastr = (partial_hdr_err_disabled) ?"PartialHdrErr DISABLED" :"";
    const char *errstr  = (kRelaxFinalParseCheck) ?"error" :"ERROR";
    if (err_flags == 0u) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseOK|RmtDebug::kRmtDebugExit),
              "ParserShared::parse(OK) Chan=%d StateCnt=%d OffsetAdj=%d Cntr=%d Phv=%p %s\n",
              chan, state_cnt, offset_adjust_, counter_, phv, xtrastr);
    } else {
      bool relax = kRelaxFinalParseCheck;
      const char *errstrs = (kRelaxFinalParseCheck) ?"errors" :"ERRORS";
      RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugParserParseError|RmtDebug::kRmtDebugExit,relax),
              "ParserShared::parse(%s) Chan=%d StateCnt=%d OffsetAdj=%d Cntr=%d "
              "%s=0x%04x[%s] Phv=%p %s\n",
              errstr, chan, state_cnt, offset_adjust_, counter_, errstrs, err_flags,
              errstr_get(err_flags), phv, xtrastr);
    }
    if (ingress()) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugParserParseOK|RmtDebug::kRmtDebugExit),
              "ParserShared::parse(%s) Chan=%d PktPri=%d,ChanPri=%d "
              "PktCongested=%d,ChanCongested=%d Phv=%p\n",
              err_flags == 0u ?"OK" :errstr, chan, pkt_pri, pri_thresh(chan),
              p->qing2e_metadata()->ing_congested(), congested_[chan], phv);
    }

    if (phv != NULL) {
      if (ingress()) phv->set_ingress(); else phv->set_egress();
      phv->set_pipe(pipe_index());
      phv->print_p("Parsing PHV completed\n", false);
      clot_print(p);
      p->set_phv(phv);
      phv->set_packet(p);
      // Update PHV version from pkt_ver (may have been updated if JBay)
      phv->set_version(pkt_ver, ingress());
      phv->set_cache_id();
      //prs_fe_rspec_.pkt_cnt disappeared in registers 20141007_170846_2869
      //inc_packet_count(chan);
      phv->set_source(0);
      inc_tx_count(chan);
      log_phv(*phv);
    }
    else {
      inc_drop_count(chan);
    }
    last_err_flags_ = err_flags;
    return phv;
  }


  // Special funcs for DV to allow IPB processing of packet prior to Parse
  Phv *ParserShared::ipb_parse(Packet *packet, int chan, uint32_t flags,
                               uint8_t resubmit_flag, uint8_t version,
                               uint16_t logical_port, uint64_t timestamp,
                               uint64_t meta1[], Phv *prev_phv) {
    RMT_ASSERT(io_index_ == 0); // Must be Ingress parser
    Ipb *ipb = get_object_manager()->ipb_lookup(pipe_index_, ipb_index(chan));
    packet = ipb->prepend_metadata_hdr(packet, ipb_chan(chan), flags,
                                       resubmit_flag, version, logical_port,
                                       timestamp, meta1);
    return parse(packet, chan, prev_phv);
  }
  Phv *ParserShared::ipb_parse(Packet *packet, int chan, Phv *prev_phv) {
    RMT_ASSERT(io_index_ == 0); // Must be Ingress parser
    Ipb *ipb = get_object_manager()->ipb_lookup(pipe_index_, ipb_index(chan));
    packet = ipb->prepend_metadata_hdr(packet, ipb_chan(chan), 0u);
    return parse(packet, chan, prev_phv);
  }



}
