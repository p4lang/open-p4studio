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

// MirrorReg - common to JBay and later chips

#ifndef _JBAY_SHARED_MIRROR_REG_
#define _JBAY_SHARED_MIRROR_REG_

#include <register_includes/mirr_s2p_sess_cfg_word0_r_mutable.h>
#include <register_includes/mirr_s2p_sess_cfg_word1_r_mutable.h>
#include <register_includes/mirr_s2p_sess_cfg_word2_r_mutable.h>
#include <register_includes/mirr_s2p_sess_cfg_word3_r_mutable.h>
#include <register_includes/mirr_s2p_sess_cfg_word4_r_mutable.h>
#include <register_includes/mirr_s2p_sess_r_array_mutable.h>
#include <register_includes/mirror_sess_entry_r_array.h>
#include <register_includes/mirr_s2p_coal_to_interval_r.h>
#include <register_includes/s2p_coal_hdr_r_array_mutable.h>
#include <register_includes/mirror_coal_sess_entry_r_array.h>
#include <register_includes/mirror_coal_to_entry_r_array.h>
#include <register_includes/mirror_copy_to_c_p_u_t_v_r_mutable.h>
#include <register_includes/mirror_min_pkt_len_r_mutable.h>

#include <port.h>
#include <register_adapters.h>
#include <model_core/spinlock.h>
#include <mcn_test.h>


namespace MODEL_CHIP_NAMESPACE {

  class MirrorReg;

  // MirrorSessionReg is a genuine stash of config words on JBay
  // populated atomically by MirrorReg::dp_load
  //
  class MirrorSessionReg {
    // GREP: mirr_s2p_sess_cfg_word_
 public:
    static constexpr int kSessRegWords = 5;

    MirrorSessionReg(int chipIndex, int pipeIndex)
      : w0_(default_adapter(w0_, chipIndex, pipeIndex)),
        w1_(default_adapter(w1_, chipIndex, pipeIndex)),
        w2_(default_adapter(w2_, chipIndex, pipeIndex)),
        w3_(default_adapter(w3_, chipIndex, pipeIndex)),
        w4_(default_adapter(w4_, chipIndex, pipeIndex)),
        index_(-1)
    {
      static_assert( (kSessRegWords == 5),
                     "jbay::MirrorSessionReg assumes 5 SessCfgWords");
      w0_.reset(); w1_.reset(); w2_.reset(); w3_.reset(); w4_.reset();
    }
    MirrorSessionReg(MirrorReg *mirrorReg, int pipeIndex, int index)
      : MirrorSessionReg(-1, pipeIndex) {
      // Just ignore index here - will be set by sess_dp_load()
    }
    MirrorSessionReg(int pipeIndex)
      : MirrorSessionReg(nullptr, pipeIndex, -1) {
    }
    virtual ~MirrorSessionReg() { }

    int      index()      const { return index_; }
    void     set_index(int i)   { index_ = i; }

    uint8_t  hash_cfg()         { return w0_.hash_cfg_f(); }
    uint8_t  icos_cfg()         { return w0_.icos_cfg_f(); }
    uint8_t  dod_cfg()          { return w0_.dod_cfg_f(); }
    uint8_t  c2c_cfg()          { return w0_.c2c_cfg_f(); }
    uint8_t  mc_cfg()           { return w0_.mc_cfg_f(); }
    uint8_t  epipe_cfg()        { return w0_.epipe_cfg_f(); }
    uint16_t yid()              { return w0_.yid_f(); }
    uint16_t xid()              { return w0_.xid_f(); }

    uint16_t mcid1_id()         { return w1_.mcid1_id_f(); }
    uint16_t mcid2_id()         { return w1_.mcid2_id_f(); }

    uint16_t epipe_port()       { return w2_.epipe_port_f(); }
    uint8_t  epipe_port_vld()   { return w2_.epipe_port_vld_f(); }
    uint8_t  def_on_drop()      { return w2_.def_on_drop_f(); }
    uint16_t rid()              { return w2_.rid_f(); }

    uint16_t hash1()            { return w3_.hash1_f(); }
    uint16_t hash2()            { return w3_.hash2_f(); }
    uint8_t  pipe_vec()         { return w3_.pipe_vec_f(); }
    uint8_t  tm_vec()           { return 0; }  // N/A for jbay
    uint8_t  egress_bypass()    { return w3_.egress_bypass_f(); }

