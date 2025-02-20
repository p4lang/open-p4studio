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

#ifndef _SHARED_PARSER_SHARED_
#define _SHARED_PARSER_SHARED_

#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include <model_core/model.h>
#include <model_core/log-buffer.h>
#include <rmt-log.h>
#include <rmt-defs.h>
#include <pipe-object.h>
#include <bitvector.h>
#include <tcam3.h>
#include <packet.h>
#include <phv.h>
#include <checksum-engine.h>
#include <pipe.h>

//  Previous includes now defunct
//  regs_5794_main
//#include <register_includes/prsr_err_cfg_mutable.h>
//#include <register_includes/prsr_counter_mutable.h>
//  regs_7217_parser-split - fe_rspec -> main_rspec & common_rspec disappeared
//#include <register_includes/prsr_reg_common_rspec_mutable.h>

// Reg defs auto-generated from Semifore
#include <register_includes/model_mem.h>

#include <register_includes/prsr_reg_main_rspec_hdr_len_adj_mutable.h>
#include <register_includes/prsr_reg_main_rspec_max_iter_mutable.h>
#include <register_includes/prsr_reg_main_rspec_no_multi_wr_mutable.h>
#include <register_includes/prsr_reg_main_rspec_phv_owner_mutable.h>
#include <register_includes/prsr_reg_main_rspec_pri_start_mutable.h>
#include <register_includes/prsr_reg_main_rspec_pri_thresh_mutable.h>
#include <register_includes/prsr_reg_main_rspec_start_state_mutable.h>

#include <register_includes/prsr_reg_main_rspec_hdr_byte_cnt_array_mutable.h>
#include <register_includes/prsr_reg_main_rspec_idle_cnt_array_mutable.h>
#include <register_includes/prsr_reg_main_rspec_pkt_drop_cnt_array_mutable.h>

#include <register_includes/prsr_reg_main_rspec_aram_mbe_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_aram_sbe_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_csum_err_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_ctr_range_err_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_fcs_err_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_multi_wr_err_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_no_tcam_match_err_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_op_fifo_full_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_op_fifo_full_stall_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_partial_hdr_err_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_phv_owner_err_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_src_ext_err_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_tcam_par_err_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_timeout_cycle_err_cnt_mutable.h>
#include <register_includes/prsr_reg_main_rspec_timeout_iter_err_cnt_mutable.h>

// These registers are instantiated but are NOT used - however allows read/write
#include <register_includes/prsr_reg_main_rspec_ecc_mutable.h>
#include <register_includes/prsr_reg_main_rspec_debug_ctrl_mutable.h>
#include <register_includes/prsr_reg_main_rspec_max_cycle_mutable.h>



namespace MODEL_CHIP_NAMESPACE {

class ParseContext {
 public:
  ParseContext(const char* preamble=nullptr) :
      preamble_(preamble),
      state_(-1),
      index_(-1),
      extracted_value_(0),
      extracted_value_size_(0) { }
  const char *preamble_;
  int state_;
  int index_;
  uint32_t extracted_value_;
  int extracted_value_size_;  // size in bytes
};

  // Used in call to memory_change_callback
  struct ParserMemoryType {
    static constexpr uint8_t kNone        = 0x00;
    static constexpr uint8_t kTcam        = 0x01;
    static constexpr uint8_t kEarlyAction = 0x02;
    static constexpr uint8_t kAction      = 0x04;
    static constexpr uint8_t kCounter     = 0x08;
    static constexpr uint8_t kChecksum    = 0x10;
    static constexpr uint8_t kSwTcam      = 0x80;
  };


  // Spilt InputBuffer into separate class now we have >1 input bufs
  // Buf 0 is normal input buffer that moves through packet
  // Buf 1 is duplicate of very first 16 bytes of packet (with read_pos_mod=16)
  // Buf 2 contains version/timestamp 10 bytes

  class InputBuffer {
 public:
    static constexpr int kInputBufferSize = RmtDefs::kParserInputBufferSize;

    InputBuffer() : rbytes_(0), gbytes_(0), wbytes_(0), read_pos_mod_(0) { }
    ~InputBuffer() { }

    inline void reset()  { rbytes_ = 0; gbytes_ = 0; wbytes_ = 0; }

    inline int getsize() { return kInputBufferSize; }

    inline int getpos(int relpos) { return rbytes_ + relpos; }

    // Max byte ever got - used by residual calc
    // TODO: Could return 1+gbytes_ here or rbytes_ (if assume last hdr shifted off)
    inline int getmaxpos() { return rbytes_; }

    // Read byte/short/int/long at pos in [read_bytes_ write_bytes_]
    template <typename T> bool get(int pos, T *val, bool load=true, bool partial=false) {
      static_assert(std::is_integral<T>::value, "only integral types allowed" );
      static_assert(!std::is_same< T, bool >::value, "bool not allowed");
      if (read_pos_mod_ > 0) pos %= read_pos_mod_;
      // Entire or partial read beyond end of buffer and !partial: return false
      if (((pos < 0) || (pos+(signed)sizeof(T)-1 > (wbytes_ - rbytes_ - 1))) && !partial) return false;
      if (!load) return true;
      *val = 0;
      // Entire read from beyond end of buffer, just return zero...
      if (pos > (wbytes_ - rbytes_)) return true;  // XXX
      // Partial read from beyond end of buffer...
      for (unsigned int i = 0; (i < sizeof(T)) && (i < static_cast<unsigned int>(wbytes_ - rbytes_ - pos)); i++)
        *val = (*val << 8) | input_buffer_[(rbytes_ + pos + i) % kInputBufferSize];
      int num_bytes = wbytes_ - rbytes_ - pos;
      if (sizeof(T) > static_cast<size_t>(num_bytes))
        *val <<= (sizeof(T) - num_bytes) * 8;
      // Track max byte ever got
      gbytes_ = std::max(gbytes_, std::min((int)(rbytes_+pos+sizeof(T)), wbytes_));
      return true;
    }

    inline int shift(int shift_bytes) {
      int shifted_bytes = std::min(shift_bytes, wbytes_ - rbytes_);
      rbytes_ += shifted_bytes;
      return shifted_bytes;
    }

    // Setup read_pos_mod_ value
    inline void set_read_pos_mod(int pos) { read_pos_mod_ = pos; }

    // Fill input buffer from Packet or from immediate value
    int  fill(Packet *p, int ppos, int nbytes=kInputBufferSize);
    int  fill_byte(uint8_t byte);
    int  fill_zeros(int nzeros);
    // Rewind input buffer by nbytes
    void rewind(int nbytes);

    // Fill a number of consecutive bytes of input buffer from byte/short/int/long value val
    template <typename T> int fill_val(T val, int nbytes=sizeof(T), bool little_endian=false) {
      static_assert(std::is_integral<T>::value, "only integral types allowed" );
      static_assert(!std::is_same< T, bool >::value, "bool not allowed");
      RMT_ASSERT ((nbytes >= 0) && (static_cast<size_t>(nbytes) <= sizeof(T)));
      int cnt = 0;
      for (int i = 0; i < nbytes; i++) {
        int i2 = (little_endian) ?i :(nbytes-1-i);
        cnt += fill_byte(static_cast<uint8_t>((val >> (i2*8)) & static_cast<T>(0xFF)));
      }
      return cnt;
    }

 private:
    int      rbytes_ = 0;
    int      gbytes_ = 0;
    int      wbytes_ = 0;
    int      read_pos_mod_ = 0;
    uint8_t  input_buffer_[kInputBufferSize];
  };



  // Track extraction info - used for constraint checking
  struct ExtractInfo {
    uint32_t  n_8b_extracts_;
    uint32_t  n_16b_extracts_;
    bool      prev_n_8b_extracts_always_4N_;
    bool      prev_n_8b_extracts_always_2N_;
    bool      prev_n_16b_extracts_always_2N_;
    uint32_t  n_8b_wc_fifo_occ_;
    uint32_t  n_16b_wc_fifo_occ_;
    uint32_t  n_32b_wc_fifo_occ_;
    uint32_t  n_8b_pad_states_needed_;
    uint32_t  n_16b_pad_states_needed_;
    bool      n2w_8b_32b_recently_seen_;
    bool      n2w_16b_32b_recently_seen_;
    bool      n2w_8b_in_curr_state_;
    bool      n2w_16b_in_curr_state_;

    ExtractInfo() : n_8b_extracts_(0), n_16b_extracts_(0),
      prev_n_8b_extracts_always_4N_(true),
      prev_n_8b_extracts_always_2N_(true),
      prev_n_16b_extracts_always_2N_(true),
      n_8b_wc_fifo_occ_(0),
      n_16b_wc_fifo_occ_(0),
      n_32b_wc_fifo_occ_(0),
      n_8b_pad_states_needed_(0),
      n_16b_pad_states_needed_(0),
      n2w_8b_32b_recently_seen_(false),
      n2w_16b_32b_recently_seen_(false),
      n2w_8b_in_curr_state_(false),
      n2w_16b_in_curr_state_(false) { }
  };



  class ParserShared : public PipeObject {

 private:
    void reset_this();

 public:
    static bool kRelaxExtractionCheck; // Defined in rmt-config.cpp
    static bool kRelaxPreExtractionCheck;
    static bool kRelaxFinalParseCheck;
    static bool kPhvFillOtherGress;

    static const char *kErrorStrings[];

    static constexpr int kType = RmtTypes::kRmtTypeParser;
    static constexpr int kChannels = RmtDefs::kParserChannels;
    static constexpr int kPriorities = RmtDefs::kParserPriorities;
    static constexpr int kInputBuffers = 3;
    static constexpr int kInputBufferSize = RmtDefs::kParserInputBufferSize;
    static constexpr int kFirstByteInbuf2 = RmtDefs::kParserFirstByteInbuf2;
    static constexpr int kStates = RmtDefs::kParserStates;
    static constexpr int kCounterInitAddrWidth = RmtDefs::kParserCounterInitAddrWidth;
    static constexpr int kCounterInitAddrMask = RmtDefs::kParserCounterInitAddrMask;
    static constexpr int kCounterInitEntries = RmtDefs::kParserCounterInitEntries;
    static constexpr int kChecksumEntries = RmtDefs::kParserChecksumEntries;
    static constexpr int kChecksumEngines = RmtDefs::kParserChecksumEngines;
    static constexpr int kChecksumRams = RmtDefs::kParserChecksumRams;
    static constexpr int kCounterStackSize = RmtDefs::kParserCounterStackSize;
    static constexpr int kMaxStates = RmtDefs::kParserMaxStates;
    static constexpr int kMaxHdrLen = 511;
    static constexpr int kExtractSrcBits = 6;
    static constexpr int kExtractSrcMax = 1<<kExtractSrcBits;
    static constexpr int kExtractSrcMSB = kExtractSrcBits-1; // Determines whether we use TS/Ver buf2
    static constexpr int kExtractSrcMask = kExtractSrcMax-1;
    static constexpr int kExtractSrcImmConstMask = 0xFF;
    static constexpr int kLoadBytes = 4;
    static constexpr int kLoadOffsetWidth = 5;
    static constexpr int kLoadOffsetMask = (1<<kLoadOffsetWidth)-1;
    static constexpr int kExtractMax_8b = RmtDefs::kParserExtract8bWordsPerCycle;
    static constexpr int kExtractMax_16b = RmtDefs::kParserExtract16bWordsPerCycle;
    static constexpr int kExtractMax_32b = RmtDefs::kParserExtract32bWordsPerCycle;
    static constexpr int kExtractMax_8b_16b = (kExtractMax_8b > kExtractMax_16b) ?kExtractMax_8b :kExtractMax_16b;
    static constexpr int kExtractMax_16b_32b = (kExtractMax_16b > kExtractMax_32b) ?kExtractMax_16b :kExtractMax_32b;
    static constexpr int kExtractMax_8b_16b_32b = (kExtractMax_8b_16b > kExtractMax_16b_32b) ?kExtractMax_8b_16b :kExtractMax_16b_32b;
    static constexpr int kExtractPhvWordsPerCycle = kExtractMax_8b_16b_32b;
    static constexpr int kCheckFlags = RmtDefs::kParserCheckFlags;
    static constexpr int kExtractFlags = RmtDefs::kParserExtractFlags;
    static constexpr int kExtractArbFifoSize = 32;    //  Only used for TofinoA0/TofinoB0

