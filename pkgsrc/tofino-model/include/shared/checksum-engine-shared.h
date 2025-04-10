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

#ifndef _SHARED_CHECKSUM_ENGINE_SHARED_
#define _SHARED_CHECKSUM_ENGINE_SHARED_

#include <cstdint>
#include <rmt-log.h>
#include <rmt-defs.h>
#include <pipe-object.h>
#include <packet.h>
#include <phv.h>
#include <clot.h>

// Reg/mem defs auto-generated from Semifore
#include <register_includes/model_mem.h>


namespace MODEL_CHIP_NAMESPACE {

  class Parser;

  class ChecksumEngineShared : public PipeObject {

 public:
    static constexpr int kType = RmtTypes::kRmtTypeChecksumEngine;
    static constexpr int kTypeVerify   = 0;
    static constexpr int kTypeResidual = 1;
    static constexpr int kTypeClot     = 2;
    static constexpr int kTypeInvalid  = 3;

    static uint32_t ones_cmpl_fold(uint32_t val32) {
      while ((val32 >> 16) != 0u) {
        val32 = (val32 & 0xFFFFu) + (val32 >> 16);
      }
      return val32;
    }


    ChecksumEngineShared(RmtObjectManager *om, int pipeIndex, int ioIndex, int prsIndex,
                         int engineIndex, int ramIndex, Parser *parser);
    virtual ~ChecksumEngineShared();


    // Called whenever a 'Parser' Memory is written (here ChecksumRAM)
    void memory_change_callback(uint8_t mem_type, uint8_t mem_inst, uint32_t mem_index);

    // ChecksumRAM
    inline memory_classes::PrsrPoCsumCtrlRowArrayMutable *get_prs_csum_ctrl() { return &prs_csum_ctrl_; }

    // Get handle on Parser
    inline Parser *get_parser() { return parser_; }


    // PER-CHIP PHV width func - only overridden/implemented on JBay
    virtual uint8_t  phv_width(int phv_or_off) { return Phv::which_width(phv_or_off); }

    // PER-CHIP mul2 funcs - only overridden/implemented on JBay
    virtual uint8_t  pcksm_mul2(int pi, int wi)  { return 0; }
    virtual void set_pcksm_mul2(int pi, int wi, uint8_t v) { }

    // PER-CHIP funcs to determine whether this checksum engine supports CLOTs
    // and if so to get/set the CLOT tag/checksum values - only overridden on JBay
    virtual bool is_clot_engine()                                   { return false; }
    virtual bool is_clot_type()                                     { return false; }
    virtual void reset_clot_tag_checksum()                          { }
    virtual void set_clot_tag_checksum(uint8_t tag, uint16_t cksum) { }
    virtual bool get_clot_tag_checksum(int pi, uint8_t *tag, uint16_t *cksum) {
      RMT_ASSERT((tag != NULL) && (cksum != NULL));
      *tag = ClotEntry::kBadTag; *cksum = 0;
      return false;
    }