    uint8_t  icos()             { return w4_.icos_f(); }
    uint8_t  color()            { return w4_.color_f(); }
    uint8_t  mcid1_vld()        { return w4_.mcid1_vld_f(); }
    uint8_t  mcid2_vld()        { return w4_.mcid2_vld_f(); }
    uint8_t  c2c_cos()          { return w4_.c2c_cos_f(); }
    uint8_t  c2c_vld()          { return w4_.c2c_vld_f(); }
    uint8_t  yid_tbl_sel()      { return w4_.yid_tbl_sel_f(); }
    uint8_t  eport_qid()        { return w4_.eport_qid_f(); }

    bool uses_def_on_drop()     { return true; }
    bool uses_egress_bypass()   { return true; }
    bool uses_yid_tbl_sel()     { return true; }
    bool is_epipe_port_vld()    { return (epipe_port_vld() == 1); }
    bool is_mcid1_vld()         { return (mcid1_vld() == 1); }
    bool is_mcid2_vld()         { return (mcid2_vld() == 1); }
    bool is_c2c_vld()           { return (c2c_vld() == 1); }


    void read(uint32_t *data) {
      w0_.read(0, data + 0); w1_.read(0, data + 1);
      w2_.read(0, data + 2); w3_.read(0, data + 3);
      w4_.read(0, data + 4);
    }
    void write(uint32_t *data) {
      w0_.write(0, *(data + 0)); w1_.write(0, *(data + 1));
      w2_.write(0, *(data + 2)); w3_.write(0, *(data + 3));
      w4_.write(0, *(data + 4));
    }

 private:
    register_classes::MirrS2pSessCfgWord0RMutable  w0_;
    register_classes::MirrS2pSessCfgWord1RMutable  w1_;
    register_classes::MirrS2pSessCfgWord2RMutable  w2_;
    register_classes::MirrS2pSessCfgWord3RMutable  w3_;
    register_classes::MirrS2pSessCfgWord4RMutable  w4_;
    int                                            index_;
  };



  class MirrorReg {
    // GREP: mirr_s2p_sess_r_, mirror_sess_entry_r_,
    // GREP: mirror_coal_sess_entry_r_, mirror_coal_to_entry_r_, s2p_coal_hdr_r_

 public:
    // Define a static func to return a slice for a packet port - use get_deparser_slice
    static int get_slice(int pkt_port) { return RmtDefs::get_deparser_slice(pkt_port); }

    // Define a static func to get the packet port for a packet
    static int get_port(const Packet *pkt) {
      RMT_ASSERT(pkt);
      Port *port = pkt->port();
      RMT_ASSERT(port);
      return Port::get_pipe_local_port_index(port->port_index());
    }
    // Define a static func to return a slice for a packet
    static int get_slice(const Packet *pkt) {
      return get_slice( get_port(pkt) );
    }
    // And a static func to get a slice name
    static const char *slice_name(int slice) {
      const char *slice_names[4] = { "Slice0/", "Slice1/", "Slice2/", "Slice3/" };
      return slice_names[slice & 3];
    }

    // Define a static accessor to simplify calling register constructor in MirrorReg CTOR
    static enum register_classes::MirrS2pSessRArrayMutable::MirrorS2pSessionMapEnum
        get_tbl_01(int i) {
      switch (i) {
        case 0: return register_classes::MirrS2pSessRArrayMutable::MirrorS2pSessionMapEnum::kTbl0;
        case 1: return register_classes::MirrS2pSessRArrayMutable::MirrorS2pSessionMapEnum::kTbl1;
        default: RMT_ASSERT(0);
      }
    }

