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

// CHECKSUM_ENGINE - JBay specific code

#include <checksum-engine.h>
#include <parser.h>

namespace MODEL_CHIP_NAMESPACE {

ChecksumEngine::ChecksumEngine(RmtObjectManager *om, int pipeIndex, int ioIndex, int prsIndex,
                               int engineIndex, int ramIndex, Parser *parser)
    : ChecksumEngineShared(om, pipeIndex, ioIndex, prsIndex, engineIndex, ramIndex, parser) {

  reset_clot_tag_checksum();
}
ChecksumEngine::~ChecksumEngine() {
}


// The PHV container written to on checksum verify is the next available 16b extractor.
// (unlike on Tofino where it's simply the PHV container cited in the ChecksumRAM slot).
// The number of extractors already in use for checksumming is determined by the sum
// of ck8, ck16 and ck32.
uint16_t ChecksumEngine::pcksm_dst_phv_verify(int match_index, int pi,
                                              int ck8, int ck16, int ck32) {
  Parser *p = get_parser();
  int xtrac_max = ParserShared::kExtractMax_16b;
  int xtrac_nxt = p->extract16_first_non_checksum(xtrac_max, ck8, ck16, ck32);
  RMT_ASSERT((xtrac_nxt >= 0) && (xtrac_nxt < xtrac_max));
  return p->extract16_dst_phv(match_index, xtrac_nxt); // Use container in next free extractor
}

// JUST USED BY UNIT TEST LOGIC
void ChecksumEngine::set_pcksm_dst_phv_by_phv(int pi, uint16_t v) {
  // Explicit types since XXX/XXX
  uint8_t type = pcksm_type(pi);
  if ((type == kTypeClot) && is_clot_engine()) {
    // Do nothing for Clot entry as dst is simply Clot tag
    set_pcksm_dst_phv_by_off(pi, v);

  } else if ((type == kTypeVerify) || (type == kTypeResidual)) {
    // May need to futz with dst_bit if in verify mode
    uint8_t dst_bit = pcksm_dst_bit(pi);

    int size, offA, offB, sz8_01;
    Phv::phv_index_to_off16_p(v, &size, &offA, &offB, &sz8_01);
    if (size == 8) {
      // Have to use offA
      set_pcksm_dst_phv_by_off(pi, static_cast<uint16_t>(offA));
      if (type == kTypeResidual) return;

      // In verify mode might need to move ok/err bit along
      RMT_ASSERT(dst_bit < 8);
      // XXX - this used to check 'if (sz8_01 == 0)'
      if (sz8_01 == 1) set_pcksm_dst_bit(pi, dst_bit+8);

    } else if (size == 16) {
      // Have to use offA
      set_pcksm_dst_phv_by_off(pi, static_cast<uint16_t>(offA));
      if (type == kTypeResidual) return;

      RMT_ASSERT(dst_bit < 16);

    } else if (size == 32) {
      // Assume using LSBs for residual (ie offA)
      // XXX - this used to use offB not offA
      set_pcksm_dst_phv_by_off(pi, static_cast<uint16_t>(offA));
      if (type == kTypeResidual) return;

      // In verify mode we might need to modify ok/err bit
      // (if >= 16 using MSBs of PHV32 so use offB and dst_bit-16)
      // XXX - this used to use offA not offB
      RMT_ASSERT(dst_bit < 32);
      if (dst_bit >= 16) {
        set_pcksm_dst_phv_by_off(pi, static_cast<uint16_t>(offB));
        set_pcksm_dst_bit(pi, dst_bit-16);
      }
    } else {
      RMT_ASSERT(0);
    }
  }
}

// XXX - fixed up _swap and mul2 to be uint32_t (they are 17b)
void ChecksumEngine::set_checksum_extra(int checksum_index,
                                        uint16_t _shift_left, uint32_t _swap, uint32_t _mask,
                                        bool _rotate, uint16_t _add, uint8_t _dst_bit_final_pos, bool _final,
                                        uint16_t _dst_phv, bool _dst_update, bool _residual, bool _start,
                                        uint32_t _mul2, bool _clot) {
  ChecksumEngineShared::set_checksum(checksum_index,
                                     _shift_left, _swap, _mask, _rotate,_add,
                                     _dst_bit_final_pos, _final,
                                     _dst_phv, _dst_update, _residual, _start);
  if (_mul2 != 0u) ChecksumEngineShared::set_pcksm_mul2(checksum_index, _mul2);
  if (_clot) {
    RMT_ASSERT(is_clot_engine() && "Can only use Clot checksum on ClotEngine (2,3 or 4)");
    RMT_ASSERT(!_residual && "Can not specify checksum type eq Residual AND eq Clot");
    set_pcksm_clot(checksum_index, true);
  }
}


void ChecksumEngine::notify_memory_change_checksum(ChecksumEngine *src_engine,
                                                   uint8_t src_mem_inst, uint32_t i) {

  RMT_ASSERT((src_engine != NULL) && (src_mem_inst == ram_index()));
  memory_classes::PrsrPoCsumCtrlRowArrayMutable *src = src_engine->get_prs_csum_ctrl();
  memory_classes::PrsrPoCsumCtrlRowArrayMutable *dst = get_prs_csum_ctrl();
  // Copy data at index i in src ChecksumEngine checksumRAM
  // into this ChecksumEngine checksumRAM
#ifdef COPY_MEMORY_USING_FIELDS
  dst->add(i, src->add(i));
  for (int j = 0; j < 17; j++) dst->swap(i, j, src->swap(i,j));
  dst->shr(i, src->shr(i));
  for (int j = 0; j < 32; j++) dst->mask(i, j, src->mask(i,j));
  dst->dst_bit_hdr_end_pos(i, src->dst_bit_hdr_end_pos(i));
  dst->dst(i, src->dst(i));
  dst->hdr_end(i, src->hdr_end(i));
  dst->type(i, src->type(i));
  dst->start(i, src->start(i));
  dst->zeros_as_ones(i, src->zeros_as_ones(i));
  dst->zeros_as_ones_pos(i, src->zeros_as_ones_pos(i));
  dst->add(i, src->add(i));
  dst->add(i, src->add(i));
  for (int j = 0; j < 17; j++) dst->mul_2(i, j, src->mul_2(i,j));
#else
  uint64_t data0 = UINT64_C(0), data1 = UINT64_C(0), T = UINT64_C(0);
  src->read(i, &data0, &data1, T);
  dst->write(i, data0, data1, T);
#endif
}

}