    // Public getters/setters for checksum RAM
    // Native access is bit-level for some reason!!
    //
    // regs_5794_main:
    // inline uint8_t pcksm_shift_left(int pi,int wi) { return prs_csum_ctrl_.rotl(pi,wi); }
    inline uint8_t pcksm_shift_left(int pi,int wi)    { return 0; }
    inline uint8_t pcksm_swap(int pi,int wi)          { return prs_csum_ctrl_.swap(pi,wi); }
    inline uint8_t pcksm_mask(int pi,int wi)          { return prs_csum_ctrl_.mask(pi,wi); }
    // Convert bit-level access to word-level access
    inline uint16_t pcksm_shift_left(int pi) {
      uint16_t v = 0u;
      // regs_5794_main
      //for (int wi = 0; wi < 16; wi++) {
      //  if ((pcksm_shift_left(pi,wi) & 0x1) == 0x1) v |= (1u<<wi);
      //}
      return v;
    }
    inline uint32_t pcksm_swap(int pi) {
      uint32_t v = 0u;
      for (int wi = 0; wi < 17; wi++) {
        if ((pcksm_swap(pi,wi) & 1) == 1) v |= (1u<<wi);
      }
      return v;
    }
    inline uint32_t pcksm_mask(int pi) {
      uint32_t v = 0u;
      for (int wi = 0; wi < 32; wi++) {
        if ((pcksm_mask(pi,wi) & 1) == 1) v |= (1u<<wi);
      }
      return v;
    }
    inline uint32_t pcksm_mul2(int pi) {
      uint32_t v = 0u;
      for (int wi = 0; wi < 17; wi++) {
        if ((pcksm_mul2(pi,wi) & 1) == 1) v |= (1u<<wi);
      }
      return v;
    }
    inline  bool     pcksm_rotate(int pi)         { return ((prs_csum_ctrl_.shr(pi) & 1) == 1); } // TODO: ROTFIX?
    inline  uint16_t pcksm_add(int pi)            { return prs_csum_ctrl_.add(pi); }
    inline  uint8_t  pcksm_dst_bit(int pi)        { return prs_csum_ctrl_.dst_bit_hdr_end_pos(pi); }
    inline  uint8_t  pcksm_final_pos(int pi)      { return prs_csum_ctrl_.dst_bit_hdr_end_pos(pi); }
    inline  bool     pcksm_final(int pi)          { return ((prs_csum_ctrl_.hdr_end(pi) & 1) == 1); }
    inline  uint16_t pcksm_dst_phv(int pi)        { return prs_csum_ctrl_.dst(pi); }
    // regs_5794_main:
    // inline bool   pcksm_dst_update(int pi)     { return prs_csum_ctrl_.dst_update(pi); }
    inline  bool     pcksm_dst_update(int pi)     { return true; }
    // PER-CHIP - 2b field on JBay (XXX/XXX - engines 2,3,4 can be used for verify/residual)
    virtual uint8_t  pcksm_type(int pi)           { return (prs_csum_ctrl_.type(pi) & 1); }
    inline  bool     pcksm_verify(int pi)         { return (pcksm_type(pi) == kTypeVerify); }
    inline  bool     pcksm_residual(int pi)       { return (pcksm_type(pi) == kTypeResidual); }
    inline  bool     pcksm_start(int pi)          { return ((prs_csum_ctrl_.start(pi) & 1) == 1); }
    inline  bool     pcksm_zeros_as_ones(int pi)  { return ((prs_csum_ctrl_.zeros_as_ones(pi) & 1) == 1); }
    inline  uint8_t  pcksm_zeros_as_ones_pos(int pi)  { return prs_csum_ctrl_.zeros_as_ones_pos(pi); }
    // Dst PHV container for verification result varies PER-CHIP - usually just value in ChecksumRAM
    virtual uint16_t pcksm_dst_phv_verify(int match_index, int pi, int ck8, int ck16, int ck32) {
      return pcksm_dst_phv(pi);
    }


    // Native access is bit-level for some reason!!
    // inline void set_pcksm_shift_left(int pi, int wi, uint8_t v) { prs_csum_ctrl_.rotl(pi,wi,v); }
    inline void set_pcksm_shift_left(int pi, int wi, uint8_t v)    { }
    inline void set_pcksm_swap(int pi, int wi, uint8_t v)          { prs_csum_ctrl_.swap(pi,wi,v); }
    inline void set_pcksm_mask(int pi, int wi, uint8_t v)          { prs_csum_ctrl_.mask(pi,wi,v); }
    // Convert bit-level access to word-level access
    inline void set_pcksm_shift_left(int pi, uint16_t v) {
      for (int wi = 0; wi < 16; wi++) {
        set_pcksm_shift_left(pi, wi, ((v & (1u<<wi)) == (1u<<wi)) ?1 :0);
      }
    }
    inline void set_pcksm_swap(int pi, uint32_t v) {
      for (int wi = 0; wi < 17; wi++) {
        set_pcksm_swap(pi, wi, ((v & (1u<<wi)) == (1u<<wi)) ?1 :0);
      }
    }
    inline void set_pcksm_mask(int pi, uint32_t v) {
      for (int wi = 0; wi < 32; wi++) {
        set_pcksm_mask(pi, wi, ((v & (1u<<wi)) == (1u<<wi)) ?1 :0);
      }
    }
    inline void set_pcksm_mul2(int pi, uint32_t v) {
      for (int wi = 0; wi < 17; wi++) {
        set_pcksm_mul2(pi, wi, ((v & (1u<<wi)) == (1u<<wi)) ?1 :0);
      }
    }
    inline  void set_pcksm_rotate(int pi, bool tf)         { prs_csum_ctrl_.shr(pi,tf ?1 :0); } // TODO: ROTFIX?
    inline  void set_pcksm_add(int pi, uint16_t v)         { prs_csum_ctrl_.add(pi,v); }
    inline  void set_pcksm_dst_bit(int pi, uint8_t v)      { prs_csum_ctrl_.dst_bit_hdr_end_pos(pi,v); }
    inline  void set_pcksm_final_pos(int pi, uint8_t v)    { prs_csum_ctrl_.dst_bit_hdr_end_pos(pi,v); }
    inline  void set_pcksm_final(int pi, bool tf)          { prs_csum_ctrl_.hdr_end(pi,tf ?1 :0); }
    // PER-CHIP set_pcskm_dst_phv to handle JBay OFF16 rather than PHV
    inline  void set_pcksm_dst_phv_raw(int pi, uint16_t v) { prs_csum_ctrl_.dst(pi,v); }
    virtual void set_pcksm_dst_phv(int pi, uint16_t v)     { set_pcksm_dst_phv_raw(pi,v); }
    // regs_5794_main: inline void set_pcksm_dst_update(int pi, bool tf) { prs_csum_ctrl_.dst_update(pi,tf); }
    inline  void set_pcksm_dst_update(int pi, bool tf)     { }
    // PER-CHIP set_pcksm_type to handle JBay 2b type (XXX/XXX)
    virtual void set_pcksm_type(int pi, uint8_t v)         { prs_csum_ctrl_.type(pi,v & 1); }
    inline  void set_pcksm_verify(int pi, bool tf)         { if (tf) set_pcksm_type(pi, kTypeVerify); }
    inline  void set_pcksm_residual(int pi, bool tf)       { if (tf) set_pcksm_type(pi, kTypeResidual); }
    inline  void set_pcksm_start(int pi, bool tf)          { prs_csum_ctrl_.start(pi,tf ?1 :0); }
    inline  void set_pcksm_zeros_as_ones(int pi, bool tf)  { prs_csum_ctrl_.zeros_as_ones(pi,tf ?1 :0); }
    inline  void set_pcksm_zeros_as_ones_pos(int pi, uint8_t v) { prs_csum_ctrl_.zeros_as_ones_pos(pi, v); }