    static constexpr int      kSlices              = RmtDefs::kMirrorSlices;         // 4
    static constexpr int      kNormalSessions      = RmtDefs::kMirrorNormalSessions; // 256
    static constexpr int      kCoalSessions        = RmtDefs::kMirrorCoalSessions;   // 16
    static constexpr int      kFcsLen              = Packet::kPktFcsLen;             // 4
    static constexpr int      kCellSize            = 176;
    static constexpr int      kMaxTruncSize        = 16368;
    static constexpr int      kNormTruncAddSize    = RmtObject::is_jbayA0() ?kFcsLen :0;
    static constexpr int      kCoalTruncAddSize    = 0;
    static constexpr int      kSessTableEntries    = kNormalSessions;
    static constexpr int      kCoalTableEntries    = kCoalSessions;
    static constexpr int      kSessEntryWords      = MirrorSessionReg::kSessRegWords;
    static constexpr int      kCoalBaseTime        = 1000000; // PUT in rmt-defs.h??
    static constexpr int      kCoalHeaderPktCntPos =  1; // Tofino uses Byte0, JBay uses Byte1!
    static constexpr int      kCoalHeaderMinSize   =  8;
    static constexpr int      kCoalHeaderMaxSize   = 16;
    static constexpr int      kCoalMinSize         = 64;
    static constexpr int      kCoalMaxSize         = 1020;
    static constexpr int      kCoalSliceHdrSize    =  4;
    static constexpr int      kCoalSliceBodySize   = kCellSize;
    static constexpr int      kCoalSliceSize       = kCoalSliceHdrSize + kCoalSliceBodySize;
    static constexpr int      kSessBufSize         = kSessTableEntries * kSessEntryWords;
    static constexpr uint16_t kSessIdInvalid       = 0xFFFF;


    MirrorReg(int chipIndex, int pipeIndex)
      : ctor_running_(true),
        sess_tbl0_(default_adapter(sess_tbl0_, chipIndex, pipeIndex, get_tbl_01(0),
                                   [this](uint32_t i){this->sess_tbl_wr_cb(0,i);},
                                   [this](uint32_t i){this->sess_tbl_rd_cb(0,i);})),
        sess_tbl1_(default_adapter(sess_tbl1_, chipIndex, pipeIndex, get_tbl_01(1),
                                   [this](uint32_t i){this->sess_tbl_wr_cb(1,i);},
                                   [this](uint32_t i){this->sess_tbl_rd_cb(1,i);})),
        sess_ { {
          { default_adapter(sess_[0], chipIndex, pipeIndex, 0,
                            [this](uint32_t i){this->sess_entry_wr_cb(0,i);}) },
          { default_adapter(sess_[1], chipIndex, pipeIndex, 1,
                            [this](uint32_t i){this->sess_entry_wr_cb(1,i);}) },
          { default_adapter(sess_[2], chipIndex, pipeIndex, 2,
                            [this](uint32_t i){this->sess_entry_wr_cb(2,i);}) },
          { default_adapter(sess_[3], chipIndex, pipeIndex, 3,
                            [this](uint32_t i){this->sess_entry_wr_cb(3,i);}) } } },
        coal_ { {
          { default_adapter(coal_[0], chipIndex, pipeIndex, 0) },
          { default_adapter(coal_[1], chipIndex, pipeIndex, 1) },
          { default_adapter(coal_[2], chipIndex, pipeIndex, 2) },
          { default_adapter(coal_[3], chipIndex, pipeIndex, 3) } } },
        coal_timeout_ { {
          { default_adapter(coal_timeout_[0], chipIndex, pipeIndex, 0) },
          { default_adapter(coal_timeout_[1], chipIndex, pipeIndex, 1) },
          { default_adapter(coal_timeout_[2], chipIndex, pipeIndex, 2) },
          { default_adapter(coal_timeout_[3], chipIndex, pipeIndex, 3) } } },
        coal_interval_(default_adapter(coal_interval_, chipIndex, pipeIndex)),
        copy_to_cpu_tv_(default_adapter(copy_to_cpu_tv_, chipIndex, pipeIndex)),
        min_pkt_len_(default_adapter(min_pkt_len_, chipIndex, pipeIndex)),
        coal_hdr_(default_adapter(coal_hdr_, chipIndex, pipeIndex)),
        spinlock_(), sess_buf_(),
        coal_to_sess_map_(), recalc_map_(),
        ctl_config_(chipIndex, pipeIndex),
        dp_(pipeIndex), // Just for accessors, NOT associated with any chip
        dp_index_valid_(false)
    {
      static_assert( (kSessTableEntries < kSessIdInvalid),
                     "jbay::MirrorReg coal_to_sess_map assumes SessID fits in uint16_t" );
      static_assert( (kSessEntryWords == MirrorSessionReg::kSessRegWords),
                     "jbay::MirrorReg logic requires identical #words as MirrorSessionReg" );
      static_assert( (kSlices == 4), "jbay::MirrorReg CTOR assumes 4 slices" );
      sess_tbl0_.reset();
      sess_tbl1_.reset();
      for (int S = 0; S < kSlices; S++) sess_[S].reset();
      for (int S = 0; S < kSlices; S++) coal_[S].reset();
      for (int S = 0; S < kSlices; S++) coal_timeout_[S].reset();
      coal_interval_.reset();
      copy_to_cpu_tv_.reset();
      min_pkt_len_.reset();
      coal_hdr_.reset();
      for (int i = 0; i < kSessBufSize; i++) sess_buf_[i] = 0u;
      for (int S = 0; S < kSlices; S++) reset_coal_to_sess_map(S);
      for (int S = 0; S < kSlices; S++) recalc_map_[S] = true;
      ctor_running_ = false;
    }
    virtual ~MirrorReg() { }

