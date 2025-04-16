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

#ifndef _SHARED_PARSER_EXTRACTORS_ALL_
#define _SHARED_PARSER_EXTRACTORS_ALL_

#include <parser-shared.h>

namespace MODEL_CHIP_NAMESPACE {

  class Parser;

  class ParserExtractorsAll {

 public:
    ParserExtractorsAll(RmtObjectManager *om, Parser *,
                        memory_classes::PrsrPoActionRowArrayMutable *actRam)
        : act_ram_(actRam) {
    }
    virtual ~ParserExtractorsAll() {
    }


    // What is the first extractor that's NOT used for checksumming
    inline int extract8_first_non_checksum(int n_xt, int ck8, int ck16, int ck32) {
      return ck8;
    }
    inline int extract16_first_non_checksum(int n_xt, int ck8, int ck16, int ck32) {
      return ck16;
    }
    inline int extract32_first_non_checksum(int n_xt, int ck8, int ck16, int ck32) {
      return ck32;
    }
    // Checksum extraction uses up high-numbered extractors
    inline bool extract8_available(int i_xt, int n_xt, int ck8, int ck16, int ck32) {
      return ((i_xt >= 0) && (i_xt < n_xt - ck8));
    }
    inline bool extract16_available(int i_xt, int n_xt, int ck8, int ck16, int ck32) {
      return ((i_xt >= 0) && (i_xt < n_xt - ck16));
    }
    inline bool extract32_available(int i_xt, int n_xt, int ck8, int ck16, int ck32) {
      return ((i_xt >= 0) && (i_xt < n_xt - ck32));
    }

