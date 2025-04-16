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
#include <parser.h>
#include <checksum-engine.h>


namespace MODEL_CHIP_NAMESPACE {

  ChecksumEngineShared::ChecksumEngineShared(RmtObjectManager *om,
                                             int pipeIndex, int ioIndex, int prsIndex,
                                             int engineIndex, int ramIndex,
                                             Parser *parser)
      : PipeObject(om, pipeIndex, prsIndex, kType, engineIndex, 0x3F),
        prs_csum_ctrl_(prsr_mem_adapter(prs_csum_ctrl_, chip_index(),
                                        pipeIndex, ioIndex, prsIndex, ChecksumEngine::get_csum_r01234(ramIndex),
                                        [this,ramIndex](uint32_t i){this->memory_change_callback(ParserMemoryType::kChecksum,ramIndex,i);})),
        engine_index_(engineIndex), ram_index_(ramIndex), parser_(parser) {

    RMT_ASSERT(engineIndex == ramIndex);
    reset_running_ = true;
    prs_csum_ctrl_.reset();
    reset_checksum();
    reset_running_ = false;
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCreate),
            "ChecksumEngine::create\n");
  }
  ChecksumEngineShared::~ChecksumEngineShared() {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDelete),
            "ChecksumEngine::delete\n");
  }

  // Called whenever a 'Parser' memory is written (here ChecksumRAM) - typically overridden in subclass
  void ChecksumEngineShared::memory_change_callback(uint8_t mem_type, uint8_t mem_inst, uint32_t mem_index) {
    if (reset_running_) return;
    // Hand off memory change handling to Parser
    parser_->memory_change_callback(mem_type, mem_inst, mem_index);
  }

  // Do checksumming of inbuf
  void ChecksumEngineShared::checksum_calc_fast_b0b1(int b0_pos, int b1_pos, bool mask0, bool mask1,
                                                     bool swap, bool mul2, bool rotL) {
    uint8_t b0 = 0, b1 = 0;
    if (mask0) (void)parser_->inbuf_get(b0_pos, &b0, true);
    if (mask1) (void)parser_->inbuf_get(b1_pos, &b1, true);
    uint32_t val16 = (((uint32_t)(swap ?b1 :b0) << 8) | (uint32_t)(swap ?b0 :b1));
    if (mul2) val16 <<= 1;
    val_ += (rotL) ? (model_common::Util::rotl16(val16, 1)) : (val16);
    folded_val_ += (rotL) ? (model_common::Util::rotl16(val16, 1)) : (val16);
    folded_val_ = ones_cmpl_fold(folded_val_);
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCsumEngCalcFastB0B1),
            "ChecksumEngine::checksum_calc_fast_b0b1[%d]: "
            "val=0x%04x, b0_pos=%2d, b1_pos=%2d, b0=0x%02x, b1=0x%02x, mask0=%d, mask1=%d, swap=%d, mul2=%d, rotL=%d\n",
            engine_index_, folded_val_, b0_pos, b1_pos, b0, b1, mask0, mask1, swap, mul2, rotL);
  }
  void ChecksumEngineShared::checksum_calc_fast_32(uint32_t mask, uint32_t swap, uint32_t mul2, uint32_t rotL) {
    checksum_calc_fast_b0b1( 0, 1, (mask & 0x00000001), (mask & 0x00000002), (swap & 0x00001), (mul2 & 0x00001), (rotL & 0x00001));
    checksum_calc_fast_b0b1( 2, 3, (mask & 0x00000004), (mask & 0x00000008), (swap & 0x00002), (mul2 & 0x00002), (rotL & 0x00002));
    checksum_calc_fast_b0b1( 4, 5, (mask & 0x00000010), (mask & 0x00000020), (swap & 0x00004), (mul2 & 0x00004), (rotL & 0x00004));
    checksum_calc_fast_b0b1( 6, 7, (mask & 0x00000040), (mask & 0x00000080), (swap & 0x00008), (mul2 & 0x00008), (rotL & 0x00008));
    checksum_calc_fast_b0b1( 8, 9, (mask & 0x00000100), (mask & 0x00000200), (swap & 0x00010), (mul2 & 0x00010), (rotL & 0x00010));
    checksum_calc_fast_b0b1(10,11, (mask & 0x00000400), (mask & 0x00000800), (swap & 0x00020), (mul2 & 0x00020), (rotL & 0x00020));
    checksum_calc_fast_b0b1(12,13, (mask & 0x00001000), (mask & 0x00002000), (swap & 0x00040), (mul2 & 0x00040), (rotL & 0x00040));
    checksum_calc_fast_b0b1(14,15, (mask & 0x00004000), (mask & 0x00008000), (swap & 0x00080), (mul2 & 0x00080), (rotL & 0x00080));
    checksum_calc_fast_b0b1(16,17, (mask & 0x00010000), (mask & 0x00020000), (swap & 0x00100), (mul2 & 0x00100), (rotL & 0x00100));
    checksum_calc_fast_b0b1(18,19, (mask & 0x00040000), (mask & 0x00080000), (swap & 0x00200), (mul2 & 0x00200), (rotL & 0x00200));
    checksum_calc_fast_b0b1(20,21, (mask & 0x00100000), (mask & 0x00200000), (swap & 0x00400), (mul2 & 0x00400), (rotL & 0x00400));
    checksum_calc_fast_b0b1(22,23, (mask & 0x00400000), (mask & 0x00800000), (swap & 0x00800), (mul2 & 0x00800), (rotL & 0x00800));
    checksum_calc_fast_b0b1(24,25, (mask & 0x01000000), (mask & 0x02000000), (swap & 0x01000), (mul2 & 0x01000), (rotL & 0x01000));
    checksum_calc_fast_b0b1(26,27, (mask & 0x04000000), (mask & 0x08000000), (swap & 0x02000), (mul2 & 0x02000), (rotL & 0x02000));
    checksum_calc_fast_b0b1(28,29, (mask & 0x10000000), (mask & 0x20000000), (swap & 0x04000), (mul2 & 0x04000), (rotL & 0x04000));
    checksum_calc_fast_b0b1(30,31, (mask & 0x40000000), (mask & 0x80000000), (swap & 0x08000), (mul2 & 0x08000), (rotL & 0x08000));
  }
  void ChecksumEngineShared::checksum_calc_fast_32_rot(uint32_t mask, uint32_t swap, uint32_t mul2, uint32_t rotL) {
    checksum_calc_fast_b0b1(-1, 0,               false, (mask & 0x00000001), (swap & 0x00001), (mul2 & 0x00001), (rotL & 0x00001));
    checksum_calc_fast_b0b1( 1, 2, (mask & 0x00000002), (mask & 0x00000004), (swap & 0x00002), (mul2 & 0x00002), (rotL & 0x00002));
    checksum_calc_fast_b0b1( 3, 4, (mask & 0x00000008), (mask & 0x00000010), (swap & 0x00004), (mul2 & 0x00004), (rotL & 0x00004));
    checksum_calc_fast_b0b1( 5, 6, (mask & 0x00000020), (mask & 0x00000040), (swap & 0x00008), (mul2 & 0x00008), (rotL & 0x00008));
    checksum_calc_fast_b0b1( 7, 8, (mask & 0x00000080), (mask & 0x00000100), (swap & 0x00010), (mul2 & 0x00010), (rotL & 0x00010));
    checksum_calc_fast_b0b1( 9,10, (mask & 0x00000200), (mask & 0x00000400), (swap & 0x00020), (mul2 & 0x00020), (rotL & 0x00020));
    checksum_calc_fast_b0b1(11,12, (mask & 0x00000800), (mask & 0x00001000), (swap & 0x00040), (mul2 & 0x00040), (rotL & 0x00040));
    checksum_calc_fast_b0b1(13,14, (mask & 0x00002000), (mask & 0x00004000), (swap & 0x00080), (mul2 & 0x00080), (rotL & 0x00080));
    checksum_calc_fast_b0b1(15,16, (mask & 0x00008000), (mask & 0x00010000), (swap & 0x00100), (mul2 & 0x00100), (rotL & 0x00100));
    checksum_calc_fast_b0b1(17,18, (mask & 0x00020000), (mask & 0x00040000), (swap & 0x00200), (mul2 & 0x00200), (rotL & 0x00200));
    checksum_calc_fast_b0b1(19,20, (mask & 0x00080000), (mask & 0x00100000), (swap & 0x00400), (mul2 & 0x00400), (rotL & 0x00400));
    checksum_calc_fast_b0b1(21,22, (mask & 0x00200000), (mask & 0x00400000), (swap & 0x00800), (mul2 & 0x00800), (rotL & 0x00800));
    checksum_calc_fast_b0b1(23,24, (mask & 0x00800000), (mask & 0x01000000), (swap & 0x01000), (mul2 & 0x01000), (rotL & 0x01000));
    checksum_calc_fast_b0b1(25,26, (mask & 0x02000000), (mask & 0x04000000), (swap & 0x02000), (mul2 & 0x02000), (rotL & 0x02000));
    checksum_calc_fast_b0b1(27,28, (mask & 0x08000000), (mask & 0x10000000), (swap & 0x04000), (mul2 & 0x04000), (rotL & 0x04000));
    checksum_calc_fast_b0b1(29,30, (mask & 0x20000000), (mask & 0x40000000), (swap & 0x08000), (mul2 & 0x08000), (rotL & 0x08000));
    checksum_calc_fast_b0b1(31,-1, (mask & 0x80000000),               false, (swap & 0x10000), (mul2 & 0x10000), (rotL & 0x10000));
  }
  void ChecksumEngineShared::checksum_calc_slow(uint32_t mask,
                                                uint32_t swap, uint32_t mul2,
                                                uint32_t rotL, int size,
                                                int start) {
    uint8_t b0, b1, bTmp;
    for (int i = start; i < size; i += 2) {
      b0 = 0; b1 = 0;
      if ((mask & 0x1) == 0x1) (void)parser_->inbuf_get(i, &b0, true);
      if ((mask & 0x2) == 0x2) (void)parser_->inbuf_get(i+1, &b1, true);
      if (swap & 0x1) { bTmp = b1; b1 = b0; b0 = bTmp; }
      uint32_t val16 = (((uint32_t)b0 << 8) | (uint32_t)b1);
      if (mul2) val16 <<= 1;
      val_ += (rotL & 0x1) ? (model_common::Util::rotl16(val16, 1)) : (val16);
      folded_val_ += (rotL & 0x1) ? (model_common::Util::rotl16(val16, 1)) : (val16);
      folded_val_ = ones_cmpl_fold(folded_val_);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCsumEngCalcSlow),
              "ChecksumEngine::checksum_calc_slow[%d]    : "
              "val=0x%04x, b0_pos=%2d, b1_pos=%2d, b0=0x%02x, b1=0x%02x, "
              "mask0=%d, mask1=%d, swap=%d, mul2=%d, rotL=%d\n",
              engine_index_, folded_val_, i, i+1, b0, b1,
              ((mask & 0x1) == 0x1), ((mask & 0x2) == 0x2), swap, mul2, rotL);
      mask >>= 2; swap >>= 1; mul2 >>= 1; rotL >>= 1;
    }
  }
  void ChecksumEngineShared::checksum_calc_slow_rot(uint32_t mask,
                                                    uint32_t swap, uint32_t mul2,
                                                    uint32_t rotL, int size) {
    uint8_t b0, b1, bTmp;
    // Special case handling for first byte pair (ZERO|byte0)
    b0 = 0; b1 = 0;
    if ((mask & 0x1) == 0x1) (void)parser_->inbuf_get(0, &b1, true);
    if (swap & 0x1) { bTmp = b1; b1 = b0; b0 = bTmp; }
    uint32_t val16 = (((uint32_t)b0 << 8) | (uint32_t)b1);
    if (mul2) val16 <<= 1;
    val_ += (rotL & 0x1) ? (model_common::Util::rotl16(val16, 1)) : (val16);
    folded_val_ += (rotL & 0x1) ? (model_common::Util::rotl16(val16, 1)) : (val16);
    folded_val_ = ones_cmpl_fold(folded_val_);
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCsumEngCalcSlow),
            "ChecksumEngine::checksum_calc_slow_rot[%d]: "
            "val=0x%04x, b0_pos=%2d, b1_pos=%2d, b0=0x%02x, b1=0x%02x, "
            "mask0=%d, mask1=%d, swap=%d, mul2=%d, rotL=%d\n",
            engine_index_, folded_val_, -1, 0, b0, b1, 0, ((mask & 0x1) == 0x1), swap, mul2, rotL);

    checksum_calc_slow(mask >> 1, swap >> 1, mul2 >> 1, rotL >> 1, size, 1);
  }
  void ChecksumEngineShared::checksum_calc(bool rotbuf, uint32_t mask,
                                           uint32_t swap, uint32_t mul2,
                                           uint32_t rotL, uint16_t add,
                                           int size, bool zeros_as_ones,
                                           uint8_t zeros_as_ones_pos) {
    // Adjust the mask when zeros_as_ones is in effect and the two bytes are zeros.
    uint32_t mask_adj = mask;
    if (zeros_as_ones && zeros_as_ones_pos + 1 < size) {
      uint8_t b0 = 0, b1 = 0;
      (void)parser_->inbuf_get(zeros_as_ones_pos, &b0, true);
      (void)parser_->inbuf_get(zeros_as_ones_pos+1, &b1, true);
      if (b0 == 0xff && b1 == 0xff) {
        mask_adj &= ~(0x3 << zeros_as_ones_pos);
      }
    }
    if (size == 32) {
      if (rotbuf) {
        checksum_calc_fast_32_rot(mask_adj, swap, mul2, rotL);
      } else {
        checksum_calc_fast_32(mask_adj, swap, mul2, rotL);
      }
    } else {
      if (rotbuf) {
        checksum_calc_slow_rot(mask_adj, swap, mul2, rotL, size);
      } else {
        checksum_calc_slow(mask_adj, swap, mul2, rotL, size);
      }
    }
    val_ += add;
    folded_val_ += add;
    folded_val_ = ones_cmpl_fold(folded_val_);
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCsumEngCalcFin),
            "ChecksumEngine::checksum_calc[%d]: "
            "val=0x%x, rotbuf=%d, mask=0x%x, swap=0x%x, mul2=0x%x, rotl=0x%x, add=0x%x, size=%d\n",
            engine_index_, folded_val_, rotbuf, mask, swap, mul2, rotL, add, size);
  }
  void ChecksumEngineShared::checksum_finalize(bool invert) {
    uint32_t input_val = folded_val_;
    // Fold to get the ones-complement result
    val_ = ones_cmpl_fold(val_);
    // Check that result is the same as folded_val_ we calculate each cycle
    RMT_ASSERT(val_ == folded_val_);
    // Maybe invert to get the negative in ones-complement arithmetic
    if (invert) { val_ = ~val_ & 0xFFFFu; folded_val_ = ~folded_val_ & 0xFFFFu; }
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCsumEngCalcFin),
            "ChecksumEngine::checksum_finalize[%d]: input val=0x%x final%s val=0x%x\n",
            engine_index_, input_val, (invert)?" inverted":"", folded_val_);
  }

  void ChecksumEngineShared::checksum_calc_residual(int start_pos,
                                                    int num_bytes) {
    RMT_ASSERT((0 <= start_pos) && (start_pos < 32));
    RMT_ASSERT(num_bytes >= 0);
    RMT_ASSERT(start_pos + num_bytes <= 32);
    uint32_t mask;
    if (num_bytes == 0) {
      mask = 0;
    } else {
      mask = ~((1u << start_pos) - 1);
      mask &= (0xFFFFFFFFu >> (32u - (start_pos + num_bytes)));
    }
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCsumEngCalcFin),
            "ChecksumEngine::checksum_calc_residual[%d]: input val=0x%x "
            "Checksumming inbuf bytes [%d,%d) mask=0x%x rotbuf=%s\n",
            engine_index_, folded_val_, start_pos, start_pos+num_bytes, mask,
            final_rotbuf_ ? "true" : "false");
    checksum_calc(final_rotbuf_ ^ is_odd(start_pos),
                  mask, 0u, 0u, 0u, 0u,
                  std::max(start_pos + num_bytes, 2));
    final_rotbuf_ ^= is_odd(num_bytes);
    final_pos_ += num_bytes;
  }

  void ChecksumEngineShared::checksum_residual(Packet *packet, int pkt_furthest_pos) {
    // (Ab)use inbuf to checksum stuff between final_pos_
    // and furthest_pos ever loaded (or inbuf_got())
    int pkt_start_pos = final_pos_ + 1;
    while (pkt_start_pos < pkt_furthest_pos) {
      parser_->inbuf_reset();
      int size = parser_->inbuf_fill(packet, pkt_start_pos);
      size = std::min(size, pkt_furthest_pos - pkt_start_pos);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCsumEngCalcFin),
              "ChecksumEngine::checksum_residual[%d]: "
              "Checksumming packet bytes [%d,%d]\n",
              engine_index_, pkt_start_pos, pkt_start_pos+size-1);
      checksum_calc_residual(0, size);
      pkt_start_pos += size;
    }
  }

  bool ChecksumEngineShared::get_residual_dst_phv_word(int pcksm_index, uint16_t *dst_phv) {
    // 3 scenarios to consider here
    if (residual_active()) {
      // 1. Already accumulating residual (started and finished)
      *dst_phv = dst_phv_word(); // Use stored dst_phv_word_
      return true;
    } else if (residual_type() && pcksm_final(pcksm_index)) {
      // 2. Checksum already started, finishing now
      *dst_phv = pcksm_dst_phv(pcksm_index); // Use current dst_phv
      return true;
    } else if (pcksm_residual(pcksm_index) &&
               pcksm_start(pcksm_index) && pcksm_final(pcksm_index)) {
      // 3. Checksum starting and finishing now
      *dst_phv = pcksm_dst_phv(pcksm_index); // Use current dst_phv
      return true;
    } else {
      return false;
    }
  }
  void ChecksumEngineShared::clear_checksum() {
    val_ = 0u;
    folded_val_ = 0u;
    started_ = false;
    finished_ = false;
    accumulating_residual_ = false;
  }
  void ChecksumEngineShared::reset_checksum() {
    phv_ = NULL;
    clear_checksum();
    type_ = kTypeInvalid;
    dst_bit_ = 0;
    dst_phv_word_ = k_phv::kBadPhv;
    final_rotbuf_ = false;
    final_pos_ = 0;
  }
  void ChecksumEngineShared::start_checksum(Phv *phv) {
    // Always called at start of packet parse
    RMT_ASSERT(parser_->inbuf_getsize() == 32);
    reset_checksum();
    phv_ = phv;
  }
  int ChecksumEngineShared::stop_checksum(Packet *packet, int pkt_furthest_pos) {
    // Always called at end of packet parse
    // If error could get phv == NULL
    int err = 0;
    if ((phv_ != NULL) && started_ && (type_ == kTypeResidual) && accumulating_residual_) {
      checksum_residual(packet, pkt_furthest_pos);
      checksum_finalize(true); // Finalize and invert
      // Handle 8b destinations differently: the 16b residual must be split
      // between two adjacent 8b containers - now handled inside write logic
      err |= write_checksum(phv_, dst_phv_word_, val_, true);
      started_ = false; finished_ = true;
    }
    return err;
  }
  int ChecksumEngineShared::do_checksum(int match_index, int engine_index,
                                        int pcksum_index,
                                        int &wrote_8b, int &wrote_16b, int &wrote_32b,
                                        int shift_from_pkt) {
    int err = 0;
    bool finishing = false;
    if (pcksm_start(pcksum_index)) {
      // Clear checksum, store checksum type
      clear_checksum();
      type_ = pcksm_type(pcksum_index);
      // Note only kTypeVerify/kTypeResidual on Tofino
      // But type is 2b field on JBay (XXX/XXX)
      if (type_ == kTypeClot) {
        if (!is_clot_engine()) {
          RMT_LOG(RmtDebug::warn(RmtDebug::kRmtDebugCsumEngErr),
                  "ChecksumEngine::do_checksum(CLOT_IN_01): clot checksum invalid in engines 0,1");
          type_ = kTypeInvalid;
        }
        // type_ stays kTypeClot until reset/restart
      }
      // if (is_clot_engine()) reset_clot_tag_checksum();
      started_ = true;
    }
    else if (!started_) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCsumEngErr),
              "ChecksumEngine::do_checksum(NOT_STARTED_WARN): checksum operation without START\n");
    }
    if (started_ && accumulating_residual_) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCsumEngErr),
              "ChecksumEngine::do_checksum(IN_RESIDUAL_WARN): expect incorrect checksum\n");
    }
    // Do checksumming
    bool rotbuf = pcksm_rotate(pcksum_index);
    uint32_t mask = pcksm_mask(pcksum_index);
    uint32_t swap = pcksm_swap(pcksum_index);
    uint32_t mul2 = pcksm_mul2(pcksum_index);
    // regs_5794_main: no more rotl/shift_left
    //uint16_t rotL = pcksm_shift_left(pcksum_index);
    uint16_t rotL = 0;
    uint16_t add = pcksm_add(pcksum_index);
    // Check if we're finishing this time round as that can affect mask etc
    finishing = pcksm_final(pcksum_index);
    // Does the current data word contain an all-zeros checksum encoded as all-ones?
    bool zeros_as_ones = pcksm_zeros_as_ones(pcksum_index);
    uint8_t zeros_as_ones_pos = pcksm_zeros_as_ones_pos(pcksum_index);
    uint8_t shift = get_parser()->shift_amount(match_index);
    // Note: WIP: shift_from_pkt can be in range [-1,32]
    // -1 ==> no shift_from_pkt, [0,32] ==> valid shift_from_pkt)
    bool shift_is_from_pkt = is_chip1_or_later() && (shift_from_pkt >= 0) && (shift_from_pkt <= 32);
    uint8_t end_pos = 0u;
    if ((finishing) && (type_ == kTypeResidual)) {
      end_pos = pcksm_final_pos(pcksum_index);
      uint32_t end_pos_mask = 0xFFFFFFFF;
      uint32_t end_pos_byte_pair_mask = 0xFFFF;
      if (end_pos < 31) {
        end_pos_mask = (1 << (end_pos + 1)) - 1;
        // Mask for cksum byte pairs that have both bytes <= end_pos;
        // byte pair 0 is [byte0,byte1], byte pair 1 is [byte2,byte3] etc unless
        // rotbuf is true (i.e. shift right) in which case
        // byte pair 0 is [-,byte0], byte pair 1 is [byte1,byte2]
        end_pos_byte_pair_mask = (1 << ((end_pos + (rotbuf ? 2 : 1)) / 2)) - 1;
      }
      if (((mask & (~end_pos_mask)) != 0u) && !shift_is_from_pkt) {
        // XXX: error if mask extends beyond hdr_end_pos
        RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugCsumEngErr),
          "ChecksumEngineShared::%s: mask 0x%08x extends beyond end_pos %d "
          "for engine (%d,%d) match index %d\n",
          __func__, mask, (int)end_pos,
          engine_index, pcksum_index, match_index);
      }
      mask &= end_pos_mask; // Zeroise mask above end_pos
      // XXX: RTL applies byte-swap and multiply-by-2 inconsistently
      // when calculating automatic residual checksum beyond end_pos, so flag
      // as an error if programmed. (Note: the model applies neither
      // byte-swap nor multiply-by-2 beyond end_pos)
      if (swap != (swap & end_pos_byte_pair_mask)) {
        RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugCsumEngErr),
          "ChecksumEngineShared::%s: byte_swap 0x%05x selects bytes beyond "
          "end_pos %d for engine (%d,%d) match index %d (rotbuf %s)\n",
          __func__, swap, (int)end_pos,
          engine_index, pcksum_index, match_index, rotbuf ? "true" : "false");
      }
      if (mul2 != (mul2 & end_pos_byte_pair_mask)) {
        RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugCsumEngErr),
          "ChecksumEngineShared::%s: mul2 0x%05x selects bytes beyond "
          "end_pos %d for engine (%d,%d) match index %d (rotbuf %s)\n",
          __func__, mul2, (int)end_pos,
          engine_index, pcksum_index, match_index, rotbuf ? "true" : "false");
      }
      // XXX: don't do check below if using shift from pkt
      if ((end_pos >= shift) && !shift_is_from_pkt) {
        // XXX: RTL will double count bytes between shift and end_pos so
        // warn if checksum engine is configured this way; model checksum calc
        // correctly counts bytes between shift and end_pos only once
        RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugCsumEngErr),
          "ChecksumEngineShared::%s: end_pos %d >= shift_amt %d "
          "for engine (%d,%d) match index %d\n",
          __func__, (int)end_pos, (int)shift,
          engine_index, pcksum_index, match_index);
      }
    }
    // XXX: WIP: shift has been loaded from packet
    if (shift_is_from_pkt) {
      shift = static_cast<uint8_t>(shift_from_pkt);
      // Zeroise mask bits >= shift_from_pkt
      if (shift < 31) mask &= ((1u << (shift)) - 1);
    }
    const char *typestr[4] = { "Verify", "Resid", "Clot",
                               "INVALID so NO checksum_calc !!!!!!!!!" };
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCsumEngCalcBefore),
            "ChecksumEngine::do_checksum[%d]: Calling checksum_calc[%d]<%d>: "
            "rotbuf=0x%x, mask=0x%x, swap=0x%x, mul2=0x%x, rotL=0x%x, add=0x%x, "
            "done=%s, end_pos=%d (type=%s)\n",
            engine_index_, engine_index_, pcksum_index, rotbuf, mask, swap,
            mul2, rotL, add, finishing ? "true" : "false", (int)end_pos,
            typestr[type_]);
    if (type_ != kTypeInvalid) {
      checksum_calc(rotbuf, mask, swap, mul2, rotL, add, 32, zeros_as_ones, zeros_as_ones_pos);
    }
    // Finish
    if (finishing) {
      dst_phv_word_ = pcksm_dst_phv(pcksum_index);

      // Explicit type check since XXX/XXX
      if (type_ == kTypeClot) {
        checksum_finalize(false); // Finalize NO invert
        //set_clot_tag_checksum(dst_phv_word_, val_);
        started_ = false; finished_ = true;

      } else if (type_ == kTypeResidual) {
        // Record that we're doing residual accumulation
        accumulating_residual_ = true;
        // Get abs position in packet corresponding to position of last byte
        // that has been checksummed so far. Automatic residual checksum
        // calculation, performed after all states have completed, will start
        // from the byte after this.
        uint8_t fin_pos = pcksm_final_pos(pcksum_index);
        // XXX: WIP: if shift was loaded from packet and is <= hdr_end_pos
        // then the last byte checksummed so far was constrained by shift
        // amount, so adjust the fin_pos accordingly
        if (shift_is_from_pkt && (shift <= fin_pos)) fin_pos = shift - 1;
        final_pos_ = parser_->inbuf_getpos(fin_pos);
        // Also store final value rotbuf as this can affect residual
        // computation; final_rotbuf_ true indicates that the last checksum
        // calc finished accumulating in the b0 position, either because the
        // fin_pos is even or the fin_pos is odd but the buffer was rotated; in
        // either of these cases the next checksum calc should start by
        // accumulating in the b1 position.
        final_rotbuf_ = pcksm_rotate(pcksum_index) ^ is_even(fin_pos);
        if (is_chip1_or_later()) {
          // for WIP, take care of any remaining bytes that need to be
          // automatically checksummed; in this final state before automatic
          // residual checksumming, these bytes are included in the residual
          // regardless of the hdr_len_inc value
          int start_pos = fin_pos + 1;
          if (start_pos < shift) {
            checksum_calc_residual(start_pos, shift-start_pos);
          }
        }
      } else if (type_ == kTypeVerify) {
        // In verify mode stash dst_bit
        dst_bit_ = pcksm_dst_bit(pcksum_index);
        checksum_finalize(true); // Finalize and invert
        if (phv_ != NULL) {
          // In verify mode we output the valid bit
          // The dst container varies PER-CHIP
          uint16_t dst_phv = pcksm_dst_phv_verify(match_index, pcksum_index,
                                                  wrote_8b, wrote_16b, wrote_32b);
          uint8_t  dst_bit = pcksm_dst_bit(pcksum_index);
          int width = phv_width(dst_phv); // PER-CHIP
          int mod = (is_jbay_or_later()) ?16 :width; // Always mod16 on JBay
          switch (width) {
            case  8: wrote_8b++;  break;
            case 16: wrote_16b++; break;
            case 32: wrote_32b++; break;
            default: RMT_ASSERT(0);
          }
          dst_bit %= mod;
          if (val_ != 0u) {
            err |= Parser::kErrCsum;
            err |= write_checksum(phv_, dst_phv, 1u << dst_bit, false); // OR
            RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugCsumEngErr),
                    "ChecksumEngine::do_checksum(CHECKSUM_ERR)\n");
          } else {
            // Ensure dst_phv valid if checksum OK
            err |= write_checksum(phv_, dst_phv, 0u, false); // OR
          }
          started_ = false; finished_ = true;
        }
      }
    }
    return err;
  }

  int ChecksumEngineShared::write_checksum(Phv *phv, int phv_or_off, uint32_t val,
                                           bool is_residual) {
    int err = 0;
    if (is_jbay_or_later()) {
      err = parser_->phv_check_write_16b(phv, phv_or_off, static_cast<uint16_t>(val & 0xFFFFu));
    } else {
      // Old code did NOT give error if non-residual written to TAGA PHV
      bool ok = (is_residual) ?Phv::is_valid_phv_p(phv_or_off) :Phv::is_valid_norm_phv_p(phv_or_off);
      if (ok) {
        // Write 0 (no effect since ORed) to check for errors
        err = parser_->phv_check_write_16b(phv, phv_or_off, 0);
        if (err == 0) phv->set_p(phv_or_off, val);
      }
    }
    if (err != 0) {
      const char *str;
      if      ((err & Parser::kErrDstCont) != 0)  str = "INVALID_PHV";
      else if ((err & Parser::kErrPhvOwner) != 0) str = "NOT_OWNER";
      else if ((err & Parser::kErrMultiWr) != 0)  str = "INVALID_MULTI_WRITE";
      else                                        str = "CAN_NOT_WRITE";
      RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugCsumEngErr),
              "ChecksumEngine::write_checksum(%s)\n", str);
    }
    return err;
  }

  void ChecksumEngineShared::set_checksum(int checksum_index,
                                          uint16_t _shift_left, uint32_t _swap, uint32_t _mask,
                                          bool _rotate, uint16_t _add,
                                          uint8_t _dst_bit_final_pos, bool _final,
                                          uint16_t _dst_phv, bool _dst_update,
                                          bool _residual, bool _start, uint32_t _mul2) {
    set_pcksm_shift_left(checksum_index, _shift_left);
    set_pcksm_swap(checksum_index, _swap);
    set_pcksm_mask(checksum_index, _mask);
    set_pcksm_rotate(checksum_index, _rotate);
    set_pcksm_add(checksum_index, _add);

    set_pcksm_residual(checksum_index, _residual);
    set_pcksm_start(checksum_index, _start);
    set_pcksm_dst_update(checksum_index, _dst_update);
    set_pcksm_dst_bit(checksum_index, _dst_bit_final_pos);
    set_pcksm_final_pos(checksum_index, _dst_bit_final_pos);
    set_pcksm_final(checksum_index, _final);
    set_pcksm_mul2(checksum_index, _mul2);

    // Call to set dst_phv is deliberately last
    // On JBay it might futz with programming of dst_bit
    set_pcksm_dst_phv(checksum_index, _dst_phv);
  }

}