 private:
    int sess_buf_off_get(uint32_t i) {
      int off = i * kSessEntryWords;
      RMT_ASSERT(off + kSessEntryWords <= kSessBufSize);
      return off;
    }
    void sess_buf_off_rd(int off, uint32_t *data) {
      for (int j = 0; j < kSessEntryWords; j++) *(data + j) = sess_buf_[off + j];
    }
    void sess_buf_off_wr(int off, uint32_t *data) {
      for (int j = 0; j < kSessEntryWords; j++) sess_buf_[off + j] = *(data + j);
    }


    void sess_tbl_wr_cb(int tbl01, uint32_t i) {
      if (ctor_running_) return;
      // Reads ctl_config_ and writes to sess_buf
      int off = sess_buf_off_get(i);
      uint32_t data[kSessEntryWords];
      spinlock_.lock();
      ctl_config_.read(data);
      sess_buf_off_wr(off, data);
      // If datapath accessing this index invalidate to force reload
      if (dp_.index() == static_cast<int>(i)) dp_index_valid_ = false;
      spinlock_.unlock();
    }
    void sess_tbl_rd_cb(int tbl01, uint32_t i) {
      if (ctor_running_) return;
      // Reads sess_buf and writes to ctl_config
      int off = sess_buf_off_get(i);
      uint32_t data[kSessEntryWords];
      spinlock_.lock();
      sess_buf_off_rd(off, data);
      ctl_config_.write(data);
      spinlock_.unlock();
      // Data word0 becomes available to CSR read so fill in tbl0/1
      if      (tbl01 == 0) sess_tbl0_.data(i, data[0]);
      else if (tbl01 == 1) sess_tbl1_.data(i, data[0]);
    }

    void dp_load(MirrorSessionReg *entry, int i) {
      // Reads sess_buf and writes to entry if not already done so
      RMT_ASSERT(entry != NULL);
      spinlock_.lock();
      bool already_loaded = (entry->index() == i);
      spinlock_.unlock();
      if (already_loaded) return;
      RMT_ASSERT((i >= 0) && (i < kSessTableEntries));
      int off = sess_buf_off_get(i);
      uint32_t data[kSessEntryWords];
      spinlock_.lock();
      entry->set_index(-1);
      sess_buf_off_rd(off, data);
      entry->write(data);
      entry->set_index(i);
      spinlock_.unlock();
    }
    void dp_load(int i) {
      // Load singleton datapath entry dp_
      if (!dp_index_valid_) dp_.set_index(-1);
      dp_load(&dp_, i);
      dp_index_valid_ = true;
    }
    void dp_check(int i) {
      // Sanity check that singleton datapath entry dp_ has correct index loaded
      RMT_ASSERT((i >= 0) && (i < kSessTableEntries));
      RMT_ASSERT(dp_.index() == i);
    }

    void sess_entry_wr_cb(int slice, uint32_t i) {
      if (ctor_running_) return;
      RMT_ASSERT((slice >= 0) && (slice < kSlices));
      recalc_map_[slice] = true;
    }
    void reset_coal_to_sess_map(int slice) {
      RMT_ASSERT((slice >= 0) && (slice < kSlices));
      for (int c = 0; c < kCoalTableEntries; c++)
        coal_to_sess_map_[slice][c] = kSessIdInvalid;
    }
    void recalculate_coal_to_sess_map(int slice) {
      RMT_ASSERT((slice >= 0) && (slice < kSlices));
      if (!recalc_map_[slice]) return;
      recalc_map_[slice] = false;

      reset_coal_to_sess_map(slice);
      for (int i = 0; i < kSessTableEntries; i++) {
        if (sess_coal_en(slice,i)) {
          uint8_t coal_num = sess_coal_num(slice, i);
          RMT_ASSERT(coal_num < kCoalTableEntries);
          coal_to_sess_map_[slice][coal_num] = i;
        }
      }
    }