    // Constraint check flags - kCheckPrevNumX flags set dynamically
    static constexpr uint32_t kCheckValidity                     = 0x00000001;
    static constexpr uint32_t kCheckCanWrite                     = 0x00000002;
    static constexpr uint32_t kCheckMismatchedWidths             = 0x00000004;
    static constexpr uint32_t kCheckFurther                      = 0x00000008;
    static constexpr uint32_t kCheckPrevNum8bitExtractsAlways2N  = 0x00010000;
    static constexpr uint32_t kCheckPrevNum8bitExtractsAlways4N  = 0x00020000;
    static constexpr uint32_t kCheckPrevNum16bitExtractsAlways2N = 0x00040000;
    // Extraction flags
    static constexpr uint32_t kExtract8bSpecial                  = 0x00000001;
    static constexpr uint32_t kExtract16bSpecial                 = 0x00000002;
    static constexpr uint32_t kExtract32bSpecial                 = 0x00000004;

    // Error codes - s/w view only
    // We no longer RELY on these corresponding to the bits in the
    // err_int_cfg/err_phv_cfg registers - we map as appropriate
    // NB. Some code are purely for s/w - no h/w analogue at moment
    static constexpr uint32_t kErrNoTcamMatch  = 0x00000001;
    static constexpr uint32_t kErrPartialHdr   = 0x00000002;
    static constexpr uint32_t kErrCtrRange     = 0x00000004;
    static constexpr uint32_t kErrTimeoutIter  = 0x00000008;
    static constexpr uint32_t kErrTimeoutCycle = 0x00000010;
    static constexpr uint32_t kErrSrcExt       = 0x00000020;
    static constexpr uint32_t kErrDstCont      = 0x00000040; // Not JBay
    static constexpr uint32_t kErrPhvOwner     = 0x00000080;
    static constexpr uint32_t kErrMultiWr      = 0x00000100;
    static constexpr uint32_t kErrARAMsbe      = 0x00000200;
    static constexpr uint32_t kErrARAMmbe      = 0x00000400;
    static constexpr uint32_t kErrFcs          = 0x00000800;
    static constexpr uint32_t kErrCsum         = 0x00001000;
    static constexpr uint32_t kErrIbufOflow    = 0x00002000;
    static constexpr uint32_t kErrIbufUflow    = 0x00004000;
    static constexpr uint32_t kErrOpFifoOflow  = 0x00008000;
    static constexpr uint32_t kErrOpFifoUflow  = 0x00010000;
    static constexpr uint32_t kErrTcamPar      = 0x00020000;
    static constexpr uint32_t kErrNarrowToWide = 0x00040000; // JBay only
    static constexpr uint32_t kErrCsumConfig   = 0x00100000;

    static constexpr uint32_t kErrNoDropMask   = 0x00101000; // Csum*
    static constexpr uint32_t kErrDfltRepMask  = 0x00FFEFFF; // All other
    static constexpr uint32_t kErrTofinoMask   = 0x0003EFFF; // tofinoA0 and tofinoB0
    static constexpr uint32_t kErrJbayMask     = 0x0007EFBF;

    static constexpr int      kErrStrBufSize   = 256;


    // Convenience funcs used when extracting immediate constants
    // (but only used by Tofino, JBay does imm consts differently)
    static uint16_t get_const8(uint8_t v) {
      return v;
    }
    static constexpr uint16_t kExtract16SrcConstMask = 0x0Fu;
    static constexpr uint16_t kExtract16SrcRotateMask = 0xF0u;
    static constexpr uint16_t kExtract16SrcRotateShift = 4;
    static uint16_t get_const16(uint8_t v) {
      uint16_t v16 = (uint16_t)v;
      int      r16 = (v16 & kExtract16SrcRotateMask) >> kExtract16SrcRotateShift;
      return model_common::Util::rotl16(v16 & kExtract16SrcConstMask, r16);
    }
    static constexpr uint32_t kExtract32SrcConstMask = 0x7u;
    static constexpr uint32_t kExtract32SrcRotateMask = 0xF8u;
    static constexpr uint32_t kExtract32SrcRotateShift = 3;
    static uint32_t get_const32(uint8_t v) {
      uint32_t v32 = (uint32_t)v;
      int      r32 = (v32 & kExtract32SrcRotateMask) >> kExtract32SrcRotateShift;
      return model_common::Util::rotl32(v32 & kExtract32SrcConstMask, r32);
    }


    // Until regs_7217_parser-split ingress/egress Parsers would get a reference
    // to a shared PrsrRegCommonRspec defined in the parser-block code. This
    // Common rspec no longer exists - all Prsr regs either in PrsrRegMainRspec
    // of which there are 36 per pipe (1 for each of 18x Ingress/Egress parsers)
    // or in PrsrRegMergeRspec of which there is one only for the whole pipe

    ParserShared(RmtObjectManager *om, int pipeIndex, int prsIndex, int ioIndex,
                 const ParserConfig &config);
    virtual ~ParserShared();


    // PER-CHIP specialisation of Phv increment (some chips wrap some don't)
    virtual int phv_increment(int phv, int incr) = 0;

    // ParseMerge
    inline ParseMergeReg *get_prs_merge() { return prs_merge_reg_; }

    // ParserChannel
    // prs_fe_rspec_.pkt_cnt disappeared in registers 20141007_170846_2869
    //inline uint64_t packet_count(int ci)              { return prs_main_pkt_cnt(ci).cnt(); }
    inline  uint64_t hdr_byte_count(int ci)             { return prs_main_hdr_byte_cnt_.cnt(ci); }
    inline  uint64_t idle_count(int ci)                 { return prs_main_idle_cnt_.cnt(ci); }
    inline  uint8_t  start_state(int ci)                { return prs_main_start_state_.state(ci); }
    inline  uint16_t max_iter()                         { return prs_main_max_iter_.max(); }
    inline  uint8_t  pri_start(int ci)                  { return prs_main_pri_start_.pri(ci); }
    virtual uint8_t  hdr_len_adj() = 0;
    virtual void set_hdr_len_adj(uint8_t v) = 0;
    virtual uint16_t hdr_max_len()                      { return kMaxHdrLen; } // Overridden on JBay
    virtual uint8_t  port_rate_cfg(int ci) = 0;
    virtual uint8_t  port_chnl_en(int ci) = 0;
    virtual bool     enabled(int ci) = 0;
    virtual uint8_t  mode() = 0;
    virtual uint8_t  mem_ctrl()                         { return 0;  } // Only overridden on JBay
    // These only overidden on JBay
    virtual uint64_t drop_count(int ci)                 { return static_cast<uint64_t>(prs_main_pkt_drop_cnt_.cnt(ci)); }
    virtual uint64_t rx_count(int ci)                   { return UINT64_C(0); }
    virtual uint64_t tx_count(int ci)                   { return UINT64_C(0); }
    // Allow access to prs_main_hdr_len_adj_ as new max_len field within it on JBay
    inline register_classes::PrsrRegMainRspecHdrLenAdjMutable *get_hdr_len_adj() { return &prs_main_hdr_len_adj_; }

    //inline void set_packet_count(int ci, uint64_t v)  { prs_main_pkt_cnt(ci).cnt(v); }
    inline  void set_hdr_byte_count(int ci, uint64_t v) { prs_main_hdr_byte_cnt_.cnt(ci,v); }
    inline  void set_idle_count(int ci, uint64_t v)     { prs_main_idle_cnt_.cnt(ci,v); }
    inline  void set_start_state(int ci, uint8_t v)     { prs_main_start_state_.state(ci,v); }
    //inline void inc_packet_count(int ci, int n=1)     { prs_main_pkt_cnt(ci).cnt(n + prs_main_pkt_cnt(ci).cnt()); }
    inline  void inc_hdr_byte_count(int ci, int n)      { prs_main_hdr_byte_cnt_.cnt(ci, n + prs_main_hdr_byte_cnt_.cnt(ci)); }
    inline  void inc_idle_count(int ci, int n=1)        { prs_main_idle_cnt_.cnt(ci, n + prs_main_idle_cnt_.cnt(ci)); }
    inline  void set_max_iter(uint16_t v)               { prs_main_max_iter_.max(v); }
    inline  void set_pri_start(int ci, uint8_t v)       { prs_main_pri_start_.pri(ci,v); }
    virtual void set_hdr_max_len(uint16_t v)            { } // Only overridden on JBay
    virtual void set_port_rate_cfg(int ci, uint8_t v) = 0;
    virtual void set_port_chnl_en(int ci, uint8_t v) = 0;
    virtual void set_enabled(int ci, bool tf) = 0;
    virtual void set_mode(uint8_t v) = 0;
    virtual void set_mem_ctrl(uint8_t v)                { } // Only overridden on JBay
    // These only overidden on JBay
    inline  void set_drop_count_raw(int ci, uint64_t v) { prs_main_pkt_drop_cnt_.cnt(ci,v); }
    virtual void set_drop_count(int ci, uint64_t v)     { prs_main_pkt_drop_cnt_.cnt(ci,static_cast<uint32_t>(v & UINT64_C(0xFFFFFFFF))); }
    virtual void set_rx_count(int ci, uint64_t v)       { }
    virtual void set_tx_count(int ci, uint64_t v)       { }
    virtual void inc_rx_count(int ci)                   { }
    virtual void inc_drop_count(int ci);
    virtual void inc_tx_count(int ci);


    // ParserError
    inline uint32_t last_err_flags()                   { return last_err_flags_; }

    // Some virtual funcs to handle en/set/inc of certain errors (passed in param e)
    // These need to be overriden PER-CHIP as not all chips have all registers/fields
    // Sometimes pass in address of register that function needs to modify
    virtual void     prs_reg_intr_status_err_set(uint32_t e) = 0;
    virtual void     prs_reg_err_cnt_inc(uint32_t e, int inc) = 0;
    virtual uint32_t prs_map_sw_errs_to_hw_errs(uint32_t e) = 0;


    // regs_5794_main: error handling config changes - use 5 new perr_ funcs below
    //inline uint32_t perr_count(register_classes::PrsrCounterMutable  *pc)              { return pc->cnt(); }
    //inline uint8_t  perr_dst_bit(register_classes::PrsrErrCfgMutable *pe)              { return pe->dst_b(); }
    //inline uint16_t perr_dst_phv(register_classes::PrsrErrCfgMutable *pe)              { return pe->dst_f(); }
    //inline bool     perr_drop(register_classes::PrsrErrCfgMutable *pe)                 { return ((pe->drop() & 0x1) == 0x1); }
    //
    //inline void perr_inc_count(register_classes::PrsrCounterMutable  *pc, int n=1)     { pc->cnt(n + pc->cnt()); }
    //inline void perr_set_count(register_classes::PrsrCounterMutable  *pc, uint32_t v)  { pc->cnt(v); }
    //inline void perr_set_dst_bit(register_classes::PrsrErrCfgMutable *pe, uint8_t v)   { pe->dst_b(v); }
    //inline void perr_set_dst_phv(register_classes::PrsrErrCfgMutable *pe, uint16_t v)  { pe->dst_f(v); }
    //inline void perr_set_drop(register_classes::PrsrErrCfgMutable *pe, bool tf)        { pe->drop(tf ? 0x1 : 0x0); }

