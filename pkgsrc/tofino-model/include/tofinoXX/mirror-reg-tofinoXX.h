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

// MirrorRegTofinoXX - common to Tofino chips

#ifndef _TOFINOXX_MIRROR_REG_TOFINOXX_
#define _TOFINOXX_MIRROR_REG_TOFINOXX_

#include <register_includes/mir_buf_regs_coal_ctrl0.h>
#include <register_includes/mir_buf_regs_coal_ctrl1.h>
#include <register_includes/mir_buf_regs_coal_ctrl2.h>
#include <register_includes/mir_buf_regs_coal_pkt_header0.h>
#include <register_includes/mir_buf_regs_coal_pkt_header1.h>
#include <register_includes/mir_buf_regs_coal_pkt_header2.h>
#include <register_includes/mir_buf_regs_coal_pkt_header3.h>
#include <register_includes/mir_buf_desc_norm_desc_grp_array.h>
#include <register_includes/mir_buf_regs_glb_ctrl.h>
#include <register_includes/mir_buf_regs_neg_mirr_ctrl.h>
#include <register_includes/mir_buf_regs_coalescing_baseid.h>
#include <register_includes/mir_buf_regs_coalescing_basetime.h>

#include <port.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

  class MirrorSessionReg;

  class MirrorCoalDescReg {
    // HAVE to instantiate an array of these manually.
    // Auto-gen CSRs can't handle Group objects in middle of an address-map
    //
    // GREP: mir_buf_regs_coal_ctrl0, mir_buf_regs_coal_ctrl1
    // GREP: mir_buf_regs_pkt_header0, mir_buf_regs_pkt_header1
 public:
    MirrorCoalDescReg(int chipIndex, int pipeIndex, int coalIndex)
      : ctrl0_(deparser_out_adapter(ctrl0_,chipIndex,pipeIndex,coalIndex)),
        ctrl1_(deparser_out_adapter(ctrl1_,chipIndex,pipeIndex,coalIndex)),
        ctrl2_(deparser_out_adapter(ctrl2_,chipIndex,pipeIndex,coalIndex)),
        pkt_hdr0_(deparser_out_adapter(pkt_hdr0_,chipIndex,pipeIndex,coalIndex)),
        pkt_hdr1_(deparser_out_adapter(pkt_hdr1_,chipIndex,pipeIndex,coalIndex)),
        pkt_hdr2_(deparser_out_adapter(pkt_hdr2_,chipIndex,pipeIndex,coalIndex)),
        pkt_hdr3_(deparser_out_adapter(pkt_hdr3_,chipIndex,pipeIndex,coalIndex))
    {
      ctrl0_.reset(); ctrl1_.reset(); ctrl2_.reset();
      pkt_hdr0_.reset(); pkt_hdr1_.reset(); pkt_hdr2_.reset(); pkt_hdr3_.reset();
    }
    virtual ~MirrorCoalDescReg() { }

    register_classes::MirBufRegsCoalCtrl0       ctrl0_;
    register_classes::MirBufRegsCoalCtrl1       ctrl1_;
    register_classes::MirBufRegsCoalCtrl2       ctrl2_;
    register_classes::MirBufRegsCoalPktHeader0  pkt_hdr0_;
    register_classes::MirBufRegsCoalPktHeader1  pkt_hdr1_;
    register_classes::MirBufRegsCoalPktHeader2  pkt_hdr2_;
    register_classes::MirBufRegsCoalPktHeader3  pkt_hdr3_;
  };



  class MirrorRegTofinoXX {
    // GREP: mir_buf_desc_norm_desc_grp, mir_buf_regs_coal_desc_grp

 public:
    // Define a static func to get the packet port for a packet
    static int get_port(const Packet *pkt) {
      RMT_ASSERT(pkt);
      Port *port = pkt->port();
      if (port == NULL) return -1;
      return Port::get_pipe_local_port_index(port->port_index());
    }
    // Static funcs to return a slice number - always 0 on Tofino
    static int get_slice(int pkt_port)       { return 0;  }
    static int get_slice(const Packet *pkt)  { return 0;  }
    static const char *slice_name(int slice) { return ""; }


    static constexpr int  kSlices               = RmtDefs::kMirrorSlices;         // 1
    static constexpr int  kNormalSessions       = RmtDefs::kMirrorNormalSessions; // 1024
    static constexpr int  kCoalSessions         = RmtDefs::kMirrorCoalSessions;   // 8
    static constexpr int  kFcsLen               = Packet::kPktFcsLen;             // 4
    static constexpr int  kCoalTruncAddSize     = 0;
    static constexpr int  kNormTruncAddSize     = kFcsLen;
    static constexpr int  kCoalSessionsBaseMask = ~(kCoalSessions-1);
    static constexpr int  kCoalHeaderPktCntPos  =  0;
    static constexpr int  kCoalHeaderMinSize    =  4;
    static constexpr int  kCoalHeaderMaxSize    = 16;
    static constexpr int  kCoalMinSize          = 32;
    static constexpr int  kCoalMaxSize          = 1020;
    static constexpr int  kCoalSliceHdrSize     =  0;
    static constexpr int  kCoalSliceBodySize    = 256; // No slices on Tofino really
    static constexpr int  kCoalSliceSize        = kCoalSliceHdrSize + kCoalSliceBodySize;
    static_assert( (__builtin_popcount((unsigned)kCoalSessions) == 1),
                   "Expected kCoalSessions to be a power of 2");


    MirrorRegTofinoXX(int chipIndex, int pipeIndex)
      : norm_(deparser_out_adapter(norm_,chipIndex,pipeIndex)),
        coal_ { { { chipIndex,pipeIndex,0 }, { chipIndex,pipeIndex,1 },
                  { chipIndex,pipeIndex,2 }, { chipIndex,pipeIndex,3 },
                  { chipIndex,pipeIndex,4 }, { chipIndex,pipeIndex,5 },
                  { chipIndex,pipeIndex,6 }, { chipIndex,pipeIndex,7 } } },
        glb_ctrl_(deparser_out_adapter(glb_ctrl_,chipIndex,pipeIndex)),
        neg_mir_ctrl_(deparser_out_adapter(neg_mir_ctrl_,chipIndex,pipeIndex)),
        coal_baseid_(deparser_out_adapter(coal_baseid_,chipIndex,pipeIndex)),
        coal_basetime_(deparser_out_adapter(coal_basetime_,chipIndex,pipeIndex))
    {
      static_assert( (kCoalSessions == 8), "MirrorRegTofinoXX CTOR assumes 8x CoalSessions" );
      norm_.reset();
      glb_ctrl_.reset();
      neg_mir_ctrl_.reset();
      coal_baseid_.reset();
      coal_basetime_.reset();
    }
    virtual ~MirrorRegTofinoXX() { }

 private:
    uint32_t meta0(int i)     { return norm_.norm_desc_grp_session_meta0(i).outp_meta0(); }
    uint32_t meta1(int i)     { return norm_.norm_desc_grp_session_meta1(i).outp_meta1(); }
    uint32_t meta2(int i)     { return norm_.norm_desc_grp_session_meta2(i).outp_meta2(); }
    uint32_t meta3(int i)     { return norm_.norm_desc_grp_session_meta3(i).outp_meta3(); }
    uint32_t meta4(int i)     { return norm_.norm_desc_grp_session_meta4(i).outp_meta4(); }

 public:
    // Does nothing on Tofino - no atomic cfg change
    void     sess_dp_load(MirrorSessionReg *entry, int i)  { }
    void     sess_dp_load(int i)                           { }

    uint8_t  hash_cfg(int i)  { return GLOBAL_ZERO; }
    uint8_t  icos_cfg(int i)  { return 0; }
    uint8_t  dod_cfg(int i)   { return 0; }
    uint8_t  c2c_cfg(int i)   { return GLOBAL_ZERO; }
    uint8_t  mc_cfg(int i)    { return GLOBAL_ZERO; }
    uint8_t  epipe_cfg(int i) { return GLOBAL_ZERO; }
    uint8_t  copy_to_cpu_die_vec()  { return 0; }            // N/A on tofinoXX
    void     set_copy_to_cpu_die_vec(uint8_t die_vec) { }    // N/A on tofinoXX
    uint8_t  min_pkt_len()          { return GLOBAL_ZERO; } // XXX: N/A on tofinoXX
    void     set_min_pkt_len(int m) { }           // XXX: N/A on tofinoXX

    // NORMAL session CSRs
    // (Comment indicates name by which OLD S/W in mirror-buf-reg.cpp referred to CSR)
    // (and *DIFF* in comment indicates a different 'harmonised' name being used here)
    uint16_t yid(int i)                { // yid
      return static_cast<uint16_t>(  (meta3(i) >>  4) & 0x01FF  );  }
    uint16_t xid(int i)                { // xid
      return static_cast<uint16_t>( ((meta2(i) >> 20) & 0x0FFF) | ((meta3(i) & 0xF) << 12) ); }
    uint16_t mcid1_id(int i)           { // mgid1 *DIFF*
      return static_cast<uint16_t>( ((meta1(i) >> 18) & 0x3FFF) | ((meta2(i) & 0x3) << 14) ); }
    uint16_t mcid2_id(int i)           { // mgid2 *DIFF*
      return static_cast<uint16_t>(  (meta2(i) >>  3) & 0xFFFF  );  }
    uint16_t epipe_port(int i)         { // egress_unicast_port *DIFF*
      return static_cast<uint16_t>(  (meta0(i) >>  4) & 0x01FF  );  }
    uint8_t  epipe_port_vld(int i)     { // egress_unicast_port_valid *DIFF*
      return static_cast<uint8_t>(   (meta0(i) >>  3) & 0x0001  );  }
    uint8_t  def_on_drop(int i)        { // NOT used
      return static_cast<uint8_t>(   (meta4(i) >>  2) & 0x0001  );  }
    uint16_t rid(int i)                { // rid
      return static_cast<uint16_t>(  (meta3(i) >> 13) & 0xFFFF  );  }
    uint16_t hash1(int i)              { // hash1
      return static_cast<uint16_t>( ((meta0(i) >> 24) & 0x00FF) | ((meta1(i) & 0x1F) << 8) ); }
    uint16_t hash2(int i)              { // hash2
      return static_cast<uint16_t>(  (meta1(i) >>  5) & 0x1FFF  );  }
    uint8_t  pipe_vec(int i)           { // pipe_mask *DIFF*
      return static_cast<uint8_t>(   (meta0(i) >> 20) & 0x000F  );  }
    uint8_t  egress_bypass(int i)      { // NOT used
      return static_cast<uint8_t>(   (meta3(i) >> 29) & 0x0001  );  }
    uint8_t  icos(int i)               { // icos
      return static_cast<uint8_t>(   (meta0(i) >>  0) & 0x0007  );  }
    uint8_t  color(int i)              { // meter_color *DIFF*
      return static_cast<uint8_t>(   (meta0(i) >> 18) & 0x0003  );  }
    uint8_t  mcid1_vld(int i)          { // mgid1_valid *DIFF*
      return static_cast<uint8_t>(   (meta2(i) >>  2) & 0x0001  );  }
    uint8_t  mcid2_vld(int i)          { // mgid2_valid *DIFF*
      return static_cast<uint8_t>(   (meta2(i) >> 19) & 0x0001  );  }
    uint8_t  c2c_cos(int i)            { // copy_to_cpu_cos *DIFF*
      return static_cast<uint8_t>(  ((meta3(i) >> 30) & 0x0003) | ((meta4(i) & 0x1) <<  2) ); }
    uint8_t  c2c_vld(int i)            { // copy_to_cpu *DIFF*
      return static_cast<uint8_t>(   (meta4(i) >>  1) & 0x0001  );  }
    uint8_t  yid_tbl_sel(int i)        { // NOT used
      return static_cast<uint8_t>(   (meta4(i) >>  3) & 0x0001  );  }
    uint8_t  eport_qid(int i)          { // qid *DIFF*
      return static_cast<uint8_t>(   (meta0(i) >> 13) & 0x001F  );  }

    bool     uses_def_on_drop(int i)   { return false; }
    bool     uses_egress_bypass(int i) { return false; }
    bool     uses_yid_tbl_sel(int i)   { return false; }
    bool     is_epipe_port_vld(int i)  { return (epipe_port_vld(i) == 1); }
    bool     is_mcid1_vld(int i)       { return (mcid1_vld(i) == 1); }
    bool     is_mcid2_vld(int i)       { return (mcid2_vld(i) == 1); }
    bool     is_c2c_vld(int i)         { return (c2c_vld(i) == 1); }


    // S is sliceID which is unused on Tofino
    bool     sess_ok(int S, int i)            { return ((i >= 0) && (i < kNormalSessions)); }
    bool     sess_en(int S, int i)            { return true; }
    bool     sess_coal_fld_en(int S, int i)   { return true; }
    bool     sess_coal_en(int S, int i)       {
      int k = sess_id_to_coal_id(S,i); return (k >= 0) ?coal_en(S,k) :false; }
    int      sess_coal_num(int S, int i)      { return sess_id_to_coal_id(S, i); }
    uint8_t  sess_pri(int S, int i)           { return 0; /* NO pri on Tofino */ }
    bool     ing_en(int S, int i)             {
      return (norm_.norm_desc_grp_session_ctrl(i).norm_ingr_ena() == 1); }
    bool     egr_en(int S, int i)             {
      return (norm_.norm_desc_grp_session_ctrl(i).norm_egr_ena() == 1); }
    bool     gress_en(int S, int i, bool ing) { return (ing) ?ing_en(S,i) :egr_en(S,i); }
    bool     sess_en(int S, int i, bool ing)  { return sess_en(S,i) && gress_en(S,i,ing); }
    bool     sess_coal_en(int S,int i,bool B) { return sess_coal_en(S,i) && gress_en(S,i,B); }
    bool     is_sess_id_coal(int S, int i)    {
      return (glob_coal_en() && (i >= 0) && ((i & kCoalSessionsBaseMask) == coal_baseid())); }
    int      sess_id_to_coal_id(int S, int i) {
      return (is_sess_id_coal(S,i)) ?i - coal_baseid() :-1; }
    int      coal_id_to_sess_id(int S, int k) {
      return ((k >= 0) && (k < kCoalSessions)) ?k + coal_baseid() :-1; }
    uint8_t  max_entries(int S, int i)        { // NOT used
      return norm_.norm_desc_grp_session_ctrl(i).norm_max_entries();}

    // XXX: On Tofino norm_trunc_size does NOT include FCS so add here
    // (FCS actually added by Deparser but do it here for consistency with JBay)
    uint16_t trunc_size_csr(int S, int i)     { // pkt_size *DIFF*
      return norm_.norm_desc_grp_session_ctrl(i).norm_trunc_size(); }
    uint16_t trunc_size_norm(int S, int i)    { return trunc_size_csr(S,i) + kNormTruncAddSize; /* +4 */}
    uint16_t trunc_size_coal(int S, int i)    { return trunc_size_csr(S,i) + kCoalTruncAddSize; /* +0 */}
    uint16_t trunc_size(int S, int i)         { return trunc_size_norm(S,i); }


    // COALESCING session CSRs - S is sliceID which is unused on Tofino
    bool     coal_ok(int S, int k)            { return ((k >= 0) && (k < kCoalSessions)); }
    bool     coal_en(int S, int k)            { return (coal_[k].ctrl0_.coal_ena() == 1); }
    // No ingress COALESCE on Tofino
    bool     coal_en(int S, int k, bool ing)  { return (ing) ?false :coal_en(S, k); }
    bool     coal_has_version(int S, int k)   { return true; }
    uint8_t  coal_ver(int S, int k)           { return coal_[k].ctrl0_.coal_vid(); }
    bool     coal_has_priority(int S, int k)  { return false; /* NO pri on Tofino */ }
    uint8_t  coal_pri(int S, int k)           { return 0; /* NO pri on Tofino */ }
    uint16_t coal_trunc_size(int S, int k)    { // Called but NOT used on Tofino
      int i = coal_id_to_sess_id(S, k); return (sess_ok(S,i)) ?trunc_size_coal(S,i) :0; }
    uint8_t  coal_pkthdr_len(int S, int k)    { return coal_[k].ctrl0_.coal_pkthdr_length(); }
    uint16_t coal_pkt_min_size(int S, int k)  { return coal_[k].ctrl0_.coal_minpkt_size(); }
    //uint16_t coal_abs_min_size(int S, int k)  // See virtual func below
    uint32_t coal_timeout(int S, int k)       {
      return static_cast<uint32_t>(coal_[k].ctrl0_.coal_timeout()); }
    bool     coal_tofino_mode(int S, int k)   { return GLOBAL_TRUE; }
    bool     coal_len_from_input(int S, int k){
      return (coal_[k].ctrl1_.coal_sflow_type() == 1); }
    uint16_t coal_extract_len(int S, int k)   { return coal_[k].ctrl1_.coal_extract_length(); }
    uint32_t coal_compiler_flag(int S, int k) { return (coal_pkthdr0(S,k) >> 16) & 0x00FF; }
    uint32_t coal_seqnum(int S, int k )       { return 0u; /* NO seqnum on Tofino */ }

    uint32_t coal_pkthdr0_HI(int S, int k)    { return (coal_pkthdr0(S,k) >> 16) & 0xFFFF; }
    uint32_t coal_pkthdr0_LO(int S, int k)    { return (coal_pkthdr0(S,k) >>  0) & 0xFFFF; }
    uint32_t coal_pkthdr0(int S, int k)       { return coal_[k].pkt_hdr0_.coal_pkt_hdr0(); }
    uint32_t coal_pkthdr1(int S, int k)       { return coal_[k].pkt_hdr1_.coal_pkt_hdr1(); }
    uint32_t coal_pkthdr2(int S, int k)       { return coal_[k].pkt_hdr2_.coal_pkt_hdr2(); }
    uint32_t coal_pkthdr3(int S, int k )      { return coal_[k].pkt_hdr3_.coal_pkt_hdr3(); }
    bool     coal_timer_enabled()             { return true; /* Always enabled on Tofino */ }
    uint32_t coal_basetime()                  { return coal_basetime_.coal_basetime(); }
    uint16_t coal_baseid()                    { return coal_baseid_.coal_sid(); }

    bool     coal_whole_pkt(uint16_t ext_len) { return GLOBAL_FALSE; /* NOT on Tofino */ }
    bool     coal_trunc_pkt(uint16_t ext_len) { return GLOBAL_FALSE; /* NOT on Tofino */ }
    uint32_t coal_slicehdr(int port, int len, bool start, bool end) { return static_cast<uint32_t>(GLOBAL_ZERO); }

    // GLOBAL CSRs
    bool     glob_ing_en()             { return (glb_ctrl_.ingr_ena() == 1); }
    bool     glob_egr_en()             { return (glb_ctrl_.egr_ena() == 1); }
    bool     glob_en(bool ing)         { return ing ?glob_ing_en() :glob_egr_en(); }
    bool     glob_coal_en()            { return (glb_ctrl_.coalescing_ena() == 1); }
    bool     glob_coal_ing_en()        { return false; }
    bool     glob_coal_egr_en()        { return true; }
    bool     glob_coal_gress(bool ing) { return ing ?glob_coal_ing_en() :glob_coal_egr_en(); }
    bool     glob_coal_en(bool ing)    { return glob_coal_en() && glob_coal_gress(ing); }
    bool     glob_mirr_if_not_coal(bool ing) {
      return glob_coal_en() && glob_en(ing) && !glob_coal_en(ing); }


    // CHIP-SPECIFIC CSRs
    virtual uint16_t coal_abs_min_size(int S, int k)  { return 0; }

 private:
    register_classes::MirBufDescNormDescGrpArray    norm_;
    std::array< MirrorCoalDescReg, kCoalSessions >  coal_;
    register_classes::MirBufRegsGlbCtrl             glb_ctrl_;
    register_classes::MirBufRegsNegMirrCtrl         neg_mir_ctrl_; // UNUSED
    register_classes::MirBufRegsCoalescingBaseid    coal_baseid_;
    register_classes::MirBufRegsCoalescingBasetime  coal_basetime_;
  };



  // MirrorSessionReg is just a shim straight through to MirrorReg on Tofino
  // as Tofino does not have atomic session config update.
  //
  // Given that certain fields span 2 metaX words a config access could see
  // a half-written register field! Mostly a non-issue as the 2-word spanning
  // fields normally have a valid bit, than can be switched off before update
  // and on when update complete. But could be an issue for hash1 which does
  // not!
  //
  class MirrorSessionReg {

 public:
    MirrorSessionReg(MirrorRegTofinoXX *mirrorReg, int pipeIndex, int index)
        : mir_(mirrorReg), i_(index) {
    }
    virtual ~MirrorSessionReg() { }

    int      index()      const { return i_; }
    void     set_index(int i)   { i_ = i; }

    uint8_t  hash_cfg()         { return mir_->hash_cfg(i_); }
    uint8_t  icos_cfg()         { return mir_->icos_cfg(i_); }
    uint8_t  dod_cfg()          { return mir_->dod_cfg(i_); }
    uint8_t  c2c_cfg()          { return mir_->c2c_cfg(i_); }
    uint8_t  mc_cfg()           { return mir_->mc_cfg(i_); }
    uint8_t  epipe_cfg()        { return mir_->epipe_cfg(i_); }
    uint16_t yid()              { return mir_->yid(i_); }
    uint16_t xid()              { return mir_->xid(i_); }

    uint16_t mcid1_id()         { return mir_->mcid1_id(i_); }
    uint16_t mcid2_id()         { return mir_->mcid2_id(i_); }

    uint16_t epipe_port()       { return mir_->epipe_port(i_); }
    uint8_t  epipe_port_vld()   { return mir_->epipe_port_vld(i_); }
    uint8_t  def_on_drop()      { return mir_->def_on_drop(i_); }
    uint16_t rid()              { return mir_->rid(i_); }

    uint16_t hash1()            { return mir_->hash1(i_); }
    uint16_t hash2()            { return mir_->hash2(i_); }
    uint8_t  pipe_vec()         { return mir_->pipe_vec(i_); }
    uint8_t  tm_vec()           { return 0; }  // N/A for jbay
    uint8_t  egress_bypass()    { return mir_->egress_bypass(i_); }

    uint8_t  icos()             { return mir_->icos(i_); }
    uint8_t  color()            { return mir_->color(i_); }
    uint8_t  mcid1_vld()        { return mir_->mcid1_vld(i_); }
    uint8_t  mcid2_vld()        { return mir_->mcid2_vld(i_); }
    uint8_t  c2c_cos()          { return mir_->c2c_cos(i_); }
    uint8_t  c2c_vld()          { return mir_->c2c_vld(i_); }
    uint8_t  yid_tbl_sel()      { return mir_->yid_tbl_sel(i_); }
    uint8_t  eport_qid()        { return mir_->eport_qid(i_); }

    bool uses_def_on_drop()     { return mir_->uses_def_on_drop(i_); }
    bool uses_egress_bypass()   { return mir_->uses_egress_bypass(i_); }
    bool uses_yid_tbl_sel()     { return mir_->uses_yid_tbl_sel(i_); }
    bool is_epipe_port_vld()    { return (epipe_port_vld() == 1); }
    bool is_mcid1_vld()         { return (mcid1_vld() == 1); }
    bool is_mcid2_vld()         { return (mcid2_vld() == 1); }
    bool is_c2c_vld()           { return (c2c_vld() == 1); }


 private:
    MirrorRegTofinoXX  *mir_;
    int                 i_;
  };


}
#endif // _TOFINOXX_MIRROR_REG_TOFINOXX_