 public:
    void     sess_dp_load(MirrorSessionReg *entry, int i)  { dp_load(entry, i); }
    void     sess_dp_load(int i)                           { dp_load(i); }

    // NORMAL session CSRs that read singleton datapath entry dp_
    // Typically caller uses their own MirrorSessionReg object,
    // populating it via a call to sess_dp_load above
    //
    bool     hash_cfg(int i)           { dp_check(i); return (dp_.hash_cfg() == 1); }
    bool     icos_cfg(int i)           { dp_check(i); return (dp_.icos_cfg() == 1); }
    bool     dod_cfg(int i)            { dp_check(i); return (dp_.dod_cfg() == 1); }
    bool     c2c_cfg(int i)            { dp_check(i); return (dp_.c2c_cfg() == 1); }
    bool     mc_cfg(int i)             { dp_check(i); return (dp_.mc_cfg() == 1); }
    bool     epipe_cfg(int i)          { dp_check(i); return (dp_.epipe_cfg() == 1); }

    uint16_t yid(int i)                { dp_check(i); return dp_.yid(); }
    uint16_t xid(int i)                { dp_check(i); return dp_.xid(); }
    uint16_t mcid1_id(int i)           { dp_check(i); return dp_.mcid1_id(); }
    uint16_t mcid2_id(int i)           { dp_check(i); return dp_.mcid2_id(); }
    uint16_t epipe_port(int i)         { dp_check(i); return dp_.epipe_port(); }
    uint8_t  epipe_port_vld(int i)     { dp_check(i); return dp_.epipe_port_vld(); }
    uint8_t  def_on_drop(int i)        { dp_check(i); return dp_.def_on_drop(); }
    uint16_t rid(int i)                { dp_check(i); return dp_.rid(); }
    uint16_t hash1(int i)              { dp_check(i); return dp_.hash1(); }
    uint16_t hash2(int i)              { dp_check(i); return dp_.hash2(); }
    uint8_t  pipe_vec(int i)           { dp_check(i); return dp_.pipe_vec(); }
    uint8_t  egress_bypass(int i)      { dp_check(i); return dp_.egress_bypass(); }
    uint8_t  icos(int i)               { dp_check(i); return dp_.icos(); }
    uint8_t  color(int i)              { dp_check(i); return dp_.color(); }
    uint8_t  mcid1_vld(int i)          { dp_check(i); return dp_.mcid1_vld(); }
    uint8_t  mcid2_vld(int i)          { dp_check(i); return dp_.mcid2_vld(); }
    uint8_t  c2c_cos(int i)            { dp_check(i); return dp_.c2c_cos(); }
    uint8_t  c2c_vld(int i)            { dp_check(i); return dp_.c2c_vld(); }
    uint8_t  yid_tbl_sel(int i)        { dp_check(i); return dp_.yid_tbl_sel(); }
    uint8_t  eport_qid(int i)          { dp_check(i); return dp_.eport_qid(); }

    bool     uses_def_on_drop(int i)   { dp_check(i); return dp_.uses_def_on_drop(); }
    bool     uses_egress_bypass(int i) { dp_check(i); return dp_.uses_egress_bypass(); }
    bool     uses_yid_tbl_sel(int i)   { dp_check(i); return dp_.uses_yid_tbl_sel(); }
    bool     is_epipe_port_vld(int i)  { dp_check(i); return dp_.is_epipe_port_vld(); }
    bool     is_mcid1_vld(int i)       { dp_check(i); return dp_.is_mcid1_vld(); }
    bool     is_mcid2_vld(int i)       { dp_check(i); return dp_.is_mcid2_vld(); }
    bool     is_c2c_vld(int i)         { dp_check(i); return dp_.is_c2c_vld(); }