    // Specify this token in Makefile to select what errors get *reported* in PHV by default
#ifndef PARSER_DFLT_ERR_PHV_MASK
#define PARSER_DFLT_ERR_PHV_MASK (kErrDfltRepMask) // Default is all except checksum errors
#endif
    // Or call this function to set it dynamically (if you wanted to see checksum errors say)
    inline void set_perr_phv_mask(uint32_t mask) { perr_phv_mask_ = mask; }
    inline uint32_t perr_phv_mask() const { return perr_phv_mask_; }

    // PER-CHIP func to actually output error, write error flags to PHV word(s)
    virtual bool perr_phv_output(int chan, uint32_t err, Phv *phv) = 0;
    virtual void perr_phv_write(uint16_t dst, uint32_t err, Phv *phv);

    inline void perr_interrupt_output(uint32_t err) {
      prs_reg_intr_status_err_set(err);

      // int_cfg register is no longer present in parser
      //register_classes::PrsrRegMainRspecErrIntCfgMutable *int_cfg = &prs_main_err_int_cfg();
      //register_classes::PrsrRegMainRspecIntStatusMutable *int_stat = &prs_main_int_status();
      //if (((err & kErrNoTcamMatch) != 0u) && (int_cfg->no_tcam_match_err_en()))
      //  int_stat->no_tcam_match_err(0x1);
      //if (((err & kErrTimeout) != 0u) && (int_cfg->timeout_err_en()))
      //  int_stat->timeout_err(0x1);
      //if (((err & kErrPartialHdr) != 0u) && (int_cfg->partial_hdr_err_en()))
      //  int_stat->partial_hdr_err(0x1);
      //if (((err & kErrCtrRange) != 0u) && (int_cfg->ctr_range_err_en()))
      //  int_stat->ctr_range_err(0x1);
      //if (((err & kErrMultiWr) != 0u) && (int_cfg->multi_wr_err_en()))
      //  int_stat->multi_wr_err(0x1);
      //if (((err & kErrFcs) != 0u) && (int_cfg->fcs_err_en()))
      //  int_stat->fcs_err(0x1);
      //if (((err & kErrMem) != 0u) && (int_cfg->ctr_mem_err_en()))
      //  int_stat->ctr_mem_err(0x1);
      //if (((err & kErrCsum) != 0u) && (int_cfg->csum_err_en()))
      //  int_stat->csum_err(0x1);
      //if (((err & kErrDstCont) != 0u) && (int_cfg->dst_cont_err_en()))
      //  int_stat->partial_hdr_err(0x1);
      //if (((err & kErrPhvOwner) != 0u) && (int_cfg->phv_owner_err_en()))
      //  int_stat->partial_hdr_err(0x1);
      //if (((err & kErrCsumConfig) != 0u) && (int_cfg->csum_config_err_en()))
      //  int_stat->partial_hdr_err(0x1);
    }
    inline void set_perr_interrupt_output(uint32_t cfg) {
      // prs_main_err_int_cfg_.write(0u,cfg);
    }

    template<typename T> inline void inc_cntr(uint32_t inc, T& cntr) {
          cntr.cnt(cntr.cnt() + inc);
    }
    inline void perr_count(uint32_t err, int inc=1) {
      while (err != 0u) {
        if      ((err & kErrNoTcamMatch) != 0u)   inc_cntr(inc, prs_main_no_tcam_match_err_cnt_);
        else if ((err & kErrPartialHdr) != 0u)    inc_cntr(inc, prs_main_partial_hdr_err_cnt_);
        else if ((err & kErrCtrRange) != 0u)      inc_cntr(inc, prs_main_ctr_range_err_cnt_);
        else if ((err & kErrTimeoutIter) != 0u)   inc_cntr(inc, prs_main_timeout_iter_err_cnt_);
        else if ((err & kErrTimeoutCycle) != 0u)  inc_cntr(inc, prs_main_timeout_cycle_err_cnt_);
        else if ((err & kErrSrcExt) != 0u)        inc_cntr(inc, prs_main_src_ext_err_cnt_);
        //else if ((err & kErrDstCont) != 0u)     inc_cntr(inc, prs_main_dst_cont_err_cnt_); // Use virtual func below
        else if ((err & kErrDstCont) != 0u)       prs_reg_err_cnt_inc(kErrDstCont, inc);
        else if ((err & kErrPhvOwner) != 0u)      inc_cntr(inc, prs_main_phv_owner_err_cnt_);
        else if ((err & kErrMultiWr) != 0u)       inc_cntr(inc, prs_main_multi_wr_err_cnt_);
        else if ((err & kErrARAMsbe) != 0u)       inc_cntr(inc, prs_main_aram_sbe_cnt_);
        else if ((err & kErrARAMmbe) != 0u)       inc_cntr(inc, prs_main_aram_mbe_cnt_);
        else if ((err & kErrFcs) != 0u)           inc_cntr(inc, prs_main_fcs_err_cnt_);
        // Checksum error accounting handled separately
        //else if ((err & kErrCsum) != 0u)        inc_cntr(inc, prs_main_csum_err_cnt_);
        // No ibuf error count regs seem to exist (also not sure about op_fifo uflow)
        //else if ((err & kErrIbufOflow) != 0u)   inc_cntr(inc, prs_main_ibuf_oflow_err_cnt_);
        //else if ((err & kErrIbufUflow) != 0u)   inc_cntr(inc, prs_main_ibuf_uflow_err_cnt_);
        else if ((err & kErrOpFifoOflow) != 0u)   inc_cntr(inc, prs_main_op_fifo_full_cnt_);
        //else if ((err & kErrOpFifoUflow) != 0u) inc_cntr(inc, prs_main_op_fifo_full_stall_cnt_);
        else if ((err & kErrTcamPar) != 0u)       inc_cntr(inc, prs_main_tcam_par_err_cnt_);
        else if ((err & kErrNarrowToWide) != 0u)  prs_reg_err_cnt_inc(kErrNarrowToWide, inc);
        else break;
        if      ((err & kErrNoTcamMatch) != 0u)   err &= ~kErrNoTcamMatch;
        else if ((err & kErrPartialHdr) != 0u)    err &= ~kErrPartialHdr;
        else if ((err & kErrCtrRange) != 0u)      err &= ~kErrCtrRange;
        else if ((err & kErrTimeoutIter) != 0u)   err &= ~kErrTimeoutIter;
        else if ((err & kErrTimeoutCycle) != 0u)  err &= ~kErrTimeoutCycle;
        else if ((err & kErrSrcExt) != 0u)        err &= ~kErrSrcExt;
        else if ((err & kErrDstCont) != 0u)       err &= ~kErrDstCont;
        else if ((err & kErrPhvOwner) != 0u)      err &= ~kErrPhvOwner;
        else if ((err & kErrMultiWr) != 0u)       err &= ~kErrMultiWr;
        else if ((err & kErrARAMsbe) != 0u)       err &= ~kErrARAMsbe;
        else if ((err & kErrARAMmbe) != 0u)       err &= ~kErrARAMmbe;
        else if ((err & kErrFcs) != 0u)           err &= ~kErrFcs;
        //else if ((err & kErrCsum) != 0u)        err &= ~kErrCsum;
        else if ((err & kErrIbufOflow) != 0u)     err &= ~kErrIbufOflow;
        else if ((err & kErrIbufUflow) != 0u)     err &= ~kErrIbufUflow;
        else if ((err & kErrOpFifoOflow) != 0u)   err &= ~kErrOpFifoOflow;
        else if ((err & kErrOpFifoUflow) != 0u)   err &= ~kErrOpFifoUflow;
        else if ((err & kErrTcamPar) != 0u)       err &= ~kErrTcamPar;
        else if ((err & kErrNarrowToWide) != 0u)  err &= ~kErrNarrowToWide;
      }
    }


    // ParserEarlyAction
    inline memory_classes::PrsrMlEaRowArrayMutable *get_prs_ea_ram() { return &prs_ea_ram_; }

    // Constants for counter_ctr_op (JBay 2b macroOPs)
    static constexpr uint8_t kCounter2AddOnly                =  0;
    static constexpr uint8_t kCounter2PopStackAdd            =  1;
    static constexpr uint8_t kCounter2LoadImmediate          =  2;
    static constexpr uint8_t kCounter2LoadFromCntrRam        =  3;
    // Constants for counter_ctr_op (WIP 4b macroOPs)
    static constexpr uint8_t kCounter4LoadImmediate          =  0;
    static constexpr uint8_t kCounter4LoadImmediatePop       =  1;
    static constexpr uint8_t kCounter4LoadImmediatePush      =  2;
    static constexpr uint8_t kCounter4AddImmediate           =  3;
    static constexpr uint8_t kCounter4AddImmediatePop        =  7;
    static constexpr uint8_t kCounter4LoadPacketApplyRam     =  4;
    static constexpr uint8_t kCounter4LoadPacketApplyRamPop  =  5;
    static constexpr uint8_t kCounter4LoadPacketApplyRamPush =  6;
    static constexpr uint8_t kCounter4LoadPacket             =  8;
    static constexpr uint8_t kCounter4LoadPacketPop          =  9;
    static constexpr uint8_t kCounter4LoadPacketPush         = 10;
    static constexpr uint8_t kCounter4Reserved0              = 11;
    static constexpr uint8_t kCounter4LoadPacketShift        = 12;
    static constexpr uint8_t kCounter4LoadPacketShiftPop     = 13;
    static constexpr uint8_t kCounter4LoadPacketShiftPush    = 14;
    static constexpr uint8_t kCounter4Reserved1              = 15;
    // MicroOPs used by counter_handle()
    // 2b/4b MacroOPs above are translated into these
    static constexpr uint32_t kCtrUopPop                 = 0x00000004u;
    static constexpr uint32_t kCtrUopPush                = 0x00000008u;
    static constexpr uint32_t kCtrUopZeroise0            = 0x00000020u;
    static constexpr uint32_t kCtrUopLoadImm             = 0x00000100u;
    static constexpr uint32_t kCtrUopLoadSearchByte      = 0x00000200u;
    static constexpr uint32_t kCtrUopLoadPktBuf          = 0x00000400u;
    static constexpr uint32_t kCtrUopCntrRamEarlyRotMask = 0x00002000u;
    static constexpr uint32_t kCtrUopApplyCntrRam        = 0x00004000u;
    static constexpr uint32_t kCtrUopCntrRamLateRotMask  = 0x00008000u;
    static constexpr uint32_t kCtrUopIncrImm0            = 0x00010000u;
    static constexpr uint32_t kCtrUopIncrStackImm        = 0x00020000u;
    static constexpr uint32_t kCtrUopIncrStackAddrBug    = 0x00040000u;
    static constexpr uint32_t kCtrUopIncrStackByAddMaybe = 0x00080000u;
    static constexpr uint32_t kCtrUopSetShift            = 0x01000000u;
    static constexpr uint32_t kCtrUopSetZeroNeg          = 0x02000000u;
    static constexpr uint32_t kCtrUopClearZeroNeg        = 0x04000000u;
    // Common microOP masks
    static constexpr uint32_t kCtrUopLoad                = 0x00000700u;
    static constexpr uint32_t kCtrUopUsesCounterRam      = 0x000C4200u;