    inline bool extract8_src_imm_val(int pi,int i)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract8bWordsPerCycle));
      switch (i) {
        case 0:  return ((act_ram_->phv_8b_src_type_0(pi) & 0x1) == 0x1);
        case 1:  return ((act_ram_->phv_8b_src_type_1(pi) & 0x1) == 0x1);
        case 2:  return ((act_ram_->phv_8b_src_type_2(pi) & 0x1) == 0x1);
        case 3:  return ((act_ram_->phv_8b_src_type_3(pi) & 0x1) == 0x1);
        default: return false;
      }
    }
    inline bool extract8_rot_imm_val(int pi, int i)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract8bWordsPerCycle));
      switch (i) {
        case 0:  return ((act_ram_->phv_8b_offset_rot_imm_0(pi) & 0x1) == 0x1);
        case 1:  return ((act_ram_->phv_8b_offset_rot_imm_1(pi) & 0x1) == 0x1);
        case 2:  return ((act_ram_->phv_8b_offset_rot_imm_2(pi) & 0x1) == 0x1);
        case 3:  return ((act_ram_->phv_8b_offset_rot_imm_3(pi) & 0x1) == 0x1);
        default: return false;
      }
    }
    inline bool extract8_add_off(int pi, int i) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract8bWordsPerCycle));
      switch (i) {
        case 0:  return ((act_ram_->phv_8b_offset_add_dst_0(pi) & 0x1) == 0x1);
        case 1:  return ((act_ram_->phv_8b_offset_add_dst_1(pi) & 0x1) == 0x1);
        case 2:  return ((act_ram_->phv_8b_offset_add_dst_2(pi) & 0x1) == 0x1);
        case 3:  return ((act_ram_->phv_8b_offset_add_dst_3(pi) & 0x1) == 0x1);
        default: return false;
      }
    }
    inline uint8_t extract8_src(int pi, int i, uint8_t mask) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract8bWordsPerCycle));
      switch (i) {
        case 0:  return act_ram_->phv_8b_src_0(pi) & mask;
        case 1:  return act_ram_->phv_8b_src_1(pi) & mask;
        case 2:  return act_ram_->phv_8b_src_2(pi) & mask;
        case 3:  return act_ram_->phv_8b_src_3(pi) & mask;
        default: return 0xFF;
      }
    }
    inline uint8_t extract8_src_msb(int pi, int i, int bitpos) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract8bWordsPerCycle));
      switch (i) {
        case 0:  return (act_ram_->phv_8b_src_0(pi) >> bitpos) & 0x1;
        case 1:  return (act_ram_->phv_8b_src_1(pi) >> bitpos) & 0x1;
        case 2:  return (act_ram_->phv_8b_src_2(pi) >> bitpos) & 0x1;
        case 3:  return (act_ram_->phv_8b_src_3(pi) >> bitpos) & 0x1;
        default: return 0x0;
      }
    }
    inline uint16_t extract8_dst_phv(int pi, int i) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract8bWordsPerCycle));
      switch (i) {
        case 0:  return act_ram_->phv_8b_dst_0(pi);
        case 1:  return act_ram_->phv_8b_dst_1(pi);
        case 2:  return act_ram_->phv_8b_dst_2(pi);
        case 3:  return act_ram_->phv_8b_dst_3(pi);
        default: return k_phv::kBadPhv;
      }
    }
    inline uint8_t extract8_type(int pi, int i)  {
      uint16_t dst = extract8_dst_phv(pi, i);
      return (Phv::is_valid_phv_x(dst)) ?ParserShared::kExtractType8b :ParserShared::kExtractTypeNone;
    }
    inline bool extract16_src_imm_val(int pi,int i)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
      switch (i) {
        case 0:  return ((act_ram_->phv_16b_src_type_0(pi) & 0x1) == 0x1);
        case 1:  return ((act_ram_->phv_16b_src_type_1(pi) & 0x1) == 0x1);
        default: return false;
      }
    }
    inline bool extract16_rot_imm_val(int pi,int i)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
      switch (i) {
        case 0:  return ((act_ram_->phv_16b_offset_rot_imm_0(pi) & 0x1) == 0x1);
        case 1:  return ((act_ram_->phv_16b_offset_rot_imm_1(pi) & 0x1) == 0x1);
        default: return false;
      }
    }
    inline bool extract16_add_off(int pi, int i) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
      switch (i) {
        case 0:  return ((act_ram_->phv_16b_offset_add_dst_0(pi) & 0x1) == 0x1);
        case 1:  return ((act_ram_->phv_16b_offset_add_dst_1(pi) & 0x1) == 0x1);
        case 2:  return ((act_ram_->phv_16b_offset_add_dst_2(pi) & 0x1) == 0x1);
        case 3:  return ((act_ram_->phv_16b_offset_add_dst_3(pi) & 0x1) == 0x1);
        default: return false;
      }
    }
    inline uint8_t extract16_src(int pi, int i, uint8_t mask) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
      switch (i) {
        case 0:  return act_ram_->phv_16b_src_0(pi) & mask;
        case 1:  return act_ram_->phv_16b_src_1(pi) & mask;
        case 2:  return act_ram_->phv_16b_src_2(pi) & mask;
        case 3:  return act_ram_->phv_16b_src_3(pi) & mask;
        default: return 0xFF;
      }
    }
    inline uint8_t extract16_src_msb(int pi, int i, int bitpos) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
      switch (i) {
        case 0:  return (act_ram_->phv_16b_src_0(pi) >> bitpos) & 0x1;
        case 1:  return (act_ram_->phv_16b_src_1(pi) >> bitpos) & 0x1;
        case 2:  return (act_ram_->phv_16b_src_2(pi) >> bitpos) & 0x1;
        case 3:  return (act_ram_->phv_16b_src_3(pi) >> bitpos) & 0x1;
        default: return 0x0;
      }
    }
    inline uint16_t extract16_dst_phv(int pi, int i) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
      switch (i) {
        case 0:  return act_ram_->phv_16b_dst_0(pi);
        case 1:  return act_ram_->phv_16b_dst_1(pi);
        case 2:  return act_ram_->phv_16b_dst_2(pi);
        case 3:  return act_ram_->phv_16b_dst_3(pi);
        default: return k_phv::kBadPhv;
      }
    }
    inline uint8_t extract16_type(int pi, int i)  {
      uint16_t dst = extract16_dst_phv(pi, i);
      return (Phv::is_valid_phv_x(dst)) ?ParserShared::kExtractType16b :ParserShared::kExtractTypeNone;
    }
    inline bool extract32_src_imm_val(int pi, int i)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract32bWordsPerCycle));
      switch (i) {
        case 0:  return ((act_ram_->phv_32b_src_type_0(pi) & 0x1) == 0x1);
        case 1:  return ((act_ram_->phv_32b_src_type_1(pi) & 0x1) == 0x1);
        default: return false;
      }
    }
    inline bool extract32_rot_imm_val(int pi, int i)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract32bWordsPerCycle));
      switch (i) {
        case 0:  return ((act_ram_->phv_32b_offset_rot_imm_0(pi) & 0x1) == 0x1);
        case 1:  return ((act_ram_->phv_32b_offset_rot_imm_1(pi) & 0x1) == 0x1);
        default: return false;
      }
    }
    inline bool extract32_add_off(int pi, int i) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract32bWordsPerCycle));
      switch (i) {
        case 0:  return ((act_ram_->phv_32b_offset_add_dst_0(pi) & 0x1) == 0x1);
        case 1:  return ((act_ram_->phv_32b_offset_add_dst_1(pi) & 0x1) == 0x1);
        case 2:  return ((act_ram_->phv_32b_offset_add_dst_2(pi) & 0x1) == 0x1);
        case 3:  return ((act_ram_->phv_32b_offset_add_dst_3(pi) & 0x1) == 0x1);
        default: return false;
      }
    }
    inline uint8_t extract32_src(int pi, int i, uint8_t mask) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract32bWordsPerCycle));
      switch (i) {
        case 0:  return act_ram_->phv_32b_src_0(pi) & mask;
        case 1:  return act_ram_->phv_32b_src_1(pi) & mask;
        case 2:  return act_ram_->phv_32b_src_2(pi) & mask;
        case 3:  return act_ram_->phv_32b_src_3(pi) & mask;
        default: return 0xFF;
      }
    }
    inline uint8_t extract32_src_msb(int pi, int i, int bitpos) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract32bWordsPerCycle));
      switch (i) {
        case 0:  return (act_ram_->phv_32b_src_0(pi) >> bitpos) & 0x1;
        case 1:  return (act_ram_->phv_32b_src_1(pi) >> bitpos) & 0x1;
        case 2:  return (act_ram_->phv_32b_src_2(pi) >> bitpos) & 0x1;
        case 3:  return (act_ram_->phv_32b_src_3(pi) >> bitpos) & 0x1;
        default: return 0x0;
      }
    }
    inline uint16_t extract32_dst_phv(int pi, int i) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract32bWordsPerCycle));
      switch (i) {
        case 0:  return act_ram_->phv_32b_dst_0(pi);
        case 1:  return act_ram_->phv_32b_dst_1(pi);
        case 2:  return act_ram_->phv_32b_dst_2(pi);
        case 3:  return act_ram_->phv_32b_dst_3(pi);
        default: return k_phv::kBadPhv;
      }
    }
    inline uint8_t extract32_type(int pi, int i)  {
      uint16_t dst = extract32_dst_phv(pi, i);
      return (Phv::is_valid_phv_x(dst)) ?ParserShared::kExtractType32b :ParserShared::kExtractTypeNone;
    }


    inline uint8_t  extract8_src_const(int pi, int i, uint8_t mask, int rot)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract8bWordsPerCycle));
      RMT_ASSERT(extract8_src_imm_val(pi,i));
      uint8_t v = ParserShared::get_const8(extract8_src(pi,i,mask));
      return model_common::Util::rotr8(v,rot);

    }
    inline uint16_t extract16_src_const(int pi, int i, uint8_t mask, int rot) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
      RMT_ASSERT(extract16_src_imm_val(pi,i));
      uint16_t v = ParserShared::get_const16(extract16_src(pi,i,mask));
      return model_common::Util::rotr16(v,rot);
    }
    inline uint32_t extract32_src_const(int pi, int i, uint8_t mask, int rot) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract32bWordsPerCycle));
      RMT_ASSERT(extract32_src_imm_val(pi,i));
      uint32_t v = ParserShared::get_const32(extract32_src(pi,i,mask));
      return model_common::Util::rotr32(v,rot);
    }
    inline uint8_t extract8_inbuf_shift(int pi, int i)   { return 0; }
    inline uint8_t extract16_inbuf_shift(int pi, int i)  { return 0; }
    inline uint8_t extract32_inbuf_shift(int pi, int i)  { return 0; }

    inline uint32_t extract_immediate_consts(int pi, int i, int rot, uint32_t dlft) {
      // This func does nothing on Tofino
      return 0u;
    }



    inline void set_extract8_src_imm_val(int pi, int i, bool tf)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract8bWordsPerCycle));
      switch (i) {
        case 0:  act_ram_->phv_8b_src_type_0(pi, tf ?0x1 :0x0); break;
        case 1:  act_ram_->phv_8b_src_type_1(pi, tf ?0x1 :0x0); break;
        case 2:  act_ram_->phv_8b_src_type_2(pi, tf ?0x1 :0x0); break;
        case 3:  act_ram_->phv_8b_src_type_3(pi, tf ?0x1 :0x0); break;
      }
    }
    inline void set_extract8_rot_imm_val(int pi, int i, bool tf)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract8bWordsPerCycle));
      switch (i) {
        case 0:  act_ram_->phv_8b_offset_rot_imm_0(pi, tf ?0x1 :0x0); break;
        case 1:  act_ram_->phv_8b_offset_rot_imm_1(pi, tf ?0x1 :0x0); break;
        case 2:  act_ram_->phv_8b_offset_rot_imm_2(pi, tf ?0x1 :0x0); break;
        case 3:  act_ram_->phv_8b_offset_rot_imm_3(pi, tf ?0x1 :0x0); break;
      }
    }
    inline void set_extract8_add_off(int pi, int i, bool tf) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract8bWordsPerCycle));
      switch (i) {
        case 0:  act_ram_->phv_8b_offset_add_dst_0(pi, tf ?0x1 :0x0); break;
        case 1:  act_ram_->phv_8b_offset_add_dst_1(pi, tf ?0x1 :0x0); break;
        case 2:  act_ram_->phv_8b_offset_add_dst_2(pi, tf ?0x1 :0x0); break;
        case 3:  act_ram_->phv_8b_offset_add_dst_3(pi, tf ?0x1 :0x0); break;
      }
    }
    inline void set_extract8_src(int pi, int i, uint8_t v) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract8bWordsPerCycle));
      switch (i) {
        case 0:  act_ram_->phv_8b_src_0(pi, v); break;
        case 1:  act_ram_->phv_8b_src_1(pi, v); break;
        case 2:  act_ram_->phv_8b_src_2(pi, v); break;
        case 3:  act_ram_->phv_8b_src_3(pi, v); break;
      }
    }
    inline void set_extract8_dst_phv(int pi, int i, uint16_t v) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract8bWordsPerCycle));
      switch (i) {
        case 0:  act_ram_->phv_8b_dst_0(pi, v); break;
        case 1:  act_ram_->phv_8b_dst_1(pi, v); break;
        case 2:  act_ram_->phv_8b_dst_2(pi, v); break;
        case 3:  act_ram_->phv_8b_dst_3(pi, v); break;
      }
    }
    inline void set_extract8_type(int pi, int i, uint8_t v)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract8bWordsPerCycle));
      RMT_ASSERT((v == ParserShared::kExtractTypeNone) || (v == ParserShared::kExtractType8b));
      if (v == ParserShared::kExtractTypeNone) set_extract8_dst_phv(pi, i, k_phv::kBadPhv);
    }
    inline void set_extract16_src_imm_val(int pi, int i, bool tf)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
      switch (i) {
        case 0:  act_ram_->phv_16b_src_type_0(pi, tf ?0x1 :0x0); break;
        case 1:  act_ram_->phv_16b_src_type_1(pi, tf ?0x1 :0x0); break;
      }
    }
    inline void set_extract16_rot_imm_val(int pi, int i, bool tf)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
      switch (i) {
        case 0:  act_ram_->phv_16b_offset_rot_imm_0(pi, tf ?0x1 :0x0); break;
        case 1:  act_ram_->phv_16b_offset_rot_imm_1(pi, tf ?0x1 :0x0); break;
      }
    }
    inline void set_extract16_add_off(int pi, int i, bool tf) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
      switch (i) {
        case 0:  act_ram_->phv_16b_offset_add_dst_0(pi, tf ?0x1 :0x0); break;
        case 1:  act_ram_->phv_16b_offset_add_dst_1(pi, tf ?0x1 :0x0); break;
        case 2:  act_ram_->phv_16b_offset_add_dst_2(pi, tf ?0x1 :0x0); break;
        case 3:  act_ram_->phv_16b_offset_add_dst_3(pi, tf ?0x1 :0x0); break;
      }
    }
    inline void set_extract16_src(int pi, int i, uint8_t v) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
      switch (i) {
        case 0:  act_ram_->phv_16b_src_0(pi, v); break;
        case 1:  act_ram_->phv_16b_src_1(pi, v); break;
        case 2:  act_ram_->phv_16b_src_2(pi, v); break;
        case 3:  act_ram_->phv_16b_src_3(pi, v); break;
      }
    }
    inline void set_extract16_dst_phv(int pi, int i, uint16_t v) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
      switch (i) {
        case 0:  act_ram_->phv_16b_dst_0(pi, v); break;
        case 1:  act_ram_->phv_16b_dst_1(pi, v); break;
        case 2:  act_ram_->phv_16b_dst_2(pi, v); break;
        case 3:  act_ram_->phv_16b_dst_3(pi, v); break;
      }
    }
    inline void set_extract16_type(int pi, int i, uint8_t v)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract16bWordsPerCycle));
      RMT_ASSERT((v == ParserShared::kExtractTypeNone) || (v == ParserShared::kExtractType16b));
      if (v == ParserShared::kExtractTypeNone) set_extract16_dst_phv(pi, i, k_phv::kBadPhv);
    }
    inline void set_extract32_src_imm_val(int pi, int i, bool tf)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract32bWordsPerCycle));
      switch (i) {
        case 0:  act_ram_->phv_32b_src_type_0(pi, tf ?0x1 :0x0); break;
        case 1:  act_ram_->phv_32b_src_type_1(pi, tf ?0x1 :0x0); break;
      }
    }
    inline void set_extract32_rot_imm_val(int pi, int i, bool tf)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract32bWordsPerCycle));
      switch (i) {
        case 0:  act_ram_->phv_32b_offset_rot_imm_0(pi, tf ?0x1 :0x0); break;
        case 1:  act_ram_->phv_32b_offset_rot_imm_1(pi, tf ?0x1 :0x0); break;
      }
    }
    inline void set_extract32_add_off(int pi, int i, bool tf) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract32bWordsPerCycle));
      switch (i) {
        case 0:  act_ram_->phv_32b_offset_add_dst_0(pi, tf ?0x1 :0x0); break;
        case 1:  act_ram_->phv_32b_offset_add_dst_1(pi, tf ?0x1 :0x0); break;
        case 2:  act_ram_->phv_32b_offset_add_dst_2(pi, tf ?0x1 :0x0); break;
        case 3:  act_ram_->phv_32b_offset_add_dst_3(pi, tf ?0x1 :0x0); break;
      }
    }
    inline void set_extract32_src(int pi, int i, uint8_t v) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract32bWordsPerCycle));
      switch (i) {
        case 0:  act_ram_->phv_32b_src_0(pi, v); break;
        case 1:  act_ram_->phv_32b_src_1(pi, v); break;
        case 2:  act_ram_->phv_32b_src_2(pi, v); break;
        case 3:  act_ram_->phv_32b_src_3(pi, v); break;
      }
    }
    inline void set_extract32_dst_phv(int pi, int i, uint16_t v) {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract32bWordsPerCycle));
      switch (i) {
        case 0:  act_ram_->phv_32b_dst_0(pi, v); break;
        case 1:  act_ram_->phv_32b_dst_1(pi, v); break;
        case 2:  act_ram_->phv_32b_dst_2(pi, v); break;
        case 3:  act_ram_->phv_32b_dst_3(pi, v); break;
      }
    }
    inline void set_extract32_type(int pi, int i, uint8_t v)  {
      RMT_ASSERT((i >= 0) && (i < RmtDefs::kParserExtract32bWordsPerCycle));
      RMT_ASSERT((v == ParserShared::kExtractTypeNone) || (v == ParserShared::kExtractType32b));
      if (v == ParserShared::kExtractTypeNone) set_extract32_dst_phv(pi, i, k_phv::kBadPhv);
    }


 private:
    memory_classes::PrsrPoActionRowArrayMutable   *act_ram_;

  };
}

#endif // _SHARED_PARSER_EXTRACTORS_ALL_