    // S is sliceID
    bool     sess_ok(int S, int i)            { return ((i >= 0) && (i < kSessTableEntries)); }
    bool     sess_en(int S, int i)            { return (sess_[S].sess_en(i) == 1); }
    bool     sess_coal_fld_en(int S, int i)   { return (sess_[S].coal_en(i) == 1); }
    bool     sess_coal_en(int S, int i)       { return sess_en(S,i) && sess_coal_fld_en(S,i); }
    int      sess_coal_num(int S, int i)      { return sess_[S].coal_num(i); }
    uint8_t  sess_pri(int S, int i)           { return sess_coal_num(S, i) & 1; }
    bool     ing_en(int S, int i)             { return (sess_[S].ingr_en(i) == 1); }
    bool     egr_en(int S, int i)             { return (sess_[S].egr_en(i) == 1); }
    bool     gress_en(int S, int i, bool ing) { return (ing) ?ing_en(S,i) :egr_en(S,i); }
    bool     sess_en(int S, int i, bool ing)  { return sess_en(S,i) && gress_en(S,i,ing); }
    bool     sess_coal_en(int S,int i,bool B) { return sess_coal_en(S,i) && gress_en(S,i,B); }
    bool     is_sess_id_coal(int S, int i)    { return sess_coal_en(S, i); }
    int      sess_id_to_coal_id(int S, int i) { return sess_coal_num(S, i); }
    int      coal_id_to_sess_id(int S, int k) {
      if (recalc_map_[S]) recalculate_coal_to_sess_map(S);
      return static_cast<int>(coal_to_sess_map_[S][k]);
    }
    // XXX: On JBay trunc_len rounded down to 4B boundary and does NOT include FCS
    // XXX: On JBayB0/WIP trunc_len rounded down to 4B boundary and DOES include FCS
    uint16_t trunc_size_csr(int S, int i)     { return sess_[S].pkt_len(i); }
    uint16_t trunc_size_rnd(int S, int i)     { return trunc_size_csr(S,i) & ~3; /* 4B aligned */}
    uint16_t trunc_size_coal(int S, int i)    { return trunc_size_rnd(S,i) + kCoalTruncAddSize; /* +0 */}
    uint16_t trunc_size_norm(int S, int i)    { return trunc_size_rnd(S,i) + kNormTruncAddSize; /* +4 (A0) else +0 */}
    uint16_t trunc_size(int S, int i)         { return trunc_size_norm(S,i); }
    uint8_t  max_entries(int S, int i)        { return 64; }
    // XXX: On WIP support min_pkt_len CSR (JBay uses dummy register)
    uint8_t  min_pkt_len()                    { return (RmtObject::is_chip1()) ?min_pkt_len_.cfg() : GLOBAL_ZERO; }
    void     set_min_pkt_len(int m)           { if (RmtObject::is_chip1()) min_pkt_len_.cfg(m & 0x3f); }




    // COALESCING session CSRs - S is sliceID
    bool     coal_ok(int S, int k)            { return ((k >= 0) && (k < kCoalTableEntries)); }
    bool     coal_en(int S, int k)            {
      int i = coal_id_to_sess_id(S, k); return (sess_ok(S, i)) ?sess_coal_en(S,i) :false; }
    bool     coal_en(int S, int k, bool ing)  {
      int i = coal_id_to_sess_id(S, k); return (sess_ok(S, i)) ?sess_coal_en(S,i,ing) :false; }
    bool     coal_has_version(int S, int k)   { return GLOBAL_FALSE; }
    uint8_t  coal_ver(int S, int k)           { return 0; /* NO version on JBay */}
      // WAS return coal_.coal_desc_grp_coal_ctrl0(k).coal_vid(); }
    bool     coal_has_priority(int S, int k)  { return true; }
    uint8_t  coal_pri(int S, int k)           { return coal_[S].pri(k); }
    uint16_t coal_trunc_size(int S, int k)    {
      int i = coal_id_to_sess_id(S, k); return (sess_ok(S,i)) ?trunc_size_coal(S,i) :0; }
    uint8_t  coal_pkthdr_len(int S, int k)    {
      return (coal_[S].coal_hdr(k) < 3) ?((coal_[S].coal_hdr(k) + 2) * 4) :0; }
    uint16_t coal_pkt_min_size(int S, int k)  { return coal_trunc_size(S,k); }
    uint16_t coal_abs_min_size(int S, int k)  { return kCoalMinSize; /* min_bcnt */ }
    uint32_t coal_timeout(int S, int k)       { return coal_timeout_[S].coal_timeout(k); }
    bool     coal_tofino_mode(int S, int k)   { return (coal_[S].coal_mode(k) == 1); }
    bool     coal_len_from_input(int S, int k){ return (coal_[S].len_cfg(k) == 1); }
      // WAS return (coal_.coal_desc_grp_coal_ctrl1(k).coal_sflow_type() == 1); }
    uint16_t coal_extract_len(int S, int k)   { return coal_[S].sample_pkt_len(k) * 4; }
    uint32_t coal_compiler_flag(int S, int k) { return coal_hdr_.compiler_flag(k); }
    uint32_t coal_seqnum(int S, int k)        { /* NB. INCREMENTS seq_num */
      uint32_t v = coal_hdr_.seq_num(k); coal_hdr_.seq_num(k, v+1); return v; }