    // CTR_AMT_IDX[7:3] may be used as index [0..31] into input buffer for looking up source byte for LoadPacket/LoadPacketShift OPs
    // However source byte for LoadPacketApplyRam OPs can only be one of 4x TCAM search bytes (this configured within CntrInitRAM entry)
    inline uint8_t counter_load_addr(int pi)          { return prs_ea_ram_.ctr_amt_idx(pi); }
    inline uint8_t counter_load_imm(int pi)           { return counter_load_addr(pi); }
    inline int8_t  counter_load_imm_int(int pi)       { return static_cast<int8_t>(counter_load_imm(pi)); }
    inline uint8_t counter_load_addr_msbs(int pi)     { return counter_load_addr(pi) >> kCounterInitAddrWidth; } // CtrAmtIdx[7:4]
    inline uint8_t counter_load_addr_lsbs(int pi)     { return counter_load_addr(pi)  & kCounterInitAddrMask; }  // CtrAmtIdx[3:0]
    inline bool    done(int pi)                       { return ((prs_ea_ram_.done(pi) & 0x1) == 0x1); }
    inline uint8_t shift_amount(int pi)               { return prs_ea_ram_.shift_amt(pi); }
    inline uint8_t buf_byte_offset(int pi)            { return counter_load_addr(pi) >> 3; }                     // CtrAmtIdx[7:3]
    inline uint8_t buf_byte_rot(int pi)               { return counter_load_addr_lsbs(pi) & 0x7; }               // CtrAmtIdx[2:0]
    inline uint8_t form_buf_byte_mask(uint8_t v,int w){ return static_cast<uint8_t>( (0xFF >> ((v>>(w-3))&7)) & (0xFF << ((v>>0)&7)) ); } // See uArch
    inline uint8_t buf_byte_norm_mask(int pi)         { return form_buf_byte_mask( prs_ea_field8_N_lookup_offset(pi,3), 6); }
    inline uint8_t buf_byte_shift_mask(int pi)        { return form_buf_byte_mask( shift_amount(pi), 6); }
    inline uint8_t buf_byte_norm(int pi, uint8_t v)   { return model_common::Util::rotr8(v,buf_byte_rot(pi)) & buf_byte_norm_mask(pi); }
    inline uint8_t buf_byte_shift(int pi, uint8_t v)  { return model_common::Util::rotr8(v,buf_byte_rot(pi)) & buf_byte_shift_mask(pi); }
    inline uint8_t field8_3_lookup_offset(int pi)     { return prs_ea_field8_N_lookup_offset(pi,3) & kLoadOffsetMask; }
    inline uint8_t field8_2_lookup_offset(int pi)     { return prs_ea_field8_N_lookup_offset(pi,2) & kLoadOffsetMask; }
    inline uint8_t field8_1_lookup_offset(int pi)     { return prs_ea_field8_N_lookup_offset(pi,1) & kLoadOffsetMask; }
    inline uint8_t field8_0_lookup_offset(int pi)     { return prs_ea_field8_N_lookup_offset(pi,0) & kLoadOffsetMask; }
    inline uint8_t field16_lookup_offset(int pi)      { return prs_ea_field16_lookup_offset(pi)    & kLoadOffsetMask; }
    inline bool    load_field8_3(int pi)              { return ((prs_ea_field8_N_load(pi,3) & 0x1) == 0x1); }
    inline bool    load_field8_2(int pi)              { return ((prs_ea_field8_N_load(pi,2) & 0x1) == 0x1); }
    inline bool    load_field8_1(int pi)              { return ((prs_ea_field8_N_load(pi,1) & 0x1) == 0x1); }
    inline bool    load_field8_0(int pi)              { return ((prs_ea_field8_N_load(pi,0) & 0x1) == 0x1); }
    inline bool    load_field16(int pi)               { return ((prs_ea_field16_load(pi) & 0x1) == 0x1); }
    inline uint8_t field8_3_msb(int pi, int bitpos)   { return (prs_ea_field8_N_lookup_offset(pi,3) >> kLoadOffsetWidth) & 0x1; }
    inline uint8_t field8_2_msb(int pi, int bitpos)   { return (prs_ea_field8_N_lookup_offset(pi,2) >> kLoadOffsetWidth) & 0x1; }
    inline uint8_t field8_1_msb(int pi, int bitpos)   { return (prs_ea_field8_N_lookup_offset(pi,1) >> kLoadOffsetWidth) & 0x1; }
    inline uint8_t field8_0_msb(int pi, int bitpos)   { return (prs_ea_field8_N_lookup_offset(pi,0) >> kLoadOffsetWidth) & 0x1; }
    inline uint8_t field16_msb(int pi, int bitpos)    { return (prs_ea_field16_lookup_offset(pi)  >> kLoadOffsetWidth) & 0x1; }
    inline uint8_t buf_req(int pi)                    { return prs_ea_ram_.buf_req(pi); }
    inline uint8_t next_state_mask(int pi)            { return prs_ea_ram_.nxt_state_mask(pi); }
    inline uint8_t next_state(int pi)                 { return prs_ea_ram_.nxt_state(pi); }
    inline uint8_t next_state(int pi, uint8_t state)  {
      return (state & ~next_state_mask(pi)) | (next_state(pi) & next_state_mask(pi));
    }
    // These need to be specialised PER-CHIP
    virtual bool     counter_stack_push(int pi) = 0;
    virtual bool     counter_stack_upd_w_top(int pi) = 0;
    virtual bool     cntrstack_push(int pi, int8_t cntr, bool propagate) = 0;
    virtual bool     cntrstack_maybe_push(int pi, int8_t cntr, int8_t incr) = 0;
    virtual void     cntrstack_increment(int pi, int8_t cntr) = 0;
    virtual int8_t   cntrstack_pop(int pi, int8_t cntr) = 0;
    virtual void     cntrstack_reset() { }
    virtual uint32_t cntrstack_hash() { return 0u; }
    virtual int      cntrstack_peek(int8_t cntrs[], bool propagates[], int size) { return 0; }
    virtual uint64_t cntrstack_counter_data(int8_t curr_cntr) {
      return (UINT64_C(1) << 56) | static_cast<uint8_t>(curr_cntr);
    }
    virtual uint32_t counter_ctr_uops(int pi) = 0;

    virtual const char *counter_ctr_op_str(uint8_t op) = 0;
    virtual uint8_t counter_ctr_op(int pi) = 0;
    virtual bool    counter_load_src(int pi) = 0;
    virtual bool    counter_load(int pi) = 0;
    virtual void    get_lookup_offset(int pi, int n, uint8_t *off, bool *load, int *which_buf) = 0;
    virtual uint8_t prs_ea_field8_N_lookup_offset(int pi, int n) = 0;
    virtual uint8_t prs_ea_field8_N_load(int pi, int n) = 0;
    virtual uint8_t prs_ea_field16_lookup_offset(int pi) = 0;
    virtual uint8_t prs_ea_field16_load(int pi) = 0;

    inline void set_counter_load_imm(int pi, uint8_t v)       { prs_ea_ram_.ctr_amt_idx(pi,v); }
    inline void set_counter_load_addr(int pi, uint8_t v)      { prs_ea_ram_.ctr_amt_idx(pi,v); }
    inline void set_done(int pi, bool tf)                     { prs_ea_ram_.done(pi,tf ?0x1 :0x0); }
    inline void set_shift_amount(int pi, uint8_t v)           { prs_ea_ram_.shift_amt(pi,v); }
    inline void set_field8_3_lookup_offset(int pi, uint8_t v) { prs_ea_set_field8_N_lookup_offset(pi,3,v); }
    inline void set_field8_2_lookup_offset(int pi, uint8_t v) { prs_ea_set_field8_N_lookup_offset(pi,2,v); }
    inline void set_field8_1_lookup_offset(int pi, uint8_t v) { prs_ea_set_field8_N_lookup_offset(pi,1,v); }
    inline void set_field8_0_lookup_offset(int pi, uint8_t v) { prs_ea_set_field8_N_lookup_offset(pi,0,v); }
    inline void set_field16_lookup_offset(int pi, uint8_t v)  { prs_ea_set_field16_lookup_offset(pi,v); }
    inline void set_load_field8_3(int pi, bool tf)            { prs_ea_set_field8_N_load(pi,3,tf ?0x1 :0x0); }
    inline void set_load_field8_2(int pi, bool tf)            { prs_ea_set_field8_N_load(pi,2,tf ?0x1 :0x0); }
    inline void set_load_field8_1(int pi, bool tf)            { prs_ea_set_field8_N_load(pi,1,tf ?0x1 :0x0); }
    inline void set_load_field8_0(int pi, bool tf)            { prs_ea_set_field8_N_load(pi,0,tf ?0x1 :0x0); }
    inline void set_load_field16(int pi, bool tf)             { prs_ea_set_field16_load(pi,tf ?0x1 :0x0); }
    inline void set_buf_req(int pi, uint8_t v)                { prs_ea_ram_.buf_req(pi,v); }
    inline void set_next_state_mask(int pi, uint8_t v)        { prs_ea_ram_.nxt_state_mask(pi,v); }
    inline void set_next_state(int pi, uint8_t v)             { prs_ea_ram_.nxt_state(pi,v); }

    // These need to be specialised PER-CHIP
    virtual void set_counter_ctr_op(int pi, uint8_t v) = 0;
    virtual void set_counter_load_src(int pi, bool tf) = 0;
    virtual void set_counter_load(int pi, bool tf) = 0;
    virtual void set_counter_stack_push(int pi, bool tf) = 0;
    virtual void set_counter_stack_upd_w_top(int pi, bool tf) = 0;

    virtual void set_lookup_offset(int pi, int n, uint8_t off, bool load, int which_buf) = 0;
    virtual void prs_ea_set_field8_N_lookup_offset(int pi, int n, uint8_t v) = 0;
    virtual void prs_ea_set_field8_N_load(int pi, int n, uint8_t v) = 0;
    virtual void prs_ea_set_field16_lookup_offset(int pi, uint8_t v) = 0;
    virtual void prs_ea_set_field16_load(int pi, uint8_t v) = 0;

    // Get/Set what ActionRam banks are enabled - *ALL* enabled on Tofino/JBay
    // Only significant on WIP
    virtual uint8_t  action_ram_en(int pi) = 0;
    virtual void set_action_ram_en(int pi, uint8_t en) = 0;


    // ParserAction
    inline memory_classes::PrsrPoActionRowArrayMutable  *get_prs_act_ram() { return &prs_action_ram_; }

    // Constants for extract_type
    static constexpr uint8_t kExtractTypeNone    = 0x0;
    static constexpr uint8_t kExtractType8bToLSB = 0x1;
    static constexpr uint8_t kExtractType8bToMSB = 0x2;
    static constexpr uint8_t kExtractType16b     = 0x3;
    static constexpr uint8_t kExtractType8b      = 0x4;
    static constexpr uint8_t kExtractType32b     = 0x5;

    // These need to be specialised PER-CHIP
    virtual bool     extract_offset_check(uint8_t xt_type, int off, bool *allow_partial,
                                          uint8_t buf_occ = kInputBufferSize) = 0;

    virtual int      extract8_first_non_checksum(int n_xt, int ck8, int ck16, int ck32) = 0;
    virtual int      extract16_first_non_checksum(int n_xt, int ck8, int ck16, int ck32) = 0;
    virtual int      extract32_first_non_checksum(int n_xt, int ck8, int ck16, int ck32) = 0;
    virtual bool     extract8_available(int i_xt, int n_xt, int ck8, int ck16, int ck32) = 0;
    virtual bool     extract16_available(int i_xt, int n_xt, int ck8, int ck16, int ck32) = 0;
    virtual bool     extract32_available(int i_xt, int n_xt, int ck8, int ck16, int ck32) = 0;
    virtual bool     extract8_src_check_needed(int i_xt, int n_xt, int ck8, int ck16, int ck32) {
        return extract8_available(i_xt, n_xt, ck8, ck16, ck32);
    }
    virtual bool     extract16_src_check_needed(int i_xt, int n_xt, int ck8, int ck16, int ck32) {
        return extract16_available(i_xt, n_xt, ck8, ck16, ck32);
    }
    virtual bool     extract32_src_check_needed(int i_xt, int n_xt, int ck8, int ck16, int ck32) {
        return extract32_available(i_xt, n_xt, ck8, ck16, ck32);
    }

