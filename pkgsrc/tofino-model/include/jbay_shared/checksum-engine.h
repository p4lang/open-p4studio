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

#ifndef _JBAY_SHARED_CHECKSUM_ENGINE_
#define _JBAY_SHARED_CHECKSUM_ENGINE_

#include <checksum-engine-shared.h>

namespace MODEL_CHIP_NAMESPACE {

  class ChecksumEngine : public ChecksumEngineShared {

 public:
    // Define a static accessor to simplify calling register constructor in checksum-engine-shared.cpp CTOR
    static enum memory_classes::PrsrPoCsumCtrlRowArrayMutable::PrsrMemMainRspecEnum
        get_csum_r01234(int i) {
      switch (i) {
        case 0: return memory_classes::PrsrPoCsumCtrlRowArrayMutable::PrsrMemMainRspecEnum::kPoCsumCtrl_0Row;
        case 1: return memory_classes::PrsrPoCsumCtrlRowArrayMutable::PrsrMemMainRspecEnum::kPoCsumCtrl_1Row;
        case 2: return memory_classes::PrsrPoCsumCtrlRowArrayMutable::PrsrMemMainRspecEnum::kPoCsumCtrl_2Row;
        case 3: return memory_classes::PrsrPoCsumCtrlRowArrayMutable::PrsrMemMainRspecEnum::kPoCsumCtrl_3Row;
        case 4: return memory_classes::PrsrPoCsumCtrlRowArrayMutable::PrsrMemMainRspecEnum::kPoCsumCtrl_4Row;
        default: RMT_ASSERT(0);
      }
    }

    ChecksumEngine(RmtObjectManager *om, int pipeIndex, int ioIndex, int prsIndex,
                   int engineIndex, int ramIndex, Parser *parser);
    virtual ~ChecksumEngine();


    inline uint8_t  phv_width(int phv_or_off) {
      int size, phvWordA;
      Phv::phv_off16_to_index_p(phv_or_off, &size, &phvWordA, NULL, NULL);
      RMT_ASSERT((size == 8) || (size == 16) || (size == 32));
      return static_cast<uint8_t>(size);
    }
    // Dst PHV container for verification result different on JBay
    // Uses first 16b extractor that's not currently in use for checksumming
    uint16_t pcksm_dst_phv_verify(int match_index, int pi, int ck8, int ck16, int ck32);

    inline  uint8_t  pcksm_mul2(int pi,int wi)           { return get_prs_csum_ctrl()->mul_2(pi,wi); }
    inline  void set_pcksm_mul2(int pi,int wi,uint8_t v) { get_prs_csum_ctrl()->mul_2(pi,wi,v); }

    inline  void set_pcksm_dst_phv_by_off(int pi, uint16_t v) { set_pcksm_dst_phv_raw(pi,v); }
            void set_pcksm_dst_phv_by_phv(int pi, uint16_t v);
    inline  void set_pcksm_dst_phv(int pi, uint16_t v)        { set_pcksm_dst_phv_by_phv(pi,v); }

    // XXX/XXX - allow engines 2,3,4 to be used for validation/residuals
    virtual uint8_t  pcksm_type(int pi)            { return (get_prs_csum_ctrl()->type(pi) & 3); }
    inline  bool     pcksm_clot(int pi)            { return (pcksm_type(pi) == kTypeClot); }
    virtual void set_pcksm_type(int pi, uint8_t v) { get_prs_csum_ctrl()->type(pi,v & 3); }
    inline  void set_pcksm_clot(int pi, bool tf)   { if (tf) set_pcksm_type(pi, kTypeClot); }


    inline bool is_clot_engine() {
      int ei = engine_index();
      return ((ei == 2) || (ei == 3) || (ei == 4));
    }
    inline bool is_clot_type() {
      // Engine must have started and be CLOT to get true here
      return (is_clot_engine() && (type() == kTypeClot));
    }
    inline void reset_clot_tag_checksum() {
      clot_tag_ = ClotEntry::kBadTag; clot_checksum_ = 0;
    }
    inline void set_clot_tag_checksum(uint8_t tag, uint16_t cksum) {
      RMT_ASSERT(is_clot_engine());
      clot_tag_ = tag; clot_checksum_ = cksum;
    }
    inline bool get_clot_tag_checksum(int pi, uint8_t *tag, uint16_t *cksum) {
      RMT_ASSERT((tag != NULL) && (cksum != NULL));
      // NB Can use this func if clot tag/checksum MUST be immediately harvested
      // (note checking is_clot_type() ensures engine has been started and is CLOT)
      if (!is_clot_type() || !finished() || !pcksm_final(pi)) {
        *tag = ClotEntry::kBadTag; *cksum = 0;
        return false;
      } else {
        *tag = dst_phv_word(); *cksum = checksum();
        return true;
      }
    }


    void set_checksum_extra(int checksum_index,
                            uint16_t _shift_left, uint32_t _swap, uint32_t _mask,
                            bool _rotate, uint16_t _add, uint8_t _dst_bit_final_pos, bool _final,
                            uint16_t _dst_phv, bool _dst_update, bool _residual, bool _start,
                            uint32_t _mul2, bool _clot);

    void notify_memory_change_checksum(ChecksumEngine *src_engine,
                                       uint8_t src_mem_inst, uint32_t i);


 private:
    uint8_t    clot_tag_;      // clot_tag/clot_checksum currently not used
    uint16_t   clot_checksum_; // (tag/checksum retrieved from dst_phv_word_/val_)

  };

}

#endif // _JBAY_SHARED_CHECKSUM_ENGINE_