    uint32_t coal_pkthdr0_HI(int S, int k)    {
      return static_cast<uint32_t>(coal_compiler_flag(S,k) & 0x00FF) << 24; }
    uint32_t coal_pkthdr0_LO(int S, int k)    {
      return static_cast<uint32_t>(coal_id_to_sess_id(S,k) & 0xFFFF) <<  0; }
    uint32_t coal_pkthdr0(int S, int k)       {
      return coal_pkthdr0_HI(S,k) | coal_pkthdr0_LO(S,k); }
    uint32_t coal_pkthdr1(int S, int k)       { return coal_seqnum(S, k); }
    uint32_t coal_pkthdr2(int S, int k)       { return coal_hdr_.user_0_3(k); }
    uint32_t coal_pkthdr3(int S, int k )      { return coal_hdr_.user_4_7(k); }
    bool     coal_timer_enabled()             { return (coal_interval_.val() > 0); }
    uint32_t coal_basetime()                  { return 1+coal_interval_.val(); }
    uint16_t coal_baseid()                    { return 0; }

    bool     coal_whole_pkt(uint16_t ext_len) { return (ext_len == 0); }
    bool     coal_trunc_pkt(uint16_t ext_len) { return true; }
    uint32_t coal_slicehdr(int port, int len, bool start, bool end) {
      return ((port & 0xFFFF) << 16) | ((len & 0xFF) << 8) | (start?0x80:0) | (end?0x40:0);
    }


    // GLOBAL CSRs
    bool     glob_ing_en()             { return GLOBAL_TRUE; }
    bool     glob_egr_en()             { return GLOBAL_TRUE; }
    bool     glob_en(bool ing)         { return ing ?glob_ing_en() :glob_egr_en(); }
    bool     glob_coal_en()            { return GLOBAL_TRUE; }
    bool     glob_coal_ing_en()        { return GLOBAL_TRUE; }
    bool     glob_coal_egr_en()        { return GLOBAL_TRUE; }
    bool     glob_coal_gress(bool ing) { return ing ?glob_coal_ing_en() :glob_coal_egr_en(); }
    bool     glob_coal_en(bool ing)    { return glob_coal_en() && glob_coal_gress(ing); }
    bool     glob_mirr_if_not_coal(bool ing) { return GLOBAL_FALSE; }

    uint8_t copy_to_cpu_die_vec() {
      // WIP only - jbay uses dummy register
      uint8_t die_vec = RmtObject::is_chip1() ? copy_to_cpu_tv_.die_vec() : 0u;
      return die_vec & 0xfu;
    }
    void set_copy_to_cpu_die_vec(uint8_t die_vec) {
      // for unit tests: WIP only - jbay uses dummy register
      if (!RmtObject::is_chip1()) return;
      copy_to_cpu_tv_.die_vec(die_vec & 0xfu);
    }


   private:
    bool                                                                 ctor_running_;
    register_classes::MirrS2pSessRArrayMutable                           sess_tbl0_;
    register_classes::MirrS2pSessRArrayMutable                           sess_tbl1_;
    std::array< register_classes::MirrorSessEntryRArray, kSlices >       sess_;
    std::array< register_classes::MirrorCoalSessEntryRArray, kSlices >   coal_;
    std::array< register_classes::MirrorCoalToEntryRArray, kSlices >     coal_timeout_;
    register_classes::MirrS2pCoalToIntervalR                             coal_interval_;
    register_classes::MirrorCopyToCPUTVRMutable                          copy_to_cpu_tv_;
    register_classes::MirrorMinPktLenRMutable                            min_pkt_len_;
    register_classes::S2pCoalHdrRArrayMutable                            coal_hdr_;
    model_core::Spinlock                                                 spinlock_;
    std::array< uint32_t, kSessBufSize >                                 sess_buf_;
    std::array< std::array< uint16_t,kCoalTableEntries >, kSlices >      coal_to_sess_map_;
    std::array< bool, kSlices >                                          recalc_map_;
    MirrorSessionReg                                                     ctl_config_;
    MirrorSessionReg                                                     dp_;
    bool                                                                 dp_index_valid_;
  };

}
#endif // _JBAY_SHARED_MIRROR_REG_