    virtual bool     extract8_src_imm_val(int pi,int i) = 0;
    virtual bool     extract8_rot_imm_val(int pi,int i) = 0;
    virtual bool     extract8_add_off(int pi, int i) = 0;
    virtual uint8_t  extract8_src(int pi, int i, uint8_t mask) = 0;
    virtual uint8_t  extract8_src_msb(int pi, int i, int bitpos) = 0;
    virtual uint16_t extract8_dst_phv(int pi, int i) = 0;
    virtual uint8_t  extract8_type(int pi, int i) = 0;
    virtual bool     extract16_src_imm_val(int pi,int i) = 0;
    virtual bool     extract16_rot_imm_val(int pi,int i) = 0;
    virtual bool     extract16_add_off(int pi, int i) = 0;
    virtual uint8_t  extract16_src(int pi, int i, uint8_t mask) = 0;
    virtual uint8_t  extract16_src_msb(int pi, int i, int bitpos) = 0;
    virtual uint16_t extract16_dst_phv(int pi, int i) = 0;
    virtual uint16_t extract16_dst_phv_by_phv(int pi, int i) = 0;
    virtual uint8_t  extract16_type(int pi, int i) = 0;
    virtual bool     extract32_src_imm_val(int pi,int i) = 0;
    virtual bool     extract32_rot_imm_val(int pi,int i) = 0;
    virtual bool     extract32_add_off(int pi, int i) = 0;
    virtual uint8_t  extract32_src(int pi, int i, uint8_t mask) = 0;
    virtual uint8_t  extract32_src_msb(int pi, int i, int bitpos) = 0;
    virtual uint16_t extract32_dst_phv(int pi, int i) = 0;
    virtual uint8_t  extract32_type(int pi, int i) = 0;
    virtual uint8_t  extract8_src_const(int pi, int i, uint8_t mask, int rotr) = 0;
    virtual uint16_t extract16_src_const(int pi, int i, uint8_t mask, int rotr) = 0;
    virtual uint32_t extract32_src_const(int pi, int i, uint8_t mask, int rotr) = 0;
    virtual uint8_t  extract8_inbuf_shift(int pi, int i) = 0;
    virtual uint8_t  extract16_inbuf_shift(int pi, int i) = 0;
    virtual uint8_t  extract32_inbuf_shift(int pi, int i) = 0;
    virtual uint32_t extract_immediate_consts(int pi, int i, int rotr, uint32_t dflt) = 0;

    virtual void set_extract8_src_imm_val(int pi, int i, bool tf) = 0;
    virtual void set_extract8_rot_imm_val(int pi, int i, bool tf) = 0;
    virtual void set_extract8_add_off(int pi, int i, bool tf) = 0;
    virtual void set_extract8_src(int pi, int i, uint8_t v) = 0;
    virtual void set_extract8_dst_phv(int pi, int i, uint16_t v) = 0;
    virtual void set_extract8_type(int pi, int i, uint8_t v) = 0;
    virtual void set_extract16_src_imm_val(int pi, int i, bool tf) = 0;
    virtual void set_extract16_rot_imm_val(int pi, int i, bool tf) = 0;
    virtual void set_extract16_add_off(int pi, int i, bool tf) = 0;
    virtual void set_extract16_src(int pi, int i, uint8_t v) = 0;
    virtual void set_extract16_dst_phv(int pi, int i, uint16_t v) = 0;
    virtual uint8_t set_extract16_dst_phv_by_phv(int pi, int i, uint16_t v) = 0;
    virtual void set_extract16_type(int pi, int i, uint8_t v) = 0;
    virtual void set_extract32_src_imm_val(int pi, int i, bool tf) = 0;
    virtual void set_extract32_rot_imm_val(int pi, int i, bool tf) = 0;
    virtual void set_extract32_add_off(int pi, int i, bool tf) = 0;
    virtual void set_extract32_src(int pi, int i, uint8_t v) = 0;
    virtual void set_extract32_dst_phv(int pi, int i, uint16_t v) = 0;
    virtual void set_extract32_type(int pi, int i, uint8_t v) = 0;

    inline int   offset_adjust()  { return offset_adjust_; } // Get current value

    // PER-CHIP specialisation of offset/checksum/pri_upd funcs
    // (these now need to be per-chip because of WIP act_ram reorganisation)
    virtual bool    offset_reset(int pi) = 0;
    virtual uint8_t offset_incr_val(int pi) = 0;
    virtual bool    checksum_enable(int pi, int i) = 0;
    virtual uint8_t checksum_ram_addr(int pi, int i) = 0;
    virtual uint8_t pri_upd_type(int pi) = 0;
    virtual uint8_t pri_upd_src(int pi) = 0;
    virtual uint8_t pri_upd_en_shr(int pi) = 0;
    virtual uint8_t pri_upd_val_mask(int pi) = 0;

    virtual void set_offset_reset(int pi, bool tf) = 0;
    virtual void set_offset_incr_val(int pi, uint8_t v) = 0;
    virtual void set_checksum_enable(int pi, int i, bool tf) = 0;
    virtual void set_checksum_ram_addr(int pi, int i, uint8_t v) = 0;
    virtual void set_pri_upd_type(int pi, uint8_t v) = 0;
    virtual void set_pri_upd_src(int pi, uint8_t v) = 0;
    virtual void set_pri_upd_en_shr(int pi, uint8_t v) = 0;
    virtual void set_pri_upd_val_mask(int pi, uint8_t v) = 0;

    // PER-CHIP specialisation of extraction funcs
    virtual bool extract_ok_phv(int phv_word) = 0;
    virtual bool extract_ok_tphv(int phv_word) = 0;
    virtual int  get_phv_to_write(int phv_word) = 0;
    virtual int  get_tphv_to_write(int phv_word) = 0;

    // Wrapper funcs to make above more usable
    inline bool extract_ok(int phv_word) {
      return extract_ok_phv(phv_word) || extract_ok_tphv(phv_word);
    }
    inline bool extract8(int pi, int i)   { return (extract_ok(extract8_dst_phv(pi,i))); }
    inline bool extract16(int pi, int i)  { return (extract_ok(extract16_dst_phv(pi,i))); }
    inline bool extract32(int pi, int i)  { return (extract_ok(extract32_dst_phv(pi,i))); }

    inline int extract_first_non_checksum(int sz, int n_xt, int ck8, int ck16, int ck32) {
      if    (n_xt == 0)  return -1;
      if      (sz == 8)  return  extract8_first_non_checksum(n_xt, ck8, ck16, ck32);
      else if (sz == 16) return extract16_first_non_checksum(n_xt, ck8, ck16, ck32);
      else if (sz == 32) return extract32_first_non_checksum(n_xt, ck8, ck16, ck32);
      else return -1;
    }
    inline bool extract_available(int sz, int i_xt, int n_xt, int ck8, int ck16, int ck32) {
      if    (n_xt == 0)  return false;
      if      (sz == 8)  return  extract8_available(i_xt, n_xt, ck8, ck16, ck32);
      else if (sz == 16) return extract16_available(i_xt, n_xt, ck8, ck16, ck32);
      else if (sz == 32) return extract32_available(i_xt, n_xt, ck8, ck16, ck32);
      else return false;
    }
    inline bool extract_src_check_needed(int sz, int i_xt, int n_xt, int ck8, int ck16, int ck32) {
      if    (n_xt == 0)  return false;
      if      (sz == 8)  return  extract8_src_check_needed(i_xt, n_xt, ck8, ck16, ck32);
      else if (sz == 16) return extract16_src_check_needed(i_xt, n_xt, ck8, ck16, ck32);
      else if (sz == 32) return extract32_src_check_needed(i_xt, n_xt, ck8, ck16, ck32);
      else return false;
    }
    inline bool extract(int sz, int pi, int i) {
      if      (sz == 8)  return extract8(pi, i);
      else if (sz == 16) return extract16(pi, i);
      else if (sz == 32) return extract32(pi, i);
      else return false;
    }
    inline uint8_t extract_type(int sz, int pi, int i) {
      if      (sz == 8)  return extract8_type(pi, i);
      else if (sz == 16) return extract16_type(pi, i);
      else if (sz == 32) return extract32_type(pi, i);
      else return kExtractTypeNone;
    }
    inline bool extract_src_imm_val(int sz, int pi, int i) {
      if      (sz == 8)  return extract8_src_imm_val(pi, i);
      else if (sz == 16) return extract16_src_imm_val(pi, i);
      else if (sz == 32) return extract32_src_imm_val(pi, i);
      else return false;
    }
    inline bool extract_rot_imm_val(int sz, int pi, int i) {
      if      (sz == 8)  return extract8_rot_imm_val(pi, i);
      else if (sz == 16) return extract16_rot_imm_val(pi, i);
      else if (sz == 32) return extract32_rot_imm_val(pi, i);
      else return false;
    }
    inline bool extract_add_off(int sz, int pi, int i) {
      if      (sz == 8)  return extract8_add_off(pi, i);
      else if (sz == 16) return extract16_add_off(pi, i);
      else if (sz == 32) return extract32_add_off(pi, i);
      else return false;
    }
    inline uint8_t extract_src(int sz, int pi, int i, uint8_t mask) {
      if      (sz == 8)  return extract8_src(pi, i, mask);
      else if (sz == 16) return extract16_src(pi, i, mask);
      else if (sz == 32) return extract32_src(pi, i, mask);
      else return 0xFF;
    }
    inline uint8_t extract_src_msb(int sz, int pi, int i, int bitpos) {
      if      (sz == 8)  return extract8_src_msb(pi, i, bitpos);
      else if (sz == 16) return extract16_src_msb(pi, i, bitpos);
      else if (sz == 32) return extract32_src_msb(pi, i, bitpos);
      else return 0x0;
    }
    inline uint16_t extract_dst_phv(int sz, int pi, int i) {
      if      (sz == 8)  return extract8_dst_phv(pi, i);
      else if (sz == 16) return extract16_dst_phv(pi, i);
      else if (sz == 32) return extract32_dst_phv(pi, i);
      else return k_phv::kBadPhv;
    }
    inline uint8_t extract_inbuf_shift(int sz, int pi, int i) {
      if      (sz == 8)  return extract8_inbuf_shift(pi, i);
      else if (sz == 16) return extract16_inbuf_shift(pi, i);
      else if (sz == 32) return extract32_inbuf_shift(pi, i);
      else return 0;
    }


    // PER-CHIP specialisation of Version Update function
    // (Tofino does NOT support this)
    virtual uint8_t update_version(int index, uint8_t curr_ver) = 0;

    // PER-CHIP specialisation of Priority Map function
    // (Tofino does NOT support this)
    virtual uint8_t map_priority(int chan, uint8_t curr_pri) = 0;

    // PER-CHIP specialisation of function to set identity Priority Map
    // (Tofino does nothing)
    virtual void set_identity_priority_map() = 0;

    inline  bool    is_hdr_len_inc_stopped()    { return hdr_len_inc_stopped_; }
    inline  void    set_hdr_len_inc_stopped(bool tf)  { hdr_len_inc_stopped_ = tf; }
    inline  void    hdr_len_inc_stopped_reset()       { set_hdr_len_inc_stopped(false); }
    void adjust_hdr_len(Packet *p, int chan);
    virtual void update_hdr_len(Packet *p, int index, uint32_t err_flags, uint8_t shift) = 0;
    // overridden for jbay/WIP, default impls here for compatibility...
    virtual bool hdr_len_inc(int pi) { return true; };
    virtual void set_hdr_len_inc(int pi, bool tf) { };