    // More public funcs
    inline int      engine_index() const { return engine_index_; }
    inline int      ram_index()    const { return ram_index_; }
    inline uint16_t checksum()     const { return static_cast<uint16_t>(val_); }
    inline bool     started()      const { return started_; }
    inline bool     finished()     const { return finished_; }
    inline void     print()              { RMT_LOG_VERBOSE("ChecksumEngine"); }
    inline uint8_t  type()               { return type_; }
    inline bool     residual_type()      { return (type() == kTypeResidual); }
    inline bool     residual_active()    { return accumulating_residual_; }
    inline uint8_t  dst_bit()            { return dst_bit_; }
    inline uint16_t dst_phv_word()       { return dst_phv_word_; }

    bool   get_residual_dst_phv_word(int pcksm_index, uint16_t *dst_phv);
    void   reset_checksum();
    void   clear_checksum();
    void   start_checksum(Phv *phv);
    int    stop_checksum(Packet *packet, int pkt_furthest_pos);
    int    do_checksum(int match_index, int engine_index, int pcksum_index,
                       int &wrote_8b, int &wrote_16b, int &wrote_32b,
                       int shift_from_pkt);

    void set_checksum(int checksum_index,
                      uint16_t _shift_left, uint32_t _swap, uint32_t _mask,
                      bool _rotate, uint16_t _add, uint8_t _dst_bit_final_pos, bool _final,
                      uint16_t _dst_phv, bool _dst_update, bool _residual, bool _start,
                      uint32_t mul2=0);


 private:
    void checksum_calc_fast_b0b1(int b0_pos, int b1_pos,
                                 bool mask0, bool mask1,
                                 bool swap, bool mul2, bool rotL);
    void checksum_calc_fast_32(uint32_t mask, uint32_t swap, uint32_t mul2, uint32_t rotL);
    void checksum_calc_fast_32_rot(uint32_t mask, uint32_t swap, uint32_t mul2, uint32_t rotL);
    void checksum_calc_slow(uint32_t mask, uint32_t swap, uint32_t mul2,
                            uint32_t rotL, int size, int start=0);
    void checksum_calc_slow_rot(uint32_t mask, uint32_t swap, uint32_t mul2,
                                uint32_t rotL, int size);
    void checksum_calc_residual(int start_pos,
                                int num_bytes);

    void checksum_calc(bool rotbuf,
                       uint32_t mask, uint32_t swap, uint32_t mul2,
                       uint32_t shiftL, uint16_t add, int size=32,
                       bool zeros_as_ones=false, uint8_t zeros_as_ones_pos=0);
    void checksum_finalize(bool invert);
    void checksum_residual(Packet *packet, int pkt_furthest_pos);
    int  write_checksum(Phv *phv, int phv_or_off, uint32_t val, bool is_residual);
    bool is_odd(const uint8_t pos) const { return pos % 2u; }
    bool is_even(const uint8_t pos) const { return !is_odd(pos); }


    memory_classes::PrsrPoCsumCtrlRowArrayMutable prs_csum_ctrl_;
    int                                           engine_index_;
    int                                           ram_index_;
    Parser                                       *parser_;
    Phv                                          *phv_;
    uint32_t                                      val_;
    uint32_t                                      folded_val_;
    bool                                          started_;
    bool                                          finished_;
    bool                                          accumulating_residual_;
    uint8_t                                       type_; // Since XXX/XXX
    uint8_t                                       dst_bit_;
    uint16_t                                      dst_phv_word_;
    bool                                          final_rotbuf_;
    // For a residual checksum, final_pos_ is the position in packet of the
    // final byte that was checksummed in the first phase, from which the
    // automatic phase starts
    int                                           final_pos_;
    bool                                          reset_running_;

  };
}
#endif // _SHARED_CHECKSUM_ENGINE_SHARED_