    // PER-CHIP specialisation of disable_partial_hdr_err func - only JBay again
    virtual bool    disable_partial_hdr_err(int pi)               { return false; }
    virtual void    set_disable_partial_hdr_err(int pi, bool tf)  { }

    virtual bool partial_hdr_err_proc(int pi) { return false; }
    virtual void set_partial_hdr_err_proc(int pi, bool tf) { }

    // ParserCounterInit
    inline memory_classes::PrsrMlCtrInitRamMArrayMutable  *get_prs_ctr_init_ram() { return &prs_ctr_init_ram_; }
    inline uint8_t  pcnt_calc_mask(uint8_t width)  { return static_cast<uint8_t>((1u << (width + 1)) - 1); }

    // PER-CHIP specialisation of mask/mask_width funcs
    virtual uint8_t  pcnt_mask_width(int pi) = 0;
    virtual uint8_t  pcnt_mask(int pi) = 0;
    virtual void set_pcnt_mask_width(int pi, uint8_t v) = 0;
    virtual void set_pcnt_mask(int pi, uint8_t v) = 0;
    virtual uint8_t  pcnt_add_to_stack(int pi) = 0;
    virtual void set_pcnt_add_to_stack(int pi, uint8_t v) = 0;


    inline uint8_t pcnt_source(int pi)             { return prs_ctr_init_ram_.src(pi); }
    inline uint8_t pcnt_add(int pi)                { return prs_ctr_init_ram_.add(pi); }
    inline int8_t  pcnt_add_int(int pi)            { return static_cast<int8_t>(pcnt_add(pi)); }
    inline uint8_t pcnt_rotate(int pi)             { return prs_ctr_init_ram_.rotate(pi); }
    inline uint8_t pcnt_max(int pi)                { return prs_ctr_init_ram_.max(pi); }
    inline uint8_t pcnt_choose_field(int pi, uint8_t f8_3, uint8_t f8_2, uint16_t f16) {
      switch (pcnt_source(pi) & 0x3) {
        case 0x0: return (uint8_t)(f16 & 0xFF);
        case 0x1: return (uint8_t)(f16 >> 8);
        case 0x2: return f8_2;
        case 0x3: return f8_3;
        default:  return 0xFF; // IMPOSSIBLE
      }
    }
    inline uint8_t pcnt_rotmask_counter_from_field(int pi, uint8_t fv) {
      return model_common::Util::rotr8(fv, pcnt_rotate(pi)) & pcnt_mask(pi);
    }
    inline int8_t pcnt_addto_counter_from_field(int pi, uint8_t fv) {
      return static_cast<int8_t>(fv + pcnt_add_int(pi));
    }
    inline int8_t pcnt_calculate_counter_from_field(int pi, uint8_t fv) {
      return pcnt_addto_counter_from_field( pi, pcnt_rotmask_counter_from_field(pi, fv) );
    }
    inline int8_t pcnt_calculate_counter(int pi, uint8_t f8_3, uint8_t f8_2, uint16_t f16) {
      return pcnt_calculate_counter_from_field( pi, pcnt_choose_field(pi, f8_3, f8_2, f16) );
    }
    inline bool pcnt_is_valid_counter(int pi, uint8_t cntr_val) {
      return (cntr_val <= pcnt_max(pi));
    }
    inline bool pcnt_is_valid_counter(int pi, uint8_t f8_3, uint8_t f8_2, uint16_t f16) {
      return pcnt_is_valid_counter( pi, pcnt_choose_field(pi, f8_3, f8_2, f16) );
    }

    inline void set_pcnt_source(int pi, uint8_t v) { prs_ctr_init_ram_.src(pi,v); }
    inline void set_pcnt_add(int pi, uint8_t v)    { prs_ctr_init_ram_.add(pi,v); }
    inline void set_pcnt_rotate(int pi, uint8_t v) { prs_ctr_init_ram_.rotate(pi,v); }
    inline void set_pcnt_max(int pi, uint8_t v)    { prs_ctr_init_ram_.max(pi,v); }



    // ChecksumEngine
    virtual ChecksumEngine *get_checksum_engine(int ci) = 0;

    inline ChecksumEngine *cksmeng_(int ci)    { return get_checksum_engine(ci); }
    inline void cksmeng_reset_checksum(int ci) {
      cksmeng_(ci)->reset_checksum();
    }
    inline void cksmeng_start_checksum(int ci, Phv *phv) {
      cksmeng_(ci)->start_checksum(phv);
    }
    inline int cksmeng_stop_checksum(int ci, Packet *packet, int pkt_furthest_pos)  {
      return cksmeng_(ci)->stop_checksum(packet, pkt_furthest_pos);
    }
    inline int cksmeng_do_checksum(int match_index, int ci, int pcksm_index,
                                   int &wrote_8b, int &wrote_16b, int &wrote_32b,
                                   int shift_from_pkt)  {
      return cksmeng_(ci)->do_checksum(match_index, ci, pcksm_index,
                                       wrote_8b, wrote_16b, wrote_32b,
                                       shift_from_pkt);
    }


    // PER-CHIP specialisation of function to apply other early (pre-extraction) checks
    // (In Tofino no other errors - currently only specialised on JBay)
    virtual uint32_t other_early_error_checks(int match_index) { return 0u; }

    // PER-CHIP specialisation of checksum checking func (only TofinoA0/TofinoB0)
    virtual bool check_checksum_constraints(int match_index,
                                            int &wrote_8b, int &wrote_16b, int &wrote_32b) = 0;


    // ParserTCAM
    inline bool is_valid(int index) const { return tcam_match_.is_valid(index); }
    inline void print() {
      for (int i = 0; i < kStates; i++) {
        if (is_valid(i)) {
          RMT_LOG_VERBOSE("Parser<%d>\n", i);
          tcam_match_.print(i);
          //pa_(i)->print();
        }
      }
    }

    inline BitVector<44> make_tcam_entry(const uint64_t v) {
      return BitVector<44>(std::array<uint64_t,1>({{v}}));
    }
    // This func sets up Tcam bits in same order as H/W
    // which makes things less confusing for debugging etc
    inline BitVector<44> make_tcam_entry(bool ver1, bool ver0,
                                         bool cnt_eq_0, bool cnt_lt_0,
                                         uint8_t state,
                                         uint8_t v8_3, uint8_t v8_2,
                                         uint8_t v8_1, uint8_t v8_0) {
      uint64_t w = UINT64_C(0);
      w |= (static_cast<uint64_t>(v8_0)  <<  0);
      w |= (static_cast<uint64_t>(v8_1)  <<  8);
      w |= (static_cast<uint64_t>(v8_2)  << 16);
      w |= (static_cast<uint64_t>(v8_3)  << 24);
      w |= (static_cast<uint64_t>(state) << 32);
      if (cnt_eq_0)    w |= (UINT64_C(1) << 40);
      if (cnt_lt_0)    w |= (UINT64_C(1) << 41);
      if (ver0)        w |= (UINT64_C(1) << 42);
      if (ver1)        w |= (UINT64_C(1) << 43);
      return make_tcam_entry(w);
    }
    inline BitVector<44> make_tcam_entry(bool ver1, bool ver0,
                                         bool cnt_eq_0, bool cnt_lt_0,
                                         uint8_t state,
                                         uint16_t v16, uint8_t v8_1, uint8_t v8_0) {
      // Map v8_1,v8_0 to v8_3,v8_2 and split v16 to form new v8_1,v8_0
      return make_tcam_entry(ver1, ver0, cnt_eq_0, cnt_lt_0, state, v8_1, v8_0,
                             static_cast<uint8_t>(v16 >> 8),
                             static_cast<uint8_t>(v16 & 0xFF));
    }
    inline BitVector<44> make_tcam_entry(uint8_t state,
                                         uint16_t v16, uint8_t v8_1, uint8_t v8_0) {
      // Primarily for setting mask values where we don't care about Ver/Cnt
      // XXX: VER_1 and VER_0 both false/0 by default
      return make_tcam_entry(false, false, false, false, state, v16, v8_1, v8_0);
    }

    inline bool valid_tcam_entry(const BitVector<44> &value, const BitVector<44> &mask) {
      // Either version bit set indicates tcam entry is valid
      return (value.bit_set(43) || value.bit_set(42));
    }


    // Program s/w TCAM
    void get_tcam_word0_word1(int index, BitVector<44> *word0, BitVector<44> *word1);
    void set_tcam_word0_word1(int index, const BitVector<44> &word0, const BitVector<44> &word1);
    void set_tcam_value_mask(int index, const BitVector<44> &value, const BitVector<44> &mask);
    void set_tcam_match(int index, const BitVector<44> &value, const BitVector<44> &mask);

    void set_parser_mode(uint8_t _mode, uint16_t _max_iter);
    void set_channel(int chan, bool _enabled, uint8_t _start_state);
    void set_channel_congested(int chan, bool _congested=true);



    // Common EarlyActionRAM initialisation func
    void set_early_action_shared(int index, uint8_t _counter_load_imm,
                                 bool _counter_load_src, bool _counter_load,
                                 bool _done, uint8_t _shift_amount,
                                 uint8_t _next_state_mask, uint8_t _next_state);
    void set_early_action_shared(int index, uint8_t _counter_load_imm, uint8_t _counter_ctr_op,
                                 bool _done, uint8_t _shift_amount,
                                 uint8_t _next_state_mask, uint8_t _next_state);

    // PER-CHIP specialisations of EarlyActionRAM initialisation to handle different chips
    // having different sets of lookup_offset and ld_lookup register fields.
    //
    virtual void set_early_action(int index, uint8_t _counter_load_imm,
                                  bool _counter_load_src, bool _counter_load,
                                  bool _done, uint8_t _shift_amount,
                                  uint8_t _field8_1_lookup_offset, uint8_t _field8_0_lookup_offset,
                                  uint8_t _field16_lookup_offset,
                                  bool _load_field8_1, bool _load_field8_0, bool _load_field16,
                                  uint8_t _next_state_mask, uint8_t _next_state) = 0;
    virtual void set_early_action(int index, uint8_t _counter_load_imm,
                                  bool _counter_load_src, bool _counter_load,
                                  bool _done, uint8_t _shift_amount,
                                  uint8_t _field8_3_lookup_offset, uint8_t _field8_2_lookup_offset,
                                  uint8_t _field8_1_lookup_offset, uint8_t _field8_0_lookup_offset,
                                  bool _load_field8_3, bool _load_field8_2,
                                  bool _load_field8_1, bool _load_field8_0,
                                  uint8_t _next_state_mask, uint8_t _next_state) = 0;


    // PER-CHIP specialisation of function to map dst_phv field
    // with from_format = ChipType
    virtual uint16_t map_dst_phv(uint16_t phv_in, int from_format) = 0;

    virtual void set_action(int index,
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
    virtual void set_action_extra(int index,
                                  bool _extract8_rot_imm_val[],
                                  bool _extract16_rot_imm_val[],
                                  bool _extract32_rot_imm_val[]);
    virtual void set_action_pri(int index,
                                uint8_t pri_upd_type, uint8_t pri_upd_src,
                                uint8_t pri_upd_en_shr, uint8_t pri_upd_val_mask);
    virtual void set_action_ver(int index,
                                uint8_t ver_upd_type, uint8_t ver_upd_src,
                                uint8_t ver_upd_en_shr, uint8_t ver_upd_val_mask);
    virtual void set_action_clot(int index, int clot_index,
                                 uint8_t clot_type,
                                 uint8_t clot_len_src, uint8_t clot_en_len_shr,
                                 uint8_t clot_len_mask, uint8_t clot_len_add,
                                 uint8_t clot_offset,
                                 uint8_t clot_tag, bool clot_tag_offset_add,
                                 uint8_t clot_has_csum);
    // PER-CHIP CLOT handling
    virtual int  clot_handle(int index, Packet *packet) { return 0; }
    virtual void clot_print(Packet *packet)             { }


    void set_counter_init(int counter_index,
                          uint8_t _add, uint8_t _mask_width, uint8_t _rotate,
                          uint8_t _max, uint8_t _source, uint8_t _add_to_stack);

    // regs_5794_main:
    // - now have 2 checksum RAMs so need to add checksum_ram_index to this func
    void set_checksum(int checksum_ram_index, int checksum_index,
                      uint16_t _shift_left, uint32_t _swap, uint32_t _mask,
                      bool _rotate, uint16_t _add, uint8_t _dst_bit_final_pos, bool _final,
                      uint16_t _dst_phv, bool _dst_update, bool _residual, bool _start, uint32_t mul2=0);
    // regs_5794_main: this func now becomes a wrapper that programs both checksum RAMs
    void set_checksum(int checksum_index,
                      uint16_t _shift_left, uint32_t _swap, uint32_t _mask,
                      bool _rotate, uint16_t _add, uint8_t _dst_bit_final_pos, bool _final,
                      uint16_t _dst_phv, bool _dst_update, bool _residual, bool _start);


    // Shim funcs to facilitate access to input buffers
    // These all default to using the 'normal' input buffer, buffer 0

    // Read byte/short/int/long at pos in [read_bytes_ write_bytes_]
    template <typename T> bool inbuf_get(int pos, T *val, bool load=true, bool partial=false, int which_buf=0) {
      static_assert(std::is_integral<T>::value, "only integral types allowed" );
      static_assert(!std::is_same< T, bool >::value, "bool not allowed");
      RMT_ASSERT ((which_buf >= 0) && (which_buf < kInputBuffers));
      return inbufs_[which_buf].get(pos, val, load, partial);
    }
    inline int inbuf_getsize(int which_buf=0) {
      RMT_ASSERT ((which_buf >= 0) && (which_buf < kInputBuffers));
      return inbufs_[which_buf].getsize();
    }
    inline int inbuf_getpos(int relpos, int which_buf=0) {
      RMT_ASSERT ((which_buf >= 0) && (which_buf < kInputBuffers));
      return inbufs_[which_buf].getpos(relpos);
    }
    // Max byte ever got - used by residual calc
    inline int inbuf_getmaxpos(int which_buf=0) {
      RMT_ASSERT ((which_buf >= 0) && (which_buf < kInputBuffers));
      return inbufs_[which_buf].getmaxpos();
    }
    inline void inbuf_reset(int which_buf=0) {
      RMT_ASSERT ((which_buf >= 0) && (which_buf < kInputBuffers));
      return inbufs_[which_buf].reset();
    }
    inline int inbuf_fill(Packet *p, int ppos, int which_buf=0) {
      RMT_ASSERT ((which_buf >= 0) && (which_buf < kInputBuffers));
      return inbufs_[which_buf].fill(p, ppos, InputBuffer::kInputBufferSize);
    }
    inline int inbuf_shift(int shift_bytes, int which_buf=0) {
      RMT_ASSERT ((which_buf >= 0) && (which_buf < kInputBuffers));
      return inbufs_[which_buf].shift(shift_bytes);
    }
    inline int inbuf_copy(int to_buf, int from_buf=0, int nbytes=InputBuffer::kInputBufferSize) {
      RMT_ASSERT((to_buf >= 0) && (to_buf < kInputBuffers));
      RMT_ASSERT((from_buf >= 0) && (from_buf < kInputBuffers));
      RMT_ASSERT(nbytes >= 0);
      int copied_bytes = 0;
      uint8_t byte;
      for (int pos = 0; pos < nbytes; pos++) {
        inbufs_[from_buf].get(pos, &byte, true, true);
        copied_bytes += inbufs_[to_buf].fill_byte(byte);
      }
      return copied_bytes;
    }
    InputBuffer *get_inbuf(int which_buf) {
      RMT_ASSERT ((which_buf >= 0) && (which_buf < kInputBuffers));
      return &inbufs_[which_buf];
    }

    // PER-CHIP specialisation of inbuf0 initial loading
    virtual void inbuf0_maybe_get_initial(int chan,
                                          uint8_t *v8_3, uint8_t *v8_2,
                                          uint8_t *v8_1, uint8_t *v8_0) = 0;
    // PER-CHIP specialisation of inbuf1 (scratch pad buffer) handling
    virtual void inbuf1_setup(Packet *p) = 0;
    virtual void inbuf1_maybe_fill(int pi,
                                   uint8_t v8_3, uint8_t v8_2,
                                   uint8_t v8_1, uint8_t v8_0, int state_cnt) = 0;
    // PER-CHIP specialisation of inbuf2 (TS/Version buf) handling
    virtual void inbuf2_setup(Packet *p) = 0;
    virtual void inbuf2_update_consts(uint32_t) = 0;


    // EXTRACT_INFO
    inline ExtractInfo  *get_extract_info() { return &extract_info_; }

    // PHV_OWNER/NO_MULTI_WR
    inline register_classes::PrsrRegMainRspecPhvOwnerMutable *get_phv_owner() {
      return &prs_main_phv_owner_;
    }
    inline register_classes::PrsrRegMainRspecNoMultiWrMutable *get_no_multi_wr() {
      return &prs_main_no_multi_wr_;
    }
    // PER-CHIP specialisation of Normal and Tagalong field handling
    virtual uint8_t prs_reg_phv_owner_norm_get(register_classes::PrsrRegMainRspecPhvOwnerMutable *r, int w) = 0;
    virtual void    prs_reg_phv_owner_norm_set(register_classes::PrsrRegMainRspecPhvOwnerMutable *r, int w, uint8_t v) = 0;
    virtual uint8_t prs_reg_no_multi_wr_norm_get(register_classes::PrsrRegMainRspecNoMultiWrMutable *r, int w) = 0;
    virtual void    prs_reg_no_multi_wr_norm_set(register_classes::PrsrRegMainRspecNoMultiWrMutable *r, int w, uint8_t v) = 0;
    virtual uint8_t prs_reg_phv_owner_taga_get(register_classes::PrsrRegMainRspecPhvOwnerMutable *r, int w) = 0;
    virtual void    prs_reg_phv_owner_taga_set(register_classes::PrsrRegMainRspecPhvOwnerMutable *r, int w, uint8_t v) = 0;
    virtual uint8_t prs_reg_no_multi_wr_taga_get(register_classes::PrsrRegMainRspecNoMultiWrMutable *r, int w) = 0;
    virtual void    prs_reg_no_multi_wr_taga_set(register_classes::PrsrRegMainRspecNoMultiWrMutable *r, int w, uint8_t v) = 0;


    // PHV ownership handling, error handling, interrupt handling
    inline uint8_t phv_owner_val(int phv_or_off) {
      uint8_t val = 0;
      if (Phv::is_valid_taga_phv_p(phv_or_off))
        val = prs_reg_phv_owner_taga_get(&prs_main_phv_owner_, phv_or_off);
      else
        val = prs_reg_phv_owner_norm_get(&prs_main_phv_owner_, phv_or_off);
      // Check same as replicated reg in prs_merge
      if (prs_merge_reg_ != NULL)
        RMT_ASSERT(val == prs_merge_reg_->phv_owner(phv_or_off));
      return val;
    }
    inline bool phv_clr_on_wr(int phv_or_off) {
      return (prs_merge_reg_ != NULL) ?prs_merge_reg_->phv_clr_on_wr(phv_or_off) :false;
    }
    inline uint8_t phv_multi_write_val(int phv_or_off) {
      uint8_t val = 0;
      if (Phv::is_valid_taga_phv_p(phv_or_off))
        val = prs_reg_no_multi_wr_taga_get(&prs_main_no_multi_wr_, phv_or_off);
      else
        val = prs_reg_no_multi_wr_norm_get(&prs_main_no_multi_wr_, phv_or_off);
      return val;
    }
    inline bool phv_init_valid(int phv_or_off) {
      return (prs_merge_reg_ != NULL) ?prs_merge_reg_->phv_init_valid(phv_or_off) :false;
    }
    inline void phv_set_owner(int phv_or_off) {
      uint8_t val = io_index_ & 0x1;
      if (Phv::is_valid_taga_phv_p(phv_or_off))
        prs_reg_phv_owner_taga_set(&prs_main_phv_owner_, phv_or_off, val);
      else
        prs_reg_phv_owner_norm_set(&prs_main_phv_owner_, phv_or_off, val);
      // Also set replicated reg in prs_merge
      if (prs_merge_reg_ != NULL) prs_merge_reg_->phv_set_owner(phv_or_off, val);
    }
    inline void phv_set_clr_on_wr(int phv_or_off, bool tf) {
      if (prs_merge_reg_ != NULL) prs_merge_reg_->phv_set_clr_on_wr(phv_or_off, tf);
    }
    // provide stub methods for unit test compatibility - jbay parser overrides these
    virtual void phv_set_tm_status_phv(int pipe, uint8_t phv_or_off) { }
    virtual bool set_tm_status_input(uint32_t val_msb, uint32_t val_lsb) { return false; }

    inline void phv_set_multi_write(int phv_or_off, bool tf) {
      if (Phv::is_valid_taga_phv_p(phv_or_off))
        prs_reg_no_multi_wr_taga_set(&prs_main_no_multi_wr_, phv_or_off, tf ?0x0 :0x1);
      else
        prs_reg_no_multi_wr_norm_set(&prs_main_no_multi_wr_, phv_or_off, tf ?0x0 :0x1);
    }
    inline void phv_set_init_valid(int phv_or_off, bool tf) {
      if (prs_merge_reg_ != NULL) prs_merge_reg_->phv_set_init_valid(phv_or_off, tf);
    }
    inline bool phv_owner(int phv_or_off)       { return (phv_owner_val(phv_or_off) == (io_index_ & 1)); }
    inline bool phv_multi_write(int phv_or_off) { return (phv_multi_write_val(phv_or_off) == 0); }
    void log_phv_set_p(Phv *phv, int word, uint32_t val, ParseContext *context, bool clobber);
    void phv_set_p(Phv *phv, int word, uint32_t val, ParseContext *context, bool clobber);

    // PER-CHIP specialisation of write checks and write itself
    virtual bool phv_can_write(Phv *phv, int phv_or_off) = 0;
    virtual bool phv_can_write_16b(Phv *phv, int phv_or_off) = 0;
    virtual bool phv_owner_16b(Phv *phv, int phv_or_off) = 0;
    virtual bool phv_valid_16b(Phv *phv, int phv_or_off) = 0;
    virtual void phv_write_16b(Phv *phv, int phv_or_off, uint16_t val, ParseContext *context=nullptr);
    virtual uint32_t phv_check_write_16b(Phv *phv, int phv_or_off, uint16_t val, ParseContext *context=nullptr);


    // Update for new interrupt registers
    //inline uint32_t int_en()      { return prs_main_int_en(io_index_ & 0x1).int_en(); }
    //inline uint32_t int_inj()     { return prs_main_int_inj().int_inj(); }
    //inline uint32_t int_status()  { uint32_t data = 0u; prs_main_int_status().read(0,&data); return data; }

    inline uint8_t  pri_thresh(int ci)                { return prs_main_pri_thresh_.pri(ci); }
    inline void     set_pri_thresh(int ci, uint8_t v) { prs_main_pri_thresh_.pri(ci,v); }


    // chip_index() defined in RmtObject superclass
    // pipe_index() defined in PipeObject superclass
    inline int   parser_index() const { return parser_index_; }
    inline int   io_index()     const { return io_index_; }
    inline bool  ingress()      const { return (io_index_ == 0); }
    inline bool  egress()       const { return (io_index_ == 1); }
    inline const char *gress_str()    { return ingress() ? "Ingress" : "Egress"; }

    // Local version/timestamp
    inline void     version_set(uint32_t v)   { version_ = v; }
    inline void     timestamp_set(uint64_t t) { timestamp_ = t; }
    char           *errstr_get(uint32_t err_flags);
    uint32_t        version_get();
    uint64_t        timestamp_get();

    // Fetch counter_info and handle counters
    virtual void counter_info_get(int8_t *ctr, int8_t *ctr_next,
                                  bool *ctr_pending, bool *ctr_range_err_pending) {
      *ctr = counter_; *ctr_next = counter_next_;
      *ctr_pending = counter_pending_; *ctr_range_err_pending = counter_range_err_pending_;
    }
    virtual void counter_reset();
    virtual bool counter_handle(int index,
                                uint8_t f8_3, uint8_t f8_2, uint8_t f8_1, uint8_t f8_0,
                                int *shift);
    bool counter_handle(int index, int state_cnt,
                        uint8_t f8_3, uint8_t f8_2, uint8_t f8_1, uint8_t f8_0,
                        int *shift);

    // Call PipeObject to setup pipe
    virtual void set_pipe(Pipe *pipe) {
      PipeObject::set_pipe(pipe);
      prs_merge_reg_ = pipe->prs_merge_reg();
    }

    // PER-CHIP parser reset and not_implemented funcs
    // (these typically overriden per-chip but call superclass func)
    virtual void reset();
    virtual void not_implemented(const char *clazz);

    // PER-CHIP specialisation: fill other gress containers with junk
    virtual void phv_fill_other_gress(Phv* phv) = 0;

    // Called whenever a Parser Memory is written
    // JBay implementation notifies listening 'twinned' Parsers of memory change
    virtual void memory_change_callback(uint8_t mem_type, uint8_t mem_inst, uint32_t mem_index);
    // Called when PHV ownership changes
    virtual void phv_owner_change_callback();

    virtual int  ipb_index(int prsrChan) { return RmtDefs::get_parser_group(parser_index()); }
    virtual int  ipb_chan(int prsrChan)  { return prsrChan; } // Overridden on Jbay
    int epb_index(int prsrChan) { return ipb_index(prsrChan); }
    int epb_chan(int prsrChan) { return ipb_chan(prsrChan); }


    Phv *parse(Packet *packet, int chan, Phv *prev_phv=NULL);


    // Special funcs for DV to allow IPB processing of packet prior to Parse
    Phv *ipb_parse(Packet *packet, int chan, uint32_t flags,
                   uint8_t resubmit_flag, uint8_t version,
                   uint16_t logical_port, uint64_t timestamp,
                   uint64_t meta1[], Phv *prev_phv=NULL);
    Phv *ipb_parse(Packet *packet, int chan, Phv *prev_phv=NULL);

 protected:
    int8_t                                                       counter_ = 0;
    int8_t                                                       counter_next_ = 0;
    bool                                                         counter_pending_ = false;
    bool                                                         counter_range_err_pending_ = false;
 private:
    DISALLOW_COPY_AND_ASSIGN(ParserShared);  // XXX
    int                                                          offset_adjust_ = 0;
    uint32_t                                                     perr_phv_mask_ = PARSER_DFLT_ERR_PHV_MASK;
    uint32_t                                                     version_ = 0u;
    uint64_t                                                     timestamp_ = UINT64_C(0);
    std::array<bool, kChannels>                                  congested_;
    std::array<InputBuffer,kInputBuffers>                        inbufs_;
    bool                                                         reset_running_;
    bool                                                         hdr_len_inc_stopped_;

    //register_classes::PrsrRegCommonRspecMutable               *prs_common_rspec_;
    ParseMergeReg                                               *prs_merge_reg_;

    register_classes::PrsrRegMainRspecHdrLenAdjMutable           prs_main_hdr_len_adj_;
    register_classes::PrsrRegMainRspecMaxIterMutable             prs_main_max_iter_;
    register_classes::PrsrRegMainRspecNoMultiWrMutable           prs_main_no_multi_wr_;
    register_classes::PrsrRegMainRspecPhvOwnerMutable            prs_main_phv_owner_;
    register_classes::PrsrRegMainRspecPriStartMutable            prs_main_pri_start_;
    register_classes::PrsrRegMainRspecPriThreshMutable           prs_main_pri_thresh_;
    register_classes::PrsrRegMainRspecStartStateMutable          prs_main_start_state_;

    register_classes::PrsrRegMainRspecHdrByteCntArrayMutable     prs_main_hdr_byte_cnt_;
    register_classes::PrsrRegMainRspecIdleCntArrayMutable        prs_main_idle_cnt_;
    register_classes::PrsrRegMainRspecPktDropCntArrayMutable     prs_main_pkt_drop_cnt_;

    register_classes::PrsrRegMainRspecAramMbeCntMutable          prs_main_aram_mbe_cnt_;
    register_classes::PrsrRegMainRspecAramSbeCntMutable          prs_main_aram_sbe_cnt_;
    register_classes::PrsrRegMainRspecCsumErrCntMutable          prs_main_csum_err_cnt_;
    register_classes::PrsrRegMainRspecCtrRangeErrCntMutable      prs_main_ctr_range_err_cnt_;
    register_classes::PrsrRegMainRspecFcsErrCntMutable           prs_main_fcs_err_cnt_;
    register_classes::PrsrRegMainRspecMultiWrErrCntMutable       prs_main_multi_wr_err_cnt_;
    register_classes::PrsrRegMainRspecNoTcamMatchErrCntMutable   prs_main_no_tcam_match_err_cnt_;
    register_classes::PrsrRegMainRspecOpFifoFullCntMutable       prs_main_op_fifo_full_cnt_;
    register_classes::PrsrRegMainRspecOpFifoFullStallCntMutable  prs_main_op_fifo_full_stall_cnt_;
    register_classes::PrsrRegMainRspecPartialHdrErrCntMutable    prs_main_partial_hdr_err_cnt_;
    register_classes::PrsrRegMainRspecPhvOwnerErrCntMutable      prs_main_phv_owner_err_cnt_;
    register_classes::PrsrRegMainRspecSrcExtErrCntMutable        prs_main_src_ext_err_cnt_;
    register_classes::PrsrRegMainRspecTcamParErrCntMutable       prs_main_tcam_par_err_cnt_;
    register_classes::PrsrRegMainRspecTimeoutCycleErrCntMutable  prs_main_timeout_cycle_err_cnt_;
    register_classes::PrsrRegMainRspecTimeoutIterErrCntMutable   prs_main_timeout_iter_err_cnt_;

    // These registers are instantiated but are NOT used - however allows read/write
    register_classes::PrsrRegMainRspecDebugCtrlMutable           prs_main_debug_ctrl_;
    register_classes::PrsrRegMainRspecEccMutable                 prs_main_ecc_;
    register_classes::PrsrRegMainRspecMaxCycleMutable            prs_main_max_cycle_;

    memory_classes::PrsrPoActionRowArrayMutable                  prs_action_ram_;
    memory_classes::PrsrMlCtrInitRamMArrayMutable                prs_ctr_init_ram_;
    memory_classes::PrsrMlEaRowArrayMutable                      prs_ea_ram_;


 private:
    Tcam3<kStates,44>                                            tcam_match_;

    int                                                          pipe_index_;
    int                                                          parser_index_;
    int                                                          io_index_;
    uint32_t                                                     last_err_flags_;
    ExtractInfo                                                  extract_info_;
    model_core::LogBuffer                                        errstr_buf_;


    // PER-CHIP call on start of parsing
    virtual void parse_start(Packet *pkt, Phv *phv)  { }
    // PER-CHIP call on start/end of each parse state
    virtual void parse_start_state(Packet *pkt, Phv *phv, int state_cnt)            { }
    virtual void parse_end_state(Packet *pkt, Phv *phv, int state_cnt, bool final)  { }
    // PER-CHIP call to get ghost_phv (only JBay ever non-NULL)
    virtual Phv *get_ghost_phv()  { return NULL; }

    template <typename T> void check_extract_constraints(int index, int which_extract,
                                                         const int word_tab[],
                                                         int phv_word, Phv *phv, T val,
                                                         uint8_t extract_type,
                                                         int ck8, int ck16, int ck32,
                                                         bool partial_hdr_err);
    template <typename T> void check_extract_constraints_2(int index, int which_extract,
                                                           const int word_tab[],
                                                           int phv_word, Phv *phv, T val,
                                                           uint8_t extract_type,
                                                           int ck8, int ck16, int ck32,
                                                           bool partial_hdr_err);
    // PER-CHIP constraint checks
    virtual void check_further_extract_constraints(uint32_t check_flags,
                                                   int index, int which_extract,
                                                   int phv_word, Phv *phv, int extract_sz,
                                                   bool partial_hdr_err) = 0;
    // PER-CHIP extraction for fixed widths
    virtual uint32_t perform_extract_8b(int index, int which_extract, const int word_tab[],
                                        int phv_word, Phv *phv, uint8_t val,
                                        uint8_t extract_type,
                                        ParseContext *context) {
      RMT_ASSERT(0); // None by default
    }
    virtual uint32_t perform_extract_16b(int index, int which_extract, const int word_tab[],
                                         int phv_word, Phv *phv, uint16_t val,
                                         uint8_t extract_type,
                                         ParseContext *context) {
      RMT_ASSERT(0); // None by default
    }
    virtual uint32_t perform_extract_32b(int index, int which_extract, const int word_tab[],
                                         int phv_word, Phv *phv, uint32_t val,
                                         uint8_t extract_type,
                                         ParseContext *context) {
      RMT_ASSERT(0); // None by default
    }
    // Generic extraction
    template <typename T> uint32_t perform_extract(int index, int which_extract,
                                                   const int word_tab[],
                                                   int phv_word, Phv *phv, T val,
                                                   uint8_t extract_type,
                                                   ParseContext *context,
                                                   int ck8, int ck16, int ck32);

    // XXX: Tofino: Mimic worse case parser arbiter fifos occupancy in order to handle
    // n2w extraction constraints properly.
    void arbiter_fifo_occ_read(uint32_t occ_8b, uint32_t occ_16b, uint32_t occ_32b);
    template <typename T> void check_arbiter_fifo_occ(const int word_tab[], int n_xt, T val);

    template <typename T> bool imm_const_get(int index, int which, T *val, int shift);
    template <typename T> uint32_t inbuf_extract_to_phv(int index, Phv *phv, const uint64_t pkt_id, T val,
                                                        int n_xt, int ck8,
                                                        int ck16, int ck32,
                                                        ParseContext *context,
                                                        bool partial_hdr_err);

    uint32_t handle_extract(int index, Phv *phv, const uint64_t pkt_id, int ck8, int ck16, int ck32, ParseContext *context,
                            bool partial_hdr_err);
    void inbuf_extract_final_checks(int index, Phv *phv, int extract_cycle);
    void extract_counter_reset();
    void inbuf_setup(Packet *pkt);
    void phv_valid_force(Phv *phv);
    void offset_reset();
    void offset_fixup(int index);
    void checksum_reset();
    void checksum_start(Phv *phv);
    int checksum_stop(Packet *packet, Phv *phv, int pkt_furthest_pos);
    int checksum_handle(int match_index, int &wrote_8b, int &wrote_16b,
                        int &wrote_32b, int &err_cnt, int shift_from_pkt);
    uint8_t update_priority(int index, uint8_t curr_pri);
    void error_debug(uint32_t err_flags_orig, uint32_t err_flags_final);
    void maybe_stash_counters(Packet *p, int cycle, int8_t curr_counter);

  };
}
#endif // _SHARED_PARSER_SHARED_
